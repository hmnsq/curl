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

#ifdef USE_SCHANNEL

#ifndef USE_WINDOWS_SSPI
#error "cannot compile Schannel support without SSPI."
#endif

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
#ifdef CURL_SCHANNEL_DEV_DEBUG
#define SCH_DEV(x) x
#define SCH_DEV_SHOWBOOL(x)                                     \
  करा {                                                          \
    infof(data, "schannel: " #x " %s", (x) ? "TRUE" : "FALSE"); \
  } पर्यंत(0)
#अन्यथा
#define SCH_DEV(x) करा {} पर्यंत(0)
#define SCH_DEV_SHOWBOOL(x) करा {} पर्यंत(0)
#endif

/* Offered by mingw-w64 v8+, MS SDK 7.0A/VS2010+ */
#ifndef SP_PROT_TLS1_0_CLIENT
#define SP_PROT_TLS1_0_CLIENT           SP_PROT_TLS1_CLIENT
#endif
#ifndef SP_PROT_TLS1_1_CLIENT
#define SP_PROT_TLS1_1_CLIENT           0x00000200
#endif
#ifndef SP_PROT_TLS1_2_CLIENT
#define SP_PROT_TLS1_2_CLIENT           0x00000800
#endif

/* Offered by mingw-w64 v8+, MS SDK 10.0.15063.0/VS2017 15.1+ */
#ifndef SP_PROT_TLS1_3_CLIENT
#define SP_PROT_TLS1_3_CLIENT           0x00002000
#endif
/* Offered by mingw-w64 v8+, MS SDK 8.1/VS2013+ */
#ifndef SCH_USE_STRONG_CRYPTO
#define SCH_USE_STRONG_CRYPTO           0x00400000
#endif

/* Offered by mingw-w64 v10+, MS SDK 7.0A/VS2010+ */
#ifndef SECBUFFER_ALERT
#define SECBUFFER_ALERT                 17
#endif

/* Both schannel buffer sizes must be > 0 */
#define CURL_SCHANNEL_BUFFER_INIT_SIZE   4096
#define CURL_SCHANNEL_BUFFER_FREE_SIZE   1024

#define CERT_THUMBPRINT_STR_LEN 40
#define CERT_THUMBPRINT_DATA_LEN 20

/* Uncomment to force verbose output
 * #define infof(x, y, ...) curl_mprintf(y, __VA_ARGS__)
 * #define failf(x, y, ...) curl_mprintf(y, __VA_ARGS__)
 */

/* Offered by mingw-w64 v4+, MS SDK 6.0A/VS2008+ */
#ifndef PKCS12_NO_PERSIST_किल्ली
#define PKCS12_NO_PERSIST_किल्ली 0x00008000
#endif

/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#ifndef CERT_FIND_HAS_PRIVATE_किल्ली
#define CERT_FIND_HAS_PRIVATE_किल्ली (21 << CERT_COMPARE_SHIFT)
#endif

/* key to use at `multi->proto_hash` */
#define MPROTO_SCHANNEL_CERT_SHARE_किल्ली   "tls:schannel:cert:share"

स्थिर bool s_win_has_alpn;

/* Offered by mingw-w64 v9+, MS SDK 8.1/VS2013+ */
#ifndef SECBUFFER_APPLICATION_PROTOCOLS
#define SECBUFFER_APPLICATION_PROTOCOLS 18
#define SECPKG_ATTR_APPLICATION_PROTOCOL 35

प्रकारव्याख्या गणना {
  SecApplicationProtocolNegotiationExt_None,
  SecApplicationProtocolNegotiationExt_NPN,
  SecApplicationProtocolNegotiationExt_ALPN
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT;

प्रकारव्याख्या गणना {
  SecApplicationProtocolNegotiationStatus_None,
  SecApplicationProtocolNegotiationStatus_Success,
  SecApplicationProtocolNegotiationStatus_SelectedClientOnly
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_STATUS;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
प्रकारव्याख्या संरचना {
  SEC_APPLICATION_PROTOCOL_NEGOTIATION_STATUS ProtoNegoStatus;
  SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT ProtoNegoExt;
  अचिन्हित अक्षर ProtocolIdSize;
  अचिन्हित अक्षर ProtocolId[0xff];
} SecPkgContext_ApplicationProtocol;
#endif

स्थिर शून्य InitSecBuffer(SecBuffer *buffer, अचिन्हित दीर्घ BufType,
                          शून्य *BufDataPtr, अचिन्हित दीर्घ BufByteSize)
{
  buffer->cbBuffer = BufByteSize;
  buffer->BufferType = BufType;
  buffer->pvBuffer = BufDataPtr;
}

स्थिर शून्य InitSecBufferDesc(SecBufferDesc *desc, SecBuffer *BufArr,
                              अचिन्हित दीर्घ अंक_Arr_Elem)
{
  desc->ulVersion = SECBUFFER_VERSION;
  desc->pBuffers = BufArr;
  desc->cBuffers = अंक_Arr_Elem;
}

स्थिर CURLcode schannel_set_ssl_version_min_max(DWORD *enabled_protocols,
                                                 संरचना Curl_cfilter *cf,
                                                 संरचना Curl_easy *data)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  दीर्घ ssl_version = conn_config->version;
  दीर्घ ssl_version_max = (दीर्घ)conn_config->version_max;
  दीर्घ i = ssl_version;

  निवड(ssl_version_max) {
  स्थिती CURL_SSLVERSION_MAX_NONE:
  स्थिती CURL_SSLVERSION_MAX_DEFAULT:

    /* Windows Server 2022 and newer (including Windows 11) support TLS 1.3
       built-in. Previous builds of Windows 10 had broken TLS 1.3
       implementations that could be enabled via registry. */
    जर(curlx_verify_windows_version(10, 0, 20348, PLATFORM_WINNT,
                                    VERSION_GREATER_THAN_EQUAL)) {
      ssl_version_max = CURL_SSLVERSION_MAX_TLSv1_3;
    }
    अन्यथा /* Windows 10 or older */
      ssl_version_max = CURL_SSLVERSION_MAX_TLSv1_2;

    थांबा;
  }

  साठी(; i <= (ssl_version_max >> 16); ++i) {
    निवड(i) {
    स्थिती CURL_SSLVERSION_TLSv1_0:
      *enabled_protocols |= SP_PROT_TLS1_0_CLIENT;
      थांबा;
    स्थिती CURL_SSLVERSION_TLSv1_1:
      *enabled_protocols |= SP_PROT_TLS1_1_CLIENT;
      थांबा;
    स्थिती CURL_SSLVERSION_TLSv1_2:
      *enabled_protocols |= SP_PROT_TLS1_2_CLIENT;
      थांबा;
    स्थिती CURL_SSLVERSION_TLSv1_3:

      /* Windows Server 2022 or newer */
      जर(curlx_verify_windows_version(10, 0, 20348, PLATFORM_WINNT,
                                      VERSION_GREATER_THAN_EQUAL)) {
        *enabled_protocols |= SP_PROT_TLS1_3_CLIENT;
        थांबा;
      }
      अन्यथा { /* Windows 10 or older */
        failf(data, "schannel: TLS 1.3 not supported on Windows 10 or older");
        परत CURLE_SSL_CONNECT_ERROR;
      }
    }
  }
  परत CURLE_OK;
}

#define CIPHEROPTION(x) { #x, x }

संरचना algo {
  स्थिरांक अक्षर *name;
  पूर्णांक id;
};

स्थिर स्थिरांक संरचना algo algs[] = {
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
  CIPHEROPTION(CALG_SCHANNEL_MAC_किल्ली),
  CIPHEROPTION(CALG_SCHANNEL_ENC_किल्ली),
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
#ifdef CALG_ECMQV
  CIPHEROPTION(CALG_ECMQV),
#endif
  CIPHEROPTION(CALG_ECDSA),
/* Offered by mingw-w64 v7+, MS SDK 7.0A/VS2010+ */
#ifdef CALG_ECDH_EPHEM
  CIPHEROPTION(CALG_ECDH_EPHEM),
#endif
  { रिक्त, 0 },
};

स्थिर पूर्णांक get_alg_id_by_name(स्थिरांक अक्षर *name)
{
  स्थिरांक अक्षर *nameEnd = म_अक्षर(name, ':');
  माप_प्रकार n = nameEnd ? (माप_प्रकार)(nameEnd - name) : म_माप(name);
  पूर्णांक i;

  साठी(i = 0; algs[i].name; i++) {
    जर((n == म_माप(algs[i].name) && !म_भेदन(algs[i].name, name, n)))
      परत algs[i].id;
  }
  परत 0; /* not found */
}

#define अंक_CIPHERS 47 /* There are a maximum of 47 options listed above */

स्थिर CURLcode set_ssl_ciphers(SCHANNEL_CRED *schannel_cred, अक्षर *ciphers,
                                ALG_ID *algIds)
{
  स्थिरांक अक्षर *startCur = ciphers;
  पूर्णांक alg_मोजणी = 0;
  पर्यंत(startCur && *startCur && (alg_मोजणी < अंक_CIPHERS)) {
    curl_off_t alg;
    जर(curlx_str_संख्या(&startCur, &alg, पूर्णांक_उच्च) || !alg)
      alg = get_alg_id_by_name(startCur);

    जर(alg)
      algIds[alg_मोजणी++] = (ALG_ID)alg;
    अन्यथा जर(!म_भेदन(startCur, "USE_STRONG_CRYPTO",
                     CURL_CSTRLEN("USE_STRONG_CRYPTO")) ||
            !म_भेदन(startCur, "SCH_USE_STRONG_CRYPTO",
                     CURL_CSTRLEN("SCH_USE_STRONG_CRYPTO")))
      schannel_cred->dwFlags |= SCH_USE_STRONG_CRYPTO;
    अन्यथा
      परत CURLE_SSL_CIPHER;
    startCur = म_अक्षर(startCur, ':');
    जर(startCur)
      startCur++;
  }
  schannel_cred->palgSupportedAlgs = algIds;
  schannel_cred->cSupportedAlgs = (DWORD)alg_मोजणी;
  परत CURLE_OK;
}

/* Function allocates memory for store_path only if CURLE_OK is returned */
स्थिर CURLcode get_cert_location(TCHAR *path, DWORD *store_name,
                                  TCHAR **store_path, TCHAR **thumbprint)
{
  TCHAR *sep;
  TCHAR *store_path_start;
  माप_प्रकार store_name_len;

  sep = _tcschr(path, _TEXT('\\'));
  जर(!sep)
    परत CURLE_SSL_CERTPROBLEM;

  store_name_len = sep - path;

  जर(_tcsncmp(path, _TEXT("CurrentUser"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_USER;
  अन्यथा जर(_tcsncmp(path, _TEXT("LocalMachine"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE;
  अन्यथा जर(_tcsncmp(path, _TEXT("CurrentService"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_SERVICE;
  अन्यथा जर(_tcsncmp(path, _TEXT("Services"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_SERVICES;
  अन्यथा जर(_tcsncmp(path, _TEXT("Users"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_USERS;
  अन्यथा जर(_tcsncmp(path, _TEXT("CurrentUserGroupPolicy"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_USER_GROUP_POLICY;
  अन्यथा जर(_tcsncmp(path, _TEXT("LocalMachineGroupPolicy"), store_name_len) ==
          0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE_GROUP_POLICY;
  अन्यथा जर(_tcsncmp(path, _TEXT("LocalMachineEnterprise"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE_ENTERPRISE;
  अन्यथा
    परत CURLE_SSL_CERTPROBLEM;

  store_path_start = sep + 1;

  sep = _tcschr(store_path_start, _TEXT('\\'));
  जर(!sep)
    परत CURLE_SSL_CERTPROBLEM;

  *thumbprint = sep + 1;
  जर(_tcslen(*thumbprint) != CERT_THUMBPRINT_STR_LEN)
    परत CURLE_SSL_CERTPROBLEM;

  *sep = _TEXT('\0');
  *store_path = curlx_tcsdup(store_path_start);
  *sep = _TEXT('\\');
  जर(!*store_path)
    परत CURLE_OUT_OF_MEMORY;

  परत CURLE_OK;
}

स्थिर CURLcode get_client_cert(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                HCERTSTORE *out_cert_store,
                                PCCERT_CONTEXT *out_cert_context)
{
  संरचना ssl_filter_config *sslc = Curl_ssl_cf_get_filter_config(cf);
  PCCERT_CONTEXT client_cert = रिक्त;
  HCERTSTORE client_cert_store = रिक्त;
  CURLcode result = CURLE_OK;

  जर(sslc->clientcert || sslc->cert_blob) {
    DWORD cert_store_name = 0;
    TCHAR *cert_store_path = रिक्त;
    TCHAR *cert_thumbprint_str = रिक्त;
    TCHAR cert_thumbprint_buf[CERT_THUMBPRINT_STR_LEN + 1];
    CRYPT_HASH_BLOB cert_thumbprint;
    BYTE cert_thumbprint_data[CERT_THUMBPRINT_DATA_LEN];
    HCERTSTORE cert_store = रिक्त;
    खाता *fInCert = रिक्त;
    शून्य *certdata = रिक्त;
    माप_प्रकार certsize = 0;
    bool blob = !!sslc->cert_blob;

    जर(blob) {
      certdata = sslc->cert_blob->data;
      certsize = sslc->cert_blob->len;
    }
    अन्यथा {
      TCHAR *cert_path =
        curlx_convert_UTF8_to_tchar(sslc->clientcert);
      जर(!cert_path)
        परत CURLE_OUT_OF_MEMORY;

      result = get_cert_location(cert_path, &cert_store_name,
                                 &cert_store_path, &cert_thumbprint_str);

      /* 'cert_thumbprint_str' points in to the allocated 'cert_path', copy
         the data. The string is verified to be CERT_THUMBPRINT_STR_LEN bytes
         long within the get_cert_location() function. */
      जर(!result && cert_thumbprint_str) {
        स_नकल(cert_thumbprint_buf, cert_thumbprint_str,
               आकार(cert_thumbprint_buf));
        cert_thumbprint_str = cert_thumbprint_buf;
      }

      curlx_free(cert_path);
      जर(result && (sslc->clientcert[0] != '\0'))
        fInCert = curlx_fopen(sslc->clientcert, "rb");

      जर(result && !fInCert) {
        failf(data, "schannel: Failed to get certificate location"
              " or file for %s",
              sslc->clientcert);
        परत result;
      }
    }

    जर((fInCert || blob) && sslc->cert_type &&
       !curl_strequal(sslc->cert_type, "P12")) {
      failf(data, "schannel: certificate format compatibility error "
            "for %s",
            blob ? "(memory blob)" : sslc->clientcert);
      curlx_free(cert_store_path);
      जर(fInCert)
        curlx_fclose(fInCert);
      परत CURLE_SSL_CERTPROBLEM;
    }

    जर(fInCert || blob) {
      /* Reading a .p12 or .pfx file, like the example at bottom of
         https://learn.microsoft.com/archive/msdn-technet-forums/3e7bc95f-b21a-4bcd-bd2c-7f996718cae5
       */
      CRYPT_DATA_BLOB datablob;
      WCHAR *pszPassword;
      माप_प्रकार pwd_len = 0;
      पूर्णांक cert_find_flags;
      स्थिरांक अक्षर *cert_showfilename_error = blob ?
        "(memory blob)" : sslc->clientcert;
      curlx_free(cert_store_path);
      जर(fInCert) {
        दीर्घ cert_tell = 0;
        bool continue_reading = ख_जाओ(fInCert, 0, अंत_से) == 0;
        जर(continue_reading) {
          cert_tell = ख_बताओ(fInCert);
          जर(cert_tell < 0)
            continue_reading = FALSE;
          अन्यथा
            certsize = (माप_प्रकार)cert_tell;
        }
        जर(continue_reading)
          continue_reading = ख_जाओ(fInCert, 0, शुरू_से) == 0;
        जर(continue_reading && (certsize < CURL_MAX_आदान_LENGTH))
          certdata = curlx_malloc(certsize + 1);
        जर(!certdata || ((पूर्णांक)ख_पढ़ो(certdata, certsize, 1, fInCert) != 1))
          continue_reading = FALSE;
        curlx_fclose(fInCert);
        जर(!continue_reading) {
          failf(data, "schannel: Failed to read cert file %s",
                sslc->clientcert);
          curlx_free(certdata);
          परत CURLE_SSL_CERTPROBLEM;
        }
      }

      /* Convert key-pair data to the in-memory certificate store */
      datablob.pbData = (BYTE *)certdata;
      datablob.cbData = (DWORD)certsize;

      जर(sslc->किल्ली_passwd)
        pwd_len = म_माप(sslc->किल्ली_passwd);
      pszPassword = curlx_malloc(आकार(WCHAR) * (pwd_len + 1));
      जर(pszPassword) {
        पूर्णांक str_w_len = 0;
        जर(pwd_len > 0)
          str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                          sslc->किल्ली_passwd, (पूर्णांक)pwd_len,
                                          pszPassword, (पूर्णांक)(pwd_len + 1));

        जर((str_w_len >= 0) && (str_w_len <= (पूर्णांक)pwd_len))
          pszPassword[str_w_len] = 0;
        अन्यथा
          pszPassword[0] = 0;

        cert_store = PFXImportCertStore(&datablob, pszPassword,
                                        PKCS12_NO_PERSIST_किल्ली);
        curlx_memzero(pszPassword, आकार(WCHAR) * (pwd_len + 1));
        curlx_free(pszPassword);
      }
      जर(!blob)
        curlx_free(certdata);
      जर(!cert_store) {
        DWORD errorcode = GetLastError();
        जर(errorcode == ERROR_INVALID_PASSWORD)
          failf(data, "schannel: Failed to import cert file %s, "
                "password is bad",
                cert_showfilename_error);
        अन्यथा
          failf(data, "schannel: Failed to import cert file %s, "
                "last error is 0x%08lx",
                cert_showfilename_error, errorcode);
        परत CURLE_SSL_CERTPROBLEM;
      }

      /* CERT_FIND_HAS_PRIVATE_KEY is only available in Windows 8 / Server
         2012, (NT 6.2). For older versions we use CURL_FIND_ANY. */
      जर(curlx_verify_windows_version(6, 2, 0, PLATFORM_WINNT,
                                      VERSION_GREATER_THAN_EQUAL))
        cert_find_flags = CERT_FIND_HAS_PRIVATE_किल्ली;
      अन्यथा
        cert_find_flags = CERT_FIND_ANY;

      client_cert =
        CertFindCertificateInStore(cert_store,
                                   X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                   cert_find_flags, रिक्त, रिक्त);

      जर(!client_cert) {
        failf(data, "schannel: Failed to get certificate from file %s"
              ", last error is 0x%08lx",
              cert_showfilename_error, GetLastError());
        CertCloseStore(cert_store, 0);
        परत CURLE_SSL_CERTPROBLEM;
      }
    }
    अन्यथा {
      cert_store =
        CertOpenStore(
#ifdef UNICODE
                      CERT_STORE_PROV_SYSTEM_W,
#अन्यथा
                      CERT_STORE_PROV_SYSTEM_A,
#endif
                      0,
                      (HCRYPTPROV)रिक्त,
                      CERT_STORE_OPEN_EXISTING_FLAG | cert_store_name,
                      cert_store_path);
      जर(!cert_store) {
        अक्षर *path_utf8 =
          curlx_convert_tchar_to_UTF8(cert_store_path);
        failf(data, "schannel: Failed to open cert store %lx %s, "
              "last error is 0x%08lx",
              cert_store_name, (path_utf8 ? path_utf8 : "(unknown)"),
              GetLastError());
        curlx_free(cert_store_path);
        curlx_free(path_utf8);
        परत CURLE_SSL_CERTPROBLEM;
      }
      curlx_free(cert_store_path);

      cert_thumbprint.pbData = cert_thumbprint_data;
      cert_thumbprint.cbData = CERT_THUMBPRINT_DATA_LEN;

      जर(!Crypt_String_To_द्विमान(cert_thumbprint_str,
                              CERT_THUMBPRINT_STR_LEN,
                              CRYPT_STRING_HEX,
                              cert_thumbprint_data,
                              &cert_thumbprint.cbData,
                              रिक्त, रिक्त)) {
        CertCloseStore(cert_store, 0);
        परत CURLE_SSL_CERTPROBLEM;
      }

      client_cert =
        CertFindCertificateInStore(cert_store,
                                   X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                   CERT_FIND_HASH, &cert_thumbprint, रिक्त);
      जर(!client_cert) {
        /* CRYPT_E_NOT_FOUND / E_INVALIDARG */
        CertCloseStore(cert_store, 0);
        failf(data, "schannel: client cert not found in cert store");
        परत CURLE_SSL_CERTPROBLEM;
      }
    }
    client_cert_store = cert_store;
  }

  *out_cert_store = client_cert_store;
  *out_cert_context = client_cert;

  परत CURLE_OK;
}

स्थिर CURLcode acquire_sspi_handle(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    संरचना schannel_ssl_backend_data *backend,
                                    PCCERT_CONTEXT client_cert,
                                    DWORD flags,
                                    DWORD enabled_protocols)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CURLcode result;

  /* We support TLS 1.3 starting in Windows Server 2022 or later
   * (OS build 20348) */
  जर(!conn_config->cipher_list &&
     curlx_verify_windows_version(10, 0, 20348, PLATFORM_WINNT,
                                  VERSION_GREATER_THAN_EQUAL)) {

    SCH_CREDENTIALS credentials = { 0 };
    TLS_PARAMETERS tls_parameters = { 0 };
    CRYPTO_SETTINGS crypto_settings[1];
    PCCERT_CONTEXT client_certs[1];

    जर(client_cert)
      client_certs[0] = client_cert;
    अन्यथा
      client_certs[0] = रिक्त;

    स_रखो(crypto_settings, 0, आकार(crypto_settings));

    tls_parameters.pDisabledCrypto = crypto_settings;

    /* The number of blocked suites */
    tls_parameters.cDisabledCrypto = (DWORD)0;
    credentials.pTlsParameters = &tls_parameters;
    credentials.cTlsParameters = 1;

    credentials.dwVersion = SCH_CREDENTIALS_VERSION;
    credentials.dwFlags = flags | SCH_USE_STRONG_CRYPTO;

    credentials.pTlsParameters->grbitDisabledProtocols = ~enabled_protocols;

    जर(client_certs[0]) {
      credentials.cCreds = 1;
      credentials.paCred = client_certs;
    }

    sspi_status =
      Curl_pSecFn->AcquireCredentialsHandle(रिक्त, CURL_UNCONST(UNISP_NAME),
                                            SECPKG_CRED_OUTBOUND, रिक्त,
                                            &credentials, रिक्त, रिक्त,
                                            &backend->cred->cred_handle, रिक्त);
  }
  अन्यथा {
    /* Pre-Windows 10 1809 or the user set a legacy algorithm list.
       Schannel does not negotiate TLS 1.3 when SCHANNEL_CRED is used. */
    ALG_ID algIds[अंक_CIPHERS];
    अक्षर *ciphers = conn_config->cipher_list;
    SCHANNEL_CRED schannel_cred = { 0 };
    PCCERT_CONTEXT client_certs[1];

    जर(client_cert)
      client_certs[0] = client_cert;
    अन्यथा
      client_certs[0] = रिक्त;

    schannel_cred.dwVersion = SCHANNEL_CRED_VERSION;
    schannel_cred.dwFlags = flags;
    schannel_cred.grbitEnabledProtocols = enabled_protocols;

    जर(ciphers) {
      जर((enabled_protocols & SP_PROT_TLS1_3_CLIENT)) {
        जर(!(enabled_protocols & ~SP_PROT_TLS1_3_CLIENT)) {
          failf(data, "schannel: TLS 1.3 is not supported with a cipher list; "
                "remove the cipher list or allow a lower TLS version");
          परत CURLE_SSL_CONNECT_ERROR;
        }
        infof(data, "schannel: WARNING: This version of Schannel "
              "negotiates a less-secure TLS version than TLS 1.3 because the "
              "user set an algorithm cipher list.");
      }
      result = set_ssl_ciphers(&schannel_cred, ciphers, algIds);
      जर(result) {
        failf(data, "schannel: Failed setting algorithm cipher list");
        परत result;
      }
    }
    अन्यथा {
      schannel_cred.dwFlags = flags | SCH_USE_STRONG_CRYPTO;
    }

    जर(client_certs[0]) {
      schannel_cred.cCreds = 1;
      schannel_cred.paCred = client_certs;
    }

    sspi_status =
      Curl_pSecFn->AcquireCredentialsHandle(रिक्त, CURL_UNCONST(UNISP_NAME),
                                            SECPKG_CRED_OUTBOUND, रिक्त,
                                            &schannel_cred, रिक्त, रिक्त,
                                            &backend->cred->cred_handle, रिक्त);
  }

  जर(sspi_status != SEC_E_OK) {
    अक्षर buffer[STRERROR_LEN];
    failf(data, "schannel: AcquireCredentialsHandle failed: %s",
          Curl_sspi_strerror(sspi_status, buffer, आकार(buffer)));
    निवड(sspi_status) {
    स्थिती SEC_E_INSUFFICIENT_MEMORY:
      परत CURLE_OUT_OF_MEMORY;
    स्थिती SEC_E_NO_CREDENTIALS:
    स्थिती SEC_E_SECPKG_NOT_FOUND:
    स्थिती SEC_E_NOT_OWNER:
    स्थिती SEC_E_UNKNOWN_CREDENTIALS:
    स्थिती SEC_E_INTERNAL_ERROR:
    पूर्वनिर्धारित:
      परत CURLE_SSL_CONNECT_ERROR;
    }
  }

  परत CURLE_OK;
}

स्थिर CURLcode schannel_acquire_credential_handle(संरचना Curl_cfilter *cf,
                                                   संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  PCCERT_CONTEXT client_cert = रिक्त;
  HCERTSTORE client_cert_store = रिक्त;
  CURLcode result;

  /* setup Schannel API options */
  DWORD flags = 0;
  DWORD enabled_protocols = 0;

  संरचना schannel_ssl_backend_data *backend =
    (संरचना schannel_ssl_backend_data *)connssl->backend;

  DEBUGASSERT(backend);

  जर(conn_config->verifypeer) {
    जर(backend->use_manual_cred_validation)
      flags = SCH_CRED_MANUAL_CRED_VALIDATION;
    अन्यथा
      flags = SCH_CRED_AUTO_CRED_VALIDATION;

    जर(conn_config->no_revoke) {
      flags |= SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
               SCH_CRED_IGNORE_REVOCATION_OFFLINE;

      DEBUGF(infof(data, "schannel: disabled server certificate revocation "
                         "checks"));
    }
    अन्यथा जर(conn_config->revoke_best_effort) {
      flags |= SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
               SCH_CRED_IGNORE_REVOCATION_OFFLINE |
               SCH_CRED_REVOCATION_CHECK_CHAIN;

      DEBUGF(infof(data, "schannel: ignore revocation offline errors"));
    }
    अन्यथा {
      flags |= SCH_CRED_REVOCATION_CHECK_CHAIN;

      DEBUGF(infof(data, "schannel: checking server certificate revocation"));
    }
  }
  अन्यथा {
    flags = SCH_CRED_MANUAL_CRED_VALIDATION |
            SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
            SCH_CRED_IGNORE_REVOCATION_OFFLINE;
    DEBUGF(infof(data, "schannel: disabled server cert revocation checks"));
  }

  जर(!conn_config->verifyhost) {
    flags |= SCH_CRED_NO_SERVERNAME_CHECK;
    DEBUGF(infof(data, "schannel: verifyhost setting prevents Schannel from "
                       "comparing the supplied target name with the subject "
                       "names in server certificates."));
  }

  जर(!conn_config->auto_client_cert) {
    flags &= ~(DWORD)SCH_CRED_USE_DEFAULT_CREDS;
    flags |= SCH_CRED_NO_DEFAULT_CREDS;
    infof(data, "schannel: disabled automatic use of client certificate");
  }
  अन्यथा
    infof(data, "schannel: enabled automatic use of client certificate");

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  निवड(conn_config->version) {
  स्थिती CURL_SSLVERSION_TLSv1:
  स्थिती CURL_SSLVERSION_TLSv1_0:
  स्थिती CURL_SSLVERSION_TLSv1_1:
  स्थिती CURL_SSLVERSION_TLSv1_2:
  स्थिती CURL_SSLVERSION_TLSv1_3: {
    result = schannel_set_ssl_version_min_max(&enabled_protocols, cf, data);
    जर(result)
      परत result;
    थांबा;
  }
  स्थिती CURL_SSLVERSION_SSLv3:
  स्थिती CURL_SSLVERSION_SSLv2:
    failf(data, "SSL versions not supported");
    परत CURLE_NOT_BUILT_IN;
  पूर्वनिर्धारित:
    failf(data, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    परत CURLE_SSL_CONNECT_ERROR;
  }

  result = get_client_cert(cf, data, &client_cert_store, &client_cert);
  जर(result)
    परत result;

  /* allocate memory for the reusable credential handle */
  backend->cred = (संरचना Curl_schannel_cred *)
    curlx_calloc(1, आकार(संरचना Curl_schannel_cred));
  जर(!backend->cred) {
    failf(data, "schannel: unable to allocate memory");

    जर(client_cert)
      CertFreeCertificateContext(client_cert);
    जर(client_cert_store)
      CertCloseStore(client_cert_store, 0);

    परत CURLE_OUT_OF_MEMORY;
  }
  backend->cred->refcount = 1;

  /* Since we did not persist the key, we need to extend the store's
   * lifetime until the end of the connection
   */
  backend->cred->client_cert_store = client_cert_store;

  result = acquire_sspi_handle(cf, data, backend, client_cert,
                               flags, enabled_protocols);

  जर(client_cert)
    CertFreeCertificateContext(client_cert);

  परत result;
}

स्थिर CURLcode schannel_connect_step1(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *data)
{
  माप_प्रकार written = 0;
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना schannel_ssl_backend_data *backend =
    (संरचना schannel_ssl_backend_data *)connssl->backend;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SecBuffer outbuf;
  SecBufferDesc outbuf_desc;
  SecBuffer inbuf;
  SecBufferDesc inbuf_desc;
  अचिन्हित अक्षर alpn_buffer[128];
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CURLcode result;

  DEBUGASSERT(backend);
  DEBUGF(infof(data, "schannel: SSL/TLS connection with %s port %d (step 1/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->port));

  backend->use_alpn = connssl->alpn && s_win_has_alpn;

  जर(conn_config->CAfile || conn_config->ca_info_blob) {
    जर(curlx_verify_windows_version(6, 1, 0, PLATFORM_WINNT,
                                    VERSION_GREATER_THAN_EQUAL)) {
      backend->use_manual_cred_validation = TRUE;
    }
    अन्यथा {
      failf(data, "schannel: this version of Windows is too old to support "
                  "certificate verification via CA bundle file.");
      परत CURLE_SSL_CACERT_BADFILE;
    }
  }
  अन्यथा
    backend->use_manual_cred_validation = FALSE;

  backend->cred = रिक्त;

  /* check for an existing reusable credential handle */
  जर(Curl_ssl_scache_use(cf, data)) {
    संरचना Curl_schannel_cred *old_cred;
    Curl_ssl_scache_lock(data);
    old_cred = Curl_ssl_scache_get_obj(cf, data, connssl->peer.scache_किल्ली);
    जर(old_cred) {
      backend->cred = old_cred;
      DEBUGF(infof(data, "schannel: reusing existing credential handle"));

      /* increment the reference counter of the credential/session handle */
      backend->cred->refcount++;
      DEBUGF(infof(data,
                   "schannel: incremented credential handle refcount = %d",
                   backend->cred->refcount));
    }
    Curl_ssl_scache_unlock(data);
  }

  जर(!backend->cred) {
    अक्षर *snihost;
    result = schannel_acquire_credential_handle(cf, data);
    जर(result || !backend->cred)
      परत result;
    /* schannel_acquire_credential_handle() sets backend->cred accordingly or
       it returns error otherwise. */

    /* A hostname associated with the credential is needed by
       InitializeSecurityContext for SNI and other reasons. */
    snihost = connssl->peer.sni ?
      connssl->peer.sni : connssl->peer.origin->hostname;
    backend->cred->sni_hostname = curlx_convert_UTF8_to_tchar(snihost);
    जर(!backend->cred->sni_hostname)
      परत CURLE_OUT_OF_MEMORY;
  }

  /* Warn if SNI is disabled due to use of an IP address */
  जर(connssl->peer.type != CURL_SSL_PEER_DNS) {
    infof(data, "schannel: using IP address, SNI is not supported by OS.");
  }

  जर(backend->use_alpn) {
    पूर्णांक cur = 0;
    पूर्णांक list_start_index = 0;
    अचिन्हित पूर्णांक *extension_len = रिक्त;
    अचिन्हित लघु *list_len = रिक्त;
    स्थिरांक अचिन्हित पूर्णांक uiALPN = SecApplicationProtocolNegotiationExt_ALPN;
    संरचना alpn_proto_buf proto;

    /* The first four bytes is an unsigned int indicating number
       of bytes of data in the rest of the buffer. */
    extension_len = (अचिन्हित पूर्णांक *)(शून्य *)&alpn_buffer[cur];
    cur += (पूर्णांक)आकार(अचिन्हित पूर्णांक);

    /* The next four bytes are an indicator that this buffer contains
       ALPN data, as opposed to NPN, for example. */
    स_नकल(&alpn_buffer[cur], &uiALPN, आकार(uiALPN));
    cur += (पूर्णांक)आकार(अचिन्हित पूर्णांक);

    /* The next two bytes is an unsigned short indicating the number
       of bytes used to list the preferred protocols. */
    list_len = (अचिन्हित लघु *)(शून्य *)&alpn_buffer[cur];
    cur += (पूर्णांक)आकार(अचिन्हित लघु);

    list_start_index = cur;

    result = Curl_alpn_to_proto_buf(&proto, connssl->alpn);
    जर(result) {
      failf(data, "Error setting ALPN");
      परत CURLE_SSL_CONNECT_ERROR;
    }
    स_नकल(&alpn_buffer[cur], proto.data, proto.len);
    cur += proto.len;

    *list_len = curlx_uitous(cur - list_start_index);
    *extension_len = (अचिन्हित पूर्णांक)(*list_len +
      आकार(अचिन्हित पूर्णांक) + आकार(अचिन्हित लघु));

    InitSecBuffer(&inbuf, SECBUFFER_APPLICATION_PROTOCOLS, alpn_buffer, cur);
    InitSecBufferDesc(&inbuf_desc, &inbuf, 1);

    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
  }
  अन्यथा {
    InitSecBuffer(&inbuf, SECBUFFER_EMPTY, रिक्त, 0);
    InitSecBufferDesc(&inbuf_desc, &inbuf, 1);
  }

  /* setup output buffer */
  InitSecBuffer(&outbuf, SECBUFFER_EMPTY, रिक्त, 0);
  InitSecBufferDesc(&outbuf_desc, &outbuf, 1);

  /* security request flags */
  backend->req_flags =
    ISC_REQ_SEQUENCE_DETECT | ISC_REQ_REPLAY_DETECT |
    ISC_REQ_CONFIDENTIALITY | ISC_REQ_ALLOCATE_MEMORY |
    ISC_REQ_STREAM |
    (!conn_config->auto_client_cert ? ISC_REQ_USE_SUPPLIED_CREDS : 0);

  /* allocate memory for the security context handle */
  backend->ctxt = (संरचना Curl_schannel_ctxt *)
    curlx_calloc(1, आकार(संरचना Curl_schannel_ctxt));
  जर(!backend->ctxt) {
    failf(data, "schannel: unable to allocate memory");
    परत CURLE_OUT_OF_MEMORY;
  }

  /* Schannel InitializeSecurityContext:
     https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-initializesecuritycontextw

     At the moment we do not pass inbuf unless we are using ALPN since we only
     use it for that, and WINE (for which we currently disable ALPN) is giving
     us problems with inbuf regardless. https://github.com/curl/curl/issues/983
   */
  sspi_status = Curl_pSecFn->InitializeSecurityContext(
    &backend->cred->cred_handle, रिक्त, backend->cred->sni_hostname,
    backend->req_flags, 0, 0,
    (backend->use_alpn ? &inbuf_desc : रिक्त),
    0, &backend->ctxt->ctxt_handle,
    &outbuf_desc, &backend->ret_flags, रिक्त);

  जर(sspi_status != SEC_I_CONTINUE_NEEDED) {
    अक्षर buffer[STRERROR_LEN];
    curlx_safefree(backend->ctxt);
    निवड(sspi_status) {
    स्थिती SEC_E_INSUFFICIENT_MEMORY:
      failf(data, "schannel: initial InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(sspi_status, buffer, आकार(buffer)));
      परत CURLE_OUT_OF_MEMORY;
    स्थिती SEC_E_WRONG_PRINCIPAL:
      failf(data, "schannel: SNI or certificate check failed: %s",
            Curl_sspi_strerror(sspi_status, buffer, आकार(buffer)));
      परत CURLE_PEER_FAILED_VERIFICATION;
#जर 0
    स्थिती SEC_E_INVALID_HANDLE:
    स्थिती SEC_E_INVALID_TOKEN:
    स्थिती SEC_E_LOGON_DENIED:
    स्थिती SEC_E_TARGET_UNKNOWN:
    स्थिती SEC_E_NO_AUTHENTICATING_AUTHORITY:
    स्थिती SEC_E_INTERNAL_ERROR:
    स्थिती SEC_E_NO_CREDENTIALS:
    स्थिती SEC_E_UNSUPPORTED_FUNCTION:
    स्थिती SEC_E_APPLICATION_PROTOCOL_MISMATCH:
#endif
    पूर्वनिर्धारित:
      failf(data, "schannel: initial InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(sspi_status, buffer, आकार(buffer)));
      परत CURLE_SSL_CONNECT_ERROR;
    }
  }

  DEBUGF(infof(data, "schannel: sending initial handshake data: "
               "sending %lu bytes.", outbuf.cbBuffer));

  /* send initial handshake data which is now stored in output buffer */
  result = Curl_conn_cf_send(cf->next, data,
                             (स्थिरांक uint8_t *)outbuf.pvBuffer,
                             outbuf.cbBuffer, FALSE,
                             &written);
  Curl_pSecFn->FreeContextBuffer(outbuf.pvBuffer);
  जर(result || (outbuf.cbBuffer != written)) {
    failf(data, "schannel: failed to send initial handshake data: "
          "sent %zu of %lu bytes", written, outbuf.cbBuffer);
    परत CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGF(infof(data, "schannel: sent initial handshake data: "
               "sent %zu bytes", written));

  backend->recv_unrecoverable_err = CURLE_OK;
  backend->recv_sspi_close_notify = FALSE;
  backend->recv_connection_closed = FALSE;
  backend->recv_renegotiating = FALSE;
  backend->renegotiate_state.started = FALSE;
  backend->encdata_is_incomplete = FALSE;
  backend->shutdown_generated = FALSE;
  backend->shutdown_offset = 0;
  स_रखो(&backend->shutdown_token, 0, आकार(backend->shutdown_token));

  /* continue to second handshake step */
  connssl->connecting_state = ssl_connect_2;

  परत CURLE_OK;
}

स्थिर CURLcode schannel_error(संरचना Curl_easy *data,
                               SECURITY_STATUS sspi_status)
{
  अक्षर buffer[STRERROR_LEN];
  निवड(sspi_status) {
  स्थिती SEC_E_INSUFFICIENT_MEMORY:
    failf(data, "schannel: next InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(sspi_status, buffer, आकार(buffer)));
    परत CURLE_OUT_OF_MEMORY;
  स्थिती SEC_E_WRONG_PRINCIPAL:
    failf(data, "schannel: SNI or certificate check failed: %s",
          Curl_sspi_strerror(sspi_status, buffer, आकार(buffer)));
    परत CURLE_PEER_FAILED_VERIFICATION;
  स्थिती SEC_E_UNTRUSTED_मूळ:
    failf(data, "schannel: %s",
          Curl_sspi_strerror(sspi_status, buffer, आकार(buffer)));
    परत CURLE_PEER_FAILED_VERIFICATION;
#जर 0
  स्थिती SEC_E_INVALID_HANDLE:
  स्थिती SEC_E_INVALID_TOKEN:
  स्थिती SEC_E_LOGON_DENIED:
  स्थिती SEC_E_TARGET_UNKNOWN:
  स्थिती SEC_E_NO_AUTHENTICATING_AUTHORITY:
  स्थिती SEC_E_INTERNAL_ERROR:
  स्थिती SEC_E_NO_CREDENTIALS:
  स्थिती SEC_E_UNSUPPORTED_FUNCTION:
  स्थिती SEC_E_APPLICATION_PROTOCOL_MISMATCH:
#endif
  पूर्वनिर्धारित:
    failf(data, "schannel: next InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(sspi_status, buffer, आकार(buffer)));
    परत CURLE_SSL_CONNECT_ERROR;
  }
}

स्थिर CURLcode schannel_pkp_pin_peer_pubkey(संरचना Curl_cfilter *cf,
                                             संरचना Curl_easy *data,
                                             स्थिरांक अक्षर *pinnedpubkey)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना schannel_ssl_backend_data *backend =
    (संरचना schannel_ssl_backend_data *)connssl->backend;
  CERT_CONTEXT *pCertContextServer = रिक्त;

  /* Result is returned to caller */
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  DEBUGASSERT(backend);

  /* if a path was not specified, do not pin */
  जर(!pinnedpubkey)
    परत CURLE_OK;

  करा {
    SECURITY_STATUS sspi_status;
    uint8_t *x509_der;
    DWORD x509_der_len;
    संरचना Curl_X509certificate x509_parsed;
    संरचना Curl_asn1Element *pubkey;

    sspi_status =
      Curl_pSecFn->QueryContextAttributes(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                          &pCertContextServer);

    जर((sspi_status != SEC_E_OK) || !pCertContextServer) {
      अक्षर buffer[STRERROR_LEN];
      failf(data, "schannel: Failed to read remote certificate context: %s",
            Curl_sspi_strerror(sspi_status, buffer, आकार(buffer)));
      थांबा; /* failed */
    }

    जर(!(((pCertContextServer->dwCertEncodingType & X509_ASN_ENCODING) != 0) &&
         (pCertContextServer->cbCertEncoded > 0)))
      थांबा;

    x509_der = (uint8_t *)pCertContextServer->pbCertEncoded;
    x509_der_len = pCertContextServer->cbCertEncoded;
    स_रखो(&x509_parsed, 0, आकार(x509_parsed));
    जर(Curl_parseX509(&x509_parsed, x509_der, x509_der + x509_der_len))
      थांबा;

    pubkey = &x509_parsed.subject_Public_किल्ली_Info;
    जर(!pubkey->header || pubkey->end <= pubkey->header) {
      failf(data, "SSL: failed retrieving public key from server certificate");
      थांबा;
    }

    result = Curl_pin_peer_pubkey(data,
                                  pinnedpubkey,
                                  (स्थिरांक अचिन्हित अक्षर *)pubkey->header,
                                  (माप_प्रकार)(pubkey->end - pubkey->header));
    जर(result) {
      failf(data, "SSL: public key does not match pinned public key");
    }
  } पर्यंत(0);

  जर(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  परत result;
}

स्थिर CURLcode ensure_encoding_size(संरचना Curl_easy *data,
                                     संरचना sbuffer *encdata,
                                     माप_प्रकार min_length)
{
  माप_प्रकार size;
  DEBUGASSERT(encdata->length >= encdata->offset);
  जर(encdata->length < encdata->offset)
    परत CURLE_FAILED_INIT;
  size = encdata->length - encdata->offset;
  जर(size < CURL_SCHANNEL_BUFFER_FREE_SIZE || encdata->length < min_length) {
    अचिन्हित अक्षर *buffer;
    माप_प्रकार length = encdata->offset + CURL_SCHANNEL_BUFFER_FREE_SIZE;
    जर(length < min_length)
      length = min_length;
    buffer = curlx_realloc(encdata->buffer, length);
    जर(!buffer) {
      failf(data, "schannel: unable to re-allocate memory");
      परत CURLE_OUT_OF_MEMORY;
    }

    encdata->buffer = buffer;
    encdata->length = length;
    SCH_DEV(infof(data, "schannel: encdata.buffer resized %zu",
                  encdata->length));
  }
  परत CURLE_OK;
}

स्थिर CURLcode ensure_decoding_size(संरचना Curl_easy *data,
                                     संरचना sbuffer *decdata,
                                     माप_प्रकार nowsize,
                                     माप_प्रकार len)
{
  माप_प्रकार size = nowsize > CURL_SCHANNEL_BUFFER_FREE_SIZE ?
    nowsize : CURL_SCHANNEL_BUFFER_FREE_SIZE;
  DEBUGASSERT(decdata->length >= decdata->offset);
  जर(decdata->length < decdata->offset)
    परत CURLE_FAILED_INIT;
  अन्यथा जर(decdata->length - decdata->offset < size ||
          decdata->length < len) {
    /* increase internal decrypted data buffer */
    माप_प्रकार length = decdata->offset + size;
    अचिन्हित अक्षर *buffer;
    /* make sure that the requested amount of data fits */
    जर(length < len)
      length = len;

    buffer = curlx_realloc(decdata->buffer, length);
    जर(!buffer) {
      failf(data, "schannel: unable to re-allocate memory");
      परत CURLE_OUT_OF_MEMORY;
    }
    decdata->buffer = buffer;
    decdata->length = length;
  }
  परत CURLE_OK;
}

स्थिर CURLcode schannel_connect_step2(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना schannel_ssl_backend_data *backend =
    (संरचना schannel_ssl_backend_data *)connssl->backend;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  पूर्णांक i;
  माप_प्रकार nread = 0;
  SecBuffer outbuf[3];
  SecBufferDesc outbuf_desc;
  SecBuffer inbuf[2];
  SecBufferDesc inbuf_desc;
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CURLcode result;
  bool doread;
  स्थिरांक अक्षर *pubkey_ptr;

  DEBUGASSERT(backend);

  doread = (connssl->io_need & CURL_SSL_IO_NEED_SEND) ? FALSE : TRUE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  DEBUGF(infof(data, "schannel: SSL/TLS connection with %s port %d (step 2/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->port));

  जर(!backend->cred || !backend->ctxt)
    परत CURLE_SSL_CONNECT_ERROR;

  /* buffer to store previously received and decrypted data */
  जर(!backend->decdata.buffer) {
    backend->decdata.offset = 0;
    backend->decdata.length = CURL_SCHANNEL_BUFFER_INIT_SIZE;
    backend->decdata.buffer = curlx_malloc(backend->decdata.length);
    जर(!backend->decdata.buffer) {
      failf(data, "schannel: unable to allocate memory");
      परत CURLE_OUT_OF_MEMORY;
    }
  }

  /* buffer to store previously received and encrypted data */
  जर(!backend->encdata.buffer) {
    backend->encdata_is_incomplete = FALSE;
    backend->encdata.offset = 0;
    backend->encdata.length = CURL_SCHANNEL_BUFFER_INIT_SIZE;
    backend->encdata.buffer = curlx_malloc(backend->encdata.length);
    जर(!backend->encdata.buffer) {
      failf(data, "schannel: unable to allocate memory");
      परत CURLE_OUT_OF_MEMORY;
    }
  }

  result = ensure_encoding_size(data, &backend->encdata, 0);
  जर(result)
    परत result;

  साठी(;;) {
    जर(doread) {
      /* read encrypted handshake data from socket */
      result = Curl_conn_cf_recv(cf->next, data,
                                 (अक्षर *)(backend->encdata.buffer +
                                          backend->encdata.offset),
                                 backend->encdata.length -
                                 backend->encdata.offset,
                                 &nread);
      जर(result == CURLE_AGAIN) {
        जर(!backend->encdata.offset || backend->encdata_is_incomplete) {
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          DEBUGF(infof(data, "schannel: failed to receive handshake, "
                       "need more data"));
          परत CURLE_OK;
        }
        अन्यथा {
          DEBUGF(infof(data, "schannel: no new handshake data received, "
                       "continuing to process existing handshake data"));
        }
      }
      अन्यथा जर(result || (nread == 0)) {
        failf(data, "schannel: failed to receive handshake, "
              "SSL/TLS connection failed");
        परत CURLE_SSL_CONNECT_ERROR;
      }
      अन्यथा {
        /* increase encrypted data buffer offset */
        backend->encdata.offset += nread;
        backend->encdata_is_incomplete = FALSE;
        SCH_DEV(infof(data, "schannel: encrypted data got %zu", nread));
      }
    }

    SCH_DEV(infof(data,
                  "schannel: encrypted data buffer: offset %zu length %zu",
                  backend->encdata.offset, backend->encdata.length));

    /* setup input buffers */
    InitSecBuffer(&inbuf[0], SECBUFFER_TOKEN,
                  curlx_memdup0((स्थिरांक अक्षर *)backend->encdata.buffer,
                                backend->encdata.offset),
                  curlx_uztoul(backend->encdata.offset));
    InitSecBuffer(&inbuf[1], SECBUFFER_EMPTY, रिक्त, 0);
    InitSecBufferDesc(&inbuf_desc, inbuf, 2);

    /* setup output buffers */
    InitSecBuffer(&outbuf[0], SECBUFFER_TOKEN, रिक्त, 0);
    InitSecBuffer(&outbuf[1], SECBUFFER_ALERT, रिक्त, 0);
    InitSecBuffer(&outbuf[2], SECBUFFER_EMPTY, रिक्त, 0);
    InitSecBufferDesc(&outbuf_desc, outbuf, 3);

    जर(!inbuf[0].pvBuffer) {
      failf(data, "schannel: unable to allocate memory");
      परत CURLE_OUT_OF_MEMORY;
    }

    /* The socket must be writable (or a poll error occurred) before we call
       InitializeSecurityContext to continue processing the received TLS
       records. This is because that function is not idempotent and we do not
       support partial save/resume sending replies of handshake tokens. */
    जर(!SOCKET_WRITABLE(Curl_conn_cf_get_socket(cf, data), 0)) {
      SCH_DEV(infof(data, "schannel: handshake waiting for writable socket"));
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      curlx_free(inbuf[0].pvBuffer);
      परत CURLE_OK;
    }

    sspi_status = Curl_pSecFn->InitializeSecurityContext(
      &backend->cred->cred_handle, &backend->ctxt->ctxt_handle,
      backend->cred->sni_hostname, backend->req_flags,
      0, 0, &inbuf_desc, 0, रिक्त,
      &outbuf_desc, &backend->ret_flags, रिक्त);

    /* free buffer for received handshake data */
    curlx_safefree(inbuf[0].pvBuffer);

    /* check if the handshake was incomplete */
    निवड(sspi_status) {
    स्थिती SEC_E_INCOMPLETE_MESSAGE:
      backend->encdata_is_incomplete = TRUE;
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      DEBUGF(infof(data,
                   "schannel: received incomplete message, need more data"));
      परत CURLE_OK;

    स्थिती SEC_I_CONTINUE_NEEDED:
    स्थिती SEC_E_OK:
      /* check if the handshake needs to be continued */
      result = CURLE_OK;
      साठी(i = 0; i < 3; i++) {
        /* search for handshake tokens that need to be sent */
        जर(outbuf[i].BufferType == SECBUFFER_TOKEN && outbuf[i].cbBuffer > 0) {
          माप_प्रकार written = 0;
          DEBUGF(infof(data, "schannel: sending next handshake data: "
                       "sending %lu bytes.", outbuf[i].cbBuffer));

          /* send handshake token to server */
          result = Curl_conn_cf_send(cf->next, data,
                                     (स्थिरांक uint8_t *)outbuf[i].pvBuffer,
                                     outbuf[i].cbBuffer,
                                     FALSE, &written);
          जर(result || (outbuf[i].cbBuffer != written)) {
            failf(data, "schannel: failed to send next handshake data: "
                  "sent %zu of %lu bytes", written, outbuf[i].cbBuffer);
            result = CURLE_SSL_CONNECT_ERROR;
          }
        }
      }
      साठी(i = 0; i < 3; i++) {
        /* free obsolete buffer */
        जर(outbuf[i].pvBuffer)
          Curl_pSecFn->FreeContextBuffer(outbuf[i].pvBuffer);
      }
      जर(result)
        परत result;
      थांबा;

    स्थिती SEC_I_INCOMPLETE_CREDENTIALS:
      जर(!(backend->req_flags & ISC_REQ_USE_SUPPLIED_CREDS)) {
        /* If the server has requested a client certificate, attempt to
           continue the handshake without one. This allows connections to
           servers which request a client certificate but do not require
           it. */
        backend->req_flags |= ISC_REQ_USE_SUPPLIED_CREDS;
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        DEBUGF(infof(data,
                     "schannel: a client certificate has been requested"));
        परत CURLE_OK;
      }
      FALLTHROUGH();

    पूर्वनिर्धारित:
      परत schannel_error(data, sspi_status);
    }

    /* check if there was additional remaining encrypted data */
    जर(inbuf[1].BufferType == SECBUFFER_EXTRA && inbuf[1].cbBuffer > 0) {
      SCH_DEV(infof(data, "schannel: encrypted data length: %lu",
                    inbuf[1].cbBuffer));
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
      जर(backend->encdata.offset > inbuf[1].cbBuffer) {
        स_हटाओ(backend->encdata.buffer,
                (backend->encdata.buffer + backend->encdata.offset) -
                inbuf[1].cbBuffer, inbuf[1].cbBuffer);
        backend->encdata.offset = inbuf[1].cbBuffer;
        जर(sspi_status == SEC_I_CONTINUE_NEEDED) {
          doread = FALSE;
          सुरूठेवा;
        }
      }
    }
    अन्यथा {
      backend->encdata.offset = 0;
    }
    थांबा;
  }

  /* check if the handshake needs to be continued */
  जर(sspi_status == SEC_I_CONTINUE_NEEDED) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    परत CURLE_OK;
  }

  /* check if the handshake is complete */
  जर(sspi_status == SEC_E_OK) {
    connssl->connecting_state = ssl_connect_3;
    DEBUGF(infof(data, "schannel: SSL/TLS handshake complete"));
  }

#ifndef CURL_DISABLE_PROXY
  pubkey_ptr = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#अन्यथा
  pubkey_ptr = CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
  जर(pubkey_ptr) {
    result = schannel_pkp_pin_peer_pubkey(cf, data, pubkey_ptr);
    जर(result) {
      failf(data, "SSL: public key does not match pinned public key");
      परत result;
    }
  }

  जर(conn_config->verifypeer && backend->use_manual_cred_validation) {
    /* Certificate verification also verifies the hostname if verifyhost */
    परत Curl_verify_certificate(cf, data);
  }

  /* Verify the hostname manually when certificate verification is disabled,
     because in that case Schannel does not verify it. */
  जर(!conn_config->verifypeer && conn_config->verifyhost)
    परत Curl_verify_host(cf, data);

  परत CURLE_OK;
}

स्थिर bool valid_cert_encoding(स्थिरांक CERT_CONTEXT *cert_context)
{
  परत cert_context &&
    ((cert_context->dwCertEncodingType & X509_ASN_ENCODING) != 0) &&
    cert_context->pbCertEncoded &&
    (cert_context->cbCertEncoded > 0);
}

प्रकारव्याख्या bool (*Read_crt_func)(स्थिरांक CERT_CONTEXT *ccert_context,
                              bool reverse_order, शून्य *arg);

स्थिर शून्य traverse_cert_store(स्थिरांक CERT_CONTEXT *context,
                                Read_crt_func func, शून्य *arg)
{
  स्थिरांक CERT_CONTEXT *current_context = रिक्त;
  bool should_continue = TRUE;
  bool first = TRUE;
  bool reverse_order = FALSE;
  पर्यंत(should_continue &&
        (current_context = CertEnumCertificatesInStore(
          context->hCertStore,
          current_context)) != रिक्त) {
    /* Windows 11 22H2 OS Build 22621.674 or higher enumerates certificates in
       leaf-to-root order while all previous versions of Windows enumerate
       certificates in root-to-leaf order. Determine the order of enumeration
       by comparing SECPKG_ATTR_REMOTE_CERT_CONTEXT's pbCertContext with the
       first certificate's pbCertContext. */
    जर(first && context->pbCertEncoded != current_context->pbCertEncoded)
      reverse_order = TRUE;
    should_continue = func(current_context, reverse_order, arg);
    first = FALSE;
  }

  जर(current_context)
    CertFreeCertificateContext(current_context);
}

स्थिर bool cert_counter_callback(स्थिरांक CERT_CONTEXT *ccert_context,
                                  bool reverse_order, शून्य *certs_मोजणी)
{
  (शून्य)reverse_order;
  जर(valid_cert_encoding(ccert_context))
    (*(पूर्णांक *)certs_मोजणी)++;
  जर(*(पूर्णांक *)certs_मोजणी > MAX_ALLOWED_CERT_AMOUNT)
    परत FALSE;
  परत TRUE;
}

संरचना Adder_args {
  संरचना Curl_easy *data;
  CURLcode result;
  पूर्णांक idx;
  पूर्णांक certs_मोजणी;
};

स्थिर bool add_cert_to_certinfo(स्थिरांक CERT_CONTEXT *ccert_context,
                                 bool reverse_order, शून्य *raw_arg)
{
  संरचना Adder_args *args = (संरचना Adder_args *)raw_arg;
  args->result = CURLE_OK;
  जर(valid_cert_encoding(ccert_context)) {
    पूर्णांक घाला_index = reverse_order ? (args->certs_मोजणी - 1) - args->idx :
                       args->idx;
    args->result = Curl_extract_certinfo(args->data, घाला_index,
                                         ccert_context->pbCertEncoded,
                                         ccert_context->pbCertEncoded +
                                         ccert_context->cbCertEncoded);
    args->idx++;
  }
  परत args->result == CURLE_OK;
}

स्थिर शून्य schannel_session_free(शून्य *sessionid)
{
  /* this is expected to be called under sessionid lock */
  संरचना Curl_schannel_cred *cred = sessionid;

  जर(cred) {
    cred->refcount--;
    जर(cred->refcount == 0) {
      Curl_pSecFn->FreeCredentialsHandle(&cred->cred_handle);
      curlx_free(cred->sni_hostname);
      जर(cred->client_cert_store) {
        CertCloseStore(cred->client_cert_store, 0);
        cred->client_cert_store = रिक्त;
      }
      curlx_safefree(cred);
    }
  }
}

स्थिर CURLcode schannel_connect_step3(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना schannel_ssl_backend_data *backend =
    (संरचना schannel_ssl_backend_data *)connssl->backend;
  संरचना ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  CURLcode result = CURLE_OK;
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CERT_CONTEXT *ccert_context = रिक्त;
  SecPkgContext_ApplicationProtocol alpn_result;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);
  DEBUGASSERT(backend);

  DEBUGF(infof(data, "schannel: SSL/TLS connection with %s port %d (step 3/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->port));

  जर(!backend->cred)
    परत CURLE_SSL_CONNECT_ERROR;

  /* check if the required context attributes are met */
  जर(backend->ret_flags != backend->req_flags) {
    जर(!(backend->ret_flags & ISC_RET_SEQUENCE_DETECT))
      failf(data, "schannel: failed to setup sequence detection");
    जर(!(backend->ret_flags & ISC_RET_REPLAY_DETECT))
      failf(data, "schannel: failed to setup replay detection");
    जर(!(backend->ret_flags & ISC_RET_CONFIDENTIALITY))
      failf(data, "schannel: failed to setup confidentiality");
    जर(!(backend->ret_flags & ISC_RET_ALLOCATED_MEMORY))
      failf(data, "schannel: failed to setup memory allocation");
    जर(!(backend->ret_flags & ISC_RET_STREAM))
      failf(data, "schannel: failed to setup stream orientation");
    परत CURLE_SSL_CONNECT_ERROR;
  }

  जर(backend->use_alpn) {
    sspi_status =
      Curl_pSecFn->QueryContextAttributes(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_APPLICATION_PROTOCOL,
                                          &alpn_result);

    जर(sspi_status != SEC_E_OK) {
      failf(data, "schannel: failed to retrieve ALPN result");
      परत CURLE_SSL_CONNECT_ERROR;
    }

    जर(alpn_result.ProtoNegoStatus ==
       SecApplicationProtocolNegotiationStatus_Success) {
      जर(backend->recv_renegotiating &&
         connssl->negotiated.alpn &&
         म_भेदन(connssl->negotiated.alpn,
                 (स्थिरांक अक्षर *)alpn_result.ProtocolId,
                 alpn_result.ProtocolIdSize)) {
        /* Renegotiation selected a different protocol now, we cannot
         * deal with this */
        failf(data, "schannel: server selected an ALPN protocol too late");
        परत CURLE_SSL_CONNECT_ERROR;
      }
      Curl_alpn_set_negotiated(cf, data, connssl, alpn_result.ProtocolId,
                               alpn_result.ProtocolIdSize);
    }
    अन्यथा {
      जर(!backend->recv_renegotiating)
        Curl_alpn_set_negotiated(cf, data, connssl, रिक्त, 0);
    }
  }

  /* save the current session data for possible reuse */
  जर(Curl_ssl_scache_use(cf, data)) {
    Curl_ssl_scache_lock(data);
    /* Up ref count since call takes ownership */
    backend->cred->refcount++;
    result = Curl_ssl_scache_add_obj(cf, data, connssl->peer.scache_किल्ली,
                                     backend->cred, schannel_session_free);
    Curl_ssl_scache_unlock(data);
    जर(result)
      परत result;
  }

  जर(ssl_config->certinfo) {
    पूर्णांक certs_मोजणी = 0;
    sspi_status =
      Curl_pSecFn->QueryContextAttributes(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                          &ccert_context);

    जर((sspi_status != SEC_E_OK) || !ccert_context) {
      failf(data, "schannel: failed to retrieve remote cert context");
      परत CURLE_PEER_FAILED_VERIFICATION;
    }

    traverse_cert_store(ccert_context, cert_counter_callback, &certs_मोजणी);
    जर(certs_मोजणी > MAX_ALLOWED_CERT_AMOUNT) {
      failf(data, "%d certificates is more than allowed (%d)",
            certs_मोजणी, MAX_ALLOWED_CERT_AMOUNT);
      CertFreeCertificateContext(ccert_context);
      परत CURLE_SSL_CONNECT_ERROR;
    }

    result = Curl_ssl_init_certinfo(data, certs_मोजणी);
    जर(!result) {
      संरचना Adder_args args;
      args.data = data;
      args.idx = 0;
      args.certs_मोजणी = certs_मोजणी;
      args.result = CURLE_OK;
      traverse_cert_store(ccert_context, add_cert_to_certinfo, &args);
      result = args.result;
    }
    CertFreeCertificateContext(ccert_context);
    जर(result)
      परत result;
  }

  connssl->connecting_state = ssl_connect_done;

  परत CURLE_OK;
}

स्थिर CURLcode schannel_connect(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 bool *done)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  CURLcode result;

  /* check if the connection has already been established */
  जर(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    परत CURLE_OK;
  }

  *done = FALSE;

  जर(connssl->connecting_state == ssl_connect_1) {
    result = schannel_connect_step1(cf, data);
    जर(result)
      परत result;
  }

  जर(connssl->connecting_state == ssl_connect_2) {
    result = schannel_connect_step2(cf, data);
    जर(result)
      परत result;
  }

  जर(connssl->connecting_state == ssl_connect_3) {
    result = schannel_connect_step3(cf, data);
    जर(result)
      परत result;
  }

  जर(connssl->connecting_state == ssl_connect_done) {
    संरचना schannel_ssl_backend_data *backend =
      (संरचना schannel_ssl_backend_data *)connssl->backend;
    DEBUGASSERT(backend);

    जर(Curl_pSecFn->QueryContextAttributes(
         &backend->ctxt->ctxt_handle,
         SECPKG_ATTR_STREAM_SIZES,
         &backend->stream_sizes)) {
      failf(data, "schannel: failed getting stream sizes");
      परत CURLE_SSL_CONNECT_ERROR;
    }

    connssl->state = ssl_connection_complete;

    /* When SSPI is used in combination with Schannel
     * we need the Schannel context to create the Schannel
     * binding to pass the IIS extended protection checks.
     * Available on Windows 7 or later.
     */
    cf->conn->sslContext = &backend->ctxt->ctxt_handle;

    *done = TRUE;
  }

  परत CURLE_OK;
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
#define MAX_RENEG_BLOCK_TIME (60 * 1000) /* 60 seconds in milliseconds */

/* This function renegotiates the connection due to a server request received
   by schannel_recv. This function returns CURLE_AGAIN if the renegotiation is
   incomplete. In that case, we remain in the renegotiation (connecting) stage
   and future calls to schannel_recv and schannel_send must call this function
   first to complete the renegotiation. */
स्थिर CURLcode schannel_recv_renegotiate(
  संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
  गणना schannel_renegotiate_caller_t caller)
{
  CURLcode result;
  curl_socket_t sockfd;
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना schannel_ssl_backend_data *backend =
    (संरचना schannel_ssl_backend_data *)connssl->backend;
  संरचना schannel_renegotiate_state *rs = &backend->renegotiate_state;

  जर(!backend || !backend->recv_renegotiating) {
    failf(data, "schannel: unexpected call to schannel_recv_renegotiate");
    परत CURLE_SSL_CONNECT_ERROR;
  }
  DEBUGASSERT(caller <= SCH_RENEG_CALLER_IS_SEND);
  जर(caller == SCH_RENEG_CALLER_IS_RECV)
    SCH_DEV(infof(data, "schannel: renegotiation caller is schannel_recv"));
  अन्यथा
    SCH_DEV(infof(data, "schannel: renegotiation caller is schannel_send"));

  sockfd = Curl_conn_cf_get_socket(cf, data);

  जर(sockfd == CURL_SOCKET_BAD) {
    failf(data, "schannel: renegotiation missing socket");
    परत CURLE_SSL_CONNECT_ERROR;
  }

  जर(!rs->started) { /* new renegotiation */
    infof(data, "schannel: renegotiating SSL/TLS connection");
    DEBUGASSERT(connssl->state == ssl_connection_complete);
    DEBUGASSERT(connssl->connecting_state == ssl_connect_done);
    connssl->state = ssl_connection_negotiating;
    connssl->connecting_state = ssl_connect_2;
    स_रखो(rs, 0, आकार(*rs));
    rs->io_need = CURL_SSL_IO_NEED_SEND;
    rs->start_time = *Curl_pgrs_now(data);
    rs->started = TRUE;
  }

  साठी(;;) {
    bool block_read, block_write, blocking, done;
    curl_socket_t readfd, writefd;
    timediff_t elapsed;

    elapsed = curlx_ptimediff_ms(Curl_pgrs_now(data), &rs->start_time);
    जर(elapsed >= MAX_RENEG_BLOCK_TIME) {
      failf(data, "schannel: renegotiation timeout");
      result = CURLE_SSL_CONNECT_ERROR;
      थांबा;
    }

    /* the current io_need state may have been overwritten since the last time
       this function was called. restore the io_need state needed to continue
       the renegotiation. */

    connssl->io_need = rs->io_need;

    result = schannel_connect(cf, data, &done);

    rs->io_need = connssl->io_need;

    जर(!result && !done)
      result = CURLE_AGAIN;

    जर(result != CURLE_AGAIN)
      थांबा;

    readfd = (rs->io_need & CURL_SSL_IO_NEED_RECV) ? sockfd : CURL_SOCKET_BAD;
    writefd = (rs->io_need & CURL_SSL_IO_NEED_SEND) ? sockfd : CURL_SOCKET_BAD;

    जर(readfd == CURL_SOCKET_BAD && writefd == CURL_SOCKET_BAD)
      सुरूठेवा;

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

    block_read = (caller == SCH_RENEG_CALLER_IS_SEND) ? TRUE : FALSE;
    block_write = (caller == SCH_RENEG_CALLER_IS_RECV) ? TRUE : FALSE;

    blocking = (block_read && (readfd != CURL_SOCKET_BAD)) ||
               (block_write && (writefd != CURL_SOCKET_BAD));

    SCH_DEV_SHOWBOOL(block_read);
    SCH_DEV_SHOWBOOL(block_write);
    SCH_DEV_SHOWBOOL(blocking);

    साठी(;;) {
      पूर्णांक what;
      timediff_t timeout_ms, remaining;

      result = Curl_pgrsUpdate(data);
      जर(result)
        थांबा;

      elapsed = curlx_ptimediff_ms(Curl_pgrs_now(data), &rs->start_time);
      जर(elapsed >= MAX_RENEG_BLOCK_TIME) {
        failf(data, "schannel: renegotiation timeout");
        result = CURLE_SSL_CONNECT_ERROR;
        थांबा;
      }
      remaining = MAX_RENEG_BLOCK_TIME - elapsed;

      जर(blocking) {
        timeout_ms = Curl_timeleft_ms(data);

        जर(timeout_ms < 0) {
          result = CURLE_OPERATION_TIMEDOUT;
          थांबा;
        }

        /* the blocking is in intervals so that the progress function can be
           called every second */
        जर(!timeout_ms || timeout_ms > 1000)
          timeout_ms = 1000;

        जर(timeout_ms > remaining)
          timeout_ms = remaining;
      }
      अन्यथा
        timeout_ms = 0;

      SCH_DEV(infof(data, "schannel: renegotiation wait until socket is"
                    "%s%s for up to %" FMT_TIMEDIFF_T " ms",
                    ((readfd != CURL_SOCKET_BAD) ? " readable" : ""),
                    ((writefd != CURL_SOCKET_BAD) ? " writable" : ""),
                    timeout_ms));

      what = Curl_socket_check(readfd, CURL_SOCKET_BAD, writefd, timeout_ms);

      जर(what > 0 && (what & (CURL_CSELECT_IN | CURL_CSELECT_OUT))) {
        SCH_DEV(infof(data, "schannel: renegotiation socket %s%s",
                      ((what & CURL_CSELECT_IN) ? "CURL_CSELECT_IN " : ""),
                      ((what & CURL_CSELECT_OUT) ? "CURL_CSELECT_OUT " : "")));
        result = CURLE_AGAIN;
        थांबा;
      }
      अन्यथा जर(!what) {
        SCH_DEV(infof(data, "schannel: renegotiation socket timeout"));
        जर(blocking)
          सुरूठेवा;
        अन्यथा
          परत CURLE_AGAIN;
      }

      failf(data, "schannel: socket error during renegotiation");
      result = CURLE_SSL_CONNECT_ERROR;
      थांबा;
    }
    जर(result != CURLE_AGAIN)
      थांबा;
  }

  DEBUGASSERT(result != CURLE_AGAIN);

  rs->started = FALSE;
  backend->recv_renegotiating = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  जर(result)
    failf(data, "schannel: renegotiation failed");
  अन्यथा
    infof(data, "schannel: SSL/TLS connection renegotiated");

  परत result;
}

स्थिर CURLcode schannel_send(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                              स्थिरांक शून्य *buf, माप_प्रकार len, माप_प्रकार *pnwritten)
{
  माप_प्रकार data_len = 0;
  अचिन्हित अक्षर *ptr = रिक्त;
  संरचना ssl_connect_data *connssl = cf->ctx;
  SecBuffer outbuf[4];
  SecBufferDesc outbuf_desc;
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CURLcode result = CURLE_OK;
  संरचना schannel_ssl_backend_data *backend =
    (संरचना schannel_ssl_backend_data *)connssl->backend;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  जर(backend->recv_renegotiating) {
    result = schannel_recv_renegotiate(cf, data, SCH_RENEG_CALLER_IS_SEND);
    जर(result)
      परत result;
  }

  /* check if the buffer is longer than the maximum message length */
  जर(len > backend->stream_sizes.cbMaximumMessage) {
    len = backend->stream_sizes.cbMaximumMessage;
  }

  /* calculate the complete message length and prepare the send buffer */
  data_len = backend->stream_sizes.cbHeader + len +
    backend->stream_sizes.cbTrailer;
  जर(data_len > backend->send_buffer_len) {
    ptr = curlx_realloc(backend->send_buffer, data_len);
    जर(!ptr)
      परत CURLE_OUT_OF_MEMORY;
    backend->send_buffer = ptr;
    backend->send_buffer_len = data_len;
  }
  अन्यथा
    ptr = backend->send_buffer;

  /* setup output buffers (header, data, trailer, empty) */
  InitSecBuffer(&outbuf[0], SECBUFFER_STREAM_HEADER,
                ptr, backend->stream_sizes.cbHeader);
  InitSecBuffer(&outbuf[1], SECBUFFER_DATA,
                ptr + backend->stream_sizes.cbHeader, curlx_uztoul(len));
  InitSecBuffer(&outbuf[2], SECBUFFER_STREAM_TRAILER,
                ptr + backend->stream_sizes.cbHeader + len,
                backend->stream_sizes.cbTrailer);
  InitSecBuffer(&outbuf[3], SECBUFFER_EMPTY, रिक्त, 0);
  InitSecBufferDesc(&outbuf_desc, outbuf, 4);

  /* copy data into output buffer */
  स_नकल(outbuf[1].pvBuffer, buf, len);

  /* https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-encryptmessage */
  sspi_status = Curl_pSecFn->EncryptMessage(&backend->ctxt->ctxt_handle, 0,
                                            &outbuf_desc, 0);

  /* check if the message was encrypted */
  जर(sspi_status == SEC_E_OK) {

    /* send the encrypted message including header, data and trailer */
    len = outbuf[0].cbBuffer + outbuf[1].cbBuffer + outbuf[2].cbBuffer;

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
    पर्यंत(len > *pnwritten) {
      माप_प्रकार this_write = 0;
      पूर्णांक what;
      timediff_t timeout_ms = Curl_timeleft_ms(data);
      जर(timeout_ms < 0) {
        /* we already got the timeout */
        failf(data, "schannel: timed out sending data (bytes sent: %zu)",
              *pnwritten);
        result = CURLE_OPERATION_TIMEDOUT;
        थांबा;
      }
      अन्यथा जर(!timeout_ms)
        timeout_ms = TIMEDIFF_T_MAX;
      what = SOCKET_WRITABLE(Curl_conn_cf_get_socket(cf, data), timeout_ms);
      जर(what < 0) {
        /* fatal error */
        failf(data, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        result = CURLE_SEND_ERROR;
        थांबा;
      }
      अन्यथा जर(what == 0) {
        failf(data, "schannel: timed out sending data (bytes sent: %zu)",
              *pnwritten);
        result = CURLE_OPERATION_TIMEDOUT;
        थांबा;
      }
      /* socket is writable */

       result = Curl_conn_cf_send(cf->next, data,
                                  (स्थिरांक uint8_t *)ptr + *pnwritten,
                                  len - *pnwritten,
                                  FALSE, &this_write);
      जर(result == CURLE_AGAIN)
        सुरूठेवा;
      अन्यथा जर(result) {
        थांबा;
      }

      *pnwritten += this_write;
    }
  }
  अन्यथा जर(sspi_status == SEC_E_INSUFFICIENT_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
  }
  अन्यथा {
    result = CURLE_SEND_ERROR;
  }

  जर(len == *pnwritten)
    /* Encrypted message including header, data and trailer entirely sent.
       The return value is the number of unencrypted bytes that were sent. */
    *pnwritten = outbuf[1].cbBuffer;

  परत result;
}

स्थिर CURLcode schannel_recv(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                              अक्षर *buf, माप_प्रकार len, माप_प्रकार *pnread)
{
  माप_प्रकार size = 0;
  माप_प्रकार nread = 0;
  संरचना ssl_connect_data *connssl = cf->ctx;
  SecBuffer inbuf[4];
  SecBufferDesc inbuf_desc;
  SECURITY_STATUS sspi_status = SEC_E_OK;
  संरचना schannel_ssl_backend_data *backend =
    (संरचना schannel_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(backend);
  *pnread = 0;

  जर(backend->recv_renegotiating) {
    result = schannel_recv_renegotiate(cf, data, SCH_RENEG_CALLER_IS_RECV);
    जर(result)
      परत result;
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

  SCH_DEV(infof(data, "schannel: client wants to read %zu bytes", len));

  जर(len && len <= backend->decdata.offset) {
    SCH_DEV(infof(data,
                  "schannel: enough decrypted data is already available"));
    जा cleanup;
  }
  अन्यथा जर(backend->recv_unrecoverable_err) {
    result = backend->recv_unrecoverable_err;
    infof(data, "schannel: an unrecoverable error occurred in a prior call");
    जा cleanup;
  }
  अन्यथा जर(backend->recv_sspi_close_notify) {
    /* once a server has indicated shutdown there is no more encrypted data */
    infof(data, "schannel: server indicated shutdown in a prior call");
    जा cleanup;
  }
  /* it is debatable what to return when !len. Regardless we cannot return
     immediately because there may be data to decrypt (in the case we want to
     decrypt all encrypted cached data) so handle !len later in cleanup. */
  अन्यथा जर(len && !backend->recv_connection_closed) {
    /* the encrypted buffer must be large enough to hold all the bytes
       requested and some TLS record overhead. 'len' is a buffer size, so this
       integer math cannot overflow. */
    स्थिरांक माप_प्रकार min_encdata_length = len + CURL_SCHANNEL_BUFFER_FREE_SIZE;

    /* make sure encrypt buffer fits the requested amount of data */
    result = ensure_encoding_size(data, &backend->encdata, min_encdata_length);
    जर(result)
      जा cleanup;

    SCH_DEV(infof(data,
                  "schannel: encrypted data buffer: offset %zu length %zu",
                  backend->encdata.offset, backend->encdata.length));

    /* read encrypted data from socket */
    result = Curl_conn_cf_recv(cf->next, data,
                               (अक्षर *)(backend->encdata.buffer +
                                        backend->encdata.offset),
                               backend->encdata.length -
                               backend->encdata.offset,
                               &nread);
    जर(result) {
      जर(result == CURLE_AGAIN)
        SCH_DEV(infof(data, "schannel: recv returned CURLE_AGAIN"));
      अन्यथा {
        infof(data, "schannel: recv returned error %d", (पूर्णांक)result);
        backend->recv_unrecoverable_err = result;
      }
    }
    अन्यथा जर(nread == 0) {
      backend->recv_connection_closed = TRUE;
      DEBUGF(infof(data, "schannel: server closed the connection"));
    }
    अन्यथा {
      backend->encdata.offset += nread;
      backend->encdata_is_incomplete = FALSE;
      SCH_DEV(infof(data, "schannel: encrypted data got %zu", nread));
    }
  }

  SCH_DEV(infof(data, "schannel: encrypted data buffer: offset %zu length %zu",
                backend->encdata.offset, backend->encdata.length));

  /* decrypt loop */
  पर्यंत(backend->encdata.offset > 0 && sspi_status == SEC_E_OK &&
        (!len || backend->decdata.offset < len ||
         backend->recv_connection_closed)) {
    /* prepare data buffer for DecryptMessage call */
    InitSecBuffer(&inbuf[0], SECBUFFER_DATA, backend->encdata.buffer,
                  curlx_uztoul(backend->encdata.offset));

    /* we need 3 more empty input buffers for possible output */
    InitSecBuffer(&inbuf[1], SECBUFFER_EMPTY, रिक्त, 0);
    InitSecBuffer(&inbuf[2], SECBUFFER_EMPTY, रिक्त, 0);
    InitSecBuffer(&inbuf[3], SECBUFFER_EMPTY, रिक्त, 0);
    InitSecBufferDesc(&inbuf_desc, inbuf, 4);

    /* https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-decryptmessage
     */
    sspi_status = Curl_pSecFn->DecryptMessage(&backend->ctxt->ctxt_handle,
                                              &inbuf_desc, 0, रिक्त);

    /* check if everything went fine (server may want to renegotiate
       or shutdown the connection context) */
    जर(sspi_status == SEC_E_OK || sspi_status == SEC_I_RENEGOTIATE ||
       sspi_status == SEC_I_CONTEXT_EXPIRED) {
      /* check for successfully decrypted data, even before actual
         renegotiation or shutdown of the connection context */
      जर(inbuf[1].BufferType == SECBUFFER_DATA) {
        SCH_DEV(infof(data, "schannel: decrypted data length: %lu",
                      inbuf[1].cbBuffer));

        /* ensure the decode buffer fits the received amount of data */
        result = ensure_decoding_size(data, &backend->decdata,
                                      inbuf[1].cbBuffer, len);
        जर(result)
          जा cleanup;

        /* copy decrypted data to internal buffer */
        size = inbuf[1].cbBuffer;
        जर(size) {
          स_नकल(backend->decdata.buffer + backend->decdata.offset,
                 inbuf[1].pvBuffer, size);
          backend->decdata.offset += size;
        }

        SCH_DEV(infof(data, "schannel: decrypted data added: %zu", size));
        SCH_DEV(infof(data,
                      "schannel: decrypted cached: offset %zu length %zu",
                      backend->decdata.offset, backend->decdata.length));
      }

      /* check for remaining encrypted data */
      जर(inbuf[3].BufferType == SECBUFFER_EXTRA && inbuf[3].cbBuffer > 0) {
        SCH_DEV(infof(data, "schannel: encrypted data length: %lu",
                      inbuf[3].cbBuffer));

        /* check if the remaining data is less than the total amount
         * and therefore begins after the already processed data
         */
        जर(backend->encdata.offset > inbuf[3].cbBuffer) {
          /* move remaining encrypted data forward to the beginning of
             buffer */
          स_हटाओ(backend->encdata.buffer,
                  (backend->encdata.buffer + backend->encdata.offset) -
                  inbuf[3].cbBuffer, inbuf[3].cbBuffer);
          backend->encdata.offset = inbuf[3].cbBuffer;
        }

        SCH_DEV(infof(data,
                      "schannel: encrypted cached: offset %zu length %zu",
                      backend->encdata.offset, backend->encdata.length));
      }
      अन्यथा {
        /* reset encrypted buffer offset, because there is no data remaining */
        backend->encdata.offset = 0;
      }

      /* check if server wants to renegotiate the connection context */
      जर(sspi_status == SEC_I_RENEGOTIATE) {
        infof(data, "schannel: remote party requests renegotiation");
        जर(result && result != CURLE_AGAIN) {
          infof(data, "schannel: cannot renegotiate, an error is pending");
          जा cleanup;
        }

        backend->recv_renegotiating = TRUE;
        result = schannel_recv_renegotiate(cf, data, SCH_RENEG_CALLER_IS_RECV);
        जर(result)
          जा cleanup;

        /* now retry receiving data */
        sspi_status = SEC_E_OK;
        सुरूठेवा;
      }
      /* check if the server closed the connection */
      अन्यथा जर(sspi_status == SEC_I_CONTEXT_EXPIRED) {
        /* In Windows 2000 SEC_I_CONTEXT_EXPIRED (close_notify) is not
           returned so we have to work around that in cleanup. */
        backend->recv_sspi_close_notify = TRUE;
        जर(!backend->recv_connection_closed)
          backend->recv_connection_closed = TRUE;
        /* We received the close notify fine, any error we got
         * from the lower filters afterwards (e.g. the socket), is not
         * an error on the TLS data stream. That one ended here. */
        जर(result == CURLE_RECV_ERROR)
          result = CURLE_OK;
        infof(data,
              "schannel: server close notification received (close_notify)");
        जा cleanup;
      }
    }
    अन्यथा जर(sspi_status == SEC_E_INCOMPLETE_MESSAGE) {
      backend->encdata_is_incomplete = TRUE;
      जर(!result)
        result = CURLE_AGAIN;
      SCH_DEV(infof(data, "schannel: failed to decrypt data, need more data"));
      जा cleanup;
    }
    अन्यथा {
      अक्षर buffer[STRERROR_LEN];
      failf(data, "schannel: failed to read data from server: %s",
            Curl_sspi_strerror(sspi_status, buffer, आकार(buffer)));
      result = CURLE_RECV_ERROR;
      जा cleanup;
    }
  }

  SCH_DEV(infof(data, "schannel: encrypted data buffer: offset %zu length %zu",
                backend->encdata.offset, backend->encdata.length));

  SCH_DEV(infof(data, "schannel: decrypted data buffer: offset %zu length %zu",
                backend->decdata.offset, backend->decdata.length));

cleanup:
  /* Warning- there is no guarantee the encdata state is valid at this point */
  SCH_DEV(infof(data, "schannel: schannel_recv cleanup"));

  /* Error if the connection has closed without a close_notify.

     The behavior here is a matter of debate. We do not want to be vulnerable
     to a truncation attack however there is some browser precedent for
     ignoring the close_notify for compatibility reasons. */
  जर(len && !backend->decdata.offset && backend->recv_connection_closed &&
     !backend->recv_sspi_close_notify) {
    result = CURLE_RECV_ERROR;
    failf(data, "schannel: server closed abruptly (missing close_notify)");
  }

  /* Any error other than CURLE_AGAIN is an unrecoverable error. */
  जर(result && result != CURLE_AGAIN)
    backend->recv_unrecoverable_err = result;

  size = len < backend->decdata.offset ? len : backend->decdata.offset;
  जर(size) {
    स_नकल(buf, backend->decdata.buffer, size);
    स_हटाओ(backend->decdata.buffer, backend->decdata.buffer + size,
            backend->decdata.offset - size);
    backend->decdata.offset -= size;
    SCH_DEV(infof(data, "schannel: decrypted data returned %zu", size));
    SCH_DEV(infof(data,
                  "schannel: decrypted data buffer: offset %zu length %zu",
                  backend->decdata.offset, backend->decdata.length));
    *pnread = size;
    परत CURLE_OK;
  }

  जर(!result && !backend->recv_connection_closed)
    result = CURLE_AGAIN;

  /* it is debatable what to return when !len. We could return whatever error
     we got from decryption but instead we override here so the return is
     consistent. */
  जर(!len)
    परत CURLE_OK;

  परत result;
}

स्थिर bool schannel_data_pending(संरचना Curl_cfilter *cf,
                                  स्थिरांक संरचना Curl_easy *data)
{
  स्थिरांक संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना schannel_ssl_backend_data *backend =
    (संरचना schannel_ssl_backend_data *)connssl->backend;

  (शून्य)data;
  DEBUGASSERT(backend);

  जर(backend->ctxt) /* SSL/TLS is in use */
    परत backend->decdata.offset > 0 ||
           (backend->encdata.offset > 0 && !backend->encdata_is_incomplete) ||
           backend->recv_connection_closed ||
           backend->recv_sspi_close_notify ||
           backend->recv_unrecoverable_err;
  अन्यथा
    परत FALSE;
}

/* shut down the SSL connection and clean up related memory.
   this function can be called multiple times on the same connection including
   if the SSL connection failed (eg connection made but failed handshake). */
स्थिर CURLcode schannel_shutdown(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  bool send_shutdown, bool *done)
{
  /* See https://learn.microsoft.com/windows/win32/secauthn/shutting-down-an-schannel-connection
   * Shutting Down an Schannel Connection
   */
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना schannel_ssl_backend_data *backend =
    (संरचना schannel_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;

  जर(cf->shutdown) {
    *done = TRUE;
    परत CURLE_OK;
  }

  DEBUGASSERT(data);
  DEBUGASSERT(backend);

  /* Not supported in schannel */
  (शून्य)send_shutdown;

  *done = FALSE;
  जर(backend->ctxt) {
    infof(data, "schannel: shutting down SSL/TLS connection with %s port %d",
          connssl->peer.origin->hostname, connssl->peer.origin->port);
  }

  जर(!backend->ctxt || cf->shutdown) {
    *done = TRUE;
    जा out;
  }

  जर(backend->cred && backend->ctxt && !backend->sent_shutdown &&
     !backend->shutdown_generated) {
    SecBufferDesc BuffDesc;
    SecBuffer Buffer;
    SECURITY_STATUS sspi_status;
    SecBuffer outbuf;
    SecBufferDesc outbuf_desc;
    DWORD dwshut = SCHANNEL_SHUTDOWN;

    InitSecBuffer(&Buffer, SECBUFFER_TOKEN, &dwshut, आकार(dwshut));
    InitSecBufferDesc(&BuffDesc, &Buffer, 1);

    sspi_status = Curl_pSecFn->ApplyControlToken(&backend->ctxt->ctxt_handle,
                                                 &BuffDesc);

    जर(sspi_status != SEC_E_OK) {
      अक्षर buffer[STRERROR_LEN];
      failf(data, "schannel: ApplyControlToken failure: %s",
            Curl_sspi_strerror(sspi_status, buffer, आकार(buffer)));
      result = CURLE_SEND_ERROR;
      जा out;
    }

    /* setup output buffer */
    InitSecBuffer(&outbuf, SECBUFFER_EMPTY, रिक्त, 0);
    InitSecBufferDesc(&outbuf_desc, &outbuf, 1);

    sspi_status = Curl_pSecFn->InitializeSecurityContext(
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

    जर((sspi_status == SEC_E_OK) || (sspi_status == SEC_I_CONTEXT_EXPIRED)) {
      backend->shutdown_token = outbuf;
      backend->shutdown_generated = TRUE;
    }
    अन्यथा {
      अक्षर buffer[STRERROR_LEN];
      जर(outbuf.pvBuffer)
        Curl_pSecFn->FreeContextBuffer(outbuf.pvBuffer);
      failf(data, "schannel: generating close msg failed: %s",
            Curl_sspi_strerror(sspi_status, buffer, आकार(buffer)));
      result = CURLE_SEND_ERROR;
      जा out;
    }
  }

  जर(backend->shutdown_generated && !backend->sent_shutdown) {
    माप_प्रकार written = 0;
    माप_प्रकार remaining;

    DEBUGASSERT(backend->shutdown_offset <= backend->shutdown_token.cbBuffer);
    remaining = backend->shutdown_token.cbBuffer - backend->shutdown_offset;

    जर(remaining) {
      result = Curl_conn_cf_send(cf->next, data,
                                 ((uint8_t *)backend->shutdown_token.pvBuffer +
                                  backend->shutdown_offset),
                                 remaining, FALSE, &written);
      जर(!result)
        backend->shutdown_offset += written;
      जर(result == CURLE_AGAIN || (!result && written < remaining)) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        result = CURLE_OK;
        जा out;
      }
    }
    अन्यथा /* no shutdown message to send? */
      result = CURLE_OK;
    जर(result) {
      जर(backend->recv_connection_closed) {
        /* The server already closed, so sending close_notify is optional. */
        *done = TRUE;
        result = CURLE_OK;
      }
      अन्यथा {
        failf(data, "schannel: error sending close msg: %d", (पूर्णांक)result);
        result = CURLE_SEND_ERROR;
      }
      जा out;
    }
    जर(backend->shutdown_token.pvBuffer)
      Curl_pSecFn->FreeContextBuffer(backend->shutdown_token.pvBuffer);
    स_रखो(&backend->shutdown_token, 0, आकार(backend->shutdown_token));
    backend->shutdown_offset = 0;
    backend->sent_shutdown = TRUE;
    *done = TRUE;
  }

  /* If the connection seems open and we have not seen the close notify
   * from the server yet, try to receive it. */
  जर(backend->cred && backend->ctxt &&
     !backend->recv_sspi_close_notify && !backend->recv_connection_closed) {
    अक्षर buffer[1024];
    माप_प्रकार nread;

    result = schannel_recv(cf, data, buffer, आकार(buffer), &nread);
    जर(result == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
    }
    अन्यथा जर(result) {
      CURL_TRC_CF(data, cf, "SSL shutdown, error %d", (पूर्णांक)result);
      result = CURLE_RECV_ERROR;
    }
    अन्यथा जर(nread == 0) {
      /* We got the close notify alert and are done. */
      backend->recv_connection_closed = TRUE;
      *done = TRUE;
    }
    अन्यथा {
      /* still data coming in? */
    }
  }

out:
  cf->shutdown = (result || *done);
  परत result;
}

स्थिर शून्य schannel_close(संरचना Curl_cfilter *cf, संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना schannel_ssl_backend_data *backend =
    (संरचना schannel_ssl_backend_data *)connssl->backend;

  DEBUGASSERT(data);
  DEBUGASSERT(backend);

  जर(backend->shutdown_token.pvBuffer)
    Curl_pSecFn->FreeContextBuffer(backend->shutdown_token.pvBuffer);
  स_रखो(&backend->shutdown_token, 0, आकार(backend->shutdown_token));
  backend->shutdown_offset = 0;

  /* free SSPI Schannel API security context handle */
  जर(backend->ctxt) {
    जर(cf->conn->sslContext == &backend->ctxt->ctxt_handle)
      cf->conn->sslContext = रिक्त;

    DEBUGF(infof(data, "schannel: clear security context handle"));
    Curl_pSecFn->DeleteSecurityContext(&backend->ctxt->ctxt_handle);
    curlx_safefree(backend->ctxt);
  }

  /* free SSPI Schannel API credential handle */
  जर(backend->cred) {
    Curl_ssl_scache_lock(data);
    schannel_session_free(backend->cred);
    Curl_ssl_scache_unlock(data);
    backend->cred = रिक्त;
  }

  /* free the buffer used to encrypt outgoing data */
  curlx_safefree(backend->send_buffer);
  backend->send_buffer_len = 0;

  /* free internal buffer for received encrypted data */
  जर(backend->encdata.buffer) {
    curlx_safefree(backend->encdata.buffer);
    backend->encdata.length = 0;
    backend->encdata.offset = 0;
    backend->encdata_is_incomplete = FALSE;
  }

  /* free internal buffer for received decrypted data */
  जर(backend->decdata.buffer) {
    curlx_safefree(backend->decdata.buffer);
    backend->decdata.length = 0;
    backend->decdata.offset = 0;
  }
}

स्थिर पूर्णांक schannel_init(शून्य)
{
  प्रकारव्याख्या स्थिरांक अक्षर *(APIENTRY *WINE_GET_VERSION_FN)(शून्य);
#जर defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ढकला
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
  WINE_GET_VERSION_FN p_wine_get_version =
    CURLX_FUNCTION_CAST(WINE_GET_VERSION_FN,
      GetProcAddress(GetModuleHandle(TEXT("ntdll")), "wine_get_version"));
#जर defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic काढा
#endif
  जर(p_wine_get_version) {  /* WINE detected */
    curl_off_t ver = 0;
    स्थिरांक अक्षर *wine_version = p_wine_get_version();  /* e.g. "6.0.2" */
    /* Assume ALPN support with WINE 6.0 or greater */
    जर(wine_version)
      curlx_str_संख्या(&wine_version, &ver, 20);
    s_win_has_alpn = (ver >= 6);
  }
  अन्यथा {
    /* ALPN is supported on Windows 8.1 / Server 2012 R2 or newer. */
    s_win_has_alpn = curlx_verify_windows_version(6, 3, 0, PLATFORM_WINNT,
                                                  VERSION_GREATER_THAN_EQUAL);
  }

  परत Curl_sspi_global_init() == CURLE_OK ? 1 : 0;
}

स्थिर शून्य schannel_cleanup(शून्य)
{
  Curl_sspi_global_cleanup();
}

स्थिर माप_प्रकार schannel_version(अक्षर *buffer, माप_प्रकार size)
{
  परत curl_msnprintf(buffer, size, "Schannel");
}

स्थिर CURLcode schannel_random(संरचना Curl_easy *data,
                                अचिन्हित अक्षर *entropy, माप_प्रकार length)
{
  (शून्य)data;

  परत curlx_win32_random(entropy, length);
}

स्थिर CURLcode schannel_checksum(स्थिरांक अचिन्हित अक्षर *आदान,
                                  माप_प्रकार inputlen,
                                  अचिन्हित अक्षर *checksum,
                                  माप_प्रकार checksumlen,
                                  DWORD provType,
                                  स्थिरांक अचिन्हित पूर्णांक algId)
{
  CURLcode result = CURLE_FAILED_INIT;

  HCRYPTPROV hProv = 0;
  HCRYPTHASH hHash = 0;

  माप_प्रकार off;

  DWORD cbHashSize;
  DWORD dwHashSizeLen;
  DWORD dwChecksumLen;

  /* since this can fail in multiple ways, zero memory first so we never
   * return old data
   */
  स_रखो(checksum, 0, checksumlen);

  जर(!CryptAcquireContext(&hProv, रिक्त, रिक्त, provType,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    जा out;

  जर(!CryptCreateHash(hProv, algId, 0, 0, &hHash))
    जा out;

  result = CURLE_BAD_FUNCTION_ARGUMENT;

  off = 0;
  पर्यंत(off < inputlen) {
    DWORD chunk = (DWORD)CURLMIN(inputlen - off, 0xffffffffUL);
    जर(!CryptHashData(hHash, आदान + off, chunk, 0))
      जा out;
    off += chunk;
  }

  /* get hash size */
  cbHashSize = 0;
  dwHashSizeLen = (DWORD)आकार(cbHashSize);
  जर(!CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE *)&cbHashSize,
                        &dwHashSizeLen, 0))
    जा out;

  /* check if hash fits into the return buffer */
  जर(checksumlen < cbHashSize)
    जा out;

  dwChecksumLen = (DWORD)checksumlen;
  जर(CryptGetHashParam(hHash, HP_HASHVAL, checksum, &dwChecksumLen, 0) &&
     dwChecksumLen == cbHashSize)
    result = CURLE_OK;

out:
  जर(hHash)
    CryptDestroyHash(hHash);

  जर(hProv)
    CryptReleaseContext(hProv, 0);

  परत result;
}

स्थिर CURLcode schannel_sha256sum(स्थिरांक अचिन्हित अक्षर *आदान,
                                   माप_प्रकार inputlen,
                                   अचिन्हित अक्षर *sha256sum,
                                   माप_प्रकार sha256len)
{
  परत schannel_checksum(आदान, inputlen, sha256sum, sha256len,
                           PROV_RSA_AES, CALG_SHA_256);
}

स्थिर शून्य *schannel_get_internals(संरचना ssl_connect_data *connssl,
                                    CURLINFO info)
{
  संरचना schannel_ssl_backend_data *backend =
    (संरचना schannel_ssl_backend_data *)connssl->backend;
  (शून्य)info;
  DEBUGASSERT(backend);
  परत &backend->ctxt->ctxt_handle;
}

HCERTSTORE Curl_schannel_get_cached_cert_store(संरचना Curl_cfilter *cf,
                                               संरचना Curl_easy *data)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना Curl_multi *multi = data->multi;
  स्थिरांक संरचना curl_blob *ca_info_blob = conn_config->ca_info_blob;
  संरचना schannel_cert_share *share;
  timediff_t timeout_ms;
  अचिन्हित अक्षर info_blob_digest[CURL_SHA256_DIGEST_LENGTH];

  DEBUGASSERT(multi);

  जर(!multi) {
    परत रिक्त;
  }

  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_SCHANNEL_CERT_SHARE_किल्ली,
                         CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_किल्ली));
  जर(!share || !share->cert_store) {
    परत रिक्त;
  }

  /* zero ca_cache_timeout completely disables caching */
  जर(!data->set.ssl_ca_cache_timeout) {
    परत रिक्त;
  }

  /* check for cache timeout by using the cached_x509_store_expired timediff
     calculation pattern from openssl.c.
     negative timeout means retain forever. */
  timeout_ms = data->set.ssl_ca_cache_timeout * (timediff_t)1000;
  जर(timeout_ms >= 0) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(data), &share->समय);
    जर(elapsed_ms >= timeout_ms) {
      परत रिक्त;
    }
  }

  जर(ca_info_blob) {
    जर(share->CAinfo_blob_size != ca_info_blob->len) {
      परत रिक्त;
    }
    जर(schannel_sha256sum((स्थिरांक अचिन्हित अक्षर *)ca_info_blob->data,
                          ca_info_blob->len,
                          info_blob_digest,
                          CURL_SHA256_DIGEST_LENGTH))
      परत रिक्त;
    जर(स_भेद(share->CAinfo_blob_digest, info_blob_digest,
              CURL_SHA256_DIGEST_LENGTH)) {
      परत रिक्त;
    }
  }
  अन्यथा {
    जर(!conn_config->CAfile || !share->CAfile ||
       म_भेद(share->CAfile, conn_config->CAfile)) {
      परत रिक्त;
    }
  }

  परत share->cert_store;
}

स्थिर शून्य schannel_cert_share_free(स्थिरांक शून्य *किल्ली, माप_प्रकार किल्ली_len, शून्य *p)
{
  संरचना schannel_cert_share *share = p;
  DEBUGASSERT(किल्ली_len == CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_किल्ली));
  DEBUGASSERT(!स_भेद(MPROTO_SCHANNEL_CERT_SHARE_किल्ली, किल्ली, किल्ली_len));
  (शून्य)किल्ली;
  (शून्य)किल्ली_len;
  जर(share->cert_store) {
    CertCloseStore(share->cert_store, 0);
  }
  curlx_free(share->CAfile);
  curlx_free(share);
}

bool Curl_schannel_set_cached_cert_store(संरचना Curl_cfilter *cf,
                                         संरचना Curl_easy *data,
                                         HCERTSTORE cert_store)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना Curl_multi *multi = data->multi;
  स्थिरांक संरचना curl_blob *ca_info_blob = conn_config->ca_info_blob;
  संरचना schannel_cert_share *share;
  अचिन्हित अक्षर digest[CURL_SHA256_DIGEST_LENGTH];
  अक्षर *CAfile = रिक्त;

  DEBUGASSERT(multi);

  जर(!multi) {
    परत FALSE;
  }

  जर(ca_info_blob) {
    जर(schannel_sha256sum((स्थिरांक अचिन्हित अक्षर *)ca_info_blob->data,
                          ca_info_blob->len, digest, आकार(digest))) {
      परत FALSE;
    }
  }
  अन्यथा जर(conn_config->CAfile) {
    CAfile = curlx_strdup(conn_config->CAfile);
    जर(!CAfile) {
      परत FALSE;
    }
  }

  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_SCHANNEL_CERT_SHARE_किल्ली,
                         CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_किल्ली));
  जर(!share) {
    share = curlx_calloc(1, आकार(*share));
    जर(!share) {
      curlx_free(CAfile);
      परत FALSE;
    }
    जर(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_SCHANNEL_CERT_SHARE_किल्ली,
                       CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_किल्ली),
                       share, schannel_cert_share_free)) {
      curlx_free(share);
      curlx_free(CAfile);
      परत FALSE;
    }
  }

  /* free old cache data */
  जर(share->cert_store) {
    CertCloseStore(share->cert_store, 0);
  }
  curlx_free(share->CAfile);

  जर(ca_info_blob) {
    स_नकल(share->CAinfo_blob_digest, digest, आकार(digest));
    share->CAinfo_blob_size = ca_info_blob->len;
  }
  अन्यथा
    share->CAinfo_blob_size = 0;

  share->समय = curlx_now();
  share->cert_store = cert_store;
  share->CAfile = CAfile;
  परत TRUE;
}

स्थिरांक संरचना Curl_ssl Curl_ssl_schannel = {
  { CURLSSLBACKEND_SCHANNEL, "schannel" }, /* info */

  SSLSUPP_CERTINFO |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_CA_CACHE |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST,

  आकार(संरचना schannel_ssl_backend_data),

  schannel_init,                     /* init */
  schannel_cleanup,                  /* cleanup */
  schannel_version,                  /* version */
  schannel_shutdown,                 /* shutdown */
  schannel_data_pending,             /* data_pending */
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

#endif /* USE_SCHANNEL */
