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
#شامل "curlx/timeval.h"

ساختار Curl_easy;

تعریفنوع شمارش {
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
ثابت ساختار curltime *Curl_pgrs_now(ساختار Curl_easy *data);

صحیح Curl_pgrsDone(ساختار Curl_easy *data, ثابت ساختار curltime *pnow);
تهی Curl_pgrsStart(ساختار Curl_easy *data, ثابت ساختار curltime *pnow);
تهی Curl_pgrsSetDownloadSize(ساختار Curl_easy *data, curl_off_t size);
تهی Curl_pgrsSetUploadSize(ساختار Curl_easy *data, curl_off_t size);
CURLcode Curl_pgrs_deliver_check(ساختار Curl_easy *data, size_t delta);
تهی Curl_pgrs_deliver_inc(ساختار Curl_easy *data, size_t delta);
تهی Curl_pgrs_download_inc(ساختار Curl_easy *data, size_t delta);
تهی Curl_pgrs_upload_inc(ساختار Curl_easy *data, size_t delta);
تهی Curl_pgrsSetUploadCounter(ساختار Curl_easy *data, curl_off_t size);

/* perform progress update, invoking callbacks at intervals */
CURLcode Curl_pgrsUpdate(ساختار Curl_easy *data);
CURLcode Curl_pgrsUpdateX(ساختار Curl_easy *data, ثابت ساختار curltime *pnow);
/* perform progress update, no callbacks invoked */
تهی Curl_pgrsUpdate_nometer(ساختار Curl_easy *data);
/* perform progress update with callbacks and speed checks */
CURLcode Curl_pgrsCheck(ساختار Curl_easy *data);
CURLcode Curl_pgrsCheckX(ساختار Curl_easy *data, ثابت ساختار curltime *pnow);

/* Inform progress/speedcheck about receive/send pausing */
تهی Curl_pgrsRecvPause(ساختار Curl_easy *data, bool enable);
تهی Curl_pgrsSendPause(ساختار Curl_easy *data, bool enable);

/* Reset sizes and counters for up- and download. */
تهی Curl_pgrsReset(ساختار Curl_easy *data);
/* Reset sizes for up- and download. */
تهی Curl_pgrsResetTransferSizes(ساختار Curl_easy *data);

تهی Curl_pgrsTime(ساختار Curl_easy *data, timerid timer);
/**
 * Update progress timer with the elapsed time from its start to `timestamp`.
 * This allows updating timers later and is used by happy eyeballing, where
 * we only want to record the winner's times.
 */
تهی Curl_pgrsTimeWas(ساختار Curl_easy *data, timerid timer,
                      ساختار curltime timestamp);

تهی Curl_pgrsEarlyData(ساختار Curl_easy *data, curl_off_t sent);

تهی Curl_pgrsCompleted(ساختار Curl_easy *data);

timediff_t Curl_pgrs_since_ms(ساختار Curl_easy *data,
                              ثابت ساختار curltime *pnow,
                              timerid timer);

#endif /* HEADER_CURL_PROGRESS_H */
