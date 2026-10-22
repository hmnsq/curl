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

ಸ್ಥಿರ ಶೂನ್ಯ trc_write(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, curl_infotype type,
                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr, size_t ಗಾತ್ರ_2)
{
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.verbose) {
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.fdebug) {
      ರಚನೆ Curl_mapi_guard guard;
      CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_fdebug);
      (ಶೂನ್ಯ)(*ದತ್ತಾಂಶ->ಗಣ.fdebug)(ದತ್ತಾಂಶ, type, CURL_UNCONST(ptr), ಗಾತ್ರ_2,
                                ದತ್ತಾಂಶ->ಗಣ.debugdata);
      CURL_CBAPI_ಕೊನೆ(&guard);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ s_infotype[CURLINFO_ಕೊನೆ][3] = {
        "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
      ಆಯ್ಕೆ(type) {
      ಸ್ಥಿತಿ CURLINFO_ಪಠ್ಯ:
      ಸ್ಥಿತಿ CURLINFO_ಶೀರ್ಷಿಕೆ_ಹೊರಹೋಗು:
      ಸ್ಥಿತಿ CURLINFO_ಶೀರ್ಷಿಕೆ_ಒಳ:
        fwrite(s_infotype[type], 2, 1, ದತ್ತಾಂಶ->ಗಣ.err);
        fwrite(ptr, ಗಾತ್ರ_2, 1, ದತ್ತಾಂಶ->ಗಣ.err);
        ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ: /* nada */
        ನಿಲ್ಲಿಸು;
      }
    }
  }
}

/* max length we trace before ending in '...' */
#ವ್ಯಾಖ್ಯಾನಿಸು TRC_ಸಾಲು_MAX 2048

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_FMT_IDSC   "[x-%" CURL_FORMAT_CURL_OFF_T "] "
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_FMT_IDSD   "[%" CURL_FORMAT_CURL_OFF_T "-x] "
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_FMT_IDSDC  "[%" CURL_FORMAT_CURL_OFF_T "-%" \
                            CURL_FORMAT_CURL_OFF_T "] "

ಸ್ಥಿರ ರಚನೆ curl_trc_feat Curl_trc_feat_ids = {
  "LIB-IDS",
  CURL_LOG_LVL_NONE,
};
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_IDS(ದತ್ತಾಂಶ) \
  (Curl_trc_is_verbose(ದತ್ತಾಂಶ) && \
  Curl_trc_feat_ids.log_level >= CURL_LOG_LVL_INFO)

ಸ್ಥಿರ size_t trc_print_ids(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಅಕ್ಷರ *buf, size_t maxlen)
{
  curl_off_t cid = ದತ್ತಾಂಶ->state.ಕೊನೆಯ_conn_id;
  ಆದರೆ(ದತ್ತಾಂಶ->id >= 0) {
    ಆದರೆ(cid >= 0)
      ಹಿಂತಿರುಗಿ curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSDC, ದತ್ತಾಂಶ->id, cid);
    ಇಲ್ಲದಿದ್ದರೆ
      ಹಿಂತಿರುಗಿ curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSD, ದತ್ತಾಂಶ->id);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cid >= 0)
    ಹಿಂತಿರುಗಿ curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSC, cid);
  ಇಲ್ಲದಿದ್ದರೆ {
    ಹಿಂತಿರುಗಿ curl_msnprintf(buf, maxlen, "[x-x] ");
  }
}

