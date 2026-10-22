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
#समावेश "curl_setup.h"

#अगर !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#समावेश "curl_sha256.h"

#अगर_घोषित USE_MBEDTLS
#समावेश <mbedtls/version.h>
#अगर MBEDTLS_VERSION_संख्या < 0x03020000
#त्रुटि "mbedTLS 3.2.0 or greater required"
#पूर्ण_अगर
#समावेश <psa/crypto_config.h>
#पूर्ण_अगर

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

#अगर_घोषित USE_OPENSSL
#समावेश <openssl/evp.h>

काष्ठा ossl_sha256_ctx {
  EVP_MD_CTX *openssl_ctx;
};
प्रकार काष्ठा ossl_sha256_ctx my_sha256_ctx;

अटल CURLcode my_sha256_init(व्योम *in)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  ctx->openssl_ctx = EVP_MD_CTX_new();
  अगर(!ctx->openssl_ctx)
    वापस CURLE_OUT_OF_MEMORY;

  अगर(!EVP_DigestInit_ex(ctx->openssl_ctx, EVP_sha256(), शून्य)) {
    EVP_MD_CTX_free(ctx->openssl_ctx);
    वापस CURLE_FAILED_INIT;
  }
  वापस CURLE_OK;
}

अटल व्योम my_sha256_update(व्योम *in,
                             स्थिर अचिन्हित अक्षर *तथ्य,
                             अचिन्हित पूर्णांक लंबाई)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  EVP_DigestUpdate(ctx->openssl_ctx, तथ्य, लंबाई);
}

अटल व्योम my_sha256_final(अचिन्हित अक्षर *digest, व्योम *in)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  EVP_DigestFinal_ex(ctx->openssl_ctx, digest, शून्य);
  EVP_MD_CTX_free(ctx->openssl_ctx);
}

#या_अगर defined(USE_WOLFSSL)
#समावेश <wolfssl/options.h>
#समावेश <wolfssl/wolfcrypt/sha256.h>

प्रकार काष्ठा wc_Sha256 my_sha256_ctx;

अटल CURLcode my_sha256_init(व्योम *in)
{
  अगर(wc_InitSha256(in))
    वापस CURLE_FAILED_INIT;
  वापस CURLE_OK;
}

अटल व्योम my_sha256_update(व्योम *in,
                             स्थिर अचिन्हित अक्षर *तथ्य,
                             अचिन्हित पूर्णांक लंबाई)
{
  (व्योम)wc_Sha256Update(in, तथ्य, (word32)लंबाई);
}

अटल व्योम my_sha256_final(अचिन्हित अक्षर *digest, व्योम *in)
{
  (व्योम)wc_Sha256Final(in, digest);
}

#या_अगर defined(USE_GNUTLS)
#समावेश <nettle/sha2.h>
#समावेश <nettle/version.h>

प्रकार काष्ठा sha256_ctx my_sha256_ctx;

अटल CURLcode my_sha256_init(व्योम *ctx)
{
  sha256_init(ctx);
  वापस CURLE_OK;
}

अटल व्योम my_sha256_update(व्योम *ctx,
                             स्थिर अचिन्हित अक्षर *तथ्य,
                             अचिन्हित पूर्णांक लंबाई)
{
  sha256_update(ctx, लंबाई, तथ्य);
}

अटल व्योम my_sha256_final(अचिन्हित अक्षर *digest, व्योम *ctx)
{
#अगर NETTLE_VERSION_MAJOR >= 4
  sha256_digest(ctx, digest);
#अन्यथा
  sha256_digest(ctx, SHA256_DIGEST_आकार, digest);
#पूर्ण_अगर
}

#या_अगर defined(USE_MBEDTLS) && \
  defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
#समावेश <psa/crypto.h>

प्रकार psa_hash_operation_t my_sha256_ctx;

अटल CURLcode my_sha256_init(व्योम *ctx)
{
  psa_hash_operation_t *pctx = (psa_hash_operation_t *)ctx;
  *pctx = psa_hash_operation_init();
  अगर(psa_hash_setup(pctx, PSA_ALG_SHA_256) != PSA_SUCCESS)
    वापस CURLE_OUT_OF_MEMORY;
  वापस CURLE_OK;
}

