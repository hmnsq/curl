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
  إذا((size_t)code < CURL_ARRAYSIZE(doh_code_str))
    أرجع doh_code_str[code];
  أرجع "bad error code";
}

ساكن ثابت حرف *doh_type2name(DNStype dnstype)
{
  بدل(dnstype) {
  حالة CURL_DNS_TYPE_A:
    أرجع "A";
  حالة CURL_DNS_TYPE_AAAA:
    أرجع "AAAA";
#ifdef USE_HTTPSRR
  حالة CURL_DNS_TYPE_HTTPS:
    أرجع "HTTPS";
#endif
  افتراضي:
    أرجع "unknown";
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
  إذا(host[hostlen - 1] != '.')
    expected_len++;

  إذا(expected_len > DOH_MAX_DNSREQ_SIZE)
    أرجع DOH_DNS_NAME_TOO_LONG;

  إذا(len < expected_len)
    أرجع DOH_TOO_SMALL_BUFFER;

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
  طالما(*hostp) {
    size_t labellen;
    ثابت حرف *dot = strchr(hostp, '.');
    إذا(dot)
      labellen = dot - hostp;
    وإلا
      labellen = strlen(hostp);
    إذا((labellen > 63) || (!labellen)) {
      /* label is too long or too short, error out */
      *olen = 0;
      أرجع DOH_DNS_BAD_LABEL;
    }
    /* label is non-empty, process it */
    *dnsp++ = (unsigned حرف)labellen;
    memcpy(dnsp, hostp, labellen);
    dnsp += labellen;
    hostp += labellen;
    /* advance past dot, but only if there is one */
    إذا(dot)
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
  أرجع DOH_OK;
}

ساكن size_t doh_probe_write_cb(حرف *contents, size_t size, size_t nmemb,
                                 فراغ *userp)
{
  size_t realsize = size * nmemb;
  بنية Curl_easy *data = userp;
  بنية doh_request *doh_req = Curl_meta_get(data, CURL_EZM_DOH_PROBE);
  إذا(!doh_req)
    أرجع CURL_WRITEFUNC_ERROR;

  إذا(curlx_dyn_addn(&doh_req->resp_body, contents, realsize))
    أرجع 0;

  أرجع realsize;
}

ساكن فراغ doh_probe_done(بنية Curl_easy *doh,
                           بنية Curl_easy *master, CURLcode result);
ساكن فراغ doh_probe_dtor(ثابت فراغ *مبودي, size_t klen, فراغ *e)
{
  (فراغ)مبودي;
  (فراغ)klen;
  إذا(e) {
    بنية doh_request *doh_req = e;
    curl_slist_free_all(doh_req->req_hds);
    curlx_dyn_free(&doh_req->resp_body);
    curlx_free(e);
  }
}

#define ERROR_CHECK_SETOPT(x, y)                        \
  افعل {                                                  \
    result = curl_easy_setopt((CURL *)doh, x, y);       \
    إذا(result &&                                        \
       result != CURLE_NOT_BUILT_IN &&                  \
       result != CURLE_UNKNOWN_OPTION)                  \
      اذهب error;                                       \
  } طالما(0)

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
  إذا(!doh_req)
    أرجع CURLE_OUT_OF_MEMORY;
  doh_req->resolv_id = resolv_id;
  doh_req->dnstype = dnstype;
  curlx_dyn_init(&doh_req->resp_body, DYN_DOH_RESPONSE);

  d = doh_req_encode(host, dnstype, doh_req->req_body,
                     حجم(doh_req->req_body),
                     &doh_req->req_body_len);
  إذا(d) {
    failf(data, "Failed to encode DoH packet [%d]", (صحيح)d);
    result = CURLE_OUT_OF_MEMORY;
    اذهب error;
  }

  timeout_ms = Curl_timeleft_ms(data);
  إذا(timeout_ms < 0) {
    result = CURLE_OPERATION_TIMEDOUT;
    اذهب error;
  }

  doh_req->req_hds =
    curl_slist_append(عدم, "Content-Type: application/dns-message");
  إذا(!doh_req->req_hds) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب error;
  }

  /* Curl_open() is the internal version of curl_easy_init() */
  result = Curl_open(&doh);
  إذا(result)
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
  إذا(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);
    ERROR_CHECK_SETOPT(CURLOPT_PIPEWAIT, 1L);
  }
