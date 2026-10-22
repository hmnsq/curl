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
#ضمن "curl_setup.h"

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_DOH

#ضمن "urldata.h"
#ضمن "curl_addrinfo.h"
#ضمن "curl_trc.h"
#ضمن "multiif.h"
#ضمن "url.h"
#ضمن "connect.h"
#ضمن "vdns/doh.h"
#ضمن "vdns/httpsrr.h"
#ضمن "curlx/strdup.h"
#ضمن "curlx/dynbuf.h"
#ضمن "escape.h"  /* for Curl_hexencode() */
#ضمن "urlapi-int.h"

#وصف_ڪريو DNS_CLASS_اندريون 0x01

ساكن فراغ doh_close(بنية Curl_easy *معلومات,
                      بنية Curl_resolv_async *async);

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
ساكن ثابت حرف * ثابت doh_ڪوڊ_str[] = {
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

ساكن ثابت حرف *doh_strerror(DOHcode ڪوڊ)
{
  إذا((size_t)ڪوڊ < CURL_ARRAYSIZE(doh_ڪوڊ_str))
    أرجع doh_ڪوڊ_str[ڪوڊ];
  أرجع "bad error code";
}

ساكن ثابت حرف *doh_type2name(DNStype dnstype)
{
  بدل(dnstype) {
  حالة CURL_DNS_TYPE_A:
    أرجع "A";
  حالة CURL_DNS_TYPE_AAAA:
    أرجع "AAAA";
#جيڪڏهن_وصف_ٿيل USE_HTTPSRR
  حالة CURL_DNS_TYPE_HTTPS:
    أرجع "HTTPS";
#جيڪڏهن_پورو
  افتراضي:
    أرجع "unknown";
  }
}

#جيڪڏهن_پورو /* CURLVERBOSE */

/* @unittest 1655
 */
UNITTEST DOHcode doh_req_encode(ثابت حرف *host,
                                DNStype dnstype,
                                بنانشان حرف *dnsp,  /*بفر*/
                                size_t len,  /* buffer size */
                                size_t *olen);  /* output length */
UNITTEST DOHcode doh_req_encode(ثابت حرف *host,
                                DNStype dnstype,
                                بنانشان حرف *dnsp, /*بفر*/
                                size_t len,   /* buffer size */
                                size_t *olen) /* output length */
{
  ثابت size_t hostlen = لڙي_ڊيگهه(host);
  بنانشان حرف *orig = dnsp;
  ثابت حرف *hostp = host;

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
  إذا(host[hostlen - 1] != '.')
    expected_len++;

  إذا(expected_len > DOH_MAX_DNSREQ_سائيز)
    أرجع DOH_DNS_NAME_TOO_LONG;

  إذا(len < expected_len)
    أرجع DOH_TOO_SMALL_بفر;

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
  طالما(*hostp) {
    size_t labellen;
    ثابت حرف *dot = strchr(hostp, '.');
    إذا(dot)
      labellen = dot - hostp;
    وإلا
      labellen = لڙي_ڊيگهه(hostp);
    إذا((labellen > 63) || (!labellen)) {
      /* label is too long or too short, error out */
      *olen = 0;
      أرجع DOH_DNS_BAD_LABEL;
    }
    /* label is non-empty, process it */
    *dnsp++ = (بنانشان حرف)labellen;
    يادگيري_نقل(dnsp, hostp, labellen);
    dnsp += labellen;
    hostp += labellen;
    /* advance past dot, but only if there is one */
    إذا(dot)
      hostp++;
  } /* next label */

  *dnsp++ = 0; /* append zero-length label for root */

  /* There are assigned TYPE codes beyond 255: use range [1..65535] */
  *dnsp++ = (بنانشان حرف)(255 & (dnstype >> 8)); /* upper 8-bit TYPE */
  *dnsp++ = (بنانشان حرف)(255 & dnstype);        /* lower 8-bit TYPE */

  *dnsp++ = '\0'; /* upper 8-bit CLASS */
  *dnsp++ = DNS_CLASS_اندريون; /* IN - "the Internet" */

  *olen = dnsp - orig;

  /* verify that our estimation of length is valid, since
   * this has led to buffer overflows in this function */
  DEBUGASSERT(*olen == expected_len);
  أرجع DOH_OK;
}

ساكن size_t doh_probe_write_cb(حرف *contents, size_t سائيز, size_t nmemb,
                                 فراغ *userp)
{
  size_t realsize = سائيز * nmemb;
  بنية Curl_easy *معلومات = userp;
  بنية doh_request *doh_req = Curl_meta_get(معلومات, CURL_EZM_DOH_PROBE);
  إذا(!doh_req)
    أرجع CURL_WRITEFUNC_ERROR;

  إذا(curlx_dyn_addn(&doh_req->resp_body, contents, realsize))
    أرجع 0;

  أرجع realsize;
}

ساكن فراغ doh_probe_پورو(بنية Curl_easy *doh,
                           بنية Curl_easy *master, CURLcode نتيجو);
ساكن فراغ doh_probe_dtor(ثابت فراغ *ڪنجي, size_t klen, فراغ *e)
{
  (فراغ)ڪنجي;
  (فراغ)klen;
  إذا(e) {
    بنية doh_request *doh_req = e;
    curl_slist_free_all(doh_req->req_hds);
    curlx_dyn_free(&doh_req->resp_body);
    curlx_free(e);
  }
}

#وصف_ڪريو ERROR_CHECK_SETOPT(x, y)                        \
  افعل {                                                  \
    نتيجو = curl_easy_setopt((CURL *)doh, x, y);       \
    إذا(نتيجو &&                                        \
       نتيجو != CURLE_NOT_BUILT_اندريون &&                  \
       نتيجو != CURLE_UNKNOWN_OPTION)                  \
      اذهب error;                                       \
  } طالما(0)

ساكن CURLcode doh_probe_لاڳيتو(بنية Curl_easy *معلومات,
                              DNStype dnstype,
                              ثابت حرف *host,
                              ثابت حرف *url, CURLM *multi,
                              uint32_t resolv_id,
                              uint32_t *pmid)
{
  بنية Curl_easy *doh = عدم;
  CURLcode نتيجو = CURLE_OK;
  timediff_t timeout_ms;
  بنية doh_request *doh_req;
  DOHcode d;
  bool maybe_https = !curl_strnequal(url, STRCONST("http:"));

  *pmid = UINT32_MAX;

  doh_req = curlx_calloc(1, حجم(*doh_req));
  إذا(!doh_req)
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  doh_req->resolv_id = resolv_id;
  doh_req->dnstype = dnstype;
  curlx_dyn_init(&doh_req->resp_body, DYN_DOH_RESPONSE);

  d = doh_req_encode(host, dnstype, doh_req->req_body,
                     حجم(doh_req->req_body),
                     &doh_req->req_body_len);
  إذا(d) {
    failf(معلومات, "Failed to encode DoH packet [%d]", (صحيح)d);
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب error;
  }

  timeout_ms = Curl_timeleft_ms(معلومات);
  إذا(timeout_ms < 0) {
    نتيجو = CURLE_OPERATION_TIMEDOUT;
    اذهب error;
  }

  doh_req->req_hds =
    curl_slist_append(عدم, "Content-Type: application/dns-message");
  إذا(!doh_req->req_hds) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب error;
  }

  /* Curl_open() is the internal version of curl_easy_init() */
  نتيجو = Curl_open(&doh);
  إذا(نتيجو)
    اذهب error;

  /* pass in the struct pointer via a local variable to please coverity and
     the gcc typecheck helpers */
  VERBOSE(doh->state.feat = &Curl_trc_feat_doh);
  ERROR_CHECK_SETOPT(CURLOPT_URL, url);
  ERROR_CHECK_SETOPT(CURLOPT_DEFAULT_PROTOCOL, "https");
  ERROR_CHECK_SETOPT(CURLOPT_WRITEFUNCTION, doh_probe_write_cb);
  ERROR_CHECK_SETOPT(CURLOPT_WRITEDATA, doh);
  ERROR_CHECK_SETOPT(CURLOPT_POSTFIELDS, doh_req->req_body);
  ERROR_CHECK_SETOPT(CURLOPT_POSTFIELDSIZE, (ڊگهو)doh_req->req_body_len);
  ERROR_CHECK_SETOPT(CURLOPT_HTTPHEADER, doh_req->req_hds);
#جيڪڏهن_وصف_ٿيل USE_HTTP2
  إذا(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_HTTP_نسخو, CURL_HTTP_نسخو_2_TLS);
    ERROR_CHECK_SETOPT(CURLOPT_PIPEWAIT, 1L);
  }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل DEBUGBUILD
  /* enforce HTTPS if not debug */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
