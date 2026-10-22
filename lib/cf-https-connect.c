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
#અન્તર્ભાવ "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "cf-setup.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "cf-https-connect.h"
#અન્તર્ભાવ "http2.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "vdns/cf-dns.h"
#અન્તર્ભાવ "vdns/hostip.h"
#અન્તર્ભાવ "vdns/httpsrr.h"
#અન્તર્ભાવ "vquic/vquic.h"

પ્રકારવ્યાખ્યા ગણના {
  CF_HC_RESOLV,
  CF_HC_INIT,
  CF_HC_CONNECT,
  CF_HC_SUCCESS,
  CF_HC_FAILURE
} cf_hc_state;

સંરચના cf_hc_baller {
  અચલ અક્ષર *name;
  સંરચના Curl_cfilter *cf;
  CURLcode result;
  પૂર્ણાંક reply_ms;
  uint8_t transport;
  ગણના alpnid alpn_id;
  BIT(shutdown);
};

સ્થિર શૂન્ય cf_hc_baller_discard(સંરચના cf_hc_baller *b,
                                 સંરચના Curl_easy *data)
{
  જો(b->cf) {
    Curl_conn_cf_discard_chain(&b->cf, data);
    b->cf = ખાલી;
  }
}

સ્થિર bool cf_hc_baller_is_connecting(સંરચના cf_hc_baller *b)
{
  પાછા b->cf && !b->result;
}

સ્થિર bool cf_hc_baller_has_started(સંરચના cf_hc_baller *b)
{
  પાછા !!b->cf;
}

સ્થિર પૂર્ણાંક cf_hc_baller_reply_ms(સંરચના cf_hc_baller *b,
                                 સંરચના Curl_easy *data)
{
  જો(b->cf && (b->reply_ms < 0))
    b->cf->cft->query(b->cf, data, CF_QUERY_CONNECT_REPLY_MS,
                      &b->reply_ms, ખાલી);
  પાછા b->reply_ms;
}

સ્થિર bool cf_hc_baller_data_pending(સંરચના cf_hc_baller *b,
                                      અચલ સંરચના Curl_easy *data)
{
  પાછા b->cf && !b->result && b->cf->cft->has_data_pending(b->cf, data);
}

સ્થિર bool cf_hc_baller_needs_flush(સંરચના cf_hc_baller *b,
                                     સંરચના Curl_easy *data)
{
  પાછા b->cf && !b->result && Curl_conn_cf_needs_flush(b->cf, data);
}

સ્થિર CURLcode cf_hc_baller_cntrl(સંરચના cf_hc_baller *b,
                                   સંરચના Curl_easy *data,
                                   પૂર્ણાંક event, પૂર્ણાંક arg1, શૂન્ય *arg2)
{
  જો(b->cf && !b->result)
    પાછા Curl_conn_cf_cntrl(b->cf, data, FALSE, event, arg1, arg2);
  પાછા CURLE_OK;
}

સંરચના cf_hc_ctx {
  cf_hc_state state;
  સંરચના Curl_peer *destination; /* who we ultimately want to talk to */
  સંરચના curltime started;  /* when connect started */
  CURLcode result;          /* overall result */
  CURLcode check_h3_result;
  સંરચના cf_hc_baller ballers[2];
  size_t baller_ગણતરી;
  timediff_t soft_eyeballs_timeout_ms;
  timediff_t hard_eyeballs_timeout_ms;
  uint8_t def_transport;
  BIT(httpsrr_resolved);
  BIT(checked_h3);
  BIT(ballers_complete);
};

સ્થિર શૂન્ય cf_hc_ctx_destroy(સંરચના Curl_easy *data,
                              સંરચના cf_hc_ctx *ctx)
{
  જો(ctx) {
    size_t i;
    માટે(i = 0; i < ctx->baller_ગણતરી; ++i)
      cf_hc_baller_discard(&ctx->ballers[i], data);
    Curl_peer_unlink(&ctx->destination);
    curlx_free(ctx);
  }
}

