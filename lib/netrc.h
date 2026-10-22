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
#அந்தர்பாவ "curl_setup.h"

#ifndef CURL_DISABLE_NETRC

#அந்தர்பாவ "curlx/dynbuf.h"

கட்டமைப்பு Curl_easy;
கட்டமைப்பு Curl_creds;

கட்டமைப்பு store_netrc {
  கட்டமைப்பு dynbuf filebuf;
  எழுத்து *filename;
  BIT(loaded);
};

வகைவரை எண்ணிக்கை {
  NETRC_OK,
  NETRC_NO_MATCH,      /* no matching entry in the file */
  NETRC_SYNTAX_ERROR,  /* in the netrc file */
  NETRC_FILE_MISSING,  /* the netrc file does not exist */
  NETRC_OUT_OF_MEMORY, /* while parsing netrc */
  NETRC_LAST /* never used */
} NETRCcode;

மாறிலி எழுத்து *Curl_netrc_strerror(NETRCcode ret);
வெறுமை Curl_netrc_init(கட்டமைப்பு store_netrc *store);
வெறுமை Curl_netrc_cleanup(கட்டமைப்பு store_netrc *store);

/* Scan a netrc file for credentials matching hostname
 * and optional user. */
NETRCcode Curl_netrc_scan(கட்டமைப்பு Curl_easy *data,
                          கட்டமைப்பு store_netrc *store,
                          மாறிலி எழுத்து *hostname,
                          மாறிலி எழுத்து *user,
                          மாறிலி எழுத்து *netrcfile,
                          கட்டமைப்பு Curl_creds **pcreds);
#இல்லையெனில்
/* disabled */
#define Curl_netrc_init(x)
#define Curl_netrc_cleanup(x)
#endif

#endif /* HEADER_CURL_NETRC_H */