#وإلا
  /* in debug mode, also allow http */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
#جيڪڏهن_پورو
  ERROR_CHECK_SETOPT(CURLOPT_TIMEOUT_MS, (ڊگهو)timeout_ms);
  ERROR_CHECK_SETOPT(CURLOPT_SHARE, (CURLSH *)معلومات->share);
  إذا(معلومات->سيٽ.err && معلومات->سيٽ.err != stderr)
    ERROR_CHECK_SETOPT(CURLOPT_STDERR, معلومات->سيٽ.err);
  إذا(Curl_trc_ft_is_verbose(معلومات, &Curl_trc_feat_doh))
    ERROR_CHECK_SETOPT(CURLOPT_VERBOSE, 1L);
  إذا(معلومات->سيٽ.no_signal)
    ERROR_CHECK_SETOPT(CURLOPT_NOSIGNAL, 1L);
  إذا(معلومات->سيٽ.fdebug)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGFUNCTION, معلومات->سيٽ.fdebug);
  إذا(معلومات->سيٽ.debugdata)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGDATA, معلومات->سيٽ.debugdata);

#جيڪڏهن_وصف_ٿيل USE_RESOLV_ARES
  إذا(CURL_EASY_STR(معلومات, STRING_DNS_SERVERS)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_SERVERS,
                       CURL_EASY_STR(معلومات, STRING_DNS_SERVERS));
  }
  إذا(CURL_EASY_STR(معلومات, STRING_DNS_INTERFACE)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_INTERFACE,
                       CURL_EASY_STR(معلومات, STRING_DNS_INTERFACE));
  }
  إذا(CURL_EASY_STR(معلومات, STRING_DNS_LOCAL_IP4)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP4,
                       CURL_EASY_STR(معلومات, STRING_DNS_LOCAL_IP4));
  }
  إذا(CURL_EASY_STR(معلومات, STRING_DNS_LOCAL_IP6)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP6,
                       CURL_EASY_STR(معلومات, STRING_DNS_LOCAL_IP6));
  }
