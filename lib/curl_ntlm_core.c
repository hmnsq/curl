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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ USE_CURL_NTLM_CORE

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

#اگر_تعریف_شدہ USE_GNUTLS
#شامل <nettle/version.h>
#اگر NETTLE_نسخہ_MAJOR < 4
#تعریف_کرو USE_GNUTLS_DES
#اگر_ختم
#اگر_ختم

#اگر defined(USE_OPENSSL) && defined(HAVE_DES_ECB_ENCRYPT)

#  include <openssl/des.h>
#  ifdef OPENSSL_IS_AWSLC  /* for versions 1.2.0 to 1.30.1 */
#    define DES_سیٹ_کلید_unchecked (خالی)DES_سیٹ_کلید
#  endif
#  define USE_OPENSSL_DES

#وگرنہ_اگر defined(USE_WOLFSSL) && defined(HAVE_WC_DES_ECBENCRYPT)

#  include <wolfssl/options.h>
#  include <wolfssl/wolfcrypt/des3.h>
#  define USE_WOLFSSL_DES

#وگرنہ_اگر defined(USE_GNUTLS_DES)
#  include <nettle/des.h>
#  define USE_CURL_DES_سیٹ_ODD_PARITY
#وگرنہ_اگر defined(USE_MBEDTLS) && defined(HAVE_MBEDTLS_DES_CRYPT_ECB)
#  include <mbedtls/version.h>
#  اگر MBEDTLS_نسخہ_نمبر < 0x03020000
#  error "mbedTLS 3.2.0 or greater required"
#  endif
#  include <mbedtls/des.h>
#  define USE_MBEDTLS_DES
#وگرنہ_اگر defined(USE_OS400CRYPTO)
#  include "cipher.mih"  /* mih/cipher */
#  define USE_CURL_DES_سیٹ_ODD_PARITY
#وگرنہ_اگر defined(USE_WIN32_CRYPTO)
#  include <wincrypt.h>
#  define USE_CURL_DES_سیٹ_ODD_PARITY
#ورنہ
#  error "cannot compile NTLM support without a crypto library with DES."
#اگر_ختم

#شامل "urldata.h"
#شامل "strcase.h"
#شامل "curl_ntlm_core.h"
#شامل "curl_md5.h"
#شامل "curl_hmac.h"
#شامل "curl_md4.h"
#شامل "vauth/vauth.h"

#اگر_تعریف_شدہ USE_CURL_DES_سیٹ_ODD_PARITY
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
ساکن خالی curl_des_سیٹ_odd_parity(غیرعلامتی حرف *بائٹیں, size_t len)
{
  size_t i;

  برائے(i = 0; i < len; i++) {
    غیرعلامتی حرف b = بائٹیں[i];

    bool needs_parity = (((b >> 7) ^ (b >> 6) ^ (b >> 5) ^
                          (b >> 4) ^ (b >> 3) ^ (b >> 2) ^
                          (b >> 1)) & 0x01) == 0;

    اگر(needs_parity)
      بائٹیں[i] |= 0x01;
    ورنہ
      بائٹیں[i] &= 0xfe;
  }
}
#اگر_ختم /* USE_CURL_DES_SET_ODD_PARITY */

/*
 * Turns a 56-bit key into being 64-bit wide.
 */
ساکن خالی extend_کلید_56_مقصد_64(ثابت غیرعلامتی حرف *کلید_56, حرف *کلید)
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

#اگر_تعریف_شدہ USE_OPENSSL_DES
/*
 * Turns a 56-bit key into a 64-bit, odd parity key and sets the key. The
 * key schedule ks is also set.
 */
ساکن خالی تیاری_des_کلید(ثابت غیرعلامتی حرف *کلید_56, DES_کلید_جدول_وقت *ks)
{
  DES_cblock کلید;

  /* Expand the 56-bit key to 64 bits */
  extend_کلید_56_مقصد_64(کلید_56, (حرف *)&کلید);

  /* Set the key parity to odd */
  DES_سیٹ_odd_parity(&کلید);

  /* Set the key */
  DES_سیٹ_کلید_unchecked(&کلید, ks);
}

