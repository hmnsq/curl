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

#ifndef CURL_DISABLE_PROGRESS_METER
/* Provide a string that is 7 letters long (plus the zero byte).

   @unittest 1636 */
UNITTEST تهی time2str(نویسه *r, size_t rsize, curl_off_t seconds);
UNITTEST تهی time2str(نویسه *r, size_t rsize, curl_off_t seconds)
{
  curl_off_t h;
  اگر(seconds <= 0) {
    curlx_strcopy(r, rsize, STRCONST("       "));
    بازگشت;
  }
  h = seconds / 3600;
  اگر(h <= 99) {
    curl_off_t m = (seconds - (h * 3600)) / 60;
    اگر(h <= 9) {
      curl_off_t s = (seconds - (h * 3600)) - (m * 60);
      اگر(h)
        curl_msnprintf(r, rsize, "%" FMT_OFF_T ":%02" FMT_OFF_T ":"
                       "%02" FMT_OFF_T, h, m, s);
      وگرنه
        curl_msnprintf(r, rsize, "  %02" FMT_OFF_T ":%02" FMT_OFF_T, m, s);
    }
    وگرنه
      curl_msnprintf(r, rsize, "%" FMT_OFF_T "h %02" FMT_OFF_T "m", h, m);
  }
  وگرنه {
    curl_off_t d = seconds / 86400;
    h = (seconds - (d * 86400)) / 3600;
    اگر(d <= 99)
      curl_msnprintf(r, rsize, "%2" FMT_OFF_T "d %02" FMT_OFF_T "h", d, h);
    وگرنه اگر(d <= 999)
      curl_msnprintf(r, rsize, "%6" FMT_OFF_T "d", d);
    وگرنه { /* more than 999 days */
      curl_off_t m = d / 30;
      اگر(m <= 999)
        curl_msnprintf(r, rsize, "%6" FMT_OFF_T "m", m);
      وگرنه { /* more than 999 months */
        curl_off_t y = d / 365;
        اگر(y <= 99999)
          curl_msnprintf(r, rsize, "%6" FMT_OFF_T "y", y);
        وگرنه
          curlx_strcopy(r, rsize, STRCONST(">99999y"));
      }
    }
  }
}

/* The point of this function would be to return a string of the input data,
   but never longer than 6 columns (+ one zero byte).
   Add suffix k, M, G when suitable...

   @unittest 1636 */
UNITTEST نویسه *max6out(curl_off_t bytes, نویسه *max6, size_t mlen);
UNITTEST نویسه *max6out(curl_off_t bytes, نویسه *max6, size_t mlen)
{
  /* a signed 64-bit value is 8192 petabytes maximum, shown as
     8.0E (exabytes)*/
  اگر(bytes < 100000)
    curl_msnprintf(max6, mlen, "%6" CURL_FORMAT_CURL_OFF_T, bytes);
  وگرنه {
    ایستا ثابت نویسه unit[] = { 'k', 'M', 'G', 'T', 'P', 'E', 0 };
    صحیح k = 0;
    curl_off_t nbytes;
    curl_off_t rest;
    انجامبده {
      nbytes = bytes / 1024;
      اگر(nbytes < 1000)
        بشکن;
      bytes = nbytes;
      k++;
      DEBUGASSERT(unit[k]);
    } وقتی(unit[k]);
    rest = bytes % 1024;
    اگر(nbytes <= 99)
      /* xx.yyU */
      curl_msnprintf(max6, mlen, "%2" CURL_FORMAT_CURL_OFF_T
                     ".%02" CURL_FORMAT_CURL_OFF_T "%c", nbytes,
                     rest * 100 / 1024, unit[k]);
    وگرنه
      /* xxx.yU */
      curl_msnprintf(max6, mlen, "%3" CURL_FORMAT_CURL_OFF_T
                     ".%" CURL_FORMAT_CURL_OFF_T "%c", nbytes,
                     rest * 10 / 1024, unit[k]);
  }
  بازگشت max6;
}
#endif

ایستا تهی pgrs_speedinit(ساختار Curl_easy *data)
{
  memset(&data->state.keeps_speed, 0, اندازه(ساختار curltime));
}

/*
 * @unittest 1606
 */
UNITTEST CURLcode pgrs_speedcheck(ساختار Curl_easy *data,
                                  ثابت ساختار curltime *pnow);
