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
ସଂରଚନା Curl_easy;
ସଂରଚନା Curl_cfilter;

/**
 * Init logging, return != 0 on failure.
 */
CURLcode Curl_trc_init(ଶୂନ୍ୟ);

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
CURLcode Curl_trc_opt(ଅଚଳ ଅକ୍ଷର *config);

/* the function used to output verbose information */
ଶୂନ୍ୟ Curl_debug(ସଂରଚନା Curl_easy *data, curl_infotype type,
                ଅଚଳ ଅକ୍ଷର *ptr, size_t size);

/**
 * Output a failure message on registered callbacks for transfer.
 */
ଶୂନ୍ୟ Curl_failf(ସଂରଚନା Curl_easy *data,
                ଅଚଳ ଅକ୍ଷର *fmt, ...) CURL_PRINTF(2, 3);

#define failf Curl_failf

/* In case failf() reported into the errorbuf, clear it again.
 * This is used to clear information from happy eyeballing attempts
 * when ultimately a successful attempt was made. */
ଶୂନ୍ୟ Curl_reset_fail(ସଂରଚନା Curl_easy *data);

#define CURL_LOG_LVL_NONE  0
#define CURL_LOG_LVL_INFO  1

/**
 * Output an informational message when transfer's verbose logging is enabled.
 */
ଶୂନ୍ୟ Curl_infof(ସଂରଚନା Curl_easy *data,
                ଅଚଳ ଅକ୍ଷର *fmt, ...) CURL_PRINTF(2, 3);

/**
 * Output an informational message when both transfer's verbose logging
 * and connection filters verbose logging are enabled.
 */
ଶୂନ୍ୟ Curl_trc_cf_infof(ସଂରଚନା Curl_easy *data, ଅଚଳ ସଂରଚନା Curl_cfilter *cf,
                       ଅଚଳ ଅକ୍ଷର *fmt, ...) CURL_PRINTF(3, 4);
ଶୂନ୍ୟ Curl_trc_multi(ସଂରଚନା Curl_easy *data,
                    ଅଚଳ ଅକ୍ଷର *fmt, ...) CURL_PRINTF(2, 3);
ଅଚଳ ଅକ୍ଷର *Curl_trc_mstate_name(ପୂର୍ଣ୍ଣସଂଖ୍ୟା state);
ଅଚଳ ଅକ୍ଷର *Curl_trc_timer_name(ପୂର୍ଣ୍ଣସଂଖ୍ୟା tid);
ଶୂନ୍ୟ Curl_trc_easy_timers(ସଂରଚନା Curl_easy *data);

ଶୂନ୍ୟ Curl_trc_write(ସଂରଚନା Curl_easy *data,
                    ଅଚଳ ଅକ୍ଷର *fmt, ...) CURL_PRINTF(2, 3);
ଶୂନ୍ୟ Curl_trc_read(ସଂରଚନା Curl_easy *data,
                   ଅଚଳ ଅକ୍ଷର *fmt, ...) CURL_PRINTF(2, 3);
ଶୂନ୍ୟ Curl_trc_dns(ସଂରଚନା Curl_easy *data,
                  ଅଚଳ ଅକ୍ଷର *fmt, ...) CURL_PRINTF(2, 3);
ଶୂନ୍ୟ Curl_trc_timer(ସଂରଚନା Curl_easy *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା tid,
                    ଅଚଳ ଅକ୍ଷର *fmt, ...) CURL_PRINTF(3, 4);

ସଂରଚନା curl_trc_feat {
  ଅଚଳ ଅକ୍ଷର *name;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level;
};

ଶୂନ୍ୟ Curl_trc_feat_infof(ସଂରଚନା Curl_easy *data,
                         ସଂରଚନା curl_trc_feat *feat,
                         ଅଚଳ ଅକ୍ଷର *fmt, ...) CURL_PRINTF(3, 4);

#ifndef CURL_DISABLE_FTP
ବାହ୍ୟ ସଂରଚନା curl_trc_feat Curl_trc_feat_ftp;
ଶୂନ୍ୟ Curl_trc_ftp(ସଂରଚନା Curl_easy *data,
                  ଅଚଳ ଅକ୍ଷର *fmt, ...) CURL_PRINTF(2, 3);
#endif
#ifndef CURL_DISABLE_SMTP
ବାହ୍ୟ ସଂରଚନା curl_trc_feat Curl_trc_feat_smtp;
ଶୂନ୍ୟ Curl_trc_smtp(ସଂରଚନା Curl_easy *data,
                   ଅଚଳ ଅକ୍ଷର *fmt, ...) CURL_PRINTF(2, 3);
#endif
#ifdef USE_SSL
ବାହ୍ୟ ସଂରଚନା curl_trc_feat Curl_trc_feat_ssls;
ଶୂନ୍ୟ Curl_trc_ssls(ସଂରଚନା Curl_easy *data,
                   ଅଚଳ ଅକ୍ଷର *fmt, ...) CURL_PRINTF(2, 3);
