#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_TRC_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_TRC_H
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
ਬਣਤਰ Curl_easy;
ਬਣਤਰ Curl_cfilter;

/**
 * Init logging, return != 0 on failure.
 */
CURLcode Curl_trc_init(ਖਾਲੀ);

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
CURLcode Curl_trc_opt(ਅਚਲ ਅੱਖਰ *config);

/* the function used to output verbose information */
ਖਾਲੀ Curl_debug(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, curl_infotype type,
                ਅਚਲ ਅੱਖਰ *ptr, size_t ਮਾਪ);

/**
 * Output a failure message on registered callbacks for transfer.
 */
ਖਾਲੀ Curl_failf(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                ਅਚਲ ਅੱਖਰ *fmt, ...) CURL_PRINTF(2, 3);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ failf Curl_failf

/* In case failf() reported into the errorbuf, clear it again.
 * This is used to clear information from happy eyeballing attempts
 * when ultimately a successful attempt was made. */
ਖਾਲੀ Curl_reset_fail(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_LOG_LVL_NONE  0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_LOG_LVL_INFO  1

/**
 * Output an informational message when transfer's verbose logging is enabled.
 */
ਖਾਲੀ Curl_infof(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                ਅਚਲ ਅੱਖਰ *fmt, ...) CURL_PRINTF(2, 3);

/**
 * Output an informational message when both transfer's verbose logging
 * and connection filters verbose logging are enabled.
 */
ਖਾਲੀ Curl_trc_cf_infof(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਬਣਤਰ Curl_cfilter *cf,
                       ਅਚਲ ਅੱਖਰ *fmt, ...) CURL_PRINTF(3, 4);
ਖਾਲੀ Curl_trc_multi(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                    ਅਚਲ ਅੱਖਰ *fmt, ...) CURL_PRINTF(2, 3);
ਅਚਲ ਅੱਖਰ *Curl_trc_mstate_name(ਪੂਰਨ state);
ਅਚਲ ਅੱਖਰ *Curl_trc_timer_name(ਪੂਰਨ tid);
ਖਾਲੀ Curl_trc_easy_timers(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

ਖਾਲੀ Curl_trc_write(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                    ਅਚਲ ਅੱਖਰ *fmt, ...) CURL_PRINTF(2, 3);
ਖਾਲੀ Curl_trc_read(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                   ਅਚਲ ਅੱਖਰ *fmt, ...) CURL_PRINTF(2, 3);
ਖਾਲੀ Curl_trc_dns(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                  ਅਚਲ ਅੱਖਰ *fmt, ...) CURL_PRINTF(2, 3);
ਖਾਲੀ Curl_trc_timer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਪੂਰਨ tid,
                    ਅਚਲ ਅੱਖਰ *fmt, ...) CURL_PRINTF(3, 4);

ਬਣਤਰ curl_trc_feat {
  ਅਚਲ ਅੱਖਰ *name;
  ਪੂਰਨ log_level;
};

ਖਾਲੀ Curl_trc_feat_infof(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         ਬਣਤਰ curl_trc_feat *feat,
                         ਅਚਲ ਅੱਖਰ *fmt, ...) CURL_PRINTF(3, 4);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
ਬਾਹਰੀ ਬਣਤਰ curl_trc_feat Curl_trc_feat_ftp;
ਖਾਲੀ Curl_trc_ftp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                  ਅਚਲ ਅੱਖਰ *fmt, ...) CURL_PRINTF(2, 3);
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_SMTP
ਬਾਹਰੀ ਬਣਤਰ curl_trc_feat Curl_trc_feat_smtp;
ਖਾਲੀ Curl_trc_smtp(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                   ਅਚਲ ਅੱਖਰ *fmt, ...) CURL_PRINTF(2, 3);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
ਬਾਹਰੀ ਬਣਤਰ curl_trc_feat Curl_trc_feat_ssls;
ਖਾਲੀ Curl_trc_ssls(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                   ਅਚਲ ਅੱਖਰ *fmt, ...) CURL_PRINTF(2, 3);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSH
ਬਾਹਰੀ ਬਣਤਰ curl_trc_feat Curl_trc_feat_ssh;
ਖਾਲੀ Curl_trc_ssh(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                  ਅਚਲ ਅੱਖਰ *fmt, ...) CURL_PRINTF(2, 3);
#ਜੇ_ਅੰਤ
#ਜੇ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
ਬਾਹਰੀ ਬਣਤਰ curl_trc_feat Curl_trc_feat_ws;
ਖਾਲੀ Curl_trc_ws(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                 ਅਚਲ ਅੱਖਰ *fmt, ...) CURL_PRINTF(2, 3);
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_M_is_verbose(ਅੰਕੜੇ) \
  Curl_trc_ft_is_verbose(ਅੰਕੜੇ, &Curl_trc_feat_multi)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_DNS_is_verbose(ਅੰਕੜੇ) \
  Curl_trc_ft_is_verbose(ਅੰਕੜੇ, &Curl_trc_feat_dns)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_TIMER_is_verbose(ਅੰਕੜੇ) \
  Curl_trc_ft_is_verbose(ਅੰਕੜੇ, &Curl_trc_feat_timer)

