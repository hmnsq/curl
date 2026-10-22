#నిర్వచితం_కాకపోతే శీర్షిక_CURL_HTTPSRR_H
#నిర్వచించు శీర్షిక_CURL_HTTPSRR_H
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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_ARES
#అన్తర్భావ <ares.h>
#అయితే_ముగింపు

#నిర్వచితమైతే USE_HTTPSRR

#నిర్వచించు CURL_MAXLEN_HOST_NAME 253
#నిర్వచించు MAX_HTTPSRR_ALPNS     4

నిర్మాణం Curl_easy;
నిర్మాణం dynbuf;

నిర్మాణం Curl_https_rrinfo {
  /*
   * Fields from HTTPS RR. The only mandatory fields are priority and target.
   * See https://datatracker.ietf.org/doc/html/rfc9460#section-14.3.2
   */
  అక్షరం *లక్ష్యం;
  గుర్తులేని అక్షరం *ipv4hints; /* keytag = 4 */
  size_t ipv4hints_len;
  గుర్తులేని అక్షరం *echconfiglist; /* keytag = 5 */
  size_t echconfiglist_len;
  గుర్తులేని అక్షరం *ipv6hints; /* keytag = 6 */
  size_t ipv6hints_len;
  గుర్తులేని అక్షరం alpns[MAX_HTTPSRR_ALPNS]; /* keytag = 1 */
  /* store parsed alpnid entries in the array, end with ALPN_none */
  uint16_t పోర్టు;
  uint16_t ప్రాధాన్యత;
  బిట్టు(no_def_alpn); /* keytag = 2 */
  బిట్టు(mandatory); /* keytag = 0 */
  బిట్టు(పోర్టు_సమితి); /* port value has been assigned */
};

CURLcode Curl_httpsrr_సమితి(నిర్మాణం Curl_https_rrinfo *rr,
                          uint16_t rrkey, స్థిరరాశి uint8_t *val, size_t vlen);

శూన్య Curl_httpsrr_destroy(నిర్మాణం Curl_https_rrinfo *rrinfo);

/* TRUE if the record is applicable to the transfer and its connection. */
bool Curl_httpsrr_applicable(నిర్మాణం Curl_easy *దత్తాంశం,
                             స్థిరరాశి నిర్మాణం Curl_https_rrinfo *rr);

/* TRUE if the record is applicable to the given peer. */
bool Curl_httpsrr_is_for_peer(నిర్మాణం Curl_peer *peer,
                              స్థిరరాశి నిర్మాణం Curl_https_rrinfo *rr);

/*
 * Code points for DNS wire format SvcParams as per RFC 9460
 */
#నిర్వచించు HTTPS_RR_సంకేతం_MANDATORY       0x00
#నిర్వచించు HTTPS_RR_సంకేతం_ALPN            0x01
#నిర్వచించు HTTPS_RR_సంకేతం_NO_DEF_ALPN     0x02
#నిర్వచించు HTTPS_RR_సంకేతం_పోర్టు            0x03
#నిర్వచించు HTTPS_RR_సంకేతం_IPV4            0x04
#నిర్వచించు HTTPS_RR_సంకేతం_ECH             0x05
#నిర్వచించు HTTPS_RR_సంకేతం_IPV6            0x06

#నిర్వచితమైతే USE_ARES
CURLcode Curl_httpsrr_from_ares(స్థిరరాశి ares_dns_రికార్డు_t *dnsrec,
                                నిర్మాణం Curl_https_rrinfo **phinfo);
#అయితే_ముగింపు /* USE_ARES */

#నిర్వచితమైతే CURLVERBOSE
CURLcode Curl_httpsrr_print(నిర్మాణం dynbuf *tmp,
                            నిర్మాణం Curl_https_rrinfo *rr);
శూన్య Curl_httpsrr_trace(నిర్మాణం Curl_easy *దత్తాంశం,
                        నిర్మాణం Curl_https_rrinfo *rr);
#లేకపోతే
#నిర్వచించు Curl_httpsrr_trace(a, b) Curl_nop_stmt
#అయితే_ముగింపు

#అయితే_ముగింపు /* USE_HTTPSRR */

#అయితే_ముగింపు /* HEADER_CURL_HTTPSRR_H */
