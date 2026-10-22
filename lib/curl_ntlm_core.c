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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

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
#ଅନ୍ତର୍ଭାଵ <nettle/version.h>
#ଯଦି NETTLE_VERSION_MAJOR < 4
#define USE_GNUTLS_DES
#endif
#endif

#ଯଦି defined(USE_OPENSSL) && defined(HAVE_DES_ECB_ENCRYPT)

#  include <openssl/des.h>
#  ifdef OPENSSL_IS_AWSLC  /* for versions 1.2.0 to 1.30.1 */
#    define DES_set_ଚାବି_unchecked (ଶୂନ୍ୟ)DES_set_ଚାବି
#  endif
#  define USE_OPENSSL_DES

#ନଚେତଯଦି defined(USE_WOLFSSL) && defined(HAVE_WC_DES_ECBENCRYPT)

#  include <wolfssl/options.h>
#  include <wolfssl/wolfcrypt/des3.h>
#  define USE_WOLFSSL_DES

#ନଚେତଯଦି defined(USE_GNUTLS_DES)
#  include <nettle/des.h>
#  define USE_CURL_DES_SET_ODD_PARITY
#ନଚେତଯଦି defined(USE_MBEDTLS) && defined(HAVE_MBEDTLS_DES_CRYPT_ECB)
#  include <mbedtls/version.h>
#  ଯଦି MBEDTLS_VERSION_ସଂଖ୍ୟା < 0x03020000
#  error "mbedTLS 3.2.0 or greater required"
#  endif
#  include <mbedtls/des.h>
#  define USE_MBEDTLS_DES
#ନଚେତଯଦି defined(USE_OS400CRYPTO)
#  include "cipher.mih"  /* mih/cipher */
#  define USE_CURL_DES_SET_ODD_PARITY
#ନଚେତଯଦି defined(USE_WIN32_CRYPTO)
#  include <wincrypt.h>
#  define USE_CURL_DES_SET_ODD_PARITY
#ଅନ୍ୟଥା
#  error "cannot compile NTLM support without a crypto library with DES."
#endif

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "strcase.h"
#ଅନ୍ତର୍ଭାଵ "curl_ntlm_core.h"
#ଅନ୍ତର୍ଭାଵ "curl_md5.h"
#ଅନ୍ତର୍ଭାଵ "curl_hmac.h"
#ଅନ୍ତର୍ଭାଵ "curl_md4.h"
#ଅନ୍ତର୍ଭାଵ "vauth/vauth.h"

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
ସ୍ଥିର ଶୂନ୍ୟ curl_des_set_odd_parity(unsigned ଅକ୍ଷର *bytes, size_t len)
{
  size_t i;

  ପାଇଁ(i = 0; i < len; i++) {
    unsigned ଅକ୍ଷର b = bytes[i];

    bool needs_parity = (((b >> 7) ^ (b >> 6) ^ (b >> 5) ^
                          (b >> 4) ^ (b >> 3) ^ (b >> 2) ^
                          (b >> 1)) & 0x01) == 0;

    ଯଦି(needs_parity)
      bytes[i] |= 0x01;
    ଅନ୍ୟଥା
      bytes[i] &= 0xfe;
  }
}
#endif /* USE_CURL_DES_SET_ODD_PARITY */

/*
 * Turns a 56-bit key into being 64-bit wide.
 */
ସ୍ଥିର ଶୂନ୍ୟ extend_ଚାବି_56_to_64(ଅଚଳ unsigned ଅକ୍ଷର *ଚାବି_56, ଅକ୍ଷର *ଚାବି)
{
  ଚାବି[0] = (ଅକ୍ଷର)ଚାବି_56[0];
  ଚାବି[1] = (ଅକ୍ଷର)(((ଚାବି_56[0] << 7) & 0xFF) | (ଚାବି_56[1] >> 1));
  ଚାବି[2] = (ଅକ୍ଷର)(((ଚାବି_56[1] << 6) & 0xFF) | (ଚାବି_56[2] >> 2));
  ଚାବି[3] = (ଅକ୍ଷର)(((ଚାବି_56[2] << 5) & 0xFF) | (ଚାବି_56[3] >> 3));
  ଚାବି[4] = (ଅକ୍ଷର)(((ଚାବି_56[3] << 4) & 0xFF) | (ଚାବି_56[4] >> 4));
  ଚାବି[5] = (ଅକ୍ଷର)(((ଚାବି_56[4] << 3) & 0xFF) | (ଚାବି_56[5] >> 5));
  ଚାବି[6] = (ଅକ୍ଷର)(((ଚାବି_56[5] << 2) & 0xFF) | (ଚାବି_56[6] >> 6));
  ଚାବି[7] = (ଅକ୍ଷର)((ଚାବି_56[6] << 1) & 0xFF);
}

