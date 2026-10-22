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

#अगर_घोषित USE_GNUTLS
#समावेश <nettle/version.h>
#अगर NETTLE_VERSION_MAJOR < 4
#घोषणा USE_GNUTLS_DES
#पूर्ण_अगर
#पूर्ण_अगर

#अगर defined(USE_OPENSSL) && defined(HAVE_DES_ECB_ENCRYPT)

#  include <openssl/des.h>
#  ifdef OPENSSL_IS_AWSLC  /* for versions 1.2.0 to 1.30.1 */
#    define DES_set_कुंजी_unchecked (व्योम)DES_set_कुंजी
#  endif
#  define USE_OPENSSL_DES

#या_अगर defined(USE_WOLFSSL) && defined(HAVE_WC_DES_ECBENCRYPT)

#  include <wolfssl/options.h>
#  include <wolfssl/wolfcrypt/des3.h>
#  define USE_WOLFSSL_DES

#या_अगर defined(USE_GNUTLS_DES)
#  include <nettle/des.h>
#  define USE_CURL_DES_SET_ODD_PARITY
#या_अगर defined(USE_MBEDTLS) && defined(HAVE_MBEDTLS_DES_CRYPT_ECB)
#  include <mbedtls/version.h>
#  अगर MBEDTLS_VERSION_संख्या < 0x03020000
#  error "mbedTLS 3.2.0 or greater required"
#  endif
#  include <mbedtls/des.h>
#  define USE_MBEDTLS_DES
#या_अगर defined(USE_OS400CRYPTO)
#  include "cipher.mih"  /* mih/cipher */
#  define USE_CURL_DES_SET_ODD_PARITY
#या_अगर defined(USE_WIN32_CRYPTO)
#  include <wincrypt.h>
#  define USE_CURL_DES_SET_ODD_PARITY
#अन्यथा
#  error "cannot compile NTLM support without a crypto library with DES."
#पूर्ण_अगर

#समावेश "urldata.h"
#समावेश "strcase.h"
#समावेश "curl_ntlm_core.h"
#समावेश "curl_md5.h"
#समावेश "curl_hmac.h"
#समावेश "curl_md4.h"
#समावेश "vauth/vauth.h"

#अगर_घोषित USE_CURL_DES_SET_ODD_PARITY
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
अटल व्योम curl_des_set_odd_parity(अचिन्हित अक्षर *bytes, माप_प्रकार len)
{
  माप_प्रकार i;

  क्रम(i = 0; i < len; i++) {
    अचिन्हित अक्षर b = bytes[i];

    bool needs_parity = (((b >> 7) ^ (b >> 6) ^ (b >> 5) ^
                          (b >> 4) ^ (b >> 3) ^ (b >> 2) ^
                          (b >> 1)) & 0x01) == 0;

    अगर(needs_parity)
      bytes[i] |= 0x01;
    अन्यथा
      bytes[i] &= 0xfe;
  }
}
#पूर्ण_अगर /* USE_CURL_DES_SET_ODD_PARITY */

/*
 * Turns a 56-bit key into being 64-bit wide.
 */
अटल व्योम extend_कुंजी_56_to_64(स्थिर अचिन्हित अक्षर *कुंजी_56, अक्षर *कुंजी)
{
  कुंजी[0] = (अक्षर)कुंजी_56[0];
  कुंजी[1] = (अक्षर)(((कुंजी_56[0] << 7) & 0xFF) | (कुंजी_56[1] >> 1));
  कुंजी[2] = (अक्षर)(((कुंजी_56[1] << 6) & 0xFF) | (कुंजी_56[2] >> 2));
  कुंजी[3] = (अक्षर)(((कुंजी_56[2] << 5) & 0xFF) | (कुंजी_56[3] >> 3));
  कुंजी[4] = (अक्षर)(((कुंजी_56[3] << 4) & 0xFF) | (कुंजी_56[4] >> 4));
  कुंजी[5] = (अक्षर)(((कुंजी_56[4] << 3) & 0xFF) | (कुंजी_56[5] >> 5));
  कुंजी[6] = (अक्षर)(((कुंजी_56[5] << 2) & 0xFF) | (कुंजी_56[6] >> 6));
  कुंजी[7] = (अक्षर)((कुंजी_56[6] << 1) & 0xFF);
}