#endif
#ifndef DEBUGBUILD
  /* enforce HTTPS if not debug */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
#وإلا
  /* in debug mode, also allow http */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
#endif
  ERROR_CHECK_SETOPT(CURLOPT_TIMEOUT_MS, (long)timeout_ms);
  ERROR_CHECK_SETOPT(CURLOPT_SHARE, (CURLSH *)data->share);
  إذا(data->set.err && data->set.err != stderr)
    ERROR_CHECK_SETOPT(CURLOPT_STDERR, data->set.err);
  إذا(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_doh))
    ERROR_CHECK_SETOPT(CURLOPT_VERBOSE, 1L);
  إذا(data->set.no_signal)
    ERROR_CHECK_SETOPT(CURLOPT_NOSIGNAL, 1L);
  إذا(data->set.fdebug)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGFUNCTION, data->set.fdebug);
  إذا(data->set.debugdata)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGDATA, data->set.debugdata);

#ifdef USE_RESOLV_ARES
  إذا(CURL_EASY_STR(data, STRING_DNS_SERVERS)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_SERVERS,
                       CURL_EASY_STR(data, STRING_DNS_SERVERS));
  }
  إذا(CURL_EASY_STR(data, STRING_DNS_INTERFACE)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_INTERFACE,
                       CURL_EASY_STR(data, STRING_DNS_INTERFACE));
  }
  إذا(CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP4,
                       CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4));
  }
  إذا(CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP6,
                       CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6));
  }
#endif
  إذا(maybe_https) {
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
    إذا(CURL_EASY_STR(data, STRING_SSL_CAFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO,
                         CURL_EASY_STR(data, STRING_SSL_CAFILE));
    }
    إذا(data->set.blobs[BLOB_CAINFO]) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO_BLOB, data->set.blobs[BLOB_CAINFO]);
    }
    إذا(CURL_EASY_STR(data, STRING_SSL_CAPATH)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAPATH,
                         CURL_EASY_STR(data, STRING_SSL_CAPATH));
    }
    إذا(CURL_EASY_STR(data, STRING_SSL_CRLFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CRLFILE,
                         CURL_EASY_STR(data, STRING_SSL_CRLFILE));
    }
    إذا(data->set.ssl.certinfo)
      ERROR_CHECK_SETOPT(CURLOPT_CERTINFO, 1L);
    إذا(data->set.ssl_fsslctx)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_FUNCTION, data->set.ssl_fsslctx);
    إذا(data->set.ssl_fsslctxp)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_DATA, data->set.ssl_fsslctxp);
    إذا(CURL_EASY_STR(data, STRING_SSL_EC_CURVES)) {
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
  إذا(result)
    اذهب error;

  /* DoH handles must not inherit private_data. The handles may be passed to
     the user via callbacks and the user will be able to identify them as
     internal handles because private data is not set. The user can then set
     private_data via CURLOPT_PRIVATE if they so choose. */
  DEBUGASSERT(!doh->set.private_data);

  إذا(Curl_multi_add_handle(multi, doh))
    اذهب error;

  *pmid = doh->mid;
  أرجع CURLE_OK;

error:
  Curl_close(&doh);
  إذا(doh_req)
    doh_probe_dtor(عدم, 0, doh_req);
  أرجع result;
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
  إذا(async->doh) {
    DEBUGASSERT(0); /* should not happen */
    Curl_doh_cleanup(data, async);
  }

  إذا(!async->dns_queries)
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef USE_HTTPSRR
  إذا(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     (async->dns_queries & CURL_DNSQ_HTTPS)) {
    /* Can't mix those in the same async resolve */
    DEBUGASSERT(0);
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  }
#وإلا
  إذا(async->dns_queries & CURL_DNSQ_HTTPS) {
    DEBUGASSERT(0);
    أرجع CURLE_NOT_BUILT_IN;
  }
#endif

  /* start clean, consider allocating this struct on demand */
  async->doh = dohp = curlx_calloc(1, حجم(بنية doh_probes));
  إذا(!dohp)
    أرجع CURLE_OUT_OF_MEMORY;

  لكل(i = 0; i < DOH_SLOT_كدي; ++i) {
    dohp->probe_rc[i] = DOH_OK;
    dohp->probe_mid[i] = UINT32_MAX;
  }

#ifdef USE_IPV6
  /* AAAA results have preference in happy eyeballing, trigger first */
  إذا(async->dns_queries & CURL_DNSQ_AAAA) {
    /* create IPv6 DoH request */
    result = doh_probe_run(data, CURL_DNS_TYPE_AAAA,
                           async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH),
                           data->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV6]);
    إذا(result)
      اذهب error;
    async->queries_ongoing++;
  }
