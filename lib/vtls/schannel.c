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
#অন্তর্ভাব "curl_setup.h"

#ifdef USE_SCHANNEL

#ifndef USE_WINDOWS_SSPI
#error "cannot compile Schannel support without SSPI."
#endif

#অন্তর্ভাব "vtls/schannel.h"
#অন্তর্ভাব "vtls/schannel_int.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "vtls/vtls_int.h"
#অন্তর্ভাব "vtls/vtls_scache.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "connect.h" /* for the connect timeout */
#অন্তর্ভাব "curlx/strdup.h"
#অন্তর্ভাব "strerror.h"
#অন্তর্ভাব "select.h" /* for the socket readiness */
#অন্তর্ভাব "curlx/win32-fopen.h"
#অন্তর্ভাব "curlx/multibyte.h"
#অন্তর্ভাব "vtls/x509asn1.h"
#অন্তর্ভাব "curlx/version_win32.h"
#অন্তর্ভাব "curlx/winapi.h"
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "curl_sha256.h"

/* Some verbose debug messages are wrapped by SCH_DEV() instead of DEBUGF()
 * and only shown if CURL_SCHANNEL_DEV_DEBUG was defined at build time. These
 * messages are extra verbose and intended for curl developers debugging
 * Schannel recv decryption and renegotiation.
 */
