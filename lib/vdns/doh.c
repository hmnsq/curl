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
#شامل "curl_setup.h"

#ifndef CURL_DISABLE_DOH

#شامل "urldata.h"
#شامل "curl_addrinfo.h"
#شامل "curl_trc.h"
#شامل "multiif.h"
#شامل "url.h"
#شامل "connect.h"
#شامل "vdns/doh.h"
#شامل "vdns/httpsrr.h"
#شامل "curlx/strdup.h"
#شامل "curlx/dynbuf.h"
#شامل "escape.h"  /* for Curl_hexencode() */
#شامل "urlapi-int.h"

#define DNS_CLASS_IN 0x01

ایستا تهی doh_close(ساختار Curl_easy *data,
                      ساختار Curl_resolv_async *async);

#ifdef CURLVERBOSE
ایستا ثابت نویسه * ثابت doh_code_str[] = {
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

ایستا ثابت نویسه *doh_strerror(DOHcode code)
{
  اگر((size_t)code < CURL_ARRAYSIZE(doh_code_str))
    بازگشت doh_code_str[code];
  بازگشت "bad error code";
}

ایستا ثابت نویسه *doh_type2name(DNStype dnstype)
{
  انتخاب(dnstype) {
  حالت CURL_DNS_TYPE_A:
    بازگشت "A";
  حالت CURL_DNS_TYPE_AAAA:
    بازگشت "AAAA";
#ifdef USE_HTTPSRR
  حالت CURL_DNS_TYPE_HTTPS:
    بازگشت "HTTPS";
#endif
  پیشفرض:
    بازگشت "unknown";
  }
}

#endif /* CURLVERBOSE */

/* @unittest 1655
 */
UNITTEST DOHcode doh_req_encode(ثابت نویسه *host,
                                DNStype dnstype,
                                unsigned نویسه *dnsp,  /* buffer */
                                size_t len,  /* buffer size */
                                size_t *olen);  /* output length */
UNITTEST DOHcode doh_req_encode(ثابت نویسه *host,
                                DNStype dnstype,
                                unsigned نویسه *dnsp, /* buffer */
                                size_t len,   /* buffer size */
                                size_t *olen) /* output length */
{
  ثابت size_t hostlen = strlen(host);
  unsigned نویسه *orig = dnsp;
  ثابت نویسه *hostp = host;

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
  اگر(host[hostlen - 1] != '.')
    expected_len++;

  اگر(expected_len > DOH_MAX_DNSREQ_SIZE)
    بازگشت DOH_DNS_NAME_TOO_LONG;

  اگر(len < expected_len)
    بازگشت DOH_TOO_SMALL_BUFFER;

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
  وقتی(*hostp) {
    size_t labellen;
    ثابت نویسه *dot = strchr(hostp, '.');
    اگر(dot)
      labellen = dot - hostp;
    وگرنه
      labellen = strlen(hostp);
    اگر((labellen > 63) || (!labellen)) {
      /* label is too long or too short, error out */
      *olen = 0;
      بازگشت DOH_DNS_BAD_LABEL;
    }
    /* label is non-empty, process it */
    *dnsp++ = (unsigned نویسه)labellen;
    memcpy(dnsp, hostp, labellen);
    dnsp += labellen;
    hostp += labellen;
    /* advance past dot, but only if there is one */
    اگر(dot)
      hostp++;
  } /* next label */

  *dnsp++ = 0; /* append zero-length label for root */

  /* There are assigned TYPE codes beyond 255: use range [1..65535] */
  *dnsp++ = (unsigned نویسه)(255 & (dnstype >> 8)); /* upper 8-bit TYPE */
  *dnsp++ = (unsigned نویسه)(255 & dnstype);        /* lower 8-bit TYPE */

  *dnsp++ = '\0'; /* upper 8-bit CLASS */
  *dnsp++ = DNS_CLASS_IN; /* IN - "the Internet" */

  *olen = dnsp - orig;

  /* verify that our estimation of length is valid, since
   * this has led to buffer overflows in this function */
  DEBUGASSERT(*olen == expected_len);
  بازگشت DOH_OK;
}

ایستا size_t doh_probe_write_cb(نویسه *contents, size_t size, size_t nmemb,
                                 تهی *userp)
{
  size_t realsize = size * nmemb;
  ساختار Curl_easy *data = userp;
  ساختار doh_request *doh_req = Curl_meta_get(data, CURL_EZM_DOH_PROBE);
  اگر(!doh_req)
    بازگشت CURL_WRITEFUNC_ERROR;

  اگر(curlx_dyn_addn(&doh_req->resp_body, contents, realsize))
    بازگشت 0;

  بازگشت realsize;
}

ایستا تهی doh_probe_done(ساختار Curl_easy *doh,
                           ساختار Curl_easy *master, CURLcode result);
ایستا تهی doh_probe_dtor(ثابت تهی *کلید, size_t klen, تهی *e)
{
  (تهی)کلید;
  (تهی)klen;
  اگر(e) {
    ساختار doh_request *doh_req = e;
    curl_slist_free_all(doh_req->req_hds);
    curlx_dyn_free(&doh_req->resp_body);
    curlx_free(e);
  }
}

#define ERROR_CHECK_SETOPT(x, y)                        \
  انجامبده {                                                  \
    result = curl_easy_setopt((CURL *)doh, x, y);       \
    اگر(result &&                                        \
       result != CURLE_NOT_BUILT_IN &&                  \
       result != CURLE_UNKNOWN_OPTION)                  \
      برو error;                                       \
  } وقتی(0)

ایستا CURLcode doh_probe_run(ساختار Curl_easy *data,
                              DNStype dnstype,
                              ثابت نویسه *host,
                              ثابت نویسه *url, CURLM *multi,
                              uint32_t resolv_id,
                              uint32_t *pmid)
{
  ساختار Curl_easy *doh = پوچ;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  ساختار doh_request *doh_req;
  DOHcode d;
  bool maybe_https = !curl_strnequal(url, STRCONST("http:"));

  *pmid = UINT32_MAX;

  doh_req = curlx_calloc(1, اندازه(*doh_req));
  اگر(!doh_req)
    بازگشت CURLE_OUT_OF_MEMORY;
  doh_req->resolv_id = resolv_id;
  doh_req->dnstype = dnstype;
  curlx_dyn_init(&doh_req->resp_body, DYN_DOH_RESPONSE);

  d = doh_req_encode(host, dnstype, doh_req->req_body,
                     اندازه(doh_req->req_body),
                     &doh_req->req_body_len);
  اگر(d) {
    failf(data, "Failed to encode DoH packet [%d]", (صحیح)d);
    result = CURLE_OUT_OF_MEMORY;
    برو error;
  }

  timeout_ms = Curl_timeleft_ms(data);
  اگر(timeout_ms < 0) {
    result = CURLE_OPERATION_TIMEDOUT;
    برو error;
  }

  doh_req->req_hds =
    curl_slist_append(پوچ, "Content-Type: application/dns-message");
  اگر(!doh_req->req_hds) {
    result = CURLE_OUT_OF_MEMORY;
    برو error;
  }

  /* Curl_open() is the internal version of curl_easy_init() */
  result = Curl_open(&doh);
  اگر(result)
    برو error;

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
  اگر(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);
    ERROR_CHECK_SETOPT(CURLOPT_PIPEWAIT, 1L);
  }
#endif
#ifndef DEBUGBUILD
  /* enforce HTTPS if not debug */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
#وگرنه
  /* in debug mode, also allow http */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
#endif
  ERROR_CHECK_SETOPT(CURLOPT_TIMEOUT_MS, (long)timeout_ms);
  ERROR_CHECK_SETOPT(CURLOPT_SHARE, (CURLSH *)data->share);
  اگر(data->set.err && data->set.err != stderr)
    ERROR_CHECK_SETOPT(CURLOPT_STDERR, data->set.err);
  اگر(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_doh))
    ERROR_CHECK_SETOPT(CURLOPT_VERBOSE, 1L);
  اگر(data->set.no_signal)
    ERROR_CHECK_SETOPT(CURLOPT_NOSIGNAL, 1L);
  اگر(data->set.fdebug)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGFUNCTION, data->set.fdebug);
  اگر(data->set.debugdata)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGDATA, data->set.debugdata);