#جيڪڏهن_پورو
  إذا(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYHOST,
                       معلومات->سيٽ.doh_verifyhost ? 2L : 0L);
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYPEER,
                       معلومات->سيٽ.doh_verifypeer ? 1L : 0L);
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYSTATUS,
                       معلومات->سيٽ.doh_verifystatus ? 1L : 0L);

    /* Inherit *some* SSL options from the user's transfer. This is a
       best-guess as to which options are needed for compatibility. #3661

       Note DoH does not inherit the user's proxy server so proxy SSL settings
       have no effect and are not inherited. If that changes then two new
       options should be added to check doh proxy insecure separately,
       CURLOPT_DOH_PROXY_SSL_VERIFYHOST and CURLOPT_DOH_PROXY_SSL_VERIFYPEER.
       */
    doh->سيٽ.ssl.custom_cafile = معلومات->سيٽ.ssl.custom_cafile;
    doh->سيٽ.ssl.custom_capath = معلومات->سيٽ.ssl.custom_capath;
    doh->سيٽ.ssl.custom_cablob = معلومات->سيٽ.ssl.custom_cablob;
    إذا(CURL_EASY_STR(معلومات, STRING_SSL_CAFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO,
                         CURL_EASY_STR(معلومات, STRING_SSL_CAFILE));
    }
    إذا(معلومات->سيٽ.blobs[BLOB_CAINFO]) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO_BLOB, معلومات->سيٽ.blobs[BLOB_CAINFO]);
    }
    إذا(CURL_EASY_STR(معلومات, STRING_SSL_CAPATH)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAPATH,
                         CURL_EASY_STR(معلومات, STRING_SSL_CAPATH));
    }
    إذا(CURL_EASY_STR(معلومات, STRING_SSL_CRLFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CRLFILE,
                         CURL_EASY_STR(معلومات, STRING_SSL_CRLFILE));
    }
    إذا(معلومات->سيٽ.ssl.certinfo)
      ERROR_CHECK_SETOPT(CURLOPT_CERTINFO, 1L);
    إذا(معلومات->سيٽ.ssl_fsslctx)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_FUNCTION, معلومات->سيٽ.ssl_fsslctx);
    إذا(معلومات->سيٽ.ssl_fsslctxp)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_معلومات, معلومات->سيٽ.ssl_fsslctxp);
    إذا(CURL_EASY_STR(معلومات, STRING_SSL_EC_CURVES)) {
      ERROR_CHECK_SETOPT(CURLOPT_SSL_EC_CURVES,
                         CURL_EASY_STR(معلومات, STRING_SSL_EC_CURVES));
    }
    (فراغ)curl_easy_setopt(doh, CURLOPT_SSL_OPTIONS,
                           ((ڊگهو)معلومات->سيٽ.ssl.ssl_options &
                            ~CURLSSLOPT_AUTO_CLIENT_CERT));
  }

  doh->state.اندروني = TRUE;
  doh->master_mid = معلومات->mid; /* master transfer of this one */
  doh->sub_xfer_پورو = doh_probe_پورو;

  نتيجو = Curl_meta_سيٽ(doh, CURL_EZM_DOH_PROBE, doh_req, doh_probe_dtor);
  doh_req = عدم; /* call took ownership */
  إذا(نتيجو)
    اذهب error;

  /* DoH handles must not inherit private_data. The handles may be passed to
     the user via callbacks and the user will be able to identify them as
     internal handles because private data is not set. The user can then set
     private_data via CURLOPT_PRIVATE if they so choose. */
  DEBUGASSERT(!doh->سيٽ.ذاتي_معلومات);

  إذا(Curl_multi_جوڙيو_handle(multi, doh))
    اذهب error;

  *pmid = doh->mid;
  أرجع CURLE_OK;

error:
  Curl_close(&doh);
  إذا(doh_req)
    doh_probe_dtor(عدم, 0, doh_req);
  أرجع نتيجو;
}

/*
 * Curl_doh() starts a name resolve using DoH. It resolves a name and returns
 * a 'Curl_addrinfo *' with the address information.
 */

CURLcode Curl_doh(بنية Curl_easy *معلومات,
                  بنية Curl_resolv_async *async)
{
  CURLcode نتيجو = CURLE_OK;
  بنية doh_probes *dohp = عدم;
  size_t i;

  DEBUGASSERT(!async->doh);
  DEBUGASSERT(async->peer->hostname[0]);
  إذا(async->doh) {
    DEBUGASSERT(0); /* should not happen */
    Curl_doh_cleanup(معلومات, async);
  }

  إذا(!async->dns_queries)
    أرجع CURLE_BAD_FUNCTION_دليل;
#جيڪڏهن_وصف_ٿيل USE_HTTPSRR
  إذا(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     (async->dns_queries & CURL_DNSQ_HTTPS)) {
    /* Can't mix those in the same async resolve */
    DEBUGASSERT(0);
    أرجع CURLE_BAD_FUNCTION_دليل;
  }
#وإلا
  إذا(async->dns_queries & CURL_DNSQ_HTTPS) {
    DEBUGASSERT(0);
    أرجع CURLE_NOT_BUILT_اندريون;
  }
#جيڪڏهن_پورو

  /* start clean, consider allocating this struct on demand */
  async->doh = dohp = curlx_calloc(1, حجم(بنية doh_probes));
  إذا(!dohp)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  لكل(i = 0; i < DOH_جڳهه_ڳڻپ; ++i) {
    dohp->probe_rc[i] = DOH_OK;
    dohp->probe_mid[i] = UINT32_MAX;
  }

#جيڪڏهن_وصف_ٿيل USE_IPV6
  /* AAAA results have preference in happy eyeballing, trigger first */
  إذا(async->dns_queries & CURL_DNSQ_AAAA) {
    /* create IPv6 DoH request */
    نتيجو = doh_probe_لاڳيتو(معلومات, CURL_DNS_TYPE_AAAA,
                           async->peer->hostname,
                           CURL_EASY_STR(معلومات, STRING_DOH),
                           معلومات->multi, async->id,
                           &dohp->probe_mid[DOH_جڳهه_IPV6]);
    إذا(نتيجو)
      اذهب error;
    async->queries_ongoing++;
  }
#جيڪڏهن_پورو

  /* create IPv4 DoH request */
  إذا(async->dns_queries & CURL_DNSQ_A) {
    نتيجو = doh_probe_لاڳيتو(معلومات, CURL_DNS_TYPE_A,
                           async->peer->hostname,
                           CURL_EASY_STR(معلومات, STRING_DOH),
                           معلومات->multi, async->id,
                           &dohp->probe_mid[DOH_جڳهه_IPV4]);
    إذا(نتيجو)
      اذهب error;
    async->queries_ongoing++;
  }

#جيڪڏهن_وصف_ٿيل USE_HTTPSRR
  إذا(async->dns_queries & CURL_DNSQ_HTTPS) {
    حرف *qname = عدم;
    إذا(async->peer->پورٽ != پورٽ_HTTPS) {
      qname = curl_maprintf("_%u._https.%s",
                            async->peer->پورٽ, async->peer->hostname);
      إذا(!qname)
        اذهب error;
    }
    نتيجو = doh_probe_لاڳيتو(معلومات, CURL_DNS_TYPE_HTTPS,
                           qname ? qname : async->peer->hostname,
                           CURL_EASY_STR(معلومات, STRING_DOH), معلومات->multi,
                           async->id,
                           &dohp->probe_mid[DOH_جڳهه_HTTPS_RR]);
    curlx_free(qname);
    إذا(نتيجو)
      اذهب error;
    async->queries_ongoing++;
  }
#جيڪڏهن_پورو
  أرجع CURLE_OK;

error:
  Curl_doh_cleanup(معلومات, async);
  أرجع نتيجو;
}

