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
#অন্তর্ভাব "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "cf-setup.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "cf-https-connect.h"
#অন্তর্ভাব "http2.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "vdns/cf-dns.h"
#অন্তর্ভাব "vdns/hostip.h"
#অন্তর্ভাব "vdns/httpsrr.h"
#অন্তর্ভাব "vquic/vquic.h"

প্রকারনির্দেশ গণনা {
  CF_HC_RESOLV,
  CF_HC_INIT,
  CF_HC_CONNECT,
  CF_HC_SUCCESS,
  CF_HC_FAILURE
} cf_hc_state;

সংরচনা cf_hc_baller {
  ধ্রুব বর্ণ *name;
  সংরচনা Curl_cfilter *cf;
  CURLcode result;
  পূর্ণাঙ্ক reply_ms;
  uint8_t transport;
  গণনা alpnid alpn_id;
  BIT(shutdown);
};

স্থির শূন্য cf_hc_baller_discard(সংরচনা cf_hc_baller *b,
                                 সংরচনা Curl_easy *data)
{
  যদি(b->cf) {
    Curl_conn_cf_discard_chain(&b->cf, data);
    b->cf = রিক্ত;
  }
}

স্থির bool cf_hc_baller_is_connecting(সংরচনা cf_hc_baller *b)
{
  প্রত্যাগম b->cf && !b->result;
}

স্থির bool cf_hc_baller_has_started(সংরচনা cf_hc_baller *b)
{
  প্রত্যাগম !!b->cf;
}

স্থির পূর্ণাঙ্ক cf_hc_baller_reply_ms(সংরচনা cf_hc_baller *b,
                                 সংরচনা Curl_easy *data)
{
  যদি(b->cf && (b->reply_ms < 0))
    b->cf->cft->query(b->cf, data, CF_QUERY_CONNECT_REPLY_MS,
                      &b->reply_ms, রিক্ত);
  প্রত্যাগম b->reply_ms;
}

স্থির bool cf_hc_baller_data_pending(সংরচনা cf_hc_baller *b,
                                      ধ্রুব সংরচনা Curl_easy *data)
{
  প্রত্যাগম b->cf && !b->result && b->cf->cft->has_data_pending(b->cf, data);
}

স্থির bool cf_hc_baller_needs_flush(সংরচনা cf_hc_baller *b,
                                     সংরচনা Curl_easy *data)
{
  প্রত্যাগম b->cf && !b->result && Curl_conn_cf_needs_flush(b->cf, data);
}

স্থির CURLcode cf_hc_baller_cntrl(সংরচনা cf_hc_baller *b,
                                   সংরচনা Curl_easy *data,
                                   পূর্ণাঙ্ক event, পূর্ণাঙ্ক arg1, শূন্য *arg2)
{
  যদি(b->cf && !b->result)
    প্রত্যাগম Curl_conn_cf_cntrl(b->cf, data, FALSE, event, arg1, arg2);
  প্রত্যাগম CURLE_OK;
}

সংরচনা cf_hc_ctx {
  cf_hc_state state;
  সংরচনা Curl_peer *destination; /* who we ultimately want to talk to */
  সংরচনা curltime started;  /* when connect started */
  CURLcode result;          /* overall result */
  CURLcode check_h3_result;
  সংরচনা cf_hc_baller ballers[2];
  size_t baller_গুনতি;
  timediff_t soft_eyeballs_timeout_ms;
  timediff_t hard_eyeballs_timeout_ms;
  uint8_t def_transport;
  BIT(httpsrr_resolved);
  BIT(checked_h3);
  BIT(ballers_complete);
};

স্থির শূন্য cf_hc_ctx_destroy(সংরচনা Curl_easy *data,
                              সংরচনা cf_hc_ctx *ctx)
{
  যদি(ctx) {
    size_t i;
    প্রতি(i = 0; i < ctx->baller_গুনতি; ++i)
      cf_hc_baller_discard(&ctx->ballers[i], data);
    Curl_peer_unlink(&ctx->destination);
    curlx_free(ctx);
  }
}

