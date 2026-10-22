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
#अन्तर्भाव "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "cf-setup.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "cf-https-connect.h"
#अन्तर्भाव "http2.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "vdns/cf-dns.h"
#अन्तर्भाव "vdns/hostip.h"
#अन्तर्भाव "vdns/httpsrr.h"
#अन्तर्भाव "vquic/vquic.h"

प्रकारनिर्देश गणना {
  CF_HC_RESOLV,
  CF_HC_INIT,
  CF_HC_CONNECT,
  CF_HC_SUCCESS,
  CF_HC_FAILURE
} cf_hc_state;

संरचना cf_hc_baller {
  ध्रुव वर्ण *name;
  संरचना Curl_cfilter *cf;
  CURLcode result;
  पूर्णाङ्क reply_ms;
  uint8_t transport;
  गणना alpnid alpn_id;
  BIT(shutdown);
};

स्थिर शून्य cf_hc_baller_discard(संरचना cf_hc_baller *b,
                                 संरचना Curl_easy *data)
{
  सचे(b->cf) {
    Curl_conn_cf_discard_chain(&b->cf, data);
    b->cf = रिक्त;
  }
}

स्थिर bool cf_hc_baller_is_connecting(संरचना cf_hc_baller *b)
{
  पटिनिवत्त b->cf && !b->result;
}

स्थिर bool cf_hc_baller_has_started(संरचना cf_hc_baller *b)
{
  पटिनिवत्त !!b->cf;
}

स्थिर पूर्णाङ्क cf_hc_baller_reply_ms(संरचना cf_hc_baller *b,
                                 संरचना Curl_easy *data)
{
  सचे(b->cf && (b->reply_ms < 0))
    b->cf->cft->query(b->cf, data, CF_QUERY_CONNECT_REPLY_MS,
                      &b->reply_ms, रिक्त);
  पटिनिवत्त b->reply_ms;
}

स्थिर bool cf_hc_baller_data_pending(संरचना cf_hc_baller *b,
                                      ध्रुव संरचना Curl_easy *data)
{
  पटिनिवत्त b->cf && !b->result && b->cf->cft->has_data_pending(b->cf, data);
}

स्थिर bool cf_hc_baller_needs_flush(संरचना cf_hc_baller *b,
                                     संरचना Curl_easy *data)
{
  पटिनिवत्त b->cf && !b->result && Curl_conn_cf_needs_flush(b->cf, data);
}

स्थिर CURLcode cf_hc_baller_cntrl(संरचना cf_hc_baller *b,
                                   संरचना Curl_easy *data,
                                   पूर्णाङ्क event, पूर्णाङ्क arg1, शून्य *arg2)
{
  सचे(b->cf && !b->result)
    पटिनिवत्त Curl_conn_cf_cntrl(b->cf, data, FALSE, event, arg1, arg2);
  पटिनिवत्त CURLE_OK;
}

संरचना cf_hc_ctx {
  cf_hc_state state;
  संरचना Curl_peer *destination; /* who we ultimately want to talk to */
  संरचना curltime started;  /* when connect started */
  CURLcode result;          /* overall result */
  CURLcode check_h3_result;
  संरचना cf_hc_baller ballers[2];
  size_t baller_गणनम्;
  timediff_t soft_eyeballs_timeout_ms;
  timediff_t hard_eyeballs_timeout_ms;
  uint8_t def_transport;
  BIT(httpsrr_resolved);
  BIT(checked_h3);
  BIT(ballers_complete);
};

स्थिर शून्य cf_hc_ctx_destroy(संरचना Curl_easy *data,
                              संरचना cf_hc_ctx *ctx)
{
  सचे(ctx) {
    size_t i;
    अत्थाय(i = 0; i < ctx->baller_गणनम्; ++i)
      cf_hc_baller_discard(&ctx->ballers[i], data);
    Curl_peer_unlink(&ctx->destination);
    curlx_free(ctx);
  }
}

