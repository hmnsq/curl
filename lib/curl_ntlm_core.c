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
#अन्तर्भाव "curl_setup.h"

#यदि_परिभाषितम् USE_CURL_NTLM_CORE

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

#यदि_परिभाषितम् USE_GNUTLS
#अन्तर्भाव <nettle/version.h>
#यदि NETTLE_संस्करणम्_MAJOR < 4
#परिभाषय USE_GNUTLS_DES
#यदि_अन्तः
#यदि_अन्तः

#यदि defined(USE_OPENSSL) && defined(HAVE_DES_ECB_ENCRYPT)

#  include <openssl/des.h>
#  ifdef OPENSSL_IS_AWSLC  /* for versions 1.2.0 to 1.30.1 */
#    define DES_समुच्चयः_कुञ्चिका_unchecked (शून्य)DES_समुच्चयः_कुञ्चिका
#  endif
#  define USE_OPENSSL_DES

#नोचेद्यदि defined(USE_WOLFSSL) && defined(HAVE_WC_DES_ECBENCRYPT)

#  include <wolfssl/options.h>
#  include <wolfssl/wolfcrypt/des3.h>
#  define USE_WOLFSSL_DES

#नोचेद्यदि defined(USE_GNUTLS_DES)
#  include <nettle/des.h>
#  define USE_CURL_DES_समुच्चयः_ODD_PARITY
#नोचेद्यदि defined(USE_MBEDTLS) && defined(HAVE_MBEDTLS_DES_CRYPT_ECB)
#  include <mbedtls/version.h>
#  यदि MBEDTLS_संस्करणम्_सङ्ख्या < 0x03020000
#  error "mbedTLS 3.2.0 or greater required"
#  endif
#  include <mbedtls/des.h>
#  define USE_MBEDTLS_DES
#नोचेद्यदि defined(USE_OS400CRYPTO)
#  include "cipher.mih"  /* mih/cipher */
#  define USE_CURL_DES_समुच्चयः_ODD_PARITY
#नोचेद्यदि defined(USE_WIN32_CRYPTO)
#  include <wincrypt.h>
#  define USE_CURL_DES_समुच्चयः_ODD_PARITY
#अन्यथा
#  error "cannot compile NTLM support without a crypto library with DES."
#यदि_अन्तः

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "strcase.h"
#अन्तर्भाव "curl_ntlm_core.h"
#अन्तर्भाव "curl_md5.h"
#अन्तर्भाव "curl_hmac.h"
#अन्तर्भाव "curl_md4.h"
#अन्तर्भाव "vauth/vauth.h"

#यदि_परिभाषितम् USE_CURL_DES_समुच्चयः_ODD_PARITY
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
स्थिर शून्य curl_des_समुच्चयः_odd_parity(अचिह्नित वर्ण *अष्टकानि, size_t len)
{
  size_t i;

  प्रति(i = 0; i < len; i++) {
    अचिह्नित वर्ण b = अष्टकानि[i];

    bool needs_parity = (((b >> 7) ^ (b >> 6) ^ (b >> 5) ^
                          (b >> 4) ^ (b >> 3) ^ (b >> 2) ^
                          (b >> 1)) & 0x01) == 0;

    यदि(needs_parity)
      अष्टकानि[i] |= 0x01;
    अन्यथा
      अष्टकानि[i] &= 0xfe;
  }
}
#यदि_अन्तः /* USE_CURL_DES_SET_ODD_PARITY */

/*
 * Turns a 56-bit key into being 64-bit wide.
 */
