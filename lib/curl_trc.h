#अगर_अघोषित HEADER_CURL_TRC_H
#घोषणा HEADER_CURL_TRC_H
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
काष्ठा Curl_easy;
काष्ठा Curl_cfilter;

/**
 * Init logging, return != 0 on failure.
 */
CURLcode Curl_trc_init(व्योम);

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
CURLcode Curl_trc_opt(स्थिर अक्षर *config);

/* the function used to output verbose information */
व्योम Curl_debug(काष्ठा Curl_easy *तथ्य, curl_infotype type,
                स्थिर अक्षर *ptr, माप_प्रकार आकार);

/**
 * Output a failure message on registered callbacks for transfer.
 */
व्योम Curl_failf(काष्ठा Curl_easy *तथ्य,
                स्थिर अक्षर *fmt, ...) CURL_PRINTF(2, 3);

#घोषणा failf Curl_failf

/* In case failf() reported into the errorbuf, clear it again.
 * This is used to clear information from happy eyeballing attempts
 * when ultimately a successful attempt was made. */
व्योम Curl_reset_fail(काष्ठा Curl_easy *तथ्य);

#घोषणा CURL_LOG_LVL_NONE  0
#घोषणा CURL_LOG_LVL_INFO  1

/**
 * Output an informational message when transfer's verbose logging is enabled.
 */
व्योम Curl_infof(काष्ठा Curl_easy *तथ्य,
                स्थिर अक्षर *fmt, ...) CURL_PRINTF(2, 3);

/**
 * Output an informational message when both transfer's verbose logging
 * and connection filters verbose logging are enabled.
 */
व्योम Curl_trc_cf_infof(काष्ठा Curl_easy *तथ्य, स्थिर काष्ठा Curl_cfilter *cf,
                       स्थिर अक्षर *fmt, ...) CURL_PRINTF(3, 4);
व्योम Curl_trc_multi(काष्ठा Curl_easy *तथ्य,
                    स्थिर अक्षर *fmt, ...) CURL_PRINTF(2, 3);
स्थिर अक्षर *Curl_trc_mstate_name(पूर्णांक state);
स्थिर अक्षर *Curl_trc_timer_name(पूर्णांक tid);
व्योम Curl_trc_easy_timers(काष्ठा Curl_easy *तथ्य);

व्योम Curl_trc_write(काष्ठा Curl_easy *तथ्य,
                    स्थिर अक्षर *fmt, ...) CURL_PRINTF(2, 3);
व्योम Curl_trc_read(काष्ठा Curl_easy *तथ्य,
                   स्थिर अक्षर *fmt, ...) CURL_PRINTF(2, 3);
व्योम Curl_trc_dns(काष्ठा Curl_easy *तथ्य,
                  स्थिर अक्षर *fmt, ...) CURL_PRINTF(2, 3);
व्योम Curl_trc_timer(काष्ठा Curl_easy *तथ्य, पूर्णांक tid,
                    स्थिर अक्षर *fmt, ...) CURL_PRINTF(3, 4);

काष्ठा curl_trc_feat {
  स्थिर अक्षर *name;
  पूर्णांक log_level;
};

व्योम Curl_trc_feat_infof(काष्ठा Curl_easy *तथ्य,
                         काष्ठा curl_trc_feat *feat,
                         स्थिर अक्षर *fmt, ...) CURL_PRINTF(3, 4);

#अगर_अघोषित CURL_DISABLE_FTP
बाह्य काष्ठा curl_trc_feat Curl_trc_feat_ftp;
व्योम Curl_trc_ftp(काष्ठा Curl_easy *तथ्य,
                  स्थिर अक्षर *fmt, ...) CURL_PRINTF(2, 3);
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_SMTP
बाह्य काष्ठा curl_trc_feat Curl_trc_feat_smtp;
व्योम Curl_trc_smtp(काष्ठा Curl_easy *तथ्य,
                   स्थिर अक्षर *fmt, ...) CURL_PRINTF(2, 3);
