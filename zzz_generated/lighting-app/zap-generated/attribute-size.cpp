/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
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

// THIS FILE IS GENERATED BY ZAP

#include <app-common/zap-generated/af-structs.h>
#include <app/util/af.h>
#include <app/util/attribute-list-byte-span.h>
#include <app/util/basic-types.h>
#include <support/SafeInt.h>
#include <support/logging/CHIPLogging.h>

using namespace chip;
using namespace chip::app::List;

// The first 2 bytes specify the number of entries. A value of 0xFFFF means the list in invalid
// and data is undefined.
constexpr uint16_t kSizeLengthInBytes = 2u;

void copyListMember(uint8_t * dest, uint8_t * src, bool write, uint16_t * offset, uint16_t length)
{
    if (write)
    {
        memmove(dest + *offset, src, length);
    }
    else
    {
        memmove(dest, src + *offset, length);
    }

    *offset = static_cast<uint16_t>(*offset + length);
}

uint16_t emberAfCopyList(ClusterId clusterId, EmberAfAttributeMetadata * am, bool write, uint8_t * dest, uint8_t * src,
                         int32_t index)
{
    if (index == -1)
    {
        memmove(dest, src, am->size);
        return am->size;
    }

    if (index == 0)
    {
        if (write)
        {
            // src is a pointer to native-endian uint16_t, dest is pointer to buffer that should hold little-endian value
            emberAfCopyInt16u(dest, 0, *reinterpret_cast<uint16_t *>(src));
        }
        else
        {
            // src is pointer to buffer holding little-endian value, dest is a pointer to native-endian uint16_t
            *reinterpret_cast<uint16_t *>(dest) = emberAfGetInt16u(src, 0, kSizeLengthInBytes);
        }
        return kSizeLengthInBytes;
    }

    if (!CanCastTo<uint16_t>(index))
    {
        ChipLogError(Zcl, "Index %" PRId32 " is invalid. Should be between 1 and 65534", index);
        return 0;
    }

    uint16_t entryLength = 0;
    switch (clusterId)
    {
    case 0x0030: // General Commissioning Cluster
    {
        uint16_t entryOffset = kSizeLengthInBytes;
        switch (am->attributeId)
        {
        case 0x0001: // BasicCommissioningInfoList
        {
            entryLength = 4;
            if (((index - 1) * entryLength) > (am->size - entryLength))
            {
                ChipLogError(Zcl, "Index %" PRId32 " is invalid.", index);
                return 0;
            }
            entryOffset = static_cast<uint16_t>(entryOffset + ((index - 1) * entryLength));
            // Struct _BasicCommissioningInfoType
            _BasicCommissioningInfoType * entry = reinterpret_cast<_BasicCommissioningInfoType *>(write ? src : dest);
            copyListMember(write ? dest : (uint8_t *) &entry->FailSafeExpiryLengthMs,
                           write ? (uint8_t *) &entry->FailSafeExpiryLengthMs : src, write, &entryOffset,
                           sizeof(entry->FailSafeExpiryLengthMs)); // INT32U
            break;
        }
        }
        break;
    }
    case 0x0033: // General Diagnostics Cluster
    {
        uint16_t entryOffset = kSizeLengthInBytes;
        switch (am->attributeId)
        {
        case 0x0000: // NetworkInterfaces
        {
            entryLength = 48;
            if (((index - 1) * entryLength) > (am->size - entryLength))
            {
                ChipLogError(Zcl, "Index %" PRId32 " is invalid.", index);
                return 0;
            }
            entryOffset = static_cast<uint16_t>(entryOffset + ((index - 1) * entryLength));
            // Struct _NetworkInterfaceType
            _NetworkInterfaceType * entry = reinterpret_cast<_NetworkInterfaceType *>(write ? src : dest);
            ByteSpan * NameSpan           = &entry->Name; // OCTET_STRING
            if (CHIP_NO_ERROR !=
                (write ? WriteByteSpan(dest + entryOffset, 34, NameSpan) : ReadByteSpan(src + entryOffset, 34, NameSpan)))
            {
                ChipLogError(Zcl, "Index %" PRId32 " is invalid. Not enough remaining space", index);
                return 0;
            }
            entryOffset = static_cast<uint16_t>(entryOffset + 34);
            copyListMember(write ? dest : (uint8_t *) &entry->FabricConnected, write ? (uint8_t *) &entry->FabricConnected : src,
                           write, &entryOffset, sizeof(entry->FabricConnected)); // BOOLEAN
            copyListMember(write ? dest : (uint8_t *) &entry->OffPremiseServicesReachableIPv4,
                           write ? (uint8_t *) &entry->OffPremiseServicesReachableIPv4 : src, write, &entryOffset,
                           sizeof(entry->OffPremiseServicesReachableIPv4)); // BOOLEAN
            copyListMember(write ? dest : (uint8_t *) &entry->OffPremiseServicesReachableIPv6,
                           write ? (uint8_t *) &entry->OffPremiseServicesReachableIPv6 : src, write, &entryOffset,
                           sizeof(entry->OffPremiseServicesReachableIPv6)); // BOOLEAN
            ByteSpan * HardwareAddressSpan = &entry->HardwareAddress;       // OCTET_STRING
            if (CHIP_NO_ERROR !=
                (write ? WriteByteSpan(dest + entryOffset, 10, HardwareAddressSpan)
                       : ReadByteSpan(src + entryOffset, 10, HardwareAddressSpan)))
            {
                ChipLogError(Zcl, "Index %" PRId32 " is invalid. Not enough remaining space", index);
                return 0;
            }
            entryOffset = static_cast<uint16_t>(entryOffset + 10);
            copyListMember(write ? dest : (uint8_t *) &entry->Type, write ? (uint8_t *) &entry->Type : src, write, &entryOffset,
                           sizeof(entry->Type)); // ENUM8
            break;
        }
        }
        break;
    }
    case 0x003E: // Operational Credentials Cluster
    {
        uint16_t entryOffset = kSizeLengthInBytes;
        switch (am->attributeId)
        {
        case 0x0001: // fabrics list
        {
            entryLength = 120;
            if (((index - 1) * entryLength) > (am->size - entryLength))
            {
                ChipLogError(Zcl, "Index %" PRId32 " is invalid.", index);
                return 0;
            }
            entryOffset = static_cast<uint16_t>(entryOffset + ((index - 1) * entryLength));
            // Struct _FabricDescriptor
            _FabricDescriptor * entry = reinterpret_cast<_FabricDescriptor *>(write ? src : dest);
            copyListMember(write ? dest : (uint8_t *) &entry->FabricIndex, write ? (uint8_t *) &entry->FabricIndex : src, write,
                           &entryOffset, sizeof(entry->FabricIndex)); // INT8U
            ByteSpan * RootPublicKeySpan = &entry->RootPublicKey;     // OCTET_STRING
            if (CHIP_NO_ERROR !=
                (write ? WriteByteSpan(dest + entryOffset, 67, RootPublicKeySpan)
                       : ReadByteSpan(src + entryOffset, 67, RootPublicKeySpan)))
            {
                ChipLogError(Zcl, "Index %" PRId32 " is invalid. Not enough remaining space", index);
                return 0;
            }
            entryOffset = static_cast<uint16_t>(entryOffset + 67);
            copyListMember(write ? dest : (uint8_t *) &entry->VendorId, write ? (uint8_t *) &entry->VendorId : src, write,
                           &entryOffset, sizeof(entry->VendorId)); // INT16U
            copyListMember(write ? dest : (uint8_t *) &entry->FabricId, write ? (uint8_t *) &entry->FabricId : src, write,
                           &entryOffset, sizeof(entry->FabricId)); // FABRIC_ID
            copyListMember(write ? dest : (uint8_t *) &entry->NodeId, write ? (uint8_t *) &entry->NodeId : src, write, &entryOffset,
                           sizeof(entry->NodeId)); // NODE_ID
            ByteSpan * LabelSpan = &entry->Label;  // OCTET_STRING
            if (CHIP_NO_ERROR !=
                (write ? WriteByteSpan(dest + entryOffset, 34, LabelSpan) : ReadByteSpan(src + entryOffset, 34, LabelSpan)))
            {
                ChipLogError(Zcl, "Index %" PRId32 " is invalid. Not enough remaining space", index);
                return 0;
            }
            entryOffset = static_cast<uint16_t>(entryOffset + 34);
            break;
        }
        case 0x0004: // TrustedRootCertificates
        {
            entryOffset = GetByteSpanOffsetFromIndex(write ? dest : src, am->size, static_cast<uint16_t>(index - 1));
            if (entryOffset == 0)
            {
                ChipLogError(Zcl, "Index %" PRId32 " is invalid.", index);
                return 0;
            }

            ByteSpan * trustedRootCertificatesSpan         = reinterpret_cast<ByteSpan *>(write ? src : dest); // OCTET_STRING
            uint16_t trustedRootCertificatesRemainingSpace = static_cast<uint16_t>(am->size - entryOffset);
            if (CHIP_NO_ERROR !=
                (write ? WriteByteSpan(dest + entryOffset, trustedRootCertificatesRemainingSpace, trustedRootCertificatesSpan)
                       : ReadByteSpan(src + entryOffset, trustedRootCertificatesRemainingSpace, trustedRootCertificatesSpan)))
            {
                ChipLogError(Zcl, "Index %" PRId32 " is invalid. Not enough remaining space", index);
                return 0;
            }

            if (!CanCastTo<uint16_t>(trustedRootCertificatesSpan->size()))
            {
                ChipLogError(Zcl, "Span size %zu is too large", trustedRootCertificatesSpan->size());
                return 0;
            }
            entryLength = static_cast<uint16_t>(trustedRootCertificatesSpan->size());
            break;
        }
        }
        break;
    }
    case 0x0035: // Thread Network Diagnostics Cluster
    {
        uint16_t entryOffset = kSizeLengthInBytes;
        switch (am->attributeId)
        {
        case 0x0007: // NeighborTableList
        {
            entryLength = 31;
            if (((index - 1) * entryLength) > (am->size - entryLength))
            {
                ChipLogError(Zcl, "Index %" PRId32 " is invalid.", index);
                return 0;
            }
            entryOffset = static_cast<uint16_t>(entryOffset + ((index - 1) * entryLength));
            // Struct _NeighborTable
            _NeighborTable * entry = reinterpret_cast<_NeighborTable *>(write ? src : dest);
            copyListMember(write ? dest : (uint8_t *) &entry->ExtAddress, write ? (uint8_t *) &entry->ExtAddress : src, write,
                           &entryOffset, sizeof(entry->ExtAddress)); // INT64U
            copyListMember(write ? dest : (uint8_t *) &entry->Age, write ? (uint8_t *) &entry->Age : src, write, &entryOffset,
                           sizeof(entry->Age)); // INT32U
            copyListMember(write ? dest : (uint8_t *) &entry->Rloc16, write ? (uint8_t *) &entry->Rloc16 : src, write, &entryOffset,
                           sizeof(entry->Rloc16)); // INT16U
            copyListMember(write ? dest : (uint8_t *) &entry->LinkFrameCounter, write ? (uint8_t *) &entry->LinkFrameCounter : src,
                           write, &entryOffset, sizeof(entry->LinkFrameCounter)); // INT32U
            copyListMember(write ? dest : (uint8_t *) &entry->MleFrameCounter, write ? (uint8_t *) &entry->MleFrameCounter : src,
                           write, &entryOffset, sizeof(entry->MleFrameCounter)); // INT32U
            copyListMember(write ? dest : (uint8_t *) &entry->LQI, write ? (uint8_t *) &entry->LQI : src, write, &entryOffset,
                           sizeof(entry->LQI)); // INT8U
            copyListMember(write ? dest : (uint8_t *) &entry->AverageRssi, write ? (uint8_t *) &entry->AverageRssi : src, write,
                           &entryOffset, sizeof(entry->AverageRssi)); // INT8S
            copyListMember(write ? dest : (uint8_t *) &entry->LastRssi, write ? (uint8_t *) &entry->LastRssi : src, write,
                           &entryOffset, sizeof(entry->LastRssi)); // INT8S
            copyListMember(write ? dest : (uint8_t *) &entry->FrameErrorRate, write ? (uint8_t *) &entry->FrameErrorRate : src,
                           write, &entryOffset, sizeof(entry->FrameErrorRate)); // INT8U
            copyListMember(write ? dest : (uint8_t *) &entry->MessageErrorRate, write ? (uint8_t *) &entry->MessageErrorRate : src,
                           write, &entryOffset, sizeof(entry->MessageErrorRate)); // INT8U
            copyListMember(write ? dest : (uint8_t *) &entry->RxOnWhenIdle, write ? (uint8_t *) &entry->RxOnWhenIdle : src, write,
                           &entryOffset, sizeof(entry->RxOnWhenIdle)); // BOOLEAN
            copyListMember(write ? dest : (uint8_t *) &entry->FullThreadDevice, write ? (uint8_t *) &entry->FullThreadDevice : src,
                           write, &entryOffset, sizeof(entry->FullThreadDevice)); // BOOLEAN
            copyListMember(write ? dest : (uint8_t *) &entry->FullNetworkData, write ? (uint8_t *) &entry->FullNetworkData : src,
                           write, &entryOffset, sizeof(entry->FullNetworkData)); // BOOLEAN
            copyListMember(write ? dest : (uint8_t *) &entry->IsChild, write ? (uint8_t *) &entry->IsChild : src, write,
                           &entryOffset, sizeof(entry->IsChild)); // BOOLEAN
            break;
        }
        case 0x0008: // RouteTableList
        {
            entryLength = 18;
            if (((index - 1) * entryLength) > (am->size - entryLength))
            {
                ChipLogError(Zcl, "Index %" PRId32 " is invalid.", index);
                return 0;
            }
            entryOffset = static_cast<uint16_t>(entryOffset + ((index - 1) * entryLength));
            // Struct _RouteTable
            _RouteTable * entry = reinterpret_cast<_RouteTable *>(write ? src : dest);
            copyListMember(write ? dest : (uint8_t *) &entry->ExtAddress, write ? (uint8_t *) &entry->ExtAddress : src, write,
                           &entryOffset, sizeof(entry->ExtAddress)); // INT64U
            copyListMember(write ? dest : (uint8_t *) &entry->Rloc16, write ? (uint8_t *) &entry->Rloc16 : src, write, &entryOffset,
                           sizeof(entry->Rloc16)); // INT16U
            copyListMember(write ? dest : (uint8_t *) &entry->RouterId, write ? (uint8_t *) &entry->RouterId : src, write,
                           &entryOffset, sizeof(entry->RouterId)); // INT8U
            copyListMember(write ? dest : (uint8_t *) &entry->NextHop, write ? (uint8_t *) &entry->NextHop : src, write,
                           &entryOffset, sizeof(entry->NextHop)); // INT8U
            copyListMember(write ? dest : (uint8_t *) &entry->PathCost, write ? (uint8_t *) &entry->PathCost : src, write,
                           &entryOffset, sizeof(entry->PathCost)); // INT8U
            copyListMember(write ? dest : (uint8_t *) &entry->LQIIn, write ? (uint8_t *) &entry->LQIIn : src, write, &entryOffset,
                           sizeof(entry->LQIIn)); // INT8U
            copyListMember(write ? dest : (uint8_t *) &entry->LQIOut, write ? (uint8_t *) &entry->LQIOut : src, write, &entryOffset,
                           sizeof(entry->LQIOut)); // INT8U
            copyListMember(write ? dest : (uint8_t *) &entry->Age, write ? (uint8_t *) &entry->Age : src, write, &entryOffset,
                           sizeof(entry->Age)); // INT8U
            copyListMember(write ? dest : (uint8_t *) &entry->Allocated, write ? (uint8_t *) &entry->Allocated : src, write,
                           &entryOffset, sizeof(entry->Allocated)); // BOOLEAN
            copyListMember(write ? dest : (uint8_t *) &entry->LinkEstablished, write ? (uint8_t *) &entry->LinkEstablished : src,
                           write, &entryOffset, sizeof(entry->LinkEstablished)); // BOOLEAN
            break;
        }
        case 0x003B: // SecurityPolicy
        {
            entryLength = 3;
            if (((index - 1) * entryLength) > (am->size - entryLength))
            {
                ChipLogError(Zcl, "Index %" PRId32 " is invalid.", index);
                return 0;
            }
            entryOffset = static_cast<uint16_t>(entryOffset + ((index - 1) * entryLength));
            // Struct _SecurityPolicy
            _SecurityPolicy * entry = reinterpret_cast<_SecurityPolicy *>(write ? src : dest);
            copyListMember(write ? dest : (uint8_t *) &entry->RotationTime, write ? (uint8_t *) &entry->RotationTime : src, write,
                           &entryOffset, sizeof(entry->RotationTime)); // INT16U
            copyListMember(write ? dest : (uint8_t *) &entry->Flags, write ? (uint8_t *) &entry->Flags : src, write, &entryOffset,
                           sizeof(entry->Flags)); // INT8U
            break;
        }
        case 0x003D: // OperationalDatasetComponents
        {
            entryLength = 12;
            if (((index - 1) * entryLength) > (am->size - entryLength))
            {
                ChipLogError(Zcl, "Index %" PRId32 " is invalid.", index);
                return 0;
            }
            entryOffset = static_cast<uint16_t>(entryOffset + ((index - 1) * entryLength));
            // Struct _OperationalDatasetComponents
            _OperationalDatasetComponents * entry = reinterpret_cast<_OperationalDatasetComponents *>(write ? src : dest);
            copyListMember(write ? dest : (uint8_t *) &entry->ActiveTimestampPresent,
                           write ? (uint8_t *) &entry->ActiveTimestampPresent : src, write, &entryOffset,
                           sizeof(entry->ActiveTimestampPresent)); // BOOLEAN
            copyListMember(write ? dest : (uint8_t *) &entry->PendingTimestampPresent,
                           write ? (uint8_t *) &entry->PendingTimestampPresent : src, write, &entryOffset,
                           sizeof(entry->PendingTimestampPresent)); // BOOLEAN
            copyListMember(write ? dest : (uint8_t *) &entry->MasterKeyPresent, write ? (uint8_t *) &entry->MasterKeyPresent : src,
                           write, &entryOffset, sizeof(entry->MasterKeyPresent)); // BOOLEAN
            copyListMember(write ? dest : (uint8_t *) &entry->NetworkNamePresent,
                           write ? (uint8_t *) &entry->NetworkNamePresent : src, write, &entryOffset,
                           sizeof(entry->NetworkNamePresent)); // BOOLEAN
            copyListMember(write ? dest : (uint8_t *) &entry->ExtendedPanIdPresent,
                           write ? (uint8_t *) &entry->ExtendedPanIdPresent : src, write, &entryOffset,
                           sizeof(entry->ExtendedPanIdPresent)); // BOOLEAN
            copyListMember(write ? dest : (uint8_t *) &entry->MeshLocalPrefixPresent,
                           write ? (uint8_t *) &entry->MeshLocalPrefixPresent : src, write, &entryOffset,
                           sizeof(entry->MeshLocalPrefixPresent)); // BOOLEAN
            copyListMember(write ? dest : (uint8_t *) &entry->DelayPresent, write ? (uint8_t *) &entry->DelayPresent : src, write,
                           &entryOffset, sizeof(entry->DelayPresent)); // BOOLEAN
            copyListMember(write ? dest : (uint8_t *) &entry->PanIdPresent, write ? (uint8_t *) &entry->PanIdPresent : src, write,
                           &entryOffset, sizeof(entry->PanIdPresent)); // BOOLEAN
            copyListMember(write ? dest : (uint8_t *) &entry->ChannelPresent, write ? (uint8_t *) &entry->ChannelPresent : src,
                           write, &entryOffset, sizeof(entry->ChannelPresent)); // BOOLEAN
            copyListMember(write ? dest : (uint8_t *) &entry->PskcPresent, write ? (uint8_t *) &entry->PskcPresent : src, write,
                           &entryOffset, sizeof(entry->PskcPresent)); // BOOLEAN
            copyListMember(write ? dest : (uint8_t *) &entry->SecurityPolicyPresent,
                           write ? (uint8_t *) &entry->SecurityPolicyPresent : src, write, &entryOffset,
                           sizeof(entry->SecurityPolicyPresent)); // BOOLEAN
            copyListMember(write ? dest : (uint8_t *) &entry->ChannelMaskPresent,
                           write ? (uint8_t *) &entry->ChannelMaskPresent : src, write, &entryOffset,
                           sizeof(entry->ChannelMaskPresent)); // BOOLEAN
            break;
        }
        case 0x003E: // ActiveNetworkFaultsList
        {
            entryLength = 1;
            if (((index - 1) * entryLength) > (am->size - entryLength))
            {
                ChipLogError(Zcl, "Index %" PRId32 " is invalid.", index);
                return 0;
            }
            entryOffset = static_cast<uint16_t>(entryOffset + ((index - 1) * entryLength));
            copyListMember(dest, src, write, &entryOffset, entryLength); // NetworkFault
            break;
        }
        }
        break;
    }
    }

    return entryLength;
}