#وگرنہ_اگر defined(USE_WOLFSSL_DES)
ساکن خالی تیاری_des_کلید(ثابت غیرعلامتی حرف *کلید_56, Des *des)
{
  byte کلید[8];

  /* Expand the 56-bit key to 64 bits */
  extend_کلید_56_مقصد_64(کلید_56, (حرف *)کلید);

  /* Set the key */
  wc_Des_سیٹ_کلید(des, کلید, تہی, 0);
}

#وگرنہ_اگر defined(USE_GNUTLS_DES)
ساکن خالی تیاری_des_کلید(ثابت غیرعلامتی حرف *کلید_56, ساخت des_ctx *des)
{
  حرف کلید[8];

  /* Expand the 56-bit key to 64 bits */
  extend_کلید_56_مقصد_64(کلید_56, کلید);

  /* Set the key parity to odd */
  curl_des_سیٹ_odd_parity((غیرعلامتی حرف *)کلید, حجم(کلید));

  /* Set the key */
  des_سیٹ_کلید(des, (ثابت uint8_t *)کلید);
}

#وگرنہ_اگر defined(USE_MBEDTLS_DES)
ساکن bool encrypt_des(ثابت غیرعلامتی حرف *اندر, غیرعلامتی حرف *برآمد,
                        ثابت غیرعلامتی حرف *کلید_56)
{
  mbedtls_des_context ctx;
  حرف کلید[8];

  /* Expand the 56-bit key to 64 bits */
  extend_کلید_56_مقصد_64(کلید_56, کلید);

  /* Set the key parity to odd */
  mbedtls_des_کلید_سیٹ_parity((غیرعلامتی حرف *)کلید);

  /* Perform the encryption */
  mbedtls_des_init(&ctx);
  mbedtls_des_setkey_enc(&ctx, (غیرعلامتی حرف *)کلید);
  واپس mbedtls_des_crypt_ecb(&ctx, اندر, برآمد) == 0;
}

#وگرنہ_اگر defined(USE_OS400CRYPTO)
ساکن bool encrypt_des(ثابت غیرعلامتی حرف *اندر, غیرعلامتی حرف *برآمد,
                        ثابت غیرعلامتی حرف *کلید_56)
{
  حرف کلید[8];
  _CIPHER_Control_T ctl;

  /* Setup the cipher control structure */
  ctl.Func_ID = ENCRYPT_ONLY;
  ctl.ڈیٹا_Len = حجم(کلید);

  /* Expand the 56-bit key to 64 bits */
  extend_کلید_56_مقصد_64(کلید_56, ctl.Crypto_کلید);

  /* Set the key parity to odd */
  curl_des_سیٹ_odd_parity((غیرعلامتی حرف *)ctl.Crypto_کلید, ctl.ڈیٹا_Len);

  /* Perform the encryption */
  _CIPHER((_SPCPTR *)&برآمد, &ctl, (_SPCPTR *)&اندر);

  واپس TRUE;
}