अटल व्योम my_sha256_update(व्योम *ctx,
                             स्थिर अचिन्हित अक्षर *तथ्य,
                             अचिन्हित पूर्णांक लंबाई)
{
  (व्योम)psa_hash_update(ctx, तथ्य, लंबाई);
}

अटल व्योम my_sha256_final(अचिन्हित अक्षर *digest, व्योम *ctx)
{
  माप_प्रकार actual_लंबाई;
  (व्योम)psa_hash_finish(ctx, digest, CURL_SHA256_DIGEST_लंबाई,
                        &actual_लंबाई);
}

#या_अगर (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
               __MAC_OS_X_VERSION_MAX_ALLOWED >= 1040) || \
      (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
               __IPHONE_OS_VERSION_MAX_ALLOWED >= 20000)
#समावेश <CommonCrypto/CommonDigest.h>

प्रकार CC_SHA256_CTX my_sha256_ctx;

अटल CURLcode my_sha256_init(व्योम *ctx)
{
  (व्योम)CC_SHA256_Init(ctx);  /* always returns 1 */
  वापस CURLE_OK;
}

अटल व्योम my_sha256_update(व्योम *ctx,
                             स्थिर अचिन्हित अक्षर *तथ्य,
                             अचिन्हित पूर्णांक लंबाई)
{
  (व्योम)CC_SHA256_Update(ctx, तथ्य, लंबाई);
}

अटल व्योम my_sha256_final(अचिन्हित अक्षर *digest, व्योम *ctx)
{
  (व्योम)CC_SHA256_Final(digest, ctx);
}

#या_अगर defined(USE_WIN32_CRYPTO)
#समावेश <wincrypt.h>

काष्ठा sha256_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
प्रकार काष्ठा sha256_ctx my_sha256_ctx;

