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
#അന്തര്ഭാവ "curlx/timeval.h"

ഘടന Curl_easy;

തരനിർവചനം എണ്ണൽ {
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
സ്ഥിരം ഘടന curltime *Curl_pgrs_now(ഘടന Curl_easy *data);

പൂർണ്ണസംഖ്യ Curl_pgrsDone(ഘടന Curl_easy *data, സ്ഥിരം ഘടന curltime *pnow);
ശൂന്യം Curl_pgrsStart(ഘടന Curl_easy *data, സ്ഥിരം ഘടന curltime *pnow);
ശൂന്യം Curl_pgrsSetDownloadSize(ഘടന Curl_easy *data, curl_off_t size);
ശൂന്യം Curl_pgrsSetUploadSize(ഘടന Curl_easy *data, curl_off_t size);
CURLcode Curl_pgrs_deliver_check(ഘടന Curl_easy *data, size_t delta);
ശൂന്യം Curl_pgrs_deliver_inc(ഘടന Curl_easy *data, size_t delta);
ശൂന്യം Curl_pgrs_download_inc(ഘടന Curl_easy *data, size_t delta);
ശൂന്യം Curl_pgrs_upload_inc(ഘടന Curl_easy *data, size_t delta);
ശൂന്യം Curl_pgrsSetUploadCounter(ഘടന Curl_easy *data, curl_off_t size);

/* perform progress update, invoking callbacks at intervals */
CURLcode Curl_pgrsUpdate(ഘടന Curl_easy *data);
CURLcode Curl_pgrsUpdateX(ഘടന Curl_easy *data, സ്ഥിരം ഘടന curltime *pnow);
/* perform progress update, no callbacks invoked */
ശൂന്യം Curl_pgrsUpdate_nometer(ഘടന Curl_easy *data);
/* perform progress update with callbacks and speed checks */
CURLcode Curl_pgrsCheck(ഘടന Curl_easy *data);
CURLcode Curl_pgrsCheckX(ഘടന Curl_easy *data, സ്ഥിരം ഘടന curltime *pnow);

/* Inform progress/speedcheck about receive/send pausing */
ശൂന്യം Curl_pgrsRecvPause(ഘടന Curl_easy *data, bool enable);
ശൂന്യം Curl_pgrsSendPause(ഘടന Curl_easy *data, bool enable);

/* Reset sizes and counters for up- and download. */
ശൂന്യം Curl_pgrsReset(ഘടന Curl_easy *data);
/* Reset sizes for up- and download. */
ശൂന്യം Curl_pgrsResetTransferSizes(ഘടന Curl_easy *data);

ശൂന്യം Curl_pgrsTime(ഘടന Curl_easy *data, timerid timer);
/**
 * Update progress timer with the elapsed time from its start to `timestamp`.
 * This allows updating timers later and is used by happy eyeballing, where
 * we only want to record the winner's times.
 */
ശൂന്യം Curl_pgrsTimeWas(ഘടന Curl_easy *data, timerid timer,
                      ഘടന curltime timestamp);

ശൂന്യം Curl_pgrsEarlyData(ഘടന Curl_easy *data, curl_off_t sent);

ശൂന്യം Curl_pgrsCompleted(ഘടന Curl_easy *data);

timediff_t Curl_pgrs_since_ms(ഘടന Curl_easy *data,
                              സ്ഥിരം ഘടന curltime *pnow,
                              timerid timer);

#endif /* HEADER_CURL_PROGRESS_H */
