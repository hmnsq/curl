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
સંરચના Curl_easy;
સંરચના Curl_cfilter;

/**
 * Init logging, return != 0 on failure.
 */
CURLcode Curl_trc_init(શૂન્ય);

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
CURLcode Curl_trc_opt(અચલ અક્ષર *config);

/* the function used to output verbose information */
શૂન્ય Curl_debug(સંરચના Curl_easy *data, curl_infotype type,
                અચલ અક્ષર *ptr, size_t size);

/**
 * Output a failure message on registered callbacks for transfer.
 */
શૂન્ય Curl_failf(સંરચના Curl_easy *data,
                અચલ અક્ષર *fmt, ...) CURL_PRINTF(2, 3);

#define failf Curl_failf

/* In case failf() reported into the errorbuf, clear it again.
 * This is used to clear information from happy eyeballing attempts
 * when ultimately a successful attempt was made. */
શૂન્ય Curl_reset_fail(સંરચના Curl_easy *data);

#define CURL_LOG_LVL_NONE  0
#define CURL_LOG_LVL_INFO  1

/**
 * Output an informational message when transfer's verbose logging is enabled.
 */
શૂન્ય Curl_infof(સંરચના Curl_easy *data,
                અચલ અક્ષર *fmt, ...) CURL_PRINTF(2, 3);

/**
 * Output an informational message when both transfer's verbose logging
 * and connection filters verbose logging are enabled.
 */
શૂન્ય Curl_trc_cf_infof(સંરચના Curl_easy *data, અચલ સંરચના Curl_cfilter *cf,
                       અચલ અક્ષર *fmt, ...) CURL_PRINTF(3, 4);
શૂન્ય Curl_trc_multi(સંરચના Curl_easy *data,
                    અચલ અક્ષર *fmt, ...) CURL_PRINTF(2, 3);
અચલ અક્ષર *Curl_trc_mstate_name(પૂર્ણાંક state);
અચલ અક્ષર *Curl_trc_timer_name(પૂર્ણાંક tid);
શૂન્ય Curl_trc_easy_timers(સંરચના Curl_easy *data);

શૂન્ય Curl_trc_write(સંરચના Curl_easy *data,
                    અચલ અક્ષર *fmt, ...) CURL_PRINTF(2, 3);
શૂન્ય Curl_trc_read(સંરચના Curl_easy *data,
                   અચલ અક્ષર *fmt, ...) CURL_PRINTF(2, 3);
શૂન્ય Curl_trc_dns(સંરચના Curl_easy *data,
                  અચલ અક્ષર *fmt, ...) CURL_PRINTF(2, 3);
શૂન્ય Curl_trc_timer(સંરચના Curl_easy *data, પૂર્ણાંક tid,
                    અચલ અક્ષર *fmt, ...) CURL_PRINTF(3, 4);

સંરચના curl_trc_feat {
  અચલ અક્ષર *name;
  પૂર્ણાંક log_level;
};

શૂન્ય Curl_trc_feat_infof(સંરચના Curl_easy *data,
                         સંરચના curl_trc_feat *feat,
                         અચલ અક્ષર *fmt, ...) CURL_PRINTF(3, 4);

#ifndef CURL_DISABLE_FTP
બાહ્ય સંરચના curl_trc_feat Curl_trc_feat_ftp;
શૂન્ય Curl_trc_ftp(સંરચના Curl_easy *data,
                  અચલ અક્ષર *fmt, ...) CURL_PRINTF(2, 3);
#endif
#ifndef CURL_DISABLE_SMTP
બાહ્ય સંરચના curl_trc_feat Curl_trc_feat_smtp;
શૂન્ય Curl_trc_smtp(સંરચના Curl_easy *data,
                   અચલ અક્ષર *fmt, ...) CURL_PRINTF(2, 3);
#endif
#ifdef USE_SSL
બાહ્ય સંરચના curl_trc_feat Curl_trc_feat_ssls;
શૂન્ય Curl_trc_ssls(સંરચના Curl_easy *data,
                   અચલ અક્ષર *fmt, ...) CURL_PRINTF(2, 3);
