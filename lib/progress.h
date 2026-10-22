#اگر_غیر_تعریف_شدہ سرخی_CURL_پیش_رفت_H
#تعریف_کرو سرخی_CURL_پیش_رفت_H
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

ساخت Curl_easy;

قسمتعریف شمار {
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
  TIMER_آخری /* must be last */
} timerid;

/* Get the current timestamp of the transfer */
ثابت ساخت curltime *Curl_pgrs_now(ساخت Curl_easy *ڈیٹا);

صحیح Curl_pgrs_مکمل(ساخت Curl_easy *ڈیٹا, ثابت ساخت curltime *pnow);
خالی Curl_pgrs_آغاز(ساخت Curl_easy *ڈیٹا, ثابت ساخت curltime *pnow);
خالی Curl_pgrs_سیٹ_Download_سائز(ساخت Curl_easy *ڈیٹا, curl_off_t سائز);
خالی Curl_pgrs_سیٹ_Upload_سائز(ساخت Curl_easy *ڈیٹا, curl_off_t سائز);
CURLcode Curl_pgrs_deliver_check(ساخت Curl_easy *ڈیٹا, size_t delta);
خالی Curl_pgrs_deliver_inc(ساخت Curl_easy *ڈیٹا, size_t delta);
خالی Curl_pgrs_download_inc(ساخت Curl_easy *ڈیٹا, size_t delta);
خالی Curl_pgrs_upload_inc(ساخت Curl_easy *ڈیٹا, size_t delta);
خالی Curl_pgrs_سیٹ_Upload_شمار(ساخت Curl_easy *ڈیٹا, curl_off_t سائز);

/* perform progress update, invoking callbacks at intervals */
CURLcode Curl_pgrsUpdate(ساخت Curl_easy *ڈیٹا);
CURLcode Curl_pgrsUpdateX(ساخت Curl_easy *ڈیٹا, ثابت ساخت curltime *pnow);
/* perform progress update, no callbacks invoked */
خالی Curl_pgrsUpdate_nometer(ساخت Curl_easy *ڈیٹا);
/* perform progress update with callbacks and speed checks */
CURLcode Curl_pgrsCheck(ساخت Curl_easy *ڈیٹا);
CURLcode Curl_pgrsCheckX(ساخت Curl_easy *ڈیٹا, ثابت ساخت curltime *pnow);

/* Inform progress/speedcheck about receive/send pausing */
خالی Curl_pgrsRecvPause(ساخت Curl_easy *ڈیٹا, bool enable);
خالی Curl_pgrsSendPause(ساخت Curl_easy *ڈیٹا, bool enable);

/* Reset sizes and counters for up- and download. */
خالی Curl_pgrsReset(ساخت Curl_easy *ڈیٹا);
/* Reset sizes for up- and download. */
خالی Curl_pgrsResetTransferSizes(ساخت Curl_easy *ڈیٹا);

خالی Curl_pgrs_وقت(ساخت Curl_easy *ڈیٹا, timerid timer);
/**
 * Update progress timer with the elapsed time from its start to `timestamp`.
 * This allows updating timers later and is used by happy eyeballing, where
 * we only want to record the winner's times.
 */
خالی Curl_pgrs_وقت_Was(ساخت Curl_easy *ڈیٹا, timerid timer,
                      ساخت curltime timestamp);

خالی Curl_pgrs_Early_ڈیٹا(ساخت Curl_easy *ڈیٹا, curl_off_t بھیجا);

خالی Curl_pgrsCompleted(ساخت Curl_easy *ڈیٹا);

timediff_t Curl_pgrs_since_ms(ساخت Curl_easy *ڈیٹا,
                              ثابت ساخت curltime *pnow,
                              timerid timer);

#اگر_ختم /* HEADER_CURL_PROGRESS_H */
