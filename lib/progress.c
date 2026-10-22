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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strcopy.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_ਤਰੱਕੀ_METER
/* Provide a string that is 7 letters long (plus the zero byte).

   @unittest 1636 */
UNITTEST ਖਾਲੀ time2str(ਅੱਖਰ *r, size_t rsize, curl_off_t seconds);
UNITTEST ਖਾਲੀ time2str(ਅੱਖਰ *r, size_t rsize, curl_off_t seconds)
{
  curl_off_t h;
  ਜੇ(seconds <= 0) {
    curlx_strcopy(r, rsize, STRCONST("       "));
    ਵਾਪਸ;
  }
  h = seconds / 3600;
  ਜੇ(h <= 99) {
    curl_off_t m = (seconds - (h * 3600)) / 60;
    ਜੇ(h <= 9) {
      curl_off_t s = (seconds - (h * 3600)) - (m * 60);
      ਜੇ(h)
        curl_msnprintf(r, rsize, "%" FMT_OFF_T ":%02" FMT_OFF_T ":"
                       "%02" FMT_OFF_T, h, m, s);
      ਨਹੀਂਤਾਂ
        curl_msnprintf(r, rsize, "  %02" FMT_OFF_T ":%02" FMT_OFF_T, m, s);
    }
    ਨਹੀਂਤਾਂ
      curl_msnprintf(r, rsize, "%" FMT_OFF_T "h %02" FMT_OFF_T "m", h, m);
  }
  ਨਹੀਂਤਾਂ {
    curl_off_t d = seconds / 86400;
    h = (seconds - (d * 86400)) / 3600;
    ਜੇ(d <= 99)
      curl_msnprintf(r, rsize, "%2" FMT_OFF_T "d %02" FMT_OFF_T "h", d, h);
    ਨਹੀਂਤਾਂ ਜੇ(d <= 999)
      curl_msnprintf(r, rsize, "%6" FMT_OFF_T "d", d);
    ਨਹੀਂਤਾਂ { /* more than 999 days */
      curl_off_t m = d / 30;
      ਜੇ(m <= 999)
        curl_msnprintf(r, rsize, "%6" FMT_OFF_T "m", m);
      ਨਹੀਂਤਾਂ { /* more than 999 months */
        curl_off_t y = d / 365;
        ਜੇ(y <= 99999)
          curl_msnprintf(r, rsize, "%6" FMT_OFF_T "y", y);
        ਨਹੀਂਤਾਂ
          curlx_strcopy(r, rsize, STRCONST(">99999y"));
      }
    }
  }
}

/* The point of this function would be to return a string of the input data,
   but never longer than 6 columns (+ one zero byte).
   Add suffix k, M, G when suitable...

   @unittest 1636 */