#अगर_घोषित USE_OPENSSL_DES
/*
 * Turns a 56-bit key into a 64-bit, odd parity key and sets the key. The
 * key schedule ks is also set.
 */
अटल व्योम setup_des_कुंजी(स्थिर अचिन्हित अक्षर *कुंजी_56, DES_कुंजी_schedule *ks)
{
  DES_cblock कुंजी;

  /* Expand the 56-bit key to 64 bits */
  extend_कुंजी_56_to_64(कुंजी_56, (अक्षर *)&कुंजी);

  /* Set the key parity to odd */
  DES_set_odd_parity(&कुंजी);

  /* Set the key */
  DES_set_कुंजी_unchecked(&कुंजी, ks);
}

#या_अगर defined(USE_WOLFSSL_DES)
अटल व्योम setup_des_कुंजी(स्थिर अचिन्हित अक्षर *कुंजी_56, Des *des)
{
  byte कुंजी[8];

  /* Expand the 56-bit key to 64 bits */
  extend_कुंजी_56_to_64(कुंजी_56, (अक्षर *)कुंजी);

  /* Set the key */
  wc_Des_Set_कुंजी(des, कुंजी, शून्य, 0);
}

#या_अगर defined(USE_GNUTLS_DES)
अटल व्योम setup_des_कुंजी(स्थिर अचिन्हित अक्षर *कुंजी_56, काष्ठा des_ctx *des)
{
  अक्षर कुंजी[8];

  /* Expand the 56-bit key to 64 bits */
  extend_कुंजी_56_to_64(कुंजी_56, कुंजी);

  /* Set the key parity to odd */
  curl_des_set_odd_parity((अचिन्हित अक्षर *)कुंजी, माप(कुंजी));

  /* Set the key */
  des_set_कुंजी(des, (स्थिर uint8_t *)कुंजी);
}

#या_अगर defined(USE_MBEDTLS_DES)
अटल bool encrypt_des(स्थिर अचिन्हित अक्षर *in, अचिन्हित अक्षर *out,
                        स्थिर अचिन्हित अक्षर *कुंजी_56)
{
  mbedtls_des_context ctx;
  अक्षर कुंजी[8];

  /* Expand the 56-bit key to 64 bits */
  extend_कुंजी_56_to_64(कुंजी_56, कुंजी);

  /* Set the key parity to odd */
  mbedtls_des_कुंजी_set_parity((अचिन्हित अक्षर *)कुंजी);

  /* Perform the encryption */
  mbedtls_des_init(&ctx);
  mbedtls_des_setkey_enc(&ctx, (अचिन्हित अक्षर *)कुंजी);
  वापस mbedtls_des_crypt_ecb(&ctx, in, out) == 0;
}

#या_अगर defined(USE_OS400CRYPTO)
अटल bool encrypt_des(स्थिर अचिन्हित अक्षर *in, अचिन्हित अक्षर *out,
                        स्थिर अचिन्हित अक्षर *कुंजी_56)
{
  अक्षर कुंजी[8];
  _CIPHER_Control_T ctl;

  /* Setup the cipher control structure */
  ctl.Func_ID = ENCRYPT_ONLY;
  ctl.तथ्य_Len = माप(कुंजी);

  /* Expand the 56-bit key to 64 bits */
  extend_कुंजी_56_to_64(कुंजी_56, ctl.Crypto_कुंजी);

  /* Set the key parity to odd */
  curl_des_set_odd_parity((अचिन्हित अक्षर *)ctl.Crypto_कुंजी, ctl.तथ्य_Len);

  /* Perform the encryption */
  _CIPHER((_SPCPTR *)&out, &ctl, (_SPCPTR *)&in);

  वापस TRUE;
}

