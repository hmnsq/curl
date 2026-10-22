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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifndef CURL_DISABLE_NETRC

#ଅନ୍ତର୍ଭାଵ "curlx/dynbuf.h"

ସଂରଚନା Curl_easy;
ସଂରଚନା Curl_creds;

ସଂରଚନା store_netrc {
  ସଂରଚନା dynbuf filebuf;
  ଅକ୍ଷର *filename;
  BIT(loaded);
};

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  NETRC_OK,
  NETRC_NO_MATCH,      /* no matching entry in the file */
  NETRC_SYNTAX_ERROR,  /* in the netrc file */
  NETRC_FILE_MISSING,  /* the netrc file does not exist */
  NETRC_OUT_OF_MEMORY, /* while parsing netrc */
  NETRC_LAST /* never used */
} NETRCcode;

ଅଚଳ ଅକ୍ଷର *Curl_netrc_strerror(NETRCcode ret);
ଶୂନ୍ୟ Curl_netrc_init(ସଂରଚନା store_netrc *store);
ଶୂନ୍ୟ Curl_netrc_cleanup(ସଂରଚନା store_netrc *store);

/* Scan a netrc file for credentials matching hostname
 * and optional user. */
NETRCcode Curl_netrc_scan(ସଂରଚନା Curl_easy *data,
                          ସଂରଚନା store_netrc *store,
                          ଅଚଳ ଅକ୍ଷର *hostname,
                          ଅଚଳ ଅକ୍ଷର *user,
                          ଅଚଳ ଅକ୍ଷର *netrcfile,
                          ସଂରଚନା Curl_creds **pcreds);
#ଅନ୍ୟଥା
/* disabled */
#define Curl_netrc_init(x)
#define Curl_netrc_cleanup(x)
#endif

#endif /* HEADER_CURL_NETRC_H */
