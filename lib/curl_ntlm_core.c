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

#اگر_تعریف_شده USE_CURL_NTLM_CORE

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

#اگر_تعریف_شده USE_GNUTLS
#ضمن <nettle/version.h>
#إذا NETTLE_نسخه_MAJOR < 4
#تعریف_کن USE_GNUTLS_DES
#پایان_اگر
#پایان_اگر

#إذا defined(USE_OPENSSL) && defined(HAVE_DES_ECB_ENCRYPT)

#  include <openssl/des.h>
#  ifdef OPENSSL_IS_AWSLC  /* for versions 1.2.0 to 1.30.1 */
#    define DES_مجموعه_کلید_unchecked (فراغ)DES_مجموعه_کلید
#  endif
#  define USE_OPENSSL_DES

#وإلاإذا defined(USE_WOLFSSL) && defined(HAVE_WC_DES_ECBENCRYPT)

#  include <wolfssl/options.h>
#  include <wolfssl/wolfcrypt/des3.h>
#  define USE_WOLFSSL_DES

#وإلاإذا defined(USE_GNUTLS_DES)
#  include <nettle/des.h>
#  define USE_CURL_DES_مجموعه_ODD_PARITY
#وإلاإذا defined(USE_MBEDTLS) && defined(HAVE_MBEDTLS_DES_CRYPT_ECB)
#  include <mbedtls/version.h>
#  إذا MBEDTLS_نسخه_عدد < 0x03020000
#  error "mbedTLS 3.2.0 or greater required"
#  endif
#  include <mbedtls/des.h>
#  define USE_MBEDTLS_DES
#وإلاإذا defined(USE_OS400CRYPTO)
#  include "cipher.mih"  /* mih/cipher */
#  define USE_CURL_DES_مجموعه_ODD_PARITY
#وإلاإذا defined(USE_WIN32_CRYPTO)
#  include <wincrypt.h>
#  define USE_CURL_DES_مجموعه_ODD_PARITY
#وإلا
#  error "cannot compile NTLM support without a crypto library with DES."
#پایان_اگر

#ضمن "urldata.h"
#ضمن "strcase.h"
#ضمن "curl_ntlm_core.h"
#ضمن "curl_md5.h"
#ضمن "curl_hmac.h"
#ضمن "curl_md4.h"
#ضمن "vauth/vauth.h"

#اگر_تعریف_شده USE_CURL_DES_مجموعه_ODD_PARITY
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
ساكن فراغ curl_des_مجموعه_odd_parity(بیعلامت حرف *بایتها, size_t len)
{
  size_t i;

  لكل(i = 0; i < len; i++) {
    بیعلامت حرف b = بایتها[i];

    bool needs_parity = (((b >> 7) ^ (b >> 6) ^ (b >> 5) ^
                          (b >> 4) ^ (b >> 3) ^ (b >> 2) ^
                          (b >> 1)) & 0x01) == 0;

    إذا(needs_parity)
      بایتها[i] |= 0x01;
    وإلا
      بایتها[i] &= 0xfe;
  }
}
#پایان_اگر /* USE_CURL_DES_SET_ODD_PARITY */

/*
 * Turns a 56-bit key into being 64-bit wide.
 */
ساكن فراغ extend_کلید_56_مقصد_64(ثابت بیعلامت حرف *کلید_56, حرف *کلید)
{
  کلید[0] = (حرف)کلید_56[0];
  کلید[1] = (حرف)(((کلید_56[0] << 7) & 0xFF) | (کلید_56[1] >> 1));
  کلید[2] = (حرف)(((کلید_56[1] << 6) & 0xFF) | (کلید_56[2] >> 2));
  کلید[3] = (حرف)(((کلید_56[2] << 5) & 0xFF) | (کلید_56[3] >> 3));
  کلید[4] = (حرف)(((کلید_56[3] << 4) & 0xFF) | (کلید_56[4] >> 4));
  کلید[5] = (حرف)(((کلید_56[4] << 3) & 0xFF) | (کلید_56[5] >> 5));
  کلید[6] = (حرف)(((کلید_56[5] << 2) & 0xFF) | (کلید_56[6] >> 6));
  کلید[7] = (حرف)((کلید_56[6] << 1) & 0xFF);
}

