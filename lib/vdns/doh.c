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

#यदि_अपरिभाषितम् CURL_DISABLE_DOH

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

#परिभाषय DNS_CLASS_अन्तर् 0x01

स्थिर शून्य doh_close(संरचना Curl_easy *दत्त,
                      संरचना Curl_resolv_async *async);

#यदि_परिभाषितम् CURLVERBOSE
स्थिर ध्रुव वर्ण * ध्रुव doh_संहिता_str[] = {
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

स्थिर ध्रुव वर्ण *doh_strerror(DOHcode संहिता)
{
  यदि((size_t)संहिता < CURL_ARRAYSIZE(doh_संहिता_str))
    प्रत्यागम doh_संहिता_str[संहिता];
  प्रत्यागम "bad error code";
}

स्थिर ध्रुव वर्ण *doh_type2name(DNStype dnstype)
{
  विकल्प(dnstype) {
  पक्ष CURL_DNS_TYPE_A:
    प्रत्यागम "A";
  पक्ष CURL_DNS_TYPE_AAAA:
    प्रत्यागम "AAAA";
#यदि_परिभाषितम् USE_HTTPSRR
  पक्ष CURL_DNS_TYPE_HTTPS:
    प्रत्यागम "HTTPS";
#यदि_अन्तः
  स्वभाव:
    प्रत्यागम "unknown";
  }
}

#यदि_अन्तः /* CURLVERBOSE */

/* @unittest 1655
 */
UNITTEST DOHcode doh_req_encode(ध्रुव वर्ण *host,
                                DNStype dnstype,
                                अचिह्नित वर्ण *dnsp,  /*बफर*/
                                size_t len,  /* buffer size */
                                size_t *olen);  /* output length */
UNITTEST DOHcode doh_req_encode(ध्रुव वर्ण *host,
                                DNStype dnstype,
                                अचिह्नित वर्ण *dnsp, /*बफर*/
                                size_t len,   /* buffer size */
                                size_t *olen) /* output length */
{
  ध्रुव size_t hostlen = वाक्यदैर्घ्यम्(host);
  अचिह्नित वर्ण *orig = dnsp;
  ध्रुव वर्ण *hostp = host;

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
  यदि(host[hostlen - 1] != '.')
    expected_len++;

  यदि(expected_len > DOH_MAX_DNSREQ_आकार)
    प्रत्यागम DOH_DNS_NAME_TOO_LONG;

  यदि(len < expected_len)
    प्रत्यागम DOH_TOO_SMALL_बफर;

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
  यावत्(*hostp) {
    size_t labellen;
    ध्रुव वर्ण *dot = strchr(hostp, '.');
    यदि(dot)
      labellen = dot - hostp;
    अन्यथा
      labellen = वाक्यदैर्घ्यम्(hostp);
    यदि((labellen > 63) || (!labellen)) {
      /* label is too long or too short, error out */
      *olen = 0;
      प्रत्यागम DOH_DNS_BAD_LABEL;
    }
    /* label is non-empty, process it */
    *dnsp++ = (अचिह्नित वर्ण)labellen;
    स्मृतिं_प्रतिलिख(dnsp, hostp, labellen);
    dnsp += labellen;
    hostp += labellen;
    /* advance past dot, but only if there is one */
    यदि(dot)
      hostp++;
  } /* next label */

  *dnsp++ = 0; /* append zero-length label for root */

  /* There are assigned TYPE codes beyond 255: use range [1..65535] */
  *dnsp++ = (अचिह्नित वर्ण)(255 & (dnstype >> 8)); /* upper 8-bit TYPE */
  *dnsp++ = (अचिह्नित वर्ण)(255 & dnstype);        /* lower 8-bit TYPE */

  *dnsp++ = '\0'; /* upper 8-bit CLASS */
  *dnsp++ = DNS_CLASS_अन्तर्; /* IN - "the Internet" */

  *olen = dnsp - orig;

  /* verify that our estimation of length is valid, since
   * this has led to buffer overflows in this function */
  DEBUGASSERT(*olen == expected_len);
  प्रत्यागम DOH_OK;
}

स्थिर size_t doh_probe_write_cb(वर्ण *contents, size_t आकार, size_t nmemb,
                                 शून्य *userp)
{
  size_t realsize = आकार * nmemb;
  संरचना Curl_easy *दत्त = userp;
  संरचना doh_request *doh_req = Curl_meta_get(दत्त, CURL_EZM_DOH_PROBE);
  यदि(!doh_req)
    प्रत्यागम CURL_WRITEFUNC_ERROR;

  यदि(curlx_dyn_addn(&doh_req->resp_body, contents, realsize))
    प्रत्यागम 0;

  प्रत्यागम realsize;
}

स्थिर शून्य doh_probe_समाप्तम्(संरचना Curl_easy *doh,
                           संरचना Curl_easy *master, CURLcode परिणाम);
स्थिर शून्य doh_probe_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *e)
{
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  यदि(e) {
    संरचना doh_request *doh_req = e;
    curl_slist_free_all(doh_req->req_hds);
    curlx_dyn_free(&doh_req->resp_body);
    curlx_free(e);
  }
}

#परिभाषय ERROR_CHECK_SETOPT(x, y)                        \
  कुरु {                                                  \
    परिणाम = curl_easy_setopt((CURL *)doh, x, y);       \
    यदि(परिणाम &&                                        \
       परिणाम != CURLE_NOT_BUILT_अन्तर् &&                  \
       परिणाम != CURLE_UNKNOWN_OPTION)                  \
      गच्छ error;                                       \
  } यावत्(0)

