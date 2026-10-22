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
#अन्तर्भाव "curl_setup.h"

#यदि !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#अन्तर्भाव "curl_sha256.h"

#ifdef USE_MBEDTLS
#अन्तर्भाव <mbedtls/version.h>
#यदि MBEDTLS_VERSION_संख्या < 0x03020000
#error "mbedTLS 3.2.0 or greater required"
#endif
#अन्तर्भाव <psa/crypto_config.h>
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
#अन्तर्भाव <openssl/evp.h>

संरचना ossl_sha256_ctx {
  EVP_MD_CTX *openssl_ctx;
};
प्रकारनिर्देश संरचना ossl_sha256_ctx my_sha256_ctx;

स्थिर CURLcode my_sha256_init(शून्य *in)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  ctx->openssl_ctx = EVP_MD_CTX_new();
  यदि(!ctx->openssl_ctx)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  यदि(!EVP_DigestInit_ex(ctx->openssl_ctx, EVP_sha256(), रिक्त)) {
    EVP_MD_CTX_free(ctx->openssl_ctx);
    प्रत्यागम CURLE_FAILED_INIT;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य my_sha256_update(शून्य *in,
                             ध्रुव अचिन्हित वर्ण *data,
                             अचिन्हित पूर्णाङ्क length)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  EVP_DigestUpdate(ctx->openssl_ctx, data, length);
}

स्थिर शून्य my_sha256_final(अचिन्हित वर्ण *digest, शून्य *in)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  EVP_DigestFinal_ex(ctx->openssl_ctx, digest, रिक्त);
  EVP_MD_CTX_free(ctx->openssl_ctx);
}

#नोचेद्यदि defined(USE_WOLFSSL)
#अन्तर्भाव <wolfssl/options.h>
#अन्तर्भाव <wolfssl/wolfcrypt/sha256.h>

प्रकारनिर्देश संरचना wc_Sha256 my_sha256_ctx;

