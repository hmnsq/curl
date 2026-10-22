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
#அந்தர்பாவ "curlx/timeval.h"

கட்டமைப்பு Curl_easy;

வகைவரை எண்ணிக்கை {
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
மாறிலி கட்டமைப்பு curltime *Curl_pgrs_now(கட்டமைப்பு Curl_easy *data);

முழு Curl_pgrsDone(கட்டமைப்பு Curl_easy *data, மாறிலி கட்டமைப்பு curltime *pnow);
வெறுமை Curl_pgrsStart(கட்டமைப்பு Curl_easy *data, மாறிலி கட்டமைப்பு curltime *pnow);
வெறுமை Curl_pgrsSetDownloadSize(கட்டமைப்பு Curl_easy *data, curl_off_t size);
வெறுமை Curl_pgrsSetUploadSize(கட்டமைப்பு Curl_easy *data, curl_off_t size);
CURLcode Curl_pgrs_deliver_check(கட்டமைப்பு Curl_easy *data, size_t delta);
வெறுமை Curl_pgrs_deliver_inc(கட்டமைப்பு Curl_easy *data, size_t delta);
வெறுமை Curl_pgrs_download_inc(கட்டமைப்பு Curl_easy *data, size_t delta);
வெறுமை Curl_pgrs_upload_inc(கட்டமைப்பு Curl_easy *data, size_t delta);
வெறுமை Curl_pgrsSetUploadCounter(கட்டமைப்பு Curl_easy *data, curl_off_t size);

/* perform progress update, invoking callbacks at intervals */
CURLcode Curl_pgrsUpdate(கட்டமைப்பு Curl_easy *data);
CURLcode Curl_pgrsUpdateX(கட்டமைப்பு Curl_easy *data, மாறிலி கட்டமைப்பு curltime *pnow);
/* perform progress update, no callbacks invoked */
வெறுமை Curl_pgrsUpdate_nometer(கட்டமைப்பு Curl_easy *data);
/* perform progress update with callbacks and speed checks */
CURLcode Curl_pgrsCheck(கட்டமைப்பு Curl_easy *data);
CURLcode Curl_pgrsCheckX(கட்டமைப்பு Curl_easy *data, மாறிலி கட்டமைப்பு curltime *pnow);

/* Inform progress/speedcheck about receive/send pausing */
வெறுமை Curl_pgrsRecvPause(கட்டமைப்பு Curl_easy *data, bool enable);
வெறுமை Curl_pgrsSendPause(கட்டமைப்பு Curl_easy *data, bool enable);

/* Reset sizes and counters for up- and download. */
வெறுமை Curl_pgrsReset(கட்டமைப்பு Curl_easy *data);
/* Reset sizes for up- and download. */
வெறுமை Curl_pgrsResetTransferSizes(கட்டமைப்பு Curl_easy *data);

வெறுமை Curl_pgrsTime(கட்டமைப்பு Curl_easy *data, timerid timer);
/**
 * Update progress timer with the elapsed time from its start to `timestamp`.
 * This allows updating timers later and is used by happy eyeballing, where
 * we only want to record the winner's times.
 */
வெறுமை Curl_pgrsTimeWas(கட்டமைப்பு Curl_easy *data, timerid timer,
                      கட்டமைப்பு curltime timestamp);

வெறுமை Curl_pgrsEarlyData(கட்டமைப்பு Curl_easy *data, curl_off_t sent);

வெறுமை Curl_pgrsCompleted(கட்டமைப்பு Curl_easy *data);

timediff_t Curl_pgrs_since_ms(கட்டமைப்பு Curl_easy *data,
                              மாறிலி கட்டமைப்பு curltime *pnow,
                              timerid timer);

#endif /* HEADER_CURL_PROGRESS_H */
