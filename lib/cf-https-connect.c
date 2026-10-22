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

#अगर_अघोषित CURL_DISABLE_HTTP

#समावेश "urldata.h"
#समावेश "curl_trc.h"
#समावेश "cfilters.h"
#समावेश "cf-setup.h"
#समावेश "connect.h"
#समावेश "multiif.h"
#समावेश "cf-https-connect.h"
#समावेश "http2.h"
#समावेश "progress.h"
#समावेश "select.h"
#समावेश "vdns/cf-dns.h"
#समावेश "vdns/hostip.h"
#समावेश "vdns/httpsrr.h"
#समावेश "vquic/vquic.h"

प्रकार क्रमागत {
  CF_HC_RESOLV,
  CF_HC_INIT,
  CF_HC_CONNECT,
  CF_HC_SUCCESS,
  CF_HC_FAILURE
} cf_hc_state;

काष्ठा cf_hc_baller {
  स्थिर अक्षर *name;
  काष्ठा Curl_cfilter *cf;
  CURLcode परिणाम;
  पूर्णांक reply_ms;
  uint8_t transport;
  क्रमागत alpnid alpn_id;
  BIT(shutdown);
};

अटल व्योम cf_hc_baller_discard(काष्ठा cf_hc_baller *b,
                                 काष्ठा Curl_easy *तथ्य)
{
  अगर(b->cf) {
    Curl_conn_cf_discard_chain(&b->cf, तथ्य);
    b->cf = शून्य;
  }
}

अटल bool cf_hc_baller_is_connecting(काष्ठा cf_hc_baller *b)
{
  वापस b->cf && !b->परिणाम;
}

अटल bool cf_hc_baller_has_started(काष्ठा cf_hc_baller *b)
{
  वापस !!b->cf;
}

अटल पूर्णांक cf_hc_baller_reply_ms(काष्ठा cf_hc_baller *b,
                                 काष्ठा Curl_easy *तथ्य)
{
  अगर(b->cf && (b->reply_ms < 0))
    b->cf->cft->query(b->cf, तथ्य, CF_QUERY_CONNECT_REPLY_MS,
                      &b->reply_ms, शून्य);
  वापस b->reply_ms;
}

अटल bool cf_hc_baller_तथ्य_pending(काष्ठा cf_hc_baller *b,
                                      स्थिर काष्ठा Curl_easy *तथ्य)
{
  वापस b->cf && !b->परिणाम && b->cf->cft->has_तथ्य_pending(b->cf, तथ्य);
}

अटल bool cf_hc_baller_needs_flush(काष्ठा cf_hc_baller *b,
                                     काष्ठा Curl_easy *तथ्य)
{
  वापस b->cf && !b->परिणाम && Curl_conn_cf_needs_flush(b->cf, तथ्य);
}

अटल CURLcode cf_hc_baller_cntrl(काष्ठा cf_hc_baller *b,
                                   काष्ठा Curl_easy *तथ्य,
                                   पूर्णांक event, पूर्णांक arg1, व्योम *arg2)
{
  अगर(b->cf && !b->परिणाम)
    वापस Curl_conn_cf_cntrl(b->cf, तथ्य, FALSE, event, arg1, arg2);
  वापस CURLE_OK;
}

काष्ठा cf_hc_ctx {
  cf_hc_state state;
  काष्ठा Curl_peer *destination; /* who we ultimately want to talk to */
  काष्ठा curltime started;  /* when connect started */
  CURLcode परिणाम;          /* overall result */
  CURLcode check_h3_परिणाम;
  काष्ठा cf_hc_baller ballers[2];
  माप_प्रकार baller_गिनती;
  timediff_t soft_eyeballs_timeout_ms;
  timediff_t hard_eyeballs_timeout_ms;
  uint8_t def_transport;
  BIT(httpsrr_resolved);
  BIT(checked_h3);
  BIT(ballers_complete);
};

अटल व्योम cf_hc_ctx_destroy(काष्ठा Curl_easy *तथ्य,
                              काष्ठा cf_hc_ctx *ctx)
{
  अगर(ctx) {
    माप_प्रकार i;
    क्रम(i = 0; i < ctx->baller_गिनती; ++i)
      cf_hc_baller_discard(&ctx->ballers[i], तथ्य);
    Curl_peer_unlink(&ctx->destination);
    curlx_free(ctx);
  }
}

