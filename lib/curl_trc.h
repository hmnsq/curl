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
কাঠামো Curl_easy;
কাঠামো Curl_cfilter;

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
CURLcode Curl_trc_opt(ধ্রুবক অক্ষর *config);

/* the function used to output verbose information */
শূন্য Curl_debug(কাঠামো Curl_easy *তথ্য, curl_infotype type,
                ধ্রুবক অক্ষর *ptr, size_t আকার_2);

/**
 * Output a failure message on registered callbacks for transfer.
 */
শূন্য Curl_failf(কাঠামো Curl_easy *তথ্য,
                ধ্রুবক অক্ষর *fmt, ...) CURL_PRINTF(2, 3);

#define failf Curl_failf

/* In case failf() reported into the errorbuf, clear it again.
 * This is used to clear information from happy eyeballing attempts
 * when ultimately a successful attempt was made. */
শূন্য Curl_reset_fail(কাঠামো Curl_easy *তথ্য);

#define CURL_LOG_LVL_NONE  0
#define CURL_LOG_LVL_INFO  1

/**
 * Output an informational message when transfer's verbose logging is enabled.
 */
শূন্য Curl_infof(কাঠামো Curl_easy *তথ্য,
                ধ্রুবক অক্ষর *fmt, ...) CURL_PRINTF(2, 3);

/**
 * Output an informational message when both transfer's verbose logging
 * and connection filters verbose logging are enabled.
 */
শূন্য Curl_trc_cf_infof(কাঠামো Curl_easy *তথ্য, ধ্রুবক কাঠামো Curl_cfilter *cf,
                       ধ্রুবক অক্ষর *fmt, ...) CURL_PRINTF(3, 4);
শূন্য Curl_trc_multi(কাঠামো Curl_easy *তথ্য,
                    ধ্রুবক অক্ষর *fmt, ...) CURL_PRINTF(2, 3);
ধ্রুবক অক্ষর *Curl_trc_mstate_name(পূর্ণসংখ্যা state);
ধ্রুবক অক্ষর *Curl_trc_timer_name(পূর্ণসংখ্যা tid);
শূন্য Curl_trc_easy_timers(কাঠামো Curl_easy *তথ্য);

শূন্য Curl_trc_write(কাঠামো Curl_easy *তথ্য,
                    ধ্রুবক অক্ষর *fmt, ...) CURL_PRINTF(2, 3);
শূন্য Curl_trc_read(কাঠামো Curl_easy *তথ্য,
                   ধ্রুবক অক্ষর *fmt, ...) CURL_PRINTF(2, 3);
শূন্য Curl_trc_dns(কাঠামো Curl_easy *তথ্য,
                  ধ্রুবক অক্ষর *fmt, ...) CURL_PRINTF(2, 3);
শূন্য Curl_trc_timer(কাঠামো Curl_easy *তথ্য, পূর্ণসংখ্যা tid,
                    ধ্রুবক অক্ষর *fmt, ...) CURL_PRINTF(3, 4);

কাঠামো curl_trc_feat {
  ধ্রুবক অক্ষর *name;
  পূর্ণসংখ্যা log_level;
};

শূন্য Curl_trc_feat_infof(কাঠামো Curl_easy *তথ্য,
                         কাঠামো curl_trc_feat *feat,
                         ধ্রুবক অক্ষর *fmt, ...) CURL_PRINTF(3, 4);

#ifndef CURL_DISABLE_FTP
বাহ্যিক কাঠামো curl_trc_feat Curl_trc_feat_ftp;
শূন্য Curl_trc_ftp(কাঠামো Curl_easy *তথ্য,
                  ধ্রুবক অক্ষর *fmt, ...) CURL_PRINTF(2, 3);
#endif
#ifndef CURL_DISABLE_SMTP
বাহ্যিক কাঠামো curl_trc_feat Curl_trc_feat_smtp;
শূন্য Curl_trc_smtp(কাঠামো Curl_easy *তথ্য,
                   ধ্রুবক অক্ষর *fmt, ...) CURL_PRINTF(2, 3);
#endif
#ifdef USE_SSL
বাহ্যিক কাঠামো curl_trc_feat Curl_trc_feat_ssls;
শূন্য Curl_trc_ssls(কাঠামো Curl_easy *তথ্য,
                   ধ্রুবক অক্ষর *fmt, ...) CURL_PRINTF(2, 3);
#endif
#ifdef USE_SSH
বাহ্যিক কাঠামো curl_trc_feat Curl_trc_feat_ssh;
শূন্য Curl_trc_ssh(কাঠামো Curl_easy *তথ্য,
                  ধ্রুবক অক্ষর *fmt, ...) CURL_PRINTF(2, 3);