स्थिर CURLcode doh_probe_सततम्(संरचना Curl_easy *दत्त,
                              DNStype dnstype,
                              ध्रुव वर्ण *host,
                              ध्रुव वर्ण *url, CURLM *multi,
                              uint32_t resolv_id,
                              uint32_t *pmid)
{
  संरचना Curl_easy *doh = रिक्त;
  CURLcode परिणाम = CURLE_OK;
  timediff_t timeout_ms;
  संरचना doh_request *doh_req;
  DOHcode d;
  bool maybe_https = !curl_strnequal(url, STRCONST("http:"));

  *pmid = UINT32_MAX;

  doh_req = curlx_calloc(1, परिमाण(*doh_req));
  यदि(!doh_req)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  doh_req->resolv_id = resolv_id;
  doh_req->dnstype = dnstype;
  curlx_dyn_init(&doh_req->resp_body, DYN_DOH_RESPONSE);

  d = doh_req_encode(host, dnstype, doh_req->req_body,
                     परिमाण(doh_req->req_body),
                     &doh_req->req_body_len);
  यदि(d) {
    failf(दत्त, "Failed to encode DoH packet [%d]", (पूर्णाङ्क)d);
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ error;
  }

  timeout_ms = Curl_timeleft_ms(दत्त);
  यदि(timeout_ms < 0) {
    परिणाम = CURLE_OPERATION_TIMEDOUT;
    गच्छ error;
  }

  doh_req->req_hds =
    curl_slist_append(रिक्त, "Content-Type: application/dns-message");
  यदि(!doh_req->req_hds) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ error;
  }

  /* Curl_open() is the internal version of curl_easy_init() */
  परिणाम = Curl_open(&doh);
  यदि(परिणाम)
    गच्छ error;

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
#यदि_परिभाषितम् USE_HTTP2
  यदि(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_HTTP_संस्करणम्, CURL_HTTP_संस्करणम्_2_TLS);
    ERROR_CHECK_SETOPT(CURLOPT_PIPEWAIT, 1L);
  }
#यदि_अन्तः
#यदि_अपरिभाषितम् DEBUGBUILD
  /* enforce HTTPS if not debug */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
#अन्यथा
  /* in debug mode, also allow http */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
#यदि_अन्तः
  ERROR_CHECK_SETOPT(CURLOPT_TIMEOUT_MS, (दीर्घ)timeout_ms);
  ERROR_CHECK_SETOPT(CURLOPT_SHARE, (CURLSH *)दत्त->share);
  यदि(दत्त->समुच्चयः.err && दत्त->समुच्चयः.err != stderr)
    ERROR_CHECK_SETOPT(CURLOPT_STDERR, दत्त->समुच्चयः.err);
  यदि(Curl_trc_ft_is_verbose(दत्त, &Curl_trc_feat_doh))
    ERROR_CHECK_SETOPT(CURLOPT_VERBOSE, 1L);
  यदि(दत्त->समुच्चयः.no_signal)
    ERROR_CHECK_SETOPT(CURLOPT_NOSIGNAL, 1L);
  यदि(दत्त->समुच्चयः.fdebug)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGFUNCTION, दत्त->समुच्चयः.fdebug);
  यदि(दत्त->समुच्चयः.debugdata)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGDATA, दत्त->समुच्चयः.debugdata);

#यदि_परिभाषितम् USE_RESOLV_ARES
  यदि(CURL_EASY_STR(दत्त, STRING_DNS_SERVERS)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_SERVERS,
                       CURL_EASY_STR(दत्त, STRING_DNS_SERVERS));
  }
  यदि(CURL_EASY_STR(दत्त, STRING_DNS_INTERFACE)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_INTERFACE,
                       CURL_EASY_STR(दत्त, STRING_DNS_INTERFACE));
  }
  यदि(CURL_EASY_STR(दत्त, STRING_DNS_LOCAL_IP4)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP4,
                       CURL_EASY_STR(दत्त, STRING_DNS_LOCAL_IP4));
  }
  यदि(CURL_EASY_STR(दत्त, STRING_DNS_LOCAL_IP6)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP6,
                       CURL_EASY_STR(दत्त, STRING_DNS_LOCAL_IP6));
  }
#यदि_अन्तः
  यदि(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYHOST,
                       दत्त->समुच्चयः.doh_verifyhost ? 2L : 0L);
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYPEER,
                       दत्त->समुच्चयः.doh_verifypeer ? 1L : 0L);
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYSTATUS,
                       दत्त->समुच्चयः.doh_verifystatus ? 1L : 0L);

    /* Inherit *some* SSL options from the user's transfer. This is a
       best-guess as to which options are needed for compatibility. #3661

       Note DoH does not inherit the user's proxy server so proxy SSL settings
       have no effect and are not inherited. If that changes then two new
       options should be added to check doh proxy insecure separately,
       CURLOPT_DOH_PROXY_SSL_VERIFYHOST and CURLOPT_DOH_PROXY_SSL_VERIFYPEER.
       */
    doh->समुच्चयः.ssl.custom_cafile = दत्त->समुच्चयः.ssl.custom_cafile;
    doh->समुच्चयः.ssl.custom_capath = दत्त->समुच्चयः.ssl.custom_capath;
    doh->समुच्चयः.ssl.custom_cablob = दत्त->समुच्चयः.ssl.custom_cablob;
    यदि(CURL_EASY_STR(दत्त, STRING_SSL_CAFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO,
                         CURL_EASY_STR(दत्त, STRING_SSL_CAFILE));
    }
    यदि(दत्त->समुच्चयः.blobs[BLOB_CAINFO]) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO_BLOB, दत्त->समुच्चयः.blobs[BLOB_CAINFO]);
    }
    यदि(CURL_EASY_STR(दत्त, STRING_SSL_CAPATH)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAPATH,
                         CURL_EASY_STR(दत्त, STRING_SSL_CAPATH));
    }
    यदि(CURL_EASY_STR(दत्त, STRING_SSL_CRLFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CRLFILE,
                         CURL_EASY_STR(दत्त, STRING_SSL_CRLFILE));
    }
    यदि(दत्त->समुच्चयः.ssl.certinfo)
      ERROR_CHECK_SETOPT(CURLOPT_CERTINFO, 1L);
    यदि(दत्त->समुच्चयः.ssl_fsslctx)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_FUNCTION, दत्त->समुच्चयः.ssl_fsslctx);
    यदि(दत्त->समुच्चयः.ssl_fsslctxp)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_दत्त, दत्त->समुच्चयः.ssl_fsslctxp);
    यदि(CURL_EASY_STR(दत्त, STRING_SSL_EC_CURVES)) {
      ERROR_CHECK_SETOPT(CURLOPT_SSL_EC_CURVES,
                         CURL_EASY_STR(दत्त, STRING_SSL_EC_CURVES));
    }
    (शून्य)curl_easy_setopt(doh, CURLOPT_SSL_OPTIONS,
                           ((दीर्घ)दत्त->समुच्चयः.ssl.ssl_options &
                            ~CURLSSLOPT_AUTO_CLIENT_CERT));
  }

  doh->state.आन्तरिकम् = TRUE;
  doh->master_mid = दत्त->mid; /* master transfer of this one */
  doh->sub_xfer_समाप्तम् = doh_probe_समाप्तम्;

  परिणाम = Curl_meta_समुच्चयः(doh, CURL_EZM_DOH_PROBE, doh_req, doh_probe_dtor);
  doh_req = रिक्त; /* call took ownership */
  यदि(परिणाम)
    गच्छ error;

  /* DoH handles must not inherit private_data. The handles may be passed to
     the user via callbacks and the user will be able to identify them as
     internal handles because private data is not set. The user can then set
     private_data via CURLOPT_PRIVATE if they so choose. */
  DEBUGASSERT(!doh->समुच्चयः.वैयक्तिकम्_दत्त);

  यदि(Curl_multi_सङ्कलय_handle(multi, doh))
    गच्छ error;

  *pmid = doh->mid;
  प्रत्यागम CURLE_OK;

