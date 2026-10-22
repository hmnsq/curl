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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "curlx/strcopy.h"

#ifndef CURL_DISABLE_PROGRESS_METER
/* Provide a string that is 7 letters long (plus the zero byte).

   @unittest 1636 */
UNITTEST শূন্য time2str(অক্ষর *r, size_t rsize, curl_off_t seconds);
UNITTEST শূন্য time2str(অক্ষর *r, size_t rsize, curl_off_t seconds)
{
  curl_off_t h;
  যদি(seconds <= 0) {
    curlx_strcopy(r, rsize, STRCONST("       "));
    ফেরত;
  }
  h = seconds / 3600;
  যদি(h <= 99) {
    curl_off_t m = (seconds - (h * 3600)) / 60;
    যদি(h <= 9) {
      curl_off_t s = (seconds - (h * 3600)) - (m * 60);
      যদি(h)
        curl_msnprintf(r, rsize, "%" FMT_OFF_T ":%02" FMT_OFF_T ":"
                       "%02" FMT_OFF_T, h, m, s);
      অন্যথা
        curl_msnprintf(r, rsize, "  %02" FMT_OFF_T ":%02" FMT_OFF_T, m, s);
    }
    অন্যথা
      curl_msnprintf(r, rsize, "%" FMT_OFF_T "h %02" FMT_OFF_T "m", h, m);
  }
  অন্যথা {
    curl_off_t d = seconds / 86400;
    h = (seconds - (d * 86400)) / 3600;
    যদি(d <= 99)
      curl_msnprintf(r, rsize, "%2" FMT_OFF_T "d %02" FMT_OFF_T "h", d, h);
    অন্যথা যদি(d <= 999)
      curl_msnprintf(r, rsize, "%6" FMT_OFF_T "d", d);
    অন্যথা { /* more than 999 days */
      curl_off_t m = d / 30;
      যদি(m <= 999)
        curl_msnprintf(r, rsize, "%6" FMT_OFF_T "m", m);
      অন্যথা { /* more than 999 months */
        curl_off_t y = d / 365;
        যদি(y <= 99999)
          curl_msnprintf(r, rsize, "%6" FMT_OFF_T "y", y);
        অন্যথা
          curlx_strcopy(r, rsize, STRCONST(">99999y"));
      }
    }
  }
}

/* The point of this function would be to return a string of the input data,
   but never longer than 6 columns (+ one zero byte).
   Add suffix k, M, G when suitable...

   @unittest 1636 */
UNITTEST অক্ষর *max6out(curl_off_t bytes, অক্ষর *max6, size_t mlen);
UNITTEST অক্ষর *max6out(curl_off_t bytes, অক্ষর *max6, size_t mlen)
{
  /* a signed 64-bit value is 8192 petabytes maximum, shown as
     8.0E (exabytes)*/
  যদি(bytes < 100000)
    curl_msnprintf(max6, mlen, "%6" CURL_FORMAT_CURL_OFF_T, bytes);
  অন্যথা {
    স্থির ধ্রুবক অক্ষর unit[] = { 'k', 'M', 'G', 'T', 'P', 'E', 0 };
    পূর্ণসংখ্যা k = 0;
    curl_off_t nbytes;
    curl_off_t rest;
    করো {
      nbytes = bytes / 1024;
      যদি(nbytes < 1000)
        বিরতি;
      bytes = nbytes;
      k++;
      DEBUGASSERT(unit[k]);
    } যখন(unit[k]);
    rest = bytes % 1024;
    যদি(nbytes <= 99)
      /* xx.yyU */
      curl_msnprintf(max6, mlen, "%2" CURL_FORMAT_CURL_OFF_T
                     ".%02" CURL_FORMAT_CURL_OFF_T "%c", nbytes,
                     rest * 100 / 1024, unit[k]);
    অন্যথা
      /* xxx.yU */
      curl_msnprintf(max6, mlen, "%3" CURL_FORMAT_CURL_OFF_T
                     ".%" CURL_FORMAT_CURL_OFF_T "%c", nbytes,
                     rest * 10 / 1024, unit[k]);
  }
  ফেরত max6;
}
#endif

