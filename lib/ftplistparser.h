#ifndef HEADER_CURL_FTPLISTPARSER_H
#define HEADER_CURL_FTPLISTPARSER_H
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

#ifndef CURL_DISABLE_FTP

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(संरचना Curl_easy *data);

/* WRITEFUNCTION callback for parsing LIST responses */
माप_प्रकार Curl_ftp_parselist(अक्षर *buffer, माप_प्रकार size, माप_प्रकार nmemb,
                          शून्य *connptr);

संरचना ftp_parselist_data; /* defined inside ftplibparser.c */

CURLcode Curl_ftp_parselist_geterror(संरचना ftp_parselist_data *pl_data);

संरचना ftp_parselist_data *Curl_ftp_parselist_data_alloc(शून्य);

शून्य Curl_ftp_parselist_data_free(संरचना ftp_parselist_data **parserp);

/* list of wildcard process states */
प्रकारव्याख्या गणना {
  CURLWC_CLEAR = 0,
  CURLWC_INIT = 1,
  CURLWC_MATCHING, /* library is trying to get list of addresses for
                      downloading */
  CURLWC_DOWNLOADING,
  CURLWC_CLEAN, /* deallocate resources and reset settings */
  CURLWC_SKIP,  /* skip over concrete file */
  CURLWC_ERROR, /* error cases */
  CURLWC_DONE   /* if is wildcard->state == CURLWC_DONE wildcard loop
                   will end */
} wildcard_states;

प्रकारव्याख्या शून्य (*wildcard_dtor)(शून्य *ptr);

/* struct keeping information about wildcard download process */
संरचना WildcardData {
  अक्षर *path; /* path to the directory, where we trying wildcard-match */
  अक्षर *pattern; /* wildcard pattern */
  संरचना Curl_llist filelist; /* llist with struct Curl_fileinfo */
  संरचना ftp_wc *ftpwc; /* pointer to FTP wildcard data */
  wildcard_dtor dtor;
  अचिन्हित अक्षर state; /* wildcard_states */
};

शून्य Curl_wildcard_init(संरचना WildcardData *wc);
शून्य Curl_wildcard_dtor(संरचना WildcardData **wcp);

संरचना Curl_easy;

#अन्यथा /* CURL_DISABLE_FTP */
#define Curl_ftp_listmode(x) FALSE
#define Curl_ftp_parselist ZERO_NULL
#define Curl_wildcard_dtor(x)
#endif /* !CURL_DISABLE_FTP */

#endif /* HEADER_CURL_FTPLISTPARSER_H */