স্থির শূন্য cf_hc_baller_assign(সংরচনা cf_hc_baller *b,
                                গণনা alpnid alpn_id,
                                uint8_t def_transport)
{
  b->alpn_id = alpn_id;
  b->transport = def_transport;
  b->cf = রিক্ত;
  b->result = CURLE_OK;
  b->reply_ms = -1;
  b->shutdown = FALSE;
  বিকল্প(b->alpn_id) {
  পক্ষ ALPN_h3:
    b->name = "h3";
    b->transport = TRNSPRT_QUIC;
    ভঙ্গ;
  পক্ষ ALPN_h2:
    b->name = "h2";
    ভঙ্গ;
  পক্ষ ALPN_h1:
    b->name = "h1";
    ভঙ্গ;
  পক্ষ ALPN_none:
    b->name = "no-alpn";
    ভঙ্গ;
  স্বভাব:
    b->result = CURLE_FAILED_INIT;
    ভঙ্গ;
  }
}

স্থির শূন্য cf_hc_baller_init(সংরচনা cf_hc_baller *b,
                              সংরচনা Curl_cfilter *cf,
                              সংরচনা Curl_easy *data)
{
  সংরচনা Curl_cfilter *save = cf->next;

  cf->next = রিক্ত;
  b->result = Curl_cf_setup_ঢোকাও_after(cf, data, b->transport,
                                         CURL_CF_SSL_ENABLE);
  b->cf = cf->next;
  cf->next = save;
}

স্থির CURLcode cf_hc_baller_connect(সংরচনা cf_hc_baller *b,
                                     সংরচনা Curl_cfilter *cf,
                                     সংরচনা Curl_easy *data,
                                     bool *done)
{
  সংরচনা Curl_cfilter *save = cf->next;

  cf->next = b->cf;
  b->result = Curl_conn_cf_connect(cf->next, data, done);
  b->cf = cf->next; /* it might mutate */
  cf->next = save;
  প্রত্যাগম b->result;
}

স্থির CURLcode baller_connected(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 সংরচনা cf_hc_baller *winner)
{
  সংরচনা cf_hc_ctx *ctx = cf->ctx;

  /* Make the winner's connection filter out own sub-filter, check, move,
   * close all remaining. */
  যদি(cf->next) {
    DEBUGASSERT(0);
    প্রত্যাগম CURLE_FAILED_INIT;
  }
  যদি(!winner->cf) {
    DEBUGASSERT(0);
    প্রত্যাগম CURLE_FAILED_INIT;
  }

  cf->next = winner->cf;
  winner->cf = রিক্ত;
  ctx->state = CF_HC_SUCCESS;
  cf->connected = TRUE;

  /* ballers may have failf()'d, the winner resets it, so our
   * errorbuf is clean again. */
  Curl_reset_fail(data);

#ifdef USE_NGHTTP2
  {
    /* For a negotiated HTTP/2 connection insert the h2 filter. */
    ধ্রুব বর্ণ *alpn = Curl_conn_cf_get_alpn_negotiated(cf->next, data);
    যদি(alpn && !strcmp("h2", alpn)) {
      CURLcode result = Curl_http2_switch_at(cf, data);
      যদি(result) {
        ctx->state = CF_HC_FAILURE;
        ctx->result = result;
        প্রত্যাগম result;
      }
    }
  }
#endif
  প্রত্যাগম CURLE_OK;
}

স্থির bool time_to_start_baller2(সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data,
                                  ধ্রুব সংরচনা curltime *pnow)
{
  সংরচনা cf_hc_ctx *ctx = cf->ctx;
  timediff_t elapsed_ms;

  যদি(ctx->baller_গুনতি < 2)
    প্রত্যাগম FALSE;
  অন্যথা যদি(cf_hc_baller_has_started(&ctx->ballers[1]))
    প্রত্যাগম FALSE;
  অন্যথা যদি(ctx->ballers[0].result) {
    CURL_TRC_CF(data, cf, "%s baller failed, starting %s",
                ctx->ballers[0].name, ctx->ballers[1].name);
    প্রত্যাগম TRUE;
  }

  elapsed_ms = curlx_ptimediff_ms(pnow, &ctx->started);
  যদি(elapsed_ms >= ctx->hard_eyeballs_timeout_ms) {
    CURL_TRC_CF(data, cf, "%s inconclusive after %" FMT_TIMEDIFF_T ", "
                "starting %s", ctx->ballers[0].name,
                ctx->hard_eyeballs_timeout_ms, ctx->ballers[1].name);
    প্রত্যাগম TRUE;
  }
  অন্যথা যদি(elapsed_ms >= ctx->soft_eyeballs_timeout_ms) {
    যদি(cf_hc_baller_reply_ms(&ctx->ballers[0], data) < 0) {
      CURL_TRC_CF(data, cf, "%s has not seen any data after %"
                  FMT_TIMEDIFF_T "ms, starting %s",
                  ctx->ballers[0].name, ctx->soft_eyeballs_timeout_ms,
                  ctx->ballers[1].name);
      প্রত্যাগম TRUE;
    }
    /* set the effective hard timeout again */
    Curl_expire_set(data, EXPIRE_ALPN_EYEBALLS,
                    ctx->hard_eyeballs_timeout_ms - elapsed_ms, pnow);
  }
  প্রত্যাগম FALSE;
}