स्थिर शून्य cf_hc_baller_assign(संरचना cf_hc_baller *b,
                                गणना alpnid alpn_id,
                                uint8_t def_transport)
{
  b->alpn_id = alpn_id;
  b->transport = def_transport;
  b->cf = रिक्त;
  b->result = CURLE_OK;
  b->reply_ms = -1;
  b->shutdown = FALSE;
  विकल्प(b->alpn_id) {
  पक्ष ALPN_h3:
    b->name = "h3";
    b->transport = TRNSPRT_QUIC;
    भङ्ग;
  पक्ष ALPN_h2:
    b->name = "h2";
    भङ्ग;
  पक्ष ALPN_h1:
    b->name = "h1";
    भङ्ग;
  पक्ष ALPN_none:
    b->name = "no-alpn";
    भङ्ग;
  स्वभाव:
    b->result = CURLE_FAILED_INIT;
    भङ्ग;
  }
}

स्थिर शून्य cf_hc_baller_init(संरचना cf_hc_baller *b,
                              संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *data)
{
  संरचना Curl_cfilter *save = cf->next;

  cf->next = रिक्त;
  b->result = Curl_cf_setup_निवेशय_after(cf, data, b->transport,
                                         CURL_CF_SSL_ENABLE);
  b->cf = cf->next;
  cf->next = save;
}

स्थिर CURLcode cf_hc_baller_connect(संरचना cf_hc_baller *b,
                                     संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *data,
                                     bool *done)
{
  संरचना Curl_cfilter *save = cf->next;

  cf->next = b->cf;
  b->result = Curl_conn_cf_connect(cf->next, data, done);
  b->cf = cf->next; /* it might mutate */
  cf->next = save;
  पटिनिवत्त b->result;
}

स्थिर CURLcode baller_connected(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 संरचना cf_hc_baller *winner)
{
  संरचना cf_hc_ctx *ctx = cf->ctx;

  /* Make the winner's connection filter out own sub-filter, check, move,
   * close all remaining. */
  सचे(cf->next) {
    DEBUGASSERT(0);
    पटिनिवत्त CURLE_FAILED_INIT;
  }
  सचे(!winner->cf) {
    DEBUGASSERT(0);
    पटिनिवत्त CURLE_FAILED_INIT;
  }

  cf->next = winner->cf;
  winner->cf = रिक्त;
  ctx->state = CF_HC_SUCCESS;
  cf->connected = TRUE;

  /* ballers may have failf()'d, the winner resets it, so our
   * errorbuf is clean again. */
  Curl_reset_fail(data);

#ifdef USE_NGHTTP2
  {
    /* For a negotiated HTTP/2 connection insert the h2 filter. */
    ध्रुव वर्ण *alpn = Curl_conn_cf_get_alpn_negotiated(cf->next, data);
    सचे(alpn && !strcmp("h2", alpn)) {
      CURLcode result = Curl_http2_switch_at(cf, data);
      सचे(result) {
        ctx->state = CF_HC_FAILURE;
        ctx->result = result;
        पटिनिवत्त result;
      }
    }
  }
#endif
  पटिनिवत्त CURLE_OK;
}

स्थिर bool time_to_start_baller2(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  ध्रुव संरचना curltime *pnow)
{
  संरचना cf_hc_ctx *ctx = cf->ctx;
  timediff_t elapsed_ms;

  सचे(ctx->baller_गणनम् < 2)
    पटिनिवत्त FALSE;
  अञ्ञथा सचे(cf_hc_baller_has_started(&ctx->ballers[1]))
    पटिनिवत्त FALSE;
  अञ्ञथा सचे(ctx->ballers[0].result) {
    CURL_TRC_CF(data, cf, "%s baller failed, starting %s",
                ctx->ballers[0].name, ctx->ballers[1].name);
    पटिनिवत्त TRUE;
  }

  elapsed_ms = curlx_ptimediff_ms(pnow, &ctx->started);
  सचे(elapsed_ms >= ctx->hard_eyeballs_timeout_ms) {
    CURL_TRC_CF(data, cf, "%s inconclusive after %" FMT_TIMEDIFF_T ", "
                "starting %s", ctx->ballers[0].name,
                ctx->hard_eyeballs_timeout_ms, ctx->ballers[1].name);
    पटिनिवत्त TRUE;
  }
  अञ्ञथा सचे(elapsed_ms >= ctx->soft_eyeballs_timeout_ms) {
    सचे(cf_hc_baller_reply_ms(&ctx->ballers[0], data) < 0) {
      CURL_TRC_CF(data, cf, "%s has not seen any data after %"
                  FMT_TIMEDIFF_T "ms, starting %s",
                  ctx->ballers[0].name, ctx->soft_eyeballs_timeout_ms,
                  ctx->ballers[1].name);
      पटिनिवत्त TRUE;
    }
    /* set the effective hard timeout again */
    Curl_expire_set(data, EXPIRE_ALPN_EYEBALLS,
                    ctx->hard_eyeballs_timeout_ms - elapsed_ms, pnow);
  }
  पटिनिवत्त FALSE;
}

