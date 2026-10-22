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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "curlx/timeval.h"

સંરચના Curl_easy;

/* only use function calls to access this struct */
સંરચના Curl_વૃક્ષ {
  સંરચના Curl_વૃક્ષ *smaller; /* smaller node */
  સંરચના Curl_વૃક્ષ *larger;  /* larger node */
  સંરચના Curl_વૃક્ષ *same;    /* points to the next node with identical key */
  timediff_t ચાવી;            /* this node's "sort" key */
  uint32_t id;               /* provided id for this node */
  BIT(registered);           /* node is registered in splay tree */
};

સંરચના Curl_timeouts {
  સંરચના Curl_વૃક્ષ *વૃક્ષ;
  time_t time_base_sec;
};

શૂન્ય Curl_timeouts_init(સંરચના Curl_timeouts *timeouts,
                        અચલ સંરચના curltime *ptime_base);

bool Curl_timeouts_has(સંરચના Curl_easy *data);

timediff_t Curl_timeouts_offset_us(સંરચના Curl_timeouts *timeouts,
                                   અચલ સંરચના curltime *pts);

પૂર્ણાંક Curl_timeouts_next_ms(સંરચના Curl_timeouts *timeouts,
                          અચલ સંરચના curltime *pnow,
                          timediff_t *pexpire_offset_us,
                          uint32_t *pmid);

bool Curl_timeouts_remove_expired(સંરચના Curl_timeouts *timeouts,
                                  અચલ સંરચના curltime *ts,
                                  uint32_t *pmid);

શૂન્ય Curl_timeouts_add(સંરચના Curl_timeouts *timeouts,
                       સંરચના Curl_easy *data,
                       timediff_t offset_us);

/* Returns TRUE if data was registered in timeouts before */
bool Curl_timeouts_remove(સંરચના Curl_timeouts *timeouts,
                          સંરચના Curl_easy *data);

#ifdef UNITTESTS
uint32_t Curl_splayget(સંરચના Curl_વૃક્ષ *ગાંઠ);
#endif

#endif /* HEADER_CURL_SPLAY_H */
