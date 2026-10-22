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
#അന്തര്ഭാവ "curl_setup.h"

#ifndef CURL_DISABLE_FTP

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(ഘടന Curl_easy *data);

/* WRITEFUNCTION callback for parsing LIST responses */
size_t Curl_ftp_parselist(അക്ഷരം *buffer, size_t size, size_t nmemb,
                          ശൂന്യം *connptr);

ഘടന ftp_parselist_data; /* defined inside ftplibparser.c */

CURLcode Curl_ftp_parselist_geterror(ഘടന ftp_parselist_data *pl_data);

ഘടന ftp_parselist_data *Curl_ftp_parselist_data_alloc(ശൂന്യം);

ശൂന്യം Curl_ftp_parselist_data_free(ഘടന ftp_parselist_data **parserp);

/* list of wildcard process states */
തരനിർവചനം എണ്ണൽ {
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

തരനിർവചനം ശൂന്യം (*wildcard_dtor)(ശൂന്യം *ptr);

/* struct keeping information about wildcard download process */
ഘടന WildcardData {
  അക്ഷരം *path; /* path to the directory, where we trying wildcard-match */
  അക്ഷരം *pattern; /* wildcard pattern */
  ഘടന Curl_llist filelist; /* llist with struct Curl_fileinfo */
  ഘടന ftp_wc *ftpwc; /* pointer to FTP wildcard data */
  wildcard_dtor dtor;
  unsigned അക്ഷരം state; /* wildcard_states */
};

ശൂന്യം Curl_wildcard_init(ഘടന WildcardData *wc);
ശൂന്യം Curl_wildcard_dtor(ഘടന WildcardData **wcp);

ഘടന Curl_easy;

#അല്ലെങ്കിൽ /* CURL_DISABLE_FTP */
#define Curl_ftp_listmode(x) FALSE
#define Curl_ftp_parselist ZERO_NULL
#define Curl_wildcard_dtor(x)
#endif /* !CURL_DISABLE_FTP */

#endif /* HEADER_CURL_FTPLISTPARSER_H */
