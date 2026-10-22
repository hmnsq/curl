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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"

#ಅನ್ತರ್ಭಾವ "cf-recvbuf.h"
#ಅನ್ತರ್ಭಾವ "cf-socket.h"
#ಅನ್ತರ್ಭಾವ "cf-setup.h"
#ಅನ್ತರ್ಭಾವ "http2.h"
#ಅನ್ತರ್ಭಾವ "http_proxy.h"
#ಅನ್ತರ್ಭಾವ "cf-h1-proxy.h"
#ಅನ್ತರ್ಭಾವ "cf-h2-proxy.h"
#ಅನ್ತರ್ಭಾವ "cf-haproxy.h"
#ಅನ್ತರ್ಭಾವ "cf-https-connect.h"
#ಅನ್ತರ್ಭಾವ "cf-ip-happy.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "socks.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "vdns/cf-dns.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "vquic/vquic.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"

ಸ್ಥಿರ ಶೂನ್ಯ trc_write(ಸಂರಚನಾ Curl_easy *data, curl_infotype type,
                      ಧ್ರುವ ವರ್ಣ *ptr, size_t size)
{
  ಯದಿ(data->set.verbose) {
    ಯದಿ(data->set.fdebug) {
      ಸಂರಚನಾ Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_fdebug);
      (ಶೂನ್ಯ)(*data->set.fdebug)(data, type, CURL_UNCONST(ptr), size,
                                data->set.debugdata);
      CURL_CBAPI_END(&guard);
    }
    ಅನ್ಯಥಾ {
      ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ s_infotype[CURLINFO_END][3] = {
        "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
      ವಿಕಲ್ಪ(type) {
      ಪಕ್ಷ CURLINFO_TEXT:
      ಪಕ್ಷ CURLINFO_HEADER_OUT:
      ಪಕ್ಷ CURLINFO_HEADER_IN:
        fwrite(s_infotype[type], 2, 1, data->set.err);
        fwrite(ptr, size, 1, data->set.err);
        ಭಙ್ಗ;
      ಸ್ವಭಾವ: /* nada */
        ಭಙ್ಗ;
      }
    }
  }
}

/* max length we trace before ending in '...' */
#define TRC_ಸಾಲು_MAX 2048

#define CURL_TRC_FMT_IDSC   "[x-%" CURL_FORMAT_CURL_OFF_T "] "
#define CURL_TRC_FMT_IDSD   "[%" CURL_FORMAT_CURL_OFF_T "-x] "
#define CURL_TRC_FMT_IDSDC  "[%" CURL_FORMAT_CURL_OFF_T "-%" \
                            CURL_FORMAT_CURL_OFF_T "] "

ಸ್ಥಿರ ಸಂರಚನಾ curl_trc_feat Curl_trc_feat_ids = {
  "LIB-IDS",
  CURL_LOG_LVL_NONE,
};
#define CURL_TRC_IDS(data) \
  (Curl_trc_is_verbose(data) && \
  Curl_trc_feat_ids.log_level >= CURL_LOG_LVL_INFO)

ಸ್ಥಿರ size_t trc_print_ids(ಸಂರಚನಾ Curl_easy *data, ವರ್ಣ *buf, size_t maxlen)
{
  curl_off_t cid = data->state.last_conn_id;
  ಯದಿ(data->id >= 0) {
    ಯದಿ(cid >= 0)
      ಪ್ರತ್ಯಾಗಮ curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSDC, data->id, cid);
    ಅನ್ಯಥಾ
      ಪ್ರತ್ಯಾಗಮ curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSD, data->id);
  }
  ಅನ್ಯಥಾ ಯದಿ(cid >= 0)
    ಪ್ರತ್ಯಾಗಮ curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSC, cid);
  ಅನ್ಯಥಾ {
    ಪ್ರತ್ಯಾಗಮ curl_msnprintf(buf, maxlen, "[x-x] ");
  }
}

