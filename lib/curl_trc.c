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

स्थिर शून्य trc_write(संरचना Curl_easy *दत्त, curl_infotype type,
                      ध्रुव वर्ण *ptr, size_t आकार)
{
  यदि(दत्त->समुच्चयः.verbose) {
    यदि(दत्त->समुच्चयः.fdebug) {
      संरचना Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, दत्त, easy_fdebug);
      (शून्य)(*दत्त->समुच्चयः.fdebug)(दत्त, type, CURL_UNCONST(ptr), आकार,
                                दत्त->समुच्चयः.debugdata);
      CURL_CBAPI_अन्तः(&guard);
    }
    अन्यथा {
      स्थिर ध्रुव वर्ण s_infotype[CURLINFO_अन्तः][3] = {
        "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
      विकल्प(type) {
      पक्ष CURLINFO_TEXT:
      पक्ष CURLINFO_शीर्षकम्_बहिर्गमनम्:
      पक्ष CURLINFO_शीर्षकम्_अन्तर्:
        fwrite(s_infotype[type], 2, 1, दत्त->समुच्चयः.err);
        fwrite(ptr, आकार, 1, दत्त->समुच्चयः.err);
        भङ्ग;
      स्वभाव: /* nada */
        भङ्ग;
      }
    }
  }
}

/* max length we trace before ending in '...' */
#परिभाषय TRC_पङ्क्तिः_MAX 2048

#परिभाषय CURL_TRC_FMT_IDSC   "[x-%" CURL_FORMAT_CURL_OFF_T "] "
#परिभाषय CURL_TRC_FMT_IDSD   "[%" CURL_FORMAT_CURL_OFF_T "-x] "
#परिभाषय CURL_TRC_FMT_IDSDC  "[%" CURL_FORMAT_CURL_OFF_T "-%" \
                            CURL_FORMAT_CURL_OFF_T "] "

स्थिर संरचना curl_trc_feat Curl_trc_feat_ids = {
  "LIB-IDS",
  CURL_LOG_LVL_NONE,
};
#परिभाषय CURL_TRC_IDS(दत्त) \
  (Curl_trc_is_verbose(दत्त) && \
  Curl_trc_feat_ids.log_level >= CURL_LOG_LVL_INFO)

स्थिर size_t trc_print_ids(संरचना Curl_easy *दत्त, वर्ण *buf, size_t maxlen)
{
  curl_off_t cid = दत्त->state.चरमः_conn_id;
  यदि(दत्त->id >= 0) {
    यदि(cid >= 0)
      प्रत्यागम curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSDC, दत्त->id, cid);
    अन्यथा
      प्रत्यागम curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSD, दत्त->id);
  }
  अन्यथा यदि(cid >= 0)
    प्रत्यागम curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSC, cid);
  अन्यथा {
    प्रत्यागम curl_msnprintf(buf, maxlen, "[x-x] ");
  }
}

स्थिर size_t trc_अन्तः_buf(वर्ण *buf, size_t len, size_t maxlen, bool addnl)
{
  /* make sure we end the trace line in `buf` properly. It needs
   * to end with a null-terminator '\0' or '\n\0' */
  यदि(len >= (maxlen - (addnl ? 2 : 1))) {
    len = maxlen - 5;
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '\n';
  }
  अन्यथा यदि(addnl)
    buf[len++] = '\n';
  buf[len] = '\0';
  प्रत्यागम len;
}

