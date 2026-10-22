/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Evgeny Grin (Karlson2k), <k2k@narod.ru>.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * SPDX-License-Identifier: curl
 *
 ***************************************************************************/
#समावेश "curl_setup.h"

#अगर !defined(CURL_DISABLE_DIGEST_AUTH) && !defined(CURL_DISABLE_SHA512_256)

#समावेश "curl_sha512_256.h"

/* The recommended order of the TLS backends:
 * 1. USE_OPENSSL
 * 2. USE_WOLFSSL
 * 3. USE_GNUTLS
 * 4. USE_MBEDTLS (TBD)
 * 5. USE_RUSTLS (TBD)
 * 6. USE_WIN32_CRYPTO (TBD)
 * Skip the backend if it does not support the required algorithm */

#अगर_घोषित USE_OPENSSL
#  include <openssl/opensslv.h>
#  अगर !defined(LIBRESSL_VERSION_संख्या) || \
  (defined(LIBRESSL_VERSION_संख्या) && LIBRESSL_VERSION_संख्या >= 0x3080000fL)
#    include <openssl/evp.h>
#    define USE_OPENSSL_SHA512_256          1
#    define HAS_SHA512_256_IMPLEMENTATION   1
#    ifdef __NetBSD__
/* Some NetBSD versions have a bug in SHA-512/256.
 * See https://gnats.netbsd.org/cgi-bin/query-pr-single.pl?number=58039
 * The problematic versions:
 * - NetBSD before 9.4
 * - NetBSD 9 all development versions (9.99.x)
 * - NetBSD 10 development versions (10.99.x) before 10.99.11
 * The bug was fixed in NetBSD 9.4 release, NetBSD 10.0 release,
 * NetBSD 10.99.11 development.
 * It is safe to apply the workaround even if the bug is not present, as
 * the workaround reduces performance slightly. */
#      अगर  __NetBSD_Version__ <   904000000 ||  \
          (__NetBSD_Version__ >=  999000000 &&  \
           __NetBSD_Version__ <  1000000000) || \
          (__NetBSD_Version__ >= 1099000000 &&  \
           __NetBSD_Version__ <  1099001100)
#        define NEED_NETBSD_SHA512_256_WORKAROUND 1
#      endif
#    endif
#  endif
#पूर्ण_अगर /* USE_OPENSSL */

#अगर !defined(HAS_SHA512_256_IMPLEMENTATION) && defined(USE_WOLFSSL)
#  include <wolfssl/options.h>
#  ifndef WOLFSSL_NOSHA512_256
#    define USE_WOLFSSL_SHA512_256          1
#    define HAS_SHA512_256_IMPLEMENTATION   1
#  endif
#पूर्ण_अगर

#अगर !defined(HAS_SHA512_256_IMPLEMENTATION) && defined(USE_GNUTLS)
#  include <nettle/sha2.h>
#  include <nettle/version.h>
#  ifdef SHA512_256_DIGEST_आकार
#    define USE_GNUTLS_SHA512_256           1
#  endif
#पूर्ण_अगर /* !HAS_SHA512_256_IMPLEMENTATION && USE_GNUTLS */

#अगर_घोषित USE_OPENSSL_SHA512_256

/* OpenSSL does not provide macros for SHA-512/256 sizes */

/**
 * Size of the SHA-512/256 single processing block in bytes.
 */
#घोषणा CURL_SHA512_256_BLOCK_आकार 128

/**
 * Size of the SHA-512/256 resulting digest in bytes.
 * This is the final digest size, not intermediate hash.
 */
#घोषणा CURL_SHA512_256_DIGEST_आकार CURL_SHA512_256_DIGEST_लंबाई

/**
 * Context type used for SHA-512/256 calculations
 */
प्रकार EVP_MD_CTX *Curl_sha512_256_ctx;

/**
 * Initialize structure for SHA-512/256 calculation.
 *
 * @param context the calculation context
 * @return CURLE_OK if succeed,
 *         error code otherwise
 */
अटल CURLcode Curl_sha512_256_init(व्योम *context)
{
  Curl_sha512_256_ctx * स्थिर ctx = (Curl_sha512_256_ctx *)context;

  *ctx = EVP_MD_CTX_new();
  अगर(!*ctx)
    वापस CURLE_OUT_OF_MEMORY;

  अगर(EVP_DigestInit_ex(*ctx, EVP_sha512_256(), शून्य)) {
    /* Check whether the header and this file use the same numbers */
    DEBUGASSERT(EVP_MD_CTX_आकार(*ctx) == CURL_SHA512_256_DIGEST_आकार);
    /* Check whether the block size is correct */
    DEBUGASSERT(EVP_MD_CTX_block_आकार(*ctx) == CURL_SHA512_256_BLOCK_आकार);

    वापस CURLE_OK; /* Success */
  }

  /* Cleanup */
  EVP_MD_CTX_free(*ctx);
  वापस CURLE_FAILED_INIT;
}

/**
 * Process portion of bytes.
 *
 * @param context the calculation context
 * @param data bytes to add to hash
 * @return CURLE_OK if succeed,
 *         error code otherwise
 */
