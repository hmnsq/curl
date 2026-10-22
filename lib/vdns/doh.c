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
#অন্তর্ভাব "curl_setup.h"

#ifndef CURL_DISABLE_DOH

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curl_addrinfo.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "vdns/doh.h"
#অন্তর্ভাব "vdns/httpsrr.h"
#অন্তর্ভাব "curlx/strdup.h"
#অন্তর্ভাব "curlx/dynbuf.h"
#অন্তর্ভাব "escape.h"  /* for Curl_hexencode() */
#অন্তর্ভাব "urlapi-int.h"

#define DNS_CLASS_IN 0x01

স্থির শূন্য doh_close(কাঠামো Curl_easy *তথ্য,
                      কাঠামো Curl_resolv_async *async);

#ifdef CURLVERBOSE
স্থির ধ্রুবক অক্ষর * ধ্রুবক doh_code_str[] = {
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

স্থির ধ্রুবক অক্ষর *doh_strerror(DOHcode code)
{
  যদি((size_t)code < CURL_ARRAYSIZE(doh_code_str))
    ফেরত doh_code_str[code];
  ফেরত "bad error code";
}

স্থির ধ্রুবক অক্ষর *doh_type2name(DNStype dnstype)
{
  নির্বাচন(dnstype) {
  ক্ষেত্র CURL_DNS_TYPE_A:
    ফেরত "A";
  ক্ষেত্র CURL_DNS_TYPE_AAAA:
    ফেরত "AAAA";
#ifdef USE_HTTPSRR
  ক্ষেত্র CURL_DNS_TYPE_HTTPS:
    ফেরত "HTTPS";
#endif
  পূর্বনির্ধারিত:
    ফেরত "unknown";
  }
}

#endif /* CURLVERBOSE */

/* @unittest 1655
 */
UNITTEST DOHcode doh_req_encode(ধ্রুবক অক্ষর *host,
                                DNStype dnstype,
                                unsigned অক্ষর *dnsp,  /*বাফার*/
                                size_t len,  /* buffer size */
                                size_t *olen);  /* output length */
UNITTEST DOHcode doh_req_encode(ধ্রুবক অক্ষর *host,
                                DNStype dnstype,
                                unsigned অক্ষর *dnsp, /*বাফার*/
                                size_t len,   /* buffer size */
                                size_t *olen) /* output length */
{
  ধ্রুবক size_t hostlen = strlen(host);
  unsigned অক্ষর *orig = dnsp;
  ধ্রুবক অক্ষর *hostp = host;

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
  যদি(host[hostlen - 1] != '.')
    expected_len++;

  যদি(expected_len > DOH_MAX_DNSREQ_আকার)
    ফেরত DOH_DNS_NAME_TOO_LONG;

  যদি(len < expected_len)
    ফেরত DOH_TOO_SMALL_বাফার;

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
  যখন(*hostp) {
    size_t labellen;
    ধ্রুবক অক্ষর *dot = strchr(hostp, '.');
    যদি(dot)
      labellen = dot - hostp;
    অন্যথা
      labellen = strlen(hostp);
    যদি((labellen > 63) || (!labellen)) {
      /* label is too long or too short, error out */
      *olen = 0;
      ফেরত DOH_DNS_BAD_LABEL;
    }
    /* label is non-empty, process it */
    *dnsp++ = (unsigned অক্ষর)labellen;
    memcpy(dnsp, hostp, labellen);
    dnsp += labellen;
    hostp += labellen;
    /* advance past dot, but only if there is one */
    যদি(dot)
      hostp++;
  } /* next label */

  *dnsp++ = 0; /* append zero-length label for root */

  /* There are assigned TYPE codes beyond 255: use range [1..65535] */
  *dnsp++ = (unsigned অক্ষর)(255 & (dnstype >> 8)); /* upper 8-bit TYPE */
  *dnsp++ = (unsigned অক্ষর)(255 & dnstype);        /* lower 8-bit TYPE */

  *dnsp++ = '\0'; /* upper 8-bit CLASS */
  *dnsp++ = DNS_CLASS_IN; /* IN - "the Internet" */

  *olen = dnsp - orig;

  /* verify that our estimation of length is valid, since
   * this has led to buffer overflows in this function */
  DEBUGASSERT(*olen == expected_len);
  ফেরত DOH_OK;
}

স্থির size_t doh_probe_write_cb(অক্ষর *contents, size_t আকার_2, size_t nmemb,
                                 শূন্য *userp)
{
  size_t realsize = আকার_2 * nmemb;
  কাঠামো Curl_easy *তথ্য = userp;
  কাঠামো doh_request *doh_req = Curl_meta_get(তথ্য, CURL_EZM_DOH_PROBE);
  যদি(!doh_req)
    ফেরত CURL_WRITEFUNC_ERROR;

  যদি(curlx_dyn_addn(&doh_req->resp_body, contents, realsize))
    ফেরত 0;

  ফেরত realsize;
}

স্থির শূন্য doh_probe_done(কাঠামো Curl_easy *doh,
                           কাঠামো Curl_easy *master, CURLcode ফলাফল);
স্থির শূন্য doh_probe_dtor(ধ্রুবক শূন্য *চাবি, size_t klen, শূন্য *e)
{
  (শূন্য)চাবি;
  (শূন্য)klen;
  যদি(e) {
    কাঠামো doh_request *doh_req = e;
    curl_slist_free_all(doh_req->req_hds);
    curlx_dyn_free(&doh_req->resp_body);
    curlx_free(e);
  }
}

#define ERROR_CHECK_SETOPT(x, y)                        \
  করো {                                                  \
    ফলাফল = curl_easy_setopt((CURL *)doh, x, y);       \
    যদি(ফলাফল &&                                        \
       ফলাফল != CURLE_NOT_BUILT_IN &&                  \
       ফলাফল != CURLE_UNKNOWN_OPTION)                  \
      যাও error;                                       \
  } যখন(0)

স্থির CURLcode doh_probe_run(কাঠামো Curl_easy *তথ্য,
                              DNStype dnstype,
                              ধ্রুবক অক্ষর *host,
                              ধ্রুবক অক্ষর *url, CURLM *multi,
                              uint32_t resolv_id,
                              uint32_t *pmid)
{
  কাঠামো Curl_easy *doh = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;
  timediff_t timeout_ms;
  কাঠামো doh_request *doh_req;
  DOHcode d;
  bool maybe_https = !curl_strnequal(url, STRCONST("http:"));

  *pmid = UINT32_MAX;

  doh_req = curlx_calloc(1, আকার(*doh_req));
  যদি(!doh_req)
    ফেরত CURLE_OUT_OF_MEMORY;
  doh_req->resolv_id = resolv_id;
  doh_req->dnstype = dnstype;
  curlx_dyn_init(&doh_req->resp_body, DYN_DOH_RESPONSE);

  d = doh_req_encode(host, dnstype, doh_req->req_body,
                     আকার(doh_req->req_body),
                     &doh_req->req_body_len);
  যদি(d) {
    failf(তথ্য, "Failed to encode DoH packet [%d]", (পূর্ণসংখ্যা)d);
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও error;
  }

  timeout_ms = Curl_timeleft_ms(তথ্য);
  যদি(timeout_ms < 0) {
    ফলাফল = CURLE_OPERATION_TIMEDOUT;
    যাও error;
  }

  doh_req->req_hds =
    curl_slist_append(ফাঁকা, "Content-Type: application/dns-message");
  যদি(!doh_req->req_hds) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও error;
  }

  /* Curl_open() is the internal version of curl_easy_init() */
  ফলাফল = Curl_open(&doh);
  যদি(ফলাফল)
    যাও error;

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
  যদি(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);
    ERROR_CHECK_SETOPT(CURLOPT_PIPEWAIT, 1L);
  }
