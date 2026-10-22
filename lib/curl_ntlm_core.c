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
#অন্তর্ভাব "curl_setup.h"

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
#অন্তর্ভাব <nettle/version.h>
#যদি NETTLE_VERSION_MAJOR < 4
#define USE_GNUTLS_DES
#endif
#endif

#যদি defined(USE_OPENSSL) && defined(HAVE_DES_ECB_ENCRYPT)

#  include <openssl/des.h>
#  ifdef OPENSSL_IS_AWSLC  /* for versions 1.2.0 to 1.30.1 */
#    define DES_set_চাবি_unchecked (শূন্য)DES_set_চাবি
#  endif
#  define USE_OPENSSL_DES

#নোচেদ্যদি defined(USE_WOLFSSL) && defined(HAVE_WC_DES_ECBENCRYPT)

#  include <wolfssl/options.h>
#  include <wolfssl/wolfcrypt/des3.h>
#  define USE_WOLFSSL_DES

#নোচেদ্যদি defined(USE_GNUTLS_DES)
#  include <nettle/des.h>
#  define USE_CURL_DES_SET_ODD_PARITY
#নোচেদ্যদি defined(USE_MBEDTLS) && defined(HAVE_MBEDTLS_DES_CRYPT_ECB)
#  include <mbedtls/version.h>
#  যদি MBEDTLS_VERSION_সংখ্যা < 0x03020000
#  error "mbedTLS 3.2.0 or greater required"
#  endif
#  include <mbedtls/des.h>
#  define USE_MBEDTLS_DES
#নোচেদ্যদি defined(USE_OS400CRYPTO)
#  include "cipher.mih"  /* mih/cipher */
#  define USE_CURL_DES_SET_ODD_PARITY
#নোচেদ্যদি defined(USE_WIN32_CRYPTO)
#  include <wincrypt.h>
#  define USE_CURL_DES_SET_ODD_PARITY
#অন্যথা
#  error "cannot compile NTLM support without a crypto library with DES."
#endif

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "strcase.h"
#অন্তর্ভাব "curl_ntlm_core.h"
#অন্তর্ভাব "curl_md5.h"
#অন্তর্ভাব "curl_hmac.h"
#অন্তর্ভাব "curl_md4.h"
#অন্তর্ভাব "vauth/vauth.h"

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
স্থির শূন্য curl_des_set_odd_parity(unsigned বর্ণ *bytes, size_t len)
{
  size_t i;

  প্রতি(i = 0; i < len; i++) {
    unsigned বর্ণ b = bytes[i];

    bool needs_parity = (((b >> 7) ^ (b >> 6) ^ (b >> 5) ^
                          (b >> 4) ^ (b >> 3) ^ (b >> 2) ^
                          (b >> 1)) & 0x01) == 0;

    যদি(needs_parity)
      bytes[i] |= 0x01;
    অন্যথা
      bytes[i] &= 0xfe;
  }
}
#endif /* USE_CURL_DES_SET_ODD_PARITY */

/*
 * Turns a 56-bit key into being 64-bit wide.
 */
স্থির শূন্য extend_চাবি_56_to_64(ধ্রুব unsigned বর্ণ *চাবি_56, বর্ণ *চাবি)
{
  চাবি[0] = (বর্ণ)চাবি_56[0];
  চাবি[1] = (বর্ণ)(((চাবি_56[0] << 7) & 0xFF) | (চাবি_56[1] >> 1));
  চাবি[2] = (বর্ণ)(((চাবি_56[1] << 6) & 0xFF) | (চাবি_56[2] >> 2));
  চাবি[3] = (বর্ণ)(((চাবি_56[2] << 5) & 0xFF) | (চাবি_56[3] >> 3));
  চাবি[4] = (বর্ণ)(((চাবি_56[3] << 4) & 0xFF) | (চাবি_56[4] >> 4));
  চাবি[5] = (বর্ণ)(((চাবি_56[4] << 3) & 0xFF) | (চাবি_56[5] >> 5));
  চাবি[6] = (বর্ণ)(((চাবি_56[5] << 2) & 0xFF) | (চাবি_56[6] >> 6));
  চাবি[7] = (বর্ণ)((চাবি_56[6] << 1) & 0xFF);
}

