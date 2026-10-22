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
#অন্তর্ভাৱ "curl_setup.h"

#ifndef CURL_DISABLE_DOH

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "curl_addrinfo.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "multiif.h"
#অন্তর্ভাৱ "url.h"
#অন্তর্ভাৱ "connect.h"
#অন্তর্ভাৱ "vdns/doh.h"
#অন্তর্ভাৱ "vdns/httpsrr.h"
#অন্তর্ভাৱ "curlx/strdup.h"
#অন্তর্ভাৱ "curlx/dynbuf.h"
#অন্তর্ভাৱ "escape.h"  /* for Curl_hexencode() */
#অন্তর্ভাৱ "urlapi-int.h"

#define DNS_CLASS_IN 0x01

স্থির শূন্য doh_close(সংরচনা Curl_easy *data,
                      সংরচনা Curl_resolv_async *async);

#ifdef CURLVERBOSE
স্থির ধ্রুৱ ৱর্ণ * ধ্রুৱ doh_code_str[] = {
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

স্থির ধ্রুৱ ৱর্ণ *doh_strerror(DOHcode code)
{
  যদি((size_t)code < CURL_ARRAYSIZE(doh_code_str))
    ঘূৰাই doh_code_str[code];
  ঘূৰাই "bad error code";
}

স্থির ধ্রুৱ ৱর্ণ *doh_type2name(DNStype dnstype)
{
  ৱিকল্প(dnstype) {
  পক্ষ CURL_DNS_TYPE_A:
    ঘূৰাই "A";
  পক্ষ CURL_DNS_TYPE_AAAA:
    ঘূৰাই "AAAA";
#ifdef USE_HTTPSRR
  পক্ষ CURL_DNS_TYPE_HTTPS:
    ঘূৰাই "HTTPS";
#endif
  স্ৱভাৱ:
    ঘূৰাই "unknown";
  }
}

#endif /* CURLVERBOSE */

/* @unittest 1655
 */
UNITTEST DOHcode doh_req_encode(ধ্রুৱ ৱর্ণ *host,
                                DNStype dnstype,
                                unsigned ৱর্ণ *dnsp,  /* buffer */
                                size_t len,  /* buffer size */
                                size_t *olen);  /* output length */
UNITTEST DOHcode doh_req_encode(ধ্রুৱ ৱর্ণ *host,
                                DNStype dnstype,
                                unsigned ৱর্ণ *dnsp, /* buffer */
                                size_t len,   /* buffer size */
                                size_t *olen) /* output length */
{
  ধ্রুৱ size_t hostlen = strlen(host);
  unsigned ৱর্ণ *orig = dnsp;
  ধ্রুৱ ৱর্ণ *hostp = host;

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

  যদি(expected_len > DOH_MAX_DNSREQ_SIZE)
    ঘূৰাই DOH_DNS_NAME_TOO_LONG;

  যদি(len < expected_len)
    ঘূৰাই DOH_TOO_SMALL_BUFFER;

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
  যেতিয়ালৈ(*hostp) {
    size_t labellen;
    ধ্রুৱ ৱর্ণ *dot = strchr(hostp, '.');
    যদি(dot)
      labellen = dot - hostp;
    অন্যথা
      labellen = strlen(hostp);
    যদি((labellen > 63) || (!labellen)) {
      /* label is too long or too short, error out */
      *olen = 0;
      ঘূৰাই DOH_DNS_BAD_LABEL;
    }
    /* label is non-empty, process it */
    *dnsp++ = (unsigned ৱর্ণ)labellen;
    memcpy(dnsp, hostp, labellen);
    dnsp += labellen;
    hostp += labellen;
    /* advance past dot, but only if there is one */
    যদি(dot)
      hostp++;
  } /* next label */

  *dnsp++ = 0; /* append zero-length label for root */

  /* There are assigned TYPE codes beyond 255: use range [1..65535] */
  *dnsp++ = (unsigned ৱর্ণ)(255 & (dnstype >> 8)); /* upper 8-bit TYPE */
  *dnsp++ = (unsigned ৱর্ণ)(255 & dnstype);        /* lower 8-bit TYPE */

  *dnsp++ = '\0'; /* upper 8-bit CLASS */
  *dnsp++ = DNS_CLASS_IN; /* IN - "the Internet" */

  *olen = dnsp - orig;

  /* verify that our estimation of length is valid, since
   * this has led to buffer overflows in this function */
  DEBUGASSERT(*olen == expected_len);
  ঘূৰাই DOH_OK;
}

স্থির size_t doh_probe_write_cb(ৱর্ণ *contents, size_t size, size_t nmemb,
                                 শূন্য *userp)
{
  size_t realsize = size * nmemb;
  সংরচনা Curl_easy *data = userp;
  সংরচনা doh_request *doh_req = Curl_meta_get(data, CURL_EZM_DOH_PROBE);
  যদি(!doh_req)
    ঘূৰাই CURL_WRITEFUNC_ERROR;

  যদি(curlx_dyn_addn(&doh_req->resp_body, contents, realsize))
    ঘূৰাই 0;

  ঘূৰাই realsize;
}

স্থির শূন্য doh_probe_done(সংরচনা Curl_easy *doh,
                           সংরচনা Curl_easy *master, CURLcode result);
স্থির শূন্য doh_probe_dtor(ধ্রুৱ শূন্য *চাবি, size_t klen, শূন্য *e)
{
  (শূন্য)চাবি;
  (শূন্য)klen;
  যদি(e) {
    সংরচনা doh_request *doh_req = e;
    curl_slist_free_all(doh_req->req_hds);
    curlx_dyn_free(&doh_req->resp_body);
    curlx_free(e);
  }
}

#define ERROR_CHECK_SETOPT(x, y)                        \
  কৰা {                                                  \
    result = curl_easy_setopt((CURL *)doh, x, y);       \
    যদি(result &&                                        \
       result != CURLE_NOT_BUILT_IN &&                  \
       result != CURLE_UNKNOWN_OPTION)                  \
      গচ্ছ error;                                       \
  } যেতিয়ালৈ(0)

স্থির CURLcode doh_probe_run(সংরচনা Curl_easy *data,
                              DNStype dnstype,
                              ধ্রুৱ ৱর্ণ *host,
                              ধ্রুৱ ৱর্ণ *url, CURLM *multi,
                              uint32_t resolv_id,
                              uint32_t *pmid)
{
  সংরচনা Curl_easy *doh = রিক্ত;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  সংরচনা doh_request *doh_req;
  DOHcode d;
  bool maybe_https = !curl_strnequal(url, STRCONST("http:"));

  *pmid = UINT32_MAX;

  doh_req = curlx_calloc(1, পরিমাণ(*doh_req));
  যদি(!doh_req)
    ঘূৰাই CURLE_OUT_OF_MEMORY;
  doh_req->resolv_id = resolv_id;
  doh_req->dnstype = dnstype;
  curlx_dyn_init(&doh_req->resp_body, DYN_DOH_RESPONSE);

  d = doh_req_encode(host, dnstype, doh_req->req_body,
                     পরিমাণ(doh_req->req_body),
                     &doh_req->req_body_len);
  যদি(d) {
    failf(data, "Failed to encode DoH packet [%d]", (পূর্ণাঙ্ক)d);
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ error;
  }

  timeout_ms = Curl_timeleft_ms(data);
  যদি(timeout_ms < 0) {
    result = CURLE_OPERATION_TIMEDOUT;
    গচ্ছ error;
  }

  doh_req->req_hds =
    curl_slist_append(রিক্ত, "Content-Type: application/dns-message");
  যদি(!doh_req->req_hds) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ error;
  }

  /* Curl_open() is the internal version of curl_easy_init() */
  result = Curl_open(&doh);
  যদি(result)
    গচ্ছ error;

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
  ERROR_CHECK_SETOPT(CURLOPT_SHARE, (CURLSH *)data->share);
  যদি(data->set.err && data->set.err != stderr)
    ERROR_CHECK_SETOPT(CURLOPT_STDERR, data->set.err);
  যদি(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_doh))
    ERROR_CHECK_SETOPT(CURLOPT_VERBOSE, 1L);
  যদি(data->set.no_signal)
    ERROR_CHECK_SETOPT(CURLOPT_NOSIGNAL, 1L);
  যদি(data->set.fdebug)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGFUNCTION, data->set.fdebug);
  যদি(data->set.debugdata)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGDATA, data->set.debugdata);

