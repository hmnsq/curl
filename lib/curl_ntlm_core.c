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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_CURL_NTLM_CORE

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

#నిర్వచితమైతే USE_GNUTLS
#అన్తర్భావ <nettle/version.h>
#ఒకవేళ NETTLE_సంస్కరణ_MAJOR < 4
#నిర్వచించు USE_GNUTLS_DES
#అయితే_ముగింపు
#అయితే_ముగింపు

#ఒకవేళ defined(USE_OPENSSL) && defined(HAVE_DES_ECB_ENCRYPT)

#  include <openssl/des.h>
#  ifdef OPENSSL_IS_AWSLC  /* for versions 1.2.0 to 1.30.1 */
#    define DES_సమితి_కీలకం_unchecked (శూన్య)DES_సమితి_కీలకం
#  endif
#  define USE_OPENSSL_DES

#లేదావేళ defined(USE_WOLFSSL) && defined(HAVE_WC_DES_ECBENCRYPT)

#  include <wolfssl/options.h>
#  include <wolfssl/wolfcrypt/des3.h>
#  define USE_WOLFSSL_DES

#లేదావేళ defined(USE_GNUTLS_DES)
#  include <nettle/des.h>
#  define USE_CURL_DES_సమితి_ODD_PARITY
#లేదావేళ defined(USE_MBEDTLS) && defined(HAVE_MBEDTLS_DES_CRYPT_ECB)
#  include <mbedtls/version.h>
#  ఒకవేళ MBEDTLS_సంస్కరణ_సంఖ్య < 0x03020000
#  error "mbedTLS 3.2.0 or greater required"
#  endif
#  include <mbedtls/des.h>
#  define USE_MBEDTLS_DES
#లేదావేళ defined(USE_OS400CRYPTO)
#  include "cipher.mih"  /* mih/cipher */
#  define USE_CURL_DES_సమితి_ODD_PARITY
#లేదావేళ defined(USE_WIN32_CRYPTO)
#  include <wincrypt.h>
#  define USE_CURL_DES_సమితి_ODD_PARITY
#లేకపోతే
#  error "cannot compile NTLM support without a crypto library with DES."
#అయితే_ముగింపు

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "strcase.h"
#అన్తర్భావ "curl_ntlm_core.h"
#అన్తర్భావ "curl_md5.h"
#అన్తర్భావ "curl_hmac.h"
#అన్తర్భావ "curl_md4.h"
#అన్తర్భావ "vauth/vauth.h"

#నిర్వచితమైతే USE_CURL_DES_సమితి_ODD_PARITY
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
స్థిర శూన్య curl_des_సమితి_odd_parity(గుర్తులేని అక్షరం *బైట్లు, size_t len)
{
  size_t i;

  కొరకు(i = 0; i < len; i++) {
    గుర్తులేని అక్షరం b = బైట్లు[i];

    bool needs_parity = (((b >> 7) ^ (b >> 6) ^ (b >> 5) ^
                          (b >> 4) ^ (b >> 3) ^ (b >> 2) ^
                          (b >> 1)) & 0x01) == 0;

    ఒకవేళ(needs_parity)
      బైట్లు[i] |= 0x01;
    లేకపోతే
      బైట్లు[i] &= 0xfe;
  }
}
#అయితే_ముగింపు /* USE_CURL_DES_SET_ODD_PARITY */

/*
 * Turns a 56-bit key into being 64-bit wide.
 */
స్థిర శూన్య extend_కీలకం_56_గమ్యం_64(స్థిరరాశి గుర్తులేని అక్షరం *కీలకం_56, అక్షరం *కీలకం)
{
  కీలకం[0] = (అక్షరం)కీలకం_56[0];
  కీలకం[1] = (అక్షరం)(((కీలకం_56[0] << 7) & 0xFF) | (కీలకం_56[1] >> 1));
  కీలకం[2] = (అక్షరం)(((కీలకం_56[1] << 6) & 0xFF) | (కీలకం_56[2] >> 2));
  కీలకం[3] = (అక్షరం)(((కీలకం_56[2] << 5) & 0xFF) | (కీలకం_56[3] >> 3));
  కీలకం[4] = (అక్షరం)(((కీలకం_56[3] << 4) & 0xFF) | (కీలకం_56[4] >> 4));
  కీలకం[5] = (అక్షరం)(((కీలకం_56[4] << 3) & 0xFF) | (కీలకం_56[5] >> 5));
  కీలకం[6] = (అక్షరం)(((కీలకం_56[5] << 2) & 0xFF) | (కీలకం_56[6] >> 6));
  కీలకం[7] = (అక్షరం)((కీలకం_56[6] << 1) & 0xFF);
}

