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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "urldata.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "multiif.h"

#అన్తర్భావ "cf-recvbuf.h"
#అన్తర్భావ "cf-socket.h"
#అన్తర్భావ "cf-setup.h"
#అన్తర్భావ "http2.h"
#అన్తర్భావ "http_proxy.h"
#అన్తర్భావ "cf-h1-proxy.h"
#అన్తర్భావ "cf-h2-proxy.h"
#అన్తర్భావ "cf-haproxy.h"
#అన్తర్భావ "cf-https-connect.h"
#అన్తర్భావ "cf-ip-happy.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "socks.h"
#అన్తర్భావ "curlx/strparse.h"
#అన్తర్భావ "vdns/cf-dns.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "vquic/vquic.h"
#అన్తర్భావ "curlx/strcopy.h"

స్థిర శూన్య trc_write(నిర్మాణం Curl_easy *దత్తాంశం, curl_infotype type,
                      స్థిరరాశి అక్షరం *ptr, size_t కొలత)
{
  ఒకవేళ(దత్తాంశం->సమితి.verbose) {
    ఒకవేళ(దత్తాంశం->సమితి.fdebug) {
      నిర్మాణం Curl_mapi_guard guard;
      CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_fdebug);
      (శూన్య)(*దత్తాంశం->సమితి.fdebug)(దత్తాంశం, type, CURL_UNCONST(ptr), కొలత,
                                దత్తాంశం->సమితి.debugdata);
      CURL_CBAPI_చివర(&guard);
    }
    లేకపోతే {
      స్థిర స్థిరరాశి అక్షరం s_infotype[CURLINFO_చివర][3] = {
        "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
      ఎంపిక(type) {
      స్థితి CURLINFO_పాఠ్యం:
      స్థితి CURLINFO_శీర్షిక_బహిర్గతం:
      స్థితి CURLINFO_శీర్షిక_లోపల:
        fwrite(s_infotype[type], 2, 1, దత్తాంశం->సమితి.err);
        fwrite(ptr, కొలత, 1, దత్తాంశం->సమితి.err);
        ఆపు;
      అప్రమేయ: /* nada */
        ఆపు;
      }
    }
  }
}

/* max length we trace before ending in '...' */
#నిర్వచించు TRC_పంక్తి_MAX 2048

#నిర్వచించు CURL_TRC_FMT_IDSC   "[x-%" CURL_FORMAT_CURL_OFF_T "] "
#నిర్వచించు CURL_TRC_FMT_IDSD   "[%" CURL_FORMAT_CURL_OFF_T "-x] "
#నిర్వచించు CURL_TRC_FMT_IDSDC  "[%" CURL_FORMAT_CURL_OFF_T "-%" \
                            CURL_FORMAT_CURL_OFF_T "] "

స్థిర నిర్మాణం curl_trc_feat Curl_trc_feat_ids = {
  "LIB-IDS",
  CURL_LOG_LVL_NONE,
};
#నిర్వచించు CURL_TRC_IDS(దత్తాంశం) \
  (Curl_trc_is_verbose(దత్తాంశం) && \
  Curl_trc_feat_ids.log_level >= CURL_LOG_LVL_INFO)

స్థిర size_t trc_print_ids(నిర్మాణం Curl_easy *దత్తాంశం, అక్షరం *buf, size_t maxlen)
{
  curl_off_t cid = దత్తాంశం->state.చివరి_conn_id;
  ఒకవేళ(దత్తాంశం->id >= 0) {
    ఒకవేళ(cid >= 0)
      తిరిగి curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSDC, దత్తాంశం->id, cid);
    లేకపోతే
      తిరిగి curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSD, దత్తాంశం->id);
  }
  లేకపోతే ఒకవేళ(cid >= 0)
    తిరిగి curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSC, cid);
  లేకపోతే {
    తిరిగి curl_msnprintf(buf, maxlen, "[x-x] ");
  }
}

స్థిర size_t trc_చివర_buf(అక్షరం *buf, size_t len, size_t maxlen, bool addnl)
{
  /* make sure we end the trace line in `buf` properly. It needs
   * to end with a null-terminator '\0' or '\n\0' */
  ఒకవేళ(len >= (maxlen - (addnl ? 2 : 1))) {
    len = maxlen - 5;
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '\n';
  }
  లేకపోతే ఒకవేళ(addnl)
    buf[len++] = '\n';
  buf[len] = '\0';
  తిరిగి len;
}

