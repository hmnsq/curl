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
#অন্তর্ভাব "curl_setup.h"

#ifndef CURL_DISABLE_FTP

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(কাঠামো Curl_easy *তথ্য);

/* WRITEFUNCTION callback for parsing LIST responses */
size_t Curl_ftp_parselist(অক্ষর *বাফার, size_t আকার_2, size_t nmemb,
                          শূন্য *connptr);

কাঠামো ftp_parselist_তথ্য; /* defined inside ftplibparser.c */

CURLcode Curl_ftp_parselist_geterror(কাঠামো ftp_parselist_তথ্য *pl_তথ্য);

কাঠামো ftp_parselist_তথ্য *Curl_ftp_parselist_তথ্য_alloc(শূন্য);

শূন্য Curl_ftp_parselist_তথ্য_free(কাঠামো ftp_parselist_তথ্য **parserp);

/* list of wildcard process states */
প্রকারলেখ গণনা {
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

প্রকারলেখ শূন্য (*wildcard_dtor)(শূন্য *ptr);

/* struct keeping information about wildcard download process */
কাঠামো Wildcard_তথ্য {
  অক্ষর *path; /* path to the directory, where we trying wildcard-match */
  অক্ষর *pattern; /* wildcard pattern */
  কাঠামো Curl_llist filelist; /* llist with struct Curl_fileinfo */
  কাঠামো ftp_wc *ftpwc; /* pointer to FTP wildcard data */
  wildcard_dtor dtor;
  unsigned অক্ষর state; /* wildcard_states */
};

শূন্য Curl_wildcard_init(কাঠামো Wildcard_তথ্য *wc);
শূন্য Curl_wildcard_dtor(কাঠামো Wildcard_তথ্য **wcp);

কাঠামো Curl_easy;

#অন্যথা /* CURL_DISABLE_FTP */
#define Curl_ftp_listmode(x) FALSE
#define Curl_ftp_parselist ZERO_NULL
#define Curl_wildcard_dtor(x)
#endif /* !CURL_DISABLE_FTP */

#endif /* HEADER_CURL_FTPLISTPARSER_H */
