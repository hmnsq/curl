#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_MULTI_NTFY_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_MULTI_NTFY_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_MNTFY_FLAG_HAS_ENTRIES  ((uint32_t)1 << 31)

ರಚನೆ Curl_easy;
ರಚನೆ Curl_multi;

ರಚನೆ curl_multi_ntfy {
  curl_notify_callback ntfy_cb;
  ಶೂನ್ಯ *ntfy_cb_ದತ್ತಾಂಶ;
  ರಚನೆ mntfy_chunk *ತಲೆ;
  ರಚನೆ mntfy_chunk *ಬಾಲ;
  uint32_t flags;
  CURLMcode failure;
};

ಶೂನ್ಯ Curl_mntfy_init(ರಚನೆ Curl_multi *multi);
ಶೂನ್ಯ Curl_mntfy_cleanup(ರಚನೆ Curl_multi *multi);

CURLMcode Curl_mntfy_enable(ರಚನೆ Curl_multi *multi, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ type);
CURLMcode Curl_mntfy_disable(ರಚನೆ Curl_multi *multi, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ type);

ಶೂನ್ಯ Curl_mntfy_ಕೂಡಿಸು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ type);

#ವ್ಯಾಖ್ಯಾನಿಸು CURLM_NTFY(d, t)                              \
  ಮಾಡು {                                                \
    ಆದರೆ((d) && (d)->multi && (d)->multi->ntfy.ntfy_cb) \
      Curl_mntfy_ಕೂಡಿಸು((d), (t));                       \
  } ಆಗ(0)

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_MNTFY_HAS_ENTRIES(m) \
  ((m)->ntfy.flags & CURL_MNTFY_FLAG_HAS_ENTRIES)

CURLMcode Curl_mntfy_dispatch_all(ರಚನೆ Curl_multi *multi);

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_MULTI_NTFY_H */
