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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "cf-setup.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "cf-https-connect.h"
#అన్తర్భావ "http2.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "vdns/cf-dns.h"
#అన్తర్భావ "vdns/hostip.h"
#అన్తర్భావ "vdns/httpsrr.h"
#అన్తర్భావ "vquic/vquic.h"

రకనిర్వచనం గణన {
  CF_HC_RESOLV,
  CF_HC_INIT,
  CF_HC_CONNECT,
  CF_HC_SUCCESS,
  CF_HC_FAILURE
} cf_hc_state;

నిర్మాణం cf_hc_baller {
  స్థిరరాశి అక్షరం *name;
  నిర్మాణం Curl_cfilter *cf;
  CURLcode ఫలితం;
  పూర్ణాంకం జవాబు_ms;
  uint8_t transport;
  గణన alpnid alpn_id;
  బిట్టు(shutdown);
};

స్థిర శూన్య cf_hc_baller_discard(నిర్మాణం cf_hc_baller *b,
                                 నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(b->cf) {
    Curl_conn_cf_discard_chain(&b->cf, దత్తాంశం);
    b->cf = ఖాళీ;
  }
}

స్థిర bool cf_hc_baller_is_connecting(నిర్మాణం cf_hc_baller *b)
{
  తిరిగి b->cf && !b->ఫలితం;
}

స్థిర bool cf_hc_baller_has_started(నిర్మాణం cf_hc_baller *b)
{
  తిరిగి !!b->cf;
}

స్థిర పూర్ణాంకం cf_hc_baller_జవాబు_ms(నిర్మాణం cf_hc_baller *b,
                                 నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(b->cf && (b->జవాబు_ms < 0))
    b->cf->cft->query(b->cf, దత్తాంశం, CF_QUERY_CONNECT_జవాబు_MS,
                      &b->జవాబు_ms, ఖాళీ);
  తిరిగి b->జవాబు_ms;
}

స్థిర bool cf_hc_baller_దత్తాంశం_pending(నిర్మాణం cf_hc_baller *b,
                                      స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం)
{
  తిరిగి b->cf && !b->ఫలితం && b->cf->cft->has_దత్తాంశం_pending(b->cf, దత్తాంశం);
}

స్థిర bool cf_hc_baller_needs_flush(నిర్మాణం cf_hc_baller *b,
                                     నిర్మాణం Curl_easy *దత్తాంశం)
{
  తిరిగి b->cf && !b->ఫలితం && Curl_conn_cf_needs_flush(b->cf, దత్తాంశం);
}

స్థిర CURLcode cf_hc_baller_cntrl(నిర్మాణం cf_hc_baller *b,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   పూర్ణాంకం event, పూర్ణాంకం arg1, శూన్య *arg2)
{
  ఒకవేళ(b->cf && !b->ఫలితం)
    తిరిగి Curl_conn_cf_cntrl(b->cf, దత్తాంశం, FALSE, event, arg1, arg2);
  తిరిగి CURLE_OK;
}

నిర్మాణం cf_hc_ctx {
  cf_hc_state state;
  నిర్మాణం Curl_peer *destination; /* who we ultimately want to talk to */
  నిర్మాణం curltime started;  /* when connect started */
  CURLcode ఫలితం;          /* overall result */
  CURLcode check_h3_ఫలితం;
  నిర్మాణం cf_hc_baller ballers[2];
  size_t baller_లెక్క;
  timediff_t soft_eyeballs_timeout_ms;
  timediff_t hard_eyeballs_timeout_ms;
  uint8_t def_transport;
  బిట్టు(httpsrr_resolved);
  బిట్టు(checked_h3);
  బిట్టు(ballers_complete);
};

స్థిర శూన్య cf_hc_ctx_destroy(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం cf_hc_ctx *ctx)
{
  ఒకవేళ(ctx) {
    size_t i;
    కొరకు(i = 0; i < ctx->baller_లెక్క; ++i)
      cf_hc_baller_discard(&ctx->ballers[i], దత్తాంశం);
    Curl_peer_unlink(&ctx->destination);
    curlx_free(ctx);
  }
}

