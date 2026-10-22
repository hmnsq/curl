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

#اگر_غیر_تعریف_شدہ CURL_DISABLE_DOH

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

#تعریف_کرو DNS_CLASS_اندر 0x01

ساکن خالی doh_close(ساخت Curl_easy *ڈیٹا,
                      ساخت Curl_resolv_async *async);

#اگر_تعریف_شدہ CURLVERBOSE
ساکن ثابت حرف * ثابت doh_رمز_str[] = {
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

ساکن ثابت حرف *doh_strerror(DOHcode رمز)
{
  اگر((size_t)رمز < CURL_ARRAYSIZE(doh_رمز_str))
    واپس doh_رمز_str[رمز];
  واپس "bad error code";
}

ساکن ثابت حرف *doh_type2name(DNStype dnstype)
{
  انتخاب(dnstype) {
  حالت CURL_DNS_TYPE_A:
    واپس "A";
  حالت CURL_DNS_TYPE_AAAA:
    واپس "AAAA";
#اگر_تعریف_شدہ USE_HTTPSRR
  حالت CURL_DNS_TYPE_HTTPS:
    واپس "HTTPS";
#اگر_ختم
  طےشدہ:
    واپس "unknown";
  }
}

#اگر_ختم /* CURLVERBOSE */

/* @unittest 1655
 */
UNITTEST DOHcode doh_req_encode(ثابت حرف *host,
                                DNStype dnstype,
                                غیرعلامتی حرف *dnsp,  /*بفر*/
                                size_t len,  /* buffer size */
                                size_t *olen);  /* output length */
UNITTEST DOHcode doh_req_encode(ثابت حرف *host,
                                DNStype dnstype,
                                غیرعلامتی حرف *dnsp, /*بفر*/
                                size_t len,   /* buffer size */
                                size_t *olen) /* output length */
{
  ثابت size_t hostlen = لڑی_لمبائی(host);
  غیرعلامتی حرف *orig = dnsp;
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
  اگر(host[hostlen - 1] != '.')
    expected_len++;

  اگر(expected_len > DOH_MAX_DNSREQ_سائز)
    واپس DOH_DNS_NAME_TOO_LONG;

  اگر(len < expected_len)
    واپس DOH_TOO_SMALL_بفر;

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
  جب(*hostp) {
    size_t labellen;
    ثابت حرف *dot = strchr(hostp, '.');
    اگر(dot)
      labellen = dot - hostp;
    ورنہ
      labellen = لڑی_لمبائی(hostp);
    اگر((labellen > 63) || (!labellen)) {
      /* label is too long or too short, error out */
      *olen = 0;
      واپس DOH_DNS_BAD_LABEL;
    }
    /* label is non-empty, process it */
    *dnsp++ = (غیرعلامتی حرف)labellen;
    حافظہ_نقل(dnsp, hostp, labellen);
    dnsp += labellen;
    hostp += labellen;
    /* advance past dot, but only if there is one */
    اگر(dot)
      hostp++;
  } /* next label */

  *dnsp++ = 0; /* append zero-length label for root */

  /* There are assigned TYPE codes beyond 255: use range [1..65535] */
  *dnsp++ = (غیرعلامتی حرف)(255 & (dnstype >> 8)); /* upper 8-bit TYPE */
  *dnsp++ = (غیرعلامتی حرف)(255 & dnstype);        /* lower 8-bit TYPE */

  *dnsp++ = '\0'; /* upper 8-bit CLASS */
  *dnsp++ = DNS_CLASS_اندر; /* IN - "the Internet" */

  *olen = dnsp - orig;

  /* verify that our estimation of length is valid, since
   * this has led to buffer overflows in this function */
  DEBUGASSERT(*olen == expected_len);
  واپس DOH_OK;
}

ساکن size_t doh_probe_write_cb(حرف *contents, size_t سائز, size_t nmemb,
                                 خالی *userp)
{
  size_t realsize = سائز * nmemb;
  ساخت Curl_easy *ڈیٹا = userp;
  ساخت doh_request *doh_req = Curl_meta_get(ڈیٹا, CURL_EZM_DOH_PROBE);
  اگر(!doh_req)
    واپس CURL_WRITEFUNC_ERROR;

  اگر(curlx_dyn_addn(&doh_req->resp_body, contents, realsize))
    واپس 0;

  واپس realsize;
}

ساکن خالی doh_probe_مکمل(ساخت Curl_easy *doh,
                           ساخت Curl_easy *master, CURLcode نتیجہ);
ساکن خالی doh_probe_dtor(ثابت خالی *کلید, size_t klen, خالی *e)
{
  (خالی)کلید;
  (خالی)klen;
  اگر(e) {
    ساخت doh_request *doh_req = e;
    curl_slist_free_all(doh_req->req_hds);
    curlx_dyn_free(&doh_req->resp_body);
    curlx_free(e);
  }
}

#تعریف_کرو ERROR_CHECK_SETOPT(x, y)                        \
  کرو {                                                  \
    نتیجہ = curl_easy_setopt((CURL *)doh, x, y);       \
    اگر(نتیجہ &&                                        \
       نتیجہ != CURLE_NOT_BUILT_اندر &&                  \
       نتیجہ != CURLE_UNKNOWN_OPTION)                  \
      جاؤ error;                                       \
  } جب(0)

ساکن CURLcode doh_probe_تسلسل(ساخت Curl_easy *ڈیٹا,
                              DNStype dnstype,
                              ثابت حرف *host,
                              ثابت حرف *url, CURLM *multi,
                              uint32_t resolv_id,
                              uint32_t *pmid)
{
  ساخت Curl_easy *doh = تہی;
  CURLcode نتیجہ = CURLE_OK;
  timediff_t timeout_ms;
  ساخت doh_request *doh_req;
  DOHcode d;
  bool maybe_https = !curl_strnequal(url, STRCONST("http:"));

  *pmid = UINT32_MAX;

  doh_req = curlx_calloc(1, حجم(*doh_req));
  اگر(!doh_req)
    واپس CURLE_برآمد_کا_حافظہ;
  doh_req->resolv_id = resolv_id;
  doh_req->dnstype = dnstype;
  curlx_dyn_init(&doh_req->resp_body, DYN_DOH_RESPONSE);

  d = doh_req_encode(host, dnstype, doh_req->req_body,
                     حجم(doh_req->req_body),
                     &doh_req->req_body_len);
  اگر(d) {
    failf(ڈیٹا, "Failed to encode DoH packet [%d]", (صحیح)d);
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ error;
  }

  timeout_ms = Curl_timeleft_ms(ڈیٹا);
  اگر(timeout_ms < 0) {
    نتیجہ = CURLE_OPERATION_TIMEDOUT;
    جاؤ error;
  }

  doh_req->req_hds =
    curl_slist_append(تہی, "Content-Type: application/dns-message");
  اگر(!doh_req->req_hds) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ error;
  }

  /* Curl_open() is the internal version of curl_easy_init() */
  نتیجہ = Curl_open(&doh);
  اگر(نتیجہ)
    جاؤ error;

  /* pass in the struct pointer via a local variable to please coverity and
     the gcc typecheck helpers */
  VERBOSE(doh->state.feat = &Curl_trc_feat_doh);
  ERROR_CHECK_SETOPT(CURLOPT_URL, url);
  ERROR_CHECK_SETOPT(CURLOPT_DEFAULT_PROTOCOL, "https");
  ERROR_CHECK_SETOPT(CURLOPT_WRITEFUNCTION, doh_probe_write_cb);
  ERROR_CHECK_SETOPT(CURLOPT_WRITEDATA, doh);
  ERROR_CHECK_SETOPT(CURLOPT_POSTFIELDS, doh_req->req_body);
  ERROR_CHECK_SETOPT(CURLOPT_POSTFIELDSIZE, (طویل)doh_req->req_body_len);
  ERROR_CHECK_SETOPT(CURLOPT_HTTPHEADER, doh_req->req_hds);
#اگر_تعریف_شدہ USE_HTTP2
  اگر(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_HTTP_نسخہ, CURL_HTTP_نسخہ_2_TLS);
    ERROR_CHECK_SETOPT(CURLOPT_PIPEWAIT, 1L);
  }
