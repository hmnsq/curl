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
#અન્તર્ભાવ "curl_setup.h"

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
#અન્તર્ભાવ <nettle/version.h>
#જો NETTLE_VERSION_MAJOR < 4
#define USE_GNUTLS_DES
#endif
#endif

#જો defined(USE_OPENSSL) && defined(HAVE_DES_ECB_ENCRYPT)

#  include <openssl/des.h>
#  ifdef OPENSSL_IS_AWSLC  /* for versions 1.2.0 to 1.30.1 */
#    define DES_set_ચાવી_unchecked (શૂન્ય)DES_set_ચાવી
#  endif
#  define USE_OPENSSL_DES

#અથવાજો defined(USE_WOLFSSL) && defined(HAVE_WC_DES_ECBENCRYPT)

#  include <wolfssl/options.h>
#  include <wolfssl/wolfcrypt/des3.h>
#  define USE_WOLFSSL_DES

#અથવાજો defined(USE_GNUTLS_DES)
#  include <nettle/des.h>
#  define USE_CURL_DES_SET_ODD_PARITY
#અથવાજો defined(USE_MBEDTLS) && defined(HAVE_MBEDTLS_DES_CRYPT_ECB)
#  include <mbedtls/version.h>
#  જો MBEDTLS_VERSION_સંખ્યા < 0x03020000
#  error "mbedTLS 3.2.0 or greater required"
#  endif
#  include <mbedtls/des.h>
#  define USE_MBEDTLS_DES
#અથવાજો defined(USE_OS400CRYPTO)
#  include "cipher.mih"  /* mih/cipher */
#  define USE_CURL_DES_SET_ODD_PARITY
#અથવાજો defined(USE_WIN32_CRYPTO)
#  include <wincrypt.h>
#  define USE_CURL_DES_SET_ODD_PARITY
#નહીંતર
#  error "cannot compile NTLM support without a crypto library with DES."
#endif

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "strcase.h"
#અન્તર્ભાવ "curl_ntlm_core.h"
#અન્તર્ભાવ "curl_md5.h"
#અન્તર્ભાવ "curl_hmac.h"
#અન્તર્ભાવ "curl_md4.h"
#અન્તર્ભાવ "vauth/vauth.h"

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
સ્થિર શૂન્ય curl_des_set_odd_parity(unsigned અક્ષર *bytes, size_t len)
{
  size_t i;

  માટે(i = 0; i < len; i++) {
    unsigned અક્ષર b = bytes[i];

    bool needs_parity = (((b >> 7) ^ (b >> 6) ^ (b >> 5) ^
                          (b >> 4) ^ (b >> 3) ^ (b >> 2) ^
                          (b >> 1)) & 0x01) == 0;

    જો(needs_parity)
      bytes[i] |= 0x01;
    નહીંતર
      bytes[i] &= 0xfe;
  }
}
#endif /* USE_CURL_DES_SET_ODD_PARITY */

/*
 * Turns a 56-bit key into being 64-bit wide.
 */
સ્થિર શૂન્ય extend_ચાવી_56_to_64(અચલ unsigned અક્ષર *ચાવી_56, અક્ષર *ચાવી)
{
  ચાવી[0] = (અક્ષર)ચાવી_56[0];
  ચાવી[1] = (અક્ષર)(((ચાવી_56[0] << 7) & 0xFF) | (ચાવી_56[1] >> 1));
  ચાવી[2] = (અક્ષર)(((ચાવી_56[1] << 6) & 0xFF) | (ચાવી_56[2] >> 2));
  ચાવી[3] = (અક્ષર)(((ચાવી_56[2] << 5) & 0xFF) | (ચાવી_56[3] >> 3));
  ચાવી[4] = (અક્ષર)(((ચાવી_56[3] << 4) & 0xFF) | (ચાવી_56[4] >> 4));
  ચાવી[5] = (અક્ષર)(((ચાવી_56[4] << 3) & 0xFF) | (ચાવી_56[5] >> 5));
  ચાવી[6] = (અક્ષર)(((ચાવી_56[5] << 2) & 0xFF) | (ચાવી_56[6] >> 6));
  ચાવી[7] = (અક્ષર)((ચાવી_56[6] << 1) & 0xFF);
}