#endif
#ifndef DEBUGBUILD
  /* enforce HTTPS if not debug */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
#অন্যথা
  /* in debug mode, also allow http */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
#endif
  ERROR_CHECK_SETOPT(CURLOPT_TIMEOUT_MS, (long)timeout_ms);
  ERROR_CHECK_SETOPT(CURLOPT_SHARE, (CURLSH *)তথ্য->share);
  যদি(তথ্য->set.err && তথ্য->set.err != stderr)
    ERROR_CHECK_SETOPT(CURLOPT_STDERR, তথ্য->set.err);
  যদি(Curl_trc_ft_is_verbose(তথ্য, &Curl_trc_feat_doh))
    ERROR_CHECK_SETOPT(CURLOPT_VERBOSE, 1L);
  যদি(তথ্য->set.no_signal)
    ERROR_CHECK_SETOPT(CURLOPT_NOSIGNAL, 1L);
  যদি(তথ্য->set.fdebug)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGFUNCTION, তথ্য->set.fdebug);
  যদি(তথ্য->set.debugdata)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGDATA, তথ্য->set.debugdata);

#ifdef USE_RESOLV_ARES
  যদি(CURL_EASY_STR(তথ্য, STRING_DNS_SERVERS)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_SERVERS,
                       CURL_EASY_STR(তথ্য, STRING_DNS_SERVERS));
  }
  যদি(CURL_EASY_STR(তথ্য, STRING_DNS_INTERFACE)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_INTERFACE,
                       CURL_EASY_STR(তথ্য, STRING_DNS_INTERFACE));
  }
  যদি(CURL_EASY_STR(তথ্য, STRING_DNS_LOCAL_IP4)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP4,
                       CURL_EASY_STR(তথ্য, STRING_DNS_LOCAL_IP4));
  }
  যদি(CURL_EASY_STR(তথ্য, STRING_DNS_LOCAL_IP6)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP6,
                       CURL_EASY_STR(তথ্য, STRING_DNS_LOCAL_IP6));
  }
