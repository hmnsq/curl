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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "curlx/strcopy.h"

#ifndef CURL_DISABLE_PROGRESS_METER
/* Provide a string that is 7 letters long (plus the zero byte).

   @unittest 1636 */
UNITTEST ശൂന്യം time2str(അക്ഷരം *r, size_t rsize, curl_off_t seconds);
UNITTEST ശൂന്യം time2str(അക്ഷരം *r, size_t rsize, curl_off_t seconds)
{
  curl_off_t h;
  എങ്കിൽ(seconds <= 0) {
    curlx_strcopy(r, rsize, STRCONST("       "));
    തിരികെ;
  }
  h = seconds / 3600;
  എങ്കിൽ(h <= 99) {
    curl_off_t m = (seconds - (h * 3600)) / 60;
    എങ്കിൽ(h <= 9) {
      curl_off_t s = (seconds - (h * 3600)) - (m * 60);
      എങ്കിൽ(h)
        curl_msnprintf(r, rsize, "%" FMT_OFF_T ":%02" FMT_OFF_T ":"
                       "%02" FMT_OFF_T, h, m, s);
      അല്ലെങ്കിൽ
        curl_msnprintf(r, rsize, "  %02" FMT_OFF_T ":%02" FMT_OFF_T, m, s);
    }
    അല്ലെങ്കിൽ
      curl_msnprintf(r, rsize, "%" FMT_OFF_T "h %02" FMT_OFF_T "m", h, m);
  }
  അല്ലെങ്കിൽ {
    curl_off_t d = seconds / 86400;
    h = (seconds - (d * 86400)) / 3600;
    എങ്കിൽ(d <= 99)
      curl_msnprintf(r, rsize, "%2" FMT_OFF_T "d %02" FMT_OFF_T "h", d, h);
    അല്ലെങ്കിൽ എങ്കിൽ(d <= 999)
      curl_msnprintf(r, rsize, "%6" FMT_OFF_T "d", d);
    അല്ലെങ്കിൽ { /* more than 999 days */
      curl_off_t m = d / 30;
      എങ്കിൽ(m <= 999)
        curl_msnprintf(r, rsize, "%6" FMT_OFF_T "m", m);
      അല്ലെങ്കിൽ { /* more than 999 months */
        curl_off_t y = d / 365;
        എങ്കിൽ(y <= 99999)
          curl_msnprintf(r, rsize, "%6" FMT_OFF_T "y", y);
        അല്ലെങ്കിൽ
          curlx_strcopy(r, rsize, STRCONST(">99999y"));
      }
    }
  }
}

/* The point of this function would be to return a string of the input data,
   but never longer than 6 columns (+ one zero byte).
   Add suffix k, M, G when suitable...

   @unittest 1636 */
UNITTEST അക്ഷരം *max6out(curl_off_t bytes, അക്ഷരം *max6, size_t mlen);
UNITTEST അക്ഷരം *max6out(curl_off_t bytes, അക്ഷരം *max6, size_t mlen)
{
  /* a signed 64-bit value is 8192 petabytes maximum, shown as
     8.0E (exabytes)*/
  എങ്കിൽ(bytes < 100000)
    curl_msnprintf(max6, mlen, "%6" CURL_FORMAT_CURL_OFF_T, bytes);
  അല്ലെങ്കിൽ {
    സ്ഥിതം സ്ഥിരം അക്ഷരം unit[] = { 'k', 'M', 'G', 'T', 'P', 'E', 0 };
    പൂർണ്ണസംഖ്യ k = 0;
    curl_off_t nbytes;
    curl_off_t rest;
    ചെയ്യുക {
      nbytes = bytes / 1024;
      എങ്കിൽ(nbytes < 1000)
        നിർത്തുക;
      bytes = nbytes;
      k++;
      DEBUGASSERT(unit[k]);
    } ആകുമ്പോൾ(unit[k]);
    rest = bytes % 1024;
    എങ്കിൽ(nbytes <= 99)
      /* xx.yyU */
      curl_msnprintf(max6, mlen, "%2" CURL_FORMAT_CURL_OFF_T
                     ".%02" CURL_FORMAT_CURL_OFF_T "%c", nbytes,
                     rest * 100 / 1024, unit[k]);
    അല്ലെങ്കിൽ
      /* xxx.yU */
      curl_msnprintf(max6, mlen, "%3" CURL_FORMAT_CURL_OFF_T
                     ".%" CURL_FORMAT_CURL_OFF_T "%c", nbytes,
                     rest * 10 / 1024, unit[k]);
  }
  തിരികെ max6;
}
#endif

