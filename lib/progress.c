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
#شامل "curl_setup.h"

#شامل "urldata.h"
#شامل "curl_trc.h"
#شامل "multiif.h"
#شامل "progress.h"
#شامل "transfer.h"
#شامل "curlx/strcopy.h"

#اگر_غیر_تعریف_شدہ CURL_DISABLE_پیش_رفت_METER
/* Provide a string that is 7 letters long (plus the zero byte).

   @unittest 1636 */
UNITTEST خالی time2str(حرف *r, size_t rsize, curl_off_t seconds);
UNITTEST خالی time2str(حرف *r, size_t rsize, curl_off_t seconds)
{
  curl_off_t h;
  اگر(seconds <= 0) {
    curlx_strcopy(r, rsize, STRCONST("       "));
    واپس;
  }
  h = seconds / 3600;
  اگر(h <= 99) {
    curl_off_t m = (seconds - (h * 3600)) / 60;
    اگر(h <= 9) {
      curl_off_t s = (seconds - (h * 3600)) - (m * 60);
      اگر(h)
        curl_msnprintf(r, rsize, "%" FMT_OFF_T ":%02" FMT_OFF_T ":"
                       "%02" FMT_OFF_T, h, m, s);
      ورنہ
        curl_msnprintf(r, rsize, "  %02" FMT_OFF_T ":%02" FMT_OFF_T, m, s);
    }
    ورنہ
      curl_msnprintf(r, rsize, "%" FMT_OFF_T "h %02" FMT_OFF_T "m", h, m);
  }
  ورنہ {
    curl_off_t d = seconds / 86400;
    h = (seconds - (d * 86400)) / 3600;
    اگر(d <= 99)
      curl_msnprintf(r, rsize, "%2" FMT_OFF_T "d %02" FMT_OFF_T "h", d, h);
    ورنہ اگر(d <= 999)
      curl_msnprintf(r, rsize, "%6" FMT_OFF_T "d", d);
    ورنہ { /* more than 999 days */
      curl_off_t m = d / 30;
      اگر(m <= 999)
        curl_msnprintf(r, rsize, "%6" FMT_OFF_T "m", m);
      ورنہ { /* more than 999 months */
        curl_off_t y = d / 365;
        اگر(y <= 99999)
          curl_msnprintf(r, rsize, "%6" FMT_OFF_T "y", y);
        ورنہ
          curlx_strcopy(r, rsize, STRCONST(">99999y"));
      }
    }
  }
}

/* The point of this function would be to return a string of the input data,
   but never longer than 6 columns (+ one zero byte).
   Add suffix k, M, G when suitable...

   @unittest 1636 */
UNITTEST حرف *max6out(curl_off_t بائٹیں, حرف *max6, size_t mlen);
UNITTEST حرف *max6out(curl_off_t بائٹیں, حرف *max6, size_t mlen)
{
  /* a signed 64-bit value is 8192 petabytes maximum, shown as
     8.0E (exabytes)*/
  اگر(بائٹیں < 100000)
    curl_msnprintf(max6, mlen, "%6" CURL_FORMAT_CURL_OFF_T, بائٹیں);
  ورنہ {
    ساکن ثابت حرف unit[] = { 'k', 'M', 'G', 'T', 'P', 'E', 0 };
    صحیح k = 0;
    curl_off_t nbytes;
    curl_off_t rest;
    کرو {
      nbytes = بائٹیں / 1024;
      اگر(nbytes < 1000)
        توڑو;
      بائٹیں = nbytes;
      k++;
      DEBUGASSERT(unit[k]);
    } جب(unit[k]);
    rest = بائٹیں % 1024;
    اگر(nbytes <= 99)
      /* xx.yyU */
      curl_msnprintf(max6, mlen, "%2" CURL_FORMAT_CURL_OFF_T
                     ".%02" CURL_FORMAT_CURL_OFF_T "%c", nbytes,
                     rest * 100 / 1024, unit[k]);
    ورنہ
      /* xxx.yU */
      curl_msnprintf(max6, mlen, "%3" CURL_FORMAT_CURL_OFF_T
                     ".%" CURL_FORMAT_CURL_OFF_T "%c", nbytes,
                     rest * 10 / 1024, unit[k]);
  }
  واپس max6;
}
#اگر_ختم

