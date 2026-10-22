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
#અન્તર્ભાવ "curlx/timeval.h"

સંરચના Curl_easy;

પ્રકારવ્યાખ્યા ગણના {
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
અચલ સંરચના curltime *Curl_pgrs_now(સંરચના Curl_easy *data);

પૂર્ણાંક Curl_pgrsDone(સંરચના Curl_easy *data, અચલ સંરચના curltime *pnow);
શૂન્ય Curl_pgrsStart(સંરચના Curl_easy *data, અચલ સંરચના curltime *pnow);
શૂન્ય Curl_pgrsSetDownloadSize(સંરચના Curl_easy *data, curl_off_t size);
શૂન્ય Curl_pgrsSetUploadSize(સંરચના Curl_easy *data, curl_off_t size);
CURLcode Curl_pgrs_deliver_check(સંરચના Curl_easy *data, size_t delta);
શૂન્ય Curl_pgrs_deliver_inc(સંરચના Curl_easy *data, size_t delta);
શૂન્ય Curl_pgrs_download_inc(સંરચના Curl_easy *data, size_t delta);
શૂન્ય Curl_pgrs_upload_inc(સંરચના Curl_easy *data, size_t delta);
શૂન્ય Curl_pgrsSetUploadCounter(સંરચના Curl_easy *data, curl_off_t size);

/* perform progress update, invoking callbacks at intervals */
CURLcode Curl_pgrsUpdate(સંરચના Curl_easy *data);
CURLcode Curl_pgrsUpdateX(સંરચના Curl_easy *data, અચલ સંરચના curltime *pnow);
/* perform progress update, no callbacks invoked */
શૂન્ય Curl_pgrsUpdate_nometer(સંરચના Curl_easy *data);
/* perform progress update with callbacks and speed checks */
CURLcode Curl_pgrsCheck(સંરચના Curl_easy *data);
CURLcode Curl_pgrsCheckX(સંરચના Curl_easy *data, અચલ સંરચના curltime *pnow);

/* Inform progress/speedcheck about receive/send pausing */
શૂન્ય Curl_pgrsRecvPause(સંરચના Curl_easy *data, bool enable);
શૂન્ય Curl_pgrsSendPause(સંરચના Curl_easy *data, bool enable);

/* Reset sizes and counters for up- and download. */
શૂન્ય Curl_pgrsReset(સંરચના Curl_easy *data);
/* Reset sizes for up- and download. */
શૂન્ય Curl_pgrsResetTransferSizes(સંરચના Curl_easy *data);

શૂન્ય Curl_pgrsTime(સંરચના Curl_easy *data, timerid timer);
/**
 * Update progress timer with the elapsed time from its start to `timestamp`.
 * This allows updating timers later and is used by happy eyeballing, where
 * we only want to record the winner's times.
 */
શૂન્ય Curl_pgrsTimeWas(સંરચના Curl_easy *data, timerid timer,
                      સંરચના curltime timestamp);

શૂન્ય Curl_pgrsEarlyData(સંરચના Curl_easy *data, curl_off_t sent);

શૂન્ય Curl_pgrsCompleted(સંરચના Curl_easy *data);

timediff_t Curl_pgrs_since_ms(સંરચના Curl_easy *data,
                              અચલ સંરચના curltime *pnow,
                              timerid timer);

#endif /* HEADER_CURL_PROGRESS_H */