శూన్య Curl_debug(నిర్మాణం Curl_easy *దత్తాంశం, curl_infotype type,
                స్థిరరాశి అక్షరం *ptr, size_t కొలత)
{
  ఒకవేళ(దత్తాంశం->సమితి.verbose) {
    స్థిర స్థిరరాశి అక్షరం s_infotype[CURLINFO_చివర][3] = {
      "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
    అక్షరం buf[TRC_పంక్తి_MAX];
    size_t len;
    ఒకవేళ(దత్తాంశం->సమితి.fdebug) {
      నిర్మాణం Curl_mapi_guard guard;
      ఒకవేళ(CURL_TRC_IDS(దత్తాంశం) && (కొలత < TRC_పంక్తి_MAX)) {
        len = trc_print_ids(దత్తాంశం, buf, TRC_పంక్తి_MAX);
        len += curl_msnprintf(buf + len, TRC_పంక్తి_MAX - len, "%.*s",
                              (పూర్ణాంకం)కొలత, ptr);
        len = trc_చివర_buf(buf, len, TRC_పంక్తి_MAX, FALSE);
        CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_fdebug);
        (శూన్య)(*దత్తాంశం->సమితి.fdebug)(దత్తాంశం, type, buf, len, దత్తాంశం->సమితి.debugdata);
        CURL_CBAPI_చివర(&guard);
      }
      లేకపోతే {
        CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_fdebug);
        (శూన్య)(*దత్తాంశం->సమితి.fdebug)(దత్తాంశం, type, CURL_UNCONST(ptr),
                                  కొలత, దత్తాంశం->సమితి.debugdata);
        CURL_CBAPI_చివర(&guard);
      }
    }
    లేకపోతే {
      ఎంపిక(type) {
      స్థితి CURLINFO_పాఠ్యం:
      స్థితి CURLINFO_శీర్షిక_బహిర్గతం:
      స్థితి CURLINFO_శీర్షిక_లోపల:
#నిర్వచితం_కాకపోతే CURL_DISABLE_VERBOSE_అక్షరశ్రేణులు
        ఒకవేళ(CURL_TRC_IDS(దత్తాంశం)) {
          len = trc_print_ids(దత్తాంశం, buf, TRC_పంక్తి_MAX);
          fwrite(buf, len, 1, దత్తాంశం->సమితి.err);
        }
#అయితే_ముగింపు
        fwrite(s_infotype[type], 2, 1, దత్తాంశం->సమితి.err);
        fwrite(ptr, కొలత, 1, దత్తాంశం->సమితి.err);
        ఆపు;
      అప్రమేయ: /* nada */
        ఆపు;
      }
    }
  }
}

/* Curl_failf() is for messages stating why we failed.
 * The message SHALL NOT include any LF or CR.
 */
శూన్య Curl_failf(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ఒకవేళ(దత్తాంశం->సమితి.verbose || దత్తాంశం->సమితి.errorbuffer) {
    va_list ap;
    size_t len;
    అక్షరం error[CURL_ERROR_కొలత + 2];
    va_start(ap, fmt);
    len = curl_mvsnprintf(error, CURL_ERROR_కొలత, fmt, ap);

    ఒకవేళ(దత్తాంశం->సమితి.errorbuffer && !దత్తాంశం->state.errorbuf) {
      curlx_strcopy(దత్తాంశం->సమితి.errorbuffer, CURL_ERROR_కొలత, error, len);
      దత్తాంశం->state.errorbuf = TRUE; /* wrote error string */
    }
    error[len++] = '\n';
    error[len] = '\0';
    trc_write(దత్తాంశం, CURLINFO_పాఠ్యం, error, len);
    va_end(ap);
  }
}

శూన్య Curl_reset_fail(నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(దత్తాంశం->సమితి.errorbuffer)
    దత్తాంశం->సమితి.errorbuffer[0] = 0;
  దత్తాంశం->state.errorbuf = FALSE;
}

