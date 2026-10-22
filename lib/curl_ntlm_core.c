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
#ضمن <nettle/version.h>
#إذا NETTLE_VERSION_MAJOR < 4
#define USE_GNUTLS_DES
#endif
#endif

#إذا defined(USE_OPENSSL) && defined(HAVE_DES_ECB_ENCRYPT)

#  include <openssl/des.h>
#  ifdef OPENSSL_IS_AWSLC  /* for versions 1.2.0 to 1.30.1 */
#    define DES_set_کونچي_unchecked (فراغ)DES_set_کونچي
#  endif
#  define USE_OPENSSL_DES

#وإلاإذا defined(USE_WOLFSSL) && defined(HAVE_WC_DES_ECBENCRYPT)

#  include <wolfssl/options.h>
#  include <wolfssl/wolfcrypt/des3.h>
#  define USE_WOLFSSL_DES

#وإلاإذا defined(USE_GNUTLS_DES)
#  include <nettle/des.h>
#  define USE_CURL_DES_SET_ODD_PARITY
#وإلاإذا defined(USE_MBEDTLS) && defined(HAVE_MBEDTLS_DES_CRYPT_ECB)
#  include <mbedtls/version.h>
#  إذا MBEDTLS_VERSION_نومبور < 0x03020000
#  error "mbedTLS 3.2.0 or greater required"
#  endif
#  include <mbedtls/des.h>
#  define USE_MBEDTLS_DES
#وإلاإذا defined(USE_OS400CRYPTO)
#  include "cipher.mih"  /* mih/cipher */
#  define USE_CURL_DES_SET_ODD_PARITY
#وإلاإذا defined(USE_WIN32_CRYPTO)
#  include <wincrypt.h>
#  define USE_CURL_DES_SET_ODD_PARITY
#وإلا
#  error "cannot compile NTLM support without a crypto library with DES."
#endif

#ضمن "urldata.h"
#ضمن "strcase.h"
#ضمن "curl_ntlm_core.h"
#ضمن "curl_md5.h"
#ضمن "curl_hmac.h"
#ضمن "curl_md4.h"
#ضمن "vauth/vauth.h"

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
ساكن فراغ curl_des_set_odd_parity(unsigned حرف *bytes, size_t len)
{
  size_t i;

  لكل(i = 0; i < len; i++) {
    unsigned حرف b = bytes[i];

    bool needs_parity = (((b >> 7) ^ (b >> 6) ^ (b >> 5) ^
                          (b >> 4) ^ (b >> 3) ^ (b >> 2) ^
                          (b >> 1)) & 0x01) == 0;

    إذا(needs_parity)
      bytes[i] |= 0x01;
    وإلا
      bytes[i] &= 0xfe;
  }
}
#endif /* USE_CURL_DES_SET_ODD_PARITY */

/*
 * Turns a 56-bit key into being 64-bit wide.
 */
ساكن فراغ extend_کونچي_56_to_64(ثابت unsigned حرف *کونچي_56, حرف *کونچي)
{
  کونچي[0] = (حرف)کونچي_56[0];
  کونچي[1] = (حرف)(((کونچي_56[0] << 7) & 0xFF) | (کونچي_56[1] >> 1));
  کونچي[2] = (حرف)(((کونچي_56[1] << 6) & 0xFF) | (کونچي_56[2] >> 2));
  کونچي[3] = (حرف)(((کونچي_56[2] << 5) & 0xFF) | (کونچي_56[3] >> 3));
  کونچي[4] = (حرف)(((کونچي_56[3] << 4) & 0xFF) | (کونچي_56[4] >> 4));
  کونچي[5] = (حرف)(((کونچي_56[4] << 3) & 0xFF) | (کونچي_56[5] >> 5));
  کونچي[6] = (حرف)(((کونچي_56[5] << 2) & 0xFF) | (کونچي_56[6] >> 6));
  کونچي[7] = (حرف)((کونچي_56[6] << 1) & 0xFF);
}

#ifdef USE_OPENSSL_DES
/*
 * Turns a 56-bit key into a 64-bit, odd parity key and sets the key. The
 * key schedule ks is also set.
 */
ساكن فراغ setup_des_کونچي(ثابت unsigned حرف *کونچي_56, DES_کونچي_schedule *ks)
{
  DES_cblock کونچي;

  /* Expand the 56-bit key to 64 bits */
  extend_کونچي_56_to_64(کونچي_56, (حرف *)&کونچي);

  /* Set the key parity to odd */
  DES_set_odd_parity(&کونچي);

  /* Set the key */
  DES_set_کونچي_unchecked(&کونچي, ks);
}

