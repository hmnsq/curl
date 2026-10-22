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
#ضمن "curl_setup.h"

#که_نه_تعریف_شوی CURL_DISABLE_HTTP

#ضمن "urldata.h"
#ضمن "curl_trc.h"
#ضمن "cfilters.h"
#ضمن "cf-setup.h"
#ضمن "connect.h"
#ضمن "multiif.h"
#ضمن "cf-https-connect.h"
#ضمن "http2.h"
#ضمن "progress.h"
#ضمن "select.h"
#ضمن "vdns/cf-dns.h"
#ضمن "vdns/hostip.h"
#ضمن "vdns/httpsrr.h"
#ضمن "vquic/vquic.h"

تعريفنوع تعداد {
  CF_HC_RESOLV,
  CF_HC_INIT,
  CF_HC_CONNECT,
  CF_HC_SUCCESS,
  CF_HC_FAILURE
} cf_hc_state;

بنية cf_hc_baller {
  ثابت حرف *name;
  بنية Curl_cfilter *cf;
  CURLcode پایله;
  صحيح ځواب_ms;
  uint8_t transport;
  تعداد alpnid alpn_id;
  بټ(shutdown);
};

ساكن فراغ cf_hc_baller_discard(بنية cf_hc_baller *b,
                                 بنية Curl_easy *معلومات)
{
  إذا(b->cf) {
    Curl_conn_cf_discard_chain(&b->cf, معلومات);
    b->cf = عدم;
  }
}

ساكن bool cf_hc_baller_is_connecting(بنية cf_hc_baller *b)
{
  أرجع b->cf && !b->پایله;
}

ساكن bool cf_hc_baller_has_started(بنية cf_hc_baller *b)
{
  أرجع !!b->cf;
}

ساكن صحيح cf_hc_baller_ځواب_ms(بنية cf_hc_baller *b,
                                 بنية Curl_easy *معلومات)
{
  إذا(b->cf && (b->ځواب_ms < 0))
    b->cf->cft->query(b->cf, معلومات, CF_QUERY_CONNECT_ځواب_MS,
                      &b->ځواب_ms, عدم);
  أرجع b->ځواب_ms;
}

ساكن bool cf_hc_baller_معلومات_pending(بنية cf_hc_baller *b,
                                      ثابت بنية Curl_easy *معلومات)
{
  أرجع b->cf && !b->پایله && b->cf->cft->has_معلومات_pending(b->cf, معلومات);
}

ساكن bool cf_hc_baller_needs_flush(بنية cf_hc_baller *b,
                                     بنية Curl_easy *معلومات)
{
  أرجع b->cf && !b->پایله && Curl_conn_cf_needs_flush(b->cf, معلومات);
}

ساكن CURLcode cf_hc_baller_cntrl(بنية cf_hc_baller *b,
                                   بنية Curl_easy *معلومات,
                                   صحيح event, صحيح arg1, فراغ *arg2)
{
  إذا(b->cf && !b->پایله)
    أرجع Curl_conn_cf_cntrl(b->cf, معلومات, FALSE, event, arg1, arg2);
  أرجع CURLE_OK;
}

بنية cf_hc_ctx {
  cf_hc_state state;
  بنية Curl_peer *destination; /* who we ultimately want to talk to */
  بنية curltime started;  /* when connect started */
  CURLcode پایله;          /* overall result */
  CURLcode check_h3_پایله;
  بنية cf_hc_baller ballers[2];
  size_t baller_شمېر;
  timediff_t soft_eyeballs_timeout_ms;
  timediff_t hard_eyeballs_timeout_ms;
  uint8_t def_transport;
  بټ(httpsrr_resolved);
  بټ(checked_h3);
  بټ(ballers_complete);
};

ساكن فراغ cf_hc_ctx_destroy(بنية Curl_easy *معلومات,
                              بنية cf_hc_ctx *ctx)
{
  إذا(ctx) {
    size_t i;
    لكل(i = 0; i < ctx->baller_شمېر; ++i)
      cf_hc_baller_discard(&ctx->ballers[i], معلومات);
    Curl_peer_unlink(&ctx->destination);
    curlx_free(ctx);
  }
}

