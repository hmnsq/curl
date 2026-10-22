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
#شامل "curl_setup.h"

#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP

#شامل "urldata.h"
#شامل "curl_trc.h"
#شامل "cfilters.h"
#شامل "cf-setup.h"
#شامل "connect.h"
#شامل "multiif.h"
#شامل "cf-https-connect.h"
#شامل "http2.h"
#شامل "progress.h"
#شامل "select.h"
#شامل "vdns/cf-dns.h"
#شامل "vdns/hostip.h"
#شامل "vdns/httpsrr.h"
#شامل "vquic/vquic.h"

قسمتعریف شمار {
  CF_HC_RESOLV,
  CF_HC_INIT,
  CF_HC_CONNECT,
  CF_HC_SUCCESS,
  CF_HC_FAILURE
} cf_hc_state;

ساخت cf_hc_baller {
  ثابت حرف *name;
  ساخت Curl_cfilter *cf;
  CURLcode نتیجہ;
  صحیح جواب_ms;
  uint8_t transport;
  شمار alpnid alpn_id;
  ثنائی_ہندسہ(shutdown);
};

ساکن خالی cf_hc_baller_discard(ساخت cf_hc_baller *b,
                                 ساخت Curl_easy *ڈیٹا)
{
  اگر(b->cf) {
    Curl_conn_cf_discard_chain(&b->cf, ڈیٹا);
    b->cf = تہی;
  }
}

ساکن bool cf_hc_baller_is_connecting(ساخت cf_hc_baller *b)
{
  واپس b->cf && !b->نتیجہ;
}

ساکن bool cf_hc_baller_has_started(ساخت cf_hc_baller *b)
{
  واپس !!b->cf;
}

ساکن صحیح cf_hc_baller_جواب_ms(ساخت cf_hc_baller *b,
                                 ساخت Curl_easy *ڈیٹا)
{
  اگر(b->cf && (b->جواب_ms < 0))
    b->cf->cft->query(b->cf, ڈیٹا, CF_QUERY_CONNECT_جواب_MS,
                      &b->جواب_ms, تہی);
  واپس b->جواب_ms;
}

ساکن bool cf_hc_baller_ڈیٹا_pending(ساخت cf_hc_baller *b,
                                      ثابت ساخت Curl_easy *ڈیٹا)
{
  واپس b->cf && !b->نتیجہ && b->cf->cft->has_ڈیٹا_pending(b->cf, ڈیٹا);
}

ساکن bool cf_hc_baller_needs_flush(ساخت cf_hc_baller *b,
                                     ساخت Curl_easy *ڈیٹا)
{
  واپس b->cf && !b->نتیجہ && Curl_conn_cf_needs_flush(b->cf, ڈیٹا);
}

ساکن CURLcode cf_hc_baller_cntrl(ساخت cf_hc_baller *b,
                                   ساخت Curl_easy *ڈیٹا,
                                   صحیح event, صحیح arg1, خالی *arg2)
{
  اگر(b->cf && !b->نتیجہ)
    واپس Curl_conn_cf_cntrl(b->cf, ڈیٹا, FALSE, event, arg1, arg2);
  واپس CURLE_OK;
}

ساخت cf_hc_ctx {
  cf_hc_state state;
  ساخت Curl_peer *destination; /* who we ultimately want to talk to */
  ساخت curltime started;  /* when connect started */
  CURLcode نتیجہ;          /* overall result */
  CURLcode check_h3_نتیجہ;
  ساخت cf_hc_baller ballers[2];
  size_t baller_گنتی;
  timediff_t soft_eyeballs_timeout_ms;
  timediff_t hard_eyeballs_timeout_ms;
  uint8_t def_transport;
  ثنائی_ہندسہ(httpsrr_resolved);
  ثنائی_ہندسہ(checked_h3);
  ثنائی_ہندسہ(ballers_complete);
};

