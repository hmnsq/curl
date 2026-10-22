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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_CURL_NTLM_CORE

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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GNUTLS
#ಅನ್ತರ್ಭಾವ <nettle/version.h>
#ಆದರೆ NETTLE_ಆವೃತ್ತಿ_MAJOR < 4
#ವ್ಯಾಖ್ಯಾನಿಸು USE_GNUTLS_DES
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(USE_OPENSSL) && defined(HAVE_DES_ECB_ENCRYPT)

#  include <openssl/des.h>
#  ifdef OPENSSL_IS_AWSLC  /* for versions 1.2.0 to 1.30.1 */
#    define DES_ಗಣ_ಕೀಲಿ_unchecked (ಶೂನ್ಯ)DES_ಗಣ_ಕೀಲಿ
#  endif
#  define USE_OPENSSL_DES

#ಅಥವಾಆದರೆ defined(USE_WOLFSSL) && defined(HAVE_WC_DES_ECBENCRYPT)

#  include <wolfssl/options.h>
#  include <wolfssl/wolfcrypt/des3.h>
#  define USE_WOLFSSL_DES

#ಅಥವಾಆದರೆ defined(USE_GNUTLS_DES)
#  include <nettle/des.h>
#  define USE_CURL_DES_ಗಣ_ODD_PARITY
#ಅಥವಾಆದರೆ defined(USE_MBEDTLS) && defined(HAVE_MBEDTLS_DES_CRYPT_ECB)
#  include <mbedtls/version.h>
#  ಆದರೆ MBEDTLS_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ < 0x03020000
#  error "mbedTLS 3.2.0 or greater required"
#  endif
#  include <mbedtls/des.h>
#  define USE_MBEDTLS_DES
#ಅಥವಾಆದರೆ defined(USE_OS400CRYPTO)
#  include "cipher.mih"  /* mih/cipher */
#  define USE_CURL_DES_ಗಣ_ODD_PARITY
#ಅಥವಾಆದರೆ defined(USE_WIN32_CRYPTO)
#  include <wincrypt.h>
#  define USE_CURL_DES_ಗಣ_ODD_PARITY
#ಇಲ್ಲದಿದ್ದರೆ
#  error "cannot compile NTLM support without a crypto library with DES."
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "curl_ntlm_core.h"
#ಅನ್ತರ್ಭಾವ "curl_md5.h"
#ಅನ್ತರ್ಭಾವ "curl_hmac.h"
#ಅನ್ತರ್ಭಾವ "curl_md4.h"
#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_CURL_DES_ಗಣ_ODD_PARITY
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
ಸ್ಥಿರ ಶೂನ್ಯ curl_des_ಗಣ_odd_parity(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಬೈಟುಗಳು, size_t len)
{
  size_t i;

  ಗಾಗಿ(i = 0; i < len; i++) {
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ b = ಬೈಟುಗಳು[i];

    bool needs_parity = (((b >> 7) ^ (b >> 6) ^ (b >> 5) ^
                          (b >> 4) ^ (b >> 3) ^ (b >> 2) ^
                          (b >> 1)) & 0x01) == 0;

    ಆದರೆ(needs_parity)
      ಬೈಟುಗಳು[i] |= 0x01;
    ಇಲ್ಲದಿದ್ದರೆ
      ಬೈಟುಗಳು[i] &= 0xfe;
  }
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_CURL_DES_SET_ODD_PARITY */

/*
 * Turns a 56-bit key into being 64-bit wide.
 */
ಸ್ಥಿರ ಶೂನ್ಯ extend_ಕೀಲಿ_56_ಗಮ್ಯ_64(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಕೀಲಿ_56, ಅಕ್ಷರ *ಕೀಲಿ)
{
  ಕೀಲಿ[0] = (ಅಕ್ಷರ)ಕೀಲಿ_56[0];
  ಕೀಲಿ[1] = (ಅಕ್ಷರ)(((ಕೀಲಿ_56[0] << 7) & 0xFF) | (ಕೀಲಿ_56[1] >> 1));
  ಕೀಲಿ[2] = (ಅಕ್ಷರ)(((ಕೀಲಿ_56[1] << 6) & 0xFF) | (ಕೀಲಿ_56[2] >> 2));
  ಕೀಲಿ[3] = (ಅಕ್ಷರ)(((ಕೀಲಿ_56[2] << 5) & 0xFF) | (ಕೀಲಿ_56[3] >> 3));
  ಕೀಲಿ[4] = (ಅಕ್ಷರ)(((ಕೀಲಿ_56[3] << 4) & 0xFF) | (ಕೀಲಿ_56[4] >> 4));
  ಕೀಲಿ[5] = (ಅಕ್ಷರ)(((ಕೀಲಿ_56[4] << 3) & 0xFF) | (ಕೀಲಿ_56[5] >> 5));
  ಕೀಲಿ[6] = (ಅಕ್ಷರ)(((ಕೀಲಿ_56[5] << 2) & 0xFF) | (ಕೀಲಿ_56[6] >> 6));
  ಕೀಲಿ[7] = (ಅಕ್ಷರ)((ಕೀಲಿ_56[6] << 1) & 0xFF);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL_DES
/*
 * Turns a 56-bit key into a 64-bit, odd parity key and sets the key. The
 * key schedule ks is also set.
 */
ಸ್ಥಿರ ಶೂನ್ಯ ಸಿದ್ಧತೆ_des_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಕೀಲಿ_56, DES_ಕೀಲಿ_ವೇಳಾಪಟ್ಟಿ *ks)
{
  DES_cblock ಕೀಲಿ;

  /* Expand the 56-bit key to 64 bits */
  extend_ಕೀಲಿ_56_ಗಮ್ಯ_64(ಕೀಲಿ_56, (ಅಕ್ಷರ *)&ಕೀಲಿ);

  /* Set the key parity to odd */
  DES_ಗಣ_odd_parity(&ಕೀಲಿ);

  /* Set the key */
  DES_ಗಣ_ಕೀಲಿ_unchecked(&ಕೀಲಿ, ks);
}

#ಅಥವಾಆದರೆ defined(USE_WOLFSSL_DES)
ಸ್ಥಿರ ಶೂನ್ಯ ಸಿದ್ಧತೆ_des_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಕೀಲಿ_56, Des *des)
{
  byte ಕೀಲಿ[8];

  /* Expand the 56-bit key to 64 bits */
  extend_ಕೀಲಿ_56_ಗಮ್ಯ_64(ಕೀಲಿ_56, (ಅಕ್ಷರ *)ಕೀಲಿ);

  /* Set the key */
  wc_Des_ಗಣ_ಕೀಲಿ(des, ಕೀಲಿ, ಖಾಲಿ, 0);
}

