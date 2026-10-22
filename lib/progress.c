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
#include "curl_setup.h"

#include "urldata.h"
#include "curl_trc.h"
#include "multiif.h"
#include "progress.h"
#include "transfer.h"
#include "curlx/strcopy.h"

#ifndef CURL_DISABLE_PROGRESS_METER
/* Provide a string that is 7 letters long (plus the zero byte).

   @unittest 1636 */
UNITTEST void time2str(var_na *r, size_t rsize, curl_off_t seconds);
UNITTEST void time2str(var_na *r, size_t rsize, curl_off_t seconds)
{
  curl_off_t h;
  නම්(seconds <= 0) {
    curlx_strcopy(r, rsize, STRCONST("       "));
    ආපසු;
  }
  h = seconds / 3600;
  නම්(h <= 99) {
    curl_off_t m = (seconds - (h * 3600)) / 60;
    නම්(h <= 9) {
      curl_off_t s = (seconds - (h * 3600)) - (m * 60);
      නම්(h)
        curl_msnprintf(r, rsize, "%" FMT_OFF_T ":%02" FMT_OFF_T ":"
                       "%02" FMT_OFF_T, h, m, s);
      නැතහොත්
        curl_msnprintf(r, rsize, "  %02" FMT_OFF_T ":%02" FMT_OFF_T, m, s);
    }
    නැතහොත්
      curl_msnprintf(r, rsize, "%" FMT_OFF_T "h %02" FMT_OFF_T "m", h, m);
  }
  නැතහොත් {
    curl_off_t d = seconds / 86400;
    h = (seconds - (d * 86400)) / 3600;
    නම්(d <= 99)
      curl_msnprintf(r, rsize, "%2" FMT_OFF_T "d %02" FMT_OFF_T "h", d, h);
    නැතහොත් නම්(d <= 999)
      curl_msnprintf(r, rsize, "%6" FMT_OFF_T "d", d);
    නැතහොත් { /* more than 999 days */
      curl_off_t m = d / 30;
      නම්(m <= 999)
        curl_msnprintf(r, rsize, "%6" FMT_OFF_T "m", m);
      නැතහොත් { /* more than 999 months */
        curl_off_t y = d / 365;
        නම්(y <= 99999)
          curl_msnprintf(r, rsize, "%6" FMT_OFF_T "y", y);
        නැතහොත්
          curlx_strcopy(r, rsize, STRCONST(">99999y"));
      }
    }
  }
}

/* The point of this function would be to return a string of the input data,
   but never longer than 6 columns (+ one zero byte).
   Add suffix k, M, G when suitable...

   @unittest 1636 */
UNITTEST var_na *max6out(curl_off_t bytes, var_na *max6, size_t mlen);
UNITTEST var_na *max6out(curl_off_t bytes, var_na *max6, size_t mlen)
{
  /* a signed 64-bit value is 8192 petabytes maximum, shown as
     8.0E (exabytes)*/
  නම්(bytes < 100000)
    curl_msnprintf(max6, mlen, "%6" CURL_FORMAT_CURL_OFF_T, bytes);
  නැතහොත් {
    static const var_na unit[] = { 'k', 'M', 'G', 'T', 'P', 'E', 0 };
    පූර්ණ_සංඛ්‍යා k = 0;
    curl_off_t nbytes;
    curl_off_t rest;
    do {
      nbytes = bytes / 1024;
      නම්(nbytes < 1000)
        bha_nga;
      bytes = nbytes;
      k++;
      DEBUGASSERT(unit[k]);
    } අතරතුර(unit[k]);
    rest = bytes % 1024;
    නම්(nbytes <= 99)
      /* xx.yyU */
      curl_msnprintf(max6, mlen, "%2" CURL_FORMAT_CURL_OFF_T
                     ".%02" CURL_FORMAT_CURL_OFF_T "%c", nbytes,
                     rest * 100 / 1024, unit[k]);
    නැතහොත්
      /* xxx.yU */
      curl_msnprintf(max6, mlen, "%3" CURL_FORMAT_CURL_OFF_T
                     ".%" CURL_FORMAT_CURL_OFF_T "%c", nbytes,
                     rest * 10 / 1024, unit[k]);
  }
  ආපසු max6;
}
#endif

static void pgrs_speedinit(sa_nracanaa Curl_easy *data)
{
  memset(&data->state.keeps_speed, 0, parimaa_na(sa_nracanaa curltime));
}

