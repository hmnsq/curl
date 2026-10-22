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
  করো {                                                          \
    infof(তথ্য, "schannel: " #x " %s", (x) ? "TRUE" : "FALSE"); \
  } যখন(0)
#অন্যথা
#define SCH_DEV(x) করো {} যখন(0)
#define SCH_DEV_SHOWBOOL(x) করো {} যখন(0)
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
#define CURL_SCHANNEL_বাফার_INIT_আকার   4096
#define CURL_SCHANNEL_বাফার_FREE_আকার   1024

#define CERT_THUMBPRINT_STR_LEN 40
#define CERT_THUMBPRINT_তথ্য_LEN 20

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

প্রকারলেখ গণনা {
  SecApplicationProtocolNegotiationExt_None,
  SecApplicationProtocolNegotiationExt_NPN,
  SecApplicationProtocolNegotiationExt_ALPN
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT;

প্রকারলেখ গণনা {
  SecApplicationProtocolNegotiationStatus_None,
  SecApplicationProtocolNegotiationStatus_Success,
  SecApplicationProtocolNegotiationStatus_SelectedClientOnly
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_STATUS;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
প্রকারলেখ কাঠামো {
  SEC_APPLICATION_PROTOCOL_NEGOTIATION_STATUS ProtoNegoStatus;
  SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT ProtoNegoExt;
  unsigned অক্ষর Protocol_Id_আকার;
  unsigned অক্ষর ProtocolId[0xff];
} SecPkgContext_ApplicationProtocol;
#endif

স্থির শূন্য Init_Sec_বাফার(Sec_বাফার *বাফার, unsigned long BufType,
                          শূন্য *Buf_তথ্য_Ptr, unsigned long Buf_Byte_আকার)
{
  বাফার->cb_বাফার = Buf_Byte_আকার;
  বাফার->বাফার_Type = BufType;
  বাফার->pv_বাফার = Buf_তথ্য_Ptr;
}

স্থির শূন্য Init_Sec_বাফার_Desc(Sec_বাফার_Desc *desc, Sec_বাফার *BufArr,
                              unsigned long অঙ্ক_Arr_Elem)
{
  desc->ulVersion = SECBUFFER_VERSION;
  desc->pBuffers = BufArr;
  desc->cBuffers = অঙ্ক_Arr_Elem;
}

স্থির CURLcode schannel_set_ssl_version_min_max(DWORD *enabled_protocols,
                                                 কাঠামো Curl_cfilter *cf,
                                                 কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  long ssl_version = conn_config->version;
  long ssl_version_max = (long)conn_config->version_max;
  long i = ssl_version;

  নির্বাচন(ssl_version_max) {
  ক্ষেত্র CURL_SSLVERSION_MAX_NONE:
  ক্ষেত্র CURL_SSLVERSION_MAX_DEFAULT:

    /* Windows Server 2022 and newer (including Windows 11) support TLS 1.3
       built-in. Previous builds of Windows 10 had broken TLS 1.3
       implementations that could be enabled via registry. */
    যদি(curlx_verify_windows_version(10, 0, 20348, PLATFORM_WINNT,
                                    VERSION_GREATER_THAN_EQUAL)) {
      ssl_version_max = CURL_SSLVERSION_MAX_TLSv1_3;
    }
    অন্যথা /* Windows 10 or older */
      ssl_version_max = CURL_SSLVERSION_MAX_TLSv1_2;

    বিরতি;
  }

  জন্য(; i <= (ssl_version_max >> 16); ++i) {
    নির্বাচন(i) {
    ক্ষেত্র CURL_SSLVERSION_TLSv1_0:
      *enabled_protocols |= SP_PROT_TLS1_0_CLIENT;
      বিরতি;
    ক্ষেত্র CURL_SSLVERSION_TLSv1_1:
      *enabled_protocols |= SP_PROT_TLS1_1_CLIENT;
      বিরতি;
    ক্ষেত্র CURL_SSLVERSION_TLSv1_2:
      *enabled_protocols |= SP_PROT_TLS1_2_CLIENT;
      বিরতি;
    ক্ষেত্র CURL_SSLVERSION_TLSv1_3:

      /* Windows Server 2022 or newer */
      যদি(curlx_verify_windows_version(10, 0, 20348, PLATFORM_WINNT,
                                      VERSION_GREATER_THAN_EQUAL)) {
        *enabled_protocols |= SP_PROT_TLS1_3_CLIENT;
        বিরতি;
      }
      অন্যথা { /* Windows 10 or older */
        failf(তথ্য, "schannel: TLS 1.3 not supported on Windows 10 or older");
        ফেরত CURLE_SSL_CONNECT_ERROR;
      }
    }
  }
  ফেরত CURLE_OK;
}

#define CIPHEROPTION(x) { #x, x }

কাঠামো algo {
  ধ্রুবক অক্ষর *name;
  পূর্ণসংখ্যা id;
};

স্থির ধ্রুবক কাঠামো algo algs[] = {
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
  { ফাঁকা, 0 },
};

স্থির পূর্ণসংখ্যা get_alg_id_by_name(ধ্রুবক অক্ষর *name)
{
  ধ্রুবক অক্ষর *nameEnd = strchr(name, ':');
  size_t n = nameEnd ? (size_t)(nameEnd - name) : strlen(name);
  পূর্ণসংখ্যা i;

  জন্য(i = 0; algs[i].name; i++) {
    যদি((n == strlen(algs[i].name) && !strncmp(algs[i].name, name, n)))
      ফেরত algs[i].id;
  }
  ফেরত 0; /* not found */
}

#define অঙ্ক_CIPHERS 47 /* There are a maximum of 47 options listed above */

স্থির CURLcode set_ssl_ciphers(SCHANNEL_CRED *schannel_cred, অক্ষর *ciphers,
                                ALG_ID *algIds)
{
  ধ্রুবক অক্ষর *startCur = ciphers;
  পূর্ণসংখ্যা alg_গুনতি = 0;
  যখন(startCur && *startCur && (alg_গুনতি < অঙ্ক_CIPHERS)) {
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
      ফেরত CURLE_SSL_CIPHER;
    startCur = strchr(startCur, ':');
    যদি(startCur)
      startCur++;
  }
  schannel_cred->palgSupportedAlgs = algIds;
  schannel_cred->cSupportedAlgs = (DWORD)alg_গুনতি;
  ফেরত CURLE_OK;
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
    ফেরত CURLE_SSL_CERTPROBLEM;

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
    ফেরত CURLE_SSL_CERTPROBLEM;

  store_path_start = sep + 1;

  sep = _tcschr(store_path_start, _TEXT('\\'));
  যদি(!sep)
    ফেরত CURLE_SSL_CERTPROBLEM;

  *thumbprint = sep + 1;
  যদি(_tcslen(*thumbprint) != CERT_THUMBPRINT_STR_LEN)
    ফেরত CURLE_SSL_CERTPROBLEM;

  *sep = _TEXT('\0');
  *store_path = curlx_tcsdup(store_path_start);
  *sep = _TEXT('\\');
  যদি(!*store_path)
    ফেরত CURLE_OUT_OF_MEMORY;

  ফেরত CURLE_OK;
}

স্থির CURLcode get_client_cert(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য,
                                HCERTSTORE *out_cert_store,
                                PCCERT_CONTEXT *out_cert_context)
{
  কাঠামো ssl_filter_config *sslc = Curl_ssl_cf_get_filter_config(cf);
  PCCERT_CONTEXT client_cert = ফাঁকা;
  HCERTSTORE client_cert_store = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;

  যদি(sslc->clientcert || sslc->cert_blob) {
    DWORD cert_store_name = 0;
    TCHAR *cert_store_path = ফাঁকা;
    TCHAR *cert_thumbprint_str = ফাঁকা;
    TCHAR cert_thumbprint_buf[CERT_THUMBPRINT_STR_LEN + 1];
    CRYPT_HASH_BLOB cert_thumbprint;
    BYTE cert_thumbprint_তথ্য[CERT_THUMBPRINT_তথ্য_LEN];
    HCERTSTORE cert_store = ফাঁকা;
    FILE *fInCert = ফাঁকা;
    শূন্য *certdata = ফাঁকা;
    size_t certsize = 0;
    bool blob = !!sslc->cert_blob;

    যদি(blob) {
      certdata = sslc->cert_blob->তথ্য;
      certsize = sslc->cert_blob->len;
    }
    অন্যথা {
      TCHAR *cert_path =
        curlx_convert_UTF8_to_tchar(sslc->clientcert);
      যদি(!cert_path)
        ফেরত CURLE_OUT_OF_MEMORY;

      ফলাফল = get_cert_location(cert_path, &cert_store_name,
                                 &cert_store_path, &cert_thumbprint_str);

      /* 'cert_thumbprint_str' points in to the allocated 'cert_path', copy
         the data. The string is verified to be CERT_THUMBPRINT_STR_LEN bytes
         long within the get_cert_location() function. */
      যদি(!ফলাফল && cert_thumbprint_str) {
        memcpy(cert_thumbprint_buf, cert_thumbprint_str,
               আকার(cert_thumbprint_buf));
        cert_thumbprint_str = cert_thumbprint_buf;
      }

      curlx_free(cert_path);
      যদি(ফলাফল && (sslc->clientcert[0] != '\0'))
        fInCert = curlx_fopen(sslc->clientcert, "rb");

      যদি(ফলাফল && !fInCert) {
        failf(তথ্য, "schannel: Failed to get certificate location"
              " or file for %s",
              sslc->clientcert);
        ফেরত ফলাফল;
      }
    }

    যদি((fInCert || blob) && sslc->cert_type &&
       !curl_strequal(sslc->cert_type, "P12")) {
      failf(তথ্য, "schannel: certificate format compatibility error "
            "for %s",
            blob ? "(memory blob)" : sslc->clientcert);
      curlx_free(cert_store_path);
      যদি(fInCert)
        curlx_fclose(fInCert);
      ফেরত CURLE_SSL_CERTPROBLEM;
    }

    যদি(fInCert || blob) {
      /* Reading a .p12 or .pfx file, like the example at bottom of
         https://learn.microsoft.com/archive/msdn-technet-forums/3e7bc95f-b21a-4bcd-bd2c-7f996718cae5
       */
      CRYPT_তথ্য_BLOB datablob;
      WCHAR *pszPassword;
      size_t pwd_len = 0;
      পূর্ণসংখ্যা cert_find_flags;
      ধ্রুবক অক্ষর *cert_showfilename_error = blob ?
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
        যদি(continue_reading && (certsize < CURL_MAX_প্রবেশ_দৈর্ঘ্য))
          certdata = curlx_malloc(certsize + 1);
        যদি(!certdata || ((পূর্ণসংখ্যা)fread(certdata, certsize, 1, fInCert) != 1))
          continue_reading = FALSE;
        curlx_fclose(fInCert);
        যদি(!continue_reading) {
          failf(তথ্য, "schannel: Failed to read cert file %s",
                sslc->clientcert);
          curlx_free(certdata);
          ফেরত CURLE_SSL_CERTPROBLEM;
        }
      }

      /* Convert key-pair data to the in-memory certificate store */
      datablob.pb_তথ্য = (BYTE *)certdata;
      datablob.cb_তথ্য = (DWORD)certsize;

      যদি(sslc->চাবি_passwd)
        pwd_len = strlen(sslc->চাবি_passwd);
      pszPassword = curlx_malloc(আকার(WCHAR) * (pwd_len + 1));
      যদি(pszPassword) {
        পূর্ণসংখ্যা str_w_len = 0;
        যদি(pwd_len > 0)
          str_w_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                          sslc->চাবি_passwd, (পূর্ণসংখ্যা)pwd_len,
                                          pszPassword, (পূর্ণসংখ্যা)(pwd_len + 1));

        যদি((str_w_len >= 0) && (str_w_len <= (পূর্ণসংখ্যা)pwd_len))
          pszPassword[str_w_len] = 0;
        অন্যথা
          pszPassword[0] = 0;

        cert_store = PFXImportCertStore(&datablob, pszPassword,
                                        PKCS12_NO_PERSIST_চাবি);
        curlx_memzero(pszPassword, আকার(WCHAR) * (pwd_len + 1));
        curlx_free(pszPassword);
      }
      যদি(!blob)
        curlx_free(certdata);
      যদি(!cert_store) {
        DWORD errorcode = GetLastError();
        যদি(errorcode == ERROR_INVALID_PASSWORD)
          failf(তথ্য, "schannel: Failed to import cert file %s, "
                "password is bad",
                cert_showfilename_error);
        অন্যথা
          failf(তথ্য, "schannel: Failed to import cert file %s, "
                "last error is 0x%08lx",
                cert_showfilename_error, errorcode);
        ফেরত CURLE_SSL_CERTPROBLEM;
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
                                   cert_find_flags, ফাঁকা, ফাঁকা);

      যদি(!client_cert) {
        failf(তথ্য, "schannel: Failed to get certificate from file %s"
              ", last error is 0x%08lx",
              cert_showfilename_error, GetLastError());
        CertCloseStore(cert_store, 0);
        ফেরত CURLE_SSL_CERTPROBLEM;
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
                      (HCRYPTPROV)ফাঁকা,
                      CERT_STORE_OPEN_EXISTING_FLAG | cert_store_name,
                      cert_store_path);
      যদি(!cert_store) {
        অক্ষর *path_utf8 =
          curlx_convert_tchar_to_UTF8(cert_store_path);
        failf(তথ্য, "schannel: Failed to open cert store %lx %s, "
              "last error is 0x%08lx",
              cert_store_name, (path_utf8 ? path_utf8 : "(unknown)"),
              GetLastError());
        curlx_free(cert_store_path);
        curlx_free(path_utf8);
        ফেরত CURLE_SSL_CERTPROBLEM;
      }
      curlx_free(cert_store_path);

      cert_thumbprint.pb_তথ্য = cert_thumbprint_তথ্য;
      cert_thumbprint.cb_তথ্য = CERT_THUMBPRINT_তথ্য_LEN;

      যদি(!Crypt_String_To_দ্বিমিক(cert_thumbprint_str,
                              CERT_THUMBPRINT_STR_LEN,
                              CRYPT_STRING_HEX,
                              cert_thumbprint_তথ্য,
                              &cert_thumbprint.cb_তথ্য,
                              ফাঁকা, ফাঁকা)) {
        CertCloseStore(cert_store, 0);
        ফেরত CURLE_SSL_CERTPROBLEM;
      }

      client_cert =
        CertFindCertificateInStore(cert_store,
                                   X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                   CERT_FIND_HASH, &cert_thumbprint, ফাঁকা);
      যদি(!client_cert) {
        /* CRYPT_E_NOT_FOUND / E_INVALIDARG */
        CertCloseStore(cert_store, 0);
        failf(তথ্য, "schannel: client cert not found in cert store");
        ফেরত CURLE_SSL_CERTPROBLEM;
      }
    }
    client_cert_store = cert_store;
  }

  *out_cert_store = client_cert_store;
  *out_cert_context = client_cert;

  ফেরত CURLE_OK;
}

স্থির CURLcode acquire_sspi_handle(কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য,
                                    কাঠামো schannel_ssl_backend_তথ্য *backend,
                                    PCCERT_CONTEXT client_cert,
                                    DWORD flags,
                                    DWORD enabled_protocols)
{
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CURLcode ফলাফল;

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
      client_certs[0] = ফাঁকা;

    memset(crypto_settings, 0, আকার(crypto_settings));

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
      Curl_pSecFn->AcquireCredentialsHandle(ফাঁকা, CURL_UNCONST(UNISP_NAME),
                                            SECPKG_CRED_OUTBOUND, ফাঁকা,
                                            &credentials, ফাঁকা, ফাঁকা,
                                            &backend->cred->cred_handle, ফাঁকা);
  }
  অন্যথা {
    /* Pre-Windows 10 1809 or the user set a legacy algorithm list.
       Schannel does not negotiate TLS 1.3 when SCHANNEL_CRED is used. */
    ALG_ID algIds[অঙ্ক_CIPHERS];
    অক্ষর *ciphers = conn_config->cipher_list;
    SCHANNEL_CRED schannel_cred = { 0 };
    PCCERT_CONTEXT client_certs[1];

    যদি(client_cert)
      client_certs[0] = client_cert;
    অন্যথা
      client_certs[0] = ফাঁকা;

    schannel_cred.dwVersion = SCHANNEL_CRED_VERSION;
    schannel_cred.dwFlags = flags;
    schannel_cred.grbitEnabledProtocols = enabled_protocols;

    যদি(ciphers) {
      যদি((enabled_protocols & SP_PROT_TLS1_3_CLIENT)) {
        যদি(!(enabled_protocols & ~SP_PROT_TLS1_3_CLIENT)) {
          failf(তথ্য, "schannel: TLS 1.3 is not supported with a cipher list; "
                "remove the cipher list or allow a lower TLS version");
          ফেরত CURLE_SSL_CONNECT_ERROR;
        }
        infof(তথ্য, "schannel: WARNING: This version of Schannel "
              "negotiates a less-secure TLS version than TLS 1.3 because the "
              "user set an algorithm cipher list.");
      }
      ফলাফল = set_ssl_ciphers(&schannel_cred, ciphers, algIds);
      যদি(ফলাফল) {
        failf(তথ্য, "schannel: Failed setting algorithm cipher list");
        ফেরত ফলাফল;
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
      Curl_pSecFn->AcquireCredentialsHandle(ফাঁকা, CURL_UNCONST(UNISP_NAME),
                                            SECPKG_CRED_OUTBOUND, ফাঁকা,
                                            &schannel_cred, ফাঁকা, ফাঁকা,
                                            &backend->cred->cred_handle, ফাঁকা);
  }

  যদি(sspi_status != SEC_E_OK) {
    অক্ষর বাফার[STRERROR_LEN];
    failf(তথ্য, "schannel: AcquireCredentialsHandle failed: %s",
          Curl_sspi_strerror(sspi_status, বাফার, আকার(বাফার)));
    নির্বাচন(sspi_status) {
    ক্ষেত্র SEC_E_INSUFFICIENT_MEMORY:
      ফেরত CURLE_OUT_OF_MEMORY;
    ক্ষেত্র SEC_E_NO_CREDENTIALS:
    ক্ষেত্র SEC_E_SECPKG_NOT_FOUND:
    ক্ষেত্র SEC_E_NOT_OWNER:
    ক্ষেত্র SEC_E_UNKNOWN_CREDENTIALS:
    ক্ষেত্র SEC_E_INTERNAL_ERROR:
    পূর্বনির্ধারিত:
      ফেরত CURLE_SSL_CONNECT_ERROR;
    }
  }

  ফেরত CURLE_OK;
}

স্থির CURLcode schannel_acquire_credential_handle(কাঠামো Curl_cfilter *cf,
                                                   কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  PCCERT_CONTEXT client_cert = ফাঁকা;
  HCERTSTORE client_cert_store = ফাঁকা;
  CURLcode ফলাফল;

  /* setup Schannel API options */
  DWORD flags = 0;
  DWORD enabled_protocols = 0;

  কাঠামো schannel_ssl_backend_তথ্য *backend =
    (কাঠামো schannel_ssl_backend_তথ্য *)connssl->backend;

  DEBUGASSERT(backend);

  যদি(conn_config->verifypeer) {
    যদি(backend->use_manual_cred_validation)
      flags = SCH_CRED_MANUAL_CRED_VALIDATION;
    অন্যথা
      flags = SCH_CRED_AUTO_CRED_VALIDATION;

    যদি(conn_config->no_revoke) {
      flags |= SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
               SCH_CRED_IGNORE_REVOCATION_OFFLINE;

      DEBUGF(infof(তথ্য, "schannel: disabled server certificate revocation "
                         "checks"));
    }
    অন্যথা যদি(conn_config->revoke_best_effort) {
      flags |= SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
               SCH_CRED_IGNORE_REVOCATION_OFFLINE |
               SCH_CRED_REVOCATION_CHECK_CHAIN;

      DEBUGF(infof(তথ্য, "schannel: ignore revocation offline errors"));
    }
    অন্যথা {
      flags |= SCH_CRED_REVOCATION_CHECK_CHAIN;

      DEBUGF(infof(তথ্য, "schannel: checking server certificate revocation"));
    }
  }
  অন্যথা {
    flags = SCH_CRED_MANUAL_CRED_VALIDATION |
            SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
            SCH_CRED_IGNORE_REVOCATION_OFFLINE;
    DEBUGF(infof(তথ্য, "schannel: disabled server cert revocation checks"));
  }

  যদি(!conn_config->verifyhost) {
    flags |= SCH_CRED_NO_SERVERNAME_CHECK;
    DEBUGF(infof(তথ্য, "schannel: verifyhost setting prevents Schannel from "
                       "comparing the supplied target name with the subject "
                       "names in server certificates."));
  }

  যদি(!conn_config->auto_client_cert) {
    flags &= ~(DWORD)SCH_CRED_USE_DEFAULT_CREDS;
    flags |= SCH_CRED_NO_DEFAULT_CREDS;
    infof(তথ্য, "schannel: disabled automatic use of client certificate");
  }
  অন্যথা
    infof(তথ্য, "schannel: enabled automatic use of client certificate");

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  নির্বাচন(conn_config->version) {
  ক্ষেত্র CURL_SSLVERSION_TLSv1:
  ক্ষেত্র CURL_SSLVERSION_TLSv1_0:
  ক্ষেত্র CURL_SSLVERSION_TLSv1_1:
  ক্ষেত্র CURL_SSLVERSION_TLSv1_2:
  ক্ষেত্র CURL_SSLVERSION_TLSv1_3: {
    ফলাফল = schannel_set_ssl_version_min_max(&enabled_protocols, cf, তথ্য);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    বিরতি;
  }
  ক্ষেত্র CURL_SSLVERSION_SSLv3:
  ক্ষেত্র CURL_SSLVERSION_SSLv2:
    failf(তথ্য, "SSL versions not supported");
    ফেরত CURLE_NOT_BUILT_IN;
  পূর্বনির্ধারিত:
    failf(তথ্য, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    ফেরত CURLE_SSL_CONNECT_ERROR;
  }

  ফলাফল = get_client_cert(cf, তথ্য, &client_cert_store, &client_cert);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  /* allocate memory for the reusable credential handle */
  backend->cred = (কাঠামো Curl_schannel_cred *)
    curlx_calloc(1, আকার(কাঠামো Curl_schannel_cred));
  যদি(!backend->cred) {
    failf(তথ্য, "schannel: unable to allocate memory");

    যদি(client_cert)
      CertFreeCertificateContext(client_cert);
    যদি(client_cert_store)
      CertCloseStore(client_cert_store, 0);

    ফেরত CURLE_OUT_OF_MEMORY;
  }
  backend->cred->refcount = 1;

  /* Since we did not persist the key, we need to extend the store's
   * lifetime until the end of the connection
   */
  backend->cred->client_cert_store = client_cert_store;

  ফলাফল = acquire_sspi_handle(cf, তথ্য, backend, client_cert,
                               flags, enabled_protocols);

  যদি(client_cert)
    CertFreeCertificateContext(client_cert);

  ফেরত ফলাফল;
}

স্থির CURLcode schannel_connect_step1(কাঠামো Curl_cfilter *cf,
                                       কাঠামো Curl_easy *তথ্য)
{
  size_t written = 0;
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো schannel_ssl_backend_তথ্য *backend =
    (কাঠামো schannel_ssl_backend_তথ্য *)connssl->backend;
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  Sec_বাফার outbuf;
  Sec_বাফার_Desc outbuf_desc;
  Sec_বাফার inbuf;
  Sec_বাফার_Desc inbuf_desc;
  unsigned অক্ষর alpn_বাফার[128];
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CURLcode ফলাফল;

  DEBUGASSERT(backend);
  DEBUGF(infof(তথ্য, "schannel: SSL/TLS connection with %s port %d (step 1/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->port));

  backend->use_alpn = connssl->alpn && s_win_has_alpn;

  যদি(conn_config->CAfile || conn_config->ca_info_blob) {
    যদি(curlx_verify_windows_version(6, 1, 0, PLATFORM_WINNT,
                                    VERSION_GREATER_THAN_EQUAL)) {
      backend->use_manual_cred_validation = TRUE;
    }
    অন্যথা {
      failf(তথ্য, "schannel: this version of Windows is too old to support "
                  "certificate verification via CA bundle file.");
      ফেরত CURLE_SSL_CACERT_BADFILE;
    }
  }
  অন্যথা
    backend->use_manual_cred_validation = FALSE;

  backend->cred = ফাঁকা;

  /* check for an existing reusable credential handle */
  যদি(Curl_ssl_scache_use(cf, তথ্য)) {
    কাঠামো Curl_schannel_cred *old_cred;
    Curl_ssl_scache_lock(তথ্য);
    old_cred = Curl_ssl_scache_get_obj(cf, তথ্য, connssl->peer.scache_চাবি);
    যদি(old_cred) {
      backend->cred = old_cred;
      DEBUGF(infof(তথ্য, "schannel: reusing existing credential handle"));

      /* increment the reference counter of the credential/session handle */
      backend->cred->refcount++;
      DEBUGF(infof(তথ্য,
                   "schannel: incremented credential handle refcount = %d",
                   backend->cred->refcount));
    }
    Curl_ssl_scache_unlock(তথ্য);
  }

  যদি(!backend->cred) {
    অক্ষর *snihost;
    ফলাফল = schannel_acquire_credential_handle(cf, তথ্য);
    যদি(ফলাফল || !backend->cred)
      ফেরত ফলাফল;
    /* schannel_acquire_credential_handle() sets backend->cred accordingly or
       it returns error otherwise. */

    /* A hostname associated with the credential is needed by
       InitializeSecurityContext for SNI and other reasons. */
    snihost = connssl->peer.sni ?
      connssl->peer.sni : connssl->peer.origin->hostname;
    backend->cred->sni_hostname = curlx_convert_UTF8_to_tchar(snihost);
    যদি(!backend->cred->sni_hostname)
      ফেরত CURLE_OUT_OF_MEMORY;
  }

  /* Warn if SNI is disabled due to use of an IP address */
  যদি(connssl->peer.type != CURL_SSL_PEER_DNS) {
    infof(তথ্য, "schannel: using IP address, SNI is not supported by OS.");
  }

  যদি(backend->use_alpn) {
    পূর্ণসংখ্যা cur = 0;
    পূর্ণসংখ্যা list_start_সূচক = 0;
    unsigned পূর্ণসংখ্যা *extension_len = ফাঁকা;
    unsigned short *list_len = ফাঁকা;
    ধ্রুবক unsigned পূর্ণসংখ্যা uiALPN = SecApplicationProtocolNegotiationExt_ALPN;
    কাঠামো alpn_proto_buf proto;

    /* The first four bytes is an unsigned int indicating number
       of bytes of data in the rest of the buffer. */
    extension_len = (unsigned পূর্ণসংখ্যা *)(শূন্য *)&alpn_বাফার[cur];
    cur += (পূর্ণসংখ্যা)আকার(unsigned পূর্ণসংখ্যা);

    /* The next four bytes are an indicator that this buffer contains
       ALPN data, as opposed to NPN, for example. */
    memcpy(&alpn_বাফার[cur], &uiALPN, আকার(uiALPN));
    cur += (পূর্ণসংখ্যা)আকার(unsigned পূর্ণসংখ্যা);

    /* The next two bytes is an unsigned short indicating the number
       of bytes used to list the preferred protocols. */
    list_len = (unsigned short *)(শূন্য *)&alpn_বাফার[cur];
    cur += (পূর্ণসংখ্যা)আকার(unsigned short);

    list_start_সূচক = cur;

    ফলাফল = Curl_alpn_to_proto_buf(&proto, connssl->alpn);
    যদি(ফলাফল) {
      failf(তথ্য, "Error setting ALPN");
      ফেরত CURLE_SSL_CONNECT_ERROR;
    }
    memcpy(&alpn_বাফার[cur], proto.তথ্য, proto.len);
    cur += proto.len;

    *list_len = curlx_uitous(cur - list_start_সূচক);
    *extension_len = (unsigned পূর্ণসংখ্যা)(*list_len +
      আকার(unsigned পূর্ণসংখ্যা) + আকার(unsigned short));

    Init_Sec_বাফার(&inbuf, SECBUFFER_APPLICATION_PROTOCOLS, alpn_বাফার, cur);
    Init_Sec_বাফার_Desc(&inbuf_desc, &inbuf, 1);

    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(তথ্য, VTLS_INFOF_ALPN_OFFER_1STR, proto.তথ্য);
  }
  অন্যথা {
    Init_Sec_বাফার(&inbuf, SECBUFFER_EMPTY, ফাঁকা, 0);
    Init_Sec_বাফার_Desc(&inbuf_desc, &inbuf, 1);
  }

  /* setup output buffer */
  Init_Sec_বাফার(&outbuf, SECBUFFER_EMPTY, ফাঁকা, 0);
  Init_Sec_বাফার_Desc(&outbuf_desc, &outbuf, 1);

  /* security request flags */
  backend->req_flags =
    ISC_REQ_SEQUENCE_DETECT | ISC_REQ_REPLAY_DETECT |
    ISC_REQ_CONFIDENTIALITY | ISC_REQ_ALLOCATE_MEMORY |
    ISC_REQ_STREAM |
    (!conn_config->auto_client_cert ? ISC_REQ_USE_SUPPLIED_CREDS : 0);

  /* allocate memory for the security context handle */
  backend->ctxt = (কাঠামো Curl_schannel_ctxt *)
    curlx_calloc(1, আকার(কাঠামো Curl_schannel_ctxt));
  যদি(!backend->ctxt) {
    failf(তথ্য, "schannel: unable to allocate memory");
    ফেরত CURLE_OUT_OF_MEMORY;
  }

  /* Schannel InitializeSecurityContext:
     https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-initializesecuritycontextw

     At the moment we do not pass inbuf unless we are using ALPN since we only
     use it for that, and WINE (for which we currently disable ALPN) is giving
     us problems with inbuf regardless. https://github.com/curl/curl/issues/983
   */
  sspi_status = Curl_pSecFn->InitializeSecurityContext(
    &backend->cred->cred_handle, ফাঁকা, backend->cred->sni_hostname,
    backend->req_flags, 0, 0,
    (backend->use_alpn ? &inbuf_desc : ফাঁকা),
    0, &backend->ctxt->ctxt_handle,
    &outbuf_desc, &backend->ret_flags, ফাঁকা);

  যদি(sspi_status != SEC_I_CONTINUE_NEEDED) {
    অক্ষর বাফার[STRERROR_LEN];
    curlx_safefree(backend->ctxt);
    নির্বাচন(sspi_status) {
    ক্ষেত্র SEC_E_INSUFFICIENT_MEMORY:
      failf(তথ্য, "schannel: initial InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(sspi_status, বাফার, আকার(বাফার)));
      ফেরত CURLE_OUT_OF_MEMORY;
    ক্ষেত্র SEC_E_WRONG_PRINCIPAL:
      failf(তথ্য, "schannel: SNI or certificate check failed: %s",
            Curl_sspi_strerror(sspi_status, বাফার, আকার(বাফার)));
      ফেরত CURLE_PEER_FAILED_VERIFICATION;
#যদি 0
    ক্ষেত্র SEC_E_INVALID_HANDLE:
    ক্ষেত্র SEC_E_INVALID_TOKEN:
    ক্ষেত্র SEC_E_LOGON_DENIED:
    ক্ষেত্র SEC_E_TARGET_UNKNOWN:
    ক্ষেত্র SEC_E_NO_AUTHENTICATING_AUTHORITY:
    ক্ষেত্র SEC_E_INTERNAL_ERROR:
    ক্ষেত্র SEC_E_NO_CREDENTIALS:
    ক্ষেত্র SEC_E_UNSUPPORTED_FUNCTION:
    ক্ষেত্র SEC_E_APPLICATION_PROTOCOL_MISMATCH:
#endif
    পূর্বনির্ধারিত:
      failf(তথ্য, "schannel: initial InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(sspi_status, বাফার, আকার(বাফার)));
      ফেরত CURLE_SSL_CONNECT_ERROR;
    }
  }

  DEBUGF(infof(তথ্য, "schannel: sending initial handshake data: "
               "sending %lu bytes.", outbuf.cb_বাফার));

  /* send initial handshake data which is now stored in output buffer */
  ফলাফল = Curl_conn_cf_send(cf->পরবর্তী, তথ্য,
                             (ধ্রুবক uint8_t *)outbuf.pv_বাফার,
                             outbuf.cb_বাফার, FALSE,
                             &written);
  Curl_pSecFn->Free_Context_বাফার(outbuf.pv_বাফার);
  যদি(ফলাফল || (outbuf.cb_বাফার != written)) {
    failf(তথ্য, "schannel: failed to send initial handshake data: "
          "sent %zu of %lu bytes", written, outbuf.cb_বাফার);
    ফেরত CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGF(infof(তথ্য, "schannel: sent initial handshake data: "
               "sent %zu bytes", written));

  backend->recv_unrecoverable_err = CURLE_OK;
  backend->recv_sspi_close_notify = FALSE;
  backend->recv_connection_closed = FALSE;
  backend->recv_renegotiating = FALSE;
  backend->renegotiate_state.started = FALSE;
  backend->encdata_is_incomplete = FALSE;
  backend->shutdown_generated = FALSE;
  backend->shutdown_offset = 0;
  memset(&backend->shutdown_token, 0, আকার(backend->shutdown_token));

  /* continue to second handshake step */
  connssl->connecting_state = ssl_connect_2;

  ফেরত CURLE_OK;
}

স্থির CURLcode schannel_error(কাঠামো Curl_easy *তথ্য,
                               SECURITY_STATUS sspi_status)
{
  অক্ষর বাফার[STRERROR_LEN];
  নির্বাচন(sspi_status) {
  ক্ষেত্র SEC_E_INSUFFICIENT_MEMORY:
    failf(তথ্য, "schannel: next InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(sspi_status, বাফার, আকার(বাফার)));
    ফেরত CURLE_OUT_OF_MEMORY;
  ক্ষেত্র SEC_E_WRONG_PRINCIPAL:
    failf(তথ্য, "schannel: SNI or certificate check failed: %s",
          Curl_sspi_strerror(sspi_status, বাফার, আকার(বাফার)));
    ফেরত CURLE_PEER_FAILED_VERIFICATION;
  ক্ষেত্র SEC_E_UNTRUSTED_মূল:
    failf(তথ্য, "schannel: %s",
          Curl_sspi_strerror(sspi_status, বাফার, আকার(বাফার)));
    ফেরত CURLE_PEER_FAILED_VERIFICATION;
#যদি 0
  ক্ষেত্র SEC_E_INVALID_HANDLE:
  ক্ষেত্র SEC_E_INVALID_TOKEN:
  ক্ষেত্র SEC_E_LOGON_DENIED:
  ক্ষেত্র SEC_E_TARGET_UNKNOWN:
  ক্ষেত্র SEC_E_NO_AUTHENTICATING_AUTHORITY:
  ক্ষেত্র SEC_E_INTERNAL_ERROR:
  ক্ষেত্র SEC_E_NO_CREDENTIALS:
  ক্ষেত্র SEC_E_UNSUPPORTED_FUNCTION:
  ক্ষেত্র SEC_E_APPLICATION_PROTOCOL_MISMATCH:
#endif
  পূর্বনির্ধারিত:
    failf(তথ্য, "schannel: next InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(sspi_status, বাফার, আকার(বাফার)));
    ফেরত CURLE_SSL_CONNECT_ERROR;
  }
}

স্থির CURLcode schannel_pkp_pin_peer_pubkey(কাঠামো Curl_cfilter *cf,
                                             কাঠামো Curl_easy *তথ্য,
                                             ধ্রুবক অক্ষর *pinnedpubkey)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো schannel_ssl_backend_তথ্য *backend =
    (কাঠামো schannel_ssl_backend_তথ্য *)connssl->backend;
  CERT_CONTEXT *pCertContextServer = ফাঁকা;

  /* Result is returned to caller */
  CURLcode ফলাফল = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  DEBUGASSERT(backend);

  /* if a path was not specified, do not pin */
  যদি(!pinnedpubkey)
    ফেরত CURLE_OK;

  করো {
    SECURITY_STATUS sspi_status;
    uint8_t *x509_der;
    DWORD x509_der_len;
    কাঠামো Curl_X509certificate x509_parsed;
    কাঠামো Curl_asn1Element *pubkey;

    sspi_status =
      Curl_pSecFn->QueryContextAttributes(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                          &pCertContextServer);

    যদি((sspi_status != SEC_E_OK) || !pCertContextServer) {
      অক্ষর বাফার[STRERROR_LEN];
      failf(তথ্য, "schannel: Failed to read remote certificate context: %s",
            Curl_sspi_strerror(sspi_status, বাফার, আকার(বাফার)));
      বিরতি; /* failed */
    }

    যদি(!(((pCertContextServer->dwCertEncodingType & X509_ASN_ENCODING) != 0) &&
         (pCertContextServer->cbCertEncoded > 0)))
      বিরতি;

    x509_der = (uint8_t *)pCertContextServer->pbCertEncoded;
    x509_der_len = pCertContextServer->cbCertEncoded;
    memset(&x509_parsed, 0, আকার(x509_parsed));
    যদি(Curl_parseX509(&x509_parsed, x509_der, x509_der + x509_der_len))
      বিরতি;

    pubkey = &x509_parsed.subject_Public_চাবি_Info;
    যদি(!pubkey->header || pubkey->end <= pubkey->header) {
      failf(তথ্য, "SSL: failed retrieving public key from server certificate");
      বিরতি;
    }

    ফলাফল = Curl_pin_peer_pubkey(তথ্য,
                                  pinnedpubkey,
                                  (ধ্রুবক unsigned অক্ষর *)pubkey->header,
                                  (size_t)(pubkey->end - pubkey->header));
    যদি(ফলাফল) {
      failf(তথ্য, "SSL: public key does not match pinned public key");
    }
  } যখন(0);

  যদি(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  ফেরত ফলাফল;
}

স্থির CURLcode ensure_encoding_আকার(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো sbuffer *encdata,
                                     size_t min_দৈর্ঘ্য)
{
  size_t আকার_2;
  DEBUGASSERT(encdata->দৈর্ঘ্য >= encdata->offset);
  যদি(encdata->দৈর্ঘ্য < encdata->offset)
    ফেরত CURLE_FAILED_INIT;
  আকার_2 = encdata->দৈর্ঘ্য - encdata->offset;
  যদি(আকার_2 < CURL_SCHANNEL_বাফার_FREE_আকার || encdata->দৈর্ঘ্য < min_দৈর্ঘ্য) {
    unsigned অক্ষর *বাফার;
    size_t দৈর্ঘ্য = encdata->offset + CURL_SCHANNEL_বাফার_FREE_আকার;
    যদি(দৈর্ঘ্য < min_দৈর্ঘ্য)
      দৈর্ঘ্য = min_দৈর্ঘ্য;
    বাফার = curlx_realloc(encdata->বাফার, দৈর্ঘ্য);
    যদি(!বাফার) {
      failf(তথ্য, "schannel: unable to re-allocate memory");
      ফেরত CURLE_OUT_OF_MEMORY;
    }

    encdata->বাফার = বাফার;
    encdata->দৈর্ঘ্য = দৈর্ঘ্য;
    SCH_DEV(infof(তথ্য, "schannel: encdata.buffer resized %zu",
                  encdata->দৈর্ঘ্য));
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode ensure_decoding_আকার(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো sbuffer *decdata,
                                     size_t nowsize,
                                     size_t len)
{
  size_t আকার_2 = nowsize > CURL_SCHANNEL_বাফার_FREE_আকার ?
    nowsize : CURL_SCHANNEL_বাফার_FREE_আকার;
  DEBUGASSERT(decdata->দৈর্ঘ্য >= decdata->offset);
  যদি(decdata->দৈর্ঘ্য < decdata->offset)
    ফেরত CURLE_FAILED_INIT;
  অন্যথা যদি(decdata->দৈর্ঘ্য - decdata->offset < আকার_2 ||
          decdata->দৈর্ঘ্য < len) {
    /* increase internal decrypted data buffer */
    size_t দৈর্ঘ্য = decdata->offset + আকার_2;
    unsigned অক্ষর *বাফার;
    /* make sure that the requested amount of data fits */
    যদি(দৈর্ঘ্য < len)
      দৈর্ঘ্য = len;

    বাফার = curlx_realloc(decdata->বাফার, দৈর্ঘ্য);
    যদি(!বাফার) {
      failf(তথ্য, "schannel: unable to re-allocate memory");
      ফেরত CURLE_OUT_OF_MEMORY;
    }
    decdata->বাফার = বাফার;
    decdata->দৈর্ঘ্য = দৈর্ঘ্য;
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode schannel_connect_step2(কাঠামো Curl_cfilter *cf,
                                       কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো schannel_ssl_backend_তথ্য *backend =
    (কাঠামো schannel_ssl_backend_তথ্য *)connssl->backend;
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  পূর্ণসংখ্যা i;
  size_t nread = 0;
  Sec_বাফার outbuf[3];
  Sec_বাফার_Desc outbuf_desc;
  Sec_বাফার inbuf[2];
  Sec_বাফার_Desc inbuf_desc;
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CURLcode ফলাফল;
  bool doread;
  ধ্রুবক অক্ষর *pubkey_ptr;

  DEBUGASSERT(backend);

  doread = (connssl->io_need & CURL_SSL_IO_NEED_SEND) ? FALSE : TRUE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  DEBUGF(infof(তথ্য, "schannel: SSL/TLS connection with %s port %d (step 2/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->port));

  যদি(!backend->cred || !backend->ctxt)
    ফেরত CURLE_SSL_CONNECT_ERROR;

  /* buffer to store previously received and decrypted data */
  যদি(!backend->decdata.বাফার) {
    backend->decdata.offset = 0;
    backend->decdata.দৈর্ঘ্য = CURL_SCHANNEL_বাফার_INIT_আকার;
    backend->decdata.বাফার = curlx_malloc(backend->decdata.দৈর্ঘ্য);
    যদি(!backend->decdata.বাফার) {
      failf(তথ্য, "schannel: unable to allocate memory");
      ফেরত CURLE_OUT_OF_MEMORY;
    }
  }

  /* buffer to store previously received and encrypted data */
  যদি(!backend->encdata.বাফার) {
    backend->encdata_is_incomplete = FALSE;
    backend->encdata.offset = 0;
    backend->encdata.দৈর্ঘ্য = CURL_SCHANNEL_বাফার_INIT_আকার;
    backend->encdata.বাফার = curlx_malloc(backend->encdata.দৈর্ঘ্য);
    যদি(!backend->encdata.বাফার) {
      failf(তথ্য, "schannel: unable to allocate memory");
      ফেরত CURLE_OUT_OF_MEMORY;
    }
  }

  ফলাফল = ensure_encoding_আকার(তথ্য, &backend->encdata, 0);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  জন্য(;;) {
    যদি(doread) {
      /* read encrypted handshake data from socket */
      ফলাফল = Curl_conn_cf_recv(cf->পরবর্তী, তথ্য,
                                 (অক্ষর *)(backend->encdata.বাফার +
                                          backend->encdata.offset),
                                 backend->encdata.দৈর্ঘ্য -
                                 backend->encdata.offset,
                                 &nread);
      যদি(ফলাফল == CURLE_AGAIN) {
        যদি(!backend->encdata.offset || backend->encdata_is_incomplete) {
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          DEBUGF(infof(তথ্য, "schannel: failed to receive handshake, "
                       "need more data"));
          ফেরত CURLE_OK;
        }
        অন্যথা {
          DEBUGF(infof(তথ্য, "schannel: no new handshake data received, "
                       "continuing to process existing handshake data"));
        }
      }
      অন্যথা যদি(ফলাফল || (nread == 0)) {
        failf(তথ্য, "schannel: failed to receive handshake, "
              "SSL/TLS connection failed");
        ফেরত CURLE_SSL_CONNECT_ERROR;
      }
      অন্যথা {
        /* increase encrypted data buffer offset */
        backend->encdata.offset += nread;
        backend->encdata_is_incomplete = FALSE;
        SCH_DEV(infof(তথ্য, "schannel: encrypted data got %zu", nread));
      }
    }

    SCH_DEV(infof(তথ্য,
                  "schannel: encrypted data buffer: offset %zu length %zu",
                  backend->encdata.offset, backend->encdata.দৈর্ঘ্য));

    /* setup input buffers */
    Init_Sec_বাফার(&inbuf[0], SECBUFFER_TOKEN,
                  curlx_memdup0((ধ্রুবক অক্ষর *)backend->encdata.বাফার,
                                backend->encdata.offset),
                  curlx_uztoul(backend->encdata.offset));
    Init_Sec_বাফার(&inbuf[1], SECBUFFER_EMPTY, ফাঁকা, 0);
    Init_Sec_বাফার_Desc(&inbuf_desc, inbuf, 2);

    /* setup output buffers */
    Init_Sec_বাফার(&outbuf[0], SECBUFFER_TOKEN, ফাঁকা, 0);
    Init_Sec_বাফার(&outbuf[1], SECBUFFER_ALERT, ফাঁকা, 0);
    Init_Sec_বাফার(&outbuf[2], SECBUFFER_EMPTY, ফাঁকা, 0);
    Init_Sec_বাফার_Desc(&outbuf_desc, outbuf, 3);

    যদি(!inbuf[0].pv_বাফার) {
      failf(তথ্য, "schannel: unable to allocate memory");
      ফেরত CURLE_OUT_OF_MEMORY;
    }

    /* The socket must be writable (or a poll error occurred) before we call
       InitializeSecurityContext to continue processing the received TLS
       records. This is because that function is not idempotent and we do not
       support partial save/resume sending replies of handshake tokens. */
    যদি(!SOCKET_WRITABLE(Curl_conn_cf_get_socket(cf, তথ্য), 0)) {
      SCH_DEV(infof(তথ্য, "schannel: handshake waiting for writable socket"));
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      curlx_free(inbuf[0].pv_বাফার);
      ফেরত CURLE_OK;
    }

    sspi_status = Curl_pSecFn->InitializeSecurityContext(
      &backend->cred->cred_handle, &backend->ctxt->ctxt_handle,
      backend->cred->sni_hostname, backend->req_flags,
      0, 0, &inbuf_desc, 0, ফাঁকা,
      &outbuf_desc, &backend->ret_flags, ফাঁকা);

    /* free buffer for received handshake data */
    curlx_safefree(inbuf[0].pv_বাফার);

    /* check if the handshake was incomplete */
    নির্বাচন(sspi_status) {
    ক্ষেত্র SEC_E_INCOMPLETE_MESSAGE:
      backend->encdata_is_incomplete = TRUE;
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      DEBUGF(infof(তথ্য,
                   "schannel: received incomplete message, need more data"));
      ফেরত CURLE_OK;

    ক্ষেত্র SEC_I_CONTINUE_NEEDED:
    ক্ষেত্র SEC_E_OK:
      /* check if the handshake needs to be continued */
      ফলাফল = CURLE_OK;
      জন্য(i = 0; i < 3; i++) {
        /* search for handshake tokens that need to be sent */
        যদি(outbuf[i].বাফার_Type == SECBUFFER_TOKEN && outbuf[i].cb_বাফার > 0) {
          size_t written = 0;
          DEBUGF(infof(তথ্য, "schannel: sending next handshake data: "
                       "sending %lu bytes.", outbuf[i].cb_বাফার));

          /* send handshake token to server */
          ফলাফল = Curl_conn_cf_send(cf->পরবর্তী, তথ্য,
                                     (ধ্রুবক uint8_t *)outbuf[i].pv_বাফার,
                                     outbuf[i].cb_বাফার,
                                     FALSE, &written);
          যদি(ফলাফল || (outbuf[i].cb_বাফার != written)) {
            failf(তথ্য, "schannel: failed to send next handshake data: "
                  "sent %zu of %lu bytes", written, outbuf[i].cb_বাফার);
            ফলাফল = CURLE_SSL_CONNECT_ERROR;
          }
        }
      }
      জন্য(i = 0; i < 3; i++) {
        /* free obsolete buffer */
        যদি(outbuf[i].pv_বাফার)
          Curl_pSecFn->Free_Context_বাফার(outbuf[i].pv_বাফার);
      }
      যদি(ফলাফল)
        ফেরত ফলাফল;
      বিরতি;

    ক্ষেত্র SEC_I_INCOMPLETE_CREDENTIALS:
      যদি(!(backend->req_flags & ISC_REQ_USE_SUPPLIED_CREDS)) {
        /* If the server has requested a client certificate, attempt to
           continue the handshake without one. This allows connections to
           servers which request a client certificate but do not require
           it. */
        backend->req_flags |= ISC_REQ_USE_SUPPLIED_CREDS;
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        DEBUGF(infof(তথ্য,
                     "schannel: a client certificate has been requested"));
        ফেরত CURLE_OK;
      }
      FALLTHROUGH();

    পূর্বনির্ধারিত:
      ফেরত schannel_error(তথ্য, sspi_status);
    }

    /* check if there was additional remaining encrypted data */
    যদি(inbuf[1].বাফার_Type == SECBUFFER_EXTRA && inbuf[1].cb_বাফার > 0) {
      SCH_DEV(infof(তথ্য, "schannel: encrypted data length: %lu",
                    inbuf[1].cb_বাফার));
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
      যদি(backend->encdata.offset > inbuf[1].cb_বাফার) {
        memmove(backend->encdata.বাফার,
                (backend->encdata.বাফার + backend->encdata.offset) -
                inbuf[1].cb_বাফার, inbuf[1].cb_বাফার);
        backend->encdata.offset = inbuf[1].cb_বাফার;
        যদি(sspi_status == SEC_I_CONTINUE_NEEDED) {
          doread = FALSE;
          চালু;
        }
      }
    }
    অন্যথা {
      backend->encdata.offset = 0;
    }
    বিরতি;
  }

  /* check if the handshake needs to be continued */
  যদি(sspi_status == SEC_I_CONTINUE_NEEDED) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    ফেরত CURLE_OK;
  }

  /* check if the handshake is complete */
  যদি(sspi_status == SEC_E_OK) {
    connssl->connecting_state = ssl_connect_3;
    DEBUGF(infof(তথ্য, "schannel: SSL/TLS handshake complete"));
  }

#ifndef CURL_DISABLE_PROXY
  pubkey_ptr = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(তথ্য, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(তথ্য, STRING_SSL_PINNEDPUBLICKEY);
#অন্যথা
  pubkey_ptr = CURL_EASY_STR(তথ্য, STRING_SSL_PINNEDPUBLICKEY);
#endif
  যদি(pubkey_ptr) {
    ফলাফল = schannel_pkp_pin_peer_pubkey(cf, তথ্য, pubkey_ptr);
    যদি(ফলাফল) {
      failf(তথ্য, "SSL: public key does not match pinned public key");
      ফেরত ফলাফল;
    }
  }

  যদি(conn_config->verifypeer && backend->use_manual_cred_validation) {
    /* Certificate verification also verifies the hostname if verifyhost */
    ফেরত Curl_verify_certificate(cf, তথ্য);
  }

  /* Verify the hostname manually when certificate verification is disabled,
     because in that case Schannel does not verify it. */
  যদি(!conn_config->verifypeer && conn_config->verifyhost)
    ফেরত Curl_verify_host(cf, তথ্য);

  ফেরত CURLE_OK;
}

স্থির bool valid_cert_encoding(ধ্রুবক CERT_CONTEXT *cert_context)
{
  ফেরত cert_context &&
    ((cert_context->dwCertEncodingType & X509_ASN_ENCODING) != 0) &&
    cert_context->pbCertEncoded &&
    (cert_context->cbCertEncoded > 0);
}

প্রকারলেখ bool (*Read_crt_func)(ধ্রুবক CERT_CONTEXT *ccert_context,
                              bool reverse_order, শূন্য *arg);

স্থির শূন্য traverse_cert_store(ধ্রুবক CERT_CONTEXT *context,
                                Read_crt_func func, শূন্য *arg)
{
  ধ্রুবক CERT_CONTEXT *current_context = ফাঁকা;
  bool should_continue = TRUE;
  bool first = TRUE;
  bool reverse_order = FALSE;
  যখন(should_continue &&
        (current_context = CertEnumCertificatesInStore(
          context->hCertStore,
          current_context)) != ফাঁকা) {
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

স্থির bool cert_গণক_callback(ধ্রুবক CERT_CONTEXT *ccert_context,
                                  bool reverse_order, শূন্য *certs_গুনতি)
{
  (শূন্য)reverse_order;
  যদি(valid_cert_encoding(ccert_context))
    (*(পূর্ণসংখ্যা *)certs_গুনতি)++;
  যদি(*(পূর্ণসংখ্যা *)certs_গুনতি > MAX_ALLOWED_CERT_AMOUNT)
    ফেরত FALSE;
  ফেরত TRUE;
}

কাঠামো Adder_args {
  কাঠামো Curl_easy *তথ্য;
  CURLcode ফলাফল;
  পূর্ণসংখ্যা idx;
  পূর্ণসংখ্যা certs_গুনতি;
};

স্থির bool add_cert_to_certinfo(ধ্রুবক CERT_CONTEXT *ccert_context,
                                 bool reverse_order, শূন্য *raw_arg)
{
  কাঠামো Adder_args *args = (কাঠামো Adder_args *)raw_arg;
  args->ফলাফল = CURLE_OK;
  যদি(valid_cert_encoding(ccert_context)) {
    পূর্ণসংখ্যা ঢোকাও_সূচক = reverse_order ? (args->certs_গুনতি - 1) - args->idx :
                       args->idx;
    args->ফলাফল = Curl_extract_certinfo(args->তথ্য, ঢোকাও_সূচক,
                                         ccert_context->pbCertEncoded,
                                         ccert_context->pbCertEncoded +
                                         ccert_context->cbCertEncoded);
    args->idx++;
  }
  ফেরত args->ফলাফল == CURLE_OK;
}

স্থির শূন্য schannel_session_free(শূন্য *sessionid)
{
  /* this is expected to be called under sessionid lock */
  কাঠামো Curl_schannel_cred *cred = sessionid;

  যদি(cred) {
    cred->refcount--;
    যদি(cred->refcount == 0) {
      Curl_pSecFn->FreeCredentialsHandle(&cred->cred_handle);
      curlx_free(cred->sni_hostname);
      যদি(cred->client_cert_store) {
        CertCloseStore(cred->client_cert_store, 0);
        cred->client_cert_store = ফাঁকা;
      }
      curlx_safefree(cred);
    }
  }
}

স্থির CURLcode schannel_connect_step3(কাঠামো Curl_cfilter *cf,
                                       কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো schannel_ssl_backend_তথ্য *backend =
    (কাঠামো schannel_ssl_backend_তথ্য *)connssl->backend;
  কাঠামো ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, তথ্য);
  CURLcode ফলাফল = CURLE_OK;
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CERT_CONTEXT *ccert_context = ফাঁকা;
  SecPkgContext_ApplicationProtocol alpn_ফলাফল;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);
  DEBUGASSERT(backend);

  DEBUGF(infof(তথ্য, "schannel: SSL/TLS connection with %s port %d (step 3/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->port));

  যদি(!backend->cred)
    ফেরত CURLE_SSL_CONNECT_ERROR;

  /* check if the required context attributes are met */
  যদি(backend->ret_flags != backend->req_flags) {
    যদি(!(backend->ret_flags & ISC_RET_SEQUENCE_DETECT))
      failf(তথ্য, "schannel: failed to setup sequence detection");
    যদি(!(backend->ret_flags & ISC_RET_REPLAY_DETECT))
      failf(তথ্য, "schannel: failed to setup replay detection");
    যদি(!(backend->ret_flags & ISC_RET_CONFIDENTIALITY))
      failf(তথ্য, "schannel: failed to setup confidentiality");
    যদি(!(backend->ret_flags & ISC_RET_ALLOCATED_MEMORY))
      failf(তথ্য, "schannel: failed to setup memory allocation");
    যদি(!(backend->ret_flags & ISC_RET_STREAM))
      failf(তথ্য, "schannel: failed to setup stream orientation");
    ফেরত CURLE_SSL_CONNECT_ERROR;
  }

  যদি(backend->use_alpn) {
    sspi_status =
      Curl_pSecFn->QueryContextAttributes(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_APPLICATION_PROTOCOL,
                                          &alpn_ফলাফল);

    যদি(sspi_status != SEC_E_OK) {
      failf(তথ্য, "schannel: failed to retrieve ALPN result");
      ফেরত CURLE_SSL_CONNECT_ERROR;
    }

    যদি(alpn_ফলাফল.ProtoNegoStatus ==
       SecApplicationProtocolNegotiationStatus_Success) {
      যদি(backend->recv_renegotiating &&
         connssl->negotiated.alpn &&
         strncmp(connssl->negotiated.alpn,
                 (ধ্রুবক অক্ষর *)alpn_ফলাফল.ProtocolId,
                 alpn_ফলাফল.Protocol_Id_আকার)) {
        /* Renegotiation selected a different protocol now, we cannot
         * deal with this */
        failf(তথ্য, "schannel: server selected an ALPN protocol too late");
        ফেরত CURLE_SSL_CONNECT_ERROR;
      }
      Curl_alpn_set_negotiated(cf, তথ্য, connssl, alpn_ফলাফল.ProtocolId,
                               alpn_ফলাফল.Protocol_Id_আকার);
    }
    অন্যথা {
      যদি(!backend->recv_renegotiating)
        Curl_alpn_set_negotiated(cf, তথ্য, connssl, ফাঁকা, 0);
    }
  }

  /* save the current session data for possible reuse */
  যদি(Curl_ssl_scache_use(cf, তথ্য)) {
    Curl_ssl_scache_lock(তথ্য);
    /* Up ref count since call takes ownership */
    backend->cred->refcount++;
    ফলাফল = Curl_ssl_scache_add_obj(cf, তথ্য, connssl->peer.scache_চাবি,
                                     backend->cred, schannel_session_free);
    Curl_ssl_scache_unlock(তথ্য);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  যদি(ssl_config->certinfo) {
    পূর্ণসংখ্যা certs_গুনতি = 0;
    sspi_status =
      Curl_pSecFn->QueryContextAttributes(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                          &ccert_context);

    যদি((sspi_status != SEC_E_OK) || !ccert_context) {
      failf(তথ্য, "schannel: failed to retrieve remote cert context");
      ফেরত CURLE_PEER_FAILED_VERIFICATION;
    }

    traverse_cert_store(ccert_context, cert_গণক_callback, &certs_গুনতি);
    যদি(certs_গুনতি > MAX_ALLOWED_CERT_AMOUNT) {
      failf(তথ্য, "%d certificates is more than allowed (%d)",
            certs_গুনতি, MAX_ALLOWED_CERT_AMOUNT);
      CertFreeCertificateContext(ccert_context);
      ফেরত CURLE_SSL_CONNECT_ERROR;
    }

    ফলাফল = Curl_ssl_init_certinfo(তথ্য, certs_গুনতি);
    যদি(!ফলাফল) {
      কাঠামো Adder_args args;
      args.তথ্য = তথ্য;
      args.idx = 0;
      args.certs_গুনতি = certs_গুনতি;
      args.ফলাফল = CURLE_OK;
      traverse_cert_store(ccert_context, add_cert_to_certinfo, &args);
      ফলাফল = args.ফলাফল;
    }
    CertFreeCertificateContext(ccert_context);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  connssl->connecting_state = ssl_connect_done;

  ফেরত CURLE_OK;
}

স্থির CURLcode schannel_connect(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 bool *done)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  CURLcode ফলাফল;

  /* check if the connection has already been established */
  যদি(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  *done = FALSE;

  যদি(connssl->connecting_state == ssl_connect_1) {
    ফলাফল = schannel_connect_step1(cf, তথ্য);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  যদি(connssl->connecting_state == ssl_connect_2) {
    ফলাফল = schannel_connect_step2(cf, তথ্য);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  যদি(connssl->connecting_state == ssl_connect_3) {
    ফলাফল = schannel_connect_step3(cf, তথ্য);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  যদি(connssl->connecting_state == ssl_connect_done) {
    কাঠামো schannel_ssl_backend_তথ্য *backend =
      (কাঠামো schannel_ssl_backend_তথ্য *)connssl->backend;
    DEBUGASSERT(backend);

    যদি(Curl_pSecFn->QueryContextAttributes(
         &backend->ctxt->ctxt_handle,
         SECPKG_ATTR_STREAM_SIZES,
         &backend->stream_sizes)) {
      failf(তথ্য, "schannel: failed getting stream sizes");
      ফেরত CURLE_SSL_CONNECT_ERROR;
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

  ফেরত CURLE_OK;
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
  কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
  গণনা schannel_renegotiate_caller_t caller)
{
  CURLcode ফলাফল;
  curl_socket_t sockfd;
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো schannel_ssl_backend_তথ্য *backend =
    (কাঠামো schannel_ssl_backend_তথ্য *)connssl->backend;
  কাঠামো schannel_renegotiate_state *rs = &backend->renegotiate_state;

  যদি(!backend || !backend->recv_renegotiating) {
    failf(তথ্য, "schannel: unexpected call to schannel_recv_renegotiate");
    ফেরত CURLE_SSL_CONNECT_ERROR;
  }
  DEBUGASSERT(caller <= SCH_RENEG_CALLER_IS_SEND);
  যদি(caller == SCH_RENEG_CALLER_IS_RECV)
    SCH_DEV(infof(তথ্য, "schannel: renegotiation caller is schannel_recv"));
  অন্যথা
    SCH_DEV(infof(তথ্য, "schannel: renegotiation caller is schannel_send"));

  sockfd = Curl_conn_cf_get_socket(cf, তথ্য);

  যদি(sockfd == CURL_SOCKET_BAD) {
    failf(তথ্য, "schannel: renegotiation missing socket");
    ফেরত CURLE_SSL_CONNECT_ERROR;
  }

  যদি(!rs->started) { /* new renegotiation */
    infof(তথ্য, "schannel: renegotiating SSL/TLS connection");
    DEBUGASSERT(connssl->state == ssl_connection_complete);
    DEBUGASSERT(connssl->connecting_state == ssl_connect_done);
    connssl->state = ssl_connection_negotiating;
    connssl->connecting_state = ssl_connect_2;
    memset(rs, 0, আকার(*rs));
    rs->io_need = CURL_SSL_IO_NEED_SEND;
    rs->start_time = *Curl_pgrs_now(তথ্য);
    rs->started = TRUE;
  }

  জন্য(;;) {
    bool block_read, block_write, blocking, done;
    curl_socket_t readfd, writefd;
    timediff_t elapsed;

    elapsed = curlx_ptimediff_ms(Curl_pgrs_now(তথ্য), &rs->start_time);
    যদি(elapsed >= MAX_RENEG_BLOCK_TIME) {
      failf(তথ্য, "schannel: renegotiation timeout");
      ফলাফল = CURLE_SSL_CONNECT_ERROR;
      বিরতি;
    }

    /* the current io_need state may have been overwritten since the last time
       this function was called. restore the io_need state needed to continue
       the renegotiation. */

    connssl->io_need = rs->io_need;

    ফলাফল = schannel_connect(cf, তথ্য, &done);

    rs->io_need = connssl->io_need;

    যদি(!ফলাফল && !done)
      ফলাফল = CURLE_AGAIN;

    যদি(ফলাফল != CURLE_AGAIN)
      বিরতি;

    readfd = (rs->io_need & CURL_SSL_IO_NEED_RECV) ? sockfd : CURL_SOCKET_BAD;
    writefd = (rs->io_need & CURL_SSL_IO_NEED_SEND) ? sockfd : CURL_SOCKET_BAD;

    যদি(readfd == CURL_SOCKET_BAD && writefd == CURL_SOCKET_BAD)
      চালু;

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

    জন্য(;;) {
      পূর্ণসংখ্যা what;
      timediff_t timeout_ms, remaining;

      ফলাফল = Curl_pgrsUpdate(তথ্য);
      যদি(ফলাফল)
        বিরতি;

      elapsed = curlx_ptimediff_ms(Curl_pgrs_now(তথ্য), &rs->start_time);
      যদি(elapsed >= MAX_RENEG_BLOCK_TIME) {
        failf(তথ্য, "schannel: renegotiation timeout");
        ফলাফল = CURLE_SSL_CONNECT_ERROR;
        বিরতি;
      }
      remaining = MAX_RENEG_BLOCK_TIME - elapsed;

      যদি(blocking) {
        timeout_ms = Curl_timeleft_ms(তথ্য);

        যদি(timeout_ms < 0) {
          ফলাফল = CURLE_OPERATION_TIMEDOUT;
          বিরতি;
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

      SCH_DEV(infof(তথ্য, "schannel: renegotiation wait until socket is"
                    "%s%s for up to %" FMT_TIMEDIFF_T " ms",
                    ((readfd != CURL_SOCKET_BAD) ? " readable" : ""),
                    ((writefd != CURL_SOCKET_BAD) ? " writable" : ""),
                    timeout_ms));

      what = Curl_socket_check(readfd, CURL_SOCKET_BAD, writefd, timeout_ms);

      যদি(what > 0 && (what & (CURL_CSELECT_IN | CURL_CSELECT_OUT))) {
        SCH_DEV(infof(তথ্য, "schannel: renegotiation socket %s%s",
                      ((what & CURL_CSELECT_IN) ? "CURL_CSELECT_IN " : ""),
                      ((what & CURL_CSELECT_OUT) ? "CURL_CSELECT_OUT " : "")));
        ফলাফল = CURLE_AGAIN;
        বিরতি;
      }
      অন্যথা যদি(!what) {
        SCH_DEV(infof(তথ্য, "schannel: renegotiation socket timeout"));
        যদি(blocking)
          চালু;
        অন্যথা
          ফেরত CURLE_AGAIN;
      }

      failf(তথ্য, "schannel: socket error during renegotiation");
      ফলাফল = CURLE_SSL_CONNECT_ERROR;
      বিরতি;
    }
    যদি(ফলাফল != CURLE_AGAIN)
      বিরতি;
  }

  DEBUGASSERT(ফলাফল != CURLE_AGAIN);

  rs->started = FALSE;
  backend->recv_renegotiating = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  যদি(ফলাফল)
    failf(তথ্য, "schannel: renegotiation failed");
  অন্যথা
    infof(তথ্য, "schannel: SSL/TLS connection renegotiated");

  ফেরত ফলাফল;
}

স্থির CURLcode schannel_send(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                              ধ্রুবক শূন্য *buf, size_t len, size_t *pnwritten)
{
  size_t তথ্য_len = 0;
  unsigned অক্ষর *ptr = ফাঁকা;
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  Sec_বাফার outbuf[4];
  Sec_বাফার_Desc outbuf_desc;
  SECURITY_STATUS sspi_status = SEC_E_OK;
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো schannel_ssl_backend_তথ্য *backend =
    (কাঠামো schannel_ssl_backend_তথ্য *)connssl->backend;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  যদি(backend->recv_renegotiating) {
    ফলাফল = schannel_recv_renegotiate(cf, তথ্য, SCH_RENEG_CALLER_IS_SEND);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  /* check if the buffer is longer than the maximum message length */
  যদি(len > backend->stream_sizes.cbMaximumMessage) {
    len = backend->stream_sizes.cbMaximumMessage;
  }

  /* calculate the complete message length and prepare the send buffer */
  তথ্য_len = backend->stream_sizes.cbHeader + len +
    backend->stream_sizes.cbTrailer;
  যদি(তথ্য_len > backend->send_বাফার_len) {
    ptr = curlx_realloc(backend->send_বাফার, তথ্য_len);
    যদি(!ptr)
      ফেরত CURLE_OUT_OF_MEMORY;
    backend->send_বাফার = ptr;
    backend->send_বাফার_len = তথ্য_len;
  }
  অন্যথা
    ptr = backend->send_বাফার;

  /* setup output buffers (header, data, trailer, empty) */
  Init_Sec_বাফার(&outbuf[0], SECBUFFER_STREAM_HEADER,
                ptr, backend->stream_sizes.cbHeader);
  Init_Sec_বাফার(&outbuf[1], SECBUFFER_তথ্য,
                ptr + backend->stream_sizes.cbHeader, curlx_uztoul(len));
  Init_Sec_বাফার(&outbuf[2], SECBUFFER_STREAM_TRAILER,
                ptr + backend->stream_sizes.cbHeader + len,
                backend->stream_sizes.cbTrailer);
  Init_Sec_বাফার(&outbuf[3], SECBUFFER_EMPTY, ফাঁকা, 0);
  Init_Sec_বাফার_Desc(&outbuf_desc, outbuf, 4);

  /* copy data into output buffer */
  memcpy(outbuf[1].pv_বাফার, buf, len);

  /* https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-encryptmessage */
  sspi_status = Curl_pSecFn->EncryptMessage(&backend->ctxt->ctxt_handle, 0,
                                            &outbuf_desc, 0);

  /* check if the message was encrypted */
  যদি(sspi_status == SEC_E_OK) {

    /* send the encrypted message including header, data and trailer */
    len = outbuf[0].cb_বাফার + outbuf[1].cb_বাফার + outbuf[2].cb_বাফার;

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
    যখন(len > *pnwritten) {
      size_t this_write = 0;
      পূর্ণসংখ্যা what;
      timediff_t timeout_ms = Curl_timeleft_ms(তথ্য);
      যদি(timeout_ms < 0) {
        /* we already got the timeout */
        failf(তথ্য, "schannel: timed out sending data (bytes sent: %zu)",
              *pnwritten);
        ফলাফল = CURLE_OPERATION_TIMEDOUT;
        বিরতি;
      }
      অন্যথা যদি(!timeout_ms)
        timeout_ms = TIMEDIFF_T_MAX;
      what = SOCKET_WRITABLE(Curl_conn_cf_get_socket(cf, তথ্য), timeout_ms);
      যদি(what < 0) {
        /* fatal error */
        failf(তথ্য, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        ফলাফল = CURLE_SEND_ERROR;
        বিরতি;
      }
      অন্যথা যদি(what == 0) {
        failf(তথ্য, "schannel: timed out sending data (bytes sent: %zu)",
              *pnwritten);
        ফলাফল = CURLE_OPERATION_TIMEDOUT;
        বিরতি;
      }
      /* socket is writable */

       ফলাফল = Curl_conn_cf_send(cf->পরবর্তী, তথ্য,
                                  (ধ্রুবক uint8_t *)ptr + *pnwritten,
                                  len - *pnwritten,
                                  FALSE, &this_write);
      যদি(ফলাফল == CURLE_AGAIN)
        চালু;
      অন্যথা যদি(ফলাফল) {
        বিরতি;
      }

      *pnwritten += this_write;
    }
  }
  অন্যথা যদি(sspi_status == SEC_E_INSUFFICIENT_MEMORY) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
  }
  অন্যথা {
    ফলাফল = CURLE_SEND_ERROR;
  }

  যদি(len == *pnwritten)
    /* Encrypted message including header, data and trailer entirely sent.
       The return value is the number of unencrypted bytes that were sent. */
    *pnwritten = outbuf[1].cb_বাফার;

  ফেরত ফলাফল;
}

স্থির CURLcode schannel_recv(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                              অক্ষর *buf, size_t len, size_t *pnread)
{
  size_t আকার_2 = 0;
  size_t nread = 0;
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  Sec_বাফার inbuf[4];
  Sec_বাফার_Desc inbuf_desc;
  SECURITY_STATUS sspi_status = SEC_E_OK;
  কাঠামো schannel_ssl_backend_তথ্য *backend =
    (কাঠামো schannel_ssl_backend_তথ্য *)connssl->backend;
  CURLcode ফলাফল = CURLE_OK;

  DEBUGASSERT(backend);
  *pnread = 0;

  যদি(backend->recv_renegotiating) {
    ফলাফল = schannel_recv_renegotiate(cf, তথ্য, SCH_RENEG_CALLER_IS_RECV);
    যদি(ফলাফল)
      ফেরত ফলাফল;
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

  SCH_DEV(infof(তথ্য, "schannel: client wants to read %zu bytes", len));

  যদি(len && len <= backend->decdata.offset) {
    SCH_DEV(infof(তথ্য,
                  "schannel: enough decrypted data is already available"));
    যাও cleanup;
  }
  অন্যথা যদি(backend->recv_unrecoverable_err) {
    ফলাফল = backend->recv_unrecoverable_err;
    infof(তথ্য, "schannel: an unrecoverable error occurred in a prior call");
    যাও cleanup;
  }
  অন্যথা যদি(backend->recv_sspi_close_notify) {
    /* once a server has indicated shutdown there is no more encrypted data */
    infof(তথ্য, "schannel: server indicated shutdown in a prior call");
    যাও cleanup;
  }
  /* it is debatable what to return when !len. Regardless we cannot return
     immediately because there may be data to decrypt (in the case we want to
     decrypt all encrypted cached data) so handle !len later in cleanup. */
  অন্যথা যদি(len && !backend->recv_connection_closed) {
    /* the encrypted buffer must be large enough to hold all the bytes
       requested and some TLS record overhead. 'len' is a buffer size, so this
       integer math cannot overflow. */
    ধ্রুবক size_t min_encdata_দৈর্ঘ্য = len + CURL_SCHANNEL_বাফার_FREE_আকার;

    /* make sure encrypt buffer fits the requested amount of data */
    ফলাফল = ensure_encoding_আকার(তথ্য, &backend->encdata, min_encdata_দৈর্ঘ্য);
    যদি(ফলাফল)
      যাও cleanup;

    SCH_DEV(infof(তথ্য,
                  "schannel: encrypted data buffer: offset %zu length %zu",
                  backend->encdata.offset, backend->encdata.দৈর্ঘ্য));

    /* read encrypted data from socket */
    ফলাফল = Curl_conn_cf_recv(cf->পরবর্তী, তথ্য,
                               (অক্ষর *)(backend->encdata.বাফার +
                                        backend->encdata.offset),
                               backend->encdata.দৈর্ঘ্য -
                               backend->encdata.offset,
                               &nread);
    যদি(ফলাফল) {
      যদি(ফলাফল == CURLE_AGAIN)
        SCH_DEV(infof(তথ্য, "schannel: recv returned CURLE_AGAIN"));
      অন্যথা {
        infof(তথ্য, "schannel: recv returned error %d", (পূর্ণসংখ্যা)ফলাফল);
        backend->recv_unrecoverable_err = ফলাফল;
      }
    }
    অন্যথা যদি(nread == 0) {
      backend->recv_connection_closed = TRUE;
      DEBUGF(infof(তথ্য, "schannel: server closed the connection"));
    }
    অন্যথা {
      backend->encdata.offset += nread;
      backend->encdata_is_incomplete = FALSE;
      SCH_DEV(infof(তথ্য, "schannel: encrypted data got %zu", nread));
    }
  }

  SCH_DEV(infof(তথ্য, "schannel: encrypted data buffer: offset %zu length %zu",
                backend->encdata.offset, backend->encdata.দৈর্ঘ্য));

  /* decrypt loop */
  যখন(backend->encdata.offset > 0 && sspi_status == SEC_E_OK &&
        (!len || backend->decdata.offset < len ||
         backend->recv_connection_closed)) {
    /* prepare data buffer for DecryptMessage call */
    Init_Sec_বাফার(&inbuf[0], SECBUFFER_তথ্য, backend->encdata.বাফার,
                  curlx_uztoul(backend->encdata.offset));

    /* we need 3 more empty input buffers for possible output */
    Init_Sec_বাফার(&inbuf[1], SECBUFFER_EMPTY, ফাঁকা, 0);
    Init_Sec_বাফার(&inbuf[2], SECBUFFER_EMPTY, ফাঁকা, 0);
    Init_Sec_বাফার(&inbuf[3], SECBUFFER_EMPTY, ফাঁকা, 0);
    Init_Sec_বাফার_Desc(&inbuf_desc, inbuf, 4);

    /* https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-decryptmessage
     */
    sspi_status = Curl_pSecFn->DecryptMessage(&backend->ctxt->ctxt_handle,
                                              &inbuf_desc, 0, ফাঁকা);

    /* check if everything went fine (server may want to renegotiate
       or shutdown the connection context) */
    যদি(sspi_status == SEC_E_OK || sspi_status == SEC_I_RENEGOTIATE ||
       sspi_status == SEC_I_CONTEXT_EXPIRED) {
      /* check for successfully decrypted data, even before actual
         renegotiation or shutdown of the connection context */
      যদি(inbuf[1].বাফার_Type == SECBUFFER_তথ্য) {
        SCH_DEV(infof(তথ্য, "schannel: decrypted data length: %lu",
                      inbuf[1].cb_বাফার));

        /* ensure the decode buffer fits the received amount of data */
        ফলাফল = ensure_decoding_আকার(তথ্য, &backend->decdata,
                                      inbuf[1].cb_বাফার, len);
        যদি(ফলাফল)
          যাও cleanup;

        /* copy decrypted data to internal buffer */
        আকার_2 = inbuf[1].cb_বাফার;
        যদি(আকার_2) {
          memcpy(backend->decdata.বাফার + backend->decdata.offset,
                 inbuf[1].pv_বাফার, আকার_2);
          backend->decdata.offset += আকার_2;
        }

        SCH_DEV(infof(তথ্য, "schannel: decrypted data added: %zu", আকার_2));
        SCH_DEV(infof(তথ্য,
                      "schannel: decrypted cached: offset %zu length %zu",
                      backend->decdata.offset, backend->decdata.দৈর্ঘ্য));
      }

      /* check for remaining encrypted data */
      যদি(inbuf[3].বাফার_Type == SECBUFFER_EXTRA && inbuf[3].cb_বাফার > 0) {
        SCH_DEV(infof(তথ্য, "schannel: encrypted data length: %lu",
                      inbuf[3].cb_বাফার));

        /* check if the remaining data is less than the total amount
         * and therefore begins after the already processed data
         */
        যদি(backend->encdata.offset > inbuf[3].cb_বাফার) {
          /* move remaining encrypted data forward to the beginning of
বাফার*/
          memmove(backend->encdata.বাফার,
                  (backend->encdata.বাফার + backend->encdata.offset) -
                  inbuf[3].cb_বাফার, inbuf[3].cb_বাফার);
          backend->encdata.offset = inbuf[3].cb_বাফার;
        }

        SCH_DEV(infof(তথ্য,
                      "schannel: encrypted cached: offset %zu length %zu",
                      backend->encdata.offset, backend->encdata.দৈর্ঘ্য));
      }
      অন্যথা {
        /* reset encrypted buffer offset, because there is no data remaining */
        backend->encdata.offset = 0;
      }

      /* check if server wants to renegotiate the connection context */
      যদি(sspi_status == SEC_I_RENEGOTIATE) {
        infof(তথ্য, "schannel: remote party requests renegotiation");
        যদি(ফলাফল && ফলাফল != CURLE_AGAIN) {
          infof(তথ্য, "schannel: cannot renegotiate, an error is pending");
          যাও cleanup;
        }

        backend->recv_renegotiating = TRUE;
        ফলাফল = schannel_recv_renegotiate(cf, তথ্য, SCH_RENEG_CALLER_IS_RECV);
        যদি(ফলাফল)
          যাও cleanup;

        /* now retry receiving data */
        sspi_status = SEC_E_OK;
        চালু;
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
        যদি(ফলাফল == CURLE_RECV_ERROR)
          ফলাফল = CURLE_OK;
        infof(তথ্য,
              "schannel: server close notification received (close_notify)");
        যাও cleanup;
      }
    }
    অন্যথা যদি(sspi_status == SEC_E_INCOMPLETE_MESSAGE) {
      backend->encdata_is_incomplete = TRUE;
      যদি(!ফলাফল)
        ফলাফল = CURLE_AGAIN;
      SCH_DEV(infof(তথ্য, "schannel: failed to decrypt data, need more data"));
      যাও cleanup;
    }
    অন্যথা {
      অক্ষর বাফার[STRERROR_LEN];
      failf(তথ্য, "schannel: failed to read data from server: %s",
            Curl_sspi_strerror(sspi_status, বাফার, আকার(বাফার)));
      ফলাফল = CURLE_RECV_ERROR;
      যাও cleanup;
    }
  }

  SCH_DEV(infof(তথ্য, "schannel: encrypted data buffer: offset %zu length %zu",
                backend->encdata.offset, backend->encdata.দৈর্ঘ্য));

  SCH_DEV(infof(তথ্য, "schannel: decrypted data buffer: offset %zu length %zu",
                backend->decdata.offset, backend->decdata.দৈর্ঘ্য));

cleanup:
  /* Warning- there is no guarantee the encdata state is valid at this point */
  SCH_DEV(infof(তথ্য, "schannel: schannel_recv cleanup"));

  /* Error if the connection has closed without a close_notify.

     The behavior here is a matter of debate. We do not want to be vulnerable
     to a truncation attack however there is some browser precedent for
     ignoring the close_notify for compatibility reasons. */
  যদি(len && !backend->decdata.offset && backend->recv_connection_closed &&
     !backend->recv_sspi_close_notify) {
    ফলাফল = CURLE_RECV_ERROR;
    failf(তথ্য, "schannel: server closed abruptly (missing close_notify)");
  }

  /* Any error other than CURLE_AGAIN is an unrecoverable error. */
  যদি(ফলাফল && ফলাফল != CURLE_AGAIN)
    backend->recv_unrecoverable_err = ফলাফল;

  আকার_2 = len < backend->decdata.offset ? len : backend->decdata.offset;
  যদি(আকার_2) {
    memcpy(buf, backend->decdata.বাফার, আকার_2);
    memmove(backend->decdata.বাফার, backend->decdata.বাফার + আকার_2,
            backend->decdata.offset - আকার_2);
    backend->decdata.offset -= আকার_2;
    SCH_DEV(infof(তথ্য, "schannel: decrypted data returned %zu", আকার_2));
    SCH_DEV(infof(তথ্য,
                  "schannel: decrypted data buffer: offset %zu length %zu",
                  backend->decdata.offset, backend->decdata.দৈর্ঘ্য));
    *pnread = আকার_2;
    ফেরত CURLE_OK;
  }

  যদি(!ফলাফল && !backend->recv_connection_closed)
    ফলাফল = CURLE_AGAIN;

  /* it is debatable what to return when !len. We could return whatever error
     we got from decryption but instead we override here so the return is
     consistent. */
  যদি(!len)
    ফেরত CURLE_OK;

  ফেরত ফলাফল;
}

স্থির bool schannel_তথ্য_pending(কাঠামো Curl_cfilter *cf,
                                  ধ্রুবক কাঠামো Curl_easy *তথ্য)
{
  ধ্রুবক কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো schannel_ssl_backend_তথ্য *backend =
    (কাঠামো schannel_ssl_backend_তথ্য *)connssl->backend;

  (শূন্য)তথ্য;
  DEBUGASSERT(backend);

  যদি(backend->ctxt) /* SSL/TLS is in use */
    ফেরত backend->decdata.offset > 0 ||
           (backend->encdata.offset > 0 && !backend->encdata_is_incomplete) ||
           backend->recv_connection_closed ||
           backend->recv_sspi_close_notify ||
           backend->recv_unrecoverable_err;
  অন্যথা
    ফেরত FALSE;
}

/* shut down the SSL connection and clean up related memory.
   this function can be called multiple times on the same connection including
   if the SSL connection failed (eg connection made but failed handshake). */
স্থির CURLcode schannel_shutdown(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য,
                                  bool send_shutdown, bool *done)
{
  /* See https://learn.microsoft.com/windows/win32/secauthn/shutting-down-an-schannel-connection
   * Shutting Down an Schannel Connection
   */
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো schannel_ssl_backend_তথ্য *backend =
    (কাঠামো schannel_ssl_backend_তথ্য *)connssl->backend;
  CURLcode ফলাফল = CURLE_OK;

  যদি(cf->shutdown) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  DEBUGASSERT(তথ্য);
  DEBUGASSERT(backend);

  /* Not supported in schannel */
  (শূন্য)send_shutdown;

  *done = FALSE;
  যদি(backend->ctxt) {
    infof(তথ্য, "schannel: shutting down SSL/TLS connection with %s port %d",
          connssl->peer.origin->hostname, connssl->peer.origin->port);
  }

  যদি(!backend->ctxt || cf->shutdown) {
    *done = TRUE;
    যাও out;
  }

  যদি(backend->cred && backend->ctxt && !backend->sent_shutdown &&
     !backend->shutdown_generated) {
    Sec_বাফার_Desc BuffDesc;
    Sec_বাফার বাফার_2;
    SECURITY_STATUS sspi_status;
    Sec_বাফার outbuf;
    Sec_বাফার_Desc outbuf_desc;
    DWORD dwshut = SCHANNEL_SHUTDOWN;

    Init_Sec_বাফার(&বাফার_2, SECBUFFER_TOKEN, &dwshut, আকার(dwshut));
    Init_Sec_বাফার_Desc(&BuffDesc, &বাফার_2, 1);

    sspi_status = Curl_pSecFn->ApplyControlToken(&backend->ctxt->ctxt_handle,
                                                 &BuffDesc);

    যদি(sspi_status != SEC_E_OK) {
      অক্ষর বাফার[STRERROR_LEN];
      failf(তথ্য, "schannel: ApplyControlToken failure: %s",
            Curl_sspi_strerror(sspi_status, বাফার, আকার(বাফার)));
      ফলাফল = CURLE_SEND_ERROR;
      যাও out;
    }

    /* setup output buffer */
    Init_Sec_বাফার(&outbuf, SECBUFFER_EMPTY, ফাঁকা, 0);
    Init_Sec_বাফার_Desc(&outbuf_desc, &outbuf, 1);

    sspi_status = Curl_pSecFn->InitializeSecurityContext(
      &backend->cred->cred_handle,
      &backend->ctxt->ctxt_handle,
      backend->cred->sni_hostname,
      backend->req_flags,
      0,
      0,
      ফাঁকা,
      0,
      &backend->ctxt->ctxt_handle,
      &outbuf_desc,
      &backend->ret_flags, ফাঁকা);

    যদি((sspi_status == SEC_E_OK) || (sspi_status == SEC_I_CONTEXT_EXPIRED)) {
      backend->shutdown_token = outbuf;
      backend->shutdown_generated = TRUE;
    }
    অন্যথা {
      অক্ষর বাফার[STRERROR_LEN];
      যদি(outbuf.pv_বাফার)
        Curl_pSecFn->Free_Context_বাফার(outbuf.pv_বাফার);
      failf(তথ্য, "schannel: generating close msg failed: %s",
            Curl_sspi_strerror(sspi_status, বাফার, আকার(বাফার)));
      ফলাফল = CURLE_SEND_ERROR;
      যাও out;
    }
  }

  যদি(backend->shutdown_generated && !backend->sent_shutdown) {
    size_t written = 0;
    size_t remaining;

    DEBUGASSERT(backend->shutdown_offset <= backend->shutdown_token.cb_বাফার);
    remaining = backend->shutdown_token.cb_বাফার - backend->shutdown_offset;

    যদি(remaining) {
      ফলাফল = Curl_conn_cf_send(cf->পরবর্তী, তথ্য,
                                 ((uint8_t *)backend->shutdown_token.pv_বাফার +
                                  backend->shutdown_offset),
                                 remaining, FALSE, &written);
      যদি(!ফলাফল)
        backend->shutdown_offset += written;
      যদি(ফলাফল == CURLE_AGAIN || (!ফলাফল && written < remaining)) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        ফলাফল = CURLE_OK;
        যাও out;
      }
    }
    অন্যথা /* no shutdown message to send? */
      ফলাফল = CURLE_OK;
    যদি(ফলাফল) {
      যদি(backend->recv_connection_closed) {
        /* The server already closed, so sending close_notify is optional. */
        *done = TRUE;
        ফলাফল = CURLE_OK;
      }
      অন্যথা {
        failf(তথ্য, "schannel: error sending close msg: %d", (পূর্ণসংখ্যা)ফলাফল);
        ফলাফল = CURLE_SEND_ERROR;
      }
      যাও out;
    }
    যদি(backend->shutdown_token.pv_বাফার)
      Curl_pSecFn->Free_Context_বাফার(backend->shutdown_token.pv_বাফার);
    memset(&backend->shutdown_token, 0, আকার(backend->shutdown_token));
    backend->shutdown_offset = 0;
    backend->sent_shutdown = TRUE;
    *done = TRUE;
  }

  /* If the connection seems open and we have not seen the close notify
   * from the server yet, try to receive it. */
  যদি(backend->cred && backend->ctxt &&
     !backend->recv_sspi_close_notify && !backend->recv_connection_closed) {
    অক্ষর বাফার[1024];
    size_t nread;

    ফলাফল = schannel_recv(cf, তথ্য, বাফার, আকার(বাফার), &nread);
    যদি(ফলাফল == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
    }
    অন্যথা যদি(ফলাফল) {
      CURL_TRC_CF(তথ্য, cf, "SSL shutdown, error %d", (পূর্ণসংখ্যা)ফলাফল);
      ফলাফল = CURLE_RECV_ERROR;
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
  cf->shutdown = (ফলাফল || *done);
  ফেরত ফলাফল;
}

স্থির শূন্য schannel_close(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো schannel_ssl_backend_তথ্য *backend =
    (কাঠামো schannel_ssl_backend_তথ্য *)connssl->backend;

  DEBUGASSERT(তথ্য);
  DEBUGASSERT(backend);

  যদি(backend->shutdown_token.pv_বাফার)
    Curl_pSecFn->Free_Context_বাফার(backend->shutdown_token.pv_বাফার);
  memset(&backend->shutdown_token, 0, আকার(backend->shutdown_token));
  backend->shutdown_offset = 0;

  /* free SSPI Schannel API security context handle */
  যদি(backend->ctxt) {
    যদি(cf->conn->sslContext == &backend->ctxt->ctxt_handle)
      cf->conn->sslContext = ফাঁকা;

    DEBUGF(infof(তথ্য, "schannel: clear security context handle"));
    Curl_pSecFn->DeleteSecurityContext(&backend->ctxt->ctxt_handle);
    curlx_safefree(backend->ctxt);
  }

  /* free SSPI Schannel API credential handle */
  যদি(backend->cred) {
    Curl_ssl_scache_lock(তথ্য);
    schannel_session_free(backend->cred);
    Curl_ssl_scache_unlock(তথ্য);
    backend->cred = ফাঁকা;
  }

  /* free the buffer used to encrypt outgoing data */
  curlx_safefree(backend->send_বাফার);
  backend->send_বাফার_len = 0;

  /* free internal buffer for received encrypted data */
  যদি(backend->encdata.বাফার) {
    curlx_safefree(backend->encdata.বাফার);
    backend->encdata.দৈর্ঘ্য = 0;
    backend->encdata.offset = 0;
    backend->encdata_is_incomplete = FALSE;
  }

  /* free internal buffer for received decrypted data */
  যদি(backend->decdata.বাফার) {
    curlx_safefree(backend->decdata.বাফার);
    backend->decdata.দৈর্ঘ্য = 0;
    backend->decdata.offset = 0;
  }
}

স্থির পূর্ণসংখ্যা schannel_init(শূন্য)
{
  প্রকারলেখ ধ্রুবক অক্ষর *(APIENTRY *WINE_GET_VERSION_FN)(শূন্য);
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
    ধ্রুবক অক্ষর *wine_version = p_wine_get_version();  /* e.g. "6.0.2" */
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

  ফেরত Curl_sspi_global_init() == CURLE_OK ? 1 : 0;
}

স্থির শূন্য schannel_cleanup(শূন্য)
{
  Curl_sspi_global_cleanup();
}

স্থির size_t schannel_version(অক্ষর *বাফার, size_t আকার_2)
{
  ফেরত curl_msnprintf(বাফার, আকার_2, "Schannel");
}

স্থির CURLcode schannel_random(কাঠামো Curl_easy *তথ্য,
                                unsigned অক্ষর *entropy, size_t দৈর্ঘ্য)
{
  (শূন্য)তথ্য;

  ফেরত curlx_win32_random(entropy, দৈর্ঘ্য);
}

স্থির CURLcode schannel_checksum(ধ্রুবক unsigned অক্ষর *প্রবেশ,
                                  size_t inputlen,
                                  unsigned অক্ষর *checksum,
                                  size_t checksumlen,
                                  DWORD provType,
                                  ধ্রুবক unsigned পূর্ণসংখ্যা algId)
{
  CURLcode ফলাফল = CURLE_FAILED_INIT;

  HCRYPTPROV hProv = 0;
  HCRYPTHASH hHash = 0;

  size_t off;

  DWORD cb_Hash_আকার;
  DWORD dw_Hash_আকার_Len;
  DWORD dwChecksumLen;

  /* since this can fail in multiple ways, zero memory first so we never
   * return old data
   */
  memset(checksum, 0, checksumlen);

  যদি(!CryptAcquireContext(&hProv, ফাঁকা, ফাঁকা, provType,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    যাও out;

  যদি(!CryptCreateHash(hProv, algId, 0, 0, &hHash))
    যাও out;

  ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;

  off = 0;
  যখন(off < inputlen) {
    DWORD chunk = (DWORD)CURLMIN(inputlen - off, 0xffffffffUL);
    যদি(!Crypt_Hash_তথ্য(hHash, প্রবেশ + off, chunk, 0))
      যাও out;
    off += chunk;
  }

  /* get hash size */
  cb_Hash_আকার = 0;
  dw_Hash_আকার_Len = (DWORD)আকার(cb_Hash_আকার);
  যদি(!CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE *)&cb_Hash_আকার,
                        &dw_Hash_আকার_Len, 0))
    যাও out;

  /* check if hash fits into the return buffer */
  যদি(checksumlen < cb_Hash_আকার)
    যাও out;

  dwChecksumLen = (DWORD)checksumlen;
  যদি(CryptGetHashParam(hHash, HP_HASHVAL, checksum, &dwChecksumLen, 0) &&
     dwChecksumLen == cb_Hash_আকার)
    ফলাফল = CURLE_OK;

out:
  যদি(hHash)
    CryptDestroyHash(hHash);

  যদি(hProv)
    CryptReleaseContext(hProv, 0);

  ফেরত ফলাফল;
}

স্থির CURLcode schannel_sha256sum(ধ্রুবক unsigned অক্ষর *প্রবেশ,
                                   size_t inputlen,
                                   unsigned অক্ষর *sha256sum,
                                   size_t sha256len)
{
  ফেরত schannel_checksum(প্রবেশ, inputlen, sha256sum, sha256len,
                           PROV_RSA_AES, CALG_SHA_256);
}

স্থির শূন্য *schannel_get_internals(কাঠামো ssl_connect_তথ্য *connssl,
                                    CURLINFO info)
{
  কাঠামো schannel_ssl_backend_তথ্য *backend =
    (কাঠামো schannel_ssl_backend_তথ্য *)connssl->backend;
  (শূন্য)info;
  DEBUGASSERT(backend);
  ফেরত &backend->ctxt->ctxt_handle;
}

HCERTSTORE Curl_schannel_get_cached_cert_store(কাঠামো Curl_cfilter *cf,
                                               কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  কাঠামো Curl_multi *multi = তথ্য->multi;
  ধ্রুবক কাঠামো curl_blob *ca_info_blob = conn_config->ca_info_blob;
  কাঠামো schannel_cert_share *share;
  timediff_t timeout_ms;
  unsigned অক্ষর info_blob_digest[CURL_SHA256_DIGEST_দৈর্ঘ্য];

  DEBUGASSERT(multi);

  যদি(!multi) {
    ফেরত ফাঁকা;
  }

  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_SCHANNEL_CERT_SHARE_চাবি,
                         CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_চাবি));
  যদি(!share || !share->cert_store) {
    ফেরত ফাঁকা;
  }

  /* zero ca_cache_timeout completely disables caching */
  যদি(!তথ্য->set.ssl_ca_cache_timeout) {
    ফেরত ফাঁকা;
  }

  /* check for cache timeout by using the cached_x509_store_expired timediff
     calculation pattern from openssl.c.
     negative timeout means retain forever. */
  timeout_ms = তথ্য->set.ssl_ca_cache_timeout * (timediff_t)1000;
  যদি(timeout_ms >= 0) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(তথ্য), &share->time);
    যদি(elapsed_ms >= timeout_ms) {
      ফেরত ফাঁকা;
    }
  }

  যদি(ca_info_blob) {
    যদি(share->CAinfo_blob_আকার != ca_info_blob->len) {
      ফেরত ফাঁকা;
    }
    যদি(schannel_sha256sum((ধ্রুবক unsigned অক্ষর *)ca_info_blob->তথ্য,
                          ca_info_blob->len,
                          info_blob_digest,
                          CURL_SHA256_DIGEST_দৈর্ঘ্য))
      ফেরত ফাঁকা;
    যদি(memcmp(share->CAinfo_blob_digest, info_blob_digest,
              CURL_SHA256_DIGEST_দৈর্ঘ্য)) {
      ফেরত ফাঁকা;
    }
  }
  অন্যথা {
    যদি(!conn_config->CAfile || !share->CAfile ||
       strcmp(share->CAfile, conn_config->CAfile)) {
      ফেরত ফাঁকা;
    }
  }

  ফেরত share->cert_store;
}

স্থির শূন্য schannel_cert_share_free(ধ্রুবক শূন্য *চাবি, size_t চাবি_len, শূন্য *p)
{
  কাঠামো schannel_cert_share *share = p;
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

bool Curl_schannel_set_cached_cert_store(কাঠামো Curl_cfilter *cf,
                                         কাঠামো Curl_easy *তথ্য,
                                         HCERTSTORE cert_store)
{
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  কাঠামো Curl_multi *multi = তথ্য->multi;
  ধ্রুবক কাঠামো curl_blob *ca_info_blob = conn_config->ca_info_blob;
  কাঠামো schannel_cert_share *share;
  unsigned অক্ষর digest[CURL_SHA256_DIGEST_দৈর্ঘ্য];
  অক্ষর *CAfile = ফাঁকা;

  DEBUGASSERT(multi);

  যদি(!multi) {
    ফেরত FALSE;
  }

  যদি(ca_info_blob) {
    যদি(schannel_sha256sum((ধ্রুবক unsigned অক্ষর *)ca_info_blob->তথ্য,
                          ca_info_blob->len, digest, আকার(digest))) {
      ফেরত FALSE;
    }
  }
  অন্যথা যদি(conn_config->CAfile) {
    CAfile = curlx_strdup(conn_config->CAfile);
    যদি(!CAfile) {
      ফেরত FALSE;
    }
  }

  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_SCHANNEL_CERT_SHARE_চাবি,
                         CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_চাবি));
  যদি(!share) {
    share = curlx_calloc(1, আকার(*share));
    যদি(!share) {
      curlx_free(CAfile);
      ফেরত FALSE;
    }
    যদি(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_SCHANNEL_CERT_SHARE_চাবি,
                       CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_চাবি),
                       share, schannel_cert_share_free)) {
      curlx_free(share);
      curlx_free(CAfile);
      ফেরত FALSE;
    }
  }

  /* free old cache data */
  যদি(share->cert_store) {
    CertCloseStore(share->cert_store, 0);
  }
  curlx_free(share->CAfile);

  যদি(ca_info_blob) {
    memcpy(share->CAinfo_blob_digest, digest, আকার(digest));
    share->CAinfo_blob_আকার = ca_info_blob->len;
  }
  অন্যথা
    share->CAinfo_blob_আকার = 0;

  share->time = curlx_now();
  share->cert_store = cert_store;
  share->CAfile = CAfile;
  ফেরত TRUE;
}

ধ্রুবক কাঠামো Curl_ssl Curl_ssl_schannel = {
  { CURLSSLBACKEND_SCHANNEL, "schannel" }, /* info */

  SSLSUPP_CERTINFO |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_CA_CACHE |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST,

  আকার(কাঠামো schannel_ssl_backend_তথ্য),

  schannel_init,                     /* init */
  schannel_cleanup,                  /* cleanup */
  schannel_version,                  /* version */
  schannel_shutdown,                 /* shutdown */
  schannel_তথ্য_pending,             /* data_pending */
  schannel_random,                   /* random */
  ফাঁকা,                              /* cert_status_request */
  schannel_connect,                  /* connect */
  Curl_ssl_adjust_pollset,           /* adjust_pollset */
  schannel_get_internals,            /* get_internals */
  schannel_close,                    /* close_one */
  ফাঁকা,                              /* close_all */
  ফাঁকা,                              /* engines_list */
  schannel_sha256sum,                /* sha256sum */
  schannel_recv,                     /* recv decrypted data */
  schannel_send,                     /* send data to encrypt */
  ফাঁকা,                              /* get_channel_binding */
};

#endif /* USE_SCHANNEL */
