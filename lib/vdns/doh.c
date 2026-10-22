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
#अन्तर्भाव "curl_setup.h"

#ifndef CURL_DISABLE_DOH

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curl_addrinfo.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "vdns/doh.h"
#अन्तर्भाव "vdns/httpsrr.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "escape.h"  /* for Curl_hexencode() */
#अन्तर्भाव "urlapi-int.h"

#define DNS_CLASS_IN 0x01

स्थिर शून्य doh_close(संरचना Curl_easy *data,
                      संरचना Curl_resolv_async *async);

#ifdef CURLVERBOSE
स्थिर स्थिरांक अक्षर * स्थिरांक doh_code_str[] = {
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

स्थिर स्थिरांक अक्षर *doh_strerror(DOHcode code)
{
  जर((माप_प्रकार)code < CURL_ARRAYSIZE(doh_code_str))
    परत doh_code_str[code];
  परत "bad error code";
}

स्थिर स्थिरांक अक्षर *doh_type2name(DNStype dnstype)
{
  निवड(dnstype) {
  स्थिती CURL_DNS_TYPE_A:
    परत "A";
  स्थिती CURL_DNS_TYPE_AAAA:
    परत "AAAA";
#ifdef USE_HTTPSRR
  स्थिती CURL_DNS_TYPE_HTTPS:
    परत "HTTPS";
#endif
  पूर्वनिर्धारित:
    परत "unknown";
  }
}

#endif /* CURLVERBOSE */

/* @unittest 1655
 */
UNITTEST DOHcode doh_req_encode(स्थिरांक अक्षर *host,
                                DNStype dnstype,
                                अचिन्हित अक्षर *dnsp,  /* buffer */
                                माप_प्रकार len,  /* buffer size */
                                माप_प्रकार *olen);  /* output length */
UNITTEST DOHcode doh_req_encode(स्थिरांक अक्षर *host,
                                DNStype dnstype,
                                अचिन्हित अक्षर *dnsp, /* buffer */
                                माप_प्रकार len,   /* buffer size */
                                माप_प्रकार *olen) /* output length */
{
  स्थिरांक माप_प्रकार hostlen = म_माप(host);
  अचिन्हित अक्षर *orig = dnsp;
  स्थिरांक अक्षर *hostp = host;

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

  माप_प्रकार expected_len;
  DEBUGASSERT(hostlen);
  expected_len = 12 + 1 + hostlen + 4;
  जर(host[hostlen - 1] != '.')
    expected_len++;

  जर(expected_len > DOH_MAX_DNSREQ_SIZE)
    परत DOH_DNS_NAME_TOO_LONG;

  जर(len < expected_len)
    परत DOH_TOO_SMALL_BUFFER;

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
  पर्यंत(*hostp) {
    माप_प्रकार labellen;
    स्थिरांक अक्षर *dot = म_अक्षर(hostp, '.');
    जर(dot)
      labellen = dot - hostp;
    अन्यथा
      labellen = म_माप(hostp);
    जर((labellen > 63) || (!labellen)) {
      /* label is too long or too short, error out */
      *olen = 0;
      परत DOH_DNS_BAD_LABEL;
    }
    /* label is non-empty, process it */
    *dnsp++ = (अचिन्हित अक्षर)labellen;
    स_नकल(dnsp, hostp, labellen);
    dnsp += labellen;
    hostp += labellen;
    /* advance past dot, but only if there is one */
    जर(dot)
      hostp++;
  } /* next label */

  *dnsp++ = 0; /* append zero-length label for root */

  /* There are assigned TYPE codes beyond 255: use range [1..65535] */
  *dnsp++ = (अचिन्हित अक्षर)(255 & (dnstype >> 8)); /* upper 8-bit TYPE */
  *dnsp++ = (अचिन्हित अक्षर)(255 & dnstype);        /* lower 8-bit TYPE */

  *dnsp++ = '\0'; /* upper 8-bit CLASS */
  *dnsp++ = DNS_CLASS_IN; /* IN - "the Internet" */

  *olen = dnsp - orig;

  /* verify that our estimation of length is valid, since
   * this has led to buffer overflows in this function */
  DEBUGASSERT(*olen == expected_len);
  परत DOH_OK;
}

स्थिर माप_प्रकार doh_probe_write_cb(अक्षर *contents, माप_प्रकार size, माप_प्रकार nmemb,
                                 शून्य *userp)
{
  माप_प्रकार realsize = size * nmemb;
  संरचना Curl_easy *data = userp;
  संरचना doh_request *doh_req = Curl_meta_get(data, CURL_EZM_DOH_PROBE);
  जर(!doh_req)
    परत CURL_WRITEFUNC_ERROR;

  जर(curlx_dyn_addn(&doh_req->resp_body, contents, realsize))
    परत 0;

  परत realsize;
}

स्थिर शून्य doh_probe_done(संरचना Curl_easy *doh,
                           संरचना Curl_easy *master, CURLcode result);
स्थिर शून्य doh_probe_dtor(स्थिरांक शून्य *किल्ली, माप_प्रकार klen, शून्य *e)
{
  (शून्य)किल्ली;
  (शून्य)klen;
  जर(e) {
    संरचना doh_request *doh_req = e;
    curl_slist_free_all(doh_req->req_hds);
    curlx_dyn_free(&doh_req->resp_body);
    curlx_free(e);
  }
}

#define ERROR_CHECK_SETOPT(x, y)                        \
  करा {                                                  \
    result = curl_easy_setopt((CURL *)doh, x, y);       \
    जर(result &&                                        \
       result != CURLE_NOT_BUILT_IN &&                  \
       result != CURLE_UNKNOWN_OPTION)                  \
      जा error;                                       \
  } पर्यंत(0)

स्थिर CURLcode doh_probe_run(संरचना Curl_easy *data,
                              DNStype dnstype,
                              स्थिरांक अक्षर *host,
                              स्थिरांक अक्षर *url, CURLM *multi,
                              uint32_t resolv_id,
                              uint32_t *pmid)
{
  संरचना Curl_easy *doh = रिक्त;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  संरचना doh_request *doh_req;
  DOHcode d;
  bool maybe_https = !curl_strnequal(url, STRCONST("http:"));

  *pmid = UINT32_MAX;

  doh_req = curlx_calloc(1, आकार(*doh_req));
  जर(!doh_req)
    परत CURLE_OUT_OF_MEMORY;
  doh_req->resolv_id = resolv_id;
  doh_req->dnstype = dnstype;
  curlx_dyn_init(&doh_req->resp_body, DYN_DOH_RESPONSE);

  d = doh_req_encode(host, dnstype, doh_req->req_body,
                     आकार(doh_req->req_body),
                     &doh_req->req_body_len);
  जर(d) {
    failf(data, "Failed to encode DoH packet [%d]", (पूर्णांक)d);
    result = CURLE_OUT_OF_MEMORY;
    जा error;
  }

  timeout_ms = Curl_timeleft_ms(data);
  जर(timeout_ms < 0) {
    result = CURLE_OPERATION_TIMEDOUT;
    जा error;
  }

  doh_req->req_hds =
    curl_slist_append(रिक्त, "Content-Type: application/dns-message");
  जर(!doh_req->req_hds) {
    result = CURLE_OUT_OF_MEMORY;
    जा error;
  }

  /* Curl_open() is the internal version of curl_easy_init() */
  result = Curl_open(&doh);
  जर(result)
    जा error;

  /* pass in the struct pointer via a local variable to please coverity and
     the gcc typecheck helpers */
  VERBOSE(doh->state.feat = &Curl_trc_feat_doh);
  ERROR_CHECK_SETOPT(CURLOPT_URL, url);
  ERROR_CHECK_SETOPT(CURLOPT_DEFAULT_PROTOCOL, "https");
  ERROR_CHECK_SETOPT(CURLOPT_WRITEFUNCTION, doh_probe_write_cb);
  ERROR_CHECK_SETOPT(CURLOPT_WRITEDATA, doh);
  ERROR_CHECK_SETOPT(CURLOPT_POSTFIELDS, doh_req->req_body);
  ERROR_CHECK_SETOPT(CURLOPT_POSTFIELDSIZE, (दीर्घ)doh_req->req_body_len);
  ERROR_CHECK_SETOPT(CURLOPT_HTTPHEADER, doh_req->req_hds);
#ifdef USE_HTTP2
  जर(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);
    ERROR_CHECK_SETOPT(CURLOPT_PIPEWAIT, 1L);
  }
#endif
#ifndef DEBUGBUILD
  /* enforce HTTPS if not debug */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
#अन्यथा
  /* in debug mode, also allow http */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
#endif
  ERROR_CHECK_SETOPT(CURLOPT_TIMEOUT_MS, (दीर्घ)timeout_ms);
  ERROR_CHECK_SETOPT(CURLOPT_SHARE, (CURLSH *)data->share);
  जर(data->set.err && data->set.err != मानक_त्रुटि)
    ERROR_CHECK_SETOPT(CURLOPT_STDERR, data->set.err);
  जर(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_doh))
    ERROR_CHECK_SETOPT(CURLOPT_VERBOSE, 1L);
  जर(data->set.no_signal)
    ERROR_CHECK_SETOPT(CURLOPT_NOSIGNAL, 1L);
  जर(data->set.fdebug)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGFUNCTION, data->set.fdebug);
  जर(data->set.debugdata)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGDATA, data->set.debugdata);