#ifdef USE_OPENSSL_DES
/*
 * Turns a 56-bit key into a 64-bit, odd parity key and sets the key. The
 * key schedule ks is also set.
 */
ସ୍ଥିର ଶୂନ୍ୟ setup_des_ଚାବି(ଅଚଳ unsigned ଅକ୍ଷର *ଚାବି_56, DES_ଚାବି_schedule *ks)
{
  DES_cblock ଚାବି;

  /* Expand the 56-bit key to 64 bits */
  extend_ଚାବି_56_to_64(ଚାବି_56, (ଅକ୍ଷର *)&ଚାବି);

  /* Set the key parity to odd */
  DES_set_odd_parity(&ଚାବି);

  /* Set the key */
  DES_set_ଚାବି_unchecked(&ଚାବି, ks);
}

#ନଚେତଯଦି defined(USE_WOLFSSL_DES)
ସ୍ଥିର ଶୂନ୍ୟ setup_des_ଚାବି(ଅଚଳ unsigned ଅକ୍ଷର *ଚାବି_56, Des *des)
{
  byte ଚାବି[8];

  /* Expand the 56-bit key to 64 bits */
  extend_ଚାବି_56_to_64(ଚାବି_56, (ଅକ୍ଷର *)ଚାବି);

  /* Set the key */
  wc_Des_Set_ଚାବି(des, ଚାବି, ଖାଲି, 0);
}

#ନଚେତଯଦି defined(USE_GNUTLS_DES)
ସ୍ଥିର ଶୂନ୍ୟ setup_des_ଚାବି(ଅଚଳ unsigned ଅକ୍ଷର *ଚାବି_56, ସଂରଚନା des_ctx *des)
{
  ଅକ୍ଷର ଚାବି[8];

  /* Expand the 56-bit key to 64 bits */
  extend_ଚାବି_56_to_64(ଚାବି_56, ଚାବି);

  /* Set the key parity to odd */
  curl_des_set_odd_parity((unsigned ଅକ୍ଷର *)ଚାବି, ଆକାର(ଚାବି));

  /* Set the key */
  des_set_ଚାବି(des, (ଅଚଳ uint8_t *)ଚାବି);
}

#ନଚେତଯଦି defined(USE_MBEDTLS_DES)
ସ୍ଥିର bool encrypt_des(ଅଚଳ unsigned ଅକ୍ଷର *in, unsigned ଅକ୍ଷର *out,
                        ଅଚଳ unsigned ଅକ୍ଷର *ଚାବି_56)
{
  mbedtls_des_context ctx;
  ଅକ୍ଷର ଚାବି[8];

  /* Expand the 56-bit key to 64 bits */
  extend_ଚାବି_56_to_64(ଚାବି_56, ଚାବି);

  /* Set the key parity to odd */
  mbedtls_des_ଚାବି_set_parity((unsigned ଅକ୍ଷର *)ଚାବି);

  /* Perform the encryption */
  mbedtls_des_init(&ctx);
  mbedtls_des_setkey_enc(&ctx, (unsigned ଅକ୍ଷର *)ଚାବି);
  ଫେରନ୍ତୁ mbedtls_des_crypt_ecb(&ctx, in, out) == 0;
}

#ନଚେତଯଦି defined(USE_OS400CRYPTO)
ସ୍ଥିର bool encrypt_des(ଅଚଳ unsigned ଅକ୍ଷର *in, unsigned ଅକ୍ଷର *out,
                        ଅଚଳ unsigned ଅକ୍ଷର *ଚାବି_56)
{
  ଅକ୍ଷର ଚାବି[8];
  _CIPHER_Control_T ctl;

  /* Setup the cipher control structure */
  ctl.Func_ID = ENCRYPT_ONLY;
  ctl.Data_Len = ଆକାର(ଚାବି);

  /* Expand the 56-bit key to 64 bits */
  extend_ଚାବି_56_to_64(ଚାବି_56, ctl.Crypto_ଚାବି);

  /* Set the key parity to odd */
  curl_des_set_odd_parity((unsigned ଅକ୍ଷର *)ctl.Crypto_ଚାବି, ctl.Data_Len);

  /* Perform the encryption */
  _CIPHER((_SPCPTR *)&out, &ctl, (_SPCPTR *)&in);

  ଫେରନ୍ତୁ TRUE;
}

