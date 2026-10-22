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
#ضمن "curl_setup.h"

#ضمن "urldata.h"
#ضمن "curl_trc.h"
#ضمن "multiif.h"
#ضمن "progress.h"
#ضمن "transfer.h"
#ضمن "curlx/strcopy.h"

#جے_بنا_تعریف CURL_DISABLE_ترقی_METER
/* Provide a string that is 7 letters long (plus the zero byte).

   @unittest 1636 */
UNITTEST فراغ time2str(حرف *r, size_t rsize, curl_off_t seconds);
UNITTEST فراغ time2str(حرف *r, size_t rsize, curl_off_t seconds)
{
  curl_off_t h;
  إذا(seconds <= 0) {
    curlx_strcopy(r, rsize, STRCONST("       "));
    أرجع;
  }
  h = seconds / 3600;
  إذا(h <= 99) {
    curl_off_t m = (seconds - (h * 3600)) / 60;
    إذا(h <= 9) {
      curl_off_t s = (seconds - (h * 3600)) - (m * 60);
      إذا(h)
        curl_msnprintf(r, rsize, "%" FMT_OFF_T ":%02" FMT_OFF_T ":"
                       "%02" FMT_OFF_T, h, m, s);
      وإلا
        curl_msnprintf(r, rsize, "  %02" FMT_OFF_T ":%02" FMT_OFF_T, m, s);
    }
    وإلا
      curl_msnprintf(r, rsize, "%" FMT_OFF_T "h %02" FMT_OFF_T "m", h, m);
  }
  وإلا {
    curl_off_t d = seconds / 86400;
    h = (seconds - (d * 86400)) / 3600;
    إذا(d <= 99)
      curl_msnprintf(r, rsize, "%2" FMT_OFF_T "d %02" FMT_OFF_T "h", d, h);
    وإلا إذا(d <= 999)
      curl_msnprintf(r, rsize, "%6" FMT_OFF_T "d", d);
    وإلا { /* more than 999 days */
      curl_off_t m = d / 30;
      إذا(m <= 999)
        curl_msnprintf(r, rsize, "%6" FMT_OFF_T "m", m);
      وإلا { /* more than 999 months */
        curl_off_t y = d / 365;
        إذا(y <= 99999)
          curl_msnprintf(r, rsize, "%6" FMT_OFF_T "y", y);
        وإلا
          curlx_strcopy(r, rsize, STRCONST(">99999y"));
      }
    }
  }
}

/* The point of this function would be to return a string of the input data,
   but never longer than 6 columns (+ one zero byte).
   Add suffix k, M, G when suitable...

   @unittest 1636 */
UNITTEST حرف *max6out(curl_off_t بائٹاں, حرف *max6, size_t mlen);
UNITTEST حرف *max6out(curl_off_t بائٹاں, حرف *max6, size_t mlen)
{
  /* a signed 64-bit value is 8192 petabytes maximum, shown as
     8.0E (exabytes)*/
  إذا(بائٹاں < 100000)
    curl_msnprintf(max6, mlen, "%6" CURL_FORMAT_CURL_OFF_T, بائٹاں);
  وإلا {
    ساكن ثابت حرف unit[] = { 'k', 'M', 'G', 'T', 'P', 'E', 0 };
    صحيح k = 0;
    curl_off_t nbytes;
    curl_off_t rest;
    افعل {
      nbytes = بائٹاں / 1024;
      إذا(nbytes < 1000)
        اكسر;
      بائٹاں = nbytes;
      k++;
      DEBUGASSERT(unit[k]);
    } طالما(unit[k]);
    rest = بائٹاں % 1024;
    إذا(nbytes <= 99)
      /* xx.yyU */
      curl_msnprintf(max6, mlen, "%2" CURL_FORMAT_CURL_OFF_T
                     ".%02" CURL_FORMAT_CURL_OFF_T "%c", nbytes,
                     rest * 100 / 1024, unit[k]);
    وإلا
      /* xxx.yU */
      curl_msnprintf(max6, mlen, "%3" CURL_FORMAT_CURL_OFF_T
                     ".%" CURL_FORMAT_CURL_OFF_T "%c", nbytes,
                     rest * 10 / 1024, unit[k]);
  }
  أرجع max6;
}
#جے_انت

