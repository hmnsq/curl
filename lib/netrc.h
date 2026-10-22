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
#അന്തര്ഭാവ "curl_setup.h"

#ifndef CURL_DISABLE_NETRC

#അന്തര്ഭാവ "curlx/dynbuf.h"

ഘടന Curl_easy;
ഘടന Curl_creds;

ഘടന store_netrc {
  ഘടന dynbuf filebuf;
  അക്ഷരം *filename;
  BIT(loaded);
};

തരനിർവചനം എണ്ണൽ {
  NETRC_OK,
  NETRC_NO_MATCH,      /* no matching entry in the file */
  NETRC_SYNTAX_ERROR,  /* in the netrc file */
  NETRC_FILE_MISSING,  /* the netrc file does not exist */
  NETRC_OUT_OF_MEMORY, /* while parsing netrc */
  NETRC_LAST /* never used */
} NETRCcode;

സ്ഥിരം അക്ഷരം *Curl_netrc_strerror(NETRCcode ret);
ശൂന്യം Curl_netrc_init(ഘടന store_netrc *store);
ശൂന്യം Curl_netrc_cleanup(ഘടന store_netrc *store);

/* Scan a netrc file for credentials matching hostname
 * and optional user. */
NETRCcode Curl_netrc_scan(ഘടന Curl_easy *data,
                          ഘടന store_netrc *store,
                          സ്ഥിരം അക്ഷരം *hostname,
                          സ്ഥിരം അക്ഷരം *user,
                          സ്ഥിരം അക്ഷരം *netrcfile,
                          ഘടന Curl_creds **pcreds);
#അല്ലെങ്കിൽ
/* disabled */
#define Curl_netrc_init(x)
#define Curl_netrc_cleanup(x)
#endif

#endif /* HEADER_CURL_NETRC_H */
