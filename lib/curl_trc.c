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
#समावेश "curl_setup.h"

#समावेश "curl_trc.h"
#समावेश "urldata.h"
#समावेश "cfilters.h"
#समावेश "multiif.h"

#समावेश "cf-recvbuf.h"
#समावेश "cf-socket.h"
#समावेश "cf-setup.h"
#समावेश "http2.h"
#समावेश "http_proxy.h"
#समावेश "cf-h1-proxy.h"
#समावेश "cf-h2-proxy.h"
#समावेश "cf-haproxy.h"
#समावेश "cf-https-connect.h"
#समावेश "cf-ip-happy.h"
#समावेश "progress.h"
#समावेश "socks.h"
#समावेश "curlx/strparse.h"
#समावेश "vdns/cf-dns.h"
#समावेश "vtls/vtls.h"
#समावेश "vquic/vquic.h"
#समावेश "curlx/strcopy.h"

अटल व्योम trc_write(काष्ठा Curl_easy *तथ्य, curl_infotype type,
                      स्थिर अक्षर *ptr, माप_प्रकार आकार)
{
  अगर(तथ्य->set.verbose) {
    अगर(तथ्य->set.fdebug) {
      काष्ठा Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, तथ्य, easy_fdebug);
      (व्योम)(*तथ्य->set.fdebug)(तथ्य, type, CURL_UNCONST(ptr), आकार,
                                तथ्य->set.debugdata);
      CURL_CBAPI_END(&guard);
    }
    अन्यथा {
      अटल स्थिर अक्षर s_infotype[CURLINFO_END][3] = {
        "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
      चयन(type) {
      हाल CURLINFO_TEXT:
      हाल CURLINFO_HEADER_OUT:
      हाल CURLINFO_HEADER_IN:
        ख_डालो(s_infotype[type], 2, 1, तथ्य->set.err);
        ख_डालो(ptr, आकार, 1, तथ्य->set.err);
        अवरोध;
      शेष: /* nada */
        अवरोध;
      }
    }
  }
}

/* max length we trace before ending in '...' */
#घोषणा TRC_पंक्ति_MAX 2048

#घोषणा CURL_TRC_FMT_IDSC   "[x-%" CURL_FORMAT_CURL_OFF_T "] "
#घोषणा CURL_TRC_FMT_IDSD   "[%" CURL_FORMAT_CURL_OFF_T "-x] "
#घोषणा CURL_TRC_FMT_IDSDC  "[%" CURL_FORMAT_CURL_OFF_T "-%" \
                            CURL_FORMAT_CURL_OFF_T "] "

अटल काष्ठा curl_trc_feat Curl_trc_feat_ids = {
  "LIB-IDS",
  CURL_LOG_LVL_NONE,
};
#घोषणा CURL_TRC_IDS(तथ्य) \
  (Curl_trc_is_verbose(तथ्य) && \
  Curl_trc_feat_ids.log_level >= CURL_LOG_LVL_INFO)

अटल माप_प्रकार trc_print_ids(काष्ठा Curl_easy *तथ्य, अक्षर *buf, माप_प्रकार maxlen)
{
  curl_off_t cid = तथ्य->state.last_conn_id;
  अगर(तथ्य->id >= 0) {
    अगर(cid >= 0)
      वापस curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSDC, तथ्य->id, cid);
    अन्यथा
      वापस curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSD, तथ्य->id);
  }
  अन्यथा अगर(cid >= 0)
    वापस curl_msnprintf(buf, maxlen, CURL_TRC_FMT_IDSC, cid);
  अन्यथा {
    वापस curl_msnprintf(buf, maxlen, "[x-x] ");
  }
}

अटल माप_प्रकार trc_end_buf(अक्षर *buf, माप_प्रकार len, माप_प्रकार maxlen, bool addnl)
{
  /* make sure we end the trace line in `buf` properly. It needs
   * to end with a null-terminator '\0' or '\n\0' */
  अगर(len >= (maxlen - (addnl ? 2 : 1))) {
    len = maxlen - 5;
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '.';
    buf[len++] = '\n';
  }
  अन्यथा अगर(addnl)
    buf[len++] = '\n';
  buf[len] = '\0';
  वापस len;
}

