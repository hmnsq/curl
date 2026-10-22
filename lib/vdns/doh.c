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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_DOH

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "curl_addrinfo.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "vdns/doh.h"
#అన్తర్భావ "vdns/httpsrr.h"
#అన్తర్భావ "curlx/strdup.h"
#అన్తర్భావ "curlx/dynbuf.h"
#అన్తర్భావ "escape.h"  /* for Curl_hexencode() */
#అన్తర్భావ "urlapi-int.h"

#నిర్వచించు DNS_CLASS_లోపల 0x01

స్థిర శూన్య doh_close(నిర్మాణం Curl_easy *దత్తాంశం,
                      నిర్మాణం Curl_resolv_async *async);

#నిర్వచితమైతే CURLVERBOSE
స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి doh_సంకేతం_str[] = {
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

స్థిర స్థిరరాశి అక్షరం *doh_strerror(DOHcode సంకేతం_2)
{
  ఒకవేళ((size_t)సంకేతం_2 < CURL_ARRAYSIZE(doh_సంకేతం_str))
    తిరిగి doh_సంకేతం_str[సంకేతం_2];
  తిరిగి "bad error code";
}

స్థిర స్థిరరాశి అక్షరం *doh_type2name(DNStype dnstype)
{
  ఎంపిక(dnstype) {
  స్థితి CURL_DNS_TYPE_A:
    తిరిగి "A";
  స్థితి CURL_DNS_TYPE_AAAA:
    తిరిగి "AAAA";
#నిర్వచితమైతే USE_HTTPSRR
  స్థితి CURL_DNS_TYPE_HTTPS:
    తిరిగి "HTTPS";
#అయితే_ముగింపు
  అప్రమేయ:
    తిరిగి "unknown";
  }
}

#అయితే_ముగింపు /* CURLVERBOSE */

/* @unittest 1655
 */
UNITTEST DOHcode doh_req_encode(స్థిరరాశి అక్షరం *host,
                                DNStype dnstype,
                                గుర్తులేని అక్షరం *dnsp,  /*బఫరు*/
                                size_t len,  /* buffer size */
                                size_t *olen);  /* output length */
UNITTEST DOHcode doh_req_encode(స్థిరరాశి అక్షరం *host,
                                DNStype dnstype,
                                గుర్తులేని అక్షరం *dnsp, /*బఫరు*/
                                size_t len,   /* buffer size */
                                size_t *olen) /* output length */
{
  స్థిరరాశి size_t hostlen = అక్షరశ్రేణి_పొడవు(host);
  గుర్తులేని అక్షరం *orig = dnsp;
  స్థిరరాశి అక్షరం *hostp = host;

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
  ఒకవేళ(host[hostlen - 1] != '.')
    expected_len++;

  ఒకవేళ(expected_len > DOH_MAX_DNSREQ_కొలత)
    తిరిగి DOH_DNS_NAME_TOO_LONG;

  ఒకవేళ(len < expected_len)
    తిరిగి DOH_TOO_SMALL_బఫరు;

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
  అయితే(*hostp) {
    size_t labellen;
    స్థిరరాశి అక్షరం *dot = strchr(hostp, '.');
    ఒకవేళ(dot)
      labellen = dot - hostp;
    లేకపోతే
      labellen = అక్షరశ్రేణి_పొడవు(hostp);
    ఒకవేళ((labellen > 63) || (!labellen)) {
      /* label is too long or too short, error out */
      *olen = 0;
      తిరిగి DOH_DNS_BAD_LABEL;
    }
    /* label is non-empty, process it */
    *dnsp++ = (గుర్తులేని అక్షరం)labellen;
    జ్ఞాపకం_నకలు(dnsp, hostp, labellen);
    dnsp += labellen;
    hostp += labellen;
    /* advance past dot, but only if there is one */
    ఒకవేళ(dot)
      hostp++;
  } /* next label */

  *dnsp++ = 0; /* append zero-length label for root */

  /* There are assigned TYPE codes beyond 255: use range [1..65535] */
  *dnsp++ = (గుర్తులేని అక్షరం)(255 & (dnstype >> 8)); /* upper 8-bit TYPE */
  *dnsp++ = (గుర్తులేని అక్షరం)(255 & dnstype);        /* lower 8-bit TYPE */

  *dnsp++ = '\0'; /* upper 8-bit CLASS */
  *dnsp++ = DNS_CLASS_లోపల; /* IN - "the Internet" */

  *olen = dnsp - orig;

  /* verify that our estimation of length is valid, since
   * this has led to buffer overflows in this function */
  DEBUGASSERT(*olen == expected_len);
  తిరిగి DOH_OK;
}

స్థిర size_t doh_probe_write_cb(అక్షరం *contents, size_t కొలత, size_t nmemb,
                                 శూన్య *userp)
{
  size_t realsize = కొలత * nmemb;
  నిర్మాణం Curl_easy *దత్తాంశం = userp;
  నిర్మాణం doh_request *doh_req = Curl_meta_get(దత్తాంశం, CURL_EZM_DOH_PROBE);
  ఒకవేళ(!doh_req)
    తిరిగి CURL_WRITEFUNC_ERROR;

  ఒకవేళ(curlx_dyn_addn(&doh_req->resp_body, contents, realsize))
    తిరిగి 0;

  తిరిగి realsize;
}

స్థిర శూన్య doh_probe_పూర్తి(నిర్మాణం Curl_easy *doh,
                           నిర్మాణం Curl_easy *master, CURLcode ఫలితం);
స్థిర శూన్య doh_probe_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *e)
{
  (శూన్య)కీలకం;
  (శూన్య)klen;
  ఒకవేళ(e) {
    నిర్మాణం doh_request *doh_req = e;
    curl_slist_free_all(doh_req->req_hds);
    curlx_dyn_free(&doh_req->resp_body);
    curlx_free(e);
  }
}

#నిర్వచించు ERROR_CHECK_SETOPT(x, y)                        \
  చేయి {                                                  \
    ఫలితం = curl_easy_setopt((CURL *)doh, x, y);       \
    ఒకవేళ(ఫలితం &&                                        \
       ఫలితం != CURLE_NOT_BUILT_లోపల &&                  \
       ఫలితం != CURLE_UNKNOWN_OPTION)                  \
      వెళ్ళు error;                                       \
  } అయితే(0)

