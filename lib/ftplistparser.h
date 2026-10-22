#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_FTPLISTPARSER_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_FTPLISTPARSER_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/* WRITEFUNCTION callback for parsing LIST responses */
size_t Curl_ftp_parselist(ਅੱਖਰ *ਬਫਰ, size_t ਮਾਪ, size_t nmemb,
                          ਖਾਲੀ *connptr);

ਬਣਤਰ ftp_parselist_ਅੰਕੜੇ; /* defined inside ftplibparser.c */

CURLcode Curl_ftp_parselist_geterror(ਬਣਤਰ ftp_parselist_ਅੰਕੜੇ *pl_ਅੰਕੜੇ);

ਬਣਤਰ ftp_parselist_ਅੰਕੜੇ *Curl_ftp_parselist_ਅੰਕੜੇ_alloc(ਖਾਲੀ);

ਖਾਲੀ Curl_ftp_parselist_ਅੰਕੜੇ_free(ਬਣਤਰ ftp_parselist_ਅੰਕੜੇ **parserp);

/* list of wildcard process states */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  CURLWC_CLEAR = 0,
  CURLWC_INIT = 1,
  CURLWC_MATCHING, /* library is trying to get list of addresses for
                      downloading */
  CURLWC_DOWNLOADING,
  CURLWC_CLEAN, /* deallocate resources and reset settings */
  CURLWC_SKIP,  /* skip over concrete file */
  CURLWC_ERROR, /* error cases */
  CURLWC_ਪੂਰਾ   /* if is wildcard->state == CURLWC_DONE wildcard loop
                   will end */
} wildcard_ਹਾਲਤਾਂ;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*wildcard_dtor)(ਖਾਲੀ *ptr);

/* struct keeping information about wildcard download process */
ਬਣਤਰ Wildcard_ਅੰਕੜੇ {
  ਅੱਖਰ *ਰਸਤਾ; /* path to the directory, where we trying wildcard-match */
  ਅੱਖਰ *ਨਮੂਨਾ; /* wildcard pattern */
  ਬਣਤਰ Curl_llist filelist; /* llist with struct Curl_fileinfo */
  ਬਣਤਰ ftp_wc *ftpwc; /* pointer to FTP wildcard data */
  wildcard_dtor dtor;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ state; /* wildcard_states */
};

ਖਾਲੀ Curl_wildcard_init(ਬਣਤਰ Wildcard_ਅੰਕੜੇ *wc);
ਖਾਲੀ Curl_wildcard_dtor(ਬਣਤਰ Wildcard_ਅੰਕੜੇ **wcp);

ਬਣਤਰ Curl_easy;

#ਨਹੀਂਤਾਂ /* CURL_DISABLE_FTP */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ftp_listmode(x) FALSE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ftp_parselist ZERO_NULL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_wildcard_dtor(x)
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_FTP */

#ਜੇ_ਅੰਤ /* HEADER_CURL_FTPLISTPARSER_H */