স্থির bool cf_hc_may_h3(সংরচনা Curl_cfilter *cf,
                         সংরচনা Curl_easy *data)
{
  সংরচনা cf_hc_ctx *ctx = cf->ctx;
  যদি(!ctx->checked_h3) {
    ctx->check_h3_result =
      Curl_conn_may_http3(data, cf->conn);
    ctx->checked_h3 = TRUE;
  }
  প্রত্যাগম !ctx->check_h3_result;
}

স্থির গণনা alpnid cf_hc_get_httpsrr_alpn(সংরচনা Curl_cfilter *cf,
                                          সংরচনা Curl_easy *data,
                                          গণনা alpnid not_this_one)
{
#ifdef USE_HTTPSRR
  সংরচনা cf_hc_ctx *ctx = cf->ctx;
  /* Is there an HTTPSRR use its ALPNs here.
   * We are here after having selected a connection to a host+port and
   * can no longer change that. Any HTTPSRR advice for other hosts and ports
   * we need to ignore. */
  ধ্রুব সংরচনা Curl_https_rrinfo *rr;
  size_t i;

  /* Do we have HTTPS-RR information? */
  rr = Curl_conn_dns_get_https(data, cf->sockindex, ctx->destination);

  CURL_TRC_CF(data, cf, "HTTPS-RR %savailable", rr ? "" : "not ");
  /* We do not support `rr->no_def_alpn`. */
  যদি(Curl_httpsrr_applicable(data, rr) && !rr->no_def_alpn) {
    প্রতি(i = 0; i < CURL_ARRAYSIZE(rr->alpns); ++i) {
      গণনা alpnid alpn_rr = (গণনা alpnid)rr->alpns[i];
      যদি(alpn_rr == not_this_one) /* do not want this one */
        অনুবর্ত;
      বিকল্প(alpn_rr) {
      পক্ষ ALPN_h3:
        যদি((data->state.http_neg.allowed & CURL_HTTP_V3x) &&
           cf_hc_may_h3(cf, data)) {
          প্রত্যাগম alpn_rr;
        }
        ভঙ্গ;
      পক্ষ ALPN_h2:
        যদি(data->state.http_neg.allowed & CURL_HTTP_V2x) {
          প্রত্যাগম alpn_rr;
        }
        ভঙ্গ;
      পক্ষ ALPN_h1:
        যদি(data->state.http_neg.allowed & CURL_HTTP_V1x) {
          প্রত্যাগম alpn_rr;
        }
        ভঙ্গ;
      স্বভাব: /* ignore */
        ভঙ্গ;
      }
    }
  }
#অন্যথা
  (শূন্য)cf;
  (শূন্য)data;
  (শূন্য)not_this_one;
#endif
  প্রত্যাগম ALPN_none;
}

