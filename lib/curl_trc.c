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
#ضمن "curl_setup.h"

#ضمن "curl_trc.h"
#ضمن "urldata.h"
#ضمن "cfilters.h"
#ضمن "multiif.h"

#ضمن "cf-recvbuf.h"
#ضمن "cf-socket.h"
#ضمن "cf-setup.h"
#ضمن "http2.h"
#ضمن "http_proxy.h"
#ضمن "cf-h1-proxy.h"
#ضمن "cf-h2-proxy.h"
#ضمن "cf-haproxy.h"
#ضمن "cf-https-connect.h"
#ضمن "cf-ip-happy.h"
#ضمن "progress.h"
#ضمن "socks.h"
#ضمن "curlx/strparse.h"
#ضمن "vdns/cf-dns.h"
#ضمن "vtls/vtls.h"
#ضمن "vquic/vquic.h"
#ضمن "curlx/strcopy.h"

ساكن فراغ trc_write(بنية Curl_easy *معلومات, curl_infotype type,
                      ثابت حرف *ptr, size_t اندازه)
{
  إذا(معلومات->سېټ.verbose) {
    إذا(معلومات->سېټ.fdebug) {
      بنية Curl_mapi_guard guard;
      CURL_CBAPI_پیل(&guard, معلومات, easy_fdebug);
      (فراغ)(*معلومات->سېټ.fdebug)(معلومات, type, CURL_UNCONST(ptr), اندازه,
                                معلومات->سېټ.debugdata);
      CURL_CBAPI_پای_ټکی(&guard);
    }
    وإلا {
      ساكن ثابت حرف s_infotype[CURLINFO_پای_ټکی][3] = {
        "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
      بدل(type) {
      حالة CURLINFO_متن:
      حالة CURLINFO_سرلیک_بهر:
      حالة CURLINFO_سرلیک_دننه:
        fwrite(s_infotype[type], 2, 1, معلومات->سېټ.err);
        fwrite(ptr, اندازه, 1, معلومات->سېټ.err);
        اكسر;
      افتراضي: /* nada */
        اكسر;
      }
    }
  }
}

/* max length we trace before ending in '...' */
#تعریف_کړه TRC_کرښه_MAX 2048

#تعریف_کړه CURL_TRC_FMT_IDSC   "[x-%" CURL_FORMAT_CURL_OFF_T "] "
#تعریف_کړه CURL_TRC_FMT_IDSD   "[%" CURL_FORMAT_CURL_OFF_T "-x] "
#تعریف_کړه CURL_TRC_FMT_IDSDC  "[%" CURL_FORMAT_CURL_OFF_T "-%" \
                            CURL_FORMAT_CURL_OFF_T "] "

ساكن بنية curl_trc_feat Curl_trc_feat_ids = {
  "LIB-IDS",
  CURL_LOG_LVL_NONE,
};
#تعریف_کړه CURL_TRC_IDS(معلومات) \
  (Curl_trc_is_verbose(معلومات) && \
  Curl_trc_feat_ids.log_level >= CURL_LOG_LVL_INFO)

ساكن size_t trc_print_ids(بنية Curl_easy *معلومات, حرف *buf, size_t maxlen)
{
  curl_off_t cid = معلومات->state.وروستی_conn_id;
  إذا(معلومات->id >= 0) {
    إذا(cid >= 0)
      أرجع curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSDC, معلومات->id, cid);
    وإلا
      أرجع curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSD, معلومات->id);
  }
  وإلا إذا(cid >= 0)
    أرجع curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSC, cid);
  وإلا {
    أرجع curl_msnprintf(buf, maxlen, "[x-x] ");
  }
}

ساكن size_t trc_پای_ټکی_buf(حرف *buf, size_t len, size_t maxlen, bool addnl)
{
  /* make sure we end the trace line in `buf` properly. It needs
   * to end with a null-terminator '\0' or '\n\0' */
  إذا(len >= (maxlen - (addnl ? 2 : 1))) {
    len = maxlen - 5;
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '\n';
  }
  وإلا إذا(addnl)
    buf[len++] = '\n';
  buf[len] = '\0';
  أرجع len;
}

