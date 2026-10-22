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
সংরচনা Curl_easy;
সংরচনা Curl_cfilter;

/**
 * Init logging, return != 0 on failure.
 */
CURLcode Curl_trc_init(শূন্য);

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
CURLcode Curl_trc_opt(ধ্রুব বর্ণ *config);

/* the function used to output verbose information */
শূন্য Curl_debug(সংরচনা Curl_easy *data, curl_infotype type,
                ধ্রুব বর্ণ *ptr, size_t size);

/**
 * Output a failure message on registered callbacks for transfer.
 */
শূন্য Curl_failf(সংরচনা Curl_easy *data,
                ধ্রুব বর্ণ *fmt, ...) CURL_PRINTF(2, 3);

#define failf Curl_failf

/* In case failf() reported into the errorbuf, clear it again.
 * This is used to clear information from happy eyeballing attempts
 * when ultimately a successful attempt was made. */
শূন্য Curl_reset_fail(সংরচনা Curl_easy *data);

#define CURL_LOG_LVL_NONE  0
#define CURL_LOG_LVL_INFO  1

/**
 * Output an informational message when transfer's verbose logging is enabled.
 */
শূন্য Curl_infof(সংরচনা Curl_easy *data,
                ধ্রুব বর্ণ *fmt, ...) CURL_PRINTF(2, 3);

/**
 * Output an informational message when both transfer's verbose logging
 * and connection filters verbose logging are enabled.
 */
শূন্য Curl_trc_cf_infof(সংরচনা Curl_easy *data, ধ্রুব সংরচনা Curl_cfilter *cf,
                       ধ্রুব বর্ণ *fmt, ...) CURL_PRINTF(3, 4);
শূন্য Curl_trc_multi(সংরচনা Curl_easy *data,
                    ধ্রুব বর্ণ *fmt, ...) CURL_PRINTF(2, 3);
ধ্রুব বর্ণ *Curl_trc_mstate_name(পূর্ণাঙ্ক state);
ধ্রুব বর্ণ *Curl_trc_timer_name(পূর্ণাঙ্ক tid);
শূন্য Curl_trc_easy_timers(সংরচনা Curl_easy *data);

শূন্য Curl_trc_write(সংরচনা Curl_easy *data,
                    ধ্রুব বর্ণ *fmt, ...) CURL_PRINTF(2, 3);
শূন্য Curl_trc_read(সংরচনা Curl_easy *data,
                   ধ্রুব বর্ণ *fmt, ...) CURL_PRINTF(2, 3);
শূন্য Curl_trc_dns(সংরচনা Curl_easy *data,
                  ধ্রুব বর্ণ *fmt, ...) CURL_PRINTF(2, 3);
শূন্য Curl_trc_timer(সংরচনা Curl_easy *data, পূর্ণাঙ্ক tid,
                    ধ্রুব বর্ণ *fmt, ...) CURL_PRINTF(3, 4);

সংরচনা curl_trc_feat {
  ধ্রুব বর্ণ *name;
  পূর্ণাঙ্ক log_level;
};

শূন্য Curl_trc_feat_infof(সংরচনা Curl_easy *data,
                         সংরচনা curl_trc_feat *feat,
                         ধ্রুব বর্ণ *fmt, ...) CURL_PRINTF(3, 4);

#ifndef CURL_DISABLE_FTP
বাহ্য সংরচনা curl_trc_feat Curl_trc_feat_ftp;
শূন্য Curl_trc_ftp(সংরচনা Curl_easy *data,
                  ধ্রুব বর্ণ *fmt, ...) CURL_PRINTF(2, 3);
#endif
#ifndef CURL_DISABLE_SMTP
বাহ্য সংরচনা curl_trc_feat Curl_trc_feat_smtp;
শূন্য Curl_trc_smtp(সংরচনা Curl_easy *data,
                   ধ্রুব বর্ণ *fmt, ...) CURL_PRINTF(2, 3);
#endif
#ifdef USE_SSL
বাহ্য সংরচনা curl_trc_feat Curl_trc_feat_ssls;
শূন্য Curl_trc_ssls(সংরচনা Curl_easy *data,
                   ধ্রুব বর্ণ *fmt, ...) CURL_PRINTF(2, 3);
