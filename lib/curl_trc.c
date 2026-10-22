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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "multiif.h"

#அந்தர்பாவ "cf-recvbuf.h"
#அந்தர்பாவ "cf-socket.h"
#அந்தர்பாவ "cf-setup.h"
#அந்தர்பாவ "http2.h"
#அந்தர்பாவ "http_proxy.h"
#அந்தர்பாவ "cf-h1-proxy.h"
#அந்தர்பாவ "cf-h2-proxy.h"
#அந்தர்பாவ "cf-haproxy.h"
#அந்தர்பாவ "cf-https-connect.h"
#அந்தர்பாவ "cf-ip-happy.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "socks.h"
#அந்தர்பாவ "curlx/strparse.h"
#அந்தர்பாவ "vdns/cf-dns.h"
#அந்தர்பாவ "vtls/vtls.h"
#அந்தர்பாவ "vquic/vquic.h"
#அந்தர்பாவ "curlx/strcopy.h"

நிலையான வெறுமை trc_write(கட்டமைப்பு Curl_easy *data, curl_infotype type,
                      மாறிலி எழுத்து *ptr, size_t size)
{
  எனில்(data->set.verbose) {
    எனில்(data->set.fdebug) {
      கட்டமைப்பு Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_fdebug);
      (வெறுமை)(*data->set.fdebug)(data, type, CURL_UNCONST(ptr), size,
                                data->set.debugdata);
      CURL_CBAPI_END(&guard);
    }
    இல்லையெனில் {
      நிலையான மாறிலி எழுத்து s_infotype[CURLINFO_END][3] = {
        "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
      தேர்வு(type) {
      நிலை CURLINFO_TEXT:
      நிலை CURLINFO_HEADER_OUT:
      நிலை CURLINFO_HEADER_IN:
        fwrite(s_infotype[type], 2, 1, data->set.err);
        fwrite(ptr, size, 1, data->set.err);
        நிறுத்து;
      இயல்பு: /* nada */
        நிறுத்து;
      }
    }
  }
}

/* max length we trace before ending in '...' */
#define TRC_வரி_MAX 2048

#define CURL_TRC_FMT_IDSC   "[x-%" CURL_FORMAT_CURL_OFF_T "] "
#define CURL_TRC_FMT_IDSD   "[%" CURL_FORMAT_CURL_OFF_T "-x] "
#define CURL_TRC_FMT_IDSDC  "[%" CURL_FORMAT_CURL_OFF_T "-%" \
                            CURL_FORMAT_CURL_OFF_T "] "

நிலையான கட்டமைப்பு curl_trc_feat Curl_trc_feat_ids = {
  "LIB-IDS",
  CURL_LOG_LVL_NONE,
};
#define CURL_TRC_IDS(data) \
  (Curl_trc_is_verbose(data) && \
  Curl_trc_feat_ids.log_level >= CURL_LOG_LVL_INFO)

நிலையான size_t trc_print_ids(கட்டமைப்பு Curl_easy *data, எழுத்து *buf, size_t maxlen)
{
  curl_off_t cid = data->state.last_conn_id;
  எனில்(data->id >= 0) {
    எனில்(cid >= 0)
      திரும்பு curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSDC, data->id, cid);
    இல்லையெனில்
      திரும்பு curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSD, data->id);
  }
  இல்லையெனில் எனில்(cid >= 0)
    திரும்பு curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSC, cid);
  இல்லையெனில் {
    திரும்பு curl_msnprintf(buf, maxlen, "[x-x] ");
  }
}

நிலையான size_t trc_end_buf(எழுத்து *buf, size_t len, size_t maxlen, bool addnl)
{
  /* make sure we end the trace line in `buf` properly. It needs
   * to end with a null-terminator '\0' or '\n\0' */
  எனில்(len >= (maxlen - (addnl ? 2 : 1))) {
    len = maxlen - 5;
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '\n';
  }
  இல்லையெனில் எனில்(addnl)
    buf[len++] = '\n';
  buf[len] = '\0';
  திரும்பு len;
}