#ifdef USE_OPENSSL_DES
/*
 * Turns a 56-bit key into a 64-bit, odd parity key and sets the key. The
 * key schedule ks is also set.
 */
স্থির শূন্য setup_des_চাবি(ধ্রুব unsigned বর্ণ *চাবি_56, DES_চাবি_schedule *ks)
{
  DES_cblock চাবি;

  /* Expand the 56-bit key to 64 bits */
  extend_চাবি_56_to_64(চাবি_56, (বর্ণ *)&চাবি);

  /* Set the key parity to odd */
  DES_set_odd_parity(&চাবি);

  /* Set the key */
  DES_set_চাবি_unchecked(&চাবি, ks);
}

#নোচেদ্যদি defined(USE_WOLFSSL_DES)
স্থির শূন্য setup_des_চাবি(ধ্রুব unsigned বর্ণ *চাবি_56, Des *des)
{
  byte চাবি[8];

  /* Expand the 56-bit key to 64 bits */
  extend_চাবি_56_to_64(চাবি_56, (বর্ণ *)চাবি);

  /* Set the key */
  wc_Des_Set_চাবি(des, চাবি, রিক্ত, 0);
}

#নোচেদ্যদি defined(USE_GNUTLS_DES)
স্থির শূন্য setup_des_চাবি(ধ্রুব unsigned বর্ণ *চাবি_56, সংরচনা des_ctx *des)
{
  বর্ণ চাবি[8];

  /* Expand the 56-bit key to 64 bits */
  extend_চাবি_56_to_64(চাবি_56, চাবি);

  /* Set the key parity to odd */
  curl_des_set_odd_parity((unsigned বর্ণ *)চাবি, পরিমাণ(চাবি));

  /* Set the key */
  des_set_চাবি(des, (ধ্রুব uint8_t *)চাবি);
}

#নোচেদ্যদি defined(USE_MBEDTLS_DES)
স্থির bool encrypt_des(ধ্রুব unsigned বর্ণ *in, unsigned বর্ণ *out,
                        ধ্রুব unsigned বর্ণ *চাবি_56)
{
  mbedtls_des_context ctx;
  বর্ণ চাবি[8];

  /* Expand the 56-bit key to 64 bits */
  extend_চাবি_56_to_64(চাবি_56, চাবি);

  /* Set the key parity to odd */
  mbedtls_des_চাবি_set_parity((unsigned বর্ণ *)চাবি);

  /* Perform the encryption */
  mbedtls_des_init(&ctx);
  mbedtls_des_setkey_enc(&ctx, (unsigned বর্ণ *)চাবি);
  প্রত্যাগম mbedtls_des_crypt_ecb(&ctx, in, out) == 0;
}

#নোচেদ্যদি defined(USE_OS400CRYPTO)
স্থির bool encrypt_des(ধ্রুব unsigned বর্ণ *in, unsigned বর্ণ *out,
                        ধ্রুব unsigned বর্ণ *চাবি_56)
{
  বর্ণ চাবি[8];
  _CIPHER_Control_T ctl;

  /* Setup the cipher control structure */
  ctl.Func_ID = ENCRYPT_ONLY;
  ctl.Data_Len = পরিমাণ(চাবি);

  /* Expand the 56-bit key to 64 bits */
  extend_চাবি_56_to_64(চাবি_56, ctl.Crypto_চাবি);

  /* Set the key parity to odd */
  curl_des_set_odd_parity((unsigned বর্ণ *)ctl.Crypto_চাবি, ctl.Data_Len);

  /* Perform the encryption */
  _CIPHER((_SPCPTR *)&out, &ctl, (_SPCPTR *)&in);

  প্রত্যাগম TRUE;
}

