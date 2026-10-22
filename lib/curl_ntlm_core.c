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

#جے_تعریف_شدہ USE_CURL_NTLM_CORE

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

#جے_تعریف_شدہ USE_GNUTLS
#ضمن <nettle/version.h>
#إذا NETTLE_نسخہ_MAJOR < 4
#تعریف_کرو USE_GNUTLS_DES
#جے_انت
#جے_انت

#إذا defined(USE_OPENSSL) && defined(HAVE_DES_ECB_ENCRYPT)

#  include <openssl/des.h>
#  ifdef OPENSSL_IS_AWSLC  /* for versions 1.2.0 to 1.30.1 */
#    define DES_سیٹ_کلید_unchecked (فراغ)DES_سیٹ_کلید
#  endif
#  define USE_OPENSSL_DES

#وإلاإذا defined(USE_WOLFSSL) && defined(HAVE_WC_DES_ECBENCRYPT)

#  include <wolfssl/options.h>
#  include <wolfssl/wolfcrypt/des3.h>
#  define USE_WOLFSSL_DES

#وإلاإذا defined(USE_GNUTLS_DES)
#  include <nettle/des.h>
#  define USE_CURL_DES_سیٹ_ODD_PARITY
#وإلاإذا defined(USE_MBEDTLS) && defined(HAVE_MBEDTLS_DES_CRYPT_ECB)
#  include <mbedtls/version.h>
#  إذا MBEDTLS_نسخہ_نمبر < 0x03020000
#  error "mbedTLS 3.2.0 or greater required"
#  endif
#  include <mbedtls/des.h>
#  define USE_MBEDTLS_DES
#وإلاإذا defined(USE_OS400CRYPTO)
#  include "cipher.mih"  /* mih/cipher */
#  define USE_CURL_DES_سیٹ_ODD_PARITY
#وإلاإذا defined(USE_WIN32_CRYPTO)
#  include <wincrypt.h>
#  define USE_CURL_DES_سیٹ_ODD_PARITY
#وإلا
#  error "cannot compile NTLM support without a crypto library with DES."
#جے_انت

#ضمن "urldata.h"
#ضمن "strcase.h"
#ضمن "curl_ntlm_core.h"
#ضمن "curl_md5.h"
#ضمن "curl_hmac.h"
#ضمن "curl_md4.h"
#ضمن "vauth/vauth.h"

#جے_تعریف_شدہ USE_CURL_DES_سیٹ_ODD_PARITY
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
ساكن فراغ curl_des_سیٹ_odd_parity(بنانشانی حرف *بائٹاں, size_t len)
{
  size_t i;

  لكل(i = 0; i < len; i++) {
    بنانشانی حرف b = بائٹاں[i];

    bool needs_parity = (((b >> 7) ^ (b >> 6) ^ (b >> 5) ^
                          (b >> 4) ^ (b >> 3) ^ (b >> 2) ^
                          (b >> 1)) & 0x01) == 0;

    إذا(needs_parity)
      بائٹاں[i] |= 0x01;
    وإلا
      بائٹاں[i] &= 0xfe;
  }
}
#جے_انت /* USE_CURL_DES_SET_ODD_PARITY */

/*
 * Turns a 56-bit key into being 64-bit wide.
 */
ساكن فراغ extend_کلید_56_منزل_64(ثابت بنانشانی حرف *کلید_56, حرف *کلید)
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

#جے_تعریف_شدہ USE_OPENSSL_DES
/*
 * Turns a 56-bit key into a 64-bit, odd parity key and sets the key. The
 * key schedule ks is also set.
 */
ساكن فراغ تیاری_des_کلید(ثابت بنانشانی حرف *کلید_56, DES_کلید_ویلا_سارنی *ks)
{
  DES_cblock کلید;

  /* Expand the 56-bit key to 64 bits */
  extend_کلید_56_منزل_64(کلید_56, (حرف *)&کلید);

  /* Set the key parity to odd */
  DES_سیٹ_odd_parity(&کلید);

  /* Set the key */
  DES_سیٹ_کلید_unchecked(&کلید, ks);
}