வெறுமை Curl_debug(கட்டமைப்பு Curl_easy *data, curl_infotype type,
                மாறிலி எழுத்து *ptr, size_t size)
{
  எனில்(data->set.verbose) {
    நிலையான மாறிலி எழுத்து s_infotype[CURLINFO_END][3] = {
      "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
    எழுத்து buf[TRC_வரி_MAX];
    size_t len;
    எனில்(data->set.fdebug) {
      கட்டமைப்பு Curl_mapi_guard guard;
      எனில்(CURL_TRC_IDS(data) && (size < TRC_வரி_MAX)) {
        len = trc_print_ids(data, buf, TRC_வரி_MAX);
        len += curl_msnprintf(buf + len, TRC_வரி_MAX - len, "%.*s",
                              (முழு)size, ptr);
        len = trc_end_buf(buf, len, TRC_வரி_MAX, FALSE);
        CURL_CBAPI_START(&guard, data, easy_fdebug);
        (வெறுமை)(*data->set.fdebug)(data, type, buf, len, data->set.debugdata);
        CURL_CBAPI_END(&guard);
      }
      இல்லையெனில் {
        CURL_CBAPI_START(&guard, data, easy_fdebug);
        (வெறுமை)(*data->set.fdebug)(data, type, CURL_UNCONST(ptr),
                                  size, data->set.debugdata);
        CURL_CBAPI_END(&guard);
      }
    }
    இல்லையெனில் {
      தேர்வு(type) {
      நிலை CURLINFO_TEXT:
      நிலை CURLINFO_HEADER_OUT:
      நிலை CURLINFO_HEADER_IN:
#ifndef CURL_DISABLE_VERBOSE_STRINGS
        எனில்(CURL_TRC_IDS(data)) {
          len = trc_print_ids(data, buf, TRC_வரி_MAX);
          fwrite(buf, len, 1, data->set.err);
        }
#endif
        fwrite(s_infotype[type], 2, 1, data->set.err);
        fwrite(ptr, size, 1, data->set.err);
        நிறுத்து;
      இயல்பு: /* nada */
        நிறுத்து;
      }
    }
  }
}

/* Curl_failf() is for messages stating why we failed.
 * The message SHALL NOT include any LF or CR.
 */
வெறுமை Curl_failf(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  எனில்(data->set.verbose || data->set.errorbuffer) {
    va_list ap;
    size_t len;
    எழுத்து error[CURL_ERROR_SIZE + 2];
    va_start(ap, fmt);
    len = curl_mvsnprintf(error, CURL_ERROR_SIZE, fmt, ap);

    எனில்(data->set.errorbuffer && !data->state.errorbuf) {
      curlx_strcopy(data->set.errorbuffer, CURL_ERROR_SIZE, error, len);
      data->state.errorbuf = TRUE; /* wrote error string */
    }
    error[len++] = '\n';
    error[len] = '\0';
    trc_write(data, CURLINFO_TEXT, error, len);
    va_end(ap);
  }
}

வெறுமை Curl_reset_fail(கட்டமைப்பு Curl_easy *data)
{
  எனில்(data->set.errorbuffer)
    data->set.errorbuffer[0] = 0;
  data->state.errorbuf = FALSE;
}

#ifdef CURLVERBOSE
கட்டமைப்பு curl_trc_feat Curl_trc_feat_multi = {
  "MULTI",
  CURL_LOG_LVL_NONE,
};
கட்டமைப்பு curl_trc_feat Curl_trc_feat_read = {
  "READ",
  CURL_LOG_LVL_NONE,
};
கட்டமைப்பு curl_trc_feat Curl_trc_feat_write = {
  "WRITE",
  CURL_LOG_LVL_NONE,
};
கட்டமைப்பு curl_trc_feat Curl_trc_feat_dns = {
  "DNS",
  CURL_LOG_LVL_NONE,
};
#ifndef CURL_DISABLE_DOH
கட்டமைப்பு curl_trc_feat Curl_trc_feat_doh = {
  "DoH",
  CURL_LOG_LVL_NONE,
};
#endif
கட்டமைப்பு curl_trc_feat Curl_trc_feat_timer = {
  "TIMER",
  CURL_LOG_LVL_NONE,
};
#ifdef USE_THREADS
கட்டமைப்பு curl_trc_feat Curl_trc_feat_threads = {
  "THREADS",
  CURL_LOG_LVL_NONE,
};
#endif
#endif

#ifndef CURL_DISABLE_VERBOSE_STRINGS

நிலையான வெறுமை trc_infof(கட்டமைப்பு Curl_easy *data,
                      கட்டமைப்பு curl_trc_feat *feat,
                      மாறிலி எழுத்து *opt_id, முழு opt_id_idx,
                      மாறிலி எழுத்து * மாறிலி fmt, va_list ap) CURL_PRINTF(5, 0);

நிலையான வெறுமை trc_infof(கட்டமைப்பு Curl_easy *data,
                      கட்டமைப்பு curl_trc_feat *feat,
                      மாறிலி எழுத்து *opt_id, முழு opt_id_idx,
                      மாறிலி எழுத்து * மாறிலி fmt, va_list ap)
{
  size_t len = 0;
  எழுத்து buf[TRC_வரி_MAX];

  எனில்(CURL_TRC_IDS(data))
    len += trc_print_ids(data, buf + len, TRC_வரி_MAX - len);
  எனில்(feat)
    len += curl_msnprintf(buf + len, TRC_வரி_MAX - len, "[%s] ", feat->name);
  எனில்(opt_id) {
    எனில்(opt_id_idx > 0)
      len += curl_msnprintf(buf + len, TRC_வரி_MAX - len, "[%s-%d] ",
                            opt_id, opt_id_idx);
    இல்லையெனில்
      len += curl_msnprintf(buf + len, TRC_வரி_MAX - len, "[%s] ", opt_id);
  }
  len += curl_mvsnprintf(buf + len, TRC_வரி_MAX - len, fmt, ap);
  len = trc_end_buf(buf, len, TRC_வரி_MAX, TRUE);
  trc_write(data, CURLINFO_TEXT, buf, len);
}

வெறுமை Curl_infof(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  எனில்(Curl_trc_is_verbose(data)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, data->state.feat, வெற்று, 0, fmt, ap);
    va_end(ap);
  }
}

