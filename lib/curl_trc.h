#నిర్వచితం_కాకపోతే శీర్షిక_CURL_TRC_H
#నిర్వచించు శీర్షిక_CURL_TRC_H
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
నిర్మాణం Curl_easy;
నిర్మాణం Curl_cfilter;

/**
 * Init logging, return != 0 on failure.
 */
CURLcode Curl_trc_init(శూన్య);

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
CURLcode Curl_trc_opt(స్థిరరాశి అక్షరం *config);

/* the function used to output verbose information */
శూన్య Curl_debug(నిర్మాణం Curl_easy *దత్తాంశం, curl_infotype type,
                స్థిరరాశి అక్షరం *ptr, size_t కొలత);

/**
 * Output a failure message on registered callbacks for transfer.
 */
శూన్య Curl_failf(నిర్మాణం Curl_easy *దత్తాంశం,
                స్థిరరాశి అక్షరం *fmt, ...) CURL_PRINTF(2, 3);

#నిర్వచించు failf Curl_failf

/* In case failf() reported into the errorbuf, clear it again.
 * This is used to clear information from happy eyeballing attempts
 * when ultimately a successful attempt was made. */
శూన్య Curl_reset_fail(నిర్మాణం Curl_easy *దత్తాంశం);

#నిర్వచించు CURL_LOG_LVL_NONE  0
#నిర్వచించు CURL_LOG_LVL_INFO  1

/**
 * Output an informational message when transfer's verbose logging is enabled.
 */
శూన్య Curl_infof(నిర్మాణం Curl_easy *దత్తాంశం,
                స్థిరరాశి అక్షరం *fmt, ...) CURL_PRINTF(2, 3);

/**
 * Output an informational message when both transfer's verbose logging
 * and connection filters verbose logging are enabled.
 */
శూన్య Curl_trc_cf_infof(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి నిర్మాణం Curl_cfilter *cf,
                       స్థిరరాశి అక్షరం *fmt, ...) CURL_PRINTF(3, 4);
శూన్య Curl_trc_multi(నిర్మాణం Curl_easy *దత్తాంశం,
                    స్థిరరాశి అక్షరం *fmt, ...) CURL_PRINTF(2, 3);
స్థిరరాశి అక్షరం *Curl_trc_mstate_name(పూర్ణాంకం state);
స్థిరరాశి అక్షరం *Curl_trc_timer_name(పూర్ణాంకం tid);
శూన్య Curl_trc_easy_timers(నిర్మాణం Curl_easy *దత్తాంశం);

శూన్య Curl_trc_write(నిర్మాణం Curl_easy *దత్తాంశం,
                    స్థిరరాశి అక్షరం *fmt, ...) CURL_PRINTF(2, 3);
శూన్య Curl_trc_read(నిర్మాణం Curl_easy *దత్తాంశం,
                   స్థిరరాశి అక్షరం *fmt, ...) CURL_PRINTF(2, 3);
శూన్య Curl_trc_dns(నిర్మాణం Curl_easy *దత్తాంశం,
                  స్థిరరాశి అక్షరం *fmt, ...) CURL_PRINTF(2, 3);
శూన్య Curl_trc_timer(నిర్మాణం Curl_easy *దత్తాంశం, పూర్ణాంకం tid,
                    స్థిరరాశి అక్షరం *fmt, ...) CURL_PRINTF(3, 4);

నిర్మాణం curl_trc_feat {
  స్థిరరాశి అక్షరం *name;
  పూర్ణాంకం log_level;
};

శూన్య Curl_trc_feat_infof(నిర్మాణం Curl_easy *దత్తాంశం,
                         నిర్మాణం curl_trc_feat *feat,
                         స్థిరరాశి అక్షరం *fmt, ...) CURL_PRINTF(3, 4);

#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
బాహ్య నిర్మాణం curl_trc_feat Curl_trc_feat_ftp;
శూన్య Curl_trc_ftp(నిర్మాణం Curl_easy *దత్తాంశం,
                  స్థిరరాశి అక్షరం *fmt, ...) CURL_PRINTF(2, 3);
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_SMTP
బాహ్య నిర్మాణం curl_trc_feat Curl_trc_feat_smtp;
శూన్య Curl_trc_smtp(నిర్మాణం Curl_easy *దత్తాంశం,
                   స్థిరరాశి అక్షరం *fmt, ...) CURL_PRINTF(2, 3);
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SSL
బాహ్య నిర్మాణం curl_trc_feat Curl_trc_feat_ssls;
శూన్య Curl_trc_ssls(నిర్మాణం Curl_easy *దత్తాంశం,
                   స్థిరరాశి అక్షరం *fmt, ...) CURL_PRINTF(2, 3);
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SSH
బాహ్య నిర్మాణం curl_trc_feat Curl_trc_feat_ssh;
శూన్య Curl_trc_ssh(నిర్మాణం Curl_easy *దత్తాంశం,
                  స్థిరరాశి అక్షరం *fmt, ...) CURL_PRINTF(2, 3);
