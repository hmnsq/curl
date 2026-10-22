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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "curlx/strcopy.h"

#ifndef CURL_DISABLE_PROGRESS_METER
/* Provide a string that is 7 letters long (plus the zero byte).

   @unittest 1636 */
UNITTEST வெறுமை time2str(எழுத்து *r, size_t rsize, curl_off_t seconds);
UNITTEST வெறுமை time2str(எழுத்து *r, size_t rsize, curl_off_t seconds)
{
  curl_off_t h;
  எனில்(seconds <= 0) {
    curlx_strcopy(r, rsize, STRCONST("       "));
    திரும்பு;
  }
  h = seconds / 3600;
  எனில்(h <= 99) {
    curl_off_t m = (seconds - (h * 3600)) / 60;
    எனில்(h <= 9) {
      curl_off_t s = (seconds - (h * 3600)) - (m * 60);
      எனில்(h)
        curl_msnprintf(r, rsize, "%" FMT_OFF_T ":%02" FMT_OFF_T ":"
                       "%02" FMT_OFF_T, h, m, s);
      இல்லையெனில்
        curl_msnprintf(r, rsize, "  %02" FMT_OFF_T ":%02" FMT_OFF_T, m, s);
    }
    இல்லையெனில்
      curl_msnprintf(r, rsize, "%" FMT_OFF_T "h %02" FMT_OFF_T "m", h, m);
  }
  இல்லையெனில் {
    curl_off_t d = seconds / 86400;
    h = (seconds - (d * 86400)) / 3600;
    எனில்(d <= 99)
      curl_msnprintf(r, rsize, "%2" FMT_OFF_T "d %02" FMT_OFF_T "h", d, h);
    இல்லையெனில் எனில்(d <= 999)
      curl_msnprintf(r, rsize, "%6" FMT_OFF_T "d", d);
    இல்லையெனில் { /* more than 999 days */
      curl_off_t m = d / 30;
      எனில்(m <= 999)
        curl_msnprintf(r, rsize, "%6" FMT_OFF_T "m", m);
      இல்லையெனில் { /* more than 999 months */
        curl_off_t y = d / 365;
        எனில்(y <= 99999)
          curl_msnprintf(r, rsize, "%6" FMT_OFF_T "y", y);
        இல்லையெனில்
          curlx_strcopy(r, rsize, STRCONST(">99999y"));
      }
    }
  }
}

/* The point of this function would be to return a string of the input data,
   but never longer than 6 columns (+ one zero byte).
   Add suffix k, M, G when suitable...

   @unittest 1636 */
UNITTEST எழுத்து *max6out(curl_off_t bytes, எழுத்து *max6, size_t mlen);
UNITTEST எழுத்து *max6out(curl_off_t bytes, எழுத்து *max6, size_t mlen)
{
  /* a signed 64-bit value is 8192 petabytes maximum, shown as
     8.0E (exabytes)*/
  எனில்(bytes < 100000)
    curl_msnprintf(max6, mlen, "%6" CURL_FORMAT_CURL_OFF_T, bytes);
  இல்லையெனில் {
    நிலையான மாறிலி எழுத்து unit[] = { 'k', 'M', 'G', 'T', 'P', 'E', 0 };
    முழு k = 0;
    curl_off_t nbytes;
    curl_off_t rest;
    செய் {
      nbytes = bytes / 1024;
      எனில்(nbytes < 1000)
        நிறுத்து;
      bytes = nbytes;
      k++;
      DEBUGASSERT(unit[k]);
    } போது(unit[k]);
    rest = bytes % 1024;
    எனில்(nbytes <= 99)
      /* xx.yyU */
      curl_msnprintf(max6, mlen, "%2" CURL_FORMAT_CURL_OFF_T
                     ".%02" CURL_FORMAT_CURL_OFF_T "%c", nbytes,
                     rest * 100 / 1024, unit[k]);
    இல்லையெனில்
      /* xxx.yU */
      curl_msnprintf(max6, mlen, "%3" CURL_FORMAT_CURL_OFF_T
                     ".%" CURL_FORMAT_CURL_OFF_T "%c", nbytes,
                     rest * 10 / 1024, unit[k]);
  }
  திரும்பு max6;
}
#endif