#ifdef USE_RESOLV_ARES
  जर(CURL_EASY_STR(data, STRING_DNS_SERVERS)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_SERVERS,
                       CURL_EASY_STR(data, STRING_DNS_SERVERS));
  }
  जर(CURL_EASY_STR(data, STRING_DNS_INTERFACE)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_INTERFACE,
                       CURL_EASY_STR(data, STRING_DNS_INTERFACE));
  }
  जर(CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP4,
                       CURL_EASY_STR(data, STRING_DNS_LOCAL_IP4));
  }
  जर(CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP6,
                       CURL_EASY_STR(data, STRING_DNS_LOCAL_IP6));
  }
#endif
  जर(maybe_https) {
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
    जर(CURL_EASY_STR(data, STRING_SSL_CAFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO,
                         CURL_EASY_STR(data, STRING_SSL_CAFILE));
    }
    जर(data->set.blobs[BLOB_CAINFO]) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO_BLOB, data->set.blobs[BLOB_CAINFO]);
    }
    जर(CURL_EASY_STR(data, STRING_SSL_CAPATH)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAPATH,
                         CURL_EASY_STR(data, STRING_SSL_CAPATH));
    }
    जर(CURL_EASY_STR(data, STRING_SSL_CRLFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CRLFILE,
                         CURL_EASY_STR(data, STRING_SSL_CRLFILE));
    }
    जर(data->set.ssl.certinfo)
      ERROR_CHECK_SETOPT(CURLOPT_CERTINFO, 1L);
    जर(data->set.ssl_fsslctx)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_FUNCTION, data->set.ssl_fsslctx);
    जर(data->set.ssl_fsslctxp)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_DATA, data->set.ssl_fsslctxp);
    जर(CURL_EASY_STR(data, STRING_SSL_EC_CURVES)) {
      ERROR_CHECK_SETOPT(CURLOPT_SSL_EC_CURVES,
                         CURL_EASY_STR(data, STRING_SSL_EC_CURVES));
    }
    (शून्य)curl_easy_setopt(doh, CURLOPT_SSL_OPTIONS,
                           ((दीर्घ)data->set.ssl.ssl_options &
                            ~CURLSSLOPT_AUTO_CLIENT_CERT));
  }

  doh->state.internal = TRUE;
  doh->master_mid = data->mid; /* master transfer of this one */
  doh->sub_xfer_done = doh_probe_done;

  result = Curl_meta_set(doh, CURL_EZM_DOH_PROBE, doh_req, doh_probe_dtor);
  doh_req = रिक्त; /* call took ownership */
  जर(result)
    जा error;

  /* DoH handles must not inherit private_data. The handles may be passed to
     the user via callbacks and the user will be able to identify them as
     internal handles because private data is not set. The user can then set
     private_data via CURLOPT_PRIVATE if they so choose. */
  DEBUGASSERT(!doh->set.private_data);

  जर(Curl_multi_add_handle(multi, doh))
    जा error;

  *pmid = doh->mid;
  परत CURLE_OK;

