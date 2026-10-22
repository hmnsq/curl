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
#ضمن "curl_setup.h"

#جيڪڏهن_وصف_ٿيل USE_CURL_NTLM_CORE

/*
 * NTLM details:
 *
 * https://davenport.sourceforge.net/ntlm.html
 * https://www.innovation.ch/java/ntlm.html
 */

/* Please keep the SSL backend-specific #if branches in this order:

   1. USE_OPENSSL
   2. USE_WOLFSSL
   3. USE_GNUTLS
   4. USE_MBEDTLS
   5. USE_OS400CRYPTO
   6. USE_WIN32_CRYPTO

   This ensures that:
   - the same SSL branch gets activated throughout this source
     file even if multiple backends are enabled at the same time.
   - OpenSSL has higher priority than Windows Crypt, due
     to issues with the latter supporting NTLM2Session responses
     in NTLM type-3 messages.
 */

#جيڪڏهن_وصف_ٿيل USE_GNUTLS
#ضمن <nettle/version.h>
#إذا NETTLE_نسخو_MAJOR < 4
#وصف_ڪريو USE_GNUTLS_DES
#جيڪڏهن_پورو
#جيڪڏهن_پورو

#إذا defined(USE_OPENSSL) && defined(HAVE_DES_ECB_ENCRYPT)

#  include <openssl/des.h>
#  ifdef OPENSSL_IS_AWSLC  /* for versions 1.2.0 to 1.30.1 */
#    define DES_سيٽ_ڪنجي_unchecked (فراغ)DES_سيٽ_ڪنجي
#  endif
#  define USE_OPENSSL_DES

#وإلاإذا defined(USE_WOLFSSL) && defined(HAVE_WC_DES_ECBENCRYPT)

#  include <wolfssl/options.h>
#  include <wolfssl/wolfcrypt/des3.h>
#  define USE_WOLFSSL_DES

#وإلاإذا defined(USE_GNUTLS_DES)
#  include <nettle/des.h>
#  define USE_CURL_DES_سيٽ_ODD_PARITY
#وإلاإذا defined(USE_MBEDTLS) && defined(HAVE_MBEDTLS_DES_CRYPT_ECB)
#  include <mbedtls/version.h>
#  إذا MBEDTLS_نسخو_عدد < 0x03020000
#  error "mbedTLS 3.2.0 or greater required"
#  endif
#  include <mbedtls/des.h>
#  define USE_MBEDTLS_DES
#وإلاإذا defined(USE_OS400CRYPTO)
#  include "cipher.mih"  /* mih/cipher */
#  define USE_CURL_DES_سيٽ_ODD_PARITY
#وإلاإذا defined(USE_WIN32_CRYPTO)
#  include <wincrypt.h>
#  define USE_CURL_DES_سيٽ_ODD_PARITY
#وإلا
#  error "cannot compile NTLM support without a crypto library with DES."
#جيڪڏهن_پورو

#ضمن "urldata.h"
#ضمن "strcase.h"
#ضمن "curl_ntlm_core.h"
#ضمن "curl_md5.h"
#ضمن "curl_hmac.h"
#ضمن "curl_md4.h"
#ضمن "vauth/vauth.h"

#جيڪڏهن_وصف_ٿيل USE_CURL_DES_سيٽ_ODD_PARITY
/*
 * curl_des_set_odd_parity()
 *
 * Copyright (C) Steve Holme, <steve_holme@hotmail.com>
 *
 * This is used to apply odd parity to the given byte array. It is typically
 * used by when a cryptography engine does not have its own version.
 *
 * The function is a port of the Java based oddParity() function over at:
 *
 * https://davenport.sourceforge.net/ntlm.html
 *
 * Parameters:
 *
 * bytes       [in/out] - The data whose parity bits are to be adjusted for
 *                        odd parity.
 * len         [in]     - The length of the data.
 */
