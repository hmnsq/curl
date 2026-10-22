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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "multiif.h"

#અન્તર્ભાવ "cf-recvbuf.h"
#અન્તર્ભાવ "cf-socket.h"
#અન્તર્ભાવ "cf-setup.h"
#અન્તર્ભાવ "http2.h"
#અન્તર્ભાવ "http_proxy.h"
#અન્તર્ભાવ "cf-h1-proxy.h"
#અન્તર્ભાવ "cf-h2-proxy.h"
#અન્તર્ભાવ "cf-haproxy.h"
#અન્તર્ભાવ "cf-https-connect.h"
#અન્તર્ભાવ "cf-ip-happy.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "socks.h"
#અન્તર્ભાવ "curlx/strparse.h"
#અન્તર્ભાવ "vdns/cf-dns.h"
#અન્તર્ભાવ "vtls/vtls.h"
#અન્તર્ભાવ "vquic/vquic.h"
#અન્તર્ભાવ "curlx/strcopy.h"

સ્થિર શૂન્ય trc_write(સંરચના Curl_easy *data, curl_infotype type,
                      અચલ અક્ષર *ptr, size_t size)
{
  જો(data->set.verbose) {
    જો(data->set.fdebug) {
      સંરચના Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_fdebug);
      (શૂન્ય)(*data->set.fdebug)(data, type, CURL_UNCONST(ptr), size,
                                data->set.debugdata);
      CURL_CBAPI_END(&guard);
    }
    નહીંતર {
      સ્થિર અચલ અક્ષર s_infotype[CURLINFO_END][3] = {
        "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
      પસંદગી(type) {
      સ્થિતિ CURLINFO_TEXT:
      સ્થિતિ CURLINFO_HEADER_OUT:
      સ્થિતિ CURLINFO_HEADER_IN:
        fwrite(s_infotype[type], 2, 1, data->set.err);
        fwrite(ptr, size, 1, data->set.err);
        તોડો;
      મૂળભૂત: /* nada */
        તોડો;
      }
    }
  }
}

/* max length we trace before ending in '...' */
#define TRC_લીટી_MAX 2048

#define CURL_TRC_FMT_IDSC   "[x-%" CURL_FORMAT_CURL_OFF_T "] "
#define CURL_TRC_FMT_IDSD   "[%" CURL_FORMAT_CURL_OFF_T "-x] "
#define CURL_TRC_FMT_IDSDC  "[%" CURL_FORMAT_CURL_OFF_T "-%" \
                            CURL_FORMAT_CURL_OFF_T "] "

સ્થિર સંરચના curl_trc_feat Curl_trc_feat_ids = {
  "LIB-IDS",
  CURL_LOG_LVL_NONE,
};
#define CURL_TRC_IDS(data) \
  (Curl_trc_is_verbose(data) && \
  Curl_trc_feat_ids.log_level >= CURL_LOG_LVL_INFO)

સ્થિર size_t trc_print_ids(સંરચના Curl_easy *data, અક્ષર *buf, size_t maxlen)
{
  curl_off_t cid = data->state.last_conn_id;
  જો(data->id >= 0) {
    જો(cid >= 0)
      પાછા curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSDC, data->id, cid);
    નહીંતર
      પાછા curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSD, data->id);
  }
  નહીંતર જો(cid >= 0)
    પાછા curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSC, cid);
  નહીંતર {
    પાછા curl_msnprintf(buf, maxlen, "[x-x] ");
  }
}

સ્થિર size_t trc_end_buf(અક્ષર *buf, size_t len, size_t maxlen, bool addnl)
{
  /* make sure we end the trace line in `buf` properly. It needs
   * to end with a null-terminator '\0' or '\n\0' */
  જો(len >= (maxlen - (addnl ? 2 : 1))) {
    len = maxlen - 5;
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '\n';
  }
  નહીંતર જો(addnl)
    buf[len++] = '\n';
  buf[len] = '\0';
  પાછા len;
}