#وإلاإذا defined(USE_WOLFSSL_DES)
ساكن فراغ تیاری_des_کلید(ثابت بنانشانی حرف *کلید_56, Des *des)
{
  byte کلید[8];

  /* Expand the 56-bit key to 64 bits */
  extend_کلید_56_منزل_64(کلید_56, (حرف *)کلید);

  /* Set the key */
  wc_Des_سیٹ_کلید(des, کلید, عدم, 0);
}

#وإلاإذا defined(USE_GNUTLS_DES)
ساكن فراغ تیاری_des_کلید(ثابت بنانشانی حرف *کلید_56, بنية des_ctx *des)
{
  حرف کلید[8];

  /* Expand the 56-bit key to 64 bits */
  extend_کلید_56_منزل_64(کلید_56, کلید);

  /* Set the key parity to odd */
  curl_des_سیٹ_odd_parity((بنانشانی حرف *)کلید, حجم(کلید));

  /* Set the key */
  des_سیٹ_کلید(des, (ثابت uint8_t *)کلید);
}

#وإلاإذا defined(USE_MBEDTLS_DES)
ساكن bool encrypt_des(ثابت بنانشانی حرف *وچ, بنانشانی حرف *باہرلا,
                        ثابت بنانشانی حرف *کلید_56)
{
  mbedtls_des_context ctx;
  حرف کلید[8];

  /* Expand the 56-bit key to 64 bits */
  extend_کلید_56_منزل_64(کلید_56, کلید);

  /* Set the key parity to odd */
  mbedtls_des_کلید_سیٹ_parity((بنانشانی حرف *)کلید);

  /* Perform the encryption */
  mbedtls_des_init(&ctx);
  mbedtls_des_setkey_enc(&ctx, (بنانشانی حرف *)کلید);
  أرجع mbedtls_des_crypt_ecb(&ctx, وچ, باہرلا) == 0;
}

#وإلاإذا defined(USE_OS400CRYPTO)
ساكن bool encrypt_des(ثابت بنانشانی حرف *وچ, بنانشانی حرف *باہرلا,
                        ثابت بنانشانی حرف *کلید_56)
{
  حرف کلید[8];
  _CIPHER_Control_T ctl;

  /* Setup the cipher control structure */
  ctl.Func_ID = ENCRYPT_ONLY;
  ctl.انکڑے_Len = حجم(کلید);

  /* Expand the 56-bit key to 64 bits */
  extend_کلید_56_منزل_64(کلید_56, ctl.Crypto_کلید);

  /* Set the key parity to odd */
  curl_des_سیٹ_odd_parity((بنانشانی حرف *)ctl.Crypto_کلید, ctl.انکڑے_Len);

  /* Perform the encryption */
  _CIPHER((_SPCPTR *)&باہرلا, &ctl, (_SPCPTR *)&وچ);

  أرجع TRUE;
}

