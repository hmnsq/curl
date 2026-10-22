/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DOH

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curl_addrinfo.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "vdns/doh.h"
#ಅನ್ತರ್ಭಾವ "vdns/httpsrr.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"
#ಅನ್ತರ್ಭಾವ "escape.h"  /* for Curl_hexencode() */
#ಅನ್ತರ್ಭಾವ "urlapi-int.h"

#ವ್ಯಾಖ್ಯಾನಿಸು DNS_CLASS_ಒಳ 0x01

ಸ್ಥಿರ ಶೂನ್ಯ doh_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                      ರಚನೆ Curl_resolv_async *async);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ doh_ಸಂಕೇತ_str[] = {
  "",
  "Bad label",
  "Out of range",
  "Label loop",
  "Too small",
  "Out of memory",
  "RDATA length",
  "Malformat",
  "Bad RCODE",
  "Unexpected TYPE",
  "Unexpected CLASS",
  "No content",
  "Bad ID",
  "Name too long",
  "No such name",
  "Transport failed",
  "Out Of Memory"
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *doh_strerror(DOHcode ಸಂಕೇತ_2)
{
  ಆದರೆ((size_t)ಸಂಕೇತ_2 < CURL_ARRAYSIZE(doh_ಸಂಕೇತ_str))
    ಹಿಂತಿರುಗಿ doh_ಸಂಕೇತ_str[ಸಂಕೇತ_2];
  ಹಿಂತಿರುಗಿ "bad error code";
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *doh_type2name(DNStype dnstype)
{
  ಆಯ್ಕೆ(dnstype) {
  ಸ್ಥಿತಿ CURL_DNS_TYPE_A:
    ಹಿಂತಿರುಗಿ "A";
  ಸ್ಥಿತಿ CURL_DNS_TYPE_AAAA:
    ಹಿಂತಿರುಗಿ "AAAA";
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
  ಸ್ಥಿತಿ CURL_DNS_TYPE_HTTPS:
    ಹಿಂತಿರುಗಿ "HTTPS";
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ "unknown";
  }
}

#ಆದರೆ_ಅಂತ್ಯ /* CURLVERBOSE */

/* @unittest 1655
 */
UNITTEST DOHcode doh_req_encode(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host,
                                DNStype dnstype,
                                ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *dnsp,  /*ಬಫರ್*/
                                size_t len,  /* buffer size */
                                size_t *olen);  /* output length */
UNITTEST DOHcode doh_req_encode(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host,
                                DNStype dnstype,
                                ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *dnsp, /*ಬಫರ್*/
                                size_t len,   /* buffer size */
                                size_t *olen) /* output length */
{
  ಸ್ಥಿರಾಂಕ size_t hostlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(host);
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *orig = dnsp;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostp = host;

  /* The expected output length is 16 bytes more than the length of
   * the QNAME-encoding of the hostname.
   *
   * A valid DNS name may not contain a zero-length label, except at
   * the end. For this reason, a name beginning with a dot, or
   * containing a sequence of two or more consecutive dots, is invalid
   * and cannot be encoded as a QNAME.
   *
   * If the hostname ends with a trailing dot, the corresponding
   * QNAME-encoding is one byte longer than the hostname. If (as is
   * also valid) the hostname is shortened by the omission of the
   * trailing dot, then its QNAME-encoding will be two bytes longer
   * than the hostname.
   *
   * Each [ label, dot ] pair is encoded as [ length, label ],
   * preserving overall length. A final [ label ] without a dot is
   * also encoded as [ length, label ], increasing overall length
   * by one. The encoding is completed by appending a NUL byte,
   * representing the zero-length root label, again increasing
   * the overall length by one.
   */

  size_t expected_len;
  DEBUGASSERT(hostlen);
  expected_len = 12 + 1 + hostlen + 4;
  ಆದರೆ(host[hostlen - 1] != '.')
    expected_len++;

  ಆದರೆ(expected_len > DOH_MAX_DNSREQ_ಗಾತ್ರ)
    ಹಿಂತಿರುಗಿ DOH_DNS_NAME_TOO_LONG;

  ಆದರೆ(len < expected_len)
    ಹಿಂತಿರುಗಿ DOH_TOO_SMALL_ಬಫರ್;

  *dnsp++ = 0; /* 16-bit id */
  *dnsp++ = 0;
  *dnsp++ = 0x01; /* |QR|   Opcode  |AA|TC|RD| Set the RD bit */
  *dnsp++ = '\0'; /* |RA|   Z    |   RCODE   |                */
  *dnsp++ = '\0';
  *dnsp++ = 1;    /* QDCOUNT (number of entries in the question section) */
  *dnsp++ = '\0';
  *dnsp++ = '\0'; /* ANCOUNT */
  *dnsp++ = '\0';
  *dnsp++ = '\0'; /* NSCOUNT */
  *dnsp++ = '\0';
  *dnsp++ = '\0'; /* ARCOUNT */

  /* encode each label and store it in the QNAME */
  ಆಗ(*hostp) {
    size_t labellen;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dot = strchr(hostp, '.');
    ಆದರೆ(dot)
      labellen = dot - hostp;
    ಇಲ್ಲದಿದ್ದರೆ
      labellen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(hostp);
    ಆದರೆ((labellen > 63) || (!labellen)) {
      /* label is too long or too short, error out */
      *olen = 0;
      ಹಿಂತಿರುಗಿ DOH_DNS_BAD_LABEL;
    }
    /* label is non-empty, process it */
    *dnsp++ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)labellen;
    ಸ್ಮರಣೆ_ನಕಲು(dnsp, hostp, labellen);
    dnsp += labellen;
    hostp += labellen;
    /* advance past dot, but only if there is one */
    ಆದರೆ(dot)
      hostp++;
  } /* next label */

  *dnsp++ = 0; /* append zero-length label for root */

  /* There are assigned TYPE codes beyond 255: use range [1..65535] */
  *dnsp++ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(255 & (dnstype >> 8)); /* upper 8-bit TYPE */
  *dnsp++ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(255 & dnstype);        /* lower 8-bit TYPE */

  *dnsp++ = '\0'; /* upper 8-bit CLASS */
  *dnsp++ = DNS_CLASS_ಒಳ; /* IN - "the Internet" */

  *olen = dnsp - orig;

  /* verify that our estimation of length is valid, since
   * this has led to buffer overflows in this function */
  DEBUGASSERT(*olen == expected_len);
  ಹಿಂತಿರುಗಿ DOH_OK;
}

ಸ್ಥಿರ size_t doh_probe_write_cb(ಅಕ್ಷರ *contents, size_t ಗಾತ್ರ_2, size_t nmemb,
                                 ಶೂನ್ಯ *userp)
{
  size_t realsize = ಗಾತ್ರ_2 * nmemb;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = userp;
  ರಚನೆ doh_request *doh_req = Curl_meta_get(ದತ್ತಾಂಶ, CURL_EZM_DOH_PROBE);
  ಆದರೆ(!doh_req)
    ಹಿಂತಿರುಗಿ CURL_WRITEFUNC_ERROR;

  ಆದರೆ(curlx_dyn_addn(&doh_req->resp_body, contents, realsize))
    ಹಿಂತಿರುಗಿ 0;

  ಹಿಂತಿರುಗಿ realsize;
}

ಸ್ಥಿರ ಶೂನ್ಯ doh_probe_ಮುಗಿದ(ರಚನೆ Curl_easy *doh,
                           ರಚನೆ Curl_easy *master, CURLcode ಫಲಿತಾಂಶ);
ಸ್ಥಿರ ಶೂನ್ಯ doh_probe_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *e)
{
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  ಆದರೆ(e) {
    ರಚನೆ doh_request *doh_req = e;
    curl_slist_free_all(doh_req->req_hds);
    curlx_dyn_free(&doh_req->resp_body);
    curlx_free(e);
  }
}