ساكن فراغ curl_des_سيٽ_odd_parity(بنانشان حرف *بائيٽون, size_t len)
{
  size_t i;

  لكل(i = 0; i < len; i++) {
    بنانشان حرف b = بائيٽون[i];

    bool needs_parity = (((b >> 7) ^ (b >> 6) ^ (b >> 5) ^
                          (b >> 4) ^ (b >> 3) ^ (b >> 2) ^
                          (b >> 1)) & 0x01) == 0;

    إذا(needs_parity)
      بائيٽون[i] |= 0x01;
    وإلا
      بائيٽون[i] &= 0xfe;
  }
}
#جيڪڏهن_پورو /* USE_CURL_DES_SET_ODD_PARITY */

/*
 * Turns a 56-bit key into being 64-bit wide.
 */
ساكن فراغ extend_ڪنجي_56_منزل_64(ثابت بنانشان حرف *ڪنجي_56, حرف *ڪنجي)
{
  ڪنجي[0] = (حرف)ڪنجي_56[0];
  ڪنجي[1] = (حرف)(((ڪنجي_56[0] << 7) & 0xFF) | (ڪنجي_56[1] >> 1));
  ڪنجي[2] = (حرف)(((ڪنجي_56[1] << 6) & 0xFF) | (ڪنجي_56[2] >> 2));
  ڪنجي[3] = (حرف)(((ڪنجي_56[2] << 5) & 0xFF) | (ڪنجي_56[3] >> 3));
  ڪنجي[4] = (حرف)(((ڪنجي_56[3] << 4) & 0xFF) | (ڪنجي_56[4] >> 4));
  ڪنجي[5] = (حرف)(((ڪنجي_56[4] << 3) & 0xFF) | (ڪنجي_56[5] >> 5));
  ڪنجي[6] = (حرف)(((ڪنجي_56[5] << 2) & 0xFF) | (ڪنجي_56[6] >> 6));
  ڪنجي[7] = (حرف)((ڪنجي_56[6] << 1) & 0xFF);
}

#جيڪڏهن_وصف_ٿيل USE_OPENSSL_DES
/*
 * Turns a 56-bit key into a 64-bit, odd parity key and sets the key. The
 * key schedule ks is also set.
 */
ساكن فراغ تياري_des_ڪنجي(ثابت بنانشان حرف *ڪنجي_56, DES_ڪنجي_وقت_جدول *ks)
{
  DES_cblock ڪنجي;

  /* Expand the 56-bit key to 64 bits */
  extend_ڪنجي_56_منزل_64(ڪنجي_56, (حرف *)&ڪنجي);

  /* Set the key parity to odd */
  DES_سيٽ_odd_parity(&ڪنجي);

  /* Set the key */
  DES_سيٽ_ڪنجي_unchecked(&ڪنجي, ks);
}

#وإلاإذا defined(USE_WOLFSSL_DES)
ساكن فراغ تياري_des_ڪنجي(ثابت بنانشان حرف *ڪنجي_56, Des *des)
{
  byte ڪنجي[8];

  /* Expand the 56-bit key to 64 bits */
  extend_ڪنجي_56_منزل_64(ڪنجي_56, (حرف *)ڪنجي);

  /* Set the key */
  wc_Des_سيٽ_ڪنجي(des, ڪنجي, عدم, 0);
}

#وإلاإذا defined(USE_GNUTLS_DES)
ساكن فراغ تياري_des_ڪنجي(ثابت بنانشان حرف *ڪنجي_56, بنية des_ctx *des)
{
  حرف ڪنجي[8];

  /* Expand the 56-bit key to 64 bits */
  extend_ڪنجي_56_منزل_64(ڪنجي_56, ڪنجي);

  /* Set the key parity to odd */
  curl_des_سيٽ_odd_parity((بنانشان حرف *)ڪنجي, حجم(ڪنجي));

  /* Set the key */
  des_سيٽ_ڪنجي(des, (ثابت uint8_t *)ڪنجي);
}

