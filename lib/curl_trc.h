#यदि_अपरिभाषितम् शीर्षकम्_CURL_TRC_H
#परिभाषय शीर्षकम्_CURL_TRC_H
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
CURLcode Curl_trc_opt(ध्रुव वर्ण *config);

/* the function used to output verbose information */
शून्य Curl_debug(संरचना Curl_easy *दत्त, curl_infotype type,
                ध्रुव वर्ण *ptr, size_t आकार);

/**
 * Output a failure message on registered callbacks for transfer.
 */
शून्य Curl_failf(संरचना Curl_easy *दत्त,
                ध्रुव वर्ण *fmt, ...) CURL_PRINTF(2, 3);

#परिभाषय failf Curl_failf

/* In case failf() reported into the errorbuf, clear it again.
 * This is used to clear information from happy eyeballing attempts
 * when ultimately a successful attempt was made. */
शून्य Curl_reset_fail(संरचना Curl_easy *दत्त);

#परिभाषय CURL_LOG_LVL_NONE  0
#परिभाषय CURL_LOG_LVL_INFO  1

/**
 * Output an informational message when transfer's verbose logging is enabled.
 */
शून्य Curl_infof(संरचना Curl_easy *दत्त,
                ध्रुव वर्ण *fmt, ...) CURL_PRINTF(2, 3);

/**
 * Output an informational message when both transfer's verbose logging
 * and connection filters verbose logging are enabled.
 */
शून्य Curl_trc_cf_infof(संरचना Curl_easy *दत्त, ध्रुव संरचना Curl_cfilter *cf,
                       ध्रुव वर्ण *fmt, ...) CURL_PRINTF(3, 4);
शून्य Curl_trc_multi(संरचना Curl_easy *दत्त,
                    ध्रुव वर्ण *fmt, ...) CURL_PRINTF(2, 3);
ध्रुव वर्ण *Curl_trc_mstate_name(पूर्णाङ्क state);
ध्रुव वर्ण *Curl_trc_timer_name(पूर्णाङ्क tid);
शून्य Curl_trc_easy_timers(संरचना Curl_easy *दत्त);

शून्य Curl_trc_write(संरचना Curl_easy *दत्त,
                    ध्रुव वर्ण *fmt, ...) CURL_PRINTF(2, 3);
शून्य Curl_trc_read(संरचना Curl_easy *दत्त,
                   ध्रुव वर्ण *fmt, ...) CURL_PRINTF(2, 3);
शून्य Curl_trc_dns(संरचना Curl_easy *दत्त,
                  ध्रुव वर्ण *fmt, ...) CURL_PRINTF(2, 3);
शून्य Curl_trc_timer(संरचना Curl_easy *दत्त, पूर्णाङ्क tid,
                    ध्रुव वर्ण *fmt, ...) CURL_PRINTF(3, 4);

संरचना curl_trc_feat {
  ध्रुव वर्ण *name;
  पूर्णाङ्क log_level;
};

शून्य Curl_trc_feat_infof(संरचना Curl_easy *दत्त,
                         संरचना curl_trc_feat *feat,
                         ध्रुव वर्ण *fmt, ...) CURL_PRINTF(3, 4);

#यदि_अपरिभाषितम् CURL_DISABLE_FTP
बाह्य संरचना curl_trc_feat Curl_trc_feat_ftp;
शून्य Curl_trc_ftp(संरचना Curl_easy *दत्त,
                  ध्रुव वर्ण *fmt, ...) CURL_PRINTF(2, 3);
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_SMTP
बाह्य संरचना curl_trc_feat Curl_trc_feat_smtp;
शून्य Curl_trc_smtp(संरचना Curl_easy *दत्त,
                   ध्रुव वर्ण *fmt, ...) CURL_PRINTF(2, 3);
#यदि_अन्तः
#यदि_परिभाषितम् USE_SSL
बाह्य संरचना curl_trc_feat Curl_trc_feat_ssls;
शून्य Curl_trc_ssls(संरचना Curl_easy *दत्त,
                   ध्रुव वर्ण *fmt, ...) CURL_PRINTF(2, 3);