UNITTEST ਅੱਖਰ *max6out(curl_off_t ਬਾਈਟਾਂ, ਅੱਖਰ *max6, size_t mlen);
UNITTEST ਅੱਖਰ *max6out(curl_off_t ਬਾਈਟਾਂ, ਅੱਖਰ *max6, size_t mlen)
{
  /* a signed 64-bit value is 8192 petabytes maximum, shown as
     8.0E (exabytes)*/
  ਜੇ(ਬਾਈਟਾਂ < 100000)
    curl_msnprintf(max6, mlen, "%6" CURL_FORMAT_CURL_OFF_T, ਬਾਈਟਾਂ);
  ਨਹੀਂਤਾਂ {
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ unit[] = { 'k', 'M', 'G', 'T', 'P', 'E', 0 };
    ਪੂਰਨ k = 0;
    curl_off_t nbytes;
    curl_off_t rest;
    ਕਰੋ {
      nbytes = ਬਾਈਟਾਂ / 1024;
      ਜੇ(nbytes < 1000)
        ਰੋਕੋ;
      ਬਾਈਟਾਂ = nbytes;
      k++;
      DEBUGASSERT(unit[k]);
    } ਜਦੋਂ(unit[k]);
    rest = ਬਾਈਟਾਂ % 1024;
    ਜੇ(nbytes <= 99)
      /* xx.yyU */
      curl_msnprintf(max6, mlen, "%2" CURL_FORMAT_CURL_OFF_T
                     ".%02" CURL_FORMAT_CURL_OFF_T "%c", nbytes,
                     rest * 100 / 1024, unit[k]);
    ਨਹੀਂਤਾਂ
      /* xxx.yU */
      curl_msnprintf(max6, mlen, "%3" CURL_FORMAT_CURL_OFF_T
                     ".%" CURL_FORMAT_CURL_OFF_T "%c", nbytes,
                     rest * 10 / 1024, unit[k]);
  }
  ਵਾਪਸ max6;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ pgrs_speedinit(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  memset(&ਅੰਕੜੇ->state.keeps_ਰਫਤਾਰ, 0, ਆਕਾਰ(ਬਣਤਰ curltime));
}

/*
 * @unittest 1606
 */
UNITTEST CURLcode pgrs_speedcheck(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਅਚਲ ਬਣਤਰ curltime *pnow);
UNITTEST CURLcode pgrs_speedcheck(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  ਜੇ(!ਅੰਕੜੇ->ਸੈੱਟ.ਨੀਵਾਂ_ਰਫਤਾਰ_ਸਮਾਂ || !ਅੰਕੜੇ->ਸੈੱਟ.ਨੀਵਾਂ_ਰਫਤਾਰ_ਹੱਦਬੰਦੀ ||
     Curl_xfer_recv_is_paused(ਅੰਕੜੇ) || Curl_xfer_send_is_paused(ਅੰਕੜੇ))
    /* A paused transfer is not qualified for speed checks */
    ਵਾਪਸ CURLE_OK;

  ਜੇ(ਅੰਕੜੇ->ਤਰੱਕੀ.current_ਰਫਤਾਰ >= 0) {
    ਜੇ(ਅੰਕੜੇ->ਤਰੱਕੀ.current_ਰਫਤਾਰ < ਅੰਕੜੇ->ਸੈੱਟ.ਨੀਵਾਂ_ਰਫਤਾਰ_ਹੱਦਬੰਦੀ) {
      ਜੇ(!ਅੰਕੜੇ->state.keeps_ਰਫਤਾਰ.tv_sec)
        /* under the limit at this moment */
        ਅੰਕੜੇ->state.keeps_ਰਫਤਾਰ = *pnow;
      ਨਹੀਂਤਾਂ {
        /* how long has it been under the limit */
        timediff_t howlong =
          curlx_ptimediff_ms(pnow, &ਅੰਕੜੇ->state.keeps_ਰਫਤਾਰ);

        ਜੇ(howlong >= ਅੰਕੜੇ->ਸੈੱਟ.ਨੀਵਾਂ_ਰਫਤਾਰ_ਸਮਾਂ * 1000) {
          /* too long */
          failf(ਅੰਕੜੇ, "Operation too slow. Less than %" FMT_OFF_T
                " bytes/sec transferred the last %u seconds",
                ਅੰਕੜੇ->ਸੈੱਟ.ਨੀਵਾਂ_ਰਫਤਾਰ_ਹੱਦਬੰਦੀ, ਅੰਕੜੇ->ਸੈੱਟ.ਨੀਵਾਂ_ਰਫਤਾਰ_ਸਮਾਂ);
          ਵਾਪਸ CURLE_OPERATION_TIMEDOUT;
        }
      }
    }
    ਨਹੀਂਤਾਂ
      /* faster right now */
      ਅੰਕੜੇ->state.keeps_ਰਫਤਾਰ.tv_sec = 0;
  }

  /* since low speed limit is enabled, set the expire timer to make this
     connection's speed get checked again in a second */
  Curl_expire_ਸੈੱਟ(ਅੰਕੜੇ, EXPIRE_SPEEDCHECK, 1000, pnow);

  ਵਾਪਸ CURLE_OK;
}

ਅਚਲ ਬਣਤਰ curltime *Curl_pgrs_now(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  curlx_pnow(&ਅੰਕੜੇ->ਤਰੱਕੀ.now);
  ਵਾਪਸ &ਅੰਕੜੇ->ਤਰੱਕੀ.now;
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

ਪੂਰਨ Curl_pgrs_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  ਪੂਰਨ rc;
  ਅੰਕੜੇ->ਤਰੱਕੀ.delta.lastshow_us = -1;
  rc = Curl_pgrsUpdateX(ਅੰਕੜੇ, pnow); /* the final (forced) update */
  ਜੇ(rc)
    ਵਾਪਸ rc;

  ਜੇ(!ਅੰਕੜੇ->ਤਰੱਕੀ.hide && !ਅੰਕੜੇ->ਤਰੱਕੀ.callback)
    /* only output if we do not use a progress callback and we are not
     * hidden */
    curl_mfprintf(ਅੰਕੜੇ->ਸੈੱਟ.err, "\n");

  ਵਾਪਸ 0;
}

ਖਾਲੀ Curl_pgrsReset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  Curl_pgrs_ਸੈੱਟ_Upload_Counter(ਅੰਕੜੇ, 0);
  ਅੰਕੜੇ->ਤਰੱਕੀ.dl.cur_ਮਾਪ = 0;
  Curl_pgrs_ਸੈੱਟ_Upload_ਮਾਪ(ਅੰਕੜੇ, -1);
  Curl_pgrs_ਸੈੱਟ_Download_ਮਾਪ(ਅੰਕੜੇ, -1);
  ਅੰਕੜੇ->ਤਰੱਕੀ.speeder_c = 0; /* reset speed records */
  ਅੰਕੜੇ->ਤਰੱਕੀ.deliver = 0;
  pgrs_speedinit(ਅੰਕੜੇ);
}

/* reset the known transfer sizes */
ਖਾਲੀ Curl_pgrsResetTransferSizes(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  Curl_pgrs_ਸੈੱਟ_Download_ਮਾਪ(ਅੰਕੜੇ, -1);
  Curl_pgrs_ਸੈੱਟ_Upload_ਮਾਪ(ਅੰਕੜੇ, -1);
}

ਖਾਲੀ Curl_pgrsRecvPause(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool enable)
{
  ਜੇ(!enable) {
    ਅੰਕੜੇ->ਤਰੱਕੀ.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(ਅੰਕੜੇ); /* reset low speed measurements */
  }
}