अटल व्योम cf_hc_baller_assign(काष्ठा cf_hc_baller *b,
                                क्रमागत alpnid alpn_id,
                                uint8_t def_transport)
{
  b->alpn_id = alpn_id;
  b->transport = def_transport;
  b->cf = शून्य;
  b->परिणाम = CURLE_OK;
  b->reply_ms = -1;
  b->shutdown = FALSE;
  चयन(b->alpn_id) {
  हाल ALPN_h3:
    b->name = "h3";
    b->transport = TRNSPRT_QUIC;
    अवरोध;
  हाल ALPN_h2:
    b->name = "h2";
    अवरोध;
  हाल ALPN_h1:
    b->name = "h1";
    अवरोध;
  हाल ALPN_none:
    b->name = "no-alpn";
    अवरोध;
  शेष:
    b->परिणाम = CURLE_FAILED_INIT;
    अवरोध;
  }
}

अटल व्योम cf_hc_baller_init(काष्ठा cf_hc_baller *b,
                              काष्ठा Curl_cfilter *cf,
                              काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_cfilter *save = cf->अग्र;

  cf->अग्र = शून्य;
  b->परिणाम = Curl_cf_setup_जोड़ो_after(cf, तथ्य, b->transport,
                                         CURL_CF_SSL_ENABLE);
  b->cf = cf->अग्र;
  cf->अग्र = save;
}

अटल CURLcode cf_hc_baller_connect(काष्ठा cf_hc_baller *b,
                                     काष्ठा Curl_cfilter *cf,
                                     काष्ठा Curl_easy *तथ्य,
                                     bool *done)
{
  काष्ठा Curl_cfilter *save = cf->अग्र;

  cf->अग्र = b->cf;
  b->परिणाम = Curl_conn_cf_connect(cf->अग्र, तथ्य, done);
  b->cf = cf->अग्र; /* it might mutate */
  cf->अग्र = save;
  वापस b->परिणाम;
}

अटल CURLcode baller_connected(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 काष्ठा cf_hc_baller *winner)
{
  काष्ठा cf_hc_ctx *ctx = cf->ctx;

  /* Make the winner's connection filter out own sub-filter, check, move,
   * close all remaining. */
  अगर(cf->अग्र) {
    DEBUGASSERT(0);
    वापस CURLE_FAILED_INIT;
  }
  अगर(!winner->cf) {
    DEBUGASSERT(0);
    वापस CURLE_FAILED_INIT;
  }

  cf->अग्र = winner->cf;
  winner->cf = शून्य;
  ctx->state = CF_HC_SUCCESS;
  cf->connected = TRUE;

  /* ballers may have failf()'d, the winner resets it, so our
   * errorbuf is clean again. */
  Curl_reset_fail(तथ्य);

#अगर_घोषित USE_NGHTTP2
  {
    /* For a negotiated HTTP/2 connection insert the h2 filter. */
    स्थिर अक्षर *alpn = Curl_conn_cf_get_alpn_negotiated(cf->अग्र, तथ्य);
    अगर(alpn && !म_भेद("h2", alpn)) {
      CURLcode परिणाम = Curl_http2_switch_at(cf, तथ्य);
      अगर(परिणाम) {
        ctx->state = CF_HC_FAILURE;
        ctx->परिणाम = परिणाम;
        वापस परिणाम;
      }
    }
  }
#पूर्ण_अगर
  वापस CURLE_OK;
}

अटल bool time_to_start_baller2(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य,
                                  स्थिर काष्ठा curltime *pnow)
{
  काष्ठा cf_hc_ctx *ctx = cf->ctx;
  timediff_t elapsed_ms;

  अगर(ctx->baller_गिनती < 2)
    वापस FALSE;
  अन्यथा अगर(cf_hc_baller_has_started(&ctx->ballers[1]))
    वापस FALSE;
  अन्यथा अगर(ctx->ballers[0].परिणाम) {
    CURL_TRC_CF(तथ्य, cf, "%s baller failed, starting %s",
                ctx->ballers[0].name, ctx->ballers[1].name);
    वापस TRUE;
  }

  elapsed_ms = curlx_ptimediff_ms(pnow, &ctx->started);
  अगर(elapsed_ms >= ctx->hard_eyeballs_timeout_ms) {
    CURL_TRC_CF(तथ्य, cf, "%s inconclusive after %" FMT_TIMEDIFF_T ", "
                "starting %s", ctx->ballers[0].name,
                ctx->hard_eyeballs_timeout_ms, ctx->ballers[1].name);
    वापस TRUE;
  }
  अन्यथा अगर(elapsed_ms >= ctx->soft_eyeballs_timeout_ms) {
    अगर(cf_hc_baller_reply_ms(&ctx->ballers[0], तथ्य) < 0) {
      CURL_TRC_CF(तथ्य, cf, "%s has not seen any data after %"
                  FMT_TIMEDIFF_T "ms, starting %s",
                  ctx->ballers[0].name, ctx->soft_eyeballs_timeout_ms,
                  ctx->ballers[1].name);
      वापस TRUE;
    }
    /* set the effective hard timeout again */
    Curl_expire_set(तथ्य, EXPIRE_ALPN_EYEBALLS,
                    ctx->hard_eyeballs_timeout_ms - elapsed_ms, pnow);
  }
  वापस FALSE;
}