#وإلاإذا defined(USE_MBEDTLS_DES)
ساكن bool encrypt_des(ثابت بنانشان حرف *اندريون, بنانشان حرف *ٻاهريون,
                        ثابت بنانشان حرف *ڪنجي_56)
{
  mbedtls_des_context ctx;
  حرف ڪنجي[8];

  /* Expand the 56-bit key to 64 bits */
  extend_ڪنجي_56_منزل_64(ڪنجي_56, ڪنجي);

  /* Set the key parity to odd */
  mbedtls_des_ڪنجي_سيٽ_parity((بنانشان حرف *)ڪنجي);

  /* Perform the encryption */
  mbedtls_des_init(&ctx);
  mbedtls_des_setkey_enc(&ctx, (بنانشان حرف *)ڪنجي);
  أرجع mbedtls_des_crypt_ecb(&ctx, اندريون, ٻاهريون) == 0;
}

#وإلاإذا defined(USE_OS400CRYPTO)
ساكن bool encrypt_des(ثابت بنانشان حرف *اندريون, بنانشان حرف *ٻاهريون,
                        ثابت بنانشان حرف *ڪنجي_56)
{
  حرف ڪنجي[8];
  _CIPHER_Control_T ctl;

  /* Setup the cipher control structure */
  ctl.Func_ID = ENCRYPT_ONLY;
  ctl.معلومات_Len = حجم(ڪنجي);

  /* Expand the 56-bit key to 64 bits */
  extend_ڪنجي_56_منزل_64(ڪنجي_56, ctl.Crypto_ڪنجي);

  /* Set the key parity to odd */
  curl_des_سيٽ_odd_parity((بنانشان حرف *)ctl.Crypto_ڪنجي, ctl.معلومات_Len);

  /* Perform the encryption */
  _CIPHER((_SPCPTR *)&ٻاهريون, &ctl, (_SPCPTR *)&اندريون);

  أرجع TRUE;
}

#وإلاإذا defined(USE_WIN32_CRYPTO)
ساكن bool encrypt_des(ثابت بنانشان حرف *اندريون, بنانشان حرف *ٻاهريون,
                        ثابت بنانشان حرف *ڪنجي_56)
{
  HCRYPTPROV hprov;
  HCRYPTKEY hkey;
  بنية {
    BLOBHEADER hdr;
    بنانشان صحيح len;
    حرف ڪنجي[8];
  } blob;
  DWORD len = 8;
  BOOL res;

  /* Acquire the crypto provider */
  إذا(!CryptAcquireContext(&hprov, عدم, عدم, PROV_RSA_ڀريل,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    أرجع FALSE;

  /* Setup the key blob structure */
  memset(&blob, 0, حجم(blob));
  blob.hdr.bType = PLAINTEXTKEYBLOB;
  blob.hdr.b_نسخو = 2;
  blob.hdr.ai_ڪنجي_Alg = CALG_DES;
  blob.len = حجم(blob.ڪنجي);

  /* Expand the 56-bit key to 64 bits */
  extend_ڪنجي_56_منزل_64(ڪنجي_56, blob.ڪنجي);

  /* Set the key parity to odd */
  curl_des_سيٽ_odd_parity((بنانشان حرف *)blob.ڪنجي, حجم(blob.ڪنجي));

  /* Import the key */
  إذا(!Crypt_Import_ڪنجي(hprov, (BYTE *)&blob, حجم(blob), 0, 0, &hkey)) {
    CryptReleaseContext(hprov, 0);

    أرجع FALSE;
  }

  يادگيري_نقل(ٻاهريون, اندريون, 8);

  /* Perform the encryption */
  res = CryptEncrypt(hkey, 0, FALSE, 0, ٻاهريون, &len, len);

  Crypt_Destroy_ڪنجي(hkey);
  CryptReleaseContext(hprov, 0);

  أرجع res;
}

#جيڪڏهن_پورو /* crypto backends */

/*
 * takes a 21-byte array and treats it as 3 56-bit DES keys. The
 * 8-byte plaintext is encrypted with each key and the resulting 24
 * bytes are stored in the results array.
 */
فراغ Curl_ntlm_core_lm_resp(ثابت بنانشان حرف *ڪنجيون,
                            ثابت بنانشان حرف *plaintext,
                            بنانشان حرف *results)
{
#جيڪڏهن_وصف_ٿيل USE_OPENSSL_DES
  DES_ڪنجي_وقت_جدول ks;

  تياري_des_ڪنجي(ڪنجيون, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)results, &ks, DES_ENCRYPT);

  تياري_des_ڪنجي(ڪنجيون + 7, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 8), &ks, DES_ENCRYPT);

  تياري_des_ڪنجي(ڪنجيون + 14, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 16), &ks, DES_ENCRYPT);
