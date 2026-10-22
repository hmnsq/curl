#अगर_अघोषित HEADER_CURL_DOH_H
#घोषणा HEADER_CURL_DOH_H
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
#समावेश "urldata.h"

/* enums outside of the #ifdef to make the types work in unitprotos.h even on
   builds without DoH support */

काष्ठा Curl_resolv_async;

प्रकार क्रमागत {
  DOH_OK,
  DOH_DNS_BAD_LABEL,        /* 1 */
  DOH_DNS_OUT_OF_RANGE,     /* 2 */
  DOH_DNS_LABEL_LOOP,       /* 3 */
  DOH_TOO_SMALL_बफर,     /* 4 */
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

प्रकार क्रमागत {
  CURL_DNS_TYPE_A = 1,
  CURL_DNS_TYPE_NS = 2,
  CURL_DNS_TYPE_CNAME = 5,
  CURL_DNS_TYPE_AAAA = 28,
  CURL_DNS_TYPE_DNAME = 39,           /* RFC6672 */
  CURL_DNS_TYPE_HTTPS = 65
} DNStype;

काष्ठा dohentry; /* forward-declare for non-DoH builds */

#अगर_अघोषित CURL_DISABLE_DOH

क्रमागत doh_slot_अंक {
  /* Explicit values for first two symbols so as to match hard-coded
   * constants in existing code
   */
  DOH_SLOT_IPV4 = 0, /* make 'V4' stand out for readability */
  DOH_SLOT_IPV6 = 1, /* 'V6' likewise */

  /* Space here for (possibly build-specific) additional slot definitions */
#अगर_घोषित USE_HTTPSRR
  DOH_SLOT_HTTPS_RR = 2,     /* for HTTPS RR */
#पूर्ण_अगर

  /* for example */
  /* #ifdef WANT_DOH_FOOBAR_TXT */
  /*   DOH_PROBE_SLOT_FOOBAR_TXT, */
  /* #endif */

  /* AFTER all slot definitions, establish how many we have */
  DOH_SLOT_गिनती
};

#घोषणा CURL_EZM_DOH_PROBE   "ezm:doh-p"

/* the largest one we can make, based on RFCs 1034, 1035 */
#घोषणा DOH_MAX_DNSREQ_आकार (256 + 16)

/* each DoH probe request has this
 * as easy meta for CURL_EZM_DOH_PROBE */
काष्ठा doh_request {
  अचिन्हित अक्षर req_body[DOH_MAX_DNSREQ_आकार];
  काष्ठा curl_slist *req_hds;
  काष्ठा dynbuf resp_body;
  माप_प्रकार req_body_len;
  uint32_t resolv_id; /* id of the resolve operation */
  DNStype dnstype;
};

/* each transfer firing off DoH requests has this
 * as easy meta for CURL_EZM_DOH_MASTER */
काष्ठा doh_probes {
  uint32_t probe_mid[DOH_SLOT_गिनती];
  DOHcode probe_rc[DOH_SLOT_गिनती];
};

/*
 * Curl_doh() starts a name resolve using DoH (DNS-over-HTTPS). It resolves a
 * name and returns a 'Curl_addrinfo *' with the address information.
 */
CURLcode Curl_doh(काष्ठा Curl_easy *तथ्य,
                  काष्ठा Curl_resolv_async *async);

CURLcode Curl_doh_take_परिणाम(काष्ठा Curl_easy *तथ्य,
                              काष्ठा Curl_resolv_async *async,
                              काष्ठा Curl_dns_entry **pdns);

#घोषणा DOH_MAX_ADDR  24
#घोषणा DOH_MAX_HTTPS 4

काष्ठा dohaddr {
  पूर्णांक type;
  जोड़ {
    अचिन्हित अक्षर v4[4]; /* network byte order */
    अचिन्हित अक्षर v6[16];
  } ip;
};

#अगर_घोषित USE_HTTPSRR

/*
 * These may need escaping when found within an ALPN string
 * value.
 */
#घोषणा COMMA_CHAR                    ','
#घोषणा BACKSLASH_CHAR                '\\'

काष्ठा dohhttps_rr {
  uint16_t len; /* raw encoded length */
  अचिन्हित अक्षर *val; /* raw encoded octets */
};
#पूर्ण_अगर

काष्ठा dohentry {
  काष्ठा dohaddr addr[DOH_MAX_ADDR];
  पूर्णांक numaddr;
  अचिन्हित पूर्णांक ttl;
#अगर_घोषित USE_HTTPSRR
  काष्ठा dohhttps_rr https_rrs[DOH_MAX_HTTPS];
  पूर्णांक numhttps_rrs;
#पूर्ण_अगर
};

व्योम Curl_doh_cleanup(काष्ठा Curl_easy *तथ्य,
                      काष्ठा Curl_resolv_async *async);
#घोषणा Curl_doh_wanted(d)  (!!(d)->set.doh)

#अन्यथा /* CURL_DISABLE_DOH */
#घोषणा Curl_doh(a, b)             शून्य
#घोषणा Curl_doh_take_परिणाम(x, y, z) CURLE_COULDNT_RESOLVE_HOST
#घोषणा Curl_doh_wanted(d)         FALSE
#पूर्ण_अगर /* !CURL_DISABLE_DOH */

#पूर्ण_अगर /* HEADER_CURL_DOH_H */
