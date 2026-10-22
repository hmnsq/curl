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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"

#ifndef CURL_DISABLE_PROGRESS_METER
/* Provide a string that is 7 letters long (plus the zero byte).

   @unittest 1636 */
UNITTEST ಶೂನ್ಯ time2str(ವರ್ಣ *r, size_t rsize, curl_off_t seconds);
UNITTEST ಶೂನ್ಯ time2str(ವರ್ಣ *r, size_t rsize, curl_off_t seconds)
{
  curl_off_t h;
  ಯದಿ(seconds <= 0) {
    curlx_strcopy(r, rsize, STRCONST("       "));
    ಪ್ರತ್ಯಾಗಮ;
  }
  h = seconds / 3600;
  ಯದಿ(h <= 99) {
    curl_off_t m = (seconds - (h * 3600)) / 60;
    ಯದಿ(h <= 9) {
      curl_off_t s = (seconds - (h * 3600)) - (m * 60);
      ಯದಿ(h)
        curl_msnprintf(r, rsize, "%" FMT_OFF_T ":%02" FMT_OFF_T ":"
                       "%02" FMT_OFF_T, h, m, s);
      ಅನ್ಯಥಾ
        curl_msnprintf(r, rsize, "  %02" FMT_OFF_T ":%02" FMT_OFF_T, m, s);
    }
    ಅನ್ಯಥಾ
      curl_msnprintf(r, rsize, "%" FMT_OFF_T "h %02" FMT_OFF_T "m", h, m);
  }
  ಅನ್ಯಥಾ {
    curl_off_t d = seconds / 86400;
    h = (seconds - (d * 86400)) / 3600;
    ಯದಿ(d <= 99)
      curl_msnprintf(r, rsize, "%2" FMT_OFF_T "d %02" FMT_OFF_T "h", d, h);
    ಅನ್ಯಥಾ ಯದಿ(d <= 999)
      curl_msnprintf(r, rsize, "%6" FMT_OFF_T "d", d);
    ಅನ್ಯಥಾ { /* more than 999 days */
      curl_off_t m = d / 30;
      ಯದಿ(m <= 999)
        curl_msnprintf(r, rsize, "%6" FMT_OFF_T "m", m);
      ಅನ್ಯಥಾ { /* more than 999 months */
        curl_off_t y = d / 365;
        ಯದಿ(y <= 99999)
          curl_msnprintf(r, rsize, "%6" FMT_OFF_T "y", y);
        ಅನ್ಯಥಾ
          curlx_strcopy(r, rsize, STRCONST(">99999y"));
      }
    }
  }
}

/* The point of this function would be to return a string of the input data,
   but never longer than 6 columns (+ one zero byte).
   Add suffix k, M, G when suitable...

   @unittest 1636 */
UNITTEST ವರ್ಣ *max6out(curl_off_t bytes, ವರ್ಣ *max6, size_t mlen);
UNITTEST ವರ್ಣ *max6out(curl_off_t bytes, ವರ್ಣ *max6, size_t mlen)
{
  /* a signed 64-bit value is 8192 petabytes maximum, shown as
     8.0E (exabytes)*/
  ಯದಿ(bytes < 100000)
    curl_msnprintf(max6, mlen, "%6" CURL_FORMAT_CURL_OFF_T, bytes);
  ಅನ್ಯಥಾ {
    ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ unit[] = { 'k', 'M', 'G', 'T', 'P', 'E', 0 };
    ಪೂರ್ಣಾಙ್ಕ k = 0;
    curl_off_t nbytes;
    curl_off_t rest;
    ಕುರು {
      nbytes = bytes / 1024;
      ಯದಿ(nbytes < 1000)
        ಭಙ್ಗ;
      bytes = nbytes;
      k++;
      DEBUGASSERT(unit[k]);
    } ಯಾವತ್(unit[k]);
    rest = bytes % 1024;
    ಯದಿ(nbytes <= 99)
      /* xx.yyU */
      curl_msnprintf(max6, mlen, "%2" CURL_FORMAT_CURL_OFF_T
                     ".%02" CURL_FORMAT_CURL_OFF_T "%c", nbytes,
                     rest * 100 / 1024, unit[k]);
    ಅನ್ಯಥಾ
      /* xxx.yU */
      curl_msnprintf(max6, mlen, "%3" CURL_FORMAT_CURL_OFF_T
                     ".%" CURL_FORMAT_CURL_OFF_T "%c", nbytes,
                     rest * 10 / 1024, unit[k]);
  }
  ಪ್ರತ್ಯಾಗಮ max6;
}
#endif

