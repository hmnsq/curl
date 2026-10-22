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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_CURL_NTLM_CORE

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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GNUTLS
#ਅਨ੍ਤਰ੍ਭਾਵ <nettle/version.h>
#ਜੇ NETTLE_ਸੰਸਕਰਣ_MAJOR < 4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_GNUTLS_DES
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

#ਜੇ defined(USE_OPENSSL) && defined(HAVE_DES_ECB_ENCRYPT)

#  include <openssl/des.h>
#  ifdef OPENSSL_IS_AWSLC  /* for versions 1.2.0 to 1.30.1 */
#    define DES_ਸੈੱਟ_ਕੁੰਜੀ_unchecked (ਖਾਲੀ)DES_ਸੈੱਟ_ਕੁੰਜੀ
#  endif
#  define USE_OPENSSL_DES

#ਜਾਂਜੇ defined(USE_WOLFSSL) && defined(HAVE_WC_DES_ECBENCRYPT)

#  include <wolfssl/options.h>
#  include <wolfssl/wolfcrypt/des3.h>
#  define USE_WOLFSSL_DES

#ਜਾਂਜੇ defined(USE_GNUTLS_DES)
#  include <nettle/des.h>
#  define USE_CURL_DES_ਸੈੱਟ_ODD_PARITY
#ਜਾਂਜੇ defined(USE_MBEDTLS) && defined(HAVE_MBEDTLS_DES_CRYPT_ECB)
#  include <mbedtls/version.h>
#  ਜੇ MBEDTLS_ਸੰਸਕਰਣ_ਸੰਖਿਆ < 0x03020000
#  error "mbedTLS 3.2.0 or greater required"
#  endif
#  include <mbedtls/des.h>
#  define USE_MBEDTLS_DES
#ਜਾਂਜੇ defined(USE_OS400CRYPTO)
#  include "cipher.mih"  /* mih/cipher */
#  define USE_CURL_DES_ਸੈੱਟ_ODD_PARITY
#ਜਾਂਜੇ defined(USE_WIN32_CRYPTO)
#  include <wincrypt.h>
#  define USE_CURL_DES_ਸੈੱਟ_ODD_PARITY
#ਨਹੀਂਤਾਂ
#  error "cannot compile NTLM support without a crypto library with DES."
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_ntlm_core.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_md5.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_hmac.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_md4.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vauth/vauth.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_CURL_DES_ਸੈੱਟ_ODD_PARITY
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
ਸਥਿਰ ਖਾਲੀ curl_des_ਸੈੱਟ_odd_parity(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਬਾਈਟਾਂ, size_t len)
{
  size_t i;

  ਲਈ(i = 0; i < len; i++) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ b = ਬਾਈਟਾਂ[i];

    bool needs_parity = (((b >> 7) ^ (b >> 6) ^ (b >> 5) ^
                          (b >> 4) ^ (b >> 3) ^ (b >> 2) ^
                          (b >> 1)) & 0x01) == 0;

    ਜੇ(needs_parity)
      ਬਾਈਟਾਂ[i] |= 0x01;
    ਨਹੀਂਤਾਂ
      ਬਾਈਟਾਂ[i] &= 0xfe;
  }
}
#ਜੇ_ਅੰਤ /* USE_CURL_DES_SET_ODD_PARITY */

/*
 * Turns a 56-bit key into being 64-bit wide.
 */
