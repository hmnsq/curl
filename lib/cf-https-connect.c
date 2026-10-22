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
#அந்தர்பாவ "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "cf-setup.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "cf-https-connect.h"
#அந்தர்பாவ "http2.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "vdns/cf-dns.h"
#அந்தர்பாவ "vdns/hostip.h"
#அந்தர்பாவ "vdns/httpsrr.h"
#அந்தர்பாவ "vquic/vquic.h"

வகைவரை எண்ணிக்கை {
  CF_HC_RESOLV,
  CF_HC_INIT,
  CF_HC_CONNECT,
  CF_HC_SUCCESS,
  CF_HC_FAILURE
} cf_hc_state;

கட்டமைப்பு cf_hc_baller {
  மாறிலி எழுத்து *name;
  கட்டமைப்பு Curl_cfilter *cf;
  CURLcode result;
  முழு reply_ms;
  uint8_t transport;
  எண்ணிக்கை alpnid alpn_id;
  BIT(shutdown);
};

நிலையான வெறுமை cf_hc_baller_discard(கட்டமைப்பு cf_hc_baller *b,
                                 கட்டமைப்பு Curl_easy *data)
{
  எனில்(b->cf) {
    Curl_conn_cf_discard_chain(&b->cf, data);
    b->cf = வெற்று;
  }
}

நிலையான bool cf_hc_baller_is_connecting(கட்டமைப்பு cf_hc_baller *b)
{
  திரும்பு b->cf && !b->result;
}

நிலையான bool cf_hc_baller_has_started(கட்டமைப்பு cf_hc_baller *b)
{
  திரும்பு !!b->cf;
}

நிலையான முழு cf_hc_baller_reply_ms(கட்டமைப்பு cf_hc_baller *b,
                                 கட்டமைப்பு Curl_easy *data)
{
  எனில்(b->cf && (b->reply_ms < 0))
    b->cf->cft->query(b->cf, data, CF_QUERY_CONNECT_REPLY_MS,
                      &b->reply_ms, வெற்று);
  திரும்பு b->reply_ms;
}

நிலையான bool cf_hc_baller_data_pending(கட்டமைப்பு cf_hc_baller *b,
                                      மாறிலி கட்டமைப்பு Curl_easy *data)
{
  திரும்பு b->cf && !b->result && b->cf->cft->has_data_pending(b->cf, data);
}

நிலையான bool cf_hc_baller_needs_flush(கட்டமைப்பு cf_hc_baller *b,
                                     கட்டமைப்பு Curl_easy *data)
{
  திரும்பு b->cf && !b->result && Curl_conn_cf_needs_flush(b->cf, data);
}

நிலையான CURLcode cf_hc_baller_cntrl(கட்டமைப்பு cf_hc_baller *b,
                                   கட்டமைப்பு Curl_easy *data,
                                   முழு event, முழு arg1, வெறுமை *arg2)
{
  எனில்(b->cf && !b->result)
    திரும்பு Curl_conn_cf_cntrl(b->cf, data, FALSE, event, arg1, arg2);
  திரும்பு CURLE_OK;
}

கட்டமைப்பு cf_hc_ctx {
  cf_hc_state state;
  கட்டமைப்பு Curl_peer *destination; /* who we ultimately want to talk to */
  கட்டமைப்பு curltime started;  /* when connect started */
  CURLcode result;          /* overall result */
  CURLcode check_h3_result;
  கட்டமைப்பு cf_hc_baller ballers[2];
  size_t baller_மொத்தம்;
  timediff_t soft_eyeballs_timeout_ms;
  timediff_t hard_eyeballs_timeout_ms;
  uint8_t def_transport;
  BIT(httpsrr_resolved);
  BIT(checked_h3);
  BIT(ballers_complete);
};

நிலையான வெறுமை cf_hc_ctx_destroy(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு cf_hc_ctx *ctx)
{
  எனில்(ctx) {
    size_t i;
    க்கு(i = 0; i < ctx->baller_மொத்தம்; ++i)
      cf_hc_baller_discard(&ctx->ballers[i], data);
    Curl_peer_unlink(&ctx->destination);
    curlx_free(ctx);
  }
}

