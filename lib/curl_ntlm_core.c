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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef USE_CURL_NTLM_CORE

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

#ifdef USE_GNUTLS
#അന്തര്ഭാവ <nettle/version.h>
#എങ്കിൽ NETTLE_VERSION_MAJOR < 4
#define USE_GNUTLS_DES
#endif
#endif

#എങ്കിൽ defined(USE_OPENSSL) && defined(HAVE_DES_ECB_ENCRYPT)

#  include <openssl/des.h>
#  ifdef OPENSSL_IS_AWSLC  /* for versions 1.2.0 to 1.30.1 */
#    define DES_set_താക്കോൽ_unchecked (ശൂന്യം)DES_set_താക്കോൽ
#  endif
#  define USE_OPENSSL_DES

#അല്ലെങ്കിലെങ്കിൽ defined(USE_WOLFSSL) && defined(HAVE_WC_DES_ECBENCRYPT)

#  include <wolfssl/options.h>
#  include <wolfssl/wolfcrypt/des3.h>
#  define USE_WOLFSSL_DES

#അല്ലെങ്കിലെങ്കിൽ defined(USE_GNUTLS_DES)
#  include <nettle/des.h>
#  define USE_CURL_DES_SET_ODD_PARITY
#അല്ലെങ്കിലെങ്കിൽ defined(USE_MBEDTLS) && defined(HAVE_MBEDTLS_DES_CRYPT_ECB)
#  include <mbedtls/version.h>
#  എങ്കിൽ MBEDTLS_VERSION_സംഖ്യ < 0x03020000
#  error "mbedTLS 3.2.0 or greater required"
#  endif
#  include <mbedtls/des.h>
#  define USE_MBEDTLS_DES
#അല്ലെങ്കിലെങ്കിൽ defined(USE_OS400CRYPTO)
#  include "cipher.mih"  /* mih/cipher */
#  define USE_CURL_DES_SET_ODD_PARITY
#അല്ലെങ്കിലെങ്കിൽ defined(USE_WIN32_CRYPTO)
#  include <wincrypt.h>
#  define USE_CURL_DES_SET_ODD_PARITY
#അല്ലെങ്കിൽ
#  error "cannot compile NTLM support without a crypto library with DES."
#endif

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "strcase.h"
#അന്തര്ഭാവ "curl_ntlm_core.h"
#അന്തര്ഭാവ "curl_md5.h"
#അന്തര്ഭാവ "curl_hmac.h"
#അന്തര്ഭാവ "curl_md4.h"
#അന്തര്ഭാവ "vauth/vauth.h"

#ifdef USE_CURL_DES_SET_ODD_PARITY
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
സ്ഥിതം ശൂന്യം curl_des_set_odd_parity(unsigned അക്ഷരം *bytes, size_t len)
{
  size_t i;

  വേണ്ടി(i = 0; i < len; i++) {
    unsigned അക്ഷരം b = bytes[i];

    bool needs_parity = (((b >> 7) ^ (b >> 6) ^ (b >> 5) ^
                          (b >> 4) ^ (b >> 3) ^ (b >> 2) ^
                          (b >> 1)) & 0x01) == 0;

    എങ്കിൽ(needs_parity)
      bytes[i] |= 0x01;
    അല്ലെങ്കിൽ
      bytes[i] &= 0xfe;
  }
}
#endif /* USE_CURL_DES_SET_ODD_PARITY */

/*
 * Turns a 56-bit key into being 64-bit wide.
 */