ਸਥਿਰ ਖਾਲੀ extend_ਕੁੰਜੀ_56_ਮੰਜ਼ਿਲ_64(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਕੁੰਜੀ_56, ਅੱਖਰ *ਕੁੰਜੀ)
{
  ਕੁੰਜੀ[0] = (ਅੱਖਰ)ਕੁੰਜੀ_56[0];
  ਕੁੰਜੀ[1] = (ਅੱਖਰ)(((ਕੁੰਜੀ_56[0] << 7) & 0xFF) | (ਕੁੰਜੀ_56[1] >> 1));
  ਕੁੰਜੀ[2] = (ਅੱਖਰ)(((ਕੁੰਜੀ_56[1] << 6) & 0xFF) | (ਕੁੰਜੀ_56[2] >> 2));
  ਕੁੰਜੀ[3] = (ਅੱਖਰ)(((ਕੁੰਜੀ_56[2] << 5) & 0xFF) | (ਕੁੰਜੀ_56[3] >> 3));
  ਕੁੰਜੀ[4] = (ਅੱਖਰ)(((ਕੁੰਜੀ_56[3] << 4) & 0xFF) | (ਕੁੰਜੀ_56[4] >> 4));
  ਕੁੰਜੀ[5] = (ਅੱਖਰ)(((ਕੁੰਜੀ_56[4] << 3) & 0xFF) | (ਕੁੰਜੀ_56[5] >> 5));
  ਕੁੰਜੀ[6] = (ਅੱਖਰ)(((ਕੁੰਜੀ_56[5] << 2) & 0xFF) | (ਕੁੰਜੀ_56[6] >> 6));
  ਕੁੰਜੀ[7] = (ਅੱਖਰ)((ਕੁੰਜੀ_56[6] << 1) & 0xFF);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL_DES
/*
 * Turns a 56-bit key into a 64-bit, odd parity key and sets the key. The
 * key schedule ks is also set.
 */
ਸਥਿਰ ਖਾਲੀ ਤਿਆਰੀ_des_ਕੁੰਜੀ(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਕੁੰਜੀ_56, DES_ਕੁੰਜੀ_ਸਮਾਂ_ਸਾਰਣੀ *ks)
{
  DES_cblock ਕੁੰਜੀ;

  /* Expand the 56-bit key to 64 bits */
  extend_ਕੁੰਜੀ_56_ਮੰਜ਼ਿਲ_64(ਕੁੰਜੀ_56, (ਅੱਖਰ *)&ਕੁੰਜੀ);

  /* Set the key parity to odd */
  DES_ਸੈੱਟ_odd_parity(&ਕੁੰਜੀ);

  /* Set the key */
  DES_ਸੈੱਟ_ਕੁੰਜੀ_unchecked(&ਕੁੰਜੀ, ks);
}

#ਜਾਂਜੇ defined(USE_WOLFSSL_DES)
ਸਥਿਰ ਖਾਲੀ ਤਿਆਰੀ_des_ਕੁੰਜੀ(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਕੁੰਜੀ_56, Des *des)
{
  byte ਕੁੰਜੀ[8];

  /* Expand the 56-bit key to 64 bits */
  extend_ਕੁੰਜੀ_56_ਮੰਜ਼ਿਲ_64(ਕੁੰਜੀ_56, (ਅੱਖਰ *)ਕੁੰਜੀ);

  /* Set the key */
  wc_Des_ਸੈੱਟ_ਕੁੰਜੀ(des, ਕੁੰਜੀ, ਨਿਰਾ, 0);
}

#ਜਾਂਜੇ defined(USE_GNUTLS_DES)
ਸਥਿਰ ਖਾਲੀ ਤਿਆਰੀ_des_ਕੁੰਜੀ(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਕੁੰਜੀ_56, ਬਣਤਰ des_ctx *des)
{
  ਅੱਖਰ ਕੁੰਜੀ[8];

  /* Expand the 56-bit key to 64 bits */
  extend_ਕੁੰਜੀ_56_ਮੰਜ਼ਿਲ_64(ਕੁੰਜੀ_56, ਕੁੰਜੀ);

  /* Set the key parity to odd */
  curl_des_ਸੈੱਟ_odd_parity((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ਕੁੰਜੀ, ਆਕਾਰ(ਕੁੰਜੀ));

  /* Set the key */
  des_ਸੈੱਟ_ਕੁੰਜੀ(des, (ਅਚਲ uint8_t *)ਕੁੰਜੀ);
}

#ਜਾਂਜੇ defined(USE_MBEDTLS_DES)
ਸਥਿਰ bool encrypt_des(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਵਿੱਚ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਬਾਹਰਲਾ,
                        ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਕੁੰਜੀ_56)
{
  mbedtls_des_context ctx;
  ਅੱਖਰ ਕੁੰਜੀ[8];

  /* Expand the 56-bit key to 64 bits */
  extend_ਕੁੰਜੀ_56_ਮੰਜ਼ਿਲ_64(ਕੁੰਜੀ_56, ਕੁੰਜੀ);

  /* Set the key parity to odd */
  mbedtls_des_ਕੁੰਜੀ_ਸੈੱਟ_parity((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ਕੁੰਜੀ);

  /* Perform the encryption */
  mbedtls_des_init(&ctx);
  mbedtls_des_setkey_enc(&ctx, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ਕੁੰਜੀ);
  ਵਾਪਸ mbedtls_des_crypt_ecb(&ctx, ਵਿੱਚ, ਬਾਹਰਲਾ) == 0;
}

#ਜਾਂਜੇ defined(USE_OS400CRYPTO)
ਸਥਿਰ bool encrypt_des(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਵਿੱਚ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਬਾਹਰਲਾ,
                        ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਕੁੰਜੀ_56)
{
  ਅੱਖਰ ਕੁੰਜੀ[8];
  _CIPHER_Control_T ctl;

  /* Setup the cipher control structure */
  ctl.Func_ID = ENCRYPT_ONLY;
  ctl.ਅੰਕੜੇ_Len = ਆਕਾਰ(ਕੁੰਜੀ);

  /* Expand the 56-bit key to 64 bits */
  extend_ਕੁੰਜੀ_56_ਮੰਜ਼ਿਲ_64(ਕੁੰਜੀ_56, ctl.Crypto_ਕੁੰਜੀ);

  /* Set the key parity to odd */
  curl_des_ਸੈੱਟ_odd_parity((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ctl.Crypto_ਕੁੰਜੀ, ctl.ਅੰਕੜੇ_Len);

  /* Perform the encryption */
  _CIPHER((_SPCPTR *)&ਬਾਹਰਲਾ, &ctl, (_SPCPTR *)&ਵਿੱਚ);

  ਵਾਪਸ TRUE;
}

#ਜਾਂਜੇ defined(USE_WIN32_CRYPTO)
ਸਥਿਰ bool encrypt_des(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਵਿੱਚ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਬਾਹਰਲਾ,
                        ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਕੁੰਜੀ_56)
{
  HCRYPTPROV hprov;
  HCRYPTKEY hkey;
  ਬਣਤਰ {
    BLOBHEADER hdr;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ len;
    ਅੱਖਰ ਕੁੰਜੀ[8];
  } blob;
  DWORD len = 8;
  BOOL res;

  /* Acquire the crypto provider */
  ਜੇ(!CryptAcquireContext(&hprov, ਨਿਰਾ, ਨਿਰਾ, PROV_RSA_ਭਰਿਆ,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    ਵਾਪਸ FALSE;

  /* Setup the key blob structure */
  memset(&blob, 0, ਆਕਾਰ(blob));
  blob.hdr.bType = PLAINTEXTKEYBLOB;
  blob.hdr.b_ਸੰਸਕਰਣ = 2;
  blob.hdr.ai_ਕੁੰਜੀ_Alg = CALG_DES;
  blob.len = ਆਕਾਰ(blob.ਕੁੰਜੀ);

  /* Expand the 56-bit key to 64 bits */
  extend_ਕੁੰਜੀ_56_ਮੰਜ਼ਿਲ_64(ਕੁੰਜੀ_56, blob.ਕੁੰਜੀ);

  /* Set the key parity to odd */
  curl_des_ਸੈੱਟ_odd_parity((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)blob.ਕੁੰਜੀ, ਆਕਾਰ(blob.ਕੁੰਜੀ));

  /* Import the key */
  ਜੇ(!Crypt_Import_ਕੁੰਜੀ(hprov, (BYTE *)&blob, ਆਕਾਰ(blob), 0, 0, &hkey)) {
    CryptReleaseContext(hprov, 0);

    ਵਾਪਸ FALSE;
  }

  ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਬਾਹਰਲਾ, ਵਿੱਚ, 8);

  /* Perform the encryption */
  res = CryptEncrypt(hkey, 0, FALSE, 0, ਬਾਹਰਲਾ, &len, len);

  Crypt_Destroy_ਕੁੰਜੀ(hkey);
  CryptReleaseContext(hprov, 0);

  ਵਾਪਸ res;
}

#ਜੇ_ਅੰਤ /* crypto backends */

/*
 * takes a 21-byte array and treats it as 3 56-bit DES keys. The
 * 8-byte plaintext is encrypted with each key and the resulting 24
 * bytes are stored in the results array.
 */
ਖਾਲੀ Curl_ntlm_core_lm_resp(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਕੁੰਜੀਆਂ,
                            ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *plaintext,
                            ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *results)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL_DES
  DES_ਕੁੰਜੀ_ਸਮਾਂ_ਸਾਰਣੀ ks;

  ਤਿਆਰੀ_des_ਕੁੰਜੀ(ਕੁੰਜੀਆਂ, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)results, &ks, DES_ENCRYPT);

  ਤਿਆਰੀ_des_ਕੁੰਜੀ(ਕੁੰਜੀਆਂ + 7, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 8), &ks, DES_ENCRYPT);

  ਤਿਆਰੀ_des_ਕੁੰਜੀ(ਕੁੰਜੀਆਂ + 14, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 16), &ks, DES_ENCRYPT);
#ਜਾਂਜੇ defined(USE_WOLFSSL_DES)
  Des des;
  ਤਿਆਰੀ_des_ਕੁੰਜੀ(ਕੁੰਜੀਆਂ, &des);
  wc_Des_EcbEncrypt(&des, results, plaintext, DES_ਕੁੰਜੀ_ਮਾਪ);
  ਤਿਆਰੀ_des_ਕੁੰਜੀ(ਕੁੰਜੀਆਂ + 7, &des);
  wc_Des_EcbEncrypt(&des, results + 8, plaintext, DES_ਕੁੰਜੀ_ਮਾਪ);
  ਤਿਆਰੀ_des_ਕੁੰਜੀ(ਕੁੰਜੀਆਂ + 14, &des);
  wc_Des_EcbEncrypt(&des, results + 16, plaintext, DES_ਕੁੰਜੀ_ਮਾਪ);
#ਜਾਂਜੇ defined(USE_GNUTLS_DES)
  ਬਣਤਰ des_ctx des;
  ਤਿਆਰੀ_des_ਕੁੰਜੀ(ਕੁੰਜੀਆਂ, &des);
  des_encrypt(&des, 8, results, plaintext);
  ਤਿਆਰੀ_des_ਕੁੰਜੀ(ਕੁੰਜੀਆਂ + 7, &des);
  des_encrypt(&des, 8, results + 8, plaintext);
  ਤਿਆਰੀ_des_ਕੁੰਜੀ(ਕੁੰਜੀਆਂ + 14, &des);
  des_encrypt(&des, 8, results + 16, plaintext);
#ਜਾਂਜੇ defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
  encrypt_des(plaintext, results, ਕੁੰਜੀਆਂ);
  encrypt_des(plaintext, results + 8, ਕੁੰਜੀਆਂ + 7);
  encrypt_des(plaintext, results + 16, ਕੁੰਜੀਆਂ + 14);
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ਕੁੰਜੀਆਂ;
  (ਖਾਲੀ)plaintext;
  (ਖਾਲੀ)results;
#ਜੇ_ਅੰਤ
}

/*
 * Set up lanmanager hashed password
 */
CURLcode Curl_ntlm_core_mk_lm_hash(ਅਚਲ ਅੱਖਰ *password,
                                   ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *lmbuffer /* 21 bytes */)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ pw[14];
  ਸਥਿਰ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ magic[] = {
    0x4B, 0x47, 0x53, 0x21, 0x40, 0x23, 0x24, 0x25 /* i.e. KGS!@#$% */
  };
  size_t len = CURLMIN(ਲੜੀ_ਲੰਬਾਈ(password), 14);

  Curl_strntoupper((ਅੱਖਰ *)pw, password, len);
  memset(&pw[len], 0, 14 - len);

  {
    /* Create LanManager hashed password. */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL_DES
    DES_ਕੁੰਜੀ_ਸਮਾਂ_ਸਾਰਣੀ ks;

    ਤਿਆਰੀ_des_ਕੁੰਜੀ(pw, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)lmbuffer, &ks, DES_ENCRYPT);

    ਤਿਆਰੀ_des_ਕੁੰਜੀ(pw + 7, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)(lmbuffer + 8), &ks, DES_ENCRYPT);
