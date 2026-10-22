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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SCHANNEL

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI
#ਗਲਤੀ "cannot compile Schannel support without SSPI."
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/schannel.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/schannel_int.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls_int.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls_scache.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h" /* for the connect timeout */
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strerror.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h" /* for the socket readiness */
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/win32-fopen.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/multibyte.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/x509asn1.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/version_win32.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/winapi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_sha256.h"

/* Some verbose debug messages are wrapped by SCH_DEV() instead of DEBUGF()
 * and only shown if CURL_SCHANNEL_DEV_DEBUG was defined at build time. These
 * messages are extra verbose and intended for curl developers debugging
 * Schannel recv decryption and renegotiation.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_SCHANNEL_DEV_DEBUG
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCH_DEV(x) x
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCH_DEV_SHOWBOOL(x)                                     \
  ਕਰੋ {                                                          \
    infof(ਅੰਕੜੇ, "schannel: " #x " %s", (x) ? "TRUE" : "FALSE"); \
  } ਜਦੋਂ(0)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCH_DEV(x) ਕਰੋ {} ਜਦੋਂ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCH_DEV_SHOWBOOL(x) ਕਰੋ {} ਜਦੋਂ(0)
#ਜੇ_ਅੰਤ

/* Offered by mingw-w64 v8+, MS SDK 7.0A/VS2010+ */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SP_PROT_TLS1_0_CLIENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SP_PROT_TLS1_0_CLIENT           SP_PROT_TLS1_CLIENT
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SP_PROT_TLS1_1_CLIENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SP_PROT_TLS1_1_CLIENT           0x00000200
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SP_PROT_TLS1_2_CLIENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SP_PROT_TLS1_2_CLIENT           0x00000800
#ਜੇ_ਅੰਤ

/* Offered by mingw-w64 v8+, MS SDK 10.0.15063.0/VS2017 15.1+ */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SP_PROT_TLS1_3_CLIENT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SP_PROT_TLS1_3_CLIENT           0x00002000
#ਜੇ_ਅੰਤ
/* Offered by mingw-w64 v8+, MS SDK 8.1/VS2013+ */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SCH_USE_STRONG_CRYPTO
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCH_USE_STRONG_CRYPTO           0x00400000
#ਜੇ_ਅੰਤ

/* Offered by mingw-w64 v10+, MS SDK 7.0A/VS2010+ */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SECBUFFER_ALERT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SECBUFFER_ALERT                 17
#ਜੇ_ਅੰਤ

/* Both schannel buffer sizes must be > 0 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SCHANNEL_ਬਫਰ_INIT_ਮਾਪ   4096
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SCHANNEL_ਬਫਰ_FREE_ਮਾਪ   1024

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CERT_THUMBPRINT_STR_LEN 40
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CERT_THUMBPRINT_ਅੰਕੜੇ_LEN 20

/* Uncomment to force verbose output
 * #define infof(x, y, ...) curl_mprintf(y, __VA_ARGS__)
 * #define failf(x, y, ...) curl_mprintf(y, __VA_ARGS__)
 */

/* Offered by mingw-w64 v4+, MS SDK 6.0A/VS2008+ */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PKCS12_NO_PERSIST_ਕੁੰਜੀ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PKCS12_NO_PERSIST_ਕੁੰਜੀ 0x00008000
#ਜੇ_ਅੰਤ

/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CERT_ਲੱਭੋ_HAS_ਨਿੱਜੀ_ਕੁੰਜੀ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CERT_ਲੱਭੋ_HAS_ਨਿੱਜੀ_ਕੁੰਜੀ (21 << CERT_COMPARE_ਖਿਸਕਾਓ)
#ਜੇ_ਅੰਤ

/* key to use at `multi->proto_hash` */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MPROTO_SCHANNEL_CERT_SHARE_ਕੁੰਜੀ   "tls:schannel:cert:share"

ਸਥਿਰ bool s_win_has_alpn;

/* Offered by mingw-w64 v9+, MS SDK 8.1/VS2013+ */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SECBUFFER_APPLICATION_PROTOCOLS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SECBUFFER_APPLICATION_PROTOCOLS 18
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SECPKG_ATTR_APPLICATION_PROTOCOL 35

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  SecApplicationProtocolNegotiationExt_None,
  SecApplicationProtocolNegotiationExt_NPN,
  SecApplicationProtocolNegotiationExt_ALPN
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  Sec_Application_Protocol_Negotiation_ਸਥਿਤੀ_None,
  Sec_Application_Protocol_Negotiation_ਸਥਿਤੀ_Success,
  Sec_Application_Protocol_Negotiation_ਸਥਿਤੀ_Selected_Client_Only
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_ਸਥਿਤੀ;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
  SEC_APPLICATION_PROTOCOL_NEGOTIATION_ਸਥਿਤੀ Proto_Nego_ਸਥਿਤੀ;
  SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT ProtoNegoExt;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ Protocol_Id_ਮਾਪ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ProtocolId[0xff];
} SecPkgContext_ApplicationProtocol;
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ Init_Sec_ਬਫਰ(Sec_ਬਫਰ *ਬਫਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ BufType,
                          ਖਾਲੀ *Buf_ਅੰਕੜੇ_Ptr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ Buf_Byte_ਮਾਪ)
{
  ਬਫਰ->cb_ਬਫਰ = Buf_Byte_ਮਾਪ;
  ਬਫਰ->ਬਫਰ_Type = BufType;
  ਬਫਰ->pv_ਬਫਰ = Buf_ਅੰਕੜੇ_Ptr;
}

ਸਥਿਰ ਖਾਲੀ Init_Sec_ਬਫਰ_Desc(Sec_ਬਫਰ_Desc *desc, Sec_ਬਫਰ *BufArr,
                              ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਅੰਕ_Arr_Elem)
{
  desc->ul_ਸੰਸਕਰਣ = SECBUFFER_ਸੰਸਕਰਣ;
  desc->pBuffers = BufArr;
  desc->cBuffers = ਅੰਕ_Arr_Elem;
}