நிலையான வெறுமை cf_hc_baller_assign(கட்டமைப்பு cf_hc_baller *b,
                                எண்ணிக்கை alpnid alpn_id,
                                uint8_t def_transport)
{
  b->alpn_id = alpn_id;
  b->transport = def_transport;
  b->cf = வெற்று;
  b->result = CURLE_OK;
  b->reply_ms = -1;
  b->shutdown = FALSE;
  தேர்வு(b->alpn_id) {
  நிலை ALPN_h3:
    b->name = "h3";
    b->transport = TRNSPRT_QUIC;
    நிறுத்து;
  நிலை ALPN_h2:
    b->name = "h2";
    நிறுத்து;
  நிலை ALPN_h1:
    b->name = "h1";
    நிறுத்து;
  நிலை ALPN_none:
    b->name = "no-alpn";
    நிறுத்து;
  இயல்பு:
    b->result = CURLE_FAILED_INIT;
    நிறுத்து;
  }
}

நிலையான வெறுமை cf_hc_baller_init(கட்டமைப்பு cf_hc_baller *b,
                              கட்டமைப்பு Curl_cfilter *cf,
                              கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_cfilter *save = cf->next;

  cf->next = வெற்று;
  b->result = Curl_cf_setup_செருகு_after(cf, data, b->transport,
                                         CURL_CF_SSL_ENABLE);
  b->cf = cf->next;
  cf->next = save;
}

நிலையான CURLcode cf_hc_baller_connect(கட்டமைப்பு cf_hc_baller *b,
                                     கட்டமைப்பு Curl_cfilter *cf,
                                     கட்டமைப்பு Curl_easy *data,
                                     bool *done)
{
  கட்டமைப்பு Curl_cfilter *save = cf->next;

  cf->next = b->cf;
  b->result = Curl_conn_cf_connect(cf->next, data, done);
  b->cf = cf->next; /* it might mutate */
  cf->next = save;
  திரும்பு b->result;
}

நிலையான CURLcode baller_connected(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு cf_hc_baller *winner)
{
  கட்டமைப்பு cf_hc_ctx *ctx = cf->ctx;

  /* Make the winner's connection filter out own sub-filter, check, move,
   * close all remaining. */
  எனில்(cf->next) {
    DEBUGASSERT(0);
    திரும்பு CURLE_FAILED_INIT;
  }
  எனில்(!winner->cf) {
    DEBUGASSERT(0);
    திரும்பு CURLE_FAILED_INIT;
  }

  cf->next = winner->cf;
  winner->cf = வெற்று;
  ctx->state = CF_HC_SUCCESS;
  cf->connected = TRUE;

  /* ballers may have failf()'d, the winner resets it, so our
   * errorbuf is clean again. */
  Curl_reset_fail(data);

#ifdef USE_NGHTTP2
  {
    /* For a negotiated HTTP/2 connection insert the h2 filter. */
    மாறிலி எழுத்து *alpn = Curl_conn_cf_get_alpn_negotiated(cf->next, data);
    எனில்(alpn && !strcmp("h2", alpn)) {
      CURLcode result = Curl_http2_switch_at(cf, data);
      எனில்(result) {
        ctx->state = CF_HC_FAILURE;
        ctx->result = result;
        திரும்பு result;
      }
    }
  }
#endif
  திரும்பு CURLE_OK;
}

