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
#અન્તર્ભાવ "curl_setup.h"

#ifndef CURL_DISABLE_FTP

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(સંરચના Curl_easy *data);

/* WRITEFUNCTION callback for parsing LIST responses */
size_t Curl_ftp_parselist(અક્ષર *buffer, size_t size, size_t nmemb,
                          શૂન્ય *connptr);

સંરચના ftp_parselist_data; /* defined inside ftplibparser.c */

CURLcode Curl_ftp_parselist_geterror(સંરચના ftp_parselist_data *pl_data);

સંરચના ftp_parselist_data *Curl_ftp_parselist_data_alloc(શૂન્ય);

શૂન્ય Curl_ftp_parselist_data_free(સંરચના ftp_parselist_data **parserp);

/* list of wildcard process states */
પ્રકારવ્યાખ્યા ગણના {
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

પ્રકારવ્યાખ્યા શૂન્ય (*wildcard_dtor)(શૂન્ય *ptr);

/* struct keeping information about wildcard download process */
સંરચના WildcardData {
  અક્ષર *path; /* path to the directory, where we trying wildcard-match */
  અક્ષર *pattern; /* wildcard pattern */
  સંરચના Curl_llist filelist; /* llist with struct Curl_fileinfo */
  સંરચના ftp_wc *ftpwc; /* pointer to FTP wildcard data */
  wildcard_dtor dtor;
  unsigned અક્ષર state; /* wildcard_states */
};

શૂન્ય Curl_wildcard_init(સંરચના WildcardData *wc);
શૂન્ય Curl_wildcard_dtor(સંરચના WildcardData **wcp);

સંરચના Curl_easy;

#નહીંતર /* CURL_DISABLE_FTP */
#define Curl_ftp_listmode(x) FALSE
#define Curl_ftp_parselist ZERO_NULL
#define Curl_wildcard_dtor(x)
#endif /* !CURL_DISABLE_FTP */

#endif /* HEADER_CURL_FTPLISTPARSER_H */