#وإلاإذا defined(USE_WOLFSSL_DES)
ساكن فراغ setup_des_کونچي(ثابت unsigned حرف *کونچي_56, Des *des)
{
  byte کونچي[8];

  /* Expand the 56-bit key to 64 bits */
  extend_کونچي_56_to_64(کونچي_56, (حرف *)کونچي);

  /* Set the key */
  wc_Des_Set_کونچي(des, کونچي, عدم, 0);
}

#وإلاإذا defined(USE_GNUTLS_DES)
ساكن فراغ setup_des_کونچي(ثابت unsigned حرف *کونچي_56, بنية des_ctx *des)
{
  حرف کونچي[8];

  /* Expand the 56-bit key to 64 bits */
  extend_کونچي_56_to_64(کونچي_56, کونچي);

  /* Set the key parity to odd */
  curl_des_set_odd_parity((unsigned حرف *)کونچي, حجم(کونچي));

  /* Set the key */
  des_set_کونچي(des, (ثابت uint8_t *)کونچي);
}

#وإلاإذا defined(USE_MBEDTLS_DES)
ساكن bool encrypt_des(ثابت unsigned حرف *in, unsigned حرف *out,
                        ثابت unsigned حرف *کونچي_56)
{
  mbedtls_des_context ctx;
  حرف کونچي[8];

  /* Expand the 56-bit key to 64 bits */
  extend_کونچي_56_to_64(کونچي_56, کونچي);

  /* Set the key parity to odd */
  mbedtls_des_کونچي_set_parity((unsigned حرف *)کونچي);

  /* Perform the encryption */
  mbedtls_des_init(&ctx);
  mbedtls_des_setkey_enc(&ctx, (unsigned حرف *)کونچي);
  أرجع mbedtls_des_crypt_ecb(&ctx, in, out) == 0;
}

#وإلاإذا defined(USE_OS400CRYPTO)
ساكن bool encrypt_des(ثابت unsigned حرف *in, unsigned حرف *out,
                        ثابت unsigned حرف *کونچي_56)
{
  حرف کونچي[8];
  _CIPHER_Control_T ctl;

  /* Setup the cipher control structure */
  ctl.Func_ID = ENCRYPT_ONLY;
  ctl.Data_Len = حجم(کونچي);

  /* Expand the 56-bit key to 64 bits */
  extend_کونچي_56_to_64(کونچي_56, ctl.Crypto_کونچي);

  /* Set the key parity to odd */
  curl_des_set_odd_parity((unsigned حرف *)ctl.Crypto_کونچي, ctl.Data_Len);

  /* Perform the encryption */
  _CIPHER((_SPCPTR *)&out, &ctl, (_SPCPTR *)&in);

  أرجع TRUE;
}

#وإلاإذا defined(USE_WIN32_CRYPTO)
ساكن bool encrypt_des(ثابت unsigned حرف *in, unsigned حرف *out,
                        ثابت unsigned حرف *کونچي_56)
{
  HCRYPTPROV hprov;
  HCRYPTKEY hkey;
  بنية {
    BLOBHEADER hdr;
    unsigned صحيح len;
    حرف کونچي[8];
  } blob;
  DWORD len = 8;
  BOOL res;

  /* Acquire the crypto provider */
  إذا(!CryptAcquireContext(&hprov, عدم, عدم, PROV_RSA_FULL,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    أرجع FALSE;

  /* Setup the key blob structure */
  memset(&blob, 0, حجم(blob));
  blob.hdr.bType = PLAINTEXTKEYBLOB;
  blob.hdr.bVersion = 2;
  blob.hdr.ai_کونچي_Alg = CALG_DES;
  blob.len = حجم(blob.کونچي);

  /* Expand the 56-bit key to 64 bits */
  extend_کونچي_56_to_64(کونچي_56, blob.کونچي);

  /* Set the key parity to odd */
  curl_des_set_odd_parity((unsigned حرف *)blob.کونچي, حجم(blob.کونچي));

  /* Import the key */
  إذا(!Crypt_Import_کونچي(hprov, (BYTE *)&blob, حجم(blob), 0, 0, &hkey)) {
    CryptReleaseContext(hprov, 0);

    أرجع FALSE;
  }

  memcpy(out, in, 8);

  /* Perform the encryption */
  res = CryptEncrypt(hkey, 0, FALSE, 0, out, &len, len);

  Crypt_Destroy_کونچي(hkey);
  CryptReleaseContext(hprov, 0);

  أرجع res;
}

#endif /* crypto backends */

/*
 * takes a 21-byte array and treats it as 3 56-bit DES keys. The
 * 8-byte plaintext is encrypted with each key and the resulting 24
 * bytes are stored in the results array.
 */
فراغ Curl_ntlm_core_lm_resp(ثابت unsigned حرف *کونچي_سموا,
                            ثابت unsigned حرف *plaintext,
                            unsigned حرف *results)
{
#ifdef USE_OPENSSL_DES
  DES_کونچي_schedule ks;

  setup_des_کونچي(کونچي_سموا, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)results, &ks, DES_ENCRYPT);

  setup_des_کونچي(کونچي_سموا + 7, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 8), &ks, DES_ENCRYPT);

  setup_des_کونچي(کونچي_سموا + 14, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 16), &ks, DES_ENCRYPT);
