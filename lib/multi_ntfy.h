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
#അന്തര്ഭാവ "curl_setup.h"

#define CURL_MNTFY_FLAG_HAS_ENTRIES  ((uint32_t)1 << 31)

ഘടന Curl_easy;
ഘടന Curl_multi;

ഘടന curl_multi_ntfy {
  curl_notify_callback ntfy_cb;
  ശൂന്യം *ntfy_cb_data;
  ഘടന mntfy_chunk *head;
  ഘടന mntfy_chunk *tail;
  uint32_t flags;
  CURLMcode failure;
};

ശൂന്യം Curl_mntfy_init(ഘടന Curl_multi *multi);
ശൂന്യം Curl_mntfy_cleanup(ഘടന Curl_multi *multi);

CURLMcode Curl_mntfy_enable(ഘടന Curl_multi *multi, unsigned പൂർണ്ണസംഖ്യ type);
CURLMcode Curl_mntfy_disable(ഘടന Curl_multi *multi, unsigned പൂർണ്ണസംഖ്യ type);

ശൂന്യം Curl_mntfy_add(ഘടന Curl_easy *data, unsigned പൂർണ്ണസംഖ്യ type);

#define CURLM_NTFY(d, t)                              \
  ചെയ്യുക {                                                \
    എങ്കിൽ((d) && (d)->multi && (d)->multi->ntfy.ntfy_cb) \
      Curl_mntfy_add((d), (t));                       \
  } ആകുമ്പോൾ(0)

#define CURL_MNTFY_HAS_ENTRIES(m) \
  ((m)->ntfy.flags & CURL_MNTFY_FLAG_HAS_ENTRIES)

CURLMcode Curl_mntfy_dispatch_all(ഘടന Curl_multi *multi);

#endif /* HEADER_CURL_MULTI_NTFY_H */
