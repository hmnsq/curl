#ifndef HEADER_CURL_SCHANNEL_INT_H
#define HEADER_CURL_SCHANNEL_INT_H
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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef USE_SCHANNEL

#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"
#ଅନ୍ତର୍ଭାଵ "curl_sha256.h"

#ଯଦି defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'LPCSTR' of greater size */
#undef CERT_STORE_PROV_MEMORY
#undef CERT_STORE_PROV_SYSTEM_A
#undef CERT_STORE_PROV_SYSTEM_W
#define CERT_STORE_PROV_MEMORY    ((LPCSTR)(size_t)2)
#define CERT_STORE_PROV_SYSTEM_A  ((LPCSTR)(size_t)9)
#define CERT_STORE_PROV_SYSTEM_W  ((LPCSTR)(size_t)10)
#endif

/* Offered by mingw-w64 v8+, MS SDK 10.0.17763.0/VS2017 15.8+ */
#ifndef SCH_CREDENTIALS_VERSION
#define SCH_CREDENTIALS_VERSION  0x00000005

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା _eTlsAlgorithmUsage {
  Tls_Parameters_Cng_Alg_Usage_ଚାବି_Exchange,
  TlsParametersCngAlgUsageSignature,
  TlsParametersCngAlgUsageCipher,
  TlsParametersCngAlgUsageDigest,
  TlsParametersCngAlgUsageCertSig
} eTlsAlgorithmUsage;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା _CRYPTO_SETTINGS {
  eTlsAlgorithmUsage eAlgorithmUsage;
  UNICODE_STRING strCngAlgId;
  DWORD cChainingModes;
  PUNICODE_STRING rgstrChainingModes; /* spellchecker:disable-line */
  DWORD dwMinBitLength;
  DWORD dwMaxBitLength;
} CRYPTO_SETTINGS, *PCRYPTO_SETTINGS;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା _TLS_PARAMETERS {
  DWORD cAlpnIds;
  PUNICODE_STRING rgstrAlpnIds; /* spellchecker:disable-line */
  DWORD grbitDisabledProtocols;
  DWORD cDisabledCrypto;
  PCRYPTO_SETTINGS pDisabledCrypto;
  DWORD dwFlags;
} TLS_PARAMETERS, *PTLS_PARAMETERS;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା _SCH_CREDENTIALS {
  DWORD dwVersion;
  DWORD dwCredFormat;
  DWORD cCreds;
  PCCERT_CONTEXT *paCred;
  HCERTSTORE h_ମୂଳ_Store;

  DWORD cMappers;
  ସଂରଚନା _HMAPPER **aphMappers;

  DWORD dwSessionLifespan;
  DWORD dwFlags;
  DWORD cTlsParameters;
  PTLS_PARAMETERS pTlsParameters;
} SCH_CREDENTIALS, *PSCH_CREDENTIALS;

#endif /* !SCH_CREDENTIALS_VERSION */

ସଂରଚନା Curl_schannel_cred {
  CredHandle cred_handle;
  TCHAR *sni_hostname;
  HCERTSTORE client_cert_store;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା refcount;
};

ସଂରଚନା Curl_schannel_ctxt {
  CtxtHandle ctxt_handle;
};

/* handle encoding/decoding buffers */
ସଂରଚନା sbuffer {
  size_t length;
  size_t offset;
  unsigned ଅକ୍ଷର *buffer;
};

ସଂରଚନା schannel_ssl_backend_data {
  unsigned ଅକ୍ଷର *send_buffer;
  size_t send_buffer_len;
  ସଂରଚନା sbuffer encdata;
  ସଂରଚନା sbuffer decdata;
  ସଂରଚନା Curl_schannel_cred *cred;
  ସଂରଚନା Curl_schannel_ctxt *ctxt;
  SecBuffer shutdown_token;
  size_t shutdown_offset;
  SecPkgContext_StreamSizes stream_sizes;
  unsigned long req_flags, ret_flags;
  CURLcode recv_unrecoverable_err; /* schannel_recv had an unrecoverable err */
  ସଂରଚନା schannel_renegotiate_state {
    bool started;
    ସଂରଚନା curltime start_time;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା io_need;
  } renegotiate_state;
  BIT(recv_sspi_close_notify); /* true if connection closed by close_notify */
  BIT(recv_connection_closed); /* true if connection closed, regardless how */
  BIT(recv_renegotiating);     /* true if recv is doing renegotiation */
  BIT(use_alpn); /* true if ALPN is used for this connection */
  BIT(use_manual_cred_validation); /* true if manual cred validation is used */
  BIT(sent_shutdown);
  BIT(shutdown_generated);
  /* encdata_is_incomplete: if encdata contains only a partial record that
     cannot be decrypted without another recv() (that is, status is
     SEC_E_INCOMPLETE_MESSAGE) then set this true. after recv() adds more
     bytes into encdata then set this back to false. */
  BIT(encdata_is_incomplete);
};

ସଂରଚନା schannel_cert_share {
  unsigned ଅକ୍ଷର CAinfo_blob_digest[CURL_SHA256_DIGEST_LENGTH];
  size_t CAinfo_blob_size;           /* CA info blob size */
  ଅକ୍ଷର *CAfile;                      /* CAfile path used to generate
                                        certificate store */
  HCERTSTORE cert_store;             /* cached certificate store or
                                        NULL if none */
  ସଂରଚନା curltime time;              /* when the cached store was created */
};

/* size of the structure: 20 bytes */
ସଂରଚନା ଅଙ୍କ_ip_data {
  DWORD size; /* 04 bytes */
  ସଂଘ {
    ସଂରଚନା in_addr  ia;  /* 04 bytes */
    ସଂରଚନା in6_addr ia6; /* 16 bytes */
  } bData;
};

HCERTSTORE Curl_schannel_get_cached_cert_store(ସଂରଚନା Curl_cfilter *cf,
                                               ସଂରଚନା Curl_easy *data);

bool Curl_schannel_set_cached_cert_store(ସଂରଚନା Curl_cfilter *cf,
                                         ସଂରଚନା Curl_easy *data,
                                         HCERTSTORE cert_store);

#endif /* USE_SCHANNEL */
#endif /* HEADER_CURL_SCHANNEL_INT_H */