अटल bool cf_hc_may_h3(काष्ठा Curl_cfilter *cf,
                         काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_hc_ctx *ctx = cf->ctx;
  अगर(!ctx->checked_h3) {
    ctx->check_h3_परिणाम =
      Curl_conn_may_http3(तथ्य, cf->conn);
    ctx->checked_h3 = TRUE;
  }
  वापस !ctx->check_h3_परिणाम;
}

अटल क्रमागत alpnid cf_hc_get_httpsrr_alpn(काष्ठा Curl_cfilter *cf,
                                          काष्ठा Curl_easy *तथ्य,
                                          क्रमागत alpnid not_this_one)
{
#अगर_घोषित USE_HTTPSRR
  काष्ठा cf_hc_ctx *ctx = cf->ctx;
  /* Is there an HTTPSRR use its ALPNs here.
   * We are here after having selected a connection to a host+port and
   * can no longer change that. Any HTTPSRR advice for other hosts and ports
   * we need to ignore. */
  स्थिर काष्ठा Curl_https_rrinfo *rr;
  माप_प्रकार i;

  /* Do we have HTTPS-RR information? */
  rr = Curl_conn_dns_get_https(तथ्य, cf->sockindex, ctx->destination);

  CURL_TRC_CF(तथ्य, cf, "HTTPS-RR %savailable", rr ? "" : "not ");
  /* We do not support `rr->no_def_alpn`. */
  अगर(Curl_httpsrr_applicable(तथ्य, rr) && !rr->no_def_alpn) {
    क्रम(i = 0; i < CURL_ARRAYSIZE(rr->alpns); ++i) {
      क्रमागत alpnid alpn_rr = (क्रमागत alpnid)rr->alpns[i];
      अगर(alpn_rr == not_this_one) /* do not want this one */
        जारी;
      चयन(alpn_rr) {
      हाल ALPN_h3:
        अगर((तथ्य->state.http_neg.allowed & CURL_HTTP_V3x) &&
           cf_hc_may_h3(cf, तथ्य)) {
          वापस alpn_rr;
        }
        अवरोध;
      हाल ALPN_h2:
        अगर(तथ्य->state.http_neg.allowed & CURL_HTTP_V2x) {
          वापस alpn_rr;
        }
        अवरोध;
      हाल ALPN_h1:
        अगर(तथ्य->state.http_neg.allowed & CURL_HTTP_V1x) {
          वापस alpn_rr;
        }
        अवरोध;
      शेष: /* ignore */
        अवरोध;
      }
    }
  }
#अन्यथा
  (व्योम)cf;
  (व्योम)तथ्य;
  (व्योम)not_this_one;
#पूर्ण_अगर
  वापस ALPN_none;
}

अटल क्रमागत alpnid cf_hc_get_pref_alpn(काष्ठा Curl_cfilter *cf,
                                       काष्ठा Curl_easy *तथ्य,
                                       क्रमागत alpnid not_this_one)
{
  अगर((तथ्य->state.http_neg.preferred & तथ्य->state.http_neg.allowed)) {
    चयन(तथ्य->state.http_neg.preferred) {
    हाल CURL_HTTP_V3x:
      अगर(cf_hc_may_h3(cf, तथ्य) && (ALPN_h3 != not_this_one))
        वापस ALPN_h3;
      अवरोध;
    हाल CURL_HTTP_V2x:
      अगर(ALPN_h2 != not_this_one)
        वापस ALPN_h2;
      अवरोध;
    हाल CURL_HTTP_V1x:
      /* If we are trying h2 already, h1 is already used as fallback */
      अगर((ALPN_h1 != not_this_one) && (ALPN_h2 != not_this_one))
        वापस ALPN_h1;
      अवरोध;
    शेष:
      अवरोध;
    }
  }
  वापस ALPN_none;
}