ساكن فراغ cf_hc_baller_ټاکل(بنية cf_hc_baller *b,
                                تعداد alpnid alpn_id,
                                uint8_t def_transport)
{
  b->alpn_id = alpn_id;
  b->transport = def_transport;
  b->cf = عدم;
  b->پایله = CURLE_OK;
  b->ځواب_ms = -1;
  b->shutdown = FALSE;
  بدل(b->alpn_id) {
  حالة ALPN_h3:
    b->name = "h3";
    b->transport = TRNSPRT_QUIC;
    اكسر;
  حالة ALPN_h2:
    b->name = "h2";
    اكسر;
  حالة ALPN_h1:
    b->name = "h1";
    اكسر;
  حالة ALPN_none:
    b->name = "no-alpn";
    اكسر;
  افتراضي:
    b->پایله = CURLE_FAILED_INIT;
    اكسر;
  }
}

ساكن فراغ cf_hc_baller_init(بنية cf_hc_baller *b,
                              بنية Curl_cfilter *cf,
                              بنية Curl_easy *معلومات)
{
  بنية Curl_cfilter *save = cf->بل;

  cf->بل = عدم;
  b->پایله = Curl_cf_تیاری_ورننباسه_after(cf, معلومات, b->transport,
                                         CURL_CF_SSL_ENABLE);
  b->cf = cf->بل;
  cf->بل = save;
}

ساكن CURLcode cf_hc_baller_connect(بنية cf_hc_baller *b,
                                     بنية Curl_cfilter *cf,
                                     بنية Curl_easy *معلومات,
                                     bool *بشپړ)
{
  بنية Curl_cfilter *save = cf->بل;

  cf->بل = b->cf;
  b->پایله = Curl_conn_cf_connect(cf->بل, معلومات, بشپړ);
  b->cf = cf->بل; /* it might mutate */
  cf->بل = save;
  أرجع b->پایله;
}

ساكن CURLcode baller_connected(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 بنية cf_hc_baller *winner)
{
  بنية cf_hc_ctx *ctx = cf->ctx;

  /* Make the winner's connection filter out own sub-filter, check, move,
   * close all remaining. */
  إذا(cf->بل) {
    DEBUGASSERT(0);
    أرجع CURLE_FAILED_INIT;
  }
  إذا(!winner->cf) {
    DEBUGASSERT(0);
    أرجع CURLE_FAILED_INIT;
  }

  cf->بل = winner->cf;
  winner->cf = عدم;
  ctx->state = CF_HC_SUCCESS;
  cf->connected = TRUE;

  /* ballers may have failf()'d, the winner resets it, so our
   * errorbuf is clean again. */
  Curl_reset_fail(معلومات);

#که_تعریف_شوی USE_NGHTTP2
  {
    /* For a negotiated HTTP/2 connection insert the h2 filter. */
    ثابت حرف *alpn = Curl_conn_cf_get_alpn_negotiated(cf->بل, معلومات);
    إذا(alpn && !strcmp("h2", alpn)) {
      CURLcode پایله = Curl_http2_switch_at(cf, معلومات);
      إذا(پایله) {
        ctx->state = CF_HC_FAILURE;
        ctx->پایله = پایله;
        أرجع پایله;
      }
    }
  }
#د_که_پای
  أرجع CURLE_OK;
}

ساكن bool وخت_منزل_پیل_baller2(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  ثابت بنية curltime *pnow)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  timediff_t elapsed_ms;

  إذا(ctx->baller_شمېر < 2)
    أرجع FALSE;
  وإلا إذا(cf_hc_baller_has_started(&ctx->ballers[1]))
    أرجع FALSE;
  وإلا إذا(ctx->ballers[0].پایله) {
    CURL_TRC_CF(معلومات, cf, "%s baller failed, starting %s",
                ctx->ballers[0].name, ctx->ballers[1].name);
    أرجع TRUE;
  }

  elapsed_ms = curlx_ptimediff_ms(pnow, &ctx->started);
  إذا(elapsed_ms >= ctx->hard_eyeballs_timeout_ms) {
    CURL_TRC_CF(معلومات, cf, "%s inconclusive after %" FMT_TIMEDIFF_T ", "
                "starting %s", ctx->ballers[0].name,
                ctx->hard_eyeballs_timeout_ms, ctx->ballers[1].name);
    أرجع TRUE;
  }
  وإلا إذا(elapsed_ms >= ctx->soft_eyeballs_timeout_ms) {
    إذا(cf_hc_baller_ځواب_ms(&ctx->ballers[0], معلومات) < 0) {
      CURL_TRC_CF(معلومات, cf, "%s has not seen any data after %"
                  FMT_TIMEDIFF_T "ms, starting %s",
                  ctx->ballers[0].name, ctx->soft_eyeballs_timeout_ms,
                  ctx->ballers[1].name);
      أرجع TRUE;
    }
    /* set the effective hard timeout again */
    Curl_expire_سېټ(معلومات, EXPIRE_ALPN_EYEBALLS,
                    ctx->hard_eyeballs_timeout_ms - elapsed_ms, pnow);
  }
  أرجع FALSE;
}

