#నిర్వచితం_కాకపోతే శీర్షిక_CURL_పురోగతి_H
#నిర్వచించు శీర్షిక_CURL_పురోగతి_H
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
#అన్తర్భావ "curlx/timeval.h"

నిర్మాణం Curl_easy;

రకనిర్వచనం గణన {
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
  TIMER_చివరి /* must be last */
} timerid;

/* Get the current timestamp of the transfer */
స్థిరరాశి నిర్మాణం curltime *Curl_pgrs_now(నిర్మాణం Curl_easy *దత్తాంశం);

పూర్ణాంకం Curl_pgrs_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి నిర్మాణం curltime *pnow);
శూన్య Curl_pgrs_ప్రారంభం(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి నిర్మాణం curltime *pnow);
శూన్య Curl_pgrs_సమితి_Download_కొలత(నిర్మాణం Curl_easy *దత్తాంశం, curl_off_t కొలత);
శూన్య Curl_pgrs_సమితి_Upload_కొలత(నిర్మాణం Curl_easy *దత్తాంశం, curl_off_t కొలత);
CURLcode Curl_pgrs_deliver_check(నిర్మాణం Curl_easy *దత్తాంశం, size_t delta);
శూన్య Curl_pgrs_deliver_inc(నిర్మాణం Curl_easy *దత్తాంశం, size_t delta);
శూన్య Curl_pgrs_download_inc(నిర్మాణం Curl_easy *దత్తాంశం, size_t delta);
శూన్య Curl_pgrs_upload_inc(నిర్మాణం Curl_easy *దత్తాంశం, size_t delta);
శూన్య Curl_pgrs_సమితి_Upload_Counter(నిర్మాణం Curl_easy *దత్తాంశం, curl_off_t కొలత);

/* perform progress update, invoking callbacks at intervals */
CURLcode Curl_pgrsUpdate(నిర్మాణం Curl_easy *దత్తాంశం);
CURLcode Curl_pgrsUpdateX(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి నిర్మాణం curltime *pnow);
/* perform progress update, no callbacks invoked */
శూన్య Curl_pgrsUpdate_nometer(నిర్మాణం Curl_easy *దత్తాంశం);
/* perform progress update with callbacks and speed checks */
CURLcode Curl_pgrsCheck(నిర్మాణం Curl_easy *దత్తాంశం);
CURLcode Curl_pgrsCheckX(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి నిర్మాణం curltime *pnow);

/* Inform progress/speedcheck about receive/send pausing */
శూన్య Curl_pgrsRecvPause(నిర్మాణం Curl_easy *దత్తాంశం, bool enable);
శూన్య Curl_pgrsSendPause(నిర్మాణం Curl_easy *దత్తాంశం, bool enable);

/* Reset sizes and counters for up- and download. */
శూన్య Curl_pgrsReset(నిర్మాణం Curl_easy *దత్తాంశం);
/* Reset sizes for up- and download. */
శూన్య Curl_pgrsResetTransferSizes(నిర్మాణం Curl_easy *దత్తాంశం);

శూన్య Curl_pgrs_సమయం(నిర్మాణం Curl_easy *దత్తాంశం, timerid timer);
/**
 * Update progress timer with the elapsed time from its start to `timestamp`.
 * This allows updating timers later and is used by happy eyeballing, where
 * we only want to record the winner's times.
 */
శూన్య Curl_pgrs_సమయం_Was(నిర్మాణం Curl_easy *దత్తాంశం, timerid timer,
                      నిర్మాణం curltime timestamp);

శూన్య Curl_pgrs_Early_దత్తాంశం(నిర్మాణం Curl_easy *దత్తాంశం, curl_off_t పంపిన);

శూన్య Curl_pgrsCompleted(నిర్మాణం Curl_easy *దత్తాంశం);

timediff_t Curl_pgrs_since_ms(నిర్మాణం Curl_easy *దత్తాంశం,
                              స్థిరరాశి నిర్మాణం curltime *pnow,
                              timerid timer);

#అయితే_ముగింపు /* HEADER_CURL_PROGRESS_H */