ਖਾਲੀ Curl_pgrsSendPause(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool enable)
{
  ਜੇ(!enable) {
    ਅੰਕੜੇ->ਤਰੱਕੀ.speeder_c = 0; /* reset speed records */
    pgrs_speedinit(ਅੰਕੜੇ); /* reset low speed measurements */
  }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ pgrs_timer_ਨਾਂ[] = {
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

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *pgrs_timer_name(timerid timer)
{
  ਜੇ((size_t)timer < CURL_ARRAYSIZE(pgrs_timer_ਨਾਂ))
    ਵਾਪਸ pgrs_timer_ਨਾਂ[(size_t)timer];
  ਵਾਪਸ "?";
}
#ਜੇ_ਅੰਤ /* CURLVERBOSE */

/*
 * Curl_pgrsTimeWas(). Store the timestamp time at the given label.
 */
ਖਾਲੀ Curl_pgrs_ਸਮਾਂ_Was(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, timerid timer,
                      ਬਣਤਰ curltime timestamp)
{
  timediff_t *delta = ਨਿਰਾ;

  ਚੋਣ(timer) {
  ਮੂਲ:
  ਹਾਲਤ TIMER_NONE:
    /* mistake filter */
    ਰੋਕੋ;
  ਹਾਲਤ TIMER_STARTOP:
    /* This is set at the start of a transfer */
    ਅੰਕੜੇ->ਤਰੱਕੀ.delta.startop_us =
      curlx_ptimediff_us(&timestamp, &ਅੰਕੜੇ->ਤਰੱਕੀ.ਸ਼ੁਰੂ);
    ਅੰਕੜੇ->ਤਰੱਕੀ.delta.startqueue_us = ਅੰਕੜੇ->ਤਰੱਕੀ.delta.startop_us;
    ਰੋਕੋ;
  ਹਾਲਤ TIMER_STARTSINGLE:
    /* This is set at the start of each single transfer */
    ਅੰਕੜੇ->ਤਰੱਕੀ.delta.startsingle_us =
      curlx_ptimediff_us(&timestamp, &ਅੰਕੜੇ->ਤਰੱਕੀ.ਸ਼ੁਰੂ);
    ਅੰਕੜੇ->ਤਰੱਕੀ.startransfer_added = FALSE;
    ਰੋਕੋ;
  ਹਾਲਤ TIMER_POSTQUEUE:
    /* Queue time is accumulative from all involved redirects */
    ਅੰਕੜੇ->ਤਰੱਕੀ.ਕੁੱਲ.queued_us +=
      curlx_ptimediff_us(&timestamp, &ਅੰਕੜੇ->ਤਰੱਕੀ.ਸ਼ੁਰੂ) -
      ਅੰਕੜੇ->ਤਰੱਕੀ.delta.startqueue_us;
    ਰੋਕੋ;
  ਹਾਲਤ TIMER_NAMELOOKUP:
    delta = &ਅੰਕੜੇ->ਤਰੱਕੀ.ਕੁੱਲ.nslookup_us;
    ਰੋਕੋ;
  ਹਾਲਤ TIMER_CONNECT:
    delta = &ਅੰਕੜੇ->ਤਰੱਕੀ.ਕੁੱਲ.connect_us;
    ਰੋਕੋ;
  ਹਾਲਤ TIMER_APPCONNECT:
    delta = &ਅੰਕੜੇ->ਤਰੱਕੀ.ਕੁੱਲ.appconnect_us;
    ਰੋਕੋ;
  ਹਾਲਤ TIMER_PRETRANSFER:
    delta = &ਅੰਕੜੇ->ਤਰੱਕੀ.ਕੁੱਲ.pretransfer_us;
    ਰੋਕੋ;
  ਹਾਲਤ TIMER_STARTTRANSFER:
    /* prevent updating t_starttransfer unless:
     *   1. this is the first time we are setting t_starttransfer
     *   2. a redirect has occurred since the last time t_starttransfer was set
     * This prevents repeated invocations of the function from incorrectly
     * changing the t_starttransfer time.
     */
    ਜੇ(ਅੰਕੜੇ->ਤਰੱਕੀ.startransfer_added) {
      CURL_TRC_M(ਅੰਕੜੇ, "[%s] ignored", pgrs_timer_name(timer));
      ਵਾਪਸ;
    }
    ਅੰਕੜੇ->ਤਰੱਕੀ.startransfer_added = TRUE;
    delta = &ਅੰਕੜੇ->ਤਰੱਕੀ.ਕੁੱਲ.starttransfer_us;
    ਰੋਕੋ;
  ਹਾਲਤ TIMER_POSTRANSFER:
    delta = &ਅੰਕੜੇ->ਤਰੱਕੀ.ਕੁੱਲ.posttransfer_us;
    ਰੋਕੋ;
  ਹਾਲਤ TIMER_REDIRECT:
    ਅੰਕੜੇ->ਤਰੱਕੀ.delta.startredirect_us =
      curlx_ptimediff_us(&timestamp, &ਅੰਕੜੇ->ਤਰੱਕੀ.ਸ਼ੁਰੂ);
    /* transfer starts queueing again */
    ਅੰਕੜੇ->ਤਰੱਕੀ.delta.startqueue_us =
      ਅੰਕੜੇ->ਤਰੱਕੀ.delta.startredirect_us;
    ਰੋਕੋ;
  }
  ਜੇ(delta) {
    timediff_t us = curlx_ptimediff_us(&timestamp, &ਅੰਕੜੇ->ਤਰੱਕੀ.ਸ਼ੁਰੂ) -
                    ਅੰਕੜੇ->ਤਰੱਕੀ.delta.startsingle_us;
    ਜੇ(us < 1)
      us = 1; /* make sure at least one microsecond passed */
    *delta += us;
    CURL_TRC_M(ਅੰਕੜੇ, "[%s] added %" FMT_TIMEDIFF_T "us",
               pgrs_timer_name(timer), us);
  }
  ਨਹੀਂਤਾਂ
    CURL_TRC_M(ਅੰਕੜੇ, "[%s] set", pgrs_timer_name(timer));
}

/*
 * Curl_pgrsTime(). Store the current time at the given label. This fetches a
 * fresh "now" and returns it.
 *
 * @unittest: 1399
 */
ਖਾਲੀ Curl_pgrs_ਸਮਾਂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, timerid timer)
{
  Curl_pgrs_ਸਮਾਂ_Was(ਅੰਕੜੇ, timer, *Curl_pgrs_now(ਅੰਕੜੇ));
}

ਖਾਲੀ Curl_pgrs_ਸ਼ੁਰੂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  ਬਣਤਰ ਤਰੱਕੀ_2 *p = &ਅੰਕੜੇ->ਤਰੱਕੀ;

  ਜੇ(!pnow)
    pnow = Curl_pgrs_now(ਅੰਕੜੇ);
  p->speeder_c = 0; /* reset the progress meter display */
  p->ਸ਼ੁਰੂ = *pnow;
  memset(&p->delta, 0, ਆਕਾਰ(p->delta));
  memset(&p->ਕੁੱਲ, 0, ਆਕਾਰ(p->ਕੁੱਲ));
  p->startransfer_added = FALSE;
  p->dl.cur_ਮਾਪ = 0;
  p->ul.cur_ਮਾਪ = 0;
  /* the sizes are unknown at start */
  p->dl_ਮਾਪ_ਗਿਆਤ = FALSE;
  p->ul_ਮਾਪ_ਗਿਆਤ = FALSE;
}

