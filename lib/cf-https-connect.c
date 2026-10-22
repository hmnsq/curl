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

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTP

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
  CURLcode نتيجو;
  صحيح جواب_ms;
  uint8_t transport;
  تعداد alpnid alpn_id;
  ٻٽو_انگ(shutdown);
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
  أرجع b->cf && !b->نتيجو;
}

ساكن bool cf_hc_baller_has_started(بنية cf_hc_baller *b)
{
  أرجع !!b->cf;
}

ساكن صحيح cf_hc_baller_جواب_ms(بنية cf_hc_baller *b,
                                 بنية Curl_easy *معلومات)
{
  إذا(b->cf && (b->جواب_ms < 0))
    b->cf->cft->query(b->cf, معلومات, CF_QUERY_CONNECT_جواب_MS,
                      &b->جواب_ms, عدم);
  أرجع b->جواب_ms;
}

ساكن bool cf_hc_baller_معلومات_pending(بنية cf_hc_baller *b,
                                      ثابت بنية Curl_easy *معلومات)
{
  أرجع b->cf && !b->نتيجو && b->cf->cft->has_معلومات_pending(b->cf, معلومات);
}

ساكن bool cf_hc_baller_needs_flush(بنية cf_hc_baller *b,
                                     بنية Curl_easy *معلومات)
{
  أرجع b->cf && !b->نتيجو && Curl_conn_cf_needs_flush(b->cf, معلومات);
}

ساكن CURLcode cf_hc_baller_cntrl(بنية cf_hc_baller *b,
                                   بنية Curl_easy *معلومات,
                                   صحيح event, صحيح arg1, فراغ *arg2)
{
  إذا(b->cf && !b->نتيجو)
    أرجع Curl_conn_cf_cntrl(b->cf, معلومات, FALSE, event, arg1, arg2);
  أرجع CURLE_OK;
}

بنية cf_hc_ctx {
  cf_hc_state state;
  بنية Curl_peer *destination; /* who we ultimately want to talk to */
  بنية curltime started;  /* when connect started */
  CURLcode نتيجو;          /* overall result */
  CURLcode check_h3_نتيجو;
  بنية cf_hc_baller ballers[2];
  size_t baller_ڳڻپ;
  timediff_t soft_eyeballs_timeout_ms;
  timediff_t hard_eyeballs_timeout_ms;
  uint8_t def_transport;
  ٻٽو_انگ(httpsrr_resolved);
  ٻٽو_انگ(checked_h3);
  ٻٽو_انگ(ballers_complete);
};

ساكن فراغ cf_hc_ctx_destroy(بنية Curl_easy *معلومات,
                              بنية cf_hc_ctx *ctx)
{
  إذا(ctx) {
    size_t i;
    لكل(i = 0; i < ctx->baller_ڳڻپ; ++i)
      cf_hc_baller_discard(&ctx->ballers[i], معلومات);
    Curl_peer_unlink(&ctx->destination);
    curlx_free(ctx);
  }
}

ساكن فراغ cf_hc_baller_سونپيو(بنية cf_hc_baller *b,
                                تعداد alpnid alpn_id,
                                uint8_t def_transport)
{
  b->alpn_id = alpn_id;
  b->transport = def_transport;
  b->cf = عدم;
  b->نتيجو = CURLE_OK;
  b->جواب_ms = -1;
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
    b->نتيجو = CURLE_FAILED_INIT;
    اكسر;
  }
}

ساكن فراغ cf_hc_baller_init(بنية cf_hc_baller *b,
                              بنية Curl_cfilter *cf,
                              بنية Curl_easy *معلومات)
{
  بنية Curl_cfilter *save = cf->ايندڙ;

  cf->ايندڙ = عدم;
  b->نتيجو = Curl_cf_تياري_وجهو_after(cf, معلومات, b->transport,
                                         CURL_CF_SSL_ENABLE);
  b->cf = cf->ايندڙ;
  cf->ايندڙ = save;
}

ساكن CURLcode cf_hc_baller_connect(بنية cf_hc_baller *b,
                                     بنية Curl_cfilter *cf,
                                     بنية Curl_easy *معلومات,
                                     bool *پورو)
{
  بنية Curl_cfilter *save = cf->ايندڙ;

  cf->ايندڙ = b->cf;
  b->نتيجو = Curl_conn_cf_connect(cf->ايندڙ, معلومات, پورو);
  b->cf = cf->ايندڙ; /* it might mutate */
  cf->ايندڙ = save;
  أرجع b->نتيجو;
}

