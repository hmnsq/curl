#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_TRC_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_TRC_H
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
ರಚನೆ Curl_easy;
ರಚನೆ Curl_cfilter;

/**
 * Init logging, return != 0 on failure.
 */
CURLcode Curl_trc_init(ಶೂನ್ಯ);

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
CURLcode Curl_trc_opt(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *config);

/* the function used to output verbose information */
ಶೂನ್ಯ Curl_debug(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, curl_infotype type,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr, size_t ಗಾತ್ರ_2);

/**
 * Output a failure message on registered callbacks for transfer.
 */
ಶೂನ್ಯ Curl_failf(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...) CURL_PRINTF(2, 3);

#ವ್ಯಾಖ್ಯಾನಿಸು failf Curl_failf

/* In case failf() reported into the errorbuf, clear it again.
 * This is used to clear information from happy eyeballing attempts
 * when ultimately a successful attempt was made. */
ಶೂನ್ಯ Curl_reset_fail(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_LOG_LVL_NONE  0
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_LOG_LVL_INFO  1

/**
 * Output an informational message when transfer's verbose logging is enabled.
 */
ಶೂನ್ಯ Curl_infof(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...) CURL_PRINTF(2, 3);

/**
 * Output an informational message when both transfer's verbose logging
 * and connection filters verbose logging are enabled.
 */
ಶೂನ್ಯ Curl_trc_cf_infof(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cfilter *cf,
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...) CURL_PRINTF(3, 4);
ಶೂನ್ಯ Curl_trc_multi(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...) CURL_PRINTF(2, 3);
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Curl_trc_mstate_name(ಪೂರ್ಣಾಂಕ state);
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Curl_trc_timer_name(ಪೂರ್ಣಾಂಕ tid);
ಶೂನ್ಯ Curl_trc_easy_timers(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

ಶೂನ್ಯ Curl_trc_write(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...) CURL_PRINTF(2, 3);
ಶೂನ್ಯ Curl_trc_read(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...) CURL_PRINTF(2, 3);
ಶೂನ್ಯ Curl_trc_dns(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...) CURL_PRINTF(2, 3);
ಶೂನ್ಯ Curl_trc_timer(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ tid,
                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...) CURL_PRINTF(3, 4);

ರಚನೆ curl_trc_feat {
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
  ಪೂರ್ಣಾಂಕ log_level;
};

ಶೂನ್ಯ Curl_trc_feat_infof(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         ರಚನೆ curl_trc_feat *feat,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...) CURL_PRINTF(3, 4);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
ಬಾಹ್ಯ ರಚನೆ curl_trc_feat Curl_trc_feat_ftp;
ಶೂನ್ಯ Curl_trc_ftp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...) CURL_PRINTF(2, 3);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_SMTP
ಬಾಹ್ಯ ರಚನೆ curl_trc_feat Curl_trc_feat_smtp;
ಶೂನ್ಯ Curl_trc_smtp(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...) CURL_PRINTF(2, 3);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
ಬಾಹ್ಯ ರಚನೆ curl_trc_feat Curl_trc_feat_ssls;
ಶೂನ್ಯ Curl_trc_ssls(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...) CURL_PRINTF(2, 3);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSH
ಬಾಹ್ಯ ರಚನೆ curl_trc_feat Curl_trc_feat_ssh;
ಶೂನ್ಯ Curl_trc_ssh(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...) CURL_PRINTF(2, 3);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
ಬಾಹ್ಯ ರಚನೆ curl_trc_feat Curl_trc_feat_ws;
ಶೂನ್ಯ Curl_trc_ws(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...) CURL_PRINTF(2, 3);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_M_is_verbose(ದತ್ತಾಂಶ) \
  Curl_trc_ft_is_verbose(ದತ್ತಾಂಶ, &Curl_trc_feat_multi)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_DNS_is_verbose(ದತ್ತಾಂಶ) \
  Curl_trc_ft_is_verbose(ದತ್ತಾಂಶ, &Curl_trc_feat_dns)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_TIMER_is_verbose(ದತ್ತಾಂಶ) \
  Curl_trc_ft_is_verbose(ದತ್ತಾಂಶ, &Curl_trc_feat_timer)

#ಆದರೆ defined(CURL_HAVE_MACRO_VARARG) && !defined(CURL_DISABLE_VERBOSE_ಅಕ್ಷರಸರಣಿಗಳು)
#ವ್ಯಾಖ್ಯಾನಿಸು infof(ದತ್ತಾಂಶ, ...)             \
  ಮಾಡು {                               \
    ಆದರೆ(Curl_trc_is_verbose(ದತ್ತಾಂಶ))    \
      Curl_infof(ದತ್ತಾಂಶ, __VA_ARGS__); \
  } ಆಗ(0)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_M(ದತ್ತಾಂಶ, ...)            \
  ಮಾಡು {                                   \
    ಆದರೆ(CURL_TRC_M_is_verbose(ದತ್ತಾಂಶ))      \
      Curl_trc_multi(ದತ್ತಾಂಶ, __VA_ARGS__); \
  } ಆಗ(0)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_CF(ದತ್ತಾಂಶ, cf, ...)              \
  ಮಾಡು {                                          \
    ಆದರೆ(Curl_trc_cf_is_verbose(cf, ದತ್ತಾಂಶ))        \
      Curl_trc_cf_infof(ದತ್ತಾಂಶ, cf, __VA_ARGS__); \
  } ಆಗ(0)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_WRITE(ದತ್ತಾಂಶ, ...)                          \
  ಮಾಡು {                                                     \
    ಆದರೆ(Curl_trc_ft_is_verbose(ದತ್ತಾಂಶ, &Curl_trc_feat_write)) \
      Curl_trc_write(ದತ್ತಾಂಶ, __VA_ARGS__);                   \
  } ಆಗ(0)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_READ(ದತ್ತಾಂಶ, ...)                          \
  ಮಾಡು {                                                    \
    ಆದರೆ(Curl_trc_ft_is_verbose(ದತ್ತಾಂಶ, &Curl_trc_feat_read)) \
      Curl_trc_read(ದತ್ತಾಂಶ, __VA_ARGS__);                   \
  } ಆಗ(0)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_DNS(ದತ್ತಾಂಶ, ...)        \
  ಮಾಡು {                                 \
    ಆದರೆ(CURL_TRC_DNS_is_verbose(ದತ್ತಾಂಶ))  \
      Curl_trc_dns(ದತ್ತಾಂಶ, __VA_ARGS__); \
  } ಆಗ(0)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_TIMER(ದತ್ತಾಂಶ, tid, ...)        \
  ಮಾಡು {                                        \
    ಆದರೆ(CURL_TRC_TIMER_is_verbose(ದತ್ತಾಂಶ))       \
      Curl_trc_timer(ದತ್ತಾಂಶ, tid, __VA_ARGS__); \
  } ಆಗ(0)

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_FTP(ದತ್ತಾಂಶ, ...)                          \
  ಮಾಡು {                                                   \
    ಆದರೆ(Curl_trc_ft_is_verbose(ದತ್ತಾಂಶ, &Curl_trc_feat_ftp)) \
      Curl_trc_ftp(ದತ್ತಾಂಶ, __VA_ARGS__);                   \
  } ಆಗ(0)
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_FTP */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_SMTP
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_SMTP(ದತ್ತಾಂಶ, ...)                          \
  ಮಾಡು {                                                    \
    ಆದರೆ(Curl_trc_ft_is_verbose(ದತ್ತಾಂಶ, &Curl_trc_feat_smtp)) \
      Curl_trc_smtp(ದತ್ತಾಂಶ, __VA_ARGS__);                   \
  } ಆಗ(0)
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_SMTP */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_SSLS(ದತ್ತಾಂಶ, ...)                          \
  ಮಾಡು {                                                    \
    ಆದರೆ(Curl_trc_ft_is_verbose(ದತ್ತಾಂಶ, &Curl_trc_feat_ssls)) \
      Curl_trc_ssls(ದತ್ತಾಂಶ, __VA_ARGS__);                   \
  } ಆಗ(0)
#ಆದರೆ_ಅಂತ್ಯ /* USE_SSL */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSH
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_SSH(ದತ್ತಾಂಶ, ...)                          \
  ಮಾಡು {                                                   \
    ಆದರೆ(Curl_trc_ft_is_verbose(ದತ್ತಾಂಶ, &Curl_trc_feat_ssh)) \
      Curl_trc_ssh(ದತ್ತಾಂಶ, __VA_ARGS__);                   \
  } ಆಗ(0)
#ಆದರೆ_ಅಂತ್ಯ /* USE_SSH */
#ಆದರೆ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_WS(ದತ್ತಾಂಶ, ...)                          \
  ಮಾಡು {                                                  \
    ಆದರೆ(Curl_trc_ft_is_verbose(ದತ್ತಾಂಶ, &Curl_trc_feat_ws)) \
      Curl_trc_ws(ದತ್ತಾಂಶ, __VA_ARGS__);                   \
  } ಆಗ(0)
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#ಅಥವಾಆದರೆ defined(CURL_HAVE_MACRO_VARARG) && defined(CURL_DISABLE_VERBOSE_ಅಕ್ಷರಸರಣಿಗಳು)

#ವ್ಯಾಖ್ಯಾನಿಸು infof(ದತ್ತಾಂಶ, ...) \
  ಮಾಡು {                   \
    (ಶೂನ್ಯ)(ದತ್ತಾಂಶ);        \
  } ಆಗ(0)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_M(ದತ್ತಾಂಶ, ...) \
  ಮಾಡು {                        \
    (ಶೂನ್ಯ)(ದತ್ತಾಂಶ);             \
  } ಆಗ(0)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_CF(ದತ್ತಾಂಶ, cf, ...) \
  ಮಾಡು {                             \
    (ಶೂನ್ಯ)(ದತ್ತಾಂಶ);                  \
    (ಶೂನ್ಯ)(cf);                    \
  } ಆಗ(0)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_WRITE(ದತ್ತಾಂಶ, ...) \
  ಮಾಡು {                            \
    (ಶೂನ್ಯ)(ದತ್ತಾಂಶ);                 \
  } ಆಗ(0)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_READ(ದತ್ತಾಂಶ, ...) \
  ಮಾಡು {                           \
    (ಶೂನ್ಯ)(ದತ್ತಾಂಶ);                \
  } ಆಗ(0)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_DNS(ದತ್ತಾಂಶ, ...) \
  ಮಾಡು {                          \
    (ಶೂನ್ಯ)(ದತ್ತಾಂಶ);               \
  } ಆಗ(0)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_TIMER(ದತ್ತಾಂಶ, tid, ...) \
  ಮಾಡು {                                 \
    (ಶೂನ್ಯ)(ದತ್ತಾಂಶ);                      \
    (ಶೂನ್ಯ)(tid);                       \
  } ಆಗ(0)
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_FTP(ದತ್ತಾಂಶ, ...) \
  ಮಾಡು {                          \
    (ಶೂನ್ಯ)(ದತ್ತಾಂಶ);               \
  } ಆಗ(0)
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_FTP */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_SMTP
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_SMTP(ದತ್ತಾಂಶ, ...) \
  ಮಾಡು {                           \
    (ಶೂನ್ಯ)(ದತ್ತಾಂಶ);                \
  } ಆಗ(0)
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_SMTP */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_SSLS(ದತ್ತಾಂಶ, ...) \
  ಮಾಡು {                           \
    (ಶೂನ್ಯ)(ದತ್ತಾಂಶ);                \
  } ಆಗ(0)
#ಆದರೆ_ಅಂತ್ಯ /* USE_SSL */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSH
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_SSH(ದತ್ತಾಂಶ, ...) \
  ಮಾಡು {                          \
    (ಶೂನ್ಯ)(ದತ್ತಾಂಶ);               \
  } ಆಗ(0)
#ಆದರೆ_ಅಂತ್ಯ /* USE_SSH */
#ಆದರೆ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_WS(ದತ್ತಾಂಶ, ...) \
  ಮಾಡು {                         \
    (ಶೂನ್ಯ)(ದತ್ತಾಂಶ);              \
  } ಆಗ(0)
#ಆದರೆ_ಅಂತ್ಯ

#ಇಲ್ಲದಿದ್ದರೆ /* !CURL_HAVE_MACRO_VARARG */

#ವ್ಯಾಖ್ಯಾನಿಸು infof          Curl_infof
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_M     Curl_trc_multi
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_CF    Curl_trc_cf_infof
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_WRITE Curl_trc_write
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_READ  Curl_trc_read
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_DNS   Curl_trc_dns
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_TIMER Curl_trc_timer

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_FTP   Curl_trc_ftp
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_SMTP
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_SMTP  Curl_trc_smtp
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_SSLS  Curl_trc_ssls
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSH
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_SSH   Curl_trc_ssh
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_WS    Curl_trc_ws
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* CURL_HAVE_MACRO_VARARG */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಬಾಹ್ಯ ರಚನೆ curl_trc_feat Curl_trc_feat_multi;
ಬಾಹ್ಯ ರಚನೆ curl_trc_feat Curl_trc_feat_read;
ಬಾಹ್ಯ ರಚನೆ curl_trc_feat Curl_trc_feat_write;
ಬಾಹ್ಯ ರಚನೆ curl_trc_feat Curl_trc_feat_dns;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DOH
ಬಾಹ್ಯ ರಚನೆ curl_trc_feat Curl_trc_feat_doh;
#ಆದರೆ_ಅಂತ್ಯ
ಬಾಹ್ಯ ರಚನೆ curl_trc_feat Curl_trc_feat_timer;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ಎಳೆಗಳು
ಬಾಹ್ಯ ರಚನೆ curl_trc_feat Curl_trc_feat_ಎಳೆಗಳು;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_VERBOSE_ಅಕ್ಷರಸರಣಿಗಳು
/* informational messages enabled */
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_trc_is_verbose(ದತ್ತಾಂಶ)                          \
  ((ದತ್ತಾಂಶ) && (ದತ್ತಾಂಶ)->ಗಣ.verbose &&                        \
   (!(ದತ್ತಾಂಶ)->state.feat ||                                 \
    ((ದತ್ತಾಂಶ)->state.feat->log_level >= CURL_LOG_LVL_INFO)))
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_trc_cf_is_verbose(cf, ದತ್ತಾಂಶ)              \
  (Curl_trc_is_verbose(ದತ್ತಾಂಶ) &&                       \
   (cf) && (cf)->cft->log_level >= CURL_LOG_LVL_INFO)
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_trc_ft_is_verbose(ದತ್ತಾಂಶ, ft) \
  (Curl_trc_is_verbose(ದತ್ತಾಂಶ) &&          \
   (ft)->log_level >= CURL_LOG_LVL_INFO)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_MSTATE_NAME(s)  Curl_trc_mstate_name((ಪೂರ್ಣಾಂಕ)(s))
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_EASY_TIMERS(ದತ್ತಾಂಶ)      \
  ಮಾಡು {                                  \
    ಆದರೆ(CURL_TRC_TIMER_is_verbose(ದತ್ತಾಂಶ)) \
      Curl_trc_easy_timers(ದತ್ತಾಂಶ);       \
  } ಆಗ(0)

#ಇಲ್ಲದಿದ್ದರೆ /* CURL_DISABLE_VERBOSE_STRINGS */
/* All informational messages are not compiled in for size savings */
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_trc_is_verbose(d)        FALSE
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_trc_cf_is_verbose(x, y)  FALSE
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_trc_ft_is_verbose(x, y)  FALSE
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_MSTATE_NAME(x)           ((ಶೂನ್ಯ)(x), "-")
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_TRC_EASY_TIMERS(x)       Curl_nop_stmt
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_VERBOSE_STRINGS */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_TRC_H */
