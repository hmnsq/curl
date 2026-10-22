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
#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#شامل "curl_sha256.h"

#اگر_تعریف_شدہ USE_MBEDTLS
#شامل <mbedtls/version.h>
#اگر MBEDTLS_نسخہ_نمبر < 0x03020000
#غلطی "mbedTLS 3.2.0 or greater required"
#اگر_ختم
#شامل <psa/crypto_config.h>
#اگر_ختم

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

#اگر_تعریف_شدہ USE_OPENSSL
#شامل <openssl/evp.h>

ساخت ossl_sha256_ctx {
  EVP_MD_CTX *openssl_ctx;
};
قسمتعریف ساخت ossl_sha256_ctx my_sha256_ctx;

ساکن CURLcode my_sha256_init(خالی *اندر)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)اندر;
  ctx->openssl_ctx = EVP_MD_CTX_new();
  اگر(!ctx->openssl_ctx)
    واپس CURLE_برآمد_کا_حافظہ;

  اگر(!EVP_DigestInit_ex(ctx->openssl_ctx, EVP_sha256(), تہی)) {
    EVP_MD_CTX_free(ctx->openssl_ctx);
    واپس CURLE_FAILED_INIT;
  }
  واپس CURLE_OK;
}

ساکن خالی my_sha256_update(خالی *اندر,
                             ثابت غیرعلامتی حرف *ڈیٹا,
                             غیرعلامتی صحیح لمبائی)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)اندر;
  EVP_DigestUpdate(ctx->openssl_ctx, ڈیٹا, لمبائی);
}

ساکن خالی my_sha256_final(غیرعلامتی حرف *digest, خالی *اندر)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)اندر;
  EVP_DigestFinal_ex(ctx->openssl_ctx, digest, تہی);
  EVP_MD_CTX_free(ctx->openssl_ctx);
}

#وگرنہ_اگر defined(USE_WOLFSSL)
#شامل <wolfssl/options.h>
#شامل <wolfssl/wolfcrypt/sha256.h>

قسمتعریف ساخت wc_Sha256 my_sha256_ctx;

ساکن CURLcode my_sha256_init(خالی *اندر)
{
  اگر(wc_InitSha256(اندر))
    واپس CURLE_FAILED_INIT;
  واپس CURLE_OK;
}

ساکن خالی my_sha256_update(خالی *اندر,
                             ثابت غیرعلامتی حرف *ڈیٹا,
                             غیرعلامتی صحیح لمبائی)
{
  (خالی)wc_Sha256Update(اندر, ڈیٹا, (word32)لمبائی);
}

ساکن خالی my_sha256_final(غیرعلامتی حرف *digest, خالی *اندر)
{
  (خالی)wc_Sha256Final(اندر, digest);
}

#وگرنہ_اگر defined(USE_GNUTLS)
#شامل <nettle/sha2.h>
#شامل <nettle/version.h>

قسمتعریف ساخت sha256_ctx my_sha256_ctx;

ساکن CURLcode my_sha256_init(خالی *ctx)
{
  sha256_init(ctx);
  واپس CURLE_OK;
}

ساکن خالی my_sha256_update(خالی *ctx,
                             ثابت غیرعلامتی حرف *ڈیٹا,
                             غیرعلامتی صحیح لمبائی)
{
  sha256_update(ctx, لمبائی, ڈیٹا);
}

ساکن خالی my_sha256_final(غیرعلامتی حرف *digest, خالی *ctx)
{
#اگر NETTLE_نسخہ_MAJOR >= 4
  sha256_digest(ctx, digest);
#ورنہ
  sha256_digest(ctx, SHA256_DIGEST_سائز, digest);
#اگر_ختم
}

#وگرنہ_اگر defined(USE_MBEDTLS) && \
  defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
#شامل <psa/crypto.h>

قسمتعریف psa_hash_operation_t my_sha256_ctx;

ساکن CURLcode my_sha256_init(خالی *ctx)
{
  psa_hash_operation_t *pctx = (psa_hash_operation_t *)ctx;
  *pctx = psa_hash_operation_init();
  اگر(psa_hash_تیاری(pctx, PSA_ALG_SHA_256) != PSA_SUCCESS)
    واپس CURLE_برآمد_کا_حافظہ;
  واپس CURLE_OK;
}

