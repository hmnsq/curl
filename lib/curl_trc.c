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
#شامل "curl_setup.h"

#شامل "curl_trc.h"
#شامل "urldata.h"
#شامل "cfilters.h"
#شامل "multiif.h"

#شامل "cf-recvbuf.h"
#شامل "cf-socket.h"
#شامل "cf-setup.h"
#شامل "http2.h"
#شامل "http_proxy.h"
#شامل "cf-h1-proxy.h"
#شامل "cf-h2-proxy.h"
#شامل "cf-haproxy.h"
#شامل "cf-https-connect.h"
#شامل "cf-ip-happy.h"
#شامل "progress.h"
#شامل "socks.h"
#شامل "curlx/strparse.h"
#شامل "vdns/cf-dns.h"
#شامل "vtls/vtls.h"
#شامل "vquic/vquic.h"
#شامل "curlx/strcopy.h"

ساکن خالی trc_write(ساخت Curl_easy *ڈیٹا, curl_infotype type,
                      ثابت حرف *ptr, size_t سائز)
{
  اگر(ڈیٹا->سیٹ.verbose) {
    اگر(ڈیٹا->سیٹ.fdebug) {
      ساخت Curl_mapi_guard guard;
      CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_fdebug);
      (خالی)(*ڈیٹا->سیٹ.fdebug)(ڈیٹا, type, CURL_UNCONST(ptr), سائز,
                                ڈیٹا->سیٹ.debugdata);
      CURL_CBAPI_انتہا(&guard);
    }
    ورنہ {
      ساکن ثابت حرف s_infotype[CURLINFO_انتہا][3] = {
        "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
      انتخاب(type) {
      حالت CURLINFO_متن:
      حالت CURLINFO_سرخی_برآمد:
      حالت CURLINFO_سرخی_اندر:
        fwrite(s_infotype[type], 2, 1, ڈیٹا->سیٹ.err);
        fwrite(ptr, سائز, 1, ڈیٹا->سیٹ.err);
        توڑو;
      طےشدہ: /* nada */
        توڑو;
      }
    }
  }
}

/* max length we trace before ending in '...' */
#تعریف_کرو TRC_سطر_MAX 2048

#تعریف_کرو CURL_TRC_FMT_IDSC   "[x-%" CURL_FORMAT_CURL_OFF_T "] "
#تعریف_کرو CURL_TRC_FMT_IDSD   "[%" CURL_FORMAT_CURL_OFF_T "-x] "
#تعریف_کرو CURL_TRC_FMT_IDSDC  "[%" CURL_FORMAT_CURL_OFF_T "-%" \
                            CURL_FORMAT_CURL_OFF_T "] "

ساکن ساخت curl_trc_feat Curl_trc_feat_ids = {
  "LIB-IDS",
  CURL_LOG_LVL_NONE,
};
#تعریف_کرو CURL_TRC_IDS(ڈیٹا) \
  (Curl_trc_is_verbose(ڈیٹا) && \
  Curl_trc_feat_ids.log_level >= CURL_LOG_LVL_INFO)

ساکن size_t trc_print_ids(ساخت Curl_easy *ڈیٹا, حرف *buf, size_t maxlen)
{
  curl_off_t cid = ڈیٹا->state.آخری_conn_id;
  اگر(ڈیٹا->id >= 0) {
    اگر(cid >= 0)
      واپس curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSDC, ڈیٹا->id, cid);
    ورنہ
      واپس curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSD, ڈیٹا->id);
  }
  ورنہ اگر(cid >= 0)
    واپس curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSC, cid);
  ورنہ {
    واپس curl_msnprintf(buf, maxlen, "[x-x] ");
  }
}

ساکن size_t trc_انتہا_buf(حرف *buf, size_t len, size_t maxlen, bool addnl)
{
  /* make sure we end the trace line in `buf` properly. It needs
   * to end with a null-terminator '\0' or '\n\0' */
  اگر(len >= (maxlen - (addnl ? 2 : 1))) {
    len = maxlen - 5;
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '\n';
  }
  ورنہ اگر(addnl)
    buf[len++] = '\n';
  buf[len] = '\0';
  واپس len;
}