શૂન્ય Curl_debug(સંરચના Curl_easy *data, curl_infotype type,
                અચલ અક્ષર *ptr, size_t size)
{
  જો(data->set.verbose) {
    સ્થિર અચલ અક્ષર s_infotype[CURLINFO_END][3] = {
      "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
    અક્ષર buf[TRC_લીટી_MAX];
    size_t len;
    જો(data->set.fdebug) {
      સંરચના Curl_mapi_guard guard;
      જો(CURL_TRC_IDS(data) && (size < TRC_લીટી_MAX)) {
        len = trc_print_ids(data, buf, TRC_લીટી_MAX);
        len += curl_msnprintf(buf + len, TRC_લીટી_MAX - len, "%.*s",
                              (પૂર્ણાંક)size, ptr);
        len = trc_end_buf(buf, len, TRC_લીટી_MAX, FALSE);
        CURL_CBAPI_START(&guard, data, easy_fdebug);
        (શૂન્ય)(*data->set.fdebug)(data, type, buf, len, data->set.debugdata);
        CURL_CBAPI_END(&guard);
      }
      નહીંતર {
        CURL_CBAPI_START(&guard, data, easy_fdebug);
        (શૂન્ય)(*data->set.fdebug)(data, type, CURL_UNCONST(ptr),
                                  size, data->set.debugdata);
        CURL_CBAPI_END(&guard);
      }
    }
    નહીંતર {
      પસંદગી(type) {
      સ્થિતિ CURLINFO_TEXT:
      સ્થિતિ CURLINFO_HEADER_OUT:
      સ્થિતિ CURLINFO_HEADER_IN:
#ifndef CURL_DISABLE_VERBOSE_STRINGS
        જો(CURL_TRC_IDS(data)) {
          len = trc_print_ids(data, buf, TRC_લીટી_MAX);
          fwrite(buf, len, 1, data->set.err);
        }
#endif
        fwrite(s_infotype[type], 2, 1, data->set.err);
        fwrite(ptr, size, 1, data->set.err);
        તોડો;
      મૂળભૂત: /* nada */
        તોડો;
      }
    }
  }
}

/* Curl_failf() is for messages stating why we failed.
 * The message SHALL NOT include any LF or CR.
 */
શૂન્ય Curl_failf(સંરચના Curl_easy *data, અચલ અક્ષર *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  જો(data->set.verbose || data->set.errorbuffer) {
    va_list ap;
    size_t len;
    અક્ષર error[CURL_ERROR_SIZE + 2];
    va_start(ap, fmt);
    len = curl_mvsnprintf(error, CURL_ERROR_SIZE, fmt, ap);

    જો(data->set.errorbuffer && !data->state.errorbuf) {
      curlx_strcopy(data->set.errorbuffer, CURL_ERROR_SIZE, error, len);
      data->state.errorbuf = TRUE; /* wrote error string */
    }
    error[len++] = '\n';
    error[len] = '\0';
    trc_write(data, CURLINFO_TEXT, error, len);
    va_end(ap);
  }
}

શૂન્ય Curl_reset_fail(સંરચના Curl_easy *data)
{
  જો(data->set.errorbuffer)
    data->set.errorbuffer[0] = 0;
  data->state.errorbuf = FALSE;
}

#ifdef CURLVERBOSE
સંરચના curl_trc_feat Curl_trc_feat_multi = {
  "MULTI",
  CURL_LOG_LVL_NONE,
};
સંરચના curl_trc_feat Curl_trc_feat_read = {
  "READ",
  CURL_LOG_LVL_NONE,
};
સંરચના curl_trc_feat Curl_trc_feat_write = {
  "WRITE",
  CURL_LOG_LVL_NONE,
};
સંરચના curl_trc_feat Curl_trc_feat_dns = {
  "DNS",
  CURL_LOG_LVL_NONE,
};
#ifndef CURL_DISABLE_DOH
સંરચના curl_trc_feat Curl_trc_feat_doh = {
  "DoH",
  CURL_LOG_LVL_NONE,
};
#endif
સંરચના curl_trc_feat Curl_trc_feat_timer = {
  "TIMER",
  CURL_LOG_LVL_NONE,
};
#ifdef USE_THREADS
સંરચના curl_trc_feat Curl_trc_feat_threads = {
  "THREADS",
  CURL_LOG_LVL_NONE,
};
#endif
#endif

#ifndef CURL_DISABLE_VERBOSE_STRINGS

સ્થિર શૂન્ય trc_infof(સંરચના Curl_easy *data,
                      સંરચના curl_trc_feat *feat,
                      અચલ અક્ષર *opt_id, પૂર્ણાંક opt_id_idx,
                      અચલ અક્ષર * અચલ fmt, va_list ap) CURL_PRINTF(5, 0);

સ્થિર શૂન્ય trc_infof(સંરચના Curl_easy *data,
                      સંરચના curl_trc_feat *feat,
                      અચલ અક્ષર *opt_id, પૂર્ણાંક opt_id_idx,
                      અચલ અક્ષર * અચલ fmt, va_list ap)
{
  size_t len = 0;
  અક્ષર buf[TRC_લીટી_MAX];

  જો(CURL_TRC_IDS(data))
    len += trc_print_ids(data, buf + len, TRC_લીટી_MAX - len);
  જો(feat)
    len += curl_msnprintf(buf + len, TRC_લીટી_MAX - len, "[%s] ", feat->name);
  જો(opt_id) {
    જો(opt_id_idx > 0)
      len += curl_msnprintf(buf + len, TRC_લીટી_MAX - len, "[%s-%d] ",
                            opt_id, opt_id_idx);
    નહીંતર
      len += curl_msnprintf(buf + len, TRC_લીટી_MAX - len, "[%s] ", opt_id);
  }
  len += curl_mvsnprintf(buf + len, TRC_લીટી_MAX - len, fmt, ap);
  len = trc_end_buf(buf, len, TRC_લીટી_MAX, TRUE);
  trc_write(data, CURLINFO_TEXT, buf, len);
}

શૂન્ય Curl_infof(સંરચના Curl_easy *data, અચલ અક્ષર *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  જો(Curl_trc_is_verbose(data)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, data->state.feat, ખાલી, 0, fmt, ap);
    va_end(ap);
  }
}