#endif
#যদি !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
বাহ্যিক কাঠামো curl_trc_feat Curl_trc_feat_ws;
শূন্য Curl_trc_ws(কাঠামো Curl_easy *তথ্য,
                 ধ্রুবক অক্ষর *fmt, ...) CURL_PRINTF(2, 3);
#endif

#define CURL_TRC_M_is_verbose(তথ্য) \
  Curl_trc_ft_is_verbose(তথ্য, &Curl_trc_feat_multi)
#define CURL_TRC_DNS_is_verbose(তথ্য) \
  Curl_trc_ft_is_verbose(তথ্য, &Curl_trc_feat_dns)
#define CURL_TRC_TIMER_is_verbose(তথ্য) \
  Curl_trc_ft_is_verbose(তথ্য, &Curl_trc_feat_timer)

#যদি defined(CURL_HAVE_MACRO_VARARG) && !defined(CURL_DISABLE_VERBOSE_STRINGS)
#define infof(তথ্য, ...)             \
  করো {                               \
    যদি(Curl_trc_is_verbose(তথ্য))    \
      Curl_infof(তথ্য, __VA_ARGS__); \
  } যখন(0)
#define CURL_TRC_M(তথ্য, ...)            \
  করো {                                   \
    যদি(CURL_TRC_M_is_verbose(তথ্য))      \
      Curl_trc_multi(তথ্য, __VA_ARGS__); \
  } যখন(0)
#define CURL_TRC_CF(তথ্য, cf, ...)              \
  করো {                                          \
    যদি(Curl_trc_cf_is_verbose(cf, তথ্য))        \
      Curl_trc_cf_infof(তথ্য, cf, __VA_ARGS__); \
  } যখন(0)
#define CURL_TRC_WRITE(তথ্য, ...)                          \
  করো {                                                     \
    যদি(Curl_trc_ft_is_verbose(তথ্য, &Curl_trc_feat_write)) \
      Curl_trc_write(তথ্য, __VA_ARGS__);                   \
  } যখন(0)
#define CURL_TRC_READ(তথ্য, ...)                          \
  করো {                                                    \
    যদি(Curl_trc_ft_is_verbose(তথ্য, &Curl_trc_feat_read)) \
      Curl_trc_read(তথ্য, __VA_ARGS__);                   \
  } যখন(0)
#define CURL_TRC_DNS(তথ্য, ...)        \
  করো {                                 \
    যদি(CURL_TRC_DNS_is_verbose(তথ্য))  \
      Curl_trc_dns(তথ্য, __VA_ARGS__); \
  } যখন(0)
#define CURL_TRC_TIMER(তথ্য, tid, ...)        \
  করো {                                        \
    যদি(CURL_TRC_TIMER_is_verbose(তথ্য))       \
      Curl_trc_timer(তথ্য, tid, __VA_ARGS__); \
  } যখন(0)

#ifndef CURL_DISABLE_FTP
#define CURL_TRC_FTP(তথ্য, ...)                          \
  করো {                                                   \
    যদি(Curl_trc_ft_is_verbose(তথ্য, &Curl_trc_feat_ftp)) \
      Curl_trc_ftp(তথ্য, __VA_ARGS__);                   \
  } যখন(0)
#endif /* !CURL_DISABLE_FTP */
#ifndef CURL_DISABLE_SMTP
#define CURL_TRC_SMTP(তথ্য, ...)                          \
  করো {                                                    \
    যদি(Curl_trc_ft_is_verbose(তথ্য, &Curl_trc_feat_smtp)) \
      Curl_trc_smtp(তথ্য, __VA_ARGS__);                   \
  } যখন(0)
#endif /* !CURL_DISABLE_SMTP */
#ifdef USE_SSL
#define CURL_TRC_SSLS(তথ্য, ...)                          \
  করো {                                                    \
    যদি(Curl_trc_ft_is_verbose(তথ্য, &Curl_trc_feat_ssls)) \
      Curl_trc_ssls(তথ্য, __VA_ARGS__);                   \
  } যখন(0)
#endif /* USE_SSL */
#ifdef USE_SSH
#define CURL_TRC_SSH(তথ্য, ...)                          \
  করো {                                                   \
    যদি(Curl_trc_ft_is_verbose(তথ্য, &Curl_trc_feat_ssh)) \
      Curl_trc_ssh(তথ্য, __VA_ARGS__);                   \
  } যখন(0)
#endif /* USE_SSH */
#যদি !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#define CURL_TRC_WS(তথ্য, ...)                          \
  করো {                                                  \
    যদি(Curl_trc_ft_is_verbose(তথ্য, &Curl_trc_feat_ws)) \
      Curl_trc_ws(তথ্য, __VA_ARGS__);                   \
  } যখন(0)
#endif /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#নতুবা_যদি defined(CURL_HAVE_MACRO_VARARG) && defined(CURL_DISABLE_VERBOSE_STRINGS)

