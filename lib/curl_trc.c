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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"

#ଅନ୍ତର୍ଭାଵ "cf-recvbuf.h"
#ଅନ୍ତର୍ଭାଵ "cf-socket.h"
#ଅନ୍ତର୍ଭାଵ "cf-setup.h"
#ଅନ୍ତର୍ଭାଵ "http2.h"
#ଅନ୍ତର୍ଭାଵ "http_proxy.h"
#ଅନ୍ତର୍ଭାଵ "cf-h1-proxy.h"
#ଅନ୍ତର୍ଭାଵ "cf-h2-proxy.h"
#ଅନ୍ତର୍ଭାଵ "cf-haproxy.h"
#ଅନ୍ତର୍ଭାଵ "cf-https-connect.h"
#ଅନ୍ତର୍ଭାଵ "cf-ip-happy.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "socks.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"
#ଅନ୍ତର୍ଭାଵ "vdns/cf-dns.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"
#ଅନ୍ତର୍ଭାଵ "vquic/vquic.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strcopy.h"

ସ୍ଥିର ଶୂନ୍ୟ trc_write(ସଂରଚନା Curl_easy *data, curl_infotype type,
                      ଅଚଳ ଅକ୍ଷର *ptr, size_t size)
{
  ଯଦି(data->set.verbose) {
    ଯଦି(data->set.fdebug) {
      ସଂରଚନା Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_fdebug);
      (ଶୂନ୍ୟ)(*data->set.fdebug)(data, type, CURL_UNCONST(ptr), size,
                                data->set.debugdata);
      CURL_CBAPI_END(&guard);
    }
    ଅନ୍ୟଥା {
      ସ୍ଥିର ଅଚଳ ଅକ୍ଷର s_infotype[CURLINFO_END][3] = {
        "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
      ବାଛ(type) {
      ସ୍ଥିତି CURLINFO_TEXT:
      ସ୍ଥିତି CURLINFO_HEADER_OUT:
      ସ୍ଥିତି CURLINFO_HEADER_IN:
        fwrite(s_infotype[type], 2, 1, data->set.err);
        fwrite(ptr, size, 1, data->set.err);
        ଭାଙ୍ଗ;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ: /* nada */
        ଭାଙ୍ଗ;
      }
    }
  }
}

/* max length we trace before ending in '...' */
#define TRC_ଧାଡ଼ି_MAX 2048

#define CURL_TRC_FMT_IDSC   "[x-%" CURL_FORMAT_CURL_OFF_T "] "
#define CURL_TRC_FMT_IDSD   "[%" CURL_FORMAT_CURL_OFF_T "-x] "
#define CURL_TRC_FMT_IDSDC  "[%" CURL_FORMAT_CURL_OFF_T "-%" \
                            CURL_FORMAT_CURL_OFF_T "] "

ସ୍ଥିର ସଂରଚନା curl_trc_feat Curl_trc_feat_ids = {
  "LIB-IDS",
  CURL_LOG_LVL_NONE,
};
#define CURL_TRC_IDS(data) \
  (Curl_trc_is_verbose(data) && \
  Curl_trc_feat_ids.log_level >= CURL_LOG_LVL_INFO)

ସ୍ଥିର size_t trc_print_ids(ସଂରଚନା Curl_easy *data, ଅକ୍ଷର *buf, size_t maxlen)
{
  curl_off_t cid = data->state.last_conn_id;
  ଯଦି(data->id >= 0) {
    ଯଦି(cid >= 0)
      ଫେରନ୍ତୁ curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSDC, data->id, cid);
    ଅନ୍ୟଥା
      ଫେରନ୍ତୁ curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSD, data->id);
  }
  ଅନ୍ୟଥା ଯଦି(cid >= 0)
    ଫେରନ୍ତୁ curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSC, cid);
  ଅନ୍ୟଥା {
    ଫେରନ୍ତୁ curl_msnprintf(buf, maxlen, "[x-x] ");
  }
}

