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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "curlx/strcopy.h"

#ifndef CURL_DISABLE_PROGRESS_METER
/* Provide a string that is 7 letters long (plus the zero byte).

   @unittest 1636 */
UNITTEST शून्य time2str(वर्ण *r, माप_प्रकार rsize, curl_off_t seconds);
UNITTEST शून्य time2str(वर्ण *r, माप_प्रकार rsize, curl_off_t seconds)
{
  curl_off_t h;
  यदि(seconds <= 0) {
    curlx_strcopy(r, rsize, STRCONST("       "));
    प्रत्यागम;
  }
  h = seconds / 3600;
  यदि(h <= 99) {
    curl_off_t m = (seconds - (h * 3600)) / 60;
    यदि(h <= 9) {
      curl_off_t s = (seconds - (h * 3600)) - (m * 60);
      यदि(h)
        curl_msnprintf(r, rsize, "%" FMT_OFF_T ":%02" FMT_OFF_T ":"
                       "%02" FMT_OFF_T, h, m, s);
      अन्यथा
        curl_msnprintf(r, rsize, "  %02" FMT_OFF_T ":%02" FMT_OFF_T, m, s);
    }
    अन्यथा
      curl_msnprintf(r, rsize, "%" FMT_OFF_T "h %02" FMT_OFF_T "m", h, m);
  }
  अन्यथा {
    curl_off_t d = seconds / 86400;
    h = (seconds - (d * 86400)) / 3600;
    यदि(d <= 99)
      curl_msnprintf(r, rsize, "%2" FMT_OFF_T "d %02" FMT_OFF_T "h", d, h);
    अन्यथा यदि(d <= 999)
      curl_msnprintf(r, rsize, "%6" FMT_OFF_T "d", d);
    अन्यथा { /* more than 999 days */
      curl_off_t m = d / 30;
      यदि(m <= 999)
        curl_msnprintf(r, rsize, "%6" FMT_OFF_T "m", m);
      अन्यथा { /* more than 999 months */
        curl_off_t y = d / 365;
        यदि(y <= 99999)
          curl_msnprintf(r, rsize, "%6" FMT_OFF_T "y", y);
        अन्यथा
          curlx_strcopy(r, rsize, STRCONST(">99999y"));
      }
    }
  }
}

/* The point of this function would be to return a string of the input data,
   but never longer than 6 columns (+ one zero byte).
   Add suffix k, M, G when suitable...

   @unittest 1636 */
UNITTEST वर्ण *max6out(curl_off_t bytes, वर्ण *max6, माप_प्रकार mlen);
UNITTEST वर्ण *max6out(curl_off_t bytes, वर्ण *max6, माप_प्रकार mlen)
{
  /* a signed 64-bit value is 8192 petabytes maximum, shown as
     8.0E (exabytes)*/
  यदि(bytes < 100000)
    curl_msnprintf(max6, mlen, "%6" CURL_FORMAT_CURL_OFF_T, bytes);
  अन्यथा {
    स्थिर ध्रुव वर्ण unit[] = { 'k', 'M', 'G', 'T', 'P', 'E', 0 };
    पूर्णाङ्क k = 0;
    curl_off_t nbytes;
    curl_off_t rest;
    कुरु {
      nbytes = bytes / 1024;
      यदि(nbytes < 1000)
        भङ्ग;
      bytes = nbytes;
      k++;
      DEBUGASSERT(unit[k]);
    } यावत्(unit[k]);
    rest = bytes % 1024;
    यदि(nbytes <= 99)
      /* xx.yyU */
      curl_msnprintf(max6, mlen, "%2" CURL_FORMAT_CURL_OFF_T
                     ".%02" CURL_FORMAT_CURL_OFF_T "%c", nbytes,
                     rest * 100 / 1024, unit[k]);
    अन्यथा
      /* xxx.yU */
      curl_msnprintf(max6, mlen, "%3" CURL_FORMAT_CURL_OFF_T
                     ".%" CURL_FORMAT_CURL_OFF_T "%c", nbytes,
                     rest * 10 / 1024, unit[k]);
  }
  प्रत्यागम max6;
}
#endif

स्थिर शून्य pgrs_speedinit(संरचना Curl_easy *data)
{
  स_रखो(&data->state.keeps_speed, 0, परिमाण(संरचना curltime));
}

/*
 * @unittest 1606
 */
UNITTEST CURLcode pgrs_speedcheck(संरचना Curl_easy *data,
                                  ध्रुव संरचना curltime *pnow);
