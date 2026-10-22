#अगर_अघोषित HEADER_CURL_HTTPSRR_H
#घोषणा HEADER_CURL_HTTPSRR_H
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
#समावेश "curl_setup.h"

#अगर_घोषित USE_ARES
#समावेश <ares.h>
#पूर्ण_अगर

#अगर_घोषित USE_HTTPSRR

#घोषणा CURL_MAXLEN_HOST_NAME 253
#घोषणा MAX_HTTPSRR_ALPNS     4

काष्ठा Curl_easy;
काष्ठा dynbuf;

काष्ठा Curl_https_rrinfo {
  /*
   * Fields from HTTPS RR. The only mandatory fields are priority and target.
   * See https://datatracker.ietf.org/doc/html/rfc9460#section-14.3.2
   */
  अक्षर *target;
  अचिन्हित अक्षर *ipv4hints; /* keytag = 4 */
  माप_प्रकार ipv4hints_len;
  अचिन्हित अक्षर *echconfiglist; /* keytag = 5 */
  माप_प्रकार echconfiglist_len;
  अचिन्हित अक्षर *ipv6hints; /* keytag = 6 */
  माप_प्रकार ipv6hints_len;
  अचिन्हित अक्षर alpns[MAX_HTTPSRR_ALPNS]; /* keytag = 1 */
  /* store parsed alpnid entries in the array, end with ALPN_none */
  uint16_t port;
  uint16_t priority;
  BIT(no_def_alpn); /* keytag = 2 */
  BIT(mandatory); /* keytag = 0 */
  BIT(port_set); /* port value has been assigned */
};

CURLcode Curl_httpsrr_set(काष्ठा Curl_https_rrinfo *rr,
                          uint16_t rrkey, स्थिर uint8_t *val, माप_प्रकार vlen);

व्योम Curl_httpsrr_destroy(काष्ठा Curl_https_rrinfo *rrinfo);

/* TRUE if the record is applicable to the transfer and its connection. */
bool Curl_httpsrr_applicable(काष्ठा Curl_easy *तथ्य,
                             स्थिर काष्ठा Curl_https_rrinfo *rr);

/* TRUE if the record is applicable to the given peer. */
bool Curl_httpsrr_is_for_peer(काष्ठा Curl_peer *peer,
                              स्थिर काष्ठा Curl_https_rrinfo *rr);

/*
 * Code points for DNS wire format SvcParams as per RFC 9460
 */
#घोषणा HTTPS_RR_CODE_MANDATORY       0x00
#घोषणा HTTPS_RR_CODE_ALPN            0x01
#घोषणा HTTPS_RR_CODE_NO_DEF_ALPN     0x02
#घोषणा HTTPS_RR_CODE_PORT            0x03
#घोषणा HTTPS_RR_CODE_IPV4            0x04
#घोषणा HTTPS_RR_CODE_ECH             0x05
#घोषणा HTTPS_RR_CODE_IPV6            0x06

#अगर_घोषित USE_ARES
CURLcode Curl_httpsrr_from_ares(स्थिर ares_dns_record_t *dnsrec,
                                काष्ठा Curl_https_rrinfo **phinfo);
#पूर्ण_अगर /* USE_ARES */

#अगर_घोषित CURLVERBOSE
CURLcode Curl_httpsrr_print(काष्ठा dynbuf *tmp,
                            काष्ठा Curl_https_rrinfo *rr);
व्योम Curl_httpsrr_trace(काष्ठा Curl_easy *तथ्य,
                        काष्ठा Curl_https_rrinfo *rr);
#अन्यथा
#घोषणा Curl_httpsrr_trace(a, b) Curl_nop_stmt
#पूर्ण_अगर

#पूर्ण_अगर /* USE_HTTPSRR */

#पूर्ण_अगर /* HEADER_CURL_HTTPSRR_H */
