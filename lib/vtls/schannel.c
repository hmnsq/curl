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
#அந்தர்பாவ "curl_setup.h"

#ifdef USE_SCHANNEL

#ifndef USE_WINDOWS_SSPI
#error "cannot compile Schannel support without SSPI."
#endif

#அந்தர்பாவ "vtls/schannel.h"
#அந்தர்பாவ "vtls/schannel_int.h"
#அந்தர்பாவ "vtls/vtls.h"
#அந்தர்பாவ "vtls/vtls_int.h"
#அந்தர்பாவ "vtls/vtls_scache.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "connect.h" /* for the connect timeout */
#அந்தர்பாவ "curlx/strdup.h"
#அந்தர்பாவ "strerror.h"
#அந்தர்பாவ "select.h" /* for the socket readiness */
#அந்தர்பாவ "curlx/win32-fopen.h"
#அந்தர்பாவ "curlx/multibyte.h"
#அந்தர்பாவ "vtls/x509asn1.h"
#அந்தர்பாவ "curlx/version_win32.h"
#அந்தர்பாவ "curlx/winapi.h"
#அந்தர்பாவ "curlx/strparse.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "curl_sha256.h"

/* Some verbose debug messages are wrapped by SCH_DEV() instead of DEBUGF()
 * and only shown if CURL_SCHANNEL_DEV_DEBUG was defined at build time. These
 * messages are extra verbose and intended for curl developers debugging
 * Schannel recv decryption and renegotiation.
 */
