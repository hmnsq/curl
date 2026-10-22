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
#অন্তর্ভাব "curl_setup.h"

#যদি !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#অন্তর্ভাব "curl_sha256.h"

#ifdef USE_MBEDTLS
#অন্তর্ভাব <mbedtls/version.h>
#যদি MBEDTLS_VERSION_সংখ্যা < 0x03020000
#error "mbedTLS 3.2.0 or greater required"
#endif
#অন্তর্ভাব <psa/crypto_config.h>
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
#অন্তর্ভাব <openssl/evp.h>

সংরচনা ossl_sha256_ctx {
  EVP_MD_CTX *openssl_ctx;
};
প্রকারনির্দেশ সংরচনা ossl_sha256_ctx my_sha256_ctx;

স্থির CURLcode my_sha256_init(শূন্য *in)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  ctx->openssl_ctx = EVP_MD_CTX_new();
  যদি(!ctx->openssl_ctx)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  যদি(!EVP_DigestInit_ex(ctx->openssl_ctx, EVP_sha256(), রিক্ত)) {
    EVP_MD_CTX_free(ctx->openssl_ctx);
    প্রত্যাগম CURLE_FAILED_INIT;
  }
  প্রত্যাগম CURLE_OK;
}

স্থির শূন্য my_sha256_update(শূন্য *in,
                             ধ্রুব unsigned বর্ণ *data,
                             unsigned পূর্ণাঙ্ক length)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  EVP_DigestUpdate(ctx->openssl_ctx, data, length);
}

স্থির শূন্য my_sha256_final(unsigned বর্ণ *digest, শূন্য *in)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  EVP_DigestFinal_ex(ctx->openssl_ctx, digest, রিক্ত);
  EVP_MD_CTX_free(ctx->openssl_ctx);
}

#নোচেদ্যদি defined(USE_WOLFSSL)
#অন্তর্ভাব <wolfssl/options.h>
#অন্তর্ভাব <wolfssl/wolfcrypt/sha256.h>

প্রকারনির্দেশ সংরচনা wc_Sha256 my_sha256_ctx;

স্থির CURLcode my_sha256_init(শূন্য *in)
{
  যদি(wc_InitSha256(in))
    প্রত্যাগম CURLE_FAILED_INIT;
  প্রত্যাগম CURLE_OK;
}

স্থির শূন্য my_sha256_update(শূন্য *in,
                             ধ্রুব unsigned বর্ণ *data,
                             unsigned পূর্ণাঙ্ক length)
{
  (শূন্য)wc_Sha256Update(in, data, (word32)length);
}

স্থির শূন্য my_sha256_final(unsigned বর্ণ *digest, শূন্য *in)
{
  (শূন্য)wc_Sha256Final(in, digest);
}

#নোচেদ্যদি defined(USE_GNUTLS)
#অন্তর্ভাব <nettle/sha2.h>
#অন্তর্ভাব <nettle/version.h>

প্রকারনির্দেশ সংরচনা sha256_ctx my_sha256_ctx;

স্থির CURLcode my_sha256_init(শূন্য *ctx)
{
  sha256_init(ctx);
  প্রত্যাগম CURLE_OK;
}

স্থির শূন্য my_sha256_update(শূন্য *ctx,
                             ধ্রুব unsigned বর্ণ *data,
                             unsigned পূর্ণাঙ্ক length)
{
  sha256_update(ctx, length, data);
}

স্থির শূন্য my_sha256_final(unsigned বর্ণ *digest, শূন্য *ctx)
{
#যদি NETTLE_VERSION_MAJOR >= 4
  sha256_digest(ctx, digest);
#অন্যথা
  sha256_digest(ctx, SHA256_DIGEST_SIZE, digest);
#endif
}

#নোচেদ্যদি defined(USE_MBEDTLS) && \
  defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
#অন্তর্ভাব <psa/crypto.h>

প্রকারনির্দেশ psa_hash_operation_t my_sha256_ctx;

স্থির CURLcode my_sha256_init(শূন্য *ctx)
{
  psa_hash_operation_t *pctx = (psa_hash_operation_t *)ctx;
  *pctx = psa_hash_operation_init();
  যদি(psa_hash_setup(pctx, PSA_ALG_SHA_256) != PSA_SUCCESS)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;
  প্রত্যাগম CURLE_OK;
}

স্থির শূন্য my_sha256_update(শূন্য *ctx,
                             ধ্রুব unsigned বর্ণ *data,
                             unsigned পূর্ণাঙ্ক length)
{
  (শূন্য)psa_hash_update(ctx, data, length);
}

স্থির শূন্য my_sha256_final(unsigned বর্ণ *digest, শূন্য *ctx)
{
  size_t actual_length;
  (শূন্য)psa_hash_finish(ctx, digest, CURL_SHA256_DIGEST_LENGTH,
                        &actual_length);
}

#নোচেদ্যদি (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
               __MAC_OS_X_VERSION_MAX_ALLOWED >= 1040) || \
      (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
               __IPHONE_OS_VERSION_MAX_ALLOWED >= 20000)
