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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-setup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cf-https-connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http2.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/cf-dns.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/hostip.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/httpsrr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/vquic.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  CF_HC_RESOLV,
  CF_HC_INIT,
  CF_HC_CONNECT,
  CF_HC_SUCCESS,
  CF_HC_FAILURE
} cf_hc_state;

ਬਣਤਰ cf_hc_baller {
  ਅਚਲ ਅੱਖਰ *name;
  ਬਣਤਰ Curl_cfilter *cf;
  CURLcode ਨਤੀਜਾ;
  ਪੂਰਨ ਜਵਾਬ_ms;
  uint8_t transport;
  ਗਿਣਤੀ alpnid alpn_id;
  ਦੋਆਧਾਰੀ_ਅੰਕ(shutdown);
};

ਸਥਿਰ ਖਾਲੀ cf_hc_baller_discard(ਬਣਤਰ cf_hc_baller *b,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(b->cf) {
    Curl_conn_cf_discard_chain(&b->cf, ਅੰਕੜੇ);
    b->cf = ਨਿਰਾ;
  }
}

ਸਥਿਰ bool cf_hc_baller_is_connecting(ਬਣਤਰ cf_hc_baller *b)
{
  ਵਾਪਸ b->cf && !b->ਨਤੀਜਾ;
}

ਸਥਿਰ bool cf_hc_baller_has_started(ਬਣਤਰ cf_hc_baller *b)
{
  ਵਾਪਸ !!b->cf;
}

ਸਥਿਰ ਪੂਰਨ cf_hc_baller_ਜਵਾਬ_ms(ਬਣਤਰ cf_hc_baller *b,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(b->cf && (b->ਜਵਾਬ_ms < 0))
    b->cf->cft->query(b->cf, ਅੰਕੜੇ, CF_QUERY_CONNECT_ਜਵਾਬ_MS,
                      &b->ਜਵਾਬ_ms, ਨਿਰਾ);
  ਵਾਪਸ b->ਜਵਾਬ_ms;
}

ਸਥਿਰ bool cf_hc_baller_ਅੰਕੜੇ_pending(ਬਣਤਰ cf_hc_baller *b,
                                      ਅਚਲ ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਵਾਪਸ b->cf && !b->ਨਤੀਜਾ && b->cf->cft->has_ਅੰਕੜੇ_pending(b->cf, ਅੰਕੜੇ);
}

ਸਥਿਰ bool cf_hc_baller_needs_flush(ਬਣਤਰ cf_hc_baller *b,
                                     ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਵਾਪਸ b->cf && !b->ਨਤੀਜਾ && Curl_conn_cf_needs_flush(b->cf, ਅੰਕੜੇ);
}

ਸਥਿਰ CURLcode cf_hc_baller_cntrl(ਬਣਤਰ cf_hc_baller *b,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਪੂਰਨ event, ਪੂਰਨ arg1, ਖਾਲੀ *arg2)
{
  ਜੇ(b->cf && !b->ਨਤੀਜਾ)
    ਵਾਪਸ Curl_conn_cf_cntrl(b->cf, ਅੰਕੜੇ, FALSE, event, arg1, arg2);
  ਵਾਪਸ CURLE_OK;
}

ਬਣਤਰ cf_hc_ctx {
  cf_hc_state state;
  ਬਣਤਰ Curl_peer *destination; /* who we ultimately want to talk to */
  ਬਣਤਰ curltime started;  /* when connect started */
  CURLcode ਨਤੀਜਾ;          /* overall result */
  CURLcode check_h3_ਨਤੀਜਾ;
  ਬਣਤਰ cf_hc_baller ballers[2];
  size_t baller_ਗਿਣਤੀ;
  timediff_t soft_eyeballs_timeout_ms;
  timediff_t hard_eyeballs_timeout_ms;
  uint8_t def_transport;
  ਦੋਆਧਾਰੀ_ਅੰਕ(httpsrr_resolved);
  ਦੋਆਧਾਰੀ_ਅੰਕ(checked_h3);
  ਦੋਆਧਾਰੀ_ਅੰਕ(ballers_complete);
};

ਸਥਿਰ ਖਾਲੀ cf_hc_ctx_destroy(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ cf_hc_ctx *ctx)
{
  ਜੇ(ctx) {
    size_t i;
    ਲਈ(i = 0; i < ctx->baller_ਗਿਣਤੀ; ++i)
      cf_hc_baller_discard(&ctx->ballers[i], ਅੰਕੜੇ);
    Curl_peer_unlink(&ctx->destination);
    curlx_free(ctx);
  }
}