error:
  Curl_close(&doh);
  जर(doh_req)
    doh_probe_dtor(रिक्त, 0, doh_req);
  परत result;
}

/*
 * Curl_doh() starts a name resolve using DoH. It resolves a name and returns
 * a 'Curl_addrinfo *' with the address information.
 */

CURLcode Curl_doh(संरचना Curl_easy *data,
                  संरचना Curl_resolv_async *async)
{
  CURLcode result = CURLE_OK;
  संरचना doh_probes *dohp = रिक्त;
  माप_प्रकार i;

  DEBUGASSERT(!async->doh);
  DEBUGASSERT(async->peer->hostname[0]);
  जर(async->doh) {
    DEBUGASSERT(0); /* should not happen */
    Curl_doh_cleanup(data, async);
  }

  जर(!async->dns_queries)
    परत CURLE_BAD_FUNCTION_ARGUMENT;
#ifdef USE_HTTPSRR
  जर(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     (async->dns_queries & CURL_DNSQ_HTTPS)) {
    /* Can't mix those in the same async resolve */
    DEBUGASSERT(0);
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  }
#अन्यथा
  जर(async->dns_queries & CURL_DNSQ_HTTPS) {
    DEBUGASSERT(0);
    परत CURLE_NOT_BUILT_IN;
  }
#endif

  /* start clean, consider allocating this struct on demand */
  async->doh = dohp = curlx_calloc(1, आकार(संरचना doh_probes));
  जर(!dohp)
    परत CURLE_OUT_OF_MEMORY;

  साठी(i = 0; i < DOH_SLOT_मोजणी; ++i) {
    dohp->probe_rc[i] = DOH_OK;
    dohp->probe_mid[i] = UINT32_MAX;
  }

#ifdef USE_IPV6
  /* AAAA results have preference in happy eyeballing, trigger first */
  जर(async->dns_queries & CURL_DNSQ_AAAA) {
    /* create IPv6 DoH request */
    result = doh_probe_run(data, CURL_DNS_TYPE_AAAA,
                           async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH),
                           data->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV6]);
    जर(result)
      जा error;
    async->queries_ongoing++;
  }
#endif

  /* create IPv4 DoH request */
  जर(async->dns_queries & CURL_DNSQ_A) {
    result = doh_probe_run(data, CURL_DNS_TYPE_A,
                           async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH),
                           data->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV4]);
    जर(result)
      जा error;
    async->queries_ongoing++;
  }

#ifdef USE_HTTPSRR
  जर(async->dns_queries & CURL_DNSQ_HTTPS) {
    अक्षर *qname = रिक्त;
    जर(async->peer->port != PORT_HTTPS) {
      qname = curl_maprintf("_%u._https.%s",
                            async->peer->port, async->peer->hostname);
      जर(!qname)
        जा error;
    }
    result = doh_probe_run(data, CURL_DNS_TYPE_HTTPS,
                           qname ? qname : async->peer->hostname,
                           CURL_EASY_STR(data, STRING_DOH), data->multi,
                           async->id,
                           &dohp->probe_mid[DOH_SLOT_HTTPS_RR]);
    curlx_free(qname);
    जर(result)
      जा error;
    async->queries_ongoing++;
  }
#endif
  परत CURLE_OK;

error:
  Curl_doh_cleanup(data, async);
  परत result;
}

