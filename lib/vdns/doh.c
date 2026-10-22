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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DOH

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_addrinfo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/doh.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/httpsrr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/dynbuf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "escape.h"  /* for Curl_hexencode() */
#ਅਨ੍ਤਰ੍ਭਾਵ "urlapi-int.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DNS_CLASS_ਵਿੱਚ 0x01

ਸਥਿਰ ਖਾਲੀ doh_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                      ਬਣਤਰ Curl_resolv_async *async);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ doh_ਕੋਡ_str[] = {
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

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *doh_strerror(DOHcode ਕੋਡ)
{
  ਜੇ((size_t)ਕੋਡ < CURL_ARRAYSIZE(doh_ਕੋਡ_str))
    ਵਾਪਸ doh_ਕੋਡ_str[ਕੋਡ];
  ਵਾਪਸ "bad error code";
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *doh_type2name(DNStype dnstype)
{
  ਚੋਣ(dnstype) {
  ਹਾਲਤ CURL_DNS_TYPE_A:
    ਵਾਪਸ "A";
  ਹਾਲਤ CURL_DNS_TYPE_AAAA:
    ਵਾਪਸ "AAAA";
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
  ਹਾਲਤ CURL_DNS_TYPE_HTTPS:
    ਵਾਪਸ "HTTPS";
#ਜੇ_ਅੰਤ
  ਮੂਲ:
    ਵਾਪਸ "unknown";
  }
}

#ਜੇ_ਅੰਤ /* CURLVERBOSE */

/* @unittest 1655
 */
UNITTEST DOHcode doh_req_encode(ਅਚਲ ਅੱਖਰ *host,
                                DNStype dnstype,
                                ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *dnsp,  /*ਬਫਰ*/
                                size_t len,  /* buffer size */
                                size_t *olen);  /* output length */
UNITTEST DOHcode doh_req_encode(ਅਚਲ ਅੱਖਰ *host,
                                DNStype dnstype,
                                ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *dnsp, /*ਬਫਰ*/
                                size_t len,   /* buffer size */
                                size_t *olen) /* output length */
{
  ਅਚਲ size_t hostlen = ਲੜੀ_ਲੰਬਾਈ(host);
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *orig = dnsp;
  ਅਚਲ ਅੱਖਰ *hostp = host;

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
  ਜੇ(host[hostlen - 1] != '.')
    expected_len++;

  ਜੇ(expected_len > DOH_MAX_DNSREQ_ਮਾਪ)
    ਵਾਪਸ DOH_DNS_NAME_TOO_LONG;

  ਜੇ(len < expected_len)
    ਵਾਪਸ DOH_TOO_SMALL_ਬਫਰ;

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
  ਜਦੋਂ(*hostp) {
    size_t labellen;
    ਅਚਲ ਅੱਖਰ *dot = strchr(hostp, '.');
    ਜੇ(dot)
      labellen = dot - hostp;
    ਨਹੀਂਤਾਂ
      labellen = ਲੜੀ_ਲੰਬਾਈ(hostp);
    ਜੇ((labellen > 63) || (!labellen)) {
      /* label is too long or too short, error out */
      *olen = 0;
      ਵਾਪਸ DOH_DNS_BAD_LABEL;
    }
    /* label is non-empty, process it */
    *dnsp++ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)labellen;
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(dnsp, hostp, labellen);
    dnsp += labellen;
    hostp += labellen;
    /* advance past dot, but only if there is one */
    ਜੇ(dot)
      hostp++;
  } /* next label */

  *dnsp++ = 0; /* append zero-length label for root */

  /* There are assigned TYPE codes beyond 255: use range [1..65535] */
  *dnsp++ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(255 & (dnstype >> 8)); /* upper 8-bit TYPE */
  *dnsp++ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(255 & dnstype);        /* lower 8-bit TYPE */

  *dnsp++ = '\0'; /* upper 8-bit CLASS */
  *dnsp++ = DNS_CLASS_ਵਿੱਚ; /* IN - "the Internet" */

  *olen = dnsp - orig;

  /* verify that our estimation of length is valid, since
   * this has led to buffer overflows in this function */
  DEBUGASSERT(*olen == expected_len);
  ਵਾਪਸ DOH_OK;
}

ਸਥਿਰ size_t doh_probe_write_cb(ਅੱਖਰ *contents, size_t ਮਾਪ, size_t nmemb,
                                 ਖਾਲੀ *userp)
{
  size_t realsize = ਮਾਪ * nmemb;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = userp;
  ਬਣਤਰ doh_request *doh_req = Curl_meta_get(ਅੰਕੜੇ, CURL_EZM_DOH_PROBE);
  ਜੇ(!doh_req)
    ਵਾਪਸ CURL_WRITEFUNC_ERROR;

  ਜੇ(curlx_dyn_addn(&doh_req->resp_body, contents, realsize))
    ਵਾਪਸ 0;

  ਵਾਪਸ realsize;
}

ਸਥਿਰ ਖਾਲੀ doh_probe_ਪੂਰਾ(ਬਣਤਰ Curl_easy *doh,
                           ਬਣਤਰ Curl_easy *master, CURLcode ਨਤੀਜਾ);
ਸਥਿਰ ਖਾਲੀ doh_probe_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t klen, ਖਾਲੀ *e)
{
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)klen;
  ਜੇ(e) {
    ਬਣਤਰ doh_request *doh_req = e;
    curl_slist_free_all(doh_req->req_hds);
    curlx_dyn_free(&doh_req->resp_body);
    curlx_free(e);
  }
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ERROR_CHECK_SETOPT(x, y)                        \
  ਕਰੋ {                                                  \
    ਨਤੀਜਾ = curl_easy_setopt((CURL *)doh, x, y);       \
    ਜੇ(ਨਤੀਜਾ &&                                        \
       ਨਤੀਜਾ != CURLE_NOT_BUILT_ਵਿੱਚ &&                  \
       ਨਤੀਜਾ != CURLE_UNKNOWN_OPTION)                  \
      ਜਾਓ error;                                       \
  } ਜਦੋਂ(0)