സ്ഥിതം ശൂന്യം extend_താക്കോൽ_56_to_64(സ്ഥിരം unsigned അക്ഷരം *താക്കോൽ_56, അക്ഷരം *താക്കോൽ)
{
  താക്കോൽ[0] = (അക്ഷരം)താക്കോൽ_56[0];
  താക്കോൽ[1] = (അക്ഷരം)(((താക്കോൽ_56[0] << 7) & 0xFF) | (താക്കോൽ_56[1] >> 1));
  താക്കോൽ[2] = (അക്ഷരം)(((താക്കോൽ_56[1] << 6) & 0xFF) | (താക്കോൽ_56[2] >> 2));
  താക്കോൽ[3] = (അക്ഷരം)(((താക്കോൽ_56[2] << 5) & 0xFF) | (താക്കോൽ_56[3] >> 3));
  താക്കോൽ[4] = (അക്ഷരം)(((താക്കോൽ_56[3] << 4) & 0xFF) | (താക്കോൽ_56[4] >> 4));
  താക്കോൽ[5] = (അക്ഷരം)(((താക്കോൽ_56[4] << 3) & 0xFF) | (താക്കോൽ_56[5] >> 5));
  താക്കോൽ[6] = (അക്ഷരം)(((താക്കോൽ_56[5] << 2) & 0xFF) | (താക്കോൽ_56[6] >> 6));
  താക്കോൽ[7] = (അക്ഷരം)((താക്കോൽ_56[6] << 1) & 0xFF);
}

#ifdef USE_OPENSSL_DES
/*
 * Turns a 56-bit key into a 64-bit, odd parity key and sets the key. The
 * key schedule ks is also set.
 */
സ്ഥിതം ശൂന്യം setup_des_താക്കോൽ(സ്ഥിരം unsigned അക്ഷരം *താക്കോൽ_56, DES_താക്കോൽ_schedule *ks)
{
  DES_cblock താക്കോൽ;

  /* Expand the 56-bit key to 64 bits */
  extend_താക്കോൽ_56_to_64(താക്കോൽ_56, (അക്ഷരം *)&താക്കോൽ);

  /* Set the key parity to odd */
  DES_set_odd_parity(&താക്കോൽ);

  /* Set the key */
  DES_set_താക്കോൽ_unchecked(&താക്കോൽ, ks);
}

#അല്ലെങ്കിലെങ്കിൽ defined(USE_WOLFSSL_DES)
സ്ഥിതം ശൂന്യം setup_des_താക്കോൽ(സ്ഥിരം unsigned അക്ഷരം *താക്കോൽ_56, Des *des)
{
  byte താക്കോൽ[8];

  /* Expand the 56-bit key to 64 bits */
  extend_താക്കോൽ_56_to_64(താക്കോൽ_56, (അക്ഷരം *)താക്കോൽ);

  /* Set the key */
  wc_Des_Set_താക്കോൽ(des, താക്കോൽ, ശൂന്യ, 0);
}

#അല്ലെങ്കിലെങ്കിൽ defined(USE_GNUTLS_DES)
സ്ഥിതം ശൂന്യം setup_des_താക്കോൽ(സ്ഥിരം unsigned അക്ഷരം *താക്കോൽ_56, ഘടന des_ctx *des)
{
  അക്ഷരം താക്കോൽ[8];

  /* Expand the 56-bit key to 64 bits */
  extend_താക്കോൽ_56_to_64(താക്കോൽ_56, താക്കോൽ);

  /* Set the key parity to odd */
  curl_des_set_odd_parity((unsigned അക്ഷരം *)താക്കോൽ, വലുപ്പം(താക്കോൽ));

  /* Set the key */
  des_set_താക്കോൽ(des, (സ്ഥിരം uint8_t *)താക്കോൽ);
}

#അല്ലെങ്കിലെങ്കിൽ defined(USE_MBEDTLS_DES)
സ്ഥിതം bool encrypt_des(സ്ഥിരം unsigned അക്ഷരം *in, unsigned അക്ഷരം *out,
                        സ്ഥിരം unsigned അക്ഷരം *താക്കോൽ_56)
{
  mbedtls_des_context ctx;
  അക്ഷരം താക്കോൽ[8];

  /* Expand the 56-bit key to 64 bits */
  extend_താക്കോൽ_56_to_64(താക്കോൽ_56, താക്കോൽ);

  /* Set the key parity to odd */
  mbedtls_des_താക്കോൽ_set_parity((unsigned അക്ഷരം *)താക്കോൽ);

  /* Perform the encryption */
  mbedtls_des_init(&ctx);
  mbedtls_des_setkey_enc(&ctx, (unsigned അക്ഷരം *)താക്കോൽ);
  തിരികെ mbedtls_des_crypt_ecb(&ctx, in, out) == 0;
}