ساکن خالی cf_hc_ctx_destroy(ساخت Curl_easy *ڈیٹا,
                              ساخت cf_hc_ctx *ctx)
{
  اگر(ctx) {
    size_t i;
    برائے(i = 0; i < ctx->baller_گنتی; ++i)
      cf_hc_baller_discard(&ctx->ballers[i], ڈیٹا);
    Curl_peer_unlink(&ctx->destination);
    curlx_free(ctx);
  }
}

ساکن خالی cf_hc_baller_تفویض(ساخت cf_hc_baller *b,
                                شمار alpnid alpn_id,
                                uint8_t def_transport)
{
  b->alpn_id = alpn_id;
  b->transport = def_transport;
  b->cf = تہی;
  b->نتیجہ = CURLE_OK;
  b->جواب_ms = -1;
  b->shutdown = FALSE;
  انتخاب(b->alpn_id) {
  حالت ALPN_h3:
    b->name = "h3";
    b->transport = TRNSPRT_QUIC;
    توڑو;
  حالت ALPN_h2:
    b->name = "h2";
    توڑو;
  حالت ALPN_h1:
    b->name = "h1";
    توڑو;
  حالت ALPN_none:
    b->name = "no-alpn";
    توڑو;
  طےشدہ:
    b->نتیجہ = CURLE_FAILED_INIT;
    توڑو;
  }
}

ساکن خالی cf_hc_baller_init(ساخت cf_hc_baller *b,
                              ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_cfilter *save = cf->اگلا;

  cf->اگلا = تہی;
  b->نتیجہ = Curl_cf_تیاری_داخل_کرو_after(cf, ڈیٹا, b->transport,
                                         CURL_CF_SSL_ENABLE);
  b->cf = cf->اگلا;
  cf->اگلا = save;
}

ساکن CURLcode cf_hc_baller_connect(ساخت cf_hc_baller *b,
                                     ساخت Curl_cfilter *cf,
                                     ساخت Curl_easy *ڈیٹا,
                                     bool *مکمل)
{
  ساخت Curl_cfilter *save = cf->اگلا;

  cf->اگلا = b->cf;
  b->نتیجہ = Curl_conn_cf_connect(cf->اگلا, ڈیٹا, مکمل);
  b->cf = cf->اگلا; /* it might mutate */
  cf->اگلا = save;
  واپس b->نتیجہ;
}

ساکن CURLcode baller_connected(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 ساخت cf_hc_baller *winner)
{
  ساخت cf_hc_ctx *ctx = cf->ctx;

  /* Make the winner's connection filter out own sub-filter, check, move,
   * close all remaining. */
  اگر(cf->اگلا) {
    DEBUGASSERT(0);
    واپس CURLE_FAILED_INIT;
  }
  اگر(!winner->cf) {
    DEBUGASSERT(0);
    واپس CURLE_FAILED_INIT;
  }

  cf->اگلا = winner->cf;
  winner->cf = تہی;
  ctx->state = CF_HC_SUCCESS;
  cf->connected = TRUE;

  /* ballers may have failf()'d, the winner resets it, so our
   * errorbuf is clean again. */
  Curl_reset_fail(ڈیٹا);

#اگر_تعریف_شدہ USE_NGHTTP2
  {
    /* For a negotiated HTTP/2 connection insert the h2 filter. */
    ثابت حرف *alpn = Curl_conn_cf_get_alpn_negotiated(cf->اگلا, ڈیٹا);
    اگر(alpn && !strcmp("h2", alpn)) {
      CURLcode نتیجہ = Curl_http2_switch_at(cf, ڈیٹا);
      اگر(نتیجہ) {
        ctx->state = CF_HC_FAILURE;
        ctx->نتیجہ = نتیجہ;
        واپس نتیجہ;
      }
    }
  }
#اگر_ختم
  واپس CURLE_OK;
}