ساكن CURLcode baller_connected(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 بنية cf_hc_baller *winner)
{
  بنية cf_hc_ctx *ctx = cf->ctx;

  /* Make the winner's connection filter out own sub-filter, check, move,
   * close all remaining. */
  إذا(cf->ايندڙ) {
    DEBUGASSERT(0);
    أرجع CURLE_FAILED_INIT;
  }
  إذا(!winner->cf) {
    DEBUGASSERT(0);
    أرجع CURLE_FAILED_INIT;
  }

  cf->ايندڙ = winner->cf;
  winner->cf = عدم;
  ctx->state = CF_HC_SUCCESS;
  cf->connected = TRUE;

  /* ballers may have failf()'d, the winner resets it, so our
   * errorbuf is clean again. */
  Curl_reset_fail(معلومات);

#جيڪڏهن_وصف_ٿيل USE_NGHTTP2
  {
    /* For a negotiated HTTP/2 connection insert the h2 filter. */
    ثابت حرف *alpn = Curl_conn_cf_get_alpn_negotiated(cf->ايندڙ, معلومات);
    إذا(alpn && !strcmp("h2", alpn)) {
      CURLcode نتيجو = Curl_http2_switch_at(cf, معلومات);
      إذا(نتيجو) {
        ctx->state = CF_HC_FAILURE;
        ctx->نتيجو = نتيجو;
        أرجع نتيجو;
      }
    }
  }
#جيڪڏهن_پورو
  أرجع CURLE_OK;
}

ساكن bool وقت_منزل_شروعات_baller2(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  ثابت بنية curltime *pnow)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  timediff_t elapsed_ms;

  إذا(ctx->baller_ڳڻپ < 2)
    أرجع FALSE;
  وإلا إذا(cf_hc_baller_has_started(&ctx->ballers[1]))
    أرجع FALSE;
  وإلا إذا(ctx->ballers[0].نتيجو) {
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
    إذا(cf_hc_baller_جواب_ms(&ctx->ballers[0], معلومات) < 0) {
      CURL_TRC_CF(معلومات, cf, "%s has not seen any data after %"
                  FMT_TIMEDIFF_T "ms, starting %s",
                  ctx->ballers[0].name, ctx->soft_eyeballs_timeout_ms,
                  ctx->ballers[1].name);
      أرجع TRUE;
    }
    /* set the effective hard timeout again */
    Curl_expire_سيٽ(معلومات, EXPIRE_ALPN_EYEBALLS,
                    ctx->hard_eyeballs_timeout_ms - elapsed_ms, pnow);
  }
  أرجع FALSE;
}

ساكن bool cf_hc_may_h3(بنية Curl_cfilter *cf,
                         بنية Curl_easy *معلومات)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  إذا(!ctx->checked_h3) {
    ctx->check_h3_نتيجو =
      Curl_conn_may_http3(معلومات, cf->conn);
    ctx->checked_h3 = TRUE;
  }
  أرجع !ctx->check_h3_نتيجو;
}