ساکن خالی my_sha256_update(خالی *ctx,
                             ثابت غیرعلامتی حرف *ڈیٹا,
                             غیرعلامتی صحیح لمبائی)
{
  (خالی)psa_hash_update(ctx, ڈیٹا, لمبائی);
}

ساکن خالی my_sha256_final(غیرعلامتی حرف *digest, خالی *ctx)
{
  size_t actual_لمبائی;
  (خالی)psa_hash_اختتام(ctx, digest, CURL_SHA256_DIGEST_لمبائی,
                        &actual_لمبائی);
}

#وگرنہ_اگر (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
               __MAC_OS_X_VERSION_MAX_ALLOWED >= 1040) || \
      (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
               __IPHONE_OS_VERSION_MAX_ALLOWED >= 20000)
#شامل <CommonCrypto/CommonDigest.h>

قسمتعریف CC_SHA256_CTX my_sha256_ctx;

ساکن CURLcode my_sha256_init(خالی *ctx)
{
  (خالی)CC_SHA256_Init(ctx);  /* always returns 1 */
  واپس CURLE_OK;
}

ساکن خالی my_sha256_update(خالی *ctx,
                             ثابت غیرعلامتی حرف *ڈیٹا,
                             غیرعلامتی صحیح لمبائی)
{
  (خالی)CC_SHA256_Update(ctx, ڈیٹا, لمبائی);
}

ساکن خالی my_sha256_final(غیرعلامتی حرف *digest, خالی *ctx)
{
  (خالی)CC_SHA256_Final(digest, ctx);
}

#وگرنہ_اگر defined(USE_WIN32_CRYPTO)
#شامل <wincrypt.h>

ساخت sha256_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
قسمتعریف ساخت sha256_ctx my_sha256_ctx;

ساکن CURLcode my_sha256_init(خالی *اندر)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)اندر;
  اگر(!CryptAcquireContext(&ctx->hCryptProv, تہی, تہی, PROV_RSA_AES,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    واپس CURLE_برآمد_کا_حافظہ;

  اگر(!CryptCreateHash(ctx->hCryptProv, CALG_SHA_256, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    واپس CURLE_FAILED_INIT;
  }

  واپس CURLE_OK;
}

ساکن خالی my_sha256_update(خالی *اندر,
                             ثابت غیرعلامتی حرف *ڈیٹا,
                             غیرعلامتی صحیح لمبائی)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)اندر;
  Crypt_Hash_ڈیٹا(ctx->hHash, (ثابت BYTE *)ڈیٹا, لمبائی, 0);
}

