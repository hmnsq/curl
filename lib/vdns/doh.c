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

#جے_بنا_تعریف CURL_DISABLE_DOH

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

#تعریف_کرو DNS_CLASS_وچ 0x01

ساكن فراغ doh_close(بنية Curl_easy *انکڑے,
                      بنية Curl_resolv_async *async);

#جے_تعریف_شدہ CURLVERBOSE
ساكن ثابت حرف * ثابت doh_کوڈ_str[] = {
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

ساكن ثابت حرف *doh_strerror(DOHcode کوڈ)
{
  إذا((size_t)کوڈ < CURL_ARRAYSIZE(doh_کوڈ_str))
    أرجع doh_کوڈ_str[کوڈ];
  أرجع "bad error code";
}

ساكن ثابت حرف *doh_type2name(DNStype dnstype)
{
  بدل(dnstype) {
  حالة CURL_DNS_TYPE_A:
    أرجع "A";
  حالة CURL_DNS_TYPE_AAAA:
    أرجع "AAAA";
#جے_تعریف_شدہ USE_HTTPSRR
  حالة CURL_DNS_TYPE_HTTPS:
    أرجع "HTTPS";
#جے_انت
  افتراضي:
    أرجع "unknown";
  }
}

#جے_انت /* CURLVERBOSE */

/* @unittest 1655
 */
UNITTEST DOHcode doh_req_encode(ثابت حرف *host,
                                DNStype dnstype,
                                بنانشانی حرف *dnsp,  /*بفر*/
                                size_t len,  /* buffer size */
                                size_t *olen);  /* output length */
UNITTEST DOHcode doh_req_encode(ثابت حرف *host,
                                DNStype dnstype,
                                بنانشانی حرف *dnsp, /*بفر*/
                                size_t len,   /* buffer size */
                                size_t *olen) /* output length */
{
  ثابت size_t hostlen = لڑی_لمبائی(host);
  بنانشانی حرف *orig = dnsp;
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

  إذا(expected_len > DOH_MAX_DNSREQ_ناپ)
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
      labellen = لڑی_لمبائی(hostp);
    إذا((labellen > 63) || (!labellen)) {
      /* label is too long or too short, error out */
      *olen = 0;
      أرجع DOH_DNS_BAD_LABEL;
    }
    /* label is non-empty, process it */
    *dnsp++ = (بنانشانی حرف)labellen;
    حافظہ_نقل(dnsp, hostp, labellen);
    dnsp += labellen;
    hostp += labellen;
    /* advance past dot, but only if there is one */
    إذا(dot)
      hostp++;
  } /* next label */

  *dnsp++ = 0; /* append zero-length label for root */

  /* There are assigned TYPE codes beyond 255: use range [1..65535] */
  *dnsp++ = (بنانشانی حرف)(255 & (dnstype >> 8)); /* upper 8-bit TYPE */
  *dnsp++ = (بنانشانی حرف)(255 & dnstype);        /* lower 8-bit TYPE */

  *dnsp++ = '\0'; /* upper 8-bit CLASS */
  *dnsp++ = DNS_CLASS_وچ; /* IN - "the Internet" */

  *olen = dnsp - orig;

  /* verify that our estimation of length is valid, since
   * this has led to buffer overflows in this function */
  DEBUGASSERT(*olen == expected_len);
  أرجع DOH_OK;
}

ساكن size_t doh_probe_write_cb(حرف *contents, size_t ناپ, size_t nmemb,
                                 فراغ *userp)
{
  size_t realsize = ناپ * nmemb;
  بنية Curl_easy *انکڑے = userp;
  بنية doh_request *doh_req = Curl_meta_get(انکڑے, CURL_EZM_DOH_PROBE);
  إذا(!doh_req)
    أرجع CURL_WRITEFUNC_ERROR;

  إذا(curlx_dyn_addn(&doh_req->resp_body, contents, realsize))
    أرجع 0;

  أرجع realsize;
}

ساكن فراغ doh_probe_پورا(بنية Curl_easy *doh,
                           بنية Curl_easy *master, CURLcode نتیجہ);
ساكن فراغ doh_probe_dtor(ثابت فراغ *کلید, size_t klen, فراغ *e)
{
  (فراغ)کلید;
  (فراغ)klen;
  إذا(e) {
    بنية doh_request *doh_req = e;
    curl_slist_free_all(doh_req->req_hds);
    curlx_dyn_free(&doh_req->resp_body);
    curlx_free(e);
  }
}

#تعریف_کرو ERROR_CHECK_SETOPT(x, y)                        \
  افعل {                                                  \
    نتیجہ = curl_easy_setopt((CURL *)doh, x, y);       \
    إذا(نتیجہ &&                                        \
       نتیجہ != CURLE_NOT_BUILT_وچ &&                  \
       نتیجہ != CURLE_UNKNOWN_OPTION)                  \
      اذهب error;                                       \
  } طالما(0)

