#اگر_تعریف_نشده سرآیند_CURL_TRC_H
#تعریف_کن سرآیند_CURL_TRC_H
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
فراغ Curl_debug(بنية Curl_easy *داده, curl_infotype type,
                ثابت حرف *ptr, size_t اندازه);

/**
 * Output a failure message on registered callbacks for transfer.
 */
فراغ Curl_failf(بنية Curl_easy *داده,
                ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);

#تعریف_کن failf Curl_failf

/* In case failf() reported into the errorbuf, clear it again.
 * This is used to clear information from happy eyeballing attempts
 * when ultimately a successful attempt was made. */
فراغ Curl_reset_fail(بنية Curl_easy *داده);

#تعریف_کن CURL_LOG_LVL_NONE  0
#تعریف_کن CURL_LOG_LVL_INFO  1

/**
 * Output an informational message when transfer's verbose logging is enabled.
 */
فراغ Curl_infof(بنية Curl_easy *داده,
                ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);

/**
 * Output an informational message when both transfer's verbose logging
 * and connection filters verbose logging are enabled.
 */
فراغ Curl_trc_cf_infof(بنية Curl_easy *داده, ثابت بنية Curl_cfilter *cf,
                       ثابت حرف *fmt, ...) CURL_PRINTF(3, 4);
فراغ Curl_trc_multi(بنية Curl_easy *داده,
                    ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
ثابت حرف *Curl_trc_mstate_name(صحيح state);
ثابت حرف *Curl_trc_timer_name(صحيح tid);
فراغ Curl_trc_easy_timers(بنية Curl_easy *داده);

فراغ Curl_trc_write(بنية Curl_easy *داده,
                    ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
فراغ Curl_trc_read(بنية Curl_easy *داده,
                   ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
فراغ Curl_trc_dns(بنية Curl_easy *داده,
                  ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
فراغ Curl_trc_timer(بنية Curl_easy *داده, صحيح tid,
                    ثابت حرف *fmt, ...) CURL_PRINTF(3, 4);

بنية curl_trc_feat {
  ثابت حرف *name;
  صحيح log_level;
};

فراغ Curl_trc_feat_infof(بنية Curl_easy *داده,
                         بنية curl_trc_feat *feat,
                         ثابت حرف *fmt, ...) CURL_PRINTF(3, 4);

#اگر_تعریف_نشده CURL_DISABLE_FTP
خارجي بنية curl_trc_feat Curl_trc_feat_ftp;
فراغ Curl_trc_ftp(بنية Curl_easy *داده,
                  ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_SMTP
خارجي بنية curl_trc_feat Curl_trc_feat_smtp;
فراغ Curl_trc_smtp(بنية Curl_easy *داده,
                   ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
#پایان_اگر
#اگر_تعریف_شده USE_SSL
خارجي بنية curl_trc_feat Curl_trc_feat_ssls;
فراغ Curl_trc_ssls(بنية Curl_easy *داده,
                   ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
#پایان_اگر
#اگر_تعریف_شده USE_SSH
خارجي بنية curl_trc_feat Curl_trc_feat_ssh;
فراغ Curl_trc_ssh(بنية Curl_easy *داده,
                  ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
#پایان_اگر
#إذا !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
خارجي بنية curl_trc_feat Curl_trc_feat_ws;
فراغ Curl_trc_ws(بنية Curl_easy *داده,
                 ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
#پایان_اگر

#تعریف_کن CURL_TRC_M_is_verbose(داده) \
  Curl_trc_ft_is_verbose(داده, &Curl_trc_feat_multi)
#تعریف_کن CURL_TRC_DNS_is_verbose(داده) \
  Curl_trc_ft_is_verbose(داده, &Curl_trc_feat_dns)
#تعریف_کن CURL_TRC_TIMER_is_verbose(داده) \
  Curl_trc_ft_is_verbose(داده, &Curl_trc_feat_timer)

#إذا defined(CURL_HAVE_MACRO_VARARG) && !defined(CURL_DISABLE_VERBOSE_رشتهها)
#تعریف_کن infof(داده, ...)             \
  افعل {                               \
    إذا(Curl_trc_is_verbose(داده))    \
      Curl_infof(داده, __VA_ARGS__); \
  } طالما(0)
#تعریف_کن CURL_TRC_M(داده, ...)            \
  افعل {                                   \
    إذا(CURL_TRC_M_is_verbose(داده))      \
      Curl_trc_multi(داده, __VA_ARGS__); \
  } طالما(0)
#تعریف_کن CURL_TRC_CF(داده, cf, ...)              \
  افعل {                                          \
    إذا(Curl_trc_cf_is_verbose(cf, داده))        \
      Curl_trc_cf_infof(داده, cf, __VA_ARGS__); \
  } طالما(0)
#تعریف_کن CURL_TRC_WRITE(داده, ...)                          \
  افعل {                                                     \
    إذا(Curl_trc_ft_is_verbose(داده, &Curl_trc_feat_write)) \
      Curl_trc_write(داده, __VA_ARGS__);                   \
  } طالما(0)
#تعریف_کن CURL_TRC_READ(داده, ...)                          \
  افعل {                                                    \
    إذا(Curl_trc_ft_is_verbose(داده, &Curl_trc_feat_read)) \
      Curl_trc_read(داده, __VA_ARGS__);                   \
  } طالما(0)
#تعریف_کن CURL_TRC_DNS(داده, ...)        \
  افعل {                                 \
    إذا(CURL_TRC_DNS_is_verbose(داده))  \
      Curl_trc_dns(داده, __VA_ARGS__); \
  } طالما(0)
#تعریف_کن CURL_TRC_TIMER(داده, tid, ...)        \
  افعل {                                        \
    إذا(CURL_TRC_TIMER_is_verbose(داده))       \
      Curl_trc_timer(داده, tid, __VA_ARGS__); \
  } طالما(0)

#اگر_تعریف_نشده CURL_DISABLE_FTP
#تعریف_کن CURL_TRC_FTP(داده, ...)                          \
  افعل {                                                   \
    إذا(Curl_trc_ft_is_verbose(داده, &Curl_trc_feat_ftp)) \
      Curl_trc_ftp(داده, __VA_ARGS__);                   \
  } طالما(0)
#پایان_اگر /* !CURL_DISABLE_FTP */
#اگر_تعریف_نشده CURL_DISABLE_SMTP
#تعریف_کن CURL_TRC_SMTP(داده, ...)                          \
  افعل {                                                    \
    إذا(Curl_trc_ft_is_verbose(داده, &Curl_trc_feat_smtp)) \
      Curl_trc_smtp(داده, __VA_ARGS__);                   \
  } طالما(0)
#پایان_اگر /* !CURL_DISABLE_SMTP */
#اگر_تعریف_شده USE_SSL
#تعریف_کن CURL_TRC_SSLS(داده, ...)                          \
  افعل {                                                    \
    إذا(Curl_trc_ft_is_verbose(داده, &Curl_trc_feat_ssls)) \
      Curl_trc_ssls(داده, __VA_ARGS__);                   \
  } طالما(0)
#پایان_اگر /* USE_SSL */
#اگر_تعریف_شده USE_SSH
#تعریف_کن CURL_TRC_SSH(داده, ...)                          \
  افعل {                                                   \
    إذا(Curl_trc_ft_is_verbose(داده, &Curl_trc_feat_ssh)) \
      Curl_trc_ssh(داده, __VA_ARGS__);                   \
  } طالما(0)
#پایان_اگر /* USE_SSH */
#إذا !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#تعریف_کن CURL_TRC_WS(داده, ...)                          \
  افعل {                                                  \
    إذا(Curl_trc_ft_is_verbose(داده, &Curl_trc_feat_ws)) \
      Curl_trc_ws(داده, __VA_ARGS__);                   \
  } طالما(0)
#پایان_اگر /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#وإلاإذا defined(CURL_HAVE_MACRO_VARARG) && defined(CURL_DISABLE_VERBOSE_رشتهها)

#تعریف_کن infof(داده, ...) \
  افعل {                   \
    (فراغ)(داده);        \
  } طالما(0)
#تعریف_کن CURL_TRC_M(داده, ...) \
  افعل {                        \
    (فراغ)(داده);             \
  } طالما(0)
#تعریف_کن CURL_TRC_CF(داده, cf, ...) \
  افعل {                             \
    (فراغ)(داده);                  \
    (فراغ)(cf);                    \
  } طالما(0)
#تعریف_کن CURL_TRC_WRITE(داده, ...) \
  افعل {                            \
    (فراغ)(داده);                 \
  } طالما(0)
#تعریف_کن CURL_TRC_READ(داده, ...) \
  افعل {                           \
    (فراغ)(داده);                \
  } طالما(0)
#تعریف_کن CURL_TRC_DNS(داده, ...) \
  افعل {                          \
    (فراغ)(داده);               \
  } طالما(0)
#تعریف_کن CURL_TRC_TIMER(داده, tid, ...) \
  افعل {                                 \
    (فراغ)(داده);                      \
    (فراغ)(tid);                       \
  } طالما(0)
#اگر_تعریف_نشده CURL_DISABLE_FTP
#تعریف_کن CURL_TRC_FTP(داده, ...) \
  افعل {                          \
    (فراغ)(داده);               \
  } طالما(0)
#پایان_اگر /* !CURL_DISABLE_FTP */
#اگر_تعریف_نشده CURL_DISABLE_SMTP
#تعریف_کن CURL_TRC_SMTP(داده, ...) \
  افعل {                           \
    (فراغ)(داده);                \
  } طالما(0)
#پایان_اگر /* !CURL_DISABLE_SMTP */
#اگر_تعریف_شده USE_SSL
#تعریف_کن CURL_TRC_SSLS(داده, ...) \
  افعل {                           \
    (فراغ)(داده);                \
  } طالما(0)
#پایان_اگر /* USE_SSL */
#اگر_تعریف_شده USE_SSH
#تعریف_کن CURL_TRC_SSH(داده, ...) \
  افعل {                          \
    (فراغ)(داده);               \
  } طالما(0)
#پایان_اگر /* USE_SSH */
#إذا !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#تعریف_کن CURL_TRC_WS(داده, ...) \
  افعل {                         \
    (فراغ)(داده);              \
  } طالما(0)
#پایان_اگر

#وإلا /* !CURL_HAVE_MACRO_VARARG */

#تعریف_کن infof          Curl_infof
#تعریف_کن CURL_TRC_M     Curl_trc_multi
#تعریف_کن CURL_TRC_CF    Curl_trc_cf_infof
#تعریف_کن CURL_TRC_WRITE Curl_trc_write
#تعریف_کن CURL_TRC_READ  Curl_trc_read
#تعریف_کن CURL_TRC_DNS   Curl_trc_dns
#تعریف_کن CURL_TRC_TIMER Curl_trc_timer

#اگر_تعریف_نشده CURL_DISABLE_FTP
#تعریف_کن CURL_TRC_FTP   Curl_trc_ftp
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_SMTP
#تعریف_کن CURL_TRC_SMTP  Curl_trc_smtp
#پایان_اگر
#اگر_تعریف_شده USE_SSL
#تعریف_کن CURL_TRC_SSLS  Curl_trc_ssls
#پایان_اگر
#اگر_تعریف_شده USE_SSH
#تعریف_کن CURL_TRC_SSH   Curl_trc_ssh
#پایان_اگر
#إذا !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#تعریف_کن CURL_TRC_WS    Curl_trc_ws
#پایان_اگر

#پایان_اگر /* CURL_HAVE_MACRO_VARARG */

#اگر_تعریف_شده CURLVERBOSE
خارجي بنية curl_trc_feat Curl_trc_feat_multi;
خارجي بنية curl_trc_feat Curl_trc_feat_read;
خارجي بنية curl_trc_feat Curl_trc_feat_write;
خارجي بنية curl_trc_feat Curl_trc_feat_dns;
#اگر_تعریف_نشده CURL_DISABLE_DOH
خارجي بنية curl_trc_feat Curl_trc_feat_doh;
#پایان_اگر
خارجي بنية curl_trc_feat Curl_trc_feat_timer;
#اگر_تعریف_شده USE_نخها
خارجي بنية curl_trc_feat Curl_trc_feat_نخها;
#پایان_اگر
#پایان_اگر

#اگر_تعریف_نشده CURL_DISABLE_VERBOSE_رشتهها
/* informational messages enabled */
#تعریف_کن Curl_trc_is_verbose(داده)                          \
  ((داده) && (داده)->مجموعه.verbose &&                        \
   (!(داده)->state.feat ||                                 \
    ((داده)->state.feat->log_level >= CURL_LOG_LVL_INFO)))
#تعریف_کن Curl_trc_cf_is_verbose(cf, داده)              \
  (Curl_trc_is_verbose(داده) &&                       \
   (cf) && (cf)->cft->log_level >= CURL_LOG_LVL_INFO)
#تعریف_کن Curl_trc_ft_is_verbose(داده, ft) \
  (Curl_trc_is_verbose(داده) &&          \
   (ft)->log_level >= CURL_LOG_LVL_INFO)
#تعریف_کن CURL_MSTATE_NAME(s)  Curl_trc_mstate_name((صحيح)(s))
#تعریف_کن CURL_TRC_EASY_TIMERS(داده)      \
  افعل {                                  \
    إذا(CURL_TRC_TIMER_is_verbose(داده)) \
      Curl_trc_easy_timers(داده);       \
  } طالما(0)

#وإلا /* CURL_DISABLE_VERBOSE_STRINGS */
/* All informational messages are not compiled in for size savings */
#تعریف_کن Curl_trc_is_verbose(d)        FALSE
#تعریف_کن Curl_trc_cf_is_verbose(x, y)  FALSE
#تعریف_کن Curl_trc_ft_is_verbose(x, y)  FALSE
#تعریف_کن CURL_MSTATE_NAME(x)           ((فراغ)(x), "-")
#تعریف_کن CURL_TRC_EASY_TIMERS(x)       Curl_nop_stmt
#پایان_اگر /* !CURL_DISABLE_VERBOSE_STRINGS */

#پایان_اگر /* HEADER_CURL_TRC_H */