#وگرنہ_اگر defined(USE_WIN32_CRYPTO)
ساکن bool encrypt_des(ثابت غیرعلامتی حرف *اندر, غیرعلامتی حرف *برآمد,
                        ثابت غیرعلامتی حرف *کلید_56)
{
  HCRYPTPROV hprov;
  HCRYPTKEY hkey;
  ساخت {
    BLOBHEADER hdr;
    غیرعلامتی صحیح len;
    حرف کلید[8];
  } blob;
  DWORD len = 8;
  BOOL res;

  /* Acquire the crypto provider */
  اگر(!CryptAcquireContext(&hprov, تہی, تہی, PROV_RSA_بھرا,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    واپس FALSE;

  /* Setup the key blob structure */
  memset(&blob, 0, حجم(blob));
  blob.hdr.bType = PLAINTEXTKEYBLOB;
  blob.hdr.b_نسخہ = 2;
  blob.hdr.ai_کلید_Alg = CALG_DES;
  blob.len = حجم(blob.کلید);

  /* Expand the 56-bit key to 64 bits */
  extend_کلید_56_مقصد_64(کلید_56, blob.کلید);

  /* Set the key parity to odd */
  curl_des_سیٹ_odd_parity((غیرعلامتی حرف *)blob.کلید, حجم(blob.کلید));

  /* Import the key */
  اگر(!Crypt_Import_کلید(hprov, (BYTE *)&blob, حجم(blob), 0, 0, &hkey)) {
    CryptReleaseContext(hprov, 0);

    واپس FALSE;
  }

  حافظہ_نقل(برآمد, اندر, 8);

  /* Perform the encryption */
  res = CryptEncrypt(hkey, 0, FALSE, 0, برآمد, &len, len);

  Crypt_Destroy_کلید(hkey);
  CryptReleaseContext(hprov, 0);

  واپس res;
}

#اگر_ختم /* crypto backends */

/*
 * takes a 21-byte array and treats it as 3 56-bit DES keys. The
 * 8-byte plaintext is encrypted with each key and the resulting 24
 * bytes are stored in the results array.
 */
خالی Curl_ntlm_core_lm_resp(ثابت غیرعلامتی حرف *کلیدیں,
                            ثابت غیرعلامتی حرف *plaintext,
                            غیرعلامتی حرف *results)
{
#اگر_تعریف_شدہ USE_OPENSSL_DES
  DES_کلید_جدول_وقت ks;

  تیاری_des_کلید(کلیدیں, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)results, &ks, DES_ENCRYPT);

  تیاری_des_کلید(کلیدیں + 7, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 8), &ks, DES_ENCRYPT);

  تیاری_des_کلید(کلیدیں + 14, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 16), &ks, DES_ENCRYPT);
#وگرنہ_اگر defined(USE_WOLFSSL_DES)
  Des des;
  تیاری_des_کلید(کلیدیں, &des);
  wc_Des_EcbEncrypt(&des, results, plaintext, DES_کلید_سائز);
  تیاری_des_کلید(کلیدیں + 7, &des);
  wc_Des_EcbEncrypt(&des, results + 8, plaintext, DES_کلید_سائز);
  تیاری_des_کلید(کلیدیں + 14, &des);
  wc_Des_EcbEncrypt(&des, results + 16, plaintext, DES_کلید_سائز);
#وگرنہ_اگر defined(USE_GNUTLS_DES)
  ساخت des_ctx des;
  تیاری_des_کلید(کلیدیں, &des);
  des_encrypt(&des, 8, results, plaintext);
  تیاری_des_کلید(کلیدیں + 7, &des);
  des_encrypt(&des, 8, results + 8, plaintext);
  تیاری_des_کلید(کلیدیں + 14, &des);
  des_encrypt(&des, 8, results + 16, plaintext);
#وگرنہ_اگر defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
  encrypt_des(plaintext, results, کلیدیں);
  encrypt_des(plaintext, results + 8, کلیدیں + 7);
  encrypt_des(plaintext, results + 16, کلیدیں + 14);
#ورنہ
  (خالی)کلیدیں;
  (خالی)plaintext;
  (خالی)results;
#اگر_ختم
}

/*
 * Set up lanmanager hashed password
 */
CURLcode Curl_ntlm_core_mk_lm_hash(ثابت حرف *password,
                                   غیرعلامتی حرف *lmbuffer /* 21 bytes */)
{
  غیرعلامتی حرف pw[14];
  ساکن ثابت غیرعلامتی حرف magic[] = {
    0x4B, 0x47, 0x53, 0x21, 0x40, 0x23, 0x24, 0x25 /* i.e. KGS!@#$% */
  };
  size_t len = CURLMIN(لڑی_لمبائی(password), 14);

  Curl_strntoupper((حرف *)pw, password, len);
  memset(&pw[len], 0, 14 - len);

  {
    /* Create LanManager hashed password. */
#اگر_تعریف_شدہ USE_OPENSSL_DES
    DES_کلید_جدول_وقت ks;

    تیاری_des_کلید(pw, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)lmbuffer, &ks, DES_ENCRYPT);

    تیاری_des_کلید(pw + 7, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)(lmbuffer + 8), &ks, DES_ENCRYPT);
