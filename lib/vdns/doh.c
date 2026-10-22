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
#include "curl_setup.h"

#ifndef CURL_DISABLE_DOH

#include "urldata.h"
#include "curl_addrinfo.h"
#include "curl_trc.h"
#include "multiif.h"
#include "url.h"
#include "connect.h"
#include "vdns/doh.h"
#include "vdns/httpsrr.h"
#include "curlx/strdup.h"
#include "curlx/dynbuf.h"
#include "escape.h"  /* for Curl_hexencode() */
#include "urlapi-int.h"

#define DNS_CLASS_IN 0x01

static void doh_close(sa_nracanaa Curl_easy *data,
                      sa_nracanaa Curl_resolv_async *async);

#ifdef CURLVERBOSE
static const var_na * const doh_code_str[] = {
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

static const var_na *doh_strerror(DOHcode code)
{
  အကယ်၍((size_t)code < CURL_ARRAYSIZE(doh_code_str))
    ပြန်ပေး doh_code_str[code];
  ပြန်ပေး "bad error code";
}

static const var_na *doh_type2name(DNStype dnstype)
{
  switch(dnstype) {
  pak_sa CURL_DNS_TYPE_A:
    ပြန်ပေး "A";
  pak_sa CURL_DNS_TYPE_AAAA:
    ပြန်ပေး "AAAA";
#ifdef USE_HTTPSRR
  pak_sa CURL_DNS_TYPE_HTTPS:
    ပြန်ပေး "HTTPS";
#endif
  default:
    ပြန်ပေး "unknown";
  }
}

#endif /* CURLVERBOSE */

/* @unittest 1655
 */
UNITTEST DOHcode doh_req_encode(const var_na *host,
                                DNStype dnstype,
                                unsigned var_na *dnsp,  /* buffer */
                                size_t len,  /* buffer size */
                                size_t *olen);  /* output length */
UNITTEST DOHcode doh_req_encode(const var_na *host,
                                DNStype dnstype,
                                unsigned var_na *dnsp, /* buffer */
                                size_t len,   /* buffer size */
                                size_t *olen) /* output length */
{
  const size_t hostlen = strlen(host);
  unsigned var_na *orig = dnsp;
  const var_na *hostp = host;

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
  အကယ်၍(host[hostlen - 1] != '.')
    expected_len++;

  အကယ်၍(expected_len > DOH_MAX_DNSREQ_SIZE)
    ပြန်ပေး DOH_DNS_NAME_TOO_LONG;

  အကယ်၍(len < expected_len)
    ပြန်ပေး DOH_TOO_SMALL_BUFFER;

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
  နေစဉ်(*hostp) {
    size_t labellen;
    const var_na *dot = strchr(hostp, '.');
    အကယ်၍(dot)
      labellen = dot - hostp;
    မဟုတ်ပါက
      labellen = strlen(hostp);
    အကယ်၍((labellen > 63) || (!labellen)) {
      /* label is too long or too short, error out */
      *olen = 0;
      ပြန်ပေး DOH_DNS_BAD_LABEL;
    }
    /* label is non-empty, process it */
    *dnsp++ = (unsigned var_na)labellen;
    memcpy(dnsp, hostp, labellen);
    dnsp += labellen;
    hostp += labellen;
    /* advance past dot, but only if there is one */
    အကယ်၍(dot)
      hostp++;
  } /* next label */

  *dnsp++ = 0; /* append zero-length label for root */

  /* There are assigned TYPE codes beyond 255: use range [1..65535] */
  *dnsp++ = (unsigned var_na)(255 & (dnstype >> 8)); /* upper 8-bit TYPE */
  *dnsp++ = (unsigned var_na)(255 & dnstype);        /* lower 8-bit TYPE */

  *dnsp++ = '\0'; /* upper 8-bit CLASS */
  *dnsp++ = DNS_CLASS_IN; /* IN - "the Internet" */

  *olen = dnsp - orig;

  /* verify that our estimation of length is valid, since
   * this has led to buffer overflows in this function */
  DEBUGASSERT(*olen == expected_len);
  ပြန်ပေး DOH_OK;
}

static size_t doh_probe_write_cb(var_na *contents, size_t size, size_t nmemb,
                                 void *userp)
{
  size_t realsize = size * nmemb;
  sa_nracanaa Curl_easy *data = userp;
  sa_nracanaa doh_request *doh_req = Curl_meta_get(data, CURL_EZM_DOH_PROBE);
  အကယ်၍(!doh_req)
    ပြန်ပေး CURL_WRITEFUNC_ERROR;

  အကယ်၍(curlx_dyn_addn(&doh_req->resp_body, contents, realsize))
    ပြန်ပေး 0;

  ပြန်ပေး realsize;
}

static void doh_probe_done(sa_nracanaa Curl_easy *doh,
                           sa_nracanaa Curl_easy *master, CURLcode result);
static void doh_probe_dtor(const void *key, size_t klen, void *e)
{
  (void)key;
  (void)klen;
  အကယ်၍(e) {
    sa_nracanaa doh_request *doh_req = e;
    curl_slist_free_all(doh_req->req_hds);
    curlx_dyn_free(&doh_req->resp_body);
    curlx_free(e);
  }
}

#define ERROR_CHECK_SETOPT(x, y)                        \
  do {                                                  \
    result = curl_easy_setopt((CURL *)doh, x, y);       \
    အကယ်၍(result &&                                        \
       result != CURLE_NOT_BUILT_IN &&                  \
       result != CURLE_UNKNOWN_OPTION)                  \
      goto error;                                       \
  } နေစဉ်(0)

static CURLcode doh_probe_run(sa_nracanaa Curl_easy *data,
                              DNStype dnstype,
                              const var_na *host,
                              const var_na *url, CURLM *multi,
                              uint32_t resolv_id,
                              uint32_t *pmid)
{
  sa_nracanaa Curl_easy *doh = NULL;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  sa_nracanaa doh_request *doh_req;
  DOHcode d;
  bool maybe_https = !curl_strnequal(url, STRCONST("http:"));

  *pmid = UINT32_MAX;

  doh_req = curlx_calloc(1, parimaa_na(*doh_req));
  အကယ်၍(!doh_req)
    ပြန်ပေး CURLE_OUT_OF_MEMORY;
  doh_req->resolv_id = resolv_id;
  doh_req->dnstype = dnstype;
  curlx_dyn_init(&doh_req->resp_body, DYN_DOH_RESPONSE);

  d = doh_req_encode(host, dnstype, doh_req->req_body,
                     parimaa_na(doh_req->req_body),
                     &doh_req->req_body_len);
  အကယ်၍(d) {
    failf(data, "Failed to encode DoH packet [%d]", (ကိန်းပြည့်)d);
    result = CURLE_OUT_OF_MEMORY;
    goto error;
  }

  timeout_ms = Curl_timeleft_ms(data);
  အကယ်၍(timeout_ms < 0) {
    result = CURLE_OPERATION_TIMEDOUT;
    goto error;
  }

  doh_req->req_hds =
    curl_slist_append(NULL, "Content-Type: application/dns-message");
  အကယ်၍(!doh_req->req_hds) {
    result = CURLE_OUT_OF_MEMORY;
    goto error;
  }

  /* Curl_open() is the internal version of curl_easy_init() */
  result = Curl_open(&doh);
  အကယ်၍(result)
    goto error;

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
  အကယ်၍(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);
    ERROR_CHECK_SETOPT(CURLOPT_PIPEWAIT, 1L);
  }
#endif
#ifndef DEBUGBUILD
  /* enforce HTTPS if not debug */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
#မဟုတ်ပါက
  /* in debug mode, also allow http */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
#endif
  ERROR_CHECK_SETOPT(CURLOPT_TIMEOUT_MS, (long)timeout_ms);
  ERROR_CHECK_SETOPT(CURLOPT_SHARE, (CURLSH *)data->share);
  အကယ်၍(data->set.err && data->set.err != stderr)
    ERROR_CHECK_SETOPT(CURLOPT_STDERR, data->set.err);
  အကယ်၍(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_doh))
    ERROR_CHECK_SETOPT(CURLOPT_VERBOSE, 1L);
  အကယ်၍(data->set.no_signal)
    ERROR_CHECK_SETOPT(CURLOPT_NOSIGNAL, 1L);
  အကယ်၍(data->set.fdebug)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGFUNCTION, data->set.fdebug);
  အကယ်၍(data->set.debugdata)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGDATA, data->set.debugdata);

