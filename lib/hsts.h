#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_HSTS_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_HSTS_H
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

ರಚನೆ hsts;

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#ಅನ್ತರ್ಭಾವ "llist.h"

ರಚನೆ Curl_peer;

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_HSTS_ENTRIES 10000

#ಆದರೆ defined(DEBUGBUILD) || defined(UNITTESTS)
ಬಾಹ್ಯ time_t deltatime;
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ stsentry {
  ರಚನೆ Curl_llist_ಗಂಟು ಗಂಟು;
  curl_off_t expires; /* the timestamp of this entry's expiry */
  ಬಿಟ್(includeSubDomains);
  ಅಕ್ಷರ host[1];
};

/* The HSTS cache. Needs to be able to tailmatch hostnames. */
ರಚನೆ hsts {
  ರಚನೆ Curl_llist list;
  ಅಕ್ಷರ *filename;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags;
};

ರಚನೆ hsts *Curl_hsts_init(ಶೂನ್ಯ);
ಶೂನ್ಯ Curl_hsts_cleanup(ರಚನೆ hsts **hp);
CURLcode Curl_hsts_copy(ರಚನೆ hsts *dst, ರಚನೆ hsts *src);
CURLcode Curl_hsts_parse(ರಚನೆ hsts *h, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಶೀರ್ಷಿಕೆ);
CURLcode Curl_hsts_save(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ hsts *h,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file);
CURLcode Curl_hsts_loadfile(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ hsts *h, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file);
CURLcode Curl_hsts_loadcb(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ರಚನೆ hsts *h);
CURLcode Curl_hsts_loadfiles(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

bool Curl_hsts_applies(ರಚನೆ hsts *h, ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_peer *dest);

#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_hsts_cleanup(x)
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_hsts_loadcb(x, y) CURLE_OK
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_hsts_save(x, y, z)
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_hsts_loadfiles(x) CURLE_OK
#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_HSTS_H */
