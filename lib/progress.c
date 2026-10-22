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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_ಪ್ರಗತಿ_METER
/* Provide a string that is 7 letters long (plus the zero byte).

   @unittest 1636 */
UNITTEST ಶೂನ್ಯ time2str(ಅಕ್ಷರ *r, size_t rsize, curl_off_t seconds);
UNITTEST ಶೂನ್ಯ time2str(ಅಕ್ಷರ *r, size_t rsize, curl_off_t seconds)
{
  curl_off_t h;
  ಆದರೆ(seconds <= 0) {
    curlx_strcopy(r, rsize, STRCONST("       "));
    ಹಿಂತಿರುಗಿ;
  }
  h = seconds / 3600;
  ಆದರೆ(h <= 99) {
    curl_off_t m = (seconds - (h * 3600)) / 60;
    ಆದರೆ(h <= 9) {
      curl_off_t s = (seconds - (h * 3600)) - (m * 60);
      ಆದರೆ(h)
        curl_msnprintf(r, rsize, "%" FMT_OFF_T ":%02" FMT_OFF_T ":"
                       "%02" FMT_OFF_T, h, m, s);
      ಇಲ್ಲದಿದ್ದರೆ
        curl_msnprintf(r, rsize, "  %02" FMT_OFF_T ":%02" FMT_OFF_T, m, s);
    }
    ಇಲ್ಲದಿದ್ದರೆ
      curl_msnprintf(r, rsize, "%" FMT_OFF_T "h %02" FMT_OFF_T "m", h, m);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    curl_off_t d = seconds / 86400;
    h = (seconds - (d * 86400)) / 3600;
    ಆದರೆ(d <= 99)
      curl_msnprintf(r, rsize, "%2" FMT_OFF_T "d %02" FMT_OFF_T "h", d, h);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(d <= 999)
      curl_msnprintf(r, rsize, "%6" FMT_OFF_T "d", d);
    ಇಲ್ಲದಿದ್ದರೆ { /* more than 999 days */
      curl_off_t m = d / 30;
      ಆದರೆ(m <= 999)
        curl_msnprintf(r, rsize, "%6" FMT_OFF_T "m", m);
      ಇಲ್ಲದಿದ್ದರೆ { /* more than 999 months */
        curl_off_t y = d / 365;
        ಆದರೆ(y <= 99999)
          curl_msnprintf(r, rsize, "%6" FMT_OFF_T "y", y);
        ಇಲ್ಲದಿದ್ದರೆ
          curlx_strcopy(r, rsize, STRCONST(">99999y"));
      }
    }
  }
}

/* The point of this function would be to return a string of the input data,
   but never longer than 6 columns (+ one zero byte).
   Add suffix k, M, G when suitable...

   @unittest 1636 */
