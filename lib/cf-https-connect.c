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

#جے_بنا_تعریف CURL_DISABLE_HTTP

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
  CURLcode نتیجہ;
  صحيح جواب_ms;
  uint8_t transport;
  تعداد alpnid alpn_id;
  ثنائی_ہندسہ(shutdown);
};

ساكن فراغ cf_hc_baller_discard(بنية cf_hc_baller *b,
                                 بنية Curl_easy *انکڑے)
{
  إذا(b->cf) {
    Curl_conn_cf_discard_chain(&b->cf, انکڑے);
    b->cf = عدم;
  }
}

ساكن bool cf_hc_baller_is_connecting(بنية cf_hc_baller *b)
{
  أرجع b->cf && !b->نتیجہ;
}

ساكن bool cf_hc_baller_has_started(بنية cf_hc_baller *b)
{
  أرجع !!b->cf;
}

ساكن صحيح cf_hc_baller_جواب_ms(بنية cf_hc_baller *b,
                                 بنية Curl_easy *انکڑے)
{
  إذا(b->cf && (b->جواب_ms < 0))
    b->cf->cft->query(b->cf, انکڑے, CF_QUERY_CONNECT_جواب_MS,
                      &b->جواب_ms, عدم);
  أرجع b->جواب_ms;
}

ساكن bool cf_hc_baller_انکڑے_pending(بنية cf_hc_baller *b,
                                      ثابت بنية Curl_easy *انکڑے)
{
  أرجع b->cf && !b->نتیجہ && b->cf->cft->has_انکڑے_pending(b->cf, انکڑے);
}

ساكن bool cf_hc_baller_needs_flush(بنية cf_hc_baller *b,
                                     بنية Curl_easy *انکڑے)
{
  أرجع b->cf && !b->نتیجہ && Curl_conn_cf_needs_flush(b->cf, انکڑے);
}

ساكن CURLcode cf_hc_baller_cntrl(بنية cf_hc_baller *b,
                                   بنية Curl_easy *انکڑے,
                                   صحيح event, صحيح arg1, فراغ *arg2)
{
  إذا(b->cf && !b->نتیجہ)
    أرجع Curl_conn_cf_cntrl(b->cf, انکڑے, FALSE, event, arg1, arg2);
  أرجع CURLE_OK;
}

بنية cf_hc_ctx {
  cf_hc_state state;
  بنية Curl_peer *destination; /* who we ultimately want to talk to */
  بنية curltime started;  /* when connect started */
  CURLcode نتیجہ;          /* overall result */
  CURLcode check_h3_نتیجہ;
  بنية cf_hc_baller ballers[2];
  size_t baller_گنتی;
  timediff_t soft_eyeballs_timeout_ms;
  timediff_t hard_eyeballs_timeout_ms;
  uint8_t def_transport;
  ثنائی_ہندسہ(httpsrr_resolved);
  ثنائی_ہندسہ(checked_h3);
  ثنائی_ہندسہ(ballers_complete);
};

ساكن فراغ cf_hc_ctx_destroy(بنية Curl_easy *انکڑے,
                              بنية cf_hc_ctx *ctx)
{
  إذا(ctx) {
    size_t i;
    لكل(i = 0; i < ctx->baller_گنتی; ++i)
      cf_hc_baller_discard(&ctx->ballers[i], انکڑے);
    Curl_peer_unlink(&ctx->destination);
    curlx_free(ctx);
  }
}

ساكن فراغ cf_hc_baller_سونپو(بنية cf_hc_baller *b,
                                تعداد alpnid alpn_id,
                                uint8_t def_transport)
{
  b->alpn_id = alpn_id;
  b->transport = def_transport;
  b->cf = عدم;
  b->نتیجہ = CURLE_OK;
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
    b->نتیجہ = CURLE_FAILED_INIT;
    اكسر;
  }
}