// A list is a collection of entries of the same data type. The data type may be any defined data type.
uint16_t emberAfAttributeValueListSize(ClusterId clusterId, AttributeId attributeId, const uint8_t * buffer)
{
    // The first 2 bytes specify the number of entries. A value of 0xFFFF means the list in invalid
    // and data is undefined.
    uint16_t entryCount = emberAfGetInt16u(buffer, 0, kSizeLengthInBytes);
    if (entryCount == 0xFFFF)
    {
        return 0;
    }

    uint16_t entryLength = 0;
    switch (clusterId)
    {
    case 0x0030: // General Commissioning Cluster
        switch (attributeId)
        {
        case 0x0001: // BasicCommissioningInfoList
            // Struct _BasicCommissioningInfoType
            entryLength = 4;
            break;
        }
        break;
    case 0x0033: // General Diagnostics Cluster
        switch (attributeId)
        {
        case 0x0000: // NetworkInterfaces
            // Struct _NetworkInterfaceType
            entryLength = 48;
            break;
        }
        break;
    case 0x003E: // Operational Credentials Cluster
        switch (attributeId)
        {
        case 0x0001: // fabrics list
            // Struct _FabricDescriptor
            entryLength = 120;
            break;
        case 0x0004: // TrustedRootCertificates
            // chip::ByteSpan
            return GetByteSpanOffsetFromIndex(buffer, 402, entryCount);
            break;
        }
        break;
    case 0x0035: // Thread Network Diagnostics Cluster
        switch (attributeId)
        {
        case 0x0007: // NeighborTableList
            // Struct _NeighborTable
            entryLength = 31;
            break;
        case 0x0008: // RouteTableList
            // Struct _RouteTable
            entryLength = 18;
            break;
        case 0x003B: // SecurityPolicy
            // Struct _SecurityPolicy
            entryLength = 3;
            break;
        case 0x003D: // OperationalDatasetComponents
            // Struct _OperationalDatasetComponents
            entryLength = 12;
            break;
        case 0x003E: // ActiveNetworkFaultsList
            // uint8_t
            entryLength = 1;
            break;
        }
        break;
    }

    uint32_t totalSize = kSizeLengthInBytes + (entryCount * entryLength);
    if (!CanCastTo<uint16_t>(totalSize))
    {
        ChipLogError(Zcl, "Cluster " ChipLogFormatMEI ": Size of attribute " ChipLogFormatMEI " is too large.",
                     ChipLogValueMEI(clusterId), ChipLogValueMEI(attributeId));
        return 0;
    }

    return static_cast<uint16_t>(totalSize);
}