#यदि_अन्तः
#यदि_परिभाषितम् USE_SSH
बाह्य संरचना curl_trc_feat Curl_trc_feat_ssh;
शून्य Curl_trc_ssh(संरचना Curl_easy *दत्त,
                  ध्रुव वर्ण *fmt, ...) CURL_PRINTF(2, 3);
#यदि_अन्तः
#यदि !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
बाह्य संरचना curl_trc_feat Curl_trc_feat_ws;
शून्य Curl_trc_ws(संरचना Curl_easy *दत्त,
                 ध्रुव वर्ण *fmt, ...) CURL_PRINTF(2, 3);
#यदि_अन्तः

#परिभाषय CURL_TRC_M_is_verbose(दत्त) \
  Curl_trc_ft_is_verbose(दत्त, &Curl_trc_feat_multi)
#परिभाषय CURL_TRC_DNS_is_verbose(दत्त) \
  Curl_trc_ft_is_verbose(दत्त, &Curl_trc_feat_dns)
#परिभाषय CURL_TRC_TIMER_is_verbose(दत्त) \
  Curl_trc_ft_is_verbose(दत्त, &Curl_trc_feat_timer)

#यदि defined(CURL_HAVE_MACRO_VARARG) && !defined(CURL_DISABLE_VERBOSE_वाक्यानि)
#परिभाषय infof(दत्त, ...)             \
  कुरु {                               \
    यदि(Curl_trc_is_verbose(दत्त))    \
      Curl_infof(दत्त, __VA_ARGS__); \
  } यावत्(0)
#परिभाषय CURL_TRC_M(दत्त, ...)            \
  कुरु {                                   \
    यदि(CURL_TRC_M_is_verbose(दत्त))      \
      Curl_trc_multi(दत्त, __VA_ARGS__); \
  } यावत्(0)
#परिभाषय CURL_TRC_CF(दत्त, cf, ...)              \
  कुरु {                                          \
    यदि(Curl_trc_cf_is_verbose(cf, दत्त))        \
      Curl_trc_cf_infof(दत्त, cf, __VA_ARGS__); \
  } यावत्(0)
#परिभाषय CURL_TRC_WRITE(दत्त, ...)                          \
  कुरु {                                                     \
    यदि(Curl_trc_ft_is_verbose(दत्त, &Curl_trc_feat_write)) \
      Curl_trc_write(दत्त, __VA_ARGS__);                   \
  } यावत्(0)
#परिभाषय CURL_TRC_READ(दत्त, ...)                          \
  कुरु {                                                    \
    यदि(Curl_trc_ft_is_verbose(दत्त, &Curl_trc_feat_read)) \
      Curl_trc_read(दत्त, __VA_ARGS__);                   \
  } यावत्(0)
#परिभाषय CURL_TRC_DNS(दत्त, ...)        \
  कुरु {                                 \
    यदि(CURL_TRC_DNS_is_verbose(दत्त))  \
      Curl_trc_dns(दत्त, __VA_ARGS__); \
  } यावत्(0)
#परिभाषय CURL_TRC_TIMER(दत्त, tid, ...)        \
  कुरु {                                        \
    यदि(CURL_TRC_TIMER_is_verbose(दत्त))       \
      Curl_trc_timer(दत्त, tid, __VA_ARGS__); \
  } यावत्(0)

#यदि_अपरिभाषितम् CURL_DISABLE_FTP
#परिभाषय CURL_TRC_FTP(दत्त, ...)                          \
  कुरु {                                                   \
    यदि(Curl_trc_ft_is_verbose(दत्त, &Curl_trc_feat_ftp)) \
      Curl_trc_ftp(दत्त, __VA_ARGS__);                   \
  } यावत्(0)
#यदि_अन्तः /* !CURL_DISABLE_FTP */
#यदि_अपरिभाषितम् CURL_DISABLE_SMTP
#परिभाषय CURL_TRC_SMTP(दत्त, ...)                          \
  कुरु {                                                    \
    यदि(Curl_trc_ft_is_verbose(दत्त, &Curl_trc_feat_smtp)) \
      Curl_trc_smtp(दत्त, __VA_ARGS__);                   \
  } यावत्(0)
