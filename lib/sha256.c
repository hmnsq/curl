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
#ضمن "curl_setup.h"

#إذا !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#ضمن "curl_sha256.h"

#جے_تعریف_شدہ USE_MBEDTLS
#ضمن <mbedtls/version.h>
#إذا MBEDTLS_نسخہ_نمبر < 0x03020000
#غلطی "mbedTLS 3.2.0 or greater required"
#جے_انت
#ضمن <psa/crypto_config.h>
#جے_انت

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

#جے_تعریف_شدہ USE_OPENSSL
#ضمن <openssl/evp.h>

بنية ossl_sha256_ctx {
  EVP_MD_CTX *openssl_ctx;
};
تعريفنوع بنية ossl_sha256_ctx my_sha256_ctx;

ساكن CURLcode my_sha256_init(فراغ *وچ)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)وچ;
  ctx->openssl_ctx = EVP_MD_CTX_new();
  إذا(!ctx->openssl_ctx)
    أرجع CURLE_باہرلا_دا_حافظہ;

  إذا(!EVP_DigestInit_ex(ctx->openssl_ctx, EVP_sha256(), عدم)) {
    EVP_MD_CTX_free(ctx->openssl_ctx);
    أرجع CURLE_FAILED_INIT;
  }
  أرجع CURLE_OK;
}

ساكن فراغ my_sha256_update(فراغ *وچ,
                             ثابت بنانشانی حرف *انکڑے,
                             بنانشانی صحيح لمائی)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)وچ;
  EVP_DigestUpdate(ctx->openssl_ctx, انکڑے, لمائی);
}

ساكن فراغ my_sha256_final(بنانشانی حرف *digest, فراغ *وچ)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)وچ;
  EVP_DigestFinal_ex(ctx->openssl_ctx, digest, عدم);
  EVP_MD_CTX_free(ctx->openssl_ctx);
}

#وإلاإذا defined(USE_WOLFSSL)
#ضمن <wolfssl/options.h>
#ضمن <wolfssl/wolfcrypt/sha256.h>

تعريفنوع بنية wc_Sha256 my_sha256_ctx;

ساكن CURLcode my_sha256_init(فراغ *وچ)
{
  إذا(wc_InitSha256(وچ))
    أرجع CURLE_FAILED_INIT;
  أرجع CURLE_OK;
}

ساكن فراغ my_sha256_update(فراغ *وچ,
                             ثابت بنانشانی حرف *انکڑے,
                             بنانشانی صحيح لمائی)
{
  (فراغ)wc_Sha256Update(وچ, انکڑے, (word32)لمائی);
}

ساكن فراغ my_sha256_final(بنانشانی حرف *digest, فراغ *وچ)
{
  (فراغ)wc_Sha256Final(وچ, digest);
}

#وإلاإذا defined(USE_GNUTLS)
#ضمن <nettle/sha2.h>
#ضمن <nettle/version.h>

تعريفنوع بنية sha256_ctx my_sha256_ctx;

ساكن CURLcode my_sha256_init(فراغ *ctx)
{
  sha256_init(ctx);
  أرجع CURLE_OK;
}

ساكن فراغ my_sha256_update(فراغ *ctx,
                             ثابت بنانشانی حرف *انکڑے,
                             بنانشانی صحيح لمائی)
{
  sha256_update(ctx, لمائی, انکڑے);
}

ساكن فراغ my_sha256_final(بنانشانی حرف *digest, فراغ *ctx)
{
#إذا NETTLE_نسخہ_MAJOR >= 4
  sha256_digest(ctx, digest);
#وإلا
  sha256_digest(ctx, SHA256_DIGEST_ناپ, digest);
#جے_انت
}

#وإلاإذا defined(USE_MBEDTLS) && \
  defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
#ضمن <psa/crypto.h>

تعريفنوع psa_hash_operation_t my_sha256_ctx;

ساكن CURLcode my_sha256_init(فراغ *ctx)
{
  psa_hash_operation_t *pctx = (psa_hash_operation_t *)ctx;
  *pctx = psa_hash_operation_init();
  إذا(psa_hash_تیاری(pctx, PSA_ALG_SHA_256) != PSA_SUCCESS)
    أرجع CURLE_باہرلا_دا_حافظہ;
  أرجع CURLE_OK;
}

