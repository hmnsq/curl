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

#ifndef CURL_DISABLE_HTTP

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

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  CF_HC_RESOLV,
  CF_HC_INIT,
  CF_HC_CONNECT,
  CF_HC_SUCCESS,
  CF_HC_FAILURE
} cf_hc_state;

ಸಂರಚನಾ cf_hc_baller {
  ಧ್ರುವ ವರ್ಣ *name;
  ಸಂರಚನಾ Curl_cfilter *cf;
  CURLcode result;
  ಪೂರ್ಣಾಙ್ಕ reply_ms;
  uint8_t transport;
  ಗಣನಾ alpnid alpn_id;
  BIT(shutdown);
};

ಸ್ಥಿರ ಶೂನ್ಯ cf_hc_baller_discard(ಸಂರಚನಾ cf_hc_baller *b,
                                 ಸಂರಚನಾ Curl_easy *data)
{
  ಯದಿ(b->cf) {
    Curl_conn_cf_discard_chain(&b->cf, data);
    b->cf = ರಿಕ್ತ;
  }
}

ಸ್ಥಿರ bool cf_hc_baller_is_connecting(ಸಂರಚನಾ cf_hc_baller *b)
{
  ಪ್ರತ್ಯಾಗಮ b->cf && !b->result;
}

ಸ್ಥಿರ bool cf_hc_baller_has_started(ಸಂರಚನಾ cf_hc_baller *b)
{
  ಪ್ರತ್ಯಾಗಮ !!b->cf;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cf_hc_baller_reply_ms(ಸಂರಚನಾ cf_hc_baller *b,
                                 ಸಂರಚನಾ Curl_easy *data)
{
  ಯದಿ(b->cf && (b->reply_ms < 0))
    b->cf->cft->query(b->cf, data, CF_QUERY_CONNECT_REPLY_MS,
                      &b->reply_ms, ರಿಕ್ತ);
  ಪ್ರತ್ಯಾಗಮ b->reply_ms;
}

ಸ್ಥಿರ bool cf_hc_baller_data_pending(ಸಂರಚನಾ cf_hc_baller *b,
                                      ಧ್ರುವ ಸಂರಚನಾ Curl_easy *data)
{
  ಪ್ರತ್ಯಾಗಮ b->cf && !b->result && b->cf->cft->has_data_pending(b->cf, data);
}

ಸ್ಥಿರ bool cf_hc_baller_needs_flush(ಸಂರಚನಾ cf_hc_baller *b,
                                     ಸಂರಚನಾ Curl_easy *data)
{
  ಪ್ರತ್ಯಾಗಮ b->cf && !b->result && Curl_conn_cf_needs_flush(b->cf, data);
}

ಸ್ಥಿರ CURLcode cf_hc_baller_cntrl(ಸಂರಚನಾ cf_hc_baller *b,
                                   ಸಂರಚನಾ Curl_easy *data,
                                   ಪೂರ್ಣಾಙ್ಕ event, ಪೂರ್ಣಾಙ್ಕ arg1, ಶೂನ್ಯ *arg2)
{
  ಯದಿ(b->cf && !b->result)
    ಪ್ರತ್ಯಾಗಮ Curl_conn_cf_cntrl(b->cf, data, FALSE, event, arg1, arg2);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸಂರಚನಾ cf_hc_ctx {
  cf_hc_state state;
  ಸಂರಚನಾ Curl_peer *destination; /* who we ultimately want to talk to */
  ಸಂರಚನಾ curltime started;  /* when connect started */
  CURLcode result;          /* overall result */
  CURLcode check_h3_result;
  ಸಂರಚನಾ cf_hc_baller ballers[2];
  size_t baller_ಎಣಿಕೆ;
  timediff_t soft_eyeballs_timeout_ms;
  timediff_t hard_eyeballs_timeout_ms;
  uint8_t def_transport;
  BIT(httpsrr_resolved);
  BIT(checked_h3);
  BIT(ballers_complete);
};

ಸ್ಥಿರ ಶೂನ್ಯ cf_hc_ctx_destroy(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ cf_hc_ctx *ctx)
{
  ಯದಿ(ctx) {
    size_t i;
    ಪ್ರತಿ(i = 0; i < ctx->baller_ಎಣಿಕೆ; ++i)
      cf_hc_baller_discard(&ctx->ballers[i], data);
    Curl_peer_unlink(&ctx->destination);
    curlx_free(ctx);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_hc_baller_assign(ಸಂರಚನಾ cf_hc_baller *b,
                                ಗಣನಾ alpnid alpn_id,
                                uint8_t def_transport)
{
  b->alpn_id = alpn_id;
  b->transport = def_transport;
  b->cf = ರಿಕ್ತ;
  b->result = CURLE_OK;
  b->reply_ms = -1;
  b->shutdown = FALSE;
  ವಿಕಲ್ಪ(b->alpn_id) {
  ಪಕ್ಷ ALPN_h3:
    b->name = "h3";
    b->transport = TRNSPRT_QUIC;
    ಭಙ್ಗ;
  ಪಕ್ಷ ALPN_h2:
    b->name = "h2";
    ಭಙ್ಗ;
  ಪಕ್ಷ ALPN_h1:
    b->name = "h1";
    ಭಙ್ಗ;
  ಪಕ್ಷ ALPN_none:
    b->name = "no-alpn";
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    b->result = CURLE_FAILED_INIT;
    ಭಙ್ಗ;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_hc_baller_init(ಸಂರಚನಾ cf_hc_baller *b,
                              ಸಂರಚನಾ Curl_cfilter *cf,
                              ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_cfilter *save = cf->next;

  cf->next = ರಿಕ್ತ;
  b->result = Curl_cf_setup_ಸೇರಿಸು_after(cf, data, b->transport,
                                         CURL_CF_SSL_ENABLE);
  b->cf = cf->next;
  cf->next = save;
}

ಸ್ಥಿರ CURLcode cf_hc_baller_connect(ಸಂರಚನಾ cf_hc_baller *b,
                                     ಸಂರಚನಾ Curl_cfilter *cf,
                                     ಸಂರಚನಾ Curl_easy *data,
                                     bool *done)
{
  ಸಂರಚನಾ Curl_cfilter *save = cf->next;

  cf->next = b->cf;
  b->result = Curl_conn_cf_connect(cf->next, data, done);
  b->cf = cf->next; /* it might mutate */
  cf->next = save;
  ಪ್ರತ್ಯಾಗಮ b->result;
}

ಸ್ಥಿರ CURLcode baller_connected(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ cf_hc_baller *winner)
{
  ಸಂರಚನಾ cf_hc_ctx *ctx = cf->ctx;

  /* Make the winner's connection filter out own sub-filter, check, move,
   * close all remaining. */
  ಯದಿ(cf->next) {
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }
  ಯದಿ(!winner->cf) {
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }

  cf->next = winner->cf;
  winner->cf = ರಿಕ್ತ;
  ctx->state = CF_HC_SUCCESS;
  cf->connected = TRUE;

  /* ballers may have failf()'d, the winner resets it, so our
   * errorbuf is clean again. */
  Curl_reset_fail(data);

#ifdef USE_NGHTTP2
  {
    /* For a negotiated HTTP/2 connection insert the h2 filter. */
    ಧ್ರುವ ವರ್ಣ *alpn = Curl_conn_cf_get_alpn_negotiated(cf->next, data);
    ಯದಿ(alpn && !strcmp("h2", alpn)) {
      CURLcode result = Curl_http2_switch_at(cf, data);
      ಯದಿ(result) {
        ctx->state = CF_HC_FAILURE;
        ctx->result = result;
        ಪ್ರತ್ಯಾಗಮ result;
      }
    }
  }
#endif
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ bool time_to_start_baller2(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  ಧ್ರುವ ಸಂರಚನಾ curltime *pnow)
{
  ಸಂರಚನಾ cf_hc_ctx *ctx = cf->ctx;
  timediff_t elapsed_ms;

  ಯದಿ(ctx->baller_ಎಣಿಕೆ < 2)
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಅನ್ಯಥಾ ಯದಿ(cf_hc_baller_has_started(&ctx->ballers[1]))
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಅನ್ಯಥಾ ಯದಿ(ctx->ballers[0].result) {
    CURL_TRC_CF(data, cf, "%s baller failed, starting %s",
                ctx->ballers[0].name, ctx->ballers[1].name);
    ಪ್ರತ್ಯಾಗಮ TRUE;
  }

  elapsed_ms = curlx_ptimediff_ms(pnow, &ctx->started);
  ಯದಿ(elapsed_ms >= ctx->hard_eyeballs_timeout_ms) {
    CURL_TRC_CF(data, cf, "%s inconclusive after %" FMT_TIMEDIFF_T ", "
                "starting %s", ctx->ballers[0].name,
                ctx->hard_eyeballs_timeout_ms, ctx->ballers[1].name);
    ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  ಅನ್ಯಥಾ ಯದಿ(elapsed_ms >= ctx->soft_eyeballs_timeout_ms) {
    ಯದಿ(cf_hc_baller_reply_ms(&ctx->ballers[0], data) < 0) {
      CURL_TRC_CF(data, cf, "%s has not seen any data after %"
                  FMT_TIMEDIFF_T "ms, starting %s",
                  ctx->ballers[0].name, ctx->soft_eyeballs_timeout_ms,
                  ctx->ballers[1].name);
      ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    /* set the effective hard timeout again */
    Curl_expire_set(data, EXPIRE_ALPN_EYEBALLS,
                    ctx->hard_eyeballs_timeout_ms - elapsed_ms, pnow);
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ bool cf_hc_may_h3(ಸಂರಚನಾ Curl_cfilter *cf,
                         ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_hc_ctx *ctx = cf->ctx;
  ಯದಿ(!ctx->checked_h3) {
    ctx->check_h3_result =
      Curl_conn_may_http3(data, cf->conn);
    ctx->checked_h3 = TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ !ctx->check_h3_result;
}

ಸ್ಥಿರ ಗಣನಾ alpnid cf_hc_get_httpsrr_alpn(ಸಂರಚನಾ Curl_cfilter *cf,
                                          ಸಂರಚನಾ Curl_easy *data,
                                          ಗಣನಾ alpnid not_this_one)
{
#ifdef USE_HTTPSRR
  ಸಂರಚನಾ cf_hc_ctx *ctx = cf->ctx;
  /* Is there an HTTPSRR use its ALPNs here.
   * We are here after having selected a connection to a host+port and
   * can no longer change that. Any HTTPSRR advice for other hosts and ports
   * we need to ignore. */
  ಧ್ರುವ ಸಂರಚನಾ Curl_https_rrinfo *rr;
  size_t i;

  /* Do we have HTTPS-RR information? */
  rr = Curl_conn_dns_get_https(data, cf->sockindex, ctx->destination);

  CURL_TRC_CF(data, cf, "HTTPS-RR %savailable", rr ? "" : "not ");
  /* We do not support `rr->no_def_alpn`. */
  ಯದಿ(Curl_httpsrr_applicable(data, rr) && !rr->no_def_alpn) {
    ಪ್ರತಿ(i = 0; i < CURL_ARRAYSIZE(rr->alpns); ++i) {
      ಗಣನಾ alpnid alpn_rr = (ಗಣನಾ alpnid)rr->alpns[i];
      ಯದಿ(alpn_rr == not_this_one) /* do not want this one */
        ಅನುವರ್ತ;
      ವಿಕಲ್ಪ(alpn_rr) {
      ಪಕ್ಷ ALPN_h3:
        ಯದಿ((data->state.http_neg.allowed & CURL_HTTP_V3x) &&
           cf_hc_may_h3(cf, data)) {
          ಪ್ರತ್ಯಾಗಮ alpn_rr;
        }
        ಭಙ್ಗ;
      ಪಕ್ಷ ALPN_h2:
        ಯದಿ(data->state.http_neg.allowed & CURL_HTTP_V2x) {
          ಪ್ರತ್ಯಾಗಮ alpn_rr;
        }
        ಭಙ್ಗ;
      ಪಕ್ಷ ALPN_h1:
        ಯದಿ(data->state.http_neg.allowed & CURL_HTTP_V1x) {
          ಪ್ರತ್ಯಾಗಮ alpn_rr;
        }
        ಭಙ್ಗ;
      ಸ್ವಭಾವ: /* ignore */
        ಭಙ್ಗ;
      }
    }
  }
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)not_this_one;
#endif
  ಪ್ರತ್ಯಾಗಮ ALPN_none;
}

ಸ್ಥಿರ ಗಣನಾ alpnid cf_hc_get_pref_alpn(ಸಂರಚನಾ Curl_cfilter *cf,
                                       ಸಂರಚನಾ Curl_easy *data,
                                       ಗಣನಾ alpnid not_this_one)
{
  ಯದಿ((data->state.http_neg.preferred & data->state.http_neg.allowed)) {
    ವಿಕಲ್ಪ(data->state.http_neg.preferred) {
    ಪಕ್ಷ CURL_HTTP_V3x:
      ಯದಿ(cf_hc_may_h3(cf, data) && (ALPN_h3 != not_this_one))
        ಪ್ರತ್ಯಾಗಮ ALPN_h3;
      ಭಙ್ಗ;
    ಪಕ್ಷ CURL_HTTP_V2x:
      ಯದಿ(ALPN_h2 != not_this_one)
        ಪ್ರತ್ಯಾಗಮ ALPN_h2;
      ಭಙ್ಗ;
    ಪಕ್ಷ CURL_HTTP_V1x:
      /* If we are trying h2 already, h1 is already used as fallback */
      ಯದಿ((ALPN_h1 != not_this_one) && (ALPN_h2 != not_this_one))
        ಪ್ರತ್ಯಾಗಮ ALPN_h1;
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      ಭಙ್ಗ;
    }
  }
  ಪ್ರತ್ಯಾಗಮ ALPN_none;
}

ಸ್ಥಿರ ಗಣನಾ alpnid cf_hc_get_first_alpn(ಸಂರಚನಾ Curl_cfilter *cf,
                                        ಸಂರಚನಾ Curl_easy *data,
                                        http_majors choices,
                                        ಗಣನಾ alpnid not_this_one)
{
  /* When told to not try h2, we also do not try h1 and vice versa */
  bool allow_h1_or_h2 = (not_this_one != ALPN_h1) &&
                        (not_this_one != ALPN_h2);
  ಯದಿ((ALPN_h3 != not_this_one) && (choices & CURL_HTTP_V3x) &&
     cf_hc_may_h3(cf, data)) {
    ಪ್ರತ್ಯಾಗಮ ALPN_h3;
  }
  ಯದಿ(allow_h1_or_h2 && (choices & CURL_HTTP_V2x)) {
    ಪ್ರತ್ಯಾಗಮ ALPN_h2;
  }
  ಯದಿ(allow_h1_or_h2 && (choices & CURL_HTTP_V1x)) {
    ಪ್ರತ್ಯಾಗಮ ALPN_h1;
  }
  ಪ್ರತ್ಯಾಗಮ ALPN_none;
}

ಸ್ಥಿರ CURLcode cf_hc_set_baller1(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_hc_ctx *ctx = cf->ctx;
  ಗಣನಾ alpnid alpn1 = ALPN_none;
  VERBOSE(ಧ್ರುವ ವರ್ಣ *source = "HTTPS-RR");

  DEBUGASSERT(cf->conn->bits.tls_enable_alpn);

  alpn1 = cf_hc_get_httpsrr_alpn(cf, data, ALPN_none);
  ಯದಿ(alpn1 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(source = "preferred version");
    alpn1 = cf_hc_get_pref_alpn(cf, data, ALPN_none);
  }
  ಯದಿ(alpn1 == ALPN_none) {
    VERBOSE(source = "wanted versions");
    alpn1 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.wanted,
                                 ALPN_none);
  }
  ಯದಿ(alpn1 == ALPN_none) {
    VERBOSE(source = "allowed versions");
    alpn1 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.allowed,
                                 ALPN_none);
  }

  ಯದಿ(alpn1 == ALPN_none) {
    /* None of the wanted/allowed HTTP versions could be chosen */
    ಯದಿ(ctx->check_h3_result) {
      CURL_TRC_CF(data, cf, "unable to use HTTP/3");
      ಪ್ರತ್ಯಾಗಮ ctx->check_h3_result;
    }
    CURL_TRC_CF(data, cf, "unable to select HTTP version");
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }

  cf_hc_baller_assign(&ctx->ballers[0], alpn1, ctx->def_transport);
  ctx->baller_ಎಣಿಕೆ = 1;
  CURL_TRC_CF(data, cf, "1st attempt uses %s from %s",
              ctx->ballers[0].name, source);

  ವಿಕಲ್ಪ(alpn1) {
  ಪಕ್ಷ ALPN_h1:
    /* We really want h1, switch off h2 to make it disappear in ALPN */
    data->state.http_neg.wanted &= (uint8_t)~CURL_HTTP_V2x;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_hc_set_baller2(ಸಂರಚನಾ Curl_cfilter *cf,
                              ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_hc_ctx *ctx = cf->ctx;
  ಗಣನಾ alpnid alpn2 = ALPN_none, alpn1 = ctx->ballers[0].alpn_id;
  VERBOSE(ಧ್ರುವ ವರ್ಣ *source = "HTTPS-RR");

  ಯದಿ(ctx->ballers_complete)
    ಪ್ರತ್ಯಾಗಮ; /* already done */
  ಯದಿ(!ctx->httpsrr_resolved)
    ಪ್ರತ್ಯಾಗಮ; /* HTTPS-RR pending */

  alpn2 = cf_hc_get_httpsrr_alpn(cf, data, alpn1);
  ಯದಿ(alpn2 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(source = "preferred version");
    alpn2 = cf_hc_get_pref_alpn(cf, data, alpn1);
  }
  ಯದಿ(alpn2 == ALPN_none) {
    VERBOSE(source = "wanted versions");
    alpn2 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.wanted,
                                 alpn1);
  }

  ಯದಿ(alpn2 != ALPN_none) {
    cf_hc_baller_assign(&ctx->ballers[1], alpn2, ctx->def_transport);
    ctx->baller_ಎಣಿಕೆ = 2;
    CURL_TRC_CF(data, cf, "2nd attempt uses %s from %s",
                ctx->ballers[1].name, source);
  }
  ctx->ballers_complete = TRUE;
}

ಸ್ಥಿರ CURLcode cf_hc_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                              ಸಂರಚನಾ Curl_easy *data,
                              bool *done)
{
  ಸಂರಚನಾ cf_hc_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ಧ್ರುವ ಸಂರಚನಾ curltime *pnow = ರಿಕ್ತ;

  ಯದಿ(cf->connected) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  *done = FALSE;

  ಯದಿ(!ctx->httpsrr_resolved) {
    ctx->httpsrr_resolved = Curl_conn_dns_resolved_https(
      data, cf->sockindex, ctx->destination);
#ifdef DEBUGBUILD
    ಯದಿ(!ctx->httpsrr_resolved && getenv("CURL_DBG_AWAIT_HTTPSRR")) {
      CURL_TRC_CF(data, cf, "awaiting HTTPS-RR");
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
#endif
  }

  ವಿಕಲ್ಪ(ctx->state) {
  ಪಕ್ಷ CF_HC_RESOLV:
    ctx->state = CF_HC_INIT;
    FALLTHROUGH();

  ಪಕ್ಷ CF_HC_INIT:
    DEBUGASSERT(!cf->next);
    CURL_TRC_CF(data, cf, "connect, init");
    result = cf_hc_set_baller1(cf, data);
    ಯದಿ(result) {
      ctx->result = result;
      ctx->state = CF_HC_FAILURE;
      ಗಚ್ಛ out;
    }
    cf_hc_set_baller2(cf, data);
    pnow = Curl_pgrs_now(data);
    ctx->started = *pnow;
    cf_hc_baller_init(&ctx->ballers[0], cf, data);
    ಯದಿ((ctx->baller_ಎಣಿಕೆ > 1) || !ctx->ballers_complete) {
      Curl_expire_set(data, EXPIRE_ALPN_EYEBALLS,
                      ctx->soft_eyeballs_timeout_ms, pnow);
    }
    ctx->state = CF_HC_CONNECT;
    FALLTHROUGH();

  ಪಕ್ಷ CF_HC_CONNECT:
    ಯದಿ(!ctx->ballers_complete)
      cf_hc_set_baller2(cf, data);

    ಯದಿ(cf_hc_baller_is_connecting(&ctx->ballers[0])) {
      result = cf_hc_baller_connect(&ctx->ballers[0], cf, data, done);
      ಯದಿ(!result && *done) {
        result = baller_connected(cf, data, &ctx->ballers[0]);
        ಗಚ್ಛ out;
      }
    }

    pnow = Curl_pgrs_now(data);
    ಯದಿ(time_to_start_baller2(cf, data, pnow)) {
      cf_hc_baller_init(&ctx->ballers[1], cf, data);
    }

    ಯದಿ(cf_hc_baller_is_connecting(&ctx->ballers[1])) {
      result = cf_hc_baller_connect(&ctx->ballers[1], cf, data, done);
      ಯದಿ(!result && *done) {
        result = baller_connected(cf, data, &ctx->ballers[1]);
        ಗಚ್ಛ out;
      }
    }

    ಯದಿ(ctx->ballers[0].result &&
       (ctx->ballers[1].result ||
        (ctx->ballers_complete && (ctx->baller_ಎಣಿಕೆ < 2)))) {
      /* all have failed. we give up */
      CURL_TRC_CF(data, cf, "connect, all attempts failed");
      ctx->result = result = ctx->ballers[0].result;
      ctx->state = CF_HC_FAILURE;
      ಗಚ್ಛ out;
    }
    result = CURLE_OK;
    *done = FALSE;
    ಭಙ್ಗ;

  ಪಕ್ಷ CF_HC_FAILURE:
    result = ctx->result;
    cf->connected = FALSE;
    *done = FALSE;
    ಭಙ್ಗ;

  ಪಕ್ಷ CF_HC_SUCCESS:
    result = CURLE_OK;
    cf->connected = TRUE;
    *done = TRUE;
    ಭಙ್ಗ;
  }

out:
  CURL_TRC_CF(data, cf, "connect -> %d, done=%d", (ಪೂರ್ಣಾಙ್ಕ)result, *done);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_hc_shutdown(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  ಸಂರಚನಾ cf_hc_ctx *ctx = cf->ctx;
  size_t i;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  ಯದಿ(cf->connected) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  ಪ್ರತಿ(i = 0; i < ctx->baller_ಎಣಿಕೆ; i++) {
    ಸಂರಚನಾ cf_hc_baller *b = &ctx->ballers[i];
    bool bdone = FALSE;
    ಯದಿ(!cf_hc_baller_is_connecting(b) || b->shutdown)
      ಅನುವರ್ತ;
    b->result = b->cf->cft->do_shutdown(b->cf, data, &bdone);
    ಯದಿ(b->result || bdone)
      b->shutdown = TRUE; /* treat a failed shutdown as done */
  }

  *done = TRUE;
  ಪ್ರತಿ(i = 0; i < ctx->baller_ಎಣಿಕೆ; i++) {
    ಯದಿ(!ctx->ballers[i].shutdown)
      *done = FALSE;
  }
  ಯದಿ(*done) {
    ಪ್ರತಿ(i = 0; i < ctx->baller_ಎಣಿಕೆ; i++) {
      ಯದಿ(ctx->ballers[i].result)
        result = ctx->ballers[i].result;
    }
  }
  CURL_TRC_CF(data, cf, "shutdown -> %d, done=%d", (ಪೂರ್ಣಾಙ್ಕ)result, *done);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_hc_adjust_pollset(ಸಂರಚನಾ Curl_cfilter *cf,
                                     ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  ಯದಿ(!cf->connected) {
    ಸಂರಚನಾ cf_hc_ctx *ctx = cf->ctx;
    size_t i;

    ಪ್ರತಿ(i = 0; (i < ctx->baller_ಎಣಿಕೆ) && !result; i++) {
      ಸಂರಚನಾ cf_hc_baller *b = &ctx->ballers[i];
      ಯದಿ(!cf_hc_baller_is_connecting(b))
        ಅನುವರ್ತ;
      result = Curl_conn_cf_adjust_pollset(b->cf, data, ps);
    }
    CURL_TRC_CF(data, cf, "adjust_pollset -> %d, %u socks", (ಪೂರ್ಣಾಙ್ಕ)result,
                ps->n);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ bool cf_hc_data_pending(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಧ್ರುವ ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  ಯದಿ(cf->connected)
    ಪ್ರತ್ಯಾಗಮ cf->next->cft->has_data_pending(cf->next, data);

  ಪ್ರತಿ(i = 0; i < ctx->baller_ಎಣಿಕೆ; i++)
    ಯದಿ(cf_hc_baller_data_pending(&ctx->ballers[i], data))
      ಪ್ರತ್ಯಾಗಮ TRUE;
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ CURLcode cf_hc_query(ಸಂರಚನಾ Curl_cfilter *cf,
                            ಸಂರಚನಾ Curl_easy *data,
                            ಪೂರ್ಣಾಙ್ಕ query, ಪೂರ್ಣಾಙ್ಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ಸಂರಚನಾ cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  ಯದಿ(!cf->connected) {
    ವಿಕಲ್ಪ(query) {
    ಪಕ್ಷ CF_QUERY_NEED_FLUSH: {
      ಪ್ರತಿ(i = 0; i < ctx->baller_ಎಣಿಕೆ; i++)
        ಯದಿ(cf_hc_baller_needs_flush(&ctx->ballers[i], data)) {
          *pres1 = TRUE;
          ಪ್ರತ್ಯಾಗಮ CURLE_OK;
        }
      ಭಙ್ಗ;
    }
    ಸ್ವಭಾವ:
      ಭಙ್ಗ;
    }
  }
  ಪ್ರತ್ಯಾಗಮ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ಸ್ಥಿರ CURLcode cf_hc_cntrl(ಸಂರಚನಾ Curl_cfilter *cf,
                            ಸಂರಚನಾ Curl_easy *data,
                            ಪೂರ್ಣಾಙ್ಕ event, ಪೂರ್ಣಾಙ್ಕ arg1, ಶೂನ್ಯ *arg2)
{
  ಸಂರಚನಾ cf_hc_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t i;

  ಯದಿ(!cf->connected) {
    ವಿಕಲ್ಪ(event) {
    ಪಕ್ಷ CF_CTRL_REPORT_STATS:
    ಪಕ್ಷ CF_CTRL_CONN_INFO_UPDATE:
      ಪ್ರತಿ(i = 0; i < ctx->baller_ಎಣಿಕೆ; i++) {
        /* Make the first baller that connected at network level report */
        ಯದಿ(Curl_conn_cf_is_ip_connected(ctx->ballers[i].cf, data)) {
          Curl_conn_cf_cntrl(ctx->ballers[i].cf, data, TRUE,
                             event, arg1, arg2);
          ಭಙ್ಗ;
        }
      }
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      ಪ್ರತಿ(i = 0; i < ctx->baller_ಎಣಿಕೆ; i++) {
        result = cf_hc_baller_cntrl(&ctx->ballers[i], data, event, arg1, arg2);
        ಯದಿ(result && (result != CURLE_AGAIN))
          ಗಚ್ಛ out;
      }
      result = CURLE_OK;
      ಭಙ್ಗ;
    }
  }
out:
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_hc_destroy(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_hc_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  cf_hc_ctx_destroy(data, ctx);
}

ಸಂರಚನಾ Curl_cftype Curl_cft_http_connect = {
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

ಸ್ಥಿರ CURLcode cf_hc_create(ಸಂರಚನಾ Curl_cfilter **pcf,
                             ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ Curl_peer *destination,
                             uint8_t def_transport)
{
  ಸಂರಚನಾ Curl_cfilter *cf = ರಿಕ್ತ;
  ಸಂರಚನಾ cf_hc_ctx *ctx;
  CURLcode result = CURLE_OK;

  ctx = curlx_calloc(1, ಪರಿಮಾಣ(*ctx));
  ಯದಿ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }
  Curl_peer_link(&ctx->destination, destination);
  ctx->def_transport = def_transport;
  ctx->hard_eyeballs_timeout_ms = data->set.happy_eyeballs_timeout;
  ctx->soft_eyeballs_timeout_ms = data->set.happy_eyeballs_timeout / 2;

  result = Curl_cf_create(&cf, &Curl_cft_http_connect, ctx);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  ctx = ರಿಕ್ತ;

out:
  *pcf = result ? ರಿಕ್ತ : cf;
  cf_hc_ctx_destroy(data, ctx);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_hc_add(ಸಂರಚನಾ Curl_easy *data,
                          ಸಂರಚನಾ Curl_peer *destination,
                          ಸಂರಚನಾ connectdata *conn,
                          int8_t sockindex,
                          uint8_t def_transport)
{
  ಸಂರಚನಾ Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_hc_create(&cf, data, destination, def_transport);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  Curl_conn_cf_add(data, conn, sockindex, cf);

#ifdef USE_HTTPSRR
  result = Curl_conn_dns_add_https_resolve(data, cf->conn, cf->sockindex,
                                           destination);
#endif
out:
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_cf_https_setup(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ Curl_peer *destination,
                             ಸಂರಚನಾ connectdata *conn,
                             int8_t sockindex)
{
  CURLcode result = CURLE_OK;

  DEBUGASSERT(conn->scheme->protocol == CURLPROTO_HTTPS);

  /* This filter is intended for HTTPS using ALPN and does
   * not support HTTPS Eyeballing to a proxy. */
  ಯದಿ((conn->scheme->protocol != CURLPROTO_HTTPS) ||
#ifndef CURL_DISABLE_PROXY
     conn->bits.origin_is_proxy ||
#endif
     !conn->bits.tls_enable_alpn)
    ಗಚ್ಛ out;

  result = cf_hc_add(data, destination, conn, sockindex,
                     conn->transport_wanted);

out:
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* !CURL_DISABLE_HTTP */