સ્થિર શૂન્ય cf_hc_baller_assign(સંરચના cf_hc_baller *b,
                                ગણના alpnid alpn_id,
                                uint8_t def_transport)
{
  b->alpn_id = alpn_id;
  b->transport = def_transport;
  b->cf = ખાલી;
  b->result = CURLE_OK;
  b->reply_ms = -1;
  b->shutdown = FALSE;
  પસંદગી(b->alpn_id) {
  સ્થિતિ ALPN_h3:
    b->name = "h3";
    b->transport = TRNSPRT_QUIC;
    તોડો;
  સ્થિતિ ALPN_h2:
    b->name = "h2";
    તોડો;
  સ્થિતિ ALPN_h1:
    b->name = "h1";
    તોડો;
  સ્થિતિ ALPN_none:
    b->name = "no-alpn";
    તોડો;
  મૂળભૂત:
    b->result = CURLE_FAILED_INIT;
    તોડો;
  }
}

સ્થિર શૂન્ય cf_hc_baller_init(સંરચના cf_hc_baller *b,
                              સંરચના Curl_cfilter *cf,
                              સંરચના Curl_easy *data)
{
  સંરચના Curl_cfilter *save = cf->next;

  cf->next = ખાલી;
  b->result = Curl_cf_setup_ઉમેરો_after(cf, data, b->transport,
                                         CURL_CF_SSL_ENABLE);
  b->cf = cf->next;
  cf->next = save;
}

સ્થિર CURLcode cf_hc_baller_connect(સંરચના cf_hc_baller *b,
                                     સંરચના Curl_cfilter *cf,
                                     સંરચના Curl_easy *data,
                                     bool *done)
{
  સંરચના Curl_cfilter *save = cf->next;

  cf->next = b->cf;
  b->result = Curl_conn_cf_connect(cf->next, data, done);
  b->cf = cf->next; /* it might mutate */
  cf->next = save;
  પાછા b->result;
}

સ્થિર CURLcode baller_connected(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 સંરચના cf_hc_baller *winner)
{
  સંરચના cf_hc_ctx *ctx = cf->ctx;

  /* Make the winner's connection filter out own sub-filter, check, move,
   * close all remaining. */
  જો(cf->next) {
    DEBUGASSERT(0);
    પાછા CURLE_FAILED_INIT;
  }
  જો(!winner->cf) {
    DEBUGASSERT(0);
    પાછા CURLE_FAILED_INIT;
  }

  cf->next = winner->cf;
  winner->cf = ખાલી;
  ctx->state = CF_HC_SUCCESS;
  cf->connected = TRUE;

  /* ballers may have failf()'d, the winner resets it, so our
   * errorbuf is clean again. */
  Curl_reset_fail(data);

#ifdef USE_NGHTTP2
  {
    /* For a negotiated HTTP/2 connection insert the h2 filter. */
    અચલ અક્ષર *alpn = Curl_conn_cf_get_alpn_negotiated(cf->next, data);
    જો(alpn && !strcmp("h2", alpn)) {
      CURLcode result = Curl_http2_switch_at(cf, data);
      જો(result) {
        ctx->state = CF_HC_FAILURE;
        ctx->result = result;
        પાછા result;
      }
    }
  }
#endif
  પાછા CURLE_OK;
}