സ്ഥിതം ശൂന്യം pgrs_speedinit(ഘടന Curl_easy *data)
{
  memset(&data->state.keeps_speed, 0, വലുപ്പം(ഘടന curltime));
}

/*
 * @unittest 1606
 */
UNITTEST CURLcode pgrs_speedcheck(ഘടന Curl_easy *data,
                                  സ്ഥിരം ഘടന curltime *pnow);
UNITTEST CURLcode pgrs_speedcheck(ഘടന Curl_easy *data,
                                  സ്ഥിരം ഘടന curltime *pnow)
{
  എങ്കിൽ(!data->set.താഴെയുള്ള_speed_time || !data->set.താഴെയുള്ള_speed_limit ||
     Curl_xfer_recv_is_paused(data) || Curl_xfer_send_is_paused(data))
    /* A paused transfer is not qualified for speed checks */
    തിരികെ CURLE_OK;

  എങ്കിൽ(data->progress.current_speed >= 0) {
    എങ്കിൽ(data->progress.current_speed < data->set.താഴെയുള്ള_speed_limit) {
      എങ്കിൽ(!data->state.keeps_speed.tv_sec)
        /* under the limit at this moment */
        data->state.keeps_speed = *pnow;
      അല്ലെങ്കിൽ {
        /* how long has it been under the limit */
        timediff_t howlong =
          curlx_ptimediff_ms(pnow, &data->state.keeps_speed);

        എങ്കിൽ(howlong >= data->set.താഴെയുള്ള_speed_time * 1000) {
          /* too long */
          failf(data, "Operation too slow. Less than %" FMT_OFF_T
                " bytes/sec transferred the last %u seconds",
                data->set.താഴെയുള്ള_speed_limit, data->set.താഴെയുള്ള_speed_time);
          തിരികെ CURLE_OPERATION_TIMEDOUT;
        }
      }
    }
    അല്ലെങ്കിൽ
      /* faster right now */
      data->state.keeps_speed.tv_sec = 0;
  }

  /* since low speed limit is enabled, set the expire timer to make this
     connection's speed get checked again in a second */
  Curl_expire_set(data, EXPIRE_SPEEDCHECK, 1000, pnow);

  തിരികെ CURLE_OK;
}