ساكن تعداد alpnid cf_hc_get_httpsrr_alpn(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *معلومات,
                                          تعداد alpnid not_this_one)
{
#جيڪڏهن_وصف_ٿيل USE_HTTPSRR
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
#جيڪڏهن_پورو
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

ساكن تعداد alpnid cf_hc_get_پهريون_alpn(بنية Curl_cfilter *cf,
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

ساكن CURLcode cf_hc_سيٽ_baller1(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  تعداد alpnid alpn1 = ALPN_none;
  VERBOSE(ثابت حرف *ذريعو = "HTTPS-RR");

  DEBUGASSERT(cf->conn->بٽ.tls_enable_alpn);

  alpn1 = cf_hc_get_httpsrr_alpn(cf, معلومات, ALPN_none);
  إذا(alpn1 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(ذريعو = "preferred version");
    alpn1 = cf_hc_get_pref_alpn(cf, معلومات, ALPN_none);
  }
  إذا(alpn1 == ALPN_none) {
    VERBOSE(ذريعو = "wanted versions");
    alpn1 = cf_hc_get_پهريون_alpn(cf, معلومات,
                                 معلومات->state.http_neg.wanted,
                                 ALPN_none);
  }
  إذا(alpn1 == ALPN_none) {
    VERBOSE(ذريعو = "allowed versions");
    alpn1 = cf_hc_get_پهريون_alpn(cf, معلومات,
                                 معلومات->state.http_neg.allowed,
                                 ALPN_none);
  }

  إذا(alpn1 == ALPN_none) {
    /* None of the wanted/allowed HTTP versions could be chosen */
    إذا(ctx->check_h3_نتيجو) {
      CURL_TRC_CF(معلومات, cf, "unable to use HTTP/3");
      أرجع ctx->check_h3_نتيجو;
    }
    CURL_TRC_CF(معلومات, cf, "unable to select HTTP version");
    أرجع CURLE_FAILED_INIT;
  }

  cf_hc_baller_سونپيو(&ctx->ballers[0], alpn1, ctx->def_transport);
  ctx->baller_ڳڻپ = 1;
  CURL_TRC_CF(معلومات, cf, "1st attempt uses %s from %s",
              ctx->ballers[0].name, ذريعو);

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

ساكن فراغ cf_hc_سيٽ_baller2(بنية Curl_cfilter *cf,
                              بنية Curl_easy *معلومات)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  تعداد alpnid alpn2 = ALPN_none, alpn1 = ctx->ballers[0].alpn_id;
  VERBOSE(ثابت حرف *ذريعو = "HTTPS-RR");

  إذا(ctx->ballers_complete)
    أرجع; /* already done */
  إذا(!ctx->httpsrr_resolved)
    أرجع; /* HTTPS-RR pending */

  alpn2 = cf_hc_get_httpsrr_alpn(cf, معلومات, alpn1);
  إذا(alpn2 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(ذريعو = "preferred version");
    alpn2 = cf_hc_get_pref_alpn(cf, معلومات, alpn1);
  }
  إذا(alpn2 == ALPN_none) {
    VERBOSE(ذريعو = "wanted versions");
    alpn2 = cf_hc_get_پهريون_alpn(cf, معلومات,
                                 معلومات->state.http_neg.wanted,
                                 alpn1);
  }

  إذا(alpn2 != ALPN_none) {
    cf_hc_baller_سونپيو(&ctx->ballers[1], alpn2, ctx->def_transport);
    ctx->baller_ڳڻپ = 2;
    CURL_TRC_CF(معلومات, cf, "2nd attempt uses %s from %s",
                ctx->ballers[1].name, ذريعو);
  }
  ctx->ballers_complete = TRUE;
}

ساكن CURLcode cf_hc_connect(بنية Curl_cfilter *cf,
                              بنية Curl_easy *معلومات,
                              bool *پورو)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  CURLcode نتيجو = CURLE_OK;
  ثابت بنية curltime *pnow = عدم;

  إذا(cf->connected) {
    *پورو = TRUE;
    أرجع CURLE_OK;
  }

  *پورو = FALSE;

  إذا(!ctx->httpsrr_resolved) {
    ctx->httpsrr_resolved = Curl_conn_dns_resolved_https(
      معلومات, cf->sockindex, ctx->destination);
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
    إذا(!ctx->httpsrr_resolved && getenv("CURL_DBG_AWAIT_HTTPSRR")) {
      CURL_TRC_CF(معلومات, cf, "awaiting HTTPS-RR");
      أرجع CURLE_OK;
    }
#جيڪڏهن_پورو
  }

  بدل(ctx->state) {
  حالة CF_HC_RESOLV:
    ctx->state = CF_HC_INIT;
    FALLTHROUGH();

  حالة CF_HC_INIT:
    DEBUGASSERT(!cf->ايندڙ);
    CURL_TRC_CF(معلومات, cf, "connect, init");
    نتيجو = cf_hc_سيٽ_baller1(cf, معلومات);
    إذا(نتيجو) {
      ctx->نتيجو = نتيجو;
      ctx->state = CF_HC_FAILURE;
      اذهب ٻاهريون;
    }
    cf_hc_سيٽ_baller2(cf, معلومات);
    pnow = Curl_pgrs_now(معلومات);
    ctx->started = *pnow;
    cf_hc_baller_init(&ctx->ballers[0], cf, معلومات);
    إذا((ctx->baller_ڳڻپ > 1) || !ctx->ballers_complete) {
      Curl_expire_سيٽ(معلومات, EXPIRE_ALPN_EYEBALLS,
                      ctx->soft_eyeballs_timeout_ms, pnow);
    }
    ctx->state = CF_HC_CONNECT;
    FALLTHROUGH();

  حالة CF_HC_CONNECT:
    إذا(!ctx->ballers_complete)
      cf_hc_سيٽ_baller2(cf, معلومات);

    إذا(cf_hc_baller_is_connecting(&ctx->ballers[0])) {
      نتيجو = cf_hc_baller_connect(&ctx->ballers[0], cf, معلومات, پورو);
      إذا(!نتيجو && *پورو) {
        نتيجو = baller_connected(cf, معلومات, &ctx->ballers[0]);
        اذهب ٻاهريون;
      }
    }

    pnow = Curl_pgrs_now(معلومات);
    إذا(وقت_منزل_شروعات_baller2(cf, معلومات, pnow)) {
      cf_hc_baller_init(&ctx->ballers[1], cf, معلومات);
    }

    إذا(cf_hc_baller_is_connecting(&ctx->ballers[1])) {
      نتيجو = cf_hc_baller_connect(&ctx->ballers[1], cf, معلومات, پورو);
      إذا(!نتيجو && *پورو) {
        نتيجو = baller_connected(cf, معلومات, &ctx->ballers[1]);
        اذهب ٻاهريون;
      }
    }

    إذا(ctx->ballers[0].نتيجو &&
       (ctx->ballers[1].نتيجو ||
        (ctx->ballers_complete && (ctx->baller_ڳڻپ < 2)))) {
      /* all have failed. we give up */
      CURL_TRC_CF(معلومات, cf, "connect, all attempts failed");
      ctx->نتيجو = نتيجو = ctx->ballers[0].نتيجو;
      ctx->state = CF_HC_FAILURE;
      اذهب ٻاهريون;
    }
    نتيجو = CURLE_OK;
    *پورو = FALSE;
    اكسر;

  حالة CF_HC_FAILURE:
    نتيجو = ctx->نتيجو;
    cf->connected = FALSE;
    *پورو = FALSE;
    اكسر;

  حالة CF_HC_SUCCESS:
    نتيجو = CURLE_OK;
    cf->connected = TRUE;
    *پورو = TRUE;
    اكسر;
  }

ٻاهريون:
  CURL_TRC_CF(معلومات, cf, "connect -> %d, done=%d", (صحيح)نتيجو, *پورو);
  أرجع نتيجو;
}

ساكن CURLcode cf_hc_shutdown(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات, bool *پورو)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  size_t i;
  CURLcode نتيجو = CURLE_OK;

  DEBUGASSERT(معلومات);
  إذا(cf->connected) {
    *پورو = TRUE;
    أرجع CURLE_OK;
  }

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  لكل(i = 0; i < ctx->baller_ڳڻپ; i++) {
    بنية cf_hc_baller *b = &ctx->ballers[i];
    bool bdone = FALSE;
    إذا(!cf_hc_baller_is_connecting(b) || b->shutdown)
      استمر;
    b->نتيجو = b->cf->cft->do_shutdown(b->cf, معلومات, &bdone);
    إذا(b->نتيجو || bdone)
      b->shutdown = TRUE; /* treat a failed shutdown as done */
  }

  *پورو = TRUE;
  لكل(i = 0; i < ctx->baller_ڳڻپ; i++) {
    إذا(!ctx->ballers[i].shutdown)
      *پورو = FALSE;
  }
  إذا(*پورو) {
    لكل(i = 0; i < ctx->baller_ڳڻپ; i++) {
      إذا(ctx->ballers[i].نتيجو)
        نتيجو = ctx->ballers[i].نتيجو;
    }
  }
  CURL_TRC_CF(معلومات, cf, "shutdown -> %d, done=%d", (صحيح)نتيجو, *پورو);
  أرجع نتيجو;
}