স্থির গণনা alpnid cf_hc_get_pref_alpn(সংরচনা Curl_cfilter *cf,
                                       সংরচনা Curl_easy *data,
                                       গণনা alpnid not_this_one)
{
  যদি((data->state.http_neg.preferred & data->state.http_neg.allowed)) {
    বিকল্প(data->state.http_neg.preferred) {
    পক্ষ CURL_HTTP_V3x:
      যদি(cf_hc_may_h3(cf, data) && (ALPN_h3 != not_this_one))
        প্রত্যাগম ALPN_h3;
      ভঙ্গ;
    পক্ষ CURL_HTTP_V2x:
      যদি(ALPN_h2 != not_this_one)
        প্রত্যাগম ALPN_h2;
      ভঙ্গ;
    পক্ষ CURL_HTTP_V1x:
      /* If we are trying h2 already, h1 is already used as fallback */
      যদি((ALPN_h1 != not_this_one) && (ALPN_h2 != not_this_one))
        প্রত্যাগম ALPN_h1;
      ভঙ্গ;
    স্বভাব:
      ভঙ্গ;
    }
  }
  প্রত্যাগম ALPN_none;
}

স্থির গণনা alpnid cf_hc_get_first_alpn(সংরচনা Curl_cfilter *cf,
                                        সংরচনা Curl_easy *data,
                                        http_majors choices,
                                        গণনা alpnid not_this_one)
{
  /* When told to not try h2, we also do not try h1 and vice versa */
  bool allow_h1_or_h2 = (not_this_one != ALPN_h1) &&
                        (not_this_one != ALPN_h2);
  যদি((ALPN_h3 != not_this_one) && (choices & CURL_HTTP_V3x) &&
     cf_hc_may_h3(cf, data)) {
    প্রত্যাগম ALPN_h3;
  }
  যদি(allow_h1_or_h2 && (choices & CURL_HTTP_V2x)) {
    প্রত্যাগম ALPN_h2;
  }
  যদি(allow_h1_or_h2 && (choices & CURL_HTTP_V1x)) {
    প্রত্যাগম ALPN_h1;
  }
  প্রত্যাগম ALPN_none;
}

স্থির CURLcode cf_hc_set_baller1(সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data)
{
  সংরচনা cf_hc_ctx *ctx = cf->ctx;
  গণনা alpnid alpn1 = ALPN_none;
  VERBOSE(ধ্রুব বর্ণ *source = "HTTPS-RR");

  DEBUGASSERT(cf->conn->bits.tls_enable_alpn);

  alpn1 = cf_hc_get_httpsrr_alpn(cf, data, ALPN_none);
  যদি(alpn1 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(source = "preferred version");
    alpn1 = cf_hc_get_pref_alpn(cf, data, ALPN_none);
  }
  যদি(alpn1 == ALPN_none) {
    VERBOSE(source = "wanted versions");
    alpn1 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.wanted,
                                 ALPN_none);
  }
  যদি(alpn1 == ALPN_none) {
    VERBOSE(source = "allowed versions");
    alpn1 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.allowed,
                                 ALPN_none);
  }

  যদি(alpn1 == ALPN_none) {
    /* None of the wanted/allowed HTTP versions could be chosen */
    যদি(ctx->check_h3_result) {
      CURL_TRC_CF(data, cf, "unable to use HTTP/3");
      প্রত্যাগম ctx->check_h3_result;
    }
    CURL_TRC_CF(data, cf, "unable to select HTTP version");
    প্রত্যাগম CURLE_FAILED_INIT;
  }

  cf_hc_baller_assign(&ctx->ballers[0], alpn1, ctx->def_transport);
  ctx->baller_গুনতি = 1;
  CURL_TRC_CF(data, cf, "1st attempt uses %s from %s",
              ctx->ballers[0].name, source);

  বিকল্প(alpn1) {
  পক্ষ ALPN_h1:
    /* We really want h1, switch off h2 to make it disappear in ALPN */
    data->state.http_neg.wanted &= (uint8_t)~CURL_HTTP_V2x;
    ভঙ্গ;
  স্বভাব:
    ভঙ্গ;
  }

  প্রত্যাগম CURLE_OK;
}

