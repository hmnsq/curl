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
#அந்தர்பாவ "curl_setup.h"

#ifndef CURL_DISABLE_DOH

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "curl_addrinfo.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "vdns/doh.h"
#அந்தர்பாவ "vdns/httpsrr.h"
#அந்தர்பாவ "curlx/strdup.h"
#அந்தர்பாவ "curlx/dynbuf.h"
#அந்தர்பாவ "escape.h"  /* for Curl_hexencode() */
#அந்தர்பாவ "urlapi-int.h"

#define DNS_CLASS_IN 0x01

நிலையான வெறுமை doh_close(கட்டமைப்பு Curl_easy *data,
                      கட்டமைப்பு Curl_resolv_async *async);

#ifdef CURLVERBOSE
நிலையான மாறிலி எழுத்து * மாறிலி doh_code_str[] = {
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

நிலையான மாறிலி எழுத்து *doh_strerror(DOHcode code)
{
  எனில்((size_t)code < CURL_ARRAYSIZE(doh_code_str))
    திரும்பு doh_code_str[code];
  திரும்பு "bad error code";
}

நிலையான மாறிலி எழுத்து *doh_type2name(DNStype dnstype)
{
  தேர்வு(dnstype) {
  நிலை CURL_DNS_TYPE_A:
    திரும்பு "A";
  நிலை CURL_DNS_TYPE_AAAA:
    திரும்பு "AAAA";
#ifdef USE_HTTPSRR
  நிலை CURL_DNS_TYPE_HTTPS:
    திரும்பு "HTTPS";
#endif
  இயல்பு:
    திரும்பு "unknown";
  }
}

#endif /* CURLVERBOSE */

/* @unittest 1655
 */
UNITTEST DOHcode doh_req_encode(மாறிலி எழுத்து *host,
                                DNStype dnstype,
                                unsigned எழுத்து *dnsp,  /* buffer */
                                size_t len,  /* buffer size */
                                size_t *olen);  /* output length */
UNITTEST DOHcode doh_req_encode(மாறிலி எழுத்து *host,
                                DNStype dnstype,
                                unsigned எழுத்து *dnsp, /* buffer */
                                size_t len,   /* buffer size */
                                size_t *olen) /* output length */
{
  மாறிலி size_t hostlen = strlen(host);
  unsigned எழுத்து *orig = dnsp;
  மாறிலி எழுத்து *hostp = host;

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
  எனில்(host[hostlen - 1] != '.')
    expected_len++;

  எனில்(expected_len > DOH_MAX_DNSREQ_SIZE)
    திரும்பு DOH_DNS_NAME_TOO_LONG;

  எனில்(len < expected_len)
    திரும்பு DOH_TOO_SMALL_BUFFER;

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
  போது(*hostp) {
    size_t labellen;
    மாறிலி எழுத்து *dot = strchr(hostp, '.');
    எனில்(dot)
      labellen = dot - hostp;
    இல்லையெனில்
      labellen = strlen(hostp);
    எனில்((labellen > 63) || (!labellen)) {
      /* label is too long or too short, error out */
      *olen = 0;
      திரும்பு DOH_DNS_BAD_LABEL;
    }
    /* label is non-empty, process it */
    *dnsp++ = (unsigned எழுத்து)labellen;
    memcpy(dnsp, hostp, labellen);
    dnsp += labellen;
    hostp += labellen;
    /* advance past dot, but only if there is one */
    எனில்(dot)
      hostp++;
  } /* next label */

  *dnsp++ = 0; /* append zero-length label for root */

  /* There are assigned TYPE codes beyond 255: use range [1..65535] */
  *dnsp++ = (unsigned எழுத்து)(255 & (dnstype >> 8)); /* upper 8-bit TYPE */
  *dnsp++ = (unsigned எழுத்து)(255 & dnstype);        /* lower 8-bit TYPE */

  *dnsp++ = '\0'; /* upper 8-bit CLASS */
  *dnsp++ = DNS_CLASS_IN; /* IN - "the Internet" */

  *olen = dnsp - orig;

  /* verify that our estimation of length is valid, since
   * this has led to buffer overflows in this function */
  DEBUGASSERT(*olen == expected_len);
  திரும்பு DOH_OK;
}

நிலையான size_t doh_probe_write_cb(எழுத்து *contents, size_t size, size_t nmemb,
                                 வெறுமை *userp)
{
  size_t realsize = size * nmemb;
  கட்டமைப்பு Curl_easy *data = userp;
  கட்டமைப்பு doh_request *doh_req = Curl_meta_get(data, CURL_EZM_DOH_PROBE);
  எனில்(!doh_req)
    திரும்பு CURL_WRITEFUNC_ERROR;

  எனில்(curlx_dyn_addn(&doh_req->resp_body, contents, realsize))
    திரும்பு 0;

  திரும்பு realsize;
}

நிலையான வெறுமை doh_probe_done(கட்டமைப்பு Curl_easy *doh,
                           கட்டமைப்பு Curl_easy *master, CURLcode result);
நிலையான வெறுமை doh_probe_dtor(மாறிலி வெறுமை *சாவி, size_t klen, வெறுமை *e)
{
  (வெறுமை)சாவி;
  (வெறுமை)klen;
  எனில்(e) {
    கட்டமைப்பு doh_request *doh_req = e;
    curl_slist_free_all(doh_req->req_hds);
    curlx_dyn_free(&doh_req->resp_body);
    curlx_free(e);
  }
}

#define ERROR_CHECK_SETOPT(x, y)                        \
  செய் {                                                  \
    result = curl_easy_setopt((CURL *)doh, x, y);       \
    எனில்(result &&                                        \
       result != CURLE_NOT_BUILT_IN &&                  \
       result != CURLE_UNKNOWN_OPTION)                  \
      செல் error;                                       \
  } போது(0)

நிலையான CURLcode doh_probe_run(கட்டமைப்பு Curl_easy *data,
                              DNStype dnstype,
                              மாறிலி எழுத்து *host,
                              மாறிலி எழுத்து *url, CURLM *multi,
                              uint32_t resolv_id,
                              uint32_t *pmid)
{
  கட்டமைப்பு Curl_easy *doh = வெற்று;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  கட்டமைப்பு doh_request *doh_req;
  DOHcode d;
  bool maybe_https = !curl_strnequal(url, STRCONST("http:"));

  *pmid = UINT32_MAX;

  doh_req = curlx_calloc(1, அளவு(*doh_req));
  எனில்(!doh_req)
    திரும்பு CURLE_OUT_OF_MEMORY;
  doh_req->resolv_id = resolv_id;
  doh_req->dnstype = dnstype;
  curlx_dyn_init(&doh_req->resp_body, DYN_DOH_RESPONSE);

  d = doh_req_encode(host, dnstype, doh_req->req_body,
                     அளவு(doh_req->req_body),
                     &doh_req->req_body_len);
  எனில்(d) {
    failf(data, "Failed to encode DoH packet [%d]", (முழு)d);
    result = CURLE_OUT_OF_MEMORY;
    செல் error;
  }

  timeout_ms = Curl_timeleft_ms(data);
  எனில்(timeout_ms < 0) {
    result = CURLE_OPERATION_TIMEDOUT;
    செல் error;
  }

  doh_req->req_hds =
    curl_slist_append(வெற்று, "Content-Type: application/dns-message");
  எனில்(!doh_req->req_hds) {
    result = CURLE_OUT_OF_MEMORY;
    செல் error;
  }

  /* Curl_open() is the internal version of curl_easy_init() */
  result = Curl_open(&doh);
  எனில்(result)
    செல் error;

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
  எனில்(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);
    ERROR_CHECK_SETOPT(CURLOPT_PIPEWAIT, 1L);
  }
#endif
#ifndef DEBUGBUILD
  /* enforce HTTPS if not debug */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
#இல்லையெனில்
  /* in debug mode, also allow http */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
#endif
  ERROR_CHECK_SETOPT(CURLOPT_TIMEOUT_MS, (long)timeout_ms);
  ERROR_CHECK_SETOPT(CURLOPT_SHARE, (CURLSH *)data->share);
  எனில்(data->set.err && data->set.err != stderr)
    ERROR_CHECK_SETOPT(CURLOPT_STDERR, data->set.err);
  எனில்(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_doh))
    ERROR_CHECK_SETOPT(CURLOPT_VERBOSE, 1L);
  எனில்(data->set.no_signal)
    ERROR_CHECK_SETOPT(CURLOPT_NOSIGNAL, 1L);
  எனில்(data->set.fdebug)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGFUNCTION, data->set.fdebug);
  எனில்(data->set.debugdata)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGDATA, data->set.debugdata);