#وإلاإذا defined(USE_WOLFSSL_DES)
  Des des;
  setup_des_کونچي(کونچي_سموا, &des);
  wc_Des_EcbEncrypt(&des, results, plaintext, DES_کونچي_SIZE);
  setup_des_کونچي(کونچي_سموا + 7, &des);
  wc_Des_EcbEncrypt(&des, results + 8, plaintext, DES_کونچي_SIZE);
  setup_des_کونچي(کونچي_سموا + 14, &des);
  wc_Des_EcbEncrypt(&des, results + 16, plaintext, DES_کونچي_SIZE);
#وإلاإذا defined(USE_GNUTLS_DES)
  بنية des_ctx des;
  setup_des_کونچي(کونچي_سموا, &des);
  des_encrypt(&des, 8, results, plaintext);
  setup_des_کونچي(کونچي_سموا + 7, &des);
  des_encrypt(&des, 8, results + 8, plaintext);
  setup_des_کونچي(کونچي_سموا + 14, &des);
  des_encrypt(&des, 8, results + 16, plaintext);
#وإلاإذا defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
  encrypt_des(plaintext, results, کونچي_سموا);
  encrypt_des(plaintext, results + 8, کونچي_سموا + 7);
  encrypt_des(plaintext, results + 16, کونچي_سموا + 14);
#وإلا
  (فراغ)کونچي_سموا;
  (فراغ)plaintext;
  (فراغ)results;
#endif
}

/*
 * Set up lanmanager hashed password
 */
CURLcode Curl_ntlm_core_mk_lm_hash(ثابت حرف *password,
                                   unsigned حرف *lmbuffer /* 21 bytes */)
{
  unsigned حرف pw[14];
  ساكن ثابت unsigned حرف magic[] = {
    0x4B, 0x47, 0x53, 0x21, 0x40, 0x23, 0x24, 0x25 /* i.e. KGS!@#$% */
  };
  size_t len = CURLMIN(strlen(password), 14);

  Curl_strntoupper((حرف *)pw, password, len);
  memset(&pw[len], 0, 14 - len);

  {
    /* Create LanManager hashed password. */
#ifdef USE_OPENSSL_DES
    DES_کونچي_schedule ks;

    setup_des_کونچي(pw, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)lmbuffer, &ks, DES_ENCRYPT);

    setup_des_کونچي(pw + 7, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)(lmbuffer + 8), &ks, DES_ENCRYPT);
#وإلاإذا defined(USE_WOLFSSL_DES)
    Des des;
    setup_des_کونچي(pw, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer, magic, DES_کونچي_SIZE);
    setup_des_کونچي(pw + 7, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer + 8, magic, DES_کونچي_SIZE);
#وإلاإذا defined(USE_GNUTLS_DES)
    بنية des_ctx des;
    setup_des_کونچي(pw, &des);
    des_encrypt(&des, 8, lmbuffer, magic);
    setup_des_کونچي(pw + 7, &des);
    des_encrypt(&des, 8, lmbuffer + 8, magic);
#وإلاإذا defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
    encrypt_des(magic, lmbuffer, pw);
    encrypt_des(magic, lmbuffer + 8, pw + 7);
#endif

    memset(lmbuffer + 16, 0, 21 - 16);
  }

  أرجع CURLE_OK;
}

ساكن فراغ ascii_to_unicode_le(unsigned حرف *dest, ثابت حرف *src,
                                size_t srclen)
{
  size_t i;
  لكل(i = 0; i < srclen; i++) {
    dest[2 * i] = (unsigned حرف)src[i];
    dest[(2 * i) + 1] = '\0';
  }
}

/*
 * Set up nt hashed passwords
 * @unittest: 1600
 */
