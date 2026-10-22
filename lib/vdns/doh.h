#جے_بنا_تعریف سرخی_CURL_DOH_H
#تعریف_کرو سرخی_CURL_DOH_H
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
  DOH_DNS_باہرلا_دا_RANGE,     /* 2 */
  DOH_DNS_LABEL_LOOP,       /* 3 */
  DOH_TOO_SMALL_بفر,     /* 4 */
  DOH_باہرلا_دا_MEM,           /* 5 */
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
  DOH_کوڈ_آخری             /* Not used, limit */
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

#جے_بنا_تعریف CURL_DISABLE_DOH

تعداد doh_تھاں_عدد {
  /* Explicit values for first two symbols so as to match hard-coded
   * constants in existing code
   */
  DOH_تھاں_IPV4 = 0, /* make 'V4' stand out for readability */
  DOH_تھاں_IPV6 = 1, /* 'V6' likewise */

  /* Space here for (possibly build-specific) additional slot definitions */
#جے_تعریف_شدہ USE_HTTPSRR
  DOH_تھاں_HTTPS_RR = 2,     /* for HTTPS RR */
#جے_انت

  /* for example */
  /* #ifdef WANT_DOH_FOOBAR_TXT */
  /*   DOH_PROBE_SLOT_FOOBAR_TXT, */
  /* #endif */

  /* AFTER all slot definitions, establish how many we have */
  DOH_تھاں_گنتی
};

#تعریف_کرو CURL_EZM_DOH_PROBE   "ezm:doh-p"

/* the largest one we can make, based on RFCs 1034, 1035 */
#تعریف_کرو DOH_MAX_DNSREQ_ناپ (256 + 16)

/* each DoH probe request has this
 * as easy meta for CURL_EZM_DOH_PROBE */
بنية doh_request {
  بنانشانی حرف req_body[DOH_MAX_DNSREQ_ناپ];
  بنية curl_slist *req_hds;
  بنية dynbuf resp_body;
  size_t req_body_len;
  uint32_t resolv_id; /* id of the resolve operation */
  DNStype dnstype;
};

/* each transfer firing off DoH requests has this
 * as easy meta for CURL_EZM_DOH_MASTER */
بنية doh_probes {
  uint32_t probe_mid[DOH_تھاں_گنتی];
  DOHcode probe_rc[DOH_تھاں_گنتی];
};

/*
 * Curl_doh() starts a name resolve using DoH (DNS-over-HTTPS). It resolves a
 * name and returns a 'Curl_addrinfo *' with the address information.
 */
CURLcode Curl_doh(بنية Curl_easy *انکڑے,
                  بنية Curl_resolv_async *async);

CURLcode Curl_doh_take_نتیجہ(بنية Curl_easy *انکڑے,
                              بنية Curl_resolv_async *async,
                              بنية Curl_dns_اندراج **pdns);

#تعریف_کرو DOH_MAX_ADDR  24
#تعریف_کرو DOH_MAX_HTTPS 4

بنية dohaddr {
  صحيح type;
  اتحاد {
    بنانشانی حرف v4[4]; /* network byte order */
    بنانشانی حرف v6[16];
  } ip;
};

#جے_تعریف_شدہ USE_HTTPSRR

/*
 * These may need escaping when found within an ALPN string
 * value.
 */
#تعریف_کرو COMMA_CHAR                    ','
#تعریف_کرو BACKSLASH_CHAR                '\\'

بنية dohhttps_rr {
  uint16_t len; /* raw encoded length */
  بنانشانی حرف *val; /* raw encoded octets */
};
#جے_انت

بنية dohentry {
  بنية dohaddr addr[DOH_MAX_ADDR];
  صحيح numaddr;
  بنانشانی صحيح ttl;
#جے_تعریف_شدہ USE_HTTPSRR
  بنية dohhttps_rr https_rrs[DOH_MAX_HTTPS];
  صحيح numhttps_rrs;
#جے_انت
};

فراغ Curl_doh_cleanup(بنية Curl_easy *انکڑے,
                      بنية Curl_resolv_async *async);
#تعریف_کرو Curl_doh_wanted(d)  (!!(d)->سیٹ.doh)

#وإلا /* CURL_DISABLE_DOH */
#تعریف_کرو Curl_doh(a, b)             عدم
#تعریف_کرو Curl_doh_take_نتیجہ(x, y, z) CURLE_COULDNT_RESOLVE_HOST
#تعریف_کرو Curl_doh_wanted(d)         FALSE
#جے_انت /* !CURL_DISABLE_DOH */

#جے_انت /* HEADER_CURL_DOH_H */
