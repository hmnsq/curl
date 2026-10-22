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
#અન્તર્ભાવ "curl_setup.h"

#ifndef CURL_DISABLE_NETRC

#અન્તર્ભાવ "curlx/dynbuf.h"

સંરચના Curl_easy;
સંરચના Curl_creds;

સંરચના store_netrc {
  સંરચના dynbuf filebuf;
  અક્ષર *filename;
  BIT(loaded);
};

પ્રકારવ્યાખ્યા ગણના {
  NETRC_OK,
  NETRC_NO_MATCH,      /* no matching entry in the file */
  NETRC_SYNTAX_ERROR,  /* in the netrc file */
  NETRC_FILE_MISSING,  /* the netrc file does not exist */
  NETRC_OUT_OF_MEMORY, /* while parsing netrc */
  NETRC_LAST /* never used */
} NETRCcode;

અચલ અક્ષર *Curl_netrc_strerror(NETRCcode ret);
શૂન્ય Curl_netrc_init(સંરચના store_netrc *store);
શૂન્ય Curl_netrc_cleanup(સંરચના store_netrc *store);

/* Scan a netrc file for credentials matching hostname
 * and optional user. */
NETRCcode Curl_netrc_scan(સંરચના Curl_easy *data,
                          સંરચના store_netrc *store,
                          અચલ અક્ષર *hostname,
                          અચલ અક્ષર *user,
                          અચલ અક્ષર *netrcfile,
                          સંરચના Curl_creds **pcreds);
#નહીંતર
/* disabled */
#define Curl_netrc_init(x)
#define Curl_netrc_cleanup(x)
#endif

#endif /* HEADER_CURL_NETRC_H */
