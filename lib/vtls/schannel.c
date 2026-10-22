/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 * Copyright (C) Marc Hoersken, <info@marc-hoersken.de>
 * Copyright (C) Mark Salisbury, <mark.salisbury@hp.com>
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
/*
 * Source file for all Schannel-specific code for the TLS/SSL layer. No code
 * but vtls.c should ever call or use these functions.
 */
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SCHANNEL

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_WINDOWS_SSPI
#ದೋಷ "cannot compile Schannel support without SSPI."
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "vtls/schannel.h"
#ಅನ್ತರ್ಭಾವ "vtls/schannel_int.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls_int.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls_scache.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "connect.h" /* for the connect timeout */
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "strerror.h"
#ಅನ್ತರ್ಭಾವ "select.h" /* for the socket readiness */
#ಅನ್ತರ್ಭಾವ "curlx/win32-fopen.h"
#ಅನ್ತರ್ಭಾವ "curlx/multibyte.h"
#ಅನ್ತರ್ಭಾವ "vtls/x509asn1.h"
#ಅನ್ತರ್ಭಾವ "curlx/version_win32.h"
#ಅನ್ತರ್ಭಾವ "curlx/winapi.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "curl_sha256.h"

/* Some verbose debug messages are wrapped by SCH_DEV() instead of DEBUGF()
 * and only shown if CURL_SCHANNEL_DEV_DEBUG was defined at build time. These
 * messages are extra verbose and intended for curl developers debugging
 * Schannel recv decryption and renegotiation.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_SCHANNEL_DEV_DEBUG
#ವ್ಯಾಖ್ಯಾನಿಸು SCH_DEV(x) x
#ವ್ಯಾಖ್ಯಾನಿಸು SCH_DEV_SHOWBOOL(x)                                     \
  ಮಾಡು {                                                          \
    infof(ದತ್ತಾಂಶ, "schannel: " #x " %s", (x) ? "TRUE" : "FALSE"); \
  } ಆಗ(0)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು SCH_DEV(x) ಮಾಡು {} ಆಗ(0)
#ವ್ಯಾಖ್ಯಾನಿಸು SCH_DEV_SHOWBOOL(x) ಮಾಡು {} ಆಗ(0)
#ಆದರೆ_ಅಂತ್ಯ

/* Offered by mingw-w64 v8+, MS SDK 7.0A/VS2010+ */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SP_PROT_TLS1_0_CLIENT
#ವ್ಯಾಖ್ಯಾನಿಸು SP_PROT_TLS1_0_CLIENT           SP_PROT_TLS1_CLIENT
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SP_PROT_TLS1_1_CLIENT
#ವ್ಯಾಖ್ಯಾನಿಸು SP_PROT_TLS1_1_CLIENT           0x00000200
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SP_PROT_TLS1_2_CLIENT
#ವ್ಯಾಖ್ಯಾನಿಸು SP_PROT_TLS1_2_CLIENT           0x00000800
#ಆದರೆ_ಅಂತ್ಯ

/* Offered by mingw-w64 v8+, MS SDK 10.0.15063.0/VS2017 15.1+ */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SP_PROT_TLS1_3_CLIENT
#ವ್ಯಾಖ್ಯಾನಿಸು SP_PROT_TLS1_3_CLIENT           0x00002000
#ಆದರೆ_ಅಂತ್ಯ
/* Offered by mingw-w64 v8+, MS SDK 8.1/VS2013+ */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SCH_USE_STRONG_CRYPTO
#ವ್ಯಾಖ್ಯಾನಿಸು SCH_USE_STRONG_CRYPTO           0x00400000
#ಆದರೆ_ಅಂತ್ಯ

/* Offered by mingw-w64 v10+, MS SDK 7.0A/VS2010+ */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SECBUFFER_ALERT
#ವ್ಯಾಖ್ಯಾನಿಸು SECBUFFER_ALERT                 17
#ಆದರೆ_ಅಂತ್ಯ

/* Both schannel buffer sizes must be > 0 */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SCHANNEL_ಬಫರ್_INIT_ಗಾತ್ರ   4096
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SCHANNEL_ಬಫರ್_FREE_ಗಾತ್ರ   1024

#ವ್ಯಾಖ್ಯಾನಿಸು CERT_THUMBPRINT_STR_LEN 40
#ವ್ಯಾಖ್ಯಾನಿಸು CERT_THUMBPRINT_ದತ್ತಾಂಶ_LEN 20

/* Uncomment to force verbose output
 * #define infof(x, y, ...) curl_mprintf(y, __VA_ARGS__)
 * #define failf(x, y, ...) curl_mprintf(y, __VA_ARGS__)
 */

/* Offered by mingw-w64 v4+, MS SDK 6.0A/VS2008+ */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PKCS12_NO_PERSIST_ಕೀಲಿ
#ವ್ಯಾಖ್ಯಾನಿಸು PKCS12_NO_PERSIST_ಕೀಲಿ 0x00008000
#ಆದರೆ_ಅಂತ್ಯ

/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CERT_ಕಂಡುಹಿಡಿ_HAS_ಖಾಸಗಿ_ಕೀಲಿ
#ವ್ಯಾಖ್ಯಾನಿಸು CERT_ಕಂಡುಹಿಡಿ_HAS_ಖಾಸಗಿ_ಕೀಲಿ (21 << CERT_COMPARE_ಸರಿಸು)
#ಆದರೆ_ಅಂತ್ಯ

/* key to use at `multi->proto_hash` */
#ವ್ಯಾಖ್ಯಾನಿಸು MPROTO_SCHANNEL_CERT_SHARE_ಕೀಲಿ   "tls:schannel:cert:share"

ಸ್ಥಿರ bool s_win_has_alpn;

/* Offered by mingw-w64 v9+, MS SDK 8.1/VS2013+ */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SECBUFFER_APPLICATION_PROTOCOLS
#ವ್ಯಾಖ್ಯಾನಿಸು SECBUFFER_APPLICATION_PROTOCOLS 18
#ವ್ಯಾಖ್ಯಾನಿಸು SECPKG_ATTR_APPLICATION_PROTOCOL 35

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  SecApplicationProtocolNegotiationExt_None,
  SecApplicationProtocolNegotiationExt_NPN,
  SecApplicationProtocolNegotiationExt_ALPN
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  Sec_Application_Protocol_Negotiation_ಸ್ಥಿತಿ_None,
  Sec_Application_Protocol_Negotiation_ಸ್ಥಿತಿ_Success,
  Sec_Application_Protocol_Negotiation_ಸ್ಥಿತಿ_Selected_Client_Only
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_ಸ್ಥಿತಿ;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
  SEC_APPLICATION_PROTOCOL_NEGOTIATION_ಸ್ಥಿತಿ Proto_Nego_ಸ್ಥಿತಿ;
  SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT ProtoNegoExt;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ Protocol_Id_ಗಾತ್ರ;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ProtocolId[0xff];
} SecPkgContext_ApplicationProtocol;
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ Init_Sec_ಬಫರ್(Sec_ಬಫರ್ *ಬಫರ್, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ BufType,
                          ಶೂನ್ಯ *Buf_ದತ್ತಾಂಶ_Ptr, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ Buf_Byte_ಗಾತ್ರ)
{
  ಬಫರ್->cb_ಬಫರ್ = Buf_Byte_ಗಾತ್ರ;
  ಬಫರ್->ಬಫರ್_Type = BufType;
  ಬಫರ್->pv_ಬಫರ್ = Buf_ದತ್ತಾಂಶ_Ptr;
}

ಸ್ಥಿರ ಶೂನ್ಯ Init_Sec_ಬಫರ್_Desc(Sec_ಬಫರ್_Desc *desc, Sec_ಬಫರ್ *BufArr,
                              ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಅಂಕೆ_Arr_Elem)
{
  desc->ul_ಆವೃತ್ತಿ = SECBUFFER_ಆವೃತ್ತಿ;
  desc->pBuffers = BufArr;
  desc->cBuffers = ಅಂಕೆ_Arr_Elem;
}

