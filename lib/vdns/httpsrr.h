#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_HTTPSRR_H
#وصف_ڪريو سرو_CURL_HTTPSRR_H
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

#جيڪڏهن_وصف_ٿيل USE_ARES
#ضمن <ares.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_HTTPSRR

#وصف_ڪريو CURL_MAXLEN_HOST_NAME 253
#وصف_ڪريو MAX_HTTPSRR_ALPNS     4

بنية Curl_easy;
بنية dynbuf;

بنية Curl_https_rrinfo {
  /*
   * Fields from HTTPS RR. The only mandatory fields are priority and target.
   * See https://datatracker.ietf.org/doc/html/rfc9460#section-14.3.2
   */
  حرف *هدف;
  بنانشان حرف *ipv4hints; /* keytag = 4 */
  size_t ipv4hints_len;
  بنانشان حرف *echconfiglist; /* keytag = 5 */
  size_t echconfiglist_len;
  بنانشان حرف *ipv6hints; /* keytag = 6 */
  size_t ipv6hints_len;
  بنانشان حرف alpns[MAX_HTTPSRR_ALPNS]; /* keytag = 1 */
  /* store parsed alpnid entries in the array, end with ALPN_none */
  uint16_t پورٽ;
  uint16_t ترجيح;
  ٻٽو_انگ(no_def_alpn); /* keytag = 2 */
  ٻٽو_انگ(mandatory); /* keytag = 0 */
  ٻٽو_انگ(پورٽ_سيٽ); /* port value has been assigned */
};

CURLcode Curl_httpsrr_سيٽ(بنية Curl_https_rrinfo *rr,
                          uint16_t rrkey, ثابت uint8_t *val, size_t vlen);

فراغ Curl_httpsrr_destroy(بنية Curl_https_rrinfo *rrinfo);

/* TRUE if the record is applicable to the transfer and its connection. */
bool Curl_httpsrr_applicable(بنية Curl_easy *معلومات,
                             ثابت بنية Curl_https_rrinfo *rr);

/* TRUE if the record is applicable to the given peer. */
bool Curl_httpsrr_is_for_peer(بنية Curl_peer *peer,
                              ثابت بنية Curl_https_rrinfo *rr);

/*
 * Code points for DNS wire format SvcParams as per RFC 9460
 */
#وصف_ڪريو HTTPS_RR_ڪوڊ_MANDATORY       0x00
#وصف_ڪريو HTTPS_RR_ڪوڊ_ALPN            0x01
#وصف_ڪريو HTTPS_RR_ڪوڊ_NO_DEF_ALPN     0x02
#وصف_ڪريو HTTPS_RR_ڪوڊ_پورٽ            0x03
#وصف_ڪريو HTTPS_RR_ڪوڊ_IPV4            0x04
#وصف_ڪريو HTTPS_RR_ڪوڊ_ECH             0x05
#وصف_ڪريو HTTPS_RR_ڪوڊ_IPV6            0x06

#جيڪڏهن_وصف_ٿيل USE_ARES
CURLcode Curl_httpsrr_from_ares(ثابت ares_dns_رڪارڊ_t *dnsrec,
                                بنية Curl_https_rrinfo **phinfo);
#جيڪڏهن_پورو /* USE_ARES */

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
CURLcode Curl_httpsrr_print(بنية dynbuf *tmp,
                            بنية Curl_https_rrinfo *rr);
فراغ Curl_httpsrr_trace(بنية Curl_easy *معلومات,
                        بنية Curl_https_rrinfo *rr);
#وإلا
#وصف_ڪريو Curl_httpsrr_trace(a, b) Curl_nop_stmt
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* USE_HTTPSRR */

#جيڪڏهن_پورو /* HEADER_CURL_HTTPSRR_H */