స్థిర CURLcode doh_probe_నిరంతరం(నిర్మాణం Curl_easy *దత్తాంశం,
                              DNStype dnstype,
                              స్థిరరాశి అక్షరం *host,
                              స్థిరరాశి అక్షరం *url, CURLM *multi,
                              uint32_t resolv_id,
                              uint32_t *pmid)
{
  నిర్మాణం Curl_easy *doh = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;
  timediff_t timeout_ms;
  నిర్మాణం doh_request *doh_req;
  DOHcode d;
  bool maybe_https = !curl_strnequal(url, STRCONST("http:"));

  *pmid = UINT32_MAX;

  doh_req = curlx_calloc(1, పరిమాణం(*doh_req));
  ఒకవేళ(!doh_req)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  doh_req->resolv_id = resolv_id;
  doh_req->dnstype = dnstype;
  curlx_dyn_init(&doh_req->resp_body, DYN_DOH_RESPONSE);

  d = doh_req_encode(host, dnstype, doh_req->req_body,
                     పరిమాణం(doh_req->req_body),
                     &doh_req->req_body_len);
  ఒకవేళ(d) {
    failf(దత్తాంశం, "Failed to encode DoH packet [%d]", (పూర్ణాంకం)d);
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు error;
  }

  timeout_ms = Curl_timeleft_ms(దత్తాంశం);
  ఒకవేళ(timeout_ms < 0) {
    ఫలితం = CURLE_OPERATION_TIMEDOUT;
    వెళ్ళు error;
  }

  doh_req->req_hds =
    curl_slist_append(ఖాళీ, "Content-Type: application/dns-message");
  ఒకవేళ(!doh_req->req_hds) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు error;
  }

  /* Curl_open() is the internal version of curl_easy_init() */
  ఫలితం = Curl_open(&doh);
  ఒకవేళ(ఫలితం)
    వెళ్ళు error;

  /* pass in the struct pointer via a local variable to please coverity and
     the gcc typecheck helpers */
  VERBOSE(doh->state.feat = &Curl_trc_feat_doh);
  ERROR_CHECK_SETOPT(CURLOPT_URL, url);
  ERROR_CHECK_SETOPT(CURLOPT_DEFAULT_PROTOCOL, "https");
  ERROR_CHECK_SETOPT(CURLOPT_WRITEFUNCTION, doh_probe_write_cb);
  ERROR_CHECK_SETOPT(CURLOPT_WRITEDATA, doh);
  ERROR_CHECK_SETOPT(CURLOPT_POSTFIELDS, doh_req->req_body);
  ERROR_CHECK_SETOPT(CURLOPT_POSTFIELDSIZE, (దీర్ఘ)doh_req->req_body_len);
  ERROR_CHECK_SETOPT(CURLOPT_HTTPHEADER, doh_req->req_hds);
#నిర్వచితమైతే USE_HTTP2
  ఒకవేళ(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_HTTP_సంస్కరణ, CURL_HTTP_సంస్కరణ_2_TLS);
    ERROR_CHECK_SETOPT(CURLOPT_PIPEWAIT, 1L);
  }
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే DEBUGBUILD
  /* enforce HTTPS if not debug */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
#లేకపోతే
  /* in debug mode, also allow http */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
#అయితే_ముగింపు
  ERROR_CHECK_SETOPT(CURLOPT_TIMEOUT_MS, (దీర్ఘ)timeout_ms);
  ERROR_CHECK_SETOPT(CURLOPT_SHARE, (CURLSH *)దత్తాంశం->share);
  ఒకవేళ(దత్తాంశం->సమితి.err && దత్తాంశం->సమితి.err != stderr)
    ERROR_CHECK_SETOPT(CURLOPT_STDERR, దత్తాంశం->సమితి.err);
  ఒకవేళ(Curl_trc_ft_is_verbose(దత్తాంశం, &Curl_trc_feat_doh))
    ERROR_CHECK_SETOPT(CURLOPT_VERBOSE, 1L);
  ఒకవేళ(దత్తాంశం->సమితి.no_signal)
    ERROR_CHECK_SETOPT(CURLOPT_NOSIGNAL, 1L);
  ఒకవేళ(దత్తాంశం->సమితి.fdebug)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGFUNCTION, దత్తాంశం->సమితి.fdebug);
  ఒకవేళ(దత్తాంశం->సమితి.debugdata)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGDATA, దత్తాంశం->సమితి.debugdata);

#నిర్వచితమైతే USE_RESOLV_ARES
  ఒకవేళ(CURL_EASY_STR(దత్తాంశం, STRING_DNS_SERVERS)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_SERVERS,
                       CURL_EASY_STR(దత్తాంశం, STRING_DNS_SERVERS));
  }
  ఒకవేళ(CURL_EASY_STR(దత్తాంశం, STRING_DNS_INTERFACE)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_INTERFACE,
                       CURL_EASY_STR(దత్తాంశం, STRING_DNS_INTERFACE));
  }
  ఒకవేళ(CURL_EASY_STR(దత్తాంశం, STRING_DNS_LOCAL_IP4)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP4,
                       CURL_EASY_STR(దత్తాంశం, STRING_DNS_LOCAL_IP4));
  }
  ఒకవేళ(CURL_EASY_STR(దత్తాంశం, STRING_DNS_LOCAL_IP6)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP6,
                       CURL_EASY_STR(దత్తాంశం, STRING_DNS_LOCAL_IP6));
  }