#నిర్వచితమైతే USE_OPENSSL_DES
/*
 * Turns a 56-bit key into a 64-bit, odd parity key and sets the key. The
 * key schedule ks is also set.
 */
స్థిర శూన్య అమరిక_des_కీలకం(స్థిరరాశి గుర్తులేని అక్షరం *కీలకం_56, DES_కీలకం_కాలపట్టిక *ks)
{
  DES_cblock కీలకం;

  /* Expand the 56-bit key to 64 bits */
  extend_కీలకం_56_గమ్యం_64(కీలకం_56, (అక్షరం *)&కీలకం);

  /* Set the key parity to odd */
  DES_సమితి_odd_parity(&కీలకం);

  /* Set the key */
  DES_సమితి_కీలకం_unchecked(&కీలకం, ks);
}

#లేదావేళ defined(USE_WOLFSSL_DES)
స్థిర శూన్య అమరిక_des_కీలకం(స్థిరరాశి గుర్తులేని అక్షరం *కీలకం_56, Des *des)
{
  byte కీలకం[8];

  /* Expand the 56-bit key to 64 bits */
  extend_కీలకం_56_గమ్యం_64(కీలకం_56, (అక్షరం *)కీలకం);

  /* Set the key */
  wc_Des_సమితి_కీలకం(des, కీలకం, ఖాళీ, 0);
}

#లేదావేళ defined(USE_GNUTLS_DES)
స్థిర శూన్య అమరిక_des_కీలకం(స్థిరరాశి గుర్తులేని అక్షరం *కీలకం_56, నిర్మాణం des_ctx *des)
{
  అక్షరం కీలకం[8];

  /* Expand the 56-bit key to 64 bits */
  extend_కీలకం_56_గమ్యం_64(కీలకం_56, కీలకం);

  /* Set the key parity to odd */
  curl_des_సమితి_odd_parity((గుర్తులేని అక్షరం *)కీలకం, పరిమాణం(కీలకం));

  /* Set the key */
  des_సమితి_కీలకం(des, (స్థిరరాశి uint8_t *)కీలకం);
}

#లేదావేళ defined(USE_MBEDTLS_DES)
స్థిర bool encrypt_des(స్థిరరాశి గుర్తులేని అక్షరం *లోపల, గుర్తులేని అక్షరం *బహిర్గతం,
                        స్థిరరాశి గుర్తులేని అక్షరం *కీలకం_56)
{
  mbedtls_des_context ctx;
  అక్షరం కీలకం[8];

  /* Expand the 56-bit key to 64 bits */
  extend_కీలకం_56_గమ్యం_64(కీలకం_56, కీలకం);

  /* Set the key parity to odd */
  mbedtls_des_కీలకం_సమితి_parity((గుర్తులేని అక్షరం *)కీలకం);

  /* Perform the encryption */
  mbedtls_des_init(&ctx);
  mbedtls_des_setkey_enc(&ctx, (గుర్తులేని అక్షరం *)కీలకం);
  తిరిగి mbedtls_des_crypt_ecb(&ctx, లోపల, బహిర్గతం) == 0;
}

#లేదావేళ defined(USE_OS400CRYPTO)
స్థిర bool encrypt_des(స్థిరరాశి గుర్తులేని అక్షరం *లోపల, గుర్తులేని అక్షరం *బహిర్గతం,
                        స్థిరరాశి గుర్తులేని అక్షరం *కీలకం_56)
{
  అక్షరం కీలకం[8];
  _CIPHER_Control_T ctl;

  /* Setup the cipher control structure */
  ctl.Func_ID = ENCRYPT_ONLY;
  ctl.దత్తాంశం_Len = పరిమాణం(కీలకం);

  /* Expand the 56-bit key to 64 bits */
  extend_కీలకం_56_గమ్యం_64(కీలకం_56, ctl.Crypto_కీలకం);

  /* Set the key parity to odd */
  curl_des_సమితి_odd_parity((గుర్తులేని అక్షరం *)ctl.Crypto_కీలకం, ctl.దత్తాంశం_Len);

  /* Perform the encryption */
  _CIPHER((_SPCPTR *)&బహిర్గతం, &ctl, (_SPCPTR *)&లోపల);

  తిరిగి TRUE;
}