ساكن فراغ my_sha256_update(فراغ *ctx,
                             ثابت بنانشانی حرف *انکڑے,
                             بنانشانی صحيح لمائی)
{
  (فراغ)psa_hash_update(ctx, انکڑے, لمائی);
}

ساكن فراغ my_sha256_final(بنانشانی حرف *digest, فراغ *ctx)
{
  size_t actual_لمائی;
  (فراغ)psa_hash_انت(ctx, digest, CURL_SHA256_DIGEST_لمائی,
                        &actual_لمائی);
}

#وإلاإذا (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
               __MAC_OS_X_VERSION_MAX_ALLOWED >= 1040) || \
      (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
               __IPHONE_OS_VERSION_MAX_ALLOWED >= 20000)
#ضمن <CommonCrypto/CommonDigest.h>

تعريفنوع CC_SHA256_CTX my_sha256_ctx;

ساكن CURLcode my_sha256_init(فراغ *ctx)
{
  (فراغ)CC_SHA256_Init(ctx);  /* always returns 1 */
  أرجع CURLE_OK;
}

ساكن فراغ my_sha256_update(فراغ *ctx,
                             ثابت بنانشانی حرف *انکڑے,
                             بنانشانی صحيح لمائی)
{
  (فراغ)CC_SHA256_Update(ctx, انکڑے, لمائی);
}

ساكن فراغ my_sha256_final(بنانشانی حرف *digest, فراغ *ctx)
{
  (فراغ)CC_SHA256_Final(digest, ctx);
}

#وإلاإذا defined(USE_WIN32_CRYPTO)
#ضمن <wincrypt.h>

بنية sha256_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
تعريفنوع بنية sha256_ctx my_sha256_ctx;

ساكن CURLcode my_sha256_init(فراغ *وچ)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)وچ;
  إذا(!CryptAcquireContext(&ctx->hCryptProv, عدم, عدم, PROV_RSA_AES,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    أرجع CURLE_باہرلا_دا_حافظہ;

  إذا(!CryptCreateHash(ctx->hCryptProv, CALG_SHA_256, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    أرجع CURLE_FAILED_INIT;
  }

  أرجع CURLE_OK;
}

ساكن فراغ my_sha256_update(فراغ *وچ,
                             ثابت بنانشانی حرف *انکڑے,
                             بنانشانی صحيح لمائی)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)وچ;
  Crypt_Hash_انکڑے(ctx->hHash, (ثابت BYTE *)انکڑے, لمائی, 0);
}