خالی Curl_debug(ساخت Curl_easy *ڈیٹا, curl_infotype type,
                ثابت حرف *ptr, size_t سائز)
{
  اگر(ڈیٹا->سیٹ.verbose) {
    ساکن ثابت حرف s_infotype[CURLINFO_انتہا][3] = {
      "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
    حرف buf[TRC_سطر_MAX];
    size_t len;
    اگر(ڈیٹا->سیٹ.fdebug) {
      ساخت Curl_mapi_guard guard;
      اگر(CURL_TRC_IDS(ڈیٹا) && (سائز < TRC_سطر_MAX)) {
        len = trc_print_ids(ڈیٹا, buf, TRC_سطر_MAX);
        len += curl_msnprintf(buf + len, TRC_سطر_MAX - len, "%.*s",
                              (صحیح)سائز, ptr);
        len = trc_انتہا_buf(buf, len, TRC_سطر_MAX, FALSE);
        CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_fdebug);
        (خالی)(*ڈیٹا->سیٹ.fdebug)(ڈیٹا, type, buf, len, ڈیٹا->سیٹ.debugdata);
        CURL_CBAPI_انتہا(&guard);
      }
      ورنہ {
        CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_fdebug);
        (خالی)(*ڈیٹا->سیٹ.fdebug)(ڈیٹا, type, CURL_UNCONST(ptr),
                                  سائز, ڈیٹا->سیٹ.debugdata);
        CURL_CBAPI_انتہا(&guard);
      }
    }
    ورنہ {
      انتخاب(type) {
      حالت CURLINFO_متن:
      حالت CURLINFO_سرخی_برآمد:
      حالت CURLINFO_سرخی_اندر:
#اگر_غیر_تعریف_شدہ CURL_DISABLE_VERBOSE_لڑیاں
        اگر(CURL_TRC_IDS(ڈیٹا)) {
          len = trc_print_ids(ڈیٹا, buf, TRC_سطر_MAX);
          fwrite(buf, len, 1, ڈیٹا->سیٹ.err);
        }
#اگر_ختم
        fwrite(s_infotype[type], 2, 1, ڈیٹا->سیٹ.err);
        fwrite(ptr, سائز, 1, ڈیٹا->سیٹ.err);
        توڑو;
      طےشدہ: /* nada */
        توڑو;
      }
    }
  }
}

/* Curl_failf() is for messages stating why we failed.
 * The message SHALL NOT include any LF or CR.
 */
خالی Curl_failf(ساخت Curl_easy *ڈیٹا, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  اگر(ڈیٹا->سیٹ.verbose || ڈیٹا->سیٹ.errorbuffer) {
    va_list ap;
    size_t len;
    حرف error[CURL_ERROR_سائز + 2];
    va_start(ap, fmt);
    len = curl_mvsnprintf(error, CURL_ERROR_سائز, fmt, ap);

    اگر(ڈیٹا->سیٹ.errorbuffer && !ڈیٹا->state.errorbuf) {
      curlx_strcopy(ڈیٹا->سیٹ.errorbuffer, CURL_ERROR_سائز, error, len);
      ڈیٹا->state.errorbuf = TRUE; /* wrote error string */
    }
    error[len++] = '\n';
    error[len] = '\0';
    trc_write(ڈیٹا, CURLINFO_متن, error, len);
    va_end(ap);
  }
}

خالی Curl_reset_fail(ساخت Curl_easy *ڈیٹا)
{
  اگر(ڈیٹا->سیٹ.errorbuffer)
    ڈیٹا->سیٹ.errorbuffer[0] = 0;
  ڈیٹا->state.errorbuf = FALSE;
}

