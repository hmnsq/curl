#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_DOH_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_DOH_H
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

ರಚನೆ Curl_resolv_async;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  DOH_OK,
  DOH_DNS_BAD_LABEL,        /* 1 */
  DOH_DNS_ಹೊರಹೋಗು_ದ_RANGE,     /* 2 */
  DOH_DNS_LABEL_LOOP,       /* 3 */
  DOH_TOO_SMALL_ಬಫರ್,     /* 4 */
  DOH_ಹೊರಹೋಗು_ದ_MEM,           /* 5 */
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
  DOH_ಸಂಕೇತ_ಕೊನೆಯ             /* Not used, limit */
} DOHcode;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  CURL_DNS_TYPE_A = 1,
  CURL_DNS_TYPE_NS = 2,
  CURL_DNS_TYPE_CNAME = 5,
  CURL_DNS_TYPE_AAAA = 28,
  CURL_DNS_TYPE_DNAME = 39,           /* RFC6672 */
  CURL_DNS_TYPE_HTTPS = 65
} DNStype;

ರಚನೆ dohentry; /* forward-declare for non-DoH builds */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DOH

ಎಣಿಕೆ doh_ಸ್ಥಾನ_ಅಂಕೆ {
  /* Explicit values for first two symbols so as to match hard-coded
   * constants in existing code
   */
  DOH_ಸ್ಥಾನ_IPV4 = 0, /* make 'V4' stand out for readability */
  DOH_ಸ್ಥಾನ_IPV6 = 1, /* 'V6' likewise */

  /* Space here for (possibly build-specific) additional slot definitions */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
  DOH_ಸ್ಥಾನ_HTTPS_RR = 2,     /* for HTTPS RR */
#ಆದರೆ_ಅಂತ್ಯ

  /* for example */
  /* #ifdef WANT_DOH_FOOBAR_TXT */
  /*   DOH_PROBE_SLOT_FOOBAR_TXT, */
  /* #endif */

  /* AFTER all slot definitions, establish how many we have */
  DOH_ಸ್ಥಾನ_ಎಣಿಕೆ
};

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_EZM_DOH_PROBE   "ezm:doh-p"

/* the largest one we can make, based on RFCs 1034, 1035 */
#ವ್ಯಾಖ್ಯಾನಿಸು DOH_MAX_DNSREQ_ಗಾತ್ರ (256 + 16)

/* each DoH probe request has this
 * as easy meta for CURL_EZM_DOH_PROBE */
ರಚನೆ doh_request {
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ req_body[DOH_MAX_DNSREQ_ಗಾತ್ರ];
  ರಚನೆ curl_slist *req_hds;
  ರಚನೆ dynbuf resp_body;
  size_t req_body_len;
  uint32_t resolv_id; /* id of the resolve operation */
  DNStype dnstype;
};

/* each transfer firing off DoH requests has this
 * as easy meta for CURL_EZM_DOH_MASTER */
ರಚನೆ doh_probes {
  uint32_t probe_mid[DOH_ಸ್ಥಾನ_ಎಣಿಕೆ];
  DOHcode probe_rc[DOH_ಸ್ಥಾನ_ಎಣಿಕೆ];
};

/*
 * Curl_doh() starts a name resolve using DoH (DNS-over-HTTPS). It resolves a
 * name and returns a 'Curl_addrinfo *' with the address information.
 */
CURLcode Curl_doh(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                  ರಚನೆ Curl_resolv_async *async);

CURLcode Curl_doh_take_ಫಲಿತಾಂಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ Curl_resolv_async *async,
                              ರಚನೆ Curl_dns_ನಮೂದು **pdns);

#ವ್ಯಾಖ್ಯಾನಿಸು DOH_MAX_ADDR  24
#ವ್ಯಾಖ್ಯಾನಿಸು DOH_MAX_HTTPS 4

ರಚನೆ dohaddr {
  ಪೂರ್ಣಾಂಕ type;
  ಒಕ್ಕೂಟ {
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ v4[4]; /* network byte order */
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ v6[16];
  } ip;
};

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR

/*
 * These may need escaping when found within an ALPN string
 * value.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು COMMA_CHAR                    ','
#ವ್ಯಾಖ್ಯಾನಿಸು BACKSLASH_CHAR                '\\'

ರಚನೆ dohhttps_rr {
  uint16_t len; /* raw encoded length */
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *val; /* raw encoded octets */
};
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ dohentry {
  ರಚನೆ dohaddr addr[DOH_MAX_ADDR];
  ಪೂರ್ಣಾಂಕ numaddr;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ttl;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
  ರಚನೆ dohhttps_rr https_rrs[DOH_MAX_HTTPS];
  ಪೂರ್ಣಾಂಕ numhttps_rrs;
#ಆದರೆ_ಅಂತ್ಯ
};

ಶೂನ್ಯ Curl_doh_cleanup(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                      ರಚನೆ Curl_resolv_async *async);
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_doh_wanted(d)  (!!(d)->ಗಣ.doh)

#ಇಲ್ಲದಿದ್ದರೆ /* CURL_DISABLE_DOH */
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_doh(a, b)             ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_doh_take_ಫಲಿತಾಂಶ(x, y, z) CURLE_COULDNT_RESOLVE_HOST
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_doh_wanted(d)         FALSE
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_DOH */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_DOH_H */