ساکن bool وقت_مقصد_آغاز_baller2(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  ثابت ساخت curltime *pnow)
{
  ساخت cf_hc_ctx *ctx = cf->ctx;
  timediff_t elapsed_ms;

  اگر(ctx->baller_گنتی < 2)
    واپس FALSE;
  ورنہ اگر(cf_hc_baller_has_started(&ctx->ballers[1]))
    واپس FALSE;
  ورنہ اگر(ctx->ballers[0].نتیجہ) {
    CURL_TRC_CF(ڈیٹا, cf, "%s baller failed, starting %s",
                ctx->ballers[0].name, ctx->ballers[1].name);
    واپس TRUE;
  }

  elapsed_ms = curlx_ptimediff_ms(pnow, &ctx->started);
  اگر(elapsed_ms >= ctx->hard_eyeballs_timeout_ms) {
    CURL_TRC_CF(ڈیٹا, cf, "%s inconclusive after %" FMT_TIMEDIFF_T ", "
                "starting %s", ctx->ballers[0].name,
                ctx->hard_eyeballs_timeout_ms, ctx->ballers[1].name);
    واپس TRUE;
  }
  ورنہ اگر(elapsed_ms >= ctx->soft_eyeballs_timeout_ms) {
    اگر(cf_hc_baller_جواب_ms(&ctx->ballers[0], ڈیٹا) < 0) {
      CURL_TRC_CF(ڈیٹا, cf, "%s has not seen any data after %"
                  FMT_TIMEDIFF_T "ms, starting %s",
                  ctx->ballers[0].name, ctx->soft_eyeballs_timeout_ms,
                  ctx->ballers[1].name);
      واپس TRUE;
    }
    /* set the effective hard timeout again */
    Curl_expire_سیٹ(ڈیٹا, EXPIRE_ALPN_EYEBALLS,
                    ctx->hard_eyeballs_timeout_ms - elapsed_ms, pnow);
  }
  واپس FALSE;
}

ساکن bool cf_hc_may_h3(ساخت Curl_cfilter *cf,
                         ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_hc_ctx *ctx = cf->ctx;
  اگر(!ctx->checked_h3) {
    ctx->check_h3_نتیجہ =
      Curl_conn_may_http3(ڈیٹا, cf->conn);
    ctx->checked_h3 = TRUE;
  }
  واپس !ctx->check_h3_نتیجہ;
}

ساکن شمار alpnid cf_hc_get_httpsrr_alpn(ساخت Curl_cfilter *cf,
                                          ساخت Curl_easy *ڈیٹا,
                                          شمار alpnid not_this_one)
{
#اگر_تعریف_شدہ USE_HTTPSRR
  ساخت cf_hc_ctx *ctx = cf->ctx;
  /* Is there an HTTPSRR use its ALPNs here.
   * We are here after having selected a connection to a host+port and
   * can no longer change that. Any HTTPSRR advice for other hosts and ports
   * we need to ignore. */
  ثابت ساخت Curl_https_rrinfo *rr;
  size_t i;

  /* Do we have HTTPS-RR information? */
  rr = Curl_conn_dns_get_https(ڈیٹا, cf->sockindex, ctx->destination);

  CURL_TRC_CF(ڈیٹا, cf, "HTTPS-RR %savailable", rr ? "" : "not ");
  /* We do not support `rr->no_def_alpn`. */
  اگر(Curl_httpsrr_applicable(ڈیٹا, rr) && !rr->no_def_alpn) {
    برائے(i = 0; i < CURL_ARRAYSIZE(rr->alpns); ++i) {
      شمار alpnid alpn_rr = (شمار alpnid)rr->alpns[i];
      اگر(alpn_rr == not_this_one) /* do not want this one */
        جاری;
      انتخاب(alpn_rr) {
      حالت ALPN_h3:
        اگر((ڈیٹا->state.http_neg.allowed & CURL_HTTP_V3x) &&
           cf_hc_may_h3(cf, ڈیٹا)) {
          واپس alpn_rr;
        }
        توڑو;
      حالت ALPN_h2:
        اگر(ڈیٹا->state.http_neg.allowed & CURL_HTTP_V2x) {
          واپس alpn_rr;
        }
        توڑو;
      حالت ALPN_h1:
        اگر(ڈیٹا->state.http_neg.allowed & CURL_HTTP_V1x) {
          واپس alpn_rr;
        }
        توڑو;
      طےشدہ: /* ignore */
        توڑو;
      }
    }
  }
#ورنہ
  (خالی)cf;
  (خالی)ڈیٹا;
  (خالی)not_this_one;
#اگر_ختم
  واپس ALPN_none;
}