#या_अगर defined(USE_WIN32_CRYPTO)
अटल bool encrypt_des(स्थिर अचिन्हित अक्षर *in, अचिन्हित अक्षर *out,
                        स्थिर अचिन्हित अक्षर *कुंजी_56)
{
  HCRYPTPROV hprov;
  HCRYPTKEY hkey;
  काष्ठा {
    BLOBHEADER hdr;
    अचिन्हित पूर्णांक len;
    अक्षर कुंजी[8];
  } blob;
  DWORD len = 8;
  BOOL res;

  /* Acquire the crypto provider */
  अगर(!CryptAcquireContext(&hprov, शून्य, शून्य, PROV_RSA_FULL,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    वापस FALSE;

  /* Setup the key blob structure */
  स_रखो(&blob, 0, माप(blob));
  blob.hdr.bType = PLAINTEXTKEYBLOB;
  blob.hdr.bVersion = 2;
  blob.hdr.ai_कुंजी_Alg = CALG_DES;
  blob.len = माप(blob.कुंजी);

  /* Expand the 56-bit key to 64 bits */
  extend_कुंजी_56_to_64(कुंजी_56, blob.कुंजी);

  /* Set the key parity to odd */
  curl_des_set_odd_parity((अचिन्हित अक्षर *)blob.कुंजी, माप(blob.कुंजी));

  /* Import the key */
  अगर(!Crypt_Import_कुंजी(hprov, (BYTE *)&blob, माप(blob), 0, 0, &hkey)) {
    CryptReleaseContext(hprov, 0);

    वापस FALSE;
  }

  स_नकल(out, in, 8);

  /* Perform the encryption */
  res = CryptEncrypt(hkey, 0, FALSE, 0, out, &len, len);

  Crypt_Destroy_कुंजी(hkey);
  CryptReleaseContext(hprov, 0);

  वापस res;
}

#पूर्ण_अगर /* crypto backends */

/*
 * takes a 21-byte array and treats it as 3 56-bit DES keys. The
 * 8-byte plaintext is encrypted with each key and the resulting 24
 * bytes are stored in the results array.
 */
व्योम Curl_ntlm_core_lm_resp(स्थिर अचिन्हित अक्षर *कुंजियाँ,
                            स्थिर अचिन्हित अक्षर *plaintext,
                            अचिन्हित अक्षर *results)
{
#अगर_घोषित USE_OPENSSL_DES
  DES_कुंजी_schedule ks;

  setup_des_कुंजी(कुंजियाँ, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)results, &ks, DES_ENCRYPT);

  setup_des_कुंजी(कुंजियाँ + 7, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 8), &ks, DES_ENCRYPT);

  setup_des_कुंजी(कुंजियाँ + 14, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 16), &ks, DES_ENCRYPT);
#या_अगर defined(USE_WOLFSSL_DES)
  Des des;
  setup_des_कुंजी(कुंजियाँ, &des);
  wc_Des_EcbEncrypt(&des, results, plaintext, DES_कुंजी_आकार);
  setup_des_कुंजी(कुंजियाँ + 7, &des);
  wc_Des_EcbEncrypt(&des, results + 8, plaintext, DES_कुंजी_आकार);
  setup_des_कुंजी(कुंजियाँ + 14, &des);
  wc_Des_EcbEncrypt(&des, results + 16, plaintext, DES_कुंजी_आकार);
#या_अगर defined(USE_GNUTLS_DES)
  काष्ठा des_ctx des;
  setup_des_कुंजी(कुंजियाँ, &des);
  des_encrypt(&des, 8, results, plaintext);
  setup_des_कुंजी(कुंजियाँ + 7, &des);
  des_encrypt(&des, 8, results + 8, plaintext);
  setup_des_कुंजी(कुंजियाँ + 14, &des);
  des_encrypt(&des, 8, results + 16, plaintext);
#या_अगर defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
  encrypt_des(plaintext, results, कुंजियाँ);
  encrypt_des(plaintext, results + 8, कुंजियाँ + 7);
  encrypt_des(plaintext, results + 16, कुंजियाँ + 14);
#अन्यथा
  (व्योम)कुंजियाँ;
  (व्योम)plaintext;
  (व्योम)results;
#पूर्ण_अगर
}

/*
 * Set up lanmanager hashed password
 */