ساكن bool cf_hc_may_h3(بنية Curl_cfilter *cf,
                         بنية Curl_easy *معلومات)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  إذا(!ctx->checked_h3) {
    ctx->check_h3_پایله =
      Curl_conn_may_http3(معلومات, cf->conn);
    ctx->checked_h3 = TRUE;
  }
  أرجع !ctx->check_h3_پایله;
}

ساكن تعداد alpnid cf_hc_get_httpsrr_alpn(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *معلومات,
                                          تعداد alpnid not_this_one)
{
#که_تعریف_شوی USE_HTTPSRR
  بنية cf_hc_ctx *ctx = cf->ctx;
  /* Is there an HTTPSRR use its ALPNs here.
   * We are here after having selected a connection to a host+port and
   * can no longer change that. Any HTTPSRR advice for other hosts and ports
   * we need to ignore. */
  ثابت بنية Curl_https_rrinfo *rr;
  size_t i;

  /* Do we have HTTPS-RR information? */
  rr = Curl_conn_dns_get_https(معلومات, cf->sockindex, ctx->destination);

  CURL_TRC_CF(معلومات, cf, "HTTPS-RR %savailable", rr ? "" : "not ");
  /* We do not support `rr->no_def_alpn`. */
  إذا(Curl_httpsrr_applicable(معلومات, rr) && !rr->no_def_alpn) {
    لكل(i = 0; i < CURL_ARRAYSIZE(rr->alpns); ++i) {
      تعداد alpnid alpn_rr = (تعداد alpnid)rr->alpns[i];
      إذا(alpn_rr == not_this_one) /* do not want this one */
        استمر;
      بدل(alpn_rr) {
      حالة ALPN_h3:
        إذا((معلومات->state.http_neg.allowed & CURL_HTTP_V3x) &&
           cf_hc_may_h3(cf, معلومات)) {
          أرجع alpn_rr;
        }
        اكسر;
      حالة ALPN_h2:
        إذا(معلومات->state.http_neg.allowed & CURL_HTTP_V2x) {
          أرجع alpn_rr;
        }
        اكسر;
      حالة ALPN_h1:
        إذا(معلومات->state.http_neg.allowed & CURL_HTTP_V1x) {
          أرجع alpn_rr;
        }
        اكسر;
      افتراضي: /* ignore */
        اكسر;
      }
    }
  }
#وإلا
  (فراغ)cf;
  (فراغ)معلومات;
  (فراغ)not_this_one;
#د_که_پای
  أرجع ALPN_none;
}

ساكن تعداد alpnid cf_hc_get_pref_alpn(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *معلومات,
                                       تعداد alpnid not_this_one)
{
  إذا((معلومات->state.http_neg.preferred & معلومات->state.http_neg.allowed)) {
    بدل(معلومات->state.http_neg.preferred) {
    حالة CURL_HTTP_V3x:
      إذا(cf_hc_may_h3(cf, معلومات) && (ALPN_h3 != not_this_one))
        أرجع ALPN_h3;
      اكسر;
    حالة CURL_HTTP_V2x:
      إذا(ALPN_h2 != not_this_one)
        أرجع ALPN_h2;
      اكسر;
    حالة CURL_HTTP_V1x:
      /* If we are trying h2 already, h1 is already used as fallback */
      إذا((ALPN_h1 != not_this_one) && (ALPN_h2 != not_this_one))
        أرجع ALPN_h1;
      اكسر;
    افتراضي:
      اكسر;
    }
  }
  أرجع ALPN_none;
}