ಸ್ಥಿರ size_t trc_end_buf(ವರ್ಣ *buf, size_t len, size_t maxlen, bool addnl)
{
  /* make sure we end the trace line in `buf` properly. It needs
   * to end with a null-terminator '\0' or '\n\0' */
  ಯದಿ(len >= (maxlen - (addnl ? 2 : 1))) {
    len = maxlen - 5;
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '\n';
  }
  ಅನ್ಯಥಾ ಯದಿ(addnl)
    buf[len++] = '\n';
  buf[len] = '\0';
  ಪ್ರತ್ಯಾಗಮ len;
}

ಶೂನ್ಯ Curl_debug(ಸಂರಚನಾ Curl_easy *data, curl_infotype type,
                ಧ್ರುವ ವರ್ಣ *ptr, size_t size)
{
  ಯದಿ(data->set.verbose) {
    ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ s_infotype[CURLINFO_END][3] = {
      "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
    ವರ್ಣ buf[TRC_ಸಾಲು_MAX];
    size_t len;
    ಯದಿ(data->set.fdebug) {
      ಸಂರಚನಾ Curl_mapi_guard guard;
      ಯದಿ(CURL_TRC_IDS(data) && (size < TRC_ಸಾಲು_MAX)) {
        len = trc_print_ids(data, buf, TRC_ಸಾಲು_MAX);
        len += curl_msnprintf(buf + len, TRC_ಸಾಲು_MAX - len, "%.*s",
                              (ಪೂರ್ಣಾಙ್ಕ)size, ptr);
        len = trc_end_buf(buf, len, TRC_ಸಾಲು_MAX, FALSE);
        CURL_CBAPI_START(&guard, data, easy_fdebug);
        (ಶೂನ್ಯ)(*data->set.fdebug)(data, type, buf, len, data->set.debugdata);
        CURL_CBAPI_END(&guard);
      }
      ಅನ್ಯಥಾ {
        CURL_CBAPI_START(&guard, data, easy_fdebug);
        (ಶೂನ್ಯ)(*data->set.fdebug)(data, type, CURL_UNCONST(ptr),
                                  size, data->set.debugdata);
        CURL_CBAPI_END(&guard);
      }
    }
    ಅನ್ಯಥಾ {
      ವಿಕಲ್ಪ(type) {
      ಪಕ್ಷ CURLINFO_TEXT:
      ಪಕ್ಷ CURLINFO_HEADER_OUT:
      ಪಕ್ಷ CURLINFO_HEADER_IN:
#ifndef CURL_DISABLE_VERBOSE_STRINGS
        ಯದಿ(CURL_TRC_IDS(data)) {
          len = trc_print_ids(data, buf, TRC_ಸಾಲು_MAX);
          fwrite(buf, len, 1, data->set.err);
        }
#endif
        fwrite(s_infotype[type], 2, 1, data->set.err);
        fwrite(ptr, size, 1, data->set.err);
        ಭಙ್ಗ;
      ಸ್ವಭಾವ: /* nada */
        ಭಙ್ಗ;
      }
    }
  }
}

/* Curl_failf() is for messages stating why we failed.
 * The message SHALL NOT include any LF or CR.
 */
ಶೂನ್ಯ Curl_failf(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ಯದಿ(data->set.verbose || data->set.errorbuffer) {
    va_list ap;
    size_t len;
    ವರ್ಣ error[CURL_ERROR_SIZE + 2];
    va_start(ap, fmt);
    len = curl_mvsnprintf(error, CURL_ERROR_SIZE, fmt, ap);

    ಯದಿ(data->set.errorbuffer && !data->state.errorbuf) {
      curlx_strcopy(data->set.errorbuffer, CURL_ERROR_SIZE, error, len);
      data->state.errorbuf = TRUE; /* wrote error string */
    }
    error[len++] = '\n';
    error[len] = '\0';
    trc_write(data, CURLINFO_TEXT, error, len);
    va_end(ap);
  }
}

ಶೂನ್ಯ Curl_reset_fail(ಸಂರಚನಾ Curl_easy *data)
{
  ಯದಿ(data->set.errorbuffer)
    data->set.errorbuffer[0] = 0;
  data->state.errorbuf = FALSE;
}

#ifdef CURLVERBOSE
ಸಂರಚನಾ curl_trc_feat Curl_trc_feat_multi = {
  "MULTI",
  CURL_LOG_LVL_NONE,
};
ಸಂರಚನಾ curl_trc_feat Curl_trc_feat_read = {
  "READ",
  CURL_LOG_LVL_NONE,
};
ಸಂರಚನಾ curl_trc_feat Curl_trc_feat_write = {
  "WRITE",
  CURL_LOG_LVL_NONE,
};
ಸಂರಚನಾ curl_trc_feat Curl_trc_feat_dns = {
  "DNS",
  CURL_LOG_LVL_NONE,
};
#ifndef CURL_DISABLE_DOH
ಸಂರಚನಾ curl_trc_feat Curl_trc_feat_doh = {
  "DoH",
  CURL_LOG_LVL_NONE,
};
#endif
ಸಂರಚನಾ curl_trc_feat Curl_trc_feat_timer = {
  "TIMER",
  CURL_LOG_LVL_NONE,
};
#ifdef USE_THREADS
ಸಂರಚನಾ curl_trc_feat Curl_trc_feat_threads = {
  "THREADS",
  CURL_LOG_LVL_NONE,
};
#endif
#endif

#ifndef CURL_DISABLE_VERBOSE_STRINGS

ಸ್ಥಿರ ಶೂನ್ಯ trc_infof(ಸಂರಚನಾ Curl_easy *data,
                      ಸಂರಚನಾ curl_trc_feat *feat,
                      ಧ್ರುವ ವರ್ಣ *opt_id, ಪೂರ್ಣಾಙ್ಕ opt_id_idx,
                      ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ fmt, va_list ap) CURL_PRINTF(5, 0);

ಸ್ಥಿರ ಶೂನ್ಯ trc_infof(ಸಂರಚನಾ Curl_easy *data,
                      ಸಂರಚನಾ curl_trc_feat *feat,
                      ಧ್ರುವ ವರ್ಣ *opt_id, ಪೂರ್ಣಾಙ್ಕ opt_id_idx,
                      ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ fmt, va_list ap)
{
  size_t len = 0;
  ವರ್ಣ buf[TRC_ಸಾಲು_MAX];

  ಯದಿ(CURL_TRC_IDS(data))
    len += trc_print_ids(data, buf + len, TRC_ಸಾಲು_MAX - len);
  ಯದಿ(feat)
    len += curl_msnprintf(buf + len, TRC_ಸಾಲು_MAX - len, "[%s] ", feat->name);
  ಯದಿ(opt_id) {
    ಯದಿ(opt_id_idx > 0)
      len += curl_msnprintf(buf + len, TRC_ಸಾಲು_MAX - len, "[%s-%d] ",
                            opt_id, opt_id_idx);
    ಅನ್ಯಥಾ
      len += curl_msnprintf(buf + len, TRC_ಸಾಲು_MAX - len, "[%s] ", opt_id);
  }
  len += curl_mvsnprintf(buf + len, TRC_ಸಾಲು_MAX - len, fmt, ap);
  len = trc_end_buf(buf, len, TRC_ಸಾಲು_MAX, TRUE);
  trc_write(data, CURLINFO_TEXT, buf, len);
}

ಶೂನ್ಯ Curl_infof(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ಯದಿ(Curl_trc_is_verbose(data)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, data->state.feat, ರಿಕ್ತ, 0, fmt, ap);
    va_end(ap);
  }
}