#അല്ലെങ്കിലെങ്കിൽ defined(USE_OS400CRYPTO)
സ്ഥിതം bool encrypt_des(സ്ഥിരം unsigned അക്ഷരം *in, unsigned അക്ഷരം *out,
                        സ്ഥിരം unsigned അക്ഷരം *താക്കോൽ_56)
{
  അക്ഷരം താക്കോൽ[8];
  _CIPHER_Control_T ctl;

  /* Setup the cipher control structure */
  ctl.Func_ID = ENCRYPT_ONLY;
  ctl.Data_Len = വലുപ്പം(താക്കോൽ);

  /* Expand the 56-bit key to 64 bits */
  extend_താക്കോൽ_56_to_64(താക്കോൽ_56, ctl.Crypto_താക്കോൽ);

  /* Set the key parity to odd */
  curl_des_set_odd_parity((unsigned അക്ഷരം *)ctl.Crypto_താക്കോൽ, ctl.Data_Len);

  /* Perform the encryption */
  _CIPHER((_SPCPTR *)&out, &ctl, (_SPCPTR *)&in);

  തിരികെ TRUE;
}

#അല്ലെങ്കിലെങ്കിൽ defined(USE_WIN32_CRYPTO)
സ്ഥിതം bool encrypt_des(സ്ഥിരം unsigned അക്ഷരം *in, unsigned അക്ഷരം *out,
                        സ്ഥിരം unsigned അക്ഷരം *താക്കോൽ_56)
{
  HCRYPTPROV hprov;
  HCRYPTKEY hkey;
  ഘടന {
    BLOBHEADER hdr;
    unsigned പൂർണ്ണസംഖ്യ len;
    അക്ഷരം താക്കോൽ[8];
  } blob;
  DWORD len = 8;
  BOOL res;

  /* Acquire the crypto provider */
  എങ്കിൽ(!CryptAcquireContext(&hprov, ശൂന്യ, ശൂന്യ, PROV_RSA_FULL,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    തിരികെ FALSE;

  /* Setup the key blob structure */
  memset(&blob, 0, വലുപ്പം(blob));
  blob.hdr.bType = PLAINTEXTKEYBLOB;
  blob.hdr.bVersion = 2;
  blob.hdr.ai_താക്കോൽ_Alg = CALG_DES;
  blob.len = വലുപ്പം(blob.താക്കോൽ);

  /* Expand the 56-bit key to 64 bits */
  extend_താക്കോൽ_56_to_64(താക്കോൽ_56, blob.താക്കോൽ);

  /* Set the key parity to odd */
  curl_des_set_odd_parity((unsigned അക്ഷരം *)blob.താക്കോൽ, വലുപ്പം(blob.താക്കോൽ));

  /* Import the key */
  എങ്കിൽ(!Crypt_Import_താക്കോൽ(hprov, (BYTE *)&blob, വലുപ്പം(blob), 0, 0, &hkey)) {
    CryptReleaseContext(hprov, 0);

    തിരികെ FALSE;
  }

  memcpy(out, in, 8);

  /* Perform the encryption */
  res = CryptEncrypt(hkey, 0, FALSE, 0, out, &len, len);

  Crypt_Destroy_താക്കോൽ(hkey);
  CryptReleaseContext(hprov, 0);

  തിരികെ res;
}

#endif /* crypto backends */

/*
 * takes a 21-byte array and treats it as 3 56-bit DES keys. The
 * 8-byte plaintext is encrypted with each key and the resulting 24
 * bytes are stored in the results array.
 */
ശൂന്യം Curl_ntlm_core_lm_resp(സ്ഥിരം unsigned അക്ഷരം *താക്കോലുകൾ,
                            സ്ഥിരം unsigned അക്ഷരം *plaintext,
                            unsigned അക്ഷരം *results)
{
#ifdef USE_OPENSSL_DES
  DES_താക്കോൽ_schedule ks;

  setup_des_താക്കോൽ(താക്കോലുകൾ, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)results, &ks, DES_ENCRYPT);

  setup_des_താക്കോൽ(താക്കോലുകൾ + 7, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 8), &ks, DES_ENCRYPT);

  setup_des_താക്കോൽ(താക്കോലുകൾ + 14, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 16), &ks, DES_ENCRYPT);