#وإلاإذا defined(USE_WOLFSSL_DES)
  Des des;
  تياري_des_ڪنجي(ڪنجيون, &des);
  wc_Des_EcbEncrypt(&des, results, plaintext, DES_ڪنجي_سائيز);
  تياري_des_ڪنجي(ڪنجيون + 7, &des);
  wc_Des_EcbEncrypt(&des, results + 8, plaintext, DES_ڪنجي_سائيز);
  تياري_des_ڪنجي(ڪنجيون + 14, &des);
  wc_Des_EcbEncrypt(&des, results + 16, plaintext, DES_ڪنجي_سائيز);
#وإلاإذا defined(USE_GNUTLS_DES)
  بنية des_ctx des;
  تياري_des_ڪنجي(ڪنجيون, &des);
  des_encrypt(&des, 8, results, plaintext);
  تياري_des_ڪنجي(ڪنجيون + 7, &des);
  des_encrypt(&des, 8, results + 8, plaintext);
  تياري_des_ڪنجي(ڪنجيون + 14, &des);
  des_encrypt(&des, 8, results + 16, plaintext);
#وإلاإذا defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
  encrypt_des(plaintext, results, ڪنجيون);
  encrypt_des(plaintext, results + 8, ڪنجيون + 7);
  encrypt_des(plaintext, results + 16, ڪنجيون + 14);
#وإلا
  (فراغ)ڪنجيون;
  (فراغ)plaintext;
  (فراغ)results;
#جيڪڏهن_پورو
}

/*
 * Set up lanmanager hashed password
 */
CURLcode Curl_ntlm_core_mk_lm_hash(ثابت حرف *password,
                                   بنانشان حرف *lmbuffer /* 21 bytes */)
{
  بنانشان حرف pw[14];
  ساكن ثابت بنانشان حرف magic[] = {
    0x4B, 0x47, 0x53, 0x21, 0x40, 0x23, 0x24, 0x25 /* i.e. KGS!@#$% */
  };
  size_t len = CURLMIN(لڙي_ڊيگهه(password), 14);

  Curl_strntoupper((حرف *)pw, password, len);
  memset(&pw[len], 0, 14 - len);

  {
    /* Create LanManager hashed password. */
#جيڪڏهن_وصف_ٿيل USE_OPENSSL_DES
    DES_ڪنجي_وقت_جدول ks;

    تياري_des_ڪنجي(pw, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)lmbuffer, &ks, DES_ENCRYPT);

    تياري_des_ڪنجي(pw + 7, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)(lmbuffer + 8), &ks, DES_ENCRYPT);
#وإلاإذا defined(USE_WOLFSSL_DES)
    Des des;
    تياري_des_ڪنجي(pw, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer, magic, DES_ڪنجي_سائيز);
    تياري_des_ڪنجي(pw + 7, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer + 8, magic, DES_ڪنجي_سائيز);
#وإلاإذا defined(USE_GNUTLS_DES)
    بنية des_ctx des;
    تياري_des_ڪنجي(pw, &des);
    des_encrypt(&des, 8, lmbuffer, magic);
    تياري_des_ڪنجي(pw + 7, &des);
    des_encrypt(&des, 8, lmbuffer + 8, magic);
#وإلاإذا defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
    encrypt_des(magic, lmbuffer, pw);
    encrypt_des(magic, lmbuffer + 8, pw + 7);
