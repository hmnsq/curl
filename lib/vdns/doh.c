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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifndef CURL_DISABLE_DOH

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "curl_addrinfo.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "vdns/doh.h"
#ଅନ୍ତର୍ଭାଵ "vdns/httpsrr.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strdup.h"
#ଅନ୍ତର୍ଭାଵ "curlx/dynbuf.h"
#ଅନ୍ତର୍ଭାଵ "escape.h"  /* for Curl_hexencode() */
#ଅନ୍ତର୍ଭାଵ "urlapi-int.h"

#define DNS_CLASS_IN 0x01

ସ୍ଥିର ଶୂନ୍ୟ doh_close(ସଂରଚନା Curl_easy *data,
                      ସଂରଚନା Curl_resolv_async *async);

#ifdef CURLVERBOSE
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ doh_code_str[] = {
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

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *doh_strerror(DOHcode code)
{
  ଯଦି((size_t)code < CURL_ARRAYSIZE(doh_code_str))
    ଫେରନ୍ତୁ doh_code_str[code];
  ଫେରନ୍ତୁ "bad error code";
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *doh_type2name(DNStype dnstype)
{
  ବାଛ(dnstype) {
  ସ୍ଥିତି CURL_DNS_TYPE_A:
    ଫେରନ୍ତୁ "A";
  ସ୍ଥିତି CURL_DNS_TYPE_AAAA:
    ଫେରନ୍ତୁ "AAAA";
#ifdef USE_HTTPSRR
  ସ୍ଥିତି CURL_DNS_TYPE_HTTPS:
    ଫେରନ୍ତୁ "HTTPS";
#endif
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ "unknown";
  }
}

#endif /* CURLVERBOSE */

/* @unittest 1655
 */
UNITTEST DOHcode doh_req_encode(ଅଚଳ ଅକ୍ଷର *host,
                                DNStype dnstype,
                                unsigned ଅକ୍ଷର *dnsp,  /* buffer */
                                size_t len,  /* buffer size */
                                size_t *olen);  /* output length */
UNITTEST DOHcode doh_req_encode(ଅଚଳ ଅକ୍ଷର *host,
                                DNStype dnstype,
                                unsigned ଅକ୍ଷର *dnsp, /* buffer */
                                size_t len,   /* buffer size */
                                size_t *olen) /* output length */
{
  ଅଚଳ size_t hostlen = strlen(host);
  unsigned ଅକ୍ଷର *orig = dnsp;
  ଅଚଳ ଅକ୍ଷର *hostp = host;

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
  ଯଦି(host[hostlen - 1] != '.')
    expected_len++;

  ଯଦି(expected_len > DOH_MAX_DNSREQ_SIZE)
    ଫେରନ୍ତୁ DOH_DNS_NAME_TOO_LONG;

  ଯଦି(len < expected_len)
    ଫେରନ୍ତୁ DOH_TOO_SMALL_BUFFER;

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
  ଯେତେବେଳେ(*hostp) {
    size_t labellen;
    ଅଚଳ ଅକ୍ଷର *dot = strchr(hostp, '.');
    ଯଦି(dot)
      labellen = dot - hostp;
    ଅନ୍ୟଥା
      labellen = strlen(hostp);
    ଯଦି((labellen > 63) || (!labellen)) {
      /* label is too long or too short, error out */
      *olen = 0;
      ଫେରନ୍ତୁ DOH_DNS_BAD_LABEL;
    }
    /* label is non-empty, process it */
    *dnsp++ = (unsigned ଅକ୍ଷର)labellen;
    memcpy(dnsp, hostp, labellen);
    dnsp += labellen;
    hostp += labellen;
    /* advance past dot, but only if there is one */
    ଯଦି(dot)
      hostp++;
  } /* next label */

  *dnsp++ = 0; /* append zero-length label for root */

  /* There are assigned TYPE codes beyond 255: use range [1..65535] */
  *dnsp++ = (unsigned ଅକ୍ଷର)(255 & (dnstype >> 8)); /* upper 8-bit TYPE */
  *dnsp++ = (unsigned ଅକ୍ଷର)(255 & dnstype);        /* lower 8-bit TYPE */

  *dnsp++ = '\0'; /* upper 8-bit CLASS */
  *dnsp++ = DNS_CLASS_IN; /* IN - "the Internet" */

  *olen = dnsp - orig;

  /* verify that our estimation of length is valid, since
   * this has led to buffer overflows in this function */
  DEBUGASSERT(*olen == expected_len);
  ଫେରନ୍ତୁ DOH_OK;
}

ସ୍ଥିର size_t doh_probe_write_cb(ଅକ୍ଷର *contents, size_t size, size_t nmemb,
                                 ଶୂନ୍ୟ *userp)
{
  size_t realsize = size * nmemb;
  ସଂରଚନା Curl_easy *data = userp;
  ସଂରଚନା doh_request *doh_req = Curl_meta_get(data, CURL_EZM_DOH_PROBE);
  ଯଦି(!doh_req)
    ଫେରନ୍ତୁ CURL_WRITEFUNC_ERROR;

  ଯଦି(curlx_dyn_addn(&doh_req->resp_body, contents, realsize))
    ଫେରନ୍ତୁ 0;

  ଫେରନ୍ତୁ realsize;
}

ସ୍ଥିର ଶୂନ୍ୟ doh_probe_done(ସଂରଚନା Curl_easy *doh,
                           ସଂରଚନା Curl_easy *master, CURLcode result);
ସ୍ଥିର ଶୂନ୍ୟ doh_probe_dtor(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t klen, ଶୂନ୍ୟ *e)
{
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)klen;
  ଯଦି(e) {
    ସଂରଚନା doh_request *doh_req = e;
    curl_slist_free_all(doh_req->req_hds);
    curlx_dyn_free(&doh_req->resp_body);
    curlx_free(e);
  }
}

#define ERROR_CHECK_SETOPT(x, y)                        \
  କର {                                                  \
    result = curl_easy_setopt((CURL *)doh, x, y);       \
    ଯଦି(result &&                                        \
       result != CURLE_NOT_BUILT_IN &&                  \
       result != CURLE_UNKNOWN_OPTION)                  \
      ଯାଅ error;                                       \
  } ଯେତେବେଳେ(0)

ସ୍ଥିର CURLcode doh_probe_run(ସଂରଚନା Curl_easy *data,
                              DNStype dnstype,
                              ଅଚଳ ଅକ୍ଷର *host,
                              ଅଚଳ ଅକ୍ଷର *url, CURLM *multi,
                              uint32_t resolv_id,
                              uint32_t *pmid)
{
  ସଂରଚନା Curl_easy *doh = ଖାଲି;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  ସଂରଚନା doh_request *doh_req;
  DOHcode d;
  bool maybe_https = !curl_strnequal(url, STRCONST("http:"));

  *pmid = UINT32_MAX;

  doh_req = curlx_calloc(1, ଆକାର(*doh_req));
  ଯଦି(!doh_req)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  doh_req->resolv_id = resolv_id;
  doh_req->dnstype = dnstype;
  curlx_dyn_init(&doh_req->resp_body, DYN_DOH_RESPONSE);

  d = doh_req_encode(host, dnstype, doh_req->req_body,
                     ଆକାର(doh_req->req_body),
                     &doh_req->req_body_len);
  ଯଦି(d) {
    failf(data, "Failed to encode DoH packet [%d]", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)d);
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ error;
  }

  timeout_ms = Curl_timeleft_ms(data);
  ଯଦି(timeout_ms < 0) {
    result = CURLE_OPERATION_TIMEDOUT;
    ଯାଅ error;
  }

  doh_req->req_hds =
    curl_slist_append(ଖାଲି, "Content-Type: application/dns-message");
  ଯଦି(!doh_req->req_hds) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ error;
  }

  /* Curl_open() is the internal version of curl_easy_init() */
  result = Curl_open(&doh);
  ଯଦି(result)
    ଯାଅ error;

  /* pass in the struct pointer via a local variable to please coverity and
     the gcc typecheck helpers */
  VERBOSE(doh->state.feat = &Curl_trc_feat_doh);
  ERROR_CHECK_SETOPT(CURLOPT_URL, url);
  ERROR_CHECK_SETOPT(CURLOPT_DEFAULT_PROTOCOL, "https");
  ERROR_CHECK_SETOPT(CURLOPT_WRITEFUNCTION, doh_probe_write_cb);
  ERROR_CHECK_SETOPT(CURLOPT_WRITEDATA, doh);
  ERROR_CHECK_SETOPT(CURLOPT_POSTFIELDS, doh_req->req_body);
  ERROR_CHECK_SETOPT(CURLOPT_POSTFIELDSIZE, (long)doh_req->req_body_len);
  ERROR_CHECK_SETOPT(CURLOPT_HTTPHEADER, doh_req->req_hds);