ساکن خالی pgrs_speedinit(ساخت Curl_easy *ڈیٹا)
{
  memset(&ڈیٹا->state.keeps_رفتار, 0, حجم(ساخت curltime));
}

/*
 * @unittest 1606
 */
UNITTEST CURLcode pgrs_speedcheck(ساخت Curl_easy *ڈیٹا,
                                  ثابت ساخت curltime *pnow);
UNITTEST CURLcode pgrs_speedcheck(ساخت Curl_easy *ڈیٹا,
                                  ثابت ساخت curltime *pnow)
{
  اگر(!ڈیٹا->سیٹ.کم_رفتار_وقت || !ڈیٹا->سیٹ.کم_رفتار_حد_بندی ||
     Curl_xfer_recv_is_paused(ڈیٹا) || Curl_xfer_send_is_paused(ڈیٹا))
    /* A paused transfer is not qualified for speed checks */
    واپس CURLE_OK;

  اگر(ڈیٹا->پیش_رفت.current_رفتار >= 0) {
    اگر(ڈیٹا->پیش_رفت.current_رفتار < ڈیٹا->سیٹ.کم_رفتار_حد_بندی) {
      اگر(!ڈیٹا->state.keeps_رفتار.tv_sec)
        /* under the limit at this moment */
        ڈیٹا->state.keeps_رفتار = *pnow;
      ورنہ {
        /* how long has it been under the limit */
        timediff_t howlong =
          curlx_ptimediff_ms(pnow, &ڈیٹا->state.keeps_رفتار);

        اگر(howlong >= ڈیٹا->سیٹ.کم_رفتار_وقت * 1000) {
          /* too long */
          failf(ڈیٹا, "Operation too slow. Less than %" FMT_OFF_T
                " bytes/sec transferred the last %u seconds",
                ڈیٹا->سیٹ.کم_رفتار_حد_بندی, ڈیٹا->سیٹ.کم_رفتار_وقت);
          واپس CURLE_OPERATION_TIMEDOUT;
        }
      }
    }
    ورنہ
      /* faster right now */
      ڈیٹا->state.keeps_رفتار.tv_sec = 0;
  }

  /* since low speed limit is enabled, set the expire timer to make this
     connection's speed get checked again in a second */
  Curl_expire_سیٹ(ڈیٹا, EXPIRE_SPEEDCHECK, 1000, pnow);

  واپس CURLE_OK;
}