#اگر_تعریف_شدہ CURLVERBOSE
ساخت curl_trc_feat Curl_trc_feat_multi = {
  "MULTI",
  CURL_LOG_LVL_NONE,
};
ساخت curl_trc_feat Curl_trc_feat_read = {
  "READ",
  CURL_LOG_LVL_NONE,
};
ساخت curl_trc_feat Curl_trc_feat_write = {
  "WRITE",
  CURL_LOG_LVL_NONE,
};
ساخت curl_trc_feat Curl_trc_feat_dns = {
  "DNS",
  CURL_LOG_LVL_NONE,
};
#اگر_غیر_تعریف_شدہ CURL_DISABLE_DOH
ساخت curl_trc_feat Curl_trc_feat_doh = {
  "DoH",
  CURL_LOG_LVL_NONE,
};
#اگر_ختم
ساخت curl_trc_feat Curl_trc_feat_timer = {
  "TIMER",
  CURL_LOG_LVL_NONE,
};
#اگر_تعریف_شدہ USE_دھاگے
ساخت curl_trc_feat Curl_trc_feat_دھاگے = {
  "THREADS",
  CURL_LOG_LVL_NONE,
};
#اگر_ختم
#اگر_ختم

#اگر_غیر_تعریف_شدہ CURL_DISABLE_VERBOSE_لڑیاں

ساکن خالی trc_infof(ساخت Curl_easy *ڈیٹا,
                      ساخت curl_trc_feat *feat,
                      ثابت حرف *opt_id, صحیح opt_id_idx,
                      ثابت حرف * ثابت fmt, va_list ap) CURL_PRINTF(5, 0);

ساکن خالی trc_infof(ساخت Curl_easy *ڈیٹا,
                      ساخت curl_trc_feat *feat,
                      ثابت حرف *opt_id, صحیح opt_id_idx,
                      ثابت حرف * ثابت fmt, va_list ap)
{
  size_t len = 0;
  حرف buf[TRC_سطر_MAX];

  اگر(CURL_TRC_IDS(ڈیٹا))
    len += trc_print_ids(ڈیٹا, buf + len, TRC_سطر_MAX - len);
  اگر(feat)
    len += curl_msnprintf(buf + len, TRC_سطر_MAX - len, "[%s] ", feat->name);
  اگر(opt_id) {
    اگر(opt_id_idx > 0)
      len += curl_msnprintf(buf + len, TRC_سطر_MAX - len, "[%s-%d] ",
                            opt_id, opt_id_idx);
    ورنہ
      len += curl_msnprintf(buf + len, TRC_سطر_MAX - len, "[%s] ", opt_id);
  }
  len += curl_mvsnprintf(buf + len, TRC_سطر_MAX - len, fmt, ap);
  len = trc_انتہا_buf(buf, len, TRC_سطر_MAX, TRUE);
  trc_write(ڈیٹا, CURLINFO_متن, buf, len);
}

خالی Curl_infof(ساخت Curl_easy *ڈیٹا, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  اگر(Curl_trc_is_verbose(ڈیٹا)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ڈیٹا, ڈیٹا->state.feat, تہی, 0, fmt, ap);
    va_end(ap);
  }
}

خالی Curl_trc_cf_infof(ساخت Curl_easy *ڈیٹا, ثابت ساخت Curl_cfilter *cf,
                       ثابت حرف *fmt, ...)
{
  DEBUGASSERT(cf);
  اگر(Curl_trc_cf_is_verbose(cf, ڈیٹا)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ڈیٹا, ڈیٹا->state.feat, cf->cft->name, cf->sockindex, fmt, ap);
    va_end(ap);
  }
}

خالی Curl_trc_feat_infof(ساخت Curl_easy *ڈیٹا,
                         ساخت curl_trc_feat *feat,
                         ثابت حرف *fmt, ...)
{
  DEBUGASSERT(feat);
  اگر(Curl_trc_ft_is_verbose(ڈیٹا, feat)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ڈیٹا, feat, تہی, 0, fmt, ap);
    va_end(ap);
  }
}