#ifdef USE_OPENSSL_DES
/*
 * Turns a 56-bit key into a 64-bit, odd parity key and sets the key. The
 * key schedule ks is also set.
 */
સ્થિર શૂન્ય setup_des_ચાવી(અચલ unsigned અક્ષર *ચાવી_56, DES_ચાવી_schedule *ks)
{
  DES_cblock ચાવી;

  /* Expand the 56-bit key to 64 bits */
  extend_ચાવી_56_to_64(ચાવી_56, (અક્ષર *)&ચાવી);

  /* Set the key parity to odd */
  DES_set_odd_parity(&ચાવી);

  /* Set the key */
  DES_set_ચાવી_unchecked(&ચાવી, ks);
}

#અથવાજો defined(USE_WOLFSSL_DES)
સ્થિર શૂન્ય setup_des_ચાવી(અચલ unsigned અક્ષર *ચાવી_56, Des *des)
{
  byte ચાવી[8];

  /* Expand the 56-bit key to 64 bits */
  extend_ચાવી_56_to_64(ચાવી_56, (અક્ષર *)ચાવી);

  /* Set the key */
  wc_Des_Set_ચાવી(des, ચાવી, ખાલી, 0);
}

#અથવાજો defined(USE_GNUTLS_DES)
સ્થિર શૂન્ય setup_des_ચાવી(અચલ unsigned અક્ષર *ચાવી_56, સંરચના des_ctx *des)
{
  અક્ષર ચાવી[8];

  /* Expand the 56-bit key to 64 bits */
  extend_ચાવી_56_to_64(ચાવી_56, ચાવી);

  /* Set the key parity to odd */
  curl_des_set_odd_parity((unsigned અક્ષર *)ચાવી, કદ(ચાવી));

  /* Set the key */
  des_set_ચાવી(des, (અચલ uint8_t *)ચાવી);
}

#અથવાજો defined(USE_MBEDTLS_DES)
સ્થિર bool encrypt_des(અચલ unsigned અક્ષર *in, unsigned અક્ષર *out,
                        અચલ unsigned અક્ષર *ચાવી_56)
{
  mbedtls_des_context ctx;
  અક્ષર ચાવી[8];

  /* Expand the 56-bit key to 64 bits */
  extend_ચાવી_56_to_64(ચાવી_56, ચાવી);

  /* Set the key parity to odd */
  mbedtls_des_ચાવી_set_parity((unsigned અક્ષર *)ચાવી);

  /* Perform the encryption */
  mbedtls_des_init(&ctx);
  mbedtls_des_setkey_enc(&ctx, (unsigned અક્ષર *)ચાવી);
  પાછા mbedtls_des_crypt_ecb(&ctx, in, out) == 0;
}

#અથવાજો defined(USE_OS400CRYPTO)
સ્થિર bool encrypt_des(અચલ unsigned અક્ષર *in, unsigned અક્ષર *out,
                        અચલ unsigned અક્ષર *ચાવી_56)
{
  અક્ષર ચાવી[8];
  _CIPHER_Control_T ctl;

  /* Setup the cipher control structure */
  ctl.Func_ID = ENCRYPT_ONLY;
  ctl.Data_Len = કદ(ચાવી);

  /* Expand the 56-bit key to 64 bits */
  extend_ચાવી_56_to_64(ચાવી_56, ctl.Crypto_ચાવી);

  /* Set the key parity to odd */
  curl_des_set_odd_parity((unsigned અક્ષર *)ctl.Crypto_ચાવી, ctl.Data_Len);

  /* Perform the encryption */
  _CIPHER((_SPCPTR *)&out, &ctl, (_SPCPTR *)&in);

  પાછા TRUE;
}