ಶೂನ್ಯ Curl_trc_cf_infof(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ಸಂರಚನಾ Curl_cfilter *cf,
                       ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  DEBUGASSERT(cf);
  ಯದಿ(Curl_trc_cf_is_verbose(cf, data)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, data->state.feat, cf->cft->name, cf->sockindex, fmt, ap);
    va_end(ap);
  }
}

ಶೂನ್ಯ Curl_trc_feat_infof(ಸಂರಚನಾ Curl_easy *data,
                         ಸಂರಚನಾ curl_trc_feat *feat,
                         ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  DEBUGASSERT(feat);
  ಯದಿ(Curl_trc_ft_is_verbose(data, feat)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, feat, ರಿಕ್ತ, 0, fmt, ap);
    va_end(ap);
  }
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ Curl_trc_timer_names[] = {
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

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *trc_timer_name(ಪೂರ್ಣಾಙ್ಕ tid)
{
  ಯದಿ((tid >= 0) && ((size_t)tid < CURL_ARRAYSIZE(Curl_trc_timer_names)))
    ಪ್ರತ್ಯಾಗಮ Curl_trc_timer_names[(size_t)tid];
  ಪ್ರತ್ಯಾಗಮ "TIMER-???";
}

ಶೂನ್ಯ Curl_trc_timer(ಸಂರಚನಾ Curl_easy *data, ಪೂರ್ಣಾಙ್ಕ tid, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ಯದಿ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_timer)) {
    ಧ್ರುವ ವರ್ಣ *tname = trc_timer_name(tid);
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_timer, tname, 0, fmt, ap);
    va_end(ap);
  }
}