#وگرنہ_اگر defined(USE_WOLFSSL_DES)
    Des des;
    تیاری_des_کلید(pw, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer, magic, DES_کلید_سائز);
    تیاری_des_کلید(pw + 7, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer + 8, magic, DES_کلید_سائز);
#وگرنہ_اگر defined(USE_GNUTLS_DES)
    ساخت des_ctx des;
    تیاری_des_کلید(pw, &des);
    des_encrypt(&des, 8, lmbuffer, magic);
    تیاری_des_کلید(pw + 7, &des);
    des_encrypt(&des, 8, lmbuffer + 8, magic);
#وگرنہ_اگر defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
    encrypt_des(magic, lmbuffer, pw);
    encrypt_des(magic, lmbuffer + 8, pw + 7);
#اگر_ختم

    memset(lmbuffer + 16, 0, 21 - 16);
  }

  واپس CURLE_OK;
}

ساکن خالی ascii_مقصد_unicode_le(غیرعلامتی حرف *dest, ثابت حرف *src,
                                size_t srclen)
{
  size_t i;
  برائے(i = 0; i < srclen; i++) {
    dest[2 * i] = (غیرعلامتی حرف)src[i];
    dest[(2 * i) + 1] = '\0';
  }
}

/*
 * Set up nt hashed passwords
 * @unittest: 1600
 */
CURLcode Curl_ntlm_core_mk_nt_hash(ثابت حرف *password,
                                   غیرعلامتی حرف *ntbuffer /* 21 bytes */)
{
  size_t len = لڑی_لمبائی(password);
  غیرعلامتی حرف *pw;
  CURLcode نتیجہ;
  اگر(len > SIZE_MAX / 2) /* avoid integer overflow */
    واپس CURLE_برآمد_کا_حافظہ;
  pw = len ? curlx_malloc(len * 2) : (غیرعلامتی حرف *)curlx_strdup("");
  اگر(!pw)
    واپس CURLE_برآمد_کا_حافظہ;

  ascii_مقصد_unicode_le(pw, password, len);

  /* Create NT hashed password. */
  نتیجہ = Curl_md4it(ntbuffer, pw, 2 * len);
  اگر(!نتیجہ)
    memset(ntbuffer + 16, 0, 21 - 16);

  curlx_free(pw);

  واپس نتیجہ;
}

#اگر_غیر_تعریف_شدہ USE_WINDOWS_SSPI

#تعریف_کرو NTLMv2_BLOB_SIGNATURE "\x01\x01\x00\x00"
#تعریف_کرو NTLMv2_BLOB_LEN       (44 - 16 + ntlm->ہدف_info_len + 4)

/* Timestamp in tenths of a microsecond since January 1, 1601 00:00:00 UTC. */
ساخت ms_filetime {
  غیرعلامتی صحیح dw_کم_Date_وقت;
  غیرعلامتی صحیح dw_بلند_Date_وقت;
};