#ifdef USE_RESOLV_ARES
  যদি(CURL_EASY_STR(data, STRING_DNS_SERVERS)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_SERVERS,
                       CURL_EASY_STR(data, STRING_DNS_SERVERS));
  }
  যদি(CURL_EASY_STR(data, STRING_DNS_INTERFACE)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_INTERFACE,
                       CURL_EASY_STR(data, STRING_DNS_INTERFACE));
  }
  যদি(CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP4,
                       CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4));
  }
  যদি(CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP6,
                       CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6));
  }
#endif
  যদি(maybe_https) {
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
    যদি(CURL_EASY_STR(data, STRING_SSL_CAFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO,
                         CURL_EASY_STR(data, STRING_SSL_CAFILE));
    }
    যদি(data->set.blobs[BLOB_CAINFO]) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO_BLOB, data->set.blobs[BLOB_CAINFO]);
    }
    যদি(CURL_EASY_STR(data, STRING_SSL_CAPATH)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAPATH,
                         CURL_EASY_STR(data, STRING_SSL_CAPATH));
    }
    যদি(CURL_EASY_STR(data, STRING_SSL_CRLFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CRLFILE,
                         CURL_EASY_STR(data, STRING_SSL_CRLFILE));
    }
    যদি(data->set.ssl.certinfo)
      ERROR_CHECK_SETOPT(CURLOPT_CERTINFO, 1L);
    যদি(data->set.ssl_fsslctx)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_FUNCTION, data->set.ssl_fsslctx);
    যদি(data->set.ssl_fsslctxp)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_DATA, data->set.ssl_fsslctxp);
    যদি(CURL_EASY_STR(data, STRING_SSL_EC_CURVES)) {
      ERROR_CHECK_SETOPT(CURLOPT_SSL_EC_CURVES,
                         CURL_EASY_STR(data, STRING_SSL_EC_CURVES));
    }
    (শূন্য)curl_easy_setopt(doh, CURLOPT_SSL_OPTIONS,
                           ((long)data->set.ssl.ssl_options &
                            ~CURLSSLOPT_AUTO_CLIENT_CERT));
  }

  doh->state.internal = TRUE;
  doh->master_mid = data->mid; /* master transfer of this one */
  doh->sub_xfer_done = doh_probe_done;

  result = Curl_meta_set(doh, CURL_EZM_DOH_PROBE, doh_req, doh_probe_dtor);
  doh_req = রিক্ত; /* call took ownership */
  যদি(result)
    গচ্ছ error;

  /* DoH handles must not inherit private_data. The handles may be passed to
     the user via callbacks and the user will be able to identify them as
     internal handles because private data is not set. The user can then set
     private_data via CURLOPT_PRIVATE if they so choose. */
  DEBUGASSERT(!doh->set.private_data);

  যদি(Curl_multi_add_handle(multi, doh))
    গচ্ছ error;

  *pmid = doh->mid;
  ঘূৰাই CURLE_OK;