#اگر_ختم
#اگر_غیر_تعریف_شدہ DEBUGBUILD
  /* enforce HTTPS if not debug */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
#ورنہ
  /* in debug mode, also allow http */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
#اگر_ختم
  ERROR_CHECK_SETOPT(CURLOPT_TIMEOUT_MS, (طویل)timeout_ms);
  ERROR_CHECK_SETOPT(CURLOPT_SHARE, (CURLSH *)ڈیٹا->share);
  اگر(ڈیٹا->سیٹ.err && ڈیٹا->سیٹ.err != stderr)
    ERROR_CHECK_SETOPT(CURLOPT_STDERR, ڈیٹا->سیٹ.err);
  اگر(Curl_trc_ft_is_verbose(ڈیٹا, &Curl_trc_feat_doh))
    ERROR_CHECK_SETOPT(CURLOPT_VERBOSE, 1L);
  اگر(ڈیٹا->سیٹ.no_signal)
    ERROR_CHECK_SETOPT(CURLOPT_NOSIGNAL, 1L);
  اگر(ڈیٹا->سیٹ.fdebug)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGFUNCTION, ڈیٹا->سیٹ.fdebug);
  اگر(ڈیٹا->سیٹ.debugdata)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGDATA, ڈیٹا->سیٹ.debugdata);

#اگر_تعریف_شدہ USE_RESOLV_ARES
  اگر(CURL_EASY_STR(ڈیٹا, STRING_DNS_SERVERS)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_SERVERS,
                       CURL_EASY_STR(ڈیٹا, STRING_DNS_SERVERS));
  }
  اگر(CURL_EASY_STR(ڈیٹا, STRING_DNS_INTERFACE)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_INTERFACE,
                       CURL_EASY_STR(ڈیٹا, STRING_DNS_INTERFACE));
  }
  اگر(CURL_EASY_STR(ڈیٹا, STRING_DNS_LOCAL_IP4)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP4,
                       CURL_EASY_STR(ڈیٹا, STRING_DNS_LOCAL_IP4));
  }
  اگر(CURL_EASY_STR(ڈیٹا, STRING_DNS_LOCAL_IP6)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP6,
                       CURL_EASY_STR(ڈیٹا, STRING_DNS_LOCAL_IP6));
  }
