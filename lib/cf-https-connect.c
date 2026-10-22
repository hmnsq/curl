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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "cf-setup.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "cf-https-connect.h"
#ଅନ୍ତର୍ଭାଵ "http2.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "vdns/cf-dns.h"
#ଅନ୍ତର୍ଭାଵ "vdns/hostip.h"
#ଅନ୍ତର୍ଭାଵ "vdns/httpsrr.h"
#ଅନ୍ତର୍ଭାଵ "vquic/vquic.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  CF_HC_RESOLV,
  CF_HC_INIT,
  CF_HC_CONNECT,
  CF_HC_SUCCESS,
  CF_HC_FAILURE
} cf_hc_state;

ସଂରଚନା cf_hc_baller {
  ଅଚଳ ଅକ୍ଷର *name;
  ସଂରଚନା Curl_cfilter *cf;
  CURLcode result;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା reply_ms;
  uint8_t transport;
  ଗଣନା alpnid alpn_id;
  BIT(shutdown);
};

ସ୍ଥିର ଶୂନ୍ୟ cf_hc_baller_discard(ସଂରଚନା cf_hc_baller *b,
                                 ସଂରଚନା Curl_easy *data)
{
  ଯଦି(b->cf) {
    Curl_conn_cf_discard_chain(&b->cf, data);
    b->cf = ଖାଲି;
  }
}

ସ୍ଥିର bool cf_hc_baller_is_connecting(ସଂରଚନା cf_hc_baller *b)
{
  ଫେରନ୍ତୁ b->cf && !b->result;
}

