/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Florin Petriuc, <petriuc.florin@gmail.com>
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#ଅନ୍ତର୍ଭାଵ "curl_sha256.h"

#ifdef USE_MBEDTLS
#ଅନ୍ତର୍ଭାଵ <mbedtls/version.h>
#ଯଦି MBEDTLS_VERSION_ସଂଖ୍ୟା < 0x03020000
#error "mbedTLS 3.2.0 or greater required"
#endif
#ଅନ୍ତର୍ଭାଵ <psa/crypto_config.h>
#endif

/* Please keep the SSL backend-specific #if branches in this order:
 *
 * 1. USE_OPENSSL
 * 2. USE_WOLFSSL
 * 3. USE_GNUTLS
 * 4. USE_MBEDTLS
 * 5. USE_WIN32_CRYPTO
 * 6. USE_COMMON_CRYPTO
 *
 * This ensures that the same SSL branch gets activated throughout this source
 * file even if multiple backends are enabled at the same time.
 */

#ifdef USE_OPENSSL
#ଅନ୍ତର୍ଭାଵ <openssl/evp.h>

ସଂରଚନା ossl_sha256_ctx {
  EVP_MD_CTX *openssl_ctx;
};
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା ossl_sha256_ctx my_sha256_ctx;

ସ୍ଥିର CURLcode my_sha256_init(ଶୂନ୍ୟ *in)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  ctx->openssl_ctx = EVP_MD_CTX_new();
  ଯଦି(!ctx->openssl_ctx)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  ଯଦି(!EVP_DigestInit_ex(ctx->openssl_ctx, EVP_sha256(), ଖାଲି)) {
    EVP_MD_CTX_free(ctx->openssl_ctx);
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ my_sha256_update(ଶୂନ୍ୟ *in,
                             ଅଚଳ unsigned ଅକ୍ଷର *data,
                             unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା length)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  EVP_DigestUpdate(ctx->openssl_ctx, data, length);
}

ସ୍ଥିର ଶୂନ୍ୟ my_sha256_final(unsigned ଅକ୍ଷର *digest, ଶୂନ୍ୟ *in)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  EVP_DigestFinal_ex(ctx->openssl_ctx, digest, ଖାଲି);
  EVP_MD_CTX_free(ctx->openssl_ctx);
}

#ନଚେତଯଦି defined(USE_WOLFSSL)
#ଅନ୍ତର୍ଭାଵ <wolfssl/options.h>
#ଅନ୍ତର୍ଭାଵ <wolfssl/wolfcrypt/sha256.h>

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା wc_Sha256 my_sha256_ctx;