#અથવાજો defined(USE_WIN32_CRYPTO)
સ્થિર bool encrypt_des(અચલ unsigned અક્ષર *in, unsigned અક્ષર *out,
                        અચલ unsigned અક્ષર *ચાવી_56)
{
  HCRYPTPROV hprov;
  HCRYPTKEY hkey;
  સંરચના {
    BLOBHEADER hdr;
    unsigned પૂર્ણાંક len;
    અક્ષર ચાવી[8];
  } blob;
  DWORD len = 8;
  BOOL res;

  /* Acquire the crypto provider */
  જો(!CryptAcquireContext(&hprov, ખાલી, ખાલી, PROV_RSA_FULL,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    પાછા FALSE;

  /* Setup the key blob structure */
  memset(&blob, 0, કદ(blob));
  blob.hdr.bType = PLAINTEXTKEYBLOB;
  blob.hdr.bVersion = 2;
  blob.hdr.ai_ચાવી_Alg = CALG_DES;
  blob.len = કદ(blob.ચાવી);

  /* Expand the 56-bit key to 64 bits */
  extend_ચાવી_56_to_64(ચાવી_56, blob.ચાવી);

  /* Set the key parity to odd */
  curl_des_set_odd_parity((unsigned અક્ષર *)blob.ચાવી, કદ(blob.ચાવી));

  /* Import the key */
  જો(!Crypt_Import_ચાવી(hprov, (BYTE *)&blob, કદ(blob), 0, 0, &hkey)) {
    CryptReleaseContext(hprov, 0);

    પાછા FALSE;
  }

  memcpy(out, in, 8);

  /* Perform the encryption */
  res = CryptEncrypt(hkey, 0, FALSE, 0, out, &len, len);

  Crypt_Destroy_ચાવી(hkey);
  CryptReleaseContext(hprov, 0);

  પાછા res;
}

#endif /* crypto backends */

/*
 * takes a 21-byte array and treats it as 3 56-bit DES keys. The
 * 8-byte plaintext is encrypted with each key and the resulting 24
 * bytes are stored in the results array.
 */
શૂન્ય Curl_ntlm_core_lm_resp(અચલ unsigned અક્ષર *ચાવીઓ,
                            અચલ unsigned અક્ષર *plaintext,
                            unsigned અક્ષર *results)
{
#ifdef USE_OPENSSL_DES
  DES_ચાવી_schedule ks;

  setup_des_ચાવી(ચાવીઓ, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)results, &ks, DES_ENCRYPT);

  setup_des_ચાવી(ચાવીઓ + 7, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 8), &ks, DES_ENCRYPT);

  setup_des_ચાવી(ચાવીઓ + 14, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 16), &ks, DES_ENCRYPT);
#અથવાજો defined(USE_WOLFSSL_DES)
  Des des;
  setup_des_ચાવી(ચાવીઓ, &des);
  wc_Des_EcbEncrypt(&des, results, plaintext, DES_ચાવી_SIZE);
  setup_des_ચાવી(ચાવીઓ + 7, &des);
  wc_Des_EcbEncrypt(&des, results + 8, plaintext, DES_ચાવી_SIZE);
  setup_des_ચાવી(ચાવીઓ + 14, &des);
  wc_Des_EcbEncrypt(&des, results + 16, plaintext, DES_ચાવી_SIZE);
#અથવાજો defined(USE_GNUTLS_DES)
  સંરચના des_ctx des;
  setup_des_ચાવી(ચાવીઓ, &des);
  des_encrypt(&des, 8, results, plaintext);
  setup_des_ચાવી(ચાવીઓ + 7, &des);
  des_encrypt(&des, 8, results + 8, plaintext);
  setup_des_ચાવી(ચાવીઓ + 14, &des);
  des_encrypt(&des, 8, results + 16, plaintext);
#અથવાજો defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
  encrypt_des(plaintext, results, ચાવીઓ);
  encrypt_des(plaintext, results + 8, ચાવીઓ + 7);
  encrypt_des(plaintext, results + 16, ચાવીઓ + 14);
