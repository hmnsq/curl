#నిర్వచితం_కాకపోతే శీర్షిక_CURL_SPLAY_H
#నిర్వచించు శీర్షిక_CURL_SPLAY_H
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

#అన్తర్భావ "curlx/timeval.h"

నిర్మాణం Curl_easy;

/* only use function calls to access this struct */
నిర్మాణం Curl_వృక్షం {
  నిర్మాణం Curl_వృక్షం *smaller; /* smaller node */
  నిర్మాణం Curl_వృక్షం *larger;  /* larger node */
  నిర్మాణం Curl_వృక్షం *same;    /* points to the next node with identical key */
  timediff_t కీలకం;            /* this node's "sort" key */
  uint32_t id;               /* provided id for this node */
  బిట్టు(registered);           /* node is registered in splay tree */
};

నిర్మాణం Curl_timeouts {
  నిర్మాణం Curl_వృక్షం *వృక్షం;
  time_t సమయం_పునాది_sec;
};

శూన్య Curl_timeouts_init(నిర్మాణం Curl_timeouts *timeouts,
                        స్థిరరాశి నిర్మాణం curltime *ptime_పునాది);

bool Curl_timeouts_has(నిర్మాణం Curl_easy *దత్తాంశం);

timediff_t Curl_timeouts_స్థానభ్రంశం_us(నిర్మాణం Curl_timeouts *timeouts,
                                   స్థిరరాశి నిర్మాణం curltime *pts);

పూర్ణాంకం Curl_timeouts_తదుపరి_ms(నిర్మాణం Curl_timeouts *timeouts,
                          స్థిరరాశి నిర్మాణం curltime *pnow,
                          timediff_t *pexpire_స్థానభ్రంశం_us,
                          uint32_t *pmid);

bool Curl_timeouts_remove_expired(నిర్మాణం Curl_timeouts *timeouts,
                                  స్థిరరాశి నిర్మాణం curltime *ts,
                                  uint32_t *pmid);

శూన్య Curl_timeouts_కూడు(నిర్మాణం Curl_timeouts *timeouts,
                       నిర్మాణం Curl_easy *దత్తాంశం,
                       timediff_t స్థానభ్రంశం_us);

/* Returns TRUE if data was registered in timeouts before */
bool Curl_timeouts_remove(నిర్మాణం Curl_timeouts *timeouts,
                          నిర్మాణం Curl_easy *దత్తాంశం);

#నిర్వచితమైతే UNITTESTS
uint32_t Curl_splayget(నిర్మాణం Curl_వృక్షం *కణుపు);
#అయితే_ముగింపు

#అయితే_ముగింపు /* HEADER_CURL_SPLAY_H */