స్థిర శూన్య cf_hc_baller_కేటాయించు(నిర్మాణం cf_hc_baller *b,
                                గణన alpnid alpn_id,
                                uint8_t def_transport)
{
  b->alpn_id = alpn_id;
  b->transport = def_transport;
  b->cf = ఖాళీ;
  b->ఫలితం = CURLE_OK;
  b->జవాబు_ms = -1;
  b->shutdown = FALSE;
  ఎంపిక(b->alpn_id) {
  స్థితి ALPN_h3:
    b->name = "h3";
    b->transport = TRNSPRT_QUIC;
    ఆపు;
  స్థితి ALPN_h2:
    b->name = "h2";
    ఆపు;
  స్థితి ALPN_h1:
    b->name = "h1";
    ఆపు;
  స్థితి ALPN_none:
    b->name = "no-alpn";
    ఆపు;
  అప్రమేయ:
    b->ఫలితం = CURLE_FAILED_INIT;
    ఆపు;
  }
}

స్థిర శూన్య cf_hc_baller_init(నిర్మాణం cf_hc_baller *b,
                              నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *save = cf->తదుపరి;

  cf->తదుపరి = ఖాళీ;
  b->ఫలితం = Curl_cf_అమరిక_చొప్పించు_after(cf, దత్తాంశం, b->transport,
                                         CURL_CF_SSL_ENABLE);
  b->cf = cf->తదుపరి;
  cf->తదుపరి = save;
}

స్థిర CURLcode cf_hc_baller_connect(నిర్మాణం cf_hc_baller *b,
                                     నిర్మాణం Curl_cfilter *cf,
                                     నిర్మాణం Curl_easy *దత్తాంశం,
                                     bool *పూర్తి)
{
  నిర్మాణం Curl_cfilter *save = cf->తదుపరి;

  cf->తదుపరి = b->cf;
  b->ఫలితం = Curl_conn_cf_connect(cf->తదుపరి, దత్తాంశం, పూర్తి);
  b->cf = cf->తదుపరి; /* it might mutate */
  cf->తదుపరి = save;
  తిరిగి b->ఫలితం;
}

స్థిర CURLcode baller_connected(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం cf_hc_baller *winner)
{
  నిర్మాణం cf_hc_ctx *ctx = cf->ctx;

  /* Make the winner's connection filter out own sub-filter, check, move,
   * close all remaining. */
  ఒకవేళ(cf->తదుపరి) {
    DEBUGASSERT(0);
    తిరిగి CURLE_FAILED_INIT;
  }
  ఒకవేళ(!winner->cf) {
    DEBUGASSERT(0);
    తిరిగి CURLE_FAILED_INIT;
  }

  cf->తదుపరి = winner->cf;
  winner->cf = ఖాళీ;
  ctx->state = CF_HC_SUCCESS;
  cf->connected = TRUE;

  /* ballers may have failf()'d, the winner resets it, so our
   * errorbuf is clean again. */
  Curl_reset_fail(దత్తాంశం);

#నిర్వచితమైతే USE_NGHTTP2
  {
    /* For a negotiated HTTP/2 connection insert the h2 filter. */
    స్థిరరాశి అక్షరం *alpn = Curl_conn_cf_get_alpn_negotiated(cf->తదుపరి, దత్తాంశం);
    ఒకవేళ(alpn && !strcmp("h2", alpn)) {
      CURLcode ఫలితం = Curl_http2_switch_at(cf, దత్తాంశం);
      ఒకవేళ(ఫలితం) {
        ctx->state = CF_HC_FAILURE;
        ctx->ఫలితం = ఫలితం;
        తిరిగి ఫలితం;
      }
    }
  }
#అయితే_ముగింపు
  తిరిగి CURLE_OK;
}

