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
ഘടന Curl_easy;
ഘടന Curl_cfilter;

/**
 * Init logging, return != 0 on failure.
 */
CURLcode Curl_trc_init(ശൂന്യം);

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
CURLcode Curl_trc_opt(സ്ഥിരം അക്ഷരം *config);

/* the function used to output verbose information */
ശൂന്യം Curl_debug(ഘടന Curl_easy *data, curl_infotype type,
                സ്ഥിരം അക്ഷരം *ptr, size_t size);

/**
 * Output a failure message on registered callbacks for transfer.
 */
ശൂന്യം Curl_failf(ഘടന Curl_easy *data,
                സ്ഥിരം അക്ഷരം *fmt, ...) CURL_PRINTF(2, 3);

#define failf Curl_failf

/* In case failf() reported into the errorbuf, clear it again.
 * This is used to clear information from happy eyeballing attempts
 * when ultimately a successful attempt was made. */
ശൂന്യം Curl_reset_fail(ഘടന Curl_easy *data);

#define CURL_LOG_LVL_NONE  0
#define CURL_LOG_LVL_INFO  1

/**
 * Output an informational message when transfer's verbose logging is enabled.
 */
ശൂന്യം Curl_infof(ഘടന Curl_easy *data,
                സ്ഥിരം അക്ഷരം *fmt, ...) CURL_PRINTF(2, 3);

/**
 * Output an informational message when both transfer's verbose logging
 * and connection filters verbose logging are enabled.
 */
ശൂന്യം Curl_trc_cf_infof(ഘടന Curl_easy *data, സ്ഥിരം ഘടന Curl_cfilter *cf,
                       സ്ഥിരം അക്ഷരം *fmt, ...) CURL_PRINTF(3, 4);
ശൂന്യം Curl_trc_multi(ഘടന Curl_easy *data,
                    സ്ഥിരം അക്ഷരം *fmt, ...) CURL_PRINTF(2, 3);
സ്ഥിരം അക്ഷരം *Curl_trc_mstate_name(പൂർണ്ണസംഖ്യ state);
സ്ഥിരം അക്ഷരം *Curl_trc_timer_name(പൂർണ്ണസംഖ്യ tid);
ശൂന്യം Curl_trc_easy_timers(ഘടന Curl_easy *data);

ശൂന്യം Curl_trc_write(ഘടന Curl_easy *data,
                    സ്ഥിരം അക്ഷരം *fmt, ...) CURL_PRINTF(2, 3);
ശൂന്യം Curl_trc_read(ഘടന Curl_easy *data,
                   സ്ഥിരം അക്ഷരം *fmt, ...) CURL_PRINTF(2, 3);
ശൂന്യം Curl_trc_dns(ഘടന Curl_easy *data,
                  സ്ഥിരം അക്ഷരം *fmt, ...) CURL_PRINTF(2, 3);
ശൂന്യം Curl_trc_timer(ഘടന Curl_easy *data, പൂർണ്ണസംഖ്യ tid,
                    സ്ഥിരം അക്ഷരം *fmt, ...) CURL_PRINTF(3, 4);

ഘടന curl_trc_feat {
  സ്ഥിരം അക്ഷരം *name;
  പൂർണ്ണസംഖ്യ log_level;
};

ശൂന്യം Curl_trc_feat_infof(ഘടന Curl_easy *data,
                         ഘടന curl_trc_feat *feat,
                         സ്ഥിരം അക്ഷരം *fmt, ...) CURL_PRINTF(3, 4);

#ifndef CURL_DISABLE_FTP
ബാഹ്യ ഘടന curl_trc_feat Curl_trc_feat_ftp;
ശൂന്യം Curl_trc_ftp(ഘടന Curl_easy *data,
                  സ്ഥിരം അക്ഷരം *fmt, ...) CURL_PRINTF(2, 3);
#endif
#ifndef CURL_DISABLE_SMTP
ബാഹ്യ ഘടന curl_trc_feat Curl_trc_feat_smtp;
ശൂന്യം Curl_trc_smtp(ഘടന Curl_easy *data,
                   സ്ഥിരം അക്ഷരം *fmt, ...) CURL_PRINTF(2, 3);
#endif
#ifdef USE_SSL
ബാഹ്യ ഘടന curl_trc_feat Curl_trc_feat_ssls;
ശൂന്യം Curl_trc_ssls(ഘടന Curl_easy *data,
                   സ്ഥിരം അക്ഷരം *fmt, ...) CURL_PRINTF(2, 3);
#endif
#ifdef USE_SSH
ബാഹ്യ ഘടന curl_trc_feat Curl_trc_feat_ssh;
ശൂന്യം Curl_trc_ssh(ഘടന Curl_easy *data,
                  സ്ഥിരം അക്ഷരം *fmt, ...) CURL_PRINTF(2, 3);