#ਜਾਂਜੇ defined(USE_WOLFSSL_DES)
    Des des;
    ਤਿਆਰੀ_des_ਕੁੰਜੀ(pw, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer, magic, DES_ਕੁੰਜੀ_ਮਾਪ);
    ਤਿਆਰੀ_des_ਕੁੰਜੀ(pw + 7, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer + 8, magic, DES_ਕੁੰਜੀ_ਮਾਪ);
#ਜਾਂਜੇ defined(USE_GNUTLS_DES)
    ਬਣਤਰ des_ctx des;
    ਤਿਆਰੀ_des_ਕੁੰਜੀ(pw, &des);
    des_encrypt(&des, 8, lmbuffer, magic);
    ਤਿਆਰੀ_des_ਕੁੰਜੀ(pw + 7, &des);
    des_encrypt(&des, 8, lmbuffer + 8, magic);
#ਜਾਂਜੇ defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
    encrypt_des(magic, lmbuffer, pw);
    encrypt_des(magic, lmbuffer + 8, pw + 7);
#ਜੇ_ਅੰਤ

    memset(lmbuffer + 16, 0, 21 - 16);
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ ascii_ਮੰਜ਼ਿਲ_unicode_le(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *dest, ਅਚਲ ਅੱਖਰ *src,
                                size_t srclen)
{
  size_t i;
  ਲਈ(i = 0; i < srclen; i++) {
    dest[2 * i] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)src[i];
    dest[(2 * i) + 1] = '\0';
  }
}

