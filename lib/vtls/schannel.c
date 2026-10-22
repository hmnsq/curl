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
#समावेश "curl_setup.h"

#अगर_घोषित USE_SCHANNEL

#अगर_अघोषित USE_WINDOWS_SSPI
#त्रुटि "cannot compile Schannel support without SSPI."
#पूर्ण_अगर

#समावेश "vtls/schannel.h"
#समावेश "vtls/schannel_int.h"
#समावेश "vtls/vtls.h"
#समावेश "vtls/vtls_int.h"
#समावेश "vtls/vtls_scache.h"
#समावेश "curl_trc.h"
#समावेश "connect.h" /* for the connect timeout */
#समावेश "curlx/strdup.h"
#समावेश "strerror.h"
#समावेश "select.h" /* for the socket readiness */
#समावेश "curlx/win32-fopen.h"
#समावेश "curlx/multibyte.h"
#समावेश "vtls/x509asn1.h"
#समावेश "curlx/version_win32.h"
#समावेश "curlx/winapi.h"
#समावेश "curlx/strparse.h"
#समावेश "progress.h"
#समावेश "curl_sha256.h"

/* Some verbose debug messages are wrapped by SCH_DEV() instead of DEBUGF()
 * and only shown if CURL_SCHANNEL_DEV_DEBUG was defined at build time. These
 * messages are extra verbose and intended for curl developers debugging
 * Schannel recv decryption and renegotiation.
 */
