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

ساكن فراغ trc_write(بنية Curl_easy *data, curl_infotype type,
                      ثابت حرف *ptr, size_t size)
{
  إذا(data->set.verbose) {
    إذا(data->set.fdebug) {
      بنية Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_fdebug);
      (فراغ)(*data->set.fdebug)(data, type, CURL_UNCONST(ptr), size,
                                data->set.debugdata);
      CURL_CBAPI_END(&guard);
    }
    وإلا {
      ساكن ثابت حرف s_infotype[CURLINFO_END][3] = {
        "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
      بدل(type) {
      حالة CURLINFO_TEXT:
      حالة CURLINFO_HEADER_OUT:
      حالة CURLINFO_HEADER_IN:
        fwrite(s_infotype[type], 2, 1, data->set.err);
        fwrite(ptr, size, 1, data->set.err);
        اكسر;
      افتراضي: /* nada */
        اكسر;
      }
    }
  }
}

/* max length we trace before ending in '...' */
#define TRC_باريس_MAX 2048

#define CURL_TRC_FMT_IDSC   "[x-%" CURL_FORMAT_CURL_OFF_T "] "
#define CURL_TRC_FMT_IDSD   "[%" CURL_FORMAT_CURL_OFF_T "-x] "
#define CURL_TRC_FMT_IDSDC  "[%" CURL_FORMAT_CURL_OFF_T "-%" \
                            CURL_FORMAT_CURL_OFF_T "] "

ساكن بنية curl_trc_feat Curl_trc_feat_ids = {
  "LIB-IDS",
  CURL_LOG_LVL_NONE,
};
#define CURL_TRC_IDS(data) \
  (Curl_trc_is_verbose(data) && \
  Curl_trc_feat_ids.log_level >= CURL_LOG_LVL_INFO)

ساكن size_t trc_print_ids(بنية Curl_easy *data, حرف *buf, size_t maxlen)
{
  curl_off_t cid = data->state.last_conn_id;
  إذا(data->id >= 0) {
    إذا(cid >= 0)
      أرجع curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSDC, data->id, cid);
    وإلا
      أرجع curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSD, data->id);
  }
  وإلا إذا(cid >= 0)
    أرجع curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSC, cid);
  وإلا {
    أرجع curl_msnprintf(buf, maxlen, "[x-x] ");
  }
}

ساكن size_t trc_end_buf(حرف *buf, size_t len, size_t maxlen, bool addnl)
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