#নোচেদ্যদি defined(USE_WIN32_CRYPTO)
স্থির bool encrypt_des(ধ্রুব unsigned বর্ণ *in, unsigned বর্ণ *out,
                        ধ্রুব unsigned বর্ণ *চাবি_56)
{
  HCRYPTPROV hprov;
  HCRYPTKEY hkey;
  সংরচনা {
    BLOBHEADER hdr;
    unsigned পূর্ণাঙ্ক len;
    বর্ণ চাবি[8];
  } blob;
  DWORD len = 8;
  BOOL res;

  /* Acquire the crypto provider */
  যদি(!CryptAcquireContext(&hprov, রিক্ত, রিক্ত, PROV_RSA_FULL,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    প্রত্যাগম FALSE;

  /* Setup the key blob structure */
  memset(&blob, 0, পরিমাণ(blob));
  blob.hdr.bType = PLAINTEXTKEYBLOB;
  blob.hdr.bVersion = 2;
  blob.hdr.ai_চাবি_Alg = CALG_DES;
  blob.len = পরিমাণ(blob.চাবি);

  /* Expand the 56-bit key to 64 bits */
  extend_চাবি_56_to_64(চাবি_56, blob.চাবি);

  /* Set the key parity to odd */
  curl_des_set_odd_parity((unsigned বর্ণ *)blob.চাবি, পরিমাণ(blob.চাবি));

  /* Import the key */
  যদি(!Crypt_Import_চাবি(hprov, (BYTE *)&blob, পরিমাণ(blob), 0, 0, &hkey)) {
    CryptReleaseContext(hprov, 0);

    প্রত্যাগম FALSE;
  }

  memcpy(out, in, 8);

  /* Perform the encryption */
  res = CryptEncrypt(hkey, 0, FALSE, 0, out, &len, len);

  Crypt_Destroy_চাবি(hkey);
  CryptReleaseContext(hprov, 0);

  প্রত্যাগম res;
}

#endif /* crypto backends */

/*
 * takes a 21-byte array and treats it as 3 56-bit DES keys. The
 * 8-byte plaintext is encrypted with each key and the resulting 24
 * bytes are stored in the results array.
 */
শূন্য Curl_ntlm_core_lm_resp(ধ্রুব unsigned বর্ণ *চাবিগুলি,
                            ধ্রুব unsigned বর্ণ *plaintext,
                            unsigned বর্ণ *results)
{
#ifdef USE_OPENSSL_DES
  DES_চাবি_schedule ks;

  setup_des_চাবি(চাবিগুলি, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)results, &ks, DES_ENCRYPT);

  setup_des_চাবি(চাবিগুলি + 7, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 8), &ks, DES_ENCRYPT);

  setup_des_চাবি(চাবিগুলি + 14, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 16), &ks, DES_ENCRYPT);
#নোচেদ্যদি defined(USE_WOLFSSL_DES)
  Des des;
  setup_des_চাবি(চাবিগুলি, &des);
  wc_Des_EcbEncrypt(&des, results, plaintext, DES_চাবি_SIZE);
  setup_des_চাবি(চাবিগুলি + 7, &des);
  wc_Des_EcbEncrypt(&des, results + 8, plaintext, DES_চাবি_SIZE);
  setup_des_চাবি(চাবিগুলি + 14, &des);
  wc_Des_EcbEncrypt(&des, results + 16, plaintext, DES_চাবি_SIZE);
#নোচেদ্যদি defined(USE_GNUTLS_DES)
  সংরচনা des_ctx des;
  setup_des_চাবি(চাবিগুলি, &des);
  des_encrypt(&des, 8, results, plaintext);
  setup_des_চাবি(চাবিগুলি + 7, &des);
  des_encrypt(&des, 8, results + 8, plaintext);
  setup_des_চাবি(চাবিগুলি + 14, &des);
  des_encrypt(&des, 8, results + 16, plaintext);
#নোচেদ্যদি defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
  encrypt_des(plaintext, results, চাবিগুলি);
  encrypt_des(plaintext, results + 8, চাবিগুলি + 7);
  encrypt_des(plaintext, results + 16, চাবিগুলি + 14);
