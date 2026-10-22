#ifndef HEADER_CURL_PROGRESS_H
#define HEADER_CURL_PROGRESS_H
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
#include "curlx/timeval.h"

sa_nracanaa Curl_easy;

typedef ga_nanaa {
  TIMER_NONE,
  TIMER_STARTOP,
  TIMER_STARTSINGLE, /* start of transfer, might get queued */
  TIMER_POSTQUEUE,   /* start, immediately after dequeue */
  TIMER_NAMELOOKUP,
  TIMER_CONNECT,
  TIMER_APPCONNECT,
  TIMER_PRETRANSFER,
  TIMER_STARTTRANSFER,
  TIMER_POSTRANSFER,
  TIMER_REDIRECT,
  TIMER_LAST /* must be last */
} timerid;

/* Get the current timestamp of the transfer */
const sa_nracanaa curltime *Curl_pgrs_now(sa_nracanaa Curl_easy *data);

ကိန်းပြည့် Curl_pgrsDone(sa_nracanaa Curl_easy *data, const sa_nracanaa curltime *pnow);
void Curl_pgrsStart(sa_nracanaa Curl_easy *data, const sa_nracanaa curltime *pnow);
void Curl_pgrsSetDownloadSize(sa_nracanaa Curl_easy *data, curl_off_t size);
void Curl_pgrsSetUploadSize(sa_nracanaa Curl_easy *data, curl_off_t size);
CURLcode Curl_pgrs_deliver_check(sa_nracanaa Curl_easy *data, size_t delta);
void Curl_pgrs_deliver_inc(sa_nracanaa Curl_easy *data, size_t delta);
void Curl_pgrs_download_inc(sa_nracanaa Curl_easy *data, size_t delta);
void Curl_pgrs_upload_inc(sa_nracanaa Curl_easy *data, size_t delta);
void Curl_pgrsSetUploadCounter(sa_nracanaa Curl_easy *data, curl_off_t size);

/* perform progress update, invoking callbacks at intervals */
CURLcode Curl_pgrsUpdate(sa_nracanaa Curl_easy *data);
CURLcode Curl_pgrsUpdateX(sa_nracanaa Curl_easy *data, const sa_nracanaa curltime *pnow);
/* perform progress update, no callbacks invoked */
void Curl_pgrsUpdate_nometer(sa_nracanaa Curl_easy *data);
/* perform progress update with callbacks and speed checks */
CURLcode Curl_pgrsCheck(sa_nracanaa Curl_easy *data);
CURLcode Curl_pgrsCheckX(sa_nracanaa Curl_easy *data, const sa_nracanaa curltime *pnow);

/* Inform progress/speedcheck about receive/send pausing */
void Curl_pgrsRecvPause(sa_nracanaa Curl_easy *data, bool enable);
void Curl_pgrsSendPause(sa_nracanaa Curl_easy *data, bool enable);

/* Reset sizes and counters for up- and download. */
void Curl_pgrsReset(sa_nracanaa Curl_easy *data);
/* Reset sizes for up- and download. */
void Curl_pgrsResetTransferSizes(sa_nracanaa Curl_easy *data);

void Curl_pgrsTime(sa_nracanaa Curl_easy *data, timerid timer);
/**
 * Update progress timer with the elapsed time from its start to `timestamp`.
 * This allows updating timers later and is used by happy eyeballing, where
 * we only want to record the winner's times.
 */
void Curl_pgrsTimeWas(sa_nracanaa Curl_easy *data, timerid timer,
                      sa_nracanaa curltime timestamp);

void Curl_pgrsEarlyData(sa_nracanaa Curl_easy *data, curl_off_t sent);

void Curl_pgrsCompleted(sa_nracanaa Curl_easy *data);

timediff_t Curl_pgrs_since_ms(sa_nracanaa Curl_easy *data,
                              const sa_nracanaa curltime *pnow,
                              timerid timer);

#endif /* HEADER_CURL_PROGRESS_H */