શૂન્ય Curl_trc_cf_infof(સંરચના Curl_easy *data, અચલ સંરચના Curl_cfilter *cf,
                       અચલ અક્ષર *fmt, ...)
{
  DEBUGASSERT(cf);
  જો(Curl_trc_cf_is_verbose(cf, data)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, data->state.feat, cf->cft->name, cf->sockindex, fmt, ap);
    va_end(ap);
  }
}

શૂન્ય Curl_trc_feat_infof(સંરચના Curl_easy *data,
                         સંરચના curl_trc_feat *feat,
                         અચલ અક્ષર *fmt, ...)
{
  DEBUGASSERT(feat);
  જો(Curl_trc_ft_is_verbose(data, feat)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, feat, ખાલી, 0, fmt, ap);
    va_end(ap);
  }
}

સ્થિર અચલ અક્ષર * અચલ Curl_trc_timer_names[] = {
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

સ્થિર અચલ અક્ષર *trc_timer_name(પૂર્ણાંક tid)
{
  જો((tid >= 0) && ((size_t)tid < CURL_ARRAYSIZE(Curl_trc_timer_names)))
    પાછા Curl_trc_timer_names[(size_t)tid];
  પાછા "TIMER-???";
}

શૂન્ય Curl_trc_timer(સંરચના Curl_easy *data, પૂર્ણાંક tid, અચલ અક્ષર *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  જો(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_timer)) {
    અચલ અક્ષર *tname = trc_timer_name(tid);
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_timer, tname, 0, fmt, ap);
    va_end(ap);
  }
}