अटल CURLcode Curl_sha512_256_update(व्योम *context,
                                       स्थिर अचिन्हित अक्षर *तथ्य,
                                       माप_प्रकार लंबाई)
{
  Curl_sha512_256_ctx * स्थिर ctx = (Curl_sha512_256_ctx *)context;

  अगर(!EVP_DigestUpdate(*ctx, तथ्य, लंबाई))
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  वापस CURLE_OK;
}

/**
 * Finalise SHA-512/256 calculation, return digest.
 *
 * @param context the calculation context
 * @param[out] digest set to the hash, must be #CURL_SHA512_256_DIGEST_SIZE
 *             bytes
 * @return CURLE_OK if succeed,
 *         error code otherwise
 */
अटल CURLcode Curl_sha512_256_finish(अचिन्हित अक्षर *digest, व्योम *context)
{
  CURLcode परिणाम;
  Curl_sha512_256_ctx * स्थिर ctx = (Curl_sha512_256_ctx *)context;

#अगर_घोषित NEED_NETBSD_SHA512_256_WORKAROUND
  /* Use a larger buffer to work around a bug in NetBSD:
     https://gnats.netbsd.org/cgi-bin/query-pr-single.pl?number=58039 */
  अचिन्हित अक्षर tmp_digest[CURL_SHA512_256_DIGEST_आकार * 2];
  परिणाम = EVP_DigestFinal_ex(*ctx, tmp_digest, शून्य) ?
    CURLE_OK : CURLE_BAD_FUNCTION_ARGUMENT;
  अगर(परिणाम == CURLE_OK)
    स_नकल(digest, tmp_digest, CURL_SHA512_256_DIGEST_आकार);
  curlx_memzero(tmp_digest, माप(tmp_digest));
#अन्यथा /* !NEED_NETBSD_SHA512_256_WORKAROUND */
  परिणाम = EVP_DigestFinal_ex(*ctx, digest, शून्य) ?
    CURLE_OK : CURLE_BAD_FUNCTION_ARGUMENT;
#पूर्ण_अगर /* NEED_NETBSD_SHA512_256_WORKAROUND */

  EVP_MD_CTX_free(*ctx);
  *ctx = शून्य;

  वापस परिणाम;
}

#या_अगर defined(USE_WOLFSSL_SHA512_256)
#समावेश <wolfssl/wolfcrypt/sha512.h>

#घोषणा CURL_SHA512_256_DIGEST_आकार WC_SHA512_256_DIGEST_आकार
#घोषणा CURL_SHA512_256_BLOCK_आकार  WC_SHA512_256_BLOCK_आकार

प्रकार काष्ठा wc_Sha512 Curl_sha512_256_ctx;

अटल CURLcode Curl_sha512_256_init(व्योम *ctx)
{
  अगर(wc_InitSha512_256(ctx))
    वापस CURLE_FAILED_INIT;
  वापस CURLE_OK;
}

अटल CURLcode Curl_sha512_256_update(व्योम *ctx,
                                       स्थिर अचिन्हित अक्षर *तथ्य,
                                       माप_प्रकार लंबाई)
{
  करो {
    word32 ilen = (word32)CURLMIN(लंबाई, अच_पूर्णांक_उच्च);
    अगर(wc_Sha512_256Update(ctx, तथ्य, ilen))
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    लंबाई -= ilen;
    तथ्य += ilen;
  } जबतक(लंबाई);
  वापस CURLE_OK;
}

अटल CURLcode Curl_sha512_256_finish(अचिन्हित अक्षर *digest, व्योम *ctx)
{
  अगर(wc_Sha512_256Final(ctx, digest))
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  वापस CURLE_OK;
}

#या_अगर defined(USE_GNUTLS_SHA512_256)

#घोषणा CURL_SHA512_256_BLOCK_आकार  SHA512_256_BLOCK_आकार
#घोषणा CURL_SHA512_256_DIGEST_आकार SHA512_256_DIGEST_आकार

/**
 * Context type used for SHA-512/256 calculations
 */
प्रकार काष्ठा sha512_256_ctx Curl_sha512_256_ctx;

/**
 * Initialize structure for SHA-512/256 calculation.
 *
 * @param context the calculation context
 * @return always CURLE_OK
 */
अटल CURLcode Curl_sha512_256_init(व्योम *context)
{
  Curl_sha512_256_ctx * स्थिर ctx = (Curl_sha512_256_ctx *)context;

  /* Check whether the header and this file use the same numbers */
  DEBUGASSERT(CURL_SHA512_256_DIGEST_लंबाई == CURL_SHA512_256_DIGEST_आकार);

  sha512_256_init(ctx);

  वापस CURLE_OK;
}

/**
 * Process portion of bytes.
 *
 * @param context the calculation context
 * @param data bytes to add to hash
 * @param length number of bytes in @a data
 * @return always CURLE_OK
 */