ساکن ثابت حرف * ثابت Curl_trc_timer_نام[] = {
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

ساکن ثابت حرف *trc_timer_name(صحیح tid)
{
  اگر((tid >= 0) && ((size_t)tid < CURL_ARRAYSIZE(Curl_trc_timer_نام)))
    واپس Curl_trc_timer_نام[(size_t)tid];
  واپس "TIMER-???";
}

خالی Curl_trc_timer(ساخت Curl_easy *ڈیٹا, صحیح tid, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  اگر(Curl_trc_ft_is_verbose(ڈیٹا, &Curl_trc_feat_timer)) {
    ثابت حرف *tname = trc_timer_name(tid);
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ڈیٹا, &Curl_trc_feat_timer, tname, 0, fmt, ap);
    va_end(ap);
  }
}

خالی Curl_trc_easy_timers(ساخت Curl_easy *ڈیٹا)
{
  اگر(CURL_TRC_TIMER_is_verbose(ڈیٹا) && ڈیٹا->multi) {
    اگر(ڈیٹا->state.timeouts.پہلا < EXPIRE_آخری) {
      ساخت expire_timers *timeouts = &ڈیٹا->state.timeouts;
      timediff_t بنیاد_us =
        Curl_timeouts_انحراف_us(&ڈیٹا->multi->timeouts, Curl_pgrs_now(ڈیٹا));
      uint8_t id = ڈیٹا->state.timeouts.پہلا;
      برائے(; id < EXPIRE_آخری; id = timeouts->اگلا[id]) {
        CURL_TRC_TIMER(ڈیٹا, id, "expires in %" FMT_TIMEDIFF_T "us",
                       timeouts->انحراف_us[id] - بنیاد_us);
      }
    }
  }
}

ساکن ثابت حرف * ثابت Curl_trc_mstate_نام[] = {
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

ثابت حرف *Curl_trc_mstate_name(صحیح state)
{
  اگر((state >= 0) && ((size_t)state < CURL_ARRAYSIZE(Curl_trc_mstate_نام)))
    واپس Curl_trc_mstate_نام[(size_t)state];
  واپس "?";
}

خالی Curl_trc_multi(ساخت Curl_easy *ڈیٹا, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  اگر(Curl_trc_ft_is_verbose(ڈیٹا, &Curl_trc_feat_multi)) {
    ثابت حرف *sname = (ڈیٹا->id >= 0) ?
                        Curl_trc_mstate_name(ڈیٹا->mstate) : تہی;
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ڈیٹا, &Curl_trc_feat_multi, sname, 0, fmt, ap);
    va_end(ap);
  }
}

خالی Curl_trc_read(ساخت Curl_easy *ڈیٹا, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  اگر(Curl_trc_ft_is_verbose(ڈیٹا, &Curl_trc_feat_read)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ڈیٹا, &Curl_trc_feat_read, تہی, 0, fmt, ap);
    va_end(ap);
  }
}

خالی Curl_trc_write(ساخت Curl_easy *ڈیٹا, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  اگر(Curl_trc_ft_is_verbose(ڈیٹا, &Curl_trc_feat_write)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ڈیٹا, &Curl_trc_feat_write, تہی, 0, fmt, ap);
    va_end(ap);
  }
}

خالی Curl_trc_dns(ساخت Curl_easy *ڈیٹا, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  اگر(Curl_trc_ft_is_verbose(ڈیٹا, &Curl_trc_feat_dns)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ڈیٹا, &Curl_trc_feat_dns, تہی, 0, fmt, ap);
    va_end(ap);
  }
}

#اگر_غیر_تعریف_شدہ CURL_DISABLE_FTP
ساخت curl_trc_feat Curl_trc_feat_ftp = {
  "FTP",
  CURL_LOG_LVL_NONE,
};

خالی Curl_trc_ftp(ساخت Curl_easy *ڈیٹا, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  اگر(Curl_trc_ft_is_verbose(ڈیٹا, &Curl_trc_feat_ftp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ڈیٹا, &Curl_trc_feat_ftp, تہی, 0, fmt, ap);
    va_end(ap);
  }
}
#اگر_ختم /* !CURL_DISABLE_FTP */

#اگر_غیر_تعریف_شدہ CURL_DISABLE_SMTP
ساخت curl_trc_feat Curl_trc_feat_smtp = {
  "SMTP",
  CURL_LOG_LVL_NONE,
};

