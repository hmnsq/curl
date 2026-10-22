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

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "cf-recvbuf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-socket.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-setup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http2.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http_proxy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-h1-proxy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-h2-proxy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-haproxy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-https-connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-ip-happy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "socks.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/cf-dns.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/vquic.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strcopy.h"

ਸਥਿਰ ਖਾਲੀ trc_write(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, curl_infotype type,
                      ਅਚਲ ਅੱਖਰ *ptr, size_t ਮਾਪ)
{
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.verbose) {
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.fdebug) {
      ਬਣਤਰ Curl_mapi_guard guard;
      CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_fdebug);
      (ਖਾਲੀ)(*ਅੰਕੜੇ->ਸੈੱਟ.fdebug)(ਅੰਕੜੇ, type, CURL_UNCONST(ptr), ਮਾਪ,
                                ਅੰਕੜੇ->ਸੈੱਟ.debugdata);
      CURL_CBAPI_ਸਿਰਾ(&guard);
    }
    ਨਹੀਂਤਾਂ {
      ਸਥਿਰ ਅਚਲ ਅੱਖਰ s_infotype[CURLINFO_ਸਿਰਾ][3] = {
        "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
      ਚੋਣ(type) {
      ਹਾਲਤ CURLINFO_ਪਾਠ:
      ਹਾਲਤ CURLINFO_ਸਿਰਲੇਖ_ਬਾਹਰਲਾ:
      ਹਾਲਤ CURLINFO_ਸਿਰਲੇਖ_ਵਿੱਚ:
        fwrite(s_infotype[type], 2, 1, ਅੰਕੜੇ->ਸੈੱਟ.err);
        fwrite(ptr, ਮਾਪ, 1, ਅੰਕੜੇ->ਸੈੱਟ.err);
        ਰੋਕੋ;
      ਮੂਲ: /* nada */
        ਰੋਕੋ;
      }
    }
  }
}

/* max length we trace before ending in '...' */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TRC_ਸਤਰ_MAX 2048

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_FMT_IDSC   "[x-%" CURL_FORMAT_CURL_OFF_T "] "
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_FMT_IDSD   "[%" CURL_FORMAT_CURL_OFF_T "-x] "
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_FMT_IDSDC  "[%" CURL_FORMAT_CURL_OFF_T "-%" \
                            CURL_FORMAT_CURL_OFF_T "] "

ਸਥਿਰ ਬਣਤਰ curl_trc_feat Curl_trc_feat_ids = {
  "LIB-IDS",
  CURL_LOG_LVL_NONE,
};
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_IDS(ਅੰਕੜੇ) \
  (Curl_trc_is_verbose(ਅੰਕੜੇ) && \
  Curl_trc_feat_ids.log_level >= CURL_LOG_LVL_INFO)

ਸਥਿਰ size_t trc_print_ids(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅੱਖਰ *buf, size_t maxlen)
{
  curl_off_t cid = ਅੰਕੜੇ->state.ਆਖਰੀ_conn_id;
  ਜੇ(ਅੰਕੜੇ->id >= 0) {
    ਜੇ(cid >= 0)
      ਵਾਪਸ curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSDC, ਅੰਕੜੇ->id, cid);
    ਨਹੀਂਤਾਂ
      ਵਾਪਸ curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSD, ਅੰਕੜੇ->id);
  }
  ਨਹੀਂਤਾਂ ਜੇ(cid >= 0)
    ਵਾਪਸ curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSC, cid);
  ਨਹੀਂਤਾਂ {
    ਵਾਪਸ curl_msnprintf(buf, maxlen, "[x-x] ");
  }
}

ਸਥਿਰ size_t trc_ਸਿਰਾ_buf(ਅੱਖਰ *buf, size_t len, size_t maxlen, bool addnl)
{
  /* make sure we end the trace line in `buf` properly. It needs
   * to end with a null-terminator '\0' or '\n\0' */
  ਜੇ(len >= (maxlen - (addnl ? 2 : 1))) {
    len = maxlen - 5;
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '\n';
  }
  ਨਹੀਂਤਾਂ ਜੇ(addnl)
    buf[len++] = '\n';
  buf[len] = '\0';
  ਵਾਪਸ len;
}