/* Convert a time_t to an MS FILETIME (MS-DTYP section 2.3.3). */
ساکن خالی time2filetime(ساخت ms_filetime *ft, time_t t)
{
#اگر SIZEOF_وقت_T > 4
  t = (t + (curl_off_t)11644473600) * 10000000;
  ft->dw_کم_Date_وقت = (غیرعلامتی صحیح)(t & 0xFFFFFFFF);
  ft->dw_بلند_Date_وقت = (غیرعلامتی صحیح)(t >> 32);
#ورنہ
  غیرعلامتی صحیح r, s;
  غیرعلامتی صحیح i;

  ft->dw_کم_Date_وقت = (غیرعلامتی صحیح)(t & 0xFFFFFFFF);
  ft->dw_بلند_Date_وقت = 0;

#اگر_غیر_تعریف_شدہ HAVE_وقت_T_UNSIGNED
  /* Extend sign if needed. */
  اگر(ft->dw_کم_Date_وقت & 0x80000000)
    ft->dw_بلند_Date_وقت = ~(غیرعلامتی صحیح)0;
#اگر_ختم

  /* Bias seconds to Jan 1, 1601.
     134774 days = 11644473600 seconds = 0x2B6109100 */
  r = ft->dw_کم_Date_وقت;
  ft->dw_کم_Date_وقت = (ft->dw_کم_Date_وقت + 0xB6109100U) & 0xFFFFFFFF;
  ft->dw_بلند_Date_وقت += ft->dw_کم_Date_وقت < r ? 0x03 : 0x02;

  /* Convert to tenths of microseconds. */
  ft->dw_بلند_Date_وقت *= 10000000;
  i = 32;
  کرو {
    i -= 8;
    s = ((ft->dw_کم_Date_وقت >> i) & 0xFF) * (10000000 - 1);
    r = (s << i) & 0xFFFFFFFF;
    s >>= 1;   /* Split shift to avoid width overflow. */
    s >>= 31 - i;
    ft->dw_کم_Date_وقت = (ft->dw_کم_Date_وقت + r) & 0xFFFFFFFF;
    اگر(ft->dw_کم_Date_وقت < r)
      s++;
    ft->dw_بلند_Date_وقت += s;
  } جب(i);
  ft->dw_بلند_Date_وقت &= 0xFFFFFFFF;
#اگر_ختم
}

ساکن خالی ascii_uppercase_مقصد_unicode_le(غیرعلامتی حرف *dest,
                                          ثابت حرف *src, size_t srclen)
{
  size_t i;
  برائے(i = 0; i < srclen; i++) {
    dest[2 * i] = (غیرعلامتی حرف)(Curl_raw_toupper(src[i]));
    dest[(2 * i) + 1] = '\0';
  }
}

/* This creates the NTLMv2 hash by using NTLM hash as the key and Unicode
 * (uppercase UserName + Domain) as the data
 */