#جيڪڏهن_پورو

    memset(lmbuffer + 16, 0, 21 - 16);
  }

  أرجع CURLE_OK;
}

ساكن فراغ ascii_منزل_unicode_le(بنانشان حرف *dest, ثابت حرف *src,
                                size_t srclen)
{
  size_t i;
  لكل(i = 0; i < srclen; i++) {
    dest[2 * i] = (بنانشان حرف)src[i];
    dest[(2 * i) + 1] = '\0';
  }
}

/*
 * Set up nt hashed passwords
 * @unittest: 1600
 */
CURLcode Curl_ntlm_core_mk_nt_hash(ثابت حرف *password,
                                   بنانشان حرف *ntbuffer /* 21 bytes */)
{
  size_t len = لڙي_ڊيگهه(password);
  بنانشان حرف *pw;
  CURLcode نتيجو;
  إذا(len > SIZE_MAX / 2) /* avoid integer overflow */
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  pw = len ? curlx_malloc(len * 2) : (بنانشان حرف *)curlx_strdup("");
  إذا(!pw)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  ascii_منزل_unicode_le(pw, password, len);

  /* Create NT hashed password. */
  نتيجو = Curl_md4it(ntbuffer, pw, 2 * len);
  إذا(!نتيجو)
    memset(ntbuffer + 16, 0, 21 - 16);

  curlx_free(pw);

  أرجع نتيجو;
}

#جيڪڏهن_وصف_نه_ٿيل USE_WINDOWS_SSPI

#وصف_ڪريو NTLMv2_BLOB_SIGNATURE "\x01\x01\x00\x00"
#وصف_ڪريو NTLMv2_BLOB_LEN       (44 - 16 + ntlm->هدف_info_len + 4)

/* Timestamp in tenths of a microsecond since January 1, 1601 00:00:00 UTC. */
بنية ms_filetime {
  بنانشان صحيح dw_هيٺيون_Date_وقت;
  بنانشان صحيح dw_مٿاهون_Date_وقت;
};

/* Convert a time_t to an MS FILETIME (MS-DTYP section 2.3.3). */
ساكن فراغ time2filetime(بنية ms_filetime *ft, time_t t)
{
#إذا SIZEOF_وقت_T > 4
  t = (t + (curl_off_t)11644473600) * 10000000;
  ft->dw_هيٺيون_Date_وقت = (بنانشان صحيح)(t & 0xFFFFFFFF);
  ft->dw_مٿاهون_Date_وقت = (بنانشان صحيح)(t >> 32);
#وإلا
  بنانشان صحيح r, s;
  بنانشان صحيح i;

  ft->dw_هيٺيون_Date_وقت = (بنانشان صحيح)(t & 0xFFFFFFFF);
  ft->dw_مٿاهون_Date_وقت = 0;

#جيڪڏهن_وصف_نه_ٿيل HAVE_وقت_T_UNSIGNED
  /* Extend sign if needed. */
  إذا(ft->dw_هيٺيون_Date_وقت & 0x80000000)
    ft->dw_مٿاهون_Date_وقت = ~(بنانشان صحيح)0;
#جيڪڏهن_پورو

  /* Bias seconds to Jan 1, 1601.
     134774 days = 11644473600 seconds = 0x2B6109100 */
  r = ft->dw_هيٺيون_Date_وقت;
  ft->dw_هيٺيون_Date_وقت = (ft->dw_هيٺيون_Date_وقت + 0xB6109100U) & 0xFFFFFFFF;
  ft->dw_مٿاهون_Date_وقت += ft->dw_هيٺيون_Date_وقت < r ? 0x03 : 0x02;

  /* Convert to tenths of microseconds. */
  ft->dw_مٿاهون_Date_وقت *= 10000000;
  i = 32;
  افعل {
    i -= 8;
    s = ((ft->dw_هيٺيون_Date_وقت >> i) & 0xFF) * (10000000 - 1);
    r = (s << i) & 0xFFFFFFFF;
    s >>= 1;   /* Split shift to avoid width overflow. */
    s >>= 31 - i;
    ft->dw_هيٺيون_Date_وقت = (ft->dw_هيٺيون_Date_وقت + r) & 0xFFFFFFFF;
    إذا(ft->dw_هيٺيون_Date_وقت < r)
      s++;
    ft->dw_مٿاهون_Date_وقت += s;
  } طالما(i);
  ft->dw_مٿاهون_Date_وقت &= 0xFFFFFFFF;
#جيڪڏهن_پورو
}

