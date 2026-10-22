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
#include "curl_setup.h"

#ifdef USE_SCHANNEL

#ifndef USE_WINDOWS_SSPI
#error "cannot compile Schannel support without SSPI."
#endif

#include "vtls/schannel.h"
#include "vtls/schannel_int.h"
#include "vtls/vtls.h"
#include "vtls/vtls_int.h"
#include "vtls/vtls_scache.h"
#include "curl_trc.h"
#include "connect.h" /* for the connect timeout */
#include "curlx/strdup.h"
#include "strerror.h"
#include "select.h" /* for the socket readiness */
#include "curlx/win32-fopen.h"
#include "curlx/multibyte.h"
#include "vtls/x509asn1.h"
#include "curlx/version_win32.h"
#include "curlx/winapi.h"
#include "curlx/strparse.h"
#include "progress.h"
#include "curl_sha256.h"

/* Some verbose debug messages are wrapped by SCH_DEV() instead of DEBUGF()
 * and only shown if CURL_SCHANNEL_DEV_DEBUG was defined at build time. These
 * messages are extra verbose and intended for curl developers debugging
 * Schannel recv decryption and renegotiation.
 */
#ifdef CURL_SCHANNEL_DEV_DEBUG
#define SCH_DEV(x) x
#define SCH_DEV_SHOWBOOL(x)                                     \
  do {                                                          \
    infof(data, "schannel: " #x " %s", (x) ? "TRUE" : "FALSE"); \
  } སྐབས(0)
#དེ་མིན
#define SCH_DEV(x) do {} སྐབས(0)
#define SCH_DEV_SHOWBOOL(x) do {} སྐབས(0)
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
#ifndef PKCS12_NO_PERSIST_KEY
#define PKCS12_NO_PERSIST_KEY 0x00008000
#endif

/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#ifndef CERT_FIND_HAS_PRIVATE_KEY
#define CERT_FIND_HAS_PRIVATE_KEY (21 << CERT_COMPARE_SHIFT)
#endif

/* key to use at `multi->proto_hash` */
#define MPROTO_SCHANNEL_CERT_SHARE_KEY   "tls:schannel:cert:share"

static bool s_win_has_alpn;

/* Offered by mingw-w64 v9+, MS SDK 8.1/VS2013+ */
#ifndef SECBUFFER_APPLICATION_PROTOCOLS
#define SECBUFFER_APPLICATION_PROTOCOLS 18
#define SECPKG_ATTR_APPLICATION_PROTOCOL 35

typedef ga_nanaa {
  SecApplicationProtocolNegotiationExt_None,
  SecApplicationProtocolNegotiationExt_NPN,
  SecApplicationProtocolNegotiationExt_ALPN
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT;

typedef ga_nanaa {
  SecApplicationProtocolNegotiationStatus_None,
  SecApplicationProtocolNegotiationStatus_Success,
  SecApplicationProtocolNegotiationStatus_SelectedClientOnly
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_STATUS;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
typedef sa_nracanaa {
  SEC_APPLICATION_PROTOCOL_NEGOTIATION_STATUS ProtoNegoStatus;
  SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT ProtoNegoExt;
  unsigned var_na ProtocolIdSize;
  unsigned var_na ProtocolId[0xff];
} SecPkgContext_ApplicationProtocol;
#endif

static void InitSecBuffer(SecBuffer *buffer, unsigned long BufType,
                          void *BufDataPtr, unsigned long BufByteSize)
{
  buffer->cbBuffer = BufByteSize;
  buffer->BufferType = BufType;
  buffer->pvBuffer = BufDataPtr;
}

static void InitSecBufferDesc(SecBufferDesc *desc, SecBuffer *BufArr,
                              unsigned long NumArrElem)
{
  desc->ulVersion = SECBUFFER_VERSION;
  desc->pBuffers = BufArr;
  desc->cBuffers = NumArrElem;
}

static CURLcode schannel_set_ssl_version_min_max(DWORD *enabled_protocols,
                                                 sa_nracanaa Curl_cfilter *cf,
                                                 sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  long ssl_version = conn_config->version;
  long ssl_version_max = (long)conn_config->version_max;
  long i = ssl_version;

  switch(ssl_version_max) {
  pak_sa CURL_SSLVERSION_MAX_NONE:
  pak_sa CURL_SSLVERSION_MAX_DEFAULT:

    /* Windows Server 2022 and newer (including Windows 11) support TLS 1.3
       built-in. Previous builds of Windows 10 had broken TLS 1.3
       implementations that could be enabled via registry. */
    གལ་ཏེ(curlx_verify_windows_version(10, 0, 20348, PLATFORM_WINNT,
                                    VERSION_GREATER_THAN_EQUAL)) {
      ssl_version_max = CURL_SSLVERSION_MAX_TLSv1_3;
    }
    དེ་མིན /* Windows 10 or older */
      ssl_version_max = CURL_SSLVERSION_MAX_TLSv1_2;

    bha_nga;
  }

  ཆེད་དུ(; i <= (ssl_version_max >> 16); ++i) {
    switch(i) {
    pak_sa CURL_SSLVERSION_TLSv1_0:
      *enabled_protocols |= SP_PROT_TLS1_0_CLIENT;
      bha_nga;
    pak_sa CURL_SSLVERSION_TLSv1_1:
      *enabled_protocols |= SP_PROT_TLS1_1_CLIENT;
      bha_nga;
    pak_sa CURL_SSLVERSION_TLSv1_2:
      *enabled_protocols |= SP_PROT_TLS1_2_CLIENT;
      bha_nga;
    pak_sa CURL_SSLVERSION_TLSv1_3:

      /* Windows Server 2022 or newer */
      གལ་ཏེ(curlx_verify_windows_version(10, 0, 20348, PLATFORM_WINNT,
                                      VERSION_GREATER_THAN_EQUAL)) {
        *enabled_protocols |= SP_PROT_TLS1_3_CLIENT;
        bha_nga;
      }
      དེ་མིན { /* Windows 10 or older */
        failf(data, "schannel: TLS 1.3 not supported on Windows 10 or older");
        ཕྱིར CURLE_SSL_CONNECT_ERROR;
      }
    }
  }
  ཕྱིར CURLE_OK;
}

#define CIPHEROPTION(x) { #x, x }

sa_nracanaa algo {
  const var_na *name;
  ཧྲིལ id;
};

static const sa_nracanaa algo algs[] = {
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
  CIPHEROPTION(CALG_SCHANNEL_MAC_KEY),
  CIPHEROPTION(CALG_SCHANNEL_ENC_KEY),
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
  { NULL, 0 },
};

static ཧྲིལ get_alg_id_by_name(const var_na *name)
{
  const var_na *nameEnd = strchr(name, ':');
  size_t n = nameEnd ? (size_t)(nameEnd - name) : strlen(name);
  ཧྲིལ i;

  ཆེད་དུ(i = 0; algs[i].name; i++) {
    གལ་ཏེ((n == strlen(algs[i].name) && !strncmp(algs[i].name, name, n)))
      ཕྱིར algs[i].id;
  }
  ཕྱིར 0; /* not found */
}

#define NUM_CIPHERS 47 /* There are a maximum of 47 options listed above */

static CURLcode set_ssl_ciphers(SCHANNEL_CRED *schannel_cred, var_na *ciphers,
                                ALG_ID *algIds)
{
  const var_na *startCur = ciphers;
  ཧྲིལ algCount = 0;
  སྐབས(startCur && *startCur && (algCount < NUM_CIPHERS)) {
    curl_off_t alg;
    གལ་ཏེ(curlx_str_number(&startCur, &alg, INT_MAX) || !alg)
      alg = get_alg_id_by_name(startCur);

    གལ་ཏེ(alg)
      algIds[algCount++] = (ALG_ID)alg;
    དེ་མིན གལ་ཏེ(!strncmp(startCur, "USE_STRONG_CRYPTO",
                     CURL_CSTRLEN("USE_STRONG_CRYPTO")) ||
            !strncmp(startCur, "SCH_USE_STRONG_CRYPTO",
                     CURL_CSTRLEN("SCH_USE_STRONG_CRYPTO")))
      schannel_cred->dwFlags |= SCH_USE_STRONG_CRYPTO;
    དེ་མིན
      ཕྱིར CURLE_SSL_CIPHER;
    startCur = strchr(startCur, ':');
    གལ་ཏེ(startCur)
      startCur++;
  }
  schannel_cred->palgSupportedAlgs = algIds;
  schannel_cred->cSupportedAlgs = (DWORD)algCount;
  ཕྱིར CURLE_OK;
}

/* Function allocates memory for store_path only if CURLE_OK is returned */
static CURLcode get_cert_location(TCHAR *path, DWORD *store_name,
                                  TCHAR **store_path, TCHAR **thumbprint)
{
  TCHAR *sep;
  TCHAR *store_path_start;
  size_t store_name_len;

  sep = _tcschr(path, _TEXT('\\'));
  གལ་ཏེ(!sep)
    ཕྱིར CURLE_SSL_CERTPROBLEM;

  store_name_len = sep - path;

  གལ་ཏེ(_tcsncmp(path, _TEXT("CurrentUser"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_USER;
  དེ་མིན གལ་ཏེ(_tcsncmp(path, _TEXT("LocalMachine"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE;
  དེ་མིན གལ་ཏེ(_tcsncmp(path, _TEXT("CurrentService"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_SERVICE;
  དེ་མིན གལ་ཏེ(_tcsncmp(path, _TEXT("Services"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_SERVICES;
  དེ་མིན གལ་ཏེ(_tcsncmp(path, _TEXT("Users"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_USERS;
  དེ་མིན གལ་ཏེ(_tcsncmp(path, _TEXT("CurrentUserGroupPolicy"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_USER_GROUP_POLICY;
  དེ་མིན གལ་ཏེ(_tcsncmp(path, _TEXT("LocalMachineGroupPolicy"), store_name_len) ==
          0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE_GROUP_POLICY;
  དེ་མིན གལ་ཏེ(_tcsncmp(path, _TEXT("LocalMachineEnterprise"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE_ENTERPRISE;
  དེ་མིན
    ཕྱིར CURLE_SSL_CERTPROBLEM;

  store_path_start = sep + 1;

  sep = _tcschr(store_path_start, _TEXT('\\'));
  གལ་ཏེ(!sep)
    ཕྱིར CURLE_SSL_CERTPROBLEM;

  *thumbprint = sep + 1;
  གལ་ཏེ(_tcslen(*thumbprint) != CERT_THUMBPRINT_STR_LEN)
    ཕྱིར CURLE_SSL_CERTPROBLEM;

  *sep = _TEXT('\0');
  *store_path = curlx_tcsdup(store_path_start);
  *sep = _TEXT('\\');
  གལ་ཏེ(!*store_path)
    ཕྱིར CURLE_OUT_OF_MEMORY;

  ཕྱིར CURLE_OK;
}

static CURLcode get_client_cert(sa_nracanaa Curl_cfilter *cf,
                                sa_nracanaa Curl_easy *data,
                                HCERTSTORE *out_cert_store,
                                PCCERT_CONTEXT *out_cert_context)
{
  sa_nracanaa ssl_filter_config *sslc = Curl_ssl_cf_get_filter_config(cf);
  PCCERT_CONTEXT client_cert = NULL;
  HCERTSTORE client_cert_store = NULL;
  CURLcode result = CURLE_OK;

  གལ་ཏེ(sslc->clientcert || sslc->cert_blob) {
    DWORD cert_store_name = 0;
    TCHAR *cert_store_path = NULL;
    TCHAR *cert_thumbprint_str = NULL;
    TCHAR cert_thumbprint_buf[CERT_THUMBPRINT_STR_LEN + 1];
    CRYPT_HASH_BLOB cert_thumbprint;
    BYTE cert_thumbprint_data[CERT_THUMBPRINT_DATA_LEN];
    HCERTSTORE cert_store = NULL;
    FILE *fInCert = NULL;
    void *certdata = NULL;
    size_t certsize = 0;
    bool blob = !!sslc->cert_blob;

    གལ་ཏེ(blob) {
      certdata = sslc->cert_blob->data;
      certsize = sslc->cert_blob->len;
    }
    དེ་མིན {
      TCHAR *cert_path =
        curlx_convert_UTF8_to_tchar(sslc->clientcert);
      གལ་ཏེ(!cert_path)
        ཕྱིར CURLE_OUT_OF_MEMORY;

      result = get_cert_location(cert_path, &cert_store_name,
                                 &cert_store_path, &cert_thumbprint_str);

      /* 'cert_thumbprint_str' points in to the allocated 'cert_path', copy
         the data. The string is verified to be CERT_THUMBPRINT_STR_LEN bytes
         long within the get_cert_location() function. */
      གལ་ཏེ(!result && cert_thumbprint_str) {
        memcpy(cert_thumbprint_buf, cert_thumbprint_str,
               parimaa_na(cert_thumbprint_buf));
        cert_thumbprint_str = cert_thumbprint_buf;
      }

      curlx_free(cert_path);
      གལ་ཏེ(result && (sslc->clientcert[0] != '\0'))
        fInCert = curlx_fopen(sslc->clientcert, "rb");

      གལ་ཏེ(result && !fInCert) {
        failf(data, "schannel: Failed to get certificate location"
              " or file for %s",
              sslc->clientcert);
        ཕྱིར result;
      }
    }

    གལ་ཏེ((fInCert || blob) && sslc->cert_type &&
       !curl_strequal(sslc->cert_type, "P12")) {
      failf(data, "schannel: certificate format compatibility error "
            "for %s",
            blob ? "(memory blob)" : sslc->clientcert);
      curlx_free(cert_store_path);
      གལ་ཏེ(fInCert)
        curlx_fclose(fInCert);
      ཕྱིར CURLE_SSL_CERTPROBLEM;
    }

    གལ་ཏེ(fInCert || blob) {
      /* Reading a .p12 or .pfx file, like the example at bottom of
         https://learn.microsoft.com/archive/msdn-technet-forums/3e7bc95f-b21a-4bcd-bd2c-7f996718cae5
       */
      CRYPT_DATA_BLOB datablob;
      WCHAR *pszPassword;
      size_t pwd_len = 0;
      ཧྲིལ cert_find_flags;
      const var_na *cert_showfilename_error = blob ?
        "(memory blob)" : sslc->clientcert;
      curlx_free(cert_store_path);
      གལ་ཏེ(fInCert) {
        long cert_tell = 0;
        bool continue_reading = fseek(fInCert, 0, SEEK_END) == 0;
        གལ་ཏེ(continue_reading) {
          cert_tell = ftell(fInCert);
          གལ་ཏེ(cert_tell < 0)
            continue_reading = FALSE;
          དེ་མིན
            certsize = (size_t)cert_tell;
        }
        གལ་ཏེ(continue_reading)
          continue_reading = fseek(fInCert, 0, SEEK_SET) == 0;
        གལ་ཏེ(continue_reading && (certsize < CURL_MAX_INPUT_LENGTH))
          certdata = curlx_malloc(certsize + 1);
        གལ་ཏེ(!certdata || ((ཧྲིལ)fread(certdata, certsize, 1, fInCert) != 1))
          continue_reading = FALSE;
        curlx_fclose(fInCert);
        གལ་ཏེ(!continue_reading) {
          failf(data, "schannel: Failed to read cert file %s",
                sslc->clientcert);
          curlx_free(certdata);
          ཕྱིར CURLE_SSL_CERTPROBLEM;
        }
      }

      /* Convert key-pair data to the in-memory certificate store */
      datablob.pbData = (BYTE *)certdata;
      datablob.cbData = (DWORD)certsize;

      གལ་ཏེ(sslc->key_passwd)
        pwd_len = strlen(sslc->key_passwd);
      pszPassword = curlx_malloc(parimaa_na(WCHAR) * (pwd_len + 1));
      གལ་ཏེ(pszPassword) {
        ཧྲིལ str_w_len = 0;
        གལ་ཏེ(pwd_len > 0)
          str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                          sslc->key_passwd, (ཧྲིལ)pwd_len,
                                          pszPassword, (ཧྲིལ)(pwd_len + 1));

        གལ་ཏེ((str_w_len >= 0) && (str_w_len <= (ཧྲིལ)pwd_len))
          pszPassword[str_w_len] = 0;
        དེ་མིན
          pszPassword[0] = 0;

        cert_store = PFXImportCertStore(&datablob, pszPassword,
                                        PKCS12_NO_PERSIST_KEY);
        curlx_memzero(pszPassword, parimaa_na(WCHAR) * (pwd_len + 1));
        curlx_free(pszPassword);
      }
      གལ་ཏེ(!blob)
        curlx_free(certdata);
      གལ་ཏེ(!cert_store) {
        DWORD errorcode = GetLastError();
        གལ་ཏེ(errorcode == ERROR_INVALID_PASSWORD)
          failf(data, "schannel: Failed to import cert file %s, "
                "password is bad",
                cert_showfilename_error);
        དེ་མིན
          failf(data, "schannel: Failed to import cert file %s, "
                "last error is 0x%08lx",
                cert_showfilename_error, errorcode);
        ཕྱིར CURLE_SSL_CERTPROBLEM;
      }

      /* CERT_FIND_HAS_PRIVATE_KEY is only available in Windows 8 / Server
         2012, (NT 6.2). For older versions we use CURL_FIND_ANY. */
      གལ་ཏེ(curlx_verify_windows_version(6, 2, 0, PLATFORM_WINNT,
                                      VERSION_GREATER_THAN_EQUAL))
        cert_find_flags = CERT_FIND_HAS_PRIVATE_KEY;
      དེ་མིན
        cert_find_flags = CERT_FIND_ANY;

      client_cert =
        CertFindCertificateInStore(cert_store,
                                   X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                   cert_find_flags, NULL, NULL);

      གལ་ཏེ(!client_cert) {
        failf(data, "schannel: Failed to get certificate from file %s"
              ", last error is 0x%08lx",
              cert_showfilename_error, GetLastError());
        CertCloseStore(cert_store, 0);
        ཕྱིར CURLE_SSL_CERTPROBLEM;
      }
    }
    དེ་མིན {
      cert_store =
        CertOpenStore(
#ifdef UNICODE
                      CERT_STORE_PROV_SYSTEM_W,
#དེ་མིན
                      CERT_STORE_PROV_SYSTEM_A,
#endif
                      0,
                      (HCRYPTPROV)NULL,
                      CERT_STORE_OPEN_EXISTING_FLAG | cert_store_name,
                      cert_store_path);
      གལ་ཏེ(!cert_store) {
        var_na *path_utf8 =
          curlx_convert_tchar_to_UTF8(cert_store_path);
        failf(data, "schannel: Failed to open cert store %lx %s, "
              "last error is 0x%08lx",
              cert_store_name, (path_utf8 ? path_utf8 : "(unknown)"),
              GetLastError());
        curlx_free(cert_store_path);
        curlx_free(path_utf8);
        ཕྱིར CURLE_SSL_CERTPROBLEM;
      }
      curlx_free(cert_store_path);

      cert_thumbprint.pbData = cert_thumbprint_data;
      cert_thumbprint.cbData = CERT_THUMBPRINT_DATA_LEN;

      གལ་ཏེ(!CryptStringToBinary(cert_thumbprint_str,
                              CERT_THUMBPRINT_STR_LEN,
                              CRYPT_STRING_HEX,
                              cert_thumbprint_data,
                              &cert_thumbprint.cbData,
                              NULL, NULL)) {
        CertCloseStore(cert_store, 0);
        ཕྱིར CURLE_SSL_CERTPROBLEM;
      }

      client_cert =
        CertFindCertificateInStore(cert_store,
                                   X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                   CERT_FIND_HASH, &cert_thumbprint, NULL);
      གལ་ཏེ(!client_cert) {
        /* CRYPT_E_NOT_FOUND / E_INVALIDARG */
        CertCloseStore(cert_store, 0);
        failf(data, "schannel: client cert not found in cert store");
        ཕྱིར CURLE_SSL_CERTPROBLEM;
      }
    }
    client_cert_store = cert_store;
  }

  *out_cert_store = client_cert_store;
  *out_cert_context = client_cert;

  ཕྱིར CURLE_OK;
}

static CURLcode acquire_sspi_handle(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data,
                                    sa_nracanaa schannel_ssl_backend_data *backend,
                                    PCCERT_CONTEXT client_cert,
                                    DWORD flags,
                                    DWORD enabled_protocols)
{
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CURLcode result;

  /* We support TLS 1.3 starting in Windows Server 2022 or later
   * (OS build 20348) */
  གལ་ཏེ(!conn_config->cipher_list &&
     curlx_verify_windows_version(10, 0, 20348, PLATFORM_WINNT,
                                  VERSION_GREATER_THAN_EQUAL)) {

    SCH_CREDENTIALS credentials = { 0 };
    TLS_PARAMETERS tls_parameters = { 0 };
    CRYPTO_SETTINGS crypto_settings[1];
    PCCERT_CONTEXT client_certs[1];

    གལ་ཏེ(client_cert)
      client_certs[0] = client_cert;
    དེ་མིན
      client_certs[0] = NULL;

    memset(crypto_settings, 0, parimaa_na(crypto_settings));

    tls_parameters.pDisabledCrypto = crypto_settings;

    /* The number of blocked suites */
    tls_parameters.cDisabledCrypto = (DWORD)0;
    credentials.pTlsParameters = &tls_parameters;
    credentials.cTlsParameters = 1;

    credentials.dwVersion = SCH_CREDENTIALS_VERSION;
    credentials.dwFlags = flags | SCH_USE_STRONG_CRYPTO;

    credentials.pTlsParameters->grbitDisabledProtocols = ~enabled_protocols;

    གལ་ཏེ(client_certs[0]) {
      credentials.cCreds = 1;
      credentials.paCred = client_certs;
    }

    sspi_status =
      Curl_pSecFn->AcquireCredentialsHandle(NULL, CURL_UNCONST(UNISP_NAME),
                                            SECPKG_CRED_OUTBOUND, NULL,
                                            &credentials, NULL, NULL,
                                            &backend->cred->cred_handle, NULL);
  }
  དེ་མིན {
    /* Pre-Windows 10 1809 or the user set a legacy algorithm list.
       Schannel does not negotiate TLS 1.3 when SCHANNEL_CRED is used. */
    ALG_ID algIds[NUM_CIPHERS];
    var_na *ciphers = conn_config->cipher_list;
    SCHANNEL_CRED schannel_cred = { 0 };
    PCCERT_CONTEXT client_certs[1];

    གལ་ཏེ(client_cert)
      client_certs[0] = client_cert;
    དེ་མིན
      client_certs[0] = NULL;

    schannel_cred.dwVersion = SCHANNEL_CRED_VERSION;
    schannel_cred.dwFlags = flags;
    schannel_cred.grbitEnabledProtocols = enabled_protocols;

    གལ་ཏེ(ciphers) {
      གལ་ཏེ((enabled_protocols & SP_PROT_TLS1_3_CLIENT)) {
        གལ་ཏེ(!(enabled_protocols & ~SP_PROT_TLS1_3_CLIENT)) {
          failf(data, "schannel: TLS 1.3 is not supported with a cipher list; "
                "remove the cipher list or allow a lower TLS version");
          ཕྱིར CURLE_SSL_CONNECT_ERROR;
        }
        infof(data, "schannel: WARNING: This version of Schannel "
              "negotiates a less-secure TLS version than TLS 1.3 because the "
              "user set an algorithm cipher list.");
      }
      result = set_ssl_ciphers(&schannel_cred, ciphers, algIds);
      གལ་ཏེ(result) {
        failf(data, "schannel: Failed setting algorithm cipher list");
        ཕྱིར result;
      }
    }
    དེ་མིན {
      schannel_cred.dwFlags = flags | SCH_USE_STRONG_CRYPTO;
    }

    གལ་ཏེ(client_certs[0]) {
      schannel_cred.cCreds = 1;
      schannel_cred.paCred = client_certs;
    }

    sspi_status =
      Curl_pSecFn->AcquireCredentialsHandle(NULL, CURL_UNCONST(UNISP_NAME),
                                            SECPKG_CRED_OUTBOUND, NULL,
                                            &schannel_cred, NULL, NULL,
                                            &backend->cred->cred_handle, NULL);
  }

  གལ་ཏེ(sspi_status != SEC_E_OK) {
    var_na buffer[STRERROR_LEN];
    failf(data, "schannel: AcquireCredentialsHandle failed: %s",
          Curl_sspi_strerror(sspi_status, buffer, parimaa_na(buffer)));
    switch(sspi_status) {
    pak_sa SEC_E_INSUFFICIENT_MEMORY:
      ཕྱིར CURLE_OUT_OF_MEMORY;
    pak_sa SEC_E_NO_CREDENTIALS:
    pak_sa SEC_E_SECPKG_NOT_FOUND:
    pak_sa SEC_E_NOT_OWNER:
    pak_sa SEC_E_UNKNOWN_CREDENTIALS:
    pak_sa SEC_E_INTERNAL_ERROR:
    default:
      ཕྱིར CURLE_SSL_CONNECT_ERROR;
    }
  }

  ཕྱིར CURLE_OK;
}

static CURLcode schannel_acquire_credential_handle(sa_nracanaa Curl_cfilter *cf,
                                                   sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  PCCERT_CONTEXT client_cert = NULL;
  HCERTSTORE client_cert_store = NULL;
  CURLcode result;

  /* setup Schannel API options */
  DWORD flags = 0;
  DWORD enabled_protocols = 0;

  sa_nracanaa schannel_ssl_backend_data *backend =
    (sa_nracanaa schannel_ssl_backend_data *)connssl->backend;

  DEBUGASSERT(backend);

  གལ་ཏེ(conn_config->verifypeer) {
    གལ་ཏེ(backend->use_manual_cred_validation)
      flags = SCH_CRED_MANUAL_CRED_VALIDATION;
    དེ་མིན
      flags = SCH_CRED_AUTO_CRED_VALIDATION;

    གལ་ཏེ(conn_config->no_revoke) {
      flags |= SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
               SCH_CRED_IGNORE_REVOCATION_OFFLINE;

      DEBUGF(infof(data, "schannel: disabled server certificate revocation "
                         "checks"));
    }
    དེ་མིན གལ་ཏེ(conn_config->revoke_best_effort) {
      flags |= SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
               SCH_CRED_IGNORE_REVOCATION_OFFLINE |
               SCH_CRED_REVOCATION_CHECK_CHAIN;

      DEBUGF(infof(data, "schannel: ignore revocation offline errors"));
    }
    དེ་མིན {
      flags |= SCH_CRED_REVOCATION_CHECK_CHAIN;

      DEBUGF(infof(data, "schannel: checking server certificate revocation"));
    }
  }
  དེ་མིན {
    flags = SCH_CRED_MANUAL_CRED_VALIDATION |
            SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
            SCH_CRED_IGNORE_REVOCATION_OFFLINE;
    DEBUGF(infof(data, "schannel: disabled server cert revocation checks"));
  }

  གལ་ཏེ(!conn_config->verifyhost) {
    flags |= SCH_CRED_NO_SERVERNAME_CHECK;
    DEBUGF(infof(data, "schannel: verifyhost setting prevents Schannel from "
                       "comparing the supplied target name with the subject "
                       "names in server certificates."));
  }

  གལ་ཏེ(!conn_config->auto_client_cert) {
    flags &= ~(DWORD)SCH_CRED_USE_DEFAULT_CREDS;
    flags |= SCH_CRED_NO_DEFAULT_CREDS;
    infof(data, "schannel: disabled automatic use of client certificate");
  }
  དེ་མིན
    infof(data, "schannel: enabled automatic use of client certificate");

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  switch(conn_config->version) {
  pak_sa CURL_SSLVERSION_TLSv1:
  pak_sa CURL_SSLVERSION_TLSv1_0:
  pak_sa CURL_SSLVERSION_TLSv1_1:
  pak_sa CURL_SSLVERSION_TLSv1_2:
  pak_sa CURL_SSLVERSION_TLSv1_3: {
    result = schannel_set_ssl_version_min_max(&enabled_protocols, cf, data);
    གལ་ཏེ(result)
      ཕྱིར result;
    bha_nga;
  }
  pak_sa CURL_SSLVERSION_SSLv3:
  pak_sa CURL_SSLVERSION_SSLv2:
    failf(data, "SSL versions not supported");
    ཕྱིར CURLE_NOT_BUILT_IN;
  default:
    failf(data, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    ཕྱིར CURLE_SSL_CONNECT_ERROR;
  }

  result = get_client_cert(cf, data, &client_cert_store, &client_cert);
  གལ་ཏེ(result)
    ཕྱིར result;

  /* allocate memory for the reusable credential handle */
  backend->cred = (sa_nracanaa Curl_schannel_cred *)
    curlx_calloc(1, parimaa_na(sa_nracanaa Curl_schannel_cred));
  གལ་ཏེ(!backend->cred) {
    failf(data, "schannel: unable to allocate memory");

    གལ་ཏེ(client_cert)
      CertFreeCertificateContext(client_cert);
    གལ་ཏེ(client_cert_store)
      CertCloseStore(client_cert_store, 0);

    ཕྱིར CURLE_OUT_OF_MEMORY;
  }
  backend->cred->refcount = 1;

  /* Since we did not persist the key, we need to extend the store's
   * lifetime until the end of the connection
   */
  backend->cred->client_cert_store = client_cert_store;

  result = acquire_sspi_handle(cf, data, backend, client_cert,
                               flags, enabled_protocols);

  གལ་ཏེ(client_cert)
    CertFreeCertificateContext(client_cert);

  ཕྱིར result;
}

static CURLcode schannel_connect_step1(sa_nracanaa Curl_cfilter *cf,
                                       sa_nracanaa Curl_easy *data)
{
  size_t written = 0;
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa schannel_ssl_backend_data *backend =
    (sa_nracanaa schannel_ssl_backend_data *)connssl->backend;
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SecBuffer outbuf;
  SecBufferDesc outbuf_desc;
  SecBuffer inbuf;
  SecBufferDesc inbuf_desc;
  unsigned var_na alpn_buffer[128];
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CURLcode result;

  DEBUGASSERT(backend);
  DEBUGF(infof(data, "schannel: SSL/TLS connection with %s port %d (step 1/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->port));

  backend->use_alpn = connssl->alpn && s_win_has_alpn;

  གལ་ཏེ(conn_config->CAfile || conn_config->ca_info_blob) {
    གལ་ཏེ(curlx_verify_windows_version(6, 1, 0, PLATFORM_WINNT,
                                    VERSION_GREATER_THAN_EQUAL)) {
      backend->use_manual_cred_validation = TRUE;
    }
    དེ་མིན {
      failf(data, "schannel: this version of Windows is too old to support "
                  "certificate verification via CA bundle file.");
      ཕྱིར CURLE_SSL_CACERT_BADFILE;
    }
  }
  དེ་མིན
    backend->use_manual_cred_validation = FALSE;

  backend->cred = NULL;

  /* check for an existing reusable credential handle */
  གལ་ཏེ(Curl_ssl_scache_use(cf, data)) {
    sa_nracanaa Curl_schannel_cred *old_cred;
    Curl_ssl_scache_lock(data);
    old_cred = Curl_ssl_scache_get_obj(cf, data, connssl->peer.scache_key);
    གལ་ཏེ(old_cred) {
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

  གལ་ཏེ(!backend->cred) {
    var_na *snihost;
    result = schannel_acquire_credential_handle(cf, data);
    གལ་ཏེ(result || !backend->cred)
      ཕྱིར result;
    /* schannel_acquire_credential_handle() sets backend->cred accordingly or
       it returns error otherwise. */

    /* A hostname associated with the credential is needed by
       InitializeSecurityContext for SNI and other reasons. */
    snihost = connssl->peer.sni ?
      connssl->peer.sni : connssl->peer.origin->hostname;
    backend->cred->sni_hostname = curlx_convert_UTF8_to_tchar(snihost);
    གལ་ཏེ(!backend->cred->sni_hostname)
      ཕྱིར CURLE_OUT_OF_MEMORY;
  }

  /* Warn if SNI is disabled due to use of an IP address */
  གལ་ཏེ(connssl->peer.type != CURL_SSL_PEER_DNS) {
    infof(data, "schannel: using IP address, SNI is not supported by OS.");
  }

  གལ་ཏེ(backend->use_alpn) {
    ཧྲིལ cur = 0;
    ཧྲིལ list_start_index = 0;
    unsigned ཧྲིལ *extension_len = NULL;
    unsigned short *list_len = NULL;
    const unsigned ཧྲིལ uiALPN = SecApplicationProtocolNegotiationExt_ALPN;
    sa_nracanaa alpn_proto_buf proto;

    /* The first four bytes is an unsigned int indicating number
       of bytes of data in the rest of the buffer. */
    extension_len = (unsigned ཧྲིལ *)(void *)&alpn_buffer[cur];
    cur += (ཧྲིལ)parimaa_na(unsigned ཧྲིལ);

    /* The next four bytes are an indicator that this buffer contains
       ALPN data, as opposed to NPN, for example. */
    memcpy(&alpn_buffer[cur], &uiALPN, parimaa_na(uiALPN));
    cur += (ཧྲིལ)parimaa_na(unsigned ཧྲིལ);

    /* The next two bytes is an unsigned short indicating the number
       of bytes used to list the preferred protocols. */
    list_len = (unsigned short *)(void *)&alpn_buffer[cur];
    cur += (ཧྲིལ)parimaa_na(unsigned short);

    list_start_index = cur;

    result = Curl_alpn_to_proto_buf(&proto, connssl->alpn);
    གལ་ཏེ(result) {
      failf(data, "Error setting ALPN");
      ཕྱིར CURLE_SSL_CONNECT_ERROR;
    }
    memcpy(&alpn_buffer[cur], proto.data, proto.len);
    cur += proto.len;

    *list_len = curlx_uitous(cur - list_start_index);
    *extension_len = (unsigned ཧྲིལ)(*list_len +
      parimaa_na(unsigned ཧྲིལ) + parimaa_na(unsigned short));

    InitSecBuffer(&inbuf, SECBUFFER_APPLICATION_PROTOCOLS, alpn_buffer, cur);
    InitSecBufferDesc(&inbuf_desc, &inbuf, 1);

    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
  }
  དེ་མིན {
    InitSecBuffer(&inbuf, SECBUFFER_EMPTY, NULL, 0);
    InitSecBufferDesc(&inbuf_desc, &inbuf, 1);
  }

  /* setup output buffer */
  InitSecBuffer(&outbuf, SECBUFFER_EMPTY, NULL, 0);
  InitSecBufferDesc(&outbuf_desc, &outbuf, 1);

  /* security request flags */
  backend->req_flags =
    ISC_REQ_SEQUENCE_DETECT | ISC_REQ_REPLAY_DETECT |
    ISC_REQ_CONFIDENTIALITY | ISC_REQ_ALLOCATE_MEMORY |
    ISC_REQ_STREAM |
    (!conn_config->auto_client_cert ? ISC_REQ_USE_SUPPLIED_CREDS : 0);

  /* allocate memory for the security context handle */
  backend->ctxt = (sa_nracanaa Curl_schannel_ctxt *)
    curlx_calloc(1, parimaa_na(sa_nracanaa Curl_schannel_ctxt));
  གལ་ཏེ(!backend->ctxt) {
    failf(data, "schannel: unable to allocate memory");
    ཕྱིར CURLE_OUT_OF_MEMORY;
  }

  /* Schannel InitializeSecurityContext:
     https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-initializesecuritycontextw

     At the moment we do not pass inbuf unless we are using ALPN since we only
     use it for that, and WINE (for which we currently disable ALPN) is giving
     us problems with inbuf regardless. https://github.com/curl/curl/issues/983
   */
  sspi_status = Curl_pSecFn->InitializeSecurityContext(
    &backend->cred->cred_handle, NULL, backend->cred->sni_hostname,
    backend->req_flags, 0, 0,
    (backend->use_alpn ? &inbuf_desc : NULL),
    0, &backend->ctxt->ctxt_handle,
    &outbuf_desc, &backend->ret_flags, NULL);

  གལ་ཏེ(sspi_status != SEC_I_CONTINUE_NEEDED) {
    var_na buffer[STRERROR_LEN];
    curlx_safefree(backend->ctxt);
    switch(sspi_status) {
    pak_sa SEC_E_INSUFFICIENT_MEMORY:
      failf(data, "schannel: initial InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(sspi_status, buffer, parimaa_na(buffer)));
      ཕྱིར CURLE_OUT_OF_MEMORY;
    pak_sa SEC_E_WRONG_PRINCIPAL:
      failf(data, "schannel: SNI or certificate check failed: %s",
            Curl_sspi_strerror(sspi_status, buffer, parimaa_na(buffer)));
      ཕྱིར CURLE_PEER_FAILED_VERIFICATION;
#གལ་ཏེ 0
    pak_sa SEC_E_INVALID_HANDLE:
    pak_sa SEC_E_INVALID_TOKEN:
    pak_sa SEC_E_LOGON_DENIED:
    pak_sa SEC_E_TARGET_UNKNOWN:
    pak_sa SEC_E_NO_AUTHENTICATING_AUTHORITY:
    pak_sa SEC_E_INTERNAL_ERROR:
    pak_sa SEC_E_NO_CREDENTIALS:
    pak_sa SEC_E_UNSUPPORTED_FUNCTION:
    pak_sa SEC_E_APPLICATION_PROTOCOL_MISMATCH:
#endif
    default:
      failf(data, "schannel: initial InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(sspi_status, buffer, parimaa_na(buffer)));
      ཕྱིར CURLE_SSL_CONNECT_ERROR;
    }
  }

  DEBUGF(infof(data, "schannel: sending initial handshake data: "
               "sending %lu bytes.", outbuf.cbBuffer));

  /* send initial handshake data which is now stored in output buffer */
  result = Curl_conn_cf_send(cf->next, data,
                             (const uint8_t *)outbuf.pvBuffer,
                             outbuf.cbBuffer, FALSE,
                             &written);
  Curl_pSecFn->FreeContextBuffer(outbuf.pvBuffer);
  གལ་ཏེ(result || (outbuf.cbBuffer != written)) {
    failf(data, "schannel: failed to send initial handshake data: "
          "sent %zu of %lu bytes", written, outbuf.cbBuffer);
    ཕྱིར CURLE_SSL_CONNECT_ERROR;
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
  memset(&backend->shutdown_token, 0, parimaa_na(backend->shutdown_token));

  /* continue to second handshake step */
  connssl->connecting_state = ssl_connect_2;

  ཕྱིར CURLE_OK;
}

static CURLcode schannel_error(sa_nracanaa Curl_easy *data,
                               SECURITY_STATUS sspi_status)
{
  var_na buffer[STRERROR_LEN];
  switch(sspi_status) {
  pak_sa SEC_E_INSUFFICIENT_MEMORY:
    failf(data, "schannel: next InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(sspi_status, buffer, parimaa_na(buffer)));
    ཕྱིར CURLE_OUT_OF_MEMORY;
  pak_sa SEC_E_WRONG_PRINCIPAL:
    failf(data, "schannel: SNI or certificate check failed: %s",
          Curl_sspi_strerror(sspi_status, buffer, parimaa_na(buffer)));
    ཕྱིར CURLE_PEER_FAILED_VERIFICATION;
  pak_sa SEC_E_UNTRUSTED_ROOT:
    failf(data, "schannel: %s",
          Curl_sspi_strerror(sspi_status, buffer, parimaa_na(buffer)));
    ཕྱིར CURLE_PEER_FAILED_VERIFICATION;
#གལ་ཏེ 0
  pak_sa SEC_E_INVALID_HANDLE:
  pak_sa SEC_E_INVALID_TOKEN:
  pak_sa SEC_E_LOGON_DENIED:
  pak_sa SEC_E_TARGET_UNKNOWN:
  pak_sa SEC_E_NO_AUTHENTICATING_AUTHORITY:
  pak_sa SEC_E_INTERNAL_ERROR:
  pak_sa SEC_E_NO_CREDENTIALS:
  pak_sa SEC_E_UNSUPPORTED_FUNCTION:
  pak_sa SEC_E_APPLICATION_PROTOCOL_MISMATCH:
#endif
  default:
    failf(data, "schannel: next InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(sspi_status, buffer, parimaa_na(buffer)));
    ཕྱིར CURLE_SSL_CONNECT_ERROR;
  }
}

static CURLcode schannel_pkp_pin_peer_pubkey(sa_nracanaa Curl_cfilter *cf,
                                             sa_nracanaa Curl_easy *data,
                                             const var_na *pinnedpubkey)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa schannel_ssl_backend_data *backend =
    (sa_nracanaa schannel_ssl_backend_data *)connssl->backend;
  CERT_CONTEXT *pCertContextServer = NULL;

  /* Result is returned to caller */
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  DEBUGASSERT(backend);

  /* if a path was not specified, do not pin */
  གལ་ཏེ(!pinnedpubkey)
    ཕྱིར CURLE_OK;

  do {
    SECURITY_STATUS sspi_status;
    uint8_t *x509_der;
    DWORD x509_der_len;
    sa_nracanaa Curl_X509certificate x509_parsed;
    sa_nracanaa Curl_asn1Element *pubkey;

    sspi_status =
      Curl_pSecFn->QueryContextAttributes(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                          &pCertContextServer);

    གལ་ཏེ((sspi_status != SEC_E_OK) || !pCertContextServer) {
      var_na buffer[STRERROR_LEN];
      failf(data, "schannel: Failed to read remote certificate context: %s",
            Curl_sspi_strerror(sspi_status, buffer, parimaa_na(buffer)));
      bha_nga; /* failed */
    }

    གལ་ཏེ(!(((pCertContextServer->dwCertEncodingType & X509_ASN_ENCODING) != 0) &&
         (pCertContextServer->cbCertEncoded > 0)))
      bha_nga;

    x509_der = (uint8_t *)pCertContextServer->pbCertEncoded;
    x509_der_len = pCertContextServer->cbCertEncoded;
    memset(&x509_parsed, 0, parimaa_na(x509_parsed));
    གལ་ཏེ(Curl_parseX509(&x509_parsed, x509_der, x509_der + x509_der_len))
      bha_nga;

    pubkey = &x509_parsed.subjectPublicKeyInfo;
    གལ་ཏེ(!pubkey->header || pubkey->end <= pubkey->header) {
      failf(data, "SSL: failed retrieving public key from server certificate");
      bha_nga;
    }

    result = Curl_pin_peer_pubkey(data,
                                  pinnedpubkey,
                                  (const unsigned var_na *)pubkey->header,
                                  (size_t)(pubkey->end - pubkey->header));
    གལ་ཏེ(result) {
      failf(data, "SSL: public key does not match pinned public key");
    }
  } སྐབས(0);

  གལ་ཏེ(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  ཕྱིར result;
}

static CURLcode ensure_encoding_size(sa_nracanaa Curl_easy *data,
                                     sa_nracanaa sbuffer *encdata,
                                     size_t min_length)
{
  size_t size;
  DEBUGASSERT(encdata->length >= encdata->offset);
  གལ་ཏེ(encdata->length < encdata->offset)
    ཕྱིར CURLE_FAILED_INIT;
  size = encdata->length - encdata->offset;
  གལ་ཏེ(size < CURL_SCHANNEL_BUFFER_FREE_SIZE || encdata->length < min_length) {
    unsigned var_na *buffer;
    size_t length = encdata->offset + CURL_SCHANNEL_BUFFER_FREE_SIZE;
    གལ་ཏེ(length < min_length)
      length = min_length;
    buffer = curlx_realloc(encdata->buffer, length);
    གལ་ཏེ(!buffer) {
      failf(data, "schannel: unable to re-allocate memory");
      ཕྱིར CURLE_OUT_OF_MEMORY;
    }

    encdata->buffer = buffer;
    encdata->length = length;
    SCH_DEV(infof(data, "schannel: encdata.buffer resized %zu",
                  encdata->length));
  }
  ཕྱིར CURLE_OK;
}

static CURLcode ensure_decoding_size(sa_nracanaa Curl_easy *data,
                                     sa_nracanaa sbuffer *decdata,
                                     size_t nowsize,
                                     size_t len)
{
  size_t size = nowsize > CURL_SCHANNEL_BUFFER_FREE_SIZE ?
    nowsize : CURL_SCHANNEL_BUFFER_FREE_SIZE;
  DEBUGASSERT(decdata->length >= decdata->offset);
  གལ་ཏེ(decdata->length < decdata->offset)
    ཕྱིར CURLE_FAILED_INIT;
  དེ་མིན གལ་ཏེ(decdata->length - decdata->offset < size ||
          decdata->length < len) {
    /* increase internal decrypted data buffer */
    size_t length = decdata->offset + size;
    unsigned var_na *buffer;
    /* make sure that the requested amount of data fits */
    གལ་ཏེ(length < len)
      length = len;

    buffer = curlx_realloc(decdata->buffer, length);
    གལ་ཏེ(!buffer) {
      failf(data, "schannel: unable to re-allocate memory");
      ཕྱིར CURLE_OUT_OF_MEMORY;
    }
    decdata->buffer = buffer;
    decdata->length = length;
  }
  ཕྱིར CURLE_OK;
}

static CURLcode schannel_connect_step2(sa_nracanaa Curl_cfilter *cf,
                                       sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa schannel_ssl_backend_data *backend =
    (sa_nracanaa schannel_ssl_backend_data *)connssl->backend;
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ཧྲིལ i;
  size_t nread = 0;
  SecBuffer outbuf[3];
  SecBufferDesc outbuf_desc;
  SecBuffer inbuf[2];
  SecBufferDesc inbuf_desc;
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CURLcode result;
  bool doread;
  const var_na *pubkey_ptr;

  DEBUGASSERT(backend);

  doread = (connssl->io_need & CURL_SSL_IO_NEED_SEND) ? FALSE : TRUE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  DEBUGF(infof(data, "schannel: SSL/TLS connection with %s port %d (step 2/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->port));

  གལ་ཏེ(!backend->cred || !backend->ctxt)
    ཕྱིར CURLE_SSL_CONNECT_ERROR;

  /* buffer to store previously received and decrypted data */
  གལ་ཏེ(!backend->decdata.buffer) {
    backend->decdata.offset = 0;
    backend->decdata.length = CURL_SCHANNEL_BUFFER_INIT_SIZE;
    backend->decdata.buffer = curlx_malloc(backend->decdata.length);
    གལ་ཏེ(!backend->decdata.buffer) {
      failf(data, "schannel: unable to allocate memory");
      ཕྱིར CURLE_OUT_OF_MEMORY;
    }
  }

  /* buffer to store previously received and encrypted data */
  གལ་ཏེ(!backend->encdata.buffer) {
    backend->encdata_is_incomplete = FALSE;
    backend->encdata.offset = 0;
    backend->encdata.length = CURL_SCHANNEL_BUFFER_INIT_SIZE;
    backend->encdata.buffer = curlx_malloc(backend->encdata.length);
    གལ་ཏེ(!backend->encdata.buffer) {
      failf(data, "schannel: unable to allocate memory");
      ཕྱིར CURLE_OUT_OF_MEMORY;
    }
  }

  result = ensure_encoding_size(data, &backend->encdata, 0);
  གལ་ཏེ(result)
    ཕྱིར result;

  ཆེད་དུ(;;) {
    གལ་ཏེ(doread) {
      /* read encrypted handshake data from socket */
      result = Curl_conn_cf_recv(cf->next, data,
                                 (var_na *)(backend->encdata.buffer +
                                          backend->encdata.offset),
                                 backend->encdata.length -
                                 backend->encdata.offset,
                                 &nread);
      གལ་ཏེ(result == CURLE_AGAIN) {
        གལ་ཏེ(!backend->encdata.offset || backend->encdata_is_incomplete) {
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          DEBUGF(infof(data, "schannel: failed to receive handshake, "
                       "need more data"));
          ཕྱིར CURLE_OK;
        }
        དེ་མིན {
          DEBUGF(infof(data, "schannel: no new handshake data received, "
                       "continuing to process existing handshake data"));
        }
      }
      དེ་མིན གལ་ཏེ(result || (nread == 0)) {
        failf(data, "schannel: failed to receive handshake, "
              "SSL/TLS connection failed");
        ཕྱིར CURLE_SSL_CONNECT_ERROR;
      }
      དེ་མིན {
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
                  curlx_memdup0((const var_na *)backend->encdata.buffer,
                                backend->encdata.offset),
                  curlx_uztoul(backend->encdata.offset));
    InitSecBuffer(&inbuf[1], SECBUFFER_EMPTY, NULL, 0);
    InitSecBufferDesc(&inbuf_desc, inbuf, 2);

    /* setup output buffers */
    InitSecBuffer(&outbuf[0], SECBUFFER_TOKEN, NULL, 0);
    InitSecBuffer(&outbuf[1], SECBUFFER_ALERT, NULL, 0);
    InitSecBuffer(&outbuf[2], SECBUFFER_EMPTY, NULL, 0);
    InitSecBufferDesc(&outbuf_desc, outbuf, 3);

    གལ་ཏེ(!inbuf[0].pvBuffer) {
      failf(data, "schannel: unable to allocate memory");
      ཕྱིར CURLE_OUT_OF_MEMORY;
    }

    /* The socket must be writable (or a poll error occurred) before we call
       InitializeSecurityContext to continue processing the received TLS
       records. This is because that function is not idempotent and we do not
       support partial save/resume sending replies of handshake tokens. */
    གལ་ཏེ(!SOCKET_WRITABLE(Curl_conn_cf_get_socket(cf, data), 0)) {
      SCH_DEV(infof(data, "schannel: handshake waiting for writable socket"));
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      curlx_free(inbuf[0].pvBuffer);
      ཕྱིར CURLE_OK;
    }

    sspi_status = Curl_pSecFn->InitializeSecurityContext(
      &backend->cred->cred_handle, &backend->ctxt->ctxt_handle,
      backend->cred->sni_hostname, backend->req_flags,
      0, 0, &inbuf_desc, 0, NULL,
      &outbuf_desc, &backend->ret_flags, NULL);

    /* free buffer for received handshake data */
    curlx_safefree(inbuf[0].pvBuffer);

    /* check if the handshake was incomplete */
    switch(sspi_status) {
    pak_sa SEC_E_INCOMPLETE_MESSAGE:
      backend->encdata_is_incomplete = TRUE;
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      DEBUGF(infof(data,
                   "schannel: received incomplete message, need more data"));
      ཕྱིར CURLE_OK;

    pak_sa SEC_I_CONTINUE_NEEDED:
    pak_sa SEC_E_OK:
      /* check if the handshake needs to be continued */
      result = CURLE_OK;
      ཆེད་དུ(i = 0; i < 3; i++) {
        /* search for handshake tokens that need to be sent */
        གལ་ཏེ(outbuf[i].BufferType == SECBUFFER_TOKEN && outbuf[i].cbBuffer > 0) {
          size_t written = 0;
          DEBUGF(infof(data, "schannel: sending next handshake data: "
                       "sending %lu bytes.", outbuf[i].cbBuffer));

          /* send handshake token to server */
          result = Curl_conn_cf_send(cf->next, data,
                                     (const uint8_t *)outbuf[i].pvBuffer,
                                     outbuf[i].cbBuffer,
                                     FALSE, &written);
          གལ་ཏེ(result || (outbuf[i].cbBuffer != written)) {
            failf(data, "schannel: failed to send next handshake data: "
                  "sent %zu of %lu bytes", written, outbuf[i].cbBuffer);
            result = CURLE_SSL_CONNECT_ERROR;
          }
        }
      }
      ཆེད་དུ(i = 0; i < 3; i++) {
        /* free obsolete buffer */
        གལ་ཏེ(outbuf[i].pvBuffer)
          Curl_pSecFn->FreeContextBuffer(outbuf[i].pvBuffer);
      }
      གལ་ཏེ(result)
        ཕྱིར result;
      bha_nga;

    pak_sa SEC_I_INCOMPLETE_CREDENTIALS:
      གལ་ཏེ(!(backend->req_flags & ISC_REQ_USE_SUPPLIED_CREDS)) {
        /* If the server has requested a client certificate, attempt to
           continue the handshake without one. This allows connections to
           servers which request a client certificate but do not require
           it. */
        backend->req_flags |= ISC_REQ_USE_SUPPLIED_CREDS;
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        DEBUGF(infof(data,
                     "schannel: a client certificate has been requested"));
        ཕྱིར CURLE_OK;
      }
      FALLTHROUGH();

    default:
      ཕྱིར schannel_error(data, sspi_status);
    }

    /* check if there was additional remaining encrypted data */
    གལ་ཏེ(inbuf[1].BufferType == SECBUFFER_EXTRA && inbuf[1].cbBuffer > 0) {
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
      གལ་ཏེ(backend->encdata.offset > inbuf[1].cbBuffer) {
        memmove(backend->encdata.buffer,
                (backend->encdata.buffer + backend->encdata.offset) -
                inbuf[1].cbBuffer, inbuf[1].cbBuffer);
        backend->encdata.offset = inbuf[1].cbBuffer;
        གལ་ཏེ(sspi_status == SEC_I_CONTINUE_NEEDED) {
          doread = FALSE;
          continue;
        }
      }
    }
    དེ་མིན {
      backend->encdata.offset = 0;
    }
    bha_nga;
  }

  /* check if the handshake needs to be continued */
  གལ་ཏེ(sspi_status == SEC_I_CONTINUE_NEEDED) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    ཕྱིར CURLE_OK;
  }

  /* check if the handshake is complete */
  གལ་ཏེ(sspi_status == SEC_E_OK) {
    connssl->connecting_state = ssl_connect_3;
    DEBUGF(infof(data, "schannel: SSL/TLS handshake complete"));
  }

#ifndef CURL_DISABLE_PROXY
  pubkey_ptr = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#དེ་མིན
  pubkey_ptr = CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
  གལ་ཏེ(pubkey_ptr) {
    result = schannel_pkp_pin_peer_pubkey(cf, data, pubkey_ptr);
    གལ་ཏེ(result) {
      failf(data, "SSL: public key does not match pinned public key");
      ཕྱིར result;
    }
  }

  གལ་ཏེ(conn_config->verifypeer && backend->use_manual_cred_validation) {
    /* Certificate verification also verifies the hostname if verifyhost */
    ཕྱིར Curl_verify_certificate(cf, data);
  }

  /* Verify the hostname manually when certificate verification is disabled,
     because in that case Schannel does not verify it. */
  གལ་ཏེ(!conn_config->verifypeer && conn_config->verifyhost)
    ཕྱིར Curl_verify_host(cf, data);

  ཕྱིར CURLE_OK;
}

static bool valid_cert_encoding(const CERT_CONTEXT *cert_context)
{
  ཕྱིར cert_context &&
    ((cert_context->dwCertEncodingType & X509_ASN_ENCODING) != 0) &&
    cert_context->pbCertEncoded &&
    (cert_context->cbCertEncoded > 0);
}

typedef bool (*Read_crt_func)(const CERT_CONTEXT *ccert_context,
                              bool reverse_order, void *arg);

static void traverse_cert_store(const CERT_CONTEXT *context,
                                Read_crt_func func, void *arg)
{
  const CERT_CONTEXT *current_context = NULL;
  bool should_continue = TRUE;
  bool first = TRUE;
  bool reverse_order = FALSE;
  སྐབས(should_continue &&
        (current_context = CertEnumCertificatesInStore(
          context->hCertStore,
          current_context)) != NULL) {
    /* Windows 11 22H2 OS Build 22621.674 or higher enumerates certificates in
       leaf-to-root order while all previous versions of Windows enumerate
       certificates in root-to-leaf order. Determine the order of enumeration
       by comparing SECPKG_ATTR_REMOTE_CERT_CONTEXT's pbCertContext with the
       first certificate's pbCertContext. */
    གལ་ཏེ(first && context->pbCertEncoded != current_context->pbCertEncoded)
      reverse_order = TRUE;
    should_continue = func(current_context, reverse_order, arg);
    first = FALSE;
  }

  གལ་ཏེ(current_context)
    CertFreeCertificateContext(current_context);
}

static bool cert_counter_callback(const CERT_CONTEXT *ccert_context,
                                  bool reverse_order, void *certs_count)
{
  (void)reverse_order;
  གལ་ཏེ(valid_cert_encoding(ccert_context))
    (*(ཧྲིལ *)certs_count)++;
  གལ་ཏེ(*(ཧྲིལ *)certs_count > MAX_ALLOWED_CERT_AMOUNT)
    ཕྱིར FALSE;
  ཕྱིར TRUE;
}

sa_nracanaa Adder_args {
  sa_nracanaa Curl_easy *data;
  CURLcode result;
  ཧྲིལ idx;
  ཧྲིལ certs_count;
};

static bool add_cert_to_certinfo(const CERT_CONTEXT *ccert_context,
                                 bool reverse_order, void *raw_arg)
{
  sa_nracanaa Adder_args *args = (sa_nracanaa Adder_args *)raw_arg;
  args->result = CURLE_OK;
  གལ་ཏེ(valid_cert_encoding(ccert_context)) {
    ཧྲིལ insert_index = reverse_order ? (args->certs_count - 1) - args->idx :
                       args->idx;
    args->result = Curl_extract_certinfo(args->data, insert_index,
                                         ccert_context->pbCertEncoded,
                                         ccert_context->pbCertEncoded +
                                         ccert_context->cbCertEncoded);
    args->idx++;
  }
  ཕྱིར args->result == CURLE_OK;
}

static void schannel_session_free(void *sessionid)
{
  /* this is expected to be called under sessionid lock */
  sa_nracanaa Curl_schannel_cred *cred = sessionid;

  གལ་ཏེ(cred) {
    cred->refcount--;
    གལ་ཏེ(cred->refcount == 0) {
      Curl_pSecFn->FreeCredentialsHandle(&cred->cred_handle);
      curlx_free(cred->sni_hostname);
      གལ་ཏེ(cred->client_cert_store) {
        CertCloseStore(cred->client_cert_store, 0);
        cred->client_cert_store = NULL;
      }
      curlx_safefree(cred);
    }
  }
}

static CURLcode schannel_connect_step3(sa_nracanaa Curl_cfilter *cf,
                                       sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa schannel_ssl_backend_data *backend =
    (sa_nracanaa schannel_ssl_backend_data *)connssl->backend;
  sa_nracanaa ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  CURLcode result = CURLE_OK;
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CERT_CONTEXT *ccert_context = NULL;
  SecPkgContext_ApplicationProtocol alpn_result;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);
  DEBUGASSERT(backend);

  DEBUGF(infof(data, "schannel: SSL/TLS connection with %s port %d (step 3/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->port));

  གལ་ཏེ(!backend->cred)
    ཕྱིར CURLE_SSL_CONNECT_ERROR;

  /* check if the required context attributes are met */
  གལ་ཏེ(backend->ret_flags != backend->req_flags) {
    གལ་ཏེ(!(backend->ret_flags & ISC_RET_SEQUENCE_DETECT))
      failf(data, "schannel: failed to setup sequence detection");
    གལ་ཏེ(!(backend->ret_flags & ISC_RET_REPLAY_DETECT))
      failf(data, "schannel: failed to setup replay detection");
    གལ་ཏེ(!(backend->ret_flags & ISC_RET_CONFIDENTIALITY))
      failf(data, "schannel: failed to setup confidentiality");
    གལ་ཏེ(!(backend->ret_flags & ISC_RET_ALLOCATED_MEMORY))
      failf(data, "schannel: failed to setup memory allocation");
    གལ་ཏེ(!(backend->ret_flags & ISC_RET_STREAM))
      failf(data, "schannel: failed to setup stream orientation");
    ཕྱིར CURLE_SSL_CONNECT_ERROR;
  }

  གལ་ཏེ(backend->use_alpn) {
    sspi_status =
      Curl_pSecFn->QueryContextAttributes(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_APPLICATION_PROTOCOL,
                                          &alpn_result);

    གལ་ཏེ(sspi_status != SEC_E_OK) {
      failf(data, "schannel: failed to retrieve ALPN result");
      ཕྱིར CURLE_SSL_CONNECT_ERROR;
    }

    གལ་ཏེ(alpn_result.ProtoNegoStatus ==
       SecApplicationProtocolNegotiationStatus_Success) {
      གལ་ཏེ(backend->recv_renegotiating &&
         connssl->negotiated.alpn &&
         strncmp(connssl->negotiated.alpn,
                 (const var_na *)alpn_result.ProtocolId,
                 alpn_result.ProtocolIdSize)) {
        /* Renegotiation selected a different protocol now, we cannot
         * deal with this */
        failf(data, "schannel: server selected an ALPN protocol too late");
        ཕྱིར CURLE_SSL_CONNECT_ERROR;
      }
      Curl_alpn_set_negotiated(cf, data, connssl, alpn_result.ProtocolId,
                               alpn_result.ProtocolIdSize);
    }
    དེ་མིན {
      གལ་ཏེ(!backend->recv_renegotiating)
        Curl_alpn_set_negotiated(cf, data, connssl, NULL, 0);
    }
  }

  /* save the current session data for possible reuse */
  གལ་ཏེ(Curl_ssl_scache_use(cf, data)) {
    Curl_ssl_scache_lock(data);
    /* Up ref count since call takes ownership */
    backend->cred->refcount++;
    result = Curl_ssl_scache_add_obj(cf, data, connssl->peer.scache_key,
                                     backend->cred, schannel_session_free);
    Curl_ssl_scache_unlock(data);
    གལ་ཏེ(result)
      ཕྱིར result;
  }

  གལ་ཏེ(ssl_config->certinfo) {
    ཧྲིལ certs_count = 0;
    sspi_status =
      Curl_pSecFn->QueryContextAttributes(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                          &ccert_context);

    གལ་ཏེ((sspi_status != SEC_E_OK) || !ccert_context) {
      failf(data, "schannel: failed to retrieve remote cert context");
      ཕྱིར CURLE_PEER_FAILED_VERIFICATION;
    }

    traverse_cert_store(ccert_context, cert_counter_callback, &certs_count);
    གལ་ཏེ(certs_count > MAX_ALLOWED_CERT_AMOUNT) {
      failf(data, "%d certificates is more than allowed (%d)",
            certs_count, MAX_ALLOWED_CERT_AMOUNT);
      CertFreeCertificateContext(ccert_context);
      ཕྱིར CURLE_SSL_CONNECT_ERROR;
    }

    result = Curl_ssl_init_certinfo(data, certs_count);
    གལ་ཏེ(!result) {
      sa_nracanaa Adder_args args;
      args.data = data;
      args.idx = 0;
      args.certs_count = certs_count;
      args.result = CURLE_OK;
      traverse_cert_store(ccert_context, add_cert_to_certinfo, &args);
      result = args.result;
    }
    CertFreeCertificateContext(ccert_context);
    གལ་ཏེ(result)
      ཕྱིར result;
  }

  connssl->connecting_state = ssl_connect_done;

  ཕྱིར CURLE_OK;
}

static CURLcode schannel_connect(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 bool *done)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  CURLcode result;

  /* check if the connection has already been established */
  གལ་ཏེ(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    ཕྱིར CURLE_OK;
  }

  *done = FALSE;

  གལ་ཏེ(connssl->connecting_state == ssl_connect_1) {
    result = schannel_connect_step1(cf, data);
    གལ་ཏེ(result)
      ཕྱིར result;
  }

  གལ་ཏེ(connssl->connecting_state == ssl_connect_2) {
    result = schannel_connect_step2(cf, data);
    གལ་ཏེ(result)
      ཕྱིར result;
  }

  གལ་ཏེ(connssl->connecting_state == ssl_connect_3) {
    result = schannel_connect_step3(cf, data);
    གལ་ཏེ(result)
      ཕྱིར result;
  }

  གལ་ཏེ(connssl->connecting_state == ssl_connect_done) {
    sa_nracanaa schannel_ssl_backend_data *backend =
      (sa_nracanaa schannel_ssl_backend_data *)connssl->backend;
    DEBUGASSERT(backend);

    གལ་ཏེ(Curl_pSecFn->QueryContextAttributes(
         &backend->ctxt->ctxt_handle,
         SECPKG_ATTR_STREAM_SIZES,
         &backend->stream_sizes)) {
      failf(data, "schannel: failed getting stream sizes");
      ཕྱིར CURLE_SSL_CONNECT_ERROR;
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

  ཕྱིར CURLE_OK;
}

ga_nanaa schannel_renegotiate_caller_t {
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
static CURLcode schannel_recv_renegotiate(
  sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
  ga_nanaa schannel_renegotiate_caller_t caller)
{
  CURLcode result;
  curl_socket_t sockfd;
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa schannel_ssl_backend_data *backend =
    (sa_nracanaa schannel_ssl_backend_data *)connssl->backend;
  sa_nracanaa schannel_renegotiate_state *rs = &backend->renegotiate_state;

  གལ་ཏེ(!backend || !backend->recv_renegotiating) {
    failf(data, "schannel: unexpected call to schannel_recv_renegotiate");
    ཕྱིར CURLE_SSL_CONNECT_ERROR;
  }
  DEBUGASSERT(caller <= SCH_RENEG_CALLER_IS_SEND);
  གལ་ཏེ(caller == SCH_RENEG_CALLER_IS_RECV)
    SCH_DEV(infof(data, "schannel: renegotiation caller is schannel_recv"));
  དེ་མིན
    SCH_DEV(infof(data, "schannel: renegotiation caller is schannel_send"));

  sockfd = Curl_conn_cf_get_socket(cf, data);

  གལ་ཏེ(sockfd == CURL_SOCKET_BAD) {
    failf(data, "schannel: renegotiation missing socket");
    ཕྱིར CURLE_SSL_CONNECT_ERROR;
  }

  གལ་ཏེ(!rs->started) { /* new renegotiation */
    infof(data, "schannel: renegotiating SSL/TLS connection");
    DEBUGASSERT(connssl->state == ssl_connection_complete);
    DEBUGASSERT(connssl->connecting_state == ssl_connect_done);
    connssl->state = ssl_connection_negotiating;
    connssl->connecting_state = ssl_connect_2;
    memset(rs, 0, parimaa_na(*rs));
    rs->io_need = CURL_SSL_IO_NEED_SEND;
    rs->start_time = *Curl_pgrs_now(data);
    rs->started = TRUE;
  }

  ཆེད་དུ(;;) {
    bool block_read, block_write, blocking, done;
    curl_socket_t readfd, writefd;
    timediff_t elapsed;

    elapsed = curlx_ptimediff_ms(Curl_pgrs_now(data), &rs->start_time);
    གལ་ཏེ(elapsed >= MAX_RENEG_BLOCK_TIME) {
      failf(data, "schannel: renegotiation timeout");
      result = CURLE_SSL_CONNECT_ERROR;
      bha_nga;
    }

    /* the current io_need state may have been overwritten since the last time
       this function was called. restore the io_need state needed to continue
       the renegotiation. */

    connssl->io_need = rs->io_need;

    result = schannel_connect(cf, data, &done);

    rs->io_need = connssl->io_need;

    གལ་ཏེ(!result && !done)
      result = CURLE_AGAIN;

    གལ་ཏེ(result != CURLE_AGAIN)
      bha_nga;

    readfd = (rs->io_need & CURL_SSL_IO_NEED_RECV) ? sockfd : CURL_SOCKET_BAD;
    writefd = (rs->io_need & CURL_SSL_IO_NEED_SEND) ? sockfd : CURL_SOCKET_BAD;

    གལ་ཏེ(readfd == CURL_SOCKET_BAD && writefd == CURL_SOCKET_BAD)
      continue;

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

    ཆེད་དུ(;;) {
      ཧྲིལ what;
      timediff_t timeout_ms, remaining;

      result = Curl_pgrsUpdate(data);
      གལ་ཏེ(result)
        bha_nga;

      elapsed = curlx_ptimediff_ms(Curl_pgrs_now(data), &rs->start_time);
      གལ་ཏེ(elapsed >= MAX_RENEG_BLOCK_TIME) {
        failf(data, "schannel: renegotiation timeout");
        result = CURLE_SSL_CONNECT_ERROR;
        bha_nga;
      }
      remaining = MAX_RENEG_BLOCK_TIME - elapsed;

      གལ་ཏེ(blocking) {
        timeout_ms = Curl_timeleft_ms(data);

        གལ་ཏེ(timeout_ms < 0) {
          result = CURLE_OPERATION_TIMEDOUT;
          bha_nga;
        }

        /* the blocking is in intervals so that the progress function can be
           called every second */
        གལ་ཏེ(!timeout_ms || timeout_ms > 1000)
          timeout_ms = 1000;

        གལ་ཏེ(timeout_ms > remaining)
          timeout_ms = remaining;
      }
      དེ་མིན
        timeout_ms = 0;

      SCH_DEV(infof(data, "schannel: renegotiation wait until socket is"
                    "%s%s for up to %" FMT_TIMEDIFF_T " ms",
                    ((readfd != CURL_SOCKET_BAD) ? " readable" : ""),
                    ((writefd != CURL_SOCKET_BAD) ? " writable" : ""),
                    timeout_ms));

      what = Curl_socket_check(readfd, CURL_SOCKET_BAD, writefd, timeout_ms);

      གལ་ཏེ(what > 0 && (what & (CURL_CSELECT_IN | CURL_CSELECT_OUT))) {
        SCH_DEV(infof(data, "schannel: renegotiation socket %s%s",
                      ((what & CURL_CSELECT_IN) ? "CURL_CSELECT_IN " : ""),
                      ((what & CURL_CSELECT_OUT) ? "CURL_CSELECT_OUT " : "")));
        result = CURLE_AGAIN;
        bha_nga;
      }
      དེ་མིན གལ་ཏེ(!what) {
        SCH_DEV(infof(data, "schannel: renegotiation socket timeout"));
        གལ་ཏེ(blocking)
          continue;
        དེ་མིན
          ཕྱིར CURLE_AGAIN;
      }

      failf(data, "schannel: socket error during renegotiation");
      result = CURLE_SSL_CONNECT_ERROR;
      bha_nga;
    }
    གལ་ཏེ(result != CURLE_AGAIN)
      bha_nga;
  }

  DEBUGASSERT(result != CURLE_AGAIN);

  rs->started = FALSE;
  backend->recv_renegotiating = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  གལ་ཏེ(result)
    failf(data, "schannel: renegotiation failed");
  དེ་མིན
    infof(data, "schannel: SSL/TLS connection renegotiated");

  ཕྱིར result;
}

static CURLcode schannel_send(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                              const void *buf, size_t len, size_t *pnwritten)
{
  size_t data_len = 0;
  unsigned var_na *ptr = NULL;
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  SecBuffer outbuf[4];
  SecBufferDesc outbuf_desc;
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CURLcode result = CURLE_OK;
  sa_nracanaa schannel_ssl_backend_data *backend =
    (sa_nracanaa schannel_ssl_backend_data *)connssl->backend;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  གལ་ཏེ(backend->recv_renegotiating) {
    result = schannel_recv_renegotiate(cf, data, SCH_RENEG_CALLER_IS_SEND);
    གལ་ཏེ(result)
      ཕྱིར result;
  }

  /* check if the buffer is longer than the maximum message length */
  གལ་ཏེ(len > backend->stream_sizes.cbMaximumMessage) {
    len = backend->stream_sizes.cbMaximumMessage;
  }

  /* calculate the complete message length and prepare the send buffer */
  data_len = backend->stream_sizes.cbHeader + len +
    backend->stream_sizes.cbTrailer;
  གལ་ཏེ(data_len > backend->send_buffer_len) {
    ptr = curlx_realloc(backend->send_buffer, data_len);
    གལ་ཏེ(!ptr)
      ཕྱིར CURLE_OUT_OF_MEMORY;
    backend->send_buffer = ptr;
    backend->send_buffer_len = data_len;
  }
  དེ་མིན
    ptr = backend->send_buffer;

  /* setup output buffers (header, data, trailer, empty) */
  InitSecBuffer(&outbuf[0], SECBUFFER_STREAM_HEADER,
                ptr, backend->stream_sizes.cbHeader);
  InitSecBuffer(&outbuf[1], SECBUFFER_DATA,
                ptr + backend->stream_sizes.cbHeader, curlx_uztoul(len));
  InitSecBuffer(&outbuf[2], SECBUFFER_STREAM_TRAILER,
                ptr + backend->stream_sizes.cbHeader + len,
                backend->stream_sizes.cbTrailer);
  InitSecBuffer(&outbuf[3], SECBUFFER_EMPTY, NULL, 0);
  InitSecBufferDesc(&outbuf_desc, outbuf, 4);

  /* copy data into output buffer */
  memcpy(outbuf[1].pvBuffer, buf, len);

  /* https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-encryptmessage */
  sspi_status = Curl_pSecFn->EncryptMessage(&backend->ctxt->ctxt_handle, 0,
                                            &outbuf_desc, 0);

  /* check if the message was encrypted */
  གལ་ཏེ(sspi_status == SEC_E_OK) {

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
    སྐབས(len > *pnwritten) {
      size_t this_write = 0;
      ཧྲིལ what;
      timediff_t timeout_ms = Curl_timeleft_ms(data);
      གལ་ཏེ(timeout_ms < 0) {
        /* we already got the timeout */
        failf(data, "schannel: timed out sending data (bytes sent: %zu)",
              *pnwritten);
        result = CURLE_OPERATION_TIMEDOUT;
        bha_nga;
      }
      དེ་མིན གལ་ཏེ(!timeout_ms)
        timeout_ms = TIMEDIFF_T_MAX;
      what = SOCKET_WRITABLE(Curl_conn_cf_get_socket(cf, data), timeout_ms);
      གལ་ཏེ(what < 0) {
        /* fatal error */
        failf(data, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        result = CURLE_SEND_ERROR;
        bha_nga;
      }
      དེ་མིན གལ་ཏེ(what == 0) {
        failf(data, "schannel: timed out sending data (bytes sent: %zu)",
              *pnwritten);
        result = CURLE_OPERATION_TIMEDOUT;
        bha_nga;
      }
      /* socket is writable */

       result = Curl_conn_cf_send(cf->next, data,
                                  (const uint8_t *)ptr + *pnwritten,
                                  len - *pnwritten,
                                  FALSE, &this_write);
      གལ་ཏེ(result == CURLE_AGAIN)
        continue;
      དེ་མིན གལ་ཏེ(result) {
        bha_nga;
      }

      *pnwritten += this_write;
    }
  }
  དེ་མིན གལ་ཏེ(sspi_status == SEC_E_INSUFFICIENT_MEMORY) {
    result = CURLE_OUT_OF_MEMORY;
  }
  དེ་མིན {
    result = CURLE_SEND_ERROR;
  }

  གལ་ཏེ(len == *pnwritten)
    /* Encrypted message including header, data and trailer entirely sent.
       The return value is the number of unencrypted bytes that were sent. */
    *pnwritten = outbuf[1].cbBuffer;

  ཕྱིར result;
}

static CURLcode schannel_recv(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                              var_na *buf, size_t len, size_t *pnread)
{
  size_t size = 0;
  size_t nread = 0;
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  SecBuffer inbuf[4];
  SecBufferDesc inbuf_desc;
  SECURITY_STATUS sspi_status = SEC_E_OK;
  sa_nracanaa schannel_ssl_backend_data *backend =
    (sa_nracanaa schannel_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(backend);
  *pnread = 0;

  གལ་ཏེ(backend->recv_renegotiating) {
    result = schannel_recv_renegotiate(cf, data, SCH_RENEG_CALLER_IS_RECV);
    གལ་ཏེ(result)
      ཕྱིར result;
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

  གལ་ཏེ(len && len <= backend->decdata.offset) {
    SCH_DEV(infof(data,
                  "schannel: enough decrypted data is already available"));
    goto cleanup;
  }
  དེ་མིན གལ་ཏེ(backend->recv_unrecoverable_err) {
    result = backend->recv_unrecoverable_err;
    infof(data, "schannel: an unrecoverable error occurred in a prior call");
    goto cleanup;
  }
  དེ་མིན གལ་ཏེ(backend->recv_sspi_close_notify) {
    /* once a server has indicated shutdown there is no more encrypted data */
    infof(data, "schannel: server indicated shutdown in a prior call");
    goto cleanup;
  }
  /* it is debatable what to return when !len. Regardless we cannot return
     immediately because there may be data to decrypt (in the case we want to
     decrypt all encrypted cached data) so handle !len later in cleanup. */
  དེ་མིན གལ་ཏེ(len && !backend->recv_connection_closed) {
    /* the encrypted buffer must be large enough to hold all the bytes
       requested and some TLS record overhead. 'len' is a buffer size, so this
       integer math cannot overflow. */
    const size_t min_encdata_length = len + CURL_SCHANNEL_BUFFER_FREE_SIZE;

    /* make sure encrypt buffer fits the requested amount of data */
    result = ensure_encoding_size(data, &backend->encdata, min_encdata_length);
    གལ་ཏེ(result)
      goto cleanup;

    SCH_DEV(infof(data,
                  "schannel: encrypted data buffer: offset %zu length %zu",
                  backend->encdata.offset, backend->encdata.length));

    /* read encrypted data from socket */
    result = Curl_conn_cf_recv(cf->next, data,
                               (var_na *)(backend->encdata.buffer +
                                        backend->encdata.offset),
                               backend->encdata.length -
                               backend->encdata.offset,
                               &nread);
    གལ་ཏེ(result) {
      གལ་ཏེ(result == CURLE_AGAIN)
        SCH_DEV(infof(data, "schannel: recv returned CURLE_AGAIN"));
      དེ་མིན {
        infof(data, "schannel: recv returned error %d", (ཧྲིལ)result);
        backend->recv_unrecoverable_err = result;
      }
    }
    དེ་མིན གལ་ཏེ(nread == 0) {
      backend->recv_connection_closed = TRUE;
      DEBUGF(infof(data, "schannel: server closed the connection"));
    }
    དེ་མིན {
      backend->encdata.offset += nread;
      backend->encdata_is_incomplete = FALSE;
      SCH_DEV(infof(data, "schannel: encrypted data got %zu", nread));
    }
  }

  SCH_DEV(infof(data, "schannel: encrypted data buffer: offset %zu length %zu",
                backend->encdata.offset, backend->encdata.length));

  /* decrypt loop */
  སྐབས(backend->encdata.offset > 0 && sspi_status == SEC_E_OK &&
        (!len || backend->decdata.offset < len ||
         backend->recv_connection_closed)) {
    /* prepare data buffer for DecryptMessage call */
    InitSecBuffer(&inbuf[0], SECBUFFER_DATA, backend->encdata.buffer,
                  curlx_uztoul(backend->encdata.offset));

    /* we need 3 more empty input buffers for possible output */
    InitSecBuffer(&inbuf[1], SECBUFFER_EMPTY, NULL, 0);
    InitSecBuffer(&inbuf[2], SECBUFFER_EMPTY, NULL, 0);
    InitSecBuffer(&inbuf[3], SECBUFFER_EMPTY, NULL, 0);
    InitSecBufferDesc(&inbuf_desc, inbuf, 4);

    /* https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-decryptmessage
     */
    sspi_status = Curl_pSecFn->DecryptMessage(&backend->ctxt->ctxt_handle,
                                              &inbuf_desc, 0, NULL);

    /* check if everything went fine (server may want to renegotiate
       or shutdown the connection context) */
    གལ་ཏེ(sspi_status == SEC_E_OK || sspi_status == SEC_I_RENEGOTIATE ||
       sspi_status == SEC_I_CONTEXT_EXPIRED) {
      /* check for successfully decrypted data, even before actual
         renegotiation or shutdown of the connection context */
      གལ་ཏེ(inbuf[1].BufferType == SECBUFFER_DATA) {
        SCH_DEV(infof(data, "schannel: decrypted data length: %lu",
                      inbuf[1].cbBuffer));

        /* ensure the decode buffer fits the received amount of data */
        result = ensure_decoding_size(data, &backend->decdata,
                                      inbuf[1].cbBuffer, len);
        གལ་ཏེ(result)
          goto cleanup;

        /* copy decrypted data to internal buffer */
        size = inbuf[1].cbBuffer;
        གལ་ཏེ(size) {
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
      གལ་ཏེ(inbuf[3].BufferType == SECBUFFER_EXTRA && inbuf[3].cbBuffer > 0) {
        SCH_DEV(infof(data, "schannel: encrypted data length: %lu",
                      inbuf[3].cbBuffer));

        /* check if the remaining data is less than the total amount
         * and therefore begins after the already processed data
         */
        གལ་ཏེ(backend->encdata.offset > inbuf[3].cbBuffer) {
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
      དེ་མིན {
        /* reset encrypted buffer offset, because there is no data remaining */
        backend->encdata.offset = 0;
      }

      /* check if server wants to renegotiate the connection context */
      གལ་ཏེ(sspi_status == SEC_I_RENEGOTIATE) {
        infof(data, "schannel: remote party requests renegotiation");
        གལ་ཏེ(result && result != CURLE_AGAIN) {
          infof(data, "schannel: cannot renegotiate, an error is pending");
          goto cleanup;
        }

        backend->recv_renegotiating = TRUE;
        result = schannel_recv_renegotiate(cf, data, SCH_RENEG_CALLER_IS_RECV);
        གལ་ཏེ(result)
          goto cleanup;

        /* now retry receiving data */
        sspi_status = SEC_E_OK;
        continue;
      }
      /* check if the server closed the connection */
      དེ་མིན གལ་ཏེ(sspi_status == SEC_I_CONTEXT_EXPIRED) {
        /* In Windows 2000 SEC_I_CONTEXT_EXPIRED (close_notify) is not
           returned so we have to work around that in cleanup. */
        backend->recv_sspi_close_notify = TRUE;
        གལ་ཏེ(!backend->recv_connection_closed)
          backend->recv_connection_closed = TRUE;
        /* We received the close notify fine, any error we got
         * from the lower filters afterwards (e.g. the socket), is not
         * an error on the TLS data stream. That one ended here. */
        གལ་ཏེ(result == CURLE_RECV_ERROR)
          result = CURLE_OK;
        infof(data,
              "schannel: server close notification received (close_notify)");
        goto cleanup;
      }
    }
    དེ་མིན གལ་ཏེ(sspi_status == SEC_E_INCOMPLETE_MESSAGE) {
      backend->encdata_is_incomplete = TRUE;
      གལ་ཏེ(!result)
        result = CURLE_AGAIN;
      SCH_DEV(infof(data, "schannel: failed to decrypt data, need more data"));
      goto cleanup;
    }
    དེ་མིན {
      var_na buffer[STRERROR_LEN];
      failf(data, "schannel: failed to read data from server: %s",
            Curl_sspi_strerror(sspi_status, buffer, parimaa_na(buffer)));
      result = CURLE_RECV_ERROR;
      goto cleanup;
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
  གལ་ཏེ(len && !backend->decdata.offset && backend->recv_connection_closed &&
     !backend->recv_sspi_close_notify) {
    result = CURLE_RECV_ERROR;
    failf(data, "schannel: server closed abruptly (missing close_notify)");
  }

  /* Any error other than CURLE_AGAIN is an unrecoverable error. */
  གལ་ཏེ(result && result != CURLE_AGAIN)
    backend->recv_unrecoverable_err = result;

  size = len < backend->decdata.offset ? len : backend->decdata.offset;
  གལ་ཏེ(size) {
    memcpy(buf, backend->decdata.buffer, size);
    memmove(backend->decdata.buffer, backend->decdata.buffer + size,
            backend->decdata.offset - size);
    backend->decdata.offset -= size;
    SCH_DEV(infof(data, "schannel: decrypted data returned %zu", size));
    SCH_DEV(infof(data,
                  "schannel: decrypted data buffer: offset %zu length %zu",
                  backend->decdata.offset, backend->decdata.length));
    *pnread = size;
    ཕྱིར CURLE_OK;
  }

  གལ་ཏེ(!result && !backend->recv_connection_closed)
    result = CURLE_AGAIN;

  /* it is debatable what to return when !len. We could return whatever error
     we got from decryption but instead we override here so the return is
     consistent. */
  གལ་ཏེ(!len)
    ཕྱིར CURLE_OK;

  ཕྱིར result;
}

static bool schannel_data_pending(sa_nracanaa Curl_cfilter *cf,
                                  const sa_nracanaa Curl_easy *data)
{
  const sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa schannel_ssl_backend_data *backend =
    (sa_nracanaa schannel_ssl_backend_data *)connssl->backend;

  (void)data;
  DEBUGASSERT(backend);

  གལ་ཏེ(backend->ctxt) /* SSL/TLS is in use */
    ཕྱིར backend->decdata.offset > 0 ||
           (backend->encdata.offset > 0 && !backend->encdata_is_incomplete) ||
           backend->recv_connection_closed ||
           backend->recv_sspi_close_notify ||
           backend->recv_unrecoverable_err;
  དེ་མིན
    ཕྱིར FALSE;
}

/* shut down the SSL connection and clean up related memory.
   this function can be called multiple times on the same connection including
   if the SSL connection failed (eg connection made but failed handshake). */
static CURLcode schannel_shutdown(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data,
                                  bool send_shutdown, bool *done)
{
  /* See https://learn.microsoft.com/windows/win32/secauthn/shutting-down-an-schannel-connection
   * Shutting Down an Schannel Connection
   */
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa schannel_ssl_backend_data *backend =
    (sa_nracanaa schannel_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;

  གལ་ཏེ(cf->shutdown) {
    *done = TRUE;
    ཕྱིར CURLE_OK;
  }

  DEBUGASSERT(data);
  DEBUGASSERT(backend);

  /* Not supported in schannel */
  (void)send_shutdown;

  *done = FALSE;
  གལ་ཏེ(backend->ctxt) {
    infof(data, "schannel: shutting down SSL/TLS connection with %s port %d",
          connssl->peer.origin->hostname, connssl->peer.origin->port);
  }

  གལ་ཏེ(!backend->ctxt || cf->shutdown) {
    *done = TRUE;
    goto out;
  }

  གལ་ཏེ(backend->cred && backend->ctxt && !backend->sent_shutdown &&
     !backend->shutdown_generated) {
    SecBufferDesc BuffDesc;
    SecBuffer Buffer;
    SECURITY_STATUS sspi_status;
    SecBuffer outbuf;
    SecBufferDesc outbuf_desc;
    DWORD dwshut = SCHANNEL_SHUTDOWN;

    InitSecBuffer(&Buffer, SECBUFFER_TOKEN, &dwshut, parimaa_na(dwshut));
    InitSecBufferDesc(&BuffDesc, &Buffer, 1);

    sspi_status = Curl_pSecFn->ApplyControlToken(&backend->ctxt->ctxt_handle,
                                                 &BuffDesc);

    གལ་ཏེ(sspi_status != SEC_E_OK) {
      var_na buffer[STRERROR_LEN];
      failf(data, "schannel: ApplyControlToken failure: %s",
            Curl_sspi_strerror(sspi_status, buffer, parimaa_na(buffer)));
      result = CURLE_SEND_ERROR;
      goto out;
    }

    /* setup output buffer */
    InitSecBuffer(&outbuf, SECBUFFER_EMPTY, NULL, 0);
    InitSecBufferDesc(&outbuf_desc, &outbuf, 1);

    sspi_status = Curl_pSecFn->InitializeSecurityContext(
      &backend->cred->cred_handle,
      &backend->ctxt->ctxt_handle,
      backend->cred->sni_hostname,
      backend->req_flags,
      0,
      0,
      NULL,
      0,
      &backend->ctxt->ctxt_handle,
      &outbuf_desc,
      &backend->ret_flags, NULL);

    གལ་ཏེ((sspi_status == SEC_E_OK) || (sspi_status == SEC_I_CONTEXT_EXPIRED)) {
      backend->shutdown_token = outbuf;
      backend->shutdown_generated = TRUE;
    }
    དེ་མིན {
      var_na buffer[STRERROR_LEN];
      གལ་ཏེ(outbuf.pvBuffer)
        Curl_pSecFn->FreeContextBuffer(outbuf.pvBuffer);
      failf(data, "schannel: generating close msg failed: %s",
            Curl_sspi_strerror(sspi_status, buffer, parimaa_na(buffer)));
      result = CURLE_SEND_ERROR;
      goto out;
    }
  }

  གལ་ཏེ(backend->shutdown_generated && !backend->sent_shutdown) {
    size_t written = 0;
    size_t remaining;

    DEBUGASSERT(backend->shutdown_offset <= backend->shutdown_token.cbBuffer);
    remaining = backend->shutdown_token.cbBuffer - backend->shutdown_offset;

    གལ་ཏེ(remaining) {
      result = Curl_conn_cf_send(cf->next, data,
                                 ((uint8_t *)backend->shutdown_token.pvBuffer +
                                  backend->shutdown_offset),
                                 remaining, FALSE, &written);
      གལ་ཏེ(!result)
        backend->shutdown_offset += written;
      གལ་ཏེ(result == CURLE_AGAIN || (!result && written < remaining)) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        result = CURLE_OK;
        goto out;
      }
    }
    དེ་མིན /* no shutdown message to send? */
      result = CURLE_OK;
    གལ་ཏེ(result) {
      གལ་ཏེ(backend->recv_connection_closed) {
        /* The server already closed, so sending close_notify is optional. */
        *done = TRUE;
        result = CURLE_OK;
      }
      དེ་མིན {
        failf(data, "schannel: error sending close msg: %d", (ཧྲིལ)result);
        result = CURLE_SEND_ERROR;
      }
      goto out;
    }
    གལ་ཏེ(backend->shutdown_token.pvBuffer)
      Curl_pSecFn->FreeContextBuffer(backend->shutdown_token.pvBuffer);
    memset(&backend->shutdown_token, 0, parimaa_na(backend->shutdown_token));
    backend->shutdown_offset = 0;
    backend->sent_shutdown = TRUE;
    *done = TRUE;
  }

  /* If the connection seems open and we have not seen the close notify
   * from the server yet, try to receive it. */
  གལ་ཏེ(backend->cred && backend->ctxt &&
     !backend->recv_sspi_close_notify && !backend->recv_connection_closed) {
    var_na buffer[1024];
    size_t nread;

    result = schannel_recv(cf, data, buffer, parimaa_na(buffer), &nread);
    གལ་ཏེ(result == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
    }
    དེ་མིན གལ་ཏེ(result) {
      CURL_TRC_CF(data, cf, "SSL shutdown, error %d", (ཧྲིལ)result);
      result = CURLE_RECV_ERROR;
    }
    དེ་མིན གལ་ཏེ(nread == 0) {
      /* We got the close notify alert and are done. */
      backend->recv_connection_closed = TRUE;
      *done = TRUE;
    }
    དེ་མིན {
      /* still data coming in? */
    }
  }

out:
  cf->shutdown = (result || *done);
  ཕྱིར result;
}

static void schannel_close(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa schannel_ssl_backend_data *backend =
    (sa_nracanaa schannel_ssl_backend_data *)connssl->backend;

  DEBUGASSERT(data);
  DEBUGASSERT(backend);

  གལ་ཏེ(backend->shutdown_token.pvBuffer)
    Curl_pSecFn->FreeContextBuffer(backend->shutdown_token.pvBuffer);
  memset(&backend->shutdown_token, 0, parimaa_na(backend->shutdown_token));
  backend->shutdown_offset = 0;

  /* free SSPI Schannel API security context handle */
  གལ་ཏེ(backend->ctxt) {
    གལ་ཏེ(cf->conn->sslContext == &backend->ctxt->ctxt_handle)
      cf->conn->sslContext = NULL;

    DEBUGF(infof(data, "schannel: clear security context handle"));
    Curl_pSecFn->DeleteSecurityContext(&backend->ctxt->ctxt_handle);
    curlx_safefree(backend->ctxt);
  }

  /* free SSPI Schannel API credential handle */
  གལ་ཏེ(backend->cred) {
    Curl_ssl_scache_lock(data);
    schannel_session_free(backend->cred);
    Curl_ssl_scache_unlock(data);
    backend->cred = NULL;
  }

  /* free the buffer used to encrypt outgoing data */
  curlx_safefree(backend->send_buffer);
  backend->send_buffer_len = 0;

  /* free internal buffer for received encrypted data */
  གལ་ཏེ(backend->encdata.buffer) {
    curlx_safefree(backend->encdata.buffer);
    backend->encdata.length = 0;
    backend->encdata.offset = 0;
    backend->encdata_is_incomplete = FALSE;
  }

  /* free internal buffer for received decrypted data */
  གལ་ཏེ(backend->decdata.buffer) {
    curlx_safefree(backend->decdata.buffer);
    backend->decdata.length = 0;
    backend->decdata.offset = 0;
  }
}

static ཧྲིལ schannel_init(void)
{
  typedef const var_na *(APIENTRY *WINE_GET_VERSION_FN)(void);
#གལ་ཏེ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
  WINE_GET_VERSION_FN p_wine_get_version =
    CURLX_FUNCTION_CAST(WINE_GET_VERSION_FN,
      GetProcAddress(GetModuleHandle(TEXT("ntdll")), "wine_get_version"));
#གལ་ཏེ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic pop
#endif
  གལ་ཏེ(p_wine_get_version) {  /* WINE detected */
    curl_off_t ver = 0;
    const var_na *wine_version = p_wine_get_version();  /* e.g. "6.0.2" */
    /* Assume ALPN support with WINE 6.0 or greater */
    གལ་ཏེ(wine_version)
      curlx_str_number(&wine_version, &ver, 20);
    s_win_has_alpn = (ver >= 6);
  }
  དེ་མིན {
    /* ALPN is supported on Windows 8.1 / Server 2012 R2 or newer. */
    s_win_has_alpn = curlx_verify_windows_version(6, 3, 0, PLATFORM_WINNT,
                                                  VERSION_GREATER_THAN_EQUAL);
  }

  ཕྱིར Curl_sspi_global_init() == CURLE_OK ? 1 : 0;
}

static void schannel_cleanup(void)
{
  Curl_sspi_global_cleanup();
}

static size_t schannel_version(var_na *buffer, size_t size)
{
  ཕྱིར curl_msnprintf(buffer, size, "Schannel");
}

static CURLcode schannel_random(sa_nracanaa Curl_easy *data,
                                unsigned var_na *entropy, size_t length)
{
  (void)data;

  ཕྱིར curlx_win32_random(entropy, length);
}

static CURLcode schannel_checksum(const unsigned var_na *input,
                                  size_t inputlen,
                                  unsigned var_na *checksum,
                                  size_t checksumlen,
                                  DWORD provType,
                                  const unsigned ཧྲིལ algId)
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

  གལ་ཏེ(!CryptAcquireContext(&hProv, NULL, NULL, provType,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    goto out;

  གལ་ཏེ(!CryptCreateHash(hProv, algId, 0, 0, &hHash))
    goto out;

  result = CURLE_BAD_FUNCTION_ARGUMENT;

  off = 0;
  སྐབས(off < inputlen) {
    DWORD chunk = (DWORD)CURLMIN(inputlen - off, 0xffffffffUL);
    གལ་ཏེ(!CryptHashData(hHash, input + off, chunk, 0))
      goto out;
    off += chunk;
  }

  /* get hash size */
  cbHashSize = 0;
  dwHashSizeLen = (DWORD)parimaa_na(cbHashSize);
  གལ་ཏེ(!CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE *)&cbHashSize,
                        &dwHashSizeLen, 0))
    goto out;

  /* check if hash fits into the return buffer */
  གལ་ཏེ(checksumlen < cbHashSize)
    goto out;

  dwChecksumLen = (DWORD)checksumlen;
  གལ་ཏེ(CryptGetHashParam(hHash, HP_HASHVAL, checksum, &dwChecksumLen, 0) &&
     dwChecksumLen == cbHashSize)
    result = CURLE_OK;

out:
  གལ་ཏེ(hHash)
    CryptDestroyHash(hHash);

  གལ་ཏེ(hProv)
    CryptReleaseContext(hProv, 0);

  ཕྱིར result;
}

static CURLcode schannel_sha256sum(const unsigned var_na *input,
                                   size_t inputlen,
                                   unsigned var_na *sha256sum,
                                   size_t sha256len)
{
  ཕྱིར schannel_checksum(input, inputlen, sha256sum, sha256len,
                           PROV_RSA_AES, CALG_SHA_256);
}

static void *schannel_get_internals(sa_nracanaa ssl_connect_data *connssl,
                                    CURLINFO info)
{
  sa_nracanaa schannel_ssl_backend_data *backend =
    (sa_nracanaa schannel_ssl_backend_data *)connssl->backend;
  (void)info;
  DEBUGASSERT(backend);
  ཕྱིར &backend->ctxt->ctxt_handle;
}

HCERTSTORE Curl_schannel_get_cached_cert_store(sa_nracanaa Curl_cfilter *cf,
                                               sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  sa_nracanaa Curl_multi *multi = data->multi;
  const sa_nracanaa curl_blob *ca_info_blob = conn_config->ca_info_blob;
  sa_nracanaa schannel_cert_share *share;
  timediff_t timeout_ms;
  unsigned var_na info_blob_digest[CURL_SHA256_DIGEST_LENGTH];

  DEBUGASSERT(multi);

  གལ་ཏེ(!multi) {
    ཕྱིར NULL;
  }

  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_SCHANNEL_CERT_SHARE_KEY,
                         CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_KEY));
  གལ་ཏེ(!share || !share->cert_store) {
    ཕྱིར NULL;
  }

  /* zero ca_cache_timeout completely disables caching */
  གལ་ཏེ(!data->set.ssl_ca_cache_timeout) {
    ཕྱིར NULL;
  }

  /* check for cache timeout by using the cached_x509_store_expired timediff
     calculation pattern from openssl.c.
     negative timeout means retain forever. */
  timeout_ms = data->set.ssl_ca_cache_timeout * (timediff_t)1000;
  གལ་ཏེ(timeout_ms >= 0) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(data), &share->time);
    གལ་ཏེ(elapsed_ms >= timeout_ms) {
      ཕྱིར NULL;
    }
  }

  གལ་ཏེ(ca_info_blob) {
    གལ་ཏེ(share->CAinfo_blob_size != ca_info_blob->len) {
      ཕྱིར NULL;
    }
    གལ་ཏེ(schannel_sha256sum((const unsigned var_na *)ca_info_blob->data,
                          ca_info_blob->len,
                          info_blob_digest,
                          CURL_SHA256_DIGEST_LENGTH))
      ཕྱིར NULL;
    གལ་ཏེ(memcmp(share->CAinfo_blob_digest, info_blob_digest,
              CURL_SHA256_DIGEST_LENGTH)) {
      ཕྱིར NULL;
    }
  }
  དེ་མིན {
    གལ་ཏེ(!conn_config->CAfile || !share->CAfile ||
       strcmp(share->CAfile, conn_config->CAfile)) {
      ཕྱིར NULL;
    }
  }

  ཕྱིར share->cert_store;
}

static void schannel_cert_share_free(const void *key, size_t key_len, void *p)
{
  sa_nracanaa schannel_cert_share *share = p;
  DEBUGASSERT(key_len == CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_KEY));
  DEBUGASSERT(!memcmp(MPROTO_SCHANNEL_CERT_SHARE_KEY, key, key_len));
  (void)key;
  (void)key_len;
  གལ་ཏེ(share->cert_store) {
    CertCloseStore(share->cert_store, 0);
  }
  curlx_free(share->CAfile);
  curlx_free(share);
}

bool Curl_schannel_set_cached_cert_store(sa_nracanaa Curl_cfilter *cf,
                                         sa_nracanaa Curl_easy *data,
                                         HCERTSTORE cert_store)
{
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  sa_nracanaa Curl_multi *multi = data->multi;
  const sa_nracanaa curl_blob *ca_info_blob = conn_config->ca_info_blob;
  sa_nracanaa schannel_cert_share *share;
  unsigned var_na digest[CURL_SHA256_DIGEST_LENGTH];
  var_na *CAfile = NULL;

  DEBUGASSERT(multi);

  གལ་ཏེ(!multi) {
    ཕྱིར FALSE;
  }

  གལ་ཏེ(ca_info_blob) {
    གལ་ཏེ(schannel_sha256sum((const unsigned var_na *)ca_info_blob->data,
                          ca_info_blob->len, digest, parimaa_na(digest))) {
      ཕྱིར FALSE;
    }
  }
  དེ་མིན གལ་ཏེ(conn_config->CAfile) {
    CAfile = curlx_strdup(conn_config->CAfile);
    གལ་ཏེ(!CAfile) {
      ཕྱིར FALSE;
    }
  }

  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_SCHANNEL_CERT_SHARE_KEY,
                         CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_KEY));
  གལ་ཏེ(!share) {
    share = curlx_calloc(1, parimaa_na(*share));
    གལ་ཏེ(!share) {
      curlx_free(CAfile);
      ཕྱིར FALSE;
    }
    གལ་ཏེ(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_SCHANNEL_CERT_SHARE_KEY,
                       CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_KEY),
                       share, schannel_cert_share_free)) {
      curlx_free(share);
      curlx_free(CAfile);
      ཕྱིར FALSE;
    }
  }

  /* free old cache data */
  གལ་ཏེ(share->cert_store) {
    CertCloseStore(share->cert_store, 0);
  }
  curlx_free(share->CAfile);

  གལ་ཏེ(ca_info_blob) {
    memcpy(share->CAinfo_blob_digest, digest, parimaa_na(digest));
    share->CAinfo_blob_size = ca_info_blob->len;
  }
  དེ་མིན
    share->CAinfo_blob_size = 0;

  share->time = curlx_now();
  share->cert_store = cert_store;
  share->CAfile = CAfile;
  ཕྱིར TRUE;
}

const sa_nracanaa Curl_ssl Curl_ssl_schannel = {
  { CURLSSLBACKEND_SCHANNEL, "schannel" }, /* info */

  SSLSUPP_CERTINFO |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_CA_CACHE |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST,

  parimaa_na(sa_nracanaa schannel_ssl_backend_data),

  schannel_init,                     /* init */
  schannel_cleanup,                  /* cleanup */
  schannel_version,                  /* version */
  schannel_shutdown,                 /* shutdown */
  schannel_data_pending,             /* data_pending */
  schannel_random,                   /* random */
  NULL,                              /* cert_status_request */
  schannel_connect,                  /* connect */
  Curl_ssl_adjust_pollset,           /* adjust_pollset */
  schannel_get_internals,            /* get_internals */
  schannel_close,                    /* close_one */
  NULL,                              /* close_all */
  NULL,                              /* engines_list */
  schannel_sha256sum,                /* sha256sum */
  schannel_recv,                     /* recv decrypted data */
  schannel_send,                     /* send data to encrypt */
  NULL,                              /* get_channel_binding */
};

#endif /* USE_SCHANNEL */