#यदि_अन्तः /* !CURL_DISABLE_SMTP */
#यदि_परिभाषितम् USE_SSL
#परिभाषय CURL_TRC_SSLS(दत्त, ...)                          \
  कुरु {                                                    \
    यदि(Curl_trc_ft_is_verbose(दत्त, &Curl_trc_feat_ssls)) \
      Curl_trc_ssls(दत्त, __VA_ARGS__);                   \
  } यावत्(0)
#यदि_अन्तः /* USE_SSL */
#यदि_परिभाषितम् USE_SSH
#परिभाषय CURL_TRC_SSH(दत्त, ...)                          \
  कुरु {                                                   \
    यदि(Curl_trc_ft_is_verbose(दत्त, &Curl_trc_feat_ssh)) \
      Curl_trc_ssh(दत्त, __VA_ARGS__);                   \
  } यावत्(0)
#यदि_अन्तः /* USE_SSH */
#यदि !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#परिभाषय CURL_TRC_WS(दत्त, ...)                          \
  कुरु {                                                  \
    यदि(Curl_trc_ft_is_verbose(दत्त, &Curl_trc_feat_ws)) \
      Curl_trc_ws(दत्त, __VA_ARGS__);                   \
  } यावत्(0)
#यदि_अन्तः /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#नोचेद्यदि defined(CURL_HAVE_MACRO_VARARG) && defined(CURL_DISABLE_VERBOSE_वाक्यानि)

#परिभाषय infof(दत्त, ...) \
  कुरु {                   \
    (शून्य)(दत्त);        \
  } यावत्(0)
#परिभाषय CURL_TRC_M(दत्त, ...) \
  कुरु {                        \
    (शून्य)(दत्त);             \
  } यावत्(0)
#परिभाषय CURL_TRC_CF(दत्त, cf, ...) \
  कुरु {                             \
    (शून्य)(दत्त);                  \
    (शून्य)(cf);                    \
  } यावत्(0)
#परिभाषय CURL_TRC_WRITE(दत्त, ...) \
  कुरु {                            \
    (शून्य)(दत्त);                 \
  } यावत्(0)
#परिभाषय CURL_TRC_READ(दत्त, ...) \
  कुरु {                           \
    (शून्य)(दत्त);                \
  } यावत्(0)
#परिभाषय CURL_TRC_DNS(दत्त, ...) \
  कुरु {                          \
    (शून्य)(दत्त);               \
  } यावत्(0)
#परिभाषय CURL_TRC_TIMER(दत्त, tid, ...) \
  कुरु {                                 \
    (शून्य)(दत्त);                      \
    (शून्य)(tid);                       \
  } यावत्(0)
#यदि_अपरिभाषितम् CURL_DISABLE_FTP
#परिभाषय CURL_TRC_FTP(दत्त, ...) \
  कुरु {                          \
    (शून्य)(दत्त);               \
  } यावत्(0)
#यदि_अन्तः /* !CURL_DISABLE_FTP */
#यदि_अपरिभाषितम् CURL_DISABLE_SMTP
#परिभाषय CURL_TRC_SMTP(दत्त, ...) \
  कुरु {                           \
    (शून्य)(दत्त);                \
  } यावत्(0)
#यदि_अन्तः /* !CURL_DISABLE_SMTP */
#यदि_परिभाषितम् USE_SSL
#परिभाषय CURL_TRC_SSLS(दत्त, ...) \
  कुरु {                           \
    (शून्य)(दत्त);                \
  } यावत्(0)
#यदि_अन्तः /* USE_SSL */
#यदि_परिभाषितम् USE_SSH
#परिभाषय CURL_TRC_SSH(दत्त, ...) \
  कुरु {                          \
    (शून्य)(दत्त);               \
  } यावत्(0)
#यदि_अन्तः /* USE_SSH */
#यदि !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#परिभाषय CURL_TRC_WS(दत्त, ...) \
  कुरु {                         \
    (शून्य)(दत्त);              \
  } यावत्(0)
#यदि_अन्तः

#अन्यथा /* !CURL_HAVE_MACRO_VARARG */