#وإلاإذا defined(USE_WIN32_CRYPTO)
ساكن bool encrypt_des(ثابت بنانشانی حرف *وچ, بنانشانی حرف *باہرلا,
                        ثابت بنانشانی حرف *کلید_56)
{
  HCRYPTPROV hprov;
  HCRYPTKEY hkey;
  بنية {
    BLOBHEADER hdr;
    بنانشانی صحيح len;
    حرف کلید[8];
  } blob;
  DWORD len = 8;
  BOOL res;

  /* Acquire the crypto provider */
  إذا(!CryptAcquireContext(&hprov, عدم, عدم, PROV_RSA_بھریا,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    أرجع FALSE;

  /* Setup the key blob structure */
  memset(&blob, 0, حجم(blob));
  blob.hdr.bType = PLAINTEXTKEYBLOB;
  blob.hdr.b_نسخہ = 2;
  blob.hdr.ai_کلید_Alg = CALG_DES;
  blob.len = حجم(blob.کلید);

  /* Expand the 56-bit key to 64 bits */
  extend_کلید_56_منزل_64(کلید_56, blob.کلید);

  /* Set the key parity to odd */
  curl_des_سیٹ_odd_parity((بنانشانی حرف *)blob.کلید, حجم(blob.کلید));

  /* Import the key */
  إذا(!Crypt_Import_کلید(hprov, (BYTE *)&blob, حجم(blob), 0, 0, &hkey)) {
    CryptReleaseContext(hprov, 0);

    أرجع FALSE;
  }

  حافظہ_نقل(باہرلا, وچ, 8);

  /* Perform the encryption */
  res = CryptEncrypt(hkey, 0, FALSE, 0, باہرلا, &len, len);

  Crypt_Destroy_کلید(hkey);
  CryptReleaseContext(hprov, 0);

  أرجع res;
}

#جے_انت /* crypto backends */

/*
 * takes a 21-byte array and treats it as 3 56-bit DES keys. The
 * 8-byte plaintext is encrypted with each key and the resulting 24
 * bytes are stored in the results array.
 */
فراغ Curl_ntlm_core_lm_resp(ثابت بنانشانی حرف *کلیدیں,
                            ثابت بنانشانی حرف *plaintext,
                            بنانشانی حرف *results)
{
#جے_تعریف_شدہ USE_OPENSSL_DES
  DES_کلید_ویلا_سارنی ks;

  تیاری_des_کلید(کلیدیں, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)results, &ks, DES_ENCRYPT);

  تیاری_des_کلید(کلیدیں + 7, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 8), &ks, DES_ENCRYPT);

  تیاری_des_کلید(کلیدیں + 14, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 16), &ks, DES_ENCRYPT);
#وإلاإذا defined(USE_WOLFSSL_DES)
  Des des;
  تیاری_des_کلید(کلیدیں, &des);
  wc_Des_EcbEncrypt(&des, results, plaintext, DES_کلید_ناپ);
  تیاری_des_کلید(کلیدیں + 7, &des);
  wc_Des_EcbEncrypt(&des, results + 8, plaintext, DES_کلید_ناپ);
  تیاری_des_کلید(کلیدیں + 14, &des);
  wc_Des_EcbEncrypt(&des, results + 16, plaintext, DES_کلید_ناپ);
#وإلاإذا defined(USE_GNUTLS_DES)
  بنية des_ctx des;
  تیاری_des_کلید(کلیدیں, &des);
  des_encrypt(&des, 8, results, plaintext);
  تیاری_des_کلید(کلیدیں + 7, &des);
  des_encrypt(&des, 8, results + 8, plaintext);
  تیاری_des_کلید(کلیدیں + 14, &des);
  des_encrypt(&des, 8, results + 16, plaintext);
#وإلاإذا defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
  encrypt_des(plaintext, results, کلیدیں);
  encrypt_des(plaintext, results + 8, کلیدیں + 7);
  encrypt_des(plaintext, results + 16, کلیدیں + 14);
#وإلا
  (فراغ)کلیدیں;
  (فراغ)plaintext;
  (فراغ)results;
#جے_انت
}

/*
 * Set up lanmanager hashed password
 */
CURLcode Curl_ntlm_core_mk_lm_hash(ثابت حرف *password,
                                   بنانشانی حرف *lmbuffer /* 21 bytes */)
{
  بنانشانی حرف pw[14];
  ساكن ثابت بنانشانی حرف magic[] = {
    0x4B, 0x47, 0x53, 0x21, 0x40, 0x23, 0x24, 0x25 /* i.e. KGS!@#$% */
  };
  size_t len = CURLMIN(لڑی_لمبائی(password), 14);

  Curl_strntoupper((حرف *)pw, password, len);
  memset(&pw[len], 0, 14 - len);

  {
    /* Create LanManager hashed password. */
#جے_تعریف_شدہ USE_OPENSSL_DES
    DES_کلید_ویلا_سارنی ks;

    تیاری_des_کلید(pw, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)lmbuffer, &ks, DES_ENCRYPT);

    تیاری_des_کلید(pw + 7, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)(lmbuffer + 8), &ks, DES_ENCRYPT);