CURLcode Curl_ntlm_core_mk_lm_hash(स्थिर अक्षर *password,
                                   अचिन्हित अक्षर *lmbuffer /* 21 bytes */)
{
  अचिन्हित अक्षर pw[14];
  अटल स्थिर अचिन्हित अक्षर magic[] = {
    0x4B, 0x47, 0x53, 0x21, 0x40, 0x23, 0x24, 0x25 /* i.e. KGS!@#$% */
  };
  माप_प्रकार len = CURLMIN(म_माप(password), 14);

  Curl_strntoupper((अक्षर *)pw, password, len);
  स_रखो(&pw[len], 0, 14 - len);

  {
    /* Create LanManager hashed password. */
#अगर_घोषित USE_OPENSSL_DES
    DES_कुंजी_schedule ks;

    setup_des_कुंजी(pw, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)lmbuffer, &ks, DES_ENCRYPT);

    setup_des_कुंजी(pw + 7, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)(lmbuffer + 8), &ks, DES_ENCRYPT);
#या_अगर defined(USE_WOLFSSL_DES)
    Des des;
    setup_des_कुंजी(pw, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer, magic, DES_कुंजी_आकार);
    setup_des_कुंजी(pw + 7, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer + 8, magic, DES_कुंजी_आकार);
#या_अगर defined(USE_GNUTLS_DES)
    काष्ठा des_ctx des;
    setup_des_कुंजी(pw, &des);
    des_encrypt(&des, 8, lmbuffer, magic);
    setup_des_कुंजी(pw + 7, &des);
    des_encrypt(&des, 8, lmbuffer + 8, magic);
#या_अगर defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
    encrypt_des(magic, lmbuffer, pw);
    encrypt_des(magic, lmbuffer + 8, pw + 7);
#पूर्ण_अगर

    स_रखो(lmbuffer + 16, 0, 21 - 16);
  }

  वापस CURLE_OK;
}

अटल व्योम ascii_to_unicode_le(अचिन्हित अक्षर *dest, स्थिर अक्षर *src,
                                माप_प्रकार srclen)
{
  माप_प्रकार i;
  क्रम(i = 0; i < srclen; i++) {
    dest[2 * i] = (अचिन्हित अक्षर)src[i];
    dest[(2 * i) + 1] = '\0';
  }
}

/*
 * Set up nt hashed passwords
 * @unittest: 1600
 */
CURLcode Curl_ntlm_core_mk_nt_hash(स्थिर अक्षर *password,
                                   अचिन्हित अक्षर *ntbuffer /* 21 bytes */)
{
  माप_प्रकार len = म_माप(password);
  अचिन्हित अक्षर *pw;
  CURLcode परिणाम;
  अगर(len > SIZE_MAX / 2) /* avoid integer overflow */
    वापस CURLE_OUT_OF_MEMORY;
  pw = len ? curlx_malloc(len * 2) : (अचिन्हित अक्षर *)curlx_strdup("");
  अगर(!pw)
    वापस CURLE_OUT_OF_MEMORY;

  ascii_to_unicode_le(pw, password, len);

  /* Create NT hashed password. */
  परिणाम = Curl_md4it(ntbuffer, pw, 2 * len);
  अगर(!परिणाम)
    स_रखो(ntbuffer + 16, 0, 21 - 16);

  curlx_free(pw);

  वापस परिणाम;
}

#अगर_अघोषित USE_WINDOWS_SSPI

#घोषणा NTLMv2_BLOB_SIGNATURE "\x01\x01\x00\x00"
#घोषणा NTLMv2_BLOB_LEN       (44 - 16 + ntlm->target_info_len + 4)

/* Timestamp in tenths of a microsecond since January 1, 1601 00:00:00 UTC. */
काष्ठा ms_filetime {
  अचिन्हित पूर्णांक dw_निचला_Date_Time;
  अचिन्हित पूर्णांक dw_ऊपरी_Date_Time;
};

