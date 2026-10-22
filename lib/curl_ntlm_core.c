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

#که_تعریف_شوی USE_CURL_NTLM_CORE

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

#که_تعریف_شوی USE_GNUTLS
#ضمن <nettle/version.h>
#إذا NETTLE_نسخه_MAJOR < 4
#تعریف_کړه USE_GNUTLS_DES
#د_که_پای
#د_که_پای

#إذا defined(USE_OPENSSL) && defined(HAVE_DES_ECB_ENCRYPT)

#  include <openssl/des.h>
#  ifdef OPENSSL_IS_AWSLC  /* for versions 1.2.0 to 1.30.1 */
#    define DES_سېټ_کلی_unchecked (فراغ)DES_سېټ_کلی
#  endif
#  define USE_OPENSSL_DES

#وإلاإذا defined(USE_WOLFSSL) && defined(HAVE_WC_DES_ECBENCRYPT)

#  include <wolfssl/options.h>
#  include <wolfssl/wolfcrypt/des3.h>
#  define USE_WOLFSSL_DES

#وإلاإذا defined(USE_GNUTLS_DES)
#  include <nettle/des.h>
#  define USE_CURL_DES_سېټ_ODD_PARITY
#وإلاإذا defined(USE_MBEDTLS) && defined(HAVE_MBEDTLS_DES_CRYPT_ECB)
#  include <mbedtls/version.h>
#  إذا MBEDTLS_نسخه_شمېره < 0x03020000
#  error "mbedTLS 3.2.0 or greater required"
#  endif
#  include <mbedtls/des.h>
#  define USE_MBEDTLS_DES
#وإلاإذا defined(USE_OS400CRYPTO)
#  include "cipher.mih"  /* mih/cipher */
#  define USE_CURL_DES_سېټ_ODD_PARITY
#وإلاإذا defined(USE_WIN32_CRYPTO)
#  include <wincrypt.h>
#  define USE_CURL_DES_سېټ_ODD_PARITY
#وإلا
#  error "cannot compile NTLM support without a crypto library with DES."
#د_که_پای

#ضمن "urldata.h"
#ضمن "strcase.h"
#ضمن "curl_ntlm_core.h"
#ضمن "curl_md5.h"
#ضمن "curl_hmac.h"
#ضمن "curl_md4.h"
#ضمن "vauth/vauth.h"

#که_تعریف_شوی USE_CURL_DES_سېټ_ODD_PARITY
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
ساكن فراغ curl_des_سېټ_odd_parity(بینښه حرف *بایټونه, size_t len)
{
  size_t i;

  لكل(i = 0; i < len; i++) {
    بینښه حرف b = بایټونه[i];

    bool needs_parity = (((b >> 7) ^ (b >> 6) ^ (b >> 5) ^
                          (b >> 4) ^ (b >> 3) ^ (b >> 2) ^
                          (b >> 1)) & 0x01) == 0;

    إذا(needs_parity)
      بایټونه[i] |= 0x01;
    وإلا
      بایټونه[i] &= 0xfe;
  }
}
#د_که_پای /* USE_CURL_DES_SET_ODD_PARITY */

/*
 * Turns a 56-bit key into being 64-bit wide.
 */
ساكن فراغ extend_کلی_56_منزل_64(ثابت بینښه حرف *کلی_56, حرف *کلی)
{
  کلی[0] = (حرف)کلی_56[0];
  کلی[1] = (حرف)(((کلی_56[0] << 7) & 0xFF) | (کلی_56[1] >> 1));
  کلی[2] = (حرف)(((کلی_56[1] << 6) & 0xFF) | (کلی_56[2] >> 2));
  کلی[3] = (حرف)(((کلی_56[2] << 5) & 0xFF) | (کلی_56[3] >> 3));
  کلی[4] = (حرف)(((کلی_56[3] << 4) & 0xFF) | (کلی_56[4] >> 4));
  کلی[5] = (حرف)(((کلی_56[4] << 3) & 0xFF) | (کلی_56[5] >> 5));
  کلی[6] = (حرف)(((کلی_56[5] << 2) & 0xFF) | (کلی_56[6] >> 6));
  کلی[7] = (حرف)((کلی_56[6] << 1) & 0xFF);
}