ସ୍ଥିର size_t trc_end_buf(ଅକ୍ଷର *buf, size_t len, size_t maxlen, bool addnl)
{
  /* make sure we end the trace line in `buf` properly. It needs
   * to end with a null-terminator '\0' or '\n\0' */
  ଯଦି(len >= (maxlen - (addnl ? 2 : 1))) {
    len = maxlen - 5;
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '\n';
  }
  ଅନ୍ୟଥା ଯଦି(addnl)
    buf[len++] = '\n';
  buf[len] = '\0';
  ଫେରନ୍ତୁ len;
}

ଶୂନ୍ୟ Curl_debug(ସଂରଚନା Curl_easy *data, curl_infotype type,
                ଅଚଳ ଅକ୍ଷର *ptr, size_t size)
{
  ଯଦି(data->set.verbose) {
    ସ୍ଥିର ଅଚଳ ଅକ୍ଷର s_infotype[CURLINFO_END][3] = {
      "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
    ଅକ୍ଷର buf[TRC_ଧାଡ଼ି_MAX];
    size_t len;
    ଯଦି(data->set.fdebug) {
      ସଂରଚନା Curl_mapi_guard guard;
      ଯଦି(CURL_TRC_IDS(data) && (size < TRC_ଧାଡ଼ି_MAX)) {
        len = trc_print_ids(data, buf, TRC_ଧାଡ଼ି_MAX);
        len += curl_msnprintf(buf + len, TRC_ଧାଡ଼ି_MAX - len, "%.*s",
                              (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)size, ptr);
        len = trc_end_buf(buf, len, TRC_ଧାଡ଼ି_MAX, FALSE);
        CURL_CBAPI_START(&guard, data, easy_fdebug);
        (ଶୂନ୍ୟ)(*data->set.fdebug)(data, type, buf, len, data->set.debugdata);
        CURL_CBAPI_END(&guard);
      }
      ଅନ୍ୟଥା {
        CURL_CBAPI_START(&guard, data, easy_fdebug);
        (ଶୂନ୍ୟ)(*data->set.fdebug)(data, type, CURL_UNCONST(ptr),
                                  size, data->set.debugdata);
        CURL_CBAPI_END(&guard);
      }
    }
    ଅନ୍ୟଥା {
      ବାଛ(type) {
      ସ୍ଥିତି CURLINFO_TEXT:
      ସ୍ଥିତି CURLINFO_HEADER_OUT:
      ସ୍ଥିତି CURLINFO_HEADER_IN:
#ifndef CURL_DISABLE_VERBOSE_STRINGS
        ଯଦି(CURL_TRC_IDS(data)) {
          len = trc_print_ids(data, buf, TRC_ଧାଡ଼ି_MAX);
          fwrite(buf, len, 1, data->set.err);
        }
#endif
        fwrite(s_infotype[type], 2, 1, data->set.err);
        fwrite(ptr, size, 1, data->set.err);
        ଭାଙ୍ଗ;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ: /* nada */
        ଭାଙ୍ଗ;
      }
    }
  }
}

/* Curl_failf() is for messages stating why we failed.
 * The message SHALL NOT include any LF or CR.
 */
ଶୂନ୍ୟ Curl_failf(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ଯଦି(data->set.verbose || data->set.errorbuffer) {
    va_list ap;
    size_t len;
    ଅକ୍ଷର error[CURL_ERROR_SIZE + 2];
    va_start(ap, fmt);
    len = curl_mvsnprintf(error, CURL_ERROR_SIZE, fmt, ap);

    ଯଦି(data->set.errorbuffer && !data->state.errorbuf) {
      curlx_strcopy(data->set.errorbuffer, CURL_ERROR_SIZE, error, len);
      data->state.errorbuf = TRUE; /* wrote error string */
    }
    error[len++] = '\n';
    error[len] = '\0';
    trc_write(data, CURLINFO_TEXT, error, len);
    va_end(ap);
  }
}

ଶୂନ୍ୟ Curl_reset_fail(ସଂରଚନା Curl_easy *data)
{
  ଯଦି(data->set.errorbuffer)
    data->set.errorbuffer[0] = 0;
  data->state.errorbuf = FALSE;
}

#ifdef CURLVERBOSE
ସଂରଚନା curl_trc_feat Curl_trc_feat_multi = {
  "MULTI",
  CURL_LOG_LVL_NONE,
};
ସଂରଚନା curl_trc_feat Curl_trc_feat_read = {
  "READ",
  CURL_LOG_LVL_NONE,
};
ସଂରଚନା curl_trc_feat Curl_trc_feat_write = {
  "WRITE",
  CURL_LOG_LVL_NONE,
};
ସଂରଚନା curl_trc_feat Curl_trc_feat_dns = {
  "DNS",
  CURL_LOG_LVL_NONE,
};
#ifndef CURL_DISABLE_DOH
ସଂରଚନା curl_trc_feat Curl_trc_feat_doh = {
  "DoH",
  CURL_LOG_LVL_NONE,
};
#endif
ସଂରଚନା curl_trc_feat Curl_trc_feat_timer = {
  "TIMER",
  CURL_LOG_LVL_NONE,
};
#ifdef USE_THREADS
ସଂରଚନା curl_trc_feat Curl_trc_feat_threads = {
  "THREADS",
  CURL_LOG_LVL_NONE,
};
#endif
#endif

#ifndef CURL_DISABLE_VERBOSE_STRINGS

ସ୍ଥିର ଶୂନ୍ୟ trc_infof(ସଂରଚନା Curl_easy *data,
                      ସଂରଚନା curl_trc_feat *feat,
                      ଅଚଳ ଅକ୍ଷର *opt_id, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_id_idx,
                      ଅଚଳ ଅକ୍ଷର * ଅଚଳ fmt, va_list ap) CURL_PRINTF(5, 0);

ସ୍ଥିର ଶୂନ୍ୟ trc_infof(ସଂରଚନା Curl_easy *data,
                      ସଂରଚନା curl_trc_feat *feat,
                      ଅଚଳ ଅକ୍ଷର *opt_id, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_id_idx,
                      ଅଚଳ ଅକ୍ଷର * ଅଚଳ fmt, va_list ap)
{
  size_t len = 0;
  ଅକ୍ଷର buf[TRC_ଧାଡ଼ି_MAX];

  ଯଦି(CURL_TRC_IDS(data))
    len += trc_print_ids(data, buf + len, TRC_ଧାଡ଼ି_MAX - len);
  ଯଦି(feat)
    len += curl_msnprintf(buf + len, TRC_ଧାଡ଼ି_MAX - len, "[%s] ", feat->name);
  ଯଦି(opt_id) {
    ଯଦି(opt_id_idx > 0)
      len += curl_msnprintf(buf + len, TRC_ଧାଡ଼ି_MAX - len, "[%s-%d] ",
                            opt_id, opt_id_idx);
    ଅନ୍ୟଥା
      len += curl_msnprintf(buf + len, TRC_ଧାଡ଼ି_MAX - len, "[%s] ", opt_id);
  }
  len += curl_mvsnprintf(buf + len, TRC_ଧାଡ଼ି_MAX - len, fmt, ap);
  len = trc_end_buf(buf, len, TRC_ଧାଡ଼ି_MAX, TRUE);
  trc_write(data, CURLINFO_TEXT, buf, len);
}

ଶୂନ୍ୟ Curl_infof(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ଯଦି(Curl_trc_is_verbose(data)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, data->state.feat, ଖାଲି, 0, fmt, ap);
    va_end(ap);
  }
}