error:
  Curl_close(&doh);
  যদি(doh_req)
    doh_probe_dtor(রিক্ত, 0, doh_req);
  ঘূৰাই result;
}

/*
 * Curl_doh() starts a name resolve using DoH. It resolves a name and returns
 * a 'Curl_addrinfo *' with the address information.
 */

CURLcode Curl_doh(সংরচনা Curl_easy *data,
                  সংরচনা Curl_resolv_async *async)
{
  CURLcode result = CURLE_OK;
  সংরচনা doh_probes *dohp = রিক্ত;
  size_t i;

  DEBUGASSERT(!async->doh);
  DEBUGASSERT(async->peer->hostname[0]);
  যদি(async->doh) {
    DEBUGASSERT(0); /* should not happen */
    Curl_doh_cleanup(data, async);
  }

  যদি(!async->dns_queries)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef USE_HTTPSRR
  যদি(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     (async->dns_queries & CURL_DNSQ_HTTPS)) {
    /* Can't mix those in the same async resolve */
    DEBUGASSERT(0);
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  }
#অন্যথা
  যদি(async->dns_queries & CURL_DNSQ_HTTPS) {
    DEBUGASSERT(0);
    ঘূৰাই CURLE_NOT_BUILT_IN;
  }
#endif

  /* start clean, consider allocating this struct on demand */
  async->doh = dohp = curlx_calloc(1, পরিমাণ(সংরচনা doh_probes));
  যদি(!dohp)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  বাবে(i = 0; i < DOH_SLOT_গুনতি; ++i) {
    dohp->probe_rc[i] = DOH_OK;
    dohp->probe_mid[i] = UINT32_MAX;
  }

#ifdef USE_IPV6
  /* AAAA results have preference in happy eyeballing, trigger first */
  যদি(async->dns_queries & CURL_DNSQ_AAAA) {
    /* create IPv6 DoH request */
    result = doh_probe_run(data, CURL_DNS_TYPE_AAAA,
                           async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH),
                           data->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV6]);
    যদি(result)
      গচ্ছ error;
    async->queries_ongoing++;
  }
#endif

  /* create IPv4 DoH request */
  যদি(async->dns_queries & CURL_DNSQ_A) {
    result = doh_probe_run(data, CURL_DNS_TYPE_A,
                           async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH),
                           data->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV4]);
    যদি(result)
      গচ্ছ error;
    async->queries_ongoing++;
  }

#ifdef USE_HTTPSRR
  যদি(async->dns_queries & CURL_DNSQ_HTTPS) {
    ৱর্ণ *qname = রিক্ত;
    যদি(async->peer->port != PORT_HTTPS) {
      qname = curl_maprintf("_%u._https.%s",
                            async->peer->port, async->peer->hostname);
      যদি(!qname)
        গচ্ছ error;
    }
    result = doh_probe_run(data, CURL_DNS_TYPE_HTTPS,
                           qname ? qname : async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH), data->multi,
                           async->id,
                           &dohp->probe_mid[DOH_SLOT_HTTPS_RR]);
    curlx_free(qname);
    যদি(result)
      গচ্ছ error;
    async->queries_ongoing++;
  }
#endif
  ঘূৰাই CURLE_OK;

error:
  Curl_doh_cleanup(data, async);
  ঘূৰাই result;
}

স্থির DOHcode doh_skipqname(ধ্রুৱ unsigned ৱর্ণ *doh, size_t dohlen,
                             unsigned পূর্ণাঙ্ক *indexp)
{
  unsigned ৱর্ণ length;
  কৰা {
    যদি(dohlen < (*indexp + 1))
      ঘূৰাই DOH_DNS_OUT_OF_RANGE;
    length = doh[*indexp];
    যদি((length & 0xc0) == 0xc0) {
      /* name pointer, advance over it and be done */
      যদি(dohlen < (*indexp + 2))
        ঘূৰাই DOH_DNS_OUT_OF_RANGE;
      *indexp += 2;
      ভঙ্গ;
    }
    যদি(length & 0xc0)
      ঘূৰাই DOH_DNS_BAD_LABEL;
    যদি(dohlen < (*indexp + 1 + length))
      ঘূৰাই DOH_DNS_OUT_OF_RANGE;
    *indexp += (unsigned পূর্ণাঙ্ক)(1 + length);
  } যেতিয়ালৈ(length);
  ঘূৰাই DOH_OK;
}