व्योम Curl_debug(काष्ठा Curl_easy *तथ्य, curl_infotype type,
                स्थिर अक्षर *ptr, माप_प्रकार आकार)
{
  अगर(तथ्य->set.verbose) {
    अटल स्थिर अक्षर s_infotype[CURLINFO_END][3] = {
      "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };
    अक्षर buf[TRC_पंक्ति_MAX];
    माप_प्रकार len;
    अगर(तथ्य->set.fdebug) {
      काष्ठा Curl_mapi_guard guard;
      अगर(CURL_TRC_IDS(तथ्य) && (आकार < TRC_पंक्ति_MAX)) {
        len = trc_print_ids(तथ्य, buf, TRC_पंक्ति_MAX);
        len += curl_msnprintf(buf + len, TRC_पंक्ति_MAX - len, "%.*s",
                              (पूर्णांक)आकार, ptr);
        len = trc_end_buf(buf, len, TRC_पंक्ति_MAX, FALSE);
        CURL_CBAPI_START(&guard, तथ्य, easy_fdebug);
        (व्योम)(*तथ्य->set.fdebug)(तथ्य, type, buf, len, तथ्य->set.debugdata);
        CURL_CBAPI_END(&guard);
      }
      अन्यथा {
        CURL_CBAPI_START(&guard, तथ्य, easy_fdebug);
        (व्योम)(*तथ्य->set.fdebug)(तथ्य, type, CURL_UNCONST(ptr),
                                  आकार, तथ्य->set.debugdata);
        CURL_CBAPI_END(&guard);
      }
    }
    अन्यथा {
      चयन(type) {
      हाल CURLINFO_TEXT:
      हाल CURLINFO_HEADER_OUT:
      हाल CURLINFO_HEADER_IN:
#अगर_अघोषित CURL_DISABLE_VERBOSE_STRINGS
        अगर(CURL_TRC_IDS(तथ्य)) {
          len = trc_print_ids(तथ्य, buf, TRC_पंक्ति_MAX);
          ख_डालो(buf, len, 1, तथ्य->set.err);
        }
#पूर्ण_अगर
        ख_डालो(s_infotype[type], 2, 1, तथ्य->set.err);
        ख_डालो(ptr, आकार, 1, तथ्य->set.err);
        अवरोध;
      शेष: /* nada */
        अवरोध;
      }
    }
  }
}

/* Curl_failf() is for messages stating why we failed.
 * The message SHALL NOT include any LF or CR.
 */
व्योम Curl_failf(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *fmt, ...)
{
  DEBUGASSERT(!म_अक्षर(fmt, '\n'));
  अगर(तथ्य->set.verbose || तथ्य->set.errorbuffer) {
    बहु_सूची ap;
    माप_प्रकार len;
    अक्षर error[CURL_ERROR_आकार + 2];
    बहु_शुरू(ap, fmt);
    len = curl_mvsnprintf(error, CURL_ERROR_आकार, fmt, ap);

    अगर(तथ्य->set.errorbuffer && !तथ्य->state.errorbuf) {
      curlx_strcopy(तथ्य->set.errorbuffer, CURL_ERROR_आकार, error, len);
      तथ्य->state.errorbuf = TRUE; /* wrote error string */
    }
    error[len++] = '\n';
    error[len] = '\0';
    trc_write(तथ्य, CURLINFO_TEXT, error, len);
    बहु_पूर्ण(ap);
  }
}

व्योम Curl_reset_fail(काष्ठा Curl_easy *तथ्य)
{
  अगर(तथ्य->set.errorbuffer)
    तथ्य->set.errorbuffer[0] = 0;
  तथ्य->state.errorbuf = FALSE;
}

#अगर_घोषित CURLVERBOSE
काष्ठा curl_trc_feat Curl_trc_feat_multi = {
  "MULTI",
  CURL_LOG_LVL_NONE,
};
काष्ठा curl_trc_feat Curl_trc_feat_read = {
  "READ",
  CURL_LOG_LVL_NONE,
};
काष्ठा curl_trc_feat Curl_trc_feat_write = {
  "WRITE",
  CURL_LOG_LVL_NONE,
};
काष्ठा curl_trc_feat Curl_trc_feat_dns = {
  "DNS",
  CURL_LOG_LVL_NONE,
};
#अगर_अघोषित CURL_DISABLE_DOH
काष्ठा curl_trc_feat Curl_trc_feat_doh = {
  "DoH",
  CURL_LOG_LVL_NONE,
};
#पूर्ण_अगर
काष्ठा curl_trc_feat Curl_trc_feat_timer = {
  "TIMER",
  CURL_LOG_LVL_NONE,
};
#अगर_घोषित USE_THREADS
काष्ठा curl_trc_feat Curl_trc_feat_threads = {
  "THREADS",
  CURL_LOG_LVL_NONE,
};
#पूर्ण_अगर
#पूर्ण_अगर

#अगर_अघोषित CURL_DISABLE_VERBOSE_STRINGS

अटल व्योम trc_infof(काष्ठा Curl_easy *तथ्य,
                      काष्ठा curl_trc_feat *feat,
                      स्थिर अक्षर *opt_id, पूर्णांक opt_id_idx,
                      स्थिर अक्षर * स्थिर fmt, बहु_सूची ap) CURL_PRINTF(5, 0);

अटल व्योम trc_infof(काष्ठा Curl_easy *तथ्य,
                      काष्ठा curl_trc_feat *feat,
                      स्थिर अक्षर *opt_id, पूर्णांक opt_id_idx,
                      स्थिर अक्षर * स्थिर fmt, बहु_सूची ap)
{
  माप_प्रकार len = 0;
  अक्षर buf[TRC_पंक्ति_MAX];

  अगर(CURL_TRC_IDS(तथ्य))
    len += trc_print_ids(तथ्य, buf + len, TRC_पंक्ति_MAX - len);
  अगर(feat)
    len += curl_msnprintf(buf + len, TRC_पंक्ति_MAX - len, "[%s] ", feat->name);
  अगर(opt_id) {
    अगर(opt_id_idx > 0)
      len += curl_msnprintf(buf + len, TRC_पंक्ति_MAX - len, "[%s-%d] ",
                            opt_id, opt_id_idx);
    अन्यथा
      len += curl_msnprintf(buf + len, TRC_पंक्ति_MAX - len, "[%s] ", opt_id);
  }
  len += curl_mvsnprintf(buf + len, TRC_पंक्ति_MAX - len, fmt, ap);
  len = trc_end_buf(buf, len, TRC_पंक्ति_MAX, TRUE);
  trc_write(तथ्य, CURLINFO_TEXT, buf, len);
}

व्योम Curl_infof(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *fmt, ...)
{
  DEBUGASSERT(!म_अक्षर(fmt, '\n'));
  अगर(Curl_trc_is_verbose(तथ्य)) {
    बहु_सूची ap;
    बहु_शुरू(ap, fmt);
    trc_infof(तथ्य, तथ्य->state.feat, शून्य, 0, fmt, ap);
    बहु_पूर्ण(ap);
  }
}

व्योम Curl_trc_cf_infof(काष्ठा Curl_easy *तथ्य, स्थिर काष्ठा Curl_cfilter *cf,
                       स्थिर अक्षर *fmt, ...)
{
  DEBUGASSERT(cf);
  अगर(Curl_trc_cf_is_verbose(cf, तथ्य)) {
    बहु_सूची ap;
    बहु_शुरू(ap, fmt);
    trc_infof(तथ्य, तथ्य->state.feat, cf->cft->name, cf->sockindex, fmt, ap);
    बहु_पूर्ण(ap);
  }
}

व्योम Curl_trc_feat_infof(काष्ठा Curl_easy *तथ्य,
                         काष्ठा curl_trc_feat *feat,
                         स्थिर अक्षर *fmt, ...)
{
  DEBUGASSERT(feat);
  अगर(Curl_trc_ft_is_verbose(तथ्य, feat)) {
    बहु_सूची ap;
    बहु_शुरू(ap, fmt);
    trc_infof(तथ्य, feat, शून्य, 0, fmt, ap);
    बहु_पूर्ण(ap);
  }
}

अटल स्थिर अक्षर * स्थिर Curl_trc_timer_names[] = {
  "100_TIMEOUT",
  "ASYNC_NAME",
  "CONNECTTIMEOUT",
  "HAPPY_EYEBALLS",
  "MULTI_PENDING",
  "SPEEDCHECK",
  "TIMEOUT",
  "TOOFAST",
  "QUIC",
  "FTP_ACCEPT",
  "ALPN_EYEBALLS",
  "SHUTDOWN",
  "SERVER_RESPONSE",
};

अटल स्थिर अक्षर *trc_timer_name(पूर्णांक tid)
{
  अगर((tid >= 0) && ((माप_प्रकार)tid < CURL_ARRAYSIZE(Curl_trc_timer_names)))
    वापस Curl_trc_timer_names[(माप_प्रकार)tid];
  वापस "TIMER-???";
}

व्योम Curl_trc_timer(काष्ठा Curl_easy *तथ्य, पूर्णांक tid, स्थिर अक्षर *fmt, ...)
{
  DEBUGASSERT(!म_अक्षर(fmt, '\n'));
  अगर(Curl_trc_ft_is_verbose(तथ्य, &Curl_trc_feat_timer)) {
    स्थिर अक्षर *tname = trc_timer_name(tid);
    बहु_सूची ap;
    बहु_शुरू(ap, fmt);
    trc_infof(तथ्य, &Curl_trc_feat_timer, tname, 0, fmt, ap);
    बहु_पूर्ण(ap);
  }
}

व्योम Curl_trc_easy_timers(काष्ठा Curl_easy *तथ्य)
{
  अगर(CURL_TRC_TIMER_is_verbose(तथ्य) && तथ्य->multi) {
    अगर(तथ्य->state.timeouts.first < EXPIRE_LAST) {
      काष्ठा expire_timers *timeouts = &तथ्य->state.timeouts;
      timediff_t base_us =
        Curl_timeouts_offset_us(&तथ्य->multi->timeouts, Curl_pgrs_now(तथ्य));
      uint8_t id = तथ्य->state.timeouts.first;
      क्रम(; id < EXPIRE_LAST; id = timeouts->अग्र[id]) {
        CURL_TRC_TIMER(तथ्य, id, "expires in %" FMT_TIMEDIFF_T "us",
                       timeouts->offset_us[id] - base_us);
      }
    }
  }
}

अटल स्थिर अक्षर * स्थिर Curl_trc_mstate_names[] = {
  "INIT",
  "PENDING",
  "SETUP",
  "CONNECT",
  "CONNECTING",
  "PROTOCONNECT",
  "PROTOCONNECTING",
  "DO",
  "DOING",
  "DOING_MORE",
  "DID",
  "PERFORMING",
  "RATELIMITING",
  "DONE",
  "COMPLETED",
  "MSGSENT",
};

स्थिर अक्षर *Curl_trc_mstate_name(पूर्णांक state)
{
  अगर((state >= 0) && ((माप_प्रकार)state < CURL_ARRAYSIZE(Curl_trc_mstate_names)))
    वापस Curl_trc_mstate_names[(माप_प्रकार)state];
  वापस "?";
}

व्योम Curl_trc_multi(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *fmt, ...)
{
  DEBUGASSERT(!म_अक्षर(fmt, '\n'));
  अगर(Curl_trc_ft_is_verbose(तथ्य, &Curl_trc_feat_multi)) {
    स्थिर अक्षर *sname = (तथ्य->id >= 0) ?
                        Curl_trc_mstate_name(तथ्य->mstate) : शून्य;
    बहु_सूची ap;
    बहु_शुरू(ap, fmt);
    trc_infof(तथ्य, &Curl_trc_feat_multi, sname, 0, fmt, ap);
    बहु_पूर्ण(ap);
  }
}

व्योम Curl_trc_read(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *fmt, ...)
{
  DEBUGASSERT(!म_अक्षर(fmt, '\n'));
  अगर(Curl_trc_ft_is_verbose(तथ्य, &Curl_trc_feat_read)) {
    बहु_सूची ap;
    बहु_शुरू(ap, fmt);
    trc_infof(तथ्य, &Curl_trc_feat_read, शून्य, 0, fmt, ap);
    बहु_पूर्ण(ap);
  }
}

व्योम Curl_trc_write(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *fmt, ...)
{
  DEBUGASSERT(!म_अक्षर(fmt, '\n'));
  अगर(Curl_trc_ft_is_verbose(तथ्य, &Curl_trc_feat_write)) {
    बहु_सूची ap;
    बहु_शुरू(ap, fmt);
    trc_infof(तथ्य, &Curl_trc_feat_write, शून्य, 0, fmt, ap);
    बहु_पूर्ण(ap);
  }
}

व्योम Curl_trc_dns(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *fmt, ...)
{
  DEBUGASSERT(!म_अक्षर(fmt, '\n'));
  अगर(Curl_trc_ft_is_verbose(तथ्य, &Curl_trc_feat_dns)) {
    बहु_सूची ap;
    बहु_शुरू(ap, fmt);
    trc_infof(तथ्य, &Curl_trc_feat_dns, शून्य, 0, fmt, ap);
    बहु_पूर्ण(ap);
  }
}

#अगर_अघोषित CURL_DISABLE_FTP
काष्ठा curl_trc_feat Curl_trc_feat_ftp = {
  "FTP",
  CURL_LOG_LVL_NONE,
};

व्योम Curl_trc_ftp(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *fmt, ...)
{
  DEBUGASSERT(!म_अक्षर(fmt, '\n'));
  अगर(Curl_trc_ft_is_verbose(तथ्य, &Curl_trc_feat_ftp)) {
    बहु_सूची ap;
    बहु_शुरू(ap, fmt);
    trc_infof(तथ्य, &Curl_trc_feat_ftp, शून्य, 0, fmt, ap);
    बहु_पूर्ण(ap);
  }
}
#पूर्ण_अगर /* !CURL_DISABLE_FTP */

#अगर_अघोषित CURL_DISABLE_SMTP
काष्ठा curl_trc_feat Curl_trc_feat_smtp = {
  "SMTP",
  CURL_LOG_LVL_NONE,
};

व्योम Curl_trc_smtp(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *fmt, ...)
{
  DEBUGASSERT(!म_अक्षर(fmt, '\n'));
  अगर(Curl_trc_ft_is_verbose(तथ्य, &Curl_trc_feat_smtp)) {
    बहु_सूची ap;
    बहु_शुरू(ap, fmt);
    trc_infof(तथ्य, &Curl_trc_feat_smtp, शून्य, 0, fmt, ap);
    बहु_पूर्ण(ap);
  }
}
#पूर्ण_अगर /* !CURL_DISABLE_SMTP */

#अगर_घोषित USE_SSL
काष्ठा curl_trc_feat Curl_trc_feat_ssls = {
  "SSLS",
  CURL_LOG_LVL_NONE,
};

व्योम Curl_trc_ssls(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *fmt, ...)
{
  DEBUGASSERT(!म_अक्षर(fmt, '\n'));
  अगर(Curl_trc_ft_is_verbose(तथ्य, &Curl_trc_feat_ssls)) {
    बहु_सूची ap;
    बहु_शुरू(ap, fmt);
    trc_infof(तथ्य, &Curl_trc_feat_ssls, शून्य, 0, fmt, ap);
    बहु_पूर्ण(ap);
  }
}
#पूर्ण_अगर /* USE_SSL */

#अगर_घोषित USE_SSH
काष्ठा curl_trc_feat Curl_trc_feat_ssh = {
  "SSH",
  CURL_LOG_LVL_NONE,
};

व्योम Curl_trc_ssh(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *fmt, ...)
{
  DEBUGASSERT(!म_अक्षर(fmt, '\n'));
  अगर(Curl_trc_ft_is_verbose(तथ्य, &Curl_trc_feat_ssh)) {
    बहु_सूची ap;
    बहु_शुरू(ap, fmt);
    trc_infof(तथ्य, &Curl_trc_feat_ssh, शून्य, 0, fmt, ap);
    बहु_पूर्ण(ap);
  }
}
#पूर्ण_अगर /* USE_SSH */

#अगर !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
काष्ठा curl_trc_feat Curl_trc_feat_ws = {
  "WS",
  CURL_LOG_LVL_NONE,
};

व्योम Curl_trc_ws(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *fmt, ...)
{
  DEBUGASSERT(!म_अक्षर(fmt, '\n'));
  अगर(Curl_trc_ft_is_verbose(तथ्य, &Curl_trc_feat_ws)) {
    बहु_सूची ap;
    बहु_शुरू(ap, fmt);
    trc_infof(तथ्य, &Curl_trc_feat_ws, शून्य, 0, fmt, ap);
    बहु_पूर्ण(ap);
  }
}
#पूर्ण_अगर /* !CURL_DISABLE_WEBSOCKETS && !CURL_DISABLE_HTTP */

#घोषणा TRC_CT_NONE        0
#घोषणा TRC_CT_PROTOCOL    (1 << 0)
#घोषणा TRC_CT_NETWORK     (1 << 1)
#घोषणा TRC_CT_PROXY       (1 << 2)
#घोषणा TRC_CT_INTERNALS   (1 << 3)

काष्ठा trc_feat_def {
  काष्ठा curl_trc_feat *feat;
  अचिन्हित पूर्णांक category;
};

अटल काष्ठा trc_feat_def trc_feats[] = {
  { &Curl_trc_feat_ids,       TRC_CT_INTERNALS },
  { &Curl_trc_feat_multi,     TRC_CT_NETWORK },
  { &Curl_trc_feat_read,      TRC_CT_NONE },
  { &Curl_trc_feat_write,     TRC_CT_NONE },
  { &Curl_trc_feat_dns,       TRC_CT_NETWORK },
#अगर_अघोषित CURL_DISABLE_DOH
  { &Curl_trc_feat_doh,       TRC_CT_NETWORK },
#पूर्ण_अगर
  { &Curl_trc_feat_timer,     TRC_CT_NETWORK },
#अगर_घोषित USE_THREADS
  { &Curl_trc_feat_threads,   TRC_CT_NONE },
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_FTP
  { &Curl_trc_feat_ftp,       TRC_CT_PROTOCOL },
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_SMTP
  { &Curl_trc_feat_smtp,      TRC_CT_PROTOCOL },
#पूर्ण_अगर
#अगर_घोषित USE_SSL
  { &Curl_trc_feat_ssls,      TRC_CT_NETWORK },
#पूर्ण_अगर
#अगर_घोषित USE_SSH
  { &Curl_trc_feat_ssh,       TRC_CT_PROTOCOL },
#पूर्ण_अगर
#अगर !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
  { &Curl_trc_feat_ws,        TRC_CT_PROTOCOL },
#पूर्ण_अगर
};

काष्ठा trc_cft_def {
  काष्ठा Curl_cftype *cft;
  अचिन्हित पूर्णांक category;
};

अटल काष्ठा trc_cft_def trc_cfts[] = {
  { &Curl_cft_dns,            TRC_CT_NETWORK },
  { &Curl_cft_tcp,            TRC_CT_NETWORK },
  { &Curl_cft_udp,            TRC_CT_NETWORK },
  { &Curl_cft_unix,           TRC_CT_NETWORK },
  { &Curl_cft_tcp_accept,     TRC_CT_NETWORK },
  { &Curl_cft_ip_happy,       TRC_CT_NETWORK },
#अगर_अघोषित CURL_DISABLE_WEBSOCKETS
  { &Curl_cft_recvbuf,        TRC_CT_PROTOCOL },
#पूर्ण_अगर
  { &Curl_cft_setup,          TRC_CT_PROTOCOL },
#अगर !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  { &Curl_cft_nghttp2,        TRC_CT_PROTOCOL },
#पूर्ण_अगर
#अगर_घोषित USE_SSL
  { &Curl_cft_ssl,            TRC_CT_NETWORK },
#अगर_अघोषित CURL_DISABLE_PROXY
  { &Curl_cft_ssl_proxy,      TRC_CT_PROXY },
#पूर्ण_अगर
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_PROXY
#अगर_अघोषित CURL_DISABLE_HTTP
  { &Curl_cft_h1_proxy,       TRC_CT_PROXY },
#अगर_घोषित USE_NGHTTP2
  { &Curl_cft_h2_proxy,       TRC_CT_PROXY },
#पूर्ण_अगर
#अगर defined(USE_PROXY_HTTP3) && defined(USE_NGHTTP3)
  { &Curl_cft_h3_proxy,       TRC_CT_PROXY },
#पूर्ण_अगर
  { &Curl_cft_http_proxy,     TRC_CT_PROXY },
#पूर्ण_अगर /* !CURL_DISABLE_HTTP */
  { &Curl_cft_haproxy,        TRC_CT_PROXY },
  { &Curl_cft_socks_proxy,    TRC_CT_PROXY },
#पूर्ण_अगर /* !CURL_DISABLE_PROXY */
#अगर !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  { &Curl_cft_http3,          TRC_CT_PROTOCOL },
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_HTTP
  { &Curl_cft_http_connect,   TRC_CT_PROTOCOL },
#पूर्ण_अगर
};

अटल व्योम trc_apply_level_by_name(काष्ठा Curl_str *token, पूर्णांक lvl)
{
  माप_प्रकार i;

  क्रम(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    अगर(curlx_str_casecompare(token, trc_cfts[i].cft->name)) {
      trc_cfts[i].cft->log_level = lvl;
      अवरोध;
    }
  }
  क्रम(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    अगर(curlx_str_casecompare(token, trc_feats[i].feat->name)) {
      trc_feats[i].feat->log_level = lvl;
      अवरोध;
    }
  }
}

अटल व्योम trc_apply_level_by_category(अचिन्हित पूर्णांक category, पूर्णांक lvl)
{
  माप_प्रकार i;

  क्रम(i = 0; i < CURL_ARRAYSIZE(trc_cfts); ++i) {
    अगर(!category || (trc_cfts[i].category & category))
      trc_cfts[i].cft->log_level = lvl;
  }
  क्रम(i = 0; i < CURL_ARRAYSIZE(trc_feats); ++i) {
    अगर(!category || (trc_feats[i].category & category))
      trc_feats[i].feat->log_level = lvl;
  }
}

अटल CURLcode trc_opt(स्थिर अक्षर *config)
{
  काष्ठा Curl_str out;
  जबतक(!curlx_str_until(&config, &out, 32, ',')) {
    पूर्णांक lvl = CURL_LOG_LVL_INFO;
    स्थिर अक्षर *token = curlx_str(&out);

    अगर(*token == '-') {
      lvl = CURL_LOG_LVL_NONE;
      curlx_str_nudge(&out, 1);
    }
    अन्यथा अगर(*token == '+')
      curlx_str_nudge(&out, 1);

    अगर(curlx_str_casecompare(&out, "all"))
      trc_apply_level_by_category(TRC_CT_NONE, lvl);
    अन्यथा अगर(curlx_str_casecompare(&out, "protocol"))
      trc_apply_level_by_category(TRC_CT_PROTOCOL, lvl);
    अन्यथा अगर(curlx_str_casecompare(&out, "network"))
      trc_apply_level_by_category(TRC_CT_NETWORK, lvl);
    अन्यथा अगर(curlx_str_casecompare(&out, "proxy"))
      trc_apply_level_by_category(TRC_CT_PROXY, lvl);
    अन्यथा
      trc_apply_level_by_name(&out, lvl);

    अगर(curlx_str_single(&config, ','))
      अवरोध;
  }
  वापस CURLE_OK;
}

CURLcode Curl_trc_opt(स्थिर अक्षर *config)
{
  CURLcode परिणाम = config ? trc_opt(config) : CURLE_OK;
#अगर_घोषित DEBUGBUILD
  /* CURL_DEBUG can override anything */
  अगर(!परिणाम) {
    स्थिर अक्षर *dbg_config = दो_पर्या("CURL_DEBUG");
    अगर(dbg_config)
      परिणाम = trc_opt(dbg_config);
  }
#पूर्ण_अगर /* DEBUGBUILD */
  वापस परिणाम;
}

CURLcode Curl_trc_init(व्योम)
{
#अगर_घोषित DEBUGBUILD
  वापस Curl_trc_opt(शून्य);
#अन्यथा
  वापस CURLE_OK;
#पूर्ण_अगर
}

#अन्यथा /* CURL_DISABLE_VERBOSE_STRINGS */

CURLcode Curl_trc_init(व्योम)
{
  वापस CURLE_OK;
}

व्योम Curl_infof(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *fmt, ...)
{
  (व्योम)तथ्य;
  (व्योम)fmt;
}

व्योम Curl_trc_cf_infof(काष्ठा Curl_easy *तथ्य, स्थिर काष्ठा Curl_cfilter *cf,
                       स्थिर अक्षर *fmt, ...)
{
  (व्योम)तथ्य;
  (व्योम)cf;
  (व्योम)fmt;
}

व्योम Curl_trc_multi(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *fmt, ...)
{
  (व्योम)तथ्य;
  (व्योम)fmt;
}

व्योम Curl_trc_write(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *fmt, ...)
{
  (व्योम)तथ्य;
  (व्योम)fmt;
}

व्योम Curl_trc_dns(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *fmt, ...)
{
  (व्योम)तथ्य;
  (व्योम)fmt;
}

व्योम Curl_trc_timer(काष्ठा Curl_easy *तथ्य, पूर्णांक tid, स्थिर अक्षर *fmt, ...)
{
  (व्योम)तथ्य;
  (व्योम)tid;
  (व्योम)fmt;
}

व्योम Curl_trc_read(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *fmt, ...)
{
  (व्योम)तथ्य;
  (व्योम)fmt;
}

#अगर_अघोषित CURL_DISABLE_FTP
व्योम Curl_trc_ftp(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *fmt, ...)
{
  (व्योम)तथ्य;
  (व्योम)fmt;
}
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_SMTP
व्योम Curl_trc_smtp(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *fmt, ...)
{
  (व्योम)तथ्य;
  (व्योम)fmt;
}
#पूर्ण_अगर
#अगर !defined(CURL_DISABLE_WEBSOCKETS) && !defined(CURL_DISABLE_HTTP)
व्योम Curl_trc_ws(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *fmt, ...)
{
  (व्योम)तथ्य;
  (व्योम)fmt;
}
#पूर्ण_अगर
#अगर_घोषित USE_SSH
व्योम Curl_trc_ssh(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *fmt, ...)
{
  (व्योम)तथ्य;
  (व्योम)fmt;
}
#पूर्ण_अगर
#अगर_घोषित USE_SSL
व्योम Curl_trc_ssls(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *fmt, ...)
{
  (व्योम)तथ्य;
  (व्योम)fmt;
}
#पूर्ण_अगर

#पूर्ण_अगर /* !CURL_DISABLE_VERBOSE_STRINGS */