#నిర్వచితమైతే CURLVERBOSE
నిర్మాణం curl_trc_feat Curl_trc_feat_multi = {
  "MULTI",
  CURL_LOG_LVL_NONE,
};
నిర్మాణం curl_trc_feat Curl_trc_feat_read = {
  "READ",
  CURL_LOG_LVL_NONE,
};
నిర్మాణం curl_trc_feat Curl_trc_feat_write = {
  "WRITE",
  CURL_LOG_LVL_NONE,
};
నిర్మాణం curl_trc_feat Curl_trc_feat_dns = {
  "DNS",
  CURL_LOG_LVL_NONE,
};
#నిర్వచితం_కాకపోతే CURL_DISABLE_DOH
నిర్మాణం curl_trc_feat Curl_trc_feat_doh = {
  "DoH",
  CURL_LOG_LVL_NONE,
};
#అయితే_ముగింపు
నిర్మాణం curl_trc_feat Curl_trc_feat_timer = {
  "TIMER",
  CURL_LOG_LVL_NONE,
};
#నిర్వచితమైతే USE_దారాలు
నిర్మాణం curl_trc_feat Curl_trc_feat_దారాలు = {
  "THREADS",
  CURL_LOG_LVL_NONE,
};
#అయితే_ముగింపు
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే CURL_DISABLE_VERBOSE_అక్షరశ్రేణులు

స్థిర శూన్య trc_infof(నిర్మాణం Curl_easy *దత్తాంశం,
                      నిర్మాణం curl_trc_feat *feat,
                      స్థిరరాశి అక్షరం *opt_id, పూర్ణాంకం opt_id_idx,
                      స్థిరరాశి అక్షరం * స్థిరరాశి fmt, va_list ap) CURL_PRINTF(5, 0);

స్థిర శూన్య trc_infof(నిర్మాణం Curl_easy *దత్తాంశం,
                      నిర్మాణం curl_trc_feat *feat,
                      స్థిరరాశి అక్షరం *opt_id, పూర్ణాంకం opt_id_idx,
                      స్థిరరాశి అక్షరం * స్థిరరాశి fmt, va_list ap)
{
  size_t len = 0;
  అక్షరం buf[TRC_పంక్తి_MAX];

  ఒకవేళ(CURL_TRC_IDS(దత్తాంశం))
    len += trc_print_ids(దత్తాంశం, buf + len, TRC_పంక్తి_MAX - len);
  ఒకవేళ(feat)
    len += curl_msnprintf(buf + len, TRC_పంక్తి_MAX - len, "[%s] ", feat->name);
  ఒకవేళ(opt_id) {
    ఒకవేళ(opt_id_idx > 0)
      len += curl_msnprintf(buf + len, TRC_పంక్తి_MAX - len, "[%s-%d] ",
                            opt_id, opt_id_idx);
    లేకపోతే
      len += curl_msnprintf(buf + len, TRC_పంక్తి_MAX - len, "[%s] ", opt_id);
  }
  len += curl_mvsnprintf(buf + len, TRC_పంక్తి_MAX - len, fmt, ap);
  len = trc_చివర_buf(buf, len, TRC_పంక్తి_MAX, TRUE);
  trc_write(దత్తాంశం, CURLINFO_పాఠ్యం, buf, len);
}

శూన్య Curl_infof(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ఒకవేళ(Curl_trc_is_verbose(దత్తాంశం)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(దత్తాంశం, దత్తాంశం->state.feat, ఖాళీ, 0, fmt, ap);
    va_end(ap);
  }
}

శూన్య Curl_trc_cf_infof(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి నిర్మాణం Curl_cfilter *cf,
                       స్థిరరాశి అక్షరం *fmt, ...)
{
  DEBUGASSERT(cf);
  ఒకవేళ(Curl_trc_cf_is_verbose(cf, దత్తాంశం)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(దత్తాంశం, దత్తాంశం->state.feat, cf->cft->name, cf->sockindex, fmt, ap);
    va_end(ap);
  }
}

శూన్య Curl_trc_feat_infof(నిర్మాణం Curl_easy *దత్తాంశం,
                         నిర్మాణం curl_trc_feat *feat,
                         స్థిరరాశి అక్షరం *fmt, ...)
{
  DEBUGASSERT(feat);
  ఒకవేళ(Curl_trc_ft_is_verbose(దత్తాంశం, feat)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(దత్తాంశం, feat, ఖాళీ, 0, fmt, ap);
    va_end(ap);
  }
}