স্থির unsigned short doh_get16bit(ধ্রুৱ unsigned ৱর্ণ *doh,
                                   unsigned পূর্ণাঙ্ক index)
{
  ঘূৰাই (unsigned short)((doh[index] << 8) | doh[index + 1]);
}

স্থির unsigned পূর্ণাঙ্ক doh_get32bit(ধ্রুৱ unsigned ৱর্ণ *doh, unsigned পূর্ণাঙ্ক index)
{
  /* make clang and gcc optimize this to bswap by incrementing
     the pointer first. */
  doh += index;

  /* avoid undefined behavior by casting to unsigned before shifting
     24 bits, possibly into the sign bit. codegen is same, but
     ub sanitizer will not be upset */
  ঘূৰাই ((unsigned)doh[0] << 24) | ((unsigned)doh[1] << 16) |
         ((unsigned)doh[2] << 8) | doh[3];
}

স্থির শূন্য doh_store_a(ধ্রুৱ unsigned ৱর্ণ *doh, পূর্ণাঙ্ক index,
                        সংরচনা dohentry *d)
{
  /* silently ignore addresses over the limit */
  যদি(d->numaddr < DOH_MAX_ADDR) {
    সংরচনা dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_A;
    memcpy(&a->ip.v4, &doh[index], 4);
    d->numaddr++;
  }
}

স্থির শূন্য doh_store_aaaa(ধ্রুৱ unsigned ৱর্ণ *doh, পূর্ণাঙ্ক index,
                           সংরচনা dohentry *d)
{
  /* silently ignore addresses over the limit */
  যদি(d->numaddr < DOH_MAX_ADDR) {
    সংরচনা dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_AAAA;
    memcpy(&a->ip.v6, &doh[index], 16);
    d->numaddr++;
  }
}

#ifdef USE_HTTPSRR
স্থির DOHcode doh_store_https(ধ্রুৱ unsigned ৱর্ণ *doh, পূর্ণাঙ্ক index,
                               সংরচনা dohentry *d, uint16_t len)
{
  /* silently ignore RRs over the limit */
  যদি(d->numhttps_rrs < DOH_MAX_HTTPS) {
    সংরচনা dohhttps_rr *h = &d->https_rrs[d->numhttps_rrs];
    h->val = curlx_memdup(&doh[index], len);
    যদি(!h->val)
      ঘূৰাই DOH_OUT_OF_MEM;
    h->len = len;
    d->numhttps_rrs++;
  }
  ঘূৰাই DOH_OK;
}
#endif

স্থির DOHcode doh_rdata(ধ্রুৱ unsigned ৱর্ণ *doh,
                         unsigned short rdlength,
                         unsigned short type,
                         পূর্ণাঙ্ক index,
                         সংরচনা dohentry *d)
{
  /* RDATA
     - A (TYPE 1): 4 bytes
     - AAAA (TYPE 28): 16 bytes
     - HTTPS (TYPE 65): N bytes */
  ৱিকল্প(type) {
  পক্ষ CURL_DNS_TYPE_A:
    যদি(rdlength != 4)
      ঘূৰাই DOH_DNS_RDATA_LEN;
    doh_store_a(doh, index, d);
    ভঙ্গ;
  পক্ষ CURL_DNS_TYPE_AAAA:
    যদি(rdlength != 16)
      ঘূৰাই DOH_DNS_RDATA_LEN;
    doh_store_aaaa(doh, index, d);
    ভঙ্গ;
#ifdef USE_HTTPSRR
  পক্ষ CURL_DNS_TYPE_HTTPS:
    যদি(rdlength < 3)
      ঘূৰাই DOH_DNS_RDATA_LEN;
    ঘূৰাই doh_store_https(doh, index, d, rdlength);
#endif
  স্ৱভাৱ:
    /* unsupported type, or type we do not store, skip it */
    ভঙ্গ;
  }
  ঘূৰাই DOH_OK;
}

/* @unittest 1655 */
UNITTEST শূন্য de_init(সংরচনা dohentry *de);
UNITTEST শূন্য de_init(সংরচনা dohentry *de)
{
  memset(de, 0, পরিমাণ(*de));
  de->ttl = INT_MAX;
}

