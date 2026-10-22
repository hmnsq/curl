#اگر_تعریف_نشده سرآیند_CURL_HTTPSRR_H
#تعریف_کن سرآیند_CURL_HTTPSRR_H
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

#اگر_تعریف_شده USE_ARES
#ضمن <ares.h>
#پایان_اگر

#اگر_تعریف_شده USE_HTTPSRR

#تعریف_کن CURL_MAXLEN_HOST_NAME 253
#تعریف_کن MAX_HTTPSRR_ALPNS     4

بنية Curl_easy;
بنية dynbuf;

بنية Curl_https_rrinfo {
  /*
   * Fields from HTTPS RR. The only mandatory fields are priority and target.
   * See https://datatracker.ietf.org/doc/html/rfc9460#section-14.3.2
   */
  حرف *هدف;
  بیعلامت حرف *ipv4hints; /* keytag = 4 */
  size_t ipv4hints_len;
  بیعلامت حرف *echconfiglist; /* keytag = 5 */
  size_t echconfiglist_len;
  بیعلامت حرف *ipv6hints; /* keytag = 6 */
  size_t ipv6hints_len;
  بیعلامت حرف alpns[MAX_HTTPSRR_ALPNS]; /* keytag = 1 */
  /* store parsed alpnid entries in the array, end with ALPN_none */
  uint16_t پورت;
  uint16_t اولویت;
  بیت(no_def_alpn); /* keytag = 2 */
  بیت(mandatory); /* keytag = 0 */
  بیت(پورت_مجموعه); /* port value has been assigned */
};

CURLcode Curl_httpsrr_مجموعه(بنية Curl_https_rrinfo *rr,
                          uint16_t rrkey, ثابت uint8_t *val, size_t vlen);

فراغ Curl_httpsrr_destroy(بنية Curl_https_rrinfo *rrinfo);

/* TRUE if the record is applicable to the transfer and its connection. */
bool Curl_httpsrr_applicable(بنية Curl_easy *داده,
                             ثابت بنية Curl_https_rrinfo *rr);

/* TRUE if the record is applicable to the given peer. */
bool Curl_httpsrr_is_for_peer(بنية Curl_peer *peer,
                              ثابت بنية Curl_https_rrinfo *rr);

/*
 * Code points for DNS wire format SvcParams as per RFC 9460
 */
#تعریف_کن HTTPS_RR_کد_MANDATORY       0x00
#تعریف_کن HTTPS_RR_کد_ALPN            0x01
#تعریف_کن HTTPS_RR_کد_NO_DEF_ALPN     0x02
#تعریف_کن HTTPS_RR_کد_پورت            0x03
#تعریف_کن HTTPS_RR_کد_IPV4            0x04
#تعریف_کن HTTPS_RR_کد_ECH             0x05
#تعریف_کن HTTPS_RR_کد_IPV6            0x06

#اگر_تعریف_شده USE_ARES
CURLcode Curl_httpsrr_from_ares(ثابت ares_dns_رکورد_t *dnsrec,
                                بنية Curl_https_rrinfo **phinfo);
#پایان_اگر /* USE_ARES */

#اگر_تعریف_شده CURLVERBOSE
CURLcode Curl_httpsrr_print(بنية dynbuf *tmp,
                            بنية Curl_https_rrinfo *rr);
فراغ Curl_httpsrr_trace(بنية Curl_easy *داده,
                        بنية Curl_https_rrinfo *rr);
#وإلا
#تعریف_کن Curl_httpsrr_trace(a, b) Curl_nop_stmt
#پایان_اگر

#پایان_اگر /* USE_HTTPSRR */

#پایان_اگر /* HEADER_CURL_HTTPSRR_H */