ساكن DOHcode doh_skipqname(ثابت بنانشان حرف *doh, size_t dohlen,
                             بنانشان صحيح *indexp)
{
  بنانشان حرف ڊيگهائي;
  افعل {
    إذا(dohlen < (*indexp + 1))
      أرجع DOH_DNS_ٻاهريون_جو_RANGE;
    ڊيگهائي = doh[*indexp];
    إذا((ڊيگهائي & 0xc0) == 0xc0) {
      /* name pointer, advance over it and be done */
      إذا(dohlen < (*indexp + 2))
        أرجع DOH_DNS_ٻاهريون_جو_RANGE;
      *indexp += 2;
      اكسر;
    }
    إذا(ڊيگهائي & 0xc0)
      أرجع DOH_DNS_BAD_LABEL;
    إذا(dohlen < (*indexp + 1 + ڊيگهائي))
      أرجع DOH_DNS_ٻاهريون_جو_RANGE;
    *indexp += (بنانشان صحيح)(1 + ڊيگهائي);
  } طالما(ڊيگهائي);
  أرجع DOH_OK;
}

ساكن بنانشان ننڍو doh_get16bit(ثابت بنانشان حرف *doh,
                                   بنانشان صحيح اشاريو)
{
  أرجع (بنانشان ننڍو)((doh[اشاريو] << 8) | doh[اشاريو + 1]);
}

ساكن بنانشان صحيح doh_get32bit(ثابت بنانشان حرف *doh, بنانشان صحيح اشاريو)
{
  /* make clang and gcc optimize this to bswap by incrementing
     the pointer first. */
  doh += اشاريو;

  /* avoid undefined behavior by casting to unsigned before shifting
     24 bits, possibly into the sign bit. codegen is same, but
     ub sanitizer will not be upset */
  أرجع ((بنانشان)doh[0] << 24) | ((بنانشان)doh[1] << 16) |
         ((بنانشان)doh[2] << 8) | doh[3];
}

ساكن فراغ doh_store_a(ثابت بنانشان حرف *doh, صحيح اشاريو,
                        بنية dohentry *d)
{
  /* silently ignore addresses over the limit */
  إذا(d->numaddr < DOH_MAX_ADDR) {
    بنية dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_A;
    يادگيري_نقل(&a->ip.v4, &doh[اشاريو], 4);
    d->numaddr++;
  }
}

ساكن فراغ doh_store_aaaa(ثابت بنانشان حرف *doh, صحيح اشاريو,
                           بنية dohentry *d)
{
  /* silently ignore addresses over the limit */
  إذا(d->numaddr < DOH_MAX_ADDR) {
    بنية dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_AAAA;
    يادگيري_نقل(&a->ip.v6, &doh[اشاريو], 16);
    d->numaddr++;
  }
}

#جيڪڏهن_وصف_ٿيل USE_HTTPSRR
ساكن DOHcode doh_store_https(ثابت بنانشان حرف *doh, صحيح اشاريو,
                               بنية dohentry *d, uint16_t len)
{
  /* silently ignore RRs over the limit */
  إذا(d->numhttps_rrs < DOH_MAX_HTTPS) {
    بنية dohhttps_rr *h = &d->https_rrs[d->numhttps_rrs];
    h->val = curlx_memdup(&doh[اشاريو], len);
    إذا(!h->val)
      أرجع DOH_ٻاهريون_جو_MEM;
    h->len = len;
    d->numhttps_rrs++;
  }
  أرجع DOH_OK;
}
#جيڪڏهن_پورو

ساكن DOHcode doh_rdata(ثابت بنانشان حرف *doh,
                         بنانشان ننڍو rdlength,
                         بنانشان ننڍو type,
                         صحيح اشاريو,
                         بنية dohentry *d)
{
  /* RDATA
     - A (TYPE 1): 4 bytes
     - AAAA (TYPE 28): 16 bytes
     - HTTPS (TYPE 65): N bytes */
  بدل(type) {
  حالة CURL_DNS_TYPE_A:
    إذا(rdlength != 4)
      أرجع DOH_DNS_RDATA_LEN;
    doh_store_a(doh, اشاريو, d);
    اكسر;
  حالة CURL_DNS_TYPE_AAAA:
    إذا(rdlength != 16)
      أرجع DOH_DNS_RDATA_LEN;
    doh_store_aaaa(doh, اشاريو, d);
    اكسر;
#جيڪڏهن_وصف_ٿيل USE_HTTPSRR
  حالة CURL_DNS_TYPE_HTTPS:
    إذا(rdlength < 3)
      أرجع DOH_DNS_RDATA_LEN;
    أرجع doh_store_https(doh, اشاريو, d, rdlength);
#جيڪڏهن_پورو
  افتراضي:
    /* unsupported type, or type we do not store, skip it */
    اكسر;
  }
  أرجع DOH_OK;
}

/* @unittest 1655 */
UNITTEST فراغ de_init(بنية dohentry *de);
UNITTEST فراغ de_init(بنية dohentry *de)
{
  memset(de, 0, حجم(*de));
  de->ttl = وڌ_کان_وڌ_عدد;
}

