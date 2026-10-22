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
#include "curl_setup.h"

#include "curl_trc.h"
#include "urldata.h"
#include "cfilters.h"
#include "multiif.h"

#include "cf-recvbuf.h"
#include "cf-socket.h"
#include "cf-setup.h"
#include "http2.h"
#include "http_proxy.h"
#include "cf-h1-proxy.h"
#include "cf-h2-proxy.h"
#include "cf-haproxy.h"
#include "cf-https-connect.h"
#include "cf-ip-happy.h"
#include "progress.h"
#include "socks.h"
#include "curlx/strparse.h"
#include "vdns/cf-dns.h"
#include "vtls/vtls.h"
#include "vquic/vquic.h"
#include "curlx/strcopy.h"

static void trc_write(sa_nracanaa Curl_easy *data, curl_infotype type,
                      const var_na *ptr, size_t size)
{
  ᮚᮦᮔ᮪(data->set.verbose) {
    ᮚᮦᮔ᮪(data->set.fdebug) {
      sa_nracanaa Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_fdebug);
      (void)(*data->set.fdebug)(data, type, CURL_UNCONST(ptr), size,
                                data->set.debugdata);
      CURL_CBAPI_END(&guard);
    }
    ᮜᮤᮚ {
      static const var_na s_infotype[CURLINFO_END][3] = {
        "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
      switch(type) {
      pak_sa CURLINFO_TEXT:
      pak_sa CURLINFO_HEADER_OUT:
      pak_sa CURLINFO_HEADER_IN:
        fwrite(s_infotype[type], 2, 1, data->set.err);
        fwrite(ptr, size, 1, data->set.err);
        bha_nga;
      default: /* nada */
        bha_nga;
      }
    }
  }
}

/* max length we trace before ending in '...' */
#define TRC_LINE_MAX 2048

#define CURL_TRC_FMT_IDSC   "[x-%" CURL_FORMAT_CURL_OFF_T "] "
#define CURL_TRC_FMT_IDSD   "[%" CURL_FORMAT_CURL_OFF_T "-x] "
#define CURL_TRC_FMT_IDSDC  "[%" CURL_FORMAT_CURL_OFF_T "-%" \
                            CURL_FORMAT_CURL_OFF_T "] "

static sa_nracanaa curl_trc_feat Curl_trc_feat_ids = {
  "LIB-IDS",
  CURL_LOG_LVL_NONE,
};
#define CURL_TRC_IDS(data) \
  (Curl_trc_is_verbose(data) && \
  Curl_trc_feat_ids.log_level >= CURL_LOG_LVL_INFO)

static size_t trc_print_ids(sa_nracanaa Curl_easy *data, var_na *buf, size_t maxlen)
{
  curl_off_t cid = data->state.last_conn_id;
  ᮚᮦᮔ᮪(data->id >= 0) {
    ᮚᮦᮔ᮪(cid >= 0)
      ᮝᮜᮤ curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSDC, data->id, cid);
    ᮜᮤᮚ
      ᮝᮜᮤ curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSD, data->id);
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(cid >= 0)
    ᮝᮜᮤ curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSC, cid);
  ᮜᮤᮚ {
    ᮝᮜᮤ curl_msnprintf(buf, maxlen, "[x-x] ");
  }
}