अटल क्रमागत alpnid cf_hc_get_first_alpn(काष्ठा Curl_cfilter *cf,
                                        काष्ठा Curl_easy *तथ्य,
                                        http_majors choices,
                                        क्रमागत alpnid not_this_one)
{
  /* When told to not try h2, we also do not try h1 and vice versa */
  bool allow_h1_or_h2 = (not_this_one != ALPN_h1) &&
                        (not_this_one != ALPN_h2);
  अगर((ALPN_h3 != not_this_one) && (choices & CURL_HTTP_V3x) &&
     cf_hc_may_h3(cf, तथ्य)) {
    वापस ALPN_h3;
  }
  अगर(allow_h1_or_h2 && (choices & CURL_HTTP_V2x)) {
    वापस ALPN_h2;
  }
  अगर(allow_h1_or_h2 && (choices & CURL_HTTP_V1x)) {
    वापस ALPN_h1;
  }
  वापस ALPN_none;
}

अटल CURLcode cf_hc_set_baller1(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_hc_ctx *ctx = cf->ctx;
  क्रमागत alpnid alpn1 = ALPN_none;
  VERBOSE(स्थिर अक्षर *source = "HTTPS-RR");

  DEBUGASSERT(cf->conn->bits.tls_enable_alpn);

  alpn1 = cf_hc_get_httpsrr_alpn(cf, तथ्य, ALPN_none);
  अगर(alpn1 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(source = "preferred version");
    alpn1 = cf_hc_get_pref_alpn(cf, तथ्य, ALPN_none);
  }
  अगर(alpn1 == ALPN_none) {
    VERBOSE(source = "wanted versions");
    alpn1 = cf_hc_get_first_alpn(cf, तथ्य,
                                 तथ्य->state.http_neg.wanted,
                                 ALPN_none);
  }
  अगर(alpn1 == ALPN_none) {
    VERBOSE(source = "allowed versions");
    alpn1 = cf_hc_get_first_alpn(cf, तथ्य,
                                 तथ्य->state.http_neg.allowed,
                                 ALPN_none);
  }

  अगर(alpn1 == ALPN_none) {
    /* None of the wanted/allowed HTTP versions could be chosen */
    अगर(ctx->check_h3_परिणाम) {
      CURL_TRC_CF(तथ्य, cf, "unable to use HTTP/3");
      वापस ctx->check_h3_परिणाम;
    }
    CURL_TRC_CF(तथ्य, cf, "unable to select HTTP version");
    वापस CURLE_FAILED_INIT;
  }

  cf_hc_baller_assign(&ctx->ballers[0], alpn1, ctx->def_transport);
  ctx->baller_गिनती = 1;
  CURL_TRC_CF(तथ्य, cf, "1st attempt uses %s from %s",
              ctx->ballers[0].name, source);

  चयन(alpn1) {
  हाल ALPN_h1:
    /* We really want h1, switch off h2 to make it disappear in ALPN */
    तथ्य->state.http_neg.wanted &= (uint8_t)~CURL_HTTP_V2x;
    अवरोध;
  शेष:
    अवरोध;
  }

  वापस CURLE_OK;
}

अटल व्योम cf_hc_set_baller2(काष्ठा Curl_cfilter *cf,
                              काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_hc_ctx *ctx = cf->ctx;
  क्रमागत alpnid alpn2 = ALPN_none, alpn1 = ctx->ballers[0].alpn_id;
  VERBOSE(स्थिर अक्षर *source = "HTTPS-RR");

  अगर(ctx->ballers_complete)
    वापस; /* already done */
  अगर(!ctx->httpsrr_resolved)
    वापस; /* HTTPS-RR pending */

  alpn2 = cf_hc_get_httpsrr_alpn(cf, तथ्य, alpn1);
  अगर(alpn2 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(source = "preferred version");
    alpn2 = cf_hc_get_pref_alpn(cf, तथ्य, alpn1);
  }
  अगर(alpn2 == ALPN_none) {
    VERBOSE(source = "wanted versions");
    alpn2 = cf_hc_get_first_alpn(cf, तथ्य,
                                 तथ्य->state.http_neg.wanted,
                                 alpn1);
  }

  अगर(alpn2 != ALPN_none) {
    cf_hc_baller_assign(&ctx->ballers[1], alpn2, ctx->def_transport);
    ctx->baller_गिनती = 2;
    CURL_TRC_CF(तथ्य, cf, "2nd attempt uses %s from %s",
                ctx->ballers[1].name, source);
  }
  ctx->ballers_complete = TRUE;
}