#ifdef USE_RESOLV_ARES
  எனில்(CURL_EASY_STR(data, STRING_DNS_SERVERS)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_SERVERS,
                       CURL_EASY_STR(data, STRING_DNS_SERVERS));
  }
  எனில்(CURL_EASY_STR(data, STRING_DNS_INTERFACE)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_INTERFACE,
                       CURL_EASY_STR(data, STRING_DNS_INTERFACE));
  }
  எனில்(CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP4,
                       CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4));
  }
  எனில்(CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP6,
                       CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6));
  }
#endif
  எனில்(maybe_https) {
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
    எனில்(CURL_EASY_STR(data, STRING_SSL_CAFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO,
                         CURL_EASY_STR(data, STRING_SSL_CAFILE));
    }
    எனில்(data->set.blobs[BLOB_CAINFO]) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO_BLOB, data->set.blobs[BLOB_CAINFO]);
    }
    எனில்(CURL_EASY_STR(data, STRING_SSL_CAPATH)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAPATH,
                         CURL_EASY_STR(data, STRING_SSL_CAPATH));
    }
    எனில்(CURL_EASY_STR(data, STRING_SSL_CRLFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CRLFILE,
                         CURL_EASY_STR(data, STRING_SSL_CRLFILE));
    }
    எனில்(data->set.ssl.certinfo)
      ERROR_CHECK_SETOPT(CURLOPT_CERTINFO, 1L);
    எனில்(data->set.ssl_fsslctx)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_FUNCTION, data->set.ssl_fsslctx);
    எனில்(data->set.ssl_fsslctxp)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_DATA, data->set.ssl_fsslctxp);
    எனில்(CURL_EASY_STR(data, STRING_SSL_EC_CURVES)) {
      ERROR_CHECK_SETOPT(CURLOPT_SSL_EC_CURVES,
                         CURL_EASY_STR(data, STRING_SSL_EC_CURVES));
    }
    (வெறுமை)curl_easy_setopt(doh, CURLOPT_SSL_OPTIONS,
                           ((long)data->set.ssl.ssl_options &
                            ~CURLSSLOPT_AUTO_CLIENT_CERT));
  }

  doh->state.internal = TRUE;
  doh->master_mid = data->mid; /* master transfer of this one */
  doh->sub_xfer_done = doh_probe_done;

  result = Curl_meta_set(doh, CURL_EZM_DOH_PROBE, doh_req, doh_probe_dtor);
  doh_req = வெற்று; /* call took ownership */
  எனில்(result)
    செல் error;

  /* DoH handles must not inherit private_data. The handles may be passed to
     the user via callbacks and the user will be able to identify them as
     internal handles because private data is not set. The user can then set
     private_data via CURLOPT_PRIVATE if they so choose. */
  DEBUGASSERT(!doh->set.private_data);

  எனில்(Curl_multi_add_handle(multi, doh))
    செல் error;

  *pmid = doh->mid;
  திரும்பு CURLE_OK;