#endif
#ifdef USE_SSH
બાહ્ય સંરચના curl_trc_feat Curl_trc_feat_ssh;
શૂન્ય Curl_trc_ssh(સંરચના Curl_easy *data,
                  અચલ અક્ષર *fmt, ...) CURL_PRINTF(2, 3);
#endif
#જો !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
બાહ્ય સંરચના curl_trc_feat Curl_trc_feat_ws;
શૂન્ય Curl_trc_ws(સંરચના Curl_easy *data,
                 અચલ અક્ષર *fmt, ...) CURL_PRINTF(2, 3);
#endif

#define CURL_TRC_M_is_verbose(data) \
  Curl_trc_ft_is_verbose(data, &Curl_trc_feat_multi)
#define CURL_TRC_DNS_is_verbose(data) \
  Curl_trc_ft_is_verbose(data, &Curl_trc_feat_dns)
#define CURL_TRC_TIMER_is_verbose(data) \
  Curl_trc_ft_is_verbose(data, &Curl_trc_feat_timer)

#જો defined(CURL_HAVE_MACRO_VARARG) && !defined(CURL_DISABLE_VERBOSE_STRINGS)
#define infof(data, ...)             \
  કરો {                               \
    જો(Curl_trc_is_verbose(data))    \
      Curl_infof(data, __VA_ARGS__); \
  } જ્યારે(0)
#define CURL_TRC_M(data, ...)            \
  કરો {                                   \
    જો(CURL_TRC_M_is_verbose(data))      \
      Curl_trc_multi(data, __VA_ARGS__); \
  } જ્યારે(0)
#define CURL_TRC_CF(data, cf, ...)              \
  કરો {                                          \
    જો(Curl_trc_cf_is_verbose(cf, data))        \
      Curl_trc_cf_infof(data, cf, __VA_ARGS__); \
  } જ્યારે(0)
#define CURL_TRC_WRITE(data, ...)                          \
  કરો {                                                     \
    જો(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_write)) \
      Curl_trc_write(data, __VA_ARGS__);                   \
  } જ્યારે(0)
#define CURL_TRC_READ(data, ...)                          \
  કરો {                                                    \
    જો(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_read)) \
      Curl_trc_read(data, __VA_ARGS__);                   \
  } જ્યારે(0)
#define CURL_TRC_DNS(data, ...)        \
  કરો {                                 \
    જો(CURL_TRC_DNS_is_verbose(data))  \
      Curl_trc_dns(data, __VA_ARGS__); \
  } જ્યારે(0)
#define CURL_TRC_TIMER(data, tid, ...)        \
  કરો {                                        \
    જો(CURL_TRC_TIMER_is_verbose(data))       \
      Curl_trc_timer(data, tid, __VA_ARGS__); \
  } જ્યારે(0)

#ifndef CURL_DISABLE_FTP
#define CURL_TRC_FTP(data, ...)                          \
  કરો {                                                   \
    જો(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ftp)) \
      Curl_trc_ftp(data, __VA_ARGS__);                   \
  } જ્યારે(0)
#endif /* !CURL_DISABLE_FTP */
#ifndef CURL_DISABLE_SMTP
#define CURL_TRC_SMTP(data, ...)                          \
  કરો {                                                    \
    જો(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_smtp)) \
      Curl_trc_smtp(data, __VA_ARGS__);                   \
  } જ્યારે(0)
#endif /* !CURL_DISABLE_SMTP */
#ifdef USE_SSL
#define CURL_TRC_SSLS(data, ...)                          \
  કરો {                                                    \
    જો(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssls)) \
      Curl_trc_ssls(data, __VA_ARGS__);                   \
  } જ્યારે(0)
#endif /* USE_SSL */
#ifdef USE_SSH
#define CURL_TRC_SSH(data, ...)                          \
  કરો {                                                   \
    જો(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssh)) \
      Curl_trc_ssh(data, __VA_ARGS__);                   \
  } જ્યારે(0)
#endif /* USE_SSH */
#જો !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#define CURL_TRC_WS(data, ...)                          \
  કરો {                                                  \
    જો(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ws)) \
      Curl_trc_ws(data, __VA_ARGS__);                   \
  } જ્યારે(0)
#endif /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#અથવાજો defined(CURL_HAVE_MACRO_VARARG) && defined(CURL_DISABLE_VERBOSE_STRINGS)