अटल CURLcode Curl_sha512_256_update(व्योम *context,
                                       स्थिर अचिन्हित अक्षर *तथ्य,
                                       माप_प्रकार लंबाई)
{
  Curl_sha512_256_ctx * स्थिर ctx = (Curl_sha512_256_ctx *)context;

  DEBUGASSERT(तथ्य || (लंबाई == 0));

  sha512_256_update(ctx, लंबाई, (स्थिर uint8_t *)तथ्य);

  वापस CURLE_OK;
}

/**
 * Finalise SHA-512/256 calculation, return digest.
 *
 * @param context the calculation context
 * @param[out] digest set to the hash, must be #CURL_SHA512_256_DIGEST_SIZE
 *             bytes
 * @return always CURLE_OK
 */
अटल CURLcode Curl_sha512_256_finish(अचिन्हित अक्षर *digest, व्योम *context)
{
  Curl_sha512_256_ctx * स्थिर ctx = (Curl_sha512_256_ctx *)context;

#अगर NETTLE_VERSION_MAJOR >= 4
  sha512_256_digest(ctx, (uint8_t *)digest);
#अन्यथा
  sha512_256_digest(ctx,
                    (माप_प्रकार)CURL_SHA512_256_DIGEST_आकार, (uint8_t *)digest);
#पूर्ण_अगर

  वापस CURLE_OK;
}

#अन्यथा /* No system or TLS backend SHA-512/256 implementation available */

/* ** This implementation of SHA-512/256 hash calculation was originally ** *
 * ** written by Evgeny Grin (Karlson2k) for GNU libmicrohttpd.          ** *
 * ** The author ported the code to libcurl. The ported code is provided ** *
 * ** under curl license.                                                ** */

#अगर_घोषित __GNUC__
#  अगर defined(__has_attribute) && defined(__STDC_VERSION__)
#    अगर __has_attribute(always_inline) && __STDC_VERSION__ >= 199901L
#      define CURL_FORCEINLINE CURL_INLINE __attribute__((always_inline))
#    endif
#  endif
#पूर्ण_अगर

#अगर !defined(CURL_FORCEINLINE) && \
  defined(_MSC_VER) && !defined(__GNUC__) && !defined(__clang__)
#घोषणा CURL_FORCEINLINE __forceinline
#पूर्ण_अगर

/* Assume that 'CURL_INLINE' keyword works or the
 * macro was already defined correctly. */
#अगर_अघोषित CURL_FORCEINLINE
#घोषणा CURL_FORCEINLINE CURL_INLINE
#पूर्ण_अगर

/* Bits manipulation macros and functions.
   Can be moved to other headers to reuse. */

#घोषणा CURL_GET_64BIT_BE(ptr)                       \
  (((uint64_t)(((स्थिर uint8_t *)(ptr))[0]) << 56) | \
   ((uint64_t)(((स्थिर uint8_t *)(ptr))[1]) << 48) | \
   ((uint64_t)(((स्थिर uint8_t *)(ptr))[2]) << 40) | \
   ((uint64_t)(((स्थिर uint8_t *)(ptr))[3]) << 32) | \
   ((uint64_t)(((स्थिर uint8_t *)(ptr))[4]) << 24) | \
   ((uint64_t)(((स्थिर uint8_t *)(ptr))[5]) << 16) | \
   ((uint64_t)(((स्थिर uint8_t *)(ptr))[6]) << 8)  | \
    (uint64_t)(((स्थिर uint8_t *)(ptr))[7]))

#घोषणा CURL_PUT_64BIT_BE(ptr, val)                             \
  करो {                                                          \
    ((uint8_t *)(ptr))[7] = (uint8_t)((uint64_t)(val));         \
    ((uint8_t *)(ptr))[6] = (uint8_t)(((uint64_t)(val)) >> 8);  \
    ((uint8_t *)(ptr))[5] = (uint8_t)(((uint64_t)(val)) >> 16); \
    ((uint8_t *)(ptr))[4] = (uint8_t)(((uint64_t)(val)) >> 24); \
    ((uint8_t *)(ptr))[3] = (uint8_t)(((uint64_t)(val)) >> 32); \
    ((uint8_t *)(ptr))[2] = (uint8_t)(((uint64_t)(val)) >> 40); \
    ((uint8_t *)(ptr))[1] = (uint8_t)(((uint64_t)(val)) >> 48); \
    ((uint8_t *)(ptr))[0] = (uint8_t)(((uint64_t)(val)) >> 56); \
  } जबतक(0)

/* Defined as a function. The macro version may duplicate the binary code
 * size as each argument is used twice, so if any calculation is used
 * as an argument, the calculation could be done twice. */
अटल CURL_FORCEINLINE uint64_t Curl_rotr64(uint64_t मूल्य, अचिन्हित पूर्णांक bits)
{
  bits %= 64;
  अगर(bits == 0)
    वापस मूल्य;
  /* Defined in a form which modern compiler could optimize. */
  वापस (मूल्य >> bits) | (मूल्य << (64 - bits));
}

/* SHA-512/256 specific data */

/**
 * Number of bits in a single SHA-512/256 word.
 */
