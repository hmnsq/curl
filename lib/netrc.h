#నిర్వచితం_కాకపోతే శీర్షిక_CURL_NETRC_H
#నిర్వచించు శీర్షిక_CURL_NETRC_H
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

#నిర్వచితం_కాకపోతే CURL_DISABLE_NETRC

#అన్తర్భావ "curlx/dynbuf.h"

నిర్మాణం Curl_easy;
నిర్మాణం Curl_creds;

నిర్మాణం store_netrc {
  నిర్మాణం dynbuf filebuf;
  అక్షరం *filename;
  బిట్టు(loaded);
};

రకనిర్వచనం గణన {
  NETRC_OK,
  NETRC_NO_సరిపోలిక,      /* no matching entry in the file */
  NETRC_SYNTAX_ERROR,  /* in the netrc file */
  NETRC_FILE_MISSING,  /* the netrc file does not exist */
  NETRC_బహిర్గతం_యొక్క_జ్ఞాపకం, /* while parsing netrc */
  NETRC_చివరి /* never used */
} NETRCcode;

స్థిరరాశి అక్షరం *Curl_netrc_strerror(NETRCcode ret);
శూన్య Curl_netrc_init(నిర్మాణం store_netrc *store);
శూన్య Curl_netrc_cleanup(నిర్మాణం store_netrc *store);

/* Scan a netrc file for credentials matching hostname
 * and optional user. */
NETRCcode Curl_netrc_పరిశీలన(నిర్మాణం Curl_easy *దత్తాంశం,
                          నిర్మాణం store_netrc *store,
                          స్థిరరాశి అక్షరం *hostname,
                          స్థిరరాశి అక్షరం *user,
                          స్థిరరాశి అక్షరం *netrcfile,
                          నిర్మాణం Curl_creds **pcreds);
#లేకపోతే
/* disabled */
#నిర్వచించు Curl_netrc_init(x)
#నిర్వచించు Curl_netrc_cleanup(x)
#అయితే_ముగింపు

#అయితే_ముగింపు /* HEADER_CURL_NETRC_H */