CURLcode Curl_ntlm_core_mk_ntlmv2_hash(ثابت حرف *user, size_t userlen,
                                       ثابت حرف *domain, size_t domlen,
                                       غیرعلامتی حرف *ntlmhash,
                                       غیرعلامتی حرف *ntlmv2hash)
{
  /* Unicode representation */
  size_t شناختی_عنصر_len;
  غیرعلامتی حرف *شناختی_عنصر;
  CURLcode نتیجہ = CURLE_OK;

  اگر((userlen > CURL_MAX_داخلہ_لمبائی) || (domlen > CURL_MAX_داخلہ_لمبائی))
    واپس CURLE_برآمد_کا_حافظہ;

  شناختی_عنصر_len = (userlen + domlen) * 2;
  شناختی_عنصر = curlx_malloc(شناختی_عنصر_len + 1);

  اگر(!شناختی_عنصر)
    واپس CURLE_برآمد_کا_حافظہ;

  ascii_uppercase_مقصد_unicode_le(شناختی_عنصر, user, userlen);
  ascii_مقصد_unicode_le(شناختی_عنصر + (userlen << 1), domain, domlen);

  نتیجہ = Curl_hmacit(&Curl_HMAC_MD5, ntlmhash, 16, شناختی_عنصر, شناختی_عنصر_len,
                       ntlmv2hash);
  curlx_free(شناختی_عنصر);

  واپس نتیجہ;
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
CURLcode Curl_ntlm_core_mk_ntlmv2_resp(ثابت غیرعلامتی حرف *ntlmv2hash,
                                       ثابت غیرعلامتی حرف *challenge_client,
                                       ثابت ساخت ntlmdata *ntlm,
                                       غیرعلامتی حرف **ntresp,
                                       غیرعلامتی صحیح *ntresp_len)
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

  غیرعلامتی صحیح len = 0;
  غیرعلامتی حرف *ptr = تہی;
  غیرعلامتی حرف hmac_output[HMAC_MD5_لمبائی];
  ساخت ms_filetime tw;

  CURLcode نتیجہ = CURLE_OK;

  /* Calculate the timestamp */
#اگر_تعریف_شدہ DEBUGBUILD
  حرف *force_timestamp = getenv("CURL_FORCETIME");
  اگر(force_timestamp)
    time2filetime(&tw, (time_t)0);
  ورنہ
#اگر_ختم
    time2filetime(&tw, وقت(تہی));

  /* Calculate the response len */
  len = HMAC_MD5_لمبائی + NTLMv2_BLOB_LEN;

  /* Allocate the response */
  ptr = curlx_calloc(1, len);
  اگر(!ptr)
    واپس CURLE_برآمد_کا_حافظہ;

  /* Create the BLOB structure */
  curl_msnprintf((حرف *)ptr + HMAC_MD5_لمبائی, NTLMv2_BLOB_LEN,
                 "%c%c%c%c"           /* NTLMv2_BLOB_SIGNATURE */
                 "%c%c%c%c"           /* Reserved = 0 */
                 "%c%c%c%c%c%c%c%c",  /* Timestamp */
                 NTLMv2_BLOB_SIGNATURE[0], NTLMv2_BLOB_SIGNATURE[1],
                 NTLMv2_BLOB_SIGNATURE[2], NTLMv2_BLOB_SIGNATURE[3],
                 0, 0, 0, 0,
                 LONGQUARTET(tw.dw_کم_Date_وقت),
                 LONGQUARTET(tw.dw_بلند_Date_وقت));

  حافظہ_نقل(ptr + 32, challenge_client, 8);
  اگر(ntlm->ہدف_info_len)
    حافظہ_نقل(ptr + 44, ntlm->ہدف_info, ntlm->ہدف_info_len);

  /* Concatenate the Type 2 challenge with the BLOB and do HMAC MD5 */
  حافظہ_نقل(ptr + 8, &ntlm->nonce[0], 8);
  نتیجہ = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, HMAC_MD5_لمبائی, ptr + 8,
                       NTLMv2_BLOB_LEN + 8, hmac_output);
  اگر(نتیجہ) {
    curlx_free(ptr);
    واپس نتیجہ;
  }

  /* Concatenate the HMAC MD5 output  with the BLOB */
  حافظہ_نقل(ptr, hmac_output, HMAC_MD5_لمبائی);

  /* Return the response */
  *ntresp = ptr;
  *ntresp_len = len;

  واپس نتیجہ;
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
CURLcode Curl_ntlm_core_mk_lmv2_resp(ثابت غیرعلامتی حرف *ntlmv2hash,
                                     ثابت غیرعلامتی حرف *challenge_client,
                                     ثابت غیرعلامتی حرف *challenge_server,
                                     غیرعلامتی حرف *lmresp)
{
  غیرعلامتی حرف ڈیٹا[16];
  غیرعلامتی حرف hmac_output[16];
  CURLcode نتیجہ = CURLE_OK;

  حافظہ_نقل(&ڈیٹا[0], challenge_server, 8);
  حافظہ_نقل(&ڈیٹا[8], challenge_client, 8);

  نتیجہ = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, 16, &ڈیٹا[0], 16,
                       hmac_output);
  اگر(نتیجہ)
    واپس نتیجہ;

  /* Concatenate the HMAC MD5 output with the client nonce */
  حافظہ_نقل(lmresp, hmac_output, 16);
  حافظہ_نقل(lmresp + 16, challenge_client, 8);

  واپس نتیجہ;
}

#اگر_ختم /* !USE_WINDOWS_SSPI */

#اگر_ختم /* USE_CURL_NTLM_CORE */