#اگر_تعریف_شده USE_OPENSSL_DES
/*
 * Turns a 56-bit key into a 64-bit, odd parity key and sets the key. The
 * key schedule ks is also set.
 */
ساكن فراغ راهاندازی_des_کلید(ثابت بیعلامت حرف *کلید_56, DES_کلید_زمانبندی *ks)
{
  DES_cblock کلید;

  /* Expand the 56-bit key to 64 bits */
  extend_کلید_56_مقصد_64(کلید_56, (حرف *)&کلید);

  /* Set the key parity to odd */
  DES_مجموعه_odd_parity(&کلید);

  /* Set the key */
  DES_مجموعه_کلید_unchecked(&کلید, ks);
}

#وإلاإذا defined(USE_WOLFSSL_DES)
ساكن فراغ راهاندازی_des_کلید(ثابت بیعلامت حرف *کلید_56, Des *des)
{
  byte کلید[8];

  /* Expand the 56-bit key to 64 bits */
  extend_کلید_56_مقصد_64(کلید_56, (حرف *)کلید);

  /* Set the key */
  wc_Des_مجموعه_کلید(des, کلید, عدم, 0);
}

#وإلاإذا defined(USE_GNUTLS_DES)
ساكن فراغ راهاندازی_des_کلید(ثابت بیعلامت حرف *کلید_56, بنية des_ctx *des)
{
  حرف کلید[8];

  /* Expand the 56-bit key to 64 bits */
  extend_کلید_56_مقصد_64(کلید_56, کلید);

  /* Set the key parity to odd */
  curl_des_مجموعه_odd_parity((بیعلامت حرف *)کلید, حجم(کلید));

  /* Set the key */
  des_مجموعه_کلید(des, (ثابت uint8_t *)کلید);
}

#وإلاإذا defined(USE_MBEDTLS_DES)
ساكن bool encrypt_des(ثابت بیعلامت حرف *درون, بیعلامت حرف *بیرون,
                        ثابت بیعلامت حرف *کلید_56)
{
  mbedtls_des_context ctx;
  حرف کلید[8];

  /* Expand the 56-bit key to 64 bits */
  extend_کلید_56_مقصد_64(کلید_56, کلید);

  /* Set the key parity to odd */
  mbedtls_des_کلید_مجموعه_parity((بیعلامت حرف *)کلید);

  /* Perform the encryption */
  mbedtls_des_init(&ctx);
  mbedtls_des_setkey_enc(&ctx, (بیعلامت حرف *)کلید);
  أرجع mbedtls_des_crypt_ecb(&ctx, درون, بیرون) == 0;
}

#وإلاإذا defined(USE_OS400CRYPTO)
ساكن bool encrypt_des(ثابت بیعلامت حرف *درون, بیعلامت حرف *بیرون,
                        ثابت بیعلامت حرف *کلید_56)
{
  حرف کلید[8];
  _CIPHER_Control_T ctl;

  /* Setup the cipher control structure */
  ctl.Func_ID = ENCRYPT_ONLY;
  ctl.داده_Len = حجم(کلید);

  /* Expand the 56-bit key to 64 bits */
  extend_کلید_56_مقصد_64(کلید_56, ctl.Crypto_کلید);

  /* Set the key parity to odd */
  curl_des_مجموعه_odd_parity((بیعلامت حرف *)ctl.Crypto_کلید, ctl.داده_Len);

  /* Perform the encryption */
  _CIPHER((_SPCPTR *)&بیرون, &ctl, (_SPCPTR *)&درون);

  أرجع TRUE;
}