#define infof(data, ...) \
  કરો {                   \
    (શૂન્ય)(data);        \
  } જ્યારે(0)
#define CURL_TRC_M(data, ...) \
  કરો {                        \
    (શૂન્ય)(data);             \
  } જ્યારે(0)
#define CURL_TRC_CF(data, cf, ...) \
  કરો {                             \
    (શૂન્ય)(data);                  \
    (શૂન્ય)(cf);                    \
  } જ્યારે(0)
#define CURL_TRC_WRITE(data, ...) \
  કરો {                            \
    (શૂન્ય)(data);                 \
  } જ્યારે(0)
#define CURL_TRC_READ(data, ...) \
  કરો {                           \
    (શૂન્ય)(data);                \
  } જ્યારે(0)
#define CURL_TRC_DNS(data, ...) \
  કરો {                          \
    (શૂન્ય)(data);               \
  } જ્યારે(0)
#define CURL_TRC_TIMER(data, tid, ...) \
  કરો {                                 \
    (શૂન્ય)(data);                      \
    (શૂન્ય)(tid);                       \
  } જ્યારે(0)
#ifndef CURL_DISABLE_FTP
#define CURL_TRC_FTP(data, ...) \
  કરો {                          \
    (શૂન્ય)(data);               \
  } જ્યારે(0)
#endif /* !CURL_DISABLE_FTP */
#ifndef CURL_DISABLE_SMTP
#define CURL_TRC_SMTP(data, ...) \
  કરો {                           \
    (શૂન્ય)(data);                \
  } જ્યારે(0)
#endif /* !CURL_DISABLE_SMTP */
#ifdef USE_SSL
#define CURL_TRC_SSLS(data, ...) \
  કરો {                           \
    (શૂન્ય)(data);                \
  } જ્યારે(0)
#endif /* USE_SSL */
#ifdef USE_SSH
#define CURL_TRC_SSH(data, ...) \
  કરો {                          \
    (શૂન્ય)(data);               \
  } જ્યારે(0)
#endif /* USE_SSH */
#જો !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#define CURL_TRC_WS(data, ...) \
  કરો {                         \
    (શૂન્ય)(data);              \
  } જ્યારે(0)
#endif

#નહીંતર /* !CURL_HAVE_MACRO_VARARG */

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
#જો !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#define CURL_TRC_WS    Curl_trc_ws
#endif

#endif /* CURL_HAVE_MACRO_VARARG */

#ifdef CURLVERBOSE
બાહ્ય સંરચના curl_trc_feat Curl_trc_feat_multi;
બાહ્ય સંરચના curl_trc_feat Curl_trc_feat_read;
બાહ્ય સંરચના curl_trc_feat Curl_trc_feat_write;
બાહ્ય સંરચના curl_trc_feat Curl_trc_feat_dns;
#ifndef CURL_DISABLE_DOH
બાહ્ય સંરચના curl_trc_feat Curl_trc_feat_doh;
#endif
બાહ્ય સંરચના curl_trc_feat Curl_trc_feat_timer;
#ifdef USE_THREADS
બાહ્ય સંરચના curl_trc_feat Curl_trc_feat_threads;
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
#define CURL_MSTATE_NAME(s)  Curl_trc_mstate_name((પૂર્ણાંક)(s))
#define CURL_TRC_EASY_TIMERS(data)      \
  કરો {                                  \
    જો(CURL_TRC_TIMER_is_verbose(data)) \
      Curl_trc_easy_timers(data);       \
  } જ્યારે(0)

#નહીંતર /* CURL_DISABLE_VERBOSE_STRINGS */
/* All informational messages are not compiled in for size savings */
#define Curl_trc_is_verbose(d)        FALSE
#define Curl_trc_cf_is_verbose(x, y)  FALSE
#define Curl_trc_ft_is_verbose(x, y)  FALSE
#define CURL_MSTATE_NAME(x)           ((શૂન્ય)(x), "-")
#define CURL_TRC_EASY_TIMERS(x)       Curl_nop_stmt
#endif /* !CURL_DISABLE_VERBOSE_STRINGS */

#endif /* HEADER_CURL_TRC_H */