ಸ್ಥಿರ CURLcode schannel_ಗಣ_ssl_ಆವೃತ್ತಿ_min_max(DWORD *enabled_protocols,
                                                 ರಚನೆ Curl_cfilter *cf,
                                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ದೀರ್ಘ ssl_ಆವೃತ್ತಿ = conn_config->ಆವೃತ್ತಿ_2;
  ದೀರ್ಘ ssl_ಆವೃತ್ತಿ_max = (ದೀರ್ಘ)conn_config->ಆವೃತ್ತಿ_max;
  ದೀರ್ಘ i = ssl_ಆವೃತ್ತಿ;

  ಆಯ್ಕೆ(ssl_ಆವೃತ್ತಿ_max) {
  ಸ್ಥಿತಿ CURL_SSLVERSION_MAX_NONE:
  ಸ್ಥಿತಿ CURL_SSLVERSION_MAX_DEFAULT:

    /* Windows Server 2022 and newer (including Windows 11) support TLS 1.3
       built-in. Previous builds of Windows 10 had broken TLS 1.3
       implementations that could be enabled via registry. */
    ಆದರೆ(curlx_verify_windows_ಆವೃತ್ತಿ(10, 0, 20348, PLATFORM_WINNT,
                                    ಆವೃತ್ತಿ_GREATER_THAN_EQUAL)) {
      ssl_ಆವೃತ್ತಿ_max = CURL_SSLVERSION_MAX_TLSv1_3;
    }
    ಇಲ್ಲದಿದ್ದರೆ /* Windows 10 or older */
      ssl_ಆವೃತ್ತಿ_max = CURL_SSLVERSION_MAX_TLSv1_2;

    ನಿಲ್ಲಿಸು;
  }

  ಗಾಗಿ(; i <= (ssl_ಆವೃತ್ತಿ_max >> 16); ++i) {
    ಆಯ್ಕೆ(i) {
    ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_0:
      *enabled_protocols |= SP_PROT_TLS1_0_CLIENT;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_1:
      *enabled_protocols |= SP_PROT_TLS1_1_CLIENT;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_2:
      *enabled_protocols |= SP_PROT_TLS1_2_CLIENT;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_3:

      /* Windows Server 2022 or newer */
      ಆದರೆ(curlx_verify_windows_ಆವೃತ್ತಿ(10, 0, 20348, PLATFORM_WINNT,
                                      ಆವೃತ್ತಿ_GREATER_THAN_EQUAL)) {
        *enabled_protocols |= SP_PROT_TLS1_3_CLIENT;
        ನಿಲ್ಲಿಸು;
      }
      ಇಲ್ಲದಿದ್ದರೆ { /* Windows 10 or older */
        failf(ದತ್ತಾಂಶ, "schannel: TLS 1.3 not supported on Windows 10 or older");
        ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
      }
    }
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ವ್ಯಾಖ್ಯಾನಿಸು CIPHEROPTION(x) { #x, x }

ರಚನೆ algo {
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
  ಪೂರ್ಣಾಂಕ id;
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ algo algs[] = {
  CIPHEROPTION(CALG_MD2),
  CIPHEROPTION(CALG_MD4),
  CIPHEROPTION(CALG_MD5),
  CIPHEROPTION(CALG_SHA),
  CIPHEROPTION(CALG_SHA1),
  CIPHEROPTION(CALG_MAC),
  CIPHEROPTION(CALG_RSA_SIGN),
  CIPHEROPTION(CALG_DSS_SIGN),
  CIPHEROPTION(CALG_NO_SIGN),
  CIPHEROPTION(CALG_RSA_KEYX),
  CIPHEROPTION(CALG_DES),
  CIPHEROPTION(CALG_3DES_112),
  CIPHEROPTION(CALG_3DES),
  CIPHEROPTION(CALG_DESX),
  CIPHEROPTION(CALG_RC2),
  CIPHEROPTION(CALG_RC4),
  CIPHEROPTION(CALG_SEAL),
  CIPHEROPTION(CALG_DH_SF),
  CIPHEROPTION(CALG_DH_EPHEM),
  CIPHEROPTION(CALG_AGREEDKEY_ANY),
  CIPHEROPTION(CALG_HUGHES_MD5),
  CIPHEROPTION(CALG_SKIPJACK),
  CIPHEROPTION(CALG_TEK),
  CIPHEROPTION(CALG_CYLINK_MEK), /* spellchecker:disable-line */
  CIPHEROPTION(CALG_SSL3_SHAMD5),
  CIPHEROPTION(CALG_SSL3_MASTER),
  CIPHEROPTION(CALG_SCHANNEL_MASTER_HASH),
  CIPHEROPTION(CALG_SCHANNEL_MAC_ಕೀಲಿ),
  CIPHEROPTION(CALG_SCHANNEL_ENC_ಕೀಲಿ),
  CIPHEROPTION(CALG_PCT1_MASTER),
  CIPHEROPTION(CALG_SSL2_MASTER),
  CIPHEROPTION(CALG_TLS1_MASTER),
  CIPHEROPTION(CALG_RC5),
  CIPHEROPTION(CALG_HMAC),
  CIPHEROPTION(CALG_TLS1PRF),
  CIPHEROPTION(CALG_HASH_REPLACE_OWF),
  CIPHEROPTION(CALG_AES_128),
  CIPHEROPTION(CALG_AES_192),
  CIPHEROPTION(CALG_AES_256),
  CIPHEROPTION(CALG_AES),
  CIPHEROPTION(CALG_SHA_256),
  CIPHEROPTION(CALG_SHA_384),
  CIPHEROPTION(CALG_SHA_512),
  CIPHEROPTION(CALG_ECDH),
/* Offered by mingw-w64 v4+, MS SDK 6.0A/VS2008+ */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CALG_ECMQV
  CIPHEROPTION(CALG_ECMQV),
#ಆದರೆ_ಅಂತ್ಯ
  CIPHEROPTION(CALG_ECDSA),
/* Offered by mingw-w64 v7+, MS SDK 7.0A/VS2010+ */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CALG_ECDH_EPHEM
  CIPHEROPTION(CALG_ECDH_EPHEM),
#ಆದರೆ_ಅಂತ್ಯ
  { ಖಾಲಿ, 0 },
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ get_alg_id_by_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name_ಕೊನೆ = strchr(name, ':');
  size_t n = name_ಕೊನೆ ? (size_t)(name_ಕೊನೆ - name) : ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name);
  ಪೂರ್ಣಾಂಕ i;

  ಗಾಗಿ(i = 0; algs[i].name; i++) {
    ಆದರೆ((n == ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(algs[i].name) && !ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(algs[i].name, name, n)))
      ಹಿಂತಿರುಗಿ algs[i].id;
  }
  ಹಿಂತಿರುಗಿ 0; /* not found */
}

#ವ್ಯಾಖ್ಯಾನಿಸು ಅಂಕೆ_CIPHERS 47 /* There are a maximum of 47 options listed above */

ಸ್ಥಿರ CURLcode ಗಣ_ssl_ciphers(SCHANNEL_CRED *schannel_cred, ಅಕ್ಷರ *ciphers,
                                ALG_ID *algIds)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಆರಂಭ_Cur = ciphers;
  ಪೂರ್ಣಾಂಕ alg_ಎಣಿಕೆ = 0;
  ಆಗ(ಆರಂಭ_Cur && *ಆರಂಭ_Cur && (alg_ಎಣಿಕೆ < ಅಂಕೆ_CIPHERS)) {
    curl_off_t alg;
    ಆದರೆ(curlx_str_ಸಂಖ್ಯೆ(&ಆರಂಭ_Cur, &alg, ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) || !alg)
      alg = get_alg_id_by_name(ಆರಂಭ_Cur);

    ಆದರೆ(alg)
      algIds[alg_ಎಣಿಕೆ++] = (ALG_ID)alg;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(ಆರಂಭ_Cur, "USE_STRONG_CRYPTO",
                     CURL_CSTRLEN("USE_STRONG_CRYPTO")) ||
            !ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(ಆರಂಭ_Cur, "SCH_USE_STRONG_CRYPTO",
                     CURL_CSTRLEN("SCH_USE_STRONG_CRYPTO")))
      schannel_cred->dwFlags |= SCH_USE_STRONG_CRYPTO;
    ಇಲ್ಲದಿದ್ದರೆ
      ಹಿಂತಿರುಗಿ CURLE_SSL_CIPHER;
    ಆರಂಭ_Cur = strchr(ಆರಂಭ_Cur, ':');
    ಆದರೆ(ಆರಂಭ_Cur)
      ಆರಂಭ_Cur++;
  }
  schannel_cred->palgSupportedAlgs = algIds;
  schannel_cred->cSupportedAlgs = (DWORD)alg_ಎಣಿಕೆ;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* Function allocates memory for store_path only if CURLE_OK is returned */
ಸ್ಥಿರ CURLcode get_cert_location(TCHAR *ಮಾರ್ಗ, DWORD *store_name,
                                  TCHAR **store_ಮಾರ್ಗ, TCHAR **thumbprint)
{
  TCHAR *sep;
  TCHAR *store_ಮಾರ್ಗ_ಆರಂಭ;
  size_t store_name_len;

  sep = _tcschr(ಮಾರ್ಗ, ಪಠ್ಯ('\\'));
  ಆದರೆ(!sep)
    ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;

  store_name_len = sep - ಮಾರ್ಗ;

  ಆದರೆ(_tcsncmp(ಮಾರ್ಗ, ಪಠ್ಯ("CurrentUser"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_USER;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(_tcsncmp(ಮಾರ್ಗ, ಪಠ್ಯ("LocalMachine"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(_tcsncmp(ಮಾರ್ಗ, ಪಠ್ಯ("CurrentService"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_SERVICE;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(_tcsncmp(ಮಾರ್ಗ, ಪಠ್ಯ("Services"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_SERVICES;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(_tcsncmp(ಮಾರ್ಗ, ಪಠ್ಯ("Users"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_USERS;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(_tcsncmp(ಮಾರ್ಗ, ಪಠ್ಯ("CurrentUserGroupPolicy"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_USER_ಗುಂಪು_POLICY;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(_tcsncmp(ಮಾರ್ಗ, ಪಠ್ಯ("LocalMachineGroupPolicy"), store_name_len) ==
          0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE_ಗುಂಪು_POLICY;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(_tcsncmp(ಮಾರ್ಗ, ಪಠ್ಯ("LocalMachineEnterprise"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE_ENTERPRISE;
  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;

  store_ಮಾರ್ಗ_ಆರಂಭ = sep + 1;

  sep = _tcschr(store_ಮಾರ್ಗ_ಆರಂಭ, ಪಠ್ಯ('\\'));
  ಆದರೆ(!sep)
    ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;

  *thumbprint = sep + 1;
  ಆದರೆ(_tcslen(*thumbprint) != CERT_THUMBPRINT_STR_LEN)
    ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;

  *sep = ಪಠ್ಯ('\0');
  *store_ಮಾರ್ಗ = curlx_tcsdup(store_ಮಾರ್ಗ_ಆರಂಭ);
  *sep = ಪಠ್ಯ('\\');
  ಆದರೆ(!*store_ಮಾರ್ಗ)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode get_client_cert(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                HCERTSTORE *ಹೊರಹೋಗು_cert_store,
                                PCCERT_CONTEXT *ಹೊರಹೋಗು_cert_context)
{
  ರಚನೆ ssl_filter_config *sslc = Curl_ssl_cf_get_filter_config(cf);
  PCCERT_CONTEXT client_cert = ಖಾಲಿ;
  HCERTSTORE client_cert_store = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(sslc->clientcert || sslc->cert_blob) {
    DWORD cert_store_name = 0;
    TCHAR *cert_store_ಮಾರ್ಗ = ಖಾಲಿ;
    TCHAR *cert_thumbprint_str = ಖಾಲಿ;
    TCHAR cert_thumbprint_buf[CERT_THUMBPRINT_STR_LEN + 1];
    CRYPT_HASH_BLOB cert_thumbprint;
    BYTE cert_thumbprint_ದತ್ತಾಂಶ[CERT_THUMBPRINT_ದತ್ತಾಂಶ_LEN];
    HCERTSTORE cert_store = ಖಾಲಿ;
    FILE *f_ಒಳ_Cert = ಖಾಲಿ;
    ಶೂನ್ಯ *certdata = ಖಾಲಿ;
    size_t certsize = 0;
    bool blob = !!sslc->cert_blob;

    ಆದರೆ(blob) {
      certdata = sslc->cert_blob->ದತ್ತಾಂಶ;
      certsize = sslc->cert_blob->len;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      TCHAR *cert_ಮಾರ್ಗ =
        curlx_convert_UTF8_ಗಮ್ಯ_tchar(sslc->clientcert);
      ಆದರೆ(!cert_ಮಾರ್ಗ)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

      ಫಲಿತಾಂಶ = get_cert_location(cert_ಮಾರ್ಗ, &cert_store_name,
                                 &cert_store_ಮಾರ್ಗ, &cert_thumbprint_str);

      /* 'cert_thumbprint_str' points in to the allocated 'cert_path', copy
         the data. The string is verified to be CERT_THUMBPRINT_STR_LEN bytes
         long within the get_cert_location() function. */
      ಆದರೆ(!ಫಲಿತಾಂಶ && cert_thumbprint_str) {
        ಸ್ಮರಣೆ_ನಕಲು(cert_thumbprint_buf, cert_thumbprint_str,
               ಗಾತ್ರ(cert_thumbprint_buf));
        cert_thumbprint_str = cert_thumbprint_buf;
      }

      curlx_free(cert_ಮಾರ್ಗ);
      ಆದರೆ(ಫಲಿತಾಂಶ && (sslc->clientcert[0] != '\0'))
        f_ಒಳ_Cert = curlx_fopen(sslc->clientcert, "rb");

      ಆದರೆ(ಫಲಿತಾಂಶ && !f_ಒಳ_Cert) {
        failf(ದತ್ತಾಂಶ, "schannel: Failed to get certificate location"
              " or file for %s",
              sslc->clientcert);
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
    }

    ಆದರೆ((f_ಒಳ_Cert || blob) && sslc->cert_type &&
       !curl_strequal(sslc->cert_type, "P12")) {
      failf(ದತ್ತಾಂಶ, "schannel: certificate format compatibility error "
            "for %s",
            blob ? "(memory blob)" : sslc->clientcert);
      curlx_free(cert_store_ಮಾರ್ಗ);
      ಆದರೆ(f_ಒಳ_Cert)
        curlx_fclose(f_ಒಳ_Cert);
      ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;
    }

    ಆದರೆ(f_ಒಳ_Cert || blob) {
      /* Reading a .p12 or .pfx file, like the example at bottom of
         https://learn.microsoft.com/archive/msdn-technet-forums/3e7bc95f-b21a-4bcd-bd2c-7f996718cae5
       */
      CRYPT_ದತ್ತಾಂಶ_BLOB datablob;
      WCHAR *pszPassword;
      size_t pwd_len = 0;
      ಪೂರ್ಣಾಂಕ cert_ಕಂಡುಹಿಡಿ_flags;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cert_showfilename_error = blob ?
        "(memory blob)" : sslc->clientcert;
      curlx_free(cert_store_ಮಾರ್ಗ);
      ಆದರೆ(f_ಒಳ_Cert) {
        ದೀರ್ಘ cert_tell = 0;
        bool continue_reading = fseek(f_ಒಳ_Cert, 0, SEEK_END) == 0;
        ಆದರೆ(continue_reading) {
          cert_tell = ftell(f_ಒಳ_Cert);
          ಆದರೆ(cert_tell < 0)
            continue_reading = FALSE;
          ಇಲ್ಲದಿದ್ದರೆ
            certsize = (size_t)cert_tell;
        }
        ಆದರೆ(continue_reading)
          continue_reading = fseek(f_ಒಳ_Cert, 0, SEEK_SET) == 0;
        ಆದರೆ(continue_reading && (certsize < CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ))
          certdata = curlx_malloc(certsize + 1);
        ಆದರೆ(!certdata || ((ಪೂರ್ಣಾಂಕ)fread(certdata, certsize, 1, f_ಒಳ_Cert) != 1))
          continue_reading = FALSE;
        curlx_fclose(f_ಒಳ_Cert);
        ಆದರೆ(!continue_reading) {
          failf(ದತ್ತಾಂಶ, "schannel: Failed to read cert file %s",
                sslc->clientcert);
          curlx_free(certdata);
          ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;
        }
      }

      /* Convert key-pair data to the in-memory certificate store */
      datablob.pb_ದತ್ತಾಂಶ = (BYTE *)certdata;
      datablob.cb_ದತ್ತಾಂಶ = (DWORD)certsize;

      ಆದರೆ(sslc->ಕೀಲಿ_passwd)
        pwd_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sslc->ಕೀಲಿ_passwd);
      pszPassword = curlx_malloc(ಗಾತ್ರ(WCHAR) * (pwd_len + 1));
      ಆದರೆ(pszPassword) {
        ಪೂರ್ಣಾಂಕ str_w_len = 0;
        ಆದರೆ(pwd_len > 0)
          str_w_len = Multi_Byte_ಗಮ್ಯ_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                          sslc->ಕೀಲಿ_passwd, (ಪೂರ್ಣಾಂಕ)pwd_len,
                                          pszPassword, (ಪೂರ್ಣಾಂಕ)(pwd_len + 1));

        ಆದರೆ((str_w_len >= 0) && (str_w_len <= (ಪೂರ್ಣಾಂಕ)pwd_len))
          pszPassword[str_w_len] = 0;
        ಇಲ್ಲದಿದ್ದರೆ
          pszPassword[0] = 0;

        cert_store = PFXImportCertStore(&datablob, pszPassword,
                                        PKCS12_NO_PERSIST_ಕೀಲಿ);
        curlx_memzero(pszPassword, ಗಾತ್ರ(WCHAR) * (pwd_len + 1));
        curlx_free(pszPassword);
      }
      ಆದರೆ(!blob)
        curlx_free(certdata);
      ಆದರೆ(!cert_store) {
        DWORD errorcode = Get_ಕೊನೆಯ_Error();
        ಆದರೆ(errorcode == ERROR_INVALID_PASSWORD)
          failf(ದತ್ತಾಂಶ, "schannel: Failed to import cert file %s, "
                "password is bad",
                cert_showfilename_error);
        ಇಲ್ಲದಿದ್ದರೆ
          failf(ದತ್ತಾಂಶ, "schannel: Failed to import cert file %s, "
                "last error is 0x%08lx",
                cert_showfilename_error, errorcode);
        ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;
      }

      /* CERT_FIND_HAS_PRIVATE_KEY is only available in Windows 8 / Server
         2012, (NT 6.2). For older versions we use CURL_FIND_ANY. */
      ಆದರೆ(curlx_verify_windows_ಆವೃತ್ತಿ(6, 2, 0, PLATFORM_WINNT,
                                      ಆವೃತ್ತಿ_GREATER_THAN_EQUAL))
        cert_ಕಂಡುಹಿಡಿ_flags = CERT_ಕಂಡುಹಿಡಿ_HAS_ಖಾಸಗಿ_ಕೀಲಿ;
      ಇಲ್ಲದಿದ್ದರೆ
        cert_ಕಂಡುಹಿಡಿ_flags = CERT_ಕಂಡುಹಿಡಿ_ANY;

      client_cert =
        Cert_ಕಂಡುಹಿಡಿ_Certificate_ಒಳ_Store(cert_store,
                                   X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                   cert_ಕಂಡುಹಿಡಿ_flags, ಖಾಲಿ, ಖಾಲಿ);

      ಆದರೆ(!client_cert) {
        failf(ದತ್ತಾಂಶ, "schannel: Failed to get certificate from file %s"
              ", last error is 0x%08lx",
              cert_showfilename_error, Get_ಕೊನೆಯ_Error());
        CertCloseStore(cert_store, 0);
        ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      cert_store =
        CertOpenStore(
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNICODE
                      CERT_STORE_PROV_SYSTEM_W,
#ಇಲ್ಲದಿದ್ದರೆ
                      CERT_STORE_PROV_SYSTEM_A,
#ಆದರೆ_ಅಂತ್ಯ
                      0,
                      (HCRYPTPROV)ಖಾಲಿ,
                      CERT_STORE_OPEN_EXISTING_FLAG | cert_store_name,
                      cert_store_ಮಾರ್ಗ);
      ಆದರೆ(!cert_store) {
        ಅಕ್ಷರ *ಮಾರ್ಗ_utf8 =
          curlx_convert_tchar_ಗಮ್ಯ_UTF8(cert_store_ಮಾರ್ಗ);
        failf(ದತ್ತಾಂಶ, "schannel: Failed to open cert store %lx %s, "
              "last error is 0x%08lx",
              cert_store_name, (ಮಾರ್ಗ_utf8 ? ಮಾರ್ಗ_utf8 : "(unknown)"),
              Get_ಕೊನೆಯ_Error());
        curlx_free(cert_store_ಮಾರ್ಗ);
        curlx_free(ಮಾರ್ಗ_utf8);
        ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;
      }
      curlx_free(cert_store_ಮಾರ್ಗ);

      cert_thumbprint.pb_ದತ್ತಾಂಶ = cert_thumbprint_ದತ್ತಾಂಶ;
      cert_thumbprint.cb_ದತ್ತಾಂಶ = CERT_THUMBPRINT_ದತ್ತಾಂಶ_LEN;

      ಆದರೆ(!Crypt_String_ಗಮ್ಯ_ದ್ವಿಮಾನ(cert_thumbprint_str,
                              CERT_THUMBPRINT_STR_LEN,
                              CRYPT_STRING_HEX,
                              cert_thumbprint_ದತ್ತಾಂಶ,
                              &cert_thumbprint.cb_ದತ್ತಾಂಶ,
                              ಖಾಲಿ, ಖಾಲಿ)) {
        CertCloseStore(cert_store, 0);
        ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;
      }

      client_cert =
        Cert_ಕಂಡುಹಿಡಿ_Certificate_ಒಳ_Store(cert_store,
                                   X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                   CERT_ಕಂಡುಹಿಡಿ_HASH, &cert_thumbprint, ಖಾಲಿ);
      ಆದರೆ(!client_cert) {
        /* CRYPT_E_NOT_FOUND / E_INVALIDARG */
        CertCloseStore(cert_store, 0);
        failf(ದತ್ತಾಂಶ, "schannel: client cert not found in cert store");
        ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;
      }
    }
    client_cert_store = cert_store;
  }

  *ಹೊರಹೋಗು_cert_store = client_cert_store;
  *ಹೊರಹೋಗು_cert_context = client_cert;

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode acquire_sspi_handle(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *backend,
                                    PCCERT_CONTEXT client_cert,
                                    DWORD flags,
                                    DWORD enabled_protocols)
{
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SECURITY_ಸ್ಥಿತಿ sspi_ಸ್ಥಿತಿ = SEC_E_OK;
  CURLcode ಫಲಿತಾಂಶ;

  /* We support TLS 1.3 starting in Windows Server 2022 or later
   * (OS build 20348) */
  ಆದರೆ(!conn_config->cipher_list &&
     curlx_verify_windows_ಆವೃತ್ತಿ(10, 0, 20348, PLATFORM_WINNT,
                                  ಆವೃತ್ತಿ_GREATER_THAN_EQUAL)) {

    SCH_CREDENTIALS credentials = { 0 };
    TLS_PARAMETERS tls_parameters = { 0 };
    CRYPTO_SETTINGS crypto_settings[1];
    PCCERT_CONTEXT client_certs[1];

    ಆದರೆ(client_cert)
      client_certs[0] = client_cert;
    ಇಲ್ಲದಿದ್ದರೆ
      client_certs[0] = ಖಾಲಿ;

    memset(crypto_settings, 0, ಗಾತ್ರ(crypto_settings));

    tls_parameters.pDisabledCrypto = crypto_settings;

    /* The number of blocked suites */
    tls_parameters.cDisabledCrypto = (DWORD)0;
    credentials.pTlsParameters = &tls_parameters;
    credentials.cTlsParameters = 1;

    credentials.dw_ಆವೃತ್ತಿ = SCH_CREDENTIALS_ಆವೃತ್ತಿ;
    credentials.dwFlags = flags | SCH_USE_STRONG_CRYPTO;

    credentials.pTlsParameters->grbitDisabledProtocols = ~enabled_protocols;

    ಆದರೆ(client_certs[0]) {
      credentials.cCreds = 1;
      credentials.paCred = client_certs;
    }

    sspi_ಸ್ಥಿತಿ =
      Curl_pSecFn->AcquireCredentialsHandle(ಖಾಲಿ, CURL_UNCONST(UNISP_NAME),
                                            SECPKG_CRED_OUTBOUND, ಖಾಲಿ,
                                            &credentials, ಖಾಲಿ, ಖಾಲಿ,
                                            &backend->cred->cred_handle, ಖಾಲಿ);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* Pre-Windows 10 1809 or the user set a legacy algorithm list.
       Schannel does not negotiate TLS 1.3 when SCHANNEL_CRED is used. */
    ALG_ID algIds[ಅಂಕೆ_CIPHERS];
    ಅಕ್ಷರ *ciphers = conn_config->cipher_list;
    SCHANNEL_CRED schannel_cred = { 0 };
    PCCERT_CONTEXT client_certs[1];

    ಆದರೆ(client_cert)
      client_certs[0] = client_cert;
    ಇಲ್ಲದಿದ್ದರೆ
      client_certs[0] = ಖಾಲಿ;

    schannel_cred.dw_ಆವೃತ್ತಿ = SCHANNEL_CRED_ಆವೃತ್ತಿ;
    schannel_cred.dwFlags = flags;
    schannel_cred.grbitEnabledProtocols = enabled_protocols;

    ಆದರೆ(ciphers) {
      ಆದರೆ((enabled_protocols & SP_PROT_TLS1_3_CLIENT)) {
        ಆದರೆ(!(enabled_protocols & ~SP_PROT_TLS1_3_CLIENT)) {
          failf(ದತ್ತಾಂಶ, "schannel: TLS 1.3 is not supported with a cipher list; "
                "remove the cipher list or allow a lower TLS version");
          ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
        }
        infof(ದತ್ತಾಂಶ, "schannel: WARNING: This version of Schannel "
              "negotiates a less-secure TLS version than TLS 1.3 because the "
              "user set an algorithm cipher list.");
      }
      ಫಲಿತಾಂಶ = ಗಣ_ssl_ciphers(&schannel_cred, ciphers, algIds);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        failf(ದತ್ತಾಂಶ, "schannel: Failed setting algorithm cipher list");
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      schannel_cred.dwFlags = flags | SCH_USE_STRONG_CRYPTO;
    }

    ಆದರೆ(client_certs[0]) {
      schannel_cred.cCreds = 1;
      schannel_cred.paCred = client_certs;
    }

    sspi_ಸ್ಥಿತಿ =
      Curl_pSecFn->AcquireCredentialsHandle(ಖಾಲಿ, CURL_UNCONST(UNISP_NAME),
                                            SECPKG_CRED_OUTBOUND, ಖಾಲಿ,
                                            &schannel_cred, ಖಾಲಿ, ಖಾಲಿ,
                                            &backend->cred->cred_handle, ಖಾಲಿ);
  }

  ಆದರೆ(sspi_ಸ್ಥಿತಿ != SEC_E_OK) {
    ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN];
    failf(ದತ್ತಾಂಶ, "schannel: AcquireCredentialsHandle failed: %s",
          Curl_sspi_strerror(sspi_ಸ್ಥಿತಿ, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
    ಆಯ್ಕೆ(sspi_ಸ್ಥಿತಿ) {
    ಸ್ಥಿತಿ SEC_E_INSUFFICIENT_ಸ್ಮರಣೆ:
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಸ್ಥಿತಿ SEC_E_NO_CREDENTIALS:
    ಸ್ಥಿತಿ SEC_E_SECPKG_NOT_ಸಿಕ್ಕಿತು:
    ಸ್ಥಿತಿ SEC_E_NOT_OWNER:
    ಸ್ಥಿತಿ SEC_E_UNKNOWN_CREDENTIALS:
    ಸ್ಥಿತಿ SEC_E_ಆಂತರಿಕ_ERROR:
    ಪೂರ್ವನಿಯೋಜಿತ:
      ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
    }
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode schannel_acquire_credential_handle(ರಚನೆ Curl_cfilter *cf,
                                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  PCCERT_CONTEXT client_cert = ಖಾಲಿ;
  HCERTSTORE client_cert_store = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;

  /* setup Schannel API options */
  DWORD flags = 0;
  DWORD enabled_protocols = 0;

  ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;

  DEBUGASSERT(backend);

  ಆದರೆ(conn_config->verifypeer) {
    ಆದರೆ(backend->use_manual_cred_validation)
      flags = SCH_CRED_MANUAL_CRED_VALIDATION;
    ಇಲ್ಲದಿದ್ದರೆ
      flags = SCH_CRED_AUTO_CRED_VALIDATION;

    ಆದರೆ(conn_config->no_revoke) {
      flags |= SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
               SCH_CRED_IGNORE_REVOCATION_OFFLINE;

      DEBUGF(infof(ದತ್ತಾಂಶ, "schannel: disabled server certificate revocation "
                         "checks"));
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(conn_config->revoke_ಉತ್ತಮ_effort) {
      flags |= SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
               SCH_CRED_IGNORE_REVOCATION_OFFLINE |
               SCH_CRED_REVOCATION_CHECK_CHAIN;

      DEBUGF(infof(ದತ್ತಾಂಶ, "schannel: ignore revocation offline errors"));
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      flags |= SCH_CRED_REVOCATION_CHECK_CHAIN;

      DEBUGF(infof(ದತ್ತಾಂಶ, "schannel: checking server certificate revocation"));
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    flags = SCH_CRED_MANUAL_CRED_VALIDATION |
            SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
            SCH_CRED_IGNORE_REVOCATION_OFFLINE;
    DEBUGF(infof(ದತ್ತಾಂಶ, "schannel: disabled server cert revocation checks"));
  }

  ಆದರೆ(!conn_config->verifyhost) {
    flags |= SCH_CRED_NO_SERVERNAME_CHECK;
    DEBUGF(infof(ದತ್ತಾಂಶ, "schannel: verifyhost setting prevents Schannel from "
                       "comparing the supplied target name with the subject "
                       "names in server certificates."));
  }

  ಆದರೆ(!conn_config->auto_client_cert) {
    flags &= ~(DWORD)SCH_CRED_USE_DEFAULT_CREDS;
    flags |= SCH_CRED_NO_DEFAULT_CREDS;
    infof(ದತ್ತಾಂಶ, "schannel: disabled automatic use of client certificate");
  }
  ಇಲ್ಲದಿದ್ದರೆ
    infof(ದತ್ತಾಂಶ, "schannel: enabled automatic use of client certificate");

  DEBUGASSERT(conn_config->ಆವೃತ್ತಿ_2 != CURL_SSLVERSION_DEFAULT);
  ಆಯ್ಕೆ(conn_config->ಆವೃತ್ತಿ_2) {
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1:
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_0:
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_1:
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_2:
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_3: {
    ಫಲಿತಾಂಶ = schannel_ಗಣ_ssl_ಆವೃತ್ತಿ_min_max(&enabled_protocols, cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    ನಿಲ್ಲಿಸು;
  }
  ಸ್ಥಿತಿ CURL_SSLVERSION_SSLv3:
  ಸ್ಥಿತಿ CURL_SSLVERSION_SSLv2:
    failf(ದತ್ತಾಂಶ, "SSL versions not supported");
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
  ಪೂರ್ವನಿಯೋಜಿತ:
    failf(ದತ್ತಾಂಶ, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
  }

  ಫಲಿತಾಂಶ = get_client_cert(cf, ದತ್ತಾಂಶ, &client_cert_store, &client_cert);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* allocate memory for the reusable credential handle */
  backend->cred = (ರಚನೆ Curl_schannel_cred *)
    curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ Curl_schannel_cred));
  ಆದರೆ(!backend->cred) {
    failf(ದತ್ತಾಂಶ, "schannel: unable to allocate memory");

    ಆದರೆ(client_cert)
      CertFreeCertificateContext(client_cert);
    ಆದರೆ(client_cert_store)
      CertCloseStore(client_cert_store, 0);

    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
  backend->cred->refcount = 1;

  /* Since we did not persist the key, we need to extend the store's
   * lifetime until the end of the connection
   */
  backend->cred->client_cert_store = client_cert_store;

  ಫಲಿತಾಂಶ = acquire_sspi_handle(cf, ದತ್ತಾಂಶ, backend, client_cert,
                               flags, enabled_protocols);

  ಆದರೆ(client_cert)
    CertFreeCertificateContext(client_cert);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode schannel_connect_step1(ರಚನೆ Curl_cfilter *cf,
                                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  size_t written = 0;
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  Sec_ಬಫರ್ outbuf;
  Sec_ಬಫರ್_Desc outbuf_desc;
  Sec_ಬಫರ್ inbuf;
  Sec_ಬಫರ್_Desc inbuf_desc;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ alpn_ಬಫರ್[128];
  SECURITY_ಸ್ಥಿತಿ sspi_ಸ್ಥಿತಿ = SEC_E_OK;
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(backend);
  DEBUGF(infof(ದತ್ತಾಂಶ, "schannel: SSL/TLS connection with %s port %d (step 1/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->ಪೋರ್ಟ್));

  backend->use_alpn = connssl->alpn && s_win_has_alpn;

  ಆದರೆ(conn_config->CAfile || conn_config->ca_info_blob) {
    ಆದರೆ(curlx_verify_windows_ಆವೃತ್ತಿ(6, 1, 0, PLATFORM_WINNT,
                                    ಆವೃತ್ತಿ_GREATER_THAN_EQUAL)) {
      backend->use_manual_cred_validation = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      failf(ದತ್ತಾಂಶ, "schannel: this version of Windows is too old to support "
                  "certificate verification via CA bundle file.");
      ಹಿಂತಿರುಗಿ CURLE_SSL_CACERT_BADFILE;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ
    backend->use_manual_cred_validation = FALSE;

  backend->cred = ಖಾಲಿ;

  /* check for an existing reusable credential handle */
  ಆದರೆ(Curl_ssl_scache_use(cf, ದತ್ತಾಂಶ)) {
    ರಚನೆ Curl_schannel_cred *old_cred;
    Curl_ssl_scache_ಬೀಗ(ದತ್ತಾಂಶ);
    old_cred = Curl_ssl_scache_get_obj(cf, ದತ್ತಾಂಶ, connssl->peer.scache_ಕೀಲಿ);
    ಆದರೆ(old_cred) {
      backend->cred = old_cred;
      DEBUGF(infof(ದತ್ತಾಂಶ, "schannel: reusing existing credential handle"));

      /* increment the reference counter of the credential/session handle */
      backend->cred->refcount++;
      DEBUGF(infof(ದತ್ತಾಂಶ,
                   "schannel: incremented credential handle refcount = %d",
                   backend->cred->refcount));
    }
    Curl_ssl_scache_unlock(ದತ್ತಾಂಶ);
  }

  ಆದರೆ(!backend->cred) {
    ಅಕ್ಷರ *snihost;
    ಫಲಿತಾಂಶ = schannel_acquire_credential_handle(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ || !backend->cred)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    /* schannel_acquire_credential_handle() sets backend->cred accordingly or
       it returns error otherwise. */

    /* A hostname associated with the credential is needed by
       InitializeSecurityContext for SNI and other reasons. */
    snihost = connssl->peer.sni ?
      connssl->peer.sni : connssl->peer.origin->hostname;
    backend->cred->sni_hostname = curlx_convert_UTF8_ಗಮ್ಯ_tchar(snihost);
    ಆದರೆ(!backend->cred->sni_hostname)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  /* Warn if SNI is disabled due to use of an IP address */
  ಆದರೆ(connssl->peer.type != CURL_SSL_PEER_DNS) {
    infof(ದತ್ತಾಂಶ, "schannel: using IP address, SNI is not supported by OS.");
  }

  ಆದರೆ(backend->use_alpn) {
    ಪೂರ್ಣಾಂಕ cur = 0;
    ಪೂರ್ಣಾಂಕ list_ಆರಂಭ_ಸೂಚ್ಯಂಕ = 0;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *extension_len = ಖಾಲಿ;
    ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ *list_len = ಖಾಲಿ;
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ uiALPN = SecApplicationProtocolNegotiationExt_ALPN;
    ರಚನೆ alpn_proto_buf proto;

    /* The first four bytes is an unsigned int indicating number
       of bytes of data in the rest of the buffer. */
    extension_len = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *)(ಶೂನ್ಯ *)&alpn_ಬಫರ್[cur];
    cur += (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ);

    /* The next four bytes are an indicator that this buffer contains
       ALPN data, as opposed to NPN, for example. */
    ಸ್ಮರಣೆ_ನಕಲು(&alpn_ಬಫರ್[cur], &uiALPN, ಗಾತ್ರ(uiALPN));
    cur += (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ);

    /* The next two bytes is an unsigned short indicating the number
       of bytes used to list the preferred protocols. */
    list_len = (ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ *)(ಶೂನ್ಯ *)&alpn_ಬಫರ್[cur];
    cur += (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ);

    list_ಆರಂಭ_ಸೂಚ್ಯಂಕ = cur;

    ಫಲಿತಾಂಶ = Curl_alpn_ಗಮ್ಯ_proto_buf(&proto, connssl->alpn);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      failf(ದತ್ತಾಂಶ, "Error setting ALPN");
      ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
    }
    ಸ್ಮರಣೆ_ನಕಲು(&alpn_ಬಫರ್[cur], proto.ದತ್ತಾಂಶ, proto.len);
    cur += proto.len;

    *list_len = curlx_uitous(cur - list_ಆರಂಭ_ಸೂಚ್ಯಂಕ);
    *extension_len = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(*list_len +
      ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ) + ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ));

    Init_Sec_ಬಫರ್(&inbuf, SECBUFFER_APPLICATION_PROTOCOLS, alpn_ಬಫರ್, cur);
    Init_Sec_ಬಫರ್_Desc(&inbuf_desc, &inbuf, 1);

    Curl_alpn_ಗಮ್ಯ_proto_str(&proto, connssl->alpn);
    infof(ದತ್ತಾಂಶ, VTLS_INFOF_ALPN_OFFER_1STR, proto.ದತ್ತಾಂಶ);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    Init_Sec_ಬಫರ್(&inbuf, SECBUFFER_ಖಾಲಿಗಳು, ಖಾಲಿ, 0);
    Init_Sec_ಬಫರ್_Desc(&inbuf_desc, &inbuf, 1);
  }

  /* setup output buffer */
  Init_Sec_ಬಫರ್(&outbuf, SECBUFFER_ಖಾಲಿಗಳು, ಖಾಲಿ, 0);
  Init_Sec_ಬಫರ್_Desc(&outbuf_desc, &outbuf, 1);

  /* security request flags */
  backend->req_flags =
    ISC_REQ_SEQUENCE_DETECT | ISC_REQ_REPLAY_DETECT |
    ISC_REQ_CONFIDENTIALITY | ISC_REQ_ALLOCATE_ಸ್ಮರಣೆ |
    ISC_REQ_ಹರಿವು |
    (!conn_config->auto_client_cert ? ISC_REQ_USE_SUPPLIED_CREDS : 0);

  /* allocate memory for the security context handle */
  backend->ctxt = (ರಚನೆ Curl_schannel_ctxt *)
    curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ Curl_schannel_ctxt));
  ಆದರೆ(!backend->ctxt) {
    failf(ದತ್ತಾಂಶ, "schannel: unable to allocate memory");
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  /* Schannel InitializeSecurityContext:
     https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-initializesecuritycontextw

     At the moment we do not pass inbuf unless we are using ALPN since we only
     use it for that, and WINE (for which we currently disable ALPN) is giving
     us problems with inbuf regardless. https://github.com/curl/curl/issues/983
   */
  sspi_ಸ್ಥಿತಿ = Curl_pSecFn->InitializeSecurityContext(
    &backend->cred->cred_handle, ಖಾಲಿ, backend->cred->sni_hostname,
    backend->req_flags, 0, 0,
    (backend->use_alpn ? &inbuf_desc : ಖಾಲಿ),
    0, &backend->ctxt->ctxt_handle,
    &outbuf_desc, &backend->ret_flags, ಖಾಲಿ);

  ಆದರೆ(sspi_ಸ್ಥಿತಿ != SEC_I_CONTINUE_NEEDED) {
    ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN];
    curlx_safefree(backend->ctxt);
    ಆಯ್ಕೆ(sspi_ಸ್ಥಿತಿ) {
    ಸ್ಥಿತಿ SEC_E_INSUFFICIENT_ಸ್ಮರಣೆ:
      failf(ದತ್ತಾಂಶ, "schannel: initial InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(sspi_ಸ್ಥಿತಿ, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಸ್ಥಿತಿ SEC_E_WRONG_PRINCIPAL:
      failf(ದತ್ತಾಂಶ, "schannel: SNI or certificate check failed: %s",
            Curl_sspi_strerror(sspi_ಸ್ಥಿತಿ, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
      ಹಿಂತಿರುಗಿ CURLE_PEER_FAILED_VERIFICATION;
#ಆದರೆ 0
    ಸ್ಥಿತಿ SEC_E_INVALID_HANDLE:
    ಸ್ಥಿತಿ SEC_E_INVALID_TOKEN:
    ಸ್ಥಿತಿ SEC_E_LOGON_DENIED:
    ಸ್ಥಿತಿ SEC_E_ಗುರಿ_UNKNOWN:
    ಸ್ಥಿತಿ SEC_E_NO_AUTHENTICATING_AUTHORITY:
    ಸ್ಥಿತಿ SEC_E_ಆಂತರಿಕ_ERROR:
    ಸ್ಥಿತಿ SEC_E_NO_CREDENTIALS:
    ಸ್ಥಿತಿ SEC_E_UNSUPPORTED_FUNCTION:
    ಸ್ಥಿತಿ SEC_E_APPLICATION_PROTOCOL_MISMATCH:
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ವನಿಯೋಜಿತ:
      failf(ದತ್ತಾಂಶ, "schannel: initial InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(sspi_ಸ್ಥಿತಿ, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
      ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
    }
  }

  DEBUGF(infof(ದತ್ತಾಂಶ, "schannel: sending initial handshake data: "
               "sending %lu bytes.", outbuf.cb_ಬಫರ್));

  /* send initial handshake data which is now stored in output buffer */
  ಫಲಿತಾಂಶ = Curl_conn_cf_send(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ,
                             (ಸ್ಥಿರಾಂಕ uint8_t *)outbuf.pv_ಬಫರ್,
                             outbuf.cb_ಬಫರ್, FALSE,
                             &written);
  Curl_pSecFn->Free_Context_ಬಫರ್(outbuf.pv_ಬಫರ್);
  ಆದರೆ(ಫಲಿತಾಂಶ || (outbuf.cb_ಬಫರ್ != written)) {
    failf(ದತ್ತಾಂಶ, "schannel: failed to send initial handshake data: "
          "sent %zu of %lu bytes", written, outbuf.cb_ಬಫರ್);
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGF(infof(ದತ್ತಾಂಶ, "schannel: sent initial handshake data: "
               "sent %zu bytes", written));

  backend->recv_unrecoverable_err = CURLE_OK;
  backend->recv_sspi_close_notify = FALSE;
  backend->recv_connection_closed = FALSE;
  backend->recv_renegotiating = FALSE;
  backend->renegotiate_state.started = FALSE;
  backend->encdata_is_incomplete = FALSE;
  backend->shutdown_generated = FALSE;
  backend->shutdown_ಸ್ಥಾನಾಂತರ = 0;
  memset(&backend->shutdown_token, 0, ಗಾತ್ರ(backend->shutdown_token));

  /* continue to second handshake step */
  connssl->connecting_state = ssl_connect_2;

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode schannel_error(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               SECURITY_ಸ್ಥಿತಿ sspi_ಸ್ಥಿತಿ)
{
  ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN];
  ಆಯ್ಕೆ(sspi_ಸ್ಥಿತಿ) {
  ಸ್ಥಿತಿ SEC_E_INSUFFICIENT_ಸ್ಮರಣೆ:
    failf(ದತ್ತಾಂಶ, "schannel: next InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(sspi_ಸ್ಥಿತಿ, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  ಸ್ಥಿತಿ SEC_E_WRONG_PRINCIPAL:
    failf(ದತ್ತಾಂಶ, "schannel: SNI or certificate check failed: %s",
          Curl_sspi_strerror(sspi_ಸ್ಥಿತಿ, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
    ಹಿಂತಿರುಗಿ CURLE_PEER_FAILED_VERIFICATION;
  ಸ್ಥಿತಿ SEC_E_UNTRUSTED_ಬೇರು:
    failf(ದತ್ತಾಂಶ, "schannel: %s",
          Curl_sspi_strerror(sspi_ಸ್ಥಿತಿ, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
    ಹಿಂತಿರುಗಿ CURLE_PEER_FAILED_VERIFICATION;
#ಆದರೆ 0
  ಸ್ಥಿತಿ SEC_E_INVALID_HANDLE:
  ಸ್ಥಿತಿ SEC_E_INVALID_TOKEN:
  ಸ್ಥಿತಿ SEC_E_LOGON_DENIED:
  ಸ್ಥಿತಿ SEC_E_ಗುರಿ_UNKNOWN:
  ಸ್ಥಿತಿ SEC_E_NO_AUTHENTICATING_AUTHORITY:
  ಸ್ಥಿತಿ SEC_E_ಆಂತರಿಕ_ERROR:
  ಸ್ಥಿತಿ SEC_E_NO_CREDENTIALS:
  ಸ್ಥಿತಿ SEC_E_UNSUPPORTED_FUNCTION:
  ಸ್ಥಿತಿ SEC_E_APPLICATION_PROTOCOL_MISMATCH:
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ವನಿಯೋಜಿತ:
    failf(ದತ್ತಾಂಶ, "schannel: next InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(sspi_ಸ್ಥಿತಿ, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
  }
}

ಸ್ಥಿರ CURLcode schannel_pkp_pin_peer_pubkey(ರಚನೆ Curl_cfilter *cf,
                                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pinnedpubkey)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  CERT_CONTEXT *pCertContextServer = ಖಾಲಿ;

  /* Result is returned to caller */
  CURLcode ಫಲಿತಾಂಶ = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  DEBUGASSERT(backend);

  /* if a path was not specified, do not pin */
  ಆದರೆ(!pinnedpubkey)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಮಾಡು {
    SECURITY_ಸ್ಥಿತಿ sspi_ಸ್ಥಿತಿ;
    uint8_t *x509_der;
    DWORD x509_der_len;
    ರಚನೆ Curl_X509certificate x509_parsed;
    ರಚನೆ Curl_asn1Element *pubkey;

    sspi_ಸ್ಥಿತಿ =
      Curl_pSecFn->Query_Context_ಗುಣಲಕ್ಷಣಗಳು(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                          &pCertContextServer);

    ಆದರೆ((sspi_ಸ್ಥಿತಿ != SEC_E_OK) || !pCertContextServer) {
      ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN];
      failf(ದತ್ತಾಂಶ, "schannel: Failed to read remote certificate context: %s",
            Curl_sspi_strerror(sspi_ಸ್ಥಿತಿ, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
      ನಿಲ್ಲಿಸು; /* failed */
    }

    ಆದರೆ(!(((pCertContextServer->dwCertEncodingType & X509_ASN_ENCODING) != 0) &&
         (pCertContextServer->cbCertEncoded > 0)))
      ನಿಲ್ಲಿಸು;

    x509_der = (uint8_t *)pCertContextServer->pbCertEncoded;
    x509_der_len = pCertContextServer->cbCertEncoded;
    memset(&x509_parsed, 0, ಗಾತ್ರ(x509_parsed));
    ಆದರೆ(Curl_parseX509(&x509_parsed, x509_der, x509_der + x509_der_len))
      ನಿಲ್ಲಿಸು;

    pubkey = &x509_parsed.subject_ಬಹಿರಂಗ_ಕೀಲಿ_Info;
    ಆದರೆ(!pubkey->ಶೀರ್ಷಿಕೆ || pubkey->ಕೊನೆ <= pubkey->ಶೀರ್ಷಿಕೆ) {
      failf(ದತ್ತಾಂಶ, "SSL: failed retrieving public key from server certificate");
      ನಿಲ್ಲಿಸು;
    }

    ಫಲಿತಾಂಶ = Curl_pin_peer_pubkey(ದತ್ತಾಂಶ,
                                  pinnedpubkey,
                                  (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)pubkey->ಶೀರ್ಷಿಕೆ,
                                  (size_t)(pubkey->ಕೊನೆ - pubkey->ಶೀರ್ಷಿಕೆ));
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      failf(ದತ್ತಾಂಶ, "SSL: public key does not match pinned public key");
    }
  } ಆಗ(0);

  ಆದರೆ(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ensure_encoding_ಗಾತ್ರ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ sbuffer *encdata,
                                     size_t min_ಉದ್ದಳತೆ)
{
  size_t ಗಾತ್ರ_2;
  DEBUGASSERT(encdata->ಉದ್ದಳತೆ >= encdata->ಸ್ಥಾನಾಂತರ);
  ಆದರೆ(encdata->ಉದ್ದಳತೆ < encdata->ಸ್ಥಾನಾಂತರ)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಗಾತ್ರ_2 = encdata->ಉದ್ದಳತೆ - encdata->ಸ್ಥಾನಾಂತರ;
  ಆದರೆ(ಗಾತ್ರ_2 < CURL_SCHANNEL_ಬಫರ್_FREE_ಗಾತ್ರ || encdata->ಉದ್ದಳತೆ < min_ಉದ್ದಳತೆ) {
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಬಫರ್;
    size_t ಉದ್ದಳತೆ = encdata->ಸ್ಥಾನಾಂತರ + CURL_SCHANNEL_ಬಫರ್_FREE_ಗಾತ್ರ;
    ಆದರೆ(ಉದ್ದಳತೆ < min_ಉದ್ದಳತೆ)
      ಉದ್ದಳತೆ = min_ಉದ್ದಳತೆ;
    ಬಫರ್ = curlx_realloc(encdata->ಬಫರ್, ಉದ್ದಳತೆ);
    ಆದರೆ(!ಬಫರ್) {
      failf(ದತ್ತಾಂಶ, "schannel: unable to re-allocate memory");
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }

    encdata->ಬಫರ್ = ಬಫರ್;
    encdata->ಉದ್ದಳತೆ = ಉದ್ದಳತೆ;
    SCH_DEV(infof(ದತ್ತಾಂಶ, "schannel: encdata.buffer resized %zu",
                  encdata->ಉದ್ದಳತೆ));
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ensure_decoding_ಗಾತ್ರ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ sbuffer *decdata,
                                     size_t nowsize,
                                     size_t len)
{
  size_t ಗಾತ್ರ_2 = nowsize > CURL_SCHANNEL_ಬಫರ್_FREE_ಗಾತ್ರ ?
    nowsize : CURL_SCHANNEL_ಬಫರ್_FREE_ಗಾತ್ರ;
  DEBUGASSERT(decdata->ಉದ್ದಳತೆ >= decdata->ಸ್ಥಾನಾಂತರ);
  ಆದರೆ(decdata->ಉದ್ದಳತೆ < decdata->ಸ್ಥಾನಾಂತರ)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(decdata->ಉದ್ದಳತೆ - decdata->ಸ್ಥಾನಾಂತರ < ಗಾತ್ರ_2 ||
          decdata->ಉದ್ದಳತೆ < len) {
    /* increase internal decrypted data buffer */
    size_t ಉದ್ದಳತೆ = decdata->ಸ್ಥಾನಾಂತರ + ಗಾತ್ರ_2;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಬಫರ್;
    /* make sure that the requested amount of data fits */
    ಆದರೆ(ಉದ್ದಳತೆ < len)
      ಉದ್ದಳತೆ = len;

    ಬಫರ್ = curlx_realloc(decdata->ಬಫರ್, ಉದ್ದಳತೆ);
    ಆದರೆ(!ಬಫರ್) {
      failf(ದತ್ತಾಂಶ, "schannel: unable to re-allocate memory");
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
    decdata->ಬಫರ್ = ಬಫರ್;
    decdata->ಉದ್ದಳತೆ = ಉದ್ದಳತೆ;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode schannel_connect_step2(ರಚನೆ Curl_cfilter *cf,
                                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಪೂರ್ಣಾಂಕ i;
  size_t nread = 0;
  Sec_ಬಫರ್ outbuf[3];
  Sec_ಬಫರ್_Desc outbuf_desc;
  Sec_ಬಫರ್ inbuf[2];
  Sec_ಬಫರ್_Desc inbuf_desc;
  SECURITY_ಸ್ಥಿತಿ sspi_ಸ್ಥಿತಿ = SEC_E_OK;
  CURLcode ಫಲಿತಾಂಶ;
  bool doread;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pubkey_ptr;

  DEBUGASSERT(backend);

  doread = (connssl->io_need & CURL_SSL_IO_NEED_SEND) ? FALSE : TRUE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  DEBUGF(infof(ದತ್ತಾಂಶ, "schannel: SSL/TLS connection with %s port %d (step 2/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->ಪೋರ್ಟ್));

  ಆದರೆ(!backend->cred || !backend->ctxt)
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;

  /* buffer to store previously received and decrypted data */
  ಆದರೆ(!backend->decdata.ಬಫರ್) {
    backend->decdata.ಸ್ಥಾನಾಂತರ = 0;
    backend->decdata.ಉದ್ದಳತೆ = CURL_SCHANNEL_ಬಫರ್_INIT_ಗಾತ್ರ;
    backend->decdata.ಬಫರ್ = curlx_malloc(backend->decdata.ಉದ್ದಳತೆ);
    ಆದರೆ(!backend->decdata.ಬಫರ್) {
      failf(ದತ್ತಾಂಶ, "schannel: unable to allocate memory");
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
  }

  /* buffer to store previously received and encrypted data */
  ಆದರೆ(!backend->encdata.ಬಫರ್) {
    backend->encdata_is_incomplete = FALSE;
    backend->encdata.ಸ್ಥಾನಾಂತರ = 0;
    backend->encdata.ಉದ್ದಳತೆ = CURL_SCHANNEL_ಬಫರ್_INIT_ಗಾತ್ರ;
    backend->encdata.ಬಫರ್ = curlx_malloc(backend->encdata.ಉದ್ದಳತೆ);
    ಆದರೆ(!backend->encdata.ಬಫರ್) {
      failf(ದತ್ತಾಂಶ, "schannel: unable to allocate memory");
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
  }

  ಫಲಿತಾಂಶ = ensure_encoding_ಗಾತ್ರ(ದತ್ತಾಂಶ, &backend->encdata, 0);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಗಾಗಿ(;;) {
    ಆದರೆ(doread) {
      /* read encrypted handshake data from socket */
      ಫಲಿತಾಂಶ = Curl_conn_cf_recv(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ,
                                 (ಅಕ್ಷರ *)(backend->encdata.ಬಫರ್ +
                                          backend->encdata.ಸ್ಥಾನಾಂತರ),
                                 backend->encdata.ಉದ್ದಳತೆ -
                                 backend->encdata.ಸ್ಥಾನಾಂತರ,
                                 &nread);
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
        ಆದರೆ(!backend->encdata.ಸ್ಥಾನಾಂತರ || backend->encdata_is_incomplete) {
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          DEBUGF(infof(ದತ್ತಾಂಶ, "schannel: failed to receive handshake, "
                       "need more data"));
          ಹಿಂತಿರುಗಿ CURLE_OK;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          DEBUGF(infof(ದತ್ತಾಂಶ, "schannel: no new handshake data received, "
                       "continuing to process existing handshake data"));
        }
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ || (nread == 0)) {
        failf(ದತ್ತಾಂಶ, "schannel: failed to receive handshake, "
              "SSL/TLS connection failed");
        ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* increase encrypted data buffer offset */
        backend->encdata.ಸ್ಥಾನಾಂತರ += nread;
        backend->encdata_is_incomplete = FALSE;
        SCH_DEV(infof(ದತ್ತಾಂಶ, "schannel: encrypted data got %zu", nread));
      }
    }

    SCH_DEV(infof(ದತ್ತಾಂಶ,
                  "schannel: encrypted data buffer: offset %zu length %zu",
                  backend->encdata.ಸ್ಥಾನಾಂತರ, backend->encdata.ಉದ್ದಳತೆ));

    /* setup input buffers */
    Init_Sec_ಬಫರ್(&inbuf[0], SECBUFFER_TOKEN,
                  curlx_memdup0((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)backend->encdata.ಬಫರ್,
                                backend->encdata.ಸ್ಥಾನಾಂತರ),
                  curlx_uztoul(backend->encdata.ಸ್ಥಾನಾಂತರ));
    Init_Sec_ಬಫರ್(&inbuf[1], SECBUFFER_ಖಾಲಿಗಳು, ಖಾಲಿ, 0);
    Init_Sec_ಬಫರ್_Desc(&inbuf_desc, inbuf, 2);

    /* setup output buffers */
    Init_Sec_ಬಫರ್(&outbuf[0], SECBUFFER_TOKEN, ಖಾಲಿ, 0);
    Init_Sec_ಬಫರ್(&outbuf[1], SECBUFFER_ALERT, ಖಾಲಿ, 0);
    Init_Sec_ಬಫರ್(&outbuf[2], SECBUFFER_ಖಾಲಿಗಳು, ಖಾಲಿ, 0);
    Init_Sec_ಬಫರ್_Desc(&outbuf_desc, outbuf, 3);

    ಆದರೆ(!inbuf[0].pv_ಬಫರ್) {
      failf(ದತ್ತಾಂಶ, "schannel: unable to allocate memory");
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }

    /* The socket must be writable (or a poll error occurred) before we call
       InitializeSecurityContext to continue processing the received TLS
       records. This is because that function is not idempotent and we do not
       support partial save/resume sending replies of handshake tokens. */
    ಆದರೆ(!SOCKET_WRITABLE(Curl_conn_cf_get_socket(cf, ದತ್ತಾಂಶ), 0)) {
      SCH_DEV(infof(ದತ್ತಾಂಶ, "schannel: handshake waiting for writable socket"));
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      curlx_free(inbuf[0].pv_ಬಫರ್);
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }

    sspi_ಸ್ಥಿತಿ = Curl_pSecFn->InitializeSecurityContext(
      &backend->cred->cred_handle, &backend->ctxt->ctxt_handle,
      backend->cred->sni_hostname, backend->req_flags,
      0, 0, &inbuf_desc, 0, ಖಾಲಿ,
      &outbuf_desc, &backend->ret_flags, ಖಾಲಿ);

    /* free buffer for received handshake data */
    curlx_safefree(inbuf[0].pv_ಬಫರ್);

    /* check if the handshake was incomplete */
    ಆಯ್ಕೆ(sspi_ಸ್ಥಿತಿ) {
    ಸ್ಥಿತಿ SEC_E_INCOMPLETE_ಸಂದೇಶ:
      backend->encdata_is_incomplete = TRUE;
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      DEBUGF(infof(ದತ್ತಾಂಶ,
                   "schannel: received incomplete message, need more data"));
      ಹಿಂತಿರುಗಿ CURLE_OK;

    ಸ್ಥಿತಿ SEC_I_CONTINUE_NEEDED:
    ಸ್ಥಿತಿ SEC_E_OK:
      /* check if the handshake needs to be continued */
      ಫಲಿತಾಂಶ = CURLE_OK;
      ಗಾಗಿ(i = 0; i < 3; i++) {
        /* search for handshake tokens that need to be sent */
        ಆದರೆ(outbuf[i].ಬಫರ್_Type == SECBUFFER_TOKEN && outbuf[i].cb_ಬಫರ್ > 0) {
          size_t written = 0;
          DEBUGF(infof(ದತ್ತಾಂಶ, "schannel: sending next handshake data: "
                       "sending %lu bytes.", outbuf[i].cb_ಬಫರ್));

          /* send handshake token to server */
          ಫಲಿತಾಂಶ = Curl_conn_cf_send(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ,
                                     (ಸ್ಥಿರಾಂಕ uint8_t *)outbuf[i].pv_ಬಫರ್,
                                     outbuf[i].cb_ಬಫರ್,
                                     FALSE, &written);
          ಆದರೆ(ಫಲಿತಾಂಶ || (outbuf[i].cb_ಬಫರ್ != written)) {
            failf(ದತ್ತಾಂಶ, "schannel: failed to send next handshake data: "
                  "sent %zu of %lu bytes", written, outbuf[i].cb_ಬಫರ್);
            ಫಲಿತಾಂಶ = CURLE_SSL_CONNECT_ERROR;
          }
        }
      }
      ಗಾಗಿ(i = 0; i < 3; i++) {
        /* free obsolete buffer */
        ಆದರೆ(outbuf[i].pv_ಬಫರ್)
          Curl_pSecFn->Free_Context_ಬಫರ್(outbuf[i].pv_ಬಫರ್);
      }
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SEC_I_INCOMPLETE_CREDENTIALS:
      ಆದರೆ(!(backend->req_flags & ISC_REQ_USE_SUPPLIED_CREDS)) {
        /* If the server has requested a client certificate, attempt to
           continue the handshake without one. This allows connections to
           servers which request a client certificate but do not require
           it. */
        backend->req_flags |= ISC_REQ_USE_SUPPLIED_CREDS;
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        DEBUGF(infof(ದತ್ತಾಂಶ,
                     "schannel: a client certificate has been requested"));
        ಹಿಂತಿರುಗಿ CURLE_OK;
      }
      FALLTHROUGH();

    ಪೂರ್ವನಿಯೋಜಿತ:
      ಹಿಂತಿರುಗಿ schannel_error(ದತ್ತಾಂಶ, sspi_ಸ್ಥಿತಿ);
    }

    /* check if there was additional remaining encrypted data */
    ಆದರೆ(inbuf[1].ಬಫರ್_Type == SECBUFFER_EXTRA && inbuf[1].cb_ಬಫರ್ > 0) {
      SCH_DEV(infof(ದತ್ತಾಂಶ, "schannel: encrypted data length: %lu",
                    inbuf[1].cb_ಬಫರ್));
      /* There are two cases where we could be getting extra data here:
         1. If we are renegotiating a connection and the handshake is already
            complete (from the server perspective), it can encrypt app data
            (not handshake data) in an extra buffer at this point.
         2. (sspi_status == SEC_I_CONTINUE_NEEDED) We are negotiating a
            connection and this extra data is part of the handshake.
            We should process the data immediately; waiting for the socket to
            be ready may fail since the server is done sending handshake data.
       */
      /* check if the remaining data is less than the total amount
         and therefore begins after the already processed data */
      ಆದರೆ(backend->encdata.ಸ್ಥಾನಾಂತರ > inbuf[1].cb_ಬಫರ್) {
        memmove(backend->encdata.ಬಫರ್,
                (backend->encdata.ಬಫರ್ + backend->encdata.ಸ್ಥಾನಾಂತರ) -
                inbuf[1].cb_ಬಫರ್, inbuf[1].cb_ಬಫರ್);
        backend->encdata.ಸ್ಥಾನಾಂತರ = inbuf[1].cb_ಬಫರ್;
        ಆದರೆ(sspi_ಸ್ಥಿತಿ == SEC_I_CONTINUE_NEEDED) {
          doread = FALSE;
          ಮುಂದುವರಿ;
        }
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      backend->encdata.ಸ್ಥಾನಾಂತರ = 0;
    }
    ನಿಲ್ಲಿಸು;
  }

  /* check if the handshake needs to be continued */
  ಆದರೆ(sspi_ಸ್ಥಿತಿ == SEC_I_CONTINUE_NEEDED) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  /* check if the handshake is complete */
  ಆದರೆ(sspi_ಸ್ಥಿತಿ == SEC_E_OK) {
    connssl->connecting_state = ssl_connect_3;
    DEBUGF(infof(ದತ್ತಾಂಶ, "schannel: SSL/TLS handshake complete"));
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  pubkey_ptr = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSL_PINNEDPUBLICKEY);
#ಇಲ್ಲದಿದ್ದರೆ
  pubkey_ptr = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSL_PINNEDPUBLICKEY);
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(pubkey_ptr) {
    ಫಲಿತಾಂಶ = schannel_pkp_pin_peer_pubkey(cf, ದತ್ತಾಂಶ, pubkey_ptr);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      failf(ದತ್ತಾಂಶ, "SSL: public key does not match pinned public key");
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }

  ಆದರೆ(conn_config->verifypeer && backend->use_manual_cred_validation) {
    /* Certificate verification also verifies the hostname if verifyhost */
    ಹಿಂತಿರುಗಿ Curl_verify_certificate(cf, ದತ್ತಾಂಶ);
  }

  /* Verify the hostname manually when certificate verification is disabled,
     because in that case Schannel does not verify it. */
  ಆದರೆ(!conn_config->verifypeer && conn_config->verifyhost)
    ಹಿಂತಿರುಗಿ Curl_verify_host(cf, ದತ್ತಾಂಶ);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ bool valid_cert_encoding(ಸ್ಥಿರಾಂಕ CERT_CONTEXT *cert_context)
{
  ಹಿಂತಿರುಗಿ cert_context &&
    ((cert_context->dwCertEncodingType & X509_ASN_ENCODING) != 0) &&
    cert_context->pbCertEncoded &&
    (cert_context->cbCertEncoded > 0);
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ bool (*Read_crt_func)(ಸ್ಥಿರಾಂಕ CERT_CONTEXT *ccert_context,
                              bool ಹಿಮ್ಮುಖ_ಕ್ರಮ, ಶೂನ್ಯ *arg);

ಸ್ಥಿರ ಶೂನ್ಯ traverse_cert_store(ಸ್ಥಿರಾಂಕ CERT_CONTEXT *context,
                                Read_crt_func func, ಶೂನ್ಯ *arg)
{
  ಸ್ಥಿರಾಂಕ CERT_CONTEXT *current_context = ಖಾಲಿ;
  bool should_continue = TRUE;
  bool ಮೊದಲ = TRUE;
  bool ಹಿಮ್ಮುಖ_ಕ್ರಮ = FALSE;
  ಆಗ(should_continue &&
        (current_context = Cert_Enum_Certificates_ಒಳ_Store(
          context->hCertStore,
          current_context)) != ಖಾಲಿ) {
    /* Windows 11 22H2 OS Build 22621.674 or higher enumerates certificates in
       leaf-to-root order while all previous versions of Windows enumerate
       certificates in root-to-leaf order. Determine the order of enumeration
       by comparing SECPKG_ATTR_REMOTE_CERT_CONTEXT's pbCertContext with the
       first certificate's pbCertContext. */
    ಆದರೆ(ಮೊದಲ && context->pbCertEncoded != current_context->pbCertEncoded)
      ಹಿಮ್ಮುಖ_ಕ್ರಮ = TRUE;
    should_continue = func(current_context, ಹಿಮ್ಮುಖ_ಕ್ರಮ, arg);
    ಮೊದಲ = FALSE;
  }

  ಆದರೆ(current_context)
    CertFreeCertificateContext(current_context);
}

ಸ್ಥಿರ bool cert_counter_callback(ಸ್ಥಿರಾಂಕ CERT_CONTEXT *ccert_context,
                                  bool ಹಿಮ್ಮುಖ_ಕ್ರಮ, ಶೂನ್ಯ *certs_ಎಣಿಕೆ)
{
  (ಶೂನ್ಯ)ಹಿಮ್ಮುಖ_ಕ್ರಮ;
  ಆದರೆ(valid_cert_encoding(ccert_context))
    (*(ಪೂರ್ಣಾಂಕ *)certs_ಎಣಿಕೆ)++;
  ಆದರೆ(*(ಪೂರ್ಣಾಂಕ *)certs_ಎಣಿಕೆ > MAX_ALLOWED_CERT_AMOUNT)
    ಹಿಂತಿರುಗಿ FALSE;
  ಹಿಂತಿರುಗಿ TRUE;
}

ರಚನೆ Adder_ವಾದಗಳು {
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ;
  CURLcode ಫಲಿತಾಂಶ;
  ಪೂರ್ಣಾಂಕ idx;
  ಪೂರ್ಣಾಂಕ certs_ಎಣಿಕೆ;
};

ಸ್ಥಿರ bool ಕೂಡಿಸು_cert_ಗಮ್ಯ_certinfo(ಸ್ಥಿರಾಂಕ CERT_CONTEXT *ccert_context,
                                 bool ಹಿಮ್ಮುಖ_ಕ್ರಮ, ಶೂನ್ಯ *raw_arg)
{
  ರಚನೆ Adder_ವಾದಗಳು *ವಾದಗಳು = (ರಚನೆ Adder_ವಾದಗಳು *)raw_arg;
  ವಾದಗಳು->ಫಲಿತಾಂಶ = CURLE_OK;
  ಆದರೆ(valid_cert_encoding(ccert_context)) {
    ಪೂರ್ಣಾಂಕ ಸೇರಿಸು_ಸೂಚ್ಯಂಕ = ಹಿಮ್ಮುಖ_ಕ್ರಮ ? (ವಾದಗಳು->certs_ಎಣಿಕೆ - 1) - ವಾದಗಳು->idx :
                       ವಾದಗಳು->idx;
    ವಾದಗಳು->ಫಲಿತಾಂಶ = Curl_ಹೊರತೆಗೆ_certinfo(ವಾದಗಳು->ದತ್ತಾಂಶ, ಸೇರಿಸು_ಸೂಚ್ಯಂಕ,
                                         ccert_context->pbCertEncoded,
                                         ccert_context->pbCertEncoded +
                                         ccert_context->cbCertEncoded);
    ವಾದಗಳು->idx++;
  }
  ಹಿಂತಿರುಗಿ ವಾದಗಳು->ಫಲಿತಾಂಶ == CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ schannel_session_free(ಶೂನ್ಯ *sessionid)
{
  /* this is expected to be called under sessionid lock */
  ರಚನೆ Curl_schannel_cred *cred = sessionid;

  ಆದರೆ(cred) {
    cred->refcount--;
    ಆದರೆ(cred->refcount == 0) {
      Curl_pSecFn->FreeCredentialsHandle(&cred->cred_handle);
      curlx_free(cred->sni_hostname);
      ಆದರೆ(cred->client_cert_store) {
        CertCloseStore(cred->client_cert_store, 0);
        cred->client_cert_store = ಖಾಲಿ;
      }
      curlx_safefree(cred);
    }
  }
}

ಸ್ಥಿರ CURLcode schannel_connect_step3(ರಚನೆ Curl_cfilter *cf,
                                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  ರಚನೆ ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, ದತ್ತಾಂಶ);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  SECURITY_ಸ್ಥಿತಿ sspi_ಸ್ಥಿತಿ = SEC_E_OK;
  CERT_CONTEXT *ccert_context = ಖಾಲಿ;
  SecPkgContext_ApplicationProtocol alpn_ಫಲಿತಾಂಶ;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);
  DEBUGASSERT(backend);

  DEBUGF(infof(ದತ್ತಾಂಶ, "schannel: SSL/TLS connection with %s port %d (step 3/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->ಪೋರ್ಟ್));

  ಆದರೆ(!backend->cred)
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;

  /* check if the required context attributes are met */
  ಆದರೆ(backend->ret_flags != backend->req_flags) {
    ಆದರೆ(!(backend->ret_flags & ISC_RET_SEQUENCE_DETECT))
      failf(ದತ್ತಾಂಶ, "schannel: failed to setup sequence detection");
    ಆದರೆ(!(backend->ret_flags & ISC_RET_REPLAY_DETECT))
      failf(ದತ್ತಾಂಶ, "schannel: failed to setup replay detection");
    ಆದರೆ(!(backend->ret_flags & ISC_RET_CONFIDENTIALITY))
      failf(ದತ್ತಾಂಶ, "schannel: failed to setup confidentiality");
    ಆದರೆ(!(backend->ret_flags & ISC_RET_ALLOCATED_ಸ್ಮರಣೆ))
      failf(ದತ್ತಾಂಶ, "schannel: failed to setup memory allocation");
    ಆದರೆ(!(backend->ret_flags & ISC_RET_ಹರಿವು))
      failf(ದತ್ತಾಂಶ, "schannel: failed to setup stream orientation");
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
  }

  ಆದರೆ(backend->use_alpn) {
    sspi_ಸ್ಥಿತಿ =
      Curl_pSecFn->Query_Context_ಗುಣಲಕ್ಷಣಗಳು(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_APPLICATION_PROTOCOL,
                                          &alpn_ಫಲಿತಾಂಶ);

    ಆದರೆ(sspi_ಸ್ಥಿತಿ != SEC_E_OK) {
      failf(ದತ್ತಾಂಶ, "schannel: failed to retrieve ALPN result");
      ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
    }

    ಆದರೆ(alpn_ಫಲಿತಾಂಶ.Proto_Nego_ಸ್ಥಿತಿ ==
       Sec_Application_Protocol_Negotiation_ಸ್ಥಿತಿ_Success) {
      ಆದರೆ(backend->recv_renegotiating &&
         connssl->negotiated.alpn &&
         ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(connssl->negotiated.alpn,
                 (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)alpn_ಫಲಿತಾಂಶ.ProtocolId,
                 alpn_ಫಲಿತಾಂಶ.Protocol_Id_ಗಾತ್ರ)) {
        /* Renegotiation selected a different protocol now, we cannot
         * deal with this */
        failf(ದತ್ತಾಂಶ, "schannel: server selected an ALPN protocol too late");
        ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
      }
      Curl_alpn_ಗಣ_negotiated(cf, ದತ್ತಾಂಶ, connssl, alpn_ಫಲಿತಾಂಶ.ProtocolId,
                               alpn_ಫಲಿತಾಂಶ.Protocol_Id_ಗಾತ್ರ);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಆದರೆ(!backend->recv_renegotiating)
        Curl_alpn_ಗಣ_negotiated(cf, ದತ್ತಾಂಶ, connssl, ಖಾಲಿ, 0);
    }
  }

  /* save the current session data for possible reuse */
  ಆದರೆ(Curl_ssl_scache_use(cf, ದತ್ತಾಂಶ)) {
    Curl_ssl_scache_ಬೀಗ(ದತ್ತಾಂಶ);
    /* Up ref count since call takes ownership */
    backend->cred->refcount++;
    ಫಲಿತಾಂಶ = Curl_ssl_scache_ಕೂಡಿಸು_obj(cf, ದತ್ತಾಂಶ, connssl->peer.scache_ಕೀಲಿ,
                                     backend->cred, schannel_session_free);
    Curl_ssl_scache_unlock(ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಆದರೆ(ssl_config->certinfo) {
    ಪೂರ್ಣಾಂಕ certs_ಎಣಿಕೆ = 0;
    sspi_ಸ್ಥಿತಿ =
      Curl_pSecFn->Query_Context_ಗುಣಲಕ್ಷಣಗಳು(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                          &ccert_context);

    ಆದರೆ((sspi_ಸ್ಥಿತಿ != SEC_E_OK) || !ccert_context) {
      failf(ದತ್ತಾಂಶ, "schannel: failed to retrieve remote cert context");
      ಹಿಂತಿರುಗಿ CURLE_PEER_FAILED_VERIFICATION;
    }

    traverse_cert_store(ccert_context, cert_counter_callback, &certs_ಎಣಿಕೆ);
    ಆದರೆ(certs_ಎಣಿಕೆ > MAX_ALLOWED_CERT_AMOUNT) {
      failf(ದತ್ತಾಂಶ, "%d certificates is more than allowed (%d)",
            certs_ಎಣಿಕೆ, MAX_ALLOWED_CERT_AMOUNT);
      CertFreeCertificateContext(ccert_context);
      ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
    }

    ಫಲಿತಾಂಶ = Curl_ssl_init_certinfo(ದತ್ತಾಂಶ, certs_ಎಣಿಕೆ);
    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      ರಚನೆ Adder_ವಾದಗಳು ವಾದಗಳು;
      ವಾದಗಳು.ದತ್ತಾಂಶ = ದತ್ತಾಂಶ;
      ವಾದಗಳು.idx = 0;
      ವಾದಗಳು.certs_ಎಣಿಕೆ = certs_ಎಣಿಕೆ;
      ವಾದಗಳು.ಫಲಿತಾಂಶ = CURLE_OK;
      traverse_cert_store(ccert_context, ಕೂಡಿಸು_cert_ಗಮ್ಯ_certinfo, &ವಾದಗಳು);
      ಫಲಿತಾಂಶ = ವಾದಗಳು.ಫಲಿತಾಂಶ;
    }
    CertFreeCertificateContext(ccert_context);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  connssl->connecting_state = ssl_connect_ಮುಗಿದ;

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode schannel_connect(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 bool *ಮುಗಿದ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ;

  /* check if the connection has already been established */
  ಆದರೆ(ssl_connection_complete == connssl->state) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  *ಮುಗಿದ = FALSE;

  ಆದರೆ(connssl->connecting_state == ssl_connect_1) {
    ಫಲಿತಾಂಶ = schannel_connect_step1(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಆದರೆ(connssl->connecting_state == ssl_connect_2) {
    ಫಲಿತಾಂಶ = schannel_connect_step2(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಆದರೆ(connssl->connecting_state == ssl_connect_3) {
    ಫಲಿತಾಂಶ = schannel_connect_step3(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಆದರೆ(connssl->connecting_state == ssl_connect_ಮುಗಿದ) {
    ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *backend =
      (ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
    DEBUGASSERT(backend);

    ಆದರೆ(Curl_pSecFn->Query_Context_ಗುಣಲಕ್ಷಣಗಳು(
         &backend->ctxt->ctxt_handle,
         SECPKG_ATTR_ಹರಿವು_SIZES,
         &backend->ಹರಿವು_sizes)) {
      failf(ದತ್ತಾಂಶ, "schannel: failed getting stream sizes");
      ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
    }

    connssl->state = ssl_connection_complete;

    /* When SSPI is used in combination with Schannel
     * we need the Schannel context to create the Schannel
     * binding to pass the IIS extended protection checks.
     * Available on Windows 7 or later.
     */
    cf->conn->sslContext = &backend->ctxt->ctxt_handle;

    *ಮುಗಿದ = TRUE;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಎಣಿಕೆ schannel_renegotiate_caller_t {
  SCH_RENEG_CALLER_IS_RECV,
  SCH_RENEG_CALLER_IS_SEND
};

/* The maximum time we allow for Schannel renegotiation which may in some
   rare cases block either due to libcurl (waiting on the socket) or Windows
   (waiting on an interactive security prompt). Note Schannel "renegotiation"
   is not necessarily literal TLS renegotiation, but means DecryptMessage
   returned SEC_I_RENEGOTIATE which means at least the security context needs
   to be re-established. */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_RENEG_ಖಂಡ_ಸಮಯ (60 * 1000) /* 60 seconds in milliseconds */

/* This function renegotiates the connection due to a server request received
   by schannel_recv. This function returns CURLE_AGAIN if the renegotiation is
   incomplete. In that case, we remain in the renegotiation (connecting) stage
   and future calls to schannel_recv and schannel_send must call this function
   first to complete the renegotiation. */
ಸ್ಥಿರ CURLcode schannel_recv_renegotiate(
  ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
  ಎಣಿಕೆ schannel_renegotiate_caller_t caller)
{
  CURLcode ಫಲಿತಾಂಶ;
  curl_socket_t sockfd;
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  ರಚನೆ schannel_renegotiate_state *rs = &backend->renegotiate_state;

  ಆದರೆ(!backend || !backend->recv_renegotiating) {
    failf(ದತ್ತಾಂಶ, "schannel: unexpected call to schannel_recv_renegotiate");
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
  }
  DEBUGASSERT(caller <= SCH_RENEG_CALLER_IS_SEND);
  ಆದರೆ(caller == SCH_RENEG_CALLER_IS_RECV)
    SCH_DEV(infof(ದತ್ತಾಂಶ, "schannel: renegotiation caller is schannel_recv"));
  ಇಲ್ಲದಿದ್ದರೆ
    SCH_DEV(infof(ದತ್ತಾಂಶ, "schannel: renegotiation caller is schannel_send"));

  sockfd = Curl_conn_cf_get_socket(cf, ದತ್ತಾಂಶ);

  ಆದರೆ(sockfd == CURL_SOCKET_BAD) {
    failf(ದತ್ತಾಂಶ, "schannel: renegotiation missing socket");
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
  }

  ಆದರೆ(!rs->started) { /* new renegotiation */
    infof(ದತ್ತಾಂಶ, "schannel: renegotiating SSL/TLS connection");
    DEBUGASSERT(connssl->state == ssl_connection_complete);
    DEBUGASSERT(connssl->connecting_state == ssl_connect_ಮುಗಿದ);
    connssl->state = ssl_connection_negotiating;
    connssl->connecting_state = ssl_connect_2;
    memset(rs, 0, ಗಾತ್ರ(*rs));
    rs->io_need = CURL_SSL_IO_NEED_SEND;
    rs->ಆರಂಭ_ಸಮಯ = *Curl_pgrs_now(ದತ್ತಾಂಶ);
    rs->started = TRUE;
  }

  ಗಾಗಿ(;;) {
    bool ಖಂಡ_read, ಖಂಡ_write, blocking, ಮುಗಿದ;
    curl_socket_t readfd, writefd;
    timediff_t elapsed;

    elapsed = curlx_ptimediff_ms(Curl_pgrs_now(ದತ್ತಾಂಶ), &rs->ಆರಂಭ_ಸಮಯ);
    ಆದರೆ(elapsed >= MAX_RENEG_ಖಂಡ_ಸಮಯ) {
      failf(ದತ್ತಾಂಶ, "schannel: renegotiation timeout");
      ಫಲಿತಾಂಶ = CURLE_SSL_CONNECT_ERROR;
      ನಿಲ್ಲಿಸು;
    }

    /* the current io_need state may have been overwritten since the last time
       this function was called. restore the io_need state needed to continue
       the renegotiation. */

    connssl->io_need = rs->io_need;

    ಫಲಿತಾಂಶ = schannel_connect(cf, ದತ್ತಾಂಶ, &ಮುಗಿದ);

    rs->io_need = connssl->io_need;

    ಆದರೆ(!ಫಲಿತಾಂಶ && !ಮುಗಿದ)
      ಫಲಿತಾಂಶ = CURLE_AGAIN;

    ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_AGAIN)
      ನಿಲ್ಲಿಸು;

    readfd = (rs->io_need & CURL_SSL_IO_NEED_RECV) ? sockfd : CURL_SOCKET_BAD;
    writefd = (rs->io_need & CURL_SSL_IO_NEED_SEND) ? sockfd : CURL_SOCKET_BAD;

    ಆದರೆ(readfd == CURL_SOCKET_BAD && writefd == CURL_SOCKET_BAD)
      ಮುಂದುವರಿ;

    /* connect should not have requested io read and write together */
    DEBUGASSERT(readfd == CURL_SOCKET_BAD || writefd == CURL_SOCKET_BAD);

    /* This function is partially blocking to avoid a stoppage that would
     * occur if the user is waiting on the socket only in one direction.
     *
     * For example, if the user has called recv then they may not be waiting
     * for a writable socket and vice versa, so we block to avoid that.
     *
     * In practice a wait is unlikely to occur. For caller recv if handshake
     * data needs to be sent then we block for a writable socket that should
     * be writable immediately except for OS resource constraints. For caller
     * send if handshake data needs to be received then we block for a readable
     * socket, which could take some time, but it is more likely the user has
     * called recv since they had called it prior (only recv can start
     * renegotiation and probably the user is going to call it again to get
     * more of their data before calling send).
     */

    ಖಂಡ_read = (caller == SCH_RENEG_CALLER_IS_SEND) ? TRUE : FALSE;
    ಖಂಡ_write = (caller == SCH_RENEG_CALLER_IS_RECV) ? TRUE : FALSE;

    blocking = (ಖಂಡ_read && (readfd != CURL_SOCKET_BAD)) ||
               (ಖಂಡ_write && (writefd != CURL_SOCKET_BAD));

    SCH_DEV_SHOWBOOL(ಖಂಡ_read);
    SCH_DEV_SHOWBOOL(ಖಂಡ_write);
    SCH_DEV_SHOWBOOL(blocking);

    ಗಾಗಿ(;;) {
      ಪೂರ್ಣಾಂಕ what;
      timediff_t timeout_ms, ಉಳಿದ;

      ಫಲಿತಾಂಶ = Curl_pgrsUpdate(ದತ್ತಾಂಶ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ನಿಲ್ಲಿಸು;

      elapsed = curlx_ptimediff_ms(Curl_pgrs_now(ದತ್ತಾಂಶ), &rs->ಆರಂಭ_ಸಮಯ);
      ಆದರೆ(elapsed >= MAX_RENEG_ಖಂಡ_ಸಮಯ) {
        failf(ದತ್ತಾಂಶ, "schannel: renegotiation timeout");
        ಫಲಿತಾಂಶ = CURLE_SSL_CONNECT_ERROR;
        ನಿಲ್ಲಿಸು;
      }
      ಉಳಿದ = MAX_RENEG_ಖಂಡ_ಸಮಯ - elapsed;

      ಆದರೆ(blocking) {
        timeout_ms = Curl_timeleft_ms(ದತ್ತಾಂಶ);

        ಆದರೆ(timeout_ms < 0) {
          ಫಲಿತಾಂಶ = CURLE_OPERATION_TIMEDOUT;
          ನಿಲ್ಲಿಸು;
        }

        /* the blocking is in intervals so that the progress function can be
           called every second */
        ಆದರೆ(!timeout_ms || timeout_ms > 1000)
          timeout_ms = 1000;

        ಆದರೆ(timeout_ms > ಉಳಿದ)
          timeout_ms = ಉಳಿದ;
      }
      ಇಲ್ಲದಿದ್ದರೆ
        timeout_ms = 0;

      SCH_DEV(infof(ದತ್ತಾಂಶ, "schannel: renegotiation wait until socket is"
                    "%s%s for up to %" FMT_TIMEDIFF_T " ms",
                    ((readfd != CURL_SOCKET_BAD) ? " readable" : ""),
                    ((writefd != CURL_SOCKET_BAD) ? " writable" : ""),
                    timeout_ms));

      what = Curl_socket_check(readfd, CURL_SOCKET_BAD, writefd, timeout_ms);

      ಆದರೆ(what > 0 && (what & (CURL_CSELECT_ಒಳ | CURL_CSELECT_ಹೊರಹೋಗು))) {
        SCH_DEV(infof(ದತ್ತಾಂಶ, "schannel: renegotiation socket %s%s",
                      ((what & CURL_CSELECT_ಒಳ) ? "CURL_CSELECT_IN " : ""),
                      ((what & CURL_CSELECT_ಹೊರಹೋಗು) ? "CURL_CSELECT_OUT " : "")));
        ಫಲಿತಾಂಶ = CURLE_AGAIN;
        ನಿಲ್ಲಿಸು;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!what) {
        SCH_DEV(infof(ದತ್ತಾಂಶ, "schannel: renegotiation socket timeout"));
        ಆದರೆ(blocking)
          ಮುಂದುವರಿ;
        ಇಲ್ಲದಿದ್ದರೆ
          ಹಿಂತಿರುಗಿ CURLE_AGAIN;
      }

      failf(ದತ್ತಾಂಶ, "schannel: socket error during renegotiation");
      ಫಲಿತಾಂಶ = CURLE_SSL_CONNECT_ERROR;
      ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_AGAIN)
      ನಿಲ್ಲಿಸು;
  }

  DEBUGASSERT(ಫಲಿತಾಂಶ != CURLE_AGAIN);

  rs->started = FALSE;
  backend->recv_renegotiating = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  ಆದರೆ(ಫಲಿತಾಂಶ)
    failf(ದತ್ತಾಂಶ, "schannel: renegotiation failed");
  ಇಲ್ಲದಿದ್ದರೆ
    infof(ದತ್ತಾಂಶ, "schannel: SSL/TLS connection renegotiated");

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode schannel_send(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *buf, size_t len, size_t *pnwritten)
{
  size_t ದತ್ತಾಂಶ_len = 0;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ptr = ಖಾಲಿ;
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  Sec_ಬಫರ್ outbuf[4];
  Sec_ಬಫರ್_Desc outbuf_desc;
  SECURITY_ಸ್ಥಿತಿ sspi_ಸ್ಥಿತಿ = SEC_E_OK;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  ಆದರೆ(backend->recv_renegotiating) {
    ಫಲಿತಾಂಶ = schannel_recv_renegotiate(cf, ದತ್ತಾಂಶ, SCH_RENEG_CALLER_IS_SEND);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /* check if the buffer is longer than the maximum message length */
  ಆದರೆ(len > backend->ಹರಿವು_sizes.cb_ಗರಿಷ್ಠ_ಸಂದೇಶ) {
    len = backend->ಹರಿವು_sizes.cb_ಗರಿಷ್ಠ_ಸಂದೇಶ;
  }

  /* calculate the complete message length and prepare the send buffer */
  ದತ್ತಾಂಶ_len = backend->ಹರಿವು_sizes.cb_ಶೀರ್ಷಿಕೆ + len +
    backend->ಹರಿವು_sizes.cbTrailer;
  ಆದರೆ(ದತ್ತಾಂಶ_len > backend->send_ಬಫರ್_len) {
    ptr = curlx_realloc(backend->send_ಬಫರ್, ದತ್ತಾಂಶ_len);
    ಆದರೆ(!ptr)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    backend->send_ಬಫರ್ = ptr;
    backend->send_ಬಫರ್_len = ದತ್ತಾಂಶ_len;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    ptr = backend->send_ಬಫರ್;

  /* setup output buffers (header, data, trailer, empty) */
  Init_Sec_ಬಫರ್(&outbuf[0], SECBUFFER_ಹರಿವು_ಶೀರ್ಷಿಕೆ,
                ptr, backend->ಹರಿವು_sizes.cb_ಶೀರ್ಷಿಕೆ);
  Init_Sec_ಬಫರ್(&outbuf[1], SECBUFFER_ದತ್ತಾಂಶ,
                ptr + backend->ಹರಿವು_sizes.cb_ಶೀರ್ಷಿಕೆ, curlx_uztoul(len));
  Init_Sec_ಬಫರ್(&outbuf[2], SECBUFFER_ಹರಿವು_TRAILER,
                ptr + backend->ಹರಿವು_sizes.cb_ಶೀರ್ಷಿಕೆ + len,
                backend->ಹರಿವು_sizes.cbTrailer);
  Init_Sec_ಬಫರ್(&outbuf[3], SECBUFFER_ಖಾಲಿಗಳು, ಖಾಲಿ, 0);
  Init_Sec_ಬಫರ್_Desc(&outbuf_desc, outbuf, 4);

  /* copy data into output buffer */
  ಸ್ಮರಣೆ_ನಕಲು(outbuf[1].pv_ಬಫರ್, buf, len);

  /* https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-encryptmessage */
  sspi_ಸ್ಥಿತಿ = Curl_pSecFn->Encrypt_ಸಂದೇಶ(&backend->ctxt->ctxt_handle, 0,
                                            &outbuf_desc, 0);

  /* check if the message was encrypted */
  ಆದರೆ(sspi_ಸ್ಥಿತಿ == SEC_E_OK) {

    /* send the encrypted message including header, data and trailer */
    len = outbuf[0].cb_ಬಫರ್ + outbuf[1].cb_ಬಫರ್ + outbuf[2].cb_ಬಫರ್;

    /* it is important to send the full message which includes the header,
       encrypted payload, and trailer. Until the client receives all the
       data a coherent message has not been delivered and the client
       cannot read any of it.

       If we wanted to buffer the unwritten encrypted bytes, we would
       tell the client that all data it has requested to be sent has been
       sent. The unwritten encrypted bytes would be the first bytes to
       send on the next invocation.
       Here's the catch with this - if we tell the client that all the
       bytes have been sent, does the client call this method again to
       send the buffered data?  Looking at who calls this function, it
       seems the answer is NO. */

    /* send entire message or fail */
    ಆಗ(len > *pnwritten) {
      size_t this_write = 0;
      ಪೂರ್ಣಾಂಕ what;
      timediff_t timeout_ms = Curl_timeleft_ms(ದತ್ತಾಂಶ);
      ಆದರೆ(timeout_ms < 0) {
        /* we already got the timeout */
        failf(ದತ್ತಾಂಶ, "schannel: timed out sending data (bytes sent: %zu)",
              *pnwritten);
        ಫಲಿತಾಂಶ = CURLE_OPERATION_TIMEDOUT;
        ನಿಲ್ಲಿಸು;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!timeout_ms)
        timeout_ms = TIMEDIFF_T_MAX;
      what = SOCKET_WRITABLE(Curl_conn_cf_get_socket(cf, ದತ್ತಾಂಶ), timeout_ms);
      ಆದರೆ(what < 0) {
        /* fatal error */
        failf(ದತ್ತಾಂಶ, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
        ನಿಲ್ಲಿಸು;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(what == 0) {
        failf(ದತ್ತಾಂಶ, "schannel: timed out sending data (bytes sent: %zu)",
              *pnwritten);
        ಫಲಿತಾಂಶ = CURLE_OPERATION_TIMEDOUT;
        ನಿಲ್ಲಿಸು;
      }
      /* socket is writable */

       ಫಲಿತಾಂಶ = Curl_conn_cf_send(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ,
                                  (ಸ್ಥಿರಾಂಕ uint8_t *)ptr + *pnwritten,
                                  len - *pnwritten,
                                  FALSE, &this_write);
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
        ಮುಂದುವರಿ;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ) {
        ನಿಲ್ಲಿಸು;
      }

      *pnwritten += this_write;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(sspi_ಸ್ಥಿತಿ == SEC_E_INSUFFICIENT_ಸ್ಮರಣೆ) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
  }

  ಆದರೆ(len == *pnwritten)
    /* Encrypted message including header, data and trailer entirely sent.
       The return value is the number of unencrypted bytes that were sent. */
    *pnwritten = outbuf[1].cb_ಬಫರ್;

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode schannel_recv(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ಅಕ್ಷರ *buf, size_t len, size_t *pnread)
{
  size_t ಗಾತ್ರ_2 = 0;
  size_t nread = 0;
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  Sec_ಬಫರ್ inbuf[4];
  Sec_ಬಫರ್_Desc inbuf_desc;
  SECURITY_ಸ್ಥಿತಿ sspi_ಸ್ಥಿತಿ = SEC_E_OK;
  ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(backend);
  *pnread = 0;

  ಆದರೆ(backend->recv_renegotiating) {
    ಫಲಿತಾಂಶ = schannel_recv_renegotiate(cf, ದತ್ತಾಂಶ, SCH_RENEG_CALLER_IS_RECV);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /****************************************************************************
   * Do not return or set backend->recv_unrecoverable_err unless in the
   * cleanup. The pattern for return error is set *err, optional infof, goto
   * cleanup.
   *
   * Some verbose debug messages are wrapped by SCH_DEV() instead of DEBUGF()
   * and only shown if CURL_SCHANNEL_DEV_DEBUG was defined at build time. These
   * messages are extra verbose and intended for curl developers debugging
   * Schannel recv decryption.
   *
   * Our priority is to always return as much decrypted data to the caller as
   * possible, even if an error occurs. The state of the decrypted buffer must
   * always be valid. Transfer of decrypted data to the caller's buffer is
   * handled in the cleanup.
   */

  SCH_DEV(infof(ದತ್ತಾಂಶ, "schannel: client wants to read %zu bytes", len));

  ಆದರೆ(len && len <= backend->decdata.ಸ್ಥಾನಾಂತರ) {
    SCH_DEV(infof(ದತ್ತಾಂಶ,
                  "schannel: enough decrypted data is already available"));
    ಹೋಗು cleanup;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(backend->recv_unrecoverable_err) {
    ಫಲಿತಾಂಶ = backend->recv_unrecoverable_err;
    infof(ದತ್ತಾಂಶ, "schannel: an unrecoverable error occurred in a prior call");
    ಹೋಗು cleanup;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(backend->recv_sspi_close_notify) {
    /* once a server has indicated shutdown there is no more encrypted data */
    infof(ದತ್ತಾಂಶ, "schannel: server indicated shutdown in a prior call");
    ಹೋಗು cleanup;
  }
  /* it is debatable what to return when !len. Regardless we cannot return
     immediately because there may be data to decrypt (in the case we want to
     decrypt all encrypted cached data) so handle !len later in cleanup. */
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(len && !backend->recv_connection_closed) {
    /* the encrypted buffer must be large enough to hold all the bytes
       requested and some TLS record overhead. 'len' is a buffer size, so this
       integer math cannot overflow. */
    ಸ್ಥಿರಾಂಕ size_t min_encdata_ಉದ್ದಳತೆ = len + CURL_SCHANNEL_ಬಫರ್_FREE_ಗಾತ್ರ;

    /* make sure encrypt buffer fits the requested amount of data */
    ಫಲಿತಾಂಶ = ensure_encoding_ಗಾತ್ರ(ದತ್ತಾಂಶ, &backend->encdata, min_encdata_ಉದ್ದಳತೆ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು cleanup;

    SCH_DEV(infof(ದತ್ತಾಂಶ,
                  "schannel: encrypted data buffer: offset %zu length %zu",
                  backend->encdata.ಸ್ಥಾನಾಂತರ, backend->encdata.ಉದ್ದಳತೆ));

    /* read encrypted data from socket */
    ಫಲಿತಾಂಶ = Curl_conn_cf_recv(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ,
                               (ಅಕ್ಷರ *)(backend->encdata.ಬಫರ್ +
                                        backend->encdata.ಸ್ಥಾನಾಂತರ),
                               backend->encdata.ಉದ್ದಳತೆ -
                               backend->encdata.ಸ್ಥಾನಾಂತರ,
                               &nread);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
        SCH_DEV(infof(ದತ್ತಾಂಶ, "schannel: recv returned CURLE_AGAIN"));
      ಇಲ್ಲದಿದ್ದರೆ {
        infof(ದತ್ತಾಂಶ, "schannel: recv returned error %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
        backend->recv_unrecoverable_err = ಫಲಿತಾಂಶ;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(nread == 0) {
      backend->recv_connection_closed = TRUE;
      DEBUGF(infof(ದತ್ತಾಂಶ, "schannel: server closed the connection"));
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      backend->encdata.ಸ್ಥಾನಾಂತರ += nread;
      backend->encdata_is_incomplete = FALSE;
      SCH_DEV(infof(ದತ್ತಾಂಶ, "schannel: encrypted data got %zu", nread));
    }
  }

  SCH_DEV(infof(ದತ್ತಾಂಶ, "schannel: encrypted data buffer: offset %zu length %zu",
                backend->encdata.ಸ್ಥಾನಾಂತರ, backend->encdata.ಉದ್ದಳತೆ));

  /* decrypt loop */
  ಆಗ(backend->encdata.ಸ್ಥಾನಾಂತರ > 0 && sspi_ಸ್ಥಿತಿ == SEC_E_OK &&
        (!len || backend->decdata.ಸ್ಥಾನಾಂತರ < len ||
         backend->recv_connection_closed)) {
    /* prepare data buffer for DecryptMessage call */
    Init_Sec_ಬಫರ್(&inbuf[0], SECBUFFER_ದತ್ತಾಂಶ, backend->encdata.ಬಫರ್,
                  curlx_uztoul(backend->encdata.ಸ್ಥಾನಾಂತರ));

    /* we need 3 more empty input buffers for possible output */
    Init_Sec_ಬಫರ್(&inbuf[1], SECBUFFER_ಖಾಲಿಗಳು, ಖಾಲಿ, 0);
    Init_Sec_ಬಫರ್(&inbuf[2], SECBUFFER_ಖಾಲಿಗಳು, ಖಾಲಿ, 0);
    Init_Sec_ಬಫರ್(&inbuf[3], SECBUFFER_ಖಾಲಿಗಳು, ಖಾಲಿ, 0);
    Init_Sec_ಬಫರ್_Desc(&inbuf_desc, inbuf, 4);

    /* https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-decryptmessage
     */
    sspi_ಸ್ಥಿತಿ = Curl_pSecFn->Decrypt_ಸಂದೇಶ(&backend->ctxt->ctxt_handle,
                                              &inbuf_desc, 0, ಖಾಲಿ);

    /* check if everything went fine (server may want to renegotiate
       or shutdown the connection context) */
    ಆದರೆ(sspi_ಸ್ಥಿತಿ == SEC_E_OK || sspi_ಸ್ಥಿತಿ == SEC_I_RENEGOTIATE ||
       sspi_ಸ್ಥಿತಿ == SEC_I_CONTEXT_EXPIRED) {
      /* check for successfully decrypted data, even before actual
         renegotiation or shutdown of the connection context */
      ಆದರೆ(inbuf[1].ಬಫರ್_Type == SECBUFFER_ದತ್ತಾಂಶ) {
        SCH_DEV(infof(ದತ್ತಾಂಶ, "schannel: decrypted data length: %lu",
                      inbuf[1].cb_ಬಫರ್));

        /* ensure the decode buffer fits the received amount of data */
        ಫಲಿತಾಂಶ = ensure_decoding_ಗಾತ್ರ(ದತ್ತಾಂಶ, &backend->decdata,
                                      inbuf[1].cb_ಬಫರ್, len);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹೋಗು cleanup;

        /* copy decrypted data to internal buffer */
        ಗಾತ್ರ_2 = inbuf[1].cb_ಬಫರ್;
        ಆದರೆ(ಗಾತ್ರ_2) {
          ಸ್ಮರಣೆ_ನಕಲು(backend->decdata.ಬಫರ್ + backend->decdata.ಸ್ಥಾನಾಂತರ,
                 inbuf[1].pv_ಬಫರ್, ಗಾತ್ರ_2);
          backend->decdata.ಸ್ಥಾನಾಂತರ += ಗಾತ್ರ_2;
        }

        SCH_DEV(infof(ದತ್ತಾಂಶ, "schannel: decrypted data added: %zu", ಗಾತ್ರ_2));
        SCH_DEV(infof(ದತ್ತಾಂಶ,
                      "schannel: decrypted cached: offset %zu length %zu",
                      backend->decdata.ಸ್ಥಾನಾಂತರ, backend->decdata.ಉದ್ದಳತೆ));
      }

      /* check for remaining encrypted data */
      ಆದರೆ(inbuf[3].ಬಫರ್_Type == SECBUFFER_EXTRA && inbuf[3].cb_ಬಫರ್ > 0) {
        SCH_DEV(infof(ದತ್ತಾಂಶ, "schannel: encrypted data length: %lu",
                      inbuf[3].cb_ಬಫರ್));

        /* check if the remaining data is less than the total amount
         * and therefore begins after the already processed data
         */
        ಆದರೆ(backend->encdata.ಸ್ಥಾನಾಂತರ > inbuf[3].cb_ಬಫರ್) {
          /* move remaining encrypted data forward to the beginning of
ಬಫರ್*/
          memmove(backend->encdata.ಬಫರ್,
                  (backend->encdata.ಬಫರ್ + backend->encdata.ಸ್ಥಾನಾಂತರ) -
                  inbuf[3].cb_ಬಫರ್, inbuf[3].cb_ಬಫರ್);
          backend->encdata.ಸ್ಥಾನಾಂತರ = inbuf[3].cb_ಬಫರ್;
        }

        SCH_DEV(infof(ದತ್ತಾಂಶ,
                      "schannel: encrypted cached: offset %zu length %zu",
                      backend->encdata.ಸ್ಥಾನಾಂತರ, backend->encdata.ಉದ್ದಳತೆ));
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* reset encrypted buffer offset, because there is no data remaining */
        backend->encdata.ಸ್ಥಾನಾಂತರ = 0;
      }

      /* check if server wants to renegotiate the connection context */
      ಆದರೆ(sspi_ಸ್ಥಿತಿ == SEC_I_RENEGOTIATE) {
        infof(ದತ್ತಾಂಶ, "schannel: remote party requests renegotiation");
        ಆದರೆ(ಫಲಿತಾಂಶ && ಫಲಿತಾಂಶ != CURLE_AGAIN) {
          infof(ದತ್ತಾಂಶ, "schannel: cannot renegotiate, an error is pending");
          ಹೋಗು cleanup;
        }

        backend->recv_renegotiating = TRUE;
        ಫಲಿತಾಂಶ = schannel_recv_renegotiate(cf, ದತ್ತಾಂಶ, SCH_RENEG_CALLER_IS_RECV);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹೋಗು cleanup;

        /* now retry receiving data */
        sspi_ಸ್ಥಿತಿ = SEC_E_OK;
        ಮುಂದುವರಿ;
      }
      /* check if the server closed the connection */
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(sspi_ಸ್ಥಿತಿ == SEC_I_CONTEXT_EXPIRED) {
        /* In Windows 2000 SEC_I_CONTEXT_EXPIRED (close_notify) is not
           returned so we have to work around that in cleanup. */
        backend->recv_sspi_close_notify = TRUE;
        ಆದರೆ(!backend->recv_connection_closed)
          backend->recv_connection_closed = TRUE;
        /* We received the close notify fine, any error we got
         * from the lower filters afterwards (e.g. the socket), is not
         * an error on the TLS data stream. That one ended here. */
        ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_RECV_ERROR)
          ಫಲಿತಾಂಶ = CURLE_OK;
        infof(ದತ್ತಾಂಶ,
              "schannel: server close notification received (close_notify)");
        ಹೋಗು cleanup;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(sspi_ಸ್ಥಿತಿ == SEC_E_INCOMPLETE_ಸಂದೇಶ) {
      backend->encdata_is_incomplete = TRUE;
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಫಲಿತಾಂಶ = CURLE_AGAIN;
      SCH_DEV(infof(ದತ್ತಾಂಶ, "schannel: failed to decrypt data, need more data"));
      ಹೋಗು cleanup;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN];
      failf(ದತ್ತಾಂಶ, "schannel: failed to read data from server: %s",
            Curl_sspi_strerror(sspi_ಸ್ಥಿತಿ, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
      ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
      ಹೋಗು cleanup;
    }
  }

  SCH_DEV(infof(ದತ್ತಾಂಶ, "schannel: encrypted data buffer: offset %zu length %zu",
                backend->encdata.ಸ್ಥಾನಾಂತರ, backend->encdata.ಉದ್ದಳತೆ));

  SCH_DEV(infof(ದತ್ತಾಂಶ, "schannel: decrypted data buffer: offset %zu length %zu",
                backend->decdata.ಸ್ಥಾನಾಂತರ, backend->decdata.ಉದ್ದಳತೆ));

cleanup:
  /* Warning- there is no guarantee the encdata state is valid at this point */
  SCH_DEV(infof(ದತ್ತಾಂಶ, "schannel: schannel_recv cleanup"));

  /* Error if the connection has closed without a close_notify.

     The behavior here is a matter of debate. We do not want to be vulnerable
     to a truncation attack however there is some browser precedent for
     ignoring the close_notify for compatibility reasons. */
  ಆದರೆ(len && !backend->decdata.ಸ್ಥಾನಾಂತರ && backend->recv_connection_closed &&
     !backend->recv_sspi_close_notify) {
    ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
    failf(ದತ್ತಾಂಶ, "schannel: server closed abruptly (missing close_notify)");
  }

  /* Any error other than CURLE_AGAIN is an unrecoverable error. */
  ಆದರೆ(ಫಲಿತಾಂಶ && ಫಲಿತಾಂಶ != CURLE_AGAIN)
    backend->recv_unrecoverable_err = ಫಲಿತಾಂಶ;

  ಗಾತ್ರ_2 = len < backend->decdata.ಸ್ಥಾನಾಂತರ ? len : backend->decdata.ಸ್ಥಾನಾಂತರ;
  ಆದರೆ(ಗಾತ್ರ_2) {
    ಸ್ಮರಣೆ_ನಕಲು(buf, backend->decdata.ಬಫರ್, ಗಾತ್ರ_2);
    memmove(backend->decdata.ಬಫರ್, backend->decdata.ಬಫರ್ + ಗಾತ್ರ_2,
            backend->decdata.ಸ್ಥಾನಾಂತರ - ಗಾತ್ರ_2);
    backend->decdata.ಸ್ಥಾನಾಂತರ -= ಗಾತ್ರ_2;
    SCH_DEV(infof(ದತ್ತಾಂಶ, "schannel: decrypted data returned %zu", ಗಾತ್ರ_2));
    SCH_DEV(infof(ದತ್ತಾಂಶ,
                  "schannel: decrypted data buffer: offset %zu length %zu",
                  backend->decdata.ಸ್ಥಾನಾಂತರ, backend->decdata.ಉದ್ದಳತೆ));
    *pnread = ಗಾತ್ರ_2;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  ಆದರೆ(!ಫಲಿತಾಂಶ && !backend->recv_connection_closed)
    ಫಲಿತಾಂಶ = CURLE_AGAIN;

  /* it is debatable what to return when !len. We could return whatever error
     we got from decryption but instead we override here so the return is
     consistent. */
  ಆದರೆ(!len)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ bool schannel_ದತ್ತಾಂಶ_pending(ರಚನೆ Curl_cfilter *cf,
                                  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  DEBUGASSERT(backend);

  ಆದರೆ(backend->ctxt) /* SSL/TLS is in use */
    ಹಿಂತಿರುಗಿ backend->decdata.ಸ್ಥಾನಾಂತರ > 0 ||
           (backend->encdata.ಸ್ಥಾನಾಂತರ > 0 && !backend->encdata_is_incomplete) ||
           backend->recv_connection_closed ||
           backend->recv_sspi_close_notify ||
           backend->recv_unrecoverable_err;
  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ FALSE;
}

/* shut down the SSL connection and clean up related memory.
   this function can be called multiple times on the same connection including
   if the SSL connection failed (eg connection made but failed handshake). */
ಸ್ಥಿರ CURLcode schannel_shutdown(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  bool send_shutdown, bool *ಮುಗಿದ)
{
  /* See https://learn.microsoft.com/windows/win32/secauthn/shutting-down-an-schannel-connection
   * Shutting Down an Schannel Connection
   */
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(cf->shutdown) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(backend);

  /* Not supported in schannel */
  (ಶೂನ್ಯ)send_shutdown;

  *ಮುಗಿದ = FALSE;
  ಆದರೆ(backend->ctxt) {
    infof(ದತ್ತಾಂಶ, "schannel: shutting down SSL/TLS connection with %s port %d",
          connssl->peer.origin->hostname, connssl->peer.origin->ಪೋರ್ಟ್);
  }

  ಆದರೆ(!backend->ctxt || cf->shutdown) {
    *ಮುಗಿದ = TRUE;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(backend->cred && backend->ctxt && !backend->ಕಳುಹಿಸಿದ_shutdown &&
     !backend->shutdown_generated) {
    Sec_ಬಫರ್_Desc BuffDesc;
    Sec_ಬಫರ್ ಬಫರ್_2;
    SECURITY_ಸ್ಥಿತಿ sspi_ಸ್ಥಿತಿ;
    Sec_ಬಫರ್ outbuf;
    Sec_ಬಫರ್_Desc outbuf_desc;
    DWORD dwshut = SCHANNEL_SHUTDOWN;

    Init_Sec_ಬಫರ್(&ಬಫರ್_2, SECBUFFER_TOKEN, &dwshut, ಗಾತ್ರ(dwshut));
    Init_Sec_ಬಫರ್_Desc(&BuffDesc, &ಬಫರ್_2, 1);

    sspi_ಸ್ಥಿತಿ = Curl_pSecFn->ApplyControlToken(&backend->ctxt->ctxt_handle,
                                                 &BuffDesc);

    ಆದರೆ(sspi_ಸ್ಥಿತಿ != SEC_E_OK) {
      ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN];
      failf(ದತ್ತಾಂಶ, "schannel: ApplyControlToken failure: %s",
            Curl_sspi_strerror(sspi_ಸ್ಥಿತಿ, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    /* setup output buffer */
    Init_Sec_ಬಫರ್(&outbuf, SECBUFFER_ಖಾಲಿಗಳು, ಖಾಲಿ, 0);
    Init_Sec_ಬಫರ್_Desc(&outbuf_desc, &outbuf, 1);

    sspi_ಸ್ಥಿತಿ = Curl_pSecFn->InitializeSecurityContext(
      &backend->cred->cred_handle,
      &backend->ctxt->ctxt_handle,
      backend->cred->sni_hostname,
      backend->req_flags,
      0,
      0,
      ಖಾಲಿ,
      0,
      &backend->ctxt->ctxt_handle,
      &outbuf_desc,
      &backend->ret_flags, ಖಾಲಿ);

    ಆದರೆ((sspi_ಸ್ಥಿತಿ == SEC_E_OK) || (sspi_ಸ್ಥಿತಿ == SEC_I_CONTEXT_EXPIRED)) {
      backend->shutdown_token = outbuf;
      backend->shutdown_generated = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN];
      ಆದರೆ(outbuf.pv_ಬಫರ್)
        Curl_pSecFn->Free_Context_ಬಫರ್(outbuf.pv_ಬಫರ್);
      failf(ದತ್ತಾಂಶ, "schannel: generating close msg failed: %s",
            Curl_sspi_strerror(sspi_ಸ್ಥಿತಿ, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  ಆದರೆ(backend->shutdown_generated && !backend->ಕಳುಹಿಸಿದ_shutdown) {
    size_t written = 0;
    size_t ಉಳಿದ;

    DEBUGASSERT(backend->shutdown_ಸ್ಥಾನಾಂತರ <= backend->shutdown_token.cb_ಬಫರ್);
    ಉಳಿದ = backend->shutdown_token.cb_ಬಫರ್ - backend->shutdown_ಸ್ಥಾನಾಂತರ;

    ಆದರೆ(ಉಳಿದ) {
      ಫಲಿತಾಂಶ = Curl_conn_cf_send(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ,
                                 ((uint8_t *)backend->shutdown_token.pv_ಬಫರ್ +
                                  backend->shutdown_ಸ್ಥಾನಾಂತರ),
                                 ಉಳಿದ, FALSE, &written);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        backend->shutdown_ಸ್ಥಾನಾಂತರ += written;
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN || (!ಫಲಿತಾಂಶ && written < ಉಳಿದ)) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        ಫಲಿತಾಂಶ = CURLE_OK;
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ /* no shutdown message to send? */
      ಫಲಿತಾಂಶ = CURLE_OK;
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಆದರೆ(backend->recv_connection_closed) {
        /* The server already closed, so sending close_notify is optional. */
        *ಮುಗಿದ = TRUE;
        ಫಲಿತಾಂಶ = CURLE_OK;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        failf(ದತ್ತಾಂಶ, "schannel: error sending close msg: %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
        ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      }
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಆದರೆ(backend->shutdown_token.pv_ಬಫರ್)
      Curl_pSecFn->Free_Context_ಬಫರ್(backend->shutdown_token.pv_ಬಫರ್);
    memset(&backend->shutdown_token, 0, ಗಾತ್ರ(backend->shutdown_token));
    backend->shutdown_ಸ್ಥಾನಾಂತರ = 0;
    backend->ಕಳುಹಿಸಿದ_shutdown = TRUE;
    *ಮುಗಿದ = TRUE;
  }

  /* If the connection seems open and we have not seen the close notify
   * from the server yet, try to receive it. */
  ಆದರೆ(backend->cred && backend->ctxt &&
     !backend->recv_sspi_close_notify && !backend->recv_connection_closed) {
    ಅಕ್ಷರ ಬಫರ್[1024];
    size_t nread;

    ಫಲಿತಾಂಶ = schannel_recv(cf, ದತ್ತಾಂಶ, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್), &nread);
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SSL shutdown, error %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
      ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(nread == 0) {
      /* We got the close notify alert and are done. */
      backend->recv_connection_closed = TRUE;
      *ಮುಗಿದ = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* still data coming in? */
    }
  }

ಹೊರಹೋಗು:
  cf->shutdown = (ಫಲಿತಾಂಶ || *ಮುಗಿದ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ schannel_close(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;

  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(backend);

  ಆದರೆ(backend->shutdown_token.pv_ಬಫರ್)
    Curl_pSecFn->Free_Context_ಬಫರ್(backend->shutdown_token.pv_ಬಫರ್);
  memset(&backend->shutdown_token, 0, ಗಾತ್ರ(backend->shutdown_token));
  backend->shutdown_ಸ್ಥಾನಾಂತರ = 0;

  /* free SSPI Schannel API security context handle */
  ಆದರೆ(backend->ctxt) {
    ಆದರೆ(cf->conn->sslContext == &backend->ctxt->ctxt_handle)
      cf->conn->sslContext = ಖಾಲಿ;

    DEBUGF(infof(ದತ್ತಾಂಶ, "schannel: clear security context handle"));
    Curl_pSecFn->DeleteSecurityContext(&backend->ctxt->ctxt_handle);
    curlx_safefree(backend->ctxt);
  }

  /* free SSPI Schannel API credential handle */
  ಆದರೆ(backend->cred) {
    Curl_ssl_scache_ಬೀಗ(ದತ್ತಾಂಶ);
    schannel_session_free(backend->cred);
    Curl_ssl_scache_unlock(ದತ್ತಾಂಶ);
    backend->cred = ಖಾಲಿ;
  }

  /* free the buffer used to encrypt outgoing data */
  curlx_safefree(backend->send_ಬಫರ್);
  backend->send_ಬಫರ್_len = 0;

  /* free internal buffer for received encrypted data */
  ಆದರೆ(backend->encdata.ಬಫರ್) {
    curlx_safefree(backend->encdata.ಬಫರ್);
    backend->encdata.ಉದ್ದಳತೆ = 0;
    backend->encdata.ಸ್ಥಾನಾಂತರ = 0;
    backend->encdata_is_incomplete = FALSE;
  }

  /* free internal buffer for received decrypted data */
  ಆದರೆ(backend->decdata.ಬಫರ್) {
    curlx_safefree(backend->decdata.ಬಫರ್);
    backend->decdata.ಉದ್ದಳತೆ = 0;
    backend->decdata.ಸ್ಥಾನಾಂತರ = 0;
  }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ schannel_init(ಶೂನ್ಯ)
{
  ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *(APIENTRY *WINE_GET_ಆವೃತ್ತಿ_FN)(ಶೂನ್ಯ);
#ಆದರೆ defined(__clang__) && __clang_major__ >= 16
#ಸೂಚನೆ clang diagnostic ತಳ್ಳು
#ಸೂಚನೆ clang diagnostic ignored "-Wcast-function-type-strict"
#ಆದರೆ_ಅಂತ್ಯ
  WINE_GET_ಆವೃತ್ತಿ_FN p_wine_get_ಆವೃತ್ತಿ =
    CURLX_FUNCTION_CAST(WINE_GET_ಆವೃತ್ತಿ_FN,
      Get_Proc_ವಿಳಾಸ(GetModuleHandle(ಪಠ್ಯ_2("ntdll")), "wine_get_version"));
#ಆದರೆ defined(__clang__) && __clang_major__ >= 16
#ಸೂಚನೆ clang diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(p_wine_get_ಆವೃತ್ತಿ) {  /* WINE detected */
    curl_off_t ver = 0;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *wine_ಆವೃತ್ತಿ = p_wine_get_ಆವೃತ್ತಿ();  /* e.g. "6.0.2" */
    /* Assume ALPN support with WINE 6.0 or greater */
    ಆದರೆ(wine_ಆವೃತ್ತಿ)
      curlx_str_ಸಂಖ್ಯೆ(&wine_ಆವೃತ್ತಿ, &ver, 20);
    s_win_has_alpn = (ver >= 6);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* ALPN is supported on Windows 8.1 / Server 2012 R2 or newer. */
    s_win_has_alpn = curlx_verify_windows_ಆವೃತ್ತಿ(6, 3, 0, PLATFORM_WINNT,
                                                  ಆವೃತ್ತಿ_GREATER_THAN_EQUAL);
  }

  ಹಿಂತಿರುಗಿ Curl_sspi_global_init() == CURLE_OK ? 1 : 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ schannel_cleanup(ಶೂನ್ಯ)
{
  Curl_sspi_global_cleanup();
}

ಸ್ಥಿರ size_t schannel_ಆವೃತ್ತಿ(ಅಕ್ಷರ *ಬಫರ್, size_t ಗಾತ್ರ_2)
{
  ಹಿಂತಿರುಗಿ curl_msnprintf(ಬಫರ್, ಗಾತ್ರ_2, "Schannel");
}

ಸ್ಥಿರ CURLcode schannel_random(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *entropy, size_t ಉದ್ದಳತೆ)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;

  ಹಿಂತಿರುಗಿ curlx_win32_random(entropy, ಉದ್ದಳತೆ);
}

ಸ್ಥಿರ CURLcode schannel_checksum(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಒಳಹರಿವು,
                                  size_t inputlen,
                                  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *checksum,
                                  size_t checksumlen,
                                  DWORD provType,
                                  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ algId)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_FAILED_INIT;

  HCRYPTPROV hProv = 0;
  HCRYPTHASH hHash = 0;

  size_t off;

  DWORD cb_Hash_ಗಾತ್ರ;
  DWORD dw_Hash_ಗಾತ್ರ_Len;
  DWORD dwChecksumLen;

  /* since this can fail in multiple ways, zero memory first so we never
   * return old data
   */
  memset(checksum, 0, checksumlen);

  ಆದರೆ(!CryptAcquireContext(&hProv, ಖಾಲಿ, ಖಾಲಿ, provType,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    ಹೋಗು ಹೊರಹೋಗು;

  ಆದರೆ(!CryptCreateHash(hProv, algId, 0, 0, &hHash))
    ಹೋಗು ಹೊರಹೋಗು;

  ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;

  off = 0;
  ಆಗ(off < inputlen) {
    DWORD chunk = (DWORD)CURLMIN(inputlen - off, 0xffffffffUL);
    ಆದರೆ(!Crypt_Hash_ದತ್ತಾಂಶ(hHash, ಒಳಹರಿವು + off, chunk, 0))
      ಹೋಗು ಹೊರಹೋಗು;
    off += chunk;
  }

  /* get hash size */
  cb_Hash_ಗಾತ್ರ = 0;
  dw_Hash_ಗಾತ್ರ_Len = (DWORD)ಗಾತ್ರ(cb_Hash_ಗಾತ್ರ);
  ಆದರೆ(!CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE *)&cb_Hash_ಗಾತ್ರ,
                        &dw_Hash_ಗಾತ್ರ_Len, 0))
    ಹೋಗು ಹೊರಹೋಗು;

  /* check if hash fits into the return buffer */
  ಆದರೆ(checksumlen < cb_Hash_ಗಾತ್ರ)
    ಹೋಗು ಹೊರಹೋಗು;

  dwChecksumLen = (DWORD)checksumlen;
  ಆದರೆ(CryptGetHashParam(hHash, HP_HASHVAL, checksum, &dwChecksumLen, 0) &&
     dwChecksumLen == cb_Hash_ಗಾತ್ರ)
    ಫಲಿತಾಂಶ = CURLE_OK;

ಹೊರಹೋಗು:
  ಆದರೆ(hHash)
    CryptDestroyHash(hHash);

  ಆದರೆ(hProv)
    CryptReleaseContext(hProv, 0);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode schannel_sha256sum(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಒಳಹರಿವು,
                                   size_t inputlen,
                                   ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *sha256sum,
                                   size_t sha256len)
{
  ಹಿಂತಿರುಗಿ schannel_checksum(ಒಳಹರಿವು, inputlen, sha256sum, sha256len,
                           PROV_RSA_AES, CALG_SHA_256);
}

ಸ್ಥಿರ ಶೂನ್ಯ *schannel_get_internals(ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl,
                                    CURLINFO info)
{
  ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  (ಶೂನ್ಯ)info;
  DEBUGASSERT(backend);
  ಹಿಂತಿರುಗಿ &backend->ctxt->ctxt_handle;
}

HCERTSTORE Curl_schannel_get_cached_cert_store(ರಚನೆ Curl_cfilter *cf,
                                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ರಚನೆ Curl_multi *multi = ದತ್ತಾಂಶ->multi;
  ಸ್ಥಿರಾಂಕ ರಚನೆ curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ರಚನೆ schannel_cert_share *share;
  timediff_t timeout_ms;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ info_blob_digest[CURL_SHA256_DIGEST_ಉದ್ದಳತೆ];

  DEBUGASSERT(multi);

  ಆದರೆ(!multi) {
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  }

  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_SCHANNEL_CERT_SHARE_ಕೀಲಿ,
                         CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_ಕೀಲಿ));
  ಆದರೆ(!share || !share->cert_store) {
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  }

  /* zero ca_cache_timeout completely disables caching */
  ಆದರೆ(!ದತ್ತಾಂಶ->ಗಣ.ssl_ca_cache_timeout) {
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  }

  /* check for cache timeout by using the cached_x509_store_expired timediff
     calculation pattern from openssl.c.
     negative timeout means retain forever. */
  timeout_ms = ದತ್ತಾಂಶ->ಗಣ.ssl_ca_cache_timeout * (timediff_t)1000;
  ಆದರೆ(timeout_ms >= 0) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(ದತ್ತಾಂಶ), &share->ಸಮಯ);
    ಆದರೆ(elapsed_ms >= timeout_ms) {
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
  }

  ಆದರೆ(ca_info_blob) {
    ಆದರೆ(share->CAinfo_blob_ಗಾತ್ರ != ca_info_blob->len) {
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ(schannel_sha256sum((ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ca_info_blob->ದತ್ತಾಂಶ,
                          ca_info_blob->len,
                          info_blob_digest,
                          CURL_SHA256_DIGEST_ಉದ್ದಳತೆ))
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ(memcmp(share->CAinfo_blob_digest, info_blob_digest,
              CURL_SHA256_DIGEST_ಉದ್ದಳತೆ)) {
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(!conn_config->CAfile || !share->CAfile ||
       strcmp(share->CAfile, conn_config->CAfile)) {
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
  }

  ಹಿಂತಿರುಗಿ share->cert_store;
}

ಸ್ಥಿರ ಶೂನ್ಯ schannel_cert_share_free(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_len, ಶೂನ್ಯ *p)
{
  ರಚನೆ schannel_cert_share *share = p;
  DEBUGASSERT(ಕೀಲಿ_len == CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_ಕೀಲಿ));
  DEBUGASSERT(!memcmp(MPROTO_SCHANNEL_CERT_SHARE_ಕೀಲಿ, ಕೀಲಿ, ಕೀಲಿ_len));
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)ಕೀಲಿ_len;
  ಆದರೆ(share->cert_store) {
    CertCloseStore(share->cert_store, 0);
  }
  curlx_free(share->CAfile);
  curlx_free(share);
}

bool Curl_schannel_ಗಣ_cached_cert_store(ರಚನೆ Curl_cfilter *cf,
                                         ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         HCERTSTORE cert_store)
{
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ರಚನೆ Curl_multi *multi = ದತ್ತಾಂಶ->multi;
  ಸ್ಥಿರಾಂಕ ರಚನೆ curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ರಚನೆ schannel_cert_share *share;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ digest[CURL_SHA256_DIGEST_ಉದ್ದಳತೆ];
  ಅಕ್ಷರ *CAfile = ಖಾಲಿ;

  DEBUGASSERT(multi);

  ಆದರೆ(!multi) {
    ಹಿಂತಿರುಗಿ FALSE;
  }

  ಆದರೆ(ca_info_blob) {
    ಆದರೆ(schannel_sha256sum((ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ca_info_blob->ದತ್ತಾಂಶ,
                          ca_info_blob->len, digest, ಗಾತ್ರ(digest))) {
      ಹಿಂತಿರುಗಿ FALSE;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(conn_config->CAfile) {
    CAfile = curlx_strdup(conn_config->CAfile);
    ಆದರೆ(!CAfile) {
      ಹಿಂತಿರುಗಿ FALSE;
    }
  }

  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_SCHANNEL_CERT_SHARE_ಕೀಲಿ,
                         CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_ಕೀಲಿ));
  ಆದರೆ(!share) {
    share = curlx_calloc(1, ಗಾತ್ರ(*share));
    ಆದರೆ(!share) {
      curlx_free(CAfile);
      ಹಿಂತಿರುಗಿ FALSE;
    }
    ಆದರೆ(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_SCHANNEL_CERT_SHARE_ಕೀಲಿ,
                       CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_ಕೀಲಿ),
                       share, schannel_cert_share_free)) {
      curlx_free(share);
      curlx_free(CAfile);
      ಹಿಂತಿರುಗಿ FALSE;
    }
  }

  /* free old cache data */
  ಆದರೆ(share->cert_store) {
    CertCloseStore(share->cert_store, 0);
  }
  curlx_free(share->CAfile);

  ಆದರೆ(ca_info_blob) {
    ಸ್ಮರಣೆ_ನಕಲು(share->CAinfo_blob_digest, digest, ಗಾತ್ರ(digest));
    share->CAinfo_blob_ಗಾತ್ರ = ca_info_blob->len;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    share->CAinfo_blob_ಗಾತ್ರ = 0;

  share->ಸಮಯ = curlx_now();
  share->cert_store = cert_store;
  share->CAfile = CAfile;
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_ssl Curl_ssl_schannel = {
  { CURLSSLBACKEND_SCHANNEL, "schannel" }, /* info */

  SSLSUPP_CERTINFO |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_CA_CACHE |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST,

  ಗಾತ್ರ(ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ),

  schannel_init,                     /* init */
  schannel_cleanup,                  /* cleanup */
  schannel_ಆವೃತ್ತಿ,                  /*ಆವೃತ್ತಿ*/
  schannel_shutdown,                 /* shutdown */
  schannel_ದತ್ತಾಂಶ_pending,             /* data_pending */
  schannel_random,                   /* random */
  ಖಾಲಿ,                              /* cert_status_request */
  schannel_connect,                  /* connect */
  Curl_ssl_adjust_pollset,           /* adjust_pollset */
  schannel_get_internals,            /* get_internals */
  schannel_close,                    /* close_one */
  ಖಾಲಿ,                              /* close_all */
  ಖಾಲಿ,                              /* engines_list */
  schannel_sha256sum,                /* sha256sum */
  schannel_recv,                     /* recv decrypted data */
  schannel_send,                     /* send data to encrypt */
  ಖಾಲಿ,                              /* get_channel_binding */
};

#ಆದರೆ_ಅಂತ್ಯ /* USE_SCHANNEL */
