#నిర్వచితం_కాకపోతే శీర్షిక_CURL_FTPLISTPARSER_H
#నిర్వచించు శీర్షిక_CURL_FTPLISTPARSER_H
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

#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(నిర్మాణం Curl_easy *దత్తాంశం);

/* WRITEFUNCTION callback for parsing LIST responses */
size_t Curl_ftp_parselist(అక్షరం *బఫరు, size_t కొలత, size_t nmemb,
                          శూన్య *connptr);

నిర్మాణం ftp_parselist_దత్తాంశం; /* defined inside ftplibparser.c */

CURLcode Curl_ftp_parselist_geterror(నిర్మాణం ftp_parselist_దత్తాంశం *pl_దత్తాంశం);

నిర్మాణం ftp_parselist_దత్తాంశం *Curl_ftp_parselist_దత్తాంశం_alloc(శూన్య);

శూన్య Curl_ftp_parselist_దత్తాంశం_free(నిర్మాణం ftp_parselist_దత్తాంశం **parserp);

/* list of wildcard process states */
రకనిర్వచనం గణన {
  CURLWC_CLEAR = 0,
  CURLWC_INIT = 1,
  CURLWC_MATCHING, /* library is trying to get list of addresses for
                      downloading */
  CURLWC_DOWNLOADING,
  CURLWC_CLEAN, /* deallocate resources and reset settings */
  CURLWC_SKIP,  /* skip over concrete file */
  CURLWC_ERROR, /* error cases */
  CURLWC_పూర్తి   /* if is wildcard->state == CURLWC_DONE wildcard loop
                   will end */
} wildcard_స్థితులు;

రకనిర్వచనం శూన్య (*wildcard_dtor)(శూన్య *ptr);

/* struct keeping information about wildcard download process */
నిర్మాణం Wildcard_దత్తాంశం {
  అక్షరం *మార్గం; /* path to the directory, where we trying wildcard-match */
  అక్షరం *నమూనా; /* wildcard pattern */
  నిర్మాణం Curl_llist filelist; /* llist with struct Curl_fileinfo */
  నిర్మాణం ftp_wc *ftpwc; /* pointer to FTP wildcard data */
  wildcard_dtor dtor;
  గుర్తులేని అక్షరం state; /* wildcard_states */
};

శూన్య Curl_wildcard_init(నిర్మాణం Wildcard_దత్తాంశం *wc);
శూన్య Curl_wildcard_dtor(నిర్మాణం Wildcard_దత్తాంశం **wcp);

నిర్మాణం Curl_easy;

#లేకపోతే /* CURL_DISABLE_FTP */
#నిర్వచించు Curl_ftp_listmode(x) FALSE
#నిర్వచించు Curl_ftp_parselist ZERO_NULL
#నిర్వచించు Curl_wildcard_dtor(x)
#అయితే_ముగింపు /* !CURL_DISABLE_FTP */

#అయితే_ముగింపు /* HEADER_CURL_FTPLISTPARSER_H */
