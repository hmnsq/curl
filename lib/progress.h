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
#ಅನ್ತರ್ಭಾವ "curlx/timeval.h"

ಸಂರಚನಾ Curl_easy;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
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
ಧ್ರುವ ಸಂರಚನಾ curltime *Curl_pgrs_now(ಸಂರಚನಾ Curl_easy *data);

ಪೂರ್ಣಾಙ್ಕ Curl_pgrsDone(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ಸಂರಚನಾ curltime *pnow);
ಶೂನ್ಯ Curl_pgrsStart(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ಸಂರಚನಾ curltime *pnow);
ಶೂನ್ಯ Curl_pgrsSetDownloadSize(ಸಂರಚನಾ Curl_easy *data, curl_off_t size);
ಶೂನ್ಯ Curl_pgrsSetUploadSize(ಸಂರಚನಾ Curl_easy *data, curl_off_t size);
CURLcode Curl_pgrs_deliver_check(ಸಂರಚನಾ Curl_easy *data, size_t delta);
ಶೂನ್ಯ Curl_pgrs_deliver_inc(ಸಂರಚನಾ Curl_easy *data, size_t delta);
ಶೂನ್ಯ Curl_pgrs_download_inc(ಸಂರಚನಾ Curl_easy *data, size_t delta);
ಶೂನ್ಯ Curl_pgrs_upload_inc(ಸಂರಚನಾ Curl_easy *data, size_t delta);
ಶೂನ್ಯ Curl_pgrsSetUploadCounter(ಸಂರಚನಾ Curl_easy *data, curl_off_t size);

/* perform progress update, invoking callbacks at intervals */
CURLcode Curl_pgrsUpdate(ಸಂರಚನಾ Curl_easy *data);
CURLcode Curl_pgrsUpdateX(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ಸಂರಚನಾ curltime *pnow);
/* perform progress update, no callbacks invoked */
ಶೂನ್ಯ Curl_pgrsUpdate_nometer(ಸಂರಚನಾ Curl_easy *data);
/* perform progress update with callbacks and speed checks */
CURLcode Curl_pgrsCheck(ಸಂರಚನಾ Curl_easy *data);
CURLcode Curl_pgrsCheckX(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ಸಂರಚನಾ curltime *pnow);

/* Inform progress/speedcheck about receive/send pausing */
ಶೂನ್ಯ Curl_pgrsRecvPause(ಸಂರಚನಾ Curl_easy *data, bool enable);
ಶೂನ್ಯ Curl_pgrsSendPause(ಸಂರಚನಾ Curl_easy *data, bool enable);

/* Reset sizes and counters for up- and download. */
ಶೂನ್ಯ Curl_pgrsReset(ಸಂರಚನಾ Curl_easy *data);
/* Reset sizes for up- and download. */
ಶೂನ್ಯ Curl_pgrsResetTransferSizes(ಸಂರಚನಾ Curl_easy *data);

ಶೂನ್ಯ Curl_pgrsTime(ಸಂರಚನಾ Curl_easy *data, timerid timer);
/**
 * Update progress timer with the elapsed time from its start to `timestamp`.
 * This allows updating timers later and is used by happy eyeballing, where
 * we only want to record the winner's times.
 */
ಶೂನ್ಯ Curl_pgrsTimeWas(ಸಂರಚನಾ Curl_easy *data, timerid timer,
                      ಸಂರಚನಾ curltime timestamp);

ಶೂನ್ಯ Curl_pgrsEarlyData(ಸಂರಚನಾ Curl_easy *data, curl_off_t sent);

ಶೂನ್ಯ Curl_pgrsCompleted(ಸಂರಚನಾ Curl_easy *data);

timediff_t Curl_pgrs_since_ms(ಸಂರಚನಾ Curl_easy *data,
                              ಧ್ರುವ ಸಂರಚನಾ curltime *pnow,
                              timerid timer);

#endif /* HEADER_CURL_PROGRESS_H */