#অন্যথা
  (শূন্য)চাবিগুলি;
  (শূন্য)plaintext;
  (শূন্য)results;
#endif
}

/*
 * Set up lanmanager hashed password
 */
CURLcode Curl_ntlm_core_mk_lm_hash(ধ্রুব বর্ণ *password,
                                   unsigned বর্ণ *lmbuffer /* 21 bytes */)
{
  unsigned বর্ণ pw[14];
  স্থির ধ্রুব unsigned বর্ণ magic[] = {
    0x4B, 0x47, 0x53, 0x21, 0x40, 0x23, 0x24, 0x25 /* i.e. KGS!@#$% */
  };
  size_t len = CURLMIN(strlen(password), 14);

  Curl_strntoupper((বর্ণ *)pw, password, len);
  memset(&pw[len], 0, 14 - len);

  {
    /* Create LanManager hashed password. */
#ifdef USE_OPENSSL_DES
    DES_চাবি_schedule ks;

    setup_des_চাবি(pw, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)lmbuffer, &ks, DES_ENCRYPT);

    setup_des_চাবি(pw + 7, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)(lmbuffer + 8), &ks, DES_ENCRYPT);
#নোচেদ্যদি defined(USE_WOLFSSL_DES)
    Des des;
    setup_des_চাবি(pw, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer, magic, DES_চাবি_SIZE);
    setup_des_চাবি(pw + 7, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer + 8, magic, DES_চাবি_SIZE);
#নোচেদ্যদি defined(USE_GNUTLS_DES)
    সংরচনা des_ctx des;
    setup_des_চাবি(pw, &des);
    des_encrypt(&des, 8, lmbuffer, magic);
    setup_des_চাবি(pw + 7, &des);
    des_encrypt(&des, 8, lmbuffer + 8, magic);
#নোচেদ্যদি defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
    encrypt_des(magic, lmbuffer, pw);
    encrypt_des(magic, lmbuffer + 8, pw + 7);
#endif

    memset(lmbuffer + 16, 0, 21 - 16);
  }

  প্রত্যাগম CURLE_OK;
}

স্থির শূন্য ascii_to_unicode_le(unsigned বর্ণ *dest, ধ্রুব বর্ণ *src,
                                size_t srclen)
{
  size_t i;
  প্রতি(i = 0; i < srclen; i++) {
    dest[2 * i] = (unsigned বর্ণ)src[i];
    dest[(2 * i) + 1] = '\0';
  }
}

/*
 * Set up nt hashed passwords
 * @unittest: 1600
 */
CURLcode Curl_ntlm_core_mk_nt_hash(ধ্রুব বর্ণ *password,
                                   unsigned বর্ণ *ntbuffer /* 21 bytes */)
{
  size_t len = strlen(password);
  unsigned বর্ণ *pw;
  CURLcode result;
  যদি(len > SIZE_MAX / 2) /* avoid integer overflow */
    প্রত্যাগম CURLE_OUT_OF_MEMORY;
  pw = len ? curlx_malloc(len * 2) : (unsigned বর্ণ *)curlx_strdup("");
  যদি(!pw)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  ascii_to_unicode_le(pw, password, len);

  /* Create NT hashed password. */
  result = Curl_md4it(ntbuffer, pw, 2 * len);
  যদি(!result)
    memset(ntbuffer + 16, 0, 21 - 16);

  curlx_free(pw);

  প্রত্যাগম result;
}

#ifndef USE_WINDOWS_SSPI

#define NTLMv2_BLOB_SIGNATURE "\x01\x01\x00\x00"
#define NTLMv2_BLOB_LEN       (44 - 16 + ntlm->target_info_len + 4)

/* Timestamp in tenths of a microsecond since January 1, 1601 00:00:00 UTC. */
সংরচনা ms_filetime {
  unsigned পূর্ণাঙ্ক dw_নিচের_Date_Time;
  unsigned পূর্ণাঙ্ক dw_উপরের_Date_Time;
};

