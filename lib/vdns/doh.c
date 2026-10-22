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
#ضمن "curl_setup.h"

#ifndef CURL_DISABLE_DOH

#ضمن "urldata.h"
#ضمن "curl_addrinfo.h"
#ضمن "curl_trc.h"
#ضمن "multiif.h"
#ضمن "url.h"
#ضمن "connect.h"
#ضمن "vdns/doh.h"
#ضمن "vdns/httpsrr.h"
#ضمن "curlx/strdup.h"
#ضمن "curlx/dynbuf.h"
#ضمن "escape.h"  /* for Curl_hexencode() */
#ضمن "urlapi-int.h"

#define DNS_CLASS_IN 0x01

ساكن فراغ doh_close(بنية Curl_easy *data,
                      بنية Curl_resolv_async *async);

#ifdef CURLVERBOSE
ساكن ثابت حرف * ثابت doh_code_str[] = {
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

ساكن ثابت حرف *doh_strerror(DOHcode code)
{
  ܐܢ((size_t)code < CURL_ARRAYSIZE(doh_code_str))
    ܦܢܐ doh_code_str[code];
  ܦܢܐ "bad error code";
}

ساكن ثابت حرف *doh_type2name(DNStype dnstype)
{
  بدل(dnstype) {
  حالة CURL_DNS_TYPE_A:
    ܦܢܐ "A";
  حالة CURL_DNS_TYPE_AAAA:
    ܦܢܐ "AAAA";
#ifdef USE_HTTPSRR
  حالة CURL_DNS_TYPE_HTTPS:
    ܦܢܐ "HTTPS";
#endif
  افتراضي:
    ܦܢܐ "unknown";
  }
}

#endif /* CURLVERBOSE */

/* @unittest 1655
 */
UNITTEST DOHcode doh_req_encode(ثابت حرف *host,
                                DNStype dnstype,
                                unsigned حرف *dnsp,  /* buffer */
                                size_t len,  /* buffer size */
                                size_t *olen);  /* output length */
UNITTEST DOHcode doh_req_encode(ثابت حرف *host,
                                DNStype dnstype,
                                unsigned حرف *dnsp, /* buffer */
                                size_t len,   /* buffer size */
                                size_t *olen) /* output length */
{
  ثابت size_t hostlen = strlen(host);
  unsigned حرف *orig = dnsp;
  ثابت حرف *hostp = host;

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
  ܐܢ(host[hostlen - 1] != '.')
    expected_len++;

  ܐܢ(expected_len > DOH_MAX_DNSREQ_SIZE)
    ܦܢܐ DOH_DNS_NAME_TOO_LONG;

  ܐܢ(len < expected_len)
    ܦܢܐ DOH_TOO_SMALL_BUFFER;

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
  ܟܡܐ(*hostp) {
    size_t labellen;
    ثابت حرف *dot = strchr(hostp, '.');
    ܐܢ(dot)
      labellen = dot - hostp;
    ܐܚܪܢܐ
      labellen = strlen(hostp);
    ܐܢ((labellen > 63) || (!labellen)) {
      /* label is too long or too short, error out */
      *olen = 0;
      ܦܢܐ DOH_DNS_BAD_LABEL;
    }
    /* label is non-empty, process it */
    *dnsp++ = (unsigned حرف)labellen;
    memcpy(dnsp, hostp, labellen);
    dnsp += labellen;
    hostp += labellen;
    /* advance past dot, but only if there is one */
    ܐܢ(dot)
      hostp++;
  } /* next label */

  *dnsp++ = 0; /* append zero-length label for root */

  /* There are assigned TYPE codes beyond 255: use range [1..65535] */
  *dnsp++ = (unsigned حرف)(255 & (dnstype >> 8)); /* upper 8-bit TYPE */
  *dnsp++ = (unsigned حرف)(255 & dnstype);        /* lower 8-bit TYPE */

  *dnsp++ = '\0'; /* upper 8-bit CLASS */
  *dnsp++ = DNS_CLASS_IN; /* IN - "the Internet" */

  *olen = dnsp - orig;

  /* verify that our estimation of length is valid, since
   * this has led to buffer overflows in this function */
  DEBUGASSERT(*olen == expected_len);
  ܦܢܐ DOH_OK;
}

ساكن size_t doh_probe_write_cb(حرف *contents, size_t size, size_t nmemb,
                                 فراغ *userp)
{
  size_t realsize = size * nmemb;
  بنية Curl_easy *data = userp;
  بنية doh_request *doh_req = Curl_meta_get(data, CURL_EZM_DOH_PROBE);
  ܐܢ(!doh_req)
    ܦܢܐ CURL_WRITEFUNC_ERROR;

  ܐܢ(curlx_dyn_addn(&doh_req->resp_body, contents, realsize))
    ܦܢܐ 0;

  ܦܢܐ realsize;
}

ساكن فراغ doh_probe_done(بنية Curl_easy *doh,
                           بنية Curl_easy *master, CURLcode result);
ساكن فراغ doh_probe_dtor(ثابت فراغ *key, size_t klen, فراغ *e)
{
  (فراغ)key;
  (فراغ)klen;
  ܐܢ(e) {
    بنية doh_request *doh_req = e;
    curl_slist_free_all(doh_req->req_hds);
    curlx_dyn_free(&doh_req->resp_body);
    curlx_free(e);
  }
}

#define ERROR_CHECK_SETOPT(x, y)                        \
  ܥܒܕ {                                                  \
    result = curl_easy_setopt((CURL *)doh, x, y);       \
    ܐܢ(result &&                                        \
       result != CURLE_NOT_BUILT_IN &&                  \
       result != CURLE_UNKNOWN_OPTION)                  \
      اذهب error;                                       \
  } ܟܡܐ(0)

ساكن CURLcode doh_probe_run(بنية Curl_easy *data,
                              DNStype dnstype,
                              ثابت حرف *host,
                              ثابت حرف *url, CURLM *multi,
                              uint32_t resolv_id,
                              uint32_t *pmid)
{
  بنية Curl_easy *doh = عدم;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  بنية doh_request *doh_req;
  DOHcode d;
  bool maybe_https = !curl_strnequal(url, STRCONST("http:"));

  *pmid = UINT32_MAX;

  doh_req = curlx_calloc(1, حجم(*doh_req));
  ܐܢ(!doh_req)
    ܦܢܐ CURLE_OUT_OF_MEMORY;
  doh_req->resolv_id = resolv_id;
  doh_req->dnstype = dnstype;
  curlx_dyn_init(&doh_req->resp_body, DYN_DOH_RESPONSE);

  d = doh_req_encode(host, dnstype, doh_req->req_body,
                     حجم(doh_req->req_body),
                     &doh_req->req_body_len);
  ܐܢ(d) {
    failf(data, "Failed to encode DoH packet [%d]", (صحيح)d);
    result = CURLE_OUT_OF_MEMORY;
    اذهب error;
  }

  timeout_ms = Curl_timeleft_ms(data);
  ܐܢ(timeout_ms < 0) {
    result = CURLE_OPERATION_TIMEDOUT;
    اذهب error;
  }

  doh_req->req_hds =
    curl_slist_append(عدم, "Content-Type: application/dns-message");
  ܐܢ(!doh_req->req_hds) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب error;
  }

  /* Curl_open() is the internal version of curl_easy_init() */
  result = Curl_open(&doh);
  ܐܢ(result)
    اذهب error;

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
  ܐܢ(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);
    ERROR_CHECK_SETOPT(CURLOPT_PIPEWAIT, 1L);
  }
