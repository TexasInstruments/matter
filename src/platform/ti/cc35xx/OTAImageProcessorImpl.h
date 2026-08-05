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

#pragma once

#include <app/clusters/ota-requestor/OTADownloader.h>
#include <lib/core/OTAImageHeader.h>
#include <platform/CHIPDeviceLayer.h>
#include <platform/OTAImageProcessor.h>

#include "psa_fwu.h"

namespace chip {

class OTAImageProcessorImpl : public OTAImageProcessorInterface
{
public:
    //////////// OTAImageProcessorInterface Implementation ///////////////
    CHIP_ERROR PrepareDownload() override;
    CHIP_ERROR Finalize() override;
    CHIP_ERROR Apply() override;
    CHIP_ERROR Abort() override;
    CHIP_ERROR ProcessBlock(ByteSpan & block) override;
    bool IsFirstImageRun() override;
    CHIP_ERROR ConfirmCurrentImage() override;

    void SetOTADownloader(OTADownloader * downloader) { mDownloader = downloader; }

private:
    //////////// Actual handlers for the OTAImageProcessorInterface ///////////////
    static void HandlePrepareDownload(intptr_t context);
    static void HandleFinalize(intptr_t context);
    static void HandleApply(intptr_t context);
    static void HandleAbort(intptr_t context);
    static void HandleProcessBlock(intptr_t context);

    CHIP_ERROR SetBlock(ByteSpan & block);
    CHIP_ERROR ReleaseBlock();

    // PSA FWU state — valid for the duration of one OTA session
    psa_fwu_component_t mStagingSlot          = Vendor_Image_Slot_1;
    PSA_FWU_GPESlot_t   mManifestBuffer        = {};
    size_t              mManifestBytesReceived  = 0;
    size_t              mImageOffset            = 0; // offset for psa_fwu_write; starts at TI_FWU_MANIFEST_SIZE
    bool                mFwuStartCalled         = false;
    uint32_t            mLastProcessedImageOffset = 0; // track last successful offset to detect retransmissions

    // Matter OTA header parser — consumed once per session
    OTAImageHeaderParser mHeaderParser;

    MutableByteSpan mBlock;
    OTADownloader * mDownloader = nullptr;
};

} // namespace chip