स्थिर शून्य extend_कुञ्चिका_56_गन्तव्यम्_64(ध्रुव अचिह्नित वर्ण *कुञ्चिका_56, वर्ण *कुञ्चिका)
{
  कुञ्चिका[0] = (वर्ण)कुञ्चिका_56[0];
  कुञ्चिका[1] = (वर्ण)(((कुञ्चिका_56[0] << 7) & 0xFF) | (कुञ्चिका_56[1] >> 1));
  कुञ्चिका[2] = (वर्ण)(((कुञ्चिका_56[1] << 6) & 0xFF) | (कुञ्चिका_56[2] >> 2));
  कुञ्चिका[3] = (वर्ण)(((कुञ्चिका_56[2] << 5) & 0xFF) | (कुञ्चिका_56[3] >> 3));
  कुञ्चिका[4] = (वर्ण)(((कुञ्चिका_56[3] << 4) & 0xFF) | (कुञ्चिका_56[4] >> 4));
  कुञ्चिका[5] = (वर्ण)(((कुञ्चिका_56[4] << 3) & 0xFF) | (कुञ्चिका_56[5] >> 5));
  कुञ्चिका[6] = (वर्ण)(((कुञ्चिका_56[5] << 2) & 0xFF) | (कुञ्चिका_56[6] >> 6));
  कुञ्चिका[7] = (वर्ण)((कुञ्चिका_56[6] << 1) & 0xFF);
}

#यदि_परिभाषितम् USE_OPENSSL_DES
/*
 * Turns a 56-bit key into a 64-bit, odd parity key and sets the key. The
 * key schedule ks is also set.
 */
स्थिर शून्य सज्जता_des_कुञ्चिका(ध्रुव अचिह्नित वर्ण *कुञ्चिका_56, DES_कुञ्चिका_कालसारणी *ks)
{
  DES_cblock कुञ्चिका;

  /* Expand the 56-bit key to 64 bits */
  extend_कुञ्चिका_56_गन्तव्यम्_64(कुञ्चिका_56, (वर्ण *)&कुञ्चिका);

  /* Set the key parity to odd */
  DES_समुच्चयः_odd_parity(&कुञ्चिका);

  /* Set the key */
  DES_समुच्चयः_कुञ्चिका_unchecked(&कुञ्चिका, ks);
}

#नोचेद्यदि defined(USE_WOLFSSL_DES)
स्थिर शून्य सज्जता_des_कुञ्चिका(ध्रुव अचिह्नित वर्ण *कुञ्चिका_56, Des *des)
{
  byte कुञ्चिका[8];

  /* Expand the 56-bit key to 64 bits */
  extend_कुञ्चिका_56_गन्तव्यम्_64(कुञ्चिका_56, (वर्ण *)कुञ्चिका);

  /* Set the key */
  wc_Des_समुच्चयः_कुञ्चिका(des, कुञ्चिका, रिक्त, 0);
}

#नोचेद्यदि defined(USE_GNUTLS_DES)
स्थिर शून्य सज्जता_des_कुञ्चिका(ध्रुव अचिह्नित वर्ण *कुञ्चिका_56, संरचना des_ctx *des)
{
  वर्ण कुञ्चिका[8];

  /* Expand the 56-bit key to 64 bits */
  extend_कुञ्चिका_56_गन्तव्यम्_64(कुञ्चिका_56, कुञ्चिका);

  /* Set the key parity to odd */
  curl_des_समुच्चयः_odd_parity((अचिह्नित वर्ण *)कुञ्चिका, परिमाण(कुञ्चिका));

  /* Set the key */
  des_समुच्चयः_कुञ्चिका(des, (ध्रुव uint8_t *)कुञ्चिका);
}

#नोचेद्यदि defined(USE_MBEDTLS_DES)
स्थिर bool encrypt_des(ध्रुव अचिह्नित वर्ण *अन्तर्, अचिह्नित वर्ण *बहिर्गमनम्,
                        ध्रुव अचिह्नित वर्ण *कुञ्चिका_56)
{
  mbedtls_des_context ctx;
  वर्ण कुञ्चिका[8];

  /* Expand the 56-bit key to 64 bits */
  extend_कुञ्चिका_56_गन्तव्यम्_64(कुञ्चिका_56, कुञ्चिका);

  /* Set the key parity to odd */
  mbedtls_des_कुञ्चिका_समुच्चयः_parity((अचिह्नित वर्ण *)कुञ्चिका);

  /* Perform the encryption */
  mbedtls_des_init(&ctx);
  mbedtls_des_setkey_enc(&ctx, (अचिह्नित वर्ण *)कुञ्चिका);
  प्रत्यागम mbedtls_des_crypt_ecb(&ctx, अन्तर्, बहिर्गमनम्) == 0;
}