#endif

  /* create IPv4 DoH request */
  إذا(async->dns_queries & CURL_DNSQ_A) {
    result = doh_probe_run(data, CURL_DNS_TYPE_A,
                           async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH),
                           data->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV4]);
    إذا(result)
      اذهب error;
    async->queries_ongoing++;
  }

#ifdef USE_HTTPSRR
  إذا(async->dns_queries & CURL_DNSQ_HTTPS) {
    حرف *qname = عدم;
    إذا(async->peer->port != PORT_HTTPS) {
      qname = curl_maprintf("_%u._https.%s",
                            async->peer->port, async->peer->hostname);
      إذا(!qname)
        اذهب error;
    }
    result = doh_probe_run(data, CURL_DNS_TYPE_HTTPS,
                           qname ? qname : async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH), data->multi,
                           async->id,
                           &dohp->probe_mid[DOH_SLOT_HTTPS_RR]);
    curlx_free(qname);
    إذا(result)
      اذهب error;
    async->queries_ongoing++;
  }
#endif
  أرجع CURLE_OK;

error:
  Curl_doh_cleanup(data, async);
  أرجع result;
}

ساكن DOHcode doh_skipqname(ثابت unsigned حرف *doh, size_t dohlen,
                             unsigned صحيح *indexp)
{
  unsigned حرف length;
  افعل {
    إذا(dohlen < (*indexp + 1))
      أرجع DOH_DNS_OUT_OF_RANGE;
    length = doh[*indexp];
    إذا((length & 0xc0) == 0xc0) {
      /* name pointer, advance over it and be done */
      إذا(dohlen < (*indexp + 2))
        أرجع DOH_DNS_OUT_OF_RANGE;
      *indexp += 2;
      اكسر;
    }
    إذا(length & 0xc0)
      أرجع DOH_DNS_BAD_LABEL;
    إذا(dohlen < (*indexp + 1 + length))
      أرجع DOH_DNS_OUT_OF_RANGE;
    *indexp += (unsigned صحيح)(1 + length);
  } طالما(length);
  أرجع DOH_OK;
}

ساكن unsigned short doh_get16bit(ثابت unsigned حرف *doh,
                                   unsigned صحيح index)
{
  أرجع (unsigned short)((doh[index] << 8) | doh[index + 1]);
}

ساكن unsigned صحيح doh_get32bit(ثابت unsigned حرف *doh, unsigned صحيح index)
{
  /* make clang and gcc optimize this to bswap by incrementing
     the pointer first. */
  doh += index;

  /* avoid undefined behavior by casting to unsigned before shifting
     24 bits, possibly into the sign bit. codegen is same, but
     ub sanitizer will not be upset */
  أرجع ((unsigned)doh[0] << 24) | ((unsigned)doh[1] << 16) |
         ((unsigned)doh[2] << 8) | doh[3];
}