ساكن CURLcode cf_hc_adjust_pollset(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *معلومات,
                                     بنية easy_pollset *ps)
{
  CURLcode نتيجو = CURLE_OK;
  إذا(!cf->connected) {
    بنية cf_hc_ctx *ctx = cf->ctx;
    size_t i;

    لكل(i = 0; (i < ctx->baller_ڳڻپ) && !نتيجو; i++) {
      بنية cf_hc_baller *b = &ctx->ballers[i];
      إذا(!cf_hc_baller_is_connecting(b))
        استمر;
      نتيجو = Curl_conn_cf_adjust_pollset(b->cf, معلومات, ps);
    }
    CURL_TRC_CF(معلومات, cf, "adjust_pollset -> %d, %u socks", (صحيح)نتيجو,
                ps->n);
  }
  أرجع نتيجو;
}

ساكن bool cf_hc_معلومات_pending(بنية Curl_cfilter *cf,
                               ثابت بنية Curl_easy *معلومات)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  إذا(cf->connected)
    أرجع cf->ايندڙ->cft->has_معلومات_pending(cf->ايندڙ, معلومات);

  لكل(i = 0; i < ctx->baller_ڳڻپ; i++)
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
      لكل(i = 0; i < ctx->baller_ڳڻپ; i++)
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
  أرجع cf->ايندڙ ?
    cf->ايندڙ->cft->query(cf->ايندڙ, معلومات, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساكن CURLcode cf_hc_cntrl(بنية Curl_cfilter *cf,
                            بنية Curl_easy *معلومات,
                            صحيح event, صحيح arg1, فراغ *arg2)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  CURLcode نتيجو = CURLE_OK;
  size_t i;

  إذا(!cf->connected) {
    بدل(event) {
    حالة CF_CTRL_REPORT_STATS:
    حالة CF_CTRL_CONN_INFO_UPDATE:
      لكل(i = 0; i < ctx->baller_ڳڻپ; i++) {
        /* Make the first baller that connected at network level report */
        إذا(Curl_conn_cf_is_ip_connected(ctx->ballers[i].cf, معلومات)) {
          Curl_conn_cf_cntrl(ctx->ballers[i].cf, معلومات, TRUE,
                             event, arg1, arg2);
          اكسر;
        }
      }
      اكسر;
    افتراضي:
      لكل(i = 0; i < ctx->baller_ڳڻپ; i++) {
        نتيجو = cf_hc_baller_cntrl(&ctx->ballers[i], معلومات, event, arg1, arg2);
        إذا(نتيجو && (نتيجو != CURLE_AGAIN))
          اذهب ٻاهريون;
      }
      نتيجو = CURLE_OK;
      اكسر;
    }
  }
ٻاهريون:
  أرجع نتيجو;
}