#وإلاإذا defined(USE_WOLFSSL_DES)
    Des des;
    تیاری_des_کلید(pw, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer, magic, DES_کلید_ناپ);
    تیاری_des_کلید(pw + 7, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer + 8, magic, DES_کلید_ناپ);
#وإلاإذا defined(USE_GNUTLS_DES)
    بنية des_ctx des;
    تیاری_des_کلید(pw, &des);
    des_encrypt(&des, 8, lmbuffer, magic);
    تیاری_des_کلید(pw + 7, &des);
    des_encrypt(&des, 8, lmbuffer + 8, magic);
#وإلاإذا defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
    encrypt_des(magic, lmbuffer, pw);
    encrypt_des(magic, lmbuffer + 8, pw + 7);
#جے_انت

    memset(lmbuffer + 16, 0, 21 - 16);
  }

  أرجع CURLE_OK;
}

ساكن فراغ ascii_منزل_unicode_le(بنانشانی حرف *dest, ثابت حرف *src,
                                size_t srclen)
{
  size_t i;
  لكل(i = 0; i < srclen; i++) {
    dest[2 * i] = (بنانشانی حرف)src[i];
    dest[(2 * i) + 1] = '\0';
  }
}

/*
 * Set up nt hashed passwords
 * @unittest: 1600
 */
CURLcode Curl_ntlm_core_mk_nt_hash(ثابت حرف *password,
                                   بنانشانی حرف *ntbuffer /* 21 bytes */)
{
  size_t len = لڑی_لمبائی(password);
  بنانشانی حرف *pw;
  CURLcode نتیجہ;
  إذا(len > SIZE_MAX / 2) /* avoid integer overflow */
    أرجع CURLE_باہرلا_دا_حافظہ;
  pw = len ? curlx_malloc(len * 2) : (بنانشانی حرف *)curlx_strdup("");
  إذا(!pw)
    أرجع CURLE_باہرلا_دا_حافظہ;

  ascii_منزل_unicode_le(pw, password, len);

  /* Create NT hashed password. */
  نتیجہ = Curl_md4it(ntbuffer, pw, 2 * len);
  إذا(!نتیجہ)
    memset(ntbuffer + 16, 0, 21 - 16);

  curlx_free(pw);

  أرجع نتیجہ;
}

#جے_بنا_تعریف USE_WINDOWS_SSPI

#تعریف_کرو NTLMv2_BLOB_SIGNATURE "\x01\x01\x00\x00"
#تعریف_کرو NTLMv2_BLOB_LEN       (44 - 16 + ntlm->نشانہ_info_len + 4)

/* Timestamp in tenths of a microsecond since January 1, 1601 00:00:00 UTC. */
بنية ms_filetime {
  بنانشانی صحيح dw_کم_Date_ویلا;
  بنانشانی صحيح dw_بلند_Date_ویلا;
};