ಸ್ಥಿರ size_t trc_ಕೊನೆ_buf(ಅಕ್ಷರ *buf, size_t len, size_t maxlen, bool addnl)
{
  /* make sure we end the trace line in `buf` properly. It needs
   * to end with a null-terminator '\0' or '\n\0' */
  ಆದರೆ(len >= (maxlen - (addnl ? 2 : 1))) {
    len = maxlen - 5;
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '\n';
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(addnl)
    buf[len++] = '\n';
  buf[len] = '\0';
  ಹಿಂತಿರುಗಿ len;
}

ಶೂನ್ಯ Curl_debug(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, curl_infotype type,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr, size_t ಗಾತ್ರ_2)
{
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.verbose) {
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ s_infotype[CURLINFO_ಕೊನೆ][3] = {
      "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
    ಅಕ್ಷರ buf[TRC_ಸಾಲು_MAX];
    size_t len;
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.fdebug) {
      ರಚನೆ Curl_mapi_guard guard;
      ಆದರೆ(CURL_TRC_IDS(ದತ್ತಾಂಶ) && (ಗಾತ್ರ_2 < TRC_ಸಾಲು_MAX)) {
        len = trc_print_ids(ದತ್ತಾಂಶ, buf, TRC_ಸಾಲು_MAX);
        len += curl_msnprintf(buf + len, TRC_ಸಾಲು_MAX - len, "%.*s",
                              (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ_2, ptr);
        len = trc_ಕೊನೆ_buf(buf, len, TRC_ಸಾಲು_MAX, FALSE);
        CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_fdebug);
        (ಶೂನ್ಯ)(*ದತ್ತಾಂಶ->ಗಣ.fdebug)(ದತ್ತಾಂಶ, type, buf, len, ದತ್ತಾಂಶ->ಗಣ.debugdata);
        CURL_CBAPI_ಕೊನೆ(&guard);
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_fdebug);
        (ಶೂನ್ಯ)(*ದತ್ತಾಂಶ->ಗಣ.fdebug)(ದತ್ತಾಂಶ, type, CURL_UNCONST(ptr),
                                  ಗಾತ್ರ_2, ದತ್ತಾಂಶ->ಗಣ.debugdata);
        CURL_CBAPI_ಕೊನೆ(&guard);
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಆಯ್ಕೆ(type) {
      ಸ್ಥಿತಿ CURLINFO_ಪಠ್ಯ:
      ಸ್ಥಿತಿ CURLINFO_ಶೀರ್ಷಿಕೆ_ಹೊರಹೋಗು:
      ಸ್ಥಿತಿ CURLINFO_ಶೀರ್ಷಿಕೆ_ಒಳ:
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_VERBOSE_ಅಕ್ಷರಸರಣಿಗಳು
        ಆದರೆ(CURL_TRC_IDS(ದತ್ತಾಂಶ)) {
          len = trc_print_ids(ದತ್ತಾಂಶ, buf, TRC_ಸಾಲು_MAX);
          fwrite(buf, len, 1, ದತ್ತಾಂಶ->ಗಣ.err);
        }
#ಆದರೆ_ಅಂತ್ಯ
        fwrite(s_infotype[type], 2, 1, ದತ್ತಾಂಶ->ಗಣ.err);
        fwrite(ptr, ಗಾತ್ರ_2, 1, ದತ್ತಾಂಶ->ಗಣ.err);
        ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ: /* nada */
        ನಿಲ್ಲಿಸು;
      }
    }
  }
}

/* Curl_failf() is for messages stating why we failed.
 * The message SHALL NOT include any LF or CR.
 */
ಶೂನ್ಯ Curl_failf(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.verbose || ದತ್ತಾಂಶ->ಗಣ.errorbuffer) {
    va_list ap;
    size_t len;
    ಅಕ್ಷರ error[CURL_ERROR_ಗಾತ್ರ + 2];
    va_start(ap, fmt);
    len = curl_mvsnprintf(error, CURL_ERROR_ಗಾತ್ರ, fmt, ap);

    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.errorbuffer && !ದತ್ತಾಂಶ->state.errorbuf) {
      curlx_strcopy(ದತ್ತಾಂಶ->ಗಣ.errorbuffer, CURL_ERROR_ಗಾತ್ರ, error, len);
      ದತ್ತಾಂಶ->state.errorbuf = TRUE; /* wrote error string */
    }
    error[len++] = '\n';
    error[len] = '\0';
    trc_write(ದತ್ತಾಂಶ, CURLINFO_ಪಠ್ಯ, error, len);
    va_end(ap);
  }
}