#وإلاإذا defined(USE_WIN32_CRYPTO)
ساكن bool encrypt_des(ثابت بیعلامت حرف *درون, بیعلامت حرف *بیرون,
                        ثابت بیعلامت حرف *کلید_56)
{
  HCRYPTPROV hprov;
  HCRYPTKEY hkey;
  بنية {
    BLOBHEADER hdr;
    بیعلامت صحيح len;
    حرف کلید[8];
  } blob;
  DWORD len = 8;
  BOOL res;

  /* Acquire the crypto provider */
  إذا(!CryptAcquireContext(&hprov, عدم, عدم, PROV_RSA_پر,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    أرجع FALSE;

  /* Setup the key blob structure */
  memset(&blob, 0, حجم(blob));
  blob.hdr.bType = PLAINTEXTKEYBLOB;
  blob.hdr.b_نسخه = 2;
  blob.hdr.ai_کلید_Alg = CALG_DES;
  blob.len = حجم(blob.کلید);

  /* Expand the 56-bit key to 64 bits */
  extend_کلید_56_مقصد_64(کلید_56, blob.کلید);

  /* Set the key parity to odd */
  curl_des_مجموعه_odd_parity((بیعلامت حرف *)blob.کلید, حجم(blob.کلید));

  /* Import the key */
  إذا(!Crypt_Import_کلید(hprov, (BYTE *)&blob, حجم(blob), 0, 0, &hkey)) {
    CryptReleaseContext(hprov, 0);

    أرجع FALSE;
  }

  کپی_حافظه(بیرون, درون, 8);

  /* Perform the encryption */
  res = CryptEncrypt(hkey, 0, FALSE, 0, بیرون, &len, len);

  Crypt_Destroy_کلید(hkey);
  CryptReleaseContext(hprov, 0);

  أرجع res;
}

#پایان_اگر /* crypto backends */

/*
 * takes a 21-byte array and treats it as 3 56-bit DES keys. The
 * 8-byte plaintext is encrypted with each key and the resulting 24
 * bytes are stored in the results array.
 */
فراغ Curl_ntlm_core_lm_resp(ثابت بیعلامت حرف *کلیدها,
                            ثابت بیعلامت حرف *plaintext,
                            بیعلامت حرف *results)
{
#اگر_تعریف_شده USE_OPENSSL_DES
  DES_کلید_زمانبندی ks;

  راهاندازی_des_کلید(کلیدها, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)results, &ks, DES_ENCRYPT);

  راهاندازی_des_کلید(کلیدها + 7, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 8), &ks, DES_ENCRYPT);

  راهاندازی_des_کلید(کلیدها + 14, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 16), &ks, DES_ENCRYPT);
#وإلاإذا defined(USE_WOLFSSL_DES)
  Des des;
  راهاندازی_des_کلید(کلیدها, &des);
  wc_Des_EcbEncrypt(&des, results, plaintext, DES_کلید_اندازه);
  راهاندازی_des_کلید(کلیدها + 7, &des);
  wc_Des_EcbEncrypt(&des, results + 8, plaintext, DES_کلید_اندازه);
  راهاندازی_des_کلید(کلیدها + 14, &des);
  wc_Des_EcbEncrypt(&des, results + 16, plaintext, DES_کلید_اندازه);
#وإلاإذا defined(USE_GNUTLS_DES)
  بنية des_ctx des;
  راهاندازی_des_کلید(کلیدها, &des);
  des_encrypt(&des, 8, results, plaintext);
  راهاندازی_des_کلید(کلیدها + 7, &des);
  des_encrypt(&des, 8, results + 8, plaintext);
  راهاندازی_des_کلید(کلیدها + 14, &des);
  des_encrypt(&des, 8, results + 16, plaintext);
#وإلاإذا defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
  encrypt_des(plaintext, results, کلیدها);
  encrypt_des(plaintext, results + 8, کلیدها + 7);
  encrypt_des(plaintext, results + 16, کلیدها + 14);
#وإلا
  (فراغ)کلیدها;
  (فراغ)plaintext;
  (فراغ)results;
#پایان_اگر
}

/*
 * Set up lanmanager hashed password
 */
