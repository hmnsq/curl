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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "curlx/strcopy.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_పురోగతి_METER
/* Provide a string that is 7 letters long (plus the zero byte).

   @unittest 1636 */
UNITTEST శూన్య time2str(అక్షరం *r, size_t rsize, curl_off_t seconds);
UNITTEST శూన్య time2str(అక్షరం *r, size_t rsize, curl_off_t seconds)
{
  curl_off_t h;
  ఒకవేళ(seconds <= 0) {
    curlx_strcopy(r, rsize, STRCONST("       "));
    తిరిగి;
  }
  h = seconds / 3600;
  ఒకవేళ(h <= 99) {
    curl_off_t m = (seconds - (h * 3600)) / 60;
    ఒకవేళ(h <= 9) {
      curl_off_t s = (seconds - (h * 3600)) - (m * 60);
      ఒకవేళ(h)
        curl_msnprintf(r, rsize, "%" FMT_OFF_T ":%02" FMT_OFF_T ":"
                       "%02" FMT_OFF_T, h, m, s);
      లేకపోతే
        curl_msnprintf(r, rsize, "  %02" FMT_OFF_T ":%02" FMT_OFF_T, m, s);
    }
    లేకపోతే
      curl_msnprintf(r, rsize, "%" FMT_OFF_T "h %02" FMT_OFF_T "m", h, m);
  }
  లేకపోతే {
    curl_off_t d = seconds / 86400;
    h = (seconds - (d * 86400)) / 3600;
    ఒకవేళ(d <= 99)
      curl_msnprintf(r, rsize, "%2" FMT_OFF_T "d %02" FMT_OFF_T "h", d, h);
    లేకపోతే ఒకవేళ(d <= 999)
      curl_msnprintf(r, rsize, "%6" FMT_OFF_T "d", d);
    లేకపోతే { /* more than 999 days */
      curl_off_t m = d / 30;
      ఒకవేళ(m <= 999)
        curl_msnprintf(r, rsize, "%6" FMT_OFF_T "m", m);
      లేకపోతే { /* more than 999 months */
        curl_off_t y = d / 365;
        ఒకవేళ(y <= 99999)
          curl_msnprintf(r, rsize, "%6" FMT_OFF_T "y", y);
        లేకపోతే
          curlx_strcopy(r, rsize, STRCONST(">99999y"));
      }
    }
  }
}

/* The point of this function would be to return a string of the input data,
   but never longer than 6 columns (+ one zero byte).
   Add suffix k, M, G when suitable...

   @unittest 1636 */
UNITTEST అక్షరం *max6out(curl_off_t బైట్లు, అక్షరం *max6, size_t mlen);
UNITTEST అక్షరం *max6out(curl_off_t బైట్లు, అక్షరం *max6, size_t mlen)
{
  /* a signed 64-bit value is 8192 petabytes maximum, shown as
     8.0E (exabytes)*/
  ఒకవేళ(బైట్లు < 100000)
    curl_msnprintf(max6, mlen, "%6" CURL_FORMAT_CURL_OFF_T, బైట్లు);
  లేకపోతే {
    స్థిర స్థిరరాశి అక్షరం unit[] = { 'k', 'M', 'G', 'T', 'P', 'E', 0 };
    పూర్ణాంకం k = 0;
    curl_off_t nbytes;
    curl_off_t rest;
    చేయి {
      nbytes = బైట్లు / 1024;
      ఒకవేళ(nbytes < 1000)
        ఆపు;
      బైట్లు = nbytes;
      k++;
      DEBUGASSERT(unit[k]);
    } అయితే(unit[k]);
    rest = బైట్లు % 1024;
    ఒకవేళ(nbytes <= 99)
      /* xx.yyU */
      curl_msnprintf(max6, mlen, "%2" CURL_FORMAT_CURL_OFF_T
                     ".%02" CURL_FORMAT_CURL_OFF_T "%c", nbytes,
                     rest * 100 / 1024, unit[k]);
    లేకపోతే
      /* xxx.yU */
      curl_msnprintf(max6, mlen, "%3" CURL_FORMAT_CURL_OFF_T
                     ".%" CURL_FORMAT_CURL_OFF_T "%c", nbytes,
                     rest * 10 / 1024, unit[k]);
  }
  తిరిగి max6;
}
#అయితే_ముగింపు

స్థిర శూన్య pgrs_speedinit(నిర్మాణం Curl_easy *దత్తాంశం)
{
  memset(&దత్తాంశం->state.keeps_వేగం, 0, పరిమాణం(నిర్మాణం curltime));
}