/* Convert a time_t to an MS FILETIME (MS-DTYP section 2.3.3). */
ساكن فراغ time2filetime(بنية ms_filetime *ft, time_t t)
{
#إذا SIZEOF_ویلا_T > 4
  t = (t + (curl_off_t)11644473600) * 10000000;
  ft->dw_کم_Date_ویلا = (بنانشانی صحيح)(t & 0xFFFFFFFF);
  ft->dw_بلند_Date_ویلا = (بنانشانی صحيح)(t >> 32);
#وإلا
  بنانشانی صحيح r, s;
  بنانشانی صحيح i;

  ft->dw_کم_Date_ویلا = (بنانشانی صحيح)(t & 0xFFFFFFFF);
  ft->dw_بلند_Date_ویلا = 0;

#جے_بنا_تعریف HAVE_ویلا_T_UNSIGNED
  /* Extend sign if needed. */
  إذا(ft->dw_کم_Date_ویلا & 0x80000000)
    ft->dw_بلند_Date_ویلا = ~(بنانشانی صحيح)0;
#جے_انت

  /* Bias seconds to Jan 1, 1601.
     134774 days = 11644473600 seconds = 0x2B6109100 */
  r = ft->dw_کم_Date_ویلا;
  ft->dw_کم_Date_ویلا = (ft->dw_کم_Date_ویلا + 0xB6109100U) & 0xFFFFFFFF;
  ft->dw_بلند_Date_ویلا += ft->dw_کم_Date_ویلا < r ? 0x03 : 0x02;

  /* Convert to tenths of microseconds. */
  ft->dw_بلند_Date_ویلا *= 10000000;
  i = 32;
  افعل {
    i -= 8;
    s = ((ft->dw_کم_Date_ویلا >> i) & 0xFF) * (10000000 - 1);
    r = (s << i) & 0xFFFFFFFF;
    s >>= 1;   /* Split shift to avoid width overflow. */
    s >>= 31 - i;
    ft->dw_کم_Date_ویلا = (ft->dw_کم_Date_ویلا + r) & 0xFFFFFFFF;
    إذا(ft->dw_کم_Date_ویلا < r)
      s++;
    ft->dw_بلند_Date_ویلا += s;
  } طالما(i);
  ft->dw_بلند_Date_ویلا &= 0xFFFFFFFF;
#جے_انت
}

ساكن فراغ ascii_uppercase_منزل_unicode_le(بنانشانی حرف *dest,
                                          ثابت حرف *src, size_t srclen)
{
  size_t i;
  لكل(i = 0; i < srclen; i++) {
    dest[2 * i] = (بنانشانی حرف)(Curl_raw_toupper(src[i]));
    dest[(2 * i) + 1] = '\0';
  }
}

/* This creates the NTLMv2 hash by using NTLM hash as the key and Unicode
 * (uppercase UserName + Domain) as the data
 */