#که_تعریف_شوی USE_OPENSSL_DES
/*
 * Turns a 56-bit key into a 64-bit, odd parity key and sets the key. The
 * key schedule ks is also set.
 */
ساكن فراغ تیاری_des_کلی(ثابت بینښه حرف *کلی_56, DES_کلی_مهالوېش *ks)
{
  DES_cblock کلی;

  /* Expand the 56-bit key to 64 bits */
  extend_کلی_56_منزل_64(کلی_56, (حرف *)&کلی);

  /* Set the key parity to odd */
  DES_سېټ_odd_parity(&کلی);

  /* Set the key */
  DES_سېټ_کلی_unchecked(&کلی, ks);
}

#وإلاإذا defined(USE_WOLFSSL_DES)
ساكن فراغ تیاری_des_کلی(ثابت بینښه حرف *کلی_56, Des *des)
{
  byte کلی[8];

  /* Expand the 56-bit key to 64 bits */
  extend_کلی_56_منزل_64(کلی_56, (حرف *)کلی);

  /* Set the key */
  wc_Des_سېټ_کلی(des, کلی, عدم, 0);
}

#وإلاإذا defined(USE_GNUTLS_DES)
ساكن فراغ تیاری_des_کلی(ثابت بینښه حرف *کلی_56, بنية des_ctx *des)
{
  حرف کلی[8];

  /* Expand the 56-bit key to 64 bits */
  extend_کلی_56_منزل_64(کلی_56, کلی);

  /* Set the key parity to odd */
  curl_des_سېټ_odd_parity((بینښه حرف *)کلی, حجم(کلی));

  /* Set the key */
  des_سېټ_کلی(des, (ثابت uint8_t *)کلی);
}

#وإلاإذا defined(USE_MBEDTLS_DES)
ساكن bool encrypt_des(ثابت بینښه حرف *دننه, بینښه حرف *بهر,
                        ثابت بینښه حرف *کلی_56)
{
  mbedtls_des_context ctx;
  حرف کلی[8];

  /* Expand the 56-bit key to 64 bits */
  extend_کلی_56_منزل_64(کلی_56, کلی);

  /* Set the key parity to odd */
  mbedtls_des_کلی_سېټ_parity((بینښه حرف *)کلی);

  /* Perform the encryption */
  mbedtls_des_init(&ctx);
  mbedtls_des_setkey_enc(&ctx, (بینښه حرف *)کلی);
  أرجع mbedtls_des_crypt_ecb(&ctx, دننه, بهر) == 0;
}

#وإلاإذا defined(USE_OS400CRYPTO)
ساكن bool encrypt_des(ثابت بینښه حرف *دننه, بینښه حرف *بهر,
                        ثابت بینښه حرف *کلی_56)
{
  حرف کلی[8];
  _CIPHER_Control_T ctl;

  /* Setup the cipher control structure */
  ctl.Func_ID = ENCRYPT_ONLY;
  ctl.معلومات_Len = حجم(کلی);

  /* Expand the 56-bit key to 64 bits */
  extend_کلی_56_منزل_64(کلی_56, ctl.Crypto_کلی);

  /* Set the key parity to odd */
  curl_des_سېټ_odd_parity((بینښه حرف *)ctl.Crypto_کلی, ctl.معلومات_Len);

  /* Perform the encryption */
  _CIPHER((_SPCPTR *)&بهر, &ctl, (_SPCPTR *)&دننه);

  أرجع TRUE;
}