ਖਾਲੀ Curl_debug(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, curl_infotype type,
                ਅਚਲ ਅੱਖਰ *ptr, size_t ਮਾਪ)
{
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.verbose) {
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ s_infotype[CURLINFO_ਸਿਰਾ][3] = {
      "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
    ਅੱਖਰ buf[TRC_ਸਤਰ_MAX];
    size_t len;
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.fdebug) {
      ਬਣਤਰ Curl_mapi_guard guard;
      ਜੇ(CURL_TRC_IDS(ਅੰਕੜੇ) && (ਮਾਪ < TRC_ਸਤਰ_MAX)) {
        len = trc_print_ids(ਅੰਕੜੇ, buf, TRC_ਸਤਰ_MAX);
        len += curl_msnprintf(buf + len, TRC_ਸਤਰ_MAX - len, "%.*s",
                              (ਪੂਰਨ)ਮਾਪ, ptr);
        len = trc_ਸਿਰਾ_buf(buf, len, TRC_ਸਤਰ_MAX, FALSE);
        CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_fdebug);
        (ਖਾਲੀ)(*ਅੰਕੜੇ->ਸੈੱਟ.fdebug)(ਅੰਕੜੇ, type, buf, len, ਅੰਕੜੇ->ਸੈੱਟ.debugdata);
        CURL_CBAPI_ਸਿਰਾ(&guard);
      }
      ਨਹੀਂਤਾਂ {
        CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_fdebug);
        (ਖਾਲੀ)(*ਅੰਕੜੇ->ਸੈੱਟ.fdebug)(ਅੰਕੜੇ, type, CURL_UNCONST(ptr),
                                  ਮਾਪ, ਅੰਕੜੇ->ਸੈੱਟ.debugdata);
        CURL_CBAPI_ਸਿਰਾ(&guard);
      }
    }
    ਨਹੀਂਤਾਂ {
      ਚੋਣ(type) {
      ਹਾਲਤ CURLINFO_ਪਾਠ:
      ਹਾਲਤ CURLINFO_ਸਿਰਲੇਖ_ਬਾਹਰਲਾ:
      ਹਾਲਤ CURLINFO_ਸਿਰਲੇਖ_ਵਿੱਚ:
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_VERBOSE_ਲੜੀਆਂ
        ਜੇ(CURL_TRC_IDS(ਅੰਕੜੇ)) {
          len = trc_print_ids(ਅੰਕੜੇ, buf, TRC_ਸਤਰ_MAX);
          fwrite(buf, len, 1, ਅੰਕੜੇ->ਸੈੱਟ.err);
        }
#ਜੇ_ਅੰਤ
        fwrite(s_infotype[type], 2, 1, ਅੰਕੜੇ->ਸੈੱਟ.err);
        fwrite(ptr, ਮਾਪ, 1, ਅੰਕੜੇ->ਸੈੱਟ.err);
        ਰੋਕੋ;
      ਮੂਲ: /* nada */
        ਰੋਕੋ;
      }
    }
  }
}

/* Curl_failf() is for messages stating why we failed.
 * The message SHALL NOT include any LF or CR.
 */
ਖਾਲੀ Curl_failf(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.verbose || ਅੰਕੜੇ->ਸੈੱਟ.errorbuffer) {
    va_list ap;
    size_t len;
    ਅੱਖਰ error[CURL_ERROR_ਮਾਪ + 2];
    va_start(ap, fmt);
    len = curl_mvsnprintf(error, CURL_ERROR_ਮਾਪ, fmt, ap);

    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.errorbuffer && !ਅੰਕੜੇ->state.errorbuf) {
      curlx_strcopy(ਅੰਕੜੇ->ਸੈੱਟ.errorbuffer, CURL_ERROR_ਮਾਪ, error, len);
      ਅੰਕੜੇ->state.errorbuf = TRUE; /* wrote error string */
    }
    error[len++] = '\n';
    error[len] = '\0';
    trc_write(ਅੰਕੜੇ, CURLINFO_ਪਾਠ, error, len);
    va_end(ap);
  }
}