/* TTL value cap */
#وصف_ڪريو MAX_DNS_TTL 86400U /* 24 hours */
/* @unittest 1650 */
UNITTEST DOHcode doh_resp_decode(ثابت بنانشان حرف *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 بنية dohentry *d);
UNITTEST DOHcode doh_resp_decode(ثابت بنانشان حرف *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 بنية dohentry *d)
{
  بنانشان حرف rcode;
  بنانشان ننڍو qdcount;
  بنانشان ننڍو ancount;
  بنانشان ننڍو type = 0;
  بنانشان ننڍو rdlength;
  بنانشان ننڍو nscount;
  بنانشان ننڍو arcount;
  بنانشان صحيح اشاريو = 12;
  DOHcode rc;

  إذا(dohlen < 12)
    أرجع DOH_TOO_SMALL_بفر; /* too small */
  إذا(!doh || doh[0] || doh[1])
    أرجع DOH_DNS_BAD_ID; /* bad ID */
  rcode = doh[3] & 0x0f;
  إذا(rcode == 3)
    أرجع DOH_DNS_NXDOMAIN; /* name does not exist */
  إذا(rcode)
    أرجع DOH_DNS_BAD_RCODE; /* bad rcode */

  qdcount = doh_get16bit(doh, 4);
  طالما(qdcount) {
    rc = doh_skipqname(doh, dohlen, &اشاريو);
    إذا(rc)
      أرجع rc; /* bad qname */
    إذا(dohlen < (اشاريو + 4))
      أرجع DOH_DNS_ٻاهريون_جو_RANGE;
    اشاريو += 4; /* skip question's type and class */
    qdcount--;
  }

  ancount = doh_get16bit(doh, 6);
  طالما(ancount) {
    بنانشان ننڍو dnsclass;
    بنانشان صحيح ttl;

    rc = doh_skipqname(doh, dohlen, &اشاريو);
    إذا(rc)
      أرجع rc; /* bad qname */

    إذا(dohlen < (اشاريو + 2))
      أرجع DOH_DNS_ٻاهريون_جو_RANGE;

    type = doh_get16bit(doh, اشاريو);
    إذا((type != CURL_DNS_TYPE_CNAME) &&  /* may be synthesized from DNAME */
       (type != CURL_DNS_TYPE_DNAME) &&  /* if present, accept and ignore */
       (type != dnstype))
      /* Not the same type as was asked for, nor CNAME nor DNAME */
      أرجع DOH_DNS_UNEXPECTED_TYPE;
    اشاريو += 2;

    إذا(dohlen < (اشاريو + 2))
      أرجع DOH_DNS_ٻاهريون_جو_RANGE;
    dnsclass = doh_get16bit(doh, اشاريو);
    إذا(DNS_CLASS_اندريون != dnsclass)
      أرجع DOH_DNS_UNEXPECTED_CLASS; /* unsupported */
    اشاريو += 2;

    إذا(dohlen < (اشاريو + 4))
      أرجع DOH_DNS_ٻاهريون_جو_RANGE;

    ttl = doh_get32bit(doh, اشاريو);
    إذا(ttl > MAX_DNS_TTL)
      ttl = MAX_DNS_TTL;
    إذا(ttl < d->ttl)
      d->ttl = ttl;
    اشاريو += 4;

    إذا(dohlen < (اشاريو + 2))
      أرجع DOH_DNS_ٻاهريون_جو_RANGE;

    rdlength = doh_get16bit(doh, اشاريو);
    اشاريو += 2;
    إذا(dohlen < (اشاريو + rdlength))
      أرجع DOH_DNS_ٻاهريون_جو_RANGE;

    rc = doh_rdata(doh, rdlength, type, (صحيح)اشاريو, d);
    إذا(rc)
      أرجع rc;
    اشاريو += rdlength;
    ancount--;
  }

  nscount = doh_get16bit(doh, 8);
  طالما(nscount) {
    rc = doh_skipqname(doh, dohlen, &اشاريو);
    إذا(rc)
      أرجع rc; /* bad qname */

    إذا(dohlen < (اشاريو + 8))
      أرجع DOH_DNS_ٻاهريون_جو_RANGE;

    اشاريو += 2 + 2 + 4; /* type, dnsclass and ttl */

    إذا(dohlen < (اشاريو + 2))
      أرجع DOH_DNS_ٻاهريون_جو_RANGE;

    rdlength = doh_get16bit(doh, اشاريو);
    اشاريو += 2;
    إذا(dohlen < (اشاريو + rdlength))
      أرجع DOH_DNS_ٻاهريون_جو_RANGE;
    اشاريو += rdlength;
    nscount--;
  }

  arcount = doh_get16bit(doh, 10);
  طالما(arcount) {
    rc = doh_skipqname(doh, dohlen, &اشاريو);
    إذا(rc)
      أرجع rc; /* bad qname */

    إذا(dohlen < (اشاريو + 8))
      أرجع DOH_DNS_ٻاهريون_جو_RANGE;

    اشاريو += 2 + 2 + 4; /* type, dnsclass and ttl */

    إذا(dohlen < (اشاريو + 2))
      أرجع DOH_DNS_ٻاهريون_جو_RANGE;

    rdlength = doh_get16bit(doh, اشاريو);
    اشاريو += 2;
    إذا(dohlen < (اشاريو + rdlength))
      أرجع DOH_DNS_ٻاهريون_جو_RANGE;
    اشاريو += rdlength;
    arcount--;
  }

  إذا(اشاريو != dohlen)
    أرجع DOH_DNS_MALFORMAT; /* something is wrong */

  أرجع DOH_OK; /* ok */
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
ساكن CURLcode doh2ai(ثابت بنية dohentry *de, ثابت حرف *hostname,
                       صحيح پورٽ, بنية Curl_addrinfo **aip)
{
  بنية Curl_addrinfo *ai;
  بنية Curl_addrinfo *prevai = عدم;
  بنية Curl_addrinfo *firstai = عدم;
  بنية sockaddr_in *addr;
#جيڪڏهن_وصف_ٿيل USE_IPV6
  بنية sockaddr_in6 *addr6;
#جيڪڏهن_پورو
  size_t hostlen = لڙي_ڊيگهه(hostname) + 1; /* include null-terminator */
  CURLcode نتيجو = CURLE_OK;
  صحيح i;

  لكل(i = 0; i < de->numaddr; i++) {
    size_t ss_سائيز;
    CURL_SA_FAMILY_T addrtype;
    إذا(de->addr[i].type == CURL_DNS_TYPE_AAAA) {
#جيڪڏهن_وصف_نه_ٿيل USE_IPV6
      /* we cannot handle IPv6 addresses */
      استمر;
#وإلا
      ss_سائيز = حجم(بنية sockaddr_in6);
      addrtype = AF_INET6;
#جيڪڏهن_پورو
    }
    وإلا {
      ss_سائيز = حجم(بنية sockaddr_in);
      addrtype = AF_INET;
    }

    ai = curlx_calloc(1, حجم(بنية Curl_addrinfo) + ss_سائيز + hostlen);
    إذا(!ai) {
      نتيجو = CURLE_ٻاهريون_جو_يادگيري;
      اكسر;
    }
    ai->ai_addr = (فراغ *)((حرف *)ai + حجم(بنية Curl_addrinfo));
    ai->ai_canonname = (فراغ *)((حرف *)ai->ai_addr + ss_سائيز);
    يادگيري_نقل(ai->ai_canonname, hostname, hostlen);

    إذا(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    إذا(prevai)
      /* make the previous entry point to this */
      prevai->ai_ايندڙ = ai;

    ai->ai_family = addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_سائيز;

    /* leave the rest of the struct filled with zero */

    بدل(ai->ai_family) {
    حالة AF_INET:
      addr = (فراغ *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(حجم(بنية in_addr) == حجم(de->addr[i].ip.v4));
      يادگيري_نقل(&addr->sin_addr, &de->addr[i].ip.v4, حجم(بنية in_addr));
      addr->sin_family = addrtype;
      addr->sin_port = htons((بنانشان ننڍو)پورٽ);
      اكسر;

#جيڪڏهن_وصف_ٿيل USE_IPV6
    حالة AF_INET6:
      addr6 = (فراغ *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(حجم(بنية in6_addr) == حجم(de->addr[i].ip.v6));
      يادگيري_نقل(&addr6->sin6_addr, &de->addr[i].ip.v6, حجم(بنية in6_addr));
      addr6->sin6_family = addrtype;
      addr6->sin6_port = htons((بنانشان ننڍو)پورٽ);
      اكسر;
#جيڪڏهن_پورو
    }

    prevai = ai;
  }

  إذا(نتيجو) {
    Curl_freeaddrinfo(firstai);
    firstai = عدم;
  }
  *aip = firstai;

  أرجع نتيجو;
}

/* @unittest 1655 */
UNITTEST فراغ de_cleanup(بنية dohentry *d);
UNITTEST فراغ de_cleanup(بنية dohentry *d)
{
#جيڪڏهن_وصف_ٿيل USE_HTTPSRR
  صحيح i = 0;
  لكل(i = 0; i < d->numhttps_rrs; i++)
    curlx_safefree(d->https_rrs[i].val);
#وإلا
  (فراغ)d;
#جيڪڏهن_پورو
}

#جيڪڏهن_وصف_ٿيل USE_HTTPSRR

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
ساكن CURLcode doh_decode_rdata_name(ثابت بنانشان حرف **buf,
                                      size_t *بچيل, حرف **dnsname)
{
  ثابت بنانشان حرف *cp = عدم;
  size_t rem = 0;
  بنانشان حرف clen = 0; /* chunk len */
  بنية dynbuf thename;

  DEBUGASSERT(buf && بچيل && dnsname);
  إذا(!buf || !بچيل || !dnsname || !*بچيل)
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  curlx_dyn_init(&thename, CURL_MAXLEN_HOST_NAME);
  rem = *بچيل;
  cp = *buf;
  clen = *cp++;
  /* RFC 9460 says it must be uncompressed */
  إذا(clen > 63)
    أرجع CURLE_WEIRD_SERVER_جواب;

  إذا(clen == 0) {
    /* special case - return "." as name */
    إذا(curlx_dyn_addn(&thename, ".", 1))
      أرجع CURLE_ٻاهريون_جو_يادگيري;
  }
  طالما(clen) {
    إذا(clen >= rem) {
      curlx_dyn_free(&thename);
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    }
    إذا(curlx_dyn_addn(&thename, cp, clen) ||
       curlx_dyn_addn(&thename, ".", 1))
      أرجع CURLE_TOO_LARGE;

    cp += clen;
    rem -= (clen + 1);
    إذا(rem <= 0) {
      curlx_dyn_free(&thename);
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    }
    clen = *cp++;
    إذا(clen > 63) {
      /* invalid format */
      curlx_dyn_free(&thename);
      أرجع CURLE_WEIRD_SERVER_جواب;
    }
  }
  *buf = cp;
  *بچيل = rem - 1;
  *dnsname = curlx_dyn_ptr(&thename);
  أرجع CURLE_OK;
}

/* scan for byte values <= 32 or 127 */
ساكن CURLcode junkscan(ثابت حرف *url)
{
  ثابت بنانشان حرف *p = (ثابت بنانشان حرف *)url;
  طالما(*p) {
    إذا(*p <= 0x20 || *p == 127)
      أرجع CURLE_WEIRD_SERVER_جواب;
    p++;
  }
  أرجع CURLE_OK;
}

/* @unittest 1658 */
UNITTEST CURLcode doh_resp_decode_httpsrr(بنية Curl_easy *معلومات,
                                          ثابت بنانشان حرف *cp, size_t len,
                                          بنية Curl_https_rrinfo **hrr);
UNITTEST CURLcode doh_resp_decode_httpsrr(بنية Curl_easy *معلومات,
                                          ثابت بنانشان حرف *cp, size_t len,
                                          بنية Curl_https_rrinfo **hrr)
{
  uint16_t pcode = 0, plen = 0;
  uint32_t expected_min_pcode = 0;
  بنية Curl_https_rrinfo *lhrr = عدم;
  حرف *dnsname = عدم;
  CURLcode نتيجو = CURLE_ٻاهريون_جو_يادگيري;

  (فراغ)معلومات;
  *hrr = عدم;
  إذا(len <= 2)
    أرجع CURLE_BAD_FUNCTION_دليل;
  lhrr = curlx_calloc(1, حجم(بنية Curl_https_rrinfo));
  إذا(!lhrr)
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  lhrr->ترجيح = doh_get16bit(cp, 0);
  cp += 2;
  len -= 2;
  إذا(doh_decode_rdata_name(&cp, &len, &dnsname) != CURLE_OK)
    اذهب err;
  lhrr->هدف = dnsname;
  نتيجو = junkscan(dnsname);
  إذا(نتيجو) {
    /* unacceptable hostname content */
    اذهب err;
  }
  طالما(len >= 4) {
    pcode = doh_get16bit(cp, 0);
    plen = doh_get16bit(cp, 2);
    cp += 4;
    len -= 4;
    إذا(pcode < expected_min_pcode || plen > len) {
      نتيجو = CURLE_WEIRD_SERVER_جواب;
      اذهب err;
    }
    نتيجو = Curl_httpsrr_سيٽ(lhrr, pcode, cp, plen);
    إذا(نتيجو)
      اذهب err;
    Curl_httpsrr_trace(معلومات, lhrr);
    cp += plen;
    len -= plen;
    expected_min_pcode = pcode + 1;
  }
  *hrr = lhrr;
  أرجع CURLE_OK;
err:
  Curl_httpsrr_destroy(lhrr);
  أرجع نتيجو;
}

#جيڪڏهن_پورو /* USE_HTTPSRR */

/* called from multi when a sub transfer, e.g. doh probe, is done.
 * Parse the response and set the results in the `async` context
 * of master, using the id from the probe's CURL_EZM_DOH_PROBE
 * meta data. */
ساكن فراغ doh_probe_پورو(بنية Curl_easy *doh,
                           بنية Curl_easy *master, CURLcode نتيجو)
{
  بنية Curl_resolv_async *async = عدم;
  بنية doh_probes *dohp = عدم;
  بنية doh_request *doh_req = عدم;
  بنية Curl_addrinfo **pdest_ai;
  بنية dohentry de;
  صحيح جڳهه, httpcode;

  de_init(&de);
  doh_req = Curl_meta_get(doh, CURL_EZM_DOH_PROBE);
  إذا(!doh_req) {
    /* transfer `doh` is not a DoH probe. */
    DEBUGASSERT(0);
    اذهب ٻاهريون;
  }

  async = Curl_async_get(master, doh_req->resolv_id);
  إذا(!async) {
    CURL_TRC_DNS(master, "[%u] ignoring outdated DoH response",
                 doh_req->resolv_id);
    اذهب ٻاهريون;
  }
  dohp = async->doh;

  لكل(جڳهه = 0; جڳهه < DOH_جڳهه_ڳڻپ; ++جڳهه) {
    إذا(dohp->probe_mid[جڳهه] == doh->mid)
      اكسر;
  }
  /* We really should have found the slot where to store the response */
  إذا(جڳهه >= DOH_جڳهه_ڳڻپ) {
    failf(master, "DoH: unknown sub request done");
    DEBUGASSERT(0);
    اذهب ٻاهريون;
  }

  async->queries_ongoing--;
  dohp = async->doh;
  httpcode = doh->info.httpcode;
  بدل(جڳهه) {
  حالة DOH_جڳهه_IPV4:
    async->dns_responses |= CURL_DNSQ_A;
    اكسر;
#جيڪڏهن_وصف_ٿيل USE_IPV6
  حالة DOH_جڳهه_IPV6:
    async->dns_responses |= CURL_DNSQ_AAAA;
    اكسر;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_HTTPSRR
  حالة DOH_جڳهه_HTTPS_RR:
    async->dns_responses |= CURL_DNSQ_HTTPS;
    اكسر;
#جيڪڏهن_پورو
  افتراضي:
    DEBUGASSERT(0);
    اكسر;
  }

  إذا(نتيجو) {
    dohp->probe_rc[جڳهه] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: %s",
          doh_type2name(doh_req->dnstype), curl_easy_strerror(نتيجو));
    اذهب ٻاهريون;
  }
  وإلا إذا((httpcode < 200) || (httpcode >= 300)) {
    dohp->probe_rc[جڳهه] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: HTTP status %d",
          doh_type2name(doh_req->dnstype), httpcode);
    اذهب ٻاهريون;
  }

  dohp->probe_rc[جڳهه] = doh_resp_decode(curlx_dyn_uptr(&doh_req->resp_body),
                                         curlx_dyn_len(&doh_req->resp_body),
                                         doh_req->dnstype, &de);
  إذا(dohp->probe_rc[جڳهه]) {
#جيڪڏهن_وصف_ٿيل USE_HTTPSRR
    إذا((dohp->probe_rc[جڳهه] == DOH_NO_CONTENT) &&
       (doh_req->dnstype == CURL_DNS_TYPE_HTTPS)) {
      dohp->probe_rc[جڳهه] = DOH_DNS_NXDOMAIN;
    }
#جيڪڏهن_پورو
    infof(doh, "[DoH] [%s] error decoding response: %s",
          doh_type2name(doh_req->dnstype),
          doh_strerror(dohp->probe_rc[جڳهه]));
    اذهب ٻاهريون;
  }

  إذا(doh_req->dnstype == CURL_DNS_TYPE_A)
    pdest_ai = &async->ai_A;
  وإلا إذا(doh_req->dnstype == CURL_DNS_TYPE_AAAA)
    pdest_ai = &async->ai_AAAA;
  وإلا
    pdest_ai = عدم;

  إذا(pdest_ai && de.numaddr) {
    إذا(*pdest_ai) {
      Curl_freeaddrinfo(*pdest_ai);
      *pdest_ai = عدم;
    }
    نتيجو = doh2ai(&de, async->peer->hostname, async->peer->پورٽ, pdest_ai);
    إذا(نتيجو) { /* hard failure on our side, fail completely */
      infof(doh, "[DoH] [%s] error creating addrinfo: %s",
            doh_type2name(doh_req->dnstype), curl_easy_strerror(نتيجو));
      dohp->probe_rc[جڳهه] = DOH_OOM;
      async->نتيجو = نتيجو;
    }
  }
#جيڪڏهن_وصف_ٿيل USE_HTTPSRR
  وإلا إذا((doh_req->dnstype == CURL_DNS_TYPE_HTTPS) && de.numhttps_rrs) {
    CURL_TRC_DNS(doh, "[HTTPS] got %d records", de.numhttps_rrs);
    نتيجو = doh_resp_decode_httpsrr(doh, de.https_rrs->val,
                                     de.https_rrs->len, &async->httpsrr);
    إذا(نتيجو) {
      dohp->probe_rc[جڳهه] = DOH_HTTP_FAILED;
      infof(doh, "[DoH] error decoding HTTPS RR: %s",
            curl_easy_strerror(نتيجو));
      اذهب ٻاهريون;
    }
  }
#جيڪڏهن_پورو /* USE_HTTPSRR */

  /* DoH request complete, run master to act on results */
  infof(doh, "DoH request complete, %u to go", async->queries_ongoing);

ٻاهريون:
  Curl_multi_mark_dirty(master);
  de_cleanup(&de);
  Curl_meta_remove(doh, CURL_EZM_DOH_PROBE);
}

CURLcode Curl_doh_take_نتيجو(بنية Curl_easy *معلومات,
                              بنية Curl_resolv_async *async,
                              بنية Curl_dns_داخلا **pdns)
{
  بنية doh_probes *dohp = async->doh;
  CURLcode نتيجو = CURLE_OK;

  *pdns = عدم; /* defaults to no response */
  إذا(!dohp)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  async->negative_answer = FALSE;
  إذا(async->نتيجو) {
    نتيجو = async->نتيجو;
    اذهب ٻاهريون;
  }

  إذا(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     dohp->probe_mid[DOH_جڳهه_IPV4] == UINT32_MAX &&
     dohp->probe_mid[DOH_جڳهه_IPV6] == UINT32_MAX) {
    failf(معلومات, "Could not DoH-resolve: %s", async->peer->hostname);
    أرجع async->for_proxy ?
      CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
  }
  وإلا إذا(!async->queries_ongoing) {
    بنية Curl_dns_داخلا *dns = عدم;
    bool negative = TRUE;
    صحيح جڳهه;

    /* remove DoH handles from multi handle and close them */
    doh_close(معلومات, async);
    /* parse the responses, create the struct and return it! */
    لكل(جڳهه = 0; جڳهه < DOH_جڳهه_ڳڻپ; جڳهه++) {
      /* Failing without an NXDOMAIN answer - a SERVFAIL-class rcode or
         an undecodable response - says nothing about the name. Such a
         failure must not be cached as a negative entry. */
      إذا(dohp->probe_rc[جڳهه] && (dohp->probe_rc[جڳهه] != DOH_DNS_NXDOMAIN))
        negative = FALSE;
    } /* next slot */

    إذا(async->ai_A || async->ai_AAAA) {
      dns = Curl_dnsc_mk_addr2(
        معلومات, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
      إذا(!dns) {
        نتيجو = CURLE_ٻاهريون_جو_يادگيري;
        اذهب ٻاهريون;
      }
    }
#جيڪڏهن_وصف_ٿيل USE_HTTPSRR
    وإلا إذا((async->dns_queries & CURL_DNSQ_HTTPS) &&
            !dohp->probe_rc[DOH_جڳهه_HTTPS_RR]) {
      Curl_httpsrr_trace(معلومات, async->httpsrr);
      dns = Curl_dnsc_mk_https(معلومات, &async->httpsrr, async->peer);
      إذا(!dns) {
        نتيجو = CURLE_ٻاهريون_جو_يادگيري;
        اذهب ٻاهريون;
      }
    }
#جيڪڏهن_پورو /* USE_HTTPSRR */
    وإلا {
      /* every query failed. Only NXDOMAIN answers for all of them
         make this a negative answer, eligible for caching. */
      async->negative_answer = negative;
      نتيجو = async->for_proxy ?
        CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
    }

    /* and add the entry to the cache */
    إذا(dns)
      نتيجو = Curl_dnscache_جوڙيو(معلومات, dns);
    *pdns = dns;
  } /* !async->queries_ongoing */
  وإلا
    /* wait for pending DoH transactions to complete */
    أرجع CURLE_AGAIN;

ٻاهريون:
  Curl_doh_cleanup(معلومات, async);
  أرجع نتيجو;
}

ساكن فراغ doh_close(بنية Curl_easy *معلومات,
                      بنية Curl_resolv_async *async)
{
  بنية doh_probes *doh = async ? async->doh : عدم;
  إذا(doh && معلومات->multi) {
    بنية Curl_easy *probe_معلومات;
    uint32_t mid;
    size_t جڳهه;
    لكل(جڳهه = 0; جڳهه < DOH_جڳهه_ڳڻپ; جڳهه++) {
      mid = doh->probe_mid[جڳهه];
      إذا(mid == UINT32_MAX)
        استمر;
      doh->probe_mid[جڳهه] = UINT32_MAX;
      /* should have been called before data is removed from multi handle */
      DEBUGASSERT(معلومات->multi);
      probe_معلومات = معلومات->multi ? Curl_multi_get_easy(معلومات->multi, mid) : عدم;
      إذا(!probe_معلومات) {
        DEBUGF(infof(معلومات, "Curl_doh_close: xfer for mid=%u not found!", mid));
        استمر;
      }
      probe_معلومات->sub_xfer_پورو = عدم; /* No longer interested in result */
      /* data->multi might already be reset at this time */
      Curl_multi_remove_handle(معلومات->multi, probe_معلومات);
      Curl_close(&probe_معلومات);
    }
    CURL_TRC_DNS(معلومات, "[DoH] probe done");
  }
}

فراغ Curl_doh_cleanup(بنية Curl_easy *معلومات,
                      بنية Curl_resolv_async *async)
{
  بنية doh_probes *dohp = async->doh;
  إذا(dohp) {
    doh_close(معلومات, async);
    curlx_safefree(async->doh);
  }
}

#جيڪڏهن_پورو /* CURL_DISABLE_DOH */
