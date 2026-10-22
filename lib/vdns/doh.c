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
#समावेश "curl_setup.h"

#अगर_अघोषित CURL_DISABLE_DOH

#समावेश "urldata.h"
#समावेश "curl_addrinfo.h"
#समावेश "curl_trc.h"
#समावेश "multiif.h"
#समावेश "url.h"
#समावेश "connect.h"
#समावेश "vdns/doh.h"
#समावेश "vdns/httpsrr.h"
#समावेश "curlx/strdup.h"
#समावेश "curlx/dynbuf.h"
#समावेश "escape.h"  /* for Curl_hexencode() */
#समावेश "urlapi-int.h"

#घोषणा DNS_CLASS_IN 0x01

अटल व्योम doh_close(काष्ठा Curl_easy *तथ्य,
                      काष्ठा Curl_resolv_async *async);

#अगर_घोषित CURLVERBOSE
अटल स्थिर अक्षर * स्थिर doh_code_str[] = {
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

अटल स्थिर अक्षर *doh_strerror(DOHcode code)
{
  अगर((माप_प्रकार)code < CURL_ARRAYSIZE(doh_code_str))
    वापस doh_code_str[code];
  वापस "bad error code";
}

अटल स्थिर अक्षर *doh_type2name(DNStype dnstype)
{
  चयन(dnstype) {
  हाल CURL_DNS_TYPE_A:
    वापस "A";
  हाल CURL_DNS_TYPE_AAAA:
    वापस "AAAA";
#अगर_घोषित USE_HTTPSRR
  हाल CURL_DNS_TYPE_HTTPS:
    वापस "HTTPS";
#पूर्ण_अगर
  शेष:
    वापस "unknown";
  }
}

#पूर्ण_अगर /* CURLVERBOSE */

/* @unittest 1655
 */
UNITTEST DOHcode doh_req_encode(स्थिर अक्षर *host,
                                DNStype dnstype,
                                अचिन्हित अक्षर *dnsp,  /*बफर*/
                                माप_प्रकार len,  /* buffer size */
                                माप_प्रकार *olen);  /* output length */
UNITTEST DOHcode doh_req_encode(स्थिर अक्षर *host,
                                DNStype dnstype,
                                अचिन्हित अक्षर *dnsp, /*बफर*/
                                माप_प्रकार len,   /* buffer size */
                                माप_प्रकार *olen) /* output length */
{
  स्थिर माप_प्रकार hostlen = म_माप(host);
  अचिन्हित अक्षर *orig = dnsp;
  स्थिर अक्षर *hostp = host;

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
  अगर(host[hostlen - 1] != '.')
    expected_len++;

  अगर(expected_len > DOH_MAX_DNSREQ_आकार)
    वापस DOH_DNS_NAME_TOO_LONG;

  अगर(len < expected_len)
    वापस DOH_TOO_SMALL_बफर;

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
  जबतक(*hostp) {
    माप_प्रकार labellen;
    स्थिर अक्षर *dot = म_अक्षर(hostp, '.');
    अगर(dot)
      labellen = dot - hostp;
    अन्यथा
      labellen = म_माप(hostp);
    अगर((labellen > 63) || (!labellen)) {
      /* label is too long or too short, error out */
      *olen = 0;
      वापस DOH_DNS_BAD_LABEL;
    }
    /* label is non-empty, process it */
    *dnsp++ = (अचिन्हित अक्षर)labellen;
    स_नकल(dnsp, hostp, labellen);
    dnsp += labellen;
    hostp += labellen;
    /* advance past dot, but only if there is one */
    अगर(dot)
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
  वापस DOH_OK;
}

अटल माप_प्रकार doh_probe_write_cb(अक्षर *contents, माप_प्रकार आकार, माप_प्रकार nmemb,
                                 व्योम *userp)
{
  माप_प्रकार realsize = आकार * nmemb;
  काष्ठा Curl_easy *तथ्य = userp;
  काष्ठा doh_request *doh_req = Curl_meta_get(तथ्य, CURL_EZM_DOH_PROBE);
  अगर(!doh_req)
    वापस CURL_WRITEFUNC_ERROR;

  अगर(curlx_dyn_addn(&doh_req->resp_body, contents, realsize))
    वापस 0;

  वापस realsize;
}

अटल व्योम doh_probe_done(काष्ठा Curl_easy *doh,
                           काष्ठा Curl_easy *master, CURLcode परिणाम);
अटल व्योम doh_probe_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *e)
{
  (व्योम)कुंजी;
  (व्योम)klen;
  अगर(e) {
    काष्ठा doh_request *doh_req = e;
    curl_slist_free_all(doh_req->req_hds);
    curlx_dyn_free(&doh_req->resp_body);
    curlx_free(e);
  }
}

#घोषणा ERROR_CHECK_SETOPT(x, y)                        \
  करो {                                                  \
    परिणाम = curl_easy_setopt((CURL *)doh, x, y);       \
    अगर(परिणाम &&                                        \
       परिणाम != CURLE_NOT_BUILT_IN &&                  \
       परिणाम != CURLE_UNKNOWN_OPTION)                  \
      जाओ error;                                       \
  } जबतक(0)

अटल CURLcode doh_probe_run(काष्ठा Curl_easy *तथ्य,
                              DNStype dnstype,
                              स्थिर अक्षर *host,
                              स्थिर अक्षर *url, CURLM *multi,
                              uint32_t resolv_id,
                              uint32_t *pmid)
{
  काष्ठा Curl_easy *doh = शून्य;
  CURLcode परिणाम = CURLE_OK;
  timediff_t timeout_ms;
  काष्ठा doh_request *doh_req;
  DOHcode d;
  bool maybe_https = !curl_strnequal(url, STRCONST("http:"));

  *pmid = UINT32_MAX;

  doh_req = curlx_calloc(1, माप(*doh_req));
  अगर(!doh_req)
    वापस CURLE_OUT_OF_MEMORY;
  doh_req->resolv_id = resolv_id;
  doh_req->dnstype = dnstype;
  curlx_dyn_init(&doh_req->resp_body, DYN_DOH_RESPONSE);

  d = doh_req_encode(host, dnstype, doh_req->req_body,
                     माप(doh_req->req_body),
                     &doh_req->req_body_len);
  अगर(d) {
    failf(तथ्य, "Failed to encode DoH packet [%d]", (पूर्णांक)d);
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ error;
  }

  timeout_ms = Curl_timeleft_ms(तथ्य);
  अगर(timeout_ms < 0) {
    परिणाम = CURLE_OPERATION_TIMEDOUT;
    जाओ error;
  }

  doh_req->req_hds =
    curl_slist_append(शून्य, "Content-Type: application/dns-message");
  अगर(!doh_req->req_hds) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ error;
  }

  /* Curl_open() is the internal version of curl_easy_init() */
  परिणाम = Curl_open(&doh);
  अगर(परिणाम)
    जाओ error;

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
#अगर_घोषित USE_HTTP2
  अगर(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);
    ERROR_CHECK_SETOPT(CURLOPT_PIPEWAIT, 1L);
  }