#घोषणा SHA512_256_WORD_आकार_BITS 64

/**
 * Number of bytes in a single SHA-512/256 word.
 */
#घोषणा SHA512_256_BYTES_IN_WORD (SHA512_256_WORD_आकार_BITS / 8)

/**
 * Hash is kept internally as 8 64-bit words.
 * This is the intermediate hash size, used during computing the final digest.
 */
#घोषणा SHA512_256_HASH_आकार_शब्दगण 8

/**
 * Size of the SHA-512/256 resulting digest in words.
 * This is the final digest size, not intermediate hash.
 */
#घोषणा SHA512_256_DIGEST_आकार_शब्दगण (SHA512_256_HASH_आकार_शब्दगण / 2)

/**
 * Size of the SHA-512/256 resulting digest in bytes
 * This is the final digest size, not intermediate hash.
 */
#घोषणा CURL_SHA512_256_DIGEST_आकार \
  (SHA512_256_DIGEST_आकार_शब्दगण * SHA512_256_BYTES_IN_WORD)

/**
 * Size of the SHA-512/256 single processing block in bits.
 */
#घोषणा SHA512_256_BLOCK_आकार_BITS 1024

/**
 * Size of the SHA-512/256 single processing block in bytes.
 */
#घोषणा CURL_SHA512_256_BLOCK_आकार (SHA512_256_BLOCK_आकार_BITS / 8)

/**
 * Size of the SHA-512/256 single processing block in words.
 */
#घोषणा SHA512_256_BLOCK_आकार_शब्दगण \
  (SHA512_256_BLOCK_आकार_BITS / SHA512_256_WORD_आकार_BITS)

/**
 * SHA-512/256 calculation context
 */
काष्ठा Curl_sha512_256ctx {
  /**
   * Intermediate hash value. The variable is properly aligned. Smart
   * compilers may automatically use fast load/store instruction for big
   * endian data on little endian machine.
   */
  uint64_t H[SHA512_256_HASH_आकार_शब्दगण];
  /**
   * SHA-512/256 input data buffer. The buffer is properly aligned. Smart
   * compilers may automatically use fast load/store instruction for big
   * endian data on little endian machine.
   */
  uint64_t बफर[SHA512_256_BLOCK_आकार_शब्दगण];
  /**
   * The number of bytes, lower part
   */
  uint64_t गिनती;
  /**
   * The number of bits, high part. Unlike lower part, this counts the number
   * of bits, not bytes.
   */
  uint64_t गिनती_bits_hi;
};

/**
 * Context type used for SHA-512/256 calculations
 */
प्रकार काष्ठा Curl_sha512_256ctx Curl_sha512_256_ctx;

/**
 * Initialize structure for SHA-512/256 calculation.
 *
 * @param context the calculation context
 * @return always CURLE_OK
 */
अटल CURLcode Curl_sha512_256_init(व्योम *context)
{
  काष्ठा Curl_sha512_256ctx * स्थिर ctx = (काष्ठा Curl_sha512_256ctx *)context;

  /* Check whether the header and this file use the same numbers */
  DEBUGASSERT(CURL_SHA512_256_DIGEST_लंबाई == CURL_SHA512_256_DIGEST_आकार);

  DEBUGASSERT(माप(uint64_t) == 8);

  /* Initial hash values, see FIPS PUB 180-4 section 5.3.6.2 */
  /* Values generated by "IV Generation Function" as described in
   * section 5.3.6 */
  ctx->H[0] = UINT64_C(0x22312194FC2BF72C);
  ctx->H[1] = UINT64_C(0x9F555FA3C84C64C2);
  ctx->H[2] = UINT64_C(0x2393B86B6F53B151);
  ctx->H[3] = UINT64_C(0x963877195940EABD);
  ctx->H[4] = UINT64_C(0x96283EE2A88EFFE3);
  ctx->H[5] = UINT64_C(0xBE5E1E2553863992);
  ctx->H[6] = UINT64_C(0x2B0199FC2C85B8AA);
  ctx->H[7] = UINT64_C(0x0EB72DDC81C52CA2);

  /* Initialize number of bytes and high part of number of bits. */
  ctx->गिनती = UINT64_C(0);
  ctx->गिनती_bits_hi = UINT64_C(0);

  वापस CURLE_OK;
}

/**
 * Base of the SHA-512/256 transformation.
 * Gets a full 128 bytes block of data and updates hash values;
 * @param H     hash values
 * @param data  the data buffer with #CURL_SHA512_256_BLOCK_SIZE bytes block
 */
