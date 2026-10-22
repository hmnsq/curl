#اگر_تعریف_نشده سرآیند_CURL_SCHANNEL_INT_H
#تعریف_کن سرآیند_CURL_SCHANNEL_INT_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Marc Hoersken, <info@marc-hoersken.de>, et al.
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
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
#ضمن "curl_setup.h"

#اگر_تعریف_شده USE_SCHANNEL

#ضمن "vtls/vtls.h"
#ضمن "curl_sha256.h"

#إذا defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'LPCSTR' of greater size */
#لغو_تعریف CERT_STORE_PROV_حافظه
#لغو_تعریف CERT_STORE_PROV_SYSTEM_A
#لغو_تعریف CERT_STORE_PROV_SYSTEM_W
#تعریف_کن CERT_STORE_PROV_حافظه    ((LPCSTR)(size_t)2)
#تعریف_کن CERT_STORE_PROV_SYSTEM_A  ((LPCSTR)(size_t)9)
#تعریف_کن CERT_STORE_PROV_SYSTEM_W  ((LPCSTR)(size_t)10)
#پایان_اگر

/* Offered by mingw-w64 v8+, MS SDK 10.0.17763.0/VS2017 15.8+ */
#اگر_تعریف_نشده SCH_CREDENTIALS_نسخه
#تعریف_کن SCH_CREDENTIALS_نسخه  0x00000005

تعريفنوع تعداد _eTlsAlgorithmUsage {
  Tls_Parameters_Cng_Alg_Usage_کلید_Exchange,
  TlsParametersCngAlgUsageSignature,
  TlsParametersCngAlgUsageCipher,
  TlsParametersCngAlgUsageDigest,
  TlsParametersCngAlgUsageCertSig
} eTlsAlgorithmUsage;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
تعريفنوع بنية _CRYPTO_SETTINGS {
  eTlsAlgorithmUsage eAlgorithmUsage;
  UNICODE_STRING strCngAlgId;
  DWORD cChainingModes;
  PUNICODE_STRING rgstrChainingModes; /* spellchecker:disable-line */
  DWORD dw_Min_بیت_درازی;
  DWORD dw_Max_بیت_درازی;
} CRYPTO_SETTINGS, *PCRYPTO_SETTINGS;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
تعريفنوع بنية _TLS_PARAMETERS {
  DWORD cAlpnIds;
  PUNICODE_STRING rgstrAlpnIds; /* spellchecker:disable-line */
  DWORD grbitDisabledProtocols;
  DWORD cDisabledCrypto;
  PCRYPTO_SETTINGS pDisabledCrypto;
  DWORD dwFlags;
} TLS_PARAMETERS, *PTLS_PARAMETERS;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
تعريفنوع بنية _SCH_CREDENTIALS {
  DWORD dw_نسخه;
  DWORD dwCredFormat;
  DWORD cCreds;
  PCCERT_CONTEXT *paCred;
  HCERTSTORE h_ریشه_Store;

  DWORD cMappers;
  بنية _HMAPPER **aphMappers;

  DWORD dwSessionLifespan;
  DWORD dwFlags;
  DWORD cTlsParameters;
  PTLS_PARAMETERS pTlsParameters;
} SCH_CREDENTIALS, *PSCH_CREDENTIALS;

#پایان_اگر /* !SCH_CREDENTIALS_VERSION */

بنية Curl_schannel_cred {
  CredHandle cred_handle;
  TCHAR *sni_hostname;
  HCERTSTORE client_cert_store;
  صحيح refcount;
};

بنية Curl_schannel_ctxt {
  CtxtHandle ctxt_handle;
};

/* handle encoding/decoding buffers */
بنية sbuffer {
  size_t درازی;
  size_t جابه_جایی;
  بیعلامت حرف *میانگیر;
};

بنية schannel_ssl_backend_داده {
  بیعلامت حرف *send_میانگیر;
  size_t send_میانگیر_len;
  بنية sbuffer encdata;
  بنية sbuffer decdata;
  بنية Curl_schannel_cred *cred;
  بنية Curl_schannel_ctxt *ctxt;
  Sec_میانگیر shutdown_token;
  size_t shutdown_جابه_جایی;
  Sec_Pkg_Context_جریان_Sizes جریان_sizes;
  بیعلامت دراز req_flags, ret_flags;
  CURLcode recv_unrecoverable_err; /* schannel_recv had an unrecoverable err */
  بنية schannel_renegotiate_state {
    bool started;
    بنية curltime شروع_زمان;
    صحيح io_need;
  } renegotiate_state;
  بیت(recv_sspi_close_notify); /* true if connection closed by close_notify */
  بیت(recv_connection_closed); /* true if connection closed, regardless how */
  بیت(recv_renegotiating);     /* true if recv is doing renegotiation */
  بیت(use_alpn); /* true if ALPN is used for this connection */
  بیت(use_manual_cred_validation); /* true if manual cred validation is used */
  بیت(فرستاده_shutdown);
  بیت(shutdown_generated);
  /* encdata_is_incomplete: if encdata contains only a partial record that
     cannot be decrypted without another recv() (that is, status is
     SEC_E_INCOMPLETE_MESSAGE) then set this true. after recv() adds more
     bytes into encdata then set this back to false. */
  بیت(encdata_is_incomplete);
};

بنية schannel_cert_share {
  بیعلامت حرف CAinfo_blob_digest[CURL_SHA256_DIGEST_درازی];
  size_t CAinfo_blob_اندازه;           /* CA info blob size */
  حرف *CAfile;                      /* CAfile path used to generate
                                        certificate store */
  HCERTSTORE cert_store;             /* cached certificate store or
                                        NULL if none */
  بنية curltime زمان;              /* when the cached store was created */
};

/* size of the structure: 20 bytes */
بنية رقم_ip_داده {
  DWORD اندازه; /* 04 bytes */
  اتحاد {
    بنية in_addr  ia;  /* 04 bytes */
    بنية in6_addr ia6; /* 16 bytes */
  } b_داده;
};

HCERTSTORE Curl_schannel_get_cached_cert_store(بنية Curl_cfilter *cf,
                                               بنية Curl_easy *داده);

bool Curl_schannel_مجموعه_cached_cert_store(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *داده,
                                         HCERTSTORE cert_store);

#پایان_اگر /* USE_SCHANNEL */
#پایان_اگر /* HEADER_CURL_SCHANNEL_INT_H */