ساکن خالی my_sha256_final(غیرعلامتی حرف *digest, خالی *اندر)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)اندر;
  غیرعلامتی طویل لمبائی = 0;

  CryptGetHashParam(ctx->hHash, HP_HASHVAL, تہی, &لمبائی, 0);
  اگر(لمبائی == CURL_SHA256_DIGEST_لمبائی)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &لمبائی, 0);

  اگر(ctx->hHash)
    CryptDestroyHash(ctx->hHash);

  اگر(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#ورنہ

/* When no other crypto library is available we use this code segment */

/* This is based on the SHA256 implementation in LibTomCrypt that was released
 * into public domain. */

#تعریف_کرو WPA_GET_BE32(a)              \
  ((((غیرعلامتی طویل)(a)[0]) << 24) | \
   (((غیرعلامتی طویل)(a)[1]) << 16) | \
   (((غیرعلامتی طویل)(a)[2]) <<  8) | \
    ((غیرعلامتی طویل)(a)[3]))
#تعریف_کرو WPA_PUT_BE32(a, val)                                         \
  کرو {                                                               \
    (a)[0] = (غیرعلامتی حرف)((((غیرعلامتی طویل)(val)) >> 24) & 0xff); \
    (a)[1] = (غیرعلامتی حرف)((((غیرعلامتی طویل)(val)) >> 16) & 0xff); \
    (a)[2] = (غیرعلامتی حرف)((((غیرعلامتی طویل)(val)) >>  8) & 0xff); \
    (a)[3] = (غیرعلامتی حرف)(((غیرعلامتی طویل)(val)) & 0xff);         \
  } جب(0)

#تعریف_کرو WPA_PUT_BE64(a, val)                            \
  کرو {                                                  \
    (a)[0] = (غیرعلامتی حرف)(((uint64_t)(val)) >> 56);  \
    (a)[1] = (غیرعلامتی حرف)(((uint64_t)(val)) >> 48);  \
    (a)[2] = (غیرعلامتی حرف)(((uint64_t)(val)) >> 40);  \
    (a)[3] = (غیرعلامتی حرف)(((uint64_t)(val)) >> 32);  \
    (a)[4] = (غیرعلامتی حرف)(((uint64_t)(val)) >> 24);  \
    (a)[5] = (غیرعلامتی حرف)(((uint64_t)(val)) >> 16);  \
    (a)[6] = (غیرعلامتی حرف)(((uint64_t)(val)) >>  8);  \
    (a)[7] = (غیرعلامتی حرف)(((uint64_t)(val)) & 0xff); \
  } جب(0)

ساخت sha256_state {
  uint64_t لمبائی;
  غیرعلامتی طویل state[8], curlen;
  غیرعلامتی حرف buf[64];
};
قسمتعریف ساخت sha256_state my_sha256_ctx;

/* The K array */
ساکن ثابت غیرعلامتی طویل K[64] = {
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
  (((((غیرعلامتی طویل)(x) & 0xFFFFFFFFUL) >> (غیرعلامتی طویل)((y) & 31)) | \
     ((غیرعلامتی طویل)(x) << (غیرعلامتی طویل)(32 - ((y) & 31)))) & 0xFFFFFFFFUL)

#تعریف_کرو Sha256_Ch(x, y, z)  (z ^ (x & (y ^ z)))
#تعریف_کرو Sha256_Maj(x, y, z) (((x | y) & z) | (x & y))
#تعریف_کرو Sha256_S(x, n)      RORc(x, n)
#تعریف_کرو Sha256_R(x, n)      (((x) & 0xFFFFFFFFUL) >> (n))

#تعریف_کرو Sigma0(x)         (Sha256_S(x, 2) ^ Sha256_S(x, 13) ^ Sha256_S(x, 22))
#تعریف_کرو Sigma1(x)         (Sha256_S(x, 6) ^ Sha256_S(x, 11) ^ Sha256_S(x, 25))
#تعریف_کرو Gamma0(x)         (Sha256_S(x, 7) ^ Sha256_S(x, 18) ^ Sha256_R(x, 3))
#تعریف_کرو Gamma1(x)         (Sha256_S(x, 17) ^ Sha256_S(x, 19) ^ Sha256_R(x, 10))

/* Compress 512 bits */
ساکن صحیح sha256_compress(ساخت sha256_state *md, ثابت غیرعلامتی حرف *buf)
{
  غیرعلامتی طویل S[8], W[64];
  صحیح i;

  /* Copy state into S */
  برائے(i = 0; i < 8; i++) {
    S[i] = md->state[i];
  }
  /* copy the state into 512 bits into W[0..15] */
  برائے(i = 0; i < 16; i++)
    W[i] = WPA_GET_BE32(buf + (4 * i));
  /* fill W[16..63] */
  برائے(i = 16; i < 64; i++) {
    W[i] = Gamma1(W[i - 2]) + W[i - 7] + Gamma0(W[i - 15]) + W[i - 16];
  }

  /* Compress */
#تعریف_کرو RND(a, b, c, d, e, f, g, h, i)                                   \
  کرو {                                                                   \
    غیرعلامتی طویل t0 = h + Sigma1(e) + Sha256_Ch(e, f, g) + K[i] + W[i]; \
    غیرعلامتی طویل t1 = Sigma0(a) + Sha256_Maj(a, b, c);                  \
    d += t0;                                                             \
    h = t0 + t1;                                                         \
  } جب(0)

  برائے(i = 0; i < 64; ++i) {
    غیرعلامتی طویل t;
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
  برائے(i = 0; i < 8; i++) {
    md->state[i] = md->state[i] + S[i];
  }

  واپس 0;
}

/* Initialize the hash state */
ساکن CURLcode my_sha256_init(خالی *اندر)
{
  ساخت sha256_state *md = (ساخت sha256_state *)اندر;
  md->curlen = 0;
  md->لمبائی = 0;
  md->state[0] = 0x6A09E667UL;
  md->state[1] = 0xBB67AE85UL;
  md->state[2] = 0x3C6EF372UL;
  md->state[3] = 0xA54FF53AUL;
  md->state[4] = 0x510E527FUL;
  md->state[5] = 0x9B05688CUL;
  md->state[6] = 0x1F83D9ABUL;
  md->state[7] = 0x5BE0CD19UL;

  واپس CURLE_OK;
}

/* Process a block of memory though the hash
   @param md     The hash state
   @param in     The data to hash
   @param inlen  The length of the data (octets)
 */
ساکن خالی my_sha256_update(خالی *ctx,
                             ثابت غیرعلامتی حرف *اندر,
                             غیرعلامتی صحیح len)
{
  غیرعلامتی طویل inlen = len;
  غیرعلامتی طویل n;
  ساخت sha256_state *md = (ساخت sha256_state *)ctx;
#تعریف_کرو CURL_SHA256_بلاک_سائز 64
  اگر(md->curlen > حجم(md->buf))
    واپس;
  جب(inlen > 0) {
    اگر(md->curlen == 0 && inlen >= CURL_SHA256_بلاک_سائز) {
      اگر(sha256_compress(md, اندر) < 0)
        واپس;
      md->لمبائی += CURL_SHA256_بلاک_سائز * 8;
      اندر += CURL_SHA256_بلاک_سائز;
      inlen -= CURL_SHA256_بلاک_سائز;
    }
    ورنہ {
      n = CURLMIN(inlen, (CURL_SHA256_بلاک_سائز - md->curlen));
      حافظہ_نقل(md->buf + md->curlen, اندر, n);
      md->curlen += n;
      اندر += n;
      inlen -= n;
      اگر(md->curlen == CURL_SHA256_بلاک_سائز) {
        اگر(sha256_compress(md, md->buf) < 0)
          واپس;
        md->لمبائی += 8 * CURL_SHA256_بلاک_سائز;
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
ساکن خالی my_sha256_final(غیرعلامتی حرف *برآمد, خالی *ctx)
{
  ساخت sha256_state *md = ctx;
  صحیح i;

  اگر(md->curlen >= حجم(md->buf))
    واپس;

  /* Increase the length of the message */
  md->لمبائی += md->curlen * 8;

  /* Append the '1' bit */
  md->buf[md->curlen++] = (غیرعلامتی حرف)0x80;

  /* If the length is currently above 56 bytes we append zeros
   * then compress. Then we can fall back to padding zeros and length
   * encoding like normal.
   */
  اگر(md->curlen > 56) {
    جب(md->curlen < 64) {
      md->buf[md->curlen++] = 0;
    }
    sha256_compress(md, md->buf);
    md->curlen = 0;
  }

  /* Pad up to 56 bytes of zeroes */
  جب(md->curlen < 56) {
    md->buf[md->curlen++] = 0;
  }

  /* Store length */
  WPA_PUT_BE64(md->buf + 56, md->لمبائی);
  sha256_compress(md, md->buf);

  /* Copy output */
  برائے(i = 0; i < 8; i++)
    WPA_PUT_BE32(برآمد + (4 * i), md->state[i]);
}

#اگر_ختم /* CRYPTO LIBS */

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
CURLcode Curl_sha256it(غیرعلامتی حرف *output, ثابت غیرعلامتی حرف *داخلہ,
                       size_t len)
{
  CURLcode نتیجہ;
  my_sha256_ctx ctx;

  نتیجہ = my_sha256_init(&ctx);
  اگر(!نتیجہ) {
    کرو {
      غیرعلامتی صحیح ilen = (غیرعلامتی صحیح)CURLMIN(len, UINT_MAX);
      my_sha256_update(&ctx, داخلہ, ilen);
      len -= ilen;
      داخلہ += ilen;
    } جب(len);
    my_sha256_final(output, &ctx);
  }
  واپس نتیجہ;
}

ثابت ساخت HMAC_params Curl_HMAC_SHA256 = {
  my_sha256_init,        /* Hash initialization function. */
  my_sha256_update,      /* Hash update function. */
  my_sha256_final,       /* Hash computation end function. */
  حجم(my_sha256_ctx), /* Size of hash context structure. */
  64,                    /* Maximum key length. */
  32                     /* Result size. */
};

#اگر_ختم /* AWS, DIGEST, or libssh2 */