ಶೂನ್ಯ Curl_reset_fail(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.errorbuffer)
    ದತ್ತಾಂಶ->ಗಣ.errorbuffer[0] = 0;
  ದತ್ತಾಂಶ->state.errorbuf = FALSE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ರಚನೆ curl_trc_feat Curl_trc_feat_multi = {
  "MULTI",
  CURL_LOG_LVL_NONE,
};
ರಚನೆ curl_trc_feat Curl_trc_feat_read = {
  "READ",
  CURL_LOG_LVL_NONE,
};
ರಚನೆ curl_trc_feat Curl_trc_feat_write = {
  "WRITE",
  CURL_LOG_LVL_NONE,
};
ರಚನೆ curl_trc_feat Curl_trc_feat_dns = {
  "DNS",
  CURL_LOG_LVL_NONE,
};
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DOH
ರಚನೆ curl_trc_feat Curl_trc_feat_doh = {
  "DoH",
  CURL_LOG_LVL_NONE,
};
#ಆದರೆ_ಅಂತ್ಯ
ರಚನೆ curl_trc_feat Curl_trc_feat_timer = {
  "TIMER",
  CURL_LOG_LVL_NONE,
};
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ಎಳೆಗಳು
ರಚನೆ curl_trc_feat Curl_trc_feat_ಎಳೆಗಳು = {
  "THREADS",
  CURL_LOG_LVL_NONE,
};
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_VERBOSE_ಅಕ್ಷರಸರಣಿಗಳು

ಸ್ಥಿರ ಶೂನ್ಯ trc_infof(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                      ರಚನೆ curl_trc_feat *feat,
                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *opt_id, ಪೂರ್ಣಾಂಕ opt_id_idx,
                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ fmt, va_list ap) CURL_PRINTF(5, 0);

ಸ್ಥಿರ ಶೂನ್ಯ trc_infof(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                      ರಚನೆ curl_trc_feat *feat,
                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *opt_id, ಪೂರ್ಣಾಂಕ opt_id_idx,
                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ fmt, va_list ap)
{
  size_t len = 0;
  ಅಕ್ಷರ buf[TRC_ಸಾಲು_MAX];

  ಆದರೆ(CURL_TRC_IDS(ದತ್ತಾಂಶ))
    len += trc_print_ids(ದತ್ತಾಂಶ, buf + len, TRC_ಸಾಲು_MAX - len);
  ಆದರೆ(feat)
    len += curl_msnprintf(buf + len, TRC_ಸಾಲು_MAX - len, "[%s] ", feat->name);
  ಆದರೆ(opt_id) {
    ಆದರೆ(opt_id_idx > 0)
      len += curl_msnprintf(buf + len, TRC_ಸಾಲು_MAX - len, "[%s-%d] ",
                            opt_id, opt_id_idx);
    ಇಲ್ಲದಿದ್ದರೆ
      len += curl_msnprintf(buf + len, TRC_ಸಾಲು_MAX - len, "[%s] ", opt_id);
  }
  len += curl_mvsnprintf(buf + len, TRC_ಸಾಲು_MAX - len, fmt, ap);
  len = trc_ಕೊನೆ_buf(buf, len, TRC_ಸಾಲು_MAX, TRUE);
  trc_write(ದತ್ತಾಂಶ, CURLINFO_ಪಠ್ಯ, buf, len);
}

ಶೂನ್ಯ Curl_infof(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ಆದರೆ(Curl_trc_is_verbose(ದತ್ತಾಂಶ)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->state.feat, ಖಾಲಿ, 0, fmt, ap);
    va_end(ap);
  }
}

ಶೂನ್ಯ Curl_trc_cf_infof(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cfilter *cf,
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  DEBUGASSERT(cf);
  ಆದರೆ(Curl_trc_cf_is_verbose(cf, ದತ್ತಾಂಶ)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->state.feat, cf->cft->name, cf->sockindex, fmt, ap);
    va_end(ap);
  }
}