error:
  Curl_close(&doh);
  यदि(doh_req)
    doh_probe_dtor(रिक्त, 0, doh_req);
  प्रत्यागम परिणाम;
}

/*
 * Curl_doh() starts a name resolve using DoH. It resolves a name and returns
 * a 'Curl_addrinfo *' with the address information.
 */

CURLcode Curl_doh(संरचना Curl_easy *दत्त,
                  संरचना Curl_resolv_async *async)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना doh_probes *dohp = रिक्त;
  size_t i;

  DEBUGASSERT(!async->doh);
  DEBUGASSERT(async->peer->hostname[0]);
  यदि(async->doh) {
    DEBUGASSERT(0); /* should not happen */
    Curl_doh_cleanup(दत्त, async);
  }

  यदि(!async->dns_queries)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
#यदि_परिभाषितम् USE_HTTPSRR
  यदि(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     (async->dns_queries & CURL_DNSQ_HTTPS)) {
    /* Can't mix those in the same async resolve */
    DEBUGASSERT(0);
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  }
#अन्यथा
  यदि(async->dns_queries & CURL_DNSQ_HTTPS) {
    DEBUGASSERT(0);
    प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
  }
#यदि_अन्तः

  /* start clean, consider allocating this struct on demand */
  async->doh = dohp = curlx_calloc(1, परिमाण(संरचना doh_probes));
  यदि(!dohp)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  प्रति(i = 0; i < DOH_SLOT_गणनम्; ++i) {
    dohp->probe_rc[i] = DOH_OK;
    dohp->probe_mid[i] = UINT32_MAX;
  }

#यदि_परिभाषितम् USE_IPV6
  /* AAAA results have preference in happy eyeballing, trigger first */
  यदि(async->dns_queries & CURL_DNSQ_AAAA) {
    /* create IPv6 DoH request */
    परिणाम = doh_probe_सततम्(दत्त, CURL_DNS_TYPE_AAAA,
                           async->peer->hostname,
                           CURL_EASY_STR(दत्त, STRING_DOH),
                           दत्त->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV6]);
    यदि(परिणाम)
      गच्छ error;
    async->queries_ongoing++;
  }
#यदि_अन्तः

  /* create IPv4 DoH request */
  यदि(async->dns_queries & CURL_DNSQ_A) {
    परिणाम = doh_probe_सततम्(दत्त, CURL_DNS_TYPE_A,
                           async->peer->hostname,
                           CURL_EASY_STR(दत्त, STRING_DOH),
                           दत्त->multi, async->id,
                           &dohp->probe_mid[DOH_SLOT_IPV4]);
    यदि(परिणाम)
      गच्छ error;
    async->queries_ongoing++;
  }

#यदि_परिभाषितम् USE_HTTPSRR
  यदि(async->dns_queries & CURL_DNSQ_HTTPS) {
    वर्ण *qname = रिक्त;
    यदि(async->peer->पत्तनम् != पत्तनम्_HTTPS) {
      qname = curl_maprintf("_%u._https.%s",
                            async->peer->पत्तनम्, async->peer->hostname);
      यदि(!qname)
        गच्छ error;
    }
    परिणाम = doh_probe_सततम्(दत्त, CURL_DNS_TYPE_HTTPS,
                           qname ? qname : async->peer->hostname,
                           CURL_EASY_STR(दत्त, STRING_DOH), दत्त->multi,
                           async->id,
                           &dohp->probe_mid[DOH_SLOT_HTTPS_RR]);
    curlx_free(qname);
    यदि(परिणाम)
      गच्छ error;
    async->queries_ongoing++;
  }
#यदि_अन्तः
  प्रत्यागम CURLE_OK;

error:
  Curl_doh_cleanup(दत्त, async);
  प्रत्यागम परिणाम;
}

स्थिर DOHcode doh_skipqname(ध्रुव अचिह्नित वर्ण *doh, size_t dohlen,
                             अचिह्नित पूर्णाङ्क *indexp)
{
  अचिह्नित वर्ण दीर्घता;
  कुरु {
    यदि(dohlen < (*indexp + 1))
      प्रत्यागम DOH_DNS_बहिर्गमनम्_सम्बन्धि_RANGE;
    दीर्घता = doh[*indexp];
    यदि((दीर्घता & 0xc0) == 0xc0) {
      /* name pointer, advance over it and be done */
      यदि(dohlen < (*indexp + 2))
        प्रत्यागम DOH_DNS_बहिर्गमनम्_सम्बन्धि_RANGE;
      *indexp += 2;
      भङ्ग;
    }
    यदि(दीर्घता & 0xc0)
      प्रत्यागम DOH_DNS_BAD_LABEL;
    यदि(dohlen < (*indexp + 1 + दीर्घता))
      प्रत्यागम DOH_DNS_बहिर्गमनम्_सम्बन्धि_RANGE;
    *indexp += (अचिह्नित पूर्णाङ्क)(1 + दीर्घता);
  } यावत्(दीर्घता);
  प्रत्यागम DOH_OK;
}

