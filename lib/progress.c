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

#यदि_अपरिभाषितम् CURL_DISABLE_प्रगतिः_METER
/* Provide a string that is 7 letters long (plus the zero byte).

   @unittest 1636 */
UNITTEST शून्य time2str(वर्ण *r, size_t rsize, curl_off_t seconds);
UNITTEST शून्य time2str(वर्ण *r, size_t rsize, curl_off_t seconds)
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
UNITTEST वर्ण *max6out(curl_off_t अष्टकानि, वर्ण *max6, size_t mlen);
UNITTEST वर्ण *max6out(curl_off_t अष्टकानि, वर्ण *max6, size_t mlen)
{
  /* a signed 64-bit value is 8192 petabytes maximum, shown as
     8.0E (exabytes)*/
  यदि(अष्टकानि < 100000)
    curl_msnprintf(max6, mlen, "%6" CURL_FORMAT_CURL_OFF_T, अष्टकानि);
  अन्यथा {
    स्थिर ध्रुव वर्ण unit[] = { 'k', 'M', 'G', 'T', 'P', 'E', 0 };
    पूर्णाङ्क k = 0;
    curl_off_t nbytes;
    curl_off_t rest;
    कुरु {
      nbytes = अष्टकानि / 1024;
      यदि(nbytes < 1000)
        भङ्ग;
      अष्टकानि = nbytes;
      k++;
      DEBUGASSERT(unit[k]);
    } यावत्(unit[k]);
    rest = अष्टकानि % 1024;
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
#यदि_अन्तः

स्थिर शून्य pgrs_speedinit(संरचना Curl_easy *दत्त)
{
  स्मृतिं_पूरय(&दत्त->state.keeps_जवः, 0, परिमाण(संरचना curltime));
}

/*
 * @unittest 1606
 */
UNITTEST CURLcode pgrs_speedcheck(संरचना Curl_easy *दत्त,
                                  ध्रुव संरचना curltime *pnow);
UNITTEST CURLcode pgrs_speedcheck(संरचना Curl_easy *दत्त,
                                  ध्रुव संरचना curltime *pnow)
{
  यदि(!दत्त->समुच्चयः.नीचः_जवः_time || !दत्त->समुच्चयः.नीचः_जवः_सीमा ||
     Curl_xfer_recv_is_paused(दत्त) || Curl_xfer_send_is_paused(दत्त))
    /* A paused transfer is not qualified for speed checks */
    प्रत्यागम CURLE_OK;

  यदि(दत्त->प्रगतिः.current_जवः >= 0) {
    यदि(दत्त->प्रगतिः.current_जवः < दत्त->समुच्चयः.नीचः_जवः_सीमा) {
      यदि(!दत्त->state.keeps_जवः.tv_sec)
        /* under the limit at this moment */
        दत्त->state.keeps_जवः = *pnow;
      अन्यथा {
        /* how long has it been under the limit */
        timediff_t howlong =
          curlx_ptimediff_ms(pnow, &दत्त->state.keeps_जवः);

        यदि(howlong >= दत्त->समुच्चयः.नीचः_जवः_time * 1000) {
          /* too long */
          failf(दत्त, "Operation too slow. Less than %" FMT_OFF_T
                " bytes/sec transferred the last %u seconds",
                दत्त->समुच्चयः.नीचः_जवः_सीमा, दत्त->समुच्चयः.नीचः_जवः_time);
          प्रत्यागम CURLE_OPERATION_TIMEDOUT;
        }
      }
    }
    अन्यथा
      /* faster right now */
      दत्त->state.keeps_जवः.tv_sec = 0;
  }

  /* since low speed limit is enabled, set the expire timer to make this
     connection's speed get checked again in a second */
  Curl_expire_समुच्चयः(दत्त, EXPIRE_SPEEDCHECK, 1000, pnow);

  प्रत्यागम CURLE_OK;
}

ध्रुव संरचना curltime *Curl_pgrs_now(संरचना Curl_easy *दत्त)
{
  curlx_pnow(&दत्त->प्रगतिः.now);
  प्रत्यागम &दत्त->प्रगतिः.now;
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

पूर्णाङ्क Curl_pgrs_समाप्तम्(संरचना Curl_easy *दत्त, ध्रुव संरचना curltime *pnow)
{
  पूर्णाङ्क rc;
  दत्त->प्रगतिः.delta.lastshow_us = -1;
  rc = Curl_pgrsUpdateX(दत्त, pnow); /* the final (forced) update */
  यदि(rc)
    प्रत्यागम rc;

  यदि(!दत्त->प्रगतिः.hide && !दत्त->प्रगतिः.callback)
    /* only output if we do not use a progress callback and we are not
     * hidden */
    curl_mfprintf(दत्त->समुच्चयः.err, "\n");

  प्रत्यागम 0;
}

शून्य Curl_pgrsReset(संरचना Curl_easy *दत्त)
{
  Curl_pgrs_समुच्चयः_Upload_गणक(दत्त, 0);
  दत्त->प्रगतिः.dl.cur_आकार = 0;
  Curl_pgrs_समुच्चयः_Upload_आकार(दत्त, -1);
  Curl_pgrs_समुच्चयः_Download_आकार(दत्त, -1);
  दत्त->प्रगतिः.speeder_c = 0; /* reset speed records */
  दत्त->प्रगतिः.deliver = 0;
  pgrs_speedinit(दत्त);
}

/* reset the known transfer sizes */
शून्य Curl_pgrsResetTransferSizes(संरचना Curl_easy *दत्त)
{
  Curl_pgrs_समुच्चयः_Download_आकार(दत्त, -1);
  Curl_pgrs_समुच्चयः_Upload_आकार(दत्त, -1);
}

शून्य Curl_pgrsRecvPause(संरचना Curl_easy *दत्त, bool enable)
{
  यदि(!enable) {
    दत्त->प्रगतिः.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(दत्त); /* reset low speed measurements */
  }
}

शून्य Curl_pgrsSendPause(संरचना Curl_easy *दत्त, bool enable)
{
  यदि(!enable) {
    दत्त->प्रगतिः.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(दत्त); /* reset low speed measurements */
  }
}

#यदि_परिभाषितम् CURLVERBOSE
स्थिर ध्रुव वर्ण * ध्रुव pgrs_timer_नामानि[] = {
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
  यदि((size_t)timer < CURL_ARRAYSIZE(pgrs_timer_नामानि))
    प्रत्यागम pgrs_timer_नामानि[(size_t)timer];
  प्रत्यागम "?";
}
#यदि_अन्तः /* CURLVERBOSE */

/*
 * Curl_pgrsTimeWas(). Store the timestamp time at the given label.
 */
शून्य Curl_pgrsTimeWas(संरचना Curl_easy *दत्त, timerid timer,
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
    दत्त->प्रगतिः.delta.startop_us =
      curlx_ptimediff_us(&timestamp, &दत्त->प्रगतिः.start);
    दत्त->प्रगतिः.delta.startqueue_us = दत्त->प्रगतिः.delta.startop_us;
    भङ्ग;
  पक्ष TIMER_STARTSINGLE:
    /* This is set at the start of each single transfer */
    दत्त->प्रगतिः.delta.startsingle_us =
      curlx_ptimediff_us(&timestamp, &दत्त->प्रगतिः.start);
    दत्त->प्रगतिः.startransfer_added = FALSE;
    भङ्ग;
  पक्ष TIMER_POSTQUEUE:
    /* Queue time is accumulative from all involved redirects */
    दत्त->प्रगतिः.योग.queued_us +=
      curlx_ptimediff_us(&timestamp, &दत्त->प्रगतिः.start) -
      दत्त->प्रगतिः.delta.startqueue_us;
    भङ्ग;
  पक्ष TIMER_NAMELOOKUP:
    delta = &दत्त->प्रगतिः.योग.nslookup_us;
    भङ्ग;
  पक्ष TIMER_CONNECT:
    delta = &दत्त->प्रगतिः.योग.connect_us;
    भङ्ग;
  पक्ष TIMER_APPCONNECT:
    delta = &दत्त->प्रगतिः.योग.appconnect_us;
    भङ्ग;
  पक्ष TIMER_PRETRANSFER:
    delta = &दत्त->प्रगतिः.योग.pretransfer_us;
    भङ्ग;
  पक्ष TIMER_STARTTRANSFER:
    /* prevent updating t_starttransfer unless:
     *   1. this is the first time we are setting t_starttransfer
     *   2. a redirect has occurred since the last time t_starttransfer was set
     * This prevents repeated invocations of the function from incorrectly
     * changing the t_starttransfer time.
     */
    यदि(दत्त->प्रगतिः.startransfer_added) {
      CURL_TRC_M(दत्त, "[%s] ignored", pgrs_timer_name(timer));
      प्रत्यागम;
    }
    दत्त->प्रगतिः.startransfer_added = TRUE;
    delta = &दत्त->प्रगतिः.योग.starttransfer_us;
    भङ्ग;
  पक्ष TIMER_POSTRANSFER:
    delta = &दत्त->प्रगतिः.योग.posttransfer_us;
    भङ्ग;
  पक्ष TIMER_REDIRECT:
    दत्त->प्रगतिः.delta.startredirect_us =
      curlx_ptimediff_us(&timestamp, &दत्त->प्रगतिः.start);
    /* transfer starts queueing again */
    दत्त->प्रगतिः.delta.startqueue_us =
      दत्त->प्रगतिः.delta.startredirect_us;
    भङ्ग;
  }
  यदि(delta) {
    timediff_t us = curlx_ptimediff_us(&timestamp, &दत्त->प्रगतिः.start) -
                    दत्त->प्रगतिः.delta.startsingle_us;
    यदि(us < 1)
      us = 1; /* make sure at least one microsecond passed */
    *delta += us;
    CURL_TRC_M(दत्त, "[%s] added %" FMT_TIMEDIFF_T "us",
               pgrs_timer_name(timer), us);
  }
  अन्यथा
    CURL_TRC_M(दत्त, "[%s] set", pgrs_timer_name(timer));
}

/*
 * Curl_pgrsTime(). Store the current time at the given label. This fetches a
 * fresh "now" and returns it.
 *
 * @unittest: 1399
 */
शून्य Curl_pgrsTime(संरचना Curl_easy *दत्त, timerid timer)
{
  Curl_pgrsTimeWas(दत्त, timer, *Curl_pgrs_now(दत्त));
}

शून्य Curl_pgrsStart(संरचना Curl_easy *दत्त, ध्रुव संरचना curltime *pnow)
{
  संरचना प्रगतिः_2 *p = &दत्त->प्रगतिः;

  यदि(!pnow)
    pnow = Curl_pgrs_now(दत्त);
  p->speeder_c = 0; /* reset the progress meter display */
  p->start = *pnow;
  स्मृतिं_पूरय(&p->delta, 0, परिमाण(p->delta));
  स्मृतिं_पूरय(&p->योग, 0, परिमाण(p->योग));
  p->startransfer_added = FALSE;
  p->dl.cur_आकार = 0;
  p->ul.cur_आकार = 0;
  /* the sizes are unknown at start */
  p->dl_आकार_ज्ञातम् = FALSE;
  p->ul_आकार_ज्ञातम् = FALSE;
}

/* check that the 'delta' amount of bytes are okay to deliver to the
   application, or return error if not. */
CURLcode Curl_pgrs_deliver_check(संरचना Curl_easy *दत्त, size_t delta)
{
  यदि(दत्त->समुच्चयः.max_filesize &&
     ((curl_off_t)delta > दत्त->समुच्चयः.max_filesize - दत्त->प्रगतिः.deliver)) {
    failf(दत्त, "Would have exceeded max file size");
    प्रत्यागम CURLE_FILESIZE_EXCEEDED;
  }
  प्रत्यागम CURLE_OK;
}

/* this counts how much data is delivered to the application, which
   in compressed cases may differ from downloaded amount */
शून्य Curl_pgrs_deliver_inc(संरचना Curl_easy *दत्त, size_t delta)
{
  दत्त->प्रगतिः.deliver += delta;
}

शून्य Curl_pgrs_download_inc(संरचना Curl_easy *दत्त, size_t delta)
{
  यदि(delta) {
    दत्त->प्रगतिः.dl.cur_आकार += delta;
    Curl_rlimit_drain(&दत्त->प्रगतिः.dl.rlimit, delta, रिक्त);
  }
}

शून्य Curl_pgrs_upload_inc(संरचना Curl_easy *दत्त, size_t delta)
{
  यदि(delta) {
    दत्त->प्रगतिः.ul.cur_आकार += delta;
    Curl_rlimit_drain(&दत्त->प्रगतिः.ul.rlimit, delta, रिक्त);
  }
}

/*
 * Set the number of uploaded bytes so far.
 */
शून्य Curl_pgrs_समुच्चयः_Upload_गणक(संरचना Curl_easy *दत्त, curl_off_t आकार)
{
  दत्त->प्रगतिः.ul.cur_आकार = आकार;
}

शून्य Curl_pgrs_समुच्चयः_Download_आकार(संरचना Curl_easy *दत्त, curl_off_t आकार)
{
  यदि(आकार >= 0) {
    दत्त->प्रगतिः.dl.योग_आकार = आकार;
    दत्त->प्रगतिः.dl_आकार_ज्ञातम् = TRUE;
  }
  अन्यथा {
    दत्त->प्रगतिः.dl.योग_आकार = 0;
    दत्त->प्रगतिः.dl_आकार_ज्ञातम् = FALSE;
  }
}

शून्य Curl_pgrs_समुच्चयः_Upload_आकार(संरचना Curl_easy *दत्त, curl_off_t आकार)
{
  यदि(आकार >= 0) {
    दत्त->प्रगतिः.ul.योग_आकार = आकार;
    दत्त->प्रगतिः.ul_आकार_ज्ञातम् = TRUE;
  }
  अन्यथा {
    दत्त->प्रगतिः.ul.योग_आकार = 0;
    दत्त->प्रगतिः.ul_आकार_ज्ञातम् = FALSE;
  }
}

शून्य Curl_pgrs_Early_दत्त(संरचना Curl_easy *दत्त, curl_off_t प्रेषितम्)
{
  दत्त->प्रगतिः.earlydata_प्रेषितम् = प्रेषितम्;
}

/* returns the average speed in bytes / second */
स्थिर curl_off_t trspeed(curl_off_t आकार, /* number of bytes */
                          curl_off_t us)   /* microseconds */
{
  यदि(us < 1)
    प्रत्यागम आकार * 1000000;
  अन्यथा यदि(आकार < CURL_OFF_T_MAX / 1000000)
    प्रत्यागम (आकार * 1000000) / us;
  अन्यथा यदि(us >= 1000000)
    प्रत्यागम आकार / (us / 1000000);
  अन्यथा
    प्रत्यागम CURL_OFF_T_MAX;
}

/* returns TRUE if it is time to show the progress meter */
स्थिर bool प्रगतिः_calc(संरचना Curl_easy *दत्त,
                          ध्रुव संरचना curltime *pnow)
{
  संरचना प्रगतिः_2 * ध्रुव p = &दत्त->प्रगतिः;
  पूर्णाङ्क i_अग्र, i_oldest, i_latest;
  timediff_t duration_us, elapsed_us;
  curl_off_t amount;

  /* The time spent so far (from the start) in microseconds */
  elapsed_us = curlx_ptimediff_us(pnow, &p->start);
  p->योग.spent_us = elapsed_us;
  p->dl.जवः = trspeed(p->dl.cur_आकार, p->योग.spent_us);
  p->ul.जवः = trspeed(p->ul.cur_आकार, p->योग.spent_us);

  यदि(!p->speeder_c) { /* no previous record exists */
    p->जवः_amount[0] = p->dl.cur_आकार + p->ul.cur_आकार;
    p->जवः_time[0] = elapsed_us;
    p->speeder_c++;
    /* use the overall average at the start */
    p->current_जवः = p->ul.जवः + p->dl.जवः;
    p->delta.lastshow_us = elapsed_us;
    प्रत्यागम TRUE;
  }
  /* We have at least one record now. Where to put the next and
   * where is the latest one? */
  i_अग्र = p->speeder_c % CURL_जवः_लेखाः;
  i_latest = (i_अग्र > 0) ? (i_अग्र - 1) : (CURL_जवः_लेखाः - 1);

  /* Make a new record only when some time has passed.
   * Too frequent calls otherwise ruin the history. */
  यदि((elapsed_us - p->जवः_time[i_latest]) >= (1000 * 1000)) {
    p->speeder_c++;
    i_latest = i_अग्र;
    p->जवः_amount[i_latest] = p->dl.cur_आकार + p->ul.cur_आकार;
    p->जवः_time[i_latest] = elapsed_us;
  }
  अन्यथा यदि(दत्त->req.समाप्तम्) {
    /* When a transfer is done, and we did not have a current speed
     * already, update the last record. Otherwise, stay at the speed
     * we have. The last chunk of data, when rate limiting, would increase
     * reported speed since it no longer measures a full second. */
    यदि(!p->current_जवः) {
      p->जवः_amount[i_latest] = p->dl.cur_आकार + p->ul.cur_आकार;
      p->जवः_time[i_latest] = elapsed_us;
    }
  }
  अन्यथा {
    /* transfer ongoing, wait for more time to pass. */
    प्रत्यागम FALSE;
  }

  i_oldest = (p->speeder_c < CURL_जवः_लेखाः) ? 0 :
             ((i_latest + 1) % CURL_जवः_लेखाः);

  /* How much we transferred between oldest and current records */
  amount = p->जवः_amount[i_latest] - p->जवः_amount[i_oldest];
  /* How long this took */
  duration_us = p->जवः_time[i_latest] - p->जवः_time[i_oldest];
  यदि(duration_us <= 0)
    duration_us = 1;

  यदि(amount > (CURL_OFF_T_MAX / 1000000)) {
    /* the 'amount' value is bigger than would fit in 64 bits if
       multiplied with 1000000, so we use the double math for this */
    p->current_जवः =
      (curl_off_t)(((द्विगुण)amount * 1000000.0) / (द्विगुण)duration_us);
  }
  अन्यथा {
    p->current_जवः = amount * 1000000 / duration_us;
  }

  यदि((p->delta.lastshow_us >= 0) && !दत्त->req.समाप्तम् &&
     ((elapsed_us - p->delta.lastshow_us) < (1000 * 1000)))
    प्रत्यागम FALSE;
  p->delta.lastshow_us = elapsed_us;
  प्रत्यागम TRUE;
}

#यदि_अपरिभाषितम् CURL_DISABLE_प्रगतिः_METER

संरचना pgrs_estimate {
  curl_off_t secs;
  curl_off_t percent;
};

स्थिर curl_off_t pgrs_est_percent(curl_off_t योग, curl_off_t cur)
{
  यदि(योग > 10000)
    प्रत्यागम cur / (योग / 100);
  अन्यथा यदि(योग > 0)
    प्रत्यागम (cur * 100) / योग;
  प्रत्यागम 0;
}

स्थिर शून्य pgrs_estimates(संरचना pgrs_dir *d,
                           bool योग_ज्ञातम्,
                           संरचना pgrs_estimate *est)
{
  est->secs = 0;
  est->percent = 0;
  यदि(योग_ज्ञातम् && (d->जवः > 0)) {
    est->secs = d->योग_आकार / d->जवः;
    est->percent = pgrs_est_percent(d->योग_आकार, d->cur_आकार);
  }
}

स्थिर शून्य प्रगतिः_meter(संरचना Curl_easy *दत्त)
{
  संरचना प्रगतिः_2 *p = &दत्त->प्रगतिः;
  वर्ण max6[6][7];
  संरचना pgrs_estimate dl_estm;
  संरचना pgrs_estimate ul_estm;
  संरचना pgrs_estimate योग_estm;
  curl_off_t योग_cur_आकार;
  curl_off_t योग_expected_आकार;
  curl_off_t dl_आकार;
  वर्ण time_वामः[8];
  वर्ण time_योग[8];
  वर्ण time_spent[8];
  curl_off_t cur_secs = (curl_off_t)p->योग.spent_us / 1000000;

  यदि(!p->शीर्षकाणि_बहिर्गमनम्) {
    यदि(दत्त->state.resume_from) {
      curl_mfprintf(दत्त->समुच्चयः.err,
                    "** Resuming transfer from byte position %" FMT_OFF_T "\n",
                    दत्त->state.resume_from);
    }
    curl_mfprintf(दत्त->समुच्चयः.err,
                  "  %% Total    %% Received %% Xferd  Average Speed  "
                  "Time    Time    Time   Current\n"
                  "                                 Dload  Upload  "
                  "Total   Spent   Left   Speed\n");
    p->शीर्षकाणि_बहिर्गमनम् = TRUE; /* headers are shown */
  }

  /* Figure out the estimated time of arrival for upload and download */
  pgrs_estimates(&p->ul, (bool)p->ul_आकार_ज्ञातम्, &ul_estm);
  pgrs_estimates(&p->dl, (bool)p->dl_आकार_ज्ञातम्, &dl_estm);

  /* Since both happen at the same time, total expected duration is max. */
  योग_estm.secs = CURLMAX(ul_estm.secs, dl_estm.secs);
  /* create the three time strings */
  time2str(time_वामः, परिमाण(time_वामः),
           योग_estm.secs > 0 ? (योग_estm.secs - cur_secs) : 0);
  time2str(time_योग, परिमाण(time_योग), योग_estm.secs);
  time2str(time_spent, परिमाण(time_spent), cur_secs);

  /* Get the total amount of data expected to get transferred */
  योग_expected_आकार = p->ul_आकार_ज्ञातम् ? p->ul.योग_आकार : p->ul.cur_आकार;

  dl_आकार = p->dl_आकार_ज्ञातम् ? p->dl.योग_आकार : p->dl.cur_आकार;

  /* integer overflow check */
  यदि((CURL_OFF_T_MAX - योग_expected_आकार) < dl_आकार)
    योग_expected_आकार = CURL_OFF_T_MAX; /* capped */
  अन्यथा
    योग_expected_आकार += dl_आकार;

  /* We have transferred this much so far */
  योग_cur_आकार = p->dl.cur_आकार + p->ul.cur_आकार;

  /* Get the percentage of data transferred so far */
  योग_estm.percent = pgrs_est_percent(योग_expected_आकार, योग_cur_आकार);

  curl_mfprintf(दत्त->समुच्चयः.err,
                "\r"
                "%3" FMT_OFF_T " %s "
                "%3" FMT_OFF_T " %s "
                "%3" FMT_OFF_T " %s %s %s %s %s %s %s",
                योग_estm.percent, /* 3 letters */    /* total % */
                max6out(योग_expected_आकार, max6[2],
                        परिमाण(max6[2])),              /* total size */
                dl_estm.percent, /* 3 letters */       /* rcvd % */
                max6out(p->dl.cur_आकार, max6[0],
                        परिमाण(max6[0])),              /* rcvd size */
                ul_estm.percent, /* 3 letters */       /* xfer % */
                max6out(p->ul.cur_आकार, max6[1],
                        परिमाण(max6[1])),              /* xfer size */
                max6out(p->dl.जवः, max6[3],
                        परिमाण(max6[3])),              /* avrg dl speed */
                max6out(p->ul.जवः, max6[4],
                        परिमाण(max6[4])),              /* avrg ul speed */
                time_योग,    /* 7 letters */         /* total time */
                time_spent,    /* 7 letters */         /* time spent */
                time_वामः,     /* 7 letters */         /* time left */
                max6out(p->current_जवः, max6[5],
                        परिमाण(max6[5]))               /* current speed */
    );

  /* we flush the output stream to make it appear as soon as possible */
  fflush(दत्त->समुच्चयः.err);
}
#अन्यथा /* CURL_DISABLE_PROGRESS_METER */
#परिभाषय प्रगतिः_meter(x) Curl_nop_stmt
#यदि_अन्तः

/*
 * Curl_pgrsUpdate() returns 0 for success or the value returned by the
 * progress callback!
 */
स्थिर CURLcode pgrsupdate(संरचना Curl_easy *दत्त, bool showprogress)
{
  यदि(!दत्त->प्रगतिः.hide) {
    पूर्णाङ्क rc;
    यदि(दत्त->समुच्चयः.fxferinfo) {
      /* There is a callback set, call that */
      संरचना Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, दत्त, easy_fxferinfo);
      rc = दत्त->समुच्चयः.fxferinfo(दत्त->समुच्चयः.प्रगतिः_client,
                               दत्त->प्रगतिः.dl.योग_आकार,
                               दत्त->प्रगतिः.dl.cur_आकार,
                               दत्त->प्रगतिः.ul.योग_आकार,
                               दत्त->प्रगतिः.ul.cur_आकार);
      CURL_CBAPI_अन्तः(&guard);
      यदि(rc != CURL_PROGRESSFUNC_CONTINUE) {
        यदि(rc) {
          failf(दत्त, "Callback aborted");
          प्रत्यागम CURLE_ABORTED_BY_CALLBACK;
        }
        प्रत्यागम CURLE_OK;
      }
    }
    अन्यथा यदि(दत्त->समुच्चयः.fprogress) {
      /* The older deprecated callback is set, call that */
      संरचना Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, दत्त, easy_fprogress);
      rc = दत्त->समुच्चयः.fprogress(दत्त->समुच्चयः.प्रगतिः_client,
                               (द्विगुण)दत्त->प्रगतिः.dl.योग_आकार,
                               (द्विगुण)दत्त->प्रगतिः.dl.cur_आकार,
                               (द्विगुण)दत्त->प्रगतिः.ul.योग_आकार,
                               (द्विगुण)दत्त->प्रगतिः.ul.cur_आकार);
      CURL_CBAPI_अन्तः(&guard);
      यदि(rc != CURL_PROGRESSFUNC_CONTINUE) {
        यदि(rc) {
          failf(दत्त, "Callback aborted");
          प्रत्यागम CURLE_ABORTED_BY_CALLBACK;
        }
        प्रत्यागम CURLE_OK;
      }
    }

    यदि(showprogress)
      प्रगतिः_meter(दत्त);
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode pgrs_update(संरचना Curl_easy *दत्त,
                            ध्रुव संरचना curltime *pnow)
{
  bool showprogress = प्रगतिः_calc(दत्त, pnow);
  प्रत्यागम pgrsupdate(दत्त, showprogress);
}

CURLcode Curl_pgrsUpdate(संरचना Curl_easy *दत्त)
{
  प्रत्यागम pgrs_update(दत्त, Curl_pgrs_now(दत्त));
}

CURLcode Curl_pgrsUpdateX(संरचना Curl_easy *दत्त,
                          ध्रुव संरचना curltime *pnow)
{
  प्रत्यागम pgrs_update(दत्त, pnow);
}

CURLcode Curl_pgrsCheckX(संरचना Curl_easy *दत्त,
                         ध्रुव संरचना curltime *pnow)
{
  CURLcode परिणाम;

  परिणाम = pgrs_update(दत्त, pnow);
  यदि(!परिणाम && !दत्त->req.समाप्तम्)
    परिणाम = pgrs_speedcheck(दत्त, pnow);
  प्रत्यागम परिणाम;
}

CURLcode Curl_pgrsCheck(संरचना Curl_easy *दत्त)
{
  प्रत्यागम Curl_pgrsCheckX(दत्त, Curl_pgrs_now(दत्त));
}

/*
 * Update all progress, do not do progress meter/callbacks.
 */
शून्य Curl_pgrsUpdate_nometer(संरचना Curl_easy *दत्त)
{
  (शून्य)प्रगतिः_calc(दत्त, Curl_pgrs_now(दत्त));
}

शून्य Curl_pgrsCompleted(संरचना Curl_easy *दत्त)
{
  संरचना प्रगतिः_2 * ध्रुव p = &दत्त->प्रगतिः;
  p->योग.spent_us = curlx_ptimediff_us(Curl_pgrs_now(दत्त), &p->start);
}

timediff_t Curl_pgrs_since_ms(संरचना Curl_easy *दत्त,
                              ध्रुव संरचना curltime *pnow,
                              timerid timer)
{
  यदि(!pnow)
    pnow = Curl_pgrs_now(दत्त);
  विकल्प(timer) {
  पक्ष TIMER_STARTOP:
    प्रत्यागम (curlx_ptimediff_us(pnow, &दत्त->प्रगतिः.start) -
            दत्त->प्रगतिः.delta.startop_us) / 1000;
  पक्ष TIMER_STARTSINGLE:
    प्रत्यागम (curlx_ptimediff_us(pnow, &दत्त->प्रगतिः.start) -
            दत्त->प्रगतिः.delta.startsingle_us) / 1000;
  स्वभाव:
    DEBUGASSERT(0);
    प्रत्यागम 0;
  }
}
