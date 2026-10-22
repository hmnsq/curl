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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ USE_SCHANNEL

#اگر_غیر_تعریف_شدہ USE_WINDOWS_SSPI
#غلطی "cannot compile Schannel support without SSPI."
#اگر_ختم

#شامل "vtls/schannel.h"
#شامل "vtls/schannel_int.h"
#شامل "vtls/vtls.h"
#شامل "vtls/vtls_int.h"
#شامل "vtls/vtls_scache.h"
#شامل "curl_trc.h"
#شامل "connect.h" /* for the connect timeout */
#شامل "curlx/strdup.h"
#شامل "strerror.h"
#شامل "select.h" /* for the socket readiness */
#شامل "curlx/win32-fopen.h"
#شامل "curlx/multibyte.h"
#شامل "vtls/x509asn1.h"
#شامل "curlx/version_win32.h"
#شامل "curlx/winapi.h"
#شامل "curlx/strparse.h"
#شامل "progress.h"
#شامل "curl_sha256.h"

/* Some verbose debug messages are wrapped by SCH_DEV() instead of DEBUGF()
 * and only shown if CURL_SCHANNEL_DEV_DEBUG was defined at build time. These
 * messages are extra verbose and intended for curl developers debugging
 * Schannel recv decryption and renegotiation.
 */