स्थिर अचिह्नित ह्रस्व doh_get16bit(ध्रुव अचिह्नित वर्ण *doh,
                                   अचिह्नित पूर्णाङ्क सूचकांक)
{
  प्रत्यागम (अचिह्नित ह्रस्व)((doh[सूचकांक] << 8) | doh[सूचकांक + 1]);
}

स्थिर अचिह्नित पूर्णाङ्क doh_get32bit(ध्रुव अचिह्नित वर्ण *doh, अचिह्नित पूर्णाङ्क सूचकांक)
{
  /* make clang and gcc optimize this to bswap by incrementing
     the pointer first. */
  doh += सूचकांक;

  /* avoid undefined behavior by casting to unsigned before shifting
     24 bits, possibly into the sign bit. codegen is same, but
     ub sanitizer will not be upset */
  प्रत्यागम ((अचिह्नित)doh[0] << 24) | ((अचिह्नित)doh[1] << 16) |
         ((अचिह्नित)doh[2] << 8) | doh[3];
}

स्थिर शून्य doh_store_a(ध्रुव अचिह्नित वर्ण *doh, पूर्णाङ्क सूचकांक,
                        संरचना dohentry *d)
{
  /* silently ignore addresses over the limit */
  यदि(d->numaddr < DOH_MAX_ADDR) {
    संरचना dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_A;
    स्मृतिं_प्रतिलिख(&a->ip.v4, &doh[सूचकांक], 4);
    d->numaddr++;
  }
}

स्थिर शून्य doh_store_aaaa(ध्रुव अचिह्नित वर्ण *doh, पूर्णाङ्क सूचकांक,
                           संरचना dohentry *d)
{
  /* silently ignore addresses over the limit */
  यदि(d->numaddr < DOH_MAX_ADDR) {
    संरचना dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_AAAA;
    स्मृतिं_प्रतिलिख(&a->ip.v6, &doh[सूचकांक], 16);
    d->numaddr++;
  }
}

#यदि_परिभाषितम् USE_HTTPSRR
स्थिर DOHcode doh_store_https(ध्रुव अचिह्नित वर्ण *doh, पूर्णाङ्क सूचकांक,
                               संरचना dohentry *d, uint16_t len)
{
  /* silently ignore RRs over the limit */
  यदि(d->numhttps_rrs < DOH_MAX_HTTPS) {
    संरचना dohhttps_rr *h = &d->https_rrs[d->numhttps_rrs];
    h->val = curlx_memdup(&doh[सूचकांक], len);
    यदि(!h->val)
      प्रत्यागम DOH_बहिर्गमनम्_सम्बन्धि_MEM;
    h->len = len;
    d->numhttps_rrs++;
  }
  प्रत्यागम DOH_OK;
}
#यदि_अन्तः

स्थिर DOHcode doh_rdata(ध्रुव अचिह्नित वर्ण *doh,
                         अचिह्नित ह्रस्व rdlength,
                         अचिह्नित ह्रस्व type,
                         पूर्णाङ्क सूचकांक,
                         संरचना dohentry *d)
{
  /* RDATA
     - A (TYPE 1): 4 bytes
     - AAAA (TYPE 28): 16 bytes
     - HTTPS (TYPE 65): N bytes */
  विकल्प(type) {
  पक्ष CURL_DNS_TYPE_A:
    यदि(rdlength != 4)
      प्रत्यागम DOH_DNS_RDATA_LEN;
    doh_store_a(doh, सूचकांक, d);
    भङ्ग;
  पक्ष CURL_DNS_TYPE_AAAA:
    यदि(rdlength != 16)
      प्रत्यागम DOH_DNS_RDATA_LEN;
    doh_store_aaaa(doh, सूचकांक, d);
    भङ्ग;
#यदि_परिभाषितम् USE_HTTPSRR
  पक्ष CURL_DNS_TYPE_HTTPS:
    यदि(rdlength < 3)
      प्रत्यागम DOH_DNS_RDATA_LEN;
    प्रत्यागम doh_store_https(doh, सूचकांक, d, rdlength);
#यदि_अन्तः
  स्वभाव:
    /* unsupported type, or type we do not store, skip it */
    भङ्ग;
  }
  प्रत्यागम DOH_OK;
}

/* @unittest 1655 */
UNITTEST शून्य de_init(संरचना dohentry *de);
UNITTEST शून्य de_init(संरचना dohentry *de)
{
  स्मृतिं_पूरय(de, 0, परिमाण(*de));
  de->ttl = अधिकतम_पूर्णाङ्कः;
}