ساكن فراغ doh_store_a(ثابت unsigned حرف *doh, صحيح index,
                        بنية dohentry *d)
{
  /* silently ignore addresses over the limit */
  إذا(d->numaddr < DOH_MAX_ADDR) {
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
  إذا(d->numaddr < DOH_MAX_ADDR) {
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
  إذا(d->numhttps_rrs < DOH_MAX_HTTPS) {
    بنية dohhttps_rr *h = &d->https_rrs[d->numhttps_rrs];
    h->val = curlx_memdup(&doh[index], len);
    إذا(!h->val)
      أرجع DOH_OUT_OF_MEM;
    h->len = len;
    d->numhttps_rrs++;
  }
  أرجع DOH_OK;
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
    إذا(rdlength != 4)
      أرجع DOH_DNS_RDATA_LEN;
    doh_store_a(doh, index, d);
    اكسر;
  حالة CURL_DNS_TYPE_AAAA:
    إذا(rdlength != 16)
      أرجع DOH_DNS_RDATA_LEN;
    doh_store_aaaa(doh, index, d);
    اكسر;
#ifdef USE_HTTPSRR
  حالة CURL_DNS_TYPE_HTTPS:
    إذا(rdlength < 3)
      أرجع DOH_DNS_RDATA_LEN;
    أرجع doh_store_https(doh, index, d, rdlength);
#endif
  افتراضي:
    /* unsupported type, or type we do not store, skip it */
    اكسر;
  }
  أرجع DOH_OK;
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

  إذا(dohlen < 12)
    أرجع DOH_TOO_SMALL_BUFFER; /* too small */
  إذا(!doh || doh[0] || doh[1])
    أرجع DOH_DNS_BAD_ID; /* bad ID */
  rcode = doh[3] & 0x0f;
  إذا(rcode == 3)
    أرجع DOH_DNS_NXDOMAIN; /* name does not exist */
  إذا(rcode)
    أرجع DOH_DNS_BAD_RCODE; /* bad rcode */

  qdcount = doh_get16bit(doh, 4);
  طالما(qdcount) {
    rc = doh_skipqname(doh, dohlen, &index);
    إذا(rc)
      أرجع rc; /* bad qname */
    إذا(dohlen < (index + 4))
      أرجع DOH_DNS_OUT_OF_RANGE;
    index += 4; /* skip question's type and class */
    qdcount--;
  }

  ancount = doh_get16bit(doh, 6);
  طالما(ancount) {
    unsigned short dnsclass;
    unsigned صحيح ttl;

    rc = doh_skipqname(doh, dohlen, &index);
    إذا(rc)
      أرجع rc; /* bad qname */

    إذا(dohlen < (index + 2))
      أرجع DOH_DNS_OUT_OF_RANGE;

    type = doh_get16bit(doh, index);
    إذا((type != CURL_DNS_TYPE_CNAME) &&  /* may be synthesized from DNAME */
       (type != CURL_DNS_TYPE_DNAME) &&  /* if present, accept and ignore */
       (type != dnstype))
      /* Not the same type as was asked for, nor CNAME nor DNAME */
      أرجع DOH_DNS_UNEXPECTED_TYPE;
    index += 2;

    إذا(dohlen < (index + 2))
      أرجع DOH_DNS_OUT_OF_RANGE;
    dnsclass = doh_get16bit(doh, index);
    إذا(DNS_CLASS_IN != dnsclass)
      أرجع DOH_DNS_UNEXPECTED_CLASS; /* unsupported */
    index += 2;

    إذا(dohlen < (index + 4))
      أرجع DOH_DNS_OUT_OF_RANGE;

    ttl = doh_get32bit(doh, index);
    إذا(ttl > MAX_DNS_TTL)
      ttl = MAX_DNS_TTL;
    إذا(ttl < d->ttl)
      d->ttl = ttl;
    index += 4;

    إذا(dohlen < (index + 2))
      أرجع DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    إذا(dohlen < (index + rdlength))
      أرجع DOH_DNS_OUT_OF_RANGE;

    rc = doh_rdata(doh, rdlength, type, (صحيح)index, d);
    إذا(rc)
      أرجع rc;
    index += rdlength;
    ancount--;
  }

  nscount = doh_get16bit(doh, 8);
  طالما(nscount) {
    rc = doh_skipqname(doh, dohlen, &index);
    إذا(rc)
      أرجع rc; /* bad qname */

    إذا(dohlen < (index + 8))
      أرجع DOH_DNS_OUT_OF_RANGE;

    index += 2 + 2 + 4; /* type, dnsclass and ttl */

    إذا(dohlen < (index + 2))
      أرجع DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    إذا(dohlen < (index + rdlength))
      أرجع DOH_DNS_OUT_OF_RANGE;
    index += rdlength;
    nscount--;
  }

  arcount = doh_get16bit(doh, 10);
  طالما(arcount) {
    rc = doh_skipqname(doh, dohlen, &index);
    إذا(rc)
      أرجع rc; /* bad qname */

    إذا(dohlen < (index + 8))
      أرجع DOH_DNS_OUT_OF_RANGE;

    index += 2 + 2 + 4; /* type, dnsclass and ttl */

    إذا(dohlen < (index + 2))
      أرجع DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    إذا(dohlen < (index + rdlength))
      أرجع DOH_DNS_OUT_OF_RANGE;
    index += rdlength;
    arcount--;
  }

  إذا(index != dohlen)
    أرجع DOH_DNS_MALFORMAT; /* something is wrong */

  أرجع DOH_OK; /* ok */
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
    إذا(de->addr[i].type == CURL_DNS_TYPE_AAAA) {
#ifndef USE_IPV6
      /* we cannot handle IPv6 addresses */
      استمر;
#وإلا
      ss_size = حجم(بنية sockaddr_in6);
      addrtype = AF_INET6;
#endif
    }
    وإلا {
      ss_size = حجم(بنية sockaddr_in);
      addrtype = AF_INET;
    }

    ai = curlx_calloc(1, حجم(بنية Curl_addrinfo) + ss_size + hostlen);
    إذا(!ai) {
      result = CURLE_OUT_OF_MEMORY;
      اكسر;
    }
    ai->ai_addr = (فراغ *)((حرف *)ai + حجم(بنية Curl_addrinfo));
    ai->ai_canonname = (فراغ *)((حرف *)ai->ai_addr + ss_size);
    memcpy(ai->ai_canonname, hostname, hostlen);

    إذا(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    إذا(prevai)
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

  إذا(result) {
    Curl_freeaddrinfo(firstai);
    firstai = عدم;
  }
  *aip = firstai;

  أرجع result;
}

/* @unittest 1655 */
UNITTEST فراغ de_cleanup(بنية dohentry *d);
UNITTEST فراغ de_cleanup(بنية dohentry *d)
{
#ifdef USE_HTTPSRR
  صحيح i = 0;
  لكل(i = 0; i < d->numhttps_rrs; i++)
    curlx_safefree(d->https_rrs[i].val);
#وإلا
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
  إذا(!buf || !remaining || !dnsname || !*remaining)
    أرجع CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&thename, CURL_MAXLEN_HOST_NAME);
  rem = *remaining;
  cp = *buf;
  clen = *cp++;
  /* RFC 9460 says it must be uncompressed */
  إذا(clen > 63)
    أرجع CURLE_WEIRD_SERVER_REPLY;

  إذا(clen == 0) {
    /* special case - return "." as name */
    إذا(curlx_dyn_addn(&thename, ".", 1))
      أرجع CURLE_OUT_OF_MEMORY;
  }
  طالما(clen) {
    إذا(clen >= rem) {
      curlx_dyn_free(&thename);
      أرجع CURLE_OUT_OF_MEMORY;
    }
    إذا(curlx_dyn_addn(&thename, cp, clen) ||
       curlx_dyn_addn(&thename, ".", 1))
      أرجع CURLE_TOO_LARGE;

    cp += clen;
    rem -= (clen + 1);
    إذا(rem <= 0) {
      curlx_dyn_free(&thename);
      أرجع CURLE_OUT_OF_MEMORY;
    }
    clen = *cp++;
    إذا(clen > 63) {
      /* invalid format */
      curlx_dyn_free(&thename);
      أرجع CURLE_WEIRD_SERVER_REPLY;
    }
  }
  *buf = cp;
  *remaining = rem - 1;
  *dnsname = curlx_dyn_ptr(&thename);
  أرجع CURLE_OK;
}

/* scan for byte values <= 32 or 127 */
ساكن CURLcode junkscan(ثابت حرف *url)
{
  ثابت unsigned حرف *p = (ثابت unsigned حرف *)url;
  طالما(*p) {
    إذا(*p <= 0x20 || *p == 127)
      أرجع CURLE_WEIRD_SERVER_REPLY;
    p++;
  }
  أرجع CURLE_OK;
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
  إذا(len <= 2)
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  lhrr = curlx_calloc(1, حجم(بنية Curl_https_rrinfo));
  إذا(!lhrr)
    أرجع CURLE_OUT_OF_MEMORY;
  lhrr->priority = doh_get16bit(cp, 0);
  cp += 2;
  len -= 2;
  إذا(doh_decode_rdata_name(&cp, &len, &dnsname) != CURLE_OK)
    اذهب err;
  lhrr->target = dnsname;
  result = junkscan(dnsname);
  إذا(result) {
    /* unacceptable hostname content */
    اذهب err;
  }
  طالما(len >= 4) {
    pcode = doh_get16bit(cp, 0);
    plen = doh_get16bit(cp, 2);
    cp += 4;
    len -= 4;
    إذا(pcode < expected_min_pcode || plen > len) {
      result = CURLE_WEIRD_SERVER_REPLY;
      اذهب err;
    }
    result = Curl_httpsrr_set(lhrr, pcode, cp, plen);
    إذا(result)
      اذهب err;
    Curl_httpsrr_trace(data, lhrr);
    cp += plen;
    len -= plen;
    expected_min_pcode = pcode + 1;
  }
  *hrr = lhrr;
  أرجع CURLE_OK;
err:
  Curl_httpsrr_destroy(lhrr);
  أرجع result;
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
  إذا(!doh_req) {
    /* transfer `doh` is not a DoH probe. */
    DEBUGASSERT(0);
    اذهب out;
  }

  async = Curl_async_get(master, doh_req->resolv_id);
  إذا(!async) {
    CURL_TRC_DNS(master, "[%u] ignoring outdated DoH response",
                 doh_req->resolv_id);
    اذهب out;
  }
  dohp = async->doh;

  لكل(slot = 0; slot < DOH_SLOT_كدي; ++slot) {
    إذا(dohp->probe_mid[slot] == doh->mid)
      اكسر;
  }
  /* We really should have found the slot where to store the response */
  إذا(slot >= DOH_SLOT_كدي) {
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

  إذا(result) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: %s",
          doh_type2name(doh_req->dnstype), curl_easy_strerror(result));
    اذهب out;
  }
  وإلا إذا((httpcode < 200) || (httpcode >= 300)) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: HTTP status %d",
          doh_type2name(doh_req->dnstype), httpcode);
    اذهب out;
  }

  dohp->probe_rc[slot] = doh_resp_decode(curlx_dyn_uptr(&doh_req->resp_body),
                                         curlx_dyn_len(&doh_req->resp_body),
                                         doh_req->dnstype, &de);
  إذا(dohp->probe_rc[slot]) {
#ifdef USE_HTTPSRR
    إذا((dohp->probe_rc[slot] == DOH_NO_CONTENT) &&
       (doh_req->dnstype == CURL_DNS_TYPE_HTTPS)) {
      dohp->probe_rc[slot] = DOH_DNS_NXDOMAIN;
    }
#endif
    infof(doh, "[DoH] [%s] error decoding response: %s",
          doh_type2name(doh_req->dnstype),
          doh_strerror(dohp->probe_rc[slot]));
    اذهب out;
  }

  إذا(doh_req->dnstype == CURL_DNS_TYPE_A)
    pdest_ai = &async->ai_A;
  وإلا إذا(doh_req->dnstype == CURL_DNS_TYPE_AAAA)
    pdest_ai = &async->ai_AAAA;
  وإلا
    pdest_ai = عدم;

  إذا(pdest_ai && de.numaddr) {
    إذا(*pdest_ai) {
      Curl_freeaddrinfo(*pdest_ai);
      *pdest_ai = عدم;
    }
    result = doh2ai(&de, async->peer->hostname, async->peer->port, pdest_ai);
    إذا(result) { /* hard failure on our side, fail completely */
      infof(doh, "[DoH] [%s] error creating addrinfo: %s",
            doh_type2name(doh_req->dnstype), curl_easy_strerror(result));
      dohp->probe_rc[slot] = DOH_OOM;
      async->result = result;
    }
  }