#ifdef USE_RESOLV_ARES
  အကယ်၍(CURL_EASY_STR(data, STRING_DNS_SERVERS)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_SERVERS,
                       CURL_EASY_STR(data, STRING_DNS_SERVERS));
  }
  အကယ်၍(CURL_EASY_STR(data, STRING_DNS_INTERFACE)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_INTERFACE,
                       CURL_EASY_STR(data, STRING_DNS_INTERFACE));
  }
  အကယ်၍(CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP4,
                       CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4));
  }
  အကယ်၍(CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP6,
                       CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6));
  }
#endif
  အကယ်၍(maybe_https) {
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
    အကယ်၍(CURL_EASY_STR(data, STRING_SSL_CAFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO,
                         CURL_EASY_STR(data, STRING_SSL_CAFILE));
    }
    အကယ်၍(data->set.blobs[BLOB_CAINFO]) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO_BLOB, data->set.blobs[BLOB_CAINFO]);
    }
    အကယ်၍(CURL_EASY_STR(data, STRING_SSL_CAPATH)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAPATH,
                         CURL_EASY_STR(data, STRING_SSL_CAPATH));
    }
    အကယ်၍(CURL_EASY_STR(data, STRING_SSL_CRLFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CRLFILE,
                         CURL_EASY_STR(data, STRING_SSL_CRLFILE));
    }
    အကယ်၍(data->set.ssl.certinfo)
      ERROR_CHECK_SETOPT(CURLOPT_CERTINFO, 1L);
    အကယ်၍(data->set.ssl_fsslctx)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_FUNCTION, data->set.ssl_fsslctx);
    အကယ်၍(data->set.ssl_fsslctxp)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_DATA, data->set.ssl_fsslctxp);
    အကယ်၍(CURL_EASY_STR(data, STRING_SSL_EC_CURVES)) {
      ERROR_CHECK_SETOPT(CURLOPT_SSL_EC_CURVES,
                         CURL_EASY_STR(data, STRING_SSL_EC_CURVES));
    }
    (void)curl_easy_setopt(doh, CURLOPT_SSL_OPTIONS,
                           ((long)data->set.ssl.ssl_options &
                            ~CURLSSLOPT_AUTO_CLIENT_CERT));
  }

  doh->state.internal = TRUE;
  doh->master_mid = data->mid; /* master transfer of this one */
  doh->sub_xfer_done = doh_probe_done;

  result = Curl_meta_set(doh, CURL_EZM_DOH_PROBE, doh_req, doh_probe_dtor);
  doh_req = NULL; /* call took ownership */
  အကယ်၍(result)
    goto error;

  /* DoH handles must not inherit private_data. The handles may be passed to
     the user via callbacks and the user will be able to identify them as
     internal handles because private data is not set. The user can then set
     private_data via CURLOPT_PRIVATE if they so choose. */
  DEBUGASSERT(!doh->set.private_data);

  အကယ်၍(Curl_multi_add_handle(multi, doh))
    goto error;

  *pmid = doh->mid;
  ပြန်ပေး CURLE_OK;

error:
  Curl_close(&doh);
  အကယ်၍(doh_req)
    doh_probe_dtor(NULL, 0, doh_req);
  ပြန်ပေး result;
}

/*
 * Curl_doh() starts a name resolve using DoH. It resolves a name and returns
 * a 'Curl_addrinfo *' with the address information.
 */

CURLcode Curl_doh(sa_nracanaa Curl_easy *data,
                  sa_nracanaa Curl_resolv_async *async)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa doh_probes *dohp = NULL;
  size_t i;

  DEBUGASSERT(!async->doh);
  DEBUGASSERT(async->peer->hostname[0]);
  အကယ်၍(async->doh) {
    DEBUGASSERT(0); /* should not happen */
    Curl_doh_cleanup(data, async);
  }

  အကယ်၍(!async->dns_queries)
    ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef USE_HTTPSRR
  အကယ်၍(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     (async->dns_queries & CURL_DNSQ_HTTPS)) {
    /* Can't mix those in the same async resolve */
    DEBUGASSERT(0);
    ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;
  }
#မဟုတ်ပါက
  အကယ်၍(async->dns_queries & CURL_DNSQ_HTTPS) {
    DEBUGASSERT(0);
    ပြန်ပေး CURLE_NOT_BUILT_IN;
  }
#endif

  /* start clean, consider allocating this struct on demand */
  async->doh = dohp = curlx_calloc(1, parimaa_na(sa_nracanaa doh_probes));
  အကယ်၍(!dohp)
    ပြန်ပေး CURLE_OUT_OF_MEMORY;

  အတွက်(i = 0; i < DOH_SLOT_COUNT; ++i) {
    dohp->probe_rc[i] = DOH_OK;
    dohp->probe_mid[i] = UINT32_MAX;
  }

#ifdef USE_IPV6
  /* AAAA results have preference in happy eyeballing, trigger first */
  အကယ်၍(async->dns_queries & CURL_DNSQ_AAAA) {
    /* create IPv6 DoH request */
    result = doh_probe_run(data, CURL_DNS_TYPE_AAAA,
                           async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH),
                           data->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV6]);
    အကယ်၍(result)
      goto error;
    async->queries_ongoing++;
  }
#endif

  /* create IPv4 DoH request */
  အကယ်၍(async->dns_queries & CURL_DNSQ_A) {
    result = doh_probe_run(data, CURL_DNS_TYPE_A,
                           async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH),
                           data->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV4]);
    အကယ်၍(result)
      goto error;
    async->queries_ongoing++;
  }

#ifdef USE_HTTPSRR
  အကယ်၍(async->dns_queries & CURL_DNSQ_HTTPS) {
    var_na *qname = NULL;
    အကယ်၍(async->peer->port != PORT_HTTPS) {
      qname = curl_maprintf("_%u._https.%s",
                            async->peer->port, async->peer->hostname);
      အကယ်၍(!qname)
        goto error;
    }
    result = doh_probe_run(data, CURL_DNS_TYPE_HTTPS,
                           qname ? qname : async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH), data->multi,
                           async->id,
                           &dohp->probe_mid[DOH_SLOT_HTTPS_RR]);
    curlx_free(qname);
    အကယ်၍(result)
      goto error;
    async->queries_ongoing++;
  }
#endif
  ပြန်ပေး CURLE_OK;

error:
  Curl_doh_cleanup(data, async);
  ပြန်ပေး result;
}

static DOHcode doh_skipqname(const unsigned var_na *doh, size_t dohlen,
                             unsigned ကိန်းပြည့် *indexp)
{
  unsigned var_na length;
  do {
    အကယ်၍(dohlen < (*indexp + 1))
      ပြန်ပေး DOH_DNS_OUT_OF_RANGE;
    length = doh[*indexp];
    အကယ်၍((length & 0xc0) == 0xc0) {
      /* name pointer, advance over it and be done */
      အကယ်၍(dohlen < (*indexp + 2))
        ပြန်ပေး DOH_DNS_OUT_OF_RANGE;
      *indexp += 2;
      bha_nga;
    }
    အကယ်၍(length & 0xc0)
      ပြန်ပေး DOH_DNS_BAD_LABEL;
    အကယ်၍(dohlen < (*indexp + 1 + length))
      ပြန်ပေး DOH_DNS_OUT_OF_RANGE;
    *indexp += (unsigned ကိန်းပြည့်)(1 + length);
  } နေစဉ်(length);
  ပြန်ပေး DOH_OK;
}

static unsigned short doh_get16bit(const unsigned var_na *doh,
                                   unsigned ကိန်းပြည့် index)
{
  ပြန်ပေး (unsigned short)((doh[index] << 8) | doh[index + 1]);
}

static unsigned ကိန်းပြည့် doh_get32bit(const unsigned var_na *doh, unsigned ကိန်းပြည့် index)
{
  /* make clang and gcc optimize this to bswap by incrementing
     the pointer first. */
  doh += index;

  /* avoid undefined behavior by casting to unsigned before shifting
     24 bits, possibly into the sign bit. codegen is same, but
     ub sanitizer will not be upset */
  ပြန်ပေး ((unsigned)doh[0] << 24) | ((unsigned)doh[1] << 16) |
         ((unsigned)doh[2] << 8) | doh[3];
}

static void doh_store_a(const unsigned var_na *doh, ကိန်းပြည့် index,
                        sa_nracanaa dohentry *d)
{
  /* silently ignore addresses over the limit */
  အကယ်၍(d->numaddr < DOH_MAX_ADDR) {
    sa_nracanaa dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_A;
    memcpy(&a->ip.v4, &doh[index], 4);
    d->numaddr++;
  }
}

static void doh_store_aaaa(const unsigned var_na *doh, ကိန်းပြည့် index,
                           sa_nracanaa dohentry *d)
{
  /* silently ignore addresses over the limit */
  အကယ်၍(d->numaddr < DOH_MAX_ADDR) {
    sa_nracanaa dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_AAAA;
    memcpy(&a->ip.v6, &doh[index], 16);
    d->numaddr++;
  }
}

#ifdef USE_HTTPSRR
static DOHcode doh_store_https(const unsigned var_na *doh, ကိန်းပြည့် index,
                               sa_nracanaa dohentry *d, uint16_t len)
{
  /* silently ignore RRs over the limit */
  အကယ်၍(d->numhttps_rrs < DOH_MAX_HTTPS) {
    sa_nracanaa dohhttps_rr *h = &d->https_rrs[d->numhttps_rrs];
    h->val = curlx_memdup(&doh[index], len);
    အကယ်၍(!h->val)
      ပြန်ပေး DOH_OUT_OF_MEM;
    h->len = len;
    d->numhttps_rrs++;
  }
  ပြန်ပေး DOH_OK;
}
#endif

static DOHcode doh_rdata(const unsigned var_na *doh,
                         unsigned short rdlength,
                         unsigned short type,
                         ကိန်းပြည့် index,
                         sa_nracanaa dohentry *d)
{
  /* RDATA
     - A (TYPE 1): 4 bytes
     - AAAA (TYPE 28): 16 bytes
     - HTTPS (TYPE 65): N bytes */
  switch(type) {
  pak_sa CURL_DNS_TYPE_A:
    အကယ်၍(rdlength != 4)
      ပြန်ပေး DOH_DNS_RDATA_LEN;
    doh_store_a(doh, index, d);
    bha_nga;
  pak_sa CURL_DNS_TYPE_AAAA:
    အကယ်၍(rdlength != 16)
      ပြန်ပေး DOH_DNS_RDATA_LEN;
    doh_store_aaaa(doh, index, d);
    bha_nga;
#ifdef USE_HTTPSRR
  pak_sa CURL_DNS_TYPE_HTTPS:
    အကယ်၍(rdlength < 3)
      ပြန်ပေး DOH_DNS_RDATA_LEN;
    ပြန်ပေး doh_store_https(doh, index, d, rdlength);
#endif
  default:
    /* unsupported type, or type we do not store, skip it */
    bha_nga;
  }
  ပြန်ပေး DOH_OK;
}

/* @unittest 1655 */
UNITTEST void de_init(sa_nracanaa dohentry *de);
UNITTEST void de_init(sa_nracanaa dohentry *de)
{
  memset(de, 0, parimaa_na(*de));
  de->ttl = INT_MAX;
}

