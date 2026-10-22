/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
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

#अगर_घोषित USE_CURL_NTLM_CORE

#समावेश "curl_md4.h"

#अगर_घोषित USE_OPENSSL
#समावेश <openssl/opensslv.h>
#अगर OPENSSL_VERSION_संख्या >= 0x30000000L && !defined(USE_AMISSL)
/* OpenSSL 3.0.0 marks the MD4 functions as deprecated */
#घोषणा OPENSSL_NO_MD4
#अन्यथा
/* Cover also OPENSSL_NO_MD4 configured in OpenSSL */
#समावेश <openssl/opensslconf.h>
#पूर्ण_अगर
#पूर्ण_अगर /* USE_OPENSSL */

#अगर_घोषित USE_WOLFSSL
#समावेश <wolfssl/options.h>
#पूर्ण_अगर

#अगर defined(USE_OPENSSL) && !defined(OPENSSL_NO_MD4)
#समावेश <openssl/evp.h>

प्रकार EVP_MD_CTX *my_md4_ctx;

अटल पूर्णांक my_md4_init(my_md4_ctx *ctx)
{
  *ctx = EVP_MD_CTX_new();
  अगर(!*ctx)
    वापस 0;

  अगर(!EVP_DigestInit_ex(*ctx, EVP_md4(), शून्य)) {
    EVP_MD_CTX_free(*ctx);
    वापस 0;
  }
  वापस 1;
}

अटल व्योम my_md4_update(my_md4_ctx *ctx,
                          स्थिर अचिन्हित अक्षर *निवेश, अचिन्हित पूर्णांक len)
{
  (व्योम)EVP_DigestUpdate(*ctx, निवेश, len);
}

अटल व्योम my_md4_final(अचिन्हित अक्षर *digest, my_md4_ctx *ctx)
{
  (व्योम)EVP_DigestFinal_ex(*ctx, digest, शून्य);
  EVP_MD_CTX_free(*ctx);
}

#या_अगर defined(USE_WOLFSSL) && !defined(NO_MD4)
#समावेश <wolfssl/wolfcrypt/md4.h>
#समावेश <wolfssl/version.h>

#अगर LIBWOLFSSL_VERSION_HEX >= 0x05007006
प्रकार wc_Md4 my_md4_ctx;
#अन्यथा
प्रकार Md4 my_md4_ctx;
#पूर्ण_अगर

अटल पूर्णांक my_md4_init(my_md4_ctx *ctx)
{
  wc_InitMd4(ctx);
  वापस 1;
}

अटल व्योम my_md4_update(my_md4_ctx *ctx,
                          स्थिर व्योम *निवेश, अचिन्हित दीर्घ len)
{
  wc_Md4Update(ctx, निवेश, (word32)len);
}

अटल व्योम my_md4_final(अचिन्हित अक्षर *digest, my_md4_ctx *ctx)
{
  wc_Md4Final(ctx, digest);
}

#या_अगर (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
               __MAC_OS_X_VERSION_MAX_ALLOWED >= 1040 && \
       defined(__MAC_OS_X_VERSION_MIN_REQUIRED) && \
               __MAC_OS_X_VERSION_MIN_REQUIRED < 101500) || \
      (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
               __IPHONE_OS_VERSION_MAX_ALLOWED >= 20000 && \
       defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && \
               __IPHONE_OS_VERSION_MIN_REQUIRED < 130000)
#समावेश <CommonCrypto/CommonDigest.h>

प्रकार CC_MD4_CTX my_md4_ctx;

अटल पूर्णांक my_md4_init(my_md4_ctx *ctx)
{
  वापस CC_MD4_Init(ctx);  /* always returns 1 */
}

अटल व्योम my_md4_update(my_md4_ctx *ctx,
                          स्थिर व्योम *निवेश, अचिन्हित दीर्घ len)
{
  (व्योम)CC_MD4_Update(ctx, निवेश, (CC_LONG)len);
}

अटल व्योम my_md4_final(अचिन्हित अक्षर *digest, my_md4_ctx *ctx)
{
  (व्योम)CC_MD4_Final(digest, ctx);
}

#या_अगर defined(USE_WIN32_CRYPTO)
#समावेश <wincrypt.h>