/* Convert a time_t to an MS FILETIME (MS-DTYP section 2.3.3). */
अटल व्योम time2filetime(काष्ठा ms_filetime *ft, समय_प्रकार t)
{
#अगर SIZEOF_TIME_T > 4
  t = (t + (curl_off_t)11644473600) * 10000000;
  ft->dw_निचला_Date_Time = (अचिन्हित पूर्णांक)(t & 0xFFFFFFFF);
  ft->dw_ऊपरी_Date_Time = (अचिन्हित पूर्णांक)(t >> 32);
#अन्यथा
  अचिन्हित पूर्णांक r, s;
  अचिन्हित पूर्णांक i;

  ft->dw_निचला_Date_Time = (अचिन्हित पूर्णांक)(t & 0xFFFFFFFF);
  ft->dw_ऊपरी_Date_Time = 0;

#अगर_अघोषित HAVE_TIME_T_UNSIGNED
  /* Extend sign if needed. */
  अगर(ft->dw_निचला_Date_Time & 0x80000000)
    ft->dw_ऊपरी_Date_Time = ~(अचिन्हित पूर्णांक)0;
#पूर्ण_अगर

  /* Bias seconds to Jan 1, 1601.
     134774 days = 11644473600 seconds = 0x2B6109100 */
  r = ft->dw_निचला_Date_Time;
  ft->dw_निचला_Date_Time = (ft->dw_निचला_Date_Time + 0xB6109100U) & 0xFFFFFFFF;
  ft->dw_ऊपरी_Date_Time += ft->dw_निचला_Date_Time < r ? 0x03 : 0x02;

  /* Convert to tenths of microseconds. */
  ft->dw_ऊपरी_Date_Time *= 10000000;
  i = 32;
  करो {
    i -= 8;
    s = ((ft->dw_निचला_Date_Time >> i) & 0xFF) * (10000000 - 1);
    r = (s << i) & 0xFFFFFFFF;
    s >>= 1;   /* Split shift to avoid width overflow. */
    s >>= 31 - i;
    ft->dw_निचला_Date_Time = (ft->dw_निचला_Date_Time + r) & 0xFFFFFFFF;
    अगर(ft->dw_निचला_Date_Time < r)
      s++;
    ft->dw_ऊपरी_Date_Time += s;
  } जबतक(i);
  ft->dw_ऊपरी_Date_Time &= 0xFFFFFFFF;
#पूर्ण_अगर
}

अटल व्योम ascii_uppercase_to_unicode_le(अचिन्हित अक्षर *dest,
                                          स्थिर अक्षर *src, माप_प्रकार srclen)
{
  माप_प्रकार i;
  क्रम(i = 0; i < srclen; i++) {
    dest[2 * i] = (अचिन्हित अक्षर)(Curl_raw_toupper(src[i]));
    dest[(2 * i) + 1] = '\0';
  }
}

/* This creates the NTLMv2 hash by using NTLM hash as the key and Unicode
 * (uppercase UserName + Domain) as the data
 */