ساکن شمار alpnid cf_hc_get_pref_alpn(ساخت Curl_cfilter *cf,
                                       ساخت Curl_easy *ڈیٹا,
                                       شمار alpnid not_this_one)
{
  اگر((ڈیٹا->state.http_neg.preferred & ڈیٹا->state.http_neg.allowed)) {
    انتخاب(ڈیٹا->state.http_neg.preferred) {
    حالت CURL_HTTP_V3x:
      اگر(cf_hc_may_h3(cf, ڈیٹا) && (ALPN_h3 != not_this_one))
        واپس ALPN_h3;
      توڑو;
    حالت CURL_HTTP_V2x:
      اگر(ALPN_h2 != not_this_one)
        واپس ALPN_h2;
      توڑو;
    حالت CURL_HTTP_V1x:
      /* If we are trying h2 already, h1 is already used as fallback */
      اگر((ALPN_h1 != not_this_one) && (ALPN_h2 != not_this_one))
        واپس ALPN_h1;
      توڑو;
    طےشدہ:
      توڑو;
    }
  }
  واپس ALPN_none;
}

ساکن شمار alpnid cf_hc_get_پہلا_alpn(ساخت Curl_cfilter *cf,
                                        ساخت Curl_easy *ڈیٹا,
                                        http_majors choices,
                                        شمار alpnid not_this_one)
{
  /* When told to not try h2, we also do not try h1 and vice versa */
  bool allow_h1_or_h2 = (not_this_one != ALPN_h1) &&
                        (not_this_one != ALPN_h2);
  اگر((ALPN_h3 != not_this_one) && (choices & CURL_HTTP_V3x) &&
     cf_hc_may_h3(cf, ڈیٹا)) {
    واپس ALPN_h3;
  }
  اگر(allow_h1_or_h2 && (choices & CURL_HTTP_V2x)) {
    واپس ALPN_h2;
  }
  اگر(allow_h1_or_h2 && (choices & CURL_HTTP_V1x)) {
    واپس ALPN_h1;
  }
  واپس ALPN_none;
}