#endif
#ifndef DEBUGBUILD
  /* enforce HTTPS if not debug */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
#ܐܚܪܢܐ
  /* in debug mode, also allow http */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
#endif
  ERROR_CHECK_SETOPT(CURLOPT_TIMEOUT_MS, (long)timeout_ms);
  ERROR_CHECK_SETOPT(CURLOPT_SHARE, (CURLSH *)data->share);
  ܐܢ(data->set.err && data->set.err != stderr)
    ERROR_CHECK_SETOPT(CURLOPT_STDERR, data->set.err);
  ܐܢ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_doh))
    ERROR_CHECK_SETOPT(CURLOPT_VERBOSE, 1L);
  ܐܢ(data->set.no_signal)
    ERROR_CHECK_SETOPT(CURLOPT_NOSIGNAL, 1L);
  ܐܢ(data->set.fdebug)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGFUNCTION, data->set.fdebug);
  ܐܢ(data->set.debugdata)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGDATA, data->set.debugdata);

#ifdef USE_RESOLV_ARES
  ܐܢ(CURL_EASY_STR(data, STRING_DNS_SERVERS)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_SERVERS,
                       CURL_EASY_STR(data, STRING_DNS_SERVERS));
  }
  ܐܢ(CURL_EASY_STR(data, STRING_DNS_INTERFACE)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_INTERFACE,
                       CURL_EASY_STR(data, STRING_DNS_INTERFACE));
  }
  ܐܢ(CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP4,
                       CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4));
  }
  ܐܢ(CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP6,
                       CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6));
  }
#endif
  ܐܢ(maybe_https) {
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
    ܐܢ(CURL_EASY_STR(data, STRING_SSL_CAFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO,
                         CURL_EASY_STR(data, STRING_SSL_CAFILE));
    }
    ܐܢ(data->set.blobs[BLOB_CAINFO]) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO_BLOB, data->set.blobs[BLOB_CAINFO]);
    }
    ܐܢ(CURL_EASY_STR(data, STRING_SSL_CAPATH)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAPATH,
                         CURL_EASY_STR(data, STRING_SSL_CAPATH));
    }
    ܐܢ(CURL_EASY_STR(data, STRING_SSL_CRLFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CRLFILE,
                         CURL_EASY_STR(data, STRING_SSL_CRLFILE));
    }
    ܐܢ(data->set.ssl.certinfo)
      ERROR_CHECK_SETOPT(CURLOPT_CERTINFO, 1L);
    ܐܢ(data->set.ssl_fsslctx)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_FUNCTION, data->set.ssl_fsslctx);
    ܐܢ(data->set.ssl_fsslctxp)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_DATA, data->set.ssl_fsslctxp);
    ܐܢ(CURL_EASY_STR(data, STRING_SSL_EC_CURVES)) {
      ERROR_CHECK_SETOPT(CURLOPT_SSL_EC_CURVES,
                         CURL_EASY_STR(data, STRING_SSL_EC_CURVES));
    }
    (فراغ)curl_easy_setopt(doh, CURLOPT_SSL_OPTIONS,
                           ((long)data->set.ssl.ssl_options &
                            ~CURLSSLOPT_AUTO_CLIENT_CERT));
  }

  doh->state.internal = TRUE;
  doh->master_mid = data->mid; /* master transfer of this one */
  doh->sub_xfer_done = doh_probe_done;

  result = Curl_meta_set(doh, CURL_EZM_DOH_PROBE, doh_req, doh_probe_dtor);
  doh_req = عدم; /* call took ownership */
  ܐܢ(result)
    اذهب error;

  /* DoH handles must not inherit private_data. The handles may be passed to
     the user via callbacks and the user will be able to identify them as
     internal handles because private data is not set. The user can then set
     private_data via CURLOPT_PRIVATE if they so choose. */
  DEBUGASSERT(!doh->set.private_data);

  ܐܢ(Curl_multi_add_handle(multi, doh))
    اذهب error;

  *pmid = doh->mid;
  ܦܢܐ CURLE_OK;