अटल CURLcode cf_hc_connect(काष्ठा Curl_cfilter *cf,
                              काष्ठा Curl_easy *तथ्य,
                              bool *done)
{
  काष्ठा cf_hc_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;
  स्थिर काष्ठा curltime *pnow = शून्य;

  अगर(cf->connected) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  *done = FALSE;

  अगर(!ctx->httpsrr_resolved) {
    ctx->httpsrr_resolved = Curl_conn_dns_resolved_https(
      तथ्य, cf->sockindex, ctx->destination);
#अगर_घोषित DEBUGBUILD
    अगर(!ctx->httpsrr_resolved && दो_पर्या("CURL_DBG_AWAIT_HTTPSRR")) {
      CURL_TRC_CF(तथ्य, cf, "awaiting HTTPS-RR");
      वापस CURLE_OK;
    }
#पूर्ण_अगर
  }

  चयन(ctx->state) {
  हाल CF_HC_RESOLV:
    ctx->state = CF_HC_INIT;
    FALLTHROUGH();

  हाल CF_HC_INIT:
    DEBUGASSERT(!cf->अग्र);
    CURL_TRC_CF(तथ्य, cf, "connect, init");
    परिणाम = cf_hc_set_baller1(cf, तथ्य);
    अगर(परिणाम) {
      ctx->परिणाम = परिणाम;
      ctx->state = CF_HC_FAILURE;
      जाओ out;
    }
    cf_hc_set_baller2(cf, तथ्य);
    pnow = Curl_pgrs_now(तथ्य);
    ctx->started = *pnow;
    cf_hc_baller_init(&ctx->ballers[0], cf, तथ्य);
    अगर((ctx->baller_गिनती > 1) || !ctx->ballers_complete) {
      Curl_expire_set(तथ्य, EXPIRE_ALPN_EYEBALLS,
                      ctx->soft_eyeballs_timeout_ms, pnow);
    }
    ctx->state = CF_HC_CONNECT;
    FALLTHROUGH();

  हाल CF_HC_CONNECT:
    अगर(!ctx->ballers_complete)
      cf_hc_set_baller2(cf, तथ्य);

    अगर(cf_hc_baller_is_connecting(&ctx->ballers[0])) {
      परिणाम = cf_hc_baller_connect(&ctx->ballers[0], cf, तथ्य, done);
      अगर(!परिणाम && *done) {
        परिणाम = baller_connected(cf, तथ्य, &ctx->ballers[0]);
        जाओ out;
      }
    }

    pnow = Curl_pgrs_now(तथ्य);
    अगर(time_to_start_baller2(cf, तथ्य, pnow)) {
      cf_hc_baller_init(&ctx->ballers[1], cf, तथ्य);
    }

    अगर(cf_hc_baller_is_connecting(&ctx->ballers[1])) {
      परिणाम = cf_hc_baller_connect(&ctx->ballers[1], cf, तथ्य, done);
      अगर(!परिणाम && *done) {
        परिणाम = baller_connected(cf, तथ्य, &ctx->ballers[1]);
        जाओ out;
      }
    }

    अगर(ctx->ballers[0].परिणाम &&
       (ctx->ballers[1].परिणाम ||
        (ctx->ballers_complete && (ctx->baller_गिनती < 2)))) {
      /* all have failed. we give up */
      CURL_TRC_CF(तथ्य, cf, "connect, all attempts failed");
      ctx->परिणाम = परिणाम = ctx->ballers[0].परिणाम;
      ctx->state = CF_HC_FAILURE;
      जाओ out;
    }
    परिणाम = CURLE_OK;
    *done = FALSE;
    अवरोध;

  हाल CF_HC_FAILURE:
    परिणाम = ctx->परिणाम;
    cf->connected = FALSE;
    *done = FALSE;
    अवरोध;

  हाल CF_HC_SUCCESS:
    परिणाम = CURLE_OK;
    cf->connected = TRUE;
    *done = TRUE;
    अवरोध;
  }

out:
  CURL_TRC_CF(तथ्य, cf, "connect -> %d, done=%d", (पूर्णांक)परिणाम, *done);
  वापस परिणाम;
}