#وإلاإذا defined(USE_WIN32_CRYPTO)
ساكن bool encrypt_des(ثابت بینښه حرف *دننه, بینښه حرف *بهر,
                        ثابت بینښه حرف *کلی_56)
{
  HCRYPTPROV hprov;
  HCRYPTKEY hkey;
  بنية {
    BLOBHEADER hdr;
    بینښه صحيح len;
    حرف کلی[8];
  } blob;
  DWORD len = 8;
  BOOL res;

  /* Acquire the crypto provider */
  إذا(!CryptAcquireContext(&hprov, عدم, عدم, PROV_RSA_ډک,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    أرجع FALSE;

  /* Setup the key blob structure */
  memset(&blob, 0, حجم(blob));
  blob.hdr.bType = PLAINTEXTKEYBLOB;
  blob.hdr.b_نسخه = 2;
  blob.hdr.ai_کلی_Alg = CALG_DES;
  blob.len = حجم(blob.کلی);

  /* Expand the 56-bit key to 64 bits */
  extend_کلی_56_منزل_64(کلی_56, blob.کلی);

  /* Set the key parity to odd */
  curl_des_سېټ_odd_parity((بینښه حرف *)blob.کلی, حجم(blob.کلی));

  /* Import the key */
  إذا(!Crypt_Import_کلی(hprov, (BYTE *)&blob, حجم(blob), 0, 0, &hkey)) {
    CryptReleaseContext(hprov, 0);

    أرجع FALSE;
  }

  حافظه_نقل(بهر, دننه, 8);

  /* Perform the encryption */
  res = CryptEncrypt(hkey, 0, FALSE, 0, بهر, &len, len);

  Crypt_Destroy_کلی(hkey);
  CryptReleaseContext(hprov, 0);

  أرجع res;
}

#د_که_پای /* crypto backends */

/*
 * takes a 21-byte array and treats it as 3 56-bit DES keys. The
 * 8-byte plaintext is encrypted with each key and the resulting 24
 * bytes are stored in the results array.
 */
فراغ Curl_ntlm_core_lm_resp(ثابت بینښه حرف *کلیګانې,
                            ثابت بینښه حرف *plaintext,
                            بینښه حرف *results)
{
#که_تعریف_شوی USE_OPENSSL_DES
  DES_کلی_مهالوېش ks;

  تیاری_des_کلی(کلیګانې, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)results, &ks, DES_ENCRYPT);

  تیاری_des_کلی(کلیګانې + 7, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 8), &ks, DES_ENCRYPT);

  تیاری_des_کلی(کلیګانې + 14, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 16), &ks, DES_ENCRYPT);
#وإلاإذا defined(USE_WOLFSSL_DES)
  Des des;
  تیاری_des_کلی(کلیګانې, &des);
  wc_Des_EcbEncrypt(&des, results, plaintext, DES_کلی_اندازه);
  تیاری_des_کلی(کلیګانې + 7, &des);
  wc_Des_EcbEncrypt(&des, results + 8, plaintext, DES_کلی_اندازه);
  تیاری_des_کلی(کلیګانې + 14, &des);
  wc_Des_EcbEncrypt(&des, results + 16, plaintext, DES_کلی_اندازه);
#وإلاإذا defined(USE_GNUTLS_DES)
  بنية des_ctx des;
  تیاری_des_کلی(کلیګانې, &des);
  des_encrypt(&des, 8, results, plaintext);
  تیاری_des_کلی(کلیګانې + 7, &des);
  des_encrypt(&des, 8, results + 8, plaintext);
  تیاری_des_کلی(کلیګانې + 14, &des);
  des_encrypt(&des, 8, results + 16, plaintext);
#وإلاإذا defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
  encrypt_des(plaintext, results, کلیګانې);
  encrypt_des(plaintext, results + 8, کلیګانې + 7);
  encrypt_des(plaintext, results + 16, کلیګانې + 14);
#وإلا
  (فراغ)کلیګانې;
  (فراغ)plaintext;
  (فراغ)results;
#د_که_پای
}

/*
 * Set up lanmanager hashed password
 */