CURLcode Curl_ntlm_core_mk_lm_hash(ثابت حرف *password,
                                   بیعلامت حرف *lmbuffer /* 21 bytes */)
{
  بیعلامت حرف pw[14];
  ساكن ثابت بیعلامت حرف magic[] = {
    0x4B, 0x47, 0x53, 0x21, 0x40, 0x23, 0x24, 0x25 /* i.e. KGS!@#$% */
  };
  size_t len = CURLMIN(طول_رشته(password), 14);

  Curl_strntoupper((حرف *)pw, password, len);
  memset(&pw[len], 0, 14 - len);

  {
    /* Create LanManager hashed password. */
#اگر_تعریف_شده USE_OPENSSL_DES
    DES_کلید_زمانبندی ks;

    راهاندازی_des_کلید(pw, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)lmbuffer, &ks, DES_ENCRYPT);

    راهاندازی_des_کلید(pw + 7, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)(lmbuffer + 8), &ks, DES_ENCRYPT);
#وإلاإذا defined(USE_WOLFSSL_DES)
    Des des;
    راهاندازی_des_کلید(pw, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer, magic, DES_کلید_اندازه);
    راهاندازی_des_کلید(pw + 7, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer + 8, magic, DES_کلید_اندازه);
#وإلاإذا defined(USE_GNUTLS_DES)
    بنية des_ctx des;
    راهاندازی_des_کلید(pw, &des);
    des_encrypt(&des, 8, lmbuffer, magic);
    راهاندازی_des_کلید(pw + 7, &des);
    des_encrypt(&des, 8, lmbuffer + 8, magic);
#وإلاإذا defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
    encrypt_des(magic, lmbuffer, pw);
    encrypt_des(magic, lmbuffer + 8, pw + 7);
#پایان_اگر

    memset(lmbuffer + 16, 0, 21 - 16);
  }

  أرجع CURLE_OK;
}

ساكن فراغ ascii_مقصد_unicode_le(بیعلامت حرف *dest, ثابت حرف *src,
                                size_t srclen)
{
  size_t i;
  لكل(i = 0; i < srclen; i++) {
    dest[2 * i] = (بیعلامت حرف)src[i];
    dest[(2 * i) + 1] = '\0';
  }
}

/*
 * Set up nt hashed passwords
 * @unittest: 1600
 */
CURLcode Curl_ntlm_core_mk_nt_hash(ثابت حرف *password,
                                   بیعلامت حرف *ntbuffer /* 21 bytes */)
{
  size_t len = طول_رشته(password);
  بیعلامت حرف *pw;
  CURLcode نتیجه;
  إذا(len > SIZE_MAX / 2) /* avoid integer overflow */
    أرجع CURLE_بیرون_از_حافظه;
  pw = len ? curlx_malloc(len * 2) : (بیعلامت حرف *)curlx_strdup("");
  إذا(!pw)
    أرجع CURLE_بیرون_از_حافظه;

  ascii_مقصد_unicode_le(pw, password, len);

  /* Create NT hashed password. */
  نتیجه = Curl_md4it(ntbuffer, pw, 2 * len);
  إذا(!نتیجه)
    memset(ntbuffer + 16, 0, 21 - 16);

  curlx_free(pw);

  أرجع نتیجه;
}

#اگر_تعریف_نشده USE_WINDOWS_SSPI

#تعریف_کن NTLMv2_BLOB_SIGNATURE "\x01\x01\x00\x00"
#تعریف_کن NTLMv2_BLOB_LEN       (44 - 16 + ntlm->هدف_info_len + 4)

/* Timestamp in tenths of a microsecond since January 1, 1601 00:00:00 UTC. */
بنية ms_filetime {
  بیعلامت صحيح dw_کم_Date_زمان;
  بیعلامت صحيح dw_بالا_Date_زمان;
};