#ifdef USE_HTTP2
  ଯଦି(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);
    ERROR_CHECK_SETOPT(CURLOPT_PIPEWAIT, 1L);
  }
#endif
#ifndef DEBUGBUILD
  /* enforce HTTPS if not debug */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
#ଅନ୍ୟଥା
  /* in debug mode, also allow http */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
#endif
  ERROR_CHECK_SETOPT(CURLOPT_TIMEOUT_MS, (long)timeout_ms);
  ERROR_CHECK_SETOPT(CURLOPT_SHARE, (CURLSH *)data->share);
  ଯଦି(data->set.err && data->set.err != stderr)
    ERROR_CHECK_SETOPT(CURLOPT_STDERR, data->set.err);
  ଯଦି(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_doh))
    ERROR_CHECK_SETOPT(CURLOPT_VERBOSE, 1L);
  ଯଦି(data->set.no_signal)
    ERROR_CHECK_SETOPT(CURLOPT_NOSIGNAL, 1L);
  ଯଦି(data->set.fdebug)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGFUNCTION, data->set.fdebug);
  ଯଦି(data->set.debugdata)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGDATA, data->set.debugdata);

#ifdef USE_RESOLV_ARES
  ଯଦି(CURL_EASY_STR(data, STRING_DNS_SERVERS)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_SERVERS,
                       CURL_EASY_STR(data, STRING_DNS_SERVERS));
  }
  ଯଦି(CURL_EASY_STR(data, STRING_DNS_INTERFACE)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_INTERFACE,
                       CURL_EASY_STR(data, STRING_DNS_INTERFACE));
  }
  ଯଦି(CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP4,
                       CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4));
  }
  ଯଦି(CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP6,
                       CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6));
  }