#అయితే_ముగింపు
  ఒకవేళ(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYHOST,
                       దత్తాంశం->సమితి.doh_verifyhost ? 2L : 0L);
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYPEER,
                       దత్తాంశం->సమితి.doh_verifypeer ? 1L : 0L);
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYSTATUS,
                       దత్తాంశం->సమితి.doh_verifystatus ? 1L : 0L);

    /* Inherit *some* SSL options from the user's transfer. This is a
       best-guess as to which options are needed for compatibility. #3661

       Note DoH does not inherit the user's proxy server so proxy SSL settings
       have no effect and are not inherited. If that changes then two new
       options should be added to check doh proxy insecure separately,
       CURLOPT_DOH_PROXY_SSL_VERIFYHOST and CURLOPT_DOH_PROXY_SSL_VERIFYPEER.
       */
    doh->సమితి.ssl.custom_cafile = దత్తాంశం->సమితి.ssl.custom_cafile;
    doh->సమితి.ssl.custom_capath = దత్తాంశం->సమితి.ssl.custom_capath;
    doh->సమితి.ssl.custom_cablob = దత్తాంశం->సమితి.ssl.custom_cablob;
    ఒకవేళ(CURL_EASY_STR(దత్తాంశం, STRING_SSL_CAFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO,
                         CURL_EASY_STR(దత్తాంశం, STRING_SSL_CAFILE));
    }
    ఒకవేళ(దత్తాంశం->సమితి.blobs[BLOB_CAINFO]) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO_BLOB, దత్తాంశం->సమితి.blobs[BLOB_CAINFO]);
    }
    ఒకవేళ(CURL_EASY_STR(దత్తాంశం, STRING_SSL_CAPATH)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAPATH,
                         CURL_EASY_STR(దత్తాంశం, STRING_SSL_CAPATH));
    }
    ఒకవేళ(CURL_EASY_STR(దత్తాంశం, STRING_SSL_CRLFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CRLFILE,
                         CURL_EASY_STR(దత్తాంశం, STRING_SSL_CRLFILE));
    }
    ఒకవేళ(దత్తాంశం->సమితి.ssl.certinfo)
      ERROR_CHECK_SETOPT(CURLOPT_CERTINFO, 1L);
    ఒకవేళ(దత్తాంశం->సమితి.ssl_fsslctx)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_FUNCTION, దత్తాంశం->సమితి.ssl_fsslctx);
    ఒకవేళ(దత్తాంశం->సమితి.ssl_fsslctxp)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_దత్తాంశం, దత్తాంశం->సమితి.ssl_fsslctxp);
    ఒకవేళ(CURL_EASY_STR(దత్తాంశం, STRING_SSL_EC_CURVES)) {
      ERROR_CHECK_SETOPT(CURLOPT_SSL_EC_CURVES,
                         CURL_EASY_STR(దత్తాంశం, STRING_SSL_EC_CURVES));
    }
    (శూన్య)curl_easy_setopt(doh, CURLOPT_SSL_OPTIONS,
                           ((దీర్ఘ)దత్తాంశం->సమితి.ssl.ssl_options &
                            ~CURLSSLOPT_AUTO_CLIENT_CERT));
  }

  doh->state.అంతర్గత = TRUE;
  doh->master_mid = దత్తాంశం->mid; /* master transfer of this one */
  doh->sub_xfer_పూర్తి = doh_probe_పూర్తి;

  ఫలితం = Curl_meta_సమితి(doh, CURL_EZM_DOH_PROBE, doh_req, doh_probe_dtor);
  doh_req = ఖాళీ; /* call took ownership */
  ఒకవేళ(ఫలితం)
    వెళ్ళు error;

  /* DoH handles must not inherit private_data. The handles may be passed to
     the user via callbacks and the user will be able to identify them as
     internal handles because private data is not set. The user can then set
     private_data via CURLOPT_PRIVATE if they so choose. */
  DEBUGASSERT(!doh->సమితి.వ్యక్తిగత_దత్తాంశం);

  ఒకవేళ(Curl_multi_కూడు_handle(multi, doh))
    వెళ్ళు error;

  *pmid = doh->mid;
  తిరిగి CURLE_OK;

error:
  Curl_close(&doh);
  ఒకవేళ(doh_req)
    doh_probe_dtor(ఖాళీ, 0, doh_req);
  తిరిగి ఫలితం;
}

/*
 * Curl_doh() starts a name resolve using DoH. It resolves a name and returns
 * a 'Curl_addrinfo *' with the address information.
 */

CURLcode Curl_doh(నిర్మాణం Curl_easy *దత్తాంశం,
                  నిర్మాణం Curl_resolv_async *async)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం doh_probes *dohp = ఖాళీ;
  size_t i;

  DEBUGASSERT(!async->doh);
  DEBUGASSERT(async->peer->hostname[0]);
  ఒకవేళ(async->doh) {
    DEBUGASSERT(0); /* should not happen */
    Curl_doh_cleanup(దత్తాంశం, async);
  }

  ఒకవేళ(!async->dns_queries)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
#నిర్వచితమైతే USE_HTTPSRR
  ఒకవేళ(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     (async->dns_queries & CURL_DNSQ_HTTPS)) {
    /* Can't mix those in the same async resolve */
    DEBUGASSERT(0);
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  }
#లేకపోతే
  ఒకవేళ(async->dns_queries & CURL_DNSQ_HTTPS) {
    DEBUGASSERT(0);
    తిరిగి CURLE_NOT_BUILT_లోపల;
  }
#అయితే_ముగింపు

  /* start clean, consider allocating this struct on demand */
  async->doh = dohp = curlx_calloc(1, పరిమాణం(నిర్మాణం doh_probes));
  ఒకవేళ(!dohp)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  కొరకు(i = 0; i < DOH_స్థానం_లెక్క; ++i) {
    dohp->probe_rc[i] = DOH_OK;
    dohp->probe_mid[i] = UINT32_MAX;
  }

#నిర్వచితమైతే USE_IPV6
  /* AAAA results have preference in happy eyeballing, trigger first */
  ఒకవేళ(async->dns_queries & CURL_DNSQ_AAAA) {
    /* create IPv6 DoH request */
    ఫలితం = doh_probe_నిరంతరం(దత్తాంశం, CURL_DNS_TYPE_AAAA,
                           async->peer->hostname,
                           CURL_EASY_STR(దత్తాంశం, STRING_DOH),
                           దత్తాంశం->multi, async->id,
                           &dohp->probe_mid[DOH_స్థానం_IPV6]);
    ఒకవేళ(ఫలితం)
      వెళ్ళు error;
    async->queries_ongoing++;
  }
#అయితే_ముగింపు

  /* create IPv4 DoH request */
  ఒకవేళ(async->dns_queries & CURL_DNSQ_A) {
    ఫలితం = doh_probe_నిరంతరం(దత్తాంశం, CURL_DNS_TYPE_A,
                           async->peer->hostname,
                           CURL_EASY_STR(దత్తాంశం, STRING_DOH),
                           దత్తాంశం->multi, async->id,
                           &dohp->probe_mid[DOH_స్థానం_IPV4]);
    ఒకవేళ(ఫలితం)
      వెళ్ళు error;
    async->queries_ongoing++;
  }

#నిర్వచితమైతే USE_HTTPSRR
  ఒకవేళ(async->dns_queries & CURL_DNSQ_HTTPS) {
    అక్షరం *qname = ఖాళీ;
    ఒకవేళ(async->peer->పోర్టు != పోర్టు_HTTPS) {
      qname = curl_maprintf("_%u._https.%s",
                            async->peer->పోర్టు, async->peer->hostname);
      ఒకవేళ(!qname)
        వెళ్ళు error;
    }
    ఫలితం = doh_probe_నిరంతరం(దత్తాంశం, CURL_DNS_TYPE_HTTPS,
                           qname ? qname : async->peer->hostname,
                           CURL_EASY_STR(దత్తాంశం, STRING_DOH), దత్తాంశం->multi,
                           async->id,
                           &dohp->probe_mid[DOH_స్థానం_HTTPS_RR]);
    curlx_free(qname);
    ఒకవేళ(ఫలితం)
      వెళ్ళు error;
    async->queries_ongoing++;
  }
#అయితే_ముగింపు
  తిరిగి CURLE_OK;