#ନଚେତଯଦି defined(USE_WIN32_CRYPTO)
ସ୍ଥିର bool encrypt_des(ଅଚଳ unsigned ଅକ୍ଷର *in, unsigned ଅକ୍ଷର *out,
                        ଅଚଳ unsigned ଅକ୍ଷର *ଚାବି_56)
{
  HCRYPTPROV hprov;
  HCRYPTKEY hkey;
  ସଂରଚନା {
    BLOBHEADER hdr;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;
    ଅକ୍ଷର ଚାବି[8];
  } blob;
  DWORD len = 8;
  BOOL res;

  /* Acquire the crypto provider */
  ଯଦି(!CryptAcquireContext(&hprov, ଖାଲି, ଖାଲି, PROV_RSA_FULL,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    ଫେରନ୍ତୁ FALSE;

  /* Setup the key blob structure */
  memset(&blob, 0, ଆକାର(blob));
  blob.hdr.bType = PLAINTEXTKEYBLOB;
  blob.hdr.bVersion = 2;
  blob.hdr.ai_ଚାବି_Alg = CALG_DES;
  blob.len = ଆକାର(blob.ଚାବି);

  /* Expand the 56-bit key to 64 bits */
  extend_ଚାବି_56_to_64(ଚାବି_56, blob.ଚାବି);

  /* Set the key parity to odd */
  curl_des_set_odd_parity((unsigned ଅକ୍ଷର *)blob.ଚାବି, ଆକାର(blob.ଚାବି));

  /* Import the key */
  ଯଦି(!Crypt_Import_ଚାବି(hprov, (BYTE *)&blob, ଆକାର(blob), 0, 0, &hkey)) {
    CryptReleaseContext(hprov, 0);

    ଫେରନ୍ତୁ FALSE;
  }

  memcpy(out, in, 8);

  /* Perform the encryption */
  res = CryptEncrypt(hkey, 0, FALSE, 0, out, &len, len);

  Crypt_Destroy_ଚାବି(hkey);
  CryptReleaseContext(hprov, 0);

  ଫେରନ୍ତୁ res;
}

#endif /* crypto backends */

/*
 * takes a 21-byte array and treats it as 3 56-bit DES keys. The
 * 8-byte plaintext is encrypted with each key and the resulting 24
 * bytes are stored in the results array.
 */
ଶୂନ୍ୟ Curl_ntlm_core_lm_resp(ଅଚଳ unsigned ଅକ୍ଷର *ଚାବିଗୁଡ଼ିକ,
                            ଅଚଳ unsigned ଅକ୍ଷର *plaintext,
                            unsigned ଅକ୍ଷର *results)
{
#ifdef USE_OPENSSL_DES
  DES_ଚାବି_schedule ks;

  setup_des_ଚାବି(ଚାବିଗୁଡ଼ିକ, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)results, &ks, DES_ENCRYPT);

  setup_des_ଚାବି(ଚାବିଗୁଡ଼ିକ + 7, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 8), &ks, DES_ENCRYPT);

  setup_des_ଚାବି(ଚାବିଗୁଡ଼ିକ + 14, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 16), &ks, DES_ENCRYPT);
#ନଚେତଯଦି defined(USE_WOLFSSL_DES)
  Des des;
  setup_des_ଚାବି(ଚାବିଗୁଡ଼ିକ, &des);
  wc_Des_EcbEncrypt(&des, results, plaintext, DES_ଚାବି_SIZE);
  setup_des_ଚାବି(ଚାବିଗୁଡ଼ିକ + 7, &des);
  wc_Des_EcbEncrypt(&des, results + 8, plaintext, DES_ଚାବି_SIZE);
  setup_des_ଚାବି(ଚାବିଗୁଡ଼ିକ + 14, &des);
  wc_Des_EcbEncrypt(&des, results + 16, plaintext, DES_ଚାବି_SIZE);