ଶୂନ୍ୟ Curl_trc_cf_infof(ସଂରଚନା Curl_easy *data, ଅଚଳ ସଂରଚନା Curl_cfilter *cf,
                       ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  DEBUGASSERT(cf);
  ଯଦି(Curl_trc_cf_is_verbose(cf, data)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, data->state.feat, cf->cft->name, cf->sockindex, fmt, ap);
    va_end(ap);
  }
}

ଶୂନ୍ୟ Curl_trc_feat_infof(ସଂରଚନା Curl_easy *data,
                         ସଂରଚନା curl_trc_feat *feat,
                         ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  DEBUGASSERT(feat);
  ଯଦି(Curl_trc_ft_is_verbose(data, feat)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, feat, ଖାଲି, 0, fmt, ap);
    va_end(ap);
  }
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ Curl_trc_timer_names[] = {
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

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *trc_timer_name(ପୂର୍ଣ୍ଣସଂଖ୍ୟା tid)
{
  ଯଦି((tid >= 0) && ((size_t)tid < CURL_ARRAYSIZE(Curl_trc_timer_names)))
    ଫେରନ୍ତୁ Curl_trc_timer_names[(size_t)tid];
  ଫେରନ୍ତୁ "TIMER-???";
}

ଶୂନ୍ୟ Curl_trc_timer(ସଂରଚନା Curl_easy *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା tid, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ଯଦି(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_timer)) {
    ଅଚଳ ଅକ୍ଷର *tname = trc_timer_name(tid);
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_timer, tname, 0, fmt, ap);
    va_end(ap);
  }
}