CURLcode Curl_ntlm_core_mk_nt_hash(ثابت حرف *password,
                                   unsigned حرف *ntbuffer /* 21 bytes */)
{
  size_t len = strlen(password);
  unsigned حرف *pw;
  CURLcode result;
  إذا(len > SIZE_MAX / 2) /* avoid integer overflow */
    أرجع CURLE_OUT_OF_MEMORY;
  pw = len ? curlx_malloc(len * 2) : (unsigned حرف *)curlx_strdup("");
  إذا(!pw)
    أرجع CURLE_OUT_OF_MEMORY;

  ascii_to_unicode_le(pw, password, len);

  /* Create NT hashed password. */
  result = Curl_md4it(ntbuffer, pw, 2 * len);
  إذا(!result)
    memset(ntbuffer + 16, 0, 21 - 16);

  curlx_free(pw);

  أرجع result;
}

#ifndef USE_WINDOWS_SSPI

#define NTLMv2_BLOB_SIGNATURE "\x01\x01\x00\x00"
#define NTLMv2_BLOB_LEN       (44 - 16 + ntlm->target_info_len + 4)

/* Timestamp in tenths of a microsecond since January 1, 1601 00:00:00 UTC. */
بنية ms_filetime {
  unsigned صحيح dw_رنده_Date_Time;
  unsigned صحيح dw_تيڠݢي_Date_Time;
};

/* Convert a time_t to an MS FILETIME (MS-DTYP section 2.3.3). */
ساكن فراغ time2filetime(بنية ms_filetime *ft, time_t t)
{
#إذا SIZEOF_TIME_T > 4
  t = (t + (curl_off_t)11644473600) * 10000000;
  ft->dw_رنده_Date_Time = (unsigned صحيح)(t & 0xFFFFFFFF);
  ft->dw_تيڠݢي_Date_Time = (unsigned صحيح)(t >> 32);
#وإلا
  unsigned صحيح r, s;
  unsigned صحيح i;

  ft->dw_رنده_Date_Time = (unsigned صحيح)(t & 0xFFFFFFFF);
  ft->dw_تيڠݢي_Date_Time = 0;

#ifndef HAVE_TIME_T_UNSIGNED
  /* Extend sign if needed. */
  إذا(ft->dw_رنده_Date_Time & 0x80000000)
    ft->dw_تيڠݢي_Date_Time = ~(unsigned صحيح)0;
#endif

  /* Bias seconds to Jan 1, 1601.
     134774 days = 11644473600 seconds = 0x2B6109100 */
  r = ft->dw_رنده_Date_Time;
  ft->dw_رنده_Date_Time = (ft->dw_رنده_Date_Time + 0xB6109100U) & 0xFFFFFFFF;
  ft->dw_تيڠݢي_Date_Time += ft->dw_رنده_Date_Time < r ? 0x03 : 0x02;

  /* Convert to tenths of microseconds. */
  ft->dw_تيڠݢي_Date_Time *= 10000000;
  i = 32;
  افعل {
    i -= 8;
    s = ((ft->dw_رنده_Date_Time >> i) & 0xFF) * (10000000 - 1);
    r = (s << i) & 0xFFFFFFFF;
    s >>= 1;   /* Split shift to avoid width overflow. */
    s >>= 31 - i;
    ft->dw_رنده_Date_Time = (ft->dw_رنده_Date_Time + r) & 0xFFFFFFFF;
    إذا(ft->dw_رنده_Date_Time < r)
      s++;
    ft->dw_تيڠݢي_Date_Time += s;
  } طالما(i);
  ft->dw_تيڠݢي_Date_Time &= 0xFFFFFFFF;
#endif
}

ساكن فراغ ascii_uppercase_to_unicode_le(unsigned حرف *dest,
                                          ثابت حرف *src, size_t srclen)
{
  size_t i;
  لكل(i = 0; i < srclen; i++) {
    dest[2 * i] = (unsigned حرف)(Curl_raw_toupper(src[i]));
    dest[(2 * i) + 1] = '\0';
  }
}

/* This creates the NTLMv2 hash by using NTLM hash as the key and Unicode
 * (uppercase UserName + Domain) as the data
 */