UNITTEST CURLcode pgrs_speedcheck(संरचना Curl_easy *data,
                                  ध्रुव संरचना curltime *pnow)
{
  यदि(!data->set.निचला_speed_time || !data->set.निचला_speed_limit ||
     Curl_xfer_recv_is_paused(data) || Curl_xfer_send_is_paused(data))
    /* A paused transfer is not qualified for speed checks */
    प्रत्यागम CURLE_OK;

  यदि(data->progress.current_speed >= 0) {
    यदि(data->progress.current_speed < data->set.निचला_speed_limit) {
      यदि(!data->state.keeps_speed.tv_sec)
        /* under the limit at this moment */
        data->state.keeps_speed = *pnow;
      अन्यथा {
        /* how long has it been under the limit */
        timediff_t howlong =
          curlx_ptimediff_ms(pnow, &data->state.keeps_speed);

        यदि(howlong >= data->set.निचला_speed_time * 1000) {
          /* too long */
          failf(data, "Operation too slow. Less than %" FMT_OFF_T
                " bytes/sec transferred the last %u seconds",
                data->set.निचला_speed_limit, data->set.निचला_speed_time);
          प्रत्यागम CURLE_OPERATION_TIMEDOUT;
        }
      }
    }
    अन्यथा
      /* faster right now */
      data->state.keeps_speed.tv_sec = 0;
  }

  /* since low speed limit is enabled, set the expire timer to make this
     connection's speed get checked again in a second */
  Curl_expire_set(data, EXPIRE_SPEEDCHECK, 1000, pnow);

  प्रत्यागम CURLE_OK;
}

