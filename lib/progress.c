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
#অন্তর্ভাৱ "curl_setup.h"

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "multiif.h"
#অন্তর্ভাৱ "progress.h"
#অন্তর্ভাৱ "transfer.h"
#অন্তর্ভাৱ "curlx/strcopy.h"

#ifndef CURL_DISABLE_PROGRESS_METER
/* Provide a string that is 7 letters long (plus the zero byte).

   @unittest 1636 */
UNITTEST শূন্য time2str(ৱর্ণ *r, size_t rsize, curl_off_t seconds);
UNITTEST শূন্য time2str(ৱর্ণ *r, size_t rsize, curl_off_t seconds)
{
  curl_off_t h;
  যদি(seconds <= 0) {
    curlx_strcopy(r, rsize, STRCONST("       "));
    ঘূৰাই;
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
UNITTEST ৱর্ণ *max6out(curl_off_t bytes, ৱর্ণ *max6, size_t mlen);
UNITTEST ৱর্ণ *max6out(curl_off_t bytes, ৱর্ণ *max6, size_t mlen)
{
  /* a signed 64-bit value is 8192 petabytes maximum, shown as
     8.0E (exabytes)*/
  যদি(bytes < 100000)
    curl_msnprintf(max6, mlen, "%6" CURL_FORMAT_CURL_OFF_T, bytes);
  অন্যথা {
    স্থির ধ্রুৱ ৱর্ণ unit[] = { 'k', 'M', 'G', 'T', 'P', 'E', 0 };
    পূর্ণাঙ্ক k = 0;
    curl_off_t nbytes;
    curl_off_t rest;
    কৰা {
      nbytes = bytes / 1024;
      যদি(nbytes < 1000)
        ভঙ্গ;
      bytes = nbytes;
      k++;
      DEBUGASSERT(unit[k]);
    } যেতিয়ালৈ(unit[k]);
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
  ঘূৰাই max6;
}
#endif

স্থির শূন্য pgrs_speedinit(সংরচনা Curl_easy *data)
{
  memset(&data->state.keeps_speed, 0, পরিমাণ(সংরচনা curltime));
}

/*
 * @unittest 1606
 */
UNITTEST CURLcode pgrs_speedcheck(সংরচনা Curl_easy *data,
                                  ধ্রুৱ সংরচনা curltime *pnow);
UNITTEST CURLcode pgrs_speedcheck(সংরচনা Curl_easy *data,
                                  ধ্রুৱ সংরচনা curltime *pnow)
{
  যদি(!data->set.নিচেৰ_speed_time || !data->set.নিচেৰ_speed_limit ||
     Curl_xfer_recv_is_paused(data) || Curl_xfer_send_is_paused(data))
    /* A paused transfer is not qualified for speed checks */
    ঘূৰাই CURLE_OK;

  যদি(data->progress.current_speed >= 0) {
    যদি(data->progress.current_speed < data->set.নিচেৰ_speed_limit) {
      যদি(!data->state.keeps_speed.tv_sec)
        /* under the limit at this moment */
        data->state.keeps_speed = *pnow;
      অন্যথা {
        /* how long has it been under the limit */
        timediff_t howlong =
          curlx_ptimediff_ms(pnow, &data->state.keeps_speed);

        যদি(howlong >= data->set.নিচেৰ_speed_time * 1000) {
          /* too long */
          failf(data, "Operation too slow. Less than %" FMT_OFF_T
                " bytes/sec transferred the last %u seconds",
                data->set.নিচেৰ_speed_limit, data->set.নিচেৰ_speed_time);
          ঘূৰাই CURLE_OPERATION_TIMEDOUT;
        }
      }
    }
    অন্যথা
      /* faster right now */
      data->state.keeps_speed.tv_sec = 0;
  }

  /* since low speed limit is enabled, set the expire timer to make this
     connection's speed get checked again in a second */
  Curl_expire_set(data, EXPIRE_SPEEDCHECK, 1000, pnow);

  ঘূৰাই CURLE_OK;
}

ধ্রুৱ সংরচনা curltime *Curl_pgrs_now(সংরচনা Curl_easy *data)
{
  curlx_pnow(&data->progress.now);
  ঘূৰাই &data->progress.now;
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

পূর্ণাঙ্ক Curl_pgrsDone(সংরচনা Curl_easy *data, ধ্রুৱ সংরচনা curltime *pnow)
{
  পূর্ণাঙ্ক rc;
  data->progress.delta.lastshow_us = -1;
  rc = Curl_pgrsUpdateX(data, pnow); /* the final (forced) update */
  যদি(rc)
    ঘূৰাই rc;

  যদি(!data->progress.hide && !data->progress.callback)
    /* only output if we do not use a progress callback and we are not
     * hidden */
    curl_mfprintf(data->set.err, "\n");

  ঘূৰাই 0;
}

শূন্য Curl_pgrsReset(সংরচনা Curl_easy *data)
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
শূন্য Curl_pgrsResetTransferSizes(সংরচনা Curl_easy *data)
{
  Curl_pgrsSetDownloadSize(data, -1);
  Curl_pgrsSetUploadSize(data, -1);
}

শূন্য Curl_pgrsRecvPause(সংরচনা Curl_easy *data, bool enable)
{
  যদি(!enable) {
    data->progress.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(data); /* reset low speed measurements */
  }
}

শূন্য Curl_pgrsSendPause(সংরচনা Curl_easy *data, bool enable)
{
  যদি(!enable) {
    data->progress.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(data); /* reset low speed measurements */
  }
}

#ifdef CURLVERBOSE
স্থির ধ্রুৱ ৱর্ণ * ধ্রুৱ pgrs_timer_names[] = {
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

স্থির ধ্রুৱ ৱর্ণ *pgrs_timer_name(timerid timer)
{
  যদি((size_t)timer < CURL_ARRAYSIZE(pgrs_timer_names))
    ঘূৰাই pgrs_timer_names[(size_t)timer];
  ঘূৰাই "?";
}
#endif /* CURLVERBOSE */

/*
 * Curl_pgrsTimeWas(). Store the timestamp time at the given label.
 */
শূন্য Curl_pgrsTimeWas(সংরচনা Curl_easy *data, timerid timer,
                      সংরচনা curltime timestamp)
{
  timediff_t *delta = রিক্ত;

  ৱিকল্প(timer) {
  স্ৱভাৱ:
  পক্ষ TIMER_NONE:
    /* mistake filter */
    ভঙ্গ;
  পক্ষ TIMER_STARTOP:
    /* This is set at the start of a transfer */
    data->progress.delta.startop_us =
      curlx_ptimediff_us(&timestamp, &data->progress.start);
    data->progress.delta.startqueue_us = data->progress.delta.startop_us;
    ভঙ্গ;
  পক্ষ TIMER_STARTSINGLE:
    /* This is set at the start of each single transfer */
    data->progress.delta.startsingle_us =
      curlx_ptimediff_us(&timestamp, &data->progress.start);
    data->progress.startransfer_added = FALSE;
    ভঙ্গ;
  পক্ষ TIMER_POSTQUEUE:
    /* Queue time is accumulative from all involved redirects */
    data->progress.total.queued_us +=
      curlx_ptimediff_us(&timestamp, &data->progress.start) -
      data->progress.delta.startqueue_us;
    ভঙ্গ;
  পক্ষ TIMER_NAMELOOKUP:
    delta = &data->progress.total.nslookup_us;
    ভঙ্গ;
  পক্ষ TIMER_CONNECT:
    delta = &data->progress.total.connect_us;
    ভঙ্গ;
  পক্ষ TIMER_APPCONNECT:
    delta = &data->progress.total.appconnect_us;
    ভঙ্গ;
  পক্ষ TIMER_PRETRANSFER:
    delta = &data->progress.total.pretransfer_us;
    ভঙ্গ;
  পক্ষ TIMER_STARTTRANSFER:
    /* prevent updating t_starttransfer unless:
     *   1. this is the first time we are setting t_starttransfer
     *   2. a redirect has occurred since the last time t_starttransfer was set
     * This prevents repeated invocations of the function from incorrectly
     * changing the t_starttransfer time.
     */
    যদি(data->progress.startransfer_added) {
      CURL_TRC_M(data, "[%s] ignored", pgrs_timer_name(timer));
      ঘূৰাই;
    }
    data->progress.startransfer_added = TRUE;
    delta = &data->progress.total.starttransfer_us;
    ভঙ্গ;
  পক্ষ TIMER_POSTRANSFER:
    delta = &data->progress.total.posttransfer_us;
    ভঙ্গ;
  পক্ষ TIMER_REDIRECT:
    data->progress.delta.startredirect_us =
      curlx_ptimediff_us(&timestamp, &data->progress.start);
    /* transfer starts queueing again */
    data->progress.delta.startqueue_us =
      data->progress.delta.startredirect_us;
    ভঙ্গ;
  }
  যদি(delta) {
    timediff_t us = curlx_ptimediff_us(&timestamp, &data->progress.start) -
                    data->progress.delta.startsingle_us;
    যদি(us < 1)
      us = 1; /* make sure at least one microsecond passed */
    *delta += us;
    CURL_TRC_M(data, "[%s] added %" FMT_TIMEDIFF_T "us",
               pgrs_timer_name(timer), us);
  }
  অন্যথা
    CURL_TRC_M(data, "[%s] set", pgrs_timer_name(timer));
}

/*
 * Curl_pgrsTime(). Store the current time at the given label. This fetches a
 * fresh "now" and returns it.
 *
 * @unittest: 1399
 */
শূন্য Curl_pgrsTime(সংরচনা Curl_easy *data, timerid timer)
{
  Curl_pgrsTimeWas(data, timer, *Curl_pgrs_now(data));
}

শূন্য Curl_pgrsStart(সংরচনা Curl_easy *data, ধ্রুৱ সংরচনা curltime *pnow)
{
  সংরচনা Progress *p = &data->progress;

  যদি(!pnow)
    pnow = Curl_pgrs_now(data);
  p->speeder_c = 0; /* reset the progress meter display */
  p->start = *pnow;
  memset(&p->delta, 0, পরিমাণ(p->delta));
  memset(&p->total, 0, পরিমাণ(p->total));
  p->startransfer_added = FALSE;
  p->dl.cur_size = 0;
  p->ul.cur_size = 0;
  /* the sizes are unknown at start */
  p->dl_size_known = FALSE;
  p->ul_size_known = FALSE;
}

/* check that the 'delta' amount of bytes are okay to deliver to the
   application, or return error if not. */
CURLcode Curl_pgrs_deliver_check(সংরচনা Curl_easy *data, size_t delta)
{
  যদি(data->set.max_filesize &&
     ((curl_off_t)delta > data->set.max_filesize - data->progress.deliver)) {
    failf(data, "Would have exceeded max file size");
    ঘূৰাই CURLE_FILESIZE_EXCEEDED;
  }
  ঘূৰাই CURLE_OK;
}

/* this counts how much data is delivered to the application, which
   in compressed cases may differ from downloaded amount */
শূন্য Curl_pgrs_deliver_inc(সংরচনা Curl_easy *data, size_t delta)
{
  data->progress.deliver += delta;
}

শূন্য Curl_pgrs_download_inc(সংরচনা Curl_easy *data, size_t delta)
{
  যদি(delta) {
    data->progress.dl.cur_size += delta;
    Curl_rlimit_drain(&data->progress.dl.rlimit, delta, রিক্ত);
  }
}

শূন্য Curl_pgrs_upload_inc(সংরচনা Curl_easy *data, size_t delta)
{
  যদি(delta) {
    data->progress.ul.cur_size += delta;
    Curl_rlimit_drain(&data->progress.ul.rlimit, delta, রিক্ত);
  }
}

/*
 * Set the number of uploaded bytes so far.
 */
শূন্য Curl_pgrsSetUploadCounter(সংরচনা Curl_easy *data, curl_off_t size)
{
  data->progress.ul.cur_size = size;
}

শূন্য Curl_pgrsSetDownloadSize(সংরচনা Curl_easy *data, curl_off_t size)
{
  যদি(size >= 0) {
    data->progress.dl.total_size = size;
    data->progress.dl_size_known = TRUE;
  }
  অন্যথা {
    data->progress.dl.total_size = 0;
    data->progress.dl_size_known = FALSE;
  }
}

শূন্য Curl_pgrsSetUploadSize(সংরচনা Curl_easy *data, curl_off_t size)
{
  যদি(size >= 0) {
    data->progress.ul.total_size = size;
    data->progress.ul_size_known = TRUE;
  }
  অন্যথা {
    data->progress.ul.total_size = 0;
    data->progress.ul_size_known = FALSE;
  }
}

শূন্য Curl_pgrsEarlyData(সংরচনা Curl_easy *data, curl_off_t sent)
{
  data->progress.earlydata_sent = sent;
}

/* returns the average speed in bytes / second */
স্থির curl_off_t trspeed(curl_off_t size, /* number of bytes */
                          curl_off_t us)   /* microseconds */
{
  যদি(us < 1)
    ঘূৰাই size * 1000000;
  অন্যথা যদি(size < CURL_OFF_T_MAX / 1000000)
    ঘূৰাই (size * 1000000) / us;
  অন্যথা যদি(us >= 1000000)
    ঘূৰাই size / (us / 1000000);
  অন্যথা
    ঘূৰাই CURL_OFF_T_MAX;
}

/* returns TRUE if it is time to show the progress meter */
স্থির bool progress_calc(সংরচনা Curl_easy *data,
                          ধ্রুৱ সংরচনা curltime *pnow)
{
  সংরচনা Progress * ধ্রুৱ p = &data->progress;
  পূর্ণাঙ্ক i_next, i_oldest, i_latest;
  timediff_t duration_us, elapsed_us;
  curl_off_t amount;

  /* The time spent so far (from the start) in microseconds */
  elapsed_us = curlx_ptimediff_us(pnow, &p->start);
  p->total.spent_us = elapsed_us;
  p->dl.speed = trspeed(p->dl.cur_size, p->total.spent_us);
  p->ul.speed = trspeed(p->ul.cur_size, p->total.spent_us);

  যদি(!p->speeder_c) { /* no previous record exists */
    p->speed_amount[0] = p->dl.cur_size + p->ul.cur_size;
    p->speed_time[0] = elapsed_us;
    p->speeder_c++;
    /* use the overall average at the start */
    p->current_speed = p->ul.speed + p->dl.speed;
    p->delta.lastshow_us = elapsed_us;
    ঘূৰাই TRUE;
  }
  /* We have at least one record now. Where to put the next and
   * where is the latest one? */
  i_next = p->speeder_c % CURL_SPEED_RECORDS;
  i_latest = (i_next > 0) ? (i_next - 1) : (CURL_SPEED_RECORDS - 1);

  /* Make a new record only when some time has passed.
   * Too frequent calls otherwise ruin the history. */
  যদি((elapsed_us - p->speed_time[i_latest]) >= (1000 * 1000)) {
    p->speeder_c++;
    i_latest = i_next;
    p->speed_amount[i_latest] = p->dl.cur_size + p->ul.cur_size;
    p->speed_time[i_latest] = elapsed_us;
  }
  অন্যথা যদি(data->req.done) {
    /* When a transfer is done, and we did not have a current speed
     * already, update the last record. Otherwise, stay at the speed
     * we have. The last chunk of data, when rate limiting, would increase
     * reported speed since it no longer measures a full second. */
    যদি(!p->current_speed) {
      p->speed_amount[i_latest] = p->dl.cur_size + p->ul.cur_size;
      p->speed_time[i_latest] = elapsed_us;
    }
  }
  অন্যথা {
    /* transfer ongoing, wait for more time to pass. */
    ঘূৰাই FALSE;
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
      (curl_off_t)(((দ্ৱিগুণ)amount * 1000000.0) / (দ্ৱিগুণ)duration_us);
  }
  অন্যথা {
    p->current_speed = amount * 1000000 / duration_us;
  }

  যদি((p->delta.lastshow_us >= 0) && !data->req.done &&
     ((elapsed_us - p->delta.lastshow_us) < (1000 * 1000)))
    ঘূৰাই FALSE;
  p->delta.lastshow_us = elapsed_us;
  ঘূৰাই TRUE;
}

#ifndef CURL_DISABLE_PROGRESS_METER

সংরচনা pgrs_estimate {
  curl_off_t secs;
  curl_off_t percent;
};

স্থির curl_off_t pgrs_est_percent(curl_off_t total, curl_off_t cur)
{
  যদি(total > 10000)
    ঘূৰাই cur / (total / 100);
  অন্যথা যদি(total > 0)
    ঘূৰাই (cur * 100) / total;
  ঘূৰাই 0;
}

স্থির শূন্য pgrs_estimates(সংরচনা pgrs_dir *d,
                           bool total_known,
                           সংরচনা pgrs_estimate *est)
{
  est->secs = 0;
  est->percent = 0;
  যদি(total_known && (d->speed > 0)) {
    est->secs = d->total_size / d->speed;
    est->percent = pgrs_est_percent(d->total_size, d->cur_size);
  }
}

স্থির শূন্য progress_meter(সংরচনা Curl_easy *data)
{
  সংরচনা Progress *p = &data->progress;
  ৱর্ণ max6[6][7];
  সংরচনা pgrs_estimate dl_estm;
  সংরচনা pgrs_estimate ul_estm;
  সংরচনা pgrs_estimate total_estm;
  curl_off_t total_cur_size;
  curl_off_t total_expected_size;
  curl_off_t dl_size;
  ৱর্ণ time_বাম[8];
  ৱর্ণ time_total[8];
  ৱর্ণ time_spent[8];
  curl_off_t cur_secs = (curl_off_t)p->total.spent_us / 1000000;

  যদি(!p->headers_out) {
    যদি(data->state.resume_from) {
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
  time2str(time_বাম, পরিমাণ(time_বাম),
           total_estm.secs > 0 ? (total_estm.secs - cur_secs) : 0);
  time2str(time_total, পরিমাণ(time_total), total_estm.secs);
  time2str(time_spent, পরিমাণ(time_spent), cur_secs);

  /* Get the total amount of data expected to get transferred */
  total_expected_size = p->ul_size_known ? p->ul.total_size : p->ul.cur_size;

  dl_size = p->dl_size_known ? p->dl.total_size : p->dl.cur_size;

  /* integer overflow check */
  যদি((CURL_OFF_T_MAX - total_expected_size) < dl_size)
    total_expected_size = CURL_OFF_T_MAX; /* capped */
  অন্যথা
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
                        পরিমাণ(max6[2])),              /* total size */
                dl_estm.percent, /* 3 letters */       /* rcvd % */
                max6out(p->dl.cur_size, max6[0],
                        পরিমাণ(max6[0])),              /* rcvd size */
                ul_estm.percent, /* 3 letters */       /* xfer % */
                max6out(p->ul.cur_size, max6[1],
                        পরিমাণ(max6[1])),              /* xfer size */
                max6out(p->dl.speed, max6[3],
                        পরিমাণ(max6[3])),              /* avrg dl speed */
                max6out(p->ul.speed, max6[4],
                        পরিমাণ(max6[4])),              /* avrg ul speed */
                time_total,    /* 7 letters */         /* total time */
                time_spent,    /* 7 letters */         /* time spent */
                time_বাম,     /* 7 letters */         /* time left */
                max6out(p->current_speed, max6[5],
                        পরিমাণ(max6[5]))               /* current speed */
    );

  /* we flush the output stream to make it appear as soon as possible */
  fflush(data->set.err);
}
#অন্যথা /* CURL_DISABLE_PROGRESS_METER */
#define progress_meter(x) Curl_nop_stmt
#endif

/*
 * Curl_pgrsUpdate() returns 0 for success or the value returned by the
 * progress callback!
 */
স্থির CURLcode pgrsupdate(সংরচনা Curl_easy *data, bool showprogress)
{
  যদি(!data->progress.hide) {
    পূর্ণাঙ্ক rc;
    যদি(data->set.fxferinfo) {
      /* There is a callback set, call that */
      সংরচনা Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_fxferinfo);
      rc = data->set.fxferinfo(data->set.progress_client,
                               data->progress.dl.total_size,
                               data->progress.dl.cur_size,
                               data->progress.ul.total_size,
                               data->progress.ul.cur_size);
      CURL_CBAPI_END(&guard);
      যদি(rc != CURL_PROGRESSFUNC_CONTINUE) {
        যদি(rc) {
          failf(data, "Callback aborted");
          ঘূৰাই CURLE_ABORTED_BY_CALLBACK;
        }
        ঘূৰাই CURLE_OK;
      }
    }
    অন্যথা যদি(data->set.fprogress) {
      /* The older deprecated callback is set, call that */
      সংরচনা Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_fprogress);
      rc = data->set.fprogress(data->set.progress_client,
                               (দ্ৱিগুণ)data->progress.dl.total_size,
                               (দ্ৱিগুণ)data->progress.dl.cur_size,
                               (দ্ৱিগুণ)data->progress.ul.total_size,
                               (দ্ৱিগুণ)data->progress.ul.cur_size);
      CURL_CBAPI_END(&guard);
      যদি(rc != CURL_PROGRESSFUNC_CONTINUE) {
        যদি(rc) {
          failf(data, "Callback aborted");
          ঘূৰাই CURLE_ABORTED_BY_CALLBACK;
        }
        ঘূৰাই CURLE_OK;
      }
    }

    যদি(showprogress)
      progress_meter(data);
  }

  ঘূৰাই CURLE_OK;
}

স্থির CURLcode pgrs_update(সংরচনা Curl_easy *data,
                            ধ্রুৱ সংরচনা curltime *pnow)
{
  bool showprogress = progress_calc(data, pnow);
  ঘূৰাই pgrsupdate(data, showprogress);
}

CURLcode Curl_pgrsUpdate(সংরচনা Curl_easy *data)
{
  ঘূৰাই pgrs_update(data, Curl_pgrs_now(data));
}

CURLcode Curl_pgrsUpdateX(সংরচনা Curl_easy *data,
                          ধ্রুৱ সংরচনা curltime *pnow)
{
  ঘূৰাই pgrs_update(data, pnow);
}

CURLcode Curl_pgrsCheckX(সংরচনা Curl_easy *data,
                         ধ্রুৱ সংরচনা curltime *pnow)
{
  CURLcode result;

  result = pgrs_update(data, pnow);
  যদি(!result && !data->req.done)
    result = pgrs_speedcheck(data, pnow);
  ঘূৰাই result;
}

CURLcode Curl_pgrsCheck(সংরচনা Curl_easy *data)
{
  ঘূৰাই Curl_pgrsCheckX(data, Curl_pgrs_now(data));
}

/*
 * Update all progress, do not do progress meter/callbacks.
 */
শূন্য Curl_pgrsUpdate_nometer(সংরচনা Curl_easy *data)
{
  (শূন্য)progress_calc(data, Curl_pgrs_now(data));
}

শূন্য Curl_pgrsCompleted(সংরচনা Curl_easy *data)
{
  সংরচনা Progress * ধ্রুৱ p = &data->progress;
  p->total.spent_us = curlx_ptimediff_us(Curl_pgrs_now(data), &p->start);
}

timediff_t Curl_pgrs_since_ms(সংরচনা Curl_easy *data,
                              ধ্রুৱ সংরচনা curltime *pnow,
                              timerid timer)
{
  যদি(!pnow)
    pnow = Curl_pgrs_now(data);
  ৱিকল্প(timer) {
  পক্ষ TIMER_STARTOP:
    ঘূৰাই (curlx_ptimediff_us(pnow, &data->progress.start) -
            data->progress.delta.startop_us) / 1000;
  পক্ষ TIMER_STARTSINGLE:
    ঘূৰাই (curlx_ptimediff_us(pnow, &data->progress.start) -
            data->progress.delta.startsingle_us) / 1000;
  স্ৱভাৱ:
    DEBUGASSERT(0);
    ঘূৰাই 0;
  }
}