#नोचेद्यदि defined(USE_OS400CRYPTO)
स्थिर bool encrypt_des(ध्रुव अचिह्नित वर्ण *अन्तर्, अचिह्नित वर्ण *बहिर्गमनम्,
                        ध्रुव अचिह्नित वर्ण *कुञ्चिका_56)
{
  वर्ण कुञ्चिका[8];
  _CIPHER_Control_T ctl;

  /* Setup the cipher control structure */
  ctl.Func_ID = ENCRYPT_ONLY;
  ctl.दत्त_Len = परिमाण(कुञ्चिका);

  /* Expand the 56-bit key to 64 bits */
  extend_कुञ्चिका_56_गन्तव्यम्_64(कुञ्चिका_56, ctl.Crypto_कुञ्चिका);

  /* Set the key parity to odd */
  curl_des_समुच्चयः_odd_parity((अचिह्नित वर्ण *)ctl.Crypto_कुञ्चिका, ctl.दत्त_Len);

  /* Perform the encryption */
  _CIPHER((_SPCPTR *)&बहिर्गमनम्, &ctl, (_SPCPTR *)&अन्तर्);

  प्रत्यागम TRUE;
}

#नोचेद्यदि defined(USE_WIN32_CRYPTO)
स्थिर bool encrypt_des(ध्रुव अचिह्नित वर्ण *अन्तर्, अचिह्नित वर्ण *बहिर्गमनम्,
                        ध्रुव अचिह्नित वर्ण *कुञ्चिका_56)
{
  HCRYPTPROV hprov;
  HCRYPTKEY hkey;
  संरचना {
    BLOBHEADER hdr;
    अचिह्नित पूर्णाङ्क len;
    वर्ण कुञ्चिका[8];
  } blob;
  DWORD len = 8;
  BOOL res;

  /* Acquire the crypto provider */
  यदि(!CryptAcquireContext(&hprov, रिक्त, रिक्त, PROV_RSA_पूर्णम्,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    प्रत्यागम FALSE;

  /* Setup the key blob structure */
  स्मृतिं_पूरय(&blob, 0, परिमाण(blob));
  blob.hdr.bType = PLAINTEXTKEYBLOB;
  blob.hdr.b_संस्करणम् = 2;
  blob.hdr.ai_कुञ्चिका_Alg = CALG_DES;
  blob.len = परिमाण(blob.कुञ्चिका);

  /* Expand the 56-bit key to 64 bits */
  extend_कुञ्चिका_56_गन्तव्यम्_64(कुञ्चिका_56, blob.कुञ्चिका);

  /* Set the key parity to odd */
  curl_des_समुच्चयः_odd_parity((अचिह्नित वर्ण *)blob.कुञ्चिका, परिमाण(blob.कुञ्चिका));

  /* Import the key */
  यदि(!Crypt_Import_कुञ्चिका(hprov, (BYTE *)&blob, परिमाण(blob), 0, 0, &hkey)) {
    CryptReleaseContext(hprov, 0);

    प्रत्यागम FALSE;
  }

  स्मृतिं_प्रतिलिख(बहिर्गमनम्, अन्तर्, 8);

  /* Perform the encryption */
  res = CryptEncrypt(hkey, 0, FALSE, 0, बहिर्गमनम्, &len, len);

  Crypt_Destroy_कुञ्चिका(hkey);
  CryptReleaseContext(hprov, 0);

  प्रत्यागम res;
}

#यदि_अन्तः /* crypto backends */

/*
 * takes a 21-byte array and treats it as 3 56-bit DES keys. The
 * 8-byte plaintext is encrypted with each key and the resulting 24
 * bytes are stored in the results array.
 */
शून्य Curl_ntlm_core_lm_resp(ध्रुव अचिह्नित वर्ण *कुञ्चिकाः,
                            ध्रुव अचिह्नित वर्ण *plaintext,
                            अचिह्नित वर्ण *results)
{
#यदि_परिभाषितम् USE_OPENSSL_DES
  DES_कुञ्चिका_कालसारणी ks;

  सज्जता_des_कुञ्चिका(कुञ्चिकाः, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)results, &ks, DES_ENCRYPT);

  सज्जता_des_कुञ्चिका(कुञ्चिकाः + 7, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 8), &ks, DES_ENCRYPT);

  सज्जता_des_कुञ्चिका(कुञ्चिकाः + 14, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 16), &ks, DES_ENCRYPT);