शून्य Curl_debug(संरचना Curl_easy *दत्त, curl_infotype type,
                ध्रुव वर्ण *ptr, size_t आकार)
{
  यदि(दत्त->समुच्चयः.verbose) {
    स्थिर ध्रुव वर्ण s_infotype[CURLINFO_अन्तः][3] = {
      "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
    वर्ण buf[TRC_पङ्क्तिः_MAX];
    size_t len;
    यदि(दत्त->समुच्चयः.fdebug) {
      संरचना Curl_mapi_guard guard;
      यदि(CURL_TRC_IDS(दत्त) && (आकार < TRC_पङ्क्तिः_MAX)) {
        len = trc_print_ids(दत्त, buf, TRC_पङ्क्तिः_MAX);
        len += curl_msnprintf(buf + len, TRC_पङ्क्तिः_MAX - len, "%.*s",
                              (पूर्णाङ्क)आकार, ptr);
        len = trc_अन्तः_buf(buf, len, TRC_पङ्क्तिः_MAX, FALSE);
        CURL_CBAPI_START(&guard, दत्त, easy_fdebug);
        (शून्य)(*दत्त->समुच्चयः.fdebug)(दत्त, type, buf, len, दत्त->समुच्चयः.debugdata);
        CURL_CBAPI_अन्तः(&guard);
      }
      अन्यथा {
        CURL_CBAPI_START(&guard, दत्त, easy_fdebug);
        (शून्य)(*दत्त->समुच्चयः.fdebug)(दत्त, type, CURL_UNCONST(ptr),
                                  आकार, दत्त->समुच्चयः.debugdata);
        CURL_CBAPI_अन्तः(&guard);
      }
    }
    अन्यथा {
      विकल्प(type) {
      पक्ष CURLINFO_TEXT:
      पक्ष CURLINFO_शीर्षकम्_बहिर्गमनम्:
      पक्ष CURLINFO_शीर्षकम्_अन्तर्:
#यदि_अपरिभाषितम् CURL_DISABLE_VERBOSE_वाक्यानि
        यदि(CURL_TRC_IDS(दत्त)) {
          len = trc_print_ids(दत्त, buf, TRC_पङ्क्तिः_MAX);
          fwrite(buf, len, 1, दत्त->समुच्चयः.err);
        }
#यदि_अन्तः
        fwrite(s_infotype[type], 2, 1, दत्त->समुच्चयः.err);
        fwrite(ptr, आकार, 1, दत्त->समुच्चयः.err);
        भङ्ग;
      स्वभाव: /* nada */
        भङ्ग;
      }
    }
  }
}

/* Curl_failf() is for messages stating why we failed.
 * The message SHALL NOT include any LF or CR.
 */
शून्य Curl_failf(संरचना Curl_easy *दत्त, ध्रुव वर्ण *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  यदि(दत्त->समुच्चयः.verbose || दत्त->समुच्चयः.errorbuffer) {
    va_list ap;
    size_t len;
    वर्ण error[CURL_ERROR_आकार + 2];
    va_start(ap, fmt);
    len = curl_mvsnprintf(error, CURL_ERROR_आकार, fmt, ap);

    यदि(दत्त->समुच्चयः.errorbuffer && !दत्त->state.errorbuf) {
      curlx_strcopy(दत्त->समुच्चयः.errorbuffer, CURL_ERROR_आकार, error, len);
      दत्त->state.errorbuf = TRUE; /* wrote error string */
    }
    error[len++] = '\n';
    error[len] = '\0';
    trc_write(दत्त, CURLINFO_TEXT, error, len);
    va_end(ap);
  }
}

शून्य Curl_reset_fail(संरचना Curl_easy *दत्त)
{
  यदि(दत्त->समुच्चयः.errorbuffer)
    दत्त->समुच्चयः.errorbuffer[0] = 0;
  दत्त->state.errorbuf = FALSE;
}

#यदि_परिभाषितम् CURLVERBOSE
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
#यदि_अपरिभाषितम् CURL_DISABLE_DOH
संरचना curl_trc_feat Curl_trc_feat_doh = {
  "DoH",
  CURL_LOG_LVL_NONE,
};
#यदि_अन्तः
संरचना curl_trc_feat Curl_trc_feat_timer = {
  "TIMER",
  CURL_LOG_LVL_NONE,
};
#यदि_परिभाषितम् USE_तन्तवः
संरचना curl_trc_feat Curl_trc_feat_तन्तवः = {
  "THREADS",
  CURL_LOG_LVL_NONE,
};
#यदि_अन्तः
#यदि_अन्तः

#यदि_अपरिभाषितम् CURL_DISABLE_VERBOSE_वाक्यानि

स्थिर शून्य trc_infof(संरचना Curl_easy *दत्त,
                      संरचना curl_trc_feat *feat,
                      ध्रुव वर्ण *opt_id, पूर्णाङ्क opt_id_idx,
                      ध्रुव वर्ण * ध्रुव fmt, va_list ap) CURL_PRINTF(5, 0);

स्थिर शून्य trc_infof(संरचना Curl_easy *दत्त,
                      संरचना curl_trc_feat *feat,
                      ध्रुव वर्ण *opt_id, पूर्णाङ्क opt_id_idx,
                      ध्रुव वर्ण * ध्रुव fmt, va_list ap)
{
  size_t len = 0;
  वर्ण buf[TRC_पङ्क्तिः_MAX];

  यदि(CURL_TRC_IDS(दत्त))
    len += trc_print_ids(दत्त, buf + len, TRC_पङ्क्तिः_MAX - len);
  यदि(feat)
    len += curl_msnprintf(buf + len, TRC_पङ्क्तिः_MAX - len, "[%s] ", feat->name);
  यदि(opt_id) {
    यदि(opt_id_idx > 0)
      len += curl_msnprintf(buf + len, TRC_पङ्क्तिः_MAX - len, "[%s-%d] ",
                            opt_id, opt_id_idx);
    अन्यथा
      len += curl_msnprintf(buf + len, TRC_पङ्क्तिः_MAX - len, "[%s] ", opt_id);
  }
  len += curl_mvsnprintf(buf + len, TRC_पङ्क्तिः_MAX - len, fmt, ap);
  len = trc_अन्तः_buf(buf, len, TRC_पङ्क्तिः_MAX, TRUE);
  trc_write(दत्त, CURLINFO_TEXT, buf, len);
}

शून्य Curl_infof(संरचना Curl_easy *दत्त, ध्रुव वर्ण *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  यदि(Curl_trc_is_verbose(दत्त)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(दत्त, दत्त->state.feat, रिक्त, 0, fmt, ap);
    va_end(ap);
  }
}

शून्य Curl_trc_cf_infof(संरचना Curl_easy *दत्त, ध्रुव संरचना Curl_cfilter *cf,
                       ध्रुव वर्ण *fmt, ...)
{
  DEBUGASSERT(cf);
  यदि(Curl_trc_cf_is_verbose(cf, दत्त)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(दत्त, दत्त->state.feat, cf->cft->name, cf->sockindex, fmt, ap);
    va_end(ap);
  }
}

शून्य Curl_trc_feat_infof(संरचना Curl_easy *दत्त,
                         संरचना curl_trc_feat *feat,
                         ध्रुव वर्ण *fmt, ...)
{
  DEBUGASSERT(feat);
  यदि(Curl_trc_ft_is_verbose(दत्त, feat)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(दत्त, feat, रिक्त, 0, fmt, ap);
    va_end(ap);
  }
}

स्थिर ध्रुव वर्ण * ध्रुव Curl_trc_timer_नामानि[] = {
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

स्थिर ध्रुव वर्ण *trc_timer_name(पूर्णाङ्क tid)
{
  यदि((tid >= 0) && ((size_t)tid < CURL_ARRAYSIZE(Curl_trc_timer_नामानि)))
    प्रत्यागम Curl_trc_timer_नामानि[(size_t)tid];
  प्रत्यागम "TIMER-???";
}

शून्य Curl_trc_timer(संरचना Curl_easy *दत्त, पूर्णाङ्क tid, ध्रुव वर्ण *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  यदि(Curl_trc_ft_is_verbose(दत्त, &Curl_trc_feat_timer)) {
    ध्रुव वर्ण *tname = trc_timer_name(tid);
    va_list ap;
    va_start(ap, fmt);
    trc_infof(दत्त, &Curl_trc_feat_timer, tname, 0, fmt, ap);
    va_end(ap);
  }
}

शून्य Curl_trc_easy_timers(संरचना Curl_easy *दत्त)
{
  यदि(CURL_TRC_TIMER_is_verbose(दत्त) && दत्त->multi) {
    यदि(दत्त->state.timeouts.first < EXPIRE_चरमः) {
      संरचना expire_timers *timeouts = &दत्त->state.timeouts;
      timediff_t आधारः_us =
        Curl_timeouts_विस्थापनम्_us(&दत्त->multi->timeouts, Curl_pgrs_now(दत्त));
      uint8_t id = दत्त->state.timeouts.first;
      प्रति(; id < EXPIRE_चरमः; id = timeouts->अग्र[id]) {
        CURL_TRC_TIMER(दत्त, id, "expires in %" FMT_TIMEDIFF_T "us",
                       timeouts->विस्थापनम्_us[id] - आधारः_us);
      }
    }
  }
}

स्थिर ध्रुव वर्ण * ध्रुव Curl_trc_mstate_नामानि[] = {
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

ध्रुव वर्ण *Curl_trc_mstate_name(पूर्णाङ्क state)
{
  यदि((state >= 0) && ((size_t)state < CURL_ARRAYSIZE(Curl_trc_mstate_नामानि)))
    प्रत्यागम Curl_trc_mstate_नामानि[(size_t)state];
  प्रत्यागम "?";
}

शून्य Curl_trc_multi(संरचना Curl_easy *दत्त, ध्रुव वर्ण *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  यदि(Curl_trc_ft_is_verbose(दत्त, &Curl_trc_feat_multi)) {
    ध्रुव वर्ण *sname = (दत्त->id >= 0) ?
                        Curl_trc_mstate_name(दत्त->mstate) : रिक्त;
    va_list ap;
    va_start(ap, fmt);
    trc_infof(दत्त, &Curl_trc_feat_multi, sname, 0, fmt, ap);
    va_end(ap);
  }
}

शून्य Curl_trc_read(संरचना Curl_easy *दत्त, ध्रुव वर्ण *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  यदि(Curl_trc_ft_is_verbose(दत्त, &Curl_trc_feat_read)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(दत्त, &Curl_trc_feat_read, रिक्त, 0, fmt, ap);
    va_end(ap);
  }
}

शून्य Curl_trc_write(संरचना Curl_easy *दत्त, ध्रुव वर्ण *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  यदि(Curl_trc_ft_is_verbose(दत्त, &Curl_trc_feat_write)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(दत्त, &Curl_trc_feat_write, रिक्त, 0, fmt, ap);
    va_end(ap);
  }
}

शून्य Curl_trc_dns(संरचना Curl_easy *दत्त, ध्रुव वर्ण *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  यदि(Curl_trc_ft_is_verbose(दत्त, &Curl_trc_feat_dns)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(दत्त, &Curl_trc_feat_dns, रिक्त, 0, fmt, ap);
    va_end(ap);
  }
}

#यदि_अपरिभाषितम् CURL_DISABLE_FTP
संरचना curl_trc_feat Curl_trc_feat_ftp = {
  "FTP",
  CURL_LOG_LVL_NONE,
};

शून्य Curl_trc_ftp(संरचना Curl_easy *दत्त, ध्रुव वर्ण *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  यदि(Curl_trc_ft_is_verbose(दत्त, &Curl_trc_feat_ftp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(दत्त, &Curl_trc_feat_ftp, रिक्त, 0, fmt, ap);
    va_end(ap);
  }
}
#यदि_अन्तः /* !CURL_DISABLE_FTP */

#यदि_अपरिभाषितम् CURL_DISABLE_SMTP
संरचना curl_trc_feat Curl_trc_feat_smtp = {
  "SMTP",
  CURL_LOG_LVL_NONE,
};

शून्य Curl_trc_smtp(संरचना Curl_easy *दत्त, ध्रुव वर्ण *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  यदि(Curl_trc_ft_is_verbose(दत्त, &Curl_trc_feat_smtp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(दत्त, &Curl_trc_feat_smtp, रिक्त, 0, fmt, ap);
    va_end(ap);
  }
}
#यदि_अन्तः /* !CURL_DISABLE_SMTP */

#यदि_परिभाषितम् USE_SSL
संरचना curl_trc_feat Curl_trc_feat_ssls = {
  "SSLS",
  CURL_LOG_LVL_NONE,
};

शून्य Curl_trc_ssls(संरचना Curl_easy *दत्त, ध्रुव वर्ण *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  यदि(Curl_trc_ft_is_verbose(दत्त, &Curl_trc_feat_ssls)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(दत्त, &Curl_trc_feat_ssls, रिक्त, 0, fmt, ap);
    va_end(ap);
  }
}
#यदि_अन्तः /* USE_SSL */

#यदि_परिभाषितम् USE_SSH
संरचना curl_trc_feat Curl_trc_feat_ssh = {
  "SSH",
  CURL_LOG_LVL_NONE,
};

शून्य Curl_trc_ssh(संरचना Curl_easy *दत्त, ध्रुव वर्ण *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  यदि(Curl_trc_ft_is_verbose(दत्त, &Curl_trc_feat_ssh)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(दत्त, &Curl_trc_feat_ssh, रिक्त, 0, fmt, ap);
    va_end(ap);
  }
}
#यदि_अन्तः /* USE_SSH */

#यदि !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
संरचना curl_trc_feat Curl_trc_feat_ws = {
  "WS",
  CURL_LOG_LVL_NONE,
};

शून्य Curl_trc_ws(संरचना Curl_easy *दत्त, ध्रुव वर्ण *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  यदि(Curl_trc_ft_is_verbose(दत्त, &Curl_trc_feat_ws)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(दत्त, &Curl_trc_feat_ws, रिक्त, 0, fmt, ap);
    va_end(ap);
  }
}
#यदि_अन्तः /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#परिभाषय TRC_CT_NONE        0
#परिभाषय TRC_CT_PROTOCOL    (1 << 0)
#परिभाषय TRC_CT_जालम्     (1 << 1)
#परिभाषय TRC_CT_PROXY       (1 << 2)
#परिभाषय TRC_CT_INTERNALS   (1 << 3)

संरचना trc_feat_def {
  संरचना curl_trc_feat *feat;
  अचिह्नित पूर्णाङ्क category;
};

स्थिर संरचना trc_feat_def trc_feats[] = {
  { &Curl_trc_feat_ids,       TRC_CT_INTERNALS },
  { &Curl_trc_feat_multi,     TRC_CT_जालम् },
  { &Curl_trc_feat_read,      TRC_CT_NONE },
  { &Curl_trc_feat_write,     TRC_CT_NONE },
  { &Curl_trc_feat_dns,       TRC_CT_जालम् },
#यदि_अपरिभाषितम् CURL_DISABLE_DOH
  { &Curl_trc_feat_doh,       TRC_CT_जालम् },
#यदि_अन्तः
  { &Curl_trc_feat_timer,     TRC_CT_जालम् },
#यदि_परिभाषितम् USE_तन्तवः
  { &Curl_trc_feat_तन्तवः,   TRC_CT_NONE },
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_FTP
  { &Curl_trc_feat_ftp,       TRC_CT_PROTOCOL },
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_SMTP
  { &Curl_trc_feat_smtp,      TRC_CT_PROTOCOL },
#यदि_अन्तः
#यदि_परिभाषितम् USE_SSL
  { &Curl_trc_feat_ssls,      TRC_CT_जालम् },
#यदि_अन्तः
#यदि_परिभाषितम् USE_SSH
  { &Curl_trc_feat_ssh,       TRC_CT_PROTOCOL },
#यदि_अन्तः
#यदि !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
  { &Curl_trc_feat_ws,        TRC_CT_PROTOCOL },
#यदि_अन्तः
};

संरचना trc_cft_def {
  संरचना Curl_cftype *cft;
  अचिह्नित पूर्णाङ्क category;
};

स्थिर संरचना trc_cft_def trc_cfts[] = {
  { &Curl_cft_dns,            TRC_CT_जालम् },
  { &Curl_cft_tcp,            TRC_CT_जालम् },
  { &Curl_cft_udp,            TRC_CT_जालम् },
  { &Curl_cft_unix,           TRC_CT_जालम् },
  { &Curl_cft_tcp_accept,     TRC_CT_जालम् },
  { &Curl_cft_ip_happy,       TRC_CT_जालम् },
#यदि_अपरिभाषितम् CURL_DISABLE_WEBSOCKETS
  { &Curl_cft_recvbuf,        TRC_CT_PROTOCOL },
#यदि_अन्तः
  { &Curl_cft_सज्जता,          TRC_CT_PROTOCOL },
#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  { &Curl_cft_nghttp2,        TRC_CT_PROTOCOL },
#यदि_अन्तः
#यदि_परिभाषितम् USE_SSL
  { &Curl_cft_ssl,            TRC_CT_जालम् },
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  { &Curl_cft_ssl_proxy,      TRC_CT_PROXY },
#यदि_अन्तः
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
#यदि_अपरिभाषितम् CURL_DISABLE_HTTP
  { &Curl_cft_h1_proxy,       TRC_CT_PROXY },
#यदि_परिभाषितम् USE_NGHTTP2
  { &Curl_cft_h2_proxy,       TRC_CT_PROXY },
#यदि_अन्तः
#यदि defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3)
  { &Curl_cft_h3_proxy,       TRC_CT_PROXY },
#यदि_अन्तः
  { &Curl_cft_http_proxy,     TRC_CT_PROXY },
#यदि_अन्तः /* !CURL_DISABLE_HTTP */
  { &Curl_cft_haproxy,        TRC_CT_PROXY },
  { &Curl_cft_socks_proxy,    TRC_CT_PROXY },
#यदि_अन्तः /* !CURL_DISABLE_PROXY */
#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { &Curl_cft_http3,          TRC_CT_PROTOCOL },
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_HTTP
  { &Curl_cft_http_connect,   TRC_CT_PROTOCOL },
#यदि_अन्तः
};

स्थिर शून्य trc_apply_level_by_name(संरचना Curl_str *token, पूर्णाङ्क lvl)
{
  size_t i;

  प्रति(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    यदि(curlx_str_casecompare(token, trc_cfts[i].cft->name)) {
      trc_cfts[i].cft->log_level = lvl;
      भङ्ग;
    }
  }
  प्रति(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    यदि(curlx_str_casecompare(token, trc_feats[i].feat->name)) {
      trc_feats[i].feat->log_level = lvl;
      भङ्ग;
    }
  }
}

स्थिर शून्य trc_apply_level_by_category(अचिह्नित पूर्णाङ्क category, पूर्णाङ्क lvl)
{
  size_t i;

  प्रति(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    यदि(!category || (trc_cfts[i].category & category))
      trc_cfts[i].cft->log_level = lvl;
  }
  प्रति(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    यदि(!category || (trc_feats[i].category & category))
      trc_feats[i].feat->log_level = lvl;
  }
}

स्थिर CURLcode trc_opt(ध्रुव वर्ण *config)
{
  संरचना Curl_str बहिर्गमनम्;
  यावत्(!curlx_str_until(&config, &बहिर्गमनम्, 32, ',')) {
    पूर्णाङ्क lvl = CURL_LOG_LVL_INFO;
    ध्रुव वर्ण *token = curlx_str(&बहिर्गमनम्);

    यदि(*token == '-') {
      lvl = CURL_LOG_LVL_NONE;
      curlx_str_nudge(&बहिर्गमनम्, 1);
    }
    अन्यथा यदि(*token == '+')
      curlx_str_nudge(&बहिर्गमनम्, 1);

    यदि(curlx_str_casecompare(&बहिर्गमनम्, "all"))
      trc_apply_level_by_category(TRC_CT_NONE, lvl);
    अन्यथा यदि(curlx_str_casecompare(&बहिर्गमनम्, "protocol"))
      trc_apply_level_by_category(TRC_CT_PROTOCOL, lvl);
    अन्यथा यदि(curlx_str_casecompare(&बहिर्गमनम्, "network"))
      trc_apply_level_by_category(TRC_CT_जालम्, lvl);
    अन्यथा यदि(curlx_str_casecompare(&बहिर्गमनम्, "proxy"))
      trc_apply_level_by_category(TRC_CT_PROXY, lvl);
    अन्यथा
      trc_apply_level_by_name(&बहिर्गमनम्, lvl);

    यदि(curlx_str_single(&config, ','))
      भङ्ग;
  }
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_trc_opt(ध्रुव वर्ण *config)
{
  CURLcode परिणाम = config ? trc_opt(config) : CURLE_OK;
#यदि_परिभाषितम् DEBUGBUILD
  /* CURL_DEBUG can override anything */
  यदि(!परिणाम) {
    ध्रुव वर्ण *dbg_config = getenv("CURL_DEBUG");
    यदि(dbg_config)
      परिणाम = trc_opt(dbg_config);
  }
#यदि_अन्तः /* DEBUGBUILD */
  प्रत्यागम परिणाम;
}

CURLcode Curl_trc_init(शून्य)
{
#यदि_परिभाषितम् DEBUGBUILD
  प्रत्यागम Curl_trc_opt(रिक्त);
#अन्यथा
  प्रत्यागम CURLE_OK;
#यदि_अन्तः
}

#अन्यथा /* CURL_DISABLE_VERBOSE_STRINGS */

CURLcode Curl_trc_init(शून्य)
{
  प्रत्यागम CURLE_OK;
}

शून्य Curl_infof(संरचना Curl_easy *दत्त, ध्रुव वर्ण *fmt, ...)
{
  (शून्य)दत्त;
  (शून्य)fmt;
}

शून्य Curl_trc_cf_infof(संरचना Curl_easy *दत्त, ध्रुव संरचना Curl_cfilter *cf,
                       ध्रुव वर्ण *fmt, ...)
{
  (शून्य)दत्त;
  (शून्य)cf;
  (शून्य)fmt;
}

शून्य Curl_trc_multi(संरचना Curl_easy *दत्त, ध्रुव वर्ण *fmt, ...)
{
  (शून्य)दत्त;
  (शून्य)fmt;
}

शून्य Curl_trc_write(संरचना Curl_easy *दत्त, ध्रुव वर्ण *fmt, ...)
{
  (शून्य)दत्त;
  (शून्य)fmt;
}

शून्य Curl_trc_dns(संरचना Curl_easy *दत्त, ध्रुव वर्ण *fmt, ...)
{
  (शून्य)दत्त;
  (शून्य)fmt;
}

शून्य Curl_trc_timer(संरचना Curl_easy *दत्त, पूर्णाङ्क tid, ध्रुव वर्ण *fmt, ...)
{
  (शून्य)दत्त;
  (शून्य)tid;
  (शून्य)fmt;
}

शून्य Curl_trc_read(संरचना Curl_easy *दत्त, ध्रुव वर्ण *fmt, ...)
{
  (शून्य)दत्त;
  (शून्य)fmt;
}

#यदि_अपरिभाषितम् CURL_DISABLE_FTP
शून्य Curl_trc_ftp(संरचना Curl_easy *दत्त, ध्रुव वर्ण *fmt, ...)
{
  (शून्य)दत्त;
  (शून्य)fmt;
}
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_SMTP
शून्य Curl_trc_smtp(संरचना Curl_easy *दत्त, ध्रुव वर्ण *fmt, ...)
{
  (शून्य)दत्त;
  (शून्य)fmt;
}
#यदि_अन्तः
#यदि !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
शून्य Curl_trc_ws(संरचना Curl_easy *दत्त, ध्रुव वर्ण *fmt, ...)
{
  (शून्य)दत्त;
  (शून्य)fmt;
}
#यदि_अन्तः
#यदि_परिभाषितम् USE_SSH
शून्य Curl_trc_ssh(संरचना Curl_easy *दत्त, ध्रुव वर्ण *fmt, ...)
{
  (शून्य)दत्त;
  (शून्य)fmt;
}
#यदि_अन्तः
#यदि_परिभाषितम् USE_SSL
शून्य Curl_trc_ssls(संरचना Curl_easy *दत्त, ध्रुव वर्ण *fmt, ...)
{
  (शून्य)दत्त;
  (शून्य)fmt;
}
#यदि_अन्तः

#यदि_अन्तः /* !CURL_DISABLE_VERBOSE_STRINGS */