#ਜੇ defined(CURL_HAVE_MACRO_VARARG) && !defined(CURL_DISABLE_VERBOSE_ਲੜੀਆਂ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ infof(ਅੰਕੜੇ, ...)             \
  ਕਰੋ {                               \
    ਜੇ(Curl_trc_is_verbose(ਅੰਕੜੇ))    \
      Curl_infof(ਅੰਕੜੇ, __VA_ARGS__); \
  } ਜਦੋਂ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_M(ਅੰਕੜੇ, ...)            \
  ਕਰੋ {                                   \
    ਜੇ(CURL_TRC_M_is_verbose(ਅੰਕੜੇ))      \
      Curl_trc_multi(ਅੰਕੜੇ, __VA_ARGS__); \
  } ਜਦੋਂ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_CF(ਅੰਕੜੇ, cf, ...)              \
  ਕਰੋ {                                          \
    ਜੇ(Curl_trc_cf_is_verbose(cf, ਅੰਕੜੇ))        \
      Curl_trc_cf_infof(ਅੰਕੜੇ, cf, __VA_ARGS__); \
  } ਜਦੋਂ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_WRITE(ਅੰਕੜੇ, ...)                          \
  ਕਰੋ {                                                     \
    ਜੇ(Curl_trc_ft_is_verbose(ਅੰਕੜੇ, &Curl_trc_feat_write)) \
      Curl_trc_write(ਅੰਕੜੇ, __VA_ARGS__);                   \
  } ਜਦੋਂ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_READ(ਅੰਕੜੇ, ...)                          \
  ਕਰੋ {                                                    \
    ਜੇ(Curl_trc_ft_is_verbose(ਅੰਕੜੇ, &Curl_trc_feat_read)) \
      Curl_trc_read(ਅੰਕੜੇ, __VA_ARGS__);                   \
  } ਜਦੋਂ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_DNS(ਅੰਕੜੇ, ...)        \
  ਕਰੋ {                                 \
    ਜੇ(CURL_TRC_DNS_is_verbose(ਅੰਕੜੇ))  \
      Curl_trc_dns(ਅੰਕੜੇ, __VA_ARGS__); \
  } ਜਦੋਂ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_TIMER(ਅੰਕੜੇ, tid, ...)        \
  ਕਰੋ {                                        \
    ਜੇ(CURL_TRC_TIMER_is_verbose(ਅੰਕੜੇ))       \
      Curl_trc_timer(ਅੰਕੜੇ, tid, __VA_ARGS__); \
  } ਜਦੋਂ(0)

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_FTP(ਅੰਕੜੇ, ...)                          \
  ਕਰੋ {                                                   \
    ਜੇ(Curl_trc_ft_is_verbose(ਅੰਕੜੇ, &Curl_trc_feat_ftp)) \
      Curl_trc_ftp(ਅੰਕੜੇ, __VA_ARGS__);                   \
  } ਜਦੋਂ(0)
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_FTP */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_SMTP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_SMTP(ਅੰਕੜੇ, ...)                          \
  ਕਰੋ {                                                    \
    ਜੇ(Curl_trc_ft_is_verbose(ਅੰਕੜੇ, &Curl_trc_feat_smtp)) \
      Curl_trc_smtp(ਅੰਕੜੇ, __VA_ARGS__);                   \
  } ਜਦੋਂ(0)
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_SMTP */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_SSLS(ਅੰਕੜੇ, ...)                          \
  ਕਰੋ {                                                    \
    ਜੇ(Curl_trc_ft_is_verbose(ਅੰਕੜੇ, &Curl_trc_feat_ssls)) \
      Curl_trc_ssls(ਅੰਕੜੇ, __VA_ARGS__);                   \
  } ਜਦੋਂ(0)
