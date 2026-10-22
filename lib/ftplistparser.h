#اگر_تعریف_نشده سرآیند_CURL_FTPLISTPARSER_H
#تعریف_کن سرآیند_CURL_FTPLISTPARSER_H
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
#ضمن "curl_setup.h"

#اگر_تعریف_نشده CURL_DISABLE_FTP

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(بنية Curl_easy *داده);

/* WRITEFUNCTION callback for parsing LIST responses */
size_t Curl_ftp_parselist(حرف *میانگیر, size_t اندازه, size_t nmemb,
                          فراغ *connptr);

بنية ftp_parselist_داده; /* defined inside ftplibparser.c */

CURLcode Curl_ftp_parselist_geterror(بنية ftp_parselist_داده *pl_داده);

بنية ftp_parselist_داده *Curl_ftp_parselist_داده_alloc(فراغ);

فراغ Curl_ftp_parselist_داده_free(بنية ftp_parselist_داده **parserp);

/* list of wildcard process states */
تعريفنوع تعداد {
  CURLWC_CLEAR = 0,
  CURLWC_INIT = 1,
  CURLWC_MATCHING, /* library is trying to get list of addresses for
                      downloading */
  CURLWC_DOWNLOADING,
  CURLWC_CLEAN, /* deallocate resources and reset settings */
  CURLWC_SKIP,  /* skip over concrete file */
  CURLWC_ERROR, /* error cases */
  CURLWC_انجامشده   /* if is wildcard->state == CURLWC_DONE wildcard loop
                   will end */
} wildcard_حالتها;

تعريفنوع فراغ (*wildcard_dtor)(فراغ *ptr);

/* struct keeping information about wildcard download process */
بنية Wildcard_داده {
  حرف *مسیر; /* path to the directory, where we trying wildcard-match */
  حرف *الگو; /* wildcard pattern */
  بنية Curl_llist filelist; /* llist with struct Curl_fileinfo */
  بنية ftp_wc *ftpwc; /* pointer to FTP wildcard data */
  wildcard_dtor dtor;
  بیعلامت حرف state; /* wildcard_states */
};

فراغ Curl_wildcard_init(بنية Wildcard_داده *wc);
فراغ Curl_wildcard_dtor(بنية Wildcard_داده **wcp);

بنية Curl_easy;

#وإلا /* CURL_DISABLE_FTP */
#تعریف_کن Curl_ftp_listmode(x) FALSE
#تعریف_کن Curl_ftp_parselist ZERO_NULL
#تعریف_کن Curl_wildcard_dtor(x)
#پایان_اگر /* !CURL_DISABLE_FTP */

#پایان_اگر /* HEADER_CURL_FTPLISTPARSER_H */