अटल व्योम Curl_sha512_256_transform(uint64_t H[SHA512_256_HASH_आकार_शब्दगण],
                                      स्थिर व्योम *तथ्य)
{
  /* Working variables,
     see FIPS PUB 180-4 section 6.7, 6.4. */
  uint64_t a = H[0];
  uint64_t b = H[1];
  uint64_t c = H[2];
  uint64_t d = H[3];
  uint64_t e = H[4];
  uint64_t f = H[5];
  uint64_t g = H[6];
  uint64_t h = H[7];

  /* Data buffer, used as a cyclic buffer.
     See FIPS PUB 180-4 section 5.2.2, 6.7, 6.4. */
  uint64_t W[16];

  /* 'Ch' and 'Maj' macro functions are defined with widely-used optimization.
     See FIPS PUB 180-4 formulae 4.8, 4.9. */
#घोषणा Sha512_Ch(x, y, z)    ((z) ^ ((x) & ((y) ^ (z))))
#घोषणा Sha512_Maj(x, y, z)   (((x) & (y)) ^ ((z) & ((x) ^ (y))))

  /* Four 'Sigma' macro functions.
     See FIPS PUB 180-4 formulae 4.10, 4.11, 4.12, 4.13. */
#घोषणा SIG0(x) (Curl_rotr64(x, 28) ^ Curl_rotr64(x, 34) ^ Curl_rotr64(x, 39))
#घोषणा SIG1(x) (Curl_rotr64(x, 14) ^ Curl_rotr64(x, 18) ^ Curl_rotr64(x, 41))
#घोषणा sig0(x) (Curl_rotr64(x,  1) ^ Curl_rotr64(x,  8) ^ ((x) >> 7))
#घोषणा sig1(x) (Curl_rotr64(x, 19) ^ Curl_rotr64(x, 61) ^ ((x) >> 6))

  अगर(1) {
    अचिन्हित पूर्णांक t;
    /* K constants array.
       See FIPS PUB 180-4 section 4.2.3 for K values. */
    अटल स्थिर uint64_t K[80] = {
      UINT64_C(0x428a2f98d728ae22), UINT64_C(0x7137449123ef65cd),
      UINT64_C(0xb5c0fbcfec4d3b2f), UINT64_C(0xe9b5dba58189dbbc),
      UINT64_C(0x3956c25bf348b538), UINT64_C(0x59f111f1b605d019),
      UINT64_C(0x923f82a4af194f9b), UINT64_C(0xab1c5ed5da6d8118),
      UINT64_C(0xd807aa98a3030242), UINT64_C(0x12835b0145706fbe),
      UINT64_C(0x243185be4ee4b28c), UINT64_C(0x550c7dc3d5ffb4e2),
      UINT64_C(0x72be5d74f27b896f), UINT64_C(0x80deb1fe3b1696b1),
      UINT64_C(0x9bdc06a725c71235), UINT64_C(0xc19bf174cf692694),
      UINT64_C(0xe49b69c19ef14ad2), UINT64_C(0xefbe4786384f25e3),
      UINT64_C(0x0fc19dc68b8cd5b5), UINT64_C(0x240ca1cc77ac9c65),
      UINT64_C(0x2de92c6f592b0275), UINT64_C(0x4a7484aa6ea6e483),
      UINT64_C(0x5cb0a9dcbd41fbd4), UINT64_C(0x76f988da831153b5),
      UINT64_C(0x983e5152ee66dfab), UINT64_C(0xa831c66d2db43210),
      UINT64_C(0xb00327c898fb213f), UINT64_C(0xbf597fc7beef0ee4),
      UINT64_C(0xc6e00bf33da88fc2), UINT64_C(0xd5a79147930aa725),
      UINT64_C(0x06ca6351e003826f), UINT64_C(0x142929670a0e6e70),
      UINT64_C(0x27b70a8546d22ffc), UINT64_C(0x2e1b21385c26c926),
      UINT64_C(0x4d2c6dfc5ac42aed), UINT64_C(0x53380d139d95b3df),
      UINT64_C(0x650a73548baf63de), UINT64_C(0x766a0abb3c77b2a8),
      UINT64_C(0x81c2c92e47edaee6), UINT64_C(0x92722c851482353b),
      UINT64_C(0xa2bfe8a14cf10364), UINT64_C(0xa81a664bbc423001),
      UINT64_C(0xc24b8b70d0f89791), UINT64_C(0xc76c51a30654be30),
      UINT64_C(0xd192e819d6ef5218), UINT64_C(0xd69906245565a910),
      UINT64_C(0xf40e35855771202a), UINT64_C(0x106aa07032bbd1b8),
      UINT64_C(0x19a4c116b8d2d0c8), UINT64_C(0x1e376c085141ab53),
      UINT64_C(0x2748774cdf8eeb99), UINT64_C(0x34b0bcb5e19b48a8),
      UINT64_C(0x391c0cb3c5c95a63), UINT64_C(0x4ed8aa4ae3418acb),
      UINT64_C(0x5b9cca4f7763e373), UINT64_C(0x682e6ff3d6b2b8a3),
      UINT64_C(0x748f82ee5defb2fc), UINT64_C(0x78a5636f43172f60),
      UINT64_C(0x84c87814a1f0ab72), UINT64_C(0x8cc702081a6439ec),
      UINT64_C(0x90befffa23631e28), UINT64_C(0xa4506cebde82bde9),
      UINT64_C(0xbef9a3f7b2c67915), UINT64_C(0xc67178f2e372532b),
      UINT64_C(0xca273eceea26619c), UINT64_C(0xd186b8c721c0c207),
      UINT64_C(0xeada7dd6cde0eb1e), UINT64_C(0xf57d4f7fee6ed178),
      UINT64_C(0x06f067aa72176fba), UINT64_C(0x0a637dc5a2c898a6),
      UINT64_C(0x113f9804bef90dae), UINT64_C(0x1b710b35131c471b),
      UINT64_C(0x28db77f523047d84), UINT64_C(0x32caab7b40c72493),
      UINT64_C(0x3c9ebe0a15c9bebc), UINT64_C(0x431d67c49c100d4c),
      UINT64_C(0x4cc5d4becb3e42b6), UINT64_C(0x597f299cfc657e2a),
      UINT64_C(0x5fcb6fab3ad6faec), UINT64_C(0x6c44198c4a475817)
    };

    /* One step of SHA-512/256 computation,
       see FIPS PUB 180-4 section 6.4.2 step 3.
       * Note: this macro updates working variables in-place, without rotation.
       * Note: the first (vH += SIG1(vE) + Ch(vE,vF,vG) + kt + wt) equals T1 in
       FIPS PUB 180-4 section 6.4.2 step 3.
       the second (vH += SIG0(vA) + Maj(vE,vF,vC) equals T1 + T2 in
       FIPS PUB 180-4 section 6.4.2 step 3.
       * Note: 'wt' must be used exactly one time in this macro as macro for
       'wt' calculation may change other data as well every time when
       used. */
#घोषणा SHA2STEP64(vA, vB, vC, vD, vE, vF, vG, vH, kt, wt)                    \
  करो {                                                                        \
    (vD) += ((vH) += SIG1(vE) + Sha512_Ch(vE, vF, vG) + (kt) + (wt));         \
    (vH) += SIG0(vA) + Sha512_Maj(vA, vB, vC);                                \
  } जबतक(0)

    /* One step of SHA-512/256 computation with working variables rotation,
       see FIPS PUB 180-4 section 6.4.2 step 3. This macro version reassigns
       all working variables on each step. */
#घोषणा SHA2STEP64RV(vA, vB, vC, vD, vE, vF, vG, vH, kt, wt)                  \
  करो {                                                                        \
    uint64_t tmp_h_ = (vH);                                                   \
    SHA2STEP64(vA, vB, vC, vD, vE, vF, vG, tmp_h_, kt, wt);                   \
    (vH) = (vG);                                                              \
    (vG) = (vF);                                                              \
    (vF) = (vE);                                                              \
    (vE) = (vD);                                                              \
    (vD) = (vC);                                                              \
    (vC) = (vB);                                                              \
    (vB) = (vA);                                                              \
    (vA) = tmp_h_;                                                            \
  } जबतक(0)

    /* Get value of W(t) from input data buffer for 0 <= t <= 15,
       See FIPS PUB 180-4 section 6.2.
       Input data must be read in big-endian bytes order,
       see FIPS PUB 180-4 section 3.1.2. */
#घोषणा SHA512_GET_W_FROM_तथ्य(buf, t) \
  CURL_GET_64BIT_BE((स्थिर uint8_t *)(buf) + ((t) * SHA512_256_BYTES_IN_WORD))

    /* During first 16 steps, before making any calculation on each step, the
       W element is read from the input data buffer as a big-endian value and
       stored in the array of W elements. */
    क्रम(t = 0; t < 16; ++t) {
      SHA2STEP64RV(a, b, c, d, e, f, g, h, K[t],
                   W[t] = SHA512_GET_W_FROM_तथ्य(तथ्य, t));
    }

    /* 'W' generation and assignment for 16 <= t <= 79.
       See FIPS PUB 180-4 section 6.4.2.
       As only the last 16 'W' are used in calculations, it is possible to
       use 16 elements array of W as a cyclic buffer.
       Note: ((t-16) & 15) have same value as (t & 15) */
#घोषणा Wgen(w, t)                                               \
  (uint64_t)((w)[((t) - 16) & 15] + sig1((w)[((t) -  2) & 15]) + \
             (w)[((t) -  7) & 15] + sig0((w)[((t) - 15) & 15]))

    /* During the last 64 steps, before making any calculation on each step,
       current W element is generated from other W elements of the cyclic
       buffer and the generated value is stored back in the cyclic buffer. */
    क्रम(t = 16; t < 80; ++t) {
      SHA2STEP64RV(a, b, c, d, e, f, g, h, K[t],
                   W[t & 15] = Wgen(W, t));
    }
  }

  /* Compute and store the intermediate hash.
     See FIPS PUB 180-4 section 6.4.2 step 4. */
  H[0] += a;
  H[1] += b;
  H[2] += c;
  H[3] += d;
  H[4] += e;
  H[5] += f;
  H[6] += g;
  H[7] += h;
}