error:
  Curl_close(&doh);
  ܐܢ(doh_req)
    doh_probe_dtor(عدم, 0, doh_req);
  ܦܢܐ result;
}

/*
 * Curl_doh() starts a name resolve using DoH. It resolves a name and returns
 * a 'Curl_addrinfo *' with the address information.
 */

CURLcode Curl_doh(بنية Curl_easy *data,
                  بنية Curl_resolv_async *async)
{
  CURLcode result = CURLE_OK;
  بنية doh_probes *dohp = عدم;
  size_t i;

  DEBUGASSERT(!async->doh);
  DEBUGASSERT(async->peer->hostname[0]);
  ܐܢ(async->doh) {
    DEBUGASSERT(0); /* should not happen */
    Curl_doh_cleanup(data, async);
  }

  ܐܢ(!async->dns_queries)
    ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef USE_HTTPSRR
  ܐܢ(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     (async->dns_queries & CURL_DNSQ_HTTPS)) {
    /* Can't mix those in the same async resolve */
    DEBUGASSERT(0);
    ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
  }
#ܐܚܪܢܐ
  ܐܢ(async->dns_queries & CURL_DNSQ_HTTPS) {
    DEBUGASSERT(0);
    ܦܢܐ CURLE_NOT_BUILT_IN;
  }
#endif

  /* start clean, consider allocating this struct on demand */
  async->doh = dohp = curlx_calloc(1, حجم(بنية doh_probes));
  ܐܢ(!dohp)
    ܦܢܐ CURLE_OUT_OF_MEMORY;

  لكل(i = 0; i < DOH_SLOT_COUNT; ++i) {
    dohp->probe_rc[i] = DOH_OK;
    dohp->probe_mid[i] = UINT32_MAX;
  }

#ifdef USE_IPV6
  /* AAAA results have preference in happy eyeballing, trigger first */
  ܐܢ(async->dns_queries & CURL_DNSQ_AAAA) {
    /* create IPv6 DoH request */
    result = doh_probe_run(data, CURL_DNS_TYPE_AAAA,
                           async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH),
                           data->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV6]);
    ܐܢ(result)
      اذهب error;
    async->queries_ongoing++;
  }
#endif

  /* create IPv4 DoH request */
  ܐܢ(async->dns_queries & CURL_DNSQ_A) {
    result = doh_probe_run(data, CURL_DNS_TYPE_A,
                           async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH),
                           data->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV4]);
    ܐܢ(result)
      اذهب error;
    async->queries_ongoing++;
  }

#ifdef USE_HTTPSRR
  ܐܢ(async->dns_queries & CURL_DNSQ_HTTPS) {
    حرف *qname = عدم;
    ܐܢ(async->peer->port != PORT_HTTPS) {
      qname = curl_maprintf("_%u._https.%s",
                            async->peer->port, async->peer->hostname);
      ܐܢ(!qname)
        اذهب error;
    }
    result = doh_probe_run(data, CURL_DNS_TYPE_HTTPS,
                           qname ? qname : async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH), data->multi,
                           async->id,
                           &dohp->probe_mid[DOH_SLOT_HTTPS_RR]);
    curlx_free(qname);
    ܐܢ(result)
      اذهب error;
    async->queries_ongoing++;
  }
#endif
  ܦܢܐ CURLE_OK;

error:
  Curl_doh_cleanup(data, async);
  ܦܢܐ result;
}

ساكن DOHcode doh_skipqname(ثابت unsigned حرف *doh, size_t dohlen,
                             unsigned صحيح *indexp)
{
  unsigned حرف length;
  ܥܒܕ {
    ܐܢ(dohlen < (*indexp + 1))
      ܦܢܐ DOH_DNS_OUT_OF_RANGE;
    length = doh[*indexp];
    ܐܢ((length & 0xc0) == 0xc0) {
      /* name pointer, advance over it and be done */
      ܐܢ(dohlen < (*indexp + 2))
        ܦܢܐ DOH_DNS_OUT_OF_RANGE;
      *indexp += 2;
      اكسر;
    }
    ܐܢ(length & 0xc0)
      ܦܢܐ DOH_DNS_BAD_LABEL;
    ܐܢ(dohlen < (*indexp + 1 + length))
      ܦܢܐ DOH_DNS_OUT_OF_RANGE;
    *indexp += (unsigned صحيح)(1 + length);
  } ܟܡܐ(length);
  ܦܢܐ DOH_OK;
}