#അല്ലെങ്കിലെങ്കിൽ defined(USE_WOLFSSL_DES)
  Des des;
  setup_des_താക്കോൽ(താക്കോലുകൾ, &des);
  wc_Des_EcbEncrypt(&des, results, plaintext, DES_താക്കോൽ_SIZE);
  setup_des_താക്കോൽ(താക്കോലുകൾ + 7, &des);
  wc_Des_EcbEncrypt(&des, results + 8, plaintext, DES_താക്കോൽ_SIZE);
  setup_des_താക്കോൽ(താക്കോലുകൾ + 14, &des);
  wc_Des_EcbEncrypt(&des, results + 16, plaintext, DES_താക്കോൽ_SIZE);
#അല്ലെങ്കിലെങ്കിൽ defined(USE_GNUTLS_DES)
  ഘടന des_ctx des;
  setup_des_താക്കോൽ(താക്കോലുകൾ, &des);
  des_encrypt(&des, 8, results, plaintext);
  setup_des_താക്കോൽ(താക്കോലുകൾ + 7, &des);
  des_encrypt(&des, 8, results + 8, plaintext);
  setup_des_താക്കോൽ(താക്കോലുകൾ + 14, &des);
  des_encrypt(&des, 8, results + 16, plaintext);
#അല്ലെങ്കിലെങ്കിൽ defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
  encrypt_des(plaintext, results, താക്കോലുകൾ);
  encrypt_des(plaintext, results + 8, താക്കോലുകൾ + 7);
  encrypt_des(plaintext, results + 16, താക്കോലുകൾ + 14);
#അല്ലെങ്കിൽ
  (ശൂന്യം)താക്കോലുകൾ;
  (ശൂന്യം)plaintext;
  (ശൂന്യം)results;
#endif
}

/*
 * Set up lanmanager hashed password
 */
CURLcode Curl_ntlm_core_mk_lm_hash(സ്ഥിരം അക്ഷരം *password,
                                   unsigned അക്ഷരം *lmbuffer /* 21 bytes */)
{
  unsigned അക്ഷരം pw[14];
  സ്ഥിതം സ്ഥിരം unsigned അക്ഷരം magic[] = {
    0x4B, 0x47, 0x53, 0x21, 0x40, 0x23, 0x24, 0x25 /* i.e. KGS!@#$% */
  };
  size_t len = CURLMIN(strlen(password), 14);

  Curl_strntoupper((അക്ഷരം *)pw, password, len);
  memset(&pw[len], 0, 14 - len);

  {
    /* Create LanManager hashed password. */
#ifdef USE_OPENSSL_DES
    DES_താക്കോൽ_schedule ks;

    setup_des_താക്കോൽ(pw, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)lmbuffer, &ks, DES_ENCRYPT);

    setup_des_താക്കോൽ(pw + 7, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)(lmbuffer + 8), &ks, DES_ENCRYPT);
#അല്ലെങ്കിലെങ്കിൽ defined(USE_WOLFSSL_DES)
    Des des;
    setup_des_താക്കോൽ(pw, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer, magic, DES_താക്കോൽ_SIZE);
    setup_des_താക്കോൽ(pw + 7, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer + 8, magic, DES_താക്കോൽ_SIZE);