શૂન્ય Curl_trc_easy_timers(સંરચના Curl_easy *data)
{
  જો(CURL_TRC_TIMER_is_verbose(data) && data->multi) {
    જો(data->state.timeouts.first < EXPIRE_LAST) {
      સંરચના expire_timers *timeouts = &data->state.timeouts;
      timediff_t base_us =
        Curl_timeouts_offset_us(&data->multi->timeouts, Curl_pgrs_now(data));
      uint8_t id = data->state.timeouts.first;
      માટે(; id < EXPIRE_LAST; id = timeouts->next[id]) {
        CURL_TRC_TIMER(data, id, "expires in %" FMT_TIMEDIFF_T "us",
                       timeouts->offset_us[id] - base_us);
      }
    }
  }
}

સ્થિર અચલ અક્ષર * અચલ Curl_trc_mstate_names[] = {
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

અચલ અક્ષર *Curl_trc_mstate_name(પૂર્ણાંક state)
{
  જો((state >= 0) && ((size_t)state < CURL_ARRAYSIZE(Curl_trc_mstate_names)))
    પાછા Curl_trc_mstate_names[(size_t)state];
  પાછા "?";
}

શૂન્ય Curl_trc_multi(સંરચના Curl_easy *data, અચલ અક્ષર *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  જો(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_multi)) {
    અચલ અક્ષર *sname = (data->id >= 0) ?
                        Curl_trc_mstate_name(data->mstate) : ખાલી;
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_multi, sname, 0, fmt, ap);
    va_end(ap);
  }
}

શૂન્ય Curl_trc_read(સંરચના Curl_easy *data, અચલ અક્ષર *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  જો(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_read)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_read, ખાલી, 0, fmt, ap);
    va_end(ap);
  }
}

શૂન્ય Curl_trc_write(સંરચના Curl_easy *data, અચલ અક્ષર *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  જો(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_write)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_write, ખાલી, 0, fmt, ap);
    va_end(ap);
  }
}

શૂન્ય Curl_trc_dns(સંરચના Curl_easy *data, અચલ અક્ષર *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  જો(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_dns)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_dns, ખાલી, 0, fmt, ap);
    va_end(ap);
  }
}

#ifndef CURL_DISABLE_FTP
સંરચના curl_trc_feat Curl_trc_feat_ftp = {
  "FTP",
  CURL_LOG_LVL_NONE,
};