நிலையான வெறுமை pgrs_speedinit(கட்டமைப்பு Curl_easy *data)
{
  memset(&data->state.keeps_speed, 0, அளவு(கட்டமைப்பு curltime));
}

/*
 * @unittest 1606
 */
UNITTEST CURLcode pgrs_speedcheck(கட்டமைப்பு Curl_easy *data,
                                  மாறிலி கட்டமைப்பு curltime *pnow);
UNITTEST CURLcode pgrs_speedcheck(கட்டமைப்பு Curl_easy *data,
                                  மாறிலி கட்டமைப்பு curltime *pnow)
{
  எனில்(!data->set.கீழ்_speed_time || !data->set.கீழ்_speed_limit ||
     Curl_xfer_recv_is_paused(data) || Curl_xfer_send_is_paused(data))
    /* A paused transfer is not qualified for speed checks */
    திரும்பு CURLE_OK;

  எனில்(data->progress.current_speed >= 0) {
    எனில்(data->progress.current_speed < data->set.கீழ்_speed_limit) {
      எனில்(!data->state.keeps_speed.tv_sec)
        /* under the limit at this moment */
        data->state.keeps_speed = *pnow;
      இல்லையெனில் {
        /* how long has it been under the limit */
        timediff_t howlong =
          curlx_ptimediff_ms(pnow, &data->state.keeps_speed);

        எனில்(howlong >= data->set.கீழ்_speed_time * 1000) {
          /* too long */
          failf(data, "Operation too slow. Less than %" FMT_OFF_T
                " bytes/sec transferred the last %u seconds",
                data->set.கீழ்_speed_limit, data->set.கீழ்_speed_time);
          திரும்பு CURLE_OPERATION_TIMEDOUT;
        }
      }
    }
    இல்லையெனில்
      /* faster right now */
      data->state.keeps_speed.tv_sec = 0;
  }

  /* since low speed limit is enabled, set the expire timer to make this
     connection's speed get checked again in a second */
  Curl_expire_set(data, EXPIRE_SPEEDCHECK, 1000, pnow);

  திரும்பு CURLE_OK;
}

