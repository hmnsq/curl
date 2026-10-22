#جے_بنا_تعریف سرخی_CURL_TRC_H
#تعریف_کرو سرخی_CURL_TRC_H
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
بنية Curl_easy;
بنية Curl_cfilter;

/**
 * Init logging, return != 0 on failure.
 */
CURLcode Curl_trc_init(فراغ);

/**
 * Configure tracing. May be called several times during global
 * initialization. Later calls may not take effect.
 *
 * Configuration format supported:
 * - comma-separated list of component names to enable logging on.
 *   E.g. 'http/2,ssl'. Unknown names are ignored. Names are compared
 *   case-insensitive.
 * - component 'all' applies to all known log components
 * - prefixing a component with '+' or '-' will en-/disable logging for
 *   that component
 * Example: 'all,-ssl' would enable logging for all components but the
 * SSL filters.
 *
 * @param config configuration string
 */
CURLcode Curl_trc_opt(ثابت حرف *config);

/* the function used to output verbose information */
فراغ Curl_debug(بنية Curl_easy *انکڑے, curl_infotype type,
                ثابت حرف *ptr, size_t ناپ);

/**
 * Output a failure message on registered callbacks for transfer.
 */
فراغ Curl_failf(بنية Curl_easy *انکڑے,
                ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);

#تعریف_کرو failf Curl_failf

/* In case failf() reported into the errorbuf, clear it again.
 * This is used to clear information from happy eyeballing attempts
 * when ultimately a successful attempt was made. */
فراغ Curl_reset_fail(بنية Curl_easy *انکڑے);

#تعریف_کرو CURL_LOG_LVL_NONE  0
#تعریف_کرو CURL_LOG_LVL_INFO  1

/**
 * Output an informational message when transfer's verbose logging is enabled.
 */
فراغ Curl_infof(بنية Curl_easy *انکڑے,
                ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);

/**
 * Output an informational message when both transfer's verbose logging
 * and connection filters verbose logging are enabled.
 */
فراغ Curl_trc_cf_infof(بنية Curl_easy *انکڑے, ثابت بنية Curl_cfilter *cf,
                       ثابت حرف *fmt, ...) CURL_PRINTF(3, 4);