/* check that the 'delta' amount of bytes are okay to deliver to the
   application, or return error if not. */
CURLcode Curl_pgrs_deliver_check(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, size_t delta)
{
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.max_filesize &&
     ((curl_off_t)delta > ਅੰਕੜੇ->ਸੈੱਟ.max_filesize - ਅੰਕੜੇ->ਤਰੱਕੀ.deliver)) {
    failf(ਅੰਕੜੇ, "Would have exceeded max file size");
    ਵਾਪਸ CURLE_FILESIZE_EXCEEDED;
  }
  ਵਾਪਸ CURLE_OK;
}

/* this counts how much data is delivered to the application, which
   in compressed cases may differ from downloaded amount */
ਖਾਲੀ Curl_pgrs_deliver_inc(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, size_t delta)
{
  ਅੰਕੜੇ->ਤਰੱਕੀ.deliver += delta;
}

ਖਾਲੀ Curl_pgrs_download_inc(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, size_t delta)
{
  ਜੇ(delta) {
    ਅੰਕੜੇ->ਤਰੱਕੀ.dl.cur_ਮਾਪ += delta;
    Curl_rlimit_drain(&ਅੰਕੜੇ->ਤਰੱਕੀ.dl.rlimit, delta, ਨਿਰਾ);
  }
}

ਖਾਲੀ Curl_pgrs_upload_inc(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, size_t delta)
{
  ਜੇ(delta) {
    ਅੰਕੜੇ->ਤਰੱਕੀ.ul.cur_ਮਾਪ += delta;
    Curl_rlimit_drain(&ਅੰਕੜੇ->ਤਰੱਕੀ.ul.rlimit, delta, ਨਿਰਾ);
  }
}

/*
 * Set the number of uploaded bytes so far.
 */
ਖਾਲੀ Curl_pgrs_ਸੈੱਟ_Upload_Counter(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, curl_off_t ਮਾਪ)
{
  ਅੰਕੜੇ->ਤਰੱਕੀ.ul.cur_ਮਾਪ = ਮਾਪ;
}

ਖਾਲੀ Curl_pgrs_ਸੈੱਟ_Download_ਮਾਪ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, curl_off_t ਮਾਪ)
{
  ਜੇ(ਮਾਪ >= 0) {
    ਅੰਕੜੇ->ਤਰੱਕੀ.dl.ਕੁੱਲ_ਮਾਪ = ਮਾਪ;
    ਅੰਕੜੇ->ਤਰੱਕੀ.dl_ਮਾਪ_ਗਿਆਤ = TRUE;
  }
  ਨਹੀਂਤਾਂ {
    ਅੰਕੜੇ->ਤਰੱਕੀ.dl.ਕੁੱਲ_ਮਾਪ = 0;
    ਅੰਕੜੇ->ਤਰੱਕੀ.dl_ਮਾਪ_ਗਿਆਤ = FALSE;
  }
}

