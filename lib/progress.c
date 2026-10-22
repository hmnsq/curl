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

#اگر_تعریف_نشده CURL_DISABLE_پیشرفت_METER
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
UNITTEST حرف *max6out(curl_off_t بایتها, حرف *max6, size_t mlen);
UNITTEST حرف *max6out(curl_off_t بایتها, حرف *max6, size_t mlen)
{
  /* a signed 64-bit value is 8192 petabytes maximum, shown as
     8.0E (exabytes)*/
  إذا(بایتها < 100000)
    curl_msnprintf(max6, mlen, "%6" CURL_FORMAT_CURL_OFF_T, بایتها);
  وإلا {
    ساكن ثابت حرف unit[] = { 'k', 'M', 'G', 'T', 'P', 'E', 0 };
    صحيح k = 0;
    curl_off_t nbytes;
    curl_off_t rest;
    افعل {
      nbytes = بایتها / 1024;
      إذا(nbytes < 1000)
        اكسر;
      بایتها = nbytes;
      k++;
      DEBUGASSERT(unit[k]);
    } طالما(unit[k]);
    rest = بایتها % 1024;
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
#پایان_اگر

ساكن فراغ pgrs_speedinit(بنية Curl_easy *داده)
{
  memset(&داده->state.keeps_سرعت, 0, حجم(بنية curltime));
}

/*
 * @unittest 1606
 */
UNITTEST CURLcode pgrs_speedcheck(بنية Curl_easy *داده,
                                  ثابت بنية curltime *pnow);
UNITTEST CURLcode pgrs_speedcheck(بنية Curl_easy *داده,
                                  ثابت بنية curltime *pnow)
{
  إذا(!داده->مجموعه.کم_سرعت_زمان || !داده->مجموعه.کم_سرعت_حد ||
     Curl_xfer_recv_is_paused(داده) || Curl_xfer_send_is_paused(داده))
    /* A paused transfer is not qualified for speed checks */
    أرجع CURLE_OK;

  إذا(داده->پیشرفت.current_سرعت >= 0) {
    إذا(داده->پیشرفت.current_سرعت < داده->مجموعه.کم_سرعت_حد) {
      إذا(!داده->state.keeps_سرعت.tv_sec)
        /* under the limit at this moment */
        داده->state.keeps_سرعت = *pnow;
      وإلا {
        /* how long has it been under the limit */
        timediff_t howlong =
          curlx_ptimediff_ms(pnow, &داده->state.keeps_سرعت);

        إذا(howlong >= داده->مجموعه.کم_سرعت_زمان * 1000) {
          /* too long */
          failf(داده, "Operation too slow. Less than %" FMT_OFF_T
                " bytes/sec transferred the last %u seconds",
                داده->مجموعه.کم_سرعت_حد, داده->مجموعه.کم_سرعت_زمان);
          أرجع CURLE_OPERATION_TIMEDOUT;
        }
      }
    }
    وإلا
      /* faster right now */
      داده->state.keeps_سرعت.tv_sec = 0;
  }

  /* since low speed limit is enabled, set the expire timer to make this
     connection's speed get checked again in a second */
  Curl_expire_مجموعه(داده, EXPIRE_SPEEDCHECK, 1000, pnow);

  أرجع CURLE_OK;
}

ثابت بنية curltime *Curl_pgrs_now(بنية Curl_easy *داده)
{
  curlx_pnow(&داده->پیشرفت.now);
  أرجع &داده->پیشرفت.now;
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

صحيح Curl_pgrs_انجامشده(بنية Curl_easy *داده, ثابت بنية curltime *pnow)
{
  صحيح rc;
  داده->پیشرفت.delta.lastshow_us = -1;
  rc = Curl_pgrsUpdateX(داده, pnow); /* the final (forced) update */
  إذا(rc)
    أرجع rc;

  إذا(!داده->پیشرفت.hide && !داده->پیشرفت.callback)
    /* only output if we do not use a progress callback and we are not
     * hidden */
    curl_mfprintf(داده->مجموعه.err, "\n");

  أرجع 0;
}

فراغ Curl_pgrsReset(بنية Curl_easy *داده)
{
  Curl_pgrs_مجموعه_Upload_Counter(داده, 0);
  داده->پیشرفت.dl.cur_اندازه = 0;
  Curl_pgrs_مجموعه_Upload_اندازه(داده, -1);
  Curl_pgrs_مجموعه_Download_اندازه(داده, -1);
  داده->پیشرفت.speeder_c = 0; /* reset speed records */
  داده->پیشرفت.deliver = 0;
  pgrs_speedinit(داده);
}

/* reset the known transfer sizes */
فراغ Curl_pgrsResetTransferSizes(بنية Curl_easy *داده)
{
  Curl_pgrs_مجموعه_Download_اندازه(داده, -1);
  Curl_pgrs_مجموعه_Upload_اندازه(داده, -1);
}

فراغ Curl_pgrsRecvPause(بنية Curl_easy *داده, bool enable)
{
  إذا(!enable) {
    داده->پیشرفت.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(داده); /* reset low speed measurements */
  }
}

فراغ Curl_pgrsSendPause(بنية Curl_easy *داده, bool enable)
{
  إذا(!enable) {
    داده->پیشرفت.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(داده); /* reset low speed measurements */
  }
}

#اگر_تعریف_شده CURLVERBOSE
ساكن ثابت حرف * ثابت pgrs_timer_نامها[] = {
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
  إذا((size_t)timer < CURL_ARRAYSIZE(pgrs_timer_نامها))
    أرجع pgrs_timer_نامها[(size_t)timer];
  أرجع "?";
}
#پایان_اگر /* CURLVERBOSE */

/*
 * Curl_pgrsTimeWas(). Store the timestamp time at the given label.
 */
فراغ Curl_pgrs_زمان_Was(بنية Curl_easy *داده, timerid timer,
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
    داده->پیشرفت.delta.startop_us =
      curlx_ptimediff_us(&timestamp, &داده->پیشرفت.شروع);
    داده->پیشرفت.delta.startqueue_us = داده->پیشرفت.delta.startop_us;
    اكسر;
  حالة TIMER_STARTSINGLE:
    /* This is set at the start of each single transfer */
    داده->پیشرفت.delta.startsingle_us =
      curlx_ptimediff_us(&timestamp, &داده->پیشرفت.شروع);
    داده->پیشرفت.startransfer_added = FALSE;
    اكسر;
  حالة TIMER_POSTQUEUE:
    /* Queue time is accumulative from all involved redirects */
    داده->پیشرفت.مجموع.queued_us +=
      curlx_ptimediff_us(&timestamp, &داده->پیشرفت.شروع) -
      داده->پیشرفت.delta.startqueue_us;
    اكسر;
  حالة TIMER_NAMELOOKUP:
    delta = &داده->پیشرفت.مجموع.nslookup_us;
    اكسر;
  حالة TIMER_CONNECT:
    delta = &داده->پیشرفت.مجموع.connect_us;
    اكسر;
  حالة TIMER_APPCONNECT:
    delta = &داده->پیشرفت.مجموع.appconnect_us;
    اكسر;
  حالة TIMER_PRETRANSFER:
    delta = &داده->پیشرفت.مجموع.pretransfer_us;
    اكسر;
  حالة TIMER_STARTTRANSFER:
    /* prevent updating t_starttransfer unless:
     *   1. this is the first time we are setting t_starttransfer
     *   2. a redirect has occurred since the last time t_starttransfer was set
     * This prevents repeated invocations of the function from incorrectly
     * changing the t_starttransfer time.
     */
    إذا(داده->پیشرفت.startransfer_added) {
      CURL_TRC_M(داده, "[%s] ignored", pgrs_timer_name(timer));
      أرجع;
    }
    داده->پیشرفت.startransfer_added = TRUE;
    delta = &داده->پیشرفت.مجموع.starttransfer_us;
    اكسر;
  حالة TIMER_POSTRANSFER:
    delta = &داده->پیشرفت.مجموع.posttransfer_us;
    اكسر;
  حالة TIMER_REDIRECT:
    داده->پیشرفت.delta.startredirect_us =
      curlx_ptimediff_us(&timestamp, &داده->پیشرفت.شروع);
    /* transfer starts queueing again */
    داده->پیشرفت.delta.startqueue_us =
      داده->پیشرفت.delta.startredirect_us;
    اكسر;
  }
  إذا(delta) {
    timediff_t us = curlx_ptimediff_us(&timestamp, &داده->پیشرفت.شروع) -
                    داده->پیشرفت.delta.startsingle_us;
    إذا(us < 1)
      us = 1; /* make sure at least one microsecond passed */
    *delta += us;
    CURL_TRC_M(داده, "[%s] added %" FMT_TIMEDIFF_T "us",
               pgrs_timer_name(timer), us);
  }
  وإلا
    CURL_TRC_M(داده, "[%s] set", pgrs_timer_name(timer));
}

/*
 * Curl_pgrsTime(). Store the current time at the given label. This fetches a
 * fresh "now" and returns it.
 *
 * @unittest: 1399
 */
فراغ Curl_pgrs_زمان(بنية Curl_easy *داده, timerid timer)
{
  Curl_pgrs_زمان_Was(داده, timer, *Curl_pgrs_now(داده));
}

فراغ Curl_pgrs_شروع(بنية Curl_easy *داده, ثابت بنية curltime *pnow)
{
  بنية پیشرفت_2 *p = &داده->پیشرفت;

  إذا(!pnow)
    pnow = Curl_pgrs_now(داده);
  p->speeder_c = 0; /* reset the progress meter display */
  p->شروع = *pnow;
  memset(&p->delta, 0, حجم(p->delta));
  memset(&p->مجموع, 0, حجم(p->مجموع));
  p->startransfer_added = FALSE;
  p->dl.cur_اندازه = 0;
  p->ul.cur_اندازه = 0;
  /* the sizes are unknown at start */
  p->dl_اندازه_معلوم = FALSE;
  p->ul_اندازه_معلوم = FALSE;
}

/* check that the 'delta' amount of bytes are okay to deliver to the
   application, or return error if not. */
CURLcode Curl_pgrs_deliver_check(بنية Curl_easy *داده, size_t delta)
{
  إذا(داده->مجموعه.max_filesize &&
     ((curl_off_t)delta > داده->مجموعه.max_filesize - داده->پیشرفت.deliver)) {
    failf(داده, "Would have exceeded max file size");
    أرجع CURLE_FILESIZE_EXCEEDED;
  }
  أرجع CURLE_OK;
}

/* this counts how much data is delivered to the application, which
   in compressed cases may differ from downloaded amount */
فراغ Curl_pgrs_deliver_inc(بنية Curl_easy *داده, size_t delta)
{
  داده->پیشرفت.deliver += delta;
}

فراغ Curl_pgrs_download_inc(بنية Curl_easy *داده, size_t delta)
{
  إذا(delta) {
    داده->پیشرفت.dl.cur_اندازه += delta;
    Curl_rlimit_drain(&داده->پیشرفت.dl.rlimit, delta, عدم);
  }
}

فراغ Curl_pgrs_upload_inc(بنية Curl_easy *داده, size_t delta)
{
  إذا(delta) {
    داده->پیشرفت.ul.cur_اندازه += delta;
    Curl_rlimit_drain(&داده->پیشرفت.ul.rlimit, delta, عدم);
  }
}

/*
 * Set the number of uploaded bytes so far.
 */
فراغ Curl_pgrs_مجموعه_Upload_Counter(بنية Curl_easy *داده, curl_off_t اندازه)
{
  داده->پیشرفت.ul.cur_اندازه = اندازه;
}

فراغ Curl_pgrs_مجموعه_Download_اندازه(بنية Curl_easy *داده, curl_off_t اندازه)
{
  إذا(اندازه >= 0) {
    داده->پیشرفت.dl.مجموع_اندازه = اندازه;
    داده->پیشرفت.dl_اندازه_معلوم = TRUE;
  }
  وإلا {
    داده->پیشرفت.dl.مجموع_اندازه = 0;
    داده->پیشرفت.dl_اندازه_معلوم = FALSE;
  }
}

فراغ Curl_pgrs_مجموعه_Upload_اندازه(بنية Curl_easy *داده, curl_off_t اندازه)
{
  إذا(اندازه >= 0) {
    داده->پیشرفت.ul.مجموع_اندازه = اندازه;
    داده->پیشرفت.ul_اندازه_معلوم = TRUE;
  }
  وإلا {
    داده->پیشرفت.ul.مجموع_اندازه = 0;
    داده->پیشرفت.ul_اندازه_معلوم = FALSE;
  }
}

فراغ Curl_pgrs_Early_داده(بنية Curl_easy *داده, curl_off_t فرستاده)
{
  داده->پیشرفت.earlydata_فرستاده = فرستاده;
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
ساكن bool پیشرفت_calc(بنية Curl_easy *داده,
                          ثابت بنية curltime *pnow)
{
  بنية پیشرفت_2 * ثابت p = &داده->پیشرفت;
  صحيح i_بعدی, i_oldest, i_latest;
  timediff_t duration_us, elapsed_us;
  curl_off_t amount;

  /* The time spent so far (from the start) in microseconds */
  elapsed_us = curlx_ptimediff_us(pnow, &p->شروع);
  p->مجموع.spent_us = elapsed_us;
  p->dl.سرعت = trspeed(p->dl.cur_اندازه, p->مجموع.spent_us);
  p->ul.سرعت = trspeed(p->ul.cur_اندازه, p->مجموع.spent_us);

  إذا(!p->speeder_c) { /* no previous record exists */
    p->سرعت_amount[0] = p->dl.cur_اندازه + p->ul.cur_اندازه;
    p->سرعت_زمان[0] = elapsed_us;
    p->speeder_c++;
    /* use the overall average at the start */
    p->current_سرعت = p->ul.سرعت + p->dl.سرعت;
    p->delta.lastshow_us = elapsed_us;
    أرجع TRUE;
  }
  /* We have at least one record now. Where to put the next and
   * where is the latest one? */
  i_بعدی = p->speeder_c % CURL_سرعت_رکوردها;
  i_latest = (i_بعدی > 0) ? (i_بعدی - 1) : (CURL_سرعت_رکوردها - 1);

  /* Make a new record only when some time has passed.
   * Too frequent calls otherwise ruin the history. */
  إذا((elapsed_us - p->سرعت_زمان[i_latest]) >= (1000 * 1000)) {
    p->speeder_c++;
    i_latest = i_بعدی;
    p->سرعت_amount[i_latest] = p->dl.cur_اندازه + p->ul.cur_اندازه;
    p->سرعت_زمان[i_latest] = elapsed_us;
  }
  وإلا إذا(داده->req.انجامشده) {
    /* When a transfer is done, and we did not have a current speed
     * already, update the last record. Otherwise, stay at the speed
     * we have. The last chunk of data, when rate limiting, would increase
     * reported speed since it no longer measures a full second. */
    إذا(!p->current_سرعت) {
      p->سرعت_amount[i_latest] = p->dl.cur_اندازه + p->ul.cur_اندازه;
      p->سرعت_زمان[i_latest] = elapsed_us;
    }
  }
  وإلا {
    /* transfer ongoing, wait for more time to pass. */
    أرجع FALSE;
  }

  i_oldest = (p->speeder_c < CURL_سرعت_رکوردها) ? 0 :
             ((i_latest + 1) % CURL_سرعت_رکوردها);

  /* How much we transferred between oldest and current records */
  amount = p->سرعت_amount[i_latest] - p->سرعت_amount[i_oldest];
  /* How long this took */
  duration_us = p->سرعت_زمان[i_latest] - p->سرعت_زمان[i_oldest];
  إذا(duration_us <= 0)
    duration_us = 1;

  إذا(amount > (CURL_OFF_T_MAX / 1000000)) {
    /* the 'amount' value is bigger than would fit in 64 bits if
       multiplied with 1000000, so we use the double math for this */
    p->current_سرعت =
      (curl_off_t)(((مزدوج)amount * 1000000.0) / (مزدوج)duration_us);
  }
  وإلا {
    p->current_سرعت = amount * 1000000 / duration_us;
  }

  إذا((p->delta.lastshow_us >= 0) && !داده->req.انجامشده &&
     ((elapsed_us - p->delta.lastshow_us) < (1000 * 1000)))
    أرجع FALSE;
  p->delta.lastshow_us = elapsed_us;
  أرجع TRUE;
}

#اگر_تعریف_نشده CURL_DISABLE_پیشرفت_METER

بنية pgrs_estimate {
  curl_off_t secs;
  curl_off_t percent;
};

ساكن curl_off_t pgrs_est_percent(curl_off_t مجموع, curl_off_t cur)
{
  إذا(مجموع > 10000)
    أرجع cur / (مجموع / 100);
  وإلا إذا(مجموع > 0)
    أرجع (cur * 100) / مجموع;
  أرجع 0;
}

ساكن فراغ pgrs_estimates(بنية pgrs_dir *d,
                           bool مجموع_معلوم,
                           بنية pgrs_estimate *est)
{
  est->secs = 0;
  est->percent = 0;
  إذا(مجموع_معلوم && (d->سرعت > 0)) {
    est->secs = d->مجموع_اندازه / d->سرعت;
    est->percent = pgrs_est_percent(d->مجموع_اندازه, d->cur_اندازه);
  }
}

ساكن فراغ پیشرفت_meter(بنية Curl_easy *داده)
{
  بنية پیشرفت_2 *p = &داده->پیشرفت;
  حرف max6[6][7];
  بنية pgrs_estimate dl_estm;
  بنية pgrs_estimate ul_estm;
  بنية pgrs_estimate مجموع_estm;
  curl_off_t مجموع_cur_اندازه;
  curl_off_t مجموع_expected_اندازه;
  curl_off_t dl_اندازه;
  حرف زمان_چپ[8];
  حرف زمان_مجموع[8];
  حرف زمان_spent[8];
  curl_off_t cur_secs = (curl_off_t)p->مجموع.spent_us / 1000000;

  إذا(!p->سرآیندها_بیرون) {
    إذا(داده->state.resume_from) {
      curl_mfprintf(داده->مجموعه.err,
                    "** Resuming transfer from byte position %" FMT_OFF_T "\n",
                    داده->state.resume_from);
    }
    curl_mfprintf(داده->مجموعه.err,
                  "  %% Total    %% Received %% Xferd  Average Speed  "
                  "Time    Time    Time   Current\n"
                  "                                 Dload  Upload  "
                  "Total   Spent   Left   Speed\n");
    p->سرآیندها_بیرون = TRUE; /* headers are shown */
  }

  /* Figure out the estimated time of arrival for upload and download */
  pgrs_estimates(&p->ul, (bool)p->ul_اندازه_معلوم, &ul_estm);
  pgrs_estimates(&p->dl, (bool)p->dl_اندازه_معلوم, &dl_estm);

  /* Since both happen at the same time, total expected duration is max. */
  مجموع_estm.secs = CURLMAX(ul_estm.secs, dl_estm.secs);
  /* create the three time strings */
  time2str(زمان_چپ, حجم(زمان_چپ),
           مجموع_estm.secs > 0 ? (مجموع_estm.secs - cur_secs) : 0);
  time2str(زمان_مجموع, حجم(زمان_مجموع), مجموع_estm.secs);
  time2str(زمان_spent, حجم(زمان_spent), cur_secs);

  /* Get the total amount of data expected to get transferred */
  مجموع_expected_اندازه = p->ul_اندازه_معلوم ? p->ul.مجموع_اندازه : p->ul.cur_اندازه;

  dl_اندازه = p->dl_اندازه_معلوم ? p->dl.مجموع_اندازه : p->dl.cur_اندازه;

  /* integer overflow check */
  إذا((CURL_OFF_T_MAX - مجموع_expected_اندازه) < dl_اندازه)
    مجموع_expected_اندازه = CURL_OFF_T_MAX; /* capped */
  وإلا
    مجموع_expected_اندازه += dl_اندازه;

  /* We have transferred this much so far */
  مجموع_cur_اندازه = p->dl.cur_اندازه + p->ul.cur_اندازه;

  /* Get the percentage of data transferred so far */
  مجموع_estm.percent = pgrs_est_percent(مجموع_expected_اندازه, مجموع_cur_اندازه);

  curl_mfprintf(داده->مجموعه.err,
                "\r"
                "%3" FMT_OFF_T " %s "
                "%3" FMT_OFF_T " %s "
                "%3" FMT_OFF_T " %s %s %s %s %s %s %s",
                مجموع_estm.percent, /* 3 letters */    /* total % */
                max6out(مجموع_expected_اندازه, max6[2],
                        حجم(max6[2])),              /* total size */
                dl_estm.percent, /* 3 letters */       /* rcvd % */
                max6out(p->dl.cur_اندازه, max6[0],
                        حجم(max6[0])),              /* rcvd size */
                ul_estm.percent, /* 3 letters */       /* xfer % */
                max6out(p->ul.cur_اندازه, max6[1],
                        حجم(max6[1])),              /* xfer size */
                max6out(p->dl.سرعت, max6[3],
                        حجم(max6[3])),              /* avrg dl speed */
                max6out(p->ul.سرعت, max6[4],
                        حجم(max6[4])),              /* avrg ul speed */
                زمان_مجموع,    /* 7 letters */         /* total time */
                زمان_spent,    /* 7 letters */         /* time spent */
                زمان_چپ,     /* 7 letters */         /* time left */
                max6out(p->current_سرعت, max6[5],
                        حجم(max6[5]))               /* current speed */
    );

  /* we flush the output stream to make it appear as soon as possible */
  fflush(داده->مجموعه.err);
}
#وإلا /* CURL_DISABLE_PROGRESS_METER */
#تعریف_کن پیشرفت_meter(x) Curl_nop_stmt
#پایان_اگر

/*
 * Curl_pgrsUpdate() returns 0 for success or the value returned by the
 * progress callback!
 */
ساكن CURLcode pgrsupdate(بنية Curl_easy *داده, bool showprogress)
{
  إذا(!داده->پیشرفت.hide) {
    صحيح rc;
    إذا(داده->مجموعه.fxferinfo) {
      /* There is a callback set, call that */
      بنية Curl_mapi_guard guard;
      CURL_CBAPI_شروع(&guard, داده, easy_fxferinfo);
      rc = داده->مجموعه.fxferinfo(داده->مجموعه.پیشرفت_client,
                               داده->پیشرفت.dl.مجموع_اندازه,
                               داده->پیشرفت.dl.cur_اندازه,
                               داده->پیشرفت.ul.مجموع_اندازه,
                               داده->پیشرفت.ul.cur_اندازه);
      CURL_CBAPI_انتها(&guard);
      إذا(rc != CURL_PROGRESSFUNC_CONTINUE) {
        إذا(rc) {
          failf(داده, "Callback aborted");
          أرجع CURLE_ABORTED_BY_CALLBACK;
        }
        أرجع CURLE_OK;
      }
    }
    وإلا إذا(داده->مجموعه.fprogress) {
      /* The older deprecated callback is set, call that */
      بنية Curl_mapi_guard guard;
      CURL_CBAPI_شروع(&guard, داده, easy_fprogress);
      rc = داده->مجموعه.fprogress(داده->مجموعه.پیشرفت_client,
                               (مزدوج)داده->پیشرفت.dl.مجموع_اندازه,
                               (مزدوج)داده->پیشرفت.dl.cur_اندازه,
                               (مزدوج)داده->پیشرفت.ul.مجموع_اندازه,
                               (مزدوج)داده->پیشرفت.ul.cur_اندازه);
      CURL_CBAPI_انتها(&guard);
      إذا(rc != CURL_PROGRESSFUNC_CONTINUE) {
        إذا(rc) {
          failf(داده, "Callback aborted");
          أرجع CURLE_ABORTED_BY_CALLBACK;
        }
        أرجع CURLE_OK;
      }
    }

    إذا(showprogress)
      پیشرفت_meter(داده);
  }

  أرجع CURLE_OK;
}

ساكن CURLcode pgrs_update(بنية Curl_easy *داده,
                            ثابت بنية curltime *pnow)
{
  bool showprogress = پیشرفت_calc(داده, pnow);
  أرجع pgrsupdate(داده, showprogress);
}

CURLcode Curl_pgrsUpdate(بنية Curl_easy *داده)
{
  أرجع pgrs_update(داده, Curl_pgrs_now(داده));
}

CURLcode Curl_pgrsUpdateX(بنية Curl_easy *داده,
                          ثابت بنية curltime *pnow)
{
  أرجع pgrs_update(داده, pnow);
}

CURLcode Curl_pgrsCheckX(بنية Curl_easy *داده,
                         ثابت بنية curltime *pnow)
{
  CURLcode نتیجه;

  نتیجه = pgrs_update(داده, pnow);
  إذا(!نتیجه && !داده->req.انجامشده)
    نتیجه = pgrs_speedcheck(داده, pnow);
  أرجع نتیجه;
}

CURLcode Curl_pgrsCheck(بنية Curl_easy *داده)
{
  أرجع Curl_pgrsCheckX(داده, Curl_pgrs_now(داده));
}

/*
 * Update all progress, do not do progress meter/callbacks.
 */
فراغ Curl_pgrsUpdate_nometer(بنية Curl_easy *داده)
{
  (فراغ)پیشرفت_calc(داده, Curl_pgrs_now(داده));
}

فراغ Curl_pgrsCompleted(بنية Curl_easy *داده)
{
  بنية پیشرفت_2 * ثابت p = &داده->پیشرفت;
  p->مجموع.spent_us = curlx_ptimediff_us(Curl_pgrs_now(داده), &p->شروع);
}

timediff_t Curl_pgrs_since_ms(بنية Curl_easy *داده,
                              ثابت بنية curltime *pnow,
                              timerid timer)
{
  إذا(!pnow)
    pnow = Curl_pgrs_now(داده);
  بدل(timer) {
  حالة TIMER_STARTOP:
    أرجع (curlx_ptimediff_us(pnow, &داده->پیشرفت.شروع) -
            داده->پیشرفت.delta.startop_us) / 1000;
  حالة TIMER_STARTSINGLE:
    أرجع (curlx_ptimediff_us(pnow, &داده->پیشرفت.شروع) -
            داده->پیشرفت.delta.startsingle_us) / 1000;
  افتراضي:
    DEBUGASSERT(0);
    أرجع 0;
  }
}