#అయితే_ముగింపు
#ఒకవేళ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
బాహ్య నిర్మాణం curl_trc_feat Curl_trc_feat_ws;
శూన్య Curl_trc_ws(నిర్మాణం Curl_easy *దత్తాంశం,
                 స్థిరరాశి అక్షరం *fmt, ...) CURL_PRINTF(2, 3);
#అయితే_ముగింపు

#నిర్వచించు CURL_TRC_M_is_verbose(దత్తాంశం) \
  Curl_trc_ft_is_verbose(దత్తాంశం, &Curl_trc_feat_multi)
#నిర్వచించు CURL_TRC_DNS_is_verbose(దత్తాంశం) \
  Curl_trc_ft_is_verbose(దత్తాంశం, &Curl_trc_feat_dns)
#నిర్వచించు CURL_TRC_TIMER_is_verbose(దత్తాంశం) \
  Curl_trc_ft_is_verbose(దత్తాంశం, &Curl_trc_feat_timer)

#ఒకవేళ defined(CURL_HAVE_MACRO_VARARG) && !defined(CURL_DISABLE_VERBOSE_అక్షరశ్రేణులు)
#నిర్వచించు infof(దత్తాంశం, ...)             \
  చేయి {                               \
    ఒకవేళ(Curl_trc_is_verbose(దత్తాంశం))    \
      Curl_infof(దత్తాంశం, __VA_ARGS__); \
  } అయితే(0)
#నిర్వచించు CURL_TRC_M(దత్తాంశం, ...)            \
  చేయి {                                   \
    ఒకవేళ(CURL_TRC_M_is_verbose(దత్తాంశం))      \
      Curl_trc_multi(దత్తాంశం, __VA_ARGS__); \
  } అయితే(0)
#నిర్వచించు CURL_TRC_CF(దత్తాంశం, cf, ...)              \
  చేయి {                                          \
    ఒకవేళ(Curl_trc_cf_is_verbose(cf, దత్తాంశం))        \
      Curl_trc_cf_infof(దత్తాంశం, cf, __VA_ARGS__); \
  } అయితే(0)
#నిర్వచించు CURL_TRC_WRITE(దత్తాంశం, ...)                          \
  చేయి {                                                     \
    ఒకవేళ(Curl_trc_ft_is_verbose(దత్తాంశం, &Curl_trc_feat_write)) \
      Curl_trc_write(దత్తాంశం, __VA_ARGS__);                   \
  } అయితే(0)
#నిర్వచించు CURL_TRC_READ(దత్తాంశం, ...)                          \
  చేయి {                                                    \
    ఒకవేళ(Curl_trc_ft_is_verbose(దత్తాంశం, &Curl_trc_feat_read)) \
      Curl_trc_read(దత్తాంశం, __VA_ARGS__);                   \
  } అయితే(0)
#నిర్వచించు CURL_TRC_DNS(దత్తాంశం, ...)        \
  చేయి {                                 \
    ఒకవేళ(CURL_TRC_DNS_is_verbose(దత్తాంశం))  \
      Curl_trc_dns(దత్తాంశం, __VA_ARGS__); \
  } అయితే(0)
#నిర్వచించు CURL_TRC_TIMER(దత్తాంశం, tid, ...)        \
  చేయి {                                        \
    ఒకవేళ(CURL_TRC_TIMER_is_verbose(దత్తాంశం))       \
      Curl_trc_timer(దత్తాంశం, tid, __VA_ARGS__); \
  } అయితే(0)

#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
#నిర్వచించు CURL_TRC_FTP(దత్తాంశం, ...)                          \
  చేయి {                                                   \
    ఒకవేళ(Curl_trc_ft_is_verbose(దత్తాంశం, &Curl_trc_feat_ftp)) \
      Curl_trc_ftp(దత్తాంశం, __VA_ARGS__);                   \
  } అయితే(0)