ساكن فراغ pgrs_speedinit(بنية Curl_easy *انکڑے)
{
  memset(&انکڑے->state.keeps_رفتار, 0, حجم(بنية curltime));
}

/*
 * @unittest 1606
 */
UNITTEST CURLcode pgrs_speedcheck(بنية Curl_easy *انکڑے,
                                  ثابت بنية curltime *pnow);
UNITTEST CURLcode pgrs_speedcheck(بنية Curl_easy *انکڑے,
                                  ثابت بنية curltime *pnow)
{
  إذا(!انکڑے->سیٹ.کم_رفتار_ویلا || !انکڑے->سیٹ.کم_رفتار_حد_بندی ||
     Curl_xfer_recv_is_paused(انکڑے) || Curl_xfer_send_is_paused(انکڑے))
    /* A paused transfer is not qualified for speed checks */
    أرجع CURLE_OK;

  إذا(انکڑے->ترقی.current_رفتار >= 0) {
    إذا(انکڑے->ترقی.current_رفتار < انکڑے->سیٹ.کم_رفتار_حد_بندی) {
      إذا(!انکڑے->state.keeps_رفتار.tv_sec)
        /* under the limit at this moment */
        انکڑے->state.keeps_رفتار = *pnow;
      وإلا {
        /* how long has it been under the limit */
        timediff_t howlong =
          curlx_ptimediff_ms(pnow, &انکڑے->state.keeps_رفتار);

        إذا(howlong >= انکڑے->سیٹ.کم_رفتار_ویلا * 1000) {
          /* too long */
          failf(انکڑے, "Operation too slow. Less than %" FMT_OFF_T
                " bytes/sec transferred the last %u seconds",
                انکڑے->سیٹ.کم_رفتار_حد_بندی, انکڑے->سیٹ.کم_رفتار_ویلا);
          أرجع CURLE_OPERATION_TIMEDOUT;
        }
      }
    }
    وإلا
      /* faster right now */
      انکڑے->state.keeps_رفتار.tv_sec = 0;
  }

  /* since low speed limit is enabled, set the expire timer to make this
     connection's speed get checked again in a second */
  Curl_expire_سیٹ(انکڑے, EXPIRE_SPEEDCHECK, 1000, pnow);

  أرجع CURLE_OK;
}