ساكن فراغ ascii_uppercase_منزل_unicode_le(بنانشان حرف *dest,
                                          ثابت حرف *src, size_t srclen)
{
  size_t i;
  لكل(i = 0; i < srclen; i++) {
    dest[2 * i] = (بنانشان حرف)(Curl_raw_toupper(src[i]));
    dest[(2 * i) + 1] = '\0';
  }
}

/* This creates the NTLMv2 hash by using NTLM hash as the key and Unicode
 * (uppercase UserName + Domain) as the data
 */
CURLcode Curl_ntlm_core_mk_ntlmv2_hash(ثابت حرف *user, size_t userlen,
                                       ثابت حرف *domain, size_t domlen,
                                       بنانشان حرف *ntlmhash,
                                       بنانشان حرف *ntlmv2hash)
{
  /* Unicode representation */
  size_t سڃاڻپ_عنصر_len;
  بنانشان حرف *سڃاڻپ_عنصر;
  CURLcode نتيجو = CURLE_OK;

  إذا((userlen > CURL_MAX_داخلا_ڊيگهائي) || (domlen > CURL_MAX_داخلا_ڊيگهائي))
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  سڃاڻپ_عنصر_len = (userlen + domlen) * 2;
  سڃاڻپ_عنصر = curlx_malloc(سڃاڻپ_عنصر_len + 1);

  إذا(!سڃاڻپ_عنصر)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  ascii_uppercase_منزل_unicode_le(سڃاڻپ_عنصر, user, userlen);
  ascii_منزل_unicode_le(سڃاڻپ_عنصر + (userlen << 1), domain, domlen);

  نتيجو = Curl_hmacit(&Curl_HMAC_MD5, ntlmhash, 16, سڃاڻپ_عنصر, سڃاڻپ_عنصر_len,
                       ntlmv2hash);
  curlx_free(سڃاڻپ_عنصر);

  أرجع نتيجو;
}

/*
 * Curl_ntlm_core_mk_ntlmv2_resp()
 *
 * This creates the NTLMv2 response as set in the NTLM type-3 message.
 *
 * Parameters:
 *
 * ntlmv2hash       [in] - The NTLMv2 hash (16 bytes)
 * challenge_client [in] - The client nonce (8 bytes)
 * ntlm             [in] - The NTLM data struct being used to read TargetInfo
                           and Server challenge received in the type-2 message
 * ntresp          [out] - The address where a pointer to newly allocated
 *                         memory holding the NTLMv2 response.
 * ntresp_len      [out] - The length of the output message.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_ntlm_core_mk_ntlmv2_resp(ثابت بنانشان حرف *ntlmv2hash,
                                       ثابت بنانشان حرف *challenge_client,
                                       ثابت بنية ntlmdata *ntlm,
                                       بنانشان حرف **ntresp,
                                       بنانشان صحيح *ntresp_len)
{
  /* NTLMv2 response structure:
  -----------------------------------------------------------------------------
  0     HMAC MD5         16 bytes
  ------BLOB-------------------------------------------------------------------
  16    Signature        0x01010000
  20    Reserved         long (0x00000000)
  24    Timestamp        LE, 64-bit signed value representing the number of
                         tenths of a microsecond since January 1, 1601.
  32    Client Nonce     8 bytes
  40    Unknown          4 bytes
  44    Target Info      N bytes (from the type-2 message)
  44+N  Unknown          4 bytes
  -----------------------------------------------------------------------------
  */

  بنانشان صحيح len = 0;
  بنانشان حرف *ptr = عدم;
  بنانشان حرف hmac_output[HMAC_MD5_ڊيگهائي];
  بنية ms_filetime tw;

  CURLcode نتيجو = CURLE_OK;

  /* Calculate the timestamp */
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  حرف *force_timestamp = getenv("CURL_FORCETIME");
  إذا(force_timestamp)
    time2filetime(&tw, (time_t)0);
  وإلا