#లేదావేళ defined(USE_WIN32_CRYPTO)
స్థిర bool encrypt_des(స్థిరరాశి గుర్తులేని అక్షరం *లోపల, గుర్తులేని అక్షరం *బహిర్గతం,
                        స్థిరరాశి గుర్తులేని అక్షరం *కీలకం_56)
{
  HCRYPTPROV hprov;
  HCRYPTKEY hkey;
  నిర్మాణం {
    BLOBHEADER hdr;
    గుర్తులేని పూర్ణాంకం len;
    అక్షరం కీలకం[8];
  } blob;
  DWORD len = 8;
  BOOL res;

  /* Acquire the crypto provider */
  ఒకవేళ(!CryptAcquireContext(&hprov, ఖాళీ, ఖాళీ, PROV_RSA_నిండిన,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    తిరిగి FALSE;

  /* Setup the key blob structure */
  memset(&blob, 0, పరిమాణం(blob));
  blob.hdr.bType = PLAINTEXTKEYBLOB;
  blob.hdr.b_సంస్కరణ = 2;
  blob.hdr.ai_కీలకం_Alg = CALG_DES;
  blob.len = పరిమాణం(blob.కీలకం);

  /* Expand the 56-bit key to 64 bits */
  extend_కీలకం_56_గమ్యం_64(కీలకం_56, blob.కీలకం);

  /* Set the key parity to odd */
  curl_des_సమితి_odd_parity((గుర్తులేని అక్షరం *)blob.కీలకం, పరిమాణం(blob.కీలకం));

  /* Import the key */
  ఒకవేళ(!Crypt_Import_కీలకం(hprov, (BYTE *)&blob, పరిమాణం(blob), 0, 0, &hkey)) {
    CryptReleaseContext(hprov, 0);

    తిరిగి FALSE;
  }

  జ్ఞాపకం_నకలు(బహిర్గతం, లోపల, 8);

  /* Perform the encryption */
  res = CryptEncrypt(hkey, 0, FALSE, 0, బహిర్గతం, &len, len);

  Crypt_Destroy_కీలకం(hkey);
  CryptReleaseContext(hprov, 0);

  తిరిగి res;
}

#అయితే_ముగింపు /* crypto backends */

/*
 * takes a 21-byte array and treats it as 3 56-bit DES keys. The
 * 8-byte plaintext is encrypted with each key and the resulting 24
 * bytes are stored in the results array.
 */
శూన్య Curl_ntlm_core_lm_resp(స్థిరరాశి గుర్తులేని అక్షరం *కీలకాలు,
                            స్థిరరాశి గుర్తులేని అక్షరం *plaintext,
                            గుర్తులేని అక్షరం *results)
{
#నిర్వచితమైతే USE_OPENSSL_DES
  DES_కీలకం_కాలపట్టిక ks;

  అమరిక_des_కీలకం(కీలకాలు, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)results, &ks, DES_ENCRYPT);

  అమరిక_des_కీలకం(కీలకాలు + 7, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 8), &ks, DES_ENCRYPT);

  అమరిక_des_కీలకం(కీలకాలు + 14, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 16), &ks, DES_ENCRYPT);
#లేదావేళ defined(USE_WOLFSSL_DES)
  Des des;
  అమరిక_des_కీలకం(కీలకాలు, &des);
  wc_Des_EcbEncrypt(&des, results, plaintext, DES_కీలకం_కొలత);
  అమరిక_des_కీలకం(కీలకాలు + 7, &des);
  wc_Des_EcbEncrypt(&des, results + 8, plaintext, DES_కీలకం_కొలత);
  అమరిక_des_కీలకం(కీలకాలు + 14, &des);
  wc_Des_EcbEncrypt(&des, results + 16, plaintext, DES_కీలకం_కొలత);
#లేదావేళ defined(USE_GNUTLS_DES)
  నిర్మాణం des_ctx des;
  అమరిక_des_కీలకం(కీలకాలు, &des);
  des_encrypt(&des, 8, results, plaintext);
  అమరిక_des_కీలకం(కీలకాలు + 7, &des);
  des_encrypt(&des, 8, results + 8, plaintext);
  అమరిక_des_కీలకం(కీలకాలు + 14, &des);
  des_encrypt(&des, 8, results + 16, plaintext);
#లేదావేళ defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
  encrypt_des(plaintext, results, కీలకాలు);
  encrypt_des(plaintext, results + 8, కీలకాలు + 7);
  encrypt_des(plaintext, results + 16, కీలకాలు + 14);