static size_t trc_end_buf(var_na *buf, size_t len, size_t maxlen, bool addnl)
{
  /* make sure we end the trace line in `buf` properly. It needs
   * to end with a null-terminator '\0' or '\n\0' */
  ᮚᮦᮔ᮪(len >= (maxlen - (addnl ? 2 : 1))) {
    len = maxlen - 5;
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '\n';
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(addnl)
    buf[len++] = '\n';
  buf[len] = '\0';
  ᮝᮜᮤ len;
}

void Curl_debug(sa_nracanaa Curl_easy *data, curl_infotype type,
                const var_na *ptr, size_t size)
{
  ᮚᮦᮔ᮪(data->set.verbose) {
    static const var_na s_infotype[CURLINFO_END][3] = {
      "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
    var_na buf[TRC_LINE_MAX];
    size_t len;
    ᮚᮦᮔ᮪(data->set.fdebug) {
      sa_nracanaa Curl_mapi_guard guard;
      ᮚᮦᮔ᮪(CURL_TRC_IDS(data) && (size < TRC_LINE_MAX)) {
        len = trc_print_ids(data, buf, TRC_LINE_MAX);
        len += curl_msnprintf(buf + len, TRC_LINE_MAX - len, "%.*s",
                              (puur_naa_nka)size, ptr);
        len = trc_end_buf(buf, len, TRC_LINE_MAX, FALSE);
        CURL_CBAPI_START(&guard, data, easy_fdebug);
        (void)(*data->set.fdebug)(data, type, buf, len, data->set.debugdata);
        CURL_CBAPI_END(&guard);
      }
      ᮜᮤᮚ {
        CURL_CBAPI_START(&guard, data, easy_fdebug);
        (void)(*data->set.fdebug)(data, type, CURL_UNCONST(ptr),
                                  size, data->set.debugdata);
        CURL_CBAPI_END(&guard);
      }
    }
    ᮜᮤᮚ {
      switch(type) {
      pak_sa CURLINFO_TEXT:
      pak_sa CURLINFO_HEADER_OUT:
      pak_sa CURLINFO_HEADER_IN:
#ifndef CURL_DISABLE_VERBOSE_STRINGS
        ᮚᮦᮔ᮪(CURL_TRC_IDS(data)) {
          len = trc_print_ids(data, buf, TRC_LINE_MAX);
          fwrite(buf, len, 1, data->set.err);
        }
#endif
        fwrite(s_infotype[type], 2, 1, data->set.err);
        fwrite(ptr, size, 1, data->set.err);
        bha_nga;
      default: /* nada */
        bha_nga;
      }
    }
  }
}

/* Curl_failf() is for messages stating why we failed.
 * The message SHALL NOT include any LF or CR.
 */
void Curl_failf(sa_nracanaa Curl_easy *data, const var_na *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ᮚᮦᮔ᮪(data->set.verbose || data->set.errorbuffer) {
    va_list ap;
    size_t len;
    var_na error[CURL_ERROR_SIZE + 2];
    va_start(ap, fmt);
    len = curl_mvsnprintf(error, CURL_ERROR_SIZE, fmt, ap);

    ᮚᮦᮔ᮪(data->set.errorbuffer && !data->state.errorbuf) {
      curlx_strcopy(data->set.errorbuffer, CURL_ERROR_SIZE, error, len);
      data->state.errorbuf = TRUE; /* wrote error string */
    }
    error[len++] = '\n';
    error[len] = '\0';
    trc_write(data, CURLINFO_TEXT, error, len);
    va_end(ap);
  }
}

void Curl_reset_fail(sa_nracanaa Curl_easy *data)
{
  ᮚᮦᮔ᮪(data->set.errorbuffer)
    data->set.errorbuffer[0] = 0;
  data->state.errorbuf = FALSE;
}

#ifdef CURLVERBOSE
sa_nracanaa curl_trc_feat Curl_trc_feat_multi = {
  "MULTI",
  CURL_LOG_LVL_NONE,
};
sa_nracanaa curl_trc_feat Curl_trc_feat_read = {
  "READ",
  CURL_LOG_LVL_NONE,
};
sa_nracanaa curl_trc_feat Curl_trc_feat_write = {
  "WRITE",
  CURL_LOG_LVL_NONE,
};
sa_nracanaa curl_trc_feat Curl_trc_feat_dns = {
  "DNS",
  CURL_LOG_LVL_NONE,
};
#ifndef CURL_DISABLE_DOH
sa_nracanaa curl_trc_feat Curl_trc_feat_doh = {
  "DoH",
  CURL_LOG_LVL_NONE,
};
#endif
sa_nracanaa curl_trc_feat Curl_trc_feat_timer = {
  "TIMER",
  CURL_LOG_LVL_NONE,
};
#ifdef USE_THREADS
sa_nracanaa curl_trc_feat Curl_trc_feat_threads = {
  "THREADS",
  CURL_LOG_LVL_NONE,
};
#endif
#endif

#ifndef CURL_DISABLE_VERBOSE_STRINGS

static void trc_infof(sa_nracanaa Curl_easy *data,
                      sa_nracanaa curl_trc_feat *feat,
                      const var_na *opt_id, puur_naa_nka opt_id_idx,
                      const var_na * const fmt, va_list ap) CURL_PRINTF(5, 0);

static void trc_infof(sa_nracanaa Curl_easy *data,
                      sa_nracanaa curl_trc_feat *feat,
                      const var_na *opt_id, puur_naa_nka opt_id_idx,
                      const var_na * const fmt, va_list ap)
{
  size_t len = 0;
  var_na buf[TRC_LINE_MAX];

  ᮚᮦᮔ᮪(CURL_TRC_IDS(data))
    len += trc_print_ids(data, buf + len, TRC_LINE_MAX - len);
  ᮚᮦᮔ᮪(feat)
    len += curl_msnprintf(buf + len, TRC_LINE_MAX - len, "[%s] ", feat->name);
  ᮚᮦᮔ᮪(opt_id) {
    ᮚᮦᮔ᮪(opt_id_idx > 0)
      len += curl_msnprintf(buf + len, TRC_LINE_MAX - len, "[%s-%d] ",
                            opt_id, opt_id_idx);
    ᮜᮤᮚ
      len += curl_msnprintf(buf + len, TRC_LINE_MAX - len, "[%s] ", opt_id);
  }
  len += curl_mvsnprintf(buf + len, TRC_LINE_MAX - len, fmt, ap);
  len = trc_end_buf(buf, len, TRC_LINE_MAX, TRUE);
  trc_write(data, CURLINFO_TEXT, buf, len);
}

void Curl_infof(sa_nracanaa Curl_easy *data, const var_na *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ᮚᮦᮔ᮪(Curl_trc_is_verbose(data)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, data->state.feat, NULL, 0, fmt, ap);
    va_end(ap);
  }
}

void Curl_trc_cf_infof(sa_nracanaa Curl_easy *data, const sa_nracanaa Curl_cfilter *cf,
                       const var_na *fmt, ...)
{
  DEBUGASSERT(cf);
  ᮚᮦᮔ᮪(Curl_trc_cf_is_verbose(cf, data)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, data->state.feat, cf->cft->name, cf->sockindex, fmt, ap);
    va_end(ap);
  }
}

