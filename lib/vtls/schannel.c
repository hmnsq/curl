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
#अन्तर्भाव "curl_setup.h"

#यदि_परिभाषितम् USE_SCHANNEL

#यदि_अपरिभाषितम् USE_WINDOWS_SSPI
#दोषः "cannot compile Schannel support without SSPI."
#यदि_अन्तः

#अन्तर्भाव "vtls/schannel.h"
#अन्तर्भाव "vtls/schannel_int.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "vtls/vtls_int.h"
#अन्तर्भाव "vtls/vtls_scache.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "connect.h" /* for the connect timeout */
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "strerror.h"
#अन्तर्भाव "select.h" /* for the socket readiness */
#अन्तर्भाव "curlx/win32-fopen.h"
#अन्तर्भाव "curlx/multibyte.h"
#अन्तर्भाव "vtls/x509asn1.h"
#अन्तर्भाव "curlx/version_win32.h"
#अन्तर्भाव "curlx/winapi.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "curl_sha256.h"

/* Some verbose debug messages are wrapped by SCH_DEV() instead of DEBUGF()
 * and only shown if CURL_SCHANNEL_DEV_DEBUG was defined at build time. These
 * messages are extra verbose and intended for curl developers debugging
 * Schannel recv decryption and renegotiation.
 */