ساکن CURLcode cf_hc_سیٹ_baller1(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_hc_ctx *ctx = cf->ctx;
  شمار alpnid alpn1 = ALPN_none;
  VERBOSE(ثابت حرف *ماخذ = "HTTPS-RR");

  DEBUGASSERT(cf->conn->بٹ.tls_enable_alpn);

  alpn1 = cf_hc_get_httpsrr_alpn(cf, ڈیٹا, ALPN_none);
  اگر(alpn1 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(ماخذ = "preferred version");
    alpn1 = cf_hc_get_pref_alpn(cf, ڈیٹا, ALPN_none);
  }
  اگر(alpn1 == ALPN_none) {
    VERBOSE(ماخذ = "wanted versions");
    alpn1 = cf_hc_get_پہلا_alpn(cf, ڈیٹا,
                                 ڈیٹا->state.http_neg.wanted,
                                 ALPN_none);
  }
  اگر(alpn1 == ALPN_none) {
    VERBOSE(ماخذ = "allowed versions");
    alpn1 = cf_hc_get_پہلا_alpn(cf, ڈیٹا,
                                 ڈیٹا->state.http_neg.allowed,
                                 ALPN_none);
  }

  اگر(alpn1 == ALPN_none) {
    /* None of the wanted/allowed HTTP versions could be chosen */
    اگر(ctx->check_h3_نتیجہ) {
      CURL_TRC_CF(ڈیٹا, cf, "unable to use HTTP/3");
      واپس ctx->check_h3_نتیجہ;
    }
    CURL_TRC_CF(ڈیٹا, cf, "unable to select HTTP version");
    واپس CURLE_FAILED_INIT;
  }

  cf_hc_baller_تفویض(&ctx->ballers[0], alpn1, ctx->def_transport);
  ctx->baller_گنتی = 1;
  CURL_TRC_CF(ڈیٹا, cf, "1st attempt uses %s from %s",
              ctx->ballers[0].name, ماخذ);

  انتخاب(alpn1) {
  حالت ALPN_h1:
    /* We really want h1, switch off h2 to make it disappear in ALPN */
    ڈیٹا->state.http_neg.wanted &= (uint8_t)~CURL_HTTP_V2x;
    توڑو;
  طےشدہ:
    توڑو;
  }

  واپس CURLE_OK;
}

ساکن خالی cf_hc_سیٹ_baller2(ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_hc_ctx *ctx = cf->ctx;
  شمار alpnid alpn2 = ALPN_none, alpn1 = ctx->ballers[0].alpn_id;
  VERBOSE(ثابت حرف *ماخذ = "HTTPS-RR");

  اگر(ctx->ballers_complete)
    واپس; /* already done */
  اگر(!ctx->httpsrr_resolved)
    واپس; /* HTTPS-RR pending */

  alpn2 = cf_hc_get_httpsrr_alpn(cf, ڈیٹا, alpn1);
  اگر(alpn2 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(ماخذ = "preferred version");
    alpn2 = cf_hc_get_pref_alpn(cf, ڈیٹا, alpn1);
  }
  اگر(alpn2 == ALPN_none) {
    VERBOSE(ماخذ = "wanted versions");
    alpn2 = cf_hc_get_پہلا_alpn(cf, ڈیٹا,
                                 ڈیٹا->state.http_neg.wanted,
                                 alpn1);
  }

  اگر(alpn2 != ALPN_none) {
    cf_hc_baller_تفویض(&ctx->ballers[1], alpn2, ctx->def_transport);
    ctx->baller_گنتی = 2;
    CURL_TRC_CF(ڈیٹا, cf, "2nd attempt uses %s from %s",
                ctx->ballers[1].name, ماخذ);
  }
  ctx->ballers_complete = TRUE;
}