স্থির শূন্য pgrs_speedinit(কাঠামো Curl_easy *তথ্য)
{
  memset(&তথ্য->state.keeps_speed, 0, আকার(কাঠামো curltime));
}

/*
 * @unittest 1606
 */
UNITTEST CURLcode pgrs_speedcheck(কাঠামো Curl_easy *তথ্য,
                                  ধ্রুবক কাঠামো curltime *pnow);
UNITTEST CURLcode pgrs_speedcheck(কাঠামো Curl_easy *তথ্য,
                                  ধ্রুবক কাঠামো curltime *pnow)
{
  যদি(!তথ্য->set.নিচের_speed_time || !তথ্য->set.নিচের_speed_limit ||
     Curl_xfer_recv_is_paused(তথ্য) || Curl_xfer_send_is_paused(তথ্য))
    /* A paused transfer is not qualified for speed checks */
    ফেরত CURLE_OK;

  যদি(তথ্য->progress.current_speed >= 0) {
    যদি(তথ্য->progress.current_speed < তথ্য->set.নিচের_speed_limit) {
      যদি(!তথ্য->state.keeps_speed.tv_sec)
        /* under the limit at this moment */
        তথ্য->state.keeps_speed = *pnow;
      অন্যথা {
        /* how long has it been under the limit */
        timediff_t howlong =
          curlx_ptimediff_ms(pnow, &তথ্য->state.keeps_speed);

        যদি(howlong >= তথ্য->set.নিচের_speed_time * 1000) {
          /* too long */
          failf(তথ্য, "Operation too slow. Less than %" FMT_OFF_T
                " bytes/sec transferred the last %u seconds",
                তথ্য->set.নিচের_speed_limit, তথ্য->set.নিচের_speed_time);
          ফেরত CURLE_OPERATION_TIMEDOUT;
        }
      }
    }
    অন্যথা
      /* faster right now */
      তথ্য->state.keeps_speed.tv_sec = 0;
  }

  /* since low speed limit is enabled, set the expire timer to make this
     connection's speed get checked again in a second */
  Curl_expire_set(তথ্য, EXPIRE_SPEEDCHECK, 1000, pnow);

  ফেরত CURLE_OK;
}