/*
 * @unittest 1606
 */
UNITTEST CURLcode pgrs_speedcheck(నిర్మాణం Curl_easy *దత్తాంశం,
                                  స్థిరరాశి నిర్మాణం curltime *pnow);
UNITTEST CURLcode pgrs_speedcheck(నిర్మాణం Curl_easy *దత్తాంశం,
                                  స్థిరరాశి నిర్మాణం curltime *pnow)
{
  ఒకవేళ(!దత్తాంశం->సమితి.దిగువ_వేగం_సమయం || !దత్తాంశం->సమితి.దిగువ_వేగం_హద్దు ||
     Curl_xfer_recv_is_paused(దత్తాంశం) || Curl_xfer_send_is_paused(దత్తాంశం))
    /* A paused transfer is not qualified for speed checks */
    తిరిగి CURLE_OK;

  ఒకవేళ(దత్తాంశం->పురోగతి.current_వేగం >= 0) {
    ఒకవేళ(దత్తాంశం->పురోగతి.current_వేగం < దత్తాంశం->సమితి.దిగువ_వేగం_హద్దు) {
      ఒకవేళ(!దత్తాంశం->state.keeps_వేగం.tv_sec)
        /* under the limit at this moment */
        దత్తాంశం->state.keeps_వేగం = *pnow;
      లేకపోతే {
        /* how long has it been under the limit */
        timediff_t howlong =
          curlx_ptimediff_ms(pnow, &దత్తాంశం->state.keeps_వేగం);

        ఒకవేళ(howlong >= దత్తాంశం->సమితి.దిగువ_వేగం_సమయం * 1000) {
          /* too long */
          failf(దత్తాంశం, "Operation too slow. Less than %" FMT_OFF_T
                " bytes/sec transferred the last %u seconds",
                దత్తాంశం->సమితి.దిగువ_వేగం_హద్దు, దత్తాంశం->సమితి.దిగువ_వేగం_సమయం);
          తిరిగి CURLE_OPERATION_TIMEDOUT;
        }
      }
    }
    లేకపోతే
      /* faster right now */
      దత్తాంశం->state.keeps_వేగం.tv_sec = 0;
  }

  /* since low speed limit is enabled, set the expire timer to make this
     connection's speed get checked again in a second */
  Curl_expire_సమితి(దత్తాంశం, EXPIRE_SPEEDCHECK, 1000, pnow);

  తిరిగి CURLE_OK;
}

స్థిరరాశి నిర్మాణం curltime *Curl_pgrs_now(నిర్మాణం Curl_easy *దత్తాంశం)
{
  curlx_pnow(&దత్తాంశం->పురోగతి.now);
  తిరిగి &దత్తాంశం->పురోగతి.now;
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

పూర్ణాంకం Curl_pgrs_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి నిర్మాణం curltime *pnow)
{
  పూర్ణాంకం rc;
  దత్తాంశం->పురోగతి.delta.lastshow_us = -1;
  rc = Curl_pgrsUpdateX(దత్తాంశం, pnow); /* the final (forced) update */
  ఒకవేళ(rc)
    తిరిగి rc;

  ఒకవేళ(!దత్తాంశం->పురోగతి.hide && !దత్తాంశం->పురోగతి.callback)
    /* only output if we do not use a progress callback and we are not
     * hidden */
    curl_mfprintf(దత్తాంశం->సమితి.err, "\n");

  తిరిగి 0;
}

శూన్య Curl_pgrsReset(నిర్మాణం Curl_easy *దత్తాంశం)
{
  Curl_pgrs_సమితి_Upload_Counter(దత్తాంశం, 0);
  దత్తాంశం->పురోగతి.dl.cur_కొలత = 0;
  Curl_pgrs_సమితి_Upload_కొలత(దత్తాంశం, -1);
  Curl_pgrs_సమితి_Download_కొలత(దత్తాంశం, -1);
  దత్తాంశం->పురోగతి.speeder_c = 0; /* reset speed records */
  దత్తాంశం->పురోగతి.deliver = 0;
  pgrs_speedinit(దత్తాంశం);
}

/* reset the known transfer sizes */
శూన్య Curl_pgrsResetTransferSizes(నిర్మాణం Curl_easy *దత్తాంశం)
{
  Curl_pgrs_సమితి_Download_కొలత(దత్తాంశం, -1);
  Curl_pgrs_సమితి_Upload_కొలత(దత్తాంశం, -1);
}