స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి Curl_trc_timer_పేర్లు[] = {
  "100_TIMEOUT",
  "ASYNC_NAME",
  "CONNECTTIMEOUT",
  "HAPPY_EYEBALLS",
  "MULTI_PENDING",
  "SPEEDCHECK",
  "TIMEOUT",
  "TOOFAST",
  "QUIC",
  "FTP_ACCEPT",
  "ALPN_EYEBALLS",
  "SHUTDOWN",
  "SERVER_RESPONSE",
};

స్థిర స్థిరరాశి అక్షరం *trc_timer_name(పూర్ణాంకం tid)
{
  ఒకవేళ((tid >= 0) && ((size_t)tid < CURL_ARRAYSIZE(Curl_trc_timer_పేర్లు)))
    తిరిగి Curl_trc_timer_పేర్లు[(size_t)tid];
  తిరిగి "TIMER-???";
}

శూన్య Curl_trc_timer(నిర్మాణం Curl_easy *దత్తాంశం, పూర్ణాంకం tid, స్థిరరాశి అక్షరం *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ఒకవేళ(Curl_trc_ft_is_verbose(దత్తాంశం, &Curl_trc_feat_timer)) {
    స్థిరరాశి అక్షరం *tname = trc_timer_name(tid);
    va_list ap;
    va_start(ap, fmt);
    trc_infof(దత్తాంశం, &Curl_trc_feat_timer, tname, 0, fmt, ap);
    va_end(ap);
  }
}

శూన్య Curl_trc_easy_timers(నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(CURL_TRC_TIMER_is_verbose(దత్తాంశం) && దత్తాంశం->multi) {
    ఒకవేళ(దత్తాంశం->state.timeouts.మొదటి < EXPIRE_చివరి) {
      నిర్మాణం expire_timers *timeouts = &దత్తాంశం->state.timeouts;
      timediff_t పునాది_us =
        Curl_timeouts_స్థానభ్రంశం_us(&దత్తాంశం->multi->timeouts, Curl_pgrs_now(దత్తాంశం));
      uint8_t id = దత్తాంశం->state.timeouts.మొదటి;
      కొరకు(; id < EXPIRE_చివరి; id = timeouts->తదుపరి[id]) {
        CURL_TRC_TIMER(దత్తాంశం, id, "expires in %" FMT_TIMEDIFF_T "us",
                       timeouts->స్థానభ్రంశం_us[id] - పునాది_us);
      }
    }
  }
}

స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి Curl_trc_mstate_పేర్లు[] = {
  "INIT",
  "PENDING",
  "SETUP",
  "CONNECT",
  "CONNECTING",
  "PROTOCONNECT",
  "PROTOCONNECTING",
  "DO",
  "DOING",
  "DOING_MORE",
  "DID",
  "PERFORMING",
  "RATELIMITING",
  "DONE",
  "COMPLETED",
  "MSGSENT",
};

స్థిరరాశి అక్షరం *Curl_trc_mstate_name(పూర్ణాంకం state)
{
  ఒకవేళ((state >= 0) && ((size_t)state < CURL_ARRAYSIZE(Curl_trc_mstate_పేర్లు)))
    తిరిగి Curl_trc_mstate_పేర్లు[(size_t)state];
  తిరిగి "?";
}

శూన్య Curl_trc_multi(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ఒకవేళ(Curl_trc_ft_is_verbose(దత్తాంశం, &Curl_trc_feat_multi)) {
    స్థిరరాశి అక్షరం *sname = (దత్తాంశం->id >= 0) ?
                        Curl_trc_mstate_name(దత్తాంశం->mstate) : ఖాళీ;
    va_list ap;
    va_start(ap, fmt);
    trc_infof(దత్తాంశం, &Curl_trc_feat_multi, sname, 0, fmt, ap);
    va_end(ap);
  }
}

శూన్య Curl_trc_read(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ఒకవేళ(Curl_trc_ft_is_verbose(దత్తాంశం, &Curl_trc_feat_read)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(దత్తాంశం, &Curl_trc_feat_read, ఖాళీ, 0, fmt, ap);
    va_end(ap);
  }
}