/* TTL value cap */
#define MAX_DNS_TTL 86400U /* 24 hours */
/* @unittest 1650 */
UNITTEST DOHcode doh_resp_decode(ধ্রুৱ unsigned ৱর্ণ *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 সংরচনা dohentry *d);
UNITTEST DOHcode doh_resp_decode(ধ্রুৱ unsigned ৱর্ণ *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 সংরচনা dohentry *d)
{
  unsigned ৱর্ণ rcode;
  unsigned short qdcount;
  unsigned short ancount;
  unsigned short type = 0;
  unsigned short rdlength;
  unsigned short nscount;
  unsigned short arcount;
  unsigned পূর্ণাঙ্ক index = 12;
  DOHcode rc;

  যদি(dohlen < 12)
    ঘূৰাই DOH_TOO_SMALL_BUFFER; /* too small */
  যদি(!doh || doh[0] || doh[1])
    ঘূৰাই DOH_DNS_BAD_ID; /* bad ID */
  rcode = doh[3] & 0x0f;
  যদি(rcode == 3)
    ঘূৰাই DOH_DNS_NXDOMAIN; /* name does not exist */
  যদি(rcode)
    ঘূৰাই DOH_DNS_BAD_RCODE; /* bad rcode */

  qdcount = doh_get16bit(doh, 4);
  যেতিয়ালৈ(qdcount) {
    rc = doh_skipqname(doh, dohlen, &index);
    যদি(rc)
      ঘূৰাই rc; /* bad qname */
    যদি(dohlen < (index + 4))
      ঘূৰাই DOH_DNS_OUT_OF_RANGE;
    index += 4; /* skip question's type and class */
    qdcount--;
  }

  ancount = doh_get16bit(doh, 6);
  যেতিয়ালৈ(ancount) {
    unsigned short dnsclass;
    unsigned পূর্ণাঙ্ক ttl;

    rc = doh_skipqname(doh, dohlen, &index);
    যদি(rc)
      ঘূৰাই rc; /* bad qname */

    যদি(dohlen < (index + 2))
      ঘূৰাই DOH_DNS_OUT_OF_RANGE;

    type = doh_get16bit(doh, index);
    যদি((type != CURL_DNS_TYPE_CNAME) &&  /* may be synthesized from DNAME */
       (type != CURL_DNS_TYPE_DNAME) &&  /* if present, accept and ignore */
       (type != dnstype))
      /* Not the same type as was asked for, nor CNAME nor DNAME */
      ঘূৰাই DOH_DNS_UNEXPECTED_TYPE;
    index += 2;

    যদি(dohlen < (index + 2))
      ঘূৰাই DOH_DNS_OUT_OF_RANGE;
    dnsclass = doh_get16bit(doh, index);
    যদি(DNS_CLASS_IN != dnsclass)
      ঘূৰাই DOH_DNS_UNEXPECTED_CLASS; /* unsupported */
    index += 2;

    যদি(dohlen < (index + 4))
      ঘূৰাই DOH_DNS_OUT_OF_RANGE;

    ttl = doh_get32bit(doh, index);
    যদি(ttl > MAX_DNS_TTL)
      ttl = MAX_DNS_TTL;
    যদি(ttl < d->ttl)
      d->ttl = ttl;
    index += 4;

    যদি(dohlen < (index + 2))
      ঘূৰাই DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    যদি(dohlen < (index + rdlength))
      ঘূৰাই DOH_DNS_OUT_OF_RANGE;

    rc = doh_rdata(doh, rdlength, type, (পূর্ণাঙ্ক)index, d);
    যদি(rc)
      ঘূৰাই rc;
    index += rdlength;
    ancount--;
  }

  nscount = doh_get16bit(doh, 8);
  যেতিয়ালৈ(nscount) {
    rc = doh_skipqname(doh, dohlen, &index);
    যদি(rc)
      ঘূৰাই rc; /* bad qname */

    যদি(dohlen < (index + 8))
      ঘূৰাই DOH_DNS_OUT_OF_RANGE;

    index += 2 + 2 + 4; /* type, dnsclass and ttl */

    যদি(dohlen < (index + 2))
      ঘূৰাই DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    যদি(dohlen < (index + rdlength))
      ঘূৰাই DOH_DNS_OUT_OF_RANGE;
    index += rdlength;
    nscount--;
  }

  arcount = doh_get16bit(doh, 10);
  যেতিয়ালৈ(arcount) {
    rc = doh_skipqname(doh, dohlen, &index);
    যদি(rc)
      ঘূৰাই rc; /* bad qname */

    যদি(dohlen < (index + 8))
      ঘূৰাই DOH_DNS_OUT_OF_RANGE;

    index += 2 + 2 + 4; /* type, dnsclass and ttl */

    যদি(dohlen < (index + 2))
      ঘূৰাই DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    যদি(dohlen < (index + rdlength))
      ঘূৰাই DOH_DNS_OUT_OF_RANGE;
    index += rdlength;
    arcount--;
  }

  যদি(index != dohlen)
    ঘূৰাই DOH_DNS_MALFORMAT; /* something is wrong */

  ঘূৰাই DOH_OK; /* ok */
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
স্থির CURLcode doh2ai(ধ্রুৱ সংরচনা dohentry *de, ধ্রুৱ ৱর্ণ *hostname,
                       পূর্ণাঙ্ক port, সংরচনা Curl_addrinfo **aip)
{
  সংরচনা Curl_addrinfo *ai;
  সংরচনা Curl_addrinfo *prevai = রিক্ত;
  সংরচনা Curl_addrinfo *firstai = রিক্ত;
  সংরচনা sockaddr_in *addr;
#ifdef USE_IPV6
  সংরচনা sockaddr_in6 *addr6;
#endif
  size_t hostlen = strlen(hostname) + 1; /* include null-terminator */
  CURLcode result = CURLE_OK;
  পূর্ণাঙ্ক i;

  বাবে(i = 0; i < de->numaddr; i++) {
    size_t ss_size;
    CURL_SA_FAMILY_T addrtype;
    যদি(de->addr[i].type == CURL_DNS_TYPE_AAAA) {
#ifndef USE_IPV6
      /* we cannot handle IPv6 addresses */
      অনুৱর্ত;
#অন্যথা
      ss_size = পরিমাণ(সংরচনা sockaddr_in6);
      addrtype = AF_INET6;
#endif
    }
    অন্যথা {
      ss_size = পরিমাণ(সংরচনা sockaddr_in);
      addrtype = AF_INET;
    }

    ai = curlx_calloc(1, পরিমাণ(সংরচনা Curl_addrinfo) + ss_size + hostlen);
    যদি(!ai) {
      result = CURLE_OUT_OF_MEMORY;
      ভঙ্গ;
    }
    ai->ai_addr = (শূন্য *)((ৱর্ণ *)ai + পরিমাণ(সংরচনা Curl_addrinfo));
    ai->ai_canonname = (শূন্য *)((ৱর্ণ *)ai->ai_addr + ss_size);
    memcpy(ai->ai_canonname, hostname, hostlen);

    যদি(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    যদি(prevai)
      /* make the previous entry point to this */
      prevai->ai_next = ai;

    ai->ai_family = addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_size;

    /* leave the rest of the struct filled with zero */

    ৱিকল্প(ai->ai_family) {
    পক্ষ AF_INET:
      addr = (শূন্য *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(পরিমাণ(সংরচনা in_addr) == পরিমাণ(de->addr[i].ip.v4));
      memcpy(&addr->sin_addr, &de->addr[i].ip.v4, পরিমাণ(সংরচনা in_addr));
      addr->sin_family = addrtype;
      addr->sin_port = htons((unsigned short)port);
      ভঙ্গ;

#ifdef USE_IPV6
    পক্ষ AF_INET6:
      addr6 = (শূন্য *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(পরিমাণ(সংরচনা in6_addr) == পরিমাণ(de->addr[i].ip.v6));
      memcpy(&addr6->sin6_addr, &de->addr[i].ip.v6, পরিমাণ(সংরচনা in6_addr));
      addr6->sin6_family = addrtype;
      addr6->sin6_port = htons((unsigned short)port);
      ভঙ্গ;
#endif
    }

    prevai = ai;
  }

  যদি(result) {
    Curl_freeaddrinfo(firstai);
    firstai = রিক্ত;
  }
  *aip = firstai;

  ঘূৰাই result;
}

/* @unittest 1655 */
UNITTEST শূন্য de_cleanup(সংরচনা dohentry *d);
UNITTEST শূন্য de_cleanup(সংরচনা dohentry *d)
{
#ifdef USE_HTTPSRR
  পূর্ণাঙ্ক i = 0;
  বাবে(i = 0; i < d->numhttps_rrs; i++)
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
স্থির CURLcode doh_decode_rdata_name(ধ্রুৱ unsigned ৱর্ণ **buf,
                                      size_t *remaining, ৱর্ণ **dnsname)
{
  ধ্রুৱ unsigned ৱর্ণ *cp = রিক্ত;
  size_t rem = 0;
  unsigned ৱর্ণ clen = 0; /* chunk len */
  সংরচনা dynbuf thename;

  DEBUGASSERT(buf && remaining && dnsname);
  যদি(!buf || !remaining || !dnsname || !*remaining)
    ঘূৰাই CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&thename, CURL_MAXLEN_HOST_NAME);
  rem = *remaining;
  cp = *buf;
  clen = *cp++;
  /* RFC 9460 says it must be uncompressed */
  যদি(clen > 63)
    ঘূৰাই CURLE_WEIRD_SERVER_REPLY;

  যদি(clen == 0) {
    /* special case - return "." as name */
    যদি(curlx_dyn_addn(&thename, ".", 1))
      ঘূৰাই CURLE_OUT_OF_MEMORY;
  }
  যেতিয়ালৈ(clen) {
    যদি(clen >= rem) {
      curlx_dyn_free(&thename);
      ঘূৰাই CURLE_OUT_OF_MEMORY;
    }
    যদি(curlx_dyn_addn(&thename, cp, clen) ||
       curlx_dyn_addn(&thename, ".", 1))
      ঘূৰাই CURLE_TOO_LARGE;

    cp += clen;
    rem -= (clen + 1);
    যদি(rem <= 0) {
      curlx_dyn_free(&thename);
      ঘূৰাই CURLE_OUT_OF_MEMORY;
    }
    clen = *cp++;
    যদি(clen > 63) {
      /* invalid format */
      curlx_dyn_free(&thename);
      ঘূৰাই CURLE_WEIRD_SERVER_REPLY;
    }
  }
  *buf = cp;
  *remaining = rem - 1;
  *dnsname = curlx_dyn_ptr(&thename);
  ঘূৰাই CURLE_OK;
}

/* scan for byte values <= 32 or 127 */
স্থির CURLcode junkscan(ধ্রুৱ ৱর্ণ *url)
{
  ধ্রুৱ unsigned ৱর্ণ *p = (ধ্রুৱ unsigned ৱর্ণ *)url;
  যেতিয়ালৈ(*p) {
    যদি(*p <= 0x20 || *p == 127)
      ঘূৰাই CURLE_WEIRD_SERVER_REPLY;
    p++;
  }
  ঘূৰাই CURLE_OK;
}

/* @unittest 1658 */
UNITTEST CURLcode doh_resp_decode_httpsrr(সংরচনা Curl_easy *data,
                                          ধ্রুৱ unsigned ৱর্ণ *cp, size_t len,
                                          সংরচনা Curl_https_rrinfo **hrr);
UNITTEST CURLcode doh_resp_decode_httpsrr(সংরচনা Curl_easy *data,
                                          ধ্রুৱ unsigned ৱর্ণ *cp, size_t len,
                                          সংরচনা Curl_https_rrinfo **hrr)
{
  uint16_t pcode = 0, plen = 0;
  uint32_t expected_min_pcode = 0;
  সংরচনা Curl_https_rrinfo *lhrr = রিক্ত;
  ৱর্ণ *dnsname = রিক্ত;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (শূন্য)data;
  *hrr = রিক্ত;
  যদি(len <= 2)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  lhrr = curlx_calloc(1, পরিমাণ(সংরচনা Curl_https_rrinfo));
  যদি(!lhrr)
    ঘূৰাই CURLE_OUT_OF_MEMORY;
  lhrr->priority = doh_get16bit(cp, 0);
  cp += 2;
  len -= 2;
  যদি(doh_decode_rdata_name(&cp, &len, &dnsname) != CURLE_OK)
    গচ্ছ err;
  lhrr->target = dnsname;
  result = junkscan(dnsname);
  যদি(result) {
    /* unacceptable hostname content */
    গচ্ছ err;
  }
  যেতিয়ালৈ(len >= 4) {
    pcode = doh_get16bit(cp, 0);
    plen = doh_get16bit(cp, 2);
    cp += 4;
    len -= 4;
    যদি(pcode < expected_min_pcode || plen > len) {
      result = CURLE_WEIRD_SERVER_REPLY;
      গচ্ছ err;
    }
    result = Curl_httpsrr_set(lhrr, pcode, cp, plen);
    যদি(result)
      গচ্ছ err;
    Curl_httpsrr_trace(data, lhrr);
    cp += plen;
    len -= plen;
    expected_min_pcode = pcode + 1;
  }
  *hrr = lhrr;
  ঘূৰাই CURLE_OK;
err:
  Curl_httpsrr_destroy(lhrr);
  ঘূৰাই result;
}

#endif /* USE_HTTPSRR */

/* called from multi when a sub transfer, e.g. doh probe, is done.
 * Parse the response and set the results in the `async` context
 * of master, using the id from the probe's CURL_EZM_DOH_PROBE
 * meta data. */
স্থির শূন্য doh_probe_done(সংরচনা Curl_easy *doh,
                           সংরচনা Curl_easy *master, CURLcode result)
{
  সংরচনা Curl_resolv_async *async = রিক্ত;
  সংরচনা doh_probes *dohp = রিক্ত;
  সংরচনা doh_request *doh_req = রিক্ত;
  সংরচনা Curl_addrinfo **pdest_ai;
  সংরচনা dohentry de;
  পূর্ণাঙ্ক slot, httpcode;

  de_init(&de);
  doh_req = Curl_meta_get(doh, CURL_EZM_DOH_PROBE);
  যদি(!doh_req) {
    /* transfer `doh` is not a DoH probe. */
    DEBUGASSERT(0);
    গচ্ছ out;
  }

  async = Curl_async_get(master, doh_req->resolv_id);
  যদি(!async) {
    CURL_TRC_DNS(master, "[%u] ignoring outdated DoH response",
                 doh_req->resolv_id);
    গচ্ছ out;
  }
  dohp = async->doh;

  বাবে(slot = 0; slot < DOH_SLOT_গুনতি; ++slot) {
    যদি(dohp->probe_mid[slot] == doh->mid)
      ভঙ্গ;
  }
  /* We really should have found the slot where to store the response */
  যদি(slot >= DOH_SLOT_গুনতি) {
    failf(master, "DoH: unknown sub request done");
    DEBUGASSERT(0);
    গচ্ছ out;
  }

  async->queries_ongoing--;
  dohp = async->doh;
  httpcode = doh->info.httpcode;
  ৱিকল্প(slot) {
  পক্ষ DOH_SLOT_IPV4:
    async->dns_responses |= CURL_DNSQ_A;
    ভঙ্গ;
#ifdef USE_IPV6
  পক্ষ DOH_SLOT_IPV6:
    async->dns_responses |= CURL_DNSQ_AAAA;
    ভঙ্গ;
#endif
#ifdef USE_HTTPSRR
  পক্ষ DOH_SLOT_HTTPS_RR:
    async->dns_responses |= CURL_DNSQ_HTTPS;
    ভঙ্গ;
#endif
  স্ৱভাৱ:
    DEBUGASSERT(0);
    ভঙ্গ;
  }

  যদি(result) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: %s",
          doh_type2name(doh_req->dnstype), curl_easy_strerror(result));
    গচ্ছ out;
  }
  অন্যথা যদি((httpcode < 200) || (httpcode >= 300)) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: HTTP status %d",
          doh_type2name(doh_req->dnstype), httpcode);
    গচ্ছ out;
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
    গচ্ছ out;
  }

  যদি(doh_req->dnstype == CURL_DNS_TYPE_A)
    pdest_ai = &async->ai_A;
  অন্যথা যদি(doh_req->dnstype == CURL_DNS_TYPE_AAAA)
    pdest_ai = &async->ai_AAAA;
  অন্যথা
    pdest_ai = রিক্ত;

  যদি(pdest_ai && de.numaddr) {
    যদি(*pdest_ai) {
      Curl_freeaddrinfo(*pdest_ai);
      *pdest_ai = রিক্ত;
    }
    result = doh2ai(&de, async->peer->hostname, async->peer->port, pdest_ai);
    যদি(result) { /* hard failure on our side, fail completely */
      infof(doh, "[DoH] [%s] error creating addrinfo: %s",
            doh_type2name(doh_req->dnstype), curl_easy_strerror(result));
      dohp->probe_rc[slot] = DOH_OOM;
      async->result = result;
    }
  }
#ifdef USE_HTTPSRR
  অন্যথা যদি((doh_req->dnstype == CURL_DNS_TYPE_HTTPS) && de.numhttps_rrs) {
    CURL_TRC_DNS(doh, "[HTTPS] got %d records", de.numhttps_rrs);
    result = doh_resp_decode_httpsrr(doh, de.https_rrs->val,
                                     de.https_rrs->len, &async->httpsrr);
    যদি(result) {
      dohp->probe_rc[slot] = DOH_HTTP_FAILED;
      infof(doh, "[DoH] error decoding HTTPS RR: %s",
            curl_easy_strerror(result));
      গচ্ছ out;
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

CURLcode Curl_doh_take_result(সংরচনা Curl_easy *data,
                              সংরচনা Curl_resolv_async *async,
                              সংরচনা Curl_dns_entry **pdns)
{
  সংরচনা doh_probes *dohp = async->doh;
  CURLcode result = CURLE_OK;

  *pdns = রিক্ত; /* defaults to no response */
  যদি(!dohp)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  async->negative_answer = FALSE;
  যদি(async->result) {
    result = async->result;
    গচ্ছ out;
  }

  যদি(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     dohp->probe_mid[DOH_SLOT_IPV4] == UINT32_MAX &&
     dohp->probe_mid[DOH_SLOT_IPV6] == UINT32_MAX) {
    failf(data, "Could not DoH-resolve: %s", async->peer->hostname);
    ঘূৰাই async->for_proxy ?
      CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
  }
  অন্যথা যদি(!async->queries_ongoing) {
    সংরচনা Curl_dns_entry *dns = রিক্ত;
    bool negative = TRUE;
    পূর্ণাঙ্ক slot;

    /* remove DoH handles from multi handle and close them */
    doh_close(data, async);
    /* parse the responses, create the struct and return it! */
    বাবে(slot = 0; slot < DOH_SLOT_গুনতি; slot++) {
      /* Failing without an NXDOMAIN answer - a SERVFAIL-class rcode or
         an undecodable response - says nothing about the name. Such a
         failure must not be cached as a negative entry. */
      যদি(dohp->probe_rc[slot] && (dohp->probe_rc[slot] != DOH_DNS_NXDOMAIN))
        negative = FALSE;
    } /* next slot */

    যদি(async->ai_A || async->ai_AAAA) {
      dns = Curl_dnsc_mk_addr2(
        data, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
      যদি(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        গচ্ছ out;
      }
    }
#ifdef USE_HTTPSRR
    অন্যথা যদি((async->dns_queries & CURL_DNSQ_HTTPS) &&
            !dohp->probe_rc[DOH_SLOT_HTTPS_RR]) {
      Curl_httpsrr_trace(data, async->httpsrr);
      dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
      যদি(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        গচ্ছ out;
      }
    }
#endif /* USE_HTTPSRR */
    অন্যথা {
      /* every query failed. Only NXDOMAIN answers for all of them
         make this a negative answer, eligible for caching. */
      async->negative_answer = negative;
      result = async->for_proxy ?
        CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
    }

    /* and add the entry to the cache */
    যদি(dns)
      result = Curl_dnscache_add(data, dns);
    *pdns = dns;
  } /* !async->queries_ongoing */
  অন্যথা
    /* wait for pending DoH transactions to complete */
    ঘূৰাই CURLE_AGAIN;

out:
  Curl_doh_cleanup(data, async);
  ঘূৰাই result;
}

স্থির শূন্য doh_close(সংরচনা Curl_easy *data,
                      সংরচনা Curl_resolv_async *async)
{
  সংরচনা doh_probes *doh = async ? async->doh : রিক্ত;
  যদি(doh && data->multi) {
    সংরচনা Curl_easy *probe_data;
    uint32_t mid;
    size_t slot;
    বাবে(slot = 0; slot < DOH_SLOT_গুনতি; slot++) {
      mid = doh->probe_mid[slot];
      যদি(mid == UINT32_MAX)
        অনুৱর্ত;
      doh->probe_mid[slot] = UINT32_MAX;
      /* should have been called before data is removed from multi handle */
      DEBUGASSERT(data->multi);
      probe_data = data->multi ? Curl_multi_get_easy(data->multi, mid) : রিক্ত;
      যদি(!probe_data) {
        DEBUGF(infof(data, "Curl_doh_close: xfer for mid=%u not found!", mid));
        অনুৱর্ত;
      }
      probe_data->sub_xfer_done = রিক্ত; /* No longer interested in result */
      /* data->multi might already be reset at this time */
      Curl_multi_remove_handle(data->multi, probe_data);
      Curl_close(&probe_data);
    }
    CURL_TRC_DNS(data, "[DoH] probe done");
  }
}

শূন্য Curl_doh_cleanup(সংরচনা Curl_easy *data,
                      সংরচনা Curl_resolv_async *async)
{
  সংরচনা doh_probes *dohp = async->doh;
  যদি(dohp) {
    doh_close(data, async);
    curlx_safefree(async->doh);
  }
}

#endif /* CURL_DISABLE_DOH */