శూన్య Curl_pgrsRecvPause(నిర్మాణం Curl_easy *దత్తాంశం, bool enable)
{
  ఒకవేళ(!enable) {
    దత్తాంశం->పురోగతి.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(దత్తాంశం); /* reset low speed measurements */
  }
}

శూన్య Curl_pgrsSendPause(నిర్మాణం Curl_easy *దత్తాంశం, bool enable)
{
  ఒకవేళ(!enable) {
    దత్తాంశం->పురోగతి.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(దత్తాంశం); /* reset low speed measurements */
  }
}

#నిర్వచితమైతే CURLVERBOSE
స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి pgrs_timer_పేర్లు[] = {
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

స్థిర స్థిరరాశి అక్షరం *pgrs_timer_name(timerid timer)
{
  ఒకవేళ((size_t)timer < CURL_ARRAYSIZE(pgrs_timer_పేర్లు))
    తిరిగి pgrs_timer_పేర్లు[(size_t)timer];
  తిరిగి "?";
}
#అయితే_ముగింపు /* CURLVERBOSE */

/*
 * Curl_pgrsTimeWas(). Store the timestamp time at the given label.
 */
శూన్య Curl_pgrs_సమయం_Was(నిర్మాణం Curl_easy *దత్తాంశం, timerid timer,
                      నిర్మాణం curltime timestamp)
{
  timediff_t *delta = ఖాళీ;

  ఎంపిక(timer) {
  అప్రమేయ:
  స్థితి TIMER_NONE:
    /* mistake filter */
    ఆపు;
  స్థితి TIMER_STARTOP:
    /* This is set at the start of a transfer */
    దత్తాంశం->పురోగతి.delta.startop_us =
      curlx_ptimediff_us(&timestamp, &దత్తాంశం->పురోగతి.ప్రారంభం);
    దత్తాంశం->పురోగతి.delta.startqueue_us = దత్తాంశం->పురోగతి.delta.startop_us;
    ఆపు;
  స్థితి TIMER_STARTSINGLE:
    /* This is set at the start of each single transfer */
    దత్తాంశం->పురోగతి.delta.startsingle_us =
      curlx_ptimediff_us(&timestamp, &దత్తాంశం->పురోగతి.ప్రారంభం);
    దత్తాంశం->పురోగతి.startransfer_added = FALSE;
    ఆపు;
  స్థితి TIMER_POSTQUEUE:
    /* Queue time is accumulative from all involved redirects */
    దత్తాంశం->పురోగతి.మొత్తం.queued_us +=
      curlx_ptimediff_us(&timestamp, &దత్తాంశం->పురోగతి.ప్రారంభం) -
      దత్తాంశం->పురోగతి.delta.startqueue_us;
    ఆపు;
  స్థితి TIMER_NAMELOOKUP:
    delta = &దత్తాంశం->పురోగతి.మొత్తం.nslookup_us;
    ఆపు;
  స్థితి TIMER_CONNECT:
    delta = &దత్తాంశం->పురోగతి.మొత్తం.connect_us;
    ఆపు;
  స్థితి TIMER_APPCONNECT:
    delta = &దత్తాంశం->పురోగతి.మొత్తం.appconnect_us;
    ఆపు;
  స్థితి TIMER_PRETRANSFER:
    delta = &దత్తాంశం->పురోగతి.మొత్తం.pretransfer_us;
    ఆపు;
  స్థితి TIMER_STARTTRANSFER:
    /* prevent updating t_starttransfer unless:
     *   1. this is the first time we are setting t_starttransfer
     *   2. a redirect has occurred since the last time t_starttransfer was set
     * This prevents repeated invocations of the function from incorrectly
     * changing the t_starttransfer time.
     */
    ఒకవేళ(దత్తాంశం->పురోగతి.startransfer_added) {
      CURL_TRC_M(దత్తాంశం, "[%s] ignored", pgrs_timer_name(timer));
      తిరిగి;
    }
    దత్తాంశం->పురోగతి.startransfer_added = TRUE;
    delta = &దత్తాంశం->పురోగతి.మొత్తం.starttransfer_us;
    ఆపు;
  స్థితి TIMER_POSTRANSFER:
    delta = &దత్తాంశం->పురోగతి.మొత్తం.posttransfer_us;
    ఆపు;
  స్థితి TIMER_REDIRECT:
    దత్తాంశం->పురోగతి.delta.startredirect_us =
      curlx_ptimediff_us(&timestamp, &దత్తాంశం->పురోగతి.ప్రారంభం);
    /* transfer starts queueing again */
    దత్తాంశం->పురోగతి.delta.startqueue_us =
      దత్తాంశం->పురోగతి.delta.startredirect_us;
    ఆపు;
  }
  ఒకవేళ(delta) {
    timediff_t us = curlx_ptimediff_us(&timestamp, &దత్తాంశం->పురోగతి.ప్రారంభం) -
                    దత్తాంశం->పురోగతి.delta.startsingle_us;
    ఒకవేళ(us < 1)
      us = 1; /* make sure at least one microsecond passed */
    *delta += us;
    CURL_TRC_M(దత్తాంశం, "[%s] added %" FMT_TIMEDIFF_T "us",
               pgrs_timer_name(timer), us);
  }
  లేకపోతే
    CURL_TRC_M(దత్తాంశం, "[%s] set", pgrs_timer_name(timer));
}

/*
 * Curl_pgrsTime(). Store the current time at the given label. This fetches a
 * fresh "now" and returns it.
 *
 * @unittest: 1399
 */
శూన్య Curl_pgrs_సమయం(నిర్మాణం Curl_easy *దత్తాంశం, timerid timer)
{
  Curl_pgrs_సమయం_Was(దత్తాంశం, timer, *Curl_pgrs_now(దత్తాంశం));
}

శూన్య Curl_pgrs_ప్రారంభం(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి నిర్మాణం curltime *pnow)
{
  నిర్మాణం పురోగతి_2 *p = &దత్తాంశం->పురోగతి;

  ఒకవేళ(!pnow)
    pnow = Curl_pgrs_now(దత్తాంశం);
  p->speeder_c = 0; /* reset the progress meter display */
  p->ప్రారంభం = *pnow;
  memset(&p->delta, 0, పరిమాణం(p->delta));
  memset(&p->మొత్తం, 0, పరిమాణం(p->మొత్తం));
  p->startransfer_added = FALSE;
  p->dl.cur_కొలత = 0;
  p->ul.cur_కొలత = 0;
  /* the sizes are unknown at start */
  p->dl_కొలత_తెలిసిన = FALSE;
  p->ul_కొలత_తెలిసిన = FALSE;
}

/* check that the 'delta' amount of bytes are okay to deliver to the
   application, or return error if not. */
CURLcode Curl_pgrs_deliver_check(నిర్మాణం Curl_easy *దత్తాంశం, size_t delta)
{
  ఒకవేళ(దత్తాంశం->సమితి.max_filesize &&
     ((curl_off_t)delta > దత్తాంశం->సమితి.max_filesize - దత్తాంశం->పురోగతి.deliver)) {
    failf(దత్తాంశం, "Would have exceeded max file size");
    తిరిగి CURLE_FILESIZE_EXCEEDED;
  }
  తిరిగి CURLE_OK;
}

/* this counts how much data is delivered to the application, which
   in compressed cases may differ from downloaded amount */
శూన్య Curl_pgrs_deliver_inc(నిర్మాణం Curl_easy *దత్తాంశం, size_t delta)
{
  దత్తాంశం->పురోగతి.deliver += delta;
}

శూన్య Curl_pgrs_download_inc(నిర్మాణం Curl_easy *దత్తాంశం, size_t delta)
{
  ఒకవేళ(delta) {
    దత్తాంశం->పురోగతి.dl.cur_కొలత += delta;
    Curl_rlimit_drain(&దత్తాంశం->పురోగతి.dl.rlimit, delta, ఖాళీ);
  }
}

శూన్య Curl_pgrs_upload_inc(నిర్మాణం Curl_easy *దత్తాంశం, size_t delta)
{
  ఒకవేళ(delta) {
    దత్తాంశం->పురోగతి.ul.cur_కొలత += delta;
    Curl_rlimit_drain(&దత్తాంశం->పురోగతి.ul.rlimit, delta, ఖాళీ);
  }
}

/*
 * Set the number of uploaded bytes so far.
 */
శూన్య Curl_pgrs_సమితి_Upload_Counter(నిర్మాణం Curl_easy *దత్తాంశం, curl_off_t కొలత)
{
  దత్తాంశం->పురోగతి.ul.cur_కొలత = కొలత;
}

శూన్య Curl_pgrs_సమితి_Download_కొలత(నిర్మాణం Curl_easy *దత్తాంశం, curl_off_t కొలత)
{
  ఒకవేళ(కొలత >= 0) {
    దత్తాంశం->పురోగతి.dl.మొత్తం_కొలత = కొలత;
    దత్తాంశం->పురోగతి.dl_కొలత_తెలిసిన = TRUE;
  }
  లేకపోతే {
    దత్తాంశం->పురోగతి.dl.మొత్తం_కొలత = 0;
    దత్తాంశం->పురోగతి.dl_కొలత_తెలిసిన = FALSE;
  }
}

శూన్య Curl_pgrs_సమితి_Upload_కొలత(నిర్మాణం Curl_easy *దత్తాంశం, curl_off_t కొలత)
{
  ఒకవేళ(కొలత >= 0) {
    దత్తాంశం->పురోగతి.ul.మొత్తం_కొలత = కొలత;
    దత్తాంశం->పురోగతి.ul_కొలత_తెలిసిన = TRUE;
  }
  లేకపోతే {
    దత్తాంశం->పురోగతి.ul.మొత్తం_కొలత = 0;
    దత్తాంశం->పురోగతి.ul_కొలత_తెలిసిన = FALSE;
  }
}

శూన్య Curl_pgrs_Early_దత్తాంశం(నిర్మాణం Curl_easy *దత్తాంశం, curl_off_t పంపిన)
{
  దత్తాంశం->పురోగతి.earlydata_పంపిన = పంపిన;
}

/* returns the average speed in bytes / second */
స్థిర curl_off_t trspeed(curl_off_t కొలత, /* number of bytes */
                          curl_off_t us)   /* microseconds */
{
  ఒకవేళ(us < 1)
    తిరిగి కొలత * 1000000;
  లేకపోతే ఒకవేళ(కొలత < CURL_OFF_T_MAX / 1000000)
    తిరిగి (కొలత * 1000000) / us;
  లేకపోతే ఒకవేళ(us >= 1000000)
    తిరిగి కొలత / (us / 1000000);
  లేకపోతే
    తిరిగి CURL_OFF_T_MAX;
}

/* returns TRUE if it is time to show the progress meter */
స్థిర bool పురోగతి_calc(నిర్మాణం Curl_easy *దత్తాంశం,
                          స్థిరరాశి నిర్మాణం curltime *pnow)
{
  నిర్మాణం పురోగతి_2 * స్థిరరాశి p = &దత్తాంశం->పురోగతి;
  పూర్ణాంకం i_తదుపరి, i_oldest, i_latest;
  timediff_t duration_us, elapsed_us;
  curl_off_t amount;

  /* The time spent so far (from the start) in microseconds */
  elapsed_us = curlx_ptimediff_us(pnow, &p->ప్రారంభం);
  p->మొత్తం.spent_us = elapsed_us;
  p->dl.వేగం = trspeed(p->dl.cur_కొలత, p->మొత్తం.spent_us);
  p->ul.వేగం = trspeed(p->ul.cur_కొలత, p->మొత్తం.spent_us);

  ఒకవేళ(!p->speeder_c) { /* no previous record exists */
    p->వేగం_amount[0] = p->dl.cur_కొలత + p->ul.cur_కొలత;
    p->వేగం_సమయం[0] = elapsed_us;
    p->speeder_c++;
    /* use the overall average at the start */
    p->current_వేగం = p->ul.వేగం + p->dl.వేగం;
    p->delta.lastshow_us = elapsed_us;
    తిరిగి TRUE;
  }
  /* We have at least one record now. Where to put the next and
   * where is the latest one? */
  i_తదుపరి = p->speeder_c % CURL_వేగం_రికార్డులు;
  i_latest = (i_తదుపరి > 0) ? (i_తదుపరి - 1) : (CURL_వేగం_రికార్డులు - 1);

  /* Make a new record only when some time has passed.
   * Too frequent calls otherwise ruin the history. */
  ఒకవేళ((elapsed_us - p->వేగం_సమయం[i_latest]) >= (1000 * 1000)) {
    p->speeder_c++;
    i_latest = i_తదుపరి;
    p->వేగం_amount[i_latest] = p->dl.cur_కొలత + p->ul.cur_కొలత;
    p->వేగం_సమయం[i_latest] = elapsed_us;
  }
  లేకపోతే ఒకవేళ(దత్తాంశం->req.పూర్తి) {
    /* When a transfer is done, and we did not have a current speed
     * already, update the last record. Otherwise, stay at the speed
     * we have. The last chunk of data, when rate limiting, would increase
     * reported speed since it no longer measures a full second. */
    ఒకవేళ(!p->current_వేగం) {
      p->వేగం_amount[i_latest] = p->dl.cur_కొలత + p->ul.cur_కొలత;
      p->వేగం_సమయం[i_latest] = elapsed_us;
    }
  }
  లేకపోతే {
    /* transfer ongoing, wait for more time to pass. */
    తిరిగి FALSE;
  }

  i_oldest = (p->speeder_c < CURL_వేగం_రికార్డులు) ? 0 :
             ((i_latest + 1) % CURL_వేగం_రికార్డులు);

  /* How much we transferred between oldest and current records */
  amount = p->వేగం_amount[i_latest] - p->వేగం_amount[i_oldest];
  /* How long this took */
  duration_us = p->వేగం_సమయం[i_latest] - p->వేగం_సమయం[i_oldest];
  ఒకవేళ(duration_us <= 0)
    duration_us = 1;

  ఒకవేళ(amount > (CURL_OFF_T_MAX / 1000000)) {
    /* the 'amount' value is bigger than would fit in 64 bits if
       multiplied with 1000000, so we use the double math for this */
    p->current_వేగం =
      (curl_off_t)(((ద్విగుణ)amount * 1000000.0) / (ద్విగుణ)duration_us);
  }
  లేకపోతే {
    p->current_వేగం = amount * 1000000 / duration_us;
  }

  ఒకవేళ((p->delta.lastshow_us >= 0) && !దత్తాంశం->req.పూర్తి &&
     ((elapsed_us - p->delta.lastshow_us) < (1000 * 1000)))
    తిరిగి FALSE;
  p->delta.lastshow_us = elapsed_us;
  తిరిగి TRUE;
}

#నిర్వచితం_కాకపోతే CURL_DISABLE_పురోగతి_METER

నిర్మాణం pgrs_estimate {
  curl_off_t secs;
  curl_off_t percent;
};

స్థిర curl_off_t pgrs_est_percent(curl_off_t మొత్తం, curl_off_t cur)
{
  ఒకవేళ(మొత్తం > 10000)
    తిరిగి cur / (మొత్తం / 100);
  లేకపోతే ఒకవేళ(మొత్తం > 0)
    తిరిగి (cur * 100) / మొత్తం;
  తిరిగి 0;
}

స్థిర శూన్య pgrs_estimates(నిర్మాణం pgrs_dir *d,
                           bool మొత్తం_తెలిసిన,
                           నిర్మాణం pgrs_estimate *est)
{
  est->secs = 0;
  est->percent = 0;
  ఒకవేళ(మొత్తం_తెలిసిన && (d->వేగం > 0)) {
    est->secs = d->మొత్తం_కొలత / d->వేగం;
    est->percent = pgrs_est_percent(d->మొత్తం_కొలత, d->cur_కొలత);
  }
}

స్థిర శూన్య పురోగతి_meter(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం పురోగతి_2 *p = &దత్తాంశం->పురోగతి;
  అక్షరం max6[6][7];
  నిర్మాణం pgrs_estimate dl_estm;
  నిర్మాణం pgrs_estimate ul_estm;
  నిర్మాణం pgrs_estimate మొత్తం_estm;
  curl_off_t మొత్తం_cur_కొలత;
  curl_off_t మొత్తం_expected_కొలత;
  curl_off_t dl_కొలత;
  అక్షరం సమయం_ఎడమ[8];
  అక్షరం సమయం_మొత్తం[8];
  అక్షరం సమయం_spent[8];
  curl_off_t cur_secs = (curl_off_t)p->మొత్తం.spent_us / 1000000;

  ఒకవేళ(!p->శీర్షికలు_బహిర్గతం) {
    ఒకవేళ(దత్తాంశం->state.resume_from) {
      curl_mfprintf(దత్తాంశం->సమితి.err,
                    "** Resuming transfer from byte position %" FMT_OFF_T "\n",
                    దత్తాంశం->state.resume_from);
    }
    curl_mfprintf(దత్తాంశం->సమితి.err,
                  "  %% Total    %% Received %% Xferd  Average Speed  "
                  "Time    Time    Time   Current\n"
                  "                                 Dload  Upload  "
                  "Total   Spent   Left   Speed\n");
    p->శీర్షికలు_బహిర్గతం = TRUE; /* headers are shown */
  }

  /* Figure out the estimated time of arrival for upload and download */
  pgrs_estimates(&p->ul, (bool)p->ul_కొలత_తెలిసిన, &ul_estm);
  pgrs_estimates(&p->dl, (bool)p->dl_కొలత_తెలిసిన, &dl_estm);

  /* Since both happen at the same time, total expected duration is max. */
  మొత్తం_estm.secs = CURLMAX(ul_estm.secs, dl_estm.secs);
  /* create the three time strings */
  time2str(సమయం_ఎడమ, పరిమాణం(సమయం_ఎడమ),
           మొత్తం_estm.secs > 0 ? (మొత్తం_estm.secs - cur_secs) : 0);
  time2str(సమయం_మొత్తం, పరిమాణం(సమయం_మొత్తం), మొత్తం_estm.secs);
  time2str(సమయం_spent, పరిమాణం(సమయం_spent), cur_secs);

  /* Get the total amount of data expected to get transferred */
  మొత్తం_expected_కొలత = p->ul_కొలత_తెలిసిన ? p->ul.మొత్తం_కొలత : p->ul.cur_కొలత;

  dl_కొలత = p->dl_కొలత_తెలిసిన ? p->dl.మొత్తం_కొలత : p->dl.cur_కొలత;

  /* integer overflow check */
  ఒకవేళ((CURL_OFF_T_MAX - మొత్తం_expected_కొలత) < dl_కొలత)
    మొత్తం_expected_కొలత = CURL_OFF_T_MAX; /* capped */
  లేకపోతే
    మొత్తం_expected_కొలత += dl_కొలత;

  /* We have transferred this much so far */
  మొత్తం_cur_కొలత = p->dl.cur_కొలత + p->ul.cur_కొలత;

  /* Get the percentage of data transferred so far */
  మొత్తం_estm.percent = pgrs_est_percent(మొత్తం_expected_కొలత, మొత్తం_cur_కొలత);

  curl_mfprintf(దత్తాంశం->సమితి.err,
                "\r"
                "%3" FMT_OFF_T " %s "
                "%3" FMT_OFF_T " %s "
                "%3" FMT_OFF_T " %s %s %s %s %s %s %s",
                మొత్తం_estm.percent, /* 3 letters */    /* total % */
                max6out(మొత్తం_expected_కొలత, max6[2],
                        పరిమాణం(max6[2])),              /* total size */
                dl_estm.percent, /* 3 letters */       /* rcvd % */
                max6out(p->dl.cur_కొలత, max6[0],
                        పరిమాణం(max6[0])),              /* rcvd size */
                ul_estm.percent, /* 3 letters */       /* xfer % */
                max6out(p->ul.cur_కొలత, max6[1],
                        పరిమాణం(max6[1])),              /* xfer size */
                max6out(p->dl.వేగం, max6[3],
                        పరిమాణం(max6[3])),              /* avrg dl speed */
                max6out(p->ul.వేగం, max6[4],
                        పరిమాణం(max6[4])),              /* avrg ul speed */
                సమయం_మొత్తం,    /* 7 letters */         /* total time */
                సమయం_spent,    /* 7 letters */         /* time spent */
                సమయం_ఎడమ,     /* 7 letters */         /* time left */
                max6out(p->current_వేగం, max6[5],
                        పరిమాణం(max6[5]))               /* current speed */
    );

  /* we flush the output stream to make it appear as soon as possible */
  fflush(దత్తాంశం->సమితి.err);
}
#లేకపోతే /* CURL_DISABLE_PROGRESS_METER */
#నిర్వచించు పురోగతి_meter(x) Curl_nop_stmt
#అయితే_ముగింపు

/*
 * Curl_pgrsUpdate() returns 0 for success or the value returned by the
 * progress callback!
 */
స్థిర CURLcode pgrsupdate(నిర్మాణం Curl_easy *దత్తాంశం, bool showprogress)
{
  ఒకవేళ(!దత్తాంశం->పురోగతి.hide) {
    పూర్ణాంకం rc;
    ఒకవేళ(దత్తాంశం->సమితి.fxferinfo) {
      /* There is a callback set, call that */
      నిర్మాణం Curl_mapi_guard guard;
      CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_fxferinfo);
      rc = దత్తాంశం->సమితి.fxferinfo(దత్తాంశం->సమితి.పురోగతి_client,
                               దత్తాంశం->పురోగతి.dl.మొత్తం_కొలత,
                               దత్తాంశం->పురోగతి.dl.cur_కొలత,
                               దత్తాంశం->పురోగతి.ul.మొత్తం_కొలత,
                               దత్తాంశం->పురోగతి.ul.cur_కొలత);
      CURL_CBAPI_చివర(&guard);
      ఒకవేళ(rc != CURL_PROGRESSFUNC_CONTINUE) {
        ఒకవేళ(rc) {
          failf(దత్తాంశం, "Callback aborted");
          తిరిగి CURLE_ABORTED_BY_CALLBACK;
        }
        తిరిగి CURLE_OK;
      }
    }
    లేకపోతే ఒకవేళ(దత్తాంశం->సమితి.fprogress) {
      /* The older deprecated callback is set, call that */
      నిర్మాణం Curl_mapi_guard guard;
      CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_fprogress);
      rc = దత్తాంశం->సమితి.fprogress(దత్తాంశం->సమితి.పురోగతి_client,
                               (ద్విగుణ)దత్తాంశం->పురోగతి.dl.మొత్తం_కొలత,
                               (ద్విగుణ)దత్తాంశం->పురోగతి.dl.cur_కొలత,
                               (ద్విగుణ)దత్తాంశం->పురోగతి.ul.మొత్తం_కొలత,
                               (ద్విగుణ)దత్తాంశం->పురోగతి.ul.cur_కొలత);
      CURL_CBAPI_చివర(&guard);
      ఒకవేళ(rc != CURL_PROGRESSFUNC_CONTINUE) {
        ఒకవేళ(rc) {
          failf(దత్తాంశం, "Callback aborted");
          తిరిగి CURLE_ABORTED_BY_CALLBACK;
        }
        తిరిగి CURLE_OK;
      }
    }

    ఒకవేళ(showprogress)
      పురోగతి_meter(దత్తాంశం);
  }

  తిరిగి CURLE_OK;
}

