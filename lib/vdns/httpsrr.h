#यदि_अपरिभाषितम् शीर्षकम्_CURL_HTTPSRR_H
#परिभाषय शीर्षकम्_CURL_HTTPSRR_H
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

#यदि_परिभाषितम् USE_ARES
#अन्तर्भाव <ares.h>
#यदि_अन्तः

#यदि_परिभाषितम् USE_HTTPSRR

#परिभाषय CURL_MAXLEN_HOST_NAME 253
#परिभाषय MAX_HTTPSRR_ALPNS     4

संरचना Curl_easy;
संरचना dynbuf;

संरचना Curl_https_rrinfo {
  /*
   * Fields from HTTPS RR. The only mandatory fields are priority and target.
   * See https://datatracker.ietf.org/doc/html/rfc9460#section-14.3.2
   */
  वर्ण *target;
  अचिह्नित वर्ण *ipv4hints; /* keytag = 4 */
  size_t ipv4hints_len;
  अचिह्नित वर्ण *echconfiglist; /* keytag = 5 */
  size_t echconfiglist_len;
  अचिह्नित वर्ण *ipv6hints; /* keytag = 6 */
  size_t ipv6hints_len;
  अचिह्नित वर्ण alpns[MAX_HTTPSRR_ALPNS]; /* keytag = 1 */
  /* store parsed alpnid entries in the array, end with ALPN_none */
  uint16_t पत्तनम्;
  uint16_t प्राथमिकता;
  BIT(no_def_alpn); /* keytag = 2 */
  BIT(mandatory); /* keytag = 0 */
  BIT(पत्तनम्_समुच्चयः); /* port value has been assigned */
};

CURLcode Curl_httpsrr_समुच्चयः(संरचना Curl_https_rrinfo *rr,
                          uint16_t rrkey, ध्रुव uint8_t *val, size_t vlen);

शून्य Curl_httpsrr_destroy(संरचना Curl_https_rrinfo *rrinfo);

/* TRUE if the record is applicable to the transfer and its connection. */
bool Curl_httpsrr_applicable(संरचना Curl_easy *दत्त,
                             ध्रुव संरचना Curl_https_rrinfo *rr);

/* TRUE if the record is applicable to the given peer. */
bool Curl_httpsrr_is_for_peer(संरचना Curl_peer *peer,
                              ध्रुव संरचना Curl_https_rrinfo *rr);

/*
 * Code points for DNS wire format SvcParams as per RFC 9460
 */
#परिभाषय HTTPS_RR_संहिता_MANDATORY       0x00
#परिभाषय HTTPS_RR_संहिता_ALPN            0x01
#परिभाषय HTTPS_RR_संहिता_NO_DEF_ALPN     0x02
#परिभाषय HTTPS_RR_संहिता_पत्तनम्            0x03
#परिभाषय HTTPS_RR_संहिता_IPV4            0x04
#परिभाषय HTTPS_RR_संहिता_ECH             0x05
#परिभाषय HTTPS_RR_संहिता_IPV6            0x06

#यदि_परिभाषितम् USE_ARES
CURLcode Curl_httpsrr_from_ares(ध्रुव ares_dns_लेखः_t *dnsrec,
                                संरचना Curl_https_rrinfo **phinfo);
#यदि_अन्तः /* USE_ARES */

#यदि_परिभाषितम् CURLVERBOSE
CURLcode Curl_httpsrr_print(संरचना dynbuf *tmp,
                            संरचना Curl_https_rrinfo *rr);
शून्य Curl_httpsrr_trace(संरचना Curl_easy *दत्त,
                        संरचना Curl_https_rrinfo *rr);
#अन्यथा
#परिभाषय Curl_httpsrr_trace(a, b) Curl_nop_stmt
#यदि_अन्तः

#यदि_अन्तः /* USE_HTTPSRR */

#यदि_अन्तः /* HEADER_CURL_HTTPSRR_H */
