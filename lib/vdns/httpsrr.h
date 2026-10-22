#ifndef HEADER_CURL_HTTPSRR_H
#define HEADER_CURL_HTTPSRR_H
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
#अन्तर्भाव "curl_setup.h"

#ifdef USE_ARES
#अन्तर्भाव <ares.h>
#endif

#ifdef USE_HTTPSRR

#define CURL_MAXLEN_HOST_NAME 253
#define MAX_HTTPSRR_ALPNS     4

संरचना Curl_easy;
संरचना dynbuf;

संरचना Curl_https_rrinfo {
  /*
   * Fields from HTTPS RR. The only mandatory fields are priority and target.
   * See https://datatracker.ietf.org/doc/html/rfc9460#section-14.3.2
   */
  वर्ण *target;
  unsigned वर्ण *ipv4hints; /* keytag = 4 */
  size_t ipv4hints_len;
  unsigned वर्ण *echconfiglist; /* keytag = 5 */
  size_t echconfiglist_len;
  unsigned वर्ण *ipv6hints; /* keytag = 6 */
  size_t ipv6hints_len;
  unsigned वर्ण alpns[MAX_HTTPSRR_ALPNS]; /* keytag = 1 */
  /* store parsed alpnid entries in the array, end with ALPN_none */
  uint16_t port;
  uint16_t priority;
  BIT(no_def_alpn); /* keytag = 2 */
  BIT(mandatory); /* keytag = 0 */
  BIT(port_set); /* port value has been assigned */
};

CURLcode Curl_httpsrr_set(संरचना Curl_https_rrinfo *rr,
                          uint16_t rrkey, ध्रुव uint8_t *val, size_t vlen);

शून्य Curl_httpsrr_destroy(संरचना Curl_https_rrinfo *rrinfo);

/* TRUE if the record is applicable to the transfer and its connection. */
bool Curl_httpsrr_applicable(संरचना Curl_easy *data,
                             ध्रुव संरचना Curl_https_rrinfo *rr);

/* TRUE if the record is applicable to the given peer. */
bool Curl_httpsrr_is_for_peer(संरचना Curl_peer *peer,
                              ध्रुव संरचना Curl_https_rrinfo *rr);

/*
 * Code points for DNS wire format SvcParams as per RFC 9460
 */
#define HTTPS_RR_CODE_MANDATORY       0x00
#define HTTPS_RR_CODE_ALPN            0x01
#define HTTPS_RR_CODE_NO_DEF_ALPN     0x02
#define HTTPS_RR_CODE_PORT            0x03
#define HTTPS_RR_CODE_IPV4            0x04
#define HTTPS_RR_CODE_ECH             0x05
#define HTTPS_RR_CODE_IPV6            0x06

#ifdef USE_ARES
CURLcode Curl_httpsrr_from_ares(ध्रुव ares_dns_record_t *dnsrec,
                                संरचना Curl_https_rrinfo **phinfo);
#endif /* USE_ARES */

#ifdef CURLVERBOSE
CURLcode Curl_httpsrr_print(संरचना dynbuf *tmp,
                            संरचना Curl_https_rrinfo *rr);
शून्य Curl_httpsrr_trace(संरचना Curl_easy *data,
                        संरचना Curl_https_rrinfo *rr);
#अन्यथा
#define Curl_httpsrr_trace(a, b) Curl_nop_stmt
#endif

#endif /* USE_HTTPSRR */

#endif /* HEADER_CURL_HTTPSRR_H */