ಶೂನ್ಯ Curl_trc_easy_timers(ಸಂರಚನಾ Curl_easy *data)
{
  ಯದಿ(CURL_TRC_TIMER_is_verbose(data) && data->multi) {
    ಯದಿ(data->state.timeouts.first < EXPIRE_LAST) {
      ಸಂರಚನಾ expire_timers *timeouts = &data->state.timeouts;
      timediff_t base_us =
        Curl_timeouts_offset_us(&data->multi->timeouts, Curl_pgrs_now(data));
      uint8_t id = data->state.timeouts.first;
      ಪ್ರತಿ(; id < EXPIRE_LAST; id = timeouts->next[id]) {
        CURL_TRC_TIMER(data, id, "expires in %" FMT_TIMEDIFF_T "us",
                       timeouts->offset_us[id] - base_us);
      }
    }
  }
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ Curl_trc_mstate_names[] = {
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

ಧ್ರುವ ವರ್ಣ *Curl_trc_mstate_name(ಪೂರ್ಣಾಙ್ಕ state)
{
  ಯದಿ((state >= 0) && ((size_t)state < CURL_ARRAYSIZE(Curl_trc_mstate_names)))
    ಪ್ರತ್ಯಾಗಮ Curl_trc_mstate_names[(size_t)state];
  ಪ್ರತ್ಯಾಗಮ "?";
}

ಶೂನ್ಯ Curl_trc_multi(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ಯದಿ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_multi)) {
    ಧ್ರುವ ವರ್ಣ *sname = (data->id >= 0) ?
                        Curl_trc_mstate_name(data->mstate) : ರಿಕ್ತ;
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_multi, sname, 0, fmt, ap);
    va_end(ap);
  }
}

ಶೂನ್ಯ Curl_trc_read(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ಯದಿ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_read)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_read, ರಿಕ್ತ, 0, fmt, ap);
    va_end(ap);
  }
}

ಶೂನ್ಯ Curl_trc_write(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ಯದಿ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_write)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_write, ರಿಕ್ತ, 0, fmt, ap);
    va_end(ap);
  }
}

ಶೂನ್ಯ Curl_trc_dns(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ಯದಿ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_dns)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_dns, ರಿಕ್ತ, 0, fmt, ap);
    va_end(ap);
  }
}

#ifndef CURL_DISABLE_FTP
ಸಂರಚನಾ curl_trc_feat Curl_trc_feat_ftp = {
  "FTP",
  CURL_LOG_LVL_NONE,
};