#पूर्ण_अगर
#अगर_घोषित USE_SSL
बाह्य काष्ठा curl_trc_feat Curl_trc_feat_ssls;
व्योम Curl_trc_ssls(काष्ठा Curl_easy *तथ्य,
                   स्थिर अक्षर *fmt, ...) CURL_PRINTF(2, 3);
#पूर्ण_अगर
#अगर_घोषित USE_SSH
बाह्य काष्ठा curl_trc_feat Curl_trc_feat_ssh;
व्योम Curl_trc_ssh(काष्ठा Curl_easy *तथ्य,
                  स्थिर अक्षर *fmt, ...) CURL_PRINTF(2, 3);
#पूर्ण_अगर
#अगर !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
बाह्य काष्ठा curl_trc_feat Curl_trc_feat_ws;
व्योम Curl_trc_ws(काष्ठा Curl_easy *तथ्य,
                 स्थिर अक्षर *fmt, ...) CURL_PRINTF(2, 3);
#पूर्ण_अगर

#घोषणा CURL_TRC_M_is_verbose(तथ्य) \
  Curl_trc_ft_is_verbose(तथ्य, &Curl_trc_feat_multi)
#घोषणा CURL_TRC_DNS_is_verbose(तथ्य) \
  Curl_trc_ft_is_verbose(तथ्य, &Curl_trc_feat_dns)
#घोषणा CURL_TRC_TIMER_is_verbose(तथ्य) \
  Curl_trc_ft_is_verbose(तथ्य, &Curl_trc_feat_timer)

#अगर defined(CURL_HAVE_MACRO_VARARG) && !defined(CURL_DISABLE_VERBOSE_STRINGS)
#घोषणा infof(तथ्य, ...)             \
  करो {                               \
    अगर(Curl_trc_is_verbose(तथ्य))    \
      Curl_infof(तथ्य, __VA_ARGS__); \
  } जबतक(0)
#घोषणा CURL_TRC_M(तथ्य, ...)            \
  करो {                                   \
    अगर(CURL_TRC_M_is_verbose(तथ्य))      \
      Curl_trc_multi(तथ्य, __VA_ARGS__); \
  } जबतक(0)
#घोषणा CURL_TRC_CF(तथ्य, cf, ...)              \
  करो {                                          \
    अगर(Curl_trc_cf_is_verbose(cf, तथ्य))        \
      Curl_trc_cf_infof(तथ्य, cf, __VA_ARGS__); \
  } जबतक(0)
#घोषणा CURL_TRC_WRITE(तथ्य, ...)                          \
  करो {                                                     \
    अगर(Curl_trc_ft_is_verbose(तथ्य, &Curl_trc_feat_write)) \
      Curl_trc_write(तथ्य, __VA_ARGS__);                   \
  } जबतक(0)
#घोषणा CURL_TRC_READ(तथ्य, ...)                          \
  करो {                                                    \
    अगर(Curl_trc_ft_is_verbose(तथ्य, &Curl_trc_feat_read)) \
      Curl_trc_read(तथ्य, __VA_ARGS__);                   \
  } जबतक(0)
#घोषणा CURL_TRC_DNS(तथ्य, ...)        \
  करो {                                 \
    अगर(CURL_TRC_DNS_is_verbose(तथ्य))  \
      Curl_trc_dns(तथ्य, __VA_ARGS__); \
  } जबतक(0)
#घोषणा CURL_TRC_TIMER(तथ्य, tid, ...)        \
  करो {                                        \
    अगर(CURL_TRC_TIMER_is_verbose(तथ्य))       \
      Curl_trc_timer(तथ्य, tid, __VA_ARGS__); \
  } जबतक(0)

#अगर_अघोषित CURL_DISABLE_FTP
#घोषणा CURL_TRC_FTP(तथ्य, ...)                          \
  करो {                                                   \
    अगर(Curl_trc_ft_is_verbose(तथ्य, &Curl_trc_feat_ftp)) \
      Curl_trc_ftp(तथ्य, __VA_ARGS__);                   \
  } जबतक(0)