#അല്ലെങ്കിലെങ്കിൽ defined(USE_GNUTLS_DES)
    ഘടന des_ctx des;
    setup_des_താക്കോൽ(pw, &des);
    des_encrypt(&des, 8, lmbuffer, magic);
    setup_des_താക്കോൽ(pw + 7, &des);
    des_encrypt(&des, 8, lmbuffer + 8, magic);
#അല്ലെങ്കിലെങ്കിൽ defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
    encrypt_des(magic, lmbuffer, pw);
    encrypt_des(magic, lmbuffer + 8, pw + 7);
#endif

    memset(lmbuffer + 16, 0, 21 - 16);
  }

  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം ascii_to_unicode_le(unsigned അക്ഷരം *dest, സ്ഥിരം അക്ഷരം *src,
                                size_t srclen)
{
  size_t i;
  വേണ്ടി(i = 0; i < srclen; i++) {
    dest[2 * i] = (unsigned അക്ഷരം)src[i];
    dest[(2 * i) + 1] = '\0';
  }
}

/*
 * Set up nt hashed passwords
 * @unittest: 1600
 */
CURLcode Curl_ntlm_core_mk_nt_hash(സ്ഥിരം അക്ഷരം *password,
                                   unsigned അക്ഷരം *ntbuffer /* 21 bytes */)
{
  size_t len = strlen(password);
  unsigned അക്ഷരം *pw;
  CURLcode result;
  എങ്കിൽ(len > SIZE_MAX / 2) /* avoid integer overflow */
    തിരികെ CURLE_OUT_OF_MEMORY;
  pw = len ? curlx_malloc(len * 2) : (unsigned അക്ഷരം *)curlx_strdup("");
  എങ്കിൽ(!pw)
    തിരികെ CURLE_OUT_OF_MEMORY;

  ascii_to_unicode_le(pw, password, len);

  /* Create NT hashed password. */
  result = Curl_md4it(ntbuffer, pw, 2 * len);
  എങ്കിൽ(!result)
    memset(ntbuffer + 16, 0, 21 - 16);

  curlx_free(pw);

  തിരികെ result;
}

#ifndef USE_WINDOWS_SSPI

#define NTLMv2_BLOB_SIGNATURE "\x01\x01\x00\x00"
#define NTLMv2_BLOB_LEN       (44 - 16 + ntlm->target_info_len + 4)

/* Timestamp in tenths of a microsecond since January 1, 1601 00:00:00 UTC. */
ഘടന ms_filetime {
  unsigned പൂർണ്ണസംഖ്യ dw_താഴെയുള്ള_Date_Time;
  unsigned പൂർണ്ണസംഖ്യ dw_മുകളിലെ_Date_Time;
};