ସ୍ଥିର CURLcode my_sha256_init(ଶୂନ୍ୟ *in)
{
  ଯଦି(wc_InitSha256(in))
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ my_sha256_update(ଶୂନ୍ୟ *in,
                             ଅଚଳ unsigned ଅକ୍ଷର *data,
                             unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା length)
{
  (ଶୂନ୍ୟ)wc_Sha256Update(in, data, (word32)length);
}

ସ୍ଥିର ଶୂନ୍ୟ my_sha256_final(unsigned ଅକ୍ଷର *digest, ଶୂନ୍ୟ *in)
{
  (ଶୂନ୍ୟ)wc_Sha256Final(in, digest);
}

#ନଚେତଯଦି defined(USE_GNUTLS)
#ଅନ୍ତର୍ଭାଵ <nettle/sha2.h>
#ଅନ୍ତର୍ଭାଵ <nettle/version.h>

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା sha256_ctx my_sha256_ctx;

ସ୍ଥିର CURLcode my_sha256_init(ଶୂନ୍ୟ *ctx)
{
  sha256_init(ctx);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ my_sha256_update(ଶୂନ୍ୟ *ctx,
                             ଅଚଳ unsigned ଅକ୍ଷର *data,
                             unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା length)
{
  sha256_update(ctx, length, data);
}

ସ୍ଥିର ଶୂନ୍ୟ my_sha256_final(unsigned ଅକ୍ଷର *digest, ଶୂନ୍ୟ *ctx)
{
#ଯଦି NETTLE_VERSION_MAJOR >= 4
  sha256_digest(ctx, digest);
#ଅନ୍ୟଥା
  sha256_digest(ctx, SHA256_DIGEST_SIZE, digest);
#endif
}

#ନଚେତଯଦି defined(USE_MBEDTLS) && \
  defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
#ଅନ୍ତର୍ଭାଵ <psa/crypto.h>

ପ୍ରକାରବ୍ୟାଖ୍ୟା psa_hash_operation_t my_sha256_ctx;

ସ୍ଥିର CURLcode my_sha256_init(ଶୂନ୍ୟ *ctx)
{
  psa_hash_operation_t *pctx = (psa_hash_operation_t *)ctx;
  *pctx = psa_hash_operation_init();
  ଯଦି(psa_hash_setup(pctx, PSA_ALG_SHA_256) != PSA_SUCCESS)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ my_sha256_update(ଶୂନ୍ୟ *ctx,
                             ଅଚଳ unsigned ଅକ୍ଷର *data,
                             unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା length)
{
  (ଶୂନ୍ୟ)psa_hash_update(ctx, data, length);
}

ସ୍ଥିର ଶୂନ୍ୟ my_sha256_final(unsigned ଅକ୍ଷର *digest, ଶୂନ୍ୟ *ctx)
{
  size_t actual_length;
  (ଶୂନ୍ୟ)psa_hash_finish(ctx, digest, CURL_SHA256_DIGEST_LENGTH,
                        &actual_length);
}

#ନଚେତଯଦି (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
               __MAC_OS_X_VERSION_MAX_ALLOWED >= 1040) || \
      (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
               __IPHONE_OS_VERSION_MAX_ALLOWED >= 20000)
#ଅନ୍ତର୍ଭାଵ <CommonCrypto/CommonDigest.h>

ପ୍ରକାରବ୍ୟାଖ୍ୟା CC_SHA256_CTX my_sha256_ctx;

ସ୍ଥିର CURLcode my_sha256_init(ଶୂନ୍ୟ *ctx)
{
  (ଶୂନ୍ୟ)CC_SHA256_Init(ctx);  /* always returns 1 */
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ my_sha256_update(ଶୂନ୍ୟ *ctx,
                             ଅଚଳ unsigned ଅକ୍ଷର *data,
                             unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା length)
{
  (ଶୂନ୍ୟ)CC_SHA256_Update(ctx, data, length);
}

ସ୍ଥିର ଶୂନ୍ୟ my_sha256_final(unsigned ଅକ୍ଷର *digest, ଶୂନ୍ୟ *ctx)
{
  (ଶୂନ୍ୟ)CC_SHA256_Final(digest, ctx);
}

#ନଚେତଯଦି defined(USE_WIN32_CRYPTO)
#ଅନ୍ତର୍ଭାଵ <wincrypt.h>

ସଂରଚନା sha256_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା sha256_ctx my_sha256_ctx;

ସ୍ଥିର CURLcode my_sha256_init(ଶୂନ୍ୟ *in)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  ଯଦି(!CryptAcquireContext(&ctx->hCryptProv, ଖାଲି, ଖାଲି, PROV_RSA_AES,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  ଯଦି(!CryptCreateHash(ctx->hCryptProv, CALG_SHA_256, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ my_sha256_update(ଶୂନ୍ୟ *in,
                             ଅଚଳ unsigned ଅକ୍ଷର *data,
                             unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା length)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  CryptHashData(ctx->hHash, (ଅଚଳ BYTE *)data, length, 0);
}

ସ୍ଥିର ଶୂନ୍ୟ my_sha256_final(unsigned ଅକ୍ଷର *digest, ଶୂନ୍ୟ *in)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  unsigned long length = 0;

  CryptGetHashParam(ctx->hHash, HP_HASHVAL, ଖାଲି, &length, 0);
  ଯଦି(length == CURL_SHA256_DIGEST_LENGTH)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &length, 0);

  ଯଦି(ctx->hHash)
    CryptDestroyHash(ctx->hHash);

  ଯଦି(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#ଅନ୍ୟଥା

/* When no other crypto library is available we use this code segment */

/* This is based on the SHA256 implementation in LibTomCrypt that was released
 * into public domain. */

#define WPA_GET_BE32(a)              \
  ((((unsigned long)(a)[0]) << 24) | \
   (((unsigned long)(a)[1]) << 16) | \
   (((unsigned long)(a)[2]) <<  8) | \
    ((unsigned long)(a)[3]))
#define WPA_PUT_BE32(a, val)                                         \
  କର {                                                               \
    (a)[0] = (unsigned ଅକ୍ଷର)((((unsigned long)(val)) >> 24) & 0xff); \
    (a)[1] = (unsigned ଅକ୍ଷର)((((unsigned long)(val)) >> 16) & 0xff); \
    (a)[2] = (unsigned ଅକ୍ଷର)((((unsigned long)(val)) >>  8) & 0xff); \
    (a)[3] = (unsigned ଅକ୍ଷର)(((unsigned long)(val)) & 0xff);         \
  } ଯେତେବେଳେ(0)

#define WPA_PUT_BE64(a, val)                            \
  କର {                                                  \
    (a)[0] = (unsigned ଅକ୍ଷର)(((uint64_t)(val)) >> 56);  \
    (a)[1] = (unsigned ଅକ୍ଷର)(((uint64_t)(val)) >> 48);  \
    (a)[2] = (unsigned ଅକ୍ଷର)(((uint64_t)(val)) >> 40);  \
    (a)[3] = (unsigned ଅକ୍ଷର)(((uint64_t)(val)) >> 32);  \
    (a)[4] = (unsigned ଅକ୍ଷର)(((uint64_t)(val)) >> 24);  \
    (a)[5] = (unsigned ଅକ୍ଷର)(((uint64_t)(val)) >> 16);  \
    (a)[6] = (unsigned ଅକ୍ଷର)(((uint64_t)(val)) >>  8);  \
    (a)[7] = (unsigned ଅକ୍ଷର)(((uint64_t)(val)) & 0xff); \
  } ଯେତେବେଳେ(0)

ସଂରଚନା sha256_state {
  uint64_t length;
  unsigned long state[8], curlen;
  unsigned ଅକ୍ଷର buf[64];
};
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା sha256_state my_sha256_ctx;

/* The K array */
ସ୍ଥିର ଅଚଳ unsigned long K[64] = {
  0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 0x3956c25bUL,
  0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL, 0xd807aa98UL, 0x12835b01UL,
  0x243185beUL, 0x550c7dc3UL, 0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL,
  0xc19bf174UL, 0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
  0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL, 0x983e5152UL,
  0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL, 0xc6e00bf3UL, 0xd5a79147UL,
  0x06ca6351UL, 0x14292967UL, 0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL,
  0x53380d13UL, 0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
  0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL, 0xd192e819UL,
  0xd6990624UL, 0xf40e3585UL, 0x106aa070UL, 0x19a4c116UL, 0x1e376c08UL,
  0x2748774cUL, 0x34b0bcb5UL, 0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL,
  0x682e6ff3UL, 0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
  0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};

/* Various logical functions */
#define RORc(x, y) \
  (((((unsigned long)(x) & 0xFFFFFFFFUL) >> (unsigned long)((y) & 31)) | \
     ((unsigned long)(x) << (unsigned long)(32 - ((y) & 31)))) & 0xFFFFFFFFUL)

#define Sha256_Ch(x, y, z)  (z ^ (x & (y ^ z)))
#define Sha256_Maj(x, y, z) (((x | y) & z) | (x & y))
#define Sha256_S(x, n)      RORc(x, n)
#define Sha256_R(x, n)      (((x) & 0xFFFFFFFFUL) >> (n))

#define Sigma0(x)         (Sha256_S(x, 2) ^ Sha256_S(x, 13) ^ Sha256_S(x, 22))
#define Sigma1(x)         (Sha256_S(x, 6) ^ Sha256_S(x, 11) ^ Sha256_S(x, 25))
#define Gamma0(x)         (Sha256_S(x, 7) ^ Sha256_S(x, 18) ^ Sha256_R(x, 3))
#define Gamma1(x)         (Sha256_S(x, 17) ^ Sha256_S(x, 19) ^ Sha256_R(x, 10))

/* Compress 512 bits */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sha256_compress(ସଂରଚନା sha256_state *md, ଅଚଳ unsigned ଅକ୍ଷର *buf)
{
  unsigned long S[8], W[64];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

  /* Copy state into S */
  ପାଇଁ(i = 0; i < 8; i++) {
    S[i] = md->state[i];
  }
  /* copy the state into 512 bits into W[0..15] */
  ପାଇଁ(i = 0; i < 16; i++)
    W[i] = WPA_GET_BE32(buf + (4 * i));
  /* fill W[16..63] */
  ପାଇଁ(i = 16; i < 64; i++) {
    W[i] = Gamma1(W[i - 2]) + W[i - 7] + Gamma0(W[i - 15]) + W[i - 16];
  }

  /* Compress */
#define RND(a, b, c, d, e, f, g, h, i)                                   \
  କର {                                                                   \
    unsigned long t0 = h + Sigma1(e) + Sha256_Ch(e, f, g) + K[i] + W[i]; \
    unsigned long t1 = Sigma0(a) + Sha256_Maj(a, b, c);                  \
    d += t0;                                                             \
    h = t0 + t1;                                                         \
  } ଯେତେବେଳେ(0)

  ପାଇଁ(i = 0; i < 64; ++i) {
    unsigned long t;
    RND(S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], i);
    t = S[7];
    S[7] = S[6];
    S[6] = S[5];
    S[5] = S[4];
    S[4] = S[3];
    S[3] = S[2];
    S[2] = S[1];
    S[1] = S[0];
    S[0] = t;
  }

  /* Feedback */
  ପାଇଁ(i = 0; i < 8; i++) {
    md->state[i] = md->state[i] + S[i];
  }

  ଫେରନ୍ତୁ 0;
}

/* Initialize the hash state */
ସ୍ଥିର CURLcode my_sha256_init(ଶୂନ୍ୟ *in)
{
  ସଂରଚନା sha256_state *md = (ସଂରଚନା sha256_state *)in;
  md->curlen = 0;
  md->length = 0;
  md->state[0] = 0x6A09E667UL;
  md->state[1] = 0xBB67AE85UL;
  md->state[2] = 0x3C6EF372UL;
  md->state[3] = 0xA54FF53AUL;
  md->state[4] = 0x510E527FUL;
  md->state[5] = 0x9B05688CUL;
  md->state[6] = 0x1F83D9ABUL;
  md->state[7] = 0x5BE0CD19UL;

  ଫେରନ୍ତୁ CURLE_OK;
}

/* Process a block of memory though the hash
   @param md     The hash state
   @param in     The data to hash
   @param inlen  The length of the data (octets)
 */
ସ୍ଥିର ଶୂନ୍ୟ my_sha256_update(ଶୂନ୍ୟ *ctx,
                             ଅଚଳ unsigned ଅକ୍ଷର *in,
                             unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
  unsigned long inlen = len;
  unsigned long n;
  ସଂରଚନା sha256_state *md = (ସଂରଚନା sha256_state *)ctx;
#define CURL_SHA256_BLOCK_SIZE 64
  ଯଦି(md->curlen > ଆକାର(md->buf))
    ଫେରନ୍ତୁ;
  ଯେତେବେଳେ(inlen > 0) {
    ଯଦି(md->curlen == 0 && inlen >= CURL_SHA256_BLOCK_SIZE) {
      ଯଦି(sha256_compress(md, in) < 0)
        ଫେରନ୍ତୁ;
      md->length += CURL_SHA256_BLOCK_SIZE * 8;
      in += CURL_SHA256_BLOCK_SIZE;
      inlen -= CURL_SHA256_BLOCK_SIZE;
    }
    ଅନ୍ୟଥା {
      n = CURLMIN(inlen, (CURL_SHA256_BLOCK_SIZE - md->curlen));
      memcpy(md->buf + md->curlen, in, n);
      md->curlen += n;
      in += n;
      inlen -= n;
      ଯଦି(md->curlen == CURL_SHA256_BLOCK_SIZE) {
        ଯଦି(sha256_compress(md, md->buf) < 0)
          ଫେରନ୍ତୁ;
        md->length += 8 * CURL_SHA256_BLOCK_SIZE;
        md->curlen = 0;
      }
    }
  }
}

/* Terminate the hash to get the digest
   @param md  The hash state
   @param out [out] The destination of the hash (32 bytes)
   @return 0 if successful
 */
ସ୍ଥିର ଶୂନ୍ୟ my_sha256_final(unsigned ଅକ୍ଷର *out, ଶୂନ୍ୟ *ctx)
{
  ସଂରଚନା sha256_state *md = ctx;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

  ଯଦି(md->curlen >= ଆକାର(md->buf))
    ଫେରନ୍ତୁ;

  /* Increase the length of the message */
  md->length += md->curlen * 8;

  /* Append the '1' bit */
  md->buf[md->curlen++] = (unsigned ଅକ୍ଷର)0x80;

  /* If the length is currently above 56 bytes we append zeros
   * then compress. Then we can fall back to padding zeros and length
   * encoding like normal.
   */
  ଯଦି(md->curlen > 56) {
    ଯେତେବେଳେ(md->curlen < 64) {
      md->buf[md->curlen++] = 0;
    }
    sha256_compress(md, md->buf);
    md->curlen = 0;
  }

  /* Pad up to 56 bytes of zeroes */
  ଯେତେବେଳେ(md->curlen < 56) {
    md->buf[md->curlen++] = 0;
  }

  /* Store length */
  WPA_PUT_BE64(md->buf + 56, md->length);
  sha256_compress(md, md->buf);

  /* Copy output */
  ପାଇଁ(i = 0; i < 8; i++)
    WPA_PUT_BE32(out + (4 * i), md->state[i]);
}

#endif /* CRYPTO LIBS */

/*
 * Curl_sha256it()
 *
 * Generates a SHA256 hash for the given input data.
 *
 * Parameters:
 *
 * output [in/out] - The output buffer.
 * input  [in]     - The input data.
 * length [in]     - The input length.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_sha256it(unsigned ଅକ୍ଷର *output, ଅଚଳ unsigned ଅକ୍ଷର *ନିବେଶ,
                       size_t len)
{
  CURLcode result;
  my_sha256_ctx ctx;

  result = my_sha256_init(&ctx);
  ଯଦି(!result) {
    କର {
      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ilen = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)CURLMIN(len, UINT_MAX);
      my_sha256_update(&ctx, ନିବେଶ, ilen);
      len -= ilen;
      ନିବେଶ += ilen;
    } ଯେତେବେଳେ(len);
    my_sha256_final(output, &ctx);
  }
  ଫେରନ୍ତୁ result;
}

ଅଚଳ ସଂରଚନା HMAC_params Curl_HMAC_SHA256 = {
  my_sha256_init,        /* Hash initialization function. */
  my_sha256_update,      /* Hash update function. */
  my_sha256_final,       /* Hash computation end function. */
  ଆକାର(my_sha256_ctx), /* Size of hash context structure. */
  64,                    /* Maximum key length. */
  32                     /* Result size. */
};

#endif /* AWS, DIGEST, or libssh2 */
