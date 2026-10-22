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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "multiif.h"

#അന്തര്ഭാവ "cf-recvbuf.h"
#അന്തര്ഭാവ "cf-socket.h"
#അന്തര്ഭാവ "cf-setup.h"
#അന്തര്ഭാവ "http2.h"
#അന്തര്ഭാവ "http_proxy.h"
#അന്തര്ഭാവ "cf-h1-proxy.h"
#അന്തര്ഭാവ "cf-h2-proxy.h"
#അന്തര്ഭാവ "cf-haproxy.h"
#അന്തര്ഭാവ "cf-https-connect.h"
#അന്തര്ഭാവ "cf-ip-happy.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "socks.h"
#അന്തര്ഭാവ "curlx/strparse.h"
#അന്തര്ഭാവ "vdns/cf-dns.h"
#അന്തര്ഭാവ "vtls/vtls.h"
#അന്തര്ഭാവ "vquic/vquic.h"
#അന്തര്ഭാവ "curlx/strcopy.h"

സ്ഥിതം ശൂന്യം trc_write(ഘടന Curl_easy *data, curl_infotype type,
                      സ്ഥിരം അക്ഷരം *ptr, size_t size)
{
  എങ്കിൽ(data->set.verbose) {
    എങ്കിൽ(data->set.fdebug) {
      ഘടന Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_fdebug);
      (ശൂന്യം)(*data->set.fdebug)(data, type, CURL_UNCONST(ptr), size,
                                data->set.debugdata);
      CURL_CBAPI_END(&guard);
    }
    അല്ലെങ്കിൽ {
      സ്ഥിതം സ്ഥിരം അക്ഷരം s_infotype[CURLINFO_END][3] = {
        "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
      തിരഞ്ഞെടുപ്പ്(type) {
      അവസ്ഥ CURLINFO_TEXT:
      അവസ്ഥ CURLINFO_HEADER_OUT:
      അവസ്ഥ CURLINFO_HEADER_IN:
        fwrite(s_infotype[type], 2, 1, data->set.err);
        fwrite(ptr, size, 1, data->set.err);
        നിർത്തുക;
      സ്ഥിരസ്ഥിതി: /* nada */
        നിർത്തുക;
      }
    }
  }
}

/* max length we trace before ending in '...' */
#define TRC_വരി_MAX 2048

#define CURL_TRC_FMT_IDSC   "[x-%" CURL_FORMAT_CURL_OFF_T "] "
#define CURL_TRC_FMT_IDSD   "[%" CURL_FORMAT_CURL_OFF_T "-x] "
#define CURL_TRC_FMT_IDSDC  "[%" CURL_FORMAT_CURL_OFF_T "-%" \
                            CURL_FORMAT_CURL_OFF_T "] "

സ്ഥിതം ഘടന curl_trc_feat Curl_trc_feat_ids = {
  "LIB-IDS",
  CURL_LOG_LVL_NONE,
};
#define CURL_TRC_IDS(data) \
  (Curl_trc_is_verbose(data) && \
  Curl_trc_feat_ids.log_level >= CURL_LOG_LVL_INFO)

സ്ഥിതം size_t trc_print_ids(ഘടന Curl_easy *data, അക്ഷരം *buf, size_t maxlen)
{
  curl_off_t cid = data->state.last_conn_id;
  എങ്കിൽ(data->id >= 0) {
    എങ്കിൽ(cid >= 0)
      തിരികെ curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSDC, data->id, cid);
    അല്ലെങ്കിൽ
      തിരികെ curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSD, data->id);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(cid >= 0)
    തിരികെ curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSC, cid);
  അല്ലെങ്കിൽ {
    തിരികെ curl_msnprintf(buf, maxlen, "[x-x] ");
  }
}

സ്ഥിതം size_t trc_end_buf(അക്ഷരം *buf, size_t len, size_t maxlen, bool addnl)
{
  /* make sure we end the trace line in `buf` properly. It needs
   * to end with a null-terminator '\0' or '\n\0' */
  എങ്കിൽ(len >= (maxlen - (addnl ? 2 : 1))) {
    len = maxlen - 5;
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '\n';
  }
  അല്ലെങ്കിൽ എങ്കിൽ(addnl)
    buf[len++] = '\n';
  buf[len] = '\0';
  തിരികെ len;
}