CURLcode Curl_ntlm_core_mk_lm_hash(ثابت حرف *password,
                                   بینښه حرف *lmbuffer /* 21 bytes */)
{
  بینښه حرف pw[14];
  ساكن ثابت بینښه حرف magic[] = {
    0x4B, 0x47, 0x53, 0x21, 0x40, 0x23, 0x24, 0x25 /* i.e. KGS!@#$% */
  };
  size_t len = CURLMIN(مزي_اوږدوالی(password), 14);

  Curl_strntoupper((حرف *)pw, password, len);
  memset(&pw[len], 0, 14 - len);

  {
    /* Create LanManager hashed password. */
#که_تعریف_شوی USE_OPENSSL_DES
    DES_کلی_مهالوېش ks;

    تیاری_des_کلی(pw, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)lmbuffer, &ks, DES_ENCRYPT);

    تیاری_des_کلی(pw + 7, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)(lmbuffer + 8), &ks, DES_ENCRYPT);
#وإلاإذا defined(USE_WOLFSSL_DES)
    Des des;
    تیاری_des_کلی(pw, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer, magic, DES_کلی_اندازه);
    تیاری_des_کلی(pw + 7, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer + 8, magic, DES_کلی_اندازه);
#وإلاإذا defined(USE_GNUTLS_DES)
    بنية des_ctx des;
    تیاری_des_کلی(pw, &des);
    des_encrypt(&des, 8, lmbuffer, magic);
    تیاری_des_کلی(pw + 7, &des);
    des_encrypt(&des, 8, lmbuffer + 8, magic);
#وإلاإذا defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
    encrypt_des(magic, lmbuffer, pw);
    encrypt_des(magic, lmbuffer + 8, pw + 7);
#د_که_پای

    memset(lmbuffer + 16, 0, 21 - 16);
  }

  أرجع CURLE_OK;
}

ساكن فراغ ascii_منزل_unicode_le(بینښه حرف *dest, ثابت حرف *src,
                                size_t srclen)
{
  size_t i;
  لكل(i = 0; i < srclen; i++) {
    dest[2 * i] = (بینښه حرف)src[i];
    dest[(2 * i) + 1] = '\0';
  }
}

/*
 * Set up nt hashed passwords
 * @unittest: 1600
 */
CURLcode Curl_ntlm_core_mk_nt_hash(ثابت حرف *password,
                                   بینښه حرف *ntbuffer /* 21 bytes */)
{
  size_t len = مزي_اوږدوالی(password);
  بینښه حرف *pw;
  CURLcode پایله;
  إذا(len > SIZE_MAX / 2) /* avoid integer overflow */
    أرجع CURLE_بهر_د_حافظه;
  pw = len ? curlx_malloc(len * 2) : (بینښه حرف *)curlx_strdup("");
  إذا(!pw)
    أرجع CURLE_بهر_د_حافظه;

  ascii_منزل_unicode_le(pw, password, len);

  /* Create NT hashed password. */
  پایله = Curl_md4it(ntbuffer, pw, 2 * len);
  إذا(!پایله)
    memset(ntbuffer + 16, 0, 21 - 16);

  curlx_free(pw);

  أرجع پایله;
}

#که_نه_تعریف_شوی USE_WINDOWS_SSPI

#تعریف_کړه NTLMv2_BLOB_SIGNATURE "\x01\x01\x00\x00"
#تعریف_کړه NTLMv2_BLOB_LEN       (44 - 16 + ntlm->موخه_info_len + 4)

/* Timestamp in tenths of a microsecond since January 1, 1601 00:00:00 UTC. */
بنية ms_filetime {
  بینښه صحيح dw_ټیټ_Date_وخت;
  بینښه صحيح dw_لوړ_Date_وخت;
};