ساکن CURLcode cf_hc_connect(ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *ڈیٹا,
                              bool *مکمل)
{
  ساخت cf_hc_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;
  ثابت ساخت curltime *pnow = تہی;

  اگر(cf->connected) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  *مکمل = FALSE;

  اگر(!ctx->httpsrr_resolved) {
    ctx->httpsrr_resolved = Curl_conn_dns_resolved_https(
      ڈیٹا, cf->sockindex, ctx->destination);
#اگر_تعریف_شدہ DEBUGBUILD
    اگر(!ctx->httpsrr_resolved && getenv("CURL_DBG_AWAIT_HTTPSRR")) {
      CURL_TRC_CF(ڈیٹا, cf, "awaiting HTTPS-RR");
      واپس CURLE_OK;
    }
#اگر_ختم
  }

  انتخاب(ctx->state) {
  حالت CF_HC_RESOLV:
    ctx->state = CF_HC_INIT;
    FALLTHROUGH();

  حالت CF_HC_INIT:
    DEBUGASSERT(!cf->اگلا);
    CURL_TRC_CF(ڈیٹا, cf, "connect, init");
    نتیجہ = cf_hc_سیٹ_baller1(cf, ڈیٹا);
    اگر(نتیجہ) {
      ctx->نتیجہ = نتیجہ;
      ctx->state = CF_HC_FAILURE;
      جاؤ برآمد;
    }
    cf_hc_سیٹ_baller2(cf, ڈیٹا);
    pnow = Curl_pgrs_now(ڈیٹا);
    ctx->started = *pnow;
    cf_hc_baller_init(&ctx->ballers[0], cf, ڈیٹا);
    اگر((ctx->baller_گنتی > 1) || !ctx->ballers_complete) {
      Curl_expire_سیٹ(ڈیٹا, EXPIRE_ALPN_EYEBALLS,
                      ctx->soft_eyeballs_timeout_ms, pnow);
    }
    ctx->state = CF_HC_CONNECT;
    FALLTHROUGH();

  حالت CF_HC_CONNECT:
    اگر(!ctx->ballers_complete)
      cf_hc_سیٹ_baller2(cf, ڈیٹا);

    اگر(cf_hc_baller_is_connecting(&ctx->ballers[0])) {
      نتیجہ = cf_hc_baller_connect(&ctx->ballers[0], cf, ڈیٹا, مکمل);
      اگر(!نتیجہ && *مکمل) {
        نتیجہ = baller_connected(cf, ڈیٹا, &ctx->ballers[0]);
        جاؤ برآمد;
      }
    }

    pnow = Curl_pgrs_now(ڈیٹا);
    اگر(وقت_مقصد_آغاز_baller2(cf, ڈیٹا, pnow)) {
      cf_hc_baller_init(&ctx->ballers[1], cf, ڈیٹا);
    }

    اگر(cf_hc_baller_is_connecting(&ctx->ballers[1])) {
      نتیجہ = cf_hc_baller_connect(&ctx->ballers[1], cf, ڈیٹا, مکمل);
      اگر(!نتیجہ && *مکمل) {
        نتیجہ = baller_connected(cf, ڈیٹا, &ctx->ballers[1]);
        جاؤ برآمد;
      }
    }

    اگر(ctx->ballers[0].نتیجہ &&
       (ctx->ballers[1].نتیجہ ||
        (ctx->ballers_complete && (ctx->baller_گنتی < 2)))) {
      /* all have failed. we give up */
      CURL_TRC_CF(ڈیٹا, cf, "connect, all attempts failed");
      ctx->نتیجہ = نتیجہ = ctx->ballers[0].نتیجہ;
      ctx->state = CF_HC_FAILURE;
      جاؤ برآمد;
    }
    نتیجہ = CURLE_OK;
    *مکمل = FALSE;
    توڑو;

  حالت CF_HC_FAILURE:
    نتیجہ = ctx->نتیجہ;
    cf->connected = FALSE;
    *مکمل = FALSE;
    توڑو;

  حالت CF_HC_SUCCESS:
    نتیجہ = CURLE_OK;
    cf->connected = TRUE;
    *مکمل = TRUE;
    توڑو;
  }

برآمد:
  CURL_TRC_CF(ڈیٹا, cf, "connect -> %d, done=%d", (صحیح)نتیجہ, *مکمل);
  واپس نتیجہ;
}

ساکن CURLcode cf_hc_shutdown(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  ساخت cf_hc_ctx *ctx = cf->ctx;
  size_t i;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(ڈیٹا);
  اگر(cf->connected) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  برائے(i = 0; i < ctx->baller_گنتی; i++) {
    ساخت cf_hc_baller *b = &ctx->ballers[i];
    bool bdone = FALSE;
    اگر(!cf_hc_baller_is_connecting(b) || b->shutdown)
      جاری;
    b->نتیجہ = b->cf->cft->do_shutdown(b->cf, ڈیٹا, &bdone);
    اگر(b->نتیجہ || bdone)
      b->shutdown = TRUE; /* treat a failed shutdown as done */
  }

  *مکمل = TRUE;
  برائے(i = 0; i < ctx->baller_گنتی; i++) {
    اگر(!ctx->ballers[i].shutdown)
      *مکمل = FALSE;
  }
  اگر(*مکمل) {
    برائے(i = 0; i < ctx->baller_گنتی; i++) {
      اگر(ctx->ballers[i].نتیجہ)
        نتیجہ = ctx->ballers[i].نتیجہ;
    }
  }
  CURL_TRC_CF(ڈیٹا, cf, "shutdown -> %d, done=%d", (صحیح)نتیجہ, *مکمل);
  واپس نتیجہ;
}