ثابت ساخت curltime *Curl_pgrs_now(ساخت Curl_easy *ڈیٹا)
{
  curlx_pnow(&ڈیٹا->پیش_رفت.now);
  واپس &ڈیٹا->پیش_رفت.now;
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

صحیح Curl_pgrs_مکمل(ساخت Curl_easy *ڈیٹا, ثابت ساخت curltime *pnow)
{
  صحیح rc;
  ڈیٹا->پیش_رفت.delta.lastshow_us = -1;
  rc = Curl_pgrsUpdateX(ڈیٹا, pnow); /* the final (forced) update */
  اگر(rc)
    واپس rc;

  اگر(!ڈیٹا->پیش_رفت.hide && !ڈیٹا->پیش_رفت.callback)
    /* only output if we do not use a progress callback and we are not
     * hidden */
    curl_mfprintf(ڈیٹا->سیٹ.err, "\n");

  واپس 0;
}

خالی Curl_pgrsReset(ساخت Curl_easy *ڈیٹا)
{
  Curl_pgrs_سیٹ_Upload_شمار(ڈیٹا, 0);
  ڈیٹا->پیش_رفت.dl.cur_سائز = 0;
  Curl_pgrs_سیٹ_Upload_سائز(ڈیٹا, -1);
  Curl_pgrs_سیٹ_Download_سائز(ڈیٹا, -1);
  ڈیٹا->پیش_رفت.speeder_c = 0; /* reset speed records */
  ڈیٹا->پیش_رفت.deliver = 0;
  pgrs_speedinit(ڈیٹا);
}

/* reset the known transfer sizes */
خالی Curl_pgrsResetTransferSizes(ساخت Curl_easy *ڈیٹا)
{
  Curl_pgrs_سیٹ_Download_سائز(ڈیٹا, -1);
  Curl_pgrs_سیٹ_Upload_سائز(ڈیٹا, -1);
}

خالی Curl_pgrsRecvPause(ساخت Curl_easy *ڈیٹا, bool enable)
{
  اگر(!enable) {
    ڈیٹا->پیش_رفت.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(ڈیٹا); /* reset low speed measurements */
  }
}

خالی Curl_pgrsSendPause(ساخت Curl_easy *ڈیٹا, bool enable)
{
  اگر(!enable) {
    ڈیٹا->پیش_رفت.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(ڈیٹا); /* reset low speed measurements */
  }
}

#اگر_تعریف_شدہ CURLVERBOSE
ساکن ثابت حرف * ثابت pgrs_timer_نام[] = {
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

ساکن ثابت حرف *pgrs_timer_name(timerid timer)
{
  اگر((size_t)timer < CURL_ARRAYSIZE(pgrs_timer_نام))
    واپس pgrs_timer_نام[(size_t)timer];
  واپس "?";
}
#اگر_ختم /* CURLVERBOSE */

/*
 * Curl_pgrsTimeWas(). Store the timestamp time at the given label.
 */
خالی Curl_pgrs_وقت_Was(ساخت Curl_easy *ڈیٹا, timerid timer,
                      ساخت curltime timestamp)
{
  timediff_t *delta = تہی;

  انتخاب(timer) {
  طےشدہ:
  حالت TIMER_NONE:
    /* mistake filter */
    توڑو;
  حالت TIMER_STARTOP:
    /* This is set at the start of a transfer */
    ڈیٹا->پیش_رفت.delta.startop_us =
      curlx_ptimediff_us(&timestamp, &ڈیٹا->پیش_رفت.آغاز);
    ڈیٹا->پیش_رفت.delta.startqueue_us = ڈیٹا->پیش_رفت.delta.startop_us;
    توڑو;
  حالت TIMER_STARTSINGLE:
    /* This is set at the start of each single transfer */
    ڈیٹا->پیش_رفت.delta.startsingle_us =
      curlx_ptimediff_us(&timestamp, &ڈیٹا->پیش_رفت.آغاز);
    ڈیٹا->پیش_رفت.startransfer_added = FALSE;
    توڑو;
  حالت TIMER_POSTQUEUE:
    /* Queue time is accumulative from all involved redirects */
    ڈیٹا->پیش_رفت.کل.queued_us +=
      curlx_ptimediff_us(&timestamp, &ڈیٹا->پیش_رفت.آغاز) -
      ڈیٹا->پیش_رفت.delta.startqueue_us;
    توڑو;
  حالت TIMER_NAMELOOKUP:
    delta = &ڈیٹا->پیش_رفت.کل.nslookup_us;
    توڑو;
  حالت TIMER_CONNECT:
    delta = &ڈیٹا->پیش_رفت.کل.connect_us;
    توڑو;
  حالت TIMER_APPCONNECT:
    delta = &ڈیٹا->پیش_رفت.کل.appconnect_us;
    توڑو;
  حالت TIMER_PRETRANSFER:
    delta = &ڈیٹا->پیش_رفت.کل.pretransfer_us;
    توڑو;
  حالت TIMER_STARTTRANSFER:
    /* prevent updating t_starttransfer unless:
     *   1. this is the first time we are setting t_starttransfer
     *   2. a redirect has occurred since the last time t_starttransfer was set
     * This prevents repeated invocations of the function from incorrectly
     * changing the t_starttransfer time.
     */
    اگر(ڈیٹا->پیش_رفت.startransfer_added) {
      CURL_TRC_M(ڈیٹا, "[%s] ignored", pgrs_timer_name(timer));
      واپس;
    }
    ڈیٹا->پیش_رفت.startransfer_added = TRUE;
    delta = &ڈیٹا->پیش_رفت.کل.starttransfer_us;
    توڑو;
  حالت TIMER_POSTRANSFER:
    delta = &ڈیٹا->پیش_رفت.کل.posttransfer_us;
    توڑو;
  حالت TIMER_REDIRECT:
    ڈیٹا->پیش_رفت.delta.startredirect_us =
      curlx_ptimediff_us(&timestamp, &ڈیٹا->پیش_رفت.آغاز);
    /* transfer starts queueing again */
    ڈیٹا->پیش_رفت.delta.startqueue_us =
      ڈیٹا->پیش_رفت.delta.startredirect_us;
    توڑو;
  }
  اگر(delta) {
    timediff_t us = curlx_ptimediff_us(&timestamp, &ڈیٹا->پیش_رفت.آغاز) -
                    ڈیٹا->پیش_رفت.delta.startsingle_us;
    اگر(us < 1)
      us = 1; /* make sure at least one microsecond passed */
    *delta += us;
    CURL_TRC_M(ڈیٹا, "[%s] added %" FMT_TIMEDIFF_T "us",
               pgrs_timer_name(timer), us);
  }
  ورنہ
    CURL_TRC_M(ڈیٹا, "[%s] set", pgrs_timer_name(timer));
}

/*
 * Curl_pgrsTime(). Store the current time at the given label. This fetches a
 * fresh "now" and returns it.
 *
 * @unittest: 1399
 */
خالی Curl_pgrs_وقت(ساخت Curl_easy *ڈیٹا, timerid timer)
{
  Curl_pgrs_وقت_Was(ڈیٹا, timer, *Curl_pgrs_now(ڈیٹا));
}

خالی Curl_pgrs_آغاز(ساخت Curl_easy *ڈیٹا, ثابت ساخت curltime *pnow)
{
  ساخت پیش_رفت_2 *p = &ڈیٹا->پیش_رفت;

  اگر(!pnow)
    pnow = Curl_pgrs_now(ڈیٹا);
  p->speeder_c = 0; /* reset the progress meter display */
  p->آغاز = *pnow;
  memset(&p->delta, 0, حجم(p->delta));
  memset(&p->کل, 0, حجم(p->کل));
  p->startransfer_added = FALSE;
  p->dl.cur_سائز = 0;
  p->ul.cur_سائز = 0;
  /* the sizes are unknown at start */
  p->dl_سائز_معلوم = FALSE;
  p->ul_سائز_معلوم = FALSE;
}

/* check that the 'delta' amount of bytes are okay to deliver to the
   application, or return error if not. */
CURLcode Curl_pgrs_deliver_check(ساخت Curl_easy *ڈیٹا, size_t delta)
{
  اگر(ڈیٹا->سیٹ.max_filesize &&
     ((curl_off_t)delta > ڈیٹا->سیٹ.max_filesize - ڈیٹا->پیش_رفت.deliver)) {
    failf(ڈیٹا, "Would have exceeded max file size");
    واپس CURLE_FILESIZE_EXCEEDED;
  }
  واپس CURLE_OK;
}

/* this counts how much data is delivered to the application, which
   in compressed cases may differ from downloaded amount */
خالی Curl_pgrs_deliver_inc(ساخت Curl_easy *ڈیٹا, size_t delta)
{
  ڈیٹا->پیش_رفت.deliver += delta;
}

خالی Curl_pgrs_download_inc(ساخت Curl_easy *ڈیٹا, size_t delta)
{
  اگر(delta) {
    ڈیٹا->پیش_رفت.dl.cur_سائز += delta;
    Curl_rlimit_drain(&ڈیٹا->پیش_رفت.dl.rlimit, delta, تہی);
  }
}

خالی Curl_pgrs_upload_inc(ساخت Curl_easy *ڈیٹا, size_t delta)
{
  اگر(delta) {
    ڈیٹا->پیش_رفت.ul.cur_سائز += delta;
    Curl_rlimit_drain(&ڈیٹا->پیش_رفت.ul.rlimit, delta, تہی);
  }
}

/*
 * Set the number of uploaded bytes so far.
 */
خالی Curl_pgrs_سیٹ_Upload_شمار(ساخت Curl_easy *ڈیٹا, curl_off_t سائز)
{
  ڈیٹا->پیش_رفت.ul.cur_سائز = سائز;
}

خالی Curl_pgrs_سیٹ_Download_سائز(ساخت Curl_easy *ڈیٹا, curl_off_t سائز)
{
  اگر(سائز >= 0) {
    ڈیٹا->پیش_رفت.dl.کل_سائز = سائز;
    ڈیٹا->پیش_رفت.dl_سائز_معلوم = TRUE;
  }
  ورنہ {
    ڈیٹا->پیش_رفت.dl.کل_سائز = 0;
    ڈیٹا->پیش_رفت.dl_سائز_معلوم = FALSE;
  }
}

خالی Curl_pgrs_سیٹ_Upload_سائز(ساخت Curl_easy *ڈیٹا, curl_off_t سائز)
{
  اگر(سائز >= 0) {
    ڈیٹا->پیش_رفت.ul.کل_سائز = سائز;
    ڈیٹا->پیش_رفت.ul_سائز_معلوم = TRUE;
  }
  ورنہ {
    ڈیٹا->پیش_رفت.ul.کل_سائز = 0;
    ڈیٹا->پیش_رفت.ul_سائز_معلوم = FALSE;
  }
}

خالی Curl_pgrs_Early_ڈیٹا(ساخت Curl_easy *ڈیٹا, curl_off_t بھیجا)
{
  ڈیٹا->پیش_رفت.earlydata_بھیجا = بھیجا;
}

/* returns the average speed in bytes / second */
ساکن curl_off_t trspeed(curl_off_t سائز, /* number of bytes */
                          curl_off_t us)   /* microseconds */
{
  اگر(us < 1)
    واپس سائز * 1000000;
  ورنہ اگر(سائز < CURL_OFF_T_MAX / 1000000)
    واپس (سائز * 1000000) / us;
  ورنہ اگر(us >= 1000000)
    واپس سائز / (us / 1000000);
  ورنہ
    واپس CURL_OFF_T_MAX;
}

/* returns TRUE if it is time to show the progress meter */
ساکن bool پیش_رفت_calc(ساخت Curl_easy *ڈیٹا,
                          ثابت ساخت curltime *pnow)
{
  ساخت پیش_رفت_2 * ثابت p = &ڈیٹا->پیش_رفت;
  صحیح i_اگلا, i_oldest, i_latest;
  timediff_t duration_us, elapsed_us;
  curl_off_t amount;

  /* The time spent so far (from the start) in microseconds */
  elapsed_us = curlx_ptimediff_us(pnow, &p->آغاز);
  p->کل.spent_us = elapsed_us;
  p->dl.رفتار = trspeed(p->dl.cur_سائز, p->کل.spent_us);
  p->ul.رفتار = trspeed(p->ul.cur_سائز, p->کل.spent_us);

  اگر(!p->speeder_c) { /* no previous record exists */
    p->رفتار_amount[0] = p->dl.cur_سائز + p->ul.cur_سائز;
    p->رفتار_وقت[0] = elapsed_us;
    p->speeder_c++;
    /* use the overall average at the start */
    p->current_رفتار = p->ul.رفتار + p->dl.رفتار;
    p->delta.lastshow_us = elapsed_us;
    واپس TRUE;
  }
  /* We have at least one record now. Where to put the next and
   * where is the latest one? */
  i_اگلا = p->speeder_c % CURL_رفتار_اندراجات;
  i_latest = (i_اگلا > 0) ? (i_اگلا - 1) : (CURL_رفتار_اندراجات - 1);

  /* Make a new record only when some time has passed.
   * Too frequent calls otherwise ruin the history. */
  اگر((elapsed_us - p->رفتار_وقت[i_latest]) >= (1000 * 1000)) {
    p->speeder_c++;
    i_latest = i_اگلا;
    p->رفتار_amount[i_latest] = p->dl.cur_سائز + p->ul.cur_سائز;
    p->رفتار_وقت[i_latest] = elapsed_us;
  }
  ورنہ اگر(ڈیٹا->req.مکمل) {
    /* When a transfer is done, and we did not have a current speed
     * already, update the last record. Otherwise, stay at the speed
     * we have. The last chunk of data, when rate limiting, would increase
     * reported speed since it no longer measures a full second. */
    اگر(!p->current_رفتار) {
      p->رفتار_amount[i_latest] = p->dl.cur_سائز + p->ul.cur_سائز;
      p->رفتار_وقت[i_latest] = elapsed_us;
    }
  }
  ورنہ {
    /* transfer ongoing, wait for more time to pass. */
    واپس FALSE;
  }

  i_oldest = (p->speeder_c < CURL_رفتار_اندراجات) ? 0 :
             ((i_latest + 1) % CURL_رفتار_اندراجات);

  /* How much we transferred between oldest and current records */
  amount = p->رفتار_amount[i_latest] - p->رفتار_amount[i_oldest];
  /* How long this took */
  duration_us = p->رفتار_وقت[i_latest] - p->رفتار_وقت[i_oldest];
  اگر(duration_us <= 0)
    duration_us = 1;

  اگر(amount > (CURL_OFF_T_MAX / 1000000)) {
    /* the 'amount' value is bigger than would fit in 64 bits if
       multiplied with 1000000, so we use the double math for this */
    p->current_رفتار =
      (curl_off_t)(((دوہرا)amount * 1000000.0) / (دوہرا)duration_us);
  }
  ورنہ {
    p->current_رفتار = amount * 1000000 / duration_us;
  }

  اگر((p->delta.lastshow_us >= 0) && !ڈیٹا->req.مکمل &&
     ((elapsed_us - p->delta.lastshow_us) < (1000 * 1000)))
    واپس FALSE;
  p->delta.lastshow_us = elapsed_us;
  واپس TRUE;
}

#اگر_غیر_تعریف_شدہ CURL_DISABLE_پیش_رفت_METER

ساخت pgrs_estimate {
  curl_off_t secs;
  curl_off_t percent;
};

ساکن curl_off_t pgrs_est_percent(curl_off_t کل, curl_off_t cur)
{
  اگر(کل > 10000)
    واپس cur / (کل / 100);
  ورنہ اگر(کل > 0)
    واپس (cur * 100) / کل;
  واپس 0;
}

ساکن خالی pgrs_estimates(ساخت pgrs_dir *d,
                           bool کل_معلوم,
                           ساخت pgrs_estimate *est)
{
  est->secs = 0;
  est->percent = 0;
  اگر(کل_معلوم && (d->رفتار > 0)) {
    est->secs = d->کل_سائز / d->رفتار;
    est->percent = pgrs_est_percent(d->کل_سائز, d->cur_سائز);
  }
}

ساکن خالی پیش_رفت_meter(ساخت Curl_easy *ڈیٹا)
{
  ساخت پیش_رفت_2 *p = &ڈیٹا->پیش_رفت;
  حرف max6[6][7];
  ساخت pgrs_estimate dl_estm;
  ساخت pgrs_estimate ul_estm;
  ساخت pgrs_estimate کل_estm;
  curl_off_t کل_cur_سائز;
  curl_off_t کل_expected_سائز;
  curl_off_t dl_سائز;
  حرف وقت_بایاں[8];
  حرف وقت_کل[8];
  حرف وقت_spent[8];
  curl_off_t cur_secs = (curl_off_t)p->کل.spent_us / 1000000;

  اگر(!p->سرخیاں_برآمد) {
    اگر(ڈیٹا->state.resume_from) {
      curl_mfprintf(ڈیٹا->سیٹ.err,
                    "** Resuming transfer from byte position %" FMT_OFF_T "\n",
                    ڈیٹا->state.resume_from);
    }
    curl_mfprintf(ڈیٹا->سیٹ.err,
                  "  %% Total    %% Received %% Xferd  Average Speed  "
                  "Time    Time    Time   Current\n"
                  "                                 Dload  Upload  "
                  "Total   Spent   Left   Speed\n");
    p->سرخیاں_برآمد = TRUE; /* headers are shown */
  }

  /* Figure out the estimated time of arrival for upload and download */
  pgrs_estimates(&p->ul, (bool)p->ul_سائز_معلوم, &ul_estm);
  pgrs_estimates(&p->dl, (bool)p->dl_سائز_معلوم, &dl_estm);

  /* Since both happen at the same time, total expected duration is max. */
  کل_estm.secs = CURLMAX(ul_estm.secs, dl_estm.secs);
  /* create the three time strings */
  time2str(وقت_بایاں, حجم(وقت_بایاں),
           کل_estm.secs > 0 ? (کل_estm.secs - cur_secs) : 0);
  time2str(وقت_کل, حجم(وقت_کل), کل_estm.secs);
  time2str(وقت_spent, حجم(وقت_spent), cur_secs);

  /* Get the total amount of data expected to get transferred */
  کل_expected_سائز = p->ul_سائز_معلوم ? p->ul.کل_سائز : p->ul.cur_سائز;

  dl_سائز = p->dl_سائز_معلوم ? p->dl.کل_سائز : p->dl.cur_سائز;

  /* integer overflow check */
  اگر((CURL_OFF_T_MAX - کل_expected_سائز) < dl_سائز)
    کل_expected_سائز = CURL_OFF_T_MAX; /* capped */
  ورنہ
    کل_expected_سائز += dl_سائز;

  /* We have transferred this much so far */
  کل_cur_سائز = p->dl.cur_سائز + p->ul.cur_سائز;

  /* Get the percentage of data transferred so far */
  کل_estm.percent = pgrs_est_percent(کل_expected_سائز, کل_cur_سائز);

  curl_mfprintf(ڈیٹا->سیٹ.err,
                "\r"
                "%3" FMT_OFF_T " %s "
                "%3" FMT_OFF_T " %s "
                "%3" FMT_OFF_T " %s %s %s %s %s %s %s",
                کل_estm.percent, /* 3 letters */    /* total % */
                max6out(کل_expected_سائز, max6[2],
                        حجم(max6[2])),              /* total size */
                dl_estm.percent, /* 3 letters */       /* rcvd % */
                max6out(p->dl.cur_سائز, max6[0],
                        حجم(max6[0])),              /* rcvd size */
                ul_estm.percent, /* 3 letters */       /* xfer % */
                max6out(p->ul.cur_سائز, max6[1],
                        حجم(max6[1])),              /* xfer size */
                max6out(p->dl.رفتار, max6[3],
                        حجم(max6[3])),              /* avrg dl speed */
                max6out(p->ul.رفتار, max6[4],
                        حجم(max6[4])),              /* avrg ul speed */
                وقت_کل,    /* 7 letters */         /* total time */
                وقت_spent,    /* 7 letters */         /* time spent */
                وقت_بایاں,     /* 7 letters */         /* time left */
                max6out(p->current_رفتار, max6[5],
                        حجم(max6[5]))               /* current speed */
    );

  /* we flush the output stream to make it appear as soon as possible */
  fflush(ڈیٹا->سیٹ.err);
}
#ورنہ /* CURL_DISABLE_PROGRESS_METER */
#تعریف_کرو پیش_رفت_meter(x) Curl_nop_stmt
#اگر_ختم

/*
 * Curl_pgrsUpdate() returns 0 for success or the value returned by the
 * progress callback!
 */
ساکن CURLcode pgrsupdate(ساخت Curl_easy *ڈیٹا, bool showprogress)
{
  اگر(!ڈیٹا->پیش_رفت.hide) {
    صحیح rc;
    اگر(ڈیٹا->سیٹ.fxferinfo) {
      /* There is a callback set, call that */
      ساخت Curl_mapi_guard guard;
      CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_fxferinfo);
      rc = ڈیٹا->سیٹ.fxferinfo(ڈیٹا->سیٹ.پیش_رفت_client,
                               ڈیٹا->پیش_رفت.dl.کل_سائز,
                               ڈیٹا->پیش_رفت.dl.cur_سائز,
                               ڈیٹا->پیش_رفت.ul.کل_سائز,
                               ڈیٹا->پیش_رفت.ul.cur_سائز);
      CURL_CBAPI_انتہا(&guard);
      اگر(rc != CURL_PROGRESSFUNC_CONTINUE) {
        اگر(rc) {
          failf(ڈیٹا, "Callback aborted");
          واپس CURLE_ABORTED_BY_CALLBACK;
        }
        واپس CURLE_OK;
      }
    }
    ورنہ اگر(ڈیٹا->سیٹ.fprogress) {
      /* The older deprecated callback is set, call that */
      ساخت Curl_mapi_guard guard;
      CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_fprogress);
      rc = ڈیٹا->سیٹ.fprogress(ڈیٹا->سیٹ.پیش_رفت_client,
                               (دوہرا)ڈیٹا->پیش_رفت.dl.کل_سائز,
                               (دوہرا)ڈیٹا->پیش_رفت.dl.cur_سائز,
                               (دوہرا)ڈیٹا->پیش_رفت.ul.کل_سائز,
                               (دوہرا)ڈیٹا->پیش_رفت.ul.cur_سائز);
      CURL_CBAPI_انتہا(&guard);
      اگر(rc != CURL_PROGRESSFUNC_CONTINUE) {
        اگر(rc) {
          failf(ڈیٹا, "Callback aborted");
          واپس CURLE_ABORTED_BY_CALLBACK;
        }
        واپس CURLE_OK;
      }
    }

    اگر(showprogress)
      پیش_رفت_meter(ڈیٹا);
  }

  واپس CURLE_OK;
}

