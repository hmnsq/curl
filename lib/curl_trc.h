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
संरचना Curl_easy;
संरचना Curl_cfilter;

/**
 * Init logging, return != 0 on failure.
 */
CURLcode Curl_trc_init(शून्य);

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
CURLcode Curl_trc_opt(अचर वर्ण *config);

/* the function used to output verbose information */
शून्य Curl_debug(संरचना Curl_easy *data, curl_infotype type,
                अचर वर्ण *ptr, माप_प्रकार size);

/**
 * Output a failure message on registered callbacks for transfer.
 */
शून्य Curl_failf(संरचना Curl_easy *data,
                अचर वर्ण *fmt, ...) CURL_PRINTF(2, 3);

#define failf Curl_failf

/* In case failf() reported into the errorbuf, clear it again.
 * This is used to clear information from happy eyeballing attempts
 * when ultimately a successful attempt was made. */
शून्य Curl_reset_fail(संरचना Curl_easy *data);

#define CURL_LOG_LVL_NONE  0
#define CURL_LOG_LVL_INFO  1

/**
 * Output an informational message when transfer's verbose logging is enabled.
 */
शून्य Curl_infof(संरचना Curl_easy *data,
                अचर वर्ण *fmt, ...) CURL_PRINTF(2, 3);

/**
 * Output an informational message when both transfer's verbose logging
 * and connection filters verbose logging are enabled.
 */
शून्य Curl_trc_cf_infof(संरचना Curl_easy *data, अचर संरचना Curl_cfilter *cf,
                       अचर वर्ण *fmt, ...) CURL_PRINTF(3, 4);
शून्य Curl_trc_multi(संरचना Curl_easy *data,
                    अचर वर्ण *fmt, ...) CURL_PRINTF(2, 3);
अचर वर्ण *Curl_trc_mstate_name(पूर्णाङ्क state);
अचर वर्ण *Curl_trc_timer_name(पूर्णाङ्क tid);
शून्य Curl_trc_easy_timers(संरचना Curl_easy *data);

शून्य Curl_trc_write(संरचना Curl_easy *data,
                    अचर वर्ण *fmt, ...) CURL_PRINTF(2, 3);
शून्य Curl_trc_read(संरचना Curl_easy *data,
                   अचर वर्ण *fmt, ...) CURL_PRINTF(2, 3);
शून्य Curl_trc_dns(संरचना Curl_easy *data,
                  अचर वर्ण *fmt, ...) CURL_PRINTF(2, 3);
शून्य Curl_trc_timer(संरचना Curl_easy *data, पूर्णाङ्क tid,
                    अचर वर्ण *fmt, ...) CURL_PRINTF(3, 4);

संरचना curl_trc_feat {
  अचर वर्ण *name;
  पूर्णाङ्क log_level;
};

शून्य Curl_trc_feat_infof(संरचना Curl_easy *data,
                         संरचना curl_trc_feat *feat,
                         अचर वर्ण *fmt, ...) CURL_PRINTF(3, 4);

#ifndef CURL_DISABLE_FTP
बाह्य संरचना curl_trc_feat Curl_trc_feat_ftp;
शून्य Curl_trc_ftp(संरचना Curl_easy *data,
                  अचर वर्ण *fmt, ...) CURL_PRINTF(2, 3);
#endif
#ifndef CURL_DISABLE_SMTP
बाह्य संरचना curl_trc_feat Curl_trc_feat_smtp;
शून्य Curl_trc_smtp(संरचना Curl_easy *data,
                   अचर वर्ण *fmt, ...) CURL_PRINTF(2, 3);
#endif
#ifdef USE_SSL
बाह्य संरचना curl_trc_feat Curl_trc_feat_ssls;
शून्य Curl_trc_ssls(संरचना Curl_easy *data,
                   अचर वर्ण *fmt, ...) CURL_PRINTF(2, 3);
#endif
#ifdef USE_SSH
बाह्य संरचना curl_trc_feat Curl_trc_feat_ssh;
शून्य Curl_trc_ssh(संरचना Curl_easy *data,
                  अचर वर्ण *fmt, ...) CURL_PRINTF(2, 3);
