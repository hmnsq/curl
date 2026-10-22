#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_NETRC_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_NETRC_H
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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_NETRC

#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/dynbuf.h"

ਬਣਤਰ Curl_easy;
ਬਣਤਰ Curl_creds;

ਬਣਤਰ store_netrc {
  ਬਣਤਰ dynbuf filebuf;
  ਅੱਖਰ *filename;
  ਦੋਆਧਾਰੀ_ਅੰਕ(loaded);
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  NETRC_OK,
  NETRC_NO_ਮੇਲ,      /* no matching entry in the file */
  NETRC_SYNTAX_ERROR,  /* in the netrc file */
  NETRC_FILE_MISSING,  /* the netrc file does not exist */
  NETRC_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ, /* while parsing netrc */
  NETRC_ਆਖਰੀ /* never used */
} NETRCcode;

ਅਚਲ ਅੱਖਰ *Curl_netrc_strerror(NETRCcode ret);
ਖਾਲੀ Curl_netrc_init(ਬਣਤਰ store_netrc *store);
ਖਾਲੀ Curl_netrc_cleanup(ਬਣਤਰ store_netrc *store);

/* Scan a netrc file for credentials matching hostname
 * and optional user. */
NETRCcode Curl_netrc_ਜਾਂਚ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਬਣਤਰ store_netrc *store,
                          ਅਚਲ ਅੱਖਰ *hostname,
                          ਅਚਲ ਅੱਖਰ *user,
                          ਅਚਲ ਅੱਖਰ *netrcfile,
                          ਬਣਤਰ Curl_creds **pcreds);
#ਨਹੀਂਤਾਂ
/* disabled */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_netrc_init(x)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_netrc_cleanup(x)
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HEADER_CURL_NETRC_H */
