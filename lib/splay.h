#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_SPLAY_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_SPLAY_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/timeval.h"

ਬਣਤਰ Curl_easy;

/* only use function calls to access this struct */
ਬਣਤਰ Curl_ਰੁੱਖ {
  ਬਣਤਰ Curl_ਰੁੱਖ *smaller; /* smaller node */
  ਬਣਤਰ Curl_ਰੁੱਖ *larger;  /* larger node */
  ਬਣਤਰ Curl_ਰੁੱਖ *same;    /* points to the next node with identical key */
  timediff_t ਕੁੰਜੀ;            /* this node's "sort" key */
  uint32_t id;               /* provided id for this node */
  ਦੋਆਧਾਰੀ_ਅੰਕ(registered);           /* node is registered in splay tree */
};

ਬਣਤਰ Curl_timeouts {
  ਬਣਤਰ Curl_ਰੁੱਖ *ਰੁੱਖ;
  time_t ਸਮਾਂ_ਅਧਾਰ_sec;
};

ਖਾਲੀ Curl_timeouts_init(ਬਣਤਰ Curl_timeouts *timeouts,
                        ਅਚਲ ਬਣਤਰ curltime *ptime_ਅਧਾਰ);

bool Curl_timeouts_has(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

timediff_t Curl_timeouts_ਵਿੱਥ_us(ਬਣਤਰ Curl_timeouts *timeouts,
                                   ਅਚਲ ਬਣਤਰ curltime *pts);

ਪੂਰਨ Curl_timeouts_ਅਗਲਾ_ms(ਬਣਤਰ Curl_timeouts *timeouts,
                          ਅਚਲ ਬਣਤਰ curltime *pnow,
                          timediff_t *pexpire_ਵਿੱਥ_us,
                          uint32_t *pmid);

bool Curl_timeouts_remove_expired(ਬਣਤਰ Curl_timeouts *timeouts,
                                  ਅਚਲ ਬਣਤਰ curltime *ts,
                                  uint32_t *pmid);

ਖਾਲੀ Curl_timeouts_ਜੋੜੋ(ਬਣਤਰ Curl_timeouts *timeouts,
                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                       timediff_t ਵਿੱਥ_us);

/* Returns TRUE if data was registered in timeouts before */
bool Curl_timeouts_remove(ਬਣਤਰ Curl_timeouts *timeouts,
                          ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNITTESTS
uint32_t Curl_splayget(ਬਣਤਰ Curl_ਰੁੱਖ *ਗੰਢ);
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HEADER_CURL_SPLAY_H */