error:
  Curl_close(&doh);
  எனில்(doh_req)
    doh_probe_dtor(வெற்று, 0, doh_req);
  திரும்பு result;
}

/*
 * Curl_doh() starts a name resolve using DoH. It resolves a name and returns
 * a 'Curl_addrinfo *' with the address information.
 */

CURLcode Curl_doh(கட்டமைப்பு Curl_easy *data,
                  கட்டமைப்பு Curl_resolv_async *async)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு doh_probes *dohp = வெற்று;
  size_t i;

  DEBUGASSERT(!async->doh);
  DEBUGASSERT(async->peer->hostname[0]);
  எனில்(async->doh) {
    DEBUGASSERT(0); /* should not happen */
    Curl_doh_cleanup(data, async);
  }

  எனில்(!async->dns_queries)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef USE_HTTPSRR
  எனில்(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     (async->dns_queries & CURL_DNSQ_HTTPS)) {
    /* Can't mix those in the same async resolve */
    DEBUGASSERT(0);
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  }
#இல்லையெனில்
  எனில்(async->dns_queries & CURL_DNSQ_HTTPS) {
    DEBUGASSERT(0);
    திரும்பு CURLE_NOT_BUILT_IN;
  }
#endif

  /* start clean, consider allocating this struct on demand */
  async->doh = dohp = curlx_calloc(1, அளவு(கட்டமைப்பு doh_probes));
  எனில்(!dohp)
    திரும்பு CURLE_OUT_OF_MEMORY;

  க்கு(i = 0; i < DOH_SLOT_மொத்தம்; ++i) {
    dohp->probe_rc[i] = DOH_OK;
    dohp->probe_mid[i] = UINT32_MAX;
  }

#ifdef USE_IPV6
  /* AAAA results have preference in happy eyeballing, trigger first */
  எனில்(async->dns_queries & CURL_DNSQ_AAAA) {
    /* create IPv6 DoH request */
    result = doh_probe_run(data, CURL_DNS_TYPE_AAAA,
                           async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH),
                           data->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV6]);
    எனில்(result)
      செல் error;
    async->queries_ongoing++;
  }
#endif

  /* create IPv4 DoH request */
  எனில்(async->dns_queries & CURL_DNSQ_A) {
    result = doh_probe_run(data, CURL_DNS_TYPE_A,
                           async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH),
                           data->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV4]);
    எனில்(result)
      செல் error;
    async->queries_ongoing++;
  }

#ifdef USE_HTTPSRR
  எனில்(async->dns_queries & CURL_DNSQ_HTTPS) {
    எழுத்து *qname = வெற்று;
    எனில்(async->peer->port != PORT_HTTPS) {
      qname = curl_maprintf("_%u._https.%s",
                            async->peer->port, async->peer->hostname);
      எனில்(!qname)
        செல் error;
    }
    result = doh_probe_run(data, CURL_DNS_TYPE_HTTPS,
                           qname ? qname : async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH), data->multi,
                           async->id,
                           &dohp->probe_mid[DOH_SLOT_HTTPS_RR]);
    curlx_free(qname);
    எனில்(result)
      செல் error;
    async->queries_ongoing++;
  }
#endif
  திரும்பு CURLE_OK;

error:
  Curl_doh_cleanup(data, async);
  திரும்பு result;
}

நிலையான DOHcode doh_skipqname(மாறிலி unsigned எழுத்து *doh, size_t dohlen,
                             unsigned முழு *indexp)
{
  unsigned எழுத்து length;
  செய் {
    எனில்(dohlen < (*indexp + 1))
      திரும்பு DOH_DNS_OUT_OF_RANGE;
    length = doh[*indexp];
    எனில்((length & 0xc0) == 0xc0) {
      /* name pointer, advance over it and be done */
      எனில்(dohlen < (*indexp + 2))
        திரும்பு DOH_DNS_OUT_OF_RANGE;
      *indexp += 2;
      நிறுத்து;
    }
    எனில்(length & 0xc0)
      திரும்பு DOH_DNS_BAD_LABEL;
    எனில்(dohlen < (*indexp + 1 + length))
      திரும்பு DOH_DNS_OUT_OF_RANGE;
    *indexp += (unsigned முழு)(1 + length);
  } போது(length);
  திரும்பு DOH_OK;
}

நிலையான unsigned short doh_get16bit(மாறிலி unsigned எழுத்து *doh,
                                   unsigned முழு index)
{
  திரும்பு (unsigned short)((doh[index] << 8) | doh[index + 1]);
}