ساكن تعداد alpnid cf_hc_get_لومړی_alpn(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *معلومات,
                                        http_majors choices,
                                        تعداد alpnid not_this_one)
{
  /* When told to not try h2, we also do not try h1 and vice versa */
  bool allow_h1_or_h2 = (not_this_one != ALPN_h1) &&
                        (not_this_one != ALPN_h2);
  إذا((ALPN_h3 != not_this_one) && (choices & CURL_HTTP_V3x) &&
     cf_hc_may_h3(cf, معلومات)) {
    أرجع ALPN_h3;
  }
  إذا(allow_h1_or_h2 && (choices & CURL_HTTP_V2x)) {
    أرجع ALPN_h2;
  }
  إذا(allow_h1_or_h2 && (choices & CURL_HTTP_V1x)) {
    أرجع ALPN_h1;
  }
  أرجع ALPN_none;
}

ساكن CURLcode cf_hc_سېټ_baller1(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  تعداد alpnid alpn1 = ALPN_none;
  VERBOSE(ثابت حرف *سرچینه = "HTTPS-RR");

  DEBUGASSERT(cf->conn->بټونه.tls_enable_alpn);

  alpn1 = cf_hc_get_httpsrr_alpn(cf, معلومات, ALPN_none);
  إذا(alpn1 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(سرچینه = "preferred version");
    alpn1 = cf_hc_get_pref_alpn(cf, معلومات, ALPN_none);
  }
  إذا(alpn1 == ALPN_none) {
    VERBOSE(سرچینه = "wanted versions");
    alpn1 = cf_hc_get_لومړی_alpn(cf, معلومات,
                                 معلومات->state.http_neg.wanted,
                                 ALPN_none);
  }
  إذا(alpn1 == ALPN_none) {
    VERBOSE(سرچینه = "allowed versions");
    alpn1 = cf_hc_get_لومړی_alpn(cf, معلومات,
                                 معلومات->state.http_neg.allowed,
                                 ALPN_none);
  }

  إذا(alpn1 == ALPN_none) {
    /* None of the wanted/allowed HTTP versions could be chosen */
    إذا(ctx->check_h3_پایله) {
      CURL_TRC_CF(معلومات, cf, "unable to use HTTP/3");
      أرجع ctx->check_h3_پایله;
    }
    CURL_TRC_CF(معلومات, cf, "unable to select HTTP version");
    أرجع CURLE_FAILED_INIT;
  }

  cf_hc_baller_ټاکل(&ctx->ballers[0], alpn1, ctx->def_transport);
  ctx->baller_شمېر = 1;
  CURL_TRC_CF(معلومات, cf, "1st attempt uses %s from %s",
              ctx->ballers[0].name, سرچینه);

  بدل(alpn1) {
  حالة ALPN_h1:
    /* We really want h1, switch off h2 to make it disappear in ALPN */
    معلومات->state.http_neg.wanted &= (uint8_t)~CURL_HTTP_V2x;
    اكسر;
  افتراضي:
    اكسر;
  }

  أرجع CURLE_OK;
}

ساكن فراغ cf_hc_سېټ_baller2(بنية Curl_cfilter *cf,
                              بنية Curl_easy *معلومات)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  تعداد alpnid alpn2 = ALPN_none, alpn1 = ctx->ballers[0].alpn_id;
  VERBOSE(ثابت حرف *سرچینه = "HTTPS-RR");

  إذا(ctx->ballers_complete)
    أرجع; /* already done */
  إذا(!ctx->httpsrr_resolved)
    أرجع; /* HTTPS-RR pending */

  alpn2 = cf_hc_get_httpsrr_alpn(cf, معلومات, alpn1);
  إذا(alpn2 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(سرچینه = "preferred version");
    alpn2 = cf_hc_get_pref_alpn(cf, معلومات, alpn1);
  }
  إذا(alpn2 == ALPN_none) {
    VERBOSE(سرچینه = "wanted versions");
    alpn2 = cf_hc_get_لومړی_alpn(cf, معلومات,
                                 معلومات->state.http_neg.wanted,
                                 alpn1);
  }

  إذا(alpn2 != ALPN_none) {
    cf_hc_baller_ټاکل(&ctx->ballers[1], alpn2, ctx->def_transport);
    ctx->baller_شمېر = 2;
    CURL_TRC_CF(معلومات, cf, "2nd attempt uses %s from %s",
                ctx->ballers[1].name, سرچینه);
  }
  ctx->ballers_complete = TRUE;
}

