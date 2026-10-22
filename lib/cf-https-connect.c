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

#اگر_تعریف_نشده CURL_DISABLE_HTTP

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
  CURLcode نتیجه;
  صحيح پاسخ_ms;
  uint8_t transport;
  تعداد alpnid alpn_id;
  بیت(shutdown);
};

ساكن فراغ cf_hc_baller_discard(بنية cf_hc_baller *b,
                                 بنية Curl_easy *داده)
{
  إذا(b->cf) {
    Curl_conn_cf_discard_chain(&b->cf, داده);
    b->cf = عدم;
  }
}

ساكن bool cf_hc_baller_is_connecting(بنية cf_hc_baller *b)
{
  أرجع b->cf && !b->نتیجه;
}

ساكن bool cf_hc_baller_has_started(بنية cf_hc_baller *b)
{
  أرجع !!b->cf;
}

ساكن صحيح cf_hc_baller_پاسخ_ms(بنية cf_hc_baller *b,
                                 بنية Curl_easy *داده)
{
  إذا(b->cf && (b->پاسخ_ms < 0))
    b->cf->cft->query(b->cf, داده, CF_QUERY_CONNECT_پاسخ_MS,
                      &b->پاسخ_ms, عدم);
  أرجع b->پاسخ_ms;
}

ساكن bool cf_hc_baller_داده_pending(بنية cf_hc_baller *b,
                                      ثابت بنية Curl_easy *داده)
{
  أرجع b->cf && !b->نتیجه && b->cf->cft->has_داده_pending(b->cf, داده);
}

ساكن bool cf_hc_baller_needs_flush(بنية cf_hc_baller *b,
                                     بنية Curl_easy *داده)
{
  أرجع b->cf && !b->نتیجه && Curl_conn_cf_needs_flush(b->cf, داده);
}

ساكن CURLcode cf_hc_baller_cntrl(بنية cf_hc_baller *b,
                                   بنية Curl_easy *داده,
                                   صحيح event, صحيح arg1, فراغ *arg2)
{
  إذا(b->cf && !b->نتیجه)
    أرجع Curl_conn_cf_cntrl(b->cf, داده, FALSE, event, arg1, arg2);
  أرجع CURLE_OK;
}

بنية cf_hc_ctx {
  cf_hc_state state;
  بنية Curl_peer *destination; /* who we ultimately want to talk to */
  بنية curltime started;  /* when connect started */
  CURLcode نتیجه;          /* overall result */
  CURLcode check_h3_نتیجه;
  بنية cf_hc_baller ballers[2];
  size_t baller_شمار;
  timediff_t soft_eyeballs_timeout_ms;
  timediff_t hard_eyeballs_timeout_ms;
  uint8_t def_transport;
  بیت(httpsrr_resolved);
  بیت(checked_h3);
  بیت(ballers_complete);
};

ساكن فراغ cf_hc_ctx_destroy(بنية Curl_easy *داده,
                              بنية cf_hc_ctx *ctx)
{
  إذا(ctx) {
    size_t i;
    لكل(i = 0; i < ctx->baller_شمار; ++i)
      cf_hc_baller_discard(&ctx->ballers[i], داده);
    Curl_peer_unlink(&ctx->destination);
    curlx_free(ctx);
  }
}

ساكن فراغ cf_hc_baller_تخصیص_بده(بنية cf_hc_baller *b,
                                تعداد alpnid alpn_id,
                                uint8_t def_transport)
{
  b->alpn_id = alpn_id;
  b->transport = def_transport;
  b->cf = عدم;
  b->نتیجه = CURLE_OK;
  b->پاسخ_ms = -1;
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
    b->نتیجه = CURLE_FAILED_INIT;
    اكسر;
  }
}

ساكن فراغ cf_hc_baller_init(بنية cf_hc_baller *b,
                              بنية Curl_cfilter *cf,
                              بنية Curl_easy *داده)
{
  بنية Curl_cfilter *save = cf->بعدی;

  cf->بعدی = عدم;
  b->نتیجه = Curl_cf_راهاندازی_درج_after(cf, داده, b->transport,
                                         CURL_CF_SSL_ENABLE);
  b->cf = cf->بعدی;
  cf->بعدی = save;
}

ساكن CURLcode cf_hc_baller_connect(بنية cf_hc_baller *b,
                                     بنية Curl_cfilter *cf,
                                     بنية Curl_easy *داده,
                                     bool *انجامشده)
{
  بنية Curl_cfilter *save = cf->بعدی;

  cf->بعدی = b->cf;
  b->نتیجه = Curl_conn_cf_connect(cf->بعدی, داده, انجامشده);
  b->cf = cf->بعدی; /* it might mutate */
  cf->بعدی = save;
  أرجع b->نتیجه;
}