#ಅಥವಾಆದರೆ defined(USE_GNUTLS_DES)
ಸ್ಥಿರ ಶೂನ್ಯ ಸಿದ್ಧತೆ_des_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಕೀಲಿ_56, ರಚನೆ des_ctx *des)
{
  ಅಕ್ಷರ ಕೀಲಿ[8];

  /* Expand the 56-bit key to 64 bits */
  extend_ಕೀಲಿ_56_ಗಮ್ಯ_64(ಕೀಲಿ_56, ಕೀಲಿ);

  /* Set the key parity to odd */
  curl_des_ಗಣ_odd_parity((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ಕೀಲಿ, ಗಾತ್ರ(ಕೀಲಿ));

  /* Set the key */
  des_ಗಣ_ಕೀಲಿ(des, (ಸ್ಥಿರಾಂಕ uint8_t *)ಕೀಲಿ);
}

#ಅಥವಾಆದರೆ defined(USE_MBEDTLS_DES)
ಸ್ಥಿರ bool encrypt_des(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಒಳ, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಹೊರಹೋಗು,
                        ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಕೀಲಿ_56)
{
  mbedtls_des_context ctx;
  ಅಕ್ಷರ ಕೀಲಿ[8];

  /* Expand the 56-bit key to 64 bits */
  extend_ಕೀಲಿ_56_ಗಮ್ಯ_64(ಕೀಲಿ_56, ಕೀಲಿ);

  /* Set the key parity to odd */
  mbedtls_des_ಕೀಲಿ_ಗಣ_parity((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ಕೀಲಿ);

  /* Perform the encryption */
  mbedtls_des_init(&ctx);
  mbedtls_des_setkey_enc(&ctx, (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ಕೀಲಿ);
  ಹಿಂತಿರುಗಿ mbedtls_des_crypt_ecb(&ctx, ಒಳ, ಹೊರಹೋಗು) == 0;
}

#ಅಥವಾಆದರೆ defined(USE_OS400CRYPTO)
ಸ್ಥಿರ bool encrypt_des(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಒಳ, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಹೊರಹೋಗು,
                        ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಕೀಲಿ_56)
{
  ಅಕ್ಷರ ಕೀಲಿ[8];
  _CIPHER_Control_T ctl;

  /* Setup the cipher control structure */
  ctl.Func_ID = ENCRYPT_ONLY;
  ctl.ದತ್ತಾಂಶ_Len = ಗಾತ್ರ(ಕೀಲಿ);

  /* Expand the 56-bit key to 64 bits */
  extend_ಕೀಲಿ_56_ಗಮ್ಯ_64(ಕೀಲಿ_56, ctl.Crypto_ಕೀಲಿ);

  /* Set the key parity to odd */
  curl_des_ಗಣ_odd_parity((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ctl.Crypto_ಕೀಲಿ, ctl.ದತ್ತಾಂಶ_Len);

  /* Perform the encryption */
  _CIPHER((_SPCPTR *)&ಹೊರಹೋಗು, &ctl, (_SPCPTR *)&ಒಳ);

  ಹಿಂತಿರುಗಿ TRUE;
}

#ಅಥವಾಆದರೆ defined(USE_WIN32_CRYPTO)
ಸ್ಥಿರ bool encrypt_des(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಒಳ, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಹೊರಹೋಗು,
                        ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಕೀಲಿ_56)
{
  HCRYPTPROV hprov;
  HCRYPTKEY hkey;
  ರಚನೆ {
    BLOBHEADER hdr;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ len;
    ಅಕ್ಷರ ಕೀಲಿ[8];
  } blob;
  DWORD len = 8;
  BOOL res;

  /* Acquire the crypto provider */
  ಆದರೆ(!CryptAcquireContext(&hprov, ಖಾಲಿ, ಖಾಲಿ, PROV_RSA_ತುಂಬಿದ,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    ಹಿಂತಿರುಗಿ FALSE;

  /* Setup the key blob structure */
  memset(&blob, 0, ಗಾತ್ರ(blob));
  blob.hdr.bType = PLAINTEXTKEYBLOB;
  blob.hdr.b_ಆವೃತ್ತಿ = 2;
  blob.hdr.ai_ಕೀಲಿ_Alg = CALG_DES;
  blob.len = ಗಾತ್ರ(blob.ಕೀಲಿ);

  /* Expand the 56-bit key to 64 bits */
  extend_ಕೀಲಿ_56_ಗಮ್ಯ_64(ಕೀಲಿ_56, blob.ಕೀಲಿ);

  /* Set the key parity to odd */
  curl_des_ಗಣ_odd_parity((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)blob.ಕೀಲಿ, ಗಾತ್ರ(blob.ಕೀಲಿ));

  /* Import the key */
  ಆದರೆ(!Crypt_Import_ಕೀಲಿ(hprov, (BYTE *)&blob, ಗಾತ್ರ(blob), 0, 0, &hkey)) {
    CryptReleaseContext(hprov, 0);

    ಹಿಂತಿರುಗಿ FALSE;
  }

  ಸ್ಮರಣೆ_ನಕಲು(ಹೊರಹೋಗು, ಒಳ, 8);

  /* Perform the encryption */
  res = CryptEncrypt(hkey, 0, FALSE, 0, ಹೊರಹೋಗು, &len, len);

  Crypt_Destroy_ಕೀಲಿ(hkey);
  CryptReleaseContext(hprov, 0);

  ಹಿಂತಿರುಗಿ res;
}

#ಆದರೆ_ಅಂತ್ಯ /* crypto backends */

/*
 * takes a 21-byte array and treats it as 3 56-bit DES keys. The
 * 8-byte plaintext is encrypted with each key and the resulting 24
 * bytes are stored in the results array.
 */
ಶೂನ್ಯ Curl_ntlm_core_lm_resp(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಕೀಲಿಗಳು,
                            ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *plaintext,
                            ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *results)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL_DES
  DES_ಕೀಲಿ_ವೇಳಾಪಟ್ಟಿ ks;

  ಸಿದ್ಧತೆ_des_ಕೀಲಿ(ಕೀಲಿಗಳು, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)results, &ks, DES_ENCRYPT);

  ಸಿದ್ಧತೆ_des_ಕೀಲಿ(ಕೀಲಿಗಳು + 7, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 8), &ks, DES_ENCRYPT);

  ಸಿದ್ಧತೆ_des_ಕೀಲಿ(ಕೀಲಿಗಳು + 14, &ks);
  DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(plaintext),
                  (DES_cblock *)(results + 16), &ks, DES_ENCRYPT);
#ಅಥವಾಆದರೆ defined(USE_WOLFSSL_DES)
  Des des;
  ಸಿದ್ಧತೆ_des_ಕೀಲಿ(ಕೀಲಿಗಳು, &des);
  wc_Des_EcbEncrypt(&des, results, plaintext, DES_ಕೀಲಿ_ಗಾತ್ರ);
  ಸಿದ್ಧತೆ_des_ಕೀಲಿ(ಕೀಲಿಗಳು + 7, &des);
  wc_Des_EcbEncrypt(&des, results + 8, plaintext, DES_ಕೀಲಿ_ಗಾತ್ರ);
  ಸಿದ್ಧತೆ_des_ಕೀಲಿ(ಕೀಲಿಗಳು + 14, &des);
  wc_Des_EcbEncrypt(&des, results + 16, plaintext, DES_ಕೀಲಿ_ಗಾತ್ರ);
#ಅಥವಾಆದರೆ defined(USE_GNUTLS_DES)
  ರಚನೆ des_ctx des;
  ಸಿದ್ಧತೆ_des_ಕೀಲಿ(ಕೀಲಿಗಳು, &des);
  des_encrypt(&des, 8, results, plaintext);
  ಸಿದ್ಧತೆ_des_ಕೀಲಿ(ಕೀಲಿಗಳು + 7, &des);
  des_encrypt(&des, 8, results + 8, plaintext);
  ಸಿದ್ಧತೆ_des_ಕೀಲಿ(ಕೀಲಿಗಳು + 14, &des);
  des_encrypt(&des, 8, results + 16, plaintext);
#ಅಥವಾಆದರೆ defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
  encrypt_des(plaintext, results, ಕೀಲಿಗಳು);
  encrypt_des(plaintext, results + 8, ಕೀಲಿಗಳು + 7);
  encrypt_des(plaintext, results + 16, ಕೀಲಿಗಳು + 14);
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ಕೀಲಿಗಳು;
  (ಶೂನ್ಯ)plaintext;
  (ಶೂನ್ಯ)results;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Set up lanmanager hashed password
 */
CURLcode Curl_ntlm_core_mk_lm_hash(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *password,
                                   ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *lmbuffer /* 21 bytes */)
{
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ pw[14];
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ magic[] = {
    0x4B, 0x47, 0x53, 0x21, 0x40, 0x23, 0x24, 0x25 /* i.e. KGS!@#$% */
  };
  size_t len = CURLMIN(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(password), 14);

  Curl_strntoupper((ಅಕ್ಷರ *)pw, password, len);
  memset(&pw[len], 0, 14 - len);

  {
    /* Create LanManager hashed password. */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL_DES
    DES_ಕೀಲಿ_ವೇಳಾಪಟ್ಟಿ ks;

    ಸಿದ್ಧತೆ_des_ಕೀಲಿ(pw, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)lmbuffer, &ks, DES_ENCRYPT);

    ಸಿದ್ಧತೆ_des_ಕೀಲಿ(pw + 7, &ks);
    DES_ecb_encrypt((DES_cblock *)CURL_UNCONST(magic),
                    (DES_cblock *)(lmbuffer + 8), &ks, DES_ENCRYPT);
#ಅಥವಾಆದರೆ defined(USE_WOLFSSL_DES)
    Des des;
    ಸಿದ್ಧತೆ_des_ಕೀಲಿ(pw, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer, magic, DES_ಕೀಲಿ_ಗಾತ್ರ);
    ಸಿದ್ಧತೆ_des_ಕೀಲಿ(pw + 7, &des);
    wc_Des_EcbEncrypt(&des, lmbuffer + 8, magic, DES_ಕೀಲಿ_ಗಾತ್ರ);
#ಅಥವಾಆದರೆ defined(USE_GNUTLS_DES)
    ರಚನೆ des_ctx des;
    ಸಿದ್ಧತೆ_des_ಕೀಲಿ(pw, &des);
    des_encrypt(&des, 8, lmbuffer, magic);
    ಸಿದ್ಧತೆ_des_ಕೀಲಿ(pw + 7, &des);
    des_encrypt(&des, 8, lmbuffer + 8, magic);
#ಅಥವಾಆದರೆ defined(USE_MBEDTLS_DES) || defined(USE_OS400CRYPTO) || \
  defined(USE_WIN32_CRYPTO)
    encrypt_des(magic, lmbuffer, pw);
    encrypt_des(magic, lmbuffer + 8, pw + 7);
#ಆದರೆ_ಅಂತ್ಯ

    memset(lmbuffer + 16, 0, 21 - 16);
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ ascii_ಗಮ್ಯ_unicode_le(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *dest, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src,
                                size_t srclen)
{
  size_t i;
  ಗಾಗಿ(i = 0; i < srclen; i++) {
    dest[2 * i] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)src[i];
    dest[(2 * i) + 1] = '\0';
  }
}

/*
 * Set up nt hashed passwords
 * @unittest: 1600
 */
CURLcode Curl_ntlm_core_mk_nt_hash(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *password,
                                   ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ntbuffer /* 21 bytes */)
{
  size_t len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(password);
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *pw;
  CURLcode ಫಲಿತಾಂಶ;
  ಆದರೆ(len > SIZE_MAX / 2) /* avoid integer overflow */
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  pw = len ? curlx_malloc(len * 2) : (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)curlx_strdup("");
  ಆದರೆ(!pw)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ascii_ಗಮ್ಯ_unicode_le(pw, password, len);

  /* Create NT hashed password. */
  ಫಲಿತಾಂಶ = Curl_md4it(ntbuffer, pw, 2 * len);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    memset(ntbuffer + 16, 0, 21 - 16);

  curlx_free(pw);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_WINDOWS_SSPI

#ವ್ಯಾಖ್ಯಾನಿಸು NTLMv2_BLOB_SIGNATURE "\x01\x01\x00\x00"
#ವ್ಯಾಖ್ಯಾನಿಸು NTLMv2_BLOB_LEN       (44 - 16 + ntlm->ಗುರಿ_info_len + 4)

/* Timestamp in tenths of a microsecond since January 1, 1601 00:00:00 UTC. */
ರಚನೆ ms_filetime {
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ dw_ಕೆಳಗಿನ_Date_ಸಮಯ;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ dw_ಮೇಲಿನ_Date_ಸಮಯ;
};

/* Convert a time_t to an MS FILETIME (MS-DTYP section 2.3.3). */
ಸ್ಥಿರ ಶೂನ್ಯ time2filetime(ರಚನೆ ms_filetime *ft, time_t t)
{
#ಆದರೆ SIZEOF_ಸಮಯ_T > 4
  t = (t + (curl_off_t)11644473600) * 10000000;
  ft->dw_ಕೆಳಗಿನ_Date_ಸಮಯ = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(t & 0xFFFFFFFF);
  ft->dw_ಮೇಲಿನ_Date_ಸಮಯ = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(t >> 32);
#ಇಲ್ಲದಿದ್ದರೆ
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ r, s;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;

  ft->dw_ಕೆಳಗಿನ_Date_ಸಮಯ = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(t & 0xFFFFFFFF);
  ft->dw_ಮೇಲಿನ_Date_ಸಮಯ = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_ಸಮಯ_T_UNSIGNED
  /* Extend sign if needed. */
  ಆದರೆ(ft->dw_ಕೆಳಗಿನ_Date_ಸಮಯ & 0x80000000)
    ft->dw_ಮೇಲಿನ_Date_ಸಮಯ = ~(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)0;
#ಆದರೆ_ಅಂತ್ಯ

  /* Bias seconds to Jan 1, 1601.
     134774 days = 11644473600 seconds = 0x2B6109100 */
  r = ft->dw_ಕೆಳಗಿನ_Date_ಸಮಯ;
  ft->dw_ಕೆಳಗಿನ_Date_ಸಮಯ = (ft->dw_ಕೆಳಗಿನ_Date_ಸಮಯ + 0xB6109100U) & 0xFFFFFFFF;
  ft->dw_ಮೇಲಿನ_Date_ಸಮಯ += ft->dw_ಕೆಳಗಿನ_Date_ಸಮಯ < r ? 0x03 : 0x02;

  /* Convert to tenths of microseconds. */
  ft->dw_ಮೇಲಿನ_Date_ಸಮಯ *= 10000000;
  i = 32;
  ಮಾಡು {
    i -= 8;
    s = ((ft->dw_ಕೆಳಗಿನ_Date_ಸಮಯ >> i) & 0xFF) * (10000000 - 1);
    r = (s << i) & 0xFFFFFFFF;
    s >>= 1;   /* Split shift to avoid width overflow. */
    s >>= 31 - i;
    ft->dw_ಕೆಳಗಿನ_Date_ಸಮಯ = (ft->dw_ಕೆಳಗಿನ_Date_ಸಮಯ + r) & 0xFFFFFFFF;
    ಆದರೆ(ft->dw_ಕೆಳಗಿನ_Date_ಸಮಯ < r)
      s++;
    ft->dw_ಮೇಲಿನ_Date_ಸಮಯ += s;
  } ಆಗ(i);
  ft->dw_ಮೇಲಿನ_Date_ಸಮಯ &= 0xFFFFFFFF;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಶೂನ್ಯ ascii_uppercase_ಗಮ್ಯ_unicode_le(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *dest,
                                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, size_t srclen)
{
  size_t i;
  ಗಾಗಿ(i = 0; i < srclen; i++) {
    dest[2 * i] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(Curl_raw_toupper(src[i]));
    dest[(2 * i) + 1] = '\0';
  }
}

/* This creates the NTLMv2 hash by using NTLM hash as the key and Unicode
 * (uppercase UserName + Domain) as the data
 */
CURLcode Curl_ntlm_core_mk_ntlmv2_hash(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *user, size_t userlen,
                                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *domain, size_t domlen,
                                       ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ntlmhash,
                                       ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ntlmv2hash)
{
  /* Unicode representation */
  size_t ತಟಸ್ಥ_len;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ತಟಸ್ಥ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ((userlen > CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ) || (domlen > CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ))
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ತಟಸ್ಥ_len = (userlen + domlen) * 2;
  ತಟಸ್ಥ = curlx_malloc(ತಟಸ್ಥ_len + 1);

  ಆದರೆ(!ತಟಸ್ಥ)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ascii_uppercase_ಗಮ್ಯ_unicode_le(ತಟಸ್ಥ, user, userlen);
  ascii_ಗಮ್ಯ_unicode_le(ತಟಸ್ಥ + (userlen << 1), domain, domlen);

  ಫಲಿತಾಂಶ = Curl_hmacit(&Curl_HMAC_MD5, ntlmhash, 16, ತಟಸ್ಥ, ತಟಸ್ಥ_len,
                       ntlmv2hash);
  curlx_free(ತಟಸ್ಥ);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
CURLcode Curl_ntlm_core_mk_ntlmv2_resp(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ntlmv2hash,
                                       ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *challenge_client,
                                       ಸ್ಥಿರಾಂಕ ರಚನೆ ntlmdata *ntlm,
                                       ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ **ntresp,
                                       ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *ntresp_len)
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

  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ len = 0;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ptr = ಖಾಲಿ;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ hmac_output[HMAC_MD5_ಉದ್ದಳತೆ];
  ರಚನೆ ms_filetime tw;

  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  /* Calculate the timestamp */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಅಕ್ಷರ *force_timestamp = getenv("CURL_FORCETIME");
  ಆದರೆ(force_timestamp)
    time2filetime(&tw, (time_t)0);
  ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    time2filetime(&tw, ಸಮಯ(ಖಾಲಿ));

  /* Calculate the response len */
  len = HMAC_MD5_ಉದ್ದಳತೆ + NTLMv2_BLOB_LEN;

  /* Allocate the response */
  ptr = curlx_calloc(1, len);
  ಆದರೆ(!ptr)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  /* Create the BLOB structure */
  curl_msnprintf((ಅಕ್ಷರ *)ptr + HMAC_MD5_ಉದ್ದಳತೆ, NTLMv2_BLOB_LEN,
                 "%c%c%c%c"           /* NTLMv2_BLOB_SIGNATURE */
                 "%c%c%c%c"           /* Reserved = 0 */
                 "%c%c%c%c%c%c%c%c",  /* Timestamp */
                 NTLMv2_BLOB_SIGNATURE[0], NTLMv2_BLOB_SIGNATURE[1],
                 NTLMv2_BLOB_SIGNATURE[2], NTLMv2_BLOB_SIGNATURE[3],
                 0, 0, 0, 0,
                 LONGQUARTET(tw.dw_ಕೆಳಗಿನ_Date_ಸಮಯ),
                 LONGQUARTET(tw.dw_ಮೇಲಿನ_Date_ಸಮಯ));

  ಸ್ಮರಣೆ_ನಕಲು(ptr + 32, challenge_client, 8);
  ಆದರೆ(ntlm->ಗುರಿ_info_len)
    ಸ್ಮರಣೆ_ನಕಲು(ptr + 44, ntlm->ಗುರಿ_info, ntlm->ಗುರಿ_info_len);

  /* Concatenate the Type 2 challenge with the BLOB and do HMAC MD5 */
  ಸ್ಮರಣೆ_ನಕಲು(ptr + 8, &ntlm->nonce[0], 8);
  ಫಲಿತಾಂಶ = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, HMAC_MD5_ಉದ್ದಳತೆ, ptr + 8,
                       NTLMv2_BLOB_LEN + 8, hmac_output);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    curlx_free(ptr);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /* Concatenate the HMAC MD5 output  with the BLOB */
  ಸ್ಮರಣೆ_ನಕಲು(ptr, hmac_output, HMAC_MD5_ಉದ್ದಳತೆ);

  /* Return the response */
  *ntresp = ptr;
  *ntresp_len = len;

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
CURLcode Curl_ntlm_core_mk_lmv2_resp(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ntlmv2hash,
                                     ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *challenge_client,
                                     ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *challenge_server,
                                     ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *lmresp)
{
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ದತ್ತಾಂಶ[16];
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ hmac_output[16];
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಸ್ಮರಣೆ_ನಕಲು(&ದತ್ತಾಂಶ[0], challenge_server, 8);
  ಸ್ಮರಣೆ_ನಕಲು(&ದತ್ತಾಂಶ[8], challenge_client, 8);

  ಫಲಿತಾಂಶ = Curl_hmacit(&Curl_HMAC_MD5, ntlmv2hash, 16, &ದತ್ತಾಂಶ[0], 16,
                       hmac_output);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* Concatenate the HMAC MD5 output with the client nonce */
  ಸ್ಮರಣೆ_ನಕಲು(lmresp, hmac_output, 16);
  ಸ್ಮರಣೆ_ನಕಲು(lmresp + 16, challenge_client, 8);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* !USE_WINDOWS_SSPI */

#ಆದರೆ_ಅಂತ್ಯ /* USE_CURL_NTLM_CORE */