நிலையான unsigned முழு doh_get32bit(மாறிலி unsigned எழுத்து *doh, unsigned முழு index)
{
  /* make clang and gcc optimize this to bswap by incrementing
     the pointer first. */
  doh += index;

  /* avoid undefined behavior by casting to unsigned before shifting
     24 bits, possibly into the sign bit. codegen is same, but
     ub sanitizer will not be upset */
  திரும்பு ((unsigned)doh[0] << 24) | ((unsigned)doh[1] << 16) |
         ((unsigned)doh[2] << 8) | doh[3];
}

நிலையான வெறுமை doh_store_a(மாறிலி unsigned எழுத்து *doh, முழு index,
                        கட்டமைப்பு dohentry *d)
{
  /* silently ignore addresses over the limit */
  எனில்(d->numaddr < DOH_MAX_ADDR) {
    கட்டமைப்பு dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_A;
    memcpy(&a->ip.v4, &doh[index], 4);
    d->numaddr++;
  }
}

நிலையான வெறுமை doh_store_aaaa(மாறிலி unsigned எழுத்து *doh, முழு index,
                           கட்டமைப்பு dohentry *d)
{
  /* silently ignore addresses over the limit */
  எனில்(d->numaddr < DOH_MAX_ADDR) {
    கட்டமைப்பு dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_AAAA;
    memcpy(&a->ip.v6, &doh[index], 16);
    d->numaddr++;
  }
}

#ifdef USE_HTTPSRR
நிலையான DOHcode doh_store_https(மாறிலி unsigned எழுத்து *doh, முழு index,
                               கட்டமைப்பு dohentry *d, uint16_t len)
{
  /* silently ignore RRs over the limit */
  எனில்(d->numhttps_rrs < DOH_MAX_HTTPS) {
    கட்டமைப்பு dohhttps_rr *h = &d->https_rrs[d->numhttps_rrs];
    h->val = curlx_memdup(&doh[index], len);
    எனில்(!h->val)
      திரும்பு DOH_OUT_OF_MEM;
    h->len = len;
    d->numhttps_rrs++;
  }
  திரும்பு DOH_OK;
}
#endif

நிலையான DOHcode doh_rdata(மாறிலி unsigned எழுத்து *doh,
                         unsigned short rdlength,
                         unsigned short type,
                         முழு index,
                         கட்டமைப்பு dohentry *d)
{
  /* RDATA
     - A (TYPE 1): 4 bytes
     - AAAA (TYPE 28): 16 bytes
     - HTTPS (TYPE 65): N bytes */
  தேர்வு(type) {
  நிலை CURL_DNS_TYPE_A:
    எனில்(rdlength != 4)
      திரும்பு DOH_DNS_RDATA_LEN;
    doh_store_a(doh, index, d);
    நிறுத்து;
  நிலை CURL_DNS_TYPE_AAAA:
    எனில்(rdlength != 16)
      திரும்பு DOH_DNS_RDATA_LEN;
    doh_store_aaaa(doh, index, d);
    நிறுத்து;
#ifdef USE_HTTPSRR
  நிலை CURL_DNS_TYPE_HTTPS:
    எனில்(rdlength < 3)
      திரும்பு DOH_DNS_RDATA_LEN;
    திரும்பு doh_store_https(doh, index, d, rdlength);
#endif
  இயல்பு:
    /* unsupported type, or type we do not store, skip it */
    நிறுத்து;
  }
  திரும்பு DOH_OK;
}

/* @unittest 1655 */
UNITTEST வெறுமை de_init(கட்டமைப்பு dohentry *de);
UNITTEST வெறுமை de_init(கட்டமைப்பு dohentry *de)
{
  memset(de, 0, அளவு(*de));
  de->ttl = INT_MAX;
}