#endif
  যদি(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYHOST,
                       তথ্য->set.doh_verifyhost ? 2L : 0L);
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYPEER,
                       তথ্য->set.doh_verifypeer ? 1L : 0L);
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYSTATUS,
                       তথ্য->set.doh_verifystatus ? 1L : 0L);

    /* Inherit *some* SSL options from the user's transfer. This is a
       best-guess as to which options are needed for compatibility. #3661

       Note DoH does not inherit the user's proxy server so proxy SSL settings
       have no effect and are not inherited. If that changes then two new
       options should be added to check doh proxy insecure separately,
       CURLOPT_DOH_PROXY_SSL_VERIFYHOST and CURLOPT_DOH_PROXY_SSL_VERIFYPEER.
       */
    doh->set.ssl.custom_cafile = তথ্য->set.ssl.custom_cafile;
    doh->set.ssl.custom_capath = তথ্য->set.ssl.custom_capath;
    doh->set.ssl.custom_cablob = তথ্য->set.ssl.custom_cablob;
    যদি(CURL_EASY_STR(তথ্য, STRING_SSL_CAFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO,
                         CURL_EASY_STR(তথ্য, STRING_SSL_CAFILE));
    }
    যদি(তথ্য->set.blobs[BLOB_CAINFO]) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO_BLOB, তথ্য->set.blobs[BLOB_CAINFO]);
    }
    যদি(CURL_EASY_STR(তথ্য, STRING_SSL_CAPATH)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAPATH,
                         CURL_EASY_STR(তথ্য, STRING_SSL_CAPATH));
    }
    যদি(CURL_EASY_STR(তথ্য, STRING_SSL_CRLFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CRLFILE,
                         CURL_EASY_STR(তথ্য, STRING_SSL_CRLFILE));
    }
    যদি(তথ্য->set.ssl.certinfo)
      ERROR_CHECK_SETOPT(CURLOPT_CERTINFO, 1L);
    যদি(তথ্য->set.ssl_fsslctx)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_FUNCTION, তথ্য->set.ssl_fsslctx);
    যদি(তথ্য->set.ssl_fsslctxp)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_তথ্য, তথ্য->set.ssl_fsslctxp);
    যদি(CURL_EASY_STR(তথ্য, STRING_SSL_EC_CURVES)) {
      ERROR_CHECK_SETOPT(CURLOPT_SSL_EC_CURVES,
                         CURL_EASY_STR(তথ্য, STRING_SSL_EC_CURVES));
    }
    (শূন্য)curl_easy_setopt(doh, CURLOPT_SSL_OPTIONS,
                           ((long)তথ্য->set.ssl.ssl_options &
                            ~CURLSSLOPT_AUTO_CLIENT_CERT));
  }

  doh->state.internal = TRUE;
  doh->master_mid = তথ্য->mid; /* master transfer of this one */
  doh->sub_xfer_done = doh_probe_done;

  ফলাফল = Curl_meta_set(doh, CURL_EZM_DOH_PROBE, doh_req, doh_probe_dtor);
  doh_req = ফাঁকা; /* call took ownership */
  যদি(ফলাফল)
    যাও error;

  /* DoH handles must not inherit private_data. The handles may be passed to
     the user via callbacks and the user will be able to identify them as
     internal handles because private data is not set. The user can then set
     private_data via CURLOPT_PRIVATE if they so choose. */
  DEBUGASSERT(!doh->set.private_তথ্য);

  যদি(Curl_multi_add_handle(multi, doh))
    যাও error;

  *pmid = doh->mid;
  ফেরত CURLE_OK;

error:
  Curl_close(&doh);
  যদি(doh_req)
    doh_probe_dtor(ফাঁকা, 0, doh_req);
  ফেরত ফলাফল;
}

/*
 * Curl_doh() starts a name resolve using DoH. It resolves a name and returns
 * a 'Curl_addrinfo *' with the address information.
 */

CURLcode Curl_doh(কাঠামো Curl_easy *তথ্য,
                  কাঠামো Curl_resolv_async *async)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো doh_probes *dohp = ফাঁকা;
  size_t i;

  DEBUGASSERT(!async->doh);
  DEBUGASSERT(async->peer->hostname[0]);
  যদি(async->doh) {
    DEBUGASSERT(0); /* should not happen */
    Curl_doh_cleanup(তথ্য, async);
  }

  যদি(!async->dns_queries)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef USE_HTTPSRR
  যদি(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     (async->dns_queries & CURL_DNSQ_HTTPS)) {
    /* Can't mix those in the same async resolve */
    DEBUGASSERT(0);
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  }
#অন্যথা
  যদি(async->dns_queries & CURL_DNSQ_HTTPS) {
    DEBUGASSERT(0);
    ফেরত CURLE_NOT_BUILT_IN;
  }
#endif

  /* start clean, consider allocating this struct on demand */
  async->doh = dohp = curlx_calloc(1, আকার(কাঠামো doh_probes));
  যদি(!dohp)
    ফেরত CURLE_OUT_OF_MEMORY;

  জন্য(i = 0; i < DOH_SLOT_গুনতি; ++i) {
    dohp->probe_rc[i] = DOH_OK;
    dohp->probe_mid[i] = UINT32_MAX;
  }

#ifdef USE_IPV6
  /* AAAA results have preference in happy eyeballing, trigger first */
  যদি(async->dns_queries & CURL_DNSQ_AAAA) {
    /* create IPv6 DoH request */
    ফলাফল = doh_probe_run(তথ্য, CURL_DNS_TYPE_AAAA,
                           async->peer->hostname,
                           CURL_EASY_STR(তথ্য, STRING_DOH),
                           তথ্য->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV6]);
    যদি(ফলাফল)
      যাও error;
    async->queries_ongoing++;
  }
#endif

  /* create IPv4 DoH request */
  যদি(async->dns_queries & CURL_DNSQ_A) {
    ফলাফল = doh_probe_run(তথ্য, CURL_DNS_TYPE_A,
                           async->peer->hostname,
                           CURL_EASY_STR(তথ্য, STRING_DOH),
                           তথ্য->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV4]);
    যদি(ফলাফল)
      যাও error;
    async->queries_ongoing++;
  }

#ifdef USE_HTTPSRR
  যদি(async->dns_queries & CURL_DNSQ_HTTPS) {
    অক্ষর *qname = ফাঁকা;
    যদি(async->peer->port != PORT_HTTPS) {
      qname = curl_maprintf("_%u._https.%s",
                            async->peer->port, async->peer->hostname);
      যদি(!qname)
        যাও error;
    }
    ফলাফল = doh_probe_run(তথ্য, CURL_DNS_TYPE_HTTPS,
                           qname ? qname : async->peer->hostname,
                           CURL_EASY_STR(তথ্য, STRING_DOH), তথ্য->multi,
                           async->id,
                           &dohp->probe_mid[DOH_SLOT_HTTPS_RR]);
    curlx_free(qname);
    যদি(ফলাফল)
      যাও error;
    async->queries_ongoing++;
  }
#endif
  ফেরত CURLE_OK;

error:
  Curl_doh_cleanup(তথ্য, async);
  ফেরত ফলাফল;
}

