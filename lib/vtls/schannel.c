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

#جيڪڏهن_وصف_ٿيل USE_SCHANNEL

#جيڪڏهن_وصف_نه_ٿيل USE_WINDOWS_SSPI
#غلطي "cannot compile Schannel support without SSPI."
#جيڪڏهن_پورو

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
#جيڪڏهن_وصف_ٿيل CURL_SCHANNEL_DEV_DEBUG
#وصف_ڪريو SCH_DEV(x) x
#وصف_ڪريو SCH_DEV_SHOWBOOL(x)                                     \
  افعل {                                                          \
    infof(معلومات, "schannel: " #x " %s", (x) ? "TRUE" : "FALSE"); \
  } طالما(0)
#وإلا
#وصف_ڪريو SCH_DEV(x) افعل {} طالما(0)
#وصف_ڪريو SCH_DEV_SHOWBOOL(x) افعل {} طالما(0)
#جيڪڏهن_پورو

/* Offered by mingw-w64 v8+, MS SDK 7.0A/VS2010+ */
#جيڪڏهن_وصف_نه_ٿيل SP_PROT_TLS1_0_CLIENT
#وصف_ڪريو SP_PROT_TLS1_0_CLIENT           SP_PROT_TLS1_CLIENT
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل SP_PROT_TLS1_1_CLIENT
#وصف_ڪريو SP_PROT_TLS1_1_CLIENT           0x00000200
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل SP_PROT_TLS1_2_CLIENT
#وصف_ڪريو SP_PROT_TLS1_2_CLIENT           0x00000800
#جيڪڏهن_پورو

/* Offered by mingw-w64 v8+, MS SDK 10.0.15063.0/VS2017 15.1+ */
#جيڪڏهن_وصف_نه_ٿيل SP_PROT_TLS1_3_CLIENT
#وصف_ڪريو SP_PROT_TLS1_3_CLIENT           0x00002000
#جيڪڏهن_پورو
/* Offered by mingw-w64 v8+, MS SDK 8.1/VS2013+ */
#جيڪڏهن_وصف_نه_ٿيل SCH_USE_STRONG_CRYPTO
#وصف_ڪريو SCH_USE_STRONG_CRYPTO           0x00400000
#جيڪڏهن_پورو

/* Offered by mingw-w64 v10+, MS SDK 7.0A/VS2010+ */
#جيڪڏهن_وصف_نه_ٿيل SECBUFFER_ALERT
#وصف_ڪريو SECBUFFER_ALERT                 17
#جيڪڏهن_پورو

/* Both schannel buffer sizes must be > 0 */
#وصف_ڪريو CURL_SCHANNEL_بفر_INIT_سائيز   4096
#وصف_ڪريو CURL_SCHANNEL_بفر_FREE_سائيز   1024

#وصف_ڪريو CERT_THUMBPRINT_STR_LEN 40
#وصف_ڪريو CERT_THUMBPRINT_معلومات_LEN 20

/* Uncomment to force verbose output
 * #define infof(x, y, ...) curl_mprintf(y, __VA_ARGS__)
 * #define failf(x, y, ...) curl_mprintf(y, __VA_ARGS__)
 */

/* Offered by mingw-w64 v4+, MS SDK 6.0A/VS2008+ */
#جيڪڏهن_وصف_نه_ٿيل PKCS12_NO_PERSIST_ڪنجي
#وصف_ڪريو PKCS12_NO_PERSIST_ڪنجي 0x00008000
#جيڪڏهن_پورو

/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#جيڪڏهن_وصف_نه_ٿيل CERT_ڳوليو_HAS_ذاتي_ڪنجي
#وصف_ڪريو CERT_ڳوليو_HAS_ذاتي_ڪنجي (21 << CERT_COMPARE_سرڪايو)
#جيڪڏهن_پورو

/* key to use at `multi->proto_hash` */
#وصف_ڪريو MPROTO_SCHANNEL_CERT_SHARE_ڪنجي   "tls:schannel:cert:share"

ساكن bool s_win_has_alpn;

/* Offered by mingw-w64 v9+, MS SDK 8.1/VS2013+ */
#جيڪڏهن_وصف_نه_ٿيل SECBUFFER_APPLICATION_PROTOCOLS
#وصف_ڪريو SECBUFFER_APPLICATION_PROTOCOLS 18
#وصف_ڪريو SECPKG_ATTR_APPLICATION_PROTOCOL 35

تعريفنوع تعداد {
  SecApplicationProtocolNegotiationExt_None,
  SecApplicationProtocolNegotiationExt_NPN,
  SecApplicationProtocolNegotiationExt_ALPN
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT;

تعريفنوع تعداد {
  Sec_Application_Protocol_Negotiation_حالت_None,
  Sec_Application_Protocol_Negotiation_حالت_Success,
  Sec_Application_Protocol_Negotiation_حالت_Selected_Client_Only
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_حالت;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
تعريفنوع بنية {
  SEC_APPLICATION_PROTOCOL_NEGOTIATION_حالت Proto_Nego_حالت;
  SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT ProtoNegoExt;
  بنانشان حرف Protocol_Id_سائيز;
  بنانشان حرف ProtocolId[0xff];
} SecPkgContext_ApplicationProtocol;
#جيڪڏهن_پورو

ساكن فراغ Init_Sec_بفر(Sec_بفر *بفر, بنانشان ڊگهو BufType,
                          فراغ *Buf_معلومات_Ptr, بنانشان ڊگهو Buf_Byte_سائيز)
{
  بفر->cb_بفر = Buf_Byte_سائيز;
  بفر->بفر_Type = BufType;
  بفر->pv_بفر = Buf_معلومات_Ptr;
}

ساكن فراغ Init_Sec_بفر_Desc(Sec_بفر_Desc *desc, Sec_بفر *BufArr,
                              بنانشان ڊگهو انگ_Arr_Elem)
{
  desc->ul_نسخو = SECBUFFER_نسخو;
  desc->pBuffers = BufArr;
  desc->cBuffers = انگ_Arr_Elem;
}

ساكن CURLcode schannel_سيٽ_ssl_نسخو_min_max(DWORD *enabled_protocols,
                                                 بنية Curl_cfilter *cf,
                                                 بنية Curl_easy *معلومات)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ڊگهو ssl_نسخو = conn_config->نسخو_2;
  ڊگهو ssl_نسخو_max = (ڊگهو)conn_config->نسخو_max;
  ڊگهو i = ssl_نسخو;

  بدل(ssl_نسخو_max) {
  حالة CURL_SSLVERSION_MAX_NONE:
  حالة CURL_SSLVERSION_MAX_DEFAULT:

    /* Windows Server 2022 and newer (including Windows 11) support TLS 1.3
       built-in. Previous builds of Windows 10 had broken TLS 1.3
       implementations that could be enabled via registry. */
    إذا(curlx_verify_windows_نسخو(10, 0, 20348, PLATFORM_WINNT,
                                    نسخو_GREATER_THAN_EQUAL)) {
      ssl_نسخو_max = CURL_SSLVERSION_MAX_TLSv1_3;
    }
    وإلا /* Windows 10 or older */
      ssl_نسخو_max = CURL_SSLVERSION_MAX_TLSv1_2;

    اكسر;
  }

  لكل(; i <= (ssl_نسخو_max >> 16); ++i) {
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
      إذا(curlx_verify_windows_نسخو(10, 0, 20348, PLATFORM_WINNT,
                                      نسخو_GREATER_THAN_EQUAL)) {
        *enabled_protocols |= SP_PROT_TLS1_3_CLIENT;
        اكسر;
      }
      وإلا { /* Windows 10 or older */
        failf(معلومات, "schannel: TLS 1.3 not supported on Windows 10 or older");
        أرجع CURLE_SSL_CONNECT_ERROR;
      }
    }
  }
  أرجع CURLE_OK;
}

#وصف_ڪريو CIPHEROPTION(x) { #x, x }

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
  CIPHEROPTION(CALG_SCHANNEL_MAC_ڪنجي),
  CIPHEROPTION(CALG_SCHANNEL_ENC_ڪنجي),
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
#جيڪڏهن_وصف_ٿيل CALG_ECMQV
  CIPHEROPTION(CALG_ECMQV),
#جيڪڏهن_پورو
  CIPHEROPTION(CALG_ECDSA),
/* Offered by mingw-w64 v7+, MS SDK 7.0A/VS2010+ */
#جيڪڏهن_وصف_ٿيل CALG_ECDH_EPHEM
  CIPHEROPTION(CALG_ECDH_EPHEM),
#جيڪڏهن_پورو
  { عدم, 0 },
};

ساكن صحيح get_alg_id_by_name(ثابت حرف *name)
{
  ثابت حرف *name_پڇاڙي = strchr(name, ':');
  size_t n = name_پڇاڙي ? (size_t)(name_پڇاڙي - name) : لڙي_ڊيگهه(name);
  صحيح i;

  لكل(i = 0; algs[i].name; i++) {
    إذا((n == لڙي_ڊيگهه(algs[i].name) && !لڙي_ڀيٽ(algs[i].name, name, n)))
      أرجع algs[i].id;
  }
  أرجع 0; /* not found */
}

#وصف_ڪريو انگ_CIPHERS 47 /* There are a maximum of 47 options listed above */

ساكن CURLcode سيٽ_ssl_ciphers(SCHANNEL_CRED *schannel_cred, حرف *ciphers,
                                ALG_ID *algIds)
{
  ثابت حرف *شروعات_Cur = ciphers;
  صحيح alg_ڳڻپ = 0;
  طالما(شروعات_Cur && *شروعات_Cur && (alg_ڳڻپ < انگ_CIPHERS)) {
    curl_off_t alg;
    إذا(curlx_str_عدد(&شروعات_Cur, &alg, وڌ_کان_وڌ_عدد) || !alg)
      alg = get_alg_id_by_name(شروعات_Cur);

    إذا(alg)
      algIds[alg_ڳڻپ++] = (ALG_ID)alg;
    وإلا إذا(!لڙي_ڀيٽ(شروعات_Cur, "USE_STRONG_CRYPTO",
                     CURL_CSTRLEN("USE_STRONG_CRYPTO")) ||
            !لڙي_ڀيٽ(شروعات_Cur, "SCH_USE_STRONG_CRYPTO",
                     CURL_CSTRLEN("SCH_USE_STRONG_CRYPTO")))
      schannel_cred->dwFlags |= SCH_USE_STRONG_CRYPTO;
    وإلا
      أرجع CURLE_SSL_CIPHER;
    شروعات_Cur = strchr(شروعات_Cur, ':');
    إذا(شروعات_Cur)
      شروعات_Cur++;
  }
  schannel_cred->palgSupportedAlgs = algIds;
  schannel_cred->cSupportedAlgs = (DWORD)alg_ڳڻپ;
  أرجع CURLE_OK;
}