శూన్య Curl_trc_write(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ఒకవేళ(Curl_trc_ft_is_verbose(దత్తాంశం, &Curl_trc_feat_write)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(దత్తాంశం, &Curl_trc_feat_write, ఖాళీ, 0, fmt, ap);
    va_end(ap);
  }
}

శూన్య Curl_trc_dns(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ఒకవేళ(Curl_trc_ft_is_verbose(దత్తాంశం, &Curl_trc_feat_dns)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(దత్తాంశం, &Curl_trc_feat_dns, ఖాళీ, 0, fmt, ap);
    va_end(ap);
  }
}

#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
నిర్మాణం curl_trc_feat Curl_trc_feat_ftp = {
  "FTP",
  CURL_LOG_LVL_NONE,
};

శూన్య Curl_trc_ftp(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ఒకవేళ(Curl_trc_ft_is_verbose(దత్తాంశం, &Curl_trc_feat_ftp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(దత్తాంశం, &Curl_trc_feat_ftp, ఖాళీ, 0, fmt, ap);
    va_end(ap);
  }
}
#అయితే_ముగింపు /* !CURL_DISABLE_FTP */

#నిర్వచితం_కాకపోతే CURL_DISABLE_SMTP
నిర్మాణం curl_trc_feat Curl_trc_feat_smtp = {
  "SMTP",
  CURL_LOG_LVL_NONE,
};

శూన్య Curl_trc_smtp(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ఒకవేళ(Curl_trc_ft_is_verbose(దత్తాంశం, &Curl_trc_feat_smtp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(దత్తాంశం, &Curl_trc_feat_smtp, ఖాళీ, 0, fmt, ap);
    va_end(ap);
  }
}
#అయితే_ముగింపు /* !CURL_DISABLE_SMTP */

#నిర్వచితమైతే USE_SSL
నిర్మాణం curl_trc_feat Curl_trc_feat_ssls = {
  "SSLS",
  CURL_LOG_LVL_NONE,
};

శూన్య Curl_trc_ssls(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ఒకవేళ(Curl_trc_ft_is_verbose(దత్తాంశం, &Curl_trc_feat_ssls)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(దత్తాంశం, &Curl_trc_feat_ssls, ఖాళీ, 0, fmt, ap);
    va_end(ap);
  }
}
#అయితే_ముగింపు /* USE_SSL */

#నిర్వచితమైతే USE_SSH
నిర్మాణం curl_trc_feat Curl_trc_feat_ssh = {
  "SSH",
  CURL_LOG_LVL_NONE,
};

శూన్య Curl_trc_ssh(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ఒకవేళ(Curl_trc_ft_is_verbose(దత్తాంశం, &Curl_trc_feat_ssh)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(దత్తాంశం, &Curl_trc_feat_ssh, ఖాళీ, 0, fmt, ap);
    va_end(ap);
  }
}
#అయితే_ముగింపు /* USE_SSH */

#ఒకవేళ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
నిర్మాణం curl_trc_feat Curl_trc_feat_ws = {
  "WS",
  CURL_LOG_LVL_NONE,
};

శూన్య Curl_trc_ws(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ఒకవేళ(Curl_trc_ft_is_verbose(దత్తాంశం, &Curl_trc_feat_ws)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(దత్తాంశం, &Curl_trc_feat_ws, ఖాళీ, 0, fmt, ap);
    va_end(ap);
  }
}
#అయితే_ముగింపు /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#నిర్వచించు TRC_CT_NONE        0
#నిర్వచించు TRC_CT_PROTOCOL    (1 << 0)
#నిర్వచించు TRC_CT_జాలం     (1 << 1)
#నిర్వచించు TRC_CT_PROXY       (1 << 2)
#నిర్వచించు TRC_CT_INTERNALS   (1 << 3)

నిర్మాణం trc_feat_def {
  నిర్మాణం curl_trc_feat *feat;
  గుర్తులేని పూర్ణాంకం category;
};

స్థిర నిర్మాణం trc_feat_def trc_feats[] = {
  { &Curl_trc_feat_ids,       TRC_CT_INTERNALS },
  { &Curl_trc_feat_multi,     TRC_CT_జాలం },
  { &Curl_trc_feat_read,      TRC_CT_NONE },
  { &Curl_trc_feat_write,     TRC_CT_NONE },
  { &Curl_trc_feat_dns,       TRC_CT_జాలం },
#నిర్వచితం_కాకపోతే CURL_DISABLE_DOH
  { &Curl_trc_feat_doh,       TRC_CT_జాలం },
#అయితే_ముగింపు
  { &Curl_trc_feat_timer,     TRC_CT_జాలం },
#నిర్వచితమైతే USE_దారాలు
  { &Curl_trc_feat_దారాలు,   TRC_CT_NONE },
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
  { &Curl_trc_feat_ftp,       TRC_CT_PROTOCOL },
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_SMTP
  { &Curl_trc_feat_smtp,      TRC_CT_PROTOCOL },
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SSL
  { &Curl_trc_feat_ssls,      TRC_CT_జాలం },
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SSH
  { &Curl_trc_feat_ssh,       TRC_CT_PROTOCOL },
#అయితే_ముగింపు
#ఒకవేళ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
  { &Curl_trc_feat_ws,        TRC_CT_PROTOCOL },
#అయితే_ముగింపు
};

నిర్మాణం trc_cft_def {
  నిర్మాణం Curl_cftype *cft;
  గుర్తులేని పూర్ణాంకం category;
};

స్థిర నిర్మాణం trc_cft_def trc_cfts[] = {
  { &Curl_cft_dns,            TRC_CT_జాలం },
  { &Curl_cft_tcp,            TRC_CT_జాలం },
  { &Curl_cft_udp,            TRC_CT_జాలం },
  { &Curl_cft_unix,           TRC_CT_జాలం },
  { &Curl_cft_tcp_accept,     TRC_CT_జాలం },
  { &Curl_cft_ip_happy,       TRC_CT_జాలం },
#నిర్వచితం_కాకపోతే CURL_DISABLE_WEBSOCKETS
  { &Curl_cft_recvbuf,        TRC_CT_PROTOCOL },
#అయితే_ముగింపు
  { &Curl_cft_అమరిక,          TRC_CT_PROTOCOL },
#ఒకవేళ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  { &Curl_cft_nghttp2,        TRC_CT_PROTOCOL },
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SSL
  { &Curl_cft_ssl,            TRC_CT_జాలం },
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  { &Curl_cft_ssl_proxy,      TRC_CT_PROXY },
#అయితే_ముగింపు
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
  { &Curl_cft_h1_proxy,       TRC_CT_PROXY },
#నిర్వచితమైతే USE_NGHTTP2
  { &Curl_cft_h2_proxy,       TRC_CT_PROXY },
#అయితే_ముగింపు
#ఒకవేళ defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3)
  { &Curl_cft_h3_proxy,       TRC_CT_PROXY },
#అయితే_ముగింపు
  { &Curl_cft_http_proxy,     TRC_CT_PROXY },
#అయితే_ముగింపు /* !CURL_DISABLE_HTTP */
  { &Curl_cft_haproxy,        TRC_CT_PROXY },
  { &Curl_cft_socks_proxy,    TRC_CT_PROXY },
#అయితే_ముగింపు /* !CURL_DISABLE_PROXY */
#ఒకవేళ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { &Curl_cft_http3,          TRC_CT_PROTOCOL },
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
  { &Curl_cft_http_connect,   TRC_CT_PROTOCOL },
#అయితే_ముగింపు
};

స్థిర శూన్య trc_apply_level_by_name(నిర్మాణం Curl_str *token, పూర్ణాంకం lvl)
{
  size_t i;

  కొరకు(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    ఒకవేళ(curlx_str_casecompare(token, trc_cfts[i].cft->name)) {
      trc_cfts[i].cft->log_level = lvl;
      ఆపు;
    }
  }
  కొరకు(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    ఒకవేళ(curlx_str_casecompare(token, trc_feats[i].feat->name)) {
      trc_feats[i].feat->log_level = lvl;
      ఆపు;
    }
  }
}

స్థిర శూన్య trc_apply_level_by_category(గుర్తులేని పూర్ణాంకం category, పూర్ణాంకం lvl)
{
  size_t i;

  కొరకు(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    ఒకవేళ(!category || (trc_cfts[i].category & category))
      trc_cfts[i].cft->log_level = lvl;
  }
  కొరకు(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    ఒకవేళ(!category || (trc_feats[i].category & category))
      trc_feats[i].feat->log_level = lvl;
  }
}

స్థిర CURLcode trc_opt(స్థిరరాశి అక్షరం *config)
{
  నిర్మాణం Curl_str బహిర్గతం;
  అయితే(!curlx_str_until(&config, &బహిర్గతం, 32, ',')) {
    పూర్ణాంకం lvl = CURL_LOG_LVL_INFO;
    స్థిరరాశి అక్షరం *token = curlx_str(&బహిర్గతం);

    ఒకవేళ(*token == '-') {
      lvl = CURL_LOG_LVL_NONE;
      curlx_str_nudge(&బహిర్గతం, 1);
    }
    లేకపోతే ఒకవేళ(*token == '+')
      curlx_str_nudge(&బహిర్గతం, 1);

    ఒకవేళ(curlx_str_casecompare(&బహిర్గతం, "all"))
      trc_apply_level_by_category(TRC_CT_NONE, lvl);
    లేకపోతే ఒకవేళ(curlx_str_casecompare(&బహిర్గతం, "protocol"))
      trc_apply_level_by_category(TRC_CT_PROTOCOL, lvl);
    లేకపోతే ఒకవేళ(curlx_str_casecompare(&బహిర్గతం, "network"))
      trc_apply_level_by_category(TRC_CT_జాలం, lvl);
    లేకపోతే ఒకవేళ(curlx_str_casecompare(&బహిర్గతం, "proxy"))
      trc_apply_level_by_category(TRC_CT_PROXY, lvl);
    లేకపోతే
      trc_apply_level_by_name(&బహిర్గతం, lvl);

    ఒకవేళ(curlx_str_single(&config, ','))
      ఆపు;
  }
  తిరిగి CURLE_OK;
}

CURLcode Curl_trc_opt(స్థిరరాశి అక్షరం *config)
{
  CURLcode ఫలితం = config ? trc_opt(config) : CURLE_OK;
#నిర్వచితమైతే DEBUGBUILD
  /* CURL_DEBUG can override anything */
  ఒకవేళ(!ఫలితం) {
    స్థిరరాశి అక్షరం *dbg_config = getenv("CURL_DEBUG");
    ఒకవేళ(dbg_config)
      ఫలితం = trc_opt(dbg_config);
  }
#అయితే_ముగింపు /* DEBUGBUILD */
  తిరిగి ఫలితం;
}

CURLcode Curl_trc_init(శూన్య)
{
#నిర్వచితమైతే DEBUGBUILD
  తిరిగి Curl_trc_opt(ఖాళీ);
#లేకపోతే
  తిరిగి CURLE_OK;
#అయితే_ముగింపు
}

#లేకపోతే /* CURL_DISABLE_VERBOSE_STRINGS */

CURLcode Curl_trc_init(శూన్య)
{
  తిరిగి CURLE_OK;
}

శూన్య Curl_infof(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *fmt, ...)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)fmt;
}