UNITTEST CURLcode pgrs_speedcheck(ساختار Curl_easy *data,
                                  ثابت ساختار curltime *pnow)
{
  اگر(!data->set.کم_speed_time || !data->set.کم_speed_limit ||
     Curl_xfer_recv_is_paused(data) || Curl_xfer_send_is_paused(data))
    /* A paused transfer is not qualified for speed checks */
    بازگشت CURLE_OK;

  اگر(data->progress.current_speed >= 0) {
    اگر(data->progress.current_speed < data->set.کم_speed_limit) {
      اگر(!data->state.keeps_speed.tv_sec)
        /* under the limit at this moment */
        data->state.keeps_speed = *pnow;
      وگرنه {
        /* how long has it been under the limit */
        timediff_t howlong =
          curlx_ptimediff_ms(pnow, &data->state.keeps_speed);

        اگر(howlong >= data->set.کم_speed_time * 1000) {
          /* too long */
          failf(data, "Operation too slow. Less than %" FMT_OFF_T
                " bytes/sec transferred the last %u seconds",
                data->set.کم_speed_limit, data->set.کم_speed_time);
          بازگشت CURLE_OPERATION_TIMEDOUT;
        }
      }
    }
    وگرنه
      /* faster right now */
      data->state.keeps_speed.tv_sec = 0;
  }

  /* since low speed limit is enabled, set the expire timer to make this
     connection's speed get checked again in a second */
  Curl_expire_set(data, EXPIRE_SPEEDCHECK, 1000, pnow);

  بازگشت CURLE_OK;
}