ಸ್ಥಿರ ಶೂನ್ಯ pgrs_speedinit(ಸಂರಚನಾ Curl_easy *data)
{
  memset(&data->state.keeps_speed, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ curltime));
}

/*
 * @unittest 1606
 */
UNITTEST CURLcode pgrs_speedcheck(ಸಂರಚನಾ Curl_easy *data,
                                  ಧ್ರುವ ಸಂರಚನಾ curltime *pnow);
UNITTEST CURLcode pgrs_speedcheck(ಸಂರಚನಾ Curl_easy *data,
                                  ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  ಯದಿ(!data->set.ಕೆಳಗಿನ_speed_time || !data->set.ಕೆಳಗಿನ_speed_limit ||
     Curl_xfer_recv_is_paused(data) || Curl_xfer_send_is_paused(data))
    /* A paused transfer is not qualified for speed checks */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  ಯದಿ(data->progress.current_speed >= 0) {
    ಯದಿ(data->progress.current_speed < data->set.ಕೆಳಗಿನ_speed_limit) {
      ಯದಿ(!data->state.keeps_speed.tv_sec)
        /* under the limit at this moment */
        data->state.keeps_speed = *pnow;
      ಅನ್ಯಥಾ {
        /* how long has it been under the limit */
        timediff_t howlong =
          curlx_ptimediff_ms(pnow, &data->state.keeps_speed);

        ಯದಿ(howlong >= data->set.ಕೆಳಗಿನ_speed_time * 1000) {
          /* too long */
          failf(data, "Operation too slow. Less than %" FMT_OFF_T
                " bytes/sec transferred the last %u seconds",
                data->set.ಕೆಳಗಿನ_speed_limit, data->set.ಕೆಳಗಿನ_speed_time);
          ಪ್ರತ್ಯಾಗಮ CURLE_OPERATION_TIMEDOUT;
        }
      }
    }
    ಅನ್ಯಥಾ
      /* faster right now */
      data->state.keeps_speed.tv_sec = 0;
  }

  /* since low speed limit is enabled, set the expire timer to make this
     connection's speed get checked again in a second */
  Curl_expire_set(data, EXPIRE_SPEEDCHECK, 1000, pnow);

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಧ್ರುವ ಸಂರಚನಾ curltime *Curl_pgrs_now(ಸಂರಚನಾ Curl_easy *data)
{
  curlx_pnow(&data->progress.now);
  ಪ್ರತ್ಯಾಗಮ &data->progress.now;
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

ಪೂರ್ಣಾಙ್ಕ Curl_pgrsDone(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  ಪೂರ್ಣಾಙ್ಕ rc;
  data->progress.delta.lastshow_us = -1;
  rc = Curl_pgrsUpdateX(data, pnow); /* the final (forced) update */
  ಯದಿ(rc)
    ಪ್ರತ್ಯಾಗಮ rc;

  ಯದಿ(!data->progress.hide && !data->progress.callback)
    /* only output if we do not use a progress callback and we are not
     * hidden */
    curl_mfprintf(data->set.err, "\n");

  ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ Curl_pgrsReset(ಸಂರಚನಾ Curl_easy *data)
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
ಶೂನ್ಯ Curl_pgrsResetTransferSizes(ಸಂರಚನಾ Curl_easy *data)
{
  Curl_pgrsSetDownloadSize(data, -1);
  Curl_pgrsSetUploadSize(data, -1);
}

ಶೂನ್ಯ Curl_pgrsRecvPause(ಸಂರಚನಾ Curl_easy *data, bool enable)
{
  ಯದಿ(!enable) {
    data->progress.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(data); /* reset low speed measurements */
  }
}

ಶೂನ್ಯ Curl_pgrsSendPause(ಸಂರಚನಾ Curl_easy *data, bool enable)
{
  ಯದಿ(!enable) {
    data->progress.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(data); /* reset low speed measurements */
  }
}

#ifdef CURLVERBOSE
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ pgrs_timer_names[] = {
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

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *pgrs_timer_name(timerid timer)
{
  ಯದಿ((size_t)timer < CURL_ARRAYSIZE(pgrs_timer_names))
    ಪ್ರತ್ಯಾಗಮ pgrs_timer_names[(size_t)timer];
  ಪ್ರತ್ಯಾಗಮ "?";
}
#endif /* CURLVERBOSE */

/*
 * Curl_pgrsTimeWas(). Store the timestamp time at the given label.
 */
ಶೂನ್ಯ Curl_pgrsTimeWas(ಸಂರಚನಾ Curl_easy *data, timerid timer,
                      ಸಂರಚನಾ curltime timestamp)
{
  timediff_t *delta = ರಿಕ್ತ;

  ವಿಕಲ್ಪ(timer) {
  ಸ್ವಭಾವ:
  ಪಕ್ಷ TIMER_NONE:
    /* mistake filter */
    ಭಙ್ಗ;
  ಪಕ್ಷ TIMER_STARTOP:
    /* This is set at the start of a transfer */
    data->progress.delta.startop_us =
      curlx_ptimediff_us(&timestamp, &data->progress.start);
    data->progress.delta.startqueue_us = data->progress.delta.startop_us;
    ಭಙ್ಗ;
  ಪಕ್ಷ TIMER_STARTSINGLE:
    /* This is set at the start of each single transfer */
    data->progress.delta.startsingle_us =
      curlx_ptimediff_us(&timestamp, &data->progress.start);
    data->progress.startransfer_added = FALSE;
    ಭಙ್ಗ;
  ಪಕ್ಷ TIMER_POSTQUEUE:
    /* Queue time is accumulative from all involved redirects */
    data->progress.total.queued_us +=
      curlx_ptimediff_us(&timestamp, &data->progress.start) -
      data->progress.delta.startqueue_us;
    ಭಙ್ಗ;
  ಪಕ್ಷ TIMER_NAMELOOKUP:
    delta = &data->progress.total.nslookup_us;
    ಭಙ್ಗ;
  ಪಕ್ಷ TIMER_CONNECT:
    delta = &data->progress.total.connect_us;
    ಭಙ್ಗ;
  ಪಕ್ಷ TIMER_APPCONNECT:
    delta = &data->progress.total.appconnect_us;
    ಭಙ್ಗ;
  ಪಕ್ಷ TIMER_PRETRANSFER:
    delta = &data->progress.total.pretransfer_us;
    ಭಙ್ಗ;
  ಪಕ್ಷ TIMER_STARTTRANSFER:
    /* prevent updating t_starttransfer unless:
     *   1. this is the first time we are setting t_starttransfer
     *   2. a redirect has occurred since the last time t_starttransfer was set
     * This prevents repeated invocations of the function from incorrectly
     * changing the t_starttransfer time.
     */
    ಯದಿ(data->progress.startransfer_added) {
      CURL_TRC_M(data, "[%s] ignored", pgrs_timer_name(timer));
      ಪ್ರತ್ಯಾಗಮ;
    }
    data->progress.startransfer_added = TRUE;
    delta = &data->progress.total.starttransfer_us;
    ಭಙ್ಗ;
  ಪಕ್ಷ TIMER_POSTRANSFER:
    delta = &data->progress.total.posttransfer_us;
    ಭಙ್ಗ;
  ಪಕ್ಷ TIMER_REDIRECT:
    data->progress.delta.startredirect_us =
      curlx_ptimediff_us(&timestamp, &data->progress.start);
    /* transfer starts queueing again */
    data->progress.delta.startqueue_us =
      data->progress.delta.startredirect_us;
    ಭಙ್ಗ;
  }
  ಯದಿ(delta) {
    timediff_t us = curlx_ptimediff_us(&timestamp, &data->progress.start) -
                    data->progress.delta.startsingle_us;
    ಯದಿ(us < 1)
      us = 1; /* make sure at least one microsecond passed */
    *delta += us;
    CURL_TRC_M(data, "[%s] added %" FMT_TIMEDIFF_T "us",
               pgrs_timer_name(timer), us);
  }
  ಅನ್ಯಥಾ
    CURL_TRC_M(data, "[%s] set", pgrs_timer_name(timer));
}

/*
 * Curl_pgrsTime(). Store the current time at the given label. This fetches a
 * fresh "now" and returns it.
 *
 * @unittest: 1399
 */
ಶೂನ್ಯ Curl_pgrsTime(ಸಂರಚನಾ Curl_easy *data, timerid timer)
{
  Curl_pgrsTimeWas(data, timer, *Curl_pgrs_now(data));
}

ಶೂನ್ಯ Curl_pgrsStart(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  ಸಂರಚನಾ Progress *p = &data->progress;

  ಯದಿ(!pnow)
    pnow = Curl_pgrs_now(data);
  p->speeder_c = 0; /* reset the progress meter display */
  p->start = *pnow;
  memset(&p->delta, 0, ಪರಿಮಾಣ(p->delta));
  memset(&p->total, 0, ಪರಿಮಾಣ(p->total));
  p->startransfer_added = FALSE;
  p->dl.cur_size = 0;
  p->ul.cur_size = 0;
  /* the sizes are unknown at start */
  p->dl_size_known = FALSE;
  p->ul_size_known = FALSE;
}

/* check that the 'delta' amount of bytes are okay to deliver to the
   application, or return error if not. */
CURLcode Curl_pgrs_deliver_check(ಸಂರಚನಾ Curl_easy *data, size_t delta)
{
  ಯದಿ(data->set.max_filesize &&
     ((curl_off_t)delta > data->set.max_filesize - data->progress.deliver)) {
    failf(data, "Would have exceeded max file size");
    ಪ್ರತ್ಯಾಗಮ CURLE_FILESIZE_EXCEEDED;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* this counts how much data is delivered to the application, which
   in compressed cases may differ from downloaded amount */
ಶೂನ್ಯ Curl_pgrs_deliver_inc(ಸಂರಚನಾ Curl_easy *data, size_t delta)
{
  data->progress.deliver += delta;
}

ಶೂನ್ಯ Curl_pgrs_download_inc(ಸಂರಚನಾ Curl_easy *data, size_t delta)
{
  ಯದಿ(delta) {
    data->progress.dl.cur_size += delta;
    Curl_rlimit_drain(&data->progress.dl.rlimit, delta, ರಿಕ್ತ);
  }
}

ಶೂನ್ಯ Curl_pgrs_upload_inc(ಸಂರಚನಾ Curl_easy *data, size_t delta)
{
  ಯದಿ(delta) {
    data->progress.ul.cur_size += delta;
    Curl_rlimit_drain(&data->progress.ul.rlimit, delta, ರಿಕ್ತ);
  }
}

/*
 * Set the number of uploaded bytes so far.
 */
ಶೂನ್ಯ Curl_pgrsSetUploadCounter(ಸಂರಚನಾ Curl_easy *data, curl_off_t size)
{
  data->progress.ul.cur_size = size;
}

ಶೂನ್ಯ Curl_pgrsSetDownloadSize(ಸಂರಚನಾ Curl_easy *data, curl_off_t size)
{
  ಯದಿ(size >= 0) {
    data->progress.dl.total_size = size;
    data->progress.dl_size_known = TRUE;
  }
  ಅನ್ಯಥಾ {
    data->progress.dl.total_size = 0;
    data->progress.dl_size_known = FALSE;
  }
}

ಶೂನ್ಯ Curl_pgrsSetUploadSize(ಸಂರಚನಾ Curl_easy *data, curl_off_t size)
{
  ಯದಿ(size >= 0) {
    data->progress.ul.total_size = size;
    data->progress.ul_size_known = TRUE;
  }
  ಅನ್ಯಥಾ {
    data->progress.ul.total_size = 0;
    data->progress.ul_size_known = FALSE;
  }
}

ಶೂನ್ಯ Curl_pgrsEarlyData(ಸಂರಚನಾ Curl_easy *data, curl_off_t sent)
{
  data->progress.earlydata_sent = sent;
}

/* returns the average speed in bytes / second */
ಸ್ಥಿರ curl_off_t trspeed(curl_off_t size, /* number of bytes */
                          curl_off_t us)   /* microseconds */
{
  ಯದಿ(us < 1)
    ಪ್ರತ್ಯಾಗಮ size * 1000000;
  ಅನ್ಯಥಾ ಯದಿ(size < CURL_OFF_T_MAX / 1000000)
    ಪ್ರತ್ಯಾಗಮ (size * 1000000) / us;
  ಅನ್ಯಥಾ ಯದಿ(us >= 1000000)
    ಪ್ರತ್ಯಾಗಮ size / (us / 1000000);
  ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ CURL_OFF_T_MAX;
}

/* returns TRUE if it is time to show the progress meter */
ಸ್ಥಿರ bool progress_calc(ಸಂರಚನಾ Curl_easy *data,
                          ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  ಸಂರಚನಾ Progress * ಧ್ರುವ p = &data->progress;
  ಪೂರ್ಣಾಙ್ಕ i_next, i_oldest, i_latest;
  timediff_t duration_us, elapsed_us;
  curl_off_t amount;

  /* The time spent so far (from the start) in microseconds */
  elapsed_us = curlx_ptimediff_us(pnow, &p->start);
  p->total.spent_us = elapsed_us;
  p->dl.speed = trspeed(p->dl.cur_size, p->total.spent_us);
  p->ul.speed = trspeed(p->ul.cur_size, p->total.spent_us);

  ಯದಿ(!p->speeder_c) { /* no previous record exists */
    p->speed_amount[0] = p->dl.cur_size + p->ul.cur_size;
    p->speed_time[0] = elapsed_us;
    p->speeder_c++;
    /* use the overall average at the start */
    p->current_speed = p->ul.speed + p->dl.speed;
    p->delta.lastshow_us = elapsed_us;
    ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  /* We have at least one record now. Where to put the next and
   * where is the latest one? */
  i_next = p->speeder_c % CURL_SPEED_RECORDS;
  i_latest = (i_next > 0) ? (i_next - 1) : (CURL_SPEED_RECORDS - 1);

  /* Make a new record only when some time has passed.
   * Too frequent calls otherwise ruin the history. */
  ಯದಿ((elapsed_us - p->speed_time[i_latest]) >= (1000 * 1000)) {
    p->speeder_c++;
    i_latest = i_next;
    p->speed_amount[i_latest] = p->dl.cur_size + p->ul.cur_size;
    p->speed_time[i_latest] = elapsed_us;
  }
  ಅನ್ಯಥಾ ಯದಿ(data->req.done) {
    /* When a transfer is done, and we did not have a current speed
     * already, update the last record. Otherwise, stay at the speed
     * we have. The last chunk of data, when rate limiting, would increase
     * reported speed since it no longer measures a full second. */
    ಯದಿ(!p->current_speed) {
      p->speed_amount[i_latest] = p->dl.cur_size + p->ul.cur_size;
      p->speed_time[i_latest] = elapsed_us;
    }
  }
  ಅನ್ಯಥಾ {
    /* transfer ongoing, wait for more time to pass. */
    ಪ್ರತ್ಯಾಗಮ FALSE;
  }

  i_oldest = (p->speeder_c < CURL_SPEED_RECORDS) ? 0 :
             ((i_latest + 1) % CURL_SPEED_RECORDS);

  /* How much we transferred between oldest and current records */
  amount = p->speed_amount[i_latest] - p->speed_amount[i_oldest];
  /* How long this took */
  duration_us = p->speed_time[i_latest] - p->speed_time[i_oldest];
  ಯದಿ(duration_us <= 0)
    duration_us = 1;

  ಯದಿ(amount > (CURL_OFF_T_MAX / 1000000)) {
    /* the 'amount' value is bigger than would fit in 64 bits if
       multiplied with 1000000, so we use the double math for this */
    p->current_speed =
      (curl_off_t)(((ದ್ವಿಗುಣ)amount * 1000000.0) / (ದ್ವಿಗುಣ)duration_us);
  }
  ಅನ್ಯಥಾ {
    p->current_speed = amount * 1000000 / duration_us;
  }

  ಯದಿ((p->delta.lastshow_us >= 0) && !data->req.done &&
     ((elapsed_us - p->delta.lastshow_us) < (1000 * 1000)))
    ಪ್ರತ್ಯಾಗಮ FALSE;
  p->delta.lastshow_us = elapsed_us;
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

#ifndef CURL_DISABLE_PROGRESS_METER

ಸಂರಚನಾ pgrs_estimate {
  curl_off_t secs;
  curl_off_t percent;
};

ಸ್ಥಿರ curl_off_t pgrs_est_percent(curl_off_t total, curl_off_t cur)
{
  ಯದಿ(total > 10000)
    ಪ್ರತ್ಯಾಗಮ cur / (total / 100);
  ಅನ್ಯಥಾ ಯದಿ(total > 0)
    ಪ್ರತ್ಯಾಗಮ (cur * 100) / total;
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ pgrs_estimates(ಸಂರಚನಾ pgrs_dir *d,
                           bool total_known,
                           ಸಂರಚನಾ pgrs_estimate *est)
{
  est->secs = 0;
  est->percent = 0;
  ಯದಿ(total_known && (d->speed > 0)) {
    est->secs = d->total_size / d->speed;
    est->percent = pgrs_est_percent(d->total_size, d->cur_size);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ progress_meter(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Progress *p = &data->progress;
  ವರ್ಣ max6[6][7];
  ಸಂರಚನಾ pgrs_estimate dl_estm;
  ಸಂರಚನಾ pgrs_estimate ul_estm;
  ಸಂರಚನಾ pgrs_estimate total_estm;
  curl_off_t total_cur_size;
  curl_off_t total_expected_size;
  curl_off_t dl_size;
  ವರ್ಣ time_ಎಡ[8];
  ವರ್ಣ time_total[8];
  ವರ್ಣ time_spent[8];
  curl_off_t cur_secs = (curl_off_t)p->total.spent_us / 1000000;

  ಯದಿ(!p->headers_out) {
    ಯದಿ(data->state.resume_from) {
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
  time2str(time_ಎಡ, ಪರಿಮಾಣ(time_ಎಡ),
           total_estm.secs > 0 ? (total_estm.secs - cur_secs) : 0);
  time2str(time_total, ಪರಿಮಾಣ(time_total), total_estm.secs);
  time2str(time_spent, ಪರಿಮಾಣ(time_spent), cur_secs);

  /* Get the total amount of data expected to get transferred */
  total_expected_size = p->ul_size_known ? p->ul.total_size : p->ul.cur_size;

  dl_size = p->dl_size_known ? p->dl.total_size : p->dl.cur_size;

  /* integer overflow check */
  ಯದಿ((CURL_OFF_T_MAX - total_expected_size) < dl_size)
    total_expected_size = CURL_OFF_T_MAX; /* capped */
  ಅನ್ಯಥಾ
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
                        ಪರಿಮಾಣ(max6[2])),              /* total size */
                dl_estm.percent, /* 3 letters */       /* rcvd % */
                max6out(p->dl.cur_size, max6[0],
                        ಪರಿಮಾಣ(max6[0])),              /* rcvd size */
                ul_estm.percent, /* 3 letters */       /* xfer % */
                max6out(p->ul.cur_size, max6[1],
                        ಪರಿಮಾಣ(max6[1])),              /* xfer size */
                max6out(p->dl.speed, max6[3],
                        ಪರಿಮಾಣ(max6[3])),              /* avrg dl speed */
                max6out(p->ul.speed, max6[4],
                        ಪರಿಮಾಣ(max6[4])),              /* avrg ul speed */
                time_total,    /* 7 letters */         /* total time */
                time_spent,    /* 7 letters */         /* time spent */
                time_ಎಡ,     /* 7 letters */         /* time left */
                max6out(p->current_speed, max6[5],
                        ಪರಿಮಾಣ(max6[5]))               /* current speed */
    );

  /* we flush the output stream to make it appear as soon as possible */
  fflush(data->set.err);
}
#ಅನ್ಯಥಾ /* CURL_DISABLE_PROGRESS_METER */
#define progress_meter(x) Curl_nop_stmt
#endif

/*
 * Curl_pgrsUpdate() returns 0 for success or the value returned by the
 * progress callback!
 */
ಸ್ಥಿರ CURLcode pgrsupdate(ಸಂರಚನಾ Curl_easy *data, bool showprogress)
{
  ಯದಿ(!data->progress.hide) {
    ಪೂರ್ಣಾಙ್ಕ rc;
    ಯದಿ(data->set.fxferinfo) {
      /* There is a callback set, call that */
      ಸಂರಚನಾ Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_fxferinfo);
      rc = data->set.fxferinfo(data->set.progress_client,
                               data->progress.dl.total_size,
                               data->progress.dl.cur_size,
                               data->progress.ul.total_size,
                               data->progress.ul.cur_size);
      CURL_CBAPI_END(&guard);
      ಯದಿ(rc != CURL_PROGRESSFUNC_CONTINUE) {
        ಯದಿ(rc) {
          failf(data, "Callback aborted");
          ಪ್ರತ್ಯಾಗಮ CURLE_ABORTED_BY_CALLBACK;
        }
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }
    }
    ಅನ್ಯಥಾ ಯದಿ(data->set.fprogress) {
      /* The older deprecated callback is set, call that */
      ಸಂರಚನಾ Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_fprogress);
      rc = data->set.fprogress(data->set.progress_client,
                               (ದ್ವಿಗುಣ)data->progress.dl.total_size,
                               (ದ್ವಿಗುಣ)data->progress.dl.cur_size,
                               (ದ್ವಿಗುಣ)data->progress.ul.total_size,
                               (ದ್ವಿಗುಣ)data->progress.ul.cur_size);
      CURL_CBAPI_END(&guard);
      ಯದಿ(rc != CURL_PROGRESSFUNC_CONTINUE) {
        ಯದಿ(rc) {
          failf(data, "Callback aborted");
          ಪ್ರತ್ಯಾಗಮ CURLE_ABORTED_BY_CALLBACK;
        }
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }
    }

    ಯದಿ(showprogress)
      progress_meter(data);
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode pgrs_update(ಸಂರಚನಾ Curl_easy *data,
                            ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  bool showprogress = progress_calc(data, pnow);
  ಪ್ರತ್ಯಾಗಮ pgrsupdate(data, showprogress);
}

CURLcode Curl_pgrsUpdate(ಸಂರಚನಾ Curl_easy *data)
{
  ಪ್ರತ್ಯಾಗಮ pgrs_update(data, Curl_pgrs_now(data));
}

CURLcode Curl_pgrsUpdateX(ಸಂರಚನಾ Curl_easy *data,
                          ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  ಪ್ರತ್ಯಾಗಮ pgrs_update(data, pnow);
}

CURLcode Curl_pgrsCheckX(ಸಂರಚನಾ Curl_easy *data,
                         ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  CURLcode result;

  result = pgrs_update(data, pnow);
  ಯದಿ(!result && !data->req.done)
    result = pgrs_speedcheck(data, pnow);
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_pgrsCheck(ಸಂರಚನಾ Curl_easy *data)
{
  ಪ್ರತ್ಯಾಗಮ Curl_pgrsCheckX(data, Curl_pgrs_now(data));
}

/*
 * Update all progress, do not do progress meter/callbacks.
 */
ಶೂನ್ಯ Curl_pgrsUpdate_nometer(ಸಂರಚನಾ Curl_easy *data)
{
  (ಶೂನ್ಯ)progress_calc(data, Curl_pgrs_now(data));
}

ಶೂನ್ಯ Curl_pgrsCompleted(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Progress * ಧ್ರುವ p = &data->progress;
  p->total.spent_us = curlx_ptimediff_us(Curl_pgrs_now(data), &p->start);
}

timediff_t Curl_pgrs_since_ms(ಸಂರಚನಾ Curl_easy *data,
                              ಧ್ರುವ ಸಂರಚನಾ curltime *pnow,
                              timerid timer)
{
  ಯದಿ(!pnow)
    pnow = Curl_pgrs_now(data);
  ವಿಕಲ್ಪ(timer) {
  ಪಕ್ಷ TIMER_STARTOP:
    ಪ್ರತ್ಯಾಗಮ (curlx_ptimediff_us(pnow, &data->progress.start) -
            data->progress.delta.startop_us) / 1000;
  ಪಕ್ಷ TIMER_STARTSINGLE:
    ಪ್ರತ್ಯಾಗಮ (curlx_ptimediff_us(pnow, &data->progress.start) -
            data->progress.delta.startsingle_us) / 1000;
  ಸ್ವಭಾವ:
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ 0;
  }
}