స్థిర bool సమయం_గమ్యం_ప్రారంభం_baller2(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  స్థిరరాశి నిర్మాణం curltime *pnow)
{
  నిర్మాణం cf_hc_ctx *ctx = cf->ctx;
  timediff_t elapsed_ms;

  ఒకవేళ(ctx->baller_లెక్క < 2)
    తిరిగి FALSE;
  లేకపోతే ఒకవేళ(cf_hc_baller_has_started(&ctx->ballers[1]))
    తిరిగి FALSE;
  లేకపోతే ఒకవేళ(ctx->ballers[0].ఫలితం) {
    CURL_TRC_CF(దత్తాంశం, cf, "%s baller failed, starting %s",
                ctx->ballers[0].name, ctx->ballers[1].name);
    తిరిగి TRUE;
  }

  elapsed_ms = curlx_ptimediff_ms(pnow, &ctx->started);
  ఒకవేళ(elapsed_ms >= ctx->hard_eyeballs_timeout_ms) {
    CURL_TRC_CF(దత్తాంశం, cf, "%s inconclusive after %" FMT_TIMEDIFF_T ", "
                "starting %s", ctx->ballers[0].name,
                ctx->hard_eyeballs_timeout_ms, ctx->ballers[1].name);
    తిరిగి TRUE;
  }
  లేకపోతే ఒకవేళ(elapsed_ms >= ctx->soft_eyeballs_timeout_ms) {
    ఒకవేళ(cf_hc_baller_జవాబు_ms(&ctx->ballers[0], దత్తాంశం) < 0) {
      CURL_TRC_CF(దత్తాంశం, cf, "%s has not seen any data after %"
                  FMT_TIMEDIFF_T "ms, starting %s",
                  ctx->ballers[0].name, ctx->soft_eyeballs_timeout_ms,
                  ctx->ballers[1].name);
      తిరిగి TRUE;
    }
    /* set the effective hard timeout again */
    Curl_expire_సమితి(దత్తాంశం, EXPIRE_ALPN_EYEBALLS,
                    ctx->hard_eyeballs_timeout_ms - elapsed_ms, pnow);
  }
  తిరిగి FALSE;
}

స్థిర bool cf_hc_may_h3(నిర్మాణం Curl_cfilter *cf,
                         నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_hc_ctx *ctx = cf->ctx;
  ఒకవేళ(!ctx->checked_h3) {
    ctx->check_h3_ఫలితం =
      Curl_conn_may_http3(దత్తాంశం, cf->conn);
    ctx->checked_h3 = TRUE;
  }
  తిరిగి !ctx->check_h3_ఫలితం;
}

స్థిర గణన alpnid cf_hc_get_httpsrr_alpn(నిర్మాణం Curl_cfilter *cf,
                                          నిర్మాణం Curl_easy *దత్తాంశం,
                                          గణన alpnid not_this_one)
{
#నిర్వచితమైతే USE_HTTPSRR
  నిర్మాణం cf_hc_ctx *ctx = cf->ctx;
  /* Is there an HTTPSRR use its ALPNs here.
   * We are here after having selected a connection to a host+port and
   * can no longer change that. Any HTTPSRR advice for other hosts and ports
   * we need to ignore. */
  స్థిరరాశి నిర్మాణం Curl_https_rrinfo *rr;
  size_t i;

  /* Do we have HTTPS-RR information? */
  rr = Curl_conn_dns_get_https(దత్తాంశం, cf->sockindex, ctx->destination);

  CURL_TRC_CF(దత్తాంశం, cf, "HTTPS-RR %savailable", rr ? "" : "not ");
  /* We do not support `rr->no_def_alpn`. */
  ఒకవేళ(Curl_httpsrr_applicable(దత్తాంశం, rr) && !rr->no_def_alpn) {
    కొరకు(i = 0; i < CURL_ARRAYSIZE(rr->alpns); ++i) {
      గణన alpnid alpn_rr = (గణన alpnid)rr->alpns[i];
      ఒకవేళ(alpn_rr == not_this_one) /* do not want this one */
        కొనసాగు;
      ఎంపిక(alpn_rr) {
      స్థితి ALPN_h3:
        ఒకవేళ((దత్తాంశం->state.http_neg.allowed & CURL_HTTP_V3x) &&
           cf_hc_may_h3(cf, దత్తాంశం)) {
          తిరిగి alpn_rr;
        }
        ఆపు;
      స్థితి ALPN_h2:
        ఒకవేళ(దత్తాంశం->state.http_neg.allowed & CURL_HTTP_V2x) {
          తిరిగి alpn_rr;
        }
        ఆపు;
      స్థితి ALPN_h1:
        ఒకవేళ(దత్తాంశం->state.http_neg.allowed & CURL_HTTP_V1x) {
          తిరిగి alpn_rr;
        }
        ఆపు;
      అప్రమేయ: /* ignore */
        ఆపు;
      }
    }
  }
#లేకపోతే
  (శూన్య)cf;
  (శూన్య)దత్తాంశం;
  (శూన్య)not_this_one;
#అయితే_ముగింపు
  తిరిగి ALPN_none;
}