ساكن CURLcode cf_hc_connect(بنية Curl_cfilter *cf,
                              بنية Curl_easy *معلومات,
                              bool *بشپړ)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  CURLcode پایله = CURLE_OK;
  ثابت بنية curltime *pnow = عدم;

  إذا(cf->connected) {
    *بشپړ = TRUE;
    أرجع CURLE_OK;
  }

  *بشپړ = FALSE;

  إذا(!ctx->httpsrr_resolved) {
    ctx->httpsrr_resolved = Curl_conn_dns_resolved_https(
      معلومات, cf->sockindex, ctx->destination);
#که_تعریف_شوی DEBUGBUILD
    إذا(!ctx->httpsrr_resolved && getenv("CURL_DBG_AWAIT_HTTPSRR")) {
      CURL_TRC_CF(معلومات, cf, "awaiting HTTPS-RR");
      أرجع CURLE_OK;
    }
#د_که_پای
  }

  بدل(ctx->state) {
  حالة CF_HC_RESOLV:
    ctx->state = CF_HC_INIT;
    FALLTHROUGH();

  حالة CF_HC_INIT:
    DEBUGASSERT(!cf->بل);
    CURL_TRC_CF(معلومات, cf, "connect, init");
    پایله = cf_hc_سېټ_baller1(cf, معلومات);
    إذا(پایله) {
      ctx->پایله = پایله;
      ctx->state = CF_HC_FAILURE;
      اذهب بهر;
    }
    cf_hc_سېټ_baller2(cf, معلومات);
    pnow = Curl_pgrs_now(معلومات);
    ctx->started = *pnow;
    cf_hc_baller_init(&ctx->ballers[0], cf, معلومات);
    إذا((ctx->baller_شمېر > 1) || !ctx->ballers_complete) {
      Curl_expire_سېټ(معلومات, EXPIRE_ALPN_EYEBALLS,
                      ctx->soft_eyeballs_timeout_ms, pnow);
    }
    ctx->state = CF_HC_CONNECT;
    FALLTHROUGH();

  حالة CF_HC_CONNECT:
    إذا(!ctx->ballers_complete)
      cf_hc_سېټ_baller2(cf, معلومات);

    إذا(cf_hc_baller_is_connecting(&ctx->ballers[0])) {
      پایله = cf_hc_baller_connect(&ctx->ballers[0], cf, معلومات, بشپړ);
      إذا(!پایله && *بشپړ) {
        پایله = baller_connected(cf, معلومات, &ctx->ballers[0]);
        اذهب بهر;
      }
    }

    pnow = Curl_pgrs_now(معلومات);
    إذا(وخت_منزل_پیل_baller2(cf, معلومات, pnow)) {
      cf_hc_baller_init(&ctx->ballers[1], cf, معلومات);
    }

    إذا(cf_hc_baller_is_connecting(&ctx->ballers[1])) {
      پایله = cf_hc_baller_connect(&ctx->ballers[1], cf, معلومات, بشپړ);
      إذا(!پایله && *بشپړ) {
        پایله = baller_connected(cf, معلومات, &ctx->ballers[1]);
        اذهب بهر;
      }
    }

    إذا(ctx->ballers[0].پایله &&
       (ctx->ballers[1].پایله ||
        (ctx->ballers_complete && (ctx->baller_شمېر < 2)))) {
      /* all have failed. we give up */
      CURL_TRC_CF(معلومات, cf, "connect, all attempts failed");
      ctx->پایله = پایله = ctx->ballers[0].پایله;
      ctx->state = CF_HC_FAILURE;
      اذهب بهر;
    }
    پایله = CURLE_OK;
    *بشپړ = FALSE;
    اكسر;

  حالة CF_HC_FAILURE:
    پایله = ctx->پایله;
    cf->connected = FALSE;
    *بشپړ = FALSE;
    اكسر;

  حالة CF_HC_SUCCESS:
    پایله = CURLE_OK;
    cf->connected = TRUE;
    *بشپړ = TRUE;
    اكسر;
  }