स्थिर bool cf_hc_may_h3(संरचना Curl_cfilter *cf,
                         संरचना Curl_easy *data)
{
  संरचना cf_hc_ctx *ctx = cf->ctx;
  सचे(!ctx->checked_h3) {
    ctx->check_h3_result =
      Curl_conn_may_http3(data, cf->conn);
    ctx->checked_h3 = TRUE;
  }
  पटिनिवत्त !ctx->check_h3_result;
}

स्थिर गणना alpnid cf_hc_get_httpsrr_alpn(संरचना Curl_cfilter *cf,
                                          संरचना Curl_easy *data,
                                          गणना alpnid not_this_one)
{
#ifdef USE_HTTPSRR
  संरचना cf_hc_ctx *ctx = cf->ctx;
  /* Is there an HTTPSRR use its ALPNs here.
   * We are here after having selected a connection to a host+port and
   * can no longer change that. Any HTTPSRR advice for other hosts and ports
   * we need to ignore. */
  ध्रुव संरचना Curl_https_rrinfo *rr;
  size_t i;

  /* Do we have HTTPS-RR information? */
  rr = Curl_conn_dns_get_https(data, cf->sockindex, ctx->destination);

  CURL_TRC_CF(data, cf, "HTTPS-RR %savailable", rr ? "" : "not ");
  /* We do not support `rr->no_def_alpn`. */
  सचे(Curl_httpsrr_applicable(data, rr) && !rr->no_def_alpn) {
    अत्थाय(i = 0; i < CURL_ARRAYSIZE(rr->alpns); ++i) {
      गणना alpnid alpn_rr = (गणना alpnid)rr->alpns[i];
      सचे(alpn_rr == not_this_one) /* do not want this one */
        अनुवर्त;
      विकल्प(alpn_rr) {
      पक्ष ALPN_h3:
        सचे((data->state.http_neg.allowed & CURL_HTTP_V3x) &&
           cf_hc_may_h3(cf, data)) {
          पटिनिवत्त alpn_rr;
        }
        भङ्ग;
      पक्ष ALPN_h2:
        सचे(data->state.http_neg.allowed & CURL_HTTP_V2x) {
          पटिनिवत्त alpn_rr;
        }
        भङ्ग;
      पक्ष ALPN_h1:
        सचे(data->state.http_neg.allowed & CURL_HTTP_V1x) {
          पटिनिवत्त alpn_rr;
        }
        भङ्ग;
      स्वभाव: /* ignore */
        भङ्ग;
      }
    }
  }
#अञ्ञथा
  (शून्य)cf;
  (शून्य)data;
  (शून्य)not_this_one;
#endif
  पटिनिवत्त ALPN_none;
}