/* Convert a time_t to an MS FILETIME (MS-DTYP section 2.3.3). */
ساكن فراغ time2filetime(بنية ms_filetime *ft, time_t t)
{
#إذا SIZEOF_زمان_T > 4
  t = (t + (curl_off_t)11644473600) * 10000000;
  ft->dw_کم_Date_زمان = (بیعلامت صحيح)(t & 0xFFFFFFFF);
  ft->dw_بالا_Date_زمان = (بیعلامت صحيح)(t >> 32);
#وإلا
  بیعلامت صحيح r, s;
  بیعلامت صحيح i;

  ft->dw_کم_Date_زمان = (بیعلامت صحيح)(t & 0xFFFFFFFF);
  ft->dw_بالا_Date_زمان = 0;

#اگر_تعریف_نشده HAVE_زمان_T_UNSIGNED
  /* Extend sign if needed. */
  إذا(ft->dw_کم_Date_زمان & 0x80000000)
    ft->dw_بالا_Date_زمان = ~(بیعلامت صحيح)0;
#پایان_اگر

  /* Bias seconds to Jan 1, 1601.
     134774 days = 11644473600 seconds = 0x2B6109100 */
  r = ft->dw_کم_Date_زمان;
  ft->dw_کم_Date_زمان = (ft->dw_کم_Date_زمان + 0xB6109100U) & 0xFFFFFFFF;
  ft->dw_بالا_Date_زمان += ft->dw_کم_Date_زمان < r ? 0x03 : 0x02;

  /* Convert to tenths of microseconds. */
  ft->dw_بالا_Date_زمان *= 10000000;
  i = 32;
  افعل {
    i -= 8;
    s = ((ft->dw_کم_Date_زمان >> i) & 0xFF) * (10000000 - 1);
    r = (s << i) & 0xFFFFFFFF;
    s >>= 1;   /* Split shift to avoid width overflow. */
    s >>= 31 - i;
    ft->dw_کم_Date_زمان = (ft->dw_کم_Date_زمان + r) & 0xFFFFFFFF;
    إذا(ft->dw_کم_Date_زمان < r)
      s++;
    ft->dw_بالا_Date_زمان += s;
  } طالما(i);
  ft->dw_بالا_Date_زمان &= 0xFFFFFFFF;
#پایان_اگر
}

ساكن فراغ ascii_uppercase_مقصد_unicode_le(بیعلامت حرف *dest,
                                          ثابت حرف *src, size_t srclen)
{
  size_t i;
  لكل(i = 0; i < srclen; i++) {
    dest[2 * i] = (بیعلامت حرف)(Curl_raw_toupper(src[i]));
    dest[(2 * i) + 1] = '\0';
  }
}

/* This creates the NTLMv2 hash by using NTLM hash as the key and Unicode
 * (uppercase UserName + Domain) as the data
 */
