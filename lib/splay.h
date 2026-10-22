#ifndef HEADER_CURL_SPLAY_H
#define HEADER_CURL_SPLAY_H
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

ಸಂರಚನಾ Curl_easy;

/* only use function calls to access this struct */
ಸಂರಚನಾ Curl_ವೃಕ್ಷ {
  ಸಂರಚನಾ Curl_ವೃಕ್ಷ *smaller; /* smaller node */
  ಸಂರಚನಾ Curl_ವೃಕ್ಷ *larger;  /* larger node */
  ಸಂರಚನಾ Curl_ವೃಕ್ಷ *same;    /* points to the next node with identical key */
  timediff_t ಕೀಲಿ;            /* this node's "sort" key */
  uint32_t id;               /* provided id for this node */
  BIT(registered);           /* node is registered in splay tree */
};

ಸಂರಚನಾ Curl_timeouts {
  ಸಂರಚನಾ Curl_ವೃಕ್ಷ *ವೃಕ್ಷ;
  time_t time_base_sec;
};

ಶೂನ್ಯ Curl_timeouts_init(ಸಂರಚನಾ Curl_timeouts *timeouts,
                        ಧ್ರುವ ಸಂರಚನಾ curltime *ptime_base);

bool Curl_timeouts_has(ಸಂರಚನಾ Curl_easy *data);

timediff_t Curl_timeouts_offset_us(ಸಂರಚನಾ Curl_timeouts *timeouts,
                                   ಧ್ರುವ ಸಂರಚನಾ curltime *pts);

ಪೂರ್ಣಾಙ್ಕ Curl_timeouts_next_ms(ಸಂರಚನಾ Curl_timeouts *timeouts,
                          ಧ್ರುವ ಸಂರಚನಾ curltime *pnow,
                          timediff_t *pexpire_offset_us,
                          uint32_t *pmid);

bool Curl_timeouts_remove_expired(ಸಂರಚನಾ Curl_timeouts *timeouts,
                                  ಧ್ರುವ ಸಂರಚನಾ curltime *ts,
                                  uint32_t *pmid);

ಶೂನ್ಯ Curl_timeouts_add(ಸಂರಚನಾ Curl_timeouts *timeouts,
                       ಸಂರಚನಾ Curl_easy *data,
                       timediff_t offset_us);

/* Returns TRUE if data was registered in timeouts before */
bool Curl_timeouts_remove(ಸಂರಚನಾ Curl_timeouts *timeouts,
                          ಸಂರಚನಾ Curl_easy *data);

#ifdef UNITTESTS
uint32_t Curl_splayget(ಸಂರಚನಾ Curl_ವೃಕ್ಷ *ಗಂಟು);
#endif

#endif /* HEADER_CURL_SPLAY_H */