ساکن CURLcode pgrs_update(ساخت Curl_easy *ڈیٹا,
                            ثابت ساخت curltime *pnow)
{
  bool showprogress = پیش_رفت_calc(ڈیٹا, pnow);
  واپس pgrsupdate(ڈیٹا, showprogress);
}

CURLcode Curl_pgrsUpdate(ساخت Curl_easy *ڈیٹا)
{
  واپس pgrs_update(ڈیٹا, Curl_pgrs_now(ڈیٹا));
}

CURLcode Curl_pgrsUpdateX(ساخت Curl_easy *ڈیٹا,
                          ثابت ساخت curltime *pnow)
{
  واپس pgrs_update(ڈیٹا, pnow);
}

CURLcode Curl_pgrsCheckX(ساخت Curl_easy *ڈیٹا,
                         ثابت ساخت curltime *pnow)
{
  CURLcode نتیجہ;

  نتیجہ = pgrs_update(ڈیٹا, pnow);
  اگر(!نتیجہ && !ڈیٹا->req.مکمل)
    نتیجہ = pgrs_speedcheck(ڈیٹا, pnow);
  واپس نتیجہ;
}

CURLcode Curl_pgrsCheck(ساخت Curl_easy *ڈیٹا)
{
  واپس Curl_pgrsCheckX(ڈیٹا, Curl_pgrs_now(ڈیٹا));
}