மாறிலி கட்டமைப்பு curltime *Curl_pgrs_now(கட்டமைப்பு Curl_easy *data)
{
  curlx_pnow(&data->progress.now);
  திரும்பு &data->progress.now;
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

முழு Curl_pgrsDone(கட்டமைப்பு Curl_easy *data, மாறிலி கட்டமைப்பு curltime *pnow)
{
  முழு rc;
  data->progress.delta.lastshow_us = -1;
  rc = Curl_pgrsUpdateX(data, pnow); /* the final (forced) update */
  எனில்(rc)
    திரும்பு rc;

  எனில்(!data->progress.hide && !data->progress.callback)
    /* only output if we do not use a progress callback and we are not
     * hidden */
    curl_mfprintf(data->set.err, "\n");

  திரும்பு 0;
}

வெறுமை Curl_pgrsReset(கட்டமைப்பு Curl_easy *data)
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
வெறுமை Curl_pgrsResetTransferSizes(கட்டமைப்பு Curl_easy *data)
{
  Curl_pgrsSetDownloadSize(data, -1);
  Curl_pgrsSetUploadSize(data, -1);
}

வெறுமை Curl_pgrsRecvPause(கட்டமைப்பு Curl_easy *data, bool enable)
{
  எனில்(!enable) {
    data->progress.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(data); /* reset low speed measurements */
  }
}

வெறுமை Curl_pgrsSendPause(கட்டமைப்பு Curl_easy *data, bool enable)
{
  எனில்(!enable) {
    data->progress.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(data); /* reset low speed measurements */
  }
}

#ifdef CURLVERBOSE
நிலையான மாறிலி எழுத்து * மாறிலி pgrs_timer_names[] = {
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

நிலையான மாறிலி எழுத்து *pgrs_timer_name(timerid timer)
{
  எனில்((size_t)timer < CURL_ARRAYSIZE(pgrs_timer_names))
    திரும்பு pgrs_timer_names[(size_t)timer];
  திரும்பு "?";
}
#endif /* CURLVERBOSE */

/*
 * Curl_pgrsTimeWas(). Store the timestamp time at the given label.
 */
வெறுமை Curl_pgrsTimeWas(கட்டமைப்பு Curl_easy *data, timerid timer,
                      கட்டமைப்பு curltime timestamp)
{
  timediff_t *delta = வெற்று;

  தேர்வு(timer) {
  இயல்பு:
  நிலை TIMER_NONE:
    /* mistake filter */
    நிறுத்து;
  நிலை TIMER_STARTOP:
    /* This is set at the start of a transfer */
    data->progress.delta.startop_us =
      curlx_ptimediff_us(&timestamp, &data->progress.start);
    data->progress.delta.startqueue_us = data->progress.delta.startop_us;
    நிறுத்து;
  நிலை TIMER_STARTSINGLE:
    /* This is set at the start of each single transfer */
    data->progress.delta.startsingle_us =
      curlx_ptimediff_us(&timestamp, &data->progress.start);
    data->progress.startransfer_added = FALSE;
    நிறுத்து;
  நிலை TIMER_POSTQUEUE:
    /* Queue time is accumulative from all involved redirects */
    data->progress.total.queued_us +=
      curlx_ptimediff_us(&timestamp, &data->progress.start) -
      data->progress.delta.startqueue_us;
    நிறுத்து;
  நிலை TIMER_NAMELOOKUP:
    delta = &data->progress.total.nslookup_us;
    நிறுத்து;
  நிலை TIMER_CONNECT:
    delta = &data->progress.total.connect_us;
    நிறுத்து;
  நிலை TIMER_APPCONNECT:
    delta = &data->progress.total.appconnect_us;
    நிறுத்து;
  நிலை TIMER_PRETRANSFER:
    delta = &data->progress.total.pretransfer_us;
    நிறுத்து;
  நிலை TIMER_STARTTRANSFER:
    /* prevent updating t_starttransfer unless:
     *   1. this is the first time we are setting t_starttransfer
     *   2. a redirect has occurred since the last time t_starttransfer was set
     * This prevents repeated invocations of the function from incorrectly
     * changing the t_starttransfer time.
     */
    எனில்(data->progress.startransfer_added) {
      CURL_TRC_M(data, "[%s] ignored", pgrs_timer_name(timer));
      திரும்பு;
    }
    data->progress.startransfer_added = TRUE;
    delta = &data->progress.total.starttransfer_us;
    நிறுத்து;
  நிலை TIMER_POSTRANSFER:
    delta = &data->progress.total.posttransfer_us;
    நிறுத்து;
  நிலை TIMER_REDIRECT:
    data->progress.delta.startredirect_us =
      curlx_ptimediff_us(&timestamp, &data->progress.start);
    /* transfer starts queueing again */
    data->progress.delta.startqueue_us =
      data->progress.delta.startredirect_us;
    நிறுத்து;
  }
  எனில்(delta) {
    timediff_t us = curlx_ptimediff_us(&timestamp, &data->progress.start) -
                    data->progress.delta.startsingle_us;
    எனில்(us < 1)
      us = 1; /* make sure at least one microsecond passed */
    *delta += us;
    CURL_TRC_M(data, "[%s] added %" FMT_TIMEDIFF_T "us",
               pgrs_timer_name(timer), us);
  }
  இல்லையெனில்
    CURL_TRC_M(data, "[%s] set", pgrs_timer_name(timer));
}

/*
 * Curl_pgrsTime(). Store the current time at the given label. This fetches a
 * fresh "now" and returns it.
 *
 * @unittest: 1399
 */
வெறுமை Curl_pgrsTime(கட்டமைப்பு Curl_easy *data, timerid timer)
{
  Curl_pgrsTimeWas(data, timer, *Curl_pgrs_now(data));
}

வெறுமை Curl_pgrsStart(கட்டமைப்பு Curl_easy *data, மாறிலி கட்டமைப்பு curltime *pnow)
{
  கட்டமைப்பு Progress *p = &data->progress;

  எனில்(!pnow)
    pnow = Curl_pgrs_now(data);
  p->speeder_c = 0; /* reset the progress meter display */
  p->start = *pnow;
  memset(&p->delta, 0, அளவு(p->delta));
  memset(&p->total, 0, அளவு(p->total));
  p->startransfer_added = FALSE;
  p->dl.cur_size = 0;
  p->ul.cur_size = 0;
  /* the sizes are unknown at start */
  p->dl_size_known = FALSE;
  p->ul_size_known = FALSE;
}

/* check that the 'delta' amount of bytes are okay to deliver to the
   application, or return error if not. */
CURLcode Curl_pgrs_deliver_check(கட்டமைப்பு Curl_easy *data, size_t delta)
{
  எனில்(data->set.max_filesize &&
     ((curl_off_t)delta > data->set.max_filesize - data->progress.deliver)) {
    failf(data, "Would have exceeded max file size");
    திரும்பு CURLE_FILESIZE_EXCEEDED;
  }
  திரும்பு CURLE_OK;
}

/* this counts how much data is delivered to the application, which
   in compressed cases may differ from downloaded amount */
வெறுமை Curl_pgrs_deliver_inc(கட்டமைப்பு Curl_easy *data, size_t delta)
{
  data->progress.deliver += delta;
}

வெறுமை Curl_pgrs_download_inc(கட்டமைப்பு Curl_easy *data, size_t delta)
{
  எனில்(delta) {
    data->progress.dl.cur_size += delta;
    Curl_rlimit_drain(&data->progress.dl.rlimit, delta, வெற்று);
  }
}

வெறுமை Curl_pgrs_upload_inc(கட்டமைப்பு Curl_easy *data, size_t delta)
{
  எனில்(delta) {
    data->progress.ul.cur_size += delta;
    Curl_rlimit_drain(&data->progress.ul.rlimit, delta, வெற்று);
  }
}

/*
 * Set the number of uploaded bytes so far.
 */
வெறுமை Curl_pgrsSetUploadCounter(கட்டமைப்பு Curl_easy *data, curl_off_t size)
{
  data->progress.ul.cur_size = size;
}

வெறுமை Curl_pgrsSetDownloadSize(கட்டமைப்பு Curl_easy *data, curl_off_t size)
{
  எனில்(size >= 0) {
    data->progress.dl.total_size = size;
    data->progress.dl_size_known = TRUE;
  }
  இல்லையெனில் {
    data->progress.dl.total_size = 0;
    data->progress.dl_size_known = FALSE;
  }
}

வெறுமை Curl_pgrsSetUploadSize(கட்டமைப்பு Curl_easy *data, curl_off_t size)
{
  எனில்(size >= 0) {
    data->progress.ul.total_size = size;
    data->progress.ul_size_known = TRUE;
  }
  இல்லையெனில் {
    data->progress.ul.total_size = 0;
    data->progress.ul_size_known = FALSE;
  }
}

வெறுமை Curl_pgrsEarlyData(கட்டமைப்பு Curl_easy *data, curl_off_t sent)
{
  data->progress.earlydata_sent = sent;
}

/* returns the average speed in bytes / second */
நிலையான curl_off_t trspeed(curl_off_t size, /* number of bytes */
                          curl_off_t us)   /* microseconds */
{
  எனில்(us < 1)
    திரும்பு size * 1000000;
  இல்லையெனில் எனில்(size < CURL_OFF_T_MAX / 1000000)
    திரும்பு (size * 1000000) / us;
  இல்லையெனில் எனில்(us >= 1000000)
    திரும்பு size / (us / 1000000);
  இல்லையெனில்
    திரும்பு CURL_OFF_T_MAX;
}

/* returns TRUE if it is time to show the progress meter */
நிலையான bool progress_calc(கட்டமைப்பு Curl_easy *data,
                          மாறிலி கட்டமைப்பு curltime *pnow)
{
  கட்டமைப்பு Progress * மாறிலி p = &data->progress;
  முழு i_next, i_oldest, i_latest;
  timediff_t duration_us, elapsed_us;
  curl_off_t amount;

  /* The time spent so far (from the start) in microseconds */
  elapsed_us = curlx_ptimediff_us(pnow, &p->start);
  p->total.spent_us = elapsed_us;
  p->dl.speed = trspeed(p->dl.cur_size, p->total.spent_us);
  p->ul.speed = trspeed(p->ul.cur_size, p->total.spent_us);

  எனில்(!p->speeder_c) { /* no previous record exists */
    p->speed_amount[0] = p->dl.cur_size + p->ul.cur_size;
    p->speed_time[0] = elapsed_us;
    p->speeder_c++;
    /* use the overall average at the start */
    p->current_speed = p->ul.speed + p->dl.speed;
    p->delta.lastshow_us = elapsed_us;
    திரும்பு TRUE;
  }
  /* We have at least one record now. Where to put the next and
   * where is the latest one? */
  i_next = p->speeder_c % CURL_SPEED_RECORDS;
  i_latest = (i_next > 0) ? (i_next - 1) : (CURL_SPEED_RECORDS - 1);

  /* Make a new record only when some time has passed.
   * Too frequent calls otherwise ruin the history. */
  எனில்((elapsed_us - p->speed_time[i_latest]) >= (1000 * 1000)) {
    p->speeder_c++;
    i_latest = i_next;
    p->speed_amount[i_latest] = p->dl.cur_size + p->ul.cur_size;
    p->speed_time[i_latest] = elapsed_us;
  }
  இல்லையெனில் எனில்(data->req.done) {
    /* When a transfer is done, and we did not have a current speed
     * already, update the last record. Otherwise, stay at the speed
     * we have. The last chunk of data, when rate limiting, would increase
     * reported speed since it no longer measures a full second. */
    எனில்(!p->current_speed) {
      p->speed_amount[i_latest] = p->dl.cur_size + p->ul.cur_size;
      p->speed_time[i_latest] = elapsed_us;
    }
  }
  இல்லையெனில் {
    /* transfer ongoing, wait for more time to pass. */
    திரும்பு FALSE;
  }

  i_oldest = (p->speeder_c < CURL_SPEED_RECORDS) ? 0 :
             ((i_latest + 1) % CURL_SPEED_RECORDS);

  /* How much we transferred between oldest and current records */
  amount = p->speed_amount[i_latest] - p->speed_amount[i_oldest];
  /* How long this took */
  duration_us = p->speed_time[i_latest] - p->speed_time[i_oldest];
  எனில்(duration_us <= 0)
    duration_us = 1;

  எனில்(amount > (CURL_OFF_T_MAX / 1000000)) {
    /* the 'amount' value is bigger than would fit in 64 bits if
       multiplied with 1000000, so we use the double math for this */
    p->current_speed =
      (curl_off_t)(((இரட்டை)amount * 1000000.0) / (இரட்டை)duration_us);
  }
  இல்லையெனில் {
    p->current_speed = amount * 1000000 / duration_us;
  }

  எனில்((p->delta.lastshow_us >= 0) && !data->req.done &&
     ((elapsed_us - p->delta.lastshow_us) < (1000 * 1000)))
    திரும்பு FALSE;
  p->delta.lastshow_us = elapsed_us;
  திரும்பு TRUE;
}

#ifndef CURL_DISABLE_PROGRESS_METER

கட்டமைப்பு pgrs_estimate {
  curl_off_t secs;
  curl_off_t percent;
};

நிலையான curl_off_t pgrs_est_percent(curl_off_t total, curl_off_t cur)
{
  எனில்(total > 10000)
    திரும்பு cur / (total / 100);
  இல்லையெனில் எனில்(total > 0)
    திரும்பு (cur * 100) / total;
  திரும்பு 0;
}

நிலையான வெறுமை pgrs_estimates(கட்டமைப்பு pgrs_dir *d,
                           bool total_known,
                           கட்டமைப்பு pgrs_estimate *est)
{
  est->secs = 0;
  est->percent = 0;
  எனில்(total_known && (d->speed > 0)) {
    est->secs = d->total_size / d->speed;
    est->percent = pgrs_est_percent(d->total_size, d->cur_size);
  }
}

நிலையான வெறுமை progress_meter(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Progress *p = &data->progress;
  எழுத்து max6[6][7];
  கட்டமைப்பு pgrs_estimate dl_estm;
  கட்டமைப்பு pgrs_estimate ul_estm;
  கட்டமைப்பு pgrs_estimate total_estm;
  curl_off_t total_cur_size;
  curl_off_t total_expected_size;
  curl_off_t dl_size;
  எழுத்து time_இடது[8];
  எழுத்து time_total[8];
  எழுத்து time_spent[8];
  curl_off_t cur_secs = (curl_off_t)p->total.spent_us / 1000000;

  எனில்(!p->headers_out) {
    எனில்(data->state.resume_from) {
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
  time2str(time_இடது, அளவு(time_இடது),
           total_estm.secs > 0 ? (total_estm.secs - cur_secs) : 0);
  time2str(time_total, அளவு(time_total), total_estm.secs);
  time2str(time_spent, அளவு(time_spent), cur_secs);

  /* Get the total amount of data expected to get transferred */
  total_expected_size = p->ul_size_known ? p->ul.total_size : p->ul.cur_size;

  dl_size = p->dl_size_known ? p->dl.total_size : p->dl.cur_size;

  /* integer overflow check */
  எனில்((CURL_OFF_T_MAX - total_expected_size) < dl_size)
    total_expected_size = CURL_OFF_T_MAX; /* capped */
  இல்லையெனில்
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
                        அளவு(max6[2])),              /* total size */
                dl_estm.percent, /* 3 letters */       /* rcvd % */
                max6out(p->dl.cur_size, max6[0],
                        அளவு(max6[0])),              /* rcvd size */
                ul_estm.percent, /* 3 letters */       /* xfer % */
                max6out(p->ul.cur_size, max6[1],
                        அளவு(max6[1])),              /* xfer size */
                max6out(p->dl.speed, max6[3],
                        அளவு(max6[3])),              /* avrg dl speed */
                max6out(p->ul.speed, max6[4],
                        அளவு(max6[4])),              /* avrg ul speed */
                time_total,    /* 7 letters */         /* total time */
                time_spent,    /* 7 letters */         /* time spent */
                time_இடது,     /* 7 letters */         /* time left */
                max6out(p->current_speed, max6[5],
                        அளவு(max6[5]))               /* current speed */
    );

  /* we flush the output stream to make it appear as soon as possible */
  fflush(data->set.err);
}
#இல்லையெனில் /* CURL_DISABLE_PROGRESS_METER */
#define progress_meter(x) Curl_nop_stmt
#endif

/*
 * Curl_pgrsUpdate() returns 0 for success or the value returned by the
 * progress callback!
 */
நிலையான CURLcode pgrsupdate(கட்டமைப்பு Curl_easy *data, bool showprogress)
{
  எனில்(!data->progress.hide) {
    முழு rc;
    எனில்(data->set.fxferinfo) {
      /* There is a callback set, call that */
      கட்டமைப்பு Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_fxferinfo);
      rc = data->set.fxferinfo(data->set.progress_client,
                               data->progress.dl.total_size,
                               data->progress.dl.cur_size,
                               data->progress.ul.total_size,
                               data->progress.ul.cur_size);
      CURL_CBAPI_END(&guard);
      எனில்(rc != CURL_PROGRESSFUNC_CONTINUE) {
        எனில்(rc) {
          failf(data, "Callback aborted");
          திரும்பு CURLE_ABORTED_BY_CALLBACK;
        }
        திரும்பு CURLE_OK;
      }
    }
    இல்லையெனில் எனில்(data->set.fprogress) {
      /* The older deprecated callback is set, call that */
      கட்டமைப்பு Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_fprogress);
      rc = data->set.fprogress(data->set.progress_client,
                               (இரட்டை)data->progress.dl.total_size,
                               (இரட்டை)data->progress.dl.cur_size,
                               (இரட்டை)data->progress.ul.total_size,
                               (இரட்டை)data->progress.ul.cur_size);
      CURL_CBAPI_END(&guard);
      எனில்(rc != CURL_PROGRESSFUNC_CONTINUE) {
        எனில்(rc) {
          failf(data, "Callback aborted");
          திரும்பு CURLE_ABORTED_BY_CALLBACK;
        }
        திரும்பு CURLE_OK;
      }
    }

    எனில்(showprogress)
      progress_meter(data);
  }

  திரும்பு CURLE_OK;
}