#ਜੇ_ਅੰਤ /* USE_SSL */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSH
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_SSH(ਅੰਕੜੇ, ...)                          \
  ਕਰੋ {                                                   \
    ਜੇ(Curl_trc_ft_is_verbose(ਅੰਕੜੇ, &Curl_trc_feat_ssh)) \
      Curl_trc_ssh(ਅੰਕੜੇ, __VA_ARGS__);                   \
  } ਜਦੋਂ(0)
#ਜੇ_ਅੰਤ /* USE_SSH */
#ਜੇ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_WS(ਅੰਕੜੇ, ...)                          \
  ਕਰੋ {                                                  \
    ਜੇ(Curl_trc_ft_is_verbose(ਅੰਕੜੇ, &Curl_trc_feat_ws)) \
      Curl_trc_ws(ਅੰਕੜੇ, __VA_ARGS__);                   \
  } ਜਦੋਂ(0)
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#ਜਾਂਜੇ defined(CURL_HAVE_MACRO_VARARG) && defined(CURL_DISABLE_VERBOSE_ਲੜੀਆਂ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ infof(ਅੰਕੜੇ, ...) \
  ਕਰੋ {                   \
    (ਖਾਲੀ)(ਅੰਕੜੇ);        \
  } ਜਦੋਂ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_M(ਅੰਕੜੇ, ...) \
  ਕਰੋ {                        \
    (ਖਾਲੀ)(ਅੰਕੜੇ);             \
  } ਜਦੋਂ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_CF(ਅੰਕੜੇ, cf, ...) \
  ਕਰੋ {                             \
    (ਖਾਲੀ)(ਅੰਕੜੇ);                  \
    (ਖਾਲੀ)(cf);                    \
  } ਜਦੋਂ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_WRITE(ਅੰਕੜੇ, ...) \
  ਕਰੋ {                            \
    (ਖਾਲੀ)(ਅੰਕੜੇ);                 \
  } ਜਦੋਂ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_READ(ਅੰਕੜੇ, ...) \
  ਕਰੋ {                           \
    (ਖਾਲੀ)(ਅੰਕੜੇ);                \
  } ਜਦੋਂ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_DNS(ਅੰਕੜੇ, ...) \
  ਕਰੋ {                          \
    (ਖਾਲੀ)(ਅੰਕੜੇ);               \
  } ਜਦੋਂ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_TIMER(ਅੰਕੜੇ, tid, ...) \
  ਕਰੋ {                                 \
    (ਖਾਲੀ)(ਅੰਕੜੇ);                      \
    (ਖਾਲੀ)(tid);                       \
  } ਜਦੋਂ(0)
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_FTP(ਅੰਕੜੇ, ...) \
  ਕਰੋ {                          \
    (ਖਾਲੀ)(ਅੰਕੜੇ);               \
  } ਜਦੋਂ(0)
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_FTP */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_SMTP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_SMTP(ਅੰਕੜੇ, ...) \
  ਕਰੋ {                           \
    (ਖਾਲੀ)(ਅੰਕੜੇ);                \
  } ਜਦੋਂ(0)
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_SMTP */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_SSLS(ਅੰਕੜੇ, ...) \
  ਕਰੋ {                           \
    (ਖਾਲੀ)(ਅੰਕੜੇ);                \
  } ਜਦੋਂ(0)
