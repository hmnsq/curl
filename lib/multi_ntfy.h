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
#অন্তর্ভাব "curl_setup.h"

#define CURL_MNTFY_FLAG_HAS_ENTRIES  ((uint32_t)1 << 31)

কাঠামো Curl_easy;
কাঠামো Curl_multi;

কাঠামো curl_multi_ntfy {
  curl_notify_callback ntfy_cb;
  শূন্য *ntfy_cb_তথ্য;
  কাঠামো mntfy_chunk *শীর্ষ;
  কাঠামো mntfy_chunk *লেজ;
  uint32_t flags;
  CURLMcode failure;
};

শূন্য Curl_mntfy_init(কাঠামো Curl_multi *multi);
শূন্য Curl_mntfy_cleanup(কাঠামো Curl_multi *multi);

CURLMcode Curl_mntfy_enable(কাঠামো Curl_multi *multi, unsigned পূর্ণসংখ্যা type);
CURLMcode Curl_mntfy_disable(কাঠামো Curl_multi *multi, unsigned পূর্ণসংখ্যা type);

শূন্য Curl_mntfy_add(কাঠামো Curl_easy *তথ্য, unsigned পূর্ণসংখ্যা type);

#define CURLM_NTFY(d, t)                              \
  করো {                                                \
    যদি((d) && (d)->multi && (d)->multi->ntfy.ntfy_cb) \
      Curl_mntfy_add((d), (t));                       \
  } যখন(0)

#define CURL_MNTFY_HAS_ENTRIES(m) \
  ((m)->ntfy.flags & CURL_MNTFY_FLAG_HAS_ENTRIES)

CURLMcode Curl_mntfy_dispatch_all(কাঠামো Curl_multi *multi);

#endif /* HEADER_CURL_MULTI_NTFY_H */
