#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_ਤਰੱਕੀ_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_ਤਰੱਕੀ_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/timeval.h"

ਬਣਤਰ Curl_easy;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
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
  TIMER_ਆਖਰੀ /* must be last */
} timerid;

/* Get the current timestamp of the transfer */
ਅਚਲ ਬਣਤਰ curltime *Curl_pgrs_now(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

ਪੂਰਨ Curl_pgrs_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਬਣਤਰ curltime *pnow);
ਖਾਲੀ Curl_pgrs_ਸ਼ੁਰੂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਬਣਤਰ curltime *pnow);
ਖਾਲੀ Curl_pgrs_ਸੈੱਟ_Download_ਮਾਪ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, curl_off_t ਮਾਪ);
ਖਾਲੀ Curl_pgrs_ਸੈੱਟ_Upload_ਮਾਪ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, curl_off_t ਮਾਪ);
CURLcode Curl_pgrs_deliver_check(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, size_t delta);
ਖਾਲੀ Curl_pgrs_deliver_inc(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, size_t delta);
ਖਾਲੀ Curl_pgrs_download_inc(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, size_t delta);
ਖਾਲੀ Curl_pgrs_upload_inc(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, size_t delta);
ਖਾਲੀ Curl_pgrs_ਸੈੱਟ_Upload_Counter(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, curl_off_t ਮਾਪ);

/* perform progress update, invoking callbacks at intervals */
CURLcode Curl_pgrsUpdate(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);
CURLcode Curl_pgrsUpdateX(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਬਣਤਰ curltime *pnow);
/* perform progress update, no callbacks invoked */
ਖਾਲੀ Curl_pgrsUpdate_nometer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);
/* perform progress update with callbacks and speed checks */
CURLcode Curl_pgrsCheck(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);
CURLcode Curl_pgrsCheckX(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਬਣਤਰ curltime *pnow);

/* Inform progress/speedcheck about receive/send pausing */
ਖਾਲੀ Curl_pgrsRecvPause(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool enable);
ਖਾਲੀ Curl_pgrsSendPause(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool enable);

/* Reset sizes and counters for up- and download. */
ਖਾਲੀ Curl_pgrsReset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);
/* Reset sizes for up- and download. */
ਖਾਲੀ Curl_pgrsResetTransferSizes(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

ਖਾਲੀ Curl_pgrs_ਸਮਾਂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, timerid timer);
/**
 * Update progress timer with the elapsed time from its start to `timestamp`.
 * This allows updating timers later and is used by happy eyeballing, where
 * we only want to record the winner's times.
 */
ਖਾਲੀ Curl_pgrs_ਸਮਾਂ_Was(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, timerid timer,
                      ਬਣਤਰ curltime timestamp);

ਖਾਲੀ Curl_pgrs_Early_ਅੰਕੜੇ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, curl_off_t ਭੇਜਿਆ);

ਖਾਲੀ Curl_pgrsCompleted(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

timediff_t Curl_pgrs_since_ms(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਅਚਲ ਬਣਤਰ curltime *pnow,
                              timerid timer);

#ਜੇ_ਅੰਤ /* HEADER_CURL_PROGRESS_H */