ਖਾਲੀ Curl_pgrs_ਸੈੱਟ_Upload_ਮਾਪ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, curl_off_t ਮਾਪ)
{
  ਜੇ(ਮਾਪ >= 0) {
    ਅੰਕੜੇ->ਤਰੱਕੀ.ul.ਕੁੱਲ_ਮਾਪ = ਮਾਪ;
    ਅੰਕੜੇ->ਤਰੱਕੀ.ul_ਮਾਪ_ਗਿਆਤ = TRUE;
  }
  ਨਹੀਂਤਾਂ {
    ਅੰਕੜੇ->ਤਰੱਕੀ.ul.ਕੁੱਲ_ਮਾਪ = 0;
    ਅੰਕੜੇ->ਤਰੱਕੀ.ul_ਮਾਪ_ਗਿਆਤ = FALSE;
  }
}

ਖਾਲੀ Curl_pgrs_Early_ਅੰਕੜੇ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, curl_off_t ਭੇਜਿਆ)
{
  ਅੰਕੜੇ->ਤਰੱਕੀ.earlydata_ਭੇਜਿਆ = ਭੇਜਿਆ;
}

/* returns the average speed in bytes / second */
ਸਥਿਰ curl_off_t trspeed(curl_off_t ਮਾਪ, /* number of bytes */
                          curl_off_t us)   /* microseconds */
{
  ਜੇ(us < 1)
    ਵਾਪਸ ਮਾਪ * 1000000;
  ਨਹੀਂਤਾਂ ਜੇ(ਮਾਪ < CURL_OFF_T_MAX / 1000000)
    ਵਾਪਸ (ਮਾਪ * 1000000) / us;
  ਨਹੀਂਤਾਂ ਜੇ(us >= 1000000)
    ਵਾਪਸ ਮਾਪ / (us / 1000000);
  ਨਹੀਂਤਾਂ
    ਵਾਪਸ CURL_OFF_T_MAX;
}