ਸਥਿਰ ਖਾਲੀ cf_hc_baller_ਸੌਂਪੋ(ਬਣਤਰ cf_hc_baller *b,
                                ਗਿਣਤੀ alpnid alpn_id,
                                uint8_t def_transport)
{
  b->alpn_id = alpn_id;
  b->transport = def_transport;
  b->cf = ਨਿਰਾ;
  b->ਨਤੀਜਾ = CURLE_OK;
  b->ਜਵਾਬ_ms = -1;
  b->shutdown = FALSE;
  ਚੋਣ(b->alpn_id) {
  ਹਾਲਤ ALPN_h3:
    b->name = "h3";
    b->transport = TRNSPRT_QUIC;
    ਰੋਕੋ;
  ਹਾਲਤ ALPN_h2:
    b->name = "h2";
    ਰੋਕੋ;
  ਹਾਲਤ ALPN_h1:
    b->name = "h1";
    ਰੋਕੋ;
  ਹਾਲਤ ALPN_none:
    b->name = "no-alpn";
    ਰੋਕੋ;
  ਮੂਲ:
    b->ਨਤੀਜਾ = CURLE_FAILED_INIT;
    ਰੋਕੋ;
  }
}

ਸਥਿਰ ਖਾਲੀ cf_hc_baller_init(ਬਣਤਰ cf_hc_baller *b,
                              ਬਣਤਰ Curl_cfilter *cf,
                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *save = cf->ਅਗਲਾ;

  cf->ਅਗਲਾ = ਨਿਰਾ;
  b->ਨਤੀਜਾ = Curl_cf_ਤਿਆਰੀ_ਪਾਓ_after(cf, ਅੰਕੜੇ, b->transport,
                                         CURL_CF_SSL_ENABLE);
  b->cf = cf->ਅਗਲਾ;
  cf->ਅਗਲਾ = save;
}

ਸਥਿਰ CURLcode cf_hc_baller_connect(ਬਣਤਰ cf_hc_baller *b,
                                     ਬਣਤਰ Curl_cfilter *cf,
                                     ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     bool *ਪੂਰਾ)
{
  ਬਣਤਰ Curl_cfilter *save = cf->ਅਗਲਾ;

  cf->ਅਗਲਾ = b->cf;
  b->ਨਤੀਜਾ = Curl_conn_cf_connect(cf->ਅਗਲਾ, ਅੰਕੜੇ, ਪੂਰਾ);
  b->cf = cf->ਅਗਲਾ; /* it might mutate */
  cf->ਅਗਲਾ = save;
  ਵਾਪਸ b->ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode baller_connected(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ cf_hc_baller *winner)
{
  ਬਣਤਰ cf_hc_ctx *ctx = cf->ctx;

  /* Make the winner's connection filter out own sub-filter, check, move,
   * close all remaining. */
  ਜੇ(cf->ਅਗਲਾ) {
    DEBUGASSERT(0);
    ਵਾਪਸ CURLE_FAILED_INIT;
  }
  ਜੇ(!winner->cf) {
    DEBUGASSERT(0);
    ਵਾਪਸ CURLE_FAILED_INIT;
  }

  cf->ਅਗਲਾ = winner->cf;
  winner->cf = ਨਿਰਾ;
  ctx->state = CF_HC_SUCCESS;
  cf->connected = TRUE;

  /* ballers may have failf()'d, the winner resets it, so our
   * errorbuf is clean again. */
  Curl_reset_fail(ਅੰਕੜੇ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NGHTTP2
  {
    /* For a negotiated HTTP/2 connection insert the h2 filter. */
    ਅਚਲ ਅੱਖਰ *alpn = Curl_conn_cf_get_alpn_negotiated(cf->ਅਗਲਾ, ਅੰਕੜੇ);
    ਜੇ(alpn && !strcmp("h2", alpn)) {
      CURLcode ਨਤੀਜਾ = Curl_http2_switch_at(cf, ਅੰਕੜੇ);
      ਜੇ(ਨਤੀਜਾ) {
        ctx->state = CF_HC_FAILURE;
        ctx->ਨਤੀਜਾ = ਨਤੀਜਾ;
        ਵਾਪਸ ਨਤੀਜਾ;
      }
    }
  }
#ਜੇ_ਅੰਤ
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ bool ਸਮਾਂ_ਮੰਜ਼ਿਲ_ਸ਼ੁਰੂ_baller2(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਅਚਲ ਬਣਤਰ curltime *pnow)
{
  ਬਣਤਰ cf_hc_ctx *ctx = cf->ctx;
  timediff_t elapsed_ms;

  ਜੇ(ctx->baller_ਗਿਣਤੀ < 2)
    ਵਾਪਸ FALSE;
  ਨਹੀਂਤਾਂ ਜੇ(cf_hc_baller_has_started(&ctx->ballers[1]))
    ਵਾਪਸ FALSE;
  ਨਹੀਂਤਾਂ ਜੇ(ctx->ballers[0].ਨਤੀਜਾ) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "%s baller failed, starting %s",
                ctx->ballers[0].name, ctx->ballers[1].name);
    ਵਾਪਸ TRUE;
  }

  elapsed_ms = curlx_ptimediff_ms(pnow, &ctx->started);
  ਜੇ(elapsed_ms >= ctx->hard_eyeballs_timeout_ms) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "%s inconclusive after %" FMT_TIMEDIFF_T ", "
                "starting %s", ctx->ballers[0].name,
                ctx->hard_eyeballs_timeout_ms, ctx->ballers[1].name);
    ਵਾਪਸ TRUE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(elapsed_ms >= ctx->soft_eyeballs_timeout_ms) {
    ਜੇ(cf_hc_baller_ਜਵਾਬ_ms(&ctx->ballers[0], ਅੰਕੜੇ) < 0) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "%s has not seen any data after %"
                  FMT_TIMEDIFF_T "ms, starting %s",
                  ctx->ballers[0].name, ctx->soft_eyeballs_timeout_ms,
                  ctx->ballers[1].name);
      ਵਾਪਸ TRUE;
    }
    /* set the effective hard timeout again */
    Curl_expire_ਸੈੱਟ(ਅੰਕੜੇ, EXPIRE_ALPN_EYEBALLS,
                    ctx->hard_eyeballs_timeout_ms - elapsed_ms, pnow);
  }
  ਵਾਪਸ FALSE;
}