#નહીંતર
  (શૂન્ય)ચાવીઓ;
  (શૂન્ય)plaintext;
  (શૂન્ય)results;
#endif
}

/*
 * Set up lanmanager hashed password
 */
CURLcode Curl_ntlm_core_mk_lm_hash(અચલ અક્ષર *password,
                                   unsigned અક્ષર *lmbuffer /* 21 bytes */)
{
  unsigned અક્ષર pw[14];
  સ્થિર અચલ unsigned અક્ષર magic[] = {
    0x4B, 0x47, 0x53, 0x21, 0x40, 0x23, 0x24, 0x25 /* i.e. KGS!@#$% */
  };
  size_t len = CURLMIN(strlen(password), 14);

  Curl_strntoupper((અક્ષર *)pw, password, len);
  memset(&pw[len], 0, 14 - len);

  {
    /* Create LanManager hashed password. */
#ifdef USE_OPENSSL_DES
    DES_ચાવી_schedule ks;

    setup_des_ચાવી(pw, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)lmbuffer, &ks, DES_ENCRYPT);

    setup_des_ચાવી(pw + 7, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)(lmbuffer + 8), &ks, DES_ENCRYPT);
#અથવાજો defined(USE_WOLFSSL_DES)
    Des des;
    setup_des_ચાવી(pw, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer, magic, DES_ચાવી_SIZE);
    setup_des_ચાવી(pw + 7, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer + 8, magic, DES_ચાવી_SIZE);
#અથવાજો defined(USE_GNUTLS_DES)
    સંરચના des_ctx des;
    setup_des_ચાવી(pw, &des);
    des_encrypt(&des, 8, lmbuffer, magic);
    setup_des_ચાવી(pw + 7, &des);
    des_encrypt(&des, 8, lmbuffer + 8, magic);
#અથવાજો defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
    encrypt_des(magic, lmbuffer, pw);
    encrypt_des(magic, lmbuffer + 8, pw + 7);
#endif

    memset(lmbuffer + 16, 0, 21 - 16);
  }

  પાછા CURLE_OK;
}

સ્થિર શૂન્ય ascii_to_unicode_le(unsigned અક્ષર *dest, અચલ અક્ષર *src,
                                size_t srclen)
{
  size_t i;
  માટે(i = 0; i < srclen; i++) {
    dest[2 * i] = (unsigned અક્ષર)src[i];
    dest[(2 * i) + 1] = '\0';
  }
}

/*
 * Set up nt hashed passwords
 * @unittest: 1600
 */
CURLcode Curl_ntlm_core_mk_nt_hash(અચલ અક્ષર *password,
                                   unsigned અક્ષર *ntbuffer /* 21 bytes */)
{
  size_t len = strlen(password);
  unsigned અક્ષર *pw;
  CURLcode result;
  જો(len > SIZE_MAX / 2) /* avoid integer overflow */
    પાછા CURLE_OUT_OF_MEMORY;
  pw = len ? curlx_malloc(len * 2) : (unsigned અક્ષર *)curlx_strdup("");
  જો(!pw)
    પાછા CURLE_OUT_OF_MEMORY;

  ascii_to_unicode_le(pw, password, len);

  /* Create NT hashed password. */
  result = Curl_md4it(ntbuffer, pw, 2 * len);
  જો(!result)
    memset(ntbuffer + 16, 0, 21 - 16);

  curlx_free(pw);

  પાછા result;
}

#ifndef USE_WINDOWS_SSPI

#define NTLMv2_BLOB_SIGNATURE "\x01\x01\x00\x00"
#define NTLMv2_BLOB_LEN       (44 - 16 + ntlm->target_info_len + 4)

/* Timestamp in tenths of a microsecond since January 1, 1601 00:00:00 UTC. */
સંરચના ms_filetime {
  unsigned પૂર્ણાંક dw_નીચલું_Date_Time;
  unsigned પૂર્ણાંક dw_ઉપલું_Date_Time;
};