خالی Curl_trc_smtp(ساخت Curl_easy *ڈیٹا, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  اگر(Curl_trc_ft_is_verbose(ڈیٹا, &Curl_trc_feat_smtp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ڈیٹا, &Curl_trc_feat_smtp, تہی, 0, fmt, ap);
    va_end(ap);
  }
}
#اگر_ختم /* !CURL_DISABLE_SMTP */

#اگر_تعریف_شدہ USE_SSL
ساخت curl_trc_feat Curl_trc_feat_ssls = {
  "SSLS",
  CURL_LOG_LVL_NONE,
};

خالی Curl_trc_ssls(ساخت Curl_easy *ڈیٹا, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  اگر(Curl_trc_ft_is_verbose(ڈیٹا, &Curl_trc_feat_ssls)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ڈیٹا, &Curl_trc_feat_ssls, تہی, 0, fmt, ap);
    va_end(ap);
  }
}
#اگر_ختم /* USE_SSL */

#اگر_تعریف_شدہ USE_SSH
ساخت curl_trc_feat Curl_trc_feat_ssh = {
  "SSH",
  CURL_LOG_LVL_NONE,
};

خالی Curl_trc_ssh(ساخت Curl_easy *ڈیٹا, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  اگر(Curl_trc_ft_is_verbose(ڈیٹا, &Curl_trc_feat_ssh)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ڈیٹا, &Curl_trc_feat_ssh, تہی, 0, fmt, ap);
    va_end(ap);
  }
}
#اگر_ختم /* USE_SSH */

#اگر !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
ساخت curl_trc_feat Curl_trc_feat_ws = {
  "WS",
  CURL_LOG_LVL_NONE,
};

خالی Curl_trc_ws(ساخت Curl_easy *ڈیٹا, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  اگر(Curl_trc_ft_is_verbose(ڈیٹا, &Curl_trc_feat_ws)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ڈیٹا, &Curl_trc_feat_ws, تہی, 0, fmt, ap);
    va_end(ap);
  }
}
#اگر_ختم /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#تعریف_کرو TRC_CT_NONE        0
#تعریف_کرو TRC_CT_PROTOCOL    (1 << 0)
#تعریف_کرو TRC_CT_جال     (1 << 1)
#تعریف_کرو TRC_CT_PROXY       (1 << 2)
#تعریف_کرو TRC_CT_INTERNALS   (1 << 3)

ساخت trc_feat_def {
  ساخت curl_trc_feat *feat;
  غیرعلامتی صحیح category;
};

ساکن ساخت trc_feat_def trc_feats[] = {
  { &Curl_trc_feat_ids,       TRC_CT_INTERNALS },
  { &Curl_trc_feat_multi,     TRC_CT_جال },
  { &Curl_trc_feat_read,      TRC_CT_NONE },
  { &Curl_trc_feat_write,     TRC_CT_NONE },
  { &Curl_trc_feat_dns,       TRC_CT_جال },
#اگر_غیر_تعریف_شدہ CURL_DISABLE_DOH
  { &Curl_trc_feat_doh,       TRC_CT_جال },
#اگر_ختم
  { &Curl_trc_feat_timer,     TRC_CT_جال },
#اگر_تعریف_شدہ USE_دھاگے
  { &Curl_trc_feat_دھاگے,   TRC_CT_NONE },
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_FTP
  { &Curl_trc_feat_ftp,       TRC_CT_PROTOCOL },
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_SMTP
  { &Curl_trc_feat_smtp,      TRC_CT_PROTOCOL },
#اگر_ختم
#اگر_تعریف_شدہ USE_SSL
  { &Curl_trc_feat_ssls,      TRC_CT_جال },
#اگر_ختم
#اگر_تعریف_شدہ USE_SSH
  { &Curl_trc_feat_ssh,       TRC_CT_PROTOCOL },
#اگر_ختم
#اگر !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
  { &Curl_trc_feat_ws,        TRC_CT_PROTOCOL },
#اگر_ختم
};

ساخت trc_cft_def {
  ساخت Curl_cftype *cft;
  غیرعلامتی صحیح category;
};