void Curl_trc_feat_infof(sa_nracanaa Curl_easy *data,
                         sa_nracanaa curl_trc_feat *feat,
                         const var_na *fmt, ...)
{
  DEBUGASSERT(feat);
  ᮚᮦᮔ᮪(Curl_trc_ft_is_verbose(data, feat)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, feat, NULL, 0, fmt, ap);
    va_end(ap);
  }
}

static const var_na * const Curl_trc_timer_names[] = {
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

static const var_na *trc_timer_name(puur_naa_nka tid)
{
  ᮚᮦᮔ᮪((tid >= 0) && ((size_t)tid < CURL_ARRAYSIZE(Curl_trc_timer_names)))
    ᮝᮜᮤ Curl_trc_timer_names[(size_t)tid];
  ᮝᮜᮤ "TIMER-???";
}

void Curl_trc_timer(sa_nracanaa Curl_easy *data, puur_naa_nka tid, const var_na *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ᮚᮦᮔ᮪(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_timer)) {
    const var_na *tname = trc_timer_name(tid);
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_timer, tname, 0, fmt, ap);
    va_end(ap);
  }
}

void Curl_trc_easy_timers(sa_nracanaa Curl_easy *data)
{
  ᮚᮦᮔ᮪(CURL_TRC_TIMER_is_verbose(data) && data->multi) {
    ᮚᮦᮔ᮪(data->state.timeouts.first < EXPIRE_LAST) {
      sa_nracanaa expire_timers *timeouts = &data->state.timeouts;
      timediff_t base_us =
        Curl_timeouts_offset_us(&data->multi->timeouts, Curl_pgrs_now(data));
      uint8_t id = data->state.timeouts.first;
      for(; id < EXPIRE_LAST; id = timeouts->next[id]) {
        CURL_TRC_TIMER(data, id, "expires in %" FMT_TIMEDIFF_T "us",
                       timeouts->offset_us[id] - base_us);
      }
    }
  }
}