स्थिर CURLcode my_sha256_init(शून्य *in)
{
  यदि(wc_InitSha256(in))
    प्रत्यागम CURLE_FAILED_INIT;
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य my_sha256_update(शून्य *in,
                             ध्रुव अचिन्हित वर्ण *data,
                             अचिन्हित पूर्णाङ्क length)
{
  (शून्य)wc_Sha256Update(in, data, (word32)length);
}

स्थिर शून्य my_sha256_final(अचिन्हित वर्ण *digest, शून्य *in)
{
  (शून्य)wc_Sha256Final(in, digest);
}

#नोचेद्यदि defined(USE_GNUTLS)
#अन्तर्भाव <nettle/sha2.h>
#अन्तर्भाव <nettle/version.h>

प्रकारनिर्देश संरचना sha256_ctx my_sha256_ctx;

स्थिर CURLcode my_sha256_init(शून्य *ctx)
{
  sha256_init(ctx);
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य my_sha256_update(शून्य *ctx,
                             ध्रुव अचिन्हित वर्ण *data,
                             अचिन्हित पूर्णाङ्क length)
{
  sha256_update(ctx, length, data);
}

स्थिर शून्य my_sha256_final(अचिन्हित वर्ण *digest, शून्य *ctx)
{
#यदि NETTLE_VERSION_MAJOR >= 4
  sha256_digest(ctx, digest);
#अन्यथा
  sha256_digest(ctx, SHA256_DIGEST_SIZE, digest);
#endif
}

#नोचेद्यदि defined(USE_MBEDTLS) && \
  defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
#अन्तर्भाव <psa/crypto.h>

प्रकारनिर्देश psa_hash_operation_t my_sha256_ctx;

स्थिर CURLcode my_sha256_init(शून्य *ctx)
{
  psa_hash_operation_t *pctx = (psa_hash_operation_t *)ctx;
  *pctx = psa_hash_operation_init();
  यदि(psa_hash_setup(pctx, PSA_ALG_SHA_256) != PSA_SUCCESS)
    प्रत्यागम CURLE_OUT_OF_MEMORY;
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य my_sha256_update(शून्य *ctx,
                             ध्रुव अचिन्हित वर्ण *data,
                             अचिन्हित पूर्णाङ्क length)
{
  (शून्य)psa_hash_update(ctx, data, length);
}

स्थिर शून्य my_sha256_final(अचिन्हित वर्ण *digest, शून्य *ctx)
{
  माप_प्रकार actual_length;
  (शून्य)psa_hash_finish(ctx, digest, CURL_SHA256_DIGEST_LENGTH,
                        &actual_length);
}

#नोचेद्यदि (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
               __MAC_OS_X_VERSION_MAX_ALLOWED >= 1040) || \
      (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
               __IPHONE_OS_VERSION_MAX_ALLOWED >= 20000)
#अन्तर्भाव <CommonCrypto/CommonDigest.h>

प्रकारनिर्देश CC_SHA256_CTX my_sha256_ctx;

स्थिर CURLcode my_sha256_init(शून्य *ctx)
{
  (शून्य)CC_SHA256_Init(ctx);  /* always returns 1 */
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य my_sha256_update(शून्य *ctx,
                             ध्रुव अचिन्हित वर्ण *data,
                             अचिन्हित पूर्णाङ्क length)
{
  (शून्य)CC_SHA256_Update(ctx, data, length);
}

स्थिर शून्य my_sha256_final(अचिन्हित वर्ण *digest, शून्य *ctx)
{
  (शून्य)CC_SHA256_Final(digest, ctx);
}

#नोचेद्यदि defined(USE_WIN32_CRYPTO)
#अन्तर्भाव <wincrypt.h>

संरचना sha256_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
प्रकारनिर्देश संरचना sha256_ctx my_sha256_ctx;

स्थिर CURLcode my_sha256_init(शून्य *in)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  यदि(!CryptAcquireContext(&ctx->hCryptProv, रिक्त, रिक्त, PROV_RSA_AES,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  यदि(!CryptCreateHash(ctx->hCryptProv, CALG_SHA_256, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    प्रत्यागम CURLE_FAILED_INIT;
  }

  प्रत्यागम CURLE_OK;
}

स्थिर शून्य my_sha256_update(शून्य *in,
                             ध्रुव अचिन्हित वर्ण *data,
                             अचिन्हित पूर्णाङ्क length)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  CryptHashData(ctx->hHash, (ध्रुव BYTE *)data, length, 0);
}

स्थिर शून्य my_sha256_final(अचिन्हित वर्ण *digest, शून्य *in)
{
  my_sha256_ctx *ctx = (my_sha256_ctx *)in;
  अचिन्हित दीर्घ length = 0;

  CryptGetHashParam(ctx->hHash, HP_HASHVAL, रिक्त, &length, 0);
  यदि(length == CURL_SHA256_DIGEST_LENGTH)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &length, 0);

  यदि(ctx->hHash)
    CryptDestroyHash(ctx->hHash);

  यदि(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#अन्यथा

/* When no other crypto library is available we use this code segment */

/* This is based on the SHA256 implementation in LibTomCrypt that was released
 * into public domain. */

#define WPA_GET_BE32(a)              \
  ((((अचिन्हित दीर्घ)(a)[0]) << 24) | \
   (((अचिन्हित दीर्घ)(a)[1]) << 16) | \
   (((अचिन्हित दीर्घ)(a)[2]) <<  8) | \
    ((अचिन्हित दीर्घ)(a)[3]))
#define WPA_PUT_BE32(a, val)                                         \
  कुरु {                                                               \
    (a)[0] = (अचिन्हित वर्ण)((((अचिन्हित दीर्घ)(val)) >> 24) & 0xff); \
    (a)[1] = (अचिन्हित वर्ण)((((अचिन्हित दीर्घ)(val)) >> 16) & 0xff); \
    (a)[2] = (अचिन्हित वर्ण)((((अचिन्हित दीर्घ)(val)) >>  8) & 0xff); \
    (a)[3] = (अचिन्हित वर्ण)(((अचिन्हित दीर्घ)(val)) & 0xff);         \
  } यावत्(0)

#define WPA_PUT_BE64(a, val)                            \
  कुरु {                                                  \
    (a)[0] = (अचिन्हित वर्ण)(((uint64_t)(val)) >> 56);  \
    (a)[1] = (अचिन्हित वर्ण)(((uint64_t)(val)) >> 48);  \
    (a)[2] = (अचिन्हित वर्ण)(((uint64_t)(val)) >> 40);  \
    (a)[3] = (अचिन्हित वर्ण)(((uint64_t)(val)) >> 32);  \
    (a)[4] = (अचिन्हित वर्ण)(((uint64_t)(val)) >> 24);  \
    (a)[5] = (अचिन्हित वर्ण)(((uint64_t)(val)) >> 16);  \
    (a)[6] = (अचिन्हित वर्ण)(((uint64_t)(val)) >>  8);  \
    (a)[7] = (अचिन्हित वर्ण)(((uint64_t)(val)) & 0xff); \
  } यावत्(0)

संरचना sha256_state {
  uint64_t length;
  अचिन्हित दीर्घ state[8], curlen;
  अचिन्हित वर्ण buf[64];
};
प्रकारनिर्देश संरचना sha256_state my_sha256_ctx;

/* The K array */
स्थिर ध्रुव अचिन्हित दीर्घ K[64] = {
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
  (((((अचिन्हित दीर्घ)(x) & 0xFFFFFFFFUL) >> (अचिन्हित दीर्घ)((y) & 31)) | \
     ((अचिन्हित दीर्घ)(x) << (अचिन्हित दीर्घ)(32 - ((y) & 31)))) & 0xFFFFFFFFUL)

#define Sha256_Ch(x, y, z)  (z ^ (x & (y ^ z)))
#define Sha256_Maj(x, y, z) (((x | y) & z) | (x & y))
#define Sha256_S(x, n)      RORc(x, n)
#define Sha256_R(x, n)      (((x) & 0xFFFFFFFFUL) >> (n))

#define Sigma0(x)         (Sha256_S(x, 2) ^ Sha256_S(x, 13) ^ Sha256_S(x, 22))
#define Sigma1(x)         (Sha256_S(x, 6) ^ Sha256_S(x, 11) ^ Sha256_S(x, 25))
#define Gamma0(x)         (Sha256_S(x, 7) ^ Sha256_S(x, 18) ^ Sha256_R(x, 3))
#define Gamma1(x)         (Sha256_S(x, 17) ^ Sha256_S(x, 19) ^ Sha256_R(x, 10))

/* Compress 512 bits */
स्थिर पूर्णाङ्क sha256_compress(संरचना sha256_state *md, ध्रुव अचिन्हित वर्ण *buf)
{
  अचिन्हित दीर्घ S[8], W[64];
  पूर्णाङ्क i;

  /* Copy state into S */
  प्रति(i = 0; i < 8; i++) {
    S[i] = md->state[i];
  }
  /* copy the state into 512 bits into W[0..15] */
  प्रति(i = 0; i < 16; i++)
    W[i] = WPA_GET_BE32(buf + (4 * i));
  /* fill W[16..63] */
  प्रति(i = 16; i < 64; i++) {
    W[i] = Gamma1(W[i - 2]) + W[i - 7] + Gamma0(W[i - 15]) + W[i - 16];
  }

  /* Compress */
#define RND(a, b, c, d, e, f, g, h, i)                                   \
  कुरु {                                                                   \
    अचिन्हित दीर्घ t0 = h + Sigma1(e) + Sha256_Ch(e, f, g) + K[i] + W[i]; \
    अचिन्हित दीर्घ t1 = Sigma0(a) + Sha256_Maj(a, b, c);                  \
    d += t0;                                                             \
    h = t0 + t1;                                                         \
  } यावत्(0)

  प्रति(i = 0; i < 64; ++i) {
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
  प्रति(i = 0; i < 8; i++) {
    md->state[i] = md->state[i] + S[i];
  }

  प्रत्यागम 0;
}

/* Initialize the hash state */
स्थिर CURLcode my_sha256_init(शून्य *in)
{
  संरचना sha256_state *md = (संरचना sha256_state *)in;
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

  प्रत्यागम CURLE_OK;
}

/* Process a block of memory though the hash
   @param md     The hash state
   @param in     The data to hash
   @param inlen  The length of the data (octets)
 */
स्थिर शून्य my_sha256_update(शून्य *ctx,
                             ध्रुव अचिन्हित वर्ण *in,
                             अचिन्हित पूर्णाङ्क len)
{
  अचिन्हित दीर्घ inlen = len;
  अचिन्हित दीर्घ n;
  संरचना sha256_state *md = (संरचना sha256_state *)ctx;
#define CURL_SHA256_BLOCK_SIZE 64
  यदि(md->curlen > परिमाण(md->buf))
    प्रत्यागम;
  यावत्(inlen > 0) {
    यदि(md->curlen == 0 && inlen >= CURL_SHA256_BLOCK_SIZE) {
      यदि(sha256_compress(md, in) < 0)
        प्रत्यागम;
      md->length += CURL_SHA256_BLOCK_SIZE * 8;
      in += CURL_SHA256_BLOCK_SIZE;
      inlen -= CURL_SHA256_BLOCK_SIZE;
    }
    अन्यथा {
      n = CURLMIN(inlen, (CURL_SHA256_BLOCK_SIZE - md->curlen));
      स_नकल(md->buf + md->curlen, in, n);
      md->curlen += n;
      in += n;
      inlen -= n;
      यदि(md->curlen == CURL_SHA256_BLOCK_SIZE) {
        यदि(sha256_compress(md, md->buf) < 0)
          प्रत्यागम;
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
स्थिर शून्य my_sha256_final(अचिन्हित वर्ण *out, शून्य *ctx)
{
  संरचना sha256_state *md = ctx;
  पूर्णाङ्क i;

  यदि(md->curlen >= परिमाण(md->buf))
    प्रत्यागम;

  /* Increase the length of the message */
  md->length += md->curlen * 8;

  /* Append the '1' bit */
  md->buf[md->curlen++] = (अचिन्हित वर्ण)0x80;

  /* If the length is currently above 56 bytes we append zeros
   * then compress. Then we can fall back to padding zeros and length
   * encoding like normal.
   */
  यदि(md->curlen > 56) {
    यावत्(md->curlen < 64) {
      md->buf[md->curlen++] = 0;
    }
    sha256_compress(md, md->buf);
    md->curlen = 0;
  }

  /* Pad up to 56 bytes of zeroes */
  यावत्(md->curlen < 56) {
    md->buf[md->curlen++] = 0;
  }

  /* Store length */
  WPA_PUT_BE64(md->buf + 56, md->length);
  sha256_compress(md, md->buf);

  /* Copy output */
  प्रति(i = 0; i < 8; i++)
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
CURLcode Curl_sha256it(अचिन्हित वर्ण *output, ध्रुव अचिन्हित वर्ण *निवेश,
                       माप_प्रकार len)
{
  CURLcode result;
  my_sha256_ctx ctx;

  result = my_sha256_init(&ctx);
  यदि(!result) {
    कुरु {
      अचिन्हित पूर्णाङ्क ilen = (अचिन्हित पूर्णाङ्क)CURLMIN(len, अच_पूर्णांक_उच्च);
      my_sha256_update(&ctx, निवेश, ilen);
      len -= ilen;
      निवेश += ilen;
    } यावत्(len);
    my_sha256_final(output, &ctx);
  }
  प्रत्यागम result;
}

ध्रुव संरचना HMAC_params Curl_HMAC_SHA256 = {
  my_sha256_init,        /* Hash initialization function. */
  my_sha256_update,      /* Hash update function. */
  my_sha256_final,       /* Hash computation end function. */
  परिमाण(my_sha256_ctx), /* Size of hash context structure. */
  64,                    /* Maximum key length. */
  32                     /* Result size. */
};

#endif /* AWS, DIGEST, or libssh2 */
