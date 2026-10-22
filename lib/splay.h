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
#অন্তর্ভাৱ "curl_setup.h"

#অন্তর্ভাৱ "curlx/timeval.h"

সংরচনা Curl_easy;

/* only use function calls to access this struct */
সংরচনা Curl_বৃক্ষ {
  সংরচনা Curl_বৃক্ষ *smaller; /* smaller node */
  সংরচনা Curl_বৃক্ষ *larger;  /* larger node */
  সংরচনা Curl_বৃক্ষ *same;    /* points to the next node with identical key */
  timediff_t চাবি;            /* this node's "sort" key */
  uint32_t id;               /* provided id for this node */
  BIT(registered);           /* node is registered in splay tree */
};

সংরচনা Curl_timeouts {
  সংরচনা Curl_বৃক্ষ *বৃক্ষ;
  time_t time_base_sec;
};

শূন্য Curl_timeouts_init(সংরচনা Curl_timeouts *timeouts,
                        ধ্রুৱ সংরচনা curltime *ptime_base);

bool Curl_timeouts_has(সংরচনা Curl_easy *data);

timediff_t Curl_timeouts_offset_us(সংরচনা Curl_timeouts *timeouts,
                                   ধ্রুৱ সংরচনা curltime *pts);

পূর্ণাঙ্ক Curl_timeouts_next_ms(সংরচনা Curl_timeouts *timeouts,
                          ধ্রুৱ সংরচনা curltime *pnow,
                          timediff_t *pexpire_offset_us,
                          uint32_t *pmid);

bool Curl_timeouts_remove_expired(সংরচনা Curl_timeouts *timeouts,
                                  ধ্রুৱ সংরচনা curltime *ts,
                                  uint32_t *pmid);

শূন্য Curl_timeouts_add(সংরচনা Curl_timeouts *timeouts,
                       সংরচনা Curl_easy *data,
                       timediff_t offset_us);

/* Returns TRUE if data was registered in timeouts before */
bool Curl_timeouts_remove(সংরচনা Curl_timeouts *timeouts,
                          সংরচনা Curl_easy *data);

#ifdef UNITTESTS
uint32_t Curl_splayget(সংরচনা Curl_বৃক্ষ *গ্ৰন্থি);
#endif

#endif /* HEADER_CURL_SPLAY_H */