ساكن CURLcode baller_connected(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده,
                                 بنية cf_hc_baller *winner)
{
  بنية cf_hc_ctx *ctx = cf->ctx;

  /* Make the winner's connection filter out own sub-filter, check, move,
   * close all remaining. */
  إذا(cf->بعدی) {
    DEBUGASSERT(0);
    أرجع CURLE_FAILED_INIT;
  }
  إذا(!winner->cf) {
    DEBUGASSERT(0);
    أرجع CURLE_FAILED_INIT;
  }

  cf->بعدی = winner->cf;
  winner->cf = عدم;
  ctx->state = CF_HC_SUCCESS;
  cf->connected = TRUE;

  /* ballers may have failf()'d, the winner resets it, so our
   * errorbuf is clean again. */
  Curl_reset_fail(داده);

#اگر_تعریف_شده USE_NGHTTP2
  {
    /* For a negotiated HTTP/2 connection insert the h2 filter. */
    ثابت حرف *alpn = Curl_conn_cf_get_alpn_negotiated(cf->بعدی, داده);
    إذا(alpn && !strcmp("h2", alpn)) {
      CURLcode نتیجه = Curl_http2_switch_at(cf, داده);
      إذا(نتیجه) {
        ctx->state = CF_HC_FAILURE;
        ctx->نتیجه = نتیجه;
        أرجع نتیجه;
      }
    }
  }
#پایان_اگر
  أرجع CURLE_OK;
}

ساكن bool زمان_مقصد_شروع_baller2(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده,
                                  ثابت بنية curltime *pnow)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  timediff_t elapsed_ms;

  إذا(ctx->baller_شمار < 2)
    أرجع FALSE;
  وإلا إذا(cf_hc_baller_has_started(&ctx->ballers[1]))
    أرجع FALSE;
  وإلا إذا(ctx->ballers[0].نتیجه) {
    CURL_TRC_CF(داده, cf, "%s baller failed, starting %s",
                ctx->ballers[0].name, ctx->ballers[1].name);
    أرجع TRUE;
  }

  elapsed_ms = curlx_ptimediff_ms(pnow, &ctx->started);
  إذا(elapsed_ms >= ctx->hard_eyeballs_timeout_ms) {
    CURL_TRC_CF(داده, cf, "%s inconclusive after %" FMT_TIMEDIFF_T ", "
                "starting %s", ctx->ballers[0].name,
                ctx->hard_eyeballs_timeout_ms, ctx->ballers[1].name);
    أرجع TRUE;
  }
  وإلا إذا(elapsed_ms >= ctx->soft_eyeballs_timeout_ms) {
    إذا(cf_hc_baller_پاسخ_ms(&ctx->ballers[0], داده) < 0) {
      CURL_TRC_CF(داده, cf, "%s has not seen any data after %"
                  FMT_TIMEDIFF_T "ms, starting %s",
                  ctx->ballers[0].name, ctx->soft_eyeballs_timeout_ms,
                  ctx->ballers[1].name);
      أرجع TRUE;
    }
    /* set the effective hard timeout again */
    Curl_expire_مجموعه(داده, EXPIRE_ALPN_EYEBALLS,
                    ctx->hard_eyeballs_timeout_ms - elapsed_ms, pnow);
  }
  أرجع FALSE;
}

ساكن bool cf_hc_may_h3(بنية Curl_cfilter *cf,
                         بنية Curl_easy *داده)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  إذا(!ctx->checked_h3) {
    ctx->check_h3_نتیجه =
      Curl_conn_may_http3(داده, cf->conn);
    ctx->checked_h3 = TRUE;
  }
  أرجع !ctx->check_h3_نتیجه;
}