સ્થિર bool time_to_start_baller2(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data,
                                  અચલ સંરચના curltime *pnow)
{
  સંરચના cf_hc_ctx *ctx = cf->ctx;
  timediff_t elapsed_ms;

  જો(ctx->baller_ગણતરી < 2)
    પાછા FALSE;
  નહીંતર જો(cf_hc_baller_has_started(&ctx->ballers[1]))
    પાછા FALSE;
  નહીંતર જો(ctx->ballers[0].result) {
    CURL_TRC_CF(data, cf, "%s baller failed, starting %s",
                ctx->ballers[0].name, ctx->ballers[1].name);
    પાછા TRUE;
  }

  elapsed_ms = curlx_ptimediff_ms(pnow, &ctx->started);
  જો(elapsed_ms >= ctx->hard_eyeballs_timeout_ms) {
    CURL_TRC_CF(data, cf, "%s inconclusive after %" FMT_TIMEDIFF_T ", "
                "starting %s", ctx->ballers[0].name,
                ctx->hard_eyeballs_timeout_ms, ctx->ballers[1].name);
    પાછા TRUE;
  }
  નહીંતર જો(elapsed_ms >= ctx->soft_eyeballs_timeout_ms) {
    જો(cf_hc_baller_reply_ms(&ctx->ballers[0], data) < 0) {
      CURL_TRC_CF(data, cf, "%s has not seen any data after %"
                  FMT_TIMEDIFF_T "ms, starting %s",
                  ctx->ballers[0].name, ctx->soft_eyeballs_timeout_ms,
                  ctx->ballers[1].name);
      પાછા TRUE;
    }
    /* set the effective hard timeout again */
    Curl_expire_set(data, EXPIRE_ALPN_EYEBALLS,
                    ctx->hard_eyeballs_timeout_ms - elapsed_ms, pnow);
  }
  પાછા FALSE;
}

સ્થિર bool cf_hc_may_h3(સંરચના Curl_cfilter *cf,
                         સંરચના Curl_easy *data)
{
  સંરચના cf_hc_ctx *ctx = cf->ctx;
  જો(!ctx->checked_h3) {
    ctx->check_h3_result =
      Curl_conn_may_http3(data, cf->conn);
    ctx->checked_h3 = TRUE;
  }
  પાછા !ctx->check_h3_result;
}

સ્થિર ગણના alpnid cf_hc_get_httpsrr_alpn(સંરચના Curl_cfilter *cf,
                                          સંરચના Curl_easy *data,
                                          ગણના alpnid not_this_one)
{
#ifdef USE_HTTPSRR
  સંરચના cf_hc_ctx *ctx = cf->ctx;
  /* Is there an HTTPSRR use its ALPNs here.
   * We are here after having selected a connection to a host+port and
   * can no longer change that. Any HTTPSRR advice for other hosts and ports
   * we need to ignore. */
  અચલ સંરચના Curl_https_rrinfo *rr;
  size_t i;

  /* Do we have HTTPS-RR information? */
  rr = Curl_conn_dns_get_https(data, cf->sockindex, ctx->destination);

  CURL_TRC_CF(data, cf, "HTTPS-RR %savailable", rr ? "" : "not ");
  /* We do not support `rr->no_def_alpn`. */
  જો(Curl_httpsrr_applicable(data, rr) && !rr->no_def_alpn) {
    માટે(i = 0; i < CURL_ARRAYSIZE(rr->alpns); ++i) {
      ગણના alpnid alpn_rr = (ગણના alpnid)rr->alpns[i];
      જો(alpn_rr == not_this_one) /* do not want this one */
        ચાલુ;
      પસંદગી(alpn_rr) {
      સ્થિતિ ALPN_h3:
        જો((data->state.http_neg.allowed & CURL_HTTP_V3x) &&
           cf_hc_may_h3(cf, data)) {
          પાછા alpn_rr;
        }
        તોડો;
      સ્થિતિ ALPN_h2:
        જો(data->state.http_neg.allowed & CURL_HTTP_V2x) {
          પાછા alpn_rr;
        }
        તોડો;
      સ્થિતિ ALPN_h1:
        જો(data->state.http_neg.allowed & CURL_HTTP_V1x) {
          પાછા alpn_rr;
        }
        તોડો;
      મૂળભૂત: /* ignore */
        તોડો;
      }
    }
  }
#નહીંતર
  (શૂન્ય)cf;
  (શૂન્ય)data;
  (શૂન્ય)not_this_one;
#endif
  પાછા ALPN_none;
}