#ifdef CURL_SCHANNEL_DEV_DEBUG
#define SCH_DEV(x) x
#define SCH_DEV_SHOWBOOL(x)                                     \
  செய் {                                                          \
    infof(data, "schannel: " #x " %s", (x) ? "TRUE" : "FALSE"); \
  } போது(0)
#இல்லையெனில்
#define SCH_DEV(x) செய் {} போது(0)
#define SCH_DEV_SHOWBOOL(x) செய் {} போது(0)
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
#ifndef PKCS12_NO_PERSIST_சாவி
#define PKCS12_NO_PERSIST_சாவி 0x00008000
#endif

/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#ifndef CERT_FIND_HAS_PRIVATE_சாவி
#define CERT_FIND_HAS_PRIVATE_சாவி (21 << CERT_COMPARE_SHIFT)
#endif

/* key to use at `multi->proto_hash` */
#define MPROTO_SCHANNEL_CERT_SHARE_சாவி   "tls:schannel:cert:share"

நிலையான bool s_win_has_alpn;

/* Offered by mingw-w64 v9+, MS SDK 8.1/VS2013+ */
#ifndef SECBUFFER_APPLICATION_PROTOCOLS
#define SECBUFFER_APPLICATION_PROTOCOLS 18
#define SECPKG_ATTR_APPLICATION_PROTOCOL 35

வகைவரை எண்ணிக்கை {
  SecApplicationProtocolNegotiationExt_None,
  SecApplicationProtocolNegotiationExt_NPN,
  SecApplicationProtocolNegotiationExt_ALPN
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT;

வகைவரை எண்ணிக்கை {
  SecApplicationProtocolNegotiationStatus_None,
  SecApplicationProtocolNegotiationStatus_Success,
  SecApplicationProtocolNegotiationStatus_SelectedClientOnly
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_STATUS;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
வகைவரை கட்டமைப்பு {
  SEC_APPLICATION_PROTOCOL_NEGOTIATION_STATUS ProtoNegoStatus;
  SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT ProtoNegoExt;
  unsigned எழுத்து ProtocolIdSize;
  unsigned எழுத்து ProtocolId[0xff];
} SecPkgContext_ApplicationProtocol;
#endif

நிலையான வெறுமை InitSecBuffer(SecBuffer *buffer, unsigned long BufType,
                          வெறுமை *BufDataPtr, unsigned long BufByteSize)
{
  buffer->cbBuffer = BufByteSize;
  buffer->BufferType = BufType;
  buffer->pvBuffer = BufDataPtr;
}

நிலையான வெறுமை InitSecBufferDesc(SecBufferDesc *desc, SecBuffer *BufArr,
                              unsigned long இலக்கம்_Arr_Elem)
{
  desc->ulVersion = SECBUFFER_VERSION;
  desc->pBuffers = BufArr;
  desc->cBuffers = இலக்கம்_Arr_Elem;
}

நிலையான CURLcode schannel_set_ssl_version_min_max(DWORD *enabled_protocols,
                                                 கட்டமைப்பு Curl_cfilter *cf,
                                                 கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  long ssl_version = conn_config->version;
  long ssl_version_max = (long)conn_config->version_max;
  long i = ssl_version;

  தேர்வு(ssl_version_max) {
  நிலை CURL_SSLVERSION_MAX_NONE:
  நிலை CURL_SSLVERSION_MAX_DEFAULT:

    /* Windows Server 2022 and newer (including Windows 11) support TLS 1.3
       built-in. Previous builds of Windows 10 had broken TLS 1.3
       implementations that could be enabled via registry. */
    எனில்(curlx_verify_windows_version(10, 0, 20348, PLATFORM_WINNT,
                                    VERSION_GREATER_THAN_EQUAL)) {
      ssl_version_max = CURL_SSLVERSION_MAX_TLSv1_3;
    }
    இல்லையெனில் /* Windows 10 or older */
      ssl_version_max = CURL_SSLVERSION_MAX_TLSv1_2;

    நிறுத்து;
  }

  க்கு(; i <= (ssl_version_max >> 16); ++i) {
    தேர்வு(i) {
    நிலை CURL_SSLVERSION_TLSv1_0:
      *enabled_protocols |= SP_PROT_TLS1_0_CLIENT;
      நிறுத்து;
    நிலை CURL_SSLVERSION_TLSv1_1:
      *enabled_protocols |= SP_PROT_TLS1_1_CLIENT;
      நிறுத்து;
    நிலை CURL_SSLVERSION_TLSv1_2:
      *enabled_protocols |= SP_PROT_TLS1_2_CLIENT;
      நிறுத்து;
    நிலை CURL_SSLVERSION_TLSv1_3:

      /* Windows Server 2022 or newer */
      எனில்(curlx_verify_windows_version(10, 0, 20348, PLATFORM_WINNT,
                                      VERSION_GREATER_THAN_EQUAL)) {
        *enabled_protocols |= SP_PROT_TLS1_3_CLIENT;
        நிறுத்து;
      }
      இல்லையெனில் { /* Windows 10 or older */
        failf(data, "schannel: TLS 1.3 not supported on Windows 10 or older");
        திரும்பு CURLE_SSL_CONNECT_ERROR;
      }
    }
  }
  திரும்பு CURLE_OK;
}

#define CIPHEROPTION(x) { #x, x }

கட்டமைப்பு algo {
  மாறிலி எழுத்து *name;
  முழு id;
};

நிலையான மாறிலி கட்டமைப்பு algo algs[] = {
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
  CIPHEROPTION(CALG_SCHANNEL_MAC_சாவி),
  CIPHEROPTION(CALG_SCHANNEL_ENC_சாவி),
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
  { வெற்று, 0 },
};

நிலையான முழு get_alg_id_by_name(மாறிலி எழுத்து *name)
{
  மாறிலி எழுத்து *nameEnd = strchr(name, ':');
  size_t n = nameEnd ? (size_t)(nameEnd - name) : strlen(name);
  முழு i;

  க்கு(i = 0; algs[i].name; i++) {
    எனில்((n == strlen(algs[i].name) && !strncmp(algs[i].name, name, n)))
      திரும்பு algs[i].id;
  }
  திரும்பு 0; /* not found */
}

#define இலக்கம்_CIPHERS 47 /* There are a maximum of 47 options listed above */

நிலையான CURLcode set_ssl_ciphers(SCHANNEL_CRED *schannel_cred, எழுத்து *ciphers,
                                ALG_ID *algIds)
{
  மாறிலி எழுத்து *startCur = ciphers;
  முழு alg_மொத்தம் = 0;
  போது(startCur && *startCur && (alg_மொத்தம் < இலக்கம்_CIPHERS)) {
    curl_off_t alg;
    எனில்(curlx_str_எண்(&startCur, &alg, INT_MAX) || !alg)
      alg = get_alg_id_by_name(startCur);

    எனில்(alg)
      algIds[alg_மொத்தம்++] = (ALG_ID)alg;
    இல்லையெனில் எனில்(!strncmp(startCur, "USE_STRONG_CRYPTO",
                     CURL_CSTRLEN("USE_STRONG_CRYPTO")) ||
            !strncmp(startCur, "SCH_USE_STRONG_CRYPTO",
                     CURL_CSTRLEN("SCH_USE_STRONG_CRYPTO")))
      schannel_cred->dwFlags |= SCH_USE_STRONG_CRYPTO;
    இல்லையெனில்
      திரும்பு CURLE_SSL_CIPHER;
    startCur = strchr(startCur, ':');
    எனில்(startCur)
      startCur++;
  }
  schannel_cred->palgSupportedAlgs = algIds;
  schannel_cred->cSupportedAlgs = (DWORD)alg_மொத்தம்;
  திரும்பு CURLE_OK;
}

/* Function allocates memory for store_path only if CURLE_OK is returned */
நிலையான CURLcode get_cert_location(TCHAR *path, DWORD *store_name,
                                  TCHAR **store_path, TCHAR **thumbprint)
{
  TCHAR *sep;
  TCHAR *store_path_start;
  size_t store_name_len;

  sep = _tcschr(path, _TEXT('\\'));
  எனில்(!sep)
    திரும்பு CURLE_SSL_CERTPROBLEM;

  store_name_len = sep - path;

  எனில்(_tcsncmp(path, _TEXT("CurrentUser"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_USER;
  இல்லையெனில் எனில்(_tcsncmp(path, _TEXT("LocalMachine"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE;
  இல்லையெனில் எனில்(_tcsncmp(path, _TEXT("CurrentService"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_SERVICE;
  இல்லையெனில் எனில்(_tcsncmp(path, _TEXT("Services"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_SERVICES;
  இல்லையெனில் எனில்(_tcsncmp(path, _TEXT("Users"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_USERS;
  இல்லையெனில் எனில்(_tcsncmp(path, _TEXT("CurrentUserGroupPolicy"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_USER_GROUP_POLICY;
  இல்லையெனில் எனில்(_tcsncmp(path, _TEXT("LocalMachineGroupPolicy"), store_name_len) ==
          0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE_GROUP_POLICY;
  இல்லையெனில் எனில்(_tcsncmp(path, _TEXT("LocalMachineEnterprise"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE_ENTERPRISE;
  இல்லையெனில்
    திரும்பு CURLE_SSL_CERTPROBLEM;

  store_path_start = sep + 1;

  sep = _tcschr(store_path_start, _TEXT('\\'));
  எனில்(!sep)
    திரும்பு CURLE_SSL_CERTPROBLEM;

  *thumbprint = sep + 1;
  எனில்(_tcslen(*thumbprint) != CERT_THUMBPRINT_STR_LEN)
    திரும்பு CURLE_SSL_CERTPROBLEM;

  *sep = _TEXT('\0');
  *store_path = curlx_tcsdup(store_path_start);
  *sep = _TEXT('\\');
  எனில்(!*store_path)
    திரும்பு CURLE_OUT_OF_MEMORY;

  திரும்பு CURLE_OK;
}

நிலையான CURLcode get_client_cert(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data,
                                HCERTSTORE *out_cert_store,
                                PCCERT_CONTEXT *out_cert_context)
{
  கட்டமைப்பு ssl_filter_config *sslc = Curl_ssl_cf_get_filter_config(cf);
  PCCERT_CONTEXT client_cert = வெற்று;
  HCERTSTORE client_cert_store = வெற்று;
  CURLcode result = CURLE_OK;

  எனில்(sslc->clientcert || sslc->cert_blob) {
    DWORD cert_store_name = 0;
    TCHAR *cert_store_path = வெற்று;
    TCHAR *cert_thumbprint_str = வெற்று;
    TCHAR cert_thumbprint_buf[CERT_THUMBPRINT_STR_LEN + 1];
    CRYPT_HASH_BLOB cert_thumbprint;
    BYTE cert_thumbprint_data[CERT_THUMBPRINT_DATA_LEN];
    HCERTSTORE cert_store = வெற்று;
    FILE *fInCert = வெற்று;
    வெறுமை *certdata = வெற்று;
    size_t certsize = 0;
    bool blob = !!sslc->cert_blob;

    எனில்(blob) {
      certdata = sslc->cert_blob->data;
      certsize = sslc->cert_blob->len;
    }
    இல்லையெனில் {
      TCHAR *cert_path =
        curlx_convert_UTF8_to_tchar(sslc->clientcert);
      எனில்(!cert_path)
        திரும்பு CURLE_OUT_OF_MEMORY;

      result = get_cert_location(cert_path, &cert_store_name,
                                 &cert_store_path, &cert_thumbprint_str);

      /* 'cert_thumbprint_str' points in to the allocated 'cert_path', copy
         the data. The string is verified to be CERT_THUMBPRINT_STR_LEN bytes
         long within the get_cert_location() function. */
      எனில்(!result && cert_thumbprint_str) {
        memcpy(cert_thumbprint_buf, cert_thumbprint_str,
               அளவு(cert_thumbprint_buf));
        cert_thumbprint_str = cert_thumbprint_buf;
      }

      curlx_free(cert_path);
      எனில்(result && (sslc->clientcert[0] != '\0'))
        fInCert = curlx_fopen(sslc->clientcert, "rb");

      எனில்(result && !fInCert) {
        failf(data, "schannel: Failed to get certificate location"
              " or file for %s",
              sslc->clientcert);
        திரும்பு result;
      }
    }

    எனில்((fInCert || blob) && sslc->cert_type &&
       !curl_strequal(sslc->cert_type, "P12")) {
      failf(data, "schannel: certificate format compatibility error "
            "for %s",
            blob ? "(memory blob)" : sslc->clientcert);
      curlx_free(cert_store_path);
      எனில்(fInCert)
        curlx_fclose(fInCert);
      திரும்பு CURLE_SSL_CERTPROBLEM;
    }

    எனில்(fInCert || blob) {
      /* Reading a .p12 or .pfx file, like the example at bottom of
         https://learn.microsoft.com/archive/msdn-technet-forums/3e7bc95f-b21a-4bcd-bd2c-7f996718cae5
       */
      CRYPT_DATA_BLOB datablob;
      WCHAR *pszPassword;
      size_t pwd_len = 0;
      முழு cert_find_flags;
      மாறிலி எழுத்து *cert_showfilename_error = blob ?
        "(memory blob)" : sslc->clientcert;
      curlx_free(cert_store_path);
      எனில்(fInCert) {
        long cert_tell = 0;
        bool continue_reading = fseek(fInCert, 0, SEEK_END) == 0;
        எனில்(continue_reading) {
          cert_tell = ftell(fInCert);
          எனில்(cert_tell < 0)
            continue_reading = FALSE;
          இல்லையெனில்
            certsize = (size_t)cert_tell;
        }
        எனில்(continue_reading)
          continue_reading = fseek(fInCert, 0, SEEK_SET) == 0;
        எனில்(continue_reading && (certsize < CURL_MAX_உள்ளீடு_LENGTH))
          certdata = curlx_malloc(certsize + 1);
        எனில்(!certdata || ((முழு)fread(certdata, certsize, 1, fInCert) != 1))
          continue_reading = FALSE;
        curlx_fclose(fInCert);
        எனில்(!continue_reading) {
          failf(data, "schannel: Failed to read cert file %s",
                sslc->clientcert);
          curlx_free(certdata);
          திரும்பு CURLE_SSL_CERTPROBLEM;
        }
      }

      /* Convert key-pair data to the in-memory certificate store */
      datablob.pbData = (BYTE *)certdata;
      datablob.cbData = (DWORD)certsize;

      எனில்(sslc->சாவி_passwd)
        pwd_len = strlen(sslc->சாவி_passwd);
      pszPassword = curlx_malloc(அளவு(WCHAR) * (pwd_len + 1));
      எனில்(pszPassword) {
        முழு str_w_len = 0;
        எனில்(pwd_len > 0)
          str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                          sslc->சாவி_passwd, (முழு)pwd_len,
                                          pszPassword, (முழு)(pwd_len + 1));

        எனில்((str_w_len >= 0) && (str_w_len <= (முழு)pwd_len))
          pszPassword[str_w_len] = 0;
        இல்லையெனில்
          pszPassword[0] = 0;

        cert_store = PFXImportCertStore(&datablob, pszPassword,
                                        PKCS12_NO_PERSIST_சாவி);
        curlx_memzero(pszPassword, அளவு(WCHAR) * (pwd_len + 1));
        curlx_free(pszPassword);
      }
      எனில்(!blob)
        curlx_free(certdata);
      எனில்(!cert_store) {
        DWORD errorcode = GetLastError();
        எனில்(errorcode == ERROR_INVALID_PASSWORD)
          failf(data, "schannel: Failed to import cert file %s, "
                "password is bad",
                cert_showfilename_error);
        இல்லையெனில்
          failf(data, "schannel: Failed to import cert file %s, "
                "last error is 0x%08lx",
                cert_showfilename_error, errorcode);
        திரும்பு CURLE_SSL_CERTPROBLEM;
      }

      /* CERT_FIND_HAS_PRIVATE_KEY is only available in Windows 8 / Server
         2012, (NT 6.2). For older versions we use CURL_FIND_ANY. */
      எனில்(curlx_verify_windows_version(6, 2, 0, PLATFORM_WINNT,
                                      VERSION_GREATER_THAN_EQUAL))
        cert_find_flags = CERT_FIND_HAS_PRIVATE_சாவி;
      இல்லையெனில்
        cert_find_flags = CERT_FIND_ANY;

      client_cert =
        CertFindCertificateInStore(cert_store,
                                   X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                   cert_find_flags, வெற்று, வெற்று);

      எனில்(!client_cert) {
        failf(data, "schannel: Failed to get certificate from file %s"
              ", last error is 0x%08lx",
              cert_showfilename_error, GetLastError());
        CertCloseStore(cert_store, 0);
        திரும்பு CURLE_SSL_CERTPROBLEM;
      }
    }
    இல்லையெனில் {
      cert_store =
        CertOpenStore(
#ifdef UNICODE
                      CERT_STORE_PROV_SYSTEM_W,
#இல்லையெனில்
                      CERT_STORE_PROV_SYSTEM_A,
#endif
                      0,
                      (HCRYPTPROV)வெற்று,
                      CERT_STORE_OPEN_EXISTING_FLAG | cert_store_name,
                      cert_store_path);
      எனில்(!cert_store) {
        எழுத்து *path_utf8 =
          curlx_convert_tchar_to_UTF8(cert_store_path);
        failf(data, "schannel: Failed to open cert store %lx %s, "
              "last error is 0x%08lx",
              cert_store_name, (path_utf8 ? path_utf8 : "(unknown)"),
              GetLastError());
        curlx_free(cert_store_path);
        curlx_free(path_utf8);
        திரும்பு CURLE_SSL_CERTPROBLEM;
      }
      curlx_free(cert_store_path);

      cert_thumbprint.pbData = cert_thumbprint_data;
      cert_thumbprint.cbData = CERT_THUMBPRINT_DATA_LEN;

      எனில்(!Crypt_String_To_இருமம்(cert_thumbprint_str,
                              CERT_THUMBPRINT_STR_LEN,
                              CRYPT_STRING_HEX,
                              cert_thumbprint_data,
                              &cert_thumbprint.cbData,
                              வெற்று, வெற்று)) {
        CertCloseStore(cert_store, 0);
        திரும்பு CURLE_SSL_CERTPROBLEM;
      }

      client_cert =
        CertFindCertificateInStore(cert_store,
                                   X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                   CERT_FIND_HASH, &cert_thumbprint, வெற்று);
      எனில்(!client_cert) {
        /* CRYPT_E_NOT_FOUND / E_INVALIDARG */
        CertCloseStore(cert_store, 0);
        failf(data, "schannel: client cert not found in cert store");
        திரும்பு CURLE_SSL_CERTPROBLEM;
      }
    }
    client_cert_store = cert_store;
  }

  *out_cert_store = client_cert_store;
  *out_cert_context = client_cert;

  திரும்பு CURLE_OK;
}

நிலையான CURLcode acquire_sspi_handle(கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு schannel_ssl_backend_data *backend,
                                    PCCERT_CONTEXT client_cert,
                                    DWORD flags,
                                    DWORD enabled_protocols)
{
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CURLcode result;

  /* We support TLS 1.3 starting in Windows Server 2022 or later
   * (OS build 20348) */
  எனில்(!conn_config->cipher_list &&
     curlx_verify_windows_version(10, 0, 20348, PLATFORM_WINNT,
                                  VERSION_GREATER_THAN_EQUAL)) {

    SCH_CREDENTIALS credentials = { 0 };
    TLS_PARAMETERS tls_parameters = { 0 };
    CRYPTO_SETTINGS crypto_settings[1];
    PCCERT_CONTEXT client_certs[1];

    எனில்(client_cert)
      client_certs[0] = client_cert;
    இல்லையெனில்
      client_certs[0] = வெற்று;

    memset(crypto_settings, 0, அளவு(crypto_settings));

    tls_parameters.pDisabledCrypto = crypto_settings;

    /* The number of blocked suites */
    tls_parameters.cDisabledCrypto = (DWORD)0;
    credentials.pTlsParameters = &tls_parameters;
    credentials.cTlsParameters = 1;

    credentials.dwVersion = SCH_CREDENTIALS_VERSION;
    credentials.dwFlags = flags | SCH_USE_STRONG_CRYPTO;

    credentials.pTlsParameters->grbitDisabledProtocols = ~enabled_protocols;

    எனில்(client_certs[0]) {
      credentials.cCreds = 1;
      credentials.paCred = client_certs;
    }

    sspi_status =
      Curl_pSecFn->AcquireCredentialsHandle(வெற்று, CURL_UNCONST(UNISP_NAME),
                                            SECPKG_CRED_OUTBOUND, வெற்று,
                                            &credentials, வெற்று, வெற்று,
                                            &backend->cred->cred_handle, வெற்று);
  }
  இல்லையெனில் {
    /* Pre-Windows 10 1809 or the user set a legacy algorithm list.
       Schannel does not negotiate TLS 1.3 when SCHANNEL_CRED is used. */
    ALG_ID algIds[இலக்கம்_CIPHERS];
    எழுத்து *ciphers = conn_config->cipher_list;
    SCHANNEL_CRED schannel_cred = { 0 };
    PCCERT_CONTEXT client_certs[1];

    எனில்(client_cert)
      client_certs[0] = client_cert;
    இல்லையெனில்
      client_certs[0] = வெற்று;

    schannel_cred.dwVersion = SCHANNEL_CRED_VERSION;
    schannel_cred.dwFlags = flags;
    schannel_cred.grbitEnabledProtocols = enabled_protocols;

    எனில்(ciphers) {
      எனில்((enabled_protocols & SP_PROT_TLS1_3_CLIENT)) {
        எனில்(!(enabled_protocols & ~SP_PROT_TLS1_3_CLIENT)) {
          failf(data, "schannel: TLS 1.3 is not supported with a cipher list; "
                "remove the cipher list or allow a lower TLS version");
          திரும்பு CURLE_SSL_CONNECT_ERROR;
        }
        infof(data, "schannel: WARNING: This version of Schannel "
              "negotiates a less-secure TLS version than TLS 1.3 because the "
              "user set an algorithm cipher list.");
      }
      result = set_ssl_ciphers(&schannel_cred, ciphers, algIds);
      எனில்(result) {
        failf(data, "schannel: Failed setting algorithm cipher list");
        திரும்பு result;
      }
    }
    இல்லையெனில் {
      schannel_cred.dwFlags = flags | SCH_USE_STRONG_CRYPTO;
    }

    எனில்(client_certs[0]) {
      schannel_cred.cCreds = 1;
      schannel_cred.paCred = client_certs;
    }

    sspi_status =
      Curl_pSecFn->AcquireCredentialsHandle(வெற்று, CURL_UNCONST(UNISP_NAME),
                                            SECPKG_CRED_OUTBOUND, வெற்று,
                                            &schannel_cred, வெற்று, வெற்று,
                                            &backend->cred->cred_handle, வெற்று);
  }

  எனில்(sspi_status != SEC_E_OK) {
    எழுத்து buffer[STRERROR_LEN];
    failf(data, "schannel: AcquireCredentialsHandle failed: %s",
          Curl_sspi_strerror(sspi_status, buffer, அளவு(buffer)));
    தேர்வு(sspi_status) {
    நிலை SEC_E_INSUFFICIENT_MEMORY:
      திரும்பு CURLE_OUT_OF_MEMORY;
    நிலை SEC_E_NO_CREDENTIALS:
    நிலை SEC_E_SECPKG_NOT_FOUND:
    நிலை SEC_E_NOT_OWNER:
    நிலை SEC_E_UNKNOWN_CREDENTIALS:
    நிலை SEC_E_INTERNAL_ERROR:
    இயல்பு:
      திரும்பு CURLE_SSL_CONNECT_ERROR;
    }
  }

  திரும்பு CURLE_OK;
}

நிலையான CURLcode schannel_acquire_credential_handle(கட்டமைப்பு Curl_cfilter *cf,
                                                   கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  PCCERT_CONTEXT client_cert = வெற்று;
  HCERTSTORE client_cert_store = வெற்று;
  CURLcode result;

  /* setup Schannel API options */
  DWORD flags = 0;
  DWORD enabled_protocols = 0;

  கட்டமைப்பு schannel_ssl_backend_data *backend =
    (கட்டமைப்பு schannel_ssl_backend_data *)connssl->backend;

  DEBUGASSERT(backend);

  எனில்(conn_config->verifypeer) {
    எனில்(backend->use_manual_cred_validation)
      flags = SCH_CRED_MANUAL_CRED_VALIDATION;
    இல்லையெனில்
      flags = SCH_CRED_AUTO_CRED_VALIDATION;

    எனில்(conn_config->no_revoke) {
      flags |= SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
               SCH_CRED_IGNORE_REVOCATION_OFFLINE;

      DEBUGF(infof(data, "schannel: disabled server certificate revocation "
                         "checks"));
    }
    இல்லையெனில் எனில்(conn_config->revoke_best_effort) {
      flags |= SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
               SCH_CRED_IGNORE_REVOCATION_OFFLINE |
               SCH_CRED_REVOCATION_CHECK_CHAIN;

      DEBUGF(infof(data, "schannel: ignore revocation offline errors"));
    }
    இல்லையெனில் {
      flags |= SCH_CRED_REVOCATION_CHECK_CHAIN;

      DEBUGF(infof(data, "schannel: checking server certificate revocation"));
    }
  }
  இல்லையெனில் {
    flags = SCH_CRED_MANUAL_CRED_VALIDATION |
            SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
            SCH_CRED_IGNORE_REVOCATION_OFFLINE;
    DEBUGF(infof(data, "schannel: disabled server cert revocation checks"));
  }

  எனில்(!conn_config->verifyhost) {
    flags |= SCH_CRED_NO_SERVERNAME_CHECK;
    DEBUGF(infof(data, "schannel: verifyhost setting prevents Schannel from "
                       "comparing the supplied target name with the subject "
                       "names in server certificates."));
  }

  எனில்(!conn_config->auto_client_cert) {
    flags &= ~(DWORD)SCH_CRED_USE_DEFAULT_CREDS;
    flags |= SCH_CRED_NO_DEFAULT_CREDS;
    infof(data, "schannel: disabled automatic use of client certificate");
  }
  இல்லையெனில்
    infof(data, "schannel: enabled automatic use of client certificate");

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  தேர்வு(conn_config->version) {
  நிலை CURL_SSLVERSION_TLSv1:
  நிலை CURL_SSLVERSION_TLSv1_0:
  நிலை CURL_SSLVERSION_TLSv1_1:
  நிலை CURL_SSLVERSION_TLSv1_2:
  நிலை CURL_SSLVERSION_TLSv1_3: {
    result = schannel_set_ssl_version_min_max(&enabled_protocols, cf, data);
    எனில்(result)
      திரும்பு result;
    நிறுத்து;
  }
  நிலை CURL_SSLVERSION_SSLv3:
  நிலை CURL_SSLVERSION_SSLv2:
    failf(data, "SSL versions not supported");
    திரும்பு CURLE_NOT_BUILT_IN;
  இயல்பு:
    failf(data, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    திரும்பு CURLE_SSL_CONNECT_ERROR;
  }

  result = get_client_cert(cf, data, &client_cert_store, &client_cert);
  எனில்(result)
    திரும்பு result;

  /* allocate memory for the reusable credential handle */
  backend->cred = (கட்டமைப்பு Curl_schannel_cred *)
    curlx_calloc(1, அளவு(கட்டமைப்பு Curl_schannel_cred));
  எனில்(!backend->cred) {
    failf(data, "schannel: unable to allocate memory");

    எனில்(client_cert)
      CertFreeCertificateContext(client_cert);
    எனில்(client_cert_store)
      CertCloseStore(client_cert_store, 0);

    திரும்பு CURLE_OUT_OF_MEMORY;
  }
  backend->cred->refcount = 1;

  /* Since we did not persist the key, we need to extend the store's
   * lifetime until the end of the connection
   */
  backend->cred->client_cert_store = client_cert_store;

  result = acquire_sspi_handle(cf, data, backend, client_cert,
                               flags, enabled_protocols);

  எனில்(client_cert)
    CertFreeCertificateContext(client_cert);

  திரும்பு result;
}

நிலையான CURLcode schannel_connect_step1(கட்டமைப்பு Curl_cfilter *cf,
                                       கட்டமைப்பு Curl_easy *data)
{
  size_t written = 0;
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு schannel_ssl_backend_data *backend =
    (கட்டமைப்பு schannel_ssl_backend_data *)connssl->backend;
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SecBuffer outbuf;
  SecBufferDesc outbuf_desc;
  SecBuffer inbuf;
  SecBufferDesc inbuf_desc;
  unsigned எழுத்து alpn_buffer[128];
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CURLcode result;

  DEBUGASSERT(backend);
  DEBUGF(infof(data, "schannel: SSL/TLS connection with %s port %d (step 1/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->port));

  backend->use_alpn = connssl->alpn && s_win_has_alpn;

  எனில்(conn_config->CAfile || conn_config->ca_info_blob) {
    எனில்(curlx_verify_windows_version(6, 1, 0, PLATFORM_WINNT,
                                    VERSION_GREATER_THAN_EQUAL)) {
      backend->use_manual_cred_validation = TRUE;
    }
    இல்லையெனில் {
      failf(data, "schannel: this version of Windows is too old to support "
                  "certificate verification via CA bundle file.");
      திரும்பு CURLE_SSL_CACERT_BADFILE;
    }
  }
  இல்லையெனில்
    backend->use_manual_cred_validation = FALSE;

  backend->cred = வெற்று;

  /* check for an existing reusable credential handle */
  எனில்(Curl_ssl_scache_use(cf, data)) {
    கட்டமைப்பு Curl_schannel_cred *old_cred;
    Curl_ssl_scache_lock(data);
    old_cred = Curl_ssl_scache_get_obj(cf, data, connssl->peer.scache_சாவி);
    எனில்(old_cred) {
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

  எனில்(!backend->cred) {
    எழுத்து *snihost;
    result = schannel_acquire_credential_handle(cf, data);
    எனில்(result || !backend->cred)
      திரும்பு result;
    /* schannel_acquire_credential_handle() sets backend->cred accordingly or
       it returns error otherwise. */

    /* A hostname associated with the credential is needed by
       InitializeSecurityContext for SNI and other reasons. */
    snihost = connssl->peer.sni ?
      connssl->peer.sni : connssl->peer.origin->hostname;
    backend->cred->sni_hostname = curlx_convert_UTF8_to_tchar(snihost);
    எனில்(!backend->cred->sni_hostname)
      திரும்பு CURLE_OUT_OF_MEMORY;
  }

  /* Warn if SNI is disabled due to use of an IP address */
  எனில்(connssl->peer.type != CURL_SSL_PEER_DNS) {
    infof(data, "schannel: using IP address, SNI is not supported by OS.");
  }

  எனில்(backend->use_alpn) {
    முழு cur = 0;
    முழு list_start_index = 0;
    unsigned முழு *extension_len = வெற்று;
    unsigned short *list_len = வெற்று;
    மாறிலி unsigned முழு uiALPN = SecApplicationProtocolNegotiationExt_ALPN;
    கட்டமைப்பு alpn_proto_buf proto;

    /* The first four bytes is an unsigned int indicating number
       of bytes of data in the rest of the buffer. */
    extension_len = (unsigned முழு *)(வெறுமை *)&alpn_buffer[cur];
    cur += (முழு)அளவு(unsigned முழு);

    /* The next four bytes are an indicator that this buffer contains
       ALPN data, as opposed to NPN, for example. */
    memcpy(&alpn_buffer[cur], &uiALPN, அளவு(uiALPN));
    cur += (முழு)அளவு(unsigned முழு);

    /* The next two bytes is an unsigned short indicating the number
       of bytes used to list the preferred protocols. */
    list_len = (unsigned short *)(வெறுமை *)&alpn_buffer[cur];
    cur += (முழு)அளவு(unsigned short);

    list_start_index = cur;

    result = Curl_alpn_to_proto_buf(&proto, connssl->alpn);
    எனில்(result) {
      failf(data, "Error setting ALPN");
      திரும்பு CURLE_SSL_CONNECT_ERROR;
    }
    memcpy(&alpn_buffer[cur], proto.data, proto.len);
    cur += proto.len;

    *list_len = curlx_uitous(cur - list_start_index);
    *extension_len = (unsigned முழு)(*list_len +
      அளவு(unsigned முழு) + அளவு(unsigned short));

    InitSecBuffer(&inbuf, SECBUFFER_APPLICATION_PROTOCOLS, alpn_buffer, cur);
    InitSecBufferDesc(&inbuf_desc, &inbuf, 1);

    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
  }
  இல்லையெனில் {
    InitSecBuffer(&inbuf, SECBUFFER_EMPTY, வெற்று, 0);
    InitSecBufferDesc(&inbuf_desc, &inbuf, 1);
  }

  /* setup output buffer */
  InitSecBuffer(&outbuf, SECBUFFER_EMPTY, வெற்று, 0);
  InitSecBufferDesc(&outbuf_desc, &outbuf, 1);

  /* security request flags */
  backend->req_flags =
    ISC_REQ_SEQUENCE_DETECT | ISC_REQ_REPLAY_DETECT |
    ISC_REQ_CONFIDENTIALITY | ISC_REQ_ALLOCATE_MEMORY |
    ISC_REQ_STREAM |
    (!conn_config->auto_client_cert ? ISC_REQ_USE_SUPPLIED_CREDS : 0);

  /* allocate memory for the security context handle */
  backend->ctxt = (கட்டமைப்பு Curl_schannel_ctxt *)
    curlx_calloc(1, அளவு(கட்டமைப்பு Curl_schannel_ctxt));
  எனில்(!backend->ctxt) {
    failf(data, "schannel: unable to allocate memory");
    திரும்பு CURLE_OUT_OF_MEMORY;
  }

  /* Schannel InitializeSecurityContext:
     https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-initializesecuritycontextw

     At the moment we do not pass inbuf unless we are using ALPN since we only
     use it for that, and WINE (for which we currently disable ALPN) is giving
     us problems with inbuf regardless. https://github.com/curl/curl/issues/983
   */
  sspi_status = Curl_pSecFn->InitializeSecurityContext(
    &backend->cred->cred_handle, வெற்று, backend->cred->sni_hostname,
    backend->req_flags, 0, 0,
    (backend->use_alpn ? &inbuf_desc : வெற்று),
    0, &backend->ctxt->ctxt_handle,
    &outbuf_desc, &backend->ret_flags, வெற்று);

  எனில்(sspi_status != SEC_I_CONTINUE_NEEDED) {
    எழுத்து buffer[STRERROR_LEN];
    curlx_safefree(backend->ctxt);
    தேர்வு(sspi_status) {
    நிலை SEC_E_INSUFFICIENT_MEMORY:
      failf(data, "schannel: initial InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(sspi_status, buffer, அளவு(buffer)));
      திரும்பு CURLE_OUT_OF_MEMORY;
    நிலை SEC_E_WRONG_PRINCIPAL:
      failf(data, "schannel: SNI or certificate check failed: %s",
            Curl_sspi_strerror(sspi_status, buffer, அளவு(buffer)));
      திரும்பு CURLE_PEER_FAILED_VERIFICATION;
#எனில் 0
    நிலை SEC_E_INVALID_HANDLE:
    நிலை SEC_E_INVALID_TOKEN:
    நிலை SEC_E_LOGON_DENIED:
    நிலை SEC_E_TARGET_UNKNOWN:
    நிலை SEC_E_NO_AUTHENTICATING_AUTHORITY:
    நிலை SEC_E_INTERNAL_ERROR:
    நிலை SEC_E_NO_CREDENTIALS:
    நிலை SEC_E_UNSUPPORTED_FUNCTION:
    நிலை SEC_E_APPLICATION_PROTOCOL_MISMATCH:
#endif
    இயல்பு:
      failf(data, "schannel: initial InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(sspi_status, buffer, அளவு(buffer)));
      திரும்பு CURLE_SSL_CONNECT_ERROR;
    }
  }

  DEBUGF(infof(data, "schannel: sending initial handshake data: "
               "sending %lu bytes.", outbuf.cbBuffer));

  /* send initial handshake data which is now stored in output buffer */
  result = Curl_conn_cf_send(cf->next, data,
                             (மாறிலி uint8_t *)outbuf.pvBuffer,
                             outbuf.cbBuffer, FALSE,
                             &written);
  Curl_pSecFn->FreeContextBuffer(outbuf.pvBuffer);
  எனில்(result || (outbuf.cbBuffer != written)) {
    failf(data, "schannel: failed to send initial handshake data: "
          "sent %zu of %lu bytes", written, outbuf.cbBuffer);
    திரும்பு CURLE_SSL_CONNECT_ERROR;
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
  memset(&backend->shutdown_token, 0, அளவு(backend->shutdown_token));

  /* continue to second handshake step */
  connssl->connecting_state = ssl_connect_2;

  திரும்பு CURLE_OK;
}

நிலையான CURLcode schannel_error(கட்டமைப்பு Curl_easy *data,
                               SECURITY_STATUS sspi_status)
{
  எழுத்து buffer[STRERROR_LEN];
  தேர்வு(sspi_status) {
  நிலை SEC_E_INSUFFICIENT_MEMORY:
    failf(data, "schannel: next InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(sspi_status, buffer, அளவு(buffer)));
    திரும்பு CURLE_OUT_OF_MEMORY;
  நிலை SEC_E_WRONG_PRINCIPAL:
    failf(data, "schannel: SNI or certificate check failed: %s",
          Curl_sspi_strerror(sspi_status, buffer, அளவு(buffer)));
    திரும்பு CURLE_PEER_FAILED_VERIFICATION;
  நிலை SEC_E_UNTRUSTED_வேர்:
    failf(data, "schannel: %s",
          Curl_sspi_strerror(sspi_status, buffer, அளவு(buffer)));
    திரும்பு CURLE_PEER_FAILED_VERIFICATION;
#எனில் 0
  நிலை SEC_E_INVALID_HANDLE:
  நிலை SEC_E_INVALID_TOKEN:
  நிலை SEC_E_LOGON_DENIED:
  நிலை SEC_E_TARGET_UNKNOWN:
  நிலை SEC_E_NO_AUTHENTICATING_AUTHORITY:
  நிலை SEC_E_INTERNAL_ERROR:
  நிலை SEC_E_NO_CREDENTIALS:
  நிலை SEC_E_UNSUPPORTED_FUNCTION:
  நிலை SEC_E_APPLICATION_PROTOCOL_MISMATCH:
#endif
  இயல்பு:
    failf(data, "schannel: next InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(sspi_status, buffer, அளவு(buffer)));
    திரும்பு CURLE_SSL_CONNECT_ERROR;
  }
}

நிலையான CURLcode schannel_pkp_pin_peer_pubkey(கட்டமைப்பு Curl_cfilter *cf,
                                             கட்டமைப்பு Curl_easy *data,
                                             மாறிலி எழுத்து *pinnedpubkey)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு schannel_ssl_backend_data *backend =
    (கட்டமைப்பு schannel_ssl_backend_data *)connssl->backend;
  CERT_CONTEXT *pCertContextServer = வெற்று;

  /* Result is returned to caller */
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  DEBUGASSERT(backend);

  /* if a path was not specified, do not pin */
  எனில்(!pinnedpubkey)
    திரும்பு CURLE_OK;

  செய் {
    SECURITY_STATUS sspi_status;
    uint8_t *x509_der;
    DWORD x509_der_len;
    கட்டமைப்பு Curl_X509certificate x509_parsed;
    கட்டமைப்பு Curl_asn1Element *pubkey;

    sspi_status =
      Curl_pSecFn->QueryContextAttributes(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                          &pCertContextServer);

    எனில்((sspi_status != SEC_E_OK) || !pCertContextServer) {
      எழுத்து buffer[STRERROR_LEN];
      failf(data, "schannel: Failed to read remote certificate context: %s",
            Curl_sspi_strerror(sspi_status, buffer, அளவு(buffer)));
      நிறுத்து; /* failed */
    }

    எனில்(!(((pCertContextServer->dwCertEncodingType & X509_ASN_ENCODING) != 0) &&
         (pCertContextServer->cbCertEncoded > 0)))
      நிறுத்து;

    x509_der = (uint8_t *)pCertContextServer->pbCertEncoded;
    x509_der_len = pCertContextServer->cbCertEncoded;
    memset(&x509_parsed, 0, அளவு(x509_parsed));
    எனில்(Curl_parseX509(&x509_parsed, x509_der, x509_der + x509_der_len))
      நிறுத்து;

    pubkey = &x509_parsed.subject_Public_சாவி_Info;
    எனில்(!pubkey->header || pubkey->end <= pubkey->header) {
      failf(data, "SSL: failed retrieving public key from server certificate");
      நிறுத்து;
    }

    result = Curl_pin_peer_pubkey(data,
                                  pinnedpubkey,
                                  (மாறிலி unsigned எழுத்து *)pubkey->header,
                                  (size_t)(pubkey->end - pubkey->header));
    எனில்(result) {
      failf(data, "SSL: public key does not match pinned public key");
    }
  } போது(0);

  எனில்(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  திரும்பு result;
}

நிலையான CURLcode ensure_encoding_size(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு sbuffer *encdata,
                                     size_t min_length)
{
  size_t size;
  DEBUGASSERT(encdata->length >= encdata->offset);
  எனில்(encdata->length < encdata->offset)
    திரும்பு CURLE_FAILED_INIT;
  size = encdata->length - encdata->offset;
  எனில்(size < CURL_SCHANNEL_BUFFER_FREE_SIZE || encdata->length < min_length) {
    unsigned எழுத்து *buffer;
    size_t length = encdata->offset + CURL_SCHANNEL_BUFFER_FREE_SIZE;
    எனில்(length < min_length)
      length = min_length;
    buffer = curlx_realloc(encdata->buffer, length);
    எனில்(!buffer) {
      failf(data, "schannel: unable to re-allocate memory");
      திரும்பு CURLE_OUT_OF_MEMORY;
    }

    encdata->buffer = buffer;
    encdata->length = length;
    SCH_DEV(infof(data, "schannel: encdata.buffer resized %zu",
                  encdata->length));
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ensure_decoding_size(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு sbuffer *decdata,
                                     size_t nowsize,
                                     size_t len)
{
  size_t size = nowsize > CURL_SCHANNEL_BUFFER_FREE_SIZE ?
    nowsize : CURL_SCHANNEL_BUFFER_FREE_SIZE;
  DEBUGASSERT(decdata->length >= decdata->offset);
  எனில்(decdata->length < decdata->offset)
    திரும்பு CURLE_FAILED_INIT;
  இல்லையெனில் எனில்(decdata->length - decdata->offset < size ||
          decdata->length < len) {
    /* increase internal decrypted data buffer */
    size_t length = decdata->offset + size;
    unsigned எழுத்து *buffer;
    /* make sure that the requested amount of data fits */
    எனில்(length < len)
      length = len;

    buffer = curlx_realloc(decdata->buffer, length);
    எனில்(!buffer) {
      failf(data, "schannel: unable to re-allocate memory");
      திரும்பு CURLE_OUT_OF_MEMORY;
    }
    decdata->buffer = buffer;
    decdata->length = length;
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode schannel_connect_step2(கட்டமைப்பு Curl_cfilter *cf,
                                       கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு schannel_ssl_backend_data *backend =
    (கட்டமைப்பு schannel_ssl_backend_data *)connssl->backend;
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  முழு i;
  size_t nread = 0;
  SecBuffer outbuf[3];
  SecBufferDesc outbuf_desc;
  SecBuffer inbuf[2];
  SecBufferDesc inbuf_desc;
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CURLcode result;
  bool doread;
  மாறிலி எழுத்து *pubkey_ptr;

  DEBUGASSERT(backend);

  doread = (connssl->io_need & CURL_SSL_IO_NEED_SEND) ? FALSE : TRUE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  DEBUGF(infof(data, "schannel: SSL/TLS connection with %s port %d (step 2/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->port));

  எனில்(!backend->cred || !backend->ctxt)
    திரும்பு CURLE_SSL_CONNECT_ERROR;

  /* buffer to store previously received and decrypted data */
  எனில்(!backend->decdata.buffer) {
    backend->decdata.offset = 0;
    backend->decdata.length = CURL_SCHANNEL_BUFFER_INIT_SIZE;
    backend->decdata.buffer = curlx_malloc(backend->decdata.length);
    எனில்(!backend->decdata.buffer) {
      failf(data, "schannel: unable to allocate memory");
      திரும்பு CURLE_OUT_OF_MEMORY;
    }
  }

  /* buffer to store previously received and encrypted data */
  எனில்(!backend->encdata.buffer) {
    backend->encdata_is_incomplete = FALSE;
    backend->encdata.offset = 0;
    backend->encdata.length = CURL_SCHANNEL_BUFFER_INIT_SIZE;
    backend->encdata.buffer = curlx_malloc(backend->encdata.length);
    எனில்(!backend->encdata.buffer) {
      failf(data, "schannel: unable to allocate memory");
      திரும்பு CURLE_OUT_OF_MEMORY;
    }
  }

  result = ensure_encoding_size(data, &backend->encdata, 0);
  எனில்(result)
    திரும்பு result;

  க்கு(;;) {
    எனில்(doread) {
      /* read encrypted handshake data from socket */
      result = Curl_conn_cf_recv(cf->next, data,
                                 (எழுத்து *)(backend->encdata.buffer +
                                          backend->encdata.offset),
                                 backend->encdata.length -
                                 backend->encdata.offset,
                                 &nread);
      எனில்(result == CURLE_AGAIN) {
        எனில்(!backend->encdata.offset || backend->encdata_is_incomplete) {
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          DEBUGF(infof(data, "schannel: failed to receive handshake, "
                       "need more data"));
          திரும்பு CURLE_OK;
        }
        இல்லையெனில் {
          DEBUGF(infof(data, "schannel: no new handshake data received, "
                       "continuing to process existing handshake data"));
        }
      }
      இல்லையெனில் எனில்(result || (nread == 0)) {
        failf(data, "schannel: failed to receive handshake, "
              "SSL/TLS connection failed");
        திரும்பு CURLE_SSL_CONNECT_ERROR;
      }
      இல்லையெனில் {
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
                  curlx_memdup0((மாறிலி எழுத்து *)backend->encdata.buffer,
                                backend->encdata.offset),
                  curlx_uztoul(backend->encdata.offset));
    InitSecBuffer(&inbuf[1], SECBUFFER_EMPTY, வெற்று, 0);
    InitSecBufferDesc(&inbuf_desc, inbuf, 2);

    /* setup output buffers */
    InitSecBuffer(&outbuf[0], SECBUFFER_TOKEN, வெற்று, 0);
    InitSecBuffer(&outbuf[1], SECBUFFER_ALERT, வெற்று, 0);
    InitSecBuffer(&outbuf[2], SECBUFFER_EMPTY, வெற்று, 0);
    InitSecBufferDesc(&outbuf_desc, outbuf, 3);

    எனில்(!inbuf[0].pvBuffer) {
      failf(data, "schannel: unable to allocate memory");
      திரும்பு CURLE_OUT_OF_MEMORY;
    }

    /* The socket must be writable (or a poll error occurred) before we call
       InitializeSecurityContext to continue processing the received TLS
       records. This is because that function is not idempotent and we do not
       support partial save/resume sending replies of handshake tokens. */
    எனில்(!SOCKET_WRITABLE(Curl_conn_cf_get_socket(cf, data), 0)) {
      SCH_DEV(infof(data, "schannel: handshake waiting for writable socket"));
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      curlx_free(inbuf[0].pvBuffer);
      திரும்பு CURLE_OK;
    }

    sspi_status = Curl_pSecFn->InitializeSecurityContext(
      &backend->cred->cred_handle, &backend->ctxt->ctxt_handle,
      backend->cred->sni_hostname, backend->req_flags,
      0, 0, &inbuf_desc, 0, வெற்று,
      &outbuf_desc, &backend->ret_flags, வெற்று);

    /* free buffer for received handshake data */
    curlx_safefree(inbuf[0].pvBuffer);

    /* check if the handshake was incomplete */
    தேர்வு(sspi_status) {
    நிலை SEC_E_INCOMPLETE_MESSAGE:
      backend->encdata_is_incomplete = TRUE;
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      DEBUGF(infof(data,
                   "schannel: received incomplete message, need more data"));
      திரும்பு CURLE_OK;

    நிலை SEC_I_CONTINUE_NEEDED:
    நிலை SEC_E_OK:
      /* check if the handshake needs to be continued */
      result = CURLE_OK;
      க்கு(i = 0; i < 3; i++) {
        /* search for handshake tokens that need to be sent */
        எனில்(outbuf[i].BufferType == SECBUFFER_TOKEN && outbuf[i].cbBuffer > 0) {
          size_t written = 0;
          DEBUGF(infof(data, "schannel: sending next handshake data: "
                       "sending %lu bytes.", outbuf[i].cbBuffer));

          /* send handshake token to server */
          result = Curl_conn_cf_send(cf->next, data,
                                     (மாறிலி uint8_t *)outbuf[i].pvBuffer,
                                     outbuf[i].cbBuffer,
                                     FALSE, &written);
          எனில்(result || (outbuf[i].cbBuffer != written)) {
            failf(data, "schannel: failed to send next handshake data: "
                  "sent %zu of %lu bytes", written, outbuf[i].cbBuffer);
            result = CURLE_SSL_CONNECT_ERROR;
          }
        }
      }
      க்கு(i = 0; i < 3; i++) {
        /* free obsolete buffer */
        எனில்(outbuf[i].pvBuffer)
          Curl_pSecFn->FreeContextBuffer(outbuf[i].pvBuffer);
      }
      எனில்(result)
        திரும்பு result;
      நிறுத்து;

    நிலை SEC_I_INCOMPLETE_CREDENTIALS:
      எனில்(!(backend->req_flags & ISC_REQ_USE_SUPPLIED_CREDS)) {
        /* If the server has requested a client certificate, attempt to
           continue the handshake without one. This allows connections to
           servers which request a client certificate but do not require
           it. */
        backend->req_flags |= ISC_REQ_USE_SUPPLIED_CREDS;
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        DEBUGF(infof(data,
                     "schannel: a client certificate has been requested"));
        திரும்பு CURLE_OK;
      }
      FALLTHROUGH();

    இயல்பு:
      திரும்பு schannel_error(data, sspi_status);
    }

    /* check if there was additional remaining encrypted data */
    எனில்(inbuf[1].BufferType == SECBUFFER_EXTRA && inbuf[1].cbBuffer > 0) {
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
      எனில்(backend->encdata.offset > inbuf[1].cbBuffer) {
        memmove(backend->encdata.buffer,
                (backend->encdata.buffer + backend->encdata.offset) -
                inbuf[1].cbBuffer, inbuf[1].cbBuffer);
        backend->encdata.offset = inbuf[1].cbBuffer;
        எனில்(sspi_status == SEC_I_CONTINUE_NEEDED) {
          doread = FALSE;
          தொடர்;
        }
      }
    }
    இல்லையெனில் {
      backend->encdata.offset = 0;
    }
    நிறுத்து;
  }

  /* check if the handshake needs to be continued */
  எனில்(sspi_status == SEC_I_CONTINUE_NEEDED) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    திரும்பு CURLE_OK;
  }

  /* check if the handshake is complete */
  எனில்(sspi_status == SEC_E_OK) {
    connssl->connecting_state = ssl_connect_3;
    DEBUGF(infof(data, "schannel: SSL/TLS handshake complete"));
  }

#ifndef CURL_DISABLE_PROXY
  pubkey_ptr = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#இல்லையெனில்
  pubkey_ptr = CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
  எனில்(pubkey_ptr) {
    result = schannel_pkp_pin_peer_pubkey(cf, data, pubkey_ptr);
    எனில்(result) {
      failf(data, "SSL: public key does not match pinned public key");
      திரும்பு result;
    }
  }

  எனில்(conn_config->verifypeer && backend->use_manual_cred_validation) {
    /* Certificate verification also verifies the hostname if verifyhost */
    திரும்பு Curl_verify_certificate(cf, data);
  }

  /* Verify the hostname manually when certificate verification is disabled,
     because in that case Schannel does not verify it. */
  எனில்(!conn_config->verifypeer && conn_config->verifyhost)
    திரும்பு Curl_verify_host(cf, data);

  திரும்பு CURLE_OK;
}

நிலையான bool valid_cert_encoding(மாறிலி CERT_CONTEXT *cert_context)
{
  திரும்பு cert_context &&
    ((cert_context->dwCertEncodingType & X509_ASN_ENCODING) != 0) &&
    cert_context->pbCertEncoded &&
    (cert_context->cbCertEncoded > 0);
}

வகைவரை bool (*Read_crt_func)(மாறிலி CERT_CONTEXT *ccert_context,
                              bool reverse_order, வெறுமை *arg);

நிலையான வெறுமை traverse_cert_store(மாறிலி CERT_CONTEXT *context,
                                Read_crt_func func, வெறுமை *arg)
{
  மாறிலி CERT_CONTEXT *current_context = வெற்று;
  bool should_continue = TRUE;
  bool first = TRUE;
  bool reverse_order = FALSE;
  போது(should_continue &&
        (current_context = CertEnumCertificatesInStore(
          context->hCertStore,
          current_context)) != வெற்று) {
    /* Windows 11 22H2 OS Build 22621.674 or higher enumerates certificates in
       leaf-to-root order while all previous versions of Windows enumerate
       certificates in root-to-leaf order. Determine the order of enumeration
       by comparing SECPKG_ATTR_REMOTE_CERT_CONTEXT's pbCertContext with the
       first certificate's pbCertContext. */
    எனில்(first && context->pbCertEncoded != current_context->pbCertEncoded)
      reverse_order = TRUE;
    should_continue = func(current_context, reverse_order, arg);
    first = FALSE;
  }

  எனில்(current_context)
    CertFreeCertificateContext(current_context);
}

நிலையான bool cert_counter_callback(மாறிலி CERT_CONTEXT *ccert_context,
                                  bool reverse_order, வெறுமை *certs_மொத்தம்)
{
  (வெறுமை)reverse_order;
  எனில்(valid_cert_encoding(ccert_context))
    (*(முழு *)certs_மொத்தம்)++;
  எனில்(*(முழு *)certs_மொத்தம் > MAX_ALLOWED_CERT_AMOUNT)
    திரும்பு FALSE;
  திரும்பு TRUE;
}

கட்டமைப்பு Adder_args {
  கட்டமைப்பு Curl_easy *data;
  CURLcode result;
  முழு idx;
  முழு certs_மொத்தம்;
};

நிலையான bool add_cert_to_certinfo(மாறிலி CERT_CONTEXT *ccert_context,
                                 bool reverse_order, வெறுமை *raw_arg)
{
  கட்டமைப்பு Adder_args *args = (கட்டமைப்பு Adder_args *)raw_arg;
  args->result = CURLE_OK;
  எனில்(valid_cert_encoding(ccert_context)) {
    முழு செருகு_index = reverse_order ? (args->certs_மொத்தம் - 1) - args->idx :
                       args->idx;
    args->result = Curl_extract_certinfo(args->data, செருகு_index,
                                         ccert_context->pbCertEncoded,
                                         ccert_context->pbCertEncoded +
                                         ccert_context->cbCertEncoded);
    args->idx++;
  }
  திரும்பு args->result == CURLE_OK;
}

நிலையான வெறுமை schannel_session_free(வெறுமை *sessionid)
{
  /* this is expected to be called under sessionid lock */
  கட்டமைப்பு Curl_schannel_cred *cred = sessionid;

  எனில்(cred) {
    cred->refcount--;
    எனில்(cred->refcount == 0) {
      Curl_pSecFn->FreeCredentialsHandle(&cred->cred_handle);
      curlx_free(cred->sni_hostname);
      எனில்(cred->client_cert_store) {
        CertCloseStore(cred->client_cert_store, 0);
        cred->client_cert_store = வெற்று;
      }
      curlx_safefree(cred);
    }
  }
}

நிலையான CURLcode schannel_connect_step3(கட்டமைப்பு Curl_cfilter *cf,
                                       கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு schannel_ssl_backend_data *backend =
    (கட்டமைப்பு schannel_ssl_backend_data *)connssl->backend;
  கட்டமைப்பு ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  CURLcode result = CURLE_OK;
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CERT_CONTEXT *ccert_context = வெற்று;
  SecPkgContext_ApplicationProtocol alpn_result;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);
  DEBUGASSERT(backend);

  DEBUGF(infof(data, "schannel: SSL/TLS connection with %s port %d (step 3/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->port));

  எனில்(!backend->cred)
    திரும்பு CURLE_SSL_CONNECT_ERROR;

  /* check if the required context attributes are met */
  எனில்(backend->ret_flags != backend->req_flags) {
    எனில்(!(backend->ret_flags & ISC_RET_SEQUENCE_DETECT))
      failf(data, "schannel: failed to setup sequence detection");
    எனில்(!(backend->ret_flags & ISC_RET_REPLAY_DETECT))
      failf(data, "schannel: failed to setup replay detection");
    எனில்(!(backend->ret_flags & ISC_RET_CONFIDENTIALITY))
      failf(data, "schannel: failed to setup confidentiality");
    எனில்(!(backend->ret_flags & ISC_RET_ALLOCATED_MEMORY))
      failf(data, "schannel: failed to setup memory allocation");
    எனில்(!(backend->ret_flags & ISC_RET_STREAM))
      failf(data, "schannel: failed to setup stream orientation");
    திரும்பு CURLE_SSL_CONNECT_ERROR;
  }

  எனில்(backend->use_alpn) {
    sspi_status =
      Curl_pSecFn->QueryContextAttributes(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_APPLICATION_PROTOCOL,
                                          &alpn_result);

    எனில்(sspi_status != SEC_E_OK) {
      failf(data, "schannel: failed to retrieve ALPN result");
      திரும்பு CURLE_SSL_CONNECT_ERROR;
    }

    எனில்(alpn_result.ProtoNegoStatus ==
       SecApplicationProtocolNegotiationStatus_Success) {
      எனில்(backend->recv_renegotiating &&
         connssl->negotiated.alpn &&
         strncmp(connssl->negotiated.alpn,
                 (மாறிலி எழுத்து *)alpn_result.ProtocolId,
                 alpn_result.ProtocolIdSize)) {
        /* Renegotiation selected a different protocol now, we cannot
         * deal with this */
        failf(data, "schannel: server selected an ALPN protocol too late");
        திரும்பு CURLE_SSL_CONNECT_ERROR;
      }
      Curl_alpn_set_negotiated(cf, data, connssl, alpn_result.ProtocolId,
                               alpn_result.ProtocolIdSize);
    }
    இல்லையெனில் {
      எனில்(!backend->recv_renegotiating)
        Curl_alpn_set_negotiated(cf, data, connssl, வெற்று, 0);
    }
  }

  /* save the current session data for possible reuse */
  எனில்(Curl_ssl_scache_use(cf, data)) {
    Curl_ssl_scache_lock(data);
    /* Up ref count since call takes ownership */
    backend->cred->refcount++;
    result = Curl_ssl_scache_add_obj(cf, data, connssl->peer.scache_சாவி,
                                     backend->cred, schannel_session_free);
    Curl_ssl_scache_unlock(data);
    எனில்(result)
      திரும்பு result;
  }

  எனில்(ssl_config->certinfo) {
    முழு certs_மொத்தம் = 0;
    sspi_status =
      Curl_pSecFn->QueryContextAttributes(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                          &ccert_context);

    எனில்((sspi_status != SEC_E_OK) || !ccert_context) {
      failf(data, "schannel: failed to retrieve remote cert context");
      திரும்பு CURLE_PEER_FAILED_VERIFICATION;
    }

    traverse_cert_store(ccert_context, cert_counter_callback, &certs_மொத்தம்);
    எனில்(certs_மொத்தம் > MAX_ALLOWED_CERT_AMOUNT) {
      failf(data, "%d certificates is more than allowed (%d)",
            certs_மொத்தம், MAX_ALLOWED_CERT_AMOUNT);
      CertFreeCertificateContext(ccert_context);
      திரும்பு CURLE_SSL_CONNECT_ERROR;
    }

    result = Curl_ssl_init_certinfo(data, certs_மொத்தம்);
    எனில்(!result) {
      கட்டமைப்பு Adder_args args;
      args.data = data;
      args.idx = 0;
      args.certs_மொத்தம் = certs_மொத்தம்;
      args.result = CURLE_OK;
      traverse_cert_store(ccert_context, add_cert_to_certinfo, &args);
      result = args.result;
    }
    CertFreeCertificateContext(ccert_context);
    எனில்(result)
      திரும்பு result;
  }

  connssl->connecting_state = ssl_connect_done;

  திரும்பு CURLE_OK;
}

நிலையான CURLcode schannel_connect(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 bool *done)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  CURLcode result;

  /* check if the connection has already been established */
  எனில்(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  *done = FALSE;

  எனில்(connssl->connecting_state == ssl_connect_1) {
    result = schannel_connect_step1(cf, data);
    எனில்(result)
      திரும்பு result;
  }

  எனில்(connssl->connecting_state == ssl_connect_2) {
    result = schannel_connect_step2(cf, data);
    எனில்(result)
      திரும்பு result;
  }

  எனில்(connssl->connecting_state == ssl_connect_3) {
    result = schannel_connect_step3(cf, data);
    எனில்(result)
      திரும்பு result;
  }

  எனில்(connssl->connecting_state == ssl_connect_done) {
    கட்டமைப்பு schannel_ssl_backend_data *backend =
      (கட்டமைப்பு schannel_ssl_backend_data *)connssl->backend;
    DEBUGASSERT(backend);

    எனில்(Curl_pSecFn->QueryContextAttributes(
         &backend->ctxt->ctxt_handle,
         SECPKG_ATTR_STREAM_SIZES,
         &backend->stream_sizes)) {
      failf(data, "schannel: failed getting stream sizes");
      திரும்பு CURLE_SSL_CONNECT_ERROR;
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

  திரும்பு CURLE_OK;
}

எண்ணிக்கை schannel_renegotiate_caller_t {
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
நிலையான CURLcode schannel_recv_renegotiate(
  கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
  எண்ணிக்கை schannel_renegotiate_caller_t caller)
{
  CURLcode result;
  curl_socket_t sockfd;
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு schannel_ssl_backend_data *backend =
    (கட்டமைப்பு schannel_ssl_backend_data *)connssl->backend;
  கட்டமைப்பு schannel_renegotiate_state *rs = &backend->renegotiate_state;

  எனில்(!backend || !backend->recv_renegotiating) {
    failf(data, "schannel: unexpected call to schannel_recv_renegotiate");
    திரும்பு CURLE_SSL_CONNECT_ERROR;
  }
  DEBUGASSERT(caller <= SCH_RENEG_CALLER_IS_SEND);
  எனில்(caller == SCH_RENEG_CALLER_IS_RECV)
    SCH_DEV(infof(data, "schannel: renegotiation caller is schannel_recv"));
  இல்லையெனில்
    SCH_DEV(infof(data, "schannel: renegotiation caller is schannel_send"));

  sockfd = Curl_conn_cf_get_socket(cf, data);

  எனில்(sockfd == CURL_SOCKET_BAD) {
    failf(data, "schannel: renegotiation missing socket");
    திரும்பு CURLE_SSL_CONNECT_ERROR;
  }

  எனில்(!rs->started) { /* new renegotiation */
    infof(data, "schannel: renegotiating SSL/TLS connection");
    DEBUGASSERT(connssl->state == ssl_connection_complete);
    DEBUGASSERT(connssl->connecting_state == ssl_connect_done);
    connssl->state = ssl_connection_negotiating;
    connssl->connecting_state = ssl_connect_2;
    memset(rs, 0, அளவு(*rs));
    rs->io_need = CURL_SSL_IO_NEED_SEND;
    rs->start_time = *Curl_pgrs_now(data);
    rs->started = TRUE;
  }

  க்கு(;;) {
    bool block_read, block_write, blocking, done;
    curl_socket_t readfd, writefd;
    timediff_t elapsed;

    elapsed = curlx_ptimediff_ms(Curl_pgrs_now(data), &rs->start_time);
    எனில்(elapsed >= MAX_RENEG_BLOCK_TIME) {
      failf(data, "schannel: renegotiation timeout");
      result = CURLE_SSL_CONNECT_ERROR;
      நிறுத்து;
    }

    /* the current io_need state may have been overwritten since the last time
       this function was called. restore the io_need state needed to continue
       the renegotiation. */

    connssl->io_need = rs->io_need;

    result = schannel_connect(cf, data, &done);

    rs->io_need = connssl->io_need;

    எனில்(!result && !done)
      result = CURLE_AGAIN;

    எனில்(result != CURLE_AGAIN)
      நிறுத்து;

    readfd = (rs->io_need & CURL_SSL_IO_NEED_RECV) ? sockfd : CURL_SOCKET_BAD;
    writefd = (rs->io_need & CURL_SSL_IO_NEED_SEND) ? sockfd : CURL_SOCKET_BAD;

    எனில்(readfd == CURL_SOCKET_BAD && writefd == CURL_SOCKET_BAD)
      தொடர்;

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

    க்கு(;;) {
      முழு what;
      timediff_t timeout_ms, remaining;

      result = Curl_pgrsUpdate(data);
      எனில்(result)
        நிறுத்து;

      elapsed = curlx_ptimediff_ms(Curl_pgrs_now(data), &rs->start_time);
      எனில்(elapsed >= MAX_RENEG_BLOCK_TIME) {
        failf(data, "schannel: renegotiation timeout");
        result = CURLE_SSL_CONNECT_ERROR;
        நிறுத்து;
      }
      remaining = MAX_RENEG_BLOCK_TIME - elapsed;

      எனில்(blocking) {
        timeout_ms = Curl_timeleft_ms(data);

        எனில்(timeout_ms < 0) {
          result = CURLE_OPERATION_TIMEDOUT;
          நிறுத்து;
        }

        /* the blocking is in intervals so that the progress function can be
           called every second */
        எனில்(!timeout_ms || timeout_ms > 1000)
          timeout_ms = 1000;

        எனில்(timeout_ms > remaining)
          timeout_ms = remaining;
      }
      இல்லையெனில்
        timeout_ms = 0;

      SCH_DEV(infof(data, "schannel: renegotiation wait until socket is"
                    "%s%s for up to %" FMT_TIMEDIFF_T " ms",
                    ((readfd != CURL_SOCKET_BAD) ? " readable" : ""),
                    ((writefd != CURL_SOCKET_BAD) ? " writable" : ""),
                    timeout_ms));

      what = Curl_socket_check(readfd, CURL_SOCKET_BAD, writefd, timeout_ms);

      எனில்(what > 0 && (what & (CURL_CSELECT_IN | CURL_CSELECT_OUT))) {
        SCH_DEV(infof(data, "schannel: renegotiation socket %s%s",
                      ((what & CURL_CSELECT_IN) ? "CURL_CSELECT_IN " : ""),
                      ((what & CURL_CSELECT_OUT) ? "CURL_CSELECT_OUT " : "")));
        result = CURLE_AGAIN;
        நிறுத்து;
      }
      இல்லையெனில் எனில்(!what) {
        SCH_DEV(infof(data, "schannel: renegotiation socket timeout"));
        எனில்(blocking)
          தொடர்;
        இல்லையெனில்
          திரும்பு CURLE_AGAIN;
      }

      failf(data, "schannel: socket error during renegotiation");
      result = CURLE_SSL_CONNECT_ERROR;
      நிறுத்து;
    }
    எனில்(result != CURLE_AGAIN)
      நிறுத்து;
  }

  DEBUGASSERT(result != CURLE_AGAIN);

  rs->started = FALSE;
  backend->recv_renegotiating = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  எனில்(result)
    failf(data, "schannel: renegotiation failed");
  இல்லையெனில்
    infof(data, "schannel: SSL/TLS connection renegotiated");

  திரும்பு result;
}

நிலையான CURLcode schannel_send(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                              மாறிலி வெறுமை *buf, size_t len, size_t *pnwritten)
{
  size_t data_len = 0;
  unsigned எழுத்து *ptr = வெற்று;
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  SecBuffer outbuf[4];
  SecBufferDesc outbuf_desc;
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CURLcode result = CURLE_OK;
  கட்டமைப்பு schannel_ssl_backend_data *backend =
    (கட்டமைப்பு schannel_ssl_backend_data *)connssl->backend;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  எனில்(backend->recv_renegotiating) {
    result = schannel_recv_renegotiate(cf, data, SCH_RENEG_CALLER_IS_SEND);
    எனில்(result)
      திரும்பு result;
  }

  /* check if the buffer is longer than the maximum message length */
  எனில்(len > backend->stream_sizes.cbMaximumMessage) {
    len = backend->stream_sizes.cbMaximumMessage;
  }

  /* calculate the complete message length and prepare the send buffer */
  data_len = backend->stream_sizes.cbHeader + len +
    backend->stream_sizes.cbTrailer;
  எனில்(data_len > backend->send_buffer_len) {
    ptr = curlx_realloc(backend->send_buffer, data_len);
    எனில்(!ptr)
      திரும்பு CURLE_OUT_OF_MEMORY;
    backend->send_buffer = ptr;
    backend->send_buffer_len = data_len;
  }
  இல்லையெனில்
    ptr = backend->send_buffer;

  /* setup output buffers (header, data, trailer, empty) */
  InitSecBuffer(&outbuf[0], SECBUFFER_STREAM_HEADER,
                ptr, backend->stream_sizes.cbHeader);
  InitSecBuffer(&outbuf[1], SECBUFFER_DATA,
                ptr + backend->stream_sizes.cbHeader, curlx_uztoul(len));
  InitSecBuffer(&outbuf[2], SECBUFFER_STREAM_TRAILER,
                ptr + backend->stream_sizes.cbHeader + len,
                backend->stream_sizes.cbTrailer);
  InitSecBuffer(&outbuf[3], SECBUFFER_EMPTY, வெற்று, 0);
  InitSecBufferDesc(&outbuf_desc, outbuf, 4);

  /* copy data into output buffer */
  memcpy(outbuf[1].pvBuffer, buf, len);

  /* https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-encryptmessage */
  sspi_status = Curl_pSecFn->EncryptMessage(&backend->ctxt->ctxt_handle, 0,
                                            &outbuf_desc, 0);

  /* check if the message was encrypted */
  எனில்(sspi_status == SEC_E_OK) {

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
    போது(len > *pnwritten) {
      size_t this_write = 0;
      முழு what;
      timediff_t timeout_ms = Curl_timeleft_ms(data);
      எனில்(timeout_ms < 0) {
        /* we already got the timeout */
        failf(data, "schannel: timed out sending data (bytes sent: %zu)",
              *pnwritten);
        result = CURLE_OPERATION_TIMEDOUT;
        நிறுத்து;
      }
      இல்லையெனில் எனில்(!timeout_ms)
        timeout_ms = TIMEDIFF_T_MAX;
      what = SOCKET_WRITABLE(Curl_conn_cf_get_socket(cf, data), timeout_ms);
      எனில்(what < 0) {
        /* fatal error */
        failf(data, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        result = CURLE_SEND_ERROR;
        நிறுத்து;
      }
      இல்லையெனில் எனில்(what == 0) {
        failf(data, "schannel: timed out sending data (bytes sent: %zu)",
              *pnwritten);
        result = CURLE_OPERATION_TIMEDOUT;
        நிறுத்து;
      }
      /* socket is writable */

       result = Curl_conn_cf_send(cf->next, data,
                                  (மாறிலி uint8_t *)ptr + *pnwritten,
                                  len - *pnwritten,
                                  FALSE, &this_write);
      எனில்(result == CURLE_AGAIN)
        தொடர்;
      இல்லையெனில் எனில்(result) {
        நிறுத்து;
      }

      *pnwritten += this_write;
    }
  }
  இல்லையெனில் எனில்(sspi_status == SEC_E_INSUFFICIENT_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
  }
  இல்லையெனில் {
    result = CURLE_SEND_ERROR;
  }

  எனில்(len == *pnwritten)
    /* Encrypted message including header, data and trailer entirely sent.
       The return value is the number of unencrypted bytes that were sent. */
    *pnwritten = outbuf[1].cbBuffer;

  திரும்பு result;
}

நிலையான CURLcode schannel_recv(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                              எழுத்து *buf, size_t len, size_t *pnread)
{
  size_t size = 0;
  size_t nread = 0;
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  SecBuffer inbuf[4];
  SecBufferDesc inbuf_desc;
  SECURITY_STATUS sspi_status = SEC_E_OK;
  கட்டமைப்பு schannel_ssl_backend_data *backend =
    (கட்டமைப்பு schannel_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(backend);
  *pnread = 0;

  எனில்(backend->recv_renegotiating) {
    result = schannel_recv_renegotiate(cf, data, SCH_RENEG_CALLER_IS_RECV);
    எனில்(result)
      திரும்பு result;
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

  எனில்(len && len <= backend->decdata.offset) {
    SCH_DEV(infof(data,
                  "schannel: enough decrypted data is already available"));
    செல் cleanup;
  }
  இல்லையெனில் எனில்(backend->recv_unrecoverable_err) {
    result = backend->recv_unrecoverable_err;
    infof(data, "schannel: an unrecoverable error occurred in a prior call");
    செல் cleanup;
  }
  இல்லையெனில் எனில்(backend->recv_sspi_close_notify) {
    /* once a server has indicated shutdown there is no more encrypted data */
    infof(data, "schannel: server indicated shutdown in a prior call");
    செல் cleanup;
  }
  /* it is debatable what to return when !len. Regardless we cannot return
     immediately because there may be data to decrypt (in the case we want to
     decrypt all encrypted cached data) so handle !len later in cleanup. */
  இல்லையெனில் எனில்(len && !backend->recv_connection_closed) {
    /* the encrypted buffer must be large enough to hold all the bytes
       requested and some TLS record overhead. 'len' is a buffer size, so this
       integer math cannot overflow. */
    மாறிலி size_t min_encdata_length = len + CURL_SCHANNEL_BUFFER_FREE_SIZE;

    /* make sure encrypt buffer fits the requested amount of data */
    result = ensure_encoding_size(data, &backend->encdata, min_encdata_length);
    எனில்(result)
      செல் cleanup;

    SCH_DEV(infof(data,
                  "schannel: encrypted data buffer: offset %zu length %zu",
                  backend->encdata.offset, backend->encdata.length));

    /* read encrypted data from socket */
    result = Curl_conn_cf_recv(cf->next, data,
                               (எழுத்து *)(backend->encdata.buffer +
                                        backend->encdata.offset),
                               backend->encdata.length -
                               backend->encdata.offset,
                               &nread);
    எனில்(result) {
      எனில்(result == CURLE_AGAIN)
        SCH_DEV(infof(data, "schannel: recv returned CURLE_AGAIN"));
      இல்லையெனில் {
        infof(data, "schannel: recv returned error %d", (முழு)result);
        backend->recv_unrecoverable_err = result;
      }
    }
    இல்லையெனில் எனில்(nread == 0) {
      backend->recv_connection_closed = TRUE;
      DEBUGF(infof(data, "schannel: server closed the connection"));
    }
    இல்லையெனில் {
      backend->encdata.offset += nread;
      backend->encdata_is_incomplete = FALSE;
      SCH_DEV(infof(data, "schannel: encrypted data got %zu", nread));
    }
  }

  SCH_DEV(infof(data, "schannel: encrypted data buffer: offset %zu length %zu",
                backend->encdata.offset, backend->encdata.length));

  /* decrypt loop */
  போது(backend->encdata.offset > 0 && sspi_status == SEC_E_OK &&
        (!len || backend->decdata.offset < len ||
         backend->recv_connection_closed)) {
    /* prepare data buffer for DecryptMessage call */
    InitSecBuffer(&inbuf[0], SECBUFFER_DATA, backend->encdata.buffer,
                  curlx_uztoul(backend->encdata.offset));

    /* we need 3 more empty input buffers for possible output */
    InitSecBuffer(&inbuf[1], SECBUFFER_EMPTY, வெற்று, 0);
    InitSecBuffer(&inbuf[2], SECBUFFER_EMPTY, வெற்று, 0);
    InitSecBuffer(&inbuf[3], SECBUFFER_EMPTY, வெற்று, 0);
    InitSecBufferDesc(&inbuf_desc, inbuf, 4);

    /* https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-decryptmessage
     */
    sspi_status = Curl_pSecFn->DecryptMessage(&backend->ctxt->ctxt_handle,
                                              &inbuf_desc, 0, வெற்று);

    /* check if everything went fine (server may want to renegotiate
       or shutdown the connection context) */
    எனில்(sspi_status == SEC_E_OK || sspi_status == SEC_I_RENEGOTIATE ||
       sspi_status == SEC_I_CONTEXT_EXPIRED) {
      /* check for successfully decrypted data, even before actual
         renegotiation or shutdown of the connection context */
      எனில்(inbuf[1].BufferType == SECBUFFER_DATA) {
        SCH_DEV(infof(data, "schannel: decrypted data length: %lu",
                      inbuf[1].cbBuffer));

        /* ensure the decode buffer fits the received amount of data */
        result = ensure_decoding_size(data, &backend->decdata,
                                      inbuf[1].cbBuffer, len);
        எனில்(result)
          செல் cleanup;

        /* copy decrypted data to internal buffer */
        size = inbuf[1].cbBuffer;
        எனில்(size) {
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
      எனில்(inbuf[3].BufferType == SECBUFFER_EXTRA && inbuf[3].cbBuffer > 0) {
        SCH_DEV(infof(data, "schannel: encrypted data length: %lu",
                      inbuf[3].cbBuffer));

        /* check if the remaining data is less than the total amount
         * and therefore begins after the already processed data
         */
        எனில்(backend->encdata.offset > inbuf[3].cbBuffer) {
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
      இல்லையெனில் {
        /* reset encrypted buffer offset, because there is no data remaining */
        backend->encdata.offset = 0;
      }

      /* check if server wants to renegotiate the connection context */
      எனில்(sspi_status == SEC_I_RENEGOTIATE) {
        infof(data, "schannel: remote party requests renegotiation");
        எனில்(result && result != CURLE_AGAIN) {
          infof(data, "schannel: cannot renegotiate, an error is pending");
          செல் cleanup;
        }

        backend->recv_renegotiating = TRUE;
        result = schannel_recv_renegotiate(cf, data, SCH_RENEG_CALLER_IS_RECV);
        எனில்(result)
          செல் cleanup;

        /* now retry receiving data */
        sspi_status = SEC_E_OK;
        தொடர்;
      }
      /* check if the server closed the connection */
      இல்லையெனில் எனில்(sspi_status == SEC_I_CONTEXT_EXPIRED) {
        /* In Windows 2000 SEC_I_CONTEXT_EXPIRED (close_notify) is not
           returned so we have to work around that in cleanup. */
        backend->recv_sspi_close_notify = TRUE;
        எனில்(!backend->recv_connection_closed)
          backend->recv_connection_closed = TRUE;
        /* We received the close notify fine, any error we got
         * from the lower filters afterwards (e.g. the socket), is not
         * an error on the TLS data stream. That one ended here. */
        எனில்(result == CURLE_RECV_ERROR)
          result = CURLE_OK;
        infof(data,
              "schannel: server close notification received (close_notify)");
        செல் cleanup;
      }
    }
    இல்லையெனில் எனில்(sspi_status == SEC_E_INCOMPLETE_MESSAGE) {
      backend->encdata_is_incomplete = TRUE;
      எனில்(!result)
        result = CURLE_AGAIN;
      SCH_DEV(infof(data, "schannel: failed to decrypt data, need more data"));
      செல் cleanup;
    }
    இல்லையெனில் {
      எழுத்து buffer[STRERROR_LEN];
      failf(data, "schannel: failed to read data from server: %s",
            Curl_sspi_strerror(sspi_status, buffer, அளவு(buffer)));
      result = CURLE_RECV_ERROR;
      செல் cleanup;
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
  எனில்(len && !backend->decdata.offset && backend->recv_connection_closed &&
     !backend->recv_sspi_close_notify) {
    result = CURLE_RECV_ERROR;
    failf(data, "schannel: server closed abruptly (missing close_notify)");
  }

  /* Any error other than CURLE_AGAIN is an unrecoverable error. */
  எனில்(result && result != CURLE_AGAIN)
    backend->recv_unrecoverable_err = result;

  size = len < backend->decdata.offset ? len : backend->decdata.offset;
  எனில்(size) {
    memcpy(buf, backend->decdata.buffer, size);
    memmove(backend->decdata.buffer, backend->decdata.buffer + size,
            backend->decdata.offset - size);
    backend->decdata.offset -= size;
    SCH_DEV(infof(data, "schannel: decrypted data returned %zu", size));
    SCH_DEV(infof(data,
                  "schannel: decrypted data buffer: offset %zu length %zu",
                  backend->decdata.offset, backend->decdata.length));
    *pnread = size;
    திரும்பு CURLE_OK;
  }

  எனில்(!result && !backend->recv_connection_closed)
    result = CURLE_AGAIN;

  /* it is debatable what to return when !len. We could return whatever error
     we got from decryption but instead we override here so the return is
     consistent. */
  எனில்(!len)
    திரும்பு CURLE_OK;

  திரும்பு result;
}

நிலையான bool schannel_data_pending(கட்டமைப்பு Curl_cfilter *cf,
                                  மாறிலி கட்டமைப்பு Curl_easy *data)
{
  மாறிலி கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு schannel_ssl_backend_data *backend =
    (கட்டமைப்பு schannel_ssl_backend_data *)connssl->backend;

  (வெறுமை)data;
  DEBUGASSERT(backend);

  எனில்(backend->ctxt) /* SSL/TLS is in use */
    திரும்பு backend->decdata.offset > 0 ||
           (backend->encdata.offset > 0 && !backend->encdata_is_incomplete) ||
           backend->recv_connection_closed ||
           backend->recv_sspi_close_notify ||
           backend->recv_unrecoverable_err;
  இல்லையெனில்
    திரும்பு FALSE;
}

/* shut down the SSL connection and clean up related memory.
   this function can be called multiple times on the same connection including
   if the SSL connection failed (eg connection made but failed handshake). */
நிலையான CURLcode schannel_shutdown(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data,
                                  bool send_shutdown, bool *done)
{
  /* See https://learn.microsoft.com/windows/win32/secauthn/shutting-down-an-schannel-connection
   * Shutting Down an Schannel Connection
   */
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு schannel_ssl_backend_data *backend =
    (கட்டமைப்பு schannel_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;

  எனில்(cf->shutdown) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  DEBUGASSERT(data);
  DEBUGASSERT(backend);

  /* Not supported in schannel */
  (வெறுமை)send_shutdown;

  *done = FALSE;
  எனில்(backend->ctxt) {
    infof(data, "schannel: shutting down SSL/TLS connection with %s port %d",
          connssl->peer.origin->hostname, connssl->peer.origin->port);
  }

  எனில்(!backend->ctxt || cf->shutdown) {
    *done = TRUE;
    செல் out;
  }

  எனில்(backend->cred && backend->ctxt && !backend->sent_shutdown &&
     !backend->shutdown_generated) {
    SecBufferDesc BuffDesc;
    SecBuffer Buffer;
    SECURITY_STATUS sspi_status;
    SecBuffer outbuf;
    SecBufferDesc outbuf_desc;
    DWORD dwshut = SCHANNEL_SHUTDOWN;

    InitSecBuffer(&Buffer, SECBUFFER_TOKEN, &dwshut, அளவு(dwshut));
    InitSecBufferDesc(&BuffDesc, &Buffer, 1);

    sspi_status = Curl_pSecFn->ApplyControlToken(&backend->ctxt->ctxt_handle,
                                                 &BuffDesc);

    எனில்(sspi_status != SEC_E_OK) {
      எழுத்து buffer[STRERROR_LEN];
      failf(data, "schannel: ApplyControlToken failure: %s",
            Curl_sspi_strerror(sspi_status, buffer, அளவு(buffer)));
      result = CURLE_SEND_ERROR;
      செல் out;
    }

    /* setup output buffer */
    InitSecBuffer(&outbuf, SECBUFFER_EMPTY, வெற்று, 0);
    InitSecBufferDesc(&outbuf_desc, &outbuf, 1);

    sspi_status = Curl_pSecFn->InitializeSecurityContext(
      &backend->cred->cred_handle,
      &backend->ctxt->ctxt_handle,
      backend->cred->sni_hostname,
      backend->req_flags,
      0,
      0,
      வெற்று,
      0,
      &backend->ctxt->ctxt_handle,
      &outbuf_desc,
      &backend->ret_flags, வெற்று);

    எனில்((sspi_status == SEC_E_OK) || (sspi_status == SEC_I_CONTEXT_EXPIRED)) {
      backend->shutdown_token = outbuf;
      backend->shutdown_generated = TRUE;
    }
    இல்லையெனில் {
      எழுத்து buffer[STRERROR_LEN];
      எனில்(outbuf.pvBuffer)
        Curl_pSecFn->FreeContextBuffer(outbuf.pvBuffer);
      failf(data, "schannel: generating close msg failed: %s",
            Curl_sspi_strerror(sspi_status, buffer, அளவு(buffer)));
      result = CURLE_SEND_ERROR;
      செல் out;
    }
  }

  எனில்(backend->shutdown_generated && !backend->sent_shutdown) {
    size_t written = 0;
    size_t remaining;

    DEBUGASSERT(backend->shutdown_offset <= backend->shutdown_token.cbBuffer);
    remaining = backend->shutdown_token.cbBuffer - backend->shutdown_offset;

    எனில்(remaining) {
      result = Curl_conn_cf_send(cf->next, data,
                                 ((uint8_t *)backend->shutdown_token.pvBuffer +
                                  backend->shutdown_offset),
                                 remaining, FALSE, &written);
      எனில்(!result)
        backend->shutdown_offset += written;
      எனில்(result == CURLE_AGAIN || (!result && written < remaining)) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        result = CURLE_OK;
        செல் out;
      }
    }
    இல்லையெனில் /* no shutdown message to send? */
      result = CURLE_OK;
    எனில்(result) {
      எனில்(backend->recv_connection_closed) {
        /* The server already closed, so sending close_notify is optional. */
        *done = TRUE;
        result = CURLE_OK;
      }
      இல்லையெனில் {
        failf(data, "schannel: error sending close msg: %d", (முழு)result);
        result = CURLE_SEND_ERROR;
      }
      செல் out;
    }
    எனில்(backend->shutdown_token.pvBuffer)
      Curl_pSecFn->FreeContextBuffer(backend->shutdown_token.pvBuffer);
    memset(&backend->shutdown_token, 0, அளவு(backend->shutdown_token));
    backend->shutdown_offset = 0;
    backend->sent_shutdown = TRUE;
    *done = TRUE;
  }

  /* If the connection seems open and we have not seen the close notify
   * from the server yet, try to receive it. */
  எனில்(backend->cred && backend->ctxt &&
     !backend->recv_sspi_close_notify && !backend->recv_connection_closed) {
    எழுத்து buffer[1024];
    size_t nread;

    result = schannel_recv(cf, data, buffer, அளவு(buffer), &nread);
    எனில்(result == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
    }
    இல்லையெனில் எனில்(result) {
      CURL_TRC_CF(data, cf, "SSL shutdown, error %d", (முழு)result);
      result = CURLE_RECV_ERROR;
    }
    இல்லையெனில் எனில்(nread == 0) {
      /* We got the close notify alert and are done. */
      backend->recv_connection_closed = TRUE;
      *done = TRUE;
    }
    இல்லையெனில் {
      /* still data coming in? */
    }
  }

out:
  cf->shutdown = (result || *done);
  திரும்பு result;
}

நிலையான வெறுமை schannel_close(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு schannel_ssl_backend_data *backend =
    (கட்டமைப்பு schannel_ssl_backend_data *)connssl->backend;

  DEBUGASSERT(data);
  DEBUGASSERT(backend);

  எனில்(backend->shutdown_token.pvBuffer)
    Curl_pSecFn->FreeContextBuffer(backend->shutdown_token.pvBuffer);
  memset(&backend->shutdown_token, 0, அளவு(backend->shutdown_token));
  backend->shutdown_offset = 0;

  /* free SSPI Schannel API security context handle */
  எனில்(backend->ctxt) {
    எனில்(cf->conn->sslContext == &backend->ctxt->ctxt_handle)
      cf->conn->sslContext = வெற்று;

    DEBUGF(infof(data, "schannel: clear security context handle"));
    Curl_pSecFn->DeleteSecurityContext(&backend->ctxt->ctxt_handle);
    curlx_safefree(backend->ctxt);
  }

  /* free SSPI Schannel API credential handle */
  எனில்(backend->cred) {
    Curl_ssl_scache_lock(data);
    schannel_session_free(backend->cred);
    Curl_ssl_scache_unlock(data);
    backend->cred = வெற்று;
  }

  /* free the buffer used to encrypt outgoing data */
  curlx_safefree(backend->send_buffer);
  backend->send_buffer_len = 0;

  /* free internal buffer for received encrypted data */
  எனில்(backend->encdata.buffer) {
    curlx_safefree(backend->encdata.buffer);
    backend->encdata.length = 0;
    backend->encdata.offset = 0;
    backend->encdata_is_incomplete = FALSE;
  }

  /* free internal buffer for received decrypted data */
  எனில்(backend->decdata.buffer) {
    curlx_safefree(backend->decdata.buffer);
    backend->decdata.length = 0;
    backend->decdata.offset = 0;
  }
}

நிலையான முழு schannel_init(வெறுமை)
{
  வகைவரை மாறிலி எழுத்து *(APIENTRY *WINE_GET_VERSION_FN)(வெறுமை);
#எனில் defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic தள்ளு
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
  WINE_GET_VERSION_FN p_wine_get_version =
    CURLX_FUNCTION_CAST(WINE_GET_VERSION_FN,
      GetProcAddress(GetModuleHandle(TEXT("ntdll")), "wine_get_version"));
#எனில் defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic எடு
#endif
  எனில்(p_wine_get_version) {  /* WINE detected */
    curl_off_t ver = 0;
    மாறிலி எழுத்து *wine_version = p_wine_get_version();  /* e.g. "6.0.2" */
    /* Assume ALPN support with WINE 6.0 or greater */
    எனில்(wine_version)
      curlx_str_எண்(&wine_version, &ver, 20);
    s_win_has_alpn = (ver >= 6);
  }
  இல்லையெனில் {
    /* ALPN is supported on Windows 8.1 / Server 2012 R2 or newer. */
    s_win_has_alpn = curlx_verify_windows_version(6, 3, 0, PLATFORM_WINNT,
                                                  VERSION_GREATER_THAN_EQUAL);
  }

  திரும்பு Curl_sspi_global_init() == CURLE_OK ? 1 : 0;
}

நிலையான வெறுமை schannel_cleanup(வெறுமை)
{
  Curl_sspi_global_cleanup();
}

நிலையான size_t schannel_version(எழுத்து *buffer, size_t size)
{
  திரும்பு curl_msnprintf(buffer, size, "Schannel");
}

நிலையான CURLcode schannel_random(கட்டமைப்பு Curl_easy *data,
                                unsigned எழுத்து *entropy, size_t length)
{
  (வெறுமை)data;

  திரும்பு curlx_win32_random(entropy, length);
}

நிலையான CURLcode schannel_checksum(மாறிலி unsigned எழுத்து *உள்ளீடு,
                                  size_t inputlen,
                                  unsigned எழுத்து *checksum,
                                  size_t checksumlen,
                                  DWORD provType,
                                  மாறிலி unsigned முழு algId)
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

  எனில்(!CryptAcquireContext(&hProv, வெற்று, வெற்று, provType,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    செல் out;

  எனில்(!CryptCreateHash(hProv, algId, 0, 0, &hHash))
    செல் out;

  result = CURLE_BAD_FUNCTION_ARGUMENT;

  off = 0;
  போது(off < inputlen) {
    DWORD chunk = (DWORD)CURLMIN(inputlen - off, 0xffffffffUL);
    எனில்(!CryptHashData(hHash, உள்ளீடு + off, chunk, 0))
      செல் out;
    off += chunk;
  }

  /* get hash size */
  cbHashSize = 0;
  dwHashSizeLen = (DWORD)அளவு(cbHashSize);
  எனில்(!CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE *)&cbHashSize,
                        &dwHashSizeLen, 0))
    செல் out;

  /* check if hash fits into the return buffer */
  எனில்(checksumlen < cbHashSize)
    செல் out;

  dwChecksumLen = (DWORD)checksumlen;
  எனில்(CryptGetHashParam(hHash, HP_HASHVAL, checksum, &dwChecksumLen, 0) &&
     dwChecksumLen == cbHashSize)
    result = CURLE_OK;

out:
  எனில்(hHash)
    CryptDestroyHash(hHash);

  எனில்(hProv)
    CryptReleaseContext(hProv, 0);

  திரும்பு result;
}

நிலையான CURLcode schannel_sha256sum(மாறிலி unsigned எழுத்து *உள்ளீடு,
                                   size_t inputlen,
                                   unsigned எழுத்து *sha256sum,
                                   size_t sha256len)
{
  திரும்பு schannel_checksum(உள்ளீடு, inputlen, sha256sum, sha256len,
                           PROV_RSA_AES, CALG_SHA_256);
}

நிலையான வெறுமை *schannel_get_internals(கட்டமைப்பு ssl_connect_data *connssl,
                                    CURLINFO info)
{
  கட்டமைப்பு schannel_ssl_backend_data *backend =
    (கட்டமைப்பு schannel_ssl_backend_data *)connssl->backend;
  (வெறுமை)info;
  DEBUGASSERT(backend);
  திரும்பு &backend->ctxt->ctxt_handle;
}

HCERTSTORE Curl_schannel_get_cached_cert_store(கட்டமைப்பு Curl_cfilter *cf,
                                               கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  கட்டமைப்பு Curl_multi *multi = data->multi;
  மாறிலி கட்டமைப்பு curl_blob *ca_info_blob = conn_config->ca_info_blob;
  கட்டமைப்பு schannel_cert_share *share;
  timediff_t timeout_ms;
  unsigned எழுத்து info_blob_digest[CURL_SHA256_DIGEST_LENGTH];

  DEBUGASSERT(multi);

  எனில்(!multi) {
    திரும்பு வெற்று;
  }

  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_SCHANNEL_CERT_SHARE_சாவி,
                         CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_சாவி));
  எனில்(!share || !share->cert_store) {
    திரும்பு வெற்று;
  }

  /* zero ca_cache_timeout completely disables caching */
  எனில்(!data->set.ssl_ca_cache_timeout) {
    திரும்பு வெற்று;
  }

  /* check for cache timeout by using the cached_x509_store_expired timediff
     calculation pattern from openssl.c.
     negative timeout means retain forever. */
  timeout_ms = data->set.ssl_ca_cache_timeout * (timediff_t)1000;
  எனில்(timeout_ms >= 0) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(data), &share->time);
    எனில்(elapsed_ms >= timeout_ms) {
      திரும்பு வெற்று;
    }
  }

  எனில்(ca_info_blob) {
    எனில்(share->CAinfo_blob_size != ca_info_blob->len) {
      திரும்பு வெற்று;
    }
    எனில்(schannel_sha256sum((மாறிலி unsigned எழுத்து *)ca_info_blob->data,
                          ca_info_blob->len,
                          info_blob_digest,
                          CURL_SHA256_DIGEST_LENGTH))
      திரும்பு வெற்று;
    எனில்(memcmp(share->CAinfo_blob_digest, info_blob_digest,
              CURL_SHA256_DIGEST_LENGTH)) {
      திரும்பு வெற்று;
    }
  }
  இல்லையெனில் {
    எனில்(!conn_config->CAfile || !share->CAfile ||
       strcmp(share->CAfile, conn_config->CAfile)) {
      திரும்பு வெற்று;
    }
  }

  திரும்பு share->cert_store;
}

நிலையான வெறுமை schannel_cert_share_free(மாறிலி வெறுமை *சாவி, size_t சாவி_len, வெறுமை *p)
{
  கட்டமைப்பு schannel_cert_share *share = p;
  DEBUGASSERT(சாவி_len == CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_சாவி));
  DEBUGASSERT(!memcmp(MPROTO_SCHANNEL_CERT_SHARE_சாவி, சாவி, சாவி_len));
  (வெறுமை)சாவி;
  (வெறுமை)சாவி_len;
  எனில்(share->cert_store) {
    CertCloseStore(share->cert_store, 0);
  }
  curlx_free(share->CAfile);
  curlx_free(share);
}

bool Curl_schannel_set_cached_cert_store(கட்டமைப்பு Curl_cfilter *cf,
                                         கட்டமைப்பு Curl_easy *data,
                                         HCERTSTORE cert_store)
{
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  கட்டமைப்பு Curl_multi *multi = data->multi;
  மாறிலி கட்டமைப்பு curl_blob *ca_info_blob = conn_config->ca_info_blob;
  கட்டமைப்பு schannel_cert_share *share;
  unsigned எழுத்து digest[CURL_SHA256_DIGEST_LENGTH];
  எழுத்து *CAfile = வெற்று;

  DEBUGASSERT(multi);

  எனில்(!multi) {
    திரும்பு FALSE;
  }

  எனில்(ca_info_blob) {
    எனில்(schannel_sha256sum((மாறிலி unsigned எழுத்து *)ca_info_blob->data,
                          ca_info_blob->len, digest, அளவு(digest))) {
      திரும்பு FALSE;
    }
  }
  இல்லையெனில் எனில்(conn_config->CAfile) {
    CAfile = curlx_strdup(conn_config->CAfile);
    எனில்(!CAfile) {
      திரும்பு FALSE;
    }
  }

  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_SCHANNEL_CERT_SHARE_சாவி,
                         CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_சாவி));
  எனில்(!share) {
    share = curlx_calloc(1, அளவு(*share));
    எனில்(!share) {
      curlx_free(CAfile);
      திரும்பு FALSE;
    }
    எனில்(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_SCHANNEL_CERT_SHARE_சாவி,
                       CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_சாவி),
                       share, schannel_cert_share_free)) {
      curlx_free(share);
      curlx_free(CAfile);
      திரும்பு FALSE;
    }
  }

  /* free old cache data */
  எனில்(share->cert_store) {
    CertCloseStore(share->cert_store, 0);
  }
  curlx_free(share->CAfile);

  எனில்(ca_info_blob) {
    memcpy(share->CAinfo_blob_digest, digest, அளவு(digest));
    share->CAinfo_blob_size = ca_info_blob->len;
  }
  இல்லையெனில்
    share->CAinfo_blob_size = 0;

  share->time = curlx_now();
  share->cert_store = cert_store;
  share->CAfile = CAfile;
  திரும்பு TRUE;
}

மாறிலி கட்டமைப்பு Curl_ssl Curl_ssl_schannel = {
  { CURLSSLBACKEND_SCHANNEL, "schannel" }, /* info */

  SSLSUPP_CERTINFO |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_CA_CACHE |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST,

  அளவு(கட்டமைப்பு schannel_ssl_backend_data),

  schannel_init,                     /* init */
  schannel_cleanup,                  /* cleanup */
  schannel_version,                  /* version */
  schannel_shutdown,                 /* shutdown */
  schannel_data_pending,             /* data_pending */
  schannel_random,                   /* random */
  வெற்று,                              /* cert_status_request */
  schannel_connect,                  /* connect */
  Curl_ssl_adjust_pollset,           /* adjust_pollset */
  schannel_get_internals,            /* get_internals */
  schannel_close,                    /* close_one */
  வெற்று,                              /* close_all */
  வெற்று,                              /* engines_list */
  schannel_sha256sum,                /* sha256sum */
  schannel_recv,                     /* recv decrypted data */
  schannel_send,                     /* send data to encrypt */
  வெற்று,                              /* get_channel_binding */
};

#endif /* USE_SCHANNEL */