/* Convert a time_t to an MS FILETIME (MS-DTYP section 2.3.3). */
സ്ഥിതം ശൂന്യം time2filetime(ഘടന ms_filetime *ft, time_t t)
{
#എങ്കിൽ SIZEOF_TIME_T > 4
  t = (t + (curl_off_t)11644473600) * 10000000;
  ft->dw_താഴെയുള്ള_Date_Time = (unsigned പൂർണ്ണസംഖ്യ)(t & 0xFFFFFFFF);
  ft->dw_മുകളിലെ_Date_Time = (unsigned പൂർണ്ണസംഖ്യ)(t >> 32);
#അല്ലെങ്കിൽ
  unsigned പൂർണ്ണസംഖ്യ r, s;
  unsigned പൂർണ്ണസംഖ്യ i;

  ft->dw_താഴെയുള്ള_Date_Time = (unsigned പൂർണ്ണസംഖ്യ)(t & 0xFFFFFFFF);
  ft->dw_മുകളിലെ_Date_Time = 0;

#ifndef HAVE_TIME_T_UNSIGNED
  /* Extend sign if needed. */
  എങ്കിൽ(ft->dw_താഴെയുള്ള_Date_Time & 0x80000000)
    ft->dw_മുകളിലെ_Date_Time = ~(unsigned പൂർണ്ണസംഖ്യ)0;
#endif

  /* Bias seconds to Jan 1, 1601.
     134774 days = 11644473600 seconds = 0x2B6109100 */
  r = ft->dw_താഴെയുള്ള_Date_Time;
  ft->dw_താഴെയുള്ള_Date_Time = (ft->dw_താഴെയുള്ള_Date_Time + 0xB6109100U) & 0xFFFFFFFF;
  ft->dw_മുകളിലെ_Date_Time += ft->dw_താഴെയുള്ള_Date_Time < r ? 0x03 : 0x02;

  /* Convert to tenths of microseconds. */
  ft->dw_മുകളിലെ_Date_Time *= 10000000;
  i = 32;
  ചെയ്യുക {
    i -= 8;
    s = ((ft->dw_താഴെയുള്ള_Date_Time >> i) & 0xFF) * (10000000 - 1);
    r = (s << i) & 0xFFFFFFFF;
    s >>= 1;   /* Split shift to avoid width overflow. */
    s >>= 31 - i;
    ft->dw_താഴെയുള്ള_Date_Time = (ft->dw_താഴെയുള്ള_Date_Time + r) & 0xFFFFFFFF;
    എങ്കിൽ(ft->dw_താഴെയുള്ള_Date_Time < r)
      s++;
    ft->dw_മുകളിലെ_Date_Time += s;
  } ആകുമ്പോൾ(i);
  ft->dw_മുകളിലെ_Date_Time &= 0xFFFFFFFF;
#endif
}

സ്ഥിതം ശൂന്യം ascii_uppercase_to_unicode_le(unsigned അക്ഷരം *dest,
                                          സ്ഥിരം അക്ഷരം *src, size_t srclen)
{
  size_t i;
  വേണ്ടി(i = 0; i < srclen; i++) {
    dest[2 * i] = (unsigned അക്ഷരം)(Curl_raw_toupper(src[i]));
    dest[(2 * i) + 1] = '\0';
  }
}

/* This creates the NTLMv2 hash by using NTLM hash as the key and Unicode
 * (uppercase UserName + Domain) as the data
 */