ਸਥਿਰ bool cf_hc_may_h3(ਬਣਤਰ Curl_cfilter *cf,
                         ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_hc_ctx *ctx = cf->ctx;
  ਜੇ(!ctx->checked_h3) {
    ctx->check_h3_ਨਤੀਜਾ =
      Curl_conn_may_http3(ਅੰਕੜੇ, cf->conn);
    ctx->checked_h3 = TRUE;
  }
  ਵਾਪਸ !ctx->check_h3_ਨਤੀਜਾ;
}

ਸਥਿਰ ਗਿਣਤੀ alpnid cf_hc_get_httpsrr_alpn(ਬਣਤਰ Curl_cfilter *cf,
                                          ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਗਿਣਤੀ alpnid not_this_one)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
  ਬਣਤਰ cf_hc_ctx *ctx = cf->ctx;
  /* Is there an HTTPSRR use its ALPNs here.
   * We are here after having selected a connection to a host+port and
   * can no longer change that. Any HTTPSRR advice for other hosts and ports
   * we need to ignore. */
  ਅਚਲ ਬਣਤਰ Curl_https_rrinfo *rr;
  size_t i;

  /* Do we have HTTPS-RR information? */
  rr = Curl_conn_dns_get_https(ਅੰਕੜੇ, cf->sockindex, ctx->destination);

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "HTTPS-RR %savailable", rr ? "" : "not ");
  /* We do not support `rr->no_def_alpn`. */
  ਜੇ(Curl_httpsrr_applicable(ਅੰਕੜੇ, rr) && !rr->no_def_alpn) {
    ਲਈ(i = 0; i < CURL_ARRAYSIZE(rr->alpns); ++i) {
      ਗਿਣਤੀ alpnid alpn_rr = (ਗਿਣਤੀ alpnid)rr->alpns[i];
      ਜੇ(alpn_rr == not_this_one) /* do not want this one */
        ਜਾਰੀ;
      ਚੋਣ(alpn_rr) {
      ਹਾਲਤ ALPN_h3:
        ਜੇ((ਅੰਕੜੇ->state.http_neg.allowed & CURL_HTTP_V3x) &&
           cf_hc_may_h3(cf, ਅੰਕੜੇ)) {
          ਵਾਪਸ alpn_rr;
        }
        ਰੋਕੋ;
      ਹਾਲਤ ALPN_h2:
        ਜੇ(ਅੰਕੜੇ->state.http_neg.allowed & CURL_HTTP_V2x) {
          ਵਾਪਸ alpn_rr;
        }
        ਰੋਕੋ;
      ਹਾਲਤ ALPN_h1:
        ਜੇ(ਅੰਕੜੇ->state.http_neg.allowed & CURL_HTTP_V1x) {
          ਵਾਪਸ alpn_rr;
        }
        ਰੋਕੋ;
      ਮੂਲ: /* ignore */
        ਰੋਕੋ;
      }
    }
  }
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)cf;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)not_this_one;
#ਜੇ_ਅੰਤ
  ਵਾਪਸ ALPN_none;
}

