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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "multiif.h"

#অন্তর্ভাব "cf-recvbuf.h"
#অন্তর্ভাব "cf-socket.h"
#অন্তর্ভাব "cf-setup.h"
#অন্তর্ভাব "http2.h"
#অন্তর্ভাব "http_proxy.h"
#অন্তর্ভাব "cf-h1-proxy.h"
#অন্তর্ভাব "cf-h2-proxy.h"
#অন্তর্ভাব "cf-haproxy.h"
#অন্তর্ভাব "cf-https-connect.h"
#অন্তর্ভাব "cf-ip-happy.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "socks.h"
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "vdns/cf-dns.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "vquic/vquic.h"
#অন্তর্ভাব "curlx/strcopy.h"

স্থির শূন্য trc_write(কাঠামো Curl_easy *তথ্য, curl_infotype type,
                      ধ্রুবক অক্ষর *ptr, size_t আকার_2)
{
  যদি(তথ্য->set.verbose) {
    যদি(তথ্য->set.fdebug) {
      কাঠামো Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, তথ্য, easy_fdebug);
      (শূন্য)(*তথ্য->set.fdebug)(তথ্য, type, CURL_UNCONST(ptr), আকার_2,
                                তথ্য->set.debugdata);
      CURL_CBAPI_END(&guard);
    }
    অন্যথা {
      স্থির ধ্রুবক অক্ষর s_infotype[CURLINFO_END][3] = {
        "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
      নির্বাচন(type) {
      ক্ষেত্র CURLINFO_TEXT:
      ক্ষেত্র CURLINFO_HEADER_OUT:
      ক্ষেত্র CURLINFO_HEADER_IN:
        fwrite(s_infotype[type], 2, 1, তথ্য->set.err);
        fwrite(ptr, আকার_2, 1, তথ্য->set.err);
        বিরতি;
      পূর্বনির্ধারিত: /* nada */
        বিরতি;
      }
    }
  }
}

/* max length we trace before ending in '...' */
#define TRC_সারি_MAX 2048

#define CURL_TRC_FMT_IDSC   "[x-%" CURL_FORMAT_CURL_OFF_T "] "
#define CURL_TRC_FMT_IDSD   "[%" CURL_FORMAT_CURL_OFF_T "-x] "
#define CURL_TRC_FMT_IDSDC  "[%" CURL_FORMAT_CURL_OFF_T "-%" \
                            CURL_FORMAT_CURL_OFF_T "] "

স্থির কাঠামো curl_trc_feat Curl_trc_feat_ids = {
  "LIB-IDS",
  CURL_LOG_LVL_NONE,
};
#define CURL_TRC_IDS(তথ্য) \
  (Curl_trc_is_verbose(তথ্য) && \
  Curl_trc_feat_ids.log_level >= CURL_LOG_LVL_INFO)

স্থির size_t trc_print_ids(কাঠামো Curl_easy *তথ্য, অক্ষর *buf, size_t maxlen)
{
  curl_off_t cid = তথ্য->state.last_conn_id;
  যদি(তথ্য->id >= 0) {
    যদি(cid >= 0)
      ফেরত curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSDC, তথ্য->id, cid);
    অন্যথা
      ফেরত curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSD, তথ্য->id);
  }
  অন্যথা যদি(cid >= 0)
    ফেরত curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSC, cid);
  অন্যথা {
    ফেরত curl_msnprintf(buf, maxlen, "[x-x] ");
  }
}

স্থির size_t trc_end_buf(অক্ষর *buf, size_t len, size_t maxlen, bool addnl)
{
  /* make sure we end the trace line in `buf` properly. It needs
   * to end with a null-terminator '\0' or '\n\0' */
  যদি(len >= (maxlen - (addnl ? 2 : 1))) {
    len = maxlen - 5;
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '\n';
  }
  অন্যথা যদি(addnl)
    buf[len++] = '\n';
  buf[len] = '\0';
  ফেরত len;
}