#অন্তর্ভাব <CommonCrypto/CommonDigest.h>

প্রকারনির্দেশ CC_SHA256_CTX my_sha256_ctx;

স্থির CURLcode my_sha256_init(শূন্য *ctx)
{
  (শূন্য)CC_SHA256_Init(ctx);  /* always returns 1 */
  প্রত্যাগম CURLE_OK;
}

স্থির শূন্য my_sha256_update(শূন্য *ctx,
                             ধ্রুব unsigned বর্ণ *data,
                             unsigned পূর্ণাঙ্ক length)
{
  (শূন্য)CC_SHA256_Update(ctx, data, length);
}

স্থির শূন্য my_sha256_final(unsigned বর্ণ *digest, শূন্য *ctx)
{
  (শূন্য)CC_SHA256_Final(digest, ctx);
}

#নোচেদ্যদি defined(USE_WIN32_CRYPTO)
#অন্তর্ভাব <wincrypt.h>

সংরচনা sha256_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
প্রকারনির্দেশ সংরচনা sha256_ctx my_sha256_ctx;

স্থির CURLcode my_sha256_init(শূন্য *in)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  যদি(!CryptAcquireContext(&ctx->hCryptProv, রিক্ত, রিক্ত, PROV_RSA_AES,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  যদি(!CryptCreateHash(ctx->hCryptProv, CALG_SHA_256, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    প্রত্যাগম CURLE_FAILED_INIT;
  }

  প্রত্যাগম CURLE_OK;
}

স্থির শূন্য my_sha256_update(শূন্য *in,
                             ধ্রুব unsigned বর্ণ *data,
                             unsigned পূর্ণাঙ্ক length)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  CryptHashData(ctx->hHash, (ধ্রুব BYTE *)data, length, 0);
}

স্থির শূন্য my_sha256_final(unsigned বর্ণ *digest, শূন্য *in)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  unsigned long length = 0;

  CryptGetHashParam(ctx->hHash, HP_HASHVAL, রিক্ত, &length, 0);
  যদি(length == CURL_SHA256_DIGEST_LENGTH)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &length, 0);

  যদি(ctx->hHash)
    CryptDestroyHash(ctx->hHash);

  যদি(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#অন্যথা

/* When no other crypto library is available we use this code segment */

/* This is based on the SHA256 implementation in LibTomCrypt that was released
 * into public domain. */

#define WPA_GET_BE32(a)              \
  ((((unsigned long)(a)[0]) << 24) | \
   (((unsigned long)(a)[1]) << 16) | \
   (((unsigned long)(a)[2]) <<  8) | \
    ((unsigned long)(a)[3]))
#define WPA_PUT_BE32(a, val)                                         \
  কুরু {                                                               \
    (a)[0] = (unsigned বর্ণ)((((unsigned long)(val)) >> 24) & 0xff); \
    (a)[1] = (unsigned বর্ণ)((((unsigned long)(val)) >> 16) & 0xff); \
    (a)[2] = (unsigned বর্ণ)((((unsigned long)(val)) >>  8) & 0xff); \
    (a)[3] = (unsigned বর্ণ)(((unsigned long)(val)) & 0xff);         \
  } যাবত্(0)

#define WPA_PUT_BE64(a, val)                            \
  কুরু {                                                  \
    (a)[0] = (unsigned বর্ণ)(((uint64_t)(val)) >> 56);  \
    (a)[1] = (unsigned বর্ণ)(((uint64_t)(val)) >> 48);  \
    (a)[2] = (unsigned বর্ণ)(((uint64_t)(val)) >> 40);  \
    (a)[3] = (unsigned বর্ণ)(((uint64_t)(val)) >> 32);  \
    (a)[4] = (unsigned বর্ণ)(((uint64_t)(val)) >> 24);  \
    (a)[5] = (unsigned বর্ণ)(((uint64_t)(val)) >> 16);  \
    (a)[6] = (unsigned বর্ণ)(((uint64_t)(val)) >>  8);  \
    (a)[7] = (unsigned বর্ণ)(((uint64_t)(val)) & 0xff); \
  } যাবত্(0)

সংরচনা sha256_state {
  uint64_t length;
  unsigned long state[8], curlen;
  unsigned বর্ণ buf[64];
};
প্রকারনির্দেশ সংরচনা sha256_state my_sha256_ctx;

/* The K array */
স্থির ধ্রুব unsigned long K[64] = {
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
স্থির পূর্ণাঙ্ক sha256_compress(সংরচনা sha256_state *md, ধ্রুব unsigned বর্ণ *buf)
{
  unsigned long S[8], W[64];
  পূর্ণাঙ্ক i;

  /* Copy state into S */
  প্রতি(i = 0; i < 8; i++) {
    S[i] = md->state[i];
  }
  /* copy the state into 512 bits into W[0..15] */
  প্রতি(i = 0; i < 16; i++)
    W[i] = WPA_GET_BE32(buf + (4 * i));
  /* fill W[16..63] */
  প্রতি(i = 16; i < 64; i++) {
    W[i] = Gamma1(W[i - 2]) + W[i - 7] + Gamma0(W[i - 15]) + W[i - 16];
  }

  /* Compress */
#define RND(a, b, c, d, e, f, g, h, i)                                   \
  কুরু {                                                                   \
    unsigned long t0 = h + Sigma1(e) + Sha256_Ch(e, f, g) + K[i] + W[i]; \
    unsigned long t1 = Sigma0(a) + Sha256_Maj(a, b, c);                  \
    d += t0;                                                             \
    h = t0 + t1;                                                         \
  } যাবত্(0)

  প্রতি(i = 0; i < 64; ++i) {
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
  প্রতি(i = 0; i < 8; i++) {
    md->state[i] = md->state[i] + S[i];
  }

  প্রত্যাগম 0;
}

/* Initialize the hash state */
স্থির CURLcode my_sha256_init(শূন্য *in)
{
  সংরচনা sha256_state *md = (সংরচনা sha256_state *)in;
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

  প্রত্যাগম CURLE_OK;
}

/* Process a block of memory though the hash
   @param md     The hash state
   @param in     The data to hash
   @param inlen  The length of the data (octets)
 */
স্থির শূন্য my_sha256_update(শূন্য *ctx,
                             ধ্রুব unsigned বর্ণ *in,
                             unsigned পূর্ণাঙ্ক len)
{
  unsigned long inlen = len;
  unsigned long n;
  সংরচনা sha256_state *md = (সংরচনা sha256_state *)ctx;
#define CURL_SHA256_BLOCK_SIZE 64
  যদি(md->curlen > পরিমাণ(md->buf))
    প্রত্যাগম;
  যাবত্(inlen > 0) {
    যদি(md->curlen == 0 && inlen >= CURL_SHA256_BLOCK_SIZE) {
      যদি(sha256_compress(md, in) < 0)
        প্রত্যাগম;
      md->length += CURL_SHA256_BLOCK_SIZE * 8;
      in += CURL_SHA256_BLOCK_SIZE;
      inlen -= CURL_SHA256_BLOCK_SIZE;
    }
    অন্যথা {
      n = CURLMIN(inlen, (CURL_SHA256_BLOCK_SIZE - md->curlen));
      memcpy(md->buf + md->curlen, in, n);
      md->curlen += n;
      in += n;
      inlen -= n;
      যদি(md->curlen == CURL_SHA256_BLOCK_SIZE) {
        যদি(sha256_compress(md, md->buf) < 0)
          প্রত্যাগম;
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
স্থির শূন্য my_sha256_final(unsigned বর্ণ *out, শূন্য *ctx)
{
  সংরচনা sha256_state *md = ctx;
  পূর্ণাঙ্ক i;

  যদি(md->curlen >= পরিমাণ(md->buf))
    প্রত্যাগম;

  /* Increase the length of the message */
  md->length += md->curlen * 8;

  /* Append the '1' bit */
  md->buf[md->curlen++] = (unsigned বর্ণ)0x80;

  /* If the length is currently above 56 bytes we append zeros
   * then compress. Then we can fall back to padding zeros and length
   * encoding like normal.
   */
  যদি(md->curlen > 56) {
    যাবত্(md->curlen < 64) {
      md->buf[md->curlen++] = 0;
    }
    sha256_compress(md, md->buf);
    md->curlen = 0;
  }

  /* Pad up to 56 bytes of zeroes */
  যাবত্(md->curlen < 56) {
    md->buf[md->curlen++] = 0;
  }

  /* Store length */
  WPA_PUT_BE64(md->buf + 56, md->length);
  sha256_compress(md, md->buf);

  /* Copy output */
  প্রতি(i = 0; i < 8; i++)
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
CURLcode Curl_sha256it(unsigned বর্ণ *output, ধ্রুব unsigned বর্ণ *প্রবেশ,
                       size_t len)
{
  CURLcode result;
  my_sha256_ctx ctx;

  result = my_sha256_init(&ctx);
  যদি(!result) {
    কুরু {
      unsigned পূর্ণাঙ্ক ilen = (unsigned পূর্ণাঙ্ক)CURLMIN(len, UINT_MAX);
      my_sha256_update(&ctx, প্রবেশ, ilen);
      len -= ilen;
      প্রবেশ += ilen;
    } যাবত্(len);
    my_sha256_final(output, &ctx);
  }
  প্রত্যাগম result;
}

ধ্রুব সংরচনা HMAC_params Curl_HMAC_SHA256 = {
  my_sha256_init,        /* Hash initialization function. */
  my_sha256_update,      /* Hash update function. */
  my_sha256_final,       /* Hash computation end function. */
  পরিমাণ(my_sha256_ctx), /* Size of hash context structure. */
  64,                    /* Maximum key length. */
  32                     /* Result size. */
};

#endif /* AWS, DIGEST, or libssh2 */