സ്ഥിരം ഘടന curltime *Curl_pgrs_now(ഘടന Curl_easy *data)
{
  curlx_pnow(&data->progress.now);
  തിരികെ &data->progress.now;
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

പൂർണ്ണസംഖ്യ Curl_pgrsDone(ഘടന Curl_easy *data, സ്ഥിരം ഘടന curltime *pnow)
{
  പൂർണ്ണസംഖ്യ rc;
  data->progress.delta.lastshow_us = -1;
  rc = Curl_pgrsUpdateX(data, pnow); /* the final (forced) update */
  എങ്കിൽ(rc)
    തിരികെ rc;

  എങ്കിൽ(!data->progress.hide && !data->progress.callback)
    /* only output if we do not use a progress callback and we are not
     * hidden */
    curl_mfprintf(data->set.err, "\n");

  തിരികെ 0;
}

ശൂന്യം Curl_pgrsReset(ഘടന Curl_easy *data)
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
ശൂന്യം Curl_pgrsResetTransferSizes(ഘടന Curl_easy *data)
{
  Curl_pgrsSetDownloadSize(data, -1);
  Curl_pgrsSetUploadSize(data, -1);
}

ശൂന്യം Curl_pgrsRecvPause(ഘടന Curl_easy *data, bool enable)
{
  എങ്കിൽ(!enable) {
    data->progress.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(data); /* reset low speed measurements */
  }
}

ശൂന്യം Curl_pgrsSendPause(ഘടന Curl_easy *data, bool enable)
{
  എങ്കിൽ(!enable) {
    data->progress.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(data); /* reset low speed measurements */
  }
}

#ifdef CURLVERBOSE
സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം pgrs_timer_names[] = {
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

സ്ഥിതം സ്ഥിരം അക്ഷരം *pgrs_timer_name(timerid timer)
{
  എങ്കിൽ((size_t)timer < CURL_ARRAYSIZE(pgrs_timer_names))
    തിരികെ pgrs_timer_names[(size_t)timer];
  തിരികെ "?";
}
#endif /* CURLVERBOSE */

/*
 * Curl_pgrsTimeWas(). Store the timestamp time at the given label.
 */
ശൂന്യം Curl_pgrsTimeWas(ഘടന Curl_easy *data, timerid timer,
                      ഘടന curltime timestamp)
{
  timediff_t *delta = ശൂന്യ;

  തിരഞ്ഞെടുപ്പ്(timer) {
  സ്ഥിരസ്ഥിതി:
  അവസ്ഥ TIMER_NONE:
    /* mistake filter */
    നിർത്തുക;
  അവസ്ഥ TIMER_STARTOP:
    /* This is set at the start of a transfer */
    data->progress.delta.startop_us =
      curlx_ptimediff_us(&timestamp, &data->progress.start);
    data->progress.delta.startqueue_us = data->progress.delta.startop_us;
    നിർത്തുക;
  അവസ്ഥ TIMER_STARTSINGLE:
    /* This is set at the start of each single transfer */
    data->progress.delta.startsingle_us =
      curlx_ptimediff_us(&timestamp, &data->progress.start);
    data->progress.startransfer_added = FALSE;
    നിർത്തുക;
  അവസ്ഥ TIMER_POSTQUEUE:
    /* Queue time is accumulative from all involved redirects */
    data->progress.total.queued_us +=
      curlx_ptimediff_us(&timestamp, &data->progress.start) -
      data->progress.delta.startqueue_us;
    നിർത്തുക;
  അവസ്ഥ TIMER_NAMELOOKUP:
    delta = &data->progress.total.nslookup_us;
    നിർത്തുക;
  അവസ്ഥ TIMER_CONNECT:
    delta = &data->progress.total.connect_us;
    നിർത്തുക;
  അവസ്ഥ TIMER_APPCONNECT:
    delta = &data->progress.total.appconnect_us;
    നിർത്തുക;
  അവസ്ഥ TIMER_PRETRANSFER:
    delta = &data->progress.total.pretransfer_us;
    നിർത്തുക;
  അവസ്ഥ TIMER_STARTTRANSFER:
    /* prevent updating t_starttransfer unless:
     *   1. this is the first time we are setting t_starttransfer
     *   2. a redirect has occurred since the last time t_starttransfer was set
     * This prevents repeated invocations of the function from incorrectly
     * changing the t_starttransfer time.
     */
    എങ്കിൽ(data->progress.startransfer_added) {
      CURL_TRC_M(data, "[%s] ignored", pgrs_timer_name(timer));
      തിരികെ;
    }
    data->progress.startransfer_added = TRUE;
    delta = &data->progress.total.starttransfer_us;
    നിർത്തുക;
  അവസ്ഥ TIMER_POSTRANSFER:
    delta = &data->progress.total.posttransfer_us;
    നിർത്തുക;
  അവസ്ഥ TIMER_REDIRECT:
    data->progress.delta.startredirect_us =
      curlx_ptimediff_us(&timestamp, &data->progress.start);
    /* transfer starts queueing again */
    data->progress.delta.startqueue_us =
      data->progress.delta.startredirect_us;
    നിർത്തുക;
  }
  എങ്കിൽ(delta) {
    timediff_t us = curlx_ptimediff_us(&timestamp, &data->progress.start) -
                    data->progress.delta.startsingle_us;
    എങ്കിൽ(us < 1)
      us = 1; /* make sure at least one microsecond passed */
    *delta += us;
    CURL_TRC_M(data, "[%s] added %" FMT_TIMEDIFF_T "us",
               pgrs_timer_name(timer), us);
  }
  അല്ലെങ്കിൽ
    CURL_TRC_M(data, "[%s] set", pgrs_timer_name(timer));
}

/*
 * Curl_pgrsTime(). Store the current time at the given label. This fetches a
 * fresh "now" and returns it.
 *
 * @unittest: 1399
 */
ശൂന്യം Curl_pgrsTime(ഘടന Curl_easy *data, timerid timer)
{
  Curl_pgrsTimeWas(data, timer, *Curl_pgrs_now(data));
}

ശൂന്യം Curl_pgrsStart(ഘടന Curl_easy *data, സ്ഥിരം ഘടന curltime *pnow)
{
  ഘടന Progress *p = &data->progress;

  എങ്കിൽ(!pnow)
    pnow = Curl_pgrs_now(data);
  p->speeder_c = 0; /* reset the progress meter display */
  p->start = *pnow;
  memset(&p->delta, 0, വലുപ്പം(p->delta));
  memset(&p->total, 0, വലുപ്പം(p->total));
  p->startransfer_added = FALSE;
  p->dl.cur_size = 0;
  p->ul.cur_size = 0;
  /* the sizes are unknown at start */
  p->dl_size_known = FALSE;
  p->ul_size_known = FALSE;
}

/* check that the 'delta' amount of bytes are okay to deliver to the
   application, or return error if not. */
CURLcode Curl_pgrs_deliver_check(ഘടന Curl_easy *data, size_t delta)
{
  എങ്കിൽ(data->set.max_filesize &&
     ((curl_off_t)delta > data->set.max_filesize - data->progress.deliver)) {
    failf(data, "Would have exceeded max file size");
    തിരികെ CURLE_FILESIZE_EXCEEDED;
  }
  തിരികെ CURLE_OK;
}

/* this counts how much data is delivered to the application, which
   in compressed cases may differ from downloaded amount */
ശൂന്യം Curl_pgrs_deliver_inc(ഘടന Curl_easy *data, size_t delta)
{
  data->progress.deliver += delta;
}

ശൂന്യം Curl_pgrs_download_inc(ഘടന Curl_easy *data, size_t delta)
{
  എങ്കിൽ(delta) {
    data->progress.dl.cur_size += delta;
    Curl_rlimit_drain(&data->progress.dl.rlimit, delta, ശൂന്യ);
  }
}

ശൂന്യം Curl_pgrs_upload_inc(ഘടന Curl_easy *data, size_t delta)
{
  എങ്കിൽ(delta) {
    data->progress.ul.cur_size += delta;
    Curl_rlimit_drain(&data->progress.ul.rlimit, delta, ശൂന്യ);
  }
}

/*
 * Set the number of uploaded bytes so far.
 */
ശൂന്യം Curl_pgrsSetUploadCounter(ഘടന Curl_easy *data, curl_off_t size)
{
  data->progress.ul.cur_size = size;
}

ശൂന്യം Curl_pgrsSetDownloadSize(ഘടന Curl_easy *data, curl_off_t size)
{
  എങ്കിൽ(size >= 0) {
    data->progress.dl.total_size = size;
    data->progress.dl_size_known = TRUE;
  }
  അല്ലെങ്കിൽ {
    data->progress.dl.total_size = 0;
    data->progress.dl_size_known = FALSE;
  }
}

ശൂന്യം Curl_pgrsSetUploadSize(ഘടന Curl_easy *data, curl_off_t size)
{
  എങ്കിൽ(size >= 0) {
    data->progress.ul.total_size = size;
    data->progress.ul_size_known = TRUE;
  }
  അല്ലെങ്കിൽ {
    data->progress.ul.total_size = 0;
    data->progress.ul_size_known = FALSE;
  }
}

ശൂന്യം Curl_pgrsEarlyData(ഘടന Curl_easy *data, curl_off_t sent)
{
  data->progress.earlydata_sent = sent;
}

/* returns the average speed in bytes / second */
സ്ഥിതം curl_off_t trspeed(curl_off_t size, /* number of bytes */
                          curl_off_t us)   /* microseconds */
{
  എങ്കിൽ(us < 1)
    തിരികെ size * 1000000;
  അല്ലെങ്കിൽ എങ്കിൽ(size < CURL_OFF_T_MAX / 1000000)
    തിരികെ (size * 1000000) / us;
  അല്ലെങ്കിൽ എങ്കിൽ(us >= 1000000)
    തിരികെ size / (us / 1000000);
  അല്ലെങ്കിൽ
    തിരികെ CURL_OFF_T_MAX;
}

/* returns TRUE if it is time to show the progress meter */
സ്ഥിതം bool progress_calc(ഘടന Curl_easy *data,
                          സ്ഥിരം ഘടന curltime *pnow)
{
  ഘടന Progress * സ്ഥിരം p = &data->progress;
  പൂർണ്ണസംഖ്യ i_next, i_oldest, i_latest;
  timediff_t duration_us, elapsed_us;
  curl_off_t amount;

  /* The time spent so far (from the start) in microseconds */
  elapsed_us = curlx_ptimediff_us(pnow, &p->start);
  p->total.spent_us = elapsed_us;
  p->dl.speed = trspeed(p->dl.cur_size, p->total.spent_us);
  p->ul.speed = trspeed(p->ul.cur_size, p->total.spent_us);

  എങ്കിൽ(!p->speeder_c) { /* no previous record exists */
    p->speed_amount[0] = p->dl.cur_size + p->ul.cur_size;
    p->speed_time[0] = elapsed_us;
    p->speeder_c++;
    /* use the overall average at the start */
    p->current_speed = p->ul.speed + p->dl.speed;
    p->delta.lastshow_us = elapsed_us;
    തിരികെ TRUE;
  }
  /* We have at least one record now. Where to put the next and
   * where is the latest one? */
  i_next = p->speeder_c % CURL_SPEED_RECORDS;
  i_latest = (i_next > 0) ? (i_next - 1) : (CURL_SPEED_RECORDS - 1);

  /* Make a new record only when some time has passed.
   * Too frequent calls otherwise ruin the history. */
  എങ്കിൽ((elapsed_us - p->speed_time[i_latest]) >= (1000 * 1000)) {
    p->speeder_c++;
    i_latest = i_next;
    p->speed_amount[i_latest] = p->dl.cur_size + p->ul.cur_size;
    p->speed_time[i_latest] = elapsed_us;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(data->req.done) {
    /* When a transfer is done, and we did not have a current speed
     * already, update the last record. Otherwise, stay at the speed
     * we have. The last chunk of data, when rate limiting, would increase
     * reported speed since it no longer measures a full second. */
    എങ്കിൽ(!p->current_speed) {
      p->speed_amount[i_latest] = p->dl.cur_size + p->ul.cur_size;
      p->speed_time[i_latest] = elapsed_us;
    }
  }
  അല്ലെങ്കിൽ {
    /* transfer ongoing, wait for more time to pass. */
    തിരികെ FALSE;
  }

  i_oldest = (p->speeder_c < CURL_SPEED_RECORDS) ? 0 :
             ((i_latest + 1) % CURL_SPEED_RECORDS);

  /* How much we transferred between oldest and current records */
  amount = p->speed_amount[i_latest] - p->speed_amount[i_oldest];
  /* How long this took */
  duration_us = p->speed_time[i_latest] - p->speed_time[i_oldest];
  എങ്കിൽ(duration_us <= 0)
    duration_us = 1;

  എങ്കിൽ(amount > (CURL_OFF_T_MAX / 1000000)) {
    /* the 'amount' value is bigger than would fit in 64 bits if
       multiplied with 1000000, so we use the double math for this */
    p->current_speed =
      (curl_off_t)(((ഇരട്ട)amount * 1000000.0) / (ഇരട്ട)duration_us);
  }
  അല്ലെങ്കിൽ {
    p->current_speed = amount * 1000000 / duration_us;
  }

  എങ്കിൽ((p->delta.lastshow_us >= 0) && !data->req.done &&
     ((elapsed_us - p->delta.lastshow_us) < (1000 * 1000)))
    തിരികെ FALSE;
  p->delta.lastshow_us = elapsed_us;
  തിരികെ TRUE;
}

#ifndef CURL_DISABLE_PROGRESS_METER

ഘടന pgrs_estimate {
  curl_off_t secs;
  curl_off_t percent;
};

സ്ഥിതം curl_off_t pgrs_est_percent(curl_off_t total, curl_off_t cur)
{
  എങ്കിൽ(total > 10000)
    തിരികെ cur / (total / 100);
  അല്ലെങ്കിൽ എങ്കിൽ(total > 0)
    തിരികെ (cur * 100) / total;
  തിരികെ 0;
}

സ്ഥിതം ശൂന്യം pgrs_estimates(ഘടന pgrs_dir *d,
                           bool total_known,
                           ഘടന pgrs_estimate *est)
{
  est->secs = 0;
  est->percent = 0;
  എങ്കിൽ(total_known && (d->speed > 0)) {
    est->secs = d->total_size / d->speed;
    est->percent = pgrs_est_percent(d->total_size, d->cur_size);
  }
}

സ്ഥിതം ശൂന്യം progress_meter(ഘടന Curl_easy *data)
{
  ഘടന Progress *p = &data->progress;
  അക്ഷരം max6[6][7];
  ഘടന pgrs_estimate dl_estm;
  ഘടന pgrs_estimate ul_estm;
  ഘടന pgrs_estimate total_estm;
  curl_off_t total_cur_size;
  curl_off_t total_expected_size;
  curl_off_t dl_size;
  അക്ഷരം time_ഇടത്[8];
  അക്ഷരം time_total[8];
  അക്ഷരം time_spent[8];
  curl_off_t cur_secs = (curl_off_t)p->total.spent_us / 1000000;

  എങ്കിൽ(!p->headers_out) {
    എങ്കിൽ(data->state.resume_from) {
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
  time2str(time_ഇടത്, വലുപ്പം(time_ഇടത്),
           total_estm.secs > 0 ? (total_estm.secs - cur_secs) : 0);
  time2str(time_total, വലുപ്പം(time_total), total_estm.secs);
  time2str(time_spent, വലുപ്പം(time_spent), cur_secs);

  /* Get the total amount of data expected to get transferred */
  total_expected_size = p->ul_size_known ? p->ul.total_size : p->ul.cur_size;

  dl_size = p->dl_size_known ? p->dl.total_size : p->dl.cur_size;

  /* integer overflow check */
  എങ്കിൽ((CURL_OFF_T_MAX - total_expected_size) < dl_size)
    total_expected_size = CURL_OFF_T_MAX; /* capped */
  അല്ലെങ്കിൽ
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
                        വലുപ്പം(max6[2])),              /* total size */
                dl_estm.percent, /* 3 letters */       /* rcvd % */
                max6out(p->dl.cur_size, max6[0],
                        വലുപ്പം(max6[0])),              /* rcvd size */
                ul_estm.percent, /* 3 letters */       /* xfer % */
                max6out(p->ul.cur_size, max6[1],
                        വലുപ്പം(max6[1])),              /* xfer size */
                max6out(p->dl.speed, max6[3],
                        വലുപ്പം(max6[3])),              /* avrg dl speed */
                max6out(p->ul.speed, max6[4],
                        വലുപ്പം(max6[4])),              /* avrg ul speed */
                time_total,    /* 7 letters */         /* total time */
                time_spent,    /* 7 letters */         /* time spent */
                time_ഇടത്,     /* 7 letters */         /* time left */
                max6out(p->current_speed, max6[5],
                        വലുപ്പം(max6[5]))               /* current speed */
    );

  /* we flush the output stream to make it appear as soon as possible */
  fflush(data->set.err);
}
#അല്ലെങ്കിൽ /* CURL_DISABLE_PROGRESS_METER */
#define progress_meter(x) Curl_nop_stmt
#endif

/*
 * Curl_pgrsUpdate() returns 0 for success or the value returned by the
 * progress callback!
 */
സ്ഥിതം CURLcode pgrsupdate(ഘടന Curl_easy *data, bool showprogress)
{
  എങ്കിൽ(!data->progress.hide) {
    പൂർണ്ണസംഖ്യ rc;
    എങ്കിൽ(data->set.fxferinfo) {
      /* There is a callback set, call that */
      ഘടന Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_fxferinfo);
      rc = data->set.fxferinfo(data->set.progress_client,
                               data->progress.dl.total_size,
                               data->progress.dl.cur_size,
                               data->progress.ul.total_size,
                               data->progress.ul.cur_size);
      CURL_CBAPI_END(&guard);
      എങ്കിൽ(rc != CURL_PROGRESSFUNC_CONTINUE) {
        എങ്കിൽ(rc) {
          failf(data, "Callback aborted");
          തിരികെ CURLE_ABORTED_BY_CALLBACK;
        }
        തിരികെ CURLE_OK;
      }
    }
    അല്ലെങ്കിൽ എങ്കിൽ(data->set.fprogress) {
      /* The older deprecated callback is set, call that */
      ഘടന Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_fprogress);
      rc = data->set.fprogress(data->set.progress_client,
                               (ഇരട്ട)data->progress.dl.total_size,
                               (ഇരട്ട)data->progress.dl.cur_size,
                               (ഇരട്ട)data->progress.ul.total_size,
                               (ഇരട്ട)data->progress.ul.cur_size);
      CURL_CBAPI_END(&guard);
      എങ്കിൽ(rc != CURL_PROGRESSFUNC_CONTINUE) {
        എങ്കിൽ(rc) {
          failf(data, "Callback aborted");
          തിരികെ CURLE_ABORTED_BY_CALLBACK;
        }
        തിരികെ CURLE_OK;
      }
    }

    എങ്കിൽ(showprogress)
      progress_meter(data);
  }

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode pgrs_update(ഘടന Curl_easy *data,
                            സ്ഥിരം ഘടന curltime *pnow)
{
  bool showprogress = progress_calc(data, pnow);
  തിരികെ pgrsupdate(data, showprogress);
}

CURLcode Curl_pgrsUpdate(ഘടന Curl_easy *data)
{
  തിരികെ pgrs_update(data, Curl_pgrs_now(data));
}

CURLcode Curl_pgrsUpdateX(ഘടന Curl_easy *data,
                          സ്ഥിരം ഘടന curltime *pnow)
{
  തിരികെ pgrs_update(data, pnow);
}

CURLcode Curl_pgrsCheckX(ഘടന Curl_easy *data,
                         സ്ഥിരം ഘടന curltime *pnow)
{
  CURLcode result;

  result = pgrs_update(data, pnow);
  എങ്കിൽ(!result && !data->req.done)
    result = pgrs_speedcheck(data, pnow);
  തിരികെ result;
}

CURLcode Curl_pgrsCheck(ഘടന Curl_easy *data)
{
  തിരികെ Curl_pgrsCheckX(data, Curl_pgrs_now(data));
}

/*
 * Update all progress, do not do progress meter/callbacks.
 */
ശൂന്യം Curl_pgrsUpdate_nometer(ഘടന Curl_easy *data)
{
  (ശൂന്യം)progress_calc(data, Curl_pgrs_now(data));
}

ശൂന്യം Curl_pgrsCompleted(ഘടന Curl_easy *data)
{
  ഘടന Progress * സ്ഥിരം p = &data->progress;
  p->total.spent_us = curlx_ptimediff_us(Curl_pgrs_now(data), &p->start);
}

timediff_t Curl_pgrs_since_ms(ഘടന Curl_easy *data,
                              സ്ഥിരം ഘടന curltime *pnow,
                              timerid timer)
{
  എങ്കിൽ(!pnow)
    pnow = Curl_pgrs_now(data);
  തിരഞ്ഞെടുപ്പ്(timer) {
  അവസ്ഥ TIMER_STARTOP:
    തിരികെ (curlx_ptimediff_us(pnow, &data->progress.start) -
            data->progress.delta.startop_us) / 1000;
  അവസ്ഥ TIMER_STARTSINGLE:
    തിരികെ (curlx_ptimediff_us(pnow, &data->progress.start) -
            data->progress.delta.startsingle_us) / 1000;
  സ്ഥിരസ്ഥിതി:
    DEBUGASSERT(0);
    തിരികെ 0;
  }
}