ساكن CURLcode doh_probe_لگاتار(بنية Curl_easy *انکڑے,
                              DNStype dnstype,
                              ثابت حرف *host,
                              ثابت حرف *url, CURLM *multi,
                              uint32_t resolv_id,
                              uint32_t *pmid)
{
  بنية Curl_easy *doh = عدم;
  CURLcode نتیجہ = CURLE_OK;
  timediff_t timeout_ms;
  بنية doh_request *doh_req;
  DOHcode d;
  bool maybe_https = !curl_strnequal(url, STRCONST("http:"));

  *pmid = UINT32_MAX;

  doh_req = curlx_calloc(1, حجم(*doh_req));
  إذا(!doh_req)
    أرجع CURLE_باہرلا_دا_حافظہ;
  doh_req->resolv_id = resolv_id;
  doh_req->dnstype = dnstype;
  curlx_dyn_init(&doh_req->resp_body, DYN_DOH_RESPONSE);

  d = doh_req_encode(host, dnstype, doh_req->req_body,
                     حجم(doh_req->req_body),
                     &doh_req->req_body_len);
  إذا(d) {
    failf(انکڑے, "Failed to encode DoH packet [%d]", (صحيح)d);
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب error;
  }

  timeout_ms = Curl_timeleft_ms(انکڑے);
  إذا(timeout_ms < 0) {
    نتیجہ = CURLE_OPERATION_TIMEDOUT;
    اذهب error;
  }

  doh_req->req_hds =
    curl_slist_append(عدم, "Content-Type: application/dns-message");
  إذا(!doh_req->req_hds) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب error;
  }

  /* Curl_open() is the internal version of curl_easy_init() */
  نتیجہ = Curl_open(&doh);
  إذا(نتیجہ)
    اذهب error;

  /* pass in the struct pointer via a local variable to please coverity and
     the gcc typecheck helpers */
  VERBOSE(doh->state.feat = &Curl_trc_feat_doh);
  ERROR_CHECK_SETOPT(CURLOPT_URL, url);
  ERROR_CHECK_SETOPT(CURLOPT_DEFAULT_PROTOCOL, "https");
  ERROR_CHECK_SETOPT(CURLOPT_WRITEFUNCTION, doh_probe_write_cb);
  ERROR_CHECK_SETOPT(CURLOPT_WRITEDATA, doh);
  ERROR_CHECK_SETOPT(CURLOPT_POSTFIELDS, doh_req->req_body);
  ERROR_CHECK_SETOPT(CURLOPT_POSTFIELDSIZE, (لما)doh_req->req_body_len);
  ERROR_CHECK_SETOPT(CURLOPT_HTTPHEADER, doh_req->req_hds);
#جے_تعریف_شدہ USE_HTTP2
  إذا(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_HTTP_نسخہ, CURL_HTTP_نسخہ_2_TLS);
    ERROR_CHECK_SETOPT(CURLOPT_PIPEWAIT, 1L);
  }
#جے_انت
#جے_بنا_تعریف DEBUGBUILD
  /* enforce HTTPS if not debug */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
#وإلا
  /* in debug mode, also allow http */
  ERROR_CHECK_SETOPT(CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
#جے_انت
  ERROR_CHECK_SETOPT(CURLOPT_TIMEOUT_MS, (لما)timeout_ms);
  ERROR_CHECK_SETOPT(CURLOPT_SHARE, (CURLSH *)انکڑے->share);
  إذا(انکڑے->سیٹ.err && انکڑے->سیٹ.err != stderr)
    ERROR_CHECK_SETOPT(CURLOPT_STDERR, انکڑے->سیٹ.err);
  إذا(Curl_trc_ft_is_verbose(انکڑے, &Curl_trc_feat_doh))
    ERROR_CHECK_SETOPT(CURLOPT_VERBOSE, 1L);
  إذا(انکڑے->سیٹ.no_signal)
    ERROR_CHECK_SETOPT(CURLOPT_NOSIGNAL, 1L);
  إذا(انکڑے->سیٹ.fdebug)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGFUNCTION, انکڑے->سیٹ.fdebug);
  إذا(انکڑے->سیٹ.debugdata)
    ERROR_CHECK_SETOPT(CURLOPT_DEBUGDATA, انکڑے->سیٹ.debugdata);

#جے_تعریف_شدہ USE_RESOLV_ARES
  إذا(CURL_EASY_STR(انکڑے, STRING_DNS_SERVERS)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_SERVERS,
                       CURL_EASY_STR(انکڑے, STRING_DNS_SERVERS));
  }
  إذا(CURL_EASY_STR(انکڑے, STRING_DNS_INTERFACE)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_INTERFACE,
                       CURL_EASY_STR(انکڑے, STRING_DNS_INTERFACE));
  }
  إذا(CURL_EASY_STR(انکڑے, STRING_DNS_LOCAL_IP4)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP4,
                       CURL_EASY_STR(انکڑے, STRING_DNS_LOCAL_IP4));
  }
  إذا(CURL_EASY_STR(انکڑے, STRING_DNS_LOCAL_IP6)) {
    ERROR_CHECK_SETOPT(CURLOPT_DNS_LOCAL_IP6,
                       CURL_EASY_STR(انکڑے, STRING_DNS_LOCAL_IP6));
  }
