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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifndef CURL_DISABLE_DOH

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curl_addrinfo.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "vdns/doh.h"
#ಅನ್ತರ್ಭಾವ "vdns/httpsrr.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"
#ಅನ್ತರ್ಭಾವ "escape.h"  /* for Curl_hexencode() */
#ಅನ್ತರ್ಭಾವ "urlapi-int.h"

#define DNS_CLASS_IN 0x01

ಸ್ಥಿರ ಶೂನ್ಯ doh_close(ಸಂರಚನಾ Curl_easy *data,
                      ಸಂರಚನಾ Curl_resolv_async *async);

#ifdef CURLVERBOSE
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ doh_code_str[] = {
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

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *doh_strerror(DOHcode code)
{
  ಯದಿ((size_t)code < CURL_ARRAYSIZE(doh_code_str))
    ಪ್ರತ್ಯಾಗಮ doh_code_str[code];
  ಪ್ರತ್ಯಾಗಮ "bad error code";
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *doh_type2name(DNStype dnstype)
{
  ವಿಕಲ್ಪ(dnstype) {
  ಪಕ್ಷ CURL_DNS_TYPE_A:
    ಪ್ರತ್ಯಾಗಮ "A";
  ಪಕ್ಷ CURL_DNS_TYPE_AAAA:
    ಪ್ರತ್ಯಾಗಮ "AAAA";
#ifdef USE_HTTPSRR
  ಪಕ್ಷ CURL_DNS_TYPE_HTTPS:
    ಪ್ರತ್ಯಾಗಮ "HTTPS";
#endif
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ "unknown";
  }
}

#endif /* CURLVERBOSE */

/* @unittest 1655
 */
UNITTEST DOHcode doh_req_encode(ಧ್ರುವ ವರ್ಣ *host,
                                DNStype dnstype,
                                unsigned ವರ್ಣ *dnsp,  /* buffer */
                                size_t len,  /* buffer size */
                                size_t *olen);  /* output length */
UNITTEST DOHcode doh_req_encode(ಧ್ರುವ ವರ್ಣ *host,
                                DNStype dnstype,
                                unsigned ವರ್ಣ *dnsp, /* buffer */
                                size_t len,   /* buffer size */
                                size_t *olen) /* output length */
{
  ಧ್ರುವ size_t hostlen = strlen(host);
  unsigned ವರ್ಣ *orig = dnsp;
  ಧ್ರುವ ವರ್ಣ *hostp = host;

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
  ಯದಿ(host[hostlen - 1] != '.')
    expected_len++;

  ಯದಿ(expected_len > DOH_MAX_DNSREQ_SIZE)
    ಪ್ರತ್ಯಾಗಮ DOH_DNS_NAME_TOO_LONG;

  ಯದಿ(len < expected_len)
    ಪ್ರತ್ಯಾಗಮ DOH_TOO_SMALL_BUFFER;

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
  ಯಾವತ್(*hostp) {
    size_t labellen;
    ಧ್ರುವ ವರ್ಣ *dot = strchr(hostp, '.');
    ಯದಿ(dot)
      labellen = dot - hostp;
    ಅನ್ಯಥಾ
      labellen = strlen(hostp);
    ಯದಿ((labellen > 63) || (!labellen)) {
      /* label is too long or too short, error out */
      *olen = 0;
      ಪ್ರತ್ಯಾಗಮ DOH_DNS_BAD_LABEL;
    }
    /* label is non-empty, process it */
    *dnsp++ = (unsigned ವರ್ಣ)labellen;
    memcpy(dnsp, hostp, labellen);
    dnsp += labellen;
    hostp += labellen;
    /* advance past dot, but only if there is one */
    ಯದಿ(dot)
      hostp++;
  } /* next label */

  *dnsp++ = 0; /* append zero-length label for root */

  /* There are assigned TYPE codes beyond 255: use range [1..65535] */
  *dnsp++ = (unsigned ವರ್ಣ)(255 & (dnstype >> 8)); /* upper 8-bit TYPE */
  *dnsp++ = (unsigned ವರ್ಣ)(255 & dnstype);        /* lower 8-bit TYPE */

  *dnsp++ = '\0'; /* upper 8-bit CLASS */
  *dnsp++ = DNS_CLASS_IN; /* IN - "the Internet" */

  *olen = dnsp - orig;

  /* verify that our estimation of length is valid, since
   * this has led to buffer overflows in this function */
  DEBUGASSERT(*olen == expected_len);
  ಪ್ರತ್ಯಾಗಮ DOH_OK;
}

ಸ್ಥಿರ size_t doh_probe_write_cb(ವರ್ಣ *contents, size_t size, size_t nmemb,
                                 ಶೂನ್ಯ *userp)
{
  size_t realsize = size * nmemb;
  ಸಂರಚನಾ Curl_easy *data = userp;
  ಸಂರಚನಾ doh_request *doh_req = Curl_meta_get(data, CURL_EZM_DOH_PROBE);
  ಯದಿ(!doh_req)
    ಪ್ರತ್ಯಾಗಮ CURL_WRITEFUNC_ERROR;

  ಯದಿ(curlx_dyn_addn(&doh_req->resp_body, contents, realsize))
    ಪ್ರತ್ಯಾಗಮ 0;

  ಪ್ರತ್ಯಾಗಮ realsize;
}

ಸ್ಥಿರ ಶೂನ್ಯ doh_probe_done(ಸಂರಚನಾ Curl_easy *doh,
                           ಸಂರಚನಾ Curl_easy *master, CURLcode result);
ಸ್ಥಿರ ಶೂನ್ಯ doh_probe_dtor(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *e)
{
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  ಯದಿ(e) {
    ಸಂರಚನಾ doh_request *doh_req = e;
    curl_slist_free_all(doh_req->req_hds);
    curlx_dyn_free(&doh_req->resp_body);
    curlx_free(e);
  }
}

#define ERROR_CHECK_SETOPT(x, y)                        \
  ಕುರು {                                                  \
    result = curl_easy_setopt((CURL *)doh, x, y);       \
    ಯದಿ(result &&                                        \
       result != CURLE_NOT_BUILT_IN &&                  \
       result != CURLE_UNKNOWN_OPTION)                  \
      ಗಚ್ಛ error;                                       \
  } ಯಾವತ್(0)

ಸ್ಥಿರ CURLcode doh_probe_run(ಸಂರಚನಾ Curl_easy *data,
                              DNStype dnstype,
                              ಧ್ರುವ ವರ್ಣ *host,
                              ಧ್ರುವ ವರ್ಣ *url, CURLM *multi,
                              uint32_t resolv_id,
                              uint32_t *pmid)
{
  ಸಂರಚನಾ Curl_easy *doh = ರಿಕ್ತ;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  ಸಂರಚನಾ doh_request *doh_req;
  DOHcode d;
  bool maybe_https = !curl_strnequal(url, STRCONST("http:"));

  *pmid = UINT32_MAX;

  doh_req = curlx_calloc(1, ಪರಿಮಾಣ(*doh_req));
  ಯದಿ(!doh_req)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  doh_req->resolv_id = resolv_id;
  doh_req->dnstype = dnstype;
  curlx_dyn_init(&doh_req->resp_body, DYN_DOH_RESPONSE);

  d = doh_req_encode(host, dnstype, doh_req->req_body,
                     ಪರಿಮಾಣ(doh_req->req_body),
                     &doh_req->req_body_len);
  ಯದಿ(d) {
    failf(data, "Failed to encode DoH packet [%d]", (ಪೂರ್ಣಾಙ್ಕ)d);
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ error;
  }

  timeout_ms = Curl_timeleft_ms(data);
  ಯದಿ(timeout_ms < 0) {
    result = CURLE_OPERATION_TIMEDOUT;
    ಗಚ್ಛ error;
  }

  doh_req->req_hds =
    curl_slist_append(ರಿಕ್ತ, "Content-Type: application/dns-message");
  ಯದಿ(!doh_req->req_hds) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ error;
  }

  /* Curl_open() is the internal version of curl_easy_init() */
  result = Curl_open(&doh);
  ಯದಿ(result)
    ಗಚ್ಛ error;

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
  ಯದಿ(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);
    ERROR_CHECK_SETOPT(CURLOPT_PIPEWAIT, 1L);
  }
#endif
#ifndef DEBUGBUILD
  /* enforce HTTPS if not debug */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
#ಅನ್ಯಥಾ
  /* in debug mode, also allow http */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
#endif
  ERROR_CHECK_SETOPT(CURLOPT_TIMEOUT_MS, (long)timeout_ms);
  ERROR_CHECK_SETOPT(CURLOPT_SHARE, (CURLSH *)data->share);
  ಯದಿ(data->set.err && data->set.err != stderr)
    ERROR_CHECK_SETOPT(CURLOPT_STDERR, data->set.err);
  ಯದಿ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_doh))
    ERROR_CHECK_SETOPT(CURLOPT_VERBOSE, 1L);
  ಯದಿ(data->set.no_signal)
    ERROR_CHECK_SETOPT(CURLOPT_NOSIGNAL, 1L);
  ಯದಿ(data->set.fdebug)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGFUNCTION, data->set.fdebug);
  ಯದಿ(data->set.debugdata)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGDATA, data->set.debugdata);

