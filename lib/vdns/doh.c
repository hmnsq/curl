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
#અન્તર્ભાવ "curl_setup.h"

#ifndef CURL_DISABLE_DOH

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "curl_addrinfo.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "vdns/doh.h"
#અન્તર્ભાવ "vdns/httpsrr.h"
#અન્તર્ભાવ "curlx/strdup.h"
#અન્તર્ભાવ "curlx/dynbuf.h"
#અન્તર્ભાવ "escape.h"  /* for Curl_hexencode() */
#અન્તર્ભાવ "urlapi-int.h"

#define DNS_CLASS_IN 0x01

સ્થિર શૂન્ય doh_close(સંરચના Curl_easy *data,
                      સંરચના Curl_resolv_async *async);

#ifdef CURLVERBOSE
સ્થિર અચલ અક્ષર * અચલ doh_code_str[] = {
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

સ્થિર અચલ અક્ષર *doh_strerror(DOHcode code)
{
  જો((size_t)code < CURL_ARRAYSIZE(doh_code_str))
    પાછા doh_code_str[code];
  પાછા "bad error code";
}

સ્થિર અચલ અક્ષર *doh_type2name(DNStype dnstype)
{
  પસંદગી(dnstype) {
  સ્થિતિ CURL_DNS_TYPE_A:
    પાછા "A";
  સ્થિતિ CURL_DNS_TYPE_AAAA:
    પાછા "AAAA";
#ifdef USE_HTTPSRR
  સ્થિતિ CURL_DNS_TYPE_HTTPS:
    પાછા "HTTPS";
#endif
  મૂળભૂત:
    પાછા "unknown";
  }
}

#endif /* CURLVERBOSE */

/* @unittest 1655
 */
UNITTEST DOHcode doh_req_encode(અચલ અક્ષર *host,
                                DNStype dnstype,
                                unsigned અક્ષર *dnsp,  /* buffer */
                                size_t len,  /* buffer size */
                                size_t *olen);  /* output length */
UNITTEST DOHcode doh_req_encode(અચલ અક્ષર *host,
                                DNStype dnstype,
                                unsigned અક્ષર *dnsp, /* buffer */
                                size_t len,   /* buffer size */
                                size_t *olen) /* output length */
{
  અચલ size_t hostlen = strlen(host);
  unsigned અક્ષર *orig = dnsp;
  અચલ અક્ષર *hostp = host;

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
  જો(host[hostlen - 1] != '.')
    expected_len++;

  જો(expected_len > DOH_MAX_DNSREQ_SIZE)
    પાછા DOH_DNS_NAME_TOO_LONG;

  જો(len < expected_len)
    પાછા DOH_TOO_SMALL_BUFFER;

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
  જ્યારે(*hostp) {
    size_t labellen;
    અચલ અક્ષર *dot = strchr(hostp, '.');
    જો(dot)
      labellen = dot - hostp;
    નહીંતર
      labellen = strlen(hostp);
    જો((labellen > 63) || (!labellen)) {
      /* label is too long or too short, error out */
      *olen = 0;
      પાછા DOH_DNS_BAD_LABEL;
    }
    /* label is non-empty, process it */
    *dnsp++ = (unsigned અક્ષર)labellen;
    memcpy(dnsp, hostp, labellen);
    dnsp += labellen;
    hostp += labellen;
    /* advance past dot, but only if there is one */
    જો(dot)
      hostp++;
  } /* next label */

  *dnsp++ = 0; /* append zero-length label for root */

  /* There are assigned TYPE codes beyond 255: use range [1..65535] */
  *dnsp++ = (unsigned અક્ષર)(255 & (dnstype >> 8)); /* upper 8-bit TYPE */
  *dnsp++ = (unsigned અક્ષર)(255 & dnstype);        /* lower 8-bit TYPE */

  *dnsp++ = '\0'; /* upper 8-bit CLASS */
  *dnsp++ = DNS_CLASS_IN; /* IN - "the Internet" */

  *olen = dnsp - orig;

  /* verify that our estimation of length is valid, since
   * this has led to buffer overflows in this function */
  DEBUGASSERT(*olen == expected_len);
  પાછા DOH_OK;
}

સ્થિર size_t doh_probe_write_cb(અક્ષર *contents, size_t size, size_t nmemb,
                                 શૂન્ય *userp)
{
  size_t realsize = size * nmemb;
  સંરચના Curl_easy *data = userp;
  સંરચના doh_request *doh_req = Curl_meta_get(data, CURL_EZM_DOH_PROBE);
  જો(!doh_req)
    પાછા CURL_WRITEFUNC_ERROR;

  જો(curlx_dyn_addn(&doh_req->resp_body, contents, realsize))
    પાછા 0;

  પાછા realsize;
}

સ્થિર શૂન્ય doh_probe_done(સંરચના Curl_easy *doh,
                           સંરચના Curl_easy *master, CURLcode result);
સ્થિર શૂન્ય doh_probe_dtor(અચલ શૂન્ય *ચાવી, size_t klen, શૂન્ય *e)
{
  (શૂન્ય)ચાવી;
  (શૂન્ય)klen;
  જો(e) {
    સંરચના doh_request *doh_req = e;
    curl_slist_free_all(doh_req->req_hds);
    curlx_dyn_free(&doh_req->resp_body);
    curlx_free(e);
  }
}

#define ERROR_CHECK_SETOPT(x, y)                        \
  કરો {                                                  \
    result = curl_easy_setopt((CURL *)doh, x, y);       \
    જો(result &&                                        \
       result != CURLE_NOT_BUILT_IN &&                  \
       result != CURLE_UNKNOWN_OPTION)                  \
      જાઓ error;                                       \
  } જ્યારે(0)

સ્થિર CURLcode doh_probe_run(સંરચના Curl_easy *data,
                              DNStype dnstype,
                              અચલ અક્ષર *host,
                              અચલ અક્ષર *url, CURLM *multi,
                              uint32_t resolv_id,
                              uint32_t *pmid)
{
  સંરચના Curl_easy *doh = ખાલી;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  સંરચના doh_request *doh_req;
  DOHcode d;
  bool maybe_https = !curl_strnequal(url, STRCONST("http:"));

  *pmid = UINT32_MAX;

  doh_req = curlx_calloc(1, કદ(*doh_req));
  જો(!doh_req)
    પાછા CURLE_OUT_OF_MEMORY;
  doh_req->resolv_id = resolv_id;
  doh_req->dnstype = dnstype;
  curlx_dyn_init(&doh_req->resp_body, DYN_DOH_RESPONSE);

  d = doh_req_encode(host, dnstype, doh_req->req_body,
                     કદ(doh_req->req_body),
                     &doh_req->req_body_len);
  જો(d) {
    failf(data, "Failed to encode DoH packet [%d]", (પૂર્ણાંક)d);
    result = CURLE_OUT_OF_MEMORY;
    જાઓ error;
  }

  timeout_ms = Curl_timeleft_ms(data);
  જો(timeout_ms < 0) {
    result = CURLE_OPERATION_TIMEDOUT;
    જાઓ error;
  }

  doh_req->req_hds =
    curl_slist_append(ખાલી, "Content-Type: application/dns-message");
  જો(!doh_req->req_hds) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ error;
  }

  /* Curl_open() is the internal version of curl_easy_init() */
  result = Curl_open(&doh);
  જો(result)
    જાઓ error;

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
  જો(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);
    ERROR_CHECK_SETOPT(CURLOPT_PIPEWAIT, 1L);
  }