ਸਥਿਰ ਗਿਣਤੀ alpnid cf_hc_get_pref_alpn(ਬਣਤਰ Curl_cfilter *cf,
                                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਗਿਣਤੀ alpnid not_this_one)
{
  ਜੇ((ਅੰਕੜੇ->state.http_neg.preferred & ਅੰਕੜੇ->state.http_neg.allowed)) {
    ਚੋਣ(ਅੰਕੜੇ->state.http_neg.preferred) {
    ਹਾਲਤ CURL_HTTP_V3x:
      ਜੇ(cf_hc_may_h3(cf, ਅੰਕੜੇ) && (ALPN_h3 != not_this_one))
        ਵਾਪਸ ALPN_h3;
      ਰੋਕੋ;
    ਹਾਲਤ CURL_HTTP_V2x:
      ਜੇ(ALPN_h2 != not_this_one)
        ਵਾਪਸ ALPN_h2;
      ਰੋਕੋ;
    ਹਾਲਤ CURL_HTTP_V1x:
      /* If we are trying h2 already, h1 is already used as fallback */
      ਜੇ((ALPN_h1 != not_this_one) && (ALPN_h2 != not_this_one))
        ਵਾਪਸ ALPN_h1;
      ਰੋਕੋ;
    ਮੂਲ:
      ਰੋਕੋ;
    }
  }
  ਵਾਪਸ ALPN_none;
}

ਸਥਿਰ ਗਿਣਤੀ alpnid cf_hc_get_ਪਹਿਲਾ_alpn(ਬਣਤਰ Curl_cfilter *cf,
                                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        http_majors choices,
                                        ਗਿਣਤੀ alpnid not_this_one)
{
  /* When told to not try h2, we also do not try h1 and vice versa */
  bool allow_h1_or_h2 = (not_this_one != ALPN_h1) &&
                        (not_this_one != ALPN_h2);
  ਜੇ((ALPN_h3 != not_this_one) && (choices & CURL_HTTP_V3x) &&
     cf_hc_may_h3(cf, ਅੰਕੜੇ)) {
    ਵਾਪਸ ALPN_h3;
  }
  ਜੇ(allow_h1_or_h2 && (choices & CURL_HTTP_V2x)) {
    ਵਾਪਸ ALPN_h2;
  }
  ਜੇ(allow_h1_or_h2 && (choices & CURL_HTTP_V1x)) {
    ਵਾਪਸ ALPN_h1;
  }
  ਵਾਪਸ ALPN_none;
}

