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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#అన్తర్భావ "curl_sha256.h"

#నిర్వచితమైతే USE_MBEDTLS
#అన్తర్భావ <mbedtls/version.h>
#ఒకవేళ MBEDTLS_సంస్కరణ_సంఖ్య < 0x03020000
#దోషం "mbedTLS 3.2.0 or greater required"
#అయితే_ముగింపు
#అన్తర్భావ <psa/crypto_config.h>
#అయితే_ముగింపు

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

#నిర్వచితమైతే USE_OPENSSL
#అన్తర్భావ <openssl/evp.h>

నిర్మాణం ossl_sha256_ctx {
  EVP_MD_CTX *openssl_ctx;
};
రకనిర్వచనం నిర్మాణం ossl_sha256_ctx my_sha256_ctx;

స్థిర CURLcode my_sha256_init(శూన్య *లోపల)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)లోపల;
  ctx->openssl_ctx = EVP_MD_CTX_new();
  ఒకవేళ(!ctx->openssl_ctx)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ఒకవేళ(!EVP_DigestInit_ex(ctx->openssl_ctx, EVP_sha256(), ఖాళీ)) {
    EVP_MD_CTX_free(ctx->openssl_ctx);
    తిరిగి CURLE_FAILED_INIT;
  }
  తిరిగి CURLE_OK;
}

స్థిర శూన్య my_sha256_update(శూన్య *లోపల,
                             స్థిరరాశి గుర్తులేని అక్షరం *దత్తాంశం,
                             గుర్తులేని పూర్ణాంకం నిడివి)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)లోపల;
  EVP_DigestUpdate(ctx->openssl_ctx, దత్తాంశం, నిడివి);
}

స్థిర శూన్య my_sha256_final(గుర్తులేని అక్షరం *digest, శూన్య *లోపల)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)లోపల;
  EVP_DigestFinal_ex(ctx->openssl_ctx, digest, ఖాళీ);
  EVP_MD_CTX_free(ctx->openssl_ctx);
}

#లేదావేళ defined(USE_WOLFSSL)
#అన్తర్భావ <wolfssl/options.h>
#అన్తర్భావ <wolfssl/wolfcrypt/sha256.h>

రకనిర్వచనం నిర్మాణం wc_Sha256 my_sha256_ctx;

స్థిర CURLcode my_sha256_init(శూన్య *లోపల)
{
  ఒకవేళ(wc_InitSha256(లోపల))
    తిరిగి CURLE_FAILED_INIT;
  తిరిగి CURLE_OK;
}

స్థిర శూన్య my_sha256_update(శూన్య *లోపల,
                             స్థిరరాశి గుర్తులేని అక్షరం *దత్తాంశం,
                             గుర్తులేని పూర్ణాంకం నిడివి)
{
  (శూన్య)wc_Sha256Update(లోపల, దత్తాంశం, (word32)నిడివి);
}

స్థిర శూన్య my_sha256_final(గుర్తులేని అక్షరం *digest, శూన్య *లోపల)
{
  (శూన్య)wc_Sha256Final(లోపల, digest);
}

#లేదావేళ defined(USE_GNUTLS)
#అన్తర్భావ <nettle/sha2.h>
#అన్తర్భావ <nettle/version.h>

రకనిర్వచనం నిర్మాణం sha256_ctx my_sha256_ctx;

స్థిర CURLcode my_sha256_init(శూన్య *ctx)
{
  sha256_init(ctx);
  తిరిగి CURLE_OK;
}

స్థిర శూన్య my_sha256_update(శూన్య *ctx,
                             స్థిరరాశి గుర్తులేని అక్షరం *దత్తాంశం,
                             గుర్తులేని పూర్ణాంకం నిడివి)
{
  sha256_update(ctx, నిడివి, దత్తాంశం);
}

స్థిర శూన్య my_sha256_final(గుర్తులేని అక్షరం *digest, శూన్య *ctx)
{
#ఒకవేళ NETTLE_సంస్కరణ_MAJOR >= 4
  sha256_digest(ctx, digest);
#లేకపోతే
  sha256_digest(ctx, SHA256_DIGEST_కొలత, digest);
#అయితే_ముగింపు
}

#లేదావేళ defined(USE_MBEDTLS) && \
  defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
#అన్తర్భావ <psa/crypto.h>

రకనిర్వచనం psa_hash_operation_t my_sha256_ctx;

స్థిర CURLcode my_sha256_init(శూన్య *ctx)
{
  psa_hash_operation_t *pctx = (psa_hash_operation_t *)ctx;
  *pctx = psa_hash_operation_init();
  ఒకవేళ(psa_hash_అమరిక(pctx, PSA_ALG_SHA_256) != PSA_SUCCESS)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  తిరిగి CURLE_OK;
}