#ਜੇ_ਅੰਤ /* USE_SSL */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSH
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_SSH(ਅੰਕੜੇ, ...) \
  ਕਰੋ {                          \
    (ਖਾਲੀ)(ਅੰਕੜੇ);               \
  } ਜਦੋਂ(0)
#ਜੇ_ਅੰਤ /* USE_SSH */
#ਜੇ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_WS(ਅੰਕੜੇ, ...) \
  ਕਰੋ {                         \
    (ਖਾਲੀ)(ਅੰਕੜੇ);              \
  } ਜਦੋਂ(0)
#ਜੇ_ਅੰਤ

#ਨਹੀਂਤਾਂ /* !CURL_HAVE_MACRO_VARARG */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ infof          Curl_infof
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_M     Curl_trc_multi
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_CF    Curl_trc_cf_infof
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_WRITE Curl_trc_write
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_READ  Curl_trc_read
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_DNS   Curl_trc_dns
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_TIMER Curl_trc_timer

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_FTP   Curl_trc_ftp
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_SMTP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_SMTP  Curl_trc_smtp
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_SSLS  Curl_trc_ssls
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSH
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_SSH   Curl_trc_ssh
#ਜੇ_ਅੰਤ
#ਜੇ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_WS    Curl_trc_ws
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* CURL_HAVE_MACRO_VARARG */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਬਾਹਰੀ ਬਣਤਰ curl_trc_feat Curl_trc_feat_multi;
ਬਾਹਰੀ ਬਣਤਰ curl_trc_feat Curl_trc_feat_read;
ਬਾਹਰੀ ਬਣਤਰ curl_trc_feat Curl_trc_feat_write;
ਬਾਹਰੀ ਬਣਤਰ curl_trc_feat Curl_trc_feat_dns;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DOH
ਬਾਹਰੀ ਬਣਤਰ curl_trc_feat Curl_trc_feat_doh;
#ਜੇ_ਅੰਤ
ਬਾਹਰੀ ਬਣਤਰ curl_trc_feat Curl_trc_feat_timer;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ਧਾਗੇ
ਬਾਹਰੀ ਬਣਤਰ curl_trc_feat Curl_trc_feat_ਧਾਗੇ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_VERBOSE_ਲੜੀਆਂ
/* informational messages enabled */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_trc_is_verbose(ਅੰਕੜੇ)                          \
  ((ਅੰਕੜੇ) && (ਅੰਕੜੇ)->ਸੈੱਟ.verbose &&                        \
   (!(ਅੰਕੜੇ)->state.feat ||                                 \
    ((ਅੰਕੜੇ)->state.feat->log_level >= CURL_LOG_LVL_INFO)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_trc_cf_is_verbose(cf, ਅੰਕੜੇ)              \
  (Curl_trc_is_verbose(ਅੰਕੜੇ) &&                       \
   (cf) && (cf)->cft->log_level >= CURL_LOG_LVL_INFO)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_trc_ft_is_verbose(ਅੰਕੜੇ, ft) \
  (Curl_trc_is_verbose(ਅੰਕੜੇ) &&          \
   (ft)->log_level >= CURL_LOG_LVL_INFO)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_MSTATE_NAME(s)  Curl_trc_mstate_name((ਪੂਰਨ)(s))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_EASY_TIMERS(ਅੰਕੜੇ)      \
  ਕਰੋ {                                  \
    ਜੇ(CURL_TRC_TIMER_is_verbose(ਅੰਕੜੇ)) \
      Curl_trc_easy_timers(ਅੰਕੜੇ);       \
  } ਜਦੋਂ(0)

#ਨਹੀਂਤਾਂ /* CURL_DISABLE_VERBOSE_STRINGS */
/* All informational messages are not compiled in for size savings */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_trc_is_verbose(d)        FALSE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_trc_cf_is_verbose(x, y)  FALSE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_trc_ft_is_verbose(x, y)  FALSE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_MSTATE_NAME(x)           ((ਖਾਲੀ)(x), "-")
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_TRC_EASY_TIMERS(x)       Curl_nop_stmt
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_VERBOSE_STRINGS */

#ਜੇ_ਅੰਤ /* HEADER_CURL_TRC_H */