#endif
#ifdef USE_SSH
ବାହ୍ୟ ସଂରଚନା curl_trc_feat Curl_trc_feat_ssh;
ଶୂନ୍ୟ Curl_trc_ssh(ସଂରଚନା Curl_easy *data,
                  ଅଚଳ ଅକ୍ଷର *fmt, ...) CURL_PRINTF(2, 3);
#endif
#ଯଦି !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
ବାହ୍ୟ ସଂରଚନା curl_trc_feat Curl_trc_feat_ws;
ଶୂନ୍ୟ Curl_trc_ws(ସଂରଚନା Curl_easy *data,
                 ଅଚଳ ଅକ୍ଷର *fmt, ...) CURL_PRINTF(2, 3);
#endif

#define CURL_TRC_M_is_verbose(data) \
  Curl_trc_ft_is_verbose(data, &Curl_trc_feat_multi)
#define CURL_TRC_DNS_is_verbose(data) \
  Curl_trc_ft_is_verbose(data, &Curl_trc_feat_dns)
#define CURL_TRC_TIMER_is_verbose(data) \
  Curl_trc_ft_is_verbose(data, &Curl_trc_feat_timer)

#ଯଦି defined(CURL_HAVE_MACRO_VARARG) && !defined(CURL_DISABLE_VERBOSE_STRINGS)
#define infof(data, ...)             \
  କର {                               \
    ଯଦି(Curl_trc_is_verbose(data))    \
      Curl_infof(data, __VA_ARGS__); \
  } ଯେତେବେଳେ(0)
#define CURL_TRC_M(data, ...)            \
  କର {                                   \
    ଯଦି(CURL_TRC_M_is_verbose(data))      \
      Curl_trc_multi(data, __VA_ARGS__); \
  } ଯେତେବେଳେ(0)
#define CURL_TRC_CF(data, cf, ...)              \
  କର {                                          \
    ଯଦି(Curl_trc_cf_is_verbose(cf, data))        \
      Curl_trc_cf_infof(data, cf, __VA_ARGS__); \
  } ଯେତେବେଳେ(0)
#define CURL_TRC_WRITE(data, ...)                          \
  କର {                                                     \
    ଯଦି(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_write)) \
      Curl_trc_write(data, __VA_ARGS__);                   \
  } ଯେତେବେଳେ(0)
#define CURL_TRC_READ(data, ...)                          \
  କର {                                                    \
    ଯଦି(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_read)) \
      Curl_trc_read(data, __VA_ARGS__);                   \
  } ଯେତେବେଳେ(0)
#define CURL_TRC_DNS(data, ...)        \
  କର {                                 \
    ଯଦି(CURL_TRC_DNS_is_verbose(data))  \
      Curl_trc_dns(data, __VA_ARGS__); \
  } ଯେତେବେଳେ(0)
#define CURL_TRC_TIMER(data, tid, ...)        \
  କର {                                        \
    ଯଦି(CURL_TRC_TIMER_is_verbose(data))       \
      Curl_trc_timer(data, tid, __VA_ARGS__); \
  } ଯେତେବେଳେ(0)

#ifndef CURL_DISABLE_FTP
#define CURL_TRC_FTP(data, ...)                          \
  କର {                                                   \
    ଯଦି(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ftp)) \
      Curl_trc_ftp(data, __VA_ARGS__);                   \
  } ଯେତେବେଳେ(0)
#endif /* !CURL_DISABLE_FTP */
#ifndef CURL_DISABLE_SMTP
#define CURL_TRC_SMTP(data, ...)                          \
  କର {                                                    \
    ଯଦି(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_smtp)) \
      Curl_trc_smtp(data, __VA_ARGS__);                   \
  } ଯେତେବେଳେ(0)
#endif /* !CURL_DISABLE_SMTP */
#ifdef USE_SSL
#define CURL_TRC_SSLS(data, ...)                          \
  କର {                                                    \
    ଯଦି(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssls)) \
      Curl_trc_ssls(data, __VA_ARGS__);                   \
  } ଯେତେବେଳେ(0)
#endif /* USE_SSL */
#ifdef USE_SSH
#define CURL_TRC_SSH(data, ...)                          \
  କର {                                                   \
    ଯଦି(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssh)) \
      Curl_trc_ssh(data, __VA_ARGS__);                   \
  } ଯେତେବେଳେ(0)
#endif /* USE_SSH */
#ଯଦି !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#define CURL_TRC_WS(data, ...)                          \
  କର {                                                  \
    ଯଦି(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ws)) \
      Curl_trc_ws(data, __VA_ARGS__);                   \
  } ଯେତେବେଳେ(0)