#अगर_घोषित CURL_SCHANNEL_DEV_DEBUG
#घोषणा SCH_DEV(x) x
#घोषणा SCH_DEV_SHOWBOOL(x)                                     \
  करो {                                                          \
    infof(तथ्य, "schannel: " #x " %s", (x) ? "TRUE" : "FALSE"); \
  } जबतक(0)
#अन्यथा
#घोषणा SCH_DEV(x) करो {} जबतक(0)
#घोषणा SCH_DEV_SHOWBOOL(x) करो {} जबतक(0)
#पूर्ण_अगर

/* Offered by mingw-w64 v8+, MS SDK 7.0A/VS2010+ */
#अगर_अघोषित SP_PROT_TLS1_0_CLIENT
#घोषणा SP_PROT_TLS1_0_CLIENT           SP_PROT_TLS1_CLIENT
#पूर्ण_अगर
#अगर_अघोषित SP_PROT_TLS1_1_CLIENT
#घोषणा SP_PROT_TLS1_1_CLIENT           0x00000200
#पूर्ण_अगर
#अगर_अघोषित SP_PROT_TLS1_2_CLIENT
#घोषणा SP_PROT_TLS1_2_CLIENT           0x00000800
#पूर्ण_अगर

/* Offered by mingw-w64 v8+, MS SDK 10.0.15063.0/VS2017 15.1+ */
#अगर_अघोषित SP_PROT_TLS1_3_CLIENT
#घोषणा SP_PROT_TLS1_3_CLIENT           0x00002000
#पूर्ण_अगर
/* Offered by mingw-w64 v8+, MS SDK 8.1/VS2013+ */
#अगर_अघोषित SCH_USE_STRONG_CRYPTO
#घोषणा SCH_USE_STRONG_CRYPTO           0x00400000
#पूर्ण_अगर

/* Offered by mingw-w64 v10+, MS SDK 7.0A/VS2010+ */
#अगर_अघोषित SECBUFFER_ALERT
#घोषणा SECBUFFER_ALERT                 17
#पूर्ण_अगर

/* Both schannel buffer sizes must be > 0 */
#घोषणा CURL_SCHANNEL_बफर_INIT_आकार   4096
#घोषणा CURL_SCHANNEL_बफर_FREE_आकार   1024

#घोषणा CERT_THUMBPRINT_STR_LEN 40
#घोषणा CERT_THUMBPRINT_तथ्य_LEN 20

/* Uncomment to force verbose output
 * #define infof(x, y, ...) curl_mprintf(y, __VA_ARGS__)
 * #define failf(x, y, ...) curl_mprintf(y, __VA_ARGS__)
 */

/* Offered by mingw-w64 v4+, MS SDK 6.0A/VS2008+ */
#अगर_अघोषित PKCS12_NO_PERSIST_कुंजी
#घोषणा PKCS12_NO_PERSIST_कुंजी 0x00008000
#पूर्ण_अगर

/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#अगर_अघोषित CERT_FIND_HAS_PRIVATE_कुंजी
#घोषणा CERT_FIND_HAS_PRIVATE_कुंजी (21 << CERT_COMPARE_SHIFT)
#पूर्ण_अगर

/* key to use at `multi->proto_hash` */
#घोषणा MPROTO_SCHANNEL_CERT_SHARE_कुंजी   "tls:schannel:cert:share"

अटल bool s_win_has_alpn;

/* Offered by mingw-w64 v9+, MS SDK 8.1/VS2013+ */
#अगर_अघोषित SECBUFFER_APPLICATION_PROTOCOLS
#घोषणा SECBUFFER_APPLICATION_PROTOCOLS 18
#घोषणा SECPKG_ATTR_APPLICATION_PROTOCOL 35

प्रकार क्रमागत {
  SecApplicationProtocolNegotiationExt_None,
  SecApplicationProtocolNegotiationExt_NPN,
  SecApplicationProtocolNegotiationExt_ALPN
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT;

प्रकार क्रमागत {
  SecApplicationProtocolNegotiationStatus_None,
  SecApplicationProtocolNegotiationStatus_Success,
  SecApplicationProtocolNegotiationStatus_SelectedClientOnly
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_STATUS;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
प्रकार काष्ठा {
  SEC_APPLICATION_PROTOCOL_NEGOTIATION_STATUS ProtoNegoStatus;
  SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT ProtoNegoExt;
  अचिन्हित अक्षर Protocol_Id_आकार;
  अचिन्हित अक्षर ProtocolId[0xff];
} SecPkgContext_ApplicationProtocol;
#पूर्ण_अगर

अटल व्योम Init_Sec_बफर(Sec_बफर *बफर, अचिन्हित दीर्घ BufType,
                          व्योम *Buf_तथ्य_Ptr, अचिन्हित दीर्घ Buf_Byte_आकार)
{
  बफर->cb_बफर = Buf_Byte_आकार;
  बफर->बफर_Type = BufType;
  बफर->pv_बफर = Buf_तथ्य_Ptr;
}

अटल व्योम Init_Sec_बफर_Desc(Sec_बफर_Desc *desc, Sec_बफर *BufArr,
                              अचिन्हित दीर्घ अंक_Arr_Elem)
{
  desc->ulVersion = SECBUFFER_VERSION;
  desc->pBuffers = BufArr;
  desc->cBuffers = अंक_Arr_Elem;
}

अटल CURLcode schannel_set_ssl_version_min_max(DWORD *enabled_protocols,
                                                 काष्ठा Curl_cfilter *cf,
                                                 काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  दीर्घ ssl_version = conn_config->version;
  दीर्घ ssl_version_max = (दीर्घ)conn_config->version_max;
  दीर्घ i = ssl_version;

  चयन(ssl_version_max) {
  हाल CURL_SSLVERSION_MAX_NONE:
  हाल CURL_SSLVERSION_MAX_DEFAULT:

    /* Windows Server 2022 and newer (including Windows 11) support TLS 1.3
       built-in. Previous builds of Windows 10 had broken TLS 1.3
       implementations that could be enabled via registry. */
    अगर(curlx_verify_windows_version(10, 0, 20348, PLATFORM_WINNT,
                                    VERSION_GREATER_THAN_EQUAL)) {
      ssl_version_max = CURL_SSLVERSION_MAX_TLSv1_3;
    }
    अन्यथा /* Windows 10 or older */
      ssl_version_max = CURL_SSLVERSION_MAX_TLSv1_2;

    अवरोध;
  }

  क्रम(; i <= (ssl_version_max >> 16); ++i) {
    चयन(i) {
    हाल CURL_SSLVERSION_TLSv1_0:
      *enabled_protocols |= SP_PROT_TLS1_0_CLIENT;
      अवरोध;
    हाल CURL_SSLVERSION_TLSv1_1:
      *enabled_protocols |= SP_PROT_TLS1_1_CLIENT;
      अवरोध;
    हाल CURL_SSLVERSION_TLSv1_2:
      *enabled_protocols |= SP_PROT_TLS1_2_CLIENT;
      अवरोध;
    हाल CURL_SSLVERSION_TLSv1_3:

      /* Windows Server 2022 or newer */
      अगर(curlx_verify_windows_version(10, 0, 20348, PLATFORM_WINNT,
                                      VERSION_GREATER_THAN_EQUAL)) {
        *enabled_protocols |= SP_PROT_TLS1_3_CLIENT;
        अवरोध;
      }
      अन्यथा { /* Windows 10 or older */
        failf(तथ्य, "schannel: TLS 1.3 not supported on Windows 10 or older");
        वापस CURLE_SSL_CONNECT_ERROR;
      }
    }
  }
  वापस CURLE_OK;
}

#घोषणा CIPHEROPTION(x) { #x, x }

काष्ठा algo {
  स्थिर अक्षर *name;
  पूर्णांक id;
};

अटल स्थिर काष्ठा algo algs[] = {
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
  CIPHEROPTION(CALG_SCHANNEL_MAC_कुंजी),
  CIPHEROPTION(CALG_SCHANNEL_ENC_कुंजी),
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
#अगर_घोषित CALG_ECMQV
  CIPHEROPTION(CALG_ECMQV),
#पूर्ण_अगर
  CIPHEROPTION(CALG_ECDSA),
/* Offered by mingw-w64 v7+, MS SDK 7.0A/VS2010+ */
#अगर_घोषित CALG_ECDH_EPHEM
  CIPHEROPTION(CALG_ECDH_EPHEM),
#पूर्ण_अगर
  { शून्य, 0 },
};

अटल पूर्णांक get_alg_id_by_name(स्थिर अक्षर *name)
{
  स्थिर अक्षर *nameEnd = म_अक्षर(name, ':');
  माप_प्रकार n = nameEnd ? (माप_प्रकार)(nameEnd - name) : म_माप(name);
  पूर्णांक i;

  क्रम(i = 0; algs[i].name; i++) {
    अगर((n == म_माप(algs[i].name) && !म_भेदन(algs[i].name, name, n)))
      वापस algs[i].id;
  }
  वापस 0; /* not found */
}

#घोषणा अंक_CIPHERS 47 /* There are a maximum of 47 options listed above */

अटल CURLcode set_ssl_ciphers(SCHANNEL_CRED *schannel_cred, अक्षर *ciphers,
                                ALG_ID *algIds)
{
  स्थिर अक्षर *startCur = ciphers;
  पूर्णांक alg_गिनती = 0;
  जबतक(startCur && *startCur && (alg_गिनती < अंक_CIPHERS)) {
    curl_off_t alg;
    अगर(curlx_str_संख्या(&startCur, &alg, पूर्णांक_उच्च) || !alg)
      alg = get_alg_id_by_name(startCur);

    अगर(alg)
      algIds[alg_गिनती++] = (ALG_ID)alg;
    अन्यथा अगर(!म_भेदन(startCur, "USE_STRONG_CRYPTO",
                     CURL_CSTRLEN("USE_STRONG_CRYPTO")) ||
            !म_भेदन(startCur, "SCH_USE_STRONG_CRYPTO",
                     CURL_CSTRLEN("SCH_USE_STRONG_CRYPTO")))
      schannel_cred->dwFlags |= SCH_USE_STRONG_CRYPTO;
    अन्यथा
      वापस CURLE_SSL_CIPHER;
    startCur = म_अक्षर(startCur, ':');
    अगर(startCur)
      startCur++;
  }
  schannel_cred->palgSupportedAlgs = algIds;
  schannel_cred->cSupportedAlgs = (DWORD)alg_गिनती;
  वापस CURLE_OK;
}

/* Function allocates memory for store_path only if CURLE_OK is returned */
अटल CURLcode get_cert_location(TCHAR *path, DWORD *store_name,
                                  TCHAR **store_path, TCHAR **thumbprint)
{
  TCHAR *sep;
  TCHAR *store_path_start;
  माप_प्रकार store_name_len;

  sep = _tcschr(path, _TEXT('\\'));
  अगर(!sep)
    वापस CURLE_SSL_CERTPROBLEM;

  store_name_len = sep - path;

  अगर(_tcsncmp(path, _TEXT("CurrentUser"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_USER;
  अन्यथा अगर(_tcsncmp(path, _TEXT("LocalMachine"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE;
  अन्यथा अगर(_tcsncmp(path, _TEXT("CurrentService"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_SERVICE;
  अन्यथा अगर(_tcsncmp(path, _TEXT("Services"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_SERVICES;
  अन्यथा अगर(_tcsncmp(path, _TEXT("Users"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_USERS;
  अन्यथा अगर(_tcsncmp(path, _TEXT("CurrentUserGroupPolicy"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_USER_GROUP_POLICY;
  अन्यथा अगर(_tcsncmp(path, _TEXT("LocalMachineGroupPolicy"), store_name_len) ==
          0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE_GROUP_POLICY;
  अन्यथा अगर(_tcsncmp(path, _TEXT("LocalMachineEnterprise"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE_ENTERPRISE;
  अन्यथा
    वापस CURLE_SSL_CERTPROBLEM;

  store_path_start = sep + 1;

  sep = _tcschr(store_path_start, _TEXT('\\'));
  अगर(!sep)
    वापस CURLE_SSL_CERTPROBLEM;

  *thumbprint = sep + 1;
  अगर(_tcslen(*thumbprint) != CERT_THUMBPRINT_STR_LEN)
    वापस CURLE_SSL_CERTPROBLEM;

  *sep = _TEXT('\0');
  *store_path = curlx_tcsdup(store_path_start);
  *sep = _TEXT('\\');
  अगर(!*store_path)
    वापस CURLE_OUT_OF_MEMORY;

  वापस CURLE_OK;
}

अटल CURLcode get_client_cert(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य,
                                HCERTSTORE *out_cert_store,
                                PCCERT_CONTEXT *out_cert_context)
{
  काष्ठा ssl_filter_config *sslc = Curl_ssl_cf_get_filter_config(cf);
  PCCERT_CONTEXT client_cert = शून्य;
  HCERTSTORE client_cert_store = शून्य;
  CURLcode परिणाम = CURLE_OK;

  अगर(sslc->clientcert || sslc->cert_blob) {
    DWORD cert_store_name = 0;
    TCHAR *cert_store_path = शून्य;
    TCHAR *cert_thumbprint_str = शून्य;
    TCHAR cert_thumbprint_buf[CERT_THUMBPRINT_STR_LEN + 1];
    CRYPT_HASH_BLOB cert_thumbprint;
    BYTE cert_thumbprint_तथ्य[CERT_THUMBPRINT_तथ्य_LEN];
    HCERTSTORE cert_store = शून्य;
    खाता *fInCert = शून्य;
    व्योम *certdata = शून्य;
    माप_प्रकार certsize = 0;
    bool blob = !!sslc->cert_blob;

    अगर(blob) {
      certdata = sslc->cert_blob->तथ्य;
      certsize = sslc->cert_blob->len;
    }
    अन्यथा {
      TCHAR *cert_path =
        curlx_convert_UTF8_to_tchar(sslc->clientcert);
      अगर(!cert_path)
        वापस CURLE_OUT_OF_MEMORY;

      परिणाम = get_cert_location(cert_path, &cert_store_name,
                                 &cert_store_path, &cert_thumbprint_str);

      /* 'cert_thumbprint_str' points in to the allocated 'cert_path', copy
         the data. The string is verified to be CERT_THUMBPRINT_STR_LEN bytes
         long within the get_cert_location() function. */
      अगर(!परिणाम && cert_thumbprint_str) {
        स_नकल(cert_thumbprint_buf, cert_thumbprint_str,
               माप(cert_thumbprint_buf));
        cert_thumbprint_str = cert_thumbprint_buf;
      }

      curlx_free(cert_path);
      अगर(परिणाम && (sslc->clientcert[0] != '\0'))
        fInCert = curlx_fopen(sslc->clientcert, "rb");

      अगर(परिणाम && !fInCert) {
        failf(तथ्य, "schannel: Failed to get certificate location"
              " or file for %s",
              sslc->clientcert);
        वापस परिणाम;
      }
    }

    अगर((fInCert || blob) && sslc->cert_type &&
       !curl_strequal(sslc->cert_type, "P12")) {
      failf(तथ्य, "schannel: certificate format compatibility error "
            "for %s",
            blob ? "(memory blob)" : sslc->clientcert);
      curlx_free(cert_store_path);
      अगर(fInCert)
        curlx_fclose(fInCert);
      वापस CURLE_SSL_CERTPROBLEM;
    }

    अगर(fInCert || blob) {
      /* Reading a .p12 or .pfx file, like the example at bottom of
         https://learn.microsoft.com/archive/msdn-technet-forums/3e7bc95f-b21a-4bcd-bd2c-7f996718cae5
       */
      CRYPT_तथ्य_BLOB datablob;
      WCHAR *pszPassword;
      माप_प्रकार pwd_len = 0;
      पूर्णांक cert_find_flags;
      स्थिर अक्षर *cert_showfilename_error = blob ?
        "(memory blob)" : sslc->clientcert;
      curlx_free(cert_store_path);
      अगर(fInCert) {
        दीर्घ cert_tell = 0;
        bool continue_reading = ख_जाओ(fInCert, 0, अंत_से) == 0;
        अगर(continue_reading) {
          cert_tell = ख_बताओ(fInCert);
          अगर(cert_tell < 0)
            continue_reading = FALSE;
          अन्यथा
            certsize = (माप_प्रकार)cert_tell;
        }
        अगर(continue_reading)
          continue_reading = ख_जाओ(fInCert, 0, शुरू_से) == 0;
        अगर(continue_reading && (certsize < CURL_MAX_निवेश_लंबाई))
          certdata = curlx_malloc(certsize + 1);
        अगर(!certdata || ((पूर्णांक)ख_पढ़ो(certdata, certsize, 1, fInCert) != 1))
          continue_reading = FALSE;
        curlx_fclose(fInCert);
        अगर(!continue_reading) {
          failf(तथ्य, "schannel: Failed to read cert file %s",
                sslc->clientcert);
          curlx_free(certdata);
          वापस CURLE_SSL_CERTPROBLEM;
        }
      }

      /* Convert key-pair data to the in-memory certificate store */
      datablob.pb_तथ्य = (BYTE *)certdata;
      datablob.cb_तथ्य = (DWORD)certsize;

      अगर(sslc->कुंजी_passwd)
        pwd_len = म_माप(sslc->कुंजी_passwd);
      pszPassword = curlx_malloc(माप(WCHAR) * (pwd_len + 1));
      अगर(pszPassword) {
        पूर्णांक str_w_len = 0;
        अगर(pwd_len > 0)
          str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                          sslc->कुंजी_passwd, (पूर्णांक)pwd_len,
                                          pszPassword, (पूर्णांक)(pwd_len + 1));

        अगर((str_w_len >= 0) && (str_w_len <= (पूर्णांक)pwd_len))
          pszPassword[str_w_len] = 0;
        अन्यथा
          pszPassword[0] = 0;

        cert_store = PFXImportCertStore(&datablob, pszPassword,
                                        PKCS12_NO_PERSIST_कुंजी);
        curlx_memzero(pszPassword, माप(WCHAR) * (pwd_len + 1));
        curlx_free(pszPassword);
      }
      अगर(!blob)
        curlx_free(certdata);
      अगर(!cert_store) {
        DWORD errorcode = GetLastError();
        अगर(errorcode == ERROR_INVALID_PASSWORD)
          failf(तथ्य, "schannel: Failed to import cert file %s, "
                "password is bad",
                cert_showfilename_error);
        अन्यथा
          failf(तथ्य, "schannel: Failed to import cert file %s, "
                "last error is 0x%08lx",
                cert_showfilename_error, errorcode);
        वापस CURLE_SSL_CERTPROBLEM;
      }

      /* CERT_FIND_HAS_PRIVATE_KEY is only available in Windows 8 / Server
         2012, (NT 6.2). For older versions we use CURL_FIND_ANY. */
      अगर(curlx_verify_windows_version(6, 2, 0, PLATFORM_WINNT,
                                      VERSION_GREATER_THAN_EQUAL))
        cert_find_flags = CERT_FIND_HAS_PRIVATE_कुंजी;
      अन्यथा
        cert_find_flags = CERT_FIND_ANY;

      client_cert =
        CertFindCertificateInStore(cert_store,
                                   X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                   cert_find_flags, शून्य, शून्य);

      अगर(!client_cert) {
        failf(तथ्य, "schannel: Failed to get certificate from file %s"
              ", last error is 0x%08lx",
              cert_showfilename_error, GetLastError());
        CertCloseStore(cert_store, 0);
        वापस CURLE_SSL_CERTPROBLEM;
      }
    }
    अन्यथा {
      cert_store =
        CertOpenStore(
#अगर_घोषित UNICODE
                      CERT_STORE_PROV_SYSTEM_W,
#अन्यथा
                      CERT_STORE_PROV_SYSTEM_A,
#पूर्ण_अगर
                      0,
                      (HCRYPTPROV)शून्य,
                      CERT_STORE_OPEN_EXISTING_FLAG | cert_store_name,
                      cert_store_path);
      अगर(!cert_store) {
        अक्षर *path_utf8 =
          curlx_convert_tchar_to_UTF8(cert_store_path);
        failf(तथ्य, "schannel: Failed to open cert store %lx %s, "
              "last error is 0x%08lx",
              cert_store_name, (path_utf8 ? path_utf8 : "(unknown)"),
              GetLastError());
        curlx_free(cert_store_path);
        curlx_free(path_utf8);
        वापस CURLE_SSL_CERTPROBLEM;
      }
      curlx_free(cert_store_path);

      cert_thumbprint.pb_तथ्य = cert_thumbprint_तथ्य;
      cert_thumbprint.cb_तथ्य = CERT_THUMBPRINT_तथ्य_LEN;

      अगर(!Crypt_String_To_द्विआधारी(cert_thumbprint_str,
                              CERT_THUMBPRINT_STR_LEN,
                              CRYPT_STRING_HEX,
                              cert_thumbprint_तथ्य,
                              &cert_thumbprint.cb_तथ्य,
                              शून्य, शून्य)) {
        CertCloseStore(cert_store, 0);
        वापस CURLE_SSL_CERTPROBLEM;
      }

      client_cert =
        CertFindCertificateInStore(cert_store,
                                   X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                   CERT_FIND_HASH, &cert_thumbprint, शून्य);
      अगर(!client_cert) {
        /* CRYPT_E_NOT_FOUND / E_INVALIDARG */
        CertCloseStore(cert_store, 0);
        failf(तथ्य, "schannel: client cert not found in cert store");
        वापस CURLE_SSL_CERTPROBLEM;
      }
    }
    client_cert_store = cert_store;
  }

  *out_cert_store = client_cert_store;
  *out_cert_context = client_cert;

  वापस CURLE_OK;
}

अटल CURLcode acquire_sspi_handle(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य,
                                    काष्ठा schannel_ssl_backend_तथ्य *backend,
                                    PCCERT_CONTEXT client_cert,
                                    DWORD flags,
                                    DWORD enabled_protocols)
{
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CURLcode परिणाम;

  /* We support TLS 1.3 starting in Windows Server 2022 or later
   * (OS build 20348) */
  अगर(!conn_config->cipher_list &&
     curlx_verify_windows_version(10, 0, 20348, PLATFORM_WINNT,
                                  VERSION_GREATER_THAN_EQUAL)) {

    SCH_CREDENTIALS credentials = { 0 };
    TLS_PARAMETERS tls_parameters = { 0 };
    CRYPTO_SETTINGS crypto_settings[1];
    PCCERT_CONTEXT client_certs[1];

    अगर(client_cert)
      client_certs[0] = client_cert;
    अन्यथा
      client_certs[0] = शून्य;

    स_रखो(crypto_settings, 0, माप(crypto_settings));

    tls_parameters.pDisabledCrypto = crypto_settings;

    /* The number of blocked suites */
    tls_parameters.cDisabledCrypto = (DWORD)0;
    credentials.pTlsParameters = &tls_parameters;
    credentials.cTlsParameters = 1;

    credentials.dwVersion = SCH_CREDENTIALS_VERSION;
    credentials.dwFlags = flags | SCH_USE_STRONG_CRYPTO;

    credentials.pTlsParameters->grbitDisabledProtocols = ~enabled_protocols;

    अगर(client_certs[0]) {
      credentials.cCreds = 1;
      credentials.paCred = client_certs;
    }

    sspi_status =
      Curl_pSecFn->AcquireCredentialsHandle(शून्य, CURL_UNCONST(UNISP_NAME),
                                            SECPKG_CRED_OUTBOUND, शून्य,
                                            &credentials, शून्य, शून्य,
                                            &backend->cred->cred_handle, शून्य);
  }
  अन्यथा {
    /* Pre-Windows 10 1809 or the user set a legacy algorithm list.
       Schannel does not negotiate TLS 1.3 when SCHANNEL_CRED is used. */
    ALG_ID algIds[अंक_CIPHERS];
    अक्षर *ciphers = conn_config->cipher_list;
    SCHANNEL_CRED schannel_cred = { 0 };
    PCCERT_CONTEXT client_certs[1];

    अगर(client_cert)
      client_certs[0] = client_cert;
    अन्यथा
      client_certs[0] = शून्य;

    schannel_cred.dwVersion = SCHANNEL_CRED_VERSION;
    schannel_cred.dwFlags = flags;
    schannel_cred.grbitEnabledProtocols = enabled_protocols;

    अगर(ciphers) {
      अगर((enabled_protocols & SP_PROT_TLS1_3_CLIENT)) {
        अगर(!(enabled_protocols & ~SP_PROT_TLS1_3_CLIENT)) {
          failf(तथ्य, "schannel: TLS 1.3 is not supported with a cipher list; "
                "remove the cipher list or allow a lower TLS version");
          वापस CURLE_SSL_CONNECT_ERROR;
        }
        infof(तथ्य, "schannel: WARNING: This version of Schannel "
              "negotiates a less-secure TLS version than TLS 1.3 because the "
              "user set an algorithm cipher list.");
      }
      परिणाम = set_ssl_ciphers(&schannel_cred, ciphers, algIds);
      अगर(परिणाम) {
        failf(तथ्य, "schannel: Failed setting algorithm cipher list");
        वापस परिणाम;
      }
    }
    अन्यथा {
      schannel_cred.dwFlags = flags | SCH_USE_STRONG_CRYPTO;
    }

    अगर(client_certs[0]) {
      schannel_cred.cCreds = 1;
      schannel_cred.paCred = client_certs;
    }

    sspi_status =
      Curl_pSecFn->AcquireCredentialsHandle(शून्य, CURL_UNCONST(UNISP_NAME),
                                            SECPKG_CRED_OUTBOUND, शून्य,
                                            &schannel_cred, शून्य, शून्य,
                                            &backend->cred->cred_handle, शून्य);
  }

  अगर(sspi_status != SEC_E_OK) {
    अक्षर बफर[STRERROR_LEN];
    failf(तथ्य, "schannel: AcquireCredentialsHandle failed: %s",
          Curl_sspi_strerror(sspi_status, बफर, माप(बफर)));
    चयन(sspi_status) {
    हाल SEC_E_INSUFFICIENT_MEMORY:
      वापस CURLE_OUT_OF_MEMORY;
    हाल SEC_E_NO_CREDENTIALS:
    हाल SEC_E_SECPKG_NOT_FOUND:
    हाल SEC_E_NOT_OWNER:
    हाल SEC_E_UNKNOWN_CREDENTIALS:
    हाल SEC_E_INTERNAL_ERROR:
    शेष:
      वापस CURLE_SSL_CONNECT_ERROR;
    }
  }

  वापस CURLE_OK;
}

अटल CURLcode schannel_acquire_credential_handle(काष्ठा Curl_cfilter *cf,
                                                   काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  PCCERT_CONTEXT client_cert = शून्य;
  HCERTSTORE client_cert_store = शून्य;
  CURLcode परिणाम;

  /* setup Schannel API options */
  DWORD flags = 0;
  DWORD enabled_protocols = 0;

  काष्ठा schannel_ssl_backend_तथ्य *backend =
    (काष्ठा schannel_ssl_backend_तथ्य *)connssl->backend;

  DEBUGASSERT(backend);

  अगर(conn_config->verifypeer) {
    अगर(backend->use_manual_cred_validation)
      flags = SCH_CRED_MANUAL_CRED_VALIDATION;
    अन्यथा
      flags = SCH_CRED_AUTO_CRED_VALIDATION;

    अगर(conn_config->no_revoke) {
      flags |= SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
               SCH_CRED_IGNORE_REVOCATION_OFFLINE;

      DEBUGF(infof(तथ्य, "schannel: disabled server certificate revocation "
                         "checks"));
    }
    अन्यथा अगर(conn_config->revoke_best_effort) {
      flags |= SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
               SCH_CRED_IGNORE_REVOCATION_OFFLINE |
               SCH_CRED_REVOCATION_CHECK_CHAIN;

      DEBUGF(infof(तथ्य, "schannel: ignore revocation offline errors"));
    }
    अन्यथा {
      flags |= SCH_CRED_REVOCATION_CHECK_CHAIN;

      DEBUGF(infof(तथ्य, "schannel: checking server certificate revocation"));
    }
  }
  अन्यथा {
    flags = SCH_CRED_MANUAL_CRED_VALIDATION |
            SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
            SCH_CRED_IGNORE_REVOCATION_OFFLINE;
    DEBUGF(infof(तथ्य, "schannel: disabled server cert revocation checks"));
  }

  अगर(!conn_config->verifyhost) {
    flags |= SCH_CRED_NO_SERVERNAME_CHECK;
    DEBUGF(infof(तथ्य, "schannel: verifyhost setting prevents Schannel from "
                       "comparing the supplied target name with the subject "
                       "names in server certificates."));
  }

  अगर(!conn_config->auto_client_cert) {
    flags &= ~(DWORD)SCH_CRED_USE_DEFAULT_CREDS;
    flags |= SCH_CRED_NO_DEFAULT_CREDS;
    infof(तथ्य, "schannel: disabled automatic use of client certificate");
  }
  अन्यथा
    infof(तथ्य, "schannel: enabled automatic use of client certificate");

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  चयन(conn_config->version) {
  हाल CURL_SSLVERSION_TLSv1:
  हाल CURL_SSLVERSION_TLSv1_0:
  हाल CURL_SSLVERSION_TLSv1_1:
  हाल CURL_SSLVERSION_TLSv1_2:
  हाल CURL_SSLVERSION_TLSv1_3: {
    परिणाम = schannel_set_ssl_version_min_max(&enabled_protocols, cf, तथ्य);
    अगर(परिणाम)
      वापस परिणाम;
    अवरोध;
  }
  हाल CURL_SSLVERSION_SSLv3:
  हाल CURL_SSLVERSION_SSLv2:
    failf(तथ्य, "SSL versions not supported");
    वापस CURLE_NOT_BUILT_IN;
  शेष:
    failf(तथ्य, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    वापस CURLE_SSL_CONNECT_ERROR;
  }

  परिणाम = get_client_cert(cf, तथ्य, &client_cert_store, &client_cert);
  अगर(परिणाम)
    वापस परिणाम;

  /* allocate memory for the reusable credential handle */
  backend->cred = (काष्ठा Curl_schannel_cred *)
    curlx_calloc(1, माप(काष्ठा Curl_schannel_cred));
  अगर(!backend->cred) {
    failf(तथ्य, "schannel: unable to allocate memory");

    अगर(client_cert)
      CertFreeCertificateContext(client_cert);
    अगर(client_cert_store)
      CertCloseStore(client_cert_store, 0);

    वापस CURLE_OUT_OF_MEMORY;
  }
  backend->cred->refcount = 1;

  /* Since we did not persist the key, we need to extend the store's
   * lifetime until the end of the connection
   */
  backend->cred->client_cert_store = client_cert_store;

  परिणाम = acquire_sspi_handle(cf, तथ्य, backend, client_cert,
                               flags, enabled_protocols);

  अगर(client_cert)
    CertFreeCertificateContext(client_cert);

  वापस परिणाम;
}

अटल CURLcode schannel_connect_step1(काष्ठा Curl_cfilter *cf,
                                       काष्ठा Curl_easy *तथ्य)
{
  माप_प्रकार written = 0;
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा schannel_ssl_backend_तथ्य *backend =
    (काष्ठा schannel_ssl_backend_तथ्य *)connssl->backend;
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  Sec_बफर outbuf;
  Sec_बफर_Desc outbuf_desc;
  Sec_बफर inbuf;
  Sec_बफर_Desc inbuf_desc;
  अचिन्हित अक्षर alpn_बफर[128];
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CURLcode परिणाम;

  DEBUGASSERT(backend);
  DEBUGF(infof(तथ्य, "schannel: SSL/TLS connection with %s port %d (step 1/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->port));

  backend->use_alpn = connssl->alpn && s_win_has_alpn;

  अगर(conn_config->CAfile || conn_config->ca_info_blob) {
    अगर(curlx_verify_windows_version(6, 1, 0, PLATFORM_WINNT,
                                    VERSION_GREATER_THAN_EQUAL)) {
      backend->use_manual_cred_validation = TRUE;
    }
    अन्यथा {
      failf(तथ्य, "schannel: this version of Windows is too old to support "
                  "certificate verification via CA bundle file.");
      वापस CURLE_SSL_CACERT_BADFILE;
    }
  }
  अन्यथा
    backend->use_manual_cred_validation = FALSE;

  backend->cred = शून्य;

  /* check for an existing reusable credential handle */
  अगर(Curl_ssl_scache_use(cf, तथ्य)) {
    काष्ठा Curl_schannel_cred *old_cred;
    Curl_ssl_scache_lock(तथ्य);
    old_cred = Curl_ssl_scache_get_obj(cf, तथ्य, connssl->peer.scache_कुंजी);
    अगर(old_cred) {
      backend->cred = old_cred;
      DEBUGF(infof(तथ्य, "schannel: reusing existing credential handle"));

      /* increment the reference counter of the credential/session handle */
      backend->cred->refcount++;
      DEBUGF(infof(तथ्य,
                   "schannel: incremented credential handle refcount = %d",
                   backend->cred->refcount));
    }
    Curl_ssl_scache_unlock(तथ्य);
  }

  अगर(!backend->cred) {
    अक्षर *snihost;
    परिणाम = schannel_acquire_credential_handle(cf, तथ्य);
    अगर(परिणाम || !backend->cred)
      वापस परिणाम;
    /* schannel_acquire_credential_handle() sets backend->cred accordingly or
       it returns error otherwise. */

    /* A hostname associated with the credential is needed by
       InitializeSecurityContext for SNI and other reasons. */
    snihost = connssl->peer.sni ?
      connssl->peer.sni : connssl->peer.origin->hostname;
    backend->cred->sni_hostname = curlx_convert_UTF8_to_tchar(snihost);
    अगर(!backend->cred->sni_hostname)
      वापस CURLE_OUT_OF_MEMORY;
  }

  /* Warn if SNI is disabled due to use of an IP address */
  अगर(connssl->peer.type != CURL_SSL_PEER_DNS) {
    infof(तथ्य, "schannel: using IP address, SNI is not supported by OS.");
  }

  अगर(backend->use_alpn) {
    पूर्णांक cur = 0;
    पूर्णांक list_start_सूचकांक = 0;
    अचिन्हित पूर्णांक *extension_len = शून्य;
    अचिन्हित लघु *list_len = शून्य;
    स्थिर अचिन्हित पूर्णांक uiALPN = SecApplicationProtocolNegotiationExt_ALPN;
    काष्ठा alpn_proto_buf proto;

    /* The first four bytes is an unsigned int indicating number
       of bytes of data in the rest of the buffer. */
    extension_len = (अचिन्हित पूर्णांक *)(व्योम *)&alpn_बफर[cur];
    cur += (पूर्णांक)माप(अचिन्हित पूर्णांक);

    /* The next four bytes are an indicator that this buffer contains
       ALPN data, as opposed to NPN, for example. */
    स_नकल(&alpn_बफर[cur], &uiALPN, माप(uiALPN));
    cur += (पूर्णांक)माप(अचिन्हित पूर्णांक);

    /* The next two bytes is an unsigned short indicating the number
       of bytes used to list the preferred protocols. */
    list_len = (अचिन्हित लघु *)(व्योम *)&alpn_बफर[cur];
    cur += (पूर्णांक)माप(अचिन्हित लघु);

    list_start_सूचकांक = cur;

    परिणाम = Curl_alpn_to_proto_buf(&proto, connssl->alpn);
    अगर(परिणाम) {
      failf(तथ्य, "Error setting ALPN");
      वापस CURLE_SSL_CONNECT_ERROR;
    }
    स_नकल(&alpn_बफर[cur], proto.तथ्य, proto.len);
    cur += proto.len;

    *list_len = curlx_uitous(cur - list_start_सूचकांक);
    *extension_len = (अचिन्हित पूर्णांक)(*list_len +
      माप(अचिन्हित पूर्णांक) + माप(अचिन्हित लघु));

    Init_Sec_बफर(&inbuf, SECBUFFER_APPLICATION_PROTOCOLS, alpn_बफर, cur);
    Init_Sec_बफर_Desc(&inbuf_desc, &inbuf, 1);

    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(तथ्य, VTLS_INFOF_ALPN_OFFER_1STR, proto.तथ्य);
  }
  अन्यथा {
    Init_Sec_बफर(&inbuf, SECBUFFER_EMPTY, शून्य, 0);
    Init_Sec_बफर_Desc(&inbuf_desc, &inbuf, 1);
  }

  /* setup output buffer */
  Init_Sec_बफर(&outbuf, SECBUFFER_EMPTY, शून्य, 0);
  Init_Sec_बफर_Desc(&outbuf_desc, &outbuf, 1);

  /* security request flags */
  backend->req_flags =
    ISC_REQ_SEQUENCE_DETECT | ISC_REQ_REPLAY_DETECT |
    ISC_REQ_CONFIDENTIALITY | ISC_REQ_ALLOCATE_MEMORY |
    ISC_REQ_STREAM |
    (!conn_config->auto_client_cert ? ISC_REQ_USE_SUPPLIED_CREDS : 0);

  /* allocate memory for the security context handle */
  backend->ctxt = (काष्ठा Curl_schannel_ctxt *)
    curlx_calloc(1, माप(काष्ठा Curl_schannel_ctxt));
  अगर(!backend->ctxt) {
    failf(तथ्य, "schannel: unable to allocate memory");
    वापस CURLE_OUT_OF_MEMORY;
  }

  /* Schannel InitializeSecurityContext:
     https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-initializesecuritycontextw

     At the moment we do not pass inbuf unless we are using ALPN since we only
     use it for that, and WINE (for which we currently disable ALPN) is giving
     us problems with inbuf regardless. https://github.com/curl/curl/issues/983
   */
  sspi_status = Curl_pSecFn->InitializeSecurityContext(
    &backend->cred->cred_handle, शून्य, backend->cred->sni_hostname,
    backend->req_flags, 0, 0,
    (backend->use_alpn ? &inbuf_desc : शून्य),
    0, &backend->ctxt->ctxt_handle,
    &outbuf_desc, &backend->ret_flags, शून्य);

  अगर(sspi_status != SEC_I_CONTINUE_NEEDED) {
    अक्षर बफर[STRERROR_LEN];
    curlx_safefree(backend->ctxt);
    चयन(sspi_status) {
    हाल SEC_E_INSUFFICIENT_MEMORY:
      failf(तथ्य, "schannel: initial InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(sspi_status, बफर, माप(बफर)));
      वापस CURLE_OUT_OF_MEMORY;
    हाल SEC_E_WRONG_PRINCIPAL:
      failf(तथ्य, "schannel: SNI or certificate check failed: %s",
            Curl_sspi_strerror(sspi_status, बफर, माप(बफर)));
      वापस CURLE_PEER_FAILED_VERIFICATION;
#अगर 0
    हाल SEC_E_INVALID_HANDLE:
    हाल SEC_E_INVALID_TOKEN:
    हाल SEC_E_LOGON_DENIED:
    हाल SEC_E_TARGET_UNKNOWN:
    हाल SEC_E_NO_AUTHENTICATING_AUTHORITY:
    हाल SEC_E_INTERNAL_ERROR:
    हाल SEC_E_NO_CREDENTIALS:
    हाल SEC_E_UNSUPPORTED_FUNCTION:
    हाल SEC_E_APPLICATION_PROTOCOL_MISMATCH:
#पूर्ण_अगर
    शेष:
      failf(तथ्य, "schannel: initial InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(sspi_status, बफर, माप(बफर)));
      वापस CURLE_SSL_CONNECT_ERROR;
    }
  }

  DEBUGF(infof(तथ्य, "schannel: sending initial handshake data: "
               "sending %lu bytes.", outbuf.cb_बफर));

  /* send initial handshake data which is now stored in output buffer */
  परिणाम = Curl_conn_cf_send(cf->अग्र, तथ्य,
                             (स्थिर uint8_t *)outbuf.pv_बफर,
                             outbuf.cb_बफर, FALSE,
                             &written);
  Curl_pSecFn->Free_Context_बफर(outbuf.pv_बफर);
  अगर(परिणाम || (outbuf.cb_बफर != written)) {
    failf(तथ्य, "schannel: failed to send initial handshake data: "
          "sent %zu of %lu bytes", written, outbuf.cb_बफर);
    वापस CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGF(infof(तथ्य, "schannel: sent initial handshake data: "
               "sent %zu bytes", written));

  backend->recv_unrecoverable_err = CURLE_OK;
  backend->recv_sspi_close_notify = FALSE;
  backend->recv_connection_closed = FALSE;
  backend->recv_renegotiating = FALSE;
  backend->renegotiate_state.started = FALSE;
  backend->encdata_is_incomplete = FALSE;
  backend->shutdown_generated = FALSE;
  backend->shutdown_offset = 0;
  स_रखो(&backend->shutdown_token, 0, माप(backend->shutdown_token));

  /* continue to second handshake step */
  connssl->connecting_state = ssl_connect_2;

  वापस CURLE_OK;
}

अटल CURLcode schannel_error(काष्ठा Curl_easy *तथ्य,
                               SECURITY_STATUS sspi_status)
{
  अक्षर बफर[STRERROR_LEN];
  चयन(sspi_status) {
  हाल SEC_E_INSUFFICIENT_MEMORY:
    failf(तथ्य, "schannel: next InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(sspi_status, बफर, माप(बफर)));
    वापस CURLE_OUT_OF_MEMORY;
  हाल SEC_E_WRONG_PRINCIPAL:
    failf(तथ्य, "schannel: SNI or certificate check failed: %s",
          Curl_sspi_strerror(sspi_status, बफर, माप(बफर)));
    वापस CURLE_PEER_FAILED_VERIFICATION;
  हाल SEC_E_UNTRUSTED_मूल:
    failf(तथ्य, "schannel: %s",
          Curl_sspi_strerror(sspi_status, बफर, माप(बफर)));
    वापस CURLE_PEER_FAILED_VERIFICATION;
#अगर 0
  हाल SEC_E_INVALID_HANDLE:
  हाल SEC_E_INVALID_TOKEN:
  हाल SEC_E_LOGON_DENIED:
  हाल SEC_E_TARGET_UNKNOWN:
  हाल SEC_E_NO_AUTHENTICATING_AUTHORITY:
  हाल SEC_E_INTERNAL_ERROR:
  हाल SEC_E_NO_CREDENTIALS:
  हाल SEC_E_UNSUPPORTED_FUNCTION:
  हाल SEC_E_APPLICATION_PROTOCOL_MISMATCH:
#पूर्ण_अगर
  शेष:
    failf(तथ्य, "schannel: next InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(sspi_status, बफर, माप(बफर)));
    वापस CURLE_SSL_CONNECT_ERROR;
  }
}

अटल CURLcode schannel_pkp_pin_peer_pubkey(काष्ठा Curl_cfilter *cf,
                                             काष्ठा Curl_easy *तथ्य,
                                             स्थिर अक्षर *pinnedpubkey)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा schannel_ssl_backend_तथ्य *backend =
    (काष्ठा schannel_ssl_backend_तथ्य *)connssl->backend;
  CERT_CONTEXT *pCertContextServer = शून्य;

  /* Result is returned to caller */
  CURLcode परिणाम = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  DEBUGASSERT(backend);

  /* if a path was not specified, do not pin */
  अगर(!pinnedpubkey)
    वापस CURLE_OK;

  करो {
    SECURITY_STATUS sspi_status;
    uint8_t *x509_der;
    DWORD x509_der_len;
    काष्ठा Curl_X509certificate x509_parsed;
    काष्ठा Curl_asn1Element *pubkey;

    sspi_status =
      Curl_pSecFn->QueryContextAttributes(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                          &pCertContextServer);

    अगर((sspi_status != SEC_E_OK) || !pCertContextServer) {
      अक्षर बफर[STRERROR_LEN];
      failf(तथ्य, "schannel: Failed to read remote certificate context: %s",
            Curl_sspi_strerror(sspi_status, बफर, माप(बफर)));
      अवरोध; /* failed */
    }

    अगर(!(((pCertContextServer->dwCertEncodingType & X509_ASN_ENCODING) != 0) &&
         (pCertContextServer->cbCertEncoded > 0)))
      अवरोध;

    x509_der = (uint8_t *)pCertContextServer->pbCertEncoded;
    x509_der_len = pCertContextServer->cbCertEncoded;
    स_रखो(&x509_parsed, 0, माप(x509_parsed));
    अगर(Curl_parseX509(&x509_parsed, x509_der, x509_der + x509_der_len))
      अवरोध;

    pubkey = &x509_parsed.subject_Public_कुंजी_Info;
    अगर(!pubkey->header || pubkey->end <= pubkey->header) {
      failf(तथ्य, "SSL: failed retrieving public key from server certificate");
      अवरोध;
    }

    परिणाम = Curl_pin_peer_pubkey(तथ्य,
                                  pinnedpubkey,
                                  (स्थिर अचिन्हित अक्षर *)pubkey->header,
                                  (माप_प्रकार)(pubkey->end - pubkey->header));
    अगर(परिणाम) {
      failf(तथ्य, "SSL: public key does not match pinned public key");
    }
  } जबतक(0);

  अगर(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  वापस परिणाम;
}

अटल CURLcode ensure_encoding_आकार(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा sbuffer *encdata,
                                     माप_प्रकार min_लंबाई)
{
  माप_प्रकार आकार;
  DEBUGASSERT(encdata->लंबाई >= encdata->offset);
  अगर(encdata->लंबाई < encdata->offset)
    वापस CURLE_FAILED_INIT;
  आकार = encdata->लंबाई - encdata->offset;
  अगर(आकार < CURL_SCHANNEL_बफर_FREE_आकार || encdata->लंबाई < min_लंबाई) {
    अचिन्हित अक्षर *बफर;
    माप_प्रकार लंबाई = encdata->offset + CURL_SCHANNEL_बफर_FREE_आकार;
    अगर(लंबाई < min_लंबाई)
      लंबाई = min_लंबाई;
    बफर = curlx_realloc(encdata->बफर, लंबाई);
    अगर(!बफर) {
      failf(तथ्य, "schannel: unable to re-allocate memory");
      वापस CURLE_OUT_OF_MEMORY;
    }

    encdata->बफर = बफर;
    encdata->लंबाई = लंबाई;
    SCH_DEV(infof(तथ्य, "schannel: encdata.buffer resized %zu",
                  encdata->लंबाई));
  }
  वापस CURLE_OK;
}

अटल CURLcode ensure_decoding_आकार(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा sbuffer *decdata,
                                     माप_प्रकार nowsize,
                                     माप_प्रकार len)
{
  माप_प्रकार आकार = nowsize > CURL_SCHANNEL_बफर_FREE_आकार ?
    nowsize : CURL_SCHANNEL_बफर_FREE_आकार;
  DEBUGASSERT(decdata->लंबाई >= decdata->offset);
  अगर(decdata->लंबाई < decdata->offset)
    वापस CURLE_FAILED_INIT;
  अन्यथा अगर(decdata->लंबाई - decdata->offset < आकार ||
          decdata->लंबाई < len) {
    /* increase internal decrypted data buffer */
    माप_प्रकार लंबाई = decdata->offset + आकार;
    अचिन्हित अक्षर *बफर;
    /* make sure that the requested amount of data fits */
    अगर(लंबाई < len)
      लंबाई = len;

    बफर = curlx_realloc(decdata->बफर, लंबाई);
    अगर(!बफर) {
      failf(तथ्य, "schannel: unable to re-allocate memory");
      वापस CURLE_OUT_OF_MEMORY;
    }
    decdata->बफर = बफर;
    decdata->लंबाई = लंबाई;
  }
  वापस CURLE_OK;
}

अटल CURLcode schannel_connect_step2(काष्ठा Curl_cfilter *cf,
                                       काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा schannel_ssl_backend_तथ्य *backend =
    (काष्ठा schannel_ssl_backend_तथ्य *)connssl->backend;
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  पूर्णांक i;
  माप_प्रकार nread = 0;
  Sec_बफर outbuf[3];
  Sec_बफर_Desc outbuf_desc;
  Sec_बफर inbuf[2];
  Sec_बफर_Desc inbuf_desc;
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CURLcode परिणाम;
  bool doread;
  स्थिर अक्षर *pubkey_ptr;

  DEBUGASSERT(backend);

  doread = (connssl->io_need & CURL_SSL_IO_NEED_SEND) ? FALSE : TRUE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  DEBUGF(infof(तथ्य, "schannel: SSL/TLS connection with %s port %d (step 2/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->port));

  अगर(!backend->cred || !backend->ctxt)
    वापस CURLE_SSL_CONNECT_ERROR;

  /* buffer to store previously received and decrypted data */
  अगर(!backend->decdata.बफर) {
    backend->decdata.offset = 0;
    backend->decdata.लंबाई = CURL_SCHANNEL_बफर_INIT_आकार;
    backend->decdata.बफर = curlx_malloc(backend->decdata.लंबाई);
    अगर(!backend->decdata.बफर) {
      failf(तथ्य, "schannel: unable to allocate memory");
      वापस CURLE_OUT_OF_MEMORY;
    }
  }

  /* buffer to store previously received and encrypted data */
  अगर(!backend->encdata.बफर) {
    backend->encdata_is_incomplete = FALSE;
    backend->encdata.offset = 0;
    backend->encdata.लंबाई = CURL_SCHANNEL_बफर_INIT_आकार;
    backend->encdata.बफर = curlx_malloc(backend->encdata.लंबाई);
    अगर(!backend->encdata.बफर) {
      failf(तथ्य, "schannel: unable to allocate memory");
      वापस CURLE_OUT_OF_MEMORY;
    }
  }

  परिणाम = ensure_encoding_आकार(तथ्य, &backend->encdata, 0);
  अगर(परिणाम)
    वापस परिणाम;

  क्रम(;;) {
    अगर(doread) {
      /* read encrypted handshake data from socket */
      परिणाम = Curl_conn_cf_recv(cf->अग्र, तथ्य,
                                 (अक्षर *)(backend->encdata.बफर +
                                          backend->encdata.offset),
                                 backend->encdata.लंबाई -
                                 backend->encdata.offset,
                                 &nread);
      अगर(परिणाम == CURLE_AGAIN) {
        अगर(!backend->encdata.offset || backend->encdata_is_incomplete) {
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          DEBUGF(infof(तथ्य, "schannel: failed to receive handshake, "
                       "need more data"));
          वापस CURLE_OK;
        }
        अन्यथा {
          DEBUGF(infof(तथ्य, "schannel: no new handshake data received, "
                       "continuing to process existing handshake data"));
        }
      }
      अन्यथा अगर(परिणाम || (nread == 0)) {
        failf(तथ्य, "schannel: failed to receive handshake, "
              "SSL/TLS connection failed");
        वापस CURLE_SSL_CONNECT_ERROR;
      }
      अन्यथा {
        /* increase encrypted data buffer offset */
        backend->encdata.offset += nread;
        backend->encdata_is_incomplete = FALSE;
        SCH_DEV(infof(तथ्य, "schannel: encrypted data got %zu", nread));
      }
    }

    SCH_DEV(infof(तथ्य,
                  "schannel: encrypted data buffer: offset %zu length %zu",
                  backend->encdata.offset, backend->encdata.लंबाई));

    /* setup input buffers */
    Init_Sec_बफर(&inbuf[0], SECBUFFER_TOKEN,
                  curlx_memdup0((स्थिर अक्षर *)backend->encdata.बफर,
                                backend->encdata.offset),
                  curlx_uztoul(backend->encdata.offset));
    Init_Sec_बफर(&inbuf[1], SECBUFFER_EMPTY, शून्य, 0);
    Init_Sec_बफर_Desc(&inbuf_desc, inbuf, 2);

    /* setup output buffers */
    Init_Sec_बफर(&outbuf[0], SECBUFFER_TOKEN, शून्य, 0);
    Init_Sec_बफर(&outbuf[1], SECBUFFER_ALERT, शून्य, 0);
    Init_Sec_बफर(&outbuf[2], SECBUFFER_EMPTY, शून्य, 0);
    Init_Sec_बफर_Desc(&outbuf_desc, outbuf, 3);

    अगर(!inbuf[0].pv_बफर) {
      failf(तथ्य, "schannel: unable to allocate memory");
      वापस CURLE_OUT_OF_MEMORY;
    }

    /* The socket must be writable (or a poll error occurred) before we call
       InitializeSecurityContext to continue processing the received TLS
       records. This is because that function is not idempotent and we do not
       support partial save/resume sending replies of handshake tokens. */
    अगर(!SOCKET_WRITABLE(Curl_conn_cf_get_socket(cf, तथ्य), 0)) {
      SCH_DEV(infof(तथ्य, "schannel: handshake waiting for writable socket"));
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      curlx_free(inbuf[0].pv_बफर);
      वापस CURLE_OK;
    }

    sspi_status = Curl_pSecFn->InitializeSecurityContext(
      &backend->cred->cred_handle, &backend->ctxt->ctxt_handle,
      backend->cred->sni_hostname, backend->req_flags,
      0, 0, &inbuf_desc, 0, शून्य,
      &outbuf_desc, &backend->ret_flags, शून्य);

    /* free buffer for received handshake data */
    curlx_safefree(inbuf[0].pv_बफर);

    /* check if the handshake was incomplete */
    चयन(sspi_status) {
    हाल SEC_E_INCOMPLETE_MESSAGE:
      backend->encdata_is_incomplete = TRUE;
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      DEBUGF(infof(तथ्य,
                   "schannel: received incomplete message, need more data"));
      वापस CURLE_OK;

    हाल SEC_I_CONTINUE_NEEDED:
    हाल SEC_E_OK:
      /* check if the handshake needs to be continued */
      परिणाम = CURLE_OK;
      क्रम(i = 0; i < 3; i++) {
        /* search for handshake tokens that need to be sent */
        अगर(outbuf[i].बफर_Type == SECBUFFER_TOKEN && outbuf[i].cb_बफर > 0) {
          माप_प्रकार written = 0;
          DEBUGF(infof(तथ्य, "schannel: sending next handshake data: "
                       "sending %lu bytes.", outbuf[i].cb_बफर));

          /* send handshake token to server */
          परिणाम = Curl_conn_cf_send(cf->अग्र, तथ्य,
                                     (स्थिर uint8_t *)outbuf[i].pv_बफर,
                                     outbuf[i].cb_बफर,
                                     FALSE, &written);
          अगर(परिणाम || (outbuf[i].cb_बफर != written)) {
            failf(तथ्य, "schannel: failed to send next handshake data: "
                  "sent %zu of %lu bytes", written, outbuf[i].cb_बफर);
            परिणाम = CURLE_SSL_CONNECT_ERROR;
          }
        }
      }
      क्रम(i = 0; i < 3; i++) {
        /* free obsolete buffer */
        अगर(outbuf[i].pv_बफर)
          Curl_pSecFn->Free_Context_बफर(outbuf[i].pv_बफर);
      }
      अगर(परिणाम)
        वापस परिणाम;
      अवरोध;

    हाल SEC_I_INCOMPLETE_CREDENTIALS:
      अगर(!(backend->req_flags & ISC_REQ_USE_SUPPLIED_CREDS)) {
        /* If the server has requested a client certificate, attempt to
           continue the handshake without one. This allows connections to
           servers which request a client certificate but do not require
           it. */
        backend->req_flags |= ISC_REQ_USE_SUPPLIED_CREDS;
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        DEBUGF(infof(तथ्य,
                     "schannel: a client certificate has been requested"));
        वापस CURLE_OK;
      }
      FALLTHROUGH();

    शेष:
      वापस schannel_error(तथ्य, sspi_status);
    }

    /* check if there was additional remaining encrypted data */
    अगर(inbuf[1].बफर_Type == SECBUFFER_EXTRA && inbuf[1].cb_बफर > 0) {
      SCH_DEV(infof(तथ्य, "schannel: encrypted data length: %lu",
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
      अगर(backend->encdata.offset > inbuf[1].cb_बफर) {
        स_हटाओ(backend->encdata.बफर,
                (backend->encdata.बफर + backend->encdata.offset) -
                inbuf[1].cb_बफर, inbuf[1].cb_बफर);
        backend->encdata.offset = inbuf[1].cb_बफर;
        अगर(sspi_status == SEC_I_CONTINUE_NEEDED) {
          doread = FALSE;
          जारी;
        }
      }
    }
    अन्यथा {
      backend->encdata.offset = 0;
    }
    अवरोध;
  }

  /* check if the handshake needs to be continued */
  अगर(sspi_status == SEC_I_CONTINUE_NEEDED) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    वापस CURLE_OK;
  }

  /* check if the handshake is complete */
  अगर(sspi_status == SEC_E_OK) {
    connssl->connecting_state = ssl_connect_3;
    DEBUGF(infof(तथ्य, "schannel: SSL/TLS handshake complete"));
  }

#अगर_अघोषित CURL_DISABLE_PROXY
  pubkey_ptr = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(तथ्य, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(तथ्य, STRING_SSL_PINNEDPUBLICKEY);
#अन्यथा
  pubkey_ptr = CURL_EASY_STR(तथ्य, STRING_SSL_PINNEDPUBLICKEY);
#पूर्ण_अगर
  अगर(pubkey_ptr) {
    परिणाम = schannel_pkp_pin_peer_pubkey(cf, तथ्य, pubkey_ptr);
    अगर(परिणाम) {
      failf(तथ्य, "SSL: public key does not match pinned public key");
      वापस परिणाम;
    }
  }

  अगर(conn_config->verifypeer && backend->use_manual_cred_validation) {
    /* Certificate verification also verifies the hostname if verifyhost */
    वापस Curl_verify_certificate(cf, तथ्य);
  }

  /* Verify the hostname manually when certificate verification is disabled,
     because in that case Schannel does not verify it. */
  अगर(!conn_config->verifypeer && conn_config->verifyhost)
    वापस Curl_verify_host(cf, तथ्य);

  वापस CURLE_OK;
}

अटल bool valid_cert_encoding(स्थिर CERT_CONTEXT *cert_context)
{
  वापस cert_context &&
    ((cert_context->dwCertEncodingType & X509_ASN_ENCODING) != 0) &&
    cert_context->pbCertEncoded &&
    (cert_context->cbCertEncoded > 0);
}

प्रकार bool (*Read_crt_func)(स्थिर CERT_CONTEXT *ccert_context,
                              bool reverse_order, व्योम *arg);

अटल व्योम traverse_cert_store(स्थिर CERT_CONTEXT *context,
                                Read_crt_func func, व्योम *arg)
{
  स्थिर CERT_CONTEXT *current_context = शून्य;
  bool should_continue = TRUE;
  bool first = TRUE;
  bool reverse_order = FALSE;
  जबतक(should_continue &&
        (current_context = CertEnumCertificatesInStore(
          context->hCertStore,
          current_context)) != शून्य) {
    /* Windows 11 22H2 OS Build 22621.674 or higher enumerates certificates in
       leaf-to-root order while all previous versions of Windows enumerate
       certificates in root-to-leaf order. Determine the order of enumeration
       by comparing SECPKG_ATTR_REMOTE_CERT_CONTEXT's pbCertContext with the
       first certificate's pbCertContext. */
    अगर(first && context->pbCertEncoded != current_context->pbCertEncoded)
      reverse_order = TRUE;
    should_continue = func(current_context, reverse_order, arg);
    first = FALSE;
  }

  अगर(current_context)
    CertFreeCertificateContext(current_context);
}

अटल bool cert_गणक_callback(स्थिर CERT_CONTEXT *ccert_context,
                                  bool reverse_order, व्योम *certs_गिनती)
{
  (व्योम)reverse_order;
  अगर(valid_cert_encoding(ccert_context))
    (*(पूर्णांक *)certs_गिनती)++;
  अगर(*(पूर्णांक *)certs_गिनती > MAX_ALLOWED_CERT_AMOUNT)
    वापस FALSE;
  वापस TRUE;
}

काष्ठा Adder_args {
  काष्ठा Curl_easy *तथ्य;
  CURLcode परिणाम;
  पूर्णांक idx;
  पूर्णांक certs_गिनती;
};

अटल bool add_cert_to_certinfo(स्थिर CERT_CONTEXT *ccert_context,
                                 bool reverse_order, व्योम *raw_arg)
{
  काष्ठा Adder_args *args = (काष्ठा Adder_args *)raw_arg;
  args->परिणाम = CURLE_OK;
  अगर(valid_cert_encoding(ccert_context)) {
    पूर्णांक जोड़ो_सूचकांक = reverse_order ? (args->certs_गिनती - 1) - args->idx :
                       args->idx;
    args->परिणाम = Curl_extract_certinfo(args->तथ्य, जोड़ो_सूचकांक,
                                         ccert_context->pbCertEncoded,
                                         ccert_context->pbCertEncoded +
                                         ccert_context->cbCertEncoded);
    args->idx++;
  }
  वापस args->परिणाम == CURLE_OK;
}

अटल व्योम schannel_session_free(व्योम *sessionid)
{
  /* this is expected to be called under sessionid lock */
  काष्ठा Curl_schannel_cred *cred = sessionid;

  अगर(cred) {
    cred->refcount--;
    अगर(cred->refcount == 0) {
      Curl_pSecFn->FreeCredentialsHandle(&cred->cred_handle);
      curlx_free(cred->sni_hostname);
      अगर(cred->client_cert_store) {
        CertCloseStore(cred->client_cert_store, 0);
        cred->client_cert_store = शून्य;
      }
      curlx_safefree(cred);
    }
  }
}

अटल CURLcode schannel_connect_step3(काष्ठा Curl_cfilter *cf,
                                       काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा schannel_ssl_backend_तथ्य *backend =
    (काष्ठा schannel_ssl_backend_तथ्य *)connssl->backend;
  काष्ठा ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, तथ्य);
  CURLcode परिणाम = CURLE_OK;
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CERT_CONTEXT *ccert_context = शून्य;
  SecPkgContext_ApplicationProtocol alpn_परिणाम;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);
  DEBUGASSERT(backend);

  DEBUGF(infof(तथ्य, "schannel: SSL/TLS connection with %s port %d (step 3/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->port));

  अगर(!backend->cred)
    वापस CURLE_SSL_CONNECT_ERROR;

  /* check if the required context attributes are met */
  अगर(backend->ret_flags != backend->req_flags) {
    अगर(!(backend->ret_flags & ISC_RET_SEQUENCE_DETECT))
      failf(तथ्य, "schannel: failed to setup sequence detection");
    अगर(!(backend->ret_flags & ISC_RET_REPLAY_DETECT))
      failf(तथ्य, "schannel: failed to setup replay detection");
    अगर(!(backend->ret_flags & ISC_RET_CONFIDENTIALITY))
      failf(तथ्य, "schannel: failed to setup confidentiality");
    अगर(!(backend->ret_flags & ISC_RET_ALLOCATED_MEMORY))
      failf(तथ्य, "schannel: failed to setup memory allocation");
    अगर(!(backend->ret_flags & ISC_RET_STREAM))
      failf(तथ्य, "schannel: failed to setup stream orientation");
    वापस CURLE_SSL_CONNECT_ERROR;
  }

  अगर(backend->use_alpn) {
    sspi_status =
      Curl_pSecFn->QueryContextAttributes(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_APPLICATION_PROTOCOL,
                                          &alpn_परिणाम);

    अगर(sspi_status != SEC_E_OK) {
      failf(तथ्य, "schannel: failed to retrieve ALPN result");
      वापस CURLE_SSL_CONNECT_ERROR;
    }

    अगर(alpn_परिणाम.ProtoNegoStatus ==
       SecApplicationProtocolNegotiationStatus_Success) {
      अगर(backend->recv_renegotiating &&
         connssl->negotiated.alpn &&
         म_भेदन(connssl->negotiated.alpn,
                 (स्थिर अक्षर *)alpn_परिणाम.ProtocolId,
                 alpn_परिणाम.Protocol_Id_आकार)) {
        /* Renegotiation selected a different protocol now, we cannot
         * deal with this */
        failf(तथ्य, "schannel: server selected an ALPN protocol too late");
        वापस CURLE_SSL_CONNECT_ERROR;
      }
      Curl_alpn_set_negotiated(cf, तथ्य, connssl, alpn_परिणाम.ProtocolId,
                               alpn_परिणाम.Protocol_Id_आकार);
    }
    अन्यथा {
      अगर(!backend->recv_renegotiating)
        Curl_alpn_set_negotiated(cf, तथ्य, connssl, शून्य, 0);
    }
  }

  /* save the current session data for possible reuse */
  अगर(Curl_ssl_scache_use(cf, तथ्य)) {
    Curl_ssl_scache_lock(तथ्य);
    /* Up ref count since call takes ownership */
    backend->cred->refcount++;
    परिणाम = Curl_ssl_scache_add_obj(cf, तथ्य, connssl->peer.scache_कुंजी,
                                     backend->cred, schannel_session_free);
    Curl_ssl_scache_unlock(तथ्य);
    अगर(परिणाम)
      वापस परिणाम;
  }

  अगर(ssl_config->certinfo) {
    पूर्णांक certs_गिनती = 0;
    sspi_status =
      Curl_pSecFn->QueryContextAttributes(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                          &ccert_context);

    अगर((sspi_status != SEC_E_OK) || !ccert_context) {
      failf(तथ्य, "schannel: failed to retrieve remote cert context");
      वापस CURLE_PEER_FAILED_VERIFICATION;
    }

    traverse_cert_store(ccert_context, cert_गणक_callback, &certs_गिनती);
    अगर(certs_गिनती > MAX_ALLOWED_CERT_AMOUNT) {
      failf(तथ्य, "%d certificates is more than allowed (%d)",
            certs_गिनती, MAX_ALLOWED_CERT_AMOUNT);
      CertFreeCertificateContext(ccert_context);
      वापस CURLE_SSL_CONNECT_ERROR;
    }

    परिणाम = Curl_ssl_init_certinfo(तथ्य, certs_गिनती);
    अगर(!परिणाम) {
      काष्ठा Adder_args args;
      args.तथ्य = तथ्य;
      args.idx = 0;
      args.certs_गिनती = certs_गिनती;
      args.परिणाम = CURLE_OK;
      traverse_cert_store(ccert_context, add_cert_to_certinfo, &args);
      परिणाम = args.परिणाम;
    }
    CertFreeCertificateContext(ccert_context);
    अगर(परिणाम)
      वापस परिणाम;
  }

  connssl->connecting_state = ssl_connect_done;

  वापस CURLE_OK;
}

अटल CURLcode schannel_connect(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 bool *done)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  CURLcode परिणाम;

  /* check if the connection has already been established */
  अगर(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  *done = FALSE;

  अगर(connssl->connecting_state == ssl_connect_1) {
    परिणाम = schannel_connect_step1(cf, तथ्य);
    अगर(परिणाम)
      वापस परिणाम;
  }

  अगर(connssl->connecting_state == ssl_connect_2) {
    परिणाम = schannel_connect_step2(cf, तथ्य);
    अगर(परिणाम)
      वापस परिणाम;
  }

  अगर(connssl->connecting_state == ssl_connect_3) {
    परिणाम = schannel_connect_step3(cf, तथ्य);
    अगर(परिणाम)
      वापस परिणाम;
  }

  अगर(connssl->connecting_state == ssl_connect_done) {
    काष्ठा schannel_ssl_backend_तथ्य *backend =
      (काष्ठा schannel_ssl_backend_तथ्य *)connssl->backend;
    DEBUGASSERT(backend);

    अगर(Curl_pSecFn->QueryContextAttributes(
         &backend->ctxt->ctxt_handle,
         SECPKG_ATTR_STREAM_SIZES,
         &backend->stream_sizes)) {
      failf(तथ्य, "schannel: failed getting stream sizes");
      वापस CURLE_SSL_CONNECT_ERROR;
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

  वापस CURLE_OK;
}

क्रमागत schannel_renegotiate_caller_t {
  SCH_RENEG_CALLER_IS_RECV,
  SCH_RENEG_CALLER_IS_SEND
};

/* The maximum time we allow for Schannel renegotiation which may in some
   rare cases block either due to libcurl (waiting on the socket) or Windows
   (waiting on an interactive security prompt). Note Schannel "renegotiation"
   is not necessarily literal TLS renegotiation, but means DecryptMessage
   returned SEC_I_RENEGOTIATE which means at least the security context needs
   to be re-established. */
#घोषणा MAX_RENEG_BLOCK_TIME (60 * 1000) /* 60 seconds in milliseconds */

/* This function renegotiates the connection due to a server request received
   by schannel_recv. This function returns CURLE_AGAIN if the renegotiation is
   incomplete. In that case, we remain in the renegotiation (connecting) stage
   and future calls to schannel_recv and schannel_send must call this function
   first to complete the renegotiation. */
अटल CURLcode schannel_recv_renegotiate(
  काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
  क्रमागत schannel_renegotiate_caller_t caller)
{
  CURLcode परिणाम;
  curl_socket_t sockfd;
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा schannel_ssl_backend_तथ्य *backend =
    (काष्ठा schannel_ssl_backend_तथ्य *)connssl->backend;
  काष्ठा schannel_renegotiate_state *rs = &backend->renegotiate_state;

  अगर(!backend || !backend->recv_renegotiating) {
    failf(तथ्य, "schannel: unexpected call to schannel_recv_renegotiate");
    वापस CURLE_SSL_CONNECT_ERROR;
  }
  DEBUGASSERT(caller <= SCH_RENEG_CALLER_IS_SEND);
  अगर(caller == SCH_RENEG_CALLER_IS_RECV)
    SCH_DEV(infof(तथ्य, "schannel: renegotiation caller is schannel_recv"));
  अन्यथा
    SCH_DEV(infof(तथ्य, "schannel: renegotiation caller is schannel_send"));

  sockfd = Curl_conn_cf_get_socket(cf, तथ्य);

  अगर(sockfd == CURL_SOCKET_BAD) {
    failf(तथ्य, "schannel: renegotiation missing socket");
    वापस CURLE_SSL_CONNECT_ERROR;
  }

  अगर(!rs->started) { /* new renegotiation */
    infof(तथ्य, "schannel: renegotiating SSL/TLS connection");
    DEBUGASSERT(connssl->state == ssl_connection_complete);
    DEBUGASSERT(connssl->connecting_state == ssl_connect_done);
    connssl->state = ssl_connection_negotiating;
    connssl->connecting_state = ssl_connect_2;
    स_रखो(rs, 0, माप(*rs));
    rs->io_need = CURL_SSL_IO_NEED_SEND;
    rs->start_time = *Curl_pgrs_now(तथ्य);
    rs->started = TRUE;
  }

  क्रम(;;) {
    bool block_read, block_write, blocking, done;
    curl_socket_t readfd, writefd;
    timediff_t elapsed;

    elapsed = curlx_ptimediff_ms(Curl_pgrs_now(तथ्य), &rs->start_time);
    अगर(elapsed >= MAX_RENEG_BLOCK_TIME) {
      failf(तथ्य, "schannel: renegotiation timeout");
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      अवरोध;
    }

    /* the current io_need state may have been overwritten since the last time
       this function was called. restore the io_need state needed to continue
       the renegotiation. */

    connssl->io_need = rs->io_need;

    परिणाम = schannel_connect(cf, तथ्य, &done);

    rs->io_need = connssl->io_need;

    अगर(!परिणाम && !done)
      परिणाम = CURLE_AGAIN;

    अगर(परिणाम != CURLE_AGAIN)
      अवरोध;

    readfd = (rs->io_need & CURL_SSL_IO_NEED_RECV) ? sockfd : CURL_SOCKET_BAD;
    writefd = (rs->io_need & CURL_SSL_IO_NEED_SEND) ? sockfd : CURL_SOCKET_BAD;

    अगर(readfd == CURL_SOCKET_BAD && writefd == CURL_SOCKET_BAD)
      जारी;

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

    क्रम(;;) {
      पूर्णांक what;
      timediff_t timeout_ms, remaining;

      परिणाम = Curl_pgrsUpdate(तथ्य);
      अगर(परिणाम)
        अवरोध;

      elapsed = curlx_ptimediff_ms(Curl_pgrs_now(तथ्य), &rs->start_time);
      अगर(elapsed >= MAX_RENEG_BLOCK_TIME) {
        failf(तथ्य, "schannel: renegotiation timeout");
        परिणाम = CURLE_SSL_CONNECT_ERROR;
        अवरोध;
      }
      remaining = MAX_RENEG_BLOCK_TIME - elapsed;

      अगर(blocking) {
        timeout_ms = Curl_timeleft_ms(तथ्य);

        अगर(timeout_ms < 0) {
          परिणाम = CURLE_OPERATION_TIMEDOUT;
          अवरोध;
        }

        /* the blocking is in intervals so that the progress function can be
           called every second */
        अगर(!timeout_ms || timeout_ms > 1000)
          timeout_ms = 1000;

        अगर(timeout_ms > remaining)
          timeout_ms = remaining;
      }
      अन्यथा
        timeout_ms = 0;

      SCH_DEV(infof(तथ्य, "schannel: renegotiation wait until socket is"
                    "%s%s for up to %" FMT_TIMEDIFF_T " ms",
                    ((readfd != CURL_SOCKET_BAD) ? " readable" : ""),
                    ((writefd != CURL_SOCKET_BAD) ? " writable" : ""),
                    timeout_ms));

      what = Curl_socket_check(readfd, CURL_SOCKET_BAD, writefd, timeout_ms);

      अगर(what > 0 && (what & (CURL_CSELECT_IN | CURL_CSELECT_OUT))) {
        SCH_DEV(infof(तथ्य, "schannel: renegotiation socket %s%s",
                      ((what & CURL_CSELECT_IN) ? "CURL_CSELECT_IN " : ""),
                      ((what & CURL_CSELECT_OUT) ? "CURL_CSELECT_OUT " : "")));
        परिणाम = CURLE_AGAIN;
        अवरोध;
      }
      अन्यथा अगर(!what) {
        SCH_DEV(infof(तथ्य, "schannel: renegotiation socket timeout"));
        अगर(blocking)
          जारी;
        अन्यथा
          वापस CURLE_AGAIN;
      }

      failf(तथ्य, "schannel: socket error during renegotiation");
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      अवरोध;
    }
    अगर(परिणाम != CURLE_AGAIN)
      अवरोध;
  }

  DEBUGASSERT(परिणाम != CURLE_AGAIN);

  rs->started = FALSE;
  backend->recv_renegotiating = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  अगर(परिणाम)
    failf(तथ्य, "schannel: renegotiation failed");
  अन्यथा
    infof(तथ्य, "schannel: SSL/TLS connection renegotiated");

  वापस परिणाम;
}

अटल CURLcode schannel_send(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                              स्थिर व्योम *buf, माप_प्रकार len, माप_प्रकार *pnwritten)
{
  माप_प्रकार तथ्य_len = 0;
  अचिन्हित अक्षर *ptr = शून्य;
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  Sec_बफर outbuf[4];
  Sec_बफर_Desc outbuf_desc;
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CURLcode परिणाम = CURLE_OK;
  काष्ठा schannel_ssl_backend_तथ्य *backend =
    (काष्ठा schannel_ssl_backend_तथ्य *)connssl->backend;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  अगर(backend->recv_renegotiating) {
    परिणाम = schannel_recv_renegotiate(cf, तथ्य, SCH_RENEG_CALLER_IS_SEND);
    अगर(परिणाम)
      वापस परिणाम;
  }

  /* check if the buffer is longer than the maximum message length */
  अगर(len > backend->stream_sizes.cbMaximumMessage) {
    len = backend->stream_sizes.cbMaximumMessage;
  }

  /* calculate the complete message length and prepare the send buffer */
  तथ्य_len = backend->stream_sizes.cbHeader + len +
    backend->stream_sizes.cbTrailer;
  अगर(तथ्य_len > backend->send_बफर_len) {
    ptr = curlx_realloc(backend->send_बफर, तथ्य_len);
    अगर(!ptr)
      वापस CURLE_OUT_OF_MEMORY;
    backend->send_बफर = ptr;
    backend->send_बफर_len = तथ्य_len;
  }
  अन्यथा
    ptr = backend->send_बफर;

  /* setup output buffers (header, data, trailer, empty) */
  Init_Sec_बफर(&outbuf[0], SECBUFFER_STREAM_HEADER,
                ptr, backend->stream_sizes.cbHeader);
  Init_Sec_बफर(&outbuf[1], SECBUFFER_तथ्य,
                ptr + backend->stream_sizes.cbHeader, curlx_uztoul(len));
  Init_Sec_बफर(&outbuf[2], SECBUFFER_STREAM_TRAILER,
                ptr + backend->stream_sizes.cbHeader + len,
                backend->stream_sizes.cbTrailer);
  Init_Sec_बफर(&outbuf[3], SECBUFFER_EMPTY, शून्य, 0);
  Init_Sec_बफर_Desc(&outbuf_desc, outbuf, 4);

  /* copy data into output buffer */
  स_नकल(outbuf[1].pv_बफर, buf, len);

  /* https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-encryptmessage */
  sspi_status = Curl_pSecFn->EncryptMessage(&backend->ctxt->ctxt_handle, 0,
                                            &outbuf_desc, 0);

  /* check if the message was encrypted */
  अगर(sspi_status == SEC_E_OK) {

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
    जबतक(len > *pnwritten) {
      माप_प्रकार this_write = 0;
      पूर्णांक what;
      timediff_t timeout_ms = Curl_timeleft_ms(तथ्य);
      अगर(timeout_ms < 0) {
        /* we already got the timeout */
        failf(तथ्य, "schannel: timed out sending data (bytes sent: %zu)",
              *pnwritten);
        परिणाम = CURLE_OPERATION_TIMEDOUT;
        अवरोध;
      }
      अन्यथा अगर(!timeout_ms)
        timeout_ms = TIMEDIFF_T_MAX;
      what = SOCKET_WRITABLE(Curl_conn_cf_get_socket(cf, तथ्य), timeout_ms);
      अगर(what < 0) {
        /* fatal error */
        failf(तथ्य, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        परिणाम = CURLE_SEND_ERROR;
        अवरोध;
      }
      अन्यथा अगर(what == 0) {
        failf(तथ्य, "schannel: timed out sending data (bytes sent: %zu)",
              *pnwritten);
        परिणाम = CURLE_OPERATION_TIMEDOUT;
        अवरोध;
      }
      /* socket is writable */

       परिणाम = Curl_conn_cf_send(cf->अग्र, तथ्य,
                                  (स्थिर uint8_t *)ptr + *pnwritten,
                                  len - *pnwritten,
                                  FALSE, &this_write);
      अगर(परिणाम == CURLE_AGAIN)
        जारी;
      अन्यथा अगर(परिणाम) {
        अवरोध;
      }

      *pnwritten += this_write;
    }
  }
  अन्यथा अगर(sspi_status == SEC_E_INSUFFICIENT_MEMORY) {
    परिणाम = CURLE_OUT_OF_MEMORY;
  }
  अन्यथा {
    परिणाम = CURLE_SEND_ERROR;
  }

  अगर(len == *pnwritten)
    /* Encrypted message including header, data and trailer entirely sent.
       The return value is the number of unencrypted bytes that were sent. */
    *pnwritten = outbuf[1].cb_बफर;

  वापस परिणाम;
}

अटल CURLcode schannel_recv(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                              अक्षर *buf, माप_प्रकार len, माप_प्रकार *pnread)
{
  माप_प्रकार आकार = 0;
  माप_प्रकार nread = 0;
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  Sec_बफर inbuf[4];
  Sec_बफर_Desc inbuf_desc;
  SECURITY_STATUS sspi_status = SEC_E_OK;
  काष्ठा schannel_ssl_backend_तथ्य *backend =
    (काष्ठा schannel_ssl_backend_तथ्य *)connssl->backend;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(backend);
  *pnread = 0;

  अगर(backend->recv_renegotiating) {
    परिणाम = schannel_recv_renegotiate(cf, तथ्य, SCH_RENEG_CALLER_IS_RECV);
    अगर(परिणाम)
      वापस परिणाम;
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

  SCH_DEV(infof(तथ्य, "schannel: client wants to read %zu bytes", len));

  अगर(len && len <= backend->decdata.offset) {
    SCH_DEV(infof(तथ्य,
                  "schannel: enough decrypted data is already available"));
    जाओ cleanup;
  }
  अन्यथा अगर(backend->recv_unrecoverable_err) {
    परिणाम = backend->recv_unrecoverable_err;
    infof(तथ्य, "schannel: an unrecoverable error occurred in a prior call");
    जाओ cleanup;
  }
  अन्यथा अगर(backend->recv_sspi_close_notify) {
    /* once a server has indicated shutdown there is no more encrypted data */
    infof(तथ्य, "schannel: server indicated shutdown in a prior call");
    जाओ cleanup;
  }
  /* it is debatable what to return when !len. Regardless we cannot return
     immediately because there may be data to decrypt (in the case we want to
     decrypt all encrypted cached data) so handle !len later in cleanup. */
  अन्यथा अगर(len && !backend->recv_connection_closed) {
    /* the encrypted buffer must be large enough to hold all the bytes
       requested and some TLS record overhead. 'len' is a buffer size, so this
       integer math cannot overflow. */
    स्थिर माप_प्रकार min_encdata_लंबाई = len + CURL_SCHANNEL_बफर_FREE_आकार;

    /* make sure encrypt buffer fits the requested amount of data */
    परिणाम = ensure_encoding_आकार(तथ्य, &backend->encdata, min_encdata_लंबाई);
    अगर(परिणाम)
      जाओ cleanup;

    SCH_DEV(infof(तथ्य,
                  "schannel: encrypted data buffer: offset %zu length %zu",
                  backend->encdata.offset, backend->encdata.लंबाई));

    /* read encrypted data from socket */
    परिणाम = Curl_conn_cf_recv(cf->अग्र, तथ्य,
                               (अक्षर *)(backend->encdata.बफर +
                                        backend->encdata.offset),
                               backend->encdata.लंबाई -
                               backend->encdata.offset,
                               &nread);
    अगर(परिणाम) {
      अगर(परिणाम == CURLE_AGAIN)
        SCH_DEV(infof(तथ्य, "schannel: recv returned CURLE_AGAIN"));
      अन्यथा {
        infof(तथ्य, "schannel: recv returned error %d", (पूर्णांक)परिणाम);
        backend->recv_unrecoverable_err = परिणाम;
      }
    }
    अन्यथा अगर(nread == 0) {
      backend->recv_connection_closed = TRUE;
      DEBUGF(infof(तथ्य, "schannel: server closed the connection"));
    }
    अन्यथा {
      backend->encdata.offset += nread;
      backend->encdata_is_incomplete = FALSE;
      SCH_DEV(infof(तथ्य, "schannel: encrypted data got %zu", nread));
    }
  }

  SCH_DEV(infof(तथ्य, "schannel: encrypted data buffer: offset %zu length %zu",
                backend->encdata.offset, backend->encdata.लंबाई));

  /* decrypt loop */
  जबतक(backend->encdata.offset > 0 && sspi_status == SEC_E_OK &&
        (!len || backend->decdata.offset < len ||
         backend->recv_connection_closed)) {
    /* prepare data buffer for DecryptMessage call */
    Init_Sec_बफर(&inbuf[0], SECBUFFER_तथ्य, backend->encdata.बफर,
                  curlx_uztoul(backend->encdata.offset));

    /* we need 3 more empty input buffers for possible output */
    Init_Sec_बफर(&inbuf[1], SECBUFFER_EMPTY, शून्य, 0);
    Init_Sec_बफर(&inbuf[2], SECBUFFER_EMPTY, शून्य, 0);
    Init_Sec_बफर(&inbuf[3], SECBUFFER_EMPTY, शून्य, 0);
    Init_Sec_बफर_Desc(&inbuf_desc, inbuf, 4);

    /* https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-decryptmessage
     */
    sspi_status = Curl_pSecFn->DecryptMessage(&backend->ctxt->ctxt_handle,
                                              &inbuf_desc, 0, शून्य);

    /* check if everything went fine (server may want to renegotiate
       or shutdown the connection context) */
    अगर(sspi_status == SEC_E_OK || sspi_status == SEC_I_RENEGOTIATE ||
       sspi_status == SEC_I_CONTEXT_EXPIRED) {
      /* check for successfully decrypted data, even before actual
         renegotiation or shutdown of the connection context */
      अगर(inbuf[1].बफर_Type == SECBUFFER_तथ्य) {
        SCH_DEV(infof(तथ्य, "schannel: decrypted data length: %lu",
                      inbuf[1].cb_बफर));

        /* ensure the decode buffer fits the received amount of data */
        परिणाम = ensure_decoding_आकार(तथ्य, &backend->decdata,
                                      inbuf[1].cb_बफर, len);
        अगर(परिणाम)
          जाओ cleanup;

        /* copy decrypted data to internal buffer */
        आकार = inbuf[1].cb_बफर;
        अगर(आकार) {
          स_नकल(backend->decdata.बफर + backend->decdata.offset,
                 inbuf[1].pv_बफर, आकार);
          backend->decdata.offset += आकार;
        }

        SCH_DEV(infof(तथ्य, "schannel: decrypted data added: %zu", आकार));
        SCH_DEV(infof(तथ्य,
                      "schannel: decrypted cached: offset %zu length %zu",
                      backend->decdata.offset, backend->decdata.लंबाई));
      }

      /* check for remaining encrypted data */
      अगर(inbuf[3].बफर_Type == SECBUFFER_EXTRA && inbuf[3].cb_बफर > 0) {
        SCH_DEV(infof(तथ्य, "schannel: encrypted data length: %lu",
                      inbuf[3].cb_बफर));

        /* check if the remaining data is less than the total amount
         * and therefore begins after the already processed data
         */
        अगर(backend->encdata.offset > inbuf[3].cb_बफर) {
          /* move remaining encrypted data forward to the beginning of
बफर*/
          स_हटाओ(backend->encdata.बफर,
                  (backend->encdata.बफर + backend->encdata.offset) -
                  inbuf[3].cb_बफर, inbuf[3].cb_बफर);
          backend->encdata.offset = inbuf[3].cb_बफर;
        }

        SCH_DEV(infof(तथ्य,
                      "schannel: encrypted cached: offset %zu length %zu",
                      backend->encdata.offset, backend->encdata.लंबाई));
      }
      अन्यथा {
        /* reset encrypted buffer offset, because there is no data remaining */
        backend->encdata.offset = 0;
      }

      /* check if server wants to renegotiate the connection context */
      अगर(sspi_status == SEC_I_RENEGOTIATE) {
        infof(तथ्य, "schannel: remote party requests renegotiation");
        अगर(परिणाम && परिणाम != CURLE_AGAIN) {
          infof(तथ्य, "schannel: cannot renegotiate, an error is pending");
          जाओ cleanup;
        }

        backend->recv_renegotiating = TRUE;
        परिणाम = schannel_recv_renegotiate(cf, तथ्य, SCH_RENEG_CALLER_IS_RECV);
        अगर(परिणाम)
          जाओ cleanup;

        /* now retry receiving data */
        sspi_status = SEC_E_OK;
        जारी;
      }
      /* check if the server closed the connection */
      अन्यथा अगर(sspi_status == SEC_I_CONTEXT_EXPIRED) {
        /* In Windows 2000 SEC_I_CONTEXT_EXPIRED (close_notify) is not
           returned so we have to work around that in cleanup. */
        backend->recv_sspi_close_notify = TRUE;
        अगर(!backend->recv_connection_closed)
          backend->recv_connection_closed = TRUE;
        /* We received the close notify fine, any error we got
         * from the lower filters afterwards (e.g. the socket), is not
         * an error on the TLS data stream. That one ended here. */
        अगर(परिणाम == CURLE_RECV_ERROR)
          परिणाम = CURLE_OK;
        infof(तथ्य,
              "schannel: server close notification received (close_notify)");
        जाओ cleanup;
      }
    }
    अन्यथा अगर(sspi_status == SEC_E_INCOMPLETE_MESSAGE) {
      backend->encdata_is_incomplete = TRUE;
      अगर(!परिणाम)
        परिणाम = CURLE_AGAIN;
      SCH_DEV(infof(तथ्य, "schannel: failed to decrypt data, need more data"));
      जाओ cleanup;
    }
    अन्यथा {
      अक्षर बफर[STRERROR_LEN];
      failf(तथ्य, "schannel: failed to read data from server: %s",
            Curl_sspi_strerror(sspi_status, बफर, माप(बफर)));
      परिणाम = CURLE_RECV_ERROR;
      जाओ cleanup;
    }
  }

  SCH_DEV(infof(तथ्य, "schannel: encrypted data buffer: offset %zu length %zu",
                backend->encdata.offset, backend->encdata.लंबाई));

  SCH_DEV(infof(तथ्य, "schannel: decrypted data buffer: offset %zu length %zu",
                backend->decdata.offset, backend->decdata.लंबाई));

cleanup:
  /* Warning- there is no guarantee the encdata state is valid at this point */
  SCH_DEV(infof(तथ्य, "schannel: schannel_recv cleanup"));

  /* Error if the connection has closed without a close_notify.

     The behavior here is a matter of debate. We do not want to be vulnerable
     to a truncation attack however there is some browser precedent for
     ignoring the close_notify for compatibility reasons. */
  अगर(len && !backend->decdata.offset && backend->recv_connection_closed &&
     !backend->recv_sspi_close_notify) {
    परिणाम = CURLE_RECV_ERROR;
    failf(तथ्य, "schannel: server closed abruptly (missing close_notify)");
  }

  /* Any error other than CURLE_AGAIN is an unrecoverable error. */
  अगर(परिणाम && परिणाम != CURLE_AGAIN)
    backend->recv_unrecoverable_err = परिणाम;

  आकार = len < backend->decdata.offset ? len : backend->decdata.offset;
  अगर(आकार) {
    स_नकल(buf, backend->decdata.बफर, आकार);
    स_हटाओ(backend->decdata.बफर, backend->decdata.बफर + आकार,
            backend->decdata.offset - आकार);
    backend->decdata.offset -= आकार;
    SCH_DEV(infof(तथ्य, "schannel: decrypted data returned %zu", आकार));
    SCH_DEV(infof(तथ्य,
                  "schannel: decrypted data buffer: offset %zu length %zu",
                  backend->decdata.offset, backend->decdata.लंबाई));
    *pnread = आकार;
    वापस CURLE_OK;
  }

  अगर(!परिणाम && !backend->recv_connection_closed)
    परिणाम = CURLE_AGAIN;

  /* it is debatable what to return when !len. We could return whatever error
     we got from decryption but instead we override here so the return is
     consistent. */
  अगर(!len)
    वापस CURLE_OK;

  वापस परिणाम;
}

अटल bool schannel_तथ्य_pending(काष्ठा Curl_cfilter *cf,
                                  स्थिर काष्ठा Curl_easy *तथ्य)
{
  स्थिर काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा schannel_ssl_backend_तथ्य *backend =
    (काष्ठा schannel_ssl_backend_तथ्य *)connssl->backend;

  (व्योम)तथ्य;
  DEBUGASSERT(backend);

  अगर(backend->ctxt) /* SSL/TLS is in use */
    वापस backend->decdata.offset > 0 ||
           (backend->encdata.offset > 0 && !backend->encdata_is_incomplete) ||
           backend->recv_connection_closed ||
           backend->recv_sspi_close_notify ||
           backend->recv_unrecoverable_err;
  अन्यथा
    वापस FALSE;
}

/* shut down the SSL connection and clean up related memory.
   this function can be called multiple times on the same connection including
   if the SSL connection failed (eg connection made but failed handshake). */
अटल CURLcode schannel_shutdown(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य,
                                  bool send_shutdown, bool *done)
{
  /* See https://learn.microsoft.com/windows/win32/secauthn/shutting-down-an-schannel-connection
   * Shutting Down an Schannel Connection
   */
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा schannel_ssl_backend_तथ्य *backend =
    (काष्ठा schannel_ssl_backend_तथ्य *)connssl->backend;
  CURLcode परिणाम = CURLE_OK;

  अगर(cf->shutdown) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  DEBUGASSERT(तथ्य);
  DEBUGASSERT(backend);

  /* Not supported in schannel */
  (व्योम)send_shutdown;

  *done = FALSE;
  अगर(backend->ctxt) {
    infof(तथ्य, "schannel: shutting down SSL/TLS connection with %s port %d",
          connssl->peer.origin->hostname, connssl->peer.origin->port);
  }

  अगर(!backend->ctxt || cf->shutdown) {
    *done = TRUE;
    जाओ out;
  }

  अगर(backend->cred && backend->ctxt && !backend->sent_shutdown &&
     !backend->shutdown_generated) {
    Sec_बफर_Desc BuffDesc;
    Sec_बफर बफर_2;
    SECURITY_STATUS sspi_status;
    Sec_बफर outbuf;
    Sec_बफर_Desc outbuf_desc;
    DWORD dwshut = SCHANNEL_SHUTDOWN;

    Init_Sec_बफर(&बफर_2, SECBUFFER_TOKEN, &dwshut, माप(dwshut));
    Init_Sec_बफर_Desc(&BuffDesc, &बफर_2, 1);

    sspi_status = Curl_pSecFn->ApplyControlToken(&backend->ctxt->ctxt_handle,
                                                 &BuffDesc);

    अगर(sspi_status != SEC_E_OK) {
      अक्षर बफर[STRERROR_LEN];
      failf(तथ्य, "schannel: ApplyControlToken failure: %s",
            Curl_sspi_strerror(sspi_status, बफर, माप(बफर)));
      परिणाम = CURLE_SEND_ERROR;
      जाओ out;
    }

    /* setup output buffer */
    Init_Sec_बफर(&outbuf, SECBUFFER_EMPTY, शून्य, 0);
    Init_Sec_बफर_Desc(&outbuf_desc, &outbuf, 1);

    sspi_status = Curl_pSecFn->InitializeSecurityContext(
      &backend->cred->cred_handle,
      &backend->ctxt->ctxt_handle,
      backend->cred->sni_hostname,
      backend->req_flags,
      0,
      0,
      शून्य,
      0,
      &backend->ctxt->ctxt_handle,
      &outbuf_desc,
      &backend->ret_flags, शून्य);

    अगर((sspi_status == SEC_E_OK) || (sspi_status == SEC_I_CONTEXT_EXPIRED)) {
      backend->shutdown_token = outbuf;
      backend->shutdown_generated = TRUE;
    }
    अन्यथा {
      अक्षर बफर[STRERROR_LEN];
      अगर(outbuf.pv_बफर)
        Curl_pSecFn->Free_Context_बफर(outbuf.pv_बफर);
      failf(तथ्य, "schannel: generating close msg failed: %s",
            Curl_sspi_strerror(sspi_status, बफर, माप(बफर)));
      परिणाम = CURLE_SEND_ERROR;
      जाओ out;
    }
  }

  अगर(backend->shutdown_generated && !backend->sent_shutdown) {
    माप_प्रकार written = 0;
    माप_प्रकार remaining;

    DEBUGASSERT(backend->shutdown_offset <= backend->shutdown_token.cb_बफर);
    remaining = backend->shutdown_token.cb_बफर - backend->shutdown_offset;

    अगर(remaining) {
      परिणाम = Curl_conn_cf_send(cf->अग्र, तथ्य,
                                 ((uint8_t *)backend->shutdown_token.pv_बफर +
                                  backend->shutdown_offset),
                                 remaining, FALSE, &written);
      अगर(!परिणाम)
        backend->shutdown_offset += written;
      अगर(परिणाम == CURLE_AGAIN || (!परिणाम && written < remaining)) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        परिणाम = CURLE_OK;
        जाओ out;
      }
    }
    अन्यथा /* no shutdown message to send? */
      परिणाम = CURLE_OK;
    अगर(परिणाम) {
      अगर(backend->recv_connection_closed) {
        /* The server already closed, so sending close_notify is optional. */
        *done = TRUE;
        परिणाम = CURLE_OK;
      }
      अन्यथा {
        failf(तथ्य, "schannel: error sending close msg: %d", (पूर्णांक)परिणाम);
        परिणाम = CURLE_SEND_ERROR;
      }
      जाओ out;
    }
    अगर(backend->shutdown_token.pv_बफर)
      Curl_pSecFn->Free_Context_बफर(backend->shutdown_token.pv_बफर);
    स_रखो(&backend->shutdown_token, 0, माप(backend->shutdown_token));
    backend->shutdown_offset = 0;
    backend->sent_shutdown = TRUE;
    *done = TRUE;
  }

  /* If the connection seems open and we have not seen the close notify
   * from the server yet, try to receive it. */
  अगर(backend->cred && backend->ctxt &&
     !backend->recv_sspi_close_notify && !backend->recv_connection_closed) {
    अक्षर बफर[1024];
    माप_प्रकार nread;

    परिणाम = schannel_recv(cf, तथ्य, बफर, माप(बफर), &nread);
    अगर(परिणाम == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
    }
    अन्यथा अगर(परिणाम) {
      CURL_TRC_CF(तथ्य, cf, "SSL shutdown, error %d", (पूर्णांक)परिणाम);
      परिणाम = CURLE_RECV_ERROR;
    }
    अन्यथा अगर(nread == 0) {
      /* We got the close notify alert and are done. */
      backend->recv_connection_closed = TRUE;
      *done = TRUE;
    }
    अन्यथा {
      /* still data coming in? */
    }
  }

out:
  cf->shutdown = (परिणाम || *done);
  वापस परिणाम;
}

अटल व्योम schannel_close(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा schannel_ssl_backend_तथ्य *backend =
    (काष्ठा schannel_ssl_backend_तथ्य *)connssl->backend;

  DEBUGASSERT(तथ्य);
  DEBUGASSERT(backend);

  अगर(backend->shutdown_token.pv_बफर)
    Curl_pSecFn->Free_Context_बफर(backend->shutdown_token.pv_बफर);
  स_रखो(&backend->shutdown_token, 0, माप(backend->shutdown_token));
  backend->shutdown_offset = 0;

  /* free SSPI Schannel API security context handle */
  अगर(backend->ctxt) {
    अगर(cf->conn->sslContext == &backend->ctxt->ctxt_handle)
      cf->conn->sslContext = शून्य;

    DEBUGF(infof(तथ्य, "schannel: clear security context handle"));
    Curl_pSecFn->DeleteSecurityContext(&backend->ctxt->ctxt_handle);
    curlx_safefree(backend->ctxt);
  }

  /* free SSPI Schannel API credential handle */
  अगर(backend->cred) {
    Curl_ssl_scache_lock(तथ्य);
    schannel_session_free(backend->cred);
    Curl_ssl_scache_unlock(तथ्य);
    backend->cred = शून्य;
  }

  /* free the buffer used to encrypt outgoing data */
  curlx_safefree(backend->send_बफर);
  backend->send_बफर_len = 0;

  /* free internal buffer for received encrypted data */
  अगर(backend->encdata.बफर) {
    curlx_safefree(backend->encdata.बफर);
    backend->encdata.लंबाई = 0;
    backend->encdata.offset = 0;
    backend->encdata_is_incomplete = FALSE;
  }

  /* free internal buffer for received decrypted data */
  अगर(backend->decdata.बफर) {
    curlx_safefree(backend->decdata.बफर);
    backend->decdata.लंबाई = 0;
    backend->decdata.offset = 0;
  }
}

अटल पूर्णांक schannel_init(व्योम)
{
  प्रकार स्थिर अक्षर *(APIENTRY *WINE_GET_VERSION_FN)(व्योम);
#अगर defined(__clang__) && __clang_major__ >= 16
#आशय clang diagnostic धकेलो
#आशय clang diagnostic ignored "-Wcast-function-type-strict"
#पूर्ण_अगर
  WINE_GET_VERSION_FN p_wine_get_version =
    CURLX_FUNCTION_CAST(WINE_GET_VERSION_FN,
      GetProcAddress(GetModuleHandle(TEXT("ntdll")), "wine_get_version"));
#अगर defined(__clang__) && __clang_major__ >= 16
#आशय clang diagnostic निकालो
#पूर्ण_अगर
  अगर(p_wine_get_version) {  /* WINE detected */
    curl_off_t ver = 0;
    स्थिर अक्षर *wine_version = p_wine_get_version();  /* e.g. "6.0.2" */
    /* Assume ALPN support with WINE 6.0 or greater */
    अगर(wine_version)
      curlx_str_संख्या(&wine_version, &ver, 20);
    s_win_has_alpn = (ver >= 6);
  }
  अन्यथा {
    /* ALPN is supported on Windows 8.1 / Server 2012 R2 or newer. */
    s_win_has_alpn = curlx_verify_windows_version(6, 3, 0, PLATFORM_WINNT,
                                                  VERSION_GREATER_THAN_EQUAL);
  }

  वापस Curl_sspi_global_init() == CURLE_OK ? 1 : 0;
}

अटल व्योम schannel_cleanup(व्योम)
{
  Curl_sspi_global_cleanup();
}

अटल माप_प्रकार schannel_version(अक्षर *बफर, माप_प्रकार आकार)
{
  वापस curl_msnprintf(बफर, आकार, "Schannel");
}

अटल CURLcode schannel_random(काष्ठा Curl_easy *तथ्य,
                                अचिन्हित अक्षर *entropy, माप_प्रकार लंबाई)
{
  (व्योम)तथ्य;

  वापस curlx_win32_random(entropy, लंबाई);
}

अटल CURLcode schannel_checksum(स्थिर अचिन्हित अक्षर *निवेश,
                                  माप_प्रकार inputlen,
                                  अचिन्हित अक्षर *checksum,
                                  माप_प्रकार checksumlen,
                                  DWORD provType,
                                  स्थिर अचिन्हित पूर्णांक algId)
{
  CURLcode परिणाम = CURLE_FAILED_INIT;

  HCRYPTPROV hProv = 0;
  HCRYPTHASH hHash = 0;

  माप_प्रकार off;

  DWORD cb_Hash_आकार;
  DWORD dw_Hash_आकार_Len;
  DWORD dwChecksumLen;

  /* since this can fail in multiple ways, zero memory first so we never
   * return old data
   */
  स_रखो(checksum, 0, checksumlen);

  अगर(!CryptAcquireContext(&hProv, शून्य, शून्य, provType,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    जाओ out;

  अगर(!CryptCreateHash(hProv, algId, 0, 0, &hHash))
    जाओ out;

  परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;

  off = 0;
  जबतक(off < inputlen) {
    DWORD chunk = (DWORD)CURLMIN(inputlen - off, 0xffffffffUL);
    अगर(!Crypt_Hash_तथ्य(hHash, निवेश + off, chunk, 0))
      जाओ out;
    off += chunk;
  }

  /* get hash size */
  cb_Hash_आकार = 0;
  dw_Hash_आकार_Len = (DWORD)माप(cb_Hash_आकार);
  अगर(!CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE *)&cb_Hash_आकार,
                        &dw_Hash_आकार_Len, 0))
    जाओ out;

  /* check if hash fits into the return buffer */
  अगर(checksumlen < cb_Hash_आकार)
    जाओ out;

  dwChecksumLen = (DWORD)checksumlen;
  अगर(CryptGetHashParam(hHash, HP_HASHVAL, checksum, &dwChecksumLen, 0) &&
     dwChecksumLen == cb_Hash_आकार)
    परिणाम = CURLE_OK;

out:
  अगर(hHash)
    CryptDestroyHash(hHash);

  अगर(hProv)
    CryptReleaseContext(hProv, 0);

  वापस परिणाम;
}

अटल CURLcode schannel_sha256sum(स्थिर अचिन्हित अक्षर *निवेश,
                                   माप_प्रकार inputlen,
                                   अचिन्हित अक्षर *sha256sum,
                                   माप_प्रकार sha256len)
{
  वापस schannel_checksum(निवेश, inputlen, sha256sum, sha256len,
                           PROV_RSA_AES, CALG_SHA_256);
}

अटल व्योम *schannel_get_internals(काष्ठा ssl_connect_तथ्य *connssl,
                                    CURLINFO info)
{
  काष्ठा schannel_ssl_backend_तथ्य *backend =
    (काष्ठा schannel_ssl_backend_तथ्य *)connssl->backend;
  (व्योम)info;
  DEBUGASSERT(backend);
  वापस &backend->ctxt->ctxt_handle;
}

HCERTSTORE Curl_schannel_get_cached_cert_store(काष्ठा Curl_cfilter *cf,
                                               काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  काष्ठा Curl_multi *multi = तथ्य->multi;
  स्थिर काष्ठा curl_blob *ca_info_blob = conn_config->ca_info_blob;
  काष्ठा schannel_cert_share *share;
  timediff_t timeout_ms;
  अचिन्हित अक्षर info_blob_digest[CURL_SHA256_DIGEST_लंबाई];

  DEBUGASSERT(multi);

  अगर(!multi) {
    वापस शून्य;
  }

  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_SCHANNEL_CERT_SHARE_कुंजी,
                         CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_कुंजी));
  अगर(!share || !share->cert_store) {
    वापस शून्य;
  }

  /* zero ca_cache_timeout completely disables caching */
  अगर(!तथ्य->set.ssl_ca_cache_timeout) {
    वापस शून्य;
  }

  /* check for cache timeout by using the cached_x509_store_expired timediff
     calculation pattern from openssl.c.
     negative timeout means retain forever. */
  timeout_ms = तथ्य->set.ssl_ca_cache_timeout * (timediff_t)1000;
  अगर(timeout_ms >= 0) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(तथ्य), &share->समय);
    अगर(elapsed_ms >= timeout_ms) {
      वापस शून्य;
    }
  }

  अगर(ca_info_blob) {
    अगर(share->CAinfo_blob_आकार != ca_info_blob->len) {
      वापस शून्य;
    }
    अगर(schannel_sha256sum((स्थिर अचिन्हित अक्षर *)ca_info_blob->तथ्य,
                          ca_info_blob->len,
                          info_blob_digest,
                          CURL_SHA256_DIGEST_लंबाई))
      वापस शून्य;
    अगर(स_भेद(share->CAinfo_blob_digest, info_blob_digest,
              CURL_SHA256_DIGEST_लंबाई)) {
      वापस शून्य;
    }
  }
  अन्यथा {
    अगर(!conn_config->CAfile || !share->CAfile ||
       म_भेद(share->CAfile, conn_config->CAfile)) {
      वापस शून्य;
    }
  }

  वापस share->cert_store;
}

अटल व्योम schannel_cert_share_free(स्थिर व्योम *कुंजी, माप_प्रकार कुंजी_len, व्योम *p)
{
  काष्ठा schannel_cert_share *share = p;
  DEBUGASSERT(कुंजी_len == CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_कुंजी));
  DEBUGASSERT(!स_भेद(MPROTO_SCHANNEL_CERT_SHARE_कुंजी, कुंजी, कुंजी_len));
  (व्योम)कुंजी;
  (व्योम)कुंजी_len;
  अगर(share->cert_store) {
    CertCloseStore(share->cert_store, 0);
  }
  curlx_free(share->CAfile);
  curlx_free(share);
}

bool Curl_schannel_set_cached_cert_store(काष्ठा Curl_cfilter *cf,
                                         काष्ठा Curl_easy *तथ्य,
                                         HCERTSTORE cert_store)
{
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  काष्ठा Curl_multi *multi = तथ्य->multi;
  स्थिर काष्ठा curl_blob *ca_info_blob = conn_config->ca_info_blob;
  काष्ठा schannel_cert_share *share;
  अचिन्हित अक्षर digest[CURL_SHA256_DIGEST_लंबाई];
  अक्षर *CAfile = शून्य;

  DEBUGASSERT(multi);

  अगर(!multi) {
    वापस FALSE;
  }

  अगर(ca_info_blob) {
    अगर(schannel_sha256sum((स्थिर अचिन्हित अक्षर *)ca_info_blob->तथ्य,
                          ca_info_blob->len, digest, माप(digest))) {
      वापस FALSE;
    }
  }
  अन्यथा अगर(conn_config->CAfile) {
    CAfile = curlx_strdup(conn_config->CAfile);
    अगर(!CAfile) {
      वापस FALSE;
    }
  }

  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_SCHANNEL_CERT_SHARE_कुंजी,
                         CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_कुंजी));
  अगर(!share) {
    share = curlx_calloc(1, माप(*share));
    अगर(!share) {
      curlx_free(CAfile);
      वापस FALSE;
    }
    अगर(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_SCHANNEL_CERT_SHARE_कुंजी,
                       CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_कुंजी),
                       share, schannel_cert_share_free)) {
      curlx_free(share);
      curlx_free(CAfile);
      वापस FALSE;
    }
  }

  /* free old cache data */
  अगर(share->cert_store) {
    CertCloseStore(share->cert_store, 0);
  }
  curlx_free(share->CAfile);

  अगर(ca_info_blob) {
    स_नकल(share->CAinfo_blob_digest, digest, माप(digest));
    share->CAinfo_blob_आकार = ca_info_blob->len;
  }
  अन्यथा
    share->CAinfo_blob_आकार = 0;

  share->समय = curlx_now();
  share->cert_store = cert_store;
  share->CAfile = CAfile;
  वापस TRUE;
}

स्थिर काष्ठा Curl_ssl Curl_ssl_schannel = {
  { CURLSSLBACKEND_SCHANNEL, "schannel" }, /* info */

  SSLSUPP_CERTINFO |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_CA_CACHE |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST,

  माप(काष्ठा schannel_ssl_backend_तथ्य),

  schannel_init,                     /* init */
  schannel_cleanup,                  /* cleanup */
  schannel_version,                  /* version */
  schannel_shutdown,                 /* shutdown */
  schannel_तथ्य_pending,             /* data_pending */
  schannel_random,                   /* random */
  शून्य,                              /* cert_status_request */
  schannel_connect,                  /* connect */
  Curl_ssl_adjust_pollset,           /* adjust_pollset */
  schannel_get_internals,            /* get_internals */
  schannel_close,                    /* close_one */
  शून्य,                              /* close_all */
  शून्य,                              /* engines_list */
  schannel_sha256sum,                /* sha256sum */
  schannel_recv,                     /* recv decrypted data */
  schannel_send,                     /* send data to encrypt */
  शून्य,                              /* get_channel_binding */
};

#पूर्ण_अगर /* USE_SCHANNEL */
