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
#અન્તર્ભાવ "curl_setup.h"

#જો !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#અન્તર્ભાવ "curl_sha256.h"

#ifdef USE_MBEDTLS
#અન્તર્ભાવ <mbedtls/version.h>
#જો MBEDTLS_VERSION_સંખ્યા < 0x03020000
#error "mbedTLS 3.2.0 or greater required"
#endif
#અન્તર્ભાવ <psa/crypto_config.h>
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
#અન્તર્ભાવ <openssl/evp.h>

સંરચના ossl_sha256_ctx {
  EVP_MD_CTX *openssl_ctx;
};
પ્રકારવ્યાખ્યા સંરચના ossl_sha256_ctx my_sha256_ctx;

સ્થિર CURLcode my_sha256_init(શૂન્ય *in)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  ctx->openssl_ctx = EVP_MD_CTX_new();
  જો(!ctx->openssl_ctx)
    પાછા CURLE_OUT_OF_MEMORY;

  જો(!EVP_DigestInit_ex(ctx->openssl_ctx, EVP_sha256(), ખાલી)) {
    EVP_MD_CTX_free(ctx->openssl_ctx);
    પાછા CURLE_FAILED_INIT;
  }
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય my_sha256_update(શૂન્ય *in,
                             અચલ unsigned અક્ષર *data,
                             unsigned પૂર્ણાંક length)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  EVP_DigestUpdate(ctx->openssl_ctx, data, length);
}

સ્થિર શૂન્ય my_sha256_final(unsigned અક્ષર *digest, શૂન્ય *in)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  EVP_DigestFinal_ex(ctx->openssl_ctx, digest, ખાલી);
  EVP_MD_CTX_free(ctx->openssl_ctx);
}

#અથવાજો defined(USE_WOLFSSL)
#અન્તર્ભાવ <wolfssl/options.h>
#અન્તર્ભાવ <wolfssl/wolfcrypt/sha256.h>

પ્રકારવ્યાખ્યા સંરચના wc_Sha256 my_sha256_ctx;

