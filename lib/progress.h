#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_ಪ್ರಗತಿ_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_ಪ್ರಗತಿ_H
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

ರಚನೆ Curl_easy;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
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
  TIMER_ಕೊನೆಯ /* must be last */
} timerid;

/* Get the current timestamp of the transfer */
ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *Curl_pgrs_now(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

ಪೂರ್ಣಾಂಕ Curl_pgrs_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow);
ಶೂನ್ಯ Curl_pgrs_ಆರಂಭ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow);
ಶೂನ್ಯ Curl_pgrs_ಗಣ_Download_ಗಾತ್ರ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, curl_off_t ಗಾತ್ರ_2);
ಶೂನ್ಯ Curl_pgrs_ಗಣ_Upload_ಗಾತ್ರ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, curl_off_t ಗಾತ್ರ_2);
CURLcode Curl_pgrs_deliver_check(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, size_t delta);
ಶೂನ್ಯ Curl_pgrs_deliver_inc(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, size_t delta);
ಶೂನ್ಯ Curl_pgrs_download_inc(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, size_t delta);
ಶೂನ್ಯ Curl_pgrs_upload_inc(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, size_t delta);
ಶೂನ್ಯ Curl_pgrs_ಗಣ_Upload_Counter(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, curl_off_t ಗಾತ್ರ_2);

/* perform progress update, invoking callbacks at intervals */
CURLcode Curl_pgrsUpdate(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
CURLcode Curl_pgrsUpdateX(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow);
/* perform progress update, no callbacks invoked */
ಶೂನ್ಯ Curl_pgrsUpdate_nometer(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
/* perform progress update with callbacks and speed checks */
CURLcode Curl_pgrsCheck(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
CURLcode Curl_pgrsCheckX(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow);

/* Inform progress/speedcheck about receive/send pausing */
ಶೂನ್ಯ Curl_pgrsRecvPause(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool enable);
ಶೂನ್ಯ Curl_pgrsSendPause(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool enable);

/* Reset sizes and counters for up- and download. */
ಶೂನ್ಯ Curl_pgrsReset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
/* Reset sizes for up- and download. */
ಶೂನ್ಯ Curl_pgrsResetTransferSizes(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

ಶೂನ್ಯ Curl_pgrs_ಸಮಯ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, timerid timer);
/**
 * Update progress timer with the elapsed time from its start to `timestamp`.
 * This allows updating timers later and is used by happy eyeballing, where
 * we only want to record the winner's times.
 */
ಶೂನ್ಯ Curl_pgrs_ಸಮಯ_Was(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, timerid timer,
                      ರಚನೆ curltime timestamp);

ಶೂನ್ಯ Curl_pgrs_Early_ದತ್ತಾಂಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, curl_off_t ಕಳುಹಿಸಿದ);

ಶೂನ್ಯ Curl_pgrsCompleted(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

timediff_t Curl_pgrs_since_ms(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow,
                              timerid timer);

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_PROGRESS_H */
