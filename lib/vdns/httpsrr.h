#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_HTTPSRR_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_HTTPSRR_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ARES
#ਅਨ੍ਤਰ੍ਭਾਵ <ares.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_MAXLEN_HOST_NAME 253
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_HTTPSRR_ALPNS     4

ਬਣਤਰ Curl_easy;
ਬਣਤਰ dynbuf;

ਬਣਤਰ Curl_https_rrinfo {
  /*
   * Fields from HTTPS RR. The only mandatory fields are priority and target.
   * See https://datatracker.ietf.org/doc/html/rfc9460#section-14.3.2
   */
  ਅੱਖਰ *ਨਿਸ਼ਾਨਾ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ipv4hints; /* keytag = 4 */
  size_t ipv4hints_len;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *echconfiglist; /* keytag = 5 */
  size_t echconfiglist_len;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ipv6hints; /* keytag = 6 */
  size_t ipv6hints_len;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ alpns[MAX_HTTPSRR_ALPNS]; /* keytag = 1 */
  /* store parsed alpnid entries in the array, end with ALPN_none */
  uint16_t ਪੋਰਟ;
  uint16_t ਤਰਜੀਹ;
  ਦੋਆਧਾਰੀ_ਅੰਕ(no_def_alpn); /* keytag = 2 */
  ਦੋਆਧਾਰੀ_ਅੰਕ(mandatory); /* keytag = 0 */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੋਰਟ_ਸੈੱਟ); /* port value has been assigned */
};

CURLcode Curl_httpsrr_ਸੈੱਟ(ਬਣਤਰ Curl_https_rrinfo *rr,
                          uint16_t rrkey, ਅਚਲ uint8_t *val, size_t vlen);

ਖਾਲੀ Curl_httpsrr_destroy(ਬਣਤਰ Curl_https_rrinfo *rrinfo);

/* TRUE if the record is applicable to the transfer and its connection. */
bool Curl_httpsrr_applicable(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਅਚਲ ਬਣਤਰ Curl_https_rrinfo *rr);

/* TRUE if the record is applicable to the given peer. */
bool Curl_httpsrr_is_for_peer(ਬਣਤਰ Curl_peer *peer,
                              ਅਚਲ ਬਣਤਰ Curl_https_rrinfo *rr);

/*
 * Code points for DNS wire format SvcParams as per RFC 9460
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTPS_RR_ਕੋਡ_MANDATORY       0x00
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTPS_RR_ਕੋਡ_ALPN            0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTPS_RR_ਕੋਡ_NO_DEF_ALPN     0x02
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTPS_RR_ਕੋਡ_ਪੋਰਟ            0x03
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTPS_RR_ਕੋਡ_IPV4            0x04
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTPS_RR_ਕੋਡ_ECH             0x05
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTPS_RR_ਕੋਡ_IPV6            0x06

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ARES
CURLcode Curl_httpsrr_from_ares(ਅਚਲ ares_dns_ਰਿਕਾਰਡ_t *dnsrec,
                                ਬਣਤਰ Curl_https_rrinfo **phinfo);
#ਜੇ_ਅੰਤ /* USE_ARES */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
CURLcode Curl_httpsrr_print(ਬਣਤਰ dynbuf *tmp,
                            ਬਣਤਰ Curl_https_rrinfo *rr);
ਖਾਲੀ Curl_httpsrr_trace(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਬਣਤਰ Curl_https_rrinfo *rr);
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_httpsrr_trace(a, b) Curl_nop_stmt
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* USE_HTTPSRR */

#ਜੇ_ਅੰਤ /* HEADER_CURL_HTTPSRR_H */
