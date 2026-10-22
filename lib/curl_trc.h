#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_TRC_H
#وصف_ڪريو سرو_CURL_TRC_H
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
فراغ Curl_debug(بنية Curl_easy *معلومات, curl_infotype type,
                ثابت حرف *ptr, size_t سائيز);

/**
 * Output a failure message on registered callbacks for transfer.
 */
فراغ Curl_failf(بنية Curl_easy *معلومات,
                ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);

#وصف_ڪريو failf Curl_failf

/* In case failf() reported into the errorbuf, clear it again.
 * This is used to clear information from happy eyeballing attempts
 * when ultimately a successful attempt was made. */
فراغ Curl_reset_fail(بنية Curl_easy *معلومات);

#وصف_ڪريو CURL_LOG_LVL_NONE  0
#وصف_ڪريو CURL_LOG_LVL_INFO  1

/**
 * Output an informational message when transfer's verbose logging is enabled.
 */
فراغ Curl_infof(بنية Curl_easy *معلومات,
                ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);

/**
 * Output an informational message when both transfer's verbose logging
 * and connection filters verbose logging are enabled.
 */
فراغ Curl_trc_cf_infof(بنية Curl_easy *معلومات, ثابت بنية Curl_cfilter *cf,
                       ثابت حرف *fmt, ...) CURL_PRINTF(3, 4);