స్థిర గణన alpnid cf_hc_get_pref_alpn(నిర్మాణం Curl_cfilter *cf,
                                       నిర్మాణం Curl_easy *దత్తాంశం,
                                       గణన alpnid not_this_one)
{
  ఒకవేళ((దత్తాంశం->state.http_neg.preferred & దత్తాంశం->state.http_neg.allowed)) {
    ఎంపిక(దత్తాంశం->state.http_neg.preferred) {
    స్థితి CURL_HTTP_V3x:
      ఒకవేళ(cf_hc_may_h3(cf, దత్తాంశం) && (ALPN_h3 != not_this_one))
        తిరిగి ALPN_h3;
      ఆపు;
    స్థితి CURL_HTTP_V2x:
      ఒకవేళ(ALPN_h2 != not_this_one)
        తిరిగి ALPN_h2;
      ఆపు;
    స్థితి CURL_HTTP_V1x:
      /* If we are trying h2 already, h1 is already used as fallback */
      ఒకవేళ((ALPN_h1 != not_this_one) && (ALPN_h2 != not_this_one))
        తిరిగి ALPN_h1;
      ఆపు;
    అప్రమేయ:
      ఆపు;
    }
  }
  తిరిగి ALPN_none;
}

స్థిర గణన alpnid cf_hc_get_మొదటి_alpn(నిర్మాణం Curl_cfilter *cf,
                                        నిర్మాణం Curl_easy *దత్తాంశం,
                                        http_majors choices,
                                        గణన alpnid not_this_one)
{
  /* When told to not try h2, we also do not try h1 and vice versa */
  bool allow_h1_or_h2 = (not_this_one != ALPN_h1) &&
                        (not_this_one != ALPN_h2);
  ఒకవేళ((ALPN_h3 != not_this_one) && (choices & CURL_HTTP_V3x) &&
     cf_hc_may_h3(cf, దత్తాంశం)) {
    తిరిగి ALPN_h3;
  }
  ఒకవేళ(allow_h1_or_h2 && (choices & CURL_HTTP_V2x)) {
    తిరిగి ALPN_h2;
  }
  ఒకవేళ(allow_h1_or_h2 && (choices & CURL_HTTP_V1x)) {
    తిరిగి ALPN_h1;
  }
  తిరిగి ALPN_none;
}