શૂન્ય Curl_trc_ftp(સંરચના Curl_easy *data, અચલ અક્ષર *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  જો(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ftp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ftp, ખાલી, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* !CURL_DISABLE_FTP */

#ifndef CURL_DISABLE_SMTP
સંરચના curl_trc_feat Curl_trc_feat_smtp = {
  "SMTP",
  CURL_LOG_LVL_NONE,
};

શૂન્ય Curl_trc_smtp(સંરચના Curl_easy *data, અચલ અક્ષર *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  જો(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_smtp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_smtp, ખાલી, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* !CURL_DISABLE_SMTP */

#ifdef USE_SSL
સંરચના curl_trc_feat Curl_trc_feat_ssls = {
  "SSLS",
  CURL_LOG_LVL_NONE,
};

શૂન્ય Curl_trc_ssls(સંરચના Curl_easy *data, અચલ અક્ષર *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  જો(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssls)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ssls, ખાલી, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* USE_SSL */

#ifdef USE_SSH
સંરચના curl_trc_feat Curl_trc_feat_ssh = {
  "SSH",
  CURL_LOG_LVL_NONE,
};

શૂન્ય Curl_trc_ssh(સંરચના Curl_easy *data, અચલ અક્ષર *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  જો(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssh)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ssh, ખાલી, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* USE_SSH */

#જો !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
સંરચના curl_trc_feat Curl_trc_feat_ws = {
  "WS",
  CURL_LOG_LVL_NONE,
};

શૂન્ય Curl_trc_ws(સંરચના Curl_easy *data, અચલ અક્ષર *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  જો(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ws)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ws, ખાલી, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#define TRC_CT_NONE        0
#define TRC_CT_PROTOCOL    (1 << 0)
#define TRC_CT_NETWORK     (1 << 1)
#define TRC_CT_PROXY       (1 << 2)
#define TRC_CT_INTERNALS   (1 << 3)

સંરચના trc_feat_def {
  સંરચના curl_trc_feat *feat;
  unsigned પૂર્ણાંક category;
};

સ્થિર સંરચના trc_feat_def trc_feats[] = {
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
#જો !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
  { &Curl_trc_feat_ws,        TRC_CT_PROTOCOL },
#endif
};

સંરચના trc_cft_def {
  સંરચના Curl_cftype *cft;
  unsigned પૂર્ણાંક category;
};

સ્થિર સંરચના trc_cft_def trc_cfts[] = {
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
#જો !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
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
#જો defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3)
  { &Curl_cft_h3_proxy,       TRC_CT_PROXY },
#endif
  { &Curl_cft_http_proxy,     TRC_CT_PROXY },
#endif /* !CURL_DISABLE_HTTP */
  { &Curl_cft_haproxy,        TRC_CT_PROXY },
  { &Curl_cft_socks_proxy,    TRC_CT_PROXY },
#endif /* !CURL_DISABLE_PROXY */
#જો !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { &Curl_cft_http3,          TRC_CT_PROTOCOL },
#endif
#ifndef CURL_DISABLE_HTTP
  { &Curl_cft_http_connect,   TRC_CT_PROTOCOL },
#endif
};

સ્થિર શૂન્ય trc_apply_level_by_name(સંરચના Curl_str *token, પૂર્ણાંક lvl)
{
  size_t i;

  માટે(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    જો(curlx_str_casecompare(token, trc_cfts[i].cft->name)) {
      trc_cfts[i].cft->log_level = lvl;
      તોડો;
    }
  }
  માટે(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    જો(curlx_str_casecompare(token, trc_feats[i].feat->name)) {
      trc_feats[i].feat->log_level = lvl;
      તોડો;
    }
  }
}

સ્થિર શૂન્ય trc_apply_level_by_category(unsigned પૂર્ણાંક category, પૂર્ણાંક lvl)
{
  size_t i;

  માટે(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    જો(!category || (trc_cfts[i].category & category))
      trc_cfts[i].cft->log_level = lvl;
  }
  માટે(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    જો(!category || (trc_feats[i].category & category))
      trc_feats[i].feat->log_level = lvl;
  }
}

સ્થિર CURLcode trc_opt(અચલ અક્ષર *config)
{
  સંરચના Curl_str out;
  જ્યારે(!curlx_str_until(&config, &out, 32, ',')) {
    પૂર્ણાંક lvl = CURL_LOG_LVL_INFO;
    અચલ અક્ષર *token = curlx_str(&out);

    જો(*token == '-') {
      lvl = CURL_LOG_LVL_NONE;
      curlx_str_nudge(&out, 1);
    }
    નહીંતર જો(*token == '+')
      curlx_str_nudge(&out, 1);

    જો(curlx_str_casecompare(&out, "all"))
      trc_apply_level_by_category(TRC_CT_NONE, lvl);
    નહીંતર જો(curlx_str_casecompare(&out, "protocol"))
      trc_apply_level_by_category(TRC_CT_PROTOCOL, lvl);
    નહીંતર જો(curlx_str_casecompare(&out, "network"))
      trc_apply_level_by_category(TRC_CT_NETWORK, lvl);
    નહીંતર જો(curlx_str_casecompare(&out, "proxy"))
      trc_apply_level_by_category(TRC_CT_PROXY, lvl);
    નહીંતર
      trc_apply_level_by_name(&out, lvl);

    જો(curlx_str_single(&config, ','))
      તોડો;
  }
  પાછા CURLE_OK;
}

CURLcode Curl_trc_opt(અચલ અક્ષર *config)
{
  CURLcode result = config ? trc_opt(config) : CURLE_OK;
#ifdef DEBUGBUILD
  /* CURL_DEBUG can override anything */
  જો(!result) {
    અચલ અક્ષર *dbg_config = getenv("CURL_DEBUG");
    જો(dbg_config)
      result = trc_opt(dbg_config);
  }
#endif /* DEBUGBUILD */
  પાછા result;
}

CURLcode Curl_trc_init(શૂન્ય)
{
#ifdef DEBUGBUILD
  પાછા Curl_trc_opt(ખાલી);
#નહીંતર
  પાછા CURLE_OK;
#endif
}

#નહીંતર /* CURL_DISABLE_VERBOSE_STRINGS */

CURLcode Curl_trc_init(શૂન્ય)
{
  પાછા CURLE_OK;
}

શૂન્ય Curl_infof(સંરચના Curl_easy *data, અચલ અક્ષર *fmt, ...)
{
  (શૂન્ય)data;
  (શૂન્ય)fmt;
}

શૂન્ય Curl_trc_cf_infof(સંરચના Curl_easy *data, અચલ સંરચના Curl_cfilter *cf,
                       અચલ અક્ષર *fmt, ...)
{
  (શૂન્ય)data;
  (શૂન્ય)cf;
  (શૂન્ય)fmt;
}

શૂન્ય Curl_trc_multi(સંરચના Curl_easy *data, અચલ અક્ષર *fmt, ...)
{
  (શૂન્ય)data;
  (શૂન્ય)fmt;
}

શૂન્ય Curl_trc_write(સંરચના Curl_easy *data, અચલ અક્ષર *fmt, ...)
{
  (શૂન્ય)data;
  (શૂન્ય)fmt;
}

શૂન્ય Curl_trc_dns(સંરચના Curl_easy *data, અચલ અક્ષર *fmt, ...)
{
  (શૂન્ય)data;
  (શૂન્ય)fmt;
}

શૂન્ય Curl_trc_timer(સંરચના Curl_easy *data, પૂર્ણાંક tid, અચલ અક્ષર *fmt, ...)
{
  (શૂન્ય)data;
  (શૂન્ય)tid;
  (શૂન્ય)fmt;
}

શૂન્ય Curl_trc_read(સંરચના Curl_easy *data, અચલ અક્ષર *fmt, ...)
{
  (શૂન્ય)data;
  (શૂન્ય)fmt;
}

#ifndef CURL_DISABLE_FTP
શૂન્ય Curl_trc_ftp(સંરચના Curl_easy *data, અચલ અક્ષર *fmt, ...)
{
  (શૂન્ય)data;
  (શૂન્ય)fmt;
}
#endif
#ifndef CURL_DISABLE_SMTP
શૂન્ય Curl_trc_smtp(સંરચના Curl_easy *data, અચલ અક્ષર *fmt, ...)
{
  (શૂન્ય)data;
  (શૂન્ય)fmt;
}
#endif
#જો !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
શૂન્ય Curl_trc_ws(સંરચના Curl_easy *data, અચલ અક્ષર *fmt, ...)
{
  (શૂન્ય)data;
  (શૂન્ય)fmt;
}
#endif
#ifdef USE_SSH
શૂન્ય Curl_trc_ssh(સંરચના Curl_easy *data, અચલ અક્ષર *fmt, ...)
{
  (શૂન્ય)data;
  (શૂન્ય)fmt;
}
#endif
#ifdef USE_SSL
શૂન્ય Curl_trc_ssls(સંરચના Curl_easy *data, અચલ અક્ષર *fmt, ...)
{
  (શૂન્ય)data;
  (શૂન્ય)fmt;
}
#endif

#endif /* !CURL_DISABLE_VERBOSE_STRINGS */