error:
  Curl_doh_cleanup(దత్తాంశం, async);
  తిరిగి ఫలితం;
}

స్థిర DOHcode doh_skipqname(స్థిరరాశి గుర్తులేని అక్షరం *doh, size_t dohlen,
                             గుర్తులేని పూర్ణాంకం *indexp)
{
  గుర్తులేని అక్షరం నిడివి;
  చేయి {
    ఒకవేళ(dohlen < (*indexp + 1))
      తిరిగి DOH_DNS_బహిర్గతం_యొక్క_RANGE;
    నిడివి = doh[*indexp];
    ఒకవేళ((నిడివి & 0xc0) == 0xc0) {
      /* name pointer, advance over it and be done */
      ఒకవేళ(dohlen < (*indexp + 2))
        తిరిగి DOH_DNS_బహిర్గతం_యొక్క_RANGE;
      *indexp += 2;
      ఆపు;
    }
    ఒకవేళ(నిడివి & 0xc0)
      తిరిగి DOH_DNS_BAD_LABEL;
    ఒకవేళ(dohlen < (*indexp + 1 + నిడివి))
      తిరిగి DOH_DNS_బహిర్గతం_యొక్క_RANGE;
    *indexp += (గుర్తులేని పూర్ణాంకం)(1 + నిడివి);
  } అయితే(నిడివి);
  తిరిగి DOH_OK;
}

స్థిర గుర్తులేని పొట్టి doh_get16bit(స్థిరరాశి గుర్తులేని అక్షరం *doh,
                                   గుర్తులేని పూర్ణాంకం సూచీ)
{
  తిరిగి (గుర్తులేని పొట్టి)((doh[సూచీ] << 8) | doh[సూచీ + 1]);
}

స్థిర గుర్తులేని పూర్ణాంకం doh_get32bit(స్థిరరాశి గుర్తులేని అక్షరం *doh, గుర్తులేని పూర్ణాంకం సూచీ)
{
  /* make clang and gcc optimize this to bswap by incrementing
     the pointer first. */
  doh += సూచీ;

  /* avoid undefined behavior by casting to unsigned before shifting
     24 bits, possibly into the sign bit. codegen is same, but
     ub sanitizer will not be upset */
  తిరిగి ((గుర్తులేని)doh[0] << 24) | ((గుర్తులేని)doh[1] << 16) |
         ((గుర్తులేని)doh[2] << 8) | doh[3];
}

స్థిర శూన్య doh_store_a(స్థిరరాశి గుర్తులేని అక్షరం *doh, పూర్ణాంకం సూచీ,
                        నిర్మాణం dohentry *d)
{
  /* silently ignore addresses over the limit */
  ఒకవేళ(d->numaddr < DOH_MAX_ADDR) {
    నిర్మాణం dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_A;
    జ్ఞాపకం_నకలు(&a->ip.v4, &doh[సూచీ], 4);
    d->numaddr++;
  }
}

స్థిర శూన్య doh_store_aaaa(స్థిరరాశి గుర్తులేని అక్షరం *doh, పూర్ణాంకం సూచీ,
                           నిర్మాణం dohentry *d)
{
  /* silently ignore addresses over the limit */
  ఒకవేళ(d->numaddr < DOH_MAX_ADDR) {
    నిర్మాణం dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_AAAA;
    జ్ఞాపకం_నకలు(&a->ip.v6, &doh[సూచీ], 16);
    d->numaddr++;
  }
}

#నిర్వచితమైతే USE_HTTPSRR
స్థిర DOHcode doh_store_https(స్థిరరాశి గుర్తులేని అక్షరం *doh, పూర్ణాంకం సూచీ,
                               నిర్మాణం dohentry *d, uint16_t len)
{
  /* silently ignore RRs over the limit */
  ఒకవేళ(d->numhttps_rrs < DOH_MAX_HTTPS) {
    నిర్మాణం dohhttps_rr *h = &d->https_rrs[d->numhttps_rrs];
    h->val = curlx_memdup(&doh[సూచీ], len);
    ఒకవేళ(!h->val)
      తిరిగి DOH_బహిర్గతం_యొక్క_MEM;
    h->len = len;
    d->numhttps_rrs++;
  }
  తిరిగి DOH_OK;
}
#అయితే_ముగింపు

స్థిర DOHcode doh_rdata(స్థిరరాశి గుర్తులేని అక్షరం *doh,
                         గుర్తులేని పొట్టి rdlength,
                         గుర్తులేని పొట్టి type,
                         పూర్ణాంకం సూచీ,
                         నిర్మాణం dohentry *d)
{
  /* RDATA
     - A (TYPE 1): 4 bytes
     - AAAA (TYPE 28): 16 bytes
     - HTTPS (TYPE 65): N bytes */
  ఎంపిక(type) {
  స్థితి CURL_DNS_TYPE_A:
    ఒకవేళ(rdlength != 4)
      తిరిగి DOH_DNS_RDATA_LEN;
    doh_store_a(doh, సూచీ, d);
    ఆపు;
  స్థితి CURL_DNS_TYPE_AAAA:
    ఒకవేళ(rdlength != 16)
      తిరిగి DOH_DNS_RDATA_LEN;
    doh_store_aaaa(doh, సూచీ, d);
    ఆపు;
#నిర్వచితమైతే USE_HTTPSRR
  స్థితి CURL_DNS_TYPE_HTTPS:
    ఒకవేళ(rdlength < 3)
      తిరిగి DOH_DNS_RDATA_LEN;
    తిరిగి doh_store_https(doh, సూచీ, d, rdlength);
#అయితే_ముగింపు
  అప్రమేయ:
    /* unsupported type, or type we do not store, skip it */
    ఆపు;
  }
  తిరిగి DOH_OK;
}

/* @unittest 1655 */
UNITTEST శూన్య de_init(నిర్మాణం dohentry *de);
UNITTEST శూన్య de_init(నిర్మాణం dohentry *de)
{
  memset(de, 0, పరిమాణం(*de));
  de->ttl = గరిష్ఠ_పూర్ణాంకం;
}