#جيڪڏهن_پورو
    time2filetime(&tw, وقت(عدم));

  /* Calculate the response len */
  len = HMAC_MD5_ڊيگهائي + NTLMv2_BLOB_LEN;

  /* Allocate the response */
  ptr = curlx_calloc(1, len);
  إذا(!ptr)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  /* Create the BLOB structure */
  curl_msnprintf((حرف *)ptr + HMAC_MD5_ڊيگهائي, NTLMv2_BLOB_LEN,
                 "%c%c%c%c"           /* NTLMv2_BLOB_SIGNATURE */
                 "%c%c%c%c"           /* Reserved = 0 */
                 "%c%c%c%c%c%c%c%c",  /* Timestamp */
                 NTLMv2_BLOB_SIGNATURE[0], NTLMv2_BLOB_SIGNATURE[1],
                 NTLMv2_BLOB_SIGNATURE[2], NTLMv2_BLOB_SIGNATURE[3],
                 0, 0, 0, 0,
                 LONGQUARTET(tw.dw_هيٺيون_Date_وقت),
                 LONGQUARTET(tw.dw_مٿاهون_Date_وقت));

  يادگيري_نقل(ptr + 32, challenge_client, 8);
  إذا(ntlm->هدف_info_len)
    يادگيري_نقل(ptr + 44, ntlm->هدف_info, ntlm->هدف_info_len);

  /* Concatenate the Type 2 challenge with the BLOB and do HMAC MD5 */
  يادگيري_نقل(ptr + 8, &ntlm->nonce[0], 8);
  نتيجو = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, HMAC_MD5_ڊيگهائي, ptr + 8,
                       NTLMv2_BLOB_LEN + 8, hmac_output);
  إذا(نتيجو) {
    curlx_free(ptr);
    أرجع نتيجو;
  }

  /* Concatenate the HMAC MD5 output  with the BLOB */
  يادگيري_نقل(ptr, hmac_output, HMAC_MD5_ڊيگهائي);

  /* Return the response */
  *ntresp = ptr;
  *ntresp_len = len;

  أرجع نتيجو;
}

/*
 * Curl_ntlm_core_mk_lmv2_resp()
 *
 * This creates the LMv2 response as used in the NTLM type-3 message.
 *
 * Parameters:
 *
 * ntlmv2hash        [in] - The NTLMv2 hash (16 bytes)
 * challenge_client  [in] - The client nonce (8 bytes)
 * challenge_server  [in] - The server challenge (8 bytes)
 * lmresp           [out] - The LMv2 response (24 bytes)
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_ntlm_core_mk_lmv2_resp(ثابت بنانشان حرف *ntlmv2hash,
                                     ثابت بنانشان حرف *challenge_client,
                                     ثابت بنانشان حرف *challenge_server,
                                     بنانشان حرف *lmresp)
{
  بنانشان حرف معلومات[16];
  بنانشان حرف hmac_output[16];
  CURLcode نتيجو = CURLE_OK;

  يادگيري_نقل(&معلومات[0], challenge_server, 8);
  يادگيري_نقل(&معلومات[8], challenge_client, 8);

  نتيجو = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, 16, &معلومات[0], 16,
                       hmac_output);
  إذا(نتيجو)
    أرجع نتيجو;

  /* Concatenate the HMAC MD5 output with the client nonce */
  يادگيري_نقل(lmresp, hmac_output, 16);
  يادگيري_نقل(lmresp + 16, challenge_client, 8);

  أرجع نتيجو;
}

#جيڪڏهن_پورو /* !USE_WINDOWS_SSPI */

#جيڪڏهن_پورو /* USE_CURL_NTLM_CORE */