/* returns TRUE if it is time to show the progress meter */
ਸਥਿਰ bool ਤਰੱਕੀ_calc(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  ਬਣਤਰ ਤਰੱਕੀ_2 * ਅਚਲ p = &ਅੰਕੜੇ->ਤਰੱਕੀ;
  ਪੂਰਨ i_ਅਗਲਾ, i_oldest, i_latest;
  timediff_t duration_us, elapsed_us;
  curl_off_t amount;

  /* The time spent so far (from the start) in microseconds */
  elapsed_us = curlx_ptimediff_us(pnow, &p->ਸ਼ੁਰੂ);
  p->ਕੁੱਲ.spent_us = elapsed_us;
  p->dl.ਰਫਤਾਰ = trspeed(p->dl.cur_ਮਾਪ, p->ਕੁੱਲ.spent_us);
  p->ul.ਰਫਤਾਰ = trspeed(p->ul.cur_ਮਾਪ, p->ਕੁੱਲ.spent_us);

  ਜੇ(!p->speeder_c) { /* no previous record exists */
    p->ਰਫਤਾਰ_amount[0] = p->dl.cur_ਮਾਪ + p->ul.cur_ਮਾਪ;
    p->ਰਫਤਾਰ_ਸਮਾਂ[0] = elapsed_us;
    p->speeder_c++;
    /* use the overall average at the start */
    p->current_ਰਫਤਾਰ = p->ul.ਰਫਤਾਰ + p->dl.ਰਫਤਾਰ;
    p->delta.lastshow_us = elapsed_us;
    ਵਾਪਸ TRUE;
  }
  /* We have at least one record now. Where to put the next and
   * where is the latest one? */
  i_ਅਗਲਾ = p->speeder_c % CURL_ਰਫਤਾਰ_ਰਿਕਾਰਡਾਂ;
  i_latest = (i_ਅਗਲਾ > 0) ? (i_ਅਗਲਾ - 1) : (CURL_ਰਫਤਾਰ_ਰਿਕਾਰਡਾਂ - 1);

  /* Make a new record only when some time has passed.
   * Too frequent calls otherwise ruin the history. */
  ਜੇ((elapsed_us - p->ਰਫਤਾਰ_ਸਮਾਂ[i_latest]) >= (1000 * 1000)) {
    p->speeder_c++;
    i_latest = i_ਅਗਲਾ;
    p->ਰਫਤਾਰ_amount[i_latest] = p->dl.cur_ਮਾਪ + p->ul.cur_ਮਾਪ;
    p->ਰਫਤਾਰ_ਸਮਾਂ[i_latest] = elapsed_us;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->req.ਪੂਰਾ) {
    /* When a transfer is done, and we did not have a current speed
     * already, update the last record. Otherwise, stay at the speed
     * we have. The last chunk of data, when rate limiting, would increase
     * reported speed since it no longer measures a full second. */
    ਜੇ(!p->current_ਰਫਤਾਰ) {
      p->ਰਫਤਾਰ_amount[i_latest] = p->dl.cur_ਮਾਪ + p->ul.cur_ਮਾਪ;
      p->ਰਫਤਾਰ_ਸਮਾਂ[i_latest] = elapsed_us;
    }
  }
  ਨਹੀਂਤਾਂ {
    /* transfer ongoing, wait for more time to pass. */
    ਵਾਪਸ FALSE;
  }

  i_oldest = (p->speeder_c < CURL_ਰਫਤਾਰ_ਰਿਕਾਰਡਾਂ) ? 0 :
             ((i_latest + 1) % CURL_ਰਫਤਾਰ_ਰਿਕਾਰਡਾਂ);

  /* How much we transferred between oldest and current records */
  amount = p->ਰਫਤਾਰ_amount[i_latest] - p->ਰਫਤਾਰ_amount[i_oldest];
  /* How long this took */
  duration_us = p->ਰਫਤਾਰ_ਸਮਾਂ[i_latest] - p->ਰਫਤਾਰ_ਸਮਾਂ[i_oldest];
  ਜੇ(duration_us <= 0)
    duration_us = 1;

  ਜੇ(amount > (CURL_OFF_T_MAX / 1000000)) {
    /* the 'amount' value is bigger than would fit in 64 bits if
       multiplied with 1000000, so we use the double math for this */
    p->current_ਰਫਤਾਰ =
      (curl_off_t)(((ਦੁੱਗਣਾ)amount * 1000000.0) / (ਦੁੱਗਣਾ)duration_us);
  }
  ਨਹੀਂਤਾਂ {
    p->current_ਰਫਤਾਰ = amount * 1000000 / duration_us;
  }

  ਜੇ((p->delta.lastshow_us >= 0) && !ਅੰਕੜੇ->req.ਪੂਰਾ &&
     ((elapsed_us - p->delta.lastshow_us) < (1000 * 1000)))
    ਵਾਪਸ FALSE;
  p->delta.lastshow_us = elapsed_us;
  ਵਾਪਸ TRUE;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_ਤਰੱਕੀ_METER

ਬਣਤਰ pgrs_estimate {
  curl_off_t secs;
  curl_off_t percent;
};

ਸਥਿਰ curl_off_t pgrs_est_percent(curl_off_t ਕੁੱਲ, curl_off_t cur)
{
  ਜੇ(ਕੁੱਲ > 10000)
    ਵਾਪਸ cur / (ਕੁੱਲ / 100);
  ਨਹੀਂਤਾਂ ਜੇ(ਕੁੱਲ > 0)
    ਵਾਪਸ (cur * 100) / ਕੁੱਲ;
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ pgrs_estimates(ਬਣਤਰ pgrs_dir *d,
                           bool ਕੁੱਲ_ਗਿਆਤ,
                           ਬਣਤਰ pgrs_estimate *est)
{
  est->secs = 0;
  est->percent = 0;
  ਜੇ(ਕੁੱਲ_ਗਿਆਤ && (d->ਰਫਤਾਰ > 0)) {
    est->secs = d->ਕੁੱਲ_ਮਾਪ / d->ਰਫਤਾਰ;
    est->percent = pgrs_est_percent(d->ਕੁੱਲ_ਮਾਪ, d->cur_ਮਾਪ);
  }
}

ਸਥਿਰ ਖਾਲੀ ਤਰੱਕੀ_meter(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ਤਰੱਕੀ_2 *p = &ਅੰਕੜੇ->ਤਰੱਕੀ;
  ਅੱਖਰ max6[6][7];
  ਬਣਤਰ pgrs_estimate dl_estm;
  ਬਣਤਰ pgrs_estimate ul_estm;
  ਬਣਤਰ pgrs_estimate ਕੁੱਲ_estm;
  curl_off_t ਕੁੱਲ_cur_ਮਾਪ;
  curl_off_t ਕੁੱਲ_expected_ਮਾਪ;
  curl_off_t dl_ਮਾਪ;
  ਅੱਖਰ ਸਮਾਂ_ਖੱਬਾ[8];
  ਅੱਖਰ ਸਮਾਂ_ਕੁੱਲ[8];
  ਅੱਖਰ ਸਮਾਂ_spent[8];
  curl_off_t cur_secs = (curl_off_t)p->ਕੁੱਲ.spent_us / 1000000;

  ਜੇ(!p->ਸਿਰਲੇਖਾਂ_ਬਾਹਰਲਾ) {
    ਜੇ(ਅੰਕੜੇ->state.resume_from) {
      curl_mfprintf(ਅੰਕੜੇ->ਸੈੱਟ.err,
                    "** Resuming transfer from byte position %" FMT_OFF_T "\n",
                    ਅੰਕੜੇ->state.resume_from);
    }
    curl_mfprintf(ਅੰਕੜੇ->ਸੈੱਟ.err,
                  "  %% Total    %% Received %% Xferd  Average Speed  "
                  "Time    Time    Time   Current\n"
                  "                                 Dload  Upload  "
                  "Total   Spent   Left   Speed\n");
    p->ਸਿਰਲੇਖਾਂ_ਬਾਹਰਲਾ = TRUE; /* headers are shown */
  }

  /* Figure out the estimated time of arrival for upload and download */
  pgrs_estimates(&p->ul, (bool)p->ul_ਮਾਪ_ਗਿਆਤ, &ul_estm);
  pgrs_estimates(&p->dl, (bool)p->dl_ਮਾਪ_ਗਿਆਤ, &dl_estm);

  /* Since both happen at the same time, total expected duration is max. */
  ਕੁੱਲ_estm.secs = CURLMAX(ul_estm.secs, dl_estm.secs);
  /* create the three time strings */
  time2str(ਸਮਾਂ_ਖੱਬਾ, ਆਕਾਰ(ਸਮਾਂ_ਖੱਬਾ),
           ਕੁੱਲ_estm.secs > 0 ? (ਕੁੱਲ_estm.secs - cur_secs) : 0);
  time2str(ਸਮਾਂ_ਕੁੱਲ, ਆਕਾਰ(ਸਮਾਂ_ਕੁੱਲ), ਕੁੱਲ_estm.secs);
  time2str(ਸਮਾਂ_spent, ਆਕਾਰ(ਸਮਾਂ_spent), cur_secs);

  /* Get the total amount of data expected to get transferred */
  ਕੁੱਲ_expected_ਮਾਪ = p->ul_ਮਾਪ_ਗਿਆਤ ? p->ul.ਕੁੱਲ_ਮਾਪ : p->ul.cur_ਮਾਪ;

  dl_ਮਾਪ = p->dl_ਮਾਪ_ਗਿਆਤ ? p->dl.ਕੁੱਲ_ਮਾਪ : p->dl.cur_ਮਾਪ;

  /* integer overflow check */
  ਜੇ((CURL_OFF_T_MAX - ਕੁੱਲ_expected_ਮਾਪ) < dl_ਮਾਪ)
    ਕੁੱਲ_expected_ਮਾਪ = CURL_OFF_T_MAX; /* capped */
  ਨਹੀਂਤਾਂ
    ਕੁੱਲ_expected_ਮਾਪ += dl_ਮਾਪ;

  /* We have transferred this much so far */
  ਕੁੱਲ_cur_ਮਾਪ = p->dl.cur_ਮਾਪ + p->ul.cur_ਮਾਪ;

  /* Get the percentage of data transferred so far */
  ਕੁੱਲ_estm.percent = pgrs_est_percent(ਕੁੱਲ_expected_ਮਾਪ, ਕੁੱਲ_cur_ਮਾਪ);

  curl_mfprintf(ਅੰਕੜੇ->ਸੈੱਟ.err,
                "\r"
                "%3" FMT_OFF_T " %s "
                "%3" FMT_OFF_T " %s "
                "%3" FMT_OFF_T " %s %s %s %s %s %s %s",
                ਕੁੱਲ_estm.percent, /* 3 letters */    /* total % */
                max6out(ਕੁੱਲ_expected_ਮਾਪ, max6[2],
                        ਆਕਾਰ(max6[2])),              /* total size */
                dl_estm.percent, /* 3 letters */       /* rcvd % */
                max6out(p->dl.cur_ਮਾਪ, max6[0],
                        ਆਕਾਰ(max6[0])),              /* rcvd size */
                ul_estm.percent, /* 3 letters */       /* xfer % */
                max6out(p->ul.cur_ਮਾਪ, max6[1],
                        ਆਕਾਰ(max6[1])),              /* xfer size */
                max6out(p->dl.ਰਫਤਾਰ, max6[3],
                        ਆਕਾਰ(max6[3])),              /* avrg dl speed */
                max6out(p->ul.ਰਫਤਾਰ, max6[4],
                        ਆਕਾਰ(max6[4])),              /* avrg ul speed */
                ਸਮਾਂ_ਕੁੱਲ,    /* 7 letters */         /* total time */
                ਸਮਾਂ_spent,    /* 7 letters */         /* time spent */
                ਸਮਾਂ_ਖੱਬਾ,     /* 7 letters */         /* time left */
                max6out(p->current_ਰਫਤਾਰ, max6[5],
                        ਆਕਾਰ(max6[5]))               /* current speed */
    );

  /* we flush the output stream to make it appear as soon as possible */
  fflush(ਅੰਕੜੇ->ਸੈੱਟ.err);
}
#ਨਹੀਂਤਾਂ /* CURL_DISABLE_PROGRESS_METER */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਤਰੱਕੀ_meter(x) Curl_nop_stmt
#ਜੇ_ਅੰਤ

/*
 * Curl_pgrsUpdate() returns 0 for success or the value returned by the
 * progress callback!
 */
ਸਥਿਰ CURLcode pgrsupdate(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool showprogress)
{
  ਜੇ(!ਅੰਕੜੇ->ਤਰੱਕੀ.hide) {
    ਪੂਰਨ rc;
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.fxferinfo) {
      /* There is a callback set, call that */
      ਬਣਤਰ Curl_mapi_guard guard;
      CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_fxferinfo);
      rc = ਅੰਕੜੇ->ਸੈੱਟ.fxferinfo(ਅੰਕੜੇ->ਸੈੱਟ.ਤਰੱਕੀ_client,
                               ਅੰਕੜੇ->ਤਰੱਕੀ.dl.ਕੁੱਲ_ਮਾਪ,
                               ਅੰਕੜੇ->ਤਰੱਕੀ.dl.cur_ਮਾਪ,
                               ਅੰਕੜੇ->ਤਰੱਕੀ.ul.ਕੁੱਲ_ਮਾਪ,
                               ਅੰਕੜੇ->ਤਰੱਕੀ.ul.cur_ਮਾਪ);
      CURL_CBAPI_ਸਿਰਾ(&guard);
      ਜੇ(rc != CURL_PROGRESSFUNC_CONTINUE) {
        ਜੇ(rc) {
          failf(ਅੰਕੜੇ, "Callback aborted");
          ਵਾਪਸ CURLE_ABORTED_BY_CALLBACK;
        }
        ਵਾਪਸ CURLE_OK;
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.fprogress) {
      /* The older deprecated callback is set, call that */
      ਬਣਤਰ Curl_mapi_guard guard;
      CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_fprogress);
      rc = ਅੰਕੜੇ->ਸੈੱਟ.fprogress(ਅੰਕੜੇ->ਸੈੱਟ.ਤਰੱਕੀ_client,
                               (ਦੁੱਗਣਾ)ਅੰਕੜੇ->ਤਰੱਕੀ.dl.ਕੁੱਲ_ਮਾਪ,
                               (ਦੁੱਗਣਾ)ਅੰਕੜੇ->ਤਰੱਕੀ.dl.cur_ਮਾਪ,
                               (ਦੁੱਗਣਾ)ਅੰਕੜੇ->ਤਰੱਕੀ.ul.ਕੁੱਲ_ਮਾਪ,
                               (ਦੁੱਗਣਾ)ਅੰਕੜੇ->ਤਰੱਕੀ.ul.cur_ਮਾਪ);
      CURL_CBAPI_ਸਿਰਾ(&guard);
      ਜੇ(rc != CURL_PROGRESSFUNC_CONTINUE) {
        ਜੇ(rc) {
          failf(ਅੰਕੜੇ, "Callback aborted");
          ਵਾਪਸ CURLE_ABORTED_BY_CALLBACK;
        }
        ਵਾਪਸ CURLE_OK;
      }
    }

    ਜੇ(showprogress)
      ਤਰੱਕੀ_meter(ਅੰਕੜੇ);
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode pgrs_update(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  bool showprogress = ਤਰੱਕੀ_calc(ਅੰਕੜੇ, pnow);
  ਵਾਪਸ pgrsupdate(ਅੰਕੜੇ, showprogress);
}

CURLcode Curl_pgrsUpdate(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਵਾਪਸ pgrs_update(ਅੰਕੜੇ, Curl_pgrs_now(ਅੰਕੜੇ));
}

CURLcode Curl_pgrsUpdateX(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  ਵਾਪਸ pgrs_update(ਅੰਕੜੇ, pnow);
}

CURLcode Curl_pgrsCheckX(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  CURLcode ਨਤੀਜਾ;

  ਨਤੀਜਾ = pgrs_update(ਅੰਕੜੇ, pnow);
  ਜੇ(!ਨਤੀਜਾ && !ਅੰਕੜੇ->req.ਪੂਰਾ)
    ਨਤੀਜਾ = pgrs_speedcheck(ਅੰਕੜੇ, pnow);
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_pgrsCheck(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਵਾਪਸ Curl_pgrsCheckX(ਅੰਕੜੇ, Curl_pgrs_now(ਅੰਕੜੇ));
}

/*
 * Update all progress, do not do progress meter/callbacks.
 */
ਖਾਲੀ Curl_pgrsUpdate_nometer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  (ਖਾਲੀ)ਤਰੱਕੀ_calc(ਅੰਕੜੇ, Curl_pgrs_now(ਅੰਕੜੇ));
}

ਖਾਲੀ Curl_pgrsCompleted(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ਤਰੱਕੀ_2 * ਅਚਲ p = &ਅੰਕੜੇ->ਤਰੱਕੀ;
  p->ਕੁੱਲ.spent_us = curlx_ptimediff_us(Curl_pgrs_now(ਅੰਕੜੇ), &p->ਸ਼ੁਰੂ);
}

timediff_t Curl_pgrs_since_ms(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਅਚਲ ਬਣਤਰ curltime *pnow,
                              timerid timer)
{
  ਜੇ(!pnow)
    pnow = Curl_pgrs_now(ਅੰਕੜੇ);
  ਚੋਣ(timer) {
  ਹਾਲਤ TIMER_STARTOP:
    ਵਾਪਸ (curlx_ptimediff_us(pnow, &ਅੰਕੜੇ->ਤਰੱਕੀ.ਸ਼ੁਰੂ) -
            ਅੰਕੜੇ->ਤਰੱਕੀ.delta.startop_us) / 1000;
  ਹਾਲਤ TIMER_STARTSINGLE:
    ਵਾਪਸ (curlx_ptimediff_us(pnow, &ਅੰਕੜੇ->ਤਰੱਕੀ.ਸ਼ੁਰੂ) -
            ਅੰਕੜੇ->ਤਰੱਕੀ.delta.startsingle_us) / 1000;
  ਮੂਲ:
    DEBUGASSERT(0);
    ਵਾਪਸ 0;
  }
}
