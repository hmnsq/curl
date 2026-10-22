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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "multiif.h"

#अन्तर्भाव "cf-recvbuf.h"
#अन्तर्भाव "cf-socket.h"
#अन्तर्भाव "cf-setup.h"
#अन्तर्भाव "http2.h"
#अन्तर्भाव "http_proxy.h"
#अन्तर्भाव "cf-h1-proxy.h"
#अन्तर्भाव "cf-h2-proxy.h"
#अन्तर्भाव "cf-haproxy.h"
#अन्तर्भाव "cf-https-connect.h"
#अन्तर्भाव "cf-ip-happy.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "socks.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "vdns/cf-dns.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "vquic/vquic.h"
#अन्तर्भाव "curlx/strcopy.h"

स्थिर शून्य trc_write(संरचना Curl_easy *data, curl_infotype type,
                      स्थिरांक अक्षर *ptr, माप_प्रकार size)
{
  जर(data->set.verbose) {
    जर(data->set.fdebug) {
      संरचना Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_fdebug);
      (शून्य)(*data->set.fdebug)(data, type, CURL_UNCONST(ptr), size,
                                data->set.debugdata);
      CURL_CBAPI_END(&guard);
    }
    अन्यथा {
      स्थिर स्थिरांक अक्षर s_infotype[CURLINFO_END][3] = {
        "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
      निवड(type) {
      स्थिती CURLINFO_TEXT:
      स्थिती CURLINFO_HEADER_OUT:
      स्थिती CURLINFO_HEADER_IN:
        ख_डालो(s_infotype[type], 2, 1, data->set.err);
        ख_डालो(ptr, size, 1, data->set.err);
        थांबा;
      पूर्वनिर्धारित: /* nada */
        थांबा;
      }
    }
  }
}

/* max length we trace before ending in '...' */
#define TRC_ओळ_MAX 2048

#define CURL_TRC_FMT_IDSC   "[x-%" CURL_FORMAT_CURL_OFF_T "] "
#define CURL_TRC_FMT_IDSD   "[%" CURL_FORMAT_CURL_OFF_T "-x] "
#define CURL_TRC_FMT_IDSDC  "[%" CURL_FORMAT_CURL_OFF_T "-%" \
                            CURL_FORMAT_CURL_OFF_T "] "

स्थिर संरचना curl_trc_feat Curl_trc_feat_ids = {
  "LIB-IDS",
  CURL_LOG_LVL_NONE,
};
#define CURL_TRC_IDS(data) \
  (Curl_trc_is_verbose(data) && \
  Curl_trc_feat_ids.log_level >= CURL_LOG_LVL_INFO)

स्थिर माप_प्रकार trc_print_ids(संरचना Curl_easy *data, अक्षर *buf, माप_प्रकार maxlen)
{
  curl_off_t cid = data->state.last_conn_id;
  जर(data->id >= 0) {
    जर(cid >= 0)
      परत curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSDC, data->id, cid);
    अन्यथा
      परत curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSD, data->id);
  }
  अन्यथा जर(cid >= 0)
    परत curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSC, cid);
  अन्यथा {
    परत curl_msnprintf(buf, maxlen, "[x-x] ");
  }
}

स्थिर माप_प्रकार trc_end_buf(अक्षर *buf, माप_प्रकार len, माप_प्रकार maxlen, bool addnl)
{
  /* make sure we end the trace line in `buf` properly. It needs
   * to end with a null-terminator '\0' or '\n\0' */
  जर(len >= (maxlen - (addnl ? 2 : 1))) {
    len = maxlen - 5;
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '\n';
  }
  अन्यथा जर(addnl)
    buf[len++] = '\n';
  buf[len] = '\0';
  परत len;
}