/* TTL value cap */
#నిర్వచించు MAX_DNS_TTL 86400U /* 24 hours */
/* @unittest 1650 */
UNITTEST DOHcode doh_resp_decode(స్థిరరాశి గుర్తులేని అక్షరం *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 నిర్మాణం dohentry *d);
UNITTEST DOHcode doh_resp_decode(స్థిరరాశి గుర్తులేని అక్షరం *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 నిర్మాణం dohentry *d)
{
  గుర్తులేని అక్షరం rcode;
  గుర్తులేని పొట్టి qdcount;
  గుర్తులేని పొట్టి ancount;
  గుర్తులేని పొట్టి type = 0;
  గుర్తులేని పొట్టి rdlength;
  గుర్తులేని పొట్టి nscount;
  గుర్తులేని పొట్టి arcount;
  గుర్తులేని పూర్ణాంకం సూచీ = 12;
  DOHcode rc;

  ఒకవేళ(dohlen < 12)
    తిరిగి DOH_TOO_SMALL_బఫరు; /* too small */
  ఒకవేళ(!doh || doh[0] || doh[1])
    తిరిగి DOH_DNS_BAD_ID; /* bad ID */
  rcode = doh[3] & 0x0f;
  ఒకవేళ(rcode == 3)
    తిరిగి DOH_DNS_NXDOMAIN; /* name does not exist */
  ఒకవేళ(rcode)
    తిరిగి DOH_DNS_BAD_RCODE; /* bad rcode */

  qdcount = doh_get16bit(doh, 4);
  అయితే(qdcount) {
    rc = doh_skipqname(doh, dohlen, &సూచీ);
    ఒకవేళ(rc)
      తిరిగి rc; /* bad qname */
    ఒకవేళ(dohlen < (సూచీ + 4))
      తిరిగి DOH_DNS_బహిర్గతం_యొక్క_RANGE;
    సూచీ += 4; /* skip question's type and class */
    qdcount--;
  }

  ancount = doh_get16bit(doh, 6);
  అయితే(ancount) {
    గుర్తులేని పొట్టి dnsclass;
    గుర్తులేని పూర్ణాంకం ttl;

    rc = doh_skipqname(doh, dohlen, &సూచీ);
    ఒకవేళ(rc)
      తిరిగి rc; /* bad qname */

    ఒకవేళ(dohlen < (సూచీ + 2))
      తిరిగి DOH_DNS_బహిర్గతం_యొక్క_RANGE;

    type = doh_get16bit(doh, సూచీ);
    ఒకవేళ((type != CURL_DNS_TYPE_CNAME) &&  /* may be synthesized from DNAME */
       (type != CURL_DNS_TYPE_DNAME) &&  /* if present, accept and ignore */
       (type != dnstype))
      /* Not the same type as was asked for, nor CNAME nor DNAME */
      తిరిగి DOH_DNS_UNEXPECTED_TYPE;
    సూచీ += 2;

    ఒకవేళ(dohlen < (సూచీ + 2))
      తిరిగి DOH_DNS_బహిర్గతం_యొక్క_RANGE;
    dnsclass = doh_get16bit(doh, సూచీ);
    ఒకవేళ(DNS_CLASS_లోపల != dnsclass)
      తిరిగి DOH_DNS_UNEXPECTED_CLASS; /* unsupported */
    సూచీ += 2;

    ఒకవేళ(dohlen < (సూచీ + 4))
      తిరిగి DOH_DNS_బహిర్గతం_యొక్క_RANGE;

    ttl = doh_get32bit(doh, సూచీ);
    ఒకవేళ(ttl > MAX_DNS_TTL)
      ttl = MAX_DNS_TTL;
    ఒకవేళ(ttl < d->ttl)
      d->ttl = ttl;
    సూచీ += 4;

    ఒకవేళ(dohlen < (సూచీ + 2))
      తిరిగి DOH_DNS_బహిర్గతం_యొక్క_RANGE;

    rdlength = doh_get16bit(doh, సూచీ);
    సూచీ += 2;
    ఒకవేళ(dohlen < (సూచీ + rdlength))
      తిరిగి DOH_DNS_బహిర్గతం_యొక్క_RANGE;

    rc = doh_rdata(doh, rdlength, type, (పూర్ణాంకం)సూచీ, d);
    ఒకవేళ(rc)
      తిరిగి rc;
    సూచీ += rdlength;
    ancount--;
  }

  nscount = doh_get16bit(doh, 8);
  అయితే(nscount) {
    rc = doh_skipqname(doh, dohlen, &సూచీ);
    ఒకవేళ(rc)
      తిరిగి rc; /* bad qname */

    ఒకవేళ(dohlen < (సూచీ + 8))
      తిరిగి DOH_DNS_బహిర్గతం_యొక్క_RANGE;

    సూచీ += 2 + 2 + 4; /* type, dnsclass and ttl */

    ఒకవేళ(dohlen < (సూచీ + 2))
      తిరిగి DOH_DNS_బహిర్గతం_యొక్క_RANGE;

    rdlength = doh_get16bit(doh, సూచీ);
    సూచీ += 2;
    ఒకవేళ(dohlen < (సూచీ + rdlength))
      తిరిగి DOH_DNS_బహిర్గతం_యొక్క_RANGE;
    సూచీ += rdlength;
    nscount--;
  }

  arcount = doh_get16bit(doh, 10);
  అయితే(arcount) {
    rc = doh_skipqname(doh, dohlen, &సూచీ);
    ఒకవేళ(rc)
      తిరిగి rc; /* bad qname */

    ఒకవేళ(dohlen < (సూచీ + 8))
      తిరిగి DOH_DNS_బహిర్గతం_యొక్క_RANGE;

    సూచీ += 2 + 2 + 4; /* type, dnsclass and ttl */

    ఒకవేళ(dohlen < (సూచీ + 2))
      తిరిగి DOH_DNS_బహిర్గతం_యొక్క_RANGE;

    rdlength = doh_get16bit(doh, సూచీ);
    సూచీ += 2;
    ఒకవేళ(dohlen < (సూచీ + rdlength))
      తిరిగి DOH_DNS_బహిర్గతం_యొక్క_RANGE;
    సూచీ += rdlength;
    arcount--;
  }

  ఒకవేళ(సూచీ != dohlen)
    తిరిగి DOH_DNS_MALFORMAT; /* something is wrong */

  తిరిగి DOH_OK; /* ok */
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
స్థిర CURLcode doh2ai(స్థిరరాశి నిర్మాణం dohentry *de, స్థిరరాశి అక్షరం *hostname,
                       పూర్ణాంకం పోర్టు, నిర్మాణం Curl_addrinfo **aip)
{
  నిర్మాణం Curl_addrinfo *ai;
  నిర్మాణం Curl_addrinfo *prevai = ఖాళీ;
  నిర్మాణం Curl_addrinfo *firstai = ఖాళీ;
  నిర్మాణం sockaddr_in *addr;
#నిర్వచితమైతే USE_IPV6
  నిర్మాణం sockaddr_in6 *addr6;
#అయితే_ముగింపు
  size_t hostlen = అక్షరశ్రేణి_పొడవు(hostname) + 1; /* include null-terminator */
  CURLcode ఫలితం = CURLE_OK;
  పూర్ణాంకం i;

  కొరకు(i = 0; i < de->numaddr; i++) {
    size_t ss_కొలత;
    CURL_SA_FAMILY_T addrtype;
    ఒకవేళ(de->addr[i].type == CURL_DNS_TYPE_AAAA) {
#నిర్వచితం_కాకపోతే USE_IPV6
      /* we cannot handle IPv6 addresses */
      కొనసాగు;
#లేకపోతే
      ss_కొలత = పరిమాణం(నిర్మాణం sockaddr_in6);
      addrtype = AF_INET6;
#అయితే_ముగింపు
    }
    లేకపోతే {
      ss_కొలత = పరిమాణం(నిర్మాణం sockaddr_in);
      addrtype = AF_INET;
    }

    ai = curlx_calloc(1, పరిమాణం(నిర్మాణం Curl_addrinfo) + ss_కొలత + hostlen);
    ఒకవేళ(!ai) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      ఆపు;
    }
    ai->ai_addr = (శూన్య *)((అక్షరం *)ai + పరిమాణం(నిర్మాణం Curl_addrinfo));
    ai->ai_canonname = (శూన్య *)((అక్షరం *)ai->ai_addr + ss_కొలత);
    జ్ఞాపకం_నకలు(ai->ai_canonname, hostname, hostlen);

    ఒకవేళ(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    ఒకవేళ(prevai)
      /* make the previous entry point to this */
      prevai->ai_తదుపరి = ai;

    ai->ai_family = addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_కొలత;

    /* leave the rest of the struct filled with zero */

    ఎంపిక(ai->ai_family) {
    స్థితి AF_INET:
      addr = (శూన్య *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(పరిమాణం(నిర్మాణం in_addr) == పరిమాణం(de->addr[i].ip.v4));
      జ్ఞాపకం_నకలు(&addr->sin_addr, &de->addr[i].ip.v4, పరిమాణం(నిర్మాణం in_addr));
      addr->sin_family = addrtype;
      addr->sin_port = htons((గుర్తులేని పొట్టి)పోర్టు);
      ఆపు;

#నిర్వచితమైతే USE_IPV6
    స్థితి AF_INET6:
      addr6 = (శూన్య *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(పరిమాణం(నిర్మాణం in6_addr) == పరిమాణం(de->addr[i].ip.v6));
      జ్ఞాపకం_నకలు(&addr6->sin6_addr, &de->addr[i].ip.v6, పరిమాణం(నిర్మాణం in6_addr));
      addr6->sin6_family = addrtype;
      addr6->sin6_port = htons((గుర్తులేని పొట్టి)పోర్టు);
      ఆపు;
#అయితే_ముగింపు
    }

    prevai = ai;
  }

  ఒకవేళ(ఫలితం) {
    Curl_freeaddrinfo(firstai);
    firstai = ఖాళీ;
  }
  *aip = firstai;

  తిరిగి ఫలితం;
}

/* @unittest 1655 */
UNITTEST శూన్య de_cleanup(నిర్మాణం dohentry *d);
UNITTEST శూన్య de_cleanup(నిర్మాణం dohentry *d)
{
#నిర్వచితమైతే USE_HTTPSRR
  పూర్ణాంకం i = 0;
  కొరకు(i = 0; i < d->numhttps_rrs; i++)
    curlx_safefree(d->https_rrs[i].val);
#లేకపోతే
  (శూన్య)d;
#అయితే_ముగింపు
}

#నిర్వచితమైతే USE_HTTPSRR

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
స్థిర CURLcode doh_decode_rdata_name(స్థిరరాశి గుర్తులేని అక్షరం **buf,
                                      size_t *మిగిలిన, అక్షరం **dnsname)
{
  స్థిరరాశి గుర్తులేని అక్షరం *cp = ఖాళీ;
  size_t rem = 0;
  గుర్తులేని అక్షరం clen = 0; /* chunk len */
  నిర్మాణం dynbuf thename;

  DEBUGASSERT(buf && మిగిలిన && dnsname);
  ఒకవేళ(!buf || !మిగిలిన || !dnsname || !*మిగిలిన)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  curlx_dyn_init(&thename, CURL_MAXLEN_HOST_NAME);
  rem = *మిగిలిన;
  cp = *buf;
  clen = *cp++;
  /* RFC 9460 says it must be uncompressed */
  ఒకవేళ(clen > 63)
    తిరిగి CURLE_WEIRD_SERVER_జవాబు;

  ఒకవేళ(clen == 0) {
    /* special case - return "." as name */
    ఒకవేళ(curlx_dyn_addn(&thename, ".", 1))
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
  అయితే(clen) {
    ఒకవేళ(clen >= rem) {
      curlx_dyn_free(&thename);
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
    ఒకవేళ(curlx_dyn_addn(&thename, cp, clen) ||
       curlx_dyn_addn(&thename, ".", 1))
      తిరిగి CURLE_TOO_LARGE;

    cp += clen;
    rem -= (clen + 1);
    ఒకవేళ(rem <= 0) {
      curlx_dyn_free(&thename);
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
    clen = *cp++;
    ఒకవేళ(clen > 63) {
      /* invalid format */
      curlx_dyn_free(&thename);
      తిరిగి CURLE_WEIRD_SERVER_జవాబు;
    }
  }
  *buf = cp;
  *మిగిలిన = rem - 1;
  *dnsname = curlx_dyn_ptr(&thename);
  తిరిగి CURLE_OK;
}

/* scan for byte values <= 32 or 127 */
స్థిర CURLcode junkscan(స్థిరరాశి అక్షరం *url)
{
  స్థిరరాశి గుర్తులేని అక్షరం *p = (స్థిరరాశి గుర్తులేని అక్షరం *)url;
  అయితే(*p) {
    ఒకవేళ(*p <= 0x20 || *p == 127)
      తిరిగి CURLE_WEIRD_SERVER_జవాబు;
    p++;
  }
  తిరిగి CURLE_OK;
}

/* @unittest 1658 */
UNITTEST CURLcode doh_resp_decode_httpsrr(నిర్మాణం Curl_easy *దత్తాంశం,
                                          స్థిరరాశి గుర్తులేని అక్షరం *cp, size_t len,
                                          నిర్మాణం Curl_https_rrinfo **hrr);
UNITTEST CURLcode doh_resp_decode_httpsrr(నిర్మాణం Curl_easy *దత్తాంశం,
                                          స్థిరరాశి గుర్తులేని అక్షరం *cp, size_t len,
                                          నిర్మాణం Curl_https_rrinfo **hrr)
{
  uint16_t pcode = 0, plen = 0;
  uint32_t expected_min_pcode = 0;
  నిర్మాణం Curl_https_rrinfo *lhrr = ఖాళీ;
  అక్షరం *dnsname = ఖాళీ;
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  (శూన్య)దత్తాంశం;
  *hrr = ఖాళీ;
  ఒకవేళ(len <= 2)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  lhrr = curlx_calloc(1, పరిమాణం(నిర్మాణం Curl_https_rrinfo));
  ఒకవేళ(!lhrr)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  lhrr->ప్రాధాన్యత = doh_get16bit(cp, 0);
  cp += 2;
  len -= 2;
  ఒకవేళ(doh_decode_rdata_name(&cp, &len, &dnsname) != CURLE_OK)
    వెళ్ళు err;
  lhrr->లక్ష్యం = dnsname;
  ఫలితం = junkscan(dnsname);
  ఒకవేళ(ఫలితం) {
    /* unacceptable hostname content */
    వెళ్ళు err;
  }
  అయితే(len >= 4) {
    pcode = doh_get16bit(cp, 0);
    plen = doh_get16bit(cp, 2);
    cp += 4;
    len -= 4;
    ఒకవేళ(pcode < expected_min_pcode || plen > len) {
      ఫలితం = CURLE_WEIRD_SERVER_జవాబు;
      వెళ్ళు err;
    }
    ఫలితం = Curl_httpsrr_సమితి(lhrr, pcode, cp, plen);
    ఒకవేళ(ఫలితం)
      వెళ్ళు err;
    Curl_httpsrr_trace(దత్తాంశం, lhrr);
    cp += plen;
    len -= plen;
    expected_min_pcode = pcode + 1;
  }
  *hrr = lhrr;
  తిరిగి CURLE_OK;
err:
  Curl_httpsrr_destroy(lhrr);
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* USE_HTTPSRR */

/* called from multi when a sub transfer, e.g. doh probe, is done.
 * Parse the response and set the results in the `async` context
 * of master, using the id from the probe's CURL_EZM_DOH_PROBE
 * meta data. */
స్థిర శూన్య doh_probe_పూర్తి(నిర్మాణం Curl_easy *doh,
                           నిర్మాణం Curl_easy *master, CURLcode ఫలితం)
{
  నిర్మాణం Curl_resolv_async *async = ఖాళీ;
  నిర్మాణం doh_probes *dohp = ఖాళీ;
  నిర్మాణం doh_request *doh_req = ఖాళీ;
  నిర్మాణం Curl_addrinfo **pdest_ai;
  నిర్మాణం dohentry de;
  పూర్ణాంకం స్థానం, httpcode;

  de_init(&de);
  doh_req = Curl_meta_get(doh, CURL_EZM_DOH_PROBE);
  ఒకవేళ(!doh_req) {
    /* transfer `doh` is not a DoH probe. */
    DEBUGASSERT(0);
    వెళ్ళు బహిర్గతం;
  }

  async = Curl_async_get(master, doh_req->resolv_id);
  ఒకవేళ(!async) {
    CURL_TRC_DNS(master, "[%u] ignoring outdated DoH response",
                 doh_req->resolv_id);
    వెళ్ళు బహిర్గతం;
  }
  dohp = async->doh;

  కొరకు(స్థానం = 0; స్థానం < DOH_స్థానం_లెక్క; ++స్థానం) {
    ఒకవేళ(dohp->probe_mid[స్థానం] == doh->mid)
      ఆపు;
  }
  /* We really should have found the slot where to store the response */
  ఒకవేళ(స్థానం >= DOH_స్థానం_లెక్క) {
    failf(master, "DoH: unknown sub request done");
    DEBUGASSERT(0);
    వెళ్ళు బహిర్గతం;
  }

  async->queries_ongoing--;
  dohp = async->doh;
  httpcode = doh->info.httpcode;
  ఎంపిక(స్థానం) {
  స్థితి DOH_స్థానం_IPV4:
    async->dns_responses |= CURL_DNSQ_A;
    ఆపు;
#నిర్వచితమైతే USE_IPV6
  స్థితి DOH_స్థానం_IPV6:
    async->dns_responses |= CURL_DNSQ_AAAA;
    ఆపు;
#అయితే_ముగింపు
#నిర్వచితమైతే USE_HTTPSRR
  స్థితి DOH_స్థానం_HTTPS_RR:
    async->dns_responses |= CURL_DNSQ_HTTPS;
    ఆపు;
#అయితే_ముగింపు
  అప్రమేయ:
    DEBUGASSERT(0);
    ఆపు;
  }

  ఒకవేళ(ఫలితం) {
    dohp->probe_rc[స్థానం] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: %s",
          doh_type2name(doh_req->dnstype), curl_easy_strerror(ఫలితం));
    వెళ్ళు బహిర్గతం;
  }
  లేకపోతే ఒకవేళ((httpcode < 200) || (httpcode >= 300)) {
    dohp->probe_rc[స్థానం] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: HTTP status %d",
          doh_type2name(doh_req->dnstype), httpcode);
    వెళ్ళు బహిర్గతం;
  }

  dohp->probe_rc[స్థానం] = doh_resp_decode(curlx_dyn_uptr(&doh_req->resp_body),
                                         curlx_dyn_len(&doh_req->resp_body),
                                         doh_req->dnstype, &de);
  ఒకవేళ(dohp->probe_rc[స్థానం]) {
#నిర్వచితమైతే USE_HTTPSRR
    ఒకవేళ((dohp->probe_rc[స్థానం] == DOH_NO_CONTENT) &&
       (doh_req->dnstype == CURL_DNS_TYPE_HTTPS)) {
      dohp->probe_rc[స్థానం] = DOH_DNS_NXDOMAIN;
    }
#అయితే_ముగింపు
    infof(doh, "[DoH] [%s] error decoding response: %s",
          doh_type2name(doh_req->dnstype),
          doh_strerror(dohp->probe_rc[స్థానం]));
    వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(doh_req->dnstype == CURL_DNS_TYPE_A)
    pdest_ai = &async->ai_A;
  లేకపోతే ఒకవేళ(doh_req->dnstype == CURL_DNS_TYPE_AAAA)
    pdest_ai = &async->ai_AAAA;
  లేకపోతే
    pdest_ai = ఖాళీ;

  ఒకవేళ(pdest_ai && de.numaddr) {
    ఒకవేళ(*pdest_ai) {
      Curl_freeaddrinfo(*pdest_ai);
      *pdest_ai = ఖాళీ;
    }
    ఫలితం = doh2ai(&de, async->peer->hostname, async->peer->పోర్టు, pdest_ai);
    ఒకవేళ(ఫలితం) { /* hard failure on our side, fail completely */
      infof(doh, "[DoH] [%s] error creating addrinfo: %s",
            doh_type2name(doh_req->dnstype), curl_easy_strerror(ఫలితం));
      dohp->probe_rc[స్థానం] = DOH_OOM;
      async->ఫలితం = ఫలితం;
    }
  }
#నిర్వచితమైతే USE_HTTPSRR
  లేకపోతే ఒకవేళ((doh_req->dnstype == CURL_DNS_TYPE_HTTPS) && de.numhttps_rrs) {
    CURL_TRC_DNS(doh, "[HTTPS] got %d records", de.numhttps_rrs);
    ఫలితం = doh_resp_decode_httpsrr(doh, de.https_rrs->val,
                                     de.https_rrs->len, &async->httpsrr);
    ఒకవేళ(ఫలితం) {
      dohp->probe_rc[స్థానం] = DOH_HTTP_FAILED;
      infof(doh, "[DoH] error decoding HTTPS RR: %s",
            curl_easy_strerror(ఫలితం));
      వెళ్ళు బహిర్గతం;
    }
  }
#అయితే_ముగింపు /* USE_HTTPSRR */

  /* DoH request complete, run master to act on results */
  infof(doh, "DoH request complete, %u to go", async->queries_ongoing);

బహిర్గతం:
  Curl_multi_mark_dirty(master);
  de_cleanup(&de);
  Curl_meta_remove(doh, CURL_EZM_DOH_PROBE);
}

CURLcode Curl_doh_take_ఫలితం(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం Curl_resolv_async *async,
                              నిర్మాణం Curl_dns_నమోదు **pdns)
{
  నిర్మాణం doh_probes *dohp = async->doh;
  CURLcode ఫలితం = CURLE_OK;

  *pdns = ఖాళీ; /* defaults to no response */
  ఒకవేళ(!dohp)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  async->negative_answer = FALSE;
  ఒకవేళ(async->ఫలితం) {
    ఫలితం = async->ఫలితం;
    వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     dohp->probe_mid[DOH_స్థానం_IPV4] == UINT32_MAX &&
     dohp->probe_mid[DOH_స్థానం_IPV6] == UINT32_MAX) {
    failf(దత్తాంశం, "Could not DoH-resolve: %s", async->peer->hostname);
    తిరిగి async->for_proxy ?
      CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
  }
  లేకపోతే ఒకవేళ(!async->queries_ongoing) {
    నిర్మాణం Curl_dns_నమోదు *dns = ఖాళీ;
    bool negative = TRUE;
    పూర్ణాంకం స్థానం;

    /* remove DoH handles from multi handle and close them */
    doh_close(దత్తాంశం, async);
    /* parse the responses, create the struct and return it! */
    కొరకు(స్థానం = 0; స్థానం < DOH_స్థానం_లెక్క; స్థానం++) {
      /* Failing without an NXDOMAIN answer - a SERVFAIL-class rcode or
         an undecodable response - says nothing about the name. Such a
         failure must not be cached as a negative entry. */
      ఒకవేళ(dohp->probe_rc[స్థానం] && (dohp->probe_rc[స్థానం] != DOH_DNS_NXDOMAIN))
        negative = FALSE;
    } /* next slot */

    ఒకవేళ(async->ai_A || async->ai_AAAA) {
      dns = Curl_dnsc_mk_addr2(
        దత్తాంశం, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
      ఒకవేళ(!dns) {
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        వెళ్ళు బహిర్గతం;
      }
    }
#నిర్వచితమైతే USE_HTTPSRR
    లేకపోతే ఒకవేళ((async->dns_queries & CURL_DNSQ_HTTPS) &&
            !dohp->probe_rc[DOH_స్థానం_HTTPS_RR]) {
      Curl_httpsrr_trace(దత్తాంశం, async->httpsrr);
      dns = Curl_dnsc_mk_https(దత్తాంశం, &async->httpsrr, async->peer);
      ఒకవేళ(!dns) {
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        వెళ్ళు బహిర్గతం;
      }
    }
#అయితే_ముగింపు /* USE_HTTPSRR */
    లేకపోతే {
      /* every query failed. Only NXDOMAIN answers for all of them
         make this a negative answer, eligible for caching. */
      async->negative_answer = negative;
      ఫలితం = async->for_proxy ?
        CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
    }

    /* and add the entry to the cache */
    ఒకవేళ(dns)
      ఫలితం = Curl_dnscache_కూడు(దత్తాంశం, dns);
    *pdns = dns;
  } /* !async->queries_ongoing */
  లేకపోతే
    /* wait for pending DoH transactions to complete */
    తిరిగి CURLE_AGAIN;

బహిర్గతం:
  Curl_doh_cleanup(దత్తాంశం, async);
  తిరిగి ఫలితం;
}

స్థిర శూన్య doh_close(నిర్మాణం Curl_easy *దత్తాంశం,
                      నిర్మాణం Curl_resolv_async *async)
{
  నిర్మాణం doh_probes *doh = async ? async->doh : ఖాళీ;
  ఒకవేళ(doh && దత్తాంశం->multi) {
    నిర్మాణం Curl_easy *probe_దత్తాంశం;
    uint32_t mid;
    size_t స్థానం;
    కొరకు(స్థానం = 0; స్థానం < DOH_స్థానం_లెక్క; స్థానం++) {
      mid = doh->probe_mid[స్థానం];
      ఒకవేళ(mid == UINT32_MAX)
        కొనసాగు;
      doh->probe_mid[స్థానం] = UINT32_MAX;
      /* should have been called before data is removed from multi handle */
      DEBUGASSERT(దత్తాంశం->multi);
      probe_దత్తాంశం = దత్తాంశం->multi ? Curl_multi_get_easy(దత్తాంశం->multi, mid) : ఖాళీ;
      ఒకవేళ(!probe_దత్తాంశం) {
        DEBUGF(infof(దత్తాంశం, "Curl_doh_close: xfer for mid=%u not found!", mid));
        కొనసాగు;
      }
      probe_దత్తాంశం->sub_xfer_పూర్తి = ఖాళీ; /* No longer interested in result */
      /* data->multi might already be reset at this time */
      Curl_multi_remove_handle(దత్తాంశం->multi, probe_దత్తాంశం);
      Curl_close(&probe_దత్తాంశం);
    }
    CURL_TRC_DNS(దత్తాంశం, "[DoH] probe done");
  }
}

శూన్య Curl_doh_cleanup(నిర్మాణం Curl_easy *దత్తాంశం,
                      నిర్మాణం Curl_resolv_async *async)
{
  నిర్మాణం doh_probes *dohp = async->doh;
  ఒకవేళ(dohp) {
    doh_close(దత్తాంశం, async);
    curlx_safefree(async->doh);
  }
}

#అయితే_ముగింపు /* CURL_DISABLE_DOH */
