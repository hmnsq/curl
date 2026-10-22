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

#که_نه_تعریف_شوی CURL_DISABLE_پرمختګ_METER
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
UNITTEST حرف *max6out(curl_off_t بایټونه, حرف *max6, size_t mlen);
UNITTEST حرف *max6out(curl_off_t بایټونه, حرف *max6, size_t mlen)
{
  /* a signed 64-bit value is 8192 petabytes maximum, shown as
     8.0E (exabytes)*/
  إذا(بایټونه < 100000)
    curl_msnprintf(max6, mlen, "%6" CURL_FORMAT_CURL_OFF_T, بایټونه);
  وإلا {
    ساكن ثابت حرف unit[] = { 'k', 'M', 'G', 'T', 'P', 'E', 0 };
    صحيح k = 0;
    curl_off_t nbytes;
    curl_off_t rest;
    افعل {
      nbytes = بایټونه / 1024;
      إذا(nbytes < 1000)
        اكسر;
      بایټونه = nbytes;
      k++;
      DEBUGASSERT(unit[k]);
    } طالما(unit[k]);
    rest = بایټونه % 1024;
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
#د_که_پای

ساكن فراغ pgrs_speedinit(بنية Curl_easy *معلومات)
{
  memset(&معلومات->state.keeps_چټکتیا, 0, حجم(بنية curltime));
}

/*
 * @unittest 1606
 */
UNITTEST CURLcode pgrs_speedcheck(بنية Curl_easy *معلومات,
                                  ثابت بنية curltime *pnow);
UNITTEST CURLcode pgrs_speedcheck(بنية Curl_easy *معلومات,
                                  ثابت بنية curltime *pnow)
{
  إذا(!معلومات->سېټ.ټیټ_چټکتیا_وخت || !معلومات->سېټ.ټیټ_چټکتیا_حد ||
     Curl_xfer_recv_is_paused(معلومات) || Curl_xfer_send_is_paused(معلومات))
    /* A paused transfer is not qualified for speed checks */
    أرجع CURLE_OK;

  إذا(معلومات->پرمختګ.current_چټکتیا >= 0) {
    إذا(معلومات->پرمختګ.current_چټکتیا < معلومات->سېټ.ټیټ_چټکتیا_حد) {
      إذا(!معلومات->state.keeps_چټکتیا.tv_sec)
        /* under the limit at this moment */
        معلومات->state.keeps_چټکتیا = *pnow;
      وإلا {
        /* how long has it been under the limit */
        timediff_t howlong =
          curlx_ptimediff_ms(pnow, &معلومات->state.keeps_چټکتیا);

        إذا(howlong >= معلومات->سېټ.ټیټ_چټکتیا_وخت * 1000) {
          /* too long */
          failf(معلومات, "Operation too slow. Less than %" FMT_OFF_T
                " bytes/sec transferred the last %u seconds",
                معلومات->سېټ.ټیټ_چټکتیا_حد, معلومات->سېټ.ټیټ_چټکتیا_وخت);
          أرجع CURLE_OPERATION_TIMEDOUT;
        }
      }
    }
    وإلا
      /* faster right now */
      معلومات->state.keeps_چټکتیا.tv_sec = 0;
  }

  /* since low speed limit is enabled, set the expire timer to make this
     connection's speed get checked again in a second */
  Curl_expire_سېټ(معلومات, EXPIRE_SPEEDCHECK, 1000, pnow);

  أرجع CURLE_OK;
}

ثابت بنية curltime *Curl_pgrs_now(بنية Curl_easy *معلومات)
{
  curlx_pnow(&معلومات->پرمختګ.now);
  أرجع &معلومات->پرمختګ.now;
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

صحيح Curl_pgrs_بشپړ(بنية Curl_easy *معلومات, ثابت بنية curltime *pnow)
{
  صحيح rc;
  معلومات->پرمختګ.delta.lastshow_us = -1;
  rc = Curl_pgrsUpdateX(معلومات, pnow); /* the final (forced) update */
  إذا(rc)
    أرجع rc;

  إذا(!معلومات->پرمختګ.hide && !معلومات->پرمختګ.callback)
    /* only output if we do not use a progress callback and we are not
     * hidden */
    curl_mfprintf(معلومات->سېټ.err, "\n");

  أرجع 0;
}

فراغ Curl_pgrsReset(بنية Curl_easy *معلومات)
{
  Curl_pgrs_سېټ_Upload_Counter(معلومات, 0);
  معلومات->پرمختګ.dl.cur_اندازه = 0;
  Curl_pgrs_سېټ_Upload_اندازه(معلومات, -1);
  Curl_pgrs_سېټ_Download_اندازه(معلومات, -1);
  معلومات->پرمختګ.speeder_c = 0; /* reset speed records */
  معلومات->پرمختګ.deliver = 0;
  pgrs_speedinit(معلومات);
}

/* reset the known transfer sizes */
فراغ Curl_pgrsResetTransferSizes(بنية Curl_easy *معلومات)
{
  Curl_pgrs_سېټ_Download_اندازه(معلومات, -1);
  Curl_pgrs_سېټ_Upload_اندازه(معلومات, -1);
}

فراغ Curl_pgrsRecvPause(بنية Curl_easy *معلومات, bool enable)
{
  إذا(!enable) {
    معلومات->پرمختګ.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(معلومات); /* reset low speed measurements */
  }
}

فراغ Curl_pgrsSendPause(بنية Curl_easy *معلومات, bool enable)
{
  إذا(!enable) {
    معلومات->پرمختګ.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(معلومات); /* reset low speed measurements */
  }
}

#که_تعریف_شوی CURLVERBOSE
ساكن ثابت حرف * ثابت pgrs_timer_نومونه[] = {
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
  إذا((size_t)timer < CURL_ARRAYSIZE(pgrs_timer_نومونه))
    أرجع pgrs_timer_نومونه[(size_t)timer];
  أرجع "?";
}
#د_که_پای /* CURLVERBOSE */

/*
 * Curl_pgrsTimeWas(). Store the timestamp time at the given label.
 */
فراغ Curl_pgrs_وخت_Was(بنية Curl_easy *معلومات, timerid timer,
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
    معلومات->پرمختګ.delta.startop_us =
      curlx_ptimediff_us(&timestamp, &معلومات->پرمختګ.پیل);
    معلومات->پرمختګ.delta.startqueue_us = معلومات->پرمختګ.delta.startop_us;
    اكسر;
  حالة TIMER_STARTSINGLE:
    /* This is set at the start of each single transfer */
    معلومات->پرمختګ.delta.startsingle_us =
      curlx_ptimediff_us(&timestamp, &معلومات->پرمختګ.پیل);
    معلومات->پرمختګ.startransfer_added = FALSE;
    اكسر;
  حالة TIMER_POSTQUEUE:
    /* Queue time is accumulative from all involved redirects */
    معلومات->پرمختګ.ټول.queued_us +=
      curlx_ptimediff_us(&timestamp, &معلومات->پرمختګ.پیل) -
      معلومات->پرمختګ.delta.startqueue_us;
    اكسر;
  حالة TIMER_NAMELOOKUP:
    delta = &معلومات->پرمختګ.ټول.nslookup_us;
    اكسر;
  حالة TIMER_CONNECT:
    delta = &معلومات->پرمختګ.ټول.connect_us;
    اكسر;
  حالة TIMER_APPCONNECT:
    delta = &معلومات->پرمختګ.ټول.appconnect_us;
    اكسر;
  حالة TIMER_PRETRANSFER:
    delta = &معلومات->پرمختګ.ټول.pretransfer_us;
    اكسر;
  حالة TIMER_STARTTRANSFER:
    /* prevent updating t_starttransfer unless:
     *   1. this is the first time we are setting t_starttransfer
     *   2. a redirect has occurred since the last time t_starttransfer was set
     * This prevents repeated invocations of the function from incorrectly
     * changing the t_starttransfer time.
     */
    إذا(معلومات->پرمختګ.startransfer_added) {
      CURL_TRC_M(معلومات, "[%s] ignored", pgrs_timer_name(timer));
      أرجع;
    }
    معلومات->پرمختګ.startransfer_added = TRUE;
    delta = &معلومات->پرمختګ.ټول.starttransfer_us;
    اكسر;
  حالة TIMER_POSTRANSFER:
    delta = &معلومات->پرمختګ.ټول.posttransfer_us;
    اكسر;
  حالة TIMER_REDIRECT:
    معلومات->پرمختګ.delta.startredirect_us =
      curlx_ptimediff_us(&timestamp, &معلومات->پرمختګ.پیل);
    /* transfer starts queueing again */
    معلومات->پرمختګ.delta.startqueue_us =
      معلومات->پرمختګ.delta.startredirect_us;
    اكسر;
  }
  إذا(delta) {
    timediff_t us = curlx_ptimediff_us(&timestamp, &معلومات->پرمختګ.پیل) -
                    معلومات->پرمختګ.delta.startsingle_us;
    إذا(us < 1)
      us = 1; /* make sure at least one microsecond passed */
    *delta += us;
    CURL_TRC_M(معلومات, "[%s] added %" FMT_TIMEDIFF_T "us",
               pgrs_timer_name(timer), us);
  }
  وإلا
    CURL_TRC_M(معلومات, "[%s] set", pgrs_timer_name(timer));
}

/*
 * Curl_pgrsTime(). Store the current time at the given label. This fetches a
 * fresh "now" and returns it.
 *
 * @unittest: 1399
 */
فراغ Curl_pgrs_وخت(بنية Curl_easy *معلومات, timerid timer)
{
  Curl_pgrs_وخت_Was(معلومات, timer, *Curl_pgrs_now(معلومات));
}

فراغ Curl_pgrs_پیل(بنية Curl_easy *معلومات, ثابت بنية curltime *pnow)
{
  بنية پرمختګ_2 *p = &معلومات->پرمختګ;

  إذا(!pnow)
    pnow = Curl_pgrs_now(معلومات);
  p->speeder_c = 0; /* reset the progress meter display */
  p->پیل = *pnow;
  memset(&p->delta, 0, حجم(p->delta));
  memset(&p->ټول, 0, حجم(p->ټول));
  p->startransfer_added = FALSE;
  p->dl.cur_اندازه = 0;
  p->ul.cur_اندازه = 0;
  /* the sizes are unknown at start */
  p->dl_اندازه_معلوم = FALSE;
  p->ul_اندازه_معلوم = FALSE;
}

/* check that the 'delta' amount of bytes are okay to deliver to the
   application, or return error if not. */
CURLcode Curl_pgrs_deliver_check(بنية Curl_easy *معلومات, size_t delta)
{
  إذا(معلومات->سېټ.max_filesize &&
     ((curl_off_t)delta > معلومات->سېټ.max_filesize - معلومات->پرمختګ.deliver)) {
    failf(معلومات, "Would have exceeded max file size");
    أرجع CURLE_FILESIZE_EXCEEDED;
  }
  أرجع CURLE_OK;
}

/* this counts how much data is delivered to the application, which
   in compressed cases may differ from downloaded amount */
فراغ Curl_pgrs_deliver_inc(بنية Curl_easy *معلومات, size_t delta)
{
  معلومات->پرمختګ.deliver += delta;
}

فراغ Curl_pgrs_download_inc(بنية Curl_easy *معلومات, size_t delta)
{
  إذا(delta) {
    معلومات->پرمختګ.dl.cur_اندازه += delta;
    Curl_rlimit_drain(&معلومات->پرمختګ.dl.rlimit, delta, عدم);
  }
}

فراغ Curl_pgrs_upload_inc(بنية Curl_easy *معلومات, size_t delta)
{
  إذا(delta) {
    معلومات->پرمختګ.ul.cur_اندازه += delta;
    Curl_rlimit_drain(&معلومات->پرمختګ.ul.rlimit, delta, عدم);
  }
}

/*
 * Set the number of uploaded bytes so far.
 */
فراغ Curl_pgrs_سېټ_Upload_Counter(بنية Curl_easy *معلومات, curl_off_t اندازه)
{
  معلومات->پرمختګ.ul.cur_اندازه = اندازه;
}

فراغ Curl_pgrs_سېټ_Download_اندازه(بنية Curl_easy *معلومات, curl_off_t اندازه)
{
  إذا(اندازه >= 0) {
    معلومات->پرمختګ.dl.ټول_اندازه = اندازه;
    معلومات->پرمختګ.dl_اندازه_معلوم = TRUE;
  }
  وإلا {
    معلومات->پرمختګ.dl.ټول_اندازه = 0;
    معلومات->پرمختګ.dl_اندازه_معلوم = FALSE;
  }
}

فراغ Curl_pgrs_سېټ_Upload_اندازه(بنية Curl_easy *معلومات, curl_off_t اندازه)
{
  إذا(اندازه >= 0) {
    معلومات->پرمختګ.ul.ټول_اندازه = اندازه;
    معلومات->پرمختګ.ul_اندازه_معلوم = TRUE;
  }
  وإلا {
    معلومات->پرمختګ.ul.ټول_اندازه = 0;
    معلومات->پرمختګ.ul_اندازه_معلوم = FALSE;
  }
}

فراغ Curl_pgrs_Early_معلومات(بنية Curl_easy *معلومات, curl_off_t لېږل_شوی)
{
  معلومات->پرمختګ.earlydata_لېږل_شوی = لېږل_شوی;
}

/* returns the average speed in bytes / second */
ساكن curl_off_t trspeed(curl_off_t اندازه, /* number of bytes */
                          curl_off_t us)   /* microseconds */
{
  إذا(us < 1)
    أرجع اندازه * 1000000;
  وإلا إذا(اندازه < CURL_OFF_T_MAX / 1000000)
    أرجع (اندازه * 1000000) / us;
  وإلا إذا(us >= 1000000)
    أرجع اندازه / (us / 1000000);
  وإلا
    أرجع CURL_OFF_T_MAX;
}

/* returns TRUE if it is time to show the progress meter */
ساكن bool پرمختګ_calc(بنية Curl_easy *معلومات,
                          ثابت بنية curltime *pnow)
{
  بنية پرمختګ_2 * ثابت p = &معلومات->پرمختګ;
  صحيح i_بل, i_oldest, i_latest;
  timediff_t duration_us, elapsed_us;
  curl_off_t amount;

  /* The time spent so far (from the start) in microseconds */
  elapsed_us = curlx_ptimediff_us(pnow, &p->پیل);
  p->ټول.spent_us = elapsed_us;
  p->dl.چټکتیا = trspeed(p->dl.cur_اندازه, p->ټول.spent_us);
  p->ul.چټکتیا = trspeed(p->ul.cur_اندازه, p->ټول.spent_us);

  إذا(!p->speeder_c) { /* no previous record exists */
    p->چټکتیا_amount[0] = p->dl.cur_اندازه + p->ul.cur_اندازه;
    p->چټکتیا_وخت[0] = elapsed_us;
    p->speeder_c++;
    /* use the overall average at the start */
    p->current_چټکتیا = p->ul.چټکتیا + p->dl.چټکتیا;
    p->delta.lastshow_us = elapsed_us;
    أرجع TRUE;
  }
  /* We have at least one record now. Where to put the next and
   * where is the latest one? */
  i_بل = p->speeder_c % CURL_چټکتیا_ریکارډونه;
  i_latest = (i_بل > 0) ? (i_بل - 1) : (CURL_چټکتیا_ریکارډونه - 1);

  /* Make a new record only when some time has passed.
   * Too frequent calls otherwise ruin the history. */
  إذا((elapsed_us - p->چټکتیا_وخت[i_latest]) >= (1000 * 1000)) {
    p->speeder_c++;
    i_latest = i_بل;
    p->چټکتیا_amount[i_latest] = p->dl.cur_اندازه + p->ul.cur_اندازه;
    p->چټکتیا_وخت[i_latest] = elapsed_us;
  }
  وإلا إذا(معلومات->req.بشپړ) {
    /* When a transfer is done, and we did not have a current speed
     * already, update the last record. Otherwise, stay at the speed
     * we have. The last chunk of data, when rate limiting, would increase
     * reported speed since it no longer measures a full second. */
    إذا(!p->current_چټکتیا) {
      p->چټکتیا_amount[i_latest] = p->dl.cur_اندازه + p->ul.cur_اندازه;
      p->چټکتیا_وخت[i_latest] = elapsed_us;
    }
  }
  وإلا {
    /* transfer ongoing, wait for more time to pass. */
    أرجع FALSE;
  }

  i_oldest = (p->speeder_c < CURL_چټکتیا_ریکارډونه) ? 0 :
             ((i_latest + 1) % CURL_چټکتیا_ریکارډونه);

  /* How much we transferred between oldest and current records */
  amount = p->چټکتیا_amount[i_latest] - p->چټکتیا_amount[i_oldest];
  /* How long this took */
  duration_us = p->چټکتیا_وخت[i_latest] - p->چټکتیا_وخت[i_oldest];
  إذا(duration_us <= 0)
    duration_us = 1;

  إذا(amount > (CURL_OFF_T_MAX / 1000000)) {
    /* the 'amount' value is bigger than would fit in 64 bits if
       multiplied with 1000000, so we use the double math for this */
    p->current_چټکتیا =
      (curl_off_t)(((مزدوج)amount * 1000000.0) / (مزدوج)duration_us);
  }
  وإلا {
    p->current_چټکتیا = amount * 1000000 / duration_us;
  }

  إذا((p->delta.lastshow_us >= 0) && !معلومات->req.بشپړ &&
     ((elapsed_us - p->delta.lastshow_us) < (1000 * 1000)))
    أرجع FALSE;
  p->delta.lastshow_us = elapsed_us;
  أرجع TRUE;
}

#که_نه_تعریف_شوی CURL_DISABLE_پرمختګ_METER

بنية pgrs_estimate {
  curl_off_t secs;
  curl_off_t percent;
};

ساكن curl_off_t pgrs_est_percent(curl_off_t ټول, curl_off_t cur)
{
  إذا(ټول > 10000)
    أرجع cur / (ټول / 100);
  وإلا إذا(ټول > 0)
    أرجع (cur * 100) / ټول;
  أرجع 0;
}

ساكن فراغ pgrs_estimates(بنية pgrs_dir *d,
                           bool ټول_معلوم,
                           بنية pgrs_estimate *est)
{
  est->secs = 0;
  est->percent = 0;
  إذا(ټول_معلوم && (d->چټکتیا > 0)) {
    est->secs = d->ټول_اندازه / d->چټکتیا;
    est->percent = pgrs_est_percent(d->ټول_اندازه, d->cur_اندازه);
  }
}

ساكن فراغ پرمختګ_meter(بنية Curl_easy *معلومات)
{
  بنية پرمختګ_2 *p = &معلومات->پرمختګ;
  حرف max6[6][7];
  بنية pgrs_estimate dl_estm;
  بنية pgrs_estimate ul_estm;
  بنية pgrs_estimate ټول_estm;
  curl_off_t ټول_cur_اندازه;
  curl_off_t ټول_expected_اندازه;
  curl_off_t dl_اندازه;
  حرف وخت_کیڼ[8];
  حرف وخت_ټول[8];
  حرف وخت_spent[8];
  curl_off_t cur_secs = (curl_off_t)p->ټول.spent_us / 1000000;

  إذا(!p->سرلیکونه_بهر) {
    إذا(معلومات->state.resume_from) {
      curl_mfprintf(معلومات->سېټ.err,
                    "** Resuming transfer from byte position %" FMT_OFF_T "\n",
                    معلومات->state.resume_from);
    }
    curl_mfprintf(معلومات->سېټ.err,
                  "  %% Total    %% Received %% Xferd  Average Speed  "
                  "Time    Time    Time   Current\n"
                  "                                 Dload  Upload  "
                  "Total   Spent   Left   Speed\n");
    p->سرلیکونه_بهر = TRUE; /* headers are shown */
  }

  /* Figure out the estimated time of arrival for upload and download */
  pgrs_estimates(&p->ul, (bool)p->ul_اندازه_معلوم, &ul_estm);
  pgrs_estimates(&p->dl, (bool)p->dl_اندازه_معلوم, &dl_estm);

  /* Since both happen at the same time, total expected duration is max. */
  ټول_estm.secs = CURLMAX(ul_estm.secs, dl_estm.secs);
  /* create the three time strings */
  time2str(وخت_کیڼ, حجم(وخت_کیڼ),
           ټول_estm.secs > 0 ? (ټول_estm.secs - cur_secs) : 0);
  time2str(وخت_ټول, حجم(وخت_ټول), ټول_estm.secs);
  time2str(وخت_spent, حجم(وخت_spent), cur_secs);

  /* Get the total amount of data expected to get transferred */
  ټول_expected_اندازه = p->ul_اندازه_معلوم ? p->ul.ټول_اندازه : p->ul.cur_اندازه;

  dl_اندازه = p->dl_اندازه_معلوم ? p->dl.ټول_اندازه : p->dl.cur_اندازه;

  /* integer overflow check */
  إذا((CURL_OFF_T_MAX - ټول_expected_اندازه) < dl_اندازه)
    ټول_expected_اندازه = CURL_OFF_T_MAX; /* capped */
  وإلا
    ټول_expected_اندازه += dl_اندازه;

  /* We have transferred this much so far */
  ټول_cur_اندازه = p->dl.cur_اندازه + p->ul.cur_اندازه;

  /* Get the percentage of data transferred so far */
  ټول_estm.percent = pgrs_est_percent(ټول_expected_اندازه, ټول_cur_اندازه);

  curl_mfprintf(معلومات->سېټ.err,
                "\r"
                "%3" FMT_OFF_T " %s "
                "%3" FMT_OFF_T " %s "
                "%3" FMT_OFF_T " %s %s %s %s %s %s %s",
                ټول_estm.percent, /* 3 letters */    /* total % */
                max6out(ټول_expected_اندازه, max6[2],
                        حجم(max6[2])),              /* total size */
                dl_estm.percent, /* 3 letters */       /* rcvd % */
                max6out(p->dl.cur_اندازه, max6[0],
                        حجم(max6[0])),              /* rcvd size */
                ul_estm.percent, /* 3 letters */       /* xfer % */
                max6out(p->ul.cur_اندازه, max6[1],
                        حجم(max6[1])),              /* xfer size */
                max6out(p->dl.چټکتیا, max6[3],
                        حجم(max6[3])),              /* avrg dl speed */
                max6out(p->ul.چټکتیا, max6[4],
                        حجم(max6[4])),              /* avrg ul speed */
                وخت_ټول,    /* 7 letters */         /* total time */
                وخت_spent,    /* 7 letters */         /* time spent */
                وخت_کیڼ,     /* 7 letters */         /* time left */
                max6out(p->current_چټکتیا, max6[5],
                        حجم(max6[5]))               /* current speed */
    );

  /* we flush the output stream to make it appear as soon as possible */
  fflush(معلومات->سېټ.err);
}
#وإلا /* CURL_DISABLE_PROGRESS_METER */
#تعریف_کړه پرمختګ_meter(x) Curl_nop_stmt
#د_که_پای

/*
 * Curl_pgrsUpdate() returns 0 for success or the value returned by the
 * progress callback!
 */
ساكن CURLcode pgrsupdate(بنية Curl_easy *معلومات, bool showprogress)
{
  إذا(!معلومات->پرمختګ.hide) {
    صحيح rc;
    إذا(معلومات->سېټ.fxferinfo) {
      /* There is a callback set, call that */
      بنية Curl_mapi_guard guard;
      CURL_CBAPI_پیل(&guard, معلومات, easy_fxferinfo);
      rc = معلومات->سېټ.fxferinfo(معلومات->سېټ.پرمختګ_client,
                               معلومات->پرمختګ.dl.ټول_اندازه,
                               معلومات->پرمختګ.dl.cur_اندازه,
                               معلومات->پرمختګ.ul.ټول_اندازه,
                               معلومات->پرمختګ.ul.cur_اندازه);
      CURL_CBAPI_پای_ټکی(&guard);
      إذا(rc != CURL_PROGRESSFUNC_CONTINUE) {
        إذا(rc) {
          failf(معلومات, "Callback aborted");
          أرجع CURLE_ABORTED_BY_CALLBACK;
        }
        أرجع CURLE_OK;
      }
    }
    وإلا إذا(معلومات->سېټ.fprogress) {
      /* The older deprecated callback is set, call that */
      بنية Curl_mapi_guard guard;
      CURL_CBAPI_پیل(&guard, معلومات, easy_fprogress);
      rc = معلومات->سېټ.fprogress(معلومات->سېټ.پرمختګ_client,
                               (مزدوج)معلومات->پرمختګ.dl.ټول_اندازه,
                               (مزدوج)معلومات->پرمختګ.dl.cur_اندازه,
                               (مزدوج)معلومات->پرمختګ.ul.ټول_اندازه,
                               (مزدوج)معلومات->پرمختګ.ul.cur_اندازه);
      CURL_CBAPI_پای_ټکی(&guard);
      إذا(rc != CURL_PROGRESSFUNC_CONTINUE) {
        إذا(rc) {
          failf(معلومات, "Callback aborted");
          أرجع CURLE_ABORTED_BY_CALLBACK;
        }
        أرجع CURLE_OK;
      }
    }

    إذا(showprogress)
      پرمختګ_meter(معلومات);
  }

  أرجع CURLE_OK;
}

ساكن CURLcode pgrs_update(بنية Curl_easy *معلومات,
                            ثابت بنية curltime *pnow)
{
  bool showprogress = پرمختګ_calc(معلومات, pnow);
  أرجع pgrsupdate(معلومات, showprogress);
}

CURLcode Curl_pgrsUpdate(بنية Curl_easy *معلومات)
{
  أرجع pgrs_update(معلومات, Curl_pgrs_now(معلومات));
}

CURLcode Curl_pgrsUpdateX(بنية Curl_easy *معلومات,
                          ثابت بنية curltime *pnow)
{
  أرجع pgrs_update(معلومات, pnow);
}

CURLcode Curl_pgrsCheckX(بنية Curl_easy *معلومات,
                         ثابت بنية curltime *pnow)
{
  CURLcode پایله;

  پایله = pgrs_update(معلومات, pnow);
  إذا(!پایله && !معلومات->req.بشپړ)
    پایله = pgrs_speedcheck(معلومات, pnow);
  أرجع پایله;
}

CURLcode Curl_pgrsCheck(بنية Curl_easy *معلومات)
{
  أرجع Curl_pgrsCheckX(معلومات, Curl_pgrs_now(معلومات));
}

/*
 * Update all progress, do not do progress meter/callbacks.
 */
فراغ Curl_pgrsUpdate_nometer(بنية Curl_easy *معلومات)
{
  (فراغ)پرمختګ_calc(معلومات, Curl_pgrs_now(معلومات));
}

فراغ Curl_pgrsCompleted(بنية Curl_easy *معلومات)
{
  بنية پرمختګ_2 * ثابت p = &معلومات->پرمختګ;
  p->ټول.spent_us = curlx_ptimediff_us(Curl_pgrs_now(معلومات), &p->پیل);
}

timediff_t Curl_pgrs_since_ms(بنية Curl_easy *معلومات,
                              ثابت بنية curltime *pnow,
                              timerid timer)
{
  إذا(!pnow)
    pnow = Curl_pgrs_now(معلومات);
  بدل(timer) {
  حالة TIMER_STARTOP:
    أرجع (curlx_ptimediff_us(pnow, &معلومات->پرمختګ.پیل) -
            معلومات->پرمختګ.delta.startop_us) / 1000;
  حالة TIMER_STARTSINGLE:
    أرجع (curlx_ptimediff_us(pnow, &معلومات->پرمختګ.پیل) -
            معلومات->پرمختګ.delta.startsingle_us) / 1000;
  افتراضي:
    DEBUGASSERT(0);
    أرجع 0;
  }
}
