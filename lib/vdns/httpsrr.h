#جے_بنا_تعریف سرخی_CURL_HTTPSRR_H
#تعریف_کرو سرخی_CURL_HTTPSRR_H
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

#جے_تعریف_شدہ USE_ARES
#ضمن <ares.h>
#جے_انت

#جے_تعریف_شدہ USE_HTTPSRR

#تعریف_کرو CURL_MAXLEN_HOST_NAME 253
#تعریف_کرو MAX_HTTPSRR_ALPNS     4

بنية Curl_easy;
بنية dynbuf;

بنية Curl_https_rrinfo {
  /*
   * Fields from HTTPS RR. The only mandatory fields are priority and target.
   * See https://datatracker.ietf.org/doc/html/rfc9460#section-14.3.2
   */
  حرف *نشانہ;
  بنانشانی حرف *ipv4hints; /* keytag = 4 */
  size_t ipv4hints_len;
  بنانشانی حرف *echconfiglist; /* keytag = 5 */
  size_t echconfiglist_len;
  بنانشانی حرف *ipv6hints; /* keytag = 6 */
  size_t ipv6hints_len;
  بنانشانی حرف alpns[MAX_HTTPSRR_ALPNS]; /* keytag = 1 */
  /* store parsed alpnid entries in the array, end with ALPN_none */
  uint16_t پورٹ;
  uint16_t ترجیح;
  ثنائی_ہندسہ(no_def_alpn); /* keytag = 2 */
  ثنائی_ہندسہ(mandatory); /* keytag = 0 */
  ثنائی_ہندسہ(پورٹ_سیٹ); /* port value has been assigned */
};

CURLcode Curl_httpsrr_سیٹ(بنية Curl_https_rrinfo *rr,
                          uint16_t rrkey, ثابت uint8_t *val, size_t vlen);

فراغ Curl_httpsrr_destroy(بنية Curl_https_rrinfo *rrinfo);

/* TRUE if the record is applicable to the transfer and its connection. */
bool Curl_httpsrr_applicable(بنية Curl_easy *انکڑے,
                             ثابت بنية Curl_https_rrinfo *rr);

/* TRUE if the record is applicable to the given peer. */
bool Curl_httpsrr_is_for_peer(بنية Curl_peer *peer,
                              ثابت بنية Curl_https_rrinfo *rr);

/*
 * Code points for DNS wire format SvcParams as per RFC 9460
 */
#تعریف_کرو HTTPS_RR_کوڈ_MANDATORY       0x00
#تعریف_کرو HTTPS_RR_کوڈ_ALPN            0x01
#تعریف_کرو HTTPS_RR_کوڈ_NO_DEF_ALPN     0x02
#تعریف_کرو HTTPS_RR_کوڈ_پورٹ            0x03
#تعریف_کرو HTTPS_RR_کوڈ_IPV4            0x04
#تعریف_کرو HTTPS_RR_کوڈ_ECH             0x05
#تعریف_کرو HTTPS_RR_کوڈ_IPV6            0x06

#جے_تعریف_شدہ USE_ARES
CURLcode Curl_httpsrr_from_ares(ثابت ares_dns_ریکارڈ_t *dnsrec,
                                بنية Curl_https_rrinfo **phinfo);
#جے_انت /* USE_ARES */

#جے_تعریف_شدہ CURLVERBOSE
CURLcode Curl_httpsrr_print(بنية dynbuf *tmp,
                            بنية Curl_https_rrinfo *rr);
فراغ Curl_httpsrr_trace(بنية Curl_easy *انکڑے,
                        بنية Curl_https_rrinfo *rr);
#وإلا
#تعریف_کرو Curl_httpsrr_trace(a, b) Curl_nop_stmt
#جے_انت

#جے_انت /* USE_HTTPSRR */

#جے_انت /* HEADER_CURL_HTTPSRR_H */