ساكن فراغ cf_hc_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات)
{
  بنية cf_hc_ctx *ctx = cf->ctx;

  CURL_TRC_CF(معلومات, cf, "destroy");
  cf_hc_ctx_destroy(معلومات, ctx);
}

بنية Curl_cftype Curl_cft_http_connect = {
  "HTTPS-CONNECT",
  CF_TYPE_تياري,
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
  CURLcode نتيجو = CURLE_OK;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب ٻاهريون;
  }
  Curl_peer_link(&ctx->destination, destination);
  ctx->def_transport = def_transport;
  ctx->hard_eyeballs_timeout_ms = معلومات->سيٽ.happy_eyeballs_timeout;
  ctx->soft_eyeballs_timeout_ms = معلومات->سيٽ.happy_eyeballs_timeout / 2;

  نتيجو = Curl_cf_create(&cf, &Curl_cft_http_connect, ctx);
  إذا(نتيجو)
    اذهب ٻاهريون;
  ctx = عدم;

ٻاهريون:
  *pcf = نتيجو ? عدم : cf;
  cf_hc_ctx_destroy(معلومات, ctx);
  أرجع نتيجو;
}

ساكن CURLcode cf_hc_جوڙيو(بنية Curl_easy *معلومات,
                          بنية Curl_peer *destination,
                          بنية connectdata *conn,
                          int8_t sockindex,
                          uint8_t def_transport)
{
  بنية Curl_cfilter *cf;
  CURLcode نتيجو = CURLE_OK;

  DEBUGASSERT(معلومات);
  نتيجو = cf_hc_create(&cf, معلومات, destination, def_transport);
  إذا(نتيجو)
    اذهب ٻاهريون;
  Curl_conn_cf_جوڙيو(معلومات, conn, sockindex, cf);

#جيڪڏهن_وصف_ٿيل USE_HTTPSRR
  نتيجو = Curl_conn_dns_جوڙيو_https_resolve(معلومات, cf->conn, cf->sockindex,
                                           destination);
#جيڪڏهن_پورو
ٻاهريون:
  أرجع نتيجو;
}

CURLcode Curl_cf_https_تياري(بنية Curl_easy *معلومات,
                             بنية Curl_peer *destination,
                             بنية connectdata *conn,
                             int8_t sockindex)
{
  CURLcode نتيجو = CURLE_OK;

  DEBUGASSERT(conn->scheme->protocol == CURLPROTO_HTTPS);

  /* This filter is intended for HTTPS using ALPN and does
   * not support HTTPS Eyeballing to a proxy. */
  إذا((conn->scheme->protocol != CURLPROTO_HTTPS) ||
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
     conn->بٽ.origin_is_proxy ||
#جيڪڏهن_پورو
     !conn->بٽ.tls_enable_alpn)
    اذهب ٻاهريون;

  نتيجو = cf_hc_جوڙيو(معلومات, destination, conn, sockindex,
                     conn->transport_wanted);

ٻاهريون:
  أرجع نتيجو;
}

#جيڪڏهن_پورو /* !CURL_DISABLE_HTTP */