স্থির DOHcode doh_skipqname(ধ্রুবক unsigned অক্ষর *doh, size_t dohlen,
                             unsigned পূর্ণসংখ্যা *indexp)
{
  unsigned অক্ষর দৈর্ঘ্য;
  করো {
    যদি(dohlen < (*indexp + 1))
      ফেরত DOH_DNS_OUT_OF_RANGE;
    দৈর্ঘ্য = doh[*indexp];
    যদি((দৈর্ঘ্য & 0xc0) == 0xc0) {
      /* name pointer, advance over it and be done */
      যদি(dohlen < (*indexp + 2))
        ফেরত DOH_DNS_OUT_OF_RANGE;
      *indexp += 2;
      বিরতি;
    }
    যদি(দৈর্ঘ্য & 0xc0)
      ফেরত DOH_DNS_BAD_LABEL;
    যদি(dohlen < (*indexp + 1 + দৈর্ঘ্য))
      ফেরত DOH_DNS_OUT_OF_RANGE;
    *indexp += (unsigned পূর্ণসংখ্যা)(1 + দৈর্ঘ্য);
  } যখন(দৈর্ঘ্য);
  ফেরত DOH_OK;
}

স্থির unsigned short doh_get16bit(ধ্রুবক unsigned অক্ষর *doh,
                                   unsigned পূর্ণসংখ্যা সূচক)
{
  ফেরত (unsigned short)((doh[সূচক] << 8) | doh[সূচক + 1]);
}

স্থির unsigned পূর্ণসংখ্যা doh_get32bit(ধ্রুবক unsigned অক্ষর *doh, unsigned পূর্ণসংখ্যা সূচক)
{
  /* make clang and gcc optimize this to bswap by incrementing
     the pointer first. */
  doh += সূচক;

  /* avoid undefined behavior by casting to unsigned before shifting
     24 bits, possibly into the sign bit. codegen is same, but
     ub sanitizer will not be upset */
  ফেরত ((unsigned)doh[0] << 24) | ((unsigned)doh[1] << 16) |
         ((unsigned)doh[2] << 8) | doh[3];
}

স্থির শূন্য doh_store_a(ধ্রুবক unsigned অক্ষর *doh, পূর্ণসংখ্যা সূচক,
                        কাঠামো dohentry *d)
{
  /* silently ignore addresses over the limit */
  যদি(d->numaddr < DOH_MAX_ADDR) {
    কাঠামো dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_A;
    memcpy(&a->ip.v4, &doh[সূচক], 4);
    d->numaddr++;
  }
}

স্থির শূন্য doh_store_aaaa(ধ্রুবক unsigned অক্ষর *doh, পূর্ণসংখ্যা সূচক,
                           কাঠামো dohentry *d)
{
  /* silently ignore addresses over the limit */
  যদি(d->numaddr < DOH_MAX_ADDR) {
    কাঠামো dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_AAAA;
    memcpy(&a->ip.v6, &doh[সূচক], 16);
    d->numaddr++;
  }
}

#ifdef USE_HTTPSRR
স্থির DOHcode doh_store_https(ধ্রুবক unsigned অক্ষর *doh, পূর্ণসংখ্যা সূচক,
                               কাঠামো dohentry *d, uint16_t len)
{
  /* silently ignore RRs over the limit */
  যদি(d->numhttps_rrs < DOH_MAX_HTTPS) {
    কাঠামো dohhttps_rr *h = &d->https_rrs[d->numhttps_rrs];
    h->val = curlx_memdup(&doh[সূচক], len);
    যদি(!h->val)
      ফেরত DOH_OUT_OF_MEM;
    h->len = len;
    d->numhttps_rrs++;
  }
  ফেরত DOH_OK;
}
#endif

স্থির DOHcode doh_rdata(ধ্রুবক unsigned অক্ষর *doh,
                         unsigned short rdlength,
                         unsigned short type,
                         পূর্ণসংখ্যা সূচক,
                         কাঠামো dohentry *d)
{
  /* RDATA
     - A (TYPE 1): 4 bytes
     - AAAA (TYPE 28): 16 bytes
     - HTTPS (TYPE 65): N bytes */
  নির্বাচন(type) {
  ক্ষেত্র CURL_DNS_TYPE_A:
    যদি(rdlength != 4)
      ফেরত DOH_DNS_RDATA_LEN;
    doh_store_a(doh, সূচক, d);
    বিরতি;
  ক্ষেত্র CURL_DNS_TYPE_AAAA:
    যদি(rdlength != 16)
      ফেরত DOH_DNS_RDATA_LEN;
    doh_store_aaaa(doh, সূচক, d);
    বিরতি;
#ifdef USE_HTTPSRR
  ক্ষেত্র CURL_DNS_TYPE_HTTPS:
    যদি(rdlength < 3)
      ফেরত DOH_DNS_RDATA_LEN;
    ফেরত doh_store_https(doh, সূচক, d, rdlength);
#endif
  পূর্বনির্ধারিত:
    /* unsupported type, or type we do not store, skip it */
    বিরতি;
  }
  ফেরত DOH_OK;
}

/* @unittest 1655 */
UNITTEST শূন্য de_init(কাঠামো dohentry *de);
UNITTEST শূন্য de_init(কাঠামো dohentry *de)
{
  memset(de, 0, আকার(*de));
  de->ttl = INT_MAX;
}