#endif
  ଯଦି(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYHOST,
                       data->set.doh_verifyhost ? 2L : 0L);
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYPEER,
                       data->set.doh_verifypeer ? 1L : 0L);
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYSTATUS,
                       data->set.doh_verifystatus ? 1L : 0L);

    /* Inherit *some* SSL options from the user's transfer. This is a
       best-guess as to which options are needed for compatibility. #3661

       Note DoH does not inherit the user's proxy server so proxy SSL settings
       have no effect and are not inherited. If that changes then two new
       options should be added to check doh proxy insecure separately,
       CURLOPT_DOH_PROXY_SSL_VERIFYHOST and CURLOPT_DOH_PROXY_SSL_VERIFYPEER.
       */
    doh->set.ssl.custom_cafile = data->set.ssl.custom_cafile;
    doh->set.ssl.custom_capath = data->set.ssl.custom_capath;
    doh->set.ssl.custom_cablob = data->set.ssl.custom_cablob;
    ଯଦି(CURL_EASY_STR(data, STRING_SSL_CAFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO,
                         CURL_EASY_STR(data, STRING_SSL_CAFILE));
    }
    ଯଦି(data->set.blobs[BLOB_CAINFO]) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO_BLOB, data->set.blobs[BLOB_CAINFO]);
    }
    ଯଦି(CURL_EASY_STR(data, STRING_SSL_CAPATH)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAPATH,
                         CURL_EASY_STR(data, STRING_SSL_CAPATH));
    }
    ଯଦି(CURL_EASY_STR(data, STRING_SSL_CRLFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CRLFILE,
                         CURL_EASY_STR(data, STRING_SSL_CRLFILE));
    }
    ଯଦି(data->set.ssl.certinfo)
      ERROR_CHECK_SETOPT(CURLOPT_CERTINFO, 1L);
    ଯଦି(data->set.ssl_fsslctx)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_FUNCTION, data->set.ssl_fsslctx);
    ଯଦି(data->set.ssl_fsslctxp)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_DATA, data->set.ssl_fsslctxp);
    ଯଦି(CURL_EASY_STR(data, STRING_SSL_EC_CURVES)) {
      ERROR_CHECK_SETOPT(CURLOPT_SSL_EC_CURVES,
                         CURL_EASY_STR(data, STRING_SSL_EC_CURVES));
    }
    (ଶୂନ୍ୟ)curl_easy_setopt(doh, CURLOPT_SSL_OPTIONS,
                           ((long)data->set.ssl.ssl_options &
                            ~CURLSSLOPT_AUTO_CLIENT_CERT));
  }

  doh->state.internal = TRUE;
  doh->master_mid = data->mid; /* master transfer of this one */
  doh->sub_xfer_done = doh_probe_done;

  result = Curl_meta_set(doh, CURL_EZM_DOH_PROBE, doh_req, doh_probe_dtor);
  doh_req = ଖାଲି; /* call took ownership */
  ଯଦି(result)
    ଯାଅ error;

  /* DoH handles must not inherit private_data. The handles may be passed to
     the user via callbacks and the user will be able to identify them as
     internal handles because private data is not set. The user can then set
     private_data via CURLOPT_PRIVATE if they so choose. */
  DEBUGASSERT(!doh->set.private_data);

  ଯଦି(Curl_multi_add_handle(multi, doh))
    ଯାଅ error;

  *pmid = doh->mid;
  ଫେରନ୍ତୁ CURLE_OK;

error:
  Curl_close(&doh);
  ଯଦି(doh_req)
    doh_probe_dtor(ଖାଲି, 0, doh_req);
  ଫେରନ୍ତୁ result;
}

/*
 * Curl_doh() starts a name resolve using DoH. It resolves a name and returns
 * a 'Curl_addrinfo *' with the address information.
 */

CURLcode Curl_doh(ସଂରଚନା Curl_easy *data,
                  ସଂରଚନା Curl_resolv_async *async)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା doh_probes *dohp = ଖାଲି;
  size_t i;

  DEBUGASSERT(!async->doh);
  DEBUGASSERT(async->peer->hostname[0]);
  ଯଦି(async->doh) {
    DEBUGASSERT(0); /* should not happen */
    Curl_doh_cleanup(data, async);
  }

  ଯଦି(!async->dns_queries)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef USE_HTTPSRR
  ଯଦି(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     (async->dns_queries & CURL_DNSQ_HTTPS)) {
    /* Can't mix those in the same async resolve */
    DEBUGASSERT(0);
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  }
#ଅନ୍ୟଥା
  ଯଦି(async->dns_queries & CURL_DNSQ_HTTPS) {
    DEBUGASSERT(0);
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
  }
#endif

  /* start clean, consider allocating this struct on demand */
  async->doh = dohp = curlx_calloc(1, ଆକାର(ସଂରଚନା doh_probes));
  ଯଦି(!dohp)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  ପାଇଁ(i = 0; i < DOH_SLOT_ଗଣନା; ++i) {
    dohp->probe_rc[i] = DOH_OK;
    dohp->probe_mid[i] = UINT32_MAX;
  }

#ifdef USE_IPV6
  /* AAAA results have preference in happy eyeballing, trigger first */
  ଯଦି(async->dns_queries & CURL_DNSQ_AAAA) {
    /* create IPv6 DoH request */
    result = doh_probe_run(data, CURL_DNS_TYPE_AAAA,
                           async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH),
                           data->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV6]);
    ଯଦି(result)
      ଯାଅ error;
    async->queries_ongoing++;
  }
#endif

  /* create IPv4 DoH request */
  ଯଦି(async->dns_queries & CURL_DNSQ_A) {
    result = doh_probe_run(data, CURL_DNS_TYPE_A,
                           async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH),
                           data->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV4]);
    ଯଦି(result)
      ଯାଅ error;
    async->queries_ongoing++;
  }

#ifdef USE_HTTPSRR
  ଯଦି(async->dns_queries & CURL_DNSQ_HTTPS) {
    ଅକ୍ଷର *qname = ଖାଲି;
    ଯଦି(async->peer->port != PORT_HTTPS) {
      qname = curl_maprintf("_%u._https.%s",
                            async->peer->port, async->peer->hostname);
      ଯଦି(!qname)
        ଯାଅ error;
    }
    result = doh_probe_run(data, CURL_DNS_TYPE_HTTPS,
                           qname ? qname : async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH), data->multi,
                           async->id,
                           &dohp->probe_mid[DOH_SLOT_HTTPS_RR]);
    curlx_free(qname);
    ଯଦି(result)
      ଯାଅ error;
    async->queries_ongoing++;
  }
#endif
  ଫେରନ୍ତୁ CURLE_OK;