ساكن فراغ cf_hc_baller_init(بنية cf_hc_baller *b,
                              بنية Curl_cfilter *cf,
                              بنية Curl_easy *انکڑے)
{
  بنية Curl_cfilter *save = cf->اگلا;

  cf->اگلا = عدم;
  b->نتیجہ = Curl_cf_تیاری_داخل_کرو_after(cf, انکڑے, b->transport,
                                         CURL_CF_SSL_ENABLE);
  b->cf = cf->اگلا;
  cf->اگلا = save;
}

ساكن CURLcode cf_hc_baller_connect(بنية cf_hc_baller *b,
                                     بنية Curl_cfilter *cf,
                                     بنية Curl_easy *انکڑے,
                                     bool *پورا)
{
  بنية Curl_cfilter *save = cf->اگلا;

  cf->اگلا = b->cf;
  b->نتیجہ = Curl_conn_cf_connect(cf->اگلا, انکڑے, پورا);
  b->cf = cf->اگلا; /* it might mutate */
  cf->اگلا = save;
  أرجع b->نتیجہ;
}

ساكن CURLcode baller_connected(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *انکڑے,
                                 بنية cf_hc_baller *winner)
{
  بنية cf_hc_ctx *ctx = cf->ctx;

  /* Make the winner's connection filter out own sub-filter, check, move,
   * close all remaining. */
  إذا(cf->اگلا) {
    DEBUGASSERT(0);
    أرجع CURLE_FAILED_INIT;
  }
  إذا(!winner->cf) {
    DEBUGASSERT(0);
    أرجع CURLE_FAILED_INIT;
  }

  cf->اگلا = winner->cf;
  winner->cf = عدم;
  ctx->state = CF_HC_SUCCESS;
  cf->connected = TRUE;

  /* ballers may have failf()'d, the winner resets it, so our
   * errorbuf is clean again. */
  Curl_reset_fail(انکڑے);

#جے_تعریف_شدہ USE_NGHTTP2
  {
    /* For a negotiated HTTP/2 connection insert the h2 filter. */
    ثابت حرف *alpn = Curl_conn_cf_get_alpn_negotiated(cf->اگلا, انکڑے);
    إذا(alpn && !strcmp("h2", alpn)) {
      CURLcode نتیجہ = Curl_http2_switch_at(cf, انکڑے);
      إذا(نتیجہ) {
        ctx->state = CF_HC_FAILURE;
        ctx->نتیجہ = نتیجہ;
        أرجع نتیجہ;
      }
    }
  }
#جے_انت
  أرجع CURLE_OK;
}

ساكن bool ویلا_منزل_شروع_baller2(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *انکڑے,
                                  ثابت بنية curltime *pnow)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  timediff_t elapsed_ms;

  إذا(ctx->baller_گنتی < 2)
    أرجع FALSE;
  وإلا إذا(cf_hc_baller_has_started(&ctx->ballers[1]))
    أرجع FALSE;
  وإلا إذا(ctx->ballers[0].نتیجہ) {
    CURL_TRC_CF(انکڑے, cf, "%s baller failed, starting %s",
                ctx->ballers[0].name, ctx->ballers[1].name);
    أرجع TRUE;
  }

  elapsed_ms = curlx_ptimediff_ms(pnow, &ctx->started);
  إذا(elapsed_ms >= ctx->hard_eyeballs_timeout_ms) {
    CURL_TRC_CF(انکڑے, cf, "%s inconclusive after %" FMT_TIMEDIFF_T ", "
                "starting %s", ctx->ballers[0].name,
                ctx->hard_eyeballs_timeout_ms, ctx->ballers[1].name);
    أرجع TRUE;
  }
  وإلا إذا(elapsed_ms >= ctx->soft_eyeballs_timeout_ms) {
    إذا(cf_hc_baller_جواب_ms(&ctx->ballers[0], انکڑے) < 0) {
      CURL_TRC_CF(انکڑے, cf, "%s has not seen any data after %"
                  FMT_TIMEDIFF_T "ms, starting %s",
                  ctx->ballers[0].name, ctx->soft_eyeballs_timeout_ms,
                  ctx->ballers[1].name);
      أرجع TRUE;
    }
    /* set the effective hard timeout again */
    Curl_expire_سیٹ(انکڑے, EXPIRE_ALPN_EYEBALLS,
                    ctx->hard_eyeballs_timeout_ms - elapsed_ms, pnow);
  }
  أرجع FALSE;
}