بهر:
  CURL_TRC_CF(معلومات, cf, "connect -> %d, done=%d", (صحيح)پایله, *بشپړ);
  أرجع پایله;
}

ساكن CURLcode cf_hc_shutdown(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات, bool *بشپړ)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  size_t i;
  CURLcode پایله = CURLE_OK;

  DEBUGASSERT(معلومات);
  إذا(cf->connected) {
    *بشپړ = TRUE;
    أرجع CURLE_OK;
  }

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  لكل(i = 0; i < ctx->baller_شمېر; i++) {
    بنية cf_hc_baller *b = &ctx->ballers[i];
    bool bdone = FALSE;
    إذا(!cf_hc_baller_is_connecting(b) || b->shutdown)
      استمر;
    b->پایله = b->cf->cft->do_shutdown(b->cf, معلومات, &bdone);
    إذا(b->پایله || bdone)
      b->shutdown = TRUE; /* treat a failed shutdown as done */
  }

  *بشپړ = TRUE;
  لكل(i = 0; i < ctx->baller_شمېر; i++) {
    إذا(!ctx->ballers[i].shutdown)
      *بشپړ = FALSE;
  }
  إذا(*بشپړ) {
    لكل(i = 0; i < ctx->baller_شمېر; i++) {
      إذا(ctx->ballers[i].پایله)
        پایله = ctx->ballers[i].پایله;
    }
  }
  CURL_TRC_CF(معلومات, cf, "shutdown -> %d, done=%d", (صحيح)پایله, *بشپړ);
  أرجع پایله;
}

ساكن CURLcode cf_hc_adjust_pollset(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *معلومات,
                                     بنية easy_pollset *ps)
{
  CURLcode پایله = CURLE_OK;
  إذا(!cf->connected) {
    بنية cf_hc_ctx *ctx = cf->ctx;
    size_t i;

    لكل(i = 0; (i < ctx->baller_شمېر) && !پایله; i++) {
      بنية cf_hc_baller *b = &ctx->ballers[i];
      إذا(!cf_hc_baller_is_connecting(b))
        استمر;
      پایله = Curl_conn_cf_adjust_pollset(b->cf, معلومات, ps);
    }
    CURL_TRC_CF(معلومات, cf, "adjust_pollset -> %d, %u socks", (صحيح)پایله,
                ps->n);
  }
  أرجع پایله;
}

ساكن bool cf_hc_معلومات_pending(بنية Curl_cfilter *cf,
                               ثابت بنية Curl_easy *معلومات)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  إذا(cf->connected)
    أرجع cf->بل->cft->has_معلومات_pending(cf->بل, معلومات);

  لكل(i = 0; i < ctx->baller_شمېر; i++)
    إذا(cf_hc_baller_معلومات_pending(&ctx->ballers[i], معلومات))
      أرجع TRUE;
  أرجع FALSE;
}