ثابت بنية curltime *Curl_pgrs_now(بنية Curl_easy *انکڑے)
{
  curlx_pnow(&انکڑے->ترقی.now);
  أرجع &انکڑے->ترقی.now;
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

صحيح Curl_pgrs_پورا(بنية Curl_easy *انکڑے, ثابت بنية curltime *pnow)
{
  صحيح rc;
  انکڑے->ترقی.delta.lastshow_us = -1;
  rc = Curl_pgrsUpdateX(انکڑے, pnow); /* the final (forced) update */
  إذا(rc)
    أرجع rc;

  إذا(!انکڑے->ترقی.hide && !انکڑے->ترقی.callback)
    /* only output if we do not use a progress callback and we are not
     * hidden */
    curl_mfprintf(انکڑے->سیٹ.err, "\n");

  أرجع 0;
}

فراغ Curl_pgrsReset(بنية Curl_easy *انکڑے)
{
  Curl_pgrs_سیٹ_Upload_Counter(انکڑے, 0);
  انکڑے->ترقی.dl.cur_ناپ = 0;
  Curl_pgrs_سیٹ_Upload_ناپ(انکڑے, -1);
  Curl_pgrs_سیٹ_Download_ناپ(انکڑے, -1);
  انکڑے->ترقی.speeder_c = 0; /* reset speed records */
  انکڑے->ترقی.deliver = 0;
  pgrs_speedinit(انکڑے);
}

/* reset the known transfer sizes */
فراغ Curl_pgrsResetTransferSizes(بنية Curl_easy *انکڑے)
{
  Curl_pgrs_سیٹ_Download_ناپ(انکڑے, -1);
  Curl_pgrs_سیٹ_Upload_ناپ(انکڑے, -1);
}

فراغ Curl_pgrsRecvPause(بنية Curl_easy *انکڑے, bool enable)
{
  إذا(!enable) {
    انکڑے->ترقی.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(انکڑے); /* reset low speed measurements */
  }
}

فراغ Curl_pgrsSendPause(بنية Curl_easy *انکڑے, bool enable)
{
  إذا(!enable) {
    انکڑے->ترقی.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(انکڑے); /* reset low speed measurements */
  }
}

#جے_تعریف_شدہ CURLVERBOSE
ساكن ثابت حرف * ثابت pgrs_timer_ناں[] = {
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

ساكن ثابت حرف *pgrs_timer_name(timerid timer)
{
  إذا((size_t)timer < CURL_ARRAYSIZE(pgrs_timer_ناں))
    أرجع pgrs_timer_ناں[(size_t)timer];
  أرجع "?";
}
#جے_انت /* CURLVERBOSE */

/*
 * Curl_pgrsTimeWas(). Store the timestamp time at the given label.
 */
فراغ Curl_pgrs_ویلا_Was(بنية Curl_easy *انکڑے, timerid timer,
                      بنية curltime timestamp)
{
  timediff_t *delta = عدم;

  بدل(timer) {
  افتراضي:
  حالة TIMER_NONE:
    /* mistake filter */
    اكسر;
  حالة TIMER_STARTOP:
    /* This is set at the start of a transfer */
    انکڑے->ترقی.delta.startop_us =
      curlx_ptimediff_us(&timestamp, &انکڑے->ترقی.شروع);
    انکڑے->ترقی.delta.startqueue_us = انکڑے->ترقی.delta.startop_us;
    اكسر;
  حالة TIMER_STARTSINGLE:
    /* This is set at the start of each single transfer */
    انکڑے->ترقی.delta.startsingle_us =
      curlx_ptimediff_us(&timestamp, &انکڑے->ترقی.شروع);
    انکڑے->ترقی.startransfer_added = FALSE;
    اكسر;
  حالة TIMER_POSTQUEUE:
    /* Queue time is accumulative from all involved redirects */
    انکڑے->ترقی.کل.queued_us +=
      curlx_ptimediff_us(&timestamp, &انکڑے->ترقی.شروع) -
      انکڑے->ترقی.delta.startqueue_us;
    اكسر;
  حالة TIMER_NAMELOOKUP:
    delta = &انکڑے->ترقی.کل.nslookup_us;
    اكسر;
  حالة TIMER_CONNECT:
    delta = &انکڑے->ترقی.کل.connect_us;
    اكسر;
  حالة TIMER_APPCONNECT:
    delta = &انکڑے->ترقی.کل.appconnect_us;
    اكسر;
  حالة TIMER_PRETRANSFER:
    delta = &انکڑے->ترقی.کل.pretransfer_us;
    اكسر;
  حالة TIMER_STARTTRANSFER:
    /* prevent updating t_starttransfer unless:
     *   1. this is the first time we are setting t_starttransfer
     *   2. a redirect has occurred since the last time t_starttransfer was set
     * This prevents repeated invocations of the function from incorrectly
     * changing the t_starttransfer time.
     */
    إذا(انکڑے->ترقی.startransfer_added) {
      CURL_TRC_M(انکڑے, "[%s] ignored", pgrs_timer_name(timer));
      أرجع;
    }
    انکڑے->ترقی.startransfer_added = TRUE;
    delta = &انکڑے->ترقی.کل.starttransfer_us;
    اكسر;
  حالة TIMER_POSTRANSFER:
    delta = &انکڑے->ترقی.کل.posttransfer_us;
    اكسر;
  حالة TIMER_REDIRECT:
    انکڑے->ترقی.delta.startredirect_us =
      curlx_ptimediff_us(&timestamp, &انکڑے->ترقی.شروع);
    /* transfer starts queueing again */
    انکڑے->ترقی.delta.startqueue_us =
      انکڑے->ترقی.delta.startredirect_us;
    اكسر;
  }
  إذا(delta) {
    timediff_t us = curlx_ptimediff_us(&timestamp, &انکڑے->ترقی.شروع) -
                    انکڑے->ترقی.delta.startsingle_us;
    إذا(us < 1)
      us = 1; /* make sure at least one microsecond passed */
    *delta += us;
    CURL_TRC_M(انکڑے, "[%s] added %" FMT_TIMEDIFF_T "us",
               pgrs_timer_name(timer), us);
  }
  وإلا
    CURL_TRC_M(انکڑے, "[%s] set", pgrs_timer_name(timer));
}

/*
 * Curl_pgrsTime(). Store the current time at the given label. This fetches a
 * fresh "now" and returns it.
 *
 * @unittest: 1399
 */
فراغ Curl_pgrs_ویلا(بنية Curl_easy *انکڑے, timerid timer)
{
  Curl_pgrs_ویلا_Was(انکڑے, timer, *Curl_pgrs_now(انکڑے));
}

فراغ Curl_pgrs_شروع(بنية Curl_easy *انکڑے, ثابت بنية curltime *pnow)
{
  بنية ترقی_2 *p = &انکڑے->ترقی;

  إذا(!pnow)
    pnow = Curl_pgrs_now(انکڑے);
  p->speeder_c = 0; /* reset the progress meter display */
  p->شروع = *pnow;
  memset(&p->delta, 0, حجم(p->delta));
  memset(&p->کل, 0, حجم(p->کل));
  p->startransfer_added = FALSE;
  p->dl.cur_ناپ = 0;
  p->ul.cur_ناپ = 0;
  /* the sizes are unknown at start */
  p->dl_ناپ_معلوم = FALSE;
  p->ul_ناپ_معلوم = FALSE;
}

/* check that the 'delta' amount of bytes are okay to deliver to the
   application, or return error if not. */
CURLcode Curl_pgrs_deliver_check(بنية Curl_easy *انکڑے, size_t delta)
{
  إذا(انکڑے->سیٹ.max_filesize &&
     ((curl_off_t)delta > انکڑے->سیٹ.max_filesize - انکڑے->ترقی.deliver)) {
    failf(انکڑے, "Would have exceeded max file size");
    أرجع CURLE_FILESIZE_EXCEEDED;
  }
  أرجع CURLE_OK;
}

/* this counts how much data is delivered to the application, which
   in compressed cases may differ from downloaded amount */
فراغ Curl_pgrs_deliver_inc(بنية Curl_easy *انکڑے, size_t delta)
{
  انکڑے->ترقی.deliver += delta;
}

فراغ Curl_pgrs_download_inc(بنية Curl_easy *انکڑے, size_t delta)
{
  إذا(delta) {
    انکڑے->ترقی.dl.cur_ناپ += delta;
    Curl_rlimit_drain(&انکڑے->ترقی.dl.rlimit, delta, عدم);
  }
}

فراغ Curl_pgrs_upload_inc(بنية Curl_easy *انکڑے, size_t delta)
{
  إذا(delta) {
    انکڑے->ترقی.ul.cur_ناپ += delta;
    Curl_rlimit_drain(&انکڑے->ترقی.ul.rlimit, delta, عدم);
  }
}

/*
 * Set the number of uploaded bytes so far.
 */
فراغ Curl_pgrs_سیٹ_Upload_Counter(بنية Curl_easy *انکڑے, curl_off_t ناپ)
{
  انکڑے->ترقی.ul.cur_ناپ = ناپ;
}

فراغ Curl_pgrs_سیٹ_Download_ناپ(بنية Curl_easy *انکڑے, curl_off_t ناپ)
{
  إذا(ناپ >= 0) {
    انکڑے->ترقی.dl.کل_ناپ = ناپ;
    انکڑے->ترقی.dl_ناپ_معلوم = TRUE;
  }
  وإلا {
    انکڑے->ترقی.dl.کل_ناپ = 0;
    انکڑے->ترقی.dl_ناپ_معلوم = FALSE;
  }
}

فراغ Curl_pgrs_سیٹ_Upload_ناپ(بنية Curl_easy *انکڑے, curl_off_t ناپ)
{
  إذا(ناپ >= 0) {
    انکڑے->ترقی.ul.کل_ناپ = ناپ;
    انکڑے->ترقی.ul_ناپ_معلوم = TRUE;
  }
  وإلا {
    انکڑے->ترقی.ul.کل_ناپ = 0;
    انکڑے->ترقی.ul_ناپ_معلوم = FALSE;
  }
}

فراغ Curl_pgrs_Early_انکڑے(بنية Curl_easy *انکڑے, curl_off_t گھلیا)
{
  انکڑے->ترقی.earlydata_گھلیا = گھلیا;
}

/* returns the average speed in bytes / second */
ساكن curl_off_t trspeed(curl_off_t ناپ, /* number of bytes */
                          curl_off_t us)   /* microseconds */
{
  إذا(us < 1)
    أرجع ناپ * 1000000;
  وإلا إذا(ناپ < CURL_OFF_T_MAX / 1000000)
    أرجع (ناپ * 1000000) / us;
  وإلا إذا(us >= 1000000)
    أرجع ناپ / (us / 1000000);
  وإلا
    أرجع CURL_OFF_T_MAX;
}

/* returns TRUE if it is time to show the progress meter */
ساكن bool ترقی_calc(بنية Curl_easy *انکڑے,
                          ثابت بنية curltime *pnow)
{
  بنية ترقی_2 * ثابت p = &انکڑے->ترقی;
  صحيح i_اگلا, i_oldest, i_latest;
  timediff_t duration_us, elapsed_us;
  curl_off_t amount;

  /* The time spent so far (from the start) in microseconds */
  elapsed_us = curlx_ptimediff_us(pnow, &p->شروع);
  p->کل.spent_us = elapsed_us;
  p->dl.رفتار = trspeed(p->dl.cur_ناپ, p->کل.spent_us);
  p->ul.رفتار = trspeed(p->ul.cur_ناپ, p->کل.spent_us);

  إذا(!p->speeder_c) { /* no previous record exists */
    p->رفتار_amount[0] = p->dl.cur_ناپ + p->ul.cur_ناپ;
    p->رفتار_ویلا[0] = elapsed_us;
    p->speeder_c++;
    /* use the overall average at the start */
    p->current_رفتار = p->ul.رفتار + p->dl.رفتار;
    p->delta.lastshow_us = elapsed_us;
    أرجع TRUE;
  }
  /* We have at least one record now. Where to put the next and
   * where is the latest one? */
  i_اگلا = p->speeder_c % CURL_رفتار_ریکارڈاں;
  i_latest = (i_اگلا > 0) ? (i_اگلا - 1) : (CURL_رفتار_ریکارڈاں - 1);

  /* Make a new record only when some time has passed.
   * Too frequent calls otherwise ruin the history. */
  إذا((elapsed_us - p->رفتار_ویلا[i_latest]) >= (1000 * 1000)) {
    p->speeder_c++;
    i_latest = i_اگلا;
    p->رفتار_amount[i_latest] = p->dl.cur_ناپ + p->ul.cur_ناپ;
    p->رفتار_ویلا[i_latest] = elapsed_us;
  }
  وإلا إذا(انکڑے->req.پورا) {
    /* When a transfer is done, and we did not have a current speed
     * already, update the last record. Otherwise, stay at the speed
     * we have. The last chunk of data, when rate limiting, would increase
     * reported speed since it no longer measures a full second. */
    إذا(!p->current_رفتار) {
      p->رفتار_amount[i_latest] = p->dl.cur_ناپ + p->ul.cur_ناپ;
      p->رفتار_ویلا[i_latest] = elapsed_us;
    }
  }
  وإلا {
    /* transfer ongoing, wait for more time to pass. */
    أرجع FALSE;
  }

  i_oldest = (p->speeder_c < CURL_رفتار_ریکارڈاں) ? 0 :
             ((i_latest + 1) % CURL_رفتار_ریکارڈاں);

  /* How much we transferred between oldest and current records */
  amount = p->رفتار_amount[i_latest] - p->رفتار_amount[i_oldest];
  /* How long this took */
  duration_us = p->رفتار_ویلا[i_latest] - p->رفتار_ویلا[i_oldest];
  إذا(duration_us <= 0)
    duration_us = 1;

  إذا(amount > (CURL_OFF_T_MAX / 1000000)) {
    /* the 'amount' value is bigger than would fit in 64 bits if
       multiplied with 1000000, so we use the double math for this */
    p->current_رفتار =
      (curl_off_t)(((مزدوج)amount * 1000000.0) / (مزدوج)duration_us);
  }
  وإلا {
    p->current_رفتار = amount * 1000000 / duration_us;
  }

  إذا((p->delta.lastshow_us >= 0) && !انکڑے->req.پورا &&
     ((elapsed_us - p->delta.lastshow_us) < (1000 * 1000)))
    أرجع FALSE;
  p->delta.lastshow_us = elapsed_us;
  أرجع TRUE;
}

#جے_بنا_تعریف CURL_DISABLE_ترقی_METER

بنية pgrs_estimate {
  curl_off_t secs;
  curl_off_t percent;
};

ساكن curl_off_t pgrs_est_percent(curl_off_t کل, curl_off_t cur)
{
  إذا(کل > 10000)
    أرجع cur / (کل / 100);
  وإلا إذا(کل > 0)
    أرجع (cur * 100) / کل;
  أرجع 0;
}

ساكن فراغ pgrs_estimates(بنية pgrs_dir *d,
                           bool کل_معلوم,
                           بنية pgrs_estimate *est)
{
  est->secs = 0;
  est->percent = 0;
  إذا(کل_معلوم && (d->رفتار > 0)) {
    est->secs = d->کل_ناپ / d->رفتار;
    est->percent = pgrs_est_percent(d->کل_ناپ, d->cur_ناپ);
  }
}

ساكن فراغ ترقی_meter(بنية Curl_easy *انکڑے)
{
  بنية ترقی_2 *p = &انکڑے->ترقی;
  حرف max6[6][7];
  بنية pgrs_estimate dl_estm;
  بنية pgrs_estimate ul_estm;
  بنية pgrs_estimate کل_estm;
  curl_off_t کل_cur_ناپ;
  curl_off_t کل_expected_ناپ;
  curl_off_t dl_ناپ;
  حرف ویلا_بایاں[8];
  حرف ویلا_کل[8];
  حرف ویلا_spent[8];
  curl_off_t cur_secs = (curl_off_t)p->کل.spent_us / 1000000;

  إذا(!p->سرخیاں_باہرلا) {
    إذا(انکڑے->state.resume_from) {
      curl_mfprintf(انکڑے->سیٹ.err,
                    "** Resuming transfer from byte position %" FMT_OFF_T "\n",
                    انکڑے->state.resume_from);
    }
    curl_mfprintf(انکڑے->سیٹ.err,
                  "  %% Total    %% Received %% Xferd  Average Speed  "
                  "Time    Time    Time   Current\n"
                  "                                 Dload  Upload  "
                  "Total   Spent   Left   Speed\n");
    p->سرخیاں_باہرلا = TRUE; /* headers are shown */
  }

  /* Figure out the estimated time of arrival for upload and download */
  pgrs_estimates(&p->ul, (bool)p->ul_ناپ_معلوم, &ul_estm);
  pgrs_estimates(&p->dl, (bool)p->dl_ناپ_معلوم, &dl_estm);

  /* Since both happen at the same time, total expected duration is max. */
  کل_estm.secs = CURLMAX(ul_estm.secs, dl_estm.secs);
  /* create the three time strings */
  time2str(ویلا_بایاں, حجم(ویلا_بایاں),
           کل_estm.secs > 0 ? (کل_estm.secs - cur_secs) : 0);
  time2str(ویلا_کل, حجم(ویلا_کل), کل_estm.secs);
  time2str(ویلا_spent, حجم(ویلا_spent), cur_secs);

  /* Get the total amount of data expected to get transferred */
  کل_expected_ناپ = p->ul_ناپ_معلوم ? p->ul.کل_ناپ : p->ul.cur_ناپ;

  dl_ناپ = p->dl_ناپ_معلوم ? p->dl.کل_ناپ : p->dl.cur_ناپ;

  /* integer overflow check */
  إذا((CURL_OFF_T_MAX - کل_expected_ناپ) < dl_ناپ)
    کل_expected_ناپ = CURL_OFF_T_MAX; /* capped */
  وإلا
    کل_expected_ناپ += dl_ناپ;

  /* We have transferred this much so far */
  کل_cur_ناپ = p->dl.cur_ناپ + p->ul.cur_ناپ;

  /* Get the percentage of data transferred so far */
  کل_estm.percent = pgrs_est_percent(کل_expected_ناپ, کل_cur_ناپ);

  curl_mfprintf(انکڑے->سیٹ.err,
                "\r"
                "%3" FMT_OFF_T " %s "
                "%3" FMT_OFF_T " %s "
                "%3" FMT_OFF_T " %s %s %s %s %s %s %s",
                کل_estm.percent, /* 3 letters */    /* total % */
                max6out(کل_expected_ناپ, max6[2],
                        حجم(max6[2])),              /* total size */
                dl_estm.percent, /* 3 letters */       /* rcvd % */
                max6out(p->dl.cur_ناپ, max6[0],
                        حجم(max6[0])),              /* rcvd size */
                ul_estm.percent, /* 3 letters */       /* xfer % */
                max6out(p->ul.cur_ناپ, max6[1],
                        حجم(max6[1])),              /* xfer size */
                max6out(p->dl.رفتار, max6[3],
                        حجم(max6[3])),              /* avrg dl speed */
                max6out(p->ul.رفتار, max6[4],
                        حجم(max6[4])),              /* avrg ul speed */
                ویلا_کل,    /* 7 letters */         /* total time */
                ویلا_spent,    /* 7 letters */         /* time spent */
                ویلا_بایاں,     /* 7 letters */         /* time left */
                max6out(p->current_رفتار, max6[5],
                        حجم(max6[5]))               /* current speed */
    );

  /* we flush the output stream to make it appear as soon as possible */
  fflush(انکڑے->سیٹ.err);
}
#وإلا /* CURL_DISABLE_PROGRESS_METER */
#تعریف_کرو ترقی_meter(x) Curl_nop_stmt
#جے_انت

/*
 * Curl_pgrsUpdate() returns 0 for success or the value returned by the
 * progress callback!
 */
ساكن CURLcode pgrsupdate(بنية Curl_easy *انکڑے, bool showprogress)
{
  إذا(!انکڑے->ترقی.hide) {
    صحيح rc;
    إذا(انکڑے->سیٹ.fxferinfo) {
      /* There is a callback set, call that */
      بنية Curl_mapi_guard guard;
      CURL_CBAPI_شروع(&guard, انکڑے, easy_fxferinfo);
      rc = انکڑے->سیٹ.fxferinfo(انکڑے->سیٹ.ترقی_client,
                               انکڑے->ترقی.dl.کل_ناپ,
                               انکڑے->ترقی.dl.cur_ناپ,
                               انکڑے->ترقی.ul.کل_ناپ,
                               انکڑے->ترقی.ul.cur_ناپ);
      CURL_CBAPI_سرا(&guard);
      إذا(rc != CURL_PROGRESSFUNC_CONTINUE) {
        إذا(rc) {
          failf(انکڑے, "Callback aborted");
          أرجع CURLE_ABORTED_BY_CALLBACK;
        }
        أرجع CURLE_OK;
      }
    }
    وإلا إذا(انکڑے->سیٹ.fprogress) {
      /* The older deprecated callback is set, call that */
      بنية Curl_mapi_guard guard;
      CURL_CBAPI_شروع(&guard, انکڑے, easy_fprogress);
      rc = انکڑے->سیٹ.fprogress(انکڑے->سیٹ.ترقی_client,
                               (مزدوج)انکڑے->ترقی.dl.کل_ناپ,
                               (مزدوج)انکڑے->ترقی.dl.cur_ناپ,
                               (مزدوج)انکڑے->ترقی.ul.کل_ناپ,
                               (مزدوج)انکڑے->ترقی.ul.cur_ناپ);
      CURL_CBAPI_سرا(&guard);
      إذا(rc != CURL_PROGRESSFUNC_CONTINUE) {
        إذا(rc) {
          failf(انکڑے, "Callback aborted");
          أرجع CURLE_ABORTED_BY_CALLBACK;
        }
        أرجع CURLE_OK;
      }
    }

    إذا(showprogress)
      ترقی_meter(انکڑے);
  }

  أرجع CURLE_OK;
}

ساكن CURLcode pgrs_update(بنية Curl_easy *انکڑے,
                            ثابت بنية curltime *pnow)
{
  bool showprogress = ترقی_calc(انکڑے, pnow);
  أرجع pgrsupdate(انکڑے, showprogress);
}

CURLcode Curl_pgrsUpdate(بنية Curl_easy *انکڑے)
{
  أرجع pgrs_update(انکڑے, Curl_pgrs_now(انکڑے));
}

CURLcode Curl_pgrsUpdateX(بنية Curl_easy *انکڑے,
                          ثابت بنية curltime *pnow)
{
  أرجع pgrs_update(انکڑے, pnow);
}

CURLcode Curl_pgrsCheckX(بنية Curl_easy *انکڑے,
                         ثابت بنية curltime *pnow)
{
  CURLcode نتیجہ;

  نتیجہ = pgrs_update(انکڑے, pnow);
  إذا(!نتیجہ && !انکڑے->req.پورا)
    نتیجہ = pgrs_speedcheck(انکڑے, pnow);
  أرجع نتیجہ;
}

CURLcode Curl_pgrsCheck(بنية Curl_easy *انکڑے)
{
  أرجع Curl_pgrsCheckX(انکڑے, Curl_pgrs_now(انکڑے));
}

/*
 * Update all progress, do not do progress meter/callbacks.
 */
فراغ Curl_pgrsUpdate_nometer(بنية Curl_easy *انکڑے)
{
  (فراغ)ترقی_calc(انکڑے, Curl_pgrs_now(انکڑے));
}

فراغ Curl_pgrsCompleted(بنية Curl_easy *انکڑے)
{
  بنية ترقی_2 * ثابت p = &انکڑے->ترقی;
  p->کل.spent_us = curlx_ptimediff_us(Curl_pgrs_now(انکڑے), &p->شروع);
}

timediff_t Curl_pgrs_since_ms(بنية Curl_easy *انکڑے,
                              ثابت بنية curltime *pnow,
                              timerid timer)
{
  إذا(!pnow)
    pnow = Curl_pgrs_now(انکڑے);
  بدل(timer) {
  حالة TIMER_STARTOP:
    أرجع (curlx_ptimediff_us(pnow, &انکڑے->ترقی.شروع) -
            انکڑے->ترقی.delta.startop_us) / 1000;
  حالة TIMER_STARTSINGLE:
    أرجع (curlx_ptimediff_us(pnow, &انکڑے->ترقی.شروع) -
            انکڑے->ترقی.delta.startsingle_us) / 1000;
  افتراضي:
    DEBUGASSERT(0);
    أرجع 0;
  }
}