error:
  Curl_doh_cleanup(data, async);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର DOHcode doh_skipqname(ଅଚଳ unsigned ଅକ୍ଷର *doh, size_t dohlen,
                             unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *indexp)
{
  unsigned ଅକ୍ଷର length;
  କର {
    ଯଦି(dohlen < (*indexp + 1))
      ଫେରନ୍ତୁ DOH_DNS_OUT_OF_RANGE;
    length = doh[*indexp];
    ଯଦି((length & 0xc0) == 0xc0) {
      /* name pointer, advance over it and be done */
      ଯଦି(dohlen < (*indexp + 2))
        ଫେରନ୍ତୁ DOH_DNS_OUT_OF_RANGE;
      *indexp += 2;
      ଭାଙ୍ଗ;
    }
    ଯଦି(length & 0xc0)
      ଫେରନ୍ତୁ DOH_DNS_BAD_LABEL;
    ଯଦି(dohlen < (*indexp + 1 + length))
      ଫେରନ୍ତୁ DOH_DNS_OUT_OF_RANGE;
    *indexp += (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(1 + length);
  } ଯେତେବେଳେ(length);
  ଫେରନ୍ତୁ DOH_OK;
}

ସ୍ଥିର unsigned short doh_get16bit(ଅଚଳ unsigned ଅକ୍ଷର *doh,
                                   unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା index)
{
  ଫେରନ୍ତୁ (unsigned short)((doh[index] << 8) | doh[index + 1]);
}

ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା doh_get32bit(ଅଚଳ unsigned ଅକ୍ଷର *doh, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା index)
{
  /* make clang and gcc optimize this to bswap by incrementing
     the pointer first. */
  doh += index;

  /* avoid undefined behavior by casting to unsigned before shifting
     24 bits, possibly into the sign bit. codegen is same, but
     ub sanitizer will not be upset */
  ଫେରନ୍ତୁ ((unsigned)doh[0] << 24) | ((unsigned)doh[1] << 16) |
         ((unsigned)doh[2] << 8) | doh[3];
}

ସ୍ଥିର ଶୂନ୍ୟ doh_store_a(ଅଚଳ unsigned ଅକ୍ଷର *doh, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index,
                        ସଂରଚନା dohentry *d)
{
  /* silently ignore addresses over the limit */
  ଯଦି(d->numaddr < DOH_MAX_ADDR) {
    ସଂରଚନା dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_A;
    memcpy(&a->ip.v4, &doh[index], 4);
    d->numaddr++;
  }
}

ସ୍ଥିର ଶୂନ୍ୟ doh_store_aaaa(ଅଚଳ unsigned ଅକ୍ଷର *doh, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index,
                           ସଂରଚନା dohentry *d)
{
  /* silently ignore addresses over the limit */
  ଯଦି(d->numaddr < DOH_MAX_ADDR) {
    ସଂରଚନା dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_AAAA;
    memcpy(&a->ip.v6, &doh[index], 16);
    d->numaddr++;
  }
}

#ifdef USE_HTTPSRR
ସ୍ଥିର DOHcode doh_store_https(ଅଚଳ unsigned ଅକ୍ଷର *doh, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index,
                               ସଂରଚନା dohentry *d, uint16_t len)
{
  /* silently ignore RRs over the limit */
  ଯଦି(d->numhttps_rrs < DOH_MAX_HTTPS) {
    ସଂରଚନା dohhttps_rr *h = &d->https_rrs[d->numhttps_rrs];
    h->val = curlx_memdup(&doh[index], len);
    ଯଦି(!h->val)
      ଫେରନ୍ତୁ DOH_OUT_OF_MEM;
    h->len = len;
    d->numhttps_rrs++;
  }
  ଫେରନ୍ତୁ DOH_OK;
}
#endif

ସ୍ଥିର DOHcode doh_rdata(ଅଚଳ unsigned ଅକ୍ଷର *doh,
                         unsigned short rdlength,
                         unsigned short type,
                         ପୂର୍ଣ୍ଣସଂଖ୍ୟା index,
                         ସଂରଚନା dohentry *d)
{
  /* RDATA
     - A (TYPE 1): 4 bytes
     - AAAA (TYPE 28): 16 bytes
     - HTTPS (TYPE 65): N bytes */
  ବାଛ(type) {
  ସ୍ଥିତି CURL_DNS_TYPE_A:
    ଯଦି(rdlength != 4)
      ଫେରନ୍ତୁ DOH_DNS_RDATA_LEN;
    doh_store_a(doh, index, d);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_DNS_TYPE_AAAA:
    ଯଦି(rdlength != 16)
      ଫେରନ୍ତୁ DOH_DNS_RDATA_LEN;
    doh_store_aaaa(doh, index, d);
    ଭାଙ୍ଗ;
#ifdef USE_HTTPSRR
  ସ୍ଥିତି CURL_DNS_TYPE_HTTPS:
    ଯଦି(rdlength < 3)
      ଫେରନ୍ତୁ DOH_DNS_RDATA_LEN;
    ଫେରନ୍ତୁ doh_store_https(doh, index, d, rdlength);
#endif
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    /* unsupported type, or type we do not store, skip it */
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ DOH_OK;
}

/* @unittest 1655 */
UNITTEST ଶୂନ୍ୟ de_init(ସଂରଚନା dohentry *de);
UNITTEST ଶୂନ୍ୟ de_init(ସଂରଚନା dohentry *de)
{
  memset(de, 0, ଆକାର(*de));
  de->ttl = INT_MAX;
}

/* TTL value cap */
#define MAX_DNS_TTL 86400U /* 24 hours */
/* @unittest 1650 */
UNITTEST DOHcode doh_resp_decode(ଅଚଳ unsigned ଅକ୍ଷର *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 ସଂରଚନା dohentry *d);
UNITTEST DOHcode doh_resp_decode(ଅଚଳ unsigned ଅକ୍ଷର *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 ସଂରଚନା dohentry *d)
{
  unsigned ଅକ୍ଷର rcode;
  unsigned short qdcount;
  unsigned short ancount;
  unsigned short type = 0;
  unsigned short rdlength;
  unsigned short nscount;
  unsigned short arcount;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା index = 12;
  DOHcode rc;

  ଯଦି(dohlen < 12)
    ଫେରନ୍ତୁ DOH_TOO_SMALL_BUFFER; /* too small */
  ଯଦି(!doh || doh[0] || doh[1])
    ଫେରନ୍ତୁ DOH_DNS_BAD_ID; /* bad ID */
  rcode = doh[3] & 0x0f;
  ଯଦି(rcode == 3)
    ଫେରନ୍ତୁ DOH_DNS_NXDOMAIN; /* name does not exist */
  ଯଦି(rcode)
    ଫେରନ୍ତୁ DOH_DNS_BAD_RCODE; /* bad rcode */

  qdcount = doh_get16bit(doh, 4);
  ଯେତେବେଳେ(qdcount) {
    rc = doh_skipqname(doh, dohlen, &index);
    ଯଦି(rc)
      ଫେରନ୍ତୁ rc; /* bad qname */
    ଯଦି(dohlen < (index + 4))
      ଫେରନ୍ତୁ DOH_DNS_OUT_OF_RANGE;
    index += 4; /* skip question's type and class */
    qdcount--;
  }

  ancount = doh_get16bit(doh, 6);
  ଯେତେବେଳେ(ancount) {
    unsigned short dnsclass;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ttl;

    rc = doh_skipqname(doh, dohlen, &index);
    ଯଦି(rc)
      ଫେରନ୍ତୁ rc; /* bad qname */

    ଯଦି(dohlen < (index + 2))
      ଫେରନ୍ତୁ DOH_DNS_OUT_OF_RANGE;

    type = doh_get16bit(doh, index);
    ଯଦି((type != CURL_DNS_TYPE_CNAME) &&  /* may be synthesized from DNAME */
       (type != CURL_DNS_TYPE_DNAME) &&  /* if present, accept and ignore */
       (type != dnstype))
      /* Not the same type as was asked for, nor CNAME nor DNAME */
      ଫେରନ୍ତୁ DOH_DNS_UNEXPECTED_TYPE;
    index += 2;

    ଯଦି(dohlen < (index + 2))
      ଫେରନ୍ତୁ DOH_DNS_OUT_OF_RANGE;
    dnsclass = doh_get16bit(doh, index);
    ଯଦି(DNS_CLASS_IN != dnsclass)
      ଫେରନ୍ତୁ DOH_DNS_UNEXPECTED_CLASS; /* unsupported */
    index += 2;

    ଯଦି(dohlen < (index + 4))
      ଫେରନ୍ତୁ DOH_DNS_OUT_OF_RANGE;

    ttl = doh_get32bit(doh, index);
    ଯଦି(ttl > MAX_DNS_TTL)
      ttl = MAX_DNS_TTL;
    ଯଦି(ttl < d->ttl)
      d->ttl = ttl;
    index += 4;

    ଯଦି(dohlen < (index + 2))
      ଫେରନ୍ତୁ DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    ଯଦି(dohlen < (index + rdlength))
      ଫେରନ୍ତୁ DOH_DNS_OUT_OF_RANGE;

    rc = doh_rdata(doh, rdlength, type, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)index, d);
    ଯଦି(rc)
      ଫେରନ୍ତୁ rc;
    index += rdlength;
    ancount--;
  }

  nscount = doh_get16bit(doh, 8);
  ଯେତେବେଳେ(nscount) {
    rc = doh_skipqname(doh, dohlen, &index);
    ଯଦି(rc)
      ଫେରନ୍ତୁ rc; /* bad qname */

    ଯଦି(dohlen < (index + 8))
      ଫେରନ୍ତୁ DOH_DNS_OUT_OF_RANGE;

    index += 2 + 2 + 4; /* type, dnsclass and ttl */

    ଯଦି(dohlen < (index + 2))
      ଫେରନ୍ତୁ DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    ଯଦି(dohlen < (index + rdlength))
      ଫେରନ୍ତୁ DOH_DNS_OUT_OF_RANGE;
    index += rdlength;
    nscount--;
  }

  arcount = doh_get16bit(doh, 10);
  ଯେତେବେଳେ(arcount) {
    rc = doh_skipqname(doh, dohlen, &index);
    ଯଦି(rc)
      ଫେରନ୍ତୁ rc; /* bad qname */

    ଯଦି(dohlen < (index + 8))
      ଫେରନ୍ତୁ DOH_DNS_OUT_OF_RANGE;

    index += 2 + 2 + 4; /* type, dnsclass and ttl */

    ଯଦି(dohlen < (index + 2))
      ଫେରନ୍ତୁ DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    ଯଦି(dohlen < (index + rdlength))
      ଫେରନ୍ତୁ DOH_DNS_OUT_OF_RANGE;
    index += rdlength;
    arcount--;
  }

  ଯଦି(index != dohlen)
    ଫେରନ୍ତୁ DOH_DNS_MALFORMAT; /* something is wrong */

  ଫେରନ୍ତୁ DOH_OK; /* ok */
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
ସ୍ଥିର CURLcode doh2ai(ଅଚଳ ସଂରଚନା dohentry *de, ଅଚଳ ଅକ୍ଷର *hostname,
                       ପୂର୍ଣ୍ଣସଂଖ୍ୟା port, ସଂରଚନା Curl_addrinfo **aip)
{
  ସଂରଚନା Curl_addrinfo *ai;
  ସଂରଚନା Curl_addrinfo *prevai = ଖାଲି;
  ସଂରଚନା Curl_addrinfo *firstai = ଖାଲି;
  ସଂରଚନା sockaddr_in *addr;
#ifdef USE_IPV6
  ସଂରଚନା sockaddr_in6 *addr6;
#endif
  size_t hostlen = strlen(hostname) + 1; /* include null-terminator */
  CURLcode result = CURLE_OK;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

  ପାଇଁ(i = 0; i < de->numaddr; i++) {
    size_t ss_size;
    CURL_SA_FAMILY_T addrtype;
    ଯଦି(de->addr[i].type == CURL_DNS_TYPE_AAAA) {
#ifndef USE_IPV6
      /* we cannot handle IPv6 addresses */
      ଜାରି;
#ଅନ୍ୟଥା
      ss_size = ଆକାର(ସଂରଚନା sockaddr_in6);
      addrtype = AF_INET6;
#endif
    }
    ଅନ୍ୟଥା {
      ss_size = ଆକାର(ସଂରଚନା sockaddr_in);
      addrtype = AF_INET;
    }

    ai = curlx_calloc(1, ଆକାର(ସଂରଚନା Curl_addrinfo) + ss_size + hostlen);
    ଯଦି(!ai) {
      result = CURLE_OUT_OF_MEMORY;
      ଭାଙ୍ଗ;
    }
    ai->ai_addr = (ଶୂନ୍ୟ *)((ଅକ୍ଷର *)ai + ଆକାର(ସଂରଚନା Curl_addrinfo));
    ai->ai_canonname = (ଶୂନ୍ୟ *)((ଅକ୍ଷର *)ai->ai_addr + ss_size);
    memcpy(ai->ai_canonname, hostname, hostlen);

    ଯଦି(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    ଯଦି(prevai)
      /* make the previous entry point to this */
      prevai->ai_next = ai;

    ai->ai_family = addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_size;

    /* leave the rest of the struct filled with zero */

    ବାଛ(ai->ai_family) {
    ସ୍ଥିତି AF_INET:
      addr = (ଶୂନ୍ୟ *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(ଆକାର(ସଂରଚନା in_addr) == ଆକାର(de->addr[i].ip.v4));
      memcpy(&addr->sin_addr, &de->addr[i].ip.v4, ଆକାର(ସଂରଚନା in_addr));
      addr->sin_family = addrtype;
      addr->sin_port = htons((unsigned short)port);
      ଭାଙ୍ଗ;

#ifdef USE_IPV6
    ସ୍ଥିତି AF_INET6:
      addr6 = (ଶୂନ୍ୟ *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(ଆକାର(ସଂରଚନା in6_addr) == ଆକାର(de->addr[i].ip.v6));
      memcpy(&addr6->sin6_addr, &de->addr[i].ip.v6, ଆକାର(ସଂରଚନା in6_addr));
      addr6->sin6_family = addrtype;
      addr6->sin6_port = htons((unsigned short)port);
      ଭାଙ୍ଗ;
#endif
    }

    prevai = ai;
  }

  ଯଦି(result) {
    Curl_freeaddrinfo(firstai);
    firstai = ଖାଲି;
  }
  *aip = firstai;

  ଫେରନ୍ତୁ result;
}

/* @unittest 1655 */
UNITTEST ଶୂନ୍ୟ de_cleanup(ସଂରଚନା dohentry *d);
UNITTEST ଶୂନ୍ୟ de_cleanup(ସଂରଚନା dohentry *d)
{
#ifdef USE_HTTPSRR
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;
  ପାଇଁ(i = 0; i < d->numhttps_rrs; i++)
    curlx_safefree(d->https_rrs[i].val);
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)d;
#endif
}

#ifdef USE_HTTPSRR

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
ସ୍ଥିର CURLcode doh_decode_rdata_name(ଅଚଳ unsigned ଅକ୍ଷର **buf,
                                      size_t *remaining, ଅକ୍ଷର **dnsname)
{
  ଅଚଳ unsigned ଅକ୍ଷର *cp = ଖାଲି;
  size_t rem = 0;
  unsigned ଅକ୍ଷର clen = 0; /* chunk len */
  ସଂରଚନା dynbuf thename;

  DEBUGASSERT(buf && remaining && dnsname);
  ଯଦି(!buf || !remaining || !dnsname || !*remaining)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&thename, CURL_MAXLEN_HOST_NAME);
  rem = *remaining;
  cp = *buf;
  clen = *cp++;
  /* RFC 9460 says it must be uncompressed */
  ଯଦି(clen > 63)
    ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;

  ଯଦି(clen == 0) {
    /* special case - return "." as name */
    ଯଦି(curlx_dyn_addn(&thename, ".", 1))
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }
  ଯେତେବେଳେ(clen) {
    ଯଦି(clen >= rem) {
      curlx_dyn_free(&thename);
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }
    ଯଦି(curlx_dyn_addn(&thename, cp, clen) ||
       curlx_dyn_addn(&thename, ".", 1))
      ଫେରନ୍ତୁ CURLE_TOO_LARGE;

    cp += clen;
    rem -= (clen + 1);
    ଯଦି(rem <= 0) {
      curlx_dyn_free(&thename);
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }
    clen = *cp++;
    ଯଦି(clen > 63) {
      /* invalid format */
      curlx_dyn_free(&thename);
      ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
    }
  }
  *buf = cp;
  *remaining = rem - 1;
  *dnsname = curlx_dyn_ptr(&thename);
  ଫେରନ୍ତୁ CURLE_OK;
}

/* scan for byte values <= 32 or 127 */
ସ୍ଥିର CURLcode junkscan(ଅଚଳ ଅକ୍ଷର *url)
{
  ଅଚଳ unsigned ଅକ୍ଷର *p = (ଅଚଳ unsigned ଅକ୍ଷର *)url;
  ଯେତେବେଳେ(*p) {
    ଯଦି(*p <= 0x20 || *p == 127)
      ଫେରନ୍ତୁ CURLE_WEIRD_SERVER_REPLY;
    p++;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

/* @unittest 1658 */
UNITTEST CURLcode doh_resp_decode_httpsrr(ସଂରଚନା Curl_easy *data,
                                          ଅଚଳ unsigned ଅକ୍ଷର *cp, size_t len,
                                          ସଂରଚନା Curl_https_rrinfo **hrr);
UNITTEST CURLcode doh_resp_decode_httpsrr(ସଂରଚନା Curl_easy *data,
                                          ଅଚଳ unsigned ଅକ୍ଷର *cp, size_t len,
                                          ସଂରଚନା Curl_https_rrinfo **hrr)
{
  uint16_t pcode = 0, plen = 0;
  uint32_t expected_min_pcode = 0;
  ସଂରଚନା Curl_https_rrinfo *lhrr = ଖାଲି;
  ଅକ୍ଷର *dnsname = ଖାଲି;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (ଶୂନ୍ୟ)data;
  *hrr = ଖାଲି;
  ଯଦି(len <= 2)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  lhrr = curlx_calloc(1, ଆକାର(ସଂରଚନା Curl_https_rrinfo));
  ଯଦି(!lhrr)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  lhrr->priority = doh_get16bit(cp, 0);
  cp += 2;
  len -= 2;
  ଯଦି(doh_decode_rdata_name(&cp, &len, &dnsname) != CURLE_OK)
    ଯାଅ err;
  lhrr->target = dnsname;
  result = junkscan(dnsname);
  ଯଦି(result) {
    /* unacceptable hostname content */
    ଯାଅ err;
  }
  ଯେତେବେଳେ(len >= 4) {
    pcode = doh_get16bit(cp, 0);
    plen = doh_get16bit(cp, 2);
    cp += 4;
    len -= 4;
    ଯଦି(pcode < expected_min_pcode || plen > len) {
      result = CURLE_WEIRD_SERVER_REPLY;
      ଯାଅ err;
    }
    result = Curl_httpsrr_set(lhrr, pcode, cp, plen);
    ଯଦି(result)
      ଯାଅ err;
    Curl_httpsrr_trace(data, lhrr);
    cp += plen;
    len -= plen;
    expected_min_pcode = pcode + 1;
  }
  *hrr = lhrr;
  ଫେରନ୍ତୁ CURLE_OK;
err:
  Curl_httpsrr_destroy(lhrr);
  ଫେରନ୍ତୁ result;
}

#endif /* USE_HTTPSRR */

/* called from multi when a sub transfer, e.g. doh probe, is done.
 * Parse the response and set the results in the `async` context
 * of master, using the id from the probe's CURL_EZM_DOH_PROBE
 * meta data. */
ସ୍ଥିର ଶୂନ୍ୟ doh_probe_done(ସଂରଚନା Curl_easy *doh,
                           ସଂରଚନା Curl_easy *master, CURLcode result)
{
  ସଂରଚନା Curl_resolv_async *async = ଖାଲି;
  ସଂରଚନା doh_probes *dohp = ଖାଲି;
  ସଂରଚନା doh_request *doh_req = ଖାଲି;
  ସଂରଚନା Curl_addrinfo **pdest_ai;
  ସଂରଚନା dohentry de;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା slot, httpcode;

  de_init(&de);
  doh_req = Curl_meta_get(doh, CURL_EZM_DOH_PROBE);
  ଯଦି(!doh_req) {
    /* transfer `doh` is not a DoH probe. */
    DEBUGASSERT(0);
    ଯାଅ out;
  }

  async = Curl_async_get(master, doh_req->resolv_id);
  ଯଦି(!async) {
    CURL_TRC_DNS(master, "[%u] ignoring outdated DoH response",
                 doh_req->resolv_id);
    ଯାଅ out;
  }
  dohp = async->doh;

  ପାଇଁ(slot = 0; slot < DOH_SLOT_ଗଣନା; ++slot) {
    ଯଦି(dohp->probe_mid[slot] == doh->mid)
      ଭାଙ୍ଗ;
  }
  /* We really should have found the slot where to store the response */
  ଯଦି(slot >= DOH_SLOT_ଗଣନା) {
    failf(master, "DoH: unknown sub request done");
    DEBUGASSERT(0);
    ଯାଅ out;
  }

  async->queries_ongoing--;
  dohp = async->doh;
  httpcode = doh->info.httpcode;
  ବାଛ(slot) {
  ସ୍ଥିତି DOH_SLOT_IPV4:
    async->dns_responses |= CURL_DNSQ_A;
    ଭାଙ୍ଗ;
#ifdef USE_IPV6
  ସ୍ଥିତି DOH_SLOT_IPV6:
    async->dns_responses |= CURL_DNSQ_AAAA;
    ଭାଙ୍ଗ;
#endif
#ifdef USE_HTTPSRR
  ସ୍ଥିତି DOH_SLOT_HTTPS_RR:
    async->dns_responses |= CURL_DNSQ_HTTPS;
    ଭାଙ୍ଗ;
#endif
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    DEBUGASSERT(0);
    ଭାଙ୍ଗ;
  }

  ଯଦି(result) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: %s",
          doh_type2name(doh_req->dnstype), curl_easy_strerror(result));
    ଯାଅ out;
  }
  ଅନ୍ୟଥା ଯଦି((httpcode < 200) || (httpcode >= 300)) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: HTTP status %d",
          doh_type2name(doh_req->dnstype), httpcode);
    ଯାଅ out;
  }

  dohp->probe_rc[slot] = doh_resp_decode(curlx_dyn_uptr(&doh_req->resp_body),
                                         curlx_dyn_len(&doh_req->resp_body),
                                         doh_req->dnstype, &de);
  ଯଦି(dohp->probe_rc[slot]) {
#ifdef USE_HTTPSRR
    ଯଦି((dohp->probe_rc[slot] == DOH_NO_CONTENT) &&
       (doh_req->dnstype == CURL_DNS_TYPE_HTTPS)) {
      dohp->probe_rc[slot] = DOH_DNS_NXDOMAIN;
    }
#endif
    infof(doh, "[DoH] [%s] error decoding response: %s",
          doh_type2name(doh_req->dnstype),
          doh_strerror(dohp->probe_rc[slot]));
    ଯାଅ out;
  }

  ଯଦି(doh_req->dnstype == CURL_DNS_TYPE_A)
    pdest_ai = &async->ai_A;
  ଅନ୍ୟଥା ଯଦି(doh_req->dnstype == CURL_DNS_TYPE_AAAA)
    pdest_ai = &async->ai_AAAA;
  ଅନ୍ୟଥା
    pdest_ai = ଖାଲି;

  ଯଦି(pdest_ai && de.numaddr) {
    ଯଦି(*pdest_ai) {
      Curl_freeaddrinfo(*pdest_ai);
      *pdest_ai = ଖାଲି;
    }
    result = doh2ai(&de, async->peer->hostname, async->peer->port, pdest_ai);
    ଯଦି(result) { /* hard failure on our side, fail completely */
      infof(doh, "[DoH] [%s] error creating addrinfo: %s",
            doh_type2name(doh_req->dnstype), curl_easy_strerror(result));
      dohp->probe_rc[slot] = DOH_OOM;
      async->result = result;
    }
  }
#ifdef USE_HTTPSRR
  ଅନ୍ୟଥା ଯଦି((doh_req->dnstype == CURL_DNS_TYPE_HTTPS) && de.numhttps_rrs) {
    CURL_TRC_DNS(doh, "[HTTPS] got %d records", de.numhttps_rrs);
    result = doh_resp_decode_httpsrr(doh, de.https_rrs->val,
                                     de.https_rrs->len, &async->httpsrr);
    ଯଦି(result) {
      dohp->probe_rc[slot] = DOH_HTTP_FAILED;
      infof(doh, "[DoH] error decoding HTTPS RR: %s",
            curl_easy_strerror(result));
      ଯାଅ out;
    }
  }
#endif /* USE_HTTPSRR */

  /* DoH request complete, run master to act on results */
  infof(doh, "DoH request complete, %u to go", async->queries_ongoing);

out:
  Curl_multi_mark_dirty(master);
  de_cleanup(&de);
  Curl_meta_remove(doh, CURL_EZM_DOH_PROBE);
}

CURLcode Curl_doh_take_result(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା Curl_resolv_async *async,
                              ସଂରଚନା Curl_dns_entry **pdns)
{
  ସଂରଚନା doh_probes *dohp = async->doh;
  CURLcode result = CURLE_OK;

  *pdns = ଖାଲି; /* defaults to no response */
  ଯଦି(!dohp)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  async->negative_answer = FALSE;
  ଯଦି(async->result) {
    result = async->result;
    ଯାଅ out;
  }

  ଯଦି(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     dohp->probe_mid[DOH_SLOT_IPV4] == UINT32_MAX &&
     dohp->probe_mid[DOH_SLOT_IPV6] == UINT32_MAX) {
    failf(data, "Could not DoH-resolve: %s", async->peer->hostname);
    ଫେରନ୍ତୁ async->for_proxy ?
      CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
  }
  ଅନ୍ୟଥା ଯଦି(!async->queries_ongoing) {
    ସଂରଚନା Curl_dns_entry *dns = ଖାଲି;
    bool negative = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା slot;

    /* remove DoH handles from multi handle and close them */
    doh_close(data, async);
    /* parse the responses, create the struct and return it! */
    ପାଇଁ(slot = 0; slot < DOH_SLOT_ଗଣନା; slot++) {
      /* Failing without an NXDOMAIN answer - a SERVFAIL-class rcode or
         an undecodable response - says nothing about the name. Such a
         failure must not be cached as a negative entry. */
      ଯଦି(dohp->probe_rc[slot] && (dohp->probe_rc[slot] != DOH_DNS_NXDOMAIN))
        negative = FALSE;
    } /* next slot */

    ଯଦି(async->ai_A || async->ai_AAAA) {
      dns = Curl_dnsc_mk_addr2(
        data, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
      ଯଦି(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        ଯାଅ out;
      }
    }
#ifdef USE_HTTPSRR
    ଅନ୍ୟଥା ଯଦି((async->dns_queries & CURL_DNSQ_HTTPS) &&
            !dohp->probe_rc[DOH_SLOT_HTTPS_RR]) {
      Curl_httpsrr_trace(data, async->httpsrr);
      dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
      ଯଦି(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        ଯାଅ out;
      }
    }
#endif /* USE_HTTPSRR */
    ଅନ୍ୟଥା {
      /* every query failed. Only NXDOMAIN answers for all of them
         make this a negative answer, eligible for caching. */
      async->negative_answer = negative;
      result = async->for_proxy ?
        CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
    }

    /* and add the entry to the cache */
    ଯଦି(dns)
      result = Curl_dnscache_add(data, dns);
    *pdns = dns;
  } /* !async->queries_ongoing */
  ଅନ୍ୟଥା
    /* wait for pending DoH transactions to complete */
    ଫେରନ୍ତୁ CURLE_AGAIN;

out:
  Curl_doh_cleanup(data, async);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ doh_close(ସଂରଚନା Curl_easy *data,
                      ସଂରଚନା Curl_resolv_async *async)
{
  ସଂରଚନା doh_probes *doh = async ? async->doh : ଖାଲି;
  ଯଦି(doh && data->multi) {
    ସଂରଚନା Curl_easy *probe_data;
    uint32_t mid;
    size_t slot;
    ପାଇଁ(slot = 0; slot < DOH_SLOT_ଗଣନା; slot++) {
      mid = doh->probe_mid[slot];
      ଯଦି(mid == UINT32_MAX)
        ଜାରି;
      doh->probe_mid[slot] = UINT32_MAX;
      /* should have been called before data is removed from multi handle */
      DEBUGASSERT(data->multi);
      probe_data = data->multi ? Curl_multi_get_easy(data->multi, mid) : ଖାଲି;
      ଯଦି(!probe_data) {
        DEBUGF(infof(data, "Curl_doh_close: xfer for mid=%u not found!", mid));
        ଜାରି;
      }
      probe_data->sub_xfer_done = ଖାଲି; /* No longer interested in result */
      /* data->multi might already be reset at this time */
      Curl_multi_remove_handle(data->multi, probe_data);
      Curl_close(&probe_data);
    }
    CURL_TRC_DNS(data, "[DoH] probe done");
  }
}

ଶୂନ୍ୟ Curl_doh_cleanup(ସଂରଚନା Curl_easy *data,
                      ସଂରଚନା Curl_resolv_async *async)
{
  ସଂରଚନା doh_probes *dohp = async->doh;
  ଯଦି(dohp) {
    doh_close(data, async);
    curlx_safefree(async->doh);
  }
}

#endif /* CURL_DISABLE_DOH */