#جے_انت
  إذا(maybe_https) {
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYHOST,
                       انکڑے->سیٹ.doh_verifyhost ? 2L : 0L);
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYPEER,
                       انکڑے->سیٹ.doh_verifypeer ? 1L : 0L);
    ERROR_CHECK_SETOPT(CURLOPT_SSL_VERIFYSTATUS,
                       انکڑے->سیٹ.doh_verifystatus ? 1L : 0L);

    /* Inherit *some* SSL options from the user's transfer. This is a
       best-guess as to which options are needed for compatibility. #3661

       Note DoH does not inherit the user's proxy server so proxy SSL settings
       have no effect and are not inherited. If that changes then two new
       options should be added to check doh proxy insecure separately,
       CURLOPT_DOH_PROXY_SSL_VERIFYHOST and CURLOPT_DOH_PROXY_SSL_VERIFYPEER.
       */
    doh->سیٹ.ssl.custom_cafile = انکڑے->سیٹ.ssl.custom_cafile;
    doh->سیٹ.ssl.custom_capath = انکڑے->سیٹ.ssl.custom_capath;
    doh->سیٹ.ssl.custom_cablob = انکڑے->سیٹ.ssl.custom_cablob;
    إذا(CURL_EASY_STR(انکڑے, STRING_SSL_CAFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO,
                         CURL_EASY_STR(انکڑے, STRING_SSL_CAFILE));
    }
    إذا(انکڑے->سیٹ.blobs[BLOB_CAINFO]) {
      ERROR_CHECK_SETOPT(CURLOPT_CAINFO_BLOB, انکڑے->سیٹ.blobs[BLOB_CAINFO]);
    }
    إذا(CURL_EASY_STR(انکڑے, STRING_SSL_CAPATH)) {
      ERROR_CHECK_SETOPT(CURLOPT_CAPATH,
                         CURL_EASY_STR(انکڑے, STRING_SSL_CAPATH));
    }
    إذا(CURL_EASY_STR(انکڑے, STRING_SSL_CRLFILE)) {
      ERROR_CHECK_SETOPT(CURLOPT_CRLFILE,
                         CURL_EASY_STR(انکڑے, STRING_SSL_CRLFILE));
    }
    إذا(انکڑے->سیٹ.ssl.certinfo)
      ERROR_CHECK_SETOPT(CURLOPT_CERTINFO, 1L);
    إذا(انکڑے->سیٹ.ssl_fsslctx)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_FUNCTION, انکڑے->سیٹ.ssl_fsslctx);
    إذا(انکڑے->سیٹ.ssl_fsslctxp)
      ERROR_CHECK_SETOPT(CURLOPT_SSL_CTX_انکڑے, انکڑے->سیٹ.ssl_fsslctxp);
    إذا(CURL_EASY_STR(انکڑے, STRING_SSL_EC_CURVES)) {
      ERROR_CHECK_SETOPT(CURLOPT_SSL_EC_CURVES,
                         CURL_EASY_STR(انکڑے, STRING_SSL_EC_CURVES));
    }
    (فراغ)curl_easy_setopt(doh, CURLOPT_SSL_OPTIONS,
                           ((لما)انکڑے->سیٹ.ssl.ssl_options &
                            ~CURLSSLOPT_AUTO_CLIENT_CERT));
  }

  doh->state.اندرونی = TRUE;
  doh->master_mid = انکڑے->mid; /* master transfer of this one */
  doh->sub_xfer_پورا = doh_probe_پورا;

  نتیجہ = Curl_meta_سیٹ(doh, CURL_EZM_DOH_PROBE, doh_req, doh_probe_dtor);
  doh_req = عدم; /* call took ownership */
  إذا(نتیجہ)
    اذهب error;

  /* DoH handles must not inherit private_data. The handles may be passed to
     the user via callbacks and the user will be able to identify them as
     internal handles because private data is not set. The user can then set
     private_data via CURLOPT_PRIVATE if they so choose. */
  DEBUGASSERT(!doh->سیٹ.نجی_انکڑے);

  إذا(Curl_multi_جوڑو_handle(multi, doh))
    اذهب error;

  *pmid = doh->mid;
  أرجع CURLE_OK;

error:
  Curl_close(&doh);
  إذا(doh_req)
    doh_probe_dtor(عدم, 0, doh_req);
  أرجع نتیجہ;
}

/*
 * Curl_doh() starts a name resolve using DoH. It resolves a name and returns
 * a 'Curl_addrinfo *' with the address information.
 */

CURLcode Curl_doh(بنية Curl_easy *انکڑے,
                  بنية Curl_resolv_async *async)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية doh_probes *dohp = عدم;
  size_t i;

  DEBUGASSERT(!async->doh);
  DEBUGASSERT(async->peer->hostname[0]);
  إذا(async->doh) {
    DEBUGASSERT(0); /* should not happen */
    Curl_doh_cleanup(انکڑے, async);
  }

  إذا(!async->dns_queries)
    أرجع CURLE_BAD_FUNCTION_دلیل;
#جے_تعریف_شدہ USE_HTTPSRR
  إذا(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     (async->dns_queries & CURL_DNSQ_HTTPS)) {
    /* Can't mix those in the same async resolve */
    DEBUGASSERT(0);
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }
#وإلا
  إذا(async->dns_queries & CURL_DNSQ_HTTPS) {
    DEBUGASSERT(0);
    أرجع CURLE_NOT_BUILT_وچ;
  }
#جے_انت

  /* start clean, consider allocating this struct on demand */
  async->doh = dohp = curlx_calloc(1, حجم(بنية doh_probes));
  إذا(!dohp)
    أرجع CURLE_باہرلا_دا_حافظہ;

  لكل(i = 0; i < DOH_تھاں_گنتی; ++i) {
    dohp->probe_rc[i] = DOH_OK;
    dohp->probe_mid[i] = UINT32_MAX;
  }

#جے_تعریف_شدہ USE_IPV6
  /* AAAA results have preference in happy eyeballing, trigger first */
  إذا(async->dns_queries & CURL_DNSQ_AAAA) {
    /* create IPv6 DoH request */
    نتیجہ = doh_probe_لگاتار(انکڑے, CURL_DNS_TYPE_AAAA,
                           async->peer->hostname,
                           CURL_EASY_STR(انکڑے, STRING_DOH),
                           انکڑے->multi, async->id,
                           &dohp->probe_mid[DOH_تھاں_IPV6]);
    إذا(نتیجہ)
      اذهب error;
    async->queries_ongoing++;
  }
#جے_انت

  /* create IPv4 DoH request */
  إذا(async->dns_queries & CURL_DNSQ_A) {
    نتیجہ = doh_probe_لگاتار(انکڑے, CURL_DNS_TYPE_A,
                           async->peer->hostname,
                           CURL_EASY_STR(انکڑے, STRING_DOH),
                           انکڑے->multi, async->id,
                           &dohp->probe_mid[DOH_تھاں_IPV4]);
    إذا(نتیجہ)
      اذهب error;
    async->queries_ongoing++;
  }