நிலையான bool time_to_start_baller2(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data,
                                  மாறிலி கட்டமைப்பு curltime *pnow)
{
  கட்டமைப்பு cf_hc_ctx *ctx = cf->ctx;
  timediff_t elapsed_ms;

  எனில்(ctx->baller_மொத்தம் < 2)
    திரும்பு FALSE;
  இல்லையெனில் எனில்(cf_hc_baller_has_started(&ctx->ballers[1]))
    திரும்பு FALSE;
  இல்லையெனில் எனில்(ctx->ballers[0].result) {
    CURL_TRC_CF(data, cf, "%s baller failed, starting %s",
                ctx->ballers[0].name, ctx->ballers[1].name);
    திரும்பு TRUE;
  }

  elapsed_ms = curlx_ptimediff_ms(pnow, &ctx->started);
  எனில்(elapsed_ms >= ctx->hard_eyeballs_timeout_ms) {
    CURL_TRC_CF(data, cf, "%s inconclusive after %" FMT_TIMEDIFF_T ", "
                "starting %s", ctx->ballers[0].name,
                ctx->hard_eyeballs_timeout_ms, ctx->ballers[1].name);
    திரும்பு TRUE;
  }
  இல்லையெனில் எனில்(elapsed_ms >= ctx->soft_eyeballs_timeout_ms) {
    எனில்(cf_hc_baller_reply_ms(&ctx->ballers[0], data) < 0) {
      CURL_TRC_CF(data, cf, "%s has not seen any data after %"
                  FMT_TIMEDIFF_T "ms, starting %s",
                  ctx->ballers[0].name, ctx->soft_eyeballs_timeout_ms,
                  ctx->ballers[1].name);
      திரும்பு TRUE;
    }
    /* set the effective hard timeout again */
    Curl_expire_set(data, EXPIRE_ALPN_EYEBALLS,
                    ctx->hard_eyeballs_timeout_ms - elapsed_ms, pnow);
  }
  திரும்பு FALSE;
}

நிலையான bool cf_hc_may_h3(கட்டமைப்பு Curl_cfilter *cf,
                         கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_hc_ctx *ctx = cf->ctx;
  எனில்(!ctx->checked_h3) {
    ctx->check_h3_result =
      Curl_conn_may_http3(data, cf->conn);
    ctx->checked_h3 = TRUE;
  }
  திரும்பு !ctx->check_h3_result;
}

நிலையான எண்ணிக்கை alpnid cf_hc_get_httpsrr_alpn(கட்டமைப்பு Curl_cfilter *cf,
                                          கட்டமைப்பு Curl_easy *data,
                                          எண்ணிக்கை alpnid not_this_one)
{
#ifdef USE_HTTPSRR
  கட்டமைப்பு cf_hc_ctx *ctx = cf->ctx;
  /* Is there an HTTPSRR use its ALPNs here.
   * We are here after having selected a connection to a host+port and
   * can no longer change that. Any HTTPSRR advice for other hosts and ports
   * we need to ignore. */
  மாறிலி கட்டமைப்பு Curl_https_rrinfo *rr;
  size_t i;

  /* Do we have HTTPS-RR information? */
  rr = Curl_conn_dns_get_https(data, cf->sockindex, ctx->destination);

  CURL_TRC_CF(data, cf, "HTTPS-RR %savailable", rr ? "" : "not ");
  /* We do not support `rr->no_def_alpn`. */
  எனில்(Curl_httpsrr_applicable(data, rr) && !rr->no_def_alpn) {
    க்கு(i = 0; i < CURL_ARRAYSIZE(rr->alpns); ++i) {
      எண்ணிக்கை alpnid alpn_rr = (எண்ணிக்கை alpnid)rr->alpns[i];
      எனில்(alpn_rr == not_this_one) /* do not want this one */
        தொடர்;
      தேர்வு(alpn_rr) {
      நிலை ALPN_h3:
        எனில்((data->state.http_neg.allowed & CURL_HTTP_V3x) &&
           cf_hc_may_h3(cf, data)) {
          திரும்பு alpn_rr;
        }
        நிறுத்து;
      நிலை ALPN_h2:
        எனில்(data->state.http_neg.allowed & CURL_HTTP_V2x) {
          திரும்பு alpn_rr;
        }
        நிறுத்து;
      நிலை ALPN_h1:
        எனில்(data->state.http_neg.allowed & CURL_HTTP_V1x) {
          திரும்பு alpn_rr;
        }
        நிறுத்து;
      இயல்பு: /* ignore */
        நிறுத்து;
      }
    }
  }
#இல்லையெனில்
  (வெறுமை)cf;
  (வெறுமை)data;
  (வெறுமை)not_this_one;
#endif
  திரும்பு ALPN_none;
}

