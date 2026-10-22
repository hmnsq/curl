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

#ifndef CURL_DISABLE_PROGRESS_METER
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
UNITTEST حرف *max6out(curl_off_t bytes, حرف *max6, size_t mlen);
UNITTEST حرف *max6out(curl_off_t bytes, حرف *max6, size_t mlen)
{
  /* a signed 64-bit value is 8192 petabytes maximum, shown as
     8.0E (exabytes)*/
  إذا(bytes < 100000)
    curl_msnprintf(max6, mlen, "%6" CURL_FORMAT_CURL_OFF_T, bytes);
  وإلا {
    ساكن ثابت حرف unit[] = { 'k', 'M', 'G', 'T', 'P', 'E', 0 };
    صحيح k = 0;
    curl_off_t nbytes;
    curl_off_t rest;
    افعل {
      nbytes = bytes / 1024;
      إذا(nbytes < 1000)
        اكسر;
      bytes = nbytes;
      k++;
      DEBUGASSERT(unit[k]);
    } طالما(unit[k]);
    rest = bytes % 1024;
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
#endif

ساكن فراغ pgrs_speedinit(بنية Curl_easy *data)
{
  memset(&data->state.keeps_speed, 0, حجم(بنية curltime));
}

/*
 * @unittest 1606
 */
UNITTEST CURLcode pgrs_speedcheck(بنية Curl_easy *data,
                                  ثابت بنية curltime *pnow);
UNITTEST CURLcode pgrs_speedcheck(بنية Curl_easy *data,
                                  ثابت بنية curltime *pnow)
{
  إذا(!data->set.رنده_speed_time || !data->set.رنده_speed_limit ||
     Curl_xfer_recv_is_paused(data) || Curl_xfer_send_is_paused(data))
    /* A paused transfer is not qualified for speed checks */
    أرجع CURLE_OK;

  إذا(data->progress.current_speed >= 0) {
    إذا(data->progress.current_speed < data->set.رنده_speed_limit) {
      إذا(!data->state.keeps_speed.tv_sec)
        /* under the limit at this moment */
        data->state.keeps_speed = *pnow;
      وإلا {
        /* how long has it been under the limit */
        timediff_t howlong =
          curlx_ptimediff_ms(pnow, &data->state.keeps_speed);

        إذا(howlong >= data->set.رنده_speed_time * 1000) {
          /* too long */
          failf(data, "Operation too slow. Less than %" FMT_OFF_T
                " bytes/sec transferred the last %u seconds",
                data->set.رنده_speed_limit, data->set.رنده_speed_time);
          أرجع CURLE_OPERATION_TIMEDOUT;
        }
      }
    }
    وإلا
      /* faster right now */
      data->state.keeps_speed.tv_sec = 0;
  }

  /* since low speed limit is enabled, set the expire timer to make this
     connection's speed get checked again in a second */
  Curl_expire_set(data, EXPIRE_SPEEDCHECK, 1000, pnow);

  أرجع CURLE_OK;
}

ثابت بنية curltime *Curl_pgrs_now(بنية Curl_easy *data)
{
  curlx_pnow(&data->progress.now);
  أرجع &data->progress.now;
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

صحيح Curl_pgrsDone(بنية Curl_easy *data, ثابت بنية curltime *pnow)
{
  صحيح rc;
  data->progress.delta.lastshow_us = -1;
  rc = Curl_pgrsUpdateX(data, pnow); /* the final (forced) update */
  إذا(rc)
    أرجع rc;

  إذا(!data->progress.hide && !data->progress.callback)
    /* only output if we do not use a progress callback and we are not
     * hidden */
    curl_mfprintf(data->set.err, "\n");

  أرجع 0;
}

فراغ Curl_pgrsReset(بنية Curl_easy *data)
{
  Curl_pgrsSetUploadCounter(data, 0);
  data->progress.dl.cur_size = 0;
  Curl_pgrsSetUploadSize(data, -1);
  Curl_pgrsSetDownloadSize(data, -1);
  data->progress.speeder_c = 0; /* reset speed records */
  data->progress.deliver = 0;
  pgrs_speedinit(data);
}

/* reset the known transfer sizes */
فراغ Curl_pgrsResetTransferSizes(بنية Curl_easy *data)
{
  Curl_pgrsSetDownloadSize(data, -1);
  Curl_pgrsSetUploadSize(data, -1);
}

فراغ Curl_pgrsRecvPause(بنية Curl_easy *data, bool enable)
{
  إذا(!enable) {
    data->progress.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(data); /* reset low speed measurements */
  }
}

فراغ Curl_pgrsSendPause(بنية Curl_easy *data, bool enable)
{
  إذا(!enable) {
    data->progress.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(data); /* reset low speed measurements */
  }
}

#ifdef CURLVERBOSE
ساكن ثابت حرف * ثابت pgrs_timer_names[] = {
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
  إذا((size_t)timer < CURL_ARRAYSIZE(pgrs_timer_names))
    أرجع pgrs_timer_names[(size_t)timer];
  أرجع "?";
}
#endif /* CURLVERBOSE */

/*
 * Curl_pgrsTimeWas(). Store the timestamp time at the given label.
 */
فراغ Curl_pgrsTimeWas(بنية Curl_easy *data, timerid timer,
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
    data->progress.delta.startop_us =
      curlx_ptimediff_us(&timestamp, &data->progress.start);
    data->progress.delta.startqueue_us = data->progress.delta.startop_us;
    اكسر;
  حالة TIMER_STARTSINGLE:
    /* This is set at the start of each single transfer */
    data->progress.delta.startsingle_us =
      curlx_ptimediff_us(&timestamp, &data->progress.start);
    data->progress.startransfer_added = FALSE;
    اكسر;
  حالة TIMER_POSTQUEUE:
    /* Queue time is accumulative from all involved redirects */
    data->progress.total.queued_us +=
      curlx_ptimediff_us(&timestamp, &data->progress.start) -
      data->progress.delta.startqueue_us;
    اكسر;
  حالة TIMER_NAMELOOKUP:
    delta = &data->progress.total.nslookup_us;
    اكسر;
  حالة TIMER_CONNECT:
    delta = &data->progress.total.connect_us;
    اكسر;
  حالة TIMER_APPCONNECT:
    delta = &data->progress.total.appconnect_us;
    اكسر;
  حالة TIMER_PRETRANSFER:
    delta = &data->progress.total.pretransfer_us;
    اكسر;
  حالة TIMER_STARTTRANSFER:
    /* prevent updating t_starttransfer unless:
     *   1. this is the first time we are setting t_starttransfer
     *   2. a redirect has occurred since the last time t_starttransfer was set
     * This prevents repeated invocations of the function from incorrectly
     * changing the t_starttransfer time.
     */
    إذا(data->progress.startransfer_added) {
      CURL_TRC_M(data, "[%s] ignored", pgrs_timer_name(timer));
      أرجع;
    }
    data->progress.startransfer_added = TRUE;
    delta = &data->progress.total.starttransfer_us;
    اكسر;
  حالة TIMER_POSTRANSFER:
    delta = &data->progress.total.posttransfer_us;
    اكسر;
  حالة TIMER_REDIRECT:
    data->progress.delta.startredirect_us =
      curlx_ptimediff_us(&timestamp, &data->progress.start);
    /* transfer starts queueing again */
    data->progress.delta.startqueue_us =
      data->progress.delta.startredirect_us;
    اكسر;
  }
  إذا(delta) {
    timediff_t us = curlx_ptimediff_us(&timestamp, &data->progress.start) -
                    data->progress.delta.startsingle_us;
    إذا(us < 1)
      us = 1; /* make sure at least one microsecond passed */
    *delta += us;
    CURL_TRC_M(data, "[%s] added %" FMT_TIMEDIFF_T "us",
               pgrs_timer_name(timer), us);
  }
  وإلا
    CURL_TRC_M(data, "[%s] set", pgrs_timer_name(timer));
}

/*
 * Curl_pgrsTime(). Store the current time at the given label. This fetches a
 * fresh "now" and returns it.
 *
 * @unittest: 1399
 */
فراغ Curl_pgrsTime(بنية Curl_easy *data, timerid timer)
{
  Curl_pgrsTimeWas(data, timer, *Curl_pgrs_now(data));
}

فراغ Curl_pgrsStart(بنية Curl_easy *data, ثابت بنية curltime *pnow)
{
  بنية Progress *p = &data->progress;

  إذا(!pnow)
    pnow = Curl_pgrs_now(data);
  p->speeder_c = 0; /* reset the progress meter display */
  p->start = *pnow;
  memset(&p->delta, 0, حجم(p->delta));
  memset(&p->total, 0, حجم(p->total));
  p->startransfer_added = FALSE;
  p->dl.cur_size = 0;
  p->ul.cur_size = 0;
  /* the sizes are unknown at start */
  p->dl_size_known = FALSE;
  p->ul_size_known = FALSE;
}

/* check that the 'delta' amount of bytes are okay to deliver to the
   application, or return error if not. */
CURLcode Curl_pgrs_deliver_check(بنية Curl_easy *data, size_t delta)
{
  إذا(data->set.max_filesize &&
     ((curl_off_t)delta > data->set.max_filesize - data->progress.deliver)) {
    failf(data, "Would have exceeded max file size");
    أرجع CURLE_FILESIZE_EXCEEDED;
  }
  أرجع CURLE_OK;
}

/* this counts how much data is delivered to the application, which
   in compressed cases may differ from downloaded amount */
فراغ Curl_pgrs_deliver_inc(بنية Curl_easy *data, size_t delta)
{
  data->progress.deliver += delta;
}

فراغ Curl_pgrs_download_inc(بنية Curl_easy *data, size_t delta)
{
  إذا(delta) {
    data->progress.dl.cur_size += delta;
    Curl_rlimit_drain(&data->progress.dl.rlimit, delta, عدم);
  }
}

فراغ Curl_pgrs_upload_inc(بنية Curl_easy *data, size_t delta)
{
  إذا(delta) {
    data->progress.ul.cur_size += delta;
    Curl_rlimit_drain(&data->progress.ul.rlimit, delta, عدم);
  }
}

/*
 * Set the number of uploaded bytes so far.
 */
فراغ Curl_pgrsSetUploadCounter(بنية Curl_easy *data, curl_off_t size)
{
  data->progress.ul.cur_size = size;
}

فراغ Curl_pgrsSetDownloadSize(بنية Curl_easy *data, curl_off_t size)
{
  إذا(size >= 0) {
    data->progress.dl.total_size = size;
    data->progress.dl_size_known = TRUE;
  }
  وإلا {
    data->progress.dl.total_size = 0;
    data->progress.dl_size_known = FALSE;
  }
}

فراغ Curl_pgrsSetUploadSize(بنية Curl_easy *data, curl_off_t size)
{
  إذا(size >= 0) {
    data->progress.ul.total_size = size;
    data->progress.ul_size_known = TRUE;
  }
  وإلا {
    data->progress.ul.total_size = 0;
    data->progress.ul_size_known = FALSE;
  }
}

فراغ Curl_pgrsEarlyData(بنية Curl_easy *data, curl_off_t sent)
{
  data->progress.earlydata_sent = sent;
}

/* returns the average speed in bytes / second */
ساكن curl_off_t trspeed(curl_off_t size, /* number of bytes */
                          curl_off_t us)   /* microseconds */
{
  إذا(us < 1)
    أرجع size * 1000000;
  وإلا إذا(size < CURL_OFF_T_MAX / 1000000)
    أرجع (size * 1000000) / us;
  وإلا إذا(us >= 1000000)
    أرجع size / (us / 1000000);
  وإلا
    أرجع CURL_OFF_T_MAX;
}

/* returns TRUE if it is time to show the progress meter */
ساكن bool progress_calc(بنية Curl_easy *data,
                          ثابت بنية curltime *pnow)
{
  بنية Progress * ثابت p = &data->progress;
  صحيح i_next, i_oldest, i_latest;
  timediff_t duration_us, elapsed_us;
  curl_off_t amount;

  /* The time spent so far (from the start) in microseconds */
  elapsed_us = curlx_ptimediff_us(pnow, &p->start);
  p->total.spent_us = elapsed_us;
  p->dl.speed = trspeed(p->dl.cur_size, p->total.spent_us);
  p->ul.speed = trspeed(p->ul.cur_size, p->total.spent_us);

  إذا(!p->speeder_c) { /* no previous record exists */
    p->speed_amount[0] = p->dl.cur_size + p->ul.cur_size;
    p->speed_time[0] = elapsed_us;
    p->speeder_c++;
    /* use the overall average at the start */
    p->current_speed = p->ul.speed + p->dl.speed;
    p->delta.lastshow_us = elapsed_us;
    أرجع TRUE;
  }
  /* We have at least one record now. Where to put the next and
   * where is the latest one? */
  i_next = p->speeder_c % CURL_SPEED_RECORDS;
  i_latest = (i_next > 0) ? (i_next - 1) : (CURL_SPEED_RECORDS - 1);

  /* Make a new record only when some time has passed.
   * Too frequent calls otherwise ruin the history. */
  إذا((elapsed_us - p->speed_time[i_latest]) >= (1000 * 1000)) {
    p->speeder_c++;
    i_latest = i_next;
    p->speed_amount[i_latest] = p->dl.cur_size + p->ul.cur_size;
    p->speed_time[i_latest] = elapsed_us;
  }
  وإلا إذا(data->req.done) {
    /* When a transfer is done, and we did not have a current speed
     * already, update the last record. Otherwise, stay at the speed
     * we have. The last chunk of data, when rate limiting, would increase
     * reported speed since it no longer measures a full second. */
    إذا(!p->current_speed) {
      p->speed_amount[i_latest] = p->dl.cur_size + p->ul.cur_size;
      p->speed_time[i_latest] = elapsed_us;
    }
  }
  وإلا {
    /* transfer ongoing, wait for more time to pass. */
    أرجع FALSE;
  }

  i_oldest = (p->speeder_c < CURL_SPEED_RECORDS) ? 0 :
             ((i_latest + 1) % CURL_SPEED_RECORDS);

  /* How much we transferred between oldest and current records */
  amount = p->speed_amount[i_latest] - p->speed_amount[i_oldest];
  /* How long this took */
  duration_us = p->speed_time[i_latest] - p->speed_time[i_oldest];
  إذا(duration_us <= 0)
    duration_us = 1;

  إذا(amount > (CURL_OFF_T_MAX / 1000000)) {
    /* the 'amount' value is bigger than would fit in 64 bits if
       multiplied with 1000000, so we use the double math for this */
    p->current_speed =
      (curl_off_t)(((مزدوج)amount * 1000000.0) / (مزدوج)duration_us);
  }
  وإلا {
    p->current_speed = amount * 1000000 / duration_us;
  }

  إذا((p->delta.lastshow_us >= 0) && !data->req.done &&
     ((elapsed_us - p->delta.lastshow_us) < (1000 * 1000)))
    أرجع FALSE;
  p->delta.lastshow_us = elapsed_us;
  أرجع TRUE;
}

#ifndef CURL_DISABLE_PROGRESS_METER

بنية pgrs_estimate {
  curl_off_t secs;
  curl_off_t percent;
};

ساكن curl_off_t pgrs_est_percent(curl_off_t total, curl_off_t cur)
{
  إذا(total > 10000)
    أرجع cur / (total / 100);
  وإلا إذا(total > 0)
    أرجع (cur * 100) / total;
  أرجع 0;
}

ساكن فراغ pgrs_estimates(بنية pgrs_dir *d,
                           bool total_known,
                           بنية pgrs_estimate *est)
{
  est->secs = 0;
  est->percent = 0;
  إذا(total_known && (d->speed > 0)) {
    est->secs = d->total_size / d->speed;
    est->percent = pgrs_est_percent(d->total_size, d->cur_size);
  }
}

ساكن فراغ progress_meter(بنية Curl_easy *data)
{
  بنية Progress *p = &data->progress;
  حرف max6[6][7];
  بنية pgrs_estimate dl_estm;
  بنية pgrs_estimate ul_estm;
  بنية pgrs_estimate total_estm;
  curl_off_t total_cur_size;
  curl_off_t total_expected_size;
  curl_off_t dl_size;
  حرف time_کيري[8];
  حرف time_total[8];
  حرف time_spent[8];
  curl_off_t cur_secs = (curl_off_t)p->total.spent_us / 1000000;

  إذا(!p->headers_out) {
    إذا(data->state.resume_from) {
      curl_mfprintf(data->set.err,
                    "** Resuming transfer from byte position %" FMT_OFF_T "\n",
                    data->state.resume_from);
    }
    curl_mfprintf(data->set.err,
                  "  %% Total    %% Received %% Xferd  Average Speed  "
                  "Time    Time    Time   Current\n"
                  "                                 Dload  Upload  "
                  "Total   Spent   Left   Speed\n");
    p->headers_out = TRUE; /* headers are shown */
  }

  /* Figure out the estimated time of arrival for upload and download */
  pgrs_estimates(&p->ul, (bool)p->ul_size_known, &ul_estm);
  pgrs_estimates(&p->dl, (bool)p->dl_size_known, &dl_estm);

  /* Since both happen at the same time, total expected duration is max. */
  total_estm.secs = CURLMAX(ul_estm.secs, dl_estm.secs);
  /* create the three time strings */
  time2str(time_کيري, حجم(time_کيري),
           total_estm.secs > 0 ? (total_estm.secs - cur_secs) : 0);
  time2str(time_total, حجم(time_total), total_estm.secs);
  time2str(time_spent, حجم(time_spent), cur_secs);

  /* Get the total amount of data expected to get transferred */
  total_expected_size = p->ul_size_known ? p->ul.total_size : p->ul.cur_size;

  dl_size = p->dl_size_known ? p->dl.total_size : p->dl.cur_size;

  /* integer overflow check */
  إذا((CURL_OFF_T_MAX - total_expected_size) < dl_size)
    total_expected_size = CURL_OFF_T_MAX; /* capped */
  وإلا
    total_expected_size += dl_size;

  /* We have transferred this much so far */
  total_cur_size = p->dl.cur_size + p->ul.cur_size;

  /* Get the percentage of data transferred so far */
  total_estm.percent = pgrs_est_percent(total_expected_size, total_cur_size);

  curl_mfprintf(data->set.err,
                "\r"
                "%3" FMT_OFF_T " %s "
                "%3" FMT_OFF_T " %s "
                "%3" FMT_OFF_T " %s %s %s %s %s %s %s",
                total_estm.percent, /* 3 letters */    /* total % */
                max6out(total_expected_size, max6[2],
                        حجم(max6[2])),              /* total size */
                dl_estm.percent, /* 3 letters */       /* rcvd % */
                max6out(p->dl.cur_size, max6[0],
                        حجم(max6[0])),              /* rcvd size */
                ul_estm.percent, /* 3 letters */       /* xfer % */
                max6out(p->ul.cur_size, max6[1],
                        حجم(max6[1])),              /* xfer size */
                max6out(p->dl.speed, max6[3],
                        حجم(max6[3])),              /* avrg dl speed */
                max6out(p->ul.speed, max6[4],
                        حجم(max6[4])),              /* avrg ul speed */
                time_total,    /* 7 letters */         /* total time */
                time_spent,    /* 7 letters */         /* time spent */
                time_کيري,     /* 7 letters */         /* time left */
                max6out(p->current_speed, max6[5],
                        حجم(max6[5]))               /* current speed */
    );

  /* we flush the output stream to make it appear as soon as possible */
  fflush(data->set.err);
}
#وإلا /* CURL_DISABLE_PROGRESS_METER */
#define progress_meter(x) Curl_nop_stmt
#endif

/*
 * Curl_pgrsUpdate() returns 0 for success or the value returned by the
 * progress callback!
 */
ساكن CURLcode pgrsupdate(بنية Curl_easy *data, bool showprogress)
{
  إذا(!data->progress.hide) {
    صحيح rc;
    إذا(data->set.fxferinfo) {
      /* There is a callback set, call that */
      بنية Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_fxferinfo);
      rc = data->set.fxferinfo(data->set.progress_client,
                               data->progress.dl.total_size,
                               data->progress.dl.cur_size,
                               data->progress.ul.total_size,
                               data->progress.ul.cur_size);
      CURL_CBAPI_END(&guard);
      إذا(rc != CURL_PROGRESSFUNC_CONTINUE) {
        إذا(rc) {
          failf(data, "Callback aborted");
          أرجع CURLE_ABORTED_BY_CALLBACK;
        }
        أرجع CURLE_OK;
      }
    }
    وإلا إذا(data->set.fprogress) {
      /* The older deprecated callback is set, call that */
      بنية Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_fprogress);
      rc = data->set.fprogress(data->set.progress_client,
                               (مزدوج)data->progress.dl.total_size,
                               (مزدوج)data->progress.dl.cur_size,
                               (مزدوج)data->progress.ul.total_size,
                               (مزدوج)data->progress.ul.cur_size);
      CURL_CBAPI_END(&guard);
      إذا(rc != CURL_PROGRESSFUNC_CONTINUE) {
        إذا(rc) {
          failf(data, "Callback aborted");
          أرجع CURLE_ABORTED_BY_CALLBACK;
        }
        أرجع CURLE_OK;
      }
    }

    إذا(showprogress)
      progress_meter(data);
  }

  أرجع CURLE_OK;
}