ਖਾਲੀ Curl_reset_fail(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.errorbuffer)
    ਅੰਕੜੇ->ਸੈੱਟ.errorbuffer[0] = 0;
  ਅੰਕੜੇ->state.errorbuf = FALSE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਬਣਤਰ curl_trc_feat Curl_trc_feat_multi = {
  "MULTI",
  CURL_LOG_LVL_NONE,
};
ਬਣਤਰ curl_trc_feat Curl_trc_feat_read = {
  "READ",
  CURL_LOG_LVL_NONE,
};
ਬਣਤਰ curl_trc_feat Curl_trc_feat_write = {
  "WRITE",
  CURL_LOG_LVL_NONE,
};
ਬਣਤਰ curl_trc_feat Curl_trc_feat_dns = {
  "DNS",
  CURL_LOG_LVL_NONE,
};
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DOH
ਬਣਤਰ curl_trc_feat Curl_trc_feat_doh = {
  "DoH",
  CURL_LOG_LVL_NONE,
};
#ਜੇ_ਅੰਤ
ਬਣਤਰ curl_trc_feat Curl_trc_feat_timer = {
  "TIMER",
  CURL_LOG_LVL_NONE,
};
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ਧਾਗੇ
ਬਣਤਰ curl_trc_feat Curl_trc_feat_ਧਾਗੇ = {
  "THREADS",
  CURL_LOG_LVL_NONE,
};
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_VERBOSE_ਲੜੀਆਂ

ਸਥਿਰ ਖਾਲੀ trc_infof(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                      ਬਣਤਰ curl_trc_feat *feat,
                      ਅਚਲ ਅੱਖਰ *opt_id, ਪੂਰਨ opt_id_idx,
                      ਅਚਲ ਅੱਖਰ * ਅਚਲ fmt, va_list ap) CURL_PRINTF(5, 0);

ਸਥਿਰ ਖਾਲੀ trc_infof(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                      ਬਣਤਰ curl_trc_feat *feat,
                      ਅਚਲ ਅੱਖਰ *opt_id, ਪੂਰਨ opt_id_idx,
                      ਅਚਲ ਅੱਖਰ * ਅਚਲ fmt, va_list ap)
{
  size_t len = 0;
  ਅੱਖਰ buf[TRC_ਸਤਰ_MAX];

  ਜੇ(CURL_TRC_IDS(ਅੰਕੜੇ))
    len += trc_print_ids(ਅੰਕੜੇ, buf + len, TRC_ਸਤਰ_MAX - len);
  ਜੇ(feat)
    len += curl_msnprintf(buf + len, TRC_ਸਤਰ_MAX - len, "[%s] ", feat->name);
  ਜੇ(opt_id) {
    ਜੇ(opt_id_idx > 0)
      len += curl_msnprintf(buf + len, TRC_ਸਤਰ_MAX - len, "[%s-%d] ",
                            opt_id, opt_id_idx);
    ਨਹੀਂਤਾਂ
      len += curl_msnprintf(buf + len, TRC_ਸਤਰ_MAX - len, "[%s] ", opt_id);
  }
  len += curl_mvsnprintf(buf + len, TRC_ਸਤਰ_MAX - len, fmt, ap);
  len = trc_ਸਿਰਾ_buf(buf, len, TRC_ਸਤਰ_MAX, TRUE);
  trc_write(ਅੰਕੜੇ, CURLINFO_ਪਾਠ, buf, len);
}

ਖਾਲੀ Curl_infof(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ਜੇ(Curl_trc_is_verbose(ਅੰਕੜੇ)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ਅੰਕੜੇ, ਅੰਕੜੇ->state.feat, ਨਿਰਾ, 0, fmt, ap);
    va_end(ap);
  }
}

ਖਾਲੀ Curl_trc_cf_infof(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਬਣਤਰ Curl_cfilter *cf,
                       ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  DEBUGASSERT(cf);
  ਜੇ(Curl_trc_cf_is_verbose(cf, ਅੰਕੜੇ)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ਅੰਕੜੇ, ਅੰਕੜੇ->state.feat, cf->cft->name, cf->sockindex, fmt, ap);
    va_end(ap);
  }
}