#define infof(তথ্য, ...) \
  করো {                   \
    (শূন্য)(তথ্য);        \
  } যখন(0)
#define CURL_TRC_M(তথ্য, ...) \
  করো {                        \
    (শূন্য)(তথ্য);             \
  } যখন(0)
#define CURL_TRC_CF(তথ্য, cf, ...) \
  করো {                             \
    (শূন্য)(তথ্য);                  \
    (শূন্য)(cf);                    \
  } যখন(0)
#define CURL_TRC_WRITE(তথ্য, ...) \
  করো {                            \
    (শূন্য)(তথ্য);                 \
  } যখন(0)
#define CURL_TRC_READ(তথ্য, ...) \
  করো {                           \
    (শূন্য)(তথ্য);                \
  } যখন(0)
#define CURL_TRC_DNS(তথ্য, ...) \
  করো {                          \
    (শূন্য)(তথ্য);               \
  } যখন(0)
#define CURL_TRC_TIMER(তথ্য, tid, ...) \
  করো {                                 \
    (শূন্য)(তথ্য);                      \
    (শূন্য)(tid);                       \
  } যখন(0)
#ifndef CURL_DISABLE_FTP
#define CURL_TRC_FTP(তথ্য, ...) \
  করো {                          \
    (শূন্য)(তথ্য);               \
  } যখন(0)
#endif /* !CURL_DISABLE_FTP */
#ifndef CURL_DISABLE_SMTP
#define CURL_TRC_SMTP(তথ্য, ...) \
  করো {                           \
    (শূন্য)(তথ্য);                \
  } যখন(0)
#endif /* !CURL_DISABLE_SMTP */
#ifdef USE_SSL
#define CURL_TRC_SSLS(তথ্য, ...) \
  করো {                           \
    (শূন্য)(তথ্য);                \
  } যখন(0)
#endif /* USE_SSL */
#ifdef USE_SSH
#define CURL_TRC_SSH(তথ্য, ...) \
  করো {                          \
    (শূন্য)(তথ্য);               \
  } যখন(0)
#endif /* USE_SSH */
#যদি !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#define CURL_TRC_WS(তথ্য, ...) \
  করো {                         \
    (শূন্য)(তথ্য);              \
  } যখন(0)
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
বাহ্যিক কাঠামো curl_trc_feat Curl_trc_feat_multi;
বাহ্যিক কাঠামো curl_trc_feat Curl_trc_feat_read;
বাহ্যিক কাঠামো curl_trc_feat Curl_trc_feat_write;
বাহ্যিক কাঠামো curl_trc_feat Curl_trc_feat_dns;
#ifndef CURL_DISABLE_DOH
বাহ্যিক কাঠামো curl_trc_feat Curl_trc_feat_doh;
#endif
বাহ্যিক কাঠামো curl_trc_feat Curl_trc_feat_timer;
#ifdef USE_THREADS
বাহ্যিক কাঠামো curl_trc_feat Curl_trc_feat_threads;
#endif
#endif

#ifndef CURL_DISABLE_VERBOSE_STRINGS
/* informational messages enabled */
#define Curl_trc_is_verbose(তথ্য)                          \
  ((তথ্য) && (তথ্য)->set.verbose &&                        \
   (!(তথ্য)->state.feat ||                                 \
    ((তথ্য)->state.feat->log_level >= CURL_LOG_LVL_INFO)))
#define Curl_trc_cf_is_verbose(cf, তথ্য)              \
  (Curl_trc_is_verbose(তথ্য) &&                       \
   (cf) && (cf)->cft->log_level >= CURL_LOG_LVL_INFO)
#define Curl_trc_ft_is_verbose(তথ্য, ft) \
  (Curl_trc_is_verbose(তথ্য) &&          \
   (ft)->log_level >= CURL_LOG_LVL_INFO)
#define CURL_MSTATE_NAME(s)  Curl_trc_mstate_name((পূর্ণসংখ্যা)(s))
#define CURL_TRC_EASY_TIMERS(তথ্য)      \
  করো {                                  \
    যদি(CURL_TRC_TIMER_is_verbose(তথ্য)) \
      Curl_trc_easy_timers(তথ্য);       \
  } যখন(0)

#অন্যথা /* CURL_DISABLE_VERBOSE_STRINGS */
/* All informational messages are not compiled in for size savings */
#define Curl_trc_is_verbose(d)        FALSE
#define Curl_trc_cf_is_verbose(x, y)  FALSE
#define Curl_trc_ft_is_verbose(x, y)  FALSE
#define CURL_MSTATE_NAME(x)           ((শূন্য)(x), "-")
#define CURL_TRC_EASY_TIMERS(x)       Curl_nop_stmt
#endif /* !CURL_DISABLE_VERBOSE_STRINGS */

#endif /* HEADER_CURL_TRC_H */
