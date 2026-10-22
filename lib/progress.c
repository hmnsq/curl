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
#समावेश "curl_setup.h"

#समावेश "urldata.h"
#समावेश "curl_trc.h"
#समावेश "multiif.h"
#समावेश "progress.h"
#समावेश "transfer.h"
#समावेश "curlx/strcopy.h"

#अगर_अघोषित CURL_DISABLE_PROGRESS_METER
/* Provide a string that is 7 letters long (plus the zero byte).

   @unittest 1636 */
UNITTEST व्योम time2str(अक्षर *r, माप_प्रकार rsize, curl_off_t seconds);
UNITTEST व्योम time2str(अक्षर *r, माप_प्रकार rsize, curl_off_t seconds)
{
  curl_off_t h;
  अगर(seconds <= 0) {
    curlx_strcopy(r, rsize, STRCONST("       "));
    वापस;
  }
  h = seconds / 3600;
  अगर(h <= 99) {
    curl_off_t m = (seconds - (h * 3600)) / 60;
    अगर(h <= 9) {
      curl_off_t s = (seconds - (h * 3600)) - (m * 60);
      अगर(h)
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
    अगर(d <= 99)
      curl_msnprintf(r, rsize, "%2" FMT_OFF_T "d %02" FMT_OFF_T "h", d, h);
    अन्यथा अगर(d <= 999)
      curl_msnprintf(r, rsize, "%6" FMT_OFF_T "d", d);
    अन्यथा { /* more than 999 days */
      curl_off_t m = d / 30;
      अगर(m <= 999)
        curl_msnprintf(r, rsize, "%6" FMT_OFF_T "m", m);
      अन्यथा { /* more than 999 months */
        curl_off_t y = d / 365;
        अगर(y <= 99999)
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
UNITTEST अक्षर *max6out(curl_off_t bytes, अक्षर *max6, माप_प्रकार mlen);
UNITTEST अक्षर *max6out(curl_off_t bytes, अक्षर *max6, माप_प्रकार mlen)
{
  /* a signed 64-bit value is 8192 petabytes maximum, shown as
     8.0E (exabytes)*/
  अगर(bytes < 100000)
    curl_msnprintf(max6, mlen, "%6" CURL_FORMAT_CURL_OFF_T, bytes);
  अन्यथा {
    अटल स्थिर अक्षर unit[] = { 'k', 'M', 'G', 'T', 'P', 'E', 0 };
    पूर्णांक k = 0;
    curl_off_t nbytes;
    curl_off_t rest;
    करो {
      nbytes = bytes / 1024;
      अगर(nbytes < 1000)
        अवरोध;
      bytes = nbytes;
      k++;
      DEBUGASSERT(unit[k]);
    } जबतक(unit[k]);
    rest = bytes % 1024;
    अगर(nbytes <= 99)
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
  वापस max6;
}
#पूर्ण_अगर

अटल व्योम pgrs_speedinit(काष्ठा Curl_easy *तथ्य)
{
  स_रखो(&तथ्य->state.keeps_speed, 0, माप(काष्ठा curltime));
}

/*
 * @unittest 1606
 */
UNITTEST CURLcode pgrs_speedcheck(काष्ठा Curl_easy *तथ्य,
                                  स्थिर काष्ठा curltime *pnow);
UNITTEST CURLcode pgrs_speedcheck(काष्ठा Curl_easy *तथ्य,
                                  स्थिर काष्ठा curltime *pnow)
{
  अगर(!तथ्य->set.निचला_speed_time || !तथ्य->set.निचला_speed_limit ||
     Curl_xfer_recv_is_paused(तथ्य) || Curl_xfer_send_is_paused(तथ्य))
    /* A paused transfer is not qualified for speed checks */
    वापस CURLE_OK;

  अगर(तथ्य->progress.current_speed >= 0) {
    अगर(तथ्य->progress.current_speed < तथ्य->set.निचला_speed_limit) {
      अगर(!तथ्य->state.keeps_speed.tv_sec)
        /* under the limit at this moment */
        तथ्य->state.keeps_speed = *pnow;
      अन्यथा {
        /* how long has it been under the limit */
        timediff_t howlong =
          curlx_ptimediff_ms(pnow, &तथ्य->state.keeps_speed);

        अगर(howlong >= तथ्य->set.निचला_speed_time * 1000) {
          /* too long */
          failf(तथ्य, "Operation too slow. Less than %" FMT_OFF_T
                " bytes/sec transferred the last %u seconds",
                तथ्य->set.निचला_speed_limit, तथ्य->set.निचला_speed_time);
          वापस CURLE_OPERATION_TIMEDOUT;
        }
      }
    }
    अन्यथा
      /* faster right now */
      तथ्य->state.keeps_speed.tv_sec = 0;
  }

  /* since low speed limit is enabled, set the expire timer to make this
     connection's speed get checked again in a second */
  Curl_expire_set(तथ्य, EXPIRE_SPEEDCHECK, 1000, pnow);

  वापस CURLE_OK;
}

स्थिर काष्ठा curltime *Curl_pgrs_now(काष्ठा Curl_easy *तथ्य)
{
  curlx_pnow(&तथ्य->progress.now);
  वापस &तथ्य->progress.now;
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

पूर्णांक Curl_pgrsDone(काष्ठा Curl_easy *तथ्य, स्थिर काष्ठा curltime *pnow)
{
  पूर्णांक rc;
  तथ्य->progress.delta.lastshow_us = -1;
  rc = Curl_pgrsUpdateX(तथ्य, pnow); /* the final (forced) update */
  अगर(rc)
    वापस rc;

  अगर(!तथ्य->progress.hide && !तथ्य->progress.callback)
    /* only output if we do not use a progress callback and we are not
     * hidden */
    curl_mfprintf(तथ्य->set.err, "\n");

  वापस 0;
}

व्योम Curl_pgrsReset(काष्ठा Curl_easy *तथ्य)
{
  Curl_pgrs_Set_Upload_गणक(तथ्य, 0);
  तथ्य->progress.dl.cur_आकार = 0;
  Curl_pgrs_Set_Upload_आकार(तथ्य, -1);
  Curl_pgrs_Set_Download_आकार(तथ्य, -1);
  तथ्य->progress.speeder_c = 0; /* reset speed records */
  तथ्य->progress.deliver = 0;
  pgrs_speedinit(तथ्य);
}

/* reset the known transfer sizes */
व्योम Curl_pgrsResetTransferSizes(काष्ठा Curl_easy *तथ्य)
{
  Curl_pgrs_Set_Download_आकार(तथ्य, -1);
  Curl_pgrs_Set_Upload_आकार(तथ्य, -1);
}

व्योम Curl_pgrsRecvPause(काष्ठा Curl_easy *तथ्य, bool enable)
{
  अगर(!enable) {
    तथ्य->progress.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(तथ्य); /* reset low speed measurements */
  }
}

व्योम Curl_pgrsSendPause(काष्ठा Curl_easy *तथ्य, bool enable)
{
  अगर(!enable) {
    तथ्य->progress.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(तथ्य); /* reset low speed measurements */
  }
}

#अगर_घोषित CURLVERBOSE
अटल स्थिर अक्षर * स्थिर pgrs_timer_names[] = {
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

अटल स्थिर अक्षर *pgrs_timer_name(timerid timer)
{
  अगर((माप_प्रकार)timer < CURL_ARRAYSIZE(pgrs_timer_names))
    वापस pgrs_timer_names[(माप_प्रकार)timer];
  वापस "?";
}
#पूर्ण_अगर /* CURLVERBOSE */

/*
 * Curl_pgrsTimeWas(). Store the timestamp time at the given label.
 */
व्योम Curl_pgrsTimeWas(काष्ठा Curl_easy *तथ्य, timerid timer,
                      काष्ठा curltime timestamp)
{
  timediff_t *delta = शून्य;

  चयन(timer) {
  शेष:
  हाल TIMER_NONE:
    /* mistake filter */
    अवरोध;
  हाल TIMER_STARTOP:
    /* This is set at the start of a transfer */
    तथ्य->progress.delta.startop_us =
      curlx_ptimediff_us(&timestamp, &तथ्य->progress.start);
    तथ्य->progress.delta.startqueue_us = तथ्य->progress.delta.startop_us;
    अवरोध;
  हाल TIMER_STARTSINGLE:
    /* This is set at the start of each single transfer */
    तथ्य->progress.delta.startsingle_us =
      curlx_ptimediff_us(&timestamp, &तथ्य->progress.start);
    तथ्य->progress.startransfer_added = FALSE;
    अवरोध;
  हाल TIMER_POSTQUEUE:
    /* Queue time is accumulative from all involved redirects */
    तथ्य->progress.योग.queued_us +=
      curlx_ptimediff_us(&timestamp, &तथ्य->progress.start) -
      तथ्य->progress.delta.startqueue_us;
    अवरोध;
  हाल TIMER_NAMELOOKUP:
    delta = &तथ्य->progress.योग.nslookup_us;
    अवरोध;
  हाल TIMER_CONNECT:
    delta = &तथ्य->progress.योग.connect_us;
    अवरोध;
  हाल TIMER_APPCONNECT:
    delta = &तथ्य->progress.योग.appconnect_us;
    अवरोध;
  हाल TIMER_PRETRANSFER:
    delta = &तथ्य->progress.योग.pretransfer_us;
    अवरोध;
  हाल TIMER_STARTTRANSFER:
    /* prevent updating t_starttransfer unless:
     *   1. this is the first time we are setting t_starttransfer
     *   2. a redirect has occurred since the last time t_starttransfer was set
     * This prevents repeated invocations of the function from incorrectly
     * changing the t_starttransfer time.
     */
    अगर(तथ्य->progress.startransfer_added) {
      CURL_TRC_M(तथ्य, "[%s] ignored", pgrs_timer_name(timer));
      वापस;
    }
    तथ्य->progress.startransfer_added = TRUE;
    delta = &तथ्य->progress.योग.starttransfer_us;
    अवरोध;
  हाल TIMER_POSTRANSFER:
    delta = &तथ्य->progress.योग.posttransfer_us;
    अवरोध;
  हाल TIMER_REDIRECT:
    तथ्य->progress.delta.startredirect_us =
      curlx_ptimediff_us(&timestamp, &तथ्य->progress.start);
    /* transfer starts queueing again */
    तथ्य->progress.delta.startqueue_us =
      तथ्य->progress.delta.startredirect_us;
    अवरोध;
  }
  अगर(delta) {
    timediff_t us = curlx_ptimediff_us(&timestamp, &तथ्य->progress.start) -
                    तथ्य->progress.delta.startsingle_us;
    अगर(us < 1)
      us = 1; /* make sure at least one microsecond passed */
    *delta += us;
    CURL_TRC_M(तथ्य, "[%s] added %" FMT_TIMEDIFF_T "us",
               pgrs_timer_name(timer), us);
  }
  अन्यथा
    CURL_TRC_M(तथ्य, "[%s] set", pgrs_timer_name(timer));
}

/*
 * Curl_pgrsTime(). Store the current time at the given label. This fetches a
 * fresh "now" and returns it.
 *
 * @unittest: 1399
 */
व्योम Curl_pgrsTime(काष्ठा Curl_easy *तथ्य, timerid timer)
{
  Curl_pgrsTimeWas(तथ्य, timer, *Curl_pgrs_now(तथ्य));
}

व्योम Curl_pgrsStart(काष्ठा Curl_easy *तथ्य, स्थिर काष्ठा curltime *pnow)
{
  काष्ठा Progress *p = &तथ्य->progress;

  अगर(!pnow)
    pnow = Curl_pgrs_now(तथ्य);
  p->speeder_c = 0; /* reset the progress meter display */
  p->start = *pnow;
  स_रखो(&p->delta, 0, माप(p->delta));
  स_रखो(&p->योग, 0, माप(p->योग));
  p->startransfer_added = FALSE;
  p->dl.cur_आकार = 0;
  p->ul.cur_आकार = 0;
  /* the sizes are unknown at start */
  p->dl_आकार_known = FALSE;
  p->ul_आकार_known = FALSE;
}

/* check that the 'delta' amount of bytes are okay to deliver to the
   application, or return error if not. */
CURLcode Curl_pgrs_deliver_check(काष्ठा Curl_easy *तथ्य, माप_प्रकार delta)
{
  अगर(तथ्य->set.max_filesize &&
     ((curl_off_t)delta > तथ्य->set.max_filesize - तथ्य->progress.deliver)) {
    failf(तथ्य, "Would have exceeded max file size");
    वापस CURLE_FILESIZE_EXCEEDED;
  }
  वापस CURLE_OK;
}

/* this counts how much data is delivered to the application, which
   in compressed cases may differ from downloaded amount */
व्योम Curl_pgrs_deliver_inc(काष्ठा Curl_easy *तथ्य, माप_प्रकार delta)
{
  तथ्य->progress.deliver += delta;
}

व्योम Curl_pgrs_download_inc(काष्ठा Curl_easy *तथ्य, माप_प्रकार delta)
{
  अगर(delta) {
    तथ्य->progress.dl.cur_आकार += delta;
    Curl_rlimit_drain(&तथ्य->progress.dl.rlimit, delta, शून्य);
  }
}

व्योम Curl_pgrs_upload_inc(काष्ठा Curl_easy *तथ्य, माप_प्रकार delta)
{
  अगर(delta) {
    तथ्य->progress.ul.cur_आकार += delta;
    Curl_rlimit_drain(&तथ्य->progress.ul.rlimit, delta, शून्य);
  }
}

/*
 * Set the number of uploaded bytes so far.
 */
व्योम Curl_pgrs_Set_Upload_गणक(काष्ठा Curl_easy *तथ्य, curl_off_t आकार)
{
  तथ्य->progress.ul.cur_आकार = आकार;
}

व्योम Curl_pgrs_Set_Download_आकार(काष्ठा Curl_easy *तथ्य, curl_off_t आकार)
{
  अगर(आकार >= 0) {
    तथ्य->progress.dl.योग_आकार = आकार;
    तथ्य->progress.dl_आकार_known = TRUE;
  }
  अन्यथा {
    तथ्य->progress.dl.योग_आकार = 0;
    तथ्य->progress.dl_आकार_known = FALSE;
  }
}

व्योम Curl_pgrs_Set_Upload_आकार(काष्ठा Curl_easy *तथ्य, curl_off_t आकार)
{
  अगर(आकार >= 0) {
    तथ्य->progress.ul.योग_आकार = आकार;
    तथ्य->progress.ul_आकार_known = TRUE;
  }
  अन्यथा {
    तथ्य->progress.ul.योग_आकार = 0;
    तथ्य->progress.ul_आकार_known = FALSE;
  }
}

व्योम Curl_pgrs_Early_तथ्य(काष्ठा Curl_easy *तथ्य, curl_off_t sent)
{
  तथ्य->progress.earlydata_sent = sent;
}

/* returns the average speed in bytes / second */
अटल curl_off_t trspeed(curl_off_t आकार, /* number of bytes */
                          curl_off_t us)   /* microseconds */
{
  अगर(us < 1)
    वापस आकार * 1000000;
  अन्यथा अगर(आकार < CURL_OFF_T_MAX / 1000000)
    वापस (आकार * 1000000) / us;
  अन्यथा अगर(us >= 1000000)
    वापस आकार / (us / 1000000);
  अन्यथा
    वापस CURL_OFF_T_MAX;
}

/* returns TRUE if it is time to show the progress meter */
अटल bool progress_calc(काष्ठा Curl_easy *तथ्य,
                          स्थिर काष्ठा curltime *pnow)
{
  काष्ठा Progress * स्थिर p = &तथ्य->progress;
  पूर्णांक i_अग्र, i_oldest, i_latest;
  timediff_t duration_us, elapsed_us;
  curl_off_t amount;

  /* The time spent so far (from the start) in microseconds */
  elapsed_us = curlx_ptimediff_us(pnow, &p->start);
  p->योग.spent_us = elapsed_us;
  p->dl.speed = trspeed(p->dl.cur_आकार, p->योग.spent_us);
  p->ul.speed = trspeed(p->ul.cur_आकार, p->योग.spent_us);

  अगर(!p->speeder_c) { /* no previous record exists */
    p->speed_amount[0] = p->dl.cur_आकार + p->ul.cur_आकार;
    p->speed_time[0] = elapsed_us;
    p->speeder_c++;
    /* use the overall average at the start */
    p->current_speed = p->ul.speed + p->dl.speed;
    p->delta.lastshow_us = elapsed_us;
    वापस TRUE;
  }
  /* We have at least one record now. Where to put the next and
   * where is the latest one? */
  i_अग्र = p->speeder_c % CURL_SPEED_RECORDS;
  i_latest = (i_अग्र > 0) ? (i_अग्र - 1) : (CURL_SPEED_RECORDS - 1);

  /* Make a new record only when some time has passed.
   * Too frequent calls otherwise ruin the history. */
  अगर((elapsed_us - p->speed_time[i_latest]) >= (1000 * 1000)) {
    p->speeder_c++;
    i_latest = i_अग्र;
    p->speed_amount[i_latest] = p->dl.cur_आकार + p->ul.cur_आकार;
    p->speed_time[i_latest] = elapsed_us;
  }
  अन्यथा अगर(तथ्य->req.done) {
    /* When a transfer is done, and we did not have a current speed
     * already, update the last record. Otherwise, stay at the speed
     * we have. The last chunk of data, when rate limiting, would increase
     * reported speed since it no longer measures a full second. */
    अगर(!p->current_speed) {
      p->speed_amount[i_latest] = p->dl.cur_आकार + p->ul.cur_आकार;
      p->speed_time[i_latest] = elapsed_us;
    }
  }
  अन्यथा {
    /* transfer ongoing, wait for more time to pass. */
    वापस FALSE;
  }

  i_oldest = (p->speeder_c < CURL_SPEED_RECORDS) ? 0 :
             ((i_latest + 1) % CURL_SPEED_RECORDS);

  /* How much we transferred between oldest and current records */
  amount = p->speed_amount[i_latest] - p->speed_amount[i_oldest];
  /* How long this took */
  duration_us = p->speed_time[i_latest] - p->speed_time[i_oldest];
  अगर(duration_us <= 0)
    duration_us = 1;

  अगर(amount > (CURL_OFF_T_MAX / 1000000)) {
    /* the 'amount' value is bigger than would fit in 64 bits if
       multiplied with 1000000, so we use the double math for this */
    p->current_speed =
      (curl_off_t)(((द्विगुन)amount * 1000000.0) / (द्विगुन)duration_us);
  }
  अन्यथा {
    p->current_speed = amount * 1000000 / duration_us;
  }

  अगर((p->delta.lastshow_us >= 0) && !तथ्य->req.done &&
     ((elapsed_us - p->delta.lastshow_us) < (1000 * 1000)))
    वापस FALSE;
  p->delta.lastshow_us = elapsed_us;
  वापस TRUE;
}

#अगर_अघोषित CURL_DISABLE_PROGRESS_METER

काष्ठा pgrs_estimate {
  curl_off_t secs;
  curl_off_t percent;
};

अटल curl_off_t pgrs_est_percent(curl_off_t योग, curl_off_t cur)
{
  अगर(योग > 10000)
    वापस cur / (योग / 100);
  अन्यथा अगर(योग > 0)
    वापस (cur * 100) / योग;
  वापस 0;
}

अटल व्योम pgrs_estimates(काष्ठा pgrs_dir *d,
                           bool योग_known,
                           काष्ठा pgrs_estimate *est)
{
  est->secs = 0;
  est->percent = 0;
  अगर(योग_known && (d->speed > 0)) {
    est->secs = d->योग_आकार / d->speed;
    est->percent = pgrs_est_percent(d->योग_आकार, d->cur_आकार);
  }
}

अटल व्योम progress_meter(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Progress *p = &तथ्य->progress;
  अक्षर max6[6][7];
  काष्ठा pgrs_estimate dl_estm;
  काष्ठा pgrs_estimate ul_estm;
  काष्ठा pgrs_estimate योग_estm;
  curl_off_t योग_cur_आकार;
  curl_off_t योग_expected_आकार;
  curl_off_t dl_आकार;
  अक्षर time_बायाँ[8];
  अक्षर time_योग[8];
  अक्षर time_spent[8];
  curl_off_t cur_secs = (curl_off_t)p->योग.spent_us / 1000000;

  अगर(!p->headers_out) {
    अगर(तथ्य->state.resume_from) {
      curl_mfprintf(तथ्य->set.err,
                    "** Resuming transfer from byte position %" FMT_OFF_T "\n",
                    तथ्य->state.resume_from);
    }
    curl_mfprintf(तथ्य->set.err,
                  "  %% Total    %% Received %% Xferd  Average Speed  "
                  "Time    Time    Time   Current\n"
                  "                                 Dload  Upload  "
                  "Total   Spent   Left   Speed\n");
    p->headers_out = TRUE; /* headers are shown */
  }

  /* Figure out the estimated time of arrival for upload and download */
  pgrs_estimates(&p->ul, (bool)p->ul_आकार_known, &ul_estm);
  pgrs_estimates(&p->dl, (bool)p->dl_आकार_known, &dl_estm);

  /* Since both happen at the same time, total expected duration is max. */
  योग_estm.secs = CURLMAX(ul_estm.secs, dl_estm.secs);
  /* create the three time strings */
  time2str(time_बायाँ, माप(time_बायाँ),
           योग_estm.secs > 0 ? (योग_estm.secs - cur_secs) : 0);
  time2str(time_योग, माप(time_योग), योग_estm.secs);
  time2str(time_spent, माप(time_spent), cur_secs);

  /* Get the total amount of data expected to get transferred */
  योग_expected_आकार = p->ul_आकार_known ? p->ul.योग_आकार : p->ul.cur_आकार;

  dl_आकार = p->dl_आकार_known ? p->dl.योग_आकार : p->dl.cur_आकार;

  /* integer overflow check */
  अगर((CURL_OFF_T_MAX - योग_expected_आकार) < dl_आकार)
    योग_expected_आकार = CURL_OFF_T_MAX; /* capped */
  अन्यथा
    योग_expected_आकार += dl_आकार;

  /* We have transferred this much so far */
  योग_cur_आकार = p->dl.cur_आकार + p->ul.cur_आकार;

  /* Get the percentage of data transferred so far */
  योग_estm.percent = pgrs_est_percent(योग_expected_आकार, योग_cur_आकार);

  curl_mfprintf(तथ्य->set.err,
                "\r"
                "%3" FMT_OFF_T " %s "
                "%3" FMT_OFF_T " %s "
                "%3" FMT_OFF_T " %s %s %s %s %s %s %s",
                योग_estm.percent, /* 3 letters */    /* total % */
                max6out(योग_expected_आकार, max6[2],
                        माप(max6[2])),              /* total size */
                dl_estm.percent, /* 3 letters */       /* rcvd % */
                max6out(p->dl.cur_आकार, max6[0],
                        माप(max6[0])),              /* rcvd size */
                ul_estm.percent, /* 3 letters */       /* xfer % */
                max6out(p->ul.cur_आकार, max6[1],
                        माप(max6[1])),              /* xfer size */
                max6out(p->dl.speed, max6[3],
                        माप(max6[3])),              /* avrg dl speed */
                max6out(p->ul.speed, max6[4],
                        माप(max6[4])),              /* avrg ul speed */
                time_योग,    /* 7 letters */         /* total time */
                time_spent,    /* 7 letters */         /* time spent */
                time_बायाँ,     /* 7 letters */         /* time left */
                max6out(p->current_speed, max6[5],
                        माप(max6[5]))               /* current speed */
    );

  /* we flush the output stream to make it appear as soon as possible */
  ख_साफ(तथ्य->set.err);
}
#अन्यथा /* CURL_DISABLE_PROGRESS_METER */
#घोषणा progress_meter(x) Curl_nop_stmt
#पूर्ण_अगर

/*
 * Curl_pgrsUpdate() returns 0 for success or the value returned by the
 * progress callback!
 */
अटल CURLcode pgrsupdate(काष्ठा Curl_easy *तथ्य, bool showprogress)
{
  अगर(!तथ्य->progress.hide) {
    पूर्णांक rc;
    अगर(तथ्य->set.fxferinfo) {
      /* There is a callback set, call that */
      काष्ठा Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, तथ्य, easy_fxferinfo);
      rc = तथ्य->set.fxferinfo(तथ्य->set.progress_client,
                               तथ्य->progress.dl.योग_आकार,
                               तथ्य->progress.dl.cur_आकार,
                               तथ्य->progress.ul.योग_आकार,
                               तथ्य->progress.ul.cur_आकार);
      CURL_CBAPI_END(&guard);
      अगर(rc != CURL_PROGRESSFUNC_CONTINUE) {
        अगर(rc) {
          failf(तथ्य, "Callback aborted");
          वापस CURLE_ABORTED_BY_CALLBACK;
        }
        वापस CURLE_OK;
      }
    }
    अन्यथा अगर(तथ्य->set.fprogress) {
      /* The older deprecated callback is set, call that */
      काष्ठा Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, तथ्य, easy_fprogress);
      rc = तथ्य->set.fprogress(तथ्य->set.progress_client,
                               (द्विगुन)तथ्य->progress.dl.योग_आकार,
                               (द्विगुन)तथ्य->progress.dl.cur_आकार,
                               (द्विगुन)तथ्य->progress.ul.योग_आकार,
                               (द्विगुन)तथ्य->progress.ul.cur_आकार);
      CURL_CBAPI_END(&guard);
      अगर(rc != CURL_PROGRESSFUNC_CONTINUE) {
        अगर(rc) {
          failf(तथ्य, "Callback aborted");
          वापस CURLE_ABORTED_BY_CALLBACK;
        }
        वापस CURLE_OK;
      }
    }

    अगर(showprogress)
      progress_meter(तथ्य);
  }

  वापस CURLE_OK;
}

अटल CURLcode pgrs_update(काष्ठा Curl_easy *तथ्य,
                            स्थिर काष्ठा curltime *pnow)
{
  bool showprogress = progress_calc(तथ्य, pnow);
  वापस pgrsupdate(तथ्य, showprogress);
}

CURLcode Curl_pgrsUpdate(काष्ठा Curl_easy *तथ्य)
{
  वापस pgrs_update(तथ्य, Curl_pgrs_now(तथ्य));
}

CURLcode Curl_pgrsUpdateX(काष्ठा Curl_easy *तथ्य,
                          स्थिर काष्ठा curltime *pnow)
{
  वापस pgrs_update(तथ्य, pnow);
}

CURLcode Curl_pgrsCheckX(काष्ठा Curl_easy *तथ्य,
                         स्थिर काष्ठा curltime *pnow)
{
  CURLcode परिणाम;

  परिणाम = pgrs_update(तथ्य, pnow);
  अगर(!परिणाम && !तथ्य->req.done)
    परिणाम = pgrs_speedcheck(तथ्य, pnow);
  वापस परिणाम;
}

CURLcode Curl_pgrsCheck(काष्ठा Curl_easy *तथ्य)
{
  वापस Curl_pgrsCheckX(तथ्य, Curl_pgrs_now(तथ्य));
}

/*
 * Update all progress, do not do progress meter/callbacks.
 */
व्योम Curl_pgrsUpdate_nometer(काष्ठा Curl_easy *तथ्य)
{
  (व्योम)progress_calc(तथ्य, Curl_pgrs_now(तथ्य));
}

व्योम Curl_pgrsCompleted(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Progress * स्थिर p = &तथ्य->progress;
  p->योग.spent_us = curlx_ptimediff_us(Curl_pgrs_now(तथ्य), &p->start);
}

timediff_t Curl_pgrs_since_ms(काष्ठा Curl_easy *तथ्य,
                              स्थिर काष्ठा curltime *pnow,
                              timerid timer)
{
  अगर(!pnow)
    pnow = Curl_pgrs_now(तथ्य);
  चयन(timer) {
  हाल TIMER_STARTOP:
    वापस (curlx_ptimediff_us(pnow, &तथ्य->progress.start) -
            तथ्य->progress.delta.startop_us) / 1000;
  हाल TIMER_STARTSINGLE:
    वापस (curlx_ptimediff_us(pnow, &तथ्य->progress.start) -
            तथ्य->progress.delta.startsingle_us) / 1000;
  शेष:
    DEBUGASSERT(0);
    वापस 0;
  }
}