#पूर्ण_अगर
#अगर_अघोषित DEBUGBUILD
  /* enforce HTTPS if not debug */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
#अन्यथा
  /* in debug mode, also allow http */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
#पूर्ण_अगर
  ERROR_CHECK_SETOPT(CURLOPT_TIMEOUT_MS, (दीर्घ)timeout_ms);
  ERROR_CHECK_SETOPT(CURLOPT_SHARE, (CURLSH *)तथ्य->share);
  अगर(तथ्य->set.err && तथ्य->set.err != मानक_त्रुटि)
    ERROR_CHECK_SETOPT(CURLOPT_STDERR, तथ्य->set.err);
  अगर(Curl_trc_ft_is_verbose(तथ्य, &Curl_trc_feat_doh))
    ERROR_CHECK_SETOPT(CURLOPT_VERBOSE, 1L);
  अगर(तथ्य->set.no_signal)
    ERROR_CHECK_SETOPT(CURLOPT_NOSIGNAL, 1L);
  अगर(तथ्य->set.fdebug)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGFUNCTION, तथ्य->set.fdebug);
  अगर(तथ्य->set.debugdata)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGDATA, तथ्य->set.debugdata);

#अगर_घोषित USE_RESOLV_ARES
  अगर(CURL_EASY_STR(तथ्य, STRING_DNS_SERVERS)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_SERVERS,
                       CURL_EASY_STR(तथ्य, STRING_DNS_SERVERS));
  }
  अगर(CURL_EASY_STR(तथ्य, STRING_DNS_INTERFACE)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_INTERFACE,
                       CURL_EASY_STR(तथ्य, STRING_DNS_INTERFACE));
  }
  अगर(CURL_EASY_STR(तथ्य, STRING_DNS_LOCAL_IP4)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP4,
                       CURL_EASY_STR(तथ्य, STRING_DNS_LOCAL_IP4));
  }
  अगर(CURL_EASY_STR(तथ्य, STRING_DNS_LOCAL_IP6)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP6,
                       CURL_EASY_STR(तथ्य, STRING_DNS_LOCAL_IP6));
  }
#पूर्ण_अगर
  अगर(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYHOST,
                       तथ्य->set.doh_verifyhost ? 2L : 0L);
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYPEER,
                       तथ्य->set.doh_verifypeer ? 1L : 0L);
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYSTATUS,
                       तथ्य->set.doh_verifystatus ? 1L : 0L);

    /* Inherit *some* SSL options from the user's transfer. This is a
       best-guess as to which options are needed for compatibility. #3661

       Note DoH does not inherit the user's proxy server so proxy SSL settings
       have no effect and are not inherited. If that changes then two new
       options should be added to check doh proxy insecure separately,
       CURLOPT_DOH_PROXY_SSL_VERIFYHOST and CURLOPT_DOH_PROXY_SSL_VERIFYPEER.
       */
    doh->set.ssl.custom_cafile = तथ्य->set.ssl.custom_cafile;
    doh->set.ssl.custom_capath = तथ्य->set.ssl.custom_capath;
    doh->set.ssl.custom_cablob = तथ्य->set.ssl.custom_cablob;
    अगर(CURL_EASY_STR(तथ्य, STRING_SSL_CAFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO,
                         CURL_EASY_STR(तथ्य, STRING_SSL_CAFILE));
    }
    अगर(तथ्य->set.blobs[BLOB_CAINFO]) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO_BLOB, तथ्य->set.blobs[BLOB_CAINFO]);
    }
    अगर(CURL_EASY_STR(तथ्य, STRING_SSL_CAPATH)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAPATH,
                         CURL_EASY_STR(तथ्य, STRING_SSL_CAPATH));
    }
    अगर(CURL_EASY_STR(तथ्य, STRING_SSL_CRLFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CRLFILE,
                         CURL_EASY_STR(तथ्य, STRING_SSL_CRLFILE));
    }
    अगर(तथ्य->set.ssl.certinfo)
      ERROR_CHECK_SETOPT(CURLOPT_CERTINFO, 1L);
    अगर(तथ्य->set.ssl_fsslctx)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_FUNCTION, तथ्य->set.ssl_fsslctx);
    अगर(तथ्य->set.ssl_fsslctxp)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_तथ्य, तथ्य->set.ssl_fsslctxp);
    अगर(CURL_EASY_STR(तथ्य, STRING_SSL_EC_CURVES)) {
      ERROR_CHECK_SETOPT(CURLOPT_SSL_EC_CURVES,
                         CURL_EASY_STR(तथ्य, STRING_SSL_EC_CURVES));
    }
    (व्योम)curl_easy_setopt(doh, CURLOPT_SSL_OPTIONS,
                           ((दीर्घ)तथ्य->set.ssl.ssl_options &
                            ~CURLSSLOPT_AUTO_CLIENT_CERT));
  }

  doh->state.internal = TRUE;
  doh->master_mid = तथ्य->mid; /* master transfer of this one */
  doh->sub_xfer_done = doh_probe_done;

  परिणाम = Curl_meta_set(doh, CURL_EZM_DOH_PROBE, doh_req, doh_probe_dtor);
  doh_req = शून्य; /* call took ownership */
  अगर(परिणाम)
    जाओ error;

  /* DoH handles must not inherit private_data. The handles may be passed to
     the user via callbacks and the user will be able to identify them as
     internal handles because private data is not set. The user can then set
     private_data via CURLOPT_PRIVATE if they so choose. */
  DEBUGASSERT(!doh->set.private_तथ्य);

  अगर(Curl_multi_add_handle(multi, doh))
    जाओ error;

  *pmid = doh->mid;
  वापस CURLE_OK;

