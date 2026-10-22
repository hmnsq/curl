#अगर_अघोषित HEADER_CURL_MULTI_NTFY_H
#घोषणा HEADER_CURL_MULTI_NTFY_H
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

#घोषणा CURL_MNTFY_FLAG_HAS_ENTRIES  ((uint32_t)1 << 31)

काष्ठा Curl_easy;
काष्ठा Curl_multi;

काष्ठा curl_multi_ntfy {
  curl_notify_callback ntfy_cb;
  व्योम *ntfy_cb_तथ्य;
  काष्ठा mntfy_chunk *शीर्ष;
  काष्ठा mntfy_chunk *पुच्छ;
  uint32_t flags;
  CURLMcode failure;
};

व्योम Curl_mntfy_init(काष्ठा Curl_multi *multi);
व्योम Curl_mntfy_cleanup(काष्ठा Curl_multi *multi);

CURLMcode Curl_mntfy_enable(काष्ठा Curl_multi *multi, अचिन्हित पूर्णांक type);
CURLMcode Curl_mntfy_disable(काष्ठा Curl_multi *multi, अचिन्हित पूर्णांक type);

व्योम Curl_mntfy_add(काष्ठा Curl_easy *तथ्य, अचिन्हित पूर्णांक type);

#घोषणा CURLM_NTFY(d, t)                              \
  करो {                                                \
    अगर((d) && (d)->multi && (d)->multi->ntfy.ntfy_cb) \
      Curl_mntfy_add((d), (t));                       \
  } जबतक(0)

#घोषणा CURL_MNTFY_HAS_ENTRIES(m) \
  ((m)->ntfy.flags & CURL_MNTFY_FLAG_HAS_ENTRIES)

CURLMcode Curl_mntfy_dispatch_all(काष्ठा Curl_multi *multi);

#पूर्ण_अगर /* HEADER_CURL_MULTI_NTFY_H */