CURLcode Curl_ntlm_core_mk_ntlmv2_hash(സ്ഥിരം അക്ഷരം *user, size_t userlen,
                                       സ്ഥിരം അക്ഷരം *domain, size_t domlen,
                                       unsigned അക്ഷരം *ntlmhash,
                                       unsigned അക്ഷരം *ntlmv2hash)
{
  /* Unicode representation */
  size_t identity_len;
  unsigned അക്ഷരം *identity;
  CURLcode result = CURLE_OK;

  എങ്കിൽ((userlen > CURL_MAX_നിവേശം_LENGTH) || (domlen > CURL_MAX_നിവേശം_LENGTH))
    തിരികെ CURLE_OUT_OF_MEMORY;

  identity_len = (userlen + domlen) * 2;
  identity = curlx_malloc(identity_len + 1);

  എങ്കിൽ(!identity)
    തിരികെ CURLE_OUT_OF_MEMORY;

  ascii_uppercase_to_unicode_le(identity, user, userlen);
  ascii_to_unicode_le(identity + (userlen << 1), domain, domlen);

  result = Curl_hmacit(&Curl_HMAC_MD5, ntlmhash, 16, identity, identity_len,
                       ntlmv2hash);
  curlx_free(identity);

  തിരികെ result;
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
CURLcode Curl_ntlm_core_mk_ntlmv2_resp(സ്ഥിരം unsigned അക്ഷരം *ntlmv2hash,
                                       സ്ഥിരം unsigned അക്ഷരം *challenge_client,
                                       സ്ഥിരം ഘടന ntlmdata *ntlm,
                                       unsigned അക്ഷരം **ntresp,
                                       unsigned പൂർണ്ണസംഖ്യ *ntresp_len)
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

  unsigned പൂർണ്ണസംഖ്യ len = 0;
  unsigned അക്ഷരം *ptr = ശൂന്യ;
  unsigned അക്ഷരം hmac_output[HMAC_MD5_LENGTH];
  ഘടന ms_filetime tw;

  CURLcode result = CURLE_OK;

  /* Calculate the timestamp */
#ifdef DEBUGBUILD
  അക്ഷരം *force_timestamp = getenv("CURL_FORCETIME");
  എങ്കിൽ(force_timestamp)
    time2filetime(&tw, (time_t)0);
  അല്ലെങ്കിൽ
#endif
    time2filetime(&tw, time(ശൂന്യ));

  /* Calculate the response len */
  len = HMAC_MD5_LENGTH + NTLMv2_BLOB_LEN;

  /* Allocate the response */
  ptr = curlx_calloc(1, len);
  എങ്കിൽ(!ptr)
    തിരികെ CURLE_OUT_OF_MEMORY;

  /* Create the BLOB structure */
  curl_msnprintf((അക്ഷരം *)ptr + HMAC_MD5_LENGTH, NTLMv2_BLOB_LEN,
                 "%c%c%c%c"           /* NTLMv2_BLOB_SIGNATURE */
                 "%c%c%c%c"           /* Reserved = 0 */
                 "%c%c%c%c%c%c%c%c",  /* Timestamp */
                 NTLMv2_BLOB_SIGNATURE[0], NTLMv2_BLOB_SIGNATURE[1],
                 NTLMv2_BLOB_SIGNATURE[2], NTLMv2_BLOB_SIGNATURE[3],
                 0, 0, 0, 0,
                 LONGQUARTET(tw.dw_താഴെയുള്ള_Date_Time),
                 LONGQUARTET(tw.dw_മുകളിലെ_Date_Time));

  memcpy(ptr + 32, challenge_client, 8);
  എങ്കിൽ(ntlm->target_info_len)
    memcpy(ptr + 44, ntlm->target_info, ntlm->target_info_len);

  /* Concatenate the Type 2 challenge with the BLOB and do HMAC MD5 */
  memcpy(ptr + 8, &ntlm->nonce[0], 8);
  result = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, HMAC_MD5_LENGTH, ptr + 8,
                       NTLMv2_BLOB_LEN + 8, hmac_output);
  എങ്കിൽ(result) {
    curlx_free(ptr);
    തിരികെ result;
  }

  /* Concatenate the HMAC MD5 output  with the BLOB */
  memcpy(ptr, hmac_output, HMAC_MD5_LENGTH);

  /* Return the response */
  *ntresp = ptr;
  *ntresp_len = len;

  തിരികെ result;
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
CURLcode Curl_ntlm_core_mk_lmv2_resp(സ്ഥിരം unsigned അക്ഷരം *ntlmv2hash,
                                     സ്ഥിരം unsigned അക്ഷരം *challenge_client,
                                     സ്ഥിരം unsigned അക്ഷരം *challenge_server,
                                     unsigned അക്ഷരം *lmresp)
{
  unsigned അക്ഷരം data[16];
  unsigned അക്ഷരം hmac_output[16];
  CURLcode result = CURLE_OK;

  memcpy(&data[0], challenge_server, 8);
  memcpy(&data[8], challenge_client, 8);

  result = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, 16, &data[0], 16,
                       hmac_output);
  എങ്കിൽ(result)
    തിരികെ result;

  /* Concatenate the HMAC MD5 output with the client nonce */
  memcpy(lmresp, hmac_output, 16);
  memcpy(lmresp + 16, challenge_client, 8);

  തിരികെ result;
}

#endif /* !USE_WINDOWS_SSPI */

#endif /* USE_CURL_NTLM_CORE */