error:
  Curl_close(&doh);
  अगर(doh_req)
    doh_probe_dtor(शून्य, 0, doh_req);
  वापस परिणाम;
}

/*
 * Curl_doh() starts a name resolve using DoH. It resolves a name and returns
 * a 'Curl_addrinfo *' with the address information.
 */

CURLcode Curl_doh(काष्ठा Curl_easy *तथ्य,
                  काष्ठा Curl_resolv_async *async)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा doh_probes *dohp = शून्य;
  माप_प्रकार i;

  DEBUGASSERT(!async->doh);
  DEBUGASSERT(async->peer->hostname[0]);
  अगर(async->doh) {
    DEBUGASSERT(0); /* should not happen */
    Curl_doh_cleanup(तथ्य, async);
  }

  अगर(!async->dns_queries)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
#अगर_घोषित USE_HTTPSRR
  अगर(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     (async->dns_queries & CURL_DNSQ_HTTPS)) {
    /* Can't mix those in the same async resolve */
    DEBUGASSERT(0);
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  }
#अन्यथा
  अगर(async->dns_queries & CURL_DNSQ_HTTPS) {
    DEBUGASSERT(0);
    वापस CURLE_NOT_BUILT_IN;
  }
#पूर्ण_अगर

  /* start clean, consider allocating this struct on demand */
  async->doh = dohp = curlx_calloc(1, माप(काष्ठा doh_probes));
  अगर(!dohp)
    वापस CURLE_OUT_OF_MEMORY;

  क्रम(i = 0; i < DOH_SLOT_गिनती; ++i) {
    dohp->probe_rc[i] = DOH_OK;
    dohp->probe_mid[i] = UINT32_MAX;
  }

#अगर_घोषित USE_IPV6
  /* AAAA results have preference in happy eyeballing, trigger first */
  अगर(async->dns_queries & CURL_DNSQ_AAAA) {
    /* create IPv6 DoH request */
    परिणाम = doh_probe_run(तथ्य, CURL_DNS_TYPE_AAAA,
                           async->peer->hostname,
                           CURL_EASY_STR(तथ्य, STRING_DOH),
                           तथ्य->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV6]);
    अगर(परिणाम)
      जाओ error;
    async->queries_ongoing++;
  }
#पूर्ण_अगर

  /* create IPv4 DoH request */
  अगर(async->dns_queries & CURL_DNSQ_A) {
    परिणाम = doh_probe_run(तथ्य, CURL_DNS_TYPE_A,
                           async->peer->hostname,
                           CURL_EASY_STR(तथ्य, STRING_DOH),
                           तथ्य->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV4]);
    अगर(परिणाम)
      जाओ error;
    async->queries_ongoing++;
  }

#अगर_घोषित USE_HTTPSRR
  अगर(async->dns_queries & CURL_DNSQ_HTTPS) {
    अक्षर *qname = शून्य;
    अगर(async->peer->port != PORT_HTTPS) {
      qname = curl_maprintf("_%u._https.%s",
                            async->peer->port, async->peer->hostname);
      अगर(!qname)
        जाओ error;
    }
    परिणाम = doh_probe_run(तथ्य, CURL_DNS_TYPE_HTTPS,
                           qname ? qname : async->peer->hostname,
                           CURL_EASY_STR(तथ्य, STRING_DOH), तथ्य->multi,
                           async->id,
                           &dohp->probe_mid[DOH_SLOT_HTTPS_RR]);
    curlx_free(qname);
    अगर(परिणाम)
      जाओ error;
    async->queries_ongoing++;
  }
#पूर्ण_अगर
  वापस CURLE_OK;

error:
  Curl_doh_cleanup(तथ्य, async);
  वापस परिणाम;
}

अटल DOHcode doh_skipqname(स्थिर अचिन्हित अक्षर *doh, माप_प्रकार dohlen,
                             अचिन्हित पूर्णांक *indexp)
{
  अचिन्हित अक्षर लंबाई;
  करो {
    अगर(dohlen < (*indexp + 1))
      वापस DOH_DNS_OUT_OF_RANGE;
    लंबाई = doh[*indexp];
    अगर((लंबाई & 0xc0) == 0xc0) {
      /* name pointer, advance over it and be done */
      अगर(dohlen < (*indexp + 2))
        वापस DOH_DNS_OUT_OF_RANGE;
      *indexp += 2;
      अवरोध;
    }
    अगर(लंबाई & 0xc0)
      वापस DOH_DNS_BAD_LABEL;
    अगर(dohlen < (*indexp + 1 + लंबाई))
      वापस DOH_DNS_OUT_OF_RANGE;
    *indexp += (अचिन्हित पूर्णांक)(1 + लंबाई);
  } जबतक(लंबाई);
  वापस DOH_OK;
}