#ifdef USE_RESOLV_ARES
  ಯದಿ(CURL_EASY_STR(data, STRING_DNS_SERVERS)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_SERVERS,
                       CURL_EASY_STR(data, STRING_DNS_SERVERS));
  }
  ಯದಿ(CURL_EASY_STR(data, STRING_DNS_INTERFACE)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_INTERFACE,
                       CURL_EASY_STR(data, STRING_DNS_INTERFACE));
  }
  ಯದಿ(CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP4,
                       CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4));
  }
  ಯದಿ(CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP6,
                       CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6));
  }
#endif
  ಯದಿ(maybe_https) {
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
    ಯದಿ(CURL_EASY_STR(data, STRING_SSL_CAFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO,
                         CURL_EASY_STR(data, STRING_SSL_CAFILE));
    }
    ಯದಿ(data->set.blobs[BLOB_CAINFO]) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO_BLOB, data->set.blobs[BLOB_CAINFO]);
    }
    ಯದಿ(CURL_EASY_STR(data, STRING_SSL_CAPATH)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAPATH,
                         CURL_EASY_STR(data, STRING_SSL_CAPATH));
    }
    ಯದಿ(CURL_EASY_STR(data, STRING_SSL_CRLFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CRLFILE,
                         CURL_EASY_STR(data, STRING_SSL_CRLFILE));
    }
    ಯದಿ(data->set.ssl.certinfo)
      ERROR_CHECK_SETOPT(CURLOPT_CERTINFO, 1L);
    ಯದಿ(data->set.ssl_fsslctx)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_FUNCTION, data->set.ssl_fsslctx);
    ಯದಿ(data->set.ssl_fsslctxp)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_DATA, data->set.ssl_fsslctxp);
    ಯದಿ(CURL_EASY_STR(data, STRING_SSL_EC_CURVES)) {
      ERROR_CHECK_SETOPT(CURLOPT_SSL_EC_CURVES,
                         CURL_EASY_STR(data, STRING_SSL_EC_CURVES));
    }
    (ಶೂನ್ಯ)curl_easy_setopt(doh, CURLOPT_SSL_OPTIONS,
                           ((long)data->set.ssl.ssl_options &
                            ~CURLSSLOPT_AUTO_CLIENT_CERT));
  }

  doh->state.internal = TRUE;
  doh->master_mid = data->mid; /* master transfer of this one */
  doh->sub_xfer_done = doh_probe_done;

  result = Curl_meta_set(doh, CURL_EZM_DOH_PROBE, doh_req, doh_probe_dtor);
  doh_req = ರಿಕ್ತ; /* call took ownership */
  ಯದಿ(result)
    ಗಚ್ಛ error;

  /* DoH handles must not inherit private_data. The handles may be passed to
     the user via callbacks and the user will be able to identify them as
     internal handles because private data is not set. The user can then set
     private_data via CURLOPT_PRIVATE if they so choose. */
  DEBUGASSERT(!doh->set.private_data);

  ಯದಿ(Curl_multi_add_handle(multi, doh))
    ಗಚ್ಛ error;

  *pmid = doh->mid;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;