#परिभाषय infof          Curl_infof
#परिभाषय CURL_TRC_M     Curl_trc_multi
#परिभाषय CURL_TRC_CF    Curl_trc_cf_infof
#परिभाषय CURL_TRC_WRITE Curl_trc_write
#परिभाषय CURL_TRC_READ  Curl_trc_read
#परिभाषय CURL_TRC_DNS   Curl_trc_dns
#परिभाषय CURL_TRC_TIMER Curl_trc_timer

#यदि_अपरिभाषितम् CURL_DISABLE_FTP
#परिभाषय CURL_TRC_FTP   Curl_trc_ftp
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_SMTP
#परिभाषय CURL_TRC_SMTP  Curl_trc_smtp
#यदि_अन्तः
#यदि_परिभाषितम् USE_SSL
#परिभाषय CURL_TRC_SSLS  Curl_trc_ssls
#यदि_अन्तः
#यदि_परिभाषितम् USE_SSH
#परिभाषय CURL_TRC_SSH   Curl_trc_ssh
#यदि_अन्तः
#यदि !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#परिभाषय CURL_TRC_WS    Curl_trc_ws
#यदि_अन्तः

#यदि_अन्तः /* CURL_HAVE_MACRO_VARARG */

#यदि_परिभाषितम् CURLVERBOSE
बाह्य संरचना curl_trc_feat Curl_trc_feat_multi;
बाह्य संरचना curl_trc_feat Curl_trc_feat_read;
बाह्य संरचना curl_trc_feat Curl_trc_feat_write;
बाह्य संरचना curl_trc_feat Curl_trc_feat_dns;
#यदि_अपरिभाषितम् CURL_DISABLE_DOH
बाह्य संरचना curl_trc_feat Curl_trc_feat_doh;
#यदि_अन्तः
बाह्य संरचना curl_trc_feat Curl_trc_feat_timer;
#यदि_परिभाषितम् USE_तन्तवः
बाह्य संरचना curl_trc_feat Curl_trc_feat_तन्तवः;
#यदि_अन्तः
#यदि_अन्तः

#यदि_अपरिभाषितम् CURL_DISABLE_VERBOSE_वाक्यानि
/* informational messages enabled */
#परिभाषय Curl_trc_is_verbose(दत्त)                          \
  ((दत्त) && (दत्त)->समुच्चयः.verbose &&                        \
   (!(दत्त)->state.feat ||                                 \
    ((दत्त)->state.feat->log_level >= CURL_LOG_LVL_INFO)))
#परिभाषय Curl_trc_cf_is_verbose(cf, दत्त)              \
  (Curl_trc_is_verbose(दत्त) &&                       \
   (cf) && (cf)->cft->log_level >= CURL_LOG_LVL_INFO)
#परिभाषय Curl_trc_ft_is_verbose(दत्त, ft) \
  (Curl_trc_is_verbose(दत्त) &&          \
   (ft)->log_level >= CURL_LOG_LVL_INFO)
#परिभाषय CURL_MSTATE_NAME(s)  Curl_trc_mstate_name((पूर्णाङ्क)(s))
#परिभाषय CURL_TRC_EASY_TIMERS(दत्त)      \
  कुरु {                                  \
    यदि(CURL_TRC_TIMER_is_verbose(दत्त)) \
      Curl_trc_easy_timers(दत्त);       \
  } यावत्(0)

#अन्यथा /* CURL_DISABLE_VERBOSE_STRINGS */
/* All informational messages are not compiled in for size savings */
#परिभाषय Curl_trc_is_verbose(d)        FALSE
#परिभाषय Curl_trc_cf_is_verbose(x, y)  FALSE
#परिभाषय Curl_trc_ft_is_verbose(x, y)  FALSE
#परिभाषय CURL_MSTATE_NAME(x)           ((शून्य)(x), "-")
#परिभाषय CURL_TRC_EASY_TIMERS(x)       Curl_nop_stmt
#यदि_अन्तः /* !CURL_DISABLE_VERBOSE_STRINGS */

#यदि_अन्तः /* HEADER_CURL_TRC_H */