فراغ Curl_debug(بنية Curl_easy *معلومات, curl_infotype type,
                ثابت حرف *ptr, size_t اندازه)
{
  إذا(معلومات->سېټ.verbose) {
    ساكن ثابت حرف s_infotype[CURLINFO_پای_ټکی][3] = {
      "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
    حرف buf[TRC_کرښه_MAX];
    size_t len;
    إذا(معلومات->سېټ.fdebug) {
      بنية Curl_mapi_guard guard;
      إذا(CURL_TRC_IDS(معلومات) && (اندازه < TRC_کرښه_MAX)) {
        len = trc_print_ids(معلومات, buf, TRC_کرښه_MAX);
        len += curl_msnprintf(buf + len, TRC_کرښه_MAX - len, "%.*s",
                              (صحيح)اندازه, ptr);
        len = trc_پای_ټکی_buf(buf, len, TRC_کرښه_MAX, FALSE);
        CURL_CBAPI_پیل(&guard, معلومات, easy_fdebug);
        (فراغ)(*معلومات->سېټ.fdebug)(معلومات, type, buf, len, معلومات->سېټ.debugdata);
        CURL_CBAPI_پای_ټکی(&guard);
      }
      وإلا {
        CURL_CBAPI_پیل(&guard, معلومات, easy_fdebug);
        (فراغ)(*معلومات->سېټ.fdebug)(معلومات, type, CURL_UNCONST(ptr),
                                  اندازه, معلومات->سېټ.debugdata);
        CURL_CBAPI_پای_ټکی(&guard);
      }
    }
    وإلا {
      بدل(type) {
      حالة CURLINFO_متن:
      حالة CURLINFO_سرلیک_بهر:
      حالة CURLINFO_سرلیک_دننه:
#که_نه_تعریف_شوی CURL_DISABLE_VERBOSE_مزي_ګانې
        إذا(CURL_TRC_IDS(معلومات)) {
          len = trc_print_ids(معلومات, buf, TRC_کرښه_MAX);
          fwrite(buf, len, 1, معلومات->سېټ.err);
        }
#د_که_پای
        fwrite(s_infotype[type], 2, 1, معلومات->سېټ.err);
        fwrite(ptr, اندازه, 1, معلومات->سېټ.err);
        اكسر;
      افتراضي: /* nada */
        اكسر;
      }
    }
  }
}

/* Curl_failf() is for messages stating why we failed.
 * The message SHALL NOT include any LF or CR.
 */
فراغ Curl_failf(بنية Curl_easy *معلومات, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  إذا(معلومات->سېټ.verbose || معلومات->سېټ.errorbuffer) {
    va_list ap;
    size_t len;
    حرف error[CURL_ERROR_اندازه + 2];
    va_start(ap, fmt);
    len = curl_mvsnprintf(error, CURL_ERROR_اندازه, fmt, ap);

    إذا(معلومات->سېټ.errorbuffer && !معلومات->state.errorbuf) {
      curlx_strcopy(معلومات->سېټ.errorbuffer, CURL_ERROR_اندازه, error, len);
      معلومات->state.errorbuf = TRUE; /* wrote error string */
    }
    error[len++] = '\n';
    error[len] = '\0';
    trc_write(معلومات, CURLINFO_متن, error, len);
    va_end(ap);
  }
}

فراغ Curl_reset_fail(بنية Curl_easy *معلومات)
{
  إذا(معلومات->سېټ.errorbuffer)
    معلومات->سېټ.errorbuffer[0] = 0;
  معلومات->state.errorbuf = FALSE;
}

#که_تعریف_شوی CURLVERBOSE
بنية curl_trc_feat Curl_trc_feat_multi = {
  "MULTI",
  CURL_LOG_LVL_NONE,
};
بنية curl_trc_feat Curl_trc_feat_read = {
  "READ",
  CURL_LOG_LVL_NONE,
};
بنية curl_trc_feat Curl_trc_feat_write = {
  "WRITE",
  CURL_LOG_LVL_NONE,
};
بنية curl_trc_feat Curl_trc_feat_dns = {
  "DNS",
  CURL_LOG_LVL_NONE,
};
#که_نه_تعریف_شوی CURL_DISABLE_DOH
بنية curl_trc_feat Curl_trc_feat_doh = {
  "DoH",
  CURL_LOG_LVL_NONE,
};
#د_که_پای
بنية curl_trc_feat Curl_trc_feat_timer = {
  "TIMER",
  CURL_LOG_LVL_NONE,
};
#که_تعریف_شوی USE_تارونه
بنية curl_trc_feat Curl_trc_feat_تارونه = {
  "THREADS",
  CURL_LOG_LVL_NONE,
};
#د_که_پای
#د_که_پای

#که_نه_تعریف_شوی CURL_DISABLE_VERBOSE_مزي_ګانې

ساكن فراغ trc_infof(بنية Curl_easy *معلومات,
                      بنية curl_trc_feat *feat,
                      ثابت حرف *opt_id, صحيح opt_id_idx,
                      ثابت حرف * ثابت fmt, va_list ap) CURL_PRINTF(5, 0);

ساكن فراغ trc_infof(بنية Curl_easy *معلومات,
                      بنية curl_trc_feat *feat,
                      ثابت حرف *opt_id, صحيح opt_id_idx,
                      ثابت حرف * ثابت fmt, va_list ap)
{
  size_t len = 0;
  حرف buf[TRC_کرښه_MAX];

  إذا(CURL_TRC_IDS(معلومات))
    len += trc_print_ids(معلومات, buf + len, TRC_کرښه_MAX - len);
  إذا(feat)
    len += curl_msnprintf(buf + len, TRC_کرښه_MAX - len, "[%s] ", feat->name);
  إذا(opt_id) {
    إذا(opt_id_idx > 0)
      len += curl_msnprintf(buf + len, TRC_کرښه_MAX - len, "[%s-%d] ",
                            opt_id, opt_id_idx);
    وإلا
      len += curl_msnprintf(buf + len, TRC_کرښه_MAX - len, "[%s] ", opt_id);
  }
  len += curl_mvsnprintf(buf + len, TRC_کرښه_MAX - len, fmt, ap);
  len = trc_پای_ټکی_buf(buf, len, TRC_کرښه_MAX, TRUE);
  trc_write(معلومات, CURLINFO_متن, buf, len);
}

فراغ Curl_infof(بنية Curl_easy *معلومات, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  إذا(Curl_trc_is_verbose(معلومات)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(معلومات, معلومات->state.feat, عدم, 0, fmt, ap);
    va_end(ap);
  }
}

فراغ Curl_trc_cf_infof(بنية Curl_easy *معلومات, ثابت بنية Curl_cfilter *cf,
                       ثابت حرف *fmt, ...)
{
  DEBUGASSERT(cf);
  إذا(Curl_trc_cf_is_verbose(cf, معلومات)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(معلومات, معلومات->state.feat, cf->cft->name, cf->sockindex, fmt, ap);
    va_end(ap);
  }
}

فراغ Curl_trc_feat_infof(بنية Curl_easy *معلومات,
                         بنية curl_trc_feat *feat,
                         ثابت حرف *fmt, ...)
{
  DEBUGASSERT(feat);
  إذا(Curl_trc_ft_is_verbose(معلومات, feat)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(معلومات, feat, عدم, 0, fmt, ap);
    va_end(ap);
  }
}

ساكن ثابت حرف * ثابت Curl_trc_timer_نومونه[] = {
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

ساكن ثابت حرف *trc_timer_name(صحيح tid)
{
  إذا((tid >= 0) && ((size_t)tid < CURL_ARRAYSIZE(Curl_trc_timer_نومونه)))
    أرجع Curl_trc_timer_نومونه[(size_t)tid];
  أرجع "TIMER-???";
}

فراغ Curl_trc_timer(بنية Curl_easy *معلومات, صحيح tid, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  إذا(Curl_trc_ft_is_verbose(معلومات, &Curl_trc_feat_timer)) {
    ثابت حرف *tname = trc_timer_name(tid);
    va_list ap;
    va_start(ap, fmt);
    trc_infof(معلومات, &Curl_trc_feat_timer, tname, 0, fmt, ap);
    va_end(ap);
  }
}

فراغ Curl_trc_easy_timers(بنية Curl_easy *معلومات)
{
  إذا(CURL_TRC_TIMER_is_verbose(معلومات) && معلومات->multi) {
    إذا(معلومات->state.timeouts.لومړی < EXPIRE_وروستی) {
      بنية expire_timers *timeouts = &معلومات->state.timeouts;
      timediff_t بنسټ_us =
        Curl_timeouts_دننه_واټن_us(&معلومات->multi->timeouts, Curl_pgrs_now(معلومات));
      uint8_t id = معلومات->state.timeouts.لومړی;
      لكل(; id < EXPIRE_وروستی; id = timeouts->بل[id]) {
        CURL_TRC_TIMER(معلومات, id, "expires in %" FMT_TIMEDIFF_T "us",
                       timeouts->دننه_واټن_us[id] - بنسټ_us);
      }
    }
  }
}

ساكن ثابت حرف * ثابت Curl_trc_mstate_نومونه[] = {
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

ثابت حرف *Curl_trc_mstate_name(صحيح state)
{
  إذا((state >= 0) && ((size_t)state < CURL_ARRAYSIZE(Curl_trc_mstate_نومونه)))
    أرجع Curl_trc_mstate_نومونه[(size_t)state];
  أرجع "?";
}

فراغ Curl_trc_multi(بنية Curl_easy *معلومات, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  إذا(Curl_trc_ft_is_verbose(معلومات, &Curl_trc_feat_multi)) {
    ثابت حرف *sname = (معلومات->id >= 0) ?
                        Curl_trc_mstate_name(معلومات->mstate) : عدم;
    va_list ap;
    va_start(ap, fmt);
    trc_infof(معلومات, &Curl_trc_feat_multi, sname, 0, fmt, ap);
    va_end(ap);
  }
}

فراغ Curl_trc_read(بنية Curl_easy *معلومات, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  إذا(Curl_trc_ft_is_verbose(معلومات, &Curl_trc_feat_read)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(معلومات, &Curl_trc_feat_read, عدم, 0, fmt, ap);
    va_end(ap);
  }
}

فراغ Curl_trc_write(بنية Curl_easy *معلومات, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  إذا(Curl_trc_ft_is_verbose(معلومات, &Curl_trc_feat_write)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(معلومات, &Curl_trc_feat_write, عدم, 0, fmt, ap);
    va_end(ap);
  }
}

فراغ Curl_trc_dns(بنية Curl_easy *معلومات, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  إذا(Curl_trc_ft_is_verbose(معلومات, &Curl_trc_feat_dns)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(معلومات, &Curl_trc_feat_dns, عدم, 0, fmt, ap);
    va_end(ap);
  }
}

#که_نه_تعریف_شوی CURL_DISABLE_FTP
بنية curl_trc_feat Curl_trc_feat_ftp = {
  "FTP",
  CURL_LOG_LVL_NONE,
};

فراغ Curl_trc_ftp(بنية Curl_easy *معلومات, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  إذا(Curl_trc_ft_is_verbose(معلومات, &Curl_trc_feat_ftp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(معلومات, &Curl_trc_feat_ftp, عدم, 0, fmt, ap);
    va_end(ap);
  }
}
#د_که_پای /* !CURL_DISABLE_FTP */

#که_نه_تعریف_شوی CURL_DISABLE_SMTP
بنية curl_trc_feat Curl_trc_feat_smtp = {
  "SMTP",
  CURL_LOG_LVL_NONE,
};

فراغ Curl_trc_smtp(بنية Curl_easy *معلومات, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  إذا(Curl_trc_ft_is_verbose(معلومات, &Curl_trc_feat_smtp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(معلومات, &Curl_trc_feat_smtp, عدم, 0, fmt, ap);
    va_end(ap);
  }
}
#د_که_پای /* !CURL_DISABLE_SMTP */

#که_تعریف_شوی USE_SSL
بنية curl_trc_feat Curl_trc_feat_ssls = {
  "SSLS",
  CURL_LOG_LVL_NONE,
};

فراغ Curl_trc_ssls(بنية Curl_easy *معلومات, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  إذا(Curl_trc_ft_is_verbose(معلومات, &Curl_trc_feat_ssls)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(معلومات, &Curl_trc_feat_ssls, عدم, 0, fmt, ap);
    va_end(ap);
  }
}
#د_که_پای /* USE_SSL */

#که_تعریف_شوی USE_SSH
بنية curl_trc_feat Curl_trc_feat_ssh = {
  "SSH",
  CURL_LOG_LVL_NONE,
};

فراغ Curl_trc_ssh(بنية Curl_easy *معلومات, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  إذا(Curl_trc_ft_is_verbose(معلومات, &Curl_trc_feat_ssh)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(معلومات, &Curl_trc_feat_ssh, عدم, 0, fmt, ap);
    va_end(ap);
  }
}
#د_که_پای /* USE_SSH */

#إذا !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
بنية curl_trc_feat Curl_trc_feat_ws = {
  "WS",
  CURL_LOG_LVL_NONE,
};

فراغ Curl_trc_ws(بنية Curl_easy *معلومات, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  إذا(Curl_trc_ft_is_verbose(معلومات, &Curl_trc_feat_ws)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(معلومات, &Curl_trc_feat_ws, عدم, 0, fmt, ap);
    va_end(ap);
  }
}
#د_که_پای /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#تعریف_کړه TRC_CT_NONE        0
#تعریف_کړه TRC_CT_PROTOCOL    (1 << 0)
#تعریف_کړه TRC_CT_شبکه     (1 << 1)
#تعریف_کړه TRC_CT_PROXY       (1 << 2)
#تعریف_کړه TRC_CT_INTERNALS   (1 << 3)

بنية trc_feat_def {
  بنية curl_trc_feat *feat;
  بینښه صحيح category;
};

ساكن بنية trc_feat_def trc_feats[] = {
  { &Curl_trc_feat_ids,       TRC_CT_INTERNALS },
  { &Curl_trc_feat_multi,     TRC_CT_شبکه },
  { &Curl_trc_feat_read,      TRC_CT_NONE },
  { &Curl_trc_feat_write,     TRC_CT_NONE },
  { &Curl_trc_feat_dns,       TRC_CT_شبکه },
#که_نه_تعریف_شوی CURL_DISABLE_DOH
  { &Curl_trc_feat_doh,       TRC_CT_شبکه },
#د_که_پای
  { &Curl_trc_feat_timer,     TRC_CT_شبکه },
#که_تعریف_شوی USE_تارونه
  { &Curl_trc_feat_تارونه,   TRC_CT_NONE },
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_FTP
  { &Curl_trc_feat_ftp,       TRC_CT_PROTOCOL },
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_SMTP
  { &Curl_trc_feat_smtp,      TRC_CT_PROTOCOL },
#د_که_پای
#که_تعریف_شوی USE_SSL
  { &Curl_trc_feat_ssls,      TRC_CT_شبکه },
#د_که_پای
#که_تعریف_شوی USE_SSH
  { &Curl_trc_feat_ssh,       TRC_CT_PROTOCOL },
#د_که_پای
#إذا !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
  { &Curl_trc_feat_ws,        TRC_CT_PROTOCOL },
#د_که_پای
};

بنية trc_cft_def {
  بنية Curl_cftype *cft;
  بینښه صحيح category;
};

ساكن بنية trc_cft_def trc_cfts[] = {
  { &Curl_cft_dns,            TRC_CT_شبکه },
  { &Curl_cft_tcp,            TRC_CT_شبکه },
  { &Curl_cft_udp,            TRC_CT_شبکه },
  { &Curl_cft_unix,           TRC_CT_شبکه },
  { &Curl_cft_tcp_accept,     TRC_CT_شبکه },
  { &Curl_cft_ip_happy,       TRC_CT_شبکه },
#که_نه_تعریف_شوی CURL_DISABLE_WEBSOCKETS
  { &Curl_cft_recvbuf,        TRC_CT_PROTOCOL },
#د_که_پای
  { &Curl_cft_تیاری,          TRC_CT_PROTOCOL },
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  { &Curl_cft_nghttp2,        TRC_CT_PROTOCOL },
#د_که_پای
#که_تعریف_شوی USE_SSL
  { &Curl_cft_ssl,            TRC_CT_شبکه },
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  { &Curl_cft_ssl_proxy,      TRC_CT_PROXY },
#د_که_پای
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
#که_نه_تعریف_شوی CURL_DISABLE_HTTP
  { &Curl_cft_h1_proxy,       TRC_CT_PROXY },
#که_تعریف_شوی USE_NGHTTP2
  { &Curl_cft_h2_proxy,       TRC_CT_PROXY },
#د_که_پای
#إذا defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3)
  { &Curl_cft_h3_proxy,       TRC_CT_PROXY },
#د_که_پای
  { &Curl_cft_http_proxy,     TRC_CT_PROXY },
#د_که_پای /* !CURL_DISABLE_HTTP */
  { &Curl_cft_haproxy,        TRC_CT_PROXY },
  { &Curl_cft_socks_proxy,    TRC_CT_PROXY },
#د_که_پای /* !CURL_DISABLE_PROXY */
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { &Curl_cft_http3,          TRC_CT_PROTOCOL },
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_HTTP
  { &Curl_cft_http_connect,   TRC_CT_PROTOCOL },
#د_که_پای
};

ساكن فراغ trc_apply_level_by_name(بنية Curl_str *token, صحيح lvl)
{
  size_t i;

  لكل(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    إذا(curlx_str_casecompare(token, trc_cfts[i].cft->name)) {
      trc_cfts[i].cft->log_level = lvl;
      اكسر;
    }
  }
  لكل(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    إذا(curlx_str_casecompare(token, trc_feats[i].feat->name)) {
      trc_feats[i].feat->log_level = lvl;
      اكسر;
    }
  }
}

ساكن فراغ trc_apply_level_by_category(بینښه صحيح category, صحيح lvl)
{
  size_t i;

  لكل(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    إذا(!category || (trc_cfts[i].category & category))
      trc_cfts[i].cft->log_level = lvl;
  }
  لكل(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    إذا(!category || (trc_feats[i].category & category))
      trc_feats[i].feat->log_level = lvl;
  }
}

ساكن CURLcode trc_opt(ثابت حرف *config)
{
  بنية Curl_str بهر;
  طالما(!curlx_str_until(&config, &بهر, 32, ',')) {
    صحيح lvl = CURL_LOG_LVL_INFO;
    ثابت حرف *token = curlx_str(&بهر);

    إذا(*token == '-') {
      lvl = CURL_LOG_LVL_NONE;
      curlx_str_nudge(&بهر, 1);
    }
    وإلا إذا(*token == '+')
      curlx_str_nudge(&بهر, 1);

    إذا(curlx_str_casecompare(&بهر, "all"))
      trc_apply_level_by_category(TRC_CT_NONE, lvl);
    وإلا إذا(curlx_str_casecompare(&بهر, "protocol"))
      trc_apply_level_by_category(TRC_CT_PROTOCOL, lvl);
    وإلا إذا(curlx_str_casecompare(&بهر, "network"))
      trc_apply_level_by_category(TRC_CT_شبکه, lvl);
    وإلا إذا(curlx_str_casecompare(&بهر, "proxy"))
      trc_apply_level_by_category(TRC_CT_PROXY, lvl);
    وإلا
      trc_apply_level_by_name(&بهر, lvl);

    إذا(curlx_str_single(&config, ','))
      اكسر;
  }
  أرجع CURLE_OK;
}

CURLcode Curl_trc_opt(ثابت حرف *config)
{
  CURLcode پایله = config ? trc_opt(config) : CURLE_OK;
#که_تعریف_شوی DEBUGBUILD
  /* CURL_DEBUG can override anything */
  إذا(!پایله) {
    ثابت حرف *dbg_config = getenv("CURL_DEBUG");
    إذا(dbg_config)
      پایله = trc_opt(dbg_config);
  }
#د_که_پای /* DEBUGBUILD */
  أرجع پایله;
}

CURLcode Curl_trc_init(فراغ)
{
#که_تعریف_شوی DEBUGBUILD
  أرجع Curl_trc_opt(عدم);
#وإلا
  أرجع CURLE_OK;
#د_که_پای
}

#وإلا /* CURL_DISABLE_VERBOSE_STRINGS */

CURLcode Curl_trc_init(فراغ)
{
  أرجع CURLE_OK;
}

فراغ Curl_infof(بنية Curl_easy *معلومات, ثابت حرف *fmt, ...)
{
  (فراغ)معلومات;
  (فراغ)fmt;
}

فراغ Curl_trc_cf_infof(بنية Curl_easy *معلومات, ثابت بنية Curl_cfilter *cf,
                       ثابت حرف *fmt, ...)
{
  (فراغ)معلومات;
  (فراغ)cf;
  (فراغ)fmt;
}

فراغ Curl_trc_multi(بنية Curl_easy *معلومات, ثابت حرف *fmt, ...)
{
  (فراغ)معلومات;
  (فراغ)fmt;
}

فراغ Curl_trc_write(بنية Curl_easy *معلومات, ثابت حرف *fmt, ...)
{
  (فراغ)معلومات;
  (فراغ)fmt;
}

فراغ Curl_trc_dns(بنية Curl_easy *معلومات, ثابت حرف *fmt, ...)
{
  (فراغ)معلومات;
  (فراغ)fmt;
}

فراغ Curl_trc_timer(بنية Curl_easy *معلومات, صحيح tid, ثابت حرف *fmt, ...)
{
  (فراغ)معلومات;
  (فراغ)tid;
  (فراغ)fmt;
}

فراغ Curl_trc_read(بنية Curl_easy *معلومات, ثابت حرف *fmt, ...)
{
  (فراغ)معلومات;
  (فراغ)fmt;
}

#که_نه_تعریف_شوی CURL_DISABLE_FTP
فراغ Curl_trc_ftp(بنية Curl_easy *معلومات, ثابت حرف *fmt, ...)
{
  (فراغ)معلومات;
  (فراغ)fmt;
}
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_SMTP
فراغ Curl_trc_smtp(بنية Curl_easy *معلومات, ثابت حرف *fmt, ...)
{
  (فراغ)معلومات;
  (فراغ)fmt;
}
#د_که_پای
#إذا !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
فراغ Curl_trc_ws(بنية Curl_easy *معلومات, ثابت حرف *fmt, ...)
{
  (فراغ)معلومات;
  (فراغ)fmt;
}
#د_که_پای
#که_تعریف_شوی USE_SSH
فراغ Curl_trc_ssh(بنية Curl_easy *معلومات, ثابت حرف *fmt, ...)
{
  (فراغ)معلومات;
  (فراغ)fmt;
}
#د_که_پای
#که_تعریف_شوی USE_SSL
فراغ Curl_trc_ssls(بنية Curl_easy *معلومات, ثابت حرف *fmt, ...)
{
  (فراغ)معلومات;
  (فراغ)fmt;
}
#د_که_پای

#د_که_پای /* !CURL_DISABLE_VERBOSE_STRINGS */