/* TTL value cap */
#define MAX_DNS_TTL 86400U /* 24 hours */
/* @unittest 1650 */
UNITTEST DOHcode doh_resp_decode(ধ্রুবক unsigned অক্ষর *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 কাঠামো dohentry *d);
UNITTEST DOHcode doh_resp_decode(ধ্রুবক unsigned অক্ষর *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 কাঠামো dohentry *d)
{
  unsigned অক্ষর rcode;
  unsigned short qdcount;
  unsigned short ancount;
  unsigned short type = 0;
  unsigned short rdlength;
  unsigned short nscount;
  unsigned short arcount;
  unsigned পূর্ণসংখ্যা সূচক = 12;
  DOHcode rc;

  যদি(dohlen < 12)
    ফেরত DOH_TOO_SMALL_বাফার; /* too small */
  যদি(!doh || doh[0] || doh[1])
    ফেরত DOH_DNS_BAD_ID; /* bad ID */
  rcode = doh[3] & 0x0f;
  যদি(rcode == 3)
    ফেরত DOH_DNS_NXDOMAIN; /* name does not exist */
  যদি(rcode)
    ফেরত DOH_DNS_BAD_RCODE; /* bad rcode */

  qdcount = doh_get16bit(doh, 4);
  যখন(qdcount) {
    rc = doh_skipqname(doh, dohlen, &সূচক);
    যদি(rc)
      ফেরত rc; /* bad qname */
    যদি(dohlen < (সূচক + 4))
      ফেরত DOH_DNS_OUT_OF_RANGE;
    সূচক += 4; /* skip question's type and class */
    qdcount--;
  }

  ancount = doh_get16bit(doh, 6);
  যখন(ancount) {
    unsigned short dnsclass;
    unsigned পূর্ণসংখ্যা ttl;

    rc = doh_skipqname(doh, dohlen, &সূচক);
    যদি(rc)
      ফেরত rc; /* bad qname */

    যদি(dohlen < (সূচক + 2))
      ফেরত DOH_DNS_OUT_OF_RANGE;

    type = doh_get16bit(doh, সূচক);
    যদি((type != CURL_DNS_TYPE_CNAME) &&  /* may be synthesized from DNAME */
       (type != CURL_DNS_TYPE_DNAME) &&  /* if present, accept and ignore */
       (type != dnstype))
      /* Not the same type as was asked for, nor CNAME nor DNAME */
      ফেরত DOH_DNS_UNEXPECTED_TYPE;
    সূচক += 2;

    যদি(dohlen < (সূচক + 2))
      ফেরত DOH_DNS_OUT_OF_RANGE;
    dnsclass = doh_get16bit(doh, সূচক);
    যদি(DNS_CLASS_IN != dnsclass)
      ফেরত DOH_DNS_UNEXPECTED_CLASS; /* unsupported */
    সূচক += 2;

    যদি(dohlen < (সূচক + 4))
      ফেরত DOH_DNS_OUT_OF_RANGE;

    ttl = doh_get32bit(doh, সূচক);
    যদি(ttl > MAX_DNS_TTL)
      ttl = MAX_DNS_TTL;
    যদি(ttl < d->ttl)
      d->ttl = ttl;
    সূচক += 4;

    যদি(dohlen < (সূচক + 2))
      ফেরত DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, সূচক);
    সূচক += 2;
    যদি(dohlen < (সূচক + rdlength))
      ফেরত DOH_DNS_OUT_OF_RANGE;

    rc = doh_rdata(doh, rdlength, type, (পূর্ণসংখ্যা)সূচক, d);
    যদি(rc)
      ফেরত rc;
    সূচক += rdlength;
    ancount--;
  }

  nscount = doh_get16bit(doh, 8);
  যখন(nscount) {
    rc = doh_skipqname(doh, dohlen, &সূচক);
    যদি(rc)
      ফেরত rc; /* bad qname */

    যদি(dohlen < (সূচক + 8))
      ফেরত DOH_DNS_OUT_OF_RANGE;

    সূচক += 2 + 2 + 4; /* type, dnsclass and ttl */

    যদি(dohlen < (সূচক + 2))
      ফেরত DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, সূচক);
    সূচক += 2;
    যদি(dohlen < (সূচক + rdlength))
      ফেরত DOH_DNS_OUT_OF_RANGE;
    সূচক += rdlength;
    nscount--;
  }

  arcount = doh_get16bit(doh, 10);
  যখন(arcount) {
    rc = doh_skipqname(doh, dohlen, &সূচক);
    যদি(rc)
      ফেরত rc; /* bad qname */

    যদি(dohlen < (সূচক + 8))
      ফেরত DOH_DNS_OUT_OF_RANGE;

    সূচক += 2 + 2 + 4; /* type, dnsclass and ttl */

    যদি(dohlen < (সূচক + 2))
      ফেরত DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, সূচক);
    সূচক += 2;
    যদি(dohlen < (সূচক + rdlength))
      ফেরত DOH_DNS_OUT_OF_RANGE;
    সূচক += rdlength;
    arcount--;
  }

  যদি(সূচক != dohlen)
    ফেরত DOH_DNS_MALFORMAT; /* something is wrong */

  ফেরত DOH_OK; /* ok */
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
স্থির CURLcode doh2ai(ধ্রুবক কাঠামো dohentry *de, ধ্রুবক অক্ষর *hostname,
                       পূর্ণসংখ্যা port, কাঠামো Curl_addrinfo **aip)
{
  কাঠামো Curl_addrinfo *ai;
  কাঠামো Curl_addrinfo *prevai = ফাঁকা;
  কাঠামো Curl_addrinfo *firstai = ফাঁকা;
  কাঠামো sockaddr_in *addr;
#ifdef USE_IPV6
  কাঠামো sockaddr_in6 *addr6;
#endif
  size_t hostlen = strlen(hostname) + 1; /* include null-terminator */
  CURLcode ফলাফল = CURLE_OK;
  পূর্ণসংখ্যা i;

  জন্য(i = 0; i < de->numaddr; i++) {
    size_t ss_আকার;
    CURL_SA_FAMILY_T addrtype;
    যদি(de->addr[i].type == CURL_DNS_TYPE_AAAA) {
#ifndef USE_IPV6
      /* we cannot handle IPv6 addresses */
      চালু;
#অন্যথা
      ss_আকার = আকার(কাঠামো sockaddr_in6);
      addrtype = AF_INET6;
#endif
    }
    অন্যথা {
      ss_আকার = আকার(কাঠামো sockaddr_in);
      addrtype = AF_INET;
    }

    ai = curlx_calloc(1, আকার(কাঠামো Curl_addrinfo) + ss_আকার + hostlen);
    যদি(!ai) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      বিরতি;
    }
    ai->ai_addr = (শূন্য *)((অক্ষর *)ai + আকার(কাঠামো Curl_addrinfo));
    ai->ai_canonname = (শূন্য *)((অক্ষর *)ai->ai_addr + ss_আকার);
    memcpy(ai->ai_canonname, hostname, hostlen);

    যদি(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    যদি(prevai)
      /* make the previous entry point to this */
      prevai->ai_পরবর্তী = ai;

    ai->ai_family = addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_আকার;

    /* leave the rest of the struct filled with zero */

    নির্বাচন(ai->ai_family) {
    ক্ষেত্র AF_INET:
      addr = (শূন্য *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(আকার(কাঠামো in_addr) == আকার(de->addr[i].ip.v4));
      memcpy(&addr->sin_addr, &de->addr[i].ip.v4, আকার(কাঠামো in_addr));
      addr->sin_family = addrtype;
      addr->sin_port = htons((unsigned short)port);
      বিরতি;

#ifdef USE_IPV6
    ক্ষেত্র AF_INET6:
      addr6 = (শূন্য *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(আকার(কাঠামো in6_addr) == আকার(de->addr[i].ip.v6));
      memcpy(&addr6->sin6_addr, &de->addr[i].ip.v6, আকার(কাঠামো in6_addr));
      addr6->sin6_family = addrtype;
      addr6->sin6_port = htons((unsigned short)port);
      বিরতি;
#endif
    }

    prevai = ai;
  }

  যদি(ফলাফল) {
    Curl_freeaddrinfo(firstai);
    firstai = ফাঁকা;
  }
  *aip = firstai;

  ফেরত ফলাফল;
}

/* @unittest 1655 */
UNITTEST শূন্য de_cleanup(কাঠামো dohentry *d);
UNITTEST শূন্য de_cleanup(কাঠামো dohentry *d)
{
#ifdef USE_HTTPSRR
  পূর্ণসংখ্যা i = 0;
  জন্য(i = 0; i < d->numhttps_rrs; i++)
    curlx_safefree(d->https_rrs[i].val);
#অন্যথা
  (শূন্য)d;
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
স্থির CURLcode doh_decode_rdata_name(ধ্রুবক unsigned অক্ষর **buf,
                                      size_t *remaining, অক্ষর **dnsname)
{
  ধ্রুবক unsigned অক্ষর *cp = ফাঁকা;
  size_t rem = 0;
  unsigned অক্ষর clen = 0; /* chunk len */
  কাঠামো dynbuf thename;

  DEBUGASSERT(buf && remaining && dnsname);
  যদি(!buf || !remaining || !dnsname || !*remaining)
    ফেরত CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&thename, CURL_MAXLEN_HOST_NAME);
  rem = *remaining;
  cp = *buf;
  clen = *cp++;
  /* RFC 9460 says it must be uncompressed */
  যদি(clen > 63)
    ফেরত CURLE_WEIRD_SERVER_REPLY;

  যদি(clen == 0) {
    /* special case - return "." as name */
    যদি(curlx_dyn_addn(&thename, ".", 1))
      ফেরত CURLE_OUT_OF_MEMORY;
  }
  যখন(clen) {
    যদি(clen >= rem) {
      curlx_dyn_free(&thename);
      ফেরত CURLE_OUT_OF_MEMORY;
    }
    যদি(curlx_dyn_addn(&thename, cp, clen) ||
       curlx_dyn_addn(&thename, ".", 1))
      ফেরত CURLE_TOO_LARGE;

    cp += clen;
    rem -= (clen + 1);
    যদি(rem <= 0) {
      curlx_dyn_free(&thename);
      ফেরত CURLE_OUT_OF_MEMORY;
    }
    clen = *cp++;
    যদি(clen > 63) {
      /* invalid format */
      curlx_dyn_free(&thename);
      ফেরত CURLE_WEIRD_SERVER_REPLY;
    }
  }
  *buf = cp;
  *remaining = rem - 1;
  *dnsname = curlx_dyn_ptr(&thename);
  ফেরত CURLE_OK;
}

/* scan for byte values <= 32 or 127 */
স্থির CURLcode junkscan(ধ্রুবক অক্ষর *url)
{
  ধ্রুবক unsigned অক্ষর *p = (ধ্রুবক unsigned অক্ষর *)url;
  যখন(*p) {
    যদি(*p <= 0x20 || *p == 127)
      ফেরত CURLE_WEIRD_SERVER_REPLY;
    p++;
  }
  ফেরত CURLE_OK;
}

/* @unittest 1658 */
UNITTEST CURLcode doh_resp_decode_httpsrr(কাঠামো Curl_easy *তথ্য,
                                          ধ্রুবক unsigned অক্ষর *cp, size_t len,
                                          কাঠামো Curl_https_rrinfo **hrr);
UNITTEST CURLcode doh_resp_decode_httpsrr(কাঠামো Curl_easy *তথ্য,
                                          ধ্রুবক unsigned অক্ষর *cp, size_t len,
                                          কাঠামো Curl_https_rrinfo **hrr)
{
  uint16_t pcode = 0, plen = 0;
  uint32_t expected_min_pcode = 0;
  কাঠামো Curl_https_rrinfo *lhrr = ফাঁকা;
  অক্ষর *dnsname = ফাঁকা;
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;

  (শূন্য)তথ্য;
  *hrr = ফাঁকা;
  যদি(len <= 2)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  lhrr = curlx_calloc(1, আকার(কাঠামো Curl_https_rrinfo));
  যদি(!lhrr)
    ফেরত CURLE_OUT_OF_MEMORY;
  lhrr->priority = doh_get16bit(cp, 0);
  cp += 2;
  len -= 2;
  যদি(doh_decode_rdata_name(&cp, &len, &dnsname) != CURLE_OK)
    যাও err;
  lhrr->target = dnsname;
  ফলাফল = junkscan(dnsname);
  যদি(ফলাফল) {
    /* unacceptable hostname content */
    যাও err;
  }
  যখন(len >= 4) {
    pcode = doh_get16bit(cp, 0);
    plen = doh_get16bit(cp, 2);
    cp += 4;
    len -= 4;
    যদি(pcode < expected_min_pcode || plen > len) {
      ফলাফল = CURLE_WEIRD_SERVER_REPLY;
      যাও err;
    }
    ফলাফল = Curl_httpsrr_set(lhrr, pcode, cp, plen);
    যদি(ফলাফল)
      যাও err;
    Curl_httpsrr_trace(তথ্য, lhrr);
    cp += plen;
    len -= plen;
    expected_min_pcode = pcode + 1;
  }
  *hrr = lhrr;
  ফেরত CURLE_OK;
err:
  Curl_httpsrr_destroy(lhrr);
  ফেরত ফলাফল;
}

#endif /* USE_HTTPSRR */

/* called from multi when a sub transfer, e.g. doh probe, is done.
 * Parse the response and set the results in the `async` context
 * of master, using the id from the probe's CURL_EZM_DOH_PROBE
 * meta data. */
স্থির শূন্য doh_probe_done(কাঠামো Curl_easy *doh,
                           কাঠামো Curl_easy *master, CURLcode ফলাফল)
{
  কাঠামো Curl_resolv_async *async = ফাঁকা;
  কাঠামো doh_probes *dohp = ফাঁকা;
  কাঠামো doh_request *doh_req = ফাঁকা;
  কাঠামো Curl_addrinfo **pdest_ai;
  কাঠামো dohentry de;
  পূর্ণসংখ্যা slot, httpcode;

  de_init(&de);
  doh_req = Curl_meta_get(doh, CURL_EZM_DOH_PROBE);
  যদি(!doh_req) {
    /* transfer `doh` is not a DoH probe. */
    DEBUGASSERT(0);
    যাও out;
  }

  async = Curl_async_get(master, doh_req->resolv_id);
  যদি(!async) {
    CURL_TRC_DNS(master, "[%u] ignoring outdated DoH response",
                 doh_req->resolv_id);
    যাও out;
  }
  dohp = async->doh;

  জন্য(slot = 0; slot < DOH_SLOT_গুনতি; ++slot) {
    যদি(dohp->probe_mid[slot] == doh->mid)
      বিরতি;
  }
  /* We really should have found the slot where to store the response */
  যদি(slot >= DOH_SLOT_গুনতি) {
    failf(master, "DoH: unknown sub request done");
    DEBUGASSERT(0);
    যাও out;
  }

  async->queries_ongoing--;
  dohp = async->doh;
  httpcode = doh->info.httpcode;
  নির্বাচন(slot) {
  ক্ষেত্র DOH_SLOT_IPV4:
    async->dns_responses |= CURL_DNSQ_A;
    বিরতি;
#ifdef USE_IPV6
  ক্ষেত্র DOH_SLOT_IPV6:
    async->dns_responses |= CURL_DNSQ_AAAA;
    বিরতি;
#endif
#ifdef USE_HTTPSRR
  ক্ষেত্র DOH_SLOT_HTTPS_RR:
    async->dns_responses |= CURL_DNSQ_HTTPS;
    বিরতি;
#endif
  পূর্বনির্ধারিত:
    DEBUGASSERT(0);
    বিরতি;
  }

  যদি(ফলাফল) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: %s",
          doh_type2name(doh_req->dnstype), curl_easy_strerror(ফলাফল));
    যাও out;
  }
  অন্যথা যদি((httpcode < 200) || (httpcode >= 300)) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: HTTP status %d",
          doh_type2name(doh_req->dnstype), httpcode);
    যাও out;
  }

  dohp->probe_rc[slot] = doh_resp_decode(curlx_dyn_uptr(&doh_req->resp_body),
                                         curlx_dyn_len(&doh_req->resp_body),
                                         doh_req->dnstype, &de);
  যদি(dohp->probe_rc[slot]) {
#ifdef USE_HTTPSRR
    যদি((dohp->probe_rc[slot] == DOH_NO_CONTENT) &&
       (doh_req->dnstype == CURL_DNS_TYPE_HTTPS)) {
      dohp->probe_rc[slot] = DOH_DNS_NXDOMAIN;
    }
#endif
    infof(doh, "[DoH] [%s] error decoding response: %s",
          doh_type2name(doh_req->dnstype),
          doh_strerror(dohp->probe_rc[slot]));
    যাও out;
  }

  যদি(doh_req->dnstype == CURL_DNS_TYPE_A)
    pdest_ai = &async->ai_A;
  অন্যথা যদি(doh_req->dnstype == CURL_DNS_TYPE_AAAA)
    pdest_ai = &async->ai_AAAA;
  অন্যথা
    pdest_ai = ফাঁকা;

  যদি(pdest_ai && de.numaddr) {
    যদি(*pdest_ai) {
      Curl_freeaddrinfo(*pdest_ai);
      *pdest_ai = ফাঁকা;
    }
    ফলাফল = doh2ai(&de, async->peer->hostname, async->peer->port, pdest_ai);
    যদি(ফলাফল) { /* hard failure on our side, fail completely */
      infof(doh, "[DoH] [%s] error creating addrinfo: %s",
            doh_type2name(doh_req->dnstype), curl_easy_strerror(ফলাফল));
      dohp->probe_rc[slot] = DOH_OOM;
      async->ফলাফল = ফলাফল;
    }
  }