ساكن تعداد alpnid cf_hc_get_httpsrr_alpn(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *داده,
                                          تعداد alpnid not_this_one)
{
#اگر_تعریف_شده USE_HTTPSRR
  بنية cf_hc_ctx *ctx = cf->ctx;
  /* Is there an HTTPSRR use its ALPNs here.
   * We are here after having selected a connection to a host+port and
   * can no longer change that. Any HTTPSRR advice for other hosts and ports
   * we need to ignore. */
  ثابت بنية Curl_https_rrinfo *rr;
  size_t i;

  /* Do we have HTTPS-RR information? */
  rr = Curl_conn_dns_get_https(داده, cf->sockindex, ctx->destination);

  CURL_TRC_CF(داده, cf, "HTTPS-RR %savailable", rr ? "" : "not ");
  /* We do not support `rr->no_def_alpn`. */
  إذا(Curl_httpsrr_applicable(داده, rr) && !rr->no_def_alpn) {
    لكل(i = 0; i < CURL_ARRAYSIZE(rr->alpns); ++i) {
      تعداد alpnid alpn_rr = (تعداد alpnid)rr->alpns[i];
      إذا(alpn_rr == not_this_one) /* do not want this one */
        استمر;
      بدل(alpn_rr) {
      حالة ALPN_h3:
        إذا((داده->state.http_neg.allowed & CURL_HTTP_V3x) &&
           cf_hc_may_h3(cf, داده)) {
          أرجع alpn_rr;
        }
        اكسر;
      حالة ALPN_h2:
        إذا(داده->state.http_neg.allowed & CURL_HTTP_V2x) {
          أرجع alpn_rr;
        }
        اكسر;
      حالة ALPN_h1:
        إذا(داده->state.http_neg.allowed & CURL_HTTP_V1x) {
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
  (فراغ)داده;
  (فراغ)not_this_one;
#پایان_اگر
  أرجع ALPN_none;
}

ساكن تعداد alpnid cf_hc_get_pref_alpn(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *داده,
                                       تعداد alpnid not_this_one)
{
  إذا((داده->state.http_neg.preferred & داده->state.http_neg.allowed)) {
    بدل(داده->state.http_neg.preferred) {
    حالة CURL_HTTP_V3x:
      إذا(cf_hc_may_h3(cf, داده) && (ALPN_h3 != not_this_one))
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

ساكن تعداد alpnid cf_hc_get_اولی_alpn(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *داده,
                                        http_majors choices,
                                        تعداد alpnid not_this_one)
{
  /* When told to not try h2, we also do not try h1 and vice versa */
  bool allow_h1_or_h2 = (not_this_one != ALPN_h1) &&
                        (not_this_one != ALPN_h2);
  إذا((ALPN_h3 != not_this_one) && (choices & CURL_HTTP_V3x) &&
     cf_hc_may_h3(cf, داده)) {
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

ساكن CURLcode cf_hc_مجموعه_baller1(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  تعداد alpnid alpn1 = ALPN_none;
  VERBOSE(ثابت حرف *منبع = "HTTPS-RR");

  DEBUGASSERT(cf->conn->بیتها.tls_enable_alpn);

  alpn1 = cf_hc_get_httpsrr_alpn(cf, داده, ALPN_none);
  إذا(alpn1 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(منبع = "preferred version");
    alpn1 = cf_hc_get_pref_alpn(cf, داده, ALPN_none);
  }
  إذا(alpn1 == ALPN_none) {
    VERBOSE(منبع = "wanted versions");
    alpn1 = cf_hc_get_اولی_alpn(cf, داده,
                                 داده->state.http_neg.wanted,
                                 ALPN_none);
  }
  إذا(alpn1 == ALPN_none) {
    VERBOSE(منبع = "allowed versions");
    alpn1 = cf_hc_get_اولی_alpn(cf, داده,
                                 داده->state.http_neg.allowed,
                                 ALPN_none);
  }

  إذا(alpn1 == ALPN_none) {
    /* None of the wanted/allowed HTTP versions could be chosen */
    إذا(ctx->check_h3_نتیجه) {
      CURL_TRC_CF(داده, cf, "unable to use HTTP/3");
      أرجع ctx->check_h3_نتیجه;
    }
    CURL_TRC_CF(داده, cf, "unable to select HTTP version");
    أرجع CURLE_FAILED_INIT;
  }

  cf_hc_baller_تخصیص_بده(&ctx->ballers[0], alpn1, ctx->def_transport);
  ctx->baller_شمار = 1;
  CURL_TRC_CF(داده, cf, "1st attempt uses %s from %s",
              ctx->ballers[0].name, منبع);

  بدل(alpn1) {
  حالة ALPN_h1:
    /* We really want h1, switch off h2 to make it disappear in ALPN */
    داده->state.http_neg.wanted &= (uint8_t)~CURL_HTTP_V2x;
    اكسر;
  افتراضي:
    اكسر;
  }

  أرجع CURLE_OK;
}

ساكن فراغ cf_hc_مجموعه_baller2(بنية Curl_cfilter *cf,
                              بنية Curl_easy *داده)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  تعداد alpnid alpn2 = ALPN_none, alpn1 = ctx->ballers[0].alpn_id;
  VERBOSE(ثابت حرف *منبع = "HTTPS-RR");

  إذا(ctx->ballers_complete)
    أرجع; /* already done */
  إذا(!ctx->httpsrr_resolved)
    أرجع; /* HTTPS-RR pending */

  alpn2 = cf_hc_get_httpsrr_alpn(cf, داده, alpn1);
  إذا(alpn2 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(منبع = "preferred version");
    alpn2 = cf_hc_get_pref_alpn(cf, داده, alpn1);
  }
  إذا(alpn2 == ALPN_none) {
    VERBOSE(منبع = "wanted versions");
    alpn2 = cf_hc_get_اولی_alpn(cf, داده,
                                 داده->state.http_neg.wanted,
                                 alpn1);
  }

  إذا(alpn2 != ALPN_none) {
    cf_hc_baller_تخصیص_بده(&ctx->ballers[1], alpn2, ctx->def_transport);
    ctx->baller_شمار = 2;
    CURL_TRC_CF(داده, cf, "2nd attempt uses %s from %s",
                ctx->ballers[1].name, منبع);
  }
  ctx->ballers_complete = TRUE;
}

ساكن CURLcode cf_hc_connect(بنية Curl_cfilter *cf,
                              بنية Curl_easy *داده,
                              bool *انجامشده)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;
  ثابت بنية curltime *pnow = عدم;

  إذا(cf->connected) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  *انجامشده = FALSE;

  إذا(!ctx->httpsrr_resolved) {
    ctx->httpsrr_resolved = Curl_conn_dns_resolved_https(
      داده, cf->sockindex, ctx->destination);
#اگر_تعریف_شده DEBUGBUILD
    إذا(!ctx->httpsrr_resolved && getenv("CURL_DBG_AWAIT_HTTPSRR")) {
      CURL_TRC_CF(داده, cf, "awaiting HTTPS-RR");
      أرجع CURLE_OK;
    }
#پایان_اگر
  }

  بدل(ctx->state) {
  حالة CF_HC_RESOLV:
    ctx->state = CF_HC_INIT;
    FALLTHROUGH();

  حالة CF_HC_INIT:
    DEBUGASSERT(!cf->بعدی);
    CURL_TRC_CF(داده, cf, "connect, init");
    نتیجه = cf_hc_مجموعه_baller1(cf, داده);
    إذا(نتیجه) {
      ctx->نتیجه = نتیجه;
      ctx->state = CF_HC_FAILURE;
      اذهب بیرون;
    }
    cf_hc_مجموعه_baller2(cf, داده);
    pnow = Curl_pgrs_now(داده);
    ctx->started = *pnow;
    cf_hc_baller_init(&ctx->ballers[0], cf, داده);
    إذا((ctx->baller_شمار > 1) || !ctx->ballers_complete) {
      Curl_expire_مجموعه(داده, EXPIRE_ALPN_EYEBALLS,
                      ctx->soft_eyeballs_timeout_ms, pnow);
    }
    ctx->state = CF_HC_CONNECT;
    FALLTHROUGH();

  حالة CF_HC_CONNECT:
    إذا(!ctx->ballers_complete)
      cf_hc_مجموعه_baller2(cf, داده);

    إذا(cf_hc_baller_is_connecting(&ctx->ballers[0])) {
      نتیجه = cf_hc_baller_connect(&ctx->ballers[0], cf, داده, انجامشده);
      إذا(!نتیجه && *انجامشده) {
        نتیجه = baller_connected(cf, داده, &ctx->ballers[0]);
        اذهب بیرون;
      }
    }

    pnow = Curl_pgrs_now(داده);
    إذا(زمان_مقصد_شروع_baller2(cf, داده, pnow)) {
      cf_hc_baller_init(&ctx->ballers[1], cf, داده);
    }

    إذا(cf_hc_baller_is_connecting(&ctx->ballers[1])) {
      نتیجه = cf_hc_baller_connect(&ctx->ballers[1], cf, داده, انجامشده);
      إذا(!نتیجه && *انجامشده) {
        نتیجه = baller_connected(cf, داده, &ctx->ballers[1]);
        اذهب بیرون;
      }
    }

    إذا(ctx->ballers[0].نتیجه &&
       (ctx->ballers[1].نتیجه ||
        (ctx->ballers_complete && (ctx->baller_شمار < 2)))) {
      /* all have failed. we give up */
      CURL_TRC_CF(داده, cf, "connect, all attempts failed");
      ctx->نتیجه = نتیجه = ctx->ballers[0].نتیجه;
      ctx->state = CF_HC_FAILURE;
      اذهب بیرون;
    }
    نتیجه = CURLE_OK;
    *انجامشده = FALSE;
    اكسر;

  حالة CF_HC_FAILURE:
    نتیجه = ctx->نتیجه;
    cf->connected = FALSE;
    *انجامشده = FALSE;
    اكسر;

  حالة CF_HC_SUCCESS:
    نتیجه = CURLE_OK;
    cf->connected = TRUE;
    *انجامشده = TRUE;
    اكسر;
  }

بیرون:
  CURL_TRC_CF(داده, cf, "connect -> %d, done=%d", (صحيح)نتیجه, *انجامشده);
  أرجع نتیجه;
}

ساكن CURLcode cf_hc_shutdown(بنية Curl_cfilter *cf,
                               بنية Curl_easy *داده, bool *انجامشده)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  size_t i;
  CURLcode نتیجه = CURLE_OK;

  DEBUGASSERT(داده);
  إذا(cf->connected) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  لكل(i = 0; i < ctx->baller_شمار; i++) {
    بنية cf_hc_baller *b = &ctx->ballers[i];
    bool bdone = FALSE;
    إذا(!cf_hc_baller_is_connecting(b) || b->shutdown)
      استمر;
    b->نتیجه = b->cf->cft->do_shutdown(b->cf, داده, &bdone);
    إذا(b->نتیجه || bdone)
      b->shutdown = TRUE; /* treat a failed shutdown as done */
  }

  *انجامشده = TRUE;
  لكل(i = 0; i < ctx->baller_شمار; i++) {
    إذا(!ctx->ballers[i].shutdown)
      *انجامشده = FALSE;
  }
  إذا(*انجامشده) {
    لكل(i = 0; i < ctx->baller_شمار; i++) {
      إذا(ctx->ballers[i].نتیجه)
        نتیجه = ctx->ballers[i].نتیجه;
    }
  }
  CURL_TRC_CF(داده, cf, "shutdown -> %d, done=%d", (صحيح)نتیجه, *انجامشده);
  أرجع نتیجه;
}

ساكن CURLcode cf_hc_adjust_pollset(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *داده,
                                     بنية easy_pollset *ps)
{
  CURLcode نتیجه = CURLE_OK;
  إذا(!cf->connected) {
    بنية cf_hc_ctx *ctx = cf->ctx;
    size_t i;

    لكل(i = 0; (i < ctx->baller_شمار) && !نتیجه; i++) {
      بنية cf_hc_baller *b = &ctx->ballers[i];
      إذا(!cf_hc_baller_is_connecting(b))
        استمر;
      نتیجه = Curl_conn_cf_adjust_pollset(b->cf, داده, ps);
    }
    CURL_TRC_CF(داده, cf, "adjust_pollset -> %d, %u socks", (صحيح)نتیجه,
                ps->n);
  }
  أرجع نتیجه;
}

ساكن bool cf_hc_داده_pending(بنية Curl_cfilter *cf,
                               ثابت بنية Curl_easy *داده)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  إذا(cf->connected)
    أرجع cf->بعدی->cft->has_داده_pending(cf->بعدی, داده);

  لكل(i = 0; i < ctx->baller_شمار; i++)
    إذا(cf_hc_baller_داده_pending(&ctx->ballers[i], داده))
      أرجع TRUE;
  أرجع FALSE;
}

ساكن CURLcode cf_hc_query(بنية Curl_cfilter *cf,
                            بنية Curl_easy *داده,
                            صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  إذا(!cf->connected) {
    بدل(query) {
    حالة CF_QUERY_NEED_FLUSH: {
      لكل(i = 0; i < ctx->baller_شمار; i++)
        إذا(cf_hc_baller_needs_flush(&ctx->ballers[i], داده)) {
          *pres1 = TRUE;
          أرجع CURLE_OK;
        }
      اكسر;
    }
    افتراضي:
      اكسر;
    }
  }
  أرجع cf->بعدی ?
    cf->بعدی->cft->query(cf->بعدی, داده, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساكن CURLcode cf_hc_cntrl(بنية Curl_cfilter *cf,
                            بنية Curl_easy *داده,
                            صحيح event, صحيح arg1, فراغ *arg2)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;
  size_t i;

  إذا(!cf->connected) {
    بدل(event) {
    حالة CF_CTRL_REPORT_STATS:
    حالة CF_CTRL_CONN_INFO_UPDATE:
      لكل(i = 0; i < ctx->baller_شمار; i++) {
        /* Make the first baller that connected at network level report */
        إذا(Curl_conn_cf_is_ip_connected(ctx->ballers[i].cf, داده)) {
          Curl_conn_cf_cntrl(ctx->ballers[i].cf, داده, TRUE,
                             event, arg1, arg2);
          اكسر;
        }
      }
      اكسر;
    افتراضي:
      لكل(i = 0; i < ctx->baller_شمار; i++) {
        نتیجه = cf_hc_baller_cntrl(&ctx->ballers[i], داده, event, arg1, arg2);
        إذا(نتیجه && (نتیجه != CURLE_AGAIN))
          اذهب بیرون;
      }
      نتیجه = CURLE_OK;
      اكسر;
    }
  }
بیرون:
  أرجع نتیجه;
}

ساكن فراغ cf_hc_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *داده)
{
  بنية cf_hc_ctx *ctx = cf->ctx;

  CURL_TRC_CF(داده, cf, "destroy");
  cf_hc_ctx_destroy(داده, ctx);
}

بنية Curl_cftype Curl_cft_http_connect = {
  "HTTPS-CONNECT",
  CF_TYPE_راهاندازی,
  CURL_LOG_LVL_NONE,
  cf_hc_destroy,
  cf_hc_connect,
  cf_hc_shutdown,
  cf_hc_adjust_pollset,
  cf_hc_داده_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  cf_hc_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_hc_query,
};

ساكن CURLcode cf_hc_create(بنية Curl_cfilter **pcf,
                             بنية Curl_easy *داده,
                             بنية Curl_peer *destination,
                             uint8_t def_transport)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_hc_ctx *ctx;
  CURLcode نتیجه = CURLE_OK;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }
  Curl_peer_link(&ctx->destination, destination);
  ctx->def_transport = def_transport;
  ctx->hard_eyeballs_timeout_ms = داده->مجموعه.happy_eyeballs_timeout;
  ctx->soft_eyeballs_timeout_ms = داده->مجموعه.happy_eyeballs_timeout / 2;

  نتیجه = Curl_cf_create(&cf, &Curl_cft_http_connect, ctx);
  إذا(نتیجه)
    اذهب بیرون;
  ctx = عدم;

بیرون:
  *pcf = نتیجه ? عدم : cf;
  cf_hc_ctx_destroy(داده, ctx);
  أرجع نتیجه;
}

ساكن CURLcode cf_hc_جمع_کن(بنية Curl_easy *داده,
                          بنية Curl_peer *destination,
                          بنية connectdata *conn,
                          int8_t sockindex,
                          uint8_t def_transport)
{
  بنية Curl_cfilter *cf;
  CURLcode نتیجه = CURLE_OK;

  DEBUGASSERT(داده);
  نتیجه = cf_hc_create(&cf, داده, destination, def_transport);
  إذا(نتیجه)
    اذهب بیرون;
  Curl_conn_cf_جمع_کن(داده, conn, sockindex, cf);

#اگر_تعریف_شده USE_HTTPSRR
  نتیجه = Curl_conn_dns_جمع_کن_https_resolve(داده, cf->conn, cf->sockindex,
                                           destination);
#پایان_اگر
بیرون:
  أرجع نتیجه;
}

CURLcode Curl_cf_https_راهاندازی(بنية Curl_easy *داده,
                             بنية Curl_peer *destination,
                             بنية connectdata *conn,
                             int8_t sockindex)
{
  CURLcode نتیجه = CURLE_OK;

  DEBUGASSERT(conn->scheme->protocol == CURLPROTO_HTTPS);

  /* This filter is intended for HTTPS using ALPN and does
   * not support HTTPS Eyeballing to a proxy. */
  إذا((conn->scheme->protocol != CURLPROTO_HTTPS) ||
#اگر_تعریف_نشده CURL_DISABLE_PROXY
     conn->بیتها.origin_is_proxy ||
#پایان_اگر
     !conn->بیتها.tls_enable_alpn)
    اذهب بیرون;

  نتیجه = cf_hc_جمع_کن(داده, destination, conn, sockindex,
                     conn->transport_wanted);

بیرون:
  أرجع نتیجه;
}

#پایان_اگر /* !CURL_DISABLE_HTTP */