#ifdef CURL_SCHANNEL_DEV_DEBUG
#define SCH_DEV(x) x
#define SCH_DEV_SHOWBOOL(x)                                     \
  কুরু {                                                          \
    infof(data, "schannel: " #x " %s", (x) ? "TRUE" : "FALSE"); \
  } যাবত্(0)
#অন্যথা
#define SCH_DEV(x) কুরু {} যাবত্(0)
#define SCH_DEV_SHOWBOOL(x) কুরু {} যাবত্(0)
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
#ifndef PKCS12_NO_PERSIST_চাবি
#define PKCS12_NO_PERSIST_চাবি 0x00008000
#endif

/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#ifndef CERT_FIND_HAS_PRIVATE_চাবি
#define CERT_FIND_HAS_PRIVATE_চাবি (21 << CERT_COMPARE_SHIFT)
#endif

/* key to use at `multi->proto_hash` */
#define MPROTO_SCHANNEL_CERT_SHARE_চাবি   "tls:schannel:cert:share"

স্থির bool s_win_has_alpn;

/* Offered by mingw-w64 v9+, MS SDK 8.1/VS2013+ */
#ifndef SECBUFFER_APPLICATION_PROTOCOLS
#define SECBUFFER_APPLICATION_PROTOCOLS 18
#define SECPKG_ATTR_APPLICATION_PROTOCOL 35

প্রকারনির্দেশ গণনা {
  SecApplicationProtocolNegotiationExt_None,
  SecApplicationProtocolNegotiationExt_NPN,
  SecApplicationProtocolNegotiationExt_ALPN
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT;

প্রকারনির্দেশ গণনা {
  SecApplicationProtocolNegotiationStatus_None,
  SecApplicationProtocolNegotiationStatus_Success,
  SecApplicationProtocolNegotiationStatus_SelectedClientOnly
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_STATUS;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
প্রকারনির্দেশ সংরচনা {
  SEC_APPLICATION_PROTOCOL_NEGOTIATION_STATUS ProtoNegoStatus;
  SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT ProtoNegoExt;
  unsigned বর্ণ ProtocolIdSize;
  unsigned বর্ণ ProtocolId[0xff];
} SecPkgContext_ApplicationProtocol;
#endif

স্থির শূন্য InitSecBuffer(SecBuffer *buffer, unsigned long BufType,
                          শূন্য *BufDataPtr, unsigned long BufByteSize)
{
  buffer->cbBuffer = BufByteSize;
  buffer->BufferType = BufType;
  buffer->pvBuffer = BufDataPtr;
}

স্থির শূন্য InitSecBufferDesc(SecBufferDesc *desc, SecBuffer *BufArr,
                              unsigned long অঙ্ক_Arr_Elem)
{
  desc->ulVersion = SECBUFFER_VERSION;
  desc->pBuffers = BufArr;
  desc->cBuffers = অঙ্ক_Arr_Elem;
}

স্থির CURLcode schannel_set_ssl_version_min_max(DWORD *enabled_protocols,
                                                 সংরচনা Curl_cfilter *cf,
                                                 সংরচনা Curl_easy *data)
{
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  long ssl_version = conn_config->version;
  long ssl_version_max = (long)conn_config->version_max;
  long i = ssl_version;

  বিকল্প(ssl_version_max) {
  পক্ষ CURL_SSLVERSION_MAX_NONE:
  পক্ষ CURL_SSLVERSION_MAX_DEFAULT:

    /* Windows Server 2022 and newer (including Windows 11) support TLS 1.3
       built-in. Previous builds of Windows 10 had broken TLS 1.3
       implementations that could be enabled via registry. */
    যদি(curlx_verify_windows_version(10, 0, 20348, PLATFORM_WINNT,
                                    VERSION_GREATER_THAN_EQUAL)) {
      ssl_version_max = CURL_SSLVERSION_MAX_TLSv1_3;
    }
    অন্যথা /* Windows 10 or older */
      ssl_version_max = CURL_SSLVERSION_MAX_TLSv1_2;

    ভঙ্গ;
  }

  প্রতি(; i <= (ssl_version_max >> 16); ++i) {
    বিকল্প(i) {
    পক্ষ CURL_SSLVERSION_TLSv1_0:
      *enabled_protocols |= SP_PROT_TLS1_0_CLIENT;
      ভঙ্গ;
    পক্ষ CURL_SSLVERSION_TLSv1_1:
      *enabled_protocols |= SP_PROT_TLS1_1_CLIENT;
      ভঙ্গ;
    পক্ষ CURL_SSLVERSION_TLSv1_2:
      *enabled_protocols |= SP_PROT_TLS1_2_CLIENT;
      ভঙ্গ;
    পক্ষ CURL_SSLVERSION_TLSv1_3:

      /* Windows Server 2022 or newer */
      যদি(curlx_verify_windows_version(10, 0, 20348, PLATFORM_WINNT,
                                      VERSION_GREATER_THAN_EQUAL)) {
        *enabled_protocols |= SP_PROT_TLS1_3_CLIENT;
        ভঙ্গ;
      }
      অন্যথা { /* Windows 10 or older */
        failf(data, "schannel: TLS 1.3 not supported on Windows 10 or older");
        প্রত্যাগম CURLE_SSL_CONNECT_ERROR;
      }
    }
  }
  প্রত্যাগম CURLE_OK;
}

#define CIPHEROPTION(x) { #x, x }

সংরচনা algo {
  ধ্রুব বর্ণ *name;
  পূর্ণাঙ্ক id;
};

স্থির ধ্রুব সংরচনা algo algs[] = {
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
  CIPHEROPTION(CALG_SCHANNEL_MAC_চাবি),
  CIPHEROPTION(CALG_SCHANNEL_ENC_চাবি),
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
  { রিক্ত, 0 },
};

স্থির পূর্ণাঙ্ক get_alg_id_by_name(ধ্রুব বর্ণ *name)
{
  ধ্রুব বর্ণ *nameEnd = strchr(name, ':');
  size_t n = nameEnd ? (size_t)(nameEnd - name) : strlen(name);
  পূর্ণাঙ্ক i;

  প্রতি(i = 0; algs[i].name; i++) {
    যদি((n == strlen(algs[i].name) && !strncmp(algs[i].name, name, n)))
      প্রত্যাগম algs[i].id;
  }
  প্রত্যাগম 0; /* not found */
}

#define অঙ্ক_CIPHERS 47 /* There are a maximum of 47 options listed above */

স্থির CURLcode set_ssl_ciphers(SCHANNEL_CRED *schannel_cred, বর্ণ *ciphers,
                                ALG_ID *algIds)
{
  ধ্রুব বর্ণ *startCur = ciphers;
  পূর্ণাঙ্ক alg_গুনতি = 0;
  যাবত্(startCur && *startCur && (alg_গুনতি < অঙ্ক_CIPHERS)) {
    curl_off_t alg;
    যদি(curlx_str_সংখ্যা(&startCur, &alg, INT_MAX) || !alg)
      alg = get_alg_id_by_name(startCur);

    যদি(alg)
      algIds[alg_গুনতি++] = (ALG_ID)alg;
    অন্যথা যদি(!strncmp(startCur, "USE_STRONG_CRYPTO",
                     CURL_CSTRLEN("USE_STRONG_CRYPTO")) ||
            !strncmp(startCur, "SCH_USE_STRONG_CRYPTO",
                     CURL_CSTRLEN("SCH_USE_STRONG_CRYPTO")))
      schannel_cred->dwFlags |= SCH_USE_STRONG_CRYPTO;
    অন্যথা
      প্রত্যাগম CURLE_SSL_CIPHER;
    startCur = strchr(startCur, ':');
    যদি(startCur)
      startCur++;
  }
  schannel_cred->palgSupportedAlgs = algIds;
  schannel_cred->cSupportedAlgs = (DWORD)alg_গুনতি;
  প্রত্যাগম CURLE_OK;
}

/* Function allocates memory for store_path only if CURLE_OK is returned */
স্থির CURLcode get_cert_location(TCHAR *path, DWORD *store_name,
                                  TCHAR **store_path, TCHAR **thumbprint)
{
  TCHAR *sep;
  TCHAR *store_path_start;
  size_t store_name_len;

  sep = _tcschr(path, _TEXT('\\'));
  যদি(!sep)
    প্রত্যাগম CURLE_SSL_CERTPROBLEM;

  store_name_len = sep - path;

  যদি(_tcsncmp(path, _TEXT("CurrentUser"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_USER;
  অন্যথা যদি(_tcsncmp(path, _TEXT("LocalMachine"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE;
  অন্যথা যদি(_tcsncmp(path, _TEXT("CurrentService"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_SERVICE;
  অন্যথা যদি(_tcsncmp(path, _TEXT("Services"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_SERVICES;
  অন্যথা যদি(_tcsncmp(path, _TEXT("Users"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_USERS;
  অন্যথা যদি(_tcsncmp(path, _TEXT("CurrentUserGroupPolicy"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_USER_GROUP_POLICY;
  অন্যথা যদি(_tcsncmp(path, _TEXT("LocalMachineGroupPolicy"), store_name_len) ==
          0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE_GROUP_POLICY;
  অন্যথা যদি(_tcsncmp(path, _TEXT("LocalMachineEnterprise"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE_ENTERPRISE;
  অন্যথা
    প্রত্যাগম CURLE_SSL_CERTPROBLEM;

  store_path_start = sep + 1;

  sep = _tcschr(store_path_start, _TEXT('\\'));
  যদি(!sep)
    প্রত্যাগম CURLE_SSL_CERTPROBLEM;

  *thumbprint = sep + 1;
  যদি(_tcslen(*thumbprint) != CERT_THUMBPRINT_STR_LEN)
    প্রত্যাগম CURLE_SSL_CERTPROBLEM;

  *sep = _TEXT('\0');
  *store_path = curlx_tcsdup(store_path_start);
  *sep = _TEXT('\\');
  যদি(!*store_path)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode get_client_cert(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data,
                                HCERTSTORE *out_cert_store,
                                PCCERT_CONTEXT *out_cert_context)
{
  সংরচনা ssl_filter_config *sslc = Curl_ssl_cf_get_filter_config(cf);
  PCCERT_CONTEXT client_cert = রিক্ত;
  HCERTSTORE client_cert_store = রিক্ত;
  CURLcode result = CURLE_OK;

  যদি(sslc->clientcert || sslc->cert_blob) {
    DWORD cert_store_name = 0;
    TCHAR *cert_store_path = রিক্ত;
    TCHAR *cert_thumbprint_str = রিক্ত;
    TCHAR cert_thumbprint_buf[CERT_THUMBPRINT_STR_LEN + 1];
    CRYPT_HASH_BLOB cert_thumbprint;
    BYTE cert_thumbprint_data[CERT_THUMBPRINT_DATA_LEN];
    HCERTSTORE cert_store = রিক্ত;
    FILE *fInCert = রিক্ত;
    শূন্য *certdata = রিক্ত;
    size_t certsize = 0;
    bool blob = !!sslc->cert_blob;

    যদি(blob) {
      certdata = sslc->cert_blob->data;
      certsize = sslc->cert_blob->len;
    }
    অন্যথা {
      TCHAR *cert_path =
        curlx_convert_UTF8_to_tchar(sslc->clientcert);
      যদি(!cert_path)
        প্রত্যাগম CURLE_OUT_OF_MEMORY;

      result = get_cert_location(cert_path, &cert_store_name,
                                 &cert_store_path, &cert_thumbprint_str);

      /* 'cert_thumbprint_str' points in to the allocated 'cert_path', copy
         the data. The string is verified to be CERT_THUMBPRINT_STR_LEN bytes
         long within the get_cert_location() function. */
      যদি(!result && cert_thumbprint_str) {
        memcpy(cert_thumbprint_buf, cert_thumbprint_str,
               পরিমাণ(cert_thumbprint_buf));
        cert_thumbprint_str = cert_thumbprint_buf;
      }

      curlx_free(cert_path);
      যদি(result && (sslc->clientcert[0] != '\0'))
        fInCert = curlx_fopen(sslc->clientcert, "rb");

      যদি(result && !fInCert) {
        failf(data, "schannel: Failed to get certificate location"
              " or file for %s",
              sslc->clientcert);
        প্রত্যাগম result;
      }
    }

    যদি((fInCert || blob) && sslc->cert_type &&
       !curl_strequal(sslc->cert_type, "P12")) {
      failf(data, "schannel: certificate format compatibility error "
            "for %s",
            blob ? "(memory blob)" : sslc->clientcert);
      curlx_free(cert_store_path);
      যদি(fInCert)
        curlx_fclose(fInCert);
      প্রত্যাগম CURLE_SSL_CERTPROBLEM;
    }

    যদি(fInCert || blob) {
      /* Reading a .p12 or .pfx file, like the example at bottom of
         https://learn.microsoft.com/archive/msdn-technet-forums/3e7bc95f-b21a-4bcd-bd2c-7f996718cae5
       */
      CRYPT_DATA_BLOB datablob;
      WCHAR *pszPassword;
      size_t pwd_len = 0;
      পূর্ণাঙ্ক cert_find_flags;
      ধ্রুব বর্ণ *cert_showfilename_error = blob ?
        "(memory blob)" : sslc->clientcert;
      curlx_free(cert_store_path);
      যদি(fInCert) {
        long cert_tell = 0;
        bool continue_reading = fseek(fInCert, 0, SEEK_END) == 0;
        যদি(continue_reading) {
          cert_tell = ftell(fInCert);
          যদি(cert_tell < 0)
            continue_reading = FALSE;
          অন্যথা
            certsize = (size_t)cert_tell;
        }
        যদি(continue_reading)
          continue_reading = fseek(fInCert, 0, SEEK_SET) == 0;
        যদি(continue_reading && (certsize < CURL_MAX_প্রবেশ_LENGTH))
          certdata = curlx_malloc(certsize + 1);
        যদি(!certdata || ((পূর্ণাঙ্ক)fread(certdata, certsize, 1, fInCert) != 1))
          continue_reading = FALSE;
        curlx_fclose(fInCert);
        যদি(!continue_reading) {
          failf(data, "schannel: Failed to read cert file %s",
                sslc->clientcert);
          curlx_free(certdata);
          প্রত্যাগম CURLE_SSL_CERTPROBLEM;
        }
      }

      /* Convert key-pair data to the in-memory certificate store */
      datablob.pbData = (BYTE *)certdata;
      datablob.cbData = (DWORD)certsize;

      যদি(sslc->চাবি_passwd)
        pwd_len = strlen(sslc->চাবি_passwd);
      pszPassword = curlx_malloc(পরিমাণ(WCHAR) * (pwd_len + 1));
      যদি(pszPassword) {
        পূর্ণাঙ্ক str_w_len = 0;
        যদি(pwd_len > 0)
          str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                          sslc->চাবি_passwd, (পূর্ণাঙ্ক)pwd_len,
                                          pszPassword, (পূর্ণাঙ্ক)(pwd_len + 1));

        যদি((str_w_len >= 0) && (str_w_len <= (পূর্ণাঙ্ক)pwd_len))
          pszPassword[str_w_len] = 0;
        অন্যথা
          pszPassword[0] = 0;

        cert_store = PFXImportCertStore(&datablob, pszPassword,
                                        PKCS12_NO_PERSIST_চাবি);
        curlx_memzero(pszPassword, পরিমাণ(WCHAR) * (pwd_len + 1));
        curlx_free(pszPassword);
      }
      যদি(!blob)
        curlx_free(certdata);
      যদি(!cert_store) {
        DWORD errorcode = GetLastError();
        যদি(errorcode == ERROR_INVALID_PASSWORD)
          failf(data, "schannel: Failed to import cert file %s, "
                "password is bad",
                cert_showfilename_error);
        অন্যথা
          failf(data, "schannel: Failed to import cert file %s, "
                "last error is 0x%08lx",
                cert_showfilename_error, errorcode);
        প্রত্যাগম CURLE_SSL_CERTPROBLEM;
      }

      /* CERT_FIND_HAS_PRIVATE_KEY is only available in Windows 8 / Server
         2012, (NT 6.2). For older versions we use CURL_FIND_ANY. */
      যদি(curlx_verify_windows_version(6, 2, 0, PLATFORM_WINNT,
                                      VERSION_GREATER_THAN_EQUAL))
        cert_find_flags = CERT_FIND_HAS_PRIVATE_চাবি;
      অন্যথা
        cert_find_flags = CERT_FIND_ANY;

      client_cert =
        CertFindCertificateInStore(cert_store,
                                   X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                   cert_find_flags, রিক্ত, রিক্ত);

      যদি(!client_cert) {
        failf(data, "schannel: Failed to get certificate from file %s"
              ", last error is 0x%08lx",
              cert_showfilename_error, GetLastError());
        CertCloseStore(cert_store, 0);
        প্রত্যাগম CURLE_SSL_CERTPROBLEM;
      }
    }
    অন্যথা {
      cert_store =
        CertOpenStore(
#ifdef UNICODE
                      CERT_STORE_PROV_SYSTEM_W,
#অন্যথা
                      CERT_STORE_PROV_SYSTEM_A,
#endif
                      0,
                      (HCRYPTPROV)রিক্ত,
                      CERT_STORE_OPEN_EXISTING_FLAG | cert_store_name,
                      cert_store_path);
      যদি(!cert_store) {
        বর্ণ *path_utf8 =
          curlx_convert_tchar_to_UTF8(cert_store_path);
        failf(data, "schannel: Failed to open cert store %lx %s, "
              "last error is 0x%08lx",
              cert_store_name, (path_utf8 ? path_utf8 : "(unknown)"),
              GetLastError());
        curlx_free(cert_store_path);
        curlx_free(path_utf8);
        প্রত্যাগম CURLE_SSL_CERTPROBLEM;
      }
      curlx_free(cert_store_path);

      cert_thumbprint.pbData = cert_thumbprint_data;
      cert_thumbprint.cbData = CERT_THUMBPRINT_DATA_LEN;

      যদি(!Crypt_String_To_দ্বিমিক(cert_thumbprint_str,
                              CERT_THUMBPRINT_STR_LEN,
                              CRYPT_STRING_HEX,
                              cert_thumbprint_data,
                              &cert_thumbprint.cbData,
                              রিক্ত, রিক্ত)) {
        CertCloseStore(cert_store, 0);
        প্রত্যাগম CURLE_SSL_CERTPROBLEM;
      }

      client_cert =
        CertFindCertificateInStore(cert_store,
                                   X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                   CERT_FIND_HASH, &cert_thumbprint, রিক্ত);
      যদি(!client_cert) {
        /* CRYPT_E_NOT_FOUND / E_INVALIDARG */
        CertCloseStore(cert_store, 0);
        failf(data, "schannel: client cert not found in cert store");
        প্রত্যাগম CURLE_SSL_CERTPROBLEM;
      }
    }
    client_cert_store = cert_store;
  }

  *out_cert_store = client_cert_store;
  *out_cert_context = client_cert;

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode acquire_sspi_handle(সংরচনা Curl_cfilter *cf,
                                    সংরচনা Curl_easy *data,
                                    সংরচনা schannel_ssl_backend_data *backend,
                                    PCCERT_CONTEXT client_cert,
                                    DWORD flags,
                                    DWORD enabled_protocols)
{
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CURLcode result;

  /* We support TLS 1.3 starting in Windows Server 2022 or later
   * (OS build 20348) */
  যদি(!conn_config->cipher_list &&
     curlx_verify_windows_version(10, 0, 20348, PLATFORM_WINNT,
                                  VERSION_GREATER_THAN_EQUAL)) {

    SCH_CREDENTIALS credentials = { 0 };
    TLS_PARAMETERS tls_parameters = { 0 };
    CRYPTO_SETTINGS crypto_settings[1];
    PCCERT_CONTEXT client_certs[1];

    যদি(client_cert)
      client_certs[0] = client_cert;
    অন্যথা
      client_certs[0] = রিক্ত;

    memset(crypto_settings, 0, পরিমাণ(crypto_settings));

    tls_parameters.pDisabledCrypto = crypto_settings;

    /* The number of blocked suites */
    tls_parameters.cDisabledCrypto = (DWORD)0;
    credentials.pTlsParameters = &tls_parameters;
    credentials.cTlsParameters = 1;

    credentials.dwVersion = SCH_CREDENTIALS_VERSION;
    credentials.dwFlags = flags | SCH_USE_STRONG_CRYPTO;

    credentials.pTlsParameters->grbitDisabledProtocols = ~enabled_protocols;

    যদি(client_certs[0]) {
      credentials.cCreds = 1;
      credentials.paCred = client_certs;
    }

    sspi_status =
      Curl_pSecFn->AcquireCredentialsHandle(রিক্ত, CURL_UNCONST(UNISP_NAME),
                                            SECPKG_CRED_OUTBOUND, রিক্ত,
                                            &credentials, রিক্ত, রিক্ত,
                                            &backend->cred->cred_handle, রিক্ত);
  }
  অন্যথা {
    /* Pre-Windows 10 1809 or the user set a legacy algorithm list.
       Schannel does not negotiate TLS 1.3 when SCHANNEL_CRED is used. */
    ALG_ID algIds[অঙ্ক_CIPHERS];
    বর্ণ *ciphers = conn_config->cipher_list;
    SCHANNEL_CRED schannel_cred = { 0 };
    PCCERT_CONTEXT client_certs[1];

    যদি(client_cert)
      client_certs[0] = client_cert;
    অন্যথা
      client_certs[0] = রিক্ত;

    schannel_cred.dwVersion = SCHANNEL_CRED_VERSION;
    schannel_cred.dwFlags = flags;
    schannel_cred.grbitEnabledProtocols = enabled_protocols;

    যদি(ciphers) {
      যদি((enabled_protocols & SP_PROT_TLS1_3_CLIENT)) {
        যদি(!(enabled_protocols & ~SP_PROT_TLS1_3_CLIENT)) {
          failf(data, "schannel: TLS 1.3 is not supported with a cipher list; "
                "remove the cipher list or allow a lower TLS version");
          প্রত্যাগম CURLE_SSL_CONNECT_ERROR;
        }
        infof(data, "schannel: WARNING: This version of Schannel "
              "negotiates a less-secure TLS version than TLS 1.3 because the "
              "user set an algorithm cipher list.");
      }
      result = set_ssl_ciphers(&schannel_cred, ciphers, algIds);
      যদি(result) {
        failf(data, "schannel: Failed setting algorithm cipher list");
        প্রত্যাগম result;
      }
    }
    অন্যথা {
      schannel_cred.dwFlags = flags | SCH_USE_STRONG_CRYPTO;
    }

    যদি(client_certs[0]) {
      schannel_cred.cCreds = 1;
      schannel_cred.paCred = client_certs;
    }

    sspi_status =
      Curl_pSecFn->AcquireCredentialsHandle(রিক্ত, CURL_UNCONST(UNISP_NAME),
                                            SECPKG_CRED_OUTBOUND, রিক্ত,
                                            &schannel_cred, রিক্ত, রিক্ত,
                                            &backend->cred->cred_handle, রিক্ত);
  }

  যদি(sspi_status != SEC_E_OK) {
    বর্ণ buffer[STRERROR_LEN];
    failf(data, "schannel: AcquireCredentialsHandle failed: %s",
          Curl_sspi_strerror(sspi_status, buffer, পরিমাণ(buffer)));
    বিকল্প(sspi_status) {
    পক্ষ SEC_E_INSUFFICIENT_MEMORY:
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    পক্ষ SEC_E_NO_CREDENTIALS:
    পক্ষ SEC_E_SECPKG_NOT_FOUND:
    পক্ষ SEC_E_NOT_OWNER:
    পক্ষ SEC_E_UNKNOWN_CREDENTIALS:
    পক্ষ SEC_E_INTERNAL_ERROR:
    স্বভাব:
      প্রত্যাগম CURLE_SSL_CONNECT_ERROR;
    }
  }

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode schannel_acquire_credential_handle(সংরচনা Curl_cfilter *cf,
                                                   সংরচনা Curl_easy *data)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  PCCERT_CONTEXT client_cert = রিক্ত;
  HCERTSTORE client_cert_store = রিক্ত;
  CURLcode result;

  /* setup Schannel API options */
  DWORD flags = 0;
  DWORD enabled_protocols = 0;

  সংরচনা schannel_ssl_backend_data *backend =
    (সংরচনা schannel_ssl_backend_data *)connssl->backend;

  DEBUGASSERT(backend);

  যদি(conn_config->verifypeer) {
    যদি(backend->use_manual_cred_validation)
      flags = SCH_CRED_MANUAL_CRED_VALIDATION;
    অন্যথা
      flags = SCH_CRED_AUTO_CRED_VALIDATION;

    যদি(conn_config->no_revoke) {
      flags |= SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
               SCH_CRED_IGNORE_REVOCATION_OFFLINE;

      DEBUGF(infof(data, "schannel: disabled server certificate revocation "
                         "checks"));
    }
    অন্যথা যদি(conn_config->revoke_best_effort) {
      flags |= SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
               SCH_CRED_IGNORE_REVOCATION_OFFLINE |
               SCH_CRED_REVOCATION_CHECK_CHAIN;

      DEBUGF(infof(data, "schannel: ignore revocation offline errors"));
    }
    অন্যথা {
      flags |= SCH_CRED_REVOCATION_CHECK_CHAIN;

      DEBUGF(infof(data, "schannel: checking server certificate revocation"));
    }
  }
  অন্যথা {
    flags = SCH_CRED_MANUAL_CRED_VALIDATION |
            SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
            SCH_CRED_IGNORE_REVOCATION_OFFLINE;
    DEBUGF(infof(data, "schannel: disabled server cert revocation checks"));
  }

  যদি(!conn_config->verifyhost) {
    flags |= SCH_CRED_NO_SERVERNAME_CHECK;
    DEBUGF(infof(data, "schannel: verifyhost setting prevents Schannel from "
                       "comparing the supplied target name with the subject "
                       "names in server certificates."));
  }

  যদি(!conn_config->auto_client_cert) {
    flags &= ~(DWORD)SCH_CRED_USE_DEFAULT_CREDS;
    flags |= SCH_CRED_NO_DEFAULT_CREDS;
    infof(data, "schannel: disabled automatic use of client certificate");
  }
  অন্যথা
    infof(data, "schannel: enabled automatic use of client certificate");

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  বিকল্প(conn_config->version) {
  পক্ষ CURL_SSLVERSION_TLSv1:
  পক্ষ CURL_SSLVERSION_TLSv1_0:
  পক্ষ CURL_SSLVERSION_TLSv1_1:
  পক্ষ CURL_SSLVERSION_TLSv1_2:
  পক্ষ CURL_SSLVERSION_TLSv1_3: {
    result = schannel_set_ssl_version_min_max(&enabled_protocols, cf, data);
    যদি(result)
      প্রত্যাগম result;
    ভঙ্গ;
  }
  পক্ষ CURL_SSLVERSION_SSLv3:
  পক্ষ CURL_SSLVERSION_SSLv2:
    failf(data, "SSL versions not supported");
    প্রত্যাগম CURLE_NOT_BUILT_IN;
  স্বভাব:
    failf(data, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    প্রত্যাগম CURLE_SSL_CONNECT_ERROR;
  }

  result = get_client_cert(cf, data, &client_cert_store, &client_cert);
  যদি(result)
    প্রত্যাগম result;

  /* allocate memory for the reusable credential handle */
  backend->cred = (সংরচনা Curl_schannel_cred *)
    curlx_calloc(1, পরিমাণ(সংরচনা Curl_schannel_cred));
  যদি(!backend->cred) {
    failf(data, "schannel: unable to allocate memory");

    যদি(client_cert)
      CertFreeCertificateContext(client_cert);
    যদি(client_cert_store)
      CertCloseStore(client_cert_store, 0);

    প্রত্যাগম CURLE_OUT_OF_MEMORY;
  }
  backend->cred->refcount = 1;

  /* Since we did not persist the key, we need to extend the store's
   * lifetime until the end of the connection
   */
  backend->cred->client_cert_store = client_cert_store;

  result = acquire_sspi_handle(cf, data, backend, client_cert,
                               flags, enabled_protocols);

  যদি(client_cert)
    CertFreeCertificateContext(client_cert);

  প্রত্যাগম result;
}

স্থির CURLcode schannel_connect_step1(সংরচনা Curl_cfilter *cf,
                                       সংরচনা Curl_easy *data)
{
  size_t written = 0;
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা schannel_ssl_backend_data *backend =
    (সংরচনা schannel_ssl_backend_data *)connssl->backend;
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SecBuffer outbuf;
  SecBufferDesc outbuf_desc;
  SecBuffer inbuf;
  SecBufferDesc inbuf_desc;
  unsigned বর্ণ alpn_buffer[128];
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CURLcode result;

  DEBUGASSERT(backend);
  DEBUGF(infof(data, "schannel: SSL/TLS connection with %s port %d (step 1/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->port));

  backend->use_alpn = connssl->alpn && s_win_has_alpn;

  যদি(conn_config->CAfile || conn_config->ca_info_blob) {
    যদি(curlx_verify_windows_version(6, 1, 0, PLATFORM_WINNT,
                                    VERSION_GREATER_THAN_EQUAL)) {
      backend->use_manual_cred_validation = TRUE;
    }
    অন্যথা {
      failf(data, "schannel: this version of Windows is too old to support "
                  "certificate verification via CA bundle file.");
      প্রত্যাগম CURLE_SSL_CACERT_BADFILE;
    }
  }
  অন্যথা
    backend->use_manual_cred_validation = FALSE;

  backend->cred = রিক্ত;

  /* check for an existing reusable credential handle */
  যদি(Curl_ssl_scache_use(cf, data)) {
    সংরচনা Curl_schannel_cred *old_cred;
    Curl_ssl_scache_lock(data);
    old_cred = Curl_ssl_scache_get_obj(cf, data, connssl->peer.scache_চাবি);
    যদি(old_cred) {
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

  যদি(!backend->cred) {
    বর্ণ *snihost;
    result = schannel_acquire_credential_handle(cf, data);
    যদি(result || !backend->cred)
      প্রত্যাগম result;
    /* schannel_acquire_credential_handle() sets backend->cred accordingly or
       it returns error otherwise. */

    /* A hostname associated with the credential is needed by
       InitializeSecurityContext for SNI and other reasons. */
    snihost = connssl->peer.sni ?
      connssl->peer.sni : connssl->peer.origin->hostname;
    backend->cred->sni_hostname = curlx_convert_UTF8_to_tchar(snihost);
    যদি(!backend->cred->sni_hostname)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
  }

  /* Warn if SNI is disabled due to use of an IP address */
  যদি(connssl->peer.type != CURL_SSL_PEER_DNS) {
    infof(data, "schannel: using IP address, SNI is not supported by OS.");
  }

  যদি(backend->use_alpn) {
    পূর্ণাঙ্ক cur = 0;
    পূর্ণাঙ্ক list_start_index = 0;
    unsigned পূর্ণাঙ্ক *extension_len = রিক্ত;
    unsigned short *list_len = রিক্ত;
    ধ্রুব unsigned পূর্ণাঙ্ক uiALPN = SecApplicationProtocolNegotiationExt_ALPN;
    সংরচনা alpn_proto_buf proto;

    /* The first four bytes is an unsigned int indicating number
       of bytes of data in the rest of the buffer. */
    extension_len = (unsigned পূর্ণাঙ্ক *)(শূন্য *)&alpn_buffer[cur];
    cur += (পূর্ণাঙ্ক)পরিমাণ(unsigned পূর্ণাঙ্ক);

    /* The next four bytes are an indicator that this buffer contains
       ALPN data, as opposed to NPN, for example. */
    memcpy(&alpn_buffer[cur], &uiALPN, পরিমাণ(uiALPN));
    cur += (পূর্ণাঙ্ক)পরিমাণ(unsigned পূর্ণাঙ্ক);

    /* The next two bytes is an unsigned short indicating the number
       of bytes used to list the preferred protocols. */
    list_len = (unsigned short *)(শূন্য *)&alpn_buffer[cur];
    cur += (পূর্ণাঙ্ক)পরিমাণ(unsigned short);

    list_start_index = cur;

    result = Curl_alpn_to_proto_buf(&proto, connssl->alpn);
    যদি(result) {
      failf(data, "Error setting ALPN");
      প্রত্যাগম CURLE_SSL_CONNECT_ERROR;
    }
    memcpy(&alpn_buffer[cur], proto.data, proto.len);
    cur += proto.len;

    *list_len = curlx_uitous(cur - list_start_index);
    *extension_len = (unsigned পূর্ণাঙ্ক)(*list_len +
      পরিমাণ(unsigned পূর্ণাঙ্ক) + পরিমাণ(unsigned short));

    InitSecBuffer(&inbuf, SECBUFFER_APPLICATION_PROTOCOLS, alpn_buffer, cur);
    InitSecBufferDesc(&inbuf_desc, &inbuf, 1);

    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
  }
  অন্যথা {
    InitSecBuffer(&inbuf, SECBUFFER_EMPTY, রিক্ত, 0);
    InitSecBufferDesc(&inbuf_desc, &inbuf, 1);
  }

  /* setup output buffer */
  InitSecBuffer(&outbuf, SECBUFFER_EMPTY, রিক্ত, 0);
  InitSecBufferDesc(&outbuf_desc, &outbuf, 1);

  /* security request flags */
  backend->req_flags =
    ISC_REQ_SEQUENCE_DETECT | ISC_REQ_REPLAY_DETECT |
    ISC_REQ_CONFIDENTIALITY | ISC_REQ_ALLOCATE_MEMORY |
    ISC_REQ_STREAM |
    (!conn_config->auto_client_cert ? ISC_REQ_USE_SUPPLIED_CREDS : 0);

  /* allocate memory for the security context handle */
  backend->ctxt = (সংরচনা Curl_schannel_ctxt *)
    curlx_calloc(1, পরিমাণ(সংরচনা Curl_schannel_ctxt));
  যদি(!backend->ctxt) {
    failf(data, "schannel: unable to allocate memory");
    প্রত্যাগম CURLE_OUT_OF_MEMORY;
  }

  /* Schannel InitializeSecurityContext:
     https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-initializesecuritycontextw

     At the moment we do not pass inbuf unless we are using ALPN since we only
     use it for that, and WINE (for which we currently disable ALPN) is giving
     us problems with inbuf regardless. https://github.com/curl/curl/issues/983
   */
  sspi_status = Curl_pSecFn->InitializeSecurityContext(
    &backend->cred->cred_handle, রিক্ত, backend->cred->sni_hostname,
    backend->req_flags, 0, 0,
    (backend->use_alpn ? &inbuf_desc : রিক্ত),
    0, &backend->ctxt->ctxt_handle,
    &outbuf_desc, &backend->ret_flags, রিক্ত);

  যদি(sspi_status != SEC_I_CONTINUE_NEEDED) {
    বর্ণ buffer[STRERROR_LEN];
    curlx_safefree(backend->ctxt);
    বিকল্প(sspi_status) {
    পক্ষ SEC_E_INSUFFICIENT_MEMORY:
      failf(data, "schannel: initial InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(sspi_status, buffer, পরিমাণ(buffer)));
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    পক্ষ SEC_E_WRONG_PRINCIPAL:
      failf(data, "schannel: SNI or certificate check failed: %s",
            Curl_sspi_strerror(sspi_status, buffer, পরিমাণ(buffer)));
      প্রত্যাগম CURLE_PEER_FAILED_VERIFICATION;
#যদি 0
    পক্ষ SEC_E_INVALID_HANDLE:
    পক্ষ SEC_E_INVALID_TOKEN:
    পক্ষ SEC_E_LOGON_DENIED:
    পক্ষ SEC_E_TARGET_UNKNOWN:
    পক্ষ SEC_E_NO_AUTHENTICATING_AUTHORITY:
    পক্ষ SEC_E_INTERNAL_ERROR:
    পক্ষ SEC_E_NO_CREDENTIALS:
    পক্ষ SEC_E_UNSUPPORTED_FUNCTION:
    পক্ষ SEC_E_APPLICATION_PROTOCOL_MISMATCH:
#endif
    স্বভাব:
      failf(data, "schannel: initial InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(sspi_status, buffer, পরিমাণ(buffer)));
      প্রত্যাগম CURLE_SSL_CONNECT_ERROR;
    }
  }

  DEBUGF(infof(data, "schannel: sending initial handshake data: "
               "sending %lu bytes.", outbuf.cbBuffer));

  /* send initial handshake data which is now stored in output buffer */
  result = Curl_conn_cf_send(cf->next, data,
                             (ধ্রুব uint8_t *)outbuf.pvBuffer,
                             outbuf.cbBuffer, FALSE,
                             &written);
  Curl_pSecFn->FreeContextBuffer(outbuf.pvBuffer);
  যদি(result || (outbuf.cbBuffer != written)) {
    failf(data, "schannel: failed to send initial handshake data: "
          "sent %zu of %lu bytes", written, outbuf.cbBuffer);
    প্রত্যাগম CURLE_SSL_CONNECT_ERROR;
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
  memset(&backend->shutdown_token, 0, পরিমাণ(backend->shutdown_token));

  /* continue to second handshake step */
  connssl->connecting_state = ssl_connect_2;

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode schannel_error(সংরচনা Curl_easy *data,
                               SECURITY_STATUS sspi_status)
{
  বর্ণ buffer[STRERROR_LEN];
  বিকল্প(sspi_status) {
  পক্ষ SEC_E_INSUFFICIENT_MEMORY:
    failf(data, "schannel: next InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(sspi_status, buffer, পরিমাণ(buffer)));
    প্রত্যাগম CURLE_OUT_OF_MEMORY;
  পক্ষ SEC_E_WRONG_PRINCIPAL:
    failf(data, "schannel: SNI or certificate check failed: %s",
          Curl_sspi_strerror(sspi_status, buffer, পরিমাণ(buffer)));
    প্রত্যাগম CURLE_PEER_FAILED_VERIFICATION;
  পক্ষ SEC_E_UNTRUSTED_মূল:
    failf(data, "schannel: %s",
          Curl_sspi_strerror(sspi_status, buffer, পরিমাণ(buffer)));
    প্রত্যাগম CURLE_PEER_FAILED_VERIFICATION;
#যদি 0
  পক্ষ SEC_E_INVALID_HANDLE:
  পক্ষ SEC_E_INVALID_TOKEN:
  পক্ষ SEC_E_LOGON_DENIED:
  পক্ষ SEC_E_TARGET_UNKNOWN:
  পক্ষ SEC_E_NO_AUTHENTICATING_AUTHORITY:
  পক্ষ SEC_E_INTERNAL_ERROR:
  পক্ষ SEC_E_NO_CREDENTIALS:
  পক্ষ SEC_E_UNSUPPORTED_FUNCTION:
  পক্ষ SEC_E_APPLICATION_PROTOCOL_MISMATCH:
#endif
  স্বভাব:
    failf(data, "schannel: next InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(sspi_status, buffer, পরিমাণ(buffer)));
    প্রত্যাগম CURLE_SSL_CONNECT_ERROR;
  }
}

স্থির CURLcode schannel_pkp_pin_peer_pubkey(সংরচনা Curl_cfilter *cf,
                                             সংরচনা Curl_easy *data,
                                             ধ্রুব বর্ণ *pinnedpubkey)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা schannel_ssl_backend_data *backend =
    (সংরচনা schannel_ssl_backend_data *)connssl->backend;
  CERT_CONTEXT *pCertContextServer = রিক্ত;

  /* Result is returned to caller */
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  DEBUGASSERT(backend);

  /* if a path was not specified, do not pin */
  যদি(!pinnedpubkey)
    প্রত্যাগম CURLE_OK;

  কুরু {
    SECURITY_STATUS sspi_status;
    uint8_t *x509_der;
    DWORD x509_der_len;
    সংরচনা Curl_X509certificate x509_parsed;
    সংরচনা Curl_asn1Element *pubkey;

    sspi_status =
      Curl_pSecFn->QueryContextAttributes(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                          &pCertContextServer);

    যদি((sspi_status != SEC_E_OK) || !pCertContextServer) {
      বর্ণ buffer[STRERROR_LEN];
      failf(data, "schannel: Failed to read remote certificate context: %s",
            Curl_sspi_strerror(sspi_status, buffer, পরিমাণ(buffer)));
      ভঙ্গ; /* failed */
    }

    যদি(!(((pCertContextServer->dwCertEncodingType & X509_ASN_ENCODING) != 0) &&
         (pCertContextServer->cbCertEncoded > 0)))
      ভঙ্গ;

    x509_der = (uint8_t *)pCertContextServer->pbCertEncoded;
    x509_der_len = pCertContextServer->cbCertEncoded;
    memset(&x509_parsed, 0, পরিমাণ(x509_parsed));
    যদি(Curl_parseX509(&x509_parsed, x509_der, x509_der + x509_der_len))
      ভঙ্গ;

    pubkey = &x509_parsed.subject_Public_চাবি_Info;
    যদি(!pubkey->header || pubkey->end <= pubkey->header) {
      failf(data, "SSL: failed retrieving public key from server certificate");
      ভঙ্গ;
    }

    result = Curl_pin_peer_pubkey(data,
                                  pinnedpubkey,
                                  (ধ্রুব unsigned বর্ণ *)pubkey->header,
                                  (size_t)(pubkey->end - pubkey->header));
    যদি(result) {
      failf(data, "SSL: public key does not match pinned public key");
    }
  } যাবত্(0);

  যদি(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  প্রত্যাগম result;
}

স্থির CURLcode ensure_encoding_size(সংরচনা Curl_easy *data,
                                     সংরচনা sbuffer *encdata,
                                     size_t min_length)
{
  size_t size;
  DEBUGASSERT(encdata->length >= encdata->offset);
  যদি(encdata->length < encdata->offset)
    প্রত্যাগম CURLE_FAILED_INIT;
  size = encdata->length - encdata->offset;
  যদি(size < CURL_SCHANNEL_BUFFER_FREE_SIZE || encdata->length < min_length) {
    unsigned বর্ণ *buffer;
    size_t length = encdata->offset + CURL_SCHANNEL_BUFFER_FREE_SIZE;
    যদি(length < min_length)
      length = min_length;
    buffer = curlx_realloc(encdata->buffer, length);
    যদি(!buffer) {
      failf(data, "schannel: unable to re-allocate memory");
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    }

    encdata->buffer = buffer;
    encdata->length = length;
    SCH_DEV(infof(data, "schannel: encdata.buffer resized %zu",
                  encdata->length));
  }
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode ensure_decoding_size(সংরচনা Curl_easy *data,
                                     সংরচনা sbuffer *decdata,
                                     size_t nowsize,
                                     size_t len)
{
  size_t size = nowsize > CURL_SCHANNEL_BUFFER_FREE_SIZE ?
    nowsize : CURL_SCHANNEL_BUFFER_FREE_SIZE;
  DEBUGASSERT(decdata->length >= decdata->offset);
  যদি(decdata->length < decdata->offset)
    প্রত্যাগম CURLE_FAILED_INIT;
  অন্যথা যদি(decdata->length - decdata->offset < size ||
          decdata->length < len) {
    /* increase internal decrypted data buffer */
    size_t length = decdata->offset + size;
    unsigned বর্ণ *buffer;
    /* make sure that the requested amount of data fits */
    যদি(length < len)
      length = len;

    buffer = curlx_realloc(decdata->buffer, length);
    যদি(!buffer) {
      failf(data, "schannel: unable to re-allocate memory");
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    }
    decdata->buffer = buffer;
    decdata->length = length;
  }
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode schannel_connect_step2(সংরচনা Curl_cfilter *cf,
                                       সংরচনা Curl_easy *data)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা schannel_ssl_backend_data *backend =
    (সংরচনা schannel_ssl_backend_data *)connssl->backend;
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  পূর্ণাঙ্ক i;
  size_t nread = 0;
  SecBuffer outbuf[3];
  SecBufferDesc outbuf_desc;
  SecBuffer inbuf[2];
  SecBufferDesc inbuf_desc;
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CURLcode result;
  bool doread;
  ধ্রুব বর্ণ *pubkey_ptr;

  DEBUGASSERT(backend);

  doread = (connssl->io_need & CURL_SSL_IO_NEED_SEND) ? FALSE : TRUE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  DEBUGF(infof(data, "schannel: SSL/TLS connection with %s port %d (step 2/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->port));

  যদি(!backend->cred || !backend->ctxt)
    প্রত্যাগম CURLE_SSL_CONNECT_ERROR;

  /* buffer to store previously received and decrypted data */
  যদি(!backend->decdata.buffer) {
    backend->decdata.offset = 0;
    backend->decdata.length = CURL_SCHANNEL_BUFFER_INIT_SIZE;
    backend->decdata.buffer = curlx_malloc(backend->decdata.length);
    যদি(!backend->decdata.buffer) {
      failf(data, "schannel: unable to allocate memory");
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    }
  }

  /* buffer to store previously received and encrypted data */
  যদি(!backend->encdata.buffer) {
    backend->encdata_is_incomplete = FALSE;
    backend->encdata.offset = 0;
    backend->encdata.length = CURL_SCHANNEL_BUFFER_INIT_SIZE;
    backend->encdata.buffer = curlx_malloc(backend->encdata.length);
    যদি(!backend->encdata.buffer) {
      failf(data, "schannel: unable to allocate memory");
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    }
  }

  result = ensure_encoding_size(data, &backend->encdata, 0);
  যদি(result)
    প্রত্যাগম result;

  প্রতি(;;) {
    যদি(doread) {
      /* read encrypted handshake data from socket */
      result = Curl_conn_cf_recv(cf->next, data,
                                 (বর্ণ *)(backend->encdata.buffer +
                                          backend->encdata.offset),
                                 backend->encdata.length -
                                 backend->encdata.offset,
                                 &nread);
      যদি(result == CURLE_AGAIN) {
        যদি(!backend->encdata.offset || backend->encdata_is_incomplete) {
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          DEBUGF(infof(data, "schannel: failed to receive handshake, "
                       "need more data"));
          প্রত্যাগম CURLE_OK;
        }
        অন্যথা {
          DEBUGF(infof(data, "schannel: no new handshake data received, "
                       "continuing to process existing handshake data"));
        }
      }
      অন্যথা যদি(result || (nread == 0)) {
        failf(data, "schannel: failed to receive handshake, "
              "SSL/TLS connection failed");
        প্রত্যাগম CURLE_SSL_CONNECT_ERROR;
      }
      অন্যথা {
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
                  curlx_memdup0((ধ্রুব বর্ণ *)backend->encdata.buffer,
                                backend->encdata.offset),
                  curlx_uztoul(backend->encdata.offset));
    InitSecBuffer(&inbuf[1], SECBUFFER_EMPTY, রিক্ত, 0);
    InitSecBufferDesc(&inbuf_desc, inbuf, 2);

    /* setup output buffers */
    InitSecBuffer(&outbuf[0], SECBUFFER_TOKEN, রিক্ত, 0);
    InitSecBuffer(&outbuf[1], SECBUFFER_ALERT, রিক্ত, 0);
    InitSecBuffer(&outbuf[2], SECBUFFER_EMPTY, রিক্ত, 0);
    InitSecBufferDesc(&outbuf_desc, outbuf, 3);

    যদি(!inbuf[0].pvBuffer) {
      failf(data, "schannel: unable to allocate memory");
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    }

    /* The socket must be writable (or a poll error occurred) before we call
       InitializeSecurityContext to continue processing the received TLS
       records. This is because that function is not idempotent and we do not
       support partial save/resume sending replies of handshake tokens. */
    যদি(!SOCKET_WRITABLE(Curl_conn_cf_get_socket(cf, data), 0)) {
      SCH_DEV(infof(data, "schannel: handshake waiting for writable socket"));
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      curlx_free(inbuf[0].pvBuffer);
      প্রত্যাগম CURLE_OK;
    }

    sspi_status = Curl_pSecFn->InitializeSecurityContext(
      &backend->cred->cred_handle, &backend->ctxt->ctxt_handle,
      backend->cred->sni_hostname, backend->req_flags,
      0, 0, &inbuf_desc, 0, রিক্ত,
      &outbuf_desc, &backend->ret_flags, রিক্ত);

    /* free buffer for received handshake data */
    curlx_safefree(inbuf[0].pvBuffer);

    /* check if the handshake was incomplete */
    বিকল্প(sspi_status) {
    পক্ষ SEC_E_INCOMPLETE_MESSAGE:
      backend->encdata_is_incomplete = TRUE;
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      DEBUGF(infof(data,
                   "schannel: received incomplete message, need more data"));
      প্রত্যাগম CURLE_OK;

    পক্ষ SEC_I_CONTINUE_NEEDED:
    পক্ষ SEC_E_OK:
      /* check if the handshake needs to be continued */
      result = CURLE_OK;
      প্রতি(i = 0; i < 3; i++) {
        /* search for handshake tokens that need to be sent */
        যদি(outbuf[i].BufferType == SECBUFFER_TOKEN && outbuf[i].cbBuffer > 0) {
          size_t written = 0;
          DEBUGF(infof(data, "schannel: sending next handshake data: "
                       "sending %lu bytes.", outbuf[i].cbBuffer));

          /* send handshake token to server */
          result = Curl_conn_cf_send(cf->next, data,
                                     (ধ্রুব uint8_t *)outbuf[i].pvBuffer,
                                     outbuf[i].cbBuffer,
                                     FALSE, &written);
          যদি(result || (outbuf[i].cbBuffer != written)) {
            failf(data, "schannel: failed to send next handshake data: "
                  "sent %zu of %lu bytes", written, outbuf[i].cbBuffer);
            result = CURLE_SSL_CONNECT_ERROR;
          }
        }
      }
      প্রতি(i = 0; i < 3; i++) {
        /* free obsolete buffer */
        যদি(outbuf[i].pvBuffer)
          Curl_pSecFn->FreeContextBuffer(outbuf[i].pvBuffer);
      }
      যদি(result)
        প্রত্যাগম result;
      ভঙ্গ;

    পক্ষ SEC_I_INCOMPLETE_CREDENTIALS:
      যদি(!(backend->req_flags & ISC_REQ_USE_SUPPLIED_CREDS)) {
        /* If the server has requested a client certificate, attempt to
           continue the handshake without one. This allows connections to
           servers which request a client certificate but do not require
           it. */
        backend->req_flags |= ISC_REQ_USE_SUPPLIED_CREDS;
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        DEBUGF(infof(data,
                     "schannel: a client certificate has been requested"));
        প্রত্যাগম CURLE_OK;
      }
      FALLTHROUGH();

    স্বভাব:
      প্রত্যাগম schannel_error(data, sspi_status);
    }

    /* check if there was additional remaining encrypted data */
    যদি(inbuf[1].BufferType == SECBUFFER_EXTRA && inbuf[1].cbBuffer > 0) {
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
      যদি(backend->encdata.offset > inbuf[1].cbBuffer) {
        memmove(backend->encdata.buffer,
                (backend->encdata.buffer + backend->encdata.offset) -
                inbuf[1].cbBuffer, inbuf[1].cbBuffer);
        backend->encdata.offset = inbuf[1].cbBuffer;
        যদি(sspi_status == SEC_I_CONTINUE_NEEDED) {
          doread = FALSE;
          অনুবর্ত;
        }
      }
    }
    অন্যথা {
      backend->encdata.offset = 0;
    }
    ভঙ্গ;
  }

  /* check if the handshake needs to be continued */
  যদি(sspi_status == SEC_I_CONTINUE_NEEDED) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    প্রত্যাগম CURLE_OK;
  }

  /* check if the handshake is complete */
  যদি(sspi_status == SEC_E_OK) {
    connssl->connecting_state = ssl_connect_3;
    DEBUGF(infof(data, "schannel: SSL/TLS handshake complete"));
  }

#ifndef CURL_DISABLE_PROXY
  pubkey_ptr = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#অন্যথা
  pubkey_ptr = CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
  যদি(pubkey_ptr) {
    result = schannel_pkp_pin_peer_pubkey(cf, data, pubkey_ptr);
    যদি(result) {
      failf(data, "SSL: public key does not match pinned public key");
      প্রত্যাগম result;
    }
  }

  যদি(conn_config->verifypeer && backend->use_manual_cred_validation) {
    /* Certificate verification also verifies the hostname if verifyhost */
    প্রত্যাগম Curl_verify_certificate(cf, data);
  }

  /* Verify the hostname manually when certificate verification is disabled,
     because in that case Schannel does not verify it. */
  যদি(!conn_config->verifypeer && conn_config->verifyhost)
    প্রত্যাগম Curl_verify_host(cf, data);

  প্রত্যাগম CURLE_OK;
}

স্থির bool valid_cert_encoding(ধ্রুব CERT_CONTEXT *cert_context)
{
  প্রত্যাগম cert_context &&
    ((cert_context->dwCertEncodingType & X509_ASN_ENCODING) != 0) &&
    cert_context->pbCertEncoded &&
    (cert_context->cbCertEncoded > 0);
}

প্রকারনির্দেশ bool (*Read_crt_func)(ধ্রুব CERT_CONTEXT *ccert_context,
                              bool reverse_order, শূন্য *arg);

স্থির শূন্য traverse_cert_store(ধ্রুব CERT_CONTEXT *context,
                                Read_crt_func func, শূন্য *arg)
{
  ধ্রুব CERT_CONTEXT *current_context = রিক্ত;
  bool should_continue = TRUE;
  bool first = TRUE;
  bool reverse_order = FALSE;
  যাবত্(should_continue &&
        (current_context = CertEnumCertificatesInStore(
          context->hCertStore,
          current_context)) != রিক্ত) {
    /* Windows 11 22H2 OS Build 22621.674 or higher enumerates certificates in
       leaf-to-root order while all previous versions of Windows enumerate
       certificates in root-to-leaf order. Determine the order of enumeration
       by comparing SECPKG_ATTR_REMOTE_CERT_CONTEXT's pbCertContext with the
       first certificate's pbCertContext. */
    যদি(first && context->pbCertEncoded != current_context->pbCertEncoded)
      reverse_order = TRUE;
    should_continue = func(current_context, reverse_order, arg);
    first = FALSE;
  }

  যদি(current_context)
    CertFreeCertificateContext(current_context);
}

স্থির bool cert_counter_callback(ধ্রুব CERT_CONTEXT *ccert_context,
                                  bool reverse_order, শূন্য *certs_গুনতি)
{
  (শূন্য)reverse_order;
  যদি(valid_cert_encoding(ccert_context))
    (*(পূর্ণাঙ্ক *)certs_গুনতি)++;
  যদি(*(পূর্ণাঙ্ক *)certs_গুনতি > MAX_ALLOWED_CERT_AMOUNT)
    প্রত্যাগম FALSE;
  প্রত্যাগম TRUE;
}

সংরচনা Adder_args {
  সংরচনা Curl_easy *data;
  CURLcode result;
  পূর্ণাঙ্ক idx;
  পূর্ণাঙ্ক certs_গুনতি;
};

স্থির bool add_cert_to_certinfo(ধ্রুব CERT_CONTEXT *ccert_context,
                                 bool reverse_order, শূন্য *raw_arg)
{
  সংরচনা Adder_args *args = (সংরচনা Adder_args *)raw_arg;
  args->result = CURLE_OK;
  যদি(valid_cert_encoding(ccert_context)) {
    পূর্ণাঙ্ক ঢোকাও_index = reverse_order ? (args->certs_গুনতি - 1) - args->idx :
                       args->idx;
    args->result = Curl_extract_certinfo(args->data, ঢোকাও_index,
                                         ccert_context->pbCertEncoded,
                                         ccert_context->pbCertEncoded +
                                         ccert_context->cbCertEncoded);
    args->idx++;
  }
  প্রত্যাগম args->result == CURLE_OK;
}

স্থির শূন্য schannel_session_free(শূন্য *sessionid)
{
  /* this is expected to be called under sessionid lock */
  সংরচনা Curl_schannel_cred *cred = sessionid;

  যদি(cred) {
    cred->refcount--;
    যদি(cred->refcount == 0) {
      Curl_pSecFn->FreeCredentialsHandle(&cred->cred_handle);
      curlx_free(cred->sni_hostname);
      যদি(cred->client_cert_store) {
        CertCloseStore(cred->client_cert_store, 0);
        cred->client_cert_store = রিক্ত;
      }
      curlx_safefree(cred);
    }
  }
}

স্থির CURLcode schannel_connect_step3(সংরচনা Curl_cfilter *cf,
                                       সংরচনা Curl_easy *data)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা schannel_ssl_backend_data *backend =
    (সংরচনা schannel_ssl_backend_data *)connssl->backend;
  সংরচনা ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  CURLcode result = CURLE_OK;
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CERT_CONTEXT *ccert_context = রিক্ত;
  SecPkgContext_ApplicationProtocol alpn_result;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);
  DEBUGASSERT(backend);

  DEBUGF(infof(data, "schannel: SSL/TLS connection with %s port %d (step 3/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->port));

  যদি(!backend->cred)
    প্রত্যাগম CURLE_SSL_CONNECT_ERROR;

  /* check if the required context attributes are met */
  যদি(backend->ret_flags != backend->req_flags) {
    যদি(!(backend->ret_flags & ISC_RET_SEQUENCE_DETECT))
      failf(data, "schannel: failed to setup sequence detection");
    যদি(!(backend->ret_flags & ISC_RET_REPLAY_DETECT))
      failf(data, "schannel: failed to setup replay detection");
    যদি(!(backend->ret_flags & ISC_RET_CONFIDENTIALITY))
      failf(data, "schannel: failed to setup confidentiality");
    যদি(!(backend->ret_flags & ISC_RET_ALLOCATED_MEMORY))
      failf(data, "schannel: failed to setup memory allocation");
    যদি(!(backend->ret_flags & ISC_RET_STREAM))
      failf(data, "schannel: failed to setup stream orientation");
    প্রত্যাগম CURLE_SSL_CONNECT_ERROR;
  }

  যদি(backend->use_alpn) {
    sspi_status =
      Curl_pSecFn->QueryContextAttributes(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_APPLICATION_PROTOCOL,
                                          &alpn_result);

    যদি(sspi_status != SEC_E_OK) {
      failf(data, "schannel: failed to retrieve ALPN result");
      প্রত্যাগম CURLE_SSL_CONNECT_ERROR;
    }

    যদি(alpn_result.ProtoNegoStatus ==
       SecApplicationProtocolNegotiationStatus_Success) {
      যদি(backend->recv_renegotiating &&
         connssl->negotiated.alpn &&
         strncmp(connssl->negotiated.alpn,
                 (ধ্রুব বর্ণ *)alpn_result.ProtocolId,
                 alpn_result.ProtocolIdSize)) {
        /* Renegotiation selected a different protocol now, we cannot
         * deal with this */
        failf(data, "schannel: server selected an ALPN protocol too late");
        প্রত্যাগম CURLE_SSL_CONNECT_ERROR;
      }
      Curl_alpn_set_negotiated(cf, data, connssl, alpn_result.ProtocolId,
                               alpn_result.ProtocolIdSize);
    }
    অন্যথা {
      যদি(!backend->recv_renegotiating)
        Curl_alpn_set_negotiated(cf, data, connssl, রিক্ত, 0);
    }
  }

  /* save the current session data for possible reuse */
  যদি(Curl_ssl_scache_use(cf, data)) {
    Curl_ssl_scache_lock(data);
    /* Up ref count since call takes ownership */
    backend->cred->refcount++;
    result = Curl_ssl_scache_add_obj(cf, data, connssl->peer.scache_চাবি,
                                     backend->cred, schannel_session_free);
    Curl_ssl_scache_unlock(data);
    যদি(result)
      প্রত্যাগম result;
  }

  যদি(ssl_config->certinfo) {
    পূর্ণাঙ্ক certs_গুনতি = 0;
    sspi_status =
      Curl_pSecFn->QueryContextAttributes(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                          &ccert_context);

    যদি((sspi_status != SEC_E_OK) || !ccert_context) {
      failf(data, "schannel: failed to retrieve remote cert context");
      প্রত্যাগম CURLE_PEER_FAILED_VERIFICATION;
    }

    traverse_cert_store(ccert_context, cert_counter_callback, &certs_গুনতি);
    যদি(certs_গুনতি > MAX_ALLOWED_CERT_AMOUNT) {
      failf(data, "%d certificates is more than allowed (%d)",
            certs_গুনতি, MAX_ALLOWED_CERT_AMOUNT);
      CertFreeCertificateContext(ccert_context);
      প্রত্যাগম CURLE_SSL_CONNECT_ERROR;
    }

    result = Curl_ssl_init_certinfo(data, certs_গুনতি);
    যদি(!result) {
      সংরচনা Adder_args args;
      args.data = data;
      args.idx = 0;
      args.certs_গুনতি = certs_গুনতি;
      args.result = CURLE_OK;
      traverse_cert_store(ccert_context, add_cert_to_certinfo, &args);
      result = args.result;
    }
    CertFreeCertificateContext(ccert_context);
    যদি(result)
      প্রত্যাগম result;
  }

  connssl->connecting_state = ssl_connect_done;

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode schannel_connect(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 bool *done)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  CURLcode result;

  /* check if the connection has already been established */
  যদি(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    প্রত্যাগম CURLE_OK;
  }

  *done = FALSE;

  যদি(connssl->connecting_state == ssl_connect_1) {
    result = schannel_connect_step1(cf, data);
    যদি(result)
      প্রত্যাগম result;
  }

  যদি(connssl->connecting_state == ssl_connect_2) {
    result = schannel_connect_step2(cf, data);
    যদি(result)
      প্রত্যাগম result;
  }

  যদি(connssl->connecting_state == ssl_connect_3) {
    result = schannel_connect_step3(cf, data);
    যদি(result)
      প্রত্যাগম result;
  }

  যদি(connssl->connecting_state == ssl_connect_done) {
    সংরচনা schannel_ssl_backend_data *backend =
      (সংরচনা schannel_ssl_backend_data *)connssl->backend;
    DEBUGASSERT(backend);

    যদি(Curl_pSecFn->QueryContextAttributes(
         &backend->ctxt->ctxt_handle,
         SECPKG_ATTR_STREAM_SIZES,
         &backend->stream_sizes)) {
      failf(data, "schannel: failed getting stream sizes");
      প্রত্যাগম CURLE_SSL_CONNECT_ERROR;
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

  প্রত্যাগম CURLE_OK;
}

গণনা schannel_renegotiate_caller_t {
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
স্থির CURLcode schannel_recv_renegotiate(
  সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
  গণনা schannel_renegotiate_caller_t caller)
{
  CURLcode result;
  curl_socket_t sockfd;
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা schannel_ssl_backend_data *backend =
    (সংরচনা schannel_ssl_backend_data *)connssl->backend;
  সংরচনা schannel_renegotiate_state *rs = &backend->renegotiate_state;

  যদি(!backend || !backend->recv_renegotiating) {
    failf(data, "schannel: unexpected call to schannel_recv_renegotiate");
    প্রত্যাগম CURLE_SSL_CONNECT_ERROR;
  }
  DEBUGASSERT(caller <= SCH_RENEG_CALLER_IS_SEND);
  যদি(caller == SCH_RENEG_CALLER_IS_RECV)
    SCH_DEV(infof(data, "schannel: renegotiation caller is schannel_recv"));
  অন্যথা
    SCH_DEV(infof(data, "schannel: renegotiation caller is schannel_send"));

  sockfd = Curl_conn_cf_get_socket(cf, data);

  যদি(sockfd == CURL_SOCKET_BAD) {
    failf(data, "schannel: renegotiation missing socket");
    প্রত্যাগম CURLE_SSL_CONNECT_ERROR;
  }

  যদি(!rs->started) { /* new renegotiation */
    infof(data, "schannel: renegotiating SSL/TLS connection");
    DEBUGASSERT(connssl->state == ssl_connection_complete);
    DEBUGASSERT(connssl->connecting_state == ssl_connect_done);
    connssl->state = ssl_connection_negotiating;
    connssl->connecting_state = ssl_connect_2;
    memset(rs, 0, পরিমাণ(*rs));
    rs->io_need = CURL_SSL_IO_NEED_SEND;
    rs->start_time = *Curl_pgrs_now(data);
    rs->started = TRUE;
  }

  প্রতি(;;) {
    bool block_read, block_write, blocking, done;
    curl_socket_t readfd, writefd;
    timediff_t elapsed;

    elapsed = curlx_ptimediff_ms(Curl_pgrs_now(data), &rs->start_time);
    যদি(elapsed >= MAX_RENEG_BLOCK_TIME) {
      failf(data, "schannel: renegotiation timeout");
      result = CURLE_SSL_CONNECT_ERROR;
      ভঙ্গ;
    }

    /* the current io_need state may have been overwritten since the last time
       this function was called. restore the io_need state needed to continue
       the renegotiation. */

    connssl->io_need = rs->io_need;

    result = schannel_connect(cf, data, &done);

    rs->io_need = connssl->io_need;

    যদি(!result && !done)
      result = CURLE_AGAIN;

    যদি(result != CURLE_AGAIN)
      ভঙ্গ;

    readfd = (rs->io_need & CURL_SSL_IO_NEED_RECV) ? sockfd : CURL_SOCKET_BAD;
    writefd = (rs->io_need & CURL_SSL_IO_NEED_SEND) ? sockfd : CURL_SOCKET_BAD;

    যদি(readfd == CURL_SOCKET_BAD && writefd == CURL_SOCKET_BAD)
      অনুবর্ত;

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

    প্রতি(;;) {
      পূর্ণাঙ্ক what;
      timediff_t timeout_ms, remaining;

      result = Curl_pgrsUpdate(data);
      যদি(result)
        ভঙ্গ;

      elapsed = curlx_ptimediff_ms(Curl_pgrs_now(data), &rs->start_time);
      যদি(elapsed >= MAX_RENEG_BLOCK_TIME) {
        failf(data, "schannel: renegotiation timeout");
        result = CURLE_SSL_CONNECT_ERROR;
        ভঙ্গ;
      }
      remaining = MAX_RENEG_BLOCK_TIME - elapsed;

      যদি(blocking) {
        timeout_ms = Curl_timeleft_ms(data);

        যদি(timeout_ms < 0) {
          result = CURLE_OPERATION_TIMEDOUT;
          ভঙ্গ;
        }

        /* the blocking is in intervals so that the progress function can be
           called every second */
        যদি(!timeout_ms || timeout_ms > 1000)
          timeout_ms = 1000;

        যদি(timeout_ms > remaining)
          timeout_ms = remaining;
      }
      অন্যথা
        timeout_ms = 0;

      SCH_DEV(infof(data, "schannel: renegotiation wait until socket is"
                    "%s%s for up to %" FMT_TIMEDIFF_T " ms",
                    ((readfd != CURL_SOCKET_BAD) ? " readable" : ""),
                    ((writefd != CURL_SOCKET_BAD) ? " writable" : ""),
                    timeout_ms));

      what = Curl_socket_check(readfd, CURL_SOCKET_BAD, writefd, timeout_ms);

      যদি(what > 0 && (what & (CURL_CSELECT_IN | CURL_CSELECT_OUT))) {
        SCH_DEV(infof(data, "schannel: renegotiation socket %s%s",
                      ((what & CURL_CSELECT_IN) ? "CURL_CSELECT_IN " : ""),
                      ((what & CURL_CSELECT_OUT) ? "CURL_CSELECT_OUT " : "")));
        result = CURLE_AGAIN;
        ভঙ্গ;
      }
      অন্যথা যদি(!what) {
        SCH_DEV(infof(data, "schannel: renegotiation socket timeout"));
        যদি(blocking)
          অনুবর্ত;
        অন্যথা
          প্রত্যাগম CURLE_AGAIN;
      }

      failf(data, "schannel: socket error during renegotiation");
      result = CURLE_SSL_CONNECT_ERROR;
      ভঙ্গ;
    }
    যদি(result != CURLE_AGAIN)
      ভঙ্গ;
  }

  DEBUGASSERT(result != CURLE_AGAIN);

  rs->started = FALSE;
  backend->recv_renegotiating = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  যদি(result)
    failf(data, "schannel: renegotiation failed");
  অন্যথা
    infof(data, "schannel: SSL/TLS connection renegotiated");

  প্রত্যাগম result;
}

স্থির CURLcode schannel_send(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                              ধ্রুব শূন্য *buf, size_t len, size_t *pnwritten)
{
  size_t data_len = 0;
  unsigned বর্ণ *ptr = রিক্ত;
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  SecBuffer outbuf[4];
  SecBufferDesc outbuf_desc;
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CURLcode result = CURLE_OK;
  সংরচনা schannel_ssl_backend_data *backend =
    (সংরচনা schannel_ssl_backend_data *)connssl->backend;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  যদি(backend->recv_renegotiating) {
    result = schannel_recv_renegotiate(cf, data, SCH_RENEG_CALLER_IS_SEND);
    যদি(result)
      প্রত্যাগম result;
  }

  /* check if the buffer is longer than the maximum message length */
  যদি(len > backend->stream_sizes.cbMaximumMessage) {
    len = backend->stream_sizes.cbMaximumMessage;
  }

  /* calculate the complete message length and prepare the send buffer */
  data_len = backend->stream_sizes.cbHeader + len +
    backend->stream_sizes.cbTrailer;
  যদি(data_len > backend->send_buffer_len) {
    ptr = curlx_realloc(backend->send_buffer, data_len);
    যদি(!ptr)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    backend->send_buffer = ptr;
    backend->send_buffer_len = data_len;
  }
  অন্যথা
    ptr = backend->send_buffer;

  /* setup output buffers (header, data, trailer, empty) */
  InitSecBuffer(&outbuf[0], SECBUFFER_STREAM_HEADER,
                ptr, backend->stream_sizes.cbHeader);
  InitSecBuffer(&outbuf[1], SECBUFFER_DATA,
                ptr + backend->stream_sizes.cbHeader, curlx_uztoul(len));
  InitSecBuffer(&outbuf[2], SECBUFFER_STREAM_TRAILER,
                ptr + backend->stream_sizes.cbHeader + len,
                backend->stream_sizes.cbTrailer);
  InitSecBuffer(&outbuf[3], SECBUFFER_EMPTY, রিক্ত, 0);
  InitSecBufferDesc(&outbuf_desc, outbuf, 4);

  /* copy data into output buffer */
  memcpy(outbuf[1].pvBuffer, buf, len);

  /* https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-encryptmessage */
  sspi_status = Curl_pSecFn->EncryptMessage(&backend->ctxt->ctxt_handle, 0,
                                            &outbuf_desc, 0);

  /* check if the message was encrypted */
  যদি(sspi_status == SEC_E_OK) {

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
    যাবত্(len > *pnwritten) {
      size_t this_write = 0;
      পূর্ণাঙ্ক what;
      timediff_t timeout_ms = Curl_timeleft_ms(data);
      যদি(timeout_ms < 0) {
        /* we already got the timeout */
        failf(data, "schannel: timed out sending data (bytes sent: %zu)",
              *pnwritten);
        result = CURLE_OPERATION_TIMEDOUT;
        ভঙ্গ;
      }
      অন্যথা যদি(!timeout_ms)
        timeout_ms = TIMEDIFF_T_MAX;
      what = SOCKET_WRITABLE(Curl_conn_cf_get_socket(cf, data), timeout_ms);
      যদি(what < 0) {
        /* fatal error */
        failf(data, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        result = CURLE_SEND_ERROR;
        ভঙ্গ;
      }
      অন্যথা যদি(what == 0) {
        failf(data, "schannel: timed out sending data (bytes sent: %zu)",
              *pnwritten);
        result = CURLE_OPERATION_TIMEDOUT;
        ভঙ্গ;
      }
      /* socket is writable */

       result = Curl_conn_cf_send(cf->next, data,
                                  (ধ্রুব uint8_t *)ptr + *pnwritten,
                                  len - *pnwritten,
                                  FALSE, &this_write);
      যদি(result == CURLE_AGAIN)
        অনুবর্ত;
      অন্যথা যদি(result) {
        ভঙ্গ;
      }

      *pnwritten += this_write;
    }
  }
  অন্যথা যদি(sspi_status == SEC_E_INSUFFICIENT_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
  }
  অন্যথা {
    result = CURLE_SEND_ERROR;
  }

  যদি(len == *pnwritten)
    /* Encrypted message including header, data and trailer entirely sent.
       The return value is the number of unencrypted bytes that were sent. */
    *pnwritten = outbuf[1].cbBuffer;

  প্রত্যাগম result;
}

স্থির CURLcode schannel_recv(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                              বর্ণ *buf, size_t len, size_t *pnread)
{
  size_t size = 0;
  size_t nread = 0;
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  SecBuffer inbuf[4];
  SecBufferDesc inbuf_desc;
  SECURITY_STATUS sspi_status = SEC_E_OK;
  সংরচনা schannel_ssl_backend_data *backend =
    (সংরচনা schannel_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(backend);
  *pnread = 0;

  যদি(backend->recv_renegotiating) {
    result = schannel_recv_renegotiate(cf, data, SCH_RENEG_CALLER_IS_RECV);
    যদি(result)
      প্রত্যাগম result;
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

  যদি(len && len <= backend->decdata.offset) {
    SCH_DEV(infof(data,
                  "schannel: enough decrypted data is already available"));
    গচ্ছ cleanup;
  }
  অন্যথা যদি(backend->recv_unrecoverable_err) {
    result = backend->recv_unrecoverable_err;
    infof(data, "schannel: an unrecoverable error occurred in a prior call");
    গচ্ছ cleanup;
  }
  অন্যথা যদি(backend->recv_sspi_close_notify) {
    /* once a server has indicated shutdown there is no more encrypted data */
    infof(data, "schannel: server indicated shutdown in a prior call");
    গচ্ছ cleanup;
  }
  /* it is debatable what to return when !len. Regardless we cannot return
     immediately because there may be data to decrypt (in the case we want to
     decrypt all encrypted cached data) so handle !len later in cleanup. */
  অন্যথা যদি(len && !backend->recv_connection_closed) {
    /* the encrypted buffer must be large enough to hold all the bytes
       requested and some TLS record overhead. 'len' is a buffer size, so this
       integer math cannot overflow. */
    ধ্রুব size_t min_encdata_length = len + CURL_SCHANNEL_BUFFER_FREE_SIZE;

    /* make sure encrypt buffer fits the requested amount of data */
    result = ensure_encoding_size(data, &backend->encdata, min_encdata_length);
    যদি(result)
      গচ্ছ cleanup;

    SCH_DEV(infof(data,
                  "schannel: encrypted data buffer: offset %zu length %zu",
                  backend->encdata.offset, backend->encdata.length));

    /* read encrypted data from socket */
    result = Curl_conn_cf_recv(cf->next, data,
                               (বর্ণ *)(backend->encdata.buffer +
                                        backend->encdata.offset),
                               backend->encdata.length -
                               backend->encdata.offset,
                               &nread);
    যদি(result) {
      যদি(result == CURLE_AGAIN)
        SCH_DEV(infof(data, "schannel: recv returned CURLE_AGAIN"));
      অন্যথা {
        infof(data, "schannel: recv returned error %d", (পূর্ণাঙ্ক)result);
        backend->recv_unrecoverable_err = result;
      }
    }
    অন্যথা যদি(nread == 0) {
      backend->recv_connection_closed = TRUE;
      DEBUGF(infof(data, "schannel: server closed the connection"));
    }
    অন্যথা {
      backend->encdata.offset += nread;
      backend->encdata_is_incomplete = FALSE;
      SCH_DEV(infof(data, "schannel: encrypted data got %zu", nread));
    }
  }

  SCH_DEV(infof(data, "schannel: encrypted data buffer: offset %zu length %zu",
                backend->encdata.offset, backend->encdata.length));

  /* decrypt loop */
  যাবত্(backend->encdata.offset > 0 && sspi_status == SEC_E_OK &&
        (!len || backend->decdata.offset < len ||
         backend->recv_connection_closed)) {
    /* prepare data buffer for DecryptMessage call */
    InitSecBuffer(&inbuf[0], SECBUFFER_DATA, backend->encdata.buffer,
                  curlx_uztoul(backend->encdata.offset));

    /* we need 3 more empty input buffers for possible output */
    InitSecBuffer(&inbuf[1], SECBUFFER_EMPTY, রিক্ত, 0);
    InitSecBuffer(&inbuf[2], SECBUFFER_EMPTY, রিক্ত, 0);
    InitSecBuffer(&inbuf[3], SECBUFFER_EMPTY, রিক্ত, 0);
    InitSecBufferDesc(&inbuf_desc, inbuf, 4);

    /* https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-decryptmessage
     */
    sspi_status = Curl_pSecFn->DecryptMessage(&backend->ctxt->ctxt_handle,
                                              &inbuf_desc, 0, রিক্ত);

    /* check if everything went fine (server may want to renegotiate
       or shutdown the connection context) */
    যদি(sspi_status == SEC_E_OK || sspi_status == SEC_I_RENEGOTIATE ||
       sspi_status == SEC_I_CONTEXT_EXPIRED) {
      /* check for successfully decrypted data, even before actual
         renegotiation or shutdown of the connection context */
      যদি(inbuf[1].BufferType == SECBUFFER_DATA) {
        SCH_DEV(infof(data, "schannel: decrypted data length: %lu",
                      inbuf[1].cbBuffer));

        /* ensure the decode buffer fits the received amount of data */
        result = ensure_decoding_size(data, &backend->decdata,
                                      inbuf[1].cbBuffer, len);
        যদি(result)
          গচ্ছ cleanup;

        /* copy decrypted data to internal buffer */
        size = inbuf[1].cbBuffer;
        যদি(size) {
          memcpy(backend->decdata.buffer + backend->decdata.offset,
                 inbuf[1].pvBuffer, size);
          backend->decdata.offset += size;
        }

        SCH_DEV(infof(data, "schannel: decrypted data added: %zu", size));
        SCH_DEV(infof(data,
                      "schannel: decrypted cached: offset %zu length %zu",
                      backend->decdata.offset, backend->decdata.length));
      }

      /* check for remaining encrypted data */
      যদি(inbuf[3].BufferType == SECBUFFER_EXTRA && inbuf[3].cbBuffer > 0) {
        SCH_DEV(infof(data, "schannel: encrypted data length: %lu",
                      inbuf[3].cbBuffer));

        /* check if the remaining data is less than the total amount
         * and therefore begins after the already processed data
         */
        যদি(backend->encdata.offset > inbuf[3].cbBuffer) {
          /* move remaining encrypted data forward to the beginning of
             buffer */
          memmove(backend->encdata.buffer,
                  (backend->encdata.buffer + backend->encdata.offset) -
                  inbuf[3].cbBuffer, inbuf[3].cbBuffer);
          backend->encdata.offset = inbuf[3].cbBuffer;
        }

        SCH_DEV(infof(data,
                      "schannel: encrypted cached: offset %zu length %zu",
                      backend->encdata.offset, backend->encdata.length));
      }
      অন্যথা {
        /* reset encrypted buffer offset, because there is no data remaining */
        backend->encdata.offset = 0;
      }

      /* check if server wants to renegotiate the connection context */
      যদি(sspi_status == SEC_I_RENEGOTIATE) {
        infof(data, "schannel: remote party requests renegotiation");
        যদি(result && result != CURLE_AGAIN) {
          infof(data, "schannel: cannot renegotiate, an error is pending");
          গচ্ছ cleanup;
        }

        backend->recv_renegotiating = TRUE;
        result = schannel_recv_renegotiate(cf, data, SCH_RENEG_CALLER_IS_RECV);
        যদি(result)
          গচ্ছ cleanup;

        /* now retry receiving data */
        sspi_status = SEC_E_OK;
        অনুবর্ত;
      }
      /* check if the server closed the connection */
      অন্যথা যদি(sspi_status == SEC_I_CONTEXT_EXPIRED) {
        /* In Windows 2000 SEC_I_CONTEXT_EXPIRED (close_notify) is not
           returned so we have to work around that in cleanup. */
        backend->recv_sspi_close_notify = TRUE;
        যদি(!backend->recv_connection_closed)
          backend->recv_connection_closed = TRUE;
        /* We received the close notify fine, any error we got
         * from the lower filters afterwards (e.g. the socket), is not
         * an error on the TLS data stream. That one ended here. */
        যদি(result == CURLE_RECV_ERROR)
          result = CURLE_OK;
        infof(data,
              "schannel: server close notification received (close_notify)");
        গচ্ছ cleanup;
      }
    }
    অন্যথা যদি(sspi_status == SEC_E_INCOMPLETE_MESSAGE) {
      backend->encdata_is_incomplete = TRUE;
      যদি(!result)
        result = CURLE_AGAIN;
      SCH_DEV(infof(data, "schannel: failed to decrypt data, need more data"));
      গচ্ছ cleanup;
    }
    অন্যথা {
      বর্ণ buffer[STRERROR_LEN];
      failf(data, "schannel: failed to read data from server: %s",
            Curl_sspi_strerror(sspi_status, buffer, পরিমাণ(buffer)));
      result = CURLE_RECV_ERROR;
      গচ্ছ cleanup;
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
  যদি(len && !backend->decdata.offset && backend->recv_connection_closed &&
     !backend->recv_sspi_close_notify) {
    result = CURLE_RECV_ERROR;
    failf(data, "schannel: server closed abruptly (missing close_notify)");
  }

  /* Any error other than CURLE_AGAIN is an unrecoverable error. */
  যদি(result && result != CURLE_AGAIN)
    backend->recv_unrecoverable_err = result;

  size = len < backend->decdata.offset ? len : backend->decdata.offset;
  যদি(size) {
    memcpy(buf, backend->decdata.buffer, size);
    memmove(backend->decdata.buffer, backend->decdata.buffer + size,
            backend->decdata.offset - size);
    backend->decdata.offset -= size;
    SCH_DEV(infof(data, "schannel: decrypted data returned %zu", size));
    SCH_DEV(infof(data,
                  "schannel: decrypted data buffer: offset %zu length %zu",
                  backend->decdata.offset, backend->decdata.length));
    *pnread = size;
    প্রত্যাগম CURLE_OK;
  }

  যদি(!result && !backend->recv_connection_closed)
    result = CURLE_AGAIN;

  /* it is debatable what to return when !len. We could return whatever error
     we got from decryption but instead we override here so the return is
     consistent. */
  যদি(!len)
    প্রত্যাগম CURLE_OK;

  প্রত্যাগম result;
}

স্থির bool schannel_data_pending(সংরচনা Curl_cfilter *cf,
                                  ধ্রুব সংরচনা Curl_easy *data)
{
  ধ্রুব সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা schannel_ssl_backend_data *backend =
    (সংরচনা schannel_ssl_backend_data *)connssl->backend;

  (শূন্য)data;
  DEBUGASSERT(backend);

  যদি(backend->ctxt) /* SSL/TLS is in use */
    প্রত্যাগম backend->decdata.offset > 0 ||
           (backend->encdata.offset > 0 && !backend->encdata_is_incomplete) ||
           backend->recv_connection_closed ||
           backend->recv_sspi_close_notify ||
           backend->recv_unrecoverable_err;
  অন্যথা
    প্রত্যাগম FALSE;
}

/* shut down the SSL connection and clean up related memory.
   this function can be called multiple times on the same connection including
   if the SSL connection failed (eg connection made but failed handshake). */
স্থির CURLcode schannel_shutdown(সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data,
                                  bool send_shutdown, bool *done)
{
  /* See https://learn.microsoft.com/windows/win32/secauthn/shutting-down-an-schannel-connection
   * Shutting Down an Schannel Connection
   */
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা schannel_ssl_backend_data *backend =
    (সংরচনা schannel_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;

  যদি(cf->shutdown) {
    *done = TRUE;
    প্রত্যাগম CURLE_OK;
  }

  DEBUGASSERT(data);
  DEBUGASSERT(backend);

  /* Not supported in schannel */
  (শূন্য)send_shutdown;

  *done = FALSE;
  যদি(backend->ctxt) {
    infof(data, "schannel: shutting down SSL/TLS connection with %s port %d",
          connssl->peer.origin->hostname, connssl->peer.origin->port);
  }

  যদি(!backend->ctxt || cf->shutdown) {
    *done = TRUE;
    গচ্ছ out;
  }

  যদি(backend->cred && backend->ctxt && !backend->sent_shutdown &&
     !backend->shutdown_generated) {
    SecBufferDesc BuffDesc;
    SecBuffer Buffer;
    SECURITY_STATUS sspi_status;
    SecBuffer outbuf;
    SecBufferDesc outbuf_desc;
    DWORD dwshut = SCHANNEL_SHUTDOWN;

    InitSecBuffer(&Buffer, SECBUFFER_TOKEN, &dwshut, পরিমাণ(dwshut));
    InitSecBufferDesc(&BuffDesc, &Buffer, 1);

    sspi_status = Curl_pSecFn->ApplyControlToken(&backend->ctxt->ctxt_handle,
                                                 &BuffDesc);

    যদি(sspi_status != SEC_E_OK) {
      বর্ণ buffer[STRERROR_LEN];
      failf(data, "schannel: ApplyControlToken failure: %s",
            Curl_sspi_strerror(sspi_status, buffer, পরিমাণ(buffer)));
      result = CURLE_SEND_ERROR;
      গচ্ছ out;
    }

    /* setup output buffer */
    InitSecBuffer(&outbuf, SECBUFFER_EMPTY, রিক্ত, 0);
    InitSecBufferDesc(&outbuf_desc, &outbuf, 1);

    sspi_status = Curl_pSecFn->InitializeSecurityContext(
      &backend->cred->cred_handle,
      &backend->ctxt->ctxt_handle,
      backend->cred->sni_hostname,
      backend->req_flags,
      0,
      0,
      রিক্ত,
      0,
      &backend->ctxt->ctxt_handle,
      &outbuf_desc,
      &backend->ret_flags, রিক্ত);

    যদি((sspi_status == SEC_E_OK) || (sspi_status == SEC_I_CONTEXT_EXPIRED)) {
      backend->shutdown_token = outbuf;
      backend->shutdown_generated = TRUE;
    }
    অন্যথা {
      বর্ণ buffer[STRERROR_LEN];
      যদি(outbuf.pvBuffer)
        Curl_pSecFn->FreeContextBuffer(outbuf.pvBuffer);
      failf(data, "schannel: generating close msg failed: %s",
            Curl_sspi_strerror(sspi_status, buffer, পরিমাণ(buffer)));
      result = CURLE_SEND_ERROR;
      গচ্ছ out;
    }
  }

  যদি(backend->shutdown_generated && !backend->sent_shutdown) {
    size_t written = 0;
    size_t remaining;

    DEBUGASSERT(backend->shutdown_offset <= backend->shutdown_token.cbBuffer);
    remaining = backend->shutdown_token.cbBuffer - backend->shutdown_offset;

    যদি(remaining) {
      result = Curl_conn_cf_send(cf->next, data,
                                 ((uint8_t *)backend->shutdown_token.pvBuffer +
                                  backend->shutdown_offset),
                                 remaining, FALSE, &written);
      যদি(!result)
        backend->shutdown_offset += written;
      যদি(result == CURLE_AGAIN || (!result && written < remaining)) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        result = CURLE_OK;
        গচ্ছ out;
      }
    }
    অন্যথা /* no shutdown message to send? */
      result = CURLE_OK;
    যদি(result) {
      যদি(backend->recv_connection_closed) {
        /* The server already closed, so sending close_notify is optional. */
        *done = TRUE;
        result = CURLE_OK;
      }
      অন্যথা {
        failf(data, "schannel: error sending close msg: %d", (পূর্ণাঙ্ক)result);
        result = CURLE_SEND_ERROR;
      }
      গচ্ছ out;
    }
    যদি(backend->shutdown_token.pvBuffer)
      Curl_pSecFn->FreeContextBuffer(backend->shutdown_token.pvBuffer);
    memset(&backend->shutdown_token, 0, পরিমাণ(backend->shutdown_token));
    backend->shutdown_offset = 0;
    backend->sent_shutdown = TRUE;
    *done = TRUE;
  }

  /* If the connection seems open and we have not seen the close notify
   * from the server yet, try to receive it. */
  যদি(backend->cred && backend->ctxt &&
     !backend->recv_sspi_close_notify && !backend->recv_connection_closed) {
    বর্ণ buffer[1024];
    size_t nread;

    result = schannel_recv(cf, data, buffer, পরিমাণ(buffer), &nread);
    যদি(result == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
    }
    অন্যথা যদি(result) {
      CURL_TRC_CF(data, cf, "SSL shutdown, error %d", (পূর্ণাঙ্ক)result);
      result = CURLE_RECV_ERROR;
    }
    অন্যথা যদি(nread == 0) {
      /* We got the close notify alert and are done. */
      backend->recv_connection_closed = TRUE;
      *done = TRUE;
    }
    অন্যথা {
      /* still data coming in? */
    }
  }

out:
  cf->shutdown = (result || *done);
  প্রত্যাগম result;
}

স্থির শূন্য schannel_close(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা schannel_ssl_backend_data *backend =
    (সংরচনা schannel_ssl_backend_data *)connssl->backend;

  DEBUGASSERT(data);
  DEBUGASSERT(backend);

  যদি(backend->shutdown_token.pvBuffer)
    Curl_pSecFn->FreeContextBuffer(backend->shutdown_token.pvBuffer);
  memset(&backend->shutdown_token, 0, পরিমাণ(backend->shutdown_token));
  backend->shutdown_offset = 0;

  /* free SSPI Schannel API security context handle */
  যদি(backend->ctxt) {
    যদি(cf->conn->sslContext == &backend->ctxt->ctxt_handle)
      cf->conn->sslContext = রিক্ত;

    DEBUGF(infof(data, "schannel: clear security context handle"));
    Curl_pSecFn->DeleteSecurityContext(&backend->ctxt->ctxt_handle);
    curlx_safefree(backend->ctxt);
  }

  /* free SSPI Schannel API credential handle */
  যদি(backend->cred) {
    Curl_ssl_scache_lock(data);
    schannel_session_free(backend->cred);
    Curl_ssl_scache_unlock(data);
    backend->cred = রিক্ত;
  }

  /* free the buffer used to encrypt outgoing data */
  curlx_safefree(backend->send_buffer);
  backend->send_buffer_len = 0;

  /* free internal buffer for received encrypted data */
  যদি(backend->encdata.buffer) {
    curlx_safefree(backend->encdata.buffer);
    backend->encdata.length = 0;
    backend->encdata.offset = 0;
    backend->encdata_is_incomplete = FALSE;
  }

  /* free internal buffer for received decrypted data */
  যদি(backend->decdata.buffer) {
    curlx_safefree(backend->decdata.buffer);
    backend->decdata.length = 0;
    backend->decdata.offset = 0;
  }
}

স্থির পূর্ণাঙ্ক schannel_init(শূন্য)
{
  প্রকারনির্দেশ ধ্রুব বর্ণ *(APIENTRY *WINE_GET_VERSION_FN)(শূন্য);
#যদি defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ঠেলো
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
  WINE_GET_VERSION_FN p_wine_get_version =
    CURLX_FUNCTION_CAST(WINE_GET_VERSION_FN,
      GetProcAddress(GetModuleHandle(TEXT("ntdll")), "wine_get_version"));
#যদি defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic বের_করো
#endif
  যদি(p_wine_get_version) {  /* WINE detected */
    curl_off_t ver = 0;
    ধ্রুব বর্ণ *wine_version = p_wine_get_version();  /* e.g. "6.0.2" */
    /* Assume ALPN support with WINE 6.0 or greater */
    যদি(wine_version)
      curlx_str_সংখ্যা(&wine_version, &ver, 20);
    s_win_has_alpn = (ver >= 6);
  }
  অন্যথা {
    /* ALPN is supported on Windows 8.1 / Server 2012 R2 or newer. */
    s_win_has_alpn = curlx_verify_windows_version(6, 3, 0, PLATFORM_WINNT,
                                                  VERSION_GREATER_THAN_EQUAL);
  }

  প্রত্যাগম Curl_sspi_global_init() == CURLE_OK ? 1 : 0;
}

স্থির শূন্য schannel_cleanup(শূন্য)
{
  Curl_sspi_global_cleanup();
}

স্থির size_t schannel_version(বর্ণ *buffer, size_t size)
{
  প্রত্যাগম curl_msnprintf(buffer, size, "Schannel");
}

স্থির CURLcode schannel_random(সংরচনা Curl_easy *data,
                                unsigned বর্ণ *entropy, size_t length)
{
  (শূন্য)data;

  প্রত্যাগম curlx_win32_random(entropy, length);
}

স্থির CURLcode schannel_checksum(ধ্রুব unsigned বর্ণ *প্রবেশ,
                                  size_t inputlen,
                                  unsigned বর্ণ *checksum,
                                  size_t checksumlen,
                                  DWORD provType,
                                  ধ্রুব unsigned পূর্ণাঙ্ক algId)
{
  CURLcode result = CURLE_FAILED_INIT;

  HCRYPTPROV hProv = 0;
  HCRYPTHASH hHash = 0;

  size_t off;

  DWORD cbHashSize;
  DWORD dwHashSizeLen;
  DWORD dwChecksumLen;

  /* since this can fail in multiple ways, zero memory first so we never
   * return old data
   */
  memset(checksum, 0, checksumlen);

  যদি(!CryptAcquireContext(&hProv, রিক্ত, রিক্ত, provType,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    গচ্ছ out;

  যদি(!CryptCreateHash(hProv, algId, 0, 0, &hHash))
    গচ্ছ out;

  result = CURLE_BAD_FUNCTION_ARGUMENT;

  off = 0;
  যাবত্(off < inputlen) {
    DWORD chunk = (DWORD)CURLMIN(inputlen - off, 0xffffffffUL);
    যদি(!CryptHashData(hHash, প্রবেশ + off, chunk, 0))
      গচ্ছ out;
    off += chunk;
  }

  /* get hash size */
  cbHashSize = 0;
  dwHashSizeLen = (DWORD)পরিমাণ(cbHashSize);
  যদি(!CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE *)&cbHashSize,
                        &dwHashSizeLen, 0))
    গচ্ছ out;

  /* check if hash fits into the return buffer */
  যদি(checksumlen < cbHashSize)
    গচ্ছ out;

  dwChecksumLen = (DWORD)checksumlen;
  যদি(CryptGetHashParam(hHash, HP_HASHVAL, checksum, &dwChecksumLen, 0) &&
     dwChecksumLen == cbHashSize)
    result = CURLE_OK;

out:
  যদি(hHash)
    CryptDestroyHash(hHash);

  যদি(hProv)
    CryptReleaseContext(hProv, 0);

  প্রত্যাগম result;
}

স্থির CURLcode schannel_sha256sum(ধ্রুব unsigned বর্ণ *প্রবেশ,
                                   size_t inputlen,
                                   unsigned বর্ণ *sha256sum,
                                   size_t sha256len)
{
  প্রত্যাগম schannel_checksum(প্রবেশ, inputlen, sha256sum, sha256len,
                           PROV_RSA_AES, CALG_SHA_256);
}

স্থির শূন্য *schannel_get_internals(সংরচনা ssl_connect_data *connssl,
                                    CURLINFO info)
{
  সংরচনা schannel_ssl_backend_data *backend =
    (সংরচনা schannel_ssl_backend_data *)connssl->backend;
  (শূন্য)info;
  DEBUGASSERT(backend);
  প্রত্যাগম &backend->ctxt->ctxt_handle;
}

HCERTSTORE Curl_schannel_get_cached_cert_store(সংরচনা Curl_cfilter *cf,
                                               সংরচনা Curl_easy *data)
{
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  সংরচনা Curl_multi *multi = data->multi;
  ধ্রুব সংরচনা curl_blob *ca_info_blob = conn_config->ca_info_blob;
  সংরচনা schannel_cert_share *share;
  timediff_t timeout_ms;
  unsigned বর্ণ info_blob_digest[CURL_SHA256_DIGEST_LENGTH];

  DEBUGASSERT(multi);

  যদি(!multi) {
    প্রত্যাগম রিক্ত;
  }

  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_SCHANNEL_CERT_SHARE_চাবি,
                         CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_চাবি));
  যদি(!share || !share->cert_store) {
    প্রত্যাগম রিক্ত;
  }

  /* zero ca_cache_timeout completely disables caching */
  যদি(!data->set.ssl_ca_cache_timeout) {
    প্রত্যাগম রিক্ত;
  }

  /* check for cache timeout by using the cached_x509_store_expired timediff
     calculation pattern from openssl.c.
     negative timeout means retain forever. */
  timeout_ms = data->set.ssl_ca_cache_timeout * (timediff_t)1000;
  যদি(timeout_ms >= 0) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(data), &share->time);
    যদি(elapsed_ms >= timeout_ms) {
      প্রত্যাগম রিক্ত;
    }
  }

  যদি(ca_info_blob) {
    যদি(share->CAinfo_blob_size != ca_info_blob->len) {
      প্রত্যাগম রিক্ত;
    }
    যদি(schannel_sha256sum((ধ্রুব unsigned বর্ণ *)ca_info_blob->data,
                          ca_info_blob->len,
                          info_blob_digest,
                          CURL_SHA256_DIGEST_LENGTH))
      প্রত্যাগম রিক্ত;
    যদি(memcmp(share->CAinfo_blob_digest, info_blob_digest,
              CURL_SHA256_DIGEST_LENGTH)) {
      প্রত্যাগম রিক্ত;
    }
  }
  অন্যথা {
    যদি(!conn_config->CAfile || !share->CAfile ||
       strcmp(share->CAfile, conn_config->CAfile)) {
      প্রত্যাগম রিক্ত;
    }
  }

  প্রত্যাগম share->cert_store;
}

স্থির শূন্য schannel_cert_share_free(ধ্রুব শূন্য *চাবি, size_t চাবি_len, শূন্য *p)
{
  সংরচনা schannel_cert_share *share = p;
  DEBUGASSERT(চাবি_len == CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_চাবি));
  DEBUGASSERT(!memcmp(MPROTO_SCHANNEL_CERT_SHARE_চাবি, চাবি, চাবি_len));
  (শূন্য)চাবি;
  (শূন্য)চাবি_len;
  যদি(share->cert_store) {
    CertCloseStore(share->cert_store, 0);
  }
  curlx_free(share->CAfile);
  curlx_free(share);
}

bool Curl_schannel_set_cached_cert_store(সংরচনা Curl_cfilter *cf,
                                         সংরচনা Curl_easy *data,
                                         HCERTSTORE cert_store)
{
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  সংরচনা Curl_multi *multi = data->multi;
  ধ্রুব সংরচনা curl_blob *ca_info_blob = conn_config->ca_info_blob;
  সংরচনা schannel_cert_share *share;
  unsigned বর্ণ digest[CURL_SHA256_DIGEST_LENGTH];
  বর্ণ *CAfile = রিক্ত;

  DEBUGASSERT(multi);

  যদি(!multi) {
    প্রত্যাগম FALSE;
  }

  যদি(ca_info_blob) {
    যদি(schannel_sha256sum((ধ্রুব unsigned বর্ণ *)ca_info_blob->data,
                          ca_info_blob->len, digest, পরিমাণ(digest))) {
      প্রত্যাগম FALSE;
    }
  }
  অন্যথা যদি(conn_config->CAfile) {
    CAfile = curlx_strdup(conn_config->CAfile);
    যদি(!CAfile) {
      প্রত্যাগম FALSE;
    }
  }

  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_SCHANNEL_CERT_SHARE_চাবি,
                         CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_চাবি));
  যদি(!share) {
    share = curlx_calloc(1, পরিমাণ(*share));
    যদি(!share) {
      curlx_free(CAfile);
      প্রত্যাগম FALSE;
    }
    যদি(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_SCHANNEL_CERT_SHARE_চাবি,
                       CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_চাবি),
                       share, schannel_cert_share_free)) {
      curlx_free(share);
      curlx_free(CAfile);
      প্রত্যাগম FALSE;
    }
  }

  /* free old cache data */
  যদি(share->cert_store) {
    CertCloseStore(share->cert_store, 0);
  }
  curlx_free(share->CAfile);

  যদি(ca_info_blob) {
    memcpy(share->CAinfo_blob_digest, digest, পরিমাণ(digest));
    share->CAinfo_blob_size = ca_info_blob->len;
  }
  অন্যথা
    share->CAinfo_blob_size = 0;

  share->time = curlx_now();
  share->cert_store = cert_store;
  share->CAfile = CAfile;
  প্রত্যাগম TRUE;
}

ধ্রুব সংরচনা Curl_ssl Curl_ssl_schannel = {
  { CURLSSLBACKEND_SCHANNEL, "schannel" }, /* info */

  SSLSUPP_CERTINFO |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_CA_CACHE |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST,

  পরিমাণ(সংরচনা schannel_ssl_backend_data),

  schannel_init,                     /* init */
  schannel_cleanup,                  /* cleanup */
  schannel_version,                  /* version */
  schannel_shutdown,                 /* shutdown */
  schannel_data_pending,             /* data_pending */
  schannel_random,                   /* random */
  রিক্ত,                              /* cert_status_request */
  schannel_connect,                  /* connect */
  Curl_ssl_adjust_pollset,           /* adjust_pollset */
  schannel_get_internals,            /* get_internals */
  schannel_close,                    /* close_one */
  রিক্ত,                              /* close_all */
  রিক্ত,                              /* engines_list */
  schannel_sha256sum,                /* sha256sum */
  schannel_recv,                     /* recv decrypted data */
  schannel_send,                     /* send data to encrypt */
  রিক্ত,                              /* get_channel_binding */
};

#endif /* USE_SCHANNEL */
