/*
 *    Copyright (c) 2023 Project CHIP Authors
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

#include "PSASessionKeystore.h"

#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>
#include <psa/crypto.h>

#include <string.h>

namespace chip {
namespace Crypto {

namespace {

#ifdef CHIP_CRYPTO_PSA_HKDF_IMPORT_WORKAROUND
// TI CC35XX: HSM does not support psa_import_key with PSA_KEY_TYPE_DERIVE,
// and PSA_ALG_HKDF / HKDF_EXTRACT / HKDF_EXPAND are also unsupported.
// MBEDTLS_SHA256_C (legacy MD) is disabled (hardware-only SHA256), so
// mbedtls_hkdf() cannot be used either.
// Solution: implement HKDF using only psa_hash_setup/update/finish which
// the TI HSM PSA driver does support.

static constexpr psa_key_id_t kHkdfRawKeyId                       = 0xFFFF0001U;
static uint8_t sHkdfRawKeyBytes[PSA_HASH_LENGTH(PSA_ALG_SHA_256)] = {};
static size_t sHkdfRawKeyLen                                      = 0;

// HMAC-SHA256 computed via PSA streaming hash only (no key import).
// Accepts up to 3 message parts to avoid extra allocations.
static psa_status_t hmac_sha256_psa(const uint8_t * key, size_t key_len, const uint8_t * p1, size_t p1_len, const uint8_t * p2,
                                    size_t p2_len, const uint8_t * p3, size_t p3_len, uint8_t mac[32])
{
    constexpr size_t kBlock = 64;
    constexpr size_t kHash  = 32;

    // Declare all locals before any goto targets to avoid C++ jump-past-init issues
    uint8_t k[kBlock]       = {};
    uint8_t k_pad[kBlock]   = {};
    uint8_t inner[kHash]    = {};
    size_t hash_out_len     = 0;
    psa_hash_operation_t op = PSA_HASH_OPERATION_INIT;
    psa_status_t status;

    // If key > block size, hash it first
    if (key_len > kBlock)
    {
        status = psa_hash_compute(PSA_ALG_SHA_256, key, key_len, k, kHash, &hash_out_len);
        if (status != PSA_SUCCESS)
            goto cleanup;
    }
    else
    {
        memcpy(k, key, key_len);
    }

    // Inner hash: SHA256((k XOR 0x36) || p1 || p2 || p3)
    for (size_t i = 0; i < kBlock; i++)
        k_pad[i] = k[i] ^ 0x36u;

    status = psa_hash_setup(&op, PSA_ALG_SHA_256);
    if (status != PSA_SUCCESS)
        goto cleanup;
    status = psa_hash_update(&op, k_pad, kBlock);
    if (status != PSA_SUCCESS)
        goto abort_op;
    if (p1_len > 0 && (status = psa_hash_update(&op, p1, p1_len)) != PSA_SUCCESS)
        goto abort_op;
    if (p2_len > 0 && (status = psa_hash_update(&op, p2, p2_len)) != PSA_SUCCESS)
        goto abort_op;
    if (p3_len > 0 && (status = psa_hash_update(&op, p3, p3_len)) != PSA_SUCCESS)
        goto abort_op;
    status = psa_hash_finish(&op, inner, kHash, &hash_out_len);
    if (status != PSA_SUCCESS)
        goto cleanup;
    // PSA spec: must call psa_hash_abort before reusing an operation object
    // after psa_hash_finish, even on success.
    psa_hash_abort(&op);

    // Outer hash: SHA256((k XOR 0x5C) || inner)
    for (size_t i = 0; i < kBlock; i++)
        k_pad[i] = k[i] ^ 0x5Cu;

    status = psa_hash_setup(&op, PSA_ALG_SHA_256);
    if (status != PSA_SUCCESS)
        goto cleanup;
    status = psa_hash_update(&op, k_pad, kBlock);
    if (status != PSA_SUCCESS)
        goto abort_op;
    status = psa_hash_update(&op, inner, kHash);
    if (status != PSA_SUCCESS)
        goto abort_op;
    status = psa_hash_finish(&op, mac, kHash, &hash_out_len);
    goto cleanup;

abort_op:
    psa_hash_abort(&op);
cleanup:
    memset(k, 0, sizeof(k));
    memset(inner, 0, sizeof(inner));
    return status;
}

// HKDF-Extract: PRK = HMAC-SHA256(salt, IKM).  RFC 5869 §2.2
static psa_status_t hkdf_extract_psa(const uint8_t * salt, size_t salt_len, const uint8_t * ikm, size_t ikm_len, uint8_t prk[32])
{
    // Per RFC 5869: if salt not provided, set to HashLen zeros
    static const uint8_t kZeroSalt[32] = {};
    if (salt == nullptr || salt_len == 0)
    {
        salt     = kZeroSalt;
        salt_len = 32;
    }
    return hmac_sha256_psa(salt, salt_len, ikm, ikm_len, nullptr, 0, nullptr, 0, prk);
}

// HKDF-Expand: OKM = T(1) || T(2) || ...  RFC 5869 §2.3
// T(i) = HMAC-SHA256(PRK, T(i-1) || info || i)
static psa_status_t hkdf_expand_psa(const uint8_t prk[32], const uint8_t * info, size_t info_len, uint8_t * okm, size_t okm_len)
{
    constexpr size_t kHash = 32;
    uint8_t t[kHash]       = {};
    size_t done            = 0;

    for (uint8_t i = 1; done < okm_len; i++)
    {
        uint8_t counter = i;
        uint8_t new_t[kHash];
        // For i==1 T(0) is empty, so pass zero-length first part
        psa_status_t s =
            hmac_sha256_psa(prk, kHash, (i > 1 ? t : nullptr), (i > 1 ? kHash : 0), info, info_len, &counter, 1, new_t);
        if (s != PSA_SUCCESS)
        {
            memset(t, 0, kHash);
            return s;
        }
        memcpy(t, new_t, kHash);
        size_t copy_len = (okm_len - done < kHash) ? (okm_len - done) : kHash;
        memcpy(okm + done, t, copy_len);
        done += copy_len;
    }
    memset(t, 0, kHash);
    return PSA_SUCCESS;
}

// Combined HKDF (Extract + Expand)
static psa_status_t sw_hkdf_sha256(const uint8_t * salt, size_t salt_len, const uint8_t * ikm, size_t ikm_len, const uint8_t * info,
                                   size_t info_len, uint8_t * okm, size_t okm_len)
{
    uint8_t prk[32];
    psa_status_t s = hkdf_extract_psa(salt, salt_len, ikm, ikm_len, prk);
    if (s == PSA_SUCCESS)
        s = hkdf_expand_psa(prk, info, info_len, okm, okm_len);
    memset(prk, 0, sizeof(prk));
    return s;
}

#endif // CHIP_CRYPTO_PSA_HKDF_IMPORT_WORKAROUND

class KeyAttributesBase
{
public:
    KeyAttributesBase(psa_key_type_t type, psa_algorithm_t algorithm, psa_key_usage_t usageFlags, size_t bits)
    {
        psa_set_key_type(&mAttrs, type);
        psa_set_key_algorithm(&mAttrs, algorithm);
        psa_set_key_usage_flags(&mAttrs, usageFlags);
        psa_set_key_bits(&mAttrs, bits);
        GetPSAKeyAllocator().UpdateKeyAttributes(mAttrs);
    }

    ~KeyAttributesBase() { psa_reset_key_attributes(&mAttrs); }

    const psa_key_attributes_t & Get() { return mAttrs; }

private:
    psa_key_attributes_t mAttrs = PSA_KEY_ATTRIBUTES_INIT;
};

class AesKeyAttributes : public KeyAttributesBase
{
public:
    AesKeyAttributes() :
        KeyAttributesBase(PSA_KEY_TYPE_AES, PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(PSA_ALG_CCM, 8),
                          PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT | PSA_KEY_USAGE_COPY,
                          CHIP_CRYPTO_SYMMETRIC_KEY_LENGTH_BYTES * 8)
    {}
};

class HmacKeyAttributes : public KeyAttributesBase
{
public:
    HmacKeyAttributes() :
        KeyAttributesBase(PSA_KEY_TYPE_HMAC, PSA_ALG_HMAC(PSA_ALG_SHA_256), PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_COPY,
                          CHIP_CRYPTO_SYMMETRIC_KEY_LENGTH_BYTES * 8)
    {}
};

class HkdfKeyAttributes : public KeyAttributesBase
{
public:
    HkdfKeyAttributes() : KeyAttributesBase(PSA_KEY_TYPE_DERIVE, PSA_ALG_HKDF(PSA_ALG_SHA_256), PSA_KEY_USAGE_DERIVE, 0) {}
};

#if CHIP_CONFIG_ENABLE_ICD_CIP
void SetKeyId(Symmetric128BitsKeyHandle & key, psa_key_id_t newKeyId)
{
    auto & KeyId = key.AsMutable<psa_key_id_t>();

    KeyId = newKeyId;
}
#endif
} // namespace

CHIP_ERROR PSASessionKeystore::CreateKey(const Symmetric128BitsKeyByteArray & keyMaterial, Aes128KeyHandle & key)
{
    // Destroy the old key if already allocated
    DestroyKey(key);

    AesKeyAttributes attrs;
    psa_status_t status =
        psa_import_key(&attrs.Get(), keyMaterial, sizeof(Symmetric128BitsKeyByteArray), &key.AsMutable<psa_key_id_t>());
    LogPsaError(status);
    VerifyOrReturnError(status == PSA_SUCCESS, CHIP_ERROR_INTERNAL);

    return CHIP_NO_ERROR;
}

CHIP_ERROR PSASessionKeystore::CreateKey(const Symmetric128BitsKeyByteArray & keyMaterial, Hmac128KeyHandle & key)
{
    // Destroy the old key if already allocated
    DestroyKey(key);

    HmacKeyAttributes attrs;
    psa_status_t status =
        psa_import_key(&attrs.Get(), keyMaterial, sizeof(Symmetric128BitsKeyByteArray), &key.AsMutable<psa_key_id_t>());
    LogPsaError(status);
    VerifyOrReturnError(status == PSA_SUCCESS, CHIP_ERROR_INTERNAL);

    return CHIP_NO_ERROR;
}

CHIP_ERROR PSASessionKeystore::CreateKey(const ByteSpan & keyMaterial, HkdfKeyHandle & key)
{
    // Destroy the old key if already allocated
    DestroyKey(key);

#ifdef CHIP_CRYPTO_PSA_HKDF_IMPORT_WORKAROUND
    // TI CC35XX workaround: psa_import_key with PSA_KEY_TYPE_DERIVE is not
    // supported by the HSM DDK. Store the raw key bytes and use them directly
    // with the HKDF_EXTRACT+HKDF_EXPAND path in DeriveSessionKeys.
    VerifyOrReturnError(keyMaterial.size() <= sizeof(sHkdfRawKeyBytes), CHIP_ERROR_INVALID_ARGUMENT);
    memcpy(sHkdfRawKeyBytes, keyMaterial.data(), keyMaterial.size());
    sHkdfRawKeyLen                = keyMaterial.size();
    key.AsMutable<psa_key_id_t>() = kHkdfRawKeyId;
    return CHIP_NO_ERROR;
#else
    HkdfKeyAttributes attrs;
    psa_status_t status = psa_import_key(&attrs.Get(), keyMaterial.data(), keyMaterial.size(), &key.AsMutable<psa_key_id_t>());
    LogPsaError(status);
    VerifyOrReturnError(status == PSA_SUCCESS, CHIP_ERROR_INTERNAL);
    return CHIP_NO_ERROR;
#endif
}

CHIP_ERROR PSASessionKeystore::DeriveKey(const P256ECDHDerivedSecret & secret, const ByteSpan & salt, const ByteSpan & info,
                                         Aes128KeyHandle & key)
{
#ifdef CHIP_CRYPTO_PSA_HKDF_IMPORT_WORKAROUND
    {
        // TI CC35XX: psa_key_derivation_output_key (HKDF_EXPAND) is unsupported by the HSM DDK.
        // Use SW HKDF (PSA hash streaming) to derive a single 16-byte AES key.
        constexpr size_t kKeyLen     = CHIP_CRYPTO_SYMMETRIC_KEY_LENGTH_BYTES; // 16
        constexpr size_t kMaxInfoLen = 32;                                     // "Sigma2"/"Sigma3" = 6 bytes
        constexpr size_t kMaxSaltLen = 160; // Sigma2/3 salt = IPK(16)+rand(32)+pubkey(65)+hash(32) = 145 bytes
        VerifyOrReturnError(info.size() <= kMaxInfoLen, CHIP_ERROR_INVALID_ARGUMENT);
        VerifyOrReturnError(salt.size() <= kMaxSaltLen, CHIP_ERROR_INVALID_ARGUMENT);

        // Copy info/salt to RAM: TI HSM DMA cannot access flash (.rodata, 0x14xxxxxx)
        uint8_t info_ram[kMaxInfoLen];
        uint8_t salt_ram[kMaxSaltLen];
        memcpy(info_ram, info.data(), info.size());
        if (salt.size() > 0)
            memcpy(salt_ram, salt.data(), salt.size());

        // ECDH secret is already in RAM (P256ECDHDerivedSecret lives on stack/heap)
        uint8_t okm[kKeyLen];
        psa_status_t psaStatus = sw_hkdf_sha256(salt.size() > 0 ? salt_ram : nullptr, salt.size(), secret.ConstBytes(),
                                                secret.Length(), info_ram, info.size(), okm, kKeyLen);
        if (psaStatus != PSA_SUCCESS)
        {
            ChipLogError(Crypto, "DeriveKey: sw_hkdf_sha256 failed %d", (int) psaStatus);
            LogPsaError(psaStatus);
            memset(okm, 0, kKeyLen);
            return CHIP_ERROR_INTERNAL;
        }

        AesKeyAttributes attrs;
        psa_status_t status = psa_import_key(&attrs.Get(), okm, kKeyLen, &key.AsMutable<psa_key_id_t>());
        memset(okm, 0, kKeyLen);
        LogPsaError(status);
        VerifyOrReturnError(status == PSA_SUCCESS, CHIP_ERROR_INTERNAL);
        return CHIP_NO_ERROR;
    }
#else
    PsaKdf kdf;
    ReturnErrorOnFailure(kdf.Init(secret.Span(), salt, info));

    AesKeyAttributes attrs;

    return kdf.DeriveKey(attrs.Get(), key.AsMutable<psa_key_id_t>());
#endif
}

CHIP_ERROR PSASessionKeystore::DeriveSessionKeys(const ByteSpan & secret, const ByteSpan & salt, const ByteSpan & info,
                                                 Aes128KeyHandle & i2rKey, Aes128KeyHandle & r2iKey,
                                                 AttestationChallenge & attestationChallenge)
{
#ifdef CHIP_CRYPTO_PSA_HKDF_IMPORT_WORKAROUND
    {
        // TI CC35XX: psa_key_derivation_output_key (HKDF_EXPAND) is unsupported by the HSM DDK.
        // Use SW HKDF (PSA hash streaming) to derive OKM, then import AES keys directly.
        constexpr size_t kKeyLen     = CHIP_CRYPTO_SYMMETRIC_KEY_LENGTH_BYTES;         // 16
        constexpr size_t kOkmLen     = kKeyLen * 2 + AttestationChallenge::Capacity(); // 48
        constexpr size_t kMaxInfoLen = 32; // "SessionKeys"=11, "SessionResumptionKeys"=21
        constexpr size_t kMaxSaltLen = 64; // CASE final session: IPK(16)+digest(32)=48
        VerifyOrReturnError(info.size() <= kMaxInfoLen, CHIP_ERROR_INVALID_ARGUMENT);
        VerifyOrReturnError(salt.size() <= kMaxSaltLen, CHIP_ERROR_INVALID_ARGUMENT);

        // Copy info/salt to RAM: TI HSM DMA cannot access flash (.rodata, 0x14xxxxxx)
        uint8_t info_ram[kMaxInfoLen];
        uint8_t salt_ram[kMaxSaltLen];
        memcpy(info_ram, info.data(), info.size());
        if (salt.size() > 0)
            memcpy(salt_ram, salt.data(), salt.size());

        uint8_t okm[kOkmLen];
        psa_status_t psaStatus = sw_hkdf_sha256(salt.size() > 0 ? salt_ram : nullptr, salt.size(), secret.data(), secret.size(),
                                                info_ram, info.size(), okm, kOkmLen);
        if (psaStatus != PSA_SUCCESS)
        {
            ChipLogError(Crypto, "DeriveSessionKeys(ByteSpan): sw_hkdf_sha256 failed %d", (int) psaStatus);
            LogPsaError(psaStatus);
            memset(okm, 0, kOkmLen);
            return CHIP_ERROR_INTERNAL;
        }

        AesKeyAttributes attrs;
        psa_status_t status;

        status = psa_import_key(&attrs.Get(), okm, kKeyLen, &i2rKey.AsMutable<psa_key_id_t>());
        if (status != PSA_SUCCESS)
        {
            ChipLogError(Crypto, "DeriveSessionKeys(ByteSpan): psa_import_key i2rKey failed %d", (int) status);
            LogPsaError(status);
            memset(okm, 0, kOkmLen);
            return CHIP_ERROR_INTERNAL;
        }

        status = psa_import_key(&attrs.Get(), okm + kKeyLen, kKeyLen, &r2iKey.AsMutable<psa_key_id_t>());
        if (status != PSA_SUCCESS)
        {
            ChipLogError(Crypto, "DeriveSessionKeys(ByteSpan): psa_import_key r2iKey failed %d", (int) status);
            LogPsaError(status);
            DestroyKey(i2rKey);
            memset(okm, 0, kOkmLen);
            return CHIP_ERROR_INTERNAL;
        }

        memcpy(attestationChallenge.Bytes(), okm + kKeyLen * 2, AttestationChallenge::Capacity());
        memset(okm, 0, kOkmLen);
        return CHIP_NO_ERROR;
    }
#else
    PsaKdf kdf;
    ReturnErrorOnFailure(kdf.Init(secret, salt, info));

    return DeriveSessionKeys(kdf, i2rKey, r2iKey, attestationChallenge);
#endif
}

CHIP_ERROR PSASessionKeystore::DeriveSessionKeys(const HkdfKeyHandle & hkdfKey, const ByteSpan & salt, const ByteSpan & info,
                                                 Aes128KeyHandle & i2rKey, Aes128KeyHandle & r2iKey,
                                                 AttestationChallenge & attestationChallenge)
{
#ifdef CHIP_CRYPTO_PSA_HKDF_IMPORT_WORKAROUND
    if (hkdfKey.As<psa_key_id_t>() == kHkdfRawKeyId)
    {
        // TI CC35XX workaround: PSA key derivation (HKDF_EXTRACT, HKDF_EXPAND,
        // psa_key_derivation_input_key) are all unsupported by the TI DDK HSM.
        // Use SW HKDF (PSA hash streaming) to compute OKM, import slices as PSA AES keys.
        VerifyOrReturnError(sHkdfRawKeyLen > 0, CHIP_ERROR_INCORRECT_STATE);

        constexpr size_t kKeyLen = CHIP_CRYPTO_SYMMETRIC_KEY_LENGTH_BYTES;         // 16
        constexpr size_t kOkmLen = kKeyLen * 2 + AttestationChallenge::Capacity(); // 48
        uint8_t okm[kOkmLen];

        // The TI HSM DMA cannot access .rodata (flash, 0x14xxxxxx address space).
        // info and salt may point into flash (e.g. SEKeysInfo / RSEKeysInfo are .rodata
        // constants in CryptoContext.cpp).  Copy them to stack RAM so that the
        // subsequent psa_hash_update() calls inside sw_hkdf_sha256 only see RAM pointers.
        constexpr size_t kMaxInfoLen = 32; // SEKeysInfo=11, RSEKeysInfo=21
        constexpr size_t kMaxSaltLen = 64;
        VerifyOrReturnError(info.size() <= kMaxInfoLen, CHIP_ERROR_INVALID_ARGUMENT);
        VerifyOrReturnError(salt.size() <= kMaxSaltLen, CHIP_ERROR_INVALID_ARGUMENT);
        uint8_t info_ram[kMaxInfoLen];
        uint8_t salt_ram[kMaxSaltLen];
        memcpy(info_ram, info.data(), info.size());
        if (salt.size() > 0)
            memcpy(salt_ram, salt.data(), salt.size());

        psa_status_t psaStatus = sw_hkdf_sha256(salt.size() > 0 ? salt_ram : nullptr, salt.size(), sHkdfRawKeyBytes, sHkdfRawKeyLen,
                                                info_ram, info.size(), okm, sizeof(okm));
        if (psaStatus != PSA_SUCCESS)
        {
            ChipLogError(Crypto, "DeriveSessionKeys: sw_hkdf_sha256 failed PSA status %d", (int) psaStatus);
            LogPsaError(psaStatus);
            memset(okm, 0, sizeof(okm));
            return CHIP_ERROR_INTERNAL;
        }

        AesKeyAttributes attrs;
        psa_status_t status;

        status = psa_import_key(&attrs.Get(), okm, kKeyLen, &i2rKey.AsMutable<psa_key_id_t>());
        if (status != PSA_SUCCESS)
        {
            ChipLogError(Crypto, "DeriveSessionKeys: psa_import_key i2rKey failed %d", (int) status);
            LogPsaError(status);
            memset(okm, 0, sizeof(okm));
            return CHIP_ERROR_INTERNAL;
        }

        status = psa_import_key(&attrs.Get(), okm + kKeyLen, kKeyLen, &r2iKey.AsMutable<psa_key_id_t>());
        if (status != PSA_SUCCESS)
        {
            ChipLogError(Crypto, "DeriveSessionKeys: psa_import_key r2iKey failed %d", (int) status);
            LogPsaError(status);
            DestroyKey(i2rKey);
            memset(okm, 0, sizeof(okm));
            return CHIP_ERROR_INTERNAL;
        }

        memcpy(attestationChallenge.Bytes(), okm + kKeyLen * 2, AttestationChallenge::Capacity());
        memset(okm, 0, sizeof(okm));
        return CHIP_NO_ERROR;
    }
#endif // CHIP_CRYPTO_PSA_HKDF_IMPORT_WORKAROUND

    PsaKdf kdf;
    ReturnErrorOnFailure(kdf.Init(hkdfKey, salt, info));

    return DeriveSessionKeys(kdf, i2rKey, r2iKey, attestationChallenge);
}

CHIP_ERROR PSASessionKeystore::DeriveSessionKeys(PsaKdf & kdf, Aes128KeyHandle & i2rKey, Aes128KeyHandle & r2iKey,
                                                 AttestationChallenge & attestationChallenge)
{
    CHIP_ERROR error;
    AesKeyAttributes attrs;

    SuccessOrExit(error = kdf.DeriveKey(attrs.Get(), i2rKey.AsMutable<psa_key_id_t>()));
    SuccessOrExit(error = kdf.DeriveKey(attrs.Get(), r2iKey.AsMutable<psa_key_id_t>()));
    SuccessOrExit(error = kdf.DeriveBytes(MutableByteSpan(attestationChallenge.Bytes(), AttestationChallenge::Capacity())));

exit:
    if (error != CHIP_NO_ERROR)
    {
        DestroyKey(i2rKey);
        DestroyKey(r2iKey);
    }

    return error;
}

void PSASessionKeystore::DestroyKey(Symmetric128BitsKeyHandle & key)
{
    auto & keyId = key.AsMutable<psa_key_id_t>();

    psa_destroy_key(keyId);
    keyId = 0;
}

void PSASessionKeystore::DestroyKey(HkdfKeyHandle & key)
{
    auto & keyId = key.AsMutable<psa_key_id_t>();

#ifdef CHIP_CRYPTO_PSA_HKDF_IMPORT_WORKAROUND
    if (keyId == kHkdfRawKeyId)
    {
        memset(sHkdfRawKeyBytes, 0, sizeof(sHkdfRawKeyBytes));
        sHkdfRawKeyLen = 0;
        keyId          = PSA_KEY_ID_NULL;
        return;
    }
#endif // CHIP_CRYPTO_PSA_HKDF_IMPORT_WORKAROUND

    psa_destroy_key(keyId);
    keyId = PSA_KEY_ID_NULL;
}

#if CHIP_CONFIG_ENABLE_ICD_CIP
CHIP_ERROR PSASessionKeystore::PersistICDKey(Symmetric128BitsKeyHandle & key)
{
    CHIP_ERROR err             = CHIP_NO_ERROR;
    psa_key_id_t newKeyId      = PSA_KEY_ID_NULL;
    psa_key_attributes_t attrs = PSA_KEY_ATTRIBUTES_INIT;

    psa_get_key_attributes(key.As<psa_key_id_t>(), &attrs);

    // Exit early if key is already persistent
    if (psa_get_key_lifetime(&attrs) == PSA_KEY_LIFETIME_PERSISTENT)
    {
        psa_reset_key_attributes(&attrs);
        return CHIP_NO_ERROR;
    }

    newKeyId = GetPSAKeyAllocator().AllocateICDKeyId();
    VerifyOrExit(PSA_KEY_ID_NULL != newKeyId, err = CHIP_ERROR_INTERNAL);

    psa_set_key_lifetime(&attrs, PSA_KEY_LIFETIME_PERSISTENT);
    psa_set_key_id(&attrs, newKeyId);
    GetPSAKeyAllocator().UpdateKeyAttributes(attrs);

    VerifyOrExit(psa_copy_key(key.As<psa_key_id_t>(), &attrs, &newKeyId) == PSA_SUCCESS, err = CHIP_ERROR_INTERNAL);

exit:
    DestroyKey(key);
    psa_reset_key_attributes(&attrs);

    if (err == CHIP_NO_ERROR)
    {
        SetKeyId(key, newKeyId);
    }

    return err;
}
#endif

} // namespace Crypto
} // namespace chip