error:
  Curl_close(&doh);
  ಯದಿ(doh_req)
    doh_probe_dtor(ರಿಕ್ತ, 0, doh_req);
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Curl_doh() starts a name resolve using DoH. It resolves a name and returns
 * a 'Curl_addrinfo *' with the address information.
 */

CURLcode Curl_doh(ಸಂರಚನಾ Curl_easy *data,
                  ಸಂರಚನಾ Curl_resolv_async *async)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ doh_probes *dohp = ರಿಕ್ತ;
  size_t i;

  DEBUGASSERT(!async->doh);
  DEBUGASSERT(async->peer->hostname[0]);
  ಯದಿ(async->doh) {
    DEBUGASSERT(0); /* should not happen */
    Curl_doh_cleanup(data, async);
  }

  ಯದಿ(!async->dns_queries)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef USE_HTTPSRR
  ಯದಿ(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     (async->dns_queries & CURL_DNSQ_HTTPS)) {
    /* Can't mix those in the same async resolve */
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  }
#ಅನ್ಯಥಾ
  ಯದಿ(async->dns_queries & CURL_DNSQ_HTTPS) {
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
  }
#endif

  /* start clean, consider allocating this struct on demand */
  async->doh = dohp = curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ doh_probes));
  ಯದಿ(!dohp)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  ಪ್ರತಿ(i = 0; i < DOH_SLOT_ಎಣಿಕೆ; ++i) {
    dohp->probe_rc[i] = DOH_OK;
    dohp->probe_mid[i] = UINT32_MAX;
  }

#ifdef USE_IPV6
  /* AAAA results have preference in happy eyeballing, trigger first */
  ಯದಿ(async->dns_queries & CURL_DNSQ_AAAA) {
    /* create IPv6 DoH request */
    result = doh_probe_run(data, CURL_DNS_TYPE_AAAA,
                           async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH),
                           data->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV6]);
    ಯದಿ(result)
      ಗಚ್ಛ error;
    async->queries_ongoing++;
  }
#endif

  /* create IPv4 DoH request */
  ಯದಿ(async->dns_queries & CURL_DNSQ_A) {
    result = doh_probe_run(data, CURL_DNS_TYPE_A,
                           async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH),
                           data->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV4]);
    ಯದಿ(result)
      ಗಚ್ಛ error;
    async->queries_ongoing++;
  }

#ifdef USE_HTTPSRR
  ಯದಿ(async->dns_queries & CURL_DNSQ_HTTPS) {
    ವರ್ಣ *qname = ರಿಕ್ತ;
    ಯದಿ(async->peer->port != PORT_HTTPS) {
      qname = curl_maprintf("_%u._https.%s",
                            async->peer->port, async->peer->hostname);
      ಯದಿ(!qname)
        ಗಚ್ಛ error;
    }
    result = doh_probe_run(data, CURL_DNS_TYPE_HTTPS,
                           qname ? qname : async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH), data->multi,
                           async->id,
                           &dohp->probe_mid[DOH_SLOT_HTTPS_RR]);
    curlx_free(qname);
    ಯದಿ(result)
      ಗಚ್ಛ error;
    async->queries_ongoing++;
  }
#endif
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;

error:
  Curl_doh_cleanup(data, async);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ DOHcode doh_skipqname(ಧ್ರುವ unsigned ವರ್ಣ *doh, size_t dohlen,
                             unsigned ಪೂರ್ಣಾಙ್ಕ *indexp)
{
  unsigned ವರ್ಣ length;
  ಕುರು {
    ಯದಿ(dohlen < (*indexp + 1))
      ಪ್ರತ್ಯಾಗಮ DOH_DNS_OUT_OF_RANGE;
    length = doh[*indexp];
    ಯದಿ((length & 0xc0) == 0xc0) {
      /* name pointer, advance over it and be done */
      ಯದಿ(dohlen < (*indexp + 2))
        ಪ್ರತ್ಯಾಗಮ DOH_DNS_OUT_OF_RANGE;
      *indexp += 2;
      ಭಙ್ಗ;
    }
    ಯದಿ(length & 0xc0)
      ಪ್ರತ್ಯಾಗಮ DOH_DNS_BAD_LABEL;
    ಯದಿ(dohlen < (*indexp + 1 + length))
      ಪ್ರತ್ಯಾಗಮ DOH_DNS_OUT_OF_RANGE;
    *indexp += (unsigned ಪೂರ್ಣಾಙ್ಕ)(1 + length);
  } ಯಾವತ್(length);
  ಪ್ರತ್ಯಾಗಮ DOH_OK;
}

ಸ್ಥಿರ unsigned short doh_get16bit(ಧ್ರುವ unsigned ವರ್ಣ *doh,
                                   unsigned ಪೂರ್ಣಾಙ್ಕ index)
{
  ಪ್ರತ್ಯಾಗಮ (unsigned short)((doh[index] << 8) | doh[index + 1]);
}

ಸ್ಥಿರ unsigned ಪೂರ್ಣಾಙ್ಕ doh_get32bit(ಧ್ರುವ unsigned ವರ್ಣ *doh, unsigned ಪೂರ್ಣಾಙ್ಕ index)
{
  /* make clang and gcc optimize this to bswap by incrementing
     the pointer first. */
  doh += index;

  /* avoid undefined behavior by casting to unsigned before shifting
     24 bits, possibly into the sign bit. codegen is same, but
     ub sanitizer will not be upset */
  ಪ್ರತ್ಯಾಗಮ ((unsigned)doh[0] << 24) | ((unsigned)doh[1] << 16) |
         ((unsigned)doh[2] << 8) | doh[3];
}