/* Function allocates memory for store_path only if CURLE_OK is returned */
ساكن CURLcode get_cert_location(TCHAR *رستو, DWORD *store_name,
                                  TCHAR **store_رستو, TCHAR **thumbprint)
{
  TCHAR *sep;
  TCHAR *store_رستو_شروعات;
  size_t store_name_len;

  sep = _tcschr(رستو, متن('\\'));
  إذا(!sep)
    أرجع CURLE_SSL_CERTPROBLEM;

  store_name_len = sep - رستو;

  إذا(_tcsncmp(رستو, متن("CurrentUser"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_USER;
  وإلا إذا(_tcsncmp(رستو, متن("LocalMachine"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE;
  وإلا إذا(_tcsncmp(رستو, متن("CurrentService"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_SERVICE;
  وإلا إذا(_tcsncmp(رستو, متن("Services"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_SERVICES;
  وإلا إذا(_tcsncmp(رستو, متن("Users"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_USERS;
  وإلا إذا(_tcsncmp(رستو, متن("CurrentUserGroupPolicy"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_USER_گروهه_POLICY;
  وإلا إذا(_tcsncmp(رستو, متن("LocalMachineGroupPolicy"), store_name_len) ==
          0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE_گروهه_POLICY;
  وإلا إذا(_tcsncmp(رستو, متن("LocalMachineEnterprise"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE_ENTERPRISE;
  وإلا
    أرجع CURLE_SSL_CERTPROBLEM;

  store_رستو_شروعات = sep + 1;

  sep = _tcschr(store_رستو_شروعات, متن('\\'));
  إذا(!sep)
    أرجع CURLE_SSL_CERTPROBLEM;

  *thumbprint = sep + 1;
  إذا(_tcslen(*thumbprint) != CERT_THUMBPRINT_STR_LEN)
    أرجع CURLE_SSL_CERTPROBLEM;

  *sep = متن('\0');
  *store_رستو = curlx_tcsdup(store_رستو_شروعات);
  *sep = متن('\\');
  إذا(!*store_رستو)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  أرجع CURLE_OK;
}

ساكن CURLcode get_client_cert(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                HCERTSTORE *ٻاهريون_cert_store,
                                PCCERT_CONTEXT *ٻاهريون_cert_context)
{
  بنية ssl_filter_config *sslc = Curl_ssl_cf_get_filter_config(cf);
  PCCERT_CONTEXT client_cert = عدم;
  HCERTSTORE client_cert_store = عدم;
  CURLcode نتيجو = CURLE_OK;

  إذا(sslc->clientcert || sslc->cert_blob) {
    DWORD cert_store_name = 0;
    TCHAR *cert_store_رستو = عدم;
    TCHAR *cert_thumbprint_str = عدم;
    TCHAR cert_thumbprint_buf[CERT_THUMBPRINT_STR_LEN + 1];
    CRYPT_HASH_BLOB cert_thumbprint;
    BYTE cert_thumbprint_معلومات[CERT_THUMBPRINT_معلومات_LEN];
    HCERTSTORE cert_store = عدم;
    FILE *f_اندريون_Cert = عدم;
    فراغ *certdata = عدم;
    size_t certsize = 0;
    bool blob = !!sslc->cert_blob;

    إذا(blob) {
      certdata = sslc->cert_blob->معلومات;
      certsize = sslc->cert_blob->len;
    }
    وإلا {
      TCHAR *cert_رستو =
        curlx_convert_UTF8_منزل_tchar(sslc->clientcert);
      إذا(!cert_رستو)
        أرجع CURLE_ٻاهريون_جو_يادگيري;

      نتيجو = get_cert_location(cert_رستو, &cert_store_name,
                                 &cert_store_رستو, &cert_thumbprint_str);

      /* 'cert_thumbprint_str' points in to the allocated 'cert_path', copy
         the data. The string is verified to be CERT_THUMBPRINT_STR_LEN bytes
         long within the get_cert_location() function. */
      إذا(!نتيجو && cert_thumbprint_str) {
        يادگيري_نقل(cert_thumbprint_buf, cert_thumbprint_str,
               حجم(cert_thumbprint_buf));
        cert_thumbprint_str = cert_thumbprint_buf;
      }

      curlx_free(cert_رستو);
      إذا(نتيجو && (sslc->clientcert[0] != '\0'))
        f_اندريون_Cert = curlx_fopen(sslc->clientcert, "rb");

      إذا(نتيجو && !f_اندريون_Cert) {
        failf(معلومات, "schannel: Failed to get certificate location"
              " or file for %s",
              sslc->clientcert);
        أرجع نتيجو;
      }
    }

    إذا((f_اندريون_Cert || blob) && sslc->cert_type &&
       !curl_strequal(sslc->cert_type, "P12")) {
      failf(معلومات, "schannel: certificate format compatibility error "
            "for %s",
            blob ? "(memory blob)" : sslc->clientcert);
      curlx_free(cert_store_رستو);
      إذا(f_اندريون_Cert)
        curlx_fclose(f_اندريون_Cert);
      أرجع CURLE_SSL_CERTPROBLEM;
    }

    إذا(f_اندريون_Cert || blob) {
      /* Reading a .p12 or .pfx file, like the example at bottom of
         https://learn.microsoft.com/archive/msdn-technet-forums/3e7bc95f-b21a-4bcd-bd2c-7f996718cae5
       */
      CRYPT_معلومات_BLOB datablob;
      WCHAR *pszPassword;
      size_t pwd_len = 0;
      صحيح cert_ڳوليو_flags;
      ثابت حرف *cert_showfilename_error = blob ?
        "(memory blob)" : sslc->clientcert;
      curlx_free(cert_store_رستو);
      إذا(f_اندريون_Cert) {
        ڊگهو cert_tell = 0;
        bool continue_reading = fseek(f_اندريون_Cert, 0, SEEK_END) == 0;
        إذا(continue_reading) {
          cert_tell = ftell(f_اندريون_Cert);
          إذا(cert_tell < 0)
            continue_reading = FALSE;
          وإلا
            certsize = (size_t)cert_tell;
        }
        إذا(continue_reading)
          continue_reading = fseek(f_اندريون_Cert, 0, SEEK_SET) == 0;
        إذا(continue_reading && (certsize < CURL_MAX_داخلا_ڊيگهائي))
          certdata = curlx_malloc(certsize + 1);
        إذا(!certdata || ((صحيح)fread(certdata, certsize, 1, f_اندريون_Cert) != 1))
          continue_reading = FALSE;
        curlx_fclose(f_اندريون_Cert);
        إذا(!continue_reading) {
          failf(معلومات, "schannel: Failed to read cert file %s",
                sslc->clientcert);
          curlx_free(certdata);
          أرجع CURLE_SSL_CERTPROBLEM;
        }
      }

      /* Convert key-pair data to the in-memory certificate store */
      datablob.pb_معلومات = (BYTE *)certdata;
      datablob.cb_معلومات = (DWORD)certsize;

      إذا(sslc->ڪنجي_passwd)
        pwd_len = لڙي_ڊيگهه(sslc->ڪنجي_passwd);
      pszPassword = curlx_malloc(حجم(WCHAR) * (pwd_len + 1));
      إذا(pszPassword) {
        صحيح str_w_len = 0;
        إذا(pwd_len > 0)
          str_w_len = Multi_Byte_منزل_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                          sslc->ڪنجي_passwd, (صحيح)pwd_len,
                                          pszPassword, (صحيح)(pwd_len + 1));

        إذا((str_w_len >= 0) && (str_w_len <= (صحيح)pwd_len))
          pszPassword[str_w_len] = 0;
        وإلا
          pszPassword[0] = 0;

        cert_store = PFXImportCertStore(&datablob, pszPassword,
                                        PKCS12_NO_PERSIST_ڪنجي);
        curlx_memzero(pszPassword, حجم(WCHAR) * (pwd_len + 1));
        curlx_free(pszPassword);
      }
      إذا(!blob)
        curlx_free(certdata);
      إذا(!cert_store) {
        DWORD errorcode = Get_آخري_Error();
        إذا(errorcode == ERROR_INVALID_PASSWORD)
          failf(معلومات, "schannel: Failed to import cert file %s, "
                "password is bad",
                cert_showfilename_error);
        وإلا
          failf(معلومات, "schannel: Failed to import cert file %s, "
                "last error is 0x%08lx",
                cert_showfilename_error, errorcode);
        أرجع CURLE_SSL_CERTPROBLEM;
      }

      /* CERT_FIND_HAS_PRIVATE_KEY is only available in Windows 8 / Server
         2012, (NT 6.2). For older versions we use CURL_FIND_ANY. */
      إذا(curlx_verify_windows_نسخو(6, 2, 0, PLATFORM_WINNT,
                                      نسخو_GREATER_THAN_EQUAL))
        cert_ڳوليو_flags = CERT_ڳوليو_HAS_ذاتي_ڪنجي;
      وإلا
        cert_ڳوليو_flags = CERT_ڳوليو_ANY;

      client_cert =
        Cert_ڳوليو_Certificate_اندريون_Store(cert_store,
                                   X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                   cert_ڳوليو_flags, عدم, عدم);

      إذا(!client_cert) {
        failf(معلومات, "schannel: Failed to get certificate from file %s"
              ", last error is 0x%08lx",
              cert_showfilename_error, Get_آخري_Error());
        CertCloseStore(cert_store, 0);
        أرجع CURLE_SSL_CERTPROBLEM;
      }
    }
    وإلا {
      cert_store =
        CertOpenStore(
#جيڪڏهن_وصف_ٿيل UNICODE
                      CERT_STORE_PROV_SYSTEM_W,
#وإلا
                      CERT_STORE_PROV_SYSTEM_A,
#جيڪڏهن_پورو
                      0,
                      (HCRYPTPROV)عدم,
                      CERT_STORE_OPEN_EXISTING_FLAG | cert_store_name,
                      cert_store_رستو);
      إذا(!cert_store) {
        حرف *رستو_utf8 =
          curlx_convert_tchar_منزل_UTF8(cert_store_رستو);
        failf(معلومات, "schannel: Failed to open cert store %lx %s, "
              "last error is 0x%08lx",
              cert_store_name, (رستو_utf8 ? رستو_utf8 : "(unknown)"),
              Get_آخري_Error());
        curlx_free(cert_store_رستو);
        curlx_free(رستو_utf8);
        أرجع CURLE_SSL_CERTPROBLEM;
      }
      curlx_free(cert_store_رستو);

      cert_thumbprint.pb_معلومات = cert_thumbprint_معلومات;
      cert_thumbprint.cb_معلومات = CERT_THUMBPRINT_معلومات_LEN;

      إذا(!Crypt_String_منزل_ٻٽو(cert_thumbprint_str,
                              CERT_THUMBPRINT_STR_LEN,
                              CRYPT_STRING_HEX,
                              cert_thumbprint_معلومات,
                              &cert_thumbprint.cb_معلومات,
                              عدم, عدم)) {
        CertCloseStore(cert_store, 0);
        أرجع CURLE_SSL_CERTPROBLEM;
      }

      client_cert =
        Cert_ڳوليو_Certificate_اندريون_Store(cert_store,
                                   X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                   CERT_ڳوليو_HASH, &cert_thumbprint, عدم);
      إذا(!client_cert) {
        /* CRYPT_E_NOT_FOUND / E_INVALIDARG */
        CertCloseStore(cert_store, 0);
        failf(معلومات, "schannel: client cert not found in cert store");
        أرجع CURLE_SSL_CERTPROBLEM;
      }
    }
    client_cert_store = cert_store;
  }

  *ٻاهريون_cert_store = client_cert_store;
  *ٻاهريون_cert_context = client_cert;

  أرجع CURLE_OK;
}

ساكن CURLcode acquire_sspi_handle(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    بنية schannel_ssl_backend_معلومات *backend,
                                    PCCERT_CONTEXT client_cert,
                                    DWORD flags,
                                    DWORD enabled_protocols)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SECURITY_حالت sspi_حالت = SEC_E_OK;
  CURLcode نتيجو;

  /* We support TLS 1.3 starting in Windows Server 2022 or later
   * (OS build 20348) */
  إذا(!conn_config->cipher_list &&
     curlx_verify_windows_نسخو(10, 0, 20348, PLATFORM_WINNT,
                                  نسخو_GREATER_THAN_EQUAL)) {

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

    credentials.dw_نسخو = SCH_CREDENTIALS_نسخو;
    credentials.dwFlags = flags | SCH_USE_STRONG_CRYPTO;

    credentials.pTlsParameters->grbitDisabledProtocols = ~enabled_protocols;

    إذا(client_certs[0]) {
      credentials.cCreds = 1;
      credentials.paCred = client_certs;
    }

    sspi_حالت =
      Curl_pSecFn->AcquireCredentialsHandle(عدم, CURL_UNCONST(UNISP_NAME),
                                            SECPKG_CRED_OUTBOUND, عدم,
                                            &credentials, عدم, عدم,
                                            &backend->cred->cred_handle, عدم);
  }
  وإلا {
    /* Pre-Windows 10 1809 or the user set a legacy algorithm list.
       Schannel does not negotiate TLS 1.3 when SCHANNEL_CRED is used. */
    ALG_ID algIds[انگ_CIPHERS];
    حرف *ciphers = conn_config->cipher_list;
    SCHANNEL_CRED schannel_cred = { 0 };
    PCCERT_CONTEXT client_certs[1];

    إذا(client_cert)
      client_certs[0] = client_cert;
    وإلا
      client_certs[0] = عدم;

    schannel_cred.dw_نسخو = SCHANNEL_CRED_نسخو;
    schannel_cred.dwFlags = flags;
    schannel_cred.grbitEnabledProtocols = enabled_protocols;

    إذا(ciphers) {
      إذا((enabled_protocols & SP_PROT_TLS1_3_CLIENT)) {
        إذا(!(enabled_protocols & ~SP_PROT_TLS1_3_CLIENT)) {
          failf(معلومات, "schannel: TLS 1.3 is not supported with a cipher list; "
                "remove the cipher list or allow a lower TLS version");
          أرجع CURLE_SSL_CONNECT_ERROR;
        }
        infof(معلومات, "schannel: WARNING: This version of Schannel "
              "negotiates a less-secure TLS version than TLS 1.3 because the "
              "user set an algorithm cipher list.");
      }
      نتيجو = سيٽ_ssl_ciphers(&schannel_cred, ciphers, algIds);
      إذا(نتيجو) {
        failf(معلومات, "schannel: Failed setting algorithm cipher list");
        أرجع نتيجو;
      }
    }
    وإلا {
      schannel_cred.dwFlags = flags | SCH_USE_STRONG_CRYPTO;
    }

    إذا(client_certs[0]) {
      schannel_cred.cCreds = 1;
      schannel_cred.paCred = client_certs;
    }

    sspi_حالت =
      Curl_pSecFn->AcquireCredentialsHandle(عدم, CURL_UNCONST(UNISP_NAME),
                                            SECPKG_CRED_OUTBOUND, عدم,
                                            &schannel_cred, عدم, عدم,
                                            &backend->cred->cred_handle, عدم);
  }

  إذا(sspi_حالت != SEC_E_OK) {
    حرف بفر[STRERROR_LEN];
    failf(معلومات, "schannel: AcquireCredentialsHandle failed: %s",
          Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
    بدل(sspi_حالت) {
    حالة SEC_E_INSUFFICIENT_يادگيري:
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    حالة SEC_E_NO_CREDENTIALS:
    حالة SEC_E_SECPKG_NOT_مليو:
    حالة SEC_E_NOT_OWNER:
    حالة SEC_E_UNKNOWN_CREDENTIALS:
    حالة SEC_E_اندروني_ERROR:
    افتراضي:
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
  }

  أرجع CURLE_OK;
}

ساكن CURLcode schannel_acquire_credential_handle(بنية Curl_cfilter *cf,
                                                   بنية Curl_easy *معلومات)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  PCCERT_CONTEXT client_cert = عدم;
  HCERTSTORE client_cert_store = عدم;
  CURLcode نتيجو;

  /* setup Schannel API options */
  DWORD flags = 0;
  DWORD enabled_protocols = 0;

  بنية schannel_ssl_backend_معلومات *backend =
    (بنية schannel_ssl_backend_معلومات *)connssl->backend;

  DEBUGASSERT(backend);

  إذا(conn_config->verifypeer) {
    إذا(backend->use_manual_cred_validation)
      flags = SCH_CRED_MANUAL_CRED_VALIDATION;
    وإلا
      flags = SCH_CRED_AUTO_CRED_VALIDATION;

    إذا(conn_config->no_revoke) {
      flags |= SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
               SCH_CRED_IGNORE_REVOCATION_OFFLINE;

      DEBUGF(infof(معلومات, "schannel: disabled server certificate revocation "
                         "checks"));
    }
    وإلا إذا(conn_config->revoke_بهترين_effort) {
      flags |= SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
               SCH_CRED_IGNORE_REVOCATION_OFFLINE |
               SCH_CRED_REVOCATION_CHECK_CHAIN;

      DEBUGF(infof(معلومات, "schannel: ignore revocation offline errors"));
    }
    وإلا {
      flags |= SCH_CRED_REVOCATION_CHECK_CHAIN;

      DEBUGF(infof(معلومات, "schannel: checking server certificate revocation"));
    }
  }
  وإلا {
    flags = SCH_CRED_MANUAL_CRED_VALIDATION |
            SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
            SCH_CRED_IGNORE_REVOCATION_OFFLINE;
    DEBUGF(infof(معلومات, "schannel: disabled server cert revocation checks"));
  }

  إذا(!conn_config->verifyhost) {
    flags |= SCH_CRED_NO_SERVERNAME_CHECK;
    DEBUGF(infof(معلومات, "schannel: verifyhost setting prevents Schannel from "
                       "comparing the supplied target name with the subject "
                       "names in server certificates."));
  }

  إذا(!conn_config->auto_client_cert) {
    flags &= ~(DWORD)SCH_CRED_USE_DEFAULT_CREDS;
    flags |= SCH_CRED_NO_DEFAULT_CREDS;
    infof(معلومات, "schannel: disabled automatic use of client certificate");
  }
  وإلا
    infof(معلومات, "schannel: enabled automatic use of client certificate");

  DEBUGASSERT(conn_config->نسخو_2 != CURL_SSLVERSION_DEFAULT);
  بدل(conn_config->نسخو_2) {
  حالة CURL_SSLVERSION_TLSv1:
  حالة CURL_SSLVERSION_TLSv1_0:
  حالة CURL_SSLVERSION_TLSv1_1:
  حالة CURL_SSLVERSION_TLSv1_2:
  حالة CURL_SSLVERSION_TLSv1_3: {
    نتيجو = schannel_سيٽ_ssl_نسخو_min_max(&enabled_protocols, cf, معلومات);
    إذا(نتيجو)
      أرجع نتيجو;
    اكسر;
  }
  حالة CURL_SSLVERSION_SSLv3:
  حالة CURL_SSLVERSION_SSLv2:
    failf(معلومات, "SSL versions not supported");
    أرجع CURLE_NOT_BUILT_اندريون;
  افتراضي:
    failf(معلومات, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  نتيجو = get_client_cert(cf, معلومات, &client_cert_store, &client_cert);
  إذا(نتيجو)
    أرجع نتيجو;

  /* allocate memory for the reusable credential handle */
  backend->cred = (بنية Curl_schannel_cred *)
    curlx_calloc(1, حجم(بنية Curl_schannel_cred));
  إذا(!backend->cred) {
    failf(معلومات, "schannel: unable to allocate memory");

    إذا(client_cert)
      CertFreeCertificateContext(client_cert);
    إذا(client_cert_store)
      CertCloseStore(client_cert_store, 0);

    أرجع CURLE_ٻاهريون_جو_يادگيري;
  }
  backend->cred->refcount = 1;

  /* Since we did not persist the key, we need to extend the store's
   * lifetime until the end of the connection
   */
  backend->cred->client_cert_store = client_cert_store;

  نتيجو = acquire_sspi_handle(cf, معلومات, backend, client_cert,
                               flags, enabled_protocols);

  إذا(client_cert)
    CertFreeCertificateContext(client_cert);

  أرجع نتيجو;
}

ساكن CURLcode schannel_connect_step1(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *معلومات)
{
  size_t written = 0;
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية schannel_ssl_backend_معلومات *backend =
    (بنية schannel_ssl_backend_معلومات *)connssl->backend;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  Sec_بفر outbuf;
  Sec_بفر_Desc outbuf_desc;
  Sec_بفر inbuf;
  Sec_بفر_Desc inbuf_desc;
  بنانشان حرف alpn_بفر[128];
  SECURITY_حالت sspi_حالت = SEC_E_OK;
  CURLcode نتيجو;

  DEBUGASSERT(backend);
  DEBUGF(infof(معلومات, "schannel: SSL/TLS connection with %s port %d (step 1/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->پورٽ));

  backend->use_alpn = connssl->alpn && s_win_has_alpn;

  إذا(conn_config->CAfile || conn_config->ca_info_blob) {
    إذا(curlx_verify_windows_نسخو(6, 1, 0, PLATFORM_WINNT,
                                    نسخو_GREATER_THAN_EQUAL)) {
      backend->use_manual_cred_validation = TRUE;
    }
    وإلا {
      failf(معلومات, "schannel: this version of Windows is too old to support "
                  "certificate verification via CA bundle file.");
      أرجع CURLE_SSL_CACERT_BADFILE;
    }
  }
  وإلا
    backend->use_manual_cred_validation = FALSE;

  backend->cred = عدم;

  /* check for an existing reusable credential handle */
  إذا(Curl_ssl_scache_use(cf, معلومات)) {
    بنية Curl_schannel_cred *old_cred;
    Curl_ssl_scache_تالو(معلومات);
    old_cred = Curl_ssl_scache_get_obj(cf, معلومات, connssl->peer.scache_ڪنجي);
    إذا(old_cred) {
      backend->cred = old_cred;
      DEBUGF(infof(معلومات, "schannel: reusing existing credential handle"));

      /* increment the reference counter of the credential/session handle */
      backend->cred->refcount++;
      DEBUGF(infof(معلومات,
                   "schannel: incremented credential handle refcount = %d",
                   backend->cred->refcount));
    }
    Curl_ssl_scache_unlock(معلومات);
  }

  إذا(!backend->cred) {
    حرف *snihost;
    نتيجو = schannel_acquire_credential_handle(cf, معلومات);
    إذا(نتيجو || !backend->cred)
      أرجع نتيجو;
    /* schannel_acquire_credential_handle() sets backend->cred accordingly or
       it returns error otherwise. */

    /* A hostname associated with the credential is needed by
       InitializeSecurityContext for SNI and other reasons. */
    snihost = connssl->peer.sni ?
      connssl->peer.sni : connssl->peer.origin->hostname;
    backend->cred->sni_hostname = curlx_convert_UTF8_منزل_tchar(snihost);
    إذا(!backend->cred->sni_hostname)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
  }

  /* Warn if SNI is disabled due to use of an IP address */
  إذا(connssl->peer.type != CURL_SSL_PEER_DNS) {
    infof(معلومات, "schannel: using IP address, SNI is not supported by OS.");
  }

  إذا(backend->use_alpn) {
    صحيح cur = 0;
    صحيح list_شروعات_اشاريو = 0;
    بنانشان صحيح *extension_len = عدم;
    بنانشان ننڍو *list_len = عدم;
    ثابت بنانشان صحيح uiALPN = SecApplicationProtocolNegotiationExt_ALPN;
    بنية alpn_proto_buf proto;

    /* The first four bytes is an unsigned int indicating number
       of bytes of data in the rest of the buffer. */
    extension_len = (بنانشان صحيح *)(فراغ *)&alpn_بفر[cur];
    cur += (صحيح)حجم(بنانشان صحيح);

    /* The next four bytes are an indicator that this buffer contains
       ALPN data, as opposed to NPN, for example. */
    يادگيري_نقل(&alpn_بفر[cur], &uiALPN, حجم(uiALPN));
    cur += (صحيح)حجم(بنانشان صحيح);

    /* The next two bytes is an unsigned short indicating the number
       of bytes used to list the preferred protocols. */
    list_len = (بنانشان ننڍو *)(فراغ *)&alpn_بفر[cur];
    cur += (صحيح)حجم(بنانشان ننڍو);

    list_شروعات_اشاريو = cur;

    نتيجو = Curl_alpn_منزل_proto_buf(&proto, connssl->alpn);
    إذا(نتيجو) {
      failf(معلومات, "Error setting ALPN");
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
    يادگيري_نقل(&alpn_بفر[cur], proto.معلومات, proto.len);
    cur += proto.len;

    *list_len = curlx_uitous(cur - list_شروعات_اشاريو);
    *extension_len = (بنانشان صحيح)(*list_len +
      حجم(بنانشان صحيح) + حجم(بنانشان ننڍو));

    Init_Sec_بفر(&inbuf, SECBUFFER_APPLICATION_PROTOCOLS, alpn_بفر, cur);
    Init_Sec_بفر_Desc(&inbuf_desc, &inbuf, 1);

    Curl_alpn_منزل_proto_str(&proto, connssl->alpn);
    infof(معلومات, VTLS_INFOF_ALPN_OFFER_1STR, proto.معلومات);
  }
  وإلا {
    Init_Sec_بفر(&inbuf, SECBUFFER_خالي_جايون, عدم, 0);
    Init_Sec_بفر_Desc(&inbuf_desc, &inbuf, 1);
  }

  /* setup output buffer */
  Init_Sec_بفر(&outbuf, SECBUFFER_خالي_جايون, عدم, 0);
  Init_Sec_بفر_Desc(&outbuf_desc, &outbuf, 1);

  /* security request flags */
  backend->req_flags =
    ISC_REQ_SEQUENCE_DETECT | ISC_REQ_REPLAY_DETECT |
    ISC_REQ_CONFIDENTIALITY | ISC_REQ_ALLOCATE_يادگيري |
    ISC_REQ_وهڪرو |
    (!conn_config->auto_client_cert ? ISC_REQ_USE_SUPPLIED_CREDS : 0);

  /* allocate memory for the security context handle */
  backend->ctxt = (بنية Curl_schannel_ctxt *)
    curlx_calloc(1, حجم(بنية Curl_schannel_ctxt));
  إذا(!backend->ctxt) {
    failf(معلومات, "schannel: unable to allocate memory");
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  }

  /* Schannel InitializeSecurityContext:
     https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-initializesecuritycontextw

     At the moment we do not pass inbuf unless we are using ALPN since we only
     use it for that, and WINE (for which we currently disable ALPN) is giving
     us problems with inbuf regardless. https://github.com/curl/curl/issues/983
   */
  sspi_حالت = Curl_pSecFn->InitializeSecurityContext(
    &backend->cred->cred_handle, عدم, backend->cred->sni_hostname,
    backend->req_flags, 0, 0,
    (backend->use_alpn ? &inbuf_desc : عدم),
    0, &backend->ctxt->ctxt_handle,
    &outbuf_desc, &backend->ret_flags, عدم);

  إذا(sspi_حالت != SEC_I_CONTINUE_NEEDED) {
    حرف بفر[STRERROR_LEN];
    curlx_safefree(backend->ctxt);
    بدل(sspi_حالت) {
    حالة SEC_E_INSUFFICIENT_يادگيري:
      failf(معلومات, "schannel: initial InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    حالة SEC_E_WRONG_PRINCIPAL:
      failf(معلومات, "schannel: SNI or certificate check failed: %s",
            Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
      أرجع CURLE_PEER_FAILED_VERIFICATION;
#إذا 0
    حالة SEC_E_INVALID_HANDLE:
    حالة SEC_E_INVALID_TOKEN:
    حالة SEC_E_LOGON_DENIED:
    حالة SEC_E_هدف_UNKNOWN:
    حالة SEC_E_NO_AUTHENTICATING_AUTHORITY:
    حالة SEC_E_اندروني_ERROR:
    حالة SEC_E_NO_CREDENTIALS:
    حالة SEC_E_UNSUPPORTED_FUNCTION:
    حالة SEC_E_APPLICATION_PROTOCOL_MISMATCH:
#جيڪڏهن_پورو
    افتراضي:
      failf(معلومات, "schannel: initial InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
  }

  DEBUGF(infof(معلومات, "schannel: sending initial handshake data: "
               "sending %lu bytes.", outbuf.cb_بفر));

  /* send initial handshake data which is now stored in output buffer */
  نتيجو = Curl_conn_cf_send(cf->ايندڙ, معلومات,
                             (ثابت uint8_t *)outbuf.pv_بفر,
                             outbuf.cb_بفر, FALSE,
                             &written);
  Curl_pSecFn->Free_Context_بفر(outbuf.pv_بفر);
  إذا(نتيجو || (outbuf.cb_بفر != written)) {
    failf(معلومات, "schannel: failed to send initial handshake data: "
          "sent %zu of %lu bytes", written, outbuf.cb_بفر);
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGF(infof(معلومات, "schannel: sent initial handshake data: "
               "sent %zu bytes", written));

  backend->recv_unrecoverable_err = CURLE_OK;
  backend->recv_sspi_close_notify = FALSE;
  backend->recv_connection_closed = FALSE;
  backend->recv_renegotiating = FALSE;
  backend->renegotiate_state.started = FALSE;
  backend->encdata_is_incomplete = FALSE;
  backend->shutdown_generated = FALSE;
  backend->shutdown_وٿي = 0;
  memset(&backend->shutdown_token, 0, حجم(backend->shutdown_token));

  /* continue to second handshake step */
  connssl->connecting_state = ssl_connect_2;

  أرجع CURLE_OK;
}

ساكن CURLcode schannel_error(بنية Curl_easy *معلومات,
                               SECURITY_حالت sspi_حالت)
{
  حرف بفر[STRERROR_LEN];
  بدل(sspi_حالت) {
  حالة SEC_E_INSUFFICIENT_يادگيري:
    failf(معلومات, "schannel: next InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  حالة SEC_E_WRONG_PRINCIPAL:
    failf(معلومات, "schannel: SNI or certificate check failed: %s",
          Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
    أرجع CURLE_PEER_FAILED_VERIFICATION;
  حالة SEC_E_UNTRUSTED_پاڙ:
    failf(معلومات, "schannel: %s",
          Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
    أرجع CURLE_PEER_FAILED_VERIFICATION;
#إذا 0
  حالة SEC_E_INVALID_HANDLE:
  حالة SEC_E_INVALID_TOKEN:
  حالة SEC_E_LOGON_DENIED:
  حالة SEC_E_هدف_UNKNOWN:
  حالة SEC_E_NO_AUTHENTICATING_AUTHORITY:
  حالة SEC_E_اندروني_ERROR:
  حالة SEC_E_NO_CREDENTIALS:
  حالة SEC_E_UNSUPPORTED_FUNCTION:
  حالة SEC_E_APPLICATION_PROTOCOL_MISMATCH:
#جيڪڏهن_پورو
  افتراضي:
    failf(معلومات, "schannel: next InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
    أرجع CURLE_SSL_CONNECT_ERROR;
  }
}

ساكن CURLcode schannel_pkp_pin_peer_pubkey(بنية Curl_cfilter *cf,
                                             بنية Curl_easy *معلومات,
                                             ثابت حرف *pinnedpubkey)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية schannel_ssl_backend_معلومات *backend =
    (بنية schannel_ssl_backend_معلومات *)connssl->backend;
  CERT_CONTEXT *pCertContextServer = عدم;

  /* Result is returned to caller */
  CURLcode نتيجو = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  DEBUGASSERT(backend);

  /* if a path was not specified, do not pin */
  إذا(!pinnedpubkey)
    أرجع CURLE_OK;

  افعل {
    SECURITY_حالت sspi_حالت;
    uint8_t *x509_der;
    DWORD x509_der_len;
    بنية Curl_X509certificate x509_parsed;
    بنية Curl_asn1Element *pubkey;

    sspi_حالت =
      Curl_pSecFn->Query_Context_خاصيتون(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                          &pCertContextServer);

    إذا((sspi_حالت != SEC_E_OK) || !pCertContextServer) {
      حرف بفر[STRERROR_LEN];
      failf(معلومات, "schannel: Failed to read remote certificate context: %s",
            Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
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

    pubkey = &x509_parsed.subject_عوامي_ڪنجي_Info;
    إذا(!pubkey->سرو || pubkey->پڇاڙي <= pubkey->سرو) {
      failf(معلومات, "SSL: failed retrieving public key from server certificate");
      اكسر;
    }

    نتيجو = Curl_pin_peer_pubkey(معلومات,
                                  pinnedpubkey,
                                  (ثابت بنانشان حرف *)pubkey->سرو,
                                  (size_t)(pubkey->پڇاڙي - pubkey->سرو));
    إذا(نتيجو) {
      failf(معلومات, "SSL: public key does not match pinned public key");
    }
  } طالما(0);

  إذا(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  أرجع نتيجو;
}

ساكن CURLcode ensure_encoding_سائيز(بنية Curl_easy *معلومات,
                                     بنية sbuffer *encdata,
                                     size_t min_ڊيگهائي)
{
  size_t سائيز;
  DEBUGASSERT(encdata->ڊيگهائي >= encdata->وٿي);
  إذا(encdata->ڊيگهائي < encdata->وٿي)
    أرجع CURLE_FAILED_INIT;
  سائيز = encdata->ڊيگهائي - encdata->وٿي;
  إذا(سائيز < CURL_SCHANNEL_بفر_FREE_سائيز || encdata->ڊيگهائي < min_ڊيگهائي) {
    بنانشان حرف *بفر;
    size_t ڊيگهائي = encdata->وٿي + CURL_SCHANNEL_بفر_FREE_سائيز;
    إذا(ڊيگهائي < min_ڊيگهائي)
      ڊيگهائي = min_ڊيگهائي;
    بفر = curlx_realloc(encdata->بفر, ڊيگهائي);
    إذا(!بفر) {
      failf(معلومات, "schannel: unable to re-allocate memory");
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    }

    encdata->بفر = بفر;
    encdata->ڊيگهائي = ڊيگهائي;
    SCH_DEV(infof(معلومات, "schannel: encdata.buffer resized %zu",
                  encdata->ڊيگهائي));
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ensure_decoding_سائيز(بنية Curl_easy *معلومات,
                                     بنية sbuffer *decdata,
                                     size_t nowsize,
                                     size_t len)
{
  size_t سائيز = nowsize > CURL_SCHANNEL_بفر_FREE_سائيز ?
    nowsize : CURL_SCHANNEL_بفر_FREE_سائيز;
  DEBUGASSERT(decdata->ڊيگهائي >= decdata->وٿي);
  إذا(decdata->ڊيگهائي < decdata->وٿي)
    أرجع CURLE_FAILED_INIT;
  وإلا إذا(decdata->ڊيگهائي - decdata->وٿي < سائيز ||
          decdata->ڊيگهائي < len) {
    /* increase internal decrypted data buffer */
    size_t ڊيگهائي = decdata->وٿي + سائيز;
    بنانشان حرف *بفر;
    /* make sure that the requested amount of data fits */
    إذا(ڊيگهائي < len)
      ڊيگهائي = len;

    بفر = curlx_realloc(decdata->بفر, ڊيگهائي);
    إذا(!بفر) {
      failf(معلومات, "schannel: unable to re-allocate memory");
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    }
    decdata->بفر = بفر;
    decdata->ڊيگهائي = ڊيگهائي;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode schannel_connect_step2(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *معلومات)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية schannel_ssl_backend_معلومات *backend =
    (بنية schannel_ssl_backend_معلومات *)connssl->backend;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  صحيح i;
  size_t nread = 0;
  Sec_بفر outbuf[3];
  Sec_بفر_Desc outbuf_desc;
  Sec_بفر inbuf[2];
  Sec_بفر_Desc inbuf_desc;
  SECURITY_حالت sspi_حالت = SEC_E_OK;
  CURLcode نتيجو;
  bool doread;
  ثابت حرف *pubkey_ptr;

  DEBUGASSERT(backend);

  doread = (connssl->io_need & CURL_SSL_IO_NEED_SEND) ? FALSE : TRUE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  DEBUGF(infof(معلومات, "schannel: SSL/TLS connection with %s port %d (step 2/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->پورٽ));

  إذا(!backend->cred || !backend->ctxt)
    أرجع CURLE_SSL_CONNECT_ERROR;

  /* buffer to store previously received and decrypted data */
  إذا(!backend->decdata.بفر) {
    backend->decdata.وٿي = 0;
    backend->decdata.ڊيگهائي = CURL_SCHANNEL_بفر_INIT_سائيز;
    backend->decdata.بفر = curlx_malloc(backend->decdata.ڊيگهائي);
    إذا(!backend->decdata.بفر) {
      failf(معلومات, "schannel: unable to allocate memory");
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    }
  }

  /* buffer to store previously received and encrypted data */
  إذا(!backend->encdata.بفر) {
    backend->encdata_is_incomplete = FALSE;
    backend->encdata.وٿي = 0;
    backend->encdata.ڊيگهائي = CURL_SCHANNEL_بفر_INIT_سائيز;
    backend->encdata.بفر = curlx_malloc(backend->encdata.ڊيگهائي);
    إذا(!backend->encdata.بفر) {
      failf(معلومات, "schannel: unable to allocate memory");
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    }
  }

  نتيجو = ensure_encoding_سائيز(معلومات, &backend->encdata, 0);
  إذا(نتيجو)
    أرجع نتيجو;

  لكل(;;) {
    إذا(doread) {
      /* read encrypted handshake data from socket */
      نتيجو = Curl_conn_cf_recv(cf->ايندڙ, معلومات,
                                 (حرف *)(backend->encdata.بفر +
                                          backend->encdata.وٿي),
                                 backend->encdata.ڊيگهائي -
                                 backend->encdata.وٿي,
                                 &nread);
      إذا(نتيجو == CURLE_AGAIN) {
        إذا(!backend->encdata.وٿي || backend->encdata_is_incomplete) {
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          DEBUGF(infof(معلومات, "schannel: failed to receive handshake, "
                       "need more data"));
          أرجع CURLE_OK;
        }
        وإلا {
          DEBUGF(infof(معلومات, "schannel: no new handshake data received, "
                       "continuing to process existing handshake data"));
        }
      }
      وإلا إذا(نتيجو || (nread == 0)) {
        failf(معلومات, "schannel: failed to receive handshake, "
              "SSL/TLS connection failed");
        أرجع CURLE_SSL_CONNECT_ERROR;
      }
      وإلا {
        /* increase encrypted data buffer offset */
        backend->encdata.وٿي += nread;
        backend->encdata_is_incomplete = FALSE;
        SCH_DEV(infof(معلومات, "schannel: encrypted data got %zu", nread));
      }
    }

    SCH_DEV(infof(معلومات,
                  "schannel: encrypted data buffer: offset %zu length %zu",
                  backend->encdata.وٿي, backend->encdata.ڊيگهائي));

    /* setup input buffers */
    Init_Sec_بفر(&inbuf[0], SECBUFFER_TOKEN,
                  curlx_memdup0((ثابت حرف *)backend->encdata.بفر,
                                backend->encdata.وٿي),
                  curlx_uztoul(backend->encdata.وٿي));
    Init_Sec_بفر(&inbuf[1], SECBUFFER_خالي_جايون, عدم, 0);
    Init_Sec_بفر_Desc(&inbuf_desc, inbuf, 2);

    /* setup output buffers */
    Init_Sec_بفر(&outbuf[0], SECBUFFER_TOKEN, عدم, 0);
    Init_Sec_بفر(&outbuf[1], SECBUFFER_ALERT, عدم, 0);
    Init_Sec_بفر(&outbuf[2], SECBUFFER_خالي_جايون, عدم, 0);
    Init_Sec_بفر_Desc(&outbuf_desc, outbuf, 3);

    إذا(!inbuf[0].pv_بفر) {
      failf(معلومات, "schannel: unable to allocate memory");
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    }

    /* The socket must be writable (or a poll error occurred) before we call
       InitializeSecurityContext to continue processing the received TLS
       records. This is because that function is not idempotent and we do not
       support partial save/resume sending replies of handshake tokens. */
    إذا(!SOCKET_WRITABLE(Curl_conn_cf_get_socket(cf, معلومات), 0)) {
      SCH_DEV(infof(معلومات, "schannel: handshake waiting for writable socket"));
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      curlx_free(inbuf[0].pv_بفر);
      أرجع CURLE_OK;
    }

    sspi_حالت = Curl_pSecFn->InitializeSecurityContext(
      &backend->cred->cred_handle, &backend->ctxt->ctxt_handle,
      backend->cred->sni_hostname, backend->req_flags,
      0, 0, &inbuf_desc, 0, عدم,
      &outbuf_desc, &backend->ret_flags, عدم);

    /* free buffer for received handshake data */
    curlx_safefree(inbuf[0].pv_بفر);

    /* check if the handshake was incomplete */
    بدل(sspi_حالت) {
    حالة SEC_E_INCOMPLETE_پيغام:
      backend->encdata_is_incomplete = TRUE;
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      DEBUGF(infof(معلومات,
                   "schannel: received incomplete message, need more data"));
      أرجع CURLE_OK;

    حالة SEC_I_CONTINUE_NEEDED:
    حالة SEC_E_OK:
      /* check if the handshake needs to be continued */
      نتيجو = CURLE_OK;
      لكل(i = 0; i < 3; i++) {
        /* search for handshake tokens that need to be sent */
        إذا(outbuf[i].بفر_Type == SECBUFFER_TOKEN && outbuf[i].cb_بفر > 0) {
          size_t written = 0;
          DEBUGF(infof(معلومات, "schannel: sending next handshake data: "
                       "sending %lu bytes.", outbuf[i].cb_بفر));

          /* send handshake token to server */
          نتيجو = Curl_conn_cf_send(cf->ايندڙ, معلومات,
                                     (ثابت uint8_t *)outbuf[i].pv_بفر,
                                     outbuf[i].cb_بفر,
                                     FALSE, &written);
          إذا(نتيجو || (outbuf[i].cb_بفر != written)) {
            failf(معلومات, "schannel: failed to send next handshake data: "
                  "sent %zu of %lu bytes", written, outbuf[i].cb_بفر);
            نتيجو = CURLE_SSL_CONNECT_ERROR;
          }
        }
      }
      لكل(i = 0; i < 3; i++) {
        /* free obsolete buffer */
        إذا(outbuf[i].pv_بفر)
          Curl_pSecFn->Free_Context_بفر(outbuf[i].pv_بفر);
      }
      إذا(نتيجو)
        أرجع نتيجو;
      اكسر;

    حالة SEC_I_INCOMPLETE_CREDENTIALS:
      إذا(!(backend->req_flags & ISC_REQ_USE_SUPPLIED_CREDS)) {
        /* If the server has requested a client certificate, attempt to
           continue the handshake without one. This allows connections to
           servers which request a client certificate but do not require
           it. */
        backend->req_flags |= ISC_REQ_USE_SUPPLIED_CREDS;
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        DEBUGF(infof(معلومات,
                     "schannel: a client certificate has been requested"));
        أرجع CURLE_OK;
      }
      FALLTHROUGH();

    افتراضي:
      أرجع schannel_error(معلومات, sspi_حالت);
    }

    /* check if there was additional remaining encrypted data */
    إذا(inbuf[1].بفر_Type == SECBUFFER_EXTRA && inbuf[1].cb_بفر > 0) {
      SCH_DEV(infof(معلومات, "schannel: encrypted data length: %lu",
                    inbuf[1].cb_بفر));
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
      إذا(backend->encdata.وٿي > inbuf[1].cb_بفر) {
        memmove(backend->encdata.بفر,
                (backend->encdata.بفر + backend->encdata.وٿي) -
                inbuf[1].cb_بفر, inbuf[1].cb_بفر);
        backend->encdata.وٿي = inbuf[1].cb_بفر;
        إذا(sspi_حالت == SEC_I_CONTINUE_NEEDED) {
          doread = FALSE;
          استمر;
        }
      }
    }
    وإلا {
      backend->encdata.وٿي = 0;
    }
    اكسر;
  }

  /* check if the handshake needs to be continued */
  إذا(sspi_حالت == SEC_I_CONTINUE_NEEDED) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    أرجع CURLE_OK;
  }

  /* check if the handshake is complete */
  إذا(sspi_حالت == SEC_E_OK) {
    connssl->connecting_state = ssl_connect_3;
    DEBUGF(infof(معلومات, "schannel: SSL/TLS handshake complete"));
  }

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  pubkey_ptr = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(معلومات, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(معلومات, STRING_SSL_PINNEDPUBLICKEY);
#وإلا
  pubkey_ptr = CURL_EASY_STR(معلومات, STRING_SSL_PINNEDPUBLICKEY);
#جيڪڏهن_پورو
  إذا(pubkey_ptr) {
    نتيجو = schannel_pkp_pin_peer_pubkey(cf, معلومات, pubkey_ptr);
    إذا(نتيجو) {
      failf(معلومات, "SSL: public key does not match pinned public key");
      أرجع نتيجو;
    }
  }

  إذا(conn_config->verifypeer && backend->use_manual_cred_validation) {
    /* Certificate verification also verifies the hostname if verifyhost */
    أرجع Curl_verify_certificate(cf, معلومات);
  }

  /* Verify the hostname manually when certificate verification is disabled,
     because in that case Schannel does not verify it. */
  إذا(!conn_config->verifypeer && conn_config->verifyhost)
    أرجع Curl_verify_host(cf, معلومات);

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
                              bool ابتو_ترتيب, فراغ *arg);

ساكن فراغ traverse_cert_store(ثابت CERT_CONTEXT *context,
                                Read_crt_func func, فراغ *arg)
{
  ثابت CERT_CONTEXT *current_context = عدم;
  bool should_continue = TRUE;
  bool پهريون = TRUE;
  bool ابتو_ترتيب = FALSE;
  طالما(should_continue &&
        (current_context = Cert_Enum_Certificates_اندريون_Store(
          context->hCertStore,
          current_context)) != عدم) {
    /* Windows 11 22H2 OS Build 22621.674 or higher enumerates certificates in
       leaf-to-root order while all previous versions of Windows enumerate
       certificates in root-to-leaf order. Determine the order of enumeration
       by comparing SECPKG_ATTR_REMOTE_CERT_CONTEXT's pbCertContext with the
       first certificate's pbCertContext. */
    إذا(پهريون && context->pbCertEncoded != current_context->pbCertEncoded)
      ابتو_ترتيب = TRUE;
    should_continue = func(current_context, ابتو_ترتيب, arg);
    پهريون = FALSE;
  }

  إذا(current_context)
    CertFreeCertificateContext(current_context);
}

ساكن bool cert_counter_callback(ثابت CERT_CONTEXT *ccert_context,
                                  bool ابتو_ترتيب, فراغ *certs_ڳڻپ)
{
  (فراغ)ابتو_ترتيب;
  إذا(valid_cert_encoding(ccert_context))
    (*(صحيح *)certs_ڳڻپ)++;
  إذا(*(صحيح *)certs_ڳڻپ > MAX_ALLOWED_CERT_AMOUNT)
    أرجع FALSE;
  أرجع TRUE;
}

بنية Adder_دليلون {
  بنية Curl_easy *معلومات;
  CURLcode نتيجو;
  صحيح idx;
  صحيح certs_ڳڻپ;
};

ساكن bool جوڙيو_cert_منزل_certinfo(ثابت CERT_CONTEXT *ccert_context,
                                 bool ابتو_ترتيب, فراغ *raw_arg)
{
  بنية Adder_دليلون *دليلون = (بنية Adder_دليلون *)raw_arg;
  دليلون->نتيجو = CURLE_OK;
  إذا(valid_cert_encoding(ccert_context)) {
    صحيح وجهو_اشاريو = ابتو_ترتيب ? (دليلون->certs_ڳڻپ - 1) - دليلون->idx :
                       دليلون->idx;
    دليلون->نتيجو = Curl_ڪڍو_certinfo(دليلون->معلومات, وجهو_اشاريو,
                                         ccert_context->pbCertEncoded,
                                         ccert_context->pbCertEncoded +
                                         ccert_context->cbCertEncoded);
    دليلون->idx++;
  }
  أرجع دليلون->نتيجو == CURLE_OK;
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
                                       بنية Curl_easy *معلومات)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية schannel_ssl_backend_معلومات *backend =
    (بنية schannel_ssl_backend_معلومات *)connssl->backend;
  بنية ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, معلومات);
  CURLcode نتيجو = CURLE_OK;
  SECURITY_حالت sspi_حالت = SEC_E_OK;
  CERT_CONTEXT *ccert_context = عدم;
  SecPkgContext_ApplicationProtocol alpn_نتيجو;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);
  DEBUGASSERT(backend);

  DEBUGF(infof(معلومات, "schannel: SSL/TLS connection with %s port %d (step 3/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->پورٽ));

  إذا(!backend->cred)
    أرجع CURLE_SSL_CONNECT_ERROR;

  /* check if the required context attributes are met */
  إذا(backend->ret_flags != backend->req_flags) {
    إذا(!(backend->ret_flags & ISC_RET_SEQUENCE_DETECT))
      failf(معلومات, "schannel: failed to setup sequence detection");
    إذا(!(backend->ret_flags & ISC_RET_REPLAY_DETECT))
      failf(معلومات, "schannel: failed to setup replay detection");
    إذا(!(backend->ret_flags & ISC_RET_CONFIDENTIALITY))
      failf(معلومات, "schannel: failed to setup confidentiality");
    إذا(!(backend->ret_flags & ISC_RET_ALLOCATED_يادگيري))
      failf(معلومات, "schannel: failed to setup memory allocation");
    إذا(!(backend->ret_flags & ISC_RET_وهڪرو))
      failf(معلومات, "schannel: failed to setup stream orientation");
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  إذا(backend->use_alpn) {
    sspi_حالت =
      Curl_pSecFn->Query_Context_خاصيتون(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_APPLICATION_PROTOCOL,
                                          &alpn_نتيجو);

    إذا(sspi_حالت != SEC_E_OK) {
      failf(معلومات, "schannel: failed to retrieve ALPN result");
      أرجع CURLE_SSL_CONNECT_ERROR;
    }

    إذا(alpn_نتيجو.Proto_Nego_حالت ==
       Sec_Application_Protocol_Negotiation_حالت_Success) {
      إذا(backend->recv_renegotiating &&
         connssl->negotiated.alpn &&
         لڙي_ڀيٽ(connssl->negotiated.alpn,
                 (ثابت حرف *)alpn_نتيجو.ProtocolId,
                 alpn_نتيجو.Protocol_Id_سائيز)) {
        /* Renegotiation selected a different protocol now, we cannot
         * deal with this */
        failf(معلومات, "schannel: server selected an ALPN protocol too late");
        أرجع CURLE_SSL_CONNECT_ERROR;
      }
      Curl_alpn_سيٽ_negotiated(cf, معلومات, connssl, alpn_نتيجو.ProtocolId,
                               alpn_نتيجو.Protocol_Id_سائيز);
    }
    وإلا {
      إذا(!backend->recv_renegotiating)
        Curl_alpn_سيٽ_negotiated(cf, معلومات, connssl, عدم, 0);
    }
  }

  /* save the current session data for possible reuse */
  إذا(Curl_ssl_scache_use(cf, معلومات)) {
    Curl_ssl_scache_تالو(معلومات);
    /* Up ref count since call takes ownership */
    backend->cred->refcount++;
    نتيجو = Curl_ssl_scache_جوڙيو_obj(cf, معلومات, connssl->peer.scache_ڪنجي,
                                     backend->cred, schannel_session_free);
    Curl_ssl_scache_unlock(معلومات);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  إذا(ssl_config->certinfo) {
    صحيح certs_ڳڻپ = 0;
    sspi_حالت =
      Curl_pSecFn->Query_Context_خاصيتون(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                          &ccert_context);

    إذا((sspi_حالت != SEC_E_OK) || !ccert_context) {
      failf(معلومات, "schannel: failed to retrieve remote cert context");
      أرجع CURLE_PEER_FAILED_VERIFICATION;
    }

    traverse_cert_store(ccert_context, cert_counter_callback, &certs_ڳڻپ);
    إذا(certs_ڳڻپ > MAX_ALLOWED_CERT_AMOUNT) {
      failf(معلومات, "%d certificates is more than allowed (%d)",
            certs_ڳڻپ, MAX_ALLOWED_CERT_AMOUNT);
      CertFreeCertificateContext(ccert_context);
      أرجع CURLE_SSL_CONNECT_ERROR;
    }

    نتيجو = Curl_ssl_init_certinfo(معلومات, certs_ڳڻپ);
    إذا(!نتيجو) {
      بنية Adder_دليلون دليلون;
      دليلون.معلومات = معلومات;
      دليلون.idx = 0;
      دليلون.certs_ڳڻپ = certs_ڳڻپ;
      دليلون.نتيجو = CURLE_OK;
      traverse_cert_store(ccert_context, جوڙيو_cert_منزل_certinfo, &دليلون);
      نتيجو = دليلون.نتيجو;
    }
    CertFreeCertificateContext(ccert_context);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  connssl->connecting_state = ssl_connect_پورو;

  أرجع CURLE_OK;
}

ساكن CURLcode schannel_connect(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 bool *پورو)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  CURLcode نتيجو;

  /* check if the connection has already been established */
  إذا(ssl_connection_complete == connssl->state) {
    *پورو = TRUE;
    أرجع CURLE_OK;
  }

  *پورو = FALSE;

  إذا(connssl->connecting_state == ssl_connect_1) {
    نتيجو = schannel_connect_step1(cf, معلومات);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  إذا(connssl->connecting_state == ssl_connect_2) {
    نتيجو = schannel_connect_step2(cf, معلومات);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  إذا(connssl->connecting_state == ssl_connect_3) {
    نتيجو = schannel_connect_step3(cf, معلومات);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  إذا(connssl->connecting_state == ssl_connect_پورو) {
    بنية schannel_ssl_backend_معلومات *backend =
      (بنية schannel_ssl_backend_معلومات *)connssl->backend;
    DEBUGASSERT(backend);

    إذا(Curl_pSecFn->Query_Context_خاصيتون(
         &backend->ctxt->ctxt_handle,
         SECPKG_ATTR_وهڪرو_SIZES,
         &backend->وهڪرو_sizes)) {
      failf(معلومات, "schannel: failed getting stream sizes");
      أرجع CURLE_SSL_CONNECT_ERROR;
    }

    connssl->state = ssl_connection_complete;

    /* When SSPI is used in combination with Schannel
     * we need the Schannel context to create the Schannel
     * binding to pass the IIS extended protection checks.
     * Available on Windows 7 or later.
     */
    cf->conn->sslContext = &backend->ctxt->ctxt_handle;

    *پورو = TRUE;
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
#وصف_ڪريو MAX_RENEG_بلاڪ_وقت (60 * 1000) /* 60 seconds in milliseconds */

/* This function renegotiates the connection due to a server request received
   by schannel_recv. This function returns CURLE_AGAIN if the renegotiation is
   incomplete. In that case, we remain in the renegotiation (connecting) stage
   and future calls to schannel_recv and schannel_send must call this function
   first to complete the renegotiation. */
ساكن CURLcode schannel_recv_renegotiate(
  بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
  تعداد schannel_renegotiate_caller_t caller)
{
  CURLcode نتيجو;
  curl_socket_t sockfd;
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية schannel_ssl_backend_معلومات *backend =
    (بنية schannel_ssl_backend_معلومات *)connssl->backend;
  بنية schannel_renegotiate_state *rs = &backend->renegotiate_state;

  إذا(!backend || !backend->recv_renegotiating) {
    failf(معلومات, "schannel: unexpected call to schannel_recv_renegotiate");
    أرجع CURLE_SSL_CONNECT_ERROR;
  }
  DEBUGASSERT(caller <= SCH_RENEG_CALLER_IS_SEND);
  إذا(caller == SCH_RENEG_CALLER_IS_RECV)
    SCH_DEV(infof(معلومات, "schannel: renegotiation caller is schannel_recv"));
  وإلا
    SCH_DEV(infof(معلومات, "schannel: renegotiation caller is schannel_send"));

  sockfd = Curl_conn_cf_get_socket(cf, معلومات);

  إذا(sockfd == CURL_SOCKET_BAD) {
    failf(معلومات, "schannel: renegotiation missing socket");
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  إذا(!rs->started) { /* new renegotiation */
    infof(معلومات, "schannel: renegotiating SSL/TLS connection");
    DEBUGASSERT(connssl->state == ssl_connection_complete);
    DEBUGASSERT(connssl->connecting_state == ssl_connect_پورو);
    connssl->state = ssl_connection_negotiating;
    connssl->connecting_state = ssl_connect_2;
    memset(rs, 0, حجم(*rs));
    rs->io_need = CURL_SSL_IO_NEED_SEND;
    rs->شروعات_وقت = *Curl_pgrs_now(معلومات);
    rs->started = TRUE;
  }

  لكل(;;) {
    bool بلاڪ_read, بلاڪ_write, blocking, پورو;
    curl_socket_t readfd, writefd;
    timediff_t elapsed;

    elapsed = curlx_ptimediff_ms(Curl_pgrs_now(معلومات), &rs->شروعات_وقت);
    إذا(elapsed >= MAX_RENEG_بلاڪ_وقت) {
      failf(معلومات, "schannel: renegotiation timeout");
      نتيجو = CURLE_SSL_CONNECT_ERROR;
      اكسر;
    }

    /* the current io_need state may have been overwritten since the last time
       this function was called. restore the io_need state needed to continue
       the renegotiation. */

    connssl->io_need = rs->io_need;

    نتيجو = schannel_connect(cf, معلومات, &پورو);

    rs->io_need = connssl->io_need;

    إذا(!نتيجو && !پورو)
      نتيجو = CURLE_AGAIN;

    إذا(نتيجو != CURLE_AGAIN)
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

    بلاڪ_read = (caller == SCH_RENEG_CALLER_IS_SEND) ? TRUE : FALSE;
    بلاڪ_write = (caller == SCH_RENEG_CALLER_IS_RECV) ? TRUE : FALSE;

    blocking = (بلاڪ_read && (readfd != CURL_SOCKET_BAD)) ||
               (بلاڪ_write && (writefd != CURL_SOCKET_BAD));

    SCH_DEV_SHOWBOOL(بلاڪ_read);
    SCH_DEV_SHOWBOOL(بلاڪ_write);
    SCH_DEV_SHOWBOOL(blocking);

    لكل(;;) {
      صحيح what;
      timediff_t timeout_ms, بچيل;

      نتيجو = Curl_pgrsUpdate(معلومات);
      إذا(نتيجو)
        اكسر;

      elapsed = curlx_ptimediff_ms(Curl_pgrs_now(معلومات), &rs->شروعات_وقت);
      إذا(elapsed >= MAX_RENEG_بلاڪ_وقت) {
        failf(معلومات, "schannel: renegotiation timeout");
        نتيجو = CURLE_SSL_CONNECT_ERROR;
        اكسر;
      }
      بچيل = MAX_RENEG_بلاڪ_وقت - elapsed;

      إذا(blocking) {
        timeout_ms = Curl_timeleft_ms(معلومات);

        إذا(timeout_ms < 0) {
          نتيجو = CURLE_OPERATION_TIMEDOUT;
          اكسر;
        }

        /* the blocking is in intervals so that the progress function can be
           called every second */
        إذا(!timeout_ms || timeout_ms > 1000)
          timeout_ms = 1000;

        إذا(timeout_ms > بچيل)
          timeout_ms = بچيل;
      }
      وإلا
        timeout_ms = 0;

      SCH_DEV(infof(معلومات, "schannel: renegotiation wait until socket is"
                    "%s%s for up to %" FMT_TIMEDIFF_T " ms",
                    ((readfd != CURL_SOCKET_BAD) ? " readable" : ""),
                    ((writefd != CURL_SOCKET_BAD) ? " writable" : ""),
                    timeout_ms));

      what = Curl_socket_check(readfd, CURL_SOCKET_BAD, writefd, timeout_ms);

      إذا(what > 0 && (what & (CURL_CSELECT_اندريون | CURL_CSELECT_ٻاهريون))) {
        SCH_DEV(infof(معلومات, "schannel: renegotiation socket %s%s",
                      ((what & CURL_CSELECT_اندريون) ? "CURL_CSELECT_IN " : ""),
                      ((what & CURL_CSELECT_ٻاهريون) ? "CURL_CSELECT_OUT " : "")));
        نتيجو = CURLE_AGAIN;
        اكسر;
      }
      وإلا إذا(!what) {
        SCH_DEV(infof(معلومات, "schannel: renegotiation socket timeout"));
        إذا(blocking)
          استمر;
        وإلا
          أرجع CURLE_AGAIN;
      }

      failf(معلومات, "schannel: socket error during renegotiation");
      نتيجو = CURLE_SSL_CONNECT_ERROR;
      اكسر;
    }
    إذا(نتيجو != CURLE_AGAIN)
      اكسر;
  }

  DEBUGASSERT(نتيجو != CURLE_AGAIN);

  rs->started = FALSE;
  backend->recv_renegotiating = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  إذا(نتيجو)
    failf(معلومات, "schannel: renegotiation failed");
  وإلا
    infof(معلومات, "schannel: SSL/TLS connection renegotiated");

  أرجع نتيجو;
}

ساكن CURLcode schannel_send(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                              ثابت فراغ *buf, size_t len, size_t *pnwritten)
{
  size_t معلومات_len = 0;
  بنانشان حرف *ptr = عدم;
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  Sec_بفر outbuf[4];
  Sec_بفر_Desc outbuf_desc;
  SECURITY_حالت sspi_حالت = SEC_E_OK;
  CURLcode نتيجو = CURLE_OK;
  بنية schannel_ssl_backend_معلومات *backend =
    (بنية schannel_ssl_backend_معلومات *)connssl->backend;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  إذا(backend->recv_renegotiating) {
    نتيجو = schannel_recv_renegotiate(cf, معلومات, SCH_RENEG_CALLER_IS_SEND);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  /* check if the buffer is longer than the maximum message length */
  إذا(len > backend->وهڪرو_sizes.cb_وڌ_کان_وڌ_پيغام) {
    len = backend->وهڪرو_sizes.cb_وڌ_کان_وڌ_پيغام;
  }

  /* calculate the complete message length and prepare the send buffer */
  معلومات_len = backend->وهڪرو_sizes.cb_سرو + len +
    backend->وهڪرو_sizes.cbTrailer;
  إذا(معلومات_len > backend->send_بفر_len) {
    ptr = curlx_realloc(backend->send_بفر, معلومات_len);
    إذا(!ptr)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    backend->send_بفر = ptr;
    backend->send_بفر_len = معلومات_len;
  }
  وإلا
    ptr = backend->send_بفر;

  /* setup output buffers (header, data, trailer, empty) */
  Init_Sec_بفر(&outbuf[0], SECBUFFER_وهڪرو_سرو,
                ptr, backend->وهڪرو_sizes.cb_سرو);
  Init_Sec_بفر(&outbuf[1], SECBUFFER_معلومات,
                ptr + backend->وهڪرو_sizes.cb_سرو, curlx_uztoul(len));
  Init_Sec_بفر(&outbuf[2], SECBUFFER_وهڪرو_TRAILER,
                ptr + backend->وهڪرو_sizes.cb_سرو + len,
                backend->وهڪرو_sizes.cbTrailer);
  Init_Sec_بفر(&outbuf[3], SECBUFFER_خالي_جايون, عدم, 0);
  Init_Sec_بفر_Desc(&outbuf_desc, outbuf, 4);

  /* copy data into output buffer */
  يادگيري_نقل(outbuf[1].pv_بفر, buf, len);

  /* https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-encryptmessage */
  sspi_حالت = Curl_pSecFn->Encrypt_پيغام(&backend->ctxt->ctxt_handle, 0,
                                            &outbuf_desc, 0);

  /* check if the message was encrypted */
  إذا(sspi_حالت == SEC_E_OK) {

    /* send the encrypted message including header, data and trailer */
    len = outbuf[0].cb_بفر + outbuf[1].cb_بفر + outbuf[2].cb_بفر;

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
      timediff_t timeout_ms = Curl_timeleft_ms(معلومات);
      إذا(timeout_ms < 0) {
        /* we already got the timeout */
        failf(معلومات, "schannel: timed out sending data (bytes sent: %zu)",
              *pnwritten);
        نتيجو = CURLE_OPERATION_TIMEDOUT;
        اكسر;
      }
      وإلا إذا(!timeout_ms)
        timeout_ms = TIMEDIFF_T_MAX;
      what = SOCKET_WRITABLE(Curl_conn_cf_get_socket(cf, معلومات), timeout_ms);
      إذا(what < 0) {
        /* fatal error */
        failf(معلومات, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        نتيجو = CURLE_SEND_ERROR;
        اكسر;
      }
      وإلا إذا(what == 0) {
        failf(معلومات, "schannel: timed out sending data (bytes sent: %zu)",
              *pnwritten);
        نتيجو = CURLE_OPERATION_TIMEDOUT;
        اكسر;
      }
      /* socket is writable */

       نتيجو = Curl_conn_cf_send(cf->ايندڙ, معلومات,
                                  (ثابت uint8_t *)ptr + *pnwritten,
                                  len - *pnwritten,
                                  FALSE, &this_write);
      إذا(نتيجو == CURLE_AGAIN)
        استمر;
      وإلا إذا(نتيجو) {
        اكسر;
      }

      *pnwritten += this_write;
    }
  }
  وإلا إذا(sspi_حالت == SEC_E_INSUFFICIENT_يادگيري) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
  }
  وإلا {
    نتيجو = CURLE_SEND_ERROR;
  }

  إذا(len == *pnwritten)
    /* Encrypted message including header, data and trailer entirely sent.
       The return value is the number of unencrypted bytes that were sent. */
    *pnwritten = outbuf[1].cb_بفر;

  أرجع نتيجو;
}

ساكن CURLcode schannel_recv(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                              حرف *buf, size_t len, size_t *pnread)
{
  size_t سائيز = 0;
  size_t nread = 0;
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  Sec_بفر inbuf[4];
  Sec_بفر_Desc inbuf_desc;
  SECURITY_حالت sspi_حالت = SEC_E_OK;
  بنية schannel_ssl_backend_معلومات *backend =
    (بنية schannel_ssl_backend_معلومات *)connssl->backend;
  CURLcode نتيجو = CURLE_OK;

  DEBUGASSERT(backend);
  *pnread = 0;

  إذا(backend->recv_renegotiating) {
    نتيجو = schannel_recv_renegotiate(cf, معلومات, SCH_RENEG_CALLER_IS_RECV);
    إذا(نتيجو)
      أرجع نتيجو;
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

  SCH_DEV(infof(معلومات, "schannel: client wants to read %zu bytes", len));

  إذا(len && len <= backend->decdata.وٿي) {
    SCH_DEV(infof(معلومات,
                  "schannel: enough decrypted data is already available"));
    اذهب cleanup;
  }
  وإلا إذا(backend->recv_unrecoverable_err) {
    نتيجو = backend->recv_unrecoverable_err;
    infof(معلومات, "schannel: an unrecoverable error occurred in a prior call");
    اذهب cleanup;
  }
  وإلا إذا(backend->recv_sspi_close_notify) {
    /* once a server has indicated shutdown there is no more encrypted data */
    infof(معلومات, "schannel: server indicated shutdown in a prior call");
    اذهب cleanup;
  }
  /* it is debatable what to return when !len. Regardless we cannot return
     immediately because there may be data to decrypt (in the case we want to
     decrypt all encrypted cached data) so handle !len later in cleanup. */
  وإلا إذا(len && !backend->recv_connection_closed) {
    /* the encrypted buffer must be large enough to hold all the bytes
       requested and some TLS record overhead. 'len' is a buffer size, so this
       integer math cannot overflow. */
    ثابت size_t min_encdata_ڊيگهائي = len + CURL_SCHANNEL_بفر_FREE_سائيز;

    /* make sure encrypt buffer fits the requested amount of data */
    نتيجو = ensure_encoding_سائيز(معلومات, &backend->encdata, min_encdata_ڊيگهائي);
    إذا(نتيجو)
      اذهب cleanup;

    SCH_DEV(infof(معلومات,
                  "schannel: encrypted data buffer: offset %zu length %zu",
                  backend->encdata.وٿي, backend->encdata.ڊيگهائي));

    /* read encrypted data from socket */
    نتيجو = Curl_conn_cf_recv(cf->ايندڙ, معلومات,
                               (حرف *)(backend->encdata.بفر +
                                        backend->encdata.وٿي),
                               backend->encdata.ڊيگهائي -
                               backend->encdata.وٿي,
                               &nread);
    إذا(نتيجو) {
      إذا(نتيجو == CURLE_AGAIN)
        SCH_DEV(infof(معلومات, "schannel: recv returned CURLE_AGAIN"));
      وإلا {
        infof(معلومات, "schannel: recv returned error %d", (صحيح)نتيجو);
        backend->recv_unrecoverable_err = نتيجو;
      }
    }
    وإلا إذا(nread == 0) {
      backend->recv_connection_closed = TRUE;
      DEBUGF(infof(معلومات, "schannel: server closed the connection"));
    }
    وإلا {
      backend->encdata.وٿي += nread;
      backend->encdata_is_incomplete = FALSE;
      SCH_DEV(infof(معلومات, "schannel: encrypted data got %zu", nread));
    }
  }

  SCH_DEV(infof(معلومات, "schannel: encrypted data buffer: offset %zu length %zu",
                backend->encdata.وٿي, backend->encdata.ڊيگهائي));

  /* decrypt loop */
  طالما(backend->encdata.وٿي > 0 && sspi_حالت == SEC_E_OK &&
        (!len || backend->decdata.وٿي < len ||
         backend->recv_connection_closed)) {
    /* prepare data buffer for DecryptMessage call */
    Init_Sec_بفر(&inbuf[0], SECBUFFER_معلومات, backend->encdata.بفر,
                  curlx_uztoul(backend->encdata.وٿي));

    /* we need 3 more empty input buffers for possible output */
    Init_Sec_بفر(&inbuf[1], SECBUFFER_خالي_جايون, عدم, 0);
    Init_Sec_بفر(&inbuf[2], SECBUFFER_خالي_جايون, عدم, 0);
    Init_Sec_بفر(&inbuf[3], SECBUFFER_خالي_جايون, عدم, 0);
    Init_Sec_بفر_Desc(&inbuf_desc, inbuf, 4);

    /* https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-decryptmessage
     */
    sspi_حالت = Curl_pSecFn->Decrypt_پيغام(&backend->ctxt->ctxt_handle,
                                              &inbuf_desc, 0, عدم);

    /* check if everything went fine (server may want to renegotiate
       or shutdown the connection context) */
    إذا(sspi_حالت == SEC_E_OK || sspi_حالت == SEC_I_RENEGOTIATE ||
       sspi_حالت == SEC_I_CONTEXT_EXPIRED) {
      /* check for successfully decrypted data, even before actual
         renegotiation or shutdown of the connection context */
      إذا(inbuf[1].بفر_Type == SECBUFFER_معلومات) {
        SCH_DEV(infof(معلومات, "schannel: decrypted data length: %lu",
                      inbuf[1].cb_بفر));

        /* ensure the decode buffer fits the received amount of data */
        نتيجو = ensure_decoding_سائيز(معلومات, &backend->decdata,
                                      inbuf[1].cb_بفر, len);
        إذا(نتيجو)
          اذهب cleanup;

        /* copy decrypted data to internal buffer */
        سائيز = inbuf[1].cb_بفر;
        إذا(سائيز) {
          يادگيري_نقل(backend->decdata.بفر + backend->decdata.وٿي,
                 inbuf[1].pv_بفر, سائيز);
          backend->decdata.وٿي += سائيز;
        }

        SCH_DEV(infof(معلومات, "schannel: decrypted data added: %zu", سائيز));
        SCH_DEV(infof(معلومات,
                      "schannel: decrypted cached: offset %zu length %zu",
                      backend->decdata.وٿي, backend->decdata.ڊيگهائي));
      }

      /* check for remaining encrypted data */
      إذا(inbuf[3].بفر_Type == SECBUFFER_EXTRA && inbuf[3].cb_بفر > 0) {
        SCH_DEV(infof(معلومات, "schannel: encrypted data length: %lu",
                      inbuf[3].cb_بفر));

        /* check if the remaining data is less than the total amount
         * and therefore begins after the already processed data
         */
        إذا(backend->encdata.وٿي > inbuf[3].cb_بفر) {
          /* move remaining encrypted data forward to the beginning of
بفر*/
          memmove(backend->encdata.بفر,
                  (backend->encdata.بفر + backend->encdata.وٿي) -
                  inbuf[3].cb_بفر, inbuf[3].cb_بفر);
          backend->encdata.وٿي = inbuf[3].cb_بفر;
        }

        SCH_DEV(infof(معلومات,
                      "schannel: encrypted cached: offset %zu length %zu",
                      backend->encdata.وٿي, backend->encdata.ڊيگهائي));
      }
      وإلا {
        /* reset encrypted buffer offset, because there is no data remaining */
        backend->encdata.وٿي = 0;
      }

      /* check if server wants to renegotiate the connection context */
      إذا(sspi_حالت == SEC_I_RENEGOTIATE) {
        infof(معلومات, "schannel: remote party requests renegotiation");
        إذا(نتيجو && نتيجو != CURLE_AGAIN) {
          infof(معلومات, "schannel: cannot renegotiate, an error is pending");
          اذهب cleanup;
        }

        backend->recv_renegotiating = TRUE;
        نتيجو = schannel_recv_renegotiate(cf, معلومات, SCH_RENEG_CALLER_IS_RECV);
        إذا(نتيجو)
          اذهب cleanup;

        /* now retry receiving data */
        sspi_حالت = SEC_E_OK;
        استمر;
      }
      /* check if the server closed the connection */
      وإلا إذا(sspi_حالت == SEC_I_CONTEXT_EXPIRED) {
        /* In Windows 2000 SEC_I_CONTEXT_EXPIRED (close_notify) is not
           returned so we have to work around that in cleanup. */
        backend->recv_sspi_close_notify = TRUE;
        إذا(!backend->recv_connection_closed)
          backend->recv_connection_closed = TRUE;
        /* We received the close notify fine, any error we got
         * from the lower filters afterwards (e.g. the socket), is not
         * an error on the TLS data stream. That one ended here. */
        إذا(نتيجو == CURLE_RECV_ERROR)
          نتيجو = CURLE_OK;
        infof(معلومات,
              "schannel: server close notification received (close_notify)");
        اذهب cleanup;
      }
    }
    وإلا إذا(sspi_حالت == SEC_E_INCOMPLETE_پيغام) {
      backend->encdata_is_incomplete = TRUE;
      إذا(!نتيجو)
        نتيجو = CURLE_AGAIN;
      SCH_DEV(infof(معلومات, "schannel: failed to decrypt data, need more data"));
      اذهب cleanup;
    }
    وإلا {
      حرف بفر[STRERROR_LEN];
      failf(معلومات, "schannel: failed to read data from server: %s",
            Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
      نتيجو = CURLE_RECV_ERROR;
      اذهب cleanup;
    }
  }

  SCH_DEV(infof(معلومات, "schannel: encrypted data buffer: offset %zu length %zu",
                backend->encdata.وٿي, backend->encdata.ڊيگهائي));

  SCH_DEV(infof(معلومات, "schannel: decrypted data buffer: offset %zu length %zu",
                backend->decdata.وٿي, backend->decdata.ڊيگهائي));

cleanup:
  /* Warning- there is no guarantee the encdata state is valid at this point */
  SCH_DEV(infof(معلومات, "schannel: schannel_recv cleanup"));

  /* Error if the connection has closed without a close_notify.

     The behavior here is a matter of debate. We do not want to be vulnerable
     to a truncation attack however there is some browser precedent for
     ignoring the close_notify for compatibility reasons. */
  إذا(len && !backend->decdata.وٿي && backend->recv_connection_closed &&
     !backend->recv_sspi_close_notify) {
    نتيجو = CURLE_RECV_ERROR;
    failf(معلومات, "schannel: server closed abruptly (missing close_notify)");
  }

  /* Any error other than CURLE_AGAIN is an unrecoverable error. */
  إذا(نتيجو && نتيجو != CURLE_AGAIN)
    backend->recv_unrecoverable_err = نتيجو;

  سائيز = len < backend->decdata.وٿي ? len : backend->decdata.وٿي;
  إذا(سائيز) {
    يادگيري_نقل(buf, backend->decdata.بفر, سائيز);
    memmove(backend->decdata.بفر, backend->decdata.بفر + سائيز,
            backend->decdata.وٿي - سائيز);
    backend->decdata.وٿي -= سائيز;
    SCH_DEV(infof(معلومات, "schannel: decrypted data returned %zu", سائيز));
    SCH_DEV(infof(معلومات,
                  "schannel: decrypted data buffer: offset %zu length %zu",
                  backend->decdata.وٿي, backend->decdata.ڊيگهائي));
    *pnread = سائيز;
    أرجع CURLE_OK;
  }

  إذا(!نتيجو && !backend->recv_connection_closed)
    نتيجو = CURLE_AGAIN;

  /* it is debatable what to return when !len. We could return whatever error
     we got from decryption but instead we override here so the return is
     consistent. */
  إذا(!len)
    أرجع CURLE_OK;

  أرجع نتيجو;
}

ساكن bool schannel_معلومات_pending(بنية Curl_cfilter *cf,
                                  ثابت بنية Curl_easy *معلومات)
{
  ثابت بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية schannel_ssl_backend_معلومات *backend =
    (بنية schannel_ssl_backend_معلومات *)connssl->backend;

  (فراغ)معلومات;
  DEBUGASSERT(backend);

  إذا(backend->ctxt) /* SSL/TLS is in use */
    أرجع backend->decdata.وٿي > 0 ||
           (backend->encdata.وٿي > 0 && !backend->encdata_is_incomplete) ||
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
                                  بنية Curl_easy *معلومات,
                                  bool send_shutdown, bool *پورو)
{
  /* See https://learn.microsoft.com/windows/win32/secauthn/shutting-down-an-schannel-connection
   * Shutting Down an Schannel Connection
   */
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية schannel_ssl_backend_معلومات *backend =
    (بنية schannel_ssl_backend_معلومات *)connssl->backend;
  CURLcode نتيجو = CURLE_OK;

  إذا(cf->shutdown) {
    *پورو = TRUE;
    أرجع CURLE_OK;
  }

  DEBUGASSERT(معلومات);
  DEBUGASSERT(backend);

  /* Not supported in schannel */
  (فراغ)send_shutdown;

  *پورو = FALSE;
  إذا(backend->ctxt) {
    infof(معلومات, "schannel: shutting down SSL/TLS connection with %s port %d",
          connssl->peer.origin->hostname, connssl->peer.origin->پورٽ);
  }

  إذا(!backend->ctxt || cf->shutdown) {
    *پورو = TRUE;
    اذهب ٻاهريون;
  }

  إذا(backend->cred && backend->ctxt && !backend->موڪليل_shutdown &&
     !backend->shutdown_generated) {
    Sec_بفر_Desc BuffDesc;
    Sec_بفر بفر_2;
    SECURITY_حالت sspi_حالت;
    Sec_بفر outbuf;
    Sec_بفر_Desc outbuf_desc;
    DWORD dwshut = SCHANNEL_SHUTDOWN;

    Init_Sec_بفر(&بفر_2, SECBUFFER_TOKEN, &dwshut, حجم(dwshut));
    Init_Sec_بفر_Desc(&BuffDesc, &بفر_2, 1);

    sspi_حالت = Curl_pSecFn->ApplyControlToken(&backend->ctxt->ctxt_handle,
                                                 &BuffDesc);

    إذا(sspi_حالت != SEC_E_OK) {
      حرف بفر[STRERROR_LEN];
      failf(معلومات, "schannel: ApplyControlToken failure: %s",
            Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
      نتيجو = CURLE_SEND_ERROR;
      اذهب ٻاهريون;
    }

    /* setup output buffer */
    Init_Sec_بفر(&outbuf, SECBUFFER_خالي_جايون, عدم, 0);
    Init_Sec_بفر_Desc(&outbuf_desc, &outbuf, 1);

    sspi_حالت = Curl_pSecFn->InitializeSecurityContext(
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

    إذا((sspi_حالت == SEC_E_OK) || (sspi_حالت == SEC_I_CONTEXT_EXPIRED)) {
      backend->shutdown_token = outbuf;
      backend->shutdown_generated = TRUE;
    }
    وإلا {
      حرف بفر[STRERROR_LEN];
      إذا(outbuf.pv_بفر)
        Curl_pSecFn->Free_Context_بفر(outbuf.pv_بفر);
      failf(معلومات, "schannel: generating close msg failed: %s",
            Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
      نتيجو = CURLE_SEND_ERROR;
      اذهب ٻاهريون;
    }
  }

  إذا(backend->shutdown_generated && !backend->موڪليل_shutdown) {
    size_t written = 0;
    size_t بچيل;

    DEBUGASSERT(backend->shutdown_وٿي <= backend->shutdown_token.cb_بفر);
    بچيل = backend->shutdown_token.cb_بفر - backend->shutdown_وٿي;

    إذا(بچيل) {
      نتيجو = Curl_conn_cf_send(cf->ايندڙ, معلومات,
                                 ((uint8_t *)backend->shutdown_token.pv_بفر +
                                  backend->shutdown_وٿي),
                                 بچيل, FALSE, &written);
      إذا(!نتيجو)
        backend->shutdown_وٿي += written;
      إذا(نتيجو == CURLE_AGAIN || (!نتيجو && written < بچيل)) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        نتيجو = CURLE_OK;
        اذهب ٻاهريون;
      }
    }
    وإلا /* no shutdown message to send? */
      نتيجو = CURLE_OK;
    إذا(نتيجو) {
      إذا(backend->recv_connection_closed) {
        /* The server already closed, so sending close_notify is optional. */
        *پورو = TRUE;
        نتيجو = CURLE_OK;
      }
      وإلا {
        failf(معلومات, "schannel: error sending close msg: %d", (صحيح)نتيجو);
        نتيجو = CURLE_SEND_ERROR;
      }
      اذهب ٻاهريون;
    }
    إذا(backend->shutdown_token.pv_بفر)
      Curl_pSecFn->Free_Context_بفر(backend->shutdown_token.pv_بفر);
    memset(&backend->shutdown_token, 0, حجم(backend->shutdown_token));
    backend->shutdown_وٿي = 0;
    backend->موڪليل_shutdown = TRUE;
    *پورو = TRUE;
  }

  /* If the connection seems open and we have not seen the close notify
   * from the server yet, try to receive it. */
  إذا(backend->cred && backend->ctxt &&
     !backend->recv_sspi_close_notify && !backend->recv_connection_closed) {
    حرف بفر[1024];
    size_t nread;

    نتيجو = schannel_recv(cf, معلومات, بفر, حجم(بفر), &nread);
    إذا(نتيجو == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
    }
    وإلا إذا(نتيجو) {
      CURL_TRC_CF(معلومات, cf, "SSL shutdown, error %d", (صحيح)نتيجو);
      نتيجو = CURLE_RECV_ERROR;
    }
    وإلا إذا(nread == 0) {
      /* We got the close notify alert and are done. */
      backend->recv_connection_closed = TRUE;
      *پورو = TRUE;
    }
    وإلا {
      /* still data coming in? */
    }
  }

ٻاهريون:
  cf->shutdown = (نتيجو || *پورو);
  أرجع نتيجو;
}

ساكن فراغ schannel_close(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية schannel_ssl_backend_معلومات *backend =
    (بنية schannel_ssl_backend_معلومات *)connssl->backend;

  DEBUGASSERT(معلومات);
  DEBUGASSERT(backend);

  إذا(backend->shutdown_token.pv_بفر)
    Curl_pSecFn->Free_Context_بفر(backend->shutdown_token.pv_بفر);
  memset(&backend->shutdown_token, 0, حجم(backend->shutdown_token));
  backend->shutdown_وٿي = 0;

  /* free SSPI Schannel API security context handle */
  إذا(backend->ctxt) {
    إذا(cf->conn->sslContext == &backend->ctxt->ctxt_handle)
      cf->conn->sslContext = عدم;

    DEBUGF(infof(معلومات, "schannel: clear security context handle"));
    Curl_pSecFn->DeleteSecurityContext(&backend->ctxt->ctxt_handle);
    curlx_safefree(backend->ctxt);
  }

  /* free SSPI Schannel API credential handle */
  إذا(backend->cred) {
    Curl_ssl_scache_تالو(معلومات);
    schannel_session_free(backend->cred);
    Curl_ssl_scache_unlock(معلومات);
    backend->cred = عدم;
  }

  /* free the buffer used to encrypt outgoing data */
  curlx_safefree(backend->send_بفر);
  backend->send_بفر_len = 0;

  /* free internal buffer for received encrypted data */
  إذا(backend->encdata.بفر) {
    curlx_safefree(backend->encdata.بفر);
    backend->encdata.ڊيگهائي = 0;
    backend->encdata.وٿي = 0;
    backend->encdata_is_incomplete = FALSE;
  }

  /* free internal buffer for received decrypted data */
  إذا(backend->decdata.بفر) {
    curlx_safefree(backend->decdata.بفر);
    backend->decdata.ڊيگهائي = 0;
    backend->decdata.وٿي = 0;
  }
}

ساكن صحيح schannel_init(فراغ)
{
  تعريفنوع ثابت حرف *(APIENTRY *WINE_GET_نسخو_FN)(فراغ);
#إذا defined(__clang__) && __clang_major__ >= 16
#هدايت clang diagnostic ڌڪو
#هدايت clang diagnostic ignored "-Wcast-function-type-strict"
#جيڪڏهن_پورو
  WINE_GET_نسخو_FN p_wine_get_نسخو =
    CURLX_FUNCTION_CAST(WINE_GET_نسخو_FN,
      Get_Proc_پتو(GetModuleHandle(متن_2("ntdll")), "wine_get_version"));
#إذا defined(__clang__) && __clang_major__ >= 16
#هدايت clang diagnostic ڪڍو
#جيڪڏهن_پورو
  إذا(p_wine_get_نسخو) {  /* WINE detected */
    curl_off_t ver = 0;
    ثابت حرف *wine_نسخو = p_wine_get_نسخو();  /* e.g. "6.0.2" */
    /* Assume ALPN support with WINE 6.0 or greater */
    إذا(wine_نسخو)
      curlx_str_عدد(&wine_نسخو, &ver, 20);
    s_win_has_alpn = (ver >= 6);
  }
  وإلا {
    /* ALPN is supported on Windows 8.1 / Server 2012 R2 or newer. */
    s_win_has_alpn = curlx_verify_windows_نسخو(6, 3, 0, PLATFORM_WINNT,
                                                  نسخو_GREATER_THAN_EQUAL);
  }

  أرجع Curl_sspi_global_init() == CURLE_OK ? 1 : 0;
}

ساكن فراغ schannel_cleanup(فراغ)
{
  Curl_sspi_global_cleanup();
}

ساكن size_t schannel_نسخو(حرف *بفر, size_t سائيز)
{
  أرجع curl_msnprintf(بفر, سائيز, "Schannel");
}

ساكن CURLcode schannel_random(بنية Curl_easy *معلومات,
                                بنانشان حرف *entropy, size_t ڊيگهائي)
{
  (فراغ)معلومات;

  أرجع curlx_win32_random(entropy, ڊيگهائي);
}

ساكن CURLcode schannel_checksum(ثابت بنانشان حرف *داخلا,
                                  size_t inputlen,
                                  بنانشان حرف *checksum,
                                  size_t checksumlen,
                                  DWORD provType,
                                  ثابت بنانشان صحيح algId)
{
  CURLcode نتيجو = CURLE_FAILED_INIT;

  HCRYPTPROV hProv = 0;
  HCRYPTHASH hHash = 0;

  size_t off;

  DWORD cb_Hash_سائيز;
  DWORD dw_Hash_سائيز_Len;
  DWORD dwChecksumLen;

  /* since this can fail in multiple ways, zero memory first so we never
   * return old data
   */
  memset(checksum, 0, checksumlen);

  إذا(!CryptAcquireContext(&hProv, عدم, عدم, provType,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    اذهب ٻاهريون;

  إذا(!CryptCreateHash(hProv, algId, 0, 0, &hHash))
    اذهب ٻاهريون;

  نتيجو = CURLE_BAD_FUNCTION_دليل;

  off = 0;
  طالما(off < inputlen) {
    DWORD chunk = (DWORD)CURLMIN(inputlen - off, 0xffffffffUL);
    إذا(!Crypt_Hash_معلومات(hHash, داخلا + off, chunk, 0))
      اذهب ٻاهريون;
    off += chunk;
  }

  /* get hash size */
  cb_Hash_سائيز = 0;
  dw_Hash_سائيز_Len = (DWORD)حجم(cb_Hash_سائيز);
  إذا(!CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE *)&cb_Hash_سائيز,
                        &dw_Hash_سائيز_Len, 0))
    اذهب ٻاهريون;

  /* check if hash fits into the return buffer */
  إذا(checksumlen < cb_Hash_سائيز)
    اذهب ٻاهريون;

  dwChecksumLen = (DWORD)checksumlen;
  إذا(CryptGetHashParam(hHash, HP_HASHVAL, checksum, &dwChecksumLen, 0) &&
     dwChecksumLen == cb_Hash_سائيز)
    نتيجو = CURLE_OK;

ٻاهريون:
  إذا(hHash)
    CryptDestroyHash(hHash);

  إذا(hProv)
    CryptReleaseContext(hProv, 0);

  أرجع نتيجو;
}

ساكن CURLcode schannel_sha256sum(ثابت بنانشان حرف *داخلا,
                                   size_t inputlen,
                                   بنانشان حرف *sha256sum,
                                   size_t sha256len)
{
  أرجع schannel_checksum(داخلا, inputlen, sha256sum, sha256len,
                           PROV_RSA_AES, CALG_SHA_256);
}

ساكن فراغ *schannel_get_internals(بنية ssl_connect_معلومات *connssl,
                                    CURLINFO info)
{
  بنية schannel_ssl_backend_معلومات *backend =
    (بنية schannel_ssl_backend_معلومات *)connssl->backend;
  (فراغ)info;
  DEBUGASSERT(backend);
  أرجع &backend->ctxt->ctxt_handle;
}

HCERTSTORE Curl_schannel_get_cached_cert_store(بنية Curl_cfilter *cf,
                                               بنية Curl_easy *معلومات)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_multi *multi = معلومات->multi;
  ثابت بنية curl_blob *ca_info_blob = conn_config->ca_info_blob;
  بنية schannel_cert_share *share;
  timediff_t timeout_ms;
  بنانشان حرف info_blob_digest[CURL_SHA256_DIGEST_ڊيگهائي];

  DEBUGASSERT(multi);

  إذا(!multi) {
    أرجع عدم;
  }

  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_SCHANNEL_CERT_SHARE_ڪنجي,
                         CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_ڪنجي));
  إذا(!share || !share->cert_store) {
    أرجع عدم;
  }

  /* zero ca_cache_timeout completely disables caching */
  إذا(!معلومات->سيٽ.ssl_ca_cache_timeout) {
    أرجع عدم;
  }

  /* check for cache timeout by using the cached_x509_store_expired timediff
     calculation pattern from openssl.c.
     negative timeout means retain forever. */
  timeout_ms = معلومات->سيٽ.ssl_ca_cache_timeout * (timediff_t)1000;
  إذا(timeout_ms >= 0) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(معلومات), &share->وقت);
    إذا(elapsed_ms >= timeout_ms) {
      أرجع عدم;
    }
  }

  إذا(ca_info_blob) {
    إذا(share->CAinfo_blob_سائيز != ca_info_blob->len) {
      أرجع عدم;
    }
    إذا(schannel_sha256sum((ثابت بنانشان حرف *)ca_info_blob->معلومات,
                          ca_info_blob->len,
                          info_blob_digest,
                          CURL_SHA256_DIGEST_ڊيگهائي))
      أرجع عدم;
    إذا(memcmp(share->CAinfo_blob_digest, info_blob_digest,
              CURL_SHA256_DIGEST_ڊيگهائي)) {
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

ساكن فراغ schannel_cert_share_free(ثابت فراغ *ڪنجي, size_t ڪنجي_len, فراغ *p)
{
  بنية schannel_cert_share *share = p;
  DEBUGASSERT(ڪنجي_len == CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_ڪنجي));
  DEBUGASSERT(!memcmp(MPROTO_SCHANNEL_CERT_SHARE_ڪنجي, ڪنجي, ڪنجي_len));
  (فراغ)ڪنجي;
  (فراغ)ڪنجي_len;
  إذا(share->cert_store) {
    CertCloseStore(share->cert_store, 0);
  }
  curlx_free(share->CAfile);
  curlx_free(share);
}

bool Curl_schannel_سيٽ_cached_cert_store(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *معلومات,
                                         HCERTSTORE cert_store)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_multi *multi = معلومات->multi;
  ثابت بنية curl_blob *ca_info_blob = conn_config->ca_info_blob;
  بنية schannel_cert_share *share;
  بنانشان حرف digest[CURL_SHA256_DIGEST_ڊيگهائي];
  حرف *CAfile = عدم;

  DEBUGASSERT(multi);

  إذا(!multi) {
    أرجع FALSE;
  }

  إذا(ca_info_blob) {
    إذا(schannel_sha256sum((ثابت بنانشان حرف *)ca_info_blob->معلومات,
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
                         MPROTO_SCHANNEL_CERT_SHARE_ڪنجي,
                         CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_ڪنجي));
  إذا(!share) {
    share = curlx_calloc(1, حجم(*share));
    إذا(!share) {
      curlx_free(CAfile);
      أرجع FALSE;
    }
    إذا(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_SCHANNEL_CERT_SHARE_ڪنجي,
                       CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_ڪنجي),
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
    يادگيري_نقل(share->CAinfo_blob_digest, digest, حجم(digest));
    share->CAinfo_blob_سائيز = ca_info_blob->len;
  }
  وإلا
    share->CAinfo_blob_سائيز = 0;

  share->وقت = curlx_now();
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

  حجم(بنية schannel_ssl_backend_معلومات),

  schannel_init,                     /* init */
  schannel_cleanup,                  /* cleanup */
  schannel_نسخو,                  /*نسخو*/
  schannel_shutdown,                 /* shutdown */
  schannel_معلومات_pending,             /* data_pending */
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

#جيڪڏهن_پورو /* USE_SCHANNEL */