स्थिर DOHcode doh_skipqname(स्थिरांक अचिन्हित अक्षर *doh, माप_प्रकार dohlen,
                             अचिन्हित पूर्णांक *indexp)
{
  अचिन्हित अक्षर length;
  करा {
    जर(dohlen < (*indexp + 1))
      परत DOH_DNS_OUT_OF_RANGE;
    length = doh[*indexp];
    जर((length & 0xc0) == 0xc0) {
      /* name pointer, advance over it and be done */
      जर(dohlen < (*indexp + 2))
        परत DOH_DNS_OUT_OF_RANGE;
      *indexp += 2;
      थांबा;
    }
    जर(length & 0xc0)
      परत DOH_DNS_BAD_LABEL;
    जर(dohlen < (*indexp + 1 + length))
      परत DOH_DNS_OUT_OF_RANGE;
    *indexp += (अचिन्हित पूर्णांक)(1 + length);
  } पर्यंत(length);
  परत DOH_OK;
}

स्थिर अचिन्हित लघु doh_get16bit(स्थिरांक अचिन्हित अक्षर *doh,
                                   अचिन्हित पूर्णांक index)
{
  परत (अचिन्हित लघु)((doh[index] << 8) | doh[index + 1]);
}

स्थिर अचिन्हित पूर्णांक doh_get32bit(स्थिरांक अचिन्हित अक्षर *doh, अचिन्हित पूर्णांक index)
{
  /* make clang and gcc optimize this to bswap by incrementing
     the pointer first. */
  doh += index;

  /* avoid undefined behavior by casting to unsigned before shifting
     24 bits, possibly into the sign bit. codegen is same, but
     ub sanitizer will not be upset */
  परत ((अचिन्हित)doh[0] << 24) | ((अचिन्हित)doh[1] << 16) |
         ((अचिन्हित)doh[2] << 8) | doh[3];
}

स्थिर शून्य doh_store_a(स्थिरांक अचिन्हित अक्षर *doh, पूर्णांक index,
                        संरचना dohentry *d)
{
  /* silently ignore addresses over the limit */
  जर(d->numaddr < DOH_MAX_ADDR) {
    संरचना dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_A;
    स_नकल(&a->ip.v4, &doh[index], 4);
    d->numaddr++;
  }
}

स्थिर शून्य doh_store_aaaa(स्थिरांक अचिन्हित अक्षर *doh, पूर्णांक index,
                           संरचना dohentry *d)
{
  /* silently ignore addresses over the limit */
  जर(d->numaddr < DOH_MAX_ADDR) {
    संरचना dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_AAAA;
    स_नकल(&a->ip.v6, &doh[index], 16);
    d->numaddr++;
  }
}

#ifdef USE_HTTPSRR
स्थिर DOHcode doh_store_https(स्थिरांक अचिन्हित अक्षर *doh, पूर्णांक index,
                               संरचना dohentry *d, uint16_t len)
{
  /* silently ignore RRs over the limit */
  जर(d->numhttps_rrs < DOH_MAX_HTTPS) {
    संरचना dohhttps_rr *h = &d->https_rrs[d->numhttps_rrs];
    h->val = curlx_memdup(&doh[index], len);
    जर(!h->val)
      परत DOH_OUT_OF_MEM;
    h->len = len;
    d->numhttps_rrs++;
  }
  परत DOH_OK;
}
#endif

स्थिर DOHcode doh_rdata(स्थिरांक अचिन्हित अक्षर *doh,
                         अचिन्हित लघु rdlength,
                         अचिन्हित लघु type,
                         पूर्णांक index,
                         संरचना dohentry *d)
{
  /* RDATA
     - A (TYPE 1): 4 bytes
     - AAAA (TYPE 28): 16 bytes
     - HTTPS (TYPE 65): N bytes */
  निवड(type) {
  स्थिती CURL_DNS_TYPE_A:
    जर(rdlength != 4)
      परत DOH_DNS_RDATA_LEN;
    doh_store_a(doh, index, d);
    थांबा;
  स्थिती CURL_DNS_TYPE_AAAA:
    जर(rdlength != 16)
      परत DOH_DNS_RDATA_LEN;
    doh_store_aaaa(doh, index, d);
    थांबा;
#ifdef USE_HTTPSRR
  स्थिती CURL_DNS_TYPE_HTTPS:
    जर(rdlength < 3)
      परत DOH_DNS_RDATA_LEN;
    परत doh_store_https(doh, index, d, rdlength);
#endif
  पूर्वनिर्धारित:
    /* unsupported type, or type we do not store, skip it */
    थांबा;
  }
  परत DOH_OK;
}

/* @unittest 1655 */
UNITTEST शून्य de_init(संरचना dohentry *de);
UNITTEST शून्य de_init(संरचना dohentry *de)
{
  स_रखो(de, 0, आकार(*de));
  de->ttl = पूर्णांक_उच्च;
}

/* TTL value cap */
#define MAX_DNS_TTL 86400U /* 24 hours */
/* @unittest 1650 */
UNITTEST DOHcode doh_resp_decode(स्थिरांक अचिन्हित अक्षर *doh,
                                 माप_प्रकार dohlen,
                                 DNStype dnstype,
                                 संरचना dohentry *d);