ধ্রুবক কাঠামো curltime *Curl_pgrs_now(কাঠামো Curl_easy *তথ্য)
{
  curlx_pnow(&তথ্য->progress.now);
  ফেরত &তথ্য->progress.now;
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

পূর্ণসংখ্যা Curl_pgrsDone(কাঠামো Curl_easy *তথ্য, ধ্রুবক কাঠামো curltime *pnow)
{
  পূর্ণসংখ্যা rc;
  তথ্য->progress.delta.lastshow_us = -1;
  rc = Curl_pgrsUpdateX(তথ্য, pnow); /* the final (forced) update */
  যদি(rc)
    ফেরত rc;

  যদি(!তথ্য->progress.hide && !তথ্য->progress.callback)
    /* only output if we do not use a progress callback and we are not
     * hidden */
    curl_mfprintf(তথ্য->set.err, "\n");

  ফেরত 0;
}

শূন্য Curl_pgrsReset(কাঠামো Curl_easy *তথ্য)
{
  Curl_pgrs_Set_Upload_গণক(তথ্য, 0);
  তথ্য->progress.dl.cur_আকার = 0;
  Curl_pgrs_Set_Upload_আকার(তথ্য, -1);
  Curl_pgrs_Set_Download_আকার(তথ্য, -1);
  তথ্য->progress.speeder_c = 0; /* reset speed records */
  তথ্য->progress.deliver = 0;
  pgrs_speedinit(তথ্য);
}

/* reset the known transfer sizes */
শূন্য Curl_pgrsResetTransferSizes(কাঠামো Curl_easy *তথ্য)
{
  Curl_pgrs_Set_Download_আকার(তথ্য, -1);
  Curl_pgrs_Set_Upload_আকার(তথ্য, -1);
}

শূন্য Curl_pgrsRecvPause(কাঠামো Curl_easy *তথ্য, bool enable)
{
  যদি(!enable) {
    তথ্য->progress.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(তথ্য); /* reset low speed measurements */
  }
}

শূন্য Curl_pgrsSendPause(কাঠামো Curl_easy *তথ্য, bool enable)
{
  যদি(!enable) {
    তথ্য->progress.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(তথ্য); /* reset low speed measurements */
  }
}

#ifdef CURLVERBOSE
স্থির ধ্রুবক অক্ষর * ধ্রুবক pgrs_timer_names[] = {
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

স্থির ধ্রুবক অক্ষর *pgrs_timer_name(timerid timer)
{
  যদি((size_t)timer < CURL_ARRAYSIZE(pgrs_timer_names))
    ফেরত pgrs_timer_names[(size_t)timer];
  ফেরত "?";
}
#endif /* CURLVERBOSE */

/*
 * Curl_pgrsTimeWas(). Store the timestamp time at the given label.
 */
শূন্য Curl_pgrsTimeWas(কাঠামো Curl_easy *তথ্য, timerid timer,
                      কাঠামো curltime timestamp)
{
  timediff_t *delta = ফাঁকা;

  নির্বাচন(timer) {
  পূর্বনির্ধারিত:
  ক্ষেত্র TIMER_NONE:
    /* mistake filter */
    বিরতি;
  ক্ষেত্র TIMER_STARTOP:
    /* This is set at the start of a transfer */
    তথ্য->progress.delta.startop_us =
      curlx_ptimediff_us(&timestamp, &তথ্য->progress.start);
    তথ্য->progress.delta.startqueue_us = তথ্য->progress.delta.startop_us;
    বিরতি;
  ক্ষেত্র TIMER_STARTSINGLE:
    /* This is set at the start of each single transfer */
    তথ্য->progress.delta.startsingle_us =
      curlx_ptimediff_us(&timestamp, &তথ্য->progress.start);
    তথ্য->progress.startransfer_added = FALSE;
    বিরতি;
  ক্ষেত্র TIMER_POSTQUEUE:
    /* Queue time is accumulative from all involved redirects */
    তথ্য->progress.যোগ.queued_us +=
      curlx_ptimediff_us(&timestamp, &তথ্য->progress.start) -
      তথ্য->progress.delta.startqueue_us;
    বিরতি;
  ক্ষেত্র TIMER_NAMELOOKUP:
    delta = &তথ্য->progress.যোগ.nslookup_us;
    বিরতি;
  ক্ষেত্র TIMER_CONNECT:
    delta = &তথ্য->progress.যোগ.connect_us;
    বিরতি;
  ক্ষেত্র TIMER_APPCONNECT:
    delta = &তথ্য->progress.যোগ.appconnect_us;
    বিরতি;
  ক্ষেত্র TIMER_PRETRANSFER:
    delta = &তথ্য->progress.যোগ.pretransfer_us;
    বিরতি;
  ক্ষেত্র TIMER_STARTTRANSFER:
    /* prevent updating t_starttransfer unless:
     *   1. this is the first time we are setting t_starttransfer
     *   2. a redirect has occurred since the last time t_starttransfer was set
     * This prevents repeated invocations of the function from incorrectly
     * changing the t_starttransfer time.
     */
    যদি(তথ্য->progress.startransfer_added) {
      CURL_TRC_M(তথ্য, "[%s] ignored", pgrs_timer_name(timer));
      ফেরত;
    }
    তথ্য->progress.startransfer_added = TRUE;
    delta = &তথ্য->progress.যোগ.starttransfer_us;
    বিরতি;
  ক্ষেত্র TIMER_POSTRANSFER:
    delta = &তথ্য->progress.যোগ.posttransfer_us;
    বিরতি;
  ক্ষেত্র TIMER_REDIRECT:
    তথ্য->progress.delta.startredirect_us =
      curlx_ptimediff_us(&timestamp, &তথ্য->progress.start);
    /* transfer starts queueing again */
    তথ্য->progress.delta.startqueue_us =
      তথ্য->progress.delta.startredirect_us;
    বিরতি;
  }
  যদি(delta) {
    timediff_t us = curlx_ptimediff_us(&timestamp, &তথ্য->progress.start) -
                    তথ্য->progress.delta.startsingle_us;
    যদি(us < 1)
      us = 1; /* make sure at least one microsecond passed */
    *delta += us;
    CURL_TRC_M(তথ্য, "[%s] added %" FMT_TIMEDIFF_T "us",
               pgrs_timer_name(timer), us);
  }
  অন্যথা
    CURL_TRC_M(তথ্য, "[%s] set", pgrs_timer_name(timer));
}

/*
 * Curl_pgrsTime(). Store the current time at the given label. This fetches a
 * fresh "now" and returns it.
 *
 * @unittest: 1399
 */
শূন্য Curl_pgrsTime(কাঠামো Curl_easy *তথ্য, timerid timer)
{
  Curl_pgrsTimeWas(তথ্য, timer, *Curl_pgrs_now(তথ্য));
}

শূন্য Curl_pgrsStart(কাঠামো Curl_easy *তথ্য, ধ্রুবক কাঠামো curltime *pnow)
{
  কাঠামো Progress *p = &তথ্য->progress;

  যদি(!pnow)
    pnow = Curl_pgrs_now(তথ্য);
  p->speeder_c = 0; /* reset the progress meter display */
  p->start = *pnow;
  memset(&p->delta, 0, আকার(p->delta));
  memset(&p->যোগ, 0, আকার(p->যোগ));
  p->startransfer_added = FALSE;
  p->dl.cur_আকার = 0;
  p->ul.cur_আকার = 0;
  /* the sizes are unknown at start */
  p->dl_আকার_known = FALSE;
  p->ul_আকার_known = FALSE;
}

/* check that the 'delta' amount of bytes are okay to deliver to the
   application, or return error if not. */
CURLcode Curl_pgrs_deliver_check(কাঠামো Curl_easy *তথ্য, size_t delta)
{
  যদি(তথ্য->set.max_filesize &&
     ((curl_off_t)delta > তথ্য->set.max_filesize - তথ্য->progress.deliver)) {
    failf(তথ্য, "Would have exceeded max file size");
    ফেরত CURLE_FILESIZE_EXCEEDED;
  }
  ফেরত CURLE_OK;
}

/* this counts how much data is delivered to the application, which
   in compressed cases may differ from downloaded amount */
শূন্য Curl_pgrs_deliver_inc(কাঠামো Curl_easy *তথ্য, size_t delta)
{
  তথ্য->progress.deliver += delta;
}

শূন্য Curl_pgrs_download_inc(কাঠামো Curl_easy *তথ্য, size_t delta)
{
  যদি(delta) {
    তথ্য->progress.dl.cur_আকার += delta;
    Curl_rlimit_drain(&তথ্য->progress.dl.rlimit, delta, ফাঁকা);
  }
}

শূন্য Curl_pgrs_upload_inc(কাঠামো Curl_easy *তথ্য, size_t delta)
{
  যদি(delta) {
    তথ্য->progress.ul.cur_আকার += delta;
    Curl_rlimit_drain(&তথ্য->progress.ul.rlimit, delta, ফাঁকা);
  }
}

/*
 * Set the number of uploaded bytes so far.
 */
শূন্য Curl_pgrs_Set_Upload_গণক(কাঠামো Curl_easy *তথ্য, curl_off_t আকার_2)
{
  তথ্য->progress.ul.cur_আকার = আকার_2;
}

শূন্য Curl_pgrs_Set_Download_আকার(কাঠামো Curl_easy *তথ্য, curl_off_t আকার_2)
{
  যদি(আকার_2 >= 0) {
    তথ্য->progress.dl.যোগ_আকার = আকার_2;
    তথ্য->progress.dl_আকার_known = TRUE;
  }
  অন্যথা {
    তথ্য->progress.dl.যোগ_আকার = 0;
    তথ্য->progress.dl_আকার_known = FALSE;
  }
}

শূন্য Curl_pgrs_Set_Upload_আকার(কাঠামো Curl_easy *তথ্য, curl_off_t আকার_2)
{
  যদি(আকার_2 >= 0) {
    তথ্য->progress.ul.যোগ_আকার = আকার_2;
    তথ্য->progress.ul_আকার_known = TRUE;
  }
  অন্যথা {
    তথ্য->progress.ul.যোগ_আকার = 0;
    তথ্য->progress.ul_আকার_known = FALSE;
  }
}

শূন্য Curl_pgrs_Early_তথ্য(কাঠামো Curl_easy *তথ্য, curl_off_t sent)
{
  তথ্য->progress.earlydata_sent = sent;
}

/* returns the average speed in bytes / second */
স্থির curl_off_t trspeed(curl_off_t আকার_2, /* number of bytes */
                          curl_off_t us)   /* microseconds */
{
  যদি(us < 1)
    ফেরত আকার_2 * 1000000;
  অন্যথা যদি(আকার_2 < CURL_OFF_T_MAX / 1000000)
    ফেরত (আকার_2 * 1000000) / us;
  অন্যথা যদি(us >= 1000000)
    ফেরত আকার_2 / (us / 1000000);
  অন্যথা
    ফেরত CURL_OFF_T_MAX;
}

/* returns TRUE if it is time to show the progress meter */
স্থির bool progress_calc(কাঠামো Curl_easy *তথ্য,
                          ধ্রুবক কাঠামো curltime *pnow)
{
  কাঠামো Progress * ধ্রুবক p = &তথ্য->progress;
  পূর্ণসংখ্যা i_পরবর্তী, i_oldest, i_latest;
  timediff_t duration_us, elapsed_us;
  curl_off_t amount;

  /* The time spent so far (from the start) in microseconds */
  elapsed_us = curlx_ptimediff_us(pnow, &p->start);
  p->যোগ.spent_us = elapsed_us;
  p->dl.speed = trspeed(p->dl.cur_আকার, p->যোগ.spent_us);
  p->ul.speed = trspeed(p->ul.cur_আকার, p->যোগ.spent_us);

  যদি(!p->speeder_c) { /* no previous record exists */
    p->speed_amount[0] = p->dl.cur_আকার + p->ul.cur_আকার;
    p->speed_time[0] = elapsed_us;
    p->speeder_c++;
    /* use the overall average at the start */
    p->current_speed = p->ul.speed + p->dl.speed;
    p->delta.lastshow_us = elapsed_us;
    ফেরত TRUE;
  }
  /* We have at least one record now. Where to put the next and
   * where is the latest one? */
  i_পরবর্তী = p->speeder_c % CURL_SPEED_RECORDS;
  i_latest = (i_পরবর্তী > 0) ? (i_পরবর্তী - 1) : (CURL_SPEED_RECORDS - 1);

  /* Make a new record only when some time has passed.
   * Too frequent calls otherwise ruin the history. */
  যদি((elapsed_us - p->speed_time[i_latest]) >= (1000 * 1000)) {
    p->speeder_c++;
    i_latest = i_পরবর্তী;
    p->speed_amount[i_latest] = p->dl.cur_আকার + p->ul.cur_আকার;
    p->speed_time[i_latest] = elapsed_us;
  }
  অন্যথা যদি(তথ্য->req.done) {
    /* When a transfer is done, and we did not have a current speed
     * already, update the last record. Otherwise, stay at the speed
     * we have. The last chunk of data, when rate limiting, would increase
     * reported speed since it no longer measures a full second. */
    যদি(!p->current_speed) {
      p->speed_amount[i_latest] = p->dl.cur_আকার + p->ul.cur_আকার;
      p->speed_time[i_latest] = elapsed_us;
    }
  }
  অন্যথা {
    /* transfer ongoing, wait for more time to pass. */
    ফেরত FALSE;
  }

  i_oldest = (p->speeder_c < CURL_SPEED_RECORDS) ? 0 :
             ((i_latest + 1) % CURL_SPEED_RECORDS);

  /* How much we transferred between oldest and current records */
  amount = p->speed_amount[i_latest] - p->speed_amount[i_oldest];
  /* How long this took */
  duration_us = p->speed_time[i_latest] - p->speed_time[i_oldest];
  যদি(duration_us <= 0)
    duration_us = 1;

  যদি(amount > (CURL_OFF_T_MAX / 1000000)) {
    /* the 'amount' value is bigger than would fit in 64 bits if
       multiplied with 1000000, so we use the double math for this */
    p->current_speed =
      (curl_off_t)(((দ্বিগুণ)amount * 1000000.0) / (দ্বিগুণ)duration_us);
  }
  অন্যথা {
    p->current_speed = amount * 1000000 / duration_us;
  }

  যদি((p->delta.lastshow_us >= 0) && !তথ্য->req.done &&
     ((elapsed_us - p->delta.lastshow_us) < (1000 * 1000)))
    ফেরত FALSE;
  p->delta.lastshow_us = elapsed_us;
  ফেরত TRUE;
}

#ifndef CURL_DISABLE_PROGRESS_METER

কাঠামো pgrs_estimate {
  curl_off_t secs;
  curl_off_t percent;
};

স্থির curl_off_t pgrs_est_percent(curl_off_t যোগ, curl_off_t cur)
{
  যদি(যোগ > 10000)
    ফেরত cur / (যোগ / 100);
  অন্যথা যদি(যোগ > 0)
    ফেরত (cur * 100) / যোগ;
  ফেরত 0;
}

স্থির শূন্য pgrs_estimates(কাঠামো pgrs_dir *d,
                           bool যোগ_known,
                           কাঠামো pgrs_estimate *est)
{
  est->secs = 0;
  est->percent = 0;
  যদি(যোগ_known && (d->speed > 0)) {
    est->secs = d->যোগ_আকার / d->speed;
    est->percent = pgrs_est_percent(d->যোগ_আকার, d->cur_আকার);
  }
}

স্থির শূন্য progress_meter(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Progress *p = &তথ্য->progress;
  অক্ষর max6[6][7];
  কাঠামো pgrs_estimate dl_estm;
  কাঠামো pgrs_estimate ul_estm;
  কাঠামো pgrs_estimate যোগ_estm;
  curl_off_t যোগ_cur_আকার;
  curl_off_t যোগ_expected_আকার;
  curl_off_t dl_আকার;
  অক্ষর time_বাম[8];
  অক্ষর time_যোগ[8];
  অক্ষর time_spent[8];
  curl_off_t cur_secs = (curl_off_t)p->যোগ.spent_us / 1000000;

  যদি(!p->headers_out) {
    যদি(তথ্য->state.resume_from) {
      curl_mfprintf(তথ্য->set.err,
                    "** Resuming transfer from byte position %" FMT_OFF_T "\n",
                    তথ্য->state.resume_from);
    }
    curl_mfprintf(তথ্য->set.err,
                  "  %% Total    %% Received %% Xferd  Average Speed  "
                  "Time    Time    Time   Current\n"
                  "                                 Dload  Upload  "
                  "Total   Spent   Left   Speed\n");
    p->headers_out = TRUE; /* headers are shown */
  }

  /* Figure out the estimated time of arrival for upload and download */
  pgrs_estimates(&p->ul, (bool)p->ul_আকার_known, &ul_estm);
  pgrs_estimates(&p->dl, (bool)p->dl_আকার_known, &dl_estm);

  /* Since both happen at the same time, total expected duration is max. */
  যোগ_estm.secs = CURLMAX(ul_estm.secs, dl_estm.secs);
  /* create the three time strings */
  time2str(time_বাম, আকার(time_বাম),
           যোগ_estm.secs > 0 ? (যোগ_estm.secs - cur_secs) : 0);
  time2str(time_যোগ, আকার(time_যোগ), যোগ_estm.secs);
  time2str(time_spent, আকার(time_spent), cur_secs);

  /* Get the total amount of data expected to get transferred */
  যোগ_expected_আকার = p->ul_আকার_known ? p->ul.যোগ_আকার : p->ul.cur_আকার;

  dl_আকার = p->dl_আকার_known ? p->dl.যোগ_আকার : p->dl.cur_আকার;

  /* integer overflow check */
  যদি((CURL_OFF_T_MAX - যোগ_expected_আকার) < dl_আকার)
    যোগ_expected_আকার = CURL_OFF_T_MAX; /* capped */
  অন্যথা
    যোগ_expected_আকার += dl_আকার;

  /* We have transferred this much so far */
  যোগ_cur_আকার = p->dl.cur_আকার + p->ul.cur_আকার;

  /* Get the percentage of data transferred so far */
  যোগ_estm.percent = pgrs_est_percent(যোগ_expected_আকার, যোগ_cur_আকার);

  curl_mfprintf(তথ্য->set.err,
                "\r"
                "%3" FMT_OFF_T " %s "
                "%3" FMT_OFF_T " %s "
                "%3" FMT_OFF_T " %s %s %s %s %s %s %s",
                যোগ_estm.percent, /* 3 letters */    /* total % */
                max6out(যোগ_expected_আকার, max6[2],
                        আকার(max6[2])),              /* total size */
                dl_estm.percent, /* 3 letters */       /* rcvd % */
                max6out(p->dl.cur_আকার, max6[0],
                        আকার(max6[0])),              /* rcvd size */
                ul_estm.percent, /* 3 letters */       /* xfer % */
                max6out(p->ul.cur_আকার, max6[1],
                        আকার(max6[1])),              /* xfer size */
                max6out(p->dl.speed, max6[3],
                        আকার(max6[3])),              /* avrg dl speed */
                max6out(p->ul.speed, max6[4],
                        আকার(max6[4])),              /* avrg ul speed */
                time_যোগ,    /* 7 letters */         /* total time */
                time_spent,    /* 7 letters */         /* time spent */
                time_বাম,     /* 7 letters */         /* time left */
                max6out(p->current_speed, max6[5],
                        আকার(max6[5]))               /* current speed */
    );

  /* we flush the output stream to make it appear as soon as possible */
  fflush(তথ্য->set.err);
}
#অন্যথা /* CURL_DISABLE_PROGRESS_METER */
#define progress_meter(x) Curl_nop_stmt
#endif

/*
 * Curl_pgrsUpdate() returns 0 for success or the value returned by the
 * progress callback!
 */
স্থির CURLcode pgrsupdate(কাঠামো Curl_easy *তথ্য, bool showprogress)
{
  যদি(!তথ্য->progress.hide) {
    পূর্ণসংখ্যা rc;
    যদি(তথ্য->set.fxferinfo) {
      /* There is a callback set, call that */
      কাঠামো Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, তথ্য, easy_fxferinfo);
      rc = তথ্য->set.fxferinfo(তথ্য->set.progress_client,
                               তথ্য->progress.dl.যোগ_আকার,
                               তথ্য->progress.dl.cur_আকার,
                               তথ্য->progress.ul.যোগ_আকার,
                               তথ্য->progress.ul.cur_আকার);
      CURL_CBAPI_END(&guard);
      যদি(rc != CURL_PROGRESSFUNC_CONTINUE) {
        যদি(rc) {
          failf(তথ্য, "Callback aborted");
          ফেরত CURLE_ABORTED_BY_CALLBACK;
        }
        ফেরত CURLE_OK;
      }
    }
    অন্যথা যদি(তথ্য->set.fprogress) {
      /* The older deprecated callback is set, call that */
      কাঠামো Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, তথ্য, easy_fprogress);
      rc = তথ্য->set.fprogress(তথ্য->set.progress_client,
                               (দ্বিগুণ)তথ্য->progress.dl.যোগ_আকার,
                               (দ্বিগুণ)তথ্য->progress.dl.cur_আকার,
                               (দ্বিগুণ)তথ্য->progress.ul.যোগ_আকার,
                               (দ্বিগুণ)তথ্য->progress.ul.cur_আকার);
      CURL_CBAPI_END(&guard);
      যদি(rc != CURL_PROGRESSFUNC_CONTINUE) {
        যদি(rc) {
          failf(তথ্য, "Callback aborted");
          ফেরত CURLE_ABORTED_BY_CALLBACK;
        }
        ফেরত CURLE_OK;
      }
    }

    যদি(showprogress)
      progress_meter(তথ্য);
  }

  ফেরত CURLE_OK;
}