ਸਥਿਰ CURLcode cf_hc_ਸੈੱਟ_baller1(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_hc_ctx *ctx = cf->ctx;
  ਗਿਣਤੀ alpnid alpn1 = ALPN_none;
  VERBOSE(ਅਚਲ ਅੱਖਰ *ਸਰੋਤ = "HTTPS-RR");

  DEBUGASSERT(cf->conn->ਬਿੱਟ.tls_enable_alpn);

  alpn1 = cf_hc_get_httpsrr_alpn(cf, ਅੰਕੜੇ, ALPN_none);
  ਜੇ(alpn1 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(ਸਰੋਤ = "preferred version");
    alpn1 = cf_hc_get_pref_alpn(cf, ਅੰਕੜੇ, ALPN_none);
  }
  ਜੇ(alpn1 == ALPN_none) {
    VERBOSE(ਸਰੋਤ = "wanted versions");
    alpn1 = cf_hc_get_ਪਹਿਲਾ_alpn(cf, ਅੰਕੜੇ,
                                 ਅੰਕੜੇ->state.http_neg.wanted,
                                 ALPN_none);
  }
  ਜੇ(alpn1 == ALPN_none) {
    VERBOSE(ਸਰੋਤ = "allowed versions");
    alpn1 = cf_hc_get_ਪਹਿਲਾ_alpn(cf, ਅੰਕੜੇ,
                                 ਅੰਕੜੇ->state.http_neg.allowed,
                                 ALPN_none);
  }

  ਜੇ(alpn1 == ALPN_none) {
    /* None of the wanted/allowed HTTP versions could be chosen */
    ਜੇ(ctx->check_h3_ਨਤੀਜਾ) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "unable to use HTTP/3");
      ਵਾਪਸ ctx->check_h3_ਨਤੀਜਾ;
    }
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "unable to select HTTP version");
    ਵਾਪਸ CURLE_FAILED_INIT;
  }

  cf_hc_baller_ਸੌਂਪੋ(&ctx->ballers[0], alpn1, ctx->def_transport);
  ctx->baller_ਗਿਣਤੀ = 1;
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "1st attempt uses %s from %s",
              ctx->ballers[0].name, ਸਰੋਤ);

  ਚੋਣ(alpn1) {
  ਹਾਲਤ ALPN_h1:
    /* We really want h1, switch off h2 to make it disappear in ALPN */
    ਅੰਕੜੇ->state.http_neg.wanted &= (uint8_t)~CURL_HTTP_V2x;
    ਰੋਕੋ;
  ਮੂਲ:
    ਰੋਕੋ;
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ cf_hc_ਸੈੱਟ_baller2(ਬਣਤਰ Curl_cfilter *cf,
                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_hc_ctx *ctx = cf->ctx;
  ਗਿਣਤੀ alpnid alpn2 = ALPN_none, alpn1 = ctx->ballers[0].alpn_id;
  VERBOSE(ਅਚਲ ਅੱਖਰ *ਸਰੋਤ = "HTTPS-RR");

  ਜੇ(ctx->ballers_complete)
    ਵਾਪਸ; /* already done */
  ਜੇ(!ctx->httpsrr_resolved)
    ਵਾਪਸ; /* HTTPS-RR pending */

  alpn2 = cf_hc_get_httpsrr_alpn(cf, ਅੰਕੜੇ, alpn1);
  ਜੇ(alpn2 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(ਸਰੋਤ = "preferred version");
    alpn2 = cf_hc_get_pref_alpn(cf, ਅੰਕੜੇ, alpn1);
  }
  ਜੇ(alpn2 == ALPN_none) {
    VERBOSE(ਸਰੋਤ = "wanted versions");
    alpn2 = cf_hc_get_ਪਹਿਲਾ_alpn(cf, ਅੰਕੜੇ,
                                 ਅੰਕੜੇ->state.http_neg.wanted,
                                 alpn1);
  }

  ਜੇ(alpn2 != ALPN_none) {
    cf_hc_baller_ਸੌਂਪੋ(&ctx->ballers[1], alpn2, ctx->def_transport);
    ctx->baller_ਗਿਣਤੀ = 2;
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "2nd attempt uses %s from %s",
                ctx->ballers[1].name, ਸਰੋਤ);
  }
  ctx->ballers_complete = TRUE;
}

ਸਥਿਰ CURLcode cf_hc_connect(ਬਣਤਰ Curl_cfilter *cf,
                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              bool *ਪੂਰਾ)
{
  ਬਣਤਰ cf_hc_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਬਣਤਰ curltime *pnow = ਨਿਰਾ;

  ਜੇ(cf->connected) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  *ਪੂਰਾ = FALSE;

  ਜੇ(!ctx->httpsrr_resolved) {
    ctx->httpsrr_resolved = Curl_conn_dns_resolved_https(
      ਅੰਕੜੇ, cf->sockindex, ctx->destination);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
    ਜੇ(!ctx->httpsrr_resolved && getenv("CURL_DBG_AWAIT_HTTPSRR")) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "awaiting HTTPS-RR");
      ਵਾਪਸ CURLE_OK;
    }