/* Convert a time_t to an MS FILETIME (MS-DTYP section 2.3.3). */
સ્થિર શૂન્ય time2filetime(સંરચના ms_filetime *ft, time_t t)
{
#જો SIZEOF_TIME_T > 4
  t = (t + (curl_off_t)11644473600) * 10000000;
  ft->dw_નીચલું_Date_Time = (unsigned પૂર્ણાંક)(t & 0xFFFFFFFF);
  ft->dw_ઉપલું_Date_Time = (unsigned પૂર્ણાંક)(t >> 32);
#નહીંતર
  unsigned પૂર્ણાંક r, s;
  unsigned પૂર્ણાંક i;

  ft->dw_નીચલું_Date_Time = (unsigned પૂર્ણાંક)(t & 0xFFFFFFFF);
  ft->dw_ઉપલું_Date_Time = 0;

#ifndef HAVE_TIME_T_UNSIGNED
  /* Extend sign if needed. */
  જો(ft->dw_નીચલું_Date_Time & 0x80000000)
    ft->dw_ઉપલું_Date_Time = ~(unsigned પૂર્ણાંક)0;
#endif

  /* Bias seconds to Jan 1, 1601.
     134774 days = 11644473600 seconds = 0x2B6109100 */
  r = ft->dw_નીચલું_Date_Time;
  ft->dw_નીચલું_Date_Time = (ft->dw_નીચલું_Date_Time + 0xB6109100U) & 0xFFFFFFFF;
  ft->dw_ઉપલું_Date_Time += ft->dw_નીચલું_Date_Time < r ? 0x03 : 0x02;

  /* Convert to tenths of microseconds. */
  ft->dw_ઉપલું_Date_Time *= 10000000;
  i = 32;
  કરો {
    i -= 8;
    s = ((ft->dw_નીચલું_Date_Time >> i) & 0xFF) * (10000000 - 1);
    r = (s << i) & 0xFFFFFFFF;
    s >>= 1;   /* Split shift to avoid width overflow. */
    s >>= 31 - i;
    ft->dw_નીચલું_Date_Time = (ft->dw_નીચલું_Date_Time + r) & 0xFFFFFFFF;
    જો(ft->dw_નીચલું_Date_Time < r)
      s++;
    ft->dw_ઉપલું_Date_Time += s;
  } જ્યારે(i);
  ft->dw_ઉપલું_Date_Time &= 0xFFFFFFFF;
#endif
}

સ્થિર શૂન્ય ascii_uppercase_to_unicode_le(unsigned અક્ષર *dest,
                                          અચલ અક્ષર *src, size_t srclen)
{
  size_t i;
  માટે(i = 0; i < srclen; i++) {
    dest[2 * i] = (unsigned અક્ષર)(Curl_raw_toupper(src[i]));
    dest[(2 * i) + 1] = '\0';
  }
}

/* This creates the NTLMv2 hash by using NTLM hash as the key and Unicode
 * (uppercase UserName + Domain) as the data
 */