ساكن فراغ my_sha256_final(بنانشانی حرف *digest, فراغ *وچ)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)وچ;
  بنانشانی لما لمائی = 0;

  CryptGetHashParam(ctx->hHash, HP_HASHVAL, عدم, &لمائی, 0);
  إذا(لمائی == CURL_SHA256_DIGEST_لمائی)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &لمائی, 0);

  إذا(ctx->hHash)
    CryptDestroyHash(ctx->hHash);

  إذا(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#وإلا

/* When no other crypto library is available we use this code segment */

/* This is based on the SHA256 implementation in LibTomCrypt that was released
 * into public domain. */

#تعریف_کرو WPA_GET_BE32(a)              \
  ((((بنانشانی لما)(a)[0]) << 24) | \
   (((بنانشانی لما)(a)[1]) << 16) | \
   (((بنانشانی لما)(a)[2]) <<  8) | \
    ((بنانشانی لما)(a)[3]))
#تعریف_کرو WPA_PUT_BE32(a, val)                                         \
  افعل {                                                               \
    (a)[0] = (بنانشانی حرف)((((بنانشانی لما)(val)) >> 24) & 0xff); \
    (a)[1] = (بنانشانی حرف)((((بنانشانی لما)(val)) >> 16) & 0xff); \
    (a)[2] = (بنانشانی حرف)((((بنانشانی لما)(val)) >>  8) & 0xff); \
    (a)[3] = (بنانشانی حرف)(((بنانشانی لما)(val)) & 0xff);         \
  } طالما(0)

#تعریف_کرو WPA_PUT_BE64(a, val)                            \
  افعل {                                                  \
    (a)[0] = (بنانشانی حرف)(((uint64_t)(val)) >> 56);  \
    (a)[1] = (بنانشانی حرف)(((uint64_t)(val)) >> 48);  \
    (a)[2] = (بنانشانی حرف)(((uint64_t)(val)) >> 40);  \
    (a)[3] = (بنانشانی حرف)(((uint64_t)(val)) >> 32);  \
    (a)[4] = (بنانشانی حرف)(((uint64_t)(val)) >> 24);  \
    (a)[5] = (بنانشانی حرف)(((uint64_t)(val)) >> 16);  \
    (a)[6] = (بنانشانی حرف)(((uint64_t)(val)) >>  8);  \
    (a)[7] = (بنانشانی حرف)(((uint64_t)(val)) & 0xff); \
  } طالما(0)

بنية sha256_state {
  uint64_t لمائی;
  بنانشانی لما state[8], curlen;
  بنانشانی حرف buf[64];
};
تعريفنوع بنية sha256_state my_sha256_ctx;

/* The K array */
ساكن ثابت بنانشانی لما K[64] = {
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
#تعریف_کرو RORc(x, y) \
  (((((بنانشانی لما)(x) & 0xFFFFFFFFUL) >> (بنانشانی لما)((y) & 31)) | \
     ((بنانشانی لما)(x) << (بنانشانی لما)(32 - ((y) & 31)))) & 0xFFFFFFFFUL)

#تعریف_کرو Sha256_Ch(x, y, z)  (z ^ (x & (y ^ z)))
#تعریف_کرو Sha256_Maj(x, y, z) (((x | y) & z) | (x & y))
#تعریف_کرو Sha256_S(x, n)      RORc(x, n)
#تعریف_کرو Sha256_R(x, n)      (((x) & 0xFFFFFFFFUL) >> (n))

#تعریف_کرو Sigma0(x)         (Sha256_S(x, 2) ^ Sha256_S(x, 13) ^ Sha256_S(x, 22))
#تعریف_کرو Sigma1(x)         (Sha256_S(x, 6) ^ Sha256_S(x, 11) ^ Sha256_S(x, 25))
#تعریف_کرو Gamma0(x)         (Sha256_S(x, 7) ^ Sha256_S(x, 18) ^ Sha256_R(x, 3))
#تعریف_کرو Gamma1(x)         (Sha256_S(x, 17) ^ Sha256_S(x, 19) ^ Sha256_R(x, 10))

/* Compress 512 bits */
ساكن صحيح sha256_compress(بنية sha256_state *md, ثابت بنانشانی حرف *buf)
{
  بنانشانی لما S[8], W[64];
  صحيح i;

  /* Copy state into S */
  لكل(i = 0; i < 8; i++) {
    S[i] = md->state[i];
  }
  /* copy the state into 512 bits into W[0..15] */
  لكل(i = 0; i < 16; i++)
    W[i] = WPA_GET_BE32(buf + (4 * i));
  /* fill W[16..63] */
  لكل(i = 16; i < 64; i++) {
    W[i] = Gamma1(W[i - 2]) + W[i - 7] + Gamma0(W[i - 15]) + W[i - 16];
  }

  /* Compress */
#تعریف_کرو RND(a, b, c, d, e, f, g, h, i)                                   \
  افعل {                                                                   \
    بنانشانی لما t0 = h + Sigma1(e) + Sha256_Ch(e, f, g) + K[i] + W[i]; \
    بنانشانی لما t1 = Sigma0(a) + Sha256_Maj(a, b, c);                  \
    d += t0;                                                             \
    h = t0 + t1;                                                         \
  } طالما(0)

  لكل(i = 0; i < 64; ++i) {
    بنانشانی لما t;
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
  لكل(i = 0; i < 8; i++) {
    md->state[i] = md->state[i] + S[i];
  }

  أرجع 0;
}

/* Initialize the hash state */
ساكن CURLcode my_sha256_init(فراغ *وچ)
{
  بنية sha256_state *md = (بنية sha256_state *)وچ;
  md->curlen = 0;
  md->لمائی = 0;
  md->state[0] = 0x6A09E667UL;
  md->state[1] = 0xBB67AE85UL;
  md->state[2] = 0x3C6EF372UL;
  md->state[3] = 0xA54FF53AUL;
  md->state[4] = 0x510E527FUL;
  md->state[5] = 0x9B05688CUL;
  md->state[6] = 0x1F83D9ABUL;
  md->state[7] = 0x5BE0CD19UL;

  أرجع CURLE_OK;
}

/* Process a block of memory though the hash
   @param md     The hash state
   @param in     The data to hash
   @param inlen  The length of the data (octets)
 */
ساكن فراغ my_sha256_update(فراغ *ctx,
                             ثابت بنانشانی حرف *وچ,
                             بنانشانی صحيح len)
{
  بنانشانی لما inlen = len;
  بنانشانی لما n;
  بنية sha256_state *md = (بنية sha256_state *)ctx;
#تعریف_کرو CURL_SHA256_بلاک_ناپ 64
  إذا(md->curlen > حجم(md->buf))
    أرجع;
  طالما(inlen > 0) {
    إذا(md->curlen == 0 && inlen >= CURL_SHA256_بلاک_ناپ) {
      إذا(sha256_compress(md, وچ) < 0)
        أرجع;
      md->لمائی += CURL_SHA256_بلاک_ناپ * 8;
      وچ += CURL_SHA256_بلاک_ناپ;
      inlen -= CURL_SHA256_بلاک_ناپ;
    }
    وإلا {
      n = CURLMIN(inlen, (CURL_SHA256_بلاک_ناپ - md->curlen));
      حافظہ_نقل(md->buf + md->curlen, وچ, n);
      md->curlen += n;
      وچ += n;
      inlen -= n;
      إذا(md->curlen == CURL_SHA256_بلاک_ناپ) {
        إذا(sha256_compress(md, md->buf) < 0)
          أرجع;
        md->لمائی += 8 * CURL_SHA256_بلاک_ناپ;
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
ساكن فراغ my_sha256_final(بنانشانی حرف *باہرلا, فراغ *ctx)
{
  بنية sha256_state *md = ctx;
  صحيح i;

  إذا(md->curlen >= حجم(md->buf))
    أرجع;

  /* Increase the length of the message */
  md->لمائی += md->curlen * 8;

  /* Append the '1' bit */
  md->buf[md->curlen++] = (بنانشانی حرف)0x80;

  /* If the length is currently above 56 bytes we append zeros
   * then compress. Then we can fall back to padding zeros and length
   * encoding like normal.
   */
  إذا(md->curlen > 56) {
    طالما(md->curlen < 64) {
      md->buf[md->curlen++] = 0;
    }
    sha256_compress(md, md->buf);
    md->curlen = 0;
  }

  /* Pad up to 56 bytes of zeroes */
  طالما(md->curlen < 56) {
    md->buf[md->curlen++] = 0;
  }

  /* Store length */
  WPA_PUT_BE64(md->buf + 56, md->لمائی);
  sha256_compress(md, md->buf);

  /* Copy output */
  لكل(i = 0; i < 8; i++)
    WPA_PUT_BE32(باہرلا + (4 * i), md->state[i]);
}

#جے_انت /* CRYPTO LIBS */

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
CURLcode Curl_sha256it(بنانشانی حرف *output, ثابت بنانشانی حرف *داخلہ,
                       size_t len)
{
  CURLcode نتیجہ;
  my_sha256_ctx ctx;

  نتیجہ = my_sha256_init(&ctx);
  إذا(!نتیجہ) {
    افعل {
      بنانشانی صحيح ilen = (بنانشانی صحيح)CURLMIN(len, UINT_MAX);
      my_sha256_update(&ctx, داخلہ, ilen);
      len -= ilen;
      داخلہ += ilen;
    } طالما(len);
    my_sha256_final(output, &ctx);
  }
  أرجع نتیجہ;
}

ثابت بنية HMAC_params Curl_HMAC_SHA256 = {
  my_sha256_init,        /* Hash initialization function. */
  my_sha256_update,      /* Hash update function. */
  my_sha256_final,       /* Hash computation end function. */
  حجم(my_sha256_ctx), /* Size of hash context structure. */
  64,                    /* Maximum key length. */
  32                     /* Result size. */
};

#جے_انت /* AWS, DIGEST, or libssh2 */