ശൂന്യം Curl_debug(ഘടന Curl_easy *data, curl_infotype type,
                സ്ഥിരം അക്ഷരം *ptr, size_t size)
{
  എങ്കിൽ(data->set.verbose) {
    സ്ഥിതം സ്ഥിരം അക്ഷരം s_infotype[CURLINFO_END][3] = {
      "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
    അക്ഷരം buf[TRC_വരി_MAX];
    size_t len;
    എങ്കിൽ(data->set.fdebug) {
      ഘടന Curl_mapi_guard guard;
      എങ്കിൽ(CURL_TRC_IDS(data) && (size < TRC_വരി_MAX)) {
        len = trc_print_ids(data, buf, TRC_വരി_MAX);
        len += curl_msnprintf(buf + len, TRC_വരി_MAX - len, "%.*s",
                              (പൂർണ്ണസംഖ്യ)size, ptr);
        len = trc_end_buf(buf, len, TRC_വരി_MAX, FALSE);
        CURL_CBAPI_START(&guard, data, easy_fdebug);
        (ശൂന്യം)(*data->set.fdebug)(data, type, buf, len, data->set.debugdata);
        CURL_CBAPI_END(&guard);
      }
      അല്ലെങ്കിൽ {
        CURL_CBAPI_START(&guard, data, easy_fdebug);
        (ശൂന്യം)(*data->set.fdebug)(data, type, CURL_UNCONST(ptr),
                                  size, data->set.debugdata);
        CURL_CBAPI_END(&guard);
      }
    }
    അല്ലെങ്കിൽ {
      തിരഞ്ഞെടുപ്പ്(type) {
      അവസ്ഥ CURLINFO_TEXT:
      അവസ്ഥ CURLINFO_HEADER_OUT:
      അവസ്ഥ CURLINFO_HEADER_IN:
#ifndef CURL_DISABLE_VERBOSE_STRINGS
        എങ്കിൽ(CURL_TRC_IDS(data)) {
          len = trc_print_ids(data, buf, TRC_വരി_MAX);
          fwrite(buf, len, 1, data->set.err);
        }
#endif
        fwrite(s_infotype[type], 2, 1, data->set.err);
        fwrite(ptr, size, 1, data->set.err);
        നിർത്തുക;
      സ്ഥിരസ്ഥിതി: /* nada */
        നിർത്തുക;
      }
    }
  }
}

/* Curl_failf() is for messages stating why we failed.
 * The message SHALL NOT include any LF or CR.
 */
ശൂന്യം Curl_failf(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  എങ്കിൽ(data->set.verbose || data->set.errorbuffer) {
    va_list ap;
    size_t len;
    അക്ഷരം error[CURL_ERROR_SIZE + 2];
    va_start(ap, fmt);
    len = curl_mvsnprintf(error, CURL_ERROR_SIZE, fmt, ap);

    എങ്കിൽ(data->set.errorbuffer && !data->state.errorbuf) {
      curlx_strcopy(data->set.errorbuffer, CURL_ERROR_SIZE, error, len);
      data->state.errorbuf = TRUE; /* wrote error string */
    }
    error[len++] = '\n';
    error[len] = '\0';
    trc_write(data, CURLINFO_TEXT, error, len);
    va_end(ap);
  }
}

ശൂന്യം Curl_reset_fail(ഘടന Curl_easy *data)
{
  എങ്കിൽ(data->set.errorbuffer)
    data->set.errorbuffer[0] = 0;
  data->state.errorbuf = FALSE;
}

#ifdef CURLVERBOSE
ഘടന curl_trc_feat Curl_trc_feat_multi = {
  "MULTI",
  CURL_LOG_LVL_NONE,
};
ഘടന curl_trc_feat Curl_trc_feat_read = {
  "READ",
  CURL_LOG_LVL_NONE,
};
ഘടന curl_trc_feat Curl_trc_feat_write = {
  "WRITE",
  CURL_LOG_LVL_NONE,
};
ഘടന curl_trc_feat Curl_trc_feat_dns = {
  "DNS",
  CURL_LOG_LVL_NONE,
};
#ifndef CURL_DISABLE_DOH
ഘടന curl_trc_feat Curl_trc_feat_doh = {
  "DoH",
  CURL_LOG_LVL_NONE,
};
#endif
ഘടന curl_trc_feat Curl_trc_feat_timer = {
  "TIMER",
  CURL_LOG_LVL_NONE,
};
#ifdef USE_THREADS
ഘടന curl_trc_feat Curl_trc_feat_threads = {
  "THREADS",
  CURL_LOG_LVL_NONE,
};
#endif
#endif

#ifndef CURL_DISABLE_VERBOSE_STRINGS

സ്ഥിതം ശൂന്യം trc_infof(ഘടന Curl_easy *data,
                      ഘടന curl_trc_feat *feat,
                      സ്ഥിരം അക്ഷരം *opt_id, പൂർണ്ണസംഖ്യ opt_id_idx,
                      സ്ഥിരം അക്ഷരം * സ്ഥിരം fmt, va_list ap) CURL_PRINTF(5, 0);

സ്ഥിതം ശൂന്യം trc_infof(ഘടന Curl_easy *data,
                      ഘടന curl_trc_feat *feat,
                      സ്ഥിരം അക്ഷരം *opt_id, പൂർണ്ണസംഖ്യ opt_id_idx,
                      സ്ഥിരം അക്ഷരം * സ്ഥിരം fmt, va_list ap)
{
  size_t len = 0;
  അക്ഷരം buf[TRC_വരി_MAX];

  എങ്കിൽ(CURL_TRC_IDS(data))
    len += trc_print_ids(data, buf + len, TRC_വരി_MAX - len);
  എങ്കിൽ(feat)
    len += curl_msnprintf(buf + len, TRC_വരി_MAX - len, "[%s] ", feat->name);
  എങ്കിൽ(opt_id) {
    എങ്കിൽ(opt_id_idx > 0)
      len += curl_msnprintf(buf + len, TRC_വരി_MAX - len, "[%s-%d] ",
                            opt_id, opt_id_idx);
    അല്ലെങ്കിൽ
      len += curl_msnprintf(buf + len, TRC_വരി_MAX - len, "[%s] ", opt_id);
  }
  len += curl_mvsnprintf(buf + len, TRC_വരി_MAX - len, fmt, ap);
  len = trc_end_buf(buf, len, TRC_വരി_MAX, TRUE);
  trc_write(data, CURLINFO_TEXT, buf, len);
}

ശൂന്യം Curl_infof(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  എങ്കിൽ(Curl_trc_is_verbose(data)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, data->state.feat, ശൂന്യ, 0, fmt, ap);
    va_end(ap);
  }
}