સ્થિર ગણના alpnid cf_hc_get_pref_alpn(સંરચના Curl_cfilter *cf,
                                       સંરચના Curl_easy *data,
                                       ગણના alpnid not_this_one)
{
  જો((data->state.http_neg.preferred & data->state.http_neg.allowed)) {
    પસંદગી(data->state.http_neg.preferred) {
    સ્થિતિ CURL_HTTP_V3x:
      જો(cf_hc_may_h3(cf, data) && (ALPN_h3 != not_this_one))
        પાછા ALPN_h3;
      તોડો;
    સ્થિતિ CURL_HTTP_V2x:
      જો(ALPN_h2 != not_this_one)
        પાછા ALPN_h2;
      તોડો;
    સ્થિતિ CURL_HTTP_V1x:
      /* If we are trying h2 already, h1 is already used as fallback */
      જો((ALPN_h1 != not_this_one) && (ALPN_h2 != not_this_one))
        પાછા ALPN_h1;
      તોડો;
    મૂળભૂત:
      તોડો;
    }
  }
  પાછા ALPN_none;
}

સ્થિર ગણના alpnid cf_hc_get_first_alpn(સંરચના Curl_cfilter *cf,
                                        સંરચના Curl_easy *data,
                                        http_majors choices,
                                        ગણના alpnid not_this_one)
{
  /* When told to not try h2, we also do not try h1 and vice versa */
  bool allow_h1_or_h2 = (not_this_one != ALPN_h1) &&
                        (not_this_one != ALPN_h2);
  જો((ALPN_h3 != not_this_one) && (choices & CURL_HTTP_V3x) &&
     cf_hc_may_h3(cf, data)) {
    પાછા ALPN_h3;
  }
  જો(allow_h1_or_h2 && (choices & CURL_HTTP_V2x)) {
    પાછા ALPN_h2;
  }
  જો(allow_h1_or_h2 && (choices & CURL_HTTP_V1x)) {
    પાછા ALPN_h1;
  }
  પાછા ALPN_none;
}

સ્થિર CURLcode cf_hc_set_baller1(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data)
{
  સંરચના cf_hc_ctx *ctx = cf->ctx;
  ગણના alpnid alpn1 = ALPN_none;
  VERBOSE(અચલ અક્ષર *source = "HTTPS-RR");

  DEBUGASSERT(cf->conn->bits.tls_enable_alpn);

  alpn1 = cf_hc_get_httpsrr_alpn(cf, data, ALPN_none);
  જો(alpn1 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(source = "preferred version");
    alpn1 = cf_hc_get_pref_alpn(cf, data, ALPN_none);
  }
  જો(alpn1 == ALPN_none) {
    VERBOSE(source = "wanted versions");
    alpn1 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.wanted,
                                 ALPN_none);
  }
  જો(alpn1 == ALPN_none) {
    VERBOSE(source = "allowed versions");
    alpn1 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.allowed,
                                 ALPN_none);
  }

  જો(alpn1 == ALPN_none) {
    /* None of the wanted/allowed HTTP versions could be chosen */
    જો(ctx->check_h3_result) {
      CURL_TRC_CF(data, cf, "unable to use HTTP/3");
      પાછા ctx->check_h3_result;
    }
    CURL_TRC_CF(data, cf, "unable to select HTTP version");
    પાછા CURLE_FAILED_INIT;
  }

  cf_hc_baller_assign(&ctx->ballers[0], alpn1, ctx->def_transport);
  ctx->baller_ગણતરી = 1;
  CURL_TRC_CF(data, cf, "1st attempt uses %s from %s",
              ctx->ballers[0].name, source);

  પસંદગી(alpn1) {
  સ્થિતિ ALPN_h1:
    /* We really want h1, switch off h2 to make it disappear in ALPN */
    data->state.http_neg.wanted &= (uint8_t)~CURL_HTTP_V2x;
    તોડો;
  મૂળભૂત:
    તોડો;
  }

  પાછા CURLE_OK;
}