#ନଚେତଯଦି defined(USE_GNUTLS_DES)
  ସଂରଚନା des_ctx des;
  setup_des_ଚାବି(ଚାବିଗୁଡ଼ିକ, &des);
  des_encrypt(&des, 8, results, plaintext);
  setup_des_ଚାବି(ଚାବିଗୁଡ଼ିକ + 7, &des);
  des_encrypt(&des, 8, results + 8, plaintext);
  setup_des_ଚାବି(ଚାବିଗୁଡ଼ିକ + 14, &des);
  des_encrypt(&des, 8, results + 16, plaintext);
#ନଚେତଯଦି defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
  encrypt_des(plaintext, results, ଚାବିଗୁଡ଼ିକ);
  encrypt_des(plaintext, results + 8, ଚାବିଗୁଡ଼ିକ + 7);
  encrypt_des(plaintext, results + 16, ଚାବିଗୁଡ଼ିକ + 14);
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)ଚାବିଗୁଡ଼ିକ;
  (ଶୂନ୍ୟ)plaintext;
  (ଶୂନ୍ୟ)results;
#endif
}

/*
 * Set up lanmanager hashed password
 */
CURLcode Curl_ntlm_core_mk_lm_hash(ଅଚଳ ଅକ୍ଷର *password,
                                   unsigned ଅକ୍ଷର *lmbuffer /* 21 bytes */)
{
  unsigned ଅକ୍ଷର pw[14];
  ସ୍ଥିର ଅଚଳ unsigned ଅକ୍ଷର magic[] = {
    0x4B, 0x47, 0x53, 0x21, 0x40, 0x23, 0x24, 0x25 /* i.e. KGS!@#$% */
  };
  size_t len = CURLMIN(strlen(password), 14);

  Curl_strntoupper((ଅକ୍ଷର *)pw, password, len);
  memset(&pw[len], 0, 14 - len);

  {
    /* Create LanManager hashed password. */
#ifdef USE_OPENSSL_DES
    DES_ଚାବି_schedule ks;

    setup_des_ଚାବି(pw, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)lmbuffer, &ks, DES_ENCRYPT);

    setup_des_ଚାବି(pw + 7, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)(lmbuffer + 8), &ks, DES_ENCRYPT);
#ନଚେତଯଦି defined(USE_WOLFSSL_DES)
    Des des;
    setup_des_ଚାବି(pw, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer, magic, DES_ଚାବି_SIZE);
    setup_des_ଚାବି(pw + 7, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer + 8, magic, DES_ଚାବି_SIZE);
#ନଚେତଯଦି defined(USE_GNUTLS_DES)
    ସଂରଚନା des_ctx des;
    setup_des_ଚାବି(pw, &des);
    des_encrypt(&des, 8, lmbuffer, magic);
    setup_des_ଚାବି(pw + 7, &des);
    des_encrypt(&des, 8, lmbuffer + 8, magic);
#ନଚେତଯଦି defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
    encrypt_des(magic, lmbuffer, pw);
    encrypt_des(magic, lmbuffer + 8, pw + 7);
#endif

    memset(lmbuffer + 16, 0, 21 - 16);
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ ascii_to_unicode_le(unsigned ଅକ୍ଷର *dest, ଅଚଳ ଅକ୍ଷର *src,
                                size_t srclen)
{
  size_t i;
  ପାଇଁ(i = 0; i < srclen; i++) {
    dest[2 * i] = (unsigned ଅକ୍ଷର)src[i];
    dest[(2 * i) + 1] = '\0';
  }
}

/*
 * Set up nt hashed passwords
 * @unittest: 1600
 */
CURLcode Curl_ntlm_core_mk_nt_hash(ଅଚଳ ଅକ୍ଷର *password,
                                   unsigned ଅକ୍ଷର *ntbuffer /* 21 bytes */)
{
  size_t len = strlen(password);
  unsigned ଅକ୍ଷର *pw;
  CURLcode result;
  ଯଦି(len > SIZE_MAX / 2) /* avoid integer overflow */
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  pw = len ? curlx_malloc(len * 2) : (unsigned ଅକ୍ଷର *)curlx_strdup("");
  ଯଦି(!pw)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  ascii_to_unicode_le(pw, password, len);

  /* Create NT hashed password. */
  result = Curl_md4it(ntbuffer, pw, 2 * len);
  ଯଦି(!result)
    memset(ntbuffer + 16, 0, 21 - 16);

  curlx_free(pw);

  ଫେରନ୍ତୁ result;
}