ശൂന്യം Curl_trc_cf_infof(ഘടന Curl_easy *data, സ്ഥിരം ഘടന Curl_cfilter *cf,
                       സ്ഥിരം അക്ഷരം *fmt, ...)
{
  DEBUGASSERT(cf);
  എങ്കിൽ(Curl_trc_cf_is_verbose(cf, data)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, data->state.feat, cf->cft->name, cf->sockindex, fmt, ap);
    va_end(ap);
  }
}

ശൂന്യം Curl_trc_feat_infof(ഘടന Curl_easy *data,
                         ഘടന curl_trc_feat *feat,
                         സ്ഥിരം അക്ഷരം *fmt, ...)
{
  DEBUGASSERT(feat);
  എങ്കിൽ(Curl_trc_ft_is_verbose(data, feat)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, feat, ശൂന്യ, 0, fmt, ap);
    va_end(ap);
  }
}

സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം Curl_trc_timer_names[] = {
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

സ്ഥിതം സ്ഥിരം അക്ഷരം *trc_timer_name(പൂർണ്ണസംഖ്യ tid)
{
  എങ്കിൽ((tid >= 0) && ((size_t)tid < CURL_ARRAYSIZE(Curl_trc_timer_names)))
    തിരികെ Curl_trc_timer_names[(size_t)tid];
  തിരികെ "TIMER-???";
}

ശൂന്യം Curl_trc_timer(ഘടന Curl_easy *data, പൂർണ്ണസംഖ്യ tid, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  എങ്കിൽ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_timer)) {
    സ്ഥിരം അക്ഷരം *tname = trc_timer_name(tid);
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_timer, tname, 0, fmt, ap);
    va_end(ap);
  }
}