સ્થિર શૂન્ય cf_hc_set_baller2(સંરચના Curl_cfilter *cf,
                              સંરચના Curl_easy *data)
{
  સંરચના cf_hc_ctx *ctx = cf->ctx;
  ગણના alpnid alpn2 = ALPN_none, alpn1 = ctx->ballers[0].alpn_id;
  VERBOSE(અચલ અક્ષર *source = "HTTPS-RR");

  જો(ctx->ballers_complete)
    પાછા; /* already done */
  જો(!ctx->httpsrr_resolved)
    પાછા; /* HTTPS-RR pending */

  alpn2 = cf_hc_get_httpsrr_alpn(cf, data, alpn1);
  જો(alpn2 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(source = "preferred version");
    alpn2 = cf_hc_get_pref_alpn(cf, data, alpn1);
  }
  જો(alpn2 == ALPN_none) {
    VERBOSE(source = "wanted versions");
    alpn2 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.wanted,
                                 alpn1);
  }

  જો(alpn2 != ALPN_none) {
    cf_hc_baller_assign(&ctx->ballers[1], alpn2, ctx->def_transport);
    ctx->baller_ગણતરી = 2;
    CURL_TRC_CF(data, cf, "2nd attempt uses %s from %s",
                ctx->ballers[1].name, source);
  }
  ctx->ballers_complete = TRUE;
}

સ્થિર CURLcode cf_hc_connect(સંરચના Curl_cfilter *cf,
                              સંરચના Curl_easy *data,
                              bool *done)
{
  સંરચના cf_hc_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  અચલ સંરચના curltime *pnow = ખાલી;

  જો(cf->connected) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  *done = FALSE;

  જો(!ctx->httpsrr_resolved) {
    ctx->httpsrr_resolved = Curl_conn_dns_resolved_https(
      data, cf->sockindex, ctx->destination);
#ifdef DEBUGBUILD
    જો(!ctx->httpsrr_resolved && getenv("CURL_DBG_AWAIT_HTTPSRR")) {
      CURL_TRC_CF(data, cf, "awaiting HTTPS-RR");
      પાછા CURLE_OK;
    }
#endif
  }

  પસંદગી(ctx->state) {
  સ્થિતિ CF_HC_RESOLV:
    ctx->state = CF_HC_INIT;
    FALLTHROUGH();

  સ્થિતિ CF_HC_INIT:
    DEBUGASSERT(!cf->next);
    CURL_TRC_CF(data, cf, "connect, init");
    result = cf_hc_set_baller1(cf, data);
    જો(result) {
      ctx->result = result;
      ctx->state = CF_HC_FAILURE;
      જાઓ out;
    }
    cf_hc_set_baller2(cf, data);
    pnow = Curl_pgrs_now(data);
    ctx->started = *pnow;
    cf_hc_baller_init(&ctx->ballers[0], cf, data);
    જો((ctx->baller_ગણતરી > 1) || !ctx->ballers_complete) {
      Curl_expire_set(data, EXPIRE_ALPN_EYEBALLS,
                      ctx->soft_eyeballs_timeout_ms, pnow);
    }
    ctx->state = CF_HC_CONNECT;
    FALLTHROUGH();

  સ્થિતિ CF_HC_CONNECT:
    જો(!ctx->ballers_complete)
      cf_hc_set_baller2(cf, data);

    જો(cf_hc_baller_is_connecting(&ctx->ballers[0])) {
      result = cf_hc_baller_connect(&ctx->ballers[0], cf, data, done);
      જો(!result && *done) {
        result = baller_connected(cf, data, &ctx->ballers[0]);
        જાઓ out;
      }
    }

    pnow = Curl_pgrs_now(data);
    જો(time_to_start_baller2(cf, data, pnow)) {
      cf_hc_baller_init(&ctx->ballers[1], cf, data);
    }

    જો(cf_hc_baller_is_connecting(&ctx->ballers[1])) {
      result = cf_hc_baller_connect(&ctx->ballers[1], cf, data, done);
      જો(!result && *done) {
        result = baller_connected(cf, data, &ctx->ballers[1]);
        જાઓ out;
      }
    }

    જો(ctx->ballers[0].result &&
       (ctx->ballers[1].result ||
        (ctx->ballers_complete && (ctx->baller_ગણતરી < 2)))) {
      /* all have failed. we give up */
      CURL_TRC_CF(data, cf, "connect, all attempts failed");
      ctx->result = result = ctx->ballers[0].result;
      ctx->state = CF_HC_FAILURE;
      જાઓ out;
    }
    result = CURLE_OK;
    *done = FALSE;
    તોડો;

  સ્થિતિ CF_HC_FAILURE:
    result = ctx->result;
    cf->connected = FALSE;
    *done = FALSE;
    તોડો;

  સ્થિતિ CF_HC_SUCCESS:
    result = CURLE_OK;
    cf->connected = TRUE;
    *done = TRUE;
    તોડો;
  }

out:
  CURL_TRC_CF(data, cf, "connect -> %d, done=%d", (પૂર્ણાંક)result, *done);
  પાછા result;
}

