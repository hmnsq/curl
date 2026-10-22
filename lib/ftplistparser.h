#अगर_अघोषित HEADER_CURL_FTPLISTPARSER_H
#घोषणा HEADER_CURL_FTPLISTPARSER_H
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

#अगर_अघोषित CURL_DISABLE_FTP

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(काष्ठा Curl_easy *तथ्य);

/* WRITEFUNCTION callback for parsing LIST responses */
माप_प्रकार Curl_ftp_parselist(अक्षर *बफर, माप_प्रकार आकार, माप_प्रकार nmemb,
                          व्योम *connptr);

काष्ठा ftp_parselist_तथ्य; /* defined inside ftplibparser.c */

CURLcode Curl_ftp_parselist_geterror(काष्ठा ftp_parselist_तथ्य *pl_तथ्य);

काष्ठा ftp_parselist_तथ्य *Curl_ftp_parselist_तथ्य_alloc(व्योम);

व्योम Curl_ftp_parselist_तथ्य_free(काष्ठा ftp_parselist_तथ्य **parserp);

/* list of wildcard process states */
प्रकार क्रमागत {
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

प्रकार व्योम (*wildcard_dtor)(व्योम *ptr);

/* struct keeping information about wildcard download process */
काष्ठा Wildcard_तथ्य {
  अक्षर *path; /* path to the directory, where we trying wildcard-match */
  अक्षर *pattern; /* wildcard pattern */
  काष्ठा Curl_llist filelist; /* llist with struct Curl_fileinfo */
  काष्ठा ftp_wc *ftpwc; /* pointer to FTP wildcard data */
  wildcard_dtor dtor;
  अचिन्हित अक्षर state; /* wildcard_states */
};

व्योम Curl_wildcard_init(काष्ठा Wildcard_तथ्य *wc);
व्योम Curl_wildcard_dtor(काष्ठा Wildcard_तथ्य **wcp);

काष्ठा Curl_easy;

#अन्यथा /* CURL_DISABLE_FTP */
#घोषणा Curl_ftp_listmode(x) FALSE
#घोषणा Curl_ftp_parselist ZERO_NULL
#घोषणा Curl_wildcard_dtor(x)
#पूर्ण_अगर /* !CURL_DISABLE_FTP */

#पूर्ण_अगर /* HEADER_CURL_FTPLISTPARSER_H */