ਖਾਲੀ Curl_trc_feat_infof(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         ਬਣਤਰ curl_trc_feat *feat,
                         ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  DEBUGASSERT(feat);
  ਜੇ(Curl_trc_ft_is_verbose(ਅੰਕੜੇ, feat)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ਅੰਕੜੇ, feat, ਨਿਰਾ, 0, fmt, ap);
    va_end(ap);
  }
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ Curl_trc_timer_ਨਾਂ[] = {
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

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *trc_timer_name(ਪੂਰਨ tid)
{
  ਜੇ((tid >= 0) && ((size_t)tid < CURL_ARRAYSIZE(Curl_trc_timer_ਨਾਂ)))
    ਵਾਪਸ Curl_trc_timer_ਨਾਂ[(size_t)tid];
  ਵਾਪਸ "TIMER-???";
}

ਖਾਲੀ Curl_trc_timer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਪੂਰਨ tid, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ਜੇ(Curl_trc_ft_is_verbose(ਅੰਕੜੇ, &Curl_trc_feat_timer)) {
    ਅਚਲ ਅੱਖਰ *tname = trc_timer_name(tid);
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ਅੰਕੜੇ, &Curl_trc_feat_timer, tname, 0, fmt, ap);
    va_end(ap);
  }
}

ਖਾਲੀ Curl_trc_easy_timers(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(CURL_TRC_TIMER_is_verbose(ਅੰਕੜੇ) && ਅੰਕੜੇ->multi) {
    ਜੇ(ਅੰਕੜੇ->state.timeouts.ਪਹਿਲਾ < EXPIRE_ਆਖਰੀ) {
      ਬਣਤਰ expire_timers *timeouts = &ਅੰਕੜੇ->state.timeouts;
      timediff_t ਅਧਾਰ_us =
        Curl_timeouts_ਵਿੱਥ_us(&ਅੰਕੜੇ->multi->timeouts, Curl_pgrs_now(ਅੰਕੜੇ));
      uint8_t id = ਅੰਕੜੇ->state.timeouts.ਪਹਿਲਾ;
      ਲਈ(; id < EXPIRE_ਆਖਰੀ; id = timeouts->ਅਗਲਾ[id]) {
        CURL_TRC_TIMER(ਅੰਕੜੇ, id, "expires in %" FMT_TIMEDIFF_T "us",
                       timeouts->ਵਿੱਥ_us[id] - ਅਧਾਰ_us);
      }
    }
  }
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ Curl_trc_mstate_ਨਾਂ[] = {
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

ਅਚਲ ਅੱਖਰ *Curl_trc_mstate_name(ਪੂਰਨ state)
{
  ਜੇ((state >= 0) && ((size_t)state < CURL_ARRAYSIZE(Curl_trc_mstate_ਨਾਂ)))
    ਵਾਪਸ Curl_trc_mstate_ਨਾਂ[(size_t)state];
  ਵਾਪਸ "?";
}

ਖਾਲੀ Curl_trc_multi(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ਜੇ(Curl_trc_ft_is_verbose(ਅੰਕੜੇ, &Curl_trc_feat_multi)) {
    ਅਚਲ ਅੱਖਰ *sname = (ਅੰਕੜੇ->id >= 0) ?
                        Curl_trc_mstate_name(ਅੰਕੜੇ->mstate) : ਨਿਰਾ;
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ਅੰਕੜੇ, &Curl_trc_feat_multi, sname, 0, fmt, ap);
    va_end(ap);
  }
}

ਖਾਲੀ Curl_trc_read(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ਜੇ(Curl_trc_ft_is_verbose(ਅੰਕੜੇ, &Curl_trc_feat_read)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ਅੰਕੜੇ, &Curl_trc_feat_read, ਨਿਰਾ, 0, fmt, ap);
    va_end(ap);
  }
}

ਖਾਲੀ Curl_trc_write(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ਜੇ(Curl_trc_ft_is_verbose(ਅੰਕੜੇ, &Curl_trc_feat_write)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ਅੰਕੜੇ, &Curl_trc_feat_write, ਨਿਰਾ, 0, fmt, ap);
    va_end(ap);
  }
}