স্থির CURLcode pgrs_update(কাঠামো Curl_easy *তথ্য,
                            ধ্রুবক কাঠামো curltime *pnow)
{
  bool showprogress = progress_calc(তথ্য, pnow);
  ফেরত pgrsupdate(তথ্য, showprogress);
}

CURLcode Curl_pgrsUpdate(কাঠামো Curl_easy *তথ্য)
{
  ফেরত pgrs_update(তথ্য, Curl_pgrs_now(তথ্য));
}

CURLcode Curl_pgrsUpdateX(কাঠামো Curl_easy *তথ্য,
                          ধ্রুবক কাঠামো curltime *pnow)
{
  ফেরত pgrs_update(তথ্য, pnow);
}

CURLcode Curl_pgrsCheckX(কাঠামো Curl_easy *তথ্য,
                         ধ্রুবক কাঠামো curltime *pnow)
{
  CURLcode ফলাফল;

  ফলাফল = pgrs_update(তথ্য, pnow);
  যদি(!ফলাফল && !তথ্য->req.done)
    ফলাফল = pgrs_speedcheck(তথ্য, pnow);
  ফেরত ফলাফল;
}

CURLcode Curl_pgrsCheck(কাঠামো Curl_easy *তথ্য)
{
  ফেরত Curl_pgrsCheckX(তথ্য, Curl_pgrs_now(তথ্য));
}