#ifndef USE_WINDOWS_SSPI

#define NTLMv2_BLOB_SIGNATURE "\x01\x01\x00\x00"
#define NTLMv2_BLOB_LEN       (44 - 16 + ntlm->target_info_len + 4)

/* Timestamp in tenths of a microsecond since January 1, 1601 00:00:00 UTC. */
ସଂରଚନା ms_filetime {
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା dw_ତଳ_Date_Time;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା dw_ଉପର_Date_Time;
};

/* Convert a time_t to an MS FILETIME (MS-DTYP section 2.3.3). */
ସ୍ଥିର ଶୂନ୍ୟ time2filetime(ସଂରଚନା ms_filetime *ft, time_t t)
{
#ଯଦି SIZEOF_TIME_T > 4
  t = (t + (curl_off_t)11644473600) * 10000000;
  ft->dw_ତଳ_Date_Time = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(t & 0xFFFFFFFF);
  ft->dw_ଉପର_Date_Time = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(t >> 32);
#ଅନ୍ୟଥା
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, s;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

  ft->dw_ତଳ_Date_Time = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(t & 0xFFFFFFFF);
  ft->dw_ଉପର_Date_Time = 0;

#ifndef HAVE_TIME_T_UNSIGNED
  /* Extend sign if needed. */
  ଯଦି(ft->dw_ତଳ_Date_Time & 0x80000000)
    ft->dw_ଉପର_Date_Time = ~(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)0;
#endif

  /* Bias seconds to Jan 1, 1601.
     134774 days = 11644473600 seconds = 0x2B6109100 */
  r = ft->dw_ତଳ_Date_Time;
  ft->dw_ତଳ_Date_Time = (ft->dw_ତଳ_Date_Time + 0xB6109100U) & 0xFFFFFFFF;
  ft->dw_ଉପର_Date_Time += ft->dw_ତଳ_Date_Time < r ? 0x03 : 0x02;

  /* Convert to tenths of microseconds. */
  ft->dw_ଉପର_Date_Time *= 10000000;
  i = 32;
  କର {
    i -= 8;
    s = ((ft->dw_ତଳ_Date_Time >> i) & 0xFF) * (10000000 - 1);
    r = (s << i) & 0xFFFFFFFF;
    s >>= 1;   /* Split shift to avoid width overflow. */
    s >>= 31 - i;
    ft->dw_ତଳ_Date_Time = (ft->dw_ତଳ_Date_Time + r) & 0xFFFFFFFF;
    ଯଦି(ft->dw_ତଳ_Date_Time < r)
      s++;
    ft->dw_ଉପର_Date_Time += s;
  } ଯେତେବେଳେ(i);
  ft->dw_ଉପର_Date_Time &= 0xFFFFFFFF;
#endif
}

ସ୍ଥିର ଶୂନ୍ୟ ascii_uppercase_to_unicode_le(unsigned ଅକ୍ଷର *dest,
                                          ଅଚଳ ଅକ୍ଷର *src, size_t srclen)
{
  size_t i;
  ପାଇଁ(i = 0; i < srclen; i++) {
    dest[2 * i] = (unsigned ଅକ୍ଷର)(Curl_raw_toupper(src[i]));
    dest[(2 * i) + 1] = '\0';
  }
}

/* This creates the NTLMv2 hash by using NTLM hash as the key and Unicode
 * (uppercase UserName + Domain) as the data
 */