CURLcode Curl_ntlm_core_mk_ntlmv2_hash(અચલ અક્ષર *user, size_t userlen,
                                       અચલ અક્ષર *domain, size_t domlen,
                                       unsigned અક્ષર *ntlmhash,
                                       unsigned અક્ષર *ntlmv2hash)
{
  /* Unicode representation */
  size_t identity_len;
  unsigned અક્ષર *identity;
  CURLcode result = CURLE_OK;

  જો((userlen > CURL_MAX_આગત_LENGTH) || (domlen > CURL_MAX_આગત_LENGTH))
    પાછા CURLE_OUT_OF_MEMORY;

  identity_len = (userlen + domlen) * 2;
  identity = curlx_malloc(identity_len + 1);

  જો(!identity)
    પાછા CURLE_OUT_OF_MEMORY;

  ascii_uppercase_to_unicode_le(identity, user, userlen);
  ascii_to_unicode_le(identity + (userlen << 1), domain, domlen);

  result = Curl_hmacit(&Curl_HMAC_MD5, ntlmhash, 16, identity, identity_len,
                       ntlmv2hash);
  curlx_free(identity);

  પાછા result;
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
CURLcode Curl_ntlm_core_mk_ntlmv2_resp(અચલ unsigned અક્ષર *ntlmv2hash,
                                       અચલ unsigned અક્ષર *challenge_client,
                                       અચલ સંરચના ntlmdata *ntlm,
                                       unsigned અક્ષર **ntresp,
                                       unsigned પૂર્ણાંક *ntresp_len)
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

  unsigned પૂર્ણાંક len = 0;
  unsigned અક્ષર *ptr = ખાલી;
  unsigned અક્ષર hmac_output[HMAC_MD5_LENGTH];
  સંરચના ms_filetime tw;

  CURLcode result = CURLE_OK;

  /* Calculate the timestamp */
#ifdef DEBUGBUILD
  અક્ષર *force_timestamp = getenv("CURL_FORCETIME");
  જો(force_timestamp)
    time2filetime(&tw, (time_t)0);
  નહીંતર
#endif
    time2filetime(&tw, time(ખાલી));

  /* Calculate the response len */
  len = HMAC_MD5_LENGTH + NTLMv2_BLOB_LEN;

  /* Allocate the response */
  ptr = curlx_calloc(1, len);
  જો(!ptr)
    પાછા CURLE_OUT_OF_MEMORY;

  /* Create the BLOB structure */
  curl_msnprintf((અક્ષર *)ptr + HMAC_MD5_LENGTH, NTLMv2_BLOB_LEN,
                 "%c%c%c%c"           /* NTLMv2_BLOB_SIGNATURE */
                 "%c%c%c%c"           /* Reserved = 0 */
                 "%c%c%c%c%c%c%c%c",  /* Timestamp */
                 NTLMv2_BLOB_SIGNATURE[0], NTLMv2_BLOB_SIGNATURE[1],
                 NTLMv2_BLOB_SIGNATURE[2], NTLMv2_BLOB_SIGNATURE[3],
                 0, 0, 0, 0,
                 LONGQUARTET(tw.dw_નીચલું_Date_Time),
                 LONGQUARTET(tw.dw_ઉપલું_Date_Time));

  memcpy(ptr + 32, challenge_client, 8);
  જો(ntlm->target_info_len)
    memcpy(ptr + 44, ntlm->target_info, ntlm->target_info_len);

  /* Concatenate the Type 2 challenge with the BLOB and do HMAC MD5 */
  memcpy(ptr + 8, &ntlm->nonce[0], 8);
  result = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, HMAC_MD5_LENGTH, ptr + 8,
                       NTLMv2_BLOB_LEN + 8, hmac_output);
  જો(result) {
    curlx_free(ptr);
    પાછા result;
  }

  /* Concatenate the HMAC MD5 output  with the BLOB */
  memcpy(ptr, hmac_output, HMAC_MD5_LENGTH);

  /* Return the response */
  *ntresp = ptr;
  *ntresp_len = len;

  પાછા result;
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
CURLcode Curl_ntlm_core_mk_lmv2_resp(અચલ unsigned અક્ષર *ntlmv2hash,
                                     અચલ unsigned અક્ષર *challenge_client,
                                     અચલ unsigned અક્ષર *challenge_server,
                                     unsigned અક્ષર *lmresp)
{
  unsigned અક્ષર data[16];
  unsigned અક્ષર hmac_output[16];
  CURLcode result = CURLE_OK;

  memcpy(&data[0], challenge_server, 8);
  memcpy(&data[8], challenge_client, 8);

  result = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, 16, &data[0], 16,
                       hmac_output);
  જો(result)
    પાછા result;

  /* Concatenate the HMAC MD5 output with the client nonce */
  memcpy(lmresp, hmac_output, 16);
  memcpy(lmresp + 16, challenge_client, 8);

  પાછા result;
}

#endif /* !USE_WINDOWS_SSPI */

#endif /* USE_CURL_NTLM_CORE */