ସ୍ଥିର bool cf_hc_baller_has_started(ସଂରଚନା cf_hc_baller *b)
{
  ଫେରନ୍ତୁ !!b->cf;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cf_hc_baller_reply_ms(ସଂରଚନା cf_hc_baller *b,
                                 ସଂରଚନା Curl_easy *data)
{
  ଯଦି(b->cf && (b->reply_ms < 0))
    b->cf->cft->query(b->cf, data, CF_QUERY_CONNECT_REPLY_MS,
                      &b->reply_ms, ଖାଲି);
  ଫେରନ୍ତୁ b->reply_ms;
}

ସ୍ଥିର bool cf_hc_baller_data_pending(ସଂରଚନା cf_hc_baller *b,
                                      ଅଚଳ ସଂରଚନା Curl_easy *data)
{
  ଫେରନ୍ତୁ b->cf && !b->result && b->cf->cft->has_data_pending(b->cf, data);
}

ସ୍ଥିର bool cf_hc_baller_needs_flush(ସଂରଚନା cf_hc_baller *b,
                                     ସଂରଚନା Curl_easy *data)
{
  ଫେରନ୍ତୁ b->cf && !b->result && Curl_conn_cf_needs_flush(b->cf, data);
}

ସ୍ଥିର CURLcode cf_hc_baller_cntrl(ସଂରଚନା cf_hc_baller *b,
                                   ସଂରଚନା Curl_easy *data,
                                   ପୂର୍ଣ୍ଣସଂଖ୍ୟା event, ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg1, ଶୂନ୍ୟ *arg2)
{
  ଯଦି(b->cf && !b->result)
    ଫେରନ୍ତୁ Curl_conn_cf_cntrl(b->cf, data, FALSE, event, arg1, arg2);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସଂରଚନା cf_hc_ctx {
  cf_hc_state state;
  ସଂରଚନା Curl_peer *destination; /* who we ultimately want to talk to */
  ସଂରଚନା curltime started;  /* when connect started */
  CURLcode result;          /* overall result */
  CURLcode check_h3_result;
  ସଂରଚନା cf_hc_baller ballers[2];
  size_t baller_ଗଣନା;
  timediff_t soft_eyeballs_timeout_ms;
  timediff_t hard_eyeballs_timeout_ms;
  uint8_t def_transport;
  BIT(httpsrr_resolved);
  BIT(checked_h3);
  BIT(ballers_complete);
};

ସ୍ଥିର ଶୂନ୍ୟ cf_hc_ctx_destroy(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା cf_hc_ctx *ctx)
{
  ଯଦି(ctx) {
    size_t i;
    ପାଇଁ(i = 0; i < ctx->baller_ଗଣନା; ++i)
      cf_hc_baller_discard(&ctx->ballers[i], data);
    Curl_peer_unlink(&ctx->destination);
    curlx_free(ctx);
  }
}

ସ୍ଥିର ଶୂନ୍ୟ cf_hc_baller_assign(ସଂରଚନା cf_hc_baller *b,
                                ଗଣନା alpnid alpn_id,
                                uint8_t def_transport)
{
  b->alpn_id = alpn_id;
  b->transport = def_transport;
  b->cf = ଖାଲି;
  b->result = CURLE_OK;
  b->reply_ms = -1;
  b->shutdown = FALSE;
  ବାଛ(b->alpn_id) {
  ସ୍ଥିତି ALPN_h3:
    b->name = "h3";
    b->transport = TRNSPRT_QUIC;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି ALPN_h2:
    b->name = "h2";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି ALPN_h1:
    b->name = "h1";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି ALPN_none:
    b->name = "no-alpn";
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    b->result = CURLE_FAILED_INIT;
    ଭାଙ୍ଗ;
  }
}

ସ୍ଥିର ଶୂନ୍ୟ cf_hc_baller_init(ସଂରଚନା cf_hc_baller *b,
                              ସଂରଚନା Curl_cfilter *cf,
                              ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_cfilter *save = cf->next;

  cf->next = ଖାଲି;
  b->result = Curl_cf_setup_ଭର୍ତ୍ତି_after(cf, data, b->transport,
                                         CURL_CF_SSL_ENABLE);
  b->cf = cf->next;
  cf->next = save;
}

ସ୍ଥିର CURLcode cf_hc_baller_connect(ସଂରଚନା cf_hc_baller *b,
                                     ସଂରଚନା Curl_cfilter *cf,
                                     ସଂରଚନା Curl_easy *data,
                                     bool *done)
{
  ସଂରଚନା Curl_cfilter *save = cf->next;

  cf->next = b->cf;
  b->result = Curl_conn_cf_connect(cf->next, data, done);
  b->cf = cf->next; /* it might mutate */
  cf->next = save;
  ଫେରନ୍ତୁ b->result;
}

ସ୍ଥିର CURLcode baller_connected(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା cf_hc_baller *winner)
{
  ସଂରଚନା cf_hc_ctx *ctx = cf->ctx;

  /* Make the winner's connection filter out own sub-filter, check, move,
   * close all remaining. */
  ଯଦି(cf->next) {
    DEBUGASSERT(0);
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }
  ଯଦି(!winner->cf) {
    DEBUGASSERT(0);
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }

  cf->next = winner->cf;
  winner->cf = ଖାଲି;
  ctx->state = CF_HC_SUCCESS;
  cf->connected = TRUE;

  /* ballers may have failf()'d, the winner resets it, so our
   * errorbuf is clean again. */
  Curl_reset_fail(data);

#ifdef USE_NGHTTP2
  {
    /* For a negotiated HTTP/2 connection insert the h2 filter. */
    ଅଚଳ ଅକ୍ଷର *alpn = Curl_conn_cf_get_alpn_negotiated(cf->next, data);
    ଯଦି(alpn && !strcmp("h2", alpn)) {
      CURLcode result = Curl_http2_switch_at(cf, data);
      ଯଦି(result) {
        ctx->state = CF_HC_FAILURE;
        ctx->result = result;
        ଫେରନ୍ତୁ result;
      }
    }
  }
#endif
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର bool time_to_start_baller2(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data,
                                  ଅଚଳ ସଂରଚନା curltime *pnow)
{
  ସଂରଚନା cf_hc_ctx *ctx = cf->ctx;
  timediff_t elapsed_ms;

  ଯଦି(ctx->baller_ଗଣନା < 2)
    ଫେରନ୍ତୁ FALSE;
  ଅନ୍ୟଥା ଯଦି(cf_hc_baller_has_started(&ctx->ballers[1]))
    ଫେରନ୍ତୁ FALSE;
  ଅନ୍ୟଥା ଯଦି(ctx->ballers[0].result) {
    CURL_TRC_CF(data, cf, "%s baller failed, starting %s",
                ctx->ballers[0].name, ctx->ballers[1].name);
    ଫେରନ୍ତୁ TRUE;
  }

  elapsed_ms = curlx_ptimediff_ms(pnow, &ctx->started);
  ଯଦି(elapsed_ms >= ctx->hard_eyeballs_timeout_ms) {
    CURL_TRC_CF(data, cf, "%s inconclusive after %" FMT_TIMEDIFF_T ", "
                "starting %s", ctx->ballers[0].name,
                ctx->hard_eyeballs_timeout_ms, ctx->ballers[1].name);
    ଫେରନ୍ତୁ TRUE;
  }
  ଅନ୍ୟଥା ଯଦି(elapsed_ms >= ctx->soft_eyeballs_timeout_ms) {
    ଯଦି(cf_hc_baller_reply_ms(&ctx->ballers[0], data) < 0) {
      CURL_TRC_CF(data, cf, "%s has not seen any data after %"
                  FMT_TIMEDIFF_T "ms, starting %s",
                  ctx->ballers[0].name, ctx->soft_eyeballs_timeout_ms,
                  ctx->ballers[1].name);
      ଫେରନ୍ତୁ TRUE;
    }
    /* set the effective hard timeout again */
    Curl_expire_set(data, EXPIRE_ALPN_EYEBALLS,
                    ctx->hard_eyeballs_timeout_ms - elapsed_ms, pnow);
  }
  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର bool cf_hc_may_h3(ସଂରଚନା Curl_cfilter *cf,
                         ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_hc_ctx *ctx = cf->ctx;
  ଯଦି(!ctx->checked_h3) {
    ctx->check_h3_result =
      Curl_conn_may_http3(data, cf->conn);
    ctx->checked_h3 = TRUE;
  }
  ଫେରନ୍ତୁ !ctx->check_h3_result;
}

ସ୍ଥିର ଗଣନା alpnid cf_hc_get_httpsrr_alpn(ସଂରଚନା Curl_cfilter *cf,
                                          ସଂରଚନା Curl_easy *data,
                                          ଗଣନା alpnid not_this_one)
{
#ifdef USE_HTTPSRR
  ସଂରଚନା cf_hc_ctx *ctx = cf->ctx;
  /* Is there an HTTPSRR use its ALPNs here.
   * We are here after having selected a connection to a host+port and
   * can no longer change that. Any HTTPSRR advice for other hosts and ports
   * we need to ignore. */
  ଅଚଳ ସଂରଚନା Curl_https_rrinfo *rr;
  size_t i;

  /* Do we have HTTPS-RR information? */
  rr = Curl_conn_dns_get_https(data, cf->sockindex, ctx->destination);

  CURL_TRC_CF(data, cf, "HTTPS-RR %savailable", rr ? "" : "not ");
  /* We do not support `rr->no_def_alpn`. */
  ଯଦି(Curl_httpsrr_applicable(data, rr) && !rr->no_def_alpn) {
    ପାଇଁ(i = 0; i < CURL_ARRAYSIZE(rr->alpns); ++i) {
      ଗଣନା alpnid alpn_rr = (ଗଣନା alpnid)rr->alpns[i];
      ଯଦି(alpn_rr == not_this_one) /* do not want this one */
        ଜାରି;
      ବାଛ(alpn_rr) {
      ସ୍ଥିତି ALPN_h3:
        ଯଦି((data->state.http_neg.allowed & CURL_HTTP_V3x) &&
           cf_hc_may_h3(cf, data)) {
          ଫେରନ୍ତୁ alpn_rr;
        }
        ଭାଙ୍ଗ;
      ସ୍ଥିତି ALPN_h2:
        ଯଦି(data->state.http_neg.allowed & CURL_HTTP_V2x) {
          ଫେରନ୍ତୁ alpn_rr;
        }
        ଭାଙ୍ଗ;
      ସ୍ଥିତି ALPN_h1:
        ଯଦି(data->state.http_neg.allowed & CURL_HTTP_V1x) {
          ଫେରନ୍ତୁ alpn_rr;
        }
        ଭାଙ୍ଗ;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ: /* ignore */
        ଭାଙ୍ଗ;
      }
    }
  }
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)cf;
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)not_this_one;
#endif
  ଫେରନ୍ତୁ ALPN_none;
}

ସ୍ଥିର ଗଣନା alpnid cf_hc_get_pref_alpn(ସଂରଚନା Curl_cfilter *cf,
                                       ସଂରଚନା Curl_easy *data,
                                       ଗଣନା alpnid not_this_one)
{
  ଯଦି((data->state.http_neg.preferred & data->state.http_neg.allowed)) {
    ବାଛ(data->state.http_neg.preferred) {
    ସ୍ଥିତି CURL_HTTP_V3x:
      ଯଦି(cf_hc_may_h3(cf, data) && (ALPN_h3 != not_this_one))
        ଫେରନ୍ତୁ ALPN_h3;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CURL_HTTP_V2x:
      ଯଦି(ALPN_h2 != not_this_one)
        ଫେରନ୍ତୁ ALPN_h2;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CURL_HTTP_V1x:
      /* If we are trying h2 already, h1 is already used as fallback */
      ଯଦି((ALPN_h1 != not_this_one) && (ALPN_h2 != not_this_one))
        ଫେରନ୍ତୁ ALPN_h1;
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଭାଙ୍ଗ;
    }
  }
  ଫେରନ୍ତୁ ALPN_none;
}

ସ୍ଥିର ଗଣନା alpnid cf_hc_get_first_alpn(ସଂରଚନା Curl_cfilter *cf,
                                        ସଂରଚନା Curl_easy *data,
                                        http_majors choices,
                                        ଗଣନା alpnid not_this_one)
{
  /* When told to not try h2, we also do not try h1 and vice versa */
  bool allow_h1_or_h2 = (not_this_one != ALPN_h1) &&
                        (not_this_one != ALPN_h2);
  ଯଦି((ALPN_h3 != not_this_one) && (choices & CURL_HTTP_V3x) &&
     cf_hc_may_h3(cf, data)) {
    ଫେରନ୍ତୁ ALPN_h3;
  }
  ଯଦି(allow_h1_or_h2 && (choices & CURL_HTTP_V2x)) {
    ଫେରନ୍ତୁ ALPN_h2;
  }
  ଯଦି(allow_h1_or_h2 && (choices & CURL_HTTP_V1x)) {
    ଫେରନ୍ତୁ ALPN_h1;
  }
  ଫେରନ୍ତୁ ALPN_none;
}

ସ୍ଥିର CURLcode cf_hc_set_baller1(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_hc_ctx *ctx = cf->ctx;
  ଗଣନା alpnid alpn1 = ALPN_none;
  VERBOSE(ଅଚଳ ଅକ୍ଷର *source = "HTTPS-RR");

  DEBUGASSERT(cf->conn->bits.tls_enable_alpn);

  alpn1 = cf_hc_get_httpsrr_alpn(cf, data, ALPN_none);
  ଯଦି(alpn1 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(source = "preferred version");
    alpn1 = cf_hc_get_pref_alpn(cf, data, ALPN_none);
  }
  ଯଦି(alpn1 == ALPN_none) {
    VERBOSE(source = "wanted versions");
    alpn1 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.wanted,
                                 ALPN_none);
  }
  ଯଦି(alpn1 == ALPN_none) {
    VERBOSE(source = "allowed versions");
    alpn1 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.allowed,
                                 ALPN_none);
  }

  ଯଦି(alpn1 == ALPN_none) {
    /* None of the wanted/allowed HTTP versions could be chosen */
    ଯଦି(ctx->check_h3_result) {
      CURL_TRC_CF(data, cf, "unable to use HTTP/3");
      ଫେରନ୍ତୁ ctx->check_h3_result;
    }
    CURL_TRC_CF(data, cf, "unable to select HTTP version");
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }

  cf_hc_baller_assign(&ctx->ballers[0], alpn1, ctx->def_transport);
  ctx->baller_ଗଣନା = 1;
  CURL_TRC_CF(data, cf, "1st attempt uses %s from %s",
              ctx->ballers[0].name, source);

  ବାଛ(alpn1) {
  ସ୍ଥିତି ALPN_h1:
    /* We really want h1, switch off h2 to make it disappear in ALPN */
    data->state.http_neg.wanted &= (uint8_t)~CURL_HTTP_V2x;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_hc_set_baller2(ସଂରଚନା Curl_cfilter *cf,
                              ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_hc_ctx *ctx = cf->ctx;
  ଗଣନା alpnid alpn2 = ALPN_none, alpn1 = ctx->ballers[0].alpn_id;
  VERBOSE(ଅଚଳ ଅକ୍ଷର *source = "HTTPS-RR");

  ଯଦି(ctx->ballers_complete)
    ଫେରନ୍ତୁ; /* already done */
  ଯଦି(!ctx->httpsrr_resolved)
    ଫେରନ୍ତୁ; /* HTTPS-RR pending */

  alpn2 = cf_hc_get_httpsrr_alpn(cf, data, alpn1);
  ଯଦି(alpn2 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(source = "preferred version");
    alpn2 = cf_hc_get_pref_alpn(cf, data, alpn1);
  }
  ଯଦି(alpn2 == ALPN_none) {
    VERBOSE(source = "wanted versions");
    alpn2 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.wanted,
                                 alpn1);
  }

  ଯଦି(alpn2 != ALPN_none) {
    cf_hc_baller_assign(&ctx->ballers[1], alpn2, ctx->def_transport);
    ctx->baller_ଗଣନା = 2;
    CURL_TRC_CF(data, cf, "2nd attempt uses %s from %s",
                ctx->ballers[1].name, source);
  }
  ctx->ballers_complete = TRUE;
}

ସ୍ଥିର CURLcode cf_hc_connect(ସଂରଚନା Curl_cfilter *cf,
                              ସଂରଚନା Curl_easy *data,
                              bool *done)
{
  ସଂରଚନା cf_hc_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ଅଚଳ ସଂରଚନା curltime *pnow = ଖାଲି;

  ଯଦି(cf->connected) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  *done = FALSE;

  ଯଦି(!ctx->httpsrr_resolved) {
    ctx->httpsrr_resolved = Curl_conn_dns_resolved_https(
      data, cf->sockindex, ctx->destination);
#ifdef DEBUGBUILD
    ଯଦି(!ctx->httpsrr_resolved && getenv("CURL_DBG_AWAIT_HTTPSRR")) {
      CURL_TRC_CF(data, cf, "awaiting HTTPS-RR");
      ଫେରନ୍ତୁ CURLE_OK;
    }
#endif
  }

  ବାଛ(ctx->state) {
  ସ୍ଥିତି CF_HC_RESOLV:
    ctx->state = CF_HC_INIT;
    FALLTHROUGH();

  ସ୍ଥିତି CF_HC_INIT:
    DEBUGASSERT(!cf->next);
    CURL_TRC_CF(data, cf, "connect, init");
    result = cf_hc_set_baller1(cf, data);
    ଯଦି(result) {
      ctx->result = result;
      ctx->state = CF_HC_FAILURE;
      ଯାଅ out;
    }
    cf_hc_set_baller2(cf, data);
    pnow = Curl_pgrs_now(data);
    ctx->started = *pnow;
    cf_hc_baller_init(&ctx->ballers[0], cf, data);
    ଯଦି((ctx->baller_ଗଣନା > 1) || !ctx->ballers_complete) {
      Curl_expire_set(data, EXPIRE_ALPN_EYEBALLS,
                      ctx->soft_eyeballs_timeout_ms, pnow);
    }
    ctx->state = CF_HC_CONNECT;
    FALLTHROUGH();

  ସ୍ଥିତି CF_HC_CONNECT:
    ଯଦି(!ctx->ballers_complete)
      cf_hc_set_baller2(cf, data);

    ଯଦି(cf_hc_baller_is_connecting(&ctx->ballers[0])) {
      result = cf_hc_baller_connect(&ctx->ballers[0], cf, data, done);
      ଯଦି(!result && *done) {
        result = baller_connected(cf, data, &ctx->ballers[0]);
        ଯାଅ out;
      }
    }

    pnow = Curl_pgrs_now(data);
    ଯଦି(time_to_start_baller2(cf, data, pnow)) {
      cf_hc_baller_init(&ctx->ballers[1], cf, data);
    }

    ଯଦି(cf_hc_baller_is_connecting(&ctx->ballers[1])) {
      result = cf_hc_baller_connect(&ctx->ballers[1], cf, data, done);
      ଯଦି(!result && *done) {
        result = baller_connected(cf, data, &ctx->ballers[1]);
        ଯାଅ out;
      }
    }

    ଯଦି(ctx->ballers[0].result &&
       (ctx->ballers[1].result ||
        (ctx->ballers_complete && (ctx->baller_ଗଣନା < 2)))) {
      /* all have failed. we give up */
      CURL_TRC_CF(data, cf, "connect, all attempts failed");
      ctx->result = result = ctx->ballers[0].result;
      ctx->state = CF_HC_FAILURE;
      ଯାଅ out;
    }
    result = CURLE_OK;
    *done = FALSE;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CF_HC_FAILURE:
    result = ctx->result;
    cf->connected = FALSE;
    *done = FALSE;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CF_HC_SUCCESS:
    result = CURLE_OK;
    cf->connected = TRUE;
    *done = TRUE;
    ଭାଙ୍ଗ;
  }

out:
  CURL_TRC_CF(data, cf, "connect -> %d, done=%d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *done);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_hc_shutdown(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data, bool *done)
{
  ସଂରଚନା cf_hc_ctx *ctx = cf->ctx;
  size_t i;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  ଯଦି(cf->connected) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  ପାଇଁ(i = 0; i < ctx->baller_ଗଣନା; i++) {
    ସଂରଚନା cf_hc_baller *b = &ctx->ballers[i];
    bool bdone = FALSE;
    ଯଦି(!cf_hc_baller_is_connecting(b) || b->shutdown)
      ଜାରି;
    b->result = b->cf->cft->do_shutdown(b->cf, data, &bdone);
    ଯଦି(b->result || bdone)
      b->shutdown = TRUE; /* treat a failed shutdown as done */
  }

  *done = TRUE;
  ପାଇଁ(i = 0; i < ctx->baller_ଗଣନା; i++) {
    ଯଦି(!ctx->ballers[i].shutdown)
      *done = FALSE;
  }
  ଯଦି(*done) {
    ପାଇଁ(i = 0; i < ctx->baller_ଗଣନା; i++) {
      ଯଦି(ctx->ballers[i].result)
        result = ctx->ballers[i].result;
    }
  }
  CURL_TRC_CF(data, cf, "shutdown -> %d, done=%d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *done);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_hc_adjust_pollset(ସଂରଚନା Curl_cfilter *cf,
                                     ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  ଯଦି(!cf->connected) {
    ସଂରଚନା cf_hc_ctx *ctx = cf->ctx;
    size_t i;

    ପାଇଁ(i = 0; (i < ctx->baller_ଗଣନା) && !result; i++) {
      ସଂରଚନା cf_hc_baller *b = &ctx->ballers[i];
      ଯଦି(!cf_hc_baller_is_connecting(b))
        ଜାରି;
      result = Curl_conn_cf_adjust_pollset(b->cf, data, ps);
    }
    CURL_TRC_CF(data, cf, "adjust_pollset -> %d, %u socks", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result,
                ps->n);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର bool cf_hc_data_pending(ସଂରଚନା Curl_cfilter *cf,
                               ଅଚଳ ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  ଯଦି(cf->connected)
    ଫେରନ୍ତୁ cf->next->cft->has_data_pending(cf->next, data);

  ପାଇଁ(i = 0; i < ctx->baller_ଗଣନା; i++)
    ଯଦି(cf_hc_baller_data_pending(&ctx->ballers[i], data))
      ଫେରନ୍ତୁ TRUE;
  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର CURLcode cf_hc_query(ସଂରଚନା Curl_cfilter *cf,
                            ସଂରଚନା Curl_easy *data,
                            ପୂର୍ଣ୍ଣସଂଖ୍ୟା query, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pres1, ଶୂନ୍ୟ *pres2)
{
  ସଂରଚନା cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  ଯଦି(!cf->connected) {
    ବାଛ(query) {
    ସ୍ଥିତି CF_QUERY_NEED_FLUSH: {
      ପାଇଁ(i = 0; i < ctx->baller_ଗଣନା; i++)
        ଯଦି(cf_hc_baller_needs_flush(&ctx->ballers[i], data)) {
          *pres1 = TRUE;
          ଫେରନ୍ତୁ CURLE_OK;
        }
      ଭାଙ୍ଗ;
    }
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଭାଙ୍ଗ;
    }
  }
  ଫେରନ୍ତୁ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ସ୍ଥିର CURLcode cf_hc_cntrl(ସଂରଚନା Curl_cfilter *cf,
                            ସଂରଚନା Curl_easy *data,
                            ପୂର୍ଣ୍ଣସଂଖ୍ୟା event, ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg1, ଶୂନ୍ୟ *arg2)
{
  ସଂରଚନା cf_hc_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t i;

  ଯଦି(!cf->connected) {
    ବାଛ(event) {
    ସ୍ଥିତି CF_CTRL_REPORT_STATS:
    ସ୍ଥିତି CF_CTRL_CONN_INFO_UPDATE:
      ପାଇଁ(i = 0; i < ctx->baller_ଗଣନା; i++) {
        /* Make the first baller that connected at network level report */
        ଯଦି(Curl_conn_cf_is_ip_connected(ctx->ballers[i].cf, data)) {
          Curl_conn_cf_cntrl(ctx->ballers[i].cf, data, TRUE,
                             event, arg1, arg2);
          ଭାଙ୍ଗ;
        }
      }
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ପାଇଁ(i = 0; i < ctx->baller_ଗଣନା; i++) {
        result = cf_hc_baller_cntrl(&ctx->ballers[i], data, event, arg1, arg2);
        ଯଦି(result && (result != CURLE_AGAIN))
          ଯାଅ out;
      }
      result = CURLE_OK;
      ଭାଙ୍ଗ;
    }
  }
out:
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_hc_destroy(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_hc_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  cf_hc_ctx_destroy(data, ctx);
}

ସଂରଚନା Curl_cftype Curl_cft_http_connect = {
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

ସ୍ଥିର CURLcode cf_hc_create(ସଂରଚନା Curl_cfilter **pcf,
                             ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା Curl_peer *destination,
                             uint8_t def_transport)
{
  ସଂରଚନା Curl_cfilter *cf = ଖାଲି;
  ସଂରଚନା cf_hc_ctx *ctx;
  CURLcode result = CURLE_OK;

  ctx = curlx_calloc(1, ଆକାର(*ctx));
  ଯଦି(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }
  Curl_peer_link(&ctx->destination, destination);
  ctx->def_transport = def_transport;
  ctx->hard_eyeballs_timeout_ms = data->set.happy_eyeballs_timeout;
  ctx->soft_eyeballs_timeout_ms = data->set.happy_eyeballs_timeout / 2;

  result = Curl_cf_create(&cf, &Curl_cft_http_connect, ctx);
  ଯଦି(result)
    ଯାଅ out;
  ctx = ଖାଲି;

out:
  *pcf = result ? ଖାଲି : cf;
  cf_hc_ctx_destroy(data, ctx);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_hc_add(ସଂରଚନା Curl_easy *data,
                          ସଂରଚନା Curl_peer *destination,
                          ସଂରଚନା connectdata *conn,
                          int8_t sockindex,
                          uint8_t def_transport)
{
  ସଂରଚନା Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_hc_create(&cf, data, destination, def_transport);
  ଯଦି(result)
    ଯାଅ out;
  Curl_conn_cf_add(data, conn, sockindex, cf);

#ifdef USE_HTTPSRR
  result = Curl_conn_dns_add_https_resolve(data, cf->conn, cf->sockindex,
                                           destination);
#endif
out:
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_cf_https_setup(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା Curl_peer *destination,
                             ସଂରଚନା connectdata *conn,
                             int8_t sockindex)
{
  CURLcode result = CURLE_OK;

  DEBUGASSERT(conn->scheme->protocol == CURLPROTO_HTTPS);

  /* This filter is intended for HTTPS using ALPN and does
   * not support HTTPS Eyeballing to a proxy. */
  ଯଦି((conn->scheme->protocol != CURLPROTO_HTTPS) ||
#ifndef CURL_DISABLE_PROXY
     conn->bits.origin_is_proxy ||
#endif
     !conn->bits.tls_enable_alpn)
    ଯାଅ out;

  result = cf_hc_add(data, destination, conn, sockindex,
                     conn->transport_wanted);

out:
  ଫେରନ୍ତୁ result;
}

#endif /* !CURL_DISABLE_HTTP */