अटल CURLcode cf_hc_shutdown(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य, bool *done)
{
  काष्ठा cf_hc_ctx *ctx = cf->ctx;
  माप_प्रकार i;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(तथ्य);
  अगर(cf->connected) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  क्रम(i = 0; i < ctx->baller_गिनती; i++) {
    काष्ठा cf_hc_baller *b = &ctx->ballers[i];
    bool bdone = FALSE;
    अगर(!cf_hc_baller_is_connecting(b) || b->shutdown)
      जारी;
    b->परिणाम = b->cf->cft->do_shutdown(b->cf, तथ्य, &bdone);
    अगर(b->परिणाम || bdone)
      b->shutdown = TRUE; /* treat a failed shutdown as done */
  }

  *done = TRUE;
  क्रम(i = 0; i < ctx->baller_गिनती; i++) {
    अगर(!ctx->ballers[i].shutdown)
      *done = FALSE;
  }
  अगर(*done) {
    क्रम(i = 0; i < ctx->baller_गिनती; i++) {
      अगर(ctx->ballers[i].परिणाम)
        परिणाम = ctx->ballers[i].परिणाम;
    }
  }
  CURL_TRC_CF(तथ्य, cf, "shutdown -> %d, done=%d", (पूर्णांक)परिणाम, *done);
  वापस परिणाम;
}

अटल CURLcode cf_hc_adjust_pollset(काष्ठा Curl_cfilter *cf,
                                     काष्ठा Curl_easy *तथ्य,
                                     काष्ठा easy_pollset *ps)
{
  CURLcode परिणाम = CURLE_OK;
  अगर(!cf->connected) {
    काष्ठा cf_hc_ctx *ctx = cf->ctx;
    माप_प्रकार i;

    क्रम(i = 0; (i < ctx->baller_गिनती) && !परिणाम; i++) {
      काष्ठा cf_hc_baller *b = &ctx->ballers[i];
      अगर(!cf_hc_baller_is_connecting(b))
        जारी;
      परिणाम = Curl_conn_cf_adjust_pollset(b->cf, तथ्य, ps);
    }
    CURL_TRC_CF(तथ्य, cf, "adjust_pollset -> %d, %u socks", (पूर्णांक)परिणाम,
                ps->n);
  }
  वापस परिणाम;
}

अटल bool cf_hc_तथ्य_pending(काष्ठा Curl_cfilter *cf,
                               स्थिर काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_hc_ctx *ctx = cf->ctx;
  माप_प्रकार i;

  अगर(cf->connected)
    वापस cf->अग्र->cft->has_तथ्य_pending(cf->अग्र, तथ्य);

  क्रम(i = 0; i < ctx->baller_गिनती; i++)
    अगर(cf_hc_baller_तथ्य_pending(&ctx->ballers[i], तथ्य))
      वापस TRUE;
  वापस FALSE;
}