ਸਥਿਰ CURLcode doh_probe_ਲੜੀਵਾਰ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              DNStype dnstype,
                              ਅਚਲ ਅੱਖਰ *host,
                              ਅਚਲ ਅੱਖਰ *url, CURLM *multi,
                              uint32_t resolv_id,
                              uint32_t *pmid)
{
  ਬਣਤਰ Curl_easy *doh = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  timediff_t timeout_ms;
  ਬਣਤਰ doh_request *doh_req;
  DOHcode d;
  bool maybe_https = !curl_strnequal(url, STRCONST("http:"));

  *pmid = UINT32_MAX;

  doh_req = curlx_calloc(1, ਆਕਾਰ(*doh_req));
  ਜੇ(!doh_req)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  doh_req->resolv_id = resolv_id;
  doh_req->dnstype = dnstype;
  curlx_dyn_init(&doh_req->resp_body, DYN_DOH_RESPONSE);

  d = doh_req_encode(host, dnstype, doh_req->req_body,
                     ਆਕਾਰ(doh_req->req_body),
                     &doh_req->req_body_len);
  ਜੇ(d) {
    failf(ਅੰਕੜੇ, "Failed to encode DoH packet [%d]", (ਪੂਰਨ)d);
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ error;
  }

  timeout_ms = Curl_timeleft_ms(ਅੰਕੜੇ);
  ਜੇ(timeout_ms < 0) {
    ਨਤੀਜਾ = CURLE_OPERATION_TIMEDOUT;
    ਜਾਓ error;
  }

  doh_req->req_hds =
    curl_slist_append(ਨਿਰਾ, "Content-Type: application/dns-message");
  ਜੇ(!doh_req->req_hds) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ error;
  }

  /* Curl_open() is the internal version of curl_easy_init() */
  ਨਤੀਜਾ = Curl_open(&doh);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ error;

  /* pass in the struct pointer via a local variable to please coverity and
     the gcc typecheck helpers */
  VERBOSE(doh->state.feat = &Curl_trc_feat_doh);
  ERROR_CHECK_SETOPT(CURLOPT_URL, url);
  ERROR_CHECK_SETOPT(CURLOPT_DEFAULT_PROTOCOL, "https");
  ERROR_CHECK_SETOPT(CURLOPT_WRITEFUNCTION, doh_probe_write_cb);
  ERROR_CHECK_SETOPT(CURLOPT_WRITEDATA, doh);
  ERROR_CHECK_SETOPT(CURLOPT_POSTFIELDS, doh_req->req_body);
  ERROR_CHECK_SETOPT(CURLOPT_POSTFIELDSIZE, (ਲੰਮਾ)doh_req->req_body_len);
  ERROR_CHECK_SETOPT(CURLOPT_HTTPHEADER, doh_req->req_hds);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTP2
  ਜੇ(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_HTTP_ਸੰਸਕਰਣ, CURL_HTTP_ਸੰਸਕਰਣ_2_TLS);
    ERROR_CHECK_SETOPT(CURLOPT_PIPEWAIT, 1L);
  }
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  /* enforce HTTPS if not debug */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
#ਨਹੀਂਤਾਂ
  /* in debug mode, also allow http */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
#ਜੇ_ਅੰਤ
  ERROR_CHECK_SETOPT(CURLOPT_TIMEOUT_MS, (ਲੰਮਾ)timeout_ms);
  ERROR_CHECK_SETOPT(CURLOPT_SHARE, (CURLSH *)ਅੰਕੜੇ->share);
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.err && ਅੰਕੜੇ->ਸੈੱਟ.err != stderr)
    ERROR_CHECK_SETOPT(CURLOPT_STDERR, ਅੰਕੜੇ->ਸੈੱਟ.err);
  ਜੇ(Curl_trc_ft_is_verbose(ਅੰਕੜੇ, &Curl_trc_feat_doh))
    ERROR_CHECK_SETOPT(CURLOPT_VERBOSE, 1L);
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.no_signal)
    ERROR_CHECK_SETOPT(CURLOPT_NOSIGNAL, 1L);
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.fdebug)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGFUNCTION, ਅੰਕੜੇ->ਸੈੱਟ.fdebug);
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.debugdata)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGDATA, ਅੰਕੜੇ->ਸੈੱਟ.debugdata);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_RESOLV_ARES
  ਜੇ(CURL_EASY_STR(ਅੰਕੜੇ, STRING_DNS_SERVERS)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_SERVERS,
                       CURL_EASY_STR(ਅੰਕੜੇ, STRING_DNS_SERVERS));
  }
  ਜੇ(CURL_EASY_STR(ਅੰਕੜੇ, STRING_DNS_INTERFACE)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_INTERFACE,
                       CURL_EASY_STR(ਅੰਕੜੇ, STRING_DNS_INTERFACE));
  }
  ਜੇ(CURL_EASY_STR(ਅੰਕੜੇ, STRING_DNS_LOCAL_IP4)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP4,
                       CURL_EASY_STR(ਅੰਕੜੇ, STRING_DNS_LOCAL_IP4));
  }
  ਜੇ(CURL_EASY_STR(ਅੰਕੜੇ, STRING_DNS_LOCAL_IP6)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP6,
                       CURL_EASY_STR(ਅੰਕੜੇ, STRING_DNS_LOCAL_IP6));
  }