స్థిర CURLcode pgrs_update(నిర్మాణం Curl_easy *దత్తాంశం,
                            స్థిరరాశి నిర్మాణం curltime *pnow)
{
  bool showprogress = పురోగతి_calc(దత్తాంశం, pnow);
  తిరిగి pgrsupdate(దత్తాంశం, showprogress);
}

CURLcode Curl_pgrsUpdate(నిర్మాణం Curl_easy *దత్తాంశం)
{
  తిరిగి pgrs_update(దత్తాంశం, Curl_pgrs_now(దత్తాంశం));
}

CURLcode Curl_pgrsUpdateX(నిర్మాణం Curl_easy *దత్తాంశం,
                          స్థిరరాశి నిర్మాణం curltime *pnow)
{
  తిరిగి pgrs_update(దత్తాంశం, pnow);
}

CURLcode Curl_pgrsCheckX(నిర్మాణం Curl_easy *దత్తాంశం,
                         స్థిరరాశి నిర్మాణం curltime *pnow)
{
  CURLcode ఫలితం;

  ఫలితం = pgrs_update(దత్తాంశం, pnow);
  ఒకవేళ(!ఫలితం && !దత్తాంశం->req.పూర్తి)
    ఫలితం = pgrs_speedcheck(దత్తాంశం, pnow);
  తిరిగి ఫలితం;
}

