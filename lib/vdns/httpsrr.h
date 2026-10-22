#که_نه_تعریف_شوی سرلیک_CURL_HTTPSRR_H
#تعریف_کړه سرلیک_CURL_HTTPSRR_H
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

#که_تعریف_شوی USE_ARES
#ضمن <ares.h>
#د_که_پای

#که_تعریف_شوی USE_HTTPSRR

#تعریف_کړه CURL_MAXLEN_HOST_NAME 253
#تعریف_کړه MAX_HTTPSRR_ALPNS     4

بنية Curl_easy;
بنية dynbuf;

بنية Curl_https_rrinfo {
  /*
   * Fields from HTTPS RR. The only mandatory fields are priority and target.
   * See https://datatracker.ietf.org/doc/html/rfc9460#section-14.3.2
   */
  حرف *موخه;
  بینښه حرف *ipv4hints; /* keytag = 4 */
  size_t ipv4hints_len;
  بینښه حرف *echconfiglist; /* keytag = 5 */
  size_t echconfiglist_len;
  بینښه حرف *ipv6hints; /* keytag = 6 */
  size_t ipv6hints_len;
  بینښه حرف alpns[MAX_HTTPSRR_ALPNS]; /* keytag = 1 */
  /* store parsed alpnid entries in the array, end with ALPN_none */
  uint16_t پورټ;
  uint16_t لومړیتوب;
  بټ(no_def_alpn); /* keytag = 2 */
  بټ(mandatory); /* keytag = 0 */
  بټ(پورټ_سېټ); /* port value has been assigned */
};

CURLcode Curl_httpsrr_سېټ(بنية Curl_https_rrinfo *rr,
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
#تعریف_کړه HTTPS_RR_کوډ_MANDATORY       0x00
#تعریف_کړه HTTPS_RR_کوډ_ALPN            0x01
#تعریف_کړه HTTPS_RR_کوډ_NO_DEF_ALPN     0x02
#تعریف_کړه HTTPS_RR_کوډ_پورټ            0x03
#تعریف_کړه HTTPS_RR_کوډ_IPV4            0x04
#تعریف_کړه HTTPS_RR_کوډ_ECH             0x05
#تعریف_کړه HTTPS_RR_کوډ_IPV6            0x06

#که_تعریف_شوی USE_ARES
CURLcode Curl_httpsrr_from_ares(ثابت ares_dns_ریکارډ_t *dnsrec,
                                بنية Curl_https_rrinfo **phinfo);
#د_که_پای /* USE_ARES */

#که_تعریف_شوی CURLVERBOSE
CURLcode Curl_httpsrr_print(بنية dynbuf *tmp,
                            بنية Curl_https_rrinfo *rr);
فراغ Curl_httpsrr_trace(بنية Curl_easy *معلومات,
                        بنية Curl_https_rrinfo *rr);
#وإلا
#تعریف_کړه Curl_httpsrr_trace(a, b) Curl_nop_stmt
#د_که_پای

#د_که_پای /* USE_HTTPSRR */

#د_که_پای /* HEADER_CURL_HTTPSRR_H */