ਸਥਿਰ CURLcode schannel_ਸੈੱਟ_ssl_ਸੰਸਕਰਣ_min_max(DWORD *enabled_protocols,
                                                 ਬਣਤਰ Curl_cfilter *cf,
                                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ਲੰਮਾ ssl_ਸੰਸਕਰਣ = conn_config->ਸੰਸਕਰਣ_2;
  ਲੰਮਾ ssl_ਸੰਸਕਰਣ_max = (ਲੰਮਾ)conn_config->ਸੰਸਕਰਣ_max;
  ਲੰਮਾ i = ssl_ਸੰਸਕਰਣ;

  ਚੋਣ(ssl_ਸੰਸਕਰਣ_max) {
  ਹਾਲਤ CURL_SSLVERSION_MAX_NONE:
  ਹਾਲਤ CURL_SSLVERSION_MAX_DEFAULT:

    /* Windows Server 2022 and newer (including Windows 11) support TLS 1.3
       built-in. Previous builds of Windows 10 had broken TLS 1.3
       implementations that could be enabled via registry. */
    ਜੇ(curlx_verify_windows_ਸੰਸਕਰਣ(10, 0, 20348, PLATFORM_WINNT,
                                    ਸੰਸਕਰਣ_GREATER_THAN_EQUAL)) {
      ssl_ਸੰਸਕਰਣ_max = CURL_SSLVERSION_MAX_TLSv1_3;
    }
    ਨਹੀਂਤਾਂ /* Windows 10 or older */
      ssl_ਸੰਸਕਰਣ_max = CURL_SSLVERSION_MAX_TLSv1_2;

    ਰੋਕੋ;
  }

  ਲਈ(; i <= (ssl_ਸੰਸਕਰਣ_max >> 16); ++i) {
    ਚੋਣ(i) {
    ਹਾਲਤ CURL_SSLVERSION_TLSv1_0:
      *enabled_protocols |= SP_PROT_TLS1_0_CLIENT;
      ਰੋਕੋ;
    ਹਾਲਤ CURL_SSLVERSION_TLSv1_1:
      *enabled_protocols |= SP_PROT_TLS1_1_CLIENT;
      ਰੋਕੋ;
    ਹਾਲਤ CURL_SSLVERSION_TLSv1_2:
      *enabled_protocols |= SP_PROT_TLS1_2_CLIENT;
      ਰੋਕੋ;
    ਹਾਲਤ CURL_SSLVERSION_TLSv1_3:

      /* Windows Server 2022 or newer */
      ਜੇ(curlx_verify_windows_ਸੰਸਕਰਣ(10, 0, 20348, PLATFORM_WINNT,
                                      ਸੰਸਕਰਣ_GREATER_THAN_EQUAL)) {
        *enabled_protocols |= SP_PROT_TLS1_3_CLIENT;
        ਰੋਕੋ;
      }
      ਨਹੀਂਤਾਂ { /* Windows 10 or older */
        failf(ਅੰਕੜੇ, "schannel: TLS 1.3 not supported on Windows 10 or older");
        ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
      }
    }
  }
  ਵਾਪਸ CURLE_OK;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CIPHEROPTION(x) { #x, x }

ਬਣਤਰ algo {
  ਅਚਲ ਅੱਖਰ *name;
  ਪੂਰਨ id;
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ algo algs[] = {
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
  CIPHEROPTION(CALG_SCHANNEL_MAC_ਕੁੰਜੀ),
  CIPHEROPTION(CALG_SCHANNEL_ENC_ਕੁੰਜੀ),
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CALG_ECMQV
  CIPHEROPTION(CALG_ECMQV),
#ਜੇ_ਅੰਤ
  CIPHEROPTION(CALG_ECDSA),
/* Offered by mingw-w64 v7+, MS SDK 7.0A/VS2010+ */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CALG_ECDH_EPHEM
  CIPHEROPTION(CALG_ECDH_EPHEM),
#ਜੇ_ਅੰਤ
  { ਨਿਰਾ, 0 },
};

ਸਥਿਰ ਪੂਰਨ get_alg_id_by_name(ਅਚਲ ਅੱਖਰ *name)
{
  ਅਚਲ ਅੱਖਰ *name_ਸਿਰਾ = strchr(name, ':');
  size_t n = name_ਸਿਰਾ ? (size_t)(name_ਸਿਰਾ - name) : ਲੜੀ_ਲੰਬਾਈ(name);
  ਪੂਰਨ i;

  ਲਈ(i = 0; algs[i].name; i++) {
    ਜੇ((n == ਲੜੀ_ਲੰਬਾਈ(algs[i].name) && !ਲੜੀ_ਤੁਲਨਾ(algs[i].name, name, n)))
      ਵਾਪਸ algs[i].id;
  }
  ਵਾਪਸ 0; /* not found */
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅੰਕ_CIPHERS 47 /* There are a maximum of 47 options listed above */

ਸਥਿਰ CURLcode ਸੈੱਟ_ssl_ciphers(SCHANNEL_CRED *schannel_cred, ਅੱਖਰ *ciphers,
                                ALG_ID *algIds)
{
  ਅਚਲ ਅੱਖਰ *ਸ਼ੁਰੂ_Cur = ciphers;
  ਪੂਰਨ alg_ਗਿਣਤੀ = 0;
  ਜਦੋਂ(ਸ਼ੁਰੂ_Cur && *ਸ਼ੁਰੂ_Cur && (alg_ਗਿਣਤੀ < ਅੰਕ_CIPHERS)) {
    curl_off_t alg;
    ਜੇ(curlx_str_ਸੰਖਿਆ(&ਸ਼ੁਰੂ_Cur, &alg, ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) || !alg)
      alg = get_alg_id_by_name(ਸ਼ੁਰੂ_Cur);

    ਜੇ(alg)
      algIds[alg_ਗਿਣਤੀ++] = (ALG_ID)alg;
    ਨਹੀਂਤਾਂ ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(ਸ਼ੁਰੂ_Cur, "USE_STRONG_CRYPTO",
                     CURL_CSTRLEN("USE_STRONG_CRYPTO")) ||
            !ਲੜੀ_ਤੁਲਨਾ(ਸ਼ੁਰੂ_Cur, "SCH_USE_STRONG_CRYPTO",
                     CURL_CSTRLEN("SCH_USE_STRONG_CRYPTO")))
      schannel_cred->dwFlags |= SCH_USE_STRONG_CRYPTO;
    ਨਹੀਂਤਾਂ
      ਵਾਪਸ CURLE_SSL_CIPHER;
    ਸ਼ੁਰੂ_Cur = strchr(ਸ਼ੁਰੂ_Cur, ':');
    ਜੇ(ਸ਼ੁਰੂ_Cur)
      ਸ਼ੁਰੂ_Cur++;
  }
  schannel_cred->palgSupportedAlgs = algIds;
  schannel_cred->cSupportedAlgs = (DWORD)alg_ਗਿਣਤੀ;
  ਵਾਪਸ CURLE_OK;
}

/* Function allocates memory for store_path only if CURLE_OK is returned */
ਸਥਿਰ CURLcode get_cert_location(TCHAR *ਰਸਤਾ, DWORD *store_name,
                                  TCHAR **store_ਰਸਤਾ, TCHAR **thumbprint)
{
  TCHAR *sep;
  TCHAR *store_ਰਸਤਾ_ਸ਼ੁਰੂ;
  size_t store_name_len;

  sep = _tcschr(ਰਸਤਾ, ਪਾਠ('\\'));
  ਜੇ(!sep)
    ਵਾਪਸ CURLE_SSL_CERTPROBLEM;

  store_name_len = sep - ਰਸਤਾ;

  ਜੇ(_tcsncmp(ਰਸਤਾ, ਪਾਠ("CurrentUser"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_USER;
  ਨਹੀਂਤਾਂ ਜੇ(_tcsncmp(ਰਸਤਾ, ਪਾਠ("LocalMachine"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE;
  ਨਹੀਂਤਾਂ ਜੇ(_tcsncmp(ਰਸਤਾ, ਪਾਠ("CurrentService"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_SERVICE;
  ਨਹੀਂਤਾਂ ਜੇ(_tcsncmp(ਰਸਤਾ, ਪਾਠ("Services"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_SERVICES;
  ਨਹੀਂਤਾਂ ਜੇ(_tcsncmp(ਰਸਤਾ, ਪਾਠ("Users"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_USERS;
  ਨਹੀਂਤਾਂ ਜੇ(_tcsncmp(ਰਸਤਾ, ਪਾਠ("CurrentUserGroupPolicy"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_USER_ਸਮੂਹ_POLICY;
  ਨਹੀਂਤਾਂ ਜੇ(_tcsncmp(ਰਸਤਾ, ਪਾਠ("LocalMachineGroupPolicy"), store_name_len) ==
          0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE_ਸਮੂਹ_POLICY;
  ਨਹੀਂਤਾਂ ਜੇ(_tcsncmp(ਰਸਤਾ, ਪਾਠ("LocalMachineEnterprise"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE_ENTERPRISE;
  ਨਹੀਂਤਾਂ
    ਵਾਪਸ CURLE_SSL_CERTPROBLEM;

  store_ਰਸਤਾ_ਸ਼ੁਰੂ = sep + 1;

  sep = _tcschr(store_ਰਸਤਾ_ਸ਼ੁਰੂ, ਪਾਠ('\\'));
  ਜੇ(!sep)
    ਵਾਪਸ CURLE_SSL_CERTPROBLEM;

  *thumbprint = sep + 1;
  ਜੇ(_tcslen(*thumbprint) != CERT_THUMBPRINT_STR_LEN)
    ਵਾਪਸ CURLE_SSL_CERTPROBLEM;

  *sep = ਪਾਠ('\0');
  *store_ਰਸਤਾ = curlx_tcsdup(store_ਰਸਤਾ_ਸ਼ੁਰੂ);
  *sep = ਪਾਠ('\\');
  ਜੇ(!*store_ਰਸਤਾ)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode get_client_cert(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                HCERTSTORE *ਬਾਹਰਲਾ_cert_store,
                                PCCERT_CONTEXT *ਬਾਹਰਲਾ_cert_context)
{
  ਬਣਤਰ ssl_filter_config *sslc = Curl_ssl_cf_get_filter_config(cf);
  PCCERT_CONTEXT client_cert = ਨਿਰਾ;
  HCERTSTORE client_cert_store = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(sslc->clientcert || sslc->cert_blob) {
    DWORD cert_store_name = 0;
    TCHAR *cert_store_ਰਸਤਾ = ਨਿਰਾ;
    TCHAR *cert_thumbprint_str = ਨਿਰਾ;
    TCHAR cert_thumbprint_buf[CERT_THUMBPRINT_STR_LEN + 1];
    CRYPT_HASH_BLOB cert_thumbprint;
    BYTE cert_thumbprint_ਅੰਕੜੇ[CERT_THUMBPRINT_ਅੰਕੜੇ_LEN];
    HCERTSTORE cert_store = ਨਿਰਾ;
    FILE *f_ਵਿੱਚ_Cert = ਨਿਰਾ;
    ਖਾਲੀ *certdata = ਨਿਰਾ;
    size_t certsize = 0;
    bool blob = !!sslc->cert_blob;

    ਜੇ(blob) {
      certdata = sslc->cert_blob->ਅੰਕੜੇ;
      certsize = sslc->cert_blob->len;
    }
    ਨਹੀਂਤਾਂ {
      TCHAR *cert_ਰਸਤਾ =
        curlx_convert_UTF8_ਮੰਜ਼ਿਲ_tchar(sslc->clientcert);
      ਜੇ(!cert_ਰਸਤਾ)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

      ਨਤੀਜਾ = get_cert_location(cert_ਰਸਤਾ, &cert_store_name,
                                 &cert_store_ਰਸਤਾ, &cert_thumbprint_str);

      /* 'cert_thumbprint_str' points in to the allocated 'cert_path', copy
         the data. The string is verified to be CERT_THUMBPRINT_STR_LEN bytes
         long within the get_cert_location() function. */
      ਜੇ(!ਨਤੀਜਾ && cert_thumbprint_str) {
        ਸਮ੍ਰਿਤੀ_ਨਕਲ(cert_thumbprint_buf, cert_thumbprint_str,
               ਆਕਾਰ(cert_thumbprint_buf));
        cert_thumbprint_str = cert_thumbprint_buf;
      }

      curlx_free(cert_ਰਸਤਾ);
      ਜੇ(ਨਤੀਜਾ && (sslc->clientcert[0] != '\0'))
        f_ਵਿੱਚ_Cert = curlx_fopen(sslc->clientcert, "rb");

      ਜੇ(ਨਤੀਜਾ && !f_ਵਿੱਚ_Cert) {
        failf(ਅੰਕੜੇ, "schannel: Failed to get certificate location"
              " or file for %s",
              sslc->clientcert);
        ਵਾਪਸ ਨਤੀਜਾ;
      }
    }

    ਜੇ((f_ਵਿੱਚ_Cert || blob) && sslc->cert_type &&
       !curl_strequal(sslc->cert_type, "P12")) {
      failf(ਅੰਕੜੇ, "schannel: certificate format compatibility error "
            "for %s",
            blob ? "(memory blob)" : sslc->clientcert);
      curlx_free(cert_store_ਰਸਤਾ);
      ਜੇ(f_ਵਿੱਚ_Cert)
        curlx_fclose(f_ਵਿੱਚ_Cert);
      ਵਾਪਸ CURLE_SSL_CERTPROBLEM;
    }

    ਜੇ(f_ਵਿੱਚ_Cert || blob) {
      /* Reading a .p12 or .pfx file, like the example at bottom of
         https://learn.microsoft.com/archive/msdn-technet-forums/3e7bc95f-b21a-4bcd-bd2c-7f996718cae5
       */
      CRYPT_ਅੰਕੜੇ_BLOB datablob;
      WCHAR *pszPassword;
      size_t pwd_len = 0;
      ਪੂਰਨ cert_ਲੱਭੋ_flags;
      ਅਚਲ ਅੱਖਰ *cert_showfilename_error = blob ?
        "(memory blob)" : sslc->clientcert;
      curlx_free(cert_store_ਰਸਤਾ);
      ਜੇ(f_ਵਿੱਚ_Cert) {
        ਲੰਮਾ cert_tell = 0;
        bool continue_reading = fseek(f_ਵਿੱਚ_Cert, 0, SEEK_END) == 0;
        ਜੇ(continue_reading) {
          cert_tell = ftell(f_ਵਿੱਚ_Cert);
          ਜੇ(cert_tell < 0)
            continue_reading = FALSE;
          ਨਹੀਂਤਾਂ
            certsize = (size_t)cert_tell;
        }
        ਜੇ(continue_reading)
          continue_reading = fseek(f_ਵਿੱਚ_Cert, 0, SEEK_SET) == 0;
        ਜੇ(continue_reading && (certsize < CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ))
          certdata = curlx_malloc(certsize + 1);
        ਜੇ(!certdata || ((ਪੂਰਨ)fread(certdata, certsize, 1, f_ਵਿੱਚ_Cert) != 1))
          continue_reading = FALSE;
        curlx_fclose(f_ਵਿੱਚ_Cert);
        ਜੇ(!continue_reading) {
          failf(ਅੰਕੜੇ, "schannel: Failed to read cert file %s",
                sslc->clientcert);
          curlx_free(certdata);
          ਵਾਪਸ CURLE_SSL_CERTPROBLEM;
        }
      }

      /* Convert key-pair data to the in-memory certificate store */
      datablob.pb_ਅੰਕੜੇ = (BYTE *)certdata;
      datablob.cb_ਅੰਕੜੇ = (DWORD)certsize;

      ਜੇ(sslc->ਕੁੰਜੀ_passwd)
        pwd_len = ਲੜੀ_ਲੰਬਾਈ(sslc->ਕੁੰਜੀ_passwd);
      pszPassword = curlx_malloc(ਆਕਾਰ(WCHAR) * (pwd_len + 1));
      ਜੇ(pszPassword) {
        ਪੂਰਨ str_w_len = 0;
        ਜੇ(pwd_len > 0)
          str_w_len = Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                          sslc->ਕੁੰਜੀ_passwd, (ਪੂਰਨ)pwd_len,
                                          pszPassword, (ਪੂਰਨ)(pwd_len + 1));

        ਜੇ((str_w_len >= 0) && (str_w_len <= (ਪੂਰਨ)pwd_len))
          pszPassword[str_w_len] = 0;
        ਨਹੀਂਤਾਂ
          pszPassword[0] = 0;

        cert_store = PFXImportCertStore(&datablob, pszPassword,
                                        PKCS12_NO_PERSIST_ਕੁੰਜੀ);
        curlx_memzero(pszPassword, ਆਕਾਰ(WCHAR) * (pwd_len + 1));
        curlx_free(pszPassword);
      }
      ਜੇ(!blob)
        curlx_free(certdata);
      ਜੇ(!cert_store) {
        DWORD errorcode = Get_ਆਖਰੀ_Error();
        ਜੇ(errorcode == ERROR_INVALID_PASSWORD)
          failf(ਅੰਕੜੇ, "schannel: Failed to import cert file %s, "
                "password is bad",
                cert_showfilename_error);
        ਨਹੀਂਤਾਂ
          failf(ਅੰਕੜੇ, "schannel: Failed to import cert file %s, "
                "last error is 0x%08lx",
                cert_showfilename_error, errorcode);
        ਵਾਪਸ CURLE_SSL_CERTPROBLEM;
      }

      /* CERT_FIND_HAS_PRIVATE_KEY is only available in Windows 8 / Server
         2012, (NT 6.2). For older versions we use CURL_FIND_ANY. */
      ਜੇ(curlx_verify_windows_ਸੰਸਕਰਣ(6, 2, 0, PLATFORM_WINNT,
                                      ਸੰਸਕਰਣ_GREATER_THAN_EQUAL))
        cert_ਲੱਭੋ_flags = CERT_ਲੱਭੋ_HAS_ਨਿੱਜੀ_ਕੁੰਜੀ;
      ਨਹੀਂਤਾਂ
        cert_ਲੱਭੋ_flags = CERT_ਲੱਭੋ_ANY;

      client_cert =
        Cert_ਲੱਭੋ_Certificate_ਵਿੱਚ_Store(cert_store,
                                   X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                   cert_ਲੱਭੋ_flags, ਨਿਰਾ, ਨਿਰਾ);

      ਜੇ(!client_cert) {
        failf(ਅੰਕੜੇ, "schannel: Failed to get certificate from file %s"
              ", last error is 0x%08lx",
              cert_showfilename_error, Get_ਆਖਰੀ_Error());
        CertCloseStore(cert_store, 0);
        ਵਾਪਸ CURLE_SSL_CERTPROBLEM;
      }
    }
    ਨਹੀਂਤਾਂ {
      cert_store =
        CertOpenStore(
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNICODE
                      CERT_STORE_PROV_SYSTEM_W,
#ਨਹੀਂਤਾਂ
                      CERT_STORE_PROV_SYSTEM_A,
#ਜੇ_ਅੰਤ
                      0,
                      (HCRYPTPROV)ਨਿਰਾ,
                      CERT_STORE_OPEN_EXISTING_FLAG | cert_store_name,
                      cert_store_ਰਸਤਾ);
      ਜੇ(!cert_store) {
        ਅੱਖਰ *ਰਸਤਾ_utf8 =
          curlx_convert_tchar_ਮੰਜ਼ਿਲ_UTF8(cert_store_ਰਸਤਾ);
        failf(ਅੰਕੜੇ, "schannel: Failed to open cert store %lx %s, "
              "last error is 0x%08lx",
              cert_store_name, (ਰਸਤਾ_utf8 ? ਰਸਤਾ_utf8 : "(unknown)"),
              Get_ਆਖਰੀ_Error());
        curlx_free(cert_store_ਰਸਤਾ);
        curlx_free(ਰਸਤਾ_utf8);
        ਵਾਪਸ CURLE_SSL_CERTPROBLEM;
      }
      curlx_free(cert_store_ਰਸਤਾ);

      cert_thumbprint.pb_ਅੰਕੜੇ = cert_thumbprint_ਅੰਕੜੇ;
      cert_thumbprint.cb_ਅੰਕੜੇ = CERT_THUMBPRINT_ਅੰਕੜੇ_LEN;

      ਜੇ(!Crypt_String_ਮੰਜ਼ਿਲ_ਦੁਆਧਾਰੀ(cert_thumbprint_str,
                              CERT_THUMBPRINT_STR_LEN,
                              CRYPT_STRING_HEX,
                              cert_thumbprint_ਅੰਕੜੇ,
                              &cert_thumbprint.cb_ਅੰਕੜੇ,
                              ਨਿਰਾ, ਨਿਰਾ)) {
        CertCloseStore(cert_store, 0);
        ਵਾਪਸ CURLE_SSL_CERTPROBLEM;
      }

      client_cert =
        Cert_ਲੱਭੋ_Certificate_ਵਿੱਚ_Store(cert_store,
                                   X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                   CERT_ਲੱਭੋ_HASH, &cert_thumbprint, ਨਿਰਾ);
      ਜੇ(!client_cert) {
        /* CRYPT_E_NOT_FOUND / E_INVALIDARG */
        CertCloseStore(cert_store, 0);
        failf(ਅੰਕੜੇ, "schannel: client cert not found in cert store");
        ਵਾਪਸ CURLE_SSL_CERTPROBLEM;
      }
    }
    client_cert_store = cert_store;
  }

  *ਬਾਹਰਲਾ_cert_store = client_cert_store;
  *ਬਾਹਰਲਾ_cert_context = client_cert;

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode acquire_sspi_handle(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *backend,
                                    PCCERT_CONTEXT client_cert,
                                    DWORD flags,
                                    DWORD enabled_protocols)
{
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SECURITY_ਸਥਿਤੀ sspi_ਸਥਿਤੀ = SEC_E_OK;
  CURLcode ਨਤੀਜਾ;

  /* We support TLS 1.3 starting in Windows Server 2022 or later
   * (OS build 20348) */
  ਜੇ(!conn_config->cipher_list &&
     curlx_verify_windows_ਸੰਸਕਰਣ(10, 0, 20348, PLATFORM_WINNT,
                                  ਸੰਸਕਰਣ_GREATER_THAN_EQUAL)) {

    SCH_CREDENTIALS credentials = { 0 };
    TLS_PARAMETERS tls_parameters = { 0 };
    CRYPTO_SETTINGS crypto_settings[1];
    PCCERT_CONTEXT client_certs[1];

    ਜੇ(client_cert)
      client_certs[0] = client_cert;
    ਨਹੀਂਤਾਂ
      client_certs[0] = ਨਿਰਾ;

    memset(crypto_settings, 0, ਆਕਾਰ(crypto_settings));

    tls_parameters.pDisabledCrypto = crypto_settings;

    /* The number of blocked suites */
    tls_parameters.cDisabledCrypto = (DWORD)0;
    credentials.pTlsParameters = &tls_parameters;
    credentials.cTlsParameters = 1;

    credentials.dw_ਸੰਸਕਰਣ = SCH_CREDENTIALS_ਸੰਸਕਰਣ;
    credentials.dwFlags = flags | SCH_USE_STRONG_CRYPTO;

    credentials.pTlsParameters->grbitDisabledProtocols = ~enabled_protocols;

    ਜੇ(client_certs[0]) {
      credentials.cCreds = 1;
      credentials.paCred = client_certs;
    }

    sspi_ਸਥਿਤੀ =
      Curl_pSecFn->AcquireCredentialsHandle(ਨਿਰਾ, CURL_UNCONST(UNISP_NAME),
                                            SECPKG_CRED_OUTBOUND, ਨਿਰਾ,
                                            &credentials, ਨਿਰਾ, ਨਿਰਾ,
                                            &backend->cred->cred_handle, ਨਿਰਾ);
  }
  ਨਹੀਂਤਾਂ {
    /* Pre-Windows 10 1809 or the user set a legacy algorithm list.
       Schannel does not negotiate TLS 1.3 when SCHANNEL_CRED is used. */
    ALG_ID algIds[ਅੰਕ_CIPHERS];
    ਅੱਖਰ *ciphers = conn_config->cipher_list;
    SCHANNEL_CRED schannel_cred = { 0 };
    PCCERT_CONTEXT client_certs[1];

    ਜੇ(client_cert)
      client_certs[0] = client_cert;
    ਨਹੀਂਤਾਂ
      client_certs[0] = ਨਿਰਾ;

    schannel_cred.dw_ਸੰਸਕਰਣ = SCHANNEL_CRED_ਸੰਸਕਰਣ;
    schannel_cred.dwFlags = flags;
    schannel_cred.grbitEnabledProtocols = enabled_protocols;

    ਜੇ(ciphers) {
      ਜੇ((enabled_protocols & SP_PROT_TLS1_3_CLIENT)) {
        ਜੇ(!(enabled_protocols & ~SP_PROT_TLS1_3_CLIENT)) {
          failf(ਅੰਕੜੇ, "schannel: TLS 1.3 is not supported with a cipher list; "
                "remove the cipher list or allow a lower TLS version");
          ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
        }
        infof(ਅੰਕੜੇ, "schannel: WARNING: This version of Schannel "
              "negotiates a less-secure TLS version than TLS 1.3 because the "
              "user set an algorithm cipher list.");
      }
      ਨਤੀਜਾ = ਸੈੱਟ_ssl_ciphers(&schannel_cred, ciphers, algIds);
      ਜੇ(ਨਤੀਜਾ) {
        failf(ਅੰਕੜੇ, "schannel: Failed setting algorithm cipher list");
        ਵਾਪਸ ਨਤੀਜਾ;
      }
    }
    ਨਹੀਂਤਾਂ {
      schannel_cred.dwFlags = flags | SCH_USE_STRONG_CRYPTO;
    }

    ਜੇ(client_certs[0]) {
      schannel_cred.cCreds = 1;
      schannel_cred.paCred = client_certs;
    }

    sspi_ਸਥਿਤੀ =
      Curl_pSecFn->AcquireCredentialsHandle(ਨਿਰਾ, CURL_UNCONST(UNISP_NAME),
                                            SECPKG_CRED_OUTBOUND, ਨਿਰਾ,
                                            &schannel_cred, ਨਿਰਾ, ਨਿਰਾ,
                                            &backend->cred->cred_handle, ਨਿਰਾ);
  }

  ਜੇ(sspi_ਸਥਿਤੀ != SEC_E_OK) {
    ਅੱਖਰ ਬਫਰ[STRERROR_LEN];
    failf(ਅੰਕੜੇ, "schannel: AcquireCredentialsHandle failed: %s",
          Curl_sspi_strerror(sspi_ਸਥਿਤੀ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
    ਚੋਣ(sspi_ਸਥਿਤੀ) {
    ਹਾਲਤ SEC_E_INSUFFICIENT_ਸਮ੍ਰਿਤੀ:
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਹਾਲਤ SEC_E_NO_CREDENTIALS:
    ਹਾਲਤ SEC_E_SECPKG_NOT_ਮਿਲਿਆ:
    ਹਾਲਤ SEC_E_NOT_OWNER:
    ਹਾਲਤ SEC_E_UNKNOWN_CREDENTIALS:
    ਹਾਲਤ SEC_E_ਅੰਦਰੂਨੀ_ERROR:
    ਮੂਲ:
      ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
    }
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode schannel_acquire_credential_handle(ਬਣਤਰ Curl_cfilter *cf,
                                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  PCCERT_CONTEXT client_cert = ਨਿਰਾ;
  HCERTSTORE client_cert_store = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;

  /* setup Schannel API options */
  DWORD flags = 0;
  DWORD enabled_protocols = 0;

  ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *)connssl->backend;

  DEBUGASSERT(backend);

  ਜੇ(conn_config->verifypeer) {
    ਜੇ(backend->use_manual_cred_validation)
      flags = SCH_CRED_MANUAL_CRED_VALIDATION;
    ਨਹੀਂਤਾਂ
      flags = SCH_CRED_AUTO_CRED_VALIDATION;

    ਜੇ(conn_config->no_revoke) {
      flags |= SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
               SCH_CRED_IGNORE_REVOCATION_OFFLINE;

      DEBUGF(infof(ਅੰਕੜੇ, "schannel: disabled server certificate revocation "
                         "checks"));
    }
    ਨਹੀਂਤਾਂ ਜੇ(conn_config->revoke_ਉੱਤਮ_effort) {
      flags |= SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
               SCH_CRED_IGNORE_REVOCATION_OFFLINE |
               SCH_CRED_REVOCATION_CHECK_CHAIN;

      DEBUGF(infof(ਅੰਕੜੇ, "schannel: ignore revocation offline errors"));
    }
    ਨਹੀਂਤਾਂ {
      flags |= SCH_CRED_REVOCATION_CHECK_CHAIN;

      DEBUGF(infof(ਅੰਕੜੇ, "schannel: checking server certificate revocation"));
    }
  }
  ਨਹੀਂਤਾਂ {
    flags = SCH_CRED_MANUAL_CRED_VALIDATION |
            SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
            SCH_CRED_IGNORE_REVOCATION_OFFLINE;
    DEBUGF(infof(ਅੰਕੜੇ, "schannel: disabled server cert revocation checks"));
  }

  ਜੇ(!conn_config->verifyhost) {
    flags |= SCH_CRED_NO_SERVERNAME_CHECK;
    DEBUGF(infof(ਅੰਕੜੇ, "schannel: verifyhost setting prevents Schannel from "
                       "comparing the supplied target name with the subject "
                       "names in server certificates."));
  }

  ਜੇ(!conn_config->auto_client_cert) {
    flags &= ~(DWORD)SCH_CRED_USE_DEFAULT_CREDS;
    flags |= SCH_CRED_NO_DEFAULT_CREDS;
    infof(ਅੰਕੜੇ, "schannel: disabled automatic use of client certificate");
  }
  ਨਹੀਂਤਾਂ
    infof(ਅੰਕੜੇ, "schannel: enabled automatic use of client certificate");

  DEBUGASSERT(conn_config->ਸੰਸਕਰਣ_2 != CURL_SSLVERSION_DEFAULT);
  ਚੋਣ(conn_config->ਸੰਸਕਰਣ_2) {
  ਹਾਲਤ CURL_SSLVERSION_TLSv1:
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_0:
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_1:
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_2:
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_3: {
    ਨਤੀਜਾ = schannel_ਸੈੱਟ_ssl_ਸੰਸਕਰਣ_min_max(&enabled_protocols, cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    ਰੋਕੋ;
  }
  ਹਾਲਤ CURL_SSLVERSION_SSLv3:
  ਹਾਲਤ CURL_SSLVERSION_SSLv2:
    failf(ਅੰਕੜੇ, "SSL versions not supported");
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
  ਮੂਲ:
    failf(ਅੰਕੜੇ, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
  }

  ਨਤੀਜਾ = get_client_cert(cf, ਅੰਕੜੇ, &client_cert_store, &client_cert);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* allocate memory for the reusable credential handle */
  backend->cred = (ਬਣਤਰ Curl_schannel_cred *)
    curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ Curl_schannel_cred));
  ਜੇ(!backend->cred) {
    failf(ਅੰਕੜੇ, "schannel: unable to allocate memory");

    ਜੇ(client_cert)
      CertFreeCertificateContext(client_cert);
    ਜੇ(client_cert_store)
      CertCloseStore(client_cert_store, 0);

    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
  backend->cred->refcount = 1;

  /* Since we did not persist the key, we need to extend the store's
   * lifetime until the end of the connection
   */
  backend->cred->client_cert_store = client_cert_store;

  ਨਤੀਜਾ = acquire_sspi_handle(cf, ਅੰਕੜੇ, backend, client_cert,
                               flags, enabled_protocols);

  ਜੇ(client_cert)
    CertFreeCertificateContext(client_cert);

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode schannel_connect_step1(ਬਣਤਰ Curl_cfilter *cf,
                                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  size_t written = 0;
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  Sec_ਬਫਰ outbuf;
  Sec_ਬਫਰ_Desc outbuf_desc;
  Sec_ਬਫਰ inbuf;
  Sec_ਬਫਰ_Desc inbuf_desc;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ alpn_ਬਫਰ[128];
  SECURITY_ਸਥਿਤੀ sspi_ਸਥਿਤੀ = SEC_E_OK;
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(backend);
  DEBUGF(infof(ਅੰਕੜੇ, "schannel: SSL/TLS connection with %s port %d (step 1/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->ਪੋਰਟ));

  backend->use_alpn = connssl->alpn && s_win_has_alpn;

  ਜੇ(conn_config->CAfile || conn_config->ca_info_blob) {
    ਜੇ(curlx_verify_windows_ਸੰਸਕਰਣ(6, 1, 0, PLATFORM_WINNT,
                                    ਸੰਸਕਰਣ_GREATER_THAN_EQUAL)) {
      backend->use_manual_cred_validation = TRUE;
    }
    ਨਹੀਂਤਾਂ {
      failf(ਅੰਕੜੇ, "schannel: this version of Windows is too old to support "
                  "certificate verification via CA bundle file.");
      ਵਾਪਸ CURLE_SSL_CACERT_BADFILE;
    }
  }
  ਨਹੀਂਤਾਂ
    backend->use_manual_cred_validation = FALSE;

  backend->cred = ਨਿਰਾ;

  /* check for an existing reusable credential handle */
  ਜੇ(Curl_ssl_scache_use(cf, ਅੰਕੜੇ)) {
    ਬਣਤਰ Curl_schannel_cred *old_cred;
    Curl_ssl_scache_ਤਾਲਾ(ਅੰਕੜੇ);
    old_cred = Curl_ssl_scache_get_obj(cf, ਅੰਕੜੇ, connssl->peer.scache_ਕੁੰਜੀ);
    ਜੇ(old_cred) {
      backend->cred = old_cred;
      DEBUGF(infof(ਅੰਕੜੇ, "schannel: reusing existing credential handle"));

      /* increment the reference counter of the credential/session handle */
      backend->cred->refcount++;
      DEBUGF(infof(ਅੰਕੜੇ,
                   "schannel: incremented credential handle refcount = %d",
                   backend->cred->refcount));
    }
    Curl_ssl_scache_unlock(ਅੰਕੜੇ);
  }

  ਜੇ(!backend->cred) {
    ਅੱਖਰ *snihost;
    ਨਤੀਜਾ = schannel_acquire_credential_handle(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ || !backend->cred)
      ਵਾਪਸ ਨਤੀਜਾ;
    /* schannel_acquire_credential_handle() sets backend->cred accordingly or
       it returns error otherwise. */

    /* A hostname associated with the credential is needed by
       InitializeSecurityContext for SNI and other reasons. */
    snihost = connssl->peer.sni ?
      connssl->peer.sni : connssl->peer.origin->hostname;
    backend->cred->sni_hostname = curlx_convert_UTF8_ਮੰਜ਼ਿਲ_tchar(snihost);
    ਜੇ(!backend->cred->sni_hostname)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  /* Warn if SNI is disabled due to use of an IP address */
  ਜੇ(connssl->peer.type != CURL_SSL_PEER_DNS) {
    infof(ਅੰਕੜੇ, "schannel: using IP address, SNI is not supported by OS.");
  }

  ਜੇ(backend->use_alpn) {
    ਪੂਰਨ cur = 0;
    ਪੂਰਨ list_ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ = 0;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *extension_len = ਨਿਰਾ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ *list_len = ਨਿਰਾ;
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ uiALPN = SecApplicationProtocolNegotiationExt_ALPN;
    ਬਣਤਰ alpn_proto_buf proto;

    /* The first four bytes is an unsigned int indicating number
       of bytes of data in the rest of the buffer. */
    extension_len = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *)(ਖਾਲੀ *)&alpn_ਬਫਰ[cur];
    cur += (ਪੂਰਨ)ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ);

    /* The next four bytes are an indicator that this buffer contains
       ALPN data, as opposed to NPN, for example. */
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(&alpn_ਬਫਰ[cur], &uiALPN, ਆਕਾਰ(uiALPN));
    cur += (ਪੂਰਨ)ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ);

    /* The next two bytes is an unsigned short indicating the number
       of bytes used to list the preferred protocols. */
    list_len = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ *)(ਖਾਲੀ *)&alpn_ਬਫਰ[cur];
    cur += (ਪੂਰਨ)ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ);

    list_ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ = cur;

    ਨਤੀਜਾ = Curl_alpn_ਮੰਜ਼ਿਲ_proto_buf(&proto, connssl->alpn);
    ਜੇ(ਨਤੀਜਾ) {
      failf(ਅੰਕੜੇ, "Error setting ALPN");
      ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
    }
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(&alpn_ਬਫਰ[cur], proto.ਅੰਕੜੇ, proto.len);
    cur += proto.len;

    *list_len = curlx_uitous(cur - list_ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ);
    *extension_len = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(*list_len +
      ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ) + ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ));

    Init_Sec_ਬਫਰ(&inbuf, SECBUFFER_APPLICATION_PROTOCOLS, alpn_ਬਫਰ, cur);
    Init_Sec_ਬਫਰ_Desc(&inbuf_desc, &inbuf, 1);

    Curl_alpn_ਮੰਜ਼ਿਲ_proto_str(&proto, connssl->alpn);
    infof(ਅੰਕੜੇ, VTLS_INFOF_ALPN_OFFER_1STR, proto.ਅੰਕੜੇ);
  }
  ਨਹੀਂਤਾਂ {
    Init_Sec_ਬਫਰ(&inbuf, SECBUFFER_ਖਾਲੀ_ਥਾਵਾਂ, ਨਿਰਾ, 0);
    Init_Sec_ਬਫਰ_Desc(&inbuf_desc, &inbuf, 1);
  }

  /* setup output buffer */
  Init_Sec_ਬਫਰ(&outbuf, SECBUFFER_ਖਾਲੀ_ਥਾਵਾਂ, ਨਿਰਾ, 0);
  Init_Sec_ਬਫਰ_Desc(&outbuf_desc, &outbuf, 1);

  /* security request flags */
  backend->req_flags =
    ISC_REQ_SEQUENCE_DETECT | ISC_REQ_REPLAY_DETECT |
    ISC_REQ_CONFIDENTIALITY | ISC_REQ_ALLOCATE_ਸਮ੍ਰਿਤੀ |
    ISC_REQ_ਧਾਰਾ |
    (!conn_config->auto_client_cert ? ISC_REQ_USE_SUPPLIED_CREDS : 0);

  /* allocate memory for the security context handle */
  backend->ctxt = (ਬਣਤਰ Curl_schannel_ctxt *)
    curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ Curl_schannel_ctxt));
  ਜੇ(!backend->ctxt) {
    failf(ਅੰਕੜੇ, "schannel: unable to allocate memory");
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  /* Schannel InitializeSecurityContext:
     https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-initializesecuritycontextw

     At the moment we do not pass inbuf unless we are using ALPN since we only
     use it for that, and WINE (for which we currently disable ALPN) is giving
     us problems with inbuf regardless. https://github.com/curl/curl/issues/983
   */
  sspi_ਸਥਿਤੀ = Curl_pSecFn->InitializeSecurityContext(
    &backend->cred->cred_handle, ਨਿਰਾ, backend->cred->sni_hostname,
    backend->req_flags, 0, 0,
    (backend->use_alpn ? &inbuf_desc : ਨਿਰਾ),
    0, &backend->ctxt->ctxt_handle,
    &outbuf_desc, &backend->ret_flags, ਨਿਰਾ);

  ਜੇ(sspi_ਸਥਿਤੀ != SEC_I_CONTINUE_NEEDED) {
    ਅੱਖਰ ਬਫਰ[STRERROR_LEN];
    curlx_safefree(backend->ctxt);
    ਚੋਣ(sspi_ਸਥਿਤੀ) {
    ਹਾਲਤ SEC_E_INSUFFICIENT_ਸਮ੍ਰਿਤੀ:
      failf(ਅੰਕੜੇ, "schannel: initial InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(sspi_ਸਥਿਤੀ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਹਾਲਤ SEC_E_WRONG_PRINCIPAL:
      failf(ਅੰਕੜੇ, "schannel: SNI or certificate check failed: %s",
            Curl_sspi_strerror(sspi_ਸਥਿਤੀ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
      ਵਾਪਸ CURLE_PEER_FAILED_VERIFICATION;
#ਜੇ 0
    ਹਾਲਤ SEC_E_INVALID_HANDLE:
    ਹਾਲਤ SEC_E_INVALID_TOKEN:
    ਹਾਲਤ SEC_E_LOGON_DENIED:
    ਹਾਲਤ SEC_E_ਨਿਸ਼ਾਨਾ_UNKNOWN:
    ਹਾਲਤ SEC_E_NO_AUTHENTICATING_AUTHORITY:
    ਹਾਲਤ SEC_E_ਅੰਦਰੂਨੀ_ERROR:
    ਹਾਲਤ SEC_E_NO_CREDENTIALS:
    ਹਾਲਤ SEC_E_UNSUPPORTED_FUNCTION:
    ਹਾਲਤ SEC_E_APPLICATION_PROTOCOL_MISMATCH:
#ਜੇ_ਅੰਤ
    ਮੂਲ:
      failf(ਅੰਕੜੇ, "schannel: initial InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(sspi_ਸਥਿਤੀ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
      ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
    }
  }

  DEBUGF(infof(ਅੰਕੜੇ, "schannel: sending initial handshake data: "
               "sending %lu bytes.", outbuf.cb_ਬਫਰ));

  /* send initial handshake data which is now stored in output buffer */
  ਨਤੀਜਾ = Curl_conn_cf_send(cf->ਅਗਲਾ, ਅੰਕੜੇ,
                             (ਅਚਲ uint8_t *)outbuf.pv_ਬਫਰ,
                             outbuf.cb_ਬਫਰ, FALSE,
                             &written);
  Curl_pSecFn->Free_Context_ਬਫਰ(outbuf.pv_ਬਫਰ);
  ਜੇ(ਨਤੀਜਾ || (outbuf.cb_ਬਫਰ != written)) {
    failf(ਅੰਕੜੇ, "schannel: failed to send initial handshake data: "
          "sent %zu of %lu bytes", written, outbuf.cb_ਬਫਰ);
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGF(infof(ਅੰਕੜੇ, "schannel: sent initial handshake data: "
               "sent %zu bytes", written));

  backend->recv_unrecoverable_err = CURLE_OK;
  backend->recv_sspi_close_notify = FALSE;
  backend->recv_connection_closed = FALSE;
  backend->recv_renegotiating = FALSE;
  backend->renegotiate_state.started = FALSE;
  backend->encdata_is_incomplete = FALSE;
  backend->shutdown_generated = FALSE;
  backend->shutdown_ਵਿੱਥ = 0;
  memset(&backend->shutdown_token, 0, ਆਕਾਰ(backend->shutdown_token));

  /* continue to second handshake step */
  connssl->connecting_state = ssl_connect_2;

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode schannel_error(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               SECURITY_ਸਥਿਤੀ sspi_ਸਥਿਤੀ)
{
  ਅੱਖਰ ਬਫਰ[STRERROR_LEN];
  ਚੋਣ(sspi_ਸਥਿਤੀ) {
  ਹਾਲਤ SEC_E_INSUFFICIENT_ਸਮ੍ਰਿਤੀ:
    failf(ਅੰਕੜੇ, "schannel: next InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(sspi_ਸਥਿਤੀ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  ਹਾਲਤ SEC_E_WRONG_PRINCIPAL:
    failf(ਅੰਕੜੇ, "schannel: SNI or certificate check failed: %s",
          Curl_sspi_strerror(sspi_ਸਥਿਤੀ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
    ਵਾਪਸ CURLE_PEER_FAILED_VERIFICATION;
  ਹਾਲਤ SEC_E_UNTRUSTED_ਜੜ੍ਹ:
    failf(ਅੰਕੜੇ, "schannel: %s",
          Curl_sspi_strerror(sspi_ਸਥਿਤੀ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
    ਵਾਪਸ CURLE_PEER_FAILED_VERIFICATION;
#ਜੇ 0
  ਹਾਲਤ SEC_E_INVALID_HANDLE:
  ਹਾਲਤ SEC_E_INVALID_TOKEN:
  ਹਾਲਤ SEC_E_LOGON_DENIED:
  ਹਾਲਤ SEC_E_ਨਿਸ਼ਾਨਾ_UNKNOWN:
  ਹਾਲਤ SEC_E_NO_AUTHENTICATING_AUTHORITY:
  ਹਾਲਤ SEC_E_ਅੰਦਰੂਨੀ_ERROR:
  ਹਾਲਤ SEC_E_NO_CREDENTIALS:
  ਹਾਲਤ SEC_E_UNSUPPORTED_FUNCTION:
  ਹਾਲਤ SEC_E_APPLICATION_PROTOCOL_MISMATCH:
#ਜੇ_ਅੰਤ
  ਮੂਲ:
    failf(ਅੰਕੜੇ, "schannel: next InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(sspi_ਸਥਿਤੀ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
  }
}

ਸਥਿਰ CURLcode schannel_pkp_pin_peer_pubkey(ਬਣਤਰ Curl_cfilter *cf,
                                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਅਚਲ ਅੱਖਰ *pinnedpubkey)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  CERT_CONTEXT *pCertContextServer = ਨਿਰਾ;

  /* Result is returned to caller */
  CURLcode ਨਤੀਜਾ = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  DEBUGASSERT(backend);

  /* if a path was not specified, do not pin */
  ਜੇ(!pinnedpubkey)
    ਵਾਪਸ CURLE_OK;

  ਕਰੋ {
    SECURITY_ਸਥਿਤੀ sspi_ਸਥਿਤੀ;
    uint8_t *x509_der;
    DWORD x509_der_len;
    ਬਣਤਰ Curl_X509certificate x509_parsed;
    ਬਣਤਰ Curl_asn1Element *pubkey;

    sspi_ਸਥਿਤੀ =
      Curl_pSecFn->Query_Context_ਗੁਣ(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                          &pCertContextServer);

    ਜੇ((sspi_ਸਥਿਤੀ != SEC_E_OK) || !pCertContextServer) {
      ਅੱਖਰ ਬਫਰ[STRERROR_LEN];
      failf(ਅੰਕੜੇ, "schannel: Failed to read remote certificate context: %s",
            Curl_sspi_strerror(sspi_ਸਥਿਤੀ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
      ਰੋਕੋ; /* failed */
    }

    ਜੇ(!(((pCertContextServer->dwCertEncodingType & X509_ASN_ENCODING) != 0) &&
         (pCertContextServer->cbCertEncoded > 0)))
      ਰੋਕੋ;

    x509_der = (uint8_t *)pCertContextServer->pbCertEncoded;
    x509_der_len = pCertContextServer->cbCertEncoded;
    memset(&x509_parsed, 0, ਆਕਾਰ(x509_parsed));
    ਜੇ(Curl_parseX509(&x509_parsed, x509_der, x509_der + x509_der_len))
      ਰੋਕੋ;

    pubkey = &x509_parsed.subject_ਜਨਤਕ_ਕੁੰਜੀ_Info;
    ਜੇ(!pubkey->ਸਿਰਲੇਖ || pubkey->ਸਿਰਾ <= pubkey->ਸਿਰਲੇਖ) {
      failf(ਅੰਕੜੇ, "SSL: failed retrieving public key from server certificate");
      ਰੋਕੋ;
    }

    ਨਤੀਜਾ = Curl_pin_peer_pubkey(ਅੰਕੜੇ,
                                  pinnedpubkey,
                                  (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)pubkey->ਸਿਰਲੇਖ,
                                  (size_t)(pubkey->ਸਿਰਾ - pubkey->ਸਿਰਲੇਖ));
    ਜੇ(ਨਤੀਜਾ) {
      failf(ਅੰਕੜੇ, "SSL: public key does not match pinned public key");
    }
  } ਜਦੋਂ(0);

  ਜੇ(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ensure_encoding_ਮਾਪ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ sbuffer *encdata,
                                     size_t min_ਲੰਮਾਈ)
{
  size_t ਮਾਪ;
  DEBUGASSERT(encdata->ਲੰਮਾਈ >= encdata->ਵਿੱਥ);
  ਜੇ(encdata->ਲੰਮਾਈ < encdata->ਵਿੱਥ)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਮਾਪ = encdata->ਲੰਮਾਈ - encdata->ਵਿੱਥ;
  ਜੇ(ਮਾਪ < CURL_SCHANNEL_ਬਫਰ_FREE_ਮਾਪ || encdata->ਲੰਮਾਈ < min_ਲੰਮਾਈ) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਬਫਰ;
    size_t ਲੰਮਾਈ = encdata->ਵਿੱਥ + CURL_SCHANNEL_ਬਫਰ_FREE_ਮਾਪ;
    ਜੇ(ਲੰਮਾਈ < min_ਲੰਮਾਈ)
      ਲੰਮਾਈ = min_ਲੰਮਾਈ;
    ਬਫਰ = curlx_realloc(encdata->ਬਫਰ, ਲੰਮਾਈ);
    ਜੇ(!ਬਫਰ) {
      failf(ਅੰਕੜੇ, "schannel: unable to re-allocate memory");
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }

    encdata->ਬਫਰ = ਬਫਰ;
    encdata->ਲੰਮਾਈ = ਲੰਮਾਈ;
    SCH_DEV(infof(ਅੰਕੜੇ, "schannel: encdata.buffer resized %zu",
                  encdata->ਲੰਮਾਈ));
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ensure_decoding_ਮਾਪ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ sbuffer *decdata,
                                     size_t nowsize,
                                     size_t len)
{
  size_t ਮਾਪ = nowsize > CURL_SCHANNEL_ਬਫਰ_FREE_ਮਾਪ ?
    nowsize : CURL_SCHANNEL_ਬਫਰ_FREE_ਮਾਪ;
  DEBUGASSERT(decdata->ਲੰਮਾਈ >= decdata->ਵਿੱਥ);
  ਜੇ(decdata->ਲੰਮਾਈ < decdata->ਵਿੱਥ)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਨਹੀਂਤਾਂ ਜੇ(decdata->ਲੰਮਾਈ - decdata->ਵਿੱਥ < ਮਾਪ ||
          decdata->ਲੰਮਾਈ < len) {
    /* increase internal decrypted data buffer */
    size_t ਲੰਮਾਈ = decdata->ਵਿੱਥ + ਮਾਪ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਬਫਰ;
    /* make sure that the requested amount of data fits */
    ਜੇ(ਲੰਮਾਈ < len)
      ਲੰਮਾਈ = len;

    ਬਫਰ = curlx_realloc(decdata->ਬਫਰ, ਲੰਮਾਈ);
    ਜੇ(!ਬਫਰ) {
      failf(ਅੰਕੜੇ, "schannel: unable to re-allocate memory");
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
    decdata->ਬਫਰ = ਬਫਰ;
    decdata->ਲੰਮਾਈ = ਲੰਮਾਈ;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode schannel_connect_step2(ਬਣਤਰ Curl_cfilter *cf,
                                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ਪੂਰਨ i;
  size_t nread = 0;
  Sec_ਬਫਰ outbuf[3];
  Sec_ਬਫਰ_Desc outbuf_desc;
  Sec_ਬਫਰ inbuf[2];
  Sec_ਬਫਰ_Desc inbuf_desc;
  SECURITY_ਸਥਿਤੀ sspi_ਸਥਿਤੀ = SEC_E_OK;
  CURLcode ਨਤੀਜਾ;
  bool doread;
  ਅਚਲ ਅੱਖਰ *pubkey_ptr;

  DEBUGASSERT(backend);

  doread = (connssl->io_need & CURL_SSL_IO_NEED_SEND) ? FALSE : TRUE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  DEBUGF(infof(ਅੰਕੜੇ, "schannel: SSL/TLS connection with %s port %d (step 2/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->ਪੋਰਟ));

  ਜੇ(!backend->cred || !backend->ctxt)
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;

  /* buffer to store previously received and decrypted data */
  ਜੇ(!backend->decdata.ਬਫਰ) {
    backend->decdata.ਵਿੱਥ = 0;
    backend->decdata.ਲੰਮਾਈ = CURL_SCHANNEL_ਬਫਰ_INIT_ਮਾਪ;
    backend->decdata.ਬਫਰ = curlx_malloc(backend->decdata.ਲੰਮਾਈ);
    ਜੇ(!backend->decdata.ਬਫਰ) {
      failf(ਅੰਕੜੇ, "schannel: unable to allocate memory");
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
  }

  /* buffer to store previously received and encrypted data */
  ਜੇ(!backend->encdata.ਬਫਰ) {
    backend->encdata_is_incomplete = FALSE;
    backend->encdata.ਵਿੱਥ = 0;
    backend->encdata.ਲੰਮਾਈ = CURL_SCHANNEL_ਬਫਰ_INIT_ਮਾਪ;
    backend->encdata.ਬਫਰ = curlx_malloc(backend->encdata.ਲੰਮਾਈ);
    ਜੇ(!backend->encdata.ਬਫਰ) {
      failf(ਅੰਕੜੇ, "schannel: unable to allocate memory");
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
  }

  ਨਤੀਜਾ = ensure_encoding_ਮਾਪ(ਅੰਕੜੇ, &backend->encdata, 0);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਲਈ(;;) {
    ਜੇ(doread) {
      /* read encrypted handshake data from socket */
      ਨਤੀਜਾ = Curl_conn_cf_recv(cf->ਅਗਲਾ, ਅੰਕੜੇ,
                                 (ਅੱਖਰ *)(backend->encdata.ਬਫਰ +
                                          backend->encdata.ਵਿੱਥ),
                                 backend->encdata.ਲੰਮਾਈ -
                                 backend->encdata.ਵਿੱਥ,
                                 &nread);
      ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
        ਜੇ(!backend->encdata.ਵਿੱਥ || backend->encdata_is_incomplete) {
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          DEBUGF(infof(ਅੰਕੜੇ, "schannel: failed to receive handshake, "
                       "need more data"));
          ਵਾਪਸ CURLE_OK;
        }
        ਨਹੀਂਤਾਂ {
          DEBUGF(infof(ਅੰਕੜੇ, "schannel: no new handshake data received, "
                       "continuing to process existing handshake data"));
        }
      }
      ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ || (nread == 0)) {
        failf(ਅੰਕੜੇ, "schannel: failed to receive handshake, "
              "SSL/TLS connection failed");
        ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
      }
      ਨਹੀਂਤਾਂ {
        /* increase encrypted data buffer offset */
        backend->encdata.ਵਿੱਥ += nread;
        backend->encdata_is_incomplete = FALSE;
        SCH_DEV(infof(ਅੰਕੜੇ, "schannel: encrypted data got %zu", nread));
      }
    }

    SCH_DEV(infof(ਅੰਕੜੇ,
                  "schannel: encrypted data buffer: offset %zu length %zu",
                  backend->encdata.ਵਿੱਥ, backend->encdata.ਲੰਮਾਈ));

    /* setup input buffers */
    Init_Sec_ਬਫਰ(&inbuf[0], SECBUFFER_TOKEN,
                  curlx_memdup0((ਅਚਲ ਅੱਖਰ *)backend->encdata.ਬਫਰ,
                                backend->encdata.ਵਿੱਥ),
                  curlx_uztoul(backend->encdata.ਵਿੱਥ));
    Init_Sec_ਬਫਰ(&inbuf[1], SECBUFFER_ਖਾਲੀ_ਥਾਵਾਂ, ਨਿਰਾ, 0);
    Init_Sec_ਬਫਰ_Desc(&inbuf_desc, inbuf, 2);

    /* setup output buffers */
    Init_Sec_ਬਫਰ(&outbuf[0], SECBUFFER_TOKEN, ਨਿਰਾ, 0);
    Init_Sec_ਬਫਰ(&outbuf[1], SECBUFFER_ALERT, ਨਿਰਾ, 0);
    Init_Sec_ਬਫਰ(&outbuf[2], SECBUFFER_ਖਾਲੀ_ਥਾਵਾਂ, ਨਿਰਾ, 0);
    Init_Sec_ਬਫਰ_Desc(&outbuf_desc, outbuf, 3);

    ਜੇ(!inbuf[0].pv_ਬਫਰ) {
      failf(ਅੰਕੜੇ, "schannel: unable to allocate memory");
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }

    /* The socket must be writable (or a poll error occurred) before we call
       InitializeSecurityContext to continue processing the received TLS
       records. This is because that function is not idempotent and we do not
       support partial save/resume sending replies of handshake tokens. */
    ਜੇ(!SOCKET_WRITABLE(Curl_conn_cf_get_socket(cf, ਅੰਕੜੇ), 0)) {
      SCH_DEV(infof(ਅੰਕੜੇ, "schannel: handshake waiting for writable socket"));
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      curlx_free(inbuf[0].pv_ਬਫਰ);
      ਵਾਪਸ CURLE_OK;
    }

    sspi_ਸਥਿਤੀ = Curl_pSecFn->InitializeSecurityContext(
      &backend->cred->cred_handle, &backend->ctxt->ctxt_handle,
      backend->cred->sni_hostname, backend->req_flags,
      0, 0, &inbuf_desc, 0, ਨਿਰਾ,
      &outbuf_desc, &backend->ret_flags, ਨਿਰਾ);

    /* free buffer for received handshake data */
    curlx_safefree(inbuf[0].pv_ਬਫਰ);

    /* check if the handshake was incomplete */
    ਚੋਣ(sspi_ਸਥਿਤੀ) {
    ਹਾਲਤ SEC_E_INCOMPLETE_ਸੁਨੇਹਾ:
      backend->encdata_is_incomplete = TRUE;
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      DEBUGF(infof(ਅੰਕੜੇ,
                   "schannel: received incomplete message, need more data"));
      ਵਾਪਸ CURLE_OK;

    ਹਾਲਤ SEC_I_CONTINUE_NEEDED:
    ਹਾਲਤ SEC_E_OK:
      /* check if the handshake needs to be continued */
      ਨਤੀਜਾ = CURLE_OK;
      ਲਈ(i = 0; i < 3; i++) {
        /* search for handshake tokens that need to be sent */
        ਜੇ(outbuf[i].ਬਫਰ_Type == SECBUFFER_TOKEN && outbuf[i].cb_ਬਫਰ > 0) {
          size_t written = 0;
          DEBUGF(infof(ਅੰਕੜੇ, "schannel: sending next handshake data: "
                       "sending %lu bytes.", outbuf[i].cb_ਬਫਰ));

          /* send handshake token to server */
          ਨਤੀਜਾ = Curl_conn_cf_send(cf->ਅਗਲਾ, ਅੰਕੜੇ,
                                     (ਅਚਲ uint8_t *)outbuf[i].pv_ਬਫਰ,
                                     outbuf[i].cb_ਬਫਰ,
                                     FALSE, &written);
          ਜੇ(ਨਤੀਜਾ || (outbuf[i].cb_ਬਫਰ != written)) {
            failf(ਅੰਕੜੇ, "schannel: failed to send next handshake data: "
                  "sent %zu of %lu bytes", written, outbuf[i].cb_ਬਫਰ);
            ਨਤੀਜਾ = CURLE_SSL_CONNECT_ERROR;
          }
        }
      }
      ਲਈ(i = 0; i < 3; i++) {
        /* free obsolete buffer */
        ਜੇ(outbuf[i].pv_ਬਫਰ)
          Curl_pSecFn->Free_Context_ਬਫਰ(outbuf[i].pv_ਬਫਰ);
      }
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
      ਰੋਕੋ;

    ਹਾਲਤ SEC_I_INCOMPLETE_CREDENTIALS:
      ਜੇ(!(backend->req_flags & ISC_REQ_USE_SUPPLIED_CREDS)) {
        /* If the server has requested a client certificate, attempt to
           continue the handshake without one. This allows connections to
           servers which request a client certificate but do not require
           it. */
        backend->req_flags |= ISC_REQ_USE_SUPPLIED_CREDS;
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        DEBUGF(infof(ਅੰਕੜੇ,
                     "schannel: a client certificate has been requested"));
        ਵਾਪਸ CURLE_OK;
      }
      FALLTHROUGH();

    ਮੂਲ:
      ਵਾਪਸ schannel_error(ਅੰਕੜੇ, sspi_ਸਥਿਤੀ);
    }

    /* check if there was additional remaining encrypted data */
    ਜੇ(inbuf[1].ਬਫਰ_Type == SECBUFFER_EXTRA && inbuf[1].cb_ਬਫਰ > 0) {
      SCH_DEV(infof(ਅੰਕੜੇ, "schannel: encrypted data length: %lu",
                    inbuf[1].cb_ਬਫਰ));
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
      ਜੇ(backend->encdata.ਵਿੱਥ > inbuf[1].cb_ਬਫਰ) {
        memmove(backend->encdata.ਬਫਰ,
                (backend->encdata.ਬਫਰ + backend->encdata.ਵਿੱਥ) -
                inbuf[1].cb_ਬਫਰ, inbuf[1].cb_ਬਫਰ);
        backend->encdata.ਵਿੱਥ = inbuf[1].cb_ਬਫਰ;
        ਜੇ(sspi_ਸਥਿਤੀ == SEC_I_CONTINUE_NEEDED) {
          doread = FALSE;
          ਜਾਰੀ;
        }
      }
    }
    ਨਹੀਂਤਾਂ {
      backend->encdata.ਵਿੱਥ = 0;
    }
    ਰੋਕੋ;
  }

  /* check if the handshake needs to be continued */
  ਜੇ(sspi_ਸਥਿਤੀ == SEC_I_CONTINUE_NEEDED) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    ਵਾਪਸ CURLE_OK;
  }

  /* check if the handshake is complete */
  ਜੇ(sspi_ਸਥਿਤੀ == SEC_E_OK) {
    connssl->connecting_state = ssl_connect_3;
    DEBUGF(infof(ਅੰਕੜੇ, "schannel: SSL/TLS handshake complete"));
  }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  pubkey_ptr = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSL_PINNEDPUBLICKEY);
#ਨਹੀਂਤਾਂ
  pubkey_ptr = CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSL_PINNEDPUBLICKEY);
#ਜੇ_ਅੰਤ
  ਜੇ(pubkey_ptr) {
    ਨਤੀਜਾ = schannel_pkp_pin_peer_pubkey(cf, ਅੰਕੜੇ, pubkey_ptr);
    ਜੇ(ਨਤੀਜਾ) {
      failf(ਅੰਕੜੇ, "SSL: public key does not match pinned public key");
      ਵਾਪਸ ਨਤੀਜਾ;
    }
  }

  ਜੇ(conn_config->verifypeer && backend->use_manual_cred_validation) {
    /* Certificate verification also verifies the hostname if verifyhost */
    ਵਾਪਸ Curl_verify_certificate(cf, ਅੰਕੜੇ);
  }

  /* Verify the hostname manually when certificate verification is disabled,
     because in that case Schannel does not verify it. */
  ਜੇ(!conn_config->verifypeer && conn_config->verifyhost)
    ਵਾਪਸ Curl_verify_host(cf, ਅੰਕੜੇ);

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ bool valid_cert_encoding(ਅਚਲ CERT_CONTEXT *cert_context)
{
  ਵਾਪਸ cert_context &&
    ((cert_context->dwCertEncodingType & X509_ASN_ENCODING) != 0) &&
    cert_context->pbCertEncoded &&
    (cert_context->cbCertEncoded > 0);
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ bool (*Read_crt_func)(ਅਚਲ CERT_CONTEXT *ccert_context,
                              bool ਉਲਟ_ਕ੍ਰਮ, ਖਾਲੀ *arg);

ਸਥਿਰ ਖਾਲੀ traverse_cert_store(ਅਚਲ CERT_CONTEXT *context,
                                Read_crt_func func, ਖਾਲੀ *arg)
{
  ਅਚਲ CERT_CONTEXT *current_context = ਨਿਰਾ;
  bool should_continue = TRUE;
  bool ਪਹਿਲਾ = TRUE;
  bool ਉਲਟ_ਕ੍ਰਮ = FALSE;
  ਜਦੋਂ(should_continue &&
        (current_context = Cert_Enum_Certificates_ਵਿੱਚ_Store(
          context->hCertStore,
          current_context)) != ਨਿਰਾ) {
    /* Windows 11 22H2 OS Build 22621.674 or higher enumerates certificates in
       leaf-to-root order while all previous versions of Windows enumerate
       certificates in root-to-leaf order. Determine the order of enumeration
       by comparing SECPKG_ATTR_REMOTE_CERT_CONTEXT's pbCertContext with the
       first certificate's pbCertContext. */
    ਜੇ(ਪਹਿਲਾ && context->pbCertEncoded != current_context->pbCertEncoded)
      ਉਲਟ_ਕ੍ਰਮ = TRUE;
    should_continue = func(current_context, ਉਲਟ_ਕ੍ਰਮ, arg);
    ਪਹਿਲਾ = FALSE;
  }

  ਜੇ(current_context)
    CertFreeCertificateContext(current_context);
}

ਸਥਿਰ bool cert_counter_callback(ਅਚਲ CERT_CONTEXT *ccert_context,
                                  bool ਉਲਟ_ਕ੍ਰਮ, ਖਾਲੀ *certs_ਗਿਣਤੀ)
{
  (ਖਾਲੀ)ਉਲਟ_ਕ੍ਰਮ;
  ਜੇ(valid_cert_encoding(ccert_context))
    (*(ਪੂਰਨ *)certs_ਗਿਣਤੀ)++;
  ਜੇ(*(ਪੂਰਨ *)certs_ਗਿਣਤੀ > MAX_ALLOWED_CERT_AMOUNT)
    ਵਾਪਸ FALSE;
  ਵਾਪਸ TRUE;
}

ਬਣਤਰ Adder_ਦਲੀਲਾਂ {
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ;
  CURLcode ਨਤੀਜਾ;
  ਪੂਰਨ idx;
  ਪੂਰਨ certs_ਗਿਣਤੀ;
};

ਸਥਿਰ bool ਜੋੜੋ_cert_ਮੰਜ਼ਿਲ_certinfo(ਅਚਲ CERT_CONTEXT *ccert_context,
                                 bool ਉਲਟ_ਕ੍ਰਮ, ਖਾਲੀ *raw_arg)
{
  ਬਣਤਰ Adder_ਦਲੀਲਾਂ *ਦਲੀਲਾਂ = (ਬਣਤਰ Adder_ਦਲੀਲਾਂ *)raw_arg;
  ਦਲੀਲਾਂ->ਨਤੀਜਾ = CURLE_OK;
  ਜੇ(valid_cert_encoding(ccert_context)) {
    ਪੂਰਨ ਪਾਓ_ਸੂਚਕਾਂਕ = ਉਲਟ_ਕ੍ਰਮ ? (ਦਲੀਲਾਂ->certs_ਗਿਣਤੀ - 1) - ਦਲੀਲਾਂ->idx :
                       ਦਲੀਲਾਂ->idx;
    ਦਲੀਲਾਂ->ਨਤੀਜਾ = Curl_ਕੱਢੋ_certinfo(ਦਲੀਲਾਂ->ਅੰਕੜੇ, ਪਾਓ_ਸੂਚਕਾਂਕ,
                                         ccert_context->pbCertEncoded,
                                         ccert_context->pbCertEncoded +
                                         ccert_context->cbCertEncoded);
    ਦਲੀਲਾਂ->idx++;
  }
  ਵਾਪਸ ਦਲੀਲਾਂ->ਨਤੀਜਾ == CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ schannel_session_free(ਖਾਲੀ *sessionid)
{
  /* this is expected to be called under sessionid lock */
  ਬਣਤਰ Curl_schannel_cred *cred = sessionid;

  ਜੇ(cred) {
    cred->refcount--;
    ਜੇ(cred->refcount == 0) {
      Curl_pSecFn->FreeCredentialsHandle(&cred->cred_handle);
      curlx_free(cred->sni_hostname);
      ਜੇ(cred->client_cert_store) {
        CertCloseStore(cred->client_cert_store, 0);
        cred->client_cert_store = ਨਿਰਾ;
      }
      curlx_safefree(cred);
    }
  }
}

ਸਥਿਰ CURLcode schannel_connect_step3(ਬਣਤਰ Curl_cfilter *cf,
                                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  ਬਣਤਰ ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, ਅੰਕੜੇ);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  SECURITY_ਸਥਿਤੀ sspi_ਸਥਿਤੀ = SEC_E_OK;
  CERT_CONTEXT *ccert_context = ਨਿਰਾ;
  SecPkgContext_ApplicationProtocol alpn_ਨਤੀਜਾ;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);
  DEBUGASSERT(backend);

  DEBUGF(infof(ਅੰਕੜੇ, "schannel: SSL/TLS connection with %s port %d (step 3/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->ਪੋਰਟ));

  ਜੇ(!backend->cred)
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;

  /* check if the required context attributes are met */
  ਜੇ(backend->ret_flags != backend->req_flags) {
    ਜੇ(!(backend->ret_flags & ISC_RET_SEQUENCE_DETECT))
      failf(ਅੰਕੜੇ, "schannel: failed to setup sequence detection");
    ਜੇ(!(backend->ret_flags & ISC_RET_REPLAY_DETECT))
      failf(ਅੰਕੜੇ, "schannel: failed to setup replay detection");
    ਜੇ(!(backend->ret_flags & ISC_RET_CONFIDENTIALITY))
      failf(ਅੰਕੜੇ, "schannel: failed to setup confidentiality");
    ਜੇ(!(backend->ret_flags & ISC_RET_ALLOCATED_ਸਮ੍ਰਿਤੀ))
      failf(ਅੰਕੜੇ, "schannel: failed to setup memory allocation");
    ਜੇ(!(backend->ret_flags & ISC_RET_ਧਾਰਾ))
      failf(ਅੰਕੜੇ, "schannel: failed to setup stream orientation");
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
  }

  ਜੇ(backend->use_alpn) {
    sspi_ਸਥਿਤੀ =
      Curl_pSecFn->Query_Context_ਗੁਣ(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_APPLICATION_PROTOCOL,
                                          &alpn_ਨਤੀਜਾ);

    ਜੇ(sspi_ਸਥਿਤੀ != SEC_E_OK) {
      failf(ਅੰਕੜੇ, "schannel: failed to retrieve ALPN result");
      ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
    }

    ਜੇ(alpn_ਨਤੀਜਾ.Proto_Nego_ਸਥਿਤੀ ==
       Sec_Application_Protocol_Negotiation_ਸਥਿਤੀ_Success) {
      ਜੇ(backend->recv_renegotiating &&
         connssl->negotiated.alpn &&
         ਲੜੀ_ਤੁਲਨਾ(connssl->negotiated.alpn,
                 (ਅਚਲ ਅੱਖਰ *)alpn_ਨਤੀਜਾ.ProtocolId,
                 alpn_ਨਤੀਜਾ.Protocol_Id_ਮਾਪ)) {
        /* Renegotiation selected a different protocol now, we cannot
         * deal with this */
        failf(ਅੰਕੜੇ, "schannel: server selected an ALPN protocol too late");
        ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
      }
      Curl_alpn_ਸੈੱਟ_negotiated(cf, ਅੰਕੜੇ, connssl, alpn_ਨਤੀਜਾ.ProtocolId,
                               alpn_ਨਤੀਜਾ.Protocol_Id_ਮਾਪ);
    }
    ਨਹੀਂਤਾਂ {
      ਜੇ(!backend->recv_renegotiating)
        Curl_alpn_ਸੈੱਟ_negotiated(cf, ਅੰਕੜੇ, connssl, ਨਿਰਾ, 0);
    }
  }

  /* save the current session data for possible reuse */
  ਜੇ(Curl_ssl_scache_use(cf, ਅੰਕੜੇ)) {
    Curl_ssl_scache_ਤਾਲਾ(ਅੰਕੜੇ);
    /* Up ref count since call takes ownership */
    backend->cred->refcount++;
    ਨਤੀਜਾ = Curl_ssl_scache_ਜੋੜੋ_obj(cf, ਅੰਕੜੇ, connssl->peer.scache_ਕੁੰਜੀ,
                                     backend->cred, schannel_session_free);
    Curl_ssl_scache_unlock(ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਜੇ(ssl_config->certinfo) {
    ਪੂਰਨ certs_ਗਿਣਤੀ = 0;
    sspi_ਸਥਿਤੀ =
      Curl_pSecFn->Query_Context_ਗੁਣ(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                          &ccert_context);

    ਜੇ((sspi_ਸਥਿਤੀ != SEC_E_OK) || !ccert_context) {
      failf(ਅੰਕੜੇ, "schannel: failed to retrieve remote cert context");
      ਵਾਪਸ CURLE_PEER_FAILED_VERIFICATION;
    }

    traverse_cert_store(ccert_context, cert_counter_callback, &certs_ਗਿਣਤੀ);
    ਜੇ(certs_ਗਿਣਤੀ > MAX_ALLOWED_CERT_AMOUNT) {
      failf(ਅੰਕੜੇ, "%d certificates is more than allowed (%d)",
            certs_ਗਿਣਤੀ, MAX_ALLOWED_CERT_AMOUNT);
      CertFreeCertificateContext(ccert_context);
      ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
    }

    ਨਤੀਜਾ = Curl_ssl_init_certinfo(ਅੰਕੜੇ, certs_ਗਿਣਤੀ);
    ਜੇ(!ਨਤੀਜਾ) {
      ਬਣਤਰ Adder_ਦਲੀਲਾਂ ਦਲੀਲਾਂ;
      ਦਲੀਲਾਂ.ਅੰਕੜੇ = ਅੰਕੜੇ;
      ਦਲੀਲਾਂ.idx = 0;
      ਦਲੀਲਾਂ.certs_ਗਿਣਤੀ = certs_ਗਿਣਤੀ;
      ਦਲੀਲਾਂ.ਨਤੀਜਾ = CURLE_OK;
      traverse_cert_store(ccert_context, ਜੋੜੋ_cert_ਮੰਜ਼ਿਲ_certinfo, &ਦਲੀਲਾਂ);
      ਨਤੀਜਾ = ਦਲੀਲਾਂ.ਨਤੀਜਾ;
    }
    CertFreeCertificateContext(ccert_context);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  connssl->connecting_state = ssl_connect_ਪੂਰਾ;

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode schannel_connect(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 bool *ਪੂਰਾ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  CURLcode ਨਤੀਜਾ;

  /* check if the connection has already been established */
  ਜੇ(ssl_connection_complete == connssl->state) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  *ਪੂਰਾ = FALSE;

  ਜੇ(connssl->connecting_state == ssl_connect_1) {
    ਨਤੀਜਾ = schannel_connect_step1(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਜੇ(connssl->connecting_state == ssl_connect_2) {
    ਨਤੀਜਾ = schannel_connect_step2(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਜੇ(connssl->connecting_state == ssl_connect_3) {
    ਨਤੀਜਾ = schannel_connect_step3(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਜੇ(connssl->connecting_state == ssl_connect_ਪੂਰਾ) {
    ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *backend =
      (ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
    DEBUGASSERT(backend);

    ਜੇ(Curl_pSecFn->Query_Context_ਗੁਣ(
         &backend->ctxt->ctxt_handle,
         SECPKG_ATTR_ਧਾਰਾ_SIZES,
         &backend->ਧਾਰਾ_sizes)) {
      failf(ਅੰਕੜੇ, "schannel: failed getting stream sizes");
      ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
    }

    connssl->state = ssl_connection_complete;

    /* When SSPI is used in combination with Schannel
     * we need the Schannel context to create the Schannel
     * binding to pass the IIS extended protection checks.
     * Available on Windows 7 or later.
     */
    cf->conn->sslContext = &backend->ctxt->ctxt_handle;

    *ਪੂਰਾ = TRUE;
  }

  ਵਾਪਸ CURLE_OK;
}

ਗਿਣਤੀ schannel_renegotiate_caller_t {
  SCH_RENEG_CALLER_IS_RECV,
  SCH_RENEG_CALLER_IS_SEND
};

/* The maximum time we allow for Schannel renegotiation which may in some
   rare cases block either due to libcurl (waiting on the socket) or Windows
   (waiting on an interactive security prompt). Note Schannel "renegotiation"
   is not necessarily literal TLS renegotiation, but means DecryptMessage
   returned SEC_I_RENEGOTIATE which means at least the security context needs
   to be re-established. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_RENEG_ਖੰਡ_ਸਮਾਂ (60 * 1000) /* 60 seconds in milliseconds */

/* This function renegotiates the connection due to a server request received
   by schannel_recv. This function returns CURLE_AGAIN if the renegotiation is
   incomplete. In that case, we remain in the renegotiation (connecting) stage
   and future calls to schannel_recv and schannel_send must call this function
   first to complete the renegotiation. */
ਸਥਿਰ CURLcode schannel_recv_renegotiate(
  ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
  ਗਿਣਤੀ schannel_renegotiate_caller_t caller)
{
  CURLcode ਨਤੀਜਾ;
  curl_socket_t sockfd;
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  ਬਣਤਰ schannel_renegotiate_state *rs = &backend->renegotiate_state;

  ਜੇ(!backend || !backend->recv_renegotiating) {
    failf(ਅੰਕੜੇ, "schannel: unexpected call to schannel_recv_renegotiate");
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
  }
  DEBUGASSERT(caller <= SCH_RENEG_CALLER_IS_SEND);
  ਜੇ(caller == SCH_RENEG_CALLER_IS_RECV)
    SCH_DEV(infof(ਅੰਕੜੇ, "schannel: renegotiation caller is schannel_recv"));
  ਨਹੀਂਤਾਂ
    SCH_DEV(infof(ਅੰਕੜੇ, "schannel: renegotiation caller is schannel_send"));

  sockfd = Curl_conn_cf_get_socket(cf, ਅੰਕੜੇ);

  ਜੇ(sockfd == CURL_SOCKET_BAD) {
    failf(ਅੰਕੜੇ, "schannel: renegotiation missing socket");
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
  }

  ਜੇ(!rs->started) { /* new renegotiation */
    infof(ਅੰਕੜੇ, "schannel: renegotiating SSL/TLS connection");
    DEBUGASSERT(connssl->state == ssl_connection_complete);
    DEBUGASSERT(connssl->connecting_state == ssl_connect_ਪੂਰਾ);
    connssl->state = ssl_connection_negotiating;
    connssl->connecting_state = ssl_connect_2;
    memset(rs, 0, ਆਕਾਰ(*rs));
    rs->io_need = CURL_SSL_IO_NEED_SEND;
    rs->ਸ਼ੁਰੂ_ਸਮਾਂ = *Curl_pgrs_now(ਅੰਕੜੇ);
    rs->started = TRUE;
  }

  ਲਈ(;;) {
    bool ਖੰਡ_read, ਖੰਡ_write, blocking, ਪੂਰਾ;
    curl_socket_t readfd, writefd;
    timediff_t elapsed;

    elapsed = curlx_ptimediff_ms(Curl_pgrs_now(ਅੰਕੜੇ), &rs->ਸ਼ੁਰੂ_ਸਮਾਂ);
    ਜੇ(elapsed >= MAX_RENEG_ਖੰਡ_ਸਮਾਂ) {
      failf(ਅੰਕੜੇ, "schannel: renegotiation timeout");
      ਨਤੀਜਾ = CURLE_SSL_CONNECT_ERROR;
      ਰੋਕੋ;
    }

    /* the current io_need state may have been overwritten since the last time
       this function was called. restore the io_need state needed to continue
       the renegotiation. */

    connssl->io_need = rs->io_need;

    ਨਤੀਜਾ = schannel_connect(cf, ਅੰਕੜੇ, &ਪੂਰਾ);

    rs->io_need = connssl->io_need;

    ਜੇ(!ਨਤੀਜਾ && !ਪੂਰਾ)
      ਨਤੀਜਾ = CURLE_AGAIN;

    ਜੇ(ਨਤੀਜਾ != CURLE_AGAIN)
      ਰੋਕੋ;

    readfd = (rs->io_need & CURL_SSL_IO_NEED_RECV) ? sockfd : CURL_SOCKET_BAD;
    writefd = (rs->io_need & CURL_SSL_IO_NEED_SEND) ? sockfd : CURL_SOCKET_BAD;

    ਜੇ(readfd == CURL_SOCKET_BAD && writefd == CURL_SOCKET_BAD)
      ਜਾਰੀ;

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

    ਖੰਡ_read = (caller == SCH_RENEG_CALLER_IS_SEND) ? TRUE : FALSE;
    ਖੰਡ_write = (caller == SCH_RENEG_CALLER_IS_RECV) ? TRUE : FALSE;

    blocking = (ਖੰਡ_read && (readfd != CURL_SOCKET_BAD)) ||
               (ਖੰਡ_write && (writefd != CURL_SOCKET_BAD));

    SCH_DEV_SHOWBOOL(ਖੰਡ_read);
    SCH_DEV_SHOWBOOL(ਖੰਡ_write);
    SCH_DEV_SHOWBOOL(blocking);

    ਲਈ(;;) {
      ਪੂਰਨ what;
      timediff_t timeout_ms, ਬਚਿਆ;

      ਨਤੀਜਾ = Curl_pgrsUpdate(ਅੰਕੜੇ);
      ਜੇ(ਨਤੀਜਾ)
        ਰੋਕੋ;

      elapsed = curlx_ptimediff_ms(Curl_pgrs_now(ਅੰਕੜੇ), &rs->ਸ਼ੁਰੂ_ਸਮਾਂ);
      ਜੇ(elapsed >= MAX_RENEG_ਖੰਡ_ਸਮਾਂ) {
        failf(ਅੰਕੜੇ, "schannel: renegotiation timeout");
        ਨਤੀਜਾ = CURLE_SSL_CONNECT_ERROR;
        ਰੋਕੋ;
      }
      ਬਚਿਆ = MAX_RENEG_ਖੰਡ_ਸਮਾਂ - elapsed;

      ਜੇ(blocking) {
        timeout_ms = Curl_timeleft_ms(ਅੰਕੜੇ);

        ਜੇ(timeout_ms < 0) {
          ਨਤੀਜਾ = CURLE_OPERATION_TIMEDOUT;
          ਰੋਕੋ;
        }

        /* the blocking is in intervals so that the progress function can be
           called every second */
        ਜੇ(!timeout_ms || timeout_ms > 1000)
          timeout_ms = 1000;

        ਜੇ(timeout_ms > ਬਚਿਆ)
          timeout_ms = ਬਚਿਆ;
      }
      ਨਹੀਂਤਾਂ
        timeout_ms = 0;

      SCH_DEV(infof(ਅੰਕੜੇ, "schannel: renegotiation wait until socket is"
                    "%s%s for up to %" FMT_TIMEDIFF_T " ms",
                    ((readfd != CURL_SOCKET_BAD) ? " readable" : ""),
                    ((writefd != CURL_SOCKET_BAD) ? " writable" : ""),
                    timeout_ms));

      what = Curl_socket_check(readfd, CURL_SOCKET_BAD, writefd, timeout_ms);

      ਜੇ(what > 0 && (what & (CURL_CSELECT_ਵਿੱਚ | CURL_CSELECT_ਬਾਹਰਲਾ))) {
        SCH_DEV(infof(ਅੰਕੜੇ, "schannel: renegotiation socket %s%s",
                      ((what & CURL_CSELECT_ਵਿੱਚ) ? "CURL_CSELECT_IN " : ""),
                      ((what & CURL_CSELECT_ਬਾਹਰਲਾ) ? "CURL_CSELECT_OUT " : "")));
        ਨਤੀਜਾ = CURLE_AGAIN;
        ਰੋਕੋ;
      }
      ਨਹੀਂਤਾਂ ਜੇ(!what) {
        SCH_DEV(infof(ਅੰਕੜੇ, "schannel: renegotiation socket timeout"));
        ਜੇ(blocking)
          ਜਾਰੀ;
        ਨਹੀਂਤਾਂ
          ਵਾਪਸ CURLE_AGAIN;
      }

      failf(ਅੰਕੜੇ, "schannel: socket error during renegotiation");
      ਨਤੀਜਾ = CURLE_SSL_CONNECT_ERROR;
      ਰੋਕੋ;
    }
    ਜੇ(ਨਤੀਜਾ != CURLE_AGAIN)
      ਰੋਕੋ;
  }

  DEBUGASSERT(ਨਤੀਜਾ != CURLE_AGAIN);

  rs->started = FALSE;
  backend->recv_renegotiating = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  ਜੇ(ਨਤੀਜਾ)
    failf(ਅੰਕੜੇ, "schannel: renegotiation failed");
  ਨਹੀਂਤਾਂ
    infof(ਅੰਕੜੇ, "schannel: SSL/TLS connection renegotiated");

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode schannel_send(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਅਚਲ ਖਾਲੀ *buf, size_t len, size_t *pnwritten)
{
  size_t ਅੰਕੜੇ_len = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ptr = ਨਿਰਾ;
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  Sec_ਬਫਰ outbuf[4];
  Sec_ਬਫਰ_Desc outbuf_desc;
  SECURITY_ਸਥਿਤੀ sspi_ਸਥਿਤੀ = SEC_E_OK;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *)connssl->backend;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  ਜੇ(backend->recv_renegotiating) {
    ਨਤੀਜਾ = schannel_recv_renegotiate(cf, ਅੰਕੜੇ, SCH_RENEG_CALLER_IS_SEND);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  /* check if the buffer is longer than the maximum message length */
  ਜੇ(len > backend->ਧਾਰਾ_sizes.cb_ਵੱਧ_ਤੋਂ_ਵੱਧ_ਸੁਨੇਹਾ) {
    len = backend->ਧਾਰਾ_sizes.cb_ਵੱਧ_ਤੋਂ_ਵੱਧ_ਸੁਨੇਹਾ;
  }

  /* calculate the complete message length and prepare the send buffer */
  ਅੰਕੜੇ_len = backend->ਧਾਰਾ_sizes.cb_ਸਿਰਲੇਖ + len +
    backend->ਧਾਰਾ_sizes.cbTrailer;
  ਜੇ(ਅੰਕੜੇ_len > backend->send_ਬਫਰ_len) {
    ptr = curlx_realloc(backend->send_ਬਫਰ, ਅੰਕੜੇ_len);
    ਜੇ(!ptr)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    backend->send_ਬਫਰ = ptr;
    backend->send_ਬਫਰ_len = ਅੰਕੜੇ_len;
  }
  ਨਹੀਂਤਾਂ
    ptr = backend->send_ਬਫਰ;

  /* setup output buffers (header, data, trailer, empty) */
  Init_Sec_ਬਫਰ(&outbuf[0], SECBUFFER_ਧਾਰਾ_ਸਿਰਲੇਖ,
                ptr, backend->ਧਾਰਾ_sizes.cb_ਸਿਰਲੇਖ);
  Init_Sec_ਬਫਰ(&outbuf[1], SECBUFFER_ਅੰਕੜੇ,
                ptr + backend->ਧਾਰਾ_sizes.cb_ਸਿਰਲੇਖ, curlx_uztoul(len));
  Init_Sec_ਬਫਰ(&outbuf[2], SECBUFFER_ਧਾਰਾ_TRAILER,
                ptr + backend->ਧਾਰਾ_sizes.cb_ਸਿਰਲੇਖ + len,
                backend->ਧਾਰਾ_sizes.cbTrailer);
  Init_Sec_ਬਫਰ(&outbuf[3], SECBUFFER_ਖਾਲੀ_ਥਾਵਾਂ, ਨਿਰਾ, 0);
  Init_Sec_ਬਫਰ_Desc(&outbuf_desc, outbuf, 4);

  /* copy data into output buffer */
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(outbuf[1].pv_ਬਫਰ, buf, len);

  /* https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-encryptmessage */
  sspi_ਸਥਿਤੀ = Curl_pSecFn->Encrypt_ਸੁਨੇਹਾ(&backend->ctxt->ctxt_handle, 0,
                                            &outbuf_desc, 0);

  /* check if the message was encrypted */
  ਜੇ(sspi_ਸਥਿਤੀ == SEC_E_OK) {

    /* send the encrypted message including header, data and trailer */
    len = outbuf[0].cb_ਬਫਰ + outbuf[1].cb_ਬਫਰ + outbuf[2].cb_ਬਫਰ;

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
    ਜਦੋਂ(len > *pnwritten) {
      size_t this_write = 0;
      ਪੂਰਨ what;
      timediff_t timeout_ms = Curl_timeleft_ms(ਅੰਕੜੇ);
      ਜੇ(timeout_ms < 0) {
        /* we already got the timeout */
        failf(ਅੰਕੜੇ, "schannel: timed out sending data (bytes sent: %zu)",
              *pnwritten);
        ਨਤੀਜਾ = CURLE_OPERATION_TIMEDOUT;
        ਰੋਕੋ;
      }
      ਨਹੀਂਤਾਂ ਜੇ(!timeout_ms)
        timeout_ms = TIMEDIFF_T_MAX;
      what = SOCKET_WRITABLE(Curl_conn_cf_get_socket(cf, ਅੰਕੜੇ), timeout_ms);
      ਜੇ(what < 0) {
        /* fatal error */
        failf(ਅੰਕੜੇ, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        ਨਤੀਜਾ = CURLE_SEND_ERROR;
        ਰੋਕੋ;
      }
      ਨਹੀਂਤਾਂ ਜੇ(what == 0) {
        failf(ਅੰਕੜੇ, "schannel: timed out sending data (bytes sent: %zu)",
              *pnwritten);
        ਨਤੀਜਾ = CURLE_OPERATION_TIMEDOUT;
        ਰੋਕੋ;
      }
      /* socket is writable */

       ਨਤੀਜਾ = Curl_conn_cf_send(cf->ਅਗਲਾ, ਅੰਕੜੇ,
                                  (ਅਚਲ uint8_t *)ptr + *pnwritten,
                                  len - *pnwritten,
                                  FALSE, &this_write);
      ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
        ਜਾਰੀ;
      ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ) {
        ਰੋਕੋ;
      }

      *pnwritten += this_write;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(sspi_ਸਥਿਤੀ == SEC_E_INSUFFICIENT_ਸਮ੍ਰਿਤੀ) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
  ਨਹੀਂਤਾਂ {
    ਨਤੀਜਾ = CURLE_SEND_ERROR;
  }

  ਜੇ(len == *pnwritten)
    /* Encrypted message including header, data and trailer entirely sent.
       The return value is the number of unencrypted bytes that were sent. */
    *pnwritten = outbuf[1].cb_ਬਫਰ;

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode schannel_recv(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਅੱਖਰ *buf, size_t len, size_t *pnread)
{
  size_t ਮਾਪ = 0;
  size_t nread = 0;
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  Sec_ਬਫਰ inbuf[4];
  Sec_ਬਫਰ_Desc inbuf_desc;
  SECURITY_ਸਥਿਤੀ sspi_ਸਥਿਤੀ = SEC_E_OK;
  ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(backend);
  *pnread = 0;

  ਜੇ(backend->recv_renegotiating) {
    ਨਤੀਜਾ = schannel_recv_renegotiate(cf, ਅੰਕੜੇ, SCH_RENEG_CALLER_IS_RECV);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
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

  SCH_DEV(infof(ਅੰਕੜੇ, "schannel: client wants to read %zu bytes", len));

  ਜੇ(len && len <= backend->decdata.ਵਿੱਥ) {
    SCH_DEV(infof(ਅੰਕੜੇ,
                  "schannel: enough decrypted data is already available"));
    ਜਾਓ cleanup;
  }
  ਨਹੀਂਤਾਂ ਜੇ(backend->recv_unrecoverable_err) {
    ਨਤੀਜਾ = backend->recv_unrecoverable_err;
    infof(ਅੰਕੜੇ, "schannel: an unrecoverable error occurred in a prior call");
    ਜਾਓ cleanup;
  }
  ਨਹੀਂਤਾਂ ਜੇ(backend->recv_sspi_close_notify) {
    /* once a server has indicated shutdown there is no more encrypted data */
    infof(ਅੰਕੜੇ, "schannel: server indicated shutdown in a prior call");
    ਜਾਓ cleanup;
  }
  /* it is debatable what to return when !len. Regardless we cannot return
     immediately because there may be data to decrypt (in the case we want to
     decrypt all encrypted cached data) so handle !len later in cleanup. */
  ਨਹੀਂਤਾਂ ਜੇ(len && !backend->recv_connection_closed) {
    /* the encrypted buffer must be large enough to hold all the bytes
       requested and some TLS record overhead. 'len' is a buffer size, so this
       integer math cannot overflow. */
    ਅਚਲ size_t min_encdata_ਲੰਮਾਈ = len + CURL_SCHANNEL_ਬਫਰ_FREE_ਮਾਪ;

    /* make sure encrypt buffer fits the requested amount of data */
    ਨਤੀਜਾ = ensure_encoding_ਮਾਪ(ਅੰਕੜੇ, &backend->encdata, min_encdata_ਲੰਮਾਈ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ cleanup;

    SCH_DEV(infof(ਅੰਕੜੇ,
                  "schannel: encrypted data buffer: offset %zu length %zu",
                  backend->encdata.ਵਿੱਥ, backend->encdata.ਲੰਮਾਈ));

    /* read encrypted data from socket */
    ਨਤੀਜਾ = Curl_conn_cf_recv(cf->ਅਗਲਾ, ਅੰਕੜੇ,
                               (ਅੱਖਰ *)(backend->encdata.ਬਫਰ +
                                        backend->encdata.ਵਿੱਥ),
                               backend->encdata.ਲੰਮਾਈ -
                               backend->encdata.ਵਿੱਥ,
                               &nread);
    ਜੇ(ਨਤੀਜਾ) {
      ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
        SCH_DEV(infof(ਅੰਕੜੇ, "schannel: recv returned CURLE_AGAIN"));
      ਨਹੀਂਤਾਂ {
        infof(ਅੰਕੜੇ, "schannel: recv returned error %d", (ਪੂਰਨ)ਨਤੀਜਾ);
        backend->recv_unrecoverable_err = ਨਤੀਜਾ;
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ(nread == 0) {
      backend->recv_connection_closed = TRUE;
      DEBUGF(infof(ਅੰਕੜੇ, "schannel: server closed the connection"));
    }
    ਨਹੀਂਤਾਂ {
      backend->encdata.ਵਿੱਥ += nread;
      backend->encdata_is_incomplete = FALSE;
      SCH_DEV(infof(ਅੰਕੜੇ, "schannel: encrypted data got %zu", nread));
    }
  }

  SCH_DEV(infof(ਅੰਕੜੇ, "schannel: encrypted data buffer: offset %zu length %zu",
                backend->encdata.ਵਿੱਥ, backend->encdata.ਲੰਮਾਈ));

  /* decrypt loop */
  ਜਦੋਂ(backend->encdata.ਵਿੱਥ > 0 && sspi_ਸਥਿਤੀ == SEC_E_OK &&
        (!len || backend->decdata.ਵਿੱਥ < len ||
         backend->recv_connection_closed)) {
    /* prepare data buffer for DecryptMessage call */
    Init_Sec_ਬਫਰ(&inbuf[0], SECBUFFER_ਅੰਕੜੇ, backend->encdata.ਬਫਰ,
                  curlx_uztoul(backend->encdata.ਵਿੱਥ));

    /* we need 3 more empty input buffers for possible output */
    Init_Sec_ਬਫਰ(&inbuf[1], SECBUFFER_ਖਾਲੀ_ਥਾਵਾਂ, ਨਿਰਾ, 0);
    Init_Sec_ਬਫਰ(&inbuf[2], SECBUFFER_ਖਾਲੀ_ਥਾਵਾਂ, ਨਿਰਾ, 0);
    Init_Sec_ਬਫਰ(&inbuf[3], SECBUFFER_ਖਾਲੀ_ਥਾਵਾਂ, ਨਿਰਾ, 0);
    Init_Sec_ਬਫਰ_Desc(&inbuf_desc, inbuf, 4);

    /* https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-decryptmessage
     */
    sspi_ਸਥਿਤੀ = Curl_pSecFn->Decrypt_ਸੁਨੇਹਾ(&backend->ctxt->ctxt_handle,
                                              &inbuf_desc, 0, ਨਿਰਾ);

    /* check if everything went fine (server may want to renegotiate
       or shutdown the connection context) */
    ਜੇ(sspi_ਸਥਿਤੀ == SEC_E_OK || sspi_ਸਥਿਤੀ == SEC_I_RENEGOTIATE ||
       sspi_ਸਥਿਤੀ == SEC_I_CONTEXT_EXPIRED) {
      /* check for successfully decrypted data, even before actual
         renegotiation or shutdown of the connection context */
      ਜੇ(inbuf[1].ਬਫਰ_Type == SECBUFFER_ਅੰਕੜੇ) {
        SCH_DEV(infof(ਅੰਕੜੇ, "schannel: decrypted data length: %lu",
                      inbuf[1].cb_ਬਫਰ));

        /* ensure the decode buffer fits the received amount of data */
        ਨਤੀਜਾ = ensure_decoding_ਮਾਪ(ਅੰਕੜੇ, &backend->decdata,
                                      inbuf[1].cb_ਬਫਰ, len);
        ਜੇ(ਨਤੀਜਾ)
          ਜਾਓ cleanup;

        /* copy decrypted data to internal buffer */
        ਮਾਪ = inbuf[1].cb_ਬਫਰ;
        ਜੇ(ਮਾਪ) {
          ਸਮ੍ਰਿਤੀ_ਨਕਲ(backend->decdata.ਬਫਰ + backend->decdata.ਵਿੱਥ,
                 inbuf[1].pv_ਬਫਰ, ਮਾਪ);
          backend->decdata.ਵਿੱਥ += ਮਾਪ;
        }

        SCH_DEV(infof(ਅੰਕੜੇ, "schannel: decrypted data added: %zu", ਮਾਪ));
        SCH_DEV(infof(ਅੰਕੜੇ,
                      "schannel: decrypted cached: offset %zu length %zu",
                      backend->decdata.ਵਿੱਥ, backend->decdata.ਲੰਮਾਈ));
      }

      /* check for remaining encrypted data */
      ਜੇ(inbuf[3].ਬਫਰ_Type == SECBUFFER_EXTRA && inbuf[3].cb_ਬਫਰ > 0) {
        SCH_DEV(infof(ਅੰਕੜੇ, "schannel: encrypted data length: %lu",
                      inbuf[3].cb_ਬਫਰ));

        /* check if the remaining data is less than the total amount
         * and therefore begins after the already processed data
         */
        ਜੇ(backend->encdata.ਵਿੱਥ > inbuf[3].cb_ਬਫਰ) {
          /* move remaining encrypted data forward to the beginning of
ਬਫਰ*/
          memmove(backend->encdata.ਬਫਰ,
                  (backend->encdata.ਬਫਰ + backend->encdata.ਵਿੱਥ) -
                  inbuf[3].cb_ਬਫਰ, inbuf[3].cb_ਬਫਰ);
          backend->encdata.ਵਿੱਥ = inbuf[3].cb_ਬਫਰ;
        }

        SCH_DEV(infof(ਅੰਕੜੇ,
                      "schannel: encrypted cached: offset %zu length %zu",
                      backend->encdata.ਵਿੱਥ, backend->encdata.ਲੰਮਾਈ));
      }
      ਨਹੀਂਤਾਂ {
        /* reset encrypted buffer offset, because there is no data remaining */
        backend->encdata.ਵਿੱਥ = 0;
      }

      /* check if server wants to renegotiate the connection context */
      ਜੇ(sspi_ਸਥਿਤੀ == SEC_I_RENEGOTIATE) {
        infof(ਅੰਕੜੇ, "schannel: remote party requests renegotiation");
        ਜੇ(ਨਤੀਜਾ && ਨਤੀਜਾ != CURLE_AGAIN) {
          infof(ਅੰਕੜੇ, "schannel: cannot renegotiate, an error is pending");
          ਜਾਓ cleanup;
        }

        backend->recv_renegotiating = TRUE;
        ਨਤੀਜਾ = schannel_recv_renegotiate(cf, ਅੰਕੜੇ, SCH_RENEG_CALLER_IS_RECV);
        ਜੇ(ਨਤੀਜਾ)
          ਜਾਓ cleanup;

        /* now retry receiving data */
        sspi_ਸਥਿਤੀ = SEC_E_OK;
        ਜਾਰੀ;
      }
      /* check if the server closed the connection */
      ਨਹੀਂਤਾਂ ਜੇ(sspi_ਸਥਿਤੀ == SEC_I_CONTEXT_EXPIRED) {
        /* In Windows 2000 SEC_I_CONTEXT_EXPIRED (close_notify) is not
           returned so we have to work around that in cleanup. */
        backend->recv_sspi_close_notify = TRUE;
        ਜੇ(!backend->recv_connection_closed)
          backend->recv_connection_closed = TRUE;
        /* We received the close notify fine, any error we got
         * from the lower filters afterwards (e.g. the socket), is not
         * an error on the TLS data stream. That one ended here. */
        ਜੇ(ਨਤੀਜਾ == CURLE_RECV_ERROR)
          ਨਤੀਜਾ = CURLE_OK;
        infof(ਅੰਕੜੇ,
              "schannel: server close notification received (close_notify)");
        ਜਾਓ cleanup;
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ(sspi_ਸਥਿਤੀ == SEC_E_INCOMPLETE_ਸੁਨੇਹਾ) {
      backend->encdata_is_incomplete = TRUE;
      ਜੇ(!ਨਤੀਜਾ)
        ਨਤੀਜਾ = CURLE_AGAIN;
      SCH_DEV(infof(ਅੰਕੜੇ, "schannel: failed to decrypt data, need more data"));
      ਜਾਓ cleanup;
    }
    ਨਹੀਂਤਾਂ {
      ਅੱਖਰ ਬਫਰ[STRERROR_LEN];
      failf(ਅੰਕੜੇ, "schannel: failed to read data from server: %s",
            Curl_sspi_strerror(sspi_ਸਥਿਤੀ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
      ਨਤੀਜਾ = CURLE_RECV_ERROR;
      ਜਾਓ cleanup;
    }
  }

  SCH_DEV(infof(ਅੰਕੜੇ, "schannel: encrypted data buffer: offset %zu length %zu",
                backend->encdata.ਵਿੱਥ, backend->encdata.ਲੰਮਾਈ));

  SCH_DEV(infof(ਅੰਕੜੇ, "schannel: decrypted data buffer: offset %zu length %zu",
                backend->decdata.ਵਿੱਥ, backend->decdata.ਲੰਮਾਈ));

cleanup:
  /* Warning- there is no guarantee the encdata state is valid at this point */
  SCH_DEV(infof(ਅੰਕੜੇ, "schannel: schannel_recv cleanup"));

  /* Error if the connection has closed without a close_notify.

     The behavior here is a matter of debate. We do not want to be vulnerable
     to a truncation attack however there is some browser precedent for
     ignoring the close_notify for compatibility reasons. */
  ਜੇ(len && !backend->decdata.ਵਿੱਥ && backend->recv_connection_closed &&
     !backend->recv_sspi_close_notify) {
    ਨਤੀਜਾ = CURLE_RECV_ERROR;
    failf(ਅੰਕੜੇ, "schannel: server closed abruptly (missing close_notify)");
  }

  /* Any error other than CURLE_AGAIN is an unrecoverable error. */
  ਜੇ(ਨਤੀਜਾ && ਨਤੀਜਾ != CURLE_AGAIN)
    backend->recv_unrecoverable_err = ਨਤੀਜਾ;

  ਮਾਪ = len < backend->decdata.ਵਿੱਥ ? len : backend->decdata.ਵਿੱਥ;
  ਜੇ(ਮਾਪ) {
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf, backend->decdata.ਬਫਰ, ਮਾਪ);
    memmove(backend->decdata.ਬਫਰ, backend->decdata.ਬਫਰ + ਮਾਪ,
            backend->decdata.ਵਿੱਥ - ਮਾਪ);
    backend->decdata.ਵਿੱਥ -= ਮਾਪ;
    SCH_DEV(infof(ਅੰਕੜੇ, "schannel: decrypted data returned %zu", ਮਾਪ));
    SCH_DEV(infof(ਅੰਕੜੇ,
                  "schannel: decrypted data buffer: offset %zu length %zu",
                  backend->decdata.ਵਿੱਥ, backend->decdata.ਲੰਮਾਈ));
    *pnread = ਮਾਪ;
    ਵਾਪਸ CURLE_OK;
  }

  ਜੇ(!ਨਤੀਜਾ && !backend->recv_connection_closed)
    ਨਤੀਜਾ = CURLE_AGAIN;

  /* it is debatable what to return when !len. We could return whatever error
     we got from decryption but instead we override here so the return is
     consistent. */
  ਜੇ(!len)
    ਵਾਪਸ CURLE_OK;

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ bool schannel_ਅੰਕੜੇ_pending(ਬਣਤਰ Curl_cfilter *cf,
                                  ਅਚਲ ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਅਚਲ ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *)connssl->backend;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  DEBUGASSERT(backend);

  ਜੇ(backend->ctxt) /* SSL/TLS is in use */
    ਵਾਪਸ backend->decdata.ਵਿੱਥ > 0 ||
           (backend->encdata.ਵਿੱਥ > 0 && !backend->encdata_is_incomplete) ||
           backend->recv_connection_closed ||
           backend->recv_sspi_close_notify ||
           backend->recv_unrecoverable_err;
  ਨਹੀਂਤਾਂ
    ਵਾਪਸ FALSE;
}

/* shut down the SSL connection and clean up related memory.
   this function can be called multiple times on the same connection including
   if the SSL connection failed (eg connection made but failed handshake). */
ਸਥਿਰ CURLcode schannel_shutdown(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  bool send_shutdown, bool *ਪੂਰਾ)
{
  /* See https://learn.microsoft.com/windows/win32/secauthn/shutting-down-an-schannel-connection
   * Shutting Down an Schannel Connection
   */
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(cf->shutdown) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(backend);

  /* Not supported in schannel */
  (ਖਾਲੀ)send_shutdown;

  *ਪੂਰਾ = FALSE;
  ਜੇ(backend->ctxt) {
    infof(ਅੰਕੜੇ, "schannel: shutting down SSL/TLS connection with %s port %d",
          connssl->peer.origin->hostname, connssl->peer.origin->ਪੋਰਟ);
  }

  ਜੇ(!backend->ctxt || cf->shutdown) {
    *ਪੂਰਾ = TRUE;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(backend->cred && backend->ctxt && !backend->ਭੇਜਿਆ_shutdown &&
     !backend->shutdown_generated) {
    Sec_ਬਫਰ_Desc BuffDesc;
    Sec_ਬਫਰ ਬਫਰ_2;
    SECURITY_ਸਥਿਤੀ sspi_ਸਥਿਤੀ;
    Sec_ਬਫਰ outbuf;
    Sec_ਬਫਰ_Desc outbuf_desc;
    DWORD dwshut = SCHANNEL_SHUTDOWN;

    Init_Sec_ਬਫਰ(&ਬਫਰ_2, SECBUFFER_TOKEN, &dwshut, ਆਕਾਰ(dwshut));
    Init_Sec_ਬਫਰ_Desc(&BuffDesc, &ਬਫਰ_2, 1);

    sspi_ਸਥਿਤੀ = Curl_pSecFn->ApplyControlToken(&backend->ctxt->ctxt_handle,
                                                 &BuffDesc);

    ਜੇ(sspi_ਸਥਿਤੀ != SEC_E_OK) {
      ਅੱਖਰ ਬਫਰ[STRERROR_LEN];
      failf(ਅੰਕੜੇ, "schannel: ApplyControlToken failure: %s",
            Curl_sspi_strerror(sspi_ਸਥਿਤੀ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    /* setup output buffer */
    Init_Sec_ਬਫਰ(&outbuf, SECBUFFER_ਖਾਲੀ_ਥਾਵਾਂ, ਨਿਰਾ, 0);
    Init_Sec_ਬਫਰ_Desc(&outbuf_desc, &outbuf, 1);

    sspi_ਸਥਿਤੀ = Curl_pSecFn->InitializeSecurityContext(
      &backend->cred->cred_handle,
      &backend->ctxt->ctxt_handle,
      backend->cred->sni_hostname,
      backend->req_flags,
      0,
      0,
      ਨਿਰਾ,
      0,
      &backend->ctxt->ctxt_handle,
      &outbuf_desc,
      &backend->ret_flags, ਨਿਰਾ);

    ਜੇ((sspi_ਸਥਿਤੀ == SEC_E_OK) || (sspi_ਸਥਿਤੀ == SEC_I_CONTEXT_EXPIRED)) {
      backend->shutdown_token = outbuf;
      backend->shutdown_generated = TRUE;
    }
    ਨਹੀਂਤਾਂ {
      ਅੱਖਰ ਬਫਰ[STRERROR_LEN];
      ਜੇ(outbuf.pv_ਬਫਰ)
        Curl_pSecFn->Free_Context_ਬਫਰ(outbuf.pv_ਬਫਰ);
      failf(ਅੰਕੜੇ, "schannel: generating close msg failed: %s",
            Curl_sspi_strerror(sspi_ਸਥਿਤੀ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  ਜੇ(backend->shutdown_generated && !backend->ਭੇਜਿਆ_shutdown) {
    size_t written = 0;
    size_t ਬਚਿਆ;

    DEBUGASSERT(backend->shutdown_ਵਿੱਥ <= backend->shutdown_token.cb_ਬਫਰ);
    ਬਚਿਆ = backend->shutdown_token.cb_ਬਫਰ - backend->shutdown_ਵਿੱਥ;

    ਜੇ(ਬਚਿਆ) {
      ਨਤੀਜਾ = Curl_conn_cf_send(cf->ਅਗਲਾ, ਅੰਕੜੇ,
                                 ((uint8_t *)backend->shutdown_token.pv_ਬਫਰ +
                                  backend->shutdown_ਵਿੱਥ),
                                 ਬਚਿਆ, FALSE, &written);
      ਜੇ(!ਨਤੀਜਾ)
        backend->shutdown_ਵਿੱਥ += written;
      ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN || (!ਨਤੀਜਾ && written < ਬਚਿਆ)) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        ਨਤੀਜਾ = CURLE_OK;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
    ਨਹੀਂਤਾਂ /* no shutdown message to send? */
      ਨਤੀਜਾ = CURLE_OK;
    ਜੇ(ਨਤੀਜਾ) {
      ਜੇ(backend->recv_connection_closed) {
        /* The server already closed, so sending close_notify is optional. */
        *ਪੂਰਾ = TRUE;
        ਨਤੀਜਾ = CURLE_OK;
      }
      ਨਹੀਂਤਾਂ {
        failf(ਅੰਕੜੇ, "schannel: error sending close msg: %d", (ਪੂਰਨ)ਨਤੀਜਾ);
        ਨਤੀਜਾ = CURLE_SEND_ERROR;
      }
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਜੇ(backend->shutdown_token.pv_ਬਫਰ)
      Curl_pSecFn->Free_Context_ਬਫਰ(backend->shutdown_token.pv_ਬਫਰ);
    memset(&backend->shutdown_token, 0, ਆਕਾਰ(backend->shutdown_token));
    backend->shutdown_ਵਿੱਥ = 0;
    backend->ਭੇਜਿਆ_shutdown = TRUE;
    *ਪੂਰਾ = TRUE;
  }

  /* If the connection seems open and we have not seen the close notify
   * from the server yet, try to receive it. */
  ਜੇ(backend->cred && backend->ctxt &&
     !backend->recv_sspi_close_notify && !backend->recv_connection_closed) {
    ਅੱਖਰ ਬਫਰ[1024];
    size_t nread;

    ਨਤੀਜਾ = schannel_recv(cf, ਅੰਕੜੇ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ), &nread);
    ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "SSL shutdown, error %d", (ਪੂਰਨ)ਨਤੀਜਾ);
      ਨਤੀਜਾ = CURLE_RECV_ERROR;
    }
    ਨਹੀਂਤਾਂ ਜੇ(nread == 0) {
      /* We got the close notify alert and are done. */
      backend->recv_connection_closed = TRUE;
      *ਪੂਰਾ = TRUE;
    }
    ਨਹੀਂਤਾਂ {
      /* still data coming in? */
    }
  }

ਬਾਹਰਲਾ:
  cf->shutdown = (ਨਤੀਜਾ || *ਪੂਰਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ schannel_close(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *)connssl->backend;

  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(backend);

  ਜੇ(backend->shutdown_token.pv_ਬਫਰ)
    Curl_pSecFn->Free_Context_ਬਫਰ(backend->shutdown_token.pv_ਬਫਰ);
  memset(&backend->shutdown_token, 0, ਆਕਾਰ(backend->shutdown_token));
  backend->shutdown_ਵਿੱਥ = 0;

  /* free SSPI Schannel API security context handle */
  ਜੇ(backend->ctxt) {
    ਜੇ(cf->conn->sslContext == &backend->ctxt->ctxt_handle)
      cf->conn->sslContext = ਨਿਰਾ;

    DEBUGF(infof(ਅੰਕੜੇ, "schannel: clear security context handle"));
    Curl_pSecFn->DeleteSecurityContext(&backend->ctxt->ctxt_handle);
    curlx_safefree(backend->ctxt);
  }

  /* free SSPI Schannel API credential handle */
  ਜੇ(backend->cred) {
    Curl_ssl_scache_ਤਾਲਾ(ਅੰਕੜੇ);
    schannel_session_free(backend->cred);
    Curl_ssl_scache_unlock(ਅੰਕੜੇ);
    backend->cred = ਨਿਰਾ;
  }

  /* free the buffer used to encrypt outgoing data */
  curlx_safefree(backend->send_ਬਫਰ);
  backend->send_ਬਫਰ_len = 0;

  /* free internal buffer for received encrypted data */
  ਜੇ(backend->encdata.ਬਫਰ) {
    curlx_safefree(backend->encdata.ਬਫਰ);
    backend->encdata.ਲੰਮਾਈ = 0;
    backend->encdata.ਵਿੱਥ = 0;
    backend->encdata_is_incomplete = FALSE;
  }

  /* free internal buffer for received decrypted data */
  ਜੇ(backend->decdata.ਬਫਰ) {
    curlx_safefree(backend->decdata.ਬਫਰ);
    backend->decdata.ਲੰਮਾਈ = 0;
    backend->decdata.ਵਿੱਥ = 0;
  }
}

ਸਥਿਰ ਪੂਰਨ schannel_init(ਖਾਲੀ)
{
  ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਅਚਲ ਅੱਖਰ *(APIENTRY *WINE_GET_ਸੰਸਕਰਣ_FN)(ਖਾਲੀ);
#ਜੇ defined(__clang__) && __clang_major__ >= 16
#ਹਦਾਇਤ clang diagnostic ਧੱਕੋ
#ਹਦਾਇਤ clang diagnostic ignored "-Wcast-function-type-strict"
#ਜੇ_ਅੰਤ
  WINE_GET_ਸੰਸਕਰਣ_FN p_wine_get_ਸੰਸਕਰਣ =
    CURLX_FUNCTION_CAST(WINE_GET_ਸੰਸਕਰਣ_FN,
      Get_Proc_ਪਤਾ(GetModuleHandle(ਪਾਠ_2("ntdll")), "wine_get_version"));
#ਜੇ defined(__clang__) && __clang_major__ >= 16
#ਹਦਾਇਤ clang diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ
  ਜੇ(p_wine_get_ਸੰਸਕਰਣ) {  /* WINE detected */
    curl_off_t ver = 0;
    ਅਚਲ ਅੱਖਰ *wine_ਸੰਸਕਰਣ = p_wine_get_ਸੰਸਕਰਣ();  /* e.g. "6.0.2" */
    /* Assume ALPN support with WINE 6.0 or greater */
    ਜੇ(wine_ਸੰਸਕਰਣ)
      curlx_str_ਸੰਖਿਆ(&wine_ਸੰਸਕਰਣ, &ver, 20);
    s_win_has_alpn = (ver >= 6);
  }
  ਨਹੀਂਤਾਂ {
    /* ALPN is supported on Windows 8.1 / Server 2012 R2 or newer. */
    s_win_has_alpn = curlx_verify_windows_ਸੰਸਕਰਣ(6, 3, 0, PLATFORM_WINNT,
                                                  ਸੰਸਕਰਣ_GREATER_THAN_EQUAL);
  }

  ਵਾਪਸ Curl_sspi_global_init() == CURLE_OK ? 1 : 0;
}

ਸਥਿਰ ਖਾਲੀ schannel_cleanup(ਖਾਲੀ)
{
  Curl_sspi_global_cleanup();
}

ਸਥਿਰ size_t schannel_ਸੰਸਕਰਣ(ਅੱਖਰ *ਬਫਰ, size_t ਮਾਪ)
{
  ਵਾਪਸ curl_msnprintf(ਬਫਰ, ਮਾਪ, "Schannel");
}

ਸਥਿਰ CURLcode schannel_random(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *entropy, size_t ਲੰਮਾਈ)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;

  ਵਾਪਸ curlx_win32_random(entropy, ਲੰਮਾਈ);
}

ਸਥਿਰ CURLcode schannel_checksum(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਵੇਸ਼,
                                  size_t inputlen,
                                  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *checksum,
                                  size_t checksumlen,
                                  DWORD provType,
                                  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ algId)
{
  CURLcode ਨਤੀਜਾ = CURLE_FAILED_INIT;

  HCRYPTPROV hProv = 0;
  HCRYPTHASH hHash = 0;

  size_t off;

  DWORD cb_Hash_ਮਾਪ;
  DWORD dw_Hash_ਮਾਪ_Len;
  DWORD dwChecksumLen;

  /* since this can fail in multiple ways, zero memory first so we never
   * return old data
   */
  memset(checksum, 0, checksumlen);

  ਜੇ(!CryptAcquireContext(&hProv, ਨਿਰਾ, ਨਿਰਾ, provType,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    ਜਾਓ ਬਾਹਰਲਾ;

  ਜੇ(!CryptCreateHash(hProv, algId, 0, 0, &hHash))
    ਜਾਓ ਬਾਹਰਲਾ;

  ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;

  off = 0;
  ਜਦੋਂ(off < inputlen) {
    DWORD chunk = (DWORD)CURLMIN(inputlen - off, 0xffffffffUL);
    ਜੇ(!Crypt_Hash_ਅੰਕੜੇ(hHash, ਨਿਵੇਸ਼ + off, chunk, 0))
      ਜਾਓ ਬਾਹਰਲਾ;
    off += chunk;
  }

  /* get hash size */
  cb_Hash_ਮਾਪ = 0;
  dw_Hash_ਮਾਪ_Len = (DWORD)ਆਕਾਰ(cb_Hash_ਮਾਪ);
  ਜੇ(!CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE *)&cb_Hash_ਮਾਪ,
                        &dw_Hash_ਮਾਪ_Len, 0))
    ਜਾਓ ਬਾਹਰਲਾ;

  /* check if hash fits into the return buffer */
  ਜੇ(checksumlen < cb_Hash_ਮਾਪ)
    ਜਾਓ ਬਾਹਰਲਾ;

  dwChecksumLen = (DWORD)checksumlen;
  ਜੇ(CryptGetHashParam(hHash, HP_HASHVAL, checksum, &dwChecksumLen, 0) &&
     dwChecksumLen == cb_Hash_ਮਾਪ)
    ਨਤੀਜਾ = CURLE_OK;

ਬਾਹਰਲਾ:
  ਜੇ(hHash)
    CryptDestroyHash(hHash);

  ਜੇ(hProv)
    CryptReleaseContext(hProv, 0);

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode schannel_sha256sum(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਵੇਸ਼,
                                   size_t inputlen,
                                   ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *sha256sum,
                                   size_t sha256len)
{
  ਵਾਪਸ schannel_checksum(ਨਿਵੇਸ਼, inputlen, sha256sum, sha256len,
                           PROV_RSA_AES, CALG_SHA_256);
}

ਸਥਿਰ ਖਾਲੀ *schannel_get_internals(ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl,
                                    CURLINFO info)
{
  ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  (ਖਾਲੀ)info;
  DEBUGASSERT(backend);
  ਵਾਪਸ &backend->ctxt->ctxt_handle;
}

HCERTSTORE Curl_schannel_get_cached_cert_store(ਬਣਤਰ Curl_cfilter *cf,
                                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ਬਣਤਰ Curl_multi *multi = ਅੰਕੜੇ->multi;
  ਅਚਲ ਬਣਤਰ curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ਬਣਤਰ schannel_cert_share *share;
  timediff_t timeout_ms;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ info_blob_digest[CURL_SHA256_DIGEST_ਲੰਮਾਈ];

  DEBUGASSERT(multi);

  ਜੇ(!multi) {
    ਵਾਪਸ ਨਿਰਾ;
  }

  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_SCHANNEL_CERT_SHARE_ਕੁੰਜੀ,
                         CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_ਕੁੰਜੀ));
  ਜੇ(!share || !share->cert_store) {
    ਵਾਪਸ ਨਿਰਾ;
  }

  /* zero ca_cache_timeout completely disables caching */
  ਜੇ(!ਅੰਕੜੇ->ਸੈੱਟ.ssl_ca_cache_timeout) {
    ਵਾਪਸ ਨਿਰਾ;
  }

  /* check for cache timeout by using the cached_x509_store_expired timediff
     calculation pattern from openssl.c.
     negative timeout means retain forever. */
  timeout_ms = ਅੰਕੜੇ->ਸੈੱਟ.ssl_ca_cache_timeout * (timediff_t)1000;
  ਜੇ(timeout_ms >= 0) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(ਅੰਕੜੇ), &share->ਸਮਾਂ);
    ਜੇ(elapsed_ms >= timeout_ms) {
      ਵਾਪਸ ਨਿਰਾ;
    }
  }

  ਜੇ(ca_info_blob) {
    ਜੇ(share->CAinfo_blob_ਮਾਪ != ca_info_blob->len) {
      ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ(schannel_sha256sum((ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ca_info_blob->ਅੰਕੜੇ,
                          ca_info_blob->len,
                          info_blob_digest,
                          CURL_SHA256_DIGEST_ਲੰਮਾਈ))
      ਵਾਪਸ ਨਿਰਾ;
    ਜੇ(memcmp(share->CAinfo_blob_digest, info_blob_digest,
              CURL_SHA256_DIGEST_ਲੰਮਾਈ)) {
      ਵਾਪਸ ਨਿਰਾ;
    }
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ(!conn_config->CAfile || !share->CAfile ||
       strcmp(share->CAfile, conn_config->CAfile)) {
      ਵਾਪਸ ਨਿਰਾ;
    }
  }

  ਵਾਪਸ share->cert_store;
}

ਸਥਿਰ ਖਾਲੀ schannel_cert_share_free(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t ਕੁੰਜੀ_len, ਖਾਲੀ *p)
{
  ਬਣਤਰ schannel_cert_share *share = p;
  DEBUGASSERT(ਕੁੰਜੀ_len == CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_ਕੁੰਜੀ));
  DEBUGASSERT(!memcmp(MPROTO_SCHANNEL_CERT_SHARE_ਕੁੰਜੀ, ਕੁੰਜੀ, ਕੁੰਜੀ_len));
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)ਕੁੰਜੀ_len;
  ਜੇ(share->cert_store) {
    CertCloseStore(share->cert_store, 0);
  }
  curlx_free(share->CAfile);
  curlx_free(share);
}

bool Curl_schannel_ਸੈੱਟ_cached_cert_store(ਬਣਤਰ Curl_cfilter *cf,
                                         ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         HCERTSTORE cert_store)
{
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ਬਣਤਰ Curl_multi *multi = ਅੰਕੜੇ->multi;
  ਅਚਲ ਬਣਤਰ curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ਬਣਤਰ schannel_cert_share *share;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ digest[CURL_SHA256_DIGEST_ਲੰਮਾਈ];
  ਅੱਖਰ *CAfile = ਨਿਰਾ;

  DEBUGASSERT(multi);

  ਜੇ(!multi) {
    ਵਾਪਸ FALSE;
  }

  ਜੇ(ca_info_blob) {
    ਜੇ(schannel_sha256sum((ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ca_info_blob->ਅੰਕੜੇ,
                          ca_info_blob->len, digest, ਆਕਾਰ(digest))) {
      ਵਾਪਸ FALSE;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(conn_config->CAfile) {
    CAfile = curlx_strdup(conn_config->CAfile);
    ਜੇ(!CAfile) {
      ਵਾਪਸ FALSE;
    }
  }

  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_SCHANNEL_CERT_SHARE_ਕੁੰਜੀ,
                         CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_ਕੁੰਜੀ));
  ਜੇ(!share) {
    share = curlx_calloc(1, ਆਕਾਰ(*share));
    ਜੇ(!share) {
      curlx_free(CAfile);
      ਵਾਪਸ FALSE;
    }
    ਜੇ(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_SCHANNEL_CERT_SHARE_ਕੁੰਜੀ,
                       CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_ਕੁੰਜੀ),
                       share, schannel_cert_share_free)) {
      curlx_free(share);
      curlx_free(CAfile);
      ਵਾਪਸ FALSE;
    }
  }

  /* free old cache data */
  ਜੇ(share->cert_store) {
    CertCloseStore(share->cert_store, 0);
  }
  curlx_free(share->CAfile);

  ਜੇ(ca_info_blob) {
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(share->CAinfo_blob_digest, digest, ਆਕਾਰ(digest));
    share->CAinfo_blob_ਮਾਪ = ca_info_blob->len;
  }
  ਨਹੀਂਤਾਂ
    share->CAinfo_blob_ਮਾਪ = 0;

  share->ਸਮਾਂ = curlx_now();
  share->cert_store = cert_store;
  share->CAfile = CAfile;
  ਵਾਪਸ TRUE;
}

ਅਚਲ ਬਣਤਰ Curl_ssl Curl_ssl_schannel = {
  { CURLSSLBACKEND_SCHANNEL, "schannel" }, /* info */

  SSLSUPP_CERTINFO |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_CA_CACHE |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST,

  ਆਕਾਰ(ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ),

  schannel_init,                     /* init */
  schannel_cleanup,                  /* cleanup */
  schannel_ਸੰਸਕਰਣ,                  /*ਸੰਸਕਰਣ*/
  schannel_shutdown,                 /* shutdown */
  schannel_ਅੰਕੜੇ_pending,             /* data_pending */
  schannel_random,                   /* random */
  ਨਿਰਾ,                              /* cert_status_request */
  schannel_connect,                  /* connect */
  Curl_ssl_adjust_pollset,           /* adjust_pollset */
  schannel_get_internals,            /* get_internals */
  schannel_close,                    /* close_one */
  ਨਿਰਾ,                              /* close_all */
  ਨਿਰਾ,                              /* engines_list */
  schannel_sha256sum,                /* sha256sum */
  schannel_recv,                     /* recv decrypted data */
  schannel_send,                     /* send data to encrypt */
  ਨਿਰਾ,                              /* get_channel_binding */
};

#ਜੇ_ਅੰਤ /* USE_SCHANNEL */
