#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_FTPLISTPARSER_H
#وصف_ڪريو سرو_CURL_FTPLISTPARSER_H
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

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_FTP

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(بنية Curl_easy *معلومات);

/* WRITEFUNCTION callback for parsing LIST responses */
size_t Curl_ftp_parselist(حرف *بفر, size_t سائيز, size_t nmemb,
                          فراغ *connptr);

بنية ftp_parselist_معلومات; /* defined inside ftplibparser.c */

CURLcode Curl_ftp_parselist_geterror(بنية ftp_parselist_معلومات *pl_معلومات);

بنية ftp_parselist_معلومات *Curl_ftp_parselist_معلومات_alloc(فراغ);

فراغ Curl_ftp_parselist_معلومات_free(بنية ftp_parselist_معلومات **parserp);

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
  CURLWC_پورو   /* if is wildcard->state == CURLWC_DONE wildcard loop
                   will end */
} wildcard_حالتون;

تعريفنوع فراغ (*wildcard_dtor)(فراغ *ptr);

/* struct keeping information about wildcard download process */
بنية Wildcard_معلومات {
  حرف *رستو; /* path to the directory, where we trying wildcard-match */
  حرف *نمونو; /* wildcard pattern */
  بنية Curl_llist filelist; /* llist with struct Curl_fileinfo */
  بنية ftp_wc *ftpwc; /* pointer to FTP wildcard data */
  wildcard_dtor dtor;
  بنانشان حرف state; /* wildcard_states */
};

فراغ Curl_wildcard_init(بنية Wildcard_معلومات *wc);
فراغ Curl_wildcard_dtor(بنية Wildcard_معلومات **wcp);

بنية Curl_easy;

#وإلا /* CURL_DISABLE_FTP */
#وصف_ڪريو Curl_ftp_listmode(x) FALSE
#وصف_ڪريو Curl_ftp_parselist ZERO_NULL
#وصف_ڪريو Curl_wildcard_dtor(x)
#جيڪڏهن_پورو /* !CURL_DISABLE_FTP */

#جيڪڏهن_پورو /* HEADER_CURL_FTPLISTPARSER_H */