#جے_تعریف_شدہ USE_HTTPSRR
  إذا(async->dns_queries & CURL_DNSQ_HTTPS) {
    حرف *qname = عدم;
    إذا(async->peer->پورٹ != پورٹ_HTTPS) {
      qname = curl_maprintf("_%u._https.%s",
                            async->peer->پورٹ, async->peer->hostname);
      إذا(!qname)
        اذهب error;
    }
    نتیجہ = doh_probe_لگاتار(انکڑے, CURL_DNS_TYPE_HTTPS,
                           qname ? qname : async->peer->hostname,
                           CURL_EASY_STR(انکڑے, STRING_DOH), انکڑے->multi,
                           async->id,
                           &dohp->probe_mid[DOH_تھاں_HTTPS_RR]);
    curlx_free(qname);
    إذا(نتیجہ)
      اذهب error;
    async->queries_ongoing++;
  }
#جے_انت
  أرجع CURLE_OK;

error:
  Curl_doh_cleanup(انکڑے, async);
  أرجع نتیجہ;
}

ساكن DOHcode doh_skipqname(ثابت بنانشانی حرف *doh, size_t dohlen,
                             بنانشانی صحيح *indexp)
{
  بنانشانی حرف لمائی;
  افعل {
    إذا(dohlen < (*indexp + 1))
      أرجع DOH_DNS_باہرلا_دا_RANGE;
    لمائی = doh[*indexp];
    إذا((لمائی & 0xc0) == 0xc0) {
      /* name pointer, advance over it and be done */
      إذا(dohlen < (*indexp + 2))
        أرجع DOH_DNS_باہرلا_دا_RANGE;
      *indexp += 2;
      اكسر;
    }
    إذا(لمائی & 0xc0)
      أرجع DOH_DNS_BAD_LABEL;
    إذا(dohlen < (*indexp + 1 + لمائی))
      أرجع DOH_DNS_باہرلا_دا_RANGE;
    *indexp += (بنانشانی صحيح)(1 + لمائی);
  } طالما(لمائی);
  أرجع DOH_OK;
}

ساكن بنانشانی چھوٹا doh_get16bit(ثابت بنانشانی حرف *doh,
                                   بنانشانی صحيح اشاریہ)
{
  أرجع (بنانشانی چھوٹا)((doh[اشاریہ] << 8) | doh[اشاریہ + 1]);
}

ساكن بنانشانی صحيح doh_get32bit(ثابت بنانشانی حرف *doh, بنانشانی صحيح اشاریہ)
{
  /* make clang and gcc optimize this to bswap by incrementing
     the pointer first. */
  doh += اشاریہ;

  /* avoid undefined behavior by casting to unsigned before shifting
     24 bits, possibly into the sign bit. codegen is same, but
     ub sanitizer will not be upset */
  أرجع ((بنانشانی)doh[0] << 24) | ((بنانشانی)doh[1] << 16) |
         ((بنانشانی)doh[2] << 8) | doh[3];
}

ساكن فراغ doh_store_a(ثابت بنانشانی حرف *doh, صحيح اشاریہ,
                        بنية dohentry *d)
{
  /* silently ignore addresses over the limit */
  إذا(d->numaddr < DOH_MAX_ADDR) {
    بنية dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_A;
    حافظہ_نقل(&a->ip.v4, &doh[اشاریہ], 4);
    d->numaddr++;
  }
}

ساكن فراغ doh_store_aaaa(ثابت بنانشانی حرف *doh, صحيح اشاریہ,
                           بنية dohentry *d)
{
  /* silently ignore addresses over the limit */
  إذا(d->numaddr < DOH_MAX_ADDR) {
    بنية dohaddr *a = &d->addr[d->numaddr];
    a->type = CURL_DNS_TYPE_AAAA;
    حافظہ_نقل(&a->ip.v6, &doh[اشاریہ], 16);
    d->numaddr++;
  }
}

#جے_تعریف_شدہ USE_HTTPSRR
ساكن DOHcode doh_store_https(ثابت بنانشانی حرف *doh, صحيح اشاریہ,
                               بنية dohentry *d, uint16_t len)
{
  /* silently ignore RRs over the limit */
  إذا(d->numhttps_rrs < DOH_MAX_HTTPS) {
    بنية dohhttps_rr *h = &d->https_rrs[d->numhttps_rrs];
    h->val = curlx_memdup(&doh[اشاریہ], len);
    إذا(!h->val)
      أرجع DOH_باہرلا_دا_MEM;
    h->len = len;
    d->numhttps_rrs++;
  }
  أرجع DOH_OK;
}
#جے_انت

ساكن DOHcode doh_rdata(ثابت بنانشانی حرف *doh,
                         بنانشانی چھوٹا rdlength,
                         بنانشانی چھوٹا type,
                         صحيح اشاریہ,
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
    doh_store_a(doh, اشاریہ, d);
    اكسر;
  حالة CURL_DNS_TYPE_AAAA:
    إذا(rdlength != 16)
      أرجع DOH_DNS_RDATA_LEN;
    doh_store_aaaa(doh, اشاریہ, d);
    اكسر;
#جے_تعریف_شدہ USE_HTTPSRR
  حالة CURL_DNS_TYPE_HTTPS:
    إذا(rdlength < 3)
      أرجع DOH_DNS_RDATA_LEN;
    أرجع doh_store_https(doh, اشاریہ, d, rdlength);
#جے_انت
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
  de->ttl = ودھ_توں_ودھ_عدد;
}