शून्य Curl_debug(संरचना Curl_easy *data, curl_infotype type,
                स्थिरांक अक्षर *ptr, माप_प्रकार size)
{
  जर(data->set.verbose) {
    स्थिर स्थिरांक अक्षर s_infotype[CURLINFO_END][3] = {
      "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
    अक्षर buf[TRC_ओळ_MAX];
    माप_प्रकार len;
    जर(data->set.fdebug) {
      संरचना Curl_mapi_guard guard;
      जर(CURL_TRC_IDS(data) && (size < TRC_ओळ_MAX)) {
        len = trc_print_ids(data, buf, TRC_ओळ_MAX);
        len += curl_msnprintf(buf + len, TRC_ओळ_MAX - len, "%.*s",
                              (पूर्णांक)size, ptr);
        len = trc_end_buf(buf, len, TRC_ओळ_MAX, FALSE);
        CURL_CBAPI_START(&guard, data, easy_fdebug);
        (शून्य)(*data->set.fdebug)(data, type, buf, len, data->set.debugdata);
        CURL_CBAPI_END(&guard);
      }
      अन्यथा {
        CURL_CBAPI_START(&guard, data, easy_fdebug);
        (शून्य)(*data->set.fdebug)(data, type, CURL_UNCONST(ptr),
                                  size, data->set.debugdata);
        CURL_CBAPI_END(&guard);
      }
    }
    अन्यथा {
      निवड(type) {
      स्थिती CURLINFO_TEXT:
      स्थिती CURLINFO_HEADER_OUT:
      स्थिती CURLINFO_HEADER_IN:
#ifndef CURL_DISABLE_VERBOSE_STRINGS
        जर(CURL_TRC_IDS(data)) {
          len = trc_print_ids(data, buf, TRC_ओळ_MAX);
          ख_डालो(buf, len, 1, data->set.err);
        }
#endif
        ख_डालो(s_infotype[type], 2, 1, data->set.err);
        ख_डालो(ptr, size, 1, data->set.err);
        थांबा;
      पूर्वनिर्धारित: /* nada */
        थांबा;
      }
    }
  }
}

/* Curl_failf() is for messages stating why we failed.
 * The message SHALL NOT include any LF or CR.
 */
शून्य Curl_failf(संरचना Curl_easy *data, स्थिरांक अक्षर *fmt, ...)
{
  DEBUGASSERT(!म_अक्षर(fmt, '\n'));
  जर(data->set.verbose || data->set.errorbuffer) {
    बहु_सूची ap;
    माप_प्रकार len;
    अक्षर error[CURL_ERROR_SIZE + 2];
    बहु_शुरू(ap, fmt);
    len = curl_mvsnprintf(error, CURL_ERROR_SIZE, fmt, ap);

    जर(data->set.errorbuffer && !data->state.errorbuf) {
      curlx_strcopy(data->set.errorbuffer, CURL_ERROR_SIZE, error, len);
      data->state.errorbuf = TRUE; /* wrote error string */
    }
    error[len++] = '\n';
    error[len] = '\0';
    trc_write(data, CURLINFO_TEXT, error, len);
    बहु_पूर्ण(ap);
  }
}

शून्य Curl_reset_fail(संरचना Curl_easy *data)
{
  जर(data->set.errorbuffer)
    data->set.errorbuffer[0] = 0;
  data->state.errorbuf = FALSE;
}

#ifdef CURLVERBOSE
संरचना curl_trc_feat Curl_trc_feat_multi = {
  "MULTI",
  CURL_LOG_LVL_NONE,
};
संरचना curl_trc_feat Curl_trc_feat_read = {
  "READ",
  CURL_LOG_LVL_NONE,
};
संरचना curl_trc_feat Curl_trc_feat_write = {
  "WRITE",
  CURL_LOG_LVL_NONE,
};
संरचना curl_trc_feat Curl_trc_feat_dns = {
  "DNS",
  CURL_LOG_LVL_NONE,
};
#ifndef CURL_DISABLE_DOH
संरचना curl_trc_feat Curl_trc_feat_doh = {
  "DoH",
  CURL_LOG_LVL_NONE,
};
#endif
संरचना curl_trc_feat Curl_trc_feat_timer = {
  "TIMER",
  CURL_LOG_LVL_NONE,
};
#ifdef USE_THREADS
संरचना curl_trc_feat Curl_trc_feat_threads = {
  "THREADS",
  CURL_LOG_LVL_NONE,
};
#endif
#endif

#ifndef CURL_DISABLE_VERBOSE_STRINGS

स्थिर शून्य trc_infof(संरचना Curl_easy *data,
                      संरचना curl_trc_feat *feat,
                      स्थिरांक अक्षर *opt_id, पूर्णांक opt_id_idx,
                      स्थिरांक अक्षर * स्थिरांक fmt, बहु_सूची ap) CURL_PRINTF(5, 0);

स्थिर शून्य trc_infof(संरचना Curl_easy *data,
                      संरचना curl_trc_feat *feat,
                      स्थिरांक अक्षर *opt_id, पूर्णांक opt_id_idx,
                      स्थिरांक अक्षर * स्थिरांक fmt, बहु_सूची ap)
{
  माप_प्रकार len = 0;
  अक्षर buf[TRC_ओळ_MAX];

  जर(CURL_TRC_IDS(data))
    len += trc_print_ids(data, buf + len, TRC_ओळ_MAX - len);
  जर(feat)
    len += curl_msnprintf(buf + len, TRC_ओळ_MAX - len, "[%s] ", feat->name);
  जर(opt_id) {
    जर(opt_id_idx > 0)
      len += curl_msnprintf(buf + len, TRC_ओळ_MAX - len, "[%s-%d] ",
                            opt_id, opt_id_idx);
    अन्यथा
      len += curl_msnprintf(buf + len, TRC_ओळ_MAX - len, "[%s] ", opt_id);
  }
  len += curl_mvsnprintf(buf + len, TRC_ओळ_MAX - len, fmt, ap);
  len = trc_end_buf(buf, len, TRC_ओळ_MAX, TRUE);
  trc_write(data, CURLINFO_TEXT, buf, len);
}

शून्य Curl_infof(संरचना Curl_easy *data, स्थिरांक अक्षर *fmt, ...)
{
  DEBUGASSERT(!म_अक्षर(fmt, '\n'));
  जर(Curl_trc_is_verbose(data)) {
    बहु_सूची ap;
    बहु_शुरू(ap, fmt);
    trc_infof(data, data->state.feat, रिक्त, 0, fmt, ap);
    बहु_पूर्ण(ap);
  }
}

शून्य Curl_trc_cf_infof(संरचना Curl_easy *data, स्थिरांक संरचना Curl_cfilter *cf,
                       स्थिरांक अक्षर *fmt, ...)
{
  DEBUGASSERT(cf);
  जर(Curl_trc_cf_is_verbose(cf, data)) {
    बहु_सूची ap;
    बहु_शुरू(ap, fmt);
    trc_infof(data, data->state.feat, cf->cft->name, cf->sockindex, fmt, ap);
    बहु_पूर्ण(ap);
  }
}

शून्य Curl_trc_feat_infof(संरचना Curl_easy *data,
                         संरचना curl_trc_feat *feat,
                         स्थिरांक अक्षर *fmt, ...)
{
  DEBUGASSERT(feat);
  जर(Curl_trc_ft_is_verbose(data, feat)) {
    बहु_सूची ap;
    बहु_शुरू(ap, fmt);
    trc_infof(data, feat, रिक्त, 0, fmt, ap);
    बहु_पूर्ण(ap);
  }
}

स्थिर स्थिरांक अक्षर * स्थिरांक Curl_trc_timer_names[] = {
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

स्थिर स्थिरांक अक्षर *trc_timer_name(पूर्णांक tid)
{
  जर((tid >= 0) && ((माप_प्रकार)tid < CURL_ARRAYSIZE(Curl_trc_timer_names)))
    परत Curl_trc_timer_names[(माप_प्रकार)tid];
  परत "TIMER-???";
}

शून्य Curl_trc_timer(संरचना Curl_easy *data, पूर्णांक tid, स्थिरांक अक्षर *fmt, ...)
{
  DEBUGASSERT(!म_अक्षर(fmt, '\n'));
  जर(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_timer)) {
    स्थिरांक अक्षर *tname = trc_timer_name(tid);
    बहु_सूची ap;
    बहु_शुरू(ap, fmt);
    trc_infof(data, &Curl_trc_feat_timer, tname, 0, fmt, ap);
    बहु_पूर्ण(ap);
  }
}

शून्य Curl_trc_easy_timers(संरचना Curl_easy *data)
{
  जर(CURL_TRC_TIMER_is_verbose(data) && data->multi) {
    जर(data->state.timeouts.first < EXPIRE_LAST) {
      संरचना expire_timers *timeouts = &data->state.timeouts;
      timediff_t base_us =
        Curl_timeouts_offset_us(&data->multi->timeouts, Curl_pgrs_now(data));
      uint8_t id = data->state.timeouts.first;
      साठी(; id < EXPIRE_LAST; id = timeouts->next[id]) {
        CURL_TRC_TIMER(data, id, "expires in %" FMT_TIMEDIFF_T "us",
                       timeouts->offset_us[id] - base_us);
      }
    }
  }
}

स्थिर स्थिरांक अक्षर * स्थिरांक Curl_trc_mstate_names[] = {
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

स्थिरांक अक्षर *Curl_trc_mstate_name(पूर्णांक state)
{
  जर((state >= 0) && ((माप_प्रकार)state < CURL_ARRAYSIZE(Curl_trc_mstate_names)))
    परत Curl_trc_mstate_names[(माप_प्रकार)state];
  परत "?";
}

शून्य Curl_trc_multi(संरचना Curl_easy *data, स्थिरांक अक्षर *fmt, ...)
{
  DEBUGASSERT(!म_अक्षर(fmt, '\n'));
  जर(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_multi)) {
    स्थिरांक अक्षर *sname = (data->id >= 0) ?
                        Curl_trc_mstate_name(data->mstate) : रिक्त;
    बहु_सूची ap;
    बहु_शुरू(ap, fmt);
    trc_infof(data, &Curl_trc_feat_multi, sname, 0, fmt, ap);
    बहु_पूर्ण(ap);
  }
}

शून्य Curl_trc_read(संरचना Curl_easy *data, स्थिरांक अक्षर *fmt, ...)
{
  DEBUGASSERT(!म_अक्षर(fmt, '\n'));
  जर(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_read)) {
    बहु_सूची ap;
    बहु_शुरू(ap, fmt);
    trc_infof(data, &Curl_trc_feat_read, रिक्त, 0, fmt, ap);
    बहु_पूर्ण(ap);
  }
}

