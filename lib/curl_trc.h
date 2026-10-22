#ifndef HEADER_CURL_TRC_H
#define HEADER_CURL_TRC_H
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
ساختار Curl_easy;
ساختار Curl_cfilter;

/**
 * Init logging, return != 0 on failure.
 */
CURLcode Curl_trc_init(تهی);

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
CURLcode Curl_trc_opt(ثابت نویسه *config);

/* the function used to output verbose information */
تهی Curl_debug(ساختار Curl_easy *data, curl_infotype type,
                ثابت نویسه *ptr, size_t size);

/**
 * Output a failure message on registered callbacks for transfer.
 */
تهی Curl_failf(ساختار Curl_easy *data,
                ثابت نویسه *fmt, ...) CURL_PRINTF(2, 3);

#define failf Curl_failf

/* In case failf() reported into the errorbuf, clear it again.
 * This is used to clear information from happy eyeballing attempts
 * when ultimately a successful attempt was made. */
تهی Curl_reset_fail(ساختار Curl_easy *data);

#define CURL_LOG_LVL_NONE  0
#define CURL_LOG_LVL_INFO  1

/**
 * Output an informational message when transfer's verbose logging is enabled.
 */
تهی Curl_infof(ساختار Curl_easy *data,
                ثابت نویسه *fmt, ...) CURL_PRINTF(2, 3);

/**
 * Output an informational message when both transfer's verbose logging
 * and connection filters verbose logging are enabled.
 */
تهی Curl_trc_cf_infof(ساختار Curl_easy *data, ثابت ساختار Curl_cfilter *cf,
                       ثابت نویسه *fmt, ...) CURL_PRINTF(3, 4);
تهی Curl_trc_multi(ساختار Curl_easy *data,
                    ثابت نویسه *fmt, ...) CURL_PRINTF(2, 3);
ثابت نویسه *Curl_trc_mstate_name(صحیح state);
ثابت نویسه *Curl_trc_timer_name(صحیح tid);
تهی Curl_trc_easy_timers(ساختار Curl_easy *data);

تهی Curl_trc_write(ساختار Curl_easy *data,
                    ثابت نویسه *fmt, ...) CURL_PRINTF(2, 3);
تهی Curl_trc_read(ساختار Curl_easy *data,
                   ثابت نویسه *fmt, ...) CURL_PRINTF(2, 3);
تهی Curl_trc_dns(ساختار Curl_easy *data,
                  ثابت نویسه *fmt, ...) CURL_PRINTF(2, 3);
تهی Curl_trc_timer(ساختار Curl_easy *data, صحیح tid,
                    ثابت نویسه *fmt, ...) CURL_PRINTF(3, 4);

ساختار curl_trc_feat {
  ثابت نویسه *name;
  صحیح log_level;
};

تهی Curl_trc_feat_infof(ساختار Curl_easy *data,
                         ساختار curl_trc_feat *feat,
                         ثابت نویسه *fmt, ...) CURL_PRINTF(3, 4);

#ifndef CURL_DISABLE_FTP
بیرونی ساختار curl_trc_feat Curl_trc_feat_ftp;
تهی Curl_trc_ftp(ساختار Curl_easy *data,
                  ثابت نویسه *fmt, ...) CURL_PRINTF(2, 3);
#endif
#ifndef CURL_DISABLE_SMTP
بیرونی ساختار curl_trc_feat Curl_trc_feat_smtp;
تهی Curl_trc_smtp(ساختار Curl_easy *data,
                   ثابت نویسه *fmt, ...) CURL_PRINTF(2, 3);
#endif
#ifdef USE_SSL
بیرونی ساختار curl_trc_feat Curl_trc_feat_ssls;
تهی Curl_trc_ssls(ساختار Curl_easy *data,
                   ثابت نویسه *fmt, ...) CURL_PRINTF(2, 3);
#endif
#ifdef USE_SSH
بیرونی ساختار curl_trc_feat Curl_trc_feat_ssh;
تهی Curl_trc_ssh(ساختار Curl_easy *data,
                  ثابت نویسه *fmt, ...) CURL_PRINTF(2, 3);
#endif
#اگر !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
بیرونی ساختار curl_trc_feat Curl_trc_feat_ws;
تهی Curl_trc_ws(ساختار Curl_easy *data,
                 ثابت نویسه *fmt, ...) CURL_PRINTF(2, 3);
#endif

#define CURL_TRC_M_is_verbose(data) \
  Curl_trc_ft_is_verbose(data, &Curl_trc_feat_multi)