#ifdef USE_RESOLV_ARES
  اگر(CURL_EASY_STR(data, STRING_DNS_SERVERS)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_SERVERS,
                       CURL_EASY_STR(data, STRING_DNS_SERVERS));
  }
  اگر(CURL_EASY_STR(data, STRING_DNS_INTERFACE)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_INTERFACE,
                       CURL_EASY_STR(data, STRING_DNS_INTERFACE));
  }
  اگر(CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP4,
                       CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4));
  }
  اگر(CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP6,
                       CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6));
  }
#endif
  اگر(maybe_https) {
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
    اگر(CURL_EASY_STR(data, STRING_SSL_CAFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO,
                         CURL_EASY_STR(data, STRING_SSL_CAFILE));
    }
    اگر(data->set.blobs[BLOB_CAINFO]) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO_BLOB, data->set.blobs[BLOB_CAINFO]);
    }
    اگر(CURL_EASY_STR(data, STRING_SSL_CAPATH)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAPATH,
                         CURL_EASY_STR(data, STRING_SSL_CAPATH));
    }
    اگر(CURL_EASY_STR(data, STRING_SSL_CRLFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CRLFILE,
                         CURL_EASY_STR(data, STRING_SSL_CRLFILE));
    }
    اگر(data->set.ssl.certinfo)
      ERROR_CHECK_SETOPT(CURLOPT_CERTINFO, 1L);
    اگر(data->set.ssl_fsslctx)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_FUNCTION, data->set.ssl_fsslctx);
    اگر(data->set.ssl_fsslctxp)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_DATA, data->set.ssl_fsslctxp);
    اگر(CURL_EASY_STR(data, STRING_SSL_EC_CURVES)) {
      ERROR_CHECK_SETOPT(CURLOPT_SSL_EC_CURVES,
                         CURL_EASY_STR(data, STRING_SSL_EC_CURVES));
    }
    (تهی)curl_easy_setopt(doh, CURLOPT_SSL_OPTIONS,
                           ((long)data->set.ssl.ssl_options &
                            ~CURLSSLOPT_AUTO_CLIENT_CERT));
  }

  doh->state.internal = TRUE;
  doh->master_mid = data->mid; /* master transfer of this one */
  doh->sub_xfer_done = doh_probe_done;

  result = Curl_meta_set(doh, CURL_EZM_DOH_PROBE, doh_req, doh_probe_dtor);
  doh_req = پوچ; /* call took ownership */
  اگر(result)
    برو error;

  /* DoH handles must not inherit private_data. The handles may be passed to
     the user via callbacks and the user will be able to identify them as
     internal handles because private data is not set. The user can then set
     private_data via CURLOPT_PRIVATE if they so choose. */
  DEBUGASSERT(!doh->set.private_data);

  اگر(Curl_multi_add_handle(multi, doh))
    برو error;

  *pmid = doh->mid;
  بازگشت CURLE_OK;