స్థిర CURLcode cf_hc_సమితి_baller1(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_hc_ctx *ctx = cf->ctx;
  గణన alpnid alpn1 = ALPN_none;
  VERBOSE(స్థిరరాశి అక్షరం *ఆధారం = "HTTPS-RR");

  DEBUGASSERT(cf->conn->బిట్లు.tls_enable_alpn);

  alpn1 = cf_hc_get_httpsrr_alpn(cf, దత్తాంశం, ALPN_none);
  ఒకవేళ(alpn1 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(ఆధారం = "preferred version");
    alpn1 = cf_hc_get_pref_alpn(cf, దత్తాంశం, ALPN_none);
  }
  ఒకవేళ(alpn1 == ALPN_none) {
    VERBOSE(ఆధారం = "wanted versions");
    alpn1 = cf_hc_get_మొదటి_alpn(cf, దత్తాంశం,
                                 దత్తాంశం->state.http_neg.wanted,
                                 ALPN_none);
  }
  ఒకవేళ(alpn1 == ALPN_none) {
    VERBOSE(ఆధారం = "allowed versions");
    alpn1 = cf_hc_get_మొదటి_alpn(cf, దత్తాంశం,
                                 దత్తాంశం->state.http_neg.allowed,
                                 ALPN_none);
  }

  ఒకవేళ(alpn1 == ALPN_none) {
    /* None of the wanted/allowed HTTP versions could be chosen */
    ఒకవేళ(ctx->check_h3_ఫలితం) {
      CURL_TRC_CF(దత్తాంశం, cf, "unable to use HTTP/3");
      తిరిగి ctx->check_h3_ఫలితం;
    }
    CURL_TRC_CF(దత్తాంశం, cf, "unable to select HTTP version");
    తిరిగి CURLE_FAILED_INIT;
  }

  cf_hc_baller_కేటాయించు(&ctx->ballers[0], alpn1, ctx->def_transport);
  ctx->baller_లెక్క = 1;
  CURL_TRC_CF(దత్తాంశం, cf, "1st attempt uses %s from %s",
              ctx->ballers[0].name, ఆధారం);

  ఎంపిక(alpn1) {
  స్థితి ALPN_h1:
    /* We really want h1, switch off h2 to make it disappear in ALPN */
    దత్తాంశం->state.http_neg.wanted &= (uint8_t)~CURL_HTTP_V2x;
    ఆపు;
  అప్రమేయ:
    ఆపు;
  }

  తిరిగి CURLE_OK;
}

స్థిర శూన్య cf_hc_సమితి_baller2(నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_hc_ctx *ctx = cf->ctx;
  గణన alpnid alpn2 = ALPN_none, alpn1 = ctx->ballers[0].alpn_id;
  VERBOSE(స్థిరరాశి అక్షరం *ఆధారం = "HTTPS-RR");

  ఒకవేళ(ctx->ballers_complete)
    తిరిగి; /* already done */
  ఒకవేళ(!ctx->httpsrr_resolved)
    తిరిగి; /* HTTPS-RR pending */

  alpn2 = cf_hc_get_httpsrr_alpn(cf, దత్తాంశం, alpn1);
  ఒకవేళ(alpn2 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(ఆధారం = "preferred version");
    alpn2 = cf_hc_get_pref_alpn(cf, దత్తాంశం, alpn1);
  }
  ఒకవేళ(alpn2 == ALPN_none) {
    VERBOSE(ఆధారం = "wanted versions");
    alpn2 = cf_hc_get_మొదటి_alpn(cf, దత్తాంశం,
                                 దత్తాంశం->state.http_neg.wanted,
                                 alpn1);
  }

  ఒకవేళ(alpn2 != ALPN_none) {
    cf_hc_baller_కేటాయించు(&ctx->ballers[1], alpn2, ctx->def_transport);
    ctx->baller_లెక్క = 2;
    CURL_TRC_CF(దత్తాంశం, cf, "2nd attempt uses %s from %s",
                ctx->ballers[1].name, ఆధారం);
  }
  ctx->ballers_complete = TRUE;
}