#ਜੇ_ਅੰਤ
  ਜੇ(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYHOST,
                       ਅੰਕੜੇ->ਸੈੱਟ.doh_verifyhost ? 2L : 0L);
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYPEER,
                       ਅੰਕੜੇ->ਸੈੱਟ.doh_verifypeer ? 1L : 0L);
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYSTATUS,
                       ਅੰਕੜੇ->ਸੈੱਟ.doh_verifystatus ? 1L : 0L);

    /* Inherit *some* SSL options from the user's transfer. This is a
       best-guess as to which options are needed for compatibility. #3661

       Note DoH does not inherit the user's proxy server so proxy SSL settings
       have no effect and are not inherited. If that changes then two new
       options should be added to check doh proxy insecure separately,
       CURLOPT_DOH_PROXY_SSL_VERIFYHOST and CURLOPT_DOH_PROXY_SSL_VERIFYPEER.
       */
    doh->ਸੈੱਟ.ssl.custom_cafile = ਅੰਕੜੇ->ਸੈੱਟ.ssl.custom_cafile;
    doh->ਸੈੱਟ.ssl.custom_capath = ਅੰਕੜੇ->ਸੈੱਟ.ssl.custom_capath;
    doh->ਸੈੱਟ.ssl.custom_cablob = ਅੰਕੜੇ->ਸੈੱਟ.ssl.custom_cablob;
    ਜੇ(CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSL_CAFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO,
                         CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSL_CAFILE));
    }
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.blobs[BLOB_CAINFO]) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO_BLOB, ਅੰਕੜੇ->ਸੈੱਟ.blobs[BLOB_CAINFO]);
    }
    ਜੇ(CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSL_CAPATH)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAPATH,
                         CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSL_CAPATH));
    }
    ਜੇ(CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSL_CRLFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CRLFILE,
                         CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSL_CRLFILE));
    }
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssl.certinfo)
      ERROR_CHECK_SETOPT(CURLOPT_CERTINFO, 1L);
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssl_fsslctx)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_FUNCTION, ਅੰਕੜੇ->ਸੈੱਟ.ssl_fsslctx);
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssl_fsslctxp)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_ਅੰਕੜੇ, ਅੰਕੜੇ->ਸੈੱਟ.ssl_fsslctxp);
    ਜੇ(CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSL_EC_CURVES)) {
      ERROR_CHECK_SETOPT(CURLOPT_SSL_EC_CURVES,
                         CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSL_EC_CURVES));
    }
    (ਖਾਲੀ)curl_easy_setopt(doh, CURLOPT_SSL_OPTIONS,
                           ((ਲੰਮਾ)ਅੰਕੜੇ->ਸੈੱਟ.ssl.ssl_options &
                            ~CURLSSLOPT_AUTO_CLIENT_CERT));
  }

  doh->state.ਅੰਦਰੂਨੀ = TRUE;
  doh->master_mid = ਅੰਕੜੇ->mid; /* master transfer of this one */
  doh->sub_xfer_ਪੂਰਾ = doh_probe_ਪੂਰਾ;

  ਨਤੀਜਾ = Curl_meta_ਸੈੱਟ(doh, CURL_EZM_DOH_PROBE, doh_req, doh_probe_dtor);
  doh_req = ਨਿਰਾ; /* call took ownership */
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ error;

  /* DoH handles must not inherit private_data. The handles may be passed to
     the user via callbacks and the user will be able to identify them as
     internal handles because private data is not set. The user can then set
     private_data via CURLOPT_PRIVATE if they so choose. */
  DEBUGASSERT(!doh->ਸੈੱਟ.ਨਿੱਜੀ_ਅੰਕੜੇ);

  ਜੇ(Curl_multi_ਜੋੜੋ_handle(multi, doh))
    ਜਾਓ error;

  *pmid = doh->mid;
  ਵਾਪਸ CURLE_OK;

error:
  Curl_close(&doh);
  ਜੇ(doh_req)
    doh_probe_dtor(ਨਿਰਾ, 0, doh_req);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Curl_doh() starts a name resolve using DoH. It resolves a name and returns
 * a 'Curl_addrinfo *' with the address information.
 */

CURLcode Curl_doh(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                  ਬਣਤਰ Curl_resolv_async *async)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ doh_probes *dohp = ਨਿਰਾ;
  size_t i;

  DEBUGASSERT(!async->doh);
  DEBUGASSERT(async->peer->hostname[0]);
  ਜੇ(async->doh) {
    DEBUGASSERT(0); /* should not happen */
    Curl_doh_cleanup(ਅੰਕੜੇ, async);
  }

  ਜੇ(!async->dns_queries)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
  ਜੇ(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     (async->dns_queries & CURL_DNSQ_HTTPS)) {
    /* Can't mix those in the same async resolve */
    DEBUGASSERT(0);
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }
#ਨਹੀਂਤਾਂ
  ਜੇ(async->dns_queries & CURL_DNSQ_HTTPS) {
    DEBUGASSERT(0);
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
  }