ثابت ساختار curltime *Curl_pgrs_now(ساختار Curl_easy *data)
{
  curlx_pnow(&data->progress.now);
  بازگشت &data->progress.now;
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

صحیح Curl_pgrsDone(ساختار Curl_easy *data, ثابت ساختار curltime *pnow)
{
  صحیح rc;
  data->progress.delta.lastshow_us = -1;
  rc = Curl_pgrsUpdateX(data, pnow); /* the final (forced) update */
  اگر(rc)
    بازگشت rc;

  اگر(!data->progress.hide && !data->progress.callback)
    /* only output if we do not use a progress callback and we are not
     * hidden */
    curl_mfprintf(data->set.err, "\n");

  بازگشت 0;
}

تهی Curl_pgrsReset(ساختار Curl_easy *data)
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
تهی Curl_pgrsResetTransferSizes(ساختار Curl_easy *data)
{
  Curl_pgrsSetDownloadSize(data, -1);
  Curl_pgrsSetUploadSize(data, -1);
}

تهی Curl_pgrsRecvPause(ساختار Curl_easy *data, bool enable)
{
  اگر(!enable) {
    data->progress.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(data); /* reset low speed measurements */
  }
}

تهی Curl_pgrsSendPause(ساختار Curl_easy *data, bool enable)
{
  اگر(!enable) {
    data->progress.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(data); /* reset low speed measurements */
  }
}

#ifdef CURLVERBOSE
ایستا ثابت نویسه * ثابت pgrs_timer_names[] = {
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

ایستا ثابت نویسه *pgrs_timer_name(timerid timer)
{
  اگر((size_t)timer < CURL_ARRAYSIZE(pgrs_timer_names))
    بازگشت pgrs_timer_names[(size_t)timer];
  بازگشت "?";
}
#endif /* CURLVERBOSE */

/*
 * Curl_pgrsTimeWas(). Store the timestamp time at the given label.
 */
تهی Curl_pgrsTimeWas(ساختار Curl_easy *data, timerid timer,
                      ساختار curltime timestamp)
{
  timediff_t *delta = پوچ;

  انتخاب(timer) {
  پیشفرض:
  حالت TIMER_NONE:
    /* mistake filter */
    بشکن;
  حالت TIMER_STARTOP:
    /* This is set at the start of a transfer */
    data->progress.delta.startop_us =
      curlx_ptimediff_us(&timestamp, &data->progress.start);
    data->progress.delta.startqueue_us = data->progress.delta.startop_us;
    بشکن;
  حالت TIMER_STARTSINGLE:
    /* This is set at the start of each single transfer */
    data->progress.delta.startsingle_us =
      curlx_ptimediff_us(&timestamp, &data->progress.start);
    data->progress.startransfer_added = FALSE;
    بشکن;
  حالت TIMER_POSTQUEUE:
    /* Queue time is accumulative from all involved redirects */
    data->progress.total.queued_us +=
      curlx_ptimediff_us(&timestamp, &data->progress.start) -
      data->progress.delta.startqueue_us;
    بشکن;
  حالت TIMER_NAMELOOKUP:
    delta = &data->progress.total.nslookup_us;
    بشکن;
  حالت TIMER_CONNECT:
    delta = &data->progress.total.connect_us;
    بشکن;
  حالت TIMER_APPCONNECT:
    delta = &data->progress.total.appconnect_us;
    بشکن;
  حالت TIMER_PRETRANSFER:
    delta = &data->progress.total.pretransfer_us;
    بشکن;
  حالت TIMER_STARTTRANSFER:
    /* prevent updating t_starttransfer unless:
     *   1. this is the first time we are setting t_starttransfer
     *   2. a redirect has occurred since the last time t_starttransfer was set
     * This prevents repeated invocations of the function from incorrectly
     * changing the t_starttransfer time.
     */
    اگر(data->progress.startransfer_added) {
      CURL_TRC_M(data, "[%s] ignored", pgrs_timer_name(timer));
      بازگشت;
    }
    data->progress.startransfer_added = TRUE;
    delta = &data->progress.total.starttransfer_us;
    بشکن;
  حالت TIMER_POSTRANSFER:
    delta = &data->progress.total.posttransfer_us;
    بشکن;
  حالت TIMER_REDIRECT:
    data->progress.delta.startredirect_us =
      curlx_ptimediff_us(&timestamp, &data->progress.start);
    /* transfer starts queueing again */
    data->progress.delta.startqueue_us =
      data->progress.delta.startredirect_us;
    بشکن;
  }
  اگر(delta) {
    timediff_t us = curlx_ptimediff_us(&timestamp, &data->progress.start) -
                    data->progress.delta.startsingle_us;
    اگر(us < 1)
      us = 1; /* make sure at least one microsecond passed */
    *delta += us;
    CURL_TRC_M(data, "[%s] added %" FMT_TIMEDIFF_T "us",
               pgrs_timer_name(timer), us);
  }
  وگرنه
    CURL_TRC_M(data, "[%s] set", pgrs_timer_name(timer));
}

/*
 * Curl_pgrsTime(). Store the current time at the given label. This fetches a
 * fresh "now" and returns it.
 *
 * @unittest: 1399
 */
تهی Curl_pgrsTime(ساختار Curl_easy *data, timerid timer)
{
  Curl_pgrsTimeWas(data, timer, *Curl_pgrs_now(data));
}

تهی Curl_pgrsStart(ساختار Curl_easy *data, ثابت ساختار curltime *pnow)
{
  ساختار Progress *p = &data->progress;

  اگر(!pnow)
    pnow = Curl_pgrs_now(data);
  p->speeder_c = 0; /* reset the progress meter display */
  p->start = *pnow;
  memset(&p->delta, 0, اندازه(p->delta));
  memset(&p->total, 0, اندازه(p->total));
  p->startransfer_added = FALSE;
  p->dl.cur_size = 0;
  p->ul.cur_size = 0;
  /* the sizes are unknown at start */
  p->dl_size_known = FALSE;
  p->ul_size_known = FALSE;
}

/* check that the 'delta' amount of bytes are okay to deliver to the
   application, or return error if not. */
CURLcode Curl_pgrs_deliver_check(ساختار Curl_easy *data, size_t delta)
{
  اگر(data->set.max_filesize &&
     ((curl_off_t)delta > data->set.max_filesize - data->progress.deliver)) {
    failf(data, "Would have exceeded max file size");
    بازگشت CURLE_FILESIZE_EXCEEDED;
  }
  بازگشت CURLE_OK;
}

/* this counts how much data is delivered to the application, which
   in compressed cases may differ from downloaded amount */
تهی Curl_pgrs_deliver_inc(ساختار Curl_easy *data, size_t delta)
{
  data->progress.deliver += delta;
}

تهی Curl_pgrs_download_inc(ساختار Curl_easy *data, size_t delta)
{
  اگر(delta) {
    data->progress.dl.cur_size += delta;
    Curl_rlimit_drain(&data->progress.dl.rlimit, delta, پوچ);
  }
}

تهی Curl_pgrs_upload_inc(ساختار Curl_easy *data, size_t delta)
{
  اگر(delta) {
    data->progress.ul.cur_size += delta;
    Curl_rlimit_drain(&data->progress.ul.rlimit, delta, پوچ);
  }
}

/*
 * Set the number of uploaded bytes so far.
 */
تهی Curl_pgrsSetUploadCounter(ساختار Curl_easy *data, curl_off_t size)
{
  data->progress.ul.cur_size = size;
}

تهی Curl_pgrsSetDownloadSize(ساختار Curl_easy *data, curl_off_t size)
{
  اگر(size >= 0) {
    data->progress.dl.total_size = size;
    data->progress.dl_size_known = TRUE;
  }
  وگرنه {
    data->progress.dl.total_size = 0;
    data->progress.dl_size_known = FALSE;
  }
}

تهی Curl_pgrsSetUploadSize(ساختار Curl_easy *data, curl_off_t size)
{
  اگر(size >= 0) {
    data->progress.ul.total_size = size;
    data->progress.ul_size_known = TRUE;
  }
  وگرنه {
    data->progress.ul.total_size = 0;
    data->progress.ul_size_known = FALSE;
  }
}

تهی Curl_pgrsEarlyData(ساختار Curl_easy *data, curl_off_t sent)
{
  data->progress.earlydata_sent = sent;
}

/* returns the average speed in bytes / second */
ایستا curl_off_t trspeed(curl_off_t size, /* number of bytes */
                          curl_off_t us)   /* microseconds */
{
  اگر(us < 1)
    بازگشت size * 1000000;
  وگرنه اگر(size < CURL_OFF_T_MAX / 1000000)
    بازگشت (size * 1000000) / us;
  وگرنه اگر(us >= 1000000)
    بازگشت size / (us / 1000000);
  وگرنه
    بازگشت CURL_OFF_T_MAX;
}

/* returns TRUE if it is time to show the progress meter */
ایستا bool progress_calc(ساختار Curl_easy *data,
                          ثابت ساختار curltime *pnow)
{
  ساختار Progress * ثابت p = &data->progress;
  صحیح i_next, i_oldest, i_latest;
  timediff_t duration_us, elapsed_us;
  curl_off_t amount;

  /* The time spent so far (from the start) in microseconds */
  elapsed_us = curlx_ptimediff_us(pnow, &p->start);
  p->total.spent_us = elapsed_us;
  p->dl.speed = trspeed(p->dl.cur_size, p->total.spent_us);
  p->ul.speed = trspeed(p->ul.cur_size, p->total.spent_us);

  اگر(!p->speeder_c) { /* no previous record exists */
    p->speed_amount[0] = p->dl.cur_size + p->ul.cur_size;
    p->speed_time[0] = elapsed_us;
    p->speeder_c++;
    /* use the overall average at the start */
    p->current_speed = p->ul.speed + p->dl.speed;
    p->delta.lastshow_us = elapsed_us;
    بازگشت TRUE;
  }
  /* We have at least one record now. Where to put the next and
   * where is the latest one? */
  i_next = p->speeder_c % CURL_SPEED_RECORDS;
  i_latest = (i_next > 0) ? (i_next - 1) : (CURL_SPEED_RECORDS - 1);

  /* Make a new record only when some time has passed.
   * Too frequent calls otherwise ruin the history. */
  اگر((elapsed_us - p->speed_time[i_latest]) >= (1000 * 1000)) {
    p->speeder_c++;
    i_latest = i_next;
    p->speed_amount[i_latest] = p->dl.cur_size + p->ul.cur_size;
    p->speed_time[i_latest] = elapsed_us;
  }
  وگرنه اگر(data->req.done) {
    /* When a transfer is done, and we did not have a current speed
     * already, update the last record. Otherwise, stay at the speed
     * we have. The last chunk of data, when rate limiting, would increase
     * reported speed since it no longer measures a full second. */
    اگر(!p->current_speed) {
      p->speed_amount[i_latest] = p->dl.cur_size + p->ul.cur_size;
      p->speed_time[i_latest] = elapsed_us;
    }
  }
  وگرنه {
    /* transfer ongoing, wait for more time to pass. */
    بازگشت FALSE;
  }

  i_oldest = (p->speeder_c < CURL_SPEED_RECORDS) ? 0 :
             ((i_latest + 1) % CURL_SPEED_RECORDS);

  /* How much we transferred between oldest and current records */
  amount = p->speed_amount[i_latest] - p->speed_amount[i_oldest];
  /* How long this took */
  duration_us = p->speed_time[i_latest] - p->speed_time[i_oldest];
  اگر(duration_us <= 0)
    duration_us = 1;

  اگر(amount > (CURL_OFF_T_MAX / 1000000)) {
    /* the 'amount' value is bigger than would fit in 64 bits if
       multiplied with 1000000, so we use the double math for this */
    p->current_speed =
      (curl_off_t)(((دوگانه)amount * 1000000.0) / (دوگانه)duration_us);
  }
  وگرنه {
    p->current_speed = amount * 1000000 / duration_us;
  }

  اگر((p->delta.lastshow_us >= 0) && !data->req.done &&
     ((elapsed_us - p->delta.lastshow_us) < (1000 * 1000)))
    بازگشت FALSE;
  p->delta.lastshow_us = elapsed_us;
  بازگشت TRUE;
}

#ifndef CURL_DISABLE_PROGRESS_METER

ساختار pgrs_estimate {
  curl_off_t secs;
  curl_off_t percent;
};

ایستا curl_off_t pgrs_est_percent(curl_off_t total, curl_off_t cur)
{
  اگر(total > 10000)
    بازگشت cur / (total / 100);
  وگرنه اگر(total > 0)
    بازگشت (cur * 100) / total;
  بازگشت 0;
}

ایستا تهی pgrs_estimates(ساختار pgrs_dir *d,
                           bool total_known,
                           ساختار pgrs_estimate *est)
{
  est->secs = 0;
  est->percent = 0;
  اگر(total_known && (d->speed > 0)) {
    est->secs = d->total_size / d->speed;
    est->percent = pgrs_est_percent(d->total_size, d->cur_size);
  }
}

ایستا تهی progress_meter(ساختار Curl_easy *data)
{
  ساختار Progress *p = &data->progress;
  نویسه max6[6][7];
  ساختار pgrs_estimate dl_estm;
  ساختار pgrs_estimate ul_estm;
  ساختار pgrs_estimate total_estm;
  curl_off_t total_cur_size;
  curl_off_t total_expected_size;
  curl_off_t dl_size;
  نویسه time_چپ[8];
  نویسه time_total[8];
  نویسه time_spent[8];
  curl_off_t cur_secs = (curl_off_t)p->total.spent_us / 1000000;

  اگر(!p->headers_out) {
    اگر(data->state.resume_from) {
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
  time2str(time_چپ, اندازه(time_چپ),
           total_estm.secs > 0 ? (total_estm.secs - cur_secs) : 0);
  time2str(time_total, اندازه(time_total), total_estm.secs);
  time2str(time_spent, اندازه(time_spent), cur_secs);

  /* Get the total amount of data expected to get transferred */
  total_expected_size = p->ul_size_known ? p->ul.total_size : p->ul.cur_size;

  dl_size = p->dl_size_known ? p->dl.total_size : p->dl.cur_size;

  /* integer overflow check */
  اگر((CURL_OFF_T_MAX - total_expected_size) < dl_size)
    total_expected_size = CURL_OFF_T_MAX; /* capped */
  وگرنه
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
                        اندازه(max6[2])),              /* total size */
                dl_estm.percent, /* 3 letters */       /* rcvd % */
                max6out(p->dl.cur_size, max6[0],
                        اندازه(max6[0])),              /* rcvd size */
                ul_estm.percent, /* 3 letters */       /* xfer % */
                max6out(p->ul.cur_size, max6[1],
                        اندازه(max6[1])),              /* xfer size */
                max6out(p->dl.speed, max6[3],
                        اندازه(max6[3])),              /* avrg dl speed */
                max6out(p->ul.speed, max6[4],
                        اندازه(max6[4])),              /* avrg ul speed */
                time_total,    /* 7 letters */         /* total time */
                time_spent,    /* 7 letters */         /* time spent */
                time_چپ,     /* 7 letters */         /* time left */
                max6out(p->current_speed, max6[5],
                        اندازه(max6[5]))               /* current speed */
    );

  /* we flush the output stream to make it appear as soon as possible */
  fflush(data->set.err);
}
#وگرنه /* CURL_DISABLE_PROGRESS_METER */
#define progress_meter(x) Curl_nop_stmt
#endif

/*
 * Curl_pgrsUpdate() returns 0 for success or the value returned by the
 * progress callback!
 */
ایستا CURLcode pgrsupdate(ساختار Curl_easy *data, bool showprogress)
{
  اگر(!data->progress.hide) {
    صحیح rc;
    اگر(data->set.fxferinfo) {
      /* There is a callback set, call that */
      ساختار Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_fxferinfo);
      rc = data->set.fxferinfo(data->set.progress_client,
                               data->progress.dl.total_size,
                               data->progress.dl.cur_size,
                               data->progress.ul.total_size,
                               data->progress.ul.cur_size);
      CURL_CBAPI_END(&guard);
      اگر(rc != CURL_PROGRESSFUNC_CONTINUE) {
        اگر(rc) {
          failf(data, "Callback aborted");
          بازگشت CURLE_ABORTED_BY_CALLBACK;
        }
        بازگشت CURLE_OK;
      }
    }
    وگرنه اگر(data->set.fprogress) {
      /* The older deprecated callback is set, call that */
      ساختار Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_fprogress);
      rc = data->set.fprogress(data->set.progress_client,
                               (دوگانه)data->progress.dl.total_size,
                               (دوگانه)data->progress.dl.cur_size,
                               (دوگانه)data->progress.ul.total_size,
                               (دوگانه)data->progress.ul.cur_size);
      CURL_CBAPI_END(&guard);
      اگر(rc != CURL_PROGRESSFUNC_CONTINUE) {
        اگر(rc) {
          failf(data, "Callback aborted");
          بازگشت CURLE_ABORTED_BY_CALLBACK;
        }
        بازگشت CURLE_OK;
      }
    }

    اگر(showprogress)
      progress_meter(data);
  }

  بازگشت CURLE_OK;
}

