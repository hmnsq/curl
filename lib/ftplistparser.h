#اگر_غیر_تعریف_شدہ سرخی_CURL_FTPLISTPARSER_H
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
#شامل "curl_setup.h"

#اگر_غیر_تعریف_شدہ CURL_DISABLE_FTP

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(ساخت Curl_easy *ڈیٹا);

/* WRITEFUNCTION callback for parsing LIST responses */
size_t Curl_ftp_parselist(حرف *بفر, size_t سائز, size_t nmemb,
                          خالی *connptr);

ساخت ftp_parselist_ڈیٹا; /* defined inside ftplibparser.c */

CURLcode Curl_ftp_parselist_geterror(ساخت ftp_parselist_ڈیٹا *pl_ڈیٹا);

ساخت ftp_parselist_ڈیٹا *Curl_ftp_parselist_ڈیٹا_alloc(خالی);

خالی Curl_ftp_parselist_ڈیٹا_free(ساخت ftp_parselist_ڈیٹا **parserp);

/* list of wildcard process states */
قسمتعریف شمار {
  CURLWC_CLEAR = 0,
  CURLWC_INIT = 1,
  CURLWC_MATCHING, /* library is trying to get list of addresses for
                      downloading */
  CURLWC_DOWNLOADING,
  CURLWC_CLEAN, /* deallocate resources and reset settings */
  CURLWC_SKIP,  /* skip over concrete file */
  CURLWC_ERROR, /* error cases */
  CURLWC_مکمل   /* if is wildcard->state == CURLWC_DONE wildcard loop
                   will end */
} wildcard_حالتیں;

قسمتعریف خالی (*wildcard_dtor)(خالی *ptr);

/* struct keeping information about wildcard download process */
ساخت Wildcard_ڈیٹا {
  حرف *راستہ; /* path to the directory, where we trying wildcard-match */
  حرف *نمونہ; /* wildcard pattern */
  ساخت Curl_llist filelist; /* llist with struct Curl_fileinfo */
  ساخت ftp_wc *ftpwc; /* pointer to FTP wildcard data */
  wildcard_dtor dtor;
  غیرعلامتی حرف state; /* wildcard_states */
};

خالی Curl_wildcard_init(ساخت Wildcard_ڈیٹا *wc);
خالی Curl_wildcard_dtor(ساخت Wildcard_ڈیٹا **wcp);

ساخت Curl_easy;

#ورنہ /* CURL_DISABLE_FTP */
#تعریف_کرو Curl_ftp_listmode(x) FALSE
#تعریف_کرو Curl_ftp_parselist ZERO_NULL
#تعریف_کرو Curl_wildcard_dtor(x)
#اگر_ختم /* !CURL_DISABLE_FTP */

#اگر_ختم /* HEADER_CURL_FTPLISTPARSER_H */