ساکن CURLcode cf_hc_adjust_pollset(ساخت Curl_cfilter *cf,
                                     ساخت Curl_easy *ڈیٹا,
                                     ساخت easy_pollset *ps)
{
  CURLcode نتیجہ = CURLE_OK;
  اگر(!cf->connected) {
    ساخت cf_hc_ctx *ctx = cf->ctx;
    size_t i;

    برائے(i = 0; (i < ctx->baller_گنتی) && !نتیجہ; i++) {
      ساخت cf_hc_baller *b = &ctx->ballers[i];
      اگر(!cf_hc_baller_is_connecting(b))
        جاری;
      نتیجہ = Curl_conn_cf_adjust_pollset(b->cf, ڈیٹا, ps);
    }
    CURL_TRC_CF(ڈیٹا, cf, "adjust_pollset -> %d, %u socks", (صحیح)نتیجہ,
                ps->n);
  }
  واپس نتیجہ;
}

ساکن bool cf_hc_ڈیٹا_pending(ساخت Curl_cfilter *cf,
                               ثابت ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  اگر(cf->connected)
    واپس cf->اگلا->cft->has_ڈیٹا_pending(cf->اگلا, ڈیٹا);

  برائے(i = 0; i < ctx->baller_گنتی; i++)
    اگر(cf_hc_baller_ڈیٹا_pending(&ctx->ballers[i], ڈیٹا))
      واپس TRUE;
  واپس FALSE;
}