ਖਾਲੀ Curl_trc_dns(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ਜੇ(Curl_trc_ft_is_verbose(ਅੰਕੜੇ, &Curl_trc_feat_dns)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ਅੰਕੜੇ, &Curl_trc_feat_dns, ਨਿਰਾ, 0, fmt, ap);
    va_end(ap);
  }
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
ਬਣਤਰ curl_trc_feat Curl_trc_feat_ftp = {
  "FTP",
  CURL_LOG_LVL_NONE,
};

ਖਾਲੀ Curl_trc_ftp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ਜੇ(Curl_trc_ft_is_verbose(ਅੰਕੜੇ, &Curl_trc_feat_ftp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ਅੰਕੜੇ, &Curl_trc_feat_ftp, ਨਿਰਾ, 0, fmt, ap);
    va_end(ap);
  }
}
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_FTP */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_SMTP
ਬਣਤਰ curl_trc_feat Curl_trc_feat_smtp = {
  "SMTP",
  CURL_LOG_LVL_NONE,
};

ਖਾਲੀ Curl_trc_smtp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ਜੇ(Curl_trc_ft_is_verbose(ਅੰਕੜੇ, &Curl_trc_feat_smtp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ਅੰਕੜੇ, &Curl_trc_feat_smtp, ਨਿਰਾ, 0, fmt, ap);
    va_end(ap);
  }
}
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_SMTP */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
ਬਣਤਰ curl_trc_feat Curl_trc_feat_ssls = {
  "SSLS",
  CURL_LOG_LVL_NONE,
};

ਖਾਲੀ Curl_trc_ssls(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ਜੇ(Curl_trc_ft_is_verbose(ਅੰਕੜੇ, &Curl_trc_feat_ssls)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ਅੰਕੜੇ, &Curl_trc_feat_ssls, ਨਿਰਾ, 0, fmt, ap);
    va_end(ap);
  }
}
#ਜੇ_ਅੰਤ /* USE_SSL */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSH
ਬਣਤਰ curl_trc_feat Curl_trc_feat_ssh = {
  "SSH",
  CURL_LOG_LVL_NONE,
};

ਖਾਲੀ Curl_trc_ssh(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ਜੇ(Curl_trc_ft_is_verbose(ਅੰਕੜੇ, &Curl_trc_feat_ssh)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ਅੰਕੜੇ, &Curl_trc_feat_ssh, ਨਿਰਾ, 0, fmt, ap);
    va_end(ap);
  }
}
#ਜੇ_ਅੰਤ /* USE_SSH */

#ਜੇ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
ਬਣਤਰ curl_trc_feat Curl_trc_feat_ws = {
  "WS",
  CURL_LOG_LVL_NONE,
};

