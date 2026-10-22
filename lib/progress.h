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
#अन्तर्भाव "curlx/timeval.h"

संरचना Curl_easy;

प्रकारनिर्देश गणना {
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
ध्रुव संरचना curltime *Curl_pgrs_now(संरचना Curl_easy *data);

पूर्णाङ्क Curl_pgrsDone(संरचना Curl_easy *data, ध्रुव संरचना curltime *pnow);
शून्य Curl_pgrsStart(संरचना Curl_easy *data, ध्रुव संरचना curltime *pnow);
शून्य Curl_pgrsSetDownloadSize(संरचना Curl_easy *data, curl_off_t size);
शून्य Curl_pgrsSetUploadSize(संरचना Curl_easy *data, curl_off_t size);
CURLcode Curl_pgrs_deliver_check(संरचना Curl_easy *data, size_t delta);
शून्य Curl_pgrs_deliver_inc(संरचना Curl_easy *data, size_t delta);
शून्य Curl_pgrs_download_inc(संरचना Curl_easy *data, size_t delta);
शून्य Curl_pgrs_upload_inc(संरचना Curl_easy *data, size_t delta);
शून्य Curl_pgrsSetUploadCounter(संरचना Curl_easy *data, curl_off_t size);

/* perform progress update, invoking callbacks at intervals */
CURLcode Curl_pgrsUpdate(संरचना Curl_easy *data);
CURLcode Curl_pgrsUpdateX(संरचना Curl_easy *data, ध्रुव संरचना curltime *pnow);
/* perform progress update, no callbacks invoked */
शून्य Curl_pgrsUpdate_nometer(संरचना Curl_easy *data);
/* perform progress update with callbacks and speed checks */
CURLcode Curl_pgrsCheck(संरचना Curl_easy *data);
CURLcode Curl_pgrsCheckX(संरचना Curl_easy *data, ध्रुव संरचना curltime *pnow);

/* Inform progress/speedcheck about receive/send pausing */
शून्य Curl_pgrsRecvPause(संरचना Curl_easy *data, bool enable);
शून्य Curl_pgrsSendPause(संरचना Curl_easy *data, bool enable);

/* Reset sizes and counters for up- and download. */
शून्य Curl_pgrsReset(संरचना Curl_easy *data);
/* Reset sizes for up- and download. */
शून्य Curl_pgrsResetTransferSizes(संरचना Curl_easy *data);

शून्य Curl_pgrsTime(संरचना Curl_easy *data, timerid timer);
/**
 * Update progress timer with the elapsed time from its start to `timestamp`.
 * This allows updating timers later and is used by happy eyeballing, where
 * we only want to record the winner's times.
 */
शून्य Curl_pgrsTimeWas(संरचना Curl_easy *data, timerid timer,
                      संरचना curltime timestamp);

शून्य Curl_pgrsEarlyData(संरचना Curl_easy *data, curl_off_t sent);

शून्य Curl_pgrsCompleted(संरचना Curl_easy *data);

timediff_t Curl_pgrs_since_ms(संरचना Curl_easy *data,
                              ध्रुव संरचना curltime *pnow,
                              timerid timer);

#endif /* HEADER_CURL_PROGRESS_H */
