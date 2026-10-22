#ifndef HEADER_CURL_NETRC_H
#define HEADER_CURL_NETRC_H
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

#ifndef CURL_DISABLE_NETRC

#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"

ಸಂರಚನಾ Curl_easy;
ಸಂರಚನಾ Curl_creds;

ಸಂರಚನಾ store_netrc {
  ಸಂರಚನಾ dynbuf filebuf;
  ವರ್ಣ *filename;
  BIT(loaded);
};

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  NETRC_OK,
  NETRC_NO_MATCH,      /* no matching entry in the file */
  NETRC_SYNTAX_ERROR,  /* in the netrc file */
  NETRC_FILE_MISSING,  /* the netrc file does not exist */
  NETRC_OUT_OF_MEMORY, /* while parsing netrc */
  NETRC_LAST /* never used */
} NETRCcode;

ಧ್ರುವ ವರ್ಣ *Curl_netrc_strerror(NETRCcode ret);
ಶೂನ್ಯ Curl_netrc_init(ಸಂರಚನಾ store_netrc *store);
ಶೂನ್ಯ Curl_netrc_cleanup(ಸಂರಚನಾ store_netrc *store);

/* Scan a netrc file for credentials matching hostname
 * and optional user. */
NETRCcode Curl_netrc_scan(ಸಂರಚನಾ Curl_easy *data,
                          ಸಂರಚನಾ store_netrc *store,
                          ಧ್ರುವ ವರ್ಣ *hostname,
                          ಧ್ರುವ ವರ್ಣ *user,
                          ಧ್ರುವ ವರ್ಣ *netrcfile,
                          ಸಂರಚನಾ Curl_creds **pcreds);
#ಅನ್ಯಥಾ
/* disabled */
#define Curl_netrc_init(x)
#define Curl_netrc_cleanup(x)
#endif

#endif /* HEADER_CURL_NETRC_H */