સ્થિર CURLcode cf_hc_shutdown(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data, bool *done)
{
  સંરચના cf_hc_ctx *ctx = cf->ctx;
  size_t i;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  જો(cf->connected) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  માટે(i = 0; i < ctx->baller_ગણતરી; i++) {
    સંરચના cf_hc_baller *b = &ctx->ballers[i];
    bool bdone = FALSE;
    જો(!cf_hc_baller_is_connecting(b) || b->shutdown)
      ચાલુ;
    b->result = b->cf->cft->do_shutdown(b->cf, data, &bdone);
    જો(b->result || bdone)
      b->shutdown = TRUE; /* treat a failed shutdown as done */
  }

  *done = TRUE;
  માટે(i = 0; i < ctx->baller_ગણતરી; i++) {
    જો(!ctx->ballers[i].shutdown)
      *done = FALSE;
  }
  જો(*done) {
    માટે(i = 0; i < ctx->baller_ગણતરી; i++) {
      જો(ctx->ballers[i].result)
        result = ctx->ballers[i].result;
    }
  }
  CURL_TRC_CF(data, cf, "shutdown -> %d, done=%d", (પૂર્ણાંક)result, *done);
  પાછા result;
}

સ્થિર CURLcode cf_hc_adjust_pollset(સંરચના Curl_cfilter *cf,
                                     સંરચના Curl_easy *data,
                                     સંરચના easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  જો(!cf->connected) {
    સંરચના cf_hc_ctx *ctx = cf->ctx;
    size_t i;

    માટે(i = 0; (i < ctx->baller_ગણતરી) && !result; i++) {
      સંરચના cf_hc_baller *b = &ctx->ballers[i];
      જો(!cf_hc_baller_is_connecting(b))
        ચાલુ;
      result = Curl_conn_cf_adjust_pollset(b->cf, data, ps);
    }
    CURL_TRC_CF(data, cf, "adjust_pollset -> %d, %u socks", (પૂર્ણાંક)result,
                ps->n);
  }
  પાછા result;
}

સ્થિર bool cf_hc_data_pending(સંરચના Curl_cfilter *cf,
                               અચલ સંરચના Curl_easy *data)
{
  સંરચના cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  જો(cf->connected)
    પાછા cf->next->cft->has_data_pending(cf->next, data);

  માટે(i = 0; i < ctx->baller_ગણતરી; i++)
    જો(cf_hc_baller_data_pending(&ctx->ballers[i], data))
      પાછા TRUE;
  પાછા FALSE;
}