فراغ Curl_debug(بنية Curl_easy *data, curl_infotype type,
                ثابت حرف *ptr, size_t size)
{
  إذا(data->set.verbose) {
    ساكن ثابت حرف s_infotype[CURLINFO_END][3] = {
      "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
    حرف buf[TRC_باريس_MAX];
    size_t len;
    إذا(data->set.fdebug) {
      بنية Curl_mapi_guard guard;
      إذا(CURL_TRC_IDS(data) && (size < TRC_باريس_MAX)) {
        len = trc_print_ids(data, buf, TRC_باريس_MAX);
        len += curl_msnprintf(buf + len, TRC_باريس_MAX - len, "%.*s",
                              (صحيح)size, ptr);
        len = trc_end_buf(buf, len, TRC_باريس_MAX, FALSE);
        CURL_CBAPI_START(&guard, data, easy_fdebug);
        (فراغ)(*data->set.fdebug)(data, type, buf, len, data->set.debugdata);
        CURL_CBAPI_END(&guard);
      }
      وإلا {
        CURL_CBAPI_START(&guard, data, easy_fdebug);
        (فراغ)(*data->set.fdebug)(data, type, CURL_UNCONST(ptr),
                                  size, data->set.debugdata);
        CURL_CBAPI_END(&guard);
      }
    }
    وإلا {
      بدل(type) {
      حالة CURLINFO_TEXT:
      حالة CURLINFO_HEADER_OUT:
      حالة CURLINFO_HEADER_IN:
#ifndef CURL_DISABLE_VERBOSE_STRINGS
        إذا(CURL_TRC_IDS(data)) {
          len = trc_print_ids(data, buf, TRC_باريس_MAX);
          fwrite(buf, len, 1, data->set.err);
        }
#endif
        fwrite(s_infotype[type], 2, 1, data->set.err);
        fwrite(ptr, size, 1, data->set.err);
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
فراغ Curl_failf(بنية Curl_easy *data, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  إذا(data->set.verbose || data->set.errorbuffer) {
    va_list ap;
    size_t len;
    حرف error[CURL_ERROR_SIZE + 2];
    va_start(ap, fmt);
    len = curl_mvsnprintf(error, CURL_ERROR_SIZE, fmt, ap);

    إذا(data->set.errorbuffer && !data->state.errorbuf) {
      curlx_strcopy(data->set.errorbuffer, CURL_ERROR_SIZE, error, len);
      data->state.errorbuf = TRUE; /* wrote error string */
    }
    error[len++] = '\n';
    error[len] = '\0';
    trc_write(data, CURLINFO_TEXT, error, len);
    va_end(ap);
  }
}

فراغ Curl_reset_fail(بنية Curl_easy *data)
{
  إذا(data->set.errorbuffer)
    data->set.errorbuffer[0] = 0;
  data->state.errorbuf = FALSE;
}

#ifdef CURLVERBOSE
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
#ifndef CURL_DISABLE_DOH
بنية curl_trc_feat Curl_trc_feat_doh = {
  "DoH",
  CURL_LOG_LVL_NONE,
};
#endif
بنية curl_trc_feat Curl_trc_feat_timer = {
  "TIMER",
  CURL_LOG_LVL_NONE,
};
#ifdef USE_THREADS
بنية curl_trc_feat Curl_trc_feat_threads = {
  "THREADS",
  CURL_LOG_LVL_NONE,
};
#endif
#endif

#ifndef CURL_DISABLE_VERBOSE_STRINGS

ساكن فراغ trc_infof(بنية Curl_easy *data,
                      بنية curl_trc_feat *feat,
                      ثابت حرف *opt_id, صحيح opt_id_idx,
                      ثابت حرف * ثابت fmt, va_list ap) CURL_PRINTF(5, 0);

ساكن فراغ trc_infof(بنية Curl_easy *data,
                      بنية curl_trc_feat *feat,
                      ثابت حرف *opt_id, صحيح opt_id_idx,
                      ثابت حرف * ثابت fmt, va_list ap)
{
  size_t len = 0;
  حرف buf[TRC_باريس_MAX];

  إذا(CURL_TRC_IDS(data))
    len += trc_print_ids(data, buf + len, TRC_باريس_MAX - len);
  إذا(feat)
    len += curl_msnprintf(buf + len, TRC_باريس_MAX - len, "[%s] ", feat->name);
  إذا(opt_id) {
    إذا(opt_id_idx > 0)
      len += curl_msnprintf(buf + len, TRC_باريس_MAX - len, "[%s-%d] ",
                            opt_id, opt_id_idx);
    وإلا
      len += curl_msnprintf(buf + len, TRC_باريس_MAX - len, "[%s] ", opt_id);
  }
  len += curl_mvsnprintf(buf + len, TRC_باريس_MAX - len, fmt, ap);
  len = trc_end_buf(buf, len, TRC_باريس_MAX, TRUE);
  trc_write(data, CURLINFO_TEXT, buf, len);
}

فراغ Curl_infof(بنية Curl_easy *data, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  إذا(Curl_trc_is_verbose(data)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, data->state.feat, عدم, 0, fmt, ap);
    va_end(ap);
  }
}

فراغ Curl_trc_cf_infof(بنية Curl_easy *data, ثابت بنية Curl_cfilter *cf,
                       ثابت حرف *fmt, ...)
{
  DEBUGASSERT(cf);
  إذا(Curl_trc_cf_is_verbose(cf, data)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, data->state.feat, cf->cft->name, cf->sockindex, fmt, ap);
    va_end(ap);
  }
}

فراغ Curl_trc_feat_infof(بنية Curl_easy *data,
                         بنية curl_trc_feat *feat,
                         ثابت حرف *fmt, ...)
{
  DEBUGASSERT(feat);
  إذا(Curl_trc_ft_is_verbose(data, feat)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, feat, عدم, 0, fmt, ap);
    va_end(ap);
  }
}

ساكن ثابت حرف * ثابت Curl_trc_timer_names[] = {
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
  إذا((tid >= 0) && ((size_t)tid < CURL_ARRAYSIZE(Curl_trc_timer_names)))
    أرجع Curl_trc_timer_names[(size_t)tid];
  أرجع "TIMER-???";
}

فراغ Curl_trc_timer(بنية Curl_easy *data, صحيح tid, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  إذا(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_timer)) {
    ثابت حرف *tname = trc_timer_name(tid);
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_timer, tname, 0, fmt, ap);
    va_end(ap);
  }
}