স্থির শূন্য cf_hc_set_baller2(সংরচনা Curl_cfilter *cf,
                              সংরচনা Curl_easy *data)
{
  সংরচনা cf_hc_ctx *ctx = cf->ctx;
  গণনা alpnid alpn2 = ALPN_none, alpn1 = ctx->ballers[0].alpn_id;
  VERBOSE(ধ্রুব বর্ণ *source = "HTTPS-RR");

  যদি(ctx->ballers_complete)
    প্রত্যাগম; /* already done */
  যদি(!ctx->httpsrr_resolved)
    প্রত্যাগম; /* HTTPS-RR pending */

  alpn2 = cf_hc_get_httpsrr_alpn(cf, data, alpn1);
  যদি(alpn2 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(source = "preferred version");
    alpn2 = cf_hc_get_pref_alpn(cf, data, alpn1);
  }
  যদি(alpn2 == ALPN_none) {
    VERBOSE(source = "wanted versions");
    alpn2 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.wanted,
                                 alpn1);
  }

  যদি(alpn2 != ALPN_none) {
    cf_hc_baller_assign(&ctx->ballers[1], alpn2, ctx->def_transport);
    ctx->baller_গুনতি = 2;
    CURL_TRC_CF(data, cf, "2nd attempt uses %s from %s",
                ctx->ballers[1].name, source);
  }
  ctx->ballers_complete = TRUE;
}

স্থির CURLcode cf_hc_connect(সংরচনা Curl_cfilter *cf,
                              সংরচনা Curl_easy *data,
                              bool *done)
{
  সংরচনা cf_hc_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ধ্রুব সংরচনা curltime *pnow = রিক্ত;

  যদি(cf->connected) {
    *done = TRUE;
    প্রত্যাগম CURLE_OK;
  }

  *done = FALSE;

  যদি(!ctx->httpsrr_resolved) {
    ctx->httpsrr_resolved = Curl_conn_dns_resolved_https(
      data, cf->sockindex, ctx->destination);
#ifdef DEBUGBUILD
    যদি(!ctx->httpsrr_resolved && getenv("CURL_DBG_AWAIT_HTTPSRR")) {
      CURL_TRC_CF(data, cf, "awaiting HTTPS-RR");
      প্রত্যাগম CURLE_OK;
    }
#endif
  }

  বিকল্প(ctx->state) {
  পক্ষ CF_HC_RESOLV:
    ctx->state = CF_HC_INIT;
    FALLTHROUGH();

  পক্ষ CF_HC_INIT:
    DEBUGASSERT(!cf->next);
    CURL_TRC_CF(data, cf, "connect, init");
    result = cf_hc_set_baller1(cf, data);
    যদি(result) {
      ctx->result = result;
      ctx->state = CF_HC_FAILURE;
      গচ্ছ out;
    }
    cf_hc_set_baller2(cf, data);
    pnow = Curl_pgrs_now(data);
    ctx->started = *pnow;
    cf_hc_baller_init(&ctx->ballers[0], cf, data);
    যদি((ctx->baller_গুনতি > 1) || !ctx->ballers_complete) {
      Curl_expire_set(data, EXPIRE_ALPN_EYEBALLS,
                      ctx->soft_eyeballs_timeout_ms, pnow);
    }
    ctx->state = CF_HC_CONNECT;
    FALLTHROUGH();

  পক্ষ CF_HC_CONNECT:
    যদি(!ctx->ballers_complete)
      cf_hc_set_baller2(cf, data);

    যদি(cf_hc_baller_is_connecting(&ctx->ballers[0])) {
      result = cf_hc_baller_connect(&ctx->ballers[0], cf, data, done);
      যদি(!result && *done) {
        result = baller_connected(cf, data, &ctx->ballers[0]);
        গচ্ছ out;
      }
    }

    pnow = Curl_pgrs_now(data);
    যদি(time_to_start_baller2(cf, data, pnow)) {
      cf_hc_baller_init(&ctx->ballers[1], cf, data);
    }

    যদি(cf_hc_baller_is_connecting(&ctx->ballers[1])) {
      result = cf_hc_baller_connect(&ctx->ballers[1], cf, data, done);
      যদি(!result && *done) {
        result = baller_connected(cf, data, &ctx->ballers[1]);
        গচ্ছ out;
      }
    }

    যদি(ctx->ballers[0].result &&
       (ctx->ballers[1].result ||
        (ctx->ballers_complete && (ctx->baller_গুনতি < 2)))) {
      /* all have failed. we give up */
      CURL_TRC_CF(data, cf, "connect, all attempts failed");
      ctx->result = result = ctx->ballers[0].result;
      ctx->state = CF_HC_FAILURE;
      গচ্ছ out;
    }
    result = CURLE_OK;
    *done = FALSE;
    ভঙ্গ;

  পক্ষ CF_HC_FAILURE:
    result = ctx->result;
    cf->connected = FALSE;
    *done = FALSE;
    ভঙ্গ;

  পক্ষ CF_HC_SUCCESS:
    result = CURLE_OK;
    cf->connected = TRUE;
    *done = TRUE;
    ভঙ্গ;
  }

out:
  CURL_TRC_CF(data, cf, "connect -> %d, done=%d", (পূর্ণাঙ্ক)result, *done);
  প্রত্যাগম result;
}

