#ifndef HEADER_CURL_DOH_H
#define HEADER_CURL_DOH_H
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
#ಅನ್ತರ್ಭಾವ "urldata.h"

/* enums outside of the #ifdef to make the types work in unitprotos.h even on
   builds without DoH support */

ಸಂರಚನಾ Curl_resolv_async;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  DOH_OK,
  DOH_DNS_BAD_LABEL,        /* 1 */
  DOH_DNS_OUT_OF_RANGE,     /* 2 */
  DOH_DNS_LABEL_LOOP,       /* 3 */
  DOH_TOO_SMALL_BUFFER,     /* 4 */
  DOH_OUT_OF_MEM,           /* 5 */
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
  DOH_CODE_LAST             /* Not used, limit */
} DOHcode;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  CURL_DNS_TYPE_A = 1,
  CURL_DNS_TYPE_NS = 2,
  CURL_DNS_TYPE_CNAME = 5,
  CURL_DNS_TYPE_AAAA = 28,
  CURL_DNS_TYPE_DNAME = 39,           /* RFC6672 */
  CURL_DNS_TYPE_HTTPS = 65
} DNStype;

ಸಂರಚನಾ dohentry; /* forward-declare for non-DoH builds */

#ifndef CURL_DISABLE_DOH

ಗಣನಾ doh_slot_ಅಂಕೆ {
  /* Explicit values for first two symbols so as to match hard-coded
   * constants in existing code
   */
  DOH_SLOT_IPV4 = 0, /* make 'V4' stand out for readability */
  DOH_SLOT_IPV6 = 1, /* 'V6' likewise */

  /* Space here for (possibly build-specific) additional slot definitions */
#ifdef USE_HTTPSRR
  DOH_SLOT_HTTPS_RR = 2,     /* for HTTPS RR */
#endif

  /* for example */
  /* #ifdef WANT_DOH_FOOBAR_TXT */
  /*   DOH_PROBE_SLOT_FOOBAR_TXT, */
  /* #endif */

  /* AFTER all slot definitions, establish how many we have */
  DOH_SLOT_ಎಣಿಕೆ
};

#define CURL_EZM_DOH_PROBE   "ezm:doh-p"

/* the largest one we can make, based on RFCs 1034, 1035 */
#define DOH_MAX_DNSREQ_SIZE (256 + 16)

/* each DoH probe request has this
 * as easy meta for CURL_EZM_DOH_PROBE */
ಸಂರಚನಾ doh_request {
  unsigned ವರ್ಣ req_body[DOH_MAX_DNSREQ_SIZE];
  ಸಂರಚನಾ curl_slist *req_hds;
  ಸಂರಚನಾ dynbuf resp_body;
  size_t req_body_len;
  uint32_t resolv_id; /* id of the resolve operation */
  DNStype dnstype;
};

/* each transfer firing off DoH requests has this
 * as easy meta for CURL_EZM_DOH_MASTER */
ಸಂರಚನಾ doh_probes {
  uint32_t probe_mid[DOH_SLOT_ಎಣಿಕೆ];
  DOHcode probe_rc[DOH_SLOT_ಎಣಿಕೆ];
};

/*
 * Curl_doh() starts a name resolve using DoH (DNS-over-HTTPS). It resolves a
 * name and returns a 'Curl_addrinfo *' with the address information.
 */
CURLcode Curl_doh(ಸಂರಚನಾ Curl_easy *data,
                  ಸಂರಚನಾ Curl_resolv_async *async);

CURLcode Curl_doh_take_result(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ Curl_resolv_async *async,
                              ಸಂರಚನಾ Curl_dns_entry **pdns);

#define DOH_MAX_ADDR  24
#define DOH_MAX_HTTPS 4

ಸಂರಚನಾ dohaddr {
  ಪೂರ್ಣಾಙ್ಕ type;
  ಸಙ್ಘ {
    unsigned ವರ್ಣ v4[4]; /* network byte order */
    unsigned ವರ್ಣ v6[16];
  } ip;
};

#ifdef USE_HTTPSRR

/*
 * These may need escaping when found within an ALPN string
 * value.
 */
#define COMMA_CHAR                    ','
#define BACKSLASH_CHAR                '\\'

ಸಂರಚನಾ dohhttps_rr {
  uint16_t len; /* raw encoded length */
  unsigned ವರ್ಣ *val; /* raw encoded octets */
};
#endif

ಸಂರಚನಾ dohentry {
  ಸಂರಚನಾ dohaddr addr[DOH_MAX_ADDR];
  ಪೂರ್ಣಾಙ್ಕ numaddr;
  unsigned ಪೂರ್ಣಾಙ್ಕ ttl;
#ifdef USE_HTTPSRR
  ಸಂರಚನಾ dohhttps_rr https_rrs[DOH_MAX_HTTPS];
  ಪೂರ್ಣಾಙ್ಕ numhttps_rrs;
#endif
};

ಶೂನ್ಯ Curl_doh_cleanup(ಸಂರಚನಾ Curl_easy *data,
                      ಸಂರಚನಾ Curl_resolv_async *async);
#define Curl_doh_wanted(d)  (!!(d)->set.doh)

#ಅನ್ಯಥಾ /* CURL_DISABLE_DOH */
#define Curl_doh(a, b)             ರಿಕ್ತ
#define Curl_doh_take_result(x, y, z) CURLE_COULDNT_RESOLVE_HOST
#define Curl_doh_wanted(d)         FALSE
#endif /* !CURL_DISABLE_DOH */

#endif /* HEADER_CURL_DOH_H */
