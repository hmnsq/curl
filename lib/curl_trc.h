#اگر_غیر_تعریف_شدہ سرخی_CURL_TRC_H
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
ساخت Curl_easy;
ساخت Curl_cfilter;

/**
 * Init logging, return != 0 on failure.
 */
CURLcode Curl_trc_init(خالی);

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
خالی Curl_debug(ساخت Curl_easy *ڈیٹا, curl_infotype type,
                ثابت حرف *ptr, size_t سائز);

/**
 * Output a failure message on registered callbacks for transfer.
 */
خالی Curl_failf(ساخت Curl_easy *ڈیٹا,
                ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);

#تعریف_کرو failf Curl_failf

/* In case failf() reported into the errorbuf, clear it again.
 * This is used to clear information from happy eyeballing attempts
 * when ultimately a successful attempt was made. */
خالی Curl_reset_fail(ساخت Curl_easy *ڈیٹا);

#تعریف_کرو CURL_LOG_LVL_NONE  0
#تعریف_کرو CURL_LOG_LVL_INFO  1

/**
 * Output an informational message when transfer's verbose logging is enabled.
 */
خالی Curl_infof(ساخت Curl_easy *ڈیٹا,
                ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);

/**
 * Output an informational message when both transfer's verbose logging
 * and connection filters verbose logging are enabled.
 */
خالی Curl_trc_cf_infof(ساخت Curl_easy *ڈیٹا, ثابت ساخت Curl_cfilter *cf,
                       ثابت حرف *fmt, ...) CURL_PRINTF(3, 4);