#पूर्ण_अगर /* !CURL_DISABLE_FTP */
#अगर_अघोषित CURL_DISABLE_SMTP
#घोषणा CURL_TRC_SMTP(तथ्य, ...)                          \
  करो {                                                    \
    अगर(Curl_trc_ft_is_verbose(तथ्य, &Curl_trc_feat_smtp)) \
      Curl_trc_smtp(तथ्य, __VA_ARGS__);                   \
  } जबतक(0)
#पूर्ण_अगर /* !CURL_DISABLE_SMTP */
#अगर_घोषित USE_SSL
#घोषणा CURL_TRC_SSLS(तथ्य, ...)                          \
  करो {                                                    \
    अगर(Curl_trc_ft_is_verbose(तथ्य, &Curl_trc_feat_ssls)) \
      Curl_trc_ssls(तथ्य, __VA_ARGS__);                   \
  } जबतक(0)
#पूर्ण_अगर /* USE_SSL */
#अगर_घोषित USE_SSH
#घोषणा CURL_TRC_SSH(तथ्य, ...)                          \
  करो {                                                   \
    अगर(Curl_trc_ft_is_verbose(तथ्य, &Curl_trc_feat_ssh)) \
      Curl_trc_ssh(तथ्य, __VA_ARGS__);                   \
  } जबतक(0)
#पूर्ण_अगर /* USE_SSH */
#अगर !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#घोषणा CURL_TRC_WS(तथ्य, ...)                          \
  करो {                                                  \
    अगर(Curl_trc_ft_is_verbose(तथ्य, &Curl_trc_feat_ws)) \
      Curl_trc_ws(तथ्य, __VA_ARGS__);                   \
  } जबतक(0)
#पूर्ण_अगर /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#या_अगर defined(CURL_HAVE_MACRO_VARARG) && defined(CURL_DISABLE_VERBOSE_STRINGS)

#घोषणा infof(तथ्य, ...) \
  करो {                   \
    (व्योम)(तथ्य);        \
  } जबतक(0)
#घोषणा CURL_TRC_M(तथ्य, ...) \
  करो {                        \
    (व्योम)(तथ्य);             \
  } जबतक(0)
#घोषणा CURL_TRC_CF(तथ्य, cf, ...) \
  करो {                             \
    (व्योम)(तथ्य);                  \
    (व्योम)(cf);                    \
  } जबतक(0)
#घोषणा CURL_TRC_WRITE(तथ्य, ...) \
  करो {                            \
    (व्योम)(तथ्य);                 \
  } जबतक(0)
#घोषणा CURL_TRC_READ(तथ्य, ...) \
  करो {                           \
    (व्योम)(तथ्य);                \
  } जबतक(0)
#घोषणा CURL_TRC_DNS(तथ्य, ...) \
  करो {                          \
    (व्योम)(तथ्य);               \
  } जबतक(0)
#घोषणा CURL_TRC_TIMER(तथ्य, tid, ...) \
  करो {                                 \
    (व्योम)(तथ्य);                      \
    (व्योम)(tid);                       \
  } जबतक(0)
#अगर_अघोषित CURL_DISABLE_FTP
#घोषणा CURL_TRC_FTP(तथ्य, ...) \
  करो {                          \
    (व्योम)(तथ्य);               \
  } जबतक(0)
#पूर्ण_अगर /* !CURL_DISABLE_FTP */
#अगर_अघोषित CURL_DISABLE_SMTP
#घोषणा CURL_TRC_SMTP(तथ्य, ...) \
  करो {                           \
    (व्योम)(तथ्य);                \
  } जबतक(0)
#पूर्ण_अगर /* !CURL_DISABLE_SMTP */
#अगर_घोषित USE_SSL
#घोषणा CURL_TRC_SSLS(तथ्य, ...) \
  करो {                           \
    (व्योम)(तथ्य);                \
  } जबतक(0)
#पूर्ण_अगर /* USE_SSL */
#अगर_घोषित USE_SSH
#घोषणा CURL_TRC_SSH(तथ्य, ...) \
  करो {                          \
    (व्योम)(तथ्य);               \
  } जबतक(0)
#पूर्ण_अगर /* USE_SSH */
#अगर !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#घोषणा CURL_TRC_WS(तथ्य, ...) \
  करो {                         \
    (व्योम)(तथ्य);              \
  } जबतक(0)
