#నిర్వచితం_కాకపోతే శీర్షిక_CURL_HSTS_H
#నిర్వచించు శీర్షిక_CURL_HSTS_H
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

నిర్మాణం hsts;

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#అన్తర్భావ "llist.h"

నిర్మాణం Curl_peer;

#నిర్వచించు MAX_HSTS_ENTRIES 10000

#ఒకవేళ defined(DEBUGBUILD) || defined(UNITTESTS)
బాహ్య time_t deltatime;
#అయితే_ముగింపు

నిర్మాణం stsentry {
  నిర్మాణం Curl_llist_కణుపు కణుపు;
  curl_off_t expires; /* the timestamp of this entry's expiry */
  బిట్టు(includeSubDomains);
  అక్షరం host[1];
};

/* The HSTS cache. Needs to be able to tailmatch hostnames. */
నిర్మాణం hsts {
  నిర్మాణం Curl_llist list;
  అక్షరం *filename;
  గుర్తులేని పూర్ణాంకం flags;
};

నిర్మాణం hsts *Curl_hsts_init(శూన్య);
శూన్య Curl_hsts_cleanup(నిర్మాణం hsts **hp);
CURLcode Curl_hsts_copy(నిర్మాణం hsts *dst, నిర్మాణం hsts *src);
CURLcode Curl_hsts_parse(నిర్మాణం hsts *h, స్థిరరాశి అక్షరం *hostname,
                         స్థిరరాశి అక్షరం *శీర్షిక);
CURLcode Curl_hsts_save(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం hsts *h,
                        స్థిరరాశి అక్షరం *file);
CURLcode Curl_hsts_loadfile(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం hsts *h, స్థిరరాశి అక్షరం *file);
CURLcode Curl_hsts_loadcb(నిర్మాణం Curl_easy *దత్తాంశం,
                          నిర్మాణం hsts *h);
CURLcode Curl_hsts_loadfiles(నిర్మాణం Curl_easy *దత్తాంశం);

bool Curl_hsts_applies(నిర్మాణం hsts *h, స్థిరరాశి నిర్మాణం Curl_peer *dest);

#లేకపోతే
#నిర్వచించు Curl_hsts_cleanup(x)
#నిర్వచించు Curl_hsts_loadcb(x, y) CURLE_OK
#నిర్వచించు Curl_hsts_save(x, y, z)
#నిర్వచించు Curl_hsts_loadfiles(x) CURLE_OK
#అయితే_ముగింపు /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
#అయితే_ముగింపు /* HEADER_CURL_HSTS_H */