/*
 * Set up nt hashed passwords
 * @unittest: 1600
 */
CURLcode Curl_ntlm_core_mk_nt_hash(ਅਚਲ ਅੱਖਰ *password,
                                   ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ntbuffer /* 21 bytes */)
{
  size_t len = ਲੜੀ_ਲੰਬਾਈ(password);
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *pw;
  CURLcode ਨਤੀਜਾ;
  ਜੇ(len > SIZE_MAX / 2) /* avoid integer overflow */
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  pw = len ? curlx_malloc(len * 2) : (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)curlx_strdup("");
  ਜੇ(!pw)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ascii_ਮੰਜ਼ਿਲ_unicode_le(pw, password, len);

  /* Create NT hashed password. */
  ਨਤੀਜਾ = Curl_md4it(ntbuffer, pw, 2 * len);
  ਜੇ(!ਨਤੀਜਾ)
    memset(ntbuffer + 16, 0, 21 - 16);

  curlx_free(pw);

  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLMv2_BLOB_SIGNATURE "\x01\x01\x00\x00"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NTLMv2_BLOB_LEN       (44 - 16 + ntlm->ਨਿਸ਼ਾਨਾ_info_len + 4)

/* Timestamp in tenths of a microsecond since January 1, 1601 00:00:00 UTC. */
ਬਣਤਰ ms_filetime {
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ dw_ਨੀਵਾਂ_Date_ਸਮਾਂ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ dw_ਉੱਚਾ_Date_ਸਮਾਂ;
};

/* Convert a time_t to an MS FILETIME (MS-DTYP section 2.3.3). */
ਸਥਿਰ ਖਾਲੀ time2filetime(ਬਣਤਰ ms_filetime *ft, time_t t)
{
#ਜੇ SIZEOF_ਸਮਾਂ_T > 4
  t = (t + (curl_off_t)11644473600) * 10000000;
  ft->dw_ਨੀਵਾਂ_Date_ਸਮਾਂ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(t & 0xFFFFFFFF);
  ft->dw_ਉੱਚਾ_Date_ਸਮਾਂ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(t >> 32);
#ਨਹੀਂਤਾਂ
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ r, s;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;

  ft->dw_ਨੀਵਾਂ_Date_ਸਮਾਂ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(t & 0xFFFFFFFF);
  ft->dw_ਉੱਚਾ_Date_ਸਮਾਂ = 0;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_ਸਮਾਂ_T_UNSIGNED
  /* Extend sign if needed. */
  ਜੇ(ft->dw_ਨੀਵਾਂ_Date_ਸਮਾਂ & 0x80000000)
    ft->dw_ਉੱਚਾ_Date_ਸਮਾਂ = ~(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)0;
#ਜੇ_ਅੰਤ

  /* Bias seconds to Jan 1, 1601.
     134774 days = 11644473600 seconds = 0x2B6109100 */
  r = ft->dw_ਨੀਵਾਂ_Date_ਸਮਾਂ;
  ft->dw_ਨੀਵਾਂ_Date_ਸਮਾਂ = (ft->dw_ਨੀਵਾਂ_Date_ਸਮਾਂ + 0xB6109100U) & 0xFFFFFFFF;
  ft->dw_ਉੱਚਾ_Date_ਸਮਾਂ += ft->dw_ਨੀਵਾਂ_Date_ਸਮਾਂ < r ? 0x03 : 0x02;

  /* Convert to tenths of microseconds. */
  ft->dw_ਉੱਚਾ_Date_ਸਮਾਂ *= 10000000;
  i = 32;
  ਕਰੋ {
    i -= 8;
    s = ((ft->dw_ਨੀਵਾਂ_Date_ਸਮਾਂ >> i) & 0xFF) * (10000000 - 1);
    r = (s << i) & 0xFFFFFFFF;
    s >>= 1;   /* Split shift to avoid width overflow. */
    s >>= 31 - i;
    ft->dw_ਨੀਵਾਂ_Date_ਸਮਾਂ = (ft->dw_ਨੀਵਾਂ_Date_ਸਮਾਂ + r) & 0xFFFFFFFF;
    ਜੇ(ft->dw_ਨੀਵਾਂ_Date_ਸਮਾਂ < r)
      s++;
    ft->dw_ਉੱਚਾ_Date_ਸਮਾਂ += s;
  } ਜਦੋਂ(i);
  ft->dw_ਉੱਚਾ_Date_ਸਮਾਂ &= 0xFFFFFFFF;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਖਾਲੀ ascii_uppercase_ਮੰਜ਼ਿਲ_unicode_le(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *dest,
                                          ਅਚਲ ਅੱਖਰ *src, size_t srclen)
{
  size_t i;
  ਲਈ(i = 0; i < srclen; i++) {
    dest[2 * i] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(Curl_raw_toupper(src[i]));
    dest[(2 * i) + 1] = '\0';
  }
}

/* This creates the NTLMv2 hash by using NTLM hash as the key and Unicode
 * (uppercase UserName + Domain) as the data
 */
CURLcode Curl_ntlm_core_mk_ntlmv2_hash(ਅਚਲ ਅੱਖਰ *user, size_t userlen,
                                       ਅਚਲ ਅੱਖਰ *domain, size_t domlen,
                                       ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ntlmhash,
                                       ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ntlmv2hash)
{
  /* Unicode representation */
  size_t ਪਛਾਣ_ਤੱਤ_len;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਪਛਾਣ_ਤੱਤ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ((userlen > CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ) || (domlen > CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ))
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਪਛਾਣ_ਤੱਤ_len = (userlen + domlen) * 2;
  ਪਛਾਣ_ਤੱਤ = curlx_malloc(ਪਛਾਣ_ਤੱਤ_len + 1);

  ਜੇ(!ਪਛਾਣ_ਤੱਤ)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ascii_uppercase_ਮੰਜ਼ਿਲ_unicode_le(ਪਛਾਣ_ਤੱਤ, user, userlen);
  ascii_ਮੰਜ਼ਿਲ_unicode_le(ਪਛਾਣ_ਤੱਤ + (userlen << 1), domain, domlen);

  ਨਤੀਜਾ = Curl_hmacit(&Curl_HMAC_MD5, ntlmhash, 16, ਪਛਾਣ_ਤੱਤ, ਪਛਾਣ_ਤੱਤ_len,
                       ntlmv2hash);
  curlx_free(ਪਛਾਣ_ਤੱਤ);

  ਵਾਪਸ ਨਤੀਜਾ;
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
CURLcode Curl_ntlm_core_mk_ntlmv2_resp(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ntlmv2hash,
                                       ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *challenge_client,
                                       ਅਚਲ ਬਣਤਰ ntlmdata *ntlm,
                                       ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ **ntresp,
                                       ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *ntresp_len)
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

  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ len = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ptr = ਨਿਰਾ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ hmac_output[HMAC_MD5_ਲੰਮਾਈ];
  ਬਣਤਰ ms_filetime tw;

  CURLcode ਨਤੀਜਾ = CURLE_OK;

  /* Calculate the timestamp */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਅੱਖਰ *force_timestamp = getenv("CURL_FORCETIME");
  ਜੇ(force_timestamp)
    time2filetime(&tw, (time_t)0);
  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    time2filetime(&tw, ਸਮਾਂ(ਨਿਰਾ));

  /* Calculate the response len */
  len = HMAC_MD5_ਲੰਮਾਈ + NTLMv2_BLOB_LEN;

  /* Allocate the response */
  ptr = curlx_calloc(1, len);
  ਜੇ(!ptr)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  /* Create the BLOB structure */
  curl_msnprintf((ਅੱਖਰ *)ptr + HMAC_MD5_ਲੰਮਾਈ, NTLMv2_BLOB_LEN,
                 "%c%c%c%c"           /* NTLMv2_BLOB_SIGNATURE */
                 "%c%c%c%c"           /* Reserved = 0 */
                 "%c%c%c%c%c%c%c%c",  /* Timestamp */
                 NTLMv2_BLOB_SIGNATURE[0], NTLMv2_BLOB_SIGNATURE[1],
                 NTLMv2_BLOB_SIGNATURE[2], NTLMv2_BLOB_SIGNATURE[3],
                 0, 0, 0, 0,
                 LONGQUARTET(tw.dw_ਨੀਵਾਂ_Date_ਸਮਾਂ),
                 LONGQUARTET(tw.dw_ਉੱਚਾ_Date_ਸਮਾਂ));

  ਸਮ੍ਰਿਤੀ_ਨਕਲ(ptr + 32, challenge_client, 8);
  ਜੇ(ntlm->ਨਿਸ਼ਾਨਾ_info_len)
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(ptr + 44, ntlm->ਨਿਸ਼ਾਨਾ_info, ntlm->ਨਿਸ਼ਾਨਾ_info_len);

  /* Concatenate the Type 2 challenge with the BLOB and do HMAC MD5 */
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(ptr + 8, &ntlm->nonce[0], 8);
  ਨਤੀਜਾ = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, HMAC_MD5_ਲੰਮਾਈ, ptr + 8,
                       NTLMv2_BLOB_LEN + 8, hmac_output);
  ਜੇ(ਨਤੀਜਾ) {
    curlx_free(ptr);
    ਵਾਪਸ ਨਤੀਜਾ;
  }

  /* Concatenate the HMAC MD5 output  with the BLOB */
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(ptr, hmac_output, HMAC_MD5_ਲੰਮਾਈ);

  /* Return the response */
  *ntresp = ptr;
  *ntresp_len = len;

  ਵਾਪਸ ਨਤੀਜਾ;
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
CURLcode Curl_ntlm_core_mk_lmv2_resp(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ntlmv2hash,
                                     ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *challenge_client,
                                     ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *challenge_server,
                                     ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *lmresp)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਅੰਕੜੇ[16];
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ hmac_output[16];
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਸਮ੍ਰਿਤੀ_ਨਕਲ(&ਅੰਕੜੇ[0], challenge_server, 8);
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(&ਅੰਕੜੇ[8], challenge_client, 8);

  ਨਤੀਜਾ = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, 16, &ਅੰਕੜੇ[0], 16,
                       hmac_output);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* Concatenate the HMAC MD5 output with the client nonce */
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(lmresp, hmac_output, 16);
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(lmresp + 16, challenge_client, 8);

  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* !USE_WINDOWS_SSPI */

#ਜੇ_ਅੰਤ /* USE_CURL_NTLM_CORE */