శూన్య Curl_trc_cf_infof(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి నిర్మాణం Curl_cfilter *cf,
                       స్థిరరాశి అక్షరం *fmt, ...)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)cf;
  (శూన్య)fmt;
}

శూన్య Curl_trc_multi(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *fmt, ...)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)fmt;
}

శూన్య Curl_trc_write(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *fmt, ...)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)fmt;
}

శూన్య Curl_trc_dns(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *fmt, ...)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)fmt;
}

శూన్య Curl_trc_timer(నిర్మాణం Curl_easy *దత్తాంశం, పూర్ణాంకం tid, స్థిరరాశి అక్షరం *fmt, ...)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)tid;
  (శూన్య)fmt;
}

శూన్య Curl_trc_read(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *fmt, ...)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)fmt;
}

#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
శూన్య Curl_trc_ftp(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *fmt, ...)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)fmt;
}
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_SMTP
శూన్య Curl_trc_smtp(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *fmt, ...)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)fmt;
}
#అయితే_ముగింపు
#ఒకవేళ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
శూన్య Curl_trc_ws(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *fmt, ...)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)fmt;
}
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SSH
శూన్య Curl_trc_ssh(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *fmt, ...)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)fmt;
}
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SSL
శూన్య Curl_trc_ssls(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *fmt, ...)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)fmt;
}
#అయితే_ముగింపు

#అయితే_ముగింపు /* !CURL_DISABLE_VERBOSE_STRINGS */