நிலையான CURLcode pgrs_update(கட்டமைப்பு Curl_easy *data,
                            மாறிலி கட்டமைப்பு curltime *pnow)
{
  bool showprogress = progress_calc(data, pnow);
  திரும்பு pgrsupdate(data, showprogress);
}

CURLcode Curl_pgrsUpdate(கட்டமைப்பு Curl_easy *data)
{
  திரும்பு pgrs_update(data, Curl_pgrs_now(data));
}

CURLcode Curl_pgrsUpdateX(கட்டமைப்பு Curl_easy *data,
                          மாறிலி கட்டமைப்பு curltime *pnow)
{
  திரும்பு pgrs_update(data, pnow);
}

CURLcode Curl_pgrsCheckX(கட்டமைப்பு Curl_easy *data,
                         மாறிலி கட்டமைப்பு curltime *pnow)
{
  CURLcode result;

  result = pgrs_update(data, pnow);
  எனில்(!result && !data->req.done)
    result = pgrs_speedcheck(data, pnow);
  திரும்பு result;
}

CURLcode Curl_pgrsCheck(கட்டமைப்பு Curl_easy *data)
{
  திரும்பு Curl_pgrsCheckX(data, Curl_pgrs_now(data));
}

/*
 * Update all progress, do not do progress meter/callbacks.
 */
வெறுமை Curl_pgrsUpdate_nometer(கட்டமைப்பு Curl_easy *data)
{
  (வெறுமை)progress_calc(data, Curl_pgrs_now(data));
}

வெறுமை Curl_pgrsCompleted(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Progress * மாறிலி p = &data->progress;
  p->total.spent_us = curlx_ptimediff_us(Curl_pgrs_now(data), &p->start);
}

timediff_t Curl_pgrs_since_ms(கட்டமைப்பு Curl_easy *data,
                              மாறிலி கட்டமைப்பு curltime *pnow,
                              timerid timer)
{
  எனில்(!pnow)
    pnow = Curl_pgrs_now(data);
  தேர்வு(timer) {
  நிலை TIMER_STARTOP:
    திரும்பு (curlx_ptimediff_us(pnow, &data->progress.start) -
            data->progress.delta.startop_us) / 1000;
  நிலை TIMER_STARTSINGLE:
    திரும்பு (curlx_ptimediff_us(pnow, &data->progress.start) -
            data->progress.delta.startsingle_us) / 1000;
  இயல்பு:
    DEBUGASSERT(0);
    திரும்பு 0;
  }
}