ایستا CURLcode pgrs_update(ساختار Curl_easy *data,
                            ثابت ساختار curltime *pnow)
{
  bool showprogress = progress_calc(data, pnow);
  بازگشت pgrsupdate(data, showprogress);
}

CURLcode Curl_pgrsUpdate(ساختار Curl_easy *data)
{
  بازگشت pgrs_update(data, Curl_pgrs_now(data));
}

CURLcode Curl_pgrsUpdateX(ساختار Curl_easy *data,
                          ثابت ساختار curltime *pnow)
{
  بازگشت pgrs_update(data, pnow);
}

CURLcode Curl_pgrsCheckX(ساختار Curl_easy *data,
                         ثابت ساختار curltime *pnow)
{
  CURLcode result;

  result = pgrs_update(data, pnow);
  اگر(!result && !data->req.done)
    result = pgrs_speedcheck(data, pnow);
  بازگشت result;
}

CURLcode Curl_pgrsCheck(ساختار Curl_easy *data)
{
  بازگشت Curl_pgrsCheckX(data, Curl_pgrs_now(data));
}

/*
 * Update all progress, do not do progress meter/callbacks.
 */
تهی Curl_pgrsUpdate_nometer(ساختار Curl_easy *data)
{
  (تهی)progress_calc(data, Curl_pgrs_now(data));
}

تهی Curl_pgrsCompleted(ساختار Curl_easy *data)
{
  ساختار Progress * ثابت p = &data->progress;
  p->total.spent_us = curlx_ptimediff_us(Curl_pgrs_now(data), &p->start);
}

timediff_t Curl_pgrs_since_ms(ساختار Curl_easy *data,
                              ثابت ساختار curltime *pnow,
                              timerid timer)
{
  اگر(!pnow)
    pnow = Curl_pgrs_now(data);
  انتخاب(timer) {
  حالت TIMER_STARTOP:
    بازگشت (curlx_ptimediff_us(pnow, &data->progress.start) -
            data->progress.delta.startop_us) / 1000;
  حالت TIMER_STARTSINGLE:
    بازگشت (curlx_ptimediff_us(pnow, &data->progress.start) -
            data->progress.delta.startsingle_us) / 1000;
  پیشفرض:
    DEBUGASSERT(0);
    بازگشت 0;
  }
}