ಶೂನ್ಯ Curl_trc_ftp(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ಯದಿ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ftp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ftp, ರಿಕ್ತ, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* !CURL_DISABLE_FTP */

#ifndef CURL_DISABLE_SMTP
ಸಂರಚನಾ curl_trc_feat Curl_trc_feat_smtp = {
  "SMTP",
  CURL_LOG_LVL_NONE,
};

ಶೂನ್ಯ Curl_trc_smtp(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ಯದಿ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_smtp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_smtp, ರಿಕ್ತ, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* !CURL_DISABLE_SMTP */

#ifdef USE_SSL
ಸಂರಚನಾ curl_trc_feat Curl_trc_feat_ssls = {
  "SSLS",
  CURL_LOG_LVL_NONE,
};

ಶೂನ್ಯ Curl_trc_ssls(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ಯದಿ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssls)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ssls, ರಿಕ್ತ, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* USE_SSL */

#ifdef USE_SSH
ಸಂರಚನಾ curl_trc_feat Curl_trc_feat_ssh = {
  "SSH",
  CURL_LOG_LVL_NONE,
};

ಶೂನ್ಯ Curl_trc_ssh(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ಯದಿ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssh)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ssh, ರಿಕ್ತ, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* USE_SSH */

#ಯದಿ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
ಸಂರಚನಾ curl_trc_feat Curl_trc_feat_ws = {
  "WS",
  CURL_LOG_LVL_NONE,
};

ಶೂನ್ಯ Curl_trc_ws(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ಯದಿ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ws)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ws, ರಿಕ್ತ, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#define TRC_CT_NONE        0
#define TRC_CT_PROTOCOL    (1 << 0)
#define TRC_CT_NETWORK     (1 << 1)
#define TRC_CT_PROXY       (1 << 2)
#define TRC_CT_INTERNALS   (1 << 3)

ಸಂರಚನಾ trc_feat_def {
  ಸಂರಚನಾ curl_trc_feat *feat;
  unsigned ಪೂರ್ಣಾಙ್ಕ category;
};

ಸ್ಥಿರ ಸಂರಚನಾ trc_feat_def trc_feats[] = {
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
#ಯದಿ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
  { &Curl_trc_feat_ws,        TRC_CT_PROTOCOL },
#endif
};

ಸಂರಚನಾ trc_cft_def {
  ಸಂರಚನಾ Curl_cftype *cft;
  unsigned ಪೂರ್ಣಾಙ್ಕ category;
};

ಸ್ಥಿರ ಸಂರಚನಾ trc_cft_def trc_cfts[] = {
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
#ಯದಿ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
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
#ಯದಿ defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3)
  { &Curl_cft_h3_proxy,       TRC_CT_PROXY },
#endif
  { &Curl_cft_http_proxy,     TRC_CT_PROXY },
#endif /* !CURL_DISABLE_HTTP */
  { &Curl_cft_haproxy,        TRC_CT_PROXY },
  { &Curl_cft_socks_proxy,    TRC_CT_PROXY },
#endif /* !CURL_DISABLE_PROXY */
#ಯದಿ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { &Curl_cft_http3,          TRC_CT_PROTOCOL },
#endif
#ifndef CURL_DISABLE_HTTP
  { &Curl_cft_http_connect,   TRC_CT_PROTOCOL },
#endif
};

ಸ್ಥಿರ ಶೂನ್ಯ trc_apply_level_by_name(ಸಂರಚನಾ Curl_str *token, ಪೂರ್ಣಾಙ್ಕ lvl)
{
  size_t i;

  ಪ್ರತಿ(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    ಯದಿ(curlx_str_casecompare(token, trc_cfts[i].cft->name)) {
      trc_cfts[i].cft->log_level = lvl;
      ಭಙ್ಗ;
    }
  }
  ಪ್ರತಿ(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    ಯದಿ(curlx_str_casecompare(token, trc_feats[i].feat->name)) {
      trc_feats[i].feat->log_level = lvl;
      ಭಙ್ಗ;
    }
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ trc_apply_level_by_category(unsigned ಪೂರ್ಣಾಙ್ಕ category, ಪೂರ್ಣಾಙ್ಕ lvl)
{
  size_t i;

  ಪ್ರತಿ(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    ಯದಿ(!category || (trc_cfts[i].category & category))
      trc_cfts[i].cft->log_level = lvl;
  }
  ಪ್ರತಿ(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    ಯದಿ(!category || (trc_feats[i].category & category))
      trc_feats[i].feat->log_level = lvl;
  }
}

ಸ್ಥಿರ CURLcode trc_opt(ಧ್ರುವ ವರ್ಣ *config)
{
  ಸಂರಚನಾ Curl_str out;
  ಯಾವತ್(!curlx_str_until(&config, &out, 32, ',')) {
    ಪೂರ್ಣಾಙ್ಕ lvl = CURL_LOG_LVL_INFO;
    ಧ್ರುವ ವರ್ಣ *token = curlx_str(&out);

    ಯದಿ(*token == '-') {
      lvl = CURL_LOG_LVL_NONE;
      curlx_str_nudge(&out, 1);
    }
    ಅನ್ಯಥಾ ಯದಿ(*token == '+')
      curlx_str_nudge(&out, 1);

    ಯದಿ(curlx_str_casecompare(&out, "all"))
      trc_apply_level_by_category(TRC_CT_NONE, lvl);
    ಅನ್ಯಥಾ ಯದಿ(curlx_str_casecompare(&out, "protocol"))
      trc_apply_level_by_category(TRC_CT_PROTOCOL, lvl);
    ಅನ್ಯಥಾ ಯದಿ(curlx_str_casecompare(&out, "network"))
      trc_apply_level_by_category(TRC_CT_NETWORK, lvl);
    ಅನ್ಯಥಾ ಯದಿ(curlx_str_casecompare(&out, "proxy"))
      trc_apply_level_by_category(TRC_CT_PROXY, lvl);
    ಅನ್ಯಥಾ
      trc_apply_level_by_name(&out, lvl);

    ಯದಿ(curlx_str_single(&config, ','))
      ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_trc_opt(ಧ್ರುವ ವರ್ಣ *config)
{
  CURLcode result = config ? trc_opt(config) : CURLE_OK;
#ifdef DEBUGBUILD
  /* CURL_DEBUG can override anything */
  ಯದಿ(!result) {
    ಧ್ರುವ ವರ್ಣ *dbg_config = getenv("CURL_DEBUG");
    ಯದಿ(dbg_config)
      result = trc_opt(dbg_config);
  }
#endif /* DEBUGBUILD */
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_trc_init(ಶೂನ್ಯ)
{
#ifdef DEBUGBUILD
  ಪ್ರತ್ಯಾಗಮ Curl_trc_opt(ರಿಕ್ತ);
#ಅನ್ಯಥಾ
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
#endif
}

#ಅನ್ಯಥಾ /* CURL_DISABLE_VERBOSE_STRINGS */

CURLcode Curl_trc_init(ಶೂನ್ಯ)
{
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಶೂನ್ಯ Curl_infof(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)fmt;
}

ಶೂನ್ಯ Curl_trc_cf_infof(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ಸಂರಚನಾ Curl_cfilter *cf,
                       ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)fmt;
}

ಶೂನ್ಯ Curl_trc_multi(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)fmt;
}

ಶೂನ್ಯ Curl_trc_write(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)fmt;
}

ಶೂನ್ಯ Curl_trc_dns(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)fmt;
}

ಶೂನ್ಯ Curl_trc_timer(ಸಂರಚನಾ Curl_easy *data, ಪೂರ್ಣಾಙ್ಕ tid, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)tid;
  (ಶೂನ್ಯ)fmt;
}

ಶೂನ್ಯ Curl_trc_read(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)fmt;
}

#ifndef CURL_DISABLE_FTP
ಶೂನ್ಯ Curl_trc_ftp(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)fmt;
}
#endif
#ifndef CURL_DISABLE_SMTP
ಶೂನ್ಯ Curl_trc_smtp(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)fmt;
}
#endif
#ಯದಿ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
ಶೂನ್ಯ Curl_trc_ws(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)fmt;
}
#endif
#ifdef USE_SSH
ಶೂನ್ಯ Curl_trc_ssh(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)fmt;
}
#endif
#ifdef USE_SSL
ಶೂನ್ಯ Curl_trc_ssls(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)fmt;
}
#endif

#endif /* !CURL_DISABLE_VERBOSE_STRINGS */