/*
 * @unittest 1606
 */
UNITTEST CURLcode pgrs_speedcheck(sa_nracanaa Curl_easy *data,
                                  const sa_nracanaa curltime *pnow);
UNITTEST CURLcode pgrs_speedcheck(sa_nracanaa Curl_easy *data,
                                  const sa_nracanaa curltime *pnow)
{
  නම්(!data->set.low_speed_time || !data->set.low_speed_limit ||
     Curl_xfer_recv_is_paused(data) || Curl_xfer_send_is_paused(data))
    /* A paused transfer is not qualified for speed checks */
    ආපසු CURLE_OK;

  නම්(data->progress.current_speed >= 0) {
    නම්(data->progress.current_speed < data->set.low_speed_limit) {
      නම්(!data->state.keeps_speed.tv_sec)
        /* under the limit at this moment */
        data->state.keeps_speed = *pnow;
      නැතහොත් {
        /* how long has it been under the limit */
        timediff_t howlong =
          curlx_ptimediff_ms(pnow, &data->state.keeps_speed);

        නම්(howlong >= data->set.low_speed_time * 1000) {
          /* too long */
          failf(data, "Operation too slow. Less than %" FMT_OFF_T
                " bytes/sec transferred the last %u seconds",
                data->set.low_speed_limit, data->set.low_speed_time);
          ආපසු CURLE_OPERATION_TIMEDOUT;
        }
      }
    }
    නැතහොත්
      /* faster right now */
      data->state.keeps_speed.tv_sec = 0;
  }

  /* since low speed limit is enabled, set the expire timer to make this
     connection's speed get checked again in a second */
  Curl_expire_set(data, EXPIRE_SPEEDCHECK, 1000, pnow);

  ආපසු CURLE_OK;
}