अटल अचिन्हित लघु doh_get16bit(स्थिर अचिन्हित अक्षर *doh,
                                   अचिन्हित पूर्णांक सूचकांक)
{
  वापस (अचिन्हित लघु)((doh[सूचकांक] << 8) | doh[सूचकांक + 1]);
}

अटल अचिन्हित पूर्णांक doh_get32bit(स्थिर अचिन्हित अक्षर *doh, अचिन्हित पूर्णांक सूचकांक)
{
  /* make clang and gcc optimize this to bswap by incrementing
     the pointer first. */
  doh += सूचकांक;

  /* avoid undefined behavior by casting to unsigned before shifting
     24 bits, possibly into the sign bit. codegen is same, but
     ub sanitizer will not be upset */
  वापस ((अचिन्हित)doh[0] << 24) | ((अचिन्हित)doh[1] << 16) |
         ((अचिन्हित)doh[2] << 8) | doh[3];
}

अटल व्योम doh_store_a(स्थिर अचिन्हित अक्षर *doh, पूर्णांक सूचकांक,
                        काष्ठा dohentry *d)
{
  /* silently ignore addresses over the limit */
  अगर(d->numaddr < DOH_MAX_ADDR) {
    काष्ठा dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_A;
    स_नकल(&a->ip.v4, &doh[सूचकांक], 4);
    d->numaddr++;
  }
}

अटल व्योम doh_store_aaaa(स्थिर अचिन्हित अक्षर *doh, पूर्णांक सूचकांक,
                           काष्ठा dohentry *d)
{
  /* silently ignore addresses over the limit */
  अगर(d->numaddr < DOH_MAX_ADDR) {
    काष्ठा dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_AAAA;
    स_नकल(&a->ip.v6, &doh[सूचकांक], 16);
    d->numaddr++;
  }
}

#अगर_घोषित USE_HTTPSRR
अटल DOHcode doh_store_https(स्थिर अचिन्हित अक्षर *doh, पूर्णांक सूचकांक,
                               काष्ठा dohentry *d, uint16_t len)
{
  /* silently ignore RRs over the limit */
  अगर(d->numhttps_rrs < DOH_MAX_HTTPS) {
    काष्ठा dohhttps_rr *h = &d->https_rrs[d->numhttps_rrs];
    h->val = curlx_memdup(&doh[सूचकांक], len);
    अगर(!h->val)
      वापस DOH_OUT_OF_MEM;
    h->len = len;
    d->numhttps_rrs++;
  }
  वापस DOH_OK;
}
#पूर्ण_अगर

अटल DOHcode doh_rdata(स्थिर अचिन्हित अक्षर *doh,
                         अचिन्हित लघु rdlength,
                         अचिन्हित लघु type,
                         पूर्णांक सूचकांक,
                         काष्ठा dohentry *d)
{
  /* RDATA
     - A (TYPE 1): 4 bytes
     - AAAA (TYPE 28): 16 bytes
     - HTTPS (TYPE 65): N bytes */
  चयन(type) {
  हाल CURL_DNS_TYPE_A:
    अगर(rdlength != 4)
      वापस DOH_DNS_RDATA_LEN;
    doh_store_a(doh, सूचकांक, d);
    अवरोध;
  हाल CURL_DNS_TYPE_AAAA:
    अगर(rdlength != 16)
      वापस DOH_DNS_RDATA_LEN;
    doh_store_aaaa(doh, सूचकांक, d);
    अवरोध;
#अगर_घोषित USE_HTTPSRR
  हाल CURL_DNS_TYPE_HTTPS:
    अगर(rdlength < 3)
      वापस DOH_DNS_RDATA_LEN;
    वापस doh_store_https(doh, सूचकांक, d, rdlength);
#पूर्ण_अगर
  शेष:
    /* unsupported type, or type we do not store, skip it */
    अवरोध;
  }
  वापस DOH_OK;
}

/* @unittest 1655 */
UNITTEST व्योम de_init(काष्ठा dohentry *de);
UNITTEST व्योम de_init(काष्ठा dohentry *de)
{
  स_रखो(de, 0, माप(*de));
  de->ttl = पूर्णांक_उच्च;
}

/* TTL value cap */
#घोषणा MAX_DNS_TTL 86400U /* 24 hours */
/* @unittest 1650 */
UNITTEST DOHcode doh_resp_decode(स्थिर अचिन्हित अक्षर *doh,
                                 माप_प्रकार dohlen,
                                 DNStype dnstype,
                                 काष्ठा dohentry *d);