#اگر_ختم
  اگر(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYHOST,
                       ڈیٹا->سیٹ.doh_verifyhost ? 2L : 0L);
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYPEER,
                       ڈیٹا->سیٹ.doh_verifypeer ? 1L : 0L);
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYSTATUS,
                       ڈیٹا->سیٹ.doh_verifystatus ? 1L : 0L);

    /* Inherit *some* SSL options from the user's transfer. This is a
       best-guess as to which options are needed for compatibility. #3661

       Note DoH does not inherit the user's proxy server so proxy SSL settings
       have no effect and are not inherited. If that changes then two new
       options should be added to check doh proxy insecure separately,
       CURLOPT_DOH_PROXY_SSL_VERIFYHOST and CURLOPT_DOH_PROXY_SSL_VERIFYPEER.
       */
    doh->سیٹ.ssl.custom_cafile = ڈیٹا->سیٹ.ssl.custom_cafile;
    doh->سیٹ.ssl.custom_capath = ڈیٹا->سیٹ.ssl.custom_capath;
    doh->سیٹ.ssl.custom_cablob = ڈیٹا->سیٹ.ssl.custom_cablob;
    اگر(CURL_EASY_STR(ڈیٹا, STRING_SSL_CAFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO,
                         CURL_EASY_STR(ڈیٹا, STRING_SSL_CAFILE));
    }
    اگر(ڈیٹا->سیٹ.blobs[BLOB_CAINFO]) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO_BLOB, ڈیٹا->سیٹ.blobs[BLOB_CAINFO]);
    }
    اگر(CURL_EASY_STR(ڈیٹا, STRING_SSL_CAPATH)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAPATH,
                         CURL_EASY_STR(ڈیٹا, STRING_SSL_CAPATH));
    }
    اگر(CURL_EASY_STR(ڈیٹا, STRING_SSL_CRLFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CRLFILE,
                         CURL_EASY_STR(ڈیٹا, STRING_SSL_CRLFILE));
    }
    اگر(ڈیٹا->سیٹ.ssl.certinfo)
      ERROR_CHECK_SETOPT(CURLOPT_CERTINFO, 1L);
    اگر(ڈیٹا->سیٹ.ssl_fsslctx)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_FUNCTION, ڈیٹا->سیٹ.ssl_fsslctx);
    اگر(ڈیٹا->سیٹ.ssl_fsslctxp)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_ڈیٹا, ڈیٹا->سیٹ.ssl_fsslctxp);
    اگر(CURL_EASY_STR(ڈیٹا, STRING_SSL_EC_CURVES)) {
      ERROR_CHECK_SETOPT(CURLOPT_SSL_EC_CURVES,
                         CURL_EASY_STR(ڈیٹا, STRING_SSL_EC_CURVES));
    }
    (خالی)curl_easy_setopt(doh, CURLOPT_SSL_OPTIONS,
                           ((طویل)ڈیٹا->سیٹ.ssl.ssl_options &
                            ~CURLSSLOPT_AUTO_CLIENT_CERT));
  }

  doh->state.اندرونی_2 = TRUE;
  doh->master_mid = ڈیٹا->mid; /* master transfer of this one */
  doh->sub_xfer_مکمل = doh_probe_مکمل;

  نتیجہ = Curl_meta_سیٹ(doh, CURL_EZM_DOH_PROBE, doh_req, doh_probe_dtor);
  doh_req = تہی; /* call took ownership */
  اگر(نتیجہ)
    جاؤ error;

  /* DoH handles must not inherit private_data. The handles may be passed to
     the user via callbacks and the user will be able to identify them as
     internal handles because private data is not set. The user can then set
     private_data via CURLOPT_PRIVATE if they so choose. */
  DEBUGASSERT(!doh->سیٹ.نجی_ڈیٹا);

  اگر(Curl_multi_جوڑو_handle(multi, doh))
    جاؤ error;

  *pmid = doh->mid;
  واپس CURLE_OK;

error:
  Curl_close(&doh);
  اگر(doh_req)
    doh_probe_dtor(تہی, 0, doh_req);
  واپس نتیجہ;
}

/*
 * Curl_doh() starts a name resolve using DoH. It resolves a name and returns
 * a 'Curl_addrinfo *' with the address information.
 */

CURLcode Curl_doh(ساخت Curl_easy *ڈیٹا,
                  ساخت Curl_resolv_async *async)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت doh_probes *dohp = تہی;
  size_t i;

  DEBUGASSERT(!async->doh);
  DEBUGASSERT(async->peer->hostname[0]);
  اگر(async->doh) {
    DEBUGASSERT(0); /* should not happen */
    Curl_doh_cleanup(ڈیٹا, async);
  }

  اگر(!async->dns_queries)
    واپس CURLE_BAD_FUNCTION_دلیل;
#اگر_تعریف_شدہ USE_HTTPSRR
  اگر(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     (async->dns_queries & CURL_DNSQ_HTTPS)) {
    /* Can't mix those in the same async resolve */
    DEBUGASSERT(0);
    واپس CURLE_BAD_FUNCTION_دلیل;
  }
#ورنہ
  اگر(async->dns_queries & CURL_DNSQ_HTTPS) {
    DEBUGASSERT(0);
    واپس CURLE_NOT_BUILT_اندر;
  }
#اگر_ختم

  /* start clean, consider allocating this struct on demand */
  async->doh = dohp = curlx_calloc(1, حجم(ساخت doh_probes));
  اگر(!dohp)
    واپس CURLE_برآمد_کا_حافظہ;

  برائے(i = 0; i < DOH_خانہ_گنتی; ++i) {
    dohp->probe_rc[i] = DOH_OK;
    dohp->probe_mid[i] = UINT32_MAX;
  }

#اگر_تعریف_شدہ USE_IPV6
  /* AAAA results have preference in happy eyeballing, trigger first */
  اگر(async->dns_queries & CURL_DNSQ_AAAA) {
    /* create IPv6 DoH request */
    نتیجہ = doh_probe_تسلسل(ڈیٹا, CURL_DNS_TYPE_AAAA,
                           async->peer->hostname,
                           CURL_EASY_STR(ڈیٹا, STRING_DOH),
                           ڈیٹا->multi, async->id,
                           &dohp->probe_mid[DOH_خانہ_IPV6]);
    اگر(نتیجہ)
      جاؤ error;
    async->queries_ongoing++;
  }
#اگر_ختم

  /* create IPv4 DoH request */
  اگر(async->dns_queries & CURL_DNSQ_A) {
    نتیجہ = doh_probe_تسلسل(ڈیٹا, CURL_DNS_TYPE_A,
                           async->peer->hostname,
                           CURL_EASY_STR(ڈیٹا, STRING_DOH),
                           ڈیٹا->multi, async->id,
                           &dohp->probe_mid[DOH_خانہ_IPV4]);
    اگر(نتیجہ)
      جاؤ error;
    async->queries_ongoing++;
  }

#اگر_تعریف_شدہ USE_HTTPSRR
  اگر(async->dns_queries & CURL_DNSQ_HTTPS) {
    حرف *qname = تہی;
    اگر(async->peer->پورٹ != پورٹ_HTTPS) {
      qname = curl_maprintf("_%u._https.%s",
                            async->peer->پورٹ, async->peer->hostname);
      اگر(!qname)
        جاؤ error;
    }
    نتیجہ = doh_probe_تسلسل(ڈیٹا, CURL_DNS_TYPE_HTTPS,
                           qname ? qname : async->peer->hostname,
                           CURL_EASY_STR(ڈیٹا, STRING_DOH), ڈیٹا->multi,
                           async->id,
                           &dohp->probe_mid[DOH_خانہ_HTTPS_RR]);
    curlx_free(qname);
    اگر(نتیجہ)
      جاؤ error;
    async->queries_ongoing++;
  }