ശൂന്യം Curl_trc_easy_timers(ഘടന Curl_easy *data)
{
  എങ്കിൽ(CURL_TRC_TIMER_is_verbose(data) && data->multi) {
    എങ്കിൽ(data->state.timeouts.first < EXPIRE_LAST) {
      ഘടന expire_timers *timeouts = &data->state.timeouts;
      timediff_t base_us =
        Curl_timeouts_offset_us(&data->multi->timeouts, Curl_pgrs_now(data));
      uint8_t id = data->state.timeouts.first;
      വേണ്ടി(; id < EXPIRE_LAST; id = timeouts->next[id]) {
        CURL_TRC_TIMER(data, id, "expires in %" FMT_TIMEDIFF_T "us",
                       timeouts->offset_us[id] - base_us);
      }
    }
  }
}

സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം Curl_trc_mstate_names[] = {
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

സ്ഥിരം അക്ഷരം *Curl_trc_mstate_name(പൂർണ്ണസംഖ്യ state)
{
  എങ്കിൽ((state >= 0) && ((size_t)state < CURL_ARRAYSIZE(Curl_trc_mstate_names)))
    തിരികെ Curl_trc_mstate_names[(size_t)state];
  തിരികെ "?";
}

ശൂന്യം Curl_trc_multi(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  എങ്കിൽ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_multi)) {
    സ്ഥിരം അക്ഷരം *sname = (data->id >= 0) ?
                        Curl_trc_mstate_name(data->mstate) : ശൂന്യ;
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_multi, sname, 0, fmt, ap);
    va_end(ap);
  }
}

ശൂന്യം Curl_trc_read(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  എങ്കിൽ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_read)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_read, ശൂന്യ, 0, fmt, ap);
    va_end(ap);
  }
}

ശൂന്യം Curl_trc_write(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  എങ്കിൽ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_write)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_write, ശൂന്യ, 0, fmt, ap);
    va_end(ap);
  }
}

ശൂന്യം Curl_trc_dns(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  എങ്കിൽ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_dns)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_dns, ശൂന്യ, 0, fmt, ap);
    va_end(ap);
  }
}

#ifndef CURL_DISABLE_FTP
ഘടന curl_trc_feat Curl_trc_feat_ftp = {
  "FTP",
  CURL_LOG_LVL_NONE,
};

