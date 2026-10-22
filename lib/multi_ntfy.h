#که_نه_تعریف_شوی سرلیک_CURL_MULTI_NTFY_H
#تعریف_کړه سرلیک_CURL_MULTI_NTFY_H
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
#ضمن "curl_setup.h"

#تعریف_کړه CURL_MNTFY_FLAG_HAS_ENTRIES  ((uint32_t)1 << 31)

بنية Curl_easy;
بنية Curl_multi;

بنية curl_multi_ntfy {
  curl_notify_callback ntfy_cb;
  فراغ *ntfy_cb_معلومات;
  بنية mntfy_chunk *سر;
  بنية mntfy_chunk *لکۍ;
  uint32_t flags;
  CURLMcode failure;
};

فراغ Curl_mntfy_init(بنية Curl_multi *multi);
فراغ Curl_mntfy_cleanup(بنية Curl_multi *multi);

CURLMcode Curl_mntfy_enable(بنية Curl_multi *multi, بینښه صحيح type);
CURLMcode Curl_mntfy_disable(بنية Curl_multi *multi, بینښه صحيح type);

فراغ Curl_mntfy_جمع_کړه(بنية Curl_easy *معلومات, بینښه صحيح type);

#تعریف_کړه CURLM_NTFY(d, t)                              \
  افعل {                                                \
    إذا((d) && (d)->multi && (d)->multi->ntfy.ntfy_cb) \
      Curl_mntfy_جمع_کړه((d), (t));                       \
  } طالما(0)

#تعریف_کړه CURL_MNTFY_HAS_ENTRIES(m) \
  ((m)->ntfy.flags & CURL_MNTFY_FLAG_HAS_ENTRIES)

CURLMcode Curl_mntfy_dispatch_all(بنية Curl_multi *multi);

#د_که_پای /* HEADER_CURL_MULTI_NTFY_H */