ଶୂନ୍ୟ Curl_trc_easy_timers(ସଂରଚନା Curl_easy *data)
{
  ଯଦି(CURL_TRC_TIMER_is_verbose(data) && data->multi) {
    ଯଦି(data->state.timeouts.first < EXPIRE_LAST) {
      ସଂରଚନା expire_timers *timeouts = &data->state.timeouts;
      timediff_t base_us =
        Curl_timeouts_offset_us(&data->multi->timeouts, Curl_pgrs_now(data));
      uint8_t id = data->state.timeouts.first;
      ପାଇଁ(; id < EXPIRE_LAST; id = timeouts->next[id]) {
        CURL_TRC_TIMER(data, id, "expires in %" FMT_TIMEDIFF_T "us",
                       timeouts->offset_us[id] - base_us);
      }
    }
  }
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ Curl_trc_mstate_names[] = {
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

ଅଚଳ ଅକ୍ଷର *Curl_trc_mstate_name(ପୂର୍ଣ୍ଣସଂଖ୍ୟା state)
{
  ଯଦି((state >= 0) && ((size_t)state < CURL_ARRAYSIZE(Curl_trc_mstate_names)))
    ଫେରନ୍ତୁ Curl_trc_mstate_names[(size_t)state];
  ଫେରନ୍ତୁ "?";
}

ଶୂନ୍ୟ Curl_trc_multi(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ଯଦି(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_multi)) {
    ଅଚଳ ଅକ୍ଷର *sname = (data->id >= 0) ?
                        Curl_trc_mstate_name(data->mstate) : ଖାଲି;
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_multi, sname, 0, fmt, ap);
    va_end(ap);
  }
}

ଶୂନ୍ୟ Curl_trc_read(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ଯଦି(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_read)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_read, ଖାଲି, 0, fmt, ap);
    va_end(ap);
  }
}

ଶୂନ୍ୟ Curl_trc_write(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ଯଦି(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_write)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_write, ଖାଲି, 0, fmt, ap);
    va_end(ap);
  }
}

ଶୂନ୍ୟ Curl_trc_dns(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ଯଦି(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_dns)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_dns, ଖାଲି, 0, fmt, ap);
    va_end(ap);
  }
}

#ifndef CURL_DISABLE_FTP
ସଂରଚନା curl_trc_feat Curl_trc_feat_ftp = {
  "FTP",
  CURL_LOG_LVL_NONE,
};