#అయితే_ముగింపు /* !CURL_DISABLE_FTP */
#నిర్వచితం_కాకపోతే CURL_DISABLE_SMTP
#నిర్వచించు CURL_TRC_SMTP(దత్తాంశం, ...)                          \
  చేయి {                                                    \
    ఒకవేళ(Curl_trc_ft_is_verbose(దత్తాంశం, &Curl_trc_feat_smtp)) \
      Curl_trc_smtp(దత్తాంశం, __VA_ARGS__);                   \
  } అయితే(0)
#అయితే_ముగింపు /* !CURL_DISABLE_SMTP */
#నిర్వచితమైతే USE_SSL
#నిర్వచించు CURL_TRC_SSLS(దత్తాంశం, ...)                          \
  చేయి {                                                    \
    ఒకవేళ(Curl_trc_ft_is_verbose(దత్తాంశం, &Curl_trc_feat_ssls)) \
      Curl_trc_ssls(దత్తాంశం, __VA_ARGS__);                   \
  } అయితే(0)
#అయితే_ముగింపు /* USE_SSL */
#నిర్వచితమైతే USE_SSH
#నిర్వచించు CURL_TRC_SSH(దత్తాంశం, ...)                          \
  చేయి {                                                   \
    ఒకవేళ(Curl_trc_ft_is_verbose(దత్తాంశం, &Curl_trc_feat_ssh)) \
      Curl_trc_ssh(దత్తాంశం, __VA_ARGS__);                   \
  } అయితే(0)
#అయితే_ముగింపు /* USE_SSH */
#ఒకవేళ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#నిర్వచించు CURL_TRC_WS(దత్తాంశం, ...)                          \
  చేయి {                                                  \
    ఒకవేళ(Curl_trc_ft_is_verbose(దత్తాంశం, &Curl_trc_feat_ws)) \
      Curl_trc_ws(దత్తాంశం, __VA_ARGS__);                   \
  } అయితే(0)
#అయితే_ముగింపు /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#లేదావేళ defined(CURL_HAVE_MACRO_VARARG) && defined(CURL_DISABLE_VERBOSE_అక్షరశ్రేణులు)

#నిర్వచించు infof(దత్తాంశం, ...) \
  చేయి {                   \
    (శూన్య)(దత్తాంశం);        \
  } అయితే(0)
#నిర్వచించు CURL_TRC_M(దత్తాంశం, ...) \
  చేయి {                        \
    (శూన్య)(దత్తాంశం);             \
  } అయితే(0)
#నిర్వచించు CURL_TRC_CF(దత్తాంశం, cf, ...) \
  చేయి {                             \
    (శూన్య)(దత్తాంశం);                  \
    (శూన్య)(cf);                    \
  } అయితే(0)
#నిర్వచించు CURL_TRC_WRITE(దత్తాంశం, ...) \
  చేయి {                            \
    (శూన్య)(దత్తాంశం);                 \
  } అయితే(0)
#నిర్వచించు CURL_TRC_READ(దత్తాంశం, ...) \
  చేయి {                           \
    (శూన్య)(దత్తాంశం);                \
  } అయితే(0)
#నిర్వచించు CURL_TRC_DNS(దత్తాంశం, ...) \
  చేయి {                          \
    (శూన్య)(దత్తాంశం);               \
  } అయితే(0)
#నిర్వచించు CURL_TRC_TIMER(దత్తాంశం, tid, ...) \
  చేయి {                                 \
    (శూన్య)(దత్తాంశం);                      \
    (శూన్య)(tid);                       \
  } అయితే(0)
#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
#నిర్వచించు CURL_TRC_FTP(దత్తాంశం, ...) \
  చేయి {                          \
    (శూన్య)(దత్తాంశం);               \
  } అయితే(0)
#అయితే_ముగింపు /* !CURL_DISABLE_FTP */
#నిర్వచితం_కాకపోతే CURL_DISABLE_SMTP
#నిర్వచించు CURL_TRC_SMTP(దత్తాంశం, ...) \
  చేయి {                           \
    (శూన్య)(దత్తాంశం);                \
  } అయితే(0)
#అయితే_ముగింపు /* !CURL_DISABLE_SMTP */
#నిర్వచితమైతే USE_SSL
#నిర్వచించు CURL_TRC_SSLS(దత్తాంశం, ...) \
  చేయి {                           \
    (శూన్య)(దత్తాంశం);                \
  } అయితే(0)
#అయితే_ముగింపు /* USE_SSL */
#నిర్వచితమైతే USE_SSH
#నిర్వచించు CURL_TRC_SSH(దత్తాంశం, ...) \
  చేయి {                          \
    (శూన్య)(దత్తాంశం);               \
  } అయితే(0)