ساكن CURLcode cf_hc_query(بنية Curl_cfilter *cf,
                            بنية Curl_easy *معلومات,
                            صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  إذا(!cf->connected) {
    بدل(query) {
    حالة CF_QUERY_NEED_FLUSH: {
      لكل(i = 0; i < ctx->baller_شمېر; i++)
        إذا(cf_hc_baller_needs_flush(&ctx->ballers[i], معلومات)) {
          *pres1 = TRUE;
          أرجع CURLE_OK;
        }
      اكسر;
    }
    افتراضي:
      اكسر;
    }
  }
  أرجع cf->بل ?
    cf->بل->cft->query(cf->بل, معلومات, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساكن CURLcode cf_hc_cntrl(بنية Curl_cfilter *cf,
                            بنية Curl_easy *معلومات,
                            صحيح event, صحيح arg1, فراغ *arg2)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  CURLcode پایله = CURLE_OK;
  size_t i;

  إذا(!cf->connected) {
    بدل(event) {
    حالة CF_CTRL_REPORT_STATS:
    حالة CF_CTRL_CONN_INFO_UPDATE:
      لكل(i = 0; i < ctx->baller_شمېر; i++) {
        /* Make the first baller that connected at network level report */
        إذا(Curl_conn_cf_is_ip_connected(ctx->ballers[i].cf, معلومات)) {
          Curl_conn_cf_cntrl(ctx->ballers[i].cf, معلومات, TRUE,
                             event, arg1, arg2);
          اكسر;
        }
      }
      اكسر;
    افتراضي:
      لكل(i = 0; i < ctx->baller_شمېر; i++) {
        پایله = cf_hc_baller_cntrl(&ctx->ballers[i], معلومات, event, arg1, arg2);
        إذا(پایله && (پایله != CURLE_AGAIN))
          اذهب بهر;
      }
      پایله = CURLE_OK;
      اكسر;
    }
  }
بهر:
  أرجع پایله;
}

ساكن فراغ cf_hc_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات)
{
  بنية cf_hc_ctx *ctx = cf->ctx;

  CURL_TRC_CF(معلومات, cf, "destroy");
  cf_hc_ctx_destroy(معلومات, ctx);
}

بنية Curl_cftype Curl_cft_http_connect = {
  "HTTPS-CONNECT",
  CF_TYPE_تیاری,
  CURL_LOG_LVL_NONE,
  cf_hc_destroy,
  cf_hc_connect,
  cf_hc_shutdown,
  cf_hc_adjust_pollset,
  cf_hc_معلومات_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  cf_hc_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_hc_query,
};

ساكن CURLcode cf_hc_create(بنية Curl_cfilter **pcf,
                             بنية Curl_easy *معلومات,
                             بنية Curl_peer *destination,
                             uint8_t def_transport)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_hc_ctx *ctx;
  CURLcode پایله = CURLE_OK;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }
  Curl_peer_link(&ctx->destination, destination);
  ctx->def_transport = def_transport;
  ctx->hard_eyeballs_timeout_ms = معلومات->سېټ.happy_eyeballs_timeout;
  ctx->soft_eyeballs_timeout_ms = معلومات->سېټ.happy_eyeballs_timeout / 2;

  پایله = Curl_cf_create(&cf, &Curl_cft_http_connect, ctx);
  إذا(پایله)
    اذهب بهر;
  ctx = عدم;

بهر:
  *pcf = پایله ? عدم : cf;
  cf_hc_ctx_destroy(معلومات, ctx);
  أرجع پایله;
}

ساكن CURLcode cf_hc_جمع_کړه(بنية Curl_easy *معلومات,
                          بنية Curl_peer *destination,
                          بنية connectdata *conn,
                          int8_t sockindex,
                          uint8_t def_transport)
{
  بنية Curl_cfilter *cf;
  CURLcode پایله = CURLE_OK;

  DEBUGASSERT(معلومات);
  پایله = cf_hc_create(&cf, معلومات, destination, def_transport);
  إذا(پایله)
    اذهب بهر;
  Curl_conn_cf_جمع_کړه(معلومات, conn, sockindex, cf);

#که_تعریف_شوی USE_HTTPSRR
  پایله = Curl_conn_dns_جمع_کړه_https_resolve(معلومات, cf->conn, cf->sockindex,
                                           destination);
#د_که_پای
بهر:
  أرجع پایله;
}

CURLcode Curl_cf_https_تیاری(بنية Curl_easy *معلومات,
                             بنية Curl_peer *destination,
                             بنية connectdata *conn,
                             int8_t sockindex)
{
  CURLcode پایله = CURLE_OK;

  DEBUGASSERT(conn->scheme->protocol == CURLPROTO_HTTPS);

  /* This filter is intended for HTTPS using ALPN and does
   * not support HTTPS Eyeballing to a proxy. */
  إذا((conn->scheme->protocol != CURLPROTO_HTTPS) ||
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
     conn->بټونه.origin_is_proxy ||
#د_که_پای
     !conn->بټونه.tls_enable_alpn)
    اذهب بهر;

  پایله = cf_hc_جمع_کړه(معلومات, destination, conn, sockindex,
                     conn->transport_wanted);

بهر:
  أرجع پایله;
}

#د_که_پای /* !CURL_DISABLE_HTTP */