ಸ್ಥಿರ ಶೂನ್ಯ doh_store_a(ಧ್ರುವ unsigned ವರ್ಣ *doh, ಪೂರ್ಣಾಙ್ಕ index,
                        ಸಂರಚನಾ dohentry *d)
{
  /* silently ignore addresses over the limit */
  ಯದಿ(d->numaddr < DOH_MAX_ADDR) {
    ಸಂರಚನಾ dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_A;
    memcpy(&a->ip.v4, &doh[index], 4);
    d->numaddr++;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ doh_store_aaaa(ಧ್ರುವ unsigned ವರ್ಣ *doh, ಪೂರ್ಣಾಙ್ಕ index,
                           ಸಂರಚನಾ dohentry *d)
{
  /* silently ignore addresses over the limit */
  ಯದಿ(d->numaddr < DOH_MAX_ADDR) {
    ಸಂರಚನಾ dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_AAAA;
    memcpy(&a->ip.v6, &doh[index], 16);
    d->numaddr++;
  }
}

#ifdef USE_HTTPSRR
ಸ್ಥಿರ DOHcode doh_store_https(ಧ್ರುವ unsigned ವರ್ಣ *doh, ಪೂರ್ಣಾಙ್ಕ index,
                               ಸಂರಚನಾ dohentry *d, uint16_t len)
{
  /* silently ignore RRs over the limit */
  ಯದಿ(d->numhttps_rrs < DOH_MAX_HTTPS) {
    ಸಂರಚನಾ dohhttps_rr *h = &d->https_rrs[d->numhttps_rrs];
    h->val = curlx_memdup(&doh[index], len);
    ಯದಿ(!h->val)
      ಪ್ರತ್ಯಾಗಮ DOH_OUT_OF_MEM;
    h->len = len;
    d->numhttps_rrs++;
  }
  ಪ್ರತ್ಯಾಗಮ DOH_OK;
}
#endif

ಸ್ಥಿರ DOHcode doh_rdata(ಧ್ರುವ unsigned ವರ್ಣ *doh,
                         unsigned short rdlength,
                         unsigned short type,
                         ಪೂರ್ಣಾಙ್ಕ index,
                         ಸಂರಚನಾ dohentry *d)
{
  /* RDATA
     - A (TYPE 1): 4 bytes
     - AAAA (TYPE 28): 16 bytes
     - HTTPS (TYPE 65): N bytes */
  ವಿಕಲ್ಪ(type) {
  ಪಕ್ಷ CURL_DNS_TYPE_A:
    ಯದಿ(rdlength != 4)
      ಪ್ರತ್ಯಾಗಮ DOH_DNS_RDATA_LEN;
    doh_store_a(doh, index, d);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_DNS_TYPE_AAAA:
    ಯದಿ(rdlength != 16)
      ಪ್ರತ್ಯಾಗಮ DOH_DNS_RDATA_LEN;
    doh_store_aaaa(doh, index, d);
    ಭಙ್ಗ;
#ifdef USE_HTTPSRR
  ಪಕ್ಷ CURL_DNS_TYPE_HTTPS:
    ಯದಿ(rdlength < 3)
      ಪ್ರತ್ಯಾಗಮ DOH_DNS_RDATA_LEN;
    ಪ್ರತ್ಯಾಗಮ doh_store_https(doh, index, d, rdlength);
#endif
  ಸ್ವಭಾವ:
    /* unsupported type, or type we do not store, skip it */
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ DOH_OK;
}

/* @unittest 1655 */
UNITTEST ಶೂನ್ಯ de_init(ಸಂರಚನಾ dohentry *de);
UNITTEST ಶೂನ್ಯ de_init(ಸಂರಚನಾ dohentry *de)
{
  memset(de, 0, ಪರಿಮಾಣ(*de));
  de->ttl = INT_MAX;
}

/* TTL value cap */
#define MAX_DNS_TTL 86400U /* 24 hours */
/* @unittest 1650 */
UNITTEST DOHcode doh_resp_decode(ಧ್ರುವ unsigned ವರ್ಣ *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 ಸಂರಚನಾ dohentry *d);
UNITTEST DOHcode doh_resp_decode(ಧ್ರುವ unsigned ವರ್ಣ *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 ಸಂರಚನಾ dohentry *d)
{
  unsigned ವರ್ಣ rcode;
  unsigned short qdcount;
  unsigned short ancount;
  unsigned short type = 0;
  unsigned short rdlength;
  unsigned short nscount;
  unsigned short arcount;
  unsigned ಪೂರ್ಣಾಙ್ಕ index = 12;
  DOHcode rc;

  ಯದಿ(dohlen < 12)
    ಪ್ರತ್ಯಾಗಮ DOH_TOO_SMALL_BUFFER; /* too small */
  ಯದಿ(!doh || doh[0] || doh[1])
    ಪ್ರತ್ಯಾಗಮ DOH_DNS_BAD_ID; /* bad ID */
  rcode = doh[3] & 0x0f;
  ಯದಿ(rcode == 3)
    ಪ್ರತ್ಯಾಗಮ DOH_DNS_NXDOMAIN; /* name does not exist */
  ಯದಿ(rcode)
    ಪ್ರತ್ಯಾಗಮ DOH_DNS_BAD_RCODE; /* bad rcode */

  qdcount = doh_get16bit(doh, 4);
  ಯಾವತ್(qdcount) {
    rc = doh_skipqname(doh, dohlen, &index);
    ಯದಿ(rc)
      ಪ್ರತ್ಯಾಗಮ rc; /* bad qname */
    ಯದಿ(dohlen < (index + 4))
      ಪ್ರತ್ಯಾಗಮ DOH_DNS_OUT_OF_RANGE;
    index += 4; /* skip question's type and class */
    qdcount--;
  }

  ancount = doh_get16bit(doh, 6);
  ಯಾವತ್(ancount) {
    unsigned short dnsclass;
    unsigned ಪೂರ್ಣಾಙ್ಕ ttl;

    rc = doh_skipqname(doh, dohlen, &index);
    ಯದಿ(rc)
      ಪ್ರತ್ಯಾಗಮ rc; /* bad qname */

    ಯದಿ(dohlen < (index + 2))
      ಪ್ರತ್ಯಾಗಮ DOH_DNS_OUT_OF_RANGE;

    type = doh_get16bit(doh, index);
    ಯದಿ((type != CURL_DNS_TYPE_CNAME) &&  /* may be synthesized from DNAME */
       (type != CURL_DNS_TYPE_DNAME) &&  /* if present, accept and ignore */
       (type != dnstype))
      /* Not the same type as was asked for, nor CNAME nor DNAME */
      ಪ್ರತ್ಯಾಗಮ DOH_DNS_UNEXPECTED_TYPE;
    index += 2;

    ಯದಿ(dohlen < (index + 2))
      ಪ್ರತ್ಯಾಗಮ DOH_DNS_OUT_OF_RANGE;
    dnsclass = doh_get16bit(doh, index);
    ಯದಿ(DNS_CLASS_IN != dnsclass)
      ಪ್ರತ್ಯಾಗಮ DOH_DNS_UNEXPECTED_CLASS; /* unsupported */
    index += 2;

    ಯದಿ(dohlen < (index + 4))
      ಪ್ರತ್ಯಾಗಮ DOH_DNS_OUT_OF_RANGE;

    ttl = doh_get32bit(doh, index);
    ಯದಿ(ttl > MAX_DNS_TTL)
      ttl = MAX_DNS_TTL;
    ಯದಿ(ttl < d->ttl)
      d->ttl = ttl;
    index += 4;

    ಯದಿ(dohlen < (index + 2))
      ಪ್ರತ್ಯಾಗಮ DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    ಯದಿ(dohlen < (index + rdlength))
      ಪ್ರತ್ಯಾಗಮ DOH_DNS_OUT_OF_RANGE;

    rc = doh_rdata(doh, rdlength, type, (ಪೂರ್ಣಾಙ್ಕ)index, d);
    ಯದಿ(rc)
      ಪ್ರತ್ಯಾಗಮ rc;
    index += rdlength;
    ancount--;
  }

  nscount = doh_get16bit(doh, 8);
  ಯಾವತ್(nscount) {
    rc = doh_skipqname(doh, dohlen, &index);
    ಯದಿ(rc)
      ಪ್ರತ್ಯಾಗಮ rc; /* bad qname */

    ಯದಿ(dohlen < (index + 8))
      ಪ್ರತ್ಯಾಗಮ DOH_DNS_OUT_OF_RANGE;

    index += 2 + 2 + 4; /* type, dnsclass and ttl */

    ಯದಿ(dohlen < (index + 2))
      ಪ್ರತ್ಯಾಗಮ DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    ಯದಿ(dohlen < (index + rdlength))
      ಪ್ರತ್ಯಾಗಮ DOH_DNS_OUT_OF_RANGE;
    index += rdlength;
    nscount--;
  }

  arcount = doh_get16bit(doh, 10);
  ಯಾವತ್(arcount) {
    rc = doh_skipqname(doh, dohlen, &index);
    ಯದಿ(rc)
      ಪ್ರತ್ಯಾಗಮ rc; /* bad qname */

    ಯದಿ(dohlen < (index + 8))
      ಪ್ರತ್ಯಾಗಮ DOH_DNS_OUT_OF_RANGE;

    index += 2 + 2 + 4; /* type, dnsclass and ttl */

    ಯದಿ(dohlen < (index + 2))
      ಪ್ರತ್ಯಾಗಮ DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    ಯದಿ(dohlen < (index + rdlength))
      ಪ್ರತ್ಯಾಗಮ DOH_DNS_OUT_OF_RANGE;
    index += rdlength;
    arcount--;
  }

  ಯದಿ(index != dohlen)
    ಪ್ರತ್ಯಾಗಮ DOH_DNS_MALFORMAT; /* something is wrong */

  ಪ್ರತ್ಯಾಗಮ DOH_OK; /* ok */
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
ಸ್ಥಿರ CURLcode doh2ai(ಧ್ರುವ ಸಂರಚನಾ dohentry *de, ಧ್ರುವ ವರ್ಣ *hostname,
                       ಪೂರ್ಣಾಙ್ಕ port, ಸಂರಚನಾ Curl_addrinfo **aip)
{
  ಸಂರಚನಾ Curl_addrinfo *ai;
  ಸಂರಚನಾ Curl_addrinfo *prevai = ರಿಕ್ತ;
  ಸಂರಚನಾ Curl_addrinfo *firstai = ರಿಕ್ತ;
  ಸಂರಚನಾ sockaddr_in *addr;
#ifdef USE_IPV6
  ಸಂರಚನಾ sockaddr_in6 *addr6;
#endif
  size_t hostlen = strlen(hostname) + 1; /* include null-terminator */
  CURLcode result = CURLE_OK;
  ಪೂರ್ಣಾಙ್ಕ i;

  ಪ್ರತಿ(i = 0; i < de->numaddr; i++) {
    size_t ss_size;
    CURL_SA_FAMILY_T addrtype;
    ಯದಿ(de->addr[i].type == CURL_DNS_TYPE_AAAA) {
#ifndef USE_IPV6
      /* we cannot handle IPv6 addresses */
      ಅನುವರ್ತ;
#ಅನ್ಯಥಾ
      ss_size = ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_in6);
      addrtype = AF_INET6;
#endif
    }
    ಅನ್ಯಥಾ {
      ss_size = ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_in);
      addrtype = AF_INET;
    }

    ai = curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_addrinfo) + ss_size + hostlen);
    ಯದಿ(!ai) {
      result = CURLE_OUT_OF_MEMORY;
      ಭಙ್ಗ;
    }
    ai->ai_addr = (ಶೂನ್ಯ *)((ವರ್ಣ *)ai + ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_addrinfo));
    ai->ai_canonname = (ಶೂನ್ಯ *)((ವರ್ಣ *)ai->ai_addr + ss_size);
    memcpy(ai->ai_canonname, hostname, hostlen);

    ಯದಿ(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    ಯದಿ(prevai)
      /* make the previous entry point to this */
      prevai->ai_next = ai;

    ai->ai_family = addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_size;

    /* leave the rest of the struct filled with zero */

    ವಿಕಲ್ಪ(ai->ai_family) {
    ಪಕ್ಷ AF_INET:
      addr = (ಶೂನ್ಯ *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(ಪರಿಮಾಣ(ಸಂರಚನಾ in_addr) == ಪರಿಮಾಣ(de->addr[i].ip.v4));
      memcpy(&addr->sin_addr, &de->addr[i].ip.v4, ಪರಿಮಾಣ(ಸಂರಚನಾ in_addr));
      addr->sin_family = addrtype;
      addr->sin_port = htons((unsigned short)port);
      ಭಙ್ಗ;

#ifdef USE_IPV6
    ಪಕ್ಷ AF_INET6:
      addr6 = (ಶೂನ್ಯ *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(ಪರಿಮಾಣ(ಸಂರಚನಾ in6_addr) == ಪರಿಮಾಣ(de->addr[i].ip.v6));
      memcpy(&addr6->sin6_addr, &de->addr[i].ip.v6, ಪರಿಮಾಣ(ಸಂರಚನಾ in6_addr));
      addr6->sin6_family = addrtype;
      addr6->sin6_port = htons((unsigned short)port);
      ಭಙ್ಗ;
#endif
    }

    prevai = ai;
  }

  ಯದಿ(result) {
    Curl_freeaddrinfo(firstai);
    firstai = ರಿಕ್ತ;
  }
  *aip = firstai;

  ಪ್ರತ್ಯಾಗಮ result;
}

/* @unittest 1655 */
UNITTEST ಶೂನ್ಯ de_cleanup(ಸಂರಚನಾ dohentry *d);
UNITTEST ಶೂನ್ಯ de_cleanup(ಸಂರಚನಾ dohentry *d)
{
#ifdef USE_HTTPSRR
  ಪೂರ್ಣಾಙ್ಕ i = 0;
  ಪ್ರತಿ(i = 0; i < d->numhttps_rrs; i++)
    curlx_safefree(d->https_rrs[i].val);
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)d;
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
ಸ್ಥಿರ CURLcode doh_decode_rdata_name(ಧ್ರುವ unsigned ವರ್ಣ **buf,
                                      size_t *remaining, ವರ್ಣ **dnsname)
{
  ಧ್ರುವ unsigned ವರ್ಣ *cp = ರಿಕ್ತ;
  size_t rem = 0;
  unsigned ವರ್ಣ clen = 0; /* chunk len */
  ಸಂರಚನಾ dynbuf thename;

  DEBUGASSERT(buf && remaining && dnsname);
  ಯದಿ(!buf || !remaining || !dnsname || !*remaining)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&thename, CURL_MAXLEN_HOST_NAME);
  rem = *remaining;
  cp = *buf;
  clen = *cp++;
  /* RFC 9460 says it must be uncompressed */
  ಯದಿ(clen > 63)
    ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;

  ಯದಿ(clen == 0) {
    /* special case - return "." as name */
    ಯದಿ(curlx_dyn_addn(&thename, ".", 1))
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }
  ಯಾವತ್(clen) {
    ಯದಿ(clen >= rem) {
      curlx_dyn_free(&thename);
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }
    ಯದಿ(curlx_dyn_addn(&thename, cp, clen) ||
       curlx_dyn_addn(&thename, ".", 1))
      ಪ್ರತ್ಯಾಗಮ CURLE_TOO_LARGE;

    cp += clen;
    rem -= (clen + 1);
    ಯದಿ(rem <= 0) {
      curlx_dyn_free(&thename);
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }
    clen = *cp++;
    ಯದಿ(clen > 63) {
      /* invalid format */
      curlx_dyn_free(&thename);
      ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
    }
  }
  *buf = cp;
  *remaining = rem - 1;
  *dnsname = curlx_dyn_ptr(&thename);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* scan for byte values <= 32 or 127 */
ಸ್ಥಿರ CURLcode junkscan(ಧ್ರುವ ವರ್ಣ *url)
{
  ಧ್ರುವ unsigned ವರ್ಣ *p = (ಧ್ರುವ unsigned ವರ್ಣ *)url;
  ಯಾವತ್(*p) {
    ಯದಿ(*p <= 0x20 || *p == 127)
      ಪ್ರತ್ಯಾಗಮ CURLE_WEIRD_SERVER_REPLY;
    p++;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* @unittest 1658 */
UNITTEST CURLcode doh_resp_decode_httpsrr(ಸಂರಚನಾ Curl_easy *data,
                                          ಧ್ರುವ unsigned ವರ್ಣ *cp, size_t len,
                                          ಸಂರಚನಾ Curl_https_rrinfo **hrr);
UNITTEST CURLcode doh_resp_decode_httpsrr(ಸಂರಚನಾ Curl_easy *data,
                                          ಧ್ರುವ unsigned ವರ್ಣ *cp, size_t len,
                                          ಸಂರಚನಾ Curl_https_rrinfo **hrr)
{
  uint16_t pcode = 0, plen = 0;
  uint32_t expected_min_pcode = 0;
  ಸಂರಚನಾ Curl_https_rrinfo *lhrr = ರಿಕ್ತ;
  ವರ್ಣ *dnsname = ರಿಕ್ತ;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (ಶೂನ್ಯ)data;
  *hrr = ರಿಕ್ತ;
  ಯದಿ(len <= 2)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  lhrr = curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_https_rrinfo));
  ಯದಿ(!lhrr)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  lhrr->priority = doh_get16bit(cp, 0);
  cp += 2;
  len -= 2;
  ಯದಿ(doh_decode_rdata_name(&cp, &len, &dnsname) != CURLE_OK)
    ಗಚ್ಛ err;
  lhrr->target = dnsname;
  result = junkscan(dnsname);
  ಯದಿ(result) {
    /* unacceptable hostname content */
    ಗಚ್ಛ err;
  }
  ಯಾವತ್(len >= 4) {
    pcode = doh_get16bit(cp, 0);
    plen = doh_get16bit(cp, 2);
    cp += 4;
    len -= 4;
    ಯದಿ(pcode < expected_min_pcode || plen > len) {
      result = CURLE_WEIRD_SERVER_REPLY;
      ಗಚ್ಛ err;
    }
    result = Curl_httpsrr_set(lhrr, pcode, cp, plen);
    ಯದಿ(result)
      ಗಚ್ಛ err;
    Curl_httpsrr_trace(data, lhrr);
    cp += plen;
    len -= plen;
    expected_min_pcode = pcode + 1;
  }
  *hrr = lhrr;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
err:
  Curl_httpsrr_destroy(lhrr);
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* USE_HTTPSRR */

/* called from multi when a sub transfer, e.g. doh probe, is done.
 * Parse the response and set the results in the `async` context
 * of master, using the id from the probe's CURL_EZM_DOH_PROBE
 * meta data. */
ಸ್ಥಿರ ಶೂನ್ಯ doh_probe_done(ಸಂರಚನಾ Curl_easy *doh,
                           ಸಂರಚನಾ Curl_easy *master, CURLcode result)
{
  ಸಂರಚನಾ Curl_resolv_async *async = ರಿಕ್ತ;
  ಸಂರಚನಾ doh_probes *dohp = ರಿಕ್ತ;
  ಸಂರಚನಾ doh_request *doh_req = ರಿಕ್ತ;
  ಸಂರಚನಾ Curl_addrinfo **pdest_ai;
  ಸಂರಚನಾ dohentry de;
  ಪೂರ್ಣಾಙ್ಕ slot, httpcode;

  de_init(&de);
  doh_req = Curl_meta_get(doh, CURL_EZM_DOH_PROBE);
  ಯದಿ(!doh_req) {
    /* transfer `doh` is not a DoH probe. */
    DEBUGASSERT(0);
    ಗಚ್ಛ out;
  }

  async = Curl_async_get(master, doh_req->resolv_id);
  ಯದಿ(!async) {
    CURL_TRC_DNS(master, "[%u] ignoring outdated DoH response",
                 doh_req->resolv_id);
    ಗಚ್ಛ out;
  }
  dohp = async->doh;

  ಪ್ರತಿ(slot = 0; slot < DOH_SLOT_ಎಣಿಕೆ; ++slot) {
    ಯದಿ(dohp->probe_mid[slot] == doh->mid)
      ಭಙ್ಗ;
  }
  /* We really should have found the slot where to store the response */
  ಯದಿ(slot >= DOH_SLOT_ಎಣಿಕೆ) {
    failf(master, "DoH: unknown sub request done");
    DEBUGASSERT(0);
    ಗಚ್ಛ out;
  }

  async->queries_ongoing--;
  dohp = async->doh;
  httpcode = doh->info.httpcode;
  ವಿಕಲ್ಪ(slot) {
  ಪಕ್ಷ DOH_SLOT_IPV4:
    async->dns_responses |= CURL_DNSQ_A;
    ಭಙ್ಗ;
#ifdef USE_IPV6
  ಪಕ್ಷ DOH_SLOT_IPV6:
    async->dns_responses |= CURL_DNSQ_AAAA;
    ಭಙ್ಗ;
#endif
#ifdef USE_HTTPSRR
  ಪಕ್ಷ DOH_SLOT_HTTPS_RR:
    async->dns_responses |= CURL_DNSQ_HTTPS;
    ಭಙ್ಗ;
#endif
  ಸ್ವಭಾವ:
    DEBUGASSERT(0);
    ಭಙ್ಗ;
  }

  ಯದಿ(result) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: %s",
          doh_type2name(doh_req->dnstype), curl_easy_strerror(result));
    ಗಚ್ಛ out;
  }
  ಅನ್ಯಥಾ ಯದಿ((httpcode < 200) || (httpcode >= 300)) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: HTTP status %d",
          doh_type2name(doh_req->dnstype), httpcode);
    ಗಚ್ಛ out;
  }

  dohp->probe_rc[slot] = doh_resp_decode(curlx_dyn_uptr(&doh_req->resp_body),
                                         curlx_dyn_len(&doh_req->resp_body),
                                         doh_req->dnstype, &de);
  ಯದಿ(dohp->probe_rc[slot]) {
#ifdef USE_HTTPSRR
    ಯದಿ((dohp->probe_rc[slot] == DOH_NO_CONTENT) &&
       (doh_req->dnstype == CURL_DNS_TYPE_HTTPS)) {
      dohp->probe_rc[slot] = DOH_DNS_NXDOMAIN;
    }
#endif
    infof(doh, "[DoH] [%s] error decoding response: %s",
          doh_type2name(doh_req->dnstype),
          doh_strerror(dohp->probe_rc[slot]));
    ಗಚ್ಛ out;
  }

  ಯದಿ(doh_req->dnstype == CURL_DNS_TYPE_A)
    pdest_ai = &async->ai_A;
  ಅನ್ಯಥಾ ಯದಿ(doh_req->dnstype == CURL_DNS_TYPE_AAAA)
    pdest_ai = &async->ai_AAAA;
  ಅನ್ಯಥಾ
    pdest_ai = ರಿಕ್ತ;

  ಯದಿ(pdest_ai && de.numaddr) {
    ಯದಿ(*pdest_ai) {
      Curl_freeaddrinfo(*pdest_ai);
      *pdest_ai = ರಿಕ್ತ;
    }
    result = doh2ai(&de, async->peer->hostname, async->peer->port, pdest_ai);
    ಯದಿ(result) { /* hard failure on our side, fail completely */
      infof(doh, "[DoH] [%s] error creating addrinfo: %s",
            doh_type2name(doh_req->dnstype), curl_easy_strerror(result));
      dohp->probe_rc[slot] = DOH_OOM;
      async->result = result;
    }
  }