#पूर्ण_अगर

#अन्यथा /* !CURL_HAVE_MACRO_VARARG */

#घोषणा infof          Curl_infof
#घोषणा CURL_TRC_M     Curl_trc_multi
#घोषणा CURL_TRC_CF    Curl_trc_cf_infof
#घोषणा CURL_TRC_WRITE Curl_trc_write
#घोषणा CURL_TRC_READ  Curl_trc_read
#घोषणा CURL_TRC_DNS   Curl_trc_dns
#घोषणा CURL_TRC_TIMER Curl_trc_timer

#अगर_अघोषित CURL_DISABLE_FTP
#घोषणा CURL_TRC_FTP   Curl_trc_ftp
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_SMTP
#घोषणा CURL_TRC_SMTP  Curl_trc_smtp
#पूर्ण_अगर
#अगर_घोषित USE_SSL
#घोषणा CURL_TRC_SSLS  Curl_trc_ssls
#पूर्ण_अगर
#अगर_घोषित USE_SSH
#घोषणा CURL_TRC_SSH   Curl_trc_ssh
#पूर्ण_अगर
#अगर !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
#घोषणा CURL_TRC_WS    Curl_trc_ws
#पूर्ण_अगर

#पूर्ण_अगर /* CURL_HAVE_MACRO_VARARG */

#अगर_घोषित CURLVERBOSE
बाह्य काष्ठा curl_trc_feat Curl_trc_feat_multi;
बाह्य काष्ठा curl_trc_feat Curl_trc_feat_read;
बाह्य काष्ठा curl_trc_feat Curl_trc_feat_write;
बाह्य काष्ठा curl_trc_feat Curl_trc_feat_dns;
#अगर_अघोषित CURL_DISABLE_DOH
बाह्य काष्ठा curl_trc_feat Curl_trc_feat_doh;
#पूर्ण_अगर
बाह्य काष्ठा curl_trc_feat Curl_trc_feat_timer;
#अगर_घोषित USE_THREADS
बाह्य काष्ठा curl_trc_feat Curl_trc_feat_threads;
#पूर्ण_अगर
#पूर्ण_अगर

#अगर_अघोषित CURL_DISABLE_VERBOSE_STRINGS
/* informational messages enabled */
#घोषणा Curl_trc_is_verbose(तथ्य)                          \
  ((तथ्य) && (तथ्य)->set.verbose &&                        \
   (!(तथ्य)->state.feat ||                                 \
    ((तथ्य)->state.feat->log_level >= CURL_LOG_LVL_INFO)))
#घोषणा Curl_trc_cf_is_verbose(cf, तथ्य)              \
  (Curl_trc_is_verbose(तथ्य) &&                       \
   (cf) && (cf)->cft->log_level >= CURL_LOG_LVL_INFO)
#घोषणा Curl_trc_ft_is_verbose(तथ्य, ft) \
  (Curl_trc_is_verbose(तथ्य) &&          \
   (ft)->log_level >= CURL_LOG_LVL_INFO)
#घोषणा CURL_MSTATE_NAME(s)  Curl_trc_mstate_name((पूर्णांक)(s))
#घोषणा CURL_TRC_EASY_TIMERS(तथ्य)      \
  करो {                                  \
    अगर(CURL_TRC_TIMER_is_verbose(तथ्य)) \
      Curl_trc_easy_timers(तथ्य);       \
  } जबतक(0)

#अन्यथा /* CURL_DISABLE_VERBOSE_STRINGS */
/* All informational messages are not compiled in for size savings */
#घोषणा Curl_trc_is_verbose(d)        FALSE
#घोषणा Curl_trc_cf_is_verbose(x, y)  FALSE
#घोषणा Curl_trc_ft_is_verbose(x, y)  FALSE
#घोषणा CURL_MSTATE_NAME(x)           ((व्योम)(x), "-")
#घोषणा CURL_TRC_EASY_TIMERS(x)       Curl_nop_stmt
#पूर्ण_अगर /* !CURL_DISABLE_VERBOSE_STRINGS */

#पूर्ण_अगर /* HEADER_CURL_TRC_H */
