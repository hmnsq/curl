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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#അന്തര്ഭാവ "curl_sha256.h"

#ifdef USE_MBEDTLS
#അന്തര്ഭാവ <mbedtls/version.h>
#എങ്കിൽ MBEDTLS_VERSION_സംഖ്യ < 0x03020000
#error "mbedTLS 3.2.0 or greater required"
#endif
#അന്തര്ഭാവ <psa/crypto_config.h>
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
#അന്തര്ഭാവ <openssl/evp.h>

ഘടന ossl_sha256_ctx {
  EVP_MD_CTX *openssl_ctx;
};
തരനിർവചനം ഘടന ossl_sha256_ctx my_sha256_ctx;

സ്ഥിതം CURLcode my_sha256_init(ശൂന്യം *in)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  ctx->openssl_ctx = EVP_MD_CTX_new();
  എങ്കിൽ(!ctx->openssl_ctx)
    തിരികെ CURLE_OUT_OF_MEMORY;

  എങ്കിൽ(!EVP_DigestInit_ex(ctx->openssl_ctx, EVP_sha256(), ശൂന്യ)) {
    EVP_MD_CTX_free(ctx->openssl_ctx);
    തിരികെ CURLE_FAILED_INIT;
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം my_sha256_update(ശൂന്യം *in,
                             സ്ഥിരം unsigned അക്ഷരം *data,
                             unsigned പൂർണ്ണസംഖ്യ length)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  EVP_DigestUpdate(ctx->openssl_ctx, data, length);
}

സ്ഥിതം ശൂന്യം my_sha256_final(unsigned അക്ഷരം *digest, ശൂന്യം *in)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  EVP_DigestFinal_ex(ctx->openssl_ctx, digest, ശൂന്യ);
  EVP_MD_CTX_free(ctx->openssl_ctx);
}

#അല്ലെങ്കിലെങ്കിൽ defined(USE_WOLFSSL)
#അന്തര്ഭാവ <wolfssl/options.h>
#അന്തര്ഭാവ <wolfssl/wolfcrypt/sha256.h>

തരനിർവചനം ഘടന wc_Sha256 my_sha256_ctx;