#यदि_परिभाषितम् CURL_SCHANNEL_DEV_DEBUG
#परिभाषय SCH_DEV(x) x
#परिभाषय SCH_DEV_SHOWBOOL(x)                                     \
  कुरु {                                                          \
    infof(दत्त, "schannel: " #x " %s", (x) ? "TRUE" : "FALSE"); \
  } यावत्(0)
#अन्यथा
#परिभाषय SCH_DEV(x) कुरु {} यावत्(0)
#परिभाषय SCH_DEV_SHOWBOOL(x) कुरु {} यावत्(0)
#यदि_अन्तः

/* Offered by mingw-w64 v8+, MS SDK 7.0A/VS2010+ */
#यदि_अपरिभाषितम् SP_PROT_TLS1_0_CLIENT
#परिभाषय SP_PROT_TLS1_0_CLIENT           SP_PROT_TLS1_CLIENT
#यदि_अन्तः
#यदि_अपरिभाषितम् SP_PROT_TLS1_1_CLIENT
#परिभाषय SP_PROT_TLS1_1_CLIENT           0x00000200
#यदि_अन्तः
#यदि_अपरिभाषितम् SP_PROT_TLS1_2_CLIENT
#परिभाषय SP_PROT_TLS1_2_CLIENT           0x00000800
#यदि_अन्तः

/* Offered by mingw-w64 v8+, MS SDK 10.0.15063.0/VS2017 15.1+ */
#यदि_अपरिभाषितम् SP_PROT_TLS1_3_CLIENT
#परिभाषय SP_PROT_TLS1_3_CLIENT           0x00002000
#यदि_अन्तः
/* Offered by mingw-w64 v8+, MS SDK 8.1/VS2013+ */
#यदि_अपरिभाषितम् SCH_USE_STRONG_CRYPTO
#परिभाषय SCH_USE_STRONG_CRYPTO           0x00400000
#यदि_अन्तः

/* Offered by mingw-w64 v10+, MS SDK 7.0A/VS2010+ */
#यदि_अपरिभाषितम् SECBUFFER_ALERT
#परिभाषय SECBUFFER_ALERT                 17
#यदि_अन्तः

/* Both schannel buffer sizes must be > 0 */
#परिभाषय CURL_SCHANNEL_बफर_INIT_आकार   4096
#परिभाषय CURL_SCHANNEL_बफर_FREE_आकार   1024

#परिभाषय CERT_THUMBPRINT_STR_LEN 40
#परिभाषय CERT_THUMBPRINT_दत्त_LEN 20

/* Uncomment to force verbose output
 * #define infof(x, y, ...) curl_mprintf(y, __VA_ARGS__)
 * #define failf(x, y, ...) curl_mprintf(y, __VA_ARGS__)
 */

/* Offered by mingw-w64 v4+, MS SDK 6.0A/VS2008+ */
#यदि_अपरिभाषितम् PKCS12_NO_PERSIST_कुञ्चिका
#परिभाषय PKCS12_NO_PERSIST_कुञ्चिका 0x00008000
#यदि_अन्तः

/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#यदि_अपरिभाषितम् CERT_FIND_HAS_वैयक्तिकम्_कुञ्चिका
#परिभाषय CERT_FIND_HAS_वैयक्तिकम्_कुञ्चिका (21 << CERT_COMPARE_सरणम्)
#यदि_अन्तः

/* key to use at `multi->proto_hash` */
#परिभाषय MPROTO_SCHANNEL_CERT_SHARE_कुञ्चिका   "tls:schannel:cert:share"

स्थिर bool s_win_has_alpn;

/* Offered by mingw-w64 v9+, MS SDK 8.1/VS2013+ */
#यदि_अपरिभाषितम् SECBUFFER_APPLICATION_PROTOCOLS
#परिभाषय SECBUFFER_APPLICATION_PROTOCOLS 18
#परिभाषय SECPKG_ATTR_APPLICATION_PROTOCOL 35

प्रकारनिर्देश गणना {
  SecApplicationProtocolNegotiationExt_None,
  SecApplicationProtocolNegotiationExt_NPN,
  SecApplicationProtocolNegotiationExt_ALPN
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT;

प्रकारनिर्देश गणना {
  Sec_Application_Protocol_Negotiation_स्थितिः_None,
  Sec_Application_Protocol_Negotiation_स्थितिः_Success,
  Sec_Application_Protocol_Negotiation_स्थितिः_Selected_Client_Only
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_स्थितिः;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
प्रकारनिर्देश संरचना {
  SEC_APPLICATION_PROTOCOL_NEGOTIATION_स्थितिः Proto_Nego_स्थितिः;
  SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT ProtoNegoExt;
  अचिह्नित वर्ण Protocol_Id_आकार;
  अचिह्नित वर्ण ProtocolId[0xff];
} SecPkgContext_ApplicationProtocol;
#यदि_अन्तः

स्थिर शून्य Init_Sec_बफर(Sec_बफर *बफर, अचिह्नित दीर्घ BufType,
                          शून्य *Buf_दत्त_Ptr, अचिह्नित दीर्घ Buf_Byte_आकार)
{
  बफर->cb_बफर = Buf_Byte_आकार;
  बफर->बफर_Type = BufType;
  बफर->pv_बफर = Buf_दत्त_Ptr;
}

स्थिर शून्य Init_Sec_बफर_Desc(Sec_बफर_Desc *desc, Sec_बफर *BufArr,
                              अचिह्नित दीर्घ अङ्कः_Arr_Elem)
{
  desc->ul_संस्करणम् = SECBUFFER_संस्करणम्;
  desc->pBuffers = BufArr;
  desc->cBuffers = अङ्कः_Arr_Elem;
}

स्थिर CURLcode schannel_समुच्चयः_ssl_संस्करणम्_min_max(DWORD *enabled_protocols,
                                                 संरचना Curl_cfilter *cf,
                                                 संरचना Curl_easy *दत्त)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  दीर्घ ssl_संस्करणम् = conn_config->संस्करणम्_2;
  दीर्घ ssl_संस्करणम्_max = (दीर्घ)conn_config->संस्करणम्_max;
  दीर्घ i = ssl_संस्करणम्;

  विकल्प(ssl_संस्करणम्_max) {
  पक्ष CURL_SSLVERSION_MAX_NONE:
  पक्ष CURL_SSLVERSION_MAX_DEFAULT:

    /* Windows Server 2022 and newer (including Windows 11) support TLS 1.3
       built-in. Previous builds of Windows 10 had broken TLS 1.3
       implementations that could be enabled via registry. */
    यदि(curlx_verify_windows_संस्करणम्(10, 0, 20348, PLATFORM_WINNT,
                                    संस्करणम्_GREATER_THAN_EQUAL)) {
      ssl_संस्करणम्_max = CURL_SSLVERSION_MAX_TLSv1_3;
    }
    अन्यथा /* Windows 10 or older */
      ssl_संस्करणम्_max = CURL_SSLVERSION_MAX_TLSv1_2;

    भङ्ग;
  }

  प्रति(; i <= (ssl_संस्करणम्_max >> 16); ++i) {
    विकल्प(i) {
    पक्ष CURL_SSLVERSION_TLSv1_0:
      *enabled_protocols |= SP_PROT_TLS1_0_CLIENT;
      भङ्ग;
    पक्ष CURL_SSLVERSION_TLSv1_1:
      *enabled_protocols |= SP_PROT_TLS1_1_CLIENT;
      भङ्ग;
    पक्ष CURL_SSLVERSION_TLSv1_2:
      *enabled_protocols |= SP_PROT_TLS1_2_CLIENT;
      भङ्ग;
    पक्ष CURL_SSLVERSION_TLSv1_3:

      /* Windows Server 2022 or newer */
      यदि(curlx_verify_windows_संस्करणम्(10, 0, 20348, PLATFORM_WINNT,
                                      संस्करणम्_GREATER_THAN_EQUAL)) {
        *enabled_protocols |= SP_PROT_TLS1_3_CLIENT;
        भङ्ग;
      }
      अन्यथा { /* Windows 10 or older */
        failf(दत्त, "schannel: TLS 1.3 not supported on Windows 10 or older");
        प्रत्यागम CURLE_SSL_CONNECT_ERROR;
      }
    }
  }
  प्रत्यागम CURLE_OK;
}

#परिभाषय CIPHEROPTION(x) { #x, x }

संरचना algo {
  ध्रुव वर्ण *name;
  पूर्णाङ्क id;
};

स्थिर ध्रुव संरचना algo algs[] = {
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
  CIPHEROPTION(CALG_SCHANNEL_MAC_कुञ्चिका),
  CIPHEROPTION(CALG_SCHANNEL_ENC_कुञ्चिका),
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
#यदि_परिभाषितम् CALG_ECMQV
  CIPHEROPTION(CALG_ECMQV),
#यदि_अन्तः
  CIPHEROPTION(CALG_ECDSA),
/* Offered by mingw-w64 v7+, MS SDK 7.0A/VS2010+ */
#यदि_परिभाषितम् CALG_ECDH_EPHEM
  CIPHEROPTION(CALG_ECDH_EPHEM),
#यदि_अन्तः
  { रिक्त, 0 },
};

स्थिर पूर्णाङ्क get_alg_id_by_name(ध्रुव वर्ण *name)
{
  ध्रुव वर्ण *name_अन्तः = strchr(name, ':');
  size_t n = name_अन्तः ? (size_t)(name_अन्तः - name) : वाक्यदैर्घ्यम्(name);
  पूर्णाङ्क i;

  प्रति(i = 0; algs[i].name; i++) {
    यदि((n == वाक्यदैर्घ्यम्(algs[i].name) && !वाक्ये_तुलय(algs[i].name, name, n)))
      प्रत्यागम algs[i].id;
  }
  प्रत्यागम 0; /* not found */
}

#परिभाषय अङ्कः_CIPHERS 47 /* There are a maximum of 47 options listed above */

स्थिर CURLcode समुच्चयः_ssl_ciphers(SCHANNEL_CRED *schannel_cred, वर्ण *ciphers,
                                ALG_ID *algIds)
{
  ध्रुव वर्ण *startCur = ciphers;
  पूर्णाङ्क alg_गणनम् = 0;
  यावत्(startCur && *startCur && (alg_गणनम् < अङ्कः_CIPHERS)) {
    curl_off_t alg;
    यदि(curlx_str_सङ्ख्या(&startCur, &alg, अधिकतम_पूर्णाङ्कः) || !alg)
      alg = get_alg_id_by_name(startCur);

    यदि(alg)
      algIds[alg_गणनम्++] = (ALG_ID)alg;
    अन्यथा यदि(!वाक्ये_तुलय(startCur, "USE_STRONG_CRYPTO",
                     CURL_CSTRLEN("USE_STRONG_CRYPTO")) ||
            !वाक्ये_तुलय(startCur, "SCH_USE_STRONG_CRYPTO",
                     CURL_CSTRLEN("SCH_USE_STRONG_CRYPTO")))
      schannel_cred->dwFlags |= SCH_USE_STRONG_CRYPTO;
    अन्यथा
      प्रत्यागम CURLE_SSL_CIPHER;
    startCur = strchr(startCur, ':');
    यदि(startCur)
      startCur++;
  }
  schannel_cred->palgSupportedAlgs = algIds;
  schannel_cred->cSupportedAlgs = (DWORD)alg_गणनम्;
  प्रत्यागम CURLE_OK;
}

/* Function allocates memory for store_path only if CURLE_OK is returned */
स्थिर CURLcode get_cert_location(TCHAR *मार्गः, DWORD *store_name,
                                  TCHAR **store_मार्गः, TCHAR **thumbprint)
{
  TCHAR *sep;
  TCHAR *store_मार्गः_start;
  size_t store_name_len;

  sep = _tcschr(मार्गः, _TEXT('\\'));
  यदि(!sep)
    प्रत्यागम CURLE_SSL_CERTPROBLEM;

  store_name_len = sep - मार्गः;

  यदि(_tcsncmp(मार्गः, _TEXT("CurrentUser"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_USER;
  अन्यथा यदि(_tcsncmp(मार्गः, _TEXT("LocalMachine"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE;
  अन्यथा यदि(_tcsncmp(मार्गः, _TEXT("CurrentService"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_SERVICE;
  अन्यथा यदि(_tcsncmp(मार्गः, _TEXT("Services"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_SERVICES;
  अन्यथा यदि(_tcsncmp(मार्गः, _TEXT("Users"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_USERS;
  अन्यथा यदि(_tcsncmp(मार्गः, _TEXT("CurrentUserGroupPolicy"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_USER_गणः_POLICY;
  अन्यथा यदि(_tcsncmp(मार्गः, _TEXT("LocalMachineGroupPolicy"), store_name_len) ==
          0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE_गणः_POLICY;
  अन्यथा यदि(_tcsncmp(मार्गः, _TEXT("LocalMachineEnterprise"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE_ENTERPRISE;
  अन्यथा
    प्रत्यागम CURLE_SSL_CERTPROBLEM;

  store_मार्गः_start = sep + 1;

  sep = _tcschr(store_मार्गः_start, _TEXT('\\'));
  यदि(!sep)
    प्रत्यागम CURLE_SSL_CERTPROBLEM;

  *thumbprint = sep + 1;
  यदि(_tcslen(*thumbprint) != CERT_THUMBPRINT_STR_LEN)
    प्रत्यागम CURLE_SSL_CERTPROBLEM;

  *sep = _TEXT('\0');
  *store_मार्गः = curlx_tcsdup(store_मार्गः_start);
  *sep = _TEXT('\\');
  यदि(!*store_मार्गः)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode get_client_cert(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त,
                                HCERTSTORE *बहिर्गमनम्_cert_store,
                                PCCERT_CONTEXT *बहिर्गमनम्_cert_context)
{
  संरचना ssl_filter_config *sslc = Curl_ssl_cf_get_filter_config(cf);
  PCCERT_CONTEXT client_cert = रिक्त;
  HCERTSTORE client_cert_store = रिक्त;
  CURLcode परिणाम = CURLE_OK;

  यदि(sslc->clientcert || sslc->cert_blob) {
    DWORD cert_store_name = 0;
    TCHAR *cert_store_मार्गः = रिक्त;
    TCHAR *cert_thumbprint_str = रिक्त;
    TCHAR cert_thumbprint_buf[CERT_THUMBPRINT_STR_LEN + 1];
    CRYPT_HASH_BLOB cert_thumbprint;
    BYTE cert_thumbprint_दत्त[CERT_THUMBPRINT_दत्त_LEN];
    HCERTSTORE cert_store = रिक्त;
    FILE *f_अन्तर्_Cert = रिक्त;
    शून्य *certdata = रिक्त;
    size_t certsize = 0;
    bool blob = !!sslc->cert_blob;

    यदि(blob) {
      certdata = sslc->cert_blob->दत्त;
      certsize = sslc->cert_blob->len;
    }
    अन्यथा {
      TCHAR *cert_मार्गः =
        curlx_convert_UTF8_गन्तव्यम्_tchar(sslc->clientcert);
      यदि(!cert_मार्गः)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

      परिणाम = get_cert_location(cert_मार्गः, &cert_store_name,
                                 &cert_store_मार्गः, &cert_thumbprint_str);

      /* 'cert_thumbprint_str' points in to the allocated 'cert_path', copy
         the data. The string is verified to be CERT_THUMBPRINT_STR_LEN bytes
         long within the get_cert_location() function. */
      यदि(!परिणाम && cert_thumbprint_str) {
        स्मृतिं_प्रतिलिख(cert_thumbprint_buf, cert_thumbprint_str,
               परिमाण(cert_thumbprint_buf));
        cert_thumbprint_str = cert_thumbprint_buf;
      }

      curlx_free(cert_मार्गः);
      यदि(परिणाम && (sslc->clientcert[0] != '\0'))
        f_अन्तर्_Cert = curlx_fopen(sslc->clientcert, "rb");

      यदि(परिणाम && !f_अन्तर्_Cert) {
        failf(दत्त, "schannel: Failed to get certificate location"
              " or file for %s",
              sslc->clientcert);
        प्रत्यागम परिणाम;
      }
    }

    यदि((f_अन्तर्_Cert || blob) && sslc->cert_type &&
       !curl_strequal(sslc->cert_type, "P12")) {
      failf(दत्त, "schannel: certificate format compatibility error "
            "for %s",
            blob ? "(memory blob)" : sslc->clientcert);
      curlx_free(cert_store_मार्गः);
      यदि(f_अन्तर्_Cert)
        curlx_fclose(f_अन्तर्_Cert);
      प्रत्यागम CURLE_SSL_CERTPROBLEM;
    }

    यदि(f_अन्तर्_Cert || blob) {
      /* Reading a .p12 or .pfx file, like the example at bottom of
         https://learn.microsoft.com/archive/msdn-technet-forums/3e7bc95f-b21a-4bcd-bd2c-7f996718cae5
       */
      CRYPT_दत्त_BLOB datablob;
      WCHAR *pszPassword;
      size_t pwd_len = 0;
      पूर्णाङ्क cert_find_flags;
      ध्रुव वर्ण *cert_showfilename_error = blob ?
        "(memory blob)" : sslc->clientcert;
      curlx_free(cert_store_मार्गः);
      यदि(f_अन्तर्_Cert) {
        दीर्घ cert_tell = 0;
        bool continue_reading = fseek(f_अन्तर्_Cert, 0, SEEK_END) == 0;
        यदि(continue_reading) {
          cert_tell = ftell(f_अन्तर्_Cert);
          यदि(cert_tell < 0)
            continue_reading = FALSE;
          अन्यथा
            certsize = (size_t)cert_tell;
        }
        यदि(continue_reading)
          continue_reading = fseek(f_अन्तर्_Cert, 0, SEEK_SET) == 0;
        यदि(continue_reading && (certsize < CURL_MAX_निवेशः_दीर्घता))
          certdata = curlx_malloc(certsize + 1);
        यदि(!certdata || ((पूर्णाङ्क)fread(certdata, certsize, 1, f_अन्तर्_Cert) != 1))
          continue_reading = FALSE;
        curlx_fclose(f_अन्तर्_Cert);
        यदि(!continue_reading) {
          failf(दत्त, "schannel: Failed to read cert file %s",
                sslc->clientcert);
          curlx_free(certdata);
          प्रत्यागम CURLE_SSL_CERTPROBLEM;
        }
      }

      /* Convert key-pair data to the in-memory certificate store */
      datablob.pb_दत्त = (BYTE *)certdata;
      datablob.cb_दत्त = (DWORD)certsize;

      यदि(sslc->कुञ्चिका_passwd)
        pwd_len = वाक्यदैर्घ्यम्(sslc->कुञ्चिका_passwd);
      pszPassword = curlx_malloc(परिमाण(WCHAR) * (pwd_len + 1));
      यदि(pszPassword) {
        पूर्णाङ्क str_w_len = 0;
        यदि(pwd_len > 0)
          str_w_len = Multi_Byte_गन्तव्यम्_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                          sslc->कुञ्चिका_passwd, (पूर्णाङ्क)pwd_len,
                                          pszPassword, (पूर्णाङ्क)(pwd_len + 1));

        यदि((str_w_len >= 0) && (str_w_len <= (पूर्णाङ्क)pwd_len))
          pszPassword[str_w_len] = 0;
        अन्यथा
          pszPassword[0] = 0;

        cert_store = PFXImportCertStore(&datablob, pszPassword,
                                        PKCS12_NO_PERSIST_कुञ्चिका);
        curlx_memzero(pszPassword, परिमाण(WCHAR) * (pwd_len + 1));
        curlx_free(pszPassword);
      }
      यदि(!blob)
        curlx_free(certdata);
      यदि(!cert_store) {
        DWORD errorcode = Get_चरमः_Error();
        यदि(errorcode == ERROR_INVALID_PASSWORD)
          failf(दत्त, "schannel: Failed to import cert file %s, "
                "password is bad",
                cert_showfilename_error);
        अन्यथा
          failf(दत्त, "schannel: Failed to import cert file %s, "
                "last error is 0x%08lx",
                cert_showfilename_error, errorcode);
        प्रत्यागम CURLE_SSL_CERTPROBLEM;
      }

      /* CERT_FIND_HAS_PRIVATE_KEY is only available in Windows 8 / Server
         2012, (NT 6.2). For older versions we use CURL_FIND_ANY. */
      यदि(curlx_verify_windows_संस्करणम्(6, 2, 0, PLATFORM_WINNT,
                                      संस्करणम्_GREATER_THAN_EQUAL))
        cert_find_flags = CERT_FIND_HAS_वैयक्तिकम्_कुञ्चिका;
      अन्यथा
        cert_find_flags = CERT_FIND_ANY;

      client_cert =
        Cert_Find_Certificate_अन्तर्_Store(cert_store,
                                   X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                   cert_find_flags, रिक्त, रिक्त);

      यदि(!client_cert) {
        failf(दत्त, "schannel: Failed to get certificate from file %s"
              ", last error is 0x%08lx",
              cert_showfilename_error, Get_चरमः_Error());
        CertCloseStore(cert_store, 0);
        प्रत्यागम CURLE_SSL_CERTPROBLEM;
      }
    }
    अन्यथा {
      cert_store =
        CertOpenStore(
#यदि_परिभाषितम् UNICODE
                      CERT_STORE_PROV_SYSTEM_W,
#अन्यथा
                      CERT_STORE_PROV_SYSTEM_A,
#यदि_अन्तः
                      0,
                      (HCRYPTPROV)रिक्त,
                      CERT_STORE_OPEN_EXISTING_FLAG | cert_store_name,
                      cert_store_मार्गः);
      यदि(!cert_store) {
        वर्ण *मार्गः_utf8 =
          curlx_convert_tchar_गन्तव्यम्_UTF8(cert_store_मार्गः);
        failf(दत्त, "schannel: Failed to open cert store %lx %s, "
              "last error is 0x%08lx",
              cert_store_name, (मार्गः_utf8 ? मार्गः_utf8 : "(unknown)"),
              Get_चरमः_Error());
        curlx_free(cert_store_मार्गः);
        curlx_free(मार्गः_utf8);
        प्रत्यागम CURLE_SSL_CERTPROBLEM;
      }
      curlx_free(cert_store_मार्गः);

      cert_thumbprint.pb_दत्त = cert_thumbprint_दत्त;
      cert_thumbprint.cb_दत्त = CERT_THUMBPRINT_दत्त_LEN;

      यदि(!Crypt_String_गन्तव्यम्_द्विमान(cert_thumbprint_str,
                              CERT_THUMBPRINT_STR_LEN,
                              CRYPT_STRING_HEX,
                              cert_thumbprint_दत्त,
                              &cert_thumbprint.cb_दत्त,
                              रिक्त, रिक्त)) {
        CertCloseStore(cert_store, 0);
        प्रत्यागम CURLE_SSL_CERTPROBLEM;
      }

      client_cert =
        Cert_Find_Certificate_अन्तर्_Store(cert_store,
                                   X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                   CERT_FIND_HASH, &cert_thumbprint, रिक्त);
      यदि(!client_cert) {
        /* CRYPT_E_NOT_FOUND / E_INVALIDARG */
        CertCloseStore(cert_store, 0);
        failf(दत्त, "schannel: client cert not found in cert store");
        प्रत्यागम CURLE_SSL_CERTPROBLEM;
      }
    }
    client_cert_store = cert_store;
  }

  *बहिर्गमनम्_cert_store = client_cert_store;
  *बहिर्गमनम्_cert_context = client_cert;

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode acquire_sspi_handle(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *दत्त,
                                    संरचना schannel_ssl_backend_दत्त *backend,
                                    PCCERT_CONTEXT client_cert,
                                    DWORD flags,
                                    DWORD enabled_protocols)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SECURITY_स्थितिः sspi_स्थितिः = SEC_E_OK;
  CURLcode परिणाम;

  /* We support TLS 1.3 starting in Windows Server 2022 or later
   * (OS build 20348) */
  यदि(!conn_config->cipher_list &&
     curlx_verify_windows_संस्करणम्(10, 0, 20348, PLATFORM_WINNT,
                                  संस्करणम्_GREATER_THAN_EQUAL)) {

    SCH_CREDENTIALS credentials = { 0 };
    TLS_PARAMETERS tls_parameters = { 0 };
    CRYPTO_SETTINGS crypto_settings[1];
    PCCERT_CONTEXT client_certs[1];

    यदि(client_cert)
      client_certs[0] = client_cert;
    अन्यथा
      client_certs[0] = रिक्त;

    स्मृतिं_पूरय(crypto_settings, 0, परिमाण(crypto_settings));

    tls_parameters.pDisabledCrypto = crypto_settings;

    /* The number of blocked suites */
    tls_parameters.cDisabledCrypto = (DWORD)0;
    credentials.pTlsParameters = &tls_parameters;
    credentials.cTlsParameters = 1;

    credentials.dw_संस्करणम् = SCH_CREDENTIALS_संस्करणम्;
    credentials.dwFlags = flags | SCH_USE_STRONG_CRYPTO;

    credentials.pTlsParameters->grbitDisabledProtocols = ~enabled_protocols;

    यदि(client_certs[0]) {
      credentials.cCreds = 1;
      credentials.paCred = client_certs;
    }

    sspi_स्थितिः =
      Curl_pSecFn->AcquireCredentialsHandle(रिक्त, CURL_UNCONST(UNISP_NAME),
                                            SECPKG_CRED_OUTBOUND, रिक्त,
                                            &credentials, रिक्त, रिक्त,
                                            &backend->cred->cred_handle, रिक्त);
  }
  अन्यथा {
    /* Pre-Windows 10 1809 or the user set a legacy algorithm list.
       Schannel does not negotiate TLS 1.3 when SCHANNEL_CRED is used. */
    ALG_ID algIds[अङ्कः_CIPHERS];
    वर्ण *ciphers = conn_config->cipher_list;
    SCHANNEL_CRED schannel_cred = { 0 };
    PCCERT_CONTEXT client_certs[1];

    यदि(client_cert)
      client_certs[0] = client_cert;
    अन्यथा
      client_certs[0] = रिक्त;

    schannel_cred.dw_संस्करणम् = SCHANNEL_CRED_संस्करणम्;
    schannel_cred.dwFlags = flags;
    schannel_cred.grbitEnabledProtocols = enabled_protocols;

    यदि(ciphers) {
      यदि((enabled_protocols & SP_PROT_TLS1_3_CLIENT)) {
        यदि(!(enabled_protocols & ~SP_PROT_TLS1_3_CLIENT)) {
          failf(दत्त, "schannel: TLS 1.3 is not supported with a cipher list; "
                "remove the cipher list or allow a lower TLS version");
          प्रत्यागम CURLE_SSL_CONNECT_ERROR;
        }
        infof(दत्त, "schannel: WARNING: This version of Schannel "
              "negotiates a less-secure TLS version than TLS 1.3 because the "
              "user set an algorithm cipher list.");
      }
      परिणाम = समुच्चयः_ssl_ciphers(&schannel_cred, ciphers, algIds);
      यदि(परिणाम) {
        failf(दत्त, "schannel: Failed setting algorithm cipher list");
        प्रत्यागम परिणाम;
      }
    }
    अन्यथा {
      schannel_cred.dwFlags = flags | SCH_USE_STRONG_CRYPTO;
    }

    यदि(client_certs[0]) {
      schannel_cred.cCreds = 1;
      schannel_cred.paCred = client_certs;
    }

    sspi_स्थितिः =
      Curl_pSecFn->AcquireCredentialsHandle(रिक्त, CURL_UNCONST(UNISP_NAME),
                                            SECPKG_CRED_OUTBOUND, रिक्त,
                                            &schannel_cred, रिक्त, रिक्त,
                                            &backend->cred->cred_handle, रिक्त);
  }

  यदि(sspi_स्थितिः != SEC_E_OK) {
    वर्ण बफर[STRERROR_LEN];
    failf(दत्त, "schannel: AcquireCredentialsHandle failed: %s",
          Curl_sspi_strerror(sspi_स्थितिः, बफर, परिमाण(बफर)));
    विकल्प(sspi_स्थितिः) {
    पक्ष SEC_E_INSUFFICIENT_स्मृतिः:
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    पक्ष SEC_E_NO_CREDENTIALS:
    पक्ष SEC_E_SECPKG_NOT_प्राप्तम्:
    पक्ष SEC_E_NOT_OWNER:
    पक्ष SEC_E_UNKNOWN_CREDENTIALS:
    पक्ष SEC_E_आन्तरिकम्_ERROR:
    स्वभाव:
      प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode schannel_acquire_credential_handle(संरचना Curl_cfilter *cf,
                                                   संरचना Curl_easy *दत्त)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  PCCERT_CONTEXT client_cert = रिक्त;
  HCERTSTORE client_cert_store = रिक्त;
  CURLcode परिणाम;

  /* setup Schannel API options */
  DWORD flags = 0;
  DWORD enabled_protocols = 0;

  संरचना schannel_ssl_backend_दत्त *backend =
    (संरचना schannel_ssl_backend_दत्त *)connssl->backend;

  DEBUGASSERT(backend);

  यदि(conn_config->verifypeer) {
    यदि(backend->use_manual_cred_validation)
      flags = SCH_CRED_MANUAL_CRED_VALIDATION;
    अन्यथा
      flags = SCH_CRED_AUTO_CRED_VALIDATION;

    यदि(conn_config->no_revoke) {
      flags |= SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
               SCH_CRED_IGNORE_REVOCATION_OFFLINE;

      DEBUGF(infof(दत्त, "schannel: disabled server certificate revocation "
                         "checks"));
    }
    अन्यथा यदि(conn_config->revoke_best_effort) {
      flags |= SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
               SCH_CRED_IGNORE_REVOCATION_OFFLINE |
               SCH_CRED_REVOCATION_CHECK_CHAIN;

      DEBUGF(infof(दत्त, "schannel: ignore revocation offline errors"));
    }
    अन्यथा {
      flags |= SCH_CRED_REVOCATION_CHECK_CHAIN;

      DEBUGF(infof(दत्त, "schannel: checking server certificate revocation"));
    }
  }
  अन्यथा {
    flags = SCH_CRED_MANUAL_CRED_VALIDATION |
            SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
            SCH_CRED_IGNORE_REVOCATION_OFFLINE;
    DEBUGF(infof(दत्त, "schannel: disabled server cert revocation checks"));
  }

  यदि(!conn_config->verifyhost) {
    flags |= SCH_CRED_NO_SERVERNAME_CHECK;
    DEBUGF(infof(दत्त, "schannel: verifyhost setting prevents Schannel from "
                       "comparing the supplied target name with the subject "
                       "names in server certificates."));
  }

  यदि(!conn_config->auto_client_cert) {
    flags &= ~(DWORD)SCH_CRED_USE_DEFAULT_CREDS;
    flags |= SCH_CRED_NO_DEFAULT_CREDS;
    infof(दत्त, "schannel: disabled automatic use of client certificate");
  }
  अन्यथा
    infof(दत्त, "schannel: enabled automatic use of client certificate");

  DEBUGASSERT(conn_config->संस्करणम्_2 != CURL_SSLVERSION_DEFAULT);
  विकल्प(conn_config->संस्करणम्_2) {
  पक्ष CURL_SSLVERSION_TLSv1:
  पक्ष CURL_SSLVERSION_TLSv1_0:
  पक्ष CURL_SSLVERSION_TLSv1_1:
  पक्ष CURL_SSLVERSION_TLSv1_2:
  पक्ष CURL_SSLVERSION_TLSv1_3: {
    परिणाम = schannel_समुच्चयः_ssl_संस्करणम्_min_max(&enabled_protocols, cf, दत्त);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    भङ्ग;
  }
  पक्ष CURL_SSLVERSION_SSLv3:
  पक्ष CURL_SSLVERSION_SSLv2:
    failf(दत्त, "SSL versions not supported");
    प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
  स्वभाव:
    failf(दत्त, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

  परिणाम = get_client_cert(cf, दत्त, &client_cert_store, &client_cert);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  /* allocate memory for the reusable credential handle */
  backend->cred = (संरचना Curl_schannel_cred *)
    curlx_calloc(1, परिमाण(संरचना Curl_schannel_cred));
  यदि(!backend->cred) {
    failf(दत्त, "schannel: unable to allocate memory");

    यदि(client_cert)
      CertFreeCertificateContext(client_cert);
    यदि(client_cert_store)
      CertCloseStore(client_cert_store, 0);

    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
  backend->cred->refcount = 1;

  /* Since we did not persist the key, we need to extend the store's
   * lifetime until the end of the connection
   */
  backend->cred->client_cert_store = client_cert_store;

  परिणाम = acquire_sspi_handle(cf, दत्त, backend, client_cert,
                               flags, enabled_protocols);

  यदि(client_cert)
    CertFreeCertificateContext(client_cert);

  प्रत्यागम परिणाम;
}

स्थिर CURLcode schannel_connect_step1(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *दत्त)
{
  size_t written = 0;
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना schannel_ssl_backend_दत्त *backend =
    (संरचना schannel_ssl_backend_दत्त *)connssl->backend;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  Sec_बफर outbuf;
  Sec_बफर_Desc outbuf_desc;
  Sec_बफर inbuf;
  Sec_बफर_Desc inbuf_desc;
  अचिह्नित वर्ण alpn_बफर[128];
  SECURITY_स्थितिः sspi_स्थितिः = SEC_E_OK;
  CURLcode परिणाम;

  DEBUGASSERT(backend);
  DEBUGF(infof(दत्त, "schannel: SSL/TLS connection with %s port %d (step 1/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->पत्तनम्));

  backend->use_alpn = connssl->alpn && s_win_has_alpn;

  यदि(conn_config->CAfile || conn_config->ca_info_blob) {
    यदि(curlx_verify_windows_संस्करणम्(6, 1, 0, PLATFORM_WINNT,
                                    संस्करणम्_GREATER_THAN_EQUAL)) {
      backend->use_manual_cred_validation = TRUE;
    }
    अन्यथा {
      failf(दत्त, "schannel: this version of Windows is too old to support "
                  "certificate verification via CA bundle file.");
      प्रत्यागम CURLE_SSL_CACERT_BADFILE;
    }
  }
  अन्यथा
    backend->use_manual_cred_validation = FALSE;

  backend->cred = रिक्त;

  /* check for an existing reusable credential handle */
  यदि(Curl_ssl_scache_use(cf, दत्त)) {
    संरचना Curl_schannel_cred *old_cred;
    Curl_ssl_scache_तालकम्(दत्त);
    old_cred = Curl_ssl_scache_get_obj(cf, दत्त, connssl->peer.scache_कुञ्चिका);
    यदि(old_cred) {
      backend->cred = old_cred;
      DEBUGF(infof(दत्त, "schannel: reusing existing credential handle"));

      /* increment the reference counter of the credential/session handle */
      backend->cred->refcount++;
      DEBUGF(infof(दत्त,
                   "schannel: incremented credential handle refcount = %d",
                   backend->cred->refcount));
    }
    Curl_ssl_scache_unlock(दत्त);
  }

  यदि(!backend->cred) {
    वर्ण *snihost;
    परिणाम = schannel_acquire_credential_handle(cf, दत्त);
    यदि(परिणाम || !backend->cred)
      प्रत्यागम परिणाम;
    /* schannel_acquire_credential_handle() sets backend->cred accordingly or
       it returns error otherwise. */

    /* A hostname associated with the credential is needed by
       InitializeSecurityContext for SNI and other reasons. */
    snihost = connssl->peer.sni ?
      connssl->peer.sni : connssl->peer.origin->hostname;
    backend->cred->sni_hostname = curlx_convert_UTF8_गन्तव्यम्_tchar(snihost);
    यदि(!backend->cred->sni_hostname)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  /* Warn if SNI is disabled due to use of an IP address */
  यदि(connssl->peer.type != CURL_SSL_PEER_DNS) {
    infof(दत्त, "schannel: using IP address, SNI is not supported by OS.");
  }

  यदि(backend->use_alpn) {
    पूर्णाङ्क cur = 0;
    पूर्णाङ्क list_start_सूचकांक = 0;
    अचिह्नित पूर्णाङ्क *extension_len = रिक्त;
    अचिह्नित ह्रस्व *list_len = रिक्त;
    ध्रुव अचिह्नित पूर्णाङ्क uiALPN = SecApplicationProtocolNegotiationExt_ALPN;
    संरचना alpn_proto_buf proto;

    /* The first four bytes is an unsigned int indicating number
       of bytes of data in the rest of the buffer. */
    extension_len = (अचिह्नित पूर्णाङ्क *)(शून्य *)&alpn_बफर[cur];
    cur += (पूर्णाङ्क)परिमाण(अचिह्नित पूर्णाङ्क);

    /* The next four bytes are an indicator that this buffer contains
       ALPN data, as opposed to NPN, for example. */
    स्मृतिं_प्रतिलिख(&alpn_बफर[cur], &uiALPN, परिमाण(uiALPN));
    cur += (पूर्णाङ्क)परिमाण(अचिह्नित पूर्णाङ्क);

    /* The next two bytes is an unsigned short indicating the number
       of bytes used to list the preferred protocols. */
    list_len = (अचिह्नित ह्रस्व *)(शून्य *)&alpn_बफर[cur];
    cur += (पूर्णाङ्क)परिमाण(अचिह्नित ह्रस्व);

    list_start_सूचकांक = cur;

    परिणाम = Curl_alpn_गन्तव्यम्_proto_buf(&proto, connssl->alpn);
    यदि(परिणाम) {
      failf(दत्त, "Error setting ALPN");
      प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }
    स्मृतिं_प्रतिलिख(&alpn_बफर[cur], proto.दत्त, proto.len);
    cur += proto.len;

    *list_len = curlx_uitous(cur - list_start_सूचकांक);
    *extension_len = (अचिह्नित पूर्णाङ्क)(*list_len +
      परिमाण(अचिह्नित पूर्णाङ्क) + परिमाण(अचिह्नित ह्रस्व));

    Init_Sec_बफर(&inbuf, SECBUFFER_APPLICATION_PROTOCOLS, alpn_बफर, cur);
    Init_Sec_बफर_Desc(&inbuf_desc, &inbuf, 1);

    Curl_alpn_गन्तव्यम्_proto_str(&proto, connssl->alpn);
    infof(दत्त, VTLS_INFOF_ALPN_OFFER_1STR, proto.दत्त);
  }
  अन्यथा {
    Init_Sec_बफर(&inbuf, SECBUFFER_रिक्तानि, रिक्त, 0);
    Init_Sec_बफर_Desc(&inbuf_desc, &inbuf, 1);
  }

  /* setup output buffer */
  Init_Sec_बफर(&outbuf, SECBUFFER_रिक्तानि, रिक्त, 0);
  Init_Sec_बफर_Desc(&outbuf_desc, &outbuf, 1);

  /* security request flags */
  backend->req_flags =
    ISC_REQ_SEQUENCE_DETECT | ISC_REQ_REPLAY_DETECT |
    ISC_REQ_CONFIDENTIALITY | ISC_REQ_ALLOCATE_स्मृतिः |
    ISC_REQ_प्रवाहः |
    (!conn_config->auto_client_cert ? ISC_REQ_USE_SUPPLIED_CREDS : 0);

  /* allocate memory for the security context handle */
  backend->ctxt = (संरचना Curl_schannel_ctxt *)
    curlx_calloc(1, परिमाण(संरचना Curl_schannel_ctxt));
  यदि(!backend->ctxt) {
    failf(दत्त, "schannel: unable to allocate memory");
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  /* Schannel InitializeSecurityContext:
     https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-initializesecuritycontextw

     At the moment we do not pass inbuf unless we are using ALPN since we only
     use it for that, and WINE (for which we currently disable ALPN) is giving
     us problems with inbuf regardless. https://github.com/curl/curl/issues/983
   */
  sspi_स्थितिः = Curl_pSecFn->InitializeSecurityContext(
    &backend->cred->cred_handle, रिक्त, backend->cred->sni_hostname,
    backend->req_flags, 0, 0,
    (backend->use_alpn ? &inbuf_desc : रिक्त),
    0, &backend->ctxt->ctxt_handle,
    &outbuf_desc, &backend->ret_flags, रिक्त);

  यदि(sspi_स्थितिः != SEC_I_CONTINUE_NEEDED) {
    वर्ण बफर[STRERROR_LEN];
    curlx_safefree(backend->ctxt);
    विकल्प(sspi_स्थितिः) {
    पक्ष SEC_E_INSUFFICIENT_स्मृतिः:
      failf(दत्त, "schannel: initial InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(sspi_स्थितिः, बफर, परिमाण(बफर)));
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    पक्ष SEC_E_WRONG_PRINCIPAL:
      failf(दत्त, "schannel: SNI or certificate check failed: %s",
            Curl_sspi_strerror(sspi_स्थितिः, बफर, परिमाण(बफर)));
      प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;
#यदि 0
    पक्ष SEC_E_INVALID_HANDLE:
    पक्ष SEC_E_INVALID_TOKEN:
    पक्ष SEC_E_LOGON_DENIED:
    पक्ष SEC_E_TARGET_UNKNOWN:
    पक्ष SEC_E_NO_AUTHENTICATING_AUTHORITY:
    पक्ष SEC_E_आन्तरिकम्_ERROR:
    पक्ष SEC_E_NO_CREDENTIALS:
    पक्ष SEC_E_UNSUPPORTED_FUNCTION:
    पक्ष SEC_E_APPLICATION_PROTOCOL_MISMATCH:
#यदि_अन्तः
    स्वभाव:
      failf(दत्त, "schannel: initial InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(sspi_स्थितिः, बफर, परिमाण(बफर)));
      प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }
  }

  DEBUGF(infof(दत्त, "schannel: sending initial handshake data: "
               "sending %lu bytes.", outbuf.cb_बफर));

  /* send initial handshake data which is now stored in output buffer */
  परिणाम = Curl_conn_cf_send(cf->अग्र, दत्त,
                             (ध्रुव uint8_t *)outbuf.pv_बफर,
                             outbuf.cb_बफर, FALSE,
                             &written);
  Curl_pSecFn->Free_Context_बफर(outbuf.pv_बफर);
  यदि(परिणाम || (outbuf.cb_बफर != written)) {
    failf(दत्त, "schannel: failed to send initial handshake data: "
          "sent %zu of %lu bytes", written, outbuf.cb_बफर);
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGF(infof(दत्त, "schannel: sent initial handshake data: "
               "sent %zu bytes", written));

  backend->recv_unrecoverable_err = CURLE_OK;
  backend->recv_sspi_close_notify = FALSE;
  backend->recv_connection_closed = FALSE;
  backend->recv_renegotiating = FALSE;
  backend->renegotiate_state.started = FALSE;
  backend->encdata_is_incomplete = FALSE;
  backend->shutdown_generated = FALSE;
  backend->shutdown_विस्थापनम् = 0;
  स्मृतिं_पूरय(&backend->shutdown_token, 0, परिमाण(backend->shutdown_token));

  /* continue to second handshake step */
  connssl->connecting_state = ssl_connect_2;

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode schannel_error(संरचना Curl_easy *दत्त,
                               SECURITY_स्थितिः sspi_स्थितिः)
{
  वर्ण बफर[STRERROR_LEN];
  विकल्प(sspi_स्थितिः) {
  पक्ष SEC_E_INSUFFICIENT_स्मृतिः:
    failf(दत्त, "schannel: next InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(sspi_स्थितिः, बफर, परिमाण(बफर)));
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  पक्ष SEC_E_WRONG_PRINCIPAL:
    failf(दत्त, "schannel: SNI or certificate check failed: %s",
          Curl_sspi_strerror(sspi_स्थितिः, बफर, परिमाण(बफर)));
    प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;
  पक्ष SEC_E_UNTRUSTED_मूलम्:
    failf(दत्त, "schannel: %s",
          Curl_sspi_strerror(sspi_स्थितिः, बफर, परिमाण(बफर)));
    प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;
#यदि 0
  पक्ष SEC_E_INVALID_HANDLE:
  पक्ष SEC_E_INVALID_TOKEN:
  पक्ष SEC_E_LOGON_DENIED:
  पक्ष SEC_E_TARGET_UNKNOWN:
  पक्ष SEC_E_NO_AUTHENTICATING_AUTHORITY:
  पक्ष SEC_E_आन्तरिकम्_ERROR:
  पक्ष SEC_E_NO_CREDENTIALS:
  पक्ष SEC_E_UNSUPPORTED_FUNCTION:
  पक्ष SEC_E_APPLICATION_PROTOCOL_MISMATCH:
#यदि_अन्तः
  स्वभाव:
    failf(दत्त, "schannel: next InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(sspi_स्थितिः, बफर, परिमाण(बफर)));
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }
}

स्थिर CURLcode schannel_pkp_pin_peer_pubkey(संरचना Curl_cfilter *cf,
                                             संरचना Curl_easy *दत्त,
                                             ध्रुव वर्ण *pinnedpubkey)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना schannel_ssl_backend_दत्त *backend =
    (संरचना schannel_ssl_backend_दत्त *)connssl->backend;
  CERT_CONTEXT *pCertContextServer = रिक्त;

  /* Result is returned to caller */
  CURLcode परिणाम = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  DEBUGASSERT(backend);

  /* if a path was not specified, do not pin */
  यदि(!pinnedpubkey)
    प्रत्यागम CURLE_OK;

  कुरु {
    SECURITY_स्थितिः sspi_स्थितिः;
    uint8_t *x509_der;
    DWORD x509_der_len;
    संरचना Curl_X509certificate x509_parsed;
    संरचना Curl_asn1Element *pubkey;

    sspi_स्थितिः =
      Curl_pSecFn->Query_Context_गुणाः(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                          &pCertContextServer);

    यदि((sspi_स्थितिः != SEC_E_OK) || !pCertContextServer) {
      वर्ण बफर[STRERROR_LEN];
      failf(दत्त, "schannel: Failed to read remote certificate context: %s",
            Curl_sspi_strerror(sspi_स्थितिः, बफर, परिमाण(बफर)));
      भङ्ग; /* failed */
    }

    यदि(!(((pCertContextServer->dwCertEncodingType & X509_ASN_ENCODING) != 0) &&
         (pCertContextServer->cbCertEncoded > 0)))
      भङ्ग;

    x509_der = (uint8_t *)pCertContextServer->pbCertEncoded;
    x509_der_len = pCertContextServer->cbCertEncoded;
    स्मृतिं_पूरय(&x509_parsed, 0, परिमाण(x509_parsed));
    यदि(Curl_parseX509(&x509_parsed, x509_der, x509_der + x509_der_len))
      भङ्ग;

    pubkey = &x509_parsed.subject_लौकिकम्_कुञ्चिका_Info;
    यदि(!pubkey->शीर्षकम् || pubkey->अन्तः <= pubkey->शीर्षकम्) {
      failf(दत्त, "SSL: failed retrieving public key from server certificate");
      भङ्ग;
    }

    परिणाम = Curl_pin_peer_pubkey(दत्त,
                                  pinnedpubkey,
                                  (ध्रुव अचिह्नित वर्ण *)pubkey->शीर्षकम्,
                                  (size_t)(pubkey->अन्तः - pubkey->शीर्षकम्));
    यदि(परिणाम) {
      failf(दत्त, "SSL: public key does not match pinned public key");
    }
  } यावत्(0);

  यदि(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  प्रत्यागम परिणाम;
}

स्थिर CURLcode ensure_encoding_आकार(संरचना Curl_easy *दत्त,
                                     संरचना sbuffer *encdata,
                                     size_t min_दीर्घता)
{
  size_t आकार;
  DEBUGASSERT(encdata->दीर्घता >= encdata->विस्थापनम्);
  यदि(encdata->दीर्घता < encdata->विस्थापनम्)
    प्रत्यागम CURLE_FAILED_INIT;
  आकार = encdata->दीर्घता - encdata->विस्थापनम्;
  यदि(आकार < CURL_SCHANNEL_बफर_FREE_आकार || encdata->दीर्घता < min_दीर्घता) {
    अचिह्नित वर्ण *बफर;
    size_t दीर्घता = encdata->विस्थापनम् + CURL_SCHANNEL_बफर_FREE_आकार;
    यदि(दीर्घता < min_दीर्घता)
      दीर्घता = min_दीर्घता;
    बफर = curlx_realloc(encdata->बफर, दीर्घता);
    यदि(!बफर) {
      failf(दत्त, "schannel: unable to re-allocate memory");
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }

    encdata->बफर = बफर;
    encdata->दीर्घता = दीर्घता;
    SCH_DEV(infof(दत्त, "schannel: encdata.buffer resized %zu",
                  encdata->दीर्घता));
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ensure_decoding_आकार(संरचना Curl_easy *दत्त,
                                     संरचना sbuffer *decdata,
                                     size_t nowsize,
                                     size_t len)
{
  size_t आकार = nowsize > CURL_SCHANNEL_बफर_FREE_आकार ?
    nowsize : CURL_SCHANNEL_बफर_FREE_आकार;
  DEBUGASSERT(decdata->दीर्घता >= decdata->विस्थापनम्);
  यदि(decdata->दीर्घता < decdata->विस्थापनम्)
    प्रत्यागम CURLE_FAILED_INIT;
  अन्यथा यदि(decdata->दीर्घता - decdata->विस्थापनम् < आकार ||
          decdata->दीर्घता < len) {
    /* increase internal decrypted data buffer */
    size_t दीर्घता = decdata->विस्थापनम् + आकार;
    अचिह्नित वर्ण *बफर;
    /* make sure that the requested amount of data fits */
    यदि(दीर्घता < len)
      दीर्घता = len;

    बफर = curlx_realloc(decdata->बफर, दीर्घता);
    यदि(!बफर) {
      failf(दत्त, "schannel: unable to re-allocate memory");
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
    decdata->बफर = बफर;
    decdata->दीर्घता = दीर्घता;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode schannel_connect_step2(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *दत्त)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना schannel_ssl_backend_दत्त *backend =
    (संरचना schannel_ssl_backend_दत्त *)connssl->backend;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  पूर्णाङ्क i;
  size_t nread = 0;
  Sec_बफर outbuf[3];
  Sec_बफर_Desc outbuf_desc;
  Sec_बफर inbuf[2];
  Sec_बफर_Desc inbuf_desc;
  SECURITY_स्थितिः sspi_स्थितिः = SEC_E_OK;
  CURLcode परिणाम;
  bool doread;
  ध्रुव वर्ण *pubkey_ptr;

  DEBUGASSERT(backend);

  doread = (connssl->io_need & CURL_SSL_IO_NEED_SEND) ? FALSE : TRUE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  DEBUGF(infof(दत्त, "schannel: SSL/TLS connection with %s port %d (step 2/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->पत्तनम्));

  यदि(!backend->cred || !backend->ctxt)
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;

  /* buffer to store previously received and decrypted data */
  यदि(!backend->decdata.बफर) {
    backend->decdata.विस्थापनम् = 0;
    backend->decdata.दीर्घता = CURL_SCHANNEL_बफर_INIT_आकार;
    backend->decdata.बफर = curlx_malloc(backend->decdata.दीर्घता);
    यदि(!backend->decdata.बफर) {
      failf(दत्त, "schannel: unable to allocate memory");
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
  }

  /* buffer to store previously received and encrypted data */
  यदि(!backend->encdata.बफर) {
    backend->encdata_is_incomplete = FALSE;
    backend->encdata.विस्थापनम् = 0;
    backend->encdata.दीर्घता = CURL_SCHANNEL_बफर_INIT_आकार;
    backend->encdata.बफर = curlx_malloc(backend->encdata.दीर्घता);
    यदि(!backend->encdata.बफर) {
      failf(दत्त, "schannel: unable to allocate memory");
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
  }

  परिणाम = ensure_encoding_आकार(दत्त, &backend->encdata, 0);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  प्रति(;;) {
    यदि(doread) {
      /* read encrypted handshake data from socket */
      परिणाम = Curl_conn_cf_recv(cf->अग्र, दत्त,
                                 (वर्ण *)(backend->encdata.बफर +
                                          backend->encdata.विस्थापनम्),
                                 backend->encdata.दीर्घता -
                                 backend->encdata.विस्थापनम्,
                                 &nread);
      यदि(परिणाम == CURLE_AGAIN) {
        यदि(!backend->encdata.विस्थापनम् || backend->encdata_is_incomplete) {
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          DEBUGF(infof(दत्त, "schannel: failed to receive handshake, "
                       "need more data"));
          प्रत्यागम CURLE_OK;
        }
        अन्यथा {
          DEBUGF(infof(दत्त, "schannel: no new handshake data received, "
                       "continuing to process existing handshake data"));
        }
      }
      अन्यथा यदि(परिणाम || (nread == 0)) {
        failf(दत्त, "schannel: failed to receive handshake, "
              "SSL/TLS connection failed");
        प्रत्यागम CURLE_SSL_CONNECT_ERROR;
      }
      अन्यथा {
        /* increase encrypted data buffer offset */
        backend->encdata.विस्थापनम् += nread;
        backend->encdata_is_incomplete = FALSE;
        SCH_DEV(infof(दत्त, "schannel: encrypted data got %zu", nread));
      }
    }

    SCH_DEV(infof(दत्त,
                  "schannel: encrypted data buffer: offset %zu length %zu",
                  backend->encdata.विस्थापनम्, backend->encdata.दीर्घता));

    /* setup input buffers */
    Init_Sec_बफर(&inbuf[0], SECBUFFER_TOKEN,
                  curlx_memdup0((ध्रुव वर्ण *)backend->encdata.बफर,
                                backend->encdata.विस्थापनम्),
                  curlx_uztoul(backend->encdata.विस्थापनम्));
    Init_Sec_बफर(&inbuf[1], SECBUFFER_रिक्तानि, रिक्त, 0);
    Init_Sec_बफर_Desc(&inbuf_desc, inbuf, 2);

    /* setup output buffers */
    Init_Sec_बफर(&outbuf[0], SECBUFFER_TOKEN, रिक्त, 0);
    Init_Sec_बफर(&outbuf[1], SECBUFFER_ALERT, रिक्त, 0);
    Init_Sec_बफर(&outbuf[2], SECBUFFER_रिक्तानि, रिक्त, 0);
    Init_Sec_बफर_Desc(&outbuf_desc, outbuf, 3);

    यदि(!inbuf[0].pv_बफर) {
      failf(दत्त, "schannel: unable to allocate memory");
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }

    /* The socket must be writable (or a poll error occurred) before we call
       InitializeSecurityContext to continue processing the received TLS
       records. This is because that function is not idempotent and we do not
       support partial save/resume sending replies of handshake tokens. */
    यदि(!SOCKET_WRITABLE(Curl_conn_cf_get_socket(cf, दत्त), 0)) {
      SCH_DEV(infof(दत्त, "schannel: handshake waiting for writable socket"));
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      curlx_free(inbuf[0].pv_बफर);
      प्रत्यागम CURLE_OK;
    }

    sspi_स्थितिः = Curl_pSecFn->InitializeSecurityContext(
      &backend->cred->cred_handle, &backend->ctxt->ctxt_handle,
      backend->cred->sni_hostname, backend->req_flags,
      0, 0, &inbuf_desc, 0, रिक्त,
      &outbuf_desc, &backend->ret_flags, रिक्त);

    /* free buffer for received handshake data */
    curlx_safefree(inbuf[0].pv_बफर);

    /* check if the handshake was incomplete */
    विकल्प(sspi_स्थितिः) {
    पक्ष SEC_E_INCOMPLETE_सन्देशः:
      backend->encdata_is_incomplete = TRUE;
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      DEBUGF(infof(दत्त,
                   "schannel: received incomplete message, need more data"));
      प्रत्यागम CURLE_OK;

    पक्ष SEC_I_CONTINUE_NEEDED:
    पक्ष SEC_E_OK:
      /* check if the handshake needs to be continued */
      परिणाम = CURLE_OK;
      प्रति(i = 0; i < 3; i++) {
        /* search for handshake tokens that need to be sent */
        यदि(outbuf[i].बफर_Type == SECBUFFER_TOKEN && outbuf[i].cb_बफर > 0) {
          size_t written = 0;
          DEBUGF(infof(दत्त, "schannel: sending next handshake data: "
                       "sending %lu bytes.", outbuf[i].cb_बफर));

          /* send handshake token to server */
          परिणाम = Curl_conn_cf_send(cf->अग्र, दत्त,
                                     (ध्रुव uint8_t *)outbuf[i].pv_बफर,
                                     outbuf[i].cb_बफर,
                                     FALSE, &written);
          यदि(परिणाम || (outbuf[i].cb_बफर != written)) {
            failf(दत्त, "schannel: failed to send next handshake data: "
                  "sent %zu of %lu bytes", written, outbuf[i].cb_बफर);
            परिणाम = CURLE_SSL_CONNECT_ERROR;
          }
        }
      }
      प्रति(i = 0; i < 3; i++) {
        /* free obsolete buffer */
        यदि(outbuf[i].pv_बफर)
          Curl_pSecFn->Free_Context_बफर(outbuf[i].pv_बफर);
      }
      यदि(परिणाम)
        प्रत्यागम परिणाम;
      भङ्ग;

    पक्ष SEC_I_INCOMPLETE_CREDENTIALS:
      यदि(!(backend->req_flags & ISC_REQ_USE_SUPPLIED_CREDS)) {
        /* If the server has requested a client certificate, attempt to
           continue the handshake without one. This allows connections to
           servers which request a client certificate but do not require
           it. */
        backend->req_flags |= ISC_REQ_USE_SUPPLIED_CREDS;
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        DEBUGF(infof(दत्त,
                     "schannel: a client certificate has been requested"));
        प्रत्यागम CURLE_OK;
      }
      FALLTHROUGH();

    स्वभाव:
      प्रत्यागम schannel_error(दत्त, sspi_स्थितिः);
    }

    /* check if there was additional remaining encrypted data */
    यदि(inbuf[1].बफर_Type == SECBUFFER_EXTRA && inbuf[1].cb_बफर > 0) {
      SCH_DEV(infof(दत्त, "schannel: encrypted data length: %lu",
                    inbuf[1].cb_बफर));
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
      यदि(backend->encdata.विस्थापनम् > inbuf[1].cb_बफर) {
        memmove(backend->encdata.बफर,
                (backend->encdata.बफर + backend->encdata.विस्थापनम्) -
                inbuf[1].cb_बफर, inbuf[1].cb_बफर);
        backend->encdata.विस्थापनम् = inbuf[1].cb_बफर;
        यदि(sspi_स्थितिः == SEC_I_CONTINUE_NEEDED) {
          doread = FALSE;
          अनुवर्त;
        }
      }
    }
    अन्यथा {
      backend->encdata.विस्थापनम् = 0;
    }
    भङ्ग;
  }

  /* check if the handshake needs to be continued */
  यदि(sspi_स्थितिः == SEC_I_CONTINUE_NEEDED) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    प्रत्यागम CURLE_OK;
  }

  /* check if the handshake is complete */
  यदि(sspi_स्थितिः == SEC_E_OK) {
    connssl->connecting_state = ssl_connect_3;
    DEBUGF(infof(दत्त, "schannel: SSL/TLS handshake complete"));
  }

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  pubkey_ptr = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(दत्त, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(दत्त, STRING_SSL_PINNEDPUBLICKEY);
#अन्यथा
  pubkey_ptr = CURL_EASY_STR(दत्त, STRING_SSL_PINNEDPUBLICKEY);
#यदि_अन्तः
  यदि(pubkey_ptr) {
    परिणाम = schannel_pkp_pin_peer_pubkey(cf, दत्त, pubkey_ptr);
    यदि(परिणाम) {
      failf(दत्त, "SSL: public key does not match pinned public key");
      प्रत्यागम परिणाम;
    }
  }

  यदि(conn_config->verifypeer && backend->use_manual_cred_validation) {
    /* Certificate verification also verifies the hostname if verifyhost */
    प्रत्यागम Curl_verify_certificate(cf, दत्त);
  }

  /* Verify the hostname manually when certificate verification is disabled,
     because in that case Schannel does not verify it. */
  यदि(!conn_config->verifypeer && conn_config->verifyhost)
    प्रत्यागम Curl_verify_host(cf, दत्त);

  प्रत्यागम CURLE_OK;
}

स्थिर bool valid_cert_encoding(ध्रुव CERT_CONTEXT *cert_context)
{
  प्रत्यागम cert_context &&
    ((cert_context->dwCertEncodingType & X509_ASN_ENCODING) != 0) &&
    cert_context->pbCertEncoded &&
    (cert_context->cbCertEncoded > 0);
}

प्रकारनिर्देश bool (*Read_crt_func)(ध्रुव CERT_CONTEXT *ccert_context,
                              bool reverse_क्रमः, शून्य *arg);

स्थिर शून्य traverse_cert_store(ध्रुव CERT_CONTEXT *context,
                                Read_crt_func func, शून्य *arg)
{
  ध्रुव CERT_CONTEXT *current_context = रिक्त;
  bool should_continue = TRUE;
  bool first = TRUE;
  bool reverse_क्रमः = FALSE;
  यावत्(should_continue &&
        (current_context = Cert_Enum_Certificates_अन्तर्_Store(
          context->hCertStore,
          current_context)) != रिक्त) {
    /* Windows 11 22H2 OS Build 22621.674 or higher enumerates certificates in
       leaf-to-root order while all previous versions of Windows enumerate
       certificates in root-to-leaf order. Determine the order of enumeration
       by comparing SECPKG_ATTR_REMOTE_CERT_CONTEXT's pbCertContext with the
       first certificate's pbCertContext. */
    यदि(first && context->pbCertEncoded != current_context->pbCertEncoded)
      reverse_क्रमः = TRUE;
    should_continue = func(current_context, reverse_क्रमः, arg);
    first = FALSE;
  }

  यदि(current_context)
    CertFreeCertificateContext(current_context);
}

स्थिर bool cert_गणक_callback(ध्रुव CERT_CONTEXT *ccert_context,
                                  bool reverse_क्रमः, शून्य *certs_गणनम्)
{
  (शून्य)reverse_क्रमः;
  यदि(valid_cert_encoding(ccert_context))
    (*(पूर्णाङ्क *)certs_गणनम्)++;
  यदि(*(पूर्णाङ्क *)certs_गणनम् > MAX_ALLOWED_CERT_AMOUNT)
    प्रत्यागम FALSE;
  प्रत्यागम TRUE;
}

संरचना Adder_तर्काः {
  संरचना Curl_easy *दत्त;
  CURLcode परिणाम;
  पूर्णाङ्क idx;
  पूर्णाङ्क certs_गणनम्;
};

स्थिर bool सङ्कलय_cert_गन्तव्यम्_certinfo(ध्रुव CERT_CONTEXT *ccert_context,
                                 bool reverse_क्रमः, शून्य *raw_arg)
{
  संरचना Adder_तर्काः *तर्काः = (संरचना Adder_तर्काः *)raw_arg;
  तर्काः->परिणाम = CURLE_OK;
  यदि(valid_cert_encoding(ccert_context)) {
    पूर्णाङ्क निवेशय_सूचकांक = reverse_क्रमः ? (तर्काः->certs_गणनम् - 1) - तर्काः->idx :
                       तर्काः->idx;
    तर्काः->परिणाम = Curl_extract_certinfo(तर्काः->दत्त, निवेशय_सूचकांक,
                                         ccert_context->pbCertEncoded,
                                         ccert_context->pbCertEncoded +
                                         ccert_context->cbCertEncoded);
    तर्काः->idx++;
  }
  प्रत्यागम तर्काः->परिणाम == CURLE_OK;
}

स्थिर शून्य schannel_session_free(शून्य *sessionid)
{
  /* this is expected to be called under sessionid lock */
  संरचना Curl_schannel_cred *cred = sessionid;

  यदि(cred) {
    cred->refcount--;
    यदि(cred->refcount == 0) {
      Curl_pSecFn->FreeCredentialsHandle(&cred->cred_handle);
      curlx_free(cred->sni_hostname);
      यदि(cred->client_cert_store) {
        CertCloseStore(cred->client_cert_store, 0);
        cred->client_cert_store = रिक्त;
      }
      curlx_safefree(cred);
    }
  }
}

स्थिर CURLcode schannel_connect_step3(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *दत्त)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना schannel_ssl_backend_दत्त *backend =
    (संरचना schannel_ssl_backend_दत्त *)connssl->backend;
  संरचना ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, दत्त);
  CURLcode परिणाम = CURLE_OK;
  SECURITY_स्थितिः sspi_स्थितिः = SEC_E_OK;
  CERT_CONTEXT *ccert_context = रिक्त;
  SecPkgContext_ApplicationProtocol alpn_परिणाम;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);
  DEBUGASSERT(backend);

  DEBUGF(infof(दत्त, "schannel: SSL/TLS connection with %s port %d (step 3/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->पत्तनम्));

  यदि(!backend->cred)
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;

  /* check if the required context attributes are met */
  यदि(backend->ret_flags != backend->req_flags) {
    यदि(!(backend->ret_flags & ISC_RET_SEQUENCE_DETECT))
      failf(दत्त, "schannel: failed to setup sequence detection");
    यदि(!(backend->ret_flags & ISC_RET_REPLAY_DETECT))
      failf(दत्त, "schannel: failed to setup replay detection");
    यदि(!(backend->ret_flags & ISC_RET_CONFIDENTIALITY))
      failf(दत्त, "schannel: failed to setup confidentiality");
    यदि(!(backend->ret_flags & ISC_RET_ALLOCATED_स्मृतिः))
      failf(दत्त, "schannel: failed to setup memory allocation");
    यदि(!(backend->ret_flags & ISC_RET_प्रवाहः))
      failf(दत्त, "schannel: failed to setup stream orientation");
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

  यदि(backend->use_alpn) {
    sspi_स्थितिः =
      Curl_pSecFn->Query_Context_गुणाः(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_APPLICATION_PROTOCOL,
                                          &alpn_परिणाम);

    यदि(sspi_स्थितिः != SEC_E_OK) {
      failf(दत्त, "schannel: failed to retrieve ALPN result");
      प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }

    यदि(alpn_परिणाम.Proto_Nego_स्थितिः ==
       Sec_Application_Protocol_Negotiation_स्थितिः_Success) {
      यदि(backend->recv_renegotiating &&
         connssl->negotiated.alpn &&
         वाक्ये_तुलय(connssl->negotiated.alpn,
                 (ध्रुव वर्ण *)alpn_परिणाम.ProtocolId,
                 alpn_परिणाम.Protocol_Id_आकार)) {
        /* Renegotiation selected a different protocol now, we cannot
         * deal with this */
        failf(दत्त, "schannel: server selected an ALPN protocol too late");
        प्रत्यागम CURLE_SSL_CONNECT_ERROR;
      }
      Curl_alpn_समुच्चयः_negotiated(cf, दत्त, connssl, alpn_परिणाम.ProtocolId,
                               alpn_परिणाम.Protocol_Id_आकार);
    }
    अन्यथा {
      यदि(!backend->recv_renegotiating)
        Curl_alpn_समुच्चयः_negotiated(cf, दत्त, connssl, रिक्त, 0);
    }
  }

  /* save the current session data for possible reuse */
  यदि(Curl_ssl_scache_use(cf, दत्त)) {
    Curl_ssl_scache_तालकम्(दत्त);
    /* Up ref count since call takes ownership */
    backend->cred->refcount++;
    परिणाम = Curl_ssl_scache_सङ्कलय_obj(cf, दत्त, connssl->peer.scache_कुञ्चिका,
                                     backend->cred, schannel_session_free);
    Curl_ssl_scache_unlock(दत्त);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  यदि(ssl_config->certinfo) {
    पूर्णाङ्क certs_गणनम् = 0;
    sspi_स्थितिः =
      Curl_pSecFn->Query_Context_गुणाः(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                          &ccert_context);

    यदि((sspi_स्थितिः != SEC_E_OK) || !ccert_context) {
      failf(दत्त, "schannel: failed to retrieve remote cert context");
      प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;
    }

    traverse_cert_store(ccert_context, cert_गणक_callback, &certs_गणनम्);
    यदि(certs_गणनम् > MAX_ALLOWED_CERT_AMOUNT) {
      failf(दत्त, "%d certificates is more than allowed (%d)",
            certs_गणनम्, MAX_ALLOWED_CERT_AMOUNT);
      CertFreeCertificateContext(ccert_context);
      प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }

    परिणाम = Curl_ssl_init_certinfo(दत्त, certs_गणनम्);
    यदि(!परिणाम) {
      संरचना Adder_तर्काः तर्काः;
      तर्काः.दत्त = दत्त;
      तर्काः.idx = 0;
      तर्काः.certs_गणनम् = certs_गणनम्;
      तर्काः.परिणाम = CURLE_OK;
      traverse_cert_store(ccert_context, सङ्कलय_cert_गन्तव्यम्_certinfo, &तर्काः);
      परिणाम = तर्काः.परिणाम;
    }
    CertFreeCertificateContext(ccert_context);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  connssl->connecting_state = ssl_connect_समाप्तम्;

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode schannel_connect(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 bool *समाप्तम्)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  CURLcode परिणाम;

  /* check if the connection has already been established */
  यदि(ssl_connection_complete == connssl->state) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  *समाप्तम् = FALSE;

  यदि(connssl->connecting_state == ssl_connect_1) {
    परिणाम = schannel_connect_step1(cf, दत्त);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  यदि(connssl->connecting_state == ssl_connect_2) {
    परिणाम = schannel_connect_step2(cf, दत्त);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  यदि(connssl->connecting_state == ssl_connect_3) {
    परिणाम = schannel_connect_step3(cf, दत्त);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  यदि(connssl->connecting_state == ssl_connect_समाप्तम्) {
    संरचना schannel_ssl_backend_दत्त *backend =
      (संरचना schannel_ssl_backend_दत्त *)connssl->backend;
    DEBUGASSERT(backend);

    यदि(Curl_pSecFn->Query_Context_गुणाः(
         &backend->ctxt->ctxt_handle,
         SECPKG_ATTR_प्रवाहः_SIZES,
         &backend->प्रवाहः_sizes)) {
      failf(दत्त, "schannel: failed getting stream sizes");
      प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }

    connssl->state = ssl_connection_complete;

    /* When SSPI is used in combination with Schannel
     * we need the Schannel context to create the Schannel
     * binding to pass the IIS extended protection checks.
     * Available on Windows 7 or later.
     */
    cf->conn->sslContext = &backend->ctxt->ctxt_handle;

    *समाप्तम् = TRUE;
  }

  प्रत्यागम CURLE_OK;
}

गणना schannel_renegotiate_caller_t {
  SCH_RENEG_CALLER_IS_RECV,
  SCH_RENEG_CALLER_IS_SEND
};

/* The maximum time we allow for Schannel renegotiation which may in some
   rare cases block either due to libcurl (waiting on the socket) or Windows
   (waiting on an interactive security prompt). Note Schannel "renegotiation"
   is not necessarily literal TLS renegotiation, but means DecryptMessage
   returned SEC_I_RENEGOTIATE which means at least the security context needs
   to be re-established. */
#परिभाषय MAX_RENEG_खण्डम्_TIME (60 * 1000) /* 60 seconds in milliseconds */

/* This function renegotiates the connection due to a server request received
   by schannel_recv. This function returns CURLE_AGAIN if the renegotiation is
   incomplete. In that case, we remain in the renegotiation (connecting) stage
   and future calls to schannel_recv and schannel_send must call this function
   first to complete the renegotiation. */
स्थिर CURLcode schannel_recv_renegotiate(
  संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
  गणना schannel_renegotiate_caller_t caller)
{
  CURLcode परिणाम;
  curl_socket_t sockfd;
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना schannel_ssl_backend_दत्त *backend =
    (संरचना schannel_ssl_backend_दत्त *)connssl->backend;
  संरचना schannel_renegotiate_state *rs = &backend->renegotiate_state;

  यदि(!backend || !backend->recv_renegotiating) {
    failf(दत्त, "schannel: unexpected call to schannel_recv_renegotiate");
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }
  DEBUGASSERT(caller <= SCH_RENEG_CALLER_IS_SEND);
  यदि(caller == SCH_RENEG_CALLER_IS_RECV)
    SCH_DEV(infof(दत्त, "schannel: renegotiation caller is schannel_recv"));
  अन्यथा
    SCH_DEV(infof(दत्त, "schannel: renegotiation caller is schannel_send"));

  sockfd = Curl_conn_cf_get_socket(cf, दत्त);

  यदि(sockfd == CURL_SOCKET_BAD) {
    failf(दत्त, "schannel: renegotiation missing socket");
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

  यदि(!rs->started) { /* new renegotiation */
    infof(दत्त, "schannel: renegotiating SSL/TLS connection");
    DEBUGASSERT(connssl->state == ssl_connection_complete);
    DEBUGASSERT(connssl->connecting_state == ssl_connect_समाप्तम्);
    connssl->state = ssl_connection_negotiating;
    connssl->connecting_state = ssl_connect_2;
    स्मृतिं_पूरय(rs, 0, परिमाण(*rs));
    rs->io_need = CURL_SSL_IO_NEED_SEND;
    rs->start_time = *Curl_pgrs_now(दत्त);
    rs->started = TRUE;
  }

  प्रति(;;) {
    bool खण्डम्_read, खण्डम्_write, blocking, समाप्तम्;
    curl_socket_t readfd, writefd;
    timediff_t elapsed;

    elapsed = curlx_ptimediff_ms(Curl_pgrs_now(दत्त), &rs->start_time);
    यदि(elapsed >= MAX_RENEG_खण्डम्_TIME) {
      failf(दत्त, "schannel: renegotiation timeout");
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      भङ्ग;
    }

    /* the current io_need state may have been overwritten since the last time
       this function was called. restore the io_need state needed to continue
       the renegotiation. */

    connssl->io_need = rs->io_need;

    परिणाम = schannel_connect(cf, दत्त, &समाप्तम्);

    rs->io_need = connssl->io_need;

    यदि(!परिणाम && !समाप्तम्)
      परिणाम = CURLE_AGAIN;

    यदि(परिणाम != CURLE_AGAIN)
      भङ्ग;

    readfd = (rs->io_need & CURL_SSL_IO_NEED_RECV) ? sockfd : CURL_SOCKET_BAD;
    writefd = (rs->io_need & CURL_SSL_IO_NEED_SEND) ? sockfd : CURL_SOCKET_BAD;

    यदि(readfd == CURL_SOCKET_BAD && writefd == CURL_SOCKET_BAD)
      अनुवर्त;

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

    खण्डम्_read = (caller == SCH_RENEG_CALLER_IS_SEND) ? TRUE : FALSE;
    खण्डम्_write = (caller == SCH_RENEG_CALLER_IS_RECV) ? TRUE : FALSE;

    blocking = (खण्डम्_read && (readfd != CURL_SOCKET_BAD)) ||
               (खण्डम्_write && (writefd != CURL_SOCKET_BAD));

    SCH_DEV_SHOWBOOL(खण्डम्_read);
    SCH_DEV_SHOWBOOL(खण्डम्_write);
    SCH_DEV_SHOWBOOL(blocking);

    प्रति(;;) {
      पूर्णाङ्क what;
      timediff_t timeout_ms, remaining;

      परिणाम = Curl_pgrsUpdate(दत्त);
      यदि(परिणाम)
        भङ्ग;

      elapsed = curlx_ptimediff_ms(Curl_pgrs_now(दत्त), &rs->start_time);
      यदि(elapsed >= MAX_RENEG_खण्डम्_TIME) {
        failf(दत्त, "schannel: renegotiation timeout");
        परिणाम = CURLE_SSL_CONNECT_ERROR;
        भङ्ग;
      }
      remaining = MAX_RENEG_खण्डम्_TIME - elapsed;

      यदि(blocking) {
        timeout_ms = Curl_timeleft_ms(दत्त);

        यदि(timeout_ms < 0) {
          परिणाम = CURLE_OPERATION_TIMEDOUT;
          भङ्ग;
        }

        /* the blocking is in intervals so that the progress function can be
           called every second */
        यदि(!timeout_ms || timeout_ms > 1000)
          timeout_ms = 1000;

        यदि(timeout_ms > remaining)
          timeout_ms = remaining;
      }
      अन्यथा
        timeout_ms = 0;

      SCH_DEV(infof(दत्त, "schannel: renegotiation wait until socket is"
                    "%s%s for up to %" FMT_TIMEDIFF_T " ms",
                    ((readfd != CURL_SOCKET_BAD) ? " readable" : ""),
                    ((writefd != CURL_SOCKET_BAD) ? " writable" : ""),
                    timeout_ms));

      what = Curl_socket_check(readfd, CURL_SOCKET_BAD, writefd, timeout_ms);

      यदि(what > 0 && (what & (CURL_CSELECT_अन्तर् | CURL_CSELECT_बहिर्गमनम्))) {
        SCH_DEV(infof(दत्त, "schannel: renegotiation socket %s%s",
                      ((what & CURL_CSELECT_अन्तर्) ? "CURL_CSELECT_IN " : ""),
                      ((what & CURL_CSELECT_बहिर्गमनम्) ? "CURL_CSELECT_OUT " : "")));
        परिणाम = CURLE_AGAIN;
        भङ्ग;
      }
      अन्यथा यदि(!what) {
        SCH_DEV(infof(दत्त, "schannel: renegotiation socket timeout"));
        यदि(blocking)
          अनुवर्त;
        अन्यथा
          प्रत्यागम CURLE_AGAIN;
      }

      failf(दत्त, "schannel: socket error during renegotiation");
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      भङ्ग;
    }
    यदि(परिणाम != CURLE_AGAIN)
      भङ्ग;
  }

  DEBUGASSERT(परिणाम != CURLE_AGAIN);

  rs->started = FALSE;
  backend->recv_renegotiating = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  यदि(परिणाम)
    failf(दत्त, "schannel: renegotiation failed");
  अन्यथा
    infof(दत्त, "schannel: SSL/TLS connection renegotiated");

  प्रत्यागम परिणाम;
}

स्थिर CURLcode schannel_send(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                              ध्रुव शून्य *buf, size_t len, size_t *pnwritten)
{
  size_t दत्त_len = 0;
  अचिह्नित वर्ण *ptr = रिक्त;
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  Sec_बफर outbuf[4];
  Sec_बफर_Desc outbuf_desc;
  SECURITY_स्थितिः sspi_स्थितिः = SEC_E_OK;
  CURLcode परिणाम = CURLE_OK;
  संरचना schannel_ssl_backend_दत्त *backend =
    (संरचना schannel_ssl_backend_दत्त *)connssl->backend;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  यदि(backend->recv_renegotiating) {
    परिणाम = schannel_recv_renegotiate(cf, दत्त, SCH_RENEG_CALLER_IS_SEND);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  /* check if the buffer is longer than the maximum message length */
  यदि(len > backend->प्रवाहः_sizes.cb_अधिकतमम्_सन्देशः) {
    len = backend->प्रवाहः_sizes.cb_अधिकतमम्_सन्देशः;
  }

  /* calculate the complete message length and prepare the send buffer */
  दत्त_len = backend->प्रवाहः_sizes.cb_शीर्षकम् + len +
    backend->प्रवाहः_sizes.cbTrailer;
  यदि(दत्त_len > backend->send_बफर_len) {
    ptr = curlx_realloc(backend->send_बफर, दत्त_len);
    यदि(!ptr)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    backend->send_बफर = ptr;
    backend->send_बफर_len = दत्त_len;
  }
  अन्यथा
    ptr = backend->send_बफर;

  /* setup output buffers (header, data, trailer, empty) */
  Init_Sec_बफर(&outbuf[0], SECBUFFER_प्रवाहः_शीर्षकम्,
                ptr, backend->प्रवाहः_sizes.cb_शीर्षकम्);
  Init_Sec_बफर(&outbuf[1], SECBUFFER_दत्त,
                ptr + backend->प्रवाहः_sizes.cb_शीर्षकम्, curlx_uztoul(len));
  Init_Sec_बफर(&outbuf[2], SECBUFFER_प्रवाहः_TRAILER,
                ptr + backend->प्रवाहः_sizes.cb_शीर्षकम् + len,
                backend->प्रवाहः_sizes.cbTrailer);
  Init_Sec_बफर(&outbuf[3], SECBUFFER_रिक्तानि, रिक्त, 0);
  Init_Sec_बफर_Desc(&outbuf_desc, outbuf, 4);

  /* copy data into output buffer */
  स्मृतिं_प्रतिलिख(outbuf[1].pv_बफर, buf, len);

  /* https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-encryptmessage */
  sspi_स्थितिः = Curl_pSecFn->Encrypt_सन्देशः(&backend->ctxt->ctxt_handle, 0,
                                            &outbuf_desc, 0);

  /* check if the message was encrypted */
  यदि(sspi_स्थितिः == SEC_E_OK) {

    /* send the encrypted message including header, data and trailer */
    len = outbuf[0].cb_बफर + outbuf[1].cb_बफर + outbuf[2].cb_बफर;

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
    यावत्(len > *pnwritten) {
      size_t this_write = 0;
      पूर्णाङ्क what;
      timediff_t timeout_ms = Curl_timeleft_ms(दत्त);
      यदि(timeout_ms < 0) {
        /* we already got the timeout */
        failf(दत्त, "schannel: timed out sending data (bytes sent: %zu)",
              *pnwritten);
        परिणाम = CURLE_OPERATION_TIMEDOUT;
        भङ्ग;
      }
      अन्यथा यदि(!timeout_ms)
        timeout_ms = TIMEDIFF_T_MAX;
      what = SOCKET_WRITABLE(Curl_conn_cf_get_socket(cf, दत्त), timeout_ms);
      यदि(what < 0) {
        /* fatal error */
        failf(दत्त, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        परिणाम = CURLE_SEND_ERROR;
        भङ्ग;
      }
      अन्यथा यदि(what == 0) {
        failf(दत्त, "schannel: timed out sending data (bytes sent: %zu)",
              *pnwritten);
        परिणाम = CURLE_OPERATION_TIMEDOUT;
        भङ्ग;
      }
      /* socket is writable */

       परिणाम = Curl_conn_cf_send(cf->अग्र, दत्त,
                                  (ध्रुव uint8_t *)ptr + *pnwritten,
                                  len - *pnwritten,
                                  FALSE, &this_write);
      यदि(परिणाम == CURLE_AGAIN)
        अनुवर्त;
      अन्यथा यदि(परिणाम) {
        भङ्ग;
      }

      *pnwritten += this_write;
    }
  }
  अन्यथा यदि(sspi_स्थितिः == SEC_E_INSUFFICIENT_स्मृतिः) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
  अन्यथा {
    परिणाम = CURLE_SEND_ERROR;
  }

  यदि(len == *pnwritten)
    /* Encrypted message including header, data and trailer entirely sent.
       The return value is the number of unencrypted bytes that were sent. */
    *pnwritten = outbuf[1].cb_बफर;

  प्रत्यागम परिणाम;
}

स्थिर CURLcode schannel_recv(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                              वर्ण *buf, size_t len, size_t *pnread)
{
  size_t आकार = 0;
  size_t nread = 0;
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  Sec_बफर inbuf[4];
  Sec_बफर_Desc inbuf_desc;
  SECURITY_स्थितिः sspi_स्थितिः = SEC_E_OK;
  संरचना schannel_ssl_backend_दत्त *backend =
    (संरचना schannel_ssl_backend_दत्त *)connssl->backend;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(backend);
  *pnread = 0;

  यदि(backend->recv_renegotiating) {
    परिणाम = schannel_recv_renegotiate(cf, दत्त, SCH_RENEG_CALLER_IS_RECV);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
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

  SCH_DEV(infof(दत्त, "schannel: client wants to read %zu bytes", len));

  यदि(len && len <= backend->decdata.विस्थापनम्) {
    SCH_DEV(infof(दत्त,
                  "schannel: enough decrypted data is already available"));
    गच्छ cleanup;
  }
  अन्यथा यदि(backend->recv_unrecoverable_err) {
    परिणाम = backend->recv_unrecoverable_err;
    infof(दत्त, "schannel: an unrecoverable error occurred in a prior call");
    गच्छ cleanup;
  }
  अन्यथा यदि(backend->recv_sspi_close_notify) {
    /* once a server has indicated shutdown there is no more encrypted data */
    infof(दत्त, "schannel: server indicated shutdown in a prior call");
    गच्छ cleanup;
  }
  /* it is debatable what to return when !len. Regardless we cannot return
     immediately because there may be data to decrypt (in the case we want to
     decrypt all encrypted cached data) so handle !len later in cleanup. */
  अन्यथा यदि(len && !backend->recv_connection_closed) {
    /* the encrypted buffer must be large enough to hold all the bytes
       requested and some TLS record overhead. 'len' is a buffer size, so this
       integer math cannot overflow. */
    ध्रुव size_t min_encdata_दीर्घता = len + CURL_SCHANNEL_बफर_FREE_आकार;

    /* make sure encrypt buffer fits the requested amount of data */
    परिणाम = ensure_encoding_आकार(दत्त, &backend->encdata, min_encdata_दीर्घता);
    यदि(परिणाम)
      गच्छ cleanup;

    SCH_DEV(infof(दत्त,
                  "schannel: encrypted data buffer: offset %zu length %zu",
                  backend->encdata.विस्थापनम्, backend->encdata.दीर्घता));

    /* read encrypted data from socket */
    परिणाम = Curl_conn_cf_recv(cf->अग्र, दत्त,
                               (वर्ण *)(backend->encdata.बफर +
                                        backend->encdata.विस्थापनम्),
                               backend->encdata.दीर्घता -
                               backend->encdata.विस्थापनम्,
                               &nread);
    यदि(परिणाम) {
      यदि(परिणाम == CURLE_AGAIN)
        SCH_DEV(infof(दत्त, "schannel: recv returned CURLE_AGAIN"));
      अन्यथा {
        infof(दत्त, "schannel: recv returned error %d", (पूर्णाङ्क)परिणाम);
        backend->recv_unrecoverable_err = परिणाम;
      }
    }
    अन्यथा यदि(nread == 0) {
      backend->recv_connection_closed = TRUE;
      DEBUGF(infof(दत्त, "schannel: server closed the connection"));
    }
    अन्यथा {
      backend->encdata.विस्थापनम् += nread;
      backend->encdata_is_incomplete = FALSE;
      SCH_DEV(infof(दत्त, "schannel: encrypted data got %zu", nread));
    }
  }

  SCH_DEV(infof(दत्त, "schannel: encrypted data buffer: offset %zu length %zu",
                backend->encdata.विस्थापनम्, backend->encdata.दीर्घता));

  /* decrypt loop */
  यावत्(backend->encdata.विस्थापनम् > 0 && sspi_स्थितिः == SEC_E_OK &&
        (!len || backend->decdata.विस्थापनम् < len ||
         backend->recv_connection_closed)) {
    /* prepare data buffer for DecryptMessage call */
    Init_Sec_बफर(&inbuf[0], SECBUFFER_दत्त, backend->encdata.बफर,
                  curlx_uztoul(backend->encdata.विस्थापनम्));

    /* we need 3 more empty input buffers for possible output */
    Init_Sec_बफर(&inbuf[1], SECBUFFER_रिक्तानि, रिक्त, 0);
    Init_Sec_बफर(&inbuf[2], SECBUFFER_रिक्तानि, रिक्त, 0);
    Init_Sec_बफर(&inbuf[3], SECBUFFER_रिक्तानि, रिक्त, 0);
    Init_Sec_बफर_Desc(&inbuf_desc, inbuf, 4);

    /* https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-decryptmessage
     */
    sspi_स्थितिः = Curl_pSecFn->Decrypt_सन्देशः(&backend->ctxt->ctxt_handle,
                                              &inbuf_desc, 0, रिक्त);

    /* check if everything went fine (server may want to renegotiate
       or shutdown the connection context) */
    यदि(sspi_स्थितिः == SEC_E_OK || sspi_स्थितिः == SEC_I_RENEGOTIATE ||
       sspi_स्थितिः == SEC_I_CONTEXT_EXPIRED) {
      /* check for successfully decrypted data, even before actual
         renegotiation or shutdown of the connection context */
      यदि(inbuf[1].बफर_Type == SECBUFFER_दत्त) {
        SCH_DEV(infof(दत्त, "schannel: decrypted data length: %lu",
                      inbuf[1].cb_बफर));

        /* ensure the decode buffer fits the received amount of data */
        परिणाम = ensure_decoding_आकार(दत्त, &backend->decdata,
                                      inbuf[1].cb_बफर, len);
        यदि(परिणाम)
          गच्छ cleanup;

        /* copy decrypted data to internal buffer */
        आकार = inbuf[1].cb_बफर;
        यदि(आकार) {
          स्मृतिं_प्रतिलिख(backend->decdata.बफर + backend->decdata.विस्थापनम्,
                 inbuf[1].pv_बफर, आकार);
          backend->decdata.विस्थापनम् += आकार;
        }

        SCH_DEV(infof(दत्त, "schannel: decrypted data added: %zu", आकार));
        SCH_DEV(infof(दत्त,
                      "schannel: decrypted cached: offset %zu length %zu",
                      backend->decdata.विस्थापनम्, backend->decdata.दीर्घता));
      }

      /* check for remaining encrypted data */
      यदि(inbuf[3].बफर_Type == SECBUFFER_EXTRA && inbuf[3].cb_बफर > 0) {
        SCH_DEV(infof(दत्त, "schannel: encrypted data length: %lu",
                      inbuf[3].cb_बफर));

        /* check if the remaining data is less than the total amount
         * and therefore begins after the already processed data
         */
        यदि(backend->encdata.विस्थापनम् > inbuf[3].cb_बफर) {
          /* move remaining encrypted data forward to the beginning of
बफर*/
          memmove(backend->encdata.बफर,
                  (backend->encdata.बफर + backend->encdata.विस्थापनम्) -
                  inbuf[3].cb_बफर, inbuf[3].cb_बफर);
          backend->encdata.विस्थापनम् = inbuf[3].cb_बफर;
        }

        SCH_DEV(infof(दत्त,
                      "schannel: encrypted cached: offset %zu length %zu",
                      backend->encdata.विस्थापनम्, backend->encdata.दीर्घता));
      }
      अन्यथा {
        /* reset encrypted buffer offset, because there is no data remaining */
        backend->encdata.विस्थापनम् = 0;
      }

      /* check if server wants to renegotiate the connection context */
      यदि(sspi_स्थितिः == SEC_I_RENEGOTIATE) {
        infof(दत्त, "schannel: remote party requests renegotiation");
        यदि(परिणाम && परिणाम != CURLE_AGAIN) {
          infof(दत्त, "schannel: cannot renegotiate, an error is pending");
          गच्छ cleanup;
        }

        backend->recv_renegotiating = TRUE;
        परिणाम = schannel_recv_renegotiate(cf, दत्त, SCH_RENEG_CALLER_IS_RECV);
        यदि(परिणाम)
          गच्छ cleanup;

        /* now retry receiving data */
        sspi_स्थितिः = SEC_E_OK;
        अनुवर्त;
      }
      /* check if the server closed the connection */
      अन्यथा यदि(sspi_स्थितिः == SEC_I_CONTEXT_EXPIRED) {
        /* In Windows 2000 SEC_I_CONTEXT_EXPIRED (close_notify) is not
           returned so we have to work around that in cleanup. */
        backend->recv_sspi_close_notify = TRUE;
        यदि(!backend->recv_connection_closed)
          backend->recv_connection_closed = TRUE;
        /* We received the close notify fine, any error we got
         * from the lower filters afterwards (e.g. the socket), is not
         * an error on the TLS data stream. That one ended here. */
        यदि(परिणाम == CURLE_RECV_ERROR)
          परिणाम = CURLE_OK;
        infof(दत्त,
              "schannel: server close notification received (close_notify)");
        गच्छ cleanup;
      }
    }
    अन्यथा यदि(sspi_स्थितिः == SEC_E_INCOMPLETE_सन्देशः) {
      backend->encdata_is_incomplete = TRUE;
      यदि(!परिणाम)
        परिणाम = CURLE_AGAIN;
      SCH_DEV(infof(दत्त, "schannel: failed to decrypt data, need more data"));
      गच्छ cleanup;
    }
    अन्यथा {
      वर्ण बफर[STRERROR_LEN];
      failf(दत्त, "schannel: failed to read data from server: %s",
            Curl_sspi_strerror(sspi_स्थितिः, बफर, परिमाण(बफर)));
      परिणाम = CURLE_RECV_ERROR;
      गच्छ cleanup;
    }
  }

  SCH_DEV(infof(दत्त, "schannel: encrypted data buffer: offset %zu length %zu",
                backend->encdata.विस्थापनम्, backend->encdata.दीर्घता));

  SCH_DEV(infof(दत्त, "schannel: decrypted data buffer: offset %zu length %zu",
                backend->decdata.विस्थापनम्, backend->decdata.दीर्घता));

cleanup:
  /* Warning- there is no guarantee the encdata state is valid at this point */
  SCH_DEV(infof(दत्त, "schannel: schannel_recv cleanup"));

  /* Error if the connection has closed without a close_notify.

     The behavior here is a matter of debate. We do not want to be vulnerable
     to a truncation attack however there is some browser precedent for
     ignoring the close_notify for compatibility reasons. */
  यदि(len && !backend->decdata.विस्थापनम् && backend->recv_connection_closed &&
     !backend->recv_sspi_close_notify) {
    परिणाम = CURLE_RECV_ERROR;
    failf(दत्त, "schannel: server closed abruptly (missing close_notify)");
  }

  /* Any error other than CURLE_AGAIN is an unrecoverable error. */
  यदि(परिणाम && परिणाम != CURLE_AGAIN)
    backend->recv_unrecoverable_err = परिणाम;

  आकार = len < backend->decdata.विस्थापनम् ? len : backend->decdata.विस्थापनम्;
  यदि(आकार) {
    स्मृतिं_प्रतिलिख(buf, backend->decdata.बफर, आकार);
    memmove(backend->decdata.बफर, backend->decdata.बफर + आकार,
            backend->decdata.विस्थापनम् - आकार);
    backend->decdata.विस्थापनम् -= आकार;
    SCH_DEV(infof(दत्त, "schannel: decrypted data returned %zu", आकार));
    SCH_DEV(infof(दत्त,
                  "schannel: decrypted data buffer: offset %zu length %zu",
                  backend->decdata.विस्थापनम्, backend->decdata.दीर्घता));
    *pnread = आकार;
    प्रत्यागम CURLE_OK;
  }

  यदि(!परिणाम && !backend->recv_connection_closed)
    परिणाम = CURLE_AGAIN;

  /* it is debatable what to return when !len. We could return whatever error
     we got from decryption but instead we override here so the return is
     consistent. */
  यदि(!len)
    प्रत्यागम CURLE_OK;

  प्रत्यागम परिणाम;
}

स्थिर bool schannel_दत्त_pending(संरचना Curl_cfilter *cf,
                                  ध्रुव संरचना Curl_easy *दत्त)
{
  ध्रुव संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना schannel_ssl_backend_दत्त *backend =
    (संरचना schannel_ssl_backend_दत्त *)connssl->backend;

  (शून्य)दत्त;
  DEBUGASSERT(backend);

  यदि(backend->ctxt) /* SSL/TLS is in use */
    प्रत्यागम backend->decdata.विस्थापनम् > 0 ||
           (backend->encdata.विस्थापनम् > 0 && !backend->encdata_is_incomplete) ||
           backend->recv_connection_closed ||
           backend->recv_sspi_close_notify ||
           backend->recv_unrecoverable_err;
  अन्यथा
    प्रत्यागम FALSE;
}

/* shut down the SSL connection and clean up related memory.
   this function can be called multiple times on the same connection including
   if the SSL connection failed (eg connection made but failed handshake). */
स्थिर CURLcode schannel_shutdown(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त,
                                  bool send_shutdown, bool *समाप्तम्)
{
  /* See https://learn.microsoft.com/windows/win32/secauthn/shutting-down-an-schannel-connection
   * Shutting Down an Schannel Connection
   */
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना schannel_ssl_backend_दत्त *backend =
    (संरचना schannel_ssl_backend_दत्त *)connssl->backend;
  CURLcode परिणाम = CURLE_OK;

  यदि(cf->shutdown) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  DEBUGASSERT(दत्त);
  DEBUGASSERT(backend);

  /* Not supported in schannel */
  (शून्य)send_shutdown;

  *समाप्तम् = FALSE;
  यदि(backend->ctxt) {
    infof(दत्त, "schannel: shutting down SSL/TLS connection with %s port %d",
          connssl->peer.origin->hostname, connssl->peer.origin->पत्तनम्);
  }

  यदि(!backend->ctxt || cf->shutdown) {
    *समाप्तम् = TRUE;
    गच्छ बहिर्गमनम्;
  }

  यदि(backend->cred && backend->ctxt && !backend->प्रेषितम्_shutdown &&
     !backend->shutdown_generated) {
    Sec_बफर_Desc BuffDesc;
    Sec_बफर बफर_2;
    SECURITY_स्थितिः sspi_स्थितिः;
    Sec_बफर outbuf;
    Sec_बफर_Desc outbuf_desc;
    DWORD dwshut = SCHANNEL_SHUTDOWN;

    Init_Sec_बफर(&बफर_2, SECBUFFER_TOKEN, &dwshut, परिमाण(dwshut));
    Init_Sec_बफर_Desc(&BuffDesc, &बफर_2, 1);

    sspi_स्थितिः = Curl_pSecFn->ApplyControlToken(&backend->ctxt->ctxt_handle,
                                                 &BuffDesc);

    यदि(sspi_स्थितिः != SEC_E_OK) {
      वर्ण बफर[STRERROR_LEN];
      failf(दत्त, "schannel: ApplyControlToken failure: %s",
            Curl_sspi_strerror(sspi_स्थितिः, बफर, परिमाण(बफर)));
      परिणाम = CURLE_SEND_ERROR;
      गच्छ बहिर्गमनम्;
    }

    /* setup output buffer */
    Init_Sec_बफर(&outbuf, SECBUFFER_रिक्तानि, रिक्त, 0);
    Init_Sec_बफर_Desc(&outbuf_desc, &outbuf, 1);

    sspi_स्थितिः = Curl_pSecFn->InitializeSecurityContext(
      &backend->cred->cred_handle,
      &backend->ctxt->ctxt_handle,
      backend->cred->sni_hostname,
      backend->req_flags,
      0,
      0,
      रिक्त,
      0,
      &backend->ctxt->ctxt_handle,
      &outbuf_desc,
      &backend->ret_flags, रिक्त);

    यदि((sspi_स्थितिः == SEC_E_OK) || (sspi_स्थितिः == SEC_I_CONTEXT_EXPIRED)) {
      backend->shutdown_token = outbuf;
      backend->shutdown_generated = TRUE;
    }
    अन्यथा {
      वर्ण बफर[STRERROR_LEN];
      यदि(outbuf.pv_बफर)
        Curl_pSecFn->Free_Context_बफर(outbuf.pv_बफर);
      failf(दत्त, "schannel: generating close msg failed: %s",
            Curl_sspi_strerror(sspi_स्थितिः, बफर, परिमाण(बफर)));
      परिणाम = CURLE_SEND_ERROR;
      गच्छ बहिर्गमनम्;
    }
  }

  यदि(backend->shutdown_generated && !backend->प्रेषितम्_shutdown) {
    size_t written = 0;
    size_t remaining;

    DEBUGASSERT(backend->shutdown_विस्थापनम् <= backend->shutdown_token.cb_बफर);
    remaining = backend->shutdown_token.cb_बफर - backend->shutdown_विस्थापनम्;

    यदि(remaining) {
      परिणाम = Curl_conn_cf_send(cf->अग्र, दत्त,
                                 ((uint8_t *)backend->shutdown_token.pv_बफर +
                                  backend->shutdown_विस्थापनम्),
                                 remaining, FALSE, &written);
      यदि(!परिणाम)
        backend->shutdown_विस्थापनम् += written;
      यदि(परिणाम == CURLE_AGAIN || (!परिणाम && written < remaining)) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        परिणाम = CURLE_OK;
        गच्छ बहिर्गमनम्;
      }
    }
    अन्यथा /* no shutdown message to send? */
      परिणाम = CURLE_OK;
    यदि(परिणाम) {
      यदि(backend->recv_connection_closed) {
        /* The server already closed, so sending close_notify is optional. */
        *समाप्तम् = TRUE;
        परिणाम = CURLE_OK;
      }
      अन्यथा {
        failf(दत्त, "schannel: error sending close msg: %d", (पूर्णाङ्क)परिणाम);
        परिणाम = CURLE_SEND_ERROR;
      }
      गच्छ बहिर्गमनम्;
    }
    यदि(backend->shutdown_token.pv_बफर)
      Curl_pSecFn->Free_Context_बफर(backend->shutdown_token.pv_बफर);
    स्मृतिं_पूरय(&backend->shutdown_token, 0, परिमाण(backend->shutdown_token));
    backend->shutdown_विस्थापनम् = 0;
    backend->प्रेषितम्_shutdown = TRUE;
    *समाप्तम् = TRUE;
  }

  /* If the connection seems open and we have not seen the close notify
   * from the server yet, try to receive it. */
  यदि(backend->cred && backend->ctxt &&
     !backend->recv_sspi_close_notify && !backend->recv_connection_closed) {
    वर्ण बफर[1024];
    size_t nread;

    परिणाम = schannel_recv(cf, दत्त, बफर, परिमाण(बफर), &nread);
    यदि(परिणाम == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
    }
    अन्यथा यदि(परिणाम) {
      CURL_TRC_CF(दत्त, cf, "SSL shutdown, error %d", (पूर्णाङ्क)परिणाम);
      परिणाम = CURLE_RECV_ERROR;
    }
    अन्यथा यदि(nread == 0) {
      /* We got the close notify alert and are done. */
      backend->recv_connection_closed = TRUE;
      *समाप्तम् = TRUE;
    }
    अन्यथा {
      /* still data coming in? */
    }
  }

बहिर्गमनम्:
  cf->shutdown = (परिणाम || *समाप्तम्);
  प्रत्यागम परिणाम;
}

स्थिर शून्य schannel_close(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना schannel_ssl_backend_दत्त *backend =
    (संरचना schannel_ssl_backend_दत्त *)connssl->backend;

  DEBUGASSERT(दत्त);
  DEBUGASSERT(backend);

  यदि(backend->shutdown_token.pv_बफर)
    Curl_pSecFn->Free_Context_बफर(backend->shutdown_token.pv_बफर);
  स्मृतिं_पूरय(&backend->shutdown_token, 0, परिमाण(backend->shutdown_token));
  backend->shutdown_विस्थापनम् = 0;

  /* free SSPI Schannel API security context handle */
  यदि(backend->ctxt) {
    यदि(cf->conn->sslContext == &backend->ctxt->ctxt_handle)
      cf->conn->sslContext = रिक्त;

    DEBUGF(infof(दत्त, "schannel: clear security context handle"));
    Curl_pSecFn->DeleteSecurityContext(&backend->ctxt->ctxt_handle);
    curlx_safefree(backend->ctxt);
  }

  /* free SSPI Schannel API credential handle */
  यदि(backend->cred) {
    Curl_ssl_scache_तालकम्(दत्त);
    schannel_session_free(backend->cred);
    Curl_ssl_scache_unlock(दत्त);
    backend->cred = रिक्त;
  }

  /* free the buffer used to encrypt outgoing data */
  curlx_safefree(backend->send_बफर);
  backend->send_बफर_len = 0;

  /* free internal buffer for received encrypted data */
  यदि(backend->encdata.बफर) {
    curlx_safefree(backend->encdata.बफर);
    backend->encdata.दीर्घता = 0;
    backend->encdata.विस्थापनम् = 0;
    backend->encdata_is_incomplete = FALSE;
  }

  /* free internal buffer for received decrypted data */
  यदि(backend->decdata.बफर) {
    curlx_safefree(backend->decdata.बफर);
    backend->decdata.दीर्घता = 0;
    backend->decdata.विस्थापनम् = 0;
  }
}

स्थिर पूर्णाङ्क schannel_init(शून्य)
{
  प्रकारनिर्देश ध्रुव वर्ण *(APIENTRY *WINE_GET_संस्करणम्_FN)(शून्य);
#यदि defined(__clang__) && __clang_major__ >= 16
#निर्देशः clang diagnostic प्रक्षिप
#निर्देशः clang diagnostic ignored "-Wcast-function-type-strict"
#यदि_अन्तः
  WINE_GET_संस्करणम्_FN p_wine_get_संस्करणम् =
    CURLX_FUNCTION_CAST(WINE_GET_संस्करणम्_FN,
      Get_Proc_स्थानाङ्कः(GetModuleHandle(TEXT("ntdll")), "wine_get_version"));
#यदि defined(__clang__) && __clang_major__ >= 16
#निर्देशः clang diagnostic निष्कासय
#यदि_अन्तः
  यदि(p_wine_get_संस्करणम्) {  /* WINE detected */
    curl_off_t ver = 0;
    ध्रुव वर्ण *wine_संस्करणम् = p_wine_get_संस्करणम्();  /* e.g. "6.0.2" */
    /* Assume ALPN support with WINE 6.0 or greater */
    यदि(wine_संस्करणम्)
      curlx_str_सङ्ख्या(&wine_संस्करणम्, &ver, 20);
    s_win_has_alpn = (ver >= 6);
  }
  अन्यथा {
    /* ALPN is supported on Windows 8.1 / Server 2012 R2 or newer. */
    s_win_has_alpn = curlx_verify_windows_संस्करणम्(6, 3, 0, PLATFORM_WINNT,
                                                  संस्करणम्_GREATER_THAN_EQUAL);
  }

  प्रत्यागम Curl_sspi_global_init() == CURLE_OK ? 1 : 0;
}

स्थिर शून्य schannel_cleanup(शून्य)
{
  Curl_sspi_global_cleanup();
}

स्थिर size_t schannel_संस्करणम्(वर्ण *बफर, size_t आकार)
{
  प्रत्यागम curl_msnprintf(बफर, आकार, "Schannel");
}

स्थिर CURLcode schannel_random(संरचना Curl_easy *दत्त,
                                अचिह्नित वर्ण *entropy, size_t दीर्घता)
{
  (शून्य)दत्त;

  प्रत्यागम curlx_win32_random(entropy, दीर्घता);
}

स्थिर CURLcode schannel_checksum(ध्रुव अचिह्नित वर्ण *निवेशः,
                                  size_t inputlen,
                                  अचिह्नित वर्ण *checksum,
                                  size_t checksumlen,
                                  DWORD provType,
                                  ध्रुव अचिह्नित पूर्णाङ्क algId)
{
  CURLcode परिणाम = CURLE_FAILED_INIT;

  HCRYPTPROV hProv = 0;
  HCRYPTHASH hHash = 0;

  size_t off;

  DWORD cb_Hash_आकार;
  DWORD dw_Hash_आकार_Len;
  DWORD dwChecksumLen;

  /* since this can fail in multiple ways, zero memory first so we never
   * return old data
   */
  स्मृतिं_पूरय(checksum, 0, checksumlen);

  यदि(!CryptAcquireContext(&hProv, रिक्त, रिक्त, provType,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    गच्छ बहिर्गमनम्;

  यदि(!CryptCreateHash(hProv, algId, 0, 0, &hHash))
    गच्छ बहिर्गमनम्;

  परिणाम = CURLE_BAD_FUNCTION_तर्कः;

  off = 0;
  यावत्(off < inputlen) {
    DWORD chunk = (DWORD)CURLMIN(inputlen - off, 0xffffffffUL);
    यदि(!Crypt_Hash_दत्त(hHash, निवेशः + off, chunk, 0))
      गच्छ बहिर्गमनम्;
    off += chunk;
  }

  /* get hash size */
  cb_Hash_आकार = 0;
  dw_Hash_आकार_Len = (DWORD)परिमाण(cb_Hash_आकार);
  यदि(!CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE *)&cb_Hash_आकार,
                        &dw_Hash_आकार_Len, 0))
    गच्छ बहिर्गमनम्;

  /* check if hash fits into the return buffer */
  यदि(checksumlen < cb_Hash_आकार)
    गच्छ बहिर्गमनम्;

  dwChecksumLen = (DWORD)checksumlen;
  यदि(CryptGetHashParam(hHash, HP_HASHVAL, checksum, &dwChecksumLen, 0) &&
     dwChecksumLen == cb_Hash_आकार)
    परिणाम = CURLE_OK;

बहिर्गमनम्:
  यदि(hHash)
    CryptDestroyHash(hHash);

  यदि(hProv)
    CryptReleaseContext(hProv, 0);

  प्रत्यागम परिणाम;
}

स्थिर CURLcode schannel_sha256sum(ध्रुव अचिह्नित वर्ण *निवेशः,
                                   size_t inputlen,
                                   अचिह्नित वर्ण *sha256sum,
                                   size_t sha256len)
{
  प्रत्यागम schannel_checksum(निवेशः, inputlen, sha256sum, sha256len,
                           PROV_RSA_AES, CALG_SHA_256);
}

स्थिर शून्य *schannel_get_internals(संरचना ssl_connect_दत्त *connssl,
                                    CURLINFO info)
{
  संरचना schannel_ssl_backend_दत्त *backend =
    (संरचना schannel_ssl_backend_दत्त *)connssl->backend;
  (शून्य)info;
  DEBUGASSERT(backend);
  प्रत्यागम &backend->ctxt->ctxt_handle;
}

HCERTSTORE Curl_schannel_get_cached_cert_store(संरचना Curl_cfilter *cf,
                                               संरचना Curl_easy *दत्त)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना Curl_multi *multi = दत्त->multi;
  ध्रुव संरचना curl_blob *ca_info_blob = conn_config->ca_info_blob;
  संरचना schannel_cert_share *share;
  timediff_t timeout_ms;
  अचिह्नित वर्ण info_blob_digest[CURL_SHA256_DIGEST_दीर्घता];

  DEBUGASSERT(multi);

  यदि(!multi) {
    प्रत्यागम रिक्त;
  }

  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_SCHANNEL_CERT_SHARE_कुञ्चिका,
                         CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_कुञ्चिका));
  यदि(!share || !share->cert_store) {
    प्रत्यागम रिक्त;
  }

  /* zero ca_cache_timeout completely disables caching */
  यदि(!दत्त->समुच्चयः.ssl_ca_cache_timeout) {
    प्रत्यागम रिक्त;
  }

  /* check for cache timeout by using the cached_x509_store_expired timediff
     calculation pattern from openssl.c.
     negative timeout means retain forever. */
  timeout_ms = दत्त->समुच्चयः.ssl_ca_cache_timeout * (timediff_t)1000;
  यदि(timeout_ms >= 0) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(दत्त), &share->time);
    यदि(elapsed_ms >= timeout_ms) {
      प्रत्यागम रिक्त;
    }
  }

  यदि(ca_info_blob) {
    यदि(share->CAinfo_blob_आकार != ca_info_blob->len) {
      प्रत्यागम रिक्त;
    }
    यदि(schannel_sha256sum((ध्रुव अचिह्नित वर्ण *)ca_info_blob->दत्त,
                          ca_info_blob->len,
                          info_blob_digest,
                          CURL_SHA256_DIGEST_दीर्घता))
      प्रत्यागम रिक्त;
    यदि(memcmp(share->CAinfo_blob_digest, info_blob_digest,
              CURL_SHA256_DIGEST_दीर्घता)) {
      प्रत्यागम रिक्त;
    }
  }
  अन्यथा {
    यदि(!conn_config->CAfile || !share->CAfile ||
       वाक्यं_तुलय(share->CAfile, conn_config->CAfile)) {
      प्रत्यागम रिक्त;
    }
  }

  प्रत्यागम share->cert_store;
}

स्थिर शून्य schannel_cert_share_free(ध्रुव शून्य *कुञ्चिका, size_t कुञ्चिका_len, शून्य *p)
{
  संरचना schannel_cert_share *share = p;
  DEBUGASSERT(कुञ्चिका_len == CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_कुञ्चिका));
  DEBUGASSERT(!memcmp(MPROTO_SCHANNEL_CERT_SHARE_कुञ्चिका, कुञ्चिका, कुञ्चिका_len));
  (शून्य)कुञ्चिका;
  (शून्य)कुञ्चिका_len;
  यदि(share->cert_store) {
    CertCloseStore(share->cert_store, 0);
  }
  curlx_free(share->CAfile);
  curlx_free(share);
}

bool Curl_schannel_समुच्चयः_cached_cert_store(संरचना Curl_cfilter *cf,
                                         संरचना Curl_easy *दत्त,
                                         HCERTSTORE cert_store)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना Curl_multi *multi = दत्त->multi;
  ध्रुव संरचना curl_blob *ca_info_blob = conn_config->ca_info_blob;
  संरचना schannel_cert_share *share;
  अचिह्नित वर्ण digest[CURL_SHA256_DIGEST_दीर्घता];
  वर्ण *CAfile = रिक्त;

  DEBUGASSERT(multi);

  यदि(!multi) {
    प्रत्यागम FALSE;
  }

  यदि(ca_info_blob) {
    यदि(schannel_sha256sum((ध्रुव अचिह्नित वर्ण *)ca_info_blob->दत्त,
                          ca_info_blob->len, digest, परिमाण(digest))) {
      प्रत्यागम FALSE;
    }
  }
  अन्यथा यदि(conn_config->CAfile) {
    CAfile = curlx_strdup(conn_config->CAfile);
    यदि(!CAfile) {
      प्रत्यागम FALSE;
    }
  }

  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_SCHANNEL_CERT_SHARE_कुञ्चिका,
                         CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_कुञ्चिका));
  यदि(!share) {
    share = curlx_calloc(1, परिमाण(*share));
    यदि(!share) {
      curlx_free(CAfile);
      प्रत्यागम FALSE;
    }
    यदि(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_SCHANNEL_CERT_SHARE_कुञ्चिका,
                       CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_कुञ्चिका),
                       share, schannel_cert_share_free)) {
      curlx_free(share);
      curlx_free(CAfile);
      प्रत्यागम FALSE;
    }
  }

  /* free old cache data */
  यदि(share->cert_store) {
    CertCloseStore(share->cert_store, 0);
  }
  curlx_free(share->CAfile);

  यदि(ca_info_blob) {
    स्मृतिं_प्रतिलिख(share->CAinfo_blob_digest, digest, परिमाण(digest));
    share->CAinfo_blob_आकार = ca_info_blob->len;
  }
  अन्यथा
    share->CAinfo_blob_आकार = 0;

  share->time = curlx_now();
  share->cert_store = cert_store;
  share->CAfile = CAfile;
  प्रत्यागम TRUE;
}

ध्रुव संरचना Curl_ssl Curl_ssl_schannel = {
  { CURLSSLBACKEND_SCHANNEL, "schannel" }, /* info */

  SSLSUPP_CERTINFO |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_CA_CACHE |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST,

  परिमाण(संरचना schannel_ssl_backend_दत्त),

  schannel_init,                     /* init */
  schannel_cleanup,                  /* cleanup */
  schannel_संस्करणम्,                  /*संस्करणम्*/
  schannel_shutdown,                 /* shutdown */
  schannel_दत्त_pending,             /* data_pending */
  schannel_random,                   /* random */
  रिक्त,                              /* cert_status_request */
  schannel_connect,                  /* connect */
  Curl_ssl_adjust_pollset,           /* adjust_pollset */
  schannel_get_internals,            /* get_internals */
  schannel_close,                    /* close_one */
  रिक्त,                              /* close_all */
  रिक्त,                              /* engines_list */
  schannel_sha256sum,                /* sha256sum */
  schannel_recv,                     /* recv decrypted data */
  schannel_send,                     /* send data to encrypt */
  रिक्त,                              /* get_channel_binding */
};

#यदि_अन्तः /* USE_SCHANNEL */
