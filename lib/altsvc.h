#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_ALTSVC_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_ALTSVC_H
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

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#ಅನ್ತರ್ಭಾವ "llist.h"

ರಚನೆ Curl_peer;

/* the maximum number of alt-svc entries kept in a single cache */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ALTSVC_ENTRIES 5000

ರಚನೆ althost {
  ಅಕ್ಷರ *host;
  uint16_t ಪೋರ್ಟ್;
  ಎಣಿಕೆ alpnid alpnid;
};

ರಚನೆ altsvc {
  ರಚನೆ althost src;
  ರಚನೆ althost dst;
  time_t expires;
  ರಚನೆ Curl_llist_ಗಂಟು ಗಂಟು;
  ಬಿಟ್(persist);
};

ರಚನೆ altsvcinfo {
  ಅಕ್ಷರ *filename;
  ರಚನೆ Curl_llist list; /* list of entries */
  ದೀರ್ಘ flags; /* the publicly set bitmask */
};

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Curl_alpnid2str(ಎಣಿಕೆ alpnid id);
ರಚನೆ altsvcinfo *Curl_altsvc_init(ಶೂನ್ಯ);
CURLcode Curl_altsvc_ತುಂಬು(ರಚನೆ altsvcinfo *asi, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file);
CURLcode Curl_altsvc_save(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ರಚನೆ altsvcinfo *asi, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file);
CURLcode Curl_altsvc_ctrl(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ದೀರ್ಘ ctrl);
ಶೂನ್ಯ Curl_altsvc_cleanup(ರಚನೆ altsvcinfo **asi);
CURLcode Curl_altsvc_parse(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ altsvcinfo *asi, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ,
                           ರಚನೆ Curl_peer *origin,
                           ಎಣಿಕೆ alpnid origin_alpnid);
bool Curl_altsvc_lookup(ರಚನೆ altsvcinfo *asi,
                        ರಚನೆ Curl_peer *origin,
                        ಎಣಿಕೆ alpnid origin_alpnid,
                        ರಚನೆ altsvc **dstentry,
                        ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ versions, /* CURLALTSVC_H* bits */
                        bool *psame_destination);
#ಇಲ್ಲದಿದ್ದರೆ
/* disabled */
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_altsvc_save(a, b, c)
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_altsvc_cleanup(x)
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_ALTSVC_H */