வெறுமை Curl_trc_cf_infof(கட்டமைப்பு Curl_easy *data, மாறிலி கட்டமைப்பு Curl_cfilter *cf,
                       மாறிலி எழுத்து *fmt, ...)
{
  DEBUGASSERT(cf);
  எனில்(Curl_trc_cf_is_verbose(cf, data)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, data->state.feat, cf->cft->name, cf->sockindex, fmt, ap);
    va_end(ap);
  }
}

வெறுமை Curl_trc_feat_infof(கட்டமைப்பு Curl_easy *data,
                         கட்டமைப்பு curl_trc_feat *feat,
                         மாறிலி எழுத்து *fmt, ...)
{
  DEBUGASSERT(feat);
  எனில்(Curl_trc_ft_is_verbose(data, feat)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, feat, வெற்று, 0, fmt, ap);
    va_end(ap);
  }
}

நிலையான மாறிலி எழுத்து * மாறிலி Curl_trc_timer_names[] = {
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

நிலையான மாறிலி எழுத்து *trc_timer_name(முழு tid)
{
  எனில்((tid >= 0) && ((size_t)tid < CURL_ARRAYSIZE(Curl_trc_timer_names)))
    திரும்பு Curl_trc_timer_names[(size_t)tid];
  திரும்பு "TIMER-???";
}

வெறுமை Curl_trc_timer(கட்டமைப்பு Curl_easy *data, முழு tid, மாறிலி எழுத்து *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  எனில்(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_timer)) {
    மாறிலி எழுத்து *tname = trc_timer_name(tid);
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_timer, tname, 0, fmt, ap);
    va_end(ap);
  }
}

வெறுமை Curl_trc_easy_timers(கட்டமைப்பு Curl_easy *data)
{
  எனில்(CURL_TRC_TIMER_is_verbose(data) && data->multi) {
    எனில்(data->state.timeouts.first < EXPIRE_LAST) {
      கட்டமைப்பு expire_timers *timeouts = &data->state.timeouts;
      timediff_t base_us =
        Curl_timeouts_offset_us(&data->multi->timeouts, Curl_pgrs_now(data));
      uint8_t id = data->state.timeouts.first;
      க்கு(; id < EXPIRE_LAST; id = timeouts->next[id]) {
        CURL_TRC_TIMER(data, id, "expires in %" FMT_TIMEDIFF_T "us",
                       timeouts->offset_us[id] - base_us);
      }
    }
  }
}

நிலையான மாறிலி எழுத்து * மாறிலி Curl_trc_mstate_names[] = {
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

மாறிலி எழுத்து *Curl_trc_mstate_name(முழு state)
{
  எனில்((state >= 0) && ((size_t)state < CURL_ARRAYSIZE(Curl_trc_mstate_names)))
    திரும்பு Curl_trc_mstate_names[(size_t)state];
  திரும்பு "?";
}

வெறுமை Curl_trc_multi(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  எனில்(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_multi)) {
    மாறிலி எழுத்து *sname = (data->id >= 0) ?
                        Curl_trc_mstate_name(data->mstate) : வெற்று;
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_multi, sname, 0, fmt, ap);
    va_end(ap);
  }
}

வெறுமை Curl_trc_read(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  எனில்(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_read)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_read, வெற்று, 0, fmt, ap);
    va_end(ap);
  }
}