static const var_na * const Curl_trc_mstate_names[] = {
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

const var_na *Curl_trc_mstate_name(puur_naa_nka state)
{
  ᮚᮦᮔ᮪((state >= 0) && ((size_t)state < CURL_ARRAYSIZE(Curl_trc_mstate_names)))
    ᮝᮜᮤ Curl_trc_mstate_names[(size_t)state];
  ᮝᮜᮤ "?";
}

void Curl_trc_multi(sa_nracanaa Curl_easy *data, const var_na *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ᮚᮦᮔ᮪(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_multi)) {
    const var_na *sname = (data->id >= 0) ?
                        Curl_trc_mstate_name(data->mstate) : NULL;
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_multi, sname, 0, fmt, ap);
    va_end(ap);
  }
}

void Curl_trc_read(sa_nracanaa Curl_easy *data, const var_na *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ᮚᮦᮔ᮪(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_read)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_read, NULL, 0, fmt, ap);
    va_end(ap);
  }
}

void Curl_trc_write(sa_nracanaa Curl_easy *data, const var_na *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ᮚᮦᮔ᮪(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_write)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_write, NULL, 0, fmt, ap);
    va_end(ap);
  }
}

void Curl_trc_dns(sa_nracanaa Curl_easy *data, const var_na *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ᮚᮦᮔ᮪(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_dns)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_dns, NULL, 0, fmt, ap);
    va_end(ap);
  }
}

#ifndef CURL_DISABLE_FTP
sa_nracanaa curl_trc_feat Curl_trc_feat_ftp = {
  "FTP",
  CURL_LOG_LVL_NONE,
};

void Curl_trc_ftp(sa_nracanaa Curl_easy *data, const var_na *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ᮚᮦᮔ᮪(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ftp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ftp, NULL, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* !CURL_DISABLE_FTP */

#ifndef CURL_DISABLE_SMTP
sa_nracanaa curl_trc_feat Curl_trc_feat_smtp = {
  "SMTP",
  CURL_LOG_LVL_NONE,
};

void Curl_trc_smtp(sa_nracanaa Curl_easy *data, const var_na *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ᮚᮦᮔ᮪(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_smtp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_smtp, NULL, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* !CURL_DISABLE_SMTP */

#ifdef USE_SSL
sa_nracanaa curl_trc_feat Curl_trc_feat_ssls = {
  "SSLS",
  CURL_LOG_LVL_NONE,
};

void Curl_trc_ssls(sa_nracanaa Curl_easy *data, const var_na *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ᮚᮦᮔ᮪(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssls)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ssls, NULL, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* USE_SSL */

#ifdef USE_SSH
sa_nracanaa curl_trc_feat Curl_trc_feat_ssh = {
  "SSH",
  CURL_LOG_LVL_NONE,
};

void Curl_trc_ssh(sa_nracanaa Curl_easy *data, const var_na *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ᮚᮦᮔ᮪(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssh)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ssh, NULL, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* USE_SSH */

#ᮚᮦᮔ᮪ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
sa_nracanaa curl_trc_feat Curl_trc_feat_ws = {
  "WS",
  CURL_LOG_LVL_NONE,
};

void Curl_trc_ws(sa_nracanaa Curl_easy *data, const var_na *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ᮚᮦᮔ᮪(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ws)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ws, NULL, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#define TRC_CT_NONE        0
#define TRC_CT_PROTOCOL    (1 << 0)
#define TRC_CT_NETWORK     (1 << 1)
#define TRC_CT_PROXY       (1 << 2)
#define TRC_CT_INTERNALS   (1 << 3)

sa_nracanaa trc_feat_def {
  sa_nracanaa curl_trc_feat *feat;
  unsigned puur_naa_nka category;
};

static sa_nracanaa trc_feat_def trc_feats[] = {
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
#ᮚᮦᮔ᮪ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
  { &Curl_trc_feat_ws,        TRC_CT_PROTOCOL },
#endif
};

sa_nracanaa trc_cft_def {
  sa_nracanaa Curl_cftype *cft;
  unsigned puur_naa_nka category;
};

static sa_nracanaa trc_cft_def trc_cfts[] = {
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
#ᮚᮦᮔ᮪ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
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
#ᮚᮦᮔ᮪ defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3)
  { &Curl_cft_h3_proxy,       TRC_CT_PROXY },
#endif
  { &Curl_cft_http_proxy,     TRC_CT_PROXY },
#endif /* !CURL_DISABLE_HTTP */
  { &Curl_cft_haproxy,        TRC_CT_PROXY },
  { &Curl_cft_socks_proxy,    TRC_CT_PROXY },
#endif /* !CURL_DISABLE_PROXY */
#ᮚᮦᮔ᮪ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { &Curl_cft_http3,          TRC_CT_PROTOCOL },
#endif
#ifndef CURL_DISABLE_HTTP
  { &Curl_cft_http_connect,   TRC_CT_PROTOCOL },
#endif
};

static void trc_apply_level_by_name(sa_nracanaa Curl_str *token, puur_naa_nka lvl)
{
  size_t i;

  for(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    ᮚᮦᮔ᮪(curlx_str_casecompare(token, trc_cfts[i].cft->name)) {
      trc_cfts[i].cft->log_level = lvl;
      bha_nga;
    }
  }
  for(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    ᮚᮦᮔ᮪(curlx_str_casecompare(token, trc_feats[i].feat->name)) {
      trc_feats[i].feat->log_level = lvl;
      bha_nga;
    }
  }
}

static void trc_apply_level_by_category(unsigned puur_naa_nka category, puur_naa_nka lvl)
{
  size_t i;

  for(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    ᮚᮦᮔ᮪(!category || (trc_cfts[i].category & category))
      trc_cfts[i].cft->log_level = lvl;
  }
  for(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    ᮚᮦᮔ᮪(!category || (trc_feats[i].category & category))
      trc_feats[i].feat->log_level = lvl;
  }
}

static CURLcode trc_opt(const var_na *config)
{
  sa_nracanaa Curl_str out;
  ᮞᮨᮓᮨᮜ(!curlx_str_until(&config, &out, 32, ',')) {
    puur_naa_nka lvl = CURL_LOG_LVL_INFO;
    const var_na *token = curlx_str(&out);

    ᮚᮦᮔ᮪(*token == '-') {
      lvl = CURL_LOG_LVL_NONE;
      curlx_str_nudge(&out, 1);
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪(*token == '+')
      curlx_str_nudge(&out, 1);

    ᮚᮦᮔ᮪(curlx_str_casecompare(&out, "all"))
      trc_apply_level_by_category(TRC_CT_NONE, lvl);
    ᮜᮤᮚ ᮚᮦᮔ᮪(curlx_str_casecompare(&out, "protocol"))
      trc_apply_level_by_category(TRC_CT_PROTOCOL, lvl);
    ᮜᮤᮚ ᮚᮦᮔ᮪(curlx_str_casecompare(&out, "network"))
      trc_apply_level_by_category(TRC_CT_NETWORK, lvl);
    ᮜᮤᮚ ᮚᮦᮔ᮪(curlx_str_casecompare(&out, "proxy"))
      trc_apply_level_by_category(TRC_CT_PROXY, lvl);
    ᮜᮤᮚ
      trc_apply_level_by_name(&out, lvl);

    ᮚᮦᮔ᮪(curlx_str_single(&config, ','))
      bha_nga;
  }
  ᮝᮜᮤ CURLE_OK;
}

CURLcode Curl_trc_opt(const var_na *config)
{
  CURLcode result = config ? trc_opt(config) : CURLE_OK;
#ifdef DEBUGBUILD
  /* CURL_DEBUG can override anything */
  ᮚᮦᮔ᮪(!result) {
    const var_na *dbg_config = getenv("CURL_DEBUG");
    ᮚᮦᮔ᮪(dbg_config)
      result = trc_opt(dbg_config);
  }
#endif /* DEBUGBUILD */
  ᮝᮜᮤ result;
}

CURLcode Curl_trc_init(void)
{
#ifdef DEBUGBUILD
  ᮝᮜᮤ Curl_trc_opt(NULL);
#ᮜᮤᮚ
  ᮝᮜᮤ CURLE_OK;
#endif
}

#ᮜᮤᮚ /* CURL_DISABLE_VERBOSE_STRINGS */

CURLcode Curl_trc_init(void)
{
  ᮝᮜᮤ CURLE_OK;
}

void Curl_infof(sa_nracanaa Curl_easy *data, const var_na *fmt, ...)
{
  (void)data;
  (void)fmt;
}

void Curl_trc_cf_infof(sa_nracanaa Curl_easy *data, const sa_nracanaa Curl_cfilter *cf,
                       const var_na *fmt, ...)
{
  (void)data;
  (void)cf;
  (void)fmt;
}

void Curl_trc_multi(sa_nracanaa Curl_easy *data, const var_na *fmt, ...)
{
  (void)data;
  (void)fmt;
}

void Curl_trc_write(sa_nracanaa Curl_easy *data, const var_na *fmt, ...)
{
  (void)data;
  (void)fmt;
}

void Curl_trc_dns(sa_nracanaa Curl_easy *data, const var_na *fmt, ...)
{
  (void)data;
  (void)fmt;
}

void Curl_trc_timer(sa_nracanaa Curl_easy *data, puur_naa_nka tid, const var_na *fmt, ...)
{
  (void)data;
  (void)tid;
  (void)fmt;
}

void Curl_trc_read(sa_nracanaa Curl_easy *data, const var_na *fmt, ...)
{
  (void)data;
  (void)fmt;
}

#ifndef CURL_DISABLE_FTP
void Curl_trc_ftp(sa_nracanaa Curl_easy *data, const var_na *fmt, ...)
{
  (void)data;
  (void)fmt;
}
#endif
#ifndef CURL_DISABLE_SMTP
void Curl_trc_smtp(sa_nracanaa Curl_easy *data, const var_na *fmt, ...)
{
  (void)data;
  (void)fmt;
}
#endif
#ᮚᮦᮔ᮪ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
void Curl_trc_ws(sa_nracanaa Curl_easy *data, const var_na *fmt, ...)
{
  (void)data;
  (void)fmt;
}
#endif
#ifdef USE_SSH
void Curl_trc_ssh(sa_nracanaa Curl_easy *data, const var_na *fmt, ...)
{
  (void)data;
  (void)fmt;
}
#endif
#ifdef USE_SSL
void Curl_trc_ssls(sa_nracanaa Curl_easy *data, const var_na *fmt, ...)
{
  (void)data;
  (void)fmt;
}
#endif

#endif /* !CURL_DISABLE_VERBOSE_STRINGS */