/* TTL value cap */
#define MAX_DNS_TTL 86400U /* 24 hours */
/* @unittest 1650 */
UNITTEST DOHcode doh_resp_decode(மாறிலி unsigned எழுத்து *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 கட்டமைப்பு dohentry *d);
UNITTEST DOHcode doh_resp_decode(மாறிலி unsigned எழுத்து *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 கட்டமைப்பு dohentry *d)
{
  unsigned எழுத்து rcode;
  unsigned short qdcount;
  unsigned short ancount;
  unsigned short type = 0;
  unsigned short rdlength;
  unsigned short nscount;
  unsigned short arcount;
  unsigned முழு index = 12;
  DOHcode rc;

  எனில்(dohlen < 12)
    திரும்பு DOH_TOO_SMALL_BUFFER; /* too small */
  எனில்(!doh || doh[0] || doh[1])
    திரும்பு DOH_DNS_BAD_ID; /* bad ID */
  rcode = doh[3] & 0x0f;
  எனில்(rcode == 3)
    திரும்பு DOH_DNS_NXDOMAIN; /* name does not exist */
  எனில்(rcode)
    திரும்பு DOH_DNS_BAD_RCODE; /* bad rcode */

  qdcount = doh_get16bit(doh, 4);
  போது(qdcount) {
    rc = doh_skipqname(doh, dohlen, &index);
    எனில்(rc)
      திரும்பு rc; /* bad qname */
    எனில்(dohlen < (index + 4))
      திரும்பு DOH_DNS_OUT_OF_RANGE;
    index += 4; /* skip question's type and class */
    qdcount--;
  }

  ancount = doh_get16bit(doh, 6);
  போது(ancount) {
    unsigned short dnsclass;
    unsigned முழு ttl;

    rc = doh_skipqname(doh, dohlen, &index);
    எனில்(rc)
      திரும்பு rc; /* bad qname */

    எனில்(dohlen < (index + 2))
      திரும்பு DOH_DNS_OUT_OF_RANGE;

    type = doh_get16bit(doh, index);
    எனில்((type != CURL_DNS_TYPE_CNAME) &&  /* may be synthesized from DNAME */
       (type != CURL_DNS_TYPE_DNAME) &&  /* if present, accept and ignore */
       (type != dnstype))
      /* Not the same type as was asked for, nor CNAME nor DNAME */
      திரும்பு DOH_DNS_UNEXPECTED_TYPE;
    index += 2;

    எனில்(dohlen < (index + 2))
      திரும்பு DOH_DNS_OUT_OF_RANGE;
    dnsclass = doh_get16bit(doh, index);
    எனில்(DNS_CLASS_IN != dnsclass)
      திரும்பு DOH_DNS_UNEXPECTED_CLASS; /* unsupported */
    index += 2;

    எனில்(dohlen < (index + 4))
      திரும்பு DOH_DNS_OUT_OF_RANGE;

    ttl = doh_get32bit(doh, index);
    எனில்(ttl > MAX_DNS_TTL)
      ttl = MAX_DNS_TTL;
    எனில்(ttl < d->ttl)
      d->ttl = ttl;
    index += 4;

    எனில்(dohlen < (index + 2))
      திரும்பு DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    எனில்(dohlen < (index + rdlength))
      திரும்பு DOH_DNS_OUT_OF_RANGE;

    rc = doh_rdata(doh, rdlength, type, (முழு)index, d);
    எனில்(rc)
      திரும்பு rc;
    index += rdlength;
    ancount--;
  }

  nscount = doh_get16bit(doh, 8);
  போது(nscount) {
    rc = doh_skipqname(doh, dohlen, &index);
    எனில்(rc)
      திரும்பு rc; /* bad qname */

    எனில்(dohlen < (index + 8))
      திரும்பு DOH_DNS_OUT_OF_RANGE;

    index += 2 + 2 + 4; /* type, dnsclass and ttl */

    எனில்(dohlen < (index + 2))
      திரும்பு DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    எனில்(dohlen < (index + rdlength))
      திரும்பு DOH_DNS_OUT_OF_RANGE;
    index += rdlength;
    nscount--;
  }

  arcount = doh_get16bit(doh, 10);
  போது(arcount) {
    rc = doh_skipqname(doh, dohlen, &index);
    எனில்(rc)
      திரும்பு rc; /* bad qname */

    எனில்(dohlen < (index + 8))
      திரும்பு DOH_DNS_OUT_OF_RANGE;

    index += 2 + 2 + 4; /* type, dnsclass and ttl */

    எனில்(dohlen < (index + 2))
      திரும்பு DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    எனில்(dohlen < (index + rdlength))
      திரும்பு DOH_DNS_OUT_OF_RANGE;
    index += rdlength;
    arcount--;
  }

  எனில்(index != dohlen)
    திரும்பு DOH_DNS_MALFORMAT; /* something is wrong */

  திரும்பு DOH_OK; /* ok */
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
நிலையான CURLcode doh2ai(மாறிலி கட்டமைப்பு dohentry *de, மாறிலி எழுத்து *hostname,
                       முழு port, கட்டமைப்பு Curl_addrinfo **aip)
{
  கட்டமைப்பு Curl_addrinfo *ai;
  கட்டமைப்பு Curl_addrinfo *prevai = வெற்று;
  கட்டமைப்பு Curl_addrinfo *firstai = வெற்று;
  கட்டமைப்பு sockaddr_in *addr;
#ifdef USE_IPV6
  கட்டமைப்பு sockaddr_in6 *addr6;
#endif
  size_t hostlen = strlen(hostname) + 1; /* include null-terminator */
  CURLcode result = CURLE_OK;
  முழு i;

  க்கு(i = 0; i < de->numaddr; i++) {
    size_t ss_size;
    CURL_SA_FAMILY_T addrtype;
    எனில்(de->addr[i].type == CURL_DNS_TYPE_AAAA) {
#ifndef USE_IPV6
      /* we cannot handle IPv6 addresses */
      தொடர்;
#இல்லையெனில்
      ss_size = அளவு(கட்டமைப்பு sockaddr_in6);
      addrtype = AF_INET6;
#endif
    }
    இல்லையெனில் {
      ss_size = அளவு(கட்டமைப்பு sockaddr_in);
      addrtype = AF_INET;
    }

    ai = curlx_calloc(1, அளவு(கட்டமைப்பு Curl_addrinfo) + ss_size + hostlen);
    எனில்(!ai) {
      result = CURLE_OUT_OF_MEMORY;
      நிறுத்து;
    }
    ai->ai_addr = (வெறுமை *)((எழுத்து *)ai + அளவு(கட்டமைப்பு Curl_addrinfo));
    ai->ai_canonname = (வெறுமை *)((எழுத்து *)ai->ai_addr + ss_size);
    memcpy(ai->ai_canonname, hostname, hostlen);

    எனில்(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    எனில்(prevai)
      /* make the previous entry point to this */
      prevai->ai_next = ai;

    ai->ai_family = addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_size;

    /* leave the rest of the struct filled with zero */

    தேர்வு(ai->ai_family) {
    நிலை AF_INET:
      addr = (வெறுமை *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(அளவு(கட்டமைப்பு in_addr) == அளவு(de->addr[i].ip.v4));
      memcpy(&addr->sin_addr, &de->addr[i].ip.v4, அளவு(கட்டமைப்பு in_addr));
      addr->sin_family = addrtype;
      addr->sin_port = htons((unsigned short)port);
      நிறுத்து;

#ifdef USE_IPV6
    நிலை AF_INET6:
      addr6 = (வெறுமை *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(அளவு(கட்டமைப்பு in6_addr) == அளவு(de->addr[i].ip.v6));
      memcpy(&addr6->sin6_addr, &de->addr[i].ip.v6, அளவு(கட்டமைப்பு in6_addr));
      addr6->sin6_family = addrtype;
      addr6->sin6_port = htons((unsigned short)port);
      நிறுத்து;
#endif
    }

    prevai = ai;
  }

  எனில்(result) {
    Curl_freeaddrinfo(firstai);
    firstai = வெற்று;
  }
  *aip = firstai;

  திரும்பு result;
}

/* @unittest 1655 */
UNITTEST வெறுமை de_cleanup(கட்டமைப்பு dohentry *d);
UNITTEST வெறுமை de_cleanup(கட்டமைப்பு dohentry *d)
{
#ifdef USE_HTTPSRR
  முழு i = 0;
  க்கு(i = 0; i < d->numhttps_rrs; i++)
    curlx_safefree(d->https_rrs[i].val);
#இல்லையெனில்
  (வெறுமை)d;
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
நிலையான CURLcode doh_decode_rdata_name(மாறிலி unsigned எழுத்து **buf,
                                      size_t *remaining, எழுத்து **dnsname)
{
  மாறிலி unsigned எழுத்து *cp = வெற்று;
  size_t rem = 0;
  unsigned எழுத்து clen = 0; /* chunk len */
  கட்டமைப்பு dynbuf thename;

  DEBUGASSERT(buf && remaining && dnsname);
  எனில்(!buf || !remaining || !dnsname || !*remaining)
    திரும்பு CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&thename, CURL_MAXLEN_HOST_NAME);
  rem = *remaining;
  cp = *buf;
  clen = *cp++;
  /* RFC 9460 says it must be uncompressed */
  எனில்(clen > 63)
    திரும்பு CURLE_WEIRD_SERVER_REPLY;

  எனில்(clen == 0) {
    /* special case - return "." as name */
    எனில்(curlx_dyn_addn(&thename, ".", 1))
      திரும்பு CURLE_OUT_OF_MEMORY;
  }
  போது(clen) {
    எனில்(clen >= rem) {
      curlx_dyn_free(&thename);
      திரும்பு CURLE_OUT_OF_MEMORY;
    }
    எனில்(curlx_dyn_addn(&thename, cp, clen) ||
       curlx_dyn_addn(&thename, ".", 1))
      திரும்பு CURLE_TOO_LARGE;

    cp += clen;
    rem -= (clen + 1);
    எனில்(rem <= 0) {
      curlx_dyn_free(&thename);
      திரும்பு CURLE_OUT_OF_MEMORY;
    }
    clen = *cp++;
    எனில்(clen > 63) {
      /* invalid format */
      curlx_dyn_free(&thename);
      திரும்பு CURLE_WEIRD_SERVER_REPLY;
    }
  }
  *buf = cp;
  *remaining = rem - 1;
  *dnsname = curlx_dyn_ptr(&thename);
  திரும்பு CURLE_OK;
}

/* scan for byte values <= 32 or 127 */
நிலையான CURLcode junkscan(மாறிலி எழுத்து *url)
{
  மாறிலி unsigned எழுத்து *p = (மாறிலி unsigned எழுத்து *)url;
  போது(*p) {
    எனில்(*p <= 0x20 || *p == 127)
      திரும்பு CURLE_WEIRD_SERVER_REPLY;
    p++;
  }
  திரும்பு CURLE_OK;
}

/* @unittest 1658 */
UNITTEST CURLcode doh_resp_decode_httpsrr(கட்டமைப்பு Curl_easy *data,
                                          மாறிலி unsigned எழுத்து *cp, size_t len,
                                          கட்டமைப்பு Curl_https_rrinfo **hrr);
UNITTEST CURLcode doh_resp_decode_httpsrr(கட்டமைப்பு Curl_easy *data,
                                          மாறிலி unsigned எழுத்து *cp, size_t len,
                                          கட்டமைப்பு Curl_https_rrinfo **hrr)
{
  uint16_t pcode = 0, plen = 0;
  uint32_t expected_min_pcode = 0;
  கட்டமைப்பு Curl_https_rrinfo *lhrr = வெற்று;
  எழுத்து *dnsname = வெற்று;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (வெறுமை)data;
  *hrr = வெற்று;
  எனில்(len <= 2)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  lhrr = curlx_calloc(1, அளவு(கட்டமைப்பு Curl_https_rrinfo));
  எனில்(!lhrr)
    திரும்பு CURLE_OUT_OF_MEMORY;
  lhrr->priority = doh_get16bit(cp, 0);
  cp += 2;
  len -= 2;
  எனில்(doh_decode_rdata_name(&cp, &len, &dnsname) != CURLE_OK)
    செல் err;
  lhrr->target = dnsname;
  result = junkscan(dnsname);
  எனில்(result) {
    /* unacceptable hostname content */
    செல் err;
  }
  போது(len >= 4) {
    pcode = doh_get16bit(cp, 0);
    plen = doh_get16bit(cp, 2);
    cp += 4;
    len -= 4;
    எனில்(pcode < expected_min_pcode || plen > len) {
      result = CURLE_WEIRD_SERVER_REPLY;
      செல் err;
    }
    result = Curl_httpsrr_set(lhrr, pcode, cp, plen);
    எனில்(result)
      செல் err;
    Curl_httpsrr_trace(data, lhrr);
    cp += plen;
    len -= plen;
    expected_min_pcode = pcode + 1;
  }
  *hrr = lhrr;
  திரும்பு CURLE_OK;
err:
  Curl_httpsrr_destroy(lhrr);
  திரும்பு result;
}

#endif /* USE_HTTPSRR */

/* called from multi when a sub transfer, e.g. doh probe, is done.
 * Parse the response and set the results in the `async` context
 * of master, using the id from the probe's CURL_EZM_DOH_PROBE
 * meta data. */
நிலையான வெறுமை doh_probe_done(கட்டமைப்பு Curl_easy *doh,
                           கட்டமைப்பு Curl_easy *master, CURLcode result)
{
  கட்டமைப்பு Curl_resolv_async *async = வெற்று;
  கட்டமைப்பு doh_probes *dohp = வெற்று;
  கட்டமைப்பு doh_request *doh_req = வெற்று;
  கட்டமைப்பு Curl_addrinfo **pdest_ai;
  கட்டமைப்பு dohentry de;
  முழு slot, httpcode;

  de_init(&de);
  doh_req = Curl_meta_get(doh, CURL_EZM_DOH_PROBE);
  எனில்(!doh_req) {
    /* transfer `doh` is not a DoH probe. */
    DEBUGASSERT(0);
    செல் out;
  }

  async = Curl_async_get(master, doh_req->resolv_id);
  எனில்(!async) {
    CURL_TRC_DNS(master, "[%u] ignoring outdated DoH response",
                 doh_req->resolv_id);
    செல் out;
  }
  dohp = async->doh;

  க்கு(slot = 0; slot < DOH_SLOT_மொத்தம்; ++slot) {
    எனில்(dohp->probe_mid[slot] == doh->mid)
      நிறுத்து;
  }
  /* We really should have found the slot where to store the response */
  எனில்(slot >= DOH_SLOT_மொத்தம்) {
    failf(master, "DoH: unknown sub request done");
    DEBUGASSERT(0);
    செல் out;
  }

  async->queries_ongoing--;
  dohp = async->doh;
  httpcode = doh->info.httpcode;
  தேர்வு(slot) {
  நிலை DOH_SLOT_IPV4:
    async->dns_responses |= CURL_DNSQ_A;
    நிறுத்து;
#ifdef USE_IPV6
  நிலை DOH_SLOT_IPV6:
    async->dns_responses |= CURL_DNSQ_AAAA;
    நிறுத்து;
#endif
#ifdef USE_HTTPSRR
  நிலை DOH_SLOT_HTTPS_RR:
    async->dns_responses |= CURL_DNSQ_HTTPS;
    நிறுத்து;
#endif
  இயல்பு:
    DEBUGASSERT(0);
    நிறுத்து;
  }

  எனில்(result) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: %s",
          doh_type2name(doh_req->dnstype), curl_easy_strerror(result));
    செல் out;
  }
  இல்லையெனில் எனில்((httpcode < 200) || (httpcode >= 300)) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: HTTP status %d",
          doh_type2name(doh_req->dnstype), httpcode);
    செல் out;
  }

  dohp->probe_rc[slot] = doh_resp_decode(curlx_dyn_uptr(&doh_req->resp_body),
                                         curlx_dyn_len(&doh_req->resp_body),
                                         doh_req->dnstype, &de);
  எனில்(dohp->probe_rc[slot]) {
#ifdef USE_HTTPSRR
    எனில்((dohp->probe_rc[slot] == DOH_NO_CONTENT) &&
       (doh_req->dnstype == CURL_DNS_TYPE_HTTPS)) {
      dohp->probe_rc[slot] = DOH_DNS_NXDOMAIN;
    }
#endif
    infof(doh, "[DoH] [%s] error decoding response: %s",
          doh_type2name(doh_req->dnstype),
          doh_strerror(dohp->probe_rc[slot]));
    செல் out;
  }

  எனில்(doh_req->dnstype == CURL_DNS_TYPE_A)
    pdest_ai = &async->ai_A;
  இல்லையெனில் எனில்(doh_req->dnstype == CURL_DNS_TYPE_AAAA)
    pdest_ai = &async->ai_AAAA;
  இல்லையெனில்
    pdest_ai = வெற்று;

  எனில்(pdest_ai && de.numaddr) {
    எனில்(*pdest_ai) {
      Curl_freeaddrinfo(*pdest_ai);
      *pdest_ai = வெற்று;
    }
    result = doh2ai(&de, async->peer->hostname, async->peer->port, pdest_ai);
    எனில்(result) { /* hard failure on our side, fail completely */
      infof(doh, "[DoH] [%s] error creating addrinfo: %s",
            doh_type2name(doh_req->dnstype), curl_easy_strerror(result));
      dohp->probe_rc[slot] = DOH_OOM;
      async->result = result;
    }
  }
#ifdef USE_HTTPSRR
  இல்லையெனில் எனில்((doh_req->dnstype == CURL_DNS_TYPE_HTTPS) && de.numhttps_rrs) {
    CURL_TRC_DNS(doh, "[HTTPS] got %d records", de.numhttps_rrs);
    result = doh_resp_decode_httpsrr(doh, de.https_rrs->val,
                                     de.https_rrs->len, &async->httpsrr);
    எனில்(result) {
      dohp->probe_rc[slot] = DOH_HTTP_FAILED;
      infof(doh, "[DoH] error decoding HTTPS RR: %s",
            curl_easy_strerror(result));
      செல் out;
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

CURLcode Curl_doh_take_result(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு Curl_resolv_async *async,
                              கட்டமைப்பு Curl_dns_entry **pdns)
{
  கட்டமைப்பு doh_probes *dohp = async->doh;
  CURLcode result = CURLE_OK;

  *pdns = வெற்று; /* defaults to no response */
  எனில்(!dohp)
    திரும்பு CURLE_OUT_OF_MEMORY;

  async->negative_answer = FALSE;
  எனில்(async->result) {
    result = async->result;
    செல் out;
  }

  எனில்(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     dohp->probe_mid[DOH_SLOT_IPV4] == UINT32_MAX &&
     dohp->probe_mid[DOH_SLOT_IPV6] == UINT32_MAX) {
    failf(data, "Could not DoH-resolve: %s", async->peer->hostname);
    திரும்பு async->for_proxy ?
      CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
  }
  இல்லையெனில் எனில்(!async->queries_ongoing) {
    கட்டமைப்பு Curl_dns_entry *dns = வெற்று;
    bool negative = TRUE;
    முழு slot;

    /* remove DoH handles from multi handle and close them */
    doh_close(data, async);
    /* parse the responses, create the struct and return it! */
    க்கு(slot = 0; slot < DOH_SLOT_மொத்தம்; slot++) {
      /* Failing without an NXDOMAIN answer - a SERVFAIL-class rcode or
         an undecodable response - says nothing about the name. Such a
         failure must not be cached as a negative entry. */
      எனில்(dohp->probe_rc[slot] && (dohp->probe_rc[slot] != DOH_DNS_NXDOMAIN))
        negative = FALSE;
    } /* next slot */

    எனில்(async->ai_A || async->ai_AAAA) {
      dns = Curl_dnsc_mk_addr2(
        data, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
      எனில்(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        செல் out;
      }
    }
#ifdef USE_HTTPSRR
    இல்லையெனில் எனில்((async->dns_queries & CURL_DNSQ_HTTPS) &&
            !dohp->probe_rc[DOH_SLOT_HTTPS_RR]) {
      Curl_httpsrr_trace(data, async->httpsrr);
      dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
      எனில்(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        செல் out;
      }
    }
#endif /* USE_HTTPSRR */
    இல்லையெனில் {
      /* every query failed. Only NXDOMAIN answers for all of them
         make this a negative answer, eligible for caching. */
      async->negative_answer = negative;
      result = async->for_proxy ?
        CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
    }

    /* and add the entry to the cache */
    எனில்(dns)
      result = Curl_dnscache_add(data, dns);
    *pdns = dns;
  } /* !async->queries_ongoing */
  இல்லையெனில்
    /* wait for pending DoH transactions to complete */
    திரும்பு CURLE_AGAIN;

out:
  Curl_doh_cleanup(data, async);
  திரும்பு result;
}

நிலையான வெறுமை doh_close(கட்டமைப்பு Curl_easy *data,
                      கட்டமைப்பு Curl_resolv_async *async)
{
  கட்டமைப்பு doh_probes *doh = async ? async->doh : வெற்று;
  எனில்(doh && data->multi) {
    கட்டமைப்பு Curl_easy *probe_data;
    uint32_t mid;
    size_t slot;
    க்கு(slot = 0; slot < DOH_SLOT_மொத்தம்; slot++) {
      mid = doh->probe_mid[slot];
      எனில்(mid == UINT32_MAX)
        தொடர்;
      doh->probe_mid[slot] = UINT32_MAX;
      /* should have been called before data is removed from multi handle */
      DEBUGASSERT(data->multi);
      probe_data = data->multi ? Curl_multi_get_easy(data->multi, mid) : வெற்று;
      எனில்(!probe_data) {
        DEBUGF(infof(data, "Curl_doh_close: xfer for mid=%u not found!", mid));
        தொடர்;
      }
      probe_data->sub_xfer_done = வெற்று; /* No longer interested in result */
      /* data->multi might already be reset at this time */
      Curl_multi_remove_handle(data->multi, probe_data);
      Curl_close(&probe_data);
    }
    CURL_TRC_DNS(data, "[DoH] probe done");
  }
}

வெறுமை Curl_doh_cleanup(கட்டமைப்பு Curl_easy *data,
                      கட்டமைப்பு Curl_resolv_async *async)
{
  கட்டமைப்பு doh_probes *dohp = async->doh;
  எனில்(dohp) {
    doh_close(data, async);
    curlx_safefree(async->doh);
  }
}

#endif /* CURL_DISABLE_DOH */
