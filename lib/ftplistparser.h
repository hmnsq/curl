#جے_بنا_تعریف سرخی_CURL_FTPLISTPARSER_H
#تعریف_کرو سرخی_CURL_FTPLISTPARSER_H
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

#جے_بنا_تعریف CURL_DISABLE_FTP

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(بنية Curl_easy *انکڑے);

/* WRITEFUNCTION callback for parsing LIST responses */
size_t Curl_ftp_parselist(حرف *بفر, size_t ناپ, size_t nmemb,
                          فراغ *connptr);

بنية ftp_parselist_انکڑے; /* defined inside ftplibparser.c */

CURLcode Curl_ftp_parselist_geterror(بنية ftp_parselist_انکڑے *pl_انکڑے);

بنية ftp_parselist_انکڑے *Curl_ftp_parselist_انکڑے_alloc(فراغ);

فراغ Curl_ftp_parselist_انکڑے_free(بنية ftp_parselist_انکڑے **parserp);

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
  CURLWC_پورا   /* if is wildcard->state == CURLWC_DONE wildcard loop
                   will end */
} wildcard_حالتاں;

تعريفنوع فراغ (*wildcard_dtor)(فراغ *ptr);

/* struct keeping information about wildcard download process */
بنية Wildcard_انکڑے {
  حرف *رستہ; /* path to the directory, where we trying wildcard-match */
  حرف *نمونہ; /* wildcard pattern */
  بنية Curl_llist filelist; /* llist with struct Curl_fileinfo */
  بنية ftp_wc *ftpwc; /* pointer to FTP wildcard data */
  wildcard_dtor dtor;
  بنانشانی حرف state; /* wildcard_states */
};

فراغ Curl_wildcard_init(بنية Wildcard_انکڑے *wc);
فراغ Curl_wildcard_dtor(بنية Wildcard_انکڑے **wcp);

بنية Curl_easy;

#وإلا /* CURL_DISABLE_FTP */
#تعریف_کرو Curl_ftp_listmode(x) FALSE
#تعریف_کرو Curl_ftp_parselist ZERO_NULL
#تعریف_کرو Curl_wildcard_dtor(x)
#جے_انت /* !CURL_DISABLE_FTP */

#جے_انت /* HEADER_CURL_FTPLISTPARSER_H */