CURLcode Curl_ntlm_core_mk_ntlmv2_hash(स्थिर अक्षर *user, माप_प्रकार userlen,
                                       स्थिर अक्षर *domain, माप_प्रकार domlen,
                                       अचिन्हित अक्षर *ntlmhash,
                                       अचिन्हित अक्षर *ntlmv2hash)
{
  /* Unicode representation */
  माप_प्रकार identity_len;
  अचिन्हित अक्षर *identity;
  CURLcode परिणाम = CURLE_OK;

  अगर((userlen > CURL_MAX_निवेश_लंबाई) || (domlen > CURL_MAX_निवेश_लंबाई))
    वापस CURLE_OUT_OF_MEMORY;

  identity_len = (userlen + domlen) * 2;
  identity = curlx_malloc(identity_len + 1);

  अगर(!identity)
    वापस CURLE_OUT_OF_MEMORY;

  ascii_uppercase_to_unicode_le(identity, user, userlen);
  ascii_to_unicode_le(identity + (userlen << 1), domain, domlen);

  परिणाम = Curl_hmacit(&Curl_HMAC_MD5, ntlmhash, 16, identity, identity_len,
                       ntlmv2hash);
  curlx_free(identity);

  वापस परिणाम;
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
CURLcode Curl_ntlm_core_mk_ntlmv2_resp(स्थिर अचिन्हित अक्षर *ntlmv2hash,
                                       स्थिर अचिन्हित अक्षर *challenge_client,
                                       स्थिर काष्ठा ntlmdata *ntlm,
                                       अचिन्हित अक्षर **ntresp,
                                       अचिन्हित पूर्णांक *ntresp_len)
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

  अचिन्हित पूर्णांक len = 0;
  अचिन्हित अक्षर *ptr = शून्य;
  अचिन्हित अक्षर hmac_output[HMAC_MD5_लंबाई];
  काष्ठा ms_filetime tw;

  CURLcode परिणाम = CURLE_OK;

  /* Calculate the timestamp */
#अगर_घोषित DEBUGBUILD
  अक्षर *force_timestamp = दो_पर्या("CURL_FORCETIME");
  अगर(force_timestamp)
    time2filetime(&tw, (समय_प्रकार)0);
  अन्यथा
#पूर्ण_अगर
    time2filetime(&tw, समय(शून्य));

  /* Calculate the response len */
  len = HMAC_MD5_लंबाई + NTLMv2_BLOB_LEN;

  /* Allocate the response */
  ptr = curlx_calloc(1, len);
  अगर(!ptr)
    वापस CURLE_OUT_OF_MEMORY;

  /* Create the BLOB structure */
  curl_msnprintf((अक्षर *)ptr + HMAC_MD5_लंबाई, NTLMv2_BLOB_LEN,
                 "%c%c%c%c"           /* NTLMv2_BLOB_SIGNATURE */
                 "%c%c%c%c"           /* Reserved = 0 */
                 "%c%c%c%c%c%c%c%c",  /* Timestamp */
                 NTLMv2_BLOB_SIGNATURE[0], NTLMv2_BLOB_SIGNATURE[1],
                 NTLMv2_BLOB_SIGNATURE[2], NTLMv2_BLOB_SIGNATURE[3],
                 0, 0, 0, 0,
                 LONGQUARTET(tw.dw_निचला_Date_Time),
                 LONGQUARTET(tw.dw_ऊपरी_Date_Time));

  स_नकल(ptr + 32, challenge_client, 8);
  अगर(ntlm->target_info_len)
    स_नकल(ptr + 44, ntlm->target_info, ntlm->target_info_len);

  /* Concatenate the Type 2 challenge with the BLOB and do HMAC MD5 */
  स_नकल(ptr + 8, &ntlm->nonce[0], 8);
  परिणाम = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, HMAC_MD5_लंबाई, ptr + 8,
                       NTLMv2_BLOB_LEN + 8, hmac_output);
  अगर(परिणाम) {
    curlx_free(ptr);
    वापस परिणाम;
  }

  /* Concatenate the HMAC MD5 output  with the BLOB */
  स_नकल(ptr, hmac_output, HMAC_MD5_लंबाई);

  /* Return the response */
  *ntresp = ptr;
  *ntresp_len = len;

  वापस परिणाम;
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
CURLcode Curl_ntlm_core_mk_lmv2_resp(स्थिर अचिन्हित अक्षर *ntlmv2hash,
                                     स्थिर अचिन्हित अक्षर *challenge_client,
                                     स्थिर अचिन्हित अक्षर *challenge_server,
                                     अचिन्हित अक्षर *lmresp)
{
  अचिन्हित अक्षर तथ्य[16];
  अचिन्हित अक्षर hmac_output[16];
  CURLcode परिणाम = CURLE_OK;

  स_नकल(&तथ्य[0], challenge_server, 8);
  स_नकल(&तथ्य[8], challenge_client, 8);

  परिणाम = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, 16, &तथ्य[0], 16,
                       hmac_output);
  अगर(परिणाम)
    वापस परिणाम;

  /* Concatenate the HMAC MD5 output with the client nonce */
  स_नकल(lmresp, hmac_output, 16);
  स_नकल(lmresp + 16, challenge_client, 8);

  वापस परिणाम;
}

#पूर्ण_अगर /* !USE_WINDOWS_SSPI */

#पूर्ण_अगर /* USE_CURL_NTLM_CORE */