#ifdef USE_HTTPSRR
  ಅನ್ಯಥಾ ಯದಿ((doh_req->dnstype == CURL_DNS_TYPE_HTTPS) && de.numhttps_rrs) {
    CURL_TRC_DNS(doh, "[HTTPS] got %d records", de.numhttps_rrs);
    result = doh_resp_decode_httpsrr(doh, de.https_rrs->val,
                                     de.https_rrs->len, &async->httpsrr);
    ಯದಿ(result) {
      dohp->probe_rc[slot] = DOH_HTTP_FAILED;
      infof(doh, "[DoH] error decoding HTTPS RR: %s",
            curl_easy_strerror(result));
      ಗಚ್ಛ out;
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

CURLcode Curl_doh_take_result(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ Curl_resolv_async *async,
                              ಸಂರಚನಾ Curl_dns_entry **pdns)
{
  ಸಂರಚನಾ doh_probes *dohp = async->doh;
  CURLcode result = CURLE_OK;

  *pdns = ರಿಕ್ತ; /* defaults to no response */
  ಯದಿ(!dohp)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  async->negative_answer = FALSE;
  ಯದಿ(async->result) {
    result = async->result;
    ಗಚ್ಛ out;
  }

  ಯದಿ(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     dohp->probe_mid[DOH_SLOT_IPV4] == UINT32_MAX &&
     dohp->probe_mid[DOH_SLOT_IPV6] == UINT32_MAX) {
    failf(data, "Could not DoH-resolve: %s", async->peer->hostname);
    ಪ್ರತ್ಯಾಗಮ async->for_proxy ?
      CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
  }
  ಅನ್ಯಥಾ ಯದಿ(!async->queries_ongoing) {
    ಸಂರಚನಾ Curl_dns_entry *dns = ರಿಕ್ತ;
    bool negative = TRUE;
    ಪೂರ್ಣಾಙ್ಕ slot;

    /* remove DoH handles from multi handle and close them */
    doh_close(data, async);
    /* parse the responses, create the struct and return it! */
    ಪ್ರತಿ(slot = 0; slot < DOH_SLOT_ಎಣಿಕೆ; slot++) {
      /* Failing without an NXDOMAIN answer - a SERVFAIL-class rcode or
         an undecodable response - says nothing about the name. Such a
         failure must not be cached as a negative entry. */
      ಯದಿ(dohp->probe_rc[slot] && (dohp->probe_rc[slot] != DOH_DNS_NXDOMAIN))
        negative = FALSE;
    } /* next slot */

    ಯದಿ(async->ai_A || async->ai_AAAA) {
      dns = Curl_dnsc_mk_addr2(
        data, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
      ಯದಿ(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        ಗಚ್ಛ out;
      }
    }
#ifdef USE_HTTPSRR
    ಅನ್ಯಥಾ ಯದಿ((async->dns_queries & CURL_DNSQ_HTTPS) &&
            !dohp->probe_rc[DOH_SLOT_HTTPS_RR]) {
      Curl_httpsrr_trace(data, async->httpsrr);
      dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
      ಯದಿ(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        ಗಚ್ಛ out;
      }
    }
#endif /* USE_HTTPSRR */
    ಅನ್ಯಥಾ {
      /* every query failed. Only NXDOMAIN answers for all of them
         make this a negative answer, eligible for caching. */
      async->negative_answer = negative;
      result = async->for_proxy ?
        CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
    }

    /* and add the entry to the cache */
    ಯದಿ(dns)
      result = Curl_dnscache_add(data, dns);
    *pdns = dns;
  } /* !async->queries_ongoing */
  ಅನ್ಯಥಾ
    /* wait for pending DoH transactions to complete */
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

out:
  Curl_doh_cleanup(data, async);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ doh_close(ಸಂರಚನಾ Curl_easy *data,
                      ಸಂರಚನಾ Curl_resolv_async *async)
{
  ಸಂರಚನಾ doh_probes *doh = async ? async->doh : ರಿಕ್ತ;
  ಯದಿ(doh && data->multi) {
    ಸಂರಚನಾ Curl_easy *probe_data;
    uint32_t mid;
    size_t slot;
    ಪ್ರತಿ(slot = 0; slot < DOH_SLOT_ಎಣಿಕೆ; slot++) {
      mid = doh->probe_mid[slot];
      ಯದಿ(mid == UINT32_MAX)
        ಅನುವರ್ತ;
      doh->probe_mid[slot] = UINT32_MAX;
      /* should have been called before data is removed from multi handle */
      DEBUGASSERT(data->multi);
      probe_data = data->multi ? Curl_multi_get_easy(data->multi, mid) : ರಿಕ್ತ;
      ಯದಿ(!probe_data) {
        DEBUGF(infof(data, "Curl_doh_close: xfer for mid=%u not found!", mid));
        ಅನುವರ್ತ;
      }
      probe_data->sub_xfer_done = ರಿಕ್ತ; /* No longer interested in result */
      /* data->multi might already be reset at this time */
      Curl_multi_remove_handle(data->multi, probe_data);
      Curl_close(&probe_data);
    }
    CURL_TRC_DNS(data, "[DoH] probe done");
  }
}

ಶೂನ್ಯ Curl_doh_cleanup(ಸಂರಚನಾ Curl_easy *data,
                      ಸಂರಚನಾ Curl_resolv_async *async)
{
  ಸಂರಚನಾ doh_probes *dohp = async->doh;
  ಯದಿ(dohp) {
    doh_close(data, async);
    curlx_safefree(async->doh);
  }
}

#endif /* CURL_DISABLE_DOH */