#اگر_ختم
  واپس CURLE_OK;

error:
  Curl_doh_cleanup(ڈیٹا, async);
  واپس نتیجہ;
}

ساکن DOHcode doh_skipqname(ثابت غیرعلامتی حرف *doh, size_t dohlen,
                             غیرعلامتی صحیح *indexp)
{
  غیرعلامتی حرف لمبائی;
  کرو {
    اگر(dohlen < (*indexp + 1))
      واپس DOH_DNS_برآمد_کا_RANGE;
    لمبائی = doh[*indexp];
    اگر((لمبائی & 0xc0) == 0xc0) {
      /* name pointer, advance over it and be done */
      اگر(dohlen < (*indexp + 2))
        واپس DOH_DNS_برآمد_کا_RANGE;
      *indexp += 2;
      توڑو;
    }
    اگر(لمبائی & 0xc0)
      واپس DOH_DNS_BAD_LABEL;
    اگر(dohlen < (*indexp + 1 + لمبائی))
      واپس DOH_DNS_برآمد_کا_RANGE;
    *indexp += (غیرعلامتی صحیح)(1 + لمبائی);
  } جب(لمبائی);
  واپس DOH_OK;
}

ساکن غیرعلامتی مختصر doh_get16bit(ثابت غیرعلامتی حرف *doh,
                                   غیرعلامتی صحیح اشاریہ)
{
  واپس (غیرعلامتی مختصر)((doh[اشاریہ] << 8) | doh[اشاریہ + 1]);
}

ساکن غیرعلامتی صحیح doh_get32bit(ثابت غیرعلامتی حرف *doh, غیرعلامتی صحیح اشاریہ)
{
  /* make clang and gcc optimize this to bswap by incrementing
     the pointer first. */
  doh += اشاریہ;

  /* avoid undefined behavior by casting to unsigned before shifting
     24 bits, possibly into the sign bit. codegen is same, but
     ub sanitizer will not be upset */
  واپس ((غیرعلامتی)doh[0] << 24) | ((غیرعلامتی)doh[1] << 16) |
         ((غیرعلامتی)doh[2] << 8) | doh[3];
}

ساکن خالی doh_store_a(ثابت غیرعلامتی حرف *doh, صحیح اشاریہ,
                        ساخت dohentry *d)
{
  /* silently ignore addresses over the limit */
  اگر(d->numaddr < DOH_MAX_ADDR) {
    ساخت dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_A;
    حافظہ_نقل(&a->ip.v4, &doh[اشاریہ], 4);
    d->numaddr++;
  }
}

ساکن خالی doh_store_aaaa(ثابت غیرعلامتی حرف *doh, صحیح اشاریہ,
                           ساخت dohentry *d)
{
  /* silently ignore addresses over the limit */
  اگر(d->numaddr < DOH_MAX_ADDR) {
    ساخت dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_AAAA;
    حافظہ_نقل(&a->ip.v6, &doh[اشاریہ], 16);
    d->numaddr++;
  }
}

#اگر_تعریف_شدہ USE_HTTPSRR
ساکن DOHcode doh_store_https(ثابت غیرعلامتی حرف *doh, صحیح اشاریہ,
                               ساخت dohentry *d, uint16_t len)
{
  /* silently ignore RRs over the limit */
  اگر(d->numhttps_rrs < DOH_MAX_HTTPS) {
    ساخت dohhttps_rr *h = &d->https_rrs[d->numhttps_rrs];
    h->val = curlx_memdup(&doh[اشاریہ], len);
    اگر(!h->val)
      واپس DOH_برآمد_کا_MEM;
    h->len = len;
    d->numhttps_rrs++;
  }
  واپس DOH_OK;
}
#اگر_ختم

ساکن DOHcode doh_rdata(ثابت غیرعلامتی حرف *doh,
                         غیرعلامتی مختصر rdlength,
                         غیرعلامتی مختصر type,
                         صحیح اشاریہ,
                         ساخت dohentry *d)
{
  /* RDATA
     - A (TYPE 1): 4 bytes
     - AAAA (TYPE 28): 16 bytes
     - HTTPS (TYPE 65): N bytes */
  انتخاب(type) {
  حالت CURL_DNS_TYPE_A:
    اگر(rdlength != 4)
      واپس DOH_DNS_RDATA_LEN;
    doh_store_a(doh, اشاریہ, d);
    توڑو;
  حالت CURL_DNS_TYPE_AAAA:
    اگر(rdlength != 16)
      واپس DOH_DNS_RDATA_LEN;
    doh_store_aaaa(doh, اشاریہ, d);
    توڑو;
#اگر_تعریف_شدہ USE_HTTPSRR
  حالت CURL_DNS_TYPE_HTTPS:
    اگر(rdlength < 3)
      واپس DOH_DNS_RDATA_LEN;
    واپس doh_store_https(doh, اشاریہ, d, rdlength);
#اگر_ختم
  طےشدہ:
    /* unsupported type, or type we do not store, skip it */
    توڑو;
  }
  واپس DOH_OK;
}

/* @unittest 1655 */
UNITTEST خالی de_init(ساخت dohentry *de);
UNITTEST خالی de_init(ساخت dohentry *de)
{
  memset(de, 0, حجم(*de));
  de->ttl = زیادہ_سے_زیادہ_عدد;
}