സ്ഥിതം CURLcode my_sha256_init(ശൂന്യം *in)
{
  എങ്കിൽ(wc_InitSha256(in))
    തിരികെ CURLE_FAILED_INIT;
  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം my_sha256_update(ശൂന്യം *in,
                             സ്ഥിരം unsigned അക്ഷരം *data,
                             unsigned പൂർണ്ണസംഖ്യ length)
{
  (ശൂന്യം)wc_Sha256Update(in, data, (word32)length);
}

സ്ഥിതം ശൂന്യം my_sha256_final(unsigned അക്ഷരം *digest, ശൂന്യം *in)
{
  (ശൂന്യം)wc_Sha256Final(in, digest);
}

#അല്ലെങ്കിലെങ്കിൽ defined(USE_GNUTLS)
#അന്തര്ഭാവ <nettle/sha2.h>
#അന്തര്ഭാവ <nettle/version.h>

തരനിർവചനം ഘടന sha256_ctx my_sha256_ctx;

സ്ഥിതം CURLcode my_sha256_init(ശൂന്യം *ctx)
{
  sha256_init(ctx);
  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം my_sha256_update(ശൂന്യം *ctx,
                             സ്ഥിരം unsigned അക്ഷരം *data,
                             unsigned പൂർണ്ണസംഖ്യ length)
{
  sha256_update(ctx, length, data);
}

സ്ഥിതം ശൂന്യം my_sha256_final(unsigned അക്ഷരം *digest, ശൂന്യം *ctx)
{
#എങ്കിൽ NETTLE_VERSION_MAJOR >= 4
  sha256_digest(ctx, digest);
#അല്ലെങ്കിൽ
  sha256_digest(ctx, SHA256_DIGEST_SIZE, digest);
#endif
}

#അല്ലെങ്കിലെങ്കിൽ defined(USE_MBEDTLS) && \
  defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
#അന്തര്ഭാവ <psa/crypto.h>

തരനിർവചനം psa_hash_operation_t my_sha256_ctx;

സ്ഥിതം CURLcode my_sha256_init(ശൂന്യം *ctx)
{
  psa_hash_operation_t *pctx = (psa_hash_operation_t *)ctx;
  *pctx = psa_hash_operation_init();
  എങ്കിൽ(psa_hash_setup(pctx, PSA_ALG_SHA_256) != PSA_SUCCESS)
    തിരികെ CURLE_OUT_OF_MEMORY;
  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം my_sha256_update(ശൂന്യം *ctx,
                             സ്ഥിരം unsigned അക്ഷരം *data,
                             unsigned പൂർണ്ണസംഖ്യ length)
{
  (ശൂന്യം)psa_hash_update(ctx, data, length);
}

സ്ഥിതം ശൂന്യം my_sha256_final(unsigned അക്ഷരം *digest, ശൂന്യം *ctx)
{
  size_t actual_length;
  (ശൂന്യം)psa_hash_finish(ctx, digest, CURL_SHA256_DIGEST_LENGTH,
                        &actual_length);
}

#അല്ലെങ്കിലെങ്കിൽ (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
               __MAC_OS_X_VERSION_MAX_ALLOWED >= 1040) || \
      (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
               __IPHONE_OS_VERSION_MAX_ALLOWED >= 20000)
#അന്തര്ഭാവ <CommonCrypto/CommonDigest.h>

തരനിർവചനം CC_SHA256_CTX my_sha256_ctx;

സ്ഥിതം CURLcode my_sha256_init(ശൂന്യം *ctx)
{
  (ശൂന്യം)CC_SHA256_Init(ctx);  /* always returns 1 */
  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം my_sha256_update(ശൂന്യം *ctx,
                             സ്ഥിരം unsigned അക്ഷരം *data,
                             unsigned പൂർണ്ണസംഖ്യ length)
{
  (ശൂന്യം)CC_SHA256_Update(ctx, data, length);
}

സ്ഥിതം ശൂന്യം my_sha256_final(unsigned അക്ഷരം *digest, ശൂന്യം *ctx)
{
  (ശൂന്യം)CC_SHA256_Final(digest, ctx);
}

#അല്ലെങ്കിലെങ്കിൽ defined(USE_WIN32_CRYPTO)
#അന്തര്ഭാവ <wincrypt.h>

ഘടന sha256_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
തരനിർവചനം ഘടന sha256_ctx my_sha256_ctx;

സ്ഥിതം CURLcode my_sha256_init(ശൂന്യം *in)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  എങ്കിൽ(!CryptAcquireContext(&ctx->hCryptProv, ശൂന്യ, ശൂന്യ, PROV_RSA_AES,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    തിരികെ CURLE_OUT_OF_MEMORY;

  എങ്കിൽ(!CryptCreateHash(ctx->hCryptProv, CALG_SHA_256, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    തിരികെ CURLE_FAILED_INIT;
  }

  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം my_sha256_update(ശൂന്യം *in,
                             സ്ഥിരം unsigned അക്ഷരം *data,
                             unsigned പൂർണ്ണസംഖ്യ length)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  CryptHashData(ctx->hHash, (സ്ഥിരം BYTE *)data, length, 0);
}

സ്ഥിതം ശൂന്യം my_sha256_final(unsigned അക്ഷരം *digest, ശൂന്യം *in)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  unsigned long length = 0;

  CryptGetHashParam(ctx->hHash, HP_HASHVAL, ശൂന്യ, &length, 0);
  എങ്കിൽ(length == CURL_SHA256_DIGEST_LENGTH)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &length, 0);

  എങ്കിൽ(ctx->hHash)
    CryptDestroyHash(ctx->hHash);

  എങ്കിൽ(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#അല്ലെങ്കിൽ

/* When no other crypto library is available we use this code segment */

/* This is based on the SHA256 implementation in LibTomCrypt that was released
 * into public domain. */

#define WPA_GET_BE32(a)              \
  ((((unsigned long)(a)[0]) << 24) | \
   (((unsigned long)(a)[1]) << 16) | \
   (((unsigned long)(a)[2]) <<  8) | \
    ((unsigned long)(a)[3]))
#define WPA_PUT_BE32(a, val)                                         \
  ചെയ്യുക {                                                               \
    (a)[0] = (unsigned അക്ഷരം)((((unsigned long)(val)) >> 24) & 0xff); \
    (a)[1] = (unsigned അക്ഷരം)((((unsigned long)(val)) >> 16) & 0xff); \
    (a)[2] = (unsigned അക്ഷരം)((((unsigned long)(val)) >>  8) & 0xff); \
    (a)[3] = (unsigned അക്ഷരം)(((unsigned long)(val)) & 0xff);         \
  } ആകുമ്പോൾ(0)

#define WPA_PUT_BE64(a, val)                            \
  ചെയ്യുക {                                                  \
    (a)[0] = (unsigned അക്ഷരം)(((uint64_t)(val)) >> 56);  \
    (a)[1] = (unsigned അക്ഷരം)(((uint64_t)(val)) >> 48);  \
    (a)[2] = (unsigned അക്ഷരം)(((uint64_t)(val)) >> 40);  \
    (a)[3] = (unsigned അക്ഷരം)(((uint64_t)(val)) >> 32);  \
    (a)[4] = (unsigned അക്ഷരം)(((uint64_t)(val)) >> 24);  \
    (a)[5] = (unsigned അക്ഷരം)(((uint64_t)(val)) >> 16);  \
    (a)[6] = (unsigned അക്ഷരം)(((uint64_t)(val)) >>  8);  \
    (a)[7] = (unsigned അക്ഷരം)(((uint64_t)(val)) & 0xff); \
  } ആകുമ്പോൾ(0)

ഘടന sha256_state {
  uint64_t length;
  unsigned long state[8], curlen;
  unsigned അക്ഷരം buf[64];
};
തരനിർവചനം ഘടന sha256_state my_sha256_ctx;

/* The K array */
സ്ഥിതം സ്ഥിരം unsigned long K[64] = {
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
സ്ഥിതം പൂർണ്ണസംഖ്യ sha256_compress(ഘടന sha256_state *md, സ്ഥിരം unsigned അക്ഷരം *buf)
{
  unsigned long S[8], W[64];
  പൂർണ്ണസംഖ്യ i;

  /* Copy state into S */
  വേണ്ടി(i = 0; i < 8; i++) {
    S[i] = md->state[i];
  }
  /* copy the state into 512 bits into W[0..15] */
  വേണ്ടി(i = 0; i < 16; i++)
    W[i] = WPA_GET_BE32(buf + (4 * i));
  /* fill W[16..63] */
  വേണ്ടി(i = 16; i < 64; i++) {
    W[i] = Gamma1(W[i - 2]) + W[i - 7] + Gamma0(W[i - 15]) + W[i - 16];
  }

  /* Compress */
#define RND(a, b, c, d, e, f, g, h, i)                                   \
  ചെയ്യുക {                                                                   \
    unsigned long t0 = h + Sigma1(e) + Sha256_Ch(e, f, g) + K[i] + W[i]; \
    unsigned long t1 = Sigma0(a) + Sha256_Maj(a, b, c);                  \
    d += t0;                                                             \
    h = t0 + t1;                                                         \
  } ആകുമ്പോൾ(0)

  വേണ്ടി(i = 0; i < 64; ++i) {
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
  വേണ്ടി(i = 0; i < 8; i++) {
    md->state[i] = md->state[i] + S[i];
  }

  തിരികെ 0;
}

/* Initialize the hash state */
സ്ഥിതം CURLcode my_sha256_init(ശൂന്യം *in)
{
  ഘടന sha256_state *md = (ഘടന sha256_state *)in;
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

  തിരികെ CURLE_OK;
}

/* Process a block of memory though the hash
   @param md     The hash state
   @param in     The data to hash
   @param inlen  The length of the data (octets)
 */
സ്ഥിതം ശൂന്യം my_sha256_update(ശൂന്യം *ctx,
                             സ്ഥിരം unsigned അക്ഷരം *in,
                             unsigned പൂർണ്ണസംഖ്യ len)
{
  unsigned long inlen = len;
  unsigned long n;
  ഘടന sha256_state *md = (ഘടന sha256_state *)ctx;
#define CURL_SHA256_BLOCK_SIZE 64
  എങ്കിൽ(md->curlen > വലുപ്പം(md->buf))
    തിരികെ;
  ആകുമ്പോൾ(inlen > 0) {
    എങ്കിൽ(md->curlen == 0 && inlen >= CURL_SHA256_BLOCK_SIZE) {
      എങ്കിൽ(sha256_compress(md, in) < 0)
        തിരികെ;
      md->length += CURL_SHA256_BLOCK_SIZE * 8;
      in += CURL_SHA256_BLOCK_SIZE;
      inlen -= CURL_SHA256_BLOCK_SIZE;
    }
    അല്ലെങ്കിൽ {
      n = CURLMIN(inlen, (CURL_SHA256_BLOCK_SIZE - md->curlen));
      memcpy(md->buf + md->curlen, in, n);
      md->curlen += n;
      in += n;
      inlen -= n;
      എങ്കിൽ(md->curlen == CURL_SHA256_BLOCK_SIZE) {
        എങ്കിൽ(sha256_compress(md, md->buf) < 0)
          തിരികെ;
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
സ്ഥിതം ശൂന്യം my_sha256_final(unsigned അക്ഷരം *out, ശൂന്യം *ctx)
{
  ഘടന sha256_state *md = ctx;
  പൂർണ്ണസംഖ്യ i;

  എങ്കിൽ(md->curlen >= വലുപ്പം(md->buf))
    തിരികെ;

  /* Increase the length of the message */
  md->length += md->curlen * 8;

  /* Append the '1' bit */
  md->buf[md->curlen++] = (unsigned അക്ഷരം)0x80;

  /* If the length is currently above 56 bytes we append zeros
   * then compress. Then we can fall back to padding zeros and length
   * encoding like normal.
   */
  എങ്കിൽ(md->curlen > 56) {
    ആകുമ്പോൾ(md->curlen < 64) {
      md->buf[md->curlen++] = 0;
    }
    sha256_compress(md, md->buf);
    md->curlen = 0;
  }

  /* Pad up to 56 bytes of zeroes */
  ആകുമ്പോൾ(md->curlen < 56) {
    md->buf[md->curlen++] = 0;
  }

  /* Store length */
  WPA_PUT_BE64(md->buf + 56, md->length);
  sha256_compress(md, md->buf);

  /* Copy output */
  വേണ്ടി(i = 0; i < 8; i++)
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
CURLcode Curl_sha256it(unsigned അക്ഷരം *output, സ്ഥിരം unsigned അക്ഷരം *നിവേശം,
                       size_t len)
{
  CURLcode result;
  my_sha256_ctx ctx;

  result = my_sha256_init(&ctx);
  എങ്കിൽ(!result) {
    ചെയ്യുക {
      unsigned പൂർണ്ണസംഖ്യ ilen = (unsigned പൂർണ്ണസംഖ്യ)CURLMIN(len, UINT_MAX);
      my_sha256_update(&ctx, നിവേശം, ilen);
      len -= ilen;
      നിവേശം += ilen;
    } ആകുമ്പോൾ(len);
    my_sha256_final(output, &ctx);
  }
  തിരികെ result;
}

സ്ഥിരം ഘടന HMAC_params Curl_HMAC_SHA256 = {
  my_sha256_init,        /* Hash initialization function. */
  my_sha256_update,      /* Hash update function. */
  my_sha256_final,       /* Hash computation end function. */
  വലുപ്പം(my_sha256_ctx), /* Size of hash context structure. */
  64,                    /* Maximum key length. */
  32                     /* Result size. */
};

#endif /* AWS, DIGEST, or libssh2 */