#endif
#ifndef DEBUGBUILD
  /* enforce HTTPS if not debug */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
#નહીંતર
  /* in debug mode, also allow http */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
#endif
  ERROR_CHECK_SETOPT(CURLOPT_TIMEOUT_MS, (long)timeout_ms);
  ERROR_CHECK_SETOPT(CURLOPT_SHARE, (CURLSH *)data->share);
  જો(data->set.err && data->set.err != stderr)
    ERROR_CHECK_SETOPT(CURLOPT_STDERR, data->set.err);
  જો(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_doh))
    ERROR_CHECK_SETOPT(CURLOPT_VERBOSE, 1L);
  જો(data->set.no_signal)
    ERROR_CHECK_SETOPT(CURLOPT_NOSIGNAL, 1L);
  જો(data->set.fdebug)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGFUNCTION, data->set.fdebug);
  જો(data->set.debugdata)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGDATA, data->set.debugdata);

#ifdef USE_RESOLV_ARES
  જો(CURL_EASY_STR(data, STRING_DNS_SERVERS)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_SERVERS,
                       CURL_EASY_STR(data, STRING_DNS_SERVERS));
  }
  જો(CURL_EASY_STR(data, STRING_DNS_INTERFACE)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_INTERFACE,
                       CURL_EASY_STR(data, STRING_DNS_INTERFACE));
  }
  જો(CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP4,
                       CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4));
  }
  જો(CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP6,
                       CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6));
  }
#endif
  જો(maybe_https) {
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
    જો(CURL_EASY_STR(data, STRING_SSL_CAFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO,
                         CURL_EASY_STR(data, STRING_SSL_CAFILE));
    }
    જો(data->set.blobs[BLOB_CAINFO]) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO_BLOB, data->set.blobs[BLOB_CAINFO]);
    }
    જો(CURL_EASY_STR(data, STRING_SSL_CAPATH)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAPATH,
                         CURL_EASY_STR(data, STRING_SSL_CAPATH));
    }
    જો(CURL_EASY_STR(data, STRING_SSL_CRLFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CRLFILE,
                         CURL_EASY_STR(data, STRING_SSL_CRLFILE));
    }
    જો(data->set.ssl.certinfo)
      ERROR_CHECK_SETOPT(CURLOPT_CERTINFO, 1L);
    જો(data->set.ssl_fsslctx)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_FUNCTION, data->set.ssl_fsslctx);
    જો(data->set.ssl_fsslctxp)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_DATA, data->set.ssl_fsslctxp);
    જો(CURL_EASY_STR(data, STRING_SSL_EC_CURVES)) {
      ERROR_CHECK_SETOPT(CURLOPT_SSL_EC_CURVES,
                         CURL_EASY_STR(data, STRING_SSL_EC_CURVES));
    }
    (શૂન્ય)curl_easy_setopt(doh, CURLOPT_SSL_OPTIONS,
                           ((long)data->set.ssl.ssl_options &
                            ~CURLSSLOPT_AUTO_CLIENT_CERT));
  }

  doh->state.internal = TRUE;
  doh->master_mid = data->mid; /* master transfer of this one */
  doh->sub_xfer_done = doh_probe_done;

  result = Curl_meta_set(doh, CURL_EZM_DOH_PROBE, doh_req, doh_probe_dtor);
  doh_req = ખાલી; /* call took ownership */
  જો(result)
    જાઓ error;

  /* DoH handles must not inherit private_data. The handles may be passed to
     the user via callbacks and the user will be able to identify them as
     internal handles because private data is not set. The user can then set
     private_data via CURLOPT_PRIVATE if they so choose. */
  DEBUGASSERT(!doh->set.private_data);

  જો(Curl_multi_add_handle(multi, doh))
    જાઓ error;

  *pmid = doh->mid;
  પાછા CURLE_OK;