స్థిర శూన్య my_sha256_update(శూన్య *ctx,
                             స్థిరరాశి గుర్తులేని అక్షరం *దత్తాంశం,
                             గుర్తులేని పూర్ణాంకం నిడివి)
{
  (శూన్య)psa_hash_update(ctx, దత్తాంశం, నిడివి);
}

స్థిర శూన్య my_sha256_final(గుర్తులేని అక్షరం *digest, శూన్య *ctx)
{
  size_t actual_నిడివి;
  (శూన్య)psa_hash_ముగింపు(ctx, digest, CURL_SHA256_DIGEST_నిడివి,
                        &actual_నిడివి);
}

#లేదావేళ (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
               __MAC_OS_X_VERSION_MAX_ALLOWED >= 1040) || \
      (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
               __IPHONE_OS_VERSION_MAX_ALLOWED >= 20000)
#అన్తర్భావ <CommonCrypto/CommonDigest.h>

రకనిర్వచనం CC_SHA256_CTX my_sha256_ctx;

స్థిర CURLcode my_sha256_init(శూన్య *ctx)
{
  (శూన్య)CC_SHA256_Init(ctx);  /* always returns 1 */
  తిరిగి CURLE_OK;
}

స్థిర శూన్య my_sha256_update(శూన్య *ctx,
                             స్థిరరాశి గుర్తులేని అక్షరం *దత్తాంశం,
                             గుర్తులేని పూర్ణాంకం నిడివి)
{
  (శూన్య)CC_SHA256_Update(ctx, దత్తాంశం, నిడివి);
}

స్థిర శూన్య my_sha256_final(గుర్తులేని అక్షరం *digest, శూన్య *ctx)
{
  (శూన్య)CC_SHA256_Final(digest, ctx);
}

#లేదావేళ defined(USE_WIN32_CRYPTO)
#అన్తర్భావ <wincrypt.h>

నిర్మాణం sha256_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
రకనిర్వచనం నిర్మాణం sha256_ctx my_sha256_ctx;

స్థిర CURLcode my_sha256_init(శూన్య *లోపల)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)లోపల;
  ఒకవేళ(!CryptAcquireContext(&ctx->hCryptProv, ఖాళీ, ఖాళీ, PROV_RSA_AES,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ఒకవేళ(!CryptCreateHash(ctx->hCryptProv, CALG_SHA_256, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    తిరిగి CURLE_FAILED_INIT;
  }

  తిరిగి CURLE_OK;
}

స్థిర శూన్య my_sha256_update(శూన్య *లోపల,
                             స్థిరరాశి గుర్తులేని అక్షరం *దత్తాంశం,
                             గుర్తులేని పూర్ణాంకం నిడివి)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)లోపల;
  Crypt_Hash_దత్తాంశం(ctx->hHash, (స్థిరరాశి BYTE *)దత్తాంశం, నిడివి, 0);
}