error:
  Curl_close(&doh);
  اگر(doh_req)
    doh_probe_dtor(پوچ, 0, doh_req);
  بازگشت result;
}

/*
 * Curl_doh() starts a name resolve using DoH. It resolves a name and returns
 * a 'Curl_addrinfo *' with the address information.
 */

CURLcode Curl_doh(ساختار Curl_easy *data,
                  ساختار Curl_resolv_async *async)
{
  CURLcode result = CURLE_OK;
  ساختار doh_probes *dohp = پوچ;
  size_t i;

  DEBUGASSERT(!async->doh);
  DEBUGASSERT(async->peer->hostname[0]);
  اگر(async->doh) {
    DEBUGASSERT(0); /* should not happen */
    Curl_doh_cleanup(data, async);
  }

  اگر(!async->dns_queries)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef USE_HTTPSRR
  اگر(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     (async->dns_queries & CURL_DNSQ_HTTPS)) {
    /* Can't mix those in the same async resolve */
    DEBUGASSERT(0);
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  }
#وگرنه
  اگر(async->dns_queries & CURL_DNSQ_HTTPS) {
    DEBUGASSERT(0);
    بازگشت CURLE_NOT_BUILT_IN;
  }
#endif

  /* start clean, consider allocating this struct on demand */
  async->doh = dohp = curlx_calloc(1, اندازه(ساختار doh_probes));
  اگر(!dohp)
    بازگشت CURLE_OUT_OF_MEMORY;

  برای(i = 0; i < DOH_SLOT_شمار; ++i) {
    dohp->probe_rc[i] = DOH_OK;
    dohp->probe_mid[i] = UINT32_MAX;
  }

#ifdef USE_IPV6
  /* AAAA results have preference in happy eyeballing, trigger first */
  اگر(async->dns_queries & CURL_DNSQ_AAAA) {
    /* create IPv6 DoH request */
    result = doh_probe_run(data, CURL_DNS_TYPE_AAAA,
                           async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH),
                           data->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV6]);
    اگر(result)
      برو error;
    async->queries_ongoing++;
  }
#endif

  /* create IPv4 DoH request */
  اگر(async->dns_queries & CURL_DNSQ_A) {
    result = doh_probe_run(data, CURL_DNS_TYPE_A,
                           async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH),
                           data->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV4]);
    اگر(result)
      برو error;
    async->queries_ongoing++;
  }

#ifdef USE_HTTPSRR
  اگر(async->dns_queries & CURL_DNSQ_HTTPS) {
    نویسه *qname = پوچ;
    اگر(async->peer->port != PORT_HTTPS) {
      qname = curl_maprintf("_%u._https.%s",
                            async->peer->port, async->peer->hostname);
      اگر(!qname)
        برو error;
    }
    result = doh_probe_run(data, CURL_DNS_TYPE_HTTPS,
                           qname ? qname : async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH), data->multi,
                           async->id,
                           &dohp->probe_mid[DOH_SLOT_HTTPS_RR]);
    curlx_free(qname);
    اگر(result)
      برو error;
    async->queries_ongoing++;
  }
#endif
  بازگشت CURLE_OK;

error:
  Curl_doh_cleanup(data, async);
  بازگشت result;
}

ایستا DOHcode doh_skipqname(ثابت unsigned نویسه *doh, size_t dohlen,
                             unsigned صحیح *indexp)
{
  unsigned نویسه length;
  انجامبده {
    اگر(dohlen < (*indexp + 1))
      بازگشت DOH_DNS_OUT_OF_RANGE;
    length = doh[*indexp];
    اگر((length & 0xc0) == 0xc0) {
      /* name pointer, advance over it and be done */
      اگر(dohlen < (*indexp + 2))
        بازگشت DOH_DNS_OUT_OF_RANGE;
      *indexp += 2;
      بشکن;
    }
    اگر(length & 0xc0)
      بازگشت DOH_DNS_BAD_LABEL;
    اگر(dohlen < (*indexp + 1 + length))
      بازگشت DOH_DNS_OUT_OF_RANGE;
    *indexp += (unsigned صحیح)(1 + length);
  } وقتی(length);
  بازگشت DOH_OK;
}