ਖਾਲੀ Curl_trc_ws(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ਜੇ(Curl_trc_ft_is_verbose(ਅੰਕੜੇ, &Curl_trc_feat_ws)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ਅੰਕੜੇ, &Curl_trc_feat_ws, ਨਿਰਾ, 0, fmt, ap);
    va_end(ap);
  }
}
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TRC_CT_NONE        0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TRC_CT_PROTOCOL    (1 << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TRC_CT_ਜਾਲ     (1 << 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TRC_CT_PROXY       (1 << 2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TRC_CT_INTERNALS   (1 << 3)

ਬਣਤਰ trc_feat_def {
  ਬਣਤਰ curl_trc_feat *feat;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ category;
};

ਸਥਿਰ ਬਣਤਰ trc_feat_def trc_feats[] = {
  { &Curl_trc_feat_ids,       TRC_CT_INTERNALS },
  { &Curl_trc_feat_multi,     TRC_CT_ਜਾਲ },
  { &Curl_trc_feat_read,      TRC_CT_NONE },
  { &Curl_trc_feat_write,     TRC_CT_NONE },
  { &Curl_trc_feat_dns,       TRC_CT_ਜਾਲ },
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DOH
  { &Curl_trc_feat_doh,       TRC_CT_ਜਾਲ },
#ਜੇ_ਅੰਤ
  { &Curl_trc_feat_timer,     TRC_CT_ਜਾਲ },
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ਧਾਗੇ
  { &Curl_trc_feat_ਧਾਗੇ,   TRC_CT_NONE },
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
  { &Curl_trc_feat_ftp,       TRC_CT_PROTOCOL },
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_SMTP
  { &Curl_trc_feat_smtp,      TRC_CT_PROTOCOL },
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
  { &Curl_trc_feat_ssls,      TRC_CT_ਜਾਲ },
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSH
  { &Curl_trc_feat_ssh,       TRC_CT_PROTOCOL },
#ਜੇ_ਅੰਤ
#ਜੇ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
  { &Curl_trc_feat_ws,        TRC_CT_PROTOCOL },
#ਜੇ_ਅੰਤ
};

ਬਣਤਰ trc_cft_def {
  ਬਣਤਰ Curl_cftype *cft;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ category;
};

ਸਥਿਰ ਬਣਤਰ trc_cft_def trc_cfts[] = {
  { &Curl_cft_dns,            TRC_CT_ਜਾਲ },
  { &Curl_cft_tcp,            TRC_CT_ਜਾਲ },
  { &Curl_cft_udp,            TRC_CT_ਜਾਲ },
  { &Curl_cft_unix,           TRC_CT_ਜਾਲ },
  { &Curl_cft_tcp_accept,     TRC_CT_ਜਾਲ },
  { &Curl_cft_ip_happy,       TRC_CT_ਜਾਲ },
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_WEBSOCKETS
  { &Curl_cft_recvbuf,        TRC_CT_PROTOCOL },
#ਜੇ_ਅੰਤ
  { &Curl_cft_ਤਿਆਰੀ,          TRC_CT_PROTOCOL },
#ਜੇ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  { &Curl_cft_nghttp2,        TRC_CT_PROTOCOL },
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
  { &Curl_cft_ssl,            TRC_CT_ਜਾਲ },
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  { &Curl_cft_ssl_proxy,      TRC_CT_PROXY },
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
  { &Curl_cft_h1_proxy,       TRC_CT_PROXY },
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NGHTTP2
  { &Curl_cft_h2_proxy,       TRC_CT_PROXY },
#ਜੇ_ਅੰਤ
#ਜੇ defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3)
  { &Curl_cft_h3_proxy,       TRC_CT_PROXY },
#ਜੇ_ਅੰਤ
  { &Curl_cft_http_proxy,     TRC_CT_PROXY },
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP */
  { &Curl_cft_haproxy,        TRC_CT_PROXY },
  { &Curl_cft_socks_proxy,    TRC_CT_PROXY },
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_PROXY */
#ਜੇ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { &Curl_cft_http3,          TRC_CT_PROTOCOL },
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
  { &Curl_cft_http_connect,   TRC_CT_PROTOCOL },
#ਜੇ_ਅੰਤ
};

ਸਥਿਰ ਖਾਲੀ trc_apply_level_by_name(ਬਣਤਰ Curl_str *token, ਪੂਰਨ lvl)
{
  size_t i;

  ਲਈ(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    ਜੇ(curlx_str_casecompare(token, trc_cfts[i].cft->name)) {
      trc_cfts[i].cft->log_level = lvl;
      ਰੋਕੋ;
    }
  }
  ਲਈ(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    ਜੇ(curlx_str_casecompare(token, trc_feats[i].feat->name)) {
      trc_feats[i].feat->log_level = lvl;
      ਰੋਕੋ;
    }
  }
}

ਸਥਿਰ ਖਾਲੀ trc_apply_level_by_category(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ category, ਪੂਰਨ lvl)
{
  size_t i;

  ਲਈ(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    ਜੇ(!category || (trc_cfts[i].category & category))
      trc_cfts[i].cft->log_level = lvl;
  }
  ਲਈ(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    ਜੇ(!category || (trc_feats[i].category & category))
      trc_feats[i].feat->log_level = lvl;
  }
}

ਸਥਿਰ CURLcode trc_opt(ਅਚਲ ਅੱਖਰ *config)
{
  ਬਣਤਰ Curl_str ਬਾਹਰਲਾ;
  ਜਦੋਂ(!curlx_str_until(&config, &ਬਾਹਰਲਾ, 32, ',')) {
    ਪੂਰਨ lvl = CURL_LOG_LVL_INFO;
    ਅਚਲ ਅੱਖਰ *token = curlx_str(&ਬਾਹਰਲਾ);

    ਜੇ(*token == '-') {
      lvl = CURL_LOG_LVL_NONE;
      curlx_str_nudge(&ਬਾਹਰਲਾ, 1);
    }
    ਨਹੀਂਤਾਂ ਜੇ(*token == '+')
      curlx_str_nudge(&ਬਾਹਰਲਾ, 1);

    ਜੇ(curlx_str_casecompare(&ਬਾਹਰਲਾ, "all"))
      trc_apply_level_by_category(TRC_CT_NONE, lvl);
    ਨਹੀਂਤਾਂ ਜੇ(curlx_str_casecompare(&ਬਾਹਰਲਾ, "protocol"))
      trc_apply_level_by_category(TRC_CT_PROTOCOL, lvl);
    ਨਹੀਂਤਾਂ ਜੇ(curlx_str_casecompare(&ਬਾਹਰਲਾ, "network"))
      trc_apply_level_by_category(TRC_CT_ਜਾਲ, lvl);
    ਨਹੀਂਤਾਂ ਜੇ(curlx_str_casecompare(&ਬਾਹਰਲਾ, "proxy"))
      trc_apply_level_by_category(TRC_CT_PROXY, lvl);
    ਨਹੀਂਤਾਂ
      trc_apply_level_by_name(&ਬਾਹਰਲਾ, lvl);

    ਜੇ(curlx_str_single(&config, ','))
      ਰੋਕੋ;
  }
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_trc_opt(ਅਚਲ ਅੱਖਰ *config)
{
  CURLcode ਨਤੀਜਾ = config ? trc_opt(config) : CURLE_OK;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  /* CURL_DEBUG can override anything */
  ਜੇ(!ਨਤੀਜਾ) {
    ਅਚਲ ਅੱਖਰ *dbg_config = getenv("CURL_DEBUG");
    ਜੇ(dbg_config)
      ਨਤੀਜਾ = trc_opt(dbg_config);
  }
#ਜੇ_ਅੰਤ /* DEBUGBUILD */
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_trc_init(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਵਾਪਸ Curl_trc_opt(ਨਿਰਾ);
#ਨਹੀਂਤਾਂ
  ਵਾਪਸ CURLE_OK;
#ਜੇ_ਅੰਤ
}

#ਨਹੀਂਤਾਂ /* CURL_DISABLE_VERBOSE_STRINGS */

CURLcode Curl_trc_init(ਖਾਲੀ)
{
  ਵਾਪਸ CURLE_OK;
}

ਖਾਲੀ Curl_infof(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)fmt;
}

ਖਾਲੀ Curl_trc_cf_infof(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਬਣਤਰ Curl_cfilter *cf,
                       ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)cf;
  (ਖਾਲੀ)fmt;
}

ਖਾਲੀ Curl_trc_multi(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)fmt;
}

ਖਾਲੀ Curl_trc_write(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)fmt;
}

ਖਾਲੀ Curl_trc_dns(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)fmt;
}

ਖਾਲੀ Curl_trc_timer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਪੂਰਨ tid, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)tid;
  (ਖਾਲੀ)fmt;
}

ਖਾਲੀ Curl_trc_read(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)fmt;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
ਖਾਲੀ Curl_trc_ftp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)fmt;
}
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_SMTP
ਖਾਲੀ Curl_trc_smtp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)fmt;
}
#ਜੇ_ਅੰਤ
#ਜੇ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
ਖਾਲੀ Curl_trc_ws(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)fmt;
}
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSH
ਖਾਲੀ Curl_trc_ssh(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)fmt;
}
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
ਖਾਲੀ Curl_trc_ssls(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)fmt;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_VERBOSE_STRINGS */