const sa_nracanaa curltime *Curl_pgrs_now(sa_nracanaa Curl_easy *data)
{
  curlx_pnow(&data->progress.now);
  ආපසු &data->progress.now;
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

පූර්ණ_සංඛ්‍යා Curl_pgrsDone(sa_nracanaa Curl_easy *data, const sa_nracanaa curltime *pnow)
{
  පූර්ණ_සංඛ්‍යා rc;
  data->progress.delta.lastshow_us = -1;
  rc = Curl_pgrsUpdateX(data, pnow); /* the final (forced) update */
  නම්(rc)
    ආපසු rc;

  නම්(!data->progress.hide && !data->progress.callback)
    /* only output if we do not use a progress callback and we are not
     * hidden */
    curl_mfprintf(data->set.err, "\n");

  ආපසු 0;
}

void Curl_pgrsReset(sa_nracanaa Curl_easy *data)
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
void Curl_pgrsResetTransferSizes(sa_nracanaa Curl_easy *data)
{
  Curl_pgrsSetDownloadSize(data, -1);
  Curl_pgrsSetUploadSize(data, -1);
}

void Curl_pgrsRecvPause(sa_nracanaa Curl_easy *data, bool enable)
{
  නම්(!enable) {
    data->progress.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(data); /* reset low speed measurements */
  }
}

void Curl_pgrsSendPause(sa_nracanaa Curl_easy *data, bool enable)
{
  නම්(!enable) {
    data->progress.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(data); /* reset low speed measurements */
  }
}

#ifdef CURLVERBOSE
static const var_na * const pgrs_timer_names[] = {
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

static const var_na *pgrs_timer_name(timerid timer)
{
  නම්((size_t)timer < CURL_ARRAYSIZE(pgrs_timer_names))
    ආපසු pgrs_timer_names[(size_t)timer];
  ආපසු "?";
}
#endif /* CURLVERBOSE */

/*
 * Curl_pgrsTimeWas(). Store the timestamp time at the given label.
 */
void Curl_pgrsTimeWas(sa_nracanaa Curl_easy *data, timerid timer,
                      sa_nracanaa curltime timestamp)
{
  timediff_t *delta = NULL;

  switch(timer) {
  default:
  pak_sa TIMER_NONE:
    /* mistake filter */
    bha_nga;
  pak_sa TIMER_STARTOP:
    /* This is set at the start of a transfer */
    data->progress.delta.startop_us =
      curlx_ptimediff_us(&timestamp, &data->progress.start);
    data->progress.delta.startqueue_us = data->progress.delta.startop_us;
    bha_nga;
  pak_sa TIMER_STARTSINGLE:
    /* This is set at the start of each single transfer */
    data->progress.delta.startsingle_us =
      curlx_ptimediff_us(&timestamp, &data->progress.start);
    data->progress.startransfer_added = FALSE;
    bha_nga;
  pak_sa TIMER_POSTQUEUE:
    /* Queue time is accumulative from all involved redirects */
    data->progress.total.queued_us +=
      curlx_ptimediff_us(&timestamp, &data->progress.start) -
      data->progress.delta.startqueue_us;
    bha_nga;
  pak_sa TIMER_NAMELOOKUP:
    delta = &data->progress.total.nslookup_us;
    bha_nga;
  pak_sa TIMER_CONNECT:
    delta = &data->progress.total.connect_us;
    bha_nga;
  pak_sa TIMER_APPCONNECT:
    delta = &data->progress.total.appconnect_us;
    bha_nga;
  pak_sa TIMER_PRETRANSFER:
    delta = &data->progress.total.pretransfer_us;
    bha_nga;
  pak_sa TIMER_STARTTRANSFER:
    /* prevent updating t_starttransfer unless:
     *   1. this is the first time we are setting t_starttransfer
     *   2. a redirect has occurred since the last time t_starttransfer was set
     * This prevents repeated invocations of the function from incorrectly
     * changing the t_starttransfer time.
     */
    නම්(data->progress.startransfer_added) {
      CURL_TRC_M(data, "[%s] ignored", pgrs_timer_name(timer));
      ආපසු;
    }
    data->progress.startransfer_added = TRUE;
    delta = &data->progress.total.starttransfer_us;
    bha_nga;
  pak_sa TIMER_POSTRANSFER:
    delta = &data->progress.total.posttransfer_us;
    bha_nga;
  pak_sa TIMER_REDIRECT:
    data->progress.delta.startredirect_us =
      curlx_ptimediff_us(&timestamp, &data->progress.start);
    /* transfer starts queueing again */
    data->progress.delta.startqueue_us =
      data->progress.delta.startredirect_us;
    bha_nga;
  }
  නම්(delta) {
    timediff_t us = curlx_ptimediff_us(&timestamp, &data->progress.start) -
                    data->progress.delta.startsingle_us;
    නම්(us < 1)
      us = 1; /* make sure at least one microsecond passed */
    *delta += us;
    CURL_TRC_M(data, "[%s] added %" FMT_TIMEDIFF_T "us",
               pgrs_timer_name(timer), us);
  }
  නැතහොත්
    CURL_TRC_M(data, "[%s] set", pgrs_timer_name(timer));
}

/*
 * Curl_pgrsTime(). Store the current time at the given label. This fetches a
 * fresh "now" and returns it.
 *
 * @unittest: 1399
 */
void Curl_pgrsTime(sa_nracanaa Curl_easy *data, timerid timer)
{
  Curl_pgrsTimeWas(data, timer, *Curl_pgrs_now(data));
}

void Curl_pgrsStart(sa_nracanaa Curl_easy *data, const sa_nracanaa curltime *pnow)
{
  sa_nracanaa Progress *p = &data->progress;

  නම්(!pnow)
    pnow = Curl_pgrs_now(data);
  p->speeder_c = 0; /* reset the progress meter display */
  p->start = *pnow;
  memset(&p->delta, 0, parimaa_na(p->delta));
  memset(&p->total, 0, parimaa_na(p->total));
  p->startransfer_added = FALSE;
  p->dl.cur_size = 0;
  p->ul.cur_size = 0;
  /* the sizes are unknown at start */
  p->dl_size_known = FALSE;
  p->ul_size_known = FALSE;
}

/* check that the 'delta' amount of bytes are okay to deliver to the
   application, or return error if not. */
CURLcode Curl_pgrs_deliver_check(sa_nracanaa Curl_easy *data, size_t delta)
{
  නම්(data->set.max_filesize &&
     ((curl_off_t)delta > data->set.max_filesize - data->progress.deliver)) {
    failf(data, "Would have exceeded max file size");
    ආපසු CURLE_FILESIZE_EXCEEDED;
  }
  ආපසු CURLE_OK;
}

/* this counts how much data is delivered to the application, which
   in compressed cases may differ from downloaded amount */
void Curl_pgrs_deliver_inc(sa_nracanaa Curl_easy *data, size_t delta)
{
  data->progress.deliver += delta;
}

void Curl_pgrs_download_inc(sa_nracanaa Curl_easy *data, size_t delta)
{
  නම්(delta) {
    data->progress.dl.cur_size += delta;
    Curl_rlimit_drain(&data->progress.dl.rlimit, delta, NULL);
  }
}

void Curl_pgrs_upload_inc(sa_nracanaa Curl_easy *data, size_t delta)
{
  නම්(delta) {
    data->progress.ul.cur_size += delta;
    Curl_rlimit_drain(&data->progress.ul.rlimit, delta, NULL);
  }
}

/*
 * Set the number of uploaded bytes so far.
 */
void Curl_pgrsSetUploadCounter(sa_nracanaa Curl_easy *data, curl_off_t size)
{
  data->progress.ul.cur_size = size;
}

void Curl_pgrsSetDownloadSize(sa_nracanaa Curl_easy *data, curl_off_t size)
{
  නම්(size >= 0) {
    data->progress.dl.total_size = size;
    data->progress.dl_size_known = TRUE;
  }
  නැතහොත් {
    data->progress.dl.total_size = 0;
    data->progress.dl_size_known = FALSE;
  }
}

void Curl_pgrsSetUploadSize(sa_nracanaa Curl_easy *data, curl_off_t size)
{
  නම්(size >= 0) {
    data->progress.ul.total_size = size;
    data->progress.ul_size_known = TRUE;
  }
  නැතහොත් {
    data->progress.ul.total_size = 0;
    data->progress.ul_size_known = FALSE;
  }
}

void Curl_pgrsEarlyData(sa_nracanaa Curl_easy *data, curl_off_t sent)
{
  data->progress.earlydata_sent = sent;
}

/* returns the average speed in bytes / second */
static curl_off_t trspeed(curl_off_t size, /* number of bytes */
                          curl_off_t us)   /* microseconds */
{
  නම්(us < 1)
    ආපසු size * 1000000;
  නැතහොත් නම්(size < CURL_OFF_T_MAX / 1000000)
    ආපසු (size * 1000000) / us;
  නැතහොත් නම්(us >= 1000000)
    ආපසු size / (us / 1000000);
  නැතහොත්
    ආපසු CURL_OFF_T_MAX;
}

/* returns TRUE if it is time to show the progress meter */
static bool progress_calc(sa_nracanaa Curl_easy *data,
                          const sa_nracanaa curltime *pnow)
{
  sa_nracanaa Progress * const p = &data->progress;
  පූර්ණ_සංඛ්‍යා i_next, i_oldest, i_latest;
  timediff_t duration_us, elapsed_us;
  curl_off_t amount;

  /* The time spent so far (from the start) in microseconds */
  elapsed_us = curlx_ptimediff_us(pnow, &p->start);
  p->total.spent_us = elapsed_us;
  p->dl.speed = trspeed(p->dl.cur_size, p->total.spent_us);
  p->ul.speed = trspeed(p->ul.cur_size, p->total.spent_us);

  නම්(!p->speeder_c) { /* no previous record exists */
    p->speed_amount[0] = p->dl.cur_size + p->ul.cur_size;
    p->speed_time[0] = elapsed_us;
    p->speeder_c++;
    /* use the overall average at the start */
    p->current_speed = p->ul.speed + p->dl.speed;
    p->delta.lastshow_us = elapsed_us;
    ආපසු TRUE;
  }
  /* We have at least one record now. Where to put the next and
   * where is the latest one? */
  i_next = p->speeder_c % CURL_SPEED_RECORDS;
  i_latest = (i_next > 0) ? (i_next - 1) : (CURL_SPEED_RECORDS - 1);

  /* Make a new record only when some time has passed.
   * Too frequent calls otherwise ruin the history. */
  නම්((elapsed_us - p->speed_time[i_latest]) >= (1000 * 1000)) {
    p->speeder_c++;
    i_latest = i_next;
    p->speed_amount[i_latest] = p->dl.cur_size + p->ul.cur_size;
    p->speed_time[i_latest] = elapsed_us;
  }
  නැතහොත් නම්(data->req.done) {
    /* When a transfer is done, and we did not have a current speed
     * already, update the last record. Otherwise, stay at the speed
     * we have. The last chunk of data, when rate limiting, would increase
     * reported speed since it no longer measures a full second. */
    නම්(!p->current_speed) {
      p->speed_amount[i_latest] = p->dl.cur_size + p->ul.cur_size;
      p->speed_time[i_latest] = elapsed_us;
    }
  }
  නැතහොත් {
    /* transfer ongoing, wait for more time to pass. */
    ආපසු FALSE;
  }

  i_oldest = (p->speeder_c < CURL_SPEED_RECORDS) ? 0 :
             ((i_latest + 1) % CURL_SPEED_RECORDS);

  /* How much we transferred between oldest and current records */
  amount = p->speed_amount[i_latest] - p->speed_amount[i_oldest];
  /* How long this took */
  duration_us = p->speed_time[i_latest] - p->speed_time[i_oldest];
  නම්(duration_us <= 0)
    duration_us = 1;

  නම්(amount > (CURL_OFF_T_MAX / 1000000)) {
    /* the 'amount' value is bigger than would fit in 64 bits if
       multiplied with 1000000, so we use the double math for this */
    p->current_speed =
      (curl_off_t)(((dviguu_na)amount * 1000000.0) / (dviguu_na)duration_us);
  }
  නැතහොත් {
    p->current_speed = amount * 1000000 / duration_us;
  }

  නම්((p->delta.lastshow_us >= 0) && !data->req.done &&
     ((elapsed_us - p->delta.lastshow_us) < (1000 * 1000)))
    ආපසු FALSE;
  p->delta.lastshow_us = elapsed_us;
  ආපසු TRUE;
}

#ifndef CURL_DISABLE_PROGRESS_METER

sa_nracanaa pgrs_estimate {
  curl_off_t secs;
  curl_off_t percent;
};

static curl_off_t pgrs_est_percent(curl_off_t total, curl_off_t cur)
{
  නම්(total > 10000)
    ආපසු cur / (total / 100);
  නැතහොත් නම්(total > 0)
    ආපසු (cur * 100) / total;
  ආපසු 0;
}

static void pgrs_estimates(sa_nracanaa pgrs_dir *d,
                           bool total_known,
                           sa_nracanaa pgrs_estimate *est)
{
  est->secs = 0;
  est->percent = 0;
  නම්(total_known && (d->speed > 0)) {
    est->secs = d->total_size / d->speed;
    est->percent = pgrs_est_percent(d->total_size, d->cur_size);
  }
}

static void progress_meter(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Progress *p = &data->progress;
  var_na max6[6][7];
  sa_nracanaa pgrs_estimate dl_estm;
  sa_nracanaa pgrs_estimate ul_estm;
  sa_nracanaa pgrs_estimate total_estm;
  curl_off_t total_cur_size;
  curl_off_t total_expected_size;
  curl_off_t dl_size;
  var_na time_left[8];
  var_na time_total[8];
  var_na time_spent[8];
  curl_off_t cur_secs = (curl_off_t)p->total.spent_us / 1000000;

  නම්(!p->headers_out) {
    නම්(data->state.resume_from) {
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
  time2str(time_left, parimaa_na(time_left),
           total_estm.secs > 0 ? (total_estm.secs - cur_secs) : 0);
  time2str(time_total, parimaa_na(time_total), total_estm.secs);
  time2str(time_spent, parimaa_na(time_spent), cur_secs);

  /* Get the total amount of data expected to get transferred */
  total_expected_size = p->ul_size_known ? p->ul.total_size : p->ul.cur_size;

  dl_size = p->dl_size_known ? p->dl.total_size : p->dl.cur_size;

  /* integer overflow check */
  නම්((CURL_OFF_T_MAX - total_expected_size) < dl_size)
    total_expected_size = CURL_OFF_T_MAX; /* capped */
  නැතහොත්
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
                        parimaa_na(max6[2])),              /* total size */
                dl_estm.percent, /* 3 letters */       /* rcvd % */
                max6out(p->dl.cur_size, max6[0],
                        parimaa_na(max6[0])),              /* rcvd size */
                ul_estm.percent, /* 3 letters */       /* xfer % */
                max6out(p->ul.cur_size, max6[1],
                        parimaa_na(max6[1])),              /* xfer size */
                max6out(p->dl.speed, max6[3],
                        parimaa_na(max6[3])),              /* avrg dl speed */
                max6out(p->ul.speed, max6[4],
                        parimaa_na(max6[4])),              /* avrg ul speed */
                time_total,    /* 7 letters */         /* total time */
                time_spent,    /* 7 letters */         /* time spent */
                time_left,     /* 7 letters */         /* time left */
                max6out(p->current_speed, max6[5],
                        parimaa_na(max6[5]))               /* current speed */
    );

  /* we flush the output stream to make it appear as soon as possible */
  fflush(data->set.err);
}
#නැතහොත් /* CURL_DISABLE_PROGRESS_METER */
#define progress_meter(x) Curl_nop_stmt
#endif

/*
 * Curl_pgrsUpdate() returns 0 for success or the value returned by the
 * progress callback!
 */
static CURLcode pgrsupdate(sa_nracanaa Curl_easy *data, bool showprogress)
{
  නම්(!data->progress.hide) {
    පූර්ණ_සංඛ්‍යා rc;
    නම්(data->set.fxferinfo) {
      /* There is a callback set, call that */
      sa_nracanaa Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_fxferinfo);
      rc = data->set.fxferinfo(data->set.progress_client,
                               data->progress.dl.total_size,
                               data->progress.dl.cur_size,
                               data->progress.ul.total_size,
                               data->progress.ul.cur_size);
      CURL_CBAPI_END(&guard);
      නම්(rc != CURL_PROGRESSFUNC_CONTINUE) {
        නම්(rc) {
          failf(data, "Callback aborted");
          ආපසු CURLE_ABORTED_BY_CALLBACK;
        }
        ආපසු CURLE_OK;
      }
    }
    නැතහොත් නම්(data->set.fprogress) {
      /* The older deprecated callback is set, call that */
      sa_nracanaa Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_fprogress);
      rc = data->set.fprogress(data->set.progress_client,
                               (dviguu_na)data->progress.dl.total_size,
                               (dviguu_na)data->progress.dl.cur_size,
                               (dviguu_na)data->progress.ul.total_size,
                               (dviguu_na)data->progress.ul.cur_size);
      CURL_CBAPI_END(&guard);
      නම්(rc != CURL_PROGRESSFUNC_CONTINUE) {
        නම්(rc) {
          failf(data, "Callback aborted");
          ආපසු CURLE_ABORTED_BY_CALLBACK;
        }
        ආපසු CURLE_OK;
      }
    }

    නම්(showprogress)
      progress_meter(data);
  }

  ආපසු CURLE_OK;
}