/* Convert a time_t to an MS FILETIME (MS-DTYP section 2.3.3). */
ساكن فراغ time2filetime(بنية ms_filetime *ft, time_t t)
{
#إذا SIZEOF_وخت_T > 4
  t = (t + (curl_off_t)11644473600) * 10000000;
  ft->dw_ټیټ_Date_وخت = (بینښه صحيح)(t & 0xFFFFFFFF);
  ft->dw_لوړ_Date_وخت = (بینښه صحيح)(t >> 32);
#وإلا
  بینښه صحيح r, s;
  بینښه صحيح i;

  ft->dw_ټیټ_Date_وخت = (بینښه صحيح)(t & 0xFFFFFFFF);
  ft->dw_لوړ_Date_وخت = 0;

#که_نه_تعریف_شوی HAVE_وخت_T_UNSIGNED
  /* Extend sign if needed. */
  إذا(ft->dw_ټیټ_Date_وخت & 0x80000000)
    ft->dw_لوړ_Date_وخت = ~(بینښه صحيح)0;
#د_که_پای

  /* Bias seconds to Jan 1, 1601.
     134774 days = 11644473600 seconds = 0x2B6109100 */
  r = ft->dw_ټیټ_Date_وخت;
  ft->dw_ټیټ_Date_وخت = (ft->dw_ټیټ_Date_وخت + 0xB6109100U) & 0xFFFFFFFF;
  ft->dw_لوړ_Date_وخت += ft->dw_ټیټ_Date_وخت < r ? 0x03 : 0x02;

  /* Convert to tenths of microseconds. */
  ft->dw_لوړ_Date_وخت *= 10000000;
  i = 32;
  افعل {
    i -= 8;
    s = ((ft->dw_ټیټ_Date_وخت >> i) & 0xFF) * (10000000 - 1);
    r = (s << i) & 0xFFFFFFFF;
    s >>= 1;   /* Split shift to avoid width overflow. */
    s >>= 31 - i;
    ft->dw_ټیټ_Date_وخت = (ft->dw_ټیټ_Date_وخت + r) & 0xFFFFFFFF;
    إذا(ft->dw_ټیټ_Date_وخت < r)
      s++;
    ft->dw_لوړ_Date_وخت += s;
  } طالما(i);
  ft->dw_لوړ_Date_وخت &= 0xFFFFFFFF;
#د_که_پای
}

ساكن فراغ ascii_uppercase_منزل_unicode_le(بینښه حرف *dest,
                                          ثابت حرف *src, size_t srclen)
{
  size_t i;
  لكل(i = 0; i < srclen; i++) {
    dest[2 * i] = (بینښه حرف)(Curl_raw_toupper(src[i]));
    dest[(2 * i) + 1] = '\0';
  }
}

/* This creates the NTLMv2 hash by using NTLM hash as the key and Unicode
 * (uppercase UserName + Domain) as the data
 */