#లేకపోతే
  (శూన్య)కీలకాలు;
  (శూన్య)plaintext;
  (శూన్య)results;
#అయితే_ముగింపు
}

/*
 * Set up lanmanager hashed password
 */
CURLcode Curl_ntlm_core_mk_lm_hash(స్థిరరాశి అక్షరం *password,
                                   గుర్తులేని అక్షరం *lmbuffer /* 21 bytes */)
{
  గుర్తులేని అక్షరం pw[14];
  స్థిర స్థిరరాశి గుర్తులేని అక్షరం magic[] = {
    0x4B, 0x47, 0x53, 0x21, 0x40, 0x23, 0x24, 0x25 /* i.e. KGS!@#$% */
  };
  size_t len = CURLMIN(అక్షరశ్రేణి_పొడవు(password), 14);

  Curl_strntoupper((అక్షరం *)pw, password, len);
  memset(&pw[len], 0, 14 - len);

  {
    /* Create LanManager hashed password. */
#నిర్వచితమైతే USE_OPENSSL_DES
    DES_కీలకం_కాలపట్టిక ks;

    అమరిక_des_కీలకం(pw, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)lmbuffer, &ks, DES_ENCRYPT);

    అమరిక_des_కీలకం(pw + 7, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)(lmbuffer + 8), &ks, DES_ENCRYPT);
#లేదావేళ defined(USE_WOLFSSL_DES)
    Des des;
    అమరిక_des_కీలకం(pw, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer, magic, DES_కీలకం_కొలత);
    అమరిక_des_కీలకం(pw + 7, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer + 8, magic, DES_కీలకం_కొలత);
#లేదావేళ defined(USE_GNUTLS_DES)
    నిర్మాణం des_ctx des;
    అమరిక_des_కీలకం(pw, &des);
    des_encrypt(&des, 8, lmbuffer, magic);
    అమరిక_des_కీలకం(pw + 7, &des);
    des_encrypt(&des, 8, lmbuffer + 8, magic);
#లేదావేళ defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
    encrypt_des(magic, lmbuffer, pw);
    encrypt_des(magic, lmbuffer + 8, pw + 7);
#అయితే_ముగింపు

    memset(lmbuffer + 16, 0, 21 - 16);
  }

  తిరిగి CURLE_OK;
}

స్థిర శూన్య ascii_గమ్యం_unicode_le(గుర్తులేని అక్షరం *dest, స్థిరరాశి అక్షరం *src,
                                size_t srclen)
{
  size_t i;
  కొరకు(i = 0; i < srclen; i++) {
    dest[2 * i] = (గుర్తులేని అక్షరం)src[i];
    dest[(2 * i) + 1] = '\0';
  }
}

/*
 * Set up nt hashed passwords
 * @unittest: 1600
 */
CURLcode Curl_ntlm_core_mk_nt_hash(స్థిరరాశి అక్షరం *password,
                                   గుర్తులేని అక్షరం *ntbuffer /* 21 bytes */)
{
  size_t len = అక్షరశ్రేణి_పొడవు(password);
  గుర్తులేని అక్షరం *pw;
  CURLcode ఫలితం;
  ఒకవేళ(len > SIZE_MAX / 2) /* avoid integer overflow */
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  pw = len ? curlx_malloc(len * 2) : (గుర్తులేని అక్షరం *)curlx_strdup("");
  ఒకవేళ(!pw)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ascii_గమ్యం_unicode_le(pw, password, len);

  /* Create NT hashed password. */
  ఫలితం = Curl_md4it(ntbuffer, pw, 2 * len);
  ఒకవేళ(!ఫలితం)
    memset(ntbuffer + 16, 0, 21 - 16);

  curlx_free(pw);

  తిరిగి ఫలితం;
}

#నిర్వచితం_కాకపోతే USE_WINDOWS_SSPI

#నిర్వచించు NTLMv2_BLOB_SIGNATURE "\x01\x01\x00\x00"
#నిర్వచించు NTLMv2_BLOB_LEN       (44 - 16 + ntlm->లక్ష్యం_info_len + 4)

/* Timestamp in tenths of a microsecond since January 1, 1601 00:00:00 UTC. */
నిర్మాణం ms_filetime {
  గుర్తులేని పూర్ణాంకం dw_దిగువ_Date_సమయం;
  గుర్తులేని పూర్ణాంకం dw_ఎగువ_Date_సమయం;
};

