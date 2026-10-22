#నిర్వచితం_కాకపోతే శీర్షిక_CURL_ALTSVC_H
#నిర్వచించు శీర్షిక_CURL_ALTSVC_H
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

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#అన్తర్భావ "llist.h"

నిర్మాణం Curl_peer;

/* the maximum number of alt-svc entries kept in a single cache */
#నిర్వచించు MAX_ALTSVC_ENTRIES 5000

నిర్మాణం althost {
  అక్షరం *host;
  uint16_t పోర్టు;
  గణన alpnid alpnid;
};

నిర్మాణం altsvc {
  నిర్మాణం althost src;
  నిర్మాణం althost dst;
  time_t expires;
  నిర్మాణం Curl_llist_కణుపు కణుపు;
  బిట్టు(persist);
};

నిర్మాణం altsvcinfo {
  అక్షరం *filename;
  నిర్మాణం Curl_llist list; /* list of entries */
  దీర్ఘ flags; /* the publicly set bitmask */
};

స్థిరరాశి అక్షరం *Curl_alpnid2str(గణన alpnid id);
నిర్మాణం altsvcinfo *Curl_altsvc_init(శూన్య);
CURLcode Curl_altsvc_ఎక్కించు(నిర్మాణం altsvcinfo *asi, స్థిరరాశి అక్షరం *file);
CURLcode Curl_altsvc_save(నిర్మాణం Curl_easy *దత్తాంశం,
                          నిర్మాణం altsvcinfo *asi, స్థిరరాశి అక్షరం *file);
CURLcode Curl_altsvc_ctrl(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి దీర్ఘ ctrl);
శూన్య Curl_altsvc_cleanup(నిర్మాణం altsvcinfo **asi);
CURLcode Curl_altsvc_parse(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం altsvcinfo *asi, స్థిరరాశి అక్షరం *విలువ,
                           నిర్మాణం Curl_peer *origin,
                           గణన alpnid origin_alpnid);
bool Curl_altsvc_lookup(నిర్మాణం altsvcinfo *asi,
                        నిర్మాణం Curl_peer *origin,
                        గణన alpnid origin_alpnid,
                        నిర్మాణం altsvc **dstentry,
                        స్థిరరాశి పూర్ణాంకం versions, /* CURLALTSVC_H* bits */
                        bool *psame_destination);
#లేకపోతే
/* disabled */
#నిర్వచించు Curl_altsvc_save(a, b, c)
#నిర్వచించు Curl_altsvc_cleanup(x)
#అయితే_ముగింపు /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
#అయితే_ముగింపు /* HEADER_CURL_ALTSVC_H */
