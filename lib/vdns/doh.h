#که_نه_تعریف_شوی سرلیک_CURL_DOH_H
#تعریف_کړه سرلیک_CURL_DOH_H
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
#ضمن "urldata.h"

/* enums outside of the #ifdef to make the types work in unitprotos.h even on
   builds without DoH support */

بنية Curl_resolv_async;

تعريفنوع تعداد {
  DOH_OK,
  DOH_DNS_BAD_LABEL,        /* 1 */
  DOH_DNS_بهر_د_RANGE,     /* 2 */
  DOH_DNS_LABEL_LOOP,       /* 3 */
  DOH_TOO_SMALL_بفر,     /* 4 */
  DOH_بهر_د_MEM,           /* 5 */
  DOH_DNS_RDATA_LEN,        /* 6 */
  DOH_DNS_MALFORMAT,        /* 7 */
  DOH_DNS_BAD_RCODE,        /* 8 - unsuccessful rcode, not NXDOMAIN */
  DOH_DNS_UNEXPECTED_TYPE,  /* 9 */
  DOH_DNS_UNEXPECTED_CLASS, /* 10 */
  DOH_NO_CONTENT,           /* 11 */
  DOH_DNS_BAD_ID,           /* 12 */
  DOH_DNS_NAME_TOO_LONG,    /* 13 */
  DOH_DNS_NXDOMAIN,         /* 14 - no such name */
  DOH_HTTP_FAILED,          /* failure at the HTTP level */
  DOH_OOM,                  /* out of memory */
  DOH_کوډ_وروستی             /* Not used, limit */
} DOHcode;

تعريفنوع تعداد {
  CURL_DNS_TYPE_A = 1,
  CURL_DNS_TYPE_NS = 2,
  CURL_DNS_TYPE_CNAME = 5,
  CURL_DNS_TYPE_AAAA = 28,
  CURL_DNS_TYPE_DNAME = 39,           /* RFC6672 */
  CURL_DNS_TYPE_HTTPS = 65
} DNStype;

بنية dohentry; /* forward-declare for non-DoH builds */

#که_نه_تعریف_شوی CURL_DISABLE_DOH

تعداد doh_خونه_عدد {
  /* Explicit values for first two symbols so as to match hard-coded
   * constants in existing code
   */
  DOH_خونه_IPV4 = 0, /* make 'V4' stand out for readability */
  DOH_خونه_IPV6 = 1, /* 'V6' likewise */

  /* Space here for (possibly build-specific) additional slot definitions */
#که_تعریف_شوی USE_HTTPSRR
  DOH_خونه_HTTPS_RR = 2,     /* for HTTPS RR */
#د_که_پای

  /* for example */
  /* #ifdef WANT_DOH_FOOBAR_TXT */
  /*   DOH_PROBE_SLOT_FOOBAR_TXT, */
  /* #endif */

  /* AFTER all slot definitions, establish how many we have */
  DOH_خونه_شمېر
};

#تعریف_کړه CURL_EZM_DOH_PROBE   "ezm:doh-p"

/* the largest one we can make, based on RFCs 1034, 1035 */
#تعریف_کړه DOH_MAX_DNSREQ_اندازه (256 + 16)

/* each DoH probe request has this
 * as easy meta for CURL_EZM_DOH_PROBE */
بنية doh_request {
  بینښه حرف req_body[DOH_MAX_DNSREQ_اندازه];
  بنية curl_slist *req_hds;
  بنية dynbuf resp_body;
  size_t req_body_len;
  uint32_t resolv_id; /* id of the resolve operation */
  DNStype dnstype;
};

/* each transfer firing off DoH requests has this
 * as easy meta for CURL_EZM_DOH_MASTER */
بنية doh_probes {
  uint32_t probe_mid[DOH_خونه_شمېر];
  DOHcode probe_rc[DOH_خونه_شمېر];
};

/*
 * Curl_doh() starts a name resolve using DoH (DNS-over-HTTPS). It resolves a
 * name and returns a 'Curl_addrinfo *' with the address information.
 */
CURLcode Curl_doh(بنية Curl_easy *معلومات,
                  بنية Curl_resolv_async *async);

CURLcode Curl_doh_take_پایله(بنية Curl_easy *معلومات,
                              بنية Curl_resolv_async *async,
                              بنية Curl_dns_ثبت **pdns);

#تعریف_کړه DOH_MAX_ADDR  24
#تعریف_کړه DOH_MAX_HTTPS 4

بنية dohaddr {
  صحيح type;
  اتحاد {
    بینښه حرف v4[4]; /* network byte order */
    بینښه حرف v6[16];
  } ip;
};

#که_تعریف_شوی USE_HTTPSRR

/*
 * These may need escaping when found within an ALPN string
 * value.
 */
#تعریف_کړه COMMA_CHAR                    ','
#تعریف_کړه BACKSLASH_CHAR                '\\'

بنية dohhttps_rr {
  uint16_t len; /* raw encoded length */
  بینښه حرف *val; /* raw encoded octets */
};
#د_که_پای

بنية dohentry {
  بنية dohaddr addr[DOH_MAX_ADDR];
  صحيح numaddr;
  بینښه صحيح ttl;
#که_تعریف_شوی USE_HTTPSRR
  بنية dohhttps_rr https_rrs[DOH_MAX_HTTPS];
  صحيح numhttps_rrs;
#د_که_پای
};

فراغ Curl_doh_cleanup(بنية Curl_easy *معلومات,
                      بنية Curl_resolv_async *async);
#تعریف_کړه Curl_doh_wanted(d)  (!!(d)->سېټ.doh)

#وإلا /* CURL_DISABLE_DOH */
#تعریف_کړه Curl_doh(a, b)             عدم
#تعریف_کړه Curl_doh_take_پایله(x, y, z) CURLE_COULDNT_RESOLVE_HOST
#تعریف_کړه Curl_doh_wanted(d)         FALSE
#د_که_پای /* !CURL_DISABLE_DOH */

#د_که_پای /* HEADER_CURL_DOH_H */