/* Convert a time_t to an MS FILETIME (MS-DTYP section 2.3.3). */
স্থির শূন্য time2filetime(সংরচনা ms_filetime *ft, time_t t)
{
#যদি SIZEOF_TIME_T > 4
  t = (t + (curl_off_t)11644473600) * 10000000;
  ft->dw_নিচের_Date_Time = (unsigned পূর্ণাঙ্ক)(t & 0xFFFFFFFF);
  ft->dw_উপরের_Date_Time = (unsigned পূর্ণাঙ্ক)(t >> 32);
#অন্যথা
  unsigned পূর্ণাঙ্ক r, s;
  unsigned পূর্ণাঙ্ক i;

  ft->dw_নিচের_Date_Time = (unsigned পূর্ণাঙ্ক)(t & 0xFFFFFFFF);
  ft->dw_উপরের_Date_Time = 0;

#ifndef HAVE_TIME_T_UNSIGNED
  /* Extend sign if needed. */
  যদি(ft->dw_নিচের_Date_Time & 0x80000000)
    ft->dw_উপরের_Date_Time = ~(unsigned পূর্ণাঙ্ক)0;
#endif

  /* Bias seconds to Jan 1, 1601.
     134774 days = 11644473600 seconds = 0x2B6109100 */
  r = ft->dw_নিচের_Date_Time;
  ft->dw_নিচের_Date_Time = (ft->dw_নিচের_Date_Time + 0xB6109100U) & 0xFFFFFFFF;
  ft->dw_উপরের_Date_Time += ft->dw_নিচের_Date_Time < r ? 0x03 : 0x02;

  /* Convert to tenths of microseconds. */
  ft->dw_উপরের_Date_Time *= 10000000;
  i = 32;
  কুরু {
    i -= 8;
    s = ((ft->dw_নিচের_Date_Time >> i) & 0xFF) * (10000000 - 1);
    r = (s << i) & 0xFFFFFFFF;
    s >>= 1;   /* Split shift to avoid width overflow. */
    s >>= 31 - i;
    ft->dw_নিচের_Date_Time = (ft->dw_নিচের_Date_Time + r) & 0xFFFFFFFF;
    যদি(ft->dw_নিচের_Date_Time < r)
      s++;
    ft->dw_উপরের_Date_Time += s;
  } যাবত্(i);
  ft->dw_উপরের_Date_Time &= 0xFFFFFFFF;
#endif
}

স্থির শূন্য ascii_uppercase_to_unicode_le(unsigned বর্ণ *dest,
                                          ধ্রুব বর্ণ *src, size_t srclen)
{
  size_t i;
  প্রতি(i = 0; i < srclen; i++) {
    dest[2 * i] = (unsigned বর্ণ)(Curl_raw_toupper(src[i]));
    dest[(2 * i) + 1] = '\0';
  }
}

/* This creates the NTLMv2 hash by using NTLM hash as the key and Unicode
 * (uppercase UserName + Domain) as the data
 */