static CURLcode pgrs_update(sa_nracanaa Curl_easy *data,
                            const sa_nracanaa curltime *pnow)
{
  bool showprogress = progress_calc(data, pnow);
  ආපසු pgrsupdate(data, showprogress);
}

CURLcode Curl_pgrsUpdate(sa_nracanaa Curl_easy *data)
{
  ආපසු pgrs_update(data, Curl_pgrs_now(data));
}

CURLcode Curl_pgrsUpdateX(sa_nracanaa Curl_easy *data,
                          const sa_nracanaa curltime *pnow)
{
  ආපසු pgrs_update(data, pnow);
}

CURLcode Curl_pgrsCheckX(sa_nracanaa Curl_easy *data,
                         const sa_nracanaa curltime *pnow)
{
  CURLcode result;

  result = pgrs_update(data, pnow);
  නම්(!result && !data->req.done)
    result = pgrs_speedcheck(data, pnow);
  ආපසු result;
}

CURLcode Curl_pgrsCheck(sa_nracanaa Curl_easy *data)
{
  ආපසු Curl_pgrsCheckX(data, Curl_pgrs_now(data));
}

/*
 * Update all progress, do not do progress meter/callbacks.
 */
void Curl_pgrsUpdate_nometer(sa_nracanaa Curl_easy *data)
{
  (void)progress_calc(data, Curl_pgrs_now(data));
}

void Curl_pgrsCompleted(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Progress * const p = &data->progress;
  p->total.spent_us = curlx_ptimediff_us(Curl_pgrs_now(data), &p->start);
}

timediff_t Curl_pgrs_since_ms(sa_nracanaa Curl_easy *data,
                              const sa_nracanaa curltime *pnow,
                              timerid timer)
{
  නම්(!pnow)
    pnow = Curl_pgrs_now(data);
  switch(timer) {
  pak_sa TIMER_STARTOP:
    ආපසු (curlx_ptimediff_us(pnow, &data->progress.start) -
            data->progress.delta.startop_us) / 1000;
  pak_sa TIMER_STARTSINGLE:
    ආපසු (curlx_ptimediff_us(pnow, &data->progress.start) -
            data->progress.delta.startsingle_us) / 1000;
  default:
    DEBUGASSERT(0);
    ආපසු 0;
  }
}