#ವ್ಯಾಖ್ಯಾನಿಸು ERROR_CHECK_SETOPT(x, y)                        \
  ಮಾಡು {                                                  \
    ಫಲಿತಾಂಶ = curl_easy_setopt((CURL *)doh, x, y);       \
    ಆದರೆ(ಫಲಿತಾಂಶ &&                                        \
       ಫಲಿತಾಂಶ != CURLE_NOT_BUILT_ಒಳ &&                  \
       ಫಲಿತಾಂಶ != CURLE_UNKNOWN_OPTION)                  \
      ಹೋಗು error;                                       \
  } ಆಗ(0)

ಸ್ಥಿರ CURLcode doh_probe_ಸತತ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              DNStype dnstype,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url, CURLM *multi,
                              uint32_t resolv_id,
                              uint32_t *pmid)
{
  ರಚನೆ Curl_easy *doh = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  timediff_t timeout_ms;
  ರಚನೆ doh_request *doh_req;
  DOHcode d;
  bool maybe_https = !curl_strnequal(url, STRCONST("http:"));

  *pmid = UINT32_MAX;

  doh_req = curlx_calloc(1, ಗಾತ್ರ(*doh_req));
  ಆದರೆ(!doh_req)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  doh_req->resolv_id = resolv_id;
  doh_req->dnstype = dnstype;
  curlx_dyn_init(&doh_req->resp_body, DYN_DOH_RESPONSE);

  d = doh_req_encode(host, dnstype, doh_req->req_body,
                     ಗಾತ್ರ(doh_req->req_body),
                     &doh_req->req_body_len);
  ಆದರೆ(d) {
    failf(ದತ್ತಾಂಶ, "Failed to encode DoH packet [%d]", (ಪೂರ್ಣಾಂಕ)d);
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು error;
  }

  timeout_ms = Curl_timeleft_ms(ದತ್ತಾಂಶ);
  ಆದರೆ(timeout_ms < 0) {
    ಫಲಿತಾಂಶ = CURLE_OPERATION_TIMEDOUT;
    ಹೋಗು error;
  }

  doh_req->req_hds =
    curl_slist_append(ಖಾಲಿ, "Content-Type: application/dns-message");
  ಆದರೆ(!doh_req->req_hds) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು error;
  }

  /* Curl_open() is the internal version of curl_easy_init() */
  ಫಲಿತಾಂಶ = Curl_open(&doh);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು error;

  /* pass in the struct pointer via a local variable to please coverity and
     the gcc typecheck helpers */
  VERBOSE(doh->state.feat = &Curl_trc_feat_doh);
  ERROR_CHECK_SETOPT(CURLOPT_URL, url);
  ERROR_CHECK_SETOPT(CURLOPT_DEFAULT_PROTOCOL, "https");
  ERROR_CHECK_SETOPT(CURLOPT_WRITEFUNCTION, doh_probe_write_cb);
  ERROR_CHECK_SETOPT(CURLOPT_WRITEDATA, doh);
  ERROR_CHECK_SETOPT(CURLOPT_POSTFIELDS, doh_req->req_body);
  ERROR_CHECK_SETOPT(CURLOPT_POSTFIELDSIZE, (ದೀರ್ಘ)doh_req->req_body_len);
  ERROR_CHECK_SETOPT(CURLOPT_HTTPHEADER, doh_req->req_hds);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTP2
  ಆದರೆ(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_HTTP_ಆವೃತ್ತಿ, CURL_HTTP_ಆವೃತ್ತಿ_2_TLS);
    ERROR_CHECK_SETOPT(CURLOPT_PIPEWAIT, 1L);
  }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DEBUGBUILD
  /* enforce HTTPS if not debug */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
#ಇಲ್ಲದಿದ್ದರೆ
  /* in debug mode, also allow http */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
