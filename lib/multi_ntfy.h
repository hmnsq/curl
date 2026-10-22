#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_MULTI_NTFY_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_MULTI_NTFY_H
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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_MNTFY_FLAG_HAS_ENTRIES  ((uint32_t)1 << 31)

ਬਣਤਰ Curl_easy;
ਬਣਤਰ Curl_multi;

ਬਣਤਰ curl_multi_ntfy {
  curl_notify_callback ntfy_cb;
  ਖਾਲੀ *ntfy_cb_ਅੰਕੜੇ;
  ਬਣਤਰ mntfy_chunk *ਸਿਰ;
  ਬਣਤਰ mntfy_chunk *ਪੂਛ;
  uint32_t flags;
  CURLMcode failure;
};

ਖਾਲੀ Curl_mntfy_init(ਬਣਤਰ Curl_multi *multi);
ਖਾਲੀ Curl_mntfy_cleanup(ਬਣਤਰ Curl_multi *multi);

CURLMcode Curl_mntfy_enable(ਬਣਤਰ Curl_multi *multi, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ type);
CURLMcode Curl_mntfy_disable(ਬਣਤਰ Curl_multi *multi, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ type);

ਖਾਲੀ Curl_mntfy_ਜੋੜੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ type);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLM_NTFY(d, t)                              \
  ਕਰੋ {                                                \
    ਜੇ((d) && (d)->multi && (d)->multi->ntfy.ntfy_cb) \
      Curl_mntfy_ਜੋੜੋ((d), (t));                       \
  } ਜਦੋਂ(0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_MNTFY_HAS_ENTRIES(m) \
  ((m)->ntfy.flags & CURL_MNTFY_FLAG_HAS_ENTRIES)

CURLMcode Curl_mntfy_dispatch_all(ਬਣਤਰ Curl_multi *multi);

#ਜੇ_ਅੰਤ /* HEADER_CURL_MULTI_NTFY_H */