/* TTL value cap */
#تعریف_کرو MAX_DNS_TTL 86400U /* 24 hours */
/* @unittest 1650 */
UNITTEST DOHcode doh_resp_decode(ثابت بنانشانی حرف *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 بنية dohentry *d);
UNITTEST DOHcode doh_resp_decode(ثابت بنانشانی حرف *doh,
                                 size_t dohlen,
                                 DNStype dnstype,
                                 بنية dohentry *d)
{
  بنانشانی حرف rcode;
  بنانشانی چھوٹا qdcount;
  بنانشانی چھوٹا ancount;
  بنانشانی چھوٹا type = 0;
  بنانشانی چھوٹا rdlength;
  بنانشانی چھوٹا nscount;
  بنانشانی چھوٹا arcount;
  بنانشانی صحيح اشاریہ = 12;
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
    rc = doh_skipqname(doh, dohlen, &اشاریہ);
    إذا(rc)
      أرجع rc; /* bad qname */
    إذا(dohlen < (اشاریہ + 4))
      أرجع DOH_DNS_باہرلا_دا_RANGE;
    اشاریہ += 4; /* skip question's type and class */
    qdcount--;
  }

  ancount = doh_get16bit(doh, 6);
  طالما(ancount) {
    بنانشانی چھوٹا dnsclass;
    بنانشانی صحيح ttl;

    rc = doh_skipqname(doh, dohlen, &اشاریہ);
    إذا(rc)
      أرجع rc; /* bad qname */

    إذا(dohlen < (اشاریہ + 2))
      أرجع DOH_DNS_باہرلا_دا_RANGE;

    type = doh_get16bit(doh, اشاریہ);
    إذا((type != CURL_DNS_TYPE_CNAME) &&  /* may be synthesized from DNAME */
       (type != CURL_DNS_TYPE_DNAME) &&  /* if present, accept and ignore */
       (type != dnstype))
      /* Not the same type as was asked for, nor CNAME nor DNAME */
      أرجع DOH_DNS_UNEXPECTED_TYPE;
    اشاریہ += 2;

    إذا(dohlen < (اشاریہ + 2))
      أرجع DOH_DNS_باہرلا_دا_RANGE;
    dnsclass = doh_get16bit(doh, اشاریہ);
    إذا(DNS_CLASS_وچ != dnsclass)
      أرجع DOH_DNS_UNEXPECTED_CLASS; /* unsupported */
    اشاریہ += 2;

    إذا(dohlen < (اشاریہ + 4))
      أرجع DOH_DNS_باہرلا_دا_RANGE;

    ttl = doh_get32bit(doh, اشاریہ);
    إذا(ttl > MAX_DNS_TTL)
      ttl = MAX_DNS_TTL;
    إذا(ttl < d->ttl)
      d->ttl = ttl;
    اشاریہ += 4;

    إذا(dohlen < (اشاریہ + 2))
      أرجع DOH_DNS_باہرلا_دا_RANGE;

    rdlength = doh_get16bit(doh, اشاریہ);
    اشاریہ += 2;
    إذا(dohlen < (اشاریہ + rdlength))
      أرجع DOH_DNS_باہرلا_دا_RANGE;

    rc = doh_rdata(doh, rdlength, type, (صحيح)اشاریہ, d);
    إذا(rc)
      أرجع rc;
    اشاریہ += rdlength;
    ancount--;
  }

  nscount = doh_get16bit(doh, 8);
  طالما(nscount) {
    rc = doh_skipqname(doh, dohlen, &اشاریہ);
    إذا(rc)
      أرجع rc; /* bad qname */

    إذا(dohlen < (اشاریہ + 8))
      أرجع DOH_DNS_باہرلا_دا_RANGE;

    اشاریہ += 2 + 2 + 4; /* type, dnsclass and ttl */

    إذا(dohlen < (اشاریہ + 2))
      أرجع DOH_DNS_باہرلا_دا_RANGE;

    rdlength = doh_get16bit(doh, اشاریہ);
    اشاریہ += 2;
    إذا(dohlen < (اشاریہ + rdlength))
      أرجع DOH_DNS_باہرلا_دا_RANGE;
    اشاریہ += rdlength;
    nscount--;
  }

  arcount = doh_get16bit(doh, 10);
  طالما(arcount) {
    rc = doh_skipqname(doh, dohlen, &اشاریہ);
    إذا(rc)
      أرجع rc; /* bad qname */

    إذا(dohlen < (اشاریہ + 8))
      أرجع DOH_DNS_باہرلا_دا_RANGE;

    اشاریہ += 2 + 2 + 4; /* type, dnsclass and ttl */

    إذا(dohlen < (اشاریہ + 2))
      أرجع DOH_DNS_باہرلا_دا_RANGE;

    rdlength = doh_get16bit(doh, اشاریہ);
    اشاریہ += 2;
    إذا(dohlen < (اشاریہ + rdlength))
      أرجع DOH_DNS_باہرلا_دا_RANGE;
    اشاریہ += rdlength;
    arcount--;
  }

  إذا(اشاریہ != dohlen)
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
                       صحيح پورٹ, بنية Curl_addrinfo **aip)
{
  بنية Curl_addrinfo *ai;
  بنية Curl_addrinfo *prevai = عدم;
  بنية Curl_addrinfo *firstai = عدم;
  بنية sockaddr_in *addr;
#جے_تعریف_شدہ USE_IPV6
  بنية sockaddr_in6 *addr6;
#جے_انت
  size_t hostlen = لڑی_لمبائی(hostname) + 1; /* include null-terminator */
  CURLcode نتیجہ = CURLE_OK;
  صحيح i;

  لكل(i = 0; i < de->numaddr; i++) {
    size_t ss_ناپ;
    CURL_SA_FAMILY_T addrtype;
    إذا(de->addr[i].type == CURL_DNS_TYPE_AAAA) {
#جے_بنا_تعریف USE_IPV6
      /* we cannot handle IPv6 addresses */
      استمر;
#وإلا
      ss_ناپ = حجم(بنية sockaddr_in6);
      addrtype = AF_INET6;
#جے_انت
    }
    وإلا {
      ss_ناپ = حجم(بنية sockaddr_in);
      addrtype = AF_INET;
    }

    ai = curlx_calloc(1, حجم(بنية Curl_addrinfo) + ss_ناپ + hostlen);
    إذا(!ai) {
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
      اكسر;
    }
    ai->ai_addr = (فراغ *)((حرف *)ai + حجم(بنية Curl_addrinfo));
    ai->ai_canonname = (فراغ *)((حرف *)ai->ai_addr + ss_ناپ);
    حافظہ_نقل(ai->ai_canonname, hostname, hostlen);

    إذا(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    إذا(prevai)
      /* make the previous entry point to this */
      prevai->ai_اگلا = ai;

    ai->ai_family = addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_ناپ;

    /* leave the rest of the struct filled with zero */

    بدل(ai->ai_family) {
    حالة AF_INET:
      addr = (فراغ *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(حجم(بنية in_addr) == حجم(de->addr[i].ip.v4));
      حافظہ_نقل(&addr->sin_addr, &de->addr[i].ip.v4, حجم(بنية in_addr));
      addr->sin_family = addrtype;
      addr->sin_port = htons((بنانشانی چھوٹا)پورٹ);
      اكسر;

#جے_تعریف_شدہ USE_IPV6
    حالة AF_INET6:
      addr6 = (فراغ *)ai->ai_addr; /* storage area for this info */
      DEBUGASSERT(حجم(بنية in6_addr) == حجم(de->addr[i].ip.v6));
      حافظہ_نقل(&addr6->sin6_addr, &de->addr[i].ip.v6, حجم(بنية in6_addr));
      addr6->sin6_family = addrtype;
      addr6->sin6_port = htons((بنانشانی چھوٹا)پورٹ);
      اكسر;
#جے_انت
    }

    prevai = ai;
  }

  إذا(نتیجہ) {
    Curl_freeaddrinfo(firstai);
    firstai = عدم;
  }
  *aip = firstai;

  أرجع نتیجہ;
}

/* @unittest 1655 */
UNITTEST فراغ de_cleanup(بنية dohentry *d);
UNITTEST فراغ de_cleanup(بنية dohentry *d)
{
#جے_تعریف_شدہ USE_HTTPSRR
  صحيح i = 0;
  لكل(i = 0; i < d->numhttps_rrs; i++)
    curlx_safefree(d->https_rrs[i].val);
#وإلا
  (فراغ)d;
#جے_انت
}

#جے_تعریف_شدہ USE_HTTPSRR

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
ساكن CURLcode doh_decode_rdata_name(ثابت بنانشانی حرف **buf,
                                      size_t *بچیا, حرف **dnsname)
{
  ثابت بنانشانی حرف *cp = عدم;
  size_t rem = 0;
  بنانشانی حرف clen = 0; /* chunk len */
  بنية dynbuf thename;

  DEBUGASSERT(buf && بچیا && dnsname);
  إذا(!buf || !بچیا || !dnsname || !*بچیا)
    أرجع CURLE_باہرلا_دا_حافظہ;
  curlx_dyn_init(&thename, CURL_MAXLEN_HOST_NAME);
  rem = *بچیا;
  cp = *buf;
  clen = *cp++;
  /* RFC 9460 says it must be uncompressed */
  إذا(clen > 63)
    أرجع CURLE_WEIRD_SERVER_جواب;

  إذا(clen == 0) {
    /* special case - return "." as name */
    إذا(curlx_dyn_addn(&thename, ".", 1))
      أرجع CURLE_باہرلا_دا_حافظہ;
  }
  طالما(clen) {
    إذا(clen >= rem) {
      curlx_dyn_free(&thename);
      أرجع CURLE_باہرلا_دا_حافظہ;
    }
    إذا(curlx_dyn_addn(&thename, cp, clen) ||
       curlx_dyn_addn(&thename, ".", 1))
      أرجع CURLE_TOO_LARGE;

    cp += clen;
    rem -= (clen + 1);
    إذا(rem <= 0) {
      curlx_dyn_free(&thename);
      أرجع CURLE_باہرلا_دا_حافظہ;
    }
    clen = *cp++;
    إذا(clen > 63) {
      /* invalid format */
      curlx_dyn_free(&thename);
      أرجع CURLE_WEIRD_SERVER_جواب;
    }
  }
  *buf = cp;
  *بچیا = rem - 1;
  *dnsname = curlx_dyn_ptr(&thename);
  أرجع CURLE_OK;
}

/* scan for byte values <= 32 or 127 */
ساكن CURLcode junkscan(ثابت حرف *url)
{
  ثابت بنانشانی حرف *p = (ثابت بنانشانی حرف *)url;
  طالما(*p) {
    إذا(*p <= 0x20 || *p == 127)
      أرجع CURLE_WEIRD_SERVER_جواب;
    p++;
  }
  أرجع CURLE_OK;
}

/* @unittest 1658 */
UNITTEST CURLcode doh_resp_decode_httpsrr(بنية Curl_easy *انکڑے,
                                          ثابت بنانشانی حرف *cp, size_t len,
                                          بنية Curl_https_rrinfo **hrr);
UNITTEST CURLcode doh_resp_decode_httpsrr(بنية Curl_easy *انکڑے,
                                          ثابت بنانشانی حرف *cp, size_t len,
                                          بنية Curl_https_rrinfo **hrr)
{
  uint16_t pcode = 0, plen = 0;
  uint32_t expected_min_pcode = 0;
  بنية Curl_https_rrinfo *lhrr = عدم;
  حرف *dnsname = عدم;
  CURLcode نتیجہ = CURLE_باہرلا_دا_حافظہ;

  (فراغ)انکڑے;
  *hrr = عدم;
  إذا(len <= 2)
    أرجع CURLE_BAD_FUNCTION_دلیل;
  lhrr = curlx_calloc(1, حجم(بنية Curl_https_rrinfo));
  إذا(!lhrr)
    أرجع CURLE_باہرلا_دا_حافظہ;
  lhrr->ترجیح = doh_get16bit(cp, 0);
  cp += 2;
  len -= 2;
  إذا(doh_decode_rdata_name(&cp, &len, &dnsname) != CURLE_OK)
    اذهب err;
  lhrr->نشانہ = dnsname;
  نتیجہ = junkscan(dnsname);
  إذا(نتیجہ) {
    /* unacceptable hostname content */
    اذهب err;
  }
  طالما(len >= 4) {
    pcode = doh_get16bit(cp, 0);
    plen = doh_get16bit(cp, 2);
    cp += 4;
    len -= 4;
    إذا(pcode < expected_min_pcode || plen > len) {
      نتیجہ = CURLE_WEIRD_SERVER_جواب;
      اذهب err;
    }
    نتیجہ = Curl_httpsrr_سیٹ(lhrr, pcode, cp, plen);
    إذا(نتیجہ)
      اذهب err;
    Curl_httpsrr_trace(انکڑے, lhrr);
    cp += plen;
    len -= plen;
    expected_min_pcode = pcode + 1;
  }
  *hrr = lhrr;
  أرجع CURLE_OK;
err:
  Curl_httpsrr_destroy(lhrr);
  أرجع نتیجہ;
}

#جے_انت /* USE_HTTPSRR */

/* called from multi when a sub transfer, e.g. doh probe, is done.
 * Parse the response and set the results in the `async` context
 * of master, using the id from the probe's CURL_EZM_DOH_PROBE
 * meta data. */
ساكن فراغ doh_probe_پورا(بنية Curl_easy *doh,
                           بنية Curl_easy *master, CURLcode نتیجہ)
{
  بنية Curl_resolv_async *async = عدم;
  بنية doh_probes *dohp = عدم;
  بنية doh_request *doh_req = عدم;
  بنية Curl_addrinfo **pdest_ai;
  بنية dohentry de;
  صحيح تھاں, httpcode;

  de_init(&de);
  doh_req = Curl_meta_get(doh, CURL_EZM_DOH_PROBE);
  إذا(!doh_req) {
    /* transfer `doh` is not a DoH probe. */
    DEBUGASSERT(0);
    اذهب باہرلا;
  }

  async = Curl_async_get(master, doh_req->resolv_id);
  إذا(!async) {
    CURL_TRC_DNS(master, "[%u] ignoring outdated DoH response",
                 doh_req->resolv_id);
    اذهب باہرلا;
  }
  dohp = async->doh;

  لكل(تھاں = 0; تھاں < DOH_تھاں_گنتی; ++تھاں) {
    إذا(dohp->probe_mid[تھاں] == doh->mid)
      اكسر;
  }
  /* We really should have found the slot where to store the response */
  إذا(تھاں >= DOH_تھاں_گنتی) {
    failf(master, "DoH: unknown sub request done");
    DEBUGASSERT(0);
    اذهب باہرلا;
  }

  async->queries_ongoing--;
  dohp = async->doh;
  httpcode = doh->info.httpcode;
  بدل(تھاں) {
  حالة DOH_تھاں_IPV4:
    async->dns_responses |= CURL_DNSQ_A;
    اكسر;
#جے_تعریف_شدہ USE_IPV6
  حالة DOH_تھاں_IPV6:
    async->dns_responses |= CURL_DNSQ_AAAA;
    اكسر;
#جے_انت
#جے_تعریف_شدہ USE_HTTPSRR
  حالة DOH_تھاں_HTTPS_RR:
    async->dns_responses |= CURL_DNSQ_HTTPS;
    اكسر;
#جے_انت
  افتراضي:
    DEBUGASSERT(0);
    اكسر;
  }

  إذا(نتیجہ) {
    dohp->probe_rc[تھاں] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: %s",
          doh_type2name(doh_req->dnstype), curl_easy_strerror(نتیجہ));
    اذهب باہرلا;
  }
  وإلا إذا((httpcode < 200) || (httpcode >= 300)) {
    dohp->probe_rc[تھاں] = DOH_HTTP_FAILED;
    infof(doh, "[DoH] [%s] error: HTTP status %d",
          doh_type2name(doh_req->dnstype), httpcode);
    اذهب باہرلا;
  }

  dohp->probe_rc[تھاں] = doh_resp_decode(curlx_dyn_uptr(&doh_req->resp_body),
                                         curlx_dyn_len(&doh_req->resp_body),
                                         doh_req->dnstype, &de);
  إذا(dohp->probe_rc[تھاں]) {
#جے_تعریف_شدہ USE_HTTPSRR
    إذا((dohp->probe_rc[تھاں] == DOH_NO_CONTENT) &&
       (doh_req->dnstype == CURL_DNS_TYPE_HTTPS)) {
      dohp->probe_rc[تھاں] = DOH_DNS_NXDOMAIN;
    }
#جے_انت
    infof(doh, "[DoH] [%s] error decoding response: %s",
          doh_type2name(doh_req->dnstype),
          doh_strerror(dohp->probe_rc[تھاں]));
    اذهب باہرلا;
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
    نتیجہ = doh2ai(&de, async->peer->hostname, async->peer->پورٹ, pdest_ai);
    إذا(نتیجہ) { /* hard failure on our side, fail completely */
      infof(doh, "[DoH] [%s] error creating addrinfo: %s",
            doh_type2name(doh_req->dnstype), curl_easy_strerror(نتیجہ));
      dohp->probe_rc[تھاں] = DOH_OOM;
      async->نتیجہ = نتیجہ;
    }
  }
#جے_تعریف_شدہ USE_HTTPSRR
  وإلا إذا((doh_req->dnstype == CURL_DNS_TYPE_HTTPS) && de.numhttps_rrs) {
    CURL_TRC_DNS(doh, "[HTTPS] got %d records", de.numhttps_rrs);
    نتیجہ = doh_resp_decode_httpsrr(doh, de.https_rrs->val,
                                     de.https_rrs->len, &async->httpsrr);
    إذا(نتیجہ) {
      dohp->probe_rc[تھاں] = DOH_HTTP_FAILED;
      infof(doh, "[DoH] error decoding HTTPS RR: %s",
            curl_easy_strerror(نتیجہ));
      اذهب باہرلا;
    }
  }
#جے_انت /* USE_HTTPSRR */

  /* DoH request complete, run master to act on results */
  infof(doh, "DoH request complete, %u to go", async->queries_ongoing);

باہرلا:
  Curl_multi_mark_dirty(master);
  de_cleanup(&de);
  Curl_meta_remove(doh, CURL_EZM_DOH_PROBE);
}

CURLcode Curl_doh_take_نتیجہ(بنية Curl_easy *انکڑے,
                              بنية Curl_resolv_async *async,
                              بنية Curl_dns_اندراج **pdns)
{
  بنية doh_probes *dohp = async->doh;
  CURLcode نتیجہ = CURLE_OK;

  *pdns = عدم; /* defaults to no response */
  إذا(!dohp)
    أرجع CURLE_باہرلا_دا_حافظہ;

  async->negative_answer = FALSE;
  إذا(async->نتیجہ) {
    نتیجہ = async->نتیجہ;
    اذهب باہرلا;
  }

  إذا(CURL_DNSQ_IS_ADDR(async->dns_queries) &&
     dohp->probe_mid[DOH_تھاں_IPV4] == UINT32_MAX &&
     dohp->probe_mid[DOH_تھاں_IPV6] == UINT32_MAX) {
    failf(انکڑے, "Could not DoH-resolve: %s", async->peer->hostname);
    أرجع async->for_proxy ?
      CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
  }
  وإلا إذا(!async->queries_ongoing) {
    بنية Curl_dns_اندراج *dns = عدم;
    bool negative = TRUE;
    صحيح تھاں;

    /* remove DoH handles from multi handle and close them */
    doh_close(انکڑے, async);
    /* parse the responses, create the struct and return it! */
    لكل(تھاں = 0; تھاں < DOH_تھاں_گنتی; تھاں++) {
      /* Failing without an NXDOMAIN answer - a SERVFAIL-class rcode or
         an undecodable response - says nothing about the name. Such a
         failure must not be cached as a negative entry. */
      إذا(dohp->probe_rc[تھاں] && (dohp->probe_rc[تھاں] != DOH_DNS_NXDOMAIN))
        negative = FALSE;
    } /* next slot */

    إذا(async->ai_A || async->ai_AAAA) {
      dns = Curl_dnsc_mk_addr2(
        انکڑے, async->dns_queries, &async->ai_A, &async->ai_AAAA, async->peer);
      إذا(!dns) {
        نتیجہ = CURLE_باہرلا_دا_حافظہ;
        اذهب باہرلا;
      }
    }
#جے_تعریف_شدہ USE_HTTPSRR
    وإلا إذا((async->dns_queries & CURL_DNSQ_HTTPS) &&
            !dohp->probe_rc[DOH_تھاں_HTTPS_RR]) {
      Curl_httpsrr_trace(انکڑے, async->httpsrr);
      dns = Curl_dnsc_mk_https(انکڑے, &async->httpsrr, async->peer);
      إذا(!dns) {
        نتیجہ = CURLE_باہرلا_دا_حافظہ;
        اذهب باہرلا;
      }
    }
#جے_انت /* USE_HTTPSRR */
    وإلا {
      /* every query failed. Only NXDOMAIN answers for all of them
         make this a negative answer, eligible for caching. */
      async->negative_answer = negative;
      نتیجہ = async->for_proxy ?
        CURLE_COULDNT_RESOLVE_PROXY : CURLE_COULDNT_RESOLVE_HOST;
    }

    /* and add the entry to the cache */
    إذا(dns)
      نتیجہ = Curl_dnscache_جوڑو(انکڑے, dns);
    *pdns = dns;
  } /* !async->queries_ongoing */
  وإلا
    /* wait for pending DoH transactions to complete */
    أرجع CURLE_AGAIN;

باہرلا:
  Curl_doh_cleanup(انکڑے, async);
  أرجع نتیجہ;
}

ساكن فراغ doh_close(بنية Curl_easy *انکڑے,
                      بنية Curl_resolv_async *async)
{
  بنية doh_probes *doh = async ? async->doh : عدم;
  إذا(doh && انکڑے->multi) {
    بنية Curl_easy *probe_انکڑے;
    uint32_t mid;
    size_t تھاں;
    لكل(تھاں = 0; تھاں < DOH_تھاں_گنتی; تھاں++) {
      mid = doh->probe_mid[تھاں];
      إذا(mid == UINT32_MAX)
        استمر;
      doh->probe_mid[تھاں] = UINT32_MAX;
      /* should have been called before data is removed from multi handle */
      DEBUGASSERT(انکڑے->multi);
      probe_انکڑے = انکڑے->multi ? Curl_multi_get_easy(انکڑے->multi, mid) : عدم;
      إذا(!probe_انکڑے) {
        DEBUGF(infof(انکڑے, "Curl_doh_close: xfer for mid=%u not found!", mid));
        استمر;
      }
      probe_انکڑے->sub_xfer_پورا = عدم; /* No longer interested in result */
      /* data->multi might already be reset at this time */
      Curl_multi_remove_handle(انکڑے->multi, probe_انکڑے);
      Curl_close(&probe_انکڑے);
    }
    CURL_TRC_DNS(انکڑے, "[DoH] probe done");
  }
}

فراغ Curl_doh_cleanup(بنية Curl_easy *انکڑے,
                      بنية Curl_resolv_async *async)
{
  بنية doh_probes *dohp = async->doh;
  إذا(dohp) {
    doh_close(انکڑے, async);
    curlx_safefree(async->doh);
  }
}

#جے_انت /* CURL_DISABLE_DOH */
