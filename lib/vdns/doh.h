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
#include "urldata.h"

/* enums outside of the #ifdef to make the types work in unitprotos.h even on
   builds without DoH support */

sa_nracanaa Curl_resolv_async;

typedef ga_nanaa {
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

typedef ga_nanaa {
  CURL_DNS_TYPE_A = 1,
  CURL_DNS_TYPE_NS = 2,
  CURL_DNS_TYPE_CNAME = 5,
  CURL_DNS_TYPE_AAAA = 28,
  CURL_DNS_TYPE_DNAME = 39,           /* RFC6672 */
  CURL_DNS_TYPE_HTTPS = 65
} DNStype;

sa_nracanaa dohentry; /* forward-declare for non-DoH builds */

#ifndef CURL_DISABLE_DOH

ga_nanaa doh_slot_num {
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
  DOH_SLOT_COUNT
};

#define CURL_EZM_DOH_PROBE   "ezm:doh-p"

/* the largest one we can make, based on RFCs 1034, 1035 */
#define DOH_MAX_DNSREQ_SIZE (256 + 16)

/* each DoH probe request has this
 * as easy meta for CURL_EZM_DOH_PROBE */
sa_nracanaa doh_request {
  unsigned var_na req_body[DOH_MAX_DNSREQ_SIZE];
  sa_nracanaa curl_slist *req_hds;
  sa_nracanaa dynbuf resp_body;
  size_t req_body_len;
  uint32_t resolv_id; /* id of the resolve operation */
  DNStype dnstype;
};

/* each transfer firing off DoH requests has this
 * as easy meta for CURL_EZM_DOH_MASTER */
sa_nracanaa doh_probes {
  uint32_t probe_mid[DOH_SLOT_COUNT];
  DOHcode probe_rc[DOH_SLOT_COUNT];
};

/*
 * Curl_doh() starts a name resolve using DoH (DNS-over-HTTPS). It resolves a
 * name and returns a 'Curl_addrinfo *' with the address information.
 */
CURLcode Curl_doh(sa_nracanaa Curl_easy *data,
                  sa_nracanaa Curl_resolv_async *async);

CURLcode Curl_doh_take_result(sa_nracanaa Curl_easy *data,
                              sa_nracanaa Curl_resolv_async *async,
                              sa_nracanaa Curl_dns_entry **pdns);

#define DOH_MAX_ADDR  24
#define DOH_MAX_HTTPS 4

sa_nracanaa dohaddr {
  ចំនួនគត់ type;
  sa_ngha {
    unsigned var_na v4[4]; /* network byte order */
    unsigned var_na v6[16];
  } ip;
};

#ifdef USE_HTTPSRR

/*
 * These may need escaping when found within an ALPN string
 * value.
 */
#define COMMA_CHAR                    ','
#define BACKSLASH_CHAR                '\\'

sa_nracanaa dohhttps_rr {
  uint16_t len; /* raw encoded length */
  unsigned var_na *val; /* raw encoded octets */
};
#endif

sa_nracanaa dohentry {
  sa_nracanaa dohaddr addr[DOH_MAX_ADDR];
  ចំនួនគត់ numaddr;
  unsigned ចំនួនគត់ ttl;
#ifdef USE_HTTPSRR
  sa_nracanaa dohhttps_rr https_rrs[DOH_MAX_HTTPS];
  ចំនួនគត់ numhttps_rrs;
#endif
};

void Curl_doh_cleanup(sa_nracanaa Curl_easy *data,
                      sa_nracanaa Curl_resolv_async *async);
#define Curl_doh_wanted(d)  (!!(d)->set.doh)

#បើមិនដូច្នេះ /* CURL_DISABLE_DOH */
#define Curl_doh(a, b)             NULL
#define Curl_doh_take_result(x, y, z) CURLE_COULDNT_RESOLVE_HOST
#define Curl_doh_wanted(d)         FALSE
#endif /* !CURL_DISABLE_DOH */

#endif /* HEADER_CURL_DOH_H */