#नोचेद्यदि defined(USE_WOLFSSL_DES)
  Des des;
  सज्जता_des_कुञ्चिका(कुञ्चिकाः, &des);
  wc_Des_EcbEncrypt(&des, results, plaintext, DES_कुञ्चिका_आकार);
  सज्जता_des_कुञ्चिका(कुञ्चिकाः + 7, &des);
  wc_Des_EcbEncrypt(&des, results + 8, plaintext, DES_कुञ्चिका_आकार);
  सज्जता_des_कुञ्चिका(कुञ्चिकाः + 14, &des);
  wc_Des_EcbEncrypt(&des, results + 16, plaintext, DES_कुञ्चिका_आकार);
#नोचेद्यदि defined(USE_GNUTLS_DES)
  संरचना des_ctx des;
  सज्जता_des_कुञ्चिका(कुञ्चिकाः, &des);
  des_encrypt(&des, 8, results, plaintext);
  सज्जता_des_कुञ्चिका(कुञ्चिकाः + 7, &des);
  des_encrypt(&des, 8, results + 8, plaintext);
  सज्जता_des_कुञ्चिका(कुञ्चिकाः + 14, &des);
  des_encrypt(&des, 8, results + 16, plaintext);
#नोचेद्यदि defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
  encrypt_des(plaintext, results, कुञ्चिकाः);
  encrypt_des(plaintext, results + 8, कुञ्चिकाः + 7);
  encrypt_des(plaintext, results + 16, कुञ्चिकाः + 14);
#अन्यथा
  (शून्य)कुञ्चिकाः;
  (शून्य)plaintext;
  (शून्य)results;
#यदि_अन्तः
}

/*
 * Set up lanmanager hashed password
 */
CURLcode Curl_ntlm_core_mk_lm_hash(ध्रुव वर्ण *password,
                                   अचिह्नित वर्ण *lmbuffer /* 21 bytes */)
{
  अचिह्नित वर्ण pw[14];
  स्थिर ध्रुव अचिह्नित वर्ण magic[] = {
    0x4B, 0x47, 0x53, 0x21, 0x40, 0x23, 0x24, 0x25 /* i.e. KGS!@#$% */
  };
  size_t len = CURLMIN(वाक्यदैर्घ्यम्(password), 14);

  Curl_strntoupper((वर्ण *)pw, password, len);
  स्मृतिं_पूरय(&pw[len], 0, 14 - len);

  {
    /* Create LanManager hashed password. */
#यदि_परिभाषितम् USE_OPENSSL_DES
    DES_कुञ्चिका_कालसारणी ks;

    सज्जता_des_कुञ्चिका(pw, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)lmbuffer, &ks, DES_ENCRYPT);

    सज्जता_des_कुञ्चिका(pw + 7, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)(lmbuffer + 8), &ks, DES_ENCRYPT);