UNITTEST DOHcode doh_resp_decode(स्थिरांक अचिन्हित अक्षर *doh,
                                 माप_प्रकार dohlen,
                                 DNStype dnstype,
                                 संरचना dohentry *d)
{
  अचिन्हित अक्षर rcode;
  अचिन्हित लघु qdcount;
  अचिन्हित लघु ancount;
  अचिन्हित लघु type = 0;
  अचिन्हित लघु rdlength;
  अचिन्हित लघु nscount;
  अचिन्हित लघु arcount;
  अचिन्हित पूर्णांक index = 12;
  DOHcode rc;

  जर(dohlen < 12)
    परत DOH_TOO_SMALL_BUFFER; /* too small */
  जर(!doh || doh[0] || doh[1])
    परत DOH_DNS_BAD_ID; /* bad ID */
  rcode = doh[3] & 0x0f;
  जर(rcode == 3)
    परत DOH_DNS_NXDOMAIN; /* name does not exist */
  जर(rcode)
    परत DOH_DNS_BAD_RCODE; /* bad rcode */

  qdcount = doh_get16bit(doh, 4);
  पर्यंत(qdcount) {
    rc = doh_skipqname(doh, dohlen, &index);
    जर(rc)
      परत rc; /* bad qname */
    जर(dohlen < (index + 4))
      परत DOH_DNS_OUT_OF_RANGE;
    index += 4; /* skip question's type and class */
    qdcount--;
  }

  ancount = doh_get16bit(doh, 6);
  पर्यंत(ancount) {
    अचिन्हित लघु dnsclass;
    अचिन्हित पूर्णांक ttl;

    rc = doh_skipqname(doh, dohlen, &index);
    जर(rc)
      परत rc; /* bad qname */

    जर(dohlen < (index + 2))
      परत DOH_DNS_OUT_OF_RANGE;

    type = doh_get16bit(doh, index);
    जर((type != CURL_DNS_TYPE_CNAME) &&  /* may be synthesized from DNAME */
       (type != CURL_DNS_TYPE_DNAME) &&  /* if present, accept and ignore */
       (type != dnstype))
      /* Not the same type as was asked for, nor CNAME nor DNAME */
      परत DOH_DNS_UNEXPECTED_TYPE;
    index += 2;

    जर(dohlen < (index + 2))
      परत DOH_DNS_OUT_OF_RANGE;
    dnsclass = doh_get16bit(doh, index);
    जर(DNS_CLASS_IN != dnsclass)
      परत DOH_DNS_UNEXPECTED_CLASS; /* unsupported */
    index += 2;

    जर(dohlen < (index + 4))
      परत DOH_DNS_OUT_OF_RANGE;

    ttl = doh_get32bit(doh, index);
    जर(ttl > MAX_DNS_TTL)
      ttl = MAX_DNS_TTL;
    जर(ttl < d->ttl)
      d->ttl = ttl;
    index += 4;

    जर(dohlen < (index + 2))
      परत DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    जर(dohlen < (index + rdlength))
      परत DOH_DNS_OUT_OF_RANGE;

    rc = doh_rdata(doh, rdlength, type, (पूर्णांक)index, d);
    जर(rc)
      परत rc;
    index += rdlength;
    ancount--;
  }

  nscount = doh_get16bit(doh, 8);
  पर्यंत(nscount) {
    rc = doh_skipqname(doh, dohlen, &index);
    जर(rc)
      परत rc; /* bad qname */

    जर(dohlen < (index + 8))
      परत DOH_DNS_OUT_OF_RANGE;

    index += 2 + 2 + 4; /* type, dnsclass and ttl */

    जर(dohlen < (index + 2))
      परत DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    जर(dohlen < (index + rdlength))
      परत DOH_DNS_OUT_OF_RANGE;
    index += rdlength;
    nscount--;
  }

  arcount = doh_get16bit(doh, 10);
  पर्यंत(arcount) {
    rc = doh_skipqname(doh, dohlen, &index);
    जर(rc)
      परत rc; /* bad qname */

    जर(dohlen < (index + 8))
      परत DOH_DNS_OUT_OF_RANGE;

    index += 2 + 2 + 4; /* type, dnsclass and ttl */

    जर(dohlen < (index + 2))
      परत DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, index);
    index += 2;
    जर(dohlen < (index + rdlength))
      परत DOH_DNS_OUT_OF_RANGE;
    index += rdlength;
    arcount--;
  }

  जर(index != dohlen)
    परत DOH_DNS_MALFORMAT; /* something is wrong */

  परत DOH_OK; /* ok */
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
स्थिर CURLcode doh2ai(स्थिरांक संरचना dohentry *de, स्थिरांक अक्षर *hostname,
                       पूर्णांक port, संरचना Curl_addrinfo **aip)
{
  संरचना Curl_addrinfo *ai;
  संरचना Curl_addrinfo *prevai = रिक्त;
  संरचना Curl_addrinfo *firstai = रिक्त;
  संरचना sockaddr_in *addr;
#ifdef USE_IPV6
  संरचना sockaddr_in6 *addr6;
#endif
  माप_प्रकार hostlen = म_माप(hostname) + 1; /* include null-terminator */
  CURLcode result = CURLE_OK;
  पूर्णांक i;

  साठी(i = 0; i < de->numaddr; i++) {
    माप_प्रकार ss_size;
    CURL_SA_FAMILY_T addrtype;
    जर(de->addr[i].type == CURL_DNS_TYPE_AAAA) {
#ifndef USE_IPV6
      /* we cannot handle IPv6 addresses */
      सुरूठेवा;
#अन्यथा
      ss_size = आकार(संरचना sockaddr_in6);
      addrtype = AF_INET6;
#endif
    }
    अन्यथा {
      ss_size = आकार(संरचना sockaddr_in);
      addrtype = AF_INET;
    }

    ai = curlx_calloc(1, आकार(संरचना Curl_addrinfo) + ss_size + hostlen);
    जर(!ai) {
      result = CURLE_OUT_OF_MEMORY;
      थांबा;
    }
    ai->ai_addr = (शून्य *)((अक्षर *)ai + आकार(संरचना Curl_addrinfo));
    ai->ai_canonname = (शून्य *)((अक्षर *)ai->ai_addr + ss_size);
    स_नकल(ai->ai_canonname, hostname, hostlen);

    जर(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    जर(prevai)
      /* make the previous entry point to this */
      prevai->ai_next = ai;

    ai->ai_family = addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_size;

    /* leave the rest of the struct filled with zero */

    निवड(ai->ai_family) {
    स्थिती AF_INET:
      addr = (शून्य *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(आकार(संरचना in_addr) == आकार(de->addr[i].ip.v4));
      स_नकल(&addr->sin_addr, &de->addr[i].ip.v4, आकार(संरचना in_addr));
      addr->sin_family = addrtype;
      addr->sin_port = htons((अचिन्हित लघु)port);
      थांबा;

#ifdef USE_IPV6
    स्थिती AF_INET6:
      addr6 = (शून्य *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(आकार(संरचना in6_addr) == आकार(de->addr[i].ip.v6));
      स_नकल(&addr6->sin6_addr, &de->addr[i].ip.v6, आकार(संरचना in6_addr));
      addr6->sin6_family = addrtype;
      addr6->sin6_port = htons((अचिन्हित लघु)port);
      थांबा;
#endif
    }

    prevai = ai;
  }

  जर(result) {
    Curl_freeaddrinfo(firstai);
    firstai = रिक्त;
  }
  *aip = firstai;

  परत result;
}

/* @unittest 1655 */
UNITTEST शून्य de_cleanup(संरचना dohentry *d);
UNITTEST शून्य de_cleanup(संरचना dohentry *d)
{
#ifdef USE_HTTPSRR
  पूर्णांक i = 0;
  साठी(i = 0; i < d->numhttps_rrs; i++)
    curlx_safefree(d->https_rrs[i].val);
#अन्यथा
  (शून्य)d;
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
स्थिर CURLcode doh_decode_rdata_name(स्थिरांक अचिन्हित अक्षर **buf,
                                      माप_प्रकार *remaining, अक्षर **dnsname)
{
  स्थिरांक अचिन्हित अक्षर *cp = रिक्त;
  माप_प्रकार rem = 0;
  अचिन्हित अक्षर clen = 0; /* chunk len */
  संरचना dynbuf thename;

  DEBUGASSERT(buf && remaining && dnsname);
  जर(!buf || !remaining || !dnsname || !*remaining)
    परत CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&thename, CURL_MAXLEN_HOST_NAME);
  rem = *remaining;
  cp = *buf;
  clen = *cp++;
  /* RFC 9460 says it must be uncompressed */
  जर(clen > 63)
    परत CURLE_WEIRD_SERVER_REPLY;

  जर(clen == 0) {
    /* special case - return "." as name */
    जर(curlx_dyn_addn(&thename, ".", 1))
      परत CURLE_OUT_OF_MEMORY;
  }
  पर्यंत(clen) {
    जर(clen >= rem) {
      curlx_dyn_free(&thename);
      परत CURLE_OUT_OF_MEMORY;
    }
    जर(curlx_dyn_addn(&thename, cp, clen) ||
       curlx_dyn_addn(&thename, ".", 1))
      परत CURLE_TOO_LARGE;

    cp += clen;
    rem -= (clen + 1);
    जर(rem <= 0) {
      curlx_dyn_free(&thename);
      परत CURLE_OUT_OF_MEMORY;
    }
    clen = *cp++;
    जर(clen > 63) {
      /* invalid format */
      curlx_dyn_free(&thename);
      परत CURLE_WEIRD_SERVER_REPLY;
    }
  }
  *buf = cp;
  *remaining = rem - 1;
  *dnsname = curlx_dyn_ptr(&thename);
  परत CURLE_OK;
}

/* scan for byte values <= 32 or 127 */
स्थिर CURLcode junkscan(स्थिरांक अक्षर *url)
{
  स्थिरांक अचिन्हित अक्षर *p = (स्थिरांक अचिन्हित अक्षर *)url;
  पर्यंत(*p) {
    जर(*p <= 0x20 || *p == 127)
      परत CURLE_WEIRD_SERVER_REPLY;
    p++;
  }
  परत CURLE_OK;
}

/* @unittest 1658 */
UNITTEST CURLcode doh_resp_decode_httpsrr(संरचना Curl_easy *data,
                                          स्थिरांक अचिन्हित अक्षर *cp, माप_प्रकार len,
                                          संरचना Curl_https_rrinfo **hrr);
UNITTEST CURLcode doh_resp_decode_httpsrr(संरचना Curl_easy *data,
                                          स्थिरांक अचिन्हित अक्षर *cp, माप_प्रकार len,
                                          संरचना Curl_https_rrinfo **hrr)
{
  uint16_t pcode = 0, plen = 0;
  uint32_t expected_min_pcode = 0;
  संरचना Curl_https_rrinfo *lhrr = रिक्त;
  अक्षर *dnsname = रिक्त;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (शून्य)data;
  *hrr = रिक्त;
  जर(len <= 2)
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  lhrr = curlx_calloc(1, आकार(संरचना Curl_https_rrinfo));
  जर(!lhrr)
    परत CURLE_OUT_OF_MEMORY;
  lhrr->priority = doh_get16bit(cp, 0);
  cp += 2;
  len -= 2;
  जर(doh_decode_rdata_name(&cp, &len, &dnsname) != CURLE_OK)
    जा err;
  lhrr->target = dnsname;
  result = junkscan(dnsname);
  जर(result) {
    /* unacceptable hostname content */
    जा err;
  }
  पर्यंत(len >= 4) {
    pcode = doh_get16bit(cp, 0);
    plen = doh_get16bit(cp, 2);
    cp += 4;
    len -= 4;
    जर(pcode < expected_min_pcode || plen > len) {
      result = CURLE_WEIRD_SERVER_REPLY;
      जा err;
    }
    result = Curl_httpsrr_set(lhrr, pcode, cp, plen);
    जर(result)
      जा err;
    Curl_httpsrr_trace(data, lhrr);
    cp += plen;
    len -= plen;
    expected_min_pcode = pcode + 1;
  }
  *hrr = lhrr;
  परत CURLE_OK;
err:
  Curl_httpsrr_destroy(lhrr);
  परत result;
}

#endif /* USE_HTTPSRR */

/* called from multi when a sub transfer, e.g. doh probe, is done.
 * Parse the response and set the results in the `async` context
 * of master, using the id from the probe's CURL_EZM_DOH_PROBE
 * meta data. */
स्थिर शून्य doh_probe_done(संरचना Curl_easy *doh,
                           संरचना Curl_easy *master, CURLcode result)
{
  संरचना Curl_resolv_async *async = रिक्त;
  संरचना doh_probes *dohp = रिक्त;
  संरचना doh_request *doh_req = रिक्त;
  संरचना Curl_addrinfo **pdest_ai;
  संरचना dohentry de;
  पूर्णांक slot, httpcode;

  de_init(&de);
  doh_req = Curl_meta_get(doh, CURL_EZM_DOH_PROBE);
  जर(!doh_req) {
    /* transfer `doh` is not a DoH probe. */
    DEBUGASSERT(0);
    जा out;
  }

  async = Curl_async_get(master, doh_req->resolv_id);
  जर(!async) {
    CURL_TRC_DNS(master, "[%u] ignoring outdated DoH response",
                 doh_req->resolv_id);
    जा out;
  }
  dohp = async->doh;

  साठी(slot = 0; slot < DOH_SLOT_मोजणी; ++slot) {
    जर(dohp->probe_mid[slot] == doh->mid)
      थांबा;
  }
  /* We really should have found the slot where to store the response */
  जर(slot >= DOH_SLOT_मोजणी) {
    failf(master, "DoH: unknown sub request done");
    DEBUGASSERT(0);
    जा out;
  }

  async->queries_ongoing--;
  dohp = async->doh;
  httpcode = doh->info.httpcode;
  निवड(slot) {
  स्थिती DOH_SLOT_IPV4:
    async->dns_responses |= CURL_DNSQ_A;
    थांबा;
#ifdef USE_IPV6
  स्थिती DOH_SLOT_IPV6:
    async->dns_responses |= CURL_DNSQ_AAAA;
    थांबा;
#endif
#ifdef USE_HTTPSRR
  स्थिती DOH_SLOT_HTTPS_RR:
    async->dns_responses |= CURL_DNSQ_HTTPS;
    थांबा;
#endif
  पूर्वनिर्धारित:
    DEBUGASSERT(0);
    थांबा;
  }

  जर(result) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: %s",
          doh_type2name(doh_req->dnstype), curl_easy_strerror(result));
    जा out;
  }
  अन्यथा जर((httpcode < 200) || (httpcode >= 300)) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: HTTP status %d",
          doh_type2name(doh_req->dnstype), httpcode);
    जा out;
  }

  dohp->probe_rc[slot] = doh_resp_decode(curlx_dyn_uptr(&doh_req->resp_body),
                                         curlx_dyn_len(&doh_req->resp_body),
                                         doh_req->dnstype, &de);
  जर(dohp->probe_rc[slot]) {
#ifdef USE_HTTPSRR
    जर((dohp->probe_rc[slot] == DOH_NO_CONTENT) &&
       (doh_req->dnstype == CURL_DNS_TYPE_HTTPS)) {
      dohp->probe_rc[slot] = DOH_DNS_NXDOMAIN;
    }
#endif
    infof(doh, "[DoH] [%s] error decoding response: %s",
          doh_type2name(doh_req->dnstype),
          doh_strerror(dohp->probe_rc[slot]));
    जा out;
  }

  जर(doh_req->dnstype == CURL_DNS_TYPE_A)
    pdest_ai = &async->ai_A;
  अन्यथा जर(doh_req->dnstype == CURL_DNS_TYPE_AAAA)
    pdest_ai = &async->ai_AAAA;
  अन्यथा
    pdest_ai = रिक्त;

  जर(pdest_ai && de.numaddr) {
    जर(*pdest_ai) {
      Curl_freeaddrinfo(*pdest_ai);
      *pdest_ai = रिक्त;
    }
    result = doh2ai(&de, async->peer->hostname, async->peer->port, pdest_ai);
    जर(result) { /* hard failure on our side, fail completely */
      infof(doh, "[DoH] [%s] error creating addrinfo: %s",
            doh_type2name(doh_req->dnstype), curl_easy_strerror(result));
      dohp->probe_rc[slot] = DOH_OOM;
      async->result = result;
    }
  }