error:
  Curl_close(&doh);
  જો(doh_req)
    doh_probe_dtor(ખાલી, 0, doh_req);
  પાછા result;
}

/*
 * Curl_doh() starts a name resolve using DoH. It resolves a name and returns
 * a 'Curl_addrinfo *' with the address information.
 */

CURLcode Curl_doh(સંરચના Curl_easy *data,
                  સંરચના Curl_resolv_async *async)
{
  CURLcode result = CURLE_OK;
  સંરચના doh_probes *dohp = ખાલી;
  size_t i;

  DEBUGASSERT(!async->doh);
  DEBUGASSERT(async->peer->hostname[0]);
  જો(async->doh) {
    DEBUGASSERT(0); /* should not happen */
    Curl_doh_cleanup(data, async);
  }

  જો(!async->dns_queries)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef USE_HTTPSRR
  જો(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     (async->dns_queries & CURL_DNSQ_HTTPS)) {
    /* Can't mix those in the same async resolve */
    DEBUGASSERT(0);
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  }
#નહીંતર
  જો(async->dns_queries & CURL_DNSQ_HTTPS) {
    DEBUGASSERT(0);
    પાછા CURLE_NOT_BUILT_IN;
  }
#endif

  /* start clean, consider allocating this struct on demand */
  async->doh = dohp = curlx_calloc(1, કદ(સંરચના doh_probes));
  જો(!dohp)
    પાછા CURLE_OUT_OF_MEMORY;

  માટે(i = 0; i < DOH_SLOT_ગણતરી; ++i) {
    dohp->probe_rc[i] = DOH_OK;
    dohp->probe_mid[i] = UINT32_MAX;
  }

#ifdef USE_IPV6
  /* AAAA results have preference in happy eyeballing, trigger first */
  જો(async->dns_queries & CURL_DNSQ_AAAA) {
    /* create IPv6 DoH request */
    result = doh_probe_run(data, CURL_DNS_TYPE_AAAA,
                           async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH),
                           data->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV6]);
    જો(result)
      જાઓ error;
    async->queries_ongoing++;
  }
#endif

  /* create IPv4 DoH request */
  જો(async->dns_queries & CURL_DNSQ_A) {
    result = doh_probe_run(data, CURL_DNS_TYPE_A,
                           async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH),
                           data->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV4]);
    જો(result)
      જાઓ error;
    async->queries_ongoing++;
  }

#ifdef USE_HTTPSRR
  જો(async->dns_queries & CURL_DNSQ_HTTPS) {
    અક્ષર *qname = ખાલી;
    જો(async->peer->port != PORT_HTTPS) {
      qname = curl_maprintf("_%u._https.%s",
                            async->peer->port, async->peer->hostname);
      જો(!qname)
        જાઓ error;
    }
    result = doh_probe_run(data, CURL_DNS_TYPE_HTTPS,
                           qname ? qname : async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH), data->multi,
                           async->id,
                           &dohp->probe_mid[DOH_SLOT_HTTPS_RR]);
    curlx_free(qname);
    જો(result)
      જાઓ error;
    async->queries_ongoing++;
  }
#endif
  પાછા CURLE_OK;

error:
  Curl_doh_cleanup(data, async);
  પાછા result;
}

સ્થિર DOHcode doh_skipqname(અચલ unsigned અક્ષર *doh, size_t dohlen,
                             unsigned પૂર્ણાંક *indexp)
{
  unsigned અક્ષર length;
  કરો {
    જો(dohlen < (*indexp + 1))
      પાછા DOH_DNS_OUT_OF_RANGE;
    length = doh[*indexp];
    જો((length & 0xc0) == 0xc0) {
      /* name pointer, advance over it and be done */
      જો(dohlen < (*indexp + 2))
        પાછા DOH_DNS_OUT_OF_RANGE;
      *indexp += 2;
      તોડો;
    }
    જો(length & 0xc0)
      પાછા DOH_DNS_BAD_LABEL;
    જો(dohlen < (*indexp + 1 + length))
      પાછા DOH_DNS_OUT_OF_RANGE;
    *indexp += (unsigned પૂર્ણાંક)(1 + length);
  } જ્યારે(length);
  પાછા DOH_OK;
}