ശൂന്യം Curl_trc_ftp(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  എങ്കിൽ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ftp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ftp, ശൂന്യ, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* !CURL_DISABLE_FTP */

#ifndef CURL_DISABLE_SMTP
ഘടന curl_trc_feat Curl_trc_feat_smtp = {
  "SMTP",
  CURL_LOG_LVL_NONE,
};

ശൂന്യം Curl_trc_smtp(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  എങ്കിൽ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_smtp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_smtp, ശൂന്യ, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* !CURL_DISABLE_SMTP */

#ifdef USE_SSL
ഘടന curl_trc_feat Curl_trc_feat_ssls = {
  "SSLS",
  CURL_LOG_LVL_NONE,
};

ശൂന്യം Curl_trc_ssls(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  എങ്കിൽ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssls)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ssls, ശൂന്യ, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* USE_SSL */

#ifdef USE_SSH
ഘടന curl_trc_feat Curl_trc_feat_ssh = {
  "SSH",
  CURL_LOG_LVL_NONE,
};

ശൂന്യം Curl_trc_ssh(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  എങ്കിൽ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssh)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ssh, ശൂന്യ, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* USE_SSH */

#എങ്കിൽ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
ഘടന curl_trc_feat Curl_trc_feat_ws = {
  "WS",
  CURL_LOG_LVL_NONE,
};

ശൂന്യം Curl_trc_ws(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  എങ്കിൽ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ws)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ws, ശൂന്യ, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#define TRC_CT_NONE        0
#define TRC_CT_PROTOCOL    (1 << 0)
#define TRC_CT_NETWORK     (1 << 1)
#define TRC_CT_PROXY       (1 << 2)
#define TRC_CT_INTERNALS   (1 << 3)

ഘടന trc_feat_def {
  ഘടന curl_trc_feat *feat;
  unsigned പൂർണ്ണസംഖ്യ category;
};

സ്ഥിതം ഘടന trc_feat_def trc_feats[] = {
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
#എങ്കിൽ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
  { &Curl_trc_feat_ws,        TRC_CT_PROTOCOL },
#endif
};

ഘടന trc_cft_def {
  ഘടന Curl_cftype *cft;
  unsigned പൂർണ്ണസംഖ്യ category;
};

സ്ഥിതം ഘടന trc_cft_def trc_cfts[] = {
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
#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
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
#എങ്കിൽ defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3)
  { &Curl_cft_h3_proxy,       TRC_CT_PROXY },
#endif
  { &Curl_cft_http_proxy,     TRC_CT_PROXY },
#endif /* !CURL_DISABLE_HTTP */
  { &Curl_cft_haproxy,        TRC_CT_PROXY },
  { &Curl_cft_socks_proxy,    TRC_CT_PROXY },
#endif /* !CURL_DISABLE_PROXY */
#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { &Curl_cft_http3,          TRC_CT_PROTOCOL },
#endif
#ifndef CURL_DISABLE_HTTP
  { &Curl_cft_http_connect,   TRC_CT_PROTOCOL },
#endif
};

സ്ഥിതം ശൂന്യം trc_apply_level_by_name(ഘടന Curl_str *token, പൂർണ്ണസംഖ്യ lvl)
{
  size_t i;

  വേണ്ടി(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    എങ്കിൽ(curlx_str_casecompare(token, trc_cfts[i].cft->name)) {
      trc_cfts[i].cft->log_level = lvl;
      നിർത്തുക;
    }
  }
  വേണ്ടി(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    എങ്കിൽ(curlx_str_casecompare(token, trc_feats[i].feat->name)) {
      trc_feats[i].feat->log_level = lvl;
      നിർത്തുക;
    }
  }
}

സ്ഥിതം ശൂന്യം trc_apply_level_by_category(unsigned പൂർണ്ണസംഖ്യ category, പൂർണ്ണസംഖ്യ lvl)
{
  size_t i;

  വേണ്ടി(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    എങ്കിൽ(!category || (trc_cfts[i].category & category))
      trc_cfts[i].cft->log_level = lvl;
  }
  വേണ്ടി(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    എങ്കിൽ(!category || (trc_feats[i].category & category))
      trc_feats[i].feat->log_level = lvl;
  }
}

സ്ഥിതം CURLcode trc_opt(സ്ഥിരം അക്ഷരം *config)
{
  ഘടന Curl_str out;
  ആകുമ്പോൾ(!curlx_str_until(&config, &out, 32, ',')) {
    പൂർണ്ണസംഖ്യ lvl = CURL_LOG_LVL_INFO;
    സ്ഥിരം അക്ഷരം *token = curlx_str(&out);

    എങ്കിൽ(*token == '-') {
      lvl = CURL_LOG_LVL_NONE;
      curlx_str_nudge(&out, 1);
    }
    അല്ലെങ്കിൽ എങ്കിൽ(*token == '+')
      curlx_str_nudge(&out, 1);

    എങ്കിൽ(curlx_str_casecompare(&out, "all"))
      trc_apply_level_by_category(TRC_CT_NONE, lvl);
    അല്ലെങ്കിൽ എങ്കിൽ(curlx_str_casecompare(&out, "protocol"))
      trc_apply_level_by_category(TRC_CT_PROTOCOL, lvl);
    അല്ലെങ്കിൽ എങ്കിൽ(curlx_str_casecompare(&out, "network"))
      trc_apply_level_by_category(TRC_CT_NETWORK, lvl);
    അല്ലെങ്കിൽ എങ്കിൽ(curlx_str_casecompare(&out, "proxy"))
      trc_apply_level_by_category(TRC_CT_PROXY, lvl);
    അല്ലെങ്കിൽ
      trc_apply_level_by_name(&out, lvl);

    എങ്കിൽ(curlx_str_single(&config, ','))
      നിർത്തുക;
  }
  തിരികെ CURLE_OK;
}

CURLcode Curl_trc_opt(സ്ഥിരം അക്ഷരം *config)
{
  CURLcode result = config ? trc_opt(config) : CURLE_OK;
#ifdef DEBUGBUILD
  /* CURL_DEBUG can override anything */
  എങ്കിൽ(!result) {
    സ്ഥിരം അക്ഷരം *dbg_config = getenv("CURL_DEBUG");
    എങ്കിൽ(dbg_config)
      result = trc_opt(dbg_config);
  }
#endif /* DEBUGBUILD */
  തിരികെ result;
}

CURLcode Curl_trc_init(ശൂന്യം)
{
#ifdef DEBUGBUILD
  തിരികെ Curl_trc_opt(ശൂന്യ);
#അല്ലെങ്കിൽ
  തിരികെ CURLE_OK;
#endif
}

#അല്ലെങ്കിൽ /* CURL_DISABLE_VERBOSE_STRINGS */

CURLcode Curl_trc_init(ശൂന്യം)
{
  തിരികെ CURLE_OK;
}

ശൂന്യം Curl_infof(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  (ശൂന്യം)data;
  (ശൂന്യം)fmt;
}

ശൂന്യം Curl_trc_cf_infof(ഘടന Curl_easy *data, സ്ഥിരം ഘടന Curl_cfilter *cf,
                       സ്ഥിരം അക്ഷരം *fmt, ...)
{
  (ശൂന്യം)data;
  (ശൂന്യം)cf;
  (ശൂന്യം)fmt;
}

ശൂന്യം Curl_trc_multi(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  (ശൂന്യം)data;
  (ശൂന്യം)fmt;
}

ശൂന്യം Curl_trc_write(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  (ശൂന്യം)data;
  (ശൂന്യം)fmt;
}

ശൂന്യം Curl_trc_dns(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  (ശൂന്യം)data;
  (ശൂന്യം)fmt;
}

ശൂന്യം Curl_trc_timer(ഘടന Curl_easy *data, പൂർണ്ണസംഖ്യ tid, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  (ശൂന്യം)data;
  (ശൂന്യം)tid;
  (ശൂന്യം)fmt;
}

ശൂന്യം Curl_trc_read(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  (ശൂന്യം)data;
  (ശൂന്യം)fmt;
}

#ifndef CURL_DISABLE_FTP
ശൂന്യം Curl_trc_ftp(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  (ശൂന്യം)data;
  (ശൂന്യം)fmt;
}
#endif
#ifndef CURL_DISABLE_SMTP
ശൂന്യം Curl_trc_smtp(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  (ശൂന്യം)data;
  (ശൂന്യം)fmt;
}
#endif
#എങ്കിൽ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
ശൂന്യം Curl_trc_ws(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  (ശൂന്യം)data;
  (ശൂന്യം)fmt;
}
#endif
#ifdef USE_SSH
ശൂന്യം Curl_trc_ssh(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  (ശൂന്യം)data;
  (ശൂന്യം)fmt;
}
#endif
#ifdef USE_SSL
ശൂന്യം Curl_trc_ssls(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  (ശൂന്യം)data;
  (ശൂന്യം)fmt;
}
#endif

#endif /* !CURL_DISABLE_VERBOSE_STRINGS */
