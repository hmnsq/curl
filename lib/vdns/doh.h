#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_DOH_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_DOH_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"

/* enums outside of the #ifdef to make the types work in unitprotos.h even on
   builds without DoH support */

ਬਣਤਰ Curl_resolv_async;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  DOH_OK,
  DOH_DNS_BAD_LABEL,        /* 1 */
  DOH_DNS_ਬਾਹਰਲਾ_ਦਾ_RANGE,     /* 2 */
  DOH_DNS_LABEL_LOOP,       /* 3 */
  DOH_TOO_SMALL_ਬਫਰ,     /* 4 */
  DOH_ਬਾਹਰਲਾ_ਦਾ_MEM,           /* 5 */
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
  DOH_ਕੋਡ_ਆਖਰੀ             /* Not used, limit */
} DOHcode;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  CURL_DNS_TYPE_A = 1,
  CURL_DNS_TYPE_NS = 2,
  CURL_DNS_TYPE_CNAME = 5,
  CURL_DNS_TYPE_AAAA = 28,
  CURL_DNS_TYPE_DNAME = 39,           /* RFC6672 */
  CURL_DNS_TYPE_HTTPS = 65
} DNStype;

ਬਣਤਰ dohentry; /* forward-declare for non-DoH builds */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DOH

ਗਿਣਤੀ doh_ਥਾਂ_ਅੰਕ {
  /* Explicit values for first two symbols so as to match hard-coded
   * constants in existing code
   */
  DOH_ਥਾਂ_IPV4 = 0, /* make 'V4' stand out for readability */
  DOH_ਥਾਂ_IPV6 = 1, /* 'V6' likewise */

  /* Space here for (possibly build-specific) additional slot definitions */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
  DOH_ਥਾਂ_HTTPS_RR = 2,     /* for HTTPS RR */
#ਜੇ_ਅੰਤ

  /* for example */
  /* #ifdef WANT_DOH_FOOBAR_TXT */
  /*   DOH_PROBE_SLOT_FOOBAR_TXT, */
  /* #endif */

  /* AFTER all slot definitions, establish how many we have */
  DOH_ਥਾਂ_ਗਿਣਤੀ
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_EZM_DOH_PROBE   "ezm:doh-p"

/* the largest one we can make, based on RFCs 1034, 1035 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOH_MAX_DNSREQ_ਮਾਪ (256 + 16)

/* each DoH probe request has this
 * as easy meta for CURL_EZM_DOH_PROBE */
ਬਣਤਰ doh_request {
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ req_body[DOH_MAX_DNSREQ_ਮਾਪ];
  ਬਣਤਰ curl_slist *req_hds;
  ਬਣਤਰ dynbuf resp_body;
  size_t req_body_len;
  uint32_t resolv_id; /* id of the resolve operation */
  DNStype dnstype;
};

/* each transfer firing off DoH requests has this
 * as easy meta for CURL_EZM_DOH_MASTER */
ਬਣਤਰ doh_probes {
  uint32_t probe_mid[DOH_ਥਾਂ_ਗਿਣਤੀ];
  DOHcode probe_rc[DOH_ਥਾਂ_ਗਿਣਤੀ];
};

/*
 * Curl_doh() starts a name resolve using DoH (DNS-over-HTTPS). It resolves a
 * name and returns a 'Curl_addrinfo *' with the address information.
 */
CURLcode Curl_doh(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                  ਬਣਤਰ Curl_resolv_async *async);

CURLcode Curl_doh_take_ਨਤੀਜਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ Curl_resolv_async *async,
                              ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ **pdns);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOH_MAX_ADDR  24
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DOH_MAX_HTTPS 4

ਬਣਤਰ dohaddr {
  ਪੂਰਨ type;
  ਸੰਘ {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ v4[4]; /* network byte order */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ v6[16];
  } ip;
};

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR

/*
 * These may need escaping when found within an ALPN string
 * value.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COMMA_CHAR                    ','
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BACKSLASH_CHAR                '\\'

ਬਣਤਰ dohhttps_rr {
  uint16_t len; /* raw encoded length */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *val; /* raw encoded octets */
};
#ਜੇ_ਅੰਤ

ਬਣਤਰ dohentry {
  ਬਣਤਰ dohaddr addr[DOH_MAX_ADDR];
  ਪੂਰਨ numaddr;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ttl;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
  ਬਣਤਰ dohhttps_rr https_rrs[DOH_MAX_HTTPS];
  ਪੂਰਨ numhttps_rrs;
#ਜੇ_ਅੰਤ
};

ਖਾਲੀ Curl_doh_cleanup(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                      ਬਣਤਰ Curl_resolv_async *async);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_doh_wanted(d)  (!!(d)->ਸੈੱਟ.doh)

#ਨਹੀਂਤਾਂ /* CURL_DISABLE_DOH */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_doh(a, b)             ਨਿਰਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_doh_take_ਨਤੀਜਾ(x, y, z) CURLE_COULDNT_RESOLVE_HOST
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_doh_wanted(d)         FALSE
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_DOH */

#ਜੇ_ਅੰਤ /* HEADER_CURL_DOH_H */