CURLcode Curl_pgrsCheck(నిర్మాణం Curl_easy *దత్తాంశం)
{
  తిరిగి Curl_pgrsCheckX(దత్తాంశం, Curl_pgrs_now(దత్తాంశం));
}

/*
 * Update all progress, do not do progress meter/callbacks.
 */
శూన్య Curl_pgrsUpdate_nometer(నిర్మాణం Curl_easy *దత్తాంశం)
{
  (శూన్య)పురోగతి_calc(దత్తాంశం, Curl_pgrs_now(దత్తాంశం));
}

శూన్య Curl_pgrsCompleted(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం పురోగతి_2 * స్థిరరాశి p = &దత్తాంశం->పురోగతి;
  p->మొత్తం.spent_us = curlx_ptimediff_us(Curl_pgrs_now(దత్తాంశం), &p->ప్రారంభం);
}

timediff_t Curl_pgrs_since_ms(నిర్మాణం Curl_easy *దత్తాంశం,
                              స్థిరరాశి నిర్మాణం curltime *pnow,
                              timerid timer)
{
  ఒకవేళ(!pnow)
    pnow = Curl_pgrs_now(దత్తాంశం);
  ఎంపిక(timer) {
  స్థితి TIMER_STARTOP:
    తిరిగి (curlx_ptimediff_us(pnow, &దత్తాంశం->పురోగతి.ప్రారంభం) -
            దత్తాంశం->పురోగతి.delta.startop_us) / 1000;
  స్థితి TIMER_STARTSINGLE:
    తిరిగి (curlx_ptimediff_us(pnow, &దత్తాంశం->పురోగతి.ప్రారంభం) -
            దత్తాంశం->పురోగతి.delta.startsingle_us) / 1000;
  అప్రమేయ:
    DEBUGASSERT(0);
    తిరిగి 0;
  }
}