ساکن ساخت trc_cft_def trc_cfts[] = {
  { &Curl_cft_dns,            TRC_CT_جال },
  { &Curl_cft_tcp,            TRC_CT_جال },
  { &Curl_cft_udp,            TRC_CT_جال },
  { &Curl_cft_unix,           TRC_CT_جال },
  { &Curl_cft_tcp_accept,     TRC_CT_جال },
  { &Curl_cft_ip_happy,       TRC_CT_جال },
#اگر_غیر_تعریف_شدہ CURL_DISABLE_WEBSOCKETS
  { &Curl_cft_recvbuf,        TRC_CT_PROTOCOL },
#اگر_ختم
  { &Curl_cft_تیاری,          TRC_CT_PROTOCOL },
#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  { &Curl_cft_nghttp2,        TRC_CT_PROTOCOL },
#اگر_ختم
#اگر_تعریف_شدہ USE_SSL
  { &Curl_cft_ssl,            TRC_CT_جال },
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  { &Curl_cft_ssl_proxy,      TRC_CT_PROXY },
#اگر_ختم
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP
  { &Curl_cft_h1_proxy,       TRC_CT_PROXY },
#اگر_تعریف_شدہ USE_NGHTTP2
  { &Curl_cft_h2_proxy,       TRC_CT_PROXY },
#اگر_ختم
#اگر defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3)
  { &Curl_cft_h3_proxy,       TRC_CT_PROXY },
#اگر_ختم
  { &Curl_cft_http_proxy,     TRC_CT_PROXY },
#اگر_ختم /* !CURL_DISABLE_HTTP */
  { &Curl_cft_haproxy,        TRC_CT_PROXY },
  { &Curl_cft_socks_proxy,    TRC_CT_PROXY },
#اگر_ختم /* !CURL_DISABLE_PROXY */
#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { &Curl_cft_http3,          TRC_CT_PROTOCOL },
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP
  { &Curl_cft_http_connect,   TRC_CT_PROTOCOL },
#اگر_ختم
};

ساکن خالی trc_apply_level_by_name(ساخت Curl_str *token, صحیح lvl)
{
  size_t i;

  برائے(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    اگر(curlx_str_casecompare(token, trc_cfts[i].cft->name)) {
      trc_cfts[i].cft->log_level = lvl;
      توڑو;
    }
  }
  برائے(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    اگر(curlx_str_casecompare(token, trc_feats[i].feat->name)) {
      trc_feats[i].feat->log_level = lvl;
      توڑو;
    }
  }
}

ساکن خالی trc_apply_level_by_category(غیرعلامتی صحیح category, صحیح lvl)
{
  size_t i;

  برائے(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    اگر(!category || (trc_cfts[i].category & category))
      trc_cfts[i].cft->log_level = lvl;
  }
  برائے(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    اگر(!category || (trc_feats[i].category & category))
      trc_feats[i].feat->log_level = lvl;
  }
}

ساکن CURLcode trc_opt(ثابت حرف *config)
{
  ساخت Curl_str برآمد;
  جب(!curlx_str_until(&config, &برآمد, 32, ',')) {
    صحیح lvl = CURL_LOG_LVL_INFO;
    ثابت حرف *token = curlx_str(&برآمد);

    اگر(*token == '-') {
      lvl = CURL_LOG_LVL_NONE;
      curlx_str_nudge(&برآمد, 1);
    }
    ورنہ اگر(*token == '+')
      curlx_str_nudge(&برآمد, 1);

    اگر(curlx_str_casecompare(&برآمد, "all"))
      trc_apply_level_by_category(TRC_CT_NONE, lvl);
    ورنہ اگر(curlx_str_casecompare(&برآمد, "protocol"))
      trc_apply_level_by_category(TRC_CT_PROTOCOL, lvl);
    ورنہ اگر(curlx_str_casecompare(&برآمد, "network"))
      trc_apply_level_by_category(TRC_CT_جال, lvl);
    ورنہ اگر(curlx_str_casecompare(&برآمد, "proxy"))
      trc_apply_level_by_category(TRC_CT_PROXY, lvl);
    ورنہ
      trc_apply_level_by_name(&برآمد, lvl);

    اگر(curlx_str_single(&config, ','))
      توڑو;
  }
  واپس CURLE_OK;
}