/**
 * Process portion of bytes.
 *
 * @param context the calculation context
 * @param data bytes to add to hash
 * @param length number of bytes in @a data
 * @return always CURLE_OK
 */
अटल CURLcode Curl_sha512_256_update(व्योम *context,
                                       स्थिर अचिन्हित अक्षर *तथ्य,
                                       माप_प्रकार लंबाई)
{
  अचिन्हित पूर्णांक bytes_have; /* Number of bytes in the context buffer */
  काष्ठा Curl_sha512_256ctx * स्थिर ctx = (काष्ठा Curl_sha512_256ctx *)context;
  /* the void pointer here is required to mute Intel compiler warning */
  व्योम * स्थिर ctx_buf = ctx->बफर;

  DEBUGASSERT(तथ्य || (लंबाई == 0));

  अगर(लंबाई == 0)
    वापस CURLE_OK; /* Shortcut, do nothing */

  /* Note: (count & (CURL_SHA512_256_BLOCK_SIZE - 1))
     equals (count % CURL_SHA512_256_BLOCK_SIZE) for this block size. */
  bytes_have = (अचिन्हित पूर्णांक)(ctx->गिनती & (CURL_SHA512_256_BLOCK_आकार - 1));
  ctx->गिनती += लंबाई;
  अगर(लंबाई > ctx->गिनती)
    ctx->गिनती_bits_hi += 1U << 3; /* Value wrap */
  ctx->गिनती_bits_hi += ctx->गिनती >> 61;
  ctx->गिनती &= UINT64_C(0x1FFFFFFFFFFFFFFF);

  अगर(bytes_have) {
    अचिन्हित पूर्णांक bytes_बायाँ = CURL_SHA512_256_BLOCK_आकार - bytes_have;
    अगर(लंबाई >= bytes_बायाँ) {
      /* Combine new data with data in the buffer and process the full
         block. */
      स_नकल((अचिन्हित अक्षर *)ctx_buf + bytes_have, तथ्य, bytes_बायाँ);
      तथ्य += bytes_बायाँ;
      लंबाई -= bytes_बायाँ;
      Curl_sha512_256_transform(ctx->H, ctx->बफर);
      bytes_have = 0;
    }
  }

  जबतक(CURL_SHA512_256_BLOCK_आकार <= लंबाई) {
    /* Process any full blocks of new data directly,
       without copying to the buffer. */
    Curl_sha512_256_transform(ctx->H, तथ्य);
    तथ्य += CURL_SHA512_256_BLOCK_आकार;
    लंबाई -= CURL_SHA512_256_BLOCK_आकार;
  }

  अगर(लंबाई) {
    /* Copy incomplete block of new data (if any)
       to the buffer. */
    स_नकल((अचिन्हित अक्षर *)ctx_buf + bytes_have, तथ्य, लंबाई);
  }

  वापस CURLE_OK;
}