/* TTL value cap */
#تعریف_کرو MAX_DNS_TTL 86400U /* 24 hours */
/* @unittest 1650 */
UNITTEST DOHcode doh_resp_decode(ثابت غیرعلامتی حرف *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 ساخت dohentry *d);
UNITTEST DOHcode doh_resp_decode(ثابت غیرعلامتی حرف *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 ساخت dohentry *d)
{
  غیرعلامتی حرف rcode;
  غیرعلامتی مختصر qdcount;
  غیرعلامتی مختصر ancount;
  غیرعلامتی مختصر type = 0;
  غیرعلامتی مختصر rdlength;
  غیرعلامتی مختصر nscount;
  غیرعلامتی مختصر arcount;
  غیرعلامتی صحیح اشاریہ = 12;
  DOHcode rc;

  اگر(dohlen < 12)
    واپس DOH_TOO_SMALL_بفر; /* too small */
  اگر(!doh || doh[0] || doh[1])
    واپس DOH_DNS_BAD_ID; /* bad ID */
  rcode = doh[3] & 0x0f;
  اگر(rcode == 3)
    واپس DOH_DNS_NXDOMAIN; /* name does not exist */
  اگر(rcode)
    واپس DOH_DNS_BAD_RCODE; /* bad rcode */

  qdcount = doh_get16bit(doh, 4);
  جب(qdcount) {
    rc = doh_skipqname(doh, dohlen, &اشاریہ);
    اگر(rc)
      واپس rc; /* bad qname */
    اگر(dohlen < (اشاریہ + 4))
      واپس DOH_DNS_برآمد_کا_RANGE;
    اشاریہ += 4; /* skip question's type and class */
    qdcount--;
  }

  ancount = doh_get16bit(doh, 6);
  جب(ancount) {
    غیرعلامتی مختصر dnsclass;
    غیرعلامتی صحیح ttl;

    rc = doh_skipqname(doh, dohlen, &اشاریہ);
    اگر(rc)
      واپس rc; /* bad qname */

    اگر(dohlen < (اشاریہ + 2))
      واپس DOH_DNS_برآمد_کا_RANGE;

    type = doh_get16bit(doh, اشاریہ);
    اگر((type != CURL_DNS_TYPE_CNAME) &&  /* may be synthesized from DNAME */
       (type != CURL_DNS_TYPE_DNAME) &&  /* if present, accept and ignore */
       (type != dnstype))
      /* Not the same type as was asked for, nor CNAME nor DNAME */
      واپس DOH_DNS_UNEXPECTED_TYPE;
    اشاریہ += 2;

    اگر(dohlen < (اشاریہ + 2))
      واپس DOH_DNS_برآمد_کا_RANGE;
    dnsclass = doh_get16bit(doh, اشاریہ);
    اگر(DNS_CLASS_اندر != dnsclass)
      واپس DOH_DNS_UNEXPECTED_CLASS; /* unsupported */
    اشاریہ += 2;

    اگر(dohlen < (اشاریہ + 4))
      واپس DOH_DNS_برآمد_کا_RANGE;

    ttl = doh_get32bit(doh, اشاریہ);
    اگر(ttl > MAX_DNS_TTL)
      ttl = MAX_DNS_TTL;
    اگر(ttl < d->ttl)
      d->ttl = ttl;
    اشاریہ += 4;

    اگر(dohlen < (اشاریہ + 2))
      واپس DOH_DNS_برآمد_کا_RANGE;

    rdlength = doh_get16bit(doh, اشاریہ);
    اشاریہ += 2;
    اگر(dohlen < (اشاریہ + rdlength))
      واپس DOH_DNS_برآمد_کا_RANGE;

    rc = doh_rdata(doh, rdlength, type, (صحیح)اشاریہ, d);
    اگر(rc)
      واپس rc;
    اشاریہ += rdlength;
    ancount--;
  }

  nscount = doh_get16bit(doh, 8);
  جب(nscount) {
    rc = doh_skipqname(doh, dohlen, &اشاریہ);
    اگر(rc)
      واپس rc; /* bad qname */

    اگر(dohlen < (اشاریہ + 8))
      واپس DOH_DNS_برآمد_کا_RANGE;

    اشاریہ += 2 + 2 + 4; /* type, dnsclass and ttl */

    اگر(dohlen < (اشاریہ + 2))
      واپس DOH_DNS_برآمد_کا_RANGE;

    rdlength = doh_get16bit(doh, اشاریہ);
    اشاریہ += 2;
    اگر(dohlen < (اشاریہ + rdlength))
      واپس DOH_DNS_برآمد_کا_RANGE;
    اشاریہ += rdlength;
    nscount--;
  }

  arcount = doh_get16bit(doh, 10);
  جب(arcount) {
    rc = doh_skipqname(doh, dohlen, &اشاریہ);
    اگر(rc)
      واپس rc; /* bad qname */

    اگر(dohlen < (اشاریہ + 8))
      واپس DOH_DNS_برآمد_کا_RANGE;

    اشاریہ += 2 + 2 + 4; /* type, dnsclass and ttl */

    اگر(dohlen < (اشاریہ + 2))
      واپس DOH_DNS_برآمد_کا_RANGE;

    rdlength = doh_get16bit(doh, اشاریہ);
    اشاریہ += 2;
    اگر(dohlen < (اشاریہ + rdlength))
      واپس DOH_DNS_برآمد_کا_RANGE;
    اشاریہ += rdlength;
    arcount--;
  }

  اگر(اشاریہ != dohlen)
    واپس DOH_DNS_MALFORMAT; /* something is wrong */

  واپس DOH_OK; /* ok */
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
ساکن CURLcode doh2ai(ثابت ساخت dohentry *de, ثابت حرف *hostname,
                       صحیح پورٹ, ساخت Curl_addrinfo **aip)
{
  ساخت Curl_addrinfo *ai;
  ساخت Curl_addrinfo *prevai = تہی;
  ساخت Curl_addrinfo *firstai = تہی;
  ساخت sockaddr_in *addr;
#اگر_تعریف_شدہ USE_IPV6
  ساخت sockaddr_in6 *addr6;
#اگر_ختم
  size_t hostlen = لڑی_لمبائی(hostname) + 1; /* include null-terminator */
  CURLcode نتیجہ = CURLE_OK;
  صحیح i;

  برائے(i = 0; i < de->numaddr; i++) {
    size_t ss_سائز;
    CURL_SA_FAMILY_T addrtype;
    اگر(de->addr[i].type == CURL_DNS_TYPE_AAAA) {
#اگر_غیر_تعریف_شدہ USE_IPV6
      /* we cannot handle IPv6 addresses */
      جاری;
#ورنہ
      ss_سائز = حجم(ساخت sockaddr_in6);
      addrtype = AF_INET6;
#اگر_ختم
    }
    ورنہ {
      ss_سائز = حجم(ساخت sockaddr_in);
      addrtype = AF_INET;
    }

    ai = curlx_calloc(1, حجم(ساخت Curl_addrinfo) + ss_سائز + hostlen);
    اگر(!ai) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      توڑو;
    }
    ai->ai_addr = (خالی *)((حرف *)ai + حجم(ساخت Curl_addrinfo));
    ai->ai_canonname = (خالی *)((حرف *)ai->ai_addr + ss_سائز);
    حافظہ_نقل(ai->ai_canonname, hostname, hostlen);

    اگر(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    اگر(prevai)
      /* make the previous entry point to this */
      prevai->ai_اگلا = ai;

    ai->ai_family = addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_سائز;

    /* leave the rest of the struct filled with zero */

    انتخاب(ai->ai_family) {
    حالت AF_INET:
      addr = (خالی *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(حجم(ساخت in_addr) == حجم(de->addr[i].ip.v4));
      حافظہ_نقل(&addr->sin_addr, &de->addr[i].ip.v4, حجم(ساخت in_addr));
      addr->sin_family = addrtype;
      addr->sin_port = htons((غیرعلامتی مختصر)پورٹ);
      توڑو;

#اگر_تعریف_شدہ USE_IPV6
    حالت AF_INET6:
      addr6 = (خالی *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(حجم(ساخت in6_addr) == حجم(de->addr[i].ip.v6));
      حافظہ_نقل(&addr6->sin6_addr, &de->addr[i].ip.v6, حجم(ساخت in6_addr));
      addr6->sin6_family = addrtype;
      addr6->sin6_port = htons((غیرعلامتی مختصر)پورٹ);
      توڑو;
#اگر_ختم
    }

    prevai = ai;
  }

  اگر(نتیجہ) {
    Curl_freeaddrinfo(firstai);
    firstai = تہی;
  }
  *aip = firstai;

  واپس نتیجہ;
}

/* @unittest 1655 */
UNITTEST خالی de_cleanup(ساخت dohentry *d);
UNITTEST خالی de_cleanup(ساخت dohentry *d)
{
#اگر_تعریف_شدہ USE_HTTPSRR
  صحیح i = 0;
  برائے(i = 0; i < d->numhttps_rrs; i++)
    curlx_safefree(d->https_rrs[i].val);
#ورنہ
  (خالی)d;
#اگر_ختم
}

#اگر_تعریف_شدہ USE_HTTPSRR

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
ساکن CURLcode doh_decode_rdata_name(ثابت غیرعلامتی حرف **buf,
                                      size_t *بقایا, حرف **dnsname)
{
  ثابت غیرعلامتی حرف *cp = تہی;
  size_t rem = 0;
  غیرعلامتی حرف clen = 0; /* chunk len */
  ساخت dynbuf thename;

  DEBUGASSERT(buf && بقایا && dnsname);
  اگر(!buf || !بقایا || !dnsname || !*بقایا)
    واپس CURLE_برآمد_کا_حافظہ;
  curlx_dyn_init(&thename, CURL_MAXLEN_HOST_NAME);
  rem = *بقایا;
  cp = *buf;
  clen = *cp++;
  /* RFC 9460 says it must be uncompressed */
  اگر(clen > 63)
    واپس CURLE_WEIRD_SERVER_جواب;

  اگر(clen == 0) {
    /* special case - return "." as name */
    اگر(curlx_dyn_addn(&thename, ".", 1))
      واپس CURLE_برآمد_کا_حافظہ;
  }
  جب(clen) {
    اگر(clen >= rem) {
      curlx_dyn_free(&thename);
      واپس CURLE_برآمد_کا_حافظہ;
    }
    اگر(curlx_dyn_addn(&thename, cp, clen) ||
       curlx_dyn_addn(&thename, ".", 1))
      واپس CURLE_TOO_LARGE;

    cp += clen;
    rem -= (clen + 1);
    اگر(rem <= 0) {
      curlx_dyn_free(&thename);
      واپس CURLE_برآمد_کا_حافظہ;
    }
    clen = *cp++;
    اگر(clen > 63) {
      /* invalid format */
      curlx_dyn_free(&thename);
      واپس CURLE_WEIRD_SERVER_جواب;
    }
  }
  *buf = cp;
  *بقایا = rem - 1;
  *dnsname = curlx_dyn_ptr(&thename);
  واپس CURLE_OK;
}

/* scan for byte values <= 32 or 127 */
ساکن CURLcode junkscan(ثابت حرف *url)
{
  ثابت غیرعلامتی حرف *p = (ثابت غیرعلامتی حرف *)url;
  جب(*p) {
    اگر(*p <= 0x20 || *p == 127)
      واپس CURLE_WEIRD_SERVER_جواب;
    p++;
  }
  واپس CURLE_OK;
}

/* @unittest 1658 */
UNITTEST CURLcode doh_resp_decode_httpsrr(ساخت Curl_easy *ڈیٹا,
                                          ثابت غیرعلامتی حرف *cp, size_t len,
                                          ساخت Curl_https_rrinfo **hrr);
UNITTEST CURLcode doh_resp_decode_httpsrr(ساخت Curl_easy *ڈیٹا,
                                          ثابت غیرعلامتی حرف *cp, size_t len,
                                          ساخت Curl_https_rrinfo **hrr)
{
  uint16_t pcode = 0, plen = 0;
  uint32_t expected_min_pcode = 0;
  ساخت Curl_https_rrinfo *lhrr = تہی;
  حرف *dnsname = تہی;
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;

  (خالی)ڈیٹا;
  *hrr = تہی;
  اگر(len <= 2)
    واپس CURLE_BAD_FUNCTION_دلیل;
  lhrr = curlx_calloc(1, حجم(ساخت Curl_https_rrinfo));
  اگر(!lhrr)
    واپس CURLE_برآمد_کا_حافظہ;
  lhrr->ترجیح = doh_get16bit(cp, 0);
  cp += 2;
  len -= 2;
  اگر(doh_decode_rdata_name(&cp, &len, &dnsname) != CURLE_OK)
    جاؤ err;
  lhrr->ہدف = dnsname;
  نتیجہ = junkscan(dnsname);
  اگر(نتیجہ) {
    /* unacceptable hostname content */
    جاؤ err;
  }
  جب(len >= 4) {
    pcode = doh_get16bit(cp, 0);
    plen = doh_get16bit(cp, 2);
    cp += 4;
    len -= 4;
    اگر(pcode < expected_min_pcode || plen > len) {
      نتیجہ = CURLE_WEIRD_SERVER_جواب;
      جاؤ err;
    }
    نتیجہ = Curl_httpsrr_سیٹ(lhrr, pcode, cp, plen);
    اگر(نتیجہ)
      جاؤ err;
    Curl_httpsrr_trace(ڈیٹا, lhrr);
    cp += plen;
    len -= plen;
    expected_min_pcode = pcode + 1;
  }
  *hrr = lhrr;
  واپس CURLE_OK;
err:
  Curl_httpsrr_destroy(lhrr);
  واپس نتیجہ;
}

#اگر_ختم /* USE_HTTPSRR */

/* called from multi when a sub transfer, e.g. doh probe, is done.
 * Parse the response and set the results in the `async` context
 * of master, using the id from the probe's CURL_EZM_DOH_PROBE
 * meta data. */
ساکن خالی doh_probe_مکمل(ساخت Curl_easy *doh,
                           ساخت Curl_easy *master, CURLcode نتیجہ)
{
  ساخت Curl_resolv_async *async = تہی;
  ساخت doh_probes *dohp = تہی;
  ساخت doh_request *doh_req = تہی;
  ساخت Curl_addrinfo **pdest_ai;
  ساخت dohentry de;
  صحیح خانہ, httpcode;

  de_init(&de);
  doh_req = Curl_meta_get(doh, CURL_EZM_DOH_PROBE);
  اگر(!doh_req) {
    /* transfer `doh` is not a DoH probe. */
    DEBUGASSERT(0);
    جاؤ برآمد;
  }

  async = Curl_async_get(master, doh_req->resolv_id);
  اگر(!async) {
    CURL_TRC_DNS(master, "[%u] ignoring outdated DoH response",
                 doh_req->resolv_id);
    جاؤ برآمد;
  }
  dohp = async->doh;

  برائے(خانہ = 0; خانہ < DOH_خانہ_گنتی; ++خانہ) {
    اگر(dohp->probe_mid[خانہ] == doh->mid)
      توڑو;
  }
  /* We really should have found the slot where to store the response */
  اگر(خانہ >= DOH_خانہ_گنتی) {
    failf(master, "DoH: unknown sub request done");
    DEBUGASSERT(0);
    جاؤ برآمد;
  }

  async->queries_ongoing--;
  dohp = async->doh;
  httpcode = doh->info.httpcode;
  انتخاب(خانہ) {
  حالت DOH_خانہ_IPV4:
    async->dns_responses |= CURL_DNSQ_A;
    توڑو;
#اگر_تعریف_شدہ USE_IPV6
  حالت DOH_خانہ_IPV6:
    async->dns_responses |= CURL_DNSQ_AAAA;
    توڑو;
#اگر_ختم
#اگر_تعریف_شدہ USE_HTTPSRR
  حالت DOH_خانہ_HTTPS_RR:
    async->dns_responses |= CURL_DNSQ_HTTPS;
    توڑو;
#اگر_ختم
  طےشدہ:
    DEBUGASSERT(0);
    توڑو;
  }

  اگر(نتیجہ) {
    dohp->probe_rc[خانہ] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: %s",
          doh_type2name(doh_req->dnstype), curl_easy_strerror(نتیجہ));
    جاؤ برآمد;
  }
  ورنہ اگر((httpcode < 200) || (httpcode >= 300)) {
    dohp->probe_rc[خانہ] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: HTTP status %d",
          doh_type2name(doh_req->dnstype), httpcode);
    جاؤ برآمد;
  }

  dohp->probe_rc[خانہ] = doh_resp_decode(curlx_dyn_uptr(&doh_req->resp_body),
                                         curlx_dyn_len(&doh_req->resp_body),
                                         doh_req->dnstype, &de);
  اگر(dohp->probe_rc[خانہ]) {
#اگر_تعریف_شدہ USE_HTTPSRR
    اگر((dohp->probe_rc[خانہ] == DOH_NO_CONTENT) &&
       (doh_req->dnstype == CURL_DNS_TYPE_HTTPS)) {
      dohp->probe_rc[خانہ] = DOH_DNS_NXDOMAIN;
    }
#اگر_ختم
    infof(doh, "[DoH] [%s] error decoding response: %s",
          doh_type2name(doh_req->dnstype),
          doh_strerror(dohp->probe_rc[خانہ]));
    جاؤ برآمد;
  }

  اگر(doh_req->dnstype == CURL_DNS_TYPE_A)
    pdest_ai = &async->ai_A;
  ورنہ اگر(doh_req->dnstype == CURL_DNS_TYPE_AAAA)
    pdest_ai = &async->ai_AAAA;
  ورنہ
    pdest_ai = تہی;

  اگر(pdest_ai && de.numaddr) {
    اگر(*pdest_ai) {
      Curl_freeaddrinfo(*pdest_ai);
      *pdest_ai = تہی;
    }
    نتیجہ = doh2ai(&de, async->peer->hostname, async->peer->پورٹ, pdest_ai);
    اگر(نتیجہ) { /* hard failure on our side, fail completely */
      infof(doh, "[DoH] [%s] error creating addrinfo: %s",
            doh_type2name(doh_req->dnstype), curl_easy_strerror(نتیجہ));
      dohp->probe_rc[خانہ] = DOH_OOM;
      async->نتیجہ = نتیجہ;
    }
  }
#اگر_تعریف_شدہ USE_HTTPSRR
  ورنہ اگر((doh_req->dnstype == CURL_DNS_TYPE_HTTPS) && de.numhttps_rrs) {
    CURL_TRC_DNS(doh, "[HTTPS] got %d records", de.numhttps_rrs);
    نتیجہ = doh_resp_decode_httpsrr(doh, de.https_rrs->val,
                                     de.https_rrs->len, &async->httpsrr);
    اگر(نتیجہ) {
      dohp->probe_rc[خانہ] = DOH_HTTP_FAILED;
      infof(doh, "[DoH] error decoding HTTPS RR: %s",
            curl_easy_strerror(نتیجہ));
      جاؤ برآمد;
    }
  }
#اگر_ختم /* USE_HTTPSRR */

  /* DoH request complete, run master to act on results */
  infof(doh, "DoH request complete, %u to go", async->queries_ongoing);

برآمد:
  Curl_multi_mark_dirty(master);
  de_cleanup(&de);
  Curl_meta_remove(doh, CURL_EZM_DOH_PROBE);
}

CURLcode Curl_doh_take_نتیجہ(ساخت Curl_easy *ڈیٹا,
                              ساخت Curl_resolv_async *async,
                              ساخت Curl_dns_اندراج **pdns)
{
  ساخت doh_probes *dohp = async->doh;
  CURLcode نتیجہ = CURLE_OK;

  *pdns = تہی; /* defaults to no response */
  اگر(!dohp)
    واپس CURLE_برآمد_کا_حافظہ;

  async->negative_answer = FALSE;
  اگر(async->نتیجہ) {
    نتیجہ = async->نتیجہ;
    جاؤ برآمد;
  }

  اگر(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     dohp->probe_mid[DOH_خانہ_IPV4] == UINT32_MAX &&
     dohp->probe_mid[DOH_خانہ_IPV6] == UINT32_MAX) {
    failf(ڈیٹا, "Could not DoH-resolve: %s", async->peer->hostname);
    واپس async->for_proxy ?
      CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
  }
  ورنہ اگر(!async->queries_ongoing) {
    ساخت Curl_dns_اندراج *dns = تہی;
    bool negative = TRUE;
    صحیح خانہ;

    /* remove DoH handles from multi handle and close them */
    doh_close(ڈیٹا, async);
    /* parse the responses, create the struct and return it! */
    برائے(خانہ = 0; خانہ < DOH_خانہ_گنتی; خانہ++) {
      /* Failing without an NXDOMAIN answer - a SERVFAIL-class rcode or
         an undecodable response - says nothing about the name. Such a
         failure must not be cached as a negative entry. */
      اگر(dohp->probe_rc[خانہ] && (dohp->probe_rc[خانہ] != DOH_DNS_NXDOMAIN))
        negative = FALSE;
    } /* next slot */

    اگر(async->ai_A || async->ai_AAAA) {
      dns = Curl_dnsc_mk_addr2(
        ڈیٹا, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
      اگر(!dns) {
        نتیجہ = CURLE_برآمد_کا_حافظہ;
        جاؤ برآمد;
      }
    }
#اگر_تعریف_شدہ USE_HTTPSRR
    ورنہ اگر((async->dns_queries & CURL_DNSQ_HTTPS) &&
            !dohp->probe_rc[DOH_خانہ_HTTPS_RR]) {
      Curl_httpsrr_trace(ڈیٹا, async->httpsrr);
      dns = Curl_dnsc_mk_https(ڈیٹا, &async->httpsrr, async->peer);
      اگر(!dns) {
        نتیجہ = CURLE_برآمد_کا_حافظہ;
        جاؤ برآمد;
      }
    }
#اگر_ختم /* USE_HTTPSRR */
    ورنہ {
      /* every query failed. Only NXDOMAIN answers for all of them
         make this a negative answer, eligible for caching. */
      async->negative_answer = negative;
      نتیجہ = async->for_proxy ?
        CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
    }

    /* and add the entry to the cache */
    اگر(dns)
      نتیجہ = Curl_dnscache_جوڑو(ڈیٹا, dns);
    *pdns = dns;
  } /* !async->queries_ongoing */
  ورنہ
    /* wait for pending DoH transactions to complete */
    واپس CURLE_AGAIN;

برآمد:
  Curl_doh_cleanup(ڈیٹا, async);
  واپس نتیجہ;
}

ساکن خالی doh_close(ساخت Curl_easy *ڈیٹا,
                      ساخت Curl_resolv_async *async)
{
  ساخت doh_probes *doh = async ? async->doh : تہی;
  اگر(doh && ڈیٹا->multi) {
    ساخت Curl_easy *probe_ڈیٹا;
    uint32_t mid;
    size_t خانہ;
    برائے(خانہ = 0; خانہ < DOH_خانہ_گنتی; خانہ++) {
      mid = doh->probe_mid[خانہ];
      اگر(mid == UINT32_MAX)
        جاری;
      doh->probe_mid[خانہ] = UINT32_MAX;
      /* should have been called before data is removed from multi handle */
      DEBUGASSERT(ڈیٹا->multi);
      probe_ڈیٹا = ڈیٹا->multi ? Curl_multi_get_easy(ڈیٹا->multi, mid) : تہی;
      اگر(!probe_ڈیٹا) {
        DEBUGF(infof(ڈیٹا, "Curl_doh_close: xfer for mid=%u not found!", mid));
        جاری;
      }
      probe_ڈیٹا->sub_xfer_مکمل = تہی; /* No longer interested in result */
      /* data->multi might already be reset at this time */
      Curl_multi_remove_handle(ڈیٹا->multi, probe_ڈیٹا);
      Curl_close(&probe_ڈیٹا);
    }
    CURL_TRC_DNS(ڈیٹا, "[DoH] probe done");
  }
}

خالی Curl_doh_cleanup(ساخت Curl_easy *ڈیٹا,
                      ساخت Curl_resolv_async *async)
{
  ساخت doh_probes *dohp = async->doh;
  اگر(dohp) {
    doh_close(ڈیٹا, async);
    curlx_safefree(async->doh);
  }
}

#اگر_ختم /* CURL_DISABLE_DOH */