CURLcode Curl_trc_opt(ثابت حرف *config)
{
  CURLcode نتیجہ = config ? trc_opt(config) : CURLE_OK;
#اگر_تعریف_شدہ DEBUGBUILD
  /* CURL_DEBUG can override anything */
  اگر(!نتیجہ) {
    ثابت حرف *dbg_config = getenv("CURL_DEBUG");
    اگر(dbg_config)
      نتیجہ = trc_opt(dbg_config);
  }
#اگر_ختم /* DEBUGBUILD */
  واپس نتیجہ;
}

CURLcode Curl_trc_init(خالی)
{
#اگر_تعریف_شدہ DEBUGBUILD
  واپس Curl_trc_opt(تہی);
#ورنہ
  واپس CURLE_OK;
#اگر_ختم
}

#ورنہ /* CURL_DISABLE_VERBOSE_STRINGS */

CURLcode Curl_trc_init(خالی)
{
  واپس CURLE_OK;
}

خالی Curl_infof(ساخت Curl_easy *ڈیٹا, ثابت حرف *fmt, ...)
{
  (خالی)ڈیٹا;
  (خالی)fmt;
}

خالی Curl_trc_cf_infof(ساخت Curl_easy *ڈیٹا, ثابت ساخت Curl_cfilter *cf,
                       ثابت حرف *fmt, ...)
{
  (خالی)ڈیٹا;
  (خالی)cf;
  (خالی)fmt;
}

خالی Curl_trc_multi(ساخت Curl_easy *ڈیٹا, ثابت حرف *fmt, ...)
{
  (خالی)ڈیٹا;
  (خالی)fmt;
}

خالی Curl_trc_write(ساخت Curl_easy *ڈیٹا, ثابت حرف *fmt, ...)
{
  (خالی)ڈیٹا;
  (خالی)fmt;
}

خالی Curl_trc_dns(ساخت Curl_easy *ڈیٹا, ثابت حرف *fmt, ...)
{
  (خالی)ڈیٹا;
  (خالی)fmt;
}

خالی Curl_trc_timer(ساخت Curl_easy *ڈیٹا, صحیح tid, ثابت حرف *fmt, ...)
{
  (خالی)ڈیٹا;
  (خالی)tid;
  (خالی)fmt;
}

خالی Curl_trc_read(ساخت Curl_easy *ڈیٹا, ثابت حرف *fmt, ...)
{
  (خالی)ڈیٹا;
  (خالی)fmt;
}

#اگر_غیر_تعریف_شدہ CURL_DISABLE_FTP
خالی Curl_trc_ftp(ساخت Curl_easy *ڈیٹا, ثابت حرف *fmt, ...)
{
  (خالی)ڈیٹا;
  (خالی)fmt;
}
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_SMTP
خالی Curl_trc_smtp(ساخت Curl_easy *ڈیٹا, ثابت حرف *fmt, ...)
{
  (خالی)ڈیٹا;
  (خالی)fmt;
}
#اگر_ختم
#اگر !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
خالی Curl_trc_ws(ساخت Curl_easy *ڈیٹا, ثابت حرف *fmt, ...)
{
  (خالی)ڈیٹا;
  (خالی)fmt;
}
#اگر_ختم
#اگر_تعریف_شدہ USE_SSH
خالی Curl_trc_ssh(ساخت Curl_easy *ڈیٹا, ثابت حرف *fmt, ...)
{
  (خالی)ڈیٹا;
  (خالی)fmt;
}
#اگر_ختم
#اگر_تعریف_شدہ USE_SSL
خالی Curl_trc_ssls(ساخت Curl_easy *ڈیٹا, ثابت حرف *fmt, ...)
{
  (خالی)ڈیٹا;
  (خالی)fmt;
}
#اگر_ختم

#اگر_ختم /* !CURL_DISABLE_VERBOSE_STRINGS */
