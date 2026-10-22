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
#অন্তর্ভাব "curl_setup.h"

#ifdef USE_ARES
#অন্তর্ভাব <ares.h>
#endif

#ifdef USE_HTTPSRR

#define CURL_MAXLEN_HOST_NAME 253
#define MAX_HTTPSRR_ALPNS     4

সংরচনা Curl_easy;
সংরচনা dynbuf;

সংরচনা Curl_https_rrinfo {
  /*
   * Fields from HTTPS RR. The only mandatory fields are priority and target.
   * See https://datatracker.ietf.org/doc/html/rfc9460#section-14.3.2
   */
  বর্ণ *target;
  unsigned বর্ণ *ipv4hints; /* keytag = 4 */
  size_t ipv4hints_len;
  unsigned বর্ণ *echconfiglist; /* keytag = 5 */
  size_t echconfiglist_len;
  unsigned বর্ণ *ipv6hints; /* keytag = 6 */
  size_t ipv6hints_len;
  unsigned বর্ণ alpns[MAX_HTTPSRR_ALPNS]; /* keytag = 1 */
  /* store parsed alpnid entries in the array, end with ALPN_none */
  uint16_t port;
  uint16_t priority;
  BIT(no_def_alpn); /* keytag = 2 */
  BIT(mandatory); /* keytag = 0 */
  BIT(port_set); /* port value has been assigned */
};

CURLcode Curl_httpsrr_set(সংরচনা Curl_https_rrinfo *rr,
                          uint16_t rrkey, ধ্রুব uint8_t *val, size_t vlen);

শূন্য Curl_httpsrr_destroy(সংরচনা Curl_https_rrinfo *rrinfo);

/* TRUE if the record is applicable to the transfer and its connection. */
bool Curl_httpsrr_applicable(সংরচনা Curl_easy *data,
                             ধ্রুব সংরচনা Curl_https_rrinfo *rr);

/* TRUE if the record is applicable to the given peer. */
bool Curl_httpsrr_is_for_peer(সংরচনা Curl_peer *peer,
                              ধ্রুব সংরচনা Curl_https_rrinfo *rr);

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
CURLcode Curl_httpsrr_from_ares(ধ্রুব ares_dns_record_t *dnsrec,
                                সংরচনা Curl_https_rrinfo **phinfo);
#endif /* USE_ARES */

#ifdef CURLVERBOSE
CURLcode Curl_httpsrr_print(সংরচনা dynbuf *tmp,
                            সংরচনা Curl_https_rrinfo *rr);
শূন্য Curl_httpsrr_trace(সংরচনা Curl_easy *data,
                        সংরচনা Curl_https_rrinfo *rr);
#অন্যথা
#define Curl_httpsrr_trace(a, b) Curl_nop_stmt
#endif

#endif /* USE_HTTPSRR */

#endif /* HEADER_CURL_HTTPSRR_H */