#ਜੇ_ਅੰਤ
  }

  ਚੋਣ(ctx->state) {
  ਹਾਲਤ CF_HC_RESOLV:
    ctx->state = CF_HC_INIT;
    FALLTHROUGH();

  ਹਾਲਤ CF_HC_INIT:
    DEBUGASSERT(!cf->ਅਗਲਾ);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "connect, init");
    ਨਤੀਜਾ = cf_hc_ਸੈੱਟ_baller1(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ) {
      ctx->ਨਤੀਜਾ = ਨਤੀਜਾ;
      ctx->state = CF_HC_FAILURE;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    cf_hc_ਸੈੱਟ_baller2(cf, ਅੰਕੜੇ);
    pnow = Curl_pgrs_now(ਅੰਕੜੇ);
    ctx->started = *pnow;
    cf_hc_baller_init(&ctx->ballers[0], cf, ਅੰਕੜੇ);
    ਜੇ((ctx->baller_ਗਿਣਤੀ > 1) || !ctx->ballers_complete) {
      Curl_expire_ਸੈੱਟ(ਅੰਕੜੇ, EXPIRE_ALPN_EYEBALLS,
                      ctx->soft_eyeballs_timeout_ms, pnow);
    }
    ctx->state = CF_HC_CONNECT;
    FALLTHROUGH();

  ਹਾਲਤ CF_HC_CONNECT:
    ਜੇ(!ctx->ballers_complete)
      cf_hc_ਸੈੱਟ_baller2(cf, ਅੰਕੜੇ);

    ਜੇ(cf_hc_baller_is_connecting(&ctx->ballers[0])) {
      ਨਤੀਜਾ = cf_hc_baller_connect(&ctx->ballers[0], cf, ਅੰਕੜੇ, ਪੂਰਾ);
      ਜੇ(!ਨਤੀਜਾ && *ਪੂਰਾ) {
        ਨਤੀਜਾ = baller_connected(cf, ਅੰਕੜੇ, &ctx->ballers[0]);
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }

    pnow = Curl_pgrs_now(ਅੰਕੜੇ);
    ਜੇ(ਸਮਾਂ_ਮੰਜ਼ਿਲ_ਸ਼ੁਰੂ_baller2(cf, ਅੰਕੜੇ, pnow)) {
      cf_hc_baller_init(&ctx->ballers[1], cf, ਅੰਕੜੇ);
    }

    ਜੇ(cf_hc_baller_is_connecting(&ctx->ballers[1])) {
      ਨਤੀਜਾ = cf_hc_baller_connect(&ctx->ballers[1], cf, ਅੰਕੜੇ, ਪੂਰਾ);
      ਜੇ(!ਨਤੀਜਾ && *ਪੂਰਾ) {
        ਨਤੀਜਾ = baller_connected(cf, ਅੰਕੜੇ, &ctx->ballers[1]);
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }

    ਜੇ(ctx->ballers[0].ਨਤੀਜਾ &&
       (ctx->ballers[1].ਨਤੀਜਾ ||
        (ctx->ballers_complete && (ctx->baller_ਗਿਣਤੀ < 2)))) {
      /* all have failed. we give up */
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "connect, all attempts failed");
      ctx->ਨਤੀਜਾ = ਨਤੀਜਾ = ctx->ballers[0].ਨਤੀਜਾ;
      ctx->state = CF_HC_FAILURE;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਤੀਜਾ = CURLE_OK;
    *ਪੂਰਾ = FALSE;
    ਰੋਕੋ;

  ਹਾਲਤ CF_HC_FAILURE:
    ਨਤੀਜਾ = ctx->ਨਤੀਜਾ;
    cf->connected = FALSE;
    *ਪੂਰਾ = FALSE;
    ਰੋਕੋ;

  ਹਾਲਤ CF_HC_SUCCESS:
    ਨਤੀਜਾ = CURLE_OK;
    cf->connected = TRUE;
    *ਪੂਰਾ = TRUE;
    ਰੋਕੋ;
  }

ਬਾਹਰਲਾ:
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "connect -> %d, done=%d", (ਪੂਰਨ)ਨਤੀਜਾ, *ਪੂਰਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_hc_shutdown(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  ਬਣਤਰ cf_hc_ctx *ctx = cf->ctx;
  size_t i;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(ਅੰਕੜੇ);
  ਜੇ(cf->connected) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  ਲਈ(i = 0; i < ctx->baller_ਗਿਣਤੀ; i++) {
    ਬਣਤਰ cf_hc_baller *b = &ctx->ballers[i];
    bool bdone = FALSE;
    ਜੇ(!cf_hc_baller_is_connecting(b) || b->shutdown)
      ਜਾਰੀ;
    b->ਨਤੀਜਾ = b->cf->cft->do_shutdown(b->cf, ਅੰਕੜੇ, &bdone);
    ਜੇ(b->ਨਤੀਜਾ || bdone)
      b->shutdown = TRUE; /* treat a failed shutdown as done */
  }

  *ਪੂਰਾ = TRUE;
  ਲਈ(i = 0; i < ctx->baller_ਗਿਣਤੀ; i++) {
    ਜੇ(!ctx->ballers[i].shutdown)
      *ਪੂਰਾ = FALSE;
  }
  ਜੇ(*ਪੂਰਾ) {
    ਲਈ(i = 0; i < ctx->baller_ਗਿਣਤੀ; i++) {
      ਜੇ(ctx->ballers[i].ਨਤੀਜਾ)
        ਨਤੀਜਾ = ctx->ballers[i].ਨਤੀਜਾ;
    }
  }
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "shutdown -> %d, done=%d", (ਪੂਰਨ)ਨਤੀਜਾ, *ਪੂਰਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_hc_adjust_pollset(ਬਣਤਰ Curl_cfilter *cf,
                                     ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ easy_pollset *ps)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਜੇ(!cf->connected) {
    ਬਣਤਰ cf_hc_ctx *ctx = cf->ctx;
    size_t i;

    ਲਈ(i = 0; (i < ctx->baller_ਗਿਣਤੀ) && !ਨਤੀਜਾ; i++) {
      ਬਣਤਰ cf_hc_baller *b = &ctx->ballers[i];
      ਜੇ(!cf_hc_baller_is_connecting(b))
        ਜਾਰੀ;
      ਨਤੀਜਾ = Curl_conn_cf_adjust_pollset(b->cf, ਅੰਕੜੇ, ps);
    }
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "adjust_pollset -> %d, %u socks", (ਪੂਰਨ)ਨਤੀਜਾ,
                ps->n);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ bool cf_hc_ਅੰਕੜੇ_pending(ਬਣਤਰ Curl_cfilter *cf,
                               ਅਚਲ ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  ਜੇ(cf->connected)
    ਵਾਪਸ cf->ਅਗਲਾ->cft->has_ਅੰਕੜੇ_pending(cf->ਅਗਲਾ, ਅੰਕੜੇ);

  ਲਈ(i = 0; i < ctx->baller_ਗਿਣਤੀ; i++)
    ਜੇ(cf_hc_baller_ਅੰਕੜੇ_pending(&ctx->ballers[i], ਅੰਕੜੇ))
      ਵਾਪਸ TRUE;
  ਵਾਪਸ FALSE;
}

ਸਥਿਰ CURLcode cf_hc_query(ਬਣਤਰ Curl_cfilter *cf,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਪੂਰਨ query, ਪੂਰਨ *pres1, ਖਾਲੀ *pres2)
{
  ਬਣਤਰ cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  ਜੇ(!cf->connected) {
    ਚੋਣ(query) {
    ਹਾਲਤ CF_QUERY_NEED_FLUSH: {
      ਲਈ(i = 0; i < ctx->baller_ਗਿਣਤੀ; i++)
        ਜੇ(cf_hc_baller_needs_flush(&ctx->ballers[i], ਅੰਕੜੇ)) {
          *pres1 = TRUE;
          ਵਾਪਸ CURLE_OK;
        }
      ਰੋਕੋ;
    }
    ਮੂਲ:
      ਰੋਕੋ;
    }
  }
  ਵਾਪਸ cf->ਅਗਲਾ ?
    cf->ਅਗਲਾ->cft->query(cf->ਅਗਲਾ, ਅੰਕੜੇ, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ਸਥਿਰ CURLcode cf_hc_cntrl(ਬਣਤਰ Curl_cfilter *cf,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਪੂਰਨ event, ਪੂਰਨ arg1, ਖਾਲੀ *arg2)
{
  ਬਣਤਰ cf_hc_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t i;

  ਜੇ(!cf->connected) {
    ਚੋਣ(event) {
    ਹਾਲਤ CF_CTRL_REPORT_STATS:
    ਹਾਲਤ CF_CTRL_CONN_INFO_UPDATE:
      ਲਈ(i = 0; i < ctx->baller_ਗਿਣਤੀ; i++) {
        /* Make the first baller that connected at network level report */
        ਜੇ(Curl_conn_cf_is_ip_connected(ctx->ballers[i].cf, ਅੰਕੜੇ)) {
          Curl_conn_cf_cntrl(ctx->ballers[i].cf, ਅੰਕੜੇ, TRUE,
                             event, arg1, arg2);
          ਰੋਕੋ;
        }
      }
      ਰੋਕੋ;
    ਮੂਲ:
      ਲਈ(i = 0; i < ctx->baller_ਗਿਣਤੀ; i++) {
        ਨਤੀਜਾ = cf_hc_baller_cntrl(&ctx->ballers[i], ਅੰਕੜੇ, event, arg1, arg2);
        ਜੇ(ਨਤੀਜਾ && (ਨਤੀਜਾ != CURLE_AGAIN))
          ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਨਤੀਜਾ = CURLE_OK;
      ਰੋਕੋ;
    }
  }
ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ cf_hc_destroy(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_hc_ctx *ctx = cf->ctx;

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "destroy");
  cf_hc_ctx_destroy(ਅੰਕੜੇ, ctx);
}

ਬਣਤਰ Curl_cftype Curl_cft_http_connect = {
  "HTTPS-CONNECT",
  CF_TYPE_ਤਿਆਰੀ,
  CURL_LOG_LVL_NONE,
  cf_hc_destroy,
  cf_hc_connect,
  cf_hc_shutdown,
  cf_hc_adjust_pollset,
  cf_hc_ਅੰਕੜੇ_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  cf_hc_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_hc_query,
};

ਸਥਿਰ CURLcode cf_hc_create(ਬਣਤਰ Curl_cfilter **pcf,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ Curl_peer *destination,
                             uint8_t def_transport)
{
  ਬਣਤਰ Curl_cfilter *cf = ਨਿਰਾ;
  ਬਣਤਰ cf_hc_ctx *ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ctx = curlx_calloc(1, ਆਕਾਰ(*ctx));
  ਜੇ(!ctx) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  Curl_peer_link(&ctx->destination, destination);
  ctx->def_transport = def_transport;
  ctx->hard_eyeballs_timeout_ms = ਅੰਕੜੇ->ਸੈੱਟ.happy_eyeballs_timeout;
  ctx->soft_eyeballs_timeout_ms = ਅੰਕੜੇ->ਸੈੱਟ.happy_eyeballs_timeout / 2;

  ਨਤੀਜਾ = Curl_cf_create(&cf, &Curl_cft_http_connect, ctx);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  ctx = ਨਿਰਾ;

ਬਾਹਰਲਾ:
  *pcf = ਨਤੀਜਾ ? ਨਿਰਾ : cf;
  cf_hc_ctx_destroy(ਅੰਕੜੇ, ctx);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_hc_ਜੋੜੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਬਣਤਰ Curl_peer *destination,
                          ਬਣਤਰ connectdata *conn,
                          int8_t sockindex,
                          uint8_t def_transport)
{
  ਬਣਤਰ Curl_cfilter *cf;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(ਅੰਕੜੇ);
  ਨਤੀਜਾ = cf_hc_create(&cf, ਅੰਕੜੇ, destination, def_transport);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  Curl_conn_cf_ਜੋੜੋ(ਅੰਕੜੇ, conn, sockindex, cf);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
  ਨਤੀਜਾ = Curl_conn_dns_ਜੋੜੋ_https_resolve(ਅੰਕੜੇ, cf->conn, cf->sockindex,
                                           destination);
#ਜੇ_ਅੰਤ
ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_cf_https_ਤਿਆਰੀ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ Curl_peer *destination,
                             ਬਣਤਰ connectdata *conn,
                             int8_t sockindex)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(conn->scheme->protocol == CURLPROTO_HTTPS);

  /* This filter is intended for HTTPS using ALPN and does
   * not support HTTPS Eyeballing to a proxy. */
  ਜੇ((conn->scheme->protocol != CURLPROTO_HTTPS) ||
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
     conn->ਬਿੱਟ.origin_is_proxy ||
#ਜੇ_ਅੰਤ
     !conn->ਬਿੱਟ.tls_enable_alpn)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਨਤੀਜਾ = cf_hc_ਜੋੜੋ(ਅੰਕੜੇ, destination, conn, sockindex,
                     conn->transport_wanted);

ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP */
