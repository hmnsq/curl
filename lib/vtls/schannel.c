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
#ضمن "curl_setup.h"

#اگر_تعریف_شده USE_SCHANNEL

#اگر_تعریف_نشده USE_WINDOWS_SSPI
#خطا "cannot compile Schannel support without SSPI."
#پایان_اگر

#ضمن "vtls/schannel.h"
#ضمن "vtls/schannel_int.h"
#ضمن "vtls/vtls.h"
#ضمن "vtls/vtls_int.h"
#ضمن "vtls/vtls_scache.h"
#ضمن "curl_trc.h"
#ضمن "connect.h" /* for the connect timeout */
#ضمن "curlx/strdup.h"
#ضمن "strerror.h"
#ضمن "select.h" /* for the socket readiness */
#ضمن "curlx/win32-fopen.h"
#ضمن "curlx/multibyte.h"
#ضمن "vtls/x509asn1.h"
#ضمن "curlx/version_win32.h"
#ضمن "curlx/winapi.h"
#ضمن "curlx/strparse.h"
#ضمن "progress.h"
#ضمن "curl_sha256.h"

/* Some verbose debug messages are wrapped by SCH_DEV() instead of DEBUGF()
 * and only shown if CURL_SCHANNEL_DEV_DEBUG was defined at build time. These
 * messages are extra verbose and intended for curl developers debugging
 * Schannel recv decryption and renegotiation.
 */
#اگر_تعریف_شده CURL_SCHANNEL_DEV_DEBUG
#تعریف_کن SCH_DEV(x) x
#تعریف_کن SCH_DEV_SHOWBOOL(x)                                     \
  افعل {                                                          \
    infof(داده, "schannel: " #x " %s", (x) ? "TRUE" : "FALSE"); \
  } طالما(0)
#وإلا
#تعریف_کن SCH_DEV(x) افعل {} طالما(0)
#تعریف_کن SCH_DEV_SHOWBOOL(x) افعل {} طالما(0)
#پایان_اگر

/* Offered by mingw-w64 v8+, MS SDK 7.0A/VS2010+ */
#اگر_تعریف_نشده SP_PROT_TLS1_0_CLIENT
#تعریف_کن SP_PROT_TLS1_0_CLIENT           SP_PROT_TLS1_CLIENT
#پایان_اگر
#اگر_تعریف_نشده SP_PROT_TLS1_1_CLIENT
#تعریف_کن SP_PROT_TLS1_1_CLIENT           0x00000200
#پایان_اگر
#اگر_تعریف_نشده SP_PROT_TLS1_2_CLIENT
#تعریف_کن SP_PROT_TLS1_2_CLIENT           0x00000800
#پایان_اگر

/* Offered by mingw-w64 v8+, MS SDK 10.0.15063.0/VS2017 15.1+ */
#اگر_تعریف_نشده SP_PROT_TLS1_3_CLIENT
#تعریف_کن SP_PROT_TLS1_3_CLIENT           0x00002000
#پایان_اگر
/* Offered by mingw-w64 v8+, MS SDK 8.1/VS2013+ */
#اگر_تعریف_نشده SCH_USE_STRONG_CRYPTO
#تعریف_کن SCH_USE_STRONG_CRYPTO           0x00400000
#پایان_اگر

/* Offered by mingw-w64 v10+, MS SDK 7.0A/VS2010+ */
#اگر_تعریف_نشده SECBUFFER_ALERT
#تعریف_کن SECBUFFER_ALERT                 17
#پایان_اگر

/* Both schannel buffer sizes must be > 0 */
#تعریف_کن CURL_SCHANNEL_میانگیر_INIT_اندازه   4096
#تعریف_کن CURL_SCHANNEL_میانگیر_FREE_اندازه   1024

#تعریف_کن CERT_THUMBPRINT_STR_LEN 40
#تعریف_کن CERT_THUMBPRINT_داده_LEN 20

/* Uncomment to force verbose output
 * #define infof(x, y, ...) curl_mprintf(y, __VA_ARGS__)
 * #define failf(x, y, ...) curl_mprintf(y, __VA_ARGS__)
 */

/* Offered by mingw-w64 v4+, MS SDK 6.0A/VS2008+ */
#اگر_تعریف_نشده PKCS12_NO_PERSIST_کلید
#تعریف_کن PKCS12_NO_PERSIST_کلید 0x00008000
#پایان_اگر

/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#اگر_تعریف_نشده CERT_پیدا_کن_HAS_خصوصی_کلید
#تعریف_کن CERT_پیدا_کن_HAS_خصوصی_کلید (21 << CERT_COMPARE_لغزش)
#پایان_اگر

/* key to use at `multi->proto_hash` */
#تعریف_کن MPROTO_SCHANNEL_CERT_SHARE_کلید   "tls:schannel:cert:share"

ساكن bool s_win_has_alpn;

/* Offered by mingw-w64 v9+, MS SDK 8.1/VS2013+ */
#اگر_تعریف_نشده SECBUFFER_APPLICATION_PROTOCOLS
#تعریف_کن SECBUFFER_APPLICATION_PROTOCOLS 18
#تعریف_کن SECPKG_ATTR_APPLICATION_PROTOCOL 35

تعريفنوع تعداد {
  SecApplicationProtocolNegotiationExt_None,
  SecApplicationProtocolNegotiationExt_NPN,
  SecApplicationProtocolNegotiationExt_ALPN
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT;

تعريفنوع تعداد {
  Sec_Application_Protocol_Negotiation_وضعیت_None,
  Sec_Application_Protocol_Negotiation_وضعیت_Success,
  Sec_Application_Protocol_Negotiation_وضعیت_Selected_Client_Only
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_وضعیت;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
تعريفنوع بنية {
  SEC_APPLICATION_PROTOCOL_NEGOTIATION_وضعیت Proto_Nego_وضعیت;
  SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT ProtoNegoExt;
  بیعلامت حرف Protocol_Id_اندازه;
  بیعلامت حرف ProtocolId[0xff];
} SecPkgContext_ApplicationProtocol;
#پایان_اگر

ساكن فراغ Init_Sec_میانگیر(Sec_میانگیر *میانگیر, بیعلامت دراز BufType,
                          فراغ *Buf_داده_Ptr, بیعلامت دراز Buf_Byte_اندازه)
{
  میانگیر->cb_میانگیر = Buf_Byte_اندازه;
  میانگیر->میانگیر_Type = BufType;
  میانگیر->pv_میانگیر = Buf_داده_Ptr;
}

ساكن فراغ Init_Sec_میانگیر_Desc(Sec_میانگیر_Desc *desc, Sec_میانگیر *BufArr,
                              بیعلامت دراز رقم_Arr_Elem)
{
  desc->ul_نسخه = SECBUFFER_نسخه;
  desc->pBuffers = BufArr;
  desc->cBuffers = رقم_Arr_Elem;
}

ساكن CURLcode schannel_مجموعه_ssl_نسخه_min_max(DWORD *enabled_protocols,
                                                 بنية Curl_cfilter *cf,
                                                 بنية Curl_easy *داده)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  دراز ssl_نسخه = conn_config->نسخه_2;
  دراز ssl_نسخه_max = (دراز)conn_config->نسخه_max;
  دراز i = ssl_نسخه;

  بدل(ssl_نسخه_max) {
  حالة CURL_SSLVERSION_MAX_NONE:
  حالة CURL_SSLVERSION_MAX_DEFAULT:

    /* Windows Server 2022 and newer (including Windows 11) support TLS 1.3
       built-in. Previous builds of Windows 10 had broken TLS 1.3
       implementations that could be enabled via registry. */
    إذا(curlx_verify_windows_نسخه(10, 0, 20348, PLATFORM_WINNT,
                                    نسخه_GREATER_THAN_EQUAL)) {
      ssl_نسخه_max = CURL_SSLVERSION_MAX_TLSv1_3;
    }
    وإلا /* Windows 10 or older */
      ssl_نسخه_max = CURL_SSLVERSION_MAX_TLSv1_2;

    اكسر;
  }

  لكل(; i <= (ssl_نسخه_max >> 16); ++i) {
    بدل(i) {
    حالة CURL_SSLVERSION_TLSv1_0:
      *enabled_protocols |= SP_PROT_TLS1_0_CLIENT;
      اكسر;
    حالة CURL_SSLVERSION_TLSv1_1:
      *enabled_protocols |= SP_PROT_TLS1_1_CLIENT;
      اكسر;
    حالة CURL_SSLVERSION_TLSv1_2:
      *enabled_protocols |= SP_PROT_TLS1_2_CLIENT;
      اكسر;
    حالة CURL_SSLVERSION_TLSv1_3:

      /* Windows Server 2022 or newer */
      إذا(curlx_verify_windows_نسخه(10, 0, 20348, PLATFORM_WINNT,
                                      نسخه_GREATER_THAN_EQUAL)) {
        *enabled_protocols |= SP_PROT_TLS1_3_CLIENT;
        اكسر;
      }
      وإلا { /* Windows 10 or older */
        failf(داده, "schannel: TLS 1.3 not supported on Windows 10 or older");
        أرجع CURLE_SSL_CONNECT_ERROR;
      }
    }
  }
  أرجع CURLE_OK;
}

#تعریف_کن CIPHEROPTION(x) { #x, x }

بنية algo {
  ثابت حرف *name;
  صحيح id;
};

ساكن ثابت بنية algo algs[] = {
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
  CIPHEROPTION(CALG_SCHANNEL_MAC_کلید),
  CIPHEROPTION(CALG_SCHANNEL_ENC_کلید),
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
#اگر_تعریف_شده CALG_ECMQV
  CIPHEROPTION(CALG_ECMQV),
#پایان_اگر
  CIPHEROPTION(CALG_ECDSA),
/* Offered by mingw-w64 v7+, MS SDK 7.0A/VS2010+ */
#اگر_تعریف_شده CALG_ECDH_EPHEM
  CIPHEROPTION(CALG_ECDH_EPHEM),
#پایان_اگر
  { عدم, 0 },
};

ساكن صحيح get_alg_id_by_name(ثابت حرف *name)
{
  ثابت حرف *name_انتها = strchr(name, ':');
  size_t n = name_انتها ? (size_t)(name_انتها - name) : طول_رشته(name);
  صحيح i;

  لكل(i = 0; algs[i].name; i++) {
    إذا((n == طول_رشته(algs[i].name) && !مقایسه_رشته(algs[i].name, name, n)))
      أرجع algs[i].id;
  }
  أرجع 0; /* not found */
}

#تعریف_کن رقم_CIPHERS 47 /* There are a maximum of 47 options listed above */

ساكن CURLcode مجموعه_ssl_ciphers(SCHANNEL_CRED *schannel_cred, حرف *ciphers,
                                ALG_ID *algIds)
{
  ثابت حرف *شروع_Cur = ciphers;
  صحيح alg_شمار = 0;
  طالما(شروع_Cur && *شروع_Cur && (alg_شمار < رقم_CIPHERS)) {
    curl_off_t alg;
    إذا(curlx_str_عدد(&شروع_Cur, &alg, بیشترین_صحیح) || !alg)
      alg = get_alg_id_by_name(شروع_Cur);

    إذا(alg)
      algIds[alg_شمار++] = (ALG_ID)alg;
    وإلا إذا(!مقایسه_رشته(شروع_Cur, "USE_STRONG_CRYPTO",
                     CURL_CSTRLEN("USE_STRONG_CRYPTO")) ||
            !مقایسه_رشته(شروع_Cur, "SCH_USE_STRONG_CRYPTO",
                     CURL_CSTRLEN("SCH_USE_STRONG_CRYPTO")))
      schannel_cred->dwFlags |= SCH_USE_STRONG_CRYPTO;
    وإلا
      أرجع CURLE_SSL_CIPHER;
    شروع_Cur = strchr(شروع_Cur, ':');
    إذا(شروع_Cur)
      شروع_Cur++;
  }
  schannel_cred->palgSupportedAlgs = algIds;
  schannel_cred->cSupportedAlgs = (DWORD)alg_شمار;
  أرجع CURLE_OK;
}

/* Function allocates memory for store_path only if CURLE_OK is returned */
ساكن CURLcode get_cert_location(TCHAR *مسیر, DWORD *store_name,
                                  TCHAR **store_مسیر, TCHAR **thumbprint)
{
  TCHAR *sep;
  TCHAR *store_مسیر_شروع;
  size_t store_name_len;

  sep = _tcschr(مسیر, متن('\\'));
  إذا(!sep)
    أرجع CURLE_SSL_CERTPROBLEM;

  store_name_len = sep - مسیر;

  إذا(_tcsncmp(مسیر, متن("CurrentUser"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_USER;
  وإلا إذا(_tcsncmp(مسیر, متن("LocalMachine"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE;
  وإلا إذا(_tcsncmp(مسیر, متن("CurrentService"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_SERVICE;
  وإلا إذا(_tcsncmp(مسیر, متن("Services"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_SERVICES;
  وإلا إذا(_tcsncmp(مسیر, متن("Users"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_USERS;
  وإلا إذا(_tcsncmp(مسیر, متن("CurrentUserGroupPolicy"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_USER_گروه_POLICY;
  وإلا إذا(_tcsncmp(مسیر, متن("LocalMachineGroupPolicy"), store_name_len) ==
          0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE_گروه_POLICY;
  وإلا إذا(_tcsncmp(مسیر, متن("LocalMachineEnterprise"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE_ENTERPRISE;
  وإلا
    أرجع CURLE_SSL_CERTPROBLEM;

  store_مسیر_شروع = sep + 1;

  sep = _tcschr(store_مسیر_شروع, متن('\\'));
  إذا(!sep)
    أرجع CURLE_SSL_CERTPROBLEM;

  *thumbprint = sep + 1;
  إذا(_tcslen(*thumbprint) != CERT_THUMBPRINT_STR_LEN)
    أرجع CURLE_SSL_CERTPROBLEM;

  *sep = متن('\0');
  *store_مسیر = curlx_tcsdup(store_مسیر_شروع);
  *sep = متن('\\');
  إذا(!*store_مسیر)
    أرجع CURLE_بیرون_از_حافظه;

  أرجع CURLE_OK;
}

ساكن CURLcode get_client_cert(بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده,
                                HCERTSTORE *بیرون_cert_store,
                                PCCERT_CONTEXT *بیرون_cert_context)
{
  بنية ssl_filter_config *sslc = Curl_ssl_cf_get_filter_config(cf);
  PCCERT_CONTEXT client_cert = عدم;
  HCERTSTORE client_cert_store = عدم;
  CURLcode نتیجه = CURLE_OK;

  إذا(sslc->clientcert || sslc->cert_blob) {
    DWORD cert_store_name = 0;
    TCHAR *cert_store_مسیر = عدم;
    TCHAR *cert_thumbprint_str = عدم;
    TCHAR cert_thumbprint_buf[CERT_THUMBPRINT_STR_LEN + 1];
    CRYPT_HASH_BLOB cert_thumbprint;
    BYTE cert_thumbprint_داده[CERT_THUMBPRINT_داده_LEN];
    HCERTSTORE cert_store = عدم;
    FILE *f_درون_Cert = عدم;
    فراغ *certdata = عدم;
    size_t certsize = 0;
    bool blob = !!sslc->cert_blob;

    إذا(blob) {
      certdata = sslc->cert_blob->داده;
      certsize = sslc->cert_blob->len;
    }
    وإلا {
      TCHAR *cert_مسیر =
        curlx_convert_UTF8_مقصد_tchar(sslc->clientcert);
      إذا(!cert_مسیر)
        أرجع CURLE_بیرون_از_حافظه;

      نتیجه = get_cert_location(cert_مسیر, &cert_store_name,
                                 &cert_store_مسیر, &cert_thumbprint_str);

      /* 'cert_thumbprint_str' points in to the allocated 'cert_path', copy
         the data. The string is verified to be CERT_THUMBPRINT_STR_LEN bytes
         long within the get_cert_location() function. */
      إذا(!نتیجه && cert_thumbprint_str) {
        کپی_حافظه(cert_thumbprint_buf, cert_thumbprint_str,
               حجم(cert_thumbprint_buf));
        cert_thumbprint_str = cert_thumbprint_buf;
      }

      curlx_free(cert_مسیر);
      إذا(نتیجه && (sslc->clientcert[0] != '\0'))
        f_درون_Cert = curlx_fopen(sslc->clientcert, "rb");

      إذا(نتیجه && !f_درون_Cert) {
        failf(داده, "schannel: Failed to get certificate location"
              " or file for %s",
              sslc->clientcert);
        أرجع نتیجه;
      }
    }

    إذا((f_درون_Cert || blob) && sslc->cert_type &&
       !curl_strequal(sslc->cert_type, "P12")) {
      failf(داده, "schannel: certificate format compatibility error "
            "for %s",
            blob ? "(memory blob)" : sslc->clientcert);
      curlx_free(cert_store_مسیر);
      إذا(f_درون_Cert)
        curlx_fclose(f_درون_Cert);
      أرجع CURLE_SSL_CERTPROBLEM;
    }

    إذا(f_درون_Cert || blob) {
      /* Reading a .p12 or .pfx file, like the example at bottom of
         https://learn.microsoft.com/archive/msdn-technet-forums/3e7bc95f-b21a-4bcd-bd2c-7f996718cae5
       */
      CRYPT_داده_BLOB datablob;
      WCHAR *pszPassword;
      size_t pwd_len = 0;
      صحيح cert_پیدا_کن_flags;
      ثابت حرف *cert_showfilename_error = blob ?
        "(memory blob)" : sslc->clientcert;
      curlx_free(cert_store_مسیر);
      إذا(f_درون_Cert) {
        دراز cert_tell = 0;
        bool continue_reading = fseek(f_درون_Cert, 0, SEEK_END) == 0;
        إذا(continue_reading) {
          cert_tell = ftell(f_درون_Cert);
          إذا(cert_tell < 0)
            continue_reading = FALSE;
          وإلا
            certsize = (size_t)cert_tell;
        }
        إذا(continue_reading)
          continue_reading = fseek(f_درون_Cert, 0, SEEK_SET) == 0;
        إذا(continue_reading && (certsize < CURL_MAX_درون_داد_درازی))
          certdata = curlx_malloc(certsize + 1);
        إذا(!certdata || ((صحيح)fread(certdata, certsize, 1, f_درون_Cert) != 1))
          continue_reading = FALSE;
        curlx_fclose(f_درون_Cert);
        إذا(!continue_reading) {
          failf(داده, "schannel: Failed to read cert file %s",
                sslc->clientcert);
          curlx_free(certdata);
          أرجع CURLE_SSL_CERTPROBLEM;
        }
      }

      /* Convert key-pair data to the in-memory certificate store */
      datablob.pb_داده = (BYTE *)certdata;
      datablob.cb_داده = (DWORD)certsize;

      إذا(sslc->کلید_passwd)
        pwd_len = طول_رشته(sslc->کلید_passwd);
      pszPassword = curlx_malloc(حجم(WCHAR) * (pwd_len + 1));
      إذا(pszPassword) {
        صحيح str_w_len = 0;
        إذا(pwd_len > 0)
          str_w_len = Multi_Byte_مقصد_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                          sslc->کلید_passwd, (صحيح)pwd_len,
                                          pszPassword, (صحيح)(pwd_len + 1));

        إذا((str_w_len >= 0) && (str_w_len <= (صحيح)pwd_len))
          pszPassword[str_w_len] = 0;
        وإلا
          pszPassword[0] = 0;

        cert_store = PFXImportCertStore(&datablob, pszPassword,
                                        PKCS12_NO_PERSIST_کلید);
        curlx_memzero(pszPassword, حجم(WCHAR) * (pwd_len + 1));
        curlx_free(pszPassword);
      }
      إذا(!blob)
        curlx_free(certdata);
      إذا(!cert_store) {
        DWORD errorcode = Get_آخرین_Error();
        إذا(errorcode == ERROR_INVALID_PASSWORD)
          failf(داده, "schannel: Failed to import cert file %s, "
                "password is bad",
                cert_showfilename_error);
        وإلا
          failf(داده, "schannel: Failed to import cert file %s, "
                "last error is 0x%08lx",
                cert_showfilename_error, errorcode);
        أرجع CURLE_SSL_CERTPROBLEM;
      }

      /* CERT_FIND_HAS_PRIVATE_KEY is only available in Windows 8 / Server
         2012, (NT 6.2). For older versions we use CURL_FIND_ANY. */
      إذا(curlx_verify_windows_نسخه(6, 2, 0, PLATFORM_WINNT,
                                      نسخه_GREATER_THAN_EQUAL))
        cert_پیدا_کن_flags = CERT_پیدا_کن_HAS_خصوصی_کلید;
      وإلا
        cert_پیدا_کن_flags = CERT_پیدا_کن_ANY;

      client_cert =
        Cert_پیدا_کن_Certificate_درون_Store(cert_store,
                                   X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                   cert_پیدا_کن_flags, عدم, عدم);

      إذا(!client_cert) {
        failf(داده, "schannel: Failed to get certificate from file %s"
              ", last error is 0x%08lx",
              cert_showfilename_error, Get_آخرین_Error());
        CertCloseStore(cert_store, 0);
        أرجع CURLE_SSL_CERTPROBLEM;
      }
    }
    وإلا {
      cert_store =
        CertOpenStore(
#اگر_تعریف_شده UNICODE
                      CERT_STORE_PROV_SYSTEM_W,
#وإلا
                      CERT_STORE_PROV_SYSTEM_A,
#پایان_اگر
                      0,
                      (HCRYPTPROV)عدم,
                      CERT_STORE_OPEN_EXISTING_FLAG | cert_store_name,
                      cert_store_مسیر);
      إذا(!cert_store) {
        حرف *مسیر_utf8 =
          curlx_convert_tchar_مقصد_UTF8(cert_store_مسیر);
        failf(داده, "schannel: Failed to open cert store %lx %s, "
              "last error is 0x%08lx",
              cert_store_name, (مسیر_utf8 ? مسیر_utf8 : "(unknown)"),
              Get_آخرین_Error());
        curlx_free(cert_store_مسیر);
        curlx_free(مسیر_utf8);
        أرجع CURLE_SSL_CERTPROBLEM;
      }
      curlx_free(cert_store_مسیر);

      cert_thumbprint.pb_داده = cert_thumbprint_داده;
      cert_thumbprint.cb_داده = CERT_THUMBPRINT_داده_LEN;

      إذا(!Crypt_String_مقصد_دودویی(cert_thumbprint_str,
                              CERT_THUMBPRINT_STR_LEN,
                              CRYPT_STRING_HEX,
                              cert_thumbprint_داده,
                              &cert_thumbprint.cb_داده,
                              عدم, عدم)) {
        CertCloseStore(cert_store, 0);
        أرجع CURLE_SSL_CERTPROBLEM;
      }

      client_cert =
        Cert_پیدا_کن_Certificate_درون_Store(cert_store,
                                   X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                   CERT_پیدا_کن_HASH, &cert_thumbprint, عدم);
      إذا(!client_cert) {
        /* CRYPT_E_NOT_FOUND / E_INVALIDARG */
        CertCloseStore(cert_store, 0);
        failf(داده, "schannel: client cert not found in cert store");
        أرجع CURLE_SSL_CERTPROBLEM;
      }
    }
    client_cert_store = cert_store;
  }

  *بیرون_cert_store = client_cert_store;
  *بیرون_cert_context = client_cert;

  أرجع CURLE_OK;
}

ساكن CURLcode acquire_sspi_handle(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *داده,
                                    بنية schannel_ssl_backend_داده *backend,
                                    PCCERT_CONTEXT client_cert,
                                    DWORD flags,
                                    DWORD enabled_protocols)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SECURITY_وضعیت sspi_وضعیت = SEC_E_OK;
  CURLcode نتیجه;

  /* We support TLS 1.3 starting in Windows Server 2022 or later
   * (OS build 20348) */
  إذا(!conn_config->cipher_list &&
     curlx_verify_windows_نسخه(10, 0, 20348, PLATFORM_WINNT,
                                  نسخه_GREATER_THAN_EQUAL)) {

    SCH_CREDENTIALS credentials = { 0 };
    TLS_PARAMETERS tls_parameters = { 0 };
    CRYPTO_SETTINGS crypto_settings[1];
    PCCERT_CONTEXT client_certs[1];

    إذا(client_cert)
      client_certs[0] = client_cert;
    وإلا
      client_certs[0] = عدم;

    memset(crypto_settings, 0, حجم(crypto_settings));

    tls_parameters.pDisabledCrypto = crypto_settings;

    /* The number of blocked suites */
    tls_parameters.cDisabledCrypto = (DWORD)0;
    credentials.pTlsParameters = &tls_parameters;
    credentials.cTlsParameters = 1;

    credentials.dw_نسخه = SCH_CREDENTIALS_نسخه;
    credentials.dwFlags = flags | SCH_USE_STRONG_CRYPTO;

    credentials.pTlsParameters->grbitDisabledProtocols = ~enabled_protocols;

    إذا(client_certs[0]) {
      credentials.cCreds = 1;
      credentials.paCred = client_certs;
    }

    sspi_وضعیت =
      Curl_pSecFn->AcquireCredentialsHandle(عدم, CURL_UNCONST(UNISP_NAME),
                                            SECPKG_CRED_OUTBOUND, عدم,
                                            &credentials, عدم, عدم,
                                            &backend->cred->cred_handle, عدم);
  }
  وإلا {
    /* Pre-Windows 10 1809 or the user set a legacy algorithm list.
       Schannel does not negotiate TLS 1.3 when SCHANNEL_CRED is used. */
    ALG_ID algIds[رقم_CIPHERS];
    حرف *ciphers = conn_config->cipher_list;
    SCHANNEL_CRED schannel_cred = { 0 };
    PCCERT_CONTEXT client_certs[1];

    إذا(client_cert)
      client_certs[0] = client_cert;
    وإلا
      client_certs[0] = عدم;

    schannel_cred.dw_نسخه = SCHANNEL_CRED_نسخه;
    schannel_cred.dwFlags = flags;
    schannel_cred.grbitEnabledProtocols = enabled_protocols;

    إذا(ciphers) {
      إذا((enabled_protocols & SP_PROT_TLS1_3_CLIENT)) {
        إذا(!(enabled_protocols & ~SP_PROT_TLS1_3_CLIENT)) {
          failf(داده, "schannel: TLS 1.3 is not supported with a cipher list; "
                "remove the cipher list or allow a lower TLS version");
          أرجع CURLE_SSL_CONNECT_ERROR;
        }
        infof(داده, "schannel: WARNING: This version of Schannel "
              "negotiates a less-secure TLS version than TLS 1.3 because the "
              "user set an algorithm cipher list.");
      }
      نتیجه = مجموعه_ssl_ciphers(&schannel_cred, ciphers, algIds);
      إذا(نتیجه) {
        failf(داده, "schannel: Failed setting algorithm cipher list");
        أرجع نتیجه;
      }
    }
    وإلا {
      schannel_cred.dwFlags = flags | SCH_USE_STRONG_CRYPTO;
    }

    إذا(client_certs[0]) {
      schannel_cred.cCreds = 1;
      schannel_cred.paCred = client_certs;
    }

    sspi_وضعیت =
      Curl_pSecFn->AcquireCredentialsHandle(عدم, CURL_UNCONST(UNISP_NAME),
                                            SECPKG_CRED_OUTBOUND, عدم,
                                            &schannel_cred, عدم, عدم,
                                            &backend->cred->cred_handle, عدم);
  }

  إذا(sspi_وضعیت != SEC_E_OK) {
    حرف میانگیر[STRERROR_LEN];
    failf(داده, "schannel: AcquireCredentialsHandle failed: %s",
          Curl_sspi_strerror(sspi_وضعیت, میانگیر, حجم(میانگیر)));
    بدل(sspi_وضعیت) {
    حالة SEC_E_INSUFFICIENT_حافظه:
      أرجع CURLE_بیرون_از_حافظه;
    حالة SEC_E_NO_CREDENTIALS:
    حالة SEC_E_SECPKG_NOT_یافت:
    حالة SEC_E_NOT_OWNER:
    حالة SEC_E_UNKNOWN_CREDENTIALS:
    حالة SEC_E_داخلی_ERROR:
    افتراضي:
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
  }

  أرجع CURLE_OK;
}

ساكن CURLcode schannel_acquire_credential_handle(بنية Curl_cfilter *cf,
                                                   بنية Curl_easy *داده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  PCCERT_CONTEXT client_cert = عدم;
  HCERTSTORE client_cert_store = عدم;
  CURLcode نتیجه;

  /* setup Schannel API options */
  DWORD flags = 0;
  DWORD enabled_protocols = 0;

  بنية schannel_ssl_backend_داده *backend =
    (بنية schannel_ssl_backend_داده *)connssl->backend;

  DEBUGASSERT(backend);

  إذا(conn_config->verifypeer) {
    إذا(backend->use_manual_cred_validation)
      flags = SCH_CRED_MANUAL_CRED_VALIDATION;
    وإلا
      flags = SCH_CRED_AUTO_CRED_VALIDATION;

    إذا(conn_config->no_revoke) {
      flags |= SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
               SCH_CRED_IGNORE_REVOCATION_OFFLINE;

      DEBUGF(infof(داده, "schannel: disabled server certificate revocation "
                         "checks"));
    }
    وإلا إذا(conn_config->revoke_بهترین_effort) {
      flags |= SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
               SCH_CRED_IGNORE_REVOCATION_OFFLINE |
               SCH_CRED_REVOCATION_CHECK_CHAIN;

      DEBUGF(infof(داده, "schannel: ignore revocation offline errors"));
    }
    وإلا {
      flags |= SCH_CRED_REVOCATION_CHECK_CHAIN;

      DEBUGF(infof(داده, "schannel: checking server certificate revocation"));
    }
  }
  وإلا {
    flags = SCH_CRED_MANUAL_CRED_VALIDATION |
            SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
            SCH_CRED_IGNORE_REVOCATION_OFFLINE;
    DEBUGF(infof(داده, "schannel: disabled server cert revocation checks"));
  }

  إذا(!conn_config->verifyhost) {
    flags |= SCH_CRED_NO_SERVERNAME_CHECK;
    DEBUGF(infof(داده, "schannel: verifyhost setting prevents Schannel from "
                       "comparing the supplied target name with the subject "
                       "names in server certificates."));
  }

  إذا(!conn_config->auto_client_cert) {
    flags &= ~(DWORD)SCH_CRED_USE_DEFAULT_CREDS;
    flags |= SCH_CRED_NO_DEFAULT_CREDS;
    infof(داده, "schannel: disabled automatic use of client certificate");
  }
  وإلا
    infof(داده, "schannel: enabled automatic use of client certificate");

  DEBUGASSERT(conn_config->نسخه_2 != CURL_SSLVERSION_DEFAULT);
  بدل(conn_config->نسخه_2) {
  حالة CURL_SSLVERSION_TLSv1:
  حالة CURL_SSLVERSION_TLSv1_0:
  حالة CURL_SSLVERSION_TLSv1_1:
  حالة CURL_SSLVERSION_TLSv1_2:
  حالة CURL_SSLVERSION_TLSv1_3: {
    نتیجه = schannel_مجموعه_ssl_نسخه_min_max(&enabled_protocols, cf, داده);
    إذا(نتیجه)
      أرجع نتیجه;
    اكسر;
  }
  حالة CURL_SSLVERSION_SSLv3:
  حالة CURL_SSLVERSION_SSLv2:
    failf(داده, "SSL versions not supported");
    أرجع CURLE_NOT_BUILT_درون;
  افتراضي:
    failf(داده, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  نتیجه = get_client_cert(cf, داده, &client_cert_store, &client_cert);
  إذا(نتیجه)
    أرجع نتیجه;

  /* allocate memory for the reusable credential handle */
  backend->cred = (بنية Curl_schannel_cred *)
    curlx_calloc(1, حجم(بنية Curl_schannel_cred));
  إذا(!backend->cred) {
    failf(داده, "schannel: unable to allocate memory");

    إذا(client_cert)
      CertFreeCertificateContext(client_cert);
    إذا(client_cert_store)
      CertCloseStore(client_cert_store, 0);

    أرجع CURLE_بیرون_از_حافظه;
  }
  backend->cred->refcount = 1;

  /* Since we did not persist the key, we need to extend the store's
   * lifetime until the end of the connection
   */
  backend->cred->client_cert_store = client_cert_store;

  نتیجه = acquire_sspi_handle(cf, داده, backend, client_cert,
                               flags, enabled_protocols);

  إذا(client_cert)
    CertFreeCertificateContext(client_cert);

  أرجع نتیجه;
}

ساكن CURLcode schannel_connect_step1(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *داده)
{
  size_t written = 0;
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية schannel_ssl_backend_داده *backend =
    (بنية schannel_ssl_backend_داده *)connssl->backend;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  Sec_میانگیر outbuf;
  Sec_میانگیر_Desc outbuf_desc;
  Sec_میانگیر inbuf;
  Sec_میانگیر_Desc inbuf_desc;
  بیعلامت حرف alpn_میانگیر[128];
  SECURITY_وضعیت sspi_وضعیت = SEC_E_OK;
  CURLcode نتیجه;

  DEBUGASSERT(backend);
  DEBUGF(infof(داده, "schannel: SSL/TLS connection with %s port %d (step 1/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->پورت));

  backend->use_alpn = connssl->alpn && s_win_has_alpn;

  إذا(conn_config->CAfile || conn_config->ca_info_blob) {
    إذا(curlx_verify_windows_نسخه(6, 1, 0, PLATFORM_WINNT,
                                    نسخه_GREATER_THAN_EQUAL)) {
      backend->use_manual_cred_validation = TRUE;
    }
    وإلا {
      failf(داده, "schannel: this version of Windows is too old to support "
                  "certificate verification via CA bundle file.");
      أرجع CURLE_SSL_CACERT_BADFILE;
    }
  }
  وإلا
    backend->use_manual_cred_validation = FALSE;

  backend->cred = عدم;

  /* check for an existing reusable credential handle */
  إذا(Curl_ssl_scache_use(cf, داده)) {
    بنية Curl_schannel_cred *old_cred;
    Curl_ssl_scache_قفل(داده);
    old_cred = Curl_ssl_scache_get_obj(cf, داده, connssl->peer.scache_کلید);
    إذا(old_cred) {
      backend->cred = old_cred;
      DEBUGF(infof(داده, "schannel: reusing existing credential handle"));

      /* increment the reference counter of the credential/session handle */
      backend->cred->refcount++;
      DEBUGF(infof(داده,
                   "schannel: incremented credential handle refcount = %d",
                   backend->cred->refcount));
    }
    Curl_ssl_scache_unlock(داده);
  }

  إذا(!backend->cred) {
    حرف *snihost;
    نتیجه = schannel_acquire_credential_handle(cf, داده);
    إذا(نتیجه || !backend->cred)
      أرجع نتیجه;
    /* schannel_acquire_credential_handle() sets backend->cred accordingly or
       it returns error otherwise. */

    /* A hostname associated with the credential is needed by
       InitializeSecurityContext for SNI and other reasons. */
    snihost = connssl->peer.sni ?
      connssl->peer.sni : connssl->peer.origin->hostname;
    backend->cred->sni_hostname = curlx_convert_UTF8_مقصد_tchar(snihost);
    إذا(!backend->cred->sni_hostname)
      أرجع CURLE_بیرون_از_حافظه;
  }

  /* Warn if SNI is disabled due to use of an IP address */
  إذا(connssl->peer.type != CURL_SSL_PEER_DNS) {
    infof(داده, "schannel: using IP address, SNI is not supported by OS.");
  }

  إذا(backend->use_alpn) {
    صحيح cur = 0;
    صحيح list_شروع_شاخص = 0;
    بیعلامت صحيح *extension_len = عدم;
    بیعلامت کوتاه *list_len = عدم;
    ثابت بیعلامت صحيح uiALPN = SecApplicationProtocolNegotiationExt_ALPN;
    بنية alpn_proto_buf proto;

    /* The first four bytes is an unsigned int indicating number
       of bytes of data in the rest of the buffer. */
    extension_len = (بیعلامت صحيح *)(فراغ *)&alpn_میانگیر[cur];
    cur += (صحيح)حجم(بیعلامت صحيح);

    /* The next four bytes are an indicator that this buffer contains
       ALPN data, as opposed to NPN, for example. */
    کپی_حافظه(&alpn_میانگیر[cur], &uiALPN, حجم(uiALPN));
    cur += (صحيح)حجم(بیعلامت صحيح);

    /* The next two bytes is an unsigned short indicating the number
       of bytes used to list the preferred protocols. */
    list_len = (بیعلامت کوتاه *)(فراغ *)&alpn_میانگیر[cur];
    cur += (صحيح)حجم(بیعلامت کوتاه);

    list_شروع_شاخص = cur;

    نتیجه = Curl_alpn_مقصد_proto_buf(&proto, connssl->alpn);
    إذا(نتیجه) {
      failf(داده, "Error setting ALPN");
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
    کپی_حافظه(&alpn_میانگیر[cur], proto.داده, proto.len);
    cur += proto.len;

    *list_len = curlx_uitous(cur - list_شروع_شاخص);
    *extension_len = (بیعلامت صحيح)(*list_len +
      حجم(بیعلامت صحيح) + حجم(بیعلامت کوتاه));

    Init_Sec_میانگیر(&inbuf, SECBUFFER_APPLICATION_PROTOCOLS, alpn_میانگیر, cur);
    Init_Sec_میانگیر_Desc(&inbuf_desc, &inbuf, 1);

    Curl_alpn_مقصد_proto_str(&proto, connssl->alpn);
    infof(داده, VTLS_INFOF_ALPN_OFFER_1STR, proto.داده);
  }
  وإلا {
    Init_Sec_میانگیر(&inbuf, SECBUFFER_خالی, عدم, 0);
    Init_Sec_میانگیر_Desc(&inbuf_desc, &inbuf, 1);
  }

  /* setup output buffer */
  Init_Sec_میانگیر(&outbuf, SECBUFFER_خالی, عدم, 0);
  Init_Sec_میانگیر_Desc(&outbuf_desc, &outbuf, 1);

  /* security request flags */
  backend->req_flags =
    ISC_REQ_SEQUENCE_DETECT | ISC_REQ_REPLAY_DETECT |
    ISC_REQ_CONFIDENTIALITY | ISC_REQ_ALLOCATE_حافظه |
    ISC_REQ_جریان |
    (!conn_config->auto_client_cert ? ISC_REQ_USE_SUPPLIED_CREDS : 0);

  /* allocate memory for the security context handle */
  backend->ctxt = (بنية Curl_schannel_ctxt *)
    curlx_calloc(1, حجم(بنية Curl_schannel_ctxt));
  إذا(!backend->ctxt) {
    failf(داده, "schannel: unable to allocate memory");
    أرجع CURLE_بیرون_از_حافظه;
  }

  /* Schannel InitializeSecurityContext:
     https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-initializesecuritycontextw

     At the moment we do not pass inbuf unless we are using ALPN since we only
     use it for that, and WINE (for which we currently disable ALPN) is giving
     us problems with inbuf regardless. https://github.com/curl/curl/issues/983
   */
  sspi_وضعیت = Curl_pSecFn->InitializeSecurityContext(
    &backend->cred->cred_handle, عدم, backend->cred->sni_hostname,
    backend->req_flags, 0, 0,
    (backend->use_alpn ? &inbuf_desc : عدم),
    0, &backend->ctxt->ctxt_handle,
    &outbuf_desc, &backend->ret_flags, عدم);

  إذا(sspi_وضعیت != SEC_I_CONTINUE_NEEDED) {
    حرف میانگیر[STRERROR_LEN];
    curlx_safefree(backend->ctxt);
    بدل(sspi_وضعیت) {
    حالة SEC_E_INSUFFICIENT_حافظه:
      failf(داده, "schannel: initial InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(sspi_وضعیت, میانگیر, حجم(میانگیر)));
      أرجع CURLE_بیرون_از_حافظه;
    حالة SEC_E_WRONG_PRINCIPAL:
      failf(داده, "schannel: SNI or certificate check failed: %s",
            Curl_sspi_strerror(sspi_وضعیت, میانگیر, حجم(میانگیر)));
      أرجع CURLE_PEER_FAILED_VERIFICATION;
#إذا 0
    حالة SEC_E_INVALID_HANDLE:
    حالة SEC_E_INVALID_TOKEN:
    حالة SEC_E_LOGON_DENIED:
    حالة SEC_E_هدف_UNKNOWN:
    حالة SEC_E_NO_AUTHENTICATING_AUTHORITY:
    حالة SEC_E_داخلی_ERROR:
    حالة SEC_E_NO_CREDENTIALS:
    حالة SEC_E_UNSUPPORTED_FUNCTION:
    حالة SEC_E_APPLICATION_PROTOCOL_MISMATCH:
#پایان_اگر
    افتراضي:
      failf(داده, "schannel: initial InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(sspi_وضعیت, میانگیر, حجم(میانگیر)));
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
  }

  DEBUGF(infof(داده, "schannel: sending initial handshake data: "
               "sending %lu bytes.", outbuf.cb_میانگیر));

  /* send initial handshake data which is now stored in output buffer */
  نتیجه = Curl_conn_cf_send(cf->بعدی, داده,
                             (ثابت uint8_t *)outbuf.pv_میانگیر,
                             outbuf.cb_میانگیر, FALSE,
                             &written);
  Curl_pSecFn->Free_Context_میانگیر(outbuf.pv_میانگیر);
  إذا(نتیجه || (outbuf.cb_میانگیر != written)) {
    failf(داده, "schannel: failed to send initial handshake data: "
          "sent %zu of %lu bytes", written, outbuf.cb_میانگیر);
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGF(infof(داده, "schannel: sent initial handshake data: "
               "sent %zu bytes", written));

  backend->recv_unrecoverable_err = CURLE_OK;
  backend->recv_sspi_close_notify = FALSE;
  backend->recv_connection_closed = FALSE;
  backend->recv_renegotiating = FALSE;
  backend->renegotiate_state.started = FALSE;
  backend->encdata_is_incomplete = FALSE;
  backend->shutdown_generated = FALSE;
  backend->shutdown_جابه_جایی = 0;
  memset(&backend->shutdown_token, 0, حجم(backend->shutdown_token));

  /* continue to second handshake step */
  connssl->connecting_state = ssl_connect_2;

  أرجع CURLE_OK;
}

ساكن CURLcode schannel_error(بنية Curl_easy *داده,
                               SECURITY_وضعیت sspi_وضعیت)
{
  حرف میانگیر[STRERROR_LEN];
  بدل(sspi_وضعیت) {
  حالة SEC_E_INSUFFICIENT_حافظه:
    failf(داده, "schannel: next InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(sspi_وضعیت, میانگیر, حجم(میانگیر)));
    أرجع CURLE_بیرون_از_حافظه;
  حالة SEC_E_WRONG_PRINCIPAL:
    failf(داده, "schannel: SNI or certificate check failed: %s",
          Curl_sspi_strerror(sspi_وضعیت, میانگیر, حجم(میانگیر)));
    أرجع CURLE_PEER_FAILED_VERIFICATION;
  حالة SEC_E_UNTRUSTED_ریشه:
    failf(داده, "schannel: %s",
          Curl_sspi_strerror(sspi_وضعیت, میانگیر, حجم(میانگیر)));
    أرجع CURLE_PEER_FAILED_VERIFICATION;
#إذا 0
  حالة SEC_E_INVALID_HANDLE:
  حالة SEC_E_INVALID_TOKEN:
  حالة SEC_E_LOGON_DENIED:
  حالة SEC_E_هدف_UNKNOWN:
  حالة SEC_E_NO_AUTHENTICATING_AUTHORITY:
  حالة SEC_E_داخلی_ERROR:
  حالة SEC_E_NO_CREDENTIALS:
  حالة SEC_E_UNSUPPORTED_FUNCTION:
  حالة SEC_E_APPLICATION_PROTOCOL_MISMATCH:
#پایان_اگر
  افتراضي:
    failf(داده, "schannel: next InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(sspi_وضعیت, میانگیر, حجم(میانگیر)));
    أرجع CURLE_SSL_CONNECT_ERROR;
  }
}

ساكن CURLcode schannel_pkp_pin_peer_pubkey(بنية Curl_cfilter *cf,
                                             بنية Curl_easy *داده,
                                             ثابت حرف *pinnedpubkey)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية schannel_ssl_backend_داده *backend =
    (بنية schannel_ssl_backend_داده *)connssl->backend;
  CERT_CONTEXT *pCertContextServer = عدم;

  /* Result is returned to caller */
  CURLcode نتیجه = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  DEBUGASSERT(backend);

  /* if a path was not specified, do not pin */
  إذا(!pinnedpubkey)
    أرجع CURLE_OK;

  افعل {
    SECURITY_وضعیت sspi_وضعیت;
    uint8_t *x509_der;
    DWORD x509_der_len;
    بنية Curl_X509certificate x509_parsed;
    بنية Curl_asn1Element *pubkey;

    sspi_وضعیت =
      Curl_pSecFn->Query_Context_صفات(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                          &pCertContextServer);

    إذا((sspi_وضعیت != SEC_E_OK) || !pCertContextServer) {
      حرف میانگیر[STRERROR_LEN];
      failf(داده, "schannel: Failed to read remote certificate context: %s",
            Curl_sspi_strerror(sspi_وضعیت, میانگیر, حجم(میانگیر)));
      اكسر; /* failed */
    }

    إذا(!(((pCertContextServer->dwCertEncodingType & X509_ASN_ENCODING) != 0) &&
         (pCertContextServer->cbCertEncoded > 0)))
      اكسر;

    x509_der = (uint8_t *)pCertContextServer->pbCertEncoded;
    x509_der_len = pCertContextServer->cbCertEncoded;
    memset(&x509_parsed, 0, حجم(x509_parsed));
    إذا(Curl_parseX509(&x509_parsed, x509_der, x509_der + x509_der_len))
      اكسر;

    pubkey = &x509_parsed.subject_عمومی_کلید_Info;
    إذا(!pubkey->سرآیند || pubkey->انتها <= pubkey->سرآیند) {
      failf(داده, "SSL: failed retrieving public key from server certificate");
      اكسر;
    }

    نتیجه = Curl_pin_peer_pubkey(داده,
                                  pinnedpubkey,
                                  (ثابت بیعلامت حرف *)pubkey->سرآیند,
                                  (size_t)(pubkey->انتها - pubkey->سرآیند));
    إذا(نتیجه) {
      failf(داده, "SSL: public key does not match pinned public key");
    }
  } طالما(0);

  إذا(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  أرجع نتیجه;
}

ساكن CURLcode ensure_encoding_اندازه(بنية Curl_easy *داده,
                                     بنية sbuffer *encdata,
                                     size_t min_درازی)
{
  size_t اندازه;
  DEBUGASSERT(encdata->درازی >= encdata->جابه_جایی);
  إذا(encdata->درازی < encdata->جابه_جایی)
    أرجع CURLE_FAILED_INIT;
  اندازه = encdata->درازی - encdata->جابه_جایی;
  إذا(اندازه < CURL_SCHANNEL_میانگیر_FREE_اندازه || encdata->درازی < min_درازی) {
    بیعلامت حرف *میانگیر;
    size_t درازی = encdata->جابه_جایی + CURL_SCHANNEL_میانگیر_FREE_اندازه;
    إذا(درازی < min_درازی)
      درازی = min_درازی;
    میانگیر = curlx_realloc(encdata->میانگیر, درازی);
    إذا(!میانگیر) {
      failf(داده, "schannel: unable to re-allocate memory");
      أرجع CURLE_بیرون_از_حافظه;
    }

    encdata->میانگیر = میانگیر;
    encdata->درازی = درازی;
    SCH_DEV(infof(داده, "schannel: encdata.buffer resized %zu",
                  encdata->درازی));
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ensure_decoding_اندازه(بنية Curl_easy *داده,
                                     بنية sbuffer *decdata,
                                     size_t nowsize,
                                     size_t len)
{
  size_t اندازه = nowsize > CURL_SCHANNEL_میانگیر_FREE_اندازه ?
    nowsize : CURL_SCHANNEL_میانگیر_FREE_اندازه;
  DEBUGASSERT(decdata->درازی >= decdata->جابه_جایی);
  إذا(decdata->درازی < decdata->جابه_جایی)
    أرجع CURLE_FAILED_INIT;
  وإلا إذا(decdata->درازی - decdata->جابه_جایی < اندازه ||
          decdata->درازی < len) {
    /* increase internal decrypted data buffer */
    size_t درازی = decdata->جابه_جایی + اندازه;
    بیعلامت حرف *میانگیر;
    /* make sure that the requested amount of data fits */
    إذا(درازی < len)
      درازی = len;

    میانگیر = curlx_realloc(decdata->میانگیر, درازی);
    إذا(!میانگیر) {
      failf(داده, "schannel: unable to re-allocate memory");
      أرجع CURLE_بیرون_از_حافظه;
    }
    decdata->میانگیر = میانگیر;
    decdata->درازی = درازی;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode schannel_connect_step2(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *داده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية schannel_ssl_backend_داده *backend =
    (بنية schannel_ssl_backend_داده *)connssl->backend;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  صحيح i;
  size_t nread = 0;
  Sec_میانگیر outbuf[3];
  Sec_میانگیر_Desc outbuf_desc;
  Sec_میانگیر inbuf[2];
  Sec_میانگیر_Desc inbuf_desc;
  SECURITY_وضعیت sspi_وضعیت = SEC_E_OK;
  CURLcode نتیجه;
  bool doread;
  ثابت حرف *pubkey_ptr;

  DEBUGASSERT(backend);

  doread = (connssl->io_need & CURL_SSL_IO_NEED_SEND) ? FALSE : TRUE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  DEBUGF(infof(داده, "schannel: SSL/TLS connection with %s port %d (step 2/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->پورت));

  إذا(!backend->cred || !backend->ctxt)
    أرجع CURLE_SSL_CONNECT_ERROR;

  /* buffer to store previously received and decrypted data */
  إذا(!backend->decdata.میانگیر) {
    backend->decdata.جابه_جایی = 0;
    backend->decdata.درازی = CURL_SCHANNEL_میانگیر_INIT_اندازه;
    backend->decdata.میانگیر = curlx_malloc(backend->decdata.درازی);
    إذا(!backend->decdata.میانگیر) {
      failf(داده, "schannel: unable to allocate memory");
      أرجع CURLE_بیرون_از_حافظه;
    }
  }

  /* buffer to store previously received and encrypted data */
  إذا(!backend->encdata.میانگیر) {
    backend->encdata_is_incomplete = FALSE;
    backend->encdata.جابه_جایی = 0;
    backend->encdata.درازی = CURL_SCHANNEL_میانگیر_INIT_اندازه;
    backend->encdata.میانگیر = curlx_malloc(backend->encdata.درازی);
    إذا(!backend->encdata.میانگیر) {
      failf(داده, "schannel: unable to allocate memory");
      أرجع CURLE_بیرون_از_حافظه;
    }
  }

  نتیجه = ensure_encoding_اندازه(داده, &backend->encdata, 0);
  إذا(نتیجه)
    أرجع نتیجه;

  لكل(;;) {
    إذا(doread) {
      /* read encrypted handshake data from socket */
      نتیجه = Curl_conn_cf_recv(cf->بعدی, داده,
                                 (حرف *)(backend->encdata.میانگیر +
                                          backend->encdata.جابه_جایی),
                                 backend->encdata.درازی -
                                 backend->encdata.جابه_جایی,
                                 &nread);
      إذا(نتیجه == CURLE_AGAIN) {
        إذا(!backend->encdata.جابه_جایی || backend->encdata_is_incomplete) {
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          DEBUGF(infof(داده, "schannel: failed to receive handshake, "
                       "need more data"));
          أرجع CURLE_OK;
        }
        وإلا {
          DEBUGF(infof(داده, "schannel: no new handshake data received, "
                       "continuing to process existing handshake data"));
        }
      }
      وإلا إذا(نتیجه || (nread == 0)) {
        failf(داده, "schannel: failed to receive handshake, "
              "SSL/TLS connection failed");
        أرجع CURLE_SSL_CONNECT_ERROR;
      }
      وإلا {
        /* increase encrypted data buffer offset */
        backend->encdata.جابه_جایی += nread;
        backend->encdata_is_incomplete = FALSE;
        SCH_DEV(infof(داده, "schannel: encrypted data got %zu", nread));
      }
    }

    SCH_DEV(infof(داده,
                  "schannel: encrypted data buffer: offset %zu length %zu",
                  backend->encdata.جابه_جایی, backend->encdata.درازی));

    /* setup input buffers */
    Init_Sec_میانگیر(&inbuf[0], SECBUFFER_TOKEN,
                  curlx_memdup0((ثابت حرف *)backend->encdata.میانگیر,
                                backend->encdata.جابه_جایی),
                  curlx_uztoul(backend->encdata.جابه_جایی));
    Init_Sec_میانگیر(&inbuf[1], SECBUFFER_خالی, عدم, 0);
    Init_Sec_میانگیر_Desc(&inbuf_desc, inbuf, 2);

    /* setup output buffers */
    Init_Sec_میانگیر(&outbuf[0], SECBUFFER_TOKEN, عدم, 0);
    Init_Sec_میانگیر(&outbuf[1], SECBUFFER_ALERT, عدم, 0);
    Init_Sec_میانگیر(&outbuf[2], SECBUFFER_خالی, عدم, 0);
    Init_Sec_میانگیر_Desc(&outbuf_desc, outbuf, 3);

    إذا(!inbuf[0].pv_میانگیر) {
      failf(داده, "schannel: unable to allocate memory");
      أرجع CURLE_بیرون_از_حافظه;
    }

    /* The socket must be writable (or a poll error occurred) before we call
       InitializeSecurityContext to continue processing the received TLS
       records. This is because that function is not idempotent and we do not
       support partial save/resume sending replies of handshake tokens. */
    إذا(!SOCKET_WRITABLE(Curl_conn_cf_get_socket(cf, داده), 0)) {
      SCH_DEV(infof(داده, "schannel: handshake waiting for writable socket"));
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      curlx_free(inbuf[0].pv_میانگیر);
      أرجع CURLE_OK;
    }

    sspi_وضعیت = Curl_pSecFn->InitializeSecurityContext(
      &backend->cred->cred_handle, &backend->ctxt->ctxt_handle,
      backend->cred->sni_hostname, backend->req_flags,
      0, 0, &inbuf_desc, 0, عدم,
      &outbuf_desc, &backend->ret_flags, عدم);

    /* free buffer for received handshake data */
    curlx_safefree(inbuf[0].pv_میانگیر);

    /* check if the handshake was incomplete */
    بدل(sspi_وضعیت) {
    حالة SEC_E_INCOMPLETE_پیام:
      backend->encdata_is_incomplete = TRUE;
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      DEBUGF(infof(داده,
                   "schannel: received incomplete message, need more data"));
      أرجع CURLE_OK;

    حالة SEC_I_CONTINUE_NEEDED:
    حالة SEC_E_OK:
      /* check if the handshake needs to be continued */
      نتیجه = CURLE_OK;
      لكل(i = 0; i < 3; i++) {
        /* search for handshake tokens that need to be sent */
        إذا(outbuf[i].میانگیر_Type == SECBUFFER_TOKEN && outbuf[i].cb_میانگیر > 0) {
          size_t written = 0;
          DEBUGF(infof(داده, "schannel: sending next handshake data: "
                       "sending %lu bytes.", outbuf[i].cb_میانگیر));

          /* send handshake token to server */
          نتیجه = Curl_conn_cf_send(cf->بعدی, داده,
                                     (ثابت uint8_t *)outbuf[i].pv_میانگیر,
                                     outbuf[i].cb_میانگیر,
                                     FALSE, &written);
          إذا(نتیجه || (outbuf[i].cb_میانگیر != written)) {
            failf(داده, "schannel: failed to send next handshake data: "
                  "sent %zu of %lu bytes", written, outbuf[i].cb_میانگیر);
            نتیجه = CURLE_SSL_CONNECT_ERROR;
          }
        }
      }
      لكل(i = 0; i < 3; i++) {
        /* free obsolete buffer */
        إذا(outbuf[i].pv_میانگیر)
          Curl_pSecFn->Free_Context_میانگیر(outbuf[i].pv_میانگیر);
      }
      إذا(نتیجه)
        أرجع نتیجه;
      اكسر;

    حالة SEC_I_INCOMPLETE_CREDENTIALS:
      إذا(!(backend->req_flags & ISC_REQ_USE_SUPPLIED_CREDS)) {
        /* If the server has requested a client certificate, attempt to
           continue the handshake without one. This allows connections to
           servers which request a client certificate but do not require
           it. */
        backend->req_flags |= ISC_REQ_USE_SUPPLIED_CREDS;
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        DEBUGF(infof(داده,
                     "schannel: a client certificate has been requested"));
        أرجع CURLE_OK;
      }
      FALLTHROUGH();

    افتراضي:
      أرجع schannel_error(داده, sspi_وضعیت);
    }

    /* check if there was additional remaining encrypted data */
    إذا(inbuf[1].میانگیر_Type == SECBUFFER_EXTRA && inbuf[1].cb_میانگیر > 0) {
      SCH_DEV(infof(داده, "schannel: encrypted data length: %lu",
                    inbuf[1].cb_میانگیر));
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
      إذا(backend->encdata.جابه_جایی > inbuf[1].cb_میانگیر) {
        memmove(backend->encdata.میانگیر,
                (backend->encdata.میانگیر + backend->encdata.جابه_جایی) -
                inbuf[1].cb_میانگیر, inbuf[1].cb_میانگیر);
        backend->encdata.جابه_جایی = inbuf[1].cb_میانگیر;
        إذا(sspi_وضعیت == SEC_I_CONTINUE_NEEDED) {
          doread = FALSE;
          استمر;
        }
      }
    }
    وإلا {
      backend->encdata.جابه_جایی = 0;
    }
    اكسر;
  }

  /* check if the handshake needs to be continued */
  إذا(sspi_وضعیت == SEC_I_CONTINUE_NEEDED) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    أرجع CURLE_OK;
  }

  /* check if the handshake is complete */
  إذا(sspi_وضعیت == SEC_E_OK) {
    connssl->connecting_state = ssl_connect_3;
    DEBUGF(infof(داده, "schannel: SSL/TLS handshake complete"));
  }

#اگر_تعریف_نشده CURL_DISABLE_PROXY
  pubkey_ptr = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(داده, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(داده, STRING_SSL_PINNEDPUBLICKEY);
#وإلا
  pubkey_ptr = CURL_EASY_STR(داده, STRING_SSL_PINNEDPUBLICKEY);
#پایان_اگر
  إذا(pubkey_ptr) {
    نتیجه = schannel_pkp_pin_peer_pubkey(cf, داده, pubkey_ptr);
    إذا(نتیجه) {
      failf(داده, "SSL: public key does not match pinned public key");
      أرجع نتیجه;
    }
  }

  إذا(conn_config->verifypeer && backend->use_manual_cred_validation) {
    /* Certificate verification also verifies the hostname if verifyhost */
    أرجع Curl_verify_certificate(cf, داده);
  }

  /* Verify the hostname manually when certificate verification is disabled,
     because in that case Schannel does not verify it. */
  إذا(!conn_config->verifypeer && conn_config->verifyhost)
    أرجع Curl_verify_host(cf, داده);

  أرجع CURLE_OK;
}

ساكن bool valid_cert_encoding(ثابت CERT_CONTEXT *cert_context)
{
  أرجع cert_context &&
    ((cert_context->dwCertEncodingType & X509_ASN_ENCODING) != 0) &&
    cert_context->pbCertEncoded &&
    (cert_context->cbCertEncoded > 0);
}

تعريفنوع bool (*Read_crt_func)(ثابت CERT_CONTEXT *ccert_context,
                              bool معکوس_ترتیب, فراغ *arg);

ساكن فراغ traverse_cert_store(ثابت CERT_CONTEXT *context,
                                Read_crt_func func, فراغ *arg)
{
  ثابت CERT_CONTEXT *current_context = عدم;
  bool should_continue = TRUE;
  bool اولی = TRUE;
  bool معکوس_ترتیب = FALSE;
  طالما(should_continue &&
        (current_context = Cert_Enum_Certificates_درون_Store(
          context->hCertStore,
          current_context)) != عدم) {
    /* Windows 11 22H2 OS Build 22621.674 or higher enumerates certificates in
       leaf-to-root order while all previous versions of Windows enumerate
       certificates in root-to-leaf order. Determine the order of enumeration
       by comparing SECPKG_ATTR_REMOTE_CERT_CONTEXT's pbCertContext with the
       first certificate's pbCertContext. */
    إذا(اولی && context->pbCertEncoded != current_context->pbCertEncoded)
      معکوس_ترتیب = TRUE;
    should_continue = func(current_context, معکوس_ترتیب, arg);
    اولی = FALSE;
  }

  إذا(current_context)
    CertFreeCertificateContext(current_context);
}

ساكن bool cert_counter_callback(ثابت CERT_CONTEXT *ccert_context,
                                  bool معکوس_ترتیب, فراغ *certs_شمار)
{
  (فراغ)معکوس_ترتیب;
  إذا(valid_cert_encoding(ccert_context))
    (*(صحيح *)certs_شمار)++;
  إذا(*(صحيح *)certs_شمار > MAX_ALLOWED_CERT_AMOUNT)
    أرجع FALSE;
  أرجع TRUE;
}

بنية Adder_آرگومانها {
  بنية Curl_easy *داده;
  CURLcode نتیجه;
  صحيح idx;
  صحيح certs_شمار;
};

ساكن bool جمع_کن_cert_مقصد_certinfo(ثابت CERT_CONTEXT *ccert_context,
                                 bool معکوس_ترتیب, فراغ *raw_arg)
{
  بنية Adder_آرگومانها *آرگومانها = (بنية Adder_آرگومانها *)raw_arg;
  آرگومانها->نتیجه = CURLE_OK;
  إذا(valid_cert_encoding(ccert_context)) {
    صحيح درج_شاخص = معکوس_ترتیب ? (آرگومانها->certs_شمار - 1) - آرگومانها->idx :
                       آرگومانها->idx;
    آرگومانها->نتیجه = Curl_استخراج_certinfo(آرگومانها->داده, درج_شاخص,
                                         ccert_context->pbCertEncoded,
                                         ccert_context->pbCertEncoded +
                                         ccert_context->cbCertEncoded);
    آرگومانها->idx++;
  }
  أرجع آرگومانها->نتیجه == CURLE_OK;
}

ساكن فراغ schannel_session_free(فراغ *sessionid)
{
  /* this is expected to be called under sessionid lock */
  بنية Curl_schannel_cred *cred = sessionid;

  إذا(cred) {
    cred->refcount--;
    إذا(cred->refcount == 0) {
      Curl_pSecFn->FreeCredentialsHandle(&cred->cred_handle);
      curlx_free(cred->sni_hostname);
      إذا(cred->client_cert_store) {
        CertCloseStore(cred->client_cert_store, 0);
        cred->client_cert_store = عدم;
      }
      curlx_safefree(cred);
    }
  }
}

ساكن CURLcode schannel_connect_step3(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *داده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية schannel_ssl_backend_داده *backend =
    (بنية schannel_ssl_backend_داده *)connssl->backend;
  بنية ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, داده);
  CURLcode نتیجه = CURLE_OK;
  SECURITY_وضعیت sspi_وضعیت = SEC_E_OK;
  CERT_CONTEXT *ccert_context = عدم;
  SecPkgContext_ApplicationProtocol alpn_نتیجه;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);
  DEBUGASSERT(backend);

  DEBUGF(infof(داده, "schannel: SSL/TLS connection with %s port %d (step 3/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->پورت));

  إذا(!backend->cred)
    أرجع CURLE_SSL_CONNECT_ERROR;

  /* check if the required context attributes are met */
  إذا(backend->ret_flags != backend->req_flags) {
    إذا(!(backend->ret_flags & ISC_RET_SEQUENCE_DETECT))
      failf(داده, "schannel: failed to setup sequence detection");
    إذا(!(backend->ret_flags & ISC_RET_REPLAY_DETECT))
      failf(داده, "schannel: failed to setup replay detection");
    إذا(!(backend->ret_flags & ISC_RET_CONFIDENTIALITY))
      failf(داده, "schannel: failed to setup confidentiality");
    إذا(!(backend->ret_flags & ISC_RET_ALLOCATED_حافظه))
      failf(داده, "schannel: failed to setup memory allocation");
    إذا(!(backend->ret_flags & ISC_RET_جریان))
      failf(داده, "schannel: failed to setup stream orientation");
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  إذا(backend->use_alpn) {
    sspi_وضعیت =
      Curl_pSecFn->Query_Context_صفات(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_APPLICATION_PROTOCOL,
                                          &alpn_نتیجه);

    إذا(sspi_وضعیت != SEC_E_OK) {
      failf(داده, "schannel: failed to retrieve ALPN result");
      أرجع CURLE_SSL_CONNECT_ERROR;
    }

    إذا(alpn_نتیجه.Proto_Nego_وضعیت ==
       Sec_Application_Protocol_Negotiation_وضعیت_Success) {
      إذا(backend->recv_renegotiating &&
         connssl->negotiated.alpn &&
         مقایسه_رشته(connssl->negotiated.alpn,
                 (ثابت حرف *)alpn_نتیجه.ProtocolId,
                 alpn_نتیجه.Protocol_Id_اندازه)) {
        /* Renegotiation selected a different protocol now, we cannot
         * deal with this */
        failf(داده, "schannel: server selected an ALPN protocol too late");
        أرجع CURLE_SSL_CONNECT_ERROR;
      }
      Curl_alpn_مجموعه_negotiated(cf, داده, connssl, alpn_نتیجه.ProtocolId,
                               alpn_نتیجه.Protocol_Id_اندازه);
    }
    وإلا {
      إذا(!backend->recv_renegotiating)
        Curl_alpn_مجموعه_negotiated(cf, داده, connssl, عدم, 0);
    }
  }

  /* save the current session data for possible reuse */
  إذا(Curl_ssl_scache_use(cf, داده)) {
    Curl_ssl_scache_قفل(داده);
    /* Up ref count since call takes ownership */
    backend->cred->refcount++;
    نتیجه = Curl_ssl_scache_جمع_کن_obj(cf, داده, connssl->peer.scache_کلید,
                                     backend->cred, schannel_session_free);
    Curl_ssl_scache_unlock(داده);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  إذا(ssl_config->certinfo) {
    صحيح certs_شمار = 0;
    sspi_وضعیت =
      Curl_pSecFn->Query_Context_صفات(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                          &ccert_context);

    إذا((sspi_وضعیت != SEC_E_OK) || !ccert_context) {
      failf(داده, "schannel: failed to retrieve remote cert context");
      أرجع CURLE_PEER_FAILED_VERIFICATION;
    }

    traverse_cert_store(ccert_context, cert_counter_callback, &certs_شمار);
    إذا(certs_شمار > MAX_ALLOWED_CERT_AMOUNT) {
      failf(داده, "%d certificates is more than allowed (%d)",
            certs_شمار, MAX_ALLOWED_CERT_AMOUNT);
      CertFreeCertificateContext(ccert_context);
      أرجع CURLE_SSL_CONNECT_ERROR;
    }

    نتیجه = Curl_ssl_init_certinfo(داده, certs_شمار);
    إذا(!نتیجه) {
      بنية Adder_آرگومانها آرگومانها;
      آرگومانها.داده = داده;
      آرگومانها.idx = 0;
      آرگومانها.certs_شمار = certs_شمار;
      آرگومانها.نتیجه = CURLE_OK;
      traverse_cert_store(ccert_context, جمع_کن_cert_مقصد_certinfo, &آرگومانها);
      نتیجه = آرگومانها.نتیجه;
    }
    CertFreeCertificateContext(ccert_context);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  connssl->connecting_state = ssl_connect_انجامشده;

  أرجع CURLE_OK;
}

ساكن CURLcode schannel_connect(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده,
                                 bool *انجامشده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  CURLcode نتیجه;

  /* check if the connection has already been established */
  إذا(ssl_connection_complete == connssl->state) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  *انجامشده = FALSE;

  إذا(connssl->connecting_state == ssl_connect_1) {
    نتیجه = schannel_connect_step1(cf, داده);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  إذا(connssl->connecting_state == ssl_connect_2) {
    نتیجه = schannel_connect_step2(cf, داده);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  إذا(connssl->connecting_state == ssl_connect_3) {
    نتیجه = schannel_connect_step3(cf, داده);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  إذا(connssl->connecting_state == ssl_connect_انجامشده) {
    بنية schannel_ssl_backend_داده *backend =
      (بنية schannel_ssl_backend_داده *)connssl->backend;
    DEBUGASSERT(backend);

    إذا(Curl_pSecFn->Query_Context_صفات(
         &backend->ctxt->ctxt_handle,
         SECPKG_ATTR_جریان_SIZES,
         &backend->جریان_sizes)) {
      failf(داده, "schannel: failed getting stream sizes");
      أرجع CURLE_SSL_CONNECT_ERROR;
    }

    connssl->state = ssl_connection_complete;

    /* When SSPI is used in combination with Schannel
     * we need the Schannel context to create the Schannel
     * binding to pass the IIS extended protection checks.
     * Available on Windows 7 or later.
     */
    cf->conn->sslContext = &backend->ctxt->ctxt_handle;

    *انجامشده = TRUE;
  }

  أرجع CURLE_OK;
}

تعداد schannel_renegotiate_caller_t {
  SCH_RENEG_CALLER_IS_RECV,
  SCH_RENEG_CALLER_IS_SEND
};

/* The maximum time we allow for Schannel renegotiation which may in some
   rare cases block either due to libcurl (waiting on the socket) or Windows
   (waiting on an interactive security prompt). Note Schannel "renegotiation"
   is not necessarily literal TLS renegotiation, but means DecryptMessage
   returned SEC_I_RENEGOTIATE which means at least the security context needs
   to be re-established. */
#تعریف_کن MAX_RENEG_بلوک_زمان (60 * 1000) /* 60 seconds in milliseconds */

/* This function renegotiates the connection due to a server request received
   by schannel_recv. This function returns CURLE_AGAIN if the renegotiation is
   incomplete. In that case, we remain in the renegotiation (connecting) stage
   and future calls to schannel_recv and schannel_send must call this function
   first to complete the renegotiation. */
ساكن CURLcode schannel_recv_renegotiate(
  بنية Curl_cfilter *cf, بنية Curl_easy *داده,
  تعداد schannel_renegotiate_caller_t caller)
{
  CURLcode نتیجه;
  curl_socket_t sockfd;
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية schannel_ssl_backend_داده *backend =
    (بنية schannel_ssl_backend_داده *)connssl->backend;
  بنية schannel_renegotiate_state *rs = &backend->renegotiate_state;

  إذا(!backend || !backend->recv_renegotiating) {
    failf(داده, "schannel: unexpected call to schannel_recv_renegotiate");
    أرجع CURLE_SSL_CONNECT_ERROR;
  }
  DEBUGASSERT(caller <= SCH_RENEG_CALLER_IS_SEND);
  إذا(caller == SCH_RENEG_CALLER_IS_RECV)
    SCH_DEV(infof(داده, "schannel: renegotiation caller is schannel_recv"));
  وإلا
    SCH_DEV(infof(داده, "schannel: renegotiation caller is schannel_send"));

  sockfd = Curl_conn_cf_get_socket(cf, داده);

  إذا(sockfd == CURL_SOCKET_BAD) {
    failf(داده, "schannel: renegotiation missing socket");
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  إذا(!rs->started) { /* new renegotiation */
    infof(داده, "schannel: renegotiating SSL/TLS connection");
    DEBUGASSERT(connssl->state == ssl_connection_complete);
    DEBUGASSERT(connssl->connecting_state == ssl_connect_انجامشده);
    connssl->state = ssl_connection_negotiating;
    connssl->connecting_state = ssl_connect_2;
    memset(rs, 0, حجم(*rs));
    rs->io_need = CURL_SSL_IO_NEED_SEND;
    rs->شروع_زمان = *Curl_pgrs_now(داده);
    rs->started = TRUE;
  }

  لكل(;;) {
    bool بلوک_read, بلوک_write, blocking, انجامشده;
    curl_socket_t readfd, writefd;
    timediff_t elapsed;

    elapsed = curlx_ptimediff_ms(Curl_pgrs_now(داده), &rs->شروع_زمان);
    إذا(elapsed >= MAX_RENEG_بلوک_زمان) {
      failf(داده, "schannel: renegotiation timeout");
      نتیجه = CURLE_SSL_CONNECT_ERROR;
      اكسر;
    }

    /* the current io_need state may have been overwritten since the last time
       this function was called. restore the io_need state needed to continue
       the renegotiation. */

    connssl->io_need = rs->io_need;

    نتیجه = schannel_connect(cf, داده, &انجامشده);

    rs->io_need = connssl->io_need;

    إذا(!نتیجه && !انجامشده)
      نتیجه = CURLE_AGAIN;

    إذا(نتیجه != CURLE_AGAIN)
      اكسر;

    readfd = (rs->io_need & CURL_SSL_IO_NEED_RECV) ? sockfd : CURL_SOCKET_BAD;
    writefd = (rs->io_need & CURL_SSL_IO_NEED_SEND) ? sockfd : CURL_SOCKET_BAD;

    إذا(readfd == CURL_SOCKET_BAD && writefd == CURL_SOCKET_BAD)
      استمر;

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

    بلوک_read = (caller == SCH_RENEG_CALLER_IS_SEND) ? TRUE : FALSE;
    بلوک_write = (caller == SCH_RENEG_CALLER_IS_RECV) ? TRUE : FALSE;

    blocking = (بلوک_read && (readfd != CURL_SOCKET_BAD)) ||
               (بلوک_write && (writefd != CURL_SOCKET_BAD));

    SCH_DEV_SHOWBOOL(بلوک_read);
    SCH_DEV_SHOWBOOL(بلوک_write);
    SCH_DEV_SHOWBOOL(blocking);

    لكل(;;) {
      صحيح what;
      timediff_t timeout_ms, مانده;

      نتیجه = Curl_pgrsUpdate(داده);
      إذا(نتیجه)
        اكسر;

      elapsed = curlx_ptimediff_ms(Curl_pgrs_now(داده), &rs->شروع_زمان);
      إذا(elapsed >= MAX_RENEG_بلوک_زمان) {
        failf(داده, "schannel: renegotiation timeout");
        نتیجه = CURLE_SSL_CONNECT_ERROR;
        اكسر;
      }
      مانده = MAX_RENEG_بلوک_زمان - elapsed;

      إذا(blocking) {
        timeout_ms = Curl_timeleft_ms(داده);

        إذا(timeout_ms < 0) {
          نتیجه = CURLE_OPERATION_TIMEDOUT;
          اكسر;
        }

        /* the blocking is in intervals so that the progress function can be
           called every second */
        إذا(!timeout_ms || timeout_ms > 1000)
          timeout_ms = 1000;

        إذا(timeout_ms > مانده)
          timeout_ms = مانده;
      }
      وإلا
        timeout_ms = 0;

      SCH_DEV(infof(داده, "schannel: renegotiation wait until socket is"
                    "%s%s for up to %" FMT_TIMEDIFF_T " ms",
                    ((readfd != CURL_SOCKET_BAD) ? " readable" : ""),
                    ((writefd != CURL_SOCKET_BAD) ? " writable" : ""),
                    timeout_ms));

      what = Curl_socket_check(readfd, CURL_SOCKET_BAD, writefd, timeout_ms);

      إذا(what > 0 && (what & (CURL_CSELECT_درون | CURL_CSELECT_بیرون))) {
        SCH_DEV(infof(داده, "schannel: renegotiation socket %s%s",
                      ((what & CURL_CSELECT_درون) ? "CURL_CSELECT_IN " : ""),
                      ((what & CURL_CSELECT_بیرون) ? "CURL_CSELECT_OUT " : "")));
        نتیجه = CURLE_AGAIN;
        اكسر;
      }
      وإلا إذا(!what) {
        SCH_DEV(infof(داده, "schannel: renegotiation socket timeout"));
        إذا(blocking)
          استمر;
        وإلا
          أرجع CURLE_AGAIN;
      }

      failf(داده, "schannel: socket error during renegotiation");
      نتیجه = CURLE_SSL_CONNECT_ERROR;
      اكسر;
    }
    إذا(نتیجه != CURLE_AGAIN)
      اكسر;
  }

  DEBUGASSERT(نتیجه != CURLE_AGAIN);

  rs->started = FALSE;
  backend->recv_renegotiating = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  إذا(نتیجه)
    failf(داده, "schannel: renegotiation failed");
  وإلا
    infof(داده, "schannel: SSL/TLS connection renegotiated");

  أرجع نتیجه;
}

ساكن CURLcode schannel_send(بنية Curl_cfilter *cf, بنية Curl_easy *داده,
                              ثابت فراغ *buf, size_t len, size_t *pnwritten)
{
  size_t داده_len = 0;
  بیعلامت حرف *ptr = عدم;
  بنية ssl_connect_داده *connssl = cf->ctx;
  Sec_میانگیر outbuf[4];
  Sec_میانگیر_Desc outbuf_desc;
  SECURITY_وضعیت sspi_وضعیت = SEC_E_OK;
  CURLcode نتیجه = CURLE_OK;
  بنية schannel_ssl_backend_داده *backend =
    (بنية schannel_ssl_backend_داده *)connssl->backend;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  إذا(backend->recv_renegotiating) {
    نتیجه = schannel_recv_renegotiate(cf, داده, SCH_RENEG_CALLER_IS_SEND);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  /* check if the buffer is longer than the maximum message length */
  إذا(len > backend->جریان_sizes.cb_بیشینه_پیام) {
    len = backend->جریان_sizes.cb_بیشینه_پیام;
  }

  /* calculate the complete message length and prepare the send buffer */
  داده_len = backend->جریان_sizes.cb_سرآیند + len +
    backend->جریان_sizes.cbTrailer;
  إذا(داده_len > backend->send_میانگیر_len) {
    ptr = curlx_realloc(backend->send_میانگیر, داده_len);
    إذا(!ptr)
      أرجع CURLE_بیرون_از_حافظه;
    backend->send_میانگیر = ptr;
    backend->send_میانگیر_len = داده_len;
  }
  وإلا
    ptr = backend->send_میانگیر;

  /* setup output buffers (header, data, trailer, empty) */
  Init_Sec_میانگیر(&outbuf[0], SECBUFFER_جریان_سرآیند,
                ptr, backend->جریان_sizes.cb_سرآیند);
  Init_Sec_میانگیر(&outbuf[1], SECBUFFER_داده,
                ptr + backend->جریان_sizes.cb_سرآیند, curlx_uztoul(len));
  Init_Sec_میانگیر(&outbuf[2], SECBUFFER_جریان_TRAILER,
                ptr + backend->جریان_sizes.cb_سرآیند + len,
                backend->جریان_sizes.cbTrailer);
  Init_Sec_میانگیر(&outbuf[3], SECBUFFER_خالی, عدم, 0);
  Init_Sec_میانگیر_Desc(&outbuf_desc, outbuf, 4);

  /* copy data into output buffer */
  کپی_حافظه(outbuf[1].pv_میانگیر, buf, len);

  /* https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-encryptmessage */
  sspi_وضعیت = Curl_pSecFn->Encrypt_پیام(&backend->ctxt->ctxt_handle, 0,
                                            &outbuf_desc, 0);

  /* check if the message was encrypted */
  إذا(sspi_وضعیت == SEC_E_OK) {

    /* send the encrypted message including header, data and trailer */
    len = outbuf[0].cb_میانگیر + outbuf[1].cb_میانگیر + outbuf[2].cb_میانگیر;

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
    طالما(len > *pnwritten) {
      size_t this_write = 0;
      صحيح what;
      timediff_t timeout_ms = Curl_timeleft_ms(داده);
      إذا(timeout_ms < 0) {
        /* we already got the timeout */
        failf(داده, "schannel: timed out sending data (bytes sent: %zu)",
              *pnwritten);
        نتیجه = CURLE_OPERATION_TIMEDOUT;
        اكسر;
      }
      وإلا إذا(!timeout_ms)
        timeout_ms = TIMEDIFF_T_MAX;
      what = SOCKET_WRITABLE(Curl_conn_cf_get_socket(cf, داده), timeout_ms);
      إذا(what < 0) {
        /* fatal error */
        failf(داده, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        نتیجه = CURLE_SEND_ERROR;
        اكسر;
      }
      وإلا إذا(what == 0) {
        failf(داده, "schannel: timed out sending data (bytes sent: %zu)",
              *pnwritten);
        نتیجه = CURLE_OPERATION_TIMEDOUT;
        اكسر;
      }
      /* socket is writable */

       نتیجه = Curl_conn_cf_send(cf->بعدی, داده,
                                  (ثابت uint8_t *)ptr + *pnwritten,
                                  len - *pnwritten,
                                  FALSE, &this_write);
      إذا(نتیجه == CURLE_AGAIN)
        استمر;
      وإلا إذا(نتیجه) {
        اكسر;
      }

      *pnwritten += this_write;
    }
  }
  وإلا إذا(sspi_وضعیت == SEC_E_INSUFFICIENT_حافظه) {
    نتیجه = CURLE_بیرون_از_حافظه;
  }
  وإلا {
    نتیجه = CURLE_SEND_ERROR;
  }

  إذا(len == *pnwritten)
    /* Encrypted message including header, data and trailer entirely sent.
       The return value is the number of unencrypted bytes that were sent. */
    *pnwritten = outbuf[1].cb_میانگیر;

  أرجع نتیجه;
}

ساكن CURLcode schannel_recv(بنية Curl_cfilter *cf, بنية Curl_easy *داده,
                              حرف *buf, size_t len, size_t *pnread)
{
  size_t اندازه = 0;
  size_t nread = 0;
  بنية ssl_connect_داده *connssl = cf->ctx;
  Sec_میانگیر inbuf[4];
  Sec_میانگیر_Desc inbuf_desc;
  SECURITY_وضعیت sspi_وضعیت = SEC_E_OK;
  بنية schannel_ssl_backend_داده *backend =
    (بنية schannel_ssl_backend_داده *)connssl->backend;
  CURLcode نتیجه = CURLE_OK;

  DEBUGASSERT(backend);
  *pnread = 0;

  إذا(backend->recv_renegotiating) {
    نتیجه = schannel_recv_renegotiate(cf, داده, SCH_RENEG_CALLER_IS_RECV);
    إذا(نتیجه)
      أرجع نتیجه;
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

  SCH_DEV(infof(داده, "schannel: client wants to read %zu bytes", len));

  إذا(len && len <= backend->decdata.جابه_جایی) {
    SCH_DEV(infof(داده,
                  "schannel: enough decrypted data is already available"));
    اذهب cleanup;
  }
  وإلا إذا(backend->recv_unrecoverable_err) {
    نتیجه = backend->recv_unrecoverable_err;
    infof(داده, "schannel: an unrecoverable error occurred in a prior call");
    اذهب cleanup;
  }
  وإلا إذا(backend->recv_sspi_close_notify) {
    /* once a server has indicated shutdown there is no more encrypted data */
    infof(داده, "schannel: server indicated shutdown in a prior call");
    اذهب cleanup;
  }
  /* it is debatable what to return when !len. Regardless we cannot return
     immediately because there may be data to decrypt (in the case we want to
     decrypt all encrypted cached data) so handle !len later in cleanup. */
  وإلا إذا(len && !backend->recv_connection_closed) {
    /* the encrypted buffer must be large enough to hold all the bytes
       requested and some TLS record overhead. 'len' is a buffer size, so this
       integer math cannot overflow. */
    ثابت size_t min_encdata_درازی = len + CURL_SCHANNEL_میانگیر_FREE_اندازه;

    /* make sure encrypt buffer fits the requested amount of data */
    نتیجه = ensure_encoding_اندازه(داده, &backend->encdata, min_encdata_درازی);
    إذا(نتیجه)
      اذهب cleanup;

    SCH_DEV(infof(داده,
                  "schannel: encrypted data buffer: offset %zu length %zu",
                  backend->encdata.جابه_جایی, backend->encdata.درازی));

    /* read encrypted data from socket */
    نتیجه = Curl_conn_cf_recv(cf->بعدی, داده,
                               (حرف *)(backend->encdata.میانگیر +
                                        backend->encdata.جابه_جایی),
                               backend->encdata.درازی -
                               backend->encdata.جابه_جایی,
                               &nread);
    إذا(نتیجه) {
      إذا(نتیجه == CURLE_AGAIN)
        SCH_DEV(infof(داده, "schannel: recv returned CURLE_AGAIN"));
      وإلا {
        infof(داده, "schannel: recv returned error %d", (صحيح)نتیجه);
        backend->recv_unrecoverable_err = نتیجه;
      }
    }
    وإلا إذا(nread == 0) {
      backend->recv_connection_closed = TRUE;
      DEBUGF(infof(داده, "schannel: server closed the connection"));
    }
    وإلا {
      backend->encdata.جابه_جایی += nread;
      backend->encdata_is_incomplete = FALSE;
      SCH_DEV(infof(داده, "schannel: encrypted data got %zu", nread));
    }
  }

  SCH_DEV(infof(داده, "schannel: encrypted data buffer: offset %zu length %zu",
                backend->encdata.جابه_جایی, backend->encdata.درازی));

  /* decrypt loop */
  طالما(backend->encdata.جابه_جایی > 0 && sspi_وضعیت == SEC_E_OK &&
        (!len || backend->decdata.جابه_جایی < len ||
         backend->recv_connection_closed)) {
    /* prepare data buffer for DecryptMessage call */
    Init_Sec_میانگیر(&inbuf[0], SECBUFFER_داده, backend->encdata.میانگیر,
                  curlx_uztoul(backend->encdata.جابه_جایی));

    /* we need 3 more empty input buffers for possible output */
    Init_Sec_میانگیر(&inbuf[1], SECBUFFER_خالی, عدم, 0);
    Init_Sec_میانگیر(&inbuf[2], SECBUFFER_خالی, عدم, 0);
    Init_Sec_میانگیر(&inbuf[3], SECBUFFER_خالی, عدم, 0);
    Init_Sec_میانگیر_Desc(&inbuf_desc, inbuf, 4);

    /* https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-decryptmessage
     */
    sspi_وضعیت = Curl_pSecFn->Decrypt_پیام(&backend->ctxt->ctxt_handle,
                                              &inbuf_desc, 0, عدم);

    /* check if everything went fine (server may want to renegotiate
       or shutdown the connection context) */
    إذا(sspi_وضعیت == SEC_E_OK || sspi_وضعیت == SEC_I_RENEGOTIATE ||
       sspi_وضعیت == SEC_I_CONTEXT_EXPIRED) {
      /* check for successfully decrypted data, even before actual
         renegotiation or shutdown of the connection context */
      إذا(inbuf[1].میانگیر_Type == SECBUFFER_داده) {
        SCH_DEV(infof(داده, "schannel: decrypted data length: %lu",
                      inbuf[1].cb_میانگیر));

        /* ensure the decode buffer fits the received amount of data */
        نتیجه = ensure_decoding_اندازه(داده, &backend->decdata,
                                      inbuf[1].cb_میانگیر, len);
        إذا(نتیجه)
          اذهب cleanup;

        /* copy decrypted data to internal buffer */
        اندازه = inbuf[1].cb_میانگیر;
        إذا(اندازه) {
          کپی_حافظه(backend->decdata.میانگیر + backend->decdata.جابه_جایی,
                 inbuf[1].pv_میانگیر, اندازه);
          backend->decdata.جابه_جایی += اندازه;
        }

        SCH_DEV(infof(داده, "schannel: decrypted data added: %zu", اندازه));
        SCH_DEV(infof(داده,
                      "schannel: decrypted cached: offset %zu length %zu",
                      backend->decdata.جابه_جایی, backend->decdata.درازی));
      }

      /* check for remaining encrypted data */
      إذا(inbuf[3].میانگیر_Type == SECBUFFER_EXTRA && inbuf[3].cb_میانگیر > 0) {
        SCH_DEV(infof(داده, "schannel: encrypted data length: %lu",
                      inbuf[3].cb_میانگیر));

        /* check if the remaining data is less than the total amount
         * and therefore begins after the already processed data
         */
        إذا(backend->encdata.جابه_جایی > inbuf[3].cb_میانگیر) {
          /* move remaining encrypted data forward to the beginning of
میانگیر*/
          memmove(backend->encdata.میانگیر,
                  (backend->encdata.میانگیر + backend->encdata.جابه_جایی) -
                  inbuf[3].cb_میانگیر, inbuf[3].cb_میانگیر);
          backend->encdata.جابه_جایی = inbuf[3].cb_میانگیر;
        }

        SCH_DEV(infof(داده,
                      "schannel: encrypted cached: offset %zu length %zu",
                      backend->encdata.جابه_جایی, backend->encdata.درازی));
      }
      وإلا {
        /* reset encrypted buffer offset, because there is no data remaining */
        backend->encdata.جابه_جایی = 0;
      }

      /* check if server wants to renegotiate the connection context */
      إذا(sspi_وضعیت == SEC_I_RENEGOTIATE) {
        infof(داده, "schannel: remote party requests renegotiation");
        إذا(نتیجه && نتیجه != CURLE_AGAIN) {
          infof(داده, "schannel: cannot renegotiate, an error is pending");
          اذهب cleanup;
        }

        backend->recv_renegotiating = TRUE;
        نتیجه = schannel_recv_renegotiate(cf, داده, SCH_RENEG_CALLER_IS_RECV);
        إذا(نتیجه)
          اذهب cleanup;

        /* now retry receiving data */
        sspi_وضعیت = SEC_E_OK;
        استمر;
      }
      /* check if the server closed the connection */
      وإلا إذا(sspi_وضعیت == SEC_I_CONTEXT_EXPIRED) {
        /* In Windows 2000 SEC_I_CONTEXT_EXPIRED (close_notify) is not
           returned so we have to work around that in cleanup. */
        backend->recv_sspi_close_notify = TRUE;
        إذا(!backend->recv_connection_closed)
          backend->recv_connection_closed = TRUE;
        /* We received the close notify fine, any error we got
         * from the lower filters afterwards (e.g. the socket), is not
         * an error on the TLS data stream. That one ended here. */
        إذا(نتیجه == CURLE_RECV_ERROR)
          نتیجه = CURLE_OK;
        infof(داده,
              "schannel: server close notification received (close_notify)");
        اذهب cleanup;
      }
    }
    وإلا إذا(sspi_وضعیت == SEC_E_INCOMPLETE_پیام) {
      backend->encdata_is_incomplete = TRUE;
      إذا(!نتیجه)
        نتیجه = CURLE_AGAIN;
      SCH_DEV(infof(داده, "schannel: failed to decrypt data, need more data"));
      اذهب cleanup;
    }
    وإلا {
      حرف میانگیر[STRERROR_LEN];
      failf(داده, "schannel: failed to read data from server: %s",
            Curl_sspi_strerror(sspi_وضعیت, میانگیر, حجم(میانگیر)));
      نتیجه = CURLE_RECV_ERROR;
      اذهب cleanup;
    }
  }

  SCH_DEV(infof(داده, "schannel: encrypted data buffer: offset %zu length %zu",
                backend->encdata.جابه_جایی, backend->encdata.درازی));

  SCH_DEV(infof(داده, "schannel: decrypted data buffer: offset %zu length %zu",
                backend->decdata.جابه_جایی, backend->decdata.درازی));

cleanup:
  /* Warning- there is no guarantee the encdata state is valid at this point */
  SCH_DEV(infof(داده, "schannel: schannel_recv cleanup"));

  /* Error if the connection has closed without a close_notify.

     The behavior here is a matter of debate. We do not want to be vulnerable
     to a truncation attack however there is some browser precedent for
     ignoring the close_notify for compatibility reasons. */
  إذا(len && !backend->decdata.جابه_جایی && backend->recv_connection_closed &&
     !backend->recv_sspi_close_notify) {
    نتیجه = CURLE_RECV_ERROR;
    failf(داده, "schannel: server closed abruptly (missing close_notify)");
  }

  /* Any error other than CURLE_AGAIN is an unrecoverable error. */
  إذا(نتیجه && نتیجه != CURLE_AGAIN)
    backend->recv_unrecoverable_err = نتیجه;

  اندازه = len < backend->decdata.جابه_جایی ? len : backend->decdata.جابه_جایی;
  إذا(اندازه) {
    کپی_حافظه(buf, backend->decdata.میانگیر, اندازه);
    memmove(backend->decdata.میانگیر, backend->decdata.میانگیر + اندازه,
            backend->decdata.جابه_جایی - اندازه);
    backend->decdata.جابه_جایی -= اندازه;
    SCH_DEV(infof(داده, "schannel: decrypted data returned %zu", اندازه));
    SCH_DEV(infof(داده,
                  "schannel: decrypted data buffer: offset %zu length %zu",
                  backend->decdata.جابه_جایی, backend->decdata.درازی));
    *pnread = اندازه;
    أرجع CURLE_OK;
  }

  إذا(!نتیجه && !backend->recv_connection_closed)
    نتیجه = CURLE_AGAIN;

  /* it is debatable what to return when !len. We could return whatever error
     we got from decryption but instead we override here so the return is
     consistent. */
  إذا(!len)
    أرجع CURLE_OK;

  أرجع نتیجه;
}

ساكن bool schannel_داده_pending(بنية Curl_cfilter *cf,
                                  ثابت بنية Curl_easy *داده)
{
  ثابت بنية ssl_connect_داده *connssl = cf->ctx;
  بنية schannel_ssl_backend_داده *backend =
    (بنية schannel_ssl_backend_داده *)connssl->backend;

  (فراغ)داده;
  DEBUGASSERT(backend);

  إذا(backend->ctxt) /* SSL/TLS is in use */
    أرجع backend->decdata.جابه_جایی > 0 ||
           (backend->encdata.جابه_جایی > 0 && !backend->encdata_is_incomplete) ||
           backend->recv_connection_closed ||
           backend->recv_sspi_close_notify ||
           backend->recv_unrecoverable_err;
  وإلا
    أرجع FALSE;
}

/* shut down the SSL connection and clean up related memory.
   this function can be called multiple times on the same connection including
   if the SSL connection failed (eg connection made but failed handshake). */
ساكن CURLcode schannel_shutdown(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده,
                                  bool send_shutdown, bool *انجامشده)
{
  /* See https://learn.microsoft.com/windows/win32/secauthn/shutting-down-an-schannel-connection
   * Shutting Down an Schannel Connection
   */
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية schannel_ssl_backend_داده *backend =
    (بنية schannel_ssl_backend_داده *)connssl->backend;
  CURLcode نتیجه = CURLE_OK;

  إذا(cf->shutdown) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  DEBUGASSERT(داده);
  DEBUGASSERT(backend);

  /* Not supported in schannel */
  (فراغ)send_shutdown;

  *انجامشده = FALSE;
  إذا(backend->ctxt) {
    infof(داده, "schannel: shutting down SSL/TLS connection with %s port %d",
          connssl->peer.origin->hostname, connssl->peer.origin->پورت);
  }

  إذا(!backend->ctxt || cf->shutdown) {
    *انجامشده = TRUE;
    اذهب بیرون;
  }

  إذا(backend->cred && backend->ctxt && !backend->فرستاده_shutdown &&
     !backend->shutdown_generated) {
    Sec_میانگیر_Desc BuffDesc;
    Sec_میانگیر میانگیر_2;
    SECURITY_وضعیت sspi_وضعیت;
    Sec_میانگیر outbuf;
    Sec_میانگیر_Desc outbuf_desc;
    DWORD dwshut = SCHANNEL_SHUTDOWN;

    Init_Sec_میانگیر(&میانگیر_2, SECBUFFER_TOKEN, &dwshut, حجم(dwshut));
    Init_Sec_میانگیر_Desc(&BuffDesc, &میانگیر_2, 1);

    sspi_وضعیت = Curl_pSecFn->ApplyControlToken(&backend->ctxt->ctxt_handle,
                                                 &BuffDesc);

    إذا(sspi_وضعیت != SEC_E_OK) {
      حرف میانگیر[STRERROR_LEN];
      failf(داده, "schannel: ApplyControlToken failure: %s",
            Curl_sspi_strerror(sspi_وضعیت, میانگیر, حجم(میانگیر)));
      نتیجه = CURLE_SEND_ERROR;
      اذهب بیرون;
    }

    /* setup output buffer */
    Init_Sec_میانگیر(&outbuf, SECBUFFER_خالی, عدم, 0);
    Init_Sec_میانگیر_Desc(&outbuf_desc, &outbuf, 1);

    sspi_وضعیت = Curl_pSecFn->InitializeSecurityContext(
      &backend->cred->cred_handle,
      &backend->ctxt->ctxt_handle,
      backend->cred->sni_hostname,
      backend->req_flags,
      0,
      0,
      عدم,
      0,
      &backend->ctxt->ctxt_handle,
      &outbuf_desc,
      &backend->ret_flags, عدم);

    إذا((sspi_وضعیت == SEC_E_OK) || (sspi_وضعیت == SEC_I_CONTEXT_EXPIRED)) {
      backend->shutdown_token = outbuf;
      backend->shutdown_generated = TRUE;
    }
    وإلا {
      حرف میانگیر[STRERROR_LEN];
      إذا(outbuf.pv_میانگیر)
        Curl_pSecFn->Free_Context_میانگیر(outbuf.pv_میانگیر);
      failf(داده, "schannel: generating close msg failed: %s",
            Curl_sspi_strerror(sspi_وضعیت, میانگیر, حجم(میانگیر)));
      نتیجه = CURLE_SEND_ERROR;
      اذهب بیرون;
    }
  }

  إذا(backend->shutdown_generated && !backend->فرستاده_shutdown) {
    size_t written = 0;
    size_t مانده;

    DEBUGASSERT(backend->shutdown_جابه_جایی <= backend->shutdown_token.cb_میانگیر);
    مانده = backend->shutdown_token.cb_میانگیر - backend->shutdown_جابه_جایی;

    إذا(مانده) {
      نتیجه = Curl_conn_cf_send(cf->بعدی, داده,
                                 ((uint8_t *)backend->shutdown_token.pv_میانگیر +
                                  backend->shutdown_جابه_جایی),
                                 مانده, FALSE, &written);
      إذا(!نتیجه)
        backend->shutdown_جابه_جایی += written;
      إذا(نتیجه == CURLE_AGAIN || (!نتیجه && written < مانده)) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        نتیجه = CURLE_OK;
        اذهب بیرون;
      }
    }
    وإلا /* no shutdown message to send? */
      نتیجه = CURLE_OK;
    إذا(نتیجه) {
      إذا(backend->recv_connection_closed) {
        /* The server already closed, so sending close_notify is optional. */
        *انجامشده = TRUE;
        نتیجه = CURLE_OK;
      }
      وإلا {
        failf(داده, "schannel: error sending close msg: %d", (صحيح)نتیجه);
        نتیجه = CURLE_SEND_ERROR;
      }
      اذهب بیرون;
    }
    إذا(backend->shutdown_token.pv_میانگیر)
      Curl_pSecFn->Free_Context_میانگیر(backend->shutdown_token.pv_میانگیر);
    memset(&backend->shutdown_token, 0, حجم(backend->shutdown_token));
    backend->shutdown_جابه_جایی = 0;
    backend->فرستاده_shutdown = TRUE;
    *انجامشده = TRUE;
  }

  /* If the connection seems open and we have not seen the close notify
   * from the server yet, try to receive it. */
  إذا(backend->cred && backend->ctxt &&
     !backend->recv_sspi_close_notify && !backend->recv_connection_closed) {
    حرف میانگیر[1024];
    size_t nread;

    نتیجه = schannel_recv(cf, داده, میانگیر, حجم(میانگیر), &nread);
    إذا(نتیجه == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
    }
    وإلا إذا(نتیجه) {
      CURL_TRC_CF(داده, cf, "SSL shutdown, error %d", (صحيح)نتیجه);
      نتیجه = CURLE_RECV_ERROR;
    }
    وإلا إذا(nread == 0) {
      /* We got the close notify alert and are done. */
      backend->recv_connection_closed = TRUE;
      *انجامشده = TRUE;
    }
    وإلا {
      /* still data coming in? */
    }
  }

بیرون:
  cf->shutdown = (نتیجه || *انجامشده);
  أرجع نتیجه;
}

ساكن فراغ schannel_close(بنية Curl_cfilter *cf, بنية Curl_easy *داده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية schannel_ssl_backend_داده *backend =
    (بنية schannel_ssl_backend_داده *)connssl->backend;

  DEBUGASSERT(داده);
  DEBUGASSERT(backend);

  إذا(backend->shutdown_token.pv_میانگیر)
    Curl_pSecFn->Free_Context_میانگیر(backend->shutdown_token.pv_میانگیر);
  memset(&backend->shutdown_token, 0, حجم(backend->shutdown_token));
  backend->shutdown_جابه_جایی = 0;

  /* free SSPI Schannel API security context handle */
  إذا(backend->ctxt) {
    إذا(cf->conn->sslContext == &backend->ctxt->ctxt_handle)
      cf->conn->sslContext = عدم;

    DEBUGF(infof(داده, "schannel: clear security context handle"));
    Curl_pSecFn->DeleteSecurityContext(&backend->ctxt->ctxt_handle);
    curlx_safefree(backend->ctxt);
  }

  /* free SSPI Schannel API credential handle */
  إذا(backend->cred) {
    Curl_ssl_scache_قفل(داده);
    schannel_session_free(backend->cred);
    Curl_ssl_scache_unlock(داده);
    backend->cred = عدم;
  }

  /* free the buffer used to encrypt outgoing data */
  curlx_safefree(backend->send_میانگیر);
  backend->send_میانگیر_len = 0;

  /* free internal buffer for received encrypted data */
  إذا(backend->encdata.میانگیر) {
    curlx_safefree(backend->encdata.میانگیر);
    backend->encdata.درازی = 0;
    backend->encdata.جابه_جایی = 0;
    backend->encdata_is_incomplete = FALSE;
  }

  /* free internal buffer for received decrypted data */
  إذا(backend->decdata.میانگیر) {
    curlx_safefree(backend->decdata.میانگیر);
    backend->decdata.درازی = 0;
    backend->decdata.جابه_جایی = 0;
  }
}

ساكن صحيح schannel_init(فراغ)
{
  تعريفنوع ثابت حرف *(APIENTRY *WINE_GET_نسخه_FN)(فراغ);
#إذا defined(__clang__) && __clang_major__ >= 16
#رهنمود clang diagnostic افزودن
#رهنمود clang diagnostic ignored "-Wcast-function-type-strict"
#پایان_اگر
  WINE_GET_نسخه_FN p_wine_get_نسخه =
    CURLX_FUNCTION_CAST(WINE_GET_نسخه_FN,
      Get_Proc_آدرس(GetModuleHandle(متن_2("ntdll")), "wine_get_version"));
#إذا defined(__clang__) && __clang_major__ >= 16
#رهنمود clang diagnostic برداشت
#پایان_اگر
  إذا(p_wine_get_نسخه) {  /* WINE detected */
    curl_off_t ver = 0;
    ثابت حرف *wine_نسخه = p_wine_get_نسخه();  /* e.g. "6.0.2" */
    /* Assume ALPN support with WINE 6.0 or greater */
    إذا(wine_نسخه)
      curlx_str_عدد(&wine_نسخه, &ver, 20);
    s_win_has_alpn = (ver >= 6);
  }
  وإلا {
    /* ALPN is supported on Windows 8.1 / Server 2012 R2 or newer. */
    s_win_has_alpn = curlx_verify_windows_نسخه(6, 3, 0, PLATFORM_WINNT,
                                                  نسخه_GREATER_THAN_EQUAL);
  }

  أرجع Curl_sspi_global_init() == CURLE_OK ? 1 : 0;
}

ساكن فراغ schannel_cleanup(فراغ)
{
  Curl_sspi_global_cleanup();
}

ساكن size_t schannel_نسخه(حرف *میانگیر, size_t اندازه)
{
  أرجع curl_msnprintf(میانگیر, اندازه, "Schannel");
}

ساكن CURLcode schannel_random(بنية Curl_easy *داده,
                                بیعلامت حرف *entropy, size_t درازی)
{
  (فراغ)داده;

  أرجع curlx_win32_random(entropy, درازی);
}

ساكن CURLcode schannel_checksum(ثابت بیعلامت حرف *درون_داد,
                                  size_t inputlen,
                                  بیعلامت حرف *checksum,
                                  size_t checksumlen,
                                  DWORD provType,
                                  ثابت بیعلامت صحيح algId)
{
  CURLcode نتیجه = CURLE_FAILED_INIT;

  HCRYPTPROV hProv = 0;
  HCRYPTHASH hHash = 0;

  size_t off;

  DWORD cb_Hash_اندازه;
  DWORD dw_Hash_اندازه_Len;
  DWORD dwChecksumLen;

  /* since this can fail in multiple ways, zero memory first so we never
   * return old data
   */
  memset(checksum, 0, checksumlen);

  إذا(!CryptAcquireContext(&hProv, عدم, عدم, provType,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    اذهب بیرون;

  إذا(!CryptCreateHash(hProv, algId, 0, 0, &hHash))
    اذهب بیرون;

  نتیجه = CURLE_BAD_FUNCTION_آرگومان;

  off = 0;
  طالما(off < inputlen) {
    DWORD chunk = (DWORD)CURLMIN(inputlen - off, 0xffffffffUL);
    إذا(!Crypt_Hash_داده(hHash, درون_داد + off, chunk, 0))
      اذهب بیرون;
    off += chunk;
  }

  /* get hash size */
  cb_Hash_اندازه = 0;
  dw_Hash_اندازه_Len = (DWORD)حجم(cb_Hash_اندازه);
  إذا(!CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE *)&cb_Hash_اندازه,
                        &dw_Hash_اندازه_Len, 0))
    اذهب بیرون;

  /* check if hash fits into the return buffer */
  إذا(checksumlen < cb_Hash_اندازه)
    اذهب بیرون;

  dwChecksumLen = (DWORD)checksumlen;
  إذا(CryptGetHashParam(hHash, HP_HASHVAL, checksum, &dwChecksumLen, 0) &&
     dwChecksumLen == cb_Hash_اندازه)
    نتیجه = CURLE_OK;

بیرون:
  إذا(hHash)
    CryptDestroyHash(hHash);

  إذا(hProv)
    CryptReleaseContext(hProv, 0);

  أرجع نتیجه;
}

ساكن CURLcode schannel_sha256sum(ثابت بیعلامت حرف *درون_داد,
                                   size_t inputlen,
                                   بیعلامت حرف *sha256sum,
                                   size_t sha256len)
{
  أرجع schannel_checksum(درون_داد, inputlen, sha256sum, sha256len,
                           PROV_RSA_AES, CALG_SHA_256);
}

ساكن فراغ *schannel_get_internals(بنية ssl_connect_داده *connssl,
                                    CURLINFO info)
{
  بنية schannel_ssl_backend_داده *backend =
    (بنية schannel_ssl_backend_داده *)connssl->backend;
  (فراغ)info;
  DEBUGASSERT(backend);
  أرجع &backend->ctxt->ctxt_handle;
}

HCERTSTORE Curl_schannel_get_cached_cert_store(بنية Curl_cfilter *cf,
                                               بنية Curl_easy *داده)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_multi *multi = داده->multi;
  ثابت بنية curl_blob *ca_info_blob = conn_config->ca_info_blob;
  بنية schannel_cert_share *share;
  timediff_t timeout_ms;
  بیعلامت حرف info_blob_digest[CURL_SHA256_DIGEST_درازی];

  DEBUGASSERT(multi);

  إذا(!multi) {
    أرجع عدم;
  }

  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_SCHANNEL_CERT_SHARE_کلید,
                         CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_کلید));
  إذا(!share || !share->cert_store) {
    أرجع عدم;
  }

  /* zero ca_cache_timeout completely disables caching */
  إذا(!داده->مجموعه.ssl_ca_cache_timeout) {
    أرجع عدم;
  }

  /* check for cache timeout by using the cached_x509_store_expired timediff
     calculation pattern from openssl.c.
     negative timeout means retain forever. */
  timeout_ms = داده->مجموعه.ssl_ca_cache_timeout * (timediff_t)1000;
  إذا(timeout_ms >= 0) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(داده), &share->زمان);
    إذا(elapsed_ms >= timeout_ms) {
      أرجع عدم;
    }
  }

  إذا(ca_info_blob) {
    إذا(share->CAinfo_blob_اندازه != ca_info_blob->len) {
      أرجع عدم;
    }
    إذا(schannel_sha256sum((ثابت بیعلامت حرف *)ca_info_blob->داده,
                          ca_info_blob->len,
                          info_blob_digest,
                          CURL_SHA256_DIGEST_درازی))
      أرجع عدم;
    إذا(memcmp(share->CAinfo_blob_digest, info_blob_digest,
              CURL_SHA256_DIGEST_درازی)) {
      أرجع عدم;
    }
  }
  وإلا {
    إذا(!conn_config->CAfile || !share->CAfile ||
       strcmp(share->CAfile, conn_config->CAfile)) {
      أرجع عدم;
    }
  }

  أرجع share->cert_store;
}

ساكن فراغ schannel_cert_share_free(ثابت فراغ *کلید, size_t کلید_len, فراغ *p)
{
  بنية schannel_cert_share *share = p;
  DEBUGASSERT(کلید_len == CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_کلید));
  DEBUGASSERT(!memcmp(MPROTO_SCHANNEL_CERT_SHARE_کلید, کلید, کلید_len));
  (فراغ)کلید;
  (فراغ)کلید_len;
  إذا(share->cert_store) {
    CertCloseStore(share->cert_store, 0);
  }
  curlx_free(share->CAfile);
  curlx_free(share);
}

bool Curl_schannel_مجموعه_cached_cert_store(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *داده,
                                         HCERTSTORE cert_store)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_multi *multi = داده->multi;
  ثابت بنية curl_blob *ca_info_blob = conn_config->ca_info_blob;
  بنية schannel_cert_share *share;
  بیعلامت حرف digest[CURL_SHA256_DIGEST_درازی];
  حرف *CAfile = عدم;

  DEBUGASSERT(multi);

  إذا(!multi) {
    أرجع FALSE;
  }

  إذا(ca_info_blob) {
    إذا(schannel_sha256sum((ثابت بیعلامت حرف *)ca_info_blob->داده,
                          ca_info_blob->len, digest, حجم(digest))) {
      أرجع FALSE;
    }
  }
  وإلا إذا(conn_config->CAfile) {
    CAfile = curlx_strdup(conn_config->CAfile);
    إذا(!CAfile) {
      أرجع FALSE;
    }
  }

  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_SCHANNEL_CERT_SHARE_کلید,
                         CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_کلید));
  إذا(!share) {
    share = curlx_calloc(1, حجم(*share));
    إذا(!share) {
      curlx_free(CAfile);
      أرجع FALSE;
    }
    إذا(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_SCHANNEL_CERT_SHARE_کلید,
                       CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_کلید),
                       share, schannel_cert_share_free)) {
      curlx_free(share);
      curlx_free(CAfile);
      أرجع FALSE;
    }
  }

  /* free old cache data */
  إذا(share->cert_store) {
    CertCloseStore(share->cert_store, 0);
  }
  curlx_free(share->CAfile);

  إذا(ca_info_blob) {
    کپی_حافظه(share->CAinfo_blob_digest, digest, حجم(digest));
    share->CAinfo_blob_اندازه = ca_info_blob->len;
  }
  وإلا
    share->CAinfo_blob_اندازه = 0;

  share->زمان = curlx_now();
  share->cert_store = cert_store;
  share->CAfile = CAfile;
  أرجع TRUE;
}

ثابت بنية Curl_ssl Curl_ssl_schannel = {
  { CURLSSLBACKEND_SCHANNEL, "schannel" }, /* info */

  SSLSUPP_CERTINFO |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_CA_CACHE |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST,

  حجم(بنية schannel_ssl_backend_داده),

  schannel_init,                     /* init */
  schannel_cleanup,                  /* cleanup */
  schannel_نسخه,                  /*نسخه*/
  schannel_shutdown,                 /* shutdown */
  schannel_داده_pending,             /* data_pending */
  schannel_random,                   /* random */
  عدم,                              /* cert_status_request */
  schannel_connect,                  /* connect */
  Curl_ssl_adjust_pollset,           /* adjust_pollset */
  schannel_get_internals,            /* get_internals */
  schannel_close,                    /* close_one */
  عدم,                              /* close_all */
  عدم,                              /* engines_list */
  schannel_sha256sum,                /* sha256sum */
  schannel_recv,                     /* recv decrypted data */
  schannel_send,                     /* send data to encrypt */
  عدم,                              /* get_channel_binding */
};

#پایان_اگر /* USE_SCHANNEL */