ساكن CURLcode pgrs_update(بنية Curl_easy *data,
                            ثابت بنية curltime *pnow)
{
  bool showprogress = progress_calc(data, pnow);
  أرجع pgrsupdate(data, showprogress);
}

CURLcode Curl_pgrsUpdate(بنية Curl_easy *data)
{
  أرجع pgrs_update(data, Curl_pgrs_now(data));
}

CURLcode Curl_pgrsUpdateX(بنية Curl_easy *data,
                          ثابت بنية curltime *pnow)
{
  أرجع pgrs_update(data, pnow);
}

CURLcode Curl_pgrsCheckX(بنية Curl_easy *data,
                         ثابت بنية curltime *pnow)
{
  CURLcode result;

  result = pgrs_update(data, pnow);
  إذا(!result && !data->req.done)
    result = pgrs_speedcheck(data, pnow);
  أرجع result;
}

CURLcode Curl_pgrsCheck(بنية Curl_easy *data)
{
  أرجع Curl_pgrsCheckX(data, Curl_pgrs_now(data));
}

/*
 * Update all progress, do not do progress meter/callbacks.
 */
فراغ Curl_pgrsUpdate_nometer(بنية Curl_easy *data)
{
  (فراغ)progress_calc(data, Curl_pgrs_now(data));
}

فراغ Curl_pgrsCompleted(بنية Curl_easy *data)
{
  بنية Progress * ثابت p = &data->progress;
  p->total.spent_us = curlx_ptimediff_us(Curl_pgrs_now(data), &p->start);
}

timediff_t Curl_pgrs_since_ms(بنية Curl_easy *data,
                              ثابت بنية curltime *pnow,
                              timerid timer)
{
  إذا(!pnow)
    pnow = Curl_pgrs_now(data);
  بدل(timer) {
  حالة TIMER_STARTOP:
    أرجع (curlx_ptimediff_us(pnow, &data->progress.start) -
            data->progress.delta.startop_us) / 1000;
  حالة TIMER_STARTSINGLE:
    أرجع (curlx_ptimediff_us(pnow, &data->progress.start) -
            data->progress.delta.startsingle_us) / 1000;
  افتراضي:
    DEBUGASSERT(0);
    أرجع 0;
  }
}
