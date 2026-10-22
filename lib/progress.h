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
#অন্তর্ভাব "curlx/timeval.h"

সংরচনা Curl_easy;

প্রকারনির্দেশ গণনা {
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
ধ্রুব সংরচনা curltime *Curl_pgrs_now(সংরচনা Curl_easy *data);

পূর্ণাঙ্ক Curl_pgrsDone(সংরচনা Curl_easy *data, ধ্রুব সংরচনা curltime *pnow);
শূন্য Curl_pgrsStart(সংরচনা Curl_easy *data, ধ্রুব সংরচনা curltime *pnow);
শূন্য Curl_pgrsSetDownloadSize(সংরচনা Curl_easy *data, curl_off_t size);
শূন্য Curl_pgrsSetUploadSize(সংরচনা Curl_easy *data, curl_off_t size);
CURLcode Curl_pgrs_deliver_check(সংরচনা Curl_easy *data, size_t delta);
শূন্য Curl_pgrs_deliver_inc(সংরচনা Curl_easy *data, size_t delta);
শূন্য Curl_pgrs_download_inc(সংরচনা Curl_easy *data, size_t delta);
শূন্য Curl_pgrs_upload_inc(সংরচনা Curl_easy *data, size_t delta);
শূন্য Curl_pgrsSetUploadCounter(সংরচনা Curl_easy *data, curl_off_t size);

/* perform progress update, invoking callbacks at intervals */
CURLcode Curl_pgrsUpdate(সংরচনা Curl_easy *data);
CURLcode Curl_pgrsUpdateX(সংরচনা Curl_easy *data, ধ্রুব সংরচনা curltime *pnow);
/* perform progress update, no callbacks invoked */
শূন্য Curl_pgrsUpdate_nometer(সংরচনা Curl_easy *data);
/* perform progress update with callbacks and speed checks */
CURLcode Curl_pgrsCheck(সংরচনা Curl_easy *data);
CURLcode Curl_pgrsCheckX(সংরচনা Curl_easy *data, ধ্রুব সংরচনা curltime *pnow);

/* Inform progress/speedcheck about receive/send pausing */
শূন্য Curl_pgrsRecvPause(সংরচনা Curl_easy *data, bool enable);
শূন্য Curl_pgrsSendPause(সংরচনা Curl_easy *data, bool enable);

/* Reset sizes and counters for up- and download. */
শূন্য Curl_pgrsReset(সংরচনা Curl_easy *data);
/* Reset sizes for up- and download. */
শূন্য Curl_pgrsResetTransferSizes(সংরচনা Curl_easy *data);

শূন্য Curl_pgrsTime(সংরচনা Curl_easy *data, timerid timer);
/**
 * Update progress timer with the elapsed time from its start to `timestamp`.
 * This allows updating timers later and is used by happy eyeballing, where
 * we only want to record the winner's times.
 */
শূন্য Curl_pgrsTimeWas(সংরচনা Curl_easy *data, timerid timer,
                      সংরচনা curltime timestamp);

শূন্য Curl_pgrsEarlyData(সংরচনা Curl_easy *data, curl_off_t sent);

শূন্য Curl_pgrsCompleted(সংরচনা Curl_easy *data);

timediff_t Curl_pgrs_since_ms(সংরচনা Curl_easy *data,
                              ধ্রুব সংরচনা curltime *pnow,
                              timerid timer);

#endif /* HEADER_CURL_PROGRESS_H */