/* Convert a time_t to an MS FILETIME (MS-DTYP section 2.3.3). */
స్థిర శూన్య time2filetime(నిర్మాణం ms_filetime *ft, time_t t)
{
#ఒకవేళ SIZEOF_సమయం_T > 4
  t = (t + (curl_off_t)11644473600) * 10000000;
  ft->dw_దిగువ_Date_సమయం = (గుర్తులేని పూర్ణాంకం)(t & 0xFFFFFFFF);
  ft->dw_ఎగువ_Date_సమయం = (గుర్తులేని పూర్ణాంకం)(t >> 32);
#లేకపోతే
  గుర్తులేని పూర్ణాంకం r, s;
  గుర్తులేని పూర్ణాంకం i;

  ft->dw_దిగువ_Date_సమయం = (గుర్తులేని పూర్ణాంకం)(t & 0xFFFFFFFF);
  ft->dw_ఎగువ_Date_సమయం = 0;

#నిర్వచితం_కాకపోతే HAVE_సమయం_T_UNSIGNED
  /* Extend sign if needed. */
  ఒకవేళ(ft->dw_దిగువ_Date_సమయం & 0x80000000)
    ft->dw_ఎగువ_Date_సమయం = ~(గుర్తులేని పూర్ణాంకం)0;
#అయితే_ముగింపు

  /* Bias seconds to Jan 1, 1601.
     134774 days = 11644473600 seconds = 0x2B6109100 */
  r = ft->dw_దిగువ_Date_సమయం;
  ft->dw_దిగువ_Date_సమయం = (ft->dw_దిగువ_Date_సమయం + 0xB6109100U) & 0xFFFFFFFF;
  ft->dw_ఎగువ_Date_సమయం += ft->dw_దిగువ_Date_సమయం < r ? 0x03 : 0x02;

  /* Convert to tenths of microseconds. */
  ft->dw_ఎగువ_Date_సమయం *= 10000000;
  i = 32;
  చేయి {
    i -= 8;
    s = ((ft->dw_దిగువ_Date_సమయం >> i) & 0xFF) * (10000000 - 1);
    r = (s << i) & 0xFFFFFFFF;
    s >>= 1;   /* Split shift to avoid width overflow. */
    s >>= 31 - i;
    ft->dw_దిగువ_Date_సమయం = (ft->dw_దిగువ_Date_సమయం + r) & 0xFFFFFFFF;
    ఒకవేళ(ft->dw_దిగువ_Date_సమయం < r)
      s++;
    ft->dw_ఎగువ_Date_సమయం += s;
  } అయితే(i);
  ft->dw_ఎగువ_Date_సమయం &= 0xFFFFFFFF;
#అయితే_ముగింపు
}

స్థిర శూన్య ascii_uppercase_గమ్యం_unicode_le(గుర్తులేని అక్షరం *dest,
                                          స్థిరరాశి అక్షరం *src, size_t srclen)
{
  size_t i;
  కొరకు(i = 0; i < srclen; i++) {
    dest[2 * i] = (గుర్తులేని అక్షరం)(Curl_raw_toupper(src[i]));
    dest[(2 * i) + 1] = '\0';
  }
}

/* This creates the NTLMv2 hash by using NTLM hash as the key and Unicode
 * (uppercase UserName + Domain) as the data
 */
