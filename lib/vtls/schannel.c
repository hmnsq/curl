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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_SCHANNEL

#నిర్వచితం_కాకపోతే USE_WINDOWS_SSPI
#దోషం "cannot compile Schannel support without SSPI."
#అయితే_ముగింపు

#అన్తర్భావ "vtls/schannel.h"
#అన్తర్భావ "vtls/schannel_int.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "vtls/vtls_int.h"
#అన్తర్భావ "vtls/vtls_scache.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "connect.h" /* for the connect timeout */
#అన్తర్భావ "curlx/strdup.h"
#అన్తర్భావ "strerror.h"
#అన్తర్భావ "select.h" /* for the socket readiness */
#అన్తర్భావ "curlx/win32-fopen.h"
#అన్తర్భావ "curlx/multibyte.h"
#అన్తర్భావ "vtls/x509asn1.h"
#అన్తర్భావ "curlx/version_win32.h"
#అన్తర్భావ "curlx/winapi.h"
#అన్తర్భావ "curlx/strparse.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "curl_sha256.h"

/* Some verbose debug messages are wrapped by SCH_DEV() instead of DEBUGF()
 * and only shown if CURL_SCHANNEL_DEV_DEBUG was defined at build time. These
 * messages are extra verbose and intended for curl developers debugging
 * Schannel recv decryption and renegotiation.
 */
