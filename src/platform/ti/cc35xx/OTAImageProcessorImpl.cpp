/*
 *
 *    Copyright (c) 2026 Project CHIP Authors
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include <app/clusters/ota-requestor/OTADownloader.h>
#include <app/clusters/ota-requestor/OTARequestorInterface.h>

#include "OTAImageProcessorImpl.h"

#include <algorithm>

extern "C" void cc35xxLog(const char * msg, ...);

using namespace chip::DeviceLayer;

namespace chip {

// PSA FWU request_type values (from psa_fwu.c)
#define PSA_FWU_REQUEST_NO_OTA 0x0
#define PSA_FWU_REQUEST_OTA_COMMIT 0x2
#define PSA_FWU_REQUEST_OTA_COMMIT_AND_PROTECT 0x3

// OTA flow: Download → Reboot 1 (test new image) → Accept → Reboot 2 (image permanent)
// This macro checks if device is in Reboot 2 (post-acceptance), when NotifyUpdateApplied should be sent
#define IS_BOOT_POST_ACCEPT(info) \
    ((info).impl.Primary && (info).state == PSA_FWU_UPDATED && \
     ((info).impl.request_type == PSA_FWU_REQUEST_OTA_COMMIT || \
      (info).impl.request_type == PSA_FWU_REQUEST_OTA_COMMIT_AND_PROTECT))

// ── Public interface — schedule work on the CHIP task ────────────────────────

CHIP_ERROR OTAImageProcessorImpl::PrepareDownload()
{
    PlatformMgr().ScheduleWork(HandlePrepareDownload, reinterpret_cast<intptr_t>(this));
    return CHIP_NO_ERROR;
}

CHIP_ERROR OTAImageProcessorImpl::Finalize()
{
    PlatformMgr().ScheduleWork(HandleFinalize, reinterpret_cast<intptr_t>(this));
    return CHIP_NO_ERROR;
}

CHIP_ERROR OTAImageProcessorImpl::Apply()
{
    PlatformMgr().ScheduleWork(HandleApply, reinterpret_cast<intptr_t>(this));
    return CHIP_NO_ERROR;
}

CHIP_ERROR OTAImageProcessorImpl::Abort()
{
    PlatformMgr().ScheduleWork(HandleAbort, reinterpret_cast<intptr_t>(this));
    return CHIP_NO_ERROR;
}

CHIP_ERROR OTAImageProcessorImpl::ProcessBlock(ByteSpan & block)
{
    if ((block.data() == nullptr) || block.empty())
    {
        return CHIP_ERROR_INVALID_ARGUMENT;
    }

    CHIP_ERROR err = SetBlock(block);
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(SoftwareUpdate, "Cannot set block data: %" CHIP_ERROR_FORMAT, err.Format());
        return err;
    }

    PlatformMgr().ScheduleWork(HandleProcessBlock, reinterpret_cast<intptr_t>(this));
    return CHIP_NO_ERROR;
}

bool OTAImageProcessorImpl::IsFirstImageRun()
{
    psa_fwu_component_info_t info1, info2;
    psa_status_t s1 = psa_fwu_query(Vendor_Image_Slot_1, &info1);
    psa_status_t s2 = psa_fwu_query(Vendor_Image_Slot_2, &info2);

    // Boot 1: Any slot in TRIAL state needs acceptance via psa_fwu_accept()
    if ((s1 == PSA_SUCCESS && info1.state == PSA_FWU_TRIAL) ||
        (s2 == PSA_SUCCESS && info2.state == PSA_FWU_TRIAL))
    {
        return true;
    }

    // Boot 2 only: OTA commit just completed, send NotifyUpdateApplied
    // Boot 3+: request_type is NO_OTA, so this check fails and ConfirmCurrentImage is skipped
    if ((s1 == PSA_SUCCESS && IS_BOOT_POST_ACCEPT(info1)) ||
        (s2 == PSA_SUCCESS && IS_BOOT_POST_ACCEPT(info2)))
    {
        return true;
    }

    return false;
}

CHIP_ERROR OTAImageProcessorImpl::ConfirmCurrentImage()
{
    OTARequestorInterface * requestor = chip::GetRequestorInstance();
    if (requestor == nullptr)
    {
        return CHIP_ERROR_INTERNAL;
    }

    uint32_t currentVersion;
    uint32_t targetVersion = requestor->GetTargetVersion();
    ReturnErrorOnFailure(DeviceLayer::ConfigurationMgr().GetSoftwareVersion(currentVersion));

    if (currentVersion != targetVersion)
    {
        ChipLogError(SoftwareUpdate, "Version mismatch: running %" PRIu32 ", expected %" PRIu32, currentVersion, targetVersion);
        // Roll back — bootloader will revert to the previous image on next boot
        psa_fwu_reject(PSA_ERROR_INVALID_ARGUMENT);
        psa_fwu_request_reboot();
        return CHIP_ERROR_INCORRECT_STATE;
    }

    // Query both slots to find OTA state
    psa_fwu_component_info_t info1, info2;
    psa_status_t s1 = psa_fwu_query(Vendor_Image_Slot_1, &info1);
    psa_status_t s2 = psa_fwu_query(Vendor_Image_Slot_2, &info2);

    // Boot 1: Check if ANY slot is in TRIAL state (psa_fwu_accept handles all TRIAL slots)
    if ((s1 == PSA_SUCCESS && info1.state == PSA_FWU_TRIAL) ||
        (s2 == PSA_SUCCESS && info2.state == PSA_FWU_TRIAL))
    {
        // Image running in TRIAL (ACTIVE+NOT_PRIMARY), accept and request commit reboot
        psa_status_t status = psa_fwu_accept();
        if (status != PSA_SUCCESS && status != PSA_SUCCESS_REBOOT)
        {
            ChipLogError(SoftwareUpdate, "psa_fwu_accept failed: %" PRId32, (int32_t) status);
            return CHIP_ERROR_INTERNAL;
        }
        ChipLogProgress(SoftwareUpdate, "OTA commit OK, requesting Reboot 2 (commit boot)");
        psa_fwu_request_reboot();
    }
    // Boot 2: No TRIAL slot, check if OTA commit just completed
    else if ((s1 == PSA_SUCCESS && IS_BOOT_POST_ACCEPT(info1)) ||
             (s2 == PSA_SUCCESS && IS_BOOT_POST_ACCEPT(info2)))
    {
        // Image committed (UPDATED+PRIMARY), NotifyUpdateApplied will execute after this
        ChipLogProgress(SoftwareUpdate, "OTA image already confirmed (UPDATED state), ready to send NotifyUpdateApplied");
    }
    else
    {
        ChipLogError(SoftwareUpdate, "No slot in TRIAL or post-accept boot state");
        return CHIP_ERROR_INTERNAL;
    }

    return CHIP_NO_ERROR;
}

// ── Slot preparation (ported from Simplelink Wi-Fi SDK ota_fwu.c::OTA_FWU_prepareSlot) ────────

static CHIP_ERROR PrepareSlot(psa_fwu_component_t slot)
{
    psa_fwu_component_info_t info;
    psa_status_t status;

    status = psa_fwu_query(slot, &info);
    if (status != PSA_SUCCESS)
    {
        ChipLogError(SoftwareUpdate, "psa_fwu_query(%d) failed: %" PRId32, (int) slot, (int32_t) status);
        return CHIP_ERROR_INTERNAL;
    }

    ChipLogProgress(SoftwareUpdate, "Staging slot %d state=%d primary=%d version=%d.%d.%d", (int) slot, (int) info.state,
                    (int) info.impl.Primary, (int) info.version.major, (int) info.version.minor, (int) info.version.patch);

    switch (info.state)
    {
    case PSA_FWU_READY:
        break;

    case PSA_FWU_WRITING:
    case PSA_FWU_CANDIDATE:
        status = psa_fwu_cancel(slot);
        if (status != PSA_SUCCESS)
        {
            ChipLogError(SoftwareUpdate, "psa_fwu_cancel(%d) failed: %" PRId32, (int) slot, (int32_t) status);
            return CHIP_ERROR_INTERNAL;
        }
        status = psa_fwu_clean(slot);
        if (status != PSA_SUCCESS)
        {
            ChipLogError(SoftwareUpdate, "psa_fwu_clean(%d) failed: %" PRId32, (int) slot, (int32_t) status);
            return CHIP_ERROR_INTERNAL;
        }
        break;

    case PSA_FWU_STAGED:
    case PSA_FWU_TRIAL:
        // psa_fwu_reject acts on all STAGED/TRIAL components — safe in Phase 1 (single component)
        status = psa_fwu_reject(PSA_ERROR_NOT_PERMITTED);
        if (status != PSA_SUCCESS && status != PSA_SUCCESS_REBOOT)
        {
            ChipLogError(SoftwareUpdate, "psa_fwu_reject failed: %" PRId32, (int32_t) status);
            return CHIP_ERROR_INTERNAL;
        }
        status = psa_fwu_clean(slot);
        if (status != PSA_SUCCESS)
        {
            ChipLogError(SoftwareUpdate, "psa_fwu_clean(%d) failed: %" PRId32, (int) slot, (int32_t) status);
            return CHIP_ERROR_INTERNAL;
        }
        break;

    case PSA_FWU_FAILED:
    case PSA_FWU_UPDATED:
    case PSA_FWU_REJECTED:
        status = psa_fwu_clean(slot);
        if (status != PSA_SUCCESS)
        {
            ChipLogError(SoftwareUpdate, "psa_fwu_clean(%d) failed: %" PRId32, (int) slot, (int32_t) status);
            return CHIP_ERROR_INTERNAL;
        }
        break;

    default:
        ChipLogError(SoftwareUpdate, "Slot %d in unexpected state %d", (int) slot, (int) info.state);
        return CHIP_ERROR_INTERNAL;
    }

    return CHIP_NO_ERROR;
}

// ── Handlers (run on CHIP task) ───────────────────────────────────────────────

void OTAImageProcessorImpl::HandlePrepareDownload(intptr_t context)
{
    auto * imageProcessor = reinterpret_cast<OTAImageProcessorImpl *>(context);
    if (imageProcessor == nullptr)
    {
        ChipLogError(SoftwareUpdate, "ImageProcessor context is null");
        return;
    }
    if (imageProcessor->mDownloader == nullptr)
    {
        ChipLogError(SoftwareUpdate, "mDownloader is null");
        return;
    }

    // Select the non-primary (staging) slot
    psa_fwu_component_info_t info1, info2;
    psa_status_t s1 = psa_fwu_query(Vendor_Image_Slot_1, &info1);
    psa_status_t s2 = psa_fwu_query(Vendor_Image_Slot_2, &info2);

    if (s1 != PSA_SUCCESS || s2 != PSA_SUCCESS)
    {
        ChipLogError(SoftwareUpdate, "Failed to query Vendor Image slots");
        imageProcessor->mDownloader->OnPreparedForDownload(CHIP_ERROR_INTERNAL);
        return;
    }

    if (!info1.impl.Primary && info2.impl.Primary)
    {
        imageProcessor->mStagingSlot = Vendor_Image_Slot_1;
    }
    else if (info1.impl.Primary && !info2.impl.Primary)
    {
        imageProcessor->mStagingSlot = Vendor_Image_Slot_2;
    }
    else
    {
        ChipLogError(SoftwareUpdate, "Cannot determine staging slot (primary flags: %d %d)", (int) info1.impl.Primary,
                     (int) info2.impl.Primary);
        imageProcessor->mDownloader->OnPreparedForDownload(CHIP_ERROR_INTERNAL);
        return;
    }

    ChipLogProgress(SoftwareUpdate, "OTA staging slot: %d", (int) imageProcessor->mStagingSlot);

    CHIP_ERROR err = PrepareSlot(imageProcessor->mStagingSlot);
    if (err != CHIP_NO_ERROR)
    {
        imageProcessor->mDownloader->OnPreparedForDownload(err);
        return;
    }

    // Reset per-session state
    imageProcessor->mManifestBytesReceived = 0;
    imageProcessor->mManifestBuffer        = {};
    imageProcessor->mFwuStartCalled        = false;
    imageProcessor->mImageOffset           = TI_FWU_MANIFEST_SIZE;
    imageProcessor->mLastProcessedImageOffset = 0;
    imageProcessor->mParams.downloadedBytes = 0;
    imageProcessor->mHeaderParser.Init();

    imageProcessor->mDownloader->OnPreparedForDownload(CHIP_NO_ERROR);
}

void OTAImageProcessorImpl::HandleProcessBlock(intptr_t context)
{
    auto * imageProcessor = reinterpret_cast<OTAImageProcessorImpl *>(context);
    if (imageProcessor == nullptr)
    {
        ChipLogError(SoftwareUpdate, "ImageProcessor context is null");
        return;
    }
    if (imageProcessor->mDownloader == nullptr)
    {
        ChipLogError(SoftwareUpdate, "mDownloader is null");
        return;
    }

    ByteSpan block(imageProcessor->mBlock.data(), imageProcessor->mBlock.size());

    // ── Step 1: consume Matter OTA header ────────────────────────────────────
    if (imageProcessor->mHeaderParser.IsInitialized())
    {
        OTAImageHeader header;
        CHIP_ERROR err = imageProcessor->mHeaderParser.AccumulateAndDecode(block, header);

        if (err == CHIP_ERROR_BUFFER_TOO_SMALL)
        {
            // Header not yet complete — entire block was consumed by the parser
            imageProcessor->mParams.downloadedBytes += imageProcessor->mBlock.size();
            imageProcessor->mDownloader->FetchNextData();
            return;
        }
        if (err != CHIP_NO_ERROR)
        {
            ChipLogError(SoftwareUpdate, "OTA header parse error: %" CHIP_ERROR_FORMAT, err.Format());
            imageProcessor->mDownloader->EndDownload(err);
            return;
        }

        // Header fully parsed — remaining bytes in block are payload
        imageProcessor->mParams.totalFileBytes = header.mPayloadSize;
        imageProcessor->mHeaderParser.Clear();
    }

    // ── Step 2: buffer PSA FWU manifest (first TI_FWU_MANIFEST_SIZE payload bytes) ──
    const uint8_t * payloadData = block.data();
    size_t          payloadSize = block.size();

    if (imageProcessor->mManifestBytesReceived < TI_FWU_MANIFEST_SIZE)
    {
        size_t needed = TI_FWU_MANIFEST_SIZE - imageProcessor->mManifestBytesReceived;
        size_t toCopy = std::min(payloadSize, needed);

        memcpy(reinterpret_cast<uint8_t *>(&imageProcessor->mManifestBuffer) + imageProcessor->mManifestBytesReceived,
               payloadData, toCopy);

        imageProcessor->mManifestBytesReceived += toCopy;
        payloadData += toCopy;
        payloadSize -= toCopy;
    }

    // ── Step 3: call psa_fwu_start once manifest is complete ─────────────────
    if (imageProcessor->mManifestBytesReceived == TI_FWU_MANIFEST_SIZE && !imageProcessor->mFwuStartCalled)
    {
        psa_status_t status =
            psa_fwu_start(imageProcessor->mStagingSlot, &imageProcessor->mManifestBuffer, TI_FWU_MANIFEST_SIZE);
        if (status != PSA_SUCCESS)
        {
            ChipLogError(SoftwareUpdate, "psa_fwu_start failed: %" PRId32, (int32_t) status);
            imageProcessor->mDownloader->EndDownload(CHIP_ERROR_WRITE_FAILED);
            return;
        }
        imageProcessor->mFwuStartCalled = true;
        ChipLogProgress(SoftwareUpdate, "psa_fwu_start OK, image data offset starts at %u",
                        (unsigned) imageProcessor->mImageOffset);
    }

    // ── Step 4: stream image data via psa_fwu_write ───────────────────────────
    if (payloadSize > 0 && imageProcessor->mFwuStartCalled)
    {
        psa_status_t status =
            psa_fwu_write(imageProcessor->mStagingSlot, imageProcessor->mImageOffset, payloadData, payloadSize);
        if (status != PSA_SUCCESS)
        {
            ChipLogError(SoftwareUpdate, "psa_fwu_write failed at offset %u: %" PRId32,
                         (unsigned) imageProcessor->mImageOffset, (int32_t) status);
            imageProcessor->mDownloader->EndDownload(CHIP_ERROR_WRITE_FAILED);
            return;
        }
        imageProcessor->mImageOffset += payloadSize;
    }

    // Only count bytes if we advanced past what we've already processed
    // This prevents counting the same bytes twice if a BDX retransmission occurs
    if (imageProcessor->mImageOffset > imageProcessor->mLastProcessedImageOffset)
    {
        uint32_t newBytes = imageProcessor->mImageOffset - imageProcessor->mLastProcessedImageOffset;
        imageProcessor->mParams.downloadedBytes += newBytes;
        imageProcessor->mLastProcessedImageOffset = imageProcessor->mImageOffset;
    }

    cc35xxLog("Downloaded %lu / %lu bytes", (unsigned long) imageProcessor->mParams.downloadedBytes,
             (unsigned long) imageProcessor->mParams.totalFileBytes);
    imageProcessor->mDownloader->FetchNextData();
}

void OTAImageProcessorImpl::HandleFinalize(intptr_t context)
{
    auto * imageProcessor = reinterpret_cast<OTAImageProcessorImpl *>(context);
    if (imageProcessor == nullptr)
    {
        return;
    }

    psa_status_t status = psa_fwu_finish(imageProcessor->mStagingSlot);
    if (status != PSA_SUCCESS)
    {
        ChipLogError(SoftwareUpdate, "psa_fwu_finish failed: %" PRId32, (int32_t) status);
    }

    imageProcessor->ReleaseBlock();
    cc35xxLog("OTA image download complete (%lu bytes)", (unsigned long) imageProcessor->mParams.downloadedBytes);
}

void OTAImageProcessorImpl::HandleApply(intptr_t context)
{
    auto * imageProcessor = reinterpret_cast<OTAImageProcessorImpl *>(context);
    if (imageProcessor == nullptr)
    {
        return;
    }

    // Install all CANDIDATE components — in Phase 1 this is only the Vendor Image
    psa_status_t status = psa_fwu_install();
    if (status != PSA_SUCCESS && status != PSA_SUCCESS_REBOOT)
    {
        ChipLogError(SoftwareUpdate, "psa_fwu_install failed: %" PRId32, (int32_t) status);
        return;
    }

    ChipLogProgress(SoftwareUpdate, "OTA install OK, requesting Reboot 1 (trial boot)");
    psa_fwu_request_reboot();
}

void OTAImageProcessorImpl::HandleAbort(intptr_t context)
{
    auto * imageProcessor = reinterpret_cast<OTAImageProcessorImpl *>(context);
    if (imageProcessor == nullptr)
    {
        return;
    }

    psa_fwu_component_info_t info;
    psa_fwu_component_t slot = imageProcessor->mStagingSlot;
    psa_status_t status;

    if (psa_fwu_query(slot, &info) == PSA_SUCCESS)
    {
        switch (info.state)
        {
        case PSA_FWU_WRITING:
        case PSA_FWU_CANDIDATE:
            psa_fwu_cancel(slot);
            psa_fwu_clean(slot);
            break;

        case PSA_FWU_STAGED:
            status = psa_fwu_reject(PSA_ERROR_SERVICE_FAILURE);
            if (status == PSA_SUCCESS || status == PSA_SUCCESS_REBOOT)
            {
                psa_fwu_clean(slot);
            }
            break;

        case PSA_FWU_FAILED:
        case PSA_FWU_UPDATED:
        case PSA_FWU_REJECTED:
            psa_fwu_clean(slot);
            break;

        default:
            break;
        }
    }

    imageProcessor->ReleaseBlock();
    ChipLogProgress(SoftwareUpdate, "OTA aborted, staging slot %d cleaned up", (int) slot);
}

// ── Block buffer helpers ──────────────────────────────────────────────────────

CHIP_ERROR OTAImageProcessorImpl::SetBlock(ByteSpan & block)
{
    if (!IsSpanUsable(block))
    {
        ReleaseBlock();
        return CHIP_NO_ERROR;
    }
    if (mBlock.size() < block.size())
    {
        if (!mBlock.empty())
        {
            ReleaseBlock();
        }
        uint8_t * buf = static_cast<uint8_t *>(chip::Platform::MemoryAlloc(block.size()));
        if (buf == nullptr)
        {
            return CHIP_ERROR_NO_MEMORY;
        }
        mBlock = MutableByteSpan(buf, block.size());
    }
    CHIP_ERROR err = CopySpanToMutableSpan(block, mBlock);
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(SoftwareUpdate, "Cannot copy block data: %" CHIP_ERROR_FORMAT, err.Format());
        return err;
    }
    return CHIP_NO_ERROR;
}

CHIP_ERROR OTAImageProcessorImpl::ReleaseBlock()
{
    if (mBlock.data() != nullptr)
    {
        chip::Platform::MemoryFree(mBlock.data());
    }
    mBlock = MutableByteSpan();
    return CHIP_NO_ERROR;
}

} // namespace chip