স্থির CURLcode cf_hc_shutdown(সংরচনা Curl_cfilter *cf,
                               সংরচনা Curl_easy *data, bool *done)
{
  সংরচনা cf_hc_ctx *ctx = cf->ctx;
  size_t i;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  যদি(cf->connected) {
    *done = TRUE;
    প্রত্যাগম CURLE_OK;
  }

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  প্রতি(i = 0; i < ctx->baller_গুনতি; i++) {
    সংরচনা cf_hc_baller *b = &ctx->ballers[i];
    bool bdone = FALSE;
    যদি(!cf_hc_baller_is_connecting(b) || b->shutdown)
      অনুবর্ত;
    b->result = b->cf->cft->do_shutdown(b->cf, data, &bdone);
    যদি(b->result || bdone)
      b->shutdown = TRUE; /* treat a failed shutdown as done */
  }

  *done = TRUE;
  প্রতি(i = 0; i < ctx->baller_গুনতি; i++) {
    যদি(!ctx->ballers[i].shutdown)
      *done = FALSE;
  }
  যদি(*done) {
    প্রতি(i = 0; i < ctx->baller_গুনতি; i++) {
      যদি(ctx->ballers[i].result)
        result = ctx->ballers[i].result;
    }
  }
  CURL_TRC_CF(data, cf, "shutdown -> %d, done=%d", (পূর্ণাঙ্ক)result, *done);
  প্রত্যাগম result;
}

স্থির CURLcode cf_hc_adjust_pollset(সংরচনা Curl_cfilter *cf,
                                     সংরচনা Curl_easy *data,
                                     সংরচনা easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  যদি(!cf->connected) {
    সংরচনা cf_hc_ctx *ctx = cf->ctx;
    size_t i;

    প্রতি(i = 0; (i < ctx->baller_গুনতি) && !result; i++) {
      সংরচনা cf_hc_baller *b = &ctx->ballers[i];
      যদি(!cf_hc_baller_is_connecting(b))
        অনুবর্ত;
      result = Curl_conn_cf_adjust_pollset(b->cf, data, ps);
    }
    CURL_TRC_CF(data, cf, "adjust_pollset -> %d, %u socks", (পূর্ণাঙ্ক)result,
                ps->n);
  }
  প্রত্যাগম result;
}

স্থির bool cf_hc_data_pending(সংরচনা Curl_cfilter *cf,
                               ধ্রুব সংরচনা Curl_easy *data)
{
  সংরচনা cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  যদি(cf->connected)
    প্রত্যাগম cf->next->cft->has_data_pending(cf->next, data);

  প্রতি(i = 0; i < ctx->baller_গুনতি; i++)
    যদি(cf_hc_baller_data_pending(&ctx->ballers[i], data))
      প্রত্যাগম TRUE;
  প্রত্যাগম FALSE;
}