ساکن CURLcode cf_hc_query(ساخت Curl_cfilter *cf,
                            ساخت Curl_easy *ڈیٹا,
                            صحیح query, صحیح *pres1, خالی *pres2)
{
  ساخت cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  اگر(!cf->connected) {
    انتخاب(query) {
    حالت CF_QUERY_NEED_FLUSH: {
      برائے(i = 0; i < ctx->baller_گنتی; i++)
        اگر(cf_hc_baller_needs_flush(&ctx->ballers[i], ڈیٹا)) {
          *pres1 = TRUE;
          واپس CURLE_OK;
        }
      توڑو;
    }
    طےشدہ:
      توڑو;
    }
  }
  واپس cf->اگلا ?
    cf->اگلا->cft->query(cf->اگلا, ڈیٹا, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساکن CURLcode cf_hc_cntrl(ساخت Curl_cfilter *cf,
                            ساخت Curl_easy *ڈیٹا,
                            صحیح event, صحیح arg1, خالی *arg2)
{
  ساخت cf_hc_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;
  size_t i;

  اگر(!cf->connected) {
    انتخاب(event) {
    حالت CF_CTRL_REPORT_STATS:
    حالت CF_CTRL_CONN_INFO_UPDATE:
      برائے(i = 0; i < ctx->baller_گنتی; i++) {
        /* Make the first baller that connected at network level report */
        اگر(Curl_conn_cf_is_ip_connected(ctx->ballers[i].cf, ڈیٹا)) {
          Curl_conn_cf_cntrl(ctx->ballers[i].cf, ڈیٹا, TRUE,
                             event, arg1, arg2);
          توڑو;
        }
      }
      توڑو;
    طےشدہ:
      برائے(i = 0; i < ctx->baller_گنتی; i++) {
        نتیجہ = cf_hc_baller_cntrl(&ctx->ballers[i], ڈیٹا, event, arg1, arg2);
        اگر(نتیجہ && (نتیجہ != CURLE_AGAIN))
          جاؤ برآمد;
      }
      نتیجہ = CURLE_OK;
      توڑو;
    }
  }
برآمد:
  واپس نتیجہ;
}

ساکن خالی cf_hc_destroy(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_hc_ctx *ctx = cf->ctx;

  CURL_TRC_CF(ڈیٹا, cf, "destroy");
  cf_hc_ctx_destroy(ڈیٹا, ctx);
}

ساخت Curl_cftype Curl_cft_http_connect = {
  "HTTPS-CONNECT",
  CF_TYPE_تیاری,
  CURL_LOG_LVL_NONE,
  cf_hc_destroy,
  cf_hc_connect,
  cf_hc_shutdown,
  cf_hc_adjust_pollset,
  cf_hc_ڈیٹا_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  cf_hc_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_hc_query,
};

ساکن CURLcode cf_hc_create(ساخت Curl_cfilter **pcf,
                             ساخت Curl_easy *ڈیٹا,
                             ساخت Curl_peer *destination,
                             uint8_t def_transport)
{
  ساخت Curl_cfilter *cf = تہی;
  ساخت cf_hc_ctx *ctx;
  CURLcode نتیجہ = CURLE_OK;

  ctx = curlx_calloc(1, حجم(*ctx));
  اگر(!ctx) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }
  Curl_peer_link(&ctx->destination, destination);
  ctx->def_transport = def_transport;
  ctx->hard_eyeballs_timeout_ms = ڈیٹا->سیٹ.happy_eyeballs_timeout;
  ctx->soft_eyeballs_timeout_ms = ڈیٹا->سیٹ.happy_eyeballs_timeout / 2;

  نتیجہ = Curl_cf_create(&cf, &Curl_cft_http_connect, ctx);
  اگر(نتیجہ)
    جاؤ برآمد;
  ctx = تہی;

برآمد:
  *pcf = نتیجہ ? تہی : cf;
  cf_hc_ctx_destroy(ڈیٹا, ctx);
  واپس نتیجہ;
}

ساکن CURLcode cf_hc_جوڑو(ساخت Curl_easy *ڈیٹا,
                          ساخت Curl_peer *destination,
                          ساخت connectdata *conn,
                          int8_t sockindex,
                          uint8_t def_transport)
{
  ساخت Curl_cfilter *cf;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(ڈیٹا);
  نتیجہ = cf_hc_create(&cf, ڈیٹا, destination, def_transport);
  اگر(نتیجہ)
    جاؤ برآمد;
  Curl_conn_cf_جوڑو(ڈیٹا, conn, sockindex, cf);

#اگر_تعریف_شدہ USE_HTTPSRR
  نتیجہ = Curl_conn_dns_جوڑو_https_resolve(ڈیٹا, cf->conn, cf->sockindex,
                                           destination);
#اگر_ختم
برآمد:
  واپس نتیجہ;
}

CURLcode Curl_cf_https_تیاری(ساخت Curl_easy *ڈیٹا,
                             ساخت Curl_peer *destination,
                             ساخت connectdata *conn,
                             int8_t sockindex)
{
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(conn->scheme->protocol == CURLPROTO_HTTPS);

  /* This filter is intended for HTTPS using ALPN and does
   * not support HTTPS Eyeballing to a proxy. */
  اگر((conn->scheme->protocol != CURLPROTO_HTTPS) ||
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
     conn->بٹ.origin_is_proxy ||
#اگر_ختم
     !conn->بٹ.tls_enable_alpn)
    جاؤ برآمد;

  نتیجہ = cf_hc_جوڑو(ڈیٹا, destination, conn, sockindex,
                     conn->transport_wanted);

برآمد:
  واپس نتیجہ;
}

#اگر_ختم /* !CURL_DISABLE_HTTP */
