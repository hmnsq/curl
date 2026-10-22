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
#شامل "curl_setup.h"

#ifndef CURL_DISABLE_FTP

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(ساختار Curl_easy *data);

/* WRITEFUNCTION callback for parsing LIST responses */
size_t Curl_ftp_parselist(نویسه *buffer, size_t size, size_t nmemb,
                          تهی *connptr);

ساختار ftp_parselist_data; /* defined inside ftplibparser.c */

CURLcode Curl_ftp_parselist_geterror(ساختار ftp_parselist_data *pl_data);

ساختار ftp_parselist_data *Curl_ftp_parselist_data_alloc(تهی);

تهی Curl_ftp_parselist_data_free(ساختار ftp_parselist_data **parserp);

/* list of wildcard process states */
تعریفنوع شمارش {
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

تعریفنوع تهی (*wildcard_dtor)(تهی *ptr);

/* struct keeping information about wildcard download process */
ساختار WildcardData {
  نویسه *path; /* path to the directory, where we trying wildcard-match */
  نویسه *pattern; /* wildcard pattern */
  ساختار Curl_llist filelist; /* llist with struct Curl_fileinfo */
  ساختار ftp_wc *ftpwc; /* pointer to FTP wildcard data */
  wildcard_dtor dtor;
  unsigned نویسه state; /* wildcard_states */
};

تهی Curl_wildcard_init(ساختار WildcardData *wc);
تهی Curl_wildcard_dtor(ساختار WildcardData **wcp);

ساختار Curl_easy;

#وگرنه /* CURL_DISABLE_FTP */
#define Curl_ftp_listmode(x) FALSE
#define Curl_ftp_parselist ZERO_NULL
#define Curl_wildcard_dtor(x)
#endif /* !CURL_DISABLE_FTP */

#endif /* HEADER_CURL_FTPLISTPARSER_H */