#नोचेद्यदि defined(USE_WOLFSSL_DES)
    Des des;
    सज्जता_des_कुञ्चिका(pw, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer, magic, DES_कुञ्चिका_आकार);
    सज्जता_des_कुञ्चिका(pw + 7, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer + 8, magic, DES_कुञ्चिका_आकार);
#नोचेद्यदि defined(USE_GNUTLS_DES)
    संरचना des_ctx des;
    सज्जता_des_कुञ्चिका(pw, &des);
    des_encrypt(&des, 8, lmbuffer, magic);
    सज्जता_des_कुञ्चिका(pw + 7, &des);
    des_encrypt(&des, 8, lmbuffer + 8, magic);
#नोचेद्यदि defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
    encrypt_des(magic, lmbuffer, pw);
    encrypt_des(magic, lmbuffer + 8, pw + 7);
#यदि_अन्तः

    स्मृतिं_पूरय(lmbuffer + 16, 0, 21 - 16);
  }

  प्रत्यागम CURLE_OK;
}

स्थिर शून्य ascii_गन्तव्यम्_unicode_le(अचिह्नित वर्ण *dest, ध्रुव वर्ण *src,
                                size_t srclen)
{
  size_t i;
  प्रति(i = 0; i < srclen; i++) {
    dest[2 * i] = (अचिह्नित वर्ण)src[i];
    dest[(2 * i) + 1] = '\0';
  }
}

/*
 * Set up nt hashed passwords
 * @unittest: 1600
 */
CURLcode Curl_ntlm_core_mk_nt_hash(ध्रुव वर्ण *password,
                                   अचिह्नित वर्ण *ntbuffer /* 21 bytes */)
{
  size_t len = वाक्यदैर्घ्यम्(password);
  अचिह्नित वर्ण *pw;
  CURLcode परिणाम;
  यदि(len > SIZE_MAX / 2) /* avoid integer overflow */
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  pw = len ? curlx_malloc(len * 2) : (अचिह्नित वर्ण *)curlx_strdup("");
  यदि(!pw)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  ascii_गन्तव्यम्_unicode_le(pw, password, len);

  /* Create NT hashed password. */
  परिणाम = Curl_md4it(ntbuffer, pw, 2 * len);
  यदि(!परिणाम)
    स्मृतिं_पूरय(ntbuffer + 16, 0, 21 - 16);

  curlx_free(pw);

  प्रत्यागम परिणाम;
}

#यदि_अपरिभाषितम् USE_WINDOWS_SSPI

#परिभाषय NTLMv2_BLOB_SIGNATURE "\x01\x01\x00\x00"
#परिभाषय NTLMv2_BLOB_LEN       (44 - 16 + ntlm->target_info_len + 4)

/* Timestamp in tenths of a microsecond since January 1, 1601 00:00:00 UTC. */
संरचना ms_filetime {
  अचिह्नित पूर्णाङ्क dw_नीचः_Date_Time;
  अचिह्नित पूर्णाङ्क dw_उच्चः_Date_Time;
};