#ਜੇ_ਅੰਤ

  /* start clean, consider allocating this struct on demand */
  async->doh = dohp = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ doh_probes));
  ਜੇ(!dohp)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਲਈ(i = 0; i < DOH_ਥਾਂ_ਗਿਣਤੀ; ++i) {
    dohp->probe_rc[i] = DOH_OK;
    dohp->probe_mid[i] = UINT32_MAX;
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  /* AAAA results have preference in happy eyeballing, trigger first */
  ਜੇ(async->dns_queries & CURL_DNSQ_AAAA) {
    /* create IPv6 DoH request */
    ਨਤੀਜਾ = doh_probe_ਲੜੀਵਾਰ(ਅੰਕੜੇ, CURL_DNS_TYPE_AAAA,
                           async->peer->hostname,
                           CURL_EASY_STR(ਅੰਕੜੇ, STRING_DOH),
                           ਅੰਕੜੇ->multi, async->id,
                           &dohp->probe_mid[DOH_ਥਾਂ_IPV6]);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ error;
    async->queries_ongoing++;
  }
#ਜੇ_ਅੰਤ

  /* create IPv4 DoH request */
  ਜੇ(async->dns_queries & CURL_DNSQ_A) {
    ਨਤੀਜਾ = doh_probe_ਲੜੀਵਾਰ(ਅੰਕੜੇ, CURL_DNS_TYPE_A,
                           async->peer->hostname,
                           CURL_EASY_STR(ਅੰਕੜੇ, STRING_DOH),
                           ਅੰਕੜੇ->multi, async->id,
                           &dohp->probe_mid[DOH_ਥਾਂ_IPV4]);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ error;
    async->queries_ongoing++;
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
  ਜੇ(async->dns_queries & CURL_DNSQ_HTTPS) {
    ਅੱਖਰ *qname = ਨਿਰਾ;
    ਜੇ(async->peer->ਪੋਰਟ != ਪੋਰਟ_HTTPS) {
      qname = curl_maprintf("_%u._https.%s",
                            async->peer->ਪੋਰਟ, async->peer->hostname);
      ਜੇ(!qname)
        ਜਾਓ error;
    }
    ਨਤੀਜਾ = doh_probe_ਲੜੀਵਾਰ(ਅੰਕੜੇ, CURL_DNS_TYPE_HTTPS,
                           qname ? qname : async->peer->hostname,
                           CURL_EASY_STR(ਅੰਕੜੇ, STRING_DOH), ਅੰਕੜੇ->multi,
                           async->id,
                           &dohp->probe_mid[DOH_ਥਾਂ_HTTPS_RR]);
    curlx_free(qname);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ error;
    async->queries_ongoing++;
  }
#ਜੇ_ਅੰਤ
  ਵਾਪਸ CURLE_OK;

error:
  Curl_doh_cleanup(ਅੰਕੜੇ, async);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ DOHcode doh_skipqname(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *doh, size_t dohlen,
                             ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *indexp)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਲੰਮਾਈ;
  ਕਰੋ {
    ਜੇ(dohlen < (*indexp + 1))
      ਵਾਪਸ DOH_DNS_ਬਾਹਰਲਾ_ਦਾ_RANGE;
    ਲੰਮਾਈ = doh[*indexp];
    ਜੇ((ਲੰਮਾਈ & 0xc0) == 0xc0) {
      /* name pointer, advance over it and be done */
      ਜੇ(dohlen < (*indexp + 2))
        ਵਾਪਸ DOH_DNS_ਬਾਹਰਲਾ_ਦਾ_RANGE;
      *indexp += 2;
      ਰੋਕੋ;
    }
    ਜੇ(ਲੰਮਾਈ & 0xc0)
      ਵਾਪਸ DOH_DNS_BAD_LABEL;
    ਜੇ(dohlen < (*indexp + 1 + ਲੰਮਾਈ))
      ਵਾਪਸ DOH_DNS_ਬਾਹਰਲਾ_ਦਾ_RANGE;
    *indexp += (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(1 + ਲੰਮਾਈ);
  } ਜਦੋਂ(ਲੰਮਾਈ);
  ਵਾਪਸ DOH_OK;
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ doh_get16bit(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *doh,
                                   ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਸੂਚਕਾਂਕ)
{
  ਵਾਪਸ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)((doh[ਸੂਚਕਾਂਕ] << 8) | doh[ਸੂਚਕਾਂਕ + 1]);
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ doh_get32bit(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *doh, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਸੂਚਕਾਂਕ)
{
  /* make clang and gcc optimize this to bswap by incrementing
     the pointer first. */
  doh += ਸੂਚਕਾਂਕ;

  /* avoid undefined behavior by casting to unsigned before shifting
     24 bits, possibly into the sign bit. codegen is same, but
     ub sanitizer will not be upset */
  ਵਾਪਸ ((ਬਿਨਾਨਿਸ਼ਾਨੀ)doh[0] << 24) | ((ਬਿਨਾਨਿਸ਼ਾਨੀ)doh[1] << 16) |
         ((ਬਿਨਾਨਿਸ਼ਾਨੀ)doh[2] << 8) | doh[3];
}

ਸਥਿਰ ਖਾਲੀ doh_store_a(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *doh, ਪੂਰਨ ਸੂਚਕਾਂਕ,
                        ਬਣਤਰ dohentry *d)
{
  /* silently ignore addresses over the limit */
  ਜੇ(d->numaddr < DOH_MAX_ADDR) {
    ਬਣਤਰ dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_A;
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(&a->ip.v4, &doh[ਸੂਚਕਾਂਕ], 4);
    d->numaddr++;
  }
}

ਸਥਿਰ ਖਾਲੀ doh_store_aaaa(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *doh, ਪੂਰਨ ਸੂਚਕਾਂਕ,
                           ਬਣਤਰ dohentry *d)
{
  /* silently ignore addresses over the limit */
  ਜੇ(d->numaddr < DOH_MAX_ADDR) {
    ਬਣਤਰ dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_AAAA;
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(&a->ip.v6, &doh[ਸੂਚਕਾਂਕ], 16);
    d->numaddr++;
  }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
ਸਥਿਰ DOHcode doh_store_https(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *doh, ਪੂਰਨ ਸੂਚਕਾਂਕ,
                               ਬਣਤਰ dohentry *d, uint16_t len)
{
  /* silently ignore RRs over the limit */
  ਜੇ(d->numhttps_rrs < DOH_MAX_HTTPS) {
    ਬਣਤਰ dohhttps_rr *h = &d->https_rrs[d->numhttps_rrs];
    h->val = curlx_memdup(&doh[ਸੂਚਕਾਂਕ], len);
    ਜੇ(!h->val)
      ਵਾਪਸ DOH_ਬਾਹਰਲਾ_ਦਾ_MEM;
    h->len = len;
    d->numhttps_rrs++;
  }
  ਵਾਪਸ DOH_OK;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ DOHcode doh_rdata(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *doh,
                         ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ rdlength,
                         ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ type,
                         ਪੂਰਨ ਸੂਚਕਾਂਕ,
                         ਬਣਤਰ dohentry *d)
{
  /* RDATA
     - A (TYPE 1): 4 bytes
     - AAAA (TYPE 28): 16 bytes
     - HTTPS (TYPE 65): N bytes */
  ਚੋਣ(type) {
  ਹਾਲਤ CURL_DNS_TYPE_A:
    ਜੇ(rdlength != 4)
      ਵਾਪਸ DOH_DNS_RDATA_LEN;
    doh_store_a(doh, ਸੂਚਕਾਂਕ, d);
    ਰੋਕੋ;
  ਹਾਲਤ CURL_DNS_TYPE_AAAA:
    ਜੇ(rdlength != 16)
      ਵਾਪਸ DOH_DNS_RDATA_LEN;
    doh_store_aaaa(doh, ਸੂਚਕਾਂਕ, d);
    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
  ਹਾਲਤ CURL_DNS_TYPE_HTTPS:
    ਜੇ(rdlength < 3)
      ਵਾਪਸ DOH_DNS_RDATA_LEN;
    ਵਾਪਸ doh_store_https(doh, ਸੂਚਕਾਂਕ, d, rdlength);
#ਜੇ_ਅੰਤ
  ਮੂਲ:
    /* unsupported type, or type we do not store, skip it */
    ਰੋਕੋ;
  }
  ਵਾਪਸ DOH_OK;
}

/* @unittest 1655 */
UNITTEST ਖਾਲੀ de_init(ਬਣਤਰ dohentry *de);
UNITTEST ਖਾਲੀ de_init(ਬਣਤਰ dohentry *de)
{
  memset(de, 0, ਆਕਾਰ(*de));
  de->ttl = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
}

/* TTL value cap */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_DNS_TTL 86400U /* 24 hours */
/* @unittest 1650 */
UNITTEST DOHcode doh_resp_decode(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 ਬਣਤਰ dohentry *d);
UNITTEST DOHcode doh_resp_decode(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 ਬਣਤਰ dohentry *d)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ rcode;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ qdcount;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ ancount;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ type = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ rdlength;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ nscount;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ arcount;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਸੂਚਕਾਂਕ = 12;
  DOHcode rc;

  ਜੇ(dohlen < 12)
    ਵਾਪਸ DOH_TOO_SMALL_ਬਫਰ; /* too small */
  ਜੇ(!doh || doh[0] || doh[1])
    ਵਾਪਸ DOH_DNS_BAD_ID; /* bad ID */
  rcode = doh[3] & 0x0f;
  ਜੇ(rcode == 3)
    ਵਾਪਸ DOH_DNS_NXDOMAIN; /* name does not exist */
  ਜੇ(rcode)
    ਵਾਪਸ DOH_DNS_BAD_RCODE; /* bad rcode */

  qdcount = doh_get16bit(doh, 4);
  ਜਦੋਂ(qdcount) {
    rc = doh_skipqname(doh, dohlen, &ਸੂਚਕਾਂਕ);
    ਜੇ(rc)
      ਵਾਪਸ rc; /* bad qname */
    ਜੇ(dohlen < (ਸੂਚਕਾਂਕ + 4))
      ਵਾਪਸ DOH_DNS_ਬਾਹਰਲਾ_ਦਾ_RANGE;
    ਸੂਚਕਾਂਕ += 4; /* skip question's type and class */
    qdcount--;
  }

  ancount = doh_get16bit(doh, 6);
  ਜਦੋਂ(ancount) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ dnsclass;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ttl;

    rc = doh_skipqname(doh, dohlen, &ਸੂਚਕਾਂਕ);
    ਜੇ(rc)
      ਵਾਪਸ rc; /* bad qname */

    ਜੇ(dohlen < (ਸੂਚਕਾਂਕ + 2))
      ਵਾਪਸ DOH_DNS_ਬਾਹਰਲਾ_ਦਾ_RANGE;

    type = doh_get16bit(doh, ਸੂਚਕਾਂਕ);
    ਜੇ((type != CURL_DNS_TYPE_CNAME) &&  /* may be synthesized from DNAME */
       (type != CURL_DNS_TYPE_DNAME) &&  /* if present, accept and ignore */
       (type != dnstype))
      /* Not the same type as was asked for, nor CNAME nor DNAME */
      ਵਾਪਸ DOH_DNS_UNEXPECTED_TYPE;
    ਸੂਚਕਾਂਕ += 2;

    ਜੇ(dohlen < (ਸੂਚਕਾਂਕ + 2))
      ਵਾਪਸ DOH_DNS_ਬਾਹਰਲਾ_ਦਾ_RANGE;
    dnsclass = doh_get16bit(doh, ਸੂਚਕਾਂਕ);
    ਜੇ(DNS_CLASS_ਵਿੱਚ != dnsclass)
      ਵਾਪਸ DOH_DNS_UNEXPECTED_CLASS; /* unsupported */
    ਸੂਚਕਾਂਕ += 2;

    ਜੇ(dohlen < (ਸੂਚਕਾਂਕ + 4))
      ਵਾਪਸ DOH_DNS_ਬਾਹਰਲਾ_ਦਾ_RANGE;

    ttl = doh_get32bit(doh, ਸੂਚਕਾਂਕ);
    ਜੇ(ttl > MAX_DNS_TTL)
      ttl = MAX_DNS_TTL;
    ਜੇ(ttl < d->ttl)
      d->ttl = ttl;
    ਸੂਚਕਾਂਕ += 4;

    ਜੇ(dohlen < (ਸੂਚਕਾਂਕ + 2))
      ਵਾਪਸ DOH_DNS_ਬਾਹਰਲਾ_ਦਾ_RANGE;

    rdlength = doh_get16bit(doh, ਸੂਚਕਾਂਕ);
    ਸੂਚਕਾਂਕ += 2;
    ਜੇ(dohlen < (ਸੂਚਕਾਂਕ + rdlength))
      ਵਾਪਸ DOH_DNS_ਬਾਹਰਲਾ_ਦਾ_RANGE;

    rc = doh_rdata(doh, rdlength, type, (ਪੂਰਨ)ਸੂਚਕਾਂਕ, d);
    ਜੇ(rc)
      ਵਾਪਸ rc;
    ਸੂਚਕਾਂਕ += rdlength;
    ancount--;
  }

  nscount = doh_get16bit(doh, 8);
  ਜਦੋਂ(nscount) {
    rc = doh_skipqname(doh, dohlen, &ਸੂਚਕਾਂਕ);
    ਜੇ(rc)
      ਵਾਪਸ rc; /* bad qname */

    ਜੇ(dohlen < (ਸੂਚਕਾਂਕ + 8))
      ਵਾਪਸ DOH_DNS_ਬਾਹਰਲਾ_ਦਾ_RANGE;

    ਸੂਚਕਾਂਕ += 2 + 2 + 4; /* type, dnsclass and ttl */

    ਜੇ(dohlen < (ਸੂਚਕਾਂਕ + 2))
      ਵਾਪਸ DOH_DNS_ਬਾਹਰਲਾ_ਦਾ_RANGE;

    rdlength = doh_get16bit(doh, ਸੂਚਕਾਂਕ);
    ਸੂਚਕਾਂਕ += 2;
    ਜੇ(dohlen < (ਸੂਚਕਾਂਕ + rdlength))
      ਵਾਪਸ DOH_DNS_ਬਾਹਰਲਾ_ਦਾ_RANGE;
    ਸੂਚਕਾਂਕ += rdlength;
    nscount--;
  }

  arcount = doh_get16bit(doh, 10);
  ਜਦੋਂ(arcount) {
    rc = doh_skipqname(doh, dohlen, &ਸੂਚਕਾਂਕ);
    ਜੇ(rc)
      ਵਾਪਸ rc; /* bad qname */

    ਜੇ(dohlen < (ਸੂਚਕਾਂਕ + 8))
      ਵਾਪਸ DOH_DNS_ਬਾਹਰਲਾ_ਦਾ_RANGE;

    ਸੂਚਕਾਂਕ += 2 + 2 + 4; /* type, dnsclass and ttl */

    ਜੇ(dohlen < (ਸੂਚਕਾਂਕ + 2))
      ਵਾਪਸ DOH_DNS_ਬਾਹਰਲਾ_ਦਾ_RANGE;

    rdlength = doh_get16bit(doh, ਸੂਚਕਾਂਕ);
    ਸੂਚਕਾਂਕ += 2;
    ਜੇ(dohlen < (ਸੂਚਕਾਂਕ + rdlength))
      ਵਾਪਸ DOH_DNS_ਬਾਹਰਲਾ_ਦਾ_RANGE;
    ਸੂਚਕਾਂਕ += rdlength;
    arcount--;
  }

  ਜੇ(ਸੂਚਕਾਂਕ != dohlen)
    ਵਾਪਸ DOH_DNS_MALFORMAT; /* something is wrong */

  ਵਾਪਸ DOH_OK; /* ok */
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
ਸਥਿਰ CURLcode doh2ai(ਅਚਲ ਬਣਤਰ dohentry *de, ਅਚਲ ਅੱਖਰ *hostname,
                       ਪੂਰਨ ਪੋਰਟ, ਬਣਤਰ Curl_addrinfo **aip)
{
  ਬਣਤਰ Curl_addrinfo *ai;
  ਬਣਤਰ Curl_addrinfo *prevai = ਨਿਰਾ;
  ਬਣਤਰ Curl_addrinfo *firstai = ਨਿਰਾ;
  ਬਣਤਰ sockaddr_in *addr;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਬਣਤਰ sockaddr_in6 *addr6;
#ਜੇ_ਅੰਤ
  size_t hostlen = ਲੜੀ_ਲੰਬਾਈ(hostname) + 1; /* include null-terminator */
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਪੂਰਨ i;

  ਲਈ(i = 0; i < de->numaddr; i++) {
    size_t ss_ਮਾਪ;
    CURL_SA_FAMILY_T addrtype;
    ਜੇ(de->addr[i].type == CURL_DNS_TYPE_AAAA) {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_IPV6
      /* we cannot handle IPv6 addresses */
      ਜਾਰੀ;
#ਨਹੀਂਤਾਂ
      ss_ਮਾਪ = ਆਕਾਰ(ਬਣਤਰ sockaddr_in6);
      addrtype = AF_INET6;
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ {
      ss_ਮਾਪ = ਆਕਾਰ(ਬਣਤਰ sockaddr_in);
      addrtype = AF_INET;
    }

    ai = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ Curl_addrinfo) + ss_ਮਾਪ + hostlen);
    ਜੇ(!ai) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਰੋਕੋ;
    }
    ai->ai_addr = (ਖਾਲੀ *)((ਅੱਖਰ *)ai + ਆਕਾਰ(ਬਣਤਰ Curl_addrinfo));
    ai->ai_canonname = (ਖਾਲੀ *)((ਅੱਖਰ *)ai->ai_addr + ss_ਮਾਪ);
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(ai->ai_canonname, hostname, hostlen);

    ਜੇ(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    ਜੇ(prevai)
      /* make the previous entry point to this */
      prevai->ai_ਅਗਲਾ = ai;

    ai->ai_family = addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_ਮਾਪ;

    /* leave the rest of the struct filled with zero */

    ਚੋਣ(ai->ai_family) {
    ਹਾਲਤ AF_INET:
      addr = (ਖਾਲੀ *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(ਆਕਾਰ(ਬਣਤਰ in_addr) == ਆਕਾਰ(de->addr[i].ip.v4));
      ਸਮ੍ਰਿਤੀ_ਨਕਲ(&addr->sin_addr, &de->addr[i].ip.v4, ਆਕਾਰ(ਬਣਤਰ in_addr));
      addr->sin_family = addrtype;
      addr->sin_port = htons((ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)ਪੋਰਟ);
      ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
    ਹਾਲਤ AF_INET6:
      addr6 = (ਖਾਲੀ *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(ਆਕਾਰ(ਬਣਤਰ in6_addr) == ਆਕਾਰ(de->addr[i].ip.v6));
      ਸਮ੍ਰਿਤੀ_ਨਕਲ(&addr6->sin6_addr, &de->addr[i].ip.v6, ਆਕਾਰ(ਬਣਤਰ in6_addr));
      addr6->sin6_family = addrtype;
      addr6->sin6_port = htons((ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)ਪੋਰਟ);
      ਰੋਕੋ;
#ਜੇ_ਅੰਤ
    }

    prevai = ai;
  }

  ਜੇ(ਨਤੀਜਾ) {
    Curl_freeaddrinfo(firstai);
    firstai = ਨਿਰਾ;
  }
  *aip = firstai;

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* @unittest 1655 */
UNITTEST ਖਾਲੀ de_cleanup(ਬਣਤਰ dohentry *d);
UNITTEST ਖਾਲੀ de_cleanup(ਬਣਤਰ dohentry *d)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
  ਪੂਰਨ i = 0;
  ਲਈ(i = 0; i < d->numhttps_rrs; i++)
    curlx_safefree(d->https_rrs[i].val);
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)d;
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR

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
ਸਥਿਰ CURLcode doh_decode_rdata_name(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ **buf,
                                      size_t *ਬਚਿਆ, ਅੱਖਰ **dnsname)
{
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *cp = ਨਿਰਾ;
  size_t rem = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ clen = 0; /* chunk len */
  ਬਣਤਰ dynbuf thename;

  DEBUGASSERT(buf && ਬਚਿਆ && dnsname);
  ਜੇ(!buf || !ਬਚਿਆ || !dnsname || !*ਬਚਿਆ)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  curlx_dyn_init(&thename, CURL_MAXLEN_HOST_NAME);
  rem = *ਬਚਿਆ;
  cp = *buf;
  clen = *cp++;
  /* RFC 9460 says it must be uncompressed */
  ਜੇ(clen > 63)
    ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;

  ਜੇ(clen == 0) {
    /* special case - return "." as name */
    ਜੇ(curlx_dyn_addn(&thename, ".", 1))
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
  ਜਦੋਂ(clen) {
    ਜੇ(clen >= rem) {
      curlx_dyn_free(&thename);
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
    ਜੇ(curlx_dyn_addn(&thename, cp, clen) ||
       curlx_dyn_addn(&thename, ".", 1))
      ਵਾਪਸ CURLE_TOO_LARGE;

    cp += clen;
    rem -= (clen + 1);
    ਜੇ(rem <= 0) {
      curlx_dyn_free(&thename);
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
    clen = *cp++;
    ਜੇ(clen > 63) {
      /* invalid format */
      curlx_dyn_free(&thename);
      ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
    }
  }
  *buf = cp;
  *ਬਚਿਆ = rem - 1;
  *dnsname = curlx_dyn_ptr(&thename);
  ਵਾਪਸ CURLE_OK;
}

/* scan for byte values <= 32 or 127 */
ਸਥਿਰ CURLcode junkscan(ਅਚਲ ਅੱਖਰ *url)
{
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *p = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)url;
  ਜਦੋਂ(*p) {
    ਜੇ(*p <= 0x20 || *p == 127)
      ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
    p++;
  }
  ਵਾਪਸ CURLE_OK;
}

/* @unittest 1658 */
UNITTEST CURLcode doh_resp_decode_httpsrr(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *cp, size_t len,
                                          ਬਣਤਰ Curl_https_rrinfo **hrr);
UNITTEST CURLcode doh_resp_decode_httpsrr(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *cp, size_t len,
                                          ਬਣਤਰ Curl_https_rrinfo **hrr)
{
  uint16_t pcode = 0, plen = 0;
  uint32_t expected_min_pcode = 0;
  ਬਣਤਰ Curl_https_rrinfo *lhrr = ਨਿਰਾ;
  ਅੱਖਰ *dnsname = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  *hrr = ਨਿਰਾ;
  ਜੇ(len <= 2)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  lhrr = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ Curl_https_rrinfo));
  ਜੇ(!lhrr)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  lhrr->ਤਰਜੀਹ = doh_get16bit(cp, 0);
  cp += 2;
  len -= 2;
  ਜੇ(doh_decode_rdata_name(&cp, &len, &dnsname) != CURLE_OK)
    ਜਾਓ err;
  lhrr->ਨਿਸ਼ਾਨਾ = dnsname;
  ਨਤੀਜਾ = junkscan(dnsname);
  ਜੇ(ਨਤੀਜਾ) {
    /* unacceptable hostname content */
    ਜਾਓ err;
  }
  ਜਦੋਂ(len >= 4) {
    pcode = doh_get16bit(cp, 0);
    plen = doh_get16bit(cp, 2);
    cp += 4;
    len -= 4;
    ਜੇ(pcode < expected_min_pcode || plen > len) {
      ਨਤੀਜਾ = CURLE_WEIRD_SERVER_ਜਵਾਬ;
      ਜਾਓ err;
    }
    ਨਤੀਜਾ = Curl_httpsrr_ਸੈੱਟ(lhrr, pcode, cp, plen);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ err;
    Curl_httpsrr_trace(ਅੰਕੜੇ, lhrr);
    cp += plen;
    len -= plen;
    expected_min_pcode = pcode + 1;
  }
  *hrr = lhrr;
  ਵਾਪਸ CURLE_OK;
err:
  Curl_httpsrr_destroy(lhrr);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* USE_HTTPSRR */

/* called from multi when a sub transfer, e.g. doh probe, is done.
 * Parse the response and set the results in the `async` context
 * of master, using the id from the probe's CURL_EZM_DOH_PROBE
 * meta data. */
ਸਥਿਰ ਖਾਲੀ doh_probe_ਪੂਰਾ(ਬਣਤਰ Curl_easy *doh,
                           ਬਣਤਰ Curl_easy *master, CURLcode ਨਤੀਜਾ)
{
  ਬਣਤਰ Curl_resolv_async *async = ਨਿਰਾ;
  ਬਣਤਰ doh_probes *dohp = ਨਿਰਾ;
  ਬਣਤਰ doh_request *doh_req = ਨਿਰਾ;
  ਬਣਤਰ Curl_addrinfo **pdest_ai;
  ਬਣਤਰ dohentry de;
  ਪੂਰਨ ਥਾਂ, httpcode;

  de_init(&de);
  doh_req = Curl_meta_get(doh, CURL_EZM_DOH_PROBE);
  ਜੇ(!doh_req) {
    /* transfer `doh` is not a DoH probe. */
    DEBUGASSERT(0);
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  async = Curl_async_get(master, doh_req->resolv_id);
  ਜੇ(!async) {
    CURL_TRC_DNS(master, "[%u] ignoring outdated DoH response",
                 doh_req->resolv_id);
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  dohp = async->doh;

  ਲਈ(ਥਾਂ = 0; ਥਾਂ < DOH_ਥਾਂ_ਗਿਣਤੀ; ++ਥਾਂ) {
    ਜੇ(dohp->probe_mid[ਥਾਂ] == doh->mid)
      ਰੋਕੋ;
  }
  /* We really should have found the slot where to store the response */
  ਜੇ(ਥਾਂ >= DOH_ਥਾਂ_ਗਿਣਤੀ) {
    failf(master, "DoH: unknown sub request done");
    DEBUGASSERT(0);
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  async->queries_ongoing--;
  dohp = async->doh;
  httpcode = doh->info.httpcode;
  ਚੋਣ(ਥਾਂ) {
  ਹਾਲਤ DOH_ਥਾਂ_IPV4:
    async->dns_responses |= CURL_DNSQ_A;
    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਹਾਲਤ DOH_ਥਾਂ_IPV6:
    async->dns_responses |= CURL_DNSQ_AAAA;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
  ਹਾਲਤ DOH_ਥਾਂ_HTTPS_RR:
    async->dns_responses |= CURL_DNSQ_HTTPS;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  ਮੂਲ:
    DEBUGASSERT(0);
    ਰੋਕੋ;
  }

  ਜੇ(ਨਤੀਜਾ) {
    dohp->probe_rc[ਥਾਂ] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: %s",
          doh_type2name(doh_req->dnstype), curl_easy_strerror(ਨਤੀਜਾ));
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਹੀਂਤਾਂ ਜੇ((httpcode < 200) || (httpcode >= 300)) {
    dohp->probe_rc[ਥਾਂ] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: HTTP status %d",
          doh_type2name(doh_req->dnstype), httpcode);
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  dohp->probe_rc[ਥਾਂ] = doh_resp_decode(curlx_dyn_uptr(&doh_req->resp_body),
                                         curlx_dyn_len(&doh_req->resp_body),
                                         doh_req->dnstype, &de);
  ਜੇ(dohp->probe_rc[ਥਾਂ]) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
    ਜੇ((dohp->probe_rc[ਥਾਂ] == DOH_NO_CONTENT) &&
       (doh_req->dnstype == CURL_DNS_TYPE_HTTPS)) {
      dohp->probe_rc[ਥਾਂ] = DOH_DNS_NXDOMAIN;
    }
#ਜੇ_ਅੰਤ
    infof(doh, "[DoH] [%s] error decoding response: %s",
          doh_type2name(doh_req->dnstype),
          doh_strerror(dohp->probe_rc[ਥਾਂ]));
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(doh_req->dnstype == CURL_DNS_TYPE_A)
    pdest_ai = &async->ai_A;
  ਨਹੀਂਤਾਂ ਜੇ(doh_req->dnstype == CURL_DNS_TYPE_AAAA)
    pdest_ai = &async->ai_AAAA;
  ਨਹੀਂਤਾਂ
    pdest_ai = ਨਿਰਾ;

  ਜੇ(pdest_ai && de.numaddr) {
    ਜੇ(*pdest_ai) {
      Curl_freeaddrinfo(*pdest_ai);
      *pdest_ai = ਨਿਰਾ;
    }
    ਨਤੀਜਾ = doh2ai(&de, async->peer->hostname, async->peer->ਪੋਰਟ, pdest_ai);
    ਜੇ(ਨਤੀਜਾ) { /* hard failure on our side, fail completely */
      infof(doh, "[DoH] [%s] error creating addrinfo: %s",
            doh_type2name(doh_req->dnstype), curl_easy_strerror(ਨਤੀਜਾ));
      dohp->probe_rc[ਥਾਂ] = DOH_OOM;
      async->ਨਤੀਜਾ = ਨਤੀਜਾ;
    }
  }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
  ਨਹੀਂਤਾਂ ਜੇ((doh_req->dnstype == CURL_DNS_TYPE_HTTPS) && de.numhttps_rrs) {
    CURL_TRC_DNS(doh, "[HTTPS] got %d records", de.numhttps_rrs);
    ਨਤੀਜਾ = doh_resp_decode_httpsrr(doh, de.https_rrs->val,
                                     de.https_rrs->len, &async->httpsrr);
    ਜੇ(ਨਤੀਜਾ) {
      dohp->probe_rc[ਥਾਂ] = DOH_HTTP_FAILED;
      infof(doh, "[DoH] error decoding HTTPS RR: %s",
            curl_easy_strerror(ਨਤੀਜਾ));
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }
#ਜੇ_ਅੰਤ /* USE_HTTPSRR */

  /* DoH request complete, run master to act on results */
  infof(doh, "DoH request complete, %u to go", async->queries_ongoing);

ਬਾਹਰਲਾ:
  Curl_multi_mark_dirty(master);
  de_cleanup(&de);
  Curl_meta_remove(doh, CURL_EZM_DOH_PROBE);
}

CURLcode Curl_doh_take_ਨਤੀਜਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ Curl_resolv_async *async,
                              ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **pdns)
{
  ਬਣਤਰ doh_probes *dohp = async->doh;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  *pdns = ਨਿਰਾ; /* defaults to no response */
  ਜੇ(!dohp)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  async->negative_answer = FALSE;
  ਜੇ(async->ਨਤੀਜਾ) {
    ਨਤੀਜਾ = async->ਨਤੀਜਾ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     dohp->probe_mid[DOH_ਥਾਂ_IPV4] == UINT32_MAX &&
     dohp->probe_mid[DOH_ਥਾਂ_IPV6] == UINT32_MAX) {
    failf(ਅੰਕੜੇ, "Could not DoH-resolve: %s", async->peer->hostname);
    ਵਾਪਸ async->for_proxy ?
      CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!async->queries_ongoing) {
    ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns = ਨਿਰਾ;
    bool negative = TRUE;
    ਪੂਰਨ ਥਾਂ;

    /* remove DoH handles from multi handle and close them */
    doh_close(ਅੰਕੜੇ, async);
    /* parse the responses, create the struct and return it! */
    ਲਈ(ਥਾਂ = 0; ਥਾਂ < DOH_ਥਾਂ_ਗਿਣਤੀ; ਥਾਂ++) {
      /* Failing without an NXDOMAIN answer - a SERVFAIL-class rcode or
         an undecodable response - says nothing about the name. Such a
         failure must not be cached as a negative entry. */
      ਜੇ(dohp->probe_rc[ਥਾਂ] && (dohp->probe_rc[ਥਾਂ] != DOH_DNS_NXDOMAIN))
        negative = FALSE;
    } /* next slot */

    ਜੇ(async->ai_A || async->ai_AAAA) {
      dns = Curl_dnsc_mk_addr2(
        ਅੰਕੜੇ, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
      ਜੇ(!dns) {
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
    ਨਹੀਂਤਾਂ ਜੇ((async->dns_queries & CURL_DNSQ_HTTPS) &&
            !dohp->probe_rc[DOH_ਥਾਂ_HTTPS_RR]) {
      Curl_httpsrr_trace(ਅੰਕੜੇ, async->httpsrr);
      dns = Curl_dnsc_mk_https(ਅੰਕੜੇ, &async->httpsrr, async->peer);
      ਜੇ(!dns) {
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
#ਜੇ_ਅੰਤ /* USE_HTTPSRR */
    ਨਹੀਂਤਾਂ {
      /* every query failed. Only NXDOMAIN answers for all of them
         make this a negative answer, eligible for caching. */
      async->negative_answer = negative;
      ਨਤੀਜਾ = async->for_proxy ?
        CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
    }

    /* and add the entry to the cache */
    ਜੇ(dns)
      ਨਤੀਜਾ = Curl_dnscache_ਜੋੜੋ(ਅੰਕੜੇ, dns);
    *pdns = dns;
  } /* !async->queries_ongoing */
  ਨਹੀਂਤਾਂ
    /* wait for pending DoH transactions to complete */
    ਵਾਪਸ CURLE_AGAIN;

ਬਾਹਰਲਾ:
  Curl_doh_cleanup(ਅੰਕੜੇ, async);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ doh_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                      ਬਣਤਰ Curl_resolv_async *async)
{
  ਬਣਤਰ doh_probes *doh = async ? async->doh : ਨਿਰਾ;
  ਜੇ(doh && ਅੰਕੜੇ->multi) {
    ਬਣਤਰ Curl_easy *probe_ਅੰਕੜੇ;
    uint32_t mid;
    size_t ਥਾਂ;
    ਲਈ(ਥਾਂ = 0; ਥਾਂ < DOH_ਥਾਂ_ਗਿਣਤੀ; ਥਾਂ++) {
      mid = doh->probe_mid[ਥਾਂ];
      ਜੇ(mid == UINT32_MAX)
        ਜਾਰੀ;
      doh->probe_mid[ਥਾਂ] = UINT32_MAX;
      /* should have been called before data is removed from multi handle */
      DEBUGASSERT(ਅੰਕੜੇ->multi);
      probe_ਅੰਕੜੇ = ਅੰਕੜੇ->multi ? Curl_multi_get_easy(ਅੰਕੜੇ->multi, mid) : ਨਿਰਾ;
      ਜੇ(!probe_ਅੰਕੜੇ) {
        DEBUGF(infof(ਅੰਕੜੇ, "Curl_doh_close: xfer for mid=%u not found!", mid));
        ਜਾਰੀ;
      }
      probe_ਅੰਕੜੇ->sub_xfer_ਪੂਰਾ = ਨਿਰਾ; /* No longer interested in result */
      /* data->multi might already be reset at this time */
      Curl_multi_remove_handle(ਅੰਕੜੇ->multi, probe_ਅੰਕੜੇ);
      Curl_close(&probe_ਅੰਕੜੇ);
    }
    CURL_TRC_DNS(ਅੰਕੜੇ, "[DoH] probe done");
  }
}

ਖਾਲੀ Curl_doh_cleanup(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                      ਬਣਤਰ Curl_resolv_async *async)
{
  ਬਣਤਰ doh_probes *dohp = async->doh;
  ਜੇ(dohp) {
    doh_close(ਅੰਕੜੇ, async);
    curlx_safefree(async->doh);
  }
}

#ਜੇ_ਅੰਤ /* CURL_DISABLE_DOH */
