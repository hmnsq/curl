#ifndef HEADER_CURL_MULTI_NTFY_H
#define HEADER_CURL_MULTI_NTFY_H
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

#define CURL_MNTFY_FLAG_HAS_ENTRIES  ((uint32_t)1 << 31)

સંરચના Curl_easy;
સંરચના Curl_multi;

સંરચના curl_multi_ntfy {
  curl_notify_callback ntfy_cb;
  શૂન્ય *ntfy_cb_data;
  સંરચના mntfy_chunk *head;
  સંરચના mntfy_chunk *tail;
  uint32_t flags;
  CURLMcode failure;
};

શૂન્ય Curl_mntfy_init(સંરચના Curl_multi *multi);
શૂન્ય Curl_mntfy_cleanup(સંરચના Curl_multi *multi);

CURLMcode Curl_mntfy_enable(સંરચના Curl_multi *multi, unsigned પૂર્ણાંક type);
CURLMcode Curl_mntfy_disable(સંરચના Curl_multi *multi, unsigned પૂર્ણાંક type);

શૂન્ય Curl_mntfy_add(સંરચના Curl_easy *data, unsigned પૂર્ણાંક type);

#define CURLM_NTFY(d, t)                              \
  કરો {                                                \
    જો((d) && (d)->multi && (d)->multi->ntfy.ntfy_cb) \
      Curl_mntfy_add((d), (t));                       \
  } જ્યારે(0)

#define CURL_MNTFY_HAS_ENTRIES(m) \
  ((m)->ntfy.flags & CURL_MNTFY_FLAG_HAS_ENTRIES)

CURLMcode Curl_mntfy_dispatch_all(સંરચના Curl_multi *multi);

#endif /* HEADER_CURL_MULTI_NTFY_H */