UNITTEST ಅಕ್ಷರ *max6out(curl_off_t ಬೈಟುಗಳು, ಅಕ್ಷರ *max6, size_t mlen);
UNITTEST ಅಕ್ಷರ *max6out(curl_off_t ಬೈಟುಗಳು, ಅಕ್ಷರ *max6, size_t mlen)
{
  /* a signed 64-bit value is 8192 petabytes maximum, shown as
     8.0E (exabytes)*/
  ಆದರೆ(ಬೈಟುಗಳು < 100000)
    curl_msnprintf(max6, mlen, "%6" CURL_FORMAT_CURL_OFF_T, ಬೈಟುಗಳು);
  ಇಲ್ಲದಿದ್ದರೆ {
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ unit[] = { 'k', 'M', 'G', 'T', 'P', 'E', 0 };
    ಪೂರ್ಣಾಂಕ k = 0;
    curl_off_t nbytes;
    curl_off_t rest;
    ಮಾಡು {
      nbytes = ಬೈಟುಗಳು / 1024;
      ಆದರೆ(nbytes < 1000)
        ನಿಲ್ಲಿಸು;
      ಬೈಟುಗಳು = nbytes;
      k++;
      DEBUGASSERT(unit[k]);
    } ಆಗ(unit[k]);
    rest = ಬೈಟುಗಳು % 1024;
    ಆದರೆ(nbytes <= 99)
      /* xx.yyU */
      curl_msnprintf(max6, mlen, "%2" CURL_FORMAT_CURL_OFF_T
                     ".%02" CURL_FORMAT_CURL_OFF_T "%c", nbytes,
                     rest * 100 / 1024, unit[k]);
    ಇಲ್ಲದಿದ್ದರೆ
      /* xxx.yU */
      curl_msnprintf(max6, mlen, "%3" CURL_FORMAT_CURL_OFF_T
                     ".%" CURL_FORMAT_CURL_OFF_T "%c", nbytes,
                     rest * 10 / 1024, unit[k]);
  }
  ಹಿಂತಿರುಗಿ max6;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ pgrs_speedinit(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  memset(&ದತ್ತಾಂಶ->state.keeps_ವೇಗ, 0, ಗಾತ್ರ(ರಚನೆ curltime));
}

/*
 * @unittest 1606
 */
UNITTEST CURLcode pgrs_speedcheck(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow);
UNITTEST CURLcode pgrs_speedcheck(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  ಆದರೆ(!ದತ್ತಾಂಶ->ಗಣ.ಕೆಳಗಿನ_ವೇಗ_ಸಮಯ || !ದತ್ತಾಂಶ->ಗಣ.ಕೆಳಗಿನ_ವೇಗ_ಎಲ್ಲೆ ||
     Curl_xfer_recv_is_paused(ದತ್ತಾಂಶ) || Curl_xfer_send_is_paused(ದತ್ತಾಂಶ))
    /* A paused transfer is not qualified for speed checks */
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಆದರೆ(ದತ್ತಾಂಶ->ಪ್ರಗತಿ.current_ವೇಗ >= 0) {
    ಆದರೆ(ದತ್ತಾಂಶ->ಪ್ರಗತಿ.current_ವೇಗ < ದತ್ತಾಂಶ->ಗಣ.ಕೆಳಗಿನ_ವೇಗ_ಎಲ್ಲೆ) {
      ಆದರೆ(!ದತ್ತಾಂಶ->state.keeps_ವೇಗ.tv_sec)
        /* under the limit at this moment */
        ದತ್ತಾಂಶ->state.keeps_ವೇಗ = *pnow;
      ಇಲ್ಲದಿದ್ದರೆ {
        /* how long has it been under the limit */
        timediff_t howlong =
          curlx_ptimediff_ms(pnow, &ದತ್ತಾಂಶ->state.keeps_ವೇಗ);

        ಆದರೆ(howlong >= ದತ್ತಾಂಶ->ಗಣ.ಕೆಳಗಿನ_ವೇಗ_ಸಮಯ * 1000) {
          /* too long */
          failf(ದತ್ತಾಂಶ, "Operation too slow. Less than %" FMT_OFF_T
                " bytes/sec transferred the last %u seconds",
                ದತ್ತಾಂಶ->ಗಣ.ಕೆಳಗಿನ_ವೇಗ_ಎಲ್ಲೆ, ದತ್ತಾಂಶ->ಗಣ.ಕೆಳಗಿನ_ವೇಗ_ಸಮಯ);
          ಹಿಂತಿರುಗಿ CURLE_OPERATION_TIMEDOUT;
        }
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ
      /* faster right now */
      ದತ್ತಾಂಶ->state.keeps_ವೇಗ.tv_sec = 0;
  }

  /* since low speed limit is enabled, set the expire timer to make this
     connection's speed get checked again in a second */
  Curl_expire_ಗಣ(ದತ್ತಾಂಶ, EXPIRE_SPEEDCHECK, 1000, pnow);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *Curl_pgrs_now(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  curlx_pnow(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.now);
  ಹಿಂತಿರುಗಿ &ದತ್ತಾಂಶ->ಪ್ರಗತಿ.now;
}

/* New proposed interface, 9th of February 2000:

   pgrsStartNow() - sets start time
   pgrsSetDownloadSize(x) - known expected download size
   pgrsSetUploadSize(x) - known expected upload size
   pgrsSetDownloadCounter() - amount of data currently downloaded
   pgrsSetUploadCounter() - amount of data currently uploaded
   pgrsUpdate() - show progress
   pgrsDone() - transfer complete
 */

ಪೂರ್ಣಾಂಕ Curl_pgrs_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  ಪೂರ್ಣಾಂಕ rc;
  ದತ್ತಾಂಶ->ಪ್ರಗತಿ.delta.lastshow_us = -1;
  rc = Curl_pgrsUpdateX(ದತ್ತಾಂಶ, pnow); /* the final (forced) update */
  ಆದರೆ(rc)
    ಹಿಂತಿರುಗಿ rc;

  ಆದರೆ(!ದತ್ತಾಂಶ->ಪ್ರಗತಿ.hide && !ದತ್ತಾಂಶ->ಪ್ರಗತಿ.callback)
    /* only output if we do not use a progress callback and we are not
     * hidden */
    curl_mfprintf(ದತ್ತಾಂಶ->ಗಣ.err, "\n");

  ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ Curl_pgrsReset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  Curl_pgrs_ಗಣ_Upload_Counter(ದತ್ತಾಂಶ, 0);
  ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.cur_ಗಾತ್ರ = 0;
  Curl_pgrs_ಗಣ_Upload_ಗಾತ್ರ(ದತ್ತಾಂಶ, -1);
  Curl_pgrs_ಗಣ_Download_ಗಾತ್ರ(ದತ್ತಾಂಶ, -1);
  ದತ್ತಾಂಶ->ಪ್ರಗತಿ.speeder_c = 0; /* reset speed records */
  ದತ್ತಾಂಶ->ಪ್ರಗತಿ.deliver = 0;
  pgrs_speedinit(ದತ್ತಾಂಶ);
}

/* reset the known transfer sizes */
ಶೂನ್ಯ Curl_pgrsResetTransferSizes(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  Curl_pgrs_ಗಣ_Download_ಗಾತ್ರ(ದತ್ತಾಂಶ, -1);
  Curl_pgrs_ಗಣ_Upload_ಗಾತ್ರ(ದತ್ತಾಂಶ, -1);
}

ಶೂನ್ಯ Curl_pgrsRecvPause(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool enable)
{
  ಆದರೆ(!enable) {
    ದತ್ತಾಂಶ->ಪ್ರಗತಿ.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(ದತ್ತಾಂಶ); /* reset low speed measurements */
  }
}

ಶೂನ್ಯ Curl_pgrsSendPause(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool enable)
{
  ಆದರೆ(!enable) {
    ದತ್ತಾಂಶ->ಪ್ರಗತಿ.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(ದತ್ತಾಂಶ); /* reset low speed measurements */
  }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ pgrs_timer_ಹೆಸರುಗಳು[] = {
  "PGRS-NONE",
  "PGRS-STARTOP",
  "PGRS-STARTSINGLE",
  "PGRS-POSTQUEUE",
  "PGRS-NAMELOOKUP",
  "PGRS-CONNECT",
  "PGRS-APPCONNECT",
  "PGRS-PRETRANSFER",
  "PGRS-STARTTRANSFER",
  "PGRS-POSTRANSFER",
  "PGRS-REDIRECT",
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pgrs_timer_name(timerid timer)
{
  ಆದರೆ((size_t)timer < CURL_ARRAYSIZE(pgrs_timer_ಹೆಸರುಗಳು))
    ಹಿಂತಿರುಗಿ pgrs_timer_ಹೆಸರುಗಳು[(size_t)timer];
  ಹಿಂತಿರುಗಿ "?";
}
#ಆದರೆ_ಅಂತ್ಯ /* CURLVERBOSE */

/*
 * Curl_pgrsTimeWas(). Store the timestamp time at the given label.
 */
ಶೂನ್ಯ Curl_pgrs_ಸಮಯ_Was(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, timerid timer,
                      ರಚನೆ curltime timestamp)
{
  timediff_t *delta = ಖಾಲಿ;

  ಆಯ್ಕೆ(timer) {
  ಪೂರ್ವನಿಯೋಜಿತ:
  ಸ್ಥಿತಿ TIMER_NONE:
    /* mistake filter */
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ TIMER_STARTOP:
    /* This is set at the start of a transfer */
    ದತ್ತಾಂಶ->ಪ್ರಗತಿ.delta.startop_us =
      curlx_ptimediff_us(&timestamp, &ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಆರಂಭ);
    ದತ್ತಾಂಶ->ಪ್ರಗತಿ.delta.startqueue_us = ದತ್ತಾಂಶ->ಪ್ರಗತಿ.delta.startop_us;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ TIMER_STARTSINGLE:
    /* This is set at the start of each single transfer */
    ದತ್ತಾಂಶ->ಪ್ರಗತಿ.delta.startsingle_us =
      curlx_ptimediff_us(&timestamp, &ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಆರಂಭ);
    ದತ್ತಾಂಶ->ಪ್ರಗತಿ.startransfer_added = FALSE;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ TIMER_POSTQUEUE:
    /* Queue time is accumulative from all involved redirects */
    ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಒಟ್ಟು.queued_us +=
      curlx_ptimediff_us(&timestamp, &ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಆರಂಭ) -
      ದತ್ತಾಂಶ->ಪ್ರಗತಿ.delta.startqueue_us;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ TIMER_NAMELOOKUP:
    delta = &ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಒಟ್ಟು.nslookup_us;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ TIMER_CONNECT:
    delta = &ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಒಟ್ಟು.connect_us;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ TIMER_APPCONNECT:
    delta = &ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಒಟ್ಟು.appconnect_us;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ TIMER_PRETRANSFER:
    delta = &ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಒಟ್ಟು.pretransfer_us;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ TIMER_STARTTRANSFER:
    /* prevent updating t_starttransfer unless:
     *   1. this is the first time we are setting t_starttransfer
     *   2. a redirect has occurred since the last time t_starttransfer was set
     * This prevents repeated invocations of the function from incorrectly
     * changing the t_starttransfer time.
     */
    ಆದರೆ(ದತ್ತಾಂಶ->ಪ್ರಗತಿ.startransfer_added) {
      CURL_TRC_M(ದತ್ತಾಂಶ, "[%s] ignored", pgrs_timer_name(timer));
      ಹಿಂತಿರುಗಿ;
    }
    ದತ್ತಾಂಶ->ಪ್ರಗತಿ.startransfer_added = TRUE;
    delta = &ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಒಟ್ಟು.starttransfer_us;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ TIMER_POSTRANSFER:
    delta = &ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಒಟ್ಟು.posttransfer_us;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ TIMER_REDIRECT:
    ದತ್ತಾಂಶ->ಪ್ರಗತಿ.delta.startredirect_us =
      curlx_ptimediff_us(&timestamp, &ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಆರಂಭ);
    /* transfer starts queueing again */
    ದತ್ತಾಂಶ->ಪ್ರಗತಿ.delta.startqueue_us =
      ದತ್ತಾಂಶ->ಪ್ರಗತಿ.delta.startredirect_us;
    ನಿಲ್ಲಿಸು;
  }
  ಆದರೆ(delta) {
    timediff_t us = curlx_ptimediff_us(&timestamp, &ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಆರಂಭ) -
                    ದತ್ತಾಂಶ->ಪ್ರಗತಿ.delta.startsingle_us;
    ಆದರೆ(us < 1)
      us = 1; /* make sure at least one microsecond passed */
    *delta += us;
    CURL_TRC_M(ದತ್ತಾಂಶ, "[%s] added %" FMT_TIMEDIFF_T "us",
               pgrs_timer_name(timer), us);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    CURL_TRC_M(ದತ್ತಾಂಶ, "[%s] set", pgrs_timer_name(timer));
}

/*
 * Curl_pgrsTime(). Store the current time at the given label. This fetches a
 * fresh "now" and returns it.
 *
 * @unittest: 1399
 */
ಶೂನ್ಯ Curl_pgrs_ಸಮಯ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, timerid timer)
{
  Curl_pgrs_ಸಮಯ_Was(ದತ್ತಾಂಶ, timer, *Curl_pgrs_now(ದತ್ತಾಂಶ));
}

ಶೂನ್ಯ Curl_pgrs_ಆರಂಭ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  ರಚನೆ ಪ್ರಗತಿ_2 *p = &ದತ್ತಾಂಶ->ಪ್ರಗತಿ;

  ಆದರೆ(!pnow)
    pnow = Curl_pgrs_now(ದತ್ತಾಂಶ);
  p->speeder_c = 0; /* reset the progress meter display */
  p->ಆರಂಭ = *pnow;
  memset(&p->delta, 0, ಗಾತ್ರ(p->delta));
  memset(&p->ಒಟ್ಟು, 0, ಗಾತ್ರ(p->ಒಟ್ಟು));
  p->startransfer_added = FALSE;
  p->dl.cur_ಗಾತ್ರ = 0;
  p->ul.cur_ಗಾತ್ರ = 0;
  /* the sizes are unknown at start */
  p->dl_ಗಾತ್ರ_ತಿಳಿದ = FALSE;
  p->ul_ಗಾತ್ರ_ತಿಳಿದ = FALSE;
}

/* check that the 'delta' amount of bytes are okay to deliver to the
   application, or return error if not. */
CURLcode Curl_pgrs_deliver_check(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, size_t delta)
{
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.max_filesize &&
     ((curl_off_t)delta > ದತ್ತಾಂಶ->ಗಣ.max_filesize - ದತ್ತಾಂಶ->ಪ್ರಗತಿ.deliver)) {
    failf(ದತ್ತಾಂಶ, "Would have exceeded max file size");
    ಹಿಂತಿರುಗಿ CURLE_FILESIZE_EXCEEDED;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* this counts how much data is delivered to the application, which
   in compressed cases may differ from downloaded amount */
ಶೂನ್ಯ Curl_pgrs_deliver_inc(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, size_t delta)
{
  ದತ್ತಾಂಶ->ಪ್ರಗತಿ.deliver += delta;
}

ಶೂನ್ಯ Curl_pgrs_download_inc(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, size_t delta)
{
  ಆದರೆ(delta) {
    ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.cur_ಗಾತ್ರ += delta;
    Curl_rlimit_drain(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.rlimit, delta, ಖಾಲಿ);
  }
}

ಶೂನ್ಯ Curl_pgrs_upload_inc(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, size_t delta)
{
  ಆದರೆ(delta) {
    ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul.cur_ಗಾತ್ರ += delta;
    Curl_rlimit_drain(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul.rlimit, delta, ಖಾಲಿ);
  }
}

/*
 * Set the number of uploaded bytes so far.
 */
ಶೂನ್ಯ Curl_pgrs_ಗಣ_Upload_Counter(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, curl_off_t ಗಾತ್ರ_2)
{
  ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul.cur_ಗಾತ್ರ = ಗಾತ್ರ_2;
}

ಶೂನ್ಯ Curl_pgrs_ಗಣ_Download_ಗಾತ್ರ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, curl_off_t ಗಾತ್ರ_2)
{
  ಆದರೆ(ಗಾತ್ರ_2 >= 0) {
    ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.ಒಟ್ಟು_ಗಾತ್ರ = ಗಾತ್ರ_2;
    ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl_ಗಾತ್ರ_ತಿಳಿದ = TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.ಒಟ್ಟು_ಗಾತ್ರ = 0;
    ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl_ಗಾತ್ರ_ತಿಳಿದ = FALSE;
  }
}

ಶೂನ್ಯ Curl_pgrs_ಗಣ_Upload_ಗಾತ್ರ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, curl_off_t ಗಾತ್ರ_2)
{
  ಆದರೆ(ಗಾತ್ರ_2 >= 0) {
    ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul.ಒಟ್ಟು_ಗಾತ್ರ = ಗಾತ್ರ_2;
    ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul_ಗಾತ್ರ_ತಿಳಿದ = TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul.ಒಟ್ಟು_ಗಾತ್ರ = 0;
    ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul_ಗಾತ್ರ_ತಿಳಿದ = FALSE;
  }
}

ಶೂನ್ಯ Curl_pgrs_Early_ದತ್ತಾಂಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, curl_off_t ಕಳುಹಿಸಿದ)
{
  ದತ್ತಾಂಶ->ಪ್ರಗತಿ.earlydata_ಕಳುಹಿಸಿದ = ಕಳುಹಿಸಿದ;
}

/* returns the average speed in bytes / second */
ಸ್ಥಿರ curl_off_t trspeed(curl_off_t ಗಾತ್ರ_2, /* number of bytes */
                          curl_off_t us)   /* microseconds */
{
  ಆದರೆ(us < 1)
    ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2 * 1000000;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಗಾತ್ರ_2 < CURL_OFF_T_MAX / 1000000)
    ಹಿಂತಿರುಗಿ (ಗಾತ್ರ_2 * 1000000) / us;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(us >= 1000000)
    ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2 / (us / 1000000);
  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ CURL_OFF_T_MAX;
}

/* returns TRUE if it is time to show the progress meter */
ಸ್ಥಿರ bool ಪ್ರಗತಿ_calc(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  ರಚನೆ ಪ್ರಗತಿ_2 * ಸ್ಥಿರಾಂಕ p = &ದತ್ತಾಂಶ->ಪ್ರಗತಿ;
  ಪೂರ್ಣಾಂಕ i_ಮುಂದಿನ, i_oldest, i_latest;
  timediff_t duration_us, elapsed_us;
  curl_off_t amount;

  /* The time spent so far (from the start) in microseconds */
  elapsed_us = curlx_ptimediff_us(pnow, &p->ಆರಂಭ);
  p->ಒಟ್ಟು.spent_us = elapsed_us;
  p->dl.ವೇಗ = trspeed(p->dl.cur_ಗಾತ್ರ, p->ಒಟ್ಟು.spent_us);
  p->ul.ವೇಗ = trspeed(p->ul.cur_ಗಾತ್ರ, p->ಒಟ್ಟು.spent_us);

  ಆದರೆ(!p->speeder_c) { /* no previous record exists */
    p->ವೇಗ_amount[0] = p->dl.cur_ಗಾತ್ರ + p->ul.cur_ಗಾತ್ರ;
    p->ವೇಗ_ಸಮಯ[0] = elapsed_us;
    p->speeder_c++;
    /* use the overall average at the start */
    p->current_ವೇಗ = p->ul.ವೇಗ + p->dl.ವೇಗ;
    p->delta.lastshow_us = elapsed_us;
    ಹಿಂತಿರುಗಿ TRUE;
  }
  /* We have at least one record now. Where to put the next and
   * where is the latest one? */
  i_ಮುಂದಿನ = p->speeder_c % CURL_ವೇಗ_ದಾಖಲೆಗಳು;
  i_latest = (i_ಮುಂದಿನ > 0) ? (i_ಮುಂದಿನ - 1) : (CURL_ವೇಗ_ದಾಖಲೆಗಳು - 1);

  /* Make a new record only when some time has passed.
   * Too frequent calls otherwise ruin the history. */
  ಆದರೆ((elapsed_us - p->ವೇಗ_ಸಮಯ[i_latest]) >= (1000 * 1000)) {
    p->speeder_c++;
    i_latest = i_ಮುಂದಿನ;
    p->ವೇಗ_amount[i_latest] = p->dl.cur_ಗಾತ್ರ + p->ul.cur_ಗಾತ್ರ;
    p->ವೇಗ_ಸಮಯ[i_latest] = elapsed_us;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->req.ಮುಗಿದ) {
    /* When a transfer is done, and we did not have a current speed
     * already, update the last record. Otherwise, stay at the speed
     * we have. The last chunk of data, when rate limiting, would increase
     * reported speed since it no longer measures a full second. */
    ಆದರೆ(!p->current_ವೇಗ) {
      p->ವೇಗ_amount[i_latest] = p->dl.cur_ಗಾತ್ರ + p->ul.cur_ಗಾತ್ರ;
      p->ವೇಗ_ಸಮಯ[i_latest] = elapsed_us;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* transfer ongoing, wait for more time to pass. */
    ಹಿಂತಿರುಗಿ FALSE;
  }

  i_oldest = (p->speeder_c < CURL_ವೇಗ_ದಾಖಲೆಗಳು) ? 0 :
             ((i_latest + 1) % CURL_ವೇಗ_ದಾಖಲೆಗಳು);

  /* How much we transferred between oldest and current records */
  amount = p->ವೇಗ_amount[i_latest] - p->ವೇಗ_amount[i_oldest];
  /* How long this took */
  duration_us = p->ವೇಗ_ಸಮಯ[i_latest] - p->ವೇಗ_ಸಮಯ[i_oldest];
  ಆದರೆ(duration_us <= 0)
    duration_us = 1;

  ಆದರೆ(amount > (CURL_OFF_T_MAX / 1000000)) {
    /* the 'amount' value is bigger than would fit in 64 bits if
       multiplied with 1000000, so we use the double math for this */
    p->current_ವೇಗ =
      (curl_off_t)(((ದ್ವಿಗುಣ)amount * 1000000.0) / (ದ್ವಿಗುಣ)duration_us);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    p->current_ವೇಗ = amount * 1000000 / duration_us;
  }

  ಆದರೆ((p->delta.lastshow_us >= 0) && !ದತ್ತಾಂಶ->req.ಮುಗಿದ &&
     ((elapsed_us - p->delta.lastshow_us) < (1000 * 1000)))
    ಹಿಂತಿರುಗಿ FALSE;
  p->delta.lastshow_us = elapsed_us;
  ಹಿಂತಿರುಗಿ TRUE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_ಪ್ರಗತಿ_METER

ರಚನೆ pgrs_estimate {
  curl_off_t secs;
  curl_off_t percent;
};

ಸ್ಥಿರ curl_off_t pgrs_est_percent(curl_off_t ಒಟ್ಟು, curl_off_t cur)
{
  ಆದರೆ(ಒಟ್ಟು > 10000)
    ಹಿಂತಿರುಗಿ cur / (ಒಟ್ಟು / 100);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಒಟ್ಟು > 0)
    ಹಿಂತಿರುಗಿ (cur * 100) / ಒಟ್ಟು;
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ pgrs_estimates(ರಚನೆ pgrs_dir *d,
                           bool ಒಟ್ಟು_ತಿಳಿದ,
                           ರಚನೆ pgrs_estimate *est)
{
  est->secs = 0;
  est->percent = 0;
  ಆದರೆ(ಒಟ್ಟು_ತಿಳಿದ && (d->ವೇಗ > 0)) {
    est->secs = d->ಒಟ್ಟು_ಗಾತ್ರ / d->ವೇಗ;
    est->percent = pgrs_est_percent(d->ಒಟ್ಟು_ಗಾತ್ರ, d->cur_ಗಾತ್ರ);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ ಪ್ರಗತಿ_meter(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ಪ್ರಗತಿ_2 *p = &ದತ್ತಾಂಶ->ಪ್ರಗತಿ;
  ಅಕ್ಷರ max6[6][7];
  ರಚನೆ pgrs_estimate dl_estm;
  ರಚನೆ pgrs_estimate ul_estm;
  ರಚನೆ pgrs_estimate ಒಟ್ಟು_estm;
  curl_off_t ಒಟ್ಟು_cur_ಗಾತ್ರ;
  curl_off_t ಒಟ್ಟು_expected_ಗಾತ್ರ;
  curl_off_t dl_ಗಾತ್ರ;
  ಅಕ್ಷರ ಸಮಯ_ಎಡ[8];
  ಅಕ್ಷರ ಸಮಯ_ಒಟ್ಟು[8];
  ಅಕ್ಷರ ಸಮಯ_spent[8];
  curl_off_t cur_secs = (curl_off_t)p->ಒಟ್ಟು.spent_us / 1000000;

  ಆದರೆ(!p->ಶೀರ್ಷಿಕೆಗಳು_ಹೊರಹೋಗು) {
    ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from) {
      curl_mfprintf(ದತ್ತಾಂಶ->ಗಣ.err,
                    "** Resuming transfer from byte position %" FMT_OFF_T "\n",
                    ದತ್ತಾಂಶ->state.resume_from);
    }
    curl_mfprintf(ದತ್ತಾಂಶ->ಗಣ.err,
                  "  %% Total    %% Received %% Xferd  Average Speed  "
                  "Time    Time    Time   Current\n"
                  "                                 Dload  Upload  "
                  "Total   Spent   Left   Speed\n");
    p->ಶೀರ್ಷಿಕೆಗಳು_ಹೊರಹೋಗು = TRUE; /* headers are shown */
  }

  /* Figure out the estimated time of arrival for upload and download */
  pgrs_estimates(&p->ul, (bool)p->ul_ಗಾತ್ರ_ತಿಳಿದ, &ul_estm);
  pgrs_estimates(&p->dl, (bool)p->dl_ಗಾತ್ರ_ತಿಳಿದ, &dl_estm);

  /* Since both happen at the same time, total expected duration is max. */
  ಒಟ್ಟು_estm.secs = CURLMAX(ul_estm.secs, dl_estm.secs);
  /* create the three time strings */
  time2str(ಸಮಯ_ಎಡ, ಗಾತ್ರ(ಸಮಯ_ಎಡ),
           ಒಟ್ಟು_estm.secs > 0 ? (ಒಟ್ಟು_estm.secs - cur_secs) : 0);
  time2str(ಸಮಯ_ಒಟ್ಟು, ಗಾತ್ರ(ಸಮಯ_ಒಟ್ಟು), ಒಟ್ಟು_estm.secs);
  time2str(ಸಮಯ_spent, ಗಾತ್ರ(ಸಮಯ_spent), cur_secs);

  /* Get the total amount of data expected to get transferred */
  ಒಟ್ಟು_expected_ಗಾತ್ರ = p->ul_ಗಾತ್ರ_ತಿಳಿದ ? p->ul.ಒಟ್ಟು_ಗಾತ್ರ : p->ul.cur_ಗಾತ್ರ;

  dl_ಗಾತ್ರ = p->dl_ಗಾತ್ರ_ತಿಳಿದ ? p->dl.ಒಟ್ಟು_ಗಾತ್ರ : p->dl.cur_ಗಾತ್ರ;

  /* integer overflow check */
  ಆದರೆ((CURL_OFF_T_MAX - ಒಟ್ಟು_expected_ಗಾತ್ರ) < dl_ಗಾತ್ರ)
    ಒಟ್ಟು_expected_ಗಾತ್ರ = CURL_OFF_T_MAX; /* capped */
  ಇಲ್ಲದಿದ್ದರೆ
    ಒಟ್ಟು_expected_ಗಾತ್ರ += dl_ಗಾತ್ರ;

  /* We have transferred this much so far */
  ಒಟ್ಟು_cur_ಗಾತ್ರ = p->dl.cur_ಗಾತ್ರ + p->ul.cur_ಗಾತ್ರ;

  /* Get the percentage of data transferred so far */
  ಒಟ್ಟು_estm.percent = pgrs_est_percent(ಒಟ್ಟು_expected_ಗಾತ್ರ, ಒಟ್ಟು_cur_ಗಾತ್ರ);

  curl_mfprintf(ದತ್ತಾಂಶ->ಗಣ.err,
                "\r"
                "%3" FMT_OFF_T " %s "
                "%3" FMT_OFF_T " %s "
                "%3" FMT_OFF_T " %s %s %s %s %s %s %s",
                ಒಟ್ಟು_estm.percent, /* 3 letters */    /* total % */
                max6out(ಒಟ್ಟು_expected_ಗಾತ್ರ, max6[2],
                        ಗಾತ್ರ(max6[2])),              /* total size */
                dl_estm.percent, /* 3 letters */       /* rcvd % */
                max6out(p->dl.cur_ಗಾತ್ರ, max6[0],
                        ಗಾತ್ರ(max6[0])),              /* rcvd size */
                ul_estm.percent, /* 3 letters */       /* xfer % */
                max6out(p->ul.cur_ಗಾತ್ರ, max6[1],
                        ಗಾತ್ರ(max6[1])),              /* xfer size */
                max6out(p->dl.ವೇಗ, max6[3],
                        ಗಾತ್ರ(max6[3])),              /* avrg dl speed */
                max6out(p->ul.ವೇಗ, max6[4],
                        ಗಾತ್ರ(max6[4])),              /* avrg ul speed */
                ಸಮಯ_ಒಟ್ಟು,    /* 7 letters */         /* total time */
                ಸಮಯ_spent,    /* 7 letters */         /* time spent */
                ಸಮಯ_ಎಡ,     /* 7 letters */         /* time left */
                max6out(p->current_ವೇಗ, max6[5],
                        ಗಾತ್ರ(max6[5]))               /* current speed */
    );

  /* we flush the output stream to make it appear as soon as possible */
  fflush(ದತ್ತಾಂಶ->ಗಣ.err);
}
#ಇಲ್ಲದಿದ್ದರೆ /* CURL_DISABLE_PROGRESS_METER */
#ವ್ಯಾಖ್ಯಾನಿಸು ಪ್ರಗತಿ_meter(x) Curl_nop_stmt
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Curl_pgrsUpdate() returns 0 for success or the value returned by the
 * progress callback!
 */
ಸ್ಥಿರ CURLcode pgrsupdate(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool showprogress)
{
  ಆದರೆ(!ದತ್ತಾಂಶ->ಪ್ರಗತಿ.hide) {
    ಪೂರ್ಣಾಂಕ rc;
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.fxferinfo) {
      /* There is a callback set, call that */
      ರಚನೆ Curl_mapi_guard guard;
      CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_fxferinfo);
      rc = ದತ್ತಾಂಶ->ಗಣ.fxferinfo(ದತ್ತಾಂಶ->ಗಣ.ಪ್ರಗತಿ_client,
                               ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.ಒಟ್ಟು_ಗಾತ್ರ,
                               ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.cur_ಗಾತ್ರ,
                               ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul.ಒಟ್ಟು_ಗಾತ್ರ,
                               ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul.cur_ಗಾತ್ರ);
      CURL_CBAPI_ಕೊನೆ(&guard);
      ಆದರೆ(rc != CURL_PROGRESSFUNC_CONTINUE) {
        ಆದರೆ(rc) {
          failf(ದತ್ತಾಂಶ, "Callback aborted");
          ಹಿಂತಿರುಗಿ CURLE_ABORTED_BY_CALLBACK;
        }
        ಹಿಂತಿರುಗಿ CURLE_OK;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.fprogress) {
      /* The older deprecated callback is set, call that */
      ರಚನೆ Curl_mapi_guard guard;
      CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_fprogress);
      rc = ದತ್ತಾಂಶ->ಗಣ.fprogress(ದತ್ತಾಂಶ->ಗಣ.ಪ್ರಗತಿ_client,
                               (ದ್ವಿಗುಣ)ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.ಒಟ್ಟು_ಗಾತ್ರ,
                               (ದ್ವಿಗುಣ)ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.cur_ಗಾತ್ರ,
                               (ದ್ವಿಗುಣ)ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul.ಒಟ್ಟು_ಗಾತ್ರ,
                               (ದ್ವಿಗುಣ)ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ul.cur_ಗಾತ್ರ);
      CURL_CBAPI_ಕೊನೆ(&guard);
      ಆದರೆ(rc != CURL_PROGRESSFUNC_CONTINUE) {
        ಆದರೆ(rc) {
          failf(ದತ್ತಾಂಶ, "Callback aborted");
          ಹಿಂತಿರುಗಿ CURLE_ABORTED_BY_CALLBACK;
        }
        ಹಿಂತಿರುಗಿ CURLE_OK;
      }
    }

    ಆದರೆ(showprogress)
      ಪ್ರಗತಿ_meter(ದತ್ತಾಂಶ);
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode pgrs_update(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  bool showprogress = ಪ್ರಗತಿ_calc(ದತ್ತಾಂಶ, pnow);
  ಹಿಂತಿರುಗಿ pgrsupdate(ದತ್ತಾಂಶ, showprogress);
}

CURLcode Curl_pgrsUpdate(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಹಿಂತಿರುಗಿ pgrs_update(ದತ್ತಾಂಶ, Curl_pgrs_now(ದತ್ತಾಂಶ));
}

CURLcode Curl_pgrsUpdateX(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  ಹಿಂತಿರುಗಿ pgrs_update(ದತ್ತಾಂಶ, pnow);
}

CURLcode Curl_pgrsCheckX(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  CURLcode ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = pgrs_update(ದತ್ತಾಂಶ, pnow);
  ಆದರೆ(!ಫಲಿತಾಂಶ && !ದತ್ತಾಂಶ->req.ಮುಗಿದ)
    ಫಲಿತಾಂಶ = pgrs_speedcheck(ದತ್ತಾಂಶ, pnow);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_pgrsCheck(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಹಿಂತಿರುಗಿ Curl_pgrsCheckX(ದತ್ತಾಂಶ, Curl_pgrs_now(ದತ್ತಾಂಶ));
}

/*
 * Update all progress, do not do progress meter/callbacks.
 */
ಶೂನ್ಯ Curl_pgrsUpdate_nometer(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  (ಶೂನ್ಯ)ಪ್ರಗತಿ_calc(ದತ್ತಾಂಶ, Curl_pgrs_now(ದತ್ತಾಂಶ));
}

ಶೂನ್ಯ Curl_pgrsCompleted(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ಪ್ರಗತಿ_2 * ಸ್ಥಿರಾಂಕ p = &ದತ್ತಾಂಶ->ಪ್ರಗತಿ;
  p->ಒಟ್ಟು.spent_us = curlx_ptimediff_us(Curl_pgrs_now(ದತ್ತಾಂಶ), &p->ಆರಂಭ);
}

timediff_t Curl_pgrs_since_ms(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow,
                              timerid timer)
{
  ಆದರೆ(!pnow)
    pnow = Curl_pgrs_now(ದತ್ತಾಂಶ);
  ಆಯ್ಕೆ(timer) {
  ಸ್ಥಿತಿ TIMER_STARTOP:
    ಹಿಂತಿರುಗಿ (curlx_ptimediff_us(pnow, &ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಆರಂಭ) -
            ದತ್ತಾಂಶ->ಪ್ರಗತಿ.delta.startop_us) / 1000;
  ಸ್ಥಿತಿ TIMER_STARTSINGLE:
    ಹಿಂತಿರುಗಿ (curlx_ptimediff_us(pnow, &ದತ್ತಾಂಶ->ಪ್ರಗತಿ.ಆರಂಭ) -
            ದತ್ತಾಂಶ->ಪ್ರಗತಿ.delta.startsingle_us) / 1000;
  ಪೂರ್ವನಿಯೋಜಿತ:
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ 0;
  }
}