#endif
#എങ്കിൽ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
ബാഹ്യ ഘടന curl_trc_feat Curl_trc_feat_ws;
ശൂന്യം Curl_trc_ws(ഘടന Curl_easy *data,
                 സ്ഥിരം അക്ഷരം *fmt, ...) CURL_PRINTF(2, 3);
#endif

#define CURL_TRC_M_is_verbose(data) \
  Curl_trc_ft_is_verbose(data, &Curl_trc_feat_multi)
#define CURL_TRC_DNS_is_verbose(data) \
  Curl_trc_ft_is_verbose(data, &Curl_trc_feat_dns)
#define CURL_TRC_TIMER_is_verbose(data) \
  Curl_trc_ft_is_verbose(data, &Curl_trc_feat_timer)

#എങ്കിൽ defined(CURL_HAVE_MACRO_VARARG) && !defined(CURL_DISABLE_VERBOSE_STRINGS)
#define infof(data, ...)             \
  ചെയ്യുക {                               \
    എങ്കിൽ(Curl_trc_is_verbose(data))    \
      Curl_infof(data, __VA_ARGS__); \
  } ആകുമ്പോൾ(0)
#define CURL_TRC_M(data, ...)            \
  ചെയ്യുക {                                   \
    എങ്കിൽ(CURL_TRC_M_is_verbose(data))      \
      Curl_trc_multi(data, __VA_ARGS__); \
  } ആകുമ്പോൾ(0)
#define CURL_TRC_CF(data, cf, ...)              \
  ചെയ്യുക {                                          \
    എങ്കിൽ(Curl_trc_cf_is_verbose(cf, data))        \
      Curl_trc_cf_infof(data, cf, __VA_ARGS__); \
  } ആകുമ്പോൾ(0)
#define CURL_TRC_WRITE(data, ...)                          \
  ചെയ്യുക {                                                     \
    എങ്കിൽ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_write)) \
      Curl_trc_write(data, __VA_ARGS__);                   \
  } ആകുമ്പോൾ(0)
#define CURL_TRC_READ(data, ...)                          \
  ചെയ്യുക {                                                    \
    എങ്കിൽ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_read)) \
      Curl_trc_read(data, __VA_ARGS__);                   \
  } ആകുമ്പോൾ(0)
#define CURL_TRC_DNS(data, ...)        \
  ചെയ്യുക {                                 \
    എങ്കിൽ(CURL_TRC_DNS_is_verbose(data))  \
      Curl_trc_dns(data, __VA_ARGS__); \
  } ആകുമ്പോൾ(0)
#define CURL_TRC_TIMER(data, tid, ...)        \
  ചെയ്യുക {                                        \
    എങ്കിൽ(CURL_TRC_TIMER_is_verbose(data))       \
      Curl_trc_timer(data, tid, __VA_ARGS__); \
  } ആകുമ്പോൾ(0)

#ifndef CURL_DISABLE_FTP
#define CURL_TRC_FTP(data, ...)                          \
  ചെയ്യുക {                                                   \
    എങ്കിൽ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ftp)) \
      Curl_trc_ftp(data, __VA_ARGS__);                   \
  } ആകുമ്പോൾ(0)
#endif /* !CURL_DISABLE_FTP */
#ifndef CURL_DISABLE_SMTP
#define CURL_TRC_SMTP(data, ...)                          \
  ചെയ്യുക {                                                    \
    എങ്കിൽ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_smtp)) \
      Curl_trc_smtp(data, __VA_ARGS__);                   \
  } ആകുമ്പോൾ(0)
#endif /* !CURL_DISABLE_SMTP */
#ifdef USE_SSL
#define CURL_TRC_SSLS(data, ...)                          \
  ചെയ്യുക {                                                    \
    എങ്കിൽ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssls)) \
      Curl_trc_ssls(data, __VA_ARGS__);                   \
  } ആകുമ്പോൾ(0)
#endif /* USE_SSL */
#ifdef USE_SSH
#define CURL_TRC_SSH(data, ...)                          \
  ചെയ്യുക {                                                   \
    എങ്കിൽ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssh)) \
      Curl_trc_ssh(data, __VA_ARGS__);                   \
  } ആകുമ്പോൾ(0)
#endif /* USE_SSH */
#എങ്കിൽ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#define CURL_TRC_WS(data, ...)                          \
  ചെയ്യുക {                                                  \
    എങ്കിൽ(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ws)) \
      Curl_trc_ws(data, __VA_ARGS__);                   \
  } ആകുമ്പോൾ(0)
#endif /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#അല്ലെങ്കിലെങ്കിൽ defined(CURL_HAVE_MACRO_VARARG) && defined(CURL_DISABLE_VERBOSE_STRINGS)