CURLcode Curl_ntlm_core_mk_ntlmv2_hash(ଅଚଳ ଅକ୍ଷର *user, size_t userlen,
                                       ଅଚଳ ଅକ୍ଷର *domain, size_t domlen,
                                       unsigned ଅକ୍ଷର *ntlmhash,
                                       unsigned ଅକ୍ଷର *ntlmv2hash)
{
  /* Unicode representation */
  size_t identity_len;
  unsigned ଅକ୍ଷର *identity;
  CURLcode result = CURLE_OK;

  ଯଦି((userlen > CURL_MAX_ନିବେଶ_LENGTH) || (domlen > CURL_MAX_ନିବେଶ_LENGTH))
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  identity_len = (userlen + domlen) * 2;
  identity = curlx_malloc(identity_len + 1);

  ଯଦି(!identity)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  ascii_uppercase_to_unicode_le(identity, user, userlen);
  ascii_to_unicode_le(identity + (userlen << 1), domain, domlen);

  result = Curl_hmacit(&Curl_HMAC_MD5, ntlmhash, 16, identity, identity_len,
                       ntlmv2hash);
  curlx_free(identity);

  ଫେରନ୍ତୁ result;
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
CURLcode Curl_ntlm_core_mk_ntlmv2_resp(ଅଚଳ unsigned ଅକ୍ଷର *ntlmv2hash,
                                       ଅଚଳ unsigned ଅକ୍ଷର *challenge_client,
                                       ଅଚଳ ସଂରଚନା ntlmdata *ntlm,
                                       unsigned ଅକ୍ଷର **ntresp,
                                       unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ntresp_len)
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

  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = 0;
  unsigned ଅକ୍ଷର *ptr = ଖାଲି;
  unsigned ଅକ୍ଷର hmac_output[HMAC_MD5_LENGTH];
  ସଂରଚନା ms_filetime tw;

  CURLcode result = CURLE_OK;

  /* Calculate the timestamp */
#ifdef DEBUGBUILD
  ଅକ୍ଷର *force_timestamp = getenv("CURL_FORCETIME");
  ଯଦି(force_timestamp)
    time2filetime(&tw, (time_t)0);
  ଅନ୍ୟଥା
#endif
    time2filetime(&tw, time(ଖାଲି));

  /* Calculate the response len */
  len = HMAC_MD5_LENGTH + NTLMv2_BLOB_LEN;

  /* Allocate the response */
  ptr = curlx_calloc(1, len);
  ଯଦି(!ptr)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  /* Create the BLOB structure */
  curl_msnprintf((ଅକ୍ଷର *)ptr + HMAC_MD5_LENGTH, NTLMv2_BLOB_LEN,
                 "%c%c%c%c"           /* NTLMv2_BLOB_SIGNATURE */
                 "%c%c%c%c"           /* Reserved = 0 */
                 "%c%c%c%c%c%c%c%c",  /* Timestamp */
                 NTLMv2_BLOB_SIGNATURE[0], NTLMv2_BLOB_SIGNATURE[1],
                 NTLMv2_BLOB_SIGNATURE[2], NTLMv2_BLOB_SIGNATURE[3],
                 0, 0, 0, 0,
                 LONGQUARTET(tw.dw_ତଳ_Date_Time),
                 LONGQUARTET(tw.dw_ଉପର_Date_Time));

  memcpy(ptr + 32, challenge_client, 8);
  ଯଦି(ntlm->target_info_len)
    memcpy(ptr + 44, ntlm->target_info, ntlm->target_info_len);

  /* Concatenate the Type 2 challenge with the BLOB and do HMAC MD5 */
  memcpy(ptr + 8, &ntlm->nonce[0], 8);
  result = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, HMAC_MD5_LENGTH, ptr + 8,
                       NTLMv2_BLOB_LEN + 8, hmac_output);
  ଯଦି(result) {
    curlx_free(ptr);
    ଫେରନ୍ତୁ result;
  }

  /* Concatenate the HMAC MD5 output  with the BLOB */
  memcpy(ptr, hmac_output, HMAC_MD5_LENGTH);

  /* Return the response */
  *ntresp = ptr;
  *ntresp_len = len;

  ଫେରନ୍ତୁ result;
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
CURLcode Curl_ntlm_core_mk_lmv2_resp(ଅଚଳ unsigned ଅକ୍ଷର *ntlmv2hash,
                                     ଅଚଳ unsigned ଅକ୍ଷର *challenge_client,
                                     ଅଚଳ unsigned ଅକ୍ଷର *challenge_server,
                                     unsigned ଅକ୍ଷର *lmresp)
{
  unsigned ଅକ୍ଷର data[16];
  unsigned ଅକ୍ଷର hmac_output[16];
  CURLcode result = CURLE_OK;

  memcpy(&data[0], challenge_server, 8);
  memcpy(&data[8], challenge_client, 8);

  result = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, 16, &data[0], 16,
                       hmac_output);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  /* Concatenate the HMAC MD5 output with the client nonce */
  memcpy(lmresp, hmac_output, 16);
  memcpy(lmresp + 16, challenge_client, 8);

  ଫେରନ୍ତୁ result;
}

#endif /* !USE_WINDOWS_SSPI */

#endif /* USE_CURL_NTLM_CORE */