CURLcode Curl_ntlm_core_mk_ntlmv2_hash(స్థిరరాశి అక్షరం *user, size_t userlen,
                                       స్థిరరాశి అక్షరం *domain, size_t domlen,
                                       గుర్తులేని అక్షరం *ntlmhash,
                                       గుర్తులేని అక్షరం *ntlmv2hash)
{
  /* Unicode representation */
  size_t తటస్థం_len;
  గుర్తులేని అక్షరం *తటస్థం;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ((userlen > CURL_MAX_ఆదానం_నిడివి) || (domlen > CURL_MAX_ఆదానం_నిడివి))
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  తటస్థం_len = (userlen + domlen) * 2;
  తటస్థం = curlx_malloc(తటస్థం_len + 1);

  ఒకవేళ(!తటస్థం)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ascii_uppercase_గమ్యం_unicode_le(తటస్థం, user, userlen);
  ascii_గమ్యం_unicode_le(తటస్థం + (userlen << 1), domain, domlen);

  ఫలితం = Curl_hmacit(&Curl_HMAC_MD5, ntlmhash, 16, తటస్థం, తటస్థం_len,
                       ntlmv2hash);
  curlx_free(తటస్థం);

  తిరిగి ఫలితం;
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
CURLcode Curl_ntlm_core_mk_ntlmv2_resp(స్థిరరాశి గుర్తులేని అక్షరం *ntlmv2hash,
                                       స్థిరరాశి గుర్తులేని అక్షరం *challenge_client,
                                       స్థిరరాశి నిర్మాణం ntlmdata *ntlm,
                                       గుర్తులేని అక్షరం **ntresp,
                                       గుర్తులేని పూర్ణాంకం *ntresp_len)
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

  గుర్తులేని పూర్ణాంకం len = 0;
  గుర్తులేని అక్షరం *ptr = ఖాళీ;
  గుర్తులేని అక్షరం hmac_output[HMAC_MD5_నిడివి];
  నిర్మాణం ms_filetime tw;

  CURLcode ఫలితం = CURLE_OK;

  /* Calculate the timestamp */
#నిర్వచితమైతే DEBUGBUILD
  అక్షరం *force_timestamp = getenv("CURL_FORCETIME");
  ఒకవేళ(force_timestamp)
    time2filetime(&tw, (time_t)0);
  లేకపోతే
#అయితే_ముగింపు
    time2filetime(&tw, సమయం(ఖాళీ));

  /* Calculate the response len */
  len = HMAC_MD5_నిడివి + NTLMv2_BLOB_LEN;

  /* Allocate the response */
  ptr = curlx_calloc(1, len);
  ఒకవేళ(!ptr)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  /* Create the BLOB structure */
  curl_msnprintf((అక్షరం *)ptr + HMAC_MD5_నిడివి, NTLMv2_BLOB_LEN,
                 "%c%c%c%c"           /* NTLMv2_BLOB_SIGNATURE */
                 "%c%c%c%c"           /* Reserved = 0 */
                 "%c%c%c%c%c%c%c%c",  /* Timestamp */
                 NTLMv2_BLOB_SIGNATURE[0], NTLMv2_BLOB_SIGNATURE[1],
                 NTLMv2_BLOB_SIGNATURE[2], NTLMv2_BLOB_SIGNATURE[3],
                 0, 0, 0, 0,
                 LONGQUARTET(tw.dw_దిగువ_Date_సమయం),
                 LONGQUARTET(tw.dw_ఎగువ_Date_సమయం));

  జ్ఞాపకం_నకలు(ptr + 32, challenge_client, 8);
  ఒకవేళ(ntlm->లక్ష్యం_info_len)
    జ్ఞాపకం_నకలు(ptr + 44, ntlm->లక్ష్యం_info, ntlm->లక్ష్యం_info_len);

  /* Concatenate the Type 2 challenge with the BLOB and do HMAC MD5 */
  జ్ఞాపకం_నకలు(ptr + 8, &ntlm->nonce[0], 8);
  ఫలితం = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, HMAC_MD5_నిడివి, ptr + 8,
                       NTLMv2_BLOB_LEN + 8, hmac_output);
  ఒకవేళ(ఫలితం) {
    curlx_free(ptr);
    తిరిగి ఫలితం;
  }

  /* Concatenate the HMAC MD5 output  with the BLOB */
  జ్ఞాపకం_నకలు(ptr, hmac_output, HMAC_MD5_నిడివి);

  /* Return the response */
  *ntresp = ptr;
  *ntresp_len = len;

  తిరిగి ఫలితం;
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
CURLcode Curl_ntlm_core_mk_lmv2_resp(స్థిరరాశి గుర్తులేని అక్షరం *ntlmv2hash,
                                     స్థిరరాశి గుర్తులేని అక్షరం *challenge_client,
                                     స్థిరరాశి గుర్తులేని అక్షరం *challenge_server,
                                     గుర్తులేని అక్షరం *lmresp)
{
  గుర్తులేని అక్షరం దత్తాంశం[16];
  గుర్తులేని అక్షరం hmac_output[16];
  CURLcode ఫలితం = CURLE_OK;

  జ్ఞాపకం_నకలు(&దత్తాంశం[0], challenge_server, 8);
  జ్ఞాపకం_నకలు(&దత్తాంశం[8], challenge_client, 8);

  ఫలితం = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, 16, &దత్తాంశం[0], 16,
                       hmac_output);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  /* Concatenate the HMAC MD5 output with the client nonce */
  జ్ఞాపకం_నకలు(lmresp, hmac_output, 16);
  జ్ఞాపకం_నకలు(lmresp + 16, challenge_client, 8);

  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* !USE_WINDOWS_SSPI */

#అయితే_ముగింపు /* USE_CURL_NTLM_CORE */