ಶೂನ್ಯ Curl_trc_feat_infof(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         ರಚನೆ curl_trc_feat *feat,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  DEBUGASSERT(feat);
  ಆದರೆ(Curl_trc_ft_is_verbose(ದತ್ತಾಂಶ, feat)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ದತ್ತಾಂಶ, feat, ಖಾಲಿ, 0, fmt, ap);
    va_end(ap);
  }
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ Curl_trc_timer_ಹೆಸರುಗಳು[] = {
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

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *trc_timer_name(ಪೂರ್ಣಾಂಕ tid)
{
  ಆದರೆ((tid >= 0) && ((size_t)tid < CURL_ARRAYSIZE(Curl_trc_timer_ಹೆಸರುಗಳು)))
    ಹಿಂತಿರುಗಿ Curl_trc_timer_ಹೆಸರುಗಳು[(size_t)tid];
  ಹಿಂತಿರುಗಿ "TIMER-???";
}

ಶೂನ್ಯ Curl_trc_timer(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ tid, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ಆದರೆ(Curl_trc_ft_is_verbose(ದತ್ತಾಂಶ, &Curl_trc_feat_timer)) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tname = trc_timer_name(tid);
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ದತ್ತಾಂಶ, &Curl_trc_feat_timer, tname, 0, fmt, ap);
    va_end(ap);
  }
}

ಶೂನ್ಯ Curl_trc_easy_timers(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(CURL_TRC_TIMER_is_verbose(ದತ್ತಾಂಶ) && ದತ್ತಾಂಶ->multi) {
    ಆದರೆ(ದತ್ತಾಂಶ->state.timeouts.ಮೊದಲ < EXPIRE_ಕೊನೆಯ) {
      ರಚನೆ expire_timers *timeouts = &ದತ್ತಾಂಶ->state.timeouts;
      timediff_t ತಳ_us =
        Curl_timeouts_ಸ್ಥಾನಾಂತರ_us(&ದತ್ತಾಂಶ->multi->timeouts, Curl_pgrs_now(ದತ್ತಾಂಶ));
      uint8_t id = ದತ್ತಾಂಶ->state.timeouts.ಮೊದಲ;
      ಗಾಗಿ(; id < EXPIRE_ಕೊನೆಯ; id = timeouts->ಮುಂದಿನ[id]) {
        CURL_TRC_TIMER(ದತ್ತಾಂಶ, id, "expires in %" FMT_TIMEDIFF_T "us",
                       timeouts->ಸ್ಥಾನಾಂತರ_us[id] - ತಳ_us);
      }
    }
  }
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ Curl_trc_mstate_ಹೆಸರುಗಳು[] = {
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

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Curl_trc_mstate_name(ಪೂರ್ಣಾಂಕ state)
{
  ಆದರೆ((state >= 0) && ((size_t)state < CURL_ARRAYSIZE(Curl_trc_mstate_ಹೆಸರುಗಳು)))
    ಹಿಂತಿರುಗಿ Curl_trc_mstate_ಹೆಸರುಗಳು[(size_t)state];
  ಹಿಂತಿರುಗಿ "?";
}

ಶೂನ್ಯ Curl_trc_multi(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ಆದರೆ(Curl_trc_ft_is_verbose(ದತ್ತಾಂಶ, &Curl_trc_feat_multi)) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sname = (ದತ್ತಾಂಶ->id >= 0) ?
                        Curl_trc_mstate_name(ದತ್ತಾಂಶ->mstate) : ಖಾಲಿ;
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ದತ್ತಾಂಶ, &Curl_trc_feat_multi, sname, 0, fmt, ap);
    va_end(ap);
  }
}

ಶೂನ್ಯ Curl_trc_read(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ಆದರೆ(Curl_trc_ft_is_verbose(ದತ್ತಾಂಶ, &Curl_trc_feat_read)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ದತ್ತಾಂಶ, &Curl_trc_feat_read, ಖಾಲಿ, 0, fmt, ap);
    va_end(ap);
  }
}

ಶೂನ್ಯ Curl_trc_write(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ಆದರೆ(Curl_trc_ft_is_verbose(ದತ್ತಾಂಶ, &Curl_trc_feat_write)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ದತ್ತಾಂಶ, &Curl_trc_feat_write, ಖಾಲಿ, 0, fmt, ap);
    va_end(ap);
  }
}

