#اگر_غیر_تعریف_شدہ سرخی_CURL_MULTI_NTFY_H
#تعریف_کرو سرخی_CURL_MULTI_NTFY_H
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
#شامل "curl_setup.h"

#تعریف_کرو CURL_MNTFY_FLAG_HAS_ENTRIES  ((uint32_t)1 << 31)

ساخت Curl_easy;
ساخت Curl_multi;

ساخت curl_multi_ntfy {
  curl_notify_callback ntfy_cb;
  خالی *ntfy_cb_ڈیٹا;
  ساخت mntfy_chunk *سر;
  ساخت mntfy_chunk *دم;
  uint32_t flags;
  CURLMcode failure;
};

خالی Curl_mntfy_init(ساخت Curl_multi *multi);
خالی Curl_mntfy_cleanup(ساخت Curl_multi *multi);

CURLMcode Curl_mntfy_enable(ساخت Curl_multi *multi, غیرعلامتی صحیح type);
CURLMcode Curl_mntfy_disable(ساخت Curl_multi *multi, غیرعلامتی صحیح type);

خالی Curl_mntfy_جوڑو(ساخت Curl_easy *ڈیٹا, غیرعلامتی صحیح type);

#تعریف_کرو CURLM_NTFY(d, t)                              \
  کرو {                                                \
    اگر((d) && (d)->multi && (d)->multi->ntfy.ntfy_cb) \
      Curl_mntfy_جوڑو((d), (t));                       \
  } جب(0)

#تعریف_کرو CURL_MNTFY_HAS_ENTRIES(m) \
  ((m)->ntfy.flags & CURL_MNTFY_FLAG_HAS_ENTRIES)

CURLMcode Curl_mntfy_dispatch_all(ساخت Curl_multi *multi);

#اگر_ختم /* HEADER_CURL_MULTI_NTFY_H */