CURLcode Curl_ntlm_core_mk_ntlmv2_hash(ثابت حرف *user, size_t userlen,
                                       ثابت حرف *domain, size_t domlen,
                                       بیعلامت حرف *ntlmhash,
                                       بیعلامت حرف *ntlmv2hash)
{
  /* Unicode representation */
  size_t عنصر_خنثی_len;
  بیعلامت حرف *عنصر_خنثی;
  CURLcode نتیجه = CURLE_OK;

  إذا((userlen > CURL_MAX_درون_داد_درازی) || (domlen > CURL_MAX_درون_داد_درازی))
    أرجع CURLE_بیرون_از_حافظه;

  عنصر_خنثی_len = (userlen + domlen) * 2;
  عنصر_خنثی = curlx_malloc(عنصر_خنثی_len + 1);

  إذا(!عنصر_خنثی)
    أرجع CURLE_بیرون_از_حافظه;

  ascii_uppercase_مقصد_unicode_le(عنصر_خنثی, user, userlen);
  ascii_مقصد_unicode_le(عنصر_خنثی + (userlen << 1), domain, domlen);

  نتیجه = Curl_hmacit(&Curl_HMAC_MD5, ntlmhash, 16, عنصر_خنثی, عنصر_خنثی_len,
                       ntlmv2hash);
  curlx_free(عنصر_خنثی);

  أرجع نتیجه;
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
CURLcode Curl_ntlm_core_mk_ntlmv2_resp(ثابت بیعلامت حرف *ntlmv2hash,
                                       ثابت بیعلامت حرف *challenge_client,
                                       ثابت بنية ntlmdata *ntlm,
                                       بیعلامت حرف **ntresp,
                                       بیعلامت صحيح *ntresp_len)
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

  بیعلامت صحيح len = 0;
  بیعلامت حرف *ptr = عدم;
  بیعلامت حرف hmac_output[HMAC_MD5_درازی];
  بنية ms_filetime tw;

  CURLcode نتیجه = CURLE_OK;

  /* Calculate the timestamp */
#اگر_تعریف_شده DEBUGBUILD
  حرف *force_timestamp = getenv("CURL_FORCETIME");
  إذا(force_timestamp)
    time2filetime(&tw, (time_t)0);
  وإلا
#پایان_اگر
    time2filetime(&tw, زمان(عدم));

  /* Calculate the response len */
  len = HMAC_MD5_درازی + NTLMv2_BLOB_LEN;

  /* Allocate the response */
  ptr = curlx_calloc(1, len);
  إذا(!ptr)
    أرجع CURLE_بیرون_از_حافظه;

  /* Create the BLOB structure */
  curl_msnprintf((حرف *)ptr + HMAC_MD5_درازی, NTLMv2_BLOB_LEN,
                 "%c%c%c%c"           /* NTLMv2_BLOB_SIGNATURE */
                 "%c%c%c%c"           /* Reserved = 0 */
                 "%c%c%c%c%c%c%c%c",  /* Timestamp */
                 NTLMv2_BLOB_SIGNATURE[0], NTLMv2_BLOB_SIGNATURE[1],
                 NTLMv2_BLOB_SIGNATURE[2], NTLMv2_BLOB_SIGNATURE[3],
                 0, 0, 0, 0,
                 LONGQUARTET(tw.dw_کم_Date_زمان),
                 LONGQUARTET(tw.dw_بالا_Date_زمان));

  کپی_حافظه(ptr + 32, challenge_client, 8);
  إذا(ntlm->هدف_info_len)
    کپی_حافظه(ptr + 44, ntlm->هدف_info, ntlm->هدف_info_len);

  /* Concatenate the Type 2 challenge with the BLOB and do HMAC MD5 */
  کپی_حافظه(ptr + 8, &ntlm->nonce[0], 8);
  نتیجه = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, HMAC_MD5_درازی, ptr + 8,
                       NTLMv2_BLOB_LEN + 8, hmac_output);
  إذا(نتیجه) {
    curlx_free(ptr);
    أرجع نتیجه;
  }

  /* Concatenate the HMAC MD5 output  with the BLOB */
  کپی_حافظه(ptr, hmac_output, HMAC_MD5_درازی);

  /* Return the response */
  *ntresp = ptr;
  *ntresp_len = len;

  أرجع نتیجه;
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
CURLcode Curl_ntlm_core_mk_lmv2_resp(ثابت بیعلامت حرف *ntlmv2hash,
                                     ثابت بیعلامت حرف *challenge_client,
                                     ثابت بیعلامت حرف *challenge_server,
                                     بیعلامت حرف *lmresp)
{
  بیعلامت حرف داده[16];
  بیعلامت حرف hmac_output[16];
  CURLcode نتیجه = CURLE_OK;

  کپی_حافظه(&داده[0], challenge_server, 8);
  کپی_حافظه(&داده[8], challenge_client, 8);

  نتیجه = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, 16, &داده[0], 16,
                       hmac_output);
  إذا(نتیجه)
    أرجع نتیجه;

  /* Concatenate the HMAC MD5 output with the client nonce */
  کپی_حافظه(lmresp, hmac_output, 16);
  کپی_حافظه(lmresp + 16, challenge_client, 8);

  أرجع نتیجه;
}

#پایان_اگر /* !USE_WINDOWS_SSPI */

#پایان_اگر /* USE_CURL_NTLM_CORE */