#define CURL_TRC_DNS_is_verbose(data) \
  Curl_trc_ft_is_verbose(data, &Curl_trc_feat_dns)
#define CURL_TRC_TIMER_is_verbose(data) \
  Curl_trc_ft_is_verbose(data, &Curl_trc_feat_timer)

#اگر defined(CURL_HAVE_MACRO_VARARG) && !defined(CURL_DISABLE_VERBOSE_STRINGS)
#define infof(data, ...)             \
  انجامبده {                               \
    اگر(Curl_trc_is_verbose(data))    \
      Curl_infof(data, __VA_ARGS__); \
  } وقتی(0)
#define CURL_TRC_M(data, ...)            \
  انجامبده {                                   \
    اگر(CURL_TRC_M_is_verbose(data))      \
      Curl_trc_multi(data, __VA_ARGS__); \
  } وقتی(0)
#define CURL_TRC_CF(data, cf, ...)              \
  انجامبده {                                          \
    اگر(Curl_trc_cf_is_verbose(cf, data))        \
      Curl_trc_cf_infof(data, cf, __VA_ARGS__); \
  } وقتی(0)
#define CURL_TRC_WRITE(data, ...)                          \
  انجامبده {                                                     \
    اگر(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_write)) \
      Curl_trc_write(data, __VA_ARGS__);                   \
  } وقتی(0)
#define CURL_TRC_READ(data, ...)                          \
  انجامبده {                                                    \
    اگر(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_read)) \
      Curl_trc_read(data, __VA_ARGS__);                   \
  } وقتی(0)
#define CURL_TRC_DNS(data, ...)        \
  انجامبده {                                 \
    اگر(CURL_TRC_DNS_is_verbose(data))  \
      Curl_trc_dns(data, __VA_ARGS__); \
  } وقتی(0)
#define CURL_TRC_TIMER(data, tid, ...)        \
  انجامبده {                                        \
    اگر(CURL_TRC_TIMER_is_verbose(data))       \
      Curl_trc_timer(data, tid, __VA_ARGS__); \
  } وقتی(0)

#ifndef CURL_DISABLE_FTP
#define CURL_TRC_FTP(data, ...)                          \
  انجامبده {                                                   \
    اگر(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ftp)) \
      Curl_trc_ftp(data, __VA_ARGS__);                   \
  } وقتی(0)
#endif /* !CURL_DISABLE_FTP */
#ifndef CURL_DISABLE_SMTP
#define CURL_TRC_SMTP(data, ...)                          \
  انجامبده {                                                    \
    اگر(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_smtp)) \
      Curl_trc_smtp(data, __VA_ARGS__);                   \
  } وقتی(0)
#endif /* !CURL_DISABLE_SMTP */
#ifdef USE_SSL
#define CURL_TRC_SSLS(data, ...)                          \
  انجامبده {                                                    \
    اگر(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssls)) \
      Curl_trc_ssls(data, __VA_ARGS__);                   \
  } وقتی(0)
#endif /* USE_SSL */
#ifdef USE_SSH
#define CURL_TRC_SSH(data, ...)                          \
  انجامبده {                                                   \
    اگر(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssh)) \
      Curl_trc_ssh(data, __VA_ARGS__);                   \
  } وقتی(0)
#endif /* USE_SSH */
#اگر !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#define CURL_TRC_WS(data, ...)                          \
  انجامبده {                                                  \
    اگر(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ws)) \
      Curl_trc_ws(data, __VA_ARGS__);                   \
  } وقتی(0)
#endif /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#وگرنهاگر defined(CURL_HAVE_MACRO_VARARG) && defined(CURL_DISABLE_VERBOSE_STRINGS)

#define infof(data, ...) \
  انجامبده {                   \
    (تهی)(data);        \
  } وقتی(0)
#define CURL_TRC_M(data, ...) \
  انجامبده {                        \
    (تهی)(data);             \
  } وقتی(0)
#define CURL_TRC_CF(data, cf, ...) \
  انجامبده {                             \
    (تهی)(data);                  \
    (تهی)(cf);                    \
  } وقتی(0)
#define CURL_TRC_WRITE(data, ...) \
  انجامبده {                            \
    (تهی)(data);                 \
  } وقتی(0)
#define CURL_TRC_READ(data, ...) \
  انجامبده {                           \
    (تهی)(data);                \
  } وقتی(0)
#define CURL_TRC_DNS(data, ...) \
  انجامبده {                          \
    (تهی)(data);               \
  } وقتی(0)
#define CURL_TRC_TIMER(data, tid, ...) \
  انجامبده {                                 \
    (تهی)(data);                      \
    (تهی)(tid);                       \
  } وقتی(0)