வெறுமை Curl_trc_write(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  எனில்(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_write)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_write, வெற்று, 0, fmt, ap);
    va_end(ap);
  }
}

வெறுமை Curl_trc_dns(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  எனில்(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_dns)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_dns, வெற்று, 0, fmt, ap);
    va_end(ap);
  }
}

#ifndef CURL_DISABLE_FTP
கட்டமைப்பு curl_trc_feat Curl_trc_feat_ftp = {
  "FTP",
  CURL_LOG_LVL_NONE,
};

வெறுமை Curl_trc_ftp(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  எனில்(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ftp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ftp, வெற்று, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* !CURL_DISABLE_FTP */

#ifndef CURL_DISABLE_SMTP
கட்டமைப்பு curl_trc_feat Curl_trc_feat_smtp = {
  "SMTP",
  CURL_LOG_LVL_NONE,
};

வெறுமை Curl_trc_smtp(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  எனில்(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_smtp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_smtp, வெற்று, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* !CURL_DISABLE_SMTP */

#ifdef USE_SSL
கட்டமைப்பு curl_trc_feat Curl_trc_feat_ssls = {
  "SSLS",
  CURL_LOG_LVL_NONE,
};

வெறுமை Curl_trc_ssls(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  எனில்(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssls)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ssls, வெற்று, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* USE_SSL */

#ifdef USE_SSH
கட்டமைப்பு curl_trc_feat Curl_trc_feat_ssh = {
  "SSH",
  CURL_LOG_LVL_NONE,
};

வெறுமை Curl_trc_ssh(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  எனில்(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssh)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ssh, வெற்று, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* USE_SSH */

#எனில் !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
கட்டமைப்பு curl_trc_feat Curl_trc_feat_ws = {
  "WS",
  CURL_LOG_LVL_NONE,
};

வெறுமை Curl_trc_ws(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  எனில்(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ws)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ws, வெற்று, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#define TRC_CT_NONE        0
#define TRC_CT_PROTOCOL    (1 << 0)
#define TRC_CT_NETWORK     (1 << 1)
#define TRC_CT_PROXY       (1 << 2)
#define TRC_CT_INTERNALS   (1 << 3)

கட்டமைப்பு trc_feat_def {
  கட்டமைப்பு curl_trc_feat *feat;
  unsigned முழு category;
};

நிலையான கட்டமைப்பு trc_feat_def trc_feats[] = {
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
#எனில் !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
  { &Curl_trc_feat_ws,        TRC_CT_PROTOCOL },
#endif
};

கட்டமைப்பு trc_cft_def {
  கட்டமைப்பு Curl_cftype *cft;
  unsigned முழு category;
};

நிலையான கட்டமைப்பு trc_cft_def trc_cfts[] = {
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
#எனில் !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
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
#எனில் defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3)
  { &Curl_cft_h3_proxy,       TRC_CT_PROXY },
#endif
  { &Curl_cft_http_proxy,     TRC_CT_PROXY },
#endif /* !CURL_DISABLE_HTTP */
  { &Curl_cft_haproxy,        TRC_CT_PROXY },
  { &Curl_cft_socks_proxy,    TRC_CT_PROXY },
#endif /* !CURL_DISABLE_PROXY */
#எனில் !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { &Curl_cft_http3,          TRC_CT_PROTOCOL },
#endif
#ifndef CURL_DISABLE_HTTP
  { &Curl_cft_http_connect,   TRC_CT_PROTOCOL },
#endif
};

நிலையான வெறுமை trc_apply_level_by_name(கட்டமைப்பு Curl_str *token, முழு lvl)
{
  size_t i;

  க்கு(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    எனில்(curlx_str_casecompare(token, trc_cfts[i].cft->name)) {
      trc_cfts[i].cft->log_level = lvl;
      நிறுத்து;
    }
  }
  க்கு(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    எனில்(curlx_str_casecompare(token, trc_feats[i].feat->name)) {
      trc_feats[i].feat->log_level = lvl;
      நிறுத்து;
    }
  }
}

நிலையான வெறுமை trc_apply_level_by_category(unsigned முழு category, முழு lvl)
{
  size_t i;

  க்கு(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    எனில்(!category || (trc_cfts[i].category & category))
      trc_cfts[i].cft->log_level = lvl;
  }
  க்கு(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    எனில்(!category || (trc_feats[i].category & category))
      trc_feats[i].feat->log_level = lvl;
  }
}

நிலையான CURLcode trc_opt(மாறிலி எழுத்து *config)
{
  கட்டமைப்பு Curl_str out;
  போது(!curlx_str_until(&config, &out, 32, ',')) {
    முழு lvl = CURL_LOG_LVL_INFO;
    மாறிலி எழுத்து *token = curlx_str(&out);

    எனில்(*token == '-') {
      lvl = CURL_LOG_LVL_NONE;
      curlx_str_nudge(&out, 1);
    }
    இல்லையெனில் எனில்(*token == '+')
      curlx_str_nudge(&out, 1);

    எனில்(curlx_str_casecompare(&out, "all"))
      trc_apply_level_by_category(TRC_CT_NONE, lvl);
    இல்லையெனில் எனில்(curlx_str_casecompare(&out, "protocol"))
      trc_apply_level_by_category(TRC_CT_PROTOCOL, lvl);
    இல்லையெனில் எனில்(curlx_str_casecompare(&out, "network"))
      trc_apply_level_by_category(TRC_CT_NETWORK, lvl);
    இல்லையெனில் எனில்(curlx_str_casecompare(&out, "proxy"))
      trc_apply_level_by_category(TRC_CT_PROXY, lvl);
    இல்லையெனில்
      trc_apply_level_by_name(&out, lvl);

    எனில்(curlx_str_single(&config, ','))
      நிறுத்து;
  }
  திரும்பு CURLE_OK;
}

CURLcode Curl_trc_opt(மாறிலி எழுத்து *config)
{
  CURLcode result = config ? trc_opt(config) : CURLE_OK;
#ifdef DEBUGBUILD
  /* CURL_DEBUG can override anything */
  எனில்(!result) {
    மாறிலி எழுத்து *dbg_config = getenv("CURL_DEBUG");
    எனில்(dbg_config)
      result = trc_opt(dbg_config);
  }
#endif /* DEBUGBUILD */
  திரும்பு result;
}

CURLcode Curl_trc_init(வெறுமை)
{
#ifdef DEBUGBUILD
  திரும்பு Curl_trc_opt(வெற்று);
#இல்லையெனில்
  திரும்பு CURLE_OK;
#endif
}

#இல்லையெனில் /* CURL_DISABLE_VERBOSE_STRINGS */

CURLcode Curl_trc_init(வெறுமை)
{
  திரும்பு CURLE_OK;
}

வெறுமை Curl_infof(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *fmt, ...)
{
  (வெறுமை)data;
  (வெறுமை)fmt;
}

வெறுமை Curl_trc_cf_infof(கட்டமைப்பு Curl_easy *data, மாறிலி கட்டமைப்பு Curl_cfilter *cf,
                       மாறிலி எழுத்து *fmt, ...)
{
  (வெறுமை)data;
  (வெறுமை)cf;
  (வெறுமை)fmt;
}

வெறுமை Curl_trc_multi(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *fmt, ...)
{
  (வெறுமை)data;
  (வெறுமை)fmt;
}

வெறுமை Curl_trc_write(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *fmt, ...)
{
  (வெறுமை)data;
  (வெறுமை)fmt;
}

வெறுமை Curl_trc_dns(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *fmt, ...)
{
  (வெறுமை)data;
  (வெறுமை)fmt;
}

வெறுமை Curl_trc_timer(கட்டமைப்பு Curl_easy *data, முழு tid, மாறிலி எழுத்து *fmt, ...)
{
  (வெறுமை)data;
  (வெறுமை)tid;
  (வெறுமை)fmt;
}

வெறுமை Curl_trc_read(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *fmt, ...)
{
  (வெறுமை)data;
  (வெறுமை)fmt;
}

#ifndef CURL_DISABLE_FTP
வெறுமை Curl_trc_ftp(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *fmt, ...)
{
  (வெறுமை)data;
  (வெறுமை)fmt;
}
#endif
#ifndef CURL_DISABLE_SMTP
வெறுமை Curl_trc_smtp(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *fmt, ...)
{
  (வெறுமை)data;
  (வெறுமை)fmt;
}
#endif
#எனில் !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
வெறுமை Curl_trc_ws(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *fmt, ...)
{
  (வெறுமை)data;
  (வெறுமை)fmt;
}
#endif
#ifdef USE_SSH
வெறுமை Curl_trc_ssh(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *fmt, ...)
{
  (வெறுமை)data;
  (வெறுமை)fmt;
}
#endif
#ifdef USE_SSL
வெறுமை Curl_trc_ssls(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *fmt, ...)
{
  (வெறுமை)data;
  (வெறுமை)fmt;
}
#endif

#endif /* !CURL_DISABLE_VERBOSE_STRINGS */