فراغ Curl_trc_multi(بنية Curl_easy *معلومات,
                    ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
ثابت حرف *Curl_trc_mstate_name(صحيح state);
ثابت حرف *Curl_trc_timer_name(صحيح tid);
فراغ Curl_trc_easy_timers(بنية Curl_easy *معلومات);

فراغ Curl_trc_write(بنية Curl_easy *معلومات,
                    ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
فراغ Curl_trc_read(بنية Curl_easy *معلومات,
                   ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
فراغ Curl_trc_dns(بنية Curl_easy *معلومات,
                  ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
فراغ Curl_trc_timer(بنية Curl_easy *معلومات, صحيح tid,
                    ثابت حرف *fmt, ...) CURL_PRINTF(3, 4);

بنية curl_trc_feat {
  ثابت حرف *name;
  صحيح log_level;
};

فراغ Curl_trc_feat_infof(بنية Curl_easy *معلومات,
                         بنية curl_trc_feat *feat,
                         ثابت حرف *fmt, ...) CURL_PRINTF(3, 4);

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_FTP
خارجي بنية curl_trc_feat Curl_trc_feat_ftp;
فراغ Curl_trc_ftp(بنية Curl_easy *معلومات,
                  ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_SMTP
خارجي بنية curl_trc_feat Curl_trc_feat_smtp;
فراغ Curl_trc_smtp(بنية Curl_easy *معلومات,
                   ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_SSL
خارجي بنية curl_trc_feat Curl_trc_feat_ssls;
فراغ Curl_trc_ssls(بنية Curl_easy *معلومات,
                   ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_SSH
خارجي بنية curl_trc_feat Curl_trc_feat_ssh;
فراغ Curl_trc_ssh(بنية Curl_easy *معلومات,
                  ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
#جيڪڏهن_پورو
#إذا !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
خارجي بنية curl_trc_feat Curl_trc_feat_ws;
فراغ Curl_trc_ws(بنية Curl_easy *معلومات,
                 ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
#جيڪڏهن_پورو

#وصف_ڪريو CURL_TRC_M_is_verbose(معلومات) \
  Curl_trc_ft_is_verbose(معلومات, &Curl_trc_feat_multi)
#وصف_ڪريو CURL_TRC_DNS_is_verbose(معلومات) \
  Curl_trc_ft_is_verbose(معلومات, &Curl_trc_feat_dns)
#وصف_ڪريو CURL_TRC_TIMER_is_verbose(معلومات) \
  Curl_trc_ft_is_verbose(معلومات, &Curl_trc_feat_timer)

#إذا defined(CURL_HAVE_MACRO_VARARG) && !defined(CURL_DISABLE_VERBOSE_لڙيون)
#وصف_ڪريو infof(معلومات, ...)             \
  افعل {                               \
    إذا(Curl_trc_is_verbose(معلومات))    \
      Curl_infof(معلومات, __VA_ARGS__); \
  } طالما(0)
#وصف_ڪريو CURL_TRC_M(معلومات, ...)            \
  افعل {                                   \
    إذا(CURL_TRC_M_is_verbose(معلومات))      \
      Curl_trc_multi(معلومات, __VA_ARGS__); \
  } طالما(0)
#وصف_ڪريو CURL_TRC_CF(معلومات, cf, ...)              \
  افعل {                                          \
    إذا(Curl_trc_cf_is_verbose(cf, معلومات))        \
      Curl_trc_cf_infof(معلومات, cf, __VA_ARGS__); \
  } طالما(0)
#وصف_ڪريو CURL_TRC_WRITE(معلومات, ...)                          \
  افعل {                                                     \
    إذا(Curl_trc_ft_is_verbose(معلومات, &Curl_trc_feat_write)) \
      Curl_trc_write(معلومات, __VA_ARGS__);                   \
  } طالما(0)
#وصف_ڪريو CURL_TRC_READ(معلومات, ...)                          \
  افعل {                                                    \
    إذا(Curl_trc_ft_is_verbose(معلومات, &Curl_trc_feat_read)) \
      Curl_trc_read(معلومات, __VA_ARGS__);                   \
  } طالما(0)
#وصف_ڪريو CURL_TRC_DNS(معلومات, ...)        \
  افعل {                                 \
    إذا(CURL_TRC_DNS_is_verbose(معلومات))  \
      Curl_trc_dns(معلومات, __VA_ARGS__); \
  } طالما(0)
#وصف_ڪريو CURL_TRC_TIMER(معلومات, tid, ...)        \
  افعل {                                        \
    إذا(CURL_TRC_TIMER_is_verbose(معلومات))       \
      Curl_trc_timer(معلومات, tid, __VA_ARGS__); \
  } طالما(0)

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_FTP
#وصف_ڪريو CURL_TRC_FTP(معلومات, ...)                          \
  افعل {                                                   \
    إذا(Curl_trc_ft_is_verbose(معلومات, &Curl_trc_feat_ftp)) \
      Curl_trc_ftp(معلومات, __VA_ARGS__);                   \
  } طالما(0)
#جيڪڏهن_پورو /* !CURL_DISABLE_FTP */
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_SMTP
#وصف_ڪريو CURL_TRC_SMTP(معلومات, ...)                          \
  افعل {                                                    \
    إذا(Curl_trc_ft_is_verbose(معلومات, &Curl_trc_feat_smtp)) \
      Curl_trc_smtp(معلومات, __VA_ARGS__);                   \
  } طالما(0)
#جيڪڏهن_پورو /* !CURL_DISABLE_SMTP */
#جيڪڏهن_وصف_ٿيل USE_SSL
#وصف_ڪريو CURL_TRC_SSLS(معلومات, ...)                          \
  افعل {                                                    \
    إذا(Curl_trc_ft_is_verbose(معلومات, &Curl_trc_feat_ssls)) \
      Curl_trc_ssls(معلومات, __VA_ARGS__);                   \
  } طالما(0)
#جيڪڏهن_پورو /* USE_SSL */
#جيڪڏهن_وصف_ٿيل USE_SSH
#وصف_ڪريو CURL_TRC_SSH(معلومات, ...)                          \
  افعل {                                                   \
    إذا(Curl_trc_ft_is_verbose(معلومات, &Curl_trc_feat_ssh)) \
      Curl_trc_ssh(معلومات, __VA_ARGS__);                   \
  } طالما(0)
#جيڪڏهن_پورو /* USE_SSH */
#إذا !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#وصف_ڪريو CURL_TRC_WS(معلومات, ...)                          \
  افعل {                                                  \
    إذا(Curl_trc_ft_is_verbose(معلومات, &Curl_trc_feat_ws)) \
      Curl_trc_ws(معلومات, __VA_ARGS__);                   \
  } طالما(0)
#جيڪڏهن_پورو /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#وإلاإذا defined(CURL_HAVE_MACRO_VARARG) && defined(CURL_DISABLE_VERBOSE_لڙيون)

#وصف_ڪريو infof(معلومات, ...) \
  افعل {                   \
    (فراغ)(معلومات);        \
  } طالما(0)
#وصف_ڪريو CURL_TRC_M(معلومات, ...) \
  افعل {                        \
    (فراغ)(معلومات);             \
  } طالما(0)
#وصف_ڪريو CURL_TRC_CF(معلومات, cf, ...) \
  افعل {                             \
    (فراغ)(معلومات);                  \
    (فراغ)(cf);                    \
  } طالما(0)
#وصف_ڪريو CURL_TRC_WRITE(معلومات, ...) \
  افعل {                            \
    (فراغ)(معلومات);                 \
  } طالما(0)
#وصف_ڪريو CURL_TRC_READ(معلومات, ...) \
  افعل {                           \
    (فراغ)(معلومات);                \
  } طالما(0)
#وصف_ڪريو CURL_TRC_DNS(معلومات, ...) \
  افعل {                          \
    (فراغ)(معلومات);               \
  } طالما(0)
#وصف_ڪريو CURL_TRC_TIMER(معلومات, tid, ...) \
  افعل {                                 \
    (فراغ)(معلومات);                      \
    (فراغ)(tid);                       \
  } طالما(0)
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_FTP
#وصف_ڪريو CURL_TRC_FTP(معلومات, ...) \
  افعل {                          \
    (فراغ)(معلومات);               \
  } طالما(0)
#جيڪڏهن_پورو /* !CURL_DISABLE_FTP */
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_SMTP
#وصف_ڪريو CURL_TRC_SMTP(معلومات, ...) \
  افعل {                           \
    (فراغ)(معلومات);                \
  } طالما(0)
#جيڪڏهن_پورو /* !CURL_DISABLE_SMTP */
#جيڪڏهن_وصف_ٿيل USE_SSL
#وصف_ڪريو CURL_TRC_SSLS(معلومات, ...) \
  افعل {                           \
    (فراغ)(معلومات);                \
  } طالما(0)
#جيڪڏهن_پورو /* USE_SSL */
#جيڪڏهن_وصف_ٿيل USE_SSH
#وصف_ڪريو CURL_TRC_SSH(معلومات, ...) \
  افعل {                          \
    (فراغ)(معلومات);               \
  } طالما(0)
#جيڪڏهن_پورو /* USE_SSH */
#إذا !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#وصف_ڪريو CURL_TRC_WS(معلومات, ...) \
  افعل {                         \
    (فراغ)(معلومات);              \
  } طالما(0)
#جيڪڏهن_پورو

#وإلا /* !CURL_HAVE_MACRO_VARARG */

#وصف_ڪريو infof          Curl_infof
#وصف_ڪريو CURL_TRC_M     Curl_trc_multi
#وصف_ڪريو CURL_TRC_CF    Curl_trc_cf_infof
#وصف_ڪريو CURL_TRC_WRITE Curl_trc_write
#وصف_ڪريو CURL_TRC_READ  Curl_trc_read
#وصف_ڪريو CURL_TRC_DNS   Curl_trc_dns
#وصف_ڪريو CURL_TRC_TIMER Curl_trc_timer

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_FTP
#وصف_ڪريو CURL_TRC_FTP   Curl_trc_ftp
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_SMTP
#وصف_ڪريو CURL_TRC_SMTP  Curl_trc_smtp
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_SSL
#وصف_ڪريو CURL_TRC_SSLS  Curl_trc_ssls
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_SSH
#وصف_ڪريو CURL_TRC_SSH   Curl_trc_ssh
#جيڪڏهن_پورو
#إذا !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#وصف_ڪريو CURL_TRC_WS    Curl_trc_ws
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* CURL_HAVE_MACRO_VARARG */

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
خارجي بنية curl_trc_feat Curl_trc_feat_multi;
خارجي بنية curl_trc_feat Curl_trc_feat_read;
خارجي بنية curl_trc_feat Curl_trc_feat_write;
خارجي بنية curl_trc_feat Curl_trc_feat_dns;
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_DOH
خارجي بنية curl_trc_feat Curl_trc_feat_doh;
#جيڪڏهن_پورو
خارجي بنية curl_trc_feat Curl_trc_feat_timer;
#جيڪڏهن_وصف_ٿيل USE_ڌاڳا
خارجي بنية curl_trc_feat Curl_trc_feat_ڌاڳا;
#جيڪڏهن_پورو
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_VERBOSE_لڙيون
/* informational messages enabled */
#وصف_ڪريو Curl_trc_is_verbose(معلومات)                          \
  ((معلومات) && (معلومات)->سيٽ.verbose &&                        \
   (!(معلومات)->state.feat ||                                 \
    ((معلومات)->state.feat->log_level >= CURL_LOG_LVL_INFO)))
#وصف_ڪريو Curl_trc_cf_is_verbose(cf, معلومات)              \
  (Curl_trc_is_verbose(معلومات) &&                       \
   (cf) && (cf)->cft->log_level >= CURL_LOG_LVL_INFO)
#وصف_ڪريو Curl_trc_ft_is_verbose(معلومات, ft) \
  (Curl_trc_is_verbose(معلومات) &&          \
   (ft)->log_level >= CURL_LOG_LVL_INFO)
#وصف_ڪريو CURL_MSTATE_NAME(s)  Curl_trc_mstate_name((صحيح)(s))
#وصف_ڪريو CURL_TRC_EASY_TIMERS(معلومات)      \
  افعل {                                  \
    إذا(CURL_TRC_TIMER_is_verbose(معلومات)) \
      Curl_trc_easy_timers(معلومات);       \
  } طالما(0)

#وإلا /* CURL_DISABLE_VERBOSE_STRINGS */
/* All informational messages are not compiled in for size savings */
#وصف_ڪريو Curl_trc_is_verbose(d)        FALSE
#وصف_ڪريو Curl_trc_cf_is_verbose(x, y)  FALSE
#وصف_ڪريو Curl_trc_ft_is_verbose(x, y)  FALSE
#وصف_ڪريو CURL_MSTATE_NAME(x)           ((فراغ)(x), "-")
#وصف_ڪريو CURL_TRC_EASY_TIMERS(x)       Curl_nop_stmt
#جيڪڏهن_پورو /* !CURL_DISABLE_VERBOSE_STRINGS */

#جيڪڏهن_پورو /* HEADER_CURL_TRC_H */
