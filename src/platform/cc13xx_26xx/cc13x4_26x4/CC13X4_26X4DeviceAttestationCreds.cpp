/*
 *
 *    Copyright (c) 2022 Project CHIP Authors
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
#include "CC13X4_26X4DeviceAttestationCreds.h"
#include <crypto/CHIPCryptoPAL.h>
#include <lib/core/CHIPError.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/Span.h>

extern uint32_t __attestation_credentials_base;

namespace chip {
namespace Credentials {
namespace CC13X2_26X2 {

namespace {

extern "C" {

extern void cc13xx_26x2Log(const char * aFormat, ...);

typedef struct
{
    const uint32_t len;
    uint8_t const * data;
} data_ptr;

typedef struct
{
    data_ptr dac_priv_key;
    data_ptr dac_pub_key;
    data_ptr dac_cert;
    data_ptr pai_cert;
} factoryData;

const uint8_t gDacPrivKey[] = {
    0x50, 0x5a, 0x21, 0x1d, 0xbd, 0xa8, 0x71, 0x33, 0x0d, 0x63, 0x5d, 0xa3, 0xb0, 0x7e, 0xb1, 0xc5,
    0x08, 0x8a, 0x8f, 0xc7, 0x01, 0x24, 0xfb, 0xb3, 0x3e, 0x93, 0xd5, 0x06, 0x05, 0x82, 0xc7, 0xc5,
};
const uint8_t gDacPubKey[] = {
    0x04, 0xc5, 0x65, 0xfd, 0xad, 0xfd, 0x16, 0xdd, 0x62, 0xe4, 0x3f, 0x19, 0x60, 0xb9, 0x93, 0xbb, 0x57,
    0x2c, 0xfd, 0xd8, 0x1f, 0x6d, 0x71, 0x67, 0x67, 0x1b, 0x77, 0x45, 0xdc, 0xbe, 0x6f, 0x65, 0xaf, 0x66,
    0x5a, 0x1d, 0x93, 0x1c, 0x05, 0xb9, 0xf9, 0xa3, 0xe9, 0x45, 0x66, 0x85, 0x60, 0x2c, 0x05, 0xc6, 0x96,
    0x46, 0xb8, 0xf7, 0x59, 0x98, 0xdb, 0xaa, 0x68, 0x7a, 0x5c, 0x56, 0x49, 0x02, 0xda,
};
const uint8_t gDacCert[] = {
    0x30, 0x82, 0x01, 0xf7, 0x30, 0x82, 0x01, 0x9d, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x08, 0x46, 0x88, 0xeb, 0x94, 0xad, 0x32,
    0xb2, 0xe4, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x02, 0x30, 0x4d, 0x31, 0x1f, 0x30, 0x1d, 0x06,
    0x03, 0x55, 0x04, 0x03, 0x0c, 0x16, 0x4d, 0x61, 0x74, 0x74, 0x65, 0x72, 0x20, 0x44, 0x65, 0x76, 0x65, 0x6c, 0x6f, 0x70, 0x6d,
    0x65, 0x6e, 0x74, 0x20, 0x50, 0x41, 0x49, 0x31, 0x14, 0x30, 0x12, 0x06, 0x0a, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x82, 0xa2, 0x7c,
    0x02, 0x01, 0x0c, 0x04, 0x46, 0x46, 0x46, 0x31, 0x31, 0x14, 0x30, 0x12, 0x06, 0x0a, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x82, 0xa2,
    0x7c, 0x02, 0x02, 0x0c, 0x04, 0x38, 0x30, 0x30, 0x36, 0x30, 0x20, 0x17, 0x0d, 0x32, 0x31, 0x30, 0x36, 0x32, 0x38, 0x31, 0x34,
    0x32, 0x33, 0x34, 0x33, 0x5a, 0x18, 0x0f, 0x39, 0x39, 0x39, 0x39, 0x31, 0x32, 0x33, 0x31, 0x32, 0x33, 0x35, 0x39, 0x35, 0x39,
    0x5a, 0x30, 0x52, 0x31, 0x24, 0x30, 0x22, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x1b, 0x4d, 0x61, 0x74, 0x74, 0x65, 0x72, 0x20,
    0x44, 0x65, 0x76, 0x65, 0x6c, 0x6f, 0x70, 0x6d, 0x65, 0x6e, 0x74, 0x20, 0x44, 0x41, 0x43, 0x20, 0x30, 0x30, 0x30, 0x30, 0x31,
    0x14, 0x30, 0x12, 0x06, 0x0a, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x82, 0xa2, 0x7c, 0x02, 0x01, 0x0c, 0x04, 0x46, 0x46, 0x46, 0x31,
    0x31, 0x14, 0x30, 0x12, 0x06, 0x0a, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x82, 0xa2, 0x7c, 0x02, 0x02, 0x0c, 0x04, 0x38, 0x30, 0x30,
    0x36, 0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d,
    0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04, 0xc5, 0x65, 0xfd, 0xad, 0xfd, 0x16, 0xdd, 0x62, 0xe4, 0x3f, 0x19, 0x60, 0xb9, 0x93,
    0xbb, 0x57, 0x2c, 0xfd, 0xd8, 0x1f, 0x6d, 0x71, 0x67, 0x67, 0x1b, 0x77, 0x45, 0xdc, 0xbe, 0x6f, 0x65, 0xaf, 0x66, 0x5a, 0x1d,
    0x93, 0x1c, 0x05, 0xb9, 0xf9, 0xa3, 0xe9, 0x45, 0x66, 0x85, 0x60, 0x2c, 0x05, 0xc6, 0x96, 0x46, 0xb8, 0xf7, 0x59, 0x98, 0xdb,
    0xaa, 0x68, 0x7a, 0x5c, 0x56, 0x49, 0x02, 0xda, 0xa3, 0x60, 0x30, 0x5e, 0x30, 0x0c, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x01, 0x01,
    0xff, 0x04, 0x02, 0x30, 0x00, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x01, 0x01, 0xff, 0x04, 0x04, 0x03, 0x02, 0x07, 0x80,
    0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0x9b, 0x40, 0x60, 0x6f, 0x9e, 0x04, 0x7f, 0xb8, 0x60, 0x78,
    0x8e, 0x3d, 0xc1, 0x12, 0xd7, 0x5e, 0x87, 0x95, 0x77, 0x68, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18, 0x30, 0x16,
    0x80, 0x14, 0x43, 0x34, 0x57, 0x12, 0xba, 0x2c, 0x87, 0xef, 0x25, 0x49, 0x7b, 0x11, 0xd7, 0x98, 0x58, 0x9b, 0x84, 0x35, 0x7f,
    0x88, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x02, 0x03, 0x48, 0x00, 0x30, 0x45, 0x02, 0x21, 0x00,
    0xb9, 0x28, 0xf9, 0x3e, 0xe3, 0x87, 0xef, 0x3e, 0x00, 0x72, 0x88, 0x22, 0x84, 0xbd, 0x8a, 0xdd, 0x5c, 0xd6, 0xd0, 0x55, 0x81,
    0xbf, 0xcc, 0x55, 0x17, 0xcf, 0x9e, 0x9b, 0xcd, 0xd4, 0x37, 0xda, 0x02, 0x20, 0x10, 0x07, 0x9c, 0xcf, 0x7f, 0x1f, 0x2d, 0xda,
    0x46, 0xac, 0xe9, 0x67, 0xae, 0x5b, 0xe9, 0x66, 0xe7, 0xf2, 0x8a, 0xdf, 0xa0, 0x28, 0xb8, 0xf8, 0x7f, 0x93, 0x9e, 0xd4, 0x15,
    0x8d, 0xc0, 0xf8,
};

const uint8_t gPaiCert[] = {
    0x30, 0x82, 0x01, 0xdc, 0x30, 0x82, 0x01, 0x81, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x08, 0x7f, 0x7e, 0xf3, 0xdb, 0x08, 0xa3,
    0x8f, 0x68, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x02, 0x30, 0x30, 0x31, 0x18, 0x30, 0x16, 0x06,
    0x03, 0x55, 0x04, 0x03, 0x0c, 0x0f, 0x4d, 0x61, 0x74, 0x74, 0x65, 0x72, 0x20, 0x54, 0x65, 0x73, 0x74, 0x20, 0x50, 0x41, 0x41,
    0x31, 0x14, 0x30, 0x12, 0x06, 0x0a, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x82, 0xa2, 0x7c, 0x02, 0x01, 0x0c, 0x04, 0x46, 0x46, 0x46,
    0x31, 0x30, 0x20, 0x17, 0x0d, 0x32, 0x31, 0x30, 0x36, 0x32, 0x38, 0x31, 0x34, 0x32, 0x33, 0x34, 0x33, 0x5a, 0x18, 0x0f, 0x39,
    0x39, 0x39, 0x39, 0x31, 0x32, 0x33, 0x31, 0x32, 0x33, 0x35, 0x39, 0x35, 0x39, 0x5a, 0x30, 0x4d, 0x31, 0x1f, 0x30, 0x1d, 0x06,
    0x03, 0x55, 0x04, 0x03, 0x0c, 0x16, 0x4d, 0x61, 0x74, 0x74, 0x65, 0x72, 0x20, 0x44, 0x65, 0x76, 0x65, 0x6c, 0x6f, 0x70, 0x6d,
    0x65, 0x6e, 0x74, 0x20, 0x50, 0x41, 0x49, 0x31, 0x14, 0x30, 0x12, 0x06, 0x0a, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x82, 0xa2, 0x7c,
    0x02, 0x01, 0x0c, 0x04, 0x46, 0x46, 0x46, 0x31, 0x31, 0x14, 0x30, 0x12, 0x06, 0x0a, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x82, 0xa2,
    0x7c, 0x02, 0x02, 0x0c, 0x04, 0x38, 0x30, 0x30, 0x36, 0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02,
    0x01, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04, 0x42, 0x93, 0x55, 0x77, 0x35, 0x35,
    0x15, 0xce, 0xfa, 0x8f, 0x0e, 0x30, 0xe2, 0x34, 0x7e, 0x90, 0xee, 0xd6, 0xfd, 0x51, 0x5b, 0xe8, 0x82, 0xd1, 0xbc, 0xba, 0x74,
    0x83, 0xd7, 0xff, 0x6e, 0xf4, 0xde, 0xdf, 0x98, 0xf7, 0xf7, 0x4f, 0x17, 0x42, 0x1d, 0xe5, 0x45, 0x0c, 0xff, 0xfb, 0x3e, 0x7f,
    0x6d, 0x4f, 0x62, 0x28, 0x53, 0x41, 0x14, 0xfb, 0xb8, 0x5c, 0x2d, 0x52, 0xd1, 0x82, 0xb6, 0x6a, 0xa3, 0x66, 0x30, 0x64, 0x30,
    0x12, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x01, 0x01, 0xff, 0x04, 0x08, 0x30, 0x06, 0x01, 0x01, 0xff, 0x02, 0x01, 0x00, 0x30, 0x0e,
    0x06, 0x03, 0x55, 0x1d, 0x0f, 0x01, 0x01, 0xff, 0x04, 0x04, 0x03, 0x02, 0x01, 0x06, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e,
    0x04, 0x16, 0x04, 0x14, 0x43, 0x34, 0x57, 0x12, 0xba, 0x2c, 0x87, 0xef, 0x25, 0x49, 0x7b, 0x11, 0xd7, 0x98, 0x58, 0x9b, 0x84,
    0x35, 0x7f, 0x88, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80, 0x14, 0x6a, 0xfd, 0x22, 0x77, 0x1f,
    0x51, 0x1f, 0xec, 0xbf, 0x16, 0x41, 0x97, 0x67, 0x10, 0xdc, 0xdc, 0x31, 0xa1, 0x71, 0x7e, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86,
    0x48, 0xce, 0x3d, 0x04, 0x03, 0x02, 0x03, 0x49, 0x00, 0x30, 0x46, 0x02, 0x21, 0x00, 0xff, 0x25, 0xf1, 0xd1, 0x54, 0xc3, 0x13,
    0x7e, 0x0e, 0x08, 0x6d, 0x82, 0xab, 0x0b, 0x11, 0xb5, 0x66, 0x18, 0x39, 0xb7, 0x12, 0xb1, 0x34, 0x2c, 0x6c, 0xde, 0x94, 0xfb,
    0xe3, 0x18, 0xa9, 0x2b, 0x02, 0x21, 0x00, 0x92, 0xe2, 0x81, 0xf9, 0x8a, 0x2f, 0xcc, 0x14, 0xcd, 0xf4, 0x07, 0x50, 0xd2, 0x80,
    0xd7, 0xdf, 0xea, 0x3f, 0x4d, 0xa4, 0x6f, 0x35, 0x7a, 0xfe, 0xac, 0xb8, 0x9b, 0x26, 0x77, 0x06, 0xd2, 0x8a,
};

const factoryData gFactoryData = {
    .dac_priv_key = {
        .len = sizeof(gDacPrivKey),
        .data = gDacPrivKey,
    },
    .dac_pub_key = {
        .len = sizeof(gDacPubKey),
        .data = gDacPubKey,
    },
    .dac_cert = {
        .len = sizeof(gDacCert),
        .data = gDacCert,
    },
    .pai_cert = {
        .len = sizeof(gPaiCert),
        .data = gPaiCert,
    },
};

} // extern "C"

CHIP_ERROR LoadKeypairFromRaw(ByteSpan private_key, ByteSpan public_key, Crypto::P256Keypair & keypair)
{
    Crypto::P256SerializedKeypair serialized_keypair;
    ReturnErrorOnFailure(serialized_keypair.SetLength(private_key.size() + public_key.size()));
    memcpy(serialized_keypair.Bytes(), public_key.data(), public_key.size());
    memcpy(serialized_keypair.Bytes() + public_key.size(), private_key.data(), private_key.size());
    return keypair.Deserialize(serialized_keypair);
}

class DeviceAttestationCredsCC13X2_26X2 : public DeviceAttestationCredentialsProvider
{

public:
    CHIP_ERROR GetCertificationDeclaration(MutableByteSpan & out_buffer) override;
    CHIP_ERROR GetFirmwareInformation(MutableByteSpan & out_firmware_info_buffer) override;
    CHIP_ERROR GetDeviceAttestationCert(MutableByteSpan & out_buffer) override;
    CHIP_ERROR GetProductAttestationIntermediateCert(MutableByteSpan & out_buffer) override;
    CHIP_ERROR SignWithDeviceAttestationKey(const ByteSpan & message_to_sign, MutableByteSpan & out_buffer) override;

private:
    factoryData const * mFactoryData = &gFactoryData;
};

CHIP_ERROR DeviceAttestationCredsCC13X2_26X2::GetCertificationDeclaration(MutableByteSpan & out_buffer)
{
    //-> format_version = 1
    //-> vendor_id = 0xFFF1
    //-> product_id_array = [ 0x8000, 0x8001, 0x8002, 0x8003, 0x8004, 0x8005, 0x8006, 0x8007, 0x8008, 0x8009, 0x800A, 0x800B,
    // 0x800C, 0x800D, 0x800E, 0x800F, 0x8010, 0x8011, 0x8012, 0x8013, 0x8014, 0x8015, 0x8016, 0x8017, 0x8018, 0x8019, 0x801A,
    // 0x801B, 0x801C, 0x801D, 0x801E, 0x801F, 0x8020, 0x8021, 0x8022, 0x8023, 0x8024, 0x8025, 0x8026, 0x8027, 0x8028, 0x8029,
    // 0x802A, 0x802B, 0x802C, 0x802D, 0x802E, 0x802F, 0x8030, 0x8031, 0x8032, 0x8033, 0x8034, 0x8035, 0x8036, 0x8037, 0x8038,
    // 0x8039, 0x803A, 0x803B, 0x803C, 0x803D, 0x803E, 0x803F, 0x8040, 0x8041, 0x8042, 0x8043, 0x8044, 0x8045, 0x8046, 0x8047,
    // 0x8048, 0x8049, 0x804A, 0x804B, 0x804C, 0x804D, 0x804E, 0x804F, 0x8050, 0x8051, 0x8052, 0x8053, 0x8054, 0x8055, 0x8056,
    // 0x8057, 0x8058, 0x8059, 0x805A, 0x805B, 0x805C, 0x805D, 0x805E, 0x805F, 0x8060, 0x8061, 0x8062, 0x8063 ]
    //-> device_type_id = 0x0016
    //-> certificate_id = "ZIG20142ZB330003-24"
    //-> security_level = 0
    //-> security_information = 0
    //-> version_number = 0x2694
    //-> certification_type = 0
    //-> dac_origin_vendor_id is not present
    //-> dac_origin_product_id is not present
    const uint8_t kCdForAllExamples[] = {
        0x30, 0x82, 0x02, 0x19, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x07, 0x02, 0xa0, 0x82, 0x02, 0x0a, 0x30,
        0x82, 0x02, 0x06, 0x02, 0x01, 0x03, 0x31, 0x0d, 0x30, 0x0b, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02,
        0x01, 0x30, 0x82, 0x01, 0x71, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x07, 0x01, 0xa0, 0x82, 0x01, 0x62,
        0x04, 0x82, 0x01, 0x5e, 0x15, 0x24, 0x00, 0x01, 0x25, 0x01, 0xf1, 0xff, 0x36, 0x02, 0x05, 0x00, 0x80, 0x05, 0x01, 0x80,
        0x05, 0x02, 0x80, 0x05, 0x03, 0x80, 0x05, 0x04, 0x80, 0x05, 0x05, 0x80, 0x05, 0x06, 0x80, 0x05, 0x07, 0x80, 0x05, 0x08,
        0x80, 0x05, 0x09, 0x80, 0x05, 0x0a, 0x80, 0x05, 0x0b, 0x80, 0x05, 0x0c, 0x80, 0x05, 0x0d, 0x80, 0x05, 0x0e, 0x80, 0x05,
        0x0f, 0x80, 0x05, 0x10, 0x80, 0x05, 0x11, 0x80, 0x05, 0x12, 0x80, 0x05, 0x13, 0x80, 0x05, 0x14, 0x80, 0x05, 0x15, 0x80,
        0x05, 0x16, 0x80, 0x05, 0x17, 0x80, 0x05, 0x18, 0x80, 0x05, 0x19, 0x80, 0x05, 0x1a, 0x80, 0x05, 0x1b, 0x80, 0x05, 0x1c,
        0x80, 0x05, 0x1d, 0x80, 0x05, 0x1e, 0x80, 0x05, 0x1f, 0x80, 0x05, 0x20, 0x80, 0x05, 0x21, 0x80, 0x05, 0x22, 0x80, 0x05,
        0x23, 0x80, 0x05, 0x24, 0x80, 0x05, 0x25, 0x80, 0x05, 0x26, 0x80, 0x05, 0x27, 0x80, 0x05, 0x28, 0x80, 0x05, 0x29, 0x80,
        0x05, 0x2a, 0x80, 0x05, 0x2b, 0x80, 0x05, 0x2c, 0x80, 0x05, 0x2d, 0x80, 0x05, 0x2e, 0x80, 0x05, 0x2f, 0x80, 0x05, 0x30,
        0x80, 0x05, 0x31, 0x80, 0x05, 0x32, 0x80, 0x05, 0x33, 0x80, 0x05, 0x34, 0x80, 0x05, 0x35, 0x80, 0x05, 0x36, 0x80, 0x05,
        0x37, 0x80, 0x05, 0x38, 0x80, 0x05, 0x39, 0x80, 0x05, 0x3a, 0x80, 0x05, 0x3b, 0x80, 0x05, 0x3c, 0x80, 0x05, 0x3d, 0x80,
        0x05, 0x3e, 0x80, 0x05, 0x3f, 0x80, 0x05, 0x40, 0x80, 0x05, 0x41, 0x80, 0x05, 0x42, 0x80, 0x05, 0x43, 0x80, 0x05, 0x44,
        0x80, 0x05, 0x45, 0x80, 0x05, 0x46, 0x80, 0x05, 0x47, 0x80, 0x05, 0x48, 0x80, 0x05, 0x49, 0x80, 0x05, 0x4a, 0x80, 0x05,
        0x4b, 0x80, 0x05, 0x4c, 0x80, 0x05, 0x4d, 0x80, 0x05, 0x4e, 0x80, 0x05, 0x4f, 0x80, 0x05, 0x50, 0x80, 0x05, 0x51, 0x80,
        0x05, 0x52, 0x80, 0x05, 0x53, 0x80, 0x05, 0x54, 0x80, 0x05, 0x55, 0x80, 0x05, 0x56, 0x80, 0x05, 0x57, 0x80, 0x05, 0x58,
        0x80, 0x05, 0x59, 0x80, 0x05, 0x5a, 0x80, 0x05, 0x5b, 0x80, 0x05, 0x5c, 0x80, 0x05, 0x5d, 0x80, 0x05, 0x5e, 0x80, 0x05,
        0x5f, 0x80, 0x05, 0x60, 0x80, 0x05, 0x61, 0x80, 0x05, 0x62, 0x80, 0x05, 0x63, 0x80, 0x18, 0x24, 0x03, 0x16, 0x2c, 0x04,
        0x13, 0x5a, 0x49, 0x47, 0x32, 0x30, 0x31, 0x34, 0x32, 0x5a, 0x42, 0x33, 0x33, 0x30, 0x30, 0x30, 0x33, 0x2d, 0x32, 0x34,
        0x24, 0x05, 0x00, 0x24, 0x06, 0x00, 0x25, 0x07, 0x94, 0x26, 0x24, 0x08, 0x00, 0x18, 0x31, 0x7d, 0x30, 0x7b, 0x02, 0x01,
        0x03, 0x80, 0x14, 0x62, 0xfa, 0x82, 0x33, 0x59, 0xac, 0xfa, 0xa9, 0x96, 0x3e, 0x1c, 0xfa, 0x14, 0x0a, 0xdd, 0xf5, 0x04,
        0xf3, 0x71, 0x60, 0x30, 0x0b, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01, 0x30, 0x0a, 0x06, 0x08,
        0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x02, 0x04, 0x47, 0x30, 0x45, 0x02, 0x20, 0x24, 0xe5, 0xd1, 0xf4, 0x7a, 0x7d,
        0x7b, 0x0d, 0x20, 0x6a, 0x26, 0xef, 0x69, 0x9b, 0x7c, 0x97, 0x57, 0xb7, 0x2d, 0x46, 0x90, 0x89, 0xde, 0x31, 0x92, 0xe6,
        0x78, 0xc7, 0x45, 0xe7, 0xf6, 0x0c, 0x02, 0x21, 0x00, 0xf8, 0xaa, 0x2f, 0xa7, 0x11, 0xfc, 0xb7, 0x9b, 0x97, 0xe3, 0x97,
        0xce, 0xda, 0x66, 0x7b, 0xae, 0x46, 0x4e, 0x2b, 0xd3, 0xff, 0xdf, 0xc3, 0xcc, 0xed, 0x7a, 0xa8, 0xca, 0x5f, 0x4c, 0x1a,
        0x7c,
    };

    return CopySpanToMutableSpan(ByteSpan{ kCdForAllExamples }, out_buffer);
}

CHIP_ERROR DeviceAttestationCredsCC13X2_26X2::GetFirmwareInformation(MutableByteSpan & out_firmware_info_buffer)
{
    out_firmware_info_buffer.reduce_size(0);

    return CHIP_NO_ERROR;
}

CHIP_ERROR DeviceAttestationCredsCC13X2_26X2::GetDeviceAttestationCert(MutableByteSpan & out_buffer)
{
    ReturnErrorCodeIf(out_buffer.size() < mFactoryData->dac_cert.len, CHIP_ERROR_BUFFER_TOO_SMALL);
    ReturnErrorCodeIf(!mFactoryData->dac_cert.data, CHIP_ERROR_PERSISTED_STORAGE_VALUE_NOT_FOUND);

    return CopySpanToMutableSpan(ByteSpan{ mFactoryData->dac_cert.data, mFactoryData->dac_cert.len }, out_buffer);
    return CHIP_NO_ERROR;
}

CHIP_ERROR DeviceAttestationCredsCC13X2_26X2::GetProductAttestationIntermediateCert(MutableByteSpan & out_buffer)
{
    ReturnErrorCodeIf(out_buffer.size() < mFactoryData->pai_cert.len, CHIP_ERROR_BUFFER_TOO_SMALL);
    ReturnErrorCodeIf(!mFactoryData->pai_cert.data, CHIP_ERROR_PERSISTED_STORAGE_VALUE_NOT_FOUND);

    return CopySpanToMutableSpan(ByteSpan{ mFactoryData->pai_cert.data, mFactoryData->pai_cert.len }, out_buffer);
}

CHIP_ERROR DeviceAttestationCredsCC13X2_26X2::SignWithDeviceAttestationKey(const ByteSpan & message_to_sign,
                                                                           MutableByteSpan & out_buffer)
{
    Crypto::P256ECDSASignature signature;
    Crypto::P256Keypair keypair;

    VerifyOrReturnError(IsSpanUsable(out_buffer), CHIP_ERROR_INVALID_ARGUMENT);
    VerifyOrReturnError(IsSpanUsable(message_to_sign), CHIP_ERROR_INVALID_ARGUMENT);
    VerifyOrReturnError(out_buffer.size() >= signature.Capacity(), CHIP_ERROR_BUFFER_TOO_SMALL);

    // In a non-exemplary implementation, the public key is not needed here. It is used here merely because
    // Crypto::P256Keypair is only (currently) constructable from raw keys if both private/public keys are present.
    ReturnErrorOnFailure(LoadKeypairFromRaw(ByteSpan(mFactoryData->dac_priv_key.data, mFactoryData->dac_priv_key.len),
                                            ByteSpan(mFactoryData->dac_pub_key.data, mFactoryData->dac_pub_key.len), keypair));
    ReturnErrorOnFailure(keypair.ECDSA_sign_msg(message_to_sign.data(), message_to_sign.size(), signature));

    return CopySpanToMutableSpan(ByteSpan{ signature.ConstBytes(), signature.Length() }, out_buffer);
}

} // namespace

DeviceAttestationCredentialsProvider * GetCC13X2_26X2DacProvider()
{
    static DeviceAttestationCredsCC13X2_26X2 dac_provider;
    return &dac_provider;
}

} // namespace CC13X2_26X2
} // namespace Credentials
} // namespace chip
