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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#define CURL_MNTFY_FLAG_HAS_ENTRIES  ((uint32_t)1 << 31)

ସଂରଚନା Curl_easy;
ସଂରଚନା Curl_multi;

ସଂରଚନା curl_multi_ntfy {
  curl_notify_callback ntfy_cb;
  ଶୂନ୍ୟ *ntfy_cb_data;
  ସଂରଚନା mntfy_chunk *head;
  ସଂରଚନା mntfy_chunk *tail;
  uint32_t flags;
  CURLMcode failure;
};

ଶୂନ୍ୟ Curl_mntfy_init(ସଂରଚନା Curl_multi *multi);
ଶୂନ୍ୟ Curl_mntfy_cleanup(ସଂରଚନା Curl_multi *multi);

CURLMcode Curl_mntfy_enable(ସଂରଚନା Curl_multi *multi, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା type);
CURLMcode Curl_mntfy_disable(ସଂରଚନା Curl_multi *multi, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା type);

ଶୂନ୍ୟ Curl_mntfy_add(ସଂରଚନା Curl_easy *data, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା type);

#define CURLM_NTFY(d, t)                              \
  କର {                                                \
    ଯଦି((d) && (d)->multi && (d)->multi->ntfy.ntfy_cb) \
      Curl_mntfy_add((d), (t));                       \
  } ଯେତେବେଳେ(0)

#define CURL_MNTFY_HAS_ENTRIES(m) \
  ((m)->ntfy.flags & CURL_MNTFY_FLAG_HAS_ENTRIES)

CURLMcode Curl_mntfy_dispatch_all(ସଂରଚନା Curl_multi *multi);

#endif /* HEADER_CURL_MULTI_NTFY_H */
