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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "cf-setup.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "cf-https-connect.h"
#ಅನ್ತರ್ಭಾವ "http2.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "vdns/cf-dns.h"
#ಅನ್ತರ್ಭಾವ "vdns/hostip.h"
#ಅನ್ತರ್ಭಾವ "vdns/httpsrr.h"
#ಅನ್ತರ್ಭಾವ "vquic/vquic.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  CF_HC_RESOLV,
  CF_HC_INIT,
  CF_HC_CONNECT,
  CF_HC_SUCCESS,
  CF_HC_FAILURE
} cf_hc_state;

ರಚನೆ cf_hc_baller {
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
  ರಚನೆ Curl_cfilter *cf;
  CURLcode ಫಲಿತಾಂಶ;
  ಪೂರ್ಣಾಂಕ ಉತ್ತರ_ms;
  uint8_t transport;
  ಎಣಿಕೆ alpnid alpn_id;
  ಬಿಟ್(shutdown);
};

ಸ್ಥಿರ ಶೂನ್ಯ cf_hc_baller_discard(ರಚನೆ cf_hc_baller *b,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(b->cf) {
    Curl_conn_cf_discard_chain(&b->cf, ದತ್ತಾಂಶ);
    b->cf = ಖಾಲಿ;
  }
}

ಸ್ಥಿರ bool cf_hc_baller_is_connecting(ರಚನೆ cf_hc_baller *b)
{
  ಹಿಂತಿರುಗಿ b->cf && !b->ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ bool cf_hc_baller_has_started(ರಚನೆ cf_hc_baller *b)
{
  ಹಿಂತಿರುಗಿ !!b->cf;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cf_hc_baller_ಉತ್ತರ_ms(ರಚನೆ cf_hc_baller *b,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(b->cf && (b->ಉತ್ತರ_ms < 0))
    b->cf->cft->query(b->cf, ದತ್ತಾಂಶ, CF_QUERY_CONNECT_ಉತ್ತರ_MS,
                      &b->ಉತ್ತರ_ms, ಖಾಲಿ);
  ಹಿಂತಿರುಗಿ b->ಉತ್ತರ_ms;
}

ಸ್ಥಿರ bool cf_hc_baller_ದತ್ತಾಂಶ_pending(ರಚನೆ cf_hc_baller *b,
                                      ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಹಿಂತಿರುಗಿ b->cf && !b->ಫಲಿತಾಂಶ && b->cf->cft->has_ದತ್ತಾಂಶ_pending(b->cf, ದತ್ತಾಂಶ);
}

ಸ್ಥಿರ bool cf_hc_baller_needs_flush(ರಚನೆ cf_hc_baller *b,
                                     ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಹಿಂತಿರುಗಿ b->cf && !b->ಫಲಿತಾಂಶ && Curl_conn_cf_needs_flush(b->cf, ದತ್ತಾಂಶ);
}

ಸ್ಥಿರ CURLcode cf_hc_baller_cntrl(ರಚನೆ cf_hc_baller *b,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ಪೂರ್ಣಾಂಕ event, ಪೂರ್ಣಾಂಕ arg1, ಶೂನ್ಯ *arg2)
{
  ಆದರೆ(b->cf && !b->ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ Curl_conn_cf_cntrl(b->cf, ದತ್ತಾಂಶ, FALSE, event, arg1, arg2);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ರಚನೆ cf_hc_ctx {
  cf_hc_state state;
  ರಚನೆ Curl_peer *destination; /* who we ultimately want to talk to */
  ರಚನೆ curltime started;  /* when connect started */
  CURLcode ಫಲಿತಾಂಶ;          /* overall result */
  CURLcode check_h3_ಫಲಿತಾಂಶ;
  ರಚನೆ cf_hc_baller ballers[2];
  size_t baller_ಎಣಿಕೆ;
  timediff_t soft_eyeballs_timeout_ms;
  timediff_t hard_eyeballs_timeout_ms;
  uint8_t def_transport;
  ಬಿಟ್(httpsrr_resolved);
  ಬಿಟ್(checked_h3);
  ಬಿಟ್(ballers_complete);
};

ಸ್ಥಿರ ಶೂನ್ಯ cf_hc_ctx_destroy(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ cf_hc_ctx *ctx)
{
  ಆದರೆ(ctx) {
    size_t i;
    ಗಾಗಿ(i = 0; i < ctx->baller_ಎಣಿಕೆ; ++i)
      cf_hc_baller_discard(&ctx->ballers[i], ದತ್ತಾಂಶ);
    Curl_peer_unlink(&ctx->destination);
    curlx_free(ctx);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_hc_baller_ನಿಯೋಜಿಸು(ರಚನೆ cf_hc_baller *b,
                                ಎಣಿಕೆ alpnid alpn_id,
                                uint8_t def_transport)
{
  b->alpn_id = alpn_id;
  b->transport = def_transport;
  b->cf = ಖಾಲಿ;
  b->ಫಲಿತಾಂಶ = CURLE_OK;
  b->ಉತ್ತರ_ms = -1;
  b->shutdown = FALSE;
  ಆಯ್ಕೆ(b->alpn_id) {
  ಸ್ಥಿತಿ ALPN_h3:
    b->name = "h3";
    b->transport = TRNSPRT_QUIC;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ ALPN_h2:
    b->name = "h2";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ ALPN_h1:
    b->name = "h1";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ ALPN_none:
    b->name = "no-alpn";
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    b->ಫಲಿತಾಂಶ = CURLE_FAILED_INIT;
    ನಿಲ್ಲಿಸು;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_hc_baller_init(ರಚನೆ cf_hc_baller *b,
                              ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *save = cf->ಮುಂದಿನ;

  cf->ಮುಂದಿನ = ಖಾಲಿ;
  b->ಫಲಿತಾಂಶ = Curl_cf_ಸಿದ್ಧತೆ_ಸೇರಿಸು_after(cf, ದತ್ತಾಂಶ, b->transport,
                                         CURL_CF_SSL_ENABLE);
  b->cf = cf->ಮುಂದಿನ;
  cf->ಮುಂದಿನ = save;
}

ಸ್ಥಿರ CURLcode cf_hc_baller_connect(ರಚನೆ cf_hc_baller *b,
                                     ರಚನೆ Curl_cfilter *cf,
                                     ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     bool *ಮುಗಿದ)
{
  ರಚನೆ Curl_cfilter *save = cf->ಮುಂದಿನ;

  cf->ಮುಂದಿನ = b->cf;
  b->ಫಲಿತಾಂಶ = Curl_conn_cf_connect(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, ಮುಗಿದ);
  b->cf = cf->ಮುಂದಿನ; /* it might mutate */
  cf->ಮುಂದಿನ = save;
  ಹಿಂತಿರುಗಿ b->ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode baller_connected(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ cf_hc_baller *winner)
{
  ರಚನೆ cf_hc_ctx *ctx = cf->ctx;

  /* Make the winner's connection filter out own sub-filter, check, move,
   * close all remaining. */
  ಆದರೆ(cf->ಮುಂದಿನ) {
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }
  ಆದರೆ(!winner->cf) {
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }

  cf->ಮುಂದಿನ = winner->cf;
  winner->cf = ಖಾಲಿ;
  ctx->state = CF_HC_SUCCESS;
  cf->connected = TRUE;

  /* ballers may have failf()'d, the winner resets it, so our
   * errorbuf is clean again. */
  Curl_reset_fail(ದತ್ತಾಂಶ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NGHTTP2
  {
    /* For a negotiated HTTP/2 connection insert the h2 filter. */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alpn = Curl_conn_cf_get_alpn_negotiated(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ);
    ಆದರೆ(alpn && !strcmp("h2", alpn)) {
      CURLcode ಫಲಿತಾಂಶ = Curl_http2_switch_at(cf, ದತ್ತಾಂಶ);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        ctx->state = CF_HC_FAILURE;
        ctx->ಫಲಿತಾಂಶ = ಫಲಿತಾಂಶ;
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
    }
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ bool ಸಮಯ_ಗಮ್ಯ_ಆರಂಭ_baller2(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow)
{
  ರಚನೆ cf_hc_ctx *ctx = cf->ctx;
  timediff_t elapsed_ms;

  ಆದರೆ(ctx->baller_ಎಣಿಕೆ < 2)
    ಹಿಂತಿರುಗಿ FALSE;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cf_hc_baller_has_started(&ctx->ballers[1]))
    ಹಿಂತಿರುಗಿ FALSE;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ctx->ballers[0].ಫಲಿತಾಂಶ) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "%s baller failed, starting %s",
                ctx->ballers[0].name, ctx->ballers[1].name);
    ಹಿಂತಿರುಗಿ TRUE;
  }

  elapsed_ms = curlx_ptimediff_ms(pnow, &ctx->started);
  ಆದರೆ(elapsed_ms >= ctx->hard_eyeballs_timeout_ms) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "%s inconclusive after %" FMT_TIMEDIFF_T ", "
                "starting %s", ctx->ballers[0].name,
                ctx->hard_eyeballs_timeout_ms, ctx->ballers[1].name);
    ಹಿಂತಿರುಗಿ TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(elapsed_ms >= ctx->soft_eyeballs_timeout_ms) {
    ಆದರೆ(cf_hc_baller_ಉತ್ತರ_ms(&ctx->ballers[0], ದತ್ತಾಂಶ) < 0) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "%s has not seen any data after %"
                  FMT_TIMEDIFF_T "ms, starting %s",
                  ctx->ballers[0].name, ctx->soft_eyeballs_timeout_ms,
                  ctx->ballers[1].name);
      ಹಿಂತಿರುಗಿ TRUE;
    }
    /* set the effective hard timeout again */
    Curl_expire_ಗಣ(ದತ್ತಾಂಶ, EXPIRE_ALPN_EYEBALLS,
                    ctx->hard_eyeballs_timeout_ms - elapsed_ms, pnow);
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ bool cf_hc_may_h3(ರಚನೆ Curl_cfilter *cf,
                         ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_hc_ctx *ctx = cf->ctx;
  ಆದರೆ(!ctx->checked_h3) {
    ctx->check_h3_ಫಲಿತಾಂಶ =
      Curl_conn_may_http3(ದತ್ತಾಂಶ, cf->conn);
    ctx->checked_h3 = TRUE;
  }
  ಹಿಂತಿರುಗಿ !ctx->check_h3_ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಎಣಿಕೆ alpnid cf_hc_get_httpsrr_alpn(ರಚನೆ Curl_cfilter *cf,
                                          ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ಎಣಿಕೆ alpnid not_this_one)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
  ರಚನೆ cf_hc_ctx *ctx = cf->ctx;
  /* Is there an HTTPSRR use its ALPNs here.
   * We are here after having selected a connection to a host+port and
   * can no longer change that. Any HTTPSRR advice for other hosts and ports
   * we need to ignore. */
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_https_rrinfo *rr;
  size_t i;

  /* Do we have HTTPS-RR information? */
  rr = Curl_conn_dns_get_https(ದತ್ತಾಂಶ, cf->sockindex, ctx->destination);

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "HTTPS-RR %savailable", rr ? "" : "not ");
  /* We do not support `rr->no_def_alpn`. */
  ಆದರೆ(Curl_httpsrr_applicable(ದತ್ತಾಂಶ, rr) && !rr->no_def_alpn) {
    ಗಾಗಿ(i = 0; i < CURL_ARRAYSIZE(rr->alpns); ++i) {
      ಎಣಿಕೆ alpnid alpn_rr = (ಎಣಿಕೆ alpnid)rr->alpns[i];
      ಆದರೆ(alpn_rr == not_this_one) /* do not want this one */
        ಮುಂದುವರಿ;
      ಆಯ್ಕೆ(alpn_rr) {
      ಸ್ಥಿತಿ ALPN_h3:
        ಆದರೆ((ದತ್ತಾಂಶ->state.http_neg.allowed & CURL_HTTP_V3x) &&
           cf_hc_may_h3(cf, ದತ್ತಾಂಶ)) {
          ಹಿಂತಿರುಗಿ alpn_rr;
        }
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ ALPN_h2:
        ಆದರೆ(ದತ್ತಾಂಶ->state.http_neg.allowed & CURL_HTTP_V2x) {
          ಹಿಂತಿರುಗಿ alpn_rr;
        }
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ ALPN_h1:
        ಆದರೆ(ದತ್ತಾಂಶ->state.http_neg.allowed & CURL_HTTP_V1x) {
          ಹಿಂತಿರುಗಿ alpn_rr;
        }
        ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ: /* ignore */
        ನಿಲ್ಲಿಸು;
      }
    }
  }
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)not_this_one;
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ ALPN_none;
}

ಸ್ಥಿರ ಎಣಿಕೆ alpnid cf_hc_get_pref_alpn(ರಚನೆ Curl_cfilter *cf,
                                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ಎಣಿಕೆ alpnid not_this_one)
{
  ಆದರೆ((ದತ್ತಾಂಶ->state.http_neg.preferred & ದತ್ತಾಂಶ->state.http_neg.allowed)) {
    ಆಯ್ಕೆ(ದತ್ತಾಂಶ->state.http_neg.preferred) {
    ಸ್ಥಿತಿ CURL_HTTP_V3x:
      ಆದರೆ(cf_hc_may_h3(cf, ದತ್ತಾಂಶ) && (ALPN_h3 != not_this_one))
        ಹಿಂತಿರುಗಿ ALPN_h3;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURL_HTTP_V2x:
      ಆದರೆ(ALPN_h2 != not_this_one)
        ಹಿಂತಿರುಗಿ ALPN_h2;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURL_HTTP_V1x:
      /* If we are trying h2 already, h1 is already used as fallback */
      ಆದರೆ((ALPN_h1 != not_this_one) && (ALPN_h2 != not_this_one))
        ಹಿಂತಿರುಗಿ ALPN_h1;
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      ನಿಲ್ಲಿಸು;
    }
  }
  ಹಿಂತಿರುಗಿ ALPN_none;
}

ಸ್ಥಿರ ಎಣಿಕೆ alpnid cf_hc_get_ಮೊದಲ_alpn(ರಚನೆ Curl_cfilter *cf,
                                        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        http_majors choices,
                                        ಎಣಿಕೆ alpnid not_this_one)
{
  /* When told to not try h2, we also do not try h1 and vice versa */
  bool allow_h1_or_h2 = (not_this_one != ALPN_h1) &&
                        (not_this_one != ALPN_h2);
  ಆದರೆ((ALPN_h3 != not_this_one) && (choices & CURL_HTTP_V3x) &&
     cf_hc_may_h3(cf, ದತ್ತಾಂಶ)) {
    ಹಿಂತಿರುಗಿ ALPN_h3;
  }
  ಆದರೆ(allow_h1_or_h2 && (choices & CURL_HTTP_V2x)) {
    ಹಿಂತಿರುಗಿ ALPN_h2;
  }
  ಆದರೆ(allow_h1_or_h2 && (choices & CURL_HTTP_V1x)) {
    ಹಿಂತಿರುಗಿ ALPN_h1;
  }
  ಹಿಂತಿರುಗಿ ALPN_none;
}

ಸ್ಥಿರ CURLcode cf_hc_ಗಣ_baller1(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_hc_ctx *ctx = cf->ctx;
  ಎಣಿಕೆ alpnid alpn1 = ALPN_none;
  VERBOSE(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ = "HTTPS-RR");

  DEBUGASSERT(cf->conn->ಬಿಟ್ಟುಗಳು.tls_enable_alpn);

  alpn1 = cf_hc_get_httpsrr_alpn(cf, ದತ್ತಾಂಶ, ALPN_none);
  ಆದರೆ(alpn1 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(ಮೂಲ = "preferred version");
    alpn1 = cf_hc_get_pref_alpn(cf, ದತ್ತಾಂಶ, ALPN_none);
  }
  ಆದರೆ(alpn1 == ALPN_none) {
    VERBOSE(ಮೂಲ = "wanted versions");
    alpn1 = cf_hc_get_ಮೊದಲ_alpn(cf, ದತ್ತಾಂಶ,
                                 ದತ್ತಾಂಶ->state.http_neg.wanted,
                                 ALPN_none);
  }
  ಆದರೆ(alpn1 == ALPN_none) {
    VERBOSE(ಮೂಲ = "allowed versions");
    alpn1 = cf_hc_get_ಮೊದಲ_alpn(cf, ದತ್ತಾಂಶ,
                                 ದತ್ತಾಂಶ->state.http_neg.allowed,
                                 ALPN_none);
  }

  ಆದರೆ(alpn1 == ALPN_none) {
    /* None of the wanted/allowed HTTP versions could be chosen */
    ಆದರೆ(ctx->check_h3_ಫಲಿತಾಂಶ) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "unable to use HTTP/3");
      ಹಿಂತಿರುಗಿ ctx->check_h3_ಫಲಿತಾಂಶ;
    }
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "unable to select HTTP version");
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }

  cf_hc_baller_ನಿಯೋಜಿಸು(&ctx->ballers[0], alpn1, ctx->def_transport);
  ctx->baller_ಎಣಿಕೆ = 1;
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "1st attempt uses %s from %s",
              ctx->ballers[0].name, ಮೂಲ);

  ಆಯ್ಕೆ(alpn1) {
  ಸ್ಥಿತಿ ALPN_h1:
    /* We really want h1, switch off h2 to make it disappear in ALPN */
    ದತ್ತಾಂಶ->state.http_neg.wanted &= (uint8_t)~CURL_HTTP_V2x;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_hc_ಗಣ_baller2(ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_hc_ctx *ctx = cf->ctx;
  ಎಣಿಕೆ alpnid alpn2 = ALPN_none, alpn1 = ctx->ballers[0].alpn_id;
  VERBOSE(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ = "HTTPS-RR");

  ಆದರೆ(ctx->ballers_complete)
    ಹಿಂತಿರುಗಿ; /* already done */
  ಆದರೆ(!ctx->httpsrr_resolved)
    ಹಿಂತಿರುಗಿ; /* HTTPS-RR pending */

  alpn2 = cf_hc_get_httpsrr_alpn(cf, ದತ್ತಾಂಶ, alpn1);
  ಆದರೆ(alpn2 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(ಮೂಲ = "preferred version");
    alpn2 = cf_hc_get_pref_alpn(cf, ದತ್ತಾಂಶ, alpn1);
  }
  ಆದರೆ(alpn2 == ALPN_none) {
    VERBOSE(ಮೂಲ = "wanted versions");
    alpn2 = cf_hc_get_ಮೊದಲ_alpn(cf, ದತ್ತಾಂಶ,
                                 ದತ್ತಾಂಶ->state.http_neg.wanted,
                                 alpn1);
  }

  ಆದರೆ(alpn2 != ALPN_none) {
    cf_hc_baller_ನಿಯೋಜಿಸು(&ctx->ballers[1], alpn2, ctx->def_transport);
    ctx->baller_ಎಣಿಕೆ = 2;
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "2nd attempt uses %s from %s",
                ctx->ballers[1].name, ಮೂಲ);
  }
  ctx->ballers_complete = TRUE;
}

ಸ್ಥಿರ CURLcode cf_hc_connect(ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              bool *ಮುಗಿದ)
{
  ರಚನೆ cf_hc_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow = ಖಾಲಿ;

  ಆದರೆ(cf->connected) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  *ಮುಗಿದ = FALSE;

  ಆದರೆ(!ctx->httpsrr_resolved) {
    ctx->httpsrr_resolved = Curl_conn_dns_resolved_https(
      ದತ್ತಾಂಶ, cf->sockindex, ctx->destination);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
    ಆದರೆ(!ctx->httpsrr_resolved && getenv("CURL_DBG_AWAIT_HTTPSRR")) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "awaiting HTTPS-RR");
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
#ಆದರೆ_ಅಂತ್ಯ
  }

  ಆಯ್ಕೆ(ctx->state) {
  ಸ್ಥಿತಿ CF_HC_RESOLV:
    ctx->state = CF_HC_INIT;
    FALLTHROUGH();

  ಸ್ಥಿತಿ CF_HC_INIT:
    DEBUGASSERT(!cf->ಮುಂದಿನ);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "connect, init");
    ಫಲಿತಾಂಶ = cf_hc_ಗಣ_baller1(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ctx->ಫಲಿತಾಂಶ = ಫಲಿತಾಂಶ;
      ctx->state = CF_HC_FAILURE;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    cf_hc_ಗಣ_baller2(cf, ದತ್ತಾಂಶ);
    pnow = Curl_pgrs_now(ದತ್ತಾಂಶ);
    ctx->started = *pnow;
    cf_hc_baller_init(&ctx->ballers[0], cf, ದತ್ತಾಂಶ);
    ಆದರೆ((ctx->baller_ಎಣಿಕೆ > 1) || !ctx->ballers_complete) {
      Curl_expire_ಗಣ(ದತ್ತಾಂಶ, EXPIRE_ALPN_EYEBALLS,
                      ctx->soft_eyeballs_timeout_ms, pnow);
    }
    ctx->state = CF_HC_CONNECT;
    FALLTHROUGH();

  ಸ್ಥಿತಿ CF_HC_CONNECT:
    ಆದರೆ(!ctx->ballers_complete)
      cf_hc_ಗಣ_baller2(cf, ದತ್ತಾಂಶ);

    ಆದರೆ(cf_hc_baller_is_connecting(&ctx->ballers[0])) {
      ಫಲಿತಾಂಶ = cf_hc_baller_connect(&ctx->ballers[0], cf, ದತ್ತಾಂಶ, ಮುಗಿದ);
      ಆದರೆ(!ಫಲಿತಾಂಶ && *ಮುಗಿದ) {
        ಫಲಿತಾಂಶ = baller_connected(cf, ದತ್ತಾಂಶ, &ctx->ballers[0]);
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }

    pnow = Curl_pgrs_now(ದತ್ತಾಂಶ);
    ಆದರೆ(ಸಮಯ_ಗಮ್ಯ_ಆರಂಭ_baller2(cf, ದತ್ತಾಂಶ, pnow)) {
      cf_hc_baller_init(&ctx->ballers[1], cf, ದತ್ತಾಂಶ);
    }

    ಆದರೆ(cf_hc_baller_is_connecting(&ctx->ballers[1])) {
      ಫಲಿತಾಂಶ = cf_hc_baller_connect(&ctx->ballers[1], cf, ದತ್ತಾಂಶ, ಮುಗಿದ);
      ಆದರೆ(!ಫಲಿತಾಂಶ && *ಮುಗಿದ) {
        ಫಲಿತಾಂಶ = baller_connected(cf, ದತ್ತಾಂಶ, &ctx->ballers[1]);
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }

    ಆದರೆ(ctx->ballers[0].ಫಲಿತಾಂಶ &&
       (ctx->ballers[1].ಫಲಿತಾಂಶ ||
        (ctx->ballers_complete && (ctx->baller_ಎಣಿಕೆ < 2)))) {
      /* all have failed. we give up */
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "connect, all attempts failed");
      ctx->ಫಲಿತಾಂಶ = ಫಲಿತಾಂಶ = ctx->ballers[0].ಫಲಿತಾಂಶ;
      ctx->state = CF_HC_FAILURE;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಫಲಿತಾಂಶ = CURLE_OK;
    *ಮುಗಿದ = FALSE;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CF_HC_FAILURE:
    ಫಲಿತಾಂಶ = ctx->ಫಲಿತಾಂಶ;
    cf->connected = FALSE;
    *ಮುಗಿದ = FALSE;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CF_HC_SUCCESS:
    ಫಲಿತಾಂಶ = CURLE_OK;
    cf->connected = TRUE;
    *ಮುಗಿದ = TRUE;
    ನಿಲ್ಲಿಸು;
  }

ಹೊರಹೋಗು:
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "connect -> %d, done=%d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *ಮುಗಿದ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_hc_shutdown(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  ರಚನೆ cf_hc_ctx *ctx = cf->ctx;
  size_t i;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(ದತ್ತಾಂಶ);
  ಆದರೆ(cf->connected) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  ಗಾಗಿ(i = 0; i < ctx->baller_ಎಣಿಕೆ; i++) {
    ರಚನೆ cf_hc_baller *b = &ctx->ballers[i];
    bool bdone = FALSE;
    ಆದರೆ(!cf_hc_baller_is_connecting(b) || b->shutdown)
      ಮುಂದುವರಿ;
    b->ಫಲಿತಾಂಶ = b->cf->cft->do_shutdown(b->cf, ದತ್ತಾಂಶ, &bdone);
    ಆದರೆ(b->ಫಲಿತಾಂಶ || bdone)
      b->shutdown = TRUE; /* treat a failed shutdown as done */
  }

  *ಮುಗಿದ = TRUE;
  ಗಾಗಿ(i = 0; i < ctx->baller_ಎಣಿಕೆ; i++) {
    ಆದರೆ(!ctx->ballers[i].shutdown)
      *ಮುಗಿದ = FALSE;
  }
  ಆದರೆ(*ಮುಗಿದ) {
    ಗಾಗಿ(i = 0; i < ctx->baller_ಎಣಿಕೆ; i++) {
      ಆದರೆ(ctx->ballers[i].ಫಲಿತಾಂಶ)
        ಫಲಿತಾಂಶ = ctx->ballers[i].ಫಲಿತಾಂಶ;
    }
  }
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "shutdown -> %d, done=%d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *ಮುಗಿದ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_hc_adjust_pollset(ರಚನೆ Curl_cfilter *cf,
                                     ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ easy_pollset *ps)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಆದರೆ(!cf->connected) {
    ರಚನೆ cf_hc_ctx *ctx = cf->ctx;
    size_t i;

    ಗಾಗಿ(i = 0; (i < ctx->baller_ಎಣಿಕೆ) && !ಫಲಿತಾಂಶ; i++) {
      ರಚನೆ cf_hc_baller *b = &ctx->ballers[i];
      ಆದರೆ(!cf_hc_baller_is_connecting(b))
        ಮುಂದುವರಿ;
      ಫಲಿತಾಂಶ = Curl_conn_cf_adjust_pollset(b->cf, ದತ್ತಾಂಶ, ps);
    }
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "adjust_pollset -> %d, %u socks", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ,
                ps->n);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ bool cf_hc_ದತ್ತಾಂಶ_pending(ರಚನೆ Curl_cfilter *cf,
                               ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  ಆದರೆ(cf->connected)
    ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ->cft->has_ದತ್ತಾಂಶ_pending(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ);

  ಗಾಗಿ(i = 0; i < ctx->baller_ಎಣಿಕೆ; i++)
    ಆದರೆ(cf_hc_baller_ದತ್ತಾಂಶ_pending(&ctx->ballers[i], ದತ್ತಾಂಶ))
      ಹಿಂತಿರುಗಿ TRUE;
  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ CURLcode cf_hc_query(ರಚನೆ Curl_cfilter *cf,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ಪೂರ್ಣಾಂಕ query, ಪೂರ್ಣಾಂಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ರಚನೆ cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  ಆದರೆ(!cf->connected) {
    ಆಯ್ಕೆ(query) {
    ಸ್ಥಿತಿ CF_QUERY_NEED_FLUSH: {
      ಗಾಗಿ(i = 0; i < ctx->baller_ಎಣಿಕೆ; i++)
        ಆದರೆ(cf_hc_baller_needs_flush(&ctx->ballers[i], ದತ್ತಾಂಶ)) {
          *pres1 = TRUE;
          ಹಿಂತಿರುಗಿ CURLE_OK;
        }
      ನಿಲ್ಲಿಸು;
    }
    ಪೂರ್ವನಿಯೋಜಿತ:
      ನಿಲ್ಲಿಸು;
    }
  }
  ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ ?
    cf->ಮುಂದಿನ->cft->query(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ಸ್ಥಿರ CURLcode cf_hc_cntrl(ರಚನೆ Curl_cfilter *cf,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ಪೂರ್ಣಾಂಕ event, ಪೂರ್ಣಾಂಕ arg1, ಶೂನ್ಯ *arg2)
{
  ರಚನೆ cf_hc_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t i;

  ಆದರೆ(!cf->connected) {
    ಆಯ್ಕೆ(event) {
    ಸ್ಥಿತಿ CF_CTRL_REPORT_STATS:
    ಸ್ಥಿತಿ CF_CTRL_CONN_INFO_UPDATE:
      ಗಾಗಿ(i = 0; i < ctx->baller_ಎಣಿಕೆ; i++) {
        /* Make the first baller that connected at network level report */
        ಆದರೆ(Curl_conn_cf_is_ip_connected(ctx->ballers[i].cf, ದತ್ತಾಂಶ)) {
          Curl_conn_cf_cntrl(ctx->ballers[i].cf, ದತ್ತಾಂಶ, TRUE,
                             event, arg1, arg2);
          ನಿಲ್ಲಿಸು;
        }
      }
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      ಗಾಗಿ(i = 0; i < ctx->baller_ಎಣಿಕೆ; i++) {
        ಫಲಿತಾಂಶ = cf_hc_baller_cntrl(&ctx->ballers[i], ದತ್ತಾಂಶ, event, arg1, arg2);
        ಆದರೆ(ಫಲಿತಾಂಶ && (ಫಲಿತಾಂಶ != CURLE_AGAIN))
          ಹೋಗು ಹೊರಹೋಗು;
      }
      ಫಲಿತಾಂಶ = CURLE_OK;
      ನಿಲ್ಲಿಸು;
    }
  }
ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_hc_destroy(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_hc_ctx *ctx = cf->ctx;

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "destroy");
  cf_hc_ctx_destroy(ದತ್ತಾಂಶ, ctx);
}

ರಚನೆ Curl_cftype Curl_cft_http_connect = {
  "HTTPS-CONNECT",
  CF_TYPE_ಸಿದ್ಧತೆ,
  CURL_LOG_LVL_NONE,
  cf_hc_destroy,
  cf_hc_connect,
  cf_hc_shutdown,
  cf_hc_adjust_pollset,
  cf_hc_ದತ್ತಾಂಶ_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  cf_hc_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_hc_query,
};

ಸ್ಥಿರ CURLcode cf_hc_create(ರಚನೆ Curl_cfilter **pcf,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ Curl_peer *destination,
                             uint8_t def_transport)
{
  ರಚನೆ Curl_cfilter *cf = ಖಾಲಿ;
  ರಚನೆ cf_hc_ctx *ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ctx = curlx_calloc(1, ಗಾತ್ರ(*ctx));
  ಆದರೆ(!ctx) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  Curl_peer_link(&ctx->destination, destination);
  ctx->def_transport = def_transport;
  ctx->hard_eyeballs_timeout_ms = ದತ್ತಾಂಶ->ಗಣ.happy_eyeballs_timeout;
  ctx->soft_eyeballs_timeout_ms = ದತ್ತಾಂಶ->ಗಣ.happy_eyeballs_timeout / 2;

  ಫಲಿತಾಂಶ = Curl_cf_create(&cf, &Curl_cft_http_connect, ctx);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  ctx = ಖಾಲಿ;

ಹೊರಹೋಗು:
  *pcf = ಫಲಿತಾಂಶ ? ಖಾಲಿ : cf;
  cf_hc_ctx_destroy(ದತ್ತಾಂಶ, ctx);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_hc_ಕೂಡಿಸು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ರಚನೆ Curl_peer *destination,
                          ರಚನೆ connectdata *conn,
                          int8_t sockindex,
                          uint8_t def_transport)
{
  ರಚನೆ Curl_cfilter *cf;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(ದತ್ತಾಂಶ);
  ಫಲಿತಾಂಶ = cf_hc_create(&cf, ದತ್ತಾಂಶ, destination, def_transport);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  Curl_conn_cf_ಕೂಡಿಸು(ದತ್ತಾಂಶ, conn, sockindex, cf);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
  ಫಲಿತಾಂಶ = Curl_conn_dns_ಕೂಡಿಸು_https_resolve(ದತ್ತಾಂಶ, cf->conn, cf->sockindex,
                                           destination);
#ಆದರೆ_ಅಂತ್ಯ
ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_cf_https_ಸಿದ್ಧತೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ Curl_peer *destination,
                             ರಚನೆ connectdata *conn,
                             int8_t sockindex)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(conn->scheme->protocol == CURLPROTO_HTTPS);

  /* This filter is intended for HTTPS using ALPN and does
   * not support HTTPS Eyeballing to a proxy. */
  ಆದರೆ((conn->scheme->protocol != CURLPROTO_HTTPS) ||
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
     conn->ಬಿಟ್ಟುಗಳು.origin_is_proxy ||
#ಆದರೆ_ಅಂತ್ಯ
     !conn->ಬಿಟ್ಟುಗಳು.tls_enable_alpn)
    ಹೋಗು ಹೊರಹೋಗು;

  ಫಲಿತಾಂಶ = cf_hc_ಕೂಡಿಸು(ದತ್ತಾಂಶ, destination, conn, sockindex,
                     conn->transport_wanted);

ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP */
