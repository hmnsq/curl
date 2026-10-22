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
#അന്തര്ഭാവ "curl_setup.h"

#ifndef CURL_DISABLE_DOH

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "curl_addrinfo.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "vdns/doh.h"
#അന്തര്ഭാവ "vdns/httpsrr.h"
#അന്തര്ഭാവ "curlx/strdup.h"
#അന്തര്ഭാവ "curlx/dynbuf.h"
#അന്തര്ഭാവ "escape.h"  /* for Curl_hexencode() */
#അന്തര്ഭാവ "urlapi-int.h"

#define DNS_CLASS_IN 0x01

സ്ഥിതം ശൂന്യം doh_close(ഘടന Curl_easy *data,
                      ഘടന Curl_resolv_async *async);

#ifdef CURLVERBOSE
സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം doh_code_str[] = {
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

സ്ഥിതം സ്ഥിരം അക്ഷരം *doh_strerror(DOHcode code)
{
  എങ്കിൽ((size_t)code < CURL_ARRAYSIZE(doh_code_str))
    തിരികെ doh_code_str[code];
  തിരികെ "bad error code";
}

സ്ഥിതം സ്ഥിരം അക്ഷരം *doh_type2name(DNStype dnstype)
{
  തിരഞ്ഞെടുപ്പ്(dnstype) {
  അവസ്ഥ CURL_DNS_TYPE_A:
    തിരികെ "A";
  അവസ്ഥ CURL_DNS_TYPE_AAAA:
    തിരികെ "AAAA";
#ifdef USE_HTTPSRR
  അവസ്ഥ CURL_DNS_TYPE_HTTPS:
    തിരികെ "HTTPS";
#endif
  സ്ഥിരസ്ഥിതി:
    തിരികെ "unknown";
  }
}

#endif /* CURLVERBOSE */

/* @unittest 1655
 */
UNITTEST DOHcode doh_req_encode(സ്ഥിരം അക്ഷരം *host,
                                DNStype dnstype,
                                unsigned അക്ഷരം *dnsp,  /* buffer */
                                size_t len,  /* buffer size */
                                size_t *olen);  /* output length */
UNITTEST DOHcode doh_req_encode(സ്ഥിരം അക്ഷരം *host,
                                DNStype dnstype,
                                unsigned അക്ഷരം *dnsp, /* buffer */
                                size_t len,   /* buffer size */
                                size_t *olen) /* output length */
{
  സ്ഥിരം size_t hostlen = strlen(host);
  unsigned അക്ഷരം *orig = dnsp;
  സ്ഥിരം അക്ഷരം *hostp = host;

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
  എങ്കിൽ(host[hostlen - 1] != '.')
    expected_len++;

  എങ്കിൽ(expected_len > DOH_MAX_DNSREQ_SIZE)
    തിരികെ DOH_DNS_NAME_TOO_LONG;

  എങ്കിൽ(len < expected_len)
    തിരികെ DOH_TOO_SMALL_BUFFER;

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
  ആകുമ്പോൾ(*hostp) {
    size_t labellen;
    സ്ഥിരം അക്ഷരം *dot = strchr(hostp, '.');
    എങ്കിൽ(dot)
      labellen = dot - hostp;
    അല്ലെങ്കിൽ
      labellen = strlen(hostp);
    എങ്കിൽ((labellen > 63) || (!labellen)) {
      /* label is too long or too short, error out */
      *olen = 0;
      തിരികെ DOH_DNS_BAD_LABEL;
    }
    /* label is non-empty, process it */
    *dnsp++ = (unsigned അക്ഷരം)labellen;
    memcpy(dnsp, hostp, labellen);
    dnsp += labellen;
    hostp += labellen;
    /* advance past dot, but only if there is one */
    എങ്കിൽ(dot)
      hostp++;
  } /* next label */

  *dnsp++ = 0; /* append zero-length label for root */

  /* There are assigned TYPE codes beyond 255: use range [1..65535] */
  *dnsp++ = (unsigned അക്ഷരം)(255 & (dnstype >> 8)); /* upper 8-bit TYPE */
  *dnsp++ = (unsigned അക്ഷരം)(255 & dnstype);        /* lower 8-bit TYPE */

  *dnsp++ = '\0'; /* upper 8-bit CLASS */
  *dnsp++ = DNS_CLASS_IN; /* IN - "the Internet" */

  *olen = dnsp - orig;

  /* verify that our estimation of length is valid, since
   * this has led to buffer overflows in this function */
  DEBUGASSERT(*olen == expected_len);
  തിരികെ DOH_OK;
}

സ്ഥിതം size_t doh_probe_write_cb(അക്ഷരം *contents, size_t size, size_t nmemb,
                                 ശൂന്യം *userp)
{
  size_t realsize = size * nmemb;
  ഘടന Curl_easy *data = userp;
  ഘടന doh_request *doh_req = Curl_meta_get(data, CURL_EZM_DOH_PROBE);
  എങ്കിൽ(!doh_req)
    തിരികെ CURL_WRITEFUNC_ERROR;

  എങ്കിൽ(curlx_dyn_addn(&doh_req->resp_body, contents, realsize))
    തിരികെ 0;

  തിരികെ realsize;
}

സ്ഥിതം ശൂന്യം doh_probe_done(ഘടന Curl_easy *doh,
                           ഘടന Curl_easy *master, CURLcode result);
സ്ഥിതം ശൂന്യം doh_probe_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t klen, ശൂന്യം *e)
{
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)klen;
  എങ്കിൽ(e) {
    ഘടന doh_request *doh_req = e;
    curl_slist_free_all(doh_req->req_hds);
    curlx_dyn_free(&doh_req->resp_body);
    curlx_free(e);
  }
}

#define ERROR_CHECK_SETOPT(x, y)                        \
  ചെയ്യുക {                                                  \
    result = curl_easy_setopt((CURL *)doh, x, y);       \
    എങ്കിൽ(result &&                                        \
       result != CURLE_NOT_BUILT_IN &&                  \
       result != CURLE_UNKNOWN_OPTION)                  \
      പോകുക error;                                       \
  } ആകുമ്പോൾ(0)

സ്ഥിതം CURLcode doh_probe_run(ഘടന Curl_easy *data,
                              DNStype dnstype,
                              സ്ഥിരം അക്ഷരം *host,
                              സ്ഥിരം അക്ഷരം *url, CURLM *multi,
                              uint32_t resolv_id,
                              uint32_t *pmid)
{
  ഘടന Curl_easy *doh = ശൂന്യ;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  ഘടന doh_request *doh_req;
  DOHcode d;
  bool maybe_https = !curl_strnequal(url, STRCONST("http:"));

  *pmid = UINT32_MAX;

  doh_req = curlx_calloc(1, വലുപ്പം(*doh_req));
  എങ്കിൽ(!doh_req)
    തിരികെ CURLE_OUT_OF_MEMORY;
  doh_req->resolv_id = resolv_id;
  doh_req->dnstype = dnstype;
  curlx_dyn_init(&doh_req->resp_body, DYN_DOH_RESPONSE);

  d = doh_req_encode(host, dnstype, doh_req->req_body,
                     വലുപ്പം(doh_req->req_body),
                     &doh_req->req_body_len);
  എങ്കിൽ(d) {
    failf(data, "Failed to encode DoH packet [%d]", (പൂർണ്ണസംഖ്യ)d);
    result = CURLE_OUT_OF_MEMORY;
    പോകുക error;
  }

  timeout_ms = Curl_timeleft_ms(data);
  എങ്കിൽ(timeout_ms < 0) {
    result = CURLE_OPERATION_TIMEDOUT;
    പോകുക error;
  }

  doh_req->req_hds =
    curl_slist_append(ശൂന്യ, "Content-Type: application/dns-message");
  എങ്കിൽ(!doh_req->req_hds) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക error;
  }

  /* Curl_open() is the internal version of curl_easy_init() */
  result = Curl_open(&doh);
  എങ്കിൽ(result)
    പോകുക error;

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
  എങ്കിൽ(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);
    ERROR_CHECK_SETOPT(CURLOPT_PIPEWAIT, 1L);
  }
#endif
#ifndef DEBUGBUILD
  /* enforce HTTPS if not debug */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
#അല്ലെങ്കിൽ
  /* in debug mode, also allow http */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
#endif
  ERROR_CHECK_SETOPT(CURLOPT_TIMEOUT_MS, (long)timeout_ms);
  ERROR_CHECK_SETOPT(CURLOPT_SHARE, (CURLSH *)data->share);
  എങ്കിൽ(data->set.err && data->set.err != stderr)
    ERROR_CHECK_SETOPT(CURLOPT_STDERR, data->set.err);
  എങ്കിൽ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_doh))
    ERROR_CHECK_SETOPT(CURLOPT_VERBOSE, 1L);
  എങ്കിൽ(data->set.no_signal)
    ERROR_CHECK_SETOPT(CURLOPT_NOSIGNAL, 1L);
  എങ്കിൽ(data->set.fdebug)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGFUNCTION, data->set.fdebug);
  എങ്കിൽ(data->set.debugdata)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGDATA, data->set.debugdata);

#ifdef USE_RESOLV_ARES
  എങ്കിൽ(CURL_EASY_STR(data, STRING_DNS_SERVERS)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_SERVERS,
                       CURL_EASY_STR(data, STRING_DNS_SERVERS));
  }
  എങ്കിൽ(CURL_EASY_STR(data, STRING_DNS_INTERFACE)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_INTERFACE,
                       CURL_EASY_STR(data, STRING_DNS_INTERFACE));
  }
  എങ്കിൽ(CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP4,
                       CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4));
  }
  എങ്കിൽ(CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP6,
                       CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6));
  }
#endif
  എങ്കിൽ(maybe_https) {
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
    എങ്കിൽ(CURL_EASY_STR(data, STRING_SSL_CAFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO,
                         CURL_EASY_STR(data, STRING_SSL_CAFILE));
    }
    എങ്കിൽ(data->set.blobs[BLOB_CAINFO]) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO_BLOB, data->set.blobs[BLOB_CAINFO]);
    }
    എങ്കിൽ(CURL_EASY_STR(data, STRING_SSL_CAPATH)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAPATH,
                         CURL_EASY_STR(data, STRING_SSL_CAPATH));
    }
    എങ്കിൽ(CURL_EASY_STR(data, STRING_SSL_CRLFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CRLFILE,
                         CURL_EASY_STR(data, STRING_SSL_CRLFILE));
    }
    എങ്കിൽ(data->set.ssl.certinfo)
      ERROR_CHECK_SETOPT(CURLOPT_CERTINFO, 1L);
    എങ്കിൽ(data->set.ssl_fsslctx)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_FUNCTION, data->set.ssl_fsslctx);
    എങ്കിൽ(data->set.ssl_fsslctxp)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_DATA, data->set.ssl_fsslctxp);
    എങ്കിൽ(CURL_EASY_STR(data, STRING_SSL_EC_CURVES)) {
      ERROR_CHECK_SETOPT(CURLOPT_SSL_EC_CURVES,
                         CURL_EASY_STR(data, STRING_SSL_EC_CURVES));
    }
    (ശൂന്യം)curl_easy_setopt(doh, CURLOPT_SSL_OPTIONS,
                           ((long)data->set.ssl.ssl_options &
                            ~CURLSSLOPT_AUTO_CLIENT_CERT));
  }

  doh->state.internal = TRUE;
  doh->master_mid = data->mid; /* master transfer of this one */
  doh->sub_xfer_done = doh_probe_done;

  result = Curl_meta_set(doh, CURL_EZM_DOH_PROBE, doh_req, doh_probe_dtor);
  doh_req = ശൂന്യ; /* call took ownership */
  എങ്കിൽ(result)
    പോകുക error;

  /* DoH handles must not inherit private_data. The handles may be passed to
     the user via callbacks and the user will be able to identify them as
     internal handles because private data is not set. The user can then set
     private_data via CURLOPT_PRIVATE if they so choose. */
  DEBUGASSERT(!doh->set.private_data);

  എങ്കിൽ(Curl_multi_add_handle(multi, doh))
    പോകുക error;

  *pmid = doh->mid;
  തിരികെ CURLE_OK;

error:
  Curl_close(&doh);
  എങ്കിൽ(doh_req)
    doh_probe_dtor(ശൂന്യ, 0, doh_req);
  തിരികെ result;
}

/*
 * Curl_doh() starts a name resolve using DoH. It resolves a name and returns
 * a 'Curl_addrinfo *' with the address information.
 */

CURLcode Curl_doh(ഘടന Curl_easy *data,
                  ഘടന Curl_resolv_async *async)
{
  CURLcode result = CURLE_OK;
  ഘടന doh_probes *dohp = ശൂന്യ;
  size_t i;

  DEBUGASSERT(!async->doh);
  DEBUGASSERT(async->peer->hostname[0]);
  എങ്കിൽ(async->doh) {
    DEBUGASSERT(0); /* should not happen */
    Curl_doh_cleanup(data, async);
  }

  എങ്കിൽ(!async->dns_queries)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef USE_HTTPSRR
  എങ്കിൽ(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     (async->dns_queries & CURL_DNSQ_HTTPS)) {
    /* Can't mix those in the same async resolve */
    DEBUGASSERT(0);
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  }
#അല്ലെങ്കിൽ
  എങ്കിൽ(async->dns_queries & CURL_DNSQ_HTTPS) {
    DEBUGASSERT(0);
    തിരികെ CURLE_NOT_BUILT_IN;
  }
#endif

  /* start clean, consider allocating this struct on demand */
  async->doh = dohp = curlx_calloc(1, വലുപ്പം(ഘടന doh_probes));
  എങ്കിൽ(!dohp)
    തിരികെ CURLE_OUT_OF_MEMORY;

  വേണ്ടി(i = 0; i < DOH_SLOT_എണ്ണം; ++i) {
    dohp->probe_rc[i] = DOH_OK;
    dohp->probe_mid[i] = UINT32_MAX;
  }

#ifdef USE_IPV6
  /* AAAA results have preference in happy eyeballing, trigger first */
  എങ്കിൽ(async->dns_queries & CURL_DNSQ_AAAA) {
    /* create IPv6 DoH request */
    result = doh_probe_run(data, CURL_DNS_TYPE_AAAA,
                           async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH),
                           data->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV6]);
    എങ്കിൽ(result)
      പോകുക error;
    async->queries_ongoing++;
  }
#endif

  /* create IPv4 DoH request */
  എങ്കിൽ(async->dns_queries & CURL_DNSQ_A) {
    result = doh_probe_run(data, CURL_DNS_TYPE_A,
                           async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH),
                           data->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV4]);
    എങ്കിൽ(result)
      പോകുക error;
    async->queries_ongoing++;
  }

#ifdef USE_HTTPSRR
  എങ്കിൽ(async->dns_queries & CURL_DNSQ_HTTPS) {
    അക്ഷരം *qname = ശൂന്യ;
    എങ്കിൽ(async->peer->port != PORT_HTTPS) {
      qname = curl_maprintf("_%u._https.%s",
                            async->peer->port, async->peer->hostname);
      എങ്കിൽ(!qname)
        പോകുക error;
    }
    result = doh_probe_run(data, CURL_DNS_TYPE_HTTPS,
                           qname ? qname : async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH), data->multi,
                           async->id,
                           &dohp->probe_mid[DOH_SLOT_HTTPS_RR]);
    curlx_free(qname);
    എങ്കിൽ(result)
      പോകുക error;
    async->queries_ongoing++;
  }
#endif
  തിരികെ CURLE_OK;

error:
  Curl_doh_cleanup(data, async);
  തിരികെ result;
}

സ്ഥിതം DOHcode doh_skipqname(സ്ഥിരം unsigned അക്ഷരം *doh, size_t dohlen,
                             unsigned പൂർണ്ണസംഖ്യ *indexp)
{
  unsigned അക്ഷരം length;
  ചെയ്യുക {
    എങ്കിൽ(dohlen < (*indexp + 1))
      തിരികെ DOH_DNS_OUT_OF_RANGE;
    length = doh[*indexp];
    എങ്കിൽ((length & 0xc0) == 0xc0) {
      /* name pointer, advance over it and be done */
      എങ്കിൽ(dohlen < (*indexp + 2))
        തിരികെ DOH_DNS_OUT_OF_RANGE;
      *indexp += 2;
      നിർത്തുക;
    }
    എങ്കിൽ(length & 0xc0)
      തിരികെ DOH_DNS_BAD_LABEL;
    എങ്കിൽ(dohlen < (*indexp + 1 + length))
      തിരികെ DOH_DNS_OUT_OF_RANGE;
    *indexp += (unsigned പൂർണ്ണസംഖ്യ)(1 + length);
  } ആകുമ്പോൾ(length);
  തിരികെ DOH_OK;
}

സ്ഥിതം unsigned short doh_get16bit(സ്ഥിരം unsigned അക്ഷരം *doh,
                                   unsigned പൂർണ്ണസംഖ്യ index)
{
  തിരികെ (unsigned short)((doh[index] << 8) | doh[index + 1]);
}

സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ doh_get32bit(സ്ഥിരം unsigned അക്ഷരം *doh, unsigned പൂർണ്ണസംഖ്യ index)
{
  /* make clang and gcc optimize this to bswap by incrementing
     the pointer first. */
  doh += index;

  /* avoid undefined behavior by casting to unsigned before shifting
     24 bits, possibly into the sign bit. codegen is same, but
     ub sanitizer will not be upset */
  തിരികെ ((unsigned)doh[0] << 24) | ((unsigned)doh[1] << 16) |
         ((unsigned)doh[2] << 8) | doh[3];
}

സ്ഥിതം ശൂന്യം doh_store_a(സ്ഥിരം unsigned അക്ഷരം *doh, പൂർണ്ണസംഖ്യ index,
                        ഘടന dohentry *d)
{
  /* silently ignore addresses over the limit */
  എങ്കിൽ(d->numaddr < DOH_MAX_ADDR) {
    ഘടന dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_A;
    memcpy(&a->ip.v4, &doh[index], 4);
    d->numaddr++;
  }
}

സ്ഥിതം ശൂന്യം doh_store_aaaa(സ്ഥിരം unsigned അക്ഷരം *doh, പൂർണ്ണസംഖ്യ index,
                           ഘടന dohentry *d)
{
  /* silently ignore addresses over the limit */
  എങ്കിൽ(d->numaddr < DOH_MAX_ADDR) {
    ഘടന dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_AAAA;
    memcpy(&a->ip.v6, &doh[index], 16);
    d->numaddr++;
  }
}

#ifdef USE_HTTPSRR
സ്ഥിതം DOHcode doh_store_https(സ്ഥിരം unsigned അക്ഷരം *doh, പൂർണ്ണസംഖ്യ index,
                               ഘടന dohentry *d, uint16_t len)
{
  /* silently ignore RRs over the limit */
  എങ്കിൽ(d->numhttps_rrs < DOH_MAX_HTTPS) {
    ഘടന dohhttps_rr *h = &d->https_rrs[d->numhttps_rrs];
    h->val = curlx_memdup(&doh[index], len);
    എങ്കിൽ(!h->val)
      തിരികെ DOH_OUT_OF_MEM;
    h->len = len;
    d->numhttps_rrs++;
  }
  തിരികെ DOH_OK;
}
#endif

സ്ഥിതം DOHcode doh_rdata(സ്ഥിരം unsigned അക്ഷരം *doh,
                         unsigned short rdlength,
                         unsigned short type,
                         പൂർണ്ണസംഖ്യ index,
                         ഘടന dohentry *d)
{
  /* RDATA
     - A (TYPE 1): 4 bytes
     - AAAA (TYPE 28): 16 bytes
     - HTTPS (TYPE 65): N bytes */
  തിരഞ്ഞെടുപ്പ്(type) {
  അവസ്ഥ CURL_DNS_TYPE_A:
    എങ്കിൽ(rdlength != 4)
      തിരികെ DOH_DNS_RDATA_LEN;
    doh_store_a(doh, index, d);
    നിർത്തുക;
  അവസ്ഥ CURL_DNS_TYPE_AAAA:
    എങ്കിൽ(rdlength != 16)
      തിരികെ DOH_DNS_RDATA_LEN;
    doh_store_aaaa(doh, index, d);
    നിർത്തുക;
#ifdef USE_HTTPSRR
  അവസ്ഥ CURL_DNS_TYPE_HTTPS:
    എങ്കിൽ(rdlength < 3)
      തിരികെ DOH_DNS_RDATA_LEN;
    തിരികെ doh_store_https(doh, index, d, rdlength);
#endif
  സ്ഥിരസ്ഥിതി:
    /* unsupported type, or type we do not store, skip it */
    നിർത്തുക;
  }
  തിരികെ DOH_OK;
}

/* @unittest 1655 */
UNITTEST ശൂന്യം de_init(ഘടന dohentry *de);
UNITTEST ശൂന്യം de_init(ഘടന dohentry *de)
{
  memset(de, 0, വലുപ്പം(*de));
  de->ttl = INT_MAX;
}

/* TTL value cap */
#define MAX_DNS_TTL 86400U /* 24 hours */
/* @unittest 1650 */
UNITTEST DOHcode doh_resp_decode(സ്ഥിരം unsigned അക്ഷരം *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 ഘടന dohentry *d);
UNITTEST DOHcode doh_resp_decode(സ്ഥിരം unsigned അക്ഷരം *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 ഘടന dohentry *d)
{
  unsigned അക്ഷരം rcode;
  unsigned short qdcount;
  unsigned short ancount;
  unsigned short type = 0;
  unsigned short rdlength;
  unsigned short nscount;
  unsigned short arcount;
  unsigned പൂർണ്ണസംഖ്യ index = 12;
  DOHcode rc;

  എങ്കിൽ(dohlen < 12)
    തിരികെ DOH_TOO_SMALL_BUFFER; /* too small */
  എങ്കിൽ(!doh || doh[0] || doh[1])
    തിരികെ DOH_DNS_BAD_ID; /* bad ID */
  rcode = doh[3] & 0x0f;
  എങ്കിൽ(rcode == 3)
    തിരികെ DOH_DNS_NXDOMAIN; /* name does not exist */
  എങ്കിൽ(rcode)
    തിരികെ DOH_DNS_BAD_RCODE; /* bad rcode */

  qdcount = doh_get16bit(doh, 4);
  ആകുമ്പോൾ(qdcount) {
    rc = doh_skipqname(doh, dohlen, &index);
    എങ്കിൽ(rc)
      തിരികെ rc; /* bad qname */
    എങ്കിൽ(dohlen < (index + 4))
      തിരികെ DOH_DNS_OUT_OF_RANGE;
    index += 4; /* skip question's type and class */
    qdcount--;
  }

  ancount = doh_get16bit(doh, 6);
  ആകുമ്പോൾ(ancount) {
    unsigned short dnsclass;
    unsigned പൂർണ്ണസംഖ്യ ttl;

    rc = doh_skipqname(doh, dohlen, &index);
    എങ്കിൽ(rc)
      തിരികെ rc; /* bad qname */

    എങ്കിൽ(dohlen < (index + 2))
      തിരികെ DOH_DNS_OUT_OF_RANGE;

    type = doh_get16bit(doh, index);
    എങ്കിൽ((type != CURL_DNS_TYPE_CNAME) &&  /* may be synthesized from DNAME */
       (type != CURL_DNS_TYPE_DNAME) &&  /* if present, accept and ignore */
       (type != dnstype))
      /* Not the same type as was asked for, nor CNAME nor DNAME */
      തിരികെ DOH_DNS_UNEXPECTED_TYPE;
    index += 2;

    എങ്കിൽ(dohlen < (index + 2))
      തിരികെ DOH_DNS_OUT_OF_RANGE;
    dnsclass = doh_get16bit(doh, index);
    എങ്കിൽ(DNS_CLASS_IN != dnsclass)
      തിരികെ DOH_DNS_UNEXPECTED_CLASS; /* unsupported */
    index += 2;

    എങ്കിൽ(dohlen < (index + 4))
      തിരികെ DOH_DNS_OUT_OF_RANGE;

    ttl = doh_get32bit(doh, index);
    എങ്കിൽ(ttl > MAX_DNS_TTL)
      ttl = MAX_DNS_TTL;
    എങ്കിൽ(ttl < d->ttl)
      d->ttl = ttl;
    index += 4;

    എങ്കിൽ(dohlen < (index + 2))
      തിരികെ DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    എങ്കിൽ(dohlen < (index + rdlength))
      തിരികെ DOH_DNS_OUT_OF_RANGE;

    rc = doh_rdata(doh, rdlength, type, (പൂർണ്ണസംഖ്യ)index, d);
    എങ്കിൽ(rc)
      തിരികെ rc;
    index += rdlength;
    ancount--;
  }

  nscount = doh_get16bit(doh, 8);
  ആകുമ്പോൾ(nscount) {
    rc = doh_skipqname(doh, dohlen, &index);
    എങ്കിൽ(rc)
      തിരികെ rc; /* bad qname */

    എങ്കിൽ(dohlen < (index + 8))
      തിരികെ DOH_DNS_OUT_OF_RANGE;

    index += 2 + 2 + 4; /* type, dnsclass and ttl */

    എങ്കിൽ(dohlen < (index + 2))
      തിരികെ DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    എങ്കിൽ(dohlen < (index + rdlength))
      തിരികെ DOH_DNS_OUT_OF_RANGE;
    index += rdlength;
    nscount--;
  }

  arcount = doh_get16bit(doh, 10);
  ആകുമ്പോൾ(arcount) {
    rc = doh_skipqname(doh, dohlen, &index);
    എങ്കിൽ(rc)
      തിരികെ rc; /* bad qname */

    എങ്കിൽ(dohlen < (index + 8))
      തിരികെ DOH_DNS_OUT_OF_RANGE;

    index += 2 + 2 + 4; /* type, dnsclass and ttl */

    എങ്കിൽ(dohlen < (index + 2))
      തിരികെ DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    എങ്കിൽ(dohlen < (index + rdlength))
      തിരികെ DOH_DNS_OUT_OF_RANGE;
    index += rdlength;
    arcount--;
  }

  എങ്കിൽ(index != dohlen)
    തിരികെ DOH_DNS_MALFORMAT; /* something is wrong */

  തിരികെ DOH_OK; /* ok */
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
സ്ഥിതം CURLcode doh2ai(സ്ഥിരം ഘടന dohentry *de, സ്ഥിരം അക്ഷരം *hostname,
                       പൂർണ്ണസംഖ്യ port, ഘടന Curl_addrinfo **aip)
{
  ഘടന Curl_addrinfo *ai;
  ഘടന Curl_addrinfo *prevai = ശൂന്യ;
  ഘടന Curl_addrinfo *firstai = ശൂന്യ;
  ഘടന sockaddr_in *addr;
#ifdef USE_IPV6
  ഘടന sockaddr_in6 *addr6;
#endif
  size_t hostlen = strlen(hostname) + 1; /* include null-terminator */
  CURLcode result = CURLE_OK;
  പൂർണ്ണസംഖ്യ i;

  വേണ്ടി(i = 0; i < de->numaddr; i++) {
    size_t ss_size;
    CURL_SA_FAMILY_T addrtype;
    എങ്കിൽ(de->addr[i].type == CURL_DNS_TYPE_AAAA) {
#ifndef USE_IPV6
      /* we cannot handle IPv6 addresses */
      തുടരുക;
#അല്ലെങ്കിൽ
      ss_size = വലുപ്പം(ഘടന sockaddr_in6);
      addrtype = AF_INET6;
#endif
    }
    അല്ലെങ്കിൽ {
      ss_size = വലുപ്പം(ഘടന sockaddr_in);
      addrtype = AF_INET;
    }

    ai = curlx_calloc(1, വലുപ്പം(ഘടന Curl_addrinfo) + ss_size + hostlen);
    എങ്കിൽ(!ai) {
      result = CURLE_OUT_OF_MEMORY;
      നിർത്തുക;
    }
    ai->ai_addr = (ശൂന്യം *)((അക്ഷരം *)ai + വലുപ്പം(ഘടന Curl_addrinfo));
    ai->ai_canonname = (ശൂന്യം *)((അക്ഷരം *)ai->ai_addr + ss_size);
    memcpy(ai->ai_canonname, hostname, hostlen);

    എങ്കിൽ(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    എങ്കിൽ(prevai)
      /* make the previous entry point to this */
      prevai->ai_next = ai;

    ai->ai_family = addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_size;

    /* leave the rest of the struct filled with zero */

    തിരഞ്ഞെടുപ്പ്(ai->ai_family) {
    അവസ്ഥ AF_INET:
      addr = (ശൂന്യം *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(വലുപ്പം(ഘടന in_addr) == വലുപ്പം(de->addr[i].ip.v4));
      memcpy(&addr->sin_addr, &de->addr[i].ip.v4, വലുപ്പം(ഘടന in_addr));
      addr->sin_family = addrtype;
      addr->sin_port = htons((unsigned short)port);
      നിർത്തുക;

#ifdef USE_IPV6
    അവസ്ഥ AF_INET6:
      addr6 = (ശൂന്യം *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(വലുപ്പം(ഘടന in6_addr) == വലുപ്പം(de->addr[i].ip.v6));
      memcpy(&addr6->sin6_addr, &de->addr[i].ip.v6, വലുപ്പം(ഘടന in6_addr));
      addr6->sin6_family = addrtype;
      addr6->sin6_port = htons((unsigned short)port);
      നിർത്തുക;
#endif
    }

    prevai = ai;
  }

  എങ്കിൽ(result) {
    Curl_freeaddrinfo(firstai);
    firstai = ശൂന്യ;
  }
  *aip = firstai;

  തിരികെ result;
}

/* @unittest 1655 */
UNITTEST ശൂന്യം de_cleanup(ഘടന dohentry *d);
UNITTEST ശൂന്യം de_cleanup(ഘടന dohentry *d)
{
#ifdef USE_HTTPSRR
  പൂർണ്ണസംഖ്യ i = 0;
  വേണ്ടി(i = 0; i < d->numhttps_rrs; i++)
    curlx_safefree(d->https_rrs[i].val);
#അല്ലെങ്കിൽ
  (ശൂന്യം)d;
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
സ്ഥിതം CURLcode doh_decode_rdata_name(സ്ഥിരം unsigned അക്ഷരം **buf,
                                      size_t *remaining, അക്ഷരം **dnsname)
{
  സ്ഥിരം unsigned അക്ഷരം *cp = ശൂന്യ;
  size_t rem = 0;
  unsigned അക്ഷരം clen = 0; /* chunk len */
  ഘടന dynbuf thename;

  DEBUGASSERT(buf && remaining && dnsname);
  എങ്കിൽ(!buf || !remaining || !dnsname || !*remaining)
    തിരികെ CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&thename, CURL_MAXLEN_HOST_NAME);
  rem = *remaining;
  cp = *buf;
  clen = *cp++;
  /* RFC 9460 says it must be uncompressed */
  എങ്കിൽ(clen > 63)
    തിരികെ CURLE_WEIRD_SERVER_REPLY;

  എങ്കിൽ(clen == 0) {
    /* special case - return "." as name */
    എങ്കിൽ(curlx_dyn_addn(&thename, ".", 1))
      തിരികെ CURLE_OUT_OF_MEMORY;
  }
  ആകുമ്പോൾ(clen) {
    എങ്കിൽ(clen >= rem) {
      curlx_dyn_free(&thename);
      തിരികെ CURLE_OUT_OF_MEMORY;
    }
    എങ്കിൽ(curlx_dyn_addn(&thename, cp, clen) ||
       curlx_dyn_addn(&thename, ".", 1))
      തിരികെ CURLE_TOO_LARGE;

    cp += clen;
    rem -= (clen + 1);
    എങ്കിൽ(rem <= 0) {
      curlx_dyn_free(&thename);
      തിരികെ CURLE_OUT_OF_MEMORY;
    }
    clen = *cp++;
    എങ്കിൽ(clen > 63) {
      /* invalid format */
      curlx_dyn_free(&thename);
      തിരികെ CURLE_WEIRD_SERVER_REPLY;
    }
  }
  *buf = cp;
  *remaining = rem - 1;
  *dnsname = curlx_dyn_ptr(&thename);
  തിരികെ CURLE_OK;
}

/* scan for byte values <= 32 or 127 */
സ്ഥിതം CURLcode junkscan(സ്ഥിരം അക്ഷരം *url)
{
  സ്ഥിരം unsigned അക്ഷരം *p = (സ്ഥിരം unsigned അക്ഷരം *)url;
  ആകുമ്പോൾ(*p) {
    എങ്കിൽ(*p <= 0x20 || *p == 127)
      തിരികെ CURLE_WEIRD_SERVER_REPLY;
    p++;
  }
  തിരികെ CURLE_OK;
}

/* @unittest 1658 */
UNITTEST CURLcode doh_resp_decode_httpsrr(ഘടന Curl_easy *data,
                                          സ്ഥിരം unsigned അക്ഷരം *cp, size_t len,
                                          ഘടന Curl_https_rrinfo **hrr);
UNITTEST CURLcode doh_resp_decode_httpsrr(ഘടന Curl_easy *data,
                                          സ്ഥിരം unsigned അക്ഷരം *cp, size_t len,
                                          ഘടന Curl_https_rrinfo **hrr)
{
  uint16_t pcode = 0, plen = 0;
  uint32_t expected_min_pcode = 0;
  ഘടന Curl_https_rrinfo *lhrr = ശൂന്യ;
  അക്ഷരം *dnsname = ശൂന്യ;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (ശൂന്യം)data;
  *hrr = ശൂന്യ;
  എങ്കിൽ(len <= 2)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  lhrr = curlx_calloc(1, വലുപ്പം(ഘടന Curl_https_rrinfo));
  എങ്കിൽ(!lhrr)
    തിരികെ CURLE_OUT_OF_MEMORY;
  lhrr->priority = doh_get16bit(cp, 0);
  cp += 2;
  len -= 2;
  എങ്കിൽ(doh_decode_rdata_name(&cp, &len, &dnsname) != CURLE_OK)
    പോകുക err;
  lhrr->target = dnsname;
  result = junkscan(dnsname);
  എങ്കിൽ(result) {
    /* unacceptable hostname content */
    പോകുക err;
  }
  ആകുമ്പോൾ(len >= 4) {
    pcode = doh_get16bit(cp, 0);
    plen = doh_get16bit(cp, 2);
    cp += 4;
    len -= 4;
    എങ്കിൽ(pcode < expected_min_pcode || plen > len) {
      result = CURLE_WEIRD_SERVER_REPLY;
      പോകുക err;
    }
    result = Curl_httpsrr_set(lhrr, pcode, cp, plen);
    എങ്കിൽ(result)
      പോകുക err;
    Curl_httpsrr_trace(data, lhrr);
    cp += plen;
    len -= plen;
    expected_min_pcode = pcode + 1;
  }
  *hrr = lhrr;
  തിരികെ CURLE_OK;
err:
  Curl_httpsrr_destroy(lhrr);
  തിരികെ result;
}

#endif /* USE_HTTPSRR */

/* called from multi when a sub transfer, e.g. doh probe, is done.
 * Parse the response and set the results in the `async` context
 * of master, using the id from the probe's CURL_EZM_DOH_PROBE
 * meta data. */
സ്ഥിതം ശൂന്യം doh_probe_done(ഘടന Curl_easy *doh,
                           ഘടന Curl_easy *master, CURLcode result)
{
  ഘടന Curl_resolv_async *async = ശൂന്യ;
  ഘടന doh_probes *dohp = ശൂന്യ;
  ഘടന doh_request *doh_req = ശൂന്യ;
  ഘടന Curl_addrinfo **pdest_ai;
  ഘടന dohentry de;
  പൂർണ്ണസംഖ്യ slot, httpcode;

  de_init(&de);
  doh_req = Curl_meta_get(doh, CURL_EZM_DOH_PROBE);
  എങ്കിൽ(!doh_req) {
    /* transfer `doh` is not a DoH probe. */
    DEBUGASSERT(0);
    പോകുക out;
  }

  async = Curl_async_get(master, doh_req->resolv_id);
  എങ്കിൽ(!async) {
    CURL_TRC_DNS(master, "[%u] ignoring outdated DoH response",
                 doh_req->resolv_id);
    പോകുക out;
  }
  dohp = async->doh;

  വേണ്ടി(slot = 0; slot < DOH_SLOT_എണ്ണം; ++slot) {
    എങ്കിൽ(dohp->probe_mid[slot] == doh->mid)
      നിർത്തുക;
  }
  /* We really should have found the slot where to store the response */
  എങ്കിൽ(slot >= DOH_SLOT_എണ്ണം) {
    failf(master, "DoH: unknown sub request done");
    DEBUGASSERT(0);
    പോകുക out;
  }

  async->queries_ongoing--;
  dohp = async->doh;
  httpcode = doh->info.httpcode;
  തിരഞ്ഞെടുപ്പ്(slot) {
  അവസ്ഥ DOH_SLOT_IPV4:
    async->dns_responses |= CURL_DNSQ_A;
    നിർത്തുക;
#ifdef USE_IPV6
  അവസ്ഥ DOH_SLOT_IPV6:
    async->dns_responses |= CURL_DNSQ_AAAA;
    നിർത്തുക;
#endif
#ifdef USE_HTTPSRR
  അവസ്ഥ DOH_SLOT_HTTPS_RR:
    async->dns_responses |= CURL_DNSQ_HTTPS;
    നിർത്തുക;
#endif
  സ്ഥിരസ്ഥിതി:
    DEBUGASSERT(0);
    നിർത്തുക;
  }

  എങ്കിൽ(result) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: %s",
          doh_type2name(doh_req->dnstype), curl_easy_strerror(result));
    പോകുക out;
  }
  അല്ലെങ്കിൽ എങ്കിൽ((httpcode < 200) || (httpcode >= 300)) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: HTTP status %d",
          doh_type2name(doh_req->dnstype), httpcode);
    പോകുക out;
  }

  dohp->probe_rc[slot] = doh_resp_decode(curlx_dyn_uptr(&doh_req->resp_body),
                                         curlx_dyn_len(&doh_req->resp_body),
                                         doh_req->dnstype, &de);
  എങ്കിൽ(dohp->probe_rc[slot]) {
#ifdef USE_HTTPSRR
    എങ്കിൽ((dohp->probe_rc[slot] == DOH_NO_CONTENT) &&
       (doh_req->dnstype == CURL_DNS_TYPE_HTTPS)) {
      dohp->probe_rc[slot] = DOH_DNS_NXDOMAIN;
    }
#endif
    infof(doh, "[DoH] [%s] error decoding response: %s",
          doh_type2name(doh_req->dnstype),
          doh_strerror(dohp->probe_rc[slot]));
    പോകുക out;
  }

  എങ്കിൽ(doh_req->dnstype == CURL_DNS_TYPE_A)
    pdest_ai = &async->ai_A;
  അല്ലെങ്കിൽ എങ്കിൽ(doh_req->dnstype == CURL_DNS_TYPE_AAAA)
    pdest_ai = &async->ai_AAAA;
  അല്ലെങ്കിൽ
    pdest_ai = ശൂന്യ;

  എങ്കിൽ(pdest_ai && de.numaddr) {
    എങ്കിൽ(*pdest_ai) {
      Curl_freeaddrinfo(*pdest_ai);
      *pdest_ai = ശൂന്യ;
    }
    result = doh2ai(&de, async->peer->hostname, async->peer->port, pdest_ai);
    എങ്കിൽ(result) { /* hard failure on our side, fail completely */
      infof(doh, "[DoH] [%s] error creating addrinfo: %s",
            doh_type2name(doh_req->dnstype), curl_easy_strerror(result));
      dohp->probe_rc[slot] = DOH_OOM;
      async->result = result;
    }
  }
#ifdef USE_HTTPSRR
  അല്ലെങ്കിൽ എങ്കിൽ((doh_req->dnstype == CURL_DNS_TYPE_HTTPS) && de.numhttps_rrs) {
    CURL_TRC_DNS(doh, "[HTTPS] got %d records", de.numhttps_rrs);
    result = doh_resp_decode_httpsrr(doh, de.https_rrs->val,
                                     de.https_rrs->len, &async->httpsrr);
    എങ്കിൽ(result) {
      dohp->probe_rc[slot] = DOH_HTTP_FAILED;
      infof(doh, "[DoH] error decoding HTTPS RR: %s",
            curl_easy_strerror(result));
      പോകുക out;
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

CURLcode Curl_doh_take_result(ഘടന Curl_easy *data,
                              ഘടന Curl_resolv_async *async,
                              ഘടന Curl_dns_entry **pdns)
{
  ഘടന doh_probes *dohp = async->doh;
  CURLcode result = CURLE_OK;

  *pdns = ശൂന്യ; /* defaults to no response */
  എങ്കിൽ(!dohp)
    തിരികെ CURLE_OUT_OF_MEMORY;

  async->negative_answer = FALSE;
  എങ്കിൽ(async->result) {
    result = async->result;
    പോകുക out;
  }

  എങ്കിൽ(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     dohp->probe_mid[DOH_SLOT_IPV4] == UINT32_MAX &&
     dohp->probe_mid[DOH_SLOT_IPV6] == UINT32_MAX) {
    failf(data, "Could not DoH-resolve: %s", async->peer->hostname);
    തിരികെ async->for_proxy ?
      CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!async->queries_ongoing) {
    ഘടന Curl_dns_entry *dns = ശൂന്യ;
    bool negative = TRUE;
    പൂർണ്ണസംഖ്യ slot;

    /* remove DoH handles from multi handle and close them */
    doh_close(data, async);
    /* parse the responses, create the struct and return it! */
    വേണ്ടി(slot = 0; slot < DOH_SLOT_എണ്ണം; slot++) {
      /* Failing without an NXDOMAIN answer - a SERVFAIL-class rcode or
         an undecodable response - says nothing about the name. Such a
         failure must not be cached as a negative entry. */
      എങ്കിൽ(dohp->probe_rc[slot] && (dohp->probe_rc[slot] != DOH_DNS_NXDOMAIN))
        negative = FALSE;
    } /* next slot */

    എങ്കിൽ(async->ai_A || async->ai_AAAA) {
      dns = Curl_dnsc_mk_addr2(
        data, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
      എങ്കിൽ(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        പോകുക out;
      }
    }
#ifdef USE_HTTPSRR
    അല്ലെങ്കിൽ എങ്കിൽ((async->dns_queries & CURL_DNSQ_HTTPS) &&
            !dohp->probe_rc[DOH_SLOT_HTTPS_RR]) {
      Curl_httpsrr_trace(data, async->httpsrr);
      dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
      എങ്കിൽ(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        പോകുക out;
      }
    }
#endif /* USE_HTTPSRR */
    അല്ലെങ്കിൽ {
      /* every query failed. Only NXDOMAIN answers for all of them
         make this a negative answer, eligible for caching. */
      async->negative_answer = negative;
      result = async->for_proxy ?
        CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
    }

    /* and add the entry to the cache */
    എങ്കിൽ(dns)
      result = Curl_dnscache_add(data, dns);
    *pdns = dns;
  } /* !async->queries_ongoing */
  അല്ലെങ്കിൽ
    /* wait for pending DoH transactions to complete */
    തിരികെ CURLE_AGAIN;

out:
  Curl_doh_cleanup(data, async);
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം doh_close(ഘടന Curl_easy *data,
                      ഘടന Curl_resolv_async *async)
{
  ഘടന doh_probes *doh = async ? async->doh : ശൂന്യ;
  എങ്കിൽ(doh && data->multi) {
    ഘടന Curl_easy *probe_data;
    uint32_t mid;
    size_t slot;
    വേണ്ടി(slot = 0; slot < DOH_SLOT_എണ്ണം; slot++) {
      mid = doh->probe_mid[slot];
      എങ്കിൽ(mid == UINT32_MAX)
        തുടരുക;
      doh->probe_mid[slot] = UINT32_MAX;
      /* should have been called before data is removed from multi handle */
      DEBUGASSERT(data->multi);
      probe_data = data->multi ? Curl_multi_get_easy(data->multi, mid) : ശൂന്യ;
      എങ്കിൽ(!probe_data) {
        DEBUGF(infof(data, "Curl_doh_close: xfer for mid=%u not found!", mid));
        തുടരുക;
      }
      probe_data->sub_xfer_done = ശൂന്യ; /* No longer interested in result */
      /* data->multi might already be reset at this time */
      Curl_multi_remove_handle(data->multi, probe_data);
      Curl_close(&probe_data);
    }
    CURL_TRC_DNS(data, "[DoH] probe done");
  }
}

ശൂന്യം Curl_doh_cleanup(ഘടന Curl_easy *data,
                      ഘടന Curl_resolv_async *async)
{
  ഘടന doh_probes *dohp = async->doh;
  എങ്കിൽ(dohp) {
    doh_close(data, async);
    curlx_safefree(async->doh);
  }
}

#endif /* CURL_DISABLE_DOH */