સ્થિર CURLcode my_sha256_init(શૂન્ય *in)
{
  જો(wc_InitSha256(in))
    પાછા CURLE_FAILED_INIT;
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય my_sha256_update(શૂન્ય *in,
                             અચલ unsigned અક્ષર *data,
                             unsigned પૂર્ણાંક length)
{
  (શૂન્ય)wc_Sha256Update(in, data, (word32)length);
}

સ્થિર શૂન્ય my_sha256_final(unsigned અક્ષર *digest, શૂન્ય *in)
{
  (શૂન્ય)wc_Sha256Final(in, digest);
}

#અથવાજો defined(USE_GNUTLS)
#અન્તર્ભાવ <nettle/sha2.h>
#અન્તર્ભાવ <nettle/version.h>

પ્રકારવ્યાખ્યા સંરચના sha256_ctx my_sha256_ctx;

સ્થિર CURLcode my_sha256_init(શૂન્ય *ctx)
{
  sha256_init(ctx);
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય my_sha256_update(શૂન્ય *ctx,
                             અચલ unsigned અક્ષર *data,
                             unsigned પૂર્ણાંક length)
{
  sha256_update(ctx, length, data);
}

સ્થિર શૂન્ય my_sha256_final(unsigned અક્ષર *digest, શૂન્ય *ctx)
{
#જો NETTLE_VERSION_MAJOR >= 4
  sha256_digest(ctx, digest);
#નહીંતર
  sha256_digest(ctx, SHA256_DIGEST_SIZE, digest);
#endif
}

#અથવાજો defined(USE_MBEDTLS) && \
  defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
#અન્તર્ભાવ <psa/crypto.h>

પ્રકારવ્યાખ્યા psa_hash_operation_t my_sha256_ctx;

સ્થિર CURLcode my_sha256_init(શૂન્ય *ctx)
{
  psa_hash_operation_t *pctx = (psa_hash_operation_t *)ctx;
  *pctx = psa_hash_operation_init();
  જો(psa_hash_setup(pctx, PSA_ALG_SHA_256) != PSA_SUCCESS)
    પાછા CURLE_OUT_OF_MEMORY;
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય my_sha256_update(શૂન્ય *ctx,
                             અચલ unsigned અક્ષર *data,
                             unsigned પૂર્ણાંક length)
{
  (શૂન્ય)psa_hash_update(ctx, data, length);
}

સ્થિર શૂન્ય my_sha256_final(unsigned અક્ષર *digest, શૂન્ય *ctx)
{
  size_t actual_length;
  (શૂન્ય)psa_hash_finish(ctx, digest, CURL_SHA256_DIGEST_LENGTH,
                        &actual_length);
}

#અથવાજો (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
               __MAC_OS_X_VERSION_MAX_ALLOWED >= 1040) || \
      (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
               __IPHONE_OS_VERSION_MAX_ALLOWED >= 20000)
#અન્તર્ભાવ <CommonCrypto/CommonDigest.h>

પ્રકારવ્યાખ્યા CC_SHA256_CTX my_sha256_ctx;

સ્થિર CURLcode my_sha256_init(શૂન્ય *ctx)
{
  (શૂન્ય)CC_SHA256_Init(ctx);  /* always returns 1 */
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય my_sha256_update(શૂન્ય *ctx,
                             અચલ unsigned અક્ષર *data,
                             unsigned પૂર્ણાંક length)
{
  (શૂન્ય)CC_SHA256_Update(ctx, data, length);
}

સ્થિર શૂન્ય my_sha256_final(unsigned અક્ષર *digest, શૂન્ય *ctx)
{
  (શૂન્ય)CC_SHA256_Final(digest, ctx);
}

#અથવાજો defined(USE_WIN32_CRYPTO)
#અન્તર્ભાવ <wincrypt.h>

સંરચના sha256_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
પ્રકારવ્યાખ્યા સંરચના sha256_ctx my_sha256_ctx;

સ્થિર CURLcode my_sha256_init(શૂન્ય *in)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  જો(!CryptAcquireContext(&ctx->hCryptProv, ખાલી, ખાલી, PROV_RSA_AES,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    પાછા CURLE_OUT_OF_MEMORY;

  જો(!CryptCreateHash(ctx->hCryptProv, CALG_SHA_256, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    પાછા CURLE_FAILED_INIT;
  }

  પાછા CURLE_OK;
}

સ્થિર શૂન્ય my_sha256_update(શૂન્ય *in,
                             અચલ unsigned અક્ષર *data,
                             unsigned પૂર્ણાંક length)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  CryptHashData(ctx->hHash, (અચલ BYTE *)data, length, 0);
}

સ્થિર શૂન્ય my_sha256_final(unsigned અક્ષર *digest, શૂન્ય *in)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  unsigned long length = 0;

  CryptGetHashParam(ctx->hHash, HP_HASHVAL, ખાલી, &length, 0);
  જો(length == CURL_SHA256_DIGEST_LENGTH)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &length, 0);

  જો(ctx->hHash)
    CryptDestroyHash(ctx->hHash);

  જો(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#નહીંતર

/* When no other crypto library is available we use this code segment */

/* This is based on the SHA256 implementation in LibTomCrypt that was released
 * into public domain. */

#define WPA_GET_BE32(a)              \
  ((((unsigned long)(a)[0]) << 24) | \
   (((unsigned long)(a)[1]) << 16) | \
   (((unsigned long)(a)[2]) <<  8) | \
    ((unsigned long)(a)[3]))
#define WPA_PUT_BE32(a, val)                                         \
  કરો {                                                               \
    (a)[0] = (unsigned અક્ષર)((((unsigned long)(val)) >> 24) & 0xff); \
    (a)[1] = (unsigned અક્ષર)((((unsigned long)(val)) >> 16) & 0xff); \
    (a)[2] = (unsigned અક્ષર)((((unsigned long)(val)) >>  8) & 0xff); \
    (a)[3] = (unsigned અક્ષર)(((unsigned long)(val)) & 0xff);         \
  } જ્યારે(0)

#define WPA_PUT_BE64(a, val)                            \
  કરો {                                                  \
    (a)[0] = (unsigned અક્ષર)(((uint64_t)(val)) >> 56);  \
    (a)[1] = (unsigned અક્ષર)(((uint64_t)(val)) >> 48);  \
    (a)[2] = (unsigned અક્ષર)(((uint64_t)(val)) >> 40);  \
    (a)[3] = (unsigned અક્ષર)(((uint64_t)(val)) >> 32);  \
    (a)[4] = (unsigned અક્ષર)(((uint64_t)(val)) >> 24);  \
    (a)[5] = (unsigned અક્ષર)(((uint64_t)(val)) >> 16);  \
    (a)[6] = (unsigned અક્ષર)(((uint64_t)(val)) >>  8);  \
    (a)[7] = (unsigned અક્ષર)(((uint64_t)(val)) & 0xff); \
  } જ્યારે(0)

સંરચના sha256_state {
  uint64_t length;
  unsigned long state[8], curlen;
  unsigned અક્ષર buf[64];
};
પ્રકારવ્યાખ્યા સંરચના sha256_state my_sha256_ctx;

/* The K array */
સ્થિર અચલ unsigned long K[64] = {
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
સ્થિર પૂર્ણાંક sha256_compress(સંરચના sha256_state *md, અચલ unsigned અક્ષર *buf)
{
  unsigned long S[8], W[64];
  પૂર્ણાંક i;

  /* Copy state into S */
  માટે(i = 0; i < 8; i++) {
    S[i] = md->state[i];
  }
  /* copy the state into 512 bits into W[0..15] */
  માટે(i = 0; i < 16; i++)
    W[i] = WPA_GET_BE32(buf + (4 * i));
  /* fill W[16..63] */
  માટે(i = 16; i < 64; i++) {
    W[i] = Gamma1(W[i - 2]) + W[i - 7] + Gamma0(W[i - 15]) + W[i - 16];
  }

  /* Compress */
#define RND(a, b, c, d, e, f, g, h, i)                                   \
  કરો {                                                                   \
    unsigned long t0 = h + Sigma1(e) + Sha256_Ch(e, f, g) + K[i] + W[i]; \
    unsigned long t1 = Sigma0(a) + Sha256_Maj(a, b, c);                  \
    d += t0;                                                             \
    h = t0 + t1;                                                         \
  } જ્યારે(0)

  માટે(i = 0; i < 64; ++i) {
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
  માટે(i = 0; i < 8; i++) {
    md->state[i] = md->state[i] + S[i];
  }

  પાછા 0;
}

/* Initialize the hash state */
સ્થિર CURLcode my_sha256_init(શૂન્ય *in)
{
  સંરચના sha256_state *md = (સંરચના sha256_state *)in;
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

  પાછા CURLE_OK;
}

/* Process a block of memory though the hash
   @param md     The hash state
   @param in     The data to hash
   @param inlen  The length of the data (octets)
 */
સ્થિર શૂન્ય my_sha256_update(શૂન્ય *ctx,
                             અચલ unsigned અક્ષર *in,
                             unsigned પૂર્ણાંક len)
{
  unsigned long inlen = len;
  unsigned long n;
  સંરચના sha256_state *md = (સંરચના sha256_state *)ctx;
#define CURL_SHA256_BLOCK_SIZE 64
  જો(md->curlen > કદ(md->buf))
    પાછા;
  જ્યારે(inlen > 0) {
    જો(md->curlen == 0 && inlen >= CURL_SHA256_BLOCK_SIZE) {
      જો(sha256_compress(md, in) < 0)
        પાછા;
      md->length += CURL_SHA256_BLOCK_SIZE * 8;
      in += CURL_SHA256_BLOCK_SIZE;
      inlen -= CURL_SHA256_BLOCK_SIZE;
    }
    નહીંતર {
      n = CURLMIN(inlen, (CURL_SHA256_BLOCK_SIZE - md->curlen));
      memcpy(md->buf + md->curlen, in, n);
      md->curlen += n;
      in += n;
      inlen -= n;
      જો(md->curlen == CURL_SHA256_BLOCK_SIZE) {
        જો(sha256_compress(md, md->buf) < 0)
          પાછા;
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
સ્થિર શૂન્ય my_sha256_final(unsigned અક્ષર *out, શૂન્ય *ctx)
{
  સંરચના sha256_state *md = ctx;
  પૂર્ણાંક i;

  જો(md->curlen >= કદ(md->buf))
    પાછા;

  /* Increase the length of the message */
  md->length += md->curlen * 8;

  /* Append the '1' bit */
  md->buf[md->curlen++] = (unsigned અક્ષર)0x80;

  /* If the length is currently above 56 bytes we append zeros
   * then compress. Then we can fall back to padding zeros and length
   * encoding like normal.
   */
  જો(md->curlen > 56) {
    જ્યારે(md->curlen < 64) {
      md->buf[md->curlen++] = 0;
    }
    sha256_compress(md, md->buf);
    md->curlen = 0;
  }

  /* Pad up to 56 bytes of zeroes */
  જ્યારે(md->curlen < 56) {
    md->buf[md->curlen++] = 0;
  }

  /* Store length */
  WPA_PUT_BE64(md->buf + 56, md->length);
  sha256_compress(md, md->buf);

  /* Copy output */
  માટે(i = 0; i < 8; i++)
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
CURLcode Curl_sha256it(unsigned અક્ષર *output, અચલ unsigned અક્ષર *આગત,
                       size_t len)
{
  CURLcode result;
  my_sha256_ctx ctx;

  result = my_sha256_init(&ctx);
  જો(!result) {
    કરો {
      unsigned પૂર્ણાંક ilen = (unsigned પૂર્ણાંક)CURLMIN(len, UINT_MAX);
      my_sha256_update(&ctx, આગત, ilen);
      len -= ilen;
      આગત += ilen;
    } જ્યારે(len);
    my_sha256_final(output, &ctx);
  }
  પાછા result;
}

અચલ સંરચના HMAC_params Curl_HMAC_SHA256 = {
  my_sha256_init,        /* Hash initialization function. */
  my_sha256_update,      /* Hash update function. */
  my_sha256_final,       /* Hash computation end function. */
  કદ(my_sha256_ctx), /* Size of hash context structure. */
  64,                    /* Maximum key length. */
  32                     /* Result size. */
};

#endif /* AWS, DIGEST, or libssh2 */
