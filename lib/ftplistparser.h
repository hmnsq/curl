#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_FTPLISTPARSER_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_FTPLISTPARSER_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

/* WRITEFUNCTION callback for parsing LIST responses */
size_t Curl_ftp_parselist(ಅಕ್ಷರ *ಬಫರ್, size_t ಗಾತ್ರ_2, size_t nmemb,
                          ಶೂನ್ಯ *connptr);

ರಚನೆ ftp_parselist_ದತ್ತಾಂಶ; /* defined inside ftplibparser.c */

CURLcode Curl_ftp_parselist_geterror(ರಚನೆ ftp_parselist_ದತ್ತಾಂಶ *pl_ದತ್ತಾಂಶ);

ರಚನೆ ftp_parselist_ದತ್ತಾಂಶ *Curl_ftp_parselist_ದತ್ತಾಂಶ_alloc(ಶೂನ್ಯ);

ಶೂನ್ಯ Curl_ftp_parselist_ದತ್ತಾಂಶ_free(ರಚನೆ ftp_parselist_ದತ್ತಾಂಶ **parserp);

/* list of wildcard process states */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  CURLWC_CLEAR = 0,
  CURLWC_INIT = 1,
  CURLWC_MATCHING, /* library is trying to get list of addresses for
                      downloading */
  CURLWC_DOWNLOADING,
  CURLWC_CLEAN, /* deallocate resources and reset settings */
  CURLWC_SKIP,  /* skip over concrete file */
  CURLWC_ERROR, /* error cases */
  CURLWC_ಮುಗಿದ   /* if is wildcard->state == CURLWC_DONE wildcard loop
                   will end */
} wildcard_ಸ್ಥಿತಿಗಳು;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*wildcard_dtor)(ಶೂನ್ಯ *ptr);

/* struct keeping information about wildcard download process */
ರಚನೆ Wildcard_ದತ್ತಾಂಶ {
  ಅಕ್ಷರ *ಮಾರ್ಗ; /* path to the directory, where we trying wildcard-match */
  ಅಕ್ಷರ *ನಮೂನೆ; /* wildcard pattern */
  ರಚನೆ Curl_llist filelist; /* llist with struct Curl_fileinfo */
  ರಚನೆ ftp_wc *ftpwc; /* pointer to FTP wildcard data */
  wildcard_dtor dtor;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ state; /* wildcard_states */
};

ಶೂನ್ಯ Curl_wildcard_init(ರಚನೆ Wildcard_ದತ್ತಾಂಶ *wc);
ಶೂನ್ಯ Curl_wildcard_dtor(ರಚನೆ Wildcard_ದತ್ತಾಂಶ **wcp);

ರಚನೆ Curl_easy;

#ಇಲ್ಲದಿದ್ದರೆ /* CURL_DISABLE_FTP */
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ftp_listmode(x) FALSE
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ftp_parselist ZERO_NULL
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_wildcard_dtor(x)
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_FTP */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_FTPLISTPARSER_H */