/*
 * Update all progress, do not do progress meter/callbacks.
 */
শূন্য Curl_pgrsUpdate_nometer(কাঠামো Curl_easy *তথ্য)
{
  (শূন্য)progress_calc(তথ্য, Curl_pgrs_now(তথ্য));
}

শূন্য Curl_pgrsCompleted(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Progress * ধ্রুবক p = &তথ্য->progress;
  p->যোগ.spent_us = curlx_ptimediff_us(Curl_pgrs_now(তথ্য), &p->start);
}

timediff_t Curl_pgrs_since_ms(কাঠামো Curl_easy *তথ্য,
                              ধ্রুবক কাঠামো curltime *pnow,
                              timerid timer)
{
  যদি(!pnow)
    pnow = Curl_pgrs_now(তথ্য);
  নির্বাচন(timer) {
  ক্ষেত্র TIMER_STARTOP:
    ফেরত (curlx_ptimediff_us(pnow, &তথ্য->progress.start) -
            তথ্য->progress.delta.startop_us) / 1000;
  ক্ষেত্র TIMER_STARTSINGLE:
    ফেরত (curlx_ptimediff_us(pnow, &তথ্য->progress.start) -
            তথ্য->progress.delta.startsingle_us) / 1000;
  পূর্বনির্ধারিত:
    DEBUGASSERT(0);
    ফেরত 0;
  }
}