काष्ठा md4_ctx {
  HCRYPTPROV hCryptProv;
  HCRYPTHASH hHash;
};
प्रकार काष्ठा md4_ctx my_md4_ctx;

अटल पूर्णांक my_md4_init(my_md4_ctx *ctx)
{
  ctx->hCryptProv = 0;
  ctx->hHash = 0;

  अगर(!CryptAcquireContext(&ctx->hCryptProv, शून्य, शून्य, PROV_RSA_FULL,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    वापस 0;

  अगर(!CryptCreateHash(ctx->hCryptProv, CALG_MD4, 0, 0, &ctx->hHash)) {
    CryptReleaseContext(ctx->hCryptProv, 0);
    ctx->hCryptProv = 0;
    वापस 0;
  }

  वापस 1;
}

अटल व्योम my_md4_update(my_md4_ctx *ctx,
                          स्थिर व्योम *निवेश, अचिन्हित दीर्घ len)
{
  Crypt_Hash_तथ्य(ctx->hHash, (स्थिर BYTE *)निवेश, (अचिन्हित पूर्णांक)len, 0);
}

अटल व्योम my_md4_final(अचिन्हित अक्षर *digest, my_md4_ctx *ctx)
{
  अचिन्हित दीर्घ लंबाई = 0;

  CryptGetHashParam(ctx->hHash, HP_HASHVAL, शून्य, &लंबाई, 0);
  अगर(लंबाई == MD4_DIGEST_लंबाई)
    CryptGetHashParam(ctx->hHash, HP_HASHVAL, digest, &लंबाई, 0);

  अगर(ctx->hHash)
    CryptDestroyHash(ctx->hHash);

  अगर(ctx->hCryptProv)
    CryptReleaseContext(ctx->hCryptProv, 0);
}

#या_अगर defined(USE_GNUTLS)
#समावेश <nettle/md4.h>
#समावेश <nettle/version.h>

प्रकार काष्ठा md4_ctx my_md4_ctx;

अटल पूर्णांक my_md4_init(my_md4_ctx *ctx)
{
  md4_init(ctx);
  वापस 1;
}

अटल व्योम my_md4_update(my_md4_ctx *ctx,
                          स्थिर व्योम *निवेश, अचिन्हित दीर्घ len)
{
  md4_update(ctx, len, निवेश);
}

अटल व्योम my_md4_final(अचिन्हित अक्षर *digest, my_md4_ctx *ctx)
{
#अगर NETTLE_VERSION_MAJOR >= 4
  md4_digest(ctx, digest);
#अन्यथा
  md4_digest(ctx, MD4_DIGEST_आकार, digest);
#पूर्ण_अगर
}

#अन्यथा
/* When no other crypto library is available, or the crypto library does not
 * support MD4, we use this code segment this implementation of it
 *
 * This is an OpenSSL-compatible implementation of the RSA Data Security, Inc.
 * MD4 Message-Digest Algorithm (RFC 1320).
 *
 * Homepage:
 * https://openwall.info/wiki/people/solar/software/public-domain-source-code/md4
 *
 * Author:
 * Alexander Peslyak, better known as Solar Designer <solar at openwall.com>
 *
 * This software was written by Alexander Peslyak in 2001. No copyright is
 * claimed, and the software is hereby placed in the public domain. In case
 * this attempt to disclaim copyright and place the software in the public
 * domain is deemed null and void, then the software is Copyright (c) 2001
 * Alexander Peslyak and it is hereby released to the general public under
 * the following terms:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 *
 * There is ABSOLUTELY NO WARRANTY, express or implied.
 *
 * (This is a heavily cut-down "BSD license".)
 */

काष्ठा md4_ctx {
  uint32_t lo, hi;
  uint32_t a, b, c, d;
  अचिन्हित अक्षर बफर[64];
  uint32_t block[16];
};
प्रकार काष्ठा md4_ctx my_md4_ctx;

/*
 * The basic MD4 functions.
 *
 * F and G are optimized compared to their RFC 1320 definitions, with the
 * optimization for F borrowed from Colin Plumb's MD5 implementation.
 */
#घोषणा MD4_F(x, y, z)                  ((z) ^ ((x) & ((y) ^ (z))))
#घोषणा MD4_G(x, y, z)                  (((x) & ((y) | (z))) | ((y) & (z)))
#घोषणा MD4_H(x, y, z)                  ((x) ^ (y) ^ (z))

/*
 * The MD4 transformation for all three rounds.
 */
#घोषणा MD4_STEP(f, a, b, c, d, x, s) \
  (a) += f(b, c, d) + (x); \
  (a) = (((a) << (s)) | (((a) & 0xffffffff) >> (32 - (s))));

/*
 * SET reads 4 input bytes in little-endian byte order and stores them
 * in a properly aligned word in host byte order.
 *
 * The check for little-endian architectures that tolerate unaligned memory
 * accesses is an optimization. Nothing will break if it does not work.
 */
#अगर defined(__i386__) || defined(__x86_64__) || defined(__vax__)
#घोषणा MD4_SET(n) (*(स्थिर uint32_t *)(स्थिर व्योम *)&ptr[(n) * 4])
#घोषणा MD4_GET(n) MD4_SET(n)
#अन्यथा
#घोषणा MD4_SET(n) (ctx->block[n] =      \
   (uint32_t)ptr[(n) * 4]              | \
  ((uint32_t)ptr[((n) * 4) + 1] <<  8) | \
  ((uint32_t)ptr[((n) * 4) + 2] << 16) | \
  ((uint32_t)ptr[((n) * 4) + 3] << 24))
#घोषणा MD4_GET(n) ctx->block[n]
#पूर्ण_अगर

/*
 * This processes one or more 64-byte data blocks, but does NOT update
 * the bit counters. There are no alignment requirements.
 */
अटल स्थिर व्योम *my_md4_body(my_md4_ctx *ctx,
                               स्थिर व्योम *निवेश, अचिन्हित दीर्घ आकार)
{
  स्थिर अचिन्हित अक्षर *ptr;
  uint32_t a, b, c, d;

  ptr = (स्थिर अचिन्हित अक्षर *)निवेश;

  a = ctx->a;
  b = ctx->b;
  c = ctx->c;
  d = ctx->d;

  करो {
    uint32_t saved_a, saved_b, saved_c, saved_d;

    saved_a = a;
    saved_b = b;
    saved_c = c;
    saved_d = d;

    /* Round 1 */
    MD4_STEP(MD4_F, a, b, c, d, MD4_SET(0), 3)
    MD4_STEP(MD4_F, d, a, b, c, MD4_SET(1), 7)
    MD4_STEP(MD4_F, c, d, a, b, MD4_SET(2), 11)
    MD4_STEP(MD4_F, b, c, d, a, MD4_SET(3), 19)
    MD4_STEP(MD4_F, a, b, c, d, MD4_SET(4), 3)
    MD4_STEP(MD4_F, d, a, b, c, MD4_SET(5), 7)
    MD4_STEP(MD4_F, c, d, a, b, MD4_SET(6), 11)
    MD4_STEP(MD4_F, b, c, d, a, MD4_SET(7), 19)
    MD4_STEP(MD4_F, a, b, c, d, MD4_SET(8), 3)
    MD4_STEP(MD4_F, d, a, b, c, MD4_SET(9), 7)
    MD4_STEP(MD4_F, c, d, a, b, MD4_SET(10), 11)
    MD4_STEP(MD4_F, b, c, d, a, MD4_SET(11), 19)
    MD4_STEP(MD4_F, a, b, c, d, MD4_SET(12), 3)
    MD4_STEP(MD4_F, d, a, b, c, MD4_SET(13), 7)
    MD4_STEP(MD4_F, c, d, a, b, MD4_SET(14), 11)
    MD4_STEP(MD4_F, b, c, d, a, MD4_SET(15), 19)

    /* Round 2 */
    MD4_STEP(MD4_G, a, b, c, d, MD4_GET(0) + 0x5a827999, 3)
    MD4_STEP(MD4_G, d, a, b, c, MD4_GET(4) + 0x5a827999, 5)
    MD4_STEP(MD4_G, c, d, a, b, MD4_GET(8) + 0x5a827999, 9)
    MD4_STEP(MD4_G, b, c, d, a, MD4_GET(12) + 0x5a827999, 13)
    MD4_STEP(MD4_G, a, b, c, d, MD4_GET(1) + 0x5a827999, 3)
    MD4_STEP(MD4_G, d, a, b, c, MD4_GET(5) + 0x5a827999, 5)
    MD4_STEP(MD4_G, c, d, a, b, MD4_GET(9) + 0x5a827999, 9)
    MD4_STEP(MD4_G, b, c, d, a, MD4_GET(13) + 0x5a827999, 13)
    MD4_STEP(MD4_G, a, b, c, d, MD4_GET(2) + 0x5a827999, 3)
    MD4_STEP(MD4_G, d, a, b, c, MD4_GET(6) + 0x5a827999, 5)
    MD4_STEP(MD4_G, c, d, a, b, MD4_GET(10) + 0x5a827999, 9)
    MD4_STEP(MD4_G, b, c, d, a, MD4_GET(14) + 0x5a827999, 13)
    MD4_STEP(MD4_G, a, b, c, d, MD4_GET(3) + 0x5a827999, 3)
    MD4_STEP(MD4_G, d, a, b, c, MD4_GET(7) + 0x5a827999, 5)
    MD4_STEP(MD4_G, c, d, a, b, MD4_GET(11) + 0x5a827999, 9)
    MD4_STEP(MD4_G, b, c, d, a, MD4_GET(15) + 0x5a827999, 13)

    /* Round 3 */
    MD4_STEP(MD4_H, a, b, c, d, MD4_GET(0) + 0x6ed9eba1, 3)
    MD4_STEP(MD4_H, d, a, b, c, MD4_GET(8) + 0x6ed9eba1, 9)
    MD4_STEP(MD4_H, c, d, a, b, MD4_GET(4) + 0x6ed9eba1, 11)
    MD4_STEP(MD4_H, b, c, d, a, MD4_GET(12) + 0x6ed9eba1, 15)
    MD4_STEP(MD4_H, a, b, c, d, MD4_GET(2) + 0x6ed9eba1, 3)
    MD4_STEP(MD4_H, d, a, b, c, MD4_GET(10) + 0x6ed9eba1, 9)
    MD4_STEP(MD4_H, c, d, a, b, MD4_GET(6) + 0x6ed9eba1, 11)
    MD4_STEP(MD4_H, b, c, d, a, MD4_GET(14) + 0x6ed9eba1, 15)
    MD4_STEP(MD4_H, a, b, c, d, MD4_GET(1) + 0x6ed9eba1, 3)
    MD4_STEP(MD4_H, d, a, b, c, MD4_GET(9) + 0x6ed9eba1, 9)
    MD4_STEP(MD4_H, c, d, a, b, MD4_GET(5) + 0x6ed9eba1, 11)
    MD4_STEP(MD4_H, b, c, d, a, MD4_GET(13) + 0x6ed9eba1, 15)
    MD4_STEP(MD4_H, a, b, c, d, MD4_GET(3) + 0x6ed9eba1, 3)
    MD4_STEP(MD4_H, d, a, b, c, MD4_GET(11) + 0x6ed9eba1, 9)
    MD4_STEP(MD4_H, c, d, a, b, MD4_GET(7) + 0x6ed9eba1, 11)
    MD4_STEP(MD4_H, b, c, d, a, MD4_GET(15) + 0x6ed9eba1, 15)

    a += saved_a;
    b += saved_b;
    c += saved_c;
    d += saved_d;

    ptr += 64;
  } जबतक(आकार -= 64);

  ctx->a = a;
  ctx->b = b;
  ctx->c = c;
  ctx->d = d;

  वापस ptr;
}

अटल पूर्णांक my_md4_init(my_md4_ctx *ctx)
{
  ctx->a = 0x67452301;
  ctx->b = 0xefcdab89;
  ctx->c = 0x98badcfe;
  ctx->d = 0x10325476;

  ctx->lo = 0;
  ctx->hi = 0;

  वापस 1;
}

अटल व्योम my_md4_update(my_md4_ctx *ctx,
                          स्थिर व्योम *निवेश, अचिन्हित दीर्घ len)
{
  uint32_t saved_lo;
  अचिन्हित दीर्घ used;

  saved_lo = ctx->lo;
  ctx->lo = (saved_lo + len) & 0x1fffffff;
  अगर(ctx->lo < saved_lo)
    ctx->hi++;
  ctx->hi += (uint32_t)len >> 29;

  used = saved_lo & 0x3f;

  अगर(used) {
    अचिन्हित दीर्घ available = 64 - used;

    अगर(len < available) {
      स_नकल(&ctx->बफर[used], निवेश, len);
      वापस;
    }

    स_नकल(&ctx->बफर[used], निवेश, available);
    निवेश = (स्थिर अचिन्हित अक्षर *)निवेश + available;
    len -= available;
    my_md4_body(ctx, ctx->बफर, 64);
  }

  अगर(len >= 64) {
    निवेश = my_md4_body(ctx, निवेश, len & ~(अचिन्हित दीर्घ)0x3f);
    len &= 0x3f;
  }

  स_नकल(ctx->बफर, निवेश, len);
}

अटल व्योम my_md4_final(अचिन्हित अक्षर *digest, my_md4_ctx *ctx)
{
  अचिन्हित दीर्घ used, available;

  used = ctx->lo & 0x3f;

  ctx->बफर[used++] = 0x80;

  available = 64 - used;

  अगर(available < 8) {
    स_रखो(&ctx->बफर[used], 0, available);
    my_md4_body(ctx, ctx->बफर, 64);
    used = 0;
    available = 64;
  }

  स_रखो(&ctx->बफर[used], 0, available - 8);

  ctx->lo <<= 3;
  ctx->बफर[56] = curlx_ultouc((ctx->lo) & 0xff);
  ctx->बफर[57] = curlx_ultouc((ctx->lo >> 8) & 0xff);
  ctx->बफर[58] = curlx_ultouc((ctx->lo >> 16) & 0xff);
  ctx->बफर[59] = curlx_ultouc((ctx->lo >> 24) & 0xff);
  ctx->बफर[60] = curlx_ultouc((ctx->hi) & 0xff);
  ctx->बफर[61] = curlx_ultouc((ctx->hi >> 8) & 0xff);
  ctx->बफर[62] = curlx_ultouc((ctx->hi >> 16) & 0xff);
  ctx->बफर[63] = curlx_ultouc(ctx->hi >> 24);

  my_md4_body(ctx, ctx->बफर, 64);

  digest[0] = curlx_ultouc((ctx->a) & 0xff);
  digest[1] = curlx_ultouc((ctx->a >> 8) & 0xff);
  digest[2] = curlx_ultouc((ctx->a >> 16) & 0xff);
  digest[3] = curlx_ultouc(ctx->a >> 24);
  digest[4] = curlx_ultouc((ctx->b) & 0xff);
  digest[5] = curlx_ultouc((ctx->b >> 8) & 0xff);
  digest[6] = curlx_ultouc((ctx->b >> 16) & 0xff);
  digest[7] = curlx_ultouc(ctx->b >> 24);
  digest[8] = curlx_ultouc((ctx->c) & 0xff);
  digest[9] = curlx_ultouc((ctx->c >> 8) & 0xff);
  digest[10] = curlx_ultouc((ctx->c >> 16) & 0xff);
  digest[11] = curlx_ultouc(ctx->c >> 24);
  digest[12] = curlx_ultouc((ctx->d) & 0xff);
  digest[13] = curlx_ultouc((ctx->d >> 8) & 0xff);
  digest[14] = curlx_ultouc((ctx->d >> 16) & 0xff);
  digest[15] = curlx_ultouc(ctx->d >> 24);

  स_रखो(ctx, 0, माप(*ctx));
}

#पूर्ण_अगर /* CRYPTO LIBS */

CURLcode Curl_md4it(अचिन्हित अक्षर *output,
                    स्थिर अचिन्हित अक्षर *निवेश, स्थिर माप_प्रकार len)
{
  my_md4_ctx ctx;
  अगर(!my_md4_init(&ctx))
    वापस CURLE_FAILED_INIT;
  my_md4_update(&ctx, निवेश, curlx_uztoui(len));
  my_md4_final(output, &ctx);
  वापस CURLE_OK;
}

#पूर्ण_अगर /* USE_CURL_NTLM_CORE */