#ifdef USE_HTTPSRR
  অন্যথা যদি((doh_req->dnstype == CURL_DNS_TYPE_HTTPS) && de.numhttps_rrs) {
    CURL_TRC_DNS(doh, "[HTTPS] got %d records", de.numhttps_rrs);
    ফলাফল = doh_resp_decode_httpsrr(doh, de.https_rrs->val,
                                     de.https_rrs->len, &async->httpsrr);
    যদি(ফলাফল) {
      dohp->probe_rc[slot] = DOH_HTTP_FAILED;
      infof(doh, "[DoH] error decoding HTTPS RR: %s",
            curl_easy_strerror(ফলাফল));
      যাও out;
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

CURLcode Curl_doh_take_ফলাফল(কাঠামো Curl_easy *তথ্য,
                              কাঠামো Curl_resolv_async *async,
                              কাঠামো Curl_dns_entry **pdns)
{
  কাঠামো doh_probes *dohp = async->doh;
  CURLcode ফলাফল = CURLE_OK;

  *pdns = ফাঁকা; /* defaults to no response */
  যদি(!dohp)
    ফেরত CURLE_OUT_OF_MEMORY;

  async->negative_answer = FALSE;
  যদি(async->ফলাফল) {
    ফলাফল = async->ফলাফল;
    যাও out;
  }

  যদি(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     dohp->probe_mid[DOH_SLOT_IPV4] == UINT32_MAX &&
     dohp->probe_mid[DOH_SLOT_IPV6] == UINT32_MAX) {
    failf(তথ্য, "Could not DoH-resolve: %s", async->peer->hostname);
    ফেরত async->for_proxy ?
      CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
  }
  অন্যথা যদি(!async->queries_ongoing) {
    কাঠামো Curl_dns_entry *dns = ফাঁকা;
    bool negative = TRUE;
    পূর্ণসংখ্যা slot;

    /* remove DoH handles from multi handle and close them */
    doh_close(তথ্য, async);
    /* parse the responses, create the struct and return it! */
    জন্য(slot = 0; slot < DOH_SLOT_গুনতি; slot++) {
      /* Failing without an NXDOMAIN answer - a SERVFAIL-class rcode or
         an undecodable response - says nothing about the name. Such a
         failure must not be cached as a negative entry. */
      যদি(dohp->probe_rc[slot] && (dohp->probe_rc[slot] != DOH_DNS_NXDOMAIN))
        negative = FALSE;
    } /* next slot */

    যদি(async->ai_A || async->ai_AAAA) {
      dns = Curl_dnsc_mk_addr2(
        তথ্য, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
      যদি(!dns) {
        ফলাফল = CURLE_OUT_OF_MEMORY;
        যাও out;
      }
    }
#ifdef USE_HTTPSRR
    অন্যথা যদি((async->dns_queries & CURL_DNSQ_HTTPS) &&
            !dohp->probe_rc[DOH_SLOT_HTTPS_RR]) {
      Curl_httpsrr_trace(তথ্য, async->httpsrr);
      dns = Curl_dnsc_mk_https(তথ্য, &async->httpsrr, async->peer);
      যদি(!dns) {
        ফলাফল = CURLE_OUT_OF_MEMORY;
        যাও out;
      }
    }
#endif /* USE_HTTPSRR */
    অন্যথা {
      /* every query failed. Only NXDOMAIN answers for all of them
         make this a negative answer, eligible for caching. */
      async->negative_answer = negative;
      ফলাফল = async->for_proxy ?
        CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
    }

    /* and add the entry to the cache */
    যদি(dns)
      ফলাফল = Curl_dnscache_add(তথ্য, dns);
    *pdns = dns;
  } /* !async->queries_ongoing */
  অন্যথা
    /* wait for pending DoH transactions to complete */
    ফেরত CURLE_AGAIN;

out:
  Curl_doh_cleanup(তথ্য, async);
  ফেরত ফলাফল;
}

স্থির শূন্য doh_close(কাঠামো Curl_easy *তথ্য,
                      কাঠামো Curl_resolv_async *async)
{
  কাঠামো doh_probes *doh = async ? async->doh : ফাঁকা;
  যদি(doh && তথ্য->multi) {
    কাঠামো Curl_easy *probe_তথ্য;
    uint32_t mid;
    size_t slot;
    জন্য(slot = 0; slot < DOH_SLOT_গুনতি; slot++) {
      mid = doh->probe_mid[slot];
      যদি(mid == UINT32_MAX)
        চালু;
      doh->probe_mid[slot] = UINT32_MAX;
      /* should have been called before data is removed from multi handle */
      DEBUGASSERT(তথ্য->multi);
      probe_তথ্য = তথ্য->multi ? Curl_multi_get_easy(তথ্য->multi, mid) : ফাঁকা;
      যদি(!probe_তথ্য) {
        DEBUGF(infof(তথ্য, "Curl_doh_close: xfer for mid=%u not found!", mid));
        চালু;
      }
      probe_তথ্য->sub_xfer_done = ফাঁকা; /* No longer interested in result */
      /* data->multi might already be reset at this time */
      Curl_multi_remove_handle(তথ্য->multi, probe_তথ্য);
      Curl_close(&probe_তথ্য);
    }
    CURL_TRC_DNS(তথ্য, "[DoH] probe done");
  }
}

শূন্য Curl_doh_cleanup(কাঠামো Curl_easy *তথ্য,
                      কাঠামো Curl_resolv_async *async)
{
  কাঠামো doh_probes *dohp = async->doh;
  যদি(dohp) {
    doh_close(তথ্য, async);
    curlx_safefree(async->doh);
  }
}

#endif /* CURL_DISABLE_DOH */
