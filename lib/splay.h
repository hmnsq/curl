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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "curlx/timeval.h"

কাঠামো Curl_easy;

/* only use function calls to access this struct */
কাঠামো Curl_বৃক্ষ {
  কাঠামো Curl_বৃক্ষ *smaller; /* smaller node */
  কাঠামো Curl_বৃক্ষ *larger;  /* larger node */
  কাঠামো Curl_বৃক্ষ *same;    /* points to the next node with identical key */
  timediff_t চাবি;            /* this node's "sort" key */
  uint32_t id;               /* provided id for this node */
  BIT(registered);           /* node is registered in splay tree */
};

কাঠামো Curl_timeouts {
  কাঠামো Curl_বৃক্ষ *বৃক্ষ;
  time_t time_base_sec;
};

শূন্য Curl_timeouts_init(কাঠামো Curl_timeouts *timeouts,
                        ধ্রুবক কাঠামো curltime *ptime_base);

bool Curl_timeouts_has(কাঠামো Curl_easy *তথ্য);

timediff_t Curl_timeouts_offset_us(কাঠামো Curl_timeouts *timeouts,
                                   ধ্রুবক কাঠামো curltime *pts);

পূর্ণসংখ্যা Curl_timeouts_পরবর্তী_ms(কাঠামো Curl_timeouts *timeouts,
                          ধ্রুবক কাঠামো curltime *pnow,
                          timediff_t *pexpire_offset_us,
                          uint32_t *pmid);

bool Curl_timeouts_remove_expired(কাঠামো Curl_timeouts *timeouts,
                                  ধ্রুবক কাঠামো curltime *ts,
                                  uint32_t *pmid);

শূন্য Curl_timeouts_add(কাঠামো Curl_timeouts *timeouts,
                       কাঠামো Curl_easy *তথ্য,
                       timediff_t offset_us);

/* Returns TRUE if data was registered in timeouts before */
bool Curl_timeouts_remove(কাঠামো Curl_timeouts *timeouts,
                          কাঠামো Curl_easy *তথ্য);

#ifdef UNITTESTS
uint32_t Curl_splayget(কাঠামো Curl_বৃক্ষ *গ্রন্থি);
#endif

#endif /* HEADER_CURL_SPLAY_H */