अटल CURLcode cf_hc_query(काष्ठा Curl_cfilter *cf,
                            काष्ठा Curl_easy *तथ्य,
                            पूर्णांक query, पूर्णांक *pres1, व्योम *pres2)
{
  काष्ठा cf_hc_ctx *ctx = cf->ctx;
  माप_प्रकार i;

  अगर(!cf->connected) {
    चयन(query) {
    हाल CF_QUERY_NEED_FLUSH: {
      क्रम(i = 0; i < ctx->baller_गिनती; i++)
        अगर(cf_hc_baller_needs_flush(&ctx->ballers[i], तथ्य)) {
          *pres1 = TRUE;
          वापस CURLE_OK;
        }
      अवरोध;
    }
    शेष:
      अवरोध;
    }
  }
  वापस cf->अग्र ?
    cf->अग्र->cft->query(cf->अग्र, तथ्य, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

अटल CURLcode cf_hc_cntrl(काष्ठा Curl_cfilter *cf,
                            काष्ठा Curl_easy *तथ्य,
                            पूर्णांक event, पूर्णांक arg1, व्योम *arg2)
{
  काष्ठा cf_hc_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार i;

  अगर(!cf->connected) {
    चयन(event) {
    हाल CF_CTRL_REPORT_STATS:
    हाल CF_CTRL_CONN_INFO_UPDATE:
      क्रम(i = 0; i < ctx->baller_गिनती; i++) {
        /* Make the first baller that connected at network level report */
        अगर(Curl_conn_cf_is_ip_connected(ctx->ballers[i].cf, तथ्य)) {
          Curl_conn_cf_cntrl(ctx->ballers[i].cf, तथ्य, TRUE,
                             event, arg1, arg2);
          अवरोध;
        }
      }
      अवरोध;
    शेष:
      क्रम(i = 0; i < ctx->baller_गिनती; i++) {
        परिणाम = cf_hc_baller_cntrl(&ctx->ballers[i], तथ्य, event, arg1, arg2);
        अगर(परिणाम && (परिणाम != CURLE_AGAIN))
          जाओ out;
      }
      परिणाम = CURLE_OK;
      अवरोध;
    }
  }
out:
  वापस परिणाम;
}

अटल व्योम cf_hc_destroy(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_hc_ctx *ctx = cf->ctx;

  CURL_TRC_CF(तथ्य, cf, "destroy");
  cf_hc_ctx_destroy(तथ्य, ctx);
}

काष्ठा Curl_cftype Curl_cft_http_connect = {
  "HTTPS-CONNECT",
  CF_TYPE_SETUP,
  CURL_LOG_LVL_NONE,
  cf_hc_destroy,
  cf_hc_connect,
  cf_hc_shutdown,
  cf_hc_adjust_pollset,
  cf_hc_तथ्य_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  cf_hc_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_hc_query,
};

अटल CURLcode cf_hc_create(काष्ठा Curl_cfilter **pcf,
                             काष्ठा Curl_easy *तथ्य,
                             काष्ठा Curl_peer *destination,
                             uint8_t def_transport)
{
  काष्ठा Curl_cfilter *cf = शून्य;
  काष्ठा cf_hc_ctx *ctx;
  CURLcode परिणाम = CURLE_OK;

  ctx = curlx_calloc(1, माप(*ctx));
  अगर(!ctx) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }
  Curl_peer_link(&ctx->destination, destination);
  ctx->def_transport = def_transport;
  ctx->hard_eyeballs_timeout_ms = तथ्य->set.happy_eyeballs_timeout;
  ctx->soft_eyeballs_timeout_ms = तथ्य->set.happy_eyeballs_timeout / 2;

  परिणाम = Curl_cf_create(&cf, &Curl_cft_http_connect, ctx);
  अगर(परिणाम)
    जाओ out;
  ctx = शून्य;

out:
  *pcf = परिणाम ? शून्य : cf;
  cf_hc_ctx_destroy(तथ्य, ctx);
  वापस परिणाम;
}

अटल CURLcode cf_hc_add(काष्ठा Curl_easy *तथ्य,
                          काष्ठा Curl_peer *destination,
                          काष्ठा connectdata *conn,
                          int8_t sockindex,
                          uint8_t def_transport)
{
  काष्ठा Curl_cfilter *cf;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(तथ्य);
  परिणाम = cf_hc_create(&cf, तथ्य, destination, def_transport);
  अगर(परिणाम)
    जाओ out;
  Curl_conn_cf_add(तथ्य, conn, sockindex, cf);

#अगर_घोषित USE_HTTPSRR
  परिणाम = Curl_conn_dns_add_https_resolve(तथ्य, cf->conn, cf->sockindex,
                                           destination);
#पूर्ण_अगर
out:
  वापस परिणाम;
}

CURLcode Curl_cf_https_setup(काष्ठा Curl_easy *तथ्य,
                             काष्ठा Curl_peer *destination,
                             काष्ठा connectdata *conn,
                             int8_t sockindex)
{
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(conn->scheme->protocol == CURLPROTO_HTTPS);

  /* This filter is intended for HTTPS using ALPN and does
   * not support HTTPS Eyeballing to a proxy. */
  अगर((conn->scheme->protocol != CURLPROTO_HTTPS) ||
#अगर_अघोषित CURL_DISABLE_PROXY
     conn->bits.origin_is_proxy ||
#पूर्ण_अगर
     !conn->bits.tls_enable_alpn)
    जाओ out;

  परिणाम = cf_hc_add(तथ्य, destination, conn, sockindex,
                     conn->transport_wanted);

out:
  वापस परिणाम;
}

#पूर्ण_अगर /* !CURL_DISABLE_HTTP */
