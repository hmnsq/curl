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
#ضمن "curlx/timeval.h"

بنية Curl_easy;

تعريفنوع تعداد {
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
ثابت بنية curltime *Curl_pgrs_now(بنية Curl_easy *data);

صحيح Curl_pgrsDone(بنية Curl_easy *data, ثابت بنية curltime *pnow);
فراغ Curl_pgrsStart(بنية Curl_easy *data, ثابت بنية curltime *pnow);
فراغ Curl_pgrsSetDownloadSize(بنية Curl_easy *data, curl_off_t size);
فراغ Curl_pgrsSetUploadSize(بنية Curl_easy *data, curl_off_t size);
CURLcode Curl_pgrs_deliver_check(بنية Curl_easy *data, size_t delta);
فراغ Curl_pgrs_deliver_inc(بنية Curl_easy *data, size_t delta);
فراغ Curl_pgrs_download_inc(بنية Curl_easy *data, size_t delta);
فراغ Curl_pgrs_upload_inc(بنية Curl_easy *data, size_t delta);
فراغ Curl_pgrsSetUploadCounter(بنية Curl_easy *data, curl_off_t size);

/* perform progress update, invoking callbacks at intervals */
CURLcode Curl_pgrsUpdate(بنية Curl_easy *data);
CURLcode Curl_pgrsUpdateX(بنية Curl_easy *data, ثابت بنية curltime *pnow);
/* perform progress update, no callbacks invoked */
فراغ Curl_pgrsUpdate_nometer(بنية Curl_easy *data);
/* perform progress update with callbacks and speed checks */
CURLcode Curl_pgrsCheck(بنية Curl_easy *data);
CURLcode Curl_pgrsCheckX(بنية Curl_easy *data, ثابت بنية curltime *pnow);

/* Inform progress/speedcheck about receive/send pausing */
فراغ Curl_pgrsRecvPause(بنية Curl_easy *data, bool enable);
فراغ Curl_pgrsSendPause(بنية Curl_easy *data, bool enable);

/* Reset sizes and counters for up- and download. */
فراغ Curl_pgrsReset(بنية Curl_easy *data);
/* Reset sizes for up- and download. */
فراغ Curl_pgrsResetTransferSizes(بنية Curl_easy *data);

فراغ Curl_pgrsTime(بنية Curl_easy *data, timerid timer);
/**
 * Update progress timer with the elapsed time from its start to `timestamp`.
 * This allows updating timers later and is used by happy eyeballing, where
 * we only want to record the winner's times.
 */
فراغ Curl_pgrsTimeWas(بنية Curl_easy *data, timerid timer,
                      بنية curltime timestamp);

فراغ Curl_pgrsEarlyData(بنية Curl_easy *data, curl_off_t sent);

فراغ Curl_pgrsCompleted(بنية Curl_easy *data);

timediff_t Curl_pgrs_since_ms(بنية Curl_easy *data,
                              ثابت بنية curltime *pnow,
                              timerid timer);

#endif /* HEADER_CURL_PROGRESS_H */