#ಆದರೆ_ಅಂತ್ಯ
  ERROR_CHECK_SETOPT(CURLOPT_TIMEOUT_MS, (ದೀರ್ಘ)timeout_ms);
  ERROR_CHECK_SETOPT(CURLOPT_SHARE, (CURLSH *)ದತ್ತಾಂಶ->share);
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.err && ದತ್ತಾಂಶ->ಗಣ.err != stderr)
    ERROR_CHECK_SETOPT(CURLOPT_STDERR, ದತ್ತಾಂಶ->ಗಣ.err);
  ಆದರೆ(Curl_trc_ft_is_verbose(ದತ್ತಾಂಶ, &Curl_trc_feat_doh))
    ERROR_CHECK_SETOPT(CURLOPT_VERBOSE, 1L);
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.no_signal)
    ERROR_CHECK_SETOPT(CURLOPT_NOSIGNAL, 1L);
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.fdebug)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGFUNCTION, ದತ್ತಾಂಶ->ಗಣ.fdebug);
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.debugdata)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGDATA, ದತ್ತಾಂಶ->ಗಣ.debugdata);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_RESOLV_ARES
  ಆದರೆ(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_DNS_SERVERS)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_SERVERS,
                       CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_DNS_SERVERS));
  }
  ಆದರೆ(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_DNS_INTERFACE)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_INTERFACE,
                       CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_DNS_INTERFACE));
  }
  ಆದರೆ(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_DNS_LOCAL_IP4)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP4,
                       CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_DNS_LOCAL_IP4));
  }
  ಆದರೆ(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_DNS_LOCAL_IP6)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP6,
                       CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_DNS_LOCAL_IP6));
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYHOST,
                       ದತ್ತಾಂಶ->ಗಣ.doh_verifyhost ? 2L : 0L);
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYPEER,
                       ದತ್ತಾಂಶ->ಗಣ.doh_verifypeer ? 1L : 0L);
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYSTATUS,
                       ದತ್ತಾಂಶ->ಗಣ.doh_verifystatus ? 1L : 0L);

    /* Inherit *some* SSL options from the user's transfer. This is a
       best-guess as to which options are needed for compatibility. #3661

       Note DoH does not inherit the user's proxy server so proxy SSL settings
       have no effect and are not inherited. If that changes then two new
       options should be added to check doh proxy insecure separately,
       CURLOPT_DOH_PROXY_SSL_VERIFYHOST and CURLOPT_DOH_PROXY_SSL_VERIFYPEER.
       */
    doh->ಗಣ.ssl.custom_cafile = ದತ್ತಾಂಶ->ಗಣ.ssl.custom_cafile;
    doh->ಗಣ.ssl.custom_capath = ದತ್ತಾಂಶ->ಗಣ.ssl.custom_capath;
    doh->ಗಣ.ssl.custom_cablob = ದತ್ತಾಂಶ->ಗಣ.ssl.custom_cablob;
    ಆದರೆ(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSL_CAFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO,
                         CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSL_CAFILE));
    }
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.blobs[BLOB_CAINFO]) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO_BLOB, ದತ್ತಾಂಶ->ಗಣ.blobs[BLOB_CAINFO]);
    }
    ಆದರೆ(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSL_CAPATH)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAPATH,
                         CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSL_CAPATH));
    }
    ಆದರೆ(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSL_CRLFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CRLFILE,
                         CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSL_CRLFILE));
    }
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssl.certinfo)
      ERROR_CHECK_SETOPT(CURLOPT_CERTINFO, 1L);
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssl_fsslctx)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_FUNCTION, ದತ್ತಾಂಶ->ಗಣ.ssl_fsslctx);
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssl_fsslctxp)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->ಗಣ.ssl_fsslctxp);
    ಆದರೆ(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSL_EC_CURVES)) {
      ERROR_CHECK_SETOPT(CURLOPT_SSL_EC_CURVES,
                         CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSL_EC_CURVES));
    }
    (ಶೂನ್ಯ)curl_easy_setopt(doh, CURLOPT_SSL_OPTIONS,
                           ((ದೀರ್ಘ)ದತ್ತಾಂಶ->ಗಣ.ssl.ssl_options &
                            ~CURLSSLOPT_AUTO_CLIENT_CERT));
  }

  doh->state.ಆಂತರಿಕ = TRUE;
  doh->master_mid = ದತ್ತಾಂಶ->mid; /* master transfer of this one */
  doh->sub_xfer_ಮುಗಿದ = doh_probe_ಮುಗಿದ;

  ಫಲಿತಾಂಶ = Curl_meta_ಗಣ(doh, CURL_EZM_DOH_PROBE, doh_req, doh_probe_dtor);
  doh_req = ಖಾಲಿ; /* call took ownership */
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು error;

  /* DoH handles must not inherit private_data. The handles may be passed to
     the user via callbacks and the user will be able to identify them as
     internal handles because private data is not set. The user can then set
     private_data via CURLOPT_PRIVATE if they so choose. */
  DEBUGASSERT(!doh->ಗಣ.ಖಾಸಗಿ_ದತ್ತಾಂಶ);

  ಆದರೆ(Curl_multi_ಕೂಡಿಸು_handle(multi, doh))
    ಹೋಗು error;

  *pmid = doh->mid;
  ಹಿಂತಿರುಗಿ CURLE_OK;

error:
  Curl_close(&doh);
  ಆದರೆ(doh_req)
    doh_probe_dtor(ಖಾಲಿ, 0, doh_req);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Curl_doh() starts a name resolve using DoH. It resolves a name and returns
 * a 'Curl_addrinfo *' with the address information.
 */

CURLcode Curl_doh(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                  ರಚನೆ Curl_resolv_async *async)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ doh_probes *dohp = ಖಾಲಿ;
  size_t i;

  DEBUGASSERT(!async->doh);
  DEBUGASSERT(async->peer->hostname[0]);
  ಆದರೆ(async->doh) {
    DEBUGASSERT(0); /* should not happen */
    Curl_doh_cleanup(ದತ್ತಾಂಶ, async);
  }

  ಆದರೆ(!async->dns_queries)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
  ಆದರೆ(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     (async->dns_queries & CURL_DNSQ_HTTPS)) {
    /* Can't mix those in the same async resolve */
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  }
#ಇಲ್ಲದಿದ್ದರೆ
  ಆದರೆ(async->dns_queries & CURL_DNSQ_HTTPS) {
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
  }
#ಆದರೆ_ಅಂತ್ಯ

  /* start clean, consider allocating this struct on demand */
  async->doh = dohp = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ doh_probes));
  ಆದರೆ(!dohp)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಗಾಗಿ(i = 0; i < DOH_ಸ್ಥಾನ_ಎಣಿಕೆ; ++i) {
    dohp->probe_rc[i] = DOH_OK;
    dohp->probe_mid[i] = UINT32_MAX;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  /* AAAA results have preference in happy eyeballing, trigger first */
  ಆದರೆ(async->dns_queries & CURL_DNSQ_AAAA) {
    /* create IPv6 DoH request */
    ಫಲಿತಾಂಶ = doh_probe_ಸತತ(ದತ್ತಾಂಶ, CURL_DNS_TYPE_AAAA,
                           async->peer->hostname,
                           CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_DOH),
                           ದತ್ತಾಂಶ->multi, async->id,
                           &dohp->probe_mid[DOH_ಸ್ಥಾನ_IPV6]);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು error;
    async->queries_ongoing++;
  }
#ಆದರೆ_ಅಂತ್ಯ

  /* create IPv4 DoH request */
  ಆದರೆ(async->dns_queries & CURL_DNSQ_A) {
    ಫಲಿತಾಂಶ = doh_probe_ಸತತ(ದತ್ತಾಂಶ, CURL_DNS_TYPE_A,
                           async->peer->hostname,
                           CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_DOH),
                           ದತ್ತಾಂಶ->multi, async->id,
                           &dohp->probe_mid[DOH_ಸ್ಥಾನ_IPV4]);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು error;
    async->queries_ongoing++;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
  ಆದರೆ(async->dns_queries & CURL_DNSQ_HTTPS) {
    ಅಕ್ಷರ *qname = ಖಾಲಿ;
    ಆದರೆ(async->peer->ಪೋರ್ಟ್ != ಪೋರ್ಟ್_HTTPS) {
      qname = curl_maprintf("_%u._https.%s",
                            async->peer->ಪೋರ್ಟ್, async->peer->hostname);
      ಆದರೆ(!qname)
        ಹೋಗು error;
    }
    ಫಲಿತಾಂಶ = doh_probe_ಸತತ(ದತ್ತಾಂಶ, CURL_DNS_TYPE_HTTPS,
                           qname ? qname : async->peer->hostname,
                           CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_DOH), ದತ್ತಾಂಶ->multi,
                           async->id,
                           &dohp->probe_mid[DOH_ಸ್ಥಾನ_HTTPS_RR]);
    curlx_free(qname);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು error;
    async->queries_ongoing++;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ CURLE_OK;