#అయితే_ముగింపు /* USE_SSH */
#ఒకవేళ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#నిర్వచించు CURL_TRC_WS(దత్తాంశం, ...) \
  చేయి {                         \
    (శూన్య)(దత్తాంశం);              \
  } అయితే(0)
#అయితే_ముగింపు

#లేకపోతే /* !CURL_HAVE_MACRO_VARARG */

#నిర్వచించు infof          Curl_infof
#నిర్వచించు CURL_TRC_M     Curl_trc_multi
#నిర్వచించు CURL_TRC_CF    Curl_trc_cf_infof
#నిర్వచించు CURL_TRC_WRITE Curl_trc_write
#నిర్వచించు CURL_TRC_READ  Curl_trc_read
#నిర్వచించు CURL_TRC_DNS   Curl_trc_dns
#నిర్వచించు CURL_TRC_TIMER Curl_trc_timer

#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
#నిర్వచించు CURL_TRC_FTP   Curl_trc_ftp
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_SMTP
#నిర్వచించు CURL_TRC_SMTP  Curl_trc_smtp
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SSL
#నిర్వచించు CURL_TRC_SSLS  Curl_trc_ssls
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SSH
#నిర్వచించు CURL_TRC_SSH   Curl_trc_ssh
#అయితే_ముగింపు
#ఒకవేళ !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#నిర్వచించు CURL_TRC_WS    Curl_trc_ws
#అయితే_ముగింపు

#అయితే_ముగింపు /* CURL_HAVE_MACRO_VARARG */

#నిర్వచితమైతే CURLVERBOSE
బాహ్య నిర్మాణం curl_trc_feat Curl_trc_feat_multi;
బాహ్య నిర్మాణం curl_trc_feat Curl_trc_feat_read;
బాహ్య నిర్మాణం curl_trc_feat Curl_trc_feat_write;
బాహ్య నిర్మాణం curl_trc_feat Curl_trc_feat_dns;
#నిర్వచితం_కాకపోతే CURL_DISABLE_DOH
బాహ్య నిర్మాణం curl_trc_feat Curl_trc_feat_doh;
#అయితే_ముగింపు
బాహ్య నిర్మాణం curl_trc_feat Curl_trc_feat_timer;
#నిర్వచితమైతే USE_దారాలు
బాహ్య నిర్మాణం curl_trc_feat Curl_trc_feat_దారాలు;
#అయితే_ముగింపు
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే CURL_DISABLE_VERBOSE_అక్షరశ్రేణులు
/* informational messages enabled */
#నిర్వచించు Curl_trc_is_verbose(దత్తాంశం)                          \
  ((దత్తాంశం) && (దత్తాంశం)->సమితి.verbose &&                        \
   (!(దత్తాంశం)->state.feat ||                                 \
    ((దత్తాంశం)->state.feat->log_level >= CURL_LOG_LVL_INFO)))
#నిర్వచించు Curl_trc_cf_is_verbose(cf, దత్తాంశం)              \
  (Curl_trc_is_verbose(దత్తాంశం) &&                       \
   (cf) && (cf)->cft->log_level >= CURL_LOG_LVL_INFO)
#నిర్వచించు Curl_trc_ft_is_verbose(దత్తాంశం, ft) \
  (Curl_trc_is_verbose(దత్తాంశం) &&          \
   (ft)->log_level >= CURL_LOG_LVL_INFO)
#నిర్వచించు CURL_MSTATE_NAME(s)  Curl_trc_mstate_name((పూర్ణాంకం)(s))
#నిర్వచించు CURL_TRC_EASY_TIMERS(దత్తాంశం)      \
  చేయి {                                  \
    ఒకవేళ(CURL_TRC_TIMER_is_verbose(దత్తాంశం)) \
      Curl_trc_easy_timers(దత్తాంశం);       \
  } అయితే(0)

#లేకపోతే /* CURL_DISABLE_VERBOSE_STRINGS */
/* All informational messages are not compiled in for size savings */
#నిర్వచించు Curl_trc_is_verbose(d)        FALSE
#నిర్వచించు Curl_trc_cf_is_verbose(x, y)  FALSE
#నిర్వచించు Curl_trc_ft_is_verbose(x, y)  FALSE
#నిర్వచించు CURL_MSTATE_NAME(x)           ((శూన్య)(x), "-")
#నిర్వచించు CURL_TRC_EASY_TIMERS(x)       Curl_nop_stmt
#అయితే_ముగింపు /* !CURL_DISABLE_VERBOSE_STRINGS */

#అయితే_ముగింపు /* HEADER_CURL_TRC_H */