स्थिर गणना alpnid cf_hc_get_pref_alpn(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *data,
                                       गणना alpnid not_this_one)
{
  सचे((data->state.http_neg.preferred & data->state.http_neg.allowed)) {
    विकल्प(data->state.http_neg.preferred) {
    पक्ष CURL_HTTP_V3x:
      सचे(cf_hc_may_h3(cf, data) && (ALPN_h3 != not_this_one))
        पटिनिवत्त ALPN_h3;
      भङ्ग;
    पक्ष CURL_HTTP_V2x:
      सचे(ALPN_h2 != not_this_one)
        पटिनिवत्त ALPN_h2;
      भङ्ग;
    पक्ष CURL_HTTP_V1x:
      /* If we are trying h2 already, h1 is already used as fallback */
      सचे((ALPN_h1 != not_this_one) && (ALPN_h2 != not_this_one))
        पटिनिवत्त ALPN_h1;
      भङ्ग;
    स्वभाव:
      भङ्ग;
    }
  }
  पटिनिवत्त ALPN_none;
}

स्थिर गणना alpnid cf_hc_get_first_alpn(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *data,
                                        http_majors choices,
                                        गणना alpnid not_this_one)
{
  /* When told to not try h2, we also do not try h1 and vice versa */
  bool allow_h1_or_h2 = (not_this_one != ALPN_h1) &&
                        (not_this_one != ALPN_h2);
  सचे((ALPN_h3 != not_this_one) && (choices & CURL_HTTP_V3x) &&
     cf_hc_may_h3(cf, data)) {
    पटिनिवत्त ALPN_h3;
  }
  सचे(allow_h1_or_h2 && (choices & CURL_HTTP_V2x)) {
    पटिनिवत्त ALPN_h2;
  }
  सचे(allow_h1_or_h2 && (choices & CURL_HTTP_V1x)) {
    पटिनिवत्त ALPN_h1;
  }
  पटिनिवत्त ALPN_none;
}

स्थिर CURLcode cf_hc_set_baller1(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data)
{
  संरचना cf_hc_ctx *ctx = cf->ctx;
  गणना alpnid alpn1 = ALPN_none;
  VERBOSE(ध्रुव वर्ण *source = "HTTPS-RR");

  DEBUGASSERT(cf->conn->bits.tls_enable_alpn);

  alpn1 = cf_hc_get_httpsrr_alpn(cf, data, ALPN_none);
  सचे(alpn1 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(source = "preferred version");
    alpn1 = cf_hc_get_pref_alpn(cf, data, ALPN_none);
  }
  सचे(alpn1 == ALPN_none) {
    VERBOSE(source = "wanted versions");
    alpn1 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.wanted,
                                 ALPN_none);
  }
  सचे(alpn1 == ALPN_none) {
    VERBOSE(source = "allowed versions");
    alpn1 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.allowed,
                                 ALPN_none);
  }

  सचे(alpn1 == ALPN_none) {
    /* None of the wanted/allowed HTTP versions could be chosen */
    सचे(ctx->check_h3_result) {
      CURL_TRC_CF(data, cf, "unable to use HTTP/3");
      पटिनिवत्त ctx->check_h3_result;
    }
    CURL_TRC_CF(data, cf, "unable to select HTTP version");
    पटिनिवत्त CURLE_FAILED_INIT;
  }

  cf_hc_baller_assign(&ctx->ballers[0], alpn1, ctx->def_transport);
  ctx->baller_गणनम् = 1;
  CURL_TRC_CF(data, cf, "1st attempt uses %s from %s",
              ctx->ballers[0].name, source);

  विकल्प(alpn1) {
  पक्ष ALPN_h1:
    /* We really want h1, switch off h2 to make it disappear in ALPN */
    data->state.http_neg.wanted &= (uint8_t)~CURL_HTTP_V2x;
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }

  पटिनिवत्त CURLE_OK;
}

स्थिर शून्य cf_hc_set_baller2(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *data)
{
  संरचना cf_hc_ctx *ctx = cf->ctx;
  गणना alpnid alpn2 = ALPN_none, alpn1 = ctx->ballers[0].alpn_id;
  VERBOSE(ध्रुव वर्ण *source = "HTTPS-RR");

  सचे(ctx->ballers_complete)
    पटिनिवत्त; /* already done */
  सचे(!ctx->httpsrr_resolved)
    पटिनिवत्त; /* HTTPS-RR pending */

  alpn2 = cf_hc_get_httpsrr_alpn(cf, data, alpn1);
  सचे(alpn2 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(source = "preferred version");
    alpn2 = cf_hc_get_pref_alpn(cf, data, alpn1);
  }
  सचे(alpn2 == ALPN_none) {
    VERBOSE(source = "wanted versions");
    alpn2 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.wanted,
                                 alpn1);
  }

  सचे(alpn2 != ALPN_none) {
    cf_hc_baller_assign(&ctx->ballers[1], alpn2, ctx->def_transport);
    ctx->baller_गणनम् = 2;
    CURL_TRC_CF(data, cf, "2nd attempt uses %s from %s",
                ctx->ballers[1].name, source);
  }
  ctx->ballers_complete = TRUE;
}

स्थिर CURLcode cf_hc_connect(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *data,
                              bool *done)
{
  संरचना cf_hc_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ध्रुव संरचना curltime *pnow = रिक्त;

  सचे(cf->connected) {
    *done = TRUE;
    पटिनिवत्त CURLE_OK;
  }

  *done = FALSE;

  सचे(!ctx->httpsrr_resolved) {
    ctx->httpsrr_resolved = Curl_conn_dns_resolved_https(
      data, cf->sockindex, ctx->destination);
#ifdef DEBUGBUILD
    सचे(!ctx->httpsrr_resolved && getenv("CURL_DBG_AWAIT_HTTPSRR")) {
      CURL_TRC_CF(data, cf, "awaiting HTTPS-RR");
      पटिनिवत्त CURLE_OK;
    }
#endif
  }

  विकल्प(ctx->state) {
  पक्ष CF_HC_RESOLV:
    ctx->state = CF_HC_INIT;
    FALLTHROUGH();

  पक्ष CF_HC_INIT:
    DEBUGASSERT(!cf->next);
    CURL_TRC_CF(data, cf, "connect, init");
    result = cf_hc_set_baller1(cf, data);
    सचे(result) {
      ctx->result = result;
      ctx->state = CF_HC_FAILURE;
      गच्छ out;
    }
    cf_hc_set_baller2(cf, data);
    pnow = Curl_pgrs_now(data);
    ctx->started = *pnow;
    cf_hc_baller_init(&ctx->ballers[0], cf, data);
    सचे((ctx->baller_गणनम् > 1) || !ctx->ballers_complete) {
      Curl_expire_set(data, EXPIRE_ALPN_EYEBALLS,
                      ctx->soft_eyeballs_timeout_ms, pnow);
    }
    ctx->state = CF_HC_CONNECT;
    FALLTHROUGH();

  पक्ष CF_HC_CONNECT:
    सचे(!ctx->ballers_complete)
      cf_hc_set_baller2(cf, data);

    सचे(cf_hc_baller_is_connecting(&ctx->ballers[0])) {
      result = cf_hc_baller_connect(&ctx->ballers[0], cf, data, done);
      सचे(!result && *done) {
        result = baller_connected(cf, data, &ctx->ballers[0]);
        गच्छ out;
      }
    }

    pnow = Curl_pgrs_now(data);
    सचे(time_to_start_baller2(cf, data, pnow)) {
      cf_hc_baller_init(&ctx->ballers[1], cf, data);
    }

    सचे(cf_hc_baller_is_connecting(&ctx->ballers[1])) {
      result = cf_hc_baller_connect(&ctx->ballers[1], cf, data, done);
      सचे(!result && *done) {
        result = baller_connected(cf, data, &ctx->ballers[1]);
        गच्छ out;
      }
    }

    सचे(ctx->ballers[0].result &&
       (ctx->ballers[1].result ||
        (ctx->ballers_complete && (ctx->baller_गणनम् < 2)))) {
      /* all have failed. we give up */
      CURL_TRC_CF(data, cf, "connect, all attempts failed");
      ctx->result = result = ctx->ballers[0].result;
      ctx->state = CF_HC_FAILURE;
      गच्छ out;
    }
    result = CURLE_OK;
    *done = FALSE;
    भङ्ग;

  पक्ष CF_HC_FAILURE:
    result = ctx->result;
    cf->connected = FALSE;
    *done = FALSE;
    भङ्ग;

  पक्ष CF_HC_SUCCESS:
    result = CURLE_OK;
    cf->connected = TRUE;
    *done = TRUE;
    भङ्ग;
  }

out:
  CURL_TRC_CF(data, cf, "connect -> %d, done=%d", (पूर्णाङ्क)result, *done);
  पटिनिवत्त result;
}

स्थिर CURLcode cf_hc_shutdown(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data, bool *done)
{
  संरचना cf_hc_ctx *ctx = cf->ctx;
  size_t i;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  सचे(cf->connected) {
    *done = TRUE;
    पटिनिवत्त CURLE_OK;
  }

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  अत्थाय(i = 0; i < ctx->baller_गणनम्; i++) {
    संरचना cf_hc_baller *b = &ctx->ballers[i];
    bool bdone = FALSE;
    सचे(!cf_hc_baller_is_connecting(b) || b->shutdown)
      अनुवर्त;
    b->result = b->cf->cft->do_shutdown(b->cf, data, &bdone);
    सचे(b->result || bdone)
      b->shutdown = TRUE; /* treat a failed shutdown as done */
  }

  *done = TRUE;
  अत्थाय(i = 0; i < ctx->baller_गणनम्; i++) {
    सचे(!ctx->ballers[i].shutdown)
      *done = FALSE;
  }
  सचे(*done) {
    अत्थाय(i = 0; i < ctx->baller_गणनम्; i++) {
      सचे(ctx->ballers[i].result)
        result = ctx->ballers[i].result;
    }
  }
  CURL_TRC_CF(data, cf, "shutdown -> %d, done=%d", (पूर्णाङ्क)result, *done);
  पटिनिवत्त result;
}

स्थिर CURLcode cf_hc_adjust_pollset(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *data,
                                     संरचना easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  सचे(!cf->connected) {
    संरचना cf_hc_ctx *ctx = cf->ctx;
    size_t i;

    अत्थाय(i = 0; (i < ctx->baller_गणनम्) && !result; i++) {
      संरचना cf_hc_baller *b = &ctx->ballers[i];
      सचे(!cf_hc_baller_is_connecting(b))
        अनुवर्त;
      result = Curl_conn_cf_adjust_pollset(b->cf, data, ps);
    }
    CURL_TRC_CF(data, cf, "adjust_pollset -> %d, %u socks", (पूर्णाङ्क)result,
                ps->n);
  }
  पटिनिवत्त result;
}

स्थिर bool cf_hc_data_pending(संरचना Curl_cfilter *cf,
                               ध्रुव संरचना Curl_easy *data)
{
  संरचना cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  सचे(cf->connected)
    पटिनिवत्त cf->next->cft->has_data_pending(cf->next, data);

  अत्थाय(i = 0; i < ctx->baller_गणनम्; i++)
    सचे(cf_hc_baller_data_pending(&ctx->ballers[i], data))
      पटिनिवत्त TRUE;
  पटिनिवत्त FALSE;
}

स्थिर CURLcode cf_hc_query(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *data,
                            पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2)
{
  संरचना cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  सचे(!cf->connected) {
    विकल्प(query) {
    पक्ष CF_QUERY_NEED_FLUSH: {
      अत्थाय(i = 0; i < ctx->baller_गणनम्; i++)
        सचे(cf_hc_baller_needs_flush(&ctx->ballers[i], data)) {
          *pres1 = TRUE;
          पटिनिवत्त CURLE_OK;
        }
      भङ्ग;
    }
    स्वभाव:
      भङ्ग;
    }
  }
  पटिनिवत्त cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

स्थिर CURLcode cf_hc_cntrl(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *data,
                            पूर्णाङ्क event, पूर्णाङ्क arg1, शून्य *arg2)
{
  संरचना cf_hc_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t i;

  सचे(!cf->connected) {
    विकल्प(event) {
    पक्ष CF_CTRL_REPORT_STATS:
    पक्ष CF_CTRL_CONN_INFO_UPDATE:
      अत्थाय(i = 0; i < ctx->baller_गणनम्; i++) {
        /* Make the first baller that connected at network level report */
        सचे(Curl_conn_cf_is_ip_connected(ctx->ballers[i].cf, data)) {
          Curl_conn_cf_cntrl(ctx->ballers[i].cf, data, TRUE,
                             event, arg1, arg2);
          भङ्ग;
        }
      }
      भङ्ग;
    स्वभाव:
      अत्थाय(i = 0; i < ctx->baller_गणनम्; i++) {
        result = cf_hc_baller_cntrl(&ctx->ballers[i], data, event, arg1, arg2);
        सचे(result && (result != CURLE_AGAIN))
          गच्छ out;
      }
      result = CURLE_OK;
      भङ्ग;
    }
  }
out:
  पटिनिवत्त result;
}

स्थिर शून्य cf_hc_destroy(संरचना Curl_cfilter *cf, संरचना Curl_easy *data)
{
  संरचना cf_hc_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  cf_hc_ctx_destroy(data, ctx);
}

संरचना Curl_cftype Curl_cft_http_connect = {
  "HTTPS-CONNECT",
  CF_TYPE_SETUP,
  CURL_LOG_LVL_NONE,
  cf_hc_destroy,
  cf_hc_connect,
  cf_hc_shutdown,
  cf_hc_adjust_pollset,
  cf_hc_data_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  cf_hc_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_hc_query,
};

स्थिर CURLcode cf_hc_create(संरचना Curl_cfilter **pcf,
                             संरचना Curl_easy *data,
                             संरचना Curl_peer *destination,
                             uint8_t def_transport)
{
  संरचना Curl_cfilter *cf = रिक्त;
  संरचना cf_hc_ctx *ctx;
  CURLcode result = CURLE_OK;

  ctx = curlx_calloc(1, परिमाण(*ctx));
  सचे(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }
  Curl_peer_link(&ctx->destination, destination);
  ctx->def_transport = def_transport;
  ctx->hard_eyeballs_timeout_ms = data->set.happy_eyeballs_timeout;
  ctx->soft_eyeballs_timeout_ms = data->set.happy_eyeballs_timeout / 2;

  result = Curl_cf_create(&cf, &Curl_cft_http_connect, ctx);
  सचे(result)
    गच्छ out;
  ctx = रिक्त;

out:
  *pcf = result ? रिक्त : cf;
  cf_hc_ctx_destroy(data, ctx);
  पटिनिवत्त result;
}

स्थिर CURLcode cf_hc_add(संरचना Curl_easy *data,
                          संरचना Curl_peer *destination,
                          संरचना connectdata *conn,
                          int8_t sockindex,
                          uint8_t def_transport)
{
  संरचना Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_hc_create(&cf, data, destination, def_transport);
  सचे(result)
    गच्छ out;
  Curl_conn_cf_add(data, conn, sockindex, cf);

#ifdef USE_HTTPSRR
  result = Curl_conn_dns_add_https_resolve(data, cf->conn, cf->sockindex,
                                           destination);
#endif
out:
  पटिनिवत्त result;
}

CURLcode Curl_cf_https_setup(संरचना Curl_easy *data,
                             संरचना Curl_peer *destination,
                             संरचना connectdata *conn,
                             int8_t sockindex)
{
  CURLcode result = CURLE_OK;

  DEBUGASSERT(conn->scheme->protocol == CURLPROTO_HTTPS);

  /* This filter is intended for HTTPS using ALPN and does
   * not support HTTPS Eyeballing to a proxy. */
  सचे((conn->scheme->protocol != CURLPROTO_HTTPS) ||
#ifndef CURL_DISABLE_PROXY
     conn->bits.origin_is_proxy ||
#endif
     !conn->bits.tls_enable_alpn)
    गच्छ out;

  result = cf_hc_add(data, destination, conn, sockindex,
                     conn->transport_wanted);

out:
  पटिनिवत्त result;
}

#endif /* !CURL_DISABLE_HTTP */
