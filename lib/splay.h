#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_SPLAY_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_SPLAY_H
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

#ಅನ್ತರ್ಭಾವ "curlx/timeval.h"

ರಚನೆ Curl_easy;

/* only use function calls to access this struct */
ರಚನೆ Curl_ವೃಕ್ಷ {
  ರಚನೆ Curl_ವೃಕ್ಷ *smaller; /* smaller node */
  ರಚನೆ Curl_ವೃಕ್ಷ *larger;  /* larger node */
  ರಚನೆ Curl_ವೃಕ್ಷ *same;    /* points to the next node with identical key */
  timediff_t ಕೀಲಿ;            /* this node's "sort" key */
  uint32_t id;               /* provided id for this node */
  ಬಿಟ್(registered);           /* node is registered in splay tree */
};

ರಚನೆ Curl_timeouts {
  ರಚನೆ Curl_ವೃಕ್ಷ *ವೃಕ್ಷ;
  time_t ಸಮಯ_ತಳ_sec;
};

ಶೂನ್ಯ Curl_timeouts_init(ರಚನೆ Curl_timeouts *timeouts,
                        ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *ptime_ತಳ);

bool Curl_timeouts_has(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

timediff_t Curl_timeouts_ಸ್ಥಾನಾಂತರ_us(ರಚನೆ Curl_timeouts *timeouts,
                                   ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pts);

ಪೂರ್ಣಾಂಕ Curl_timeouts_ಮುಂದಿನ_ms(ರಚನೆ Curl_timeouts *timeouts,
                          ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow,
                          timediff_t *pexpire_ಸ್ಥಾನಾಂತರ_us,
                          uint32_t *pmid);

bool Curl_timeouts_remove_expired(ರಚನೆ Curl_timeouts *timeouts,
                                  ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *ts,
                                  uint32_t *pmid);

ಶೂನ್ಯ Curl_timeouts_ಕೂಡಿಸು(ರಚನೆ Curl_timeouts *timeouts,
                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                       timediff_t ಸ್ಥಾನಾಂತರ_us);

/* Returns TRUE if data was registered in timeouts before */
bool Curl_timeouts_remove(ರಚನೆ Curl_timeouts *timeouts,
                          ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNITTESTS
uint32_t Curl_splayget(ರಚನೆ Curl_ವೃಕ್ಷ *ಗಂಟು);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_SPLAY_H */