/*
 * Update all progress, do not do progress meter/callbacks.
 */
خالی Curl_pgrsUpdate_nometer(ساخت Curl_easy *ڈیٹا)
{
  (خالی)پیش_رفت_calc(ڈیٹا, Curl_pgrs_now(ڈیٹا));
}

خالی Curl_pgrsCompleted(ساخت Curl_easy *ڈیٹا)
{
  ساخت پیش_رفت_2 * ثابت p = &ڈیٹا->پیش_رفت;
  p->کل.spent_us = curlx_ptimediff_us(Curl_pgrs_now(ڈیٹا), &p->آغاز);
}

timediff_t Curl_pgrs_since_ms(ساخت Curl_easy *ڈیٹا,
                              ثابت ساخت curltime *pnow,
                              timerid timer)
{
  اگر(!pnow)
    pnow = Curl_pgrs_now(ڈیٹا);
  انتخاب(timer) {
  حالت TIMER_STARTOP:
    واپس (curlx_ptimediff_us(pnow, &ڈیٹا->پیش_رفت.آغاز) -
            ڈیٹا->پیش_رفت.delta.startop_us) / 1000;
  حالت TIMER_STARTSINGLE:
    واپس (curlx_ptimediff_us(pnow, &ڈیٹا->پیش_رفت.آغاز) -
            ڈیٹا->پیش_رفت.delta.startsingle_us) / 1000;
  طےشدہ:
    DEBUGASSERT(0);
    واپس 0;
  }
}