/**
 * Size of "length" insertion in bits.
 * See FIPS PUB 180-4 section 5.1.2.
 */
#घोषणा SHA512_256_आकार_OF_LEN_ADD_BITS 128

/**
 * Size of "length" insertion in bytes.
 */
#घोषणा SHA512_256_आकार_OF_LEN_ADD (SHA512_256_आकार_OF_LEN_ADD_BITS / 8)

/**
 * Finalise SHA-512/256 calculation, return digest.
 *
 * @param context the calculation context
 * @param[out] digest set to the hash, must be #CURL_SHA512_256_DIGEST_SIZE
 *             bytes
 * @return always CURLE_OK
 */
अटल CURLcode Curl_sha512_256_finish(अचिन्हित अक्षर *digest, व्योम *context)
{
  काष्ठा Curl_sha512_256ctx * स्थिर ctx = (काष्ठा Curl_sha512_256ctx *)context;
  uint64_t अंक_bits; /* Number of processed bits */
  अचिन्हित पूर्णांक bytes_have; /* Number of bytes in the context buffer */
  /* the void pointer here is required to mute Intel compiler warning */
  व्योम * स्थिर ctx_buf = ctx->बफर;

  /* Memorise the number of processed bits.
     The padding and other data added here during the postprocessing must
     not change the amount of hashed data. */
  अंक_bits = ctx->गिनती << 3;

  /* Note: (count & (CURL_SHA512_256_BLOCK_SIZE-1))
           equals (count % CURL_SHA512_256_BLOCK_SIZE) for this block size. */
  bytes_have = (अचिन्हित पूर्णांक)(ctx->गिनती & (CURL_SHA512_256_BLOCK_आकार - 1));

  /* Input data must be padded with a single bit "1", then with zeros and
     the finally the length of data in bits must be added as the final bytes
     of the last block.
     See FIPS PUB 180-4 section 5.1.2. */

  /* Data is always processed in form of bytes (not by individual bits),
     therefore position of the first padding bit in byte is always
     predefined (0x80). */
  /* Buffer always have space at least for one byte (as full buffers are
     processed when formed). */
  ((अचिन्हित अक्षर *)ctx_buf)[bytes_have++] = 0x80U;

  अगर(CURL_SHA512_256_BLOCK_आकार - bytes_have < SHA512_256_आकार_OF_LEN_ADD) {
    /* No space in the current block to put the total length of message.
       Pad the current block with zeros and process it. */
    अगर(bytes_have < CURL_SHA512_256_BLOCK_आकार)
      स_रखो((अचिन्हित अक्षर *)ctx_buf + bytes_have, 0,
             CURL_SHA512_256_BLOCK_आकार - bytes_have);
    /* Process the full block. */
    Curl_sha512_256_transform(ctx->H, ctx->बफर);
    /* Start the new block. */
    bytes_have = 0;
  }

  /* Pad the rest of the buffer with zeros. */
  स_रखो((अचिन्हित अक्षर *)ctx_buf + bytes_have, 0,
         CURL_SHA512_256_BLOCK_आकार - SHA512_256_आकार_OF_LEN_ADD - bytes_have);
  /* Put high part of number of bits in processed message and then lower
     part of number of bits as big-endian values.
     See FIPS PUB 180-4 section 5.1.2. */
  /* Note: the target location is predefined and buffer is always aligned */
  CURL_PUT_64BIT_BE((अचिन्हित अक्षर *)ctx_buf +
                    CURL_SHA512_256_BLOCK_आकार - SHA512_256_आकार_OF_LEN_ADD,
                    ctx->गिनती_bits_hi);
  CURL_PUT_64BIT_BE((अचिन्हित अक्षर *)ctx_buf +
                    CURL_SHA512_256_BLOCK_आकार - SHA512_256_आकार_OF_LEN_ADD +
                    SHA512_256_BYTES_IN_WORD,
                    अंक_bits);
  /* Process the full final block. */
  Curl_sha512_256_transform(ctx->H, ctx->बफर);

  /* Put in BE mode the leftmost part of the hash as the final digest.
     See FIPS PUB 180-4 section 6.7. */

  CURL_PUT_64BIT_BE(digest + (0 * SHA512_256_BYTES_IN_WORD), ctx->H[0]);
  CURL_PUT_64BIT_BE(digest + (1 * SHA512_256_BYTES_IN_WORD), ctx->H[1]);
  CURL_PUT_64BIT_BE(digest + (2 * SHA512_256_BYTES_IN_WORD), ctx->H[2]);
  CURL_PUT_64BIT_BE(digest + (3 * SHA512_256_BYTES_IN_WORD), ctx->H[3]);

  /* Erase potentially sensitive data. */
  स_रखो(ctx, 0, माप(काष्ठा Curl_sha512_256ctx));

  वापस CURLE_OK;
}