#اگر_تعریف_شدہ CURL_SCHANNEL_DEV_DEBUG
#تعریف_کرو SCH_DEV(x) x
#تعریف_کرو SCH_DEV_SHOWBOOL(x)                                     \
  کرو {                                                          \
    infof(ڈیٹا, "schannel: " #x " %s", (x) ? "TRUE" : "FALSE"); \
  } جب(0)
#ورنہ
#تعریف_کرو SCH_DEV(x) کرو {} جب(0)
#تعریف_کرو SCH_DEV_SHOWBOOL(x) کرو {} جب(0)
#اگر_ختم

/* Offered by mingw-w64 v8+, MS SDK 7.0A/VS2010+ */
#اگر_غیر_تعریف_شدہ SP_PROT_TLS1_0_CLIENT
#تعریف_کرو SP_PROT_TLS1_0_CLIENT           SP_PROT_TLS1_CLIENT
#اگر_ختم
#اگر_غیر_تعریف_شدہ SP_PROT_TLS1_1_CLIENT
#تعریف_کرو SP_PROT_TLS1_1_CLIENT           0x00000200
#اگر_ختم
#اگر_غیر_تعریف_شدہ SP_PROT_TLS1_2_CLIENT
#تعریف_کرو SP_PROT_TLS1_2_CLIENT           0x00000800
#اگر_ختم

/* Offered by mingw-w64 v8+, MS SDK 10.0.15063.0/VS2017 15.1+ */
#اگر_غیر_تعریف_شدہ SP_PROT_TLS1_3_CLIENT
#تعریف_کرو SP_PROT_TLS1_3_CLIENT           0x00002000
#اگر_ختم
/* Offered by mingw-w64 v8+, MS SDK 8.1/VS2013+ */
#اگر_غیر_تعریف_شدہ SCH_USE_STRONG_CRYPTO
#تعریف_کرو SCH_USE_STRONG_CRYPTO           0x00400000
#اگر_ختم

/* Offered by mingw-w64 v10+, MS SDK 7.0A/VS2010+ */
#اگر_غیر_تعریف_شدہ SECBUFFER_ALERT
#تعریف_کرو SECBUFFER_ALERT                 17
#اگر_ختم

/* Both schannel buffer sizes must be > 0 */
#تعریف_کرو CURL_SCHANNEL_بفر_INIT_سائز   4096
#تعریف_کرو CURL_SCHANNEL_بفر_FREE_سائز   1024

#تعریف_کرو CERT_THUMBPRINT_STR_LEN 40
#تعریف_کرو CERT_THUMBPRINT_ڈیٹا_LEN 20

/* Uncomment to force verbose output
 * #define infof(x, y, ...) curl_mprintf(y, __VA_ARGS__)
 * #define failf(x, y, ...) curl_mprintf(y, __VA_ARGS__)
 */

/* Offered by mingw-w64 v4+, MS SDK 6.0A/VS2008+ */
#اگر_غیر_تعریف_شدہ PKCS12_NO_PERSIST_کلید
#تعریف_کرو PKCS12_NO_PERSIST_کلید 0x00008000
#اگر_ختم

/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#اگر_غیر_تعریف_شدہ CERT_ڈھونڈو_HAS_نجی_کلید
#تعریف_کرو CERT_ڈھونڈو_HAS_نجی_کلید (21 << CERT_COMPARE_سرکاؤ)
#اگر_ختم

/* key to use at `multi->proto_hash` */
#تعریف_کرو MPROTO_SCHANNEL_CERT_SHARE_کلید   "tls:schannel:cert:share"

ساکن bool s_win_has_alpn;

/* Offered by mingw-w64 v9+, MS SDK 8.1/VS2013+ */
#اگر_غیر_تعریف_شدہ SECBUFFER_APPLICATION_PROTOCOLS
#تعریف_کرو SECBUFFER_APPLICATION_PROTOCOLS 18
#تعریف_کرو SECPKG_ATTR_APPLICATION_PROTOCOL 35

قسمتعریف شمار {
  SecApplicationProtocolNegotiationExt_None,
  SecApplicationProtocolNegotiationExt_NPN,
  SecApplicationProtocolNegotiationExt_ALPN
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT;

قسمتعریف شمار {
  Sec_Application_Protocol_Negotiation_حالت_None,
  Sec_Application_Protocol_Negotiation_حالت_Success,
  Sec_Application_Protocol_Negotiation_حالت_Selected_Client_Only
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_حالت;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
قسمتعریف ساخت {
  SEC_APPLICATION_PROTOCOL_NEGOTIATION_حالت Proto_Nego_حالت;
  SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT ProtoNegoExt;
  غیرعلامتی حرف Protocol_Id_سائز;
  غیرعلامتی حرف ProtocolId[0xff];
} SecPkgContext_ApplicationProtocol;
#اگر_ختم

ساکن خالی Init_Sec_بفر(Sec_بفر *بفر, غیرعلامتی طویل BufType,
                          خالی *Buf_ڈیٹا_Ptr, غیرعلامتی طویل Buf_Byte_سائز)
{
  بفر->cb_بفر = Buf_Byte_سائز;
  بفر->بفر_Type = BufType;
  بفر->pv_بفر = Buf_ڈیٹا_Ptr;
}

ساکن خالی Init_Sec_بفر_Desc(Sec_بفر_Desc *desc, Sec_بفر *BufArr,
                              غیرعلامتی طویل عدد_Arr_Elem)
{
  desc->ul_نسخہ = SECBUFFER_نسخہ;
  desc->pBuffers = BufArr;
  desc->cBuffers = عدد_Arr_Elem;
}

ساکن CURLcode schannel_سیٹ_ssl_نسخہ_min_max(DWORD *enabled_protocols,
                                                 ساخت Curl_cfilter *cf,
                                                 ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  طویل ssl_نسخہ = conn_config->نسخہ_2;
  طویل ssl_نسخہ_max = (طویل)conn_config->نسخہ_max;
  طویل i = ssl_نسخہ;

  انتخاب(ssl_نسخہ_max) {
  حالت CURL_SSLVERSION_MAX_NONE:
  حالت CURL_SSLVERSION_MAX_DEFAULT:

    /* Windows Server 2022 and newer (including Windows 11) support TLS 1.3
       built-in. Previous builds of Windows 10 had broken TLS 1.3
       implementations that could be enabled via registry. */
    اگر(curlx_verify_windows_نسخہ(10, 0, 20348, PLATFORM_WINNT,
                                    نسخہ_GREATER_THAN_EQUAL)) {
      ssl_نسخہ_max = CURL_SSLVERSION_MAX_TLSv1_3;
    }
    ورنہ /* Windows 10 or older */
      ssl_نسخہ_max = CURL_SSLVERSION_MAX_TLSv1_2;

    توڑو;
  }

  برائے(; i <= (ssl_نسخہ_max >> 16); ++i) {
    انتخاب(i) {
    حالت CURL_SSLVERSION_TLSv1_0:
      *enabled_protocols |= SP_PROT_TLS1_0_CLIENT;
      توڑو;
    حالت CURL_SSLVERSION_TLSv1_1:
      *enabled_protocols |= SP_PROT_TLS1_1_CLIENT;
      توڑو;
    حالت CURL_SSLVERSION_TLSv1_2:
      *enabled_protocols |= SP_PROT_TLS1_2_CLIENT;
      توڑو;
    حالت CURL_SSLVERSION_TLSv1_3:

      /* Windows Server 2022 or newer */
      اگر(curlx_verify_windows_نسخہ(10, 0, 20348, PLATFORM_WINNT,
                                      نسخہ_GREATER_THAN_EQUAL)) {
        *enabled_protocols |= SP_PROT_TLS1_3_CLIENT;
        توڑو;
      }
      ورنہ { /* Windows 10 or older */
        failf(ڈیٹا, "schannel: TLS 1.3 not supported on Windows 10 or older");
        واپس CURLE_SSL_CONNECT_ERROR;
      }
    }
  }
  واپس CURLE_OK;
}

#تعریف_کرو CIPHEROPTION(x) { #x, x }

ساخت algo {
  ثابت حرف *name;
  صحیح id;
};

ساکن ثابت ساخت algo algs[] = {
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
#اگر_تعریف_شدہ CALG_ECMQV
  CIPHEROPTION(CALG_ECMQV),
#اگر_ختم
  CIPHEROPTION(CALG_ECDSA),
/* Offered by mingw-w64 v7+, MS SDK 7.0A/VS2010+ */
#اگر_تعریف_شدہ CALG_ECDH_EPHEM
  CIPHEROPTION(CALG_ECDH_EPHEM),
#اگر_ختم
  { تہی, 0 },
};

ساکن صحیح get_alg_id_by_name(ثابت حرف *name)
{
  ثابت حرف *name_انتہا = strchr(name, ':');
  size_t n = name_انتہا ? (size_t)(name_انتہا - name) : لڑی_لمبائی(name);
  صحیح i;

  برائے(i = 0; algs[i].name; i++) {
    اگر((n == لڑی_لمبائی(algs[i].name) && !لڑی_موازنہ(algs[i].name, name, n)))
      واپس algs[i].id;
  }
  واپس 0; /* not found */
}

#تعریف_کرو عدد_CIPHERS 47 /* There are a maximum of 47 options listed above */

ساکن CURLcode سیٹ_ssl_ciphers(SCHANNEL_CRED *schannel_cred, حرف *ciphers,
                                ALG_ID *algIds)
{
  ثابت حرف *آغاز_Cur = ciphers;
  صحیح alg_گنتی = 0;
  جب(آغاز_Cur && *آغاز_Cur && (alg_گنتی < عدد_CIPHERS)) {
    curl_off_t alg;
    اگر(curlx_str_نمبر(&آغاز_Cur, &alg, زیادہ_سے_زیادہ_عدد) || !alg)
      alg = get_alg_id_by_name(آغاز_Cur);

    اگر(alg)
      algIds[alg_گنتی++] = (ALG_ID)alg;
    ورنہ اگر(!لڑی_موازنہ(آغاز_Cur, "USE_STRONG_CRYPTO",
                     CURL_CSTRLEN("USE_STRONG_CRYPTO")) ||
            !لڑی_موازنہ(آغاز_Cur, "SCH_USE_STRONG_CRYPTO",
                     CURL_CSTRLEN("SCH_USE_STRONG_CRYPTO")))
      schannel_cred->dwFlags |= SCH_USE_STRONG_CRYPTO;
    ورنہ
      واپس CURLE_SSL_CIPHER;
    آغاز_Cur = strchr(آغاز_Cur, ':');
    اگر(آغاز_Cur)
      آغاز_Cur++;
  }
  schannel_cred->palgSupportedAlgs = algIds;
  schannel_cred->cSupportedAlgs = (DWORD)alg_گنتی;
  واپس CURLE_OK;
}

/* Function allocates memory for store_path only if CURLE_OK is returned */
ساکن CURLcode get_cert_location(TCHAR *راستہ, DWORD *store_name,
                                  TCHAR **store_راستہ, TCHAR **thumbprint)
{
  TCHAR *sep;
  TCHAR *store_راستہ_آغاز;
  size_t store_name_len;

  sep = _tcschr(راستہ, متن('\\'));
  اگر(!sep)
    واپس CURLE_SSL_CERTPROBLEM;

  store_name_len = sep - راستہ;

  اگر(_tcsncmp(راستہ, متن("CurrentUser"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_USER;
  ورنہ اگر(_tcsncmp(راستہ, متن("LocalMachine"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE;
  ورنہ اگر(_tcsncmp(راستہ, متن("CurrentService"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_SERVICE;
  ورنہ اگر(_tcsncmp(راستہ, متن("Services"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_SERVICES;
  ورنہ اگر(_tcsncmp(راستہ, متن("Users"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_USERS;
  ورنہ اگر(_tcsncmp(راستہ, متن("CurrentUserGroupPolicy"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_CURRENT_USER_گروہ_POLICY;
  ورنہ اگر(_tcsncmp(راستہ, متن("LocalMachineGroupPolicy"), store_name_len) ==
          0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE_گروہ_POLICY;
  ورنہ اگر(_tcsncmp(راستہ, متن("LocalMachineEnterprise"), store_name_len) == 0)
    *store_name = CERT_SYSTEM_STORE_LOCAL_MACHINE_ENTERPRISE;
  ورنہ
    واپس CURLE_SSL_CERTPROBLEM;

  store_راستہ_آغاز = sep + 1;

  sep = _tcschr(store_راستہ_آغاز, متن('\\'));
  اگر(!sep)
    واپس CURLE_SSL_CERTPROBLEM;

  *thumbprint = sep + 1;
  اگر(_tcslen(*thumbprint) != CERT_THUMBPRINT_STR_LEN)
    واپس CURLE_SSL_CERTPROBLEM;

  *sep = متن('\0');
  *store_راستہ = curlx_tcsdup(store_راستہ_آغاز);
  *sep = متن('\\');
  اگر(!*store_راستہ)
    واپس CURLE_برآمد_کا_حافظہ;

  واپس CURLE_OK;
}

ساکن CURLcode get_client_cert(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا,
                                HCERTSTORE *برآمد_cert_store,
                                PCCERT_CONTEXT *برآمد_cert_context)
{
  ساخت ssl_filter_config *sslc = Curl_ssl_cf_get_filter_config(cf);
  PCCERT_CONTEXT client_cert = تہی;
  HCERTSTORE client_cert_store = تہی;
  CURLcode نتیجہ = CURLE_OK;

  اگر(sslc->clientcert || sslc->cert_blob) {
    DWORD cert_store_name = 0;
    TCHAR *cert_store_راستہ = تہی;
    TCHAR *cert_thumbprint_str = تہی;
    TCHAR cert_thumbprint_buf[CERT_THUMBPRINT_STR_LEN + 1];
    CRYPT_HASH_BLOB cert_thumbprint;
    BYTE cert_thumbprint_ڈیٹا[CERT_THUMBPRINT_ڈیٹا_LEN];
    HCERTSTORE cert_store = تہی;
    FILE *f_اندر_Cert = تہی;
    خالی *certdata = تہی;
    size_t certsize = 0;
    bool blob = !!sslc->cert_blob;

    اگر(blob) {
      certdata = sslc->cert_blob->ڈیٹا;
      certsize = sslc->cert_blob->len;
    }
    ورنہ {
      TCHAR *cert_راستہ =
        curlx_convert_UTF8_مقصد_tchar(sslc->clientcert);
      اگر(!cert_راستہ)
        واپس CURLE_برآمد_کا_حافظہ;

      نتیجہ = get_cert_location(cert_راستہ, &cert_store_name,
                                 &cert_store_راستہ, &cert_thumbprint_str);

      /* 'cert_thumbprint_str' points in to the allocated 'cert_path', copy
         the data. The string is verified to be CERT_THUMBPRINT_STR_LEN bytes
         long within the get_cert_location() function. */
      اگر(!نتیجہ && cert_thumbprint_str) {
        حافظہ_نقل(cert_thumbprint_buf, cert_thumbprint_str,
               حجم(cert_thumbprint_buf));
        cert_thumbprint_str = cert_thumbprint_buf;
      }

      curlx_free(cert_راستہ);
      اگر(نتیجہ && (sslc->clientcert[0] != '\0'))
        f_اندر_Cert = curlx_fopen(sslc->clientcert, "rb");

      اگر(نتیجہ && !f_اندر_Cert) {
        failf(ڈیٹا, "schannel: Failed to get certificate location"
              " or file for %s",
              sslc->clientcert);
        واپس نتیجہ;
      }
    }

    اگر((f_اندر_Cert || blob) && sslc->cert_type &&
       !curl_strequal(sslc->cert_type, "P12")) {
      failf(ڈیٹا, "schannel: certificate format compatibility error "
            "for %s",
            blob ? "(memory blob)" : sslc->clientcert);
      curlx_free(cert_store_راستہ);
      اگر(f_اندر_Cert)
        curlx_fclose(f_اندر_Cert);
      واپس CURLE_SSL_CERTPROBLEM;
    }

    اگر(f_اندر_Cert || blob) {
      /* Reading a .p12 or .pfx file, like the example at bottom of
         https://learn.microsoft.com/archive/msdn-technet-forums/3e7bc95f-b21a-4bcd-bd2c-7f996718cae5
       */
      CRYPT_ڈیٹا_BLOB datablob;
      WCHAR *pszPassword;
      size_t pwd_len = 0;
      صحیح cert_ڈھونڈو_flags;
      ثابت حرف *cert_showfilename_error = blob ?
        "(memory blob)" : sslc->clientcert;
      curlx_free(cert_store_راستہ);
      اگر(f_اندر_Cert) {
        طویل cert_tell = 0;
        bool continue_reading = fseek(f_اندر_Cert, 0, SEEK_END) == 0;
        اگر(continue_reading) {
          cert_tell = ftell(f_اندر_Cert);
          اگر(cert_tell < 0)
            continue_reading = FALSE;
          ورنہ
            certsize = (size_t)cert_tell;
        }
        اگر(continue_reading)
          continue_reading = fseek(f_اندر_Cert, 0, SEEK_SET) == 0;
        اگر(continue_reading && (certsize < CURL_MAX_داخلہ_لمبائی))
          certdata = curlx_malloc(certsize + 1);
        اگر(!certdata || ((صحیح)fread(certdata, certsize, 1, f_اندر_Cert) != 1))
          continue_reading = FALSE;
        curlx_fclose(f_اندر_Cert);
        اگر(!continue_reading) {
          failf(ڈیٹا, "schannel: Failed to read cert file %s",
                sslc->clientcert);
          curlx_free(certdata);
          واپس CURLE_SSL_CERTPROBLEM;
        }
      }

      /* Convert key-pair data to the in-memory certificate store */
      datablob.pb_ڈیٹا = (BYTE *)certdata;
      datablob.cb_ڈیٹا = (DWORD)certsize;

      اگر(sslc->کلید_passwd)
        pwd_len = لڑی_لمبائی(sslc->کلید_passwd);
      pszPassword = curlx_malloc(حجم(WCHAR) * (pwd_len + 1));
      اگر(pszPassword) {
        صحیح str_w_len = 0;
        اگر(pwd_len > 0)
          str_w_len = Multi_Byte_مقصد_Wide_Char(CP_UTF8, MB_ERR_INVALID_CHARS,
                                          sslc->کلید_passwd, (صحیح)pwd_len,
                                          pszPassword, (صحیح)(pwd_len + 1));

        اگر((str_w_len >= 0) && (str_w_len <= (صحیح)pwd_len))
          pszPassword[str_w_len] = 0;
        ورنہ
          pszPassword[0] = 0;

        cert_store = PFXImportCertStore(&datablob, pszPassword,
                                        PKCS12_NO_PERSIST_کلید);
        curlx_memzero(pszPassword, حجم(WCHAR) * (pwd_len + 1));
        curlx_free(pszPassword);
      }
      اگر(!blob)
        curlx_free(certdata);
      اگر(!cert_store) {
        DWORD errorcode = Get_آخری_Error();
        اگر(errorcode == ERROR_INVALID_PASSWORD)
          failf(ڈیٹا, "schannel: Failed to import cert file %s, "
                "password is bad",
                cert_showfilename_error);
        ورنہ
          failf(ڈیٹا, "schannel: Failed to import cert file %s, "
                "last error is 0x%08lx",
                cert_showfilename_error, errorcode);
        واپس CURLE_SSL_CERTPROBLEM;
      }

      /* CERT_FIND_HAS_PRIVATE_KEY is only available in Windows 8 / Server
         2012, (NT 6.2). For older versions we use CURL_FIND_ANY. */
      اگر(curlx_verify_windows_نسخہ(6, 2, 0, PLATFORM_WINNT,
                                      نسخہ_GREATER_THAN_EQUAL))
        cert_ڈھونڈو_flags = CERT_ڈھونڈو_HAS_نجی_کلید;
      ورنہ
        cert_ڈھونڈو_flags = CERT_ڈھونڈو_ANY;

      client_cert =
        Cert_ڈھونڈو_Certificate_اندر_Store(cert_store,
                                   X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                   cert_ڈھونڈو_flags, تہی, تہی);

      اگر(!client_cert) {
        failf(ڈیٹا, "schannel: Failed to get certificate from file %s"
              ", last error is 0x%08lx",
              cert_showfilename_error, Get_آخری_Error());
        CertCloseStore(cert_store, 0);
        واپس CURLE_SSL_CERTPROBLEM;
      }
    }
    ورنہ {
      cert_store =
        CertOpenStore(
#اگر_تعریف_شدہ UNICODE
                      CERT_STORE_PROV_SYSTEM_W,
#ورنہ
                      CERT_STORE_PROV_SYSTEM_A,
#اگر_ختم
                      0,
                      (HCRYPTPROV)تہی,
                      CERT_STORE_OPEN_EXISTING_FLAG | cert_store_name,
                      cert_store_راستہ);
      اگر(!cert_store) {
        حرف *راستہ_utf8 =
          curlx_convert_tchar_مقصد_UTF8(cert_store_راستہ);
        failf(ڈیٹا, "schannel: Failed to open cert store %lx %s, "
              "last error is 0x%08lx",
              cert_store_name, (راستہ_utf8 ? راستہ_utf8 : "(unknown)"),
              Get_آخری_Error());
        curlx_free(cert_store_راستہ);
        curlx_free(راستہ_utf8);
        واپس CURLE_SSL_CERTPROBLEM;
      }
      curlx_free(cert_store_راستہ);

      cert_thumbprint.pb_ڈیٹا = cert_thumbprint_ڈیٹا;
      cert_thumbprint.cb_ڈیٹا = CERT_THUMBPRINT_ڈیٹا_LEN;

      اگر(!Crypt_String_مقصد_ثنائی(cert_thumbprint_str,
                              CERT_THUMBPRINT_STR_LEN,
                              CRYPT_STRING_HEX,
                              cert_thumbprint_ڈیٹا,
                              &cert_thumbprint.cb_ڈیٹا,
                              تہی, تہی)) {
        CertCloseStore(cert_store, 0);
        واپس CURLE_SSL_CERTPROBLEM;
      }

      client_cert =
        Cert_ڈھونڈو_Certificate_اندر_Store(cert_store,
                                   X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0,
                                   CERT_ڈھونڈو_HASH, &cert_thumbprint, تہی);
      اگر(!client_cert) {
        /* CRYPT_E_NOT_FOUND / E_INVALIDARG */
        CertCloseStore(cert_store, 0);
        failf(ڈیٹا, "schannel: client cert not found in cert store");
        واپس CURLE_SSL_CERTPROBLEM;
      }
    }
    client_cert_store = cert_store;
  }

  *برآمد_cert_store = client_cert_store;
  *برآمد_cert_context = client_cert;

  واپس CURLE_OK;
}

ساکن CURLcode acquire_sspi_handle(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا,
                                    ساخت schannel_ssl_backend_ڈیٹا *backend,
                                    PCCERT_CONTEXT client_cert,
                                    DWORD flags,
                                    DWORD enabled_protocols)
{
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SECURITY_حالت sspi_حالت = SEC_E_OK;
  CURLcode نتیجہ;

  /* We support TLS 1.3 starting in Windows Server 2022 or later
   * (OS build 20348) */
  اگر(!conn_config->cipher_list &&
     curlx_verify_windows_نسخہ(10, 0, 20348, PLATFORM_WINNT,
                                  نسخہ_GREATER_THAN_EQUAL)) {

    SCH_CREDENTIALS credentials = { 0 };
    TLS_PARAMETERS tls_parameters = { 0 };
    CRYPTO_SETTINGS crypto_settings[1];
    PCCERT_CONTEXT client_certs[1];

    اگر(client_cert)
      client_certs[0] = client_cert;
    ورنہ
      client_certs[0] = تہی;

    memset(crypto_settings, 0, حجم(crypto_settings));

    tls_parameters.pDisabledCrypto = crypto_settings;

    /* The number of blocked suites */
    tls_parameters.cDisabledCrypto = (DWORD)0;
    credentials.pTlsParameters = &tls_parameters;
    credentials.cTlsParameters = 1;

    credentials.dw_نسخہ = SCH_CREDENTIALS_نسخہ;
    credentials.dwFlags = flags | SCH_USE_STRONG_CRYPTO;

    credentials.pTlsParameters->grbitDisabledProtocols = ~enabled_protocols;

    اگر(client_certs[0]) {
      credentials.cCreds = 1;
      credentials.paCred = client_certs;
    }

    sspi_حالت =
      Curl_pSecFn->AcquireCredentialsHandle(تہی, CURL_UNCONST(UNISP_NAME),
                                            SECPKG_CRED_OUTBOUND, تہی,
                                            &credentials, تہی, تہی,
                                            &backend->cred->cred_handle, تہی);
  }
  ورنہ {
    /* Pre-Windows 10 1809 or the user set a legacy algorithm list.
       Schannel does not negotiate TLS 1.3 when SCHANNEL_CRED is used. */
    ALG_ID algIds[عدد_CIPHERS];
    حرف *ciphers = conn_config->cipher_list;
    SCHANNEL_CRED schannel_cred = { 0 };
    PCCERT_CONTEXT client_certs[1];

    اگر(client_cert)
      client_certs[0] = client_cert;
    ورنہ
      client_certs[0] = تہی;

    schannel_cred.dw_نسخہ = SCHANNEL_CRED_نسخہ;
    schannel_cred.dwFlags = flags;
    schannel_cred.grbitEnabledProtocols = enabled_protocols;

    اگر(ciphers) {
      اگر((enabled_protocols & SP_PROT_TLS1_3_CLIENT)) {
        اگر(!(enabled_protocols & ~SP_PROT_TLS1_3_CLIENT)) {
          failf(ڈیٹا, "schannel: TLS 1.3 is not supported with a cipher list; "
                "remove the cipher list or allow a lower TLS version");
          واپس CURLE_SSL_CONNECT_ERROR;
        }
        infof(ڈیٹا, "schannel: WARNING: This version of Schannel "
              "negotiates a less-secure TLS version than TLS 1.3 because the "
              "user set an algorithm cipher list.");
      }
      نتیجہ = سیٹ_ssl_ciphers(&schannel_cred, ciphers, algIds);
      اگر(نتیجہ) {
        failf(ڈیٹا, "schannel: Failed setting algorithm cipher list");
        واپس نتیجہ;
      }
    }
    ورنہ {
      schannel_cred.dwFlags = flags | SCH_USE_STRONG_CRYPTO;
    }

    اگر(client_certs[0]) {
      schannel_cred.cCreds = 1;
      schannel_cred.paCred = client_certs;
    }

    sspi_حالت =
      Curl_pSecFn->AcquireCredentialsHandle(تہی, CURL_UNCONST(UNISP_NAME),
                                            SECPKG_CRED_OUTBOUND, تہی,
                                            &schannel_cred, تہی, تہی,
                                            &backend->cred->cred_handle, تہی);
  }

  اگر(sspi_حالت != SEC_E_OK) {
    حرف بفر[STRERROR_LEN];
    failf(ڈیٹا, "schannel: AcquireCredentialsHandle failed: %s",
          Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
    انتخاب(sspi_حالت) {
    حالت SEC_E_INSUFFICIENT_حافظہ:
      واپس CURLE_برآمد_کا_حافظہ;
    حالت SEC_E_NO_CREDENTIALS:
    حالت SEC_E_SECPKG_NOT_ملا:
    حالت SEC_E_NOT_OWNER:
    حالت SEC_E_UNKNOWN_CREDENTIALS:
    حالت SEC_E_اندرونی_ERROR:
    طےشدہ:
      واپس CURLE_SSL_CONNECT_ERROR;
    }
  }

  واپس CURLE_OK;
}

ساکن CURLcode schannel_acquire_credential_handle(ساخت Curl_cfilter *cf,
                                                   ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  PCCERT_CONTEXT client_cert = تہی;
  HCERTSTORE client_cert_store = تہی;
  CURLcode نتیجہ;

  /* setup Schannel API options */
  DWORD flags = 0;
  DWORD enabled_protocols = 0;

  ساخت schannel_ssl_backend_ڈیٹا *backend =
    (ساخت schannel_ssl_backend_ڈیٹا *)connssl->backend;

  DEBUGASSERT(backend);

  اگر(conn_config->verifypeer) {
    اگر(backend->use_manual_cred_validation)
      flags = SCH_CRED_MANUAL_CRED_VALIDATION;
    ورنہ
      flags = SCH_CRED_AUTO_CRED_VALIDATION;

    اگر(conn_config->no_revoke) {
      flags |= SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
               SCH_CRED_IGNORE_REVOCATION_OFFLINE;

      DEBUGF(infof(ڈیٹا, "schannel: disabled server certificate revocation "
                         "checks"));
    }
    ورنہ اگر(conn_config->revoke_بہترین_effort) {
      flags |= SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
               SCH_CRED_IGNORE_REVOCATION_OFFLINE |
               SCH_CRED_REVOCATION_CHECK_CHAIN;

      DEBUGF(infof(ڈیٹا, "schannel: ignore revocation offline errors"));
    }
    ورنہ {
      flags |= SCH_CRED_REVOCATION_CHECK_CHAIN;

      DEBUGF(infof(ڈیٹا, "schannel: checking server certificate revocation"));
    }
  }
  ورنہ {
    flags = SCH_CRED_MANUAL_CRED_VALIDATION |
            SCH_CRED_IGNORE_NO_REVOCATION_CHECK |
            SCH_CRED_IGNORE_REVOCATION_OFFLINE;
    DEBUGF(infof(ڈیٹا, "schannel: disabled server cert revocation checks"));
  }

  اگر(!conn_config->verifyhost) {
    flags |= SCH_CRED_NO_SERVERNAME_CHECK;
    DEBUGF(infof(ڈیٹا, "schannel: verifyhost setting prevents Schannel from "
                       "comparing the supplied target name with the subject "
                       "names in server certificates."));
  }

  اگر(!conn_config->auto_client_cert) {
    flags &= ~(DWORD)SCH_CRED_USE_DEFAULT_CREDS;
    flags |= SCH_CRED_NO_DEFAULT_CREDS;
    infof(ڈیٹا, "schannel: disabled automatic use of client certificate");
  }
  ورنہ
    infof(ڈیٹا, "schannel: enabled automatic use of client certificate");

  DEBUGASSERT(conn_config->نسخہ_2 != CURL_SSLVERSION_DEFAULT);
  انتخاب(conn_config->نسخہ_2) {
  حالت CURL_SSLVERSION_TLSv1:
  حالت CURL_SSLVERSION_TLSv1_0:
  حالت CURL_SSLVERSION_TLSv1_1:
  حالت CURL_SSLVERSION_TLSv1_2:
  حالت CURL_SSLVERSION_TLSv1_3: {
    نتیجہ = schannel_سیٹ_ssl_نسخہ_min_max(&enabled_protocols, cf, ڈیٹا);
    اگر(نتیجہ)
      واپس نتیجہ;
    توڑو;
  }
  حالت CURL_SSLVERSION_SSLv3:
  حالت CURL_SSLVERSION_SSLv2:
    failf(ڈیٹا, "SSL versions not supported");
    واپس CURLE_NOT_BUILT_اندر;
  طےشدہ:
    failf(ڈیٹا, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    واپس CURLE_SSL_CONNECT_ERROR;
  }

  نتیجہ = get_client_cert(cf, ڈیٹا, &client_cert_store, &client_cert);
  اگر(نتیجہ)
    واپس نتیجہ;

  /* allocate memory for the reusable credential handle */
  backend->cred = (ساخت Curl_schannel_cred *)
    curlx_calloc(1, حجم(ساخت Curl_schannel_cred));
  اگر(!backend->cred) {
    failf(ڈیٹا, "schannel: unable to allocate memory");

    اگر(client_cert)
      CertFreeCertificateContext(client_cert);
    اگر(client_cert_store)
      CertCloseStore(client_cert_store, 0);

    واپس CURLE_برآمد_کا_حافظہ;
  }
  backend->cred->refcount = 1;

  /* Since we did not persist the key, we need to extend the store's
   * lifetime until the end of the connection
   */
  backend->cred->client_cert_store = client_cert_store;

  نتیجہ = acquire_sspi_handle(cf, ڈیٹا, backend, client_cert,
                               flags, enabled_protocols);

  اگر(client_cert)
    CertFreeCertificateContext(client_cert);

  واپس نتیجہ;
}

ساکن CURLcode schannel_connect_step1(ساخت Curl_cfilter *cf,
                                       ساخت Curl_easy *ڈیٹا)
{
  size_t written = 0;
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت schannel_ssl_backend_ڈیٹا *backend =
    (ساخت schannel_ssl_backend_ڈیٹا *)connssl->backend;
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  Sec_بفر outbuf;
  Sec_بفر_Desc outbuf_desc;
  Sec_بفر inbuf;
  Sec_بفر_Desc inbuf_desc;
  غیرعلامتی حرف alpn_بفر[128];
  SECURITY_حالت sspi_حالت = SEC_E_OK;
  CURLcode نتیجہ;

  DEBUGASSERT(backend);
  DEBUGF(infof(ڈیٹا, "schannel: SSL/TLS connection with %s port %d (step 1/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->پورٹ));

  backend->use_alpn = connssl->alpn && s_win_has_alpn;

  اگر(conn_config->CAfile || conn_config->ca_info_blob) {
    اگر(curlx_verify_windows_نسخہ(6, 1, 0, PLATFORM_WINNT,
                                    نسخہ_GREATER_THAN_EQUAL)) {
      backend->use_manual_cred_validation = TRUE;
    }
    ورنہ {
      failf(ڈیٹا, "schannel: this version of Windows is too old to support "
                  "certificate verification via CA bundle file.");
      واپس CURLE_SSL_CACERT_BADFILE;
    }
  }
  ورنہ
    backend->use_manual_cred_validation = FALSE;

  backend->cred = تہی;

  /* check for an existing reusable credential handle */
  اگر(Curl_ssl_scache_use(cf, ڈیٹا)) {
    ساخت Curl_schannel_cred *old_cred;
    Curl_ssl_scache_تالا(ڈیٹا);
    old_cred = Curl_ssl_scache_get_obj(cf, ڈیٹا, connssl->peer.scache_کلید);
    اگر(old_cred) {
      backend->cred = old_cred;
      DEBUGF(infof(ڈیٹا, "schannel: reusing existing credential handle"));

      /* increment the reference counter of the credential/session handle */
      backend->cred->refcount++;
      DEBUGF(infof(ڈیٹا,
                   "schannel: incremented credential handle refcount = %d",
                   backend->cred->refcount));
    }
    Curl_ssl_scache_unlock(ڈیٹا);
  }

  اگر(!backend->cred) {
    حرف *snihost;
    نتیجہ = schannel_acquire_credential_handle(cf, ڈیٹا);
    اگر(نتیجہ || !backend->cred)
      واپس نتیجہ;
    /* schannel_acquire_credential_handle() sets backend->cred accordingly or
       it returns error otherwise. */

    /* A hostname associated with the credential is needed by
       InitializeSecurityContext for SNI and other reasons. */
    snihost = connssl->peer.sni ?
      connssl->peer.sni : connssl->peer.origin->hostname;
    backend->cred->sni_hostname = curlx_convert_UTF8_مقصد_tchar(snihost);
    اگر(!backend->cred->sni_hostname)
      واپس CURLE_برآمد_کا_حافظہ;
  }

  /* Warn if SNI is disabled due to use of an IP address */
  اگر(connssl->peer.type != CURL_SSL_PEER_DNS) {
    infof(ڈیٹا, "schannel: using IP address, SNI is not supported by OS.");
  }

  اگر(backend->use_alpn) {
    صحیح cur = 0;
    صحیح list_آغاز_اشاریہ = 0;
    غیرعلامتی صحیح *extension_len = تہی;
    غیرعلامتی مختصر *list_len = تہی;
    ثابت غیرعلامتی صحیح uiALPN = SecApplicationProtocolNegotiationExt_ALPN;
    ساخت alpn_proto_buf proto;

    /* The first four bytes is an unsigned int indicating number
       of bytes of data in the rest of the buffer. */
    extension_len = (غیرعلامتی صحیح *)(خالی *)&alpn_بفر[cur];
    cur += (صحیح)حجم(غیرعلامتی صحیح);

    /* The next four bytes are an indicator that this buffer contains
       ALPN data, as opposed to NPN, for example. */
    حافظہ_نقل(&alpn_بفر[cur], &uiALPN, حجم(uiALPN));
    cur += (صحیح)حجم(غیرعلامتی صحیح);

    /* The next two bytes is an unsigned short indicating the number
       of bytes used to list the preferred protocols. */
    list_len = (غیرعلامتی مختصر *)(خالی *)&alpn_بفر[cur];
    cur += (صحیح)حجم(غیرعلامتی مختصر);

    list_آغاز_اشاریہ = cur;

    نتیجہ = Curl_alpn_مقصد_proto_buf(&proto, connssl->alpn);
    اگر(نتیجہ) {
      failf(ڈیٹا, "Error setting ALPN");
      واپس CURLE_SSL_CONNECT_ERROR;
    }
    حافظہ_نقل(&alpn_بفر[cur], proto.ڈیٹا, proto.len);
    cur += proto.len;

    *list_len = curlx_uitous(cur - list_آغاز_اشاریہ);
    *extension_len = (غیرعلامتی صحیح)(*list_len +
      حجم(غیرعلامتی صحیح) + حجم(غیرعلامتی مختصر));

    Init_Sec_بفر(&inbuf, SECBUFFER_APPLICATION_PROTOCOLS, alpn_بفر, cur);
    Init_Sec_بفر_Desc(&inbuf_desc, &inbuf, 1);

    Curl_alpn_مقصد_proto_str(&proto, connssl->alpn);
    infof(ڈیٹا, VTLS_INFOF_ALPN_OFFER_1STR, proto.ڈیٹا);
  }
  ورنہ {
    Init_Sec_بفر(&inbuf, SECBUFFER_خالی_خانے, تہی, 0);
    Init_Sec_بفر_Desc(&inbuf_desc, &inbuf, 1);
  }

  /* setup output buffer */
  Init_Sec_بفر(&outbuf, SECBUFFER_خالی_خانے, تہی, 0);
  Init_Sec_بفر_Desc(&outbuf_desc, &outbuf, 1);

  /* security request flags */
  backend->req_flags =
    ISC_REQ_SEQUENCE_DETECT | ISC_REQ_REPLAY_DETECT |
    ISC_REQ_CONFIDENTIALITY | ISC_REQ_ALLOCATE_حافظہ |
    ISC_REQ_دھارا |
    (!conn_config->auto_client_cert ? ISC_REQ_USE_SUPPLIED_CREDS : 0);

  /* allocate memory for the security context handle */
  backend->ctxt = (ساخت Curl_schannel_ctxt *)
    curlx_calloc(1, حجم(ساخت Curl_schannel_ctxt));
  اگر(!backend->ctxt) {
    failf(ڈیٹا, "schannel: unable to allocate memory");
    واپس CURLE_برآمد_کا_حافظہ;
  }

  /* Schannel InitializeSecurityContext:
     https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-initializesecuritycontextw

     At the moment we do not pass inbuf unless we are using ALPN since we only
     use it for that, and WINE (for which we currently disable ALPN) is giving
     us problems with inbuf regardless. https://github.com/curl/curl/issues/983
   */
  sspi_حالت = Curl_pSecFn->InitializeSecurityContext(
    &backend->cred->cred_handle, تہی, backend->cred->sni_hostname,
    backend->req_flags, 0, 0,
    (backend->use_alpn ? &inbuf_desc : تہی),
    0, &backend->ctxt->ctxt_handle,
    &outbuf_desc, &backend->ret_flags, تہی);

  اگر(sspi_حالت != SEC_I_CONTINUE_NEEDED) {
    حرف بفر[STRERROR_LEN];
    curlx_safefree(backend->ctxt);
    انتخاب(sspi_حالت) {
    حالت SEC_E_INSUFFICIENT_حافظہ:
      failf(ڈیٹا, "schannel: initial InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
      واپس CURLE_برآمد_کا_حافظہ;
    حالت SEC_E_WRONG_PRINCIPAL:
      failf(ڈیٹا, "schannel: SNI or certificate check failed: %s",
            Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
      واپس CURLE_PEER_FAILED_VERIFICATION;
#اگر 0
    حالت SEC_E_INVALID_HANDLE:
    حالت SEC_E_INVALID_TOKEN:
    حالت SEC_E_LOGON_DENIED:
    حالت SEC_E_ہدف_UNKNOWN:
    حالت SEC_E_NO_AUTHENTICATING_AUTHORITY:
    حالت SEC_E_اندرونی_ERROR:
    حالت SEC_E_NO_CREDENTIALS:
    حالت SEC_E_UNSUPPORTED_FUNCTION:
    حالت SEC_E_APPLICATION_PROTOCOL_MISMATCH:
#اگر_ختم
    طےشدہ:
      failf(ڈیٹا, "schannel: initial InitializeSecurityContext failed: %s",
            Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
      واپس CURLE_SSL_CONNECT_ERROR;
    }
  }

  DEBUGF(infof(ڈیٹا, "schannel: sending initial handshake data: "
               "sending %lu bytes.", outbuf.cb_بفر));

  /* send initial handshake data which is now stored in output buffer */
  نتیجہ = Curl_conn_cf_send(cf->اگلا, ڈیٹا,
                             (ثابت uint8_t *)outbuf.pv_بفر,
                             outbuf.cb_بفر, FALSE,
                             &written);
  Curl_pSecFn->Free_Context_بفر(outbuf.pv_بفر);
  اگر(نتیجہ || (outbuf.cb_بفر != written)) {
    failf(ڈیٹا, "schannel: failed to send initial handshake data: "
          "sent %zu of %lu bytes", written, outbuf.cb_بفر);
    واپس CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGF(infof(ڈیٹا, "schannel: sent initial handshake data: "
               "sent %zu bytes", written));

  backend->recv_unrecoverable_err = CURLE_OK;
  backend->recv_sspi_close_notify = FALSE;
  backend->recv_connection_closed = FALSE;
  backend->recv_renegotiating = FALSE;
  backend->renegotiate_state.started = FALSE;
  backend->encdata_is_incomplete = FALSE;
  backend->shutdown_generated = FALSE;
  backend->shutdown_انحراف = 0;
  memset(&backend->shutdown_token, 0, حجم(backend->shutdown_token));

  /* continue to second handshake step */
  connssl->connecting_state = ssl_connect_2;

  واپس CURLE_OK;
}

ساکن CURLcode schannel_error(ساخت Curl_easy *ڈیٹا,
                               SECURITY_حالت sspi_حالت)
{
  حرف بفر[STRERROR_LEN];
  انتخاب(sspi_حالت) {
  حالت SEC_E_INSUFFICIENT_حافظہ:
    failf(ڈیٹا, "schannel: next InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
    واپس CURLE_برآمد_کا_حافظہ;
  حالت SEC_E_WRONG_PRINCIPAL:
    failf(ڈیٹا, "schannel: SNI or certificate check failed: %s",
          Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
    واپس CURLE_PEER_FAILED_VERIFICATION;
  حالت SEC_E_UNTRUSTED_جڑ:
    failf(ڈیٹا, "schannel: %s",
          Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
    واپس CURLE_PEER_FAILED_VERIFICATION;
#اگر 0
  حالت SEC_E_INVALID_HANDLE:
  حالت SEC_E_INVALID_TOKEN:
  حالت SEC_E_LOGON_DENIED:
  حالت SEC_E_ہدف_UNKNOWN:
  حالت SEC_E_NO_AUTHENTICATING_AUTHORITY:
  حالت SEC_E_اندرونی_ERROR:
  حالت SEC_E_NO_CREDENTIALS:
  حالت SEC_E_UNSUPPORTED_FUNCTION:
  حالت SEC_E_APPLICATION_PROTOCOL_MISMATCH:
#اگر_ختم
  طےشدہ:
    failf(ڈیٹا, "schannel: next InitializeSecurityContext failed: %s",
          Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
    واپس CURLE_SSL_CONNECT_ERROR;
  }
}

ساکن CURLcode schannel_pkp_pin_peer_pubkey(ساخت Curl_cfilter *cf,
                                             ساخت Curl_easy *ڈیٹا,
                                             ثابت حرف *pinnedpubkey)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت schannel_ssl_backend_ڈیٹا *backend =
    (ساخت schannel_ssl_backend_ڈیٹا *)connssl->backend;
  CERT_CONTEXT *pCertContextServer = تہی;

  /* Result is returned to caller */
  CURLcode نتیجہ = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  DEBUGASSERT(backend);

  /* if a path was not specified, do not pin */
  اگر(!pinnedpubkey)
    واپس CURLE_OK;

  کرو {
    SECURITY_حالت sspi_حالت;
    uint8_t *x509_der;
    DWORD x509_der_len;
    ساخت Curl_X509certificate x509_parsed;
    ساخت Curl_asn1Element *pubkey;

    sspi_حالت =
      Curl_pSecFn->Query_Context_اوصاف(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                          &pCertContextServer);

    اگر((sspi_حالت != SEC_E_OK) || !pCertContextServer) {
      حرف بفر[STRERROR_LEN];
      failf(ڈیٹا, "schannel: Failed to read remote certificate context: %s",
            Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
      توڑو; /* failed */
    }

    اگر(!(((pCertContextServer->dwCertEncodingType & X509_ASN_ENCODING) != 0) &&
         (pCertContextServer->cbCertEncoded > 0)))
      توڑو;

    x509_der = (uint8_t *)pCertContextServer->pbCertEncoded;
    x509_der_len = pCertContextServer->cbCertEncoded;
    memset(&x509_parsed, 0, حجم(x509_parsed));
    اگر(Curl_parseX509(&x509_parsed, x509_der, x509_der + x509_der_len))
      توڑو;

    pubkey = &x509_parsed.subject_عوامی_کلید_Info;
    اگر(!pubkey->سرخی || pubkey->انتہا <= pubkey->سرخی) {
      failf(ڈیٹا, "SSL: failed retrieving public key from server certificate");
      توڑو;
    }

    نتیجہ = Curl_pin_peer_pubkey(ڈیٹا,
                                  pinnedpubkey,
                                  (ثابت غیرعلامتی حرف *)pubkey->سرخی,
                                  (size_t)(pubkey->انتہا - pubkey->سرخی));
    اگر(نتیجہ) {
      failf(ڈیٹا, "SSL: public key does not match pinned public key");
    }
  } جب(0);

  اگر(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  واپس نتیجہ;
}

ساکن CURLcode ensure_encoding_سائز(ساخت Curl_easy *ڈیٹا,
                                     ساخت sbuffer *encdata,
                                     size_t min_لمبائی)
{
  size_t سائز;
  DEBUGASSERT(encdata->لمبائی >= encdata->انحراف);
  اگر(encdata->لمبائی < encdata->انحراف)
    واپس CURLE_FAILED_INIT;
  سائز = encdata->لمبائی - encdata->انحراف;
  اگر(سائز < CURL_SCHANNEL_بفر_FREE_سائز || encdata->لمبائی < min_لمبائی) {
    غیرعلامتی حرف *بفر;
    size_t لمبائی = encdata->انحراف + CURL_SCHANNEL_بفر_FREE_سائز;
    اگر(لمبائی < min_لمبائی)
      لمبائی = min_لمبائی;
    بفر = curlx_realloc(encdata->بفر, لمبائی);
    اگر(!بفر) {
      failf(ڈیٹا, "schannel: unable to re-allocate memory");
      واپس CURLE_برآمد_کا_حافظہ;
    }

    encdata->بفر = بفر;
    encdata->لمبائی = لمبائی;
    SCH_DEV(infof(ڈیٹا, "schannel: encdata.buffer resized %zu",
                  encdata->لمبائی));
  }
  واپس CURLE_OK;
}

ساکن CURLcode ensure_decoding_سائز(ساخت Curl_easy *ڈیٹا,
                                     ساخت sbuffer *decdata,
                                     size_t nowsize,
                                     size_t len)
{
  size_t سائز = nowsize > CURL_SCHANNEL_بفر_FREE_سائز ?
    nowsize : CURL_SCHANNEL_بفر_FREE_سائز;
  DEBUGASSERT(decdata->لمبائی >= decdata->انحراف);
  اگر(decdata->لمبائی < decdata->انحراف)
    واپس CURLE_FAILED_INIT;
  ورنہ اگر(decdata->لمبائی - decdata->انحراف < سائز ||
          decdata->لمبائی < len) {
    /* increase internal decrypted data buffer */
    size_t لمبائی = decdata->انحراف + سائز;
    غیرعلامتی حرف *بفر;
    /* make sure that the requested amount of data fits */
    اگر(لمبائی < len)
      لمبائی = len;

    بفر = curlx_realloc(decdata->بفر, لمبائی);
    اگر(!بفر) {
      failf(ڈیٹا, "schannel: unable to re-allocate memory");
      واپس CURLE_برآمد_کا_حافظہ;
    }
    decdata->بفر = بفر;
    decdata->لمبائی = لمبائی;
  }
  واپس CURLE_OK;
}

ساکن CURLcode schannel_connect_step2(ساخت Curl_cfilter *cf,
                                       ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت schannel_ssl_backend_ڈیٹا *backend =
    (ساخت schannel_ssl_backend_ڈیٹا *)connssl->backend;
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  صحیح i;
  size_t nread = 0;
  Sec_بفر outbuf[3];
  Sec_بفر_Desc outbuf_desc;
  Sec_بفر inbuf[2];
  Sec_بفر_Desc inbuf_desc;
  SECURITY_حالت sspi_حالت = SEC_E_OK;
  CURLcode نتیجہ;
  bool doread;
  ثابت حرف *pubkey_ptr;

  DEBUGASSERT(backend);

  doread = (connssl->io_need & CURL_SSL_IO_NEED_SEND) ? FALSE : TRUE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  DEBUGF(infof(ڈیٹا, "schannel: SSL/TLS connection with %s port %d (step 2/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->پورٹ));

  اگر(!backend->cred || !backend->ctxt)
    واپس CURLE_SSL_CONNECT_ERROR;

  /* buffer to store previously received and decrypted data */
  اگر(!backend->decdata.بفر) {
    backend->decdata.انحراف = 0;
    backend->decdata.لمبائی = CURL_SCHANNEL_بفر_INIT_سائز;
    backend->decdata.بفر = curlx_malloc(backend->decdata.لمبائی);
    اگر(!backend->decdata.بفر) {
      failf(ڈیٹا, "schannel: unable to allocate memory");
      واپس CURLE_برآمد_کا_حافظہ;
    }
  }

  /* buffer to store previously received and encrypted data */
  اگر(!backend->encdata.بفر) {
    backend->encdata_is_incomplete = FALSE;
    backend->encdata.انحراف = 0;
    backend->encdata.لمبائی = CURL_SCHANNEL_بفر_INIT_سائز;
    backend->encdata.بفر = curlx_malloc(backend->encdata.لمبائی);
    اگر(!backend->encdata.بفر) {
      failf(ڈیٹا, "schannel: unable to allocate memory");
      واپس CURLE_برآمد_کا_حافظہ;
    }
  }

  نتیجہ = ensure_encoding_سائز(ڈیٹا, &backend->encdata, 0);
  اگر(نتیجہ)
    واپس نتیجہ;

  برائے(;;) {
    اگر(doread) {
      /* read encrypted handshake data from socket */
      نتیجہ = Curl_conn_cf_recv(cf->اگلا, ڈیٹا,
                                 (حرف *)(backend->encdata.بفر +
                                          backend->encdata.انحراف),
                                 backend->encdata.لمبائی -
                                 backend->encdata.انحراف,
                                 &nread);
      اگر(نتیجہ == CURLE_AGAIN) {
        اگر(!backend->encdata.انحراف || backend->encdata_is_incomplete) {
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          DEBUGF(infof(ڈیٹا, "schannel: failed to receive handshake, "
                       "need more data"));
          واپس CURLE_OK;
        }
        ورنہ {
          DEBUGF(infof(ڈیٹا, "schannel: no new handshake data received, "
                       "continuing to process existing handshake data"));
        }
      }
      ورنہ اگر(نتیجہ || (nread == 0)) {
        failf(ڈیٹا, "schannel: failed to receive handshake, "
              "SSL/TLS connection failed");
        واپس CURLE_SSL_CONNECT_ERROR;
      }
      ورنہ {
        /* increase encrypted data buffer offset */
        backend->encdata.انحراف += nread;
        backend->encdata_is_incomplete = FALSE;
        SCH_DEV(infof(ڈیٹا, "schannel: encrypted data got %zu", nread));
      }
    }

    SCH_DEV(infof(ڈیٹا,
                  "schannel: encrypted data buffer: offset %zu length %zu",
                  backend->encdata.انحراف, backend->encdata.لمبائی));

    /* setup input buffers */
    Init_Sec_بفر(&inbuf[0], SECBUFFER_TOKEN,
                  curlx_memdup0((ثابت حرف *)backend->encdata.بفر,
                                backend->encdata.انحراف),
                  curlx_uztoul(backend->encdata.انحراف));
    Init_Sec_بفر(&inbuf[1], SECBUFFER_خالی_خانے, تہی, 0);
    Init_Sec_بفر_Desc(&inbuf_desc, inbuf, 2);

    /* setup output buffers */
    Init_Sec_بفر(&outbuf[0], SECBUFFER_TOKEN, تہی, 0);
    Init_Sec_بفر(&outbuf[1], SECBUFFER_ALERT, تہی, 0);
    Init_Sec_بفر(&outbuf[2], SECBUFFER_خالی_خانے, تہی, 0);
    Init_Sec_بفر_Desc(&outbuf_desc, outbuf, 3);

    اگر(!inbuf[0].pv_بفر) {
      failf(ڈیٹا, "schannel: unable to allocate memory");
      واپس CURLE_برآمد_کا_حافظہ;
    }

    /* The socket must be writable (or a poll error occurred) before we call
       InitializeSecurityContext to continue processing the received TLS
       records. This is because that function is not idempotent and we do not
       support partial save/resume sending replies of handshake tokens. */
    اگر(!SOCKET_WRITABLE(Curl_conn_cf_get_socket(cf, ڈیٹا), 0)) {
      SCH_DEV(infof(ڈیٹا, "schannel: handshake waiting for writable socket"));
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      curlx_free(inbuf[0].pv_بفر);
      واپس CURLE_OK;
    }

    sspi_حالت = Curl_pSecFn->InitializeSecurityContext(
      &backend->cred->cred_handle, &backend->ctxt->ctxt_handle,
      backend->cred->sni_hostname, backend->req_flags,
      0, 0, &inbuf_desc, 0, تہی,
      &outbuf_desc, &backend->ret_flags, تہی);

    /* free buffer for received handshake data */
    curlx_safefree(inbuf[0].pv_بفر);

    /* check if the handshake was incomplete */
    انتخاب(sspi_حالت) {
    حالت SEC_E_INCOMPLETE_پیغام:
      backend->encdata_is_incomplete = TRUE;
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      DEBUGF(infof(ڈیٹا,
                   "schannel: received incomplete message, need more data"));
      واپس CURLE_OK;

    حالت SEC_I_CONTINUE_NEEDED:
    حالت SEC_E_OK:
      /* check if the handshake needs to be continued */
      نتیجہ = CURLE_OK;
      برائے(i = 0; i < 3; i++) {
        /* search for handshake tokens that need to be sent */
        اگر(outbuf[i].بفر_Type == SECBUFFER_TOKEN && outbuf[i].cb_بفر > 0) {
          size_t written = 0;
          DEBUGF(infof(ڈیٹا, "schannel: sending next handshake data: "
                       "sending %lu bytes.", outbuf[i].cb_بفر));

          /* send handshake token to server */
          نتیجہ = Curl_conn_cf_send(cf->اگلا, ڈیٹا,
                                     (ثابت uint8_t *)outbuf[i].pv_بفر,
                                     outbuf[i].cb_بفر,
                                     FALSE, &written);
          اگر(نتیجہ || (outbuf[i].cb_بفر != written)) {
            failf(ڈیٹا, "schannel: failed to send next handshake data: "
                  "sent %zu of %lu bytes", written, outbuf[i].cb_بفر);
            نتیجہ = CURLE_SSL_CONNECT_ERROR;
          }
        }
      }
      برائے(i = 0; i < 3; i++) {
        /* free obsolete buffer */
        اگر(outbuf[i].pv_بفر)
          Curl_pSecFn->Free_Context_بفر(outbuf[i].pv_بفر);
      }
      اگر(نتیجہ)
        واپس نتیجہ;
      توڑو;

    حالت SEC_I_INCOMPLETE_CREDENTIALS:
      اگر(!(backend->req_flags & ISC_REQ_USE_SUPPLIED_CREDS)) {
        /* If the server has requested a client certificate, attempt to
           continue the handshake without one. This allows connections to
           servers which request a client certificate but do not require
           it. */
        backend->req_flags |= ISC_REQ_USE_SUPPLIED_CREDS;
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        DEBUGF(infof(ڈیٹا,
                     "schannel: a client certificate has been requested"));
        واپس CURLE_OK;
      }
      FALLTHROUGH();

    طےشدہ:
      واپس schannel_error(ڈیٹا, sspi_حالت);
    }

    /* check if there was additional remaining encrypted data */
    اگر(inbuf[1].بفر_Type == SECBUFFER_EXTRA && inbuf[1].cb_بفر > 0) {
      SCH_DEV(infof(ڈیٹا, "schannel: encrypted data length: %lu",
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
      اگر(backend->encdata.انحراف > inbuf[1].cb_بفر) {
        memmove(backend->encdata.بفر,
                (backend->encdata.بفر + backend->encdata.انحراف) -
                inbuf[1].cb_بفر, inbuf[1].cb_بفر);
        backend->encdata.انحراف = inbuf[1].cb_بفر;
        اگر(sspi_حالت == SEC_I_CONTINUE_NEEDED) {
          doread = FALSE;
          جاری;
        }
      }
    }
    ورنہ {
      backend->encdata.انحراف = 0;
    }
    توڑو;
  }

  /* check if the handshake needs to be continued */
  اگر(sspi_حالت == SEC_I_CONTINUE_NEEDED) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    واپس CURLE_OK;
  }

  /* check if the handshake is complete */
  اگر(sspi_حالت == SEC_E_OK) {
    connssl->connecting_state = ssl_connect_3;
    DEBUGF(infof(ڈیٹا, "schannel: SSL/TLS handshake complete"));
  }

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  pubkey_ptr = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(ڈیٹا, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(ڈیٹا, STRING_SSL_PINNEDPUBLICKEY);
#ورنہ
  pubkey_ptr = CURL_EASY_STR(ڈیٹا, STRING_SSL_PINNEDPUBLICKEY);
#اگر_ختم
  اگر(pubkey_ptr) {
    نتیجہ = schannel_pkp_pin_peer_pubkey(cf, ڈیٹا, pubkey_ptr);
    اگر(نتیجہ) {
      failf(ڈیٹا, "SSL: public key does not match pinned public key");
      واپس نتیجہ;
    }
  }

  اگر(conn_config->verifypeer && backend->use_manual_cred_validation) {
    /* Certificate verification also verifies the hostname if verifyhost */
    واپس Curl_verify_certificate(cf, ڈیٹا);
  }

  /* Verify the hostname manually when certificate verification is disabled,
     because in that case Schannel does not verify it. */
  اگر(!conn_config->verifypeer && conn_config->verifyhost)
    واپس Curl_verify_host(cf, ڈیٹا);

  واپس CURLE_OK;
}

ساکن bool valid_cert_encoding(ثابت CERT_CONTEXT *cert_context)
{
  واپس cert_context &&
    ((cert_context->dwCertEncodingType & X509_ASN_ENCODING) != 0) &&
    cert_context->pbCertEncoded &&
    (cert_context->cbCertEncoded > 0);
}

قسمتعریف bool (*Read_crt_func)(ثابت CERT_CONTEXT *ccert_context,
                              bool الٹ_ترتیب, خالی *arg);

ساکن خالی traverse_cert_store(ثابت CERT_CONTEXT *context,
                                Read_crt_func func, خالی *arg)
{
  ثابت CERT_CONTEXT *current_context = تہی;
  bool should_continue = TRUE;
  bool پہلا = TRUE;
  bool الٹ_ترتیب = FALSE;
  جب(should_continue &&
        (current_context = Cert_Enum_Certificates_اندر_Store(
          context->hCertStore,
          current_context)) != تہی) {
    /* Windows 11 22H2 OS Build 22621.674 or higher enumerates certificates in
       leaf-to-root order while all previous versions of Windows enumerate
       certificates in root-to-leaf order. Determine the order of enumeration
       by comparing SECPKG_ATTR_REMOTE_CERT_CONTEXT's pbCertContext with the
       first certificate's pbCertContext. */
    اگر(پہلا && context->pbCertEncoded != current_context->pbCertEncoded)
      الٹ_ترتیب = TRUE;
    should_continue = func(current_context, الٹ_ترتیب, arg);
    پہلا = FALSE;
  }

  اگر(current_context)
    CertFreeCertificateContext(current_context);
}

ساکن bool cert_شمار_callback(ثابت CERT_CONTEXT *ccert_context,
                                  bool الٹ_ترتیب, خالی *certs_گنتی)
{
  (خالی)الٹ_ترتیب;
  اگر(valid_cert_encoding(ccert_context))
    (*(صحیح *)certs_گنتی)++;
  اگر(*(صحیح *)certs_گنتی > MAX_ALLOWED_CERT_AMOUNT)
    واپس FALSE;
  واپس TRUE;
}

ساخت Adder_دلائل {
  ساخت Curl_easy *ڈیٹا;
  CURLcode نتیجہ;
  صحیح idx;
  صحیح certs_گنتی;
};

ساکن bool جوڑو_cert_مقصد_certinfo(ثابت CERT_CONTEXT *ccert_context,
                                 bool الٹ_ترتیب, خالی *raw_arg)
{
  ساخت Adder_دلائل *دلائل = (ساخت Adder_دلائل *)raw_arg;
  دلائل->نتیجہ = CURLE_OK;
  اگر(valid_cert_encoding(ccert_context)) {
    صحیح داخل_کرو_اشاریہ = الٹ_ترتیب ? (دلائل->certs_گنتی - 1) - دلائل->idx :
                       دلائل->idx;
    دلائل->نتیجہ = Curl_نکالو_certinfo(دلائل->ڈیٹا, داخل_کرو_اشاریہ,
                                         ccert_context->pbCertEncoded,
                                         ccert_context->pbCertEncoded +
                                         ccert_context->cbCertEncoded);
    دلائل->idx++;
  }
  واپس دلائل->نتیجہ == CURLE_OK;
}

ساکن خالی schannel_session_free(خالی *sessionid)
{
  /* this is expected to be called under sessionid lock */
  ساخت Curl_schannel_cred *cred = sessionid;

  اگر(cred) {
    cred->refcount--;
    اگر(cred->refcount == 0) {
      Curl_pSecFn->FreeCredentialsHandle(&cred->cred_handle);
      curlx_free(cred->sni_hostname);
      اگر(cred->client_cert_store) {
        CertCloseStore(cred->client_cert_store, 0);
        cred->client_cert_store = تہی;
      }
      curlx_safefree(cred);
    }
  }
}

ساکن CURLcode schannel_connect_step3(ساخت Curl_cfilter *cf,
                                       ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت schannel_ssl_backend_ڈیٹا *backend =
    (ساخت schannel_ssl_backend_ڈیٹا *)connssl->backend;
  ساخت ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, ڈیٹا);
  CURLcode نتیجہ = CURLE_OK;
  SECURITY_حالت sspi_حالت = SEC_E_OK;
  CERT_CONTEXT *ccert_context = تہی;
  SecPkgContext_ApplicationProtocol alpn_نتیجہ;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);
  DEBUGASSERT(backend);

  DEBUGF(infof(ڈیٹا, "schannel: SSL/TLS connection with %s port %d (step 3/3)",
               connssl->peer.origin->hostname, connssl->peer.origin->پورٹ));

  اگر(!backend->cred)
    واپس CURLE_SSL_CONNECT_ERROR;

  /* check if the required context attributes are met */
  اگر(backend->ret_flags != backend->req_flags) {
    اگر(!(backend->ret_flags & ISC_RET_SEQUENCE_DETECT))
      failf(ڈیٹا, "schannel: failed to setup sequence detection");
    اگر(!(backend->ret_flags & ISC_RET_REPLAY_DETECT))
      failf(ڈیٹا, "schannel: failed to setup replay detection");
    اگر(!(backend->ret_flags & ISC_RET_CONFIDENTIALITY))
      failf(ڈیٹا, "schannel: failed to setup confidentiality");
    اگر(!(backend->ret_flags & ISC_RET_ALLOCATED_حافظہ))
      failf(ڈیٹا, "schannel: failed to setup memory allocation");
    اگر(!(backend->ret_flags & ISC_RET_دھارا))
      failf(ڈیٹا, "schannel: failed to setup stream orientation");
    واپس CURLE_SSL_CONNECT_ERROR;
  }

  اگر(backend->use_alpn) {
    sspi_حالت =
      Curl_pSecFn->Query_Context_اوصاف(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_APPLICATION_PROTOCOL,
                                          &alpn_نتیجہ);

    اگر(sspi_حالت != SEC_E_OK) {
      failf(ڈیٹا, "schannel: failed to retrieve ALPN result");
      واپس CURLE_SSL_CONNECT_ERROR;
    }

    اگر(alpn_نتیجہ.Proto_Nego_حالت ==
       Sec_Application_Protocol_Negotiation_حالت_Success) {
      اگر(backend->recv_renegotiating &&
         connssl->negotiated.alpn &&
         لڑی_موازنہ(connssl->negotiated.alpn,
                 (ثابت حرف *)alpn_نتیجہ.ProtocolId,
                 alpn_نتیجہ.Protocol_Id_سائز)) {
        /* Renegotiation selected a different protocol now, we cannot
         * deal with this */
        failf(ڈیٹا, "schannel: server selected an ALPN protocol too late");
        واپس CURLE_SSL_CONNECT_ERROR;
      }
      Curl_alpn_سیٹ_negotiated(cf, ڈیٹا, connssl, alpn_نتیجہ.ProtocolId,
                               alpn_نتیجہ.Protocol_Id_سائز);
    }
    ورنہ {
      اگر(!backend->recv_renegotiating)
        Curl_alpn_سیٹ_negotiated(cf, ڈیٹا, connssl, تہی, 0);
    }
  }

  /* save the current session data for possible reuse */
  اگر(Curl_ssl_scache_use(cf, ڈیٹا)) {
    Curl_ssl_scache_تالا(ڈیٹا);
    /* Up ref count since call takes ownership */
    backend->cred->refcount++;
    نتیجہ = Curl_ssl_scache_جوڑو_obj(cf, ڈیٹا, connssl->peer.scache_کلید,
                                     backend->cred, schannel_session_free);
    Curl_ssl_scache_unlock(ڈیٹا);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  اگر(ssl_config->certinfo) {
    صحیح certs_گنتی = 0;
    sspi_حالت =
      Curl_pSecFn->Query_Context_اوصاف(&backend->ctxt->ctxt_handle,
                                          SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                          &ccert_context);

    اگر((sspi_حالت != SEC_E_OK) || !ccert_context) {
      failf(ڈیٹا, "schannel: failed to retrieve remote cert context");
      واپس CURLE_PEER_FAILED_VERIFICATION;
    }

    traverse_cert_store(ccert_context, cert_شمار_callback, &certs_گنتی);
    اگر(certs_گنتی > MAX_ALLOWED_CERT_AMOUNT) {
      failf(ڈیٹا, "%d certificates is more than allowed (%d)",
            certs_گنتی, MAX_ALLOWED_CERT_AMOUNT);
      CertFreeCertificateContext(ccert_context);
      واپس CURLE_SSL_CONNECT_ERROR;
    }

    نتیجہ = Curl_ssl_init_certinfo(ڈیٹا, certs_گنتی);
    اگر(!نتیجہ) {
      ساخت Adder_دلائل دلائل;
      دلائل.ڈیٹا = ڈیٹا;
      دلائل.idx = 0;
      دلائل.certs_گنتی = certs_گنتی;
      دلائل.نتیجہ = CURLE_OK;
      traverse_cert_store(ccert_context, جوڑو_cert_مقصد_certinfo, &دلائل);
      نتیجہ = دلائل.نتیجہ;
    }
    CertFreeCertificateContext(ccert_context);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  connssl->connecting_state = ssl_connect_مکمل;

  واپس CURLE_OK;
}

ساکن CURLcode schannel_connect(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 bool *مکمل)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  CURLcode نتیجہ;

  /* check if the connection has already been established */
  اگر(ssl_connection_complete == connssl->state) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  *مکمل = FALSE;

  اگر(connssl->connecting_state == ssl_connect_1) {
    نتیجہ = schannel_connect_step1(cf, ڈیٹا);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  اگر(connssl->connecting_state == ssl_connect_2) {
    نتیجہ = schannel_connect_step2(cf, ڈیٹا);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  اگر(connssl->connecting_state == ssl_connect_3) {
    نتیجہ = schannel_connect_step3(cf, ڈیٹا);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  اگر(connssl->connecting_state == ssl_connect_مکمل) {
    ساخت schannel_ssl_backend_ڈیٹا *backend =
      (ساخت schannel_ssl_backend_ڈیٹا *)connssl->backend;
    DEBUGASSERT(backend);

    اگر(Curl_pSecFn->Query_Context_اوصاف(
         &backend->ctxt->ctxt_handle,
         SECPKG_ATTR_دھارا_SIZES,
         &backend->دھارا_sizes)) {
      failf(ڈیٹا, "schannel: failed getting stream sizes");
      واپس CURLE_SSL_CONNECT_ERROR;
    }

    connssl->state = ssl_connection_complete;

    /* When SSPI is used in combination with Schannel
     * we need the Schannel context to create the Schannel
     * binding to pass the IIS extended protection checks.
     * Available on Windows 7 or later.
     */
    cf->conn->sslContext = &backend->ctxt->ctxt_handle;

    *مکمل = TRUE;
  }

  واپس CURLE_OK;
}

شمار schannel_renegotiate_caller_t {
  SCH_RENEG_CALLER_IS_RECV,
  SCH_RENEG_CALLER_IS_SEND
};

/* The maximum time we allow for Schannel renegotiation which may in some
   rare cases block either due to libcurl (waiting on the socket) or Windows
   (waiting on an interactive security prompt). Note Schannel "renegotiation"
   is not necessarily literal TLS renegotiation, but means DecryptMessage
   returned SEC_I_RENEGOTIATE which means at least the security context needs
   to be re-established. */
#تعریف_کرو MAX_RENEG_بلاک_وقت (60 * 1000) /* 60 seconds in milliseconds */

/* This function renegotiates the connection due to a server request received
   by schannel_recv. This function returns CURLE_AGAIN if the renegotiation is
   incomplete. In that case, we remain in the renegotiation (connecting) stage
   and future calls to schannel_recv and schannel_send must call this function
   first to complete the renegotiation. */
ساکن CURLcode schannel_recv_renegotiate(
  ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
  شمار schannel_renegotiate_caller_t caller)
{
  CURLcode نتیجہ;
  curl_socket_t sockfd;
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت schannel_ssl_backend_ڈیٹا *backend =
    (ساخت schannel_ssl_backend_ڈیٹا *)connssl->backend;
  ساخت schannel_renegotiate_state *rs = &backend->renegotiate_state;

  اگر(!backend || !backend->recv_renegotiating) {
    failf(ڈیٹا, "schannel: unexpected call to schannel_recv_renegotiate");
    واپس CURLE_SSL_CONNECT_ERROR;
  }
  DEBUGASSERT(caller <= SCH_RENEG_CALLER_IS_SEND);
  اگر(caller == SCH_RENEG_CALLER_IS_RECV)
    SCH_DEV(infof(ڈیٹا, "schannel: renegotiation caller is schannel_recv"));
  ورنہ
    SCH_DEV(infof(ڈیٹا, "schannel: renegotiation caller is schannel_send"));

  sockfd = Curl_conn_cf_get_socket(cf, ڈیٹا);

  اگر(sockfd == CURL_SOCKET_BAD) {
    failf(ڈیٹا, "schannel: renegotiation missing socket");
    واپس CURLE_SSL_CONNECT_ERROR;
  }

  اگر(!rs->started) { /* new renegotiation */
    infof(ڈیٹا, "schannel: renegotiating SSL/TLS connection");
    DEBUGASSERT(connssl->state == ssl_connection_complete);
    DEBUGASSERT(connssl->connecting_state == ssl_connect_مکمل);
    connssl->state = ssl_connection_negotiating;
    connssl->connecting_state = ssl_connect_2;
    memset(rs, 0, حجم(*rs));
    rs->io_need = CURL_SSL_IO_NEED_SEND;
    rs->آغاز_وقت = *Curl_pgrs_now(ڈیٹا);
    rs->started = TRUE;
  }

  برائے(;;) {
    bool بلاک_read, بلاک_write, blocking, مکمل;
    curl_socket_t readfd, writefd;
    timediff_t elapsed;

    elapsed = curlx_ptimediff_ms(Curl_pgrs_now(ڈیٹا), &rs->آغاز_وقت);
    اگر(elapsed >= MAX_RENEG_بلاک_وقت) {
      failf(ڈیٹا, "schannel: renegotiation timeout");
      نتیجہ = CURLE_SSL_CONNECT_ERROR;
      توڑو;
    }

    /* the current io_need state may have been overwritten since the last time
       this function was called. restore the io_need state needed to continue
       the renegotiation. */

    connssl->io_need = rs->io_need;

    نتیجہ = schannel_connect(cf, ڈیٹا, &مکمل);

    rs->io_need = connssl->io_need;

    اگر(!نتیجہ && !مکمل)
      نتیجہ = CURLE_AGAIN;

    اگر(نتیجہ != CURLE_AGAIN)
      توڑو;

    readfd = (rs->io_need & CURL_SSL_IO_NEED_RECV) ? sockfd : CURL_SOCKET_BAD;
    writefd = (rs->io_need & CURL_SSL_IO_NEED_SEND) ? sockfd : CURL_SOCKET_BAD;

    اگر(readfd == CURL_SOCKET_BAD && writefd == CURL_SOCKET_BAD)
      جاری;

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

    بلاک_read = (caller == SCH_RENEG_CALLER_IS_SEND) ? TRUE : FALSE;
    بلاک_write = (caller == SCH_RENEG_CALLER_IS_RECV) ? TRUE : FALSE;

    blocking = (بلاک_read && (readfd != CURL_SOCKET_BAD)) ||
               (بلاک_write && (writefd != CURL_SOCKET_BAD));

    SCH_DEV_SHOWBOOL(بلاک_read);
    SCH_DEV_SHOWBOOL(بلاک_write);
    SCH_DEV_SHOWBOOL(blocking);

    برائے(;;) {
      صحیح what;
      timediff_t timeout_ms, بقایا;

      نتیجہ = Curl_pgrsUpdate(ڈیٹا);
      اگر(نتیجہ)
        توڑو;

      elapsed = curlx_ptimediff_ms(Curl_pgrs_now(ڈیٹا), &rs->آغاز_وقت);
      اگر(elapsed >= MAX_RENEG_بلاک_وقت) {
        failf(ڈیٹا, "schannel: renegotiation timeout");
        نتیجہ = CURLE_SSL_CONNECT_ERROR;
        توڑو;
      }
      بقایا = MAX_RENEG_بلاک_وقت - elapsed;

      اگر(blocking) {
        timeout_ms = Curl_timeleft_ms(ڈیٹا);

        اگر(timeout_ms < 0) {
          نتیجہ = CURLE_OPERATION_TIMEDOUT;
          توڑو;
        }

        /* the blocking is in intervals so that the progress function can be
           called every second */
        اگر(!timeout_ms || timeout_ms > 1000)
          timeout_ms = 1000;

        اگر(timeout_ms > بقایا)
          timeout_ms = بقایا;
      }
      ورنہ
        timeout_ms = 0;

      SCH_DEV(infof(ڈیٹا, "schannel: renegotiation wait until socket is"
                    "%s%s for up to %" FMT_TIMEDIFF_T " ms",
                    ((readfd != CURL_SOCKET_BAD) ? " readable" : ""),
                    ((writefd != CURL_SOCKET_BAD) ? " writable" : ""),
                    timeout_ms));

      what = Curl_socket_check(readfd, CURL_SOCKET_BAD, writefd, timeout_ms);

      اگر(what > 0 && (what & (CURL_CSELECT_اندر | CURL_CSELECT_برآمد))) {
        SCH_DEV(infof(ڈیٹا, "schannel: renegotiation socket %s%s",
                      ((what & CURL_CSELECT_اندر) ? "CURL_CSELECT_IN " : ""),
                      ((what & CURL_CSELECT_برآمد) ? "CURL_CSELECT_OUT " : "")));
        نتیجہ = CURLE_AGAIN;
        توڑو;
      }
      ورنہ اگر(!what) {
        SCH_DEV(infof(ڈیٹا, "schannel: renegotiation socket timeout"));
        اگر(blocking)
          جاری;
        ورنہ
          واپس CURLE_AGAIN;
      }

      failf(ڈیٹا, "schannel: socket error during renegotiation");
      نتیجہ = CURLE_SSL_CONNECT_ERROR;
      توڑو;
    }
    اگر(نتیجہ != CURLE_AGAIN)
      توڑو;
  }

  DEBUGASSERT(نتیجہ != CURLE_AGAIN);

  rs->started = FALSE;
  backend->recv_renegotiating = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  اگر(نتیجہ)
    failf(ڈیٹا, "schannel: renegotiation failed");
  ورنہ
    infof(ڈیٹا, "schannel: SSL/TLS connection renegotiated");

  واپس نتیجہ;
}

ساکن CURLcode schannel_send(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                              ثابت خالی *buf, size_t len, size_t *pnwritten)
{
  size_t ڈیٹا_len = 0;
  غیرعلامتی حرف *ptr = تہی;
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  Sec_بفر outbuf[4];
  Sec_بفر_Desc outbuf_desc;
  SECURITY_حالت sspi_حالت = SEC_E_OK;
  CURLcode نتیجہ = CURLE_OK;
  ساخت schannel_ssl_backend_ڈیٹا *backend =
    (ساخت schannel_ssl_backend_ڈیٹا *)connssl->backend;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  اگر(backend->recv_renegotiating) {
    نتیجہ = schannel_recv_renegotiate(cf, ڈیٹا, SCH_RENEG_CALLER_IS_SEND);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  /* check if the buffer is longer than the maximum message length */
  اگر(len > backend->دھارا_sizes.cb_زیادہ_سے_زیادہ_پیغام) {
    len = backend->دھارا_sizes.cb_زیادہ_سے_زیادہ_پیغام;
  }

  /* calculate the complete message length and prepare the send buffer */
  ڈیٹا_len = backend->دھارا_sizes.cb_سرخی + len +
    backend->دھارا_sizes.cbTrailer;
  اگر(ڈیٹا_len > backend->send_بفر_len) {
    ptr = curlx_realloc(backend->send_بفر, ڈیٹا_len);
    اگر(!ptr)
      واپس CURLE_برآمد_کا_حافظہ;
    backend->send_بفر = ptr;
    backend->send_بفر_len = ڈیٹا_len;
  }
  ورنہ
    ptr = backend->send_بفر;

  /* setup output buffers (header, data, trailer, empty) */
  Init_Sec_بفر(&outbuf[0], SECBUFFER_دھارا_سرخی,
                ptr, backend->دھارا_sizes.cb_سرخی);
  Init_Sec_بفر(&outbuf[1], SECBUFFER_ڈیٹا,
                ptr + backend->دھارا_sizes.cb_سرخی, curlx_uztoul(len));
  Init_Sec_بفر(&outbuf[2], SECBUFFER_دھارا_TRAILER,
                ptr + backend->دھارا_sizes.cb_سرخی + len,
                backend->دھارا_sizes.cbTrailer);
  Init_Sec_بفر(&outbuf[3], SECBUFFER_خالی_خانے, تہی, 0);
  Init_Sec_بفر_Desc(&outbuf_desc, outbuf, 4);

  /* copy data into output buffer */
  حافظہ_نقل(outbuf[1].pv_بفر, buf, len);

  /* https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-encryptmessage */
  sspi_حالت = Curl_pSecFn->Encrypt_پیغام(&backend->ctxt->ctxt_handle, 0,
                                            &outbuf_desc, 0);

  /* check if the message was encrypted */
  اگر(sspi_حالت == SEC_E_OK) {

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
    جب(len > *pnwritten) {
      size_t this_write = 0;
      صحیح what;
      timediff_t timeout_ms = Curl_timeleft_ms(ڈیٹا);
      اگر(timeout_ms < 0) {
        /* we already got the timeout */
        failf(ڈیٹا, "schannel: timed out sending data (bytes sent: %zu)",
              *pnwritten);
        نتیجہ = CURLE_OPERATION_TIMEDOUT;
        توڑو;
      }
      ورنہ اگر(!timeout_ms)
        timeout_ms = TIMEDIFF_T_MAX;
      what = SOCKET_WRITABLE(Curl_conn_cf_get_socket(cf, ڈیٹا), timeout_ms);
      اگر(what < 0) {
        /* fatal error */
        failf(ڈیٹا, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        نتیجہ = CURLE_SEND_ERROR;
        توڑو;
      }
      ورنہ اگر(what == 0) {
        failf(ڈیٹا, "schannel: timed out sending data (bytes sent: %zu)",
              *pnwritten);
        نتیجہ = CURLE_OPERATION_TIMEDOUT;
        توڑو;
      }
      /* socket is writable */

       نتیجہ = Curl_conn_cf_send(cf->اگلا, ڈیٹا,
                                  (ثابت uint8_t *)ptr + *pnwritten,
                                  len - *pnwritten,
                                  FALSE, &this_write);
      اگر(نتیجہ == CURLE_AGAIN)
        جاری;
      ورنہ اگر(نتیجہ) {
        توڑو;
      }

      *pnwritten += this_write;
    }
  }
  ورنہ اگر(sspi_حالت == SEC_E_INSUFFICIENT_حافظہ) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
  }
  ورنہ {
    نتیجہ = CURLE_SEND_ERROR;
  }

  اگر(len == *pnwritten)
    /* Encrypted message including header, data and trailer entirely sent.
       The return value is the number of unencrypted bytes that were sent. */
    *pnwritten = outbuf[1].cb_بفر;

  واپس نتیجہ;
}

ساکن CURLcode schannel_recv(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                              حرف *buf, size_t len, size_t *pnread)
{
  size_t سائز = 0;
  size_t nread = 0;
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  Sec_بفر inbuf[4];
  Sec_بفر_Desc inbuf_desc;
  SECURITY_حالت sspi_حالت = SEC_E_OK;
  ساخت schannel_ssl_backend_ڈیٹا *backend =
    (ساخت schannel_ssl_backend_ڈیٹا *)connssl->backend;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(backend);
  *pnread = 0;

  اگر(backend->recv_renegotiating) {
    نتیجہ = schannel_recv_renegotiate(cf, ڈیٹا, SCH_RENEG_CALLER_IS_RECV);
    اگر(نتیجہ)
      واپس نتیجہ;
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

  SCH_DEV(infof(ڈیٹا, "schannel: client wants to read %zu bytes", len));

  اگر(len && len <= backend->decdata.انحراف) {
    SCH_DEV(infof(ڈیٹا,
                  "schannel: enough decrypted data is already available"));
    جاؤ cleanup;
  }
  ورنہ اگر(backend->recv_unrecoverable_err) {
    نتیجہ = backend->recv_unrecoverable_err;
    infof(ڈیٹا, "schannel: an unrecoverable error occurred in a prior call");
    جاؤ cleanup;
  }
  ورنہ اگر(backend->recv_sspi_close_notify) {
    /* once a server has indicated shutdown there is no more encrypted data */
    infof(ڈیٹا, "schannel: server indicated shutdown in a prior call");
    جاؤ cleanup;
  }
  /* it is debatable what to return when !len. Regardless we cannot return
     immediately because there may be data to decrypt (in the case we want to
     decrypt all encrypted cached data) so handle !len later in cleanup. */
  ورنہ اگر(len && !backend->recv_connection_closed) {
    /* the encrypted buffer must be large enough to hold all the bytes
       requested and some TLS record overhead. 'len' is a buffer size, so this
       integer math cannot overflow. */
    ثابت size_t min_encdata_لمبائی = len + CURL_SCHANNEL_بفر_FREE_سائز;

    /* make sure encrypt buffer fits the requested amount of data */
    نتیجہ = ensure_encoding_سائز(ڈیٹا, &backend->encdata, min_encdata_لمبائی);
    اگر(نتیجہ)
      جاؤ cleanup;

    SCH_DEV(infof(ڈیٹا,
                  "schannel: encrypted data buffer: offset %zu length %zu",
                  backend->encdata.انحراف, backend->encdata.لمبائی));

    /* read encrypted data from socket */
    نتیجہ = Curl_conn_cf_recv(cf->اگلا, ڈیٹا,
                               (حرف *)(backend->encdata.بفر +
                                        backend->encdata.انحراف),
                               backend->encdata.لمبائی -
                               backend->encdata.انحراف,
                               &nread);
    اگر(نتیجہ) {
      اگر(نتیجہ == CURLE_AGAIN)
        SCH_DEV(infof(ڈیٹا, "schannel: recv returned CURLE_AGAIN"));
      ورنہ {
        infof(ڈیٹا, "schannel: recv returned error %d", (صحیح)نتیجہ);
        backend->recv_unrecoverable_err = نتیجہ;
      }
    }
    ورنہ اگر(nread == 0) {
      backend->recv_connection_closed = TRUE;
      DEBUGF(infof(ڈیٹا, "schannel: server closed the connection"));
    }
    ورنہ {
      backend->encdata.انحراف += nread;
      backend->encdata_is_incomplete = FALSE;
      SCH_DEV(infof(ڈیٹا, "schannel: encrypted data got %zu", nread));
    }
  }

  SCH_DEV(infof(ڈیٹا, "schannel: encrypted data buffer: offset %zu length %zu",
                backend->encdata.انحراف, backend->encdata.لمبائی));

  /* decrypt loop */
  جب(backend->encdata.انحراف > 0 && sspi_حالت == SEC_E_OK &&
        (!len || backend->decdata.انحراف < len ||
         backend->recv_connection_closed)) {
    /* prepare data buffer for DecryptMessage call */
    Init_Sec_بفر(&inbuf[0], SECBUFFER_ڈیٹا, backend->encdata.بفر,
                  curlx_uztoul(backend->encdata.انحراف));

    /* we need 3 more empty input buffers for possible output */
    Init_Sec_بفر(&inbuf[1], SECBUFFER_خالی_خانے, تہی, 0);
    Init_Sec_بفر(&inbuf[2], SECBUFFER_خالی_خانے, تہی, 0);
    Init_Sec_بفر(&inbuf[3], SECBUFFER_خالی_خانے, تہی, 0);
    Init_Sec_بفر_Desc(&inbuf_desc, inbuf, 4);

    /* https://learn.microsoft.com/windows/win32/api/sspi/nf-sspi-decryptmessage
     */
    sspi_حالت = Curl_pSecFn->Decrypt_پیغام(&backend->ctxt->ctxt_handle,
                                              &inbuf_desc, 0, تہی);

    /* check if everything went fine (server may want to renegotiate
       or shutdown the connection context) */
    اگر(sspi_حالت == SEC_E_OK || sspi_حالت == SEC_I_RENEGOTIATE ||
       sspi_حالت == SEC_I_CONTEXT_EXPIRED) {
      /* check for successfully decrypted data, even before actual
         renegotiation or shutdown of the connection context */
      اگر(inbuf[1].بفر_Type == SECBUFFER_ڈیٹا) {
        SCH_DEV(infof(ڈیٹا, "schannel: decrypted data length: %lu",
                      inbuf[1].cb_بفر));

        /* ensure the decode buffer fits the received amount of data */
        نتیجہ = ensure_decoding_سائز(ڈیٹا, &backend->decdata,
                                      inbuf[1].cb_بفر, len);
        اگر(نتیجہ)
          جاؤ cleanup;

        /* copy decrypted data to internal buffer */
        سائز = inbuf[1].cb_بفر;
        اگر(سائز) {
          حافظہ_نقل(backend->decdata.بفر + backend->decdata.انحراف,
                 inbuf[1].pv_بفر, سائز);
          backend->decdata.انحراف += سائز;
        }

        SCH_DEV(infof(ڈیٹا, "schannel: decrypted data added: %zu", سائز));
        SCH_DEV(infof(ڈیٹا,
                      "schannel: decrypted cached: offset %zu length %zu",
                      backend->decdata.انحراف, backend->decdata.لمبائی));
      }

      /* check for remaining encrypted data */
      اگر(inbuf[3].بفر_Type == SECBUFFER_EXTRA && inbuf[3].cb_بفر > 0) {
        SCH_DEV(infof(ڈیٹا, "schannel: encrypted data length: %lu",
                      inbuf[3].cb_بفر));

        /* check if the remaining data is less than the total amount
         * and therefore begins after the already processed data
         */
        اگر(backend->encdata.انحراف > inbuf[3].cb_بفر) {
          /* move remaining encrypted data forward to the beginning of
بفر*/
          memmove(backend->encdata.بفر,
                  (backend->encdata.بفر + backend->encdata.انحراف) -
                  inbuf[3].cb_بفر, inbuf[3].cb_بفر);
          backend->encdata.انحراف = inbuf[3].cb_بفر;
        }

        SCH_DEV(infof(ڈیٹا,
                      "schannel: encrypted cached: offset %zu length %zu",
                      backend->encdata.انحراف, backend->encdata.لمبائی));
      }
      ورنہ {
        /* reset encrypted buffer offset, because there is no data remaining */
        backend->encdata.انحراف = 0;
      }

      /* check if server wants to renegotiate the connection context */
      اگر(sspi_حالت == SEC_I_RENEGOTIATE) {
        infof(ڈیٹا, "schannel: remote party requests renegotiation");
        اگر(نتیجہ && نتیجہ != CURLE_AGAIN) {
          infof(ڈیٹا, "schannel: cannot renegotiate, an error is pending");
          جاؤ cleanup;
        }

        backend->recv_renegotiating = TRUE;
        نتیجہ = schannel_recv_renegotiate(cf, ڈیٹا, SCH_RENEG_CALLER_IS_RECV);
        اگر(نتیجہ)
          جاؤ cleanup;

        /* now retry receiving data */
        sspi_حالت = SEC_E_OK;
        جاری;
      }
      /* check if the server closed the connection */
      ورنہ اگر(sspi_حالت == SEC_I_CONTEXT_EXPIRED) {
        /* In Windows 2000 SEC_I_CONTEXT_EXPIRED (close_notify) is not
           returned so we have to work around that in cleanup. */
        backend->recv_sspi_close_notify = TRUE;
        اگر(!backend->recv_connection_closed)
          backend->recv_connection_closed = TRUE;
        /* We received the close notify fine, any error we got
         * from the lower filters afterwards (e.g. the socket), is not
         * an error on the TLS data stream. That one ended here. */
        اگر(نتیجہ == CURLE_RECV_ERROR)
          نتیجہ = CURLE_OK;
        infof(ڈیٹا,
              "schannel: server close notification received (close_notify)");
        جاؤ cleanup;
      }
    }
    ورنہ اگر(sspi_حالت == SEC_E_INCOMPLETE_پیغام) {
      backend->encdata_is_incomplete = TRUE;
      اگر(!نتیجہ)
        نتیجہ = CURLE_AGAIN;
      SCH_DEV(infof(ڈیٹا, "schannel: failed to decrypt data, need more data"));
      جاؤ cleanup;
    }
    ورنہ {
      حرف بفر[STRERROR_LEN];
      failf(ڈیٹا, "schannel: failed to read data from server: %s",
            Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
      نتیجہ = CURLE_RECV_ERROR;
      جاؤ cleanup;
    }
  }

  SCH_DEV(infof(ڈیٹا, "schannel: encrypted data buffer: offset %zu length %zu",
                backend->encdata.انحراف, backend->encdata.لمبائی));

  SCH_DEV(infof(ڈیٹا, "schannel: decrypted data buffer: offset %zu length %zu",
                backend->decdata.انحراف, backend->decdata.لمبائی));

cleanup:
  /* Warning- there is no guarantee the encdata state is valid at this point */
  SCH_DEV(infof(ڈیٹا, "schannel: schannel_recv cleanup"));

  /* Error if the connection has closed without a close_notify.

     The behavior here is a matter of debate. We do not want to be vulnerable
     to a truncation attack however there is some browser precedent for
     ignoring the close_notify for compatibility reasons. */
  اگر(len && !backend->decdata.انحراف && backend->recv_connection_closed &&
     !backend->recv_sspi_close_notify) {
    نتیجہ = CURLE_RECV_ERROR;
    failf(ڈیٹا, "schannel: server closed abruptly (missing close_notify)");
  }

  /* Any error other than CURLE_AGAIN is an unrecoverable error. */
  اگر(نتیجہ && نتیجہ != CURLE_AGAIN)
    backend->recv_unrecoverable_err = نتیجہ;

  سائز = len < backend->decdata.انحراف ? len : backend->decdata.انحراف;
  اگر(سائز) {
    حافظہ_نقل(buf, backend->decdata.بفر, سائز);
    memmove(backend->decdata.بفر, backend->decdata.بفر + سائز,
            backend->decdata.انحراف - سائز);
    backend->decdata.انحراف -= سائز;
    SCH_DEV(infof(ڈیٹا, "schannel: decrypted data returned %zu", سائز));
    SCH_DEV(infof(ڈیٹا,
                  "schannel: decrypted data buffer: offset %zu length %zu",
                  backend->decdata.انحراف, backend->decdata.لمبائی));
    *pnread = سائز;
    واپس CURLE_OK;
  }

  اگر(!نتیجہ && !backend->recv_connection_closed)
    نتیجہ = CURLE_AGAIN;

  /* it is debatable what to return when !len. We could return whatever error
     we got from decryption but instead we override here so the return is
     consistent. */
  اگر(!len)
    واپس CURLE_OK;

  واپس نتیجہ;
}

ساکن bool schannel_ڈیٹا_pending(ساخت Curl_cfilter *cf,
                                  ثابت ساخت Curl_easy *ڈیٹا)
{
  ثابت ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت schannel_ssl_backend_ڈیٹا *backend =
    (ساخت schannel_ssl_backend_ڈیٹا *)connssl->backend;

  (خالی)ڈیٹا;
  DEBUGASSERT(backend);

  اگر(backend->ctxt) /* SSL/TLS is in use */
    واپس backend->decdata.انحراف > 0 ||
           (backend->encdata.انحراف > 0 && !backend->encdata_is_incomplete) ||
           backend->recv_connection_closed ||
           backend->recv_sspi_close_notify ||
           backend->recv_unrecoverable_err;
  ورنہ
    واپس FALSE;
}

/* shut down the SSL connection and clean up related memory.
   this function can be called multiple times on the same connection including
   if the SSL connection failed (eg connection made but failed handshake). */
ساکن CURLcode schannel_shutdown(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  bool send_shutdown, bool *مکمل)
{
  /* See https://learn.microsoft.com/windows/win32/secauthn/shutting-down-an-schannel-connection
   * Shutting Down an Schannel Connection
   */
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت schannel_ssl_backend_ڈیٹا *backend =
    (ساخت schannel_ssl_backend_ڈیٹا *)connssl->backend;
  CURLcode نتیجہ = CURLE_OK;

  اگر(cf->shutdown) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(backend);

  /* Not supported in schannel */
  (خالی)send_shutdown;

  *مکمل = FALSE;
  اگر(backend->ctxt) {
    infof(ڈیٹا, "schannel: shutting down SSL/TLS connection with %s port %d",
          connssl->peer.origin->hostname, connssl->peer.origin->پورٹ);
  }

  اگر(!backend->ctxt || cf->shutdown) {
    *مکمل = TRUE;
    جاؤ برآمد;
  }

  اگر(backend->cred && backend->ctxt && !backend->بھیجا_shutdown &&
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

    اگر(sspi_حالت != SEC_E_OK) {
      حرف بفر[STRERROR_LEN];
      failf(ڈیٹا, "schannel: ApplyControlToken failure: %s",
            Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
      نتیجہ = CURLE_SEND_ERROR;
      جاؤ برآمد;
    }

    /* setup output buffer */
    Init_Sec_بفر(&outbuf, SECBUFFER_خالی_خانے, تہی, 0);
    Init_Sec_بفر_Desc(&outbuf_desc, &outbuf, 1);

    sspi_حالت = Curl_pSecFn->InitializeSecurityContext(
      &backend->cred->cred_handle,
      &backend->ctxt->ctxt_handle,
      backend->cred->sni_hostname,
      backend->req_flags,
      0,
      0,
      تہی,
      0,
      &backend->ctxt->ctxt_handle,
      &outbuf_desc,
      &backend->ret_flags, تہی);

    اگر((sspi_حالت == SEC_E_OK) || (sspi_حالت == SEC_I_CONTEXT_EXPIRED)) {
      backend->shutdown_token = outbuf;
      backend->shutdown_generated = TRUE;
    }
    ورنہ {
      حرف بفر[STRERROR_LEN];
      اگر(outbuf.pv_بفر)
        Curl_pSecFn->Free_Context_بفر(outbuf.pv_بفر);
      failf(ڈیٹا, "schannel: generating close msg failed: %s",
            Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
      نتیجہ = CURLE_SEND_ERROR;
      جاؤ برآمد;
    }
  }

  اگر(backend->shutdown_generated && !backend->بھیجا_shutdown) {
    size_t written = 0;
    size_t بقایا;

    DEBUGASSERT(backend->shutdown_انحراف <= backend->shutdown_token.cb_بفر);
    بقایا = backend->shutdown_token.cb_بفر - backend->shutdown_انحراف;

    اگر(بقایا) {
      نتیجہ = Curl_conn_cf_send(cf->اگلا, ڈیٹا,
                                 ((uint8_t *)backend->shutdown_token.pv_بفر +
                                  backend->shutdown_انحراف),
                                 بقایا, FALSE, &written);
      اگر(!نتیجہ)
        backend->shutdown_انحراف += written;
      اگر(نتیجہ == CURLE_AGAIN || (!نتیجہ && written < بقایا)) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        نتیجہ = CURLE_OK;
        جاؤ برآمد;
      }
    }
    ورنہ /* no shutdown message to send? */
      نتیجہ = CURLE_OK;
    اگر(نتیجہ) {
      اگر(backend->recv_connection_closed) {
        /* The server already closed, so sending close_notify is optional. */
        *مکمل = TRUE;
        نتیجہ = CURLE_OK;
      }
      ورنہ {
        failf(ڈیٹا, "schannel: error sending close msg: %d", (صحیح)نتیجہ);
        نتیجہ = CURLE_SEND_ERROR;
      }
      جاؤ برآمد;
    }
    اگر(backend->shutdown_token.pv_بفر)
      Curl_pSecFn->Free_Context_بفر(backend->shutdown_token.pv_بفر);
    memset(&backend->shutdown_token, 0, حجم(backend->shutdown_token));
    backend->shutdown_انحراف = 0;
    backend->بھیجا_shutdown = TRUE;
    *مکمل = TRUE;
  }

  /* If the connection seems open and we have not seen the close notify
   * from the server yet, try to receive it. */
  اگر(backend->cred && backend->ctxt &&
     !backend->recv_sspi_close_notify && !backend->recv_connection_closed) {
    حرف بفر[1024];
    size_t nread;

    نتیجہ = schannel_recv(cf, ڈیٹا, بفر, حجم(بفر), &nread);
    اگر(نتیجہ == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
    }
    ورنہ اگر(نتیجہ) {
      CURL_TRC_CF(ڈیٹا, cf, "SSL shutdown, error %d", (صحیح)نتیجہ);
      نتیجہ = CURLE_RECV_ERROR;
    }
    ورنہ اگر(nread == 0) {
      /* We got the close notify alert and are done. */
      backend->recv_connection_closed = TRUE;
      *مکمل = TRUE;
    }
    ورنہ {
      /* still data coming in? */
    }
  }

برآمد:
  cf->shutdown = (نتیجہ || *مکمل);
  واپس نتیجہ;
}

ساکن خالی schannel_close(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت schannel_ssl_backend_ڈیٹا *backend =
    (ساخت schannel_ssl_backend_ڈیٹا *)connssl->backend;

  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(backend);

  اگر(backend->shutdown_token.pv_بفر)
    Curl_pSecFn->Free_Context_بفر(backend->shutdown_token.pv_بفر);
  memset(&backend->shutdown_token, 0, حجم(backend->shutdown_token));
  backend->shutdown_انحراف = 0;

  /* free SSPI Schannel API security context handle */
  اگر(backend->ctxt) {
    اگر(cf->conn->sslContext == &backend->ctxt->ctxt_handle)
      cf->conn->sslContext = تہی;

    DEBUGF(infof(ڈیٹا, "schannel: clear security context handle"));
    Curl_pSecFn->DeleteSecurityContext(&backend->ctxt->ctxt_handle);
    curlx_safefree(backend->ctxt);
  }

  /* free SSPI Schannel API credential handle */
  اگر(backend->cred) {
    Curl_ssl_scache_تالا(ڈیٹا);
    schannel_session_free(backend->cred);
    Curl_ssl_scache_unlock(ڈیٹا);
    backend->cred = تہی;
  }

  /* free the buffer used to encrypt outgoing data */
  curlx_safefree(backend->send_بفر);
  backend->send_بفر_len = 0;

  /* free internal buffer for received encrypted data */
  اگر(backend->encdata.بفر) {
    curlx_safefree(backend->encdata.بفر);
    backend->encdata.لمبائی = 0;
    backend->encdata.انحراف = 0;
    backend->encdata_is_incomplete = FALSE;
  }

  /* free internal buffer for received decrypted data */
  اگر(backend->decdata.بفر) {
    curlx_safefree(backend->decdata.بفر);
    backend->decdata.لمبائی = 0;
    backend->decdata.انحراف = 0;
  }
}

ساکن صحیح schannel_init(خالی)
{
  قسمتعریف ثابت حرف *(APIENTRY *WINE_GET_نسخہ_FN)(خالی);
#اگر defined(__clang__) && __clang_major__ >= 16
#ہدایت clang diagnostic ڈالو
#ہدایت clang diagnostic ignored "-Wcast-function-type-strict"
#اگر_ختم
  WINE_GET_نسخہ_FN p_wine_get_نسخہ =
    CURLX_FUNCTION_CAST(WINE_GET_نسخہ_FN,
      Get_Proc_پتہ(GetModuleHandle(متن_2("ntdll")), "wine_get_version"));
#اگر defined(__clang__) && __clang_major__ >= 16
#ہدایت clang diagnostic نکالو
#اگر_ختم
  اگر(p_wine_get_نسخہ) {  /* WINE detected */
    curl_off_t ver = 0;
    ثابت حرف *wine_نسخہ = p_wine_get_نسخہ();  /* e.g. "6.0.2" */
    /* Assume ALPN support with WINE 6.0 or greater */
    اگر(wine_نسخہ)
      curlx_str_نمبر(&wine_نسخہ, &ver, 20);
    s_win_has_alpn = (ver >= 6);
  }
  ورنہ {
    /* ALPN is supported on Windows 8.1 / Server 2012 R2 or newer. */
    s_win_has_alpn = curlx_verify_windows_نسخہ(6, 3, 0, PLATFORM_WINNT,
                                                  نسخہ_GREATER_THAN_EQUAL);
  }

  واپس Curl_sspi_global_init() == CURLE_OK ? 1 : 0;
}

ساکن خالی schannel_cleanup(خالی)
{
  Curl_sspi_global_cleanup();
}

ساکن size_t schannel_نسخہ(حرف *بفر, size_t سائز)
{
  واپس curl_msnprintf(بفر, سائز, "Schannel");
}

ساکن CURLcode schannel_random(ساخت Curl_easy *ڈیٹا,
                                غیرعلامتی حرف *entropy, size_t لمبائی)
{
  (خالی)ڈیٹا;

  واپس curlx_win32_random(entropy, لمبائی);
}

ساکن CURLcode schannel_checksum(ثابت غیرعلامتی حرف *داخلہ,
                                  size_t inputlen,
                                  غیرعلامتی حرف *checksum,
                                  size_t checksumlen,
                                  DWORD provType,
                                  ثابت غیرعلامتی صحیح algId)
{
  CURLcode نتیجہ = CURLE_FAILED_INIT;

  HCRYPTPROV hProv = 0;
  HCRYPTHASH hHash = 0;

  size_t off;

  DWORD cb_Hash_سائز;
  DWORD dw_Hash_سائز_Len;
  DWORD dwChecksumLen;

  /* since this can fail in multiple ways, zero memory first so we never
   * return old data
   */
  memset(checksum, 0, checksumlen);

  اگر(!CryptAcquireContext(&hProv, تہی, تہی, provType,
                          CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    جاؤ برآمد;

  اگر(!CryptCreateHash(hProv, algId, 0, 0, &hHash))
    جاؤ برآمد;

  نتیجہ = CURLE_BAD_FUNCTION_دلیل;

  off = 0;
  جب(off < inputlen) {
    DWORD chunk = (DWORD)CURLMIN(inputlen - off, 0xffffffffUL);
    اگر(!Crypt_Hash_ڈیٹا(hHash, داخلہ + off, chunk, 0))
      جاؤ برآمد;
    off += chunk;
  }

  /* get hash size */
  cb_Hash_سائز = 0;
  dw_Hash_سائز_Len = (DWORD)حجم(cb_Hash_سائز);
  اگر(!CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE *)&cb_Hash_سائز,
                        &dw_Hash_سائز_Len, 0))
    جاؤ برآمد;

  /* check if hash fits into the return buffer */
  اگر(checksumlen < cb_Hash_سائز)
    جاؤ برآمد;

  dwChecksumLen = (DWORD)checksumlen;
  اگر(CryptGetHashParam(hHash, HP_HASHVAL, checksum, &dwChecksumLen, 0) &&
     dwChecksumLen == cb_Hash_سائز)
    نتیجہ = CURLE_OK;

برآمد:
  اگر(hHash)
    CryptDestroyHash(hHash);

  اگر(hProv)
    CryptReleaseContext(hProv, 0);

  واپس نتیجہ;
}

ساکن CURLcode schannel_sha256sum(ثابت غیرعلامتی حرف *داخلہ,
                                   size_t inputlen,
                                   غیرعلامتی حرف *sha256sum,
                                   size_t sha256len)
{
  واپس schannel_checksum(داخلہ, inputlen, sha256sum, sha256len,
                           PROV_RSA_AES, CALG_SHA_256);
}

ساکن خالی *schannel_get_internals(ساخت ssl_connect_ڈیٹا *connssl,
                                    CURLINFO info)
{
  ساخت schannel_ssl_backend_ڈیٹا *backend =
    (ساخت schannel_ssl_backend_ڈیٹا *)connssl->backend;
  (خالی)info;
  DEBUGASSERT(backend);
  واپس &backend->ctxt->ctxt_handle;
}

HCERTSTORE Curl_schannel_get_cached_cert_store(ساخت Curl_cfilter *cf,
                                               ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ساخت Curl_multi *multi = ڈیٹا->multi;
  ثابت ساخت curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ساخت schannel_cert_share *share;
  timediff_t timeout_ms;
  غیرعلامتی حرف info_blob_digest[CURL_SHA256_DIGEST_لمبائی];

  DEBUGASSERT(multi);

  اگر(!multi) {
    واپس تہی;
  }

  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_SCHANNEL_CERT_SHARE_کلید,
                         CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_کلید));
  اگر(!share || !share->cert_store) {
    واپس تہی;
  }

  /* zero ca_cache_timeout completely disables caching */
  اگر(!ڈیٹا->سیٹ.ssl_ca_cache_timeout) {
    واپس تہی;
  }

  /* check for cache timeout by using the cached_x509_store_expired timediff
     calculation pattern from openssl.c.
     negative timeout means retain forever. */
  timeout_ms = ڈیٹا->سیٹ.ssl_ca_cache_timeout * (timediff_t)1000;
  اگر(timeout_ms >= 0) {
    timediff_t elapsed_ms =
      curlx_ptimediff_ms(Curl_pgrs_now(ڈیٹا), &share->وقت);
    اگر(elapsed_ms >= timeout_ms) {
      واپس تہی;
    }
  }

  اگر(ca_info_blob) {
    اگر(share->CAinfo_blob_سائز != ca_info_blob->len) {
      واپس تہی;
    }
    اگر(schannel_sha256sum((ثابت غیرعلامتی حرف *)ca_info_blob->ڈیٹا,
                          ca_info_blob->len,
                          info_blob_digest,
                          CURL_SHA256_DIGEST_لمبائی))
      واپس تہی;
    اگر(memcmp(share->CAinfo_blob_digest, info_blob_digest,
              CURL_SHA256_DIGEST_لمبائی)) {
      واپس تہی;
    }
  }
  ورنہ {
    اگر(!conn_config->CAfile || !share->CAfile ||
       strcmp(share->CAfile, conn_config->CAfile)) {
      واپس تہی;
    }
  }

  واپس share->cert_store;
}

ساکن خالی schannel_cert_share_free(ثابت خالی *کلید, size_t کلید_len, خالی *p)
{
  ساخت schannel_cert_share *share = p;
  DEBUGASSERT(کلید_len == CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_کلید));
  DEBUGASSERT(!memcmp(MPROTO_SCHANNEL_CERT_SHARE_کلید, کلید, کلید_len));
  (خالی)کلید;
  (خالی)کلید_len;
  اگر(share->cert_store) {
    CertCloseStore(share->cert_store, 0);
  }
  curlx_free(share->CAfile);
  curlx_free(share);
}

bool Curl_schannel_سیٹ_cached_cert_store(ساخت Curl_cfilter *cf,
                                         ساخت Curl_easy *ڈیٹا,
                                         HCERTSTORE cert_store)
{
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ساخت Curl_multi *multi = ڈیٹا->multi;
  ثابت ساخت curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ساخت schannel_cert_share *share;
  غیرعلامتی حرف digest[CURL_SHA256_DIGEST_لمبائی];
  حرف *CAfile = تہی;

  DEBUGASSERT(multi);

  اگر(!multi) {
    واپس FALSE;
  }

  اگر(ca_info_blob) {
    اگر(schannel_sha256sum((ثابت غیرعلامتی حرف *)ca_info_blob->ڈیٹا,
                          ca_info_blob->len, digest, حجم(digest))) {
      واپس FALSE;
    }
  }
  ورنہ اگر(conn_config->CAfile) {
    CAfile = curlx_strdup(conn_config->CAfile);
    اگر(!CAfile) {
      واپس FALSE;
    }
  }

  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_SCHANNEL_CERT_SHARE_کلید,
                         CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_کلید));
  اگر(!share) {
    share = curlx_calloc(1, حجم(*share));
    اگر(!share) {
      curlx_free(CAfile);
      واپس FALSE;
    }
    اگر(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_SCHANNEL_CERT_SHARE_کلید,
                       CURL_CSTRLEN(MPROTO_SCHANNEL_CERT_SHARE_کلید),
                       share, schannel_cert_share_free)) {
      curlx_free(share);
      curlx_free(CAfile);
      واپس FALSE;
    }
  }

  /* free old cache data */
  اگر(share->cert_store) {
    CertCloseStore(share->cert_store, 0);
  }
  curlx_free(share->CAfile);

  اگر(ca_info_blob) {
    حافظہ_نقل(share->CAinfo_blob_digest, digest, حجم(digest));
    share->CAinfo_blob_سائز = ca_info_blob->len;
  }
  ورنہ
    share->CAinfo_blob_سائز = 0;

  share->وقت = curlx_now();
  share->cert_store = cert_store;
  share->CAfile = CAfile;
  واپس TRUE;
}

ثابت ساخت Curl_ssl Curl_ssl_schannel = {
  { CURLSSLBACKEND_SCHANNEL, "schannel" }, /* info */

  SSLSUPP_CERTINFO |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_CA_CACHE |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST,

  حجم(ساخت schannel_ssl_backend_ڈیٹا),

  schannel_init,                     /* init */
  schannel_cleanup,                  /* cleanup */
  schannel_نسخہ,                  /*نسخہ*/
  schannel_shutdown,                 /* shutdown */
  schannel_ڈیٹا_pending,             /* data_pending */
  schannel_random,                   /* random */
  تہی,                              /* cert_status_request */
  schannel_connect,                  /* connect */
  Curl_ssl_adjust_pollset,           /* adjust_pollset */
  schannel_get_internals,            /* get_internals */
  schannel_close,                    /* close_one */
  تہی,                              /* close_all */
  تہی,                              /* engines_list */
  schannel_sha256sum,                /* sha256sum */
  schannel_recv,                     /* recv decrypted data */
  schannel_send,                     /* send data to encrypt */
  تہی,                              /* get_channel_binding */
};

#اگر_ختم /* USE_SCHANNEL */