शून्य Curl_trc_write(संरचना Curl_easy *data, स्थिरांक अक्षर *fmt, ...)
{
  DEBUGASSERT(!म_अक्षर(fmt, '\n'));
  जर(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_write)) {
    बहु_सूची ap;
    बहु_शुरू(ap, fmt);
    trc_infof(data, &Curl_trc_feat_write, रिक्त, 0, fmt, ap);
    बहु_पूर्ण(ap);
  }
}

शून्य Curl_trc_dns(संरचना Curl_easy *data, स्थिरांक अक्षर *fmt, ...)
{
  DEBUGASSERT(!म_अक्षर(fmt, '\n'));
  जर(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_dns)) {
    बहु_सूची ap;
    बहु_शुरू(ap, fmt);
    trc_infof(data, &Curl_trc_feat_dns, रिक्त, 0, fmt, ap);
    बहु_पूर्ण(ap);
  }
}

#ifndef CURL_DISABLE_FTP
संरचना curl_trc_feat Curl_trc_feat_ftp = {
  "FTP",
  CURL_LOG_LVL_NONE,
};

शून्य Curl_trc_ftp(संरचना Curl_easy *data, स्थिरांक अक्षर *fmt, ...)
{
  DEBUGASSERT(!म_अक्षर(fmt, '\n'));
  जर(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ftp)) {
    बहु_सूची ap;
    बहु_शुरू(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ftp, रिक्त, 0, fmt, ap);
    बहु_पूर्ण(ap);
  }
}
#endif /* !CURL_DISABLE_FTP */