فراغ Curl_trc_multi(بنية Curl_easy *انکڑے,
                    ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
ثابت حرف *Curl_trc_mstate_name(صحيح state);
ثابت حرف *Curl_trc_timer_name(صحيح tid);
فراغ Curl_trc_easy_timers(بنية Curl_easy *انکڑے);

فراغ Curl_trc_write(بنية Curl_easy *انکڑے,
                    ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
فراغ Curl_trc_read(بنية Curl_easy *انکڑے,
                   ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
فراغ Curl_trc_dns(بنية Curl_easy *انکڑے,
                  ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
فراغ Curl_trc_timer(بنية Curl_easy *انکڑے, صحيح tid,
                    ثابت حرف *fmt, ...) CURL_PRINTF(3, 4);

بنية curl_trc_feat {
  ثابت حرف *name;
  صحيح log_level;
};

فراغ Curl_trc_feat_infof(بنية Curl_easy *انکڑے,
                         بنية curl_trc_feat *feat,
                         ثابت حرف *fmt, ...) CURL_PRINTF(3, 4);

#جے_بنا_تعریف CURL_DISABLE_FTP
خارجي بنية curl_trc_feat Curl_trc_feat_ftp;
فراغ Curl_trc_ftp(بنية Curl_easy *انکڑے,
                  ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_SMTP
خارجي بنية curl_trc_feat Curl_trc_feat_smtp;
فراغ Curl_trc_smtp(بنية Curl_easy *انکڑے,
                   ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
#جے_انت
#جے_تعریف_شدہ USE_SSL
خارجي بنية curl_trc_feat Curl_trc_feat_ssls;
فراغ Curl_trc_ssls(بنية Curl_easy *انکڑے,
                   ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
#جے_انت
#جے_تعریف_شدہ USE_SSH
خارجي بنية curl_trc_feat Curl_trc_feat_ssh;
فراغ Curl_trc_ssh(بنية Curl_easy *انکڑے,
                  ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
#جے_انت
#إذا !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
خارجي بنية curl_trc_feat Curl_trc_feat_ws;
فراغ Curl_trc_ws(بنية Curl_easy *انکڑے,
                 ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
#جے_انت

#تعریف_کرو CURL_TRC_M_is_verbose(انکڑے) \
  Curl_trc_ft_is_verbose(انکڑے, &Curl_trc_feat_multi)
#تعریف_کرو CURL_TRC_DNS_is_verbose(انکڑے) \
  Curl_trc_ft_is_verbose(انکڑے, &Curl_trc_feat_dns)
#تعریف_کرو CURL_TRC_TIMER_is_verbose(انکڑے) \
  Curl_trc_ft_is_verbose(انکڑے, &Curl_trc_feat_timer)

#إذا defined(CURL_HAVE_MACRO_VARARG) && !defined(CURL_DISABLE_VERBOSE_لڑیاں)
#تعریف_کرو infof(انکڑے, ...)             \
  افعل {                               \
    إذا(Curl_trc_is_verbose(انکڑے))    \
      Curl_infof(انکڑے, __VA_ARGS__); \
  } طالما(0)
#تعریف_کرو CURL_TRC_M(انکڑے, ...)            \
  افعل {                                   \
    إذا(CURL_TRC_M_is_verbose(انکڑے))      \
      Curl_trc_multi(انکڑے, __VA_ARGS__); \
  } طالما(0)
#تعریف_کرو CURL_TRC_CF(انکڑے, cf, ...)              \
  افعل {                                          \
    إذا(Curl_trc_cf_is_verbose(cf, انکڑے))        \
      Curl_trc_cf_infof(انکڑے, cf, __VA_ARGS__); \
  } طالما(0)
#تعریف_کرو CURL_TRC_WRITE(انکڑے, ...)                          \
  افعل {                                                     \
    إذا(Curl_trc_ft_is_verbose(انکڑے, &Curl_trc_feat_write)) \
      Curl_trc_write(انکڑے, __VA_ARGS__);                   \
  } طالما(0)
#تعریف_کرو CURL_TRC_READ(انکڑے, ...)                          \
  افعل {                                                    \
    إذا(Curl_trc_ft_is_verbose(انکڑے, &Curl_trc_feat_read)) \
      Curl_trc_read(انکڑے, __VA_ARGS__);                   \
  } طالما(0)
#تعریف_کرو CURL_TRC_DNS(انکڑے, ...)        \
  افعل {                                 \
    إذا(CURL_TRC_DNS_is_verbose(انکڑے))  \
      Curl_trc_dns(انکڑے, __VA_ARGS__); \
  } طالما(0)
#تعریف_کرو CURL_TRC_TIMER(انکڑے, tid, ...)        \
  افعل {                                        \
    إذا(CURL_TRC_TIMER_is_verbose(انکڑے))       \
      Curl_trc_timer(انکڑے, tid, __VA_ARGS__); \
  } طالما(0)

#جے_بنا_تعریف CURL_DISABLE_FTP
#تعریف_کرو CURL_TRC_FTP(انکڑے, ...)                          \
  افعل {                                                   \
    إذا(Curl_trc_ft_is_verbose(انکڑے, &Curl_trc_feat_ftp)) \
      Curl_trc_ftp(انکڑے, __VA_ARGS__);                   \
  } طالما(0)
#جے_انت /* !CURL_DISABLE_FTP */
#جے_بنا_تعریف CURL_DISABLE_SMTP
#تعریف_کرو CURL_TRC_SMTP(انکڑے, ...)                          \
  افعل {                                                    \
    إذا(Curl_trc_ft_is_verbose(انکڑے, &Curl_trc_feat_smtp)) \
      Curl_trc_smtp(انکڑے, __VA_ARGS__);                   \
  } طالما(0)
#جے_انت /* !CURL_DISABLE_SMTP */
#جے_تعریف_شدہ USE_SSL
#تعریف_کرو CURL_TRC_SSLS(انکڑے, ...)                          \
  افعل {                                                    \
    إذا(Curl_trc_ft_is_verbose(انکڑے, &Curl_trc_feat_ssls)) \
      Curl_trc_ssls(انکڑے, __VA_ARGS__);                   \
  } طالما(0)
#جے_انت /* USE_SSL */
#جے_تعریف_شدہ USE_SSH
#تعریف_کرو CURL_TRC_SSH(انکڑے, ...)                          \
  افعل {                                                   \
    إذا(Curl_trc_ft_is_verbose(انکڑے, &Curl_trc_feat_ssh)) \
      Curl_trc_ssh(انکڑے, __VA_ARGS__);                   \
  } طالما(0)
#جے_انت /* USE_SSH */
#إذا !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#تعریف_کرو CURL_TRC_WS(انکڑے, ...)                          \
  افعل {                                                  \
    إذا(Curl_trc_ft_is_verbose(انکڑے, &Curl_trc_feat_ws)) \
      Curl_trc_ws(انکڑے, __VA_ARGS__);                   \
  } طالما(0)
#جے_انت /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#وإلاإذا defined(CURL_HAVE_MACRO_VARARG) && defined(CURL_DISABLE_VERBOSE_لڑیاں)

#تعریف_کرو infof(انکڑے, ...) \
  افعل {                   \
    (فراغ)(انکڑے);        \
  } طالما(0)
#تعریف_کرو CURL_TRC_M(انکڑے, ...) \
  افعل {                        \
    (فراغ)(انکڑے);             \
  } طالما(0)
#تعریف_کرو CURL_TRC_CF(انکڑے, cf, ...) \
  افعل {                             \
    (فراغ)(انکڑے);                  \
    (فراغ)(cf);                    \
  } طالما(0)
#تعریف_کرو CURL_TRC_WRITE(انکڑے, ...) \
  افعل {                            \
    (فراغ)(انکڑے);                 \
  } طالما(0)
#تعریف_کرو CURL_TRC_READ(انکڑے, ...) \
  افعل {                           \
    (فراغ)(انکڑے);                \
  } طالما(0)
#تعریف_کرو CURL_TRC_DNS(انکڑے, ...) \
  افعل {                          \
    (فراغ)(انکڑے);               \
  } طالما(0)
#تعریف_کرو CURL_TRC_TIMER(انکڑے, tid, ...) \
  افعل {                                 \
    (فراغ)(انکڑے);                      \
    (فراغ)(tid);                       \
  } طالما(0)
#جے_بنا_تعریف CURL_DISABLE_FTP
#تعریف_کرو CURL_TRC_FTP(انکڑے, ...) \
  افعل {                          \
    (فراغ)(انکڑے);               \
  } طالما(0)
#جے_انت /* !CURL_DISABLE_FTP */
#جے_بنا_تعریف CURL_DISABLE_SMTP
#تعریف_کرو CURL_TRC_SMTP(انکڑے, ...) \
  افعل {                           \
    (فراغ)(انکڑے);                \
  } طالما(0)
#جے_انت /* !CURL_DISABLE_SMTP */
#جے_تعریف_شدہ USE_SSL
#تعریف_کرو CURL_TRC_SSLS(انکڑے, ...) \
  افعل {                           \
    (فراغ)(انکڑے);                \
  } طالما(0)
#جے_انت /* USE_SSL */
#جے_تعریف_شدہ USE_SSH
#تعریف_کرو CURL_TRC_SSH(انکڑے, ...) \
  افعل {                          \
    (فراغ)(انکڑے);               \
  } طالما(0)
#جے_انت /* USE_SSH */
#إذا !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#تعریف_کرو CURL_TRC_WS(انکڑے, ...) \
  افعل {                         \
    (فراغ)(انکڑے);              \
  } طالما(0)
#جے_انت

#وإلا /* !CURL_HAVE_MACRO_VARARG */

#تعریف_کرو infof          Curl_infof
#تعریف_کرو CURL_TRC_M     Curl_trc_multi
#تعریف_کرو CURL_TRC_CF    Curl_trc_cf_infof
#تعریف_کرو CURL_TRC_WRITE Curl_trc_write
#تعریف_کرو CURL_TRC_READ  Curl_trc_read
#تعریف_کرو CURL_TRC_DNS   Curl_trc_dns
#تعریف_کرو CURL_TRC_TIMER Curl_trc_timer

#جے_بنا_تعریف CURL_DISABLE_FTP
#تعریف_کرو CURL_TRC_FTP   Curl_trc_ftp
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_SMTP
#تعریف_کرو CURL_TRC_SMTP  Curl_trc_smtp
#جے_انت
#جے_تعریف_شدہ USE_SSL
#تعریف_کرو CURL_TRC_SSLS  Curl_trc_ssls
#جے_انت
#جے_تعریف_شدہ USE_SSH
#تعریف_کرو CURL_TRC_SSH   Curl_trc_ssh
#جے_انت
#إذا !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#تعریف_کرو CURL_TRC_WS    Curl_trc_ws
#جے_انت

#جے_انت /* CURL_HAVE_MACRO_VARARG */

#جے_تعریف_شدہ CURLVERBOSE
خارجي بنية curl_trc_feat Curl_trc_feat_multi;
خارجي بنية curl_trc_feat Curl_trc_feat_read;
خارجي بنية curl_trc_feat Curl_trc_feat_write;
خارجي بنية curl_trc_feat Curl_trc_feat_dns;
#جے_بنا_تعریف CURL_DISABLE_DOH
خارجي بنية curl_trc_feat Curl_trc_feat_doh;
#جے_انت
خارجي بنية curl_trc_feat Curl_trc_feat_timer;
#جے_تعریف_شدہ USE_دھاگے
خارجي بنية curl_trc_feat Curl_trc_feat_دھاگے;
#جے_انت
#جے_انت

#جے_بنا_تعریف CURL_DISABLE_VERBOSE_لڑیاں
/* informational messages enabled */
#تعریف_کرو Curl_trc_is_verbose(انکڑے)                          \
  ((انکڑے) && (انکڑے)->سیٹ.verbose &&                        \
   (!(انکڑے)->state.feat ||                                 \
    ((انکڑے)->state.feat->log_level >= CURL_LOG_LVL_INFO)))
#تعریف_کرو Curl_trc_cf_is_verbose(cf, انکڑے)              \
  (Curl_trc_is_verbose(انکڑے) &&                       \
   (cf) && (cf)->cft->log_level >= CURL_LOG_LVL_INFO)
#تعریف_کرو Curl_trc_ft_is_verbose(انکڑے, ft) \
  (Curl_trc_is_verbose(انکڑے) &&          \
   (ft)->log_level >= CURL_LOG_LVL_INFO)
#تعریف_کرو CURL_MSTATE_NAME(s)  Curl_trc_mstate_name((صحيح)(s))
#تعریف_کرو CURL_TRC_EASY_TIMERS(انکڑے)      \
  افعل {                                  \
    إذا(CURL_TRC_TIMER_is_verbose(انکڑے)) \
      Curl_trc_easy_timers(انکڑے);       \
  } طالما(0)

#وإلا /* CURL_DISABLE_VERBOSE_STRINGS */
/* All informational messages are not compiled in for size savings */
#تعریف_کرو Curl_trc_is_verbose(d)        FALSE
#تعریف_کرو Curl_trc_cf_is_verbose(x, y)  FALSE
#تعریف_کرو Curl_trc_ft_is_verbose(x, y)  FALSE
#تعریف_کرو CURL_MSTATE_NAME(x)           ((فراغ)(x), "-")
#تعریف_کرو CURL_TRC_EASY_TIMERS(x)       Curl_nop_stmt
#جے_انت /* !CURL_DISABLE_VERBOSE_STRINGS */

#جے_انت /* HEADER_CURL_TRC_H */