ساكن bool cf_hc_may_h3(بنية Curl_cfilter *cf,
                         بنية Curl_easy *انکڑے)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  إذا(!ctx->checked_h3) {
    ctx->check_h3_نتیجہ =
      Curl_conn_may_http3(انکڑے, cf->conn);
    ctx->checked_h3 = TRUE;
  }
  أرجع !ctx->check_h3_نتیجہ;
}

ساكن تعداد alpnid cf_hc_get_httpsrr_alpn(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *انکڑے,
                                          تعداد alpnid not_this_one)
{
#جے_تعریف_شدہ USE_HTTPSRR
  بنية cf_hc_ctx *ctx = cf->ctx;
  /* Is there an HTTPSRR use its ALPNs here.
   * We are here after having selected a connection to a host+port and
   * can no longer change that. Any HTTPSRR advice for other hosts and ports
   * we need to ignore. */
  ثابت بنية Curl_https_rrinfo *rr;
  size_t i;

  /* Do we have HTTPS-RR information? */
  rr = Curl_conn_dns_get_https(انکڑے, cf->sockindex, ctx->destination);

  CURL_TRC_CF(انکڑے, cf, "HTTPS-RR %savailable", rr ? "" : "not ");
  /* We do not support `rr->no_def_alpn`. */
  إذا(Curl_httpsrr_applicable(انکڑے, rr) && !rr->no_def_alpn) {
    لكل(i = 0; i < CURL_ARRAYSIZE(rr->alpns); ++i) {
      تعداد alpnid alpn_rr = (تعداد alpnid)rr->alpns[i];
      إذا(alpn_rr == not_this_one) /* do not want this one */
        استمر;
      بدل(alpn_rr) {
      حالة ALPN_h3:
        إذا((انکڑے->state.http_neg.allowed & CURL_HTTP_V3x) &&
           cf_hc_may_h3(cf, انکڑے)) {
          أرجع alpn_rr;
        }
        اكسر;
      حالة ALPN_h2:
        إذا(انکڑے->state.http_neg.allowed & CURL_HTTP_V2x) {
          أرجع alpn_rr;
        }
        اكسر;
      حالة ALPN_h1:
        إذا(انکڑے->state.http_neg.allowed & CURL_HTTP_V1x) {
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
  (فراغ)انکڑے;
  (فراغ)not_this_one;
#جے_انت
  أرجع ALPN_none;
}

ساكن تعداد alpnid cf_hc_get_pref_alpn(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *انکڑے,
                                       تعداد alpnid not_this_one)
{
  إذا((انکڑے->state.http_neg.preferred & انکڑے->state.http_neg.allowed)) {
    بدل(انکڑے->state.http_neg.preferred) {
    حالة CURL_HTTP_V3x:
      إذا(cf_hc_may_h3(cf, انکڑے) && (ALPN_h3 != not_this_one))
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

ساكن تعداد alpnid cf_hc_get_پہلا_alpn(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *انکڑے,
                                        http_majors choices,
                                        تعداد alpnid not_this_one)
{
  /* When told to not try h2, we also do not try h1 and vice versa */
  bool allow_h1_or_h2 = (not_this_one != ALPN_h1) &&
                        (not_this_one != ALPN_h2);
  إذا((ALPN_h3 != not_this_one) && (choices & CURL_HTTP_V3x) &&
     cf_hc_may_h3(cf, انکڑے)) {
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

ساكن CURLcode cf_hc_سیٹ_baller1(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *انکڑے)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  تعداد alpnid alpn1 = ALPN_none;
  VERBOSE(ثابت حرف *سروت = "HTTPS-RR");

  DEBUGASSERT(cf->conn->بٹ.tls_enable_alpn);

  alpn1 = cf_hc_get_httpsrr_alpn(cf, انکڑے, ALPN_none);
  إذا(alpn1 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(سروت = "preferred version");
    alpn1 = cf_hc_get_pref_alpn(cf, انکڑے, ALPN_none);
  }
  إذا(alpn1 == ALPN_none) {
    VERBOSE(سروت = "wanted versions");
    alpn1 = cf_hc_get_پہلا_alpn(cf, انکڑے,
                                 انکڑے->state.http_neg.wanted,
                                 ALPN_none);
  }
  إذا(alpn1 == ALPN_none) {
    VERBOSE(سروت = "allowed versions");
    alpn1 = cf_hc_get_پہلا_alpn(cf, انکڑے,
                                 انکڑے->state.http_neg.allowed,
                                 ALPN_none);
  }

  إذا(alpn1 == ALPN_none) {
    /* None of the wanted/allowed HTTP versions could be chosen */
    إذا(ctx->check_h3_نتیجہ) {
      CURL_TRC_CF(انکڑے, cf, "unable to use HTTP/3");
      أرجع ctx->check_h3_نتیجہ;
    }
    CURL_TRC_CF(انکڑے, cf, "unable to select HTTP version");
    أرجع CURLE_FAILED_INIT;
  }

  cf_hc_baller_سونپو(&ctx->ballers[0], alpn1, ctx->def_transport);
  ctx->baller_گنتی = 1;
  CURL_TRC_CF(انکڑے, cf, "1st attempt uses %s from %s",
              ctx->ballers[0].name, سروت);

  بدل(alpn1) {
  حالة ALPN_h1:
    /* We really want h1, switch off h2 to make it disappear in ALPN */
    انکڑے->state.http_neg.wanted &= (uint8_t)~CURL_HTTP_V2x;
    اكسر;
  افتراضي:
    اكسر;
  }

  أرجع CURLE_OK;
}

ساكن فراغ cf_hc_سیٹ_baller2(بنية Curl_cfilter *cf,
                              بنية Curl_easy *انکڑے)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  تعداد alpnid alpn2 = ALPN_none, alpn1 = ctx->ballers[0].alpn_id;
  VERBOSE(ثابت حرف *سروت = "HTTPS-RR");

  إذا(ctx->ballers_complete)
    أرجع; /* already done */
  إذا(!ctx->httpsrr_resolved)
    أرجع; /* HTTPS-RR pending */

  alpn2 = cf_hc_get_httpsrr_alpn(cf, انکڑے, alpn1);
  إذا(alpn2 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(سروت = "preferred version");
    alpn2 = cf_hc_get_pref_alpn(cf, انکڑے, alpn1);
  }
  إذا(alpn2 == ALPN_none) {
    VERBOSE(سروت = "wanted versions");
    alpn2 = cf_hc_get_پہلا_alpn(cf, انکڑے,
                                 انکڑے->state.http_neg.wanted,
                                 alpn1);
  }

  إذا(alpn2 != ALPN_none) {
    cf_hc_baller_سونپو(&ctx->ballers[1], alpn2, ctx->def_transport);
    ctx->baller_گنتی = 2;
    CURL_TRC_CF(انکڑے, cf, "2nd attempt uses %s from %s",
                ctx->ballers[1].name, سروت);
  }
  ctx->ballers_complete = TRUE;
}

ساكن CURLcode cf_hc_connect(بنية Curl_cfilter *cf,
                              بنية Curl_easy *انکڑے,
                              bool *پورا)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;
  ثابت بنية curltime *pnow = عدم;

  إذا(cf->connected) {
    *پورا = TRUE;
    أرجع CURLE_OK;
  }

  *پورا = FALSE;

  إذا(!ctx->httpsrr_resolved) {
    ctx->httpsrr_resolved = Curl_conn_dns_resolved_https(
      انکڑے, cf->sockindex, ctx->destination);
#جے_تعریف_شدہ DEBUGBUILD
    إذا(!ctx->httpsrr_resolved && getenv("CURL_DBG_AWAIT_HTTPSRR")) {
      CURL_TRC_CF(انکڑے, cf, "awaiting HTTPS-RR");
      أرجع CURLE_OK;
    }
#جے_انت
  }

  بدل(ctx->state) {
  حالة CF_HC_RESOLV:
    ctx->state = CF_HC_INIT;
    FALLTHROUGH();

  حالة CF_HC_INIT:
    DEBUGASSERT(!cf->اگلا);
    CURL_TRC_CF(انکڑے, cf, "connect, init");
    نتیجہ = cf_hc_سیٹ_baller1(cf, انکڑے);
    إذا(نتیجہ) {
      ctx->نتیجہ = نتیجہ;
      ctx->state = CF_HC_FAILURE;
      اذهب باہرلا;
    }
    cf_hc_سیٹ_baller2(cf, انکڑے);
    pnow = Curl_pgrs_now(انکڑے);
    ctx->started = *pnow;
    cf_hc_baller_init(&ctx->ballers[0], cf, انکڑے);
    إذا((ctx->baller_گنتی > 1) || !ctx->ballers_complete) {
      Curl_expire_سیٹ(انکڑے, EXPIRE_ALPN_EYEBALLS,
                      ctx->soft_eyeballs_timeout_ms, pnow);
    }
    ctx->state = CF_HC_CONNECT;
    FALLTHROUGH();

  حالة CF_HC_CONNECT:
    إذا(!ctx->ballers_complete)
      cf_hc_سیٹ_baller2(cf, انکڑے);

    إذا(cf_hc_baller_is_connecting(&ctx->ballers[0])) {
      نتیجہ = cf_hc_baller_connect(&ctx->ballers[0], cf, انکڑے, پورا);
      إذا(!نتیجہ && *پورا) {
        نتیجہ = baller_connected(cf, انکڑے, &ctx->ballers[0]);
        اذهب باہرلا;
      }
    }

    pnow = Curl_pgrs_now(انکڑے);
    إذا(ویلا_منزل_شروع_baller2(cf, انکڑے, pnow)) {
      cf_hc_baller_init(&ctx->ballers[1], cf, انکڑے);
    }

    إذا(cf_hc_baller_is_connecting(&ctx->ballers[1])) {
      نتیجہ = cf_hc_baller_connect(&ctx->ballers[1], cf, انکڑے, پورا);
      إذا(!نتیجہ && *پورا) {
        نتیجہ = baller_connected(cf, انکڑے, &ctx->ballers[1]);
        اذهب باہرلا;
      }
    }

    إذا(ctx->ballers[0].نتیجہ &&
       (ctx->ballers[1].نتیجہ ||
        (ctx->ballers_complete && (ctx->baller_گنتی < 2)))) {
      /* all have failed. we give up */
      CURL_TRC_CF(انکڑے, cf, "connect, all attempts failed");
      ctx->نتیجہ = نتیجہ = ctx->ballers[0].نتیجہ;
      ctx->state = CF_HC_FAILURE;
      اذهب باہرلا;
    }
    نتیجہ = CURLE_OK;
    *پورا = FALSE;
    اكسر;

  حالة CF_HC_FAILURE:
    نتیجہ = ctx->نتیجہ;
    cf->connected = FALSE;
    *پورا = FALSE;
    اكسر;

  حالة CF_HC_SUCCESS:
    نتیجہ = CURLE_OK;
    cf->connected = TRUE;
    *پورا = TRUE;
    اكسر;
  }

باہرلا:
  CURL_TRC_CF(انکڑے, cf, "connect -> %d, done=%d", (صحيح)نتیجہ, *پورا);
  أرجع نتیجہ;
}

ساكن CURLcode cf_hc_shutdown(بنية Curl_cfilter *cf,
                               بنية Curl_easy *انکڑے, bool *پورا)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  size_t i;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(انکڑے);
  إذا(cf->connected) {
    *پورا = TRUE;
    أرجع CURLE_OK;
  }

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  لكل(i = 0; i < ctx->baller_گنتی; i++) {
    بنية cf_hc_baller *b = &ctx->ballers[i];
    bool bdone = FALSE;
    إذا(!cf_hc_baller_is_connecting(b) || b->shutdown)
      استمر;
    b->نتیجہ = b->cf->cft->do_shutdown(b->cf, انکڑے, &bdone);
    إذا(b->نتیجہ || bdone)
      b->shutdown = TRUE; /* treat a failed shutdown as done */
  }

  *پورا = TRUE;
  لكل(i = 0; i < ctx->baller_گنتی; i++) {
    إذا(!ctx->ballers[i].shutdown)
      *پورا = FALSE;
  }
  إذا(*پورا) {
    لكل(i = 0; i < ctx->baller_گنتی; i++) {
      إذا(ctx->ballers[i].نتیجہ)
        نتیجہ = ctx->ballers[i].نتیجہ;
    }
  }
  CURL_TRC_CF(انکڑے, cf, "shutdown -> %d, done=%d", (صحيح)نتیجہ, *پورا);
  أرجع نتیجہ;
}

ساكن CURLcode cf_hc_adjust_pollset(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *انکڑے,
                                     بنية easy_pollset *ps)
{
  CURLcode نتیجہ = CURLE_OK;
  إذا(!cf->connected) {
    بنية cf_hc_ctx *ctx = cf->ctx;
    size_t i;

    لكل(i = 0; (i < ctx->baller_گنتی) && !نتیجہ; i++) {
      بنية cf_hc_baller *b = &ctx->ballers[i];
      إذا(!cf_hc_baller_is_connecting(b))
        استمر;
      نتیجہ = Curl_conn_cf_adjust_pollset(b->cf, انکڑے, ps);
    }
    CURL_TRC_CF(انکڑے, cf, "adjust_pollset -> %d, %u socks", (صحيح)نتیجہ,
                ps->n);
  }
  أرجع نتیجہ;
}

ساكن bool cf_hc_انکڑے_pending(بنية Curl_cfilter *cf,
                               ثابت بنية Curl_easy *انکڑے)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  إذا(cf->connected)
    أرجع cf->اگلا->cft->has_انکڑے_pending(cf->اگلا, انکڑے);

  لكل(i = 0; i < ctx->baller_گنتی; i++)
    إذا(cf_hc_baller_انکڑے_pending(&ctx->ballers[i], انکڑے))
      أرجع TRUE;
  أرجع FALSE;
}

ساكن CURLcode cf_hc_query(بنية Curl_cfilter *cf,
                            بنية Curl_easy *انکڑے,
                            صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  إذا(!cf->connected) {
    بدل(query) {
    حالة CF_QUERY_NEED_FLUSH: {
      لكل(i = 0; i < ctx->baller_گنتی; i++)
        إذا(cf_hc_baller_needs_flush(&ctx->ballers[i], انکڑے)) {
          *pres1 = TRUE;
          أرجع CURLE_OK;
        }
      اكسر;
    }
    افتراضي:
      اكسر;
    }
  }
  أرجع cf->اگلا ?
    cf->اگلا->cft->query(cf->اگلا, انکڑے, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساكن CURLcode cf_hc_cntrl(بنية Curl_cfilter *cf,
                            بنية Curl_easy *انکڑے,
                            صحيح event, صحيح arg1, فراغ *arg2)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;
  size_t i;

  إذا(!cf->connected) {
    بدل(event) {
    حالة CF_CTRL_REPORT_STATS:
    حالة CF_CTRL_CONN_INFO_UPDATE:
      لكل(i = 0; i < ctx->baller_گنتی; i++) {
        /* Make the first baller that connected at network level report */
        إذا(Curl_conn_cf_is_ip_connected(ctx->ballers[i].cf, انکڑے)) {
          Curl_conn_cf_cntrl(ctx->ballers[i].cf, انکڑے, TRUE,
                             event, arg1, arg2);
          اكسر;
        }
      }
      اكسر;
    افتراضي:
      لكل(i = 0; i < ctx->baller_گنتی; i++) {
        نتیجہ = cf_hc_baller_cntrl(&ctx->ballers[i], انکڑے, event, arg1, arg2);
        إذا(نتیجہ && (نتیجہ != CURLE_AGAIN))
          اذهب باہرلا;
      }
      نتیجہ = CURLE_OK;
      اكسر;
    }
  }
باہرلا:
  أرجع نتیجہ;
}

ساكن فراغ cf_hc_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *انکڑے)
{
  بنية cf_hc_ctx *ctx = cf->ctx;

  CURL_TRC_CF(انکڑے, cf, "destroy");
  cf_hc_ctx_destroy(انکڑے, ctx);
}

بنية Curl_cftype Curl_cft_http_connect = {
  "HTTPS-CONNECT",
  CF_TYPE_تیاری,
  CURL_LOG_LVL_NONE,
  cf_hc_destroy,
  cf_hc_connect,
  cf_hc_shutdown,
  cf_hc_adjust_pollset,
  cf_hc_انکڑے_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  cf_hc_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_hc_query,
};

ساكن CURLcode cf_hc_create(بنية Curl_cfilter **pcf,
                             بنية Curl_easy *انکڑے,
                             بنية Curl_peer *destination,
                             uint8_t def_transport)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_hc_ctx *ctx;
  CURLcode نتیجہ = CURLE_OK;

  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }
  Curl_peer_link(&ctx->destination, destination);
  ctx->def_transport = def_transport;
  ctx->hard_eyeballs_timeout_ms = انکڑے->سیٹ.happy_eyeballs_timeout;
  ctx->soft_eyeballs_timeout_ms = انکڑے->سیٹ.happy_eyeballs_timeout / 2;

  نتیجہ = Curl_cf_create(&cf, &Curl_cft_http_connect, ctx);
  إذا(نتیجہ)
    اذهب باہرلا;
  ctx = عدم;

باہرلا:
  *pcf = نتیجہ ? عدم : cf;
  cf_hc_ctx_destroy(انکڑے, ctx);
  أرجع نتیجہ;
}

ساكن CURLcode cf_hc_جوڑو(بنية Curl_easy *انکڑے,
                          بنية Curl_peer *destination,
                          بنية connectdata *conn,
                          int8_t sockindex,
                          uint8_t def_transport)
{
  بنية Curl_cfilter *cf;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(انکڑے);
  نتیجہ = cf_hc_create(&cf, انکڑے, destination, def_transport);
  إذا(نتیجہ)
    اذهب باہرلا;
  Curl_conn_cf_جوڑو(انکڑے, conn, sockindex, cf);

#جے_تعریف_شدہ USE_HTTPSRR
  نتیجہ = Curl_conn_dns_جوڑو_https_resolve(انکڑے, cf->conn, cf->sockindex,
                                           destination);
#جے_انت
باہرلا:
  أرجع نتیجہ;
}

CURLcode Curl_cf_https_تیاری(بنية Curl_easy *انکڑے,
                             بنية Curl_peer *destination,
                             بنية connectdata *conn,
                             int8_t sockindex)
{
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(conn->scheme->protocol == CURLPROTO_HTTPS);

  /* This filter is intended for HTTPS using ALPN and does
   * not support HTTPS Eyeballing to a proxy. */
  إذا((conn->scheme->protocol != CURLPROTO_HTTPS) ||
#جے_بنا_تعریف CURL_DISABLE_PROXY
     conn->بٹ.origin_is_proxy ||
#جے_انت
     !conn->بٹ.tls_enable_alpn)
    اذهب باہرلا;

  نتیجہ = cf_hc_جوڑو(انکڑے, destination, conn, sockindex,
                     conn->transport_wanted);

باہرلا:
  أرجع نتیجہ;
}

#جے_انت /* !CURL_DISABLE_HTTP */