CURLcode Curl_ntlm_core_mk_ntlmv2_hash(ধ্রুব বর্ণ *user, size_t userlen,
                                       ধ্রুব বর্ণ *domain, size_t domlen,
                                       unsigned বর্ণ *ntlmhash,
                                       unsigned বর্ণ *ntlmv2hash)
{
  /* Unicode representation */
  size_t identity_len;
  unsigned বর্ণ *identity;
  CURLcode result = CURLE_OK;

  যদি((userlen > CURL_MAX_প্রবেশ_LENGTH) || (domlen > CURL_MAX_প্রবেশ_LENGTH))
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  identity_len = (userlen + domlen) * 2;
  identity = curlx_malloc(identity_len + 1);

  যদি(!identity)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  ascii_uppercase_to_unicode_le(identity, user, userlen);
  ascii_to_unicode_le(identity + (userlen << 1), domain, domlen);

  result = Curl_hmacit(&Curl_HMAC_MD5, ntlmhash, 16, identity, identity_len,
                       ntlmv2hash);
  curlx_free(identity);

  প্রত্যাগম result;
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
CURLcode Curl_ntlm_core_mk_ntlmv2_resp(ধ্রুব unsigned বর্ণ *ntlmv2hash,
                                       ধ্রুব unsigned বর্ণ *challenge_client,
                                       ধ্রুব সংরচনা ntlmdata *ntlm,
                                       unsigned বর্ণ **ntresp,
                                       unsigned পূর্ণাঙ্ক *ntresp_len)
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

  unsigned পূর্ণাঙ্ক len = 0;
  unsigned বর্ণ *ptr = রিক্ত;
  unsigned বর্ণ hmac_output[HMAC_MD5_LENGTH];
  সংরচনা ms_filetime tw;

  CURLcode result = CURLE_OK;

  /* Calculate the timestamp */
#ifdef DEBUGBUILD
  বর্ণ *force_timestamp = getenv("CURL_FORCETIME");
  যদি(force_timestamp)
    time2filetime(&tw, (time_t)0);
  অন্যথা
#endif
    time2filetime(&tw, time(রিক্ত));

  /* Calculate the response len */
  len = HMAC_MD5_LENGTH + NTLMv2_BLOB_LEN;

  /* Allocate the response */
  ptr = curlx_calloc(1, len);
  যদি(!ptr)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  /* Create the BLOB structure */
  curl_msnprintf((বর্ণ *)ptr + HMAC_MD5_LENGTH, NTLMv2_BLOB_LEN,
                 "%c%c%c%c"           /* NTLMv2_BLOB_SIGNATURE */
                 "%c%c%c%c"           /* Reserved = 0 */
                 "%c%c%c%c%c%c%c%c",  /* Timestamp */
                 NTLMv2_BLOB_SIGNATURE[0], NTLMv2_BLOB_SIGNATURE[1],
                 NTLMv2_BLOB_SIGNATURE[2], NTLMv2_BLOB_SIGNATURE[3],
                 0, 0, 0, 0,
                 LONGQUARTET(tw.dw_নিচের_Date_Time),
                 LONGQUARTET(tw.dw_উপরের_Date_Time));

  memcpy(ptr + 32, challenge_client, 8);
  যদি(ntlm->target_info_len)
    memcpy(ptr + 44, ntlm->target_info, ntlm->target_info_len);

  /* Concatenate the Type 2 challenge with the BLOB and do HMAC MD5 */
  memcpy(ptr + 8, &ntlm->nonce[0], 8);
  result = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, HMAC_MD5_LENGTH, ptr + 8,
                       NTLMv2_BLOB_LEN + 8, hmac_output);
  যদি(result) {
    curlx_free(ptr);
    প্রত্যাগম result;
  }

  /* Concatenate the HMAC MD5 output  with the BLOB */
  memcpy(ptr, hmac_output, HMAC_MD5_LENGTH);

  /* Return the response */
  *ntresp = ptr;
  *ntresp_len = len;

  প্রত্যাগম result;
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
CURLcode Curl_ntlm_core_mk_lmv2_resp(ধ্রুব unsigned বর্ণ *ntlmv2hash,
                                     ধ্রুব unsigned বর্ণ *challenge_client,
                                     ধ্রুব unsigned বর্ণ *challenge_server,
                                     unsigned বর্ণ *lmresp)
{
  unsigned বর্ণ data[16];
  unsigned বর্ণ hmac_output[16];
  CURLcode result = CURLE_OK;

  memcpy(&data[0], challenge_server, 8);
  memcpy(&data[8], challenge_client, 8);

  result = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, 16, &data[0], 16,
                       hmac_output);
  যদি(result)
    প্রত্যাগম result;

  /* Concatenate the HMAC MD5 output with the client nonce */
  memcpy(lmresp, hmac_output, 16);
  memcpy(lmresp + 16, challenge_client, 8);

  প্রত্যাগম result;
}

#endif /* !USE_WINDOWS_SSPI */

#endif /* USE_CURL_NTLM_CORE */
