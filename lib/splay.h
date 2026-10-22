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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "curlx/timeval.h"

ഘടന Curl_easy;

/* only use function calls to access this struct */
ഘടന Curl_വൃക്ഷം {
  ഘടന Curl_വൃക്ഷം *smaller; /* smaller node */
  ഘടന Curl_വൃക്ഷം *larger;  /* larger node */
  ഘടന Curl_വൃക്ഷം *same;    /* points to the next node with identical key */
  timediff_t താക്കോൽ;            /* this node's "sort" key */
  uint32_t id;               /* provided id for this node */
  BIT(registered);           /* node is registered in splay tree */
};

ഘടന Curl_timeouts {
  ഘടന Curl_വൃക്ഷം *വൃക്ഷം;
  time_t time_base_sec;
};

ശൂന്യം Curl_timeouts_init(ഘടന Curl_timeouts *timeouts,
                        സ്ഥിരം ഘടന curltime *ptime_base);

bool Curl_timeouts_has(ഘടന Curl_easy *data);

timediff_t Curl_timeouts_offset_us(ഘടന Curl_timeouts *timeouts,
                                   സ്ഥിരം ഘടന curltime *pts);

പൂർണ്ണസംഖ്യ Curl_timeouts_next_ms(ഘടന Curl_timeouts *timeouts,
                          സ്ഥിരം ഘടന curltime *pnow,
                          timediff_t *pexpire_offset_us,
                          uint32_t *pmid);

bool Curl_timeouts_remove_expired(ഘടന Curl_timeouts *timeouts,
                                  സ്ഥിരം ഘടന curltime *ts,
                                  uint32_t *pmid);

ശൂന്യം Curl_timeouts_add(ഘടന Curl_timeouts *timeouts,
                       ഘടന Curl_easy *data,
                       timediff_t offset_us);

/* Returns TRUE if data was registered in timeouts before */
bool Curl_timeouts_remove(ഘടന Curl_timeouts *timeouts,
                          ഘടന Curl_easy *data);

#ifdef UNITTESTS
uint32_t Curl_splayget(ഘടന Curl_വൃക്ഷം *കണ്ണി);
#endif

#endif /* HEADER_CURL_SPLAY_H */