ساكن unsigned short doh_get16bit(ثابت unsigned حرف *doh,
                                   unsigned صحيح index)
{
  ܦܢܐ (unsigned short)((doh[index] << 8) | doh[index + 1]);
}

ساكن unsigned صحيح doh_get32bit(ثابت unsigned حرف *doh, unsigned صحيح index)
{
  /* make clang and gcc optimize this to bswap by incrementing
     the pointer first. */
  doh += index;

  /* avoid undefined behavior by casting to unsigned before shifting
     24 bits, possibly into the sign bit. codegen is same, but
     ub sanitizer will not be upset */
  ܦܢܐ ((unsigned)doh[0] << 24) | ((unsigned)doh[1] << 16) |
         ((unsigned)doh[2] << 8) | doh[3];
}

ساكن فراغ doh_store_a(ثابت unsigned حرف *doh, صحيح index,
                        بنية dohentry *d)
{
  /* silently ignore addresses over the limit */
  ܐܢ(d->numaddr < DOH_MAX_ADDR) {
    بنية dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_A;
    memcpy(&a->ip.v4, &doh[index], 4);
    d->numaddr++;
  }
}

ساكن فراغ doh_store_aaaa(ثابت unsigned حرف *doh, صحيح index,
                           بنية dohentry *d)
{
  /* silently ignore addresses over the limit */
  ܐܢ(d->numaddr < DOH_MAX_ADDR) {
    بنية dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_AAAA;
    memcpy(&a->ip.v6, &doh[index], 16);
    d->numaddr++;
  }
}

#ifdef USE_HTTPSRR
ساكن DOHcode doh_store_https(ثابت unsigned حرف *doh, صحيح index,
                               بنية dohentry *d, uint16_t len)
{
  /* silently ignore RRs over the limit */
  ܐܢ(d->numhttps_rrs < DOH_MAX_HTTPS) {
    بنية dohhttps_rr *h = &d->https_rrs[d->numhttps_rrs];
    h->val = curlx_memdup(&doh[index], len);
    ܐܢ(!h->val)
      ܦܢܐ DOH_OUT_OF_MEM;
    h->len = len;
    d->numhttps_rrs++;
  }
  ܦܢܐ DOH_OK;
}
#endif

ساكن DOHcode doh_rdata(ثابت unsigned حرف *doh,
                         unsigned short rdlength,
                         unsigned short type,
                         صحيح index,
                         بنية dohentry *d)
{
  /* RDATA
     - A (TYPE 1): 4 bytes
     - AAAA (TYPE 28): 16 bytes
     - HTTPS (TYPE 65): N bytes */
  بدل(type) {
  حالة CURL_DNS_TYPE_A:
    ܐܢ(rdlength != 4)
      ܦܢܐ DOH_DNS_RDATA_LEN;
    doh_store_a(doh, index, d);
    اكسر;
  حالة CURL_DNS_TYPE_AAAA:
    ܐܢ(rdlength != 16)
      ܦܢܐ DOH_DNS_RDATA_LEN;
    doh_store_aaaa(doh, index, d);
    اكسر;
#ifdef USE_HTTPSRR
  حالة CURL_DNS_TYPE_HTTPS:
    ܐܢ(rdlength < 3)
      ܦܢܐ DOH_DNS_RDATA_LEN;
    ܦܢܐ doh_store_https(doh, index, d, rdlength);
#endif
  افتراضي:
    /* unsupported type, or type we do not store, skip it */
    اكسر;
  }
  ܦܢܐ DOH_OK;
}

/* @unittest 1655 */
UNITTEST فراغ de_init(بنية dohentry *de);
UNITTEST فراغ de_init(بنية dohentry *de)
{
  memset(de, 0, حجم(*de));
  de->ttl = INT_MAX;
}