#endif
#यदि !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
बाह्य संरचना curl_trc_feat Curl_trc_feat_ws;
शून्य Curl_trc_ws(संरचना Curl_easy *data,
                 अचर वर्ण *fmt, ...) CURL_PRINTF(2, 3);
#endif

#define CURL_TRC_M_is_verbose(data) \
  Curl_trc_ft_is_verbose(data, &Curl_trc_feat_multi)
#define CURL_TRC_DNS_is_verbose(data) \
  Curl_trc_ft_is_verbose(data, &Curl_trc_feat_dns)
#define CURL_TRC_TIMER_is_verbose(data) \
  Curl_trc_ft_is_verbose(data, &Curl_trc_feat_timer)

#यदि defined(CURL_HAVE_MACRO_VARARG) && !defined(CURL_DISABLE_VERBOSE_STRINGS)
#define infof(data, ...)             \
  गर {                               \
    यदि(Curl_trc_is_verbose(data))    \
      Curl_infof(data, __VA_ARGS__); \
  } जबसम्म(0)
#define CURL_TRC_M(data, ...)            \
  गर {                                   \
    यदि(CURL_TRC_M_is_verbose(data))      \
      Curl_trc_multi(data, __VA_ARGS__); \
  } जबसम्म(0)
#define CURL_TRC_CF(data, cf, ...)              \
  गर {                                          \
    यदि(Curl_trc_cf_is_verbose(cf, data))        \
      Curl_trc_cf_infof(data, cf, __VA_ARGS__); \
  } जबसम्म(0)
#define CURL_TRC_WRITE(data, ...)                          \
  गर {                                                     \
    यदि(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_write)) \
      Curl_trc_write(data, __VA_ARGS__);                   \
  } जबसम्म(0)
#define CURL_TRC_READ(data, ...)                          \
  गर {                                                    \
    यदि(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_read)) \
      Curl_trc_read(data, __VA_ARGS__);                   \
  } जबसम्म(0)
#define CURL_TRC_DNS(data, ...)        \
  गर {                                 \
    यदि(CURL_TRC_DNS_is_verbose(data))  \
      Curl_trc_dns(data, __VA_ARGS__); \
  } जबसम्म(0)
#define CURL_TRC_TIMER(data, tid, ...)        \
  गर {                                        \
    यदि(CURL_TRC_TIMER_is_verbose(data))       \
      Curl_trc_timer(data, tid, __VA_ARGS__); \
  } जबसम्म(0)

#ifndef CURL_DISABLE_FTP
#define CURL_TRC_FTP(data, ...)                          \
  गर {                                                   \
    यदि(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ftp)) \
      Curl_trc_ftp(data, __VA_ARGS__);                   \
  } जबसम्म(0)
#endif /* !CURL_DISABLE_FTP */
#ifndef CURL_DISABLE_SMTP
#define CURL_TRC_SMTP(data, ...)                          \
  गर {                                                    \
    यदि(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_smtp)) \
      Curl_trc_smtp(data, __VA_ARGS__);                   \
  } जबसम्म(0)
#endif /* !CURL_DISABLE_SMTP */
#ifdef USE_SSL
#define CURL_TRC_SSLS(data, ...)                          \
  गर {                                                    \
    यदि(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssls)) \
      Curl_trc_ssls(data, __VA_ARGS__);                   \
  } जबसम्म(0)
#endif /* USE_SSL */
#ifdef USE_SSH
#define CURL_TRC_SSH(data, ...)                          \
  गर {                                                   \
    यदि(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ssh)) \
      Curl_trc_ssh(data, __VA_ARGS__);                   \
  } जबसम्म(0)
#endif /* USE_SSH */
#यदि !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#define CURL_TRC_WS(data, ...)                          \
  गर {                                                  \
    यदि(Curl_trc_ft_is_verbose(data, &Curl_trc_feat_ws)) \
      Curl_trc_ws(data, __VA_ARGS__);                   \
  } जबसम्म(0)