error:
  Curl_doh_cleanup(ದತ್ತಾಂಶ, async);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ DOHcode doh_skipqname(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *doh, size_t dohlen,
                             ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *indexp)
{
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಉದ್ದಳತೆ;
  ಮಾಡು {
    ಆದರೆ(dohlen < (*indexp + 1))
      ಹಿಂತಿರುಗಿ DOH_DNS_ಹೊರಹೋಗು_ದ_RANGE;
    ಉದ್ದಳತೆ = doh[*indexp];
    ಆದರೆ((ಉದ್ದಳತೆ & 0xc0) == 0xc0) {
      /* name pointer, advance over it and be done */
      ಆದರೆ(dohlen < (*indexp + 2))
        ಹಿಂತಿರುಗಿ DOH_DNS_ಹೊರಹೋಗು_ದ_RANGE;
      *indexp += 2;
      ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ(ಉದ್ದಳತೆ & 0xc0)
      ಹಿಂತಿರುಗಿ DOH_DNS_BAD_LABEL;
    ಆದರೆ(dohlen < (*indexp + 1 + ಉದ್ದಳತೆ))
      ಹಿಂತಿರುಗಿ DOH_DNS_ಹೊರಹೋಗು_ದ_RANGE;
    *indexp += (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(1 + ಉದ್ದಳತೆ);
  } ಆಗ(ಉದ್ದಳತೆ);
  ಹಿಂತಿರುಗಿ DOH_OK;
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ doh_get16bit(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *doh,
                                   ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ)
{
  ಹಿಂತಿರುಗಿ (ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)((doh[ಸೂಚ್ಯಂಕ] << 8) | doh[ಸೂಚ್ಯಂಕ + 1]);
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ doh_get32bit(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *doh, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ)
{
  /* make clang and gcc optimize this to bswap by incrementing
     the pointer first. */
  doh += ಸೂಚ್ಯಂಕ;

  /* avoid undefined behavior by casting to unsigned before shifting
     24 bits, possibly into the sign bit. codegen is same, but
     ub sanitizer will not be upset */
  ಹಿಂತಿರುಗಿ ((ಚಿಹ್ನೆರಹಿತ)doh[0] << 24) | ((ಚಿಹ್ನೆರಹಿತ)doh[1] << 16) |
         ((ಚಿಹ್ನೆರಹಿತ)doh[2] << 8) | doh[3];
}

ಸ್ಥಿರ ಶೂನ್ಯ doh_store_a(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *doh, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ,
                        ರಚನೆ dohentry *d)
{
  /* silently ignore addresses over the limit */
  ಆದರೆ(d->numaddr < DOH_MAX_ADDR) {
    ರಚನೆ dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_A;
    ಸ್ಮರಣೆ_ನಕಲು(&a->ip.v4, &doh[ಸೂಚ್ಯಂಕ], 4);
    d->numaddr++;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ doh_store_aaaa(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *doh, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ,
                           ರಚನೆ dohentry *d)
{
  /* silently ignore addresses over the limit */
  ಆದರೆ(d->numaddr < DOH_MAX_ADDR) {
    ರಚನೆ dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_AAAA;
    ಸ್ಮರಣೆ_ನಕಲು(&a->ip.v6, &doh[ಸೂಚ್ಯಂಕ], 16);
    d->numaddr++;
  }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
ಸ್ಥಿರ DOHcode doh_store_https(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *doh, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ,
                               ರಚನೆ dohentry *d, uint16_t len)
{
  /* silently ignore RRs over the limit */
  ಆದರೆ(d->numhttps_rrs < DOH_MAX_HTTPS) {
    ರಚನೆ dohhttps_rr *h = &d->https_rrs[d->numhttps_rrs];
    h->val = curlx_memdup(&doh[ಸೂಚ್ಯಂಕ], len);
    ಆದರೆ(!h->val)
      ಹಿಂತಿರುಗಿ DOH_ಹೊರಹೋಗು_ದ_MEM;
    h->len = len;
    d->numhttps_rrs++;
  }
  ಹಿಂತಿರುಗಿ DOH_OK;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ DOHcode doh_rdata(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *doh,
                         ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ rdlength,
                         ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ type,
                         ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ,
                         ರಚನೆ dohentry *d)
{
  /* RDATA
     - A (TYPE 1): 4 bytes
     - AAAA (TYPE 28): 16 bytes
     - HTTPS (TYPE 65): N bytes */
  ಆಯ್ಕೆ(type) {
  ಸ್ಥಿತಿ CURL_DNS_TYPE_A:
    ಆದರೆ(rdlength != 4)
      ಹಿಂತಿರುಗಿ DOH_DNS_RDATA_LEN;
    doh_store_a(doh, ಸೂಚ್ಯಂಕ, d);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_DNS_TYPE_AAAA:
    ಆದರೆ(rdlength != 16)
      ಹಿಂತಿರುಗಿ DOH_DNS_RDATA_LEN;
    doh_store_aaaa(doh, ಸೂಚ್ಯಂಕ, d);
    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
  ಸ್ಥಿತಿ CURL_DNS_TYPE_HTTPS:
    ಆದರೆ(rdlength < 3)
      ಹಿಂತಿರುಗಿ DOH_DNS_RDATA_LEN;
    ಹಿಂತಿರುಗಿ doh_store_https(doh, ಸೂಚ್ಯಂಕ, d, rdlength);
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ವನಿಯೋಜಿತ:
    /* unsupported type, or type we do not store, skip it */
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ DOH_OK;
}

/* @unittest 1655 */
UNITTEST ಶೂನ್ಯ de_init(ರಚನೆ dohentry *de);
UNITTEST ಶೂನ್ಯ de_init(ರಚನೆ dohentry *de)
{
  memset(de, 0, ಗಾತ್ರ(*de));
  de->ttl = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
}

/* TTL value cap */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_DNS_TTL 86400U /* 24 hours */
/* @unittest 1650 */
UNITTEST DOHcode doh_resp_decode(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 ರಚನೆ dohentry *d);
UNITTEST DOHcode doh_resp_decode(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 ರಚನೆ dohentry *d)
{
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ rcode;
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ qdcount;
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ ancount;
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ type = 0;
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ rdlength;
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ nscount;
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ arcount;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ = 12;
  DOHcode rc;

  ಆದರೆ(dohlen < 12)
    ಹಿಂತಿರುಗಿ DOH_TOO_SMALL_ಬಫರ್; /* too small */
  ಆದರೆ(!doh || doh[0] || doh[1])
    ಹಿಂತಿರುಗಿ DOH_DNS_BAD_ID; /* bad ID */
  rcode = doh[3] & 0x0f;
  ಆದರೆ(rcode == 3)
    ಹಿಂತಿರುಗಿ DOH_DNS_NXDOMAIN; /* name does not exist */
  ಆದರೆ(rcode)
    ಹಿಂತಿರುಗಿ DOH_DNS_BAD_RCODE; /* bad rcode */

  qdcount = doh_get16bit(doh, 4);
  ಆಗ(qdcount) {
    rc = doh_skipqname(doh, dohlen, &ಸೂಚ್ಯಂಕ);
    ಆದರೆ(rc)
      ಹಿಂತಿರುಗಿ rc; /* bad qname */
    ಆದರೆ(dohlen < (ಸೂಚ್ಯಂಕ + 4))
      ಹಿಂತಿರುಗಿ DOH_DNS_ಹೊರಹೋಗು_ದ_RANGE;
    ಸೂಚ್ಯಂಕ += 4; /* skip question's type and class */
    qdcount--;
  }

  ancount = doh_get16bit(doh, 6);
  ಆಗ(ancount) {
    ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ dnsclass;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ttl;

    rc = doh_skipqname(doh, dohlen, &ಸೂಚ್ಯಂಕ);
    ಆದರೆ(rc)
      ಹಿಂತಿರುಗಿ rc; /* bad qname */

    ಆದರೆ(dohlen < (ಸೂಚ್ಯಂಕ + 2))
      ಹಿಂತಿರುಗಿ DOH_DNS_ಹೊರಹೋಗು_ದ_RANGE;

    type = doh_get16bit(doh, ಸೂಚ್ಯಂಕ);
    ಆದರೆ((type != CURL_DNS_TYPE_CNAME) &&  /* may be synthesized from DNAME */
       (type != CURL_DNS_TYPE_DNAME) &&  /* if present, accept and ignore */
       (type != dnstype))
      /* Not the same type as was asked for, nor CNAME nor DNAME */
      ಹಿಂತಿರುಗಿ DOH_DNS_UNEXPECTED_TYPE;
    ಸೂಚ್ಯಂಕ += 2;

    ಆದರೆ(dohlen < (ಸೂಚ್ಯಂಕ + 2))
      ಹಿಂತಿರುಗಿ DOH_DNS_ಹೊರಹೋಗು_ದ_RANGE;
    dnsclass = doh_get16bit(doh, ಸೂಚ್ಯಂಕ);
    ಆದರೆ(DNS_CLASS_ಒಳ != dnsclass)
      ಹಿಂತಿರುಗಿ DOH_DNS_UNEXPECTED_CLASS; /* unsupported */
    ಸೂಚ್ಯಂಕ += 2;

    ಆದರೆ(dohlen < (ಸೂಚ್ಯಂಕ + 4))
      ಹಿಂತಿರುಗಿ DOH_DNS_ಹೊರಹೋಗು_ದ_RANGE;

    ttl = doh_get32bit(doh, ಸೂಚ್ಯಂಕ);
    ಆದರೆ(ttl > MAX_DNS_TTL)
      ttl = MAX_DNS_TTL;
    ಆದರೆ(ttl < d->ttl)
      d->ttl = ttl;
    ಸೂಚ್ಯಂಕ += 4;

    ಆದರೆ(dohlen < (ಸೂಚ್ಯಂಕ + 2))
      ಹಿಂತಿರುಗಿ DOH_DNS_ಹೊರಹೋಗು_ದ_RANGE;

    rdlength = doh_get16bit(doh, ಸೂಚ್ಯಂಕ);
    ಸೂಚ್ಯಂಕ += 2;
    ಆದರೆ(dohlen < (ಸೂಚ್ಯಂಕ + rdlength))
      ಹಿಂತಿರುಗಿ DOH_DNS_ಹೊರಹೋಗು_ದ_RANGE;

    rc = doh_rdata(doh, rdlength, type, (ಪೂರ್ಣಾಂಕ)ಸೂಚ್ಯಂಕ, d);
    ಆದರೆ(rc)
      ಹಿಂತಿರುಗಿ rc;
    ಸೂಚ್ಯಂಕ += rdlength;
    ancount--;
  }

  nscount = doh_get16bit(doh, 8);
  ಆಗ(nscount) {
    rc = doh_skipqname(doh, dohlen, &ಸೂಚ್ಯಂಕ);
    ಆದರೆ(rc)
      ಹಿಂತಿರುಗಿ rc; /* bad qname */

    ಆದರೆ(dohlen < (ಸೂಚ್ಯಂಕ + 8))
      ಹಿಂತಿರುಗಿ DOH_DNS_ಹೊರಹೋಗು_ದ_RANGE;

    ಸೂಚ್ಯಂಕ += 2 + 2 + 4; /* type, dnsclass and ttl */

    ಆದರೆ(dohlen < (ಸೂಚ್ಯಂಕ + 2))
      ಹಿಂತಿರುಗಿ DOH_DNS_ಹೊರಹೋಗು_ದ_RANGE;

    rdlength = doh_get16bit(doh, ಸೂಚ್ಯಂಕ);
    ಸೂಚ್ಯಂಕ += 2;
    ಆದರೆ(dohlen < (ಸೂಚ್ಯಂಕ + rdlength))
      ಹಿಂತಿರುಗಿ DOH_DNS_ಹೊರಹೋಗು_ದ_RANGE;
    ಸೂಚ್ಯಂಕ += rdlength;
    nscount--;
  }

  arcount = doh_get16bit(doh, 10);
  ಆಗ(arcount) {
    rc = doh_skipqname(doh, dohlen, &ಸೂಚ್ಯಂಕ);
    ಆದರೆ(rc)
      ಹಿಂತಿರುಗಿ rc; /* bad qname */

    ಆದರೆ(dohlen < (ಸೂಚ್ಯಂಕ + 8))
      ಹಿಂತಿರುಗಿ DOH_DNS_ಹೊರಹೋಗು_ದ_RANGE;

    ಸೂಚ್ಯಂಕ += 2 + 2 + 4; /* type, dnsclass and ttl */

    ಆದರೆ(dohlen < (ಸೂಚ್ಯಂಕ + 2))
      ಹಿಂತಿರುಗಿ DOH_DNS_ಹೊರಹೋಗು_ದ_RANGE;

    rdlength = doh_get16bit(doh, ಸೂಚ್ಯಂಕ);
    ಸೂಚ್ಯಂಕ += 2;
    ಆದರೆ(dohlen < (ಸೂಚ್ಯಂಕ + rdlength))
      ಹಿಂತಿರುಗಿ DOH_DNS_ಹೊರಹೋಗು_ದ_RANGE;
    ಸೂಚ್ಯಂಕ += rdlength;
    arcount--;
  }

  ಆದರೆ(ಸೂಚ್ಯಂಕ != dohlen)
    ಹಿಂತಿರುಗಿ DOH_DNS_MALFORMAT; /* something is wrong */

  ಹಿಂತಿರುಗಿ DOH_OK; /* ok */
}

/*
 * This function returns a pointer to the first element of a newly allocated
 * Curl_addrinfo struct linked list filled with the data from a set of DoH
 * lookups. Curl_addrinfo is meant to work like the addrinfo struct does for
 * an IPv6 stack, but usable also for IPv4, all hosts and environments.
 *
 * The memory allocated by this function *MUST* be free'd later on calling
 * Curl_freeaddrinfo(). For each successful call to this function there
 * must be an associated call later to Curl_freeaddrinfo().
 */
ಸ್ಥಿರ CURLcode doh2ai(ಸ್ಥಿರಾಂಕ ರಚನೆ dohentry *de, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname,
                       ಪೂರ್ಣಾಂಕ ಪೋರ್ಟ್, ರಚನೆ Curl_addrinfo **aip)
{
  ರಚನೆ Curl_addrinfo *ai;
  ರಚನೆ Curl_addrinfo *prevai = ಖಾಲಿ;
  ರಚನೆ Curl_addrinfo *firstai = ಖಾಲಿ;
  ರಚನೆ sockaddr_in *addr;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ರಚನೆ sockaddr_in6 *addr6;
#ಆದರೆ_ಅಂತ್ಯ
  size_t hostlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(hostname) + 1; /* include null-terminator */
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಪೂರ್ಣಾಂಕ i;

  ಗಾಗಿ(i = 0; i < de->numaddr; i++) {
    size_t ss_ಗಾತ್ರ;
    CURL_SA_FAMILY_T addrtype;
    ಆದರೆ(de->addr[i].type == CURL_DNS_TYPE_AAAA) {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_IPV6
      /* we cannot handle IPv6 addresses */
      ಮುಂದುವರಿ;
#ಇಲ್ಲದಿದ್ದರೆ
      ss_ಗಾತ್ರ = ಗಾತ್ರ(ರಚನೆ sockaddr_in6);
      addrtype = AF_INET6;
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ss_ಗಾತ್ರ = ಗಾತ್ರ(ರಚನೆ sockaddr_in);
      addrtype = AF_INET;
    }

    ai = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ Curl_addrinfo) + ss_ಗಾತ್ರ + hostlen);
    ಆದರೆ(!ai) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ನಿಲ್ಲಿಸು;
    }
    ai->ai_addr = (ಶೂನ್ಯ *)((ಅಕ್ಷರ *)ai + ಗಾತ್ರ(ರಚನೆ Curl_addrinfo));
    ai->ai_canonname = (ಶೂನ್ಯ *)((ಅಕ್ಷರ *)ai->ai_addr + ss_ಗಾತ್ರ);
    ಸ್ಮರಣೆ_ನಕಲು(ai->ai_canonname, hostname, hostlen);

    ಆದರೆ(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    ಆದರೆ(prevai)
      /* make the previous entry point to this */
      prevai->ai_ಮುಂದಿನ = ai;

    ai->ai_family = addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_ಗಾತ್ರ;

    /* leave the rest of the struct filled with zero */

    ಆಯ್ಕೆ(ai->ai_family) {
    ಸ್ಥಿತಿ AF_INET:
      addr = (ಶೂನ್ಯ *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(ಗಾತ್ರ(ರಚನೆ in_addr) == ಗಾತ್ರ(de->addr[i].ip.v4));
      ಸ್ಮರಣೆ_ನಕಲು(&addr->sin_addr, &de->addr[i].ip.v4, ಗಾತ್ರ(ರಚನೆ in_addr));
      addr->sin_family = addrtype;
      addr->sin_port = htons((ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)ಪೋರ್ಟ್);
      ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
    ಸ್ಥಿತಿ AF_INET6:
      addr6 = (ಶೂನ್ಯ *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(ಗಾತ್ರ(ರಚನೆ in6_addr) == ಗಾತ್ರ(de->addr[i].ip.v6));
      ಸ್ಮರಣೆ_ನಕಲು(&addr6->sin6_addr, &de->addr[i].ip.v6, ಗಾತ್ರ(ರಚನೆ in6_addr));
      addr6->sin6_family = addrtype;
      addr6->sin6_port = htons((ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)ಪೋರ್ಟ್);
      ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
    }

    prevai = ai;
  }

  ಆದರೆ(ಫಲಿತಾಂಶ) {
    Curl_freeaddrinfo(firstai);
    firstai = ಖಾಲಿ;
  }
  *aip = firstai;

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* @unittest 1655 */
UNITTEST ಶೂನ್ಯ de_cleanup(ರಚನೆ dohentry *d);
UNITTEST ಶೂನ್ಯ de_cleanup(ರಚನೆ dohentry *d)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
  ಪೂರ್ಣಾಂಕ i = 0;
  ಗಾಗಿ(i = 0; i < d->numhttps_rrs; i++)
    curlx_safefree(d->https_rrs[i].val);
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)d;
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR

/*
 * @brief decode the DNS name in a binary RRData
 * @param buf points to the buffer (in/out)
 * @param remaining points to the remaining buffer length (in/out)
 * @param dnsname returns the string form name on success
 * @return is 1 for success, error otherwise
 *
 * The encoding here is defined in
 * https://datatracker.ietf.org/doc/html/rfc1035#section-3.1
 *
 * The input buffer pointer will be modified so it points to after the end of
 * the DNS name encoding on output. (that is why it is an "unsigned char
 * **" :-)
 */
ಸ್ಥಿರ CURLcode doh_decode_rdata_name(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ **buf,
                                      size_t *ಉಳಿದ, ಅಕ್ಷರ **dnsname)
{
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *cp = ಖಾಲಿ;
  size_t rem = 0;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ clen = 0; /* chunk len */
  ರಚನೆ dynbuf thename;

  DEBUGASSERT(buf && ಉಳಿದ && dnsname);
  ಆದರೆ(!buf || !ಉಳಿದ || !dnsname || !*ಉಳಿದ)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  curlx_dyn_init(&thename, CURL_MAXLEN_HOST_NAME);
  rem = *ಉಳಿದ;
  cp = *buf;
  clen = *cp++;
  /* RFC 9460 says it must be uncompressed */
  ಆದರೆ(clen > 63)
    ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;

  ಆದರೆ(clen == 0) {
    /* special case - return "." as name */
    ಆದರೆ(curlx_dyn_addn(&thename, ".", 1))
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
  ಆಗ(clen) {
    ಆದರೆ(clen >= rem) {
      curlx_dyn_free(&thename);
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
    ಆದರೆ(curlx_dyn_addn(&thename, cp, clen) ||
       curlx_dyn_addn(&thename, ".", 1))
      ಹಿಂತಿರುಗಿ CURLE_TOO_LARGE;

    cp += clen;
    rem -= (clen + 1);
    ಆದರೆ(rem <= 0) {
      curlx_dyn_free(&thename);
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
    clen = *cp++;
    ಆದರೆ(clen > 63) {
      /* invalid format */
      curlx_dyn_free(&thename);
      ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
    }
  }
  *buf = cp;
  *ಉಳಿದ = rem - 1;
  *dnsname = curlx_dyn_ptr(&thename);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* scan for byte values <= 32 or 127 */
ಸ್ಥಿರ CURLcode junkscan(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url)
{
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *p = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)url;
  ಆಗ(*p) {
    ಆದರೆ(*p <= 0x20 || *p == 127)
      ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
    p++;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* @unittest 1658 */
UNITTEST CURLcode doh_resp_decode_httpsrr(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *cp, size_t len,
                                          ರಚನೆ Curl_https_rrinfo **hrr);
UNITTEST CURLcode doh_resp_decode_httpsrr(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *cp, size_t len,
                                          ರಚನೆ Curl_https_rrinfo **hrr)
{
  uint16_t pcode = 0, plen = 0;
  uint32_t expected_min_pcode = 0;
  ರಚನೆ Curl_https_rrinfo *lhrr = ಖಾಲಿ;
  ಅಕ್ಷರ *dnsname = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  *hrr = ಖಾಲಿ;
  ಆದರೆ(len <= 2)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  lhrr = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ Curl_https_rrinfo));
  ಆದರೆ(!lhrr)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  lhrr->ಆದ್ಯತೆ = doh_get16bit(cp, 0);
  cp += 2;
  len -= 2;
  ಆದರೆ(doh_decode_rdata_name(&cp, &len, &dnsname) != CURLE_OK)
    ಹೋಗು err;
  lhrr->ಗುರಿ = dnsname;
  ಫಲಿತಾಂಶ = junkscan(dnsname);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    /* unacceptable hostname content */
    ಹೋಗು err;
  }
  ಆಗ(len >= 4) {
    pcode = doh_get16bit(cp, 0);
    plen = doh_get16bit(cp, 2);
    cp += 4;
    len -= 4;
    ಆದರೆ(pcode < expected_min_pcode || plen > len) {
      ಫಲಿತಾಂಶ = CURLE_WEIRD_SERVER_ಉತ್ತರ;
      ಹೋಗು err;
    }
    ಫಲಿತಾಂಶ = Curl_httpsrr_ಗಣ(lhrr, pcode, cp, plen);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು err;
    Curl_httpsrr_trace(ದತ್ತಾಂಶ, lhrr);
    cp += plen;
    len -= plen;
    expected_min_pcode = pcode + 1;
  }
  *hrr = lhrr;
  ಹಿಂತಿರುಗಿ CURLE_OK;
err:
  Curl_httpsrr_destroy(lhrr);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_HTTPSRR */

/* called from multi when a sub transfer, e.g. doh probe, is done.
 * Parse the response and set the results in the `async` context
 * of master, using the id from the probe's CURL_EZM_DOH_PROBE
 * meta data. */
ಸ್ಥಿರ ಶೂನ್ಯ doh_probe_ಮುಗಿದ(ರಚನೆ Curl_easy *doh,
                           ರಚನೆ Curl_easy *master, CURLcode ಫಲಿತಾಂಶ)
{
  ರಚನೆ Curl_resolv_async *async = ಖಾಲಿ;
  ರಚನೆ doh_probes *dohp = ಖಾಲಿ;
  ರಚನೆ doh_request *doh_req = ಖಾಲಿ;
  ರಚನೆ Curl_addrinfo **pdest_ai;
  ರಚನೆ dohentry de;
  ಪೂರ್ಣಾಂಕ ಸ್ಥಾನ, httpcode;

  de_init(&de);
  doh_req = Curl_meta_get(doh, CURL_EZM_DOH_PROBE);
  ಆದರೆ(!doh_req) {
    /* transfer `doh` is not a DoH probe. */
    DEBUGASSERT(0);
    ಹೋಗು ಹೊರಹೋಗು;
  }

  async = Curl_async_get(master, doh_req->resolv_id);
  ಆದರೆ(!async) {
    CURL_TRC_DNS(master, "[%u] ignoring outdated DoH response",
                 doh_req->resolv_id);
    ಹೋಗು ಹೊರಹೋಗು;
  }
  dohp = async->doh;

  ಗಾಗಿ(ಸ್ಥಾನ = 0; ಸ್ಥಾನ < DOH_ಸ್ಥಾನ_ಎಣಿಕೆ; ++ಸ್ಥಾನ) {
    ಆದರೆ(dohp->probe_mid[ಸ್ಥಾನ] == doh->mid)
      ನಿಲ್ಲಿಸು;
  }
  /* We really should have found the slot where to store the response */
  ಆದರೆ(ಸ್ಥಾನ >= DOH_ಸ್ಥಾನ_ಎಣಿಕೆ) {
    failf(master, "DoH: unknown sub request done");
    DEBUGASSERT(0);
    ಹೋಗು ಹೊರಹೋಗು;
  }

  async->queries_ongoing--;
  dohp = async->doh;
  httpcode = doh->info.httpcode;
  ಆಯ್ಕೆ(ಸ್ಥಾನ) {
  ಸ್ಥಿತಿ DOH_ಸ್ಥಾನ_IPV4:
    async->dns_responses |= CURL_DNSQ_A;
    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ಸ್ಥಿತಿ DOH_ಸ್ಥಾನ_IPV6:
    async->dns_responses |= CURL_DNSQ_AAAA;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
  ಸ್ಥಿತಿ DOH_ಸ್ಥಾನ_HTTPS_RR:
    async->dns_responses |= CURL_DNSQ_HTTPS;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ವನಿಯೋಜಿತ:
    DEBUGASSERT(0);
    ನಿಲ್ಲಿಸು;
  }

  ಆದರೆ(ಫಲಿತಾಂಶ) {
    dohp->probe_rc[ಸ್ಥಾನ] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: %s",
          doh_type2name(doh_req->dnstype), curl_easy_strerror(ಫಲಿತಾಂಶ));
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((httpcode < 200) || (httpcode >= 300)) {
    dohp->probe_rc[ಸ್ಥಾನ] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: HTTP status %d",
          doh_type2name(doh_req->dnstype), httpcode);
    ಹೋಗು ಹೊರಹೋಗು;
  }

  dohp->probe_rc[ಸ್ಥಾನ] = doh_resp_decode(curlx_dyn_uptr(&doh_req->resp_body),
                                         curlx_dyn_len(&doh_req->resp_body),
                                         doh_req->dnstype, &de);
  ಆದರೆ(dohp->probe_rc[ಸ್ಥಾನ]) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
    ಆದರೆ((dohp->probe_rc[ಸ್ಥಾನ] == DOH_NO_CONTENT) &&
       (doh_req->dnstype == CURL_DNS_TYPE_HTTPS)) {
      dohp->probe_rc[ಸ್ಥಾನ] = DOH_DNS_NXDOMAIN;
    }
#ಆದರೆ_ಅಂತ್ಯ
    infof(doh, "[DoH] [%s] error decoding response: %s",
          doh_type2name(doh_req->dnstype),
          doh_strerror(dohp->probe_rc[ಸ್ಥಾನ]));
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(doh_req->dnstype == CURL_DNS_TYPE_A)
    pdest_ai = &async->ai_A;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(doh_req->dnstype == CURL_DNS_TYPE_AAAA)
    pdest_ai = &async->ai_AAAA;
  ಇಲ್ಲದಿದ್ದರೆ
    pdest_ai = ಖಾಲಿ;

  ಆದರೆ(pdest_ai && de.numaddr) {
    ಆದರೆ(*pdest_ai) {
      Curl_freeaddrinfo(*pdest_ai);
      *pdest_ai = ಖಾಲಿ;
    }
    ಫಲಿತಾಂಶ = doh2ai(&de, async->peer->hostname, async->peer->ಪೋರ್ಟ್, pdest_ai);
    ಆದರೆ(ಫಲಿತಾಂಶ) { /* hard failure on our side, fail completely */
      infof(doh, "[DoH] [%s] error creating addrinfo: %s",
            doh_type2name(doh_req->dnstype), curl_easy_strerror(ಫಲಿತಾಂಶ));
      dohp->probe_rc[ಸ್ಥಾನ] = DOH_OOM;
      async->ಫಲಿತಾಂಶ = ಫಲಿತಾಂಶ;
    }
  }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((doh_req->dnstype == CURL_DNS_TYPE_HTTPS) && de.numhttps_rrs) {
    CURL_TRC_DNS(doh, "[HTTPS] got %d records", de.numhttps_rrs);
    ಫಲಿತಾಂಶ = doh_resp_decode_httpsrr(doh, de.https_rrs->val,
                                     de.https_rrs->len, &async->httpsrr);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      dohp->probe_rc[ಸ್ಥಾನ] = DOH_HTTP_FAILED;
      infof(doh, "[DoH] error decoding HTTPS RR: %s",
            curl_easy_strerror(ಫಲಿತಾಂಶ));
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }
#ಆದರೆ_ಅಂತ್ಯ /* USE_HTTPSRR */

  /* DoH request complete, run master to act on results */
  infof(doh, "DoH request complete, %u to go", async->queries_ongoing);

ಹೊರಹೋಗು:
  Curl_multi_mark_dirty(master);
  de_cleanup(&de);
  Curl_meta_remove(doh, CURL_EZM_DOH_PROBE);
}

CURLcode Curl_doh_take_ಫಲಿತಾಂಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ Curl_resolv_async *async,
                              ರಚನೆ Curl_dns_ನಮೂದು **pdns)
{
  ರಚನೆ doh_probes *dohp = async->doh;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  *pdns = ಖಾಲಿ; /* defaults to no response */
  ಆದರೆ(!dohp)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  async->negative_answer = FALSE;
  ಆದರೆ(async->ಫಲಿತಾಂಶ) {
    ಫಲಿತಾಂಶ = async->ಫಲಿತಾಂಶ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     dohp->probe_mid[DOH_ಸ್ಥಾನ_IPV4] == UINT32_MAX &&
     dohp->probe_mid[DOH_ಸ್ಥಾನ_IPV6] == UINT32_MAX) {
    failf(ದತ್ತಾಂಶ, "Could not DoH-resolve: %s", async->peer->hostname);
    ಹಿಂತಿರುಗಿ async->for_proxy ?
      CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!async->queries_ongoing) {
    ರಚನೆ Curl_dns_ನಮೂದು *dns = ಖಾಲಿ;
    bool negative = TRUE;
    ಪೂರ್ಣಾಂಕ ಸ್ಥಾನ;

    /* remove DoH handles from multi handle and close them */
    doh_close(ದತ್ತಾಂಶ, async);
    /* parse the responses, create the struct and return it! */
    ಗಾಗಿ(ಸ್ಥಾನ = 0; ಸ್ಥಾನ < DOH_ಸ್ಥಾನ_ಎಣಿಕೆ; ಸ್ಥಾನ++) {
      /* Failing without an NXDOMAIN answer - a SERVFAIL-class rcode or
         an undecodable response - says nothing about the name. Such a
         failure must not be cached as a negative entry. */
      ಆದರೆ(dohp->probe_rc[ಸ್ಥಾನ] && (dohp->probe_rc[ಸ್ಥಾನ] != DOH_DNS_NXDOMAIN))
        negative = FALSE;
    } /* next slot */

    ಆದರೆ(async->ai_A || async->ai_AAAA) {
      dns = Curl_dnsc_mk_addr2(
        ದತ್ತಾಂಶ, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
      ಆದರೆ(!dns) {
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((async->dns_queries & CURL_DNSQ_HTTPS) &&
            !dohp->probe_rc[DOH_ಸ್ಥಾನ_HTTPS_RR]) {
      Curl_httpsrr_trace(ದತ್ತಾಂಶ, async->httpsrr);
      dns = Curl_dnsc_mk_https(ದತ್ತಾಂಶ, &async->httpsrr, async->peer);
      ಆದರೆ(!dns) {
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }
#ಆದರೆ_ಅಂತ್ಯ /* USE_HTTPSRR */
    ಇಲ್ಲದಿದ್ದರೆ {
      /* every query failed. Only NXDOMAIN answers for all of them
         make this a negative answer, eligible for caching. */
      async->negative_answer = negative;
      ಫಲಿತಾಂಶ = async->for_proxy ?
        CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
    }

    /* and add the entry to the cache */
    ಆದರೆ(dns)
      ಫಲಿತಾಂಶ = Curl_dnscache_ಕೂಡಿಸು(ದತ್ತಾಂಶ, dns);
    *pdns = dns;
  } /* !async->queries_ongoing */
  ಇಲ್ಲದಿದ್ದರೆ
    /* wait for pending DoH transactions to complete */
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;

ಹೊರಹೋಗು:
  Curl_doh_cleanup(ದತ್ತಾಂಶ, async);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ doh_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                      ರಚನೆ Curl_resolv_async *async)
{
  ರಚನೆ doh_probes *doh = async ? async->doh : ಖಾಲಿ;
  ಆದರೆ(doh && ದತ್ತಾಂಶ->multi) {
    ರಚನೆ Curl_easy *probe_ದತ್ತಾಂಶ;
    uint32_t mid;
    size_t ಸ್ಥಾನ;
    ಗಾಗಿ(ಸ್ಥಾನ = 0; ಸ್ಥಾನ < DOH_ಸ್ಥಾನ_ಎಣಿಕೆ; ಸ್ಥಾನ++) {
      mid = doh->probe_mid[ಸ್ಥಾನ];
      ಆದರೆ(mid == UINT32_MAX)
        ಮುಂದುವರಿ;
      doh->probe_mid[ಸ್ಥಾನ] = UINT32_MAX;
      /* should have been called before data is removed from multi handle */
      DEBUGASSERT(ದತ್ತಾಂಶ->multi);
      probe_ದತ್ತಾಂಶ = ದತ್ತಾಂಶ->multi ? Curl_multi_get_easy(ದತ್ತಾಂಶ->multi, mid) : ಖಾಲಿ;
      ಆದರೆ(!probe_ದತ್ತಾಂಶ) {
        DEBUGF(infof(ದತ್ತಾಂಶ, "Curl_doh_close: xfer for mid=%u not found!", mid));
        ಮುಂದುವರಿ;
      }
      probe_ದತ್ತಾಂಶ->sub_xfer_ಮುಗಿದ = ಖಾಲಿ; /* No longer interested in result */
      /* data->multi might already be reset at this time */
      Curl_multi_remove_handle(ದತ್ತಾಂಶ->multi, probe_ದತ್ತಾಂಶ);
      Curl_close(&probe_ದತ್ತಾಂಶ);
    }
    CURL_TRC_DNS(ದತ್ತಾಂಶ, "[DoH] probe done");
  }
}

ಶೂನ್ಯ Curl_doh_cleanup(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                      ರಚನೆ Curl_resolv_async *async)
{
  ರಚನೆ doh_probes *dohp = async->doh;
  ಆದರೆ(dohp) {
    doh_close(ದತ್ತಾಂಶ, async);
    curlx_safefree(async->doh);
  }
}

#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_DOH */