ध्रुव संरचना curltime *Curl_pgrs_now(संरचना Curl_easy *data)
{
  curlx_pnow(&data->progress.now);
  प्रत्यागम &data->progress.now;
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

पूर्णाङ्क Curl_pgrsDone(संरचना Curl_easy *data, ध्रुव संरचना curltime *pnow)
{
  पूर्णाङ्क rc;
  data->progress.delta.lastshow_us = -1;
  rc = Curl_pgrsUpdateX(data, pnow); /* the final (forced) update */
  यदि(rc)
    प्रत्यागम rc;

  यदि(!data->progress.hide && !data->progress.callback)
    /* only output if we do not use a progress callback and we are not
     * hidden */
    curl_mfprintf(data->set.err, "\n");

  प्रत्यागम 0;
}

शून्य Curl_pgrsReset(संरचना Curl_easy *data)
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
शून्य Curl_pgrsResetTransferSizes(संरचना Curl_easy *data)
{
  Curl_pgrsSetDownloadSize(data, -1);
  Curl_pgrsSetUploadSize(data, -1);
}

शून्य Curl_pgrsRecvPause(संरचना Curl_easy *data, bool enable)
{
  यदि(!enable) {
    data->progress.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(data); /* reset low speed measurements */
  }
}

शून्य Curl_pgrsSendPause(संरचना Curl_easy *data, bool enable)
{
  यदि(!enable) {
    data->progress.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(data); /* reset low speed measurements */
  }
}

#ifdef CURLVERBOSE
स्थिर ध्रुव वर्ण * ध्रुव pgrs_timer_names[] = {
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

स्थिर ध्रुव वर्ण *pgrs_timer_name(timerid timer)
{
  यदि((माप_प्रकार)timer < CURL_ARRAYSIZE(pgrs_timer_names))
    प्रत्यागम pgrs_timer_names[(माप_प्रकार)timer];
  प्रत्यागम "?";
}
#endif /* CURLVERBOSE */

/*
 * Curl_pgrsTimeWas(). Store the timestamp time at the given label.
 */
शून्य Curl_pgrsTimeWas(संरचना Curl_easy *data, timerid timer,
                      संरचना curltime timestamp)
{
  timediff_t *delta = रिक्त;

  विकल्प(timer) {
  स्वभाव:
  पक्ष TIMER_NONE:
    /* mistake filter */
    भङ्ग;
  पक्ष TIMER_STARTOP:
    /* This is set at the start of a transfer */
    data->progress.delta.startop_us =
      curlx_ptimediff_us(&timestamp, &data->progress.start);
    data->progress.delta.startqueue_us = data->progress.delta.startop_us;
    भङ्ग;
  पक्ष TIMER_STARTSINGLE:
    /* This is set at the start of each single transfer */
    data->progress.delta.startsingle_us =
      curlx_ptimediff_us(&timestamp, &data->progress.start);
    data->progress.startransfer_added = FALSE;
    भङ्ग;
  पक्ष TIMER_POSTQUEUE:
    /* Queue time is accumulative from all involved redirects */
    data->progress.total.queued_us +=
      curlx_ptimediff_us(&timestamp, &data->progress.start) -
      data->progress.delta.startqueue_us;
    भङ्ग;
  पक्ष TIMER_NAMELOOKUP:
    delta = &data->progress.total.nslookup_us;
    भङ्ग;
  पक्ष TIMER_CONNECT:
    delta = &data->progress.total.connect_us;
    भङ्ग;
  पक्ष TIMER_APPCONNECT:
    delta = &data->progress.total.appconnect_us;
    भङ्ग;
  पक्ष TIMER_PRETRANSFER:
    delta = &data->progress.total.pretransfer_us;
    भङ्ग;
  पक्ष TIMER_STARTTRANSFER:
    /* prevent updating t_starttransfer unless:
     *   1. this is the first time we are setting t_starttransfer
     *   2. a redirect has occurred since the last time t_starttransfer was set
     * This prevents repeated invocations of the function from incorrectly
     * changing the t_starttransfer time.
     */
    यदि(data->progress.startransfer_added) {
      CURL_TRC_M(data, "[%s] ignored", pgrs_timer_name(timer));
      प्रत्यागम;
    }
    data->progress.startransfer_added = TRUE;
    delta = &data->progress.total.starttransfer_us;
    भङ्ग;
  पक्ष TIMER_POSTRANSFER:
    delta = &data->progress.total.posttransfer_us;
    भङ्ग;
  पक्ष TIMER_REDIRECT:
    data->progress.delta.startredirect_us =
      curlx_ptimediff_us(&timestamp, &data->progress.start);
    /* transfer starts queueing again */
    data->progress.delta.startqueue_us =
      data->progress.delta.startredirect_us;
    भङ्ग;
  }
  यदि(delta) {
    timediff_t us = curlx_ptimediff_us(&timestamp, &data->progress.start) -
                    data->progress.delta.startsingle_us;
    यदि(us < 1)
      us = 1; /* make sure at least one microsecond passed */
    *delta += us;
    CURL_TRC_M(data, "[%s] added %" FMT_TIMEDIFF_T "us",
               pgrs_timer_name(timer), us);
  }
  अन्यथा
    CURL_TRC_M(data, "[%s] set", pgrs_timer_name(timer));
}

/*
 * Curl_pgrsTime(). Store the current time at the given label. This fetches a
 * fresh "now" and returns it.
 *
 * @unittest: 1399
 */
शून्य Curl_pgrsTime(संरचना Curl_easy *data, timerid timer)
{
  Curl_pgrsTimeWas(data, timer, *Curl_pgrs_now(data));
}

शून्य Curl_pgrsStart(संरचना Curl_easy *data, ध्रुव संरचना curltime *pnow)
{
  संरचना Progress *p = &data->progress;

  यदि(!pnow)
    pnow = Curl_pgrs_now(data);
  p->speeder_c = 0; /* reset the progress meter display */
  p->start = *pnow;
  स_रखो(&p->delta, 0, परिमाण(p->delta));
  स_रखो(&p->total, 0, परिमाण(p->total));
  p->startransfer_added = FALSE;
  p->dl.cur_size = 0;
  p->ul.cur_size = 0;
  /* the sizes are unknown at start */
  p->dl_size_known = FALSE;
  p->ul_size_known = FALSE;
}

/* check that the 'delta' amount of bytes are okay to deliver to the
   application, or return error if not. */
CURLcode Curl_pgrs_deliver_check(संरचना Curl_easy *data, माप_प्रकार delta)
{
  यदि(data->set.max_filesize &&
     ((curl_off_t)delta > data->set.max_filesize - data->progress.deliver)) {
    failf(data, "Would have exceeded max file size");
    प्रत्यागम CURLE_FILESIZE_EXCEEDED;
  }
  प्रत्यागम CURLE_OK;
}

/* this counts how much data is delivered to the application, which
   in compressed cases may differ from downloaded amount */
शून्य Curl_pgrs_deliver_inc(संरचना Curl_easy *data, माप_प्रकार delta)
{
  data->progress.deliver += delta;
}

शून्य Curl_pgrs_download_inc(संरचना Curl_easy *data, माप_प्रकार delta)
{
  यदि(delta) {
    data->progress.dl.cur_size += delta;
    Curl_rlimit_drain(&data->progress.dl.rlimit, delta, रिक्त);
  }
}

शून्य Curl_pgrs_upload_inc(संरचना Curl_easy *data, माप_प्रकार delta)
{
  यदि(delta) {
    data->progress.ul.cur_size += delta;
    Curl_rlimit_drain(&data->progress.ul.rlimit, delta, रिक्त);
  }
}

/*
 * Set the number of uploaded bytes so far.
 */
शून्य Curl_pgrsSetUploadCounter(संरचना Curl_easy *data, curl_off_t size)
{
  data->progress.ul.cur_size = size;
}

शून्य Curl_pgrsSetDownloadSize(संरचना Curl_easy *data, curl_off_t size)
{
  यदि(size >= 0) {
    data->progress.dl.total_size = size;
    data->progress.dl_size_known = TRUE;
  }
  अन्यथा {
    data->progress.dl.total_size = 0;
    data->progress.dl_size_known = FALSE;
  }
}

शून्य Curl_pgrsSetUploadSize(संरचना Curl_easy *data, curl_off_t size)
{
  यदि(size >= 0) {
    data->progress.ul.total_size = size;
    data->progress.ul_size_known = TRUE;
  }
  अन्यथा {
    data->progress.ul.total_size = 0;
    data->progress.ul_size_known = FALSE;
  }
}

शून्य Curl_pgrsEarlyData(संरचना Curl_easy *data, curl_off_t sent)
{
  data->progress.earlydata_sent = sent;
}

/* returns the average speed in bytes / second */
स्थिर curl_off_t trspeed(curl_off_t size, /* number of bytes */
                          curl_off_t us)   /* microseconds */
{
  यदि(us < 1)
    प्रत्यागम size * 1000000;
  अन्यथा यदि(size < CURL_OFF_T_MAX / 1000000)
    प्रत्यागम (size * 1000000) / us;
  अन्यथा यदि(us >= 1000000)
    प्रत्यागम size / (us / 1000000);
  अन्यथा
    प्रत्यागम CURL_OFF_T_MAX;
}

/* returns TRUE if it is time to show the progress meter */
स्थिर bool progress_calc(संरचना Curl_easy *data,
                          ध्रुव संरचना curltime *pnow)
{
  संरचना Progress * ध्रुव p = &data->progress;
  पूर्णाङ्क i_next, i_oldest, i_latest;
  timediff_t duration_us, elapsed_us;
  curl_off_t amount;

  /* The time spent so far (from the start) in microseconds */
  elapsed_us = curlx_ptimediff_us(pnow, &p->start);
  p->total.spent_us = elapsed_us;
  p->dl.speed = trspeed(p->dl.cur_size, p->total.spent_us);
  p->ul.speed = trspeed(p->ul.cur_size, p->total.spent_us);

  यदि(!p->speeder_c) { /* no previous record exists */
    p->speed_amount[0] = p->dl.cur_size + p->ul.cur_size;
    p->speed_time[0] = elapsed_us;
    p->speeder_c++;
    /* use the overall average at the start */
    p->current_speed = p->ul.speed + p->dl.speed;
    p->delta.lastshow_us = elapsed_us;
    प्रत्यागम TRUE;
  }
  /* We have at least one record now. Where to put the next and
   * where is the latest one? */
  i_next = p->speeder_c % CURL_SPEED_RECORDS;
  i_latest = (i_next > 0) ? (i_next - 1) : (CURL_SPEED_RECORDS - 1);

  /* Make a new record only when some time has passed.
   * Too frequent calls otherwise ruin the history. */
  यदि((elapsed_us - p->speed_time[i_latest]) >= (1000 * 1000)) {
    p->speeder_c++;
    i_latest = i_next;
    p->speed_amount[i_latest] = p->dl.cur_size + p->ul.cur_size;
    p->speed_time[i_latest] = elapsed_us;
  }
  अन्यथा यदि(data->req.done) {
    /* When a transfer is done, and we did not have a current speed
     * already, update the last record. Otherwise, stay at the speed
     * we have. The last chunk of data, when rate limiting, would increase
     * reported speed since it no longer measures a full second. */
    यदि(!p->current_speed) {
      p->speed_amount[i_latest] = p->dl.cur_size + p->ul.cur_size;
      p->speed_time[i_latest] = elapsed_us;
    }
  }
  अन्यथा {
    /* transfer ongoing, wait for more time to pass. */
    प्रत्यागम FALSE;
  }

  i_oldest = (p->speeder_c < CURL_SPEED_RECORDS) ? 0 :
             ((i_latest + 1) % CURL_SPEED_RECORDS);

  /* How much we transferred between oldest and current records */
  amount = p->speed_amount[i_latest] - p->speed_amount[i_oldest];
  /* How long this took */
  duration_us = p->speed_time[i_latest] - p->speed_time[i_oldest];
  यदि(duration_us <= 0)
    duration_us = 1;

  यदि(amount > (CURL_OFF_T_MAX / 1000000)) {
    /* the 'amount' value is bigger than would fit in 64 bits if
       multiplied with 1000000, so we use the double math for this */
    p->current_speed =
      (curl_off_t)(((द्विगुण)amount * 1000000.0) / (द्विगुण)duration_us);
  }
  अन्यथा {
    p->current_speed = amount * 1000000 / duration_us;
  }

  यदि((p->delta.lastshow_us >= 0) && !data->req.done &&
     ((elapsed_us - p->delta.lastshow_us) < (1000 * 1000)))
    प्रत्यागम FALSE;
  p->delta.lastshow_us = elapsed_us;
  प्रत्यागम TRUE;
}

#ifndef CURL_DISABLE_PROGRESS_METER

संरचना pgrs_estimate {
  curl_off_t secs;
  curl_off_t percent;
};

स्थिर curl_off_t pgrs_est_percent(curl_off_t total, curl_off_t cur)
{
  यदि(total > 10000)
    प्रत्यागम cur / (total / 100);
  अन्यथा यदि(total > 0)
    प्रत्यागम (cur * 100) / total;
  प्रत्यागम 0;
}

स्थिर शून्य pgrs_estimates(संरचना pgrs_dir *d,
                           bool total_known,
                           संरचना pgrs_estimate *est)
{
  est->secs = 0;
  est->percent = 0;
  यदि(total_known && (d->speed > 0)) {
    est->secs = d->total_size / d->speed;
    est->percent = pgrs_est_percent(d->total_size, d->cur_size);
  }
}

स्थिर शून्य progress_meter(संरचना Curl_easy *data)
{
  संरचना Progress *p = &data->progress;
  वर्ण max6[6][7];
  संरचना pgrs_estimate dl_estm;
  संरचना pgrs_estimate ul_estm;
  संरचना pgrs_estimate total_estm;
  curl_off_t total_cur_size;
  curl_off_t total_expected_size;
  curl_off_t dl_size;
  वर्ण time_बायाँ[8];
  वर्ण time_total[8];
  वर्ण time_spent[8];
  curl_off_t cur_secs = (curl_off_t)p->total.spent_us / 1000000;

  यदि(!p->headers_out) {
    यदि(data->state.resume_from) {
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
  time2str(time_बायाँ, परिमाण(time_बायाँ),
           total_estm.secs > 0 ? (total_estm.secs - cur_secs) : 0);
  time2str(time_total, परिमाण(time_total), total_estm.secs);
  time2str(time_spent, परिमाण(time_spent), cur_secs);

  /* Get the total amount of data expected to get transferred */
  total_expected_size = p->ul_size_known ? p->ul.total_size : p->ul.cur_size;

  dl_size = p->dl_size_known ? p->dl.total_size : p->dl.cur_size;

  /* integer overflow check */
  यदि((CURL_OFF_T_MAX - total_expected_size) < dl_size)
    total_expected_size = CURL_OFF_T_MAX; /* capped */
  अन्यथा
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
                        परिमाण(max6[2])),              /* total size */
                dl_estm.percent, /* 3 letters */       /* rcvd % */
                max6out(p->dl.cur_size, max6[0],
                        परिमाण(max6[0])),              /* rcvd size */
                ul_estm.percent, /* 3 letters */       /* xfer % */
                max6out(p->ul.cur_size, max6[1],
                        परिमाण(max6[1])),              /* xfer size */
                max6out(p->dl.speed, max6[3],
                        परिमाण(max6[3])),              /* avrg dl speed */
                max6out(p->ul.speed, max6[4],
                        परिमाण(max6[4])),              /* avrg ul speed */
                time_total,    /* 7 letters */         /* total time */
                time_spent,    /* 7 letters */         /* time spent */
                time_बायाँ,     /* 7 letters */         /* time left */
                max6out(p->current_speed, max6[5],
                        परिमाण(max6[5]))               /* current speed */
    );

  /* we flush the output stream to make it appear as soon as possible */
  ख_साफ(data->set.err);
}
#अन्यथा /* CURL_DISABLE_PROGRESS_METER */
#define progress_meter(x) Curl_nop_stmt
#endif

/*
 * Curl_pgrsUpdate() returns 0 for success or the value returned by the
 * progress callback!
 */
स्थिर CURLcode pgrsupdate(संरचना Curl_easy *data, bool showprogress)
{
  यदि(!data->progress.hide) {
    पूर्णाङ्क rc;
    यदि(data->set.fxferinfo) {
      /* There is a callback set, call that */
      संरचना Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_fxferinfo);
      rc = data->set.fxferinfo(data->set.progress_client,
                               data->progress.dl.total_size,
                               data->progress.dl.cur_size,
                               data->progress.ul.total_size,
                               data->progress.ul.cur_size);
      CURL_CBAPI_END(&guard);
      यदि(rc != CURL_PROGRESSFUNC_CONTINUE) {
        यदि(rc) {
          failf(data, "Callback aborted");
          प्रत्यागम CURLE_ABORTED_BY_CALLBACK;
        }
        प्रत्यागम CURLE_OK;
      }
    }
    अन्यथा यदि(data->set.fprogress) {
      /* The older deprecated callback is set, call that */
      संरचना Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_fprogress);
      rc = data->set.fprogress(data->set.progress_client,
                               (द्विगुण)data->progress.dl.total_size,
                               (द्विगुण)data->progress.dl.cur_size,
                               (द्विगुण)data->progress.ul.total_size,
                               (द्विगुण)data->progress.ul.cur_size);
      CURL_CBAPI_END(&guard);
      यदि(rc != CURL_PROGRESSFUNC_CONTINUE) {
        यदि(rc) {
          failf(data, "Callback aborted");
          प्रत्यागम CURLE_ABORTED_BY_CALLBACK;
        }
        प्रत्यागम CURLE_OK;
      }
    }

    यदि(showprogress)
      progress_meter(data);
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode pgrs_update(संरचना Curl_easy *data,
                            ध्रुव संरचना curltime *pnow)
{
  bool showprogress = progress_calc(data, pnow);
  प्रत्यागम pgrsupdate(data, showprogress);
}

CURLcode Curl_pgrsUpdate(संरचना Curl_easy *data)
{
  प्रत्यागम pgrs_update(data, Curl_pgrs_now(data));
}

CURLcode Curl_pgrsUpdateX(संरचना Curl_easy *data,
                          ध्रुव संरचना curltime *pnow)
{
  प्रत्यागम pgrs_update(data, pnow);
}

CURLcode Curl_pgrsCheckX(संरचना Curl_easy *data,
                         ध्रुव संरचना curltime *pnow)
{
  CURLcode result;

  result = pgrs_update(data, pnow);
  यदि(!result && !data->req.done)
    result = pgrs_speedcheck(data, pnow);
  प्रत्यागम result;
}

CURLcode Curl_pgrsCheck(संरचना Curl_easy *data)
{
  प्रत्यागम Curl_pgrsCheckX(data, Curl_pgrs_now(data));
}

/*
 * Update all progress, do not do progress meter/callbacks.
 */
शून्य Curl_pgrsUpdate_nometer(संरचना Curl_easy *data)
{
  (शून्य)progress_calc(data, Curl_pgrs_now(data));
}

शून्य Curl_pgrsCompleted(संरचना Curl_easy *data)
{
  संरचना Progress * ध्रुव p = &data->progress;
  p->total.spent_us = curlx_ptimediff_us(Curl_pgrs_now(data), &p->start);
}

timediff_t Curl_pgrs_since_ms(संरचना Curl_easy *data,
                              ध्रुव संरचना curltime *pnow,
                              timerid timer)
{
  यदि(!pnow)
    pnow = Curl_pgrs_now(data);
  विकल्प(timer) {
  पक्ष TIMER_STARTOP:
    प्रत्यागम (curlx_ptimediff_us(pnow, &data->progress.start) -
            data->progress.delta.startop_us) / 1000;
  पक्ष TIMER_STARTSINGLE:
    प्रत्यागम (curlx_ptimediff_us(pnow, &data->progress.start) -
            data->progress.delta.startsingle_us) / 1000;
  स्वभाव:
    DEBUGASSERT(0);
    प्रत्यागम 0;
  }
}