CURLcode Curl_ntlm_core_mk_ntlmv2_hash(ثابت حرف *user, size_t userlen,
                                       ثابت حرف *domain, size_t domlen,
                                       بنانشانی حرف *ntlmhash,
                                       بنانشانی حرف *ntlmv2hash)
{
  /* Unicode representation */
  size_t پچھان_تت_len;
  بنانشانی حرف *پچھان_تت;
  CURLcode نتیجہ = CURLE_OK;

  إذا((userlen > CURL_MAX_داخلہ_لمائی) || (domlen > CURL_MAX_داخلہ_لمائی))
    أرجع CURLE_باہرلا_دا_حافظہ;

  پچھان_تت_len = (userlen + domlen) * 2;
  پچھان_تت = curlx_malloc(پچھان_تت_len + 1);

  إذا(!پچھان_تت)
    أرجع CURLE_باہرلا_دا_حافظہ;

  ascii_uppercase_منزل_unicode_le(پچھان_تت, user, userlen);
  ascii_منزل_unicode_le(پچھان_تت + (userlen << 1), domain, domlen);

  نتیجہ = Curl_hmacit(&Curl_HMAC_MD5, ntlmhash, 16, پچھان_تت, پچھان_تت_len,
                       ntlmv2hash);
  curlx_free(پچھان_تت);

  أرجع نتیجہ;
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
CURLcode Curl_ntlm_core_mk_ntlmv2_resp(ثابت بنانشانی حرف *ntlmv2hash,
                                       ثابت بنانشانی حرف *challenge_client,
                                       ثابت بنية ntlmdata *ntlm,
                                       بنانشانی حرف **ntresp,
                                       بنانشانی صحيح *ntresp_len)
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

  بنانشانی صحيح len = 0;
  بنانشانی حرف *ptr = عدم;
  بنانشانی حرف hmac_output[HMAC_MD5_لمائی];
  بنية ms_filetime tw;

  CURLcode نتیجہ = CURLE_OK;

  /* Calculate the timestamp */
#جے_تعریف_شدہ DEBUGBUILD
  حرف *force_timestamp = getenv("CURL_FORCETIME");
  إذا(force_timestamp)
    time2filetime(&tw, (time_t)0);
  وإلا
#جے_انت
    time2filetime(&tw, ویلا(عدم));

  /* Calculate the response len */
  len = HMAC_MD5_لمائی + NTLMv2_BLOB_LEN;

  /* Allocate the response */
  ptr = curlx_calloc(1, len);
  إذا(!ptr)
    أرجع CURLE_باہرلا_دا_حافظہ;

  /* Create the BLOB structure */
  curl_msnprintf((حرف *)ptr + HMAC_MD5_لمائی, NTLMv2_BLOB_LEN,
                 "%c%c%c%c"           /* NTLMv2_BLOB_SIGNATURE */
                 "%c%c%c%c"           /* Reserved = 0 */
                 "%c%c%c%c%c%c%c%c",  /* Timestamp */
                 NTLMv2_BLOB_SIGNATURE[0], NTLMv2_BLOB_SIGNATURE[1],
                 NTLMv2_BLOB_SIGNATURE[2], NTLMv2_BLOB_SIGNATURE[3],
                 0, 0, 0, 0,
                 LONGQUARTET(tw.dw_کم_Date_ویلا),
                 LONGQUARTET(tw.dw_بلند_Date_ویلا));

  حافظہ_نقل(ptr + 32, challenge_client, 8);
  إذا(ntlm->نشانہ_info_len)
    حافظہ_نقل(ptr + 44, ntlm->نشانہ_info, ntlm->نشانہ_info_len);

  /* Concatenate the Type 2 challenge with the BLOB and do HMAC MD5 */
  حافظہ_نقل(ptr + 8, &ntlm->nonce[0], 8);
  نتیجہ = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, HMAC_MD5_لمائی, ptr + 8,
                       NTLMv2_BLOB_LEN + 8, hmac_output);
  إذا(نتیجہ) {
    curlx_free(ptr);
    أرجع نتیجہ;
  }

  /* Concatenate the HMAC MD5 output  with the BLOB */
  حافظہ_نقل(ptr, hmac_output, HMAC_MD5_لمائی);

  /* Return the response */
  *ntresp = ptr;
  *ntresp_len = len;

  أرجع نتیجہ;
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
CURLcode Curl_ntlm_core_mk_lmv2_resp(ثابت بنانشانی حرف *ntlmv2hash,
                                     ثابت بنانشانی حرف *challenge_client,
                                     ثابت بنانشانی حرف *challenge_server,
                                     بنانشانی حرف *lmresp)
{
  بنانشانی حرف انکڑے[16];
  بنانشانی حرف hmac_output[16];
  CURLcode نتیجہ = CURLE_OK;

  حافظہ_نقل(&انکڑے[0], challenge_server, 8);
  حافظہ_نقل(&انکڑے[8], challenge_client, 8);

  نتیجہ = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, 16, &انکڑے[0], 16,
                       hmac_output);
  إذا(نتیجہ)
    أرجع نتیجہ;

  /* Concatenate the HMAC MD5 output with the client nonce */
  حافظہ_نقل(lmresp, hmac_output, 16);
  حافظہ_نقل(lmresp + 16, challenge_client, 8);

  أرجع نتیجہ;
}

#جے_انت /* !USE_WINDOWS_SSPI */

#جے_انت /* USE_CURL_NTLM_CORE */