ಶೂನ್ಯ Curl_trc_dns(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ಆದರೆ(Curl_trc_ft_is_verbose(ದತ್ತಾಂಶ, &Curl_trc_feat_dns)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ದತ್ತಾಂಶ, &Curl_trc_feat_dns, ಖಾಲಿ, 0, fmt, ap);
    va_end(ap);
  }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
ರಚನೆ curl_trc_feat Curl_trc_feat_ftp = {
  "FTP",
  CURL_LOG_LVL_NONE,
};

ಶೂನ್ಯ Curl_trc_ftp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ಆದರೆ(Curl_trc_ft_is_verbose(ದತ್ತಾಂಶ, &Curl_trc_feat_ftp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ದತ್ತಾಂಶ, &Curl_trc_feat_ftp, ಖಾಲಿ, 0, fmt, ap);
    va_end(ap);
  }
}
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_FTP */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_SMTP
ರಚನೆ curl_trc_feat Curl_trc_feat_smtp = {
  "SMTP",
  CURL_LOG_LVL_NONE,
};

ಶೂನ್ಯ Curl_trc_smtp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ಆದರೆ(Curl_trc_ft_is_verbose(ದತ್ತಾಂಶ, &Curl_trc_feat_smtp)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ದತ್ತಾಂಶ, &Curl_trc_feat_smtp, ಖಾಲಿ, 0, fmt, ap);
    va_end(ap);
  }
}
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_SMTP */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
ರಚನೆ curl_trc_feat Curl_trc_feat_ssls = {
  "SSLS",
  CURL_LOG_LVL_NONE,
};

ಶೂನ್ಯ Curl_trc_ssls(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ಆದರೆ(Curl_trc_ft_is_verbose(ದತ್ತಾಂಶ, &Curl_trc_feat_ssls)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ದತ್ತಾಂಶ, &Curl_trc_feat_ssls, ಖಾಲಿ, 0, fmt, ap);
    va_end(ap);
  }
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_SSL */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSH
ರಚನೆ curl_trc_feat Curl_trc_feat_ssh = {
  "SSH",
  CURL_LOG_LVL_NONE,
};

ಶೂನ್ಯ Curl_trc_ssh(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ಆದರೆ(Curl_trc_ft_is_verbose(ದತ್ತಾಂಶ, &Curl_trc_feat_ssh)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ದತ್ತಾಂಶ, &Curl_trc_feat_ssh, ಖಾಲಿ, 0, fmt, ap);
    va_end(ap);
  }
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_SSH */

#ಆದರೆ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
ರಚನೆ curl_trc_feat Curl_trc_feat_ws = {
  "WS",
  CURL_LOG_LVL_NONE,
};

ಶೂನ್ಯ Curl_trc_ws(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  DEBUGASSERT(!strchr(fmt, '\n'));
  ಆದರೆ(Curl_trc_ft_is_verbose(ದತ್ತಾಂಶ, &Curl_trc_feat_ws)) {
    va_list ap;
    va_start(ap, fmt);
    trc_infof(ದತ್ತಾಂಶ, &Curl_trc_feat_ws, ಖಾಲಿ, 0, fmt, ap);
    va_end(ap);
  }
}
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#ವ್ಯಾಖ್ಯಾನಿಸು TRC_CT_NONE        0
#ವ್ಯಾಖ್ಯಾನಿಸು TRC_CT_PROTOCOL    (1 << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು TRC_CT_ಜಾಲ     (1 << 1)
#ವ್ಯಾಖ್ಯಾನಿಸು TRC_CT_PROXY       (1 << 2)
#ವ್ಯಾಖ್ಯಾನಿಸು TRC_CT_INTERNALS   (1 << 3)

ರಚನೆ trc_feat_def {
  ರಚನೆ curl_trc_feat *feat;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ category;
};