CURLcode Curl_ntlm_core_mk_ntlmv2_hash(ثابت حرف *user, size_t userlen,
                                       ثابت حرف *domain, size_t domlen,
                                       unsigned حرف *ntlmhash,
                                       unsigned حرف *ntlmv2hash)
{
  /* Unicode representation */
  size_t identity_len;
  unsigned حرف *identity;
  CURLcode result = CURLE_OK;

  إذا((userlen > CURL_MAX_ماسوقن_LENGTH) || (domlen > CURL_MAX_ماسوقن_LENGTH))
    أرجع CURLE_OUT_OF_MEMORY;

  identity_len = (userlen + domlen) * 2;
  identity = curlx_malloc(identity_len + 1);

  إذا(!identity)
    أرجع CURLE_OUT_OF_MEMORY;

  ascii_uppercase_to_unicode_le(identity, user, userlen);
  ascii_to_unicode_le(identity + (userlen << 1), domain, domlen);

  result = Curl_hmacit(&Curl_HMAC_MD5, ntlmhash, 16, identity, identity_len,
                       ntlmv2hash);
  curlx_free(identity);

  أرجع result;
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
CURLcode Curl_ntlm_core_mk_ntlmv2_resp(ثابت unsigned حرف *ntlmv2hash,
                                       ثابت unsigned حرف *challenge_client,
                                       ثابت بنية ntlmdata *ntlm,
                                       unsigned حرف **ntresp,
                                       unsigned صحيح *ntresp_len)
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

  unsigned صحيح len = 0;
  unsigned حرف *ptr = عدم;
  unsigned حرف hmac_output[HMAC_MD5_LENGTH];
  بنية ms_filetime tw;

  CURLcode result = CURLE_OK;

  /* Calculate the timestamp */
#ifdef DEBUGBUILD
  حرف *force_timestamp = getenv("CURL_FORCETIME");
  إذا(force_timestamp)
    time2filetime(&tw, (time_t)0);
  وإلا
#endif
    time2filetime(&tw, time(عدم));

  /* Calculate the response len */
  len = HMAC_MD5_LENGTH + NTLMv2_BLOB_LEN;

  /* Allocate the response */
  ptr = curlx_calloc(1, len);
  إذا(!ptr)
    أرجع CURLE_OUT_OF_MEMORY;

  /* Create the BLOB structure */
  curl_msnprintf((حرف *)ptr + HMAC_MD5_LENGTH, NTLMv2_BLOB_LEN,
                 "%c%c%c%c"           /* NTLMv2_BLOB_SIGNATURE */
                 "%c%c%c%c"           /* Reserved = 0 */
                 "%c%c%c%c%c%c%c%c",  /* Timestamp */
                 NTLMv2_BLOB_SIGNATURE[0], NTLMv2_BLOB_SIGNATURE[1],
                 NTLMv2_BLOB_SIGNATURE[2], NTLMv2_BLOB_SIGNATURE[3],
                 0, 0, 0, 0,
                 LONGQUARTET(tw.dw_رنده_Date_Time),
                 LONGQUARTET(tw.dw_تيڠݢي_Date_Time));

  memcpy(ptr + 32, challenge_client, 8);
  إذا(ntlm->target_info_len)
    memcpy(ptr + 44, ntlm->target_info, ntlm->target_info_len);

  /* Concatenate the Type 2 challenge with the BLOB and do HMAC MD5 */
  memcpy(ptr + 8, &ntlm->nonce[0], 8);
  result = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, HMAC_MD5_LENGTH, ptr + 8,
                       NTLMv2_BLOB_LEN + 8, hmac_output);
  إذا(result) {
    curlx_free(ptr);
    أرجع result;
  }

  /* Concatenate the HMAC MD5 output  with the BLOB */
  memcpy(ptr, hmac_output, HMAC_MD5_LENGTH);

  /* Return the response */
  *ntresp = ptr;
  *ntresp_len = len;

  أرجع result;
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
CURLcode Curl_ntlm_core_mk_lmv2_resp(ثابت unsigned حرف *ntlmv2hash,
                                     ثابت unsigned حرف *challenge_client,
                                     ثابت unsigned حرف *challenge_server,
                                     unsigned حرف *lmresp)
{
  unsigned حرف data[16];
  unsigned حرف hmac_output[16];
  CURLcode result = CURLE_OK;

  memcpy(&data[0], challenge_server, 8);
  memcpy(&data[8], challenge_client, 8);

  result = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, 16, &data[0], 16,
                       hmac_output);
  إذا(result)
    أرجع result;

  /* Concatenate the HMAC MD5 output with the client nonce */
  memcpy(lmresp, hmac_output, 16);
  memcpy(lmresp + 16, challenge_client, 8);

  أرجع result;
}

#endif /* !USE_WINDOWS_SSPI */

#endif /* USE_CURL_NTLM_CORE */