/* Convert a time_t to an MS FILETIME (MS-DTYP section 2.3.3). */
स्थिर शून्य time2filetime(संरचना ms_filetime *ft, time_t t)
{
#यदि SIZEOF_TIME_T > 4
  t = (t + (curl_off_t)11644473600) * 10000000;
  ft->dw_नीचः_Date_Time = (अचिह्नित पूर्णाङ्क)(t & 0xFFFFFFFF);
  ft->dw_उच्चः_Date_Time = (अचिह्नित पूर्णाङ्क)(t >> 32);
#अन्यथा
  अचिह्नित पूर्णाङ्क r, s;
  अचिह्नित पूर्णाङ्क i;

  ft->dw_नीचः_Date_Time = (अचिह्नित पूर्णाङ्क)(t & 0xFFFFFFFF);
  ft->dw_उच्चः_Date_Time = 0;

#यदि_अपरिभाषितम् HAVE_TIME_T_UNSIGNED
  /* Extend sign if needed. */
  यदि(ft->dw_नीचः_Date_Time & 0x80000000)
    ft->dw_उच्चः_Date_Time = ~(अचिह्नित पूर्णाङ्क)0;
#यदि_अन्तः

  /* Bias seconds to Jan 1, 1601.
     134774 days = 11644473600 seconds = 0x2B6109100 */
  r = ft->dw_नीचः_Date_Time;
  ft->dw_नीचः_Date_Time = (ft->dw_नीचः_Date_Time + 0xB6109100U) & 0xFFFFFFFF;
  ft->dw_उच्चः_Date_Time += ft->dw_नीचः_Date_Time < r ? 0x03 : 0x02;

  /* Convert to tenths of microseconds. */
  ft->dw_उच्चः_Date_Time *= 10000000;
  i = 32;
  कुरु {
    i -= 8;
    s = ((ft->dw_नीचः_Date_Time >> i) & 0xFF) * (10000000 - 1);
    r = (s << i) & 0xFFFFFFFF;
    s >>= 1;   /* Split shift to avoid width overflow. */
    s >>= 31 - i;
    ft->dw_नीचः_Date_Time = (ft->dw_नीचः_Date_Time + r) & 0xFFFFFFFF;
    यदि(ft->dw_नीचः_Date_Time < r)
      s++;
    ft->dw_उच्चः_Date_Time += s;
  } यावत्(i);
  ft->dw_उच्चः_Date_Time &= 0xFFFFFFFF;
#यदि_अन्तः
}

स्थिर शून्य ascii_uppercase_गन्तव्यम्_unicode_le(अचिह्नित वर्ण *dest,
                                          ध्रुव वर्ण *src, size_t srclen)
{
  size_t i;
  प्रति(i = 0; i < srclen; i++) {
    dest[2 * i] = (अचिह्नित वर्ण)(Curl_raw_toupper(src[i]));
    dest[(2 * i) + 1] = '\0';
  }
}

/* This creates the NTLMv2 hash by using NTLM hash as the key and Unicode
 * (uppercase UserName + Domain) as the data
 */