CURLcode Curl_ntlm_core_mk_ntlmv2_hash(ثابت حرف *user, size_t userlen,
                                       ثابت حرف *domain, size_t domlen,
                                       بینښه حرف *ntlmhash,
                                       بینښه حرف *ntlmv2hash)
{
  /* Unicode representation */
  size_t بې_اغېزه_عنصر_len;
  بینښه حرف *بې_اغېزه_عنصر;
  CURLcode پایله = CURLE_OK;

  إذا((userlen > CURL_MAX_ننوت_اوږدتیا) || (domlen > CURL_MAX_ننوت_اوږدتیا))
    أرجع CURLE_بهر_د_حافظه;

  بې_اغېزه_عنصر_len = (userlen + domlen) * 2;
  بې_اغېزه_عنصر = curlx_malloc(بې_اغېزه_عنصر_len + 1);

  إذا(!بې_اغېزه_عنصر)
    أرجع CURLE_بهر_د_حافظه;

  ascii_uppercase_منزل_unicode_le(بې_اغېزه_عنصر, user, userlen);
  ascii_منزل_unicode_le(بې_اغېزه_عنصر + (userlen << 1), domain, domlen);

  پایله = Curl_hmacit(&Curl_HMAC_MD5, ntlmhash, 16, بې_اغېزه_عنصر, بې_اغېزه_عنصر_len,
                       ntlmv2hash);
  curlx_free(بې_اغېزه_عنصر);

  أرجع پایله;
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
CURLcode Curl_ntlm_core_mk_ntlmv2_resp(ثابت بینښه حرف *ntlmv2hash,
                                       ثابت بینښه حرف *challenge_client,
                                       ثابت بنية ntlmdata *ntlm,
                                       بینښه حرف **ntresp,
                                       بینښه صحيح *ntresp_len)
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

  بینښه صحيح len = 0;
  بینښه حرف *ptr = عدم;
  بینښه حرف hmac_output[HMAC_MD5_اوږدتیا];
  بنية ms_filetime tw;

  CURLcode پایله = CURLE_OK;

  /* Calculate the timestamp */
#که_تعریف_شوی DEBUGBUILD
  حرف *force_timestamp = getenv("CURL_FORCETIME");
  إذا(force_timestamp)
    time2filetime(&tw, (time_t)0);
  وإلا
#د_که_پای
    time2filetime(&tw, وخت(عدم));

  /* Calculate the response len */
  len = HMAC_MD5_اوږدتیا + NTLMv2_BLOB_LEN;

  /* Allocate the response */
  ptr = curlx_calloc(1, len);
  إذا(!ptr)
    أرجع CURLE_بهر_د_حافظه;

  /* Create the BLOB structure */
  curl_msnprintf((حرف *)ptr + HMAC_MD5_اوږدتیا, NTLMv2_BLOB_LEN,
                 "%c%c%c%c"           /* NTLMv2_BLOB_SIGNATURE */
                 "%c%c%c%c"           /* Reserved = 0 */
                 "%c%c%c%c%c%c%c%c",  /* Timestamp */
                 NTLMv2_BLOB_SIGNATURE[0], NTLMv2_BLOB_SIGNATURE[1],
                 NTLMv2_BLOB_SIGNATURE[2], NTLMv2_BLOB_SIGNATURE[3],
                 0, 0, 0, 0,
                 LONGQUARTET(tw.dw_ټیټ_Date_وخت),
                 LONGQUARTET(tw.dw_لوړ_Date_وخت));

  حافظه_نقل(ptr + 32, challenge_client, 8);
  إذا(ntlm->موخه_info_len)
    حافظه_نقل(ptr + 44, ntlm->موخه_info, ntlm->موخه_info_len);

  /* Concatenate the Type 2 challenge with the BLOB and do HMAC MD5 */
  حافظه_نقل(ptr + 8, &ntlm->nonce[0], 8);
  پایله = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, HMAC_MD5_اوږدتیا, ptr + 8,
                       NTLMv2_BLOB_LEN + 8, hmac_output);
  إذا(پایله) {
    curlx_free(ptr);
    أرجع پایله;
  }

  /* Concatenate the HMAC MD5 output  with the BLOB */
  حافظه_نقل(ptr, hmac_output, HMAC_MD5_اوږدتیا);

  /* Return the response */
  *ntresp = ptr;
  *ntresp_len = len;

  أرجع پایله;
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
CURLcode Curl_ntlm_core_mk_lmv2_resp(ثابت بینښه حرف *ntlmv2hash,
                                     ثابت بینښه حرف *challenge_client,
                                     ثابت بینښه حرف *challenge_server,
                                     بینښه حرف *lmresp)
{
  بینښه حرف معلومات[16];
  بینښه حرف hmac_output[16];
  CURLcode پایله = CURLE_OK;

  حافظه_نقل(&معلومات[0], challenge_server, 8);
  حافظه_نقل(&معلومات[8], challenge_client, 8);

  پایله = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, 16, &معلومات[0], 16,
                       hmac_output);
  إذا(پایله)
    أرجع پایله;

  /* Concatenate the HMAC MD5 output with the client nonce */
  حافظه_نقل(lmresp, hmac_output, 16);
  حافظه_نقل(lmresp + 16, challenge_client, 8);

  أرجع پایله;
}

#د_که_پای /* !USE_WINDOWS_SSPI */

#د_که_پای /* USE_CURL_NTLM_CORE */