ایستا unsigned short doh_get16bit(ثابت unsigned نویسه *doh,
                                   unsigned صحیح index)
{
  بازگشت (unsigned short)((doh[index] << 8) | doh[index + 1]);
}

ایستا unsigned صحیح doh_get32bit(ثابت unsigned نویسه *doh, unsigned صحیح index)
{
  /* make clang and gcc optimize this to bswap by incrementing
     the pointer first. */
  doh += index;

  /* avoid undefined behavior by casting to unsigned before shifting
     24 bits, possibly into the sign bit. codegen is same, but
     ub sanitizer will not be upset */
  بازگشت ((unsigned)doh[0] << 24) | ((unsigned)doh[1] << 16) |
         ((unsigned)doh[2] << 8) | doh[3];
}

ایستا تهی doh_store_a(ثابت unsigned نویسه *doh, صحیح index,
                        ساختار dohentry *d)
{
  /* silently ignore addresses over the limit */
  اگر(d->numaddr < DOH_MAX_ADDR) {
    ساختار dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_A;
    memcpy(&a->ip.v4, &doh[index], 4);
    d->numaddr++;
  }
}

ایستا تهی doh_store_aaaa(ثابت unsigned نویسه *doh, صحیح index,
                           ساختار dohentry *d)
{
  /* silently ignore addresses over the limit */
  اگر(d->numaddr < DOH_MAX_ADDR) {
    ساختار dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_AAAA;
    memcpy(&a->ip.v6, &doh[index], 16);
    d->numaddr++;
  }
}

#ifdef USE_HTTPSRR
ایستا DOHcode doh_store_https(ثابت unsigned نویسه *doh, صحیح index,
                               ساختار dohentry *d, uint16_t len)
{
  /* silently ignore RRs over the limit */
  اگر(d->numhttps_rrs < DOH_MAX_HTTPS) {
    ساختار dohhttps_rr *h = &d->https_rrs[d->numhttps_rrs];
    h->val = curlx_memdup(&doh[index], len);
    اگر(!h->val)
      بازگشت DOH_OUT_OF_MEM;
    h->len = len;
    d->numhttps_rrs++;
  }
  بازگشت DOH_OK;
}
#endif

ایستا DOHcode doh_rdata(ثابت unsigned نویسه *doh,
                         unsigned short rdlength,
                         unsigned short type,
                         صحیح index,
                         ساختار dohentry *d)
{
  /* RDATA
     - A (TYPE 1): 4 bytes
     - AAAA (TYPE 28): 16 bytes
     - HTTPS (TYPE 65): N bytes */
  انتخاب(type) {
  حالت CURL_DNS_TYPE_A:
    اگر(rdlength != 4)
      بازگشت DOH_DNS_RDATA_LEN;
    doh_store_a(doh, index, d);
    بشکن;
  حالت CURL_DNS_TYPE_AAAA:
    اگر(rdlength != 16)
      بازگشت DOH_DNS_RDATA_LEN;
    doh_store_aaaa(doh, index, d);
    بشکن;
#ifdef USE_HTTPSRR
  حالت CURL_DNS_TYPE_HTTPS:
    اگر(rdlength < 3)
      بازگشت DOH_DNS_RDATA_LEN;
    بازگشت doh_store_https(doh, index, d, rdlength);
#endif
  پیشفرض:
    /* unsupported type, or type we do not store, skip it */
    بشکن;
  }
  بازگشت DOH_OK;
}

/* @unittest 1655 */
UNITTEST تهی de_init(ساختار dohentry *de);
UNITTEST تهی de_init(ساختار dohentry *de)
{
  memset(de, 0, اندازه(*de));
  de->ttl = INT_MAX;
}