நிலையான எண்ணிக்கை alpnid cf_hc_get_pref_alpn(கட்டமைப்பு Curl_cfilter *cf,
                                       கட்டமைப்பு Curl_easy *data,
                                       எண்ணிக்கை alpnid not_this_one)
{
  எனில்((data->state.http_neg.preferred & data->state.http_neg.allowed)) {
    தேர்வு(data->state.http_neg.preferred) {
    நிலை CURL_HTTP_V3x:
      எனில்(cf_hc_may_h3(cf, data) && (ALPN_h3 != not_this_one))
        திரும்பு ALPN_h3;
      நிறுத்து;
    நிலை CURL_HTTP_V2x:
      எனில்(ALPN_h2 != not_this_one)
        திரும்பு ALPN_h2;
      நிறுத்து;
    நிலை CURL_HTTP_V1x:
      /* If we are trying h2 already, h1 is already used as fallback */
      எனில்((ALPN_h1 != not_this_one) && (ALPN_h2 != not_this_one))
        திரும்பு ALPN_h1;
      நிறுத்து;
    இயல்பு:
      நிறுத்து;
    }
  }
  திரும்பு ALPN_none;
}

நிலையான எண்ணிக்கை alpnid cf_hc_get_first_alpn(கட்டமைப்பு Curl_cfilter *cf,
                                        கட்டமைப்பு Curl_easy *data,
                                        http_majors choices,
                                        எண்ணிக்கை alpnid not_this_one)
{
  /* When told to not try h2, we also do not try h1 and vice versa */
  bool allow_h1_or_h2 = (not_this_one != ALPN_h1) &&
                        (not_this_one != ALPN_h2);
  எனில்((ALPN_h3 != not_this_one) && (choices & CURL_HTTP_V3x) &&
     cf_hc_may_h3(cf, data)) {
    திரும்பு ALPN_h3;
  }
  எனில்(allow_h1_or_h2 && (choices & CURL_HTTP_V2x)) {
    திரும்பு ALPN_h2;
  }
  எனில்(allow_h1_or_h2 && (choices & CURL_HTTP_V1x)) {
    திரும்பு ALPN_h1;
  }
  திரும்பு ALPN_none;
}

நிலையான CURLcode cf_hc_set_baller1(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_hc_ctx *ctx = cf->ctx;
  எண்ணிக்கை alpnid alpn1 = ALPN_none;
  VERBOSE(மாறிலி எழுத்து *source = "HTTPS-RR");

  DEBUGASSERT(cf->conn->bits.tls_enable_alpn);

  alpn1 = cf_hc_get_httpsrr_alpn(cf, data, ALPN_none);
  எனில்(alpn1 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(source = "preferred version");
    alpn1 = cf_hc_get_pref_alpn(cf, data, ALPN_none);
  }
  எனில்(alpn1 == ALPN_none) {
    VERBOSE(source = "wanted versions");
    alpn1 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.wanted,
                                 ALPN_none);
  }
  எனில்(alpn1 == ALPN_none) {
    VERBOSE(source = "allowed versions");
    alpn1 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.allowed,
                                 ALPN_none);
  }

  எனில்(alpn1 == ALPN_none) {
    /* None of the wanted/allowed HTTP versions could be chosen */
    எனில்(ctx->check_h3_result) {
      CURL_TRC_CF(data, cf, "unable to use HTTP/3");
      திரும்பு ctx->check_h3_result;
    }
    CURL_TRC_CF(data, cf, "unable to select HTTP version");
    திரும்பு CURLE_FAILED_INIT;
  }

  cf_hc_baller_assign(&ctx->ballers[0], alpn1, ctx->def_transport);
  ctx->baller_மொத்தம் = 1;
  CURL_TRC_CF(data, cf, "1st attempt uses %s from %s",
              ctx->ballers[0].name, source);

  தேர்வு(alpn1) {
  நிலை ALPN_h1:
    /* We really want h1, switch off h2 to make it disappear in ALPN */
    data->state.http_neg.wanted &= (uint8_t)~CURL_HTTP_V2x;
    நிறுத்து;
  இயல்பு:
    நிறுத்து;
  }

  திரும்பு CURLE_OK;
}