/* TTL value cap */
#परिभाषय MAX_DNS_TTL 86400U /* 24 hours */
/* @unittest 1650 */
UNITTEST DOHcode doh_resp_decode(ध्रुव अचिह्नित वर्ण *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 संरचना dohentry *d);
UNITTEST DOHcode doh_resp_decode(ध्रुव अचिह्नित वर्ण *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 संरचना dohentry *d)
{
  अचिह्नित वर्ण rcode;
  अचिह्नित ह्रस्व qdcount;
  अचिह्नित ह्रस्व ancount;
  अचिह्नित ह्रस्व type = 0;
  अचिह्नित ह्रस्व rdlength;
  अचिह्नित ह्रस्व nscount;
  अचिह्नित ह्रस्व arcount;
  अचिह्नित पूर्णाङ्क सूचकांक = 12;
  DOHcode rc;

  यदि(dohlen < 12)
    प्रत्यागम DOH_TOO_SMALL_बफर; /* too small */
  यदि(!doh || doh[0] || doh[1])
    प्रत्यागम DOH_DNS_BAD_ID; /* bad ID */
  rcode = doh[3] & 0x0f;
  यदि(rcode == 3)
    प्रत्यागम DOH_DNS_NXDOMAIN; /* name does not exist */
  यदि(rcode)
    प्रत्यागम DOH_DNS_BAD_RCODE; /* bad rcode */

  qdcount = doh_get16bit(doh, 4);
  यावत्(qdcount) {
    rc = doh_skipqname(doh, dohlen, &सूचकांक);
    यदि(rc)
      प्रत्यागम rc; /* bad qname */
    यदि(dohlen < (सूचकांक + 4))
      प्रत्यागम DOH_DNS_बहिर्गमनम्_सम्बन्धि_RANGE;
    सूचकांक += 4; /* skip question's type and class */
    qdcount--;
  }

  ancount = doh_get16bit(doh, 6);
  यावत्(ancount) {
    अचिह्नित ह्रस्व dnsclass;
    अचिह्नित पूर्णाङ्क ttl;

    rc = doh_skipqname(doh, dohlen, &सूचकांक);
    यदि(rc)
      प्रत्यागम rc; /* bad qname */

    यदि(dohlen < (सूचकांक + 2))
      प्रत्यागम DOH_DNS_बहिर्गमनम्_सम्बन्धि_RANGE;

    type = doh_get16bit(doh, सूचकांक);
    यदि((type != CURL_DNS_TYPE_CNAME) &&  /* may be synthesized from DNAME */
       (type != CURL_DNS_TYPE_DNAME) &&  /* if present, accept and ignore */
       (type != dnstype))
      /* Not the same type as was asked for, nor CNAME nor DNAME */
      प्रत्यागम DOH_DNS_UNEXPECTED_TYPE;
    सूचकांक += 2;

    यदि(dohlen < (सूचकांक + 2))
      प्रत्यागम DOH_DNS_बहिर्गमनम्_सम्बन्धि_RANGE;
    dnsclass = doh_get16bit(doh, सूचकांक);
    यदि(DNS_CLASS_अन्तर् != dnsclass)
      प्रत्यागम DOH_DNS_UNEXPECTED_CLASS; /* unsupported */
    सूचकांक += 2;

    यदि(dohlen < (सूचकांक + 4))
      प्रत्यागम DOH_DNS_बहिर्गमनम्_सम्बन्धि_RANGE;

    ttl = doh_get32bit(doh, सूचकांक);
    यदि(ttl > MAX_DNS_TTL)
      ttl = MAX_DNS_TTL;
    यदि(ttl < d->ttl)
      d->ttl = ttl;
    सूचकांक += 4;

    यदि(dohlen < (सूचकांक + 2))
      प्रत्यागम DOH_DNS_बहिर्गमनम्_सम्बन्धि_RANGE;

    rdlength = doh_get16bit(doh, सूचकांक);
    सूचकांक += 2;
    यदि(dohlen < (सूचकांक + rdlength))
      प्रत्यागम DOH_DNS_बहिर्गमनम्_सम्बन्धि_RANGE;

    rc = doh_rdata(doh, rdlength, type, (पूर्णाङ्क)सूचकांक, d);
    यदि(rc)
      प्रत्यागम rc;
    सूचकांक += rdlength;
    ancount--;
  }

  nscount = doh_get16bit(doh, 8);
  यावत्(nscount) {
    rc = doh_skipqname(doh, dohlen, &सूचकांक);
    यदि(rc)
      प्रत्यागम rc; /* bad qname */

    यदि(dohlen < (सूचकांक + 8))
      प्रत्यागम DOH_DNS_बहिर्गमनम्_सम्बन्धि_RANGE;

    सूचकांक += 2 + 2 + 4; /* type, dnsclass and ttl */

    यदि(dohlen < (सूचकांक + 2))
      प्रत्यागम DOH_DNS_बहिर्गमनम्_सम्बन्धि_RANGE;

    rdlength = doh_get16bit(doh, सूचकांक);
    सूचकांक += 2;
    यदि(dohlen < (सूचकांक + rdlength))
      प्रत्यागम DOH_DNS_बहिर्गमनम्_सम्बन्धि_RANGE;
    सूचकांक += rdlength;
    nscount--;
  }

  arcount = doh_get16bit(doh, 10);
  यावत्(arcount) {
    rc = doh_skipqname(doh, dohlen, &सूचकांक);
    यदि(rc)
      प्रत्यागम rc; /* bad qname */

    यदि(dohlen < (सूचकांक + 8))
      प्रत्यागम DOH_DNS_बहिर्गमनम्_सम्बन्धि_RANGE;

    सूचकांक += 2 + 2 + 4; /* type, dnsclass and ttl */

    यदि(dohlen < (सूचकांक + 2))
      प्रत्यागम DOH_DNS_बहिर्गमनम्_सम्बन्धि_RANGE;

    rdlength = doh_get16bit(doh, सूचकांक);
    सूचकांक += 2;
    यदि(dohlen < (सूचकांक + rdlength))
      प्रत्यागम DOH_DNS_बहिर्गमनम्_सम्बन्धि_RANGE;
    सूचकांक += rdlength;
    arcount--;
  }

  यदि(सूचकांक != dohlen)
    प्रत्यागम DOH_DNS_MALFORMAT; /* something is wrong */

  प्रत्यागम DOH_OK; /* ok */
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
स्थिर CURLcode doh2ai(ध्रुव संरचना dohentry *de, ध्रुव वर्ण *hostname,
                       पूर्णाङ्क पत्तनम्, संरचना Curl_addrinfo **aip)
{
  संरचना Curl_addrinfo *ai;
  संरचना Curl_addrinfo *prevai = रिक्त;
  संरचना Curl_addrinfo *firstai = रिक्त;
  संरचना sockaddr_in *addr;
#यदि_परिभाषितम् USE_IPV6
  संरचना sockaddr_in6 *addr6;
#यदि_अन्तः
  size_t hostlen = वाक्यदैर्घ्यम्(hostname) + 1; /* include null-terminator */
  CURLcode परिणाम = CURLE_OK;
  पूर्णाङ्क i;

  प्रति(i = 0; i < de->numaddr; i++) {
    size_t ss_आकार;
    CURL_SA_FAMILY_T addrtype;
    यदि(de->addr[i].type == CURL_DNS_TYPE_AAAA) {
#यदि_अपरिभाषितम् USE_IPV6
      /* we cannot handle IPv6 addresses */
      अनुवर्त;
#अन्यथा
      ss_आकार = परिमाण(संरचना sockaddr_in6);
      addrtype = AF_INET6;
#यदि_अन्तः
    }
    अन्यथा {
      ss_आकार = परिमाण(संरचना sockaddr_in);
      addrtype = AF_INET;
    }

    ai = curlx_calloc(1, परिमाण(संरचना Curl_addrinfo) + ss_आकार + hostlen);
    यदि(!ai) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      भङ्ग;
    }
    ai->ai_addr = (शून्य *)((वर्ण *)ai + परिमाण(संरचना Curl_addrinfo));
    ai->ai_canonname = (शून्य *)((वर्ण *)ai->ai_addr + ss_आकार);
    स्मृतिं_प्रतिलिख(ai->ai_canonname, hostname, hostlen);

    यदि(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    यदि(prevai)
      /* make the previous entry point to this */
      prevai->ai_अग्र = ai;

    ai->ai_family = addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_आकार;

    /* leave the rest of the struct filled with zero */

    विकल्प(ai->ai_family) {
    पक्ष AF_INET:
      addr = (शून्य *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(परिमाण(संरचना in_addr) == परिमाण(de->addr[i].ip.v4));
      स्मृतिं_प्रतिलिख(&addr->sin_addr, &de->addr[i].ip.v4, परिमाण(संरचना in_addr));
      addr->sin_family = addrtype;
      addr->sin_port = htons((अचिह्नित ह्रस्व)पत्तनम्);
      भङ्ग;

#यदि_परिभाषितम् USE_IPV6
    पक्ष AF_INET6:
      addr6 = (शून्य *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(परिमाण(संरचना in6_addr) == परिमाण(de->addr[i].ip.v6));
      स्मृतिं_प्रतिलिख(&addr6->sin6_addr, &de->addr[i].ip.v6, परिमाण(संरचना in6_addr));
      addr6->sin6_family = addrtype;
      addr6->sin6_port = htons((अचिह्नित ह्रस्व)पत्तनम्);
      भङ्ग;
#यदि_अन्तः
    }

    prevai = ai;
  }

  यदि(परिणाम) {
    Curl_freeaddrinfo(firstai);
    firstai = रिक्त;
  }
  *aip = firstai;

  प्रत्यागम परिणाम;
}

/* @unittest 1655 */
UNITTEST शून्य de_cleanup(संरचना dohentry *d);
UNITTEST शून्य de_cleanup(संरचना dohentry *d)
{
#यदि_परिभाषितम् USE_HTTPSRR
  पूर्णाङ्क i = 0;
  प्रति(i = 0; i < d->numhttps_rrs; i++)
    curlx_safefree(d->https_rrs[i].val);
#अन्यथा
  (शून्य)d;
#यदि_अन्तः
}

#यदि_परिभाषितम् USE_HTTPSRR

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
स्थिर CURLcode doh_decode_rdata_name(ध्रुव अचिह्नित वर्ण **buf,
                                      size_t *remaining, वर्ण **dnsname)
{
  ध्रुव अचिह्नित वर्ण *cp = रिक्त;
  size_t rem = 0;
  अचिह्नित वर्ण clen = 0; /* chunk len */
  संरचना dynbuf thename;

  DEBUGASSERT(buf && remaining && dnsname);
  यदि(!buf || !remaining || !dnsname || !*remaining)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  curlx_dyn_init(&thename, CURL_MAXLEN_HOST_NAME);
  rem = *remaining;
  cp = *buf;
  clen = *cp++;
  /* RFC 9460 says it must be uncompressed */
  यदि(clen > 63)
    प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;

  यदि(clen == 0) {
    /* special case - return "." as name */
    यदि(curlx_dyn_addn(&thename, ".", 1))
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
  यावत्(clen) {
    यदि(clen >= rem) {
      curlx_dyn_free(&thename);
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
    यदि(curlx_dyn_addn(&thename, cp, clen) ||
       curlx_dyn_addn(&thename, ".", 1))
      प्रत्यागम CURLE_TOO_LARGE;

    cp += clen;
    rem -= (clen + 1);
    यदि(rem <= 0) {
      curlx_dyn_free(&thename);
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
    clen = *cp++;
    यदि(clen > 63) {
      /* invalid format */
      curlx_dyn_free(&thename);
      प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
    }
  }
  *buf = cp;
  *remaining = rem - 1;
  *dnsname = curlx_dyn_ptr(&thename);
  प्रत्यागम CURLE_OK;
}

/* scan for byte values <= 32 or 127 */
स्थिर CURLcode junkscan(ध्रुव वर्ण *url)
{
  ध्रुव अचिह्नित वर्ण *p = (ध्रुव अचिह्नित वर्ण *)url;
  यावत्(*p) {
    यदि(*p <= 0x20 || *p == 127)
      प्रत्यागम CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
    p++;
  }
  प्रत्यागम CURLE_OK;
}

/* @unittest 1658 */
UNITTEST CURLcode doh_resp_decode_httpsrr(संरचना Curl_easy *दत्त,
                                          ध्रुव अचिह्नित वर्ण *cp, size_t len,
                                          संरचना Curl_https_rrinfo **hrr);
UNITTEST CURLcode doh_resp_decode_httpsrr(संरचना Curl_easy *दत्त,
                                          ध्रुव अचिह्नित वर्ण *cp, size_t len,
                                          संरचना Curl_https_rrinfo **hrr)
{
  uint16_t pcode = 0, plen = 0;
  uint32_t expected_min_pcode = 0;
  संरचना Curl_https_rrinfo *lhrr = रिक्त;
  वर्ण *dnsname = रिक्त;
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  (शून्य)दत्त;
  *hrr = रिक्त;
  यदि(len <= 2)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  lhrr = curlx_calloc(1, परिमाण(संरचना Curl_https_rrinfo));
  यदि(!lhrr)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  lhrr->प्राथमिकता = doh_get16bit(cp, 0);
  cp += 2;
  len -= 2;
  यदि(doh_decode_rdata_name(&cp, &len, &dnsname) != CURLE_OK)
    गच्छ err;
  lhrr->target = dnsname;
  परिणाम = junkscan(dnsname);
  यदि(परिणाम) {
    /* unacceptable hostname content */
    गच्छ err;
  }
  यावत्(len >= 4) {
    pcode = doh_get16bit(cp, 0);
    plen = doh_get16bit(cp, 2);
    cp += 4;
    len -= 4;
    यदि(pcode < expected_min_pcode || plen > len) {
      परिणाम = CURLE_WEIRD_SERVER_प्रत्युत्तरम्;
      गच्छ err;
    }
    परिणाम = Curl_httpsrr_समुच्चयः(lhrr, pcode, cp, plen);
    यदि(परिणाम)
      गच्छ err;
    Curl_httpsrr_trace(दत्त, lhrr);
    cp += plen;
    len -= plen;
    expected_min_pcode = pcode + 1;
  }
  *hrr = lhrr;
  प्रत्यागम CURLE_OK;
err:
  Curl_httpsrr_destroy(lhrr);
  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* USE_HTTPSRR */

/* called from multi when a sub transfer, e.g. doh probe, is done.
 * Parse the response and set the results in the `async` context
 * of master, using the id from the probe's CURL_EZM_DOH_PROBE
 * meta data. */
स्थिर शून्य doh_probe_समाप्तम्(संरचना Curl_easy *doh,
                           संरचना Curl_easy *master, CURLcode परिणाम)
{
  संरचना Curl_resolv_async *async = रिक्त;
  संरचना doh_probes *dohp = रिक्त;
  संरचना doh_request *doh_req = रिक्त;
  संरचना Curl_addrinfo **pdest_ai;
  संरचना dohentry de;
  पूर्णाङ्क slot, httpcode;

  de_init(&de);
  doh_req = Curl_meta_get(doh, CURL_EZM_DOH_PROBE);
  यदि(!doh_req) {
    /* transfer `doh` is not a DoH probe. */
    DEBUGASSERT(0);
    गच्छ बहिर्गमनम्;
  }

  async = Curl_async_get(master, doh_req->resolv_id);
  यदि(!async) {
    CURL_TRC_DNS(master, "[%u] ignoring outdated DoH response",
                 doh_req->resolv_id);
    गच्छ बहिर्गमनम्;
  }
  dohp = async->doh;

  प्रति(slot = 0; slot < DOH_SLOT_गणनम्; ++slot) {
    यदि(dohp->probe_mid[slot] == doh->mid)
      भङ्ग;
  }
  /* We really should have found the slot where to store the response */
  यदि(slot >= DOH_SLOT_गणनम्) {
    failf(master, "DoH: unknown sub request done");
    DEBUGASSERT(0);
    गच्छ बहिर्गमनम्;
  }

  async->queries_ongoing--;
  dohp = async->doh;
  httpcode = doh->info.httpcode;
  विकल्प(slot) {
  पक्ष DOH_SLOT_IPV4:
    async->dns_responses |= CURL_DNSQ_A;
    भङ्ग;
#यदि_परिभाषितम् USE_IPV6
  पक्ष DOH_SLOT_IPV6:
    async->dns_responses |= CURL_DNSQ_AAAA;
    भङ्ग;
#यदि_अन्तः
#यदि_परिभाषितम् USE_HTTPSRR
  पक्ष DOH_SLOT_HTTPS_RR:
    async->dns_responses |= CURL_DNSQ_HTTPS;
    भङ्ग;
#यदि_अन्तः
  स्वभाव:
    DEBUGASSERT(0);
    भङ्ग;
  }

  यदि(परिणाम) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: %s",
          doh_type2name(doh_req->dnstype), curl_easy_strerror(परिणाम));
    गच्छ बहिर्गमनम्;
  }
  अन्यथा यदि((httpcode < 200) || (httpcode >= 300)) {
    dohp->probe_rc[slot] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: HTTP status %d",
          doh_type2name(doh_req->dnstype), httpcode);
    गच्छ बहिर्गमनम्;
  }

  dohp->probe_rc[slot] = doh_resp_decode(curlx_dyn_uptr(&doh_req->resp_body),
                                         curlx_dyn_len(&doh_req->resp_body),
                                         doh_req->dnstype, &de);
  यदि(dohp->probe_rc[slot]) {
#यदि_परिभाषितम् USE_HTTPSRR
    यदि((dohp->probe_rc[slot] == DOH_NO_CONTENT) &&
       (doh_req->dnstype == CURL_DNS_TYPE_HTTPS)) {
      dohp->probe_rc[slot] = DOH_DNS_NXDOMAIN;
    }
#यदि_अन्तः
    infof(doh, "[DoH] [%s] error decoding response: %s",
          doh_type2name(doh_req->dnstype),
          doh_strerror(dohp->probe_rc[slot]));
    गच्छ बहिर्गमनम्;
  }

  यदि(doh_req->dnstype == CURL_DNS_TYPE_A)
    pdest_ai = &async->ai_A;
  अन्यथा यदि(doh_req->dnstype == CURL_DNS_TYPE_AAAA)
    pdest_ai = &async->ai_AAAA;
  अन्यथा
    pdest_ai = रिक्त;

  यदि(pdest_ai && de.numaddr) {
    यदि(*pdest_ai) {
      Curl_freeaddrinfo(*pdest_ai);
      *pdest_ai = रिक्त;
    }
    परिणाम = doh2ai(&de, async->peer->hostname, async->peer->पत्तनम्, pdest_ai);
    यदि(परिणाम) { /* hard failure on our side, fail completely */
      infof(doh, "[DoH] [%s] error creating addrinfo: %s",
            doh_type2name(doh_req->dnstype), curl_easy_strerror(परिणाम));
      dohp->probe_rc[slot] = DOH_OOM;
      async->परिणाम = परिणाम;
    }
  }
#यदि_परिभाषितम् USE_HTTPSRR
  अन्यथा यदि((doh_req->dnstype == CURL_DNS_TYPE_HTTPS) && de.numhttps_rrs) {
    CURL_TRC_DNS(doh, "[HTTPS] got %d records", de.numhttps_rrs);
    परिणाम = doh_resp_decode_httpsrr(doh, de.https_rrs->val,
                                     de.https_rrs->len, &async->httpsrr);
    यदि(परिणाम) {
      dohp->probe_rc[slot] = DOH_HTTP_FAILED;
      infof(doh, "[DoH] error decoding HTTPS RR: %s",
            curl_easy_strerror(परिणाम));
      गच्छ बहिर्गमनम्;
    }
  }
#यदि_अन्तः /* USE_HTTPSRR */

  /* DoH request complete, run master to act on results */
  infof(doh, "DoH request complete, %u to go", async->queries_ongoing);

बहिर्गमनम्:
  Curl_multi_mark_dirty(master);
  de_cleanup(&de);
  Curl_meta_remove(doh, CURL_EZM_DOH_PROBE);
}

CURLcode Curl_doh_take_परिणाम(संरचना Curl_easy *दत्त,
                              संरचना Curl_resolv_async *async,
                              संरचना Curl_dns_प्रविष्टिः **pdns)
{
  संरचना doh_probes *dohp = async->doh;
  CURLcode परिणाम = CURLE_OK;

  *pdns = रिक्त; /* defaults to no response */
  यदि(!dohp)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  async->negative_answer = FALSE;
  यदि(async->परिणाम) {
    परिणाम = async->परिणाम;
    गच्छ बहिर्गमनम्;
  }

  यदि(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     dohp->probe_mid[DOH_SLOT_IPV4] == UINT32_MAX &&
     dohp->probe_mid[DOH_SLOT_IPV6] == UINT32_MAX) {
    failf(दत्त, "Could not DoH-resolve: %s", async->peer->hostname);
    प्रत्यागम async->for_proxy ?
      CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
  }
  अन्यथा यदि(!async->queries_ongoing) {
    संरचना Curl_dns_प्रविष्टिः *dns = रिक्त;
    bool negative = TRUE;
    पूर्णाङ्क slot;

    /* remove DoH handles from multi handle and close them */
    doh_close(दत्त, async);
    /* parse the responses, create the struct and return it! */
    प्रति(slot = 0; slot < DOH_SLOT_गणनम्; slot++) {
      /* Failing without an NXDOMAIN answer - a SERVFAIL-class rcode or
         an undecodable response - says nothing about the name. Such a
         failure must not be cached as a negative entry. */
      यदि(dohp->probe_rc[slot] && (dohp->probe_rc[slot] != DOH_DNS_NXDOMAIN))
        negative = FALSE;
    } /* next slot */

    यदि(async->ai_A || async->ai_AAAA) {
      dns = Curl_dnsc_mk_addr2(
        दत्त, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
      यदि(!dns) {
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        गच्छ बहिर्गमनम्;
      }
    }
#यदि_परिभाषितम् USE_HTTPSRR
    अन्यथा यदि((async->dns_queries & CURL_DNSQ_HTTPS) &&
            !dohp->probe_rc[DOH_SLOT_HTTPS_RR]) {
      Curl_httpsrr_trace(दत्त, async->httpsrr);
      dns = Curl_dnsc_mk_https(दत्त, &async->httpsrr, async->peer);
      यदि(!dns) {
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        गच्छ बहिर्गमनम्;
      }
    }
#यदि_अन्तः /* USE_HTTPSRR */
    अन्यथा {
      /* every query failed. Only NXDOMAIN answers for all of them
         make this a negative answer, eligible for caching. */
      async->negative_answer = negative;
      परिणाम = async->for_proxy ?
        CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
    }

    /* and add the entry to the cache */
    यदि(dns)
      परिणाम = Curl_dnscache_सङ्कलय(दत्त, dns);
    *pdns = dns;
  } /* !async->queries_ongoing */
  अन्यथा
    /* wait for pending DoH transactions to complete */
    प्रत्यागम CURLE_AGAIN;

बहिर्गमनम्:
  Curl_doh_cleanup(दत्त, async);
  प्रत्यागम परिणाम;
}

स्थिर शून्य doh_close(संरचना Curl_easy *दत्त,
                      संरचना Curl_resolv_async *async)
{
  संरचना doh_probes *doh = async ? async->doh : रिक्त;
  यदि(doh && दत्त->multi) {
    संरचना Curl_easy *probe_दत्त;
    uint32_t mid;
    size_t slot;
    प्रति(slot = 0; slot < DOH_SLOT_गणनम्; slot++) {
      mid = doh->probe_mid[slot];
      यदि(mid == UINT32_MAX)
        अनुवर्त;
      doh->probe_mid[slot] = UINT32_MAX;
      /* should have been called before data is removed from multi handle */
      DEBUGASSERT(दत्त->multi);
      probe_दत्त = दत्त->multi ? Curl_multi_get_easy(दत्त->multi, mid) : रिक्त;
      यदि(!probe_दत्त) {
        DEBUGF(infof(दत्त, "Curl_doh_close: xfer for mid=%u not found!", mid));
        अनुवर्त;
      }
      probe_दत्त->sub_xfer_समाप्तम् = रिक्त; /* No longer interested in result */
      /* data->multi might already be reset at this time */
      Curl_multi_remove_handle(दत्त->multi, probe_दत्त);
      Curl_close(&probe_दत्त);
    }
    CURL_TRC_DNS(दत्त, "[DoH] probe done");
  }
}

शून्य Curl_doh_cleanup(संरचना Curl_easy *दत्त,
                      संरचना Curl_resolv_async *async)
{
  संरचना doh_probes *dohp = async->doh;
  यदि(dohp) {
    doh_close(दत्त, async);
    curlx_safefree(async->doh);
  }
}

#यदि_अन्तः /* CURL_DISABLE_DOH */