ಸ್ಥಿರ ರಚನೆ trc_feat_def trc_feats[] = {
  { &Curl_trc_feat_ids,       TRC_CT_INTERNALS },
  { &Curl_trc_feat_multi,     TRC_CT_ಜಾಲ },
  { &Curl_trc_feat_read,      TRC_CT_NONE },
  { &Curl_trc_feat_write,     TRC_CT_NONE },
  { &Curl_trc_feat_dns,       TRC_CT_ಜಾಲ },
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DOH
  { &Curl_trc_feat_doh,       TRC_CT_ಜಾಲ },
#ಆದರೆ_ಅಂತ್ಯ
  { &Curl_trc_feat_timer,     TRC_CT_ಜಾಲ },
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ಎಳೆಗಳು
  { &Curl_trc_feat_ಎಳೆಗಳು,   TRC_CT_NONE },
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
  { &Curl_trc_feat_ftp,       TRC_CT_PROTOCOL },
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_SMTP
  { &Curl_trc_feat_smtp,      TRC_CT_PROTOCOL },
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
  { &Curl_trc_feat_ssls,      TRC_CT_ಜಾಲ },
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSH
  { &Curl_trc_feat_ssh,       TRC_CT_PROTOCOL },
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
  { &Curl_trc_feat_ws,        TRC_CT_PROTOCOL },
#ಆದರೆ_ಅಂತ್ಯ
};

ರಚನೆ trc_cft_def {
  ರಚನೆ Curl_cftype *cft;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ category;
};

ಸ್ಥಿರ ರಚನೆ trc_cft_def trc_cfts[] = {
  { &Curl_cft_dns,            TRC_CT_ಜಾಲ },
  { &Curl_cft_tcp,            TRC_CT_ಜಾಲ },
  { &Curl_cft_udp,            TRC_CT_ಜಾಲ },
  { &Curl_cft_unix,           TRC_CT_ಜಾಲ },
  { &Curl_cft_tcp_accept,     TRC_CT_ಜಾಲ },
  { &Curl_cft_ip_happy,       TRC_CT_ಜಾಲ },
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_WEBSOCKETS
  { &Curl_cft_recvbuf,        TRC_CT_PROTOCOL },
#ಆದರೆ_ಅಂತ್ಯ
  { &Curl_cft_ಸಿದ್ಧತೆ,          TRC_CT_PROTOCOL },
#ಆದರೆ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  { &Curl_cft_nghttp2,        TRC_CT_PROTOCOL },
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
  { &Curl_cft_ssl,            TRC_CT_ಜಾಲ },
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  { &Curl_cft_ssl_proxy,      TRC_CT_PROXY },
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
  { &Curl_cft_h1_proxy,       TRC_CT_PROXY },
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NGHTTP2
  { &Curl_cft_h2_proxy,       TRC_CT_PROXY },
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3)
  { &Curl_cft_h3_proxy,       TRC_CT_PROXY },
#ಆದರೆ_ಅಂತ್ಯ
  { &Curl_cft_http_proxy,     TRC_CT_PROXY },
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP */
  { &Curl_cft_haproxy,        TRC_CT_PROXY },
  { &Curl_cft_socks_proxy,    TRC_CT_PROXY },
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_PROXY */
#ಆದರೆ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { &Curl_cft_http3,          TRC_CT_PROTOCOL },
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
  { &Curl_cft_http_connect,   TRC_CT_PROTOCOL },
#ಆದರೆ_ಅಂತ್ಯ
};