শূন্য Curl_debug(কাঠামো Curl_easy *তথ্য, curl_infotype type,
                ধ্রুবক অক্ষর *ptr, size_t আকার_2)
{
  যদি(তথ্য->set.verbose) {
    স্থির ধ্রুবক অক্ষর s_infotype[CURLINFO_END][3] = {
      "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
    অক্ষর buf[TRC_সারি_MAX];
    size_t len;
    যদি(তথ্য->set.fdebug) {
      কাঠামো Curl_mapi_guard guard;
      যদি(CURL_TRC_IDS(তথ্য) && (আকার_2 < TRC_সারি_MAX)) {
        len = trc_print_ids(তথ্য, buf, TRC_সারি_MAX);
        len += curl_msnprintf(buf + len, TRC_সারি_MAX - len, "%.*s",
                              (পূর্ণসংখ্যা)আকার_2, ptr);
        len = trc_end_buf(buf, len, TRC_সারি_MAX, FALSE);
        CURL_CBAPI_START(&guard, তথ্য, easy_fdebug);
        (শূন্য)(*তথ্য->set.fdebug)(তথ্য, type, buf, len, তথ্য->set.debugdata);
        CURL_CBAPI_END(&guard);
      }
      অন্যথা {
        CURL_CBAPI_START(&guard, তথ্য, easy_fdebug);
        (শূন্য)(*তথ্য->set.fdebug)(তথ্য, type, CURL_UNCONST(ptr),
                                  আকার_2, তথ্য->set.debugdata);
        CURL_CBAPI_END(&guard);
      }
    }
    অন্যথা {
      নির্বাচন(type) {
      ক্ষেত্র CURLINFO_TEXT:
      ক্ষেত্র CURLINFO_HEADER_OUT:
      ক্ষেত্র CURLINFO_HEADER_IN:
#ifndef CURL_DISABLE_VERBOSE_STRINGS
        যদি(CURL_TRC_IDS(তথ্য)) {
          len = trc_print_ids(তথ্য, buf, TRC_সারি_MAX);
          fwrite(buf, len, 1, তথ্য->set.err);
        }
#endif
        fwrite(s_infotype[type], 2, 1, তথ্য->set.err);
        fwrite(ptr, আকার_2, 1, তথ্য->set.err);
        বিরতি;
      পূর্বনির্ধারিত: /* nada */
        বিরতি;
      }
    }
  }
}

/* Curl_failf() is for messages stating why we failed.
 * The message SHALL NOT include any LF or CR.
 */
শূন্য Curl_failf(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  যদি(তথ্য->set.verbose || তথ্য->set.errorbuffer) {
    va_list ap;
    size_t len;
    অক্ষর error[CURL_ERROR_আকার + 2];
    va_start(ap, fmt);
    len = curl_mvsnprintf(error, CURL_ERROR_আকার, fmt, ap);

    যদি(তথ্য->set.errorbuffer && !তথ্য->state.errorbuf) {
      curlx_strcopy(তথ্য->set.errorbuffer, CURL_ERROR_আকার, error, len);
      তথ্য->state.errorbuf = TRUE; /* wrote error string */
    }
    error[len++] = '\n';
    error[len] = '\0';
    trc_write(তথ্য, CURLINFO_TEXT, error, len);
    va_end(ap);
  }
}

শূন্য Curl_reset_fail(কাঠামো Curl_easy *তথ্য)
{
  যদি(তথ্য->set.errorbuffer)
    তথ্য->set.errorbuffer[0] = 0;
  তথ্য->state.errorbuf = FALSE;
}

#ifdef CURLVERBOSE
কাঠামো curl_trc_feat Curl_trc_feat_multi = {
  "MULTI",
  CURL_LOG_LVL_NONE,
};
কাঠামো curl_trc_feat Curl_trc_feat_read = {
  "READ",
  CURL_LOG_LVL_NONE,
};
কাঠামো curl_trc_feat Curl_trc_feat_write = {
  "WRITE",
  CURL_LOG_LVL_NONE,
};
কাঠামো curl_trc_feat Curl_trc_feat_dns = {
  "DNS",
  CURL_LOG_LVL_NONE,
};
#ifndef CURL_DISABLE_DOH
কাঠামো curl_trc_feat Curl_trc_feat_doh = {
  "DoH",
  CURL_LOG_LVL_NONE,
};
#endif
কাঠামো curl_trc_feat Curl_trc_feat_timer = {
  "TIMER",
  CURL_LOG_LVL_NONE,
};
#ifdef USE_THREADS
কাঠামো curl_trc_feat Curl_trc_feat_threads = {
  "THREADS",
  CURL_LOG_LVL_NONE,
};
#endif
#endif

#ifndef CURL_DISABLE_VERBOSE_STRINGS

স্থির শূন্য trc_infof(কাঠামো Curl_easy *তথ্য,
                      কাঠামো curl_trc_feat *feat,
                      ধ্রুবক অক্ষর *opt_id, পূর্ণসংখ্যা opt_id_idx,
                      ধ্রুবক অক্ষর * ধ্রুবক fmt, va_list ap) CURL_PRINTF(5, 0);

স্থির শূন্য trc_infof(কাঠামো Curl_easy *তথ্য,
                      কাঠামো curl_trc_feat *feat,
                      ধ্রুবক অক্ষর *opt_id, পূর্ণসংখ্যা opt_id_idx,
                      ধ্রুবক অক্ষর * ধ্রুবক fmt, va_list ap)
{
  size_t len = 0;
  অক্ষর buf[TRC_সারি_MAX];

  যদি(CURL_TRC_IDS(তথ্য))
    len += trc_print_ids(তথ্য, buf + len, TRC_সারি_MAX - len);
  যদি(feat)
    len += curl_msnprintf(buf + len, TRC_সারি_MAX - len, "[%s] ", feat->name);
  যদি(opt_id) {
    যদি(opt_id_idx > 0)
      len += curl_msnprintf(buf + len, TRC_সারি_MAX - len, "[%s-%d] ",
                            opt_id, opt_id_idx);
    অন্যথা
      len += curl_msnprintf(buf + len, TRC_সারি_MAX - len, "[%s] ", opt_id);
  }
  len += curl_mvsnprintf(buf + len, TRC_সারি_MAX - len, fmt, ap);
  len = trc_end_buf(buf, len, TRC_সারি_MAX, TRUE);
  trc_write(তথ্য, CURLINFO_TEXT, buf, len);
}

শূন্য Curl_infof(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  যদি(Curl_trc_is_verbose(তথ্য)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(তথ্য, তথ্য->state.feat, ফাঁকা, 0, fmt, ap);
    va_end(ap);
  }
}

শূন্য Curl_trc_cf_infof(কাঠামো Curl_easy *তথ্য, ধ্রুবক কাঠামো Curl_cfilter *cf,
                       ধ্রুবক অক্ষর *fmt, ...)
{
  DEBUGASSERT(cf);
  যদি(Curl_trc_cf_is_verbose(cf, তথ্য)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(তথ্য, তথ্য->state.feat, cf->cft->name, cf->sockindex, fmt, ap);
    va_end(ap);
  }
}

শূন্য Curl_trc_feat_infof(কাঠামো Curl_easy *তথ্য,
                         কাঠামো curl_trc_feat *feat,
                         ধ্রুবক অক্ষর *fmt, ...)
{
  DEBUGASSERT(feat);
  যদি(Curl_trc_ft_is_verbose(তথ্য, feat)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(তথ্য, feat, ফাঁকা, 0, fmt, ap);
    va_end(ap);
  }
}

স্থির ধ্রুবক অক্ষর * ধ্রুবক Curl_trc_timer_names[] = {
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

স্থির ধ্রুবক অক্ষর *trc_timer_name(পূর্ণসংখ্যা tid)
{
  যদি((tid >= 0) && ((size_t)tid < CURL_ARRAYSIZE(Curl_trc_timer_names)))
    ফেরত Curl_trc_timer_names[(size_t)tid];
  ফেরত "TIMER-???";
}

শূন্য Curl_trc_timer(কাঠামো Curl_easy *তথ্য, পূর্ণসংখ্যা tid, ধ্রুবক অক্ষর *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  যদি(Curl_trc_ft_is_verbose(তথ্য, &Curl_trc_feat_timer)) {
    ধ্রুবক অক্ষর *tname = trc_timer_name(tid);
    va_list ap;
    va_start(ap, fmt);
    trc_infof(তথ্য, &Curl_trc_feat_timer, tname, 0, fmt, ap);
    va_end(ap);
  }
}

শূন্য Curl_trc_easy_timers(কাঠামো Curl_easy *তথ্য)
{
  যদি(CURL_TRC_TIMER_is_verbose(তথ্য) && তথ্য->multi) {
    যদি(তথ্য->state.timeouts.first < EXPIRE_LAST) {
      কাঠামো expire_timers *timeouts = &তথ্য->state.timeouts;
      timediff_t base_us =
        Curl_timeouts_offset_us(&তথ্য->multi->timeouts, Curl_pgrs_now(তথ্য));
      uint8_t id = তথ্য->state.timeouts.first;
      জন্য(; id < EXPIRE_LAST; id = timeouts->পরবর্তী[id]) {
        CURL_TRC_TIMER(তথ্য, id, "expires in %" FMT_TIMEDIFF_T "us",
                       timeouts->offset_us[id] - base_us);
      }
    }
  }
}

স্থির ধ্রুবক অক্ষর * ধ্রুবক Curl_trc_mstate_names[] = {
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

ধ্রুবক অক্ষর *Curl_trc_mstate_name(পূর্ণসংখ্যা state)
{
  যদি((state >= 0) && ((size_t)state < CURL_ARRAYSIZE(Curl_trc_mstate_names)))
    ফেরত Curl_trc_mstate_names[(size_t)state];
  ফেরত "?";
}

শূন্য Curl_trc_multi(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  যদি(Curl_trc_ft_is_verbose(তথ্য, &Curl_trc_feat_multi)) {
    ধ্রুবক অক্ষর *sname = (তথ্য->id >= 0) ?
                        Curl_trc_mstate_name(তথ্য->mstate) : ফাঁকা;
    va_list ap;
    va_start(ap, fmt);
    trc_infof(তথ্য, &Curl_trc_feat_multi, sname, 0, fmt, ap);
    va_end(ap);
  }
}

শূন্য Curl_trc_read(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  যদি(Curl_trc_ft_is_verbose(তথ্য, &Curl_trc_feat_read)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(তথ্য, &Curl_trc_feat_read, ফাঁকা, 0, fmt, ap);
    va_end(ap);
  }
}

শূন্য Curl_trc_write(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  যদি(Curl_trc_ft_is_verbose(তথ্য, &Curl_trc_feat_write)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(তথ্য, &Curl_trc_feat_write, ফাঁকা, 0, fmt, ap);
    va_end(ap);
  }
}

শূন্য Curl_trc_dns(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  যদি(Curl_trc_ft_is_verbose(তথ্য, &Curl_trc_feat_dns)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(তথ্য, &Curl_trc_feat_dns, ফাঁকা, 0, fmt, ap);
    va_end(ap);
  }
}

#ifndef CURL_DISABLE_FTP
কাঠামো curl_trc_feat Curl_trc_feat_ftp = {
  "FTP",
  CURL_LOG_LVL_NONE,
};

শূন্য Curl_trc_ftp(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  যদি(Curl_trc_ft_is_verbose(তথ্য, &Curl_trc_feat_ftp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(তথ্য, &Curl_trc_feat_ftp, ফাঁকা, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* !CURL_DISABLE_FTP */

#ifndef CURL_DISABLE_SMTP
কাঠামো curl_trc_feat Curl_trc_feat_smtp = {
  "SMTP",
  CURL_LOG_LVL_NONE,
};

শূন্য Curl_trc_smtp(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  যদি(Curl_trc_ft_is_verbose(তথ্য, &Curl_trc_feat_smtp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(তথ্য, &Curl_trc_feat_smtp, ফাঁকা, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* !CURL_DISABLE_SMTP */

#ifdef USE_SSL
কাঠামো curl_trc_feat Curl_trc_feat_ssls = {
  "SSLS",
  CURL_LOG_LVL_NONE,
};

শূন্য Curl_trc_ssls(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  যদি(Curl_trc_ft_is_verbose(তথ্য, &Curl_trc_feat_ssls)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(তথ্য, &Curl_trc_feat_ssls, ফাঁকা, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* USE_SSL */

#ifdef USE_SSH
কাঠামো curl_trc_feat Curl_trc_feat_ssh = {
  "SSH",
  CURL_LOG_LVL_NONE,
};

শূন্য Curl_trc_ssh(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  যদি(Curl_trc_ft_is_verbose(তথ্য, &Curl_trc_feat_ssh)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(তথ্য, &Curl_trc_feat_ssh, ফাঁকা, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* USE_SSH */

#যদি !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
কাঠামো curl_trc_feat Curl_trc_feat_ws = {
  "WS",
  CURL_LOG_LVL_NONE,
};

শূন্য Curl_trc_ws(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  যদি(Curl_trc_ft_is_verbose(তথ্য, &Curl_trc_feat_ws)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(তথ্য, &Curl_trc_feat_ws, ফাঁকা, 0, fmt, ap);
    va_end(ap);
  }
}
#endif /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#define TRC_CT_NONE        0
#define TRC_CT_PROTOCOL    (1 << 0)
#define TRC_CT_NETWORK     (1 << 1)
#define TRC_CT_PROXY       (1 << 2)
#define TRC_CT_INTERNALS   (1 << 3)

কাঠামো trc_feat_def {
  কাঠামো curl_trc_feat *feat;
  unsigned পূর্ণসংখ্যা category;
};

স্থির কাঠামো trc_feat_def trc_feats[] = {
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
#যদি !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
  { &Curl_trc_feat_ws,        TRC_CT_PROTOCOL },
#endif
};

কাঠামো trc_cft_def {
  কাঠামো Curl_cftype *cft;
  unsigned পূর্ণসংখ্যা category;
};

স্থির কাঠামো trc_cft_def trc_cfts[] = {
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
#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
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
#যদি defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3)
  { &Curl_cft_h3_proxy,       TRC_CT_PROXY },
#endif
  { &Curl_cft_http_proxy,     TRC_CT_PROXY },
#endif /* !CURL_DISABLE_HTTP */
  { &Curl_cft_haproxy,        TRC_CT_PROXY },
  { &Curl_cft_socks_proxy,    TRC_CT_PROXY },
#endif /* !CURL_DISABLE_PROXY */
#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { &Curl_cft_http3,          TRC_CT_PROTOCOL },
#endif
#ifndef CURL_DISABLE_HTTP
  { &Curl_cft_http_connect,   TRC_CT_PROTOCOL },
#endif
};

স্থির শূন্য trc_apply_level_by_name(কাঠামো Curl_str *token, পূর্ণসংখ্যা lvl)
{
  size_t i;

  জন্য(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    যদি(curlx_str_casecompare(token, trc_cfts[i].cft->name)) {
      trc_cfts[i].cft->log_level = lvl;
      বিরতি;
    }
  }
  জন্য(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    যদি(curlx_str_casecompare(token, trc_feats[i].feat->name)) {
      trc_feats[i].feat->log_level = lvl;
      বিরতি;
    }
  }
}

স্থির শূন্য trc_apply_level_by_category(unsigned পূর্ণসংখ্যা category, পূর্ণসংখ্যা lvl)
{
  size_t i;

  জন্য(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    যদি(!category || (trc_cfts[i].category & category))
      trc_cfts[i].cft->log_level = lvl;
  }
  জন্য(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    যদি(!category || (trc_feats[i].category & category))
      trc_feats[i].feat->log_level = lvl;
  }
}

স্থির CURLcode trc_opt(ধ্রুবক অক্ষর *config)
{
  কাঠামো Curl_str out;
  যখন(!curlx_str_until(&config, &out, 32, ',')) {
    পূর্ণসংখ্যা lvl = CURL_LOG_LVL_INFO;
    ধ্রুবক অক্ষর *token = curlx_str(&out);

    যদি(*token == '-') {
      lvl = CURL_LOG_LVL_NONE;
      curlx_str_nudge(&out, 1);
    }
    অন্যথা যদি(*token == '+')
      curlx_str_nudge(&out, 1);

    যদি(curlx_str_casecompare(&out, "all"))
      trc_apply_level_by_category(TRC_CT_NONE, lvl);
    অন্যথা যদি(curlx_str_casecompare(&out, "protocol"))
      trc_apply_level_by_category(TRC_CT_PROTOCOL, lvl);
    অন্যথা যদি(curlx_str_casecompare(&out, "network"))
      trc_apply_level_by_category(TRC_CT_NETWORK, lvl);
    অন্যথা যদি(curlx_str_casecompare(&out, "proxy"))
      trc_apply_level_by_category(TRC_CT_PROXY, lvl);
    অন্যথা
      trc_apply_level_by_name(&out, lvl);

    যদি(curlx_str_single(&config, ','))
      বিরতি;
  }
  ফেরত CURLE_OK;
}

CURLcode Curl_trc_opt(ধ্রুবক অক্ষর *config)
{
  CURLcode ফলাফল = config ? trc_opt(config) : CURLE_OK;
#ifdef DEBUGBUILD
  /* CURL_DEBUG can override anything */
  যদি(!ফলাফল) {
    ধ্রুবক অক্ষর *dbg_config = getenv("CURL_DEBUG");
    যদি(dbg_config)
      ফলাফল = trc_opt(dbg_config);
  }
#endif /* DEBUGBUILD */
  ফেরত ফলাফল;
}

CURLcode Curl_trc_init(শূন্য)
{
#ifdef DEBUGBUILD
  ফেরত Curl_trc_opt(ফাঁকা);
#অন্যথা
  ফেরত CURLE_OK;
#endif
}

#অন্যথা /* CURL_DISABLE_VERBOSE_STRINGS */

CURLcode Curl_trc_init(শূন্য)
{
  ফেরত CURLE_OK;
}

শূন্য Curl_infof(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *fmt, ...)
{
  (শূন্য)তথ্য;
  (শূন্য)fmt;
}

শূন্য Curl_trc_cf_infof(কাঠামো Curl_easy *তথ্য, ধ্রুবক কাঠামো Curl_cfilter *cf,
                       ধ্রুবক অক্ষর *fmt, ...)
{
  (শূন্য)তথ্য;
  (শূন্য)cf;
  (শূন্য)fmt;
}

শূন্য Curl_trc_multi(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *fmt, ...)
{
  (শূন্য)তথ্য;
  (শূন্য)fmt;
}

শূন্য Curl_trc_write(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *fmt, ...)
{
  (শূন্য)তথ্য;
  (শূন্য)fmt;
}

শূন্য Curl_trc_dns(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *fmt, ...)
{
  (শূন্য)তথ্য;
  (শূন্য)fmt;
}

শূন্য Curl_trc_timer(কাঠামো Curl_easy *তথ্য, পূর্ণসংখ্যা tid, ধ্রুবক অক্ষর *fmt, ...)
{
  (শূন্য)তথ্য;
  (শূন্য)tid;
  (শূন্য)fmt;
}

শূন্য Curl_trc_read(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *fmt, ...)
{
  (শূন্য)তথ্য;
  (শূন্য)fmt;
}

#ifndef CURL_DISABLE_FTP
শূন্য Curl_trc_ftp(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *fmt, ...)
{
  (শূন্য)তথ্য;
  (শূন্য)fmt;
}
#endif
#ifndef CURL_DISABLE_SMTP
শূন্য Curl_trc_smtp(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *fmt, ...)
{
  (শূন্য)তথ্য;
  (শূন্য)fmt;
}
#endif
#যদি !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
শূন্য Curl_trc_ws(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *fmt, ...)
{
  (শূন্য)তথ্য;
  (শূন্য)fmt;
}
#endif
#ifdef USE_SSH
শূন্য Curl_trc_ssh(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *fmt, ...)
{
  (শূন্য)তথ্য;
  (শূন্য)fmt;
}
#endif
#ifdef USE_SSL
শূন্য Curl_trc_ssls(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *fmt, ...)
{
  (শূন্য)তথ্য;
  (শূন্য)fmt;
}
#endif

#endif /* !CURL_DISABLE_VERBOSE_STRINGS */