#endif
#ifdef USE_SSH
বাহ্য সংরচনা curl_trc_feat Curl_trc_feat_ssh;
শূন্য Curl_trc_ssh(সংরচনা Curl_easy *data,
                  ধ্রুব বর্ণ *fmt, ...) CURL_PRINTF(2, 3);
#endif
#যদি !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
বাহ্য সংরচনা curl_trc_feat Curl_trc_feat_ws;
শূন্য Curl_trc_ws(সংরচনা Curl_easy *data,
                 ধ্রুব বর্ণ *fmt, ...) CURL_PRINTF(2, 3);
#endif

#define CURL_TRC_M_is_verbose(data) \
  Curl_trc_ft_is_verbose(data, &Curl_trc_feat_multi)
#define CURL_TRC_DNS_is_verbose(data) \
  Curl_trc_ft_is_verbose(data, &Curl_trc_feat_dns)
#define CURL_TRC_TIMER_is_verbose(data) \
  Curl_trc_ft_is_verbose(data, &Curl_trc_feat_timer)

#যদি defined(CURL_HAVE_MACRO_VARARG) && !defined(CURL_DISABLE_VERBOSE_STRINGS)
#define infof(data, ...)             \
  কুরু {                               \
    যদি(Curl_trc_is_verbose(data))    \
      Curl_infof(data, __VA_ARGS__); \
  } যাবত্(0)
#define CURL_TRC_M(data, ...)            \
  কুরু {                                   \
    যদি(CURL_TRC_M_is_verbose(data))      \
      Curl_trc_multi(data, __VA_ARGS__); \
  } যাবত্(0)
#define CURL_TRC_CF(data, cf, ...)              \
  কুরু {                                          \
    যদি(Curl_trc_cf_is_verbose(cf, data))        \
      Curl_trc_cf_infof(data, cf, __VA_ARGS__); \
  } যাবত্(0)
#define CURL_TRC_WRITE(data, ...)                          \
  কুরু {                                                     \
    যদি(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_write)) \
      Curl_trc_write(data, __VA_ARGS__);                   \
  } যাবত্(0)
#define CURL_TRC_READ(data, ...)                          \
  কুরু {                                                    \
    যদি(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_read)) \
      Curl_trc_read(data, __VA_ARGS__);                   \
  } যাবত্(0)
#define CURL_TRC_DNS(data, ...)        \
  কুরু {                                 \
    যদি(CURL_TRC_DNS_is_verbose(data))  \
      Curl_trc_dns(data, __VA_ARGS__); \
  } যাবত্(0)
#define CURL_TRC_TIMER(data, tid, ...)        \
  কুরু {                                        \
    যদি(CURL_TRC_TIMER_is_verbose(data))       \
      Curl_trc_timer(data, tid, __VA_ARGS__); \
  } যাবত্(0)

#ifndef CURL_DISABLE_FTP
#define CURL_TRC_FTP(data, ...)                          \
  কুরু {                                                   \
    যদি(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ftp)) \
      Curl_trc_ftp(data, __VA_ARGS__);                   \
  } যাবত্(0)
#endif /* !CURL_DISABLE_FTP */
#ifndef CURL_DISABLE_SMTP
#define CURL_TRC_SMTP(data, ...)                          \
  কুরু {                                                    \
    যদি(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_smtp)) \
      Curl_trc_smtp(data, __VA_ARGS__);                   \
  } যাবত্(0)
#endif /* !CURL_DISABLE_SMTP */
#ifdef USE_SSL
#define CURL_TRC_SSLS(data, ...)                          \
  কুরু {                                                    \
    যদি(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssls)) \
      Curl_trc_ssls(data, __VA_ARGS__);                   \
  } যাবত্(0)
#endif /* USE_SSL */
#ifdef USE_SSH
#define CURL_TRC_SSH(data, ...)                          \
  কুরু {                                                   \
    যদি(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssh)) \
      Curl_trc_ssh(data, __VA_ARGS__);                   \
  } যাবত্(0)
#endif /* USE_SSH */
#যদি !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#define CURL_TRC_WS(data, ...)                          \
  কুরু {                                                  \
    যদি(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ws)) \
      Curl_trc_ws(data, __VA_ARGS__);                   \
  } যাবত্(0)