#ifdef USE_HTTPSRR
  وإلا إذا((doh_req->dnstype == CURL_DNS_TYPE_HTTPS) && de.numhttps_rrs) {
    CURL_TRC_DNS(doh, "[HTTPS] got %d records", de.numhttps_rrs);
    result = doh_resp_decode_httpsrr(doh, de.https_rrs->val,
                                     de.https_rrs->len, &async->httpsrr);
    إذا(result) {
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
  إذا(!dohp)
    أرجع CURLE_OUT_OF_MEMORY;

  async->negative_answer = FALSE;
  إذا(async->result) {
    result = async->result;
    اذهب out;
  }

  إذا(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     dohp->probe_mid[DOH_SLOT_IPV4] == UINT32_MAX &&
     dohp->probe_mid[DOH_SLOT_IPV6] == UINT32_MAX) {
    failf(data, "Could not DoH-resolve: %s", async->peer->hostname);
    أرجع async->for_proxy ?
      CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
  }
  وإلا إذا(!async->queries_ongoing) {
    بنية Curl_dns_entry *dns = عدم;
    bool negative = TRUE;
    صحيح slot;

    /* remove DoH handles from multi handle and close them */
    doh_close(data, async);
    /* parse the responses, create the struct and return it! */
    لكل(slot = 0; slot < DOH_SLOT_كدي; slot++) {
      /* Failing without an NXDOMAIN answer - a SERVFAIL-class rcode or
         an undecodable response - says nothing about the name. Such a
         failure must not be cached as a negative entry. */
      إذا(dohp->probe_rc[slot] && (dohp->probe_rc[slot] != DOH_DNS_NXDOMAIN))
        negative = FALSE;
    } /* next slot */

    إذا(async->ai_A || async->ai_AAAA) {
      dns = Curl_dnsc_mk_addr2(
        data, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
      إذا(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        اذهب out;
      }
    }
#ifdef USE_HTTPSRR
    وإلا إذا((async->dns_queries & CURL_DNSQ_HTTPS) &&
            !dohp->probe_rc[DOH_SLOT_HTTPS_RR]) {
      Curl_httpsrr_trace(data, async->httpsrr);
      dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
      إذا(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        اذهب out;
      }
    }
#endif /* USE_HTTPSRR */
    وإلا {
      /* every query failed. Only NXDOMAIN answers for all of them
         make this a negative answer, eligible for caching. */
      async->negative_answer = negative;
      result = async->for_proxy ?
        CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
    }

    /* and add the entry to the cache */
    إذا(dns)
      result = Curl_dnscache_add(data, dns);
    *pdns = dns;
  } /* !async->queries_ongoing */
  وإلا
    /* wait for pending DoH transactions to complete */
    أرجع CURLE_AGAIN;

out:
  Curl_doh_cleanup(data, async);
  أرجع result;
}

ساكن فراغ doh_close(بنية Curl_easy *data,
                      بنية Curl_resolv_async *async)
{
  بنية doh_probes *doh = async ? async->doh : عدم;
  إذا(doh && data->multi) {
    بنية Curl_easy *probe_data;
    uint32_t mid;
    size_t slot;
    لكل(slot = 0; slot < DOH_SLOT_كدي; slot++) {
      mid = doh->probe_mid[slot];
      إذا(mid == UINT32_MAX)
        استمر;
      doh->probe_mid[slot] = UINT32_MAX;
      /* should have been called before data is removed from multi handle */
      DEBUGASSERT(data->multi);
      probe_data = data->multi ? Curl_multi_get_easy(data->multi, mid) : عدم;
      إذا(!probe_data) {
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
  إذا(dohp) {
    doh_close(data, async);
    curlx_safefree(async->doh);
  }
}

#endif /* CURL_DISABLE_DOH */