સ્થિર CURLcode cf_hc_query(સંરચના Curl_cfilter *cf,
                            સંરચના Curl_easy *data,
                            પૂર્ણાંક query, પૂર્ણાંક *pres1, શૂન્ય *pres2)
{
  સંરચના cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  જો(!cf->connected) {
    પસંદગી(query) {
    સ્થિતિ CF_QUERY_NEED_FLUSH: {
      માટે(i = 0; i < ctx->baller_ગણતરી; i++)
        જો(cf_hc_baller_needs_flush(&ctx->ballers[i], data)) {
          *pres1 = TRUE;
          પાછા CURLE_OK;
        }
      તોડો;
    }
    મૂળભૂત:
      તોડો;
    }
  }
  પાછા cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

સ્થિર CURLcode cf_hc_cntrl(સંરચના Curl_cfilter *cf,
                            સંરચના Curl_easy *data,
                            પૂર્ણાંક event, પૂર્ણાંક arg1, શૂન્ય *arg2)
{
  સંરચના cf_hc_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t i;

  જો(!cf->connected) {
    પસંદગી(event) {
    સ્થિતિ CF_CTRL_REPORT_STATS:
    સ્થિતિ CF_CTRL_CONN_INFO_UPDATE:
      માટે(i = 0; i < ctx->baller_ગણતરી; i++) {
        /* Make the first baller that connected at network level report */
        જો(Curl_conn_cf_is_ip_connected(ctx->ballers[i].cf, data)) {
          Curl_conn_cf_cntrl(ctx->ballers[i].cf, data, TRUE,
                             event, arg1, arg2);
          તોડો;
        }
      }
      તોડો;
    મૂળભૂત:
      માટે(i = 0; i < ctx->baller_ગણતરી; i++) {
        result = cf_hc_baller_cntrl(&ctx->ballers[i], data, event, arg1, arg2);
        જો(result && (result != CURLE_AGAIN))
          જાઓ out;
      }
      result = CURLE_OK;
      તોડો;
    }
  }
out:
  પાછા result;
}

સ્થિર શૂન્ય cf_hc_destroy(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data)
{
  સંરચના cf_hc_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  cf_hc_ctx_destroy(data, ctx);
}

સંરચના Curl_cftype Curl_cft_http_connect = {
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

સ્થિર CURLcode cf_hc_create(સંરચના Curl_cfilter **pcf,
                             સંરચના Curl_easy *data,
                             સંરચના Curl_peer *destination,
                             uint8_t def_transport)
{
  સંરચના Curl_cfilter *cf = ખાલી;
  સંરચના cf_hc_ctx *ctx;
  CURLcode result = CURLE_OK;

  ctx = curlx_calloc(1, કદ(*ctx));
  જો(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }
  Curl_peer_link(&ctx->destination, destination);
  ctx->def_transport = def_transport;
  ctx->hard_eyeballs_timeout_ms = data->set.happy_eyeballs_timeout;
  ctx->soft_eyeballs_timeout_ms = data->set.happy_eyeballs_timeout / 2;

  result = Curl_cf_create(&cf, &Curl_cft_http_connect, ctx);
  જો(result)
    જાઓ out;
  ctx = ખાલી;

out:
  *pcf = result ? ખાલી : cf;
  cf_hc_ctx_destroy(data, ctx);
  પાછા result;
}

સ્થિર CURLcode cf_hc_add(સંરચના Curl_easy *data,
                          સંરચના Curl_peer *destination,
                          સંરચના connectdata *conn,
                          int8_t sockindex,
                          uint8_t def_transport)
{
  સંરચના Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_hc_create(&cf, data, destination, def_transport);
  જો(result)
    જાઓ out;
  Curl_conn_cf_add(data, conn, sockindex, cf);

#ifdef USE_HTTPSRR
  result = Curl_conn_dns_add_https_resolve(data, cf->conn, cf->sockindex,
                                           destination);
#endif
out:
  પાછા result;
}

CURLcode Curl_cf_https_setup(સંરચના Curl_easy *data,
                             સંરચના Curl_peer *destination,
                             સંરચના connectdata *conn,
                             int8_t sockindex)
{
  CURLcode result = CURLE_OK;

  DEBUGASSERT(conn->scheme->protocol == CURLPROTO_HTTPS);

  /* This filter is intended for HTTPS using ALPN and does
   * not support HTTPS Eyeballing to a proxy. */
  જો((conn->scheme->protocol != CURLPROTO_HTTPS) ||
#ifndef CURL_DISABLE_PROXY
     conn->bits.origin_is_proxy ||
#endif
     !conn->bits.tls_enable_alpn)
    જાઓ out;

  result = cf_hc_add(data, destination, conn, sockindex,
                     conn->transport_wanted);

out:
  પાછા result;
}

#endif /* !CURL_DISABLE_HTTP */