فراغ Curl_trc_easy_timers(بنية Curl_easy *data)
{
  إذا(CURL_TRC_TIMER_is_verbose(data) && data->multi) {
    إذا(data->state.timeouts.first < EXPIRE_LAST) {
      بنية expire_timers *timeouts = &data->state.timeouts;
      timediff_t base_us =
        Curl_timeouts_offset_us(&data->multi->timeouts, Curl_pgrs_now(data));
      uint8_t id = data->state.timeouts.first;
      لكل(; id < EXPIRE_LAST; id = timeouts->next[id]) {
        CURL_TRC_TIMER(data, id, "expires in %" FMT_TIMEDIFF_T "us",
                       timeouts->offset_us[id] - base_us);
      }
    }
  }
}

ساكن ثابت حرف * ثابت Curl_trc_mstate_names[] = {
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
  إذا((state >= 0) && ((size_t)state < CURL_ARRAYSIZE(Curl_trc_mstate_names)))
    أرجع Curl_trc_mstate_names[(size_t)state];
  أرجع "?";
}

فراغ Curl_trc_multi(بنية Curl_easy *data, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  إذا(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_multi)) {
    ثابت حرف *sname = (data->id >= 0) ?
                        Curl_trc_mstate_name(data->mstate) : عدم;
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_multi, sname, 0, fmt, ap);
    va_end(ap);
  }
}

فراغ Curl_trc_read(بنية Curl_easy *data, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  إذا(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_read)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_read, عدم, 0, fmt, ap);
    va_end(ap);
  }
}

فراغ Curl_trc_write(بنية Curl_easy *data, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  إذا(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_write)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_write, عدم, 0, fmt, ap);
    va_end(ap);
  }
}

فراغ Curl_trc_dns(بنية Curl_easy *data, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  إذا(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_dns)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_dns, عدم, 0, fmt, ap);
    va_end(ap);
  }
}

#ifndef CURL_DISABLE_FTP
بنية curl_trc_feat Curl_trc_feat_ftp = {
  "FTP",
  CURL_LOG_LVL_NONE,
};