#ifndef CURL_DISABLE_SMTP
संरचना curl_trc_feat Curl_trc_feat_smtp = {
  "SMTP",
  CURL_LOG_LVL_NONE,
};

शून्य Curl_trc_smtp(संरचना Curl_easy *data, स्थिरांक अक्षर *fmt, ...)
{
  DEBUGASSERT(!म_अक्षर(fmt, '\n'));
  जर(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_smtp)) {
    बहु_सूची ap;
    बहु_शुरू(ap, fmt);
    trc_infof(data, &Curl_trc_feat_smtp, रिक्त, 0, fmt, ap);
    बहु_पूर्ण(ap);
  }
}
#endif /* !CURL_DISABLE_SMTP */

#ifdef USE_SSL
संरचना curl_trc_feat Curl_trc_feat_ssls = {
  "SSLS",
  CURL_LOG_LVL_NONE,
};

शून्य Curl_trc_ssls(संरचना Curl_easy *data, स्थिरांक अक्षर *fmt, ...)
{
  DEBUGASSERT(!म_अक्षर(fmt, '\n'));
  जर(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssls)) {
    बहु_सूची ap;
    बहु_शुरू(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ssls, रिक्त, 0, fmt, ap);
    बहु_पूर्ण(ap);
  }
}
#endif /* USE_SSL */