ಸ್ಥಿರ ಶೂನ್ಯ trc_apply_level_by_name(ರಚನೆ Curl_str *token, ಪೂರ್ಣಾಂಕ lvl)
{
  size_t i;

  ಗಾಗಿ(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    ಆದರೆ(curlx_str_casecompare(token, trc_cfts[i].cft->name)) {
      trc_cfts[i].cft->log_level = lvl;
      ನಿಲ್ಲಿಸು;
    }
  }
  ಗಾಗಿ(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    ಆದರೆ(curlx_str_casecompare(token, trc_feats[i].feat->name)) {
      trc_feats[i].feat->log_level = lvl;
      ನಿಲ್ಲಿಸು;
    }
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ trc_apply_level_by_category(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ category, ಪೂರ್ಣಾಂಕ lvl)
{
  size_t i;

  ಗಾಗಿ(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    ಆದರೆ(!category || (trc_cfts[i].category & category))
      trc_cfts[i].cft->log_level = lvl;
  }
  ಗಾಗಿ(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    ಆದರೆ(!category || (trc_feats[i].category & category))
      trc_feats[i].feat->log_level = lvl;
  }
}

ಸ್ಥಿರ CURLcode trc_opt(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *config)
{
  ರಚನೆ Curl_str ಹೊರಹೋಗು;
  ಆಗ(!curlx_str_until(&config, &ಹೊರಹೋಗು, 32, ',')) {
    ಪೂರ್ಣಾಂಕ lvl = CURL_LOG_LVL_INFO;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *token = curlx_str(&ಹೊರಹೋಗು);

    ಆದರೆ(*token == '-') {
      lvl = CURL_LOG_LVL_NONE;
      curlx_str_nudge(&ಹೊರಹೋಗು, 1);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*token == '+')
      curlx_str_nudge(&ಹೊರಹೋಗು, 1);

    ಆದರೆ(curlx_str_casecompare(&ಹೊರಹೋಗು, "all"))
      trc_apply_level_by_category(TRC_CT_NONE, lvl);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curlx_str_casecompare(&ಹೊರಹೋಗು, "protocol"))
      trc_apply_level_by_category(TRC_CT_PROTOCOL, lvl);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curlx_str_casecompare(&ಹೊರಹೋಗು, "network"))
      trc_apply_level_by_category(TRC_CT_ಜಾಲ, lvl);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curlx_str_casecompare(&ಹೊರಹೋಗು, "proxy"))
      trc_apply_level_by_category(TRC_CT_PROXY, lvl);
    ಇಲ್ಲದಿದ್ದರೆ
      trc_apply_level_by_name(&ಹೊರಹೋಗು, lvl);

    ಆದರೆ(curlx_str_single(&config, ','))
      ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_trc_opt(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *config)
{
  CURLcode ಫಲಿತಾಂಶ = config ? trc_opt(config) : CURLE_OK;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  /* CURL_DEBUG can override anything */
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dbg_config = getenv("CURL_DEBUG");
    ಆದರೆ(dbg_config)
      ಫಲಿತಾಂಶ = trc_opt(dbg_config);
  }
#ಆದರೆ_ಅಂತ್ಯ /* DEBUGBUILD */
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_trc_init(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಹಿಂತಿರುಗಿ Curl_trc_opt(ಖಾಲಿ);
#ಇಲ್ಲದಿದ್ದರೆ
  ಹಿಂತಿರುಗಿ CURLE_OK;
#ಆದರೆ_ಅಂತ್ಯ
}

#ಇಲ್ಲದಿದ್ದರೆ /* CURL_DISABLE_VERBOSE_STRINGS */

CURLcode Curl_trc_init(ಶೂನ್ಯ)
{
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಶೂನ್ಯ Curl_infof(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)fmt;
}

ಶೂನ್ಯ Curl_trc_cf_infof(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cfilter *cf,
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)fmt;
}

ಶೂನ್ಯ Curl_trc_multi(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)fmt;
}

ಶೂನ್ಯ Curl_trc_write(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)fmt;
}

ಶೂನ್ಯ Curl_trc_dns(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)fmt;
}

ಶೂನ್ಯ Curl_trc_timer(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ tid, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)tid;
  (ಶೂನ್ಯ)fmt;
}

ಶೂನ್ಯ Curl_trc_read(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)fmt;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
ಶೂನ್ಯ Curl_trc_ftp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)fmt;
}
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_SMTP
ಶೂನ್ಯ Curl_trc_smtp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)fmt;
}
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
ಶೂನ್ಯ Curl_trc_ws(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)fmt;
}
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSH
ಶೂನ್ಯ Curl_trc_ssh(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)fmt;
}
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
ಶೂನ್ಯ Curl_trc_ssls(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)fmt;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_VERBOSE_STRINGS */