/* TTL value cap */
#define MAX_DNS_TTL 86400U /* 24 hours */
/* @unittest 1650 */
UNITTEST DOHcode doh_resp_decode(ثابت unsigned حرف *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 بنية dohentry *d);
UNITTEST DOHcode doh_resp_decode(ثابت unsigned حرف *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 بنية dohentry *d)
{
  unsigned حرف rcode;
  unsigned short qdcount;
  unsigned short ancount;
  unsigned short type = 0;
  unsigned short rdlength;
  unsigned short nscount;
  unsigned short arcount;
  unsigned صحيح index = 12;
  DOHcode rc;

  ܐܢ(dohlen < 12)
    ܦܢܐ DOH_TOO_SMALL_BUFFER; /* too small */
  ܐܢ(!doh || doh[0] || doh[1])
    ܦܢܐ DOH_DNS_BAD_ID; /* bad ID */
  rcode = doh[3] & 0x0f;
  ܐܢ(rcode == 3)
    ܦܢܐ DOH_DNS_NXDOMAIN; /* name does not exist */
  ܐܢ(rcode)
    ܦܢܐ DOH_DNS_BAD_RCODE; /* bad rcode */

  qdcount = doh_get16bit(doh, 4);
  ܟܡܐ(qdcount) {
    rc = doh_skipqname(doh, dohlen, &index);
    ܐܢ(rc)
      ܦܢܐ rc; /* bad qname */
    ܐܢ(dohlen < (index + 4))
      ܦܢܐ DOH_DNS_OUT_OF_RANGE;
    index += 4; /* skip question's type and class */
    qdcount--;
  }

  ancount = doh_get16bit(doh, 6);
  ܟܡܐ(ancount) {
    unsigned short dnsclass;
    unsigned صحيح ttl;

    rc = doh_skipqname(doh, dohlen, &index);
    ܐܢ(rc)
      ܦܢܐ rc; /* bad qname */

    ܐܢ(dohlen < (index + 2))
      ܦܢܐ DOH_DNS_OUT_OF_RANGE;

    type = doh_get16bit(doh, index);
    ܐܢ((type != CURL_DNS_TYPE_CNAME) &&  /* may be synthesized from DNAME */
       (type != CURL_DNS_TYPE_DNAME) &&  /* if present, accept and ignore */
       (type != dnstype))
      /* Not the same type as was asked for, nor CNAME nor DNAME */
      ܦܢܐ DOH_DNS_UNEXPECTED_TYPE;
    index += 2;

    ܐܢ(dohlen < (index + 2))
      ܦܢܐ DOH_DNS_OUT_OF_RANGE;
    dnsclass = doh_get16bit(doh, index);
    ܐܢ(DNS_CLASS_IN != dnsclass)
      ܦܢܐ DOH_DNS_UNEXPECTED_CLASS; /* unsupported */
    index += 2;

    ܐܢ(dohlen < (index + 4))
      ܦܢܐ DOH_DNS_OUT_OF_RANGE;

    ttl = doh_get32bit(doh, index);
    ܐܢ(ttl > MAX_DNS_TTL)
      ttl = MAX_DNS_TTL;
    ܐܢ(ttl < d->ttl)
      d->ttl = ttl;
    index += 4;

    ܐܢ(dohlen < (index + 2))
      ܦܢܐ DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    ܐܢ(dohlen < (index + rdlength))
      ܦܢܐ DOH_DNS_OUT_OF_RANGE;

    rc = doh_rdata(doh, rdlength, type, (صحيح)index, d);
    ܐܢ(rc)
      ܦܢܐ rc;
    index += rdlength;
    ancount--;
  }

  nscount = doh_get16bit(doh, 8);
  ܟܡܐ(nscount) {
    rc = doh_skipqname(doh, dohlen, &index);
    ܐܢ(rc)
      ܦܢܐ rc; /* bad qname */

    ܐܢ(dohlen < (index + 8))
      ܦܢܐ DOH_DNS_OUT_OF_RANGE;

    index += 2 + 2 + 4; /* type, dnsclass and ttl */

    ܐܢ(dohlen < (index + 2))
      ܦܢܐ DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    ܐܢ(dohlen < (index + rdlength))
      ܦܢܐ DOH_DNS_OUT_OF_RANGE;
    index += rdlength;
    nscount--;
  }

  arcount = doh_get16bit(doh, 10);
  ܟܡܐ(arcount) {
    rc = doh_skipqname(doh, dohlen, &index);
    ܐܢ(rc)
      ܦܢܐ rc; /* bad qname */

    ܐܢ(dohlen < (index + 8))
      ܦܢܐ DOH_DNS_OUT_OF_RANGE;

    index += 2 + 2 + 4; /* type, dnsclass and ttl */

    ܐܢ(dohlen < (index + 2))
      ܦܢܐ DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    ܐܢ(dohlen < (index + rdlength))
      ܦܢܐ DOH_DNS_OUT_OF_RANGE;
    index += rdlength;
    arcount--;
  }

  ܐܢ(index != dohlen)
    ܦܢܐ DOH_DNS_MALFORMAT; /* something is wrong */

  ܦܢܐ DOH_OK; /* ok */
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
ساكن CURLcode doh2ai(ثابت بنية dohentry *de, ثابت حرف *hostname,
                       صحيح port, بنية Curl_addrinfo **aip)
{
  بنية Curl_addrinfo *ai;
  بنية Curl_addrinfo *prevai = عدم;
  بنية Curl_addrinfo *firstai = عدم;
  بنية sockaddr_in *addr;
#ifdef USE_IPV6
  بنية sockaddr_in6 *addr6;
#endif
  size_t hostlen = strlen(hostname) + 1; /* include null-terminator */
  CURLcode result = CURLE_OK;
  صحيح i;

  لكل(i = 0; i < de->numaddr; i++) {
    size_t ss_size;
    CURL_SA_FAMILY_T addrtype;
    ܐܢ(de->addr[i].type == CURL_DNS_TYPE_AAAA) {
#ifndef USE_IPV6
      /* we cannot handle IPv6 addresses */
      استمر;
#ܐܚܪܢܐ
      ss_size = حجم(بنية sockaddr_in6);
      addrtype = AF_INET6;
#endif
    }
    ܐܚܪܢܐ {
      ss_size = حجم(بنية sockaddr_in);
      addrtype = AF_INET;
    }

    ai = curlx_calloc(1, حجم(بنية Curl_addrinfo) + ss_size + hostlen);
    ܐܢ(!ai) {
      result = CURLE_OUT_OF_MEMORY;
      اكسر;
    }
    ai->ai_addr = (فراغ *)((حرف *)ai + حجم(بنية Curl_addrinfo));
    ai->ai_canonname = (فراغ *)((حرف *)ai->ai_addr + ss_size);
    memcpy(ai->ai_canonname, hostname, hostlen);

    ܐܢ(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    ܐܢ(prevai)
      /* make the previous entry point to this */
      prevai->ai_next = ai;

    ai->ai_family = addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_size;

    /* leave the rest of the struct filled with zero */

    بدل(ai->ai_family) {
    حالة AF_INET:
      addr = (فراغ *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(حجم(بنية in_addr) == حجم(de->addr[i].ip.v4));
      memcpy(&addr->sin_addr, &de->addr[i].ip.v4, حجم(بنية in_addr));
      addr->sin_family = addrtype;
      addr->sin_port = htons((unsigned short)port);
      اكسر;

#ifdef USE_IPV6
    حالة AF_INET6:
      addr6 = (فراغ *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(حجم(بنية in6_addr) == حجم(de->addr[i].ip.v6));
      memcpy(&addr6->sin6_addr, &de->addr[i].ip.v6, حجم(بنية in6_addr));
      addr6->sin6_family = addrtype;
      addr6->sin6_port = htons((unsigned short)port);
      اكسر;
#endif
    }

    prevai = ai;
  }

  ܐܢ(result) {
    Curl_freeaddrinfo(firstai);
    firstai = عدم;
  }
  *aip = firstai;

  ܦܢܐ result;
}

/* @unittest 1655 */
UNITTEST فراغ de_cleanup(بنية dohentry *d);
UNITTEST فراغ de_cleanup(بنية dohentry *d)
{
#ifdef USE_HTTPSRR
  صحيح i = 0;
  لكل(i = 0; i < d->numhttps_rrs; i++)
    curlx_safefree(d->https_rrs[i].val);
#ܐܚܪܢܐ
  (فراغ)d;
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
ساكن CURLcode doh_decode_rdata_name(ثابت unsigned حرف **buf,
                                      size_t *remaining, حرف **dnsname)
{
  ثابت unsigned حرف *cp = عدم;
  size_t rem = 0;
  unsigned حرف clen = 0; /* chunk len */
  بنية dynbuf thename;

  DEBUGASSERT(buf && remaining && dnsname);
  ܐܢ(!buf || !remaining || !dnsname || !*remaining)
    ܦܢܐ CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&thename, CURL_MAXLEN_HOST_NAME);
  rem = *remaining;
  cp = *buf;
  clen = *cp++;
  /* RFC 9460 says it must be uncompressed */
  ܐܢ(clen > 63)
    ܦܢܐ CURLE_WEIRD_SERVER_REPLY;

  ܐܢ(clen == 0) {
    /* special case - return "." as name */
    ܐܢ(curlx_dyn_addn(&thename, ".", 1))
      ܦܢܐ CURLE_OUT_OF_MEMORY;
  }
  ܟܡܐ(clen) {
    ܐܢ(clen >= rem) {
      curlx_dyn_free(&thename);
      ܦܢܐ CURLE_OUT_OF_MEMORY;
    }
    ܐܢ(curlx_dyn_addn(&thename, cp, clen) ||
       curlx_dyn_addn(&thename, ".", 1))
      ܦܢܐ CURLE_TOO_LARGE;

    cp += clen;
    rem -= (clen + 1);
    ܐܢ(rem <= 0) {
      curlx_dyn_free(&thename);
      ܦܢܐ CURLE_OUT_OF_MEMORY;
    }
    clen = *cp++;
    ܐܢ(clen > 63) {
      /* invalid format */
      curlx_dyn_free(&thename);
      ܦܢܐ CURLE_WEIRD_SERVER_REPLY;
    }
  }
  *buf = cp;
  *remaining = rem - 1;
  *dnsname = curlx_dyn_ptr(&thename);
  ܦܢܐ CURLE_OK;
}

/* scan for byte values <= 32 or 127 */
ساكن CURLcode junkscan(ثابت حرف *url)
{
  ثابت unsigned حرف *p = (ثابت unsigned حرف *)url;
  ܟܡܐ(*p) {
    ܐܢ(*p <= 0x20 || *p == 127)
      ܦܢܐ CURLE_WEIRD_SERVER_REPLY;
    p++;
  }
  ܦܢܐ CURLE_OK;
}

/* @unittest 1658 */
UNITTEST CURLcode doh_resp_decode_httpsrr(بنية Curl_easy *data,
                                          ثابت unsigned حرف *cp, size_t len,
                                          بنية Curl_https_rrinfo **hrr);
UNITTEST CURLcode doh_resp_decode_httpsrr(بنية Curl_easy *data,
                                          ثابت unsigned حرف *cp, size_t len,
                                          بنية Curl_https_rrinfo **hrr)
{
  uint16_t pcode = 0, plen = 0;
  uint32_t expected_min_pcode = 0;
  بنية Curl_https_rrinfo *lhrr = عدم;
  حرف *dnsname = عدم;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (فراغ)data;
  *hrr = عدم;
  ܐܢ(len <= 2)
    ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
  lhrr = curlx_calloc(1, حجم(بنية Curl_https_rrinfo));
  ܐܢ(!lhrr)
    ܦܢܐ CURLE_OUT_OF_MEMORY;
  lhrr->priority = doh_get16bit(cp, 0);
  cp += 2;
  len -= 2;
  ܐܢ(doh_decode_rdata_name(&cp, &len, &dnsname) != CURLE_OK)
    اذهب err;
  lhrr->target = dnsname;
  result = junkscan(dnsname);
  ܐܢ(result) {
    /* unacceptable hostname content */
    اذهب err;
  }
  ܟܡܐ(len >= 4) {
    pcode = doh_get16bit(cp, 0);
    plen = doh_get16bit(cp, 2);
    cp += 4;
    len -= 4;
    ܐܢ(pcode < expected_min_pcode || plen > len) {
      result = CURLE_WEIRD_SERVER_REPLY;
      اذهب err;
    }
    result = Curl_httpsrr_set(lhrr, pcode, cp, plen);
    ܐܢ(result)
      اذهب err;
    Curl_httpsrr_trace(data, lhrr);
    cp += plen;
    len -= plen;
    expected_min_pcode = pcode + 1;
  }
  *hrr = lhrr;
  ܦܢܐ CURLE_OK;
err:
  Curl_httpsrr_destroy(lhrr);
  ܦܢܐ result;
}

#endif /* USE_HTTPSRR */

/* called from multi when a sub transfer, e.g. doh probe, is done.
 * Parse the response and set the results in the `async` context
 * of master, using the id from the probe's CURL_EZM_DOH_PROBE
 * meta data. */
ساكن فراغ doh_probe_done(بنية Curl_easy *doh,
                           بنية Curl_easy *master, CURLcode result)
{
  بنية Curl_resolv_async *async = عدم;
  بنية doh_probes *dohp = عدم;
  بنية doh_request *doh_req = عدم;
  بنية Curl_addrinfo **pdest_ai;
  بنية dohentry de;
  صحيح slot, httpcode;

  de_init(&de);
  doh_req = Curl_meta_get(doh, CURL_EZM_DOH_PROBE);
  ܐܢ(!doh_req) {
    /* transfer `doh` is not a DoH probe. */
    DEBUGASSERT(0);
    اذهب out;
  }

  async = Curl_async_get(master, doh_req->resolv_id);
  ܐܢ(!async) {
    CURL_TRC_DNS(master, "[%u] ignoring outdated DoH response",
                 doh_req->resolv_id);
    اذهب out;
  }
  dohp = async->doh;

  لكل(slot = 0; slot < DOH_SLOT_COUNT; ++slot) {
    ܐܢ(dohp->probe_mid[slot] == doh->mid)
      اكسر;
  }
  /* We really should have found the slot where to store the response */
  ܐܢ(slot >= DOH_SLOT_COUNT) {
    failf(master, "DoH: unknown sub request done");
    DEBUGASSERT(0);
    اذهب out;
  }

  async->queries_ongoing--;
  dohp = async->doh;
  httpcode = doh->info.httpcode;
  بدل(slot) {
  حالة DOH_SLOT_IPV4:
    async->dns_responses |= CURL_DNSQ_A;
    اكسر;
#ifdef USE_IPV6
  حالة DOH_SLOT_IPV6:
    async->dns_responses |= CURL_DNSQ_AAAA;
    اكسر;
#endif
#ifdef USE_HTTPSRR
  حالة DOH_SLOT_HTTPS_RR:
    async->dns_responses |= CURL_DNSQ_HTTPS;
    اكسر;
#endif
  افتراضي:
    DEBUGASSERT(0);
    اكسر;
  }

  ܐܢ(result) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: %s",
          doh_type2name(doh_req->dnstype), curl_easy_strerror(result));
    اذهب out;
  }
  ܐܚܪܢܐ ܐܢ((httpcode < 200) || (httpcode >= 300)) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: HTTP status %d",
          doh_type2name(doh_req->dnstype), httpcode);
    اذهب out;
  }

  dohp->probe_rc[slot] = doh_resp_decode(curlx_dyn_uptr(&doh_req->resp_body),
                                         curlx_dyn_len(&doh_req->resp_body),
                                         doh_req->dnstype, &de);
  ܐܢ(dohp->probe_rc[slot]) {
#ifdef USE_HTTPSRR
    ܐܢ((dohp->probe_rc[slot] == DOH_NO_CONTENT) &&
       (doh_req->dnstype == CURL_DNS_TYPE_HTTPS)) {
      dohp->probe_rc[slot] = DOH_DNS_NXDOMAIN;
    }
#endif
    infof(doh, "[DoH] [%s] error decoding response: %s",
          doh_type2name(doh_req->dnstype),
          doh_strerror(dohp->probe_rc[slot]));
    اذهب out;
  }

  ܐܢ(doh_req->dnstype == CURL_DNS_TYPE_A)
    pdest_ai = &async->ai_A;
  ܐܚܪܢܐ ܐܢ(doh_req->dnstype == CURL_DNS_TYPE_AAAA)
    pdest_ai = &async->ai_AAAA;
  ܐܚܪܢܐ
    pdest_ai = عدم;

  ܐܢ(pdest_ai && de.numaddr) {
    ܐܢ(*pdest_ai) {
      Curl_freeaddrinfo(*pdest_ai);
      *pdest_ai = عدم;
    }
    result = doh2ai(&de, async->peer->hostname, async->peer->port, pdest_ai);
    ܐܢ(result) { /* hard failure on our side, fail completely */
      infof(doh, "[DoH] [%s] error creating addrinfo: %s",
            doh_type2name(doh_req->dnstype), curl_easy_strerror(result));
      dohp->probe_rc[slot] = DOH_OOM;
      async->result = result;
    }
  }
#ifdef USE_HTTPSRR
  ܐܚܪܢܐ ܐܢ((doh_req->dnstype == CURL_DNS_TYPE_HTTPS) && de.numhttps_rrs) {
    CURL_TRC_DNS(doh, "[HTTPS] got %d records", de.numhttps_rrs);
    result = doh_resp_decode_httpsrr(doh, de.https_rrs->val,
                                     de.https_rrs->len, &async->httpsrr);
    ܐܢ(result) {
      dohp->probe_rc[slot] = DOH_HTTP_FAILED;
      infof(doh, "[DoH] error decoding HTTPS RR: %s",
            curl_easy_strerror(result));
      اذهب out;
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

CURLcode Curl_doh_take_result(بنية Curl_easy *data,
                              بنية Curl_resolv_async *async,
                              بنية Curl_dns_entry **pdns)
{
  بنية doh_probes *dohp = async->doh;
  CURLcode result = CURLE_OK;

  *pdns = عدم; /* defaults to no response */
  ܐܢ(!dohp)
    ܦܢܐ CURLE_OUT_OF_MEMORY;

  async->negative_answer = FALSE;
  ܐܢ(async->result) {
    result = async->result;
    اذهب out;
  }

  ܐܢ(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     dohp->probe_mid[DOH_SLOT_IPV4] == UINT32_MAX &&
     dohp->probe_mid[DOH_SLOT_IPV6] == UINT32_MAX) {
    failf(data, "Could not DoH-resolve: %s", async->peer->hostname);
    ܦܢܐ async->for_proxy ?
      CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
  }
  ܐܚܪܢܐ ܐܢ(!async->queries_ongoing) {
    بنية Curl_dns_entry *dns = عدم;
    bool negative = TRUE;
    صحيح slot;

    /* remove DoH handles from multi handle and close them */
    doh_close(data, async);
    /* parse the responses, create the struct and return it! */
    لكل(slot = 0; slot < DOH_SLOT_COUNT; slot++) {
      /* Failing without an NXDOMAIN answer - a SERVFAIL-class rcode or
         an undecodable response - says nothing about the name. Such a
         failure must not be cached as a negative entry. */
      ܐܢ(dohp->probe_rc[slot] && (dohp->probe_rc[slot] != DOH_DNS_NXDOMAIN))
        negative = FALSE;
    } /* next slot */

    ܐܢ(async->ai_A || async->ai_AAAA) {
      dns = Curl_dnsc_mk_addr2(
        data, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
      ܐܢ(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        اذهب out;
      }
    }
#ifdef USE_HTTPSRR
    ܐܚܪܢܐ ܐܢ((async->dns_queries & CURL_DNSQ_HTTPS) &&
            !dohp->probe_rc[DOH_SLOT_HTTPS_RR]) {
      Curl_httpsrr_trace(data, async->httpsrr);
      dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
      ܐܢ(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        اذهب out;
      }
    }
#endif /* USE_HTTPSRR */
    ܐܚܪܢܐ {
      /* every query failed. Only NXDOMAIN answers for all of them
         make this a negative answer, eligible for caching. */
      async->negative_answer = negative;
      result = async->for_proxy ?
        CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
    }

    /* and add the entry to the cache */
    ܐܢ(dns)
      result = Curl_dnscache_add(data, dns);
    *pdns = dns;
  } /* !async->queries_ongoing */
  ܐܚܪܢܐ
    /* wait for pending DoH transactions to complete */
    ܦܢܐ CURLE_AGAIN;

out:
  Curl_doh_cleanup(data, async);
  ܦܢܐ result;
}

ساكن فراغ doh_close(بنية Curl_easy *data,
                      بنية Curl_resolv_async *async)
{
  بنية doh_probes *doh = async ? async->doh : عدم;
  ܐܢ(doh && data->multi) {
    بنية Curl_easy *probe_data;
    uint32_t mid;
    size_t slot;
    لكل(slot = 0; slot < DOH_SLOT_COUNT; slot++) {
      mid = doh->probe_mid[slot];
      ܐܢ(mid == UINT32_MAX)
        استمر;
      doh->probe_mid[slot] = UINT32_MAX;
      /* should have been called before data is removed from multi handle */
      DEBUGASSERT(data->multi);
      probe_data = data->multi ? Curl_multi_get_easy(data->multi, mid) : عدم;
      ܐܢ(!probe_data) {
        DEBUGF(infof(data, "Curl_doh_close: xfer for mid=%u not found!", mid));
        استمر;
      }
      probe_data->sub_xfer_done = عدم; /* No longer interested in result */
      /* data->multi might already be reset at this time */
      Curl_multi_remove_handle(data->multi, probe_data);
      Curl_close(&probe_data);
    }
    CURL_TRC_DNS(data, "[DoH] probe done");
  }
}

فراغ Curl_doh_cleanup(بنية Curl_easy *data,
                      بنية Curl_resolv_async *async)
{
  بنية doh_probes *dohp = async->doh;
  ܐܢ(dohp) {
    doh_close(data, async);
    curlx_safefree(async->doh);
  }
}

#endif /* CURL_DISABLE_DOH */