/* TTL value cap */
#define MAX_DNS_TTL 86400U /* 24 hours */
/* @unittest 1650 */
UNITTEST DOHcode doh_resp_decode(ثابت unsigned نویسه *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 ساختار dohentry *d);
UNITTEST DOHcode doh_resp_decode(ثابت unsigned نویسه *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 ساختار dohentry *d)
{
  unsigned نویسه rcode;
  unsigned short qdcount;
  unsigned short ancount;
  unsigned short type = 0;
  unsigned short rdlength;
  unsigned short nscount;
  unsigned short arcount;
  unsigned صحیح index = 12;
  DOHcode rc;

  اگر(dohlen < 12)
    بازگشت DOH_TOO_SMALL_BUFFER; /* too small */
  اگر(!doh || doh[0] || doh[1])
    بازگشت DOH_DNS_BAD_ID; /* bad ID */
  rcode = doh[3] & 0x0f;
  اگر(rcode == 3)
    بازگشت DOH_DNS_NXDOMAIN; /* name does not exist */
  اگر(rcode)
    بازگشت DOH_DNS_BAD_RCODE; /* bad rcode */

  qdcount = doh_get16bit(doh, 4);
  وقتی(qdcount) {
    rc = doh_skipqname(doh, dohlen, &index);
    اگر(rc)
      بازگشت rc; /* bad qname */
    اگر(dohlen < (index + 4))
      بازگشت DOH_DNS_OUT_OF_RANGE;
    index += 4; /* skip question's type and class */
    qdcount--;
  }

  ancount = doh_get16bit(doh, 6);
  وقتی(ancount) {
    unsigned short dnsclass;
    unsigned صحیح ttl;

    rc = doh_skipqname(doh, dohlen, &index);
    اگر(rc)
      بازگشت rc; /* bad qname */

    اگر(dohlen < (index + 2))
      بازگشت DOH_DNS_OUT_OF_RANGE;

    type = doh_get16bit(doh, index);
    اگر((type != CURL_DNS_TYPE_CNAME) &&  /* may be synthesized from DNAME */
       (type != CURL_DNS_TYPE_DNAME) &&  /* if present, accept and ignore */
       (type != dnstype))
      /* Not the same type as was asked for, nor CNAME nor DNAME */
      بازگشت DOH_DNS_UNEXPECTED_TYPE;
    index += 2;

    اگر(dohlen < (index + 2))
      بازگشت DOH_DNS_OUT_OF_RANGE;
    dnsclass = doh_get16bit(doh, index);
    اگر(DNS_CLASS_IN != dnsclass)
      بازگشت DOH_DNS_UNEXPECTED_CLASS; /* unsupported */
    index += 2;

    اگر(dohlen < (index + 4))
      بازگشت DOH_DNS_OUT_OF_RANGE;

    ttl = doh_get32bit(doh, index);
    اگر(ttl > MAX_DNS_TTL)
      ttl = MAX_DNS_TTL;
    اگر(ttl < d->ttl)
      d->ttl = ttl;
    index += 4;

    اگر(dohlen < (index + 2))
      بازگشت DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    اگر(dohlen < (index + rdlength))
      بازگشت DOH_DNS_OUT_OF_RANGE;

    rc = doh_rdata(doh, rdlength, type, (صحیح)index, d);
    اگر(rc)
      بازگشت rc;
    index += rdlength;
    ancount--;
  }

  nscount = doh_get16bit(doh, 8);
  وقتی(nscount) {
    rc = doh_skipqname(doh, dohlen, &index);
    اگر(rc)
      بازگشت rc; /* bad qname */

    اگر(dohlen < (index + 8))
      بازگشت DOH_DNS_OUT_OF_RANGE;

    index += 2 + 2 + 4; /* type, dnsclass and ttl */

    اگر(dohlen < (index + 2))
      بازگشت DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    اگر(dohlen < (index + rdlength))
      بازگشت DOH_DNS_OUT_OF_RANGE;
    index += rdlength;
    nscount--;
  }

  arcount = doh_get16bit(doh, 10);
  وقتی(arcount) {
    rc = doh_skipqname(doh, dohlen, &index);
    اگر(rc)
      بازگشت rc; /* bad qname */

    اگر(dohlen < (index + 8))
      بازگشت DOH_DNS_OUT_OF_RANGE;

    index += 2 + 2 + 4; /* type, dnsclass and ttl */

    اگر(dohlen < (index + 2))
      بازگشت DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    اگر(dohlen < (index + rdlength))
      بازگشت DOH_DNS_OUT_OF_RANGE;
    index += rdlength;
    arcount--;
  }

  اگر(index != dohlen)
    بازگشت DOH_DNS_MALFORMAT; /* something is wrong */

  بازگشت DOH_OK; /* ok */
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
ایستا CURLcode doh2ai(ثابت ساختار dohentry *de, ثابت نویسه *hostname,
                       صحیح port, ساختار Curl_addrinfo **aip)
{
  ساختار Curl_addrinfo *ai;
  ساختار Curl_addrinfo *prevai = پوچ;
  ساختار Curl_addrinfo *firstai = پوچ;
  ساختار sockaddr_in *addr;
#ifdef USE_IPV6
  ساختار sockaddr_in6 *addr6;
#endif
  size_t hostlen = strlen(hostname) + 1; /* include null-terminator */
  CURLcode result = CURLE_OK;
  صحیح i;

  برای(i = 0; i < de->numaddr; i++) {
    size_t ss_size;
    CURL_SA_FAMILY_T addrtype;
    اگر(de->addr[i].type == CURL_DNS_TYPE_AAAA) {
#ifndef USE_IPV6
      /* we cannot handle IPv6 addresses */
      ادامه;
#وگرنه
      ss_size = اندازه(ساختار sockaddr_in6);
      addrtype = AF_INET6;
#endif
    }
    وگرنه {
      ss_size = اندازه(ساختار sockaddr_in);
      addrtype = AF_INET;
    }

    ai = curlx_calloc(1, اندازه(ساختار Curl_addrinfo) + ss_size + hostlen);
    اگر(!ai) {
      result = CURLE_OUT_OF_MEMORY;
      بشکن;
    }
    ai->ai_addr = (تهی *)((نویسه *)ai + اندازه(ساختار Curl_addrinfo));
    ai->ai_canonname = (تهی *)((نویسه *)ai->ai_addr + ss_size);
    memcpy(ai->ai_canonname, hostname, hostlen);

    اگر(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    اگر(prevai)
      /* make the previous entry point to this */
      prevai->ai_next = ai;

    ai->ai_family = addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_size;

    /* leave the rest of the struct filled with zero */

    انتخاب(ai->ai_family) {
    حالت AF_INET:
      addr = (تهی *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(اندازه(ساختار in_addr) == اندازه(de->addr[i].ip.v4));
      memcpy(&addr->sin_addr, &de->addr[i].ip.v4, اندازه(ساختار in_addr));
      addr->sin_family = addrtype;
      addr->sin_port = htons((unsigned short)port);
      بشکن;

#ifdef USE_IPV6
    حالت AF_INET6:
      addr6 = (تهی *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(اندازه(ساختار in6_addr) == اندازه(de->addr[i].ip.v6));
      memcpy(&addr6->sin6_addr, &de->addr[i].ip.v6, اندازه(ساختار in6_addr));
      addr6->sin6_family = addrtype;
      addr6->sin6_port = htons((unsigned short)port);
      بشکن;
#endif
    }

    prevai = ai;
  }

  اگر(result) {
    Curl_freeaddrinfo(firstai);
    firstai = پوچ;
  }
  *aip = firstai;

  بازگشت result;
}

/* @unittest 1655 */
UNITTEST تهی de_cleanup(ساختار dohentry *d);
UNITTEST تهی de_cleanup(ساختار dohentry *d)
{
#ifdef USE_HTTPSRR
  صحیح i = 0;
  برای(i = 0; i < d->numhttps_rrs; i++)
    curlx_safefree(d->https_rrs[i].val);
#وگرنه
  (تهی)d;
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
ایستا CURLcode doh_decode_rdata_name(ثابت unsigned نویسه **buf,
                                      size_t *remaining, نویسه **dnsname)
{
  ثابت unsigned نویسه *cp = پوچ;
  size_t rem = 0;
  unsigned نویسه clen = 0; /* chunk len */
  ساختار dynbuf thename;

  DEBUGASSERT(buf && remaining && dnsname);
  اگر(!buf || !remaining || !dnsname || !*remaining)
    بازگشت CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&thename, CURL_MAXLEN_HOST_NAME);
  rem = *remaining;
  cp = *buf;
  clen = *cp++;
  /* RFC 9460 says it must be uncompressed */
  اگر(clen > 63)
    بازگشت CURLE_WEIRD_SERVER_REPLY;

  اگر(clen == 0) {
    /* special case - return "." as name */
    اگر(curlx_dyn_addn(&thename, ".", 1))
      بازگشت CURLE_OUT_OF_MEMORY;
  }
  وقتی(clen) {
    اگر(clen >= rem) {
      curlx_dyn_free(&thename);
      بازگشت CURLE_OUT_OF_MEMORY;
    }
    اگر(curlx_dyn_addn(&thename, cp, clen) ||
       curlx_dyn_addn(&thename, ".", 1))
      بازگشت CURLE_TOO_LARGE;

    cp += clen;
    rem -= (clen + 1);
    اگر(rem <= 0) {
      curlx_dyn_free(&thename);
      بازگشت CURLE_OUT_OF_MEMORY;
    }
    clen = *cp++;
    اگر(clen > 63) {
      /* invalid format */
      curlx_dyn_free(&thename);
      بازگشت CURLE_WEIRD_SERVER_REPLY;
    }
  }
  *buf = cp;
  *remaining = rem - 1;
  *dnsname = curlx_dyn_ptr(&thename);
  بازگشت CURLE_OK;
}

/* scan for byte values <= 32 or 127 */
ایستا CURLcode junkscan(ثابت نویسه *url)
{
  ثابت unsigned نویسه *p = (ثابت unsigned نویسه *)url;
  وقتی(*p) {
    اگر(*p <= 0x20 || *p == 127)
      بازگشت CURLE_WEIRD_SERVER_REPLY;
    p++;
  }
  بازگشت CURLE_OK;
}

/* @unittest 1658 */
UNITTEST CURLcode doh_resp_decode_httpsrr(ساختار Curl_easy *data,
                                          ثابت unsigned نویسه *cp, size_t len,
                                          ساختار Curl_https_rrinfo **hrr);
UNITTEST CURLcode doh_resp_decode_httpsrr(ساختار Curl_easy *data,
                                          ثابت unsigned نویسه *cp, size_t len,
                                          ساختار Curl_https_rrinfo **hrr)
{
  uint16_t pcode = 0, plen = 0;
  uint32_t expected_min_pcode = 0;
  ساختار Curl_https_rrinfo *lhrr = پوچ;
  نویسه *dnsname = پوچ;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (تهی)data;
  *hrr = پوچ;
  اگر(len <= 2)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  lhrr = curlx_calloc(1, اندازه(ساختار Curl_https_rrinfo));
  اگر(!lhrr)
    بازگشت CURLE_OUT_OF_MEMORY;
  lhrr->priority = doh_get16bit(cp, 0);
  cp += 2;
  len -= 2;
  اگر(doh_decode_rdata_name(&cp, &len, &dnsname) != CURLE_OK)
    برو err;
  lhrr->target = dnsname;
  result = junkscan(dnsname);
  اگر(result) {
    /* unacceptable hostname content */
    برو err;
  }
  وقتی(len >= 4) {
    pcode = doh_get16bit(cp, 0);
    plen = doh_get16bit(cp, 2);
    cp += 4;
    len -= 4;
    اگر(pcode < expected_min_pcode || plen > len) {
      result = CURLE_WEIRD_SERVER_REPLY;
      برو err;
    }
    result = Curl_httpsrr_set(lhrr, pcode, cp, plen);
    اگر(result)
      برو err;
    Curl_httpsrr_trace(data, lhrr);
    cp += plen;
    len -= plen;
    expected_min_pcode = pcode + 1;
  }
  *hrr = lhrr;
  بازگشت CURLE_OK;
err:
  Curl_httpsrr_destroy(lhrr);
  بازگشت result;
}

#endif /* USE_HTTPSRR */

/* called from multi when a sub transfer, e.g. doh probe, is done.
 * Parse the response and set the results in the `async` context
 * of master, using the id from the probe's CURL_EZM_DOH_PROBE
 * meta data. */
ایستا تهی doh_probe_done(ساختار Curl_easy *doh,
                           ساختار Curl_easy *master, CURLcode result)
{
  ساختار Curl_resolv_async *async = پوچ;
  ساختار doh_probes *dohp = پوچ;
  ساختار doh_request *doh_req = پوچ;
  ساختار Curl_addrinfo **pdest_ai;
  ساختار dohentry de;
  صحیح slot, httpcode;

  de_init(&de);
  doh_req = Curl_meta_get(doh, CURL_EZM_DOH_PROBE);
  اگر(!doh_req) {
    /* transfer `doh` is not a DoH probe. */
    DEBUGASSERT(0);
    برو out;
  }

  async = Curl_async_get(master, doh_req->resolv_id);
  اگر(!async) {
    CURL_TRC_DNS(master, "[%u] ignoring outdated DoH response",
                 doh_req->resolv_id);
    برو out;
  }
  dohp = async->doh;

  برای(slot = 0; slot < DOH_SLOT_شمار; ++slot) {
    اگر(dohp->probe_mid[slot] == doh->mid)
      بشکن;
  }
  /* We really should have found the slot where to store the response */
  اگر(slot >= DOH_SLOT_شمار) {
    failf(master, "DoH: unknown sub request done");
    DEBUGASSERT(0);
    برو out;
  }

  async->queries_ongoing--;
  dohp = async->doh;
  httpcode = doh->info.httpcode;
  انتخاب(slot) {
  حالت DOH_SLOT_IPV4:
    async->dns_responses |= CURL_DNSQ_A;
    بشکن;
#ifdef USE_IPV6
  حالت DOH_SLOT_IPV6:
    async->dns_responses |= CURL_DNSQ_AAAA;
    بشکن;
#endif
#ifdef USE_HTTPSRR
  حالت DOH_SLOT_HTTPS_RR:
    async->dns_responses |= CURL_DNSQ_HTTPS;
    بشکن;
#endif
  پیشفرض:
    DEBUGASSERT(0);
    بشکن;
  }

  اگر(result) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: %s",
          doh_type2name(doh_req->dnstype), curl_easy_strerror(result));
    برو out;
  }
  وگرنه اگر((httpcode < 200) || (httpcode >= 300)) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: HTTP status %d",
          doh_type2name(doh_req->dnstype), httpcode);
    برو out;
  }

  dohp->probe_rc[slot] = doh_resp_decode(curlx_dyn_uptr(&doh_req->resp_body),
                                         curlx_dyn_len(&doh_req->resp_body),
                                         doh_req->dnstype, &de);
  اگر(dohp->probe_rc[slot]) {
#ifdef USE_HTTPSRR
    اگر((dohp->probe_rc[slot] == DOH_NO_CONTENT) &&
       (doh_req->dnstype == CURL_DNS_TYPE_HTTPS)) {
      dohp->probe_rc[slot] = DOH_DNS_NXDOMAIN;
    }
#endif
    infof(doh, "[DoH] [%s] error decoding response: %s",
          doh_type2name(doh_req->dnstype),
          doh_strerror(dohp->probe_rc[slot]));
    برو out;
  }

  اگر(doh_req->dnstype == CURL_DNS_TYPE_A)
    pdest_ai = &async->ai_A;
  وگرنه اگر(doh_req->dnstype == CURL_DNS_TYPE_AAAA)
    pdest_ai = &async->ai_AAAA;
  وگرنه
    pdest_ai = پوچ;

  اگر(pdest_ai && de.numaddr) {
    اگر(*pdest_ai) {
      Curl_freeaddrinfo(*pdest_ai);
      *pdest_ai = پوچ;
    }
    result = doh2ai(&de, async->peer->hostname, async->peer->port, pdest_ai);
    اگر(result) { /* hard failure on our side, fail completely */
      infof(doh, "[DoH] [%s] error creating addrinfo: %s",
            doh_type2name(doh_req->dnstype), curl_easy_strerror(result));
      dohp->probe_rc[slot] = DOH_OOM;
      async->result = result;
    }
  }
#ifdef USE_HTTPSRR
  وگرنه اگر((doh_req->dnstype == CURL_DNS_TYPE_HTTPS) && de.numhttps_rrs) {
    CURL_TRC_DNS(doh, "[HTTPS] got %d records", de.numhttps_rrs);
    result = doh_resp_decode_httpsrr(doh, de.https_rrs->val,
                                     de.https_rrs->len, &async->httpsrr);
    اگر(result) {
      dohp->probe_rc[slot] = DOH_HTTP_FAILED;
      infof(doh, "[DoH] error decoding HTTPS RR: %s",
            curl_easy_strerror(result));
      برو out;
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

CURLcode Curl_doh_take_result(ساختار Curl_easy *data,
                              ساختار Curl_resolv_async *async,
                              ساختار Curl_dns_entry **pdns)
{
  ساختار doh_probes *dohp = async->doh;
  CURLcode result = CURLE_OK;

  *pdns = پوچ; /* defaults to no response */
  اگر(!dohp)
    بازگشت CURLE_OUT_OF_MEMORY;

  async->negative_answer = FALSE;
  اگر(async->result) {
    result = async->result;
    برو out;
  }

  اگر(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     dohp->probe_mid[DOH_SLOT_IPV4] == UINT32_MAX &&
     dohp->probe_mid[DOH_SLOT_IPV6] == UINT32_MAX) {
    failf(data, "Could not DoH-resolve: %s", async->peer->hostname);
    بازگشت async->for_proxy ?
      CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
  }
  وگرنه اگر(!async->queries_ongoing) {
    ساختار Curl_dns_entry *dns = پوچ;
    bool negative = TRUE;
    صحیح slot;

    /* remove DoH handles from multi handle and close them */
    doh_close(data, async);
    /* parse the responses, create the struct and return it! */
    برای(slot = 0; slot < DOH_SLOT_شمار; slot++) {
      /* Failing without an NXDOMAIN answer - a SERVFAIL-class rcode or
         an undecodable response - says nothing about the name. Such a
         failure must not be cached as a negative entry. */
      اگر(dohp->probe_rc[slot] && (dohp->probe_rc[slot] != DOH_DNS_NXDOMAIN))
        negative = FALSE;
    } /* next slot */

    اگر(async->ai_A || async->ai_AAAA) {
      dns = Curl_dnsc_mk_addr2(
        data, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
      اگر(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        برو out;
      }
    }
#ifdef USE_HTTPSRR
    وگرنه اگر((async->dns_queries & CURL_DNSQ_HTTPS) &&
            !dohp->probe_rc[DOH_SLOT_HTTPS_RR]) {
      Curl_httpsrr_trace(data, async->httpsrr);
      dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
      اگر(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        برو out;
      }
    }
#endif /* USE_HTTPSRR */
    وگرنه {
      /* every query failed. Only NXDOMAIN answers for all of them
         make this a negative answer, eligible for caching. */
      async->negative_answer = negative;
      result = async->for_proxy ?
        CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
    }

    /* and add the entry to the cache */
    اگر(dns)
      result = Curl_dnscache_add(data, dns);
    *pdns = dns;
  } /* !async->queries_ongoing */
  وگرنه
    /* wait for pending DoH transactions to complete */
    بازگشت CURLE_AGAIN;

out:
  Curl_doh_cleanup(data, async);
  بازگشت result;
}

ایستا تهی doh_close(ساختار Curl_easy *data,
                      ساختار Curl_resolv_async *async)
{
  ساختار doh_probes *doh = async ? async->doh : پوچ;
  اگر(doh && data->multi) {
    ساختار Curl_easy *probe_data;
    uint32_t mid;
    size_t slot;
    برای(slot = 0; slot < DOH_SLOT_شمار; slot++) {
      mid = doh->probe_mid[slot];
      اگر(mid == UINT32_MAX)
        ادامه;
      doh->probe_mid[slot] = UINT32_MAX;
      /* should have been called before data is removed from multi handle */
      DEBUGASSERT(data->multi);
      probe_data = data->multi ? Curl_multi_get_easy(data->multi, mid) : پوچ;
      اگر(!probe_data) {
        DEBUGF(infof(data, "Curl_doh_close: xfer for mid=%u not found!", mid));
        ادامه;
      }
      probe_data->sub_xfer_done = پوچ; /* No longer interested in result */
      /* data->multi might already be reset at this time */
      Curl_multi_remove_handle(data->multi, probe_data);
      Curl_close(&probe_data);
    }
    CURL_TRC_DNS(data, "[DoH] probe done");
  }
}

تهی Curl_doh_cleanup(ساختار Curl_easy *data,
                      ساختار Curl_resolv_async *async)
{
  ساختار doh_probes *dohp = async->doh;
  اگر(dohp) {
    doh_close(data, async);
    curlx_safefree(async->doh);
  }
}

#endif /* CURL_DISABLE_DOH */