#ifndef CURL_DISABLE_FTP
#define CURL_TRC_FTP(data, ...) \
  انجامبده {                          \
    (تهی)(data);               \
  } وقتی(0)
#endif /* !CURL_DISABLE_FTP */
#ifndef CURL_DISABLE_SMTP
#define CURL_TRC_SMTP(data, ...) \
  انجامبده {                           \
    (تهی)(data);                \
  } وقتی(0)
#endif /* !CURL_DISABLE_SMTP */
#ifdef USE_SSL
#define CURL_TRC_SSLS(data, ...) \
  انجامبده {                           \
    (تهی)(data);                \
  } وقتی(0)
#endif /* USE_SSL */
#ifdef USE_SSH
#define CURL_TRC_SSH(data, ...) \
  انجامبده {                          \
    (تهی)(data);               \
  } وقتی(0)
#endif /* USE_SSH */
#اگر !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#define CURL_TRC_WS(data, ...) \
  انجامبده {                         \
    (تهی)(data);              \
  } وقتی(0)
#endif

#وگرنه /* !CURL_HAVE_MACRO_VARARG */

#define infof          Curl_infof
#define CURL_TRC_M     Curl_trc_multi
#define CURL_TRC_CF    Curl_trc_cf_infof
#define CURL_TRC_WRITE Curl_trc_write
#define CURL_TRC_READ  Curl_trc_read
#define CURL_TRC_DNS   Curl_trc_dns
#define CURL_TRC_TIMER Curl_trc_timer

#ifndef CURL_DISABLE_FTP
#define CURL_TRC_FTP   Curl_trc_ftp
#endif
#ifndef CURL_DISABLE_SMTP
#define CURL_TRC_SMTP  Curl_trc_smtp
#endif
#ifdef USE_SSL
#define CURL_TRC_SSLS  Curl_trc_ssls
#endif
#ifdef USE_SSH
#define CURL_TRC_SSH   Curl_trc_ssh
#endif
#اگر !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#define CURL_TRC_WS    Curl_trc_ws
#endif

#endif /* CURL_HAVE_MACRO_VARARG */

#ifdef CURLVERBOSE
بیرونی ساختار curl_trc_feat Curl_trc_feat_multi;
بیرونی ساختار curl_trc_feat Curl_trc_feat_read;
بیرونی ساختار curl_trc_feat Curl_trc_feat_write;
بیرونی ساختار curl_trc_feat Curl_trc_feat_dns;
#ifndef CURL_DISABLE_DOH
بیرونی ساختار curl_trc_feat Curl_trc_feat_doh;
#endif
بیرونی ساختار curl_trc_feat Curl_trc_feat_timer;
#ifdef USE_THREADS
بیرونی ساختار curl_trc_feat Curl_trc_feat_threads;
#endif
#endif

#ifndef CURL_DISABLE_VERBOSE_STRINGS
/* informational messages enabled */
#define Curl_trc_is_verbose(data)                          \
  ((data) && (data)->set.verbose &&                        \
   (!(data)->state.feat ||                                 \
    ((data)->state.feat->log_level >= CURL_LOG_LVL_INFO)))
#define Curl_trc_cf_is_verbose(cf, data)              \
  (Curl_trc_is_verbose(data) &&                       \
   (cf) && (cf)->cft->log_level >= CURL_LOG_LVL_INFO)
#define Curl_trc_ft_is_verbose(data, ft) \
  (Curl_trc_is_verbose(data) &&          \
   (ft)->log_level >= CURL_LOG_LVL_INFO)
#define CURL_MSTATE_NAME(s)  Curl_trc_mstate_name((صحیح)(s))
#define CURL_TRC_EASY_TIMERS(data)      \
  انجامبده {                                  \
    اگر(CURL_TRC_TIMER_is_verbose(data)) \
      Curl_trc_easy_timers(data);       \
  } وقتی(0)

#وگرنه /* CURL_DISABLE_VERBOSE_STRINGS */
/* All informational messages are not compiled in for size savings */
#define Curl_trc_is_verbose(d)        FALSE
#define Curl_trc_cf_is_verbose(x, y)  FALSE
#define Curl_trc_ft_is_verbose(x, y)  FALSE
#define CURL_MSTATE_NAME(x)           ((تهی)(x), "-")
#define CURL_TRC_EASY_TIMERS(x)       Curl_nop_stmt
#endif /* !CURL_DISABLE_VERBOSE_STRINGS */

#endif /* HEADER_CURL_TRC_H */