CURLcode Curl_ntlm_core_mk_ntlmv2_hash(ध्रुव वर्ण *user, size_t userlen,
                                       ध्रुव वर्ण *domain, size_t domlen,
                                       अचिह्नित वर्ण *ntlmhash,
                                       अचिह्नित वर्ण *ntlmv2hash)
{
  /* Unicode representation */
  size_t तटस्थः_len;
  अचिह्नित वर्ण *तटस्थः;
  CURLcode परिणाम = CURLE_OK;

  यदि((userlen > CURL_MAX_निवेशः_दीर्घता) || (domlen > CURL_MAX_निवेशः_दीर्घता))
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  तटस्थः_len = (userlen + domlen) * 2;
  तटस्थः = curlx_malloc(तटस्थः_len + 1);

  यदि(!तटस्थः)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  ascii_uppercase_गन्तव्यम्_unicode_le(तटस्थः, user, userlen);
  ascii_गन्तव्यम्_unicode_le(तटस्थः + (userlen << 1), domain, domlen);

  परिणाम = Curl_hmacit(&Curl_HMAC_MD5, ntlmhash, 16, तटस्थः, तटस्थः_len,
                       ntlmv2hash);
  curlx_free(तटस्थः);

  प्रत्यागम परिणाम;
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
CURLcode Curl_ntlm_core_mk_ntlmv2_resp(ध्रुव अचिह्नित वर्ण *ntlmv2hash,
                                       ध्रुव अचिह्नित वर्ण *challenge_client,
                                       ध्रुव संरचना ntlmdata *ntlm,
                                       अचिह्नित वर्ण **ntresp,
                                       अचिह्नित पूर्णाङ्क *ntresp_len)
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

  अचिह्नित पूर्णाङ्क len = 0;
  अचिह्नित वर्ण *ptr = रिक्त;
  अचिह्नित वर्ण hmac_output[HMAC_MD5_दीर्घता];
  संरचना ms_filetime tw;

  CURLcode परिणाम = CURLE_OK;

  /* Calculate the timestamp */
#यदि_परिभाषितम् DEBUGBUILD
  वर्ण *force_timestamp = getenv("CURL_FORCETIME");
  यदि(force_timestamp)
    time2filetime(&tw, (time_t)0);
  अन्यथा
#यदि_अन्तः
    time2filetime(&tw, time(रिक्त));

  /* Calculate the response len */
  len = HMAC_MD5_दीर्घता + NTLMv2_BLOB_LEN;

  /* Allocate the response */
  ptr = curlx_calloc(1, len);
  यदि(!ptr)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  /* Create the BLOB structure */
  curl_msnprintf((वर्ण *)ptr + HMAC_MD5_दीर्घता, NTLMv2_BLOB_LEN,
                 "%c%c%c%c"           /* NTLMv2_BLOB_SIGNATURE */
                 "%c%c%c%c"           /* Reserved = 0 */
                 "%c%c%c%c%c%c%c%c",  /* Timestamp */
                 NTLMv2_BLOB_SIGNATURE[0], NTLMv2_BLOB_SIGNATURE[1],
                 NTLMv2_BLOB_SIGNATURE[2], NTLMv2_BLOB_SIGNATURE[3],
                 0, 0, 0, 0,
                 LONGQUARTET(tw.dw_नीचः_Date_Time),
                 LONGQUARTET(tw.dw_उच्चः_Date_Time));

  स्मृतिं_प्रतिलिख(ptr + 32, challenge_client, 8);
  यदि(ntlm->target_info_len)
    स्मृतिं_प्रतिलिख(ptr + 44, ntlm->target_info, ntlm->target_info_len);

  /* Concatenate the Type 2 challenge with the BLOB and do HMAC MD5 */
  स्मृतिं_प्रतिलिख(ptr + 8, &ntlm->nonce[0], 8);
  परिणाम = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, HMAC_MD5_दीर्घता, ptr + 8,
                       NTLMv2_BLOB_LEN + 8, hmac_output);
  यदि(परिणाम) {
    curlx_free(ptr);
    प्रत्यागम परिणाम;
  }

  /* Concatenate the HMAC MD5 output  with the BLOB */
  स्मृतिं_प्रतिलिख(ptr, hmac_output, HMAC_MD5_दीर्घता);

  /* Return the response */
  *ntresp = ptr;
  *ntresp_len = len;

  प्रत्यागम परिणाम;
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
CURLcode Curl_ntlm_core_mk_lmv2_resp(ध्रुव अचिह्नित वर्ण *ntlmv2hash,
                                     ध्रुव अचिह्नित वर्ण *challenge_client,
                                     ध्रुव अचिह्नित वर्ण *challenge_server,
                                     अचिह्नित वर्ण *lmresp)
{
  अचिह्नित वर्ण दत्त[16];
  अचिह्नित वर्ण hmac_output[16];
  CURLcode परिणाम = CURLE_OK;

  स्मृतिं_प्रतिलिख(&दत्त[0], challenge_server, 8);
  स्मृतिं_प्रतिलिख(&दत्त[8], challenge_client, 8);

  परिणाम = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, 16, &दत्त[0], 16,
                       hmac_output);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  /* Concatenate the HMAC MD5 output with the client nonce */
  स्मृतिं_प्रतिलिख(lmresp, hmac_output, 16);
  स्मृतिं_प्रतिलिख(lmresp + 16, challenge_client, 8);

  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* !USE_WINDOWS_SSPI */

#यदि_अन्तः /* USE_CURL_NTLM_CORE */