స్థిర శూన్య my_sha256_final(గుర్తులేని అక్షరం *digest, శూన్య *లోపల)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)లోపల;
  గుర్తులేని దీర్ఘ నిడివి = 0;

  CryptGetHashParam(ctx->hHash, HP_HASHVAL, ఖాళీ, &నిడివి, 0);
  ఒకవేళ(నిడివి == CURL_SHA256_DIGEST_నిడివి)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &నిడివి, 0);

  ఒకవేళ(ctx->hHash)
    CryptDestroyHash(ctx->hHash);

  ఒకవేళ(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#లేకపోతే

/* When no other crypto library is available we use this code segment */

/* This is based on the SHA256 implementation in LibTomCrypt that was released
 * into public domain. */

#నిర్వచించు WPA_GET_BE32(a)              \
  ((((గుర్తులేని దీర్ఘ)(a)[0]) << 24) | \
   (((గుర్తులేని దీర్ఘ)(a)[1]) << 16) | \
   (((గుర్తులేని దీర్ఘ)(a)[2]) <<  8) | \
    ((గుర్తులేని దీర్ఘ)(a)[3]))
#నిర్వచించు WPA_PUT_BE32(a, val)                                         \
  చేయి {                                                               \
    (a)[0] = (గుర్తులేని అక్షరం)((((గుర్తులేని దీర్ఘ)(val)) >> 24) & 0xff); \
    (a)[1] = (గుర్తులేని అక్షరం)((((గుర్తులేని దీర్ఘ)(val)) >> 16) & 0xff); \
    (a)[2] = (గుర్తులేని అక్షరం)((((గుర్తులేని దీర్ఘ)(val)) >>  8) & 0xff); \
    (a)[3] = (గుర్తులేని అక్షరం)(((గుర్తులేని దీర్ఘ)(val)) & 0xff);         \
  } అయితే(0)

#నిర్వచించు WPA_PUT_BE64(a, val)                            \
  చేయి {                                                  \
    (a)[0] = (గుర్తులేని అక్షరం)(((uint64_t)(val)) >> 56);  \
    (a)[1] = (గుర్తులేని అక్షరం)(((uint64_t)(val)) >> 48);  \
    (a)[2] = (గుర్తులేని అక్షరం)(((uint64_t)(val)) >> 40);  \
    (a)[3] = (గుర్తులేని అక్షరం)(((uint64_t)(val)) >> 32);  \
    (a)[4] = (గుర్తులేని అక్షరం)(((uint64_t)(val)) >> 24);  \
    (a)[5] = (గుర్తులేని అక్షరం)(((uint64_t)(val)) >> 16);  \
    (a)[6] = (గుర్తులేని అక్షరం)(((uint64_t)(val)) >>  8);  \
    (a)[7] = (గుర్తులేని అక్షరం)(((uint64_t)(val)) & 0xff); \
  } అయితే(0)

నిర్మాణం sha256_state {
  uint64_t నిడివి;
  గుర్తులేని దీర్ఘ state[8], curlen;
  గుర్తులేని అక్షరం buf[64];
};
రకనిర్వచనం నిర్మాణం sha256_state my_sha256_ctx;

/* The K array */
స్థిర స్థిరరాశి గుర్తులేని దీర్ఘ K[64] = {
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
#నిర్వచించు RORc(x, y) \
  (((((గుర్తులేని దీర్ఘ)(x) & 0xFFFFFFFFUL) >> (గుర్తులేని దీర్ఘ)((y) & 31)) | \
     ((గుర్తులేని దీర్ఘ)(x) << (గుర్తులేని దీర్ఘ)(32 - ((y) & 31)))) & 0xFFFFFFFFUL)

#నిర్వచించు Sha256_Ch(x, y, z)  (z ^ (x & (y ^ z)))
#నిర్వచించు Sha256_Maj(x, y, z) (((x | y) & z) | (x & y))
#నిర్వచించు Sha256_S(x, n)      RORc(x, n)
#నిర్వచించు Sha256_R(x, n)      (((x) & 0xFFFFFFFFUL) >> (n))

#నిర్వచించు Sigma0(x)         (Sha256_S(x, 2) ^ Sha256_S(x, 13) ^ Sha256_S(x, 22))
#నిర్వచించు Sigma1(x)         (Sha256_S(x, 6) ^ Sha256_S(x, 11) ^ Sha256_S(x, 25))
#నిర్వచించు Gamma0(x)         (Sha256_S(x, 7) ^ Sha256_S(x, 18) ^ Sha256_R(x, 3))
#నిర్వచించు Gamma1(x)         (Sha256_S(x, 17) ^ Sha256_S(x, 19) ^ Sha256_R(x, 10))

/* Compress 512 bits */
స్థిర పూర్ణాంకం sha256_compress(నిర్మాణం sha256_state *md, స్థిరరాశి గుర్తులేని అక్షరం *buf)
{
  గుర్తులేని దీర్ఘ S[8], W[64];
  పూర్ణాంకం i;

  /* Copy state into S */
  కొరకు(i = 0; i < 8; i++) {
    S[i] = md->state[i];
  }
  /* copy the state into 512 bits into W[0..15] */
  కొరకు(i = 0; i < 16; i++)
    W[i] = WPA_GET_BE32(buf + (4 * i));
  /* fill W[16..63] */
  కొరకు(i = 16; i < 64; i++) {
    W[i] = Gamma1(W[i - 2]) + W[i - 7] + Gamma0(W[i - 15]) + W[i - 16];
  }

  /* Compress */
#నిర్వచించు RND(a, b, c, d, e, f, g, h, i)                                   \
  చేయి {                                                                   \
    గుర్తులేని దీర్ఘ t0 = h + Sigma1(e) + Sha256_Ch(e, f, g) + K[i] + W[i]; \
    గుర్తులేని దీర్ఘ t1 = Sigma0(a) + Sha256_Maj(a, b, c);                  \
    d += t0;                                                             \
    h = t0 + t1;                                                         \
  } అయితే(0)

  కొరకు(i = 0; i < 64; ++i) {
    గుర్తులేని దీర్ఘ t;
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
  కొరకు(i = 0; i < 8; i++) {
    md->state[i] = md->state[i] + S[i];
  }

  తిరిగి 0;
}

/* Initialize the hash state */
స్థిర CURLcode my_sha256_init(శూన్య *లోపల)
{
  నిర్మాణం sha256_state *md = (నిర్మాణం sha256_state *)లోపల;
  md->curlen = 0;
  md->నిడివి = 0;
  md->state[0] = 0x6A09E667UL;
  md->state[1] = 0xBB67AE85UL;
  md->state[2] = 0x3C6EF372UL;
  md->state[3] = 0xA54FF53AUL;
  md->state[4] = 0x510E527FUL;
  md->state[5] = 0x9B05688CUL;
  md->state[6] = 0x1F83D9ABUL;
  md->state[7] = 0x5BE0CD19UL;

  తిరిగి CURLE_OK;
}

/* Process a block of memory though the hash
   @param md     The hash state
   @param in     The data to hash
   @param inlen  The length of the data (octets)
 */
స్థిర శూన్య my_sha256_update(శూన్య *ctx,
                             స్థిరరాశి గుర్తులేని అక్షరం *లోపల,
                             గుర్తులేని పూర్ణాంకం len)
{
  గుర్తులేని దీర్ఘ inlen = len;
  గుర్తులేని దీర్ఘ n;
  నిర్మాణం sha256_state *md = (నిర్మాణం sha256_state *)ctx;
#నిర్వచించు CURL_SHA256_ఖండం_కొలత 64
  ఒకవేళ(md->curlen > పరిమాణం(md->buf))
    తిరిగి;
  అయితే(inlen > 0) {
    ఒకవేళ(md->curlen == 0 && inlen >= CURL_SHA256_ఖండం_కొలత) {
      ఒకవేళ(sha256_compress(md, లోపల) < 0)
        తిరిగి;
      md->నిడివి += CURL_SHA256_ఖండం_కొలత * 8;
      లోపల += CURL_SHA256_ఖండం_కొలత;
      inlen -= CURL_SHA256_ఖండం_కొలత;
    }
    లేకపోతే {
      n = CURLMIN(inlen, (CURL_SHA256_ఖండం_కొలత - md->curlen));
      జ్ఞాపకం_నకలు(md->buf + md->curlen, లోపల, n);
      md->curlen += n;
      లోపల += n;
      inlen -= n;
      ఒకవేళ(md->curlen == CURL_SHA256_ఖండం_కొలత) {
        ఒకవేళ(sha256_compress(md, md->buf) < 0)
          తిరిగి;
        md->నిడివి += 8 * CURL_SHA256_ఖండం_కొలత;
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
స్థిర శూన్య my_sha256_final(గుర్తులేని అక్షరం *బహిర్గతం, శూన్య *ctx)
{
  నిర్మాణం sha256_state *md = ctx;
  పూర్ణాంకం i;

  ఒకవేళ(md->curlen >= పరిమాణం(md->buf))
    తిరిగి;

  /* Increase the length of the message */
  md->నిడివి += md->curlen * 8;

  /* Append the '1' bit */
  md->buf[md->curlen++] = (గుర్తులేని అక్షరం)0x80;

  /* If the length is currently above 56 bytes we append zeros
   * then compress. Then we can fall back to padding zeros and length
   * encoding like normal.
   */
  ఒకవేళ(md->curlen > 56) {
    అయితే(md->curlen < 64) {
      md->buf[md->curlen++] = 0;
    }
    sha256_compress(md, md->buf);
    md->curlen = 0;
  }

  /* Pad up to 56 bytes of zeroes */
  అయితే(md->curlen < 56) {
    md->buf[md->curlen++] = 0;
  }

  /* Store length */
  WPA_PUT_BE64(md->buf + 56, md->నిడివి);
  sha256_compress(md, md->buf);

  /* Copy output */
  కొరకు(i = 0; i < 8; i++)
    WPA_PUT_BE32(బహిర్గతం + (4 * i), md->state[i]);
}

#అయితే_ముగింపు /* CRYPTO LIBS */

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
CURLcode Curl_sha256it(గుర్తులేని అక్షరం *output, స్థిరరాశి గుర్తులేని అక్షరం *ఆదానం,
                       size_t len)
{
  CURLcode ఫలితం;
  my_sha256_ctx ctx;

  ఫలితం = my_sha256_init(&ctx);
  ఒకవేళ(!ఫలితం) {
    చేయి {
      గుర్తులేని పూర్ణాంకం ilen = (గుర్తులేని పూర్ణాంకం)CURLMIN(len, UINT_MAX);
      my_sha256_update(&ctx, ఆదానం, ilen);
      len -= ilen;
      ఆదానం += ilen;
    } అయితే(len);
    my_sha256_final(output, &ctx);
  }
  తిరిగి ఫలితం;
}

స్థిరరాశి నిర్మాణం HMAC_params Curl_HMAC_SHA256 = {
  my_sha256_init,        /* Hash initialization function. */
  my_sha256_update,      /* Hash update function. */
  my_sha256_final,       /* Hash computation end function. */
  పరిమాణం(my_sha256_ctx), /* Size of hash context structure. */
  64,                    /* Maximum key length. */
  32                     /* Result size. */
};

#అయితే_ముగింపు /* AWS, DIGEST, or libssh2 */
