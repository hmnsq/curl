#अगर_अघोषित HEADER_CURL_PROGRESS_H
#घोषणा HEADER_CURL_PROGRESS_H
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
#समावेश "curlx/timeval.h"

काष्ठा Curl_easy;

प्रकार क्रमागत {
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
स्थिर काष्ठा curltime *Curl_pgrs_now(काष्ठा Curl_easy *तथ्य);

पूर्णांक Curl_pgrsDone(काष्ठा Curl_easy *तथ्य, स्थिर काष्ठा curltime *pnow);
व्योम Curl_pgrsStart(काष्ठा Curl_easy *तथ्य, स्थिर काष्ठा curltime *pnow);
व्योम Curl_pgrs_Set_Download_आकार(काष्ठा Curl_easy *तथ्य, curl_off_t आकार);
व्योम Curl_pgrs_Set_Upload_आकार(काष्ठा Curl_easy *तथ्य, curl_off_t आकार);
CURLcode Curl_pgrs_deliver_check(काष्ठा Curl_easy *तथ्य, माप_प्रकार delta);
व्योम Curl_pgrs_deliver_inc(काष्ठा Curl_easy *तथ्य, माप_प्रकार delta);
व्योम Curl_pgrs_download_inc(काष्ठा Curl_easy *तथ्य, माप_प्रकार delta);
व्योम Curl_pgrs_upload_inc(काष्ठा Curl_easy *तथ्य, माप_प्रकार delta);
व्योम Curl_pgrs_Set_Upload_गणक(काष्ठा Curl_easy *तथ्य, curl_off_t आकार);

/* perform progress update, invoking callbacks at intervals */
CURLcode Curl_pgrsUpdate(काष्ठा Curl_easy *तथ्य);
CURLcode Curl_pgrsUpdateX(काष्ठा Curl_easy *तथ्य, स्थिर काष्ठा curltime *pnow);
/* perform progress update, no callbacks invoked */
व्योम Curl_pgrsUpdate_nometer(काष्ठा Curl_easy *तथ्य);
/* perform progress update with callbacks and speed checks */
CURLcode Curl_pgrsCheck(काष्ठा Curl_easy *तथ्य);
CURLcode Curl_pgrsCheckX(काष्ठा Curl_easy *तथ्य, स्थिर काष्ठा curltime *pnow);

/* Inform progress/speedcheck about receive/send pausing */
व्योम Curl_pgrsRecvPause(काष्ठा Curl_easy *तथ्य, bool enable);
व्योम Curl_pgrsSendPause(काष्ठा Curl_easy *तथ्य, bool enable);

/* Reset sizes and counters for up- and download. */
व्योम Curl_pgrsReset(काष्ठा Curl_easy *तथ्य);
/* Reset sizes for up- and download. */
व्योम Curl_pgrsResetTransferSizes(काष्ठा Curl_easy *तथ्य);

व्योम Curl_pgrsTime(काष्ठा Curl_easy *तथ्य, timerid timer);
/**
 * Update progress timer with the elapsed time from its start to `timestamp`.
 * This allows updating timers later and is used by happy eyeballing, where
 * we only want to record the winner's times.
 */
व्योम Curl_pgrsTimeWas(काष्ठा Curl_easy *तथ्य, timerid timer,
                      काष्ठा curltime timestamp);

व्योम Curl_pgrs_Early_तथ्य(काष्ठा Curl_easy *तथ्य, curl_off_t sent);

व्योम Curl_pgrsCompleted(काष्ठा Curl_easy *तथ्य);

timediff_t Curl_pgrs_since_ms(काष्ठा Curl_easy *तथ्य,
                              स्थिर काष्ठा curltime *pnow,
                              timerid timer);

#पूर्ण_अगर /* HEADER_CURL_PROGRESS_H */
