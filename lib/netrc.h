#अगर_अघोषित HEADER_CURL_NETRC_H
#घोषणा HEADER_CURL_NETRC_H
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
#समावेश "curl_setup.h"

#अगर_अघोषित CURL_DISABLE_NETRC

#समावेश "curlx/dynbuf.h"

काष्ठा Curl_easy;
काष्ठा Curl_creds;

काष्ठा store_netrc {
  काष्ठा dynbuf filebuf;
  अक्षर *filename;
  BIT(loaded);
};

प्रकार क्रमागत {
  NETRC_OK,
  NETRC_NO_MATCH,      /* no matching entry in the file */
  NETRC_SYNTAX_ERROR,  /* in the netrc file */
  NETRC_FILE_MISSING,  /* the netrc file does not exist */
  NETRC_OUT_OF_MEMORY, /* while parsing netrc */
  NETRC_LAST /* never used */
} NETRCcode;

स्थिर अक्षर *Curl_netrc_strerror(NETRCcode ret);
व्योम Curl_netrc_init(काष्ठा store_netrc *store);
व्योम Curl_netrc_cleanup(काष्ठा store_netrc *store);

/* Scan a netrc file for credentials matching hostname
 * and optional user. */
NETRCcode Curl_netrc_scan(काष्ठा Curl_easy *तथ्य,
                          काष्ठा store_netrc *store,
                          स्थिर अक्षर *hostname,
                          स्थिर अक्षर *user,
                          स्थिर अक्षर *netrcfile,
                          काष्ठा Curl_creds **pcreds);
#अन्यथा
/* disabled */
#घोषणा Curl_netrc_init(x)
#घोषणा Curl_netrc_cleanup(x)
#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURL_NETRC_H */
