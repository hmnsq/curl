#यदि_अपरिभाषितम् शीर्षकम्_CURL_FTPLISTPARSER_H
#परिभाषय शीर्षकम्_CURL_FTPLISTPARSER_H
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

#यदि_अपरिभाषितम् CURL_DISABLE_FTP

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(संरचना Curl_easy *दत्त);

/* WRITEFUNCTION callback for parsing LIST responses */
size_t Curl_ftp_parselist(वर्ण *बफर, size_t आकार, size_t nmemb,
                          शून्य *connptr);

संरचना ftp_parselist_दत्त; /* defined inside ftplibparser.c */

CURLcode Curl_ftp_parselist_geterror(संरचना ftp_parselist_दत्त *pl_दत्त);

संरचना ftp_parselist_दत्त *Curl_ftp_parselist_दत्त_alloc(शून्य);

शून्य Curl_ftp_parselist_दत्त_free(संरचना ftp_parselist_दत्त **parserp);

/* list of wildcard process states */
प्रकारनिर्देश गणना {
  CURLWC_CLEAR = 0,
  CURLWC_INIT = 1,
  CURLWC_MATCHING, /* library is trying to get list of addresses for
                      downloading */
  CURLWC_DOWNLOADING,
  CURLWC_CLEAN, /* deallocate resources and reset settings */
  CURLWC_SKIP,  /* skip over concrete file */
  CURLWC_ERROR, /* error cases */
  CURLWC_समाप्तम्   /* if is wildcard->state == CURLWC_DONE wildcard loop
                   will end */
} wildcard_अवस्थाः;

प्रकारनिर्देश शून्य (*wildcard_dtor)(शून्य *ptr);

/* struct keeping information about wildcard download process */
संरचना Wildcard_दत्त {
  वर्ण *मार्गः; /* path to the directory, where we trying wildcard-match */
  वर्ण *आदर्शः; /* wildcard pattern */
  संरचना Curl_llist filelist; /* llist with struct Curl_fileinfo */
  संरचना ftp_wc *ftpwc; /* pointer to FTP wildcard data */
  wildcard_dtor dtor;
  अचिह्नित वर्ण state; /* wildcard_states */
};

शून्य Curl_wildcard_init(संरचना Wildcard_दत्त *wc);
शून्य Curl_wildcard_dtor(संरचना Wildcard_दत्त **wcp);

संरचना Curl_easy;

#अन्यथा /* CURL_DISABLE_FTP */
#परिभाषय Curl_ftp_listmode(x) FALSE
#परिभाषय Curl_ftp_parselist ZERO_NULL
#परिभाषय Curl_wildcard_dtor(x)
#यदि_अन्तः /* !CURL_DISABLE_FTP */

#यदि_अन्तः /* HEADER_CURL_FTPLISTPARSER_H */
