#اگر_تعریف_نشده سرآیند_CURL_پیشرفت_H
#تعریف_کن سرآیند_CURL_پیشرفت_H
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
  TIMER_آخرین /* must be last */
} timerid;

/* Get the current timestamp of the transfer */
ثابت بنية curltime *Curl_pgrs_now(بنية Curl_easy *داده);

صحيح Curl_pgrs_انجامشده(بنية Curl_easy *داده, ثابت بنية curltime *pnow);
فراغ Curl_pgrs_شروع(بنية Curl_easy *داده, ثابت بنية curltime *pnow);
فراغ Curl_pgrs_مجموعه_Download_اندازه(بنية Curl_easy *داده, curl_off_t اندازه);
فراغ Curl_pgrs_مجموعه_Upload_اندازه(بنية Curl_easy *داده, curl_off_t اندازه);
CURLcode Curl_pgrs_deliver_check(بنية Curl_easy *داده, size_t delta);
فراغ Curl_pgrs_deliver_inc(بنية Curl_easy *داده, size_t delta);
فراغ Curl_pgrs_download_inc(بنية Curl_easy *داده, size_t delta);
فراغ Curl_pgrs_upload_inc(بنية Curl_easy *داده, size_t delta);
فراغ Curl_pgrs_مجموعه_Upload_Counter(بنية Curl_easy *داده, curl_off_t اندازه);

/* perform progress update, invoking callbacks at intervals */
CURLcode Curl_pgrsUpdate(بنية Curl_easy *داده);
CURLcode Curl_pgrsUpdateX(بنية Curl_easy *داده, ثابت بنية curltime *pnow);
/* perform progress update, no callbacks invoked */
فراغ Curl_pgrsUpdate_nometer(بنية Curl_easy *داده);
/* perform progress update with callbacks and speed checks */
CURLcode Curl_pgrsCheck(بنية Curl_easy *داده);
CURLcode Curl_pgrsCheckX(بنية Curl_easy *داده, ثابت بنية curltime *pnow);

/* Inform progress/speedcheck about receive/send pausing */
فراغ Curl_pgrsRecvPause(بنية Curl_easy *داده, bool enable);
فراغ Curl_pgrsSendPause(بنية Curl_easy *داده, bool enable);

/* Reset sizes and counters for up- and download. */
فراغ Curl_pgrsReset(بنية Curl_easy *داده);
/* Reset sizes for up- and download. */
فراغ Curl_pgrsResetTransferSizes(بنية Curl_easy *داده);

فراغ Curl_pgrs_زمان(بنية Curl_easy *داده, timerid timer);
/**
 * Update progress timer with the elapsed time from its start to `timestamp`.
 * This allows updating timers later and is used by happy eyeballing, where
 * we only want to record the winner's times.
 */
فراغ Curl_pgrs_زمان_Was(بنية Curl_easy *داده, timerid timer,
                      بنية curltime timestamp);

فراغ Curl_pgrs_Early_داده(بنية Curl_easy *داده, curl_off_t فرستاده);

فراغ Curl_pgrsCompleted(بنية Curl_easy *داده);

timediff_t Curl_pgrs_since_ms(بنية Curl_easy *داده,
                              ثابت بنية curltime *pnow,
                              timerid timer);

#پایان_اگر /* HEADER_CURL_PROGRESS_H */