#endif /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#নোচেদ্যদি defined(CURL_HAVE_MACRO_VARARG) && defined(CURL_DISABLE_VERBOSE_STRINGS)

#define infof(data, ...) \
  কুরু {                   \
    (শূন্য)(data);        \
  } যাবত্(0)
#define CURL_TRC_M(data, ...) \
  কুরু {                        \
    (শূন্য)(data);             \
  } যাবত্(0)
#define CURL_TRC_CF(data, cf, ...) \
  কুরু {                             \
    (শূন্য)(data);                  \
    (শূন্য)(cf);                    \
  } যাবত্(0)
#define CURL_TRC_WRITE(data, ...) \
  কুরু {                            \
    (শূন্য)(data);                 \
  } যাবত্(0)
#define CURL_TRC_READ(data, ...) \
  কুরু {                           \
    (শূন্য)(data);                \
  } যাবত্(0)
#define CURL_TRC_DNS(data, ...) \
  কুরু {                          \
    (শূন্য)(data);               \
  } যাবত্(0)
#define CURL_TRC_TIMER(data, tid, ...) \
  কুরু {                                 \
    (শূন্য)(data);                      \
    (শূন্য)(tid);                       \
  } যাবত্(0)
#ifndef CURL_DISABLE_FTP
#define CURL_TRC_FTP(data, ...) \
  কুরু {                          \
    (শূন্য)(data);               \
  } যাবত্(0)
#endif /* !CURL_DISABLE_FTP */
#ifndef CURL_DISABLE_SMTP
#define CURL_TRC_SMTP(data, ...) \
  কুরু {                           \
    (শূন্য)(data);                \
  } যাবত্(0)
#endif /* !CURL_DISABLE_SMTP */
#ifdef USE_SSL
#define CURL_TRC_SSLS(data, ...) \
  কুরু {                           \
    (শূন্য)(data);                \
  } যাবত্(0)
#endif /* USE_SSL */
#ifdef USE_SSH
#define CURL_TRC_SSH(data, ...) \
  কুরু {                          \
    (শূন্য)(data);               \
  } যাবত্(0)
#endif /* USE_SSH */
#যদি !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#define CURL_TRC_WS(data, ...) \
  কুরু {                         \
    (শূন্য)(data);              \
  } যাবত্(0)
#endif

#অন্যথা /* !CURL_HAVE_MACRO_VARARG */

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
#যদি !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#define CURL_TRC_WS    Curl_trc_ws
#endif

#endif /* CURL_HAVE_MACRO_VARARG */

#ifdef CURLVERBOSE
বাহ্য সংরচনা curl_trc_feat Curl_trc_feat_multi;
বাহ্য সংরচনা curl_trc_feat Curl_trc_feat_read;
বাহ্য সংরচনা curl_trc_feat Curl_trc_feat_write;
বাহ্য সংরচনা curl_trc_feat Curl_trc_feat_dns;
#ifndef CURL_DISABLE_DOH
বাহ্য সংরচনা curl_trc_feat Curl_trc_feat_doh;
#endif
বাহ্য সংরচনা curl_trc_feat Curl_trc_feat_timer;
#ifdef USE_THREADS
বাহ্য সংরচনা curl_trc_feat Curl_trc_feat_threads;
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
#define CURL_MSTATE_NAME(s)  Curl_trc_mstate_name((পূর্ণাঙ্ক)(s))
#define CURL_TRC_EASY_TIMERS(data)      \
  কুরু {                                  \
    যদি(CURL_TRC_TIMER_is_verbose(data)) \
      Curl_trc_easy_timers(data);       \
  } যাবত্(0)

#অন্যথা /* CURL_DISABLE_VERBOSE_STRINGS */
/* All informational messages are not compiled in for size savings */
#define Curl_trc_is_verbose(d)        FALSE
#define Curl_trc_cf_is_verbose(x, y)  FALSE
#define Curl_trc_ft_is_verbose(x, y)  FALSE
#define CURL_MSTATE_NAME(x)           ((শূন্য)(x), "-")
#define CURL_TRC_EASY_TIMERS(x)       Curl_nop_stmt
#endif /* !CURL_DISABLE_VERBOSE_STRINGS */

#endif /* HEADER_CURL_TRC_H */