#पूर्ण_अगर /* Local SHA-512/256 code */

/**
 * Compute SHA-512/256 hash for the given data in one function call
 * @param[out] output the pointer to put the hash
 * @param[in] input the pointer to the data to process
 * @param input_size the size of the data pointed by @a input
 * @return always #CURLE_OK
 */
CURLcode Curl_sha512_256it(अचिन्हित अक्षर *output, स्थिर अचिन्हित अक्षर *निवेश,
                           माप_प्रकार निवेश_आकार)
{
  Curl_sha512_256_ctx ctx;
  CURLcode परिणाम;

  परिणाम = Curl_sha512_256_init(&ctx);
  अगर(परिणाम != CURLE_OK)
    वापस परिणाम;

  परिणाम = Curl_sha512_256_update(&ctx, (स्थिर व्योम *)निवेश, निवेश_आकार);

  अगर(परिणाम != CURLE_OK) {
    (व्योम)Curl_sha512_256_finish(output, &ctx);
    वापस परिणाम;
  }

  वापस Curl_sha512_256_finish(output, &ctx);
}

/* Wrapper function, takes 'unsigned int' as length type, returns void */
अटल व्योम Curl_sha512_256_update_i(व्योम *context,
                                     स्थिर अचिन्हित अक्षर *तथ्य,
                                     अचिन्हित पूर्णांक लंबाई)
{
  /* Hypothetically the function may fail, but assume it does not */
  (व्योम)Curl_sha512_256_update(context, तथ्य, लंबाई);
}

/* Wrapper function, returns void */
अटल व्योम Curl_sha512_256_finish_v(अचिन्हित अक्षर *परिणाम, व्योम *context)
{
  /* Hypothetically the function may fail, but assume it does not */
  (व्योम)Curl_sha512_256_finish(परिणाम, context);
}

/* Wrapper function, takes 'unsigned int' as length type, returns void */

स्थिर काष्ठा HMAC_params Curl_HMAC_SHA512_256[] = {
  {
    /* Initialize context procedure. */
    Curl_sha512_256_init,
    /* Update context with data. */
    Curl_sha512_256_update_i,
    /* Get final result procedure. */
    Curl_sha512_256_finish_v,
    /* Context structure size. */
    माप(Curl_sha512_256_ctx),
    /* Maximum key length (bytes). */
    CURL_SHA512_256_BLOCK_आकार,
    /* Result length (bytes). */
    CURL_SHA512_256_DIGEST_आकार
  }
};

#पूर्ण_अगर /* !CURL_DISABLE_DIGEST_AUTH && !CURL_DISABLE_SHA512_256 */