#endif /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#अथवायदि defined(CURL_HAVE_MACRO_VARARG) && defined(CURL_DISABLE_VERBOSE_STRINGS)

#define infof(data, ...) \
  गर {                   \
    (शून्य)(data);        \
  } जबसम्म(0)
#define CURL_TRC_M(data, ...) \
  गर {                        \
    (शून्य)(data);             \
  } जबसम्म(0)
#define CURL_TRC_CF(data, cf, ...) \
  गर {                             \
    (शून्य)(data);                  \
    (शून्य)(cf);                    \
  } जबसम्म(0)
#define CURL_TRC_WRITE(data, ...) \
  गर {                            \
    (शून्य)(data);                 \
  } जबसम्म(0)
#define CURL_TRC_READ(data, ...) \
  गर {                           \
    (शून्य)(data);                \
  } जबसम्म(0)
#define CURL_TRC_DNS(data, ...) \
  गर {                          \
    (शून्य)(data);               \
  } जबसम्म(0)
#define CURL_TRC_TIMER(data, tid, ...) \
  गर {                                 \
    (शून्य)(data);                      \
    (शून्य)(tid);                       \
  } जबसम्म(0)
#ifndef CURL_DISABLE_FTP
#define CURL_TRC_FTP(data, ...) \
  गर {                          \
    (शून्य)(data);               \
  } जबसम्म(0)
#endif /* !CURL_DISABLE_FTP */
#ifndef CURL_DISABLE_SMTP
#define CURL_TRC_SMTP(data, ...) \
  गर {                           \
    (शून्य)(data);                \
  } जबसम्म(0)
#endif /* !CURL_DISABLE_SMTP */
#ifdef USE_SSL
#define CURL_TRC_SSLS(data, ...) \
  गर {                           \
    (शून्य)(data);                \
  } जबसम्म(0)
#endif /* USE_SSL */
#ifdef USE_SSH
#define CURL_TRC_SSH(data, ...) \
  गर {                          \
    (शून्य)(data);               \
  } जबसम्म(0)
#endif /* USE_SSH */
#यदि !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#define CURL_TRC_WS(data, ...) \
  गर {                         \
    (शून्य)(data);              \
  } जबसम्म(0)
#endif

#अन्यथा /* !CURL_HAVE_MACRO_VARARG */

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
#यदि !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#define CURL_TRC_WS    Curl_trc_ws
#endif

#endif /* CURL_HAVE_MACRO_VARARG */

#ifdef CURLVERBOSE
बाह्य संरचना curl_trc_feat Curl_trc_feat_multi;
बाह्य संरचना curl_trc_feat Curl_trc_feat_read;
बाह्य संरचना curl_trc_feat Curl_trc_feat_write;
बाह्य संरचना curl_trc_feat Curl_trc_feat_dns;
#ifndef CURL_DISABLE_DOH
बाह्य संरचना curl_trc_feat Curl_trc_feat_doh;
#endif
बाह्य संरचना curl_trc_feat Curl_trc_feat_timer;
#ifdef USE_THREADS
बाह्य संरचना curl_trc_feat Curl_trc_feat_threads;
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
#define CURL_MSTATE_NAME(s)  Curl_trc_mstate_name((पूर्णाङ्क)(s))
#define CURL_TRC_EASY_TIMERS(data)      \
  गर {                                  \
    यदि(CURL_TRC_TIMER_is_verbose(data)) \
      Curl_trc_easy_timers(data);       \
  } जबसम्म(0)

#अन्यथा /* CURL_DISABLE_VERBOSE_STRINGS */
/* All informational messages are not compiled in for size savings */
#define Curl_trc_is_verbose(d)        FALSE
#define Curl_trc_cf_is_verbose(x, y)  FALSE
#define Curl_trc_ft_is_verbose(x, y)  FALSE
#define CURL_MSTATE_NAME(x)           ((शून्य)(x), "-")
#define CURL_TRC_EASY_TIMERS(x)       Curl_nop_stmt
#endif /* !CURL_DISABLE_VERBOSE_STRINGS */

#endif /* HEADER_CURL_TRC_H */