#ifdef USE_HTTPSRR
  अन्यथा जर((doh_req->dnstype == CURL_DNS_TYPE_HTTPS) && de.numhttps_rrs) {
    CURL_TRC_DNS(doh, "[HTTPS] got %d records", de.numhttps_rrs);
    result = doh_resp_decode_httpsrr(doh, de.https_rrs->val,
                                     de.https_rrs->len, &async->httpsrr);
    जर(result) {
      dohp->probe_rc[slot] = DOH_HTTP_FAILED;
      infof(doh, "[DoH] error decoding HTTPS RR: %s",
            curl_easy_strerror(result));
      जा out;
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

CURLcode Curl_doh_take_result(संरचना Curl_easy *data,
                              संरचना Curl_resolv_async *async,
                              संरचना Curl_dns_entry **pdns)
{
  संरचना doh_probes *dohp = async->doh;
  CURLcode result = CURLE_OK;

  *pdns = रिक्त; /* defaults to no response */
  जर(!dohp)
    परत CURLE_OUT_OF_MEMORY;

  async->negative_answer = FALSE;
  जर(async->result) {
    result = async->result;
    जा out;
  }

  जर(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     dohp->probe_mid[DOH_SLOT_IPV4] == UINT32_MAX &&
     dohp->probe_mid[DOH_SLOT_IPV6] == UINT32_MAX) {
    failf(data, "Could not DoH-resolve: %s", async->peer->hostname);
    परत async->for_proxy ?
      CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
  }
  अन्यथा जर(!async->queries_ongoing) {
    संरचना Curl_dns_entry *dns = रिक्त;
    bool negative = TRUE;
    पूर्णांक slot;

    /* remove DoH handles from multi handle and close them */
    doh_close(data, async);
    /* parse the responses, create the struct and return it! */
    साठी(slot = 0; slot < DOH_SLOT_मोजणी; slot++) {
      /* Failing without an NXDOMAIN answer - a SERVFAIL-class rcode or
         an undecodable response - says nothing about the name. Such a
         failure must not be cached as a negative entry. */
      जर(dohp->probe_rc[slot] && (dohp->probe_rc[slot] != DOH_DNS_NXDOMAIN))
        negative = FALSE;
    } /* next slot */

    जर(async->ai_A || async->ai_AAAA) {
      dns = Curl_dnsc_mk_addr2(
        data, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
      जर(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        जा out;
      }
    }
#ifdef USE_HTTPSRR
    अन्यथा जर((async->dns_queries & CURL_DNSQ_HTTPS) &&
            !dohp->probe_rc[DOH_SLOT_HTTPS_RR]) {
      Curl_httpsrr_trace(data, async->httpsrr);
      dns = Curl_dnsc_mk_https(data, &async->httpsrr, async->peer);
      जर(!dns) {
        result = CURLE_OUT_OF_MEMORY;
        जा out;
      }
    }
#endif /* USE_HTTPSRR */
    अन्यथा {
      /* every query failed. Only NXDOMAIN answers for all of them
         make this a negative answer, eligible for caching. */
      async->negative_answer = negative;
      result = async->for_proxy ?
        CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
    }

    /* and add the entry to the cache */
    जर(dns)
      result = Curl_dnscache_add(data, dns);
    *pdns = dns;
  } /* !async->queries_ongoing */
  अन्यथा
    /* wait for pending DoH transactions to complete */
    परत CURLE_AGAIN;

out:
  Curl_doh_cleanup(data, async);
  परत result;
}

स्थिर शून्य doh_close(संरचना Curl_easy *data,
                      संरचना Curl_resolv_async *async)
{
  संरचना doh_probes *doh = async ? async->doh : रिक्त;
  जर(doh && data->multi) {
    संरचना Curl_easy *probe_data;
    uint32_t mid;
    माप_प्रकार slot;
    साठी(slot = 0; slot < DOH_SLOT_मोजणी; slot++) {
      mid = doh->probe_mid[slot];
      जर(mid == UINT32_MAX)
        सुरूठेवा;
      doh->probe_mid[slot] = UINT32_MAX;
      /* should have been called before data is removed from multi handle */
      DEBUGASSERT(data->multi);
      probe_data = data->multi ? Curl_multi_get_easy(data->multi, mid) : रिक्त;
      जर(!probe_data) {
        DEBUGF(infof(data, "Curl_doh_close: xfer for mid=%u not found!", mid));
        सुरूठेवा;
      }
      probe_data->sub_xfer_done = रिक्त; /* No longer interested in result */
      /* data->multi might already be reset at this time */
      Curl_multi_remove_handle(data->multi, probe_data);
      Curl_close(&probe_data);
    }
    CURL_TRC_DNS(data, "[DoH] probe done");
  }
}

शून्य Curl_doh_cleanup(संरचना Curl_easy *data,
                      संरचना Curl_resolv_async *async)
{
  संरचना doh_probes *dohp = async->doh;
  जर(dohp) {
    doh_close(data, async);
    curlx_safefree(async->doh);
  }
}

#endif /* CURL_DISABLE_DOH */