நிலையான வெறுமை cf_hc_set_baller2(கட்டமைப்பு Curl_cfilter *cf,
                              கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_hc_ctx *ctx = cf->ctx;
  எண்ணிக்கை alpnid alpn2 = ALPN_none, alpn1 = ctx->ballers[0].alpn_id;
  VERBOSE(மாறிலி எழுத்து *source = "HTTPS-RR");

  எனில்(ctx->ballers_complete)
    திரும்பு; /* already done */
  எனில்(!ctx->httpsrr_resolved)
    திரும்பு; /* HTTPS-RR pending */

  alpn2 = cf_hc_get_httpsrr_alpn(cf, data, alpn1);
  எனில்(alpn2 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(source = "preferred version");
    alpn2 = cf_hc_get_pref_alpn(cf, data, alpn1);
  }
  எனில்(alpn2 == ALPN_none) {
    VERBOSE(source = "wanted versions");
    alpn2 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.wanted,
                                 alpn1);
  }

  எனில்(alpn2 != ALPN_none) {
    cf_hc_baller_assign(&ctx->ballers[1], alpn2, ctx->def_transport);
    ctx->baller_மொத்தம் = 2;
    CURL_TRC_CF(data, cf, "2nd attempt uses %s from %s",
                ctx->ballers[1].name, source);
  }
  ctx->ballers_complete = TRUE;
}

நிலையான CURLcode cf_hc_connect(கட்டமைப்பு Curl_cfilter *cf,
                              கட்டமைப்பு Curl_easy *data,
                              bool *done)
{
  கட்டமைப்பு cf_hc_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  மாறிலி கட்டமைப்பு curltime *pnow = வெற்று;

  எனில்(cf->connected) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  *done = FALSE;

  எனில்(!ctx->httpsrr_resolved) {
    ctx->httpsrr_resolved = Curl_conn_dns_resolved_https(
      data, cf->sockindex, ctx->destination);
#ifdef DEBUGBUILD
    எனில்(!ctx->httpsrr_resolved && getenv("CURL_DBG_AWAIT_HTTPSRR")) {
      CURL_TRC_CF(data, cf, "awaiting HTTPS-RR");
      திரும்பு CURLE_OK;
    }
#endif
  }

  தேர்வு(ctx->state) {
  நிலை CF_HC_RESOLV:
    ctx->state = CF_HC_INIT;
    FALLTHROUGH();

  நிலை CF_HC_INIT:
    DEBUGASSERT(!cf->next);
    CURL_TRC_CF(data, cf, "connect, init");
    result = cf_hc_set_baller1(cf, data);
    எனில்(result) {
      ctx->result = result;
      ctx->state = CF_HC_FAILURE;
      செல் out;
    }
    cf_hc_set_baller2(cf, data);
    pnow = Curl_pgrs_now(data);
    ctx->started = *pnow;
    cf_hc_baller_init(&ctx->ballers[0], cf, data);
    எனில்((ctx->baller_மொத்தம் > 1) || !ctx->ballers_complete) {
      Curl_expire_set(data, EXPIRE_ALPN_EYEBALLS,
                      ctx->soft_eyeballs_timeout_ms, pnow);
    }
    ctx->state = CF_HC_CONNECT;
    FALLTHROUGH();

  நிலை CF_HC_CONNECT:
    எனில்(!ctx->ballers_complete)
      cf_hc_set_baller2(cf, data);

    எனில்(cf_hc_baller_is_connecting(&ctx->ballers[0])) {
      result = cf_hc_baller_connect(&ctx->ballers[0], cf, data, done);
      எனில்(!result && *done) {
        result = baller_connected(cf, data, &ctx->ballers[0]);
        செல் out;
      }
    }

    pnow = Curl_pgrs_now(data);
    எனில்(time_to_start_baller2(cf, data, pnow)) {
      cf_hc_baller_init(&ctx->ballers[1], cf, data);
    }

    எனில்(cf_hc_baller_is_connecting(&ctx->ballers[1])) {
      result = cf_hc_baller_connect(&ctx->ballers[1], cf, data, done);
      எனில்(!result && *done) {
        result = baller_connected(cf, data, &ctx->ballers[1]);
        செல் out;
      }
    }

    எனில்(ctx->ballers[0].result &&
       (ctx->ballers[1].result ||
        (ctx->ballers_complete && (ctx->baller_மொத்தம் < 2)))) {
      /* all have failed. we give up */
      CURL_TRC_CF(data, cf, "connect, all attempts failed");
      ctx->result = result = ctx->ballers[0].result;
      ctx->state = CF_HC_FAILURE;
      செல் out;
    }
    result = CURLE_OK;
    *done = FALSE;
    நிறுத்து;

  நிலை CF_HC_FAILURE:
    result = ctx->result;
    cf->connected = FALSE;
    *done = FALSE;
    நிறுத்து;

  நிலை CF_HC_SUCCESS:
    result = CURLE_OK;
    cf->connected = TRUE;
    *done = TRUE;
    நிறுத்து;
  }

out:
  CURL_TRC_CF(data, cf, "connect -> %d, done=%d", (முழு)result, *done);
  திரும்பு result;
}