خالی Curl_trc_multi(ساخت Curl_easy *ڈیٹا,
                    ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
ثابت حرف *Curl_trc_mstate_name(صحیح state);
ثابت حرف *Curl_trc_timer_name(صحیح tid);
خالی Curl_trc_easy_timers(ساخت Curl_easy *ڈیٹا);

خالی Curl_trc_write(ساخت Curl_easy *ڈیٹا,
                    ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
خالی Curl_trc_read(ساخت Curl_easy *ڈیٹا,
                   ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
خالی Curl_trc_dns(ساخت Curl_easy *ڈیٹا,
                  ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
خالی Curl_trc_timer(ساخت Curl_easy *ڈیٹا, صحیح tid,
                    ثابت حرف *fmt, ...) CURL_PRINTF(3, 4);

ساخت curl_trc_feat {
  ثابت حرف *name;
  صحیح log_level;
};

خالی Curl_trc_feat_infof(ساخت Curl_easy *ڈیٹا,
                         ساخت curl_trc_feat *feat,
                         ثابت حرف *fmt, ...) CURL_PRINTF(3, 4);

#اگر_غیر_تعریف_شدہ CURL_DISABLE_FTP
خارجی ساخت curl_trc_feat Curl_trc_feat_ftp;
خالی Curl_trc_ftp(ساخت Curl_easy *ڈیٹا,
                  ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_SMTP
خارجی ساخت curl_trc_feat Curl_trc_feat_smtp;
خالی Curl_trc_smtp(ساخت Curl_easy *ڈیٹا,
                   ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
#اگر_ختم
#اگر_تعریف_شدہ USE_SSL
خارجی ساخت curl_trc_feat Curl_trc_feat_ssls;
خالی Curl_trc_ssls(ساخت Curl_easy *ڈیٹا,
                   ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
#اگر_ختم
#اگر_تعریف_شدہ USE_SSH
خارجی ساخت curl_trc_feat Curl_trc_feat_ssh;
خالی Curl_trc_ssh(ساخت Curl_easy *ڈیٹا,
                  ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
#اگر_ختم
#اگر !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
خارجی ساخت curl_trc_feat Curl_trc_feat_ws;
خالی Curl_trc_ws(ساخت Curl_easy *ڈیٹا,
                 ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);
#اگر_ختم

#تعریف_کرو CURL_TRC_M_is_verbose(ڈیٹا) \
  Curl_trc_ft_is_verbose(ڈیٹا, &Curl_trc_feat_multi)
#تعریف_کرو CURL_TRC_DNS_is_verbose(ڈیٹا) \
  Curl_trc_ft_is_verbose(ڈیٹا, &Curl_trc_feat_dns)
#تعریف_کرو CURL_TRC_TIMER_is_verbose(ڈیٹا) \
  Curl_trc_ft_is_verbose(ڈیٹا, &Curl_trc_feat_timer)

#اگر defined(CURL_HAVE_MACRO_VARARG) && !defined(CURL_DISABLE_VERBOSE_لڑیاں)
#تعریف_کرو infof(ڈیٹا, ...)             \
  کرو {                               \
    اگر(Curl_trc_is_verbose(ڈیٹا))    \
      Curl_infof(ڈیٹا, __VA_ARGS__); \
  } جب(0)
#تعریف_کرو CURL_TRC_M(ڈیٹا, ...)            \
  کرو {                                   \
    اگر(CURL_TRC_M_is_verbose(ڈیٹا))      \
      Curl_trc_multi(ڈیٹا, __VA_ARGS__); \
  } جب(0)
#تعریف_کرو CURL_TRC_CF(ڈیٹا, cf, ...)              \
  کرو {                                          \
    اگر(Curl_trc_cf_is_verbose(cf, ڈیٹا))        \
      Curl_trc_cf_infof(ڈیٹا, cf, __VA_ARGS__); \
  } جب(0)
#تعریف_کرو CURL_TRC_WRITE(ڈیٹا, ...)                          \
  کرو {                                                     \
    اگر(Curl_trc_ft_is_verbose(ڈیٹا, &Curl_trc_feat_write)) \
      Curl_trc_write(ڈیٹا, __VA_ARGS__);                   \
  } جب(0)
#تعریف_کرو CURL_TRC_READ(ڈیٹا, ...)                          \
  کرو {                                                    \
    اگر(Curl_trc_ft_is_verbose(ڈیٹا, &Curl_trc_feat_read)) \
      Curl_trc_read(ڈیٹا, __VA_ARGS__);                   \
  } جب(0)
#تعریف_کرو CURL_TRC_DNS(ڈیٹا, ...)        \
  کرو {                                 \
    اگر(CURL_TRC_DNS_is_verbose(ڈیٹا))  \
      Curl_trc_dns(ڈیٹا, __VA_ARGS__); \
  } جب(0)
#تعریف_کرو CURL_TRC_TIMER(ڈیٹا, tid, ...)        \
  کرو {                                        \
    اگر(CURL_TRC_TIMER_is_verbose(ڈیٹا))       \
      Curl_trc_timer(ڈیٹا, tid, __VA_ARGS__); \
  } جب(0)

#اگر_غیر_تعریف_شدہ CURL_DISABLE_FTP
#تعریف_کرو CURL_TRC_FTP(ڈیٹا, ...)                          \
  کرو {                                                   \
    اگر(Curl_trc_ft_is_verbose(ڈیٹا, &Curl_trc_feat_ftp)) \
      Curl_trc_ftp(ڈیٹا, __VA_ARGS__);                   \
  } جب(0)
#اگر_ختم /* !CURL_DISABLE_FTP */
#اگر_غیر_تعریف_شدہ CURL_DISABLE_SMTP
#تعریف_کرو CURL_TRC_SMTP(ڈیٹا, ...)                          \
  کرو {                                                    \
    اگر(Curl_trc_ft_is_verbose(ڈیٹا, &Curl_trc_feat_smtp)) \
      Curl_trc_smtp(ڈیٹا, __VA_ARGS__);                   \
  } جب(0)
#اگر_ختم /* !CURL_DISABLE_SMTP */
#اگر_تعریف_شدہ USE_SSL
#تعریف_کرو CURL_TRC_SSLS(ڈیٹا, ...)                          \
  کرو {                                                    \
    اگر(Curl_trc_ft_is_verbose(ڈیٹا, &Curl_trc_feat_ssls)) \
      Curl_trc_ssls(ڈیٹا, __VA_ARGS__);                   \
  } جب(0)
#اگر_ختم /* USE_SSL */
#اگر_تعریف_شدہ USE_SSH
#تعریف_کرو CURL_TRC_SSH(ڈیٹا, ...)                          \
  کرو {                                                   \
    اگر(Curl_trc_ft_is_verbose(ڈیٹا, &Curl_trc_feat_ssh)) \
      Curl_trc_ssh(ڈیٹا, __VA_ARGS__);                   \
  } جب(0)
#اگر_ختم /* USE_SSH */
#اگر !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#تعریف_کرو CURL_TRC_WS(ڈیٹا, ...)                          \
  کرو {                                                  \
    اگر(Curl_trc_ft_is_verbose(ڈیٹا, &Curl_trc_feat_ws)) \
      Curl_trc_ws(ڈیٹا, __VA_ARGS__);                   \
  } جب(0)
#اگر_ختم /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#وگرنہ_اگر defined(CURL_HAVE_MACRO_VARARG) && defined(CURL_DISABLE_VERBOSE_لڑیاں)

#تعریف_کرو infof(ڈیٹا, ...) \
  کرو {                   \
    (خالی)(ڈیٹا);        \
  } جب(0)
#تعریف_کرو CURL_TRC_M(ڈیٹا, ...) \
  کرو {                        \
    (خالی)(ڈیٹا);             \
  } جب(0)
#تعریف_کرو CURL_TRC_CF(ڈیٹا, cf, ...) \
  کرو {                             \
    (خالی)(ڈیٹا);                  \
    (خالی)(cf);                    \
  } جب(0)
#تعریف_کرو CURL_TRC_WRITE(ڈیٹا, ...) \
  کرو {                            \
    (خالی)(ڈیٹا);                 \
  } جب(0)
#تعریف_کرو CURL_TRC_READ(ڈیٹا, ...) \
  کرو {                           \
    (خالی)(ڈیٹا);                \
  } جب(0)
#تعریف_کرو CURL_TRC_DNS(ڈیٹا, ...) \
  کرو {                          \
    (خالی)(ڈیٹا);               \
  } جب(0)
#تعریف_کرو CURL_TRC_TIMER(ڈیٹا, tid, ...) \
  کرو {                                 \
    (خالی)(ڈیٹا);                      \
    (خالی)(tid);                       \
  } جب(0)
#اگر_غیر_تعریف_شدہ CURL_DISABLE_FTP
#تعریف_کرو CURL_TRC_FTP(ڈیٹا, ...) \
  کرو {                          \
    (خالی)(ڈیٹا);               \
  } جب(0)
#اگر_ختم /* !CURL_DISABLE_FTP */
#اگر_غیر_تعریف_شدہ CURL_DISABLE_SMTP
#تعریف_کرو CURL_TRC_SMTP(ڈیٹا, ...) \
  کرو {                           \
    (خالی)(ڈیٹا);                \
  } جب(0)
#اگر_ختم /* !CURL_DISABLE_SMTP */
#اگر_تعریف_شدہ USE_SSL
#تعریف_کرو CURL_TRC_SSLS(ڈیٹا, ...) \
  کرو {                           \
    (خالی)(ڈیٹا);                \
  } جب(0)
#اگر_ختم /* USE_SSL */
#اگر_تعریف_شدہ USE_SSH
#تعریف_کرو CURL_TRC_SSH(ڈیٹا, ...) \
  کرو {                          \
    (خالی)(ڈیٹا);               \
  } جب(0)
#اگر_ختم /* USE_SSH */
#اگر !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#تعریف_کرو CURL_TRC_WS(ڈیٹا, ...) \
  کرو {                         \
    (خالی)(ڈیٹا);              \
  } جب(0)
#اگر_ختم

#ورنہ /* !CURL_HAVE_MACRO_VARARG */

#تعریف_کرو infof          Curl_infof
#تعریف_کرو CURL_TRC_M     Curl_trc_multi
#تعریف_کرو CURL_TRC_CF    Curl_trc_cf_infof
#تعریف_کرو CURL_TRC_WRITE Curl_trc_write
#تعریف_کرو CURL_TRC_READ  Curl_trc_read
#تعریف_کرو CURL_TRC_DNS   Curl_trc_dns
#تعریف_کرو CURL_TRC_TIMER Curl_trc_timer

#اگر_غیر_تعریف_شدہ CURL_DISABLE_FTP
#تعریف_کرو CURL_TRC_FTP   Curl_trc_ftp
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_SMTP
#تعریف_کرو CURL_TRC_SMTP  Curl_trc_smtp
#اگر_ختم
#اگر_تعریف_شدہ USE_SSL
#تعریف_کرو CURL_TRC_SSLS  Curl_trc_ssls
#اگر_ختم
#اگر_تعریف_شدہ USE_SSH
#تعریف_کرو CURL_TRC_SSH   Curl_trc_ssh
#اگر_ختم
#اگر !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#تعریف_کرو CURL_TRC_WS    Curl_trc_ws
#اگر_ختم

#اگر_ختم /* CURL_HAVE_MACRO_VARARG */

#اگر_تعریف_شدہ CURLVERBOSE
خارجی ساخت curl_trc_feat Curl_trc_feat_multi;
خارجی ساخت curl_trc_feat Curl_trc_feat_read;
خارجی ساخت curl_trc_feat Curl_trc_feat_write;
خارجی ساخت curl_trc_feat Curl_trc_feat_dns;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_DOH
خارجی ساخت curl_trc_feat Curl_trc_feat_doh;
#اگر_ختم
خارجی ساخت curl_trc_feat Curl_trc_feat_timer;
#اگر_تعریف_شدہ USE_دھاگے
خارجی ساخت curl_trc_feat Curl_trc_feat_دھاگے;
#اگر_ختم
#اگر_ختم

#اگر_غیر_تعریف_شدہ CURL_DISABLE_VERBOSE_لڑیاں
/* informational messages enabled */
#تعریف_کرو Curl_trc_is_verbose(ڈیٹا)                          \
  ((ڈیٹا) && (ڈیٹا)->سیٹ.verbose &&                        \
   (!(ڈیٹا)->state.feat ||                                 \
    ((ڈیٹا)->state.feat->log_level >= CURL_LOG_LVL_INFO)))
#تعریف_کرو Curl_trc_cf_is_verbose(cf, ڈیٹا)              \
  (Curl_trc_is_verbose(ڈیٹا) &&                       \
   (cf) && (cf)->cft->log_level >= CURL_LOG_LVL_INFO)
#تعریف_کرو Curl_trc_ft_is_verbose(ڈیٹا, ft) \
  (Curl_trc_is_verbose(ڈیٹا) &&          \
   (ft)->log_level >= CURL_LOG_LVL_INFO)
#تعریف_کرو CURL_MSTATE_NAME(s)  Curl_trc_mstate_name((صحیح)(s))
#تعریف_کرو CURL_TRC_EASY_TIMERS(ڈیٹا)      \
  کرو {                                  \
    اگر(CURL_TRC_TIMER_is_verbose(ڈیٹا)) \
      Curl_trc_easy_timers(ڈیٹا);       \
  } جب(0)

#ورنہ /* CURL_DISABLE_VERBOSE_STRINGS */
/* All informational messages are not compiled in for size savings */
#تعریف_کرو Curl_trc_is_verbose(d)        FALSE
#تعریف_کرو Curl_trc_cf_is_verbose(x, y)  FALSE
#تعریف_کرو Curl_trc_ft_is_verbose(x, y)  FALSE
#تعریف_کرو CURL_MSTATE_NAME(x)           ((خالی)(x), "-")
#تعریف_کرو CURL_TRC_EASY_TIMERS(x)       Curl_nop_stmt
#اگر_ختم /* !CURL_DISABLE_VERBOSE_STRINGS */

#اگر_ختم /* HEADER_CURL_TRC_H */
