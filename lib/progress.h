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

কাঠামো Curl_easy;

প্রকারলেখ গণনা {
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
ধ্রুবক কাঠামো curltime *Curl_pgrs_now(কাঠামো Curl_easy *তথ্য);

পূর্ণসংখ্যা Curl_pgrsDone(কাঠামো Curl_easy *তথ্য, ধ্রুবক কাঠামো curltime *pnow);
শূন্য Curl_pgrsStart(কাঠামো Curl_easy *তথ্য, ধ্রুবক কাঠামো curltime *pnow);
শূন্য Curl_pgrs_Set_Download_আকার(কাঠামো Curl_easy *তথ্য, curl_off_t আকার_2);
শূন্য Curl_pgrs_Set_Upload_আকার(কাঠামো Curl_easy *তথ্য, curl_off_t আকার_2);
CURLcode Curl_pgrs_deliver_check(কাঠামো Curl_easy *তথ্য, size_t delta);
শূন্য Curl_pgrs_deliver_inc(কাঠামো Curl_easy *তথ্য, size_t delta);
শূন্য Curl_pgrs_download_inc(কাঠামো Curl_easy *তথ্য, size_t delta);
শূন্য Curl_pgrs_upload_inc(কাঠামো Curl_easy *তথ্য, size_t delta);
শূন্য Curl_pgrs_Set_Upload_গণক(কাঠামো Curl_easy *তথ্য, curl_off_t আকার_2);

/* perform progress update, invoking callbacks at intervals */
CURLcode Curl_pgrsUpdate(কাঠামো Curl_easy *তথ্য);
CURLcode Curl_pgrsUpdateX(কাঠামো Curl_easy *তথ্য, ধ্রুবক কাঠামো curltime *pnow);
/* perform progress update, no callbacks invoked */
শূন্য Curl_pgrsUpdate_nometer(কাঠামো Curl_easy *তথ্য);
/* perform progress update with callbacks and speed checks */
CURLcode Curl_pgrsCheck(কাঠামো Curl_easy *তথ্য);
CURLcode Curl_pgrsCheckX(কাঠামো Curl_easy *তথ্য, ধ্রুবক কাঠামো curltime *pnow);

/* Inform progress/speedcheck about receive/send pausing */
শূন্য Curl_pgrsRecvPause(কাঠামো Curl_easy *তথ্য, bool enable);
শূন্য Curl_pgrsSendPause(কাঠামো Curl_easy *তথ্য, bool enable);

/* Reset sizes and counters for up- and download. */
শূন্য Curl_pgrsReset(কাঠামো Curl_easy *তথ্য);
/* Reset sizes for up- and download. */
শূন্য Curl_pgrsResetTransferSizes(কাঠামো Curl_easy *তথ্য);

শূন্য Curl_pgrsTime(কাঠামো Curl_easy *তথ্য, timerid timer);
/**
 * Update progress timer with the elapsed time from its start to `timestamp`.
 * This allows updating timers later and is used by happy eyeballing, where
 * we only want to record the winner's times.
 */
শূন্য Curl_pgrsTimeWas(কাঠামো Curl_easy *তথ্য, timerid timer,
                      কাঠামো curltime timestamp);

শূন্য Curl_pgrs_Early_তথ্য(কাঠামো Curl_easy *তথ্য, curl_off_t sent);

শূন্য Curl_pgrsCompleted(কাঠামো Curl_easy *তথ্য);

timediff_t Curl_pgrs_since_ms(কাঠামো Curl_easy *তথ্য,
                              ধ্রুবক কাঠামো curltime *pnow,
                              timerid timer);

#endif /* HEADER_CURL_PROGRESS_H */