ଶୂନ୍ୟ Curl_trc_ftp(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ଯଦି(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ftp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ftp, ଖାଲି, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* !CURL_DISABLE_FTP */

#ifndef CURL_DISABLE_SMTP
ସଂରଚନା curl_trc_feat Curl_trc_feat_smtp = {
  "SMTP",
  CURL_LOG_LVL_NONE,
};

ଶୂନ୍ୟ Curl_trc_smtp(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ଯଦି(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_smtp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_smtp, ଖାଲି, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* !CURL_DISABLE_SMTP */

#ifdef USE_SSL
ସଂରଚନା curl_trc_feat Curl_trc_feat_ssls = {
  "SSLS",
  CURL_LOG_LVL_NONE,
};

ଶୂନ୍ୟ Curl_trc_ssls(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ଯଦି(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssls)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ssls, ଖାଲି, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* USE_SSL */

#ifdef USE_SSH
ସଂରଚନା curl_trc_feat Curl_trc_feat_ssh = {
  "SSH",
  CURL_LOG_LVL_NONE,
};

ଶୂନ୍ୟ Curl_trc_ssh(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ଯଦି(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssh)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ssh, ଖାଲି, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* USE_SSH */

#ଯଦି !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
ସଂରଚନା curl_trc_feat Curl_trc_feat_ws = {
  "WS",
  CURL_LOG_LVL_NONE,
};

ଶୂନ୍ୟ Curl_trc_ws(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ଯଦି(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ws)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ws, ଖାଲି, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#define TRC_CT_NONE        0
#define TRC_CT_PROTOCOL    (1 << 0)
#define TRC_CT_NETWORK     (1 << 1)
#define TRC_CT_PROXY       (1 << 2)
#define TRC_CT_INTERNALS   (1 << 3)

ସଂରଚନା trc_feat_def {
  ସଂରଚନା curl_trc_feat *feat;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା category;
};

ସ୍ଥିର ସଂରଚନା trc_feat_def trc_feats[] = {
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
#ଯଦି !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
  { &Curl_trc_feat_ws,        TRC_CT_PROTOCOL },
#endif
};

ସଂରଚନା trc_cft_def {
  ସଂରଚନା Curl_cftype *cft;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା category;
};

ସ୍ଥିର ସଂରଚନା trc_cft_def trc_cfts[] = {
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
#ଯଦି !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
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
#ଯଦି defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3)
  { &Curl_cft_h3_proxy,       TRC_CT_PROXY },
#endif
  { &Curl_cft_http_proxy,     TRC_CT_PROXY },
#endif /* !CURL_DISABLE_HTTP */
  { &Curl_cft_haproxy,        TRC_CT_PROXY },
  { &Curl_cft_socks_proxy,    TRC_CT_PROXY },
#endif /* !CURL_DISABLE_PROXY */
#ଯଦି !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { &Curl_cft_http3,          TRC_CT_PROTOCOL },
#endif
#ifndef CURL_DISABLE_HTTP
  { &Curl_cft_http_connect,   TRC_CT_PROTOCOL },
#endif
};

ସ୍ଥିର ଶୂନ୍ୟ trc_apply_level_by_name(ସଂରଚନା Curl_str *token, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lvl)
{
  size_t i;

  ପାଇଁ(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    ଯଦି(curlx_str_casecompare(token, trc_cfts[i].cft->name)) {
      trc_cfts[i].cft->log_level = lvl;
      ଭାଙ୍ଗ;
    }
  }
  ପାଇଁ(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    ଯଦି(curlx_str_casecompare(token, trc_feats[i].feat->name)) {
      trc_feats[i].feat->log_level = lvl;
      ଭାଙ୍ଗ;
    }
  }
}

ସ୍ଥିର ଶୂନ୍ୟ trc_apply_level_by_category(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା category, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lvl)
{
  size_t i;

  ପାଇଁ(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    ଯଦି(!category || (trc_cfts[i].category & category))
      trc_cfts[i].cft->log_level = lvl;
  }
  ପାଇଁ(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    ଯଦି(!category || (trc_feats[i].category & category))
      trc_feats[i].feat->log_level = lvl;
  }
}

ସ୍ଥିର CURLcode trc_opt(ଅଚଳ ଅକ୍ଷର *config)
{
  ସଂରଚନା Curl_str out;
  ଯେତେବେଳେ(!curlx_str_until(&config, &out, 32, ',')) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lvl = CURL_LOG_LVL_INFO;
    ଅଚଳ ଅକ୍ଷର *token = curlx_str(&out);

    ଯଦି(*token == '-') {
      lvl = CURL_LOG_LVL_NONE;
      curlx_str_nudge(&out, 1);
    }
    ଅନ୍ୟଥା ଯଦି(*token == '+')
      curlx_str_nudge(&out, 1);

    ଯଦି(curlx_str_casecompare(&out, "all"))
      trc_apply_level_by_category(TRC_CT_NONE, lvl);
    ଅନ୍ୟଥା ଯଦି(curlx_str_casecompare(&out, "protocol"))
      trc_apply_level_by_category(TRC_CT_PROTOCOL, lvl);
    ଅନ୍ୟଥା ଯଦି(curlx_str_casecompare(&out, "network"))
      trc_apply_level_by_category(TRC_CT_NETWORK, lvl);
    ଅନ୍ୟଥା ଯଦି(curlx_str_casecompare(&out, "proxy"))
      trc_apply_level_by_category(TRC_CT_PROXY, lvl);
    ଅନ୍ୟଥା
      trc_apply_level_by_name(&out, lvl);

    ଯଦି(curlx_str_single(&config, ','))
      ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_trc_opt(ଅଚଳ ଅକ୍ଷର *config)
{
  CURLcode result = config ? trc_opt(config) : CURLE_OK;
#ifdef DEBUGBUILD
  /* CURL_DEBUG can override anything */
  ଯଦି(!result) {
    ଅଚଳ ଅକ୍ଷର *dbg_config = getenv("CURL_DEBUG");
    ଯଦି(dbg_config)
      result = trc_opt(dbg_config);
  }
#endif /* DEBUGBUILD */
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_trc_init(ଶୂନ୍ୟ)
{
#ifdef DEBUGBUILD
  ଫେରନ୍ତୁ Curl_trc_opt(ଖାଲି);
#ଅନ୍ୟଥା
  ଫେରନ୍ତୁ CURLE_OK;
#endif
}

#ଅନ୍ୟଥା /* CURL_DISABLE_VERBOSE_STRINGS */

CURLcode Curl_trc_init(ଶୂନ୍ୟ)
{
  ଫେରନ୍ତୁ CURLE_OK;
}

ଶୂନ୍ୟ Curl_infof(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)fmt;
}

ଶୂନ୍ୟ Curl_trc_cf_infof(ସଂରଚନା Curl_easy *data, ଅଚଳ ସଂରଚନା Curl_cfilter *cf,
                       ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)cf;
  (ଶୂନ୍ୟ)fmt;
}

ଶୂନ୍ୟ Curl_trc_multi(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)fmt;
}

ଶୂନ୍ୟ Curl_trc_write(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)fmt;
}

ଶୂନ୍ୟ Curl_trc_dns(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)fmt;
}

ଶୂନ୍ୟ Curl_trc_timer(ସଂରଚନା Curl_easy *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା tid, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)tid;
  (ଶୂନ୍ୟ)fmt;
}

ଶୂନ୍ୟ Curl_trc_read(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)fmt;
}

#ifndef CURL_DISABLE_FTP
ଶୂନ୍ୟ Curl_trc_ftp(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)fmt;
}
#endif
#ifndef CURL_DISABLE_SMTP
ଶୂନ୍ୟ Curl_trc_smtp(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)fmt;
}
#endif
#ଯଦି !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
ଶୂନ୍ୟ Curl_trc_ws(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)fmt;
}
#endif
#ifdef USE_SSH
ଶୂନ୍ୟ Curl_trc_ssh(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)fmt;
}
#endif
#ifdef USE_SSL
ଶୂନ୍ୟ Curl_trc_ssls(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)fmt;
}
#endif

#endif /* !CURL_DISABLE_VERBOSE_STRINGS */