স্থির CURLcode cf_hc_query(সংরচনা Curl_cfilter *cf,
                            সংরচনা Curl_easy *data,
                            পূর্ণাঙ্ক query, পূর্ণাঙ্ক *pres1, শূন্য *pres2)
{
  সংরচনা cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  যদি(!cf->connected) {
    বিকল্প(query) {
    পক্ষ CF_QUERY_NEED_FLUSH: {
      প্রতি(i = 0; i < ctx->baller_গুনতি; i++)
        যদি(cf_hc_baller_needs_flush(&ctx->ballers[i], data)) {
          *pres1 = TRUE;
          প্রত্যাগম CURLE_OK;
        }
      ভঙ্গ;
    }
    স্বভাব:
      ভঙ্গ;
    }
  }
  প্রত্যাগম cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

স্থির CURLcode cf_hc_cntrl(সংরচনা Curl_cfilter *cf,
                            সংরচনা Curl_easy *data,
                            পূর্ণাঙ্ক event, পূর্ণাঙ্ক arg1, শূন্য *arg2)
{
  সংরচনা cf_hc_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t i;

  যদি(!cf->connected) {
    বিকল্প(event) {
    পক্ষ CF_CTRL_REPORT_STATS:
    পক্ষ CF_CTRL_CONN_INFO_UPDATE:
      প্রতি(i = 0; i < ctx->baller_গুনতি; i++) {
        /* Make the first baller that connected at network level report */
        যদি(Curl_conn_cf_is_ip_connected(ctx->ballers[i].cf, data)) {
          Curl_conn_cf_cntrl(ctx->ballers[i].cf, data, TRUE,
                             event, arg1, arg2);
          ভঙ্গ;
        }
      }
      ভঙ্গ;
    স্বভাব:
      প্রতি(i = 0; i < ctx->baller_গুনতি; i++) {
        result = cf_hc_baller_cntrl(&ctx->ballers[i], data, event, arg1, arg2);
        যদি(result && (result != CURLE_AGAIN))
          গচ্ছ out;
      }
      result = CURLE_OK;
      ভঙ্গ;
    }
  }
out:
  প্রত্যাগম result;
}

স্থির শূন্য cf_hc_destroy(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data)
{
  সংরচনা cf_hc_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  cf_hc_ctx_destroy(data, ctx);
}

সংরচনা Curl_cftype Curl_cft_http_connect = {
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

স্থির CURLcode cf_hc_create(সংরচনা Curl_cfilter **pcf,
                             সংরচনা Curl_easy *data,
                             সংরচনা Curl_peer *destination,
                             uint8_t def_transport)
{
  সংরচনা Curl_cfilter *cf = রিক্ত;
  সংরচনা cf_hc_ctx *ctx;
  CURLcode result = CURLE_OK;

  ctx = curlx_calloc(1, পরিমাণ(*ctx));
  যদি(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }
  Curl_peer_link(&ctx->destination, destination);
  ctx->def_transport = def_transport;
  ctx->hard_eyeballs_timeout_ms = data->set.happy_eyeballs_timeout;
  ctx->soft_eyeballs_timeout_ms = data->set.happy_eyeballs_timeout / 2;

  result = Curl_cf_create(&cf, &Curl_cft_http_connect, ctx);
  যদি(result)
    গচ্ছ out;
  ctx = রিক্ত;

out:
  *pcf = result ? রিক্ত : cf;
  cf_hc_ctx_destroy(data, ctx);
  প্রত্যাগম result;
}

স্থির CURLcode cf_hc_add(সংরচনা Curl_easy *data,
                          সংরচনা Curl_peer *destination,
                          সংরচনা connectdata *conn,
                          int8_t sockindex,
                          uint8_t def_transport)
{
  সংরচনা Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_hc_create(&cf, data, destination, def_transport);
  যদি(result)
    গচ্ছ out;
  Curl_conn_cf_add(data, conn, sockindex, cf);

#ifdef USE_HTTPSRR
  result = Curl_conn_dns_add_https_resolve(data, cf->conn, cf->sockindex,
                                           destination);
#endif
out:
  প্রত্যাগম result;
}

CURLcode Curl_cf_https_setup(সংরচনা Curl_easy *data,
                             সংরচনা Curl_peer *destination,
                             সংরচনা connectdata *conn,
                             int8_t sockindex)
{
  CURLcode result = CURLE_OK;

  DEBUGASSERT(conn->scheme->protocol == CURLPROTO_HTTPS);

  /* This filter is intended for HTTPS using ALPN and does
   * not support HTTPS Eyeballing to a proxy. */
  যদি((conn->scheme->protocol != CURLPROTO_HTTPS) ||
#ifndef CURL_DISABLE_PROXY
     conn->bits.origin_is_proxy ||
#endif
     !conn->bits.tls_enable_alpn)
    গচ্ছ out;

  result = cf_hc_add(data, destination, conn, sockindex,
                     conn->transport_wanted);

out:
  প্রত্যাগম result;
}

#endif /* !CURL_DISABLE_HTTP */
