#నిర్వచితం_కాకపోతే శీర్షిక_CURL_MULTI_NTFY_H
#నిర్వచించు శీర్షిక_CURL_MULTI_NTFY_H
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
#అన్తర్భావ "curl_setup.h"

#నిర్వచించు CURL_MNTFY_FLAG_HAS_ENTRIES  ((uint32_t)1 << 31)

నిర్మాణం Curl_easy;
నిర్మాణం Curl_multi;

నిర్మాణం curl_multi_ntfy {
  curl_notify_callback ntfy_cb;
  శూన్య *ntfy_cb_దత్తాంశం;
  నిర్మాణం mntfy_chunk *తల;
  నిర్మాణం mntfy_chunk *తోక;
  uint32_t flags;
  CURLMcode failure;
};

శూన్య Curl_mntfy_init(నిర్మాణం Curl_multi *multi);
శూన్య Curl_mntfy_cleanup(నిర్మాణం Curl_multi *multi);

CURLMcode Curl_mntfy_enable(నిర్మాణం Curl_multi *multi, గుర్తులేని పూర్ణాంకం type);
CURLMcode Curl_mntfy_disable(నిర్మాణం Curl_multi *multi, గుర్తులేని పూర్ణాంకం type);

శూన్య Curl_mntfy_కూడు(నిర్మాణం Curl_easy *దత్తాంశం, గుర్తులేని పూర్ణాంకం type);

#నిర్వచించు CURLM_NTFY(d, t)                              \
  చేయి {                                                \
    ఒకవేళ((d) && (d)->multi && (d)->multi->ntfy.ntfy_cb) \
      Curl_mntfy_కూడు((d), (t));                       \
  } అయితే(0)

#నిర్వచించు CURL_MNTFY_HAS_ENTRIES(m) \
  ((m)->ntfy.flags & CURL_MNTFY_FLAG_HAS_ENTRIES)

CURLMcode Curl_mntfy_dispatch_all(నిర్మాణం Curl_multi *multi);

#అయితే_ముగింపు /* HEADER_CURL_MULTI_NTFY_H */