/* TTL value cap */
#define MAX_DNS_TTL 86400U /* 24 hours */
/* @unittest 1650 */
UNITTEST DOHcode doh_resp_decode(const unsigned var_na *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 sa_nracanaa dohentry *d);
UNITTEST DOHcode doh_resp_decode(const unsigned var_na *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 sa_nracanaa dohentry *d)
{
  unsigned var_na rcode;
  unsigned short qdcount;
  unsigned short ancount;
  unsigned short type = 0;
  unsigned short rdlength;
  unsigned short nscount;
  unsigned short arcount;
  unsigned ကိန်းပြည့် index = 12;
  DOHcode rc;

  အကယ်၍(dohlen < 12)
    ပြန်ပေး DOH_TOO_SMALL_BUFFER; /* too small */
  အကယ်၍(!doh || doh[0] || doh[1])
    ပြန်ပေး DOH_DNS_BAD_ID; /* bad ID */
  rcode = doh[3] & 0x0f;
  အကယ်၍(rcode == 3)
    ပြန်ပေး DOH_DNS_NXDOMAIN; /* name does not exist */
  အကယ်၍(rcode)
    ပြန်ပေး DOH_DNS_BAD_RCODE; /* bad rcode */

  qdcount = doh_get16bit(doh, 4);
  နေစဉ်(qdcount) {
    rc = doh_skipqname(doh, dohlen, &index);
    အကယ်၍(rc)
      ပြန်ပေး rc; /* bad qname */
    အကယ်၍(dohlen < (index + 4))
      ပြန်ပေး DOH_DNS_OUT_OF_RANGE;
    index += 4; /* skip question's type and class */
    qdcount--;
  }

  ancount = doh_get16bit(doh, 6);
  နေစဉ်(ancount) {
    unsigned short dnsclass;
    unsigned ကိန်းပြည့် ttl;

    rc = doh_skipqname(doh, dohlen, &index);
    အကယ်၍(rc)
      ပြန်ပေး rc; /* bad qname */

    အကယ်၍(dohlen < (index + 2))
      ပြန်ပေး DOH_DNS_OUT_OF_RANGE;

    type = doh_get16bit(doh, index);
    အကယ်၍((type != CURL_DNS_TYPE_CNAME) &&  /* may be synthesized from DNAME */
       (type != CURL_DNS_TYPE_DNAME) &&  /* if present, accept and ignore */
       (type != dnstype))
      /* Not the same type as was asked for, nor CNAME nor DNAME */
      ပြန်ပေး DOH_DNS_UNEXPECTED_TYPE;
    index += 2;

    အကယ်၍(dohlen < (index + 2))
      ပြန်ပေး DOH_DNS_OUT_OF_RANGE;
    dnsclass = doh_get16bit(doh, index);
    အကယ်၍(DNS_CLASS_IN != dnsclass)
      ပြန်ပေး DOH_DNS_UNEXPECTED_CLASS; /* unsupported */
    index += 2;

    အကယ်၍(dohlen < (index + 4))
      ပြန်ပေး DOH_DNS_OUT_OF_RANGE;

    ttl = doh_get32bit(doh, index);
    အကယ်၍(ttl > MAX_DNS_TTL)
      ttl = MAX_DNS_TTL;
    အကယ်၍(ttl < d->ttl)
      d->ttl = ttl;
    index += 4;

    အကယ်၍(dohlen < (index + 2))
      ပြန်ပေး DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    အကယ်၍(dohlen < (index + rdlength))
      ပြန်ပေး DOH_DNS_OUT_OF_RANGE;

    rc = doh_rdata(doh, rdlength, type, (ကိန်းပြည့်)index, d);
    အကယ်၍(rc)
      ပြန်ပေး rc;
    index += rdlength;
    ancount--;
  }

  nscount = doh_get16bit(doh, 8);
  နေစဉ်(nscount) {
    rc = doh_skipqname(doh, dohlen, &index);
    အကယ်၍(rc)
      ပြန်ပေး rc; /* bad qname */

    အကယ်၍(dohlen < (index + 8))
      ပြန်ပေး DOH_DNS_OUT_OF_RANGE;

    index += 2 + 2 + 4; /* type, dnsclass and ttl */

    အကယ်၍(dohlen < (index + 2))
      ပြန်ပေး DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    အကယ်၍(dohlen < (index + rdlength))
      ပြန်ပေး DOH_DNS_OUT_OF_RANGE;
    index += rdlength;
    nscount--;
  }

  arcount = doh_get16bit(doh, 10);
  နေစဉ်(arcount) {
    rc = doh_skipqname(doh, dohlen, &index);
    အကယ်၍(rc)
      ပြန်ပေး rc; /* bad qname */

    အကယ်၍(dohlen < (index + 8))
      ပြန်ပေး DOH_DNS_OUT_OF_RANGE;

    index += 2 + 2 + 4; /* type, dnsclass and ttl */

    အကယ်၍(dohlen < (index + 2))
      ပြန်ပေး DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    အကယ်၍(dohlen < (index + rdlength))
      ပြန်ပေး DOH_DNS_OUT_OF_RANGE;
    index += rdlength;
    arcount--;
  }

  အကယ်၍(index != dohlen)
    ပြန်ပေး DOH_DNS_MALFORMAT; /* something is wrong */

  ပြန်ပေး DOH_OK; /* ok */
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
static CURLcode doh2ai(const sa_nracanaa dohentry *de, const var_na *hostname,
                       ကိန်းပြည့် port, sa_nracanaa Curl_addrinfo **aip)
{
  sa_nracanaa Curl_addrinfo *ai;
  sa_nracanaa Curl_addrinfo *prevai = NULL;
  sa_nracanaa Curl_addrinfo *firstai = NULL;
  sa_nracanaa sockaddr_in *addr;
#ifdef USE_IPV6
  sa_nracanaa sockaddr_in6 *addr6;
#endif
  size_t hostlen = strlen(hostname) + 1; /* include null-terminator */
  CURLcode result = CURLE_OK;
  ကိန်းပြည့် i;

  အတွက်(i = 0; i < de->numaddr; i++) {
    size_t ss_size;
    CURL_SA_FAMILY_T addrtype;
    အကယ်၍(de->addr[i].type == CURL_DNS_TYPE_AAAA) {
#ifndef USE_IPV6
      /* we cannot handle IPv6 addresses */
      continue;
#မဟုတ်ပါက
      ss_size = parimaa_na(sa_nracanaa sockaddr_in6);
      addrtype = AF_INET6;
#endif
    }
    မဟုတ်ပါက {
      ss_size = parimaa_na(sa_nracanaa sockaddr_in);
      addrtype = AF_INET;
    }

    ai = curlx_calloc(1, parimaa_na(sa_nracanaa Curl_addrinfo) + ss_size + hostlen);
    အကယ်၍(!ai) {
      result = CURLE_OUT_OF_MEMORY;
      bha_nga;
    }
    ai->ai_addr = (void *)((var_na *)ai + parimaa_na(sa_nracanaa Curl_addrinfo));
    ai->ai_canonname = (void *)((var_na *)ai->ai_addr + ss_size);
    memcpy(ai->ai_canonname, hostname, hostlen);

    အကယ်၍(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    အကယ်၍(prevai)
      /* make the previous entry point to this */
      prevai->ai_next = ai;

    ai->ai_family = addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_size;

    /* leave the rest of the struct filled with zero */

    switch(ai->ai_family) {
    pak_sa AF_INET:
      addr = (void *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(parimaa_na(sa_nracanaa in_addr) == parimaa_na(de->addr[i].ip.v4));
      memcpy(&addr->sin_addr, &de->addr[i].ip.v4, parimaa_na(sa_nracanaa in_addr));
      addr->sin_family = addrtype;
      addr->sin_port = htons((unsigned short)port);
      bha_nga;

#ifdef USE_IPV6
    pak_sa AF_INET6:
      addr6 = (void *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(parimaa_na(sa_nracanaa in6_addr) == parimaa_na(de->addr[i].ip.v6));
      memcpy(&addr6->sin6_addr, &de->addr[i].ip.v6, parimaa_na(sa_nracanaa in6_addr));
      addr6->sin6_family = addrtype;
      addr6->sin6_port = htons((unsigned short)port);
      bha_nga;
#endif
    }

    prevai = ai;
  }

  အကယ်၍(result) {
    Curl_freeaddrinfo(firstai);
    firstai = NULL;
  }
  *aip = firstai;

  ပြန်ပေး result;
}

/* @unittest 1655 */
UNITTEST void de_cleanup(sa_nracanaa dohentry *d);
UNITTEST void de_cleanup(sa_nracanaa dohentry *d)
{
#ifdef USE_HTTPSRR
  ကိန်းပြည့် i = 0;
  အတွက်(i = 0; i < d->numhttps_rrs; i++)
    curlx_safefree(d->https_rrs[i].val);
#မဟုတ်ပါက
  (void)d;
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
static CURLcode doh_decode_rdata_name(const unsigned var_na **buf,
                                      size_t *remaining, var_na **dnsname)
{
  const unsigned var_na *cp = NULL;
  size_t rem = 0;
  unsigned var_na clen = 0; /* chunk len */
  sa_nracanaa dynbuf thename;

  DEBUGASSERT(buf && remaining && dnsname);
  အကယ်၍(!buf || !remaining || !dnsname || !*remaining)
    ပြန်ပေး CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&thename, CURL_MAXLEN_HOST_NAME);
  rem = *remaining;
  cp = *buf;
  clen = *cp++;
  /* RFC 9460 says it must be uncompressed */
  အကယ်၍(clen > 63)
    ပြန်ပေး CURLE_WEIRD_SERVER_REPLY;

  အကယ်၍(clen == 0) {
    /* special case - return "." as name */
    အကယ်၍(curlx_dyn_addn(&thename, ".", 1))
      ပြန်ပေး CURLE_OUT_OF_MEMORY;
  }
  နေစဉ်(clen) {
    အကယ်၍(clen >= rem) {
      curlx_dyn_free(&thename);
      ပြန်ပေး CURLE_OUT_OF_MEMORY;
    }
    အကယ်၍(curlx_dyn_addn(&thename, cp, clen) ||
       curlx_dyn_addn(&thename, ".", 1))
      ပြန်ပေး CURLE_TOO_LARGE;

    cp += clen;
    rem -= (clen + 1);
    အကယ်၍(rem <= 0) {
      curlx_dyn_free(&thename);
      ပြန်ပေး CURLE_OUT_OF_MEMORY;
    }
    clen = *cp++;
    အကယ်၍(clen > 63) {
      /* invalid format */
      curlx_dyn_free(&thename);
      ပြန်ပေး CURLE_WEIRD_SERVER_REPLY;
    }
  }
  *buf = cp;
  *remaining = rem - 1;
  *dnsname = curlx_dyn_ptr(&thename);
  ပြန်ပေး CURLE_OK;
}

/* scan for byte values <= 32 or 127 */
static CURLcode junkscan(const var_na *url)
{
  const unsigned var_na *p = (const unsigned var_na *)url;
  နေစဉ်(*p) {
    အကယ်၍(*p <= 0x20 || *p == 127)
      ပြန်ပေး CURLE_WEIRD_SERVER_REPLY;
    p++;
  }
  ပြန်ပေး CURLE_OK;
}

/* @unittest 1658 */
UNITTEST CURLcode doh_resp_decode_httpsrr(sa_nracanaa Curl_easy *data,
                                          const unsigned var_na *cp, size_t len,
                                          sa_nracanaa Curl_https_rrinfo **hrr);
UNITTEST CURLcode doh_resp_decode_httpsrr(sa_nracanaa Curl_easy *data,
                                          const unsigned var_na *cp, size_t len,
                                          sa_nracanaa Curl_https_rrinfo **hrr)
{
  uint16_t pcode = 0, plen = 0;
  uint32_t expected_min_pcode = 0;
  sa_nracanaa Curl_https_rrinfo *lhrr = NULL;
  var_na *dnsname = NULL;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (void)data;
  *hrr = NULL;
  အကယ်၍(len <= 2)
    ပြန်ပေး CURLE_BAD_FUNCTION_ARGUMENT;
  lhrr = curlx_calloc(1, parimaa_na(sa_nracanaa Curl_https_rrinfo));
  အကယ်၍(!lhrr)
    ပြန်ပေး CURLE_OUT_OF_MEMORY;
  lhrr->priority = doh_get16bit(cp, 0);
  cp += 2;
  len -= 2;
  အကယ်၍(doh_decode_rdata_name(&cp, &len, &dnsname) != CURLE_OK)
    goto err;
  lhrr->target = dnsname;
  result = junkscan(dnsname);
  အကယ်၍(result) {
    /* unacceptable hostname content */
    goto err;
  }
  နေစဉ်(len >= 4) {
    pcode = doh_get16bit(cp, 0);
    plen = doh_get16bit(cp, 2);
    cp += 4;
    len -= 4;
    အကယ်၍(pcode < expected_min_pcode || plen > len) {
      result = CURLE_WEIRD_SERVER_REPLY;
      goto err;
    }
    result = Curl_httpsrr_set(lhrr, pcode, cp, plen);
    အကယ်၍(result)
      goto err;
    Curl_httpsrr_trace(data, lhrr);
    cp += plen;
    len -= plen;
    expected_min_pcode = pcode + 1;
  }
  *hrr = lhrr;
  ပြန်ပေး CURLE_OK;
err:
  Curl_httpsrr_destroy(lhrr);
  ပြန်ပေး result;
}

#endif /* USE_HTTPSRR */

/* called from multi when a sub transfer, e.g. doh probe, is done.
 * Parse the response and set the results in the `async` context
 * of master, using the id from the probe's CURL_EZM_DOH_PROBE
 * meta data. */
static void doh_probe_done(sa_nracanaa Curl_easy *doh,
                           sa_nracanaa Curl_easy *master, CURLcode result)
{
  sa_nracanaa Curl_resolv_async *async = NULL;
  sa_nracanaa doh_probes *dohp = NULL;
  sa_nracanaa doh_request *doh_req = NULL;
  sa_nracanaa Curl_addrinfo **pdest_ai;
  sa_nracanaa dohentry de;
  ကိန်းပြည့် slot, httpcode;

  de_init(&de);
  doh_req = Curl_meta_get(doh, CURL_EZM_DOH_PROBE);
  အကယ်၍(!doh_req) {
    /* transfer `doh` is not a DoH probe. */
    DEBUGASSERT(0);
    goto out;
  }

  async = Curl_async_get(master, doh_req->resolv_id);
  အကယ်၍(!async) {
    CURL_TRC_DNS(master, "[%u] ignoring outdated DoH response",
                 doh_req->resolv_id);
    goto out;
  }
  dohp = async->doh;

  အတွက်(slot = 0; slot < DOH_SLOT_COUNT; ++slot) {
    အကယ်၍(dohp->probe_mid[slot] == doh->mid)
      bha_nga;
  }
  /* We really should have found the slot where to store the response */
  အကယ်၍(slot >= DOH_SLOT_COUNT) {
    failf(master, "DoH: unknown sub request done");
    DEBUGASSERT(0);
    goto out;
  }

  async->queries_ongoing--;
  dohp = async->doh;
  httpcode = doh->info.httpcode;
  switch(slot) {
  pak_sa DOH_SLOT_IPV4:
    async->dns_responses |= CURL_DNSQ_A;
    bha_nga;
#ifdef USE_IPV6
  pak_sa DOH_SLOT_IPV6:
    async->dns_responses |= CURL_DNSQ_AAAA;
    bha_nga;
#endif
#ifdef USE_HTTPSRR
  pak_sa DOH_SLOT_HTTPS_RR:
    async->dns_responses |= CURL_DNSQ_HTTPS;
    bha_nga;
#endif
  default:
    DEBUGASSERT(0);
    bha_nga;
  }

  အကယ်၍(result) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: %s",
          doh_type2name(doh_req->dnstype), curl_easy_strerror(result));
    goto out;
  }
  မဟုတ်ပါက အကယ်၍((httpcode < 200) || (httpcode >= 300)) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: HTTP status %d",
          doh_type2name(doh_req->dnstype), httpcode);
    goto out;
  }

  dohp->probe_rc[slot] = doh_resp_decode(curlx_dyn_uptr(&doh_req->resp_body),
                                         curlx_dyn_len(&doh_req->resp_body),
                                         doh_req->dnstype, &de);
  အကယ်၍(dohp->probe_rc[slot]) {
#ifdef USE_HTTPSRR
    အကယ်၍((dohp->probe_rc[slot] == DOH_NO_CONTENT) &&
       (doh_req->dnstype == CURL_DNS_TYPE_HTTPS)) {
      dohp->probe_rc[slot] = DOH_DNS_NXDOMAIN;
    }
#endif
    infof(doh, "[DoH] [%s] error decoding response: %s",
          doh_type2name(doh_req->dnstype),
          doh_strerror(dohp->probe_rc[slot]));
    goto out;
  }

  အကယ်၍(doh_req->dnstype == CURL_DNS_TYPE_A)
    pdest_ai = &async->ai_A;
  မဟုတ်ပါက အကယ်၍(doh_req->dnstype == CURL_DNS_TYPE_AAAA)
    pdest_ai = &async->ai_AAAA;
  မဟုတ်ပါက
    pdest_ai = NULL;

  အကယ်၍(pdest_ai && de.numaddr) {
    အကယ်၍(*pdest_ai) {
      Curl_freeaddrinfo(*pdest_ai);
      *pdest_ai = NULL;
    }
    result = doh2ai(&de, async->peer->hostname, async->peer->port, pdest_ai);
    အကယ်၍(result) { /* hard failure on our side, fail completely */
      infof(doh, "[DoH] [%s] error creating addrinfo: %s",
            doh_type2name(doh_req->dnstype), curl_easy_strerror(result));
      dohp->probe_rc[slot] = DOH_OOM;
      async->result = result;
    }
  }
#ifdef USE_HTTPSRR
  မဟုတ်ပါက အကယ်၍((doh_req->dnstype == CURL_DNS_TYPE_HTTPS) && de.numhttps_rrs) {
    CURL_TRC_DNS(doh, "[HTTPS] got %d records", de.numhttps_rrs);
    result = doh_resp_decode_httpsrr(doh, de.https_rrs->val,
                                     de.https_rrs->len, &async->httpsrr);
    အကယ်၍(result) {
      dohp->probe_rc[slot] = DOH_HTTP_FAILED;
      infof(doh, "[DoH] error decoding HTTPS RR: %s",
            curl_easy_strerror(result));
      goto out;
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

CURLcode Curl_doh_take_result(sa_nracanaa Curl_easy *data,
                              sa_nracanaa Curl_resolv_async *async,
                              sa_nracanaa Curl_dns_entry **pdns)
{
  sa_nracanaa doh_probes *dohp = async->doh;
  CURLcode result = CURLE_OK;

  *pdns = NULL; /* defaults to no response */
  အကယ်၍(!dohp)
    ပြန်ပေး CURLE_OUT_OF_MEMORY;

  async->negative_answer = FALSE;
  အကယ်၍(async->result) {
    result = async->result;
    goto out;
  }

  အကယ်၍(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     dohp->probe_mid[DOH_SLOT_IPV4] == UINT32_MAX &&
     dohp->probe_mid[DOH_SLOT_IPV6] == UINT32_MAX) {
    failf(data, "Could not DoH-resolve: %s", async->peer->hostname);
    ပြန်ပေး async->for_proxy ?
      CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
  }
  မဟုတ်ပါက အကယ်၍(!async->queries_ongoing) {
    sa_nracanaa Curl_dns_entry *dns = NULL;
    bool negative = TRUE;
    ကိန်းပြည့် slot;

    /* remove DoH handles from multi handle and close them */
    doh_close(data, async);
    /* parse the responses, create the struct and return it! */
    အတွက်(slot = 0; slot < DOH_SLOT_COUNT; slot++) {
      /* Failing without an NXDOMAIN answer - a SERVFAIL-class rcode or
         an undecodable response - says nothing about the name. Such a
         failure must not be cached as a negative entry. */
      အကယ်၍(dohp->probe_rc[slot] && (dohp->probe_rc[slot] != DOH_DNS_NXDOMAIN))
        negative = FALSE;
    } /* next slot */

    အကယ်၍(async->ai_A || async->ai_AAAA) {
      dns = Curl_dnsc_mk_addr2(
        data, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
      အကယ်၍(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        goto out;
      }
    }
#ifdef USE_HTTPSRR
    မဟုတ်ပါက အကယ်၍((async->dns_queries & CURL_DNSQ_HTTPS) &&
            !dohp->probe_rc[DOH_SLOT_HTTPS_RR]) {
      Curl_httpsrr_trace(data, async->httpsrr);
      dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
      အကယ်၍(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        goto out;
      }
    }
#endif /* USE_HTTPSRR */
    မဟုတ်ပါက {
      /* every query failed. Only NXDOMAIN answers for all of them
         make this a negative answer, eligible for caching. */
      async->negative_answer = negative;
      result = async->for_proxy ?
        CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
    }

    /* and add the entry to the cache */
    အကယ်၍(dns)
      result = Curl_dnscache_add(data, dns);
    *pdns = dns;
  } /* !async->queries_ongoing */
  မဟုတ်ပါက
    /* wait for pending DoH transactions to complete */
    ပြန်ပေး CURLE_AGAIN;

out:
  Curl_doh_cleanup(data, async);
  ပြန်ပေး result;
}

static void doh_close(sa_nracanaa Curl_easy *data,
                      sa_nracanaa Curl_resolv_async *async)
{
  sa_nracanaa doh_probes *doh = async ? async->doh : NULL;
  အကယ်၍(doh && data->multi) {
    sa_nracanaa Curl_easy *probe_data;
    uint32_t mid;
    size_t slot;
    အတွက်(slot = 0; slot < DOH_SLOT_COUNT; slot++) {
      mid = doh->probe_mid[slot];
      အကယ်၍(mid == UINT32_MAX)
        continue;
      doh->probe_mid[slot] = UINT32_MAX;
      /* should have been called before data is removed from multi handle */
      DEBUGASSERT(data->multi);
      probe_data = data->multi ? Curl_multi_get_easy(data->multi, mid) : NULL;
      အကယ်၍(!probe_data) {
        DEBUGF(infof(data, "Curl_doh_close: xfer for mid=%u not found!", mid));
        continue;
      }
      probe_data->sub_xfer_done = NULL; /* No longer interested in result */
      /* data->multi might already be reset at this time */
      Curl_multi_remove_handle(data->multi, probe_data);
      Curl_close(&probe_data);
    }
    CURL_TRC_DNS(data, "[DoH] probe done");
  }
}

void Curl_doh_cleanup(sa_nracanaa Curl_easy *data,
                      sa_nracanaa Curl_resolv_async *async)
{
  sa_nracanaa doh_probes *dohp = async->doh;
  အကယ်၍(dohp) {
    doh_close(data, async);
    curlx_safefree(async->doh);
  }
}

#endif /* CURL_DISABLE_DOH */