UNITTEST DOHcode doh_resp_decode(स्थिर अचिन्हित अक्षर *doh,
                                 माप_प्रकार dohlen,
                                 DNStype dnstype,
                                 काष्ठा dohentry *d)
{
  अचिन्हित अक्षर rcode;
  अचिन्हित लघु qdcount;
  अचिन्हित लघु ancount;
  अचिन्हित लघु type = 0;
  अचिन्हित लघु rdlength;
  अचिन्हित लघु nscount;
  अचिन्हित लघु arcount;
  अचिन्हित पूर्णांक सूचकांक = 12;
  DOHcode rc;

  अगर(dohlen < 12)
    वापस DOH_TOO_SMALL_बफर; /* too small */
  अगर(!doh || doh[0] || doh[1])
    वापस DOH_DNS_BAD_ID; /* bad ID */
  rcode = doh[3] & 0x0f;
  अगर(rcode == 3)
    वापस DOH_DNS_NXDOMAIN; /* name does not exist */
  अगर(rcode)
    वापस DOH_DNS_BAD_RCODE; /* bad rcode */

  qdcount = doh_get16bit(doh, 4);
  जबतक(qdcount) {
    rc = doh_skipqname(doh, dohlen, &सूचकांक);
    अगर(rc)
      वापस rc; /* bad qname */
    अगर(dohlen < (सूचकांक + 4))
      वापस DOH_DNS_OUT_OF_RANGE;
    सूचकांक += 4; /* skip question's type and class */
    qdcount--;
  }

  ancount = doh_get16bit(doh, 6);
  जबतक(ancount) {
    अचिन्हित लघु dnsclass;
    अचिन्हित पूर्णांक ttl;

    rc = doh_skipqname(doh, dohlen, &सूचकांक);
    अगर(rc)
      वापस rc; /* bad qname */

    अगर(dohlen < (सूचकांक + 2))
      वापस DOH_DNS_OUT_OF_RANGE;

    type = doh_get16bit(doh, सूचकांक);
    अगर((type != CURL_DNS_TYPE_CNAME) &&  /* may be synthesized from DNAME */
       (type != CURL_DNS_TYPE_DNAME) &&  /* if present, accept and ignore */
       (type != dnstype))
      /* Not the same type as was asked for, nor CNAME nor DNAME */
      वापस DOH_DNS_UNEXPECTED_TYPE;
    सूचकांक += 2;

    अगर(dohlen < (सूचकांक + 2))
      वापस DOH_DNS_OUT_OF_RANGE;
    dnsclass = doh_get16bit(doh, सूचकांक);
    अगर(DNS_CLASS_IN != dnsclass)
      वापस DOH_DNS_UNEXPECTED_CLASS; /* unsupported */
    सूचकांक += 2;

    अगर(dohlen < (सूचकांक + 4))
      वापस DOH_DNS_OUT_OF_RANGE;

    ttl = doh_get32bit(doh, सूचकांक);
    अगर(ttl > MAX_DNS_TTL)
      ttl = MAX_DNS_TTL;
    अगर(ttl < d->ttl)
      d->ttl = ttl;
    सूचकांक += 4;

    अगर(dohlen < (सूचकांक + 2))
      वापस DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, सूचकांक);
    सूचकांक += 2;
    अगर(dohlen < (सूचकांक + rdlength))
      वापस DOH_DNS_OUT_OF_RANGE;

    rc = doh_rdata(doh, rdlength, type, (पूर्णांक)सूचकांक, d);
    अगर(rc)
      वापस rc;
    सूचकांक += rdlength;
    ancount--;
  }

  nscount = doh_get16bit(doh, 8);
  जबतक(nscount) {
    rc = doh_skipqname(doh, dohlen, &सूचकांक);
    अगर(rc)
      वापस rc; /* bad qname */

    अगर(dohlen < (सूचकांक + 8))
      वापस DOH_DNS_OUT_OF_RANGE;

    सूचकांक += 2 + 2 + 4; /* type, dnsclass and ttl */

    अगर(dohlen < (सूचकांक + 2))
      वापस DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, सूचकांक);
    सूचकांक += 2;
    अगर(dohlen < (सूचकांक + rdlength))
      वापस DOH_DNS_OUT_OF_RANGE;
    सूचकांक += rdlength;
    nscount--;
  }

  arcount = doh_get16bit(doh, 10);
  जबतक(arcount) {
    rc = doh_skipqname(doh, dohlen, &सूचकांक);
    अगर(rc)
      वापस rc; /* bad qname */

    अगर(dohlen < (सूचकांक + 8))
      वापस DOH_DNS_OUT_OF_RANGE;

    सूचकांक += 2 + 2 + 4; /* type, dnsclass and ttl */

    अगर(dohlen < (सूचकांक + 2))
      वापस DOH_DNS_OUT_OF_RANGE;

    rdlength = doh_get16bit(doh, सूचकांक);
    सूचकांक += 2;
    अगर(dohlen < (सूचकांक + rdlength))
      वापस DOH_DNS_OUT_OF_RANGE;
    सूचकांक += rdlength;
    arcount--;
  }

  अगर(सूचकांक != dohlen)
    वापस DOH_DNS_MALFORMAT; /* something is wrong */

  वापस DOH_OK; /* ok */
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
अटल CURLcode doh2ai(स्थिर काष्ठा dohentry *de, स्थिर अक्षर *hostname,
                       पूर्णांक port, काष्ठा Curl_addrinfo **aip)
{
  काष्ठा Curl_addrinfo *ai;
  काष्ठा Curl_addrinfo *prevai = शून्य;
  काष्ठा Curl_addrinfo *firstai = शून्य;
  काष्ठा sockaddr_in *addr;
#अगर_घोषित USE_IPV6
  काष्ठा sockaddr_in6 *addr6;
#पूर्ण_अगर
  माप_प्रकार hostlen = म_माप(hostname) + 1; /* include null-terminator */
  CURLcode परिणाम = CURLE_OK;
  पूर्णांक i;

  क्रम(i = 0; i < de->numaddr; i++) {
    माप_प्रकार ss_आकार;
    CURL_SA_FAMILY_T addrtype;
    अगर(de->addr[i].type == CURL_DNS_TYPE_AAAA) {
#अगर_अघोषित USE_IPV6
      /* we cannot handle IPv6 addresses */
      जारी;
#अन्यथा
      ss_आकार = माप(काष्ठा sockaddr_in6);
      addrtype = AF_INET6;
#पूर्ण_अगर
    }
    अन्यथा {
      ss_आकार = माप(काष्ठा sockaddr_in);
      addrtype = AF_INET;
    }

    ai = curlx_calloc(1, माप(काष्ठा Curl_addrinfo) + ss_आकार + hostlen);
    अगर(!ai) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      अवरोध;
    }
    ai->ai_addr = (व्योम *)((अक्षर *)ai + माप(काष्ठा Curl_addrinfo));
    ai->ai_canonname = (व्योम *)((अक्षर *)ai->ai_addr + ss_आकार);
    स_नकल(ai->ai_canonname, hostname, hostlen);

    अगर(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    अगर(prevai)
      /* make the previous entry point to this */
      prevai->ai_अग्र = ai;

    ai->ai_family = addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_आकार;

    /* leave the rest of the struct filled with zero */

    चयन(ai->ai_family) {
    हाल AF_INET:
      addr = (व्योम *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(माप(काष्ठा in_addr) == माप(de->addr[i].ip.v4));
      स_नकल(&addr->sin_addr, &de->addr[i].ip.v4, माप(काष्ठा in_addr));
      addr->sin_family = addrtype;
      addr->sin_port = htons((अचिन्हित लघु)port);
      अवरोध;

#अगर_घोषित USE_IPV6
    हाल AF_INET6:
      addr6 = (व्योम *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(माप(काष्ठा in6_addr) == माप(de->addr[i].ip.v6));
      स_नकल(&addr6->sin6_addr, &de->addr[i].ip.v6, माप(काष्ठा in6_addr));
      addr6->sin6_family = addrtype;
      addr6->sin6_port = htons((अचिन्हित लघु)port);
      अवरोध;
#पूर्ण_अगर
    }

    prevai = ai;
  }

  अगर(परिणाम) {
    Curl_freeaddrinfo(firstai);
    firstai = शून्य;
  }
  *aip = firstai;

  वापस परिणाम;
}

/* @unittest 1655 */
UNITTEST व्योम de_cleanup(काष्ठा dohentry *d);
UNITTEST व्योम de_cleanup(काष्ठा dohentry *d)
{
#अगर_घोषित USE_HTTPSRR
  पूर्णांक i = 0;
  क्रम(i = 0; i < d->numhttps_rrs; i++)
    curlx_safefree(d->https_rrs[i].val);
#अन्यथा
  (व्योम)d;
#पूर्ण_अगर
}

#अगर_घोषित USE_HTTPSRR

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
अटल CURLcode doh_decode_rdata_name(स्थिर अचिन्हित अक्षर **buf,
                                      माप_प्रकार *remaining, अक्षर **dnsname)
{
  स्थिर अचिन्हित अक्षर *cp = शून्य;
  माप_प्रकार rem = 0;
  अचिन्हित अक्षर clen = 0; /* chunk len */
  काष्ठा dynbuf thename;

  DEBUGASSERT(buf && remaining && dnsname);
  अगर(!buf || !remaining || !dnsname || !*remaining)
    वापस CURLE_OUT_OF_MEMORY;
  curlx_dyn_init(&thename, CURL_MAXLEN_HOST_NAME);
  rem = *remaining;
  cp = *buf;
  clen = *cp++;
  /* RFC 9460 says it must be uncompressed */
  अगर(clen > 63)
    वापस CURLE_WEIRD_SERVER_REPLY;

  अगर(clen == 0) {
    /* special case - return "." as name */
    अगर(curlx_dyn_addn(&thename, ".", 1))
      वापस CURLE_OUT_OF_MEMORY;
  }
  जबतक(clen) {
    अगर(clen >= rem) {
      curlx_dyn_free(&thename);
      वापस CURLE_OUT_OF_MEMORY;
    }
    अगर(curlx_dyn_addn(&thename, cp, clen) ||
       curlx_dyn_addn(&thename, ".", 1))
      वापस CURLE_TOO_LARGE;

    cp += clen;
    rem -= (clen + 1);
    अगर(rem <= 0) {
      curlx_dyn_free(&thename);
      वापस CURLE_OUT_OF_MEMORY;
    }
    clen = *cp++;
    अगर(clen > 63) {
      /* invalid format */
      curlx_dyn_free(&thename);
      वापस CURLE_WEIRD_SERVER_REPLY;
    }
  }
  *buf = cp;
  *remaining = rem - 1;
  *dnsname = curlx_dyn_ptr(&thename);
  वापस CURLE_OK;
}

/* scan for byte values <= 32 or 127 */
अटल CURLcode junkscan(स्थिर अक्षर *url)
{
  स्थिर अचिन्हित अक्षर *p = (स्थिर अचिन्हित अक्षर *)url;
  जबतक(*p) {
    अगर(*p <= 0x20 || *p == 127)
      वापस CURLE_WEIRD_SERVER_REPLY;
    p++;
  }
  वापस CURLE_OK;
}

/* @unittest 1658 */
UNITTEST CURLcode doh_resp_decode_httpsrr(काष्ठा Curl_easy *तथ्य,
                                          स्थिर अचिन्हित अक्षर *cp, माप_प्रकार len,
                                          काष्ठा Curl_https_rrinfo **hrr);
UNITTEST CURLcode doh_resp_decode_httpsrr(काष्ठा Curl_easy *तथ्य,
                                          स्थिर अचिन्हित अक्षर *cp, माप_प्रकार len,
                                          काष्ठा Curl_https_rrinfo **hrr)
{
  uint16_t pcode = 0, plen = 0;
  uint32_t expected_min_pcode = 0;
  काष्ठा Curl_https_rrinfo *lhrr = शून्य;
  अक्षर *dnsname = शून्य;
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;

  (व्योम)तथ्य;
  *hrr = शून्य;
  अगर(len <= 2)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  lhrr = curlx_calloc(1, माप(काष्ठा Curl_https_rrinfo));
  अगर(!lhrr)
    वापस CURLE_OUT_OF_MEMORY;
  lhrr->priority = doh_get16bit(cp, 0);
  cp += 2;
  len -= 2;
  अगर(doh_decode_rdata_name(&cp, &len, &dnsname) != CURLE_OK)
    जाओ err;
  lhrr->target = dnsname;
  परिणाम = junkscan(dnsname);
  अगर(परिणाम) {
    /* unacceptable hostname content */
    जाओ err;
  }
  जबतक(len >= 4) {
    pcode = doh_get16bit(cp, 0);
    plen = doh_get16bit(cp, 2);
    cp += 4;
    len -= 4;
    अगर(pcode < expected_min_pcode || plen > len) {
      परिणाम = CURLE_WEIRD_SERVER_REPLY;
      जाओ err;
    }
    परिणाम = Curl_httpsrr_set(lhrr, pcode, cp, plen);
    अगर(परिणाम)
      जाओ err;
    Curl_httpsrr_trace(तथ्य, lhrr);
    cp += plen;
    len -= plen;
    expected_min_pcode = pcode + 1;
  }
  *hrr = lhrr;
  वापस CURLE_OK;
err:
  Curl_httpsrr_destroy(lhrr);
  वापस परिणाम;
}

#पूर्ण_अगर /* USE_HTTPSRR */

/* called from multi when a sub transfer, e.g. doh probe, is done.
 * Parse the response and set the results in the `async` context
 * of master, using the id from the probe's CURL_EZM_DOH_PROBE
 * meta data. */
अटल व्योम doh_probe_done(काष्ठा Curl_easy *doh,
                           काष्ठा Curl_easy *master, CURLcode परिणाम)
{
  काष्ठा Curl_resolv_async *async = शून्य;
  काष्ठा doh_probes *dohp = शून्य;
  काष्ठा doh_request *doh_req = शून्य;
  काष्ठा Curl_addrinfo **pdest_ai;
  काष्ठा dohentry de;
  पूर्णांक slot, httpcode;

  de_init(&de);
  doh_req = Curl_meta_get(doh, CURL_EZM_DOH_PROBE);
  अगर(!doh_req) {
    /* transfer `doh` is not a DoH probe. */
    DEBUGASSERT(0);
    जाओ out;
  }

  async = Curl_async_get(master, doh_req->resolv_id);
  अगर(!async) {
    CURL_TRC_DNS(master, "[%u] ignoring outdated DoH response",
                 doh_req->resolv_id);
    जाओ out;
  }
  dohp = async->doh;

  क्रम(slot = 0; slot < DOH_SLOT_गिनती; ++slot) {
    अगर(dohp->probe_mid[slot] == doh->mid)
      अवरोध;
  }
  /* We really should have found the slot where to store the response */
  अगर(slot >= DOH_SLOT_गिनती) {
    failf(master, "DoH: unknown sub request done");
    DEBUGASSERT(0);
    जाओ out;
  }

  async->queries_ongoing--;
  dohp = async->doh;
  httpcode = doh->info.httpcode;
  चयन(slot) {
  हाल DOH_SLOT_IPV4:
    async->dns_responses |= CURL_DNSQ_A;
    अवरोध;
#अगर_घोषित USE_IPV6
  हाल DOH_SLOT_IPV6:
    async->dns_responses |= CURL_DNSQ_AAAA;
    अवरोध;
#पूर्ण_अगर
#अगर_घोषित USE_HTTPSRR
  हाल DOH_SLOT_HTTPS_RR:
    async->dns_responses |= CURL_DNSQ_HTTPS;
    अवरोध;
#पूर्ण_अगर
  शेष:
    DEBUGASSERT(0);
    अवरोध;
  }

  अगर(परिणाम) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: %s",
          doh_type2name(doh_req->dnstype), curl_easy_strerror(परिणाम));
    जाओ out;
  }
  अन्यथा अगर((httpcode < 200) || (httpcode >= 300)) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: HTTP status %d",
          doh_type2name(doh_req->dnstype), httpcode);
    जाओ out;
  }

  dohp->probe_rc[slot] = doh_resp_decode(curlx_dyn_uptr(&doh_req->resp_body),
                                         curlx_dyn_len(&doh_req->resp_body),
                                         doh_req->dnstype, &de);
  अगर(dohp->probe_rc[slot]) {
#अगर_घोषित USE_HTTPSRR
    अगर((dohp->probe_rc[slot] == DOH_NO_CONTENT) &&
       (doh_req->dnstype == CURL_DNS_TYPE_HTTPS)) {
      dohp->probe_rc[slot] = DOH_DNS_NXDOMAIN;
    }
#पूर्ण_अगर
    infof(doh, "[DoH] [%s] error decoding response: %s",
          doh_type2name(doh_req->dnstype),
          doh_strerror(dohp->probe_rc[slot]));
    जाओ out;
  }

  अगर(doh_req->dnstype == CURL_DNS_TYPE_A)
    pdest_ai = &async->ai_A;
  अन्यथा अगर(doh_req->dnstype == CURL_DNS_TYPE_AAAA)
    pdest_ai = &async->ai_AAAA;
  अन्यथा
    pdest_ai = शून्य;

  अगर(pdest_ai && de.numaddr) {
    अगर(*pdest_ai) {
      Curl_freeaddrinfo(*pdest_ai);
      *pdest_ai = शून्य;
    }
    परिणाम = doh2ai(&de, async->peer->hostname, async->peer->port, pdest_ai);
    अगर(परिणाम) { /* hard failure on our side, fail completely */
      infof(doh, "[DoH] [%s] error creating addrinfo: %s",
            doh_type2name(doh_req->dnstype), curl_easy_strerror(परिणाम));
      dohp->probe_rc[slot] = DOH_OOM;
      async->परिणाम = परिणाम;
    }
  }