#నిర్వచితమైతే CURL_SCHANNEL_DEV_DEBUG
#నిర్వచించు SCH_DEV(x) x
#నిర్వచించు SCH_DEV_SHOWBOOL(x)                                     \
  చేయి {                                                          \
    infof(దత్తాంశం, "schannel: " #x " %s", (x) ? "TRUE" : "FALSE"); \
  } అయితే(0)
#లేకపోతే
#నిర్వచించు SCH_DEV(x) చేయి {} అయితే(0)
#నిర్వచించు SCH_DEV_SHOWBOOL(x) చేయి {} అయితే(0)
#అయితే_ముగింపు

/* Offered by mingw-w64 v8+, MS SDK 7.0A/VS2010+ */
#నిర్వచితం_కాకపోతే SP_PROT_TLS1_0_CLIENT
#నిర్వచించు SP_PROT_TLS1_0_CLIENT           SP_PROT_TLS1_CLIENT
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే SP_PROT_TLS1_1_CLIENT
#నిర్వచించు SP_PROT_TLS1_1_CLIENT           0x00000200
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే SP_PROT_TLS1_2_CLIENT
#నిర్వచించు SP_PROT_TLS1_2_CLIENT           0x00000800
#అయితే_ముగింపు

/* Offered by mingw-w64 v8+, MS SDK 10.0.15063.0/VS2017 15.1+ */
#నిర్వచితం_కాకపోతే SP_PROT_TLS1_3_CLIENT
#నిర్వచించు SP_PROT_TLS1_3_CLIENT           0x00002000
#అయితే_ముగింపు
/* Offered by mingw-w64 v8+, MS SDK 8.1/VS2013+ */
#నిర్వచితం_కాకపోతే SCH_USE_STRONG_CRYPTO
#నిర్వచించు SCH_USE_STRONG_CRYPTO           0x00400000
#అయితే_ముగింపు

/* Offered by mingw-w64 v10+, MS SDK 7.0A/VS2010+ */
#నిర్వచితం_కాకపోతే SECBUFFER_ALERT
#నిర్వచించు SECBUFFER_ALERT                 17
#అయితే_ముగింపు

/* Both schannel buffer sizes must be > 0 */
#నిర్వచించు CURL_SCHANNEL_బఫరు_INIT_కొలత   4096
#నిర్వచించు CURL_SCHANNEL_బఫరు_FREE_కొలత   1024

#నిర్వచించు CERT_THUMBPRINT_STR_LEN 40
#నిర్వచించు CERT_THUMBPRINT_దత్తాంశం_LEN 20

/* Uncomment to force verbose output
 * #define infof(x, y, ...) curl_mprintf(y, __VA_ARGS__)
 * #define failf(x, y, ...) curl_mprintf(y, __VA_ARGS__)
 */

/* Offered by mingw-w64 v4+, MS SDK 6.0A/VS2008+ */
#నిర్వచితం_కాకపోతే PKCS12_NO_PERSIST_కీలకం
#నిర్వచించు PKCS12_NO_PERSIST_కీలకం 0x00008000
#అయితే_ముగింపు

/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#నిర్వచితం_కాకపోతే CERT_కనుగొను_HAS_వ్యక్తిగత_కీలకం
#నిర్వచించు CERT_కనుగొను_HAS_వ్యక్తిగత_కీలకం (21 << CERT_COMPARE_జరుపు)
#అయితే_ముగింపు

/* key to use at `multi->proto_hash` */
#నిర్వచించు MPROTO_SCHANNEL_CERT_SHARE_కీలకం   "tls:schannel:cert:share"

స్థిర bool s_win_has_alpn;

/* Offered by mingw-w64 v9+, MS SDK 8.1/VS2013+ */
#నిర్వచితం_కాకపోతే SECBUFFER_APPLICATION_PROTOCOLS
#నిర్వచించు SECBUFFER_APPLICATION_PROTOCOLS 18
#నిర్వచించు SECPKG_ATTR_APPLICATION_PROTOCOL 35

రకనిర్వచనం గణన {
  SecApplicationProtocolNegotiationExt_None,
  SecApplicationProtocolNegotiationExt_NPN,
  SecApplicationProtocolNegotiationExt_ALPN
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT;

రకనిర్వచనం గణన {
  Sec_Application_Protocol_Negotiation_స్థితి_None,
  Sec_Application_Protocol_Negotiation_స్థితి_Success,
  Sec_Application_Protocol_Negotiation_స్థితి_Selected_Client_Only
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_స్థితి;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
రకనిర్వచనం నిర్మాణం {
  SEC_APPLICATION_PROTOCOL_NEGOTIATION_స్థితి Proto_Nego_స్థితి;
  SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT ProtoNegoExt;
  గుర్తులేని అక్షరం Protocol_Id_కొలత;
  గుర్తులేని అక్షరం ProtocolId[0xff];
} SecPkgContext_ApplicationProtocol;
#అయితే_ముగింపు

స్థిర శూన్య Init_Sec_బఫరు(Sec_బఫరు *బఫరు, గుర్తులేని దీర్ఘ BufType,
                          శూన్య *Buf_దత్తాంశం_Ptr, గుర్తులేని దీర్ఘ Buf_Byte_కొలత)
{
  బఫరు->cb_బఫరు = Buf_Byte_కొలత;
  బఫరు->బఫరు_Type = BufType;
  బఫరు->pv_బఫరు = Buf_దత్తాంశం_Ptr;
}

స్థిర శూన్య Init_Sec_బఫరు_Desc(Sec_బఫరు_Desc *desc, Sec_బఫరు *BufArr,
                              గుర్తులేని దీర్ఘ అంకె_Arr_Elem)
{
  desc->ul_సంస్కరణ = SECBUFFER_సంస్కరణ;
  desc->pBuffers = BufArr;
  desc->cBuffers = అంకె_Arr_Elem;
}

స్థిర CURLcode schannel_సమితి_ssl_సంస్కరణ_min_max(DWORD *enabled_protocols,
                                                 నిర్మాణం Curl_cfilter *cf,
                                                 నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  దీర్ఘ ssl_సంస్కరణ = conn_config->సంస్కరణ_2;
  దీర్ఘ ssl_సంస్కరణ_max = (దీర్ఘ)conn_config->సంస్కరణ_max;
  దీర్ఘ i = ssl_సంస్కరణ;

  ఎంపిక(ssl_సంస్కరణ_max) {
  స్థితి CURL_SSLVERSION_MAX_NONE:
  స్థితి CURL_SSLVERSION_MAX_DEFAULT:

    /* Windows Server 2022 and newer (including Windows 11) support TLS 1.3
       built-in. Previous builds of Windows 10 had broken TLS 1.3
       implementations that could be enabled via registry. */
    ఒకవేళ(curlx_verify_windows_సంస్కరణ(10, 0, 20348, PLATFORM_WINNT,
                                    సంస్కరణ_GREATER_THAN_EQUAL)) {
      ssl_సంస్కరణ_max = CURL_SSLVERSION_MAX_TLSv1_3;
    }
    లేకపోతే /* Windows 10 or older */
      ssl_సంస్కరణ_max = CURL_SSLVERSION_MAX_TLSv1_2;

    ఆపు;
  }

  కొరకు(; i <= (ssl_సంస్కరణ_max >> 16); ++i) {
    ఎంపిక(i) {
    స్థితి CURL_SSLVERSION_TLSv1_0:
      *enabled_protocols |= SP_PROT_TLS1_0_CLIENT;
      ఆపు;
    స్థితి CURL_SSLVERSION_TLSv1_1:
      *enabled_protocols |= SP_PROT_TLS1_1_CLIENT;
      ఆపు;
    స్థితి CURL_SSLVERSION_TLSv1_2:
      *enabled_protocols |= SP_PROT_TLS1_2_CLIENT;
      ఆపు;
    స్థితి CURL_SSLVERSION_TLSv1_3:

      /* Windows Server 2022 or newer */
      ఒకవేళ(curlx_verify_windows_సంస్కరణ(10, 0, 20348, PLATFORM_WINNT,
                                      సంస్కరణ_GREATER_THAN_EQUAL)) {
        *enabled_protocols |= SP_PROT_TLS1_3_CLIENT;
        ఆపు;
      }
      లేకపోతే { /* Windows 10 or older */
        failf(దత్తాంశం, "schannel: TLS 1.3 not supported on Windows 10 or older");
        తిరిగి CURLE_SSL_CONNECT_ERROR;
      }
    }
  }
  తిరిగి CURLE_OK;
}

#నిర్వచించు CIPHEROPTION(x) { #x, x }

నిర్మాణం algo {
  స్థిరరాశి అక్షరం *name;
  పూర్ణాంకం id;
};

స్థిర స్థిరరాశి నిర్మాణం algo algs[] = {
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
  CIPHEROPTION(CALG_SCHANNEL_MAC_కీలకం),
  CIPHEROPTION(CALG_SCHANNEL_ENC_కీలకం),
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
#నిర్వచితమైతే CALG_ECMQV
  CIPHEROPTION(CALG_ECMQV),
#అయితే_ముగింపు
  CIPHEROPTION(CALG_ECDSA),
/* Offered by mingw-w64 v7+, MS SDK 7.0A/VS2010+ */
#నిర్వచితమైతే CALG_ECDH_EPHEM
  CIPHEROPTION(CALG_ECDH_EPHEM),
#అయితే_ముగింపు
  { ఖాళీ, 0 },
};

స్థిర పూర్ణాంకం get_alg_id_by_name(స్థిరరాశి అక్షరం *name)
{
  స్థిరరాశి అక్షరం *name_చివర = strchr(name, ':');
  size_t n = name_చివర ? (size_t)(name_చివర - name) : అక్షరశ్రేణి_పొడవు(name);
  పూర్ణాంకం i;

  కొరకు(i = 0; algs[i].name; i++) {
    ఒకవేళ((n == అక్షరశ్రేణి_పొడవు(algs[i].name) && !అక్షరశ్రేణి_పోలిక(algs[i].name, name, n)))
      తిరిగి algs[i].id;
  }
  తిరిగి 0; /* not found */
}

#నిర్వచించు అంకె_CIPHERS 47 /* There are a maximum of 47 options listed above */

స్థిర CURLcode సమితి_ssl_ciphers(SCHANNEL_CRED *schannel_cred, అక్షరం *ciphers,
                                ALG_ID *algIds)
{
  స్థిరరాశి అక్షరం *ప్రారంభం_Cur = ciphers;
  పూర్ణాంకం alg_లెక్క = 0;
  అయితే(ప్రారంభం_Cur && *ప్రారంభం_Cur && (alg_లెక్క < అంకె_CIPHERS)) {
    curl_off_t alg;
    ఒకవేళ(curlx_str_సంఖ్య(&ప్రారంభం_Cur, &alg, గరిష్ఠ_పూర్ణాంకం) || !alg)
      alg = get_alg_id_by_name(ప్రారంభం_Cur);

    ఒకవేళ(alg)
      algIds[alg_లెక్క++] = (ALG_ID)alg;
    లేకపోతే ఒకవేళ(!అక్షరశ్రేణి_పోలిక(ప్రారంభం_Cur, "USE_STRONG_CRYPTO",
                     CURL_CSTRLEN("USE_STRONG_CRYPTO")) ||
            !అక్షరశ్రేణి_పోలిక(ప్రారంభం_Cur, "SCH_USE_STRONG_CRYPTO",
                     CURL_CSTRLEN("SCH_USE_STRONG_CRYPTO")))
      schannel_cred->dwFlags |= SCH_USE_STRONG_CRYPTO;
    లేకపోతే
      తిరిగి CURLE_SSL_CIPHER;
    ప్రారంభం_Cur = strchr(ప్రారంభం_Cur, ':');
    ఒకవేళ(ప్రారంభం_Cur)
      ప్రారంభం_Cur++;
  }
  schannel_cred->palgSupportedAlgs = algIds;
  schannel_cred->cSupportedAlgs = (DWORD)alg_లెక్క;
  తిరిగి CURLE_OK;
}

/* Function allocates memory for store_path only if CURLE_OK is returned */
స్థిర CURLcode get_cert_location(TCHAR *మార్గం, DWORD *store_name,
                                  TCHAR **store_మార్గం, TCHAR **thumbprint)
{
  TCHAR *sep;
  TCHAR *store_మార్గం_ప్రారంభం;
  size_t store_name_len;

  sep = _tcschr(మార్గం, పాఠ్యం('\\'));
  ఒకవేళ(!sep)
    తిరిగి CURLE_SSL_CERTPROBLEM;

  store_name_len = sep - మార్గం;

  ఒకవేళ(_tcsncmp(మార్గం, పాఠ్యం("CurrentUser"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_USER;
  లేకపోతే ఒకవేళ(_tcsncmp(మార్గం, పాఠ్యం("LocalMachine"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE;
  లేకపోతే ఒకవేళ(_tcsncmp(మార్గం, పాఠ్యం("CurrentService"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_SERVICE;
  లేకపోతే ఒకవేళ(_tcsncmp(మార్గం, పాఠ్యం("Services"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_SERVICES;
  లేకపోతే ఒకవేళ(_tcsncmp(మార్గం, పాఠ్యం("Users"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_USERS;
  లేకపోతే ఒకవేళ(_tcsncmp(మార్గం, పాఠ్యం("CurrentUserGroupPolicy"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_USER_సమూహం_POLICY;
  లేకపోతే ఒకవేళ(_tcsncmp(మార్గం, పాఠ్యం("LocalMachineGroupPolicy"), store_name_len) ==
          0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE_సమూహం_POLICY;
  లేకపోతే ఒకవేళ(_tcsncmp(మార్గం, పాఠ్యం("LocalMachineEnterprise"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE_ENTERPRISE;
  లేకపోతే
    తిరిగి CURLE_SSL_CERTPROBLEM;

  store_మార్గం_ప్రారంభం = sep + 1;

  sep = _tcschr(store_మార్గం_ప్రారంభం, పాఠ్యం('\\'));
  ఒకవేళ(!sep)
    తిరిగి CURLE_SSL_CERTPROBLEM;

  *thumbprint = sep + 1;
  ఒకవేళ(_tcslen(*thumbprint) != CERT_THUMBPRINT_STR_LEN)
    తిరిగి CURLE_SSL_CERTPROBLEM;

  *sep = పాఠ్యం('\0');
  *store_మార్గం = curlx_tcsdup(store_మార్గం_ప్రారంభం);
  *sep = పాఠ్యం('\\');
  ఒకవేళ(!*store_మార్గం)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  తిరిగి CURLE_OK;
}

స్థిర CURLcode get_client_cert(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                HCERTSTORE *బహిర్గతం_cert_store,
                                PCCERT_CONTEXT *బహిర్గతం_cert_context)
{
  నిర్మాణం ssl_filter_config *sslc = Curl_ssl_cf_get_filter_config(cf);
  PCCERT_CONTEXT client_cert = ఖాళీ;
  HCERTSTORE client_cert_store = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(sslc->clientcert || sslc->cert_blob) {
    DWORD cert_store_name = 0;
    TCHAR *cert_store_మార్గం = ఖాళీ;
    TCHAR *cert_thumbprint_str = ఖాళీ;
    TCHAR cert_thumbprint_buf[CERT_THUMBPRINT_STR_LEN + 1];
    CRYPT_HASH_BLOB cert_thumbprint;
    BYTE cert_thumbprint_దత్తాంశం[CERT_THUMBPRINT_దత్తాంశం_LEN];
    HCERTSTORE cert_store = ఖాళీ;
    FILE *f_లోపల_Cert = ఖాళీ;
    శూన్య *certdata = ఖాళీ;
    size_t certsize = 0;
    bool blob = !!sslc->cert_blob;

    ఒకవేళ(blob) {
      certdata = sslc->cert_blob->దత్తాంశం;
      certsize = sslc->cert_blob->len;
    }
    లేకపోతే {
      TCHAR *cert_మార్గం =
        curlx_convert_UTF8_గమ్యం_tchar(sslc->clientcert);
      ఒకవేళ(!cert_మార్గం)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

      ఫలితం = get_cert_location(cert_మార్గం, &cert_store_name,
                                 &cert_store_మార్గం, &cert_thumbprint_str);

      /* 'cert_thumbprint_str' points in to the allocated 'cert_path', copy
         the data. The string is verified to be CERT_THUMBPRINT_STR_LEN bytes
         long within the get_cert_location() function. */
      ఒకవేళ(!ఫలితం && cert_thumbprint_str) {
        జ్ఞాపకం_నకలు(cert_thumbprint_buf, cert_thumbprint_str,
               పరిమాణం(cert_thumbprint_buf));
        cert_thumbprint_str = cert_thumbprint_buf;
      }

      curlx_free(cert_మార్గం);
      ఒకవేళ(ఫలితం && (sslc->clientcert[0] != '\0'))
        f_లోపల_Cert = curlx_fopen(sslc->clientcert, "rb");

      ఒకవేళ(ఫలితం && !f_లోపల_Cert) {
        failf(దత్తాంశం, "schannel: Failed to get certificate location"
              " or file for %s",
              sslc->clientcert);
        తిరిగి ఫలితం;
      }
    }

    ఒకవేళ((f_లోపల_Cert || blob) && sslc->cert_type &&
       !curl_strequal(sslc->cert_type, "P12")) {
      failf(దత్తాంశం, "schannel: certificate format compatibility error "
            "for %s",
            blob ? "(memory blob)" : sslc->clientcert);
      curlx_free(cert_store_మార్గం);
      ఒకవేళ(f_లోపల_Cert)
        curlx_fclose(f_లోపల_Cert);
      తిరిగి CURLE_SSL_CERTPROBLEM;
    }

    ఒకవేళ(f_లోపల_Cert || blob) {
      /* Reading a .p12 or .pfx file, like the example at bottom of
         https://learn.microsoft.com/archive/msdn-technet-forums/3e7bc95f-b21a-4bcd-bd2c-7f996718cae5
       */
      CRYPT_దత్తాంశం_BLOB datablob;
      WCHAR *pszPassword;
      size_t pwd_len = 0;
      పూర్ణాంకం cert_కనుగొను_flags;
      స్థిరరాశి అక్షరం *cert_showfilename_error = blob ?
        "(memory blob)" : sslc->clientcert;
      curlx_free(cert_store_మార్గం);
      ఒకవేళ(f_లోపల_Cert) {
        దీర్ఘ cert_tell = 0;
        bool continue_reading = fseek(f_లోపల_Cert, 0, SEEK_END) == 0;
        ఒకవేళ(continue_reading) {
          cert_tell = ftell(f_లోపల_Cert);
          ఒకవేళ(cert_tell < 0)
            continue_reading = FALSE;
          లేకపోతే
            certsize = (size_t)cert_tell;
        }
        ఒకవేళ(continue_reading)
          continue_reading = fseek(f_లోపల_Cert, 0, SEEK_SET) == 0;
        ఒకవేళ(continue_reading && (certsize < CURL_MAX_ఆదానం_నిడివి))
          certdata = curlx_malloc(certsize + 1);
        ఒకవేళ(!certdata || ((పూర్ణాంకం)fread(certdata, certsize, 1, f_లోపల_Cert) != 1))
          continue_reading = FALSE;
        curlx_fclose(f_లోపల_Cert);
        ఒకవేళ(!continue_reading) {
          failf(దత్తాంశం, "schannel: Failed to read cert file %s",
                sslc->clientcert);
          curlx_free(certdata);
          తిరిగి CURLE_SSL_CERTPROBLEM;
        }
      }

      /* Convert key-pair data to the in-memory certificate store */
      datablob.pb_దత్తాంశం = (BYTE *)certdata;
      datablob.cb_దత్తాంశం = (DWORD)certsize;

      ఒకవేళ(sslc->కీలకం_passwd)
        pwd_len = అక్షరశ్రేణి_పొడవు(sslc->కీలకం_passwd);
      pszPassword = curlx_malloc(పరిమాణం(WCHAR) * (pwd_len + 1));
      ఒకవేళ(pszPassword) {
        పూర్ణాంకం str_w_len = 0;
        ఒకవేళ(pwd_len > 0)
          str_w_len = Multi_Byte_గమ్యం_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                          sslc->కీలకం_passwd, (పూర్ణాంకం)pwd_len,
                                          pszPassword, (పూర్ణాంకం)(pwd_len + 1));

        ఒకవేళ((str_w_len >= 0) && (str_w_len <= (పూర్ణాంకం)pwd_len))
          pszPassword[str_w_len] = 0;
        లేకపోతే
          pszPassword[0] = 0;

        cert_store = PFXImportCertStore(&datablob, pszPassword,
                                        PKCS12_NO_PERSIST_కీలకం);
        curlx_memzero(pszPassword, పరిమాణం(WCHAR) * (pwd_len + 1));
        curlx_free(pszPassword);
      }
      ఒకవేళ(!blob)
        curlx_free(certdata);
      ఒకవేళ(!cert_store) {
        DWORD errorcode = Get_చివరి_Error();
        ఒకవేళ(errorcode == ERROR_INVALID_PASSWORD)
          failf(దత్తాంశం, "schannel: Failed to import cert file %s, "
                "password is bad",
                cert_showfilename_error);
        లేకపోతే
          failf(దత్తాంశం, "schannel: Failed to import cert file %s, "
                "last error is 0x%08lx",
                cert_showfilename_error, errorcode);
        తిరిగి CURLE_SSL_CERTPROBLEM;
      }

      /* CERT_FIND_HAS_PRIVATE_KEY is only available in Windows 8 / Server
         2012, (NT 6.2). For older versions we use CURL_FIND_ANY. */
      ఒకవేళ(curlx_verify_windows_సంస్కరణ(6, 2, 0, PLATFORM_WINNT,
                                      సంస్కరణ_GREATER_THAN_EQUAL))
        cert_కనుగొను_flags = CERT_కనుగొను_HAS_వ్యక్తిగత_కీలకం;
      లేకపోతే
        cert_కనుగొను_flags = CERT_కనుగొను_ANY;

      client_cert =
        Cert_కనుగొను_Certificate_లోపల_Store(cert_store,
                                   X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                   cert_కనుగొను_flags, ఖాళీ, ఖాళీ);

      ఒకవేళ(!client_cert) {
        failf(దత్తాంశం, "schannel: Failed to get certificate from file %s"
              ", last error is 0x%08lx",
              cert_showfilename_error, Get_చివరి_Error());
        CertCloseStore(cert_store, 0);
        తిరిగి CURLE_SSL_CERTPROBLEM;
      }
    }
    లేకపోతే {
      cert_store =
        CertOpenStore(
#నిర్వచితమైతే UNICODE
                      CERT_STORE_PROV_SYSTEM_W,
#లేకపోతే
                      CERT_STORE_PROV_SYSTEM_A,
#అయితే_ముగింపు
                      0,
                      (HCRYPTPROV)ఖాళీ,
                      CERT_STORE_OPEN_EXISTING_FLAG | cert_store_name,
                      cert_store_మార్గం);
      ఒకవేళ(!cert_store) {
        అక్షరం *మార్గం_utf8 =
          curlx_convert_tchar_గమ్యం_UTF8(cert_store_మార్గం);
        failf(దత్తాంశం, "schannel: Failed to open cert store %lx %s, "
              "last error is 0x%08lx",
              cert_store_name, (మార్గం_utf8 ? మార్గం_utf8 : "(unknown)"),
              Get_చివరి_Error());
        curlx_free(cert_store_మార్గం);
        curlx_free(మార్గం_utf8);
        తిరిగి CURLE_SSL_CERTPROBLEM;
      }
      curlx_free(cert_store_మార్గం);

      cert_thumbprint.pb_దత్తాంశం = cert_thumbprint_దత్తాంశం;
      cert_thumbprint.cb_దత్తాంశం = CERT_THUMBPRINT_దత్తాంశం_LEN;

      ఒకవేళ(!Crypt_String_గమ్యం_ద్విమాన(cert_thumbprint_str,
                              CERT_THUMBPRINT_STR_LEN,
                              CRYPT_STRING_HEX,
                              cert_thumbprint_దత్తాంశం,
                              &cert_thumbprint.cb_దత్తాంశం,
                              ఖాళీ, ఖాళీ)) {
        CertCloseStore(cert_store, 0);
        తిరిగి CURLE_SSL_CERTPROBLEM;
      }

      client_cert =
        Cert_కనుగొను_Certificate_లోపల_Store(cert_store,
                                   X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                   CERT_కనుగొను_HASH, &cert_thumbprint, ఖాళీ);
      ఒకవేళ(!client_cert) {
        /* CRYPT_E_NOT_FOUND / E_INVALIDARG */
        CertCloseStore(cert_store, 0);
        failf(దత్తాంశం, "schannel: client cert not found in cert store");
        తిరిగి CURLE_SSL_CERTPROBLEM;
      }
    }
    client_cert_store = cert_store;
  }

  *బహిర్గతం_cert_store = client_cert_store;
  *బహిర్గతం_cert_context = client_cert;

  తిరిగి CURLE_OK;
}

స్థిర CURLcode acquire_sspi_handle(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం schannel_ssl_backend_దత్తాంశం *backend,
                                    PCCERT_CONTEXT client_cert,
                                    DWORD flags,
                                    DWORD enabled_protocols)
{
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SECURITY_స్థితి sspi_స్థితి = SEC_E_OK;
  CURLcode ఫలితం;

  /* We support TLS 1.3 starting in Windows Server 2022 or later
   * (OS build 20348) */
  ఒకవేళ(!conn_config->cipher_list &&
     curlx_verify_windows_సంస్కరణ(10, 0, 20348, PLATFORM_WINNT,
                                  సంస్కరణ_GREATER_THAN_EQUAL)) {

    SCH_CREDENTIALS credentials = { 0 };
    TLS_PARAMETERS tls_parameters = { 0 };
    CRYPTO_SETTINGS crypto_settings[1];
    PCCERT_CONTEXT client_certs[1];

    ఒకవేళ(client_cert)
      client_certs[0] = client_cert;
    లేకపోతే
      client_certs[0] = ఖాళీ;

    memset(crypto_settings, 0, పరిమాణం(crypto_settings));

    tls_parameters.pDisabledCrypto = crypto_settings;

    /* The number of blocked suites */
    tls_parameters.cDisabledCrypto = (DWORD)0;
    credentials.pTlsParameters = &tls_parameters;
    credentials.cTlsParameters = 1;

    credentials.dw_సంస్కరణ = SCH_CREDENTIALS_సంస్కరణ;
    credentials.dwFlags = flags | SCH_USE_STRONG_CRYPTO;

    credentials.pTlsParameters->grbitDisabledProtocols = ~enabled_protocols;

    ఒకవేళ(client_certs[0]) {
      credentials.cCreds = 1;
      credentials.paCred = client_certs;
    }

    sspi_స్థితి =
      Curl_pSecFn->AcquireCredentialsHandle(ఖాళీ, CURL_UNCONST(UNISP_NAME),
                                            SECPKG_CRED_OUTBOUND, ఖాళీ,
                                            &credentials, ఖాళీ, ఖాళీ,
                                            &backend->cred->cred_handle, ఖాళీ);
  }
  లేకపోతే {
    /* Pre-Windows 10 1809 or the user set a legacy algorithm list.
       Schannel does not negotiate TLS 1.3 when SCHANNEL_CRED is used. */
    ALG_ID algIds[అంకె_CIPHERS];
    అక్షరం *ciphers = conn_config->cipher_list;
    SCHANNEL_CRED schannel_cred = { 0 };
    PCCERT_CONTEXT client_certs[1];

    ఒకవేళ(client_cert)
      client_certs[0] = client_cert;
    లేకపోతే
      client_certs[0] = ఖాళీ;

    schannel_cred.dw_సంస్కరణ = SCHANNEL_CRED_సంస్కరణ;
    schannel_cred.dwFlags = flags;
    schannel_cred.grbitEnabledProtocols = enabled_protocols;

    ఒకవేళ(ciphers) {
      ఒకవేళ((enabled_protocols & SP_PROT_TLS1_3_CLIENT)) {
        ఒకవేళ(!(enabled_protocols & ~SP_PROT_TLS1_3_CLIENT)) {
          failf(దత్తాంశం, "schannel: TLS 1.3 is not supported with a cipher list; "
                "remove the cipher list or allow a lower TLS version");
          తిరిగి CURLE_SSL_CONNECT_ERROR;
        }
        infof(దత్తాంశం, "schannel: WARNING: This version of Schannel "
              "negotiates a less-secure TLS version than TLS 1.3 because the "
              "user set an algorithm cipher list.");
      }
      ఫలితం = సమితి_ssl_ciphers(&schannel_cred, ciphers, algIds);
      ఒకవేళ(ఫలితం) {
        failf(దత్తాంశం, "schannel: Failed setting algorithm cipher list");
        తిరిగి ఫలితం;
      }
    }
    లేకపోతే {
      schannel_cred.dwFlags = flags | SCH_USE_STRONG_CRYPTO;
    }

    ఒకవేళ(client_certs[0]) {
      schannel_cred.cCreds = 1;
      schannel_cred.paCred = client_certs;
    }

    sspi_స్థితి =
      Curl_pSecFn->AcquireCredentialsHandle(ఖాళీ, CURL_UNCONST(UNISP_NAME),
                                            SECPKG_CRED_OUTBOUND, ఖాళీ,
                                            &schannel_cred, ఖాళీ, ఖాళీ,
                                            &backend->cred->cred_handle, ఖాళీ);
  }

  ఒకవేళ(sspi_స్థితి != SEC_E_OK) {
    అక్షరం బఫరు[STRERROR_LEN];
    failf(దత్తాంశం, "schannel: AcquireCredentialsHandle failed: %s",
          Curl_sspi_strerror(sspi_స్థితి, బఫరు, పరిమాణం(బఫరు)));
    ఎంపిక(sspi_స్థితి) {
    స్థితి SEC_E_INSUFFICIENT_జ్ఞాపకం:
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    స్థితి SEC_E_NO_CREDENTIALS:
    స్థితి SEC_E_SECPKG_NOT_దొరికింది:
    స్థితి SEC_E_NOT_OWNER:
    స్థితి SEC_E_UNKNOWN_CREDENTIALS:
    స్థితి SEC_E_అంతర్గత_ERROR:
    అప్రమేయ:
      తిరిగి CURLE_SSL_CONNECT_ERROR;
    }
  }

  తిరిగి CURLE_OK;
}

స్థిర CURLcode schannel_acquire_credential_handle(నిర్మాణం Curl_cfilter *cf,
                                                   నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  PCCERT_CONTEXT client_cert = ఖాళీ;
  HCERTSTORE client_cert_store = ఖాళీ;
  CURLcode ఫలితం;

  /* setup Schannel API options */
  DWORD flags = 0;
  DWORD enabled_protocols = 0;

  నిర్మాణం schannel_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం schannel_ssl_backend_దత్తాంశం *)connssl->backend;

  DEBUGASSERT(backend);

  ఒకవేళ(conn_config->verifypeer) {
    ఒకవేళ(backend->use_manual_cred_validation)
      flags = SCH_CRED_MANUAL_CRED_VALIDATION;
    లేకపోతే
      flags = SCH_CRED_AUTO_CRED_VALIDATION;

    ఒకవేళ(conn_config->no_revoke) {
      flags |= SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
               SCH_CRED_IGNORE_REVOCATION_OFFLINE;

      DEBUGF(infof(దత్తాంశం, "schannel: disabled server certificate revocation "
                         "checks"));
    }
    లేకపోతే ఒకవేళ(conn_config->revoke_ఉత్తమ_effort) {
      flags |= SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
               SCH_CRED_IGNORE_REVOCATION_OFFLINE |
               SCH_CRED_REVOCATION_CHECK_CHAIN;

      DEBUGF(infof(దత్తాంశం, "schannel: ignore revocation offline errors"));
    }
    లేకపోతే {
      flags |= SCH_CRED_REVOCATION_CHECK_CHAIN;

      DEBUGF(infof(దత్తాంశం, "schannel: checking server certificate revocation"));
    }
  }
  లేకపోతే {
    flags = SCH_CRED_MANUAL_CRED_VALIDATION |
            SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
            SCH_CRED_IGNORE_REVOCATION_OFFLINE;
    DEBUGF(infof(దత్తాంశం, "schannel: disabled server cert revocation checks"));
  }

  ఒకవేళ(!conn_config->verifyhost) {
    flags |= SCH_CRED_NO_SERVERNAME_CHECK;
    DEBUGF(infof(దత్తాంశం, "schannel: verifyhost setting prevents Schannel from "
                       "comparing the supplied target name with the subject "
                       "names in server certificates."));
  }

  ఒకవేళ(!conn_config->auto_client_cert) {
    flags &= ~(DWORD)SCH_CRED_USE_DEFAULT_CREDS;
    flags |= SCH_CRED_NO_DEFAULT_CREDS;
    infof(దత్తాంశం, "schannel: disabled automatic use of client certificate");
  }
  లేకపోతే
    infof(దత్తాంశం, "schannel: enabled automatic use of client certificate");

  DEBUGASSERT(conn_config->సంస్కరణ_2 != CURL_SSLVERSION_DEFAULT);
  ఎంపిక(conn_config->సంస్కరణ_2) {
  స్థితి CURL_SSLVERSION_TLSv1:
  స్థితి CURL_SSLVERSION_TLSv1_0:
  స్థితి CURL_SSLVERSION_TLSv1_1:
  స్థితి CURL_SSLVERSION_TLSv1_2:
  స్థితి CURL_SSLVERSION_TLSv1_3: {
    ఫలితం = schannel_సమితి_ssl_సంస్కరణ_min_max(&enabled_protocols, cf, దత్తాంశం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    ఆపు;
  }
  స్థితి CURL_SSLVERSION_SSLv3:
  స్థితి CURL_SSLVERSION_SSLv2:
    failf(దత్తాంశం, "SSL versions not supported");
    తిరిగి CURLE_NOT_BUILT_లోపల;
  అప్రమేయ:
    failf(దత్తాంశం, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    తిరిగి CURLE_SSL_CONNECT_ERROR;
  }

  ఫలితం = get_client_cert(cf, దత్తాంశం, &client_cert_store, &client_cert);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  /* allocate memory for the reusable credential handle */
  backend->cred = (నిర్మాణం Curl_schannel_cred *)
    curlx_calloc(1, పరిమాణం(నిర్మాణం Curl_schannel_cred));
  ఒకవేళ(!backend->cred) {
    failf(దత్తాంశం, "schannel: unable to allocate memory");

    ఒకవేళ(client_cert)
      CertFreeCertificateContext(client_cert);
    ఒకవేళ(client_cert_store)
      CertCloseStore(client_cert_store, 0);

    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
  backend->cred->refcount = 1;

  /* Since we did not persist the key, we need to extend the store's
   * lifetime until the end of the connection
   */
  backend->cred->client_cert_store = client_cert_store;

  ఫలితం = acquire_sspi_handle(cf, దత్తాంశం, backend, client_cert,
                               flags, enabled_protocols);

  ఒకవేళ(client_cert)
    CertFreeCertificateContext(client_cert);

  తిరిగి ఫలితం;
}

స్థిర CURLcode schannel_connect_step1(నిర్మాణం Curl_cfilter *cf,
                                       నిర్మాణం Curl_easy *దత్తాంశం)
{
  size_t written = 0;
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం schannel_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం schannel_ssl_backend_దత్తాంశం *)connssl->backend;
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  Sec_బఫరు outbuf;
  Sec_బఫరు_Desc outbuf_desc;
  Sec_బఫరు inbuf;
  Sec_బఫరు_Desc inbuf_desc;
  గుర్తులేని అక్షరం alpn_బఫరు[128];
  SECURITY_స్థితి sspi_స్థితి = SEC_E_OK;
  CURLcode ఫలితం;

  DEBUGASSERT(backend);
  DEBUGF(infof(దత్తాంశం, "schannel: SSL/TLS connection with %s port %d (step 1/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->పోర్టు));

  backend->use_alpn = connssl->alpn && s_win_has_alpn;

  ఒకవేళ(conn_config->CAfile || conn_config->ca_info_blob) {
    ఒకవేళ(curlx_verify_windows_సంస్కరణ(6, 1, 0, PLATFORM_WINNT,
                                    సంస్కరణ_GREATER_THAN_EQUAL)) {
      backend->use_manual_cred_validation = TRUE;
    }
    లేకపోతే {
      failf(దత్తాంశం, "schannel: this version of Windows is too old to support "
                  "certificate verification via CA bundle file.");
      తిరిగి CURLE_SSL_CACERT_BADFILE;
    }
  }
  లేకపోతే
    backend->use_manual_cred_validation = FALSE;

  backend->cred = ఖాళీ;

  /* check for an existing reusable credential handle */
  ఒకవేళ(Curl_ssl_scache_use(cf, దత్తాంశం)) {
    నిర్మాణం Curl_schannel_cred *old_cred;
    Curl_ssl_scache_తాళం(దత్తాంశం);
    old_cred = Curl_ssl_scache_get_obj(cf, దత్తాంశం, connssl->peer.scache_కీలకం);
    ఒకవేళ(old_cred) {
      backend->cred = old_cred;
      DEBUGF(infof(దత్తాంశం, "schannel: reusing existing credential handle"));

      /* increment the reference counter of the credential/session handle */
      backend->cred->refcount++;
      DEBUGF(infof(దత్తాంశం,
                   "schannel: incremented credential handle refcount = %d",
                   backend->cred->refcount));
    }
    Curl_ssl_scache_unlock(దత్తాంశం);
  }

  ఒకవేళ(!backend->cred) {
    అక్షరం *snihost;
    ఫలితం = schannel_acquire_credential_handle(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం || !backend->cred)
      తిరిగి ఫలితం;
    /* schannel_acquire_credential_handle() sets backend->cred accordingly or
       it returns error otherwise. */

    /* A hostname associated with the credential is needed by
       InitializeSecurityContext for SNI and other reasons. */
    snihost = connssl->peer.sni ?
      connssl->peer.sni : connssl->peer.origin->hostname;
    backend->cred->sni_hostname = curlx_convert_UTF8_గమ్యం_tchar(snihost);
    ఒకవేళ(!backend->cred->sni_hostname)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  /* Warn if SNI is disabled due to use of an IP address */
  ఒకవేళ(connssl->peer.type != CURL_SSL_PEER_DNS) {
    infof(దత్తాంశం, "schannel: using IP address, SNI is not supported by OS.");
  }

  ఒకవేళ(backend->use_alpn) {
    పూర్ణాంకం cur = 0;
    పూర్ణాంకం list_ప్రారంభం_సూచీ = 0;
    గుర్తులేని పూర్ణాంకం *extension_len = ఖాళీ;
    గుర్తులేని పొట్టి *list_len = ఖాళీ;
    స్థిరరాశి గుర్తులేని పూర్ణాంకం uiALPN = SecApplicationProtocolNegotiationExt_ALPN;
    నిర్మాణం alpn_proto_buf proto;

    /* The first four bytes is an unsigned int indicating number
       of bytes of data in the rest of the buffer. */
    extension_len = (గుర్తులేని పూర్ణాంకం *)(శూన్య *)&alpn_బఫరు[cur];
    cur += (పూర్ణాంకం)పరిమాణం(గుర్తులేని పూర్ణాంకం);

    /* The next four bytes are an indicator that this buffer contains
       ALPN data, as opposed to NPN, for example. */
    జ్ఞాపకం_నకలు(&alpn_బఫరు[cur], &uiALPN, పరిమాణం(uiALPN));
    cur += (పూర్ణాంకం)పరిమాణం(గుర్తులేని పూర్ణాంకం);

    /* The next two bytes is an unsigned short indicating the number
       of bytes used to list the preferred protocols. */
    list_len = (గుర్తులేని పొట్టి *)(శూన్య *)&alpn_బఫరు[cur];
    cur += (పూర్ణాంకం)పరిమాణం(గుర్తులేని పొట్టి);

    list_ప్రారంభం_సూచీ = cur;

    ఫలితం = Curl_alpn_గమ్యం_proto_buf(&proto, connssl->alpn);
    ఒకవేళ(ఫలితం) {
      failf(దత్తాంశం, "Error setting ALPN");
      తిరిగి CURLE_SSL_CONNECT_ERROR;
    }
    జ్ఞాపకం_నకలు(&alpn_బఫరు[cur], proto.దత్తాంశం, proto.len);
    cur += proto.len;

    *list_len = curlx_uitous(cur - list_ప్రారంభం_సూచీ);
    *extension_len = (గుర్తులేని పూర్ణాంకం)(*list_len +
      పరిమాణం(గుర్తులేని పూర్ణాంకం) + పరిమాణం(గుర్తులేని పొట్టి));

    Init_Sec_బఫరు(&inbuf, SECBUFFER_APPLICATION_PROTOCOLS, alpn_బఫరు, cur);
    Init_Sec_బఫరు_Desc(&inbuf_desc, &inbuf, 1);

    Curl_alpn_గమ్యం_proto_str(&proto, connssl->alpn);
    infof(దత్తాంశం, VTLS_INFOF_ALPN_OFFER_1STR, proto.దత్తాంశం);
  }
  లేకపోతే {
    Init_Sec_బఫరు(&inbuf, SECBUFFER_ఖాళీలు, ఖాళీ, 0);
    Init_Sec_బఫరు_Desc(&inbuf_desc, &inbuf, 1);
  }

  /* setup output buffer */
  Init_Sec_బఫరు(&outbuf, SECBUFFER_ఖాళీలు, ఖాళీ, 0);
  Init_Sec_బఫరు_Desc(&outbuf_desc, &outbuf, 1);

  /* security request flags */
  backend->req_flags =
    ISC_REQ_SEQUENCE_DETECT | ISC_REQ_REPLAY_DETECT |
    ISC_REQ_CONFIDENTIALITY | ISC_REQ_ALLOCATE_జ్ఞాపకం |
    ISC_REQ_ప్రవాహం |
    (!conn_config->auto_client_cert ? ISC_REQ_USE_SUPPLIED_CREDS : 0);

  /* allocate memory for the security context handle */
  backend->ctxt = (నిర్మాణం Curl_schannel_ctxt *)
    curlx_calloc(1, పరిమాణం(నిర్మాణం Curl_schannel_ctxt));
  ఒకవేళ(!backend->ctxt) {
    failf(దత్తాంశం, "schannel: unable to allocate memory");
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  /* Schannel InitializeSecurityContext:
     https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-initializesecuritycontextw

     At the moment we do not pass inbuf unless we are using ALPN since we only
     use it for that, and WINE (for which we currently disable ALPN) is giving
     us problems with inbuf regardless. https://github.com/curl/curl/issues/983
   */
  sspi_స్థితి = Curl_pSecFn->InitializeSecurityContext(
    &backend->cred->cred_handle, ఖాళీ, backend->cred->sni_hostname,
    backend->req_flags, 0, 0,
    (backend->use_alpn ? &inbuf_desc : ఖాళీ),
    0, &backend->ctxt->ctxt_handle,
    &outbuf_desc, &backend->ret_flags, ఖాళీ);

  ఒకవేళ(sspi_స్థితి != SEC_I_CONTINUE_NEEDED) {
    అక్షరం బఫరు[STRERROR_LEN];
    curlx_safefree(backend->ctxt);
    ఎంపిక(sspi_స్థితి) {
    స్థితి SEC_E_INSUFFICIENT_జ్ఞాపకం:
      failf(దత్తాంశం, "schannel: initial InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(sspi_స్థితి, బఫరు, పరిమాణం(బఫరు)));
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    స్థితి SEC_E_WRONG_PRINCIPAL:
      failf(దత్తాంశం, "schannel: SNI or certificate check failed: %s",
            Curl_sspi_strerror(sspi_స్థితి, బఫరు, పరిమాణం(బఫరు)));
      తిరిగి CURLE_PEER_FAILED_VERIFICATION;
#ఒకవేళ 0
    స్థితి SEC_E_INVALID_HANDLE:
    స్థితి SEC_E_INVALID_TOKEN:
    స్థితి SEC_E_LOGON_DENIED:
    స్థితి SEC_E_లక్ష్యం_UNKNOWN:
    స్థితి SEC_E_NO_AUTHENTICATING_AUTHORITY:
    స్థితి SEC_E_అంతర్గత_ERROR:
    స్థితి SEC_E_NO_CREDENTIALS:
    స్థితి SEC_E_UNSUPPORTED_FUNCTION:
    స్థితి SEC_E_APPLICATION_PROTOCOL_MISMATCH:
#అయితే_ముగింపు
    అప్రమేయ:
      failf(దత్తాంశం, "schannel: initial InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(sspi_స్థితి, బఫరు, పరిమాణం(బఫరు)));
      తిరిగి CURLE_SSL_CONNECT_ERROR;
    }
  }

  DEBUGF(infof(దత్తాంశం, "schannel: sending initial handshake data: "
               "sending %lu bytes.", outbuf.cb_బఫరు));

  /* send initial handshake data which is now stored in output buffer */
  ఫలితం = Curl_conn_cf_send(cf->తదుపరి, దత్తాంశం,
                             (స్థిరరాశి uint8_t *)outbuf.pv_బఫరు,
                             outbuf.cb_బఫరు, FALSE,
                             &written);
  Curl_pSecFn->Free_Context_బఫరు(outbuf.pv_బఫరు);
  ఒకవేళ(ఫలితం || (outbuf.cb_బఫరు != written)) {
    failf(దత్తాంశం, "schannel: failed to send initial handshake data: "
          "sent %zu of %lu bytes", written, outbuf.cb_బఫరు);
    తిరిగి CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGF(infof(దత్తాంశం, "schannel: sent initial handshake data: "
               "sent %zu bytes", written));

  backend->recv_unrecoverable_err = CURLE_OK;
  backend->recv_sspi_close_notify = FALSE;
  backend->recv_connection_closed = FALSE;
  backend->recv_renegotiating = FALSE;
  backend->renegotiate_state.started = FALSE;
  backend->encdata_is_incomplete = FALSE;
  backend->shutdown_generated = FALSE;
  backend->shutdown_స్థానభ్రంశం = 0;
  memset(&backend->shutdown_token, 0, పరిమాణం(backend->shutdown_token));

  /* continue to second handshake step */
  connssl->connecting_state = ssl_connect_2;

  తిరిగి CURLE_OK;
}

స్థిర CURLcode schannel_error(నిర్మాణం Curl_easy *దత్తాంశం,
                               SECURITY_స్థితి sspi_స్థితి)
{
  అక్షరం బఫరు[STRERROR_LEN];
  ఎంపిక(sspi_స్థితి) {
  స్థితి SEC_E_INSUFFICIENT_జ్ఞాపకం:
    failf(దత్తాంశం, "schannel: next InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(sspi_స్థితి, బఫరు, పరిమాణం(బఫరు)));
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  స్థితి SEC_E_WRONG_PRINCIPAL:
    failf(దత్తాంశం, "schannel: SNI or certificate check failed: %s",
          Curl_sspi_strerror(sspi_స్థితి, బఫరు, పరిమాణం(బఫరు)));
    తిరిగి CURLE_PEER_FAILED_VERIFICATION;
  స్థితి SEC_E_UNTRUSTED_మూలం:
    failf(దత్తాంశం, "schannel: %s",
          Curl_sspi_strerror(sspi_స్థితి, బఫరు, పరిమాణం(బఫరు)));
    తిరిగి CURLE_PEER_FAILED_VERIFICATION;
#ఒకవేళ 0
  స్థితి SEC_E_INVALID_HANDLE:
  స్థితి SEC_E_INVALID_TOKEN:
  స్థితి SEC_E_LOGON_DENIED:
  స్థితి SEC_E_లక్ష్యం_UNKNOWN:
  స్థితి SEC_E_NO_AUTHENTICATING_AUTHORITY:
  స్థితి SEC_E_అంతర్గత_ERROR:
  స్థితి SEC_E_NO_CREDENTIALS:
  స్థితి SEC_E_UNSUPPORTED_FUNCTION:
  స్థితి SEC_E_APPLICATION_PROTOCOL_MISMATCH:
#అయితే_ముగింపు
  అప్రమేయ:
    failf(దత్తాంశం, "schannel: next InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(sspi_స్థితి, బఫరు, పరిమాణం(బఫరు)));
    తిరిగి CURLE_SSL_CONNECT_ERROR;
  }
}

స్థిర CURLcode schannel_pkp_pin_peer_pubkey(నిర్మాణం Curl_cfilter *cf,
                                             నిర్మాణం Curl_easy *దత్తాంశం,
                                             స్థిరరాశి అక్షరం *pinnedpubkey)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం schannel_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం schannel_ssl_backend_దత్తాంశం *)connssl->backend;
  CERT_CONTEXT *pCertContextServer = ఖాళీ;

  /* Result is returned to caller */
  CURLcode ఫలితం = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  DEBUGASSERT(backend);

  /* if a path was not specified, do not pin */
  ఒకవేళ(!pinnedpubkey)
    తిరిగి CURLE_OK;

  చేయి {
    SECURITY_స్థితి sspi_స్థితి;
    uint8_t *x509_der;
    DWORD x509_der_len;
    నిర్మాణం Curl_X509certificate x509_parsed;
    నిర్మాణం Curl_asn1Element *pubkey;

    sspi_స్థితి =
      Curl_pSecFn->Query_Context_లక్షణాలు(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                          &pCertContextServer);

    ఒకవేళ((sspi_స్థితి != SEC_E_OK) || !pCertContextServer) {
      అక్షరం బఫరు[STRERROR_LEN];
      failf(దత్తాంశం, "schannel: Failed to read remote certificate context: %s",
            Curl_sspi_strerror(sspi_స్థితి, బఫరు, పరిమాణం(బఫరు)));
      ఆపు; /* failed */
    }

    ఒకవేళ(!(((pCertContextServer->dwCertEncodingType & X509_ASN_ENCODING) != 0) &&
         (pCertContextServer->cbCertEncoded > 0)))
      ఆపు;

    x509_der = (uint8_t *)pCertContextServer->pbCertEncoded;
    x509_der_len = pCertContextServer->cbCertEncoded;
    memset(&x509_parsed, 0, పరిమాణం(x509_parsed));
    ఒకవేళ(Curl_parseX509(&x509_parsed, x509_der, x509_der + x509_der_len))
      ఆపు;

    pubkey = &x509_parsed.subject_ప్రజా_కీలకం_Info;
    ఒకవేళ(!pubkey->శీర్షిక || pubkey->చివర <= pubkey->శీర్షిక) {
      failf(దత్తాంశం, "SSL: failed retrieving public key from server certificate");
      ఆపు;
    }

    ఫలితం = Curl_pin_peer_pubkey(దత్తాంశం,
                                  pinnedpubkey,
                                  (స్థిరరాశి గుర్తులేని అక్షరం *)pubkey->శీర్షిక,
                                  (size_t)(pubkey->చివర - pubkey->శీర్షిక));
    ఒకవేళ(ఫలితం) {
      failf(దత్తాంశం, "SSL: public key does not match pinned public key");
    }
  } అయితే(0);

  ఒకవేళ(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  తిరిగి ఫలితం;
}

స్థిర CURLcode ensure_encoding_కొలత(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం sbuffer *encdata,
                                     size_t min_నిడివి)
{
  size_t కొలత;
  DEBUGASSERT(encdata->నిడివి >= encdata->స్థానభ్రంశం);
  ఒకవేళ(encdata->నిడివి < encdata->స్థానభ్రంశం)
    తిరిగి CURLE_FAILED_INIT;
  కొలత = encdata->నిడివి - encdata->స్థానభ్రంశం;
  ఒకవేళ(కొలత < CURL_SCHANNEL_బఫరు_FREE_కొలత || encdata->నిడివి < min_నిడివి) {
    గుర్తులేని అక్షరం *బఫరు;
    size_t నిడివి = encdata->స్థానభ్రంశం + CURL_SCHANNEL_బఫరు_FREE_కొలత;
    ఒకవేళ(నిడివి < min_నిడివి)
      నిడివి = min_నిడివి;
    బఫరు = curlx_realloc(encdata->బఫరు, నిడివి);
    ఒకవేళ(!బఫరు) {
      failf(దత్తాంశం, "schannel: unable to re-allocate memory");
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }

    encdata->బఫరు = బఫరు;
    encdata->నిడివి = నిడివి;
    SCH_DEV(infof(దత్తాంశం, "schannel: encdata.buffer resized %zu",
                  encdata->నిడివి));
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ensure_decoding_కొలత(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం sbuffer *decdata,
                                     size_t nowsize,
                                     size_t len)
{
  size_t కొలత = nowsize > CURL_SCHANNEL_బఫరు_FREE_కొలత ?
    nowsize : CURL_SCHANNEL_బఫరు_FREE_కొలత;
  DEBUGASSERT(decdata->నిడివి >= decdata->స్థానభ్రంశం);
  ఒకవేళ(decdata->నిడివి < decdata->స్థానభ్రంశం)
    తిరిగి CURLE_FAILED_INIT;
  లేకపోతే ఒకవేళ(decdata->నిడివి - decdata->స్థానభ్రంశం < కొలత ||
          decdata->నిడివి < len) {
    /* increase internal decrypted data buffer */
    size_t నిడివి = decdata->స్థానభ్రంశం + కొలత;
    గుర్తులేని అక్షరం *బఫరు;
    /* make sure that the requested amount of data fits */
    ఒకవేళ(నిడివి < len)
      నిడివి = len;

    బఫరు = curlx_realloc(decdata->బఫరు, నిడివి);
    ఒకవేళ(!బఫరు) {
      failf(దత్తాంశం, "schannel: unable to re-allocate memory");
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
    decdata->బఫరు = బఫరు;
    decdata->నిడివి = నిడివి;
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode schannel_connect_step2(నిర్మాణం Curl_cfilter *cf,
                                       నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం schannel_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం schannel_ssl_backend_దత్తాంశం *)connssl->backend;
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  పూర్ణాంకం i;
  size_t nread = 0;
  Sec_బఫరు outbuf[3];
  Sec_బఫరు_Desc outbuf_desc;
  Sec_బఫరు inbuf[2];
  Sec_బఫరు_Desc inbuf_desc;
  SECURITY_స్థితి sspi_స్థితి = SEC_E_OK;
  CURLcode ఫలితం;
  bool doread;
  స్థిరరాశి అక్షరం *pubkey_ptr;

  DEBUGASSERT(backend);

  doread = (connssl->io_need & CURL_SSL_IO_NEED_SEND) ? FALSE : TRUE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  DEBUGF(infof(దత్తాంశం, "schannel: SSL/TLS connection with %s port %d (step 2/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->పోర్టు));

  ఒకవేళ(!backend->cred || !backend->ctxt)
    తిరిగి CURLE_SSL_CONNECT_ERROR;

  /* buffer to store previously received and decrypted data */
  ఒకవేళ(!backend->decdata.బఫరు) {
    backend->decdata.స్థానభ్రంశం = 0;
    backend->decdata.నిడివి = CURL_SCHANNEL_బఫరు_INIT_కొలత;
    backend->decdata.బఫరు = curlx_malloc(backend->decdata.నిడివి);
    ఒకవేళ(!backend->decdata.బఫరు) {
      failf(దత్తాంశం, "schannel: unable to allocate memory");
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
  }

  /* buffer to store previously received and encrypted data */
  ఒకవేళ(!backend->encdata.బఫరు) {
    backend->encdata_is_incomplete = FALSE;
    backend->encdata.స్థానభ్రంశం = 0;
    backend->encdata.నిడివి = CURL_SCHANNEL_బఫరు_INIT_కొలత;
    backend->encdata.బఫరు = curlx_malloc(backend->encdata.నిడివి);
    ఒకవేళ(!backend->encdata.బఫరు) {
      failf(దత్తాంశం, "schannel: unable to allocate memory");
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
  }

  ఫలితం = ensure_encoding_కొలత(దత్తాంశం, &backend->encdata, 0);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  కొరకు(;;) {
    ఒకవేళ(doread) {
      /* read encrypted handshake data from socket */
      ఫలితం = Curl_conn_cf_recv(cf->తదుపరి, దత్తాంశం,
                                 (అక్షరం *)(backend->encdata.బఫరు +
                                          backend->encdata.స్థానభ్రంశం),
                                 backend->encdata.నిడివి -
                                 backend->encdata.స్థానభ్రంశం,
                                 &nread);
      ఒకవేళ(ఫలితం == CURLE_AGAIN) {
        ఒకవేళ(!backend->encdata.స్థానభ్రంశం || backend->encdata_is_incomplete) {
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          DEBUGF(infof(దత్తాంశం, "schannel: failed to receive handshake, "
                       "need more data"));
          తిరిగి CURLE_OK;
        }
        లేకపోతే {
          DEBUGF(infof(దత్తాంశం, "schannel: no new handshake data received, "
                       "continuing to process existing handshake data"));
        }
      }
      లేకపోతే ఒకవేళ(ఫలితం || (nread == 0)) {
        failf(దత్తాంశం, "schannel: failed to receive handshake, "
              "SSL/TLS connection failed");
        తిరిగి CURLE_SSL_CONNECT_ERROR;
      }
      లేకపోతే {
        /* increase encrypted data buffer offset */
        backend->encdata.స్థానభ్రంశం += nread;
        backend->encdata_is_incomplete = FALSE;
        SCH_DEV(infof(దత్తాంశం, "schannel: encrypted data got %zu", nread));
      }
    }

    SCH_DEV(infof(దత్తాంశం,
                  "schannel: encrypted data buffer: offset %zu length %zu",
                  backend->encdata.స్థానభ్రంశం, backend->encdata.నిడివి));

    /* setup input buffers */
    Init_Sec_బఫరు(&inbuf[0], SECBUFFER_TOKEN,
                  curlx_memdup0((స్థిరరాశి అక్షరం *)backend->encdata.బఫరు,
                                backend->encdata.స్థానభ్రంశం),
                  curlx_uztoul(backend->encdata.స్థానభ్రంశం));
    Init_Sec_బఫరు(&inbuf[1], SECBUFFER_ఖాళీలు, ఖాళీ, 0);
    Init_Sec_బఫరు_Desc(&inbuf_desc, inbuf, 2);

    /* setup output buffers */
    Init_Sec_బఫరు(&outbuf[0], SECBUFFER_TOKEN, ఖాళీ, 0);
    Init_Sec_బఫరు(&outbuf[1], SECBUFFER_ALERT, ఖాళీ, 0);
    Init_Sec_బఫరు(&outbuf[2], SECBUFFER_ఖాళీలు, ఖాళీ, 0);
    Init_Sec_బఫరు_Desc(&outbuf_desc, outbuf, 3);

    ఒకవేళ(!inbuf[0].pv_బఫరు) {
      failf(దత్తాంశం, "schannel: unable to allocate memory");
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }

    /* The socket must be writable (or a poll error occurred) before we call
       InitializeSecurityContext to continue processing the received TLS
       records. This is because that function is not idempotent and we do not
       support partial save/resume sending replies of handshake tokens. */
    ఒకవేళ(!SOCKET_WRITABLE(Curl_conn_cf_get_socket(cf, దత్తాంశం), 0)) {
      SCH_DEV(infof(దత్తాంశం, "schannel: handshake waiting for writable socket"));
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      curlx_free(inbuf[0].pv_బఫరు);
      తిరిగి CURLE_OK;
    }

    sspi_స్థితి = Curl_pSecFn->InitializeSecurityContext(
      &backend->cred->cred_handle, &backend->ctxt->ctxt_handle,
      backend->cred->sni_hostname, backend->req_flags,
      0, 0, &inbuf_desc, 0, ఖాళీ,
      &outbuf_desc, &backend->ret_flags, ఖాళీ);

    /* free buffer for received handshake data */
    curlx_safefree(inbuf[0].pv_బఫరు);

    /* check if the handshake was incomplete */
    ఎంపిక(sspi_స్థితి) {
    స్థితి SEC_E_INCOMPLETE_సందేశం:
      backend->encdata_is_incomplete = TRUE;
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      DEBUGF(infof(దత్తాంశం,
                   "schannel: received incomplete message, need more data"));
      తిరిగి CURLE_OK;

    స్థితి SEC_I_CONTINUE_NEEDED:
    స్థితి SEC_E_OK:
      /* check if the handshake needs to be continued */
      ఫలితం = CURLE_OK;
      కొరకు(i = 0; i < 3; i++) {
        /* search for handshake tokens that need to be sent */
        ఒకవేళ(outbuf[i].బఫరు_Type == SECBUFFER_TOKEN && outbuf[i].cb_బఫరు > 0) {
          size_t written = 0;
          DEBUGF(infof(దత్తాంశం, "schannel: sending next handshake data: "
                       "sending %lu bytes.", outbuf[i].cb_బఫరు));

          /* send handshake token to server */
          ఫలితం = Curl_conn_cf_send(cf->తదుపరి, దత్తాంశం,
                                     (స్థిరరాశి uint8_t *)outbuf[i].pv_బఫరు,
                                     outbuf[i].cb_బఫరు,
                                     FALSE, &written);
          ఒకవేళ(ఫలితం || (outbuf[i].cb_బఫరు != written)) {
            failf(దత్తాంశం, "schannel: failed to send next handshake data: "
                  "sent %zu of %lu bytes", written, outbuf[i].cb_బఫరు);
            ఫలితం = CURLE_SSL_CONNECT_ERROR;
          }
        }
      }
      కొరకు(i = 0; i < 3; i++) {
        /* free obsolete buffer */
        ఒకవేళ(outbuf[i].pv_బఫరు)
          Curl_pSecFn->Free_Context_బఫరు(outbuf[i].pv_బఫరు);
      }
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
      ఆపు;

    స్థితి SEC_I_INCOMPLETE_CREDENTIALS:
      ఒకవేళ(!(backend->req_flags & ISC_REQ_USE_SUPPLIED_CREDS)) {
        /* If the server has requested a client certificate, attempt to
           continue the handshake without one. This allows connections to
           servers which request a client certificate but do not require
           it. */
        backend->req_flags |= ISC_REQ_USE_SUPPLIED_CREDS;
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        DEBUGF(infof(దత్తాంశం,
                     "schannel: a client certificate has been requested"));
        తిరిగి CURLE_OK;
      }
      FALLTHROUGH();

    అప్రమేయ:
      తిరిగి schannel_error(దత్తాంశం, sspi_స్థితి);
    }

    /* check if there was additional remaining encrypted data */
    ఒకవేళ(inbuf[1].బఫరు_Type == SECBUFFER_EXTRA && inbuf[1].cb_బఫరు > 0) {
      SCH_DEV(infof(దత్తాంశం, "schannel: encrypted data length: %lu",
                    inbuf[1].cb_బఫరు));
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
      ఒకవేళ(backend->encdata.స్థానభ్రంశం > inbuf[1].cb_బఫరు) {
        memmove(backend->encdata.బఫరు,
                (backend->encdata.బఫరు + backend->encdata.స్థానభ్రంశం) -
                inbuf[1].cb_బఫరు, inbuf[1].cb_బఫరు);
        backend->encdata.స్థానభ్రంశం = inbuf[1].cb_బఫరు;
        ఒకవేళ(sspi_స్థితి == SEC_I_CONTINUE_NEEDED) {
          doread = FALSE;
          కొనసాగు;
        }
      }
    }
    లేకపోతే {
      backend->encdata.స్థానభ్రంశం = 0;
    }
    ఆపు;
  }

  /* check if the handshake needs to be continued */
  ఒకవేళ(sspi_స్థితి == SEC_I_CONTINUE_NEEDED) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    తిరిగి CURLE_OK;
  }

  /* check if the handshake is complete */
  ఒకవేళ(sspi_స్థితి == SEC_E_OK) {
    connssl->connecting_state = ssl_connect_3;
    DEBUGF(infof(దత్తాంశం, "schannel: SSL/TLS handshake complete"));
  }

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  pubkey_ptr = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(దత్తాంశం, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(దత్తాంశం, STRING_SSL_PINNEDPUBLICKEY);
#లేకపోతే
  pubkey_ptr = CURL_EASY_STR(దత్తాంశం, STRING_SSL_PINNEDPUBLICKEY);
#అయితే_ముగింపు
  ఒకవేళ(pubkey_ptr) {
    ఫలితం = schannel_pkp_pin_peer_pubkey(cf, దత్తాంశం, pubkey_ptr);
    ఒకవేళ(ఫలితం) {
      failf(దత్తాంశం, "SSL: public key does not match pinned public key");
      తిరిగి ఫలితం;
    }
  }

  ఒకవేళ(conn_config->verifypeer && backend->use_manual_cred_validation) {
    /* Certificate verification also verifies the hostname if verifyhost */
    తిరిగి Curl_verify_certificate(cf, దత్తాంశం);
  }

  /* Verify the hostname manually when certificate verification is disabled,
     because in that case Schannel does not verify it. */
  ఒకవేళ(!conn_config->verifypeer && conn_config->verifyhost)
    తిరిగి Curl_verify_host(cf, దత్తాంశం);

  తిరిగి CURLE_OK;
}

స్థిర bool valid_cert_encoding(స్థిరరాశి CERT_CONTEXT *cert_context)
{
  తిరిగి cert_context &&
    ((cert_context->dwCertEncodingType & X509_ASN_ENCODING) != 0) &&
    cert_context->pbCertEncoded &&
    (cert_context->cbCertEncoded > 0);
}

రకనిర్వచనం bool (*Read_crt_func)(స్థిరరాశి CERT_CONTEXT *ccert_context,
                              bool తిరగేయి_క్రమం, శూన్య *arg);

స్థిర శూన్య traverse_cert_store(స్థిరరాశి CERT_CONTEXT *context,
                                Read_crt_func func, శూన్య *arg)
{
  స్థిరరాశి CERT_CONTEXT *current_context = ఖాళీ;
  bool should_continue = TRUE;
  bool మొదటి = TRUE;
  bool తిరగేయి_క్రమం = FALSE;
  అయితే(should_continue &&
        (current_context = Cert_Enum_Certificates_లోపల_Store(
          context->hCertStore,
          current_context)) != ఖాళీ) {
    /* Windows 11 22H2 OS Build 22621.674 or higher enumerates certificates in
       leaf-to-root order while all previous versions of Windows enumerate
       certificates in root-to-leaf order. Determine the order of enumeration
       by comparing SECPKG_ATTR_REMOTE_CERT_CONTEXT's pbCertContext with the
       first certificate's pbCertContext. */
    ఒకవేళ(మొదటి && context->pbCertEncoded != current_context->pbCertEncoded)
      తిరగేయి_క్రమం = TRUE;
    should_continue = func(current_context, తిరగేయి_క్రమం, arg);
    మొదటి = FALSE;
  }

  ఒకవేళ(current_context)
    CertFreeCertificateContext(current_context);
}

స్థిర bool cert_counter_callback(స్థిరరాశి CERT_CONTEXT *ccert_context,
                                  bool తిరగేయి_క్రమం, శూన్య *certs_లెక్క)
{
  (శూన్య)తిరగేయి_క్రమం;
  ఒకవేళ(valid_cert_encoding(ccert_context))
    (*(పూర్ణాంకం *)certs_లెక్క)++;
  ఒకవేళ(*(పూర్ణాంకం *)certs_లెక్క > MAX_ALLOWED_CERT_AMOUNT)
    తిరిగి FALSE;
  తిరిగి TRUE;
}

నిర్మాణం Adder_వాదనలు {
  నిర్మాణం Curl_easy *దత్తాంశం;
  CURLcode ఫలితం;
  పూర్ణాంకం idx;
  పూర్ణాంకం certs_లెక్క;
};

స్థిర bool కూడు_cert_గమ్యం_certinfo(స్థిరరాశి CERT_CONTEXT *ccert_context,
                                 bool తిరగేయి_క్రమం, శూన్య *raw_arg)
{
  నిర్మాణం Adder_వాదనలు *వాదనలు = (నిర్మాణం Adder_వాదనలు *)raw_arg;
  వాదనలు->ఫలితం = CURLE_OK;
  ఒకవేళ(valid_cert_encoding(ccert_context)) {
    పూర్ణాంకం చొప్పించు_సూచీ = తిరగేయి_క్రమం ? (వాదనలు->certs_లెక్క - 1) - వాదనలు->idx :
                       వాదనలు->idx;
    వాదనలు->ఫలితం = Curl_వెలికితీయి_certinfo(వాదనలు->దత్తాంశం, చొప్పించు_సూచీ,
                                         ccert_context->pbCertEncoded,
                                         ccert_context->pbCertEncoded +
                                         ccert_context->cbCertEncoded);
    వాదనలు->idx++;
  }
  తిరిగి వాదనలు->ఫలితం == CURLE_OK;
}

స్థిర శూన్య schannel_session_free(శూన్య *sessionid)
{
  /* this is expected to be called under sessionid lock */
  నిర్మాణం Curl_schannel_cred *cred = sessionid;

  ఒకవేళ(cred) {
    cred->refcount--;
    ఒకవేళ(cred->refcount == 0) {
      Curl_pSecFn->FreeCredentialsHandle(&cred->cred_handle);
      curlx_free(cred->sni_hostname);
      ఒకవేళ(cred->client_cert_store) {
        CertCloseStore(cred->client_cert_store, 0);
        cred->client_cert_store = ఖాళీ;
      }
      curlx_safefree(cred);
    }
  }
}

స్థిర CURLcode schannel_connect_step3(నిర్మాణం Curl_cfilter *cf,
                                       నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం schannel_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం schannel_ssl_backend_దత్తాంశం *)connssl->backend;
  నిర్మాణం ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, దత్తాంశం);
  CURLcode ఫలితం = CURLE_OK;
  SECURITY_స్థితి sspi_స్థితి = SEC_E_OK;
  CERT_CONTEXT *ccert_context = ఖాళీ;
  SecPkgContext_ApplicationProtocol alpn_ఫలితం;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);
  DEBUGASSERT(backend);

  DEBUGF(infof(దత్తాంశం, "schannel: SSL/TLS connection with %s port %d (step 3/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->పోర్టు));

  ఒకవేళ(!backend->cred)
    తిరిగి CURLE_SSL_CONNECT_ERROR;

  /* check if the required context attributes are met */
  ఒకవేళ(backend->ret_flags != backend->req_flags) {
    ఒకవేళ(!(backend->ret_flags & ISC_RET_SEQUENCE_DETECT))
      failf(దత్తాంశం, "schannel: failed to setup sequence detection");
    ఒకవేళ(!(backend->ret_flags & ISC_RET_REPLAY_DETECT))
      failf(దత్తాంశం, "schannel: failed to setup replay detection");
    ఒకవేళ(!(backend->ret_flags & ISC_RET_CONFIDENTIALITY))
      failf(దత్తాంశం, "schannel: failed to setup confidentiality");
    ఒకవేళ(!(backend->ret_flags & ISC_RET_ALLOCATED_జ్ఞాపకం))
      failf(దత్తాంశం, "schannel: failed to setup memory allocation");
    ఒకవేళ(!(backend->ret_flags & ISC_RET_ప్రవాహం))
      failf(దత్తాంశం, "schannel: failed to setup stream orientation");
    తిరిగి CURLE_SSL_CONNECT_ERROR;
  }

  ఒకవేళ(backend->use_alpn) {
    sspi_స్థితి =
      Curl_pSecFn->Query_Context_లక్షణాలు(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_APPLICATION_PROTOCOL,
                                          &alpn_ఫలితం);

    ఒకవేళ(sspi_స్థితి != SEC_E_OK) {
      failf(దత్తాంశం, "schannel: failed to retrieve ALPN result");
      తిరిగి CURLE_SSL_CONNECT_ERROR;
    }

    ఒకవేళ(alpn_ఫలితం.Proto_Nego_స్థితి ==
       Sec_Application_Protocol_Negotiation_స్థితి_Success) {
      ఒకవేళ(backend->recv_renegotiating &&
         connssl->negotiated.alpn &&
         అక్షరశ్రేణి_పోలిక(connssl->negotiated.alpn,
                 (స్థిరరాశి అక్షరం *)alpn_ఫలితం.ProtocolId,
                 alpn_ఫలితం.Protocol_Id_కొలత)) {
        /* Renegotiation selected a different protocol now, we cannot
         * deal with this */
        failf(దత్తాంశం, "schannel: server selected an ALPN protocol too late");
        తిరిగి CURLE_SSL_CONNECT_ERROR;
      }
      Curl_alpn_సమితి_negotiated(cf, దత్తాంశం, connssl, alpn_ఫలితం.ProtocolId,
                               alpn_ఫలితం.Protocol_Id_కొలత);
    }
    లేకపోతే {
      ఒకవేళ(!backend->recv_renegotiating)
        Curl_alpn_సమితి_negotiated(cf, దత్తాంశం, connssl, ఖాళీ, 0);
    }
  }

  /* save the current session data for possible reuse */
  ఒకవేళ(Curl_ssl_scache_use(cf, దత్తాంశం)) {
    Curl_ssl_scache_తాళం(దత్తాంశం);
    /* Up ref count since call takes ownership */
    backend->cred->refcount++;
    ఫలితం = Curl_ssl_scache_కూడు_obj(cf, దత్తాంశం, connssl->peer.scache_కీలకం,
                                     backend->cred, schannel_session_free);
    Curl_ssl_scache_unlock(దత్తాంశం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  ఒకవేళ(ssl_config->certinfo) {
    పూర్ణాంకం certs_లెక్క = 0;
    sspi_స్థితి =
      Curl_pSecFn->Query_Context_లక్షణాలు(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                          &ccert_context);

    ఒకవేళ((sspi_స్థితి != SEC_E_OK) || !ccert_context) {
      failf(దత్తాంశం, "schannel: failed to retrieve remote cert context");
      తిరిగి CURLE_PEER_FAILED_VERIFICATION;
    }

    traverse_cert_store(ccert_context, cert_counter_callback, &certs_లెక్క);
    ఒకవేళ(certs_లెక్క > MAX_ALLOWED_CERT_AMOUNT) {
      failf(దత్తాంశం, "%d certificates is more than allowed (%d)",
            certs_లెక్క, MAX_ALLOWED_CERT_AMOUNT);
      CertFreeCertificateContext(ccert_context);
      తిరిగి CURLE_SSL_CONNECT_ERROR;
    }

    ఫలితం = Curl_ssl_init_certinfo(దత్తాంశం, certs_లెక్క);
    ఒకవేళ(!ఫలితం) {
      నిర్మాణం Adder_వాదనలు వాదనలు;
      వాదనలు.దత్తాంశం = దత్తాంశం;
      వాదనలు.idx = 0;
      వాదనలు.certs_లెక్క = certs_లెక్క;
      వాదనలు.ఫలితం = CURLE_OK;
      traverse_cert_store(ccert_context, కూడు_cert_గమ్యం_certinfo, &వాదనలు);
      ఫలితం = వాదనలు.ఫలితం;
    }
    CertFreeCertificateContext(ccert_context);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  connssl->connecting_state = ssl_connect_పూర్తి;

  తిరిగి CURLE_OK;
}

స్థిర CURLcode schannel_connect(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 bool *పూర్తి)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  CURLcode ఫలితం;

  /* check if the connection has already been established */
  ఒకవేళ(ssl_connection_complete == connssl->state) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  *పూర్తి = FALSE;

  ఒకవేళ(connssl->connecting_state == ssl_connect_1) {
    ఫలితం = schannel_connect_step1(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  ఒకవేళ(connssl->connecting_state == ssl_connect_2) {
    ఫలితం = schannel_connect_step2(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  ఒకవేళ(connssl->connecting_state == ssl_connect_3) {
    ఫలితం = schannel_connect_step3(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  ఒకవేళ(connssl->connecting_state == ssl_connect_పూర్తి) {
    నిర్మాణం schannel_ssl_backend_దత్తాంశం *backend =
      (నిర్మాణం schannel_ssl_backend_దత్తాంశం *)connssl->backend;
    DEBUGASSERT(backend);

    ఒకవేళ(Curl_pSecFn->Query_Context_లక్షణాలు(
         &backend->ctxt->ctxt_handle,
         SECPKG_ATTR_ప్రవాహం_SIZES,
         &backend->ప్రవాహం_sizes)) {
      failf(దత్తాంశం, "schannel: failed getting stream sizes");
      తిరిగి CURLE_SSL_CONNECT_ERROR;
    }

    connssl->state = ssl_connection_complete;

    /* When SSPI is used in combination with Schannel
     * we need the Schannel context to create the Schannel
     * binding to pass the IIS extended protection checks.
     * Available on Windows 7 or later.
     */
    cf->conn->sslContext = &backend->ctxt->ctxt_handle;

    *పూర్తి = TRUE;
  }

  తిరిగి CURLE_OK;
}

గణన schannel_renegotiate_caller_t {
  SCH_RENEG_CALLER_IS_RECV,
  SCH_RENEG_CALLER_IS_SEND
};

/* The maximum time we allow for Schannel renegotiation which may in some
   rare cases block either due to libcurl (waiting on the socket) or Windows
   (waiting on an interactive security prompt). Note Schannel "renegotiation"
   is not necessarily literal TLS renegotiation, but means DecryptMessage
   returned SEC_I_RENEGOTIATE which means at least the security context needs
   to be re-established. */
#నిర్వచించు MAX_RENEG_ఖండం_సమయం (60 * 1000) /* 60 seconds in milliseconds */

/* This function renegotiates the connection due to a server request received
   by schannel_recv. This function returns CURLE_AGAIN if the renegotiation is
   incomplete. In that case, we remain in the renegotiation (connecting) stage
   and future calls to schannel_recv and schannel_send must call this function
   first to complete the renegotiation. */
స్థిర CURLcode schannel_recv_renegotiate(
  నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
  గణన schannel_renegotiate_caller_t caller)
{
  CURLcode ఫలితం;
  curl_socket_t sockfd;
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం schannel_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం schannel_ssl_backend_దత్తాంశం *)connssl->backend;
  నిర్మాణం schannel_renegotiate_state *rs = &backend->renegotiate_state;

  ఒకవేళ(!backend || !backend->recv_renegotiating) {
    failf(దత్తాంశం, "schannel: unexpected call to schannel_recv_renegotiate");
    తిరిగి CURLE_SSL_CONNECT_ERROR;
  }
  DEBUGASSERT(caller <= SCH_RENEG_CALLER_IS_SEND);
  ఒకవేళ(caller == SCH_RENEG_CALLER_IS_RECV)
    SCH_DEV(infof(దత్తాంశం, "schannel: renegotiation caller is schannel_recv"));
  లేకపోతే
    SCH_DEV(infof(దత్తాంశం, "schannel: renegotiation caller is schannel_send"));

  sockfd = Curl_conn_cf_get_socket(cf, దత్తాంశం);

  ఒకవేళ(sockfd == CURL_SOCKET_BAD) {
    failf(దత్తాంశం, "schannel: renegotiation missing socket");
    తిరిగి CURLE_SSL_CONNECT_ERROR;
  }

  ఒకవేళ(!rs->started) { /* new renegotiation */
    infof(దత్తాంశం, "schannel: renegotiating SSL/TLS connection");
    DEBUGASSERT(connssl->state == ssl_connection_complete);
    DEBUGASSERT(connssl->connecting_state == ssl_connect_పూర్తి);
    connssl->state = ssl_connection_negotiating;
    connssl->connecting_state = ssl_connect_2;
    memset(rs, 0, పరిమాణం(*rs));
    rs->io_need = CURL_SSL_IO_NEED_SEND;
    rs->ప్రారంభం_సమయం = *Curl_pgrs_now(దత్తాంశం);
    rs->started = TRUE;
  }

  కొరకు(;;) {
    bool ఖండం_read, ఖండం_write, blocking, పూర్తి;
    curl_socket_t readfd, writefd;
    timediff_t elapsed;

    elapsed = curlx_ptimediff_ms(Curl_pgrs_now(దత్తాంశం), &rs->ప్రారంభం_సమయం);
    ఒకవేళ(elapsed >= MAX_RENEG_ఖండం_సమయం) {
      failf(దత్తాంశం, "schannel: renegotiation timeout");
      ఫలితం = CURLE_SSL_CONNECT_ERROR;
      ఆపు;
    }

    /* the current io_need state may have been overwritten since the last time
       this function was called. restore the io_need state needed to continue
       the renegotiation. */

    connssl->io_need = rs->io_need;

    ఫలితం = schannel_connect(cf, దత్తాంశం, &పూర్తి);

    rs->io_need = connssl->io_need;

    ఒకవేళ(!ఫలితం && !పూర్తి)
      ఫలితం = CURLE_AGAIN;

    ఒకవేళ(ఫలితం != CURLE_AGAIN)
      ఆపు;

    readfd = (rs->io_need & CURL_SSL_IO_NEED_RECV) ? sockfd : CURL_SOCKET_BAD;
    writefd = (rs->io_need & CURL_SSL_IO_NEED_SEND) ? sockfd : CURL_SOCKET_BAD;

    ఒకవేళ(readfd == CURL_SOCKET_BAD && writefd == CURL_SOCKET_BAD)
      కొనసాగు;

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

    ఖండం_read = (caller == SCH_RENEG_CALLER_IS_SEND) ? TRUE : FALSE;
    ఖండం_write = (caller == SCH_RENEG_CALLER_IS_RECV) ? TRUE : FALSE;

    blocking = (ఖండం_read && (readfd != CURL_SOCKET_BAD)) ||
               (ఖండం_write && (writefd != CURL_SOCKET_BAD));

    SCH_DEV_SHOWBOOL(ఖండం_read);
    SCH_DEV_SHOWBOOL(ఖండం_write);
    SCH_DEV_SHOWBOOL(blocking);

    కొరకు(;;) {
      పూర్ణాంకం what;
      timediff_t timeout_ms, మిగిలిన;

      ఫలితం = Curl_pgrsUpdate(దత్తాంశం);
      ఒకవేళ(ఫలితం)
        ఆపు;

      elapsed = curlx_ptimediff_ms(Curl_pgrs_now(దత్తాంశం), &rs->ప్రారంభం_సమయం);
      ఒకవేళ(elapsed >= MAX_RENEG_ఖండం_సమయం) {
        failf(దత్తాంశం, "schannel: renegotiation timeout");
        ఫలితం = CURLE_SSL_CONNECT_ERROR;
        ఆపు;
      }
      మిగిలిన = MAX_RENEG_ఖండం_సమయం - elapsed;

      ఒకవేళ(blocking) {
        timeout_ms = Curl_timeleft_ms(దత్తాంశం);

        ఒకవేళ(timeout_ms < 0) {
          ఫలితం = CURLE_OPERATION_TIMEDOUT;
          ఆపు;
        }

        /* the blocking is in intervals so that the progress function can be
           called every second */
        ఒకవేళ(!timeout_ms || timeout_ms > 1000)
          timeout_ms = 1000;

        ఒకవేళ(timeout_ms > మిగిలిన)
          timeout_ms = మిగిలిన;
      }
      లేకపోతే
        timeout_ms = 0;

      SCH_DEV(infof(దత్తాంశం, "schannel: renegotiation wait until socket is"
                    "%s%s for up to %" FMT_TIMEDIFF_T " ms",
                    ((readfd != CURL_SOCKET_BAD) ? " readable" : ""),
                    ((writefd != CURL_SOCKET_BAD) ? " writable" : ""),
                    timeout_ms));

      what = Curl_socket_check(readfd, CURL_SOCKET_BAD, writefd, timeout_ms);

      ఒకవేళ(what > 0 && (what & (CURL_CSELECT_లోపల | CURL_CSELECT_బహిర్గతం))) {
        SCH_DEV(infof(దత్తాంశం, "schannel: renegotiation socket %s%s",
                      ((what & CURL_CSELECT_లోపల) ? "CURL_CSELECT_IN " : ""),
                      ((what & CURL_CSELECT_బహిర్గతం) ? "CURL_CSELECT_OUT " : "")));
        ఫలితం = CURLE_AGAIN;
        ఆపు;
      }
      లేకపోతే ఒకవేళ(!what) {
        SCH_DEV(infof(దత్తాంశం, "schannel: renegotiation socket timeout"));
        ఒకవేళ(blocking)
          కొనసాగు;
        లేకపోతే
          తిరిగి CURLE_AGAIN;
      }

      failf(దత్తాంశం, "schannel: socket error during renegotiation");
      ఫలితం = CURLE_SSL_CONNECT_ERROR;
      ఆపు;
    }
    ఒకవేళ(ఫలితం != CURLE_AGAIN)
      ఆపు;
  }

  DEBUGASSERT(ఫలితం != CURLE_AGAIN);

  rs->started = FALSE;
  backend->recv_renegotiating = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  ఒకవేళ(ఫలితం)
    failf(దత్తాంశం, "schannel: renegotiation failed");
  లేకపోతే
    infof(దత్తాంశం, "schannel: SSL/TLS connection renegotiated");

  తిరిగి ఫలితం;
}

స్థిర CURLcode schannel_send(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                              స్థిరరాశి శూన్య *buf, size_t len, size_t *pnwritten)
{
  size_t దత్తాంశం_len = 0;
  గుర్తులేని అక్షరం *ptr = ఖాళీ;
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  Sec_బఫరు outbuf[4];
  Sec_బఫరు_Desc outbuf_desc;
  SECURITY_స్థితి sspi_స్థితి = SEC_E_OK;
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం schannel_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం schannel_ssl_backend_దత్తాంశం *)connssl->backend;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  ఒకవేళ(backend->recv_renegotiating) {
    ఫలితం = schannel_recv_renegotiate(cf, దత్తాంశం, SCH_RENEG_CALLER_IS_SEND);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  /* check if the buffer is longer than the maximum message length */
  ఒకవేళ(len > backend->ప్రవాహం_sizes.cb_గరిష్ఠ_సందేశం) {
    len = backend->ప్రవాహం_sizes.cb_గరిష్ఠ_సందేశం;
  }

  /* calculate the complete message length and prepare the send buffer */
  దత్తాంశం_len = backend->ప్రవాహం_sizes.cb_శీర్షిక + len +
    backend->ప్రవాహం_sizes.cbTrailer;
  ఒకవేళ(దత్తాంశం_len > backend->send_బఫరు_len) {
    ptr = curlx_realloc(backend->send_బఫరు, దత్తాంశం_len);
    ఒకవేళ(!ptr)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    backend->send_బఫరు = ptr;
    backend->send_బఫరు_len = దత్తాంశం_len;
  }
  లేకపోతే
    ptr = backend->send_బఫరు;

  /* setup output buffers (header, data, trailer, empty) */
  Init_Sec_బఫరు(&outbuf[0], SECBUFFER_ప్రవాహం_శీర్షిక,
                ptr, backend->ప్రవాహం_sizes.cb_శీర్షిక);
  Init_Sec_బఫరు(&outbuf[1], SECBUFFER_దత్తాంశం,
                ptr + backend->ప్రవాహం_sizes.cb_శీర్షిక, curlx_uztoul(len));
  Init_Sec_బఫరు(&outbuf[2], SECBUFFER_ప్రవాహం_TRAILER,
                ptr + backend->ప్రవాహం_sizes.cb_శీర్షిక + len,
                backend->ప్రవాహం_sizes.cbTrailer);
  Init_Sec_బఫరు(&outbuf[3], SECBUFFER_ఖాళీలు, ఖాళీ, 0);
  Init_Sec_బఫరు_Desc(&outbuf_desc, outbuf, 4);

  /* copy data into output buffer */
  జ్ఞాపకం_నకలు(outbuf[1].pv_బఫరు, buf, len);

  /* https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-encryptmessage */
  sspi_స్థితి = Curl_pSecFn->Encrypt_సందేశం(&backend->ctxt->ctxt_handle, 0,
                                            &outbuf_desc, 0);

  /* check if the message was encrypted */
  ఒకవేళ(sspi_స్థితి == SEC_E_OK) {

    /* send the encrypted message including header, data and trailer */
    len = outbuf[0].cb_బఫరు + outbuf[1].cb_బఫరు + outbuf[2].cb_బఫరు;

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
    అయితే(len > *pnwritten) {
      size_t this_write = 0;
      పూర్ణాంకం what;
      timediff_t timeout_ms = Curl_timeleft_ms(దత్తాంశం);
      ఒకవేళ(timeout_ms < 0) {
        /* we already got the timeout */
        failf(దత్తాంశం, "schannel: timed out sending data (bytes sent: %zu)",
              *pnwritten);
        ఫలితం = CURLE_OPERATION_TIMEDOUT;
        ఆపు;
      }
      లేకపోతే ఒకవేళ(!timeout_ms)
        timeout_ms = TIMEDIFF_T_MAX;
      what = SOCKET_WRITABLE(Curl_conn_cf_get_socket(cf, దత్తాంశం), timeout_ms);
      ఒకవేళ(what < 0) {
        /* fatal error */
        failf(దత్తాంశం, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        ఫలితం = CURLE_SEND_ERROR;
        ఆపు;
      }
      లేకపోతే ఒకవేళ(what == 0) {
        failf(దత్తాంశం, "schannel: timed out sending data (bytes sent: %zu)",
              *pnwritten);
        ఫలితం = CURLE_OPERATION_TIMEDOUT;
        ఆపు;
      }
      /* socket is writable */

       ఫలితం = Curl_conn_cf_send(cf->తదుపరి, దత్తాంశం,
                                  (స్థిరరాశి uint8_t *)ptr + *pnwritten,
                                  len - *pnwritten,
                                  FALSE, &this_write);
      ఒకవేళ(ఫలితం == CURLE_AGAIN)
        కొనసాగు;
      లేకపోతే ఒకవేళ(ఫలితం) {
        ఆపు;
      }

      *pnwritten += this_write;
    }
  }
  లేకపోతే ఒకవేళ(sspi_స్థితి == SEC_E_INSUFFICIENT_జ్ఞాపకం) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
  లేకపోతే {
    ఫలితం = CURLE_SEND_ERROR;
  }

  ఒకవేళ(len == *pnwritten)
    /* Encrypted message including header, data and trailer entirely sent.
       The return value is the number of unencrypted bytes that were sent. */
    *pnwritten = outbuf[1].cb_బఫరు;

  తిరిగి ఫలితం;
}

స్థిర CURLcode schannel_recv(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                              అక్షరం *buf, size_t len, size_t *pnread)
{
  size_t కొలత = 0;
  size_t nread = 0;
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  Sec_బఫరు inbuf[4];
  Sec_బఫరు_Desc inbuf_desc;
  SECURITY_స్థితి sspi_స్థితి = SEC_E_OK;
  నిర్మాణం schannel_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం schannel_ssl_backend_దత్తాంశం *)connssl->backend;
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(backend);
  *pnread = 0;

  ఒకవేళ(backend->recv_renegotiating) {
    ఫలితం = schannel_recv_renegotiate(cf, దత్తాంశం, SCH_RENEG_CALLER_IS_RECV);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
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

  SCH_DEV(infof(దత్తాంశం, "schannel: client wants to read %zu bytes", len));

  ఒకవేళ(len && len <= backend->decdata.స్థానభ్రంశం) {
    SCH_DEV(infof(దత్తాంశం,
                  "schannel: enough decrypted data is already available"));
    వెళ్ళు cleanup;
  }
  లేకపోతే ఒకవేళ(backend->recv_unrecoverable_err) {
    ఫలితం = backend->recv_unrecoverable_err;
    infof(దత్తాంశం, "schannel: an unrecoverable error occurred in a prior call");
    వెళ్ళు cleanup;
  }
  లేకపోతే ఒకవేళ(backend->recv_sspi_close_notify) {
    /* once a server has indicated shutdown there is no more encrypted data */
    infof(దత్తాంశం, "schannel: server indicated shutdown in a prior call");
    వెళ్ళు cleanup;
  }
  /* it is debatable what to return when !len. Regardless we cannot return
     immediately because there may be data to decrypt (in the case we want to
     decrypt all encrypted cached data) so handle !len later in cleanup. */
  లేకపోతే ఒకవేళ(len && !backend->recv_connection_closed) {
    /* the encrypted buffer must be large enough to hold all the bytes
       requested and some TLS record overhead. 'len' is a buffer size, so this
       integer math cannot overflow. */
    స్థిరరాశి size_t min_encdata_నిడివి = len + CURL_SCHANNEL_బఫరు_FREE_కొలత;

    /* make sure encrypt buffer fits the requested amount of data */
    ఫలితం = ensure_encoding_కొలత(దత్తాంశం, &backend->encdata, min_encdata_నిడివి);
    ఒకవేళ(ఫలితం)
      వెళ్ళు cleanup;

    SCH_DEV(infof(దత్తాంశం,
                  "schannel: encrypted data buffer: offset %zu length %zu",
                  backend->encdata.స్థానభ్రంశం, backend->encdata.నిడివి));

    /* read encrypted data from socket */
    ఫలితం = Curl_conn_cf_recv(cf->తదుపరి, దత్తాంశం,
                               (అక్షరం *)(backend->encdata.బఫరు +
                                        backend->encdata.స్థానభ్రంశం),
                               backend->encdata.నిడివి -
                               backend->encdata.స్థానభ్రంశం,
                               &nread);
    ఒకవేళ(ఫలితం) {
      ఒకవేళ(ఫలితం == CURLE_AGAIN)
        SCH_DEV(infof(దత్తాంశం, "schannel: recv returned CURLE_AGAIN"));
      లేకపోతే {
        infof(దత్తాంశం, "schannel: recv returned error %d", (పూర్ణాంకం)ఫలితం);
        backend->recv_unrecoverable_err = ఫలితం;
      }
    }
    లేకపోతే ఒకవేళ(nread == 0) {
      backend->recv_connection_closed = TRUE;
      DEBUGF(infof(దత్తాంశం, "schannel: server closed the connection"));
    }
    లేకపోతే {
      backend->encdata.స్థానభ్రంశం += nread;
      backend->encdata_is_incomplete = FALSE;
      SCH_DEV(infof(దత్తాంశం, "schannel: encrypted data got %zu", nread));
    }
  }

  SCH_DEV(infof(దత్తాంశం, "schannel: encrypted data buffer: offset %zu length %zu",
                backend->encdata.స్థానభ్రంశం, backend->encdata.నిడివి));

  /* decrypt loop */
  అయితే(backend->encdata.స్థానభ్రంశం > 0 && sspi_స్థితి == SEC_E_OK &&
        (!len || backend->decdata.స్థానభ్రంశం < len ||
         backend->recv_connection_closed)) {
    /* prepare data buffer for DecryptMessage call */
    Init_Sec_బఫరు(&inbuf[0], SECBUFFER_దత్తాంశం, backend->encdata.బఫరు,
                  curlx_uztoul(backend->encdata.స్థానభ్రంశం));

    /* we need 3 more empty input buffers for possible output */
    Init_Sec_బఫరు(&inbuf[1], SECBUFFER_ఖాళీలు, ఖాళీ, 0);
    Init_Sec_బఫరు(&inbuf[2], SECBUFFER_ఖాళీలు, ఖాళీ, 0);
    Init_Sec_బఫరు(&inbuf[3], SECBUFFER_ఖాళీలు, ఖాళీ, 0);
    Init_Sec_బఫరు_Desc(&inbuf_desc, inbuf, 4);

    /* https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-decryptmessage
     */
    sspi_స్థితి = Curl_pSecFn->Decrypt_సందేశం(&backend->ctxt->ctxt_handle,
                                              &inbuf_desc, 0, ఖాళీ);

    /* check if everything went fine (server may want to renegotiate
       or shutdown the connection context) */
    ఒకవేళ(sspi_స్థితి == SEC_E_OK || sspi_స్థితి == SEC_I_RENEGOTIATE ||
       sspi_స్థితి == SEC_I_CONTEXT_EXPIRED) {
      /* check for successfully decrypted data, even before actual
         renegotiation or shutdown of the connection context */
      ఒకవేళ(inbuf[1].బఫరు_Type == SECBUFFER_దత్తాంశం) {
        SCH_DEV(infof(దత్తాంశం, "schannel: decrypted data length: %lu",
                      inbuf[1].cb_బఫరు));

        /* ensure the decode buffer fits the received amount of data */
        ఫలితం = ensure_decoding_కొలత(దత్తాంశం, &backend->decdata,
                                      inbuf[1].cb_బఫరు, len);
        ఒకవేళ(ఫలితం)
          వెళ్ళు cleanup;

        /* copy decrypted data to internal buffer */
        కొలత = inbuf[1].cb_బఫరు;
        ఒకవేళ(కొలత) {
          జ్ఞాపకం_నకలు(backend->decdata.బఫరు + backend->decdata.స్థానభ్రంశం,
                 inbuf[1].pv_బఫరు, కొలత);
          backend->decdata.స్థానభ్రంశం += కొలత;
        }

        SCH_DEV(infof(దత్తాంశం, "schannel: decrypted data added: %zu", కొలత));
        SCH_DEV(infof(దత్తాంశం,
                      "schannel: decrypted cached: offset %zu length %zu",
                      backend->decdata.స్థానభ్రంశం, backend->decdata.నిడివి));
      }

      /* check for remaining encrypted data */
      ఒకవేళ(inbuf[3].బఫరు_Type == SECBUFFER_EXTRA && inbuf[3].cb_బఫరు > 0) {
        SCH_DEV(infof(దత్తాంశం, "schannel: encrypted data length: %lu",
                      inbuf[3].cb_బఫరు));

        /* check if the remaining data is less than the total amount
         * and therefore begins after the already processed data
         */
        ఒకవేళ(backend->encdata.స్థానభ్రంశం > inbuf[3].cb_బఫరు) {
          /* move remaining encrypted data forward to the beginning of
బఫరు*/
          memmove(backend->encdata.బఫరు,
                  (backend->encdata.బఫరు + backend->encdata.స్థానభ్రంశం) -
                  inbuf[3].cb_బఫరు, inbuf[3].cb_బఫరు);
          backend->encdata.స్థానభ్రంశం = inbuf[3].cb_బఫరు;
        }

        SCH_DEV(infof(దత్తాంశం,
                      "schannel: encrypted cached: offset %zu length %zu",
                      backend->encdata.స్థానభ్రంశం, backend->encdata.నిడివి));
      }
      లేకపోతే {
        /* reset encrypted buffer offset, because there is no data remaining */
        backend->encdata.స్థానభ్రంశం = 0;
      }

      /* check if server wants to renegotiate the connection context */
      ఒకవేళ(sspi_స్థితి == SEC_I_RENEGOTIATE) {
        infof(దత్తాంశం, "schannel: remote party requests renegotiation");
        ఒకవేళ(ఫలితం && ఫలితం != CURLE_AGAIN) {
          infof(దత్తాంశం, "schannel: cannot renegotiate, an error is pending");
          వెళ్ళు cleanup;
        }

        backend->recv_renegotiating = TRUE;
        ఫలితం = schannel_recv_renegotiate(cf, దత్తాంశం, SCH_RENEG_CALLER_IS_RECV);
        ఒకవేళ(ఫలితం)
          వెళ్ళు cleanup;

        /* now retry receiving data */
        sspi_స్థితి = SEC_E_OK;
        కొనసాగు;
      }
      /* check if the server closed the connection */
      లేకపోతే ఒకవేళ(sspi_స్థితి == SEC_I_CONTEXT_EXPIRED) {
        /* In Windows 2000 SEC_I_CONTEXT_EXPIRED (close_notify) is not
           returned so we have to work around that in cleanup. */
        backend->recv_sspi_close_notify = TRUE;
        ఒకవేళ(!backend->recv_connection_closed)
          backend->recv_connection_closed = TRUE;
        /* We received the close notify fine, any error we got
         * from the lower filters afterwards (e.g. the socket), is not
         * an error on the TLS data stream. That one ended here. */
        ఒకవేళ(ఫలితం == CURLE_RECV_ERROR)
          ఫలితం = CURLE_OK;
        infof(దత్తాంశం,
              "schannel: server close notification received (close_notify)");
        వెళ్ళు cleanup;
      }
    }
    లేకపోతే ఒకవేళ(sspi_స్థితి == SEC_E_INCOMPLETE_సందేశం) {
      backend->encdata_is_incomplete = TRUE;
      ఒకవేళ(!ఫలితం)
        ఫలితం = CURLE_AGAIN;
      SCH_DEV(infof(దత్తాంశం, "schannel: failed to decrypt data, need more data"));
      వెళ్ళు cleanup;
    }
    లేకపోతే {
      అక్షరం బఫరు[STRERROR_LEN];
      failf(దత్తాంశం, "schannel: failed to read data from server: %s",
            Curl_sspi_strerror(sspi_స్థితి, బఫరు, పరిమాణం(బఫరు)));
      ఫలితం = CURLE_RECV_ERROR;
      వెళ్ళు cleanup;
    }
  }

  SCH_DEV(infof(దత్తాంశం, "schannel: encrypted data buffer: offset %zu length %zu",
                backend->encdata.స్థానభ్రంశం, backend->encdata.నిడివి));

  SCH_DEV(infof(దత్తాంశం, "schannel: decrypted data buffer: offset %zu length %zu",
                backend->decdata.స్థానభ్రంశం, backend->decdata.నిడివి));

cleanup:
  /* Warning- there is no guarantee the encdata state is valid at this point */
  SCH_DEV(infof(దత్తాంశం, "schannel: schannel_recv cleanup"));

  /* Error if the connection has closed without a close_notify.

     The behavior here is a matter of debate. We do not want to be vulnerable
     to a truncation attack however there is some browser precedent for
     ignoring the close_notify for compatibility reasons. */
  ఒకవేళ(len && !backend->decdata.స్థానభ్రంశం && backend->recv_connection_closed &&
     !backend->recv_sspi_close_notify) {
    ఫలితం = CURLE_RECV_ERROR;
    failf(దత్తాంశం, "schannel: server closed abruptly (missing close_notify)");
  }

  /* Any error other than CURLE_AGAIN is an unrecoverable error. */
  ఒకవేళ(ఫలితం && ఫలితం != CURLE_AGAIN)
    backend->recv_unrecoverable_err = ఫలితం;

  కొలత = len < backend->decdata.స్థానభ్రంశం ? len : backend->decdata.స్థానభ్రంశం;
  ఒకవేళ(కొలత) {
    జ్ఞాపకం_నకలు(buf, backend->decdata.బఫరు, కొలత);
    memmove(backend->decdata.బఫరు, backend->decdata.బఫరు + కొలత,
            backend->decdata.స్థానభ్రంశం - కొలత);
    backend->decdata.స్థానభ్రంశం -= కొలత;
    SCH_DEV(infof(దత్తాంశం, "schannel: decrypted data returned %zu", కొలత));
    SCH_DEV(infof(దత్తాంశం,
                  "schannel: decrypted data buffer: offset %zu length %zu",
                  backend->decdata.స్థానభ్రంశం, backend->decdata.నిడివి));
    *pnread = కొలత;
    తిరిగి CURLE_OK;
  }

  ఒకవేళ(!ఫలితం && !backend->recv_connection_closed)
    ఫలితం = CURLE_AGAIN;

  /* it is debatable what to return when !len. We could return whatever error
     we got from decryption but instead we override here so the return is
     consistent. */
  ఒకవేళ(!len)
    తిరిగి CURLE_OK;

  తిరిగి ఫలితం;
}

స్థిర bool schannel_దత్తాంశం_pending(నిర్మాణం Curl_cfilter *cf,
                                  స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం)
{
  స్థిరరాశి నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం schannel_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం schannel_ssl_backend_దత్తాంశం *)connssl->backend;

  (శూన్య)దత్తాంశం;
  DEBUGASSERT(backend);

  ఒకవేళ(backend->ctxt) /* SSL/TLS is in use */
    తిరిగి backend->decdata.స్థానభ్రంశం > 0 ||
           (backend->encdata.స్థానభ్రంశం > 0 && !backend->encdata_is_incomplete) ||
           backend->recv_connection_closed ||
           backend->recv_sspi_close_notify ||
           backend->recv_unrecoverable_err;
  లేకపోతే
    తిరిగి FALSE;
}

/* shut down the SSL connection and clean up related memory.
   this function can be called multiple times on the same connection including
   if the SSL connection failed (eg connection made but failed handshake). */
స్థిర CURLcode schannel_shutdown(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  bool send_shutdown, bool *పూర్తి)
{
  /* See https://learn.microsoft.com/windows/win32/secauthn/shutting-down-an-schannel-connection
   * Shutting Down an Schannel Connection
   */
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం schannel_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం schannel_ssl_backend_దత్తాంశం *)connssl->backend;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(cf->shutdown) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(backend);

  /* Not supported in schannel */
  (శూన్య)send_shutdown;

  *పూర్తి = FALSE;
  ఒకవేళ(backend->ctxt) {
    infof(దత్తాంశం, "schannel: shutting down SSL/TLS connection with %s port %d",
          connssl->peer.origin->hostname, connssl->peer.origin->పోర్టు);
  }

  ఒకవేళ(!backend->ctxt || cf->shutdown) {
    *పూర్తి = TRUE;
    వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(backend->cred && backend->ctxt && !backend->పంపిన_shutdown &&
     !backend->shutdown_generated) {
    Sec_బఫరు_Desc BuffDesc;
    Sec_బఫరు బఫరు_2;
    SECURITY_స్థితి sspi_స్థితి;
    Sec_బఫరు outbuf;
    Sec_బఫరు_Desc outbuf_desc;
    DWORD dwshut = SCHANNEL_SHUTDOWN;

    Init_Sec_బఫరు(&బఫరు_2, SECBUFFER_TOKEN, &dwshut, పరిమాణం(dwshut));
    Init_Sec_బఫరు_Desc(&BuffDesc, &బఫరు_2, 1);

    sspi_స్థితి = Curl_pSecFn->ApplyControlToken(&backend->ctxt->ctxt_handle,
                                                 &BuffDesc);

    ఒకవేళ(sspi_స్థితి != SEC_E_OK) {
      అక్షరం బఫరు[STRERROR_LEN];
      failf(దత్తాంశం, "schannel: ApplyControlToken failure: %s",
            Curl_sspi_strerror(sspi_స్థితి, బఫరు, పరిమాణం(బఫరు)));
      ఫలితం = CURLE_SEND_ERROR;
      వెళ్ళు బహిర్గతం;
    }

    /* setup output buffer */
    Init_Sec_బఫరు(&outbuf, SECBUFFER_ఖాళీలు, ఖాళీ, 0);
    Init_Sec_బఫరు_Desc(&outbuf_desc, &outbuf, 1);

    sspi_స్థితి = Curl_pSecFn->InitializeSecurityContext(
      &backend->cred->cred_handle,
      &backend->ctxt->ctxt_handle,
      backend->cred->sni_hostname,
      backend->req_flags,
      0,
      0,
      ఖాళీ,
      0,
      &backend->ctxt->ctxt_handle,
      &outbuf_desc,
      &backend->ret_flags, ఖాళీ);

    ఒకవేళ((sspi_స్థితి == SEC_E_OK) || (sspi_స్థితి == SEC_I_CONTEXT_EXPIRED)) {
      backend->shutdown_token = outbuf;
      backend->shutdown_generated = TRUE;
    }
    లేకపోతే {
      అక్షరం బఫరు[STRERROR_LEN];
      ఒకవేళ(outbuf.pv_బఫరు)
        Curl_pSecFn->Free_Context_బఫరు(outbuf.pv_బఫరు);
      failf(దత్తాంశం, "schannel: generating close msg failed: %s",
            Curl_sspi_strerror(sspi_స్థితి, బఫరు, పరిమాణం(బఫరు)));
      ఫలితం = CURLE_SEND_ERROR;
      వెళ్ళు బహిర్గతం;
    }
  }

  ఒకవేళ(backend->shutdown_generated && !backend->పంపిన_shutdown) {
    size_t written = 0;
    size_t మిగిలిన;

    DEBUGASSERT(backend->shutdown_స్థానభ్రంశం <= backend->shutdown_token.cb_బఫరు);
    మిగిలిన = backend->shutdown_token.cb_బఫరు - backend->shutdown_స్థానభ్రంశం;

    ఒకవేళ(మిగిలిన) {
      ఫలితం = Curl_conn_cf_send(cf->తదుపరి, దత్తాంశం,
                                 ((uint8_t *)backend->shutdown_token.pv_బఫరు +
                                  backend->shutdown_స్థానభ్రంశం),
                                 మిగిలిన, FALSE, &written);
      ఒకవేళ(!ఫలితం)
        backend->shutdown_స్థానభ్రంశం += written;
      ఒకవేళ(ఫలితం == CURLE_AGAIN || (!ఫలితం && written < మిగిలిన)) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        ఫలితం = CURLE_OK;
        వెళ్ళు బహిర్గతం;
      }
    }
    లేకపోతే /* no shutdown message to send? */
      ఫలితం = CURLE_OK;
    ఒకవేళ(ఫలితం) {
      ఒకవేళ(backend->recv_connection_closed) {
        /* The server already closed, so sending close_notify is optional. */
        *పూర్తి = TRUE;
        ఫలితం = CURLE_OK;
      }
      లేకపోతే {
        failf(దత్తాంశం, "schannel: error sending close msg: %d", (పూర్ణాంకం)ఫలితం);
        ఫలితం = CURLE_SEND_ERROR;
      }
      వెళ్ళు బహిర్గతం;
    }
    ఒకవేళ(backend->shutdown_token.pv_బఫరు)
      Curl_pSecFn->Free_Context_బఫరు(backend->shutdown_token.pv_బఫరు);
    memset(&backend->shutdown_token, 0, పరిమాణం(backend->shutdown_token));
    backend->shutdown_స్థానభ్రంశం = 0;
    backend->పంపిన_shutdown = TRUE;
    *పూర్తి = TRUE;
  }

  /* If the connection seems open and we have not seen the close notify
   * from the server yet, try to receive it. */
  ఒకవేళ(backend->cred && backend->ctxt &&
     !backend->recv_sspi_close_notify && !backend->recv_connection_closed) {
    అక్షరం బఫరు[1024];
    size_t nread;

    ఫలితం = schannel_recv(cf, దత్తాంశం, బఫరు, పరిమాణం(బఫరు), &nread);
    ఒకవేళ(ఫలితం == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
    }
    లేకపోతే ఒకవేళ(ఫలితం) {
      CURL_TRC_CF(దత్తాంశం, cf, "SSL shutdown, error %d", (పూర్ణాంకం)ఫలితం);
      ఫలితం = CURLE_RECV_ERROR;
    }
    లేకపోతే ఒకవేళ(nread == 0) {
      /* We got the close notify alert and are done. */
      backend->recv_connection_closed = TRUE;
      *పూర్తి = TRUE;
    }
    లేకపోతే {
      /* still data coming in? */
    }
  }

బహిర్గతం:
  cf->shutdown = (ఫలితం || *పూర్తి);
  తిరిగి ఫలితం;
}

స్థిర శూన్య schannel_close(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం schannel_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం schannel_ssl_backend_దత్తాంశం *)connssl->backend;

  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(backend);

  ఒకవేళ(backend->shutdown_token.pv_బఫరు)
    Curl_pSecFn->Free_Context_బఫరు(backend->shutdown_token.pv_బఫరు);
  memset(&backend->shutdown_token, 0, పరిమాణం(backend->shutdown_token));
  backend->shutdown_స్థానభ్రంశం = 0;

  /* free SSPI Schannel API security context handle */
  ఒకవేళ(backend->ctxt) {
    ఒకవేళ(cf->conn->sslContext == &backend->ctxt->ctxt_handle)
      cf->conn->sslContext = ఖాళీ;

    DEBUGF(infof(దత్తాంశం, "schannel: clear security context handle"));
    Curl_pSecFn->DeleteSecurityContext(&backend->ctxt->ctxt_handle);
    curlx_safefree(backend->ctxt);
  }

  /* free SSPI Schannel API credential handle */
  ఒకవేళ(backend->cred) {
    Curl_ssl_scache_తాళం(దత్తాంశం);
    schannel_session_free(backend->cred);
    Curl_ssl_scache_unlock(దత్తాంశం);
    backend->cred = ఖాళీ;
  }

  /* free the buffer used to encrypt outgoing data */
  curlx_safefree(backend->send_బఫరు);
  backend->send_బఫరు_len = 0;

  /* free internal buffer for received encrypted data */
  ఒకవేళ(backend->encdata.బఫరు) {
    curlx_safefree(backend->encdata.బఫరు);
    backend->encdata.నిడివి = 0;
    backend->encdata.స్థానభ్రంశం = 0;
    backend->encdata_is_incomplete = FALSE;
  }

  /* free internal buffer for received decrypted data */
  ఒకవేళ(backend->decdata.బఫరు) {
    curlx_safefree(backend->decdata.బఫరు);
    backend->decdata.నిడివి = 0;
    backend->decdata.స్థానభ్రంశం = 0;
  }
}

స్థిర పూర్ణాంకం schannel_init(శూన్య)
{
  రకనిర్వచనం స్థిరరాశి అక్షరం *(APIENTRY *WINE_GET_సంస్కరణ_FN)(శూన్య);
#ఒకవేళ defined(__clang__) && __clang_major__ >= 16
#సూచన clang diagnostic నెట్టు
#సూచన clang diagnostic ignored "-Wcast-function-type-strict"
#అయితే_ముగింపు
  WINE_GET_సంస్కరణ_FN p_wine_get_సంస్కరణ =
    CURLX_FUNCTION_CAST(WINE_GET_సంస్కరణ_FN,
      Get_Proc_చిరునామా(GetModuleHandle(పాఠ్యం_2("ntdll")), "wine_get_version"));
#ఒకవేళ defined(__clang__) && __clang_major__ >= 16
#సూచన clang diagnostic తీయి
#అయితే_ముగింపు
  ఒకవేళ(p_wine_get_సంస్కరణ) {  /* WINE detected */
    curl_off_t ver = 0;
    స్థిరరాశి అక్షరం *wine_సంస్కరణ = p_wine_get_సంస్కరణ();  /* e.g. "6.0.2" */
    /* Assume ALPN support with WINE 6.0 or greater */
    ఒకవేళ(wine_సంస్కరణ)
      curlx_str_సంఖ్య(&wine_సంస్కరణ, &ver, 20);
    s_win_has_alpn = (ver >= 6);
  }
  లేకపోతే {
    /* ALPN is supported on Windows 8.1 / Server 2012 R2 or newer. */
    s_win_has_alpn = curlx_verify_windows_సంస్కరణ(6, 3, 0, PLATFORM_WINNT,
                                                  సంస్కరణ_GREATER_THAN_EQUAL);
  }

  తిరిగి Curl_sspi_global_init() == CURLE_OK ? 1 : 0;
}

స్థిర శూన్య schannel_cleanup(శూన్య)
{
  Curl_sspi_global_cleanup();
}

స్థిర size_t schannel_సంస్కరణ(అక్షరం *బఫరు, size_t కొలత)
{
  తిరిగి curl_msnprintf(బఫరు, కొలత, "Schannel");
}

స్థిర CURLcode schannel_random(నిర్మాణం Curl_easy *దత్తాంశం,
                                గుర్తులేని అక్షరం *entropy, size_t నిడివి)
{
  (శూన్య)దత్తాంశం;

  తిరిగి curlx_win32_random(entropy, నిడివి);
}

స్థిర CURLcode schannel_checksum(స్థిరరాశి గుర్తులేని అక్షరం *ఆదానం,
                                  size_t inputlen,
                                  గుర్తులేని అక్షరం *checksum,
                                  size_t checksumlen,
                                  DWORD provType,
                                  స్థిరరాశి గుర్తులేని పూర్ణాంకం algId)
{
  CURLcode ఫలితం = CURLE_FAILED_INIT;

  HCRYPTPROV hProv = 0;
  HCRYPTHASH hHash = 0;

  size_t off;

  DWORD cb_Hash_కొలత;
  DWORD dw_Hash_కొలత_Len;
  DWORD dwChecksumLen;

  /* since this can fail in multiple ways, zero memory first so we never
   * return old data
   */
  memset(checksum, 0, checksumlen);

  ఒకవేళ(!CryptAcquireContext(&hProv, ఖాళీ, ఖాళీ, provType,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    వెళ్ళు బహిర్గతం;

  ఒకవేళ(!CryptCreateHash(hProv, algId, 0, 0, &hHash))
    వెళ్ళు బహిర్గతం;

  ఫలితం = CURLE_BAD_FUNCTION_వాదం;

  off = 0;
  అయితే(off < inputlen) {
    DWORD chunk = (DWORD)CURLMIN(inputlen - off, 0xffffffffUL);
    ఒకవేళ(!Crypt_Hash_దత్తాంశం(hHash, ఆదానం + off, chunk, 0))
      వెళ్ళు బహిర్గతం;
    off += chunk;
  }

  /* get hash size */
  cb_Hash_కొలత = 0;
  dw_Hash_కొలత_Len = (DWORD)పరిమాణం(cb_Hash_కొలత);
  ఒకవేళ(!CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE *)&cb_Hash_కొలత,
                        &dw_Hash_కొలత_Len, 0))
    వెళ్ళు బహిర్గతం;

  /* check if hash fits into the return buffer */
  ఒకవేళ(checksumlen < cb_Hash_కొలత)
    వెళ్ళు బహిర్గతం;

  dwChecksumLen = (DWORD)checksumlen;
  ఒకవేళ(CryptGetHashParam(hHash, HP_HASHVAL, checksum, &dwChecksumLen, 0) &&
     dwChecksumLen == cb_Hash_కొలత)
    ఫలితం = CURLE_OK;

బహిర్గతం:
  ఒకవేళ(hHash)
    CryptDestroyHash(hHash);

  ఒకవేళ(hProv)
    CryptReleaseContext(hProv, 0);

  తిరిగి ఫలితం;
}

స్థిర CURLcode schannel_sha256sum(స్థిరరాశి గుర్తులేని అక్షరం *ఆదానం,
                                   size_t inputlen,
                                   గుర్తులేని అక్షరం *sha256sum,
                                   size_t sha256len)
{
  తిరిగి schannel_checksum(ఆదానం, inputlen, sha256sum, sha256len,
                           PROV_RSA_AES, CALG_SHA_256);
}

స్థిర శూన్య *schannel_get_internals(నిర్మాణం ssl_connect_దత్తాంశం *connssl,
                                    CURLINFO info)
{
  నిర్మాణం schannel_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం schannel_ssl_backend_దత్తాంశం *)connssl->backend;
  (శూన్య)info;
  DEBUGASSERT(backend);
  తిరిగి &backend->ctxt->ctxt_handle;
}

HCERTSTORE Curl_schannel_get_cached_cert_store(నిర్మాణం Curl_cfilter *cf,
                                               నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  నిర్మాణం Curl_multi *multi = దత్తాంశం->multi;
  స్థిరరాశి నిర్మాణం curl_blob *ca_info_blob = conn_config->ca_info_blob;
  నిర్మాణం schannel_cert_share *share;
  timediff_t timeout_ms;
  గుర్తులేని అక్షరం info_blob_digest[CURL_SHA256_DIGEST_నిడివి];

  DEBUGASSERT(multi);

  ఒకవేళ(!multi) {
    తిరిగి ఖాళీ;
  }

  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_SCHANNEL_CERT_SHARE_కీలకం,
                         CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_కీలకం));
  ఒకవేళ(!share || !share->cert_store) {
    తిరిగి ఖాళీ;
  }

  /* zero ca_cache_timeout completely disables caching */
  ఒకవేళ(!దత్తాంశం->సమితి.ssl_ca_cache_timeout) {
    తిరిగి ఖాళీ;
  }

  /* check for cache timeout by using the cached_x509_store_expired timediff
     calculation pattern from openssl.c.
     negative timeout means retain forever. */
  timeout_ms = దత్తాంశం->సమితి.ssl_ca_cache_timeout * (timediff_t)1000;
  ఒకవేళ(timeout_ms >= 0) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(దత్తాంశం), &share->సమయం);
    ఒకవేళ(elapsed_ms >= timeout_ms) {
      తిరిగి ఖాళీ;
    }
  }

  ఒకవేళ(ca_info_blob) {
    ఒకవేళ(share->CAinfo_blob_కొలత != ca_info_blob->len) {
      తిరిగి ఖాళీ;
    }
    ఒకవేళ(schannel_sha256sum((స్థిరరాశి గుర్తులేని అక్షరం *)ca_info_blob->దత్తాంశం,
                          ca_info_blob->len,
                          info_blob_digest,
                          CURL_SHA256_DIGEST_నిడివి))
      తిరిగి ఖాళీ;
    ఒకవేళ(memcmp(share->CAinfo_blob_digest, info_blob_digest,
              CURL_SHA256_DIGEST_నిడివి)) {
      తిరిగి ఖాళీ;
    }
  }
  లేకపోతే {
    ఒకవేళ(!conn_config->CAfile || !share->CAfile ||
       strcmp(share->CAfile, conn_config->CAfile)) {
      తిరిగి ఖాళీ;
    }
  }

  తిరిగి share->cert_store;
}

స్థిర శూన్య schannel_cert_share_free(స్థిరరాశి శూన్య *కీలకం, size_t కీలకం_len, శూన్య *p)
{
  నిర్మాణం schannel_cert_share *share = p;
  DEBUGASSERT(కీలకం_len == CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_కీలకం));
  DEBUGASSERT(!memcmp(MPROTO_SCHANNEL_CERT_SHARE_కీలకం, కీలకం, కీలకం_len));
  (శూన్య)కీలకం;
  (శూన్య)కీలకం_len;
  ఒకవేళ(share->cert_store) {
    CertCloseStore(share->cert_store, 0);
  }
  curlx_free(share->CAfile);
  curlx_free(share);
}

bool Curl_schannel_సమితి_cached_cert_store(నిర్మాణం Curl_cfilter *cf,
                                         నిర్మాణం Curl_easy *దత్తాంశం,
                                         HCERTSTORE cert_store)
{
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  నిర్మాణం Curl_multi *multi = దత్తాంశం->multi;
  స్థిరరాశి నిర్మాణం curl_blob *ca_info_blob = conn_config->ca_info_blob;
  నిర్మాణం schannel_cert_share *share;
  గుర్తులేని అక్షరం digest[CURL_SHA256_DIGEST_నిడివి];
  అక్షరం *CAfile = ఖాళీ;

  DEBUGASSERT(multi);

  ఒకవేళ(!multi) {
    తిరిగి FALSE;
  }

  ఒకవేళ(ca_info_blob) {
    ఒకవేళ(schannel_sha256sum((స్థిరరాశి గుర్తులేని అక్షరం *)ca_info_blob->దత్తాంశం,
                          ca_info_blob->len, digest, పరిమాణం(digest))) {
      తిరిగి FALSE;
    }
  }
  లేకపోతే ఒకవేళ(conn_config->CAfile) {
    CAfile = curlx_strdup(conn_config->CAfile);
    ఒకవేళ(!CAfile) {
      తిరిగి FALSE;
    }
  }

  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_SCHANNEL_CERT_SHARE_కీలకం,
                         CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_కీలకం));
  ఒకవేళ(!share) {
    share = curlx_calloc(1, పరిమాణం(*share));
    ఒకవేళ(!share) {
      curlx_free(CAfile);
      తిరిగి FALSE;
    }
    ఒకవేళ(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_SCHANNEL_CERT_SHARE_కీలకం,
                       CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_కీలకం),
                       share, schannel_cert_share_free)) {
      curlx_free(share);
      curlx_free(CAfile);
      తిరిగి FALSE;
    }
  }

  /* free old cache data */
  ఒకవేళ(share->cert_store) {
    CertCloseStore(share->cert_store, 0);
  }
  curlx_free(share->CAfile);

  ఒకవేళ(ca_info_blob) {
    జ్ఞాపకం_నకలు(share->CAinfo_blob_digest, digest, పరిమాణం(digest));
    share->CAinfo_blob_కొలత = ca_info_blob->len;
  }
  లేకపోతే
    share->CAinfo_blob_కొలత = 0;

  share->సమయం = curlx_now();
  share->cert_store = cert_store;
  share->CAfile = CAfile;
  తిరిగి TRUE;
}

స్థిరరాశి నిర్మాణం Curl_ssl Curl_ssl_schannel = {
  { CURLSSLBACKEND_SCHANNEL, "schannel" }, /* info */

  SSLSUPP_CERTINFO |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_CA_CACHE |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST,

  పరిమాణం(నిర్మాణం schannel_ssl_backend_దత్తాంశం),

  schannel_init,                     /* init */
  schannel_cleanup,                  /* cleanup */
  schannel_సంస్కరణ,                  /*సంస్కరణ*/
  schannel_shutdown,                 /* shutdown */
  schannel_దత్తాంశం_pending,             /* data_pending */
  schannel_random,                   /* random */
  ఖాళీ,                              /* cert_status_request */
  schannel_connect,                  /* connect */
  Curl_ssl_adjust_pollset,           /* adjust_pollset */
  schannel_get_internals,            /* get_internals */
  schannel_close,                    /* close_one */
  ఖాళీ,                              /* close_all */
  ఖాళీ,                              /* engines_list */
  schannel_sha256sum,                /* sha256sum */
  schannel_recv,                     /* recv decrypted data */
  schannel_send,                     /* send data to encrypt */
  ఖాళీ,                              /* get_channel_binding */
};

#అయితే_ముగింపు /* USE_SCHANNEL */