#endif /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#ନଚେତଯଦି defined(CURL_HAVE_MACRO_VARARG) && defined(CURL_DISABLE_VERBOSE_STRINGS)

#define infof(data, ...) \
  କର {                   \
    (ଶୂନ୍ୟ)(data);        \
  } ଯେତେବେଳେ(0)
#define CURL_TRC_M(data, ...) \
  କର {                        \
    (ଶୂନ୍ୟ)(data);             \
  } ଯେତେବେଳେ(0)
#define CURL_TRC_CF(data, cf, ...) \
  କର {                             \
    (ଶୂନ୍ୟ)(data);                  \
    (ଶୂନ୍ୟ)(cf);                    \
  } ଯେତେବେଳେ(0)
#define CURL_TRC_WRITE(data, ...) \
  କର {                            \
    (ଶୂନ୍ୟ)(data);                 \
  } ଯେତେବେଳେ(0)
#define CURL_TRC_READ(data, ...) \
  କର {                           \
    (ଶୂନ୍ୟ)(data);                \
  } ଯେତେବେଳେ(0)
#define CURL_TRC_DNS(data, ...) \
  କର {                          \
    (ଶୂନ୍ୟ)(data);               \
  } ଯେତେବେଳେ(0)
#define CURL_TRC_TIMER(data, tid, ...) \
  କର {                                 \
    (ଶୂନ୍ୟ)(data);                      \
    (ଶୂନ୍ୟ)(tid);                       \
  } ଯେତେବେଳେ(0)
#ifndef CURL_DISABLE_FTP
#define CURL_TRC_FTP(data, ...) \
  କର {                          \
    (ଶୂନ୍ୟ)(data);               \
  } ଯେତେବେଳେ(0)
#endif /* !CURL_DISABLE_FTP */
#ifndef CURL_DISABLE_SMTP
#define CURL_TRC_SMTP(data, ...) \
  କର {                           \
    (ଶୂନ୍ୟ)(data);                \
  } ଯେତେବେଳେ(0)
#endif /* !CURL_DISABLE_SMTP */
#ifdef USE_SSL
#define CURL_TRC_SSLS(data, ...) \
  କର {                           \
    (ଶୂନ୍ୟ)(data);                \
  } ଯେତେବେଳେ(0)
#endif /* USE_SSL */
#ifdef USE_SSH
#define CURL_TRC_SSH(data, ...) \
  କର {                          \
    (ଶୂନ୍ୟ)(data);               \
  } ଯେତେବେଳେ(0)
#endif /* USE_SSH */
#ଯଦି !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#define CURL_TRC_WS(data, ...) \
  କର {                         \
    (ଶୂନ୍ୟ)(data);              \
  } ଯେତେବେଳେ(0)
#endif

#ଅନ୍ୟଥା /* !CURL_HAVE_MACRO_VARARG */

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
#ଯଦି !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#define CURL_TRC_WS    Curl_trc_ws
#endif

#endif /* CURL_HAVE_MACRO_VARARG */

#ifdef CURLVERBOSE
ବାହ୍ୟ ସଂରଚନା curl_trc_feat Curl_trc_feat_multi;
ବାହ୍ୟ ସଂରଚନା curl_trc_feat Curl_trc_feat_read;
ବାହ୍ୟ ସଂରଚନା curl_trc_feat Curl_trc_feat_write;
ବାହ୍ୟ ସଂରଚନା curl_trc_feat Curl_trc_feat_dns;
#ifndef CURL_DISABLE_DOH
ବାହ୍ୟ ସଂରଚନା curl_trc_feat Curl_trc_feat_doh;
#endif
ବାହ୍ୟ ସଂରଚନା curl_trc_feat Curl_trc_feat_timer;
#ifdef USE_THREADS
ବାହ୍ୟ ସଂରଚନା curl_trc_feat Curl_trc_feat_threads;
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
#define CURL_MSTATE_NAME(s)  Curl_trc_mstate_name((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(s))
#define CURL_TRC_EASY_TIMERS(data)      \
  କର {                                  \
    ଯଦି(CURL_TRC_TIMER_is_verbose(data)) \
      Curl_trc_easy_timers(data);       \
  } ଯେତେବେଳେ(0)

#ଅନ୍ୟଥା /* CURL_DISABLE_VERBOSE_STRINGS */
/* All informational messages are not compiled in for size savings */
#define Curl_trc_is_verbose(d)        FALSE
#define Curl_trc_cf_is_verbose(x, y)  FALSE
#define Curl_trc_ft_is_verbose(x, y)  FALSE
#define CURL_MSTATE_NAME(x)           ((ଶୂନ୍ୟ)(x), "-")
#define CURL_TRC_EASY_TIMERS(x)       Curl_nop_stmt
#endif /* !CURL_DISABLE_VERBOSE_STRINGS */

#endif /* HEADER_CURL_TRC_H */