فراغ Curl_trc_ftp(بنية Curl_easy *data, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  إذا(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ftp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ftp, عدم, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* !CURL_DISABLE_FTP */

#ifndef CURL_DISABLE_SMTP
بنية curl_trc_feat Curl_trc_feat_smtp = {
  "SMTP",
  CURL_LOG_LVL_NONE,
};

فراغ Curl_trc_smtp(بنية Curl_easy *data, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  إذا(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_smtp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_smtp, عدم, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* !CURL_DISABLE_SMTP */

#ifdef USE_SSL
بنية curl_trc_feat Curl_trc_feat_ssls = {
  "SSLS",
  CURL_LOG_LVL_NONE,
};

فراغ Curl_trc_ssls(بنية Curl_easy *data, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  إذا(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssls)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ssls, عدم, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* USE_SSL */

#ifdef USE_SSH
بنية curl_trc_feat Curl_trc_feat_ssh = {
  "SSH",
  CURL_LOG_LVL_NONE,
};

فراغ Curl_trc_ssh(بنية Curl_easy *data, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  إذا(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssh)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ssh, عدم, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* USE_SSH */

#إذا !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
بنية curl_trc_feat Curl_trc_feat_ws = {
  "WS",
  CURL_LOG_LVL_NONE,
};

فراغ Curl_trc_ws(بنية Curl_easy *data, ثابت حرف *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  إذا(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ws)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ws, عدم, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#define TRC_CT_NONE        0
#define TRC_CT_PROTOCOL    (1 << 0)
#define TRC_CT_NETWORK     (1 << 1)
#define TRC_CT_PROXY       (1 << 2)
#define TRC_CT_INTERNALS   (1 << 3)

بنية trc_feat_def {
  بنية curl_trc_feat *feat;
  unsigned صحيح category;
};

ساكن بنية trc_feat_def trc_feats[] = {
  { &Curl_trc_feat_ids,       TRC_CT_INTERNALS },
  { &Curl_trc_feat_multi,     TRC_CT_NETWORK },
  { &Curl_trc_feat_read,      TRC_CT_NONE },
  { &Curl_trc_feat_write,     TRC_CT_NONE },
  { &Curl_trc_feat_dns,       TRC_CT_NETWORK },
#ifndef CURL_DISABLE_DOH
  { &Curl_trc_feat_doh,       TRC_CT_NETWORK },
#endif
  { &Curl_trc_feat_timer,     TRC_CT_NETWORK },
#ifdef USE_THREADS
  { &Curl_trc_feat_threads,   TRC_CT_NONE },
#endif
#ifndef CURL_DISABLE_FTP
  { &Curl_trc_feat_ftp,       TRC_CT_PROTOCOL },
#endif
#ifndef CURL_DISABLE_SMTP
  { &Curl_trc_feat_smtp,      TRC_CT_PROTOCOL },
#endif
#ifdef USE_SSL
  { &Curl_trc_feat_ssls,      TRC_CT_NETWORK },
#endif
#ifdef USE_SSH
  { &Curl_trc_feat_ssh,       TRC_CT_PROTOCOL },
#endif
#إذا !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
  { &Curl_trc_feat_ws,        TRC_CT_PROTOCOL },
#endif
};

بنية trc_cft_def {
  بنية Curl_cftype *cft;
  unsigned صحيح category;
};

ساكن بنية trc_cft_def trc_cfts[] = {
  { &Curl_cft_dns,            TRC_CT_NETWORK },
  { &Curl_cft_tcp,            TRC_CT_NETWORK },
  { &Curl_cft_udp,            TRC_CT_NETWORK },
  { &Curl_cft_unix,           TRC_CT_NETWORK },
  { &Curl_cft_tcp_accept,     TRC_CT_NETWORK },
  { &Curl_cft_ip_happy,       TRC_CT_NETWORK },
#ifndef CURL_DISABLE_WEBSOCKETS
  { &Curl_cft_recvbuf,        TRC_CT_PROTOCOL },
#endif
  { &Curl_cft_setup,          TRC_CT_PROTOCOL },
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  { &Curl_cft_nghttp2,        TRC_CT_PROTOCOL },
#endif
#ifdef USE_SSL
  { &Curl_cft_ssl,            TRC_CT_NETWORK },
#ifndef CURL_DISABLE_PROXY
  { &Curl_cft_ssl_proxy,      TRC_CT_PROXY },
#endif
#endif
#ifndef CURL_DISABLE_PROXY
#ifndef CURL_DISABLE_HTTP
  { &Curl_cft_h1_proxy,       TRC_CT_PROXY },
#ifdef USE_NGHTTP2
  { &Curl_cft_h2_proxy,       TRC_CT_PROXY },
#endif
#إذا defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3)
  { &Curl_cft_h3_proxy,       TRC_CT_PROXY },
#endif
  { &Curl_cft_http_proxy,     TRC_CT_PROXY },
#endif /* !CURL_DISABLE_HTTP */
  { &Curl_cft_haproxy,        TRC_CT_PROXY },
  { &Curl_cft_socks_proxy,    TRC_CT_PROXY },
#endif /* !CURL_DISABLE_PROXY */
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { &Curl_cft_http3,          TRC_CT_PROTOCOL },
#endif
#ifndef CURL_DISABLE_HTTP
  { &Curl_cft_http_connect,   TRC_CT_PROTOCOL },
#endif
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

ساكن فراغ trc_apply_level_by_category(unsigned صحيح category, صحيح lvl)
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
  بنية Curl_str out;
  طالما(!curlx_str_until(&config, &out, 32, ',')) {
    صحيح lvl = CURL_LOG_LVL_INFO;
    ثابت حرف *token = curlx_str(&out);

    إذا(*token == '-') {
      lvl = CURL_LOG_LVL_NONE;
      curlx_str_nudge(&out, 1);
    }
    وإلا إذا(*token == '+')
      curlx_str_nudge(&out, 1);

    إذا(curlx_str_casecompare(&out, "all"))
      trc_apply_level_by_category(TRC_CT_NONE, lvl);
    وإلا إذا(curlx_str_casecompare(&out, "protocol"))
      trc_apply_level_by_category(TRC_CT_PROTOCOL, lvl);
    وإلا إذا(curlx_str_casecompare(&out, "network"))
      trc_apply_level_by_category(TRC_CT_NETWORK, lvl);
    وإلا إذا(curlx_str_casecompare(&out, "proxy"))
      trc_apply_level_by_category(TRC_CT_PROXY, lvl);
    وإلا
      trc_apply_level_by_name(&out, lvl);

    إذا(curlx_str_single(&config, ','))
      اكسر;
  }
  أرجع CURLE_OK;
}

CURLcode Curl_trc_opt(ثابت حرف *config)
{
  CURLcode result = config ? trc_opt(config) : CURLE_OK;
#ifdef DEBUGBUILD
  /* CURL_DEBUG can override anything */
  إذا(!result) {
    ثابت حرف *dbg_config = getenv("CURL_DEBUG");
    إذا(dbg_config)
      result = trc_opt(dbg_config);
  }
#endif /* DEBUGBUILD */
  أرجع result;
}

CURLcode Curl_trc_init(فراغ)
{
#ifdef DEBUGBUILD
  أرجع Curl_trc_opt(عدم);
#وإلا
  أرجع CURLE_OK;
#endif
}

#وإلا /* CURL_DISABLE_VERBOSE_STRINGS */

CURLcode Curl_trc_init(فراغ)
{
  أرجع CURLE_OK;
}

فراغ Curl_infof(بنية Curl_easy *data, ثابت حرف *fmt, ...)
{
  (فراغ)data;
  (فراغ)fmt;
}

فراغ Curl_trc_cf_infof(بنية Curl_easy *data, ثابت بنية Curl_cfilter *cf,
                       ثابت حرف *fmt, ...)
{
  (فراغ)data;
  (فراغ)cf;
  (فراغ)fmt;
}

فراغ Curl_trc_multi(بنية Curl_easy *data, ثابت حرف *fmt, ...)
{
  (فراغ)data;
  (فراغ)fmt;
}

فراغ Curl_trc_write(بنية Curl_easy *data, ثابت حرف *fmt, ...)
{
  (فراغ)data;
  (فراغ)fmt;
}

فراغ Curl_trc_dns(بنية Curl_easy *data, ثابت حرف *fmt, ...)
{
  (فراغ)data;
  (فراغ)fmt;
}

فراغ Curl_trc_timer(بنية Curl_easy *data, صحيح tid, ثابت حرف *fmt, ...)
{
  (فراغ)data;
  (فراغ)tid;
  (فراغ)fmt;
}

فراغ Curl_trc_read(بنية Curl_easy *data, ثابت حرف *fmt, ...)
{
  (فراغ)data;
  (فراغ)fmt;
}

#ifndef CURL_DISABLE_FTP
فراغ Curl_trc_ftp(بنية Curl_easy *data, ثابت حرف *fmt, ...)
{
  (فراغ)data;
  (فراغ)fmt;
}
#endif
#ifndef CURL_DISABLE_SMTP
فراغ Curl_trc_smtp(بنية Curl_easy *data, ثابت حرف *fmt, ...)
{
  (فراغ)data;
  (فراغ)fmt;
}
#endif
#إذا !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
فراغ Curl_trc_ws(بنية Curl_easy *data, ثابت حرف *fmt, ...)
{
  (فراغ)data;
  (فراغ)fmt;
}
#endif
#ifdef USE_SSH
فراغ Curl_trc_ssh(بنية Curl_easy *data, ثابت حرف *fmt, ...)
{
  (فراغ)data;
  (فراغ)fmt;
}
#endif
#ifdef USE_SSL
فراغ Curl_trc_ssls(بنية Curl_easy *data, ثابت حرف *fmt, ...)
{
  (فراغ)data;
  (فراغ)fmt;
}
#endif

#endif /* !CURL_DISABLE_VERBOSE_STRINGS */