#ifdef USE_SSH
संरचना curl_trc_feat Curl_trc_feat_ssh = {
  "SSH",
  CURL_LOG_LVL_NONE,
};

शून्य Curl_trc_ssh(संरचना Curl_easy *data, स्थिरांक अक्षर *fmt, ...)
{
  DEBUGASSERT(!म_अक्षर(fmt, '\n'));
  जर(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssh)) {
    बहु_सूची ap;
    बहु_शुरू(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ssh, रिक्त, 0, fmt, ap);
    बहु_पूर्ण(ap);
  }
}
#endif /* USE_SSH */

#जर !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
संरचना curl_trc_feat Curl_trc_feat_ws = {
  "WS",
  CURL_LOG_LVL_NONE,
};

शून्य Curl_trc_ws(संरचना Curl_easy *data, स्थिरांक अक्षर *fmt, ...)
{
  DEBUGASSERT(!म_अक्षर(fmt, '\n'));
  जर(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ws)) {
    बहु_सूची ap;
    बहु_शुरू(ap, fmt);
    trc_infof(data, &Curl_trc_feat_ws, रिक्त, 0, fmt, ap);
    बहु_पूर्ण(ap);
  }
}
#endif /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#define TRC_CT_NONE        0
#define TRC_CT_PROTOCOL    (1 << 0)
#define TRC_CT_NETWORK     (1 << 1)
#define TRC_CT_PROXY       (1 << 2)
#define TRC_CT_INTERNALS   (1 << 3)

संरचना trc_feat_def {
  संरचना curl_trc_feat *feat;
  अचिन्हित पूर्णांक category;
};

स्थिर संरचना trc_feat_def trc_feats[] = {
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
#जर !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
  { &Curl_trc_feat_ws,        TRC_CT_PROTOCOL },
#endif
};

संरचना trc_cft_def {
  संरचना Curl_cftype *cft;
  अचिन्हित पूर्णांक category;
};

स्थिर संरचना trc_cft_def trc_cfts[] = {
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
#जर !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
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
#जर defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3)
  { &Curl_cft_h3_proxy,       TRC_CT_PROXY },
#endif
  { &Curl_cft_http_proxy,     TRC_CT_PROXY },
#endif /* !CURL_DISABLE_HTTP */
  { &Curl_cft_haproxy,        TRC_CT_PROXY },
  { &Curl_cft_socks_proxy,    TRC_CT_PROXY },
#endif /* !CURL_DISABLE_PROXY */
#जर !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { &Curl_cft_http3,          TRC_CT_PROTOCOL },
#endif
#ifndef CURL_DISABLE_HTTP
  { &Curl_cft_http_connect,   TRC_CT_PROTOCOL },
#endif
};

स्थिर शून्य trc_apply_level_by_name(संरचना Curl_str *token, पूर्णांक lvl)
{
  माप_प्रकार i;

  साठी(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    जर(curlx_str_casecompare(token, trc_cfts[i].cft->name)) {
      trc_cfts[i].cft->log_level = lvl;
      थांबा;
    }
  }
  साठी(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    जर(curlx_str_casecompare(token, trc_feats[i].feat->name)) {
      trc_feats[i].feat->log_level = lvl;
      थांबा;
    }
  }
}

स्थिर शून्य trc_apply_level_by_category(अचिन्हित पूर्णांक category, पूर्णांक lvl)
{
  माप_प्रकार i;

  साठी(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    जर(!category || (trc_cfts[i].category & category))
      trc_cfts[i].cft->log_level = lvl;
  }
  साठी(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    जर(!category || (trc_feats[i].category & category))
      trc_feats[i].feat->log_level = lvl;
  }
}