સ્થિર unsigned short doh_get16bit(અચલ unsigned અક્ષર *doh,
                                   unsigned પૂર્ણાંક index)
{
  પાછા (unsigned short)((doh[index] << 8) | doh[index + 1]);
}

સ્થિર unsigned પૂર્ણાંક doh_get32bit(અચલ unsigned અક્ષર *doh, unsigned પૂર્ણાંક index)
{
  /* make clang and gcc optimize this to bswap by incrementing
     the pointer first. */
  doh += index;

  /* avoid undefined behavior by casting to unsigned before shifting
     24 bits, possibly into the sign bit. codegen is same, but
     ub sanitizer will not be upset */
  પાછા ((unsigned)doh[0] << 24) | ((unsigned)doh[1] << 16) |
         ((unsigned)doh[2] << 8) | doh[3];
}

સ્થિર શૂન્ય doh_store_a(અચલ unsigned અક્ષર *doh, પૂર્ણાંક index,
                        સંરચના dohentry *d)
{
  /* silently ignore addresses over the limit */
  જો(d->numaddr < DOH_MAX_ADDR) {
    સંરચના dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_A;
    memcpy(&a->ip.v4, &doh[index], 4);
    d->numaddr++;
  }
}

સ્થિર શૂન્ય doh_store_aaaa(અચલ unsigned અક્ષર *doh, પૂર્ણાંક index,
                           સંરચના dohentry *d)
{
  /* silently ignore addresses over the limit */
  જો(d->numaddr < DOH_MAX_ADDR) {
    સંરચના dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_AAAA;
    memcpy(&a->ip.v6, &doh[index], 16);
    d->numaddr++;
  }
}

#ifdef USE_HTTPSRR
સ્થિર DOHcode doh_store_https(અચલ unsigned અક્ષર *doh, પૂર્ણાંક index,
                               સંરચના dohentry *d, uint16_t len)
{
  /* silently ignore RRs over the limit */
  જો(d->numhttps_rrs < DOH_MAX_HTTPS) {
    સંરચના dohhttps_rr *h = &d->https_rrs[d->numhttps_rrs];
    h->val = curlx_memdup(&doh[index], len);
    જો(!h->val)
      પાછા DOH_OUT_OF_MEM;
    h->len = len;
    d->numhttps_rrs++;
  }
  પાછા DOH_OK;
}
#endif

સ્થિર DOHcode doh_rdata(અચલ unsigned અક્ષર *doh,
                         unsigned short rdlength,
                         unsigned short type,
                         પૂર્ણાંક index,
                         સંરચના dohentry *d)
{
  /* RDATA
     - A (TYPE 1): 4 bytes
     - AAAA (TYPE 28): 16 bytes
     - HTTPS (TYPE 65): N bytes */
  પસંદગી(type) {
  સ્થિતિ CURL_DNS_TYPE_A:
    જો(rdlength != 4)
      પાછા DOH_DNS_RDATA_LEN;
    doh_store_a(doh, index, d);
    તોડો;
  સ્થિતિ CURL_DNS_TYPE_AAAA:
    જો(rdlength != 16)
      પાછા DOH_DNS_RDATA_LEN;
    doh_store_aaaa(doh, index, d);
    તોડો;
#ifdef USE_HTTPSRR
  સ્થિતિ CURL_DNS_TYPE_HTTPS:
    જો(rdlength < 3)
      પાછા DOH_DNS_RDATA_LEN;
    પાછા doh_store_https(doh, index, d, rdlength);
#endif
  મૂળભૂત:
    /* unsupported type, or type we do not store, skip it */
    તોડો;
  }
  પાછા DOH_OK;
}

/* @unittest 1655 */
UNITTEST શૂન્ય de_init(સંરચના dohentry *de);
UNITTEST શૂન્ય de_init(સંરચના dohentry *de)
{
  memset(de, 0, કદ(*de));
  de->ttl = INT_MAX;
}

