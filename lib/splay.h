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
#شامل "curl_setup.h"

#شامل "curlx/timeval.h"

ساختار Curl_easy;

/* only use function calls to access this struct */
ساختار Curl_درخت {
  ساختار Curl_درخت *smaller; /* smaller node */
  ساختار Curl_درخت *larger;  /* larger node */
  ساختار Curl_درخت *same;    /* points to the next node with identical key */
  timediff_t کلید;            /* this node's "sort" key */
  uint32_t id;               /* provided id for this node */
  BIT(registered);           /* node is registered in splay tree */
};

ساختار Curl_timeouts {
  ساختار Curl_درخت *درخت;
  time_t time_base_sec;
};

تهی Curl_timeouts_init(ساختار Curl_timeouts *timeouts,
                        ثابت ساختار curltime *ptime_base);

bool Curl_timeouts_has(ساختار Curl_easy *data);

timediff_t Curl_timeouts_offset_us(ساختار Curl_timeouts *timeouts,
                                   ثابت ساختار curltime *pts);

صحیح Curl_timeouts_next_ms(ساختار Curl_timeouts *timeouts,
                          ثابت ساختار curltime *pnow,
                          timediff_t *pexpire_offset_us,
                          uint32_t *pmid);

bool Curl_timeouts_remove_expired(ساختار Curl_timeouts *timeouts,
                                  ثابت ساختار curltime *ts,
                                  uint32_t *pmid);

تهی Curl_timeouts_add(ساختار Curl_timeouts *timeouts,
                       ساختار Curl_easy *data,
                       timediff_t offset_us);

/* Returns TRUE if data was registered in timeouts before */
bool Curl_timeouts_remove(ساختار Curl_timeouts *timeouts,
                          ساختار Curl_easy *data);

#ifdef UNITTESTS
uint32_t Curl_splayget(ساختار Curl_درخت *گره);
#endif

#endif /* HEADER_CURL_SPLAY_H */