स्थिर CURLcode trc_opt(स्थिरांक अक्षर *config)
{
  संरचना Curl_str out;
  पर्यंत(!curlx_str_until(&config, &out, 32, ',')) {
    पूर्णांक lvl = CURL_LOG_LVL_INFO;
    स्थिरांक अक्षर *token = curlx_str(&out);

    जर(*token == '-') {
      lvl = CURL_LOG_LVL_NONE;
      curlx_str_nudge(&out, 1);
    }
    अन्यथा जर(*token == '+')
      curlx_str_nudge(&out, 1);

    जर(curlx_str_casecompare(&out, "all"))
      trc_apply_level_by_category(TRC_CT_NONE, lvl);
    अन्यथा जर(curlx_str_casecompare(&out, "protocol"))
      trc_apply_level_by_category(TRC_CT_PROTOCOL, lvl);
    अन्यथा जर(curlx_str_casecompare(&out, "network"))
      trc_apply_level_by_category(TRC_CT_NETWORK, lvl);
    अन्यथा जर(curlx_str_casecompare(&out, "proxy"))
      trc_apply_level_by_category(TRC_CT_PROXY, lvl);
    अन्यथा
      trc_apply_level_by_name(&out, lvl);

    जर(curlx_str_single(&config, ','))
      थांबा;
  }
  परत CURLE_OK;
}

CURLcode Curl_trc_opt(स्थिरांक अक्षर *config)
{
  CURLcode result = config ? trc_opt(config) : CURLE_OK;
#ifdef DEBUGBUILD
  /* CURL_DEBUG can override anything */
  जर(!result) {
    स्थिरांक अक्षर *dbg_config = दो_पर्या("CURL_DEBUG");
    जर(dbg_config)
      result = trc_opt(dbg_config);
  }
#endif /* DEBUGBUILD */
  परत result;
}

CURLcode Curl_trc_init(शून्य)
{
#ifdef DEBUGBUILD
  परत Curl_trc_opt(रिक्त);
#अन्यथा
  परत CURLE_OK;
#endif
}

#अन्यथा /* CURL_DISABLE_VERBOSE_STRINGS */

CURLcode Curl_trc_init(शून्य)
{
  परत CURLE_OK;
}

शून्य Curl_infof(संरचना Curl_easy *data, स्थिरांक अक्षर *fmt, ...)
{
  (शून्य)data;
  (शून्य)fmt;
}

शून्य Curl_trc_cf_infof(संरचना Curl_easy *data, स्थिरांक संरचना Curl_cfilter *cf,
                       स्थिरांक अक्षर *fmt, ...)
{
  (शून्य)data;
  (शून्य)cf;
  (शून्य)fmt;
}

शून्य Curl_trc_multi(संरचना Curl_easy *data, स्थिरांक अक्षर *fmt, ...)
{
  (शून्य)data;
  (शून्य)fmt;
}

शून्य Curl_trc_write(संरचना Curl_easy *data, स्थिरांक अक्षर *fmt, ...)
{
  (शून्य)data;
  (शून्य)fmt;
}

शून्य Curl_trc_dns(संरचना Curl_easy *data, स्थिरांक अक्षर *fmt, ...)
{
  (शून्य)data;
  (शून्य)fmt;
}

शून्य Curl_trc_timer(संरचना Curl_easy *data, पूर्णांक tid, स्थिरांक अक्षर *fmt, ...)
{
  (शून्य)data;
  (शून्य)tid;
  (शून्य)fmt;
}

शून्य Curl_trc_read(संरचना Curl_easy *data, स्थिरांक अक्षर *fmt, ...)
{
  (शून्य)data;
  (शून्य)fmt;
}

#ifndef CURL_DISABLE_FTP
शून्य Curl_trc_ftp(संरचना Curl_easy *data, स्थिरांक अक्षर *fmt, ...)
{
  (शून्य)data;
  (शून्य)fmt;
}
#endif
#ifndef CURL_DISABLE_SMTP
शून्य Curl_trc_smtp(संरचना Curl_easy *data, स्थिरांक अक्षर *fmt, ...)
{
  (शून्य)data;
  (शून्य)fmt;
}
#endif
#जर !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
शून्य Curl_trc_ws(संरचना Curl_easy *data, स्थिरांक अक्षर *fmt, ...)
{
  (शून्य)data;
  (शून्य)fmt;
}
#endif
#ifdef USE_SSH
शून्य Curl_trc_ssh(संरचना Curl_easy *data, स्थिरांक अक्षर *fmt, ...)
{
  (शून्य)data;
  (शून्य)fmt;
}
#endif
#ifdef USE_SSL
शून्य Curl_trc_ssls(संरचना Curl_easy *data, स्थिरांक अक्षर *fmt, ...)
{
  (शून्य)data;
  (शून्य)fmt;
}
#endif

#endif /* !CURL_DISABLE_VERBOSE_STRINGS */