#define infof(data, ...) \
  ചെയ്യുക {                   \
    (ശൂന്യം)(data);        \
  } ആകുമ്പോൾ(0)
#define CURL_TRC_M(data, ...) \
  ചെയ്യുക {                        \
    (ശൂന്യം)(data);             \
  } ആകുമ്പോൾ(0)
#define CURL_TRC_CF(data, cf, ...) \
  ചെയ്യുക {                             \
    (ശൂന്യം)(data);                  \
    (ശൂന്യം)(cf);                    \
  } ആകുമ്പോൾ(0)
#define CURL_TRC_WRITE(data, ...) \
  ചെയ്യുക {                            \
    (ശൂന്യം)(data);                 \
  } ആകുമ്പോൾ(0)
#define CURL_TRC_READ(data, ...) \
  ചെയ്യുക {                           \
    (ശൂന്യം)(data);                \
  } ആകുമ്പോൾ(0)
#define CURL_TRC_DNS(data, ...) \
  ചെയ്യുക {                          \
    (ശൂന്യം)(data);               \
  } ആകുമ്പോൾ(0)
#define CURL_TRC_TIMER(data, tid, ...) \
  ചെയ്യുക {                                 \
    (ശൂന്യം)(data);                      \
    (ശൂന്യം)(tid);                       \
  } ആകുമ്പോൾ(0)
#ifndef CURL_DISABLE_FTP
#define CURL_TRC_FTP(data, ...) \
  ചെയ്യുക {                          \
    (ശൂന്യം)(data);               \
  } ആകുമ്പോൾ(0)
#endif /* !CURL_DISABLE_FTP */
#ifndef CURL_DISABLE_SMTP
#define CURL_TRC_SMTP(data, ...) \
  ചെയ്യുക {                           \
    (ശൂന്യം)(data);                \
  } ആകുമ്പോൾ(0)
#endif /* !CURL_DISABLE_SMTP */
#ifdef USE_SSL
#define CURL_TRC_SSLS(data, ...) \
  ചെയ്യുക {                           \
    (ശൂന്യം)(data);                \
  } ആകുമ്പോൾ(0)
#endif /* USE_SSL */
#ifdef USE_SSH
#define CURL_TRC_SSH(data, ...) \
  ചെയ്യുക {                          \
    (ശൂന്യം)(data);               \
  } ആകുമ്പോൾ(0)
#endif /* USE_SSH */
#എങ്കിൽ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#define CURL_TRC_WS(data, ...) \
  ചെയ്യുക {                         \
    (ശൂന്യം)(data);              \
  } ആകുമ്പോൾ(0)
#endif

#അല്ലെങ്കിൽ /* !CURL_HAVE_MACRO_VARARG */

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
#എങ്കിൽ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#define CURL_TRC_WS    Curl_trc_ws
#endif

#endif /* CURL_HAVE_MACRO_VARARG */

#ifdef CURLVERBOSE
ബാഹ്യ ഘടന curl_trc_feat Curl_trc_feat_multi;
ബാഹ്യ ഘടന curl_trc_feat Curl_trc_feat_read;
ബാഹ്യ ഘടന curl_trc_feat Curl_trc_feat_write;
ബാഹ്യ ഘടന curl_trc_feat Curl_trc_feat_dns;
#ifndef CURL_DISABLE_DOH
ബാഹ്യ ഘടന curl_trc_feat Curl_trc_feat_doh;
#endif
ബാഹ്യ ഘടന curl_trc_feat Curl_trc_feat_timer;
#ifdef USE_THREADS
ബാഹ്യ ഘടന curl_trc_feat Curl_trc_feat_threads;
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
#define CURL_MSTATE_NAME(s)  Curl_trc_mstate_name((പൂർണ്ണസംഖ്യ)(s))
#define CURL_TRC_EASY_TIMERS(data)      \
  ചെയ്യുക {                                  \
    എങ്കിൽ(CURL_TRC_TIMER_is_verbose(data)) \
      Curl_trc_easy_timers(data);       \
  } ആകുമ്പോൾ(0)

#അല്ലെങ്കിൽ /* CURL_DISABLE_VERBOSE_STRINGS */
/* All informational messages are not compiled in for size savings */
#define Curl_trc_is_verbose(d)        FALSE
#define Curl_trc_cf_is_verbose(x, y)  FALSE
#define Curl_trc_ft_is_verbose(x, y)  FALSE
#define CURL_MSTATE_NAME(x)           ((ശൂന്യം)(x), "-")
#define CURL_TRC_EASY_TIMERS(x)       Curl_nop_stmt
#endif /* !CURL_DISABLE_VERBOSE_STRINGS */

#endif /* HEADER_CURL_TRC_H */