/* TTL value cap */
#define MAX_DNS_TTL 86400U /* 24 hours */
/* @unittest 1650 */
UNITTEST DOHcode doh_resp_decode(અચલ unsigned અક્ષર *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 સંરચના dohentry *d);
UNITTEST DOHcode doh_resp_decode(અચલ unsigned અક્ષર *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 સંરચના dohentry *d)
{
  unsigned અક્ષર rcode;
  unsigned short qdcount;
  unsigned short ancount;
  unsigned short type = 0;
  unsigned short rdlength;
  unsigned short nscount;
  unsigned short arcount;
  unsigned પૂર્ણાંક index = 12;
  DOHcode rc;

  જો(dohlen < 12)
    પાછા DOH_TOO_SMALL_BUFFER; /* too small */
  જો(!doh || doh[0] || doh[1])
    પાછા DOH_DNS_BAD_ID; /* bad ID */
  rcode = doh[3] & 0x0f;
  જો(rcode == 3)
    પાછા DOH_DNS_NXDOMAIN; /* name does not exist */
  જો(rcode)
    પાછા DOH_DNS_BAD_RCODE; /* bad rcode */

  qdcount = doh_get16bit(doh, 4);
  જ્યારે(qdcount) {
    rc = doh_skipqname(doh, dohlen, &index);
    જો(rc)
      પાછા rc; /* bad qname */
    જો(dohlen < (index + 4))
      પાછા DOH_DNS_OUT_OF_RANGE;
    index += 4; /* skip question's type and class */
    qdcount--;
  }

  ancount = doh_get16bit(doh, 6);
  જ્યારે(ancount) {
    unsigned short dnsclass;
    unsigned પૂર્ણાંક ttl;

    rc = doh_skipqname(doh, dohlen, &index);
    જો(rc)
      પાછા rc; /* bad qname */

    જો(dohlen < (index + 2))
      પાછા DOH_DNS_OUT_OF_RANGE;

    type = doh_get16bit(doh, index);
    જો((type != CURL_DNS_TYPE_CNAME) &&  /* may be synthesized from DNAME */
       (type != CURL_DNS_TYPE_DNAME) &&  /* if present, accept and ignore */
       (type != dnstype))
      /* Not the same type as was asked for, nor CNAME nor DNAME */
      પાછા DOH_DNS_UNEXPECTED_TYPE;
    index += 2;

    જો(dohlen < (index + 2))
      પાછા DOH_DNS_OUT_OF_RANGE;
    dnsclass = doh_get16bit(doh, index);
    જો(DNS_CLASS_IN != dnsclass)
      પાછા DOH_DNS_UNEXPECTED_CLASS; /* unsupported */
    index += 2;

    જો(dohlen < (index + 4))
      પાછા DOH_DNS_OUT_OF_RANGE;

    ttl = doh_get32bit(doh, index);
    જો(ttl > MAX_DNS_TTL)
      ttl = MAX_DNS_TTL;
    જો(ttl < d->ttl)
      d->ttl = ttl;
    index += 4;

    જો(dohlen < (index + 2))
      પાછા DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    જો(dohlen < (index + rdlength))
      પાછા DOH_DNS_OUT_OF_RANGE;

    rc = doh_rdata(doh, rdlength, type, (પૂર્ણાંક)index, d);
    જો(rc)
      પાછા rc;
    index += rdlength;
    ancount--;
  }

  nscount = doh_get16bit(doh, 8);
  જ્યારે(nscount) {
    rc = doh_skipqname(doh, dohlen, &index);
    જો(rc)
      પાછા rc; /* bad qname */

    જો(dohlen < (index + 8))
      પાછા DOH_DNS_OUT_OF_RANGE;

    index += 2 + 2 + 4; /* type, dnsclass and ttl */

    જો(dohlen < (index + 2))
      પાછા DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    જો(dohlen < (index + rdlength))
      પાછા DOH_DNS_OUT_OF_RANGE;
    index += rdlength;
    nscount--;
  }

  arcount = doh_get16bit(doh, 10);
  જ્યારે(arcount) {
    rc = doh_skipqname(doh, dohlen, &index);
    જો(rc)
      પાછા rc; /* bad qname */

    જો(dohlen < (index + 8))
      પાછા DOH_DNS_OUT_OF_RANGE;

    index += 2 + 2 + 4; /* type, dnsclass and ttl */

    જો(dohlen < (index + 2))
      પાછા DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    જો(dohlen < (index + rdlength))
      પાછા DOH_DNS_OUT_OF_RANGE;
    index += rdlength;
    arcount--;
  }

  જો(index != dohlen)
    પાછા DOH_DNS_MALFORMAT; /* something is wrong */

  પાછા DOH_OK; /* ok */
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
સ્થિર CURLcode doh2ai(અચલ સંરચના dohentry *de, અચલ અક્ષર *hostname,
                       પૂર્ણાંક port, સંરચના Curl_addrinfo **aip)
{
  સંરચના Curl_addrinfo *ai;
  સંરચના Curl_addrinfo *prevai = ખાલી;
  સંરચના Curl_addrinfo *firstai = ખાલી;
  સંરચના sockaddr_in *addr;
#ifdef USE_IPV6
  સંરચના sockaddr_in6 *addr6;
#endif
  size_t hostlen = strlen(hostname) + 1; /* include null-terminator */
  CURLcode result = CURLE_OK;
  પૂર્ણાંક i;

  માટે(i = 0; i < de->numaddr; i++) {
    size_t ss_size;
    CURL_SA_FAMILY_T addrtype;
    જો(de->addr[i].type == CURL_DNS_TYPE_AAAA) {
#ifndef USE_IPV6
      /* we cannot handle IPv6 addresses */
      ચાલુ;
#નહીંતર
      ss_size = કદ(સંરચના sockaddr_in6);
      addrtype = AF_INET6;
#endif
    }
    નહીંતર {
      ss_size = કદ(સંરચના sockaddr_in);
      addrtype = AF_INET;
    }

    ai = curlx_calloc(1, કદ(સંરચના Curl_addrinfo) + ss_size + hostlen);
    જો(!ai) {
      result = CURLE_OUT_OF_MEMORY;
      તોડો;
    }
    ai->ai_addr = (શૂન્ય *)((અક્ષર *)ai + કદ(સંરચના Curl_addrinfo));
    ai->ai_canonname = (શૂન્ય *)((અક્ષર *)ai->ai_addr + ss_size);
    memcpy(ai->ai_canonname, hostname, hostlen);

    જો(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    જો(prevai)
      /* make the previous entry point to this */
      prevai->ai_next = ai;

    ai->ai_family = addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_size;

    /* leave the rest of the struct filled with zero */

    પસંદગી(ai->ai_family) {
    સ્થિતિ AF_INET:
      addr = (શૂન્ય *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(કદ(સંરચના in_addr) == કદ(de->addr[i].ip.v4));
      memcpy(&addr->sin_addr, &de->addr[i].ip.v4, કદ(સંરચના in_addr));
      addr->sin_family = addrtype;
      addr->sin_port = htons((unsigned short)port);
      તોડો;

#ifdef USE_IPV6
    સ્થિતિ AF_INET6:
      addr6 = (શૂન્ય *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(કદ(સંરચના in6_addr) == કદ(de->addr[i].ip.v6));
      memcpy(&addr6->sin6_addr, &de->addr[i].ip.v6, કદ(સંરચના in6_addr));
      addr6->sin6_family = addrtype;
      addr6->sin6_port = htons((unsigned short)port);
      તોડો;
#endif
    }

    prevai = ai;
  }

  જો(result) {
    Curl_freeaddrinfo(firstai);
    firstai = ખાલી;
  }
  *aip = firstai;

  પાછા result;
}

/* @unittest 1655 */
UNITTEST શૂન્ય de_cleanup(સંરચના dohentry *d);
UNITTEST શૂન્ય de_cleanup(સંરચના dohentry *d)
{
#ifdef USE_HTTPSRR
  પૂર્ણાંક i = 0;
  માટે(i = 0; i < d->numhttps_rrs; i++)
    curlx_safefree(d->https_rrs[i].val);
#નહીંતર
  (શૂન્ય)d;
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
સ્થિર CURLcode doh_decode_rdata_name(અચલ unsigned અક્ષર **buf,
                                      size_t *remaining, અક્ષર **dnsname)
{
  અચલ unsigned અક્ષર *cp = ખાલી;
  size_t rem = 0;
  unsigned અક્ષર clen = 0; /* chunk len */
  સંરચના dynbuf thename;

  DEBUGASSERT(buf && remaining && dnsname);
  જો(!buf || !remaining || !dnsname || !*remaining)
    પાછા CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&thename, CURL_MAXLEN_HOST_NAME);
  rem = *remaining;
  cp = *buf;
  clen = *cp++;
  /* RFC 9460 says it must be uncompressed */
  જો(clen > 63)
    પાછા CURLE_WEIRD_SERVER_REPLY;

  જો(clen == 0) {
    /* special case - return "." as name */
    જો(curlx_dyn_addn(&thename, ".", 1))
      પાછા CURLE_OUT_OF_MEMORY;
  }
  જ્યારે(clen) {
    જો(clen >= rem) {
      curlx_dyn_free(&thename);
      પાછા CURLE_OUT_OF_MEMORY;
    }
    જો(curlx_dyn_addn(&thename, cp, clen) ||
       curlx_dyn_addn(&thename, ".", 1))
      પાછા CURLE_TOO_LARGE;

    cp += clen;
    rem -= (clen + 1);
    જો(rem <= 0) {
      curlx_dyn_free(&thename);
      પાછા CURLE_OUT_OF_MEMORY;
    }
    clen = *cp++;
    જો(clen > 63) {
      /* invalid format */
      curlx_dyn_free(&thename);
      પાછા CURLE_WEIRD_SERVER_REPLY;
    }
  }
  *buf = cp;
  *remaining = rem - 1;
  *dnsname = curlx_dyn_ptr(&thename);
  પાછા CURLE_OK;
}

/* scan for byte values <= 32 or 127 */
સ્થિર CURLcode junkscan(અચલ અક્ષર *url)
{
  અચલ unsigned અક્ષર *p = (અચલ unsigned અક્ષર *)url;
  જ્યારે(*p) {
    જો(*p <= 0x20 || *p == 127)
      પાછા CURLE_WEIRD_SERVER_REPLY;
    p++;
  }
  પાછા CURLE_OK;
}

/* @unittest 1658 */
UNITTEST CURLcode doh_resp_decode_httpsrr(સંરચના Curl_easy *data,
                                          અચલ unsigned અક્ષર *cp, size_t len,
                                          સંરચના Curl_https_rrinfo **hrr);
UNITTEST CURLcode doh_resp_decode_httpsrr(સંરચના Curl_easy *data,
                                          અચલ unsigned અક્ષર *cp, size_t len,
                                          સંરચના Curl_https_rrinfo **hrr)
{
  uint16_t pcode = 0, plen = 0;
  uint32_t expected_min_pcode = 0;
  સંરચના Curl_https_rrinfo *lhrr = ખાલી;
  અક્ષર *dnsname = ખાલી;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (શૂન્ય)data;
  *hrr = ખાલી;
  જો(len <= 2)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  lhrr = curlx_calloc(1, કદ(સંરચના Curl_https_rrinfo));
  જો(!lhrr)
    પાછા CURLE_OUT_OF_MEMORY;
  lhrr->priority = doh_get16bit(cp, 0);
  cp += 2;
  len -= 2;
  જો(doh_decode_rdata_name(&cp, &len, &dnsname) != CURLE_OK)
    જાઓ err;
  lhrr->target = dnsname;
  result = junkscan(dnsname);
  જો(result) {
    /* unacceptable hostname content */
    જાઓ err;
  }
  જ્યારે(len >= 4) {
    pcode = doh_get16bit(cp, 0);
    plen = doh_get16bit(cp, 2);
    cp += 4;
    len -= 4;
    જો(pcode < expected_min_pcode || plen > len) {
      result = CURLE_WEIRD_SERVER_REPLY;
      જાઓ err;
    }
    result = Curl_httpsrr_set(lhrr, pcode, cp, plen);
    જો(result)
      જાઓ err;
    Curl_httpsrr_trace(data, lhrr);
    cp += plen;
    len -= plen;
    expected_min_pcode = pcode + 1;
  }
  *hrr = lhrr;
  પાછા CURLE_OK;
err:
  Curl_httpsrr_destroy(lhrr);
  પાછા result;
}

#endif /* USE_HTTPSRR */

/* called from multi when a sub transfer, e.g. doh probe, is done.
 * Parse the response and set the results in the `async` context
 * of master, using the id from the probe's CURL_EZM_DOH_PROBE
 * meta data. */
સ્થિર શૂન્ય doh_probe_done(સંરચના Curl_easy *doh,
                           સંરચના Curl_easy *master, CURLcode result)
{
  સંરચના Curl_resolv_async *async = ખાલી;
  સંરચના doh_probes *dohp = ખાલી;
  સંરચના doh_request *doh_req = ખાલી;
  સંરચના Curl_addrinfo **pdest_ai;
  સંરચના dohentry de;
  પૂર્ણાંક slot, httpcode;

  de_init(&de);
  doh_req = Curl_meta_get(doh, CURL_EZM_DOH_PROBE);
  જો(!doh_req) {
    /* transfer `doh` is not a DoH probe. */
    DEBUGASSERT(0);
    જાઓ out;
  }

  async = Curl_async_get(master, doh_req->resolv_id);
  જો(!async) {
    CURL_TRC_DNS(master, "[%u] ignoring outdated DoH response",
                 doh_req->resolv_id);
    જાઓ out;
  }
  dohp = async->doh;

  માટે(slot = 0; slot < DOH_SLOT_ગણતરી; ++slot) {
    જો(dohp->probe_mid[slot] == doh->mid)
      તોડો;
  }
  /* We really should have found the slot where to store the response */
  જો(slot >= DOH_SLOT_ગણતરી) {
    failf(master, "DoH: unknown sub request done");
    DEBUGASSERT(0);
    જાઓ out;
  }

  async->queries_ongoing--;
  dohp = async->doh;
  httpcode = doh->info.httpcode;
  પસંદગી(slot) {
  સ્થિતિ DOH_SLOT_IPV4:
    async->dns_responses |= CURL_DNSQ_A;
    તોડો;
#ifdef USE_IPV6
  સ્થિતિ DOH_SLOT_IPV6:
    async->dns_responses |= CURL_DNSQ_AAAA;
    તોડો;
#endif
#ifdef USE_HTTPSRR
  સ્થિતિ DOH_SLOT_HTTPS_RR:
    async->dns_responses |= CURL_DNSQ_HTTPS;
    તોડો;
#endif
  મૂળભૂત:
    DEBUGASSERT(0);
    તોડો;
  }

  જો(result) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: %s",
          doh_type2name(doh_req->dnstype), curl_easy_strerror(result));
    જાઓ out;
  }
  નહીંતર જો((httpcode < 200) || (httpcode >= 300)) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: HTTP status %d",
          doh_type2name(doh_req->dnstype), httpcode);
    જાઓ out;
  }

  dohp->probe_rc[slot] = doh_resp_decode(curlx_dyn_uptr(&doh_req->resp_body),
                                         curlx_dyn_len(&doh_req->resp_body),
                                         doh_req->dnstype, &de);
  જો(dohp->probe_rc[slot]) {
#ifdef USE_HTTPSRR
    જો((dohp->probe_rc[slot] == DOH_NO_CONTENT) &&
       (doh_req->dnstype == CURL_DNS_TYPE_HTTPS)) {
      dohp->probe_rc[slot] = DOH_DNS_NXDOMAIN;
    }
#endif
    infof(doh, "[DoH] [%s] error decoding response: %s",
          doh_type2name(doh_req->dnstype),
          doh_strerror(dohp->probe_rc[slot]));
    જાઓ out;
  }

  જો(doh_req->dnstype == CURL_DNS_TYPE_A)
    pdest_ai = &async->ai_A;
  નહીંતર જો(doh_req->dnstype == CURL_DNS_TYPE_AAAA)
    pdest_ai = &async->ai_AAAA;
  નહીંતર
    pdest_ai = ખાલી;

  જો(pdest_ai && de.numaddr) {
    જો(*pdest_ai) {
      Curl_freeaddrinfo(*pdest_ai);
      *pdest_ai = ખાલી;
    }
    result = doh2ai(&de, async->peer->hostname, async->peer->port, pdest_ai);
    જો(result) { /* hard failure on our side, fail completely */
      infof(doh, "[DoH] [%s] error creating addrinfo: %s",
            doh_type2name(doh_req->dnstype), curl_easy_strerror(result));
      dohp->probe_rc[slot] = DOH_OOM;
      async->result = result;
    }
  }
#ifdef USE_HTTPSRR
  નહીંતર જો((doh_req->dnstype == CURL_DNS_TYPE_HTTPS) && de.numhttps_rrs) {
    CURL_TRC_DNS(doh, "[HTTPS] got %d records", de.numhttps_rrs);
    result = doh_resp_decode_httpsrr(doh, de.https_rrs->val,
                                     de.https_rrs->len, &async->httpsrr);
    જો(result) {
      dohp->probe_rc[slot] = DOH_HTTP_FAILED;
      infof(doh, "[DoH] error decoding HTTPS RR: %s",
            curl_easy_strerror(result));
      જાઓ out;
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

CURLcode Curl_doh_take_result(સંરચના Curl_easy *data,
                              સંરચના Curl_resolv_async *async,
                              સંરચના Curl_dns_entry **pdns)
{
  સંરચના doh_probes *dohp = async->doh;
  CURLcode result = CURLE_OK;

  *pdns = ખાલી; /* defaults to no response */
  જો(!dohp)
    પાછા CURLE_OUT_OF_MEMORY;

  async->negative_answer = FALSE;
  જો(async->result) {
    result = async->result;
    જાઓ out;
  }

  જો(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     dohp->probe_mid[DOH_SLOT_IPV4] == UINT32_MAX &&
     dohp->probe_mid[DOH_SLOT_IPV6] == UINT32_MAX) {
    failf(data, "Could not DoH-resolve: %s", async->peer->hostname);
    પાછા async->for_proxy ?
      CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
  }
  નહીંતર જો(!async->queries_ongoing) {
    સંરચના Curl_dns_entry *dns = ખાલી;
    bool negative = TRUE;
    પૂર્ણાંક slot;

    /* remove DoH handles from multi handle and close them */
    doh_close(data, async);
    /* parse the responses, create the struct and return it! */
    માટે(slot = 0; slot < DOH_SLOT_ગણતરી; slot++) {
      /* Failing without an NXDOMAIN answer - a SERVFAIL-class rcode or
         an undecodable response - says nothing about the name. Such a
         failure must not be cached as a negative entry. */
      જો(dohp->probe_rc[slot] && (dohp->probe_rc[slot] != DOH_DNS_NXDOMAIN))
        negative = FALSE;
    } /* next slot */

    જો(async->ai_A || async->ai_AAAA) {
      dns = Curl_dnsc_mk_addr2(
        data, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
      જો(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        જાઓ out;
      }
    }
#ifdef USE_HTTPSRR
    નહીંતર જો((async->dns_queries & CURL_DNSQ_HTTPS) &&
            !dohp->probe_rc[DOH_SLOT_HTTPS_RR]) {
      Curl_httpsrr_trace(data, async->httpsrr);
      dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
      જો(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        જાઓ out;
      }
    }
#endif /* USE_HTTPSRR */
    નહીંતર {
      /* every query failed. Only NXDOMAIN answers for all of them
         make this a negative answer, eligible for caching. */
      async->negative_answer = negative;
      result = async->for_proxy ?
        CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
    }

    /* and add the entry to the cache */
    જો(dns)
      result = Curl_dnscache_add(data, dns);
    *pdns = dns;
  } /* !async->queries_ongoing */
  નહીંતર
    /* wait for pending DoH transactions to complete */
    પાછા CURLE_AGAIN;

out:
  Curl_doh_cleanup(data, async);
  પાછા result;
}

સ્થિર શૂન્ય doh_close(સંરચના Curl_easy *data,
                      સંરચના Curl_resolv_async *async)
{
  સંરચના doh_probes *doh = async ? async->doh : ખાલી;
  જો(doh && data->multi) {
    સંરચના Curl_easy *probe_data;
    uint32_t mid;
    size_t slot;
    માટે(slot = 0; slot < DOH_SLOT_ગણતરી; slot++) {
      mid = doh->probe_mid[slot];
      જો(mid == UINT32_MAX)
        ચાલુ;
      doh->probe_mid[slot] = UINT32_MAX;
      /* should have been called before data is removed from multi handle */
      DEBUGASSERT(data->multi);
      probe_data = data->multi ? Curl_multi_get_easy(data->multi, mid) : ખાલી;
      જો(!probe_data) {
        DEBUGF(infof(data, "Curl_doh_close: xfer for mid=%u not found!", mid));
        ચાલુ;
      }
      probe_data->sub_xfer_done = ખાલી; /* No longer interested in result */
      /* data->multi might already be reset at this time */
      Curl_multi_remove_handle(data->multi, probe_data);
      Curl_close(&probe_data);
    }
    CURL_TRC_DNS(data, "[DoH] probe done");
  }
}

શૂન્ય Curl_doh_cleanup(સંરચના Curl_easy *data,
                      સંરચના Curl_resolv_async *async)
{
  સંરચના doh_probes *dohp = async->doh;
  જો(dohp) {
    doh_close(data, async);
    curlx_safefree(async->doh);
  }
}

#endif /* CURL_DISABLE_DOH */