अटल CURLcode my_sha256_init(व्योम *in)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  अगर(!CryptAcquireContext(&ctx->hCryptProv, शून्य, शून्य, PROV_RSA_AES,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    वापस CURLE_OUT_OF_MEMORY;

  अगर(!CryptCreateHash(ctx->hCryptProv, CALG_SHA_256, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    वापस CURLE_FAILED_INIT;
  }

  वापस CURLE_OK;
}

अटल व्योम my_sha256_update(व्योम *in,
                             स्थिर अचिन्हित अक्षर *तथ्य,
                             अचिन्हित पूर्णांक लंबाई)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  Crypt_Hash_तथ्य(ctx->hHash, (स्थिर BYTE *)तथ्य, लंबाई, 0);
}

अटल व्योम my_sha256_final(अचिन्हित अक्षर *digest, व्योम *in)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  अचिन्हित दीर्घ लंबाई = 0;

  CryptGetHashParam(ctx->hHash, HP_HASHVAL, शून्य, &लंबाई, 0);
  अगर(लंबाई == CURL_SHA256_DIGEST_लंबाई)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &लंबाई, 0);

  अगर(ctx->hHash)
    CryptDestroyHash(ctx->hHash);

  अगर(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#अन्यथा

/* When no other crypto library is available we use this code segment */

/* This is based on the SHA256 implementation in LibTomCrypt that was released
 * into public domain. */

#घोषणा WPA_GET_BE32(a)              \
  ((((अचिन्हित दीर्घ)(a)[0]) << 24) | \
   (((अचिन्हित दीर्घ)(a)[1]) << 16) | \
   (((अचिन्हित दीर्घ)(a)[2]) <<  8) | \
    ((अचिन्हित दीर्घ)(a)[3]))
#घोषणा WPA_PUT_BE32(a, val)                                         \
  करो {                                                               \
    (a)[0] = (अचिन्हित अक्षर)((((अचिन्हित दीर्घ)(val)) >> 24) & 0xff); \
    (a)[1] = (अचिन्हित अक्षर)((((अचिन्हित दीर्घ)(val)) >> 16) & 0xff); \
    (a)[2] = (अचिन्हित अक्षर)((((अचिन्हित दीर्घ)(val)) >>  8) & 0xff); \
    (a)[3] = (अचिन्हित अक्षर)(((अचिन्हित दीर्घ)(val)) & 0xff);         \
  } जबतक(0)

#घोषणा WPA_PUT_BE64(a, val)                            \
  करो {                                                  \
    (a)[0] = (अचिन्हित अक्षर)(((uint64_t)(val)) >> 56);  \
    (a)[1] = (अचिन्हित अक्षर)(((uint64_t)(val)) >> 48);  \
    (a)[2] = (अचिन्हित अक्षर)(((uint64_t)(val)) >> 40);  \
    (a)[3] = (अचिन्हित अक्षर)(((uint64_t)(val)) >> 32);  \
    (a)[4] = (अचिन्हित अक्षर)(((uint64_t)(val)) >> 24);  \
    (a)[5] = (अचिन्हित अक्षर)(((uint64_t)(val)) >> 16);  \
    (a)[6] = (अचिन्हित अक्षर)(((uint64_t)(val)) >>  8);  \
    (a)[7] = (अचिन्हित अक्षर)(((uint64_t)(val)) & 0xff); \
  } जबतक(0)

काष्ठा sha256_state {
  uint64_t लंबाई;
  अचिन्हित दीर्घ state[8], curlen;
  अचिन्हित अक्षर buf[64];
};
प्रकार काष्ठा sha256_state my_sha256_ctx;

/* The K array */
अटल स्थिर अचिन्हित दीर्घ K[64] = {
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
#घोषणा RORc(x, y) \
  (((((अचिन्हित दीर्घ)(x) & 0xFFFFFFFFUL) >> (अचिन्हित दीर्घ)((y) & 31)) | \
     ((अचिन्हित दीर्घ)(x) << (अचिन्हित दीर्घ)(32 - ((y) & 31)))) & 0xFFFFFFFFUL)

#घोषणा Sha256_Ch(x, y, z)  (z ^ (x & (y ^ z)))
#घोषणा Sha256_Maj(x, y, z) (((x | y) & z) | (x & y))
#घोषणा Sha256_S(x, n)      RORc(x, n)
#घोषणा Sha256_R(x, n)      (((x) & 0xFFFFFFFFUL) >> (n))

#घोषणा Sigma0(x)         (Sha256_S(x, 2) ^ Sha256_S(x, 13) ^ Sha256_S(x, 22))
#घोषणा Sigma1(x)         (Sha256_S(x, 6) ^ Sha256_S(x, 11) ^ Sha256_S(x, 25))
#घोषणा Gamma0(x)         (Sha256_S(x, 7) ^ Sha256_S(x, 18) ^ Sha256_R(x, 3))
#घोषणा Gamma1(x)         (Sha256_S(x, 17) ^ Sha256_S(x, 19) ^ Sha256_R(x, 10))

/* Compress 512 bits */
अटल पूर्णांक sha256_compress(काष्ठा sha256_state *md, स्थिर अचिन्हित अक्षर *buf)
{
  अचिन्हित दीर्घ S[8], W[64];
  पूर्णांक i;

  /* Copy state into S */
  क्रम(i = 0; i < 8; i++) {
    S[i] = md->state[i];
  }
  /* copy the state into 512 bits into W[0..15] */
  क्रम(i = 0; i < 16; i++)
    W[i] = WPA_GET_BE32(buf + (4 * i));
  /* fill W[16..63] */
  क्रम(i = 16; i < 64; i++) {
    W[i] = Gamma1(W[i - 2]) + W[i - 7] + Gamma0(W[i - 15]) + W[i - 16];
  }

  /* Compress */
#घोषणा RND(a, b, c, d, e, f, g, h, i)                                   \
  करो {                                                                   \
    अचिन्हित दीर्घ t0 = h + Sigma1(e) + Sha256_Ch(e, f, g) + K[i] + W[i]; \
    अचिन्हित दीर्घ t1 = Sigma0(a) + Sha256_Maj(a, b, c);                  \
    d += t0;                                                             \
    h = t0 + t1;                                                         \
  } जबतक(0)

  क्रम(i = 0; i < 64; ++i) {
    अचिन्हित दीर्घ t;
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
  क्रम(i = 0; i < 8; i++) {
    md->state[i] = md->state[i] + S[i];
  }

  वापस 0;
}

/* Initialize the hash state */
अटल CURLcode my_sha256_init(व्योम *in)
{
  काष्ठा sha256_state *md = (काष्ठा sha256_state *)in;
  md->curlen = 0;
  md->लंबाई = 0;
  md->state[0] = 0x6A09E667UL;
  md->state[1] = 0xBB67AE85UL;
  md->state[2] = 0x3C6EF372UL;
  md->state[3] = 0xA54FF53AUL;
  md->state[4] = 0x510E527FUL;
  md->state[5] = 0x9B05688CUL;
  md->state[6] = 0x1F83D9ABUL;
  md->state[7] = 0x5BE0CD19UL;

  वापस CURLE_OK;
}

/* Process a block of memory though the hash
   @param md     The hash state
   @param in     The data to hash
   @param inlen  The length of the data (octets)
 */
अटल व्योम my_sha256_update(व्योम *ctx,
                             स्थिर अचिन्हित अक्षर *in,
                             अचिन्हित पूर्णांक len)
{
  अचिन्हित दीर्घ inlen = len;
  अचिन्हित दीर्घ n;
  काष्ठा sha256_state *md = (काष्ठा sha256_state *)ctx;
#घोषणा CURL_SHA256_BLOCK_आकार 64
  अगर(md->curlen > माप(md->buf))
    वापस;
  जबतक(inlen > 0) {
    अगर(md->curlen == 0 && inlen >= CURL_SHA256_BLOCK_आकार) {
      अगर(sha256_compress(md, in) < 0)
        वापस;
      md->लंबाई += CURL_SHA256_BLOCK_आकार * 8;
      in += CURL_SHA256_BLOCK_आकार;
      inlen -= CURL_SHA256_BLOCK_आकार;
    }
    अन्यथा {
      n = CURLMIN(inlen, (CURL_SHA256_BLOCK_आकार - md->curlen));
      स_नकल(md->buf + md->curlen, in, n);
      md->curlen += n;
      in += n;
      inlen -= n;
      अगर(md->curlen == CURL_SHA256_BLOCK_आकार) {
        अगर(sha256_compress(md, md->buf) < 0)
          वापस;
        md->लंबाई += 8 * CURL_SHA256_BLOCK_आकार;
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
अटल व्योम my_sha256_final(अचिन्हित अक्षर *out, व्योम *ctx)
{
  काष्ठा sha256_state *md = ctx;
  पूर्णांक i;

  अगर(md->curlen >= माप(md->buf))
    वापस;

  /* Increase the length of the message */
  md->लंबाई += md->curlen * 8;

  /* Append the '1' bit */
  md->buf[md->curlen++] = (अचिन्हित अक्षर)0x80;

  /* If the length is currently above 56 bytes we append zeros
   * then compress. Then we can fall back to padding zeros and length
   * encoding like normal.
   */
  अगर(md->curlen > 56) {
    जबतक(md->curlen < 64) {
      md->buf[md->curlen++] = 0;
    }
    sha256_compress(md, md->buf);
    md->curlen = 0;
  }

  /* Pad up to 56 bytes of zeroes */
  जबतक(md->curlen < 56) {
    md->buf[md->curlen++] = 0;
  }

  /* Store length */
  WPA_PUT_BE64(md->buf + 56, md->लंबाई);
  sha256_compress(md, md->buf);

  /* Copy output */
  क्रम(i = 0; i < 8; i++)
    WPA_PUT_BE32(out + (4 * i), md->state[i]);
}

#पूर्ण_अगर /* CRYPTO LIBS */

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
CURLcode Curl_sha256it(अचिन्हित अक्षर *output, स्थिर अचिन्हित अक्षर *निवेश,
                       माप_प्रकार len)
{
  CURLcode परिणाम;
  my_sha256_ctx ctx;

  परिणाम = my_sha256_init(&ctx);
  अगर(!परिणाम) {
    करो {
      अचिन्हित पूर्णांक ilen = (अचिन्हित पूर्णांक)CURLMIN(len, अच_पूर्णांक_उच्च);
      my_sha256_update(&ctx, निवेश, ilen);
      len -= ilen;
      निवेश += ilen;
    } जबतक(len);
    my_sha256_final(output, &ctx);
  }
  वापस परिणाम;
}

स्थिर काष्ठा HMAC_params Curl_HMAC_SHA256 = {
  my_sha256_init,        /* Hash initialization function. */
  my_sha256_update,      /* Hash update function. */
  my_sha256_final,       /* Hash computation end function. */
  माप(my_sha256_ctx), /* Size of hash context structure. */
  64,                    /* Maximum key length. */
  32                     /* Result size. */
};

#पूर्ण_अगर /* AWS, DIGEST, or libssh2 */
