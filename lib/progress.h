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
#ଅନ୍ତର୍ଭାଵ "curlx/timeval.h"

ସଂରଚନା Curl_easy;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
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
ଅଚଳ ସଂରଚନା curltime *Curl_pgrs_now(ସଂରଚନା Curl_easy *data);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_pgrsDone(ସଂରଚନା Curl_easy *data, ଅଚଳ ସଂରଚନା curltime *pnow);
ଶୂନ୍ୟ Curl_pgrsStart(ସଂରଚନା Curl_easy *data, ଅଚଳ ସଂରଚନା curltime *pnow);
ଶୂନ୍ୟ Curl_pgrsSetDownloadSize(ସଂରଚନା Curl_easy *data, curl_off_t size);
ଶୂନ୍ୟ Curl_pgrsSetUploadSize(ସଂରଚନା Curl_easy *data, curl_off_t size);
CURLcode Curl_pgrs_deliver_check(ସଂରଚନା Curl_easy *data, size_t delta);
ଶୂନ୍ୟ Curl_pgrs_deliver_inc(ସଂରଚନା Curl_easy *data, size_t delta);
ଶୂନ୍ୟ Curl_pgrs_download_inc(ସଂରଚନା Curl_easy *data, size_t delta);
ଶୂନ୍ୟ Curl_pgrs_upload_inc(ସଂରଚନା Curl_easy *data, size_t delta);
ଶୂନ୍ୟ Curl_pgrsSetUploadCounter(ସଂରଚନା Curl_easy *data, curl_off_t size);

/* perform progress update, invoking callbacks at intervals */
CURLcode Curl_pgrsUpdate(ସଂରଚନା Curl_easy *data);
CURLcode Curl_pgrsUpdateX(ସଂରଚନା Curl_easy *data, ଅଚଳ ସଂରଚନା curltime *pnow);
/* perform progress update, no callbacks invoked */
ଶୂନ୍ୟ Curl_pgrsUpdate_nometer(ସଂରଚନା Curl_easy *data);
/* perform progress update with callbacks and speed checks */
CURLcode Curl_pgrsCheck(ସଂରଚନା Curl_easy *data);
CURLcode Curl_pgrsCheckX(ସଂରଚନା Curl_easy *data, ଅଚଳ ସଂରଚନା curltime *pnow);

/* Inform progress/speedcheck about receive/send pausing */
ଶୂନ୍ୟ Curl_pgrsRecvPause(ସଂରଚନା Curl_easy *data, bool enable);
ଶୂନ୍ୟ Curl_pgrsSendPause(ସଂରଚନା Curl_easy *data, bool enable);

/* Reset sizes and counters for up- and download. */
ଶୂନ୍ୟ Curl_pgrsReset(ସଂରଚନା Curl_easy *data);
/* Reset sizes for up- and download. */
ଶୂନ୍ୟ Curl_pgrsResetTransferSizes(ସଂରଚନା Curl_easy *data);

ଶୂନ୍ୟ Curl_pgrsTime(ସଂରଚନା Curl_easy *data, timerid timer);
/**
 * Update progress timer with the elapsed time from its start to `timestamp`.
 * This allows updating timers later and is used by happy eyeballing, where
 * we only want to record the winner's times.
 */
ଶୂନ୍ୟ Curl_pgrsTimeWas(ସଂରଚନା Curl_easy *data, timerid timer,
                      ସଂରଚନା curltime timestamp);

ଶୂନ୍ୟ Curl_pgrsEarlyData(ସଂରଚନା Curl_easy *data, curl_off_t sent);

ଶୂନ୍ୟ Curl_pgrsCompleted(ସଂରଚନା Curl_easy *data);

timediff_t Curl_pgrs_since_ms(ସଂରଚନା Curl_easy *data,
                              ଅଚଳ ସଂରଚନା curltime *pnow,
                              timerid timer);

#endif /* HEADER_CURL_PROGRESS_H */