స్థిర CURLcode cf_hc_connect(నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *దత్తాంశం,
                              bool *పూర్తి)
{
  నిర్మాణం cf_hc_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి నిర్మాణం curltime *pnow = ఖాళీ;

  ఒకవేళ(cf->connected) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  *పూర్తి = FALSE;

  ఒకవేళ(!ctx->httpsrr_resolved) {
    ctx->httpsrr_resolved = Curl_conn_dns_resolved_https(
      దత్తాంశం, cf->sockindex, ctx->destination);
#నిర్వచితమైతే DEBUGBUILD
    ఒకవేళ(!ctx->httpsrr_resolved && getenv("CURL_DBG_AWAIT_HTTPSRR")) {
      CURL_TRC_CF(దత్తాంశం, cf, "awaiting HTTPS-RR");
      తిరిగి CURLE_OK;
    }
#అయితే_ముగింపు
  }

  ఎంపిక(ctx->state) {
  స్థితి CF_HC_RESOLV:
    ctx->state = CF_HC_INIT;
    FALLTHROUGH();

  స్థితి CF_HC_INIT:
    DEBUGASSERT(!cf->తదుపరి);
    CURL_TRC_CF(దత్తాంశం, cf, "connect, init");
    ఫలితం = cf_hc_సమితి_baller1(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం) {
      ctx->ఫలితం = ఫలితం;
      ctx->state = CF_HC_FAILURE;
      వెళ్ళు బహిర్గతం;
    }
    cf_hc_సమితి_baller2(cf, దత్తాంశం);
    pnow = Curl_pgrs_now(దత్తాంశం);
    ctx->started = *pnow;
    cf_hc_baller_init(&ctx->ballers[0], cf, దత్తాంశం);
    ఒకవేళ((ctx->baller_లెక్క > 1) || !ctx->ballers_complete) {
      Curl_expire_సమితి(దత్తాంశం, EXPIRE_ALPN_EYEBALLS,
                      ctx->soft_eyeballs_timeout_ms, pnow);
    }
    ctx->state = CF_HC_CONNECT;
    FALLTHROUGH();

  స్థితి CF_HC_CONNECT:
    ఒకవేళ(!ctx->ballers_complete)
      cf_hc_సమితి_baller2(cf, దత్తాంశం);

    ఒకవేళ(cf_hc_baller_is_connecting(&ctx->ballers[0])) {
      ఫలితం = cf_hc_baller_connect(&ctx->ballers[0], cf, దత్తాంశం, పూర్తి);
      ఒకవేళ(!ఫలితం && *పూర్తి) {
        ఫలితం = baller_connected(cf, దత్తాంశం, &ctx->ballers[0]);
        వెళ్ళు బహిర్గతం;
      }
    }

    pnow = Curl_pgrs_now(దత్తాంశం);
    ఒకవేళ(సమయం_గమ్యం_ప్రారంభం_baller2(cf, దత్తాంశం, pnow)) {
      cf_hc_baller_init(&ctx->ballers[1], cf, దత్తాంశం);
    }

    ఒకవేళ(cf_hc_baller_is_connecting(&ctx->ballers[1])) {
      ఫలితం = cf_hc_baller_connect(&ctx->ballers[1], cf, దత్తాంశం, పూర్తి);
      ఒకవేళ(!ఫలితం && *పూర్తి) {
        ఫలితం = baller_connected(cf, దత్తాంశం, &ctx->ballers[1]);
        వెళ్ళు బహిర్గతం;
      }
    }

    ఒకవేళ(ctx->ballers[0].ఫలితం &&
       (ctx->ballers[1].ఫలితం ||
        (ctx->ballers_complete && (ctx->baller_లెక్క < 2)))) {
      /* all have failed. we give up */
      CURL_TRC_CF(దత్తాంశం, cf, "connect, all attempts failed");
      ctx->ఫలితం = ఫలితం = ctx->ballers[0].ఫలితం;
      ctx->state = CF_HC_FAILURE;
      వెళ్ళు బహిర్గతం;
    }
    ఫలితం = CURLE_OK;
    *పూర్తి = FALSE;
    ఆపు;

  స్థితి CF_HC_FAILURE:
    ఫలితం = ctx->ఫలితం;
    cf->connected = FALSE;
    *పూర్తి = FALSE;
    ఆపు;

  స్థితి CF_HC_SUCCESS:
    ఫలితం = CURLE_OK;
    cf->connected = TRUE;
    *పూర్తి = TRUE;
    ఆపు;
  }

బహిర్గతం:
  CURL_TRC_CF(దత్తాంశం, cf, "connect -> %d, done=%d", (పూర్ణాంకం)ఫలితం, *పూర్తి);
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_hc_shutdown(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  నిర్మాణం cf_hc_ctx *ctx = cf->ctx;
  size_t i;
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(దత్తాంశం);
  ఒకవేళ(cf->connected) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  కొరకు(i = 0; i < ctx->baller_లెక్క; i++) {
    నిర్మాణం cf_hc_baller *b = &ctx->ballers[i];
    bool bdone = FALSE;
    ఒకవేళ(!cf_hc_baller_is_connecting(b) || b->shutdown)
      కొనసాగు;
    b->ఫలితం = b->cf->cft->do_shutdown(b->cf, దత్తాంశం, &bdone);
    ఒకవేళ(b->ఫలితం || bdone)
      b->shutdown = TRUE; /* treat a failed shutdown as done */
  }

  *పూర్తి = TRUE;
  కొరకు(i = 0; i < ctx->baller_లెక్క; i++) {
    ఒకవేళ(!ctx->ballers[i].shutdown)
      *పూర్తి = FALSE;
  }
  ఒకవేళ(*పూర్తి) {
    కొరకు(i = 0; i < ctx->baller_లెక్క; i++) {
      ఒకవేళ(ctx->ballers[i].ఫలితం)
        ఫలితం = ctx->ballers[i].ఫలితం;
    }
  }
  CURL_TRC_CF(దత్తాంశం, cf, "shutdown -> %d, done=%d", (పూర్ణాంకం)ఫలితం, *పూర్తి);
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_hc_adjust_pollset(నిర్మాణం Curl_cfilter *cf,
                                     నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం easy_pollset *ps)
{
  CURLcode ఫలితం = CURLE_OK;
  ఒకవేళ(!cf->connected) {
    నిర్మాణం cf_hc_ctx *ctx = cf->ctx;
    size_t i;

    కొరకు(i = 0; (i < ctx->baller_లెక్క) && !ఫలితం; i++) {
      నిర్మాణం cf_hc_baller *b = &ctx->ballers[i];
      ఒకవేళ(!cf_hc_baller_is_connecting(b))
        కొనసాగు;
      ఫలితం = Curl_conn_cf_adjust_pollset(b->cf, దత్తాంశం, ps);
    }
    CURL_TRC_CF(దత్తాంశం, cf, "adjust_pollset -> %d, %u socks", (పూర్ణాంకం)ఫలితం,
                ps->n);
  }
  తిరిగి ఫలితం;
}

స్థిర bool cf_hc_దత్తాంశం_pending(నిర్మాణం Curl_cfilter *cf,
                               స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  ఒకవేళ(cf->connected)
    తిరిగి cf->తదుపరి->cft->has_దత్తాంశం_pending(cf->తదుపరి, దత్తాంశం);

  కొరకు(i = 0; i < ctx->baller_లెక్క; i++)
    ఒకవేళ(cf_hc_baller_దత్తాంశం_pending(&ctx->ballers[i], దత్తాంశం))
      తిరిగి TRUE;
  తిరిగి FALSE;
}

స్థిర CURLcode cf_hc_query(నిర్మాణం Curl_cfilter *cf,
                            నిర్మాణం Curl_easy *దత్తాంశం,
                            పూర్ణాంకం query, పూర్ణాంకం *pres1, శూన్య *pres2)
{
  నిర్మాణం cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  ఒకవేళ(!cf->connected) {
    ఎంపిక(query) {
    స్థితి CF_QUERY_NEED_FLUSH: {
      కొరకు(i = 0; i < ctx->baller_లెక్క; i++)
        ఒకవేళ(cf_hc_baller_needs_flush(&ctx->ballers[i], దత్తాంశం)) {
          *pres1 = TRUE;
          తిరిగి CURLE_OK;
        }
      ఆపు;
    }
    అప్రమేయ:
      ఆపు;
    }
  }
  తిరిగి cf->తదుపరి ?
    cf->తదుపరి->cft->query(cf->తదుపరి, దత్తాంశం, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

స్థిర CURLcode cf_hc_cntrl(నిర్మాణం Curl_cfilter *cf,
                            నిర్మాణం Curl_easy *దత్తాంశం,
                            పూర్ణాంకం event, పూర్ణాంకం arg1, శూన్య *arg2)
{
  నిర్మాణం cf_hc_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;
  size_t i;

  ఒకవేళ(!cf->connected) {
    ఎంపిక(event) {
    స్థితి CF_CTRL_REPORT_STATS:
    స్థితి CF_CTRL_CONN_INFO_UPDATE:
      కొరకు(i = 0; i < ctx->baller_లెక్క; i++) {
        /* Make the first baller that connected at network level report */
        ఒకవేళ(Curl_conn_cf_is_ip_connected(ctx->ballers[i].cf, దత్తాంశం)) {
          Curl_conn_cf_cntrl(ctx->ballers[i].cf, దత్తాంశం, TRUE,
                             event, arg1, arg2);
          ఆపు;
        }
      }
      ఆపు;
    అప్రమేయ:
      కొరకు(i = 0; i < ctx->baller_లెక్క; i++) {
        ఫలితం = cf_hc_baller_cntrl(&ctx->ballers[i], దత్తాంశం, event, arg1, arg2);
        ఒకవేళ(ఫలితం && (ఫలితం != CURLE_AGAIN))
          వెళ్ళు బహిర్గతం;
      }
      ఫలితం = CURLE_OK;
      ఆపు;
    }
  }
బహిర్గతం:
  తిరిగి ఫలితం;
}

స్థిర శూన్య cf_hc_destroy(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_hc_ctx *ctx = cf->ctx;

  CURL_TRC_CF(దత్తాంశం, cf, "destroy");
  cf_hc_ctx_destroy(దత్తాంశం, ctx);
}

నిర్మాణం Curl_cftype Curl_cft_http_connect = {
  "HTTPS-CONNECT",
  CF_TYPE_అమరిక,
  CURL_LOG_LVL_NONE,
  cf_hc_destroy,
  cf_hc_connect,
  cf_hc_shutdown,
  cf_hc_adjust_pollset,
  cf_hc_దత్తాంశం_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  cf_hc_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_hc_query,
};

స్థిర CURLcode cf_hc_create(నిర్మాణం Curl_cfilter **pcf,
                             నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం Curl_peer *destination,
                             uint8_t def_transport)
{
  నిర్మాణం Curl_cfilter *cf = ఖాళీ;
  నిర్మాణం cf_hc_ctx *ctx;
  CURLcode ఫలితం = CURLE_OK;

  ctx = curlx_calloc(1, పరిమాణం(*ctx));
  ఒకవేళ(!ctx) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }
  Curl_peer_link(&ctx->destination, destination);
  ctx->def_transport = def_transport;
  ctx->hard_eyeballs_timeout_ms = దత్తాంశం->సమితి.happy_eyeballs_timeout;
  ctx->soft_eyeballs_timeout_ms = దత్తాంశం->సమితి.happy_eyeballs_timeout / 2;

  ఫలితం = Curl_cf_create(&cf, &Curl_cft_http_connect, ctx);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  ctx = ఖాళీ;

బహిర్గతం:
  *pcf = ఫలితం ? ఖాళీ : cf;
  cf_hc_ctx_destroy(దత్తాంశం, ctx);
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_hc_కూడు(నిర్మాణం Curl_easy *దత్తాంశం,
                          నిర్మాణం Curl_peer *destination,
                          నిర్మాణం connectdata *conn,
                          int8_t sockindex,
                          uint8_t def_transport)
{
  నిర్మాణం Curl_cfilter *cf;
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(దత్తాంశం);
  ఫలితం = cf_hc_create(&cf, దత్తాంశం, destination, def_transport);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  Curl_conn_cf_కూడు(దత్తాంశం, conn, sockindex, cf);

#నిర్వచితమైతే USE_HTTPSRR
  ఫలితం = Curl_conn_dns_కూడు_https_resolve(దత్తాంశం, cf->conn, cf->sockindex,
                                           destination);
#అయితే_ముగింపు
బహిర్గతం:
  తిరిగి ఫలితం;
}

CURLcode Curl_cf_https_అమరిక(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం Curl_peer *destination,
                             నిర్మాణం connectdata *conn,
                             int8_t sockindex)
{
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(conn->scheme->protocol == CURLPROTO_HTTPS);

  /* This filter is intended for HTTPS using ALPN and does
   * not support HTTPS Eyeballing to a proxy. */
  ఒకవేళ((conn->scheme->protocol != CURLPROTO_HTTPS) ||
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
     conn->బిట్లు.origin_is_proxy ||
#అయితే_ముగింపు
     !conn->బిట్లు.tls_enable_alpn)
    వెళ్ళు బహిర్గతం;

  ఫలితం = cf_hc_కూడు(దత్తాంశం, destination, conn, sockindex,
                     conn->transport_wanted);

బహిర్గతం:
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* !CURL_DISABLE_HTTP */