நிலையான CURLcode cf_hc_shutdown(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data, bool *done)
{
  கட்டமைப்பு cf_hc_ctx *ctx = cf->ctx;
  size_t i;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  எனில்(cf->connected) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  க்கு(i = 0; i < ctx->baller_மொத்தம்; i++) {
    கட்டமைப்பு cf_hc_baller *b = &ctx->ballers[i];
    bool bdone = FALSE;
    எனில்(!cf_hc_baller_is_connecting(b) || b->shutdown)
      தொடர்;
    b->result = b->cf->cft->do_shutdown(b->cf, data, &bdone);
    எனில்(b->result || bdone)
      b->shutdown = TRUE; /* treat a failed shutdown as done */
  }

  *done = TRUE;
  க்கு(i = 0; i < ctx->baller_மொத்தம்; i++) {
    எனில்(!ctx->ballers[i].shutdown)
      *done = FALSE;
  }
  எனில்(*done) {
    க்கு(i = 0; i < ctx->baller_மொத்தம்; i++) {
      எனில்(ctx->ballers[i].result)
        result = ctx->ballers[i].result;
    }
  }
  CURL_TRC_CF(data, cf, "shutdown -> %d, done=%d", (முழு)result, *done);
  திரும்பு result;
}

நிலையான CURLcode cf_hc_adjust_pollset(கட்டமைப்பு Curl_cfilter *cf,
                                     கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  எனில்(!cf->connected) {
    கட்டமைப்பு cf_hc_ctx *ctx = cf->ctx;
    size_t i;

    க்கு(i = 0; (i < ctx->baller_மொத்தம்) && !result; i++) {
      கட்டமைப்பு cf_hc_baller *b = &ctx->ballers[i];
      எனில்(!cf_hc_baller_is_connecting(b))
        தொடர்;
      result = Curl_conn_cf_adjust_pollset(b->cf, data, ps);
    }
    CURL_TRC_CF(data, cf, "adjust_pollset -> %d, %u socks", (முழு)result,
                ps->n);
  }
  திரும்பு result;
}

நிலையான bool cf_hc_data_pending(கட்டமைப்பு Curl_cfilter *cf,
                               மாறிலி கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  எனில்(cf->connected)
    திரும்பு cf->next->cft->has_data_pending(cf->next, data);

  க்கு(i = 0; i < ctx->baller_மொத்தம்; i++)
    எனில்(cf_hc_baller_data_pending(&ctx->ballers[i], data))
      திரும்பு TRUE;
  திரும்பு FALSE;
}

நிலையான CURLcode cf_hc_query(கட்டமைப்பு Curl_cfilter *cf,
                            கட்டமைப்பு Curl_easy *data,
                            முழு query, முழு *pres1, வெறுமை *pres2)
{
  கட்டமைப்பு cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  எனில்(!cf->connected) {
    தேர்வு(query) {
    நிலை CF_QUERY_NEED_FLUSH: {
      க்கு(i = 0; i < ctx->baller_மொத்தம்; i++)
        எனில்(cf_hc_baller_needs_flush(&ctx->ballers[i], data)) {
          *pres1 = TRUE;
          திரும்பு CURLE_OK;
        }
      நிறுத்து;
    }
    இயல்பு:
      நிறுத்து;
    }
  }
  திரும்பு cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

நிலையான CURLcode cf_hc_cntrl(கட்டமைப்பு Curl_cfilter *cf,
                            கட்டமைப்பு Curl_easy *data,
                            முழு event, முழு arg1, வெறுமை *arg2)
{
  கட்டமைப்பு cf_hc_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t i;

  எனில்(!cf->connected) {
    தேர்வு(event) {
    நிலை CF_CTRL_REPORT_STATS:
    நிலை CF_CTRL_CONN_INFO_UPDATE:
      க்கு(i = 0; i < ctx->baller_மொத்தம்; i++) {
        /* Make the first baller that connected at network level report */
        எனில்(Curl_conn_cf_is_ip_connected(ctx->ballers[i].cf, data)) {
          Curl_conn_cf_cntrl(ctx->ballers[i].cf, data, TRUE,
                             event, arg1, arg2);
          நிறுத்து;
        }
      }
      நிறுத்து;
    இயல்பு:
      க்கு(i = 0; i < ctx->baller_மொத்தம்; i++) {
        result = cf_hc_baller_cntrl(&ctx->ballers[i], data, event, arg1, arg2);
        எனில்(result && (result != CURLE_AGAIN))
          செல் out;
      }
      result = CURLE_OK;
      நிறுத்து;
    }
  }
out:
  திரும்பு result;
}

நிலையான வெறுமை cf_hc_destroy(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_hc_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  cf_hc_ctx_destroy(data, ctx);
}

கட்டமைப்பு Curl_cftype Curl_cft_http_connect = {
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

நிலையான CURLcode cf_hc_create(கட்டமைப்பு Curl_cfilter **pcf,
                             கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு Curl_peer *destination,
                             uint8_t def_transport)
{
  கட்டமைப்பு Curl_cfilter *cf = வெற்று;
  கட்டமைப்பு cf_hc_ctx *ctx;
  CURLcode result = CURLE_OK;

  ctx = curlx_calloc(1, அளவு(*ctx));
  எனில்(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }
  Curl_peer_link(&ctx->destination, destination);
  ctx->def_transport = def_transport;
  ctx->hard_eyeballs_timeout_ms = data->set.happy_eyeballs_timeout;
  ctx->soft_eyeballs_timeout_ms = data->set.happy_eyeballs_timeout / 2;

  result = Curl_cf_create(&cf, &Curl_cft_http_connect, ctx);
  எனில்(result)
    செல் out;
  ctx = வெற்று;

out:
  *pcf = result ? வெற்று : cf;
  cf_hc_ctx_destroy(data, ctx);
  திரும்பு result;
}

நிலையான CURLcode cf_hc_add(கட்டமைப்பு Curl_easy *data,
                          கட்டமைப்பு Curl_peer *destination,
                          கட்டமைப்பு connectdata *conn,
                          int8_t sockindex,
                          uint8_t def_transport)
{
  கட்டமைப்பு Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_hc_create(&cf, data, destination, def_transport);
  எனில்(result)
    செல் out;
  Curl_conn_cf_add(data, conn, sockindex, cf);

#ifdef USE_HTTPSRR
  result = Curl_conn_dns_add_https_resolve(data, cf->conn, cf->sockindex,
                                           destination);
#endif
out:
  திரும்பு result;
}

CURLcode Curl_cf_https_setup(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு Curl_peer *destination,
                             கட்டமைப்பு connectdata *conn,
                             int8_t sockindex)
{
  CURLcode result = CURLE_OK;

  DEBUGASSERT(conn->scheme->protocol == CURLPROTO_HTTPS);

  /* This filter is intended for HTTPS using ALPN and does
   * not support HTTPS Eyeballing to a proxy. */
  எனில்((conn->scheme->protocol != CURLPROTO_HTTPS) ||
#ifndef CURL_DISABLE_PROXY
     conn->bits.origin_is_proxy ||
#endif
     !conn->bits.tls_enable_alpn)
    செல் out;

  result = cf_hc_add(data, destination, conn, sockindex,
                     conn->transport_wanted);

out:
  திரும்பு result;
}

#endif /* !CURL_DISABLE_HTTP */