#अगर_घोषित USE_HTTPSRR
  अन्यथा अगर((doh_req->dnstype == CURL_DNS_TYPE_HTTPS) && de.numhttps_rrs) {
    CURL_TRC_DNS(doh, "[HTTPS] got %d records", de.numhttps_rrs);
    परिणाम = doh_resp_decode_httpsrr(doh, de.https_rrs->val,
                                     de.https_rrs->len, &async->httpsrr);
    अगर(परिणाम) {
      dohp->probe_rc[slot] = DOH_HTTP_FAILED;
      infof(doh, "[DoH] error decoding HTTPS RR: %s",
            curl_easy_strerror(परिणाम));
      जाओ out;
    }
  }
#पूर्ण_अगर /* USE_HTTPSRR */

  /* DoH request complete, run master to act on results */
  infof(doh, "DoH request complete, %u to go", async->queries_ongoing);

out:
  Curl_multi_mark_dirty(master);
  de_cleanup(&de);
  Curl_meta_remove(doh, CURL_EZM_DOH_PROBE);
}

CURLcode Curl_doh_take_परिणाम(काष्ठा Curl_easy *तथ्य,
                              काष्ठा Curl_resolv_async *async,
                              काष्ठा Curl_dns_entry **pdns)
{
  काष्ठा doh_probes *dohp = async->doh;
  CURLcode परिणाम = CURLE_OK;

  *pdns = शून्य; /* defaults to no response */
  अगर(!dohp)
    वापस CURLE_OUT_OF_MEMORY;

  async->negative_answer = FALSE;
  अगर(async->परिणाम) {
    परिणाम = async->परिणाम;
    जाओ out;
  }

  अगर(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     dohp->probe_mid[DOH_SLOT_IPV4] == UINT32_MAX &&
     dohp->probe_mid[DOH_SLOT_IPV6] == UINT32_MAX) {
    failf(तथ्य, "Could not DoH-resolve: %s", async->peer->hostname);
    वापस async->for_proxy ?
      CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
  }
  अन्यथा अगर(!async->queries_ongoing) {
    काष्ठा Curl_dns_entry *dns = शून्य;
    bool negative = TRUE;
    पूर्णांक slot;

    /* remove DoH handles from multi handle and close them */
    doh_close(तथ्य, async);
    /* parse the responses, create the struct and return it! */
    क्रम(slot = 0; slot < DOH_SLOT_गिनती; slot++) {
      /* Failing without an NXDOMAIN answer - a SERVFAIL-class rcode or
         an undecodable response - says nothing about the name. Such a
         failure must not be cached as a negative entry. */
      अगर(dohp->probe_rc[slot] && (dohp->probe_rc[slot] != DOH_DNS_NXDOMAIN))
        negative = FALSE;
    } /* next slot */

    अगर(async->ai_A || async->ai_AAAA) {
      dns = Curl_dnsc_mk_addr2(
        तथ्य, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
      अगर(!dns) {
        परिणाम = CURLE_OUT_OF_MEMORY;
        जाओ out;
      }
    }
#अगर_घोषित USE_HTTPSRR
    अन्यथा अगर((async->dns_queries & CURL_DNSQ_HTTPS) &&
            !dohp->probe_rc[DOH_SLOT_HTTPS_RR]) {
      Curl_httpsrr_trace(तथ्य, async->httpsrr);
      dns = Curl_dnsc_mk_https(तथ्य, &async->httpsrr, async->peer);
      अगर(!dns) {
        परिणाम = CURLE_OUT_OF_MEMORY;
        जाओ out;
      }
    }
#पूर्ण_अगर /* USE_HTTPSRR */
    अन्यथा {
      /* every query failed. Only NXDOMAIN answers for all of them
         make this a negative answer, eligible for caching. */
      async->negative_answer = negative;
      परिणाम = async->for_proxy ?
        CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
    }

    /* and add the entry to the cache */
    अगर(dns)
      परिणाम = Curl_dnscache_add(तथ्य, dns);
    *pdns = dns;
  } /* !async->queries_ongoing */
  अन्यथा
    /* wait for pending DoH transactions to complete */
    वापस CURLE_AGAIN;

out:
  Curl_doh_cleanup(तथ्य, async);
  वापस परिणाम;
}

अटल व्योम doh_close(काष्ठा Curl_easy *तथ्य,
                      काष्ठा Curl_resolv_async *async)
{
  काष्ठा doh_probes *doh = async ? async->doh : शून्य;
  अगर(doh && तथ्य->multi) {
    काष्ठा Curl_easy *probe_तथ्य;
    uint32_t mid;
    माप_प्रकार slot;
    क्रम(slot = 0; slot < DOH_SLOT_गिनती; slot++) {
      mid = doh->probe_mid[slot];
      अगर(mid == UINT32_MAX)
        जारी;
      doh->probe_mid[slot] = UINT32_MAX;
      /* should have been called before data is removed from multi handle */
      DEBUGASSERT(तथ्य->multi);
      probe_तथ्य = तथ्य->multi ? Curl_multi_get_easy(तथ्य->multi, mid) : शून्य;
      अगर(!probe_तथ्य) {
        DEBUGF(infof(तथ्य, "Curl_doh_close: xfer for mid=%u not found!", mid));
        जारी;
      }
      probe_तथ्य->sub_xfer_done = शून्य; /* No longer interested in result */
      /* data->multi might already be reset at this time */
      Curl_multi_remove_handle(तथ्य->multi, probe_तथ्य);
      Curl_close(&probe_तथ्य);
    }
    CURL_TRC_DNS(तथ्य, "[DoH] probe done");
  }
}

व्योम Curl_doh_cleanup(काष्ठा Curl_easy *तथ्य,
                      काष्ठा Curl_resolv_async *async)
{
  काष्ठा doh_probes *dohp = async->doh;
  अगर(dohp) {
    doh_close(तथ्य, async);
    curlx_safefree(async->doh);
  }
}

#पूर्ण_अगर /* CURL_DISABLE_DOH */
