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
#അന്തര്ഭാവ "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "cf-setup.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "cf-https-connect.h"
#അന്തര്ഭാവ "http2.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "vdns/cf-dns.h"
#അന്തര്ഭാവ "vdns/hostip.h"
#അന്തര്ഭാവ "vdns/httpsrr.h"
#അന്തര്ഭാവ "vquic/vquic.h"

തരനിർവചനം എണ്ണൽ {
  CF_HC_RESOLV,
  CF_HC_INIT,
  CF_HC_CONNECT,
  CF_HC_SUCCESS,
  CF_HC_FAILURE
} cf_hc_state;

ഘടന cf_hc_baller {
  സ്ഥിരം അക്ഷരം *name;
  ഘടന Curl_cfilter *cf;
  CURLcode result;
  പൂർണ്ണസംഖ്യ reply_ms;
  uint8_t transport;
  എണ്ണൽ alpnid alpn_id;
  BIT(shutdown);
};

സ്ഥിതം ശൂന്യം cf_hc_baller_discard(ഘടന cf_hc_baller *b,
                                 ഘടന Curl_easy *data)
{
  എങ്കിൽ(b->cf) {
    Curl_conn_cf_discard_chain(&b->cf, data);
    b->cf = ശൂന്യ;
  }
}

സ്ഥിതം bool cf_hc_baller_is_connecting(ഘടന cf_hc_baller *b)
{
  തിരികെ b->cf && !b->result;
}

സ്ഥിതം bool cf_hc_baller_has_started(ഘടന cf_hc_baller *b)
{
  തിരികെ !!b->cf;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cf_hc_baller_reply_ms(ഘടന cf_hc_baller *b,
                                 ഘടന Curl_easy *data)
{
  എങ്കിൽ(b->cf && (b->reply_ms < 0))
    b->cf->cft->query(b->cf, data, CF_QUERY_CONNECT_REPLY_MS,
                      &b->reply_ms, ശൂന്യ);
  തിരികെ b->reply_ms;
}

സ്ഥിതം bool cf_hc_baller_data_pending(ഘടന cf_hc_baller *b,
                                      സ്ഥിരം ഘടന Curl_easy *data)
{
  തിരികെ b->cf && !b->result && b->cf->cft->has_data_pending(b->cf, data);
}

സ്ഥിതം bool cf_hc_baller_needs_flush(ഘടന cf_hc_baller *b,
                                     ഘടന Curl_easy *data)
{
  തിരികെ b->cf && !b->result && Curl_conn_cf_needs_flush(b->cf, data);
}

സ്ഥിതം CURLcode cf_hc_baller_cntrl(ഘടന cf_hc_baller *b,
                                   ഘടന Curl_easy *data,
                                   പൂർണ്ണസംഖ്യ event, പൂർണ്ണസംഖ്യ arg1, ശൂന്യം *arg2)
{
  എങ്കിൽ(b->cf && !b->result)
    തിരികെ Curl_conn_cf_cntrl(b->cf, data, FALSE, event, arg1, arg2);
  തിരികെ CURLE_OK;
}

ഘടന cf_hc_ctx {
  cf_hc_state state;
  ഘടന Curl_peer *destination; /* who we ultimately want to talk to */
  ഘടന curltime started;  /* when connect started */
  CURLcode result;          /* overall result */
  CURLcode check_h3_result;
  ഘടന cf_hc_baller ballers[2];
  size_t baller_എണ്ണം;
  timediff_t soft_eyeballs_timeout_ms;
  timediff_t hard_eyeballs_timeout_ms;
  uint8_t def_transport;
  BIT(httpsrr_resolved);
  BIT(checked_h3);
  BIT(ballers_complete);
};

സ്ഥിതം ശൂന്യം cf_hc_ctx_destroy(ഘടന Curl_easy *data,
                              ഘടന cf_hc_ctx *ctx)
{
  എങ്കിൽ(ctx) {
    size_t i;
    വേണ്ടി(i = 0; i < ctx->baller_എണ്ണം; ++i)
      cf_hc_baller_discard(&ctx->ballers[i], data);
    Curl_peer_unlink(&ctx->destination);
    curlx_free(ctx);
  }
}

സ്ഥിതം ശൂന്യം cf_hc_baller_assign(ഘടന cf_hc_baller *b,
                                എണ്ണൽ alpnid alpn_id,
                                uint8_t def_transport)
{
  b->alpn_id = alpn_id;
  b->transport = def_transport;
  b->cf = ശൂന്യ;
  b->result = CURLE_OK;
  b->reply_ms = -1;
  b->shutdown = FALSE;
  തിരഞ്ഞെടുപ്പ്(b->alpn_id) {
  അവസ്ഥ ALPN_h3:
    b->name = "h3";
    b->transport = TRNSPRT_QUIC;
    നിർത്തുക;
  അവസ്ഥ ALPN_h2:
    b->name = "h2";
    നിർത്തുക;
  അവസ്ഥ ALPN_h1:
    b->name = "h1";
    നിർത്തുക;
  അവസ്ഥ ALPN_none:
    b->name = "no-alpn";
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    b->result = CURLE_FAILED_INIT;
    നിർത്തുക;
  }
}

സ്ഥിതം ശൂന്യം cf_hc_baller_init(ഘടന cf_hc_baller *b,
                              ഘടന Curl_cfilter *cf,
                              ഘടന Curl_easy *data)
{
  ഘടന Curl_cfilter *save = cf->next;

  cf->next = ശൂന്യ;
  b->result = Curl_cf_setup_തിരുകുക_after(cf, data, b->transport,
                                         CURL_CF_SSL_ENABLE);
  b->cf = cf->next;
  cf->next = save;
}

സ്ഥിതം CURLcode cf_hc_baller_connect(ഘടന cf_hc_baller *b,
                                     ഘടന Curl_cfilter *cf,
                                     ഘടന Curl_easy *data,
                                     bool *done)
{
  ഘടന Curl_cfilter *save = cf->next;

  cf->next = b->cf;
  b->result = Curl_conn_cf_connect(cf->next, data, done);
  b->cf = cf->next; /* it might mutate */
  cf->next = save;
  തിരികെ b->result;
}

സ്ഥിതം CURLcode baller_connected(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 ഘടന cf_hc_baller *winner)
{
  ഘടന cf_hc_ctx *ctx = cf->ctx;

  /* Make the winner's connection filter out own sub-filter, check, move,
   * close all remaining. */
  എങ്കിൽ(cf->next) {
    DEBUGASSERT(0);
    തിരികെ CURLE_FAILED_INIT;
  }
  എങ്കിൽ(!winner->cf) {
    DEBUGASSERT(0);
    തിരികെ CURLE_FAILED_INIT;
  }

  cf->next = winner->cf;
  winner->cf = ശൂന്യ;
  ctx->state = CF_HC_SUCCESS;
  cf->connected = TRUE;

  /* ballers may have failf()'d, the winner resets it, so our
   * errorbuf is clean again. */
  Curl_reset_fail(data);

#ifdef USE_NGHTTP2
  {
    /* For a negotiated HTTP/2 connection insert the h2 filter. */
    സ്ഥിരം അക്ഷരം *alpn = Curl_conn_cf_get_alpn_negotiated(cf->next, data);
    എങ്കിൽ(alpn && !strcmp("h2", alpn)) {
      CURLcode result = Curl_http2_switch_at(cf, data);
      എങ്കിൽ(result) {
        ctx->state = CF_HC_FAILURE;
        ctx->result = result;
        തിരികെ result;
      }
    }
  }
#endif
  തിരികെ CURLE_OK;
}

സ്ഥിതം bool time_to_start_baller2(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data,
                                  സ്ഥിരം ഘടന curltime *pnow)
{
  ഘടന cf_hc_ctx *ctx = cf->ctx;
  timediff_t elapsed_ms;

  എങ്കിൽ(ctx->baller_എണ്ണം < 2)
    തിരികെ FALSE;
  അല്ലെങ്കിൽ എങ്കിൽ(cf_hc_baller_has_started(&ctx->ballers[1]))
    തിരികെ FALSE;
  അല്ലെങ്കിൽ എങ്കിൽ(ctx->ballers[0].result) {
    CURL_TRC_CF(data, cf, "%s baller failed, starting %s",
                ctx->ballers[0].name, ctx->ballers[1].name);
    തിരികെ TRUE;
  }

  elapsed_ms = curlx_ptimediff_ms(pnow, &ctx->started);
  എങ്കിൽ(elapsed_ms >= ctx->hard_eyeballs_timeout_ms) {
    CURL_TRC_CF(data, cf, "%s inconclusive after %" FMT_TIMEDIFF_T ", "
                "starting %s", ctx->ballers[0].name,
                ctx->hard_eyeballs_timeout_ms, ctx->ballers[1].name);
    തിരികെ TRUE;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(elapsed_ms >= ctx->soft_eyeballs_timeout_ms) {
    എങ്കിൽ(cf_hc_baller_reply_ms(&ctx->ballers[0], data) < 0) {
      CURL_TRC_CF(data, cf, "%s has not seen any data after %"
                  FMT_TIMEDIFF_T "ms, starting %s",
                  ctx->ballers[0].name, ctx->soft_eyeballs_timeout_ms,
                  ctx->ballers[1].name);
      തിരികെ TRUE;
    }
    /* set the effective hard timeout again */
    Curl_expire_set(data, EXPIRE_ALPN_EYEBALLS,
                    ctx->hard_eyeballs_timeout_ms - elapsed_ms, pnow);
  }
  തിരികെ FALSE;
}

സ്ഥിതം bool cf_hc_may_h3(ഘടന Curl_cfilter *cf,
                         ഘടന Curl_easy *data)
{
  ഘടന cf_hc_ctx *ctx = cf->ctx;
  എങ്കിൽ(!ctx->checked_h3) {
    ctx->check_h3_result =
      Curl_conn_may_http3(data, cf->conn);
    ctx->checked_h3 = TRUE;
  }
  തിരികെ !ctx->check_h3_result;
}

സ്ഥിതം എണ്ണൽ alpnid cf_hc_get_httpsrr_alpn(ഘടന Curl_cfilter *cf,
                                          ഘടന Curl_easy *data,
                                          എണ്ണൽ alpnid not_this_one)
{
#ifdef USE_HTTPSRR
  ഘടന cf_hc_ctx *ctx = cf->ctx;
  /* Is there an HTTPSRR use its ALPNs here.
   * We are here after having selected a connection to a host+port and
   * can no longer change that. Any HTTPSRR advice for other hosts and ports
   * we need to ignore. */
  സ്ഥിരം ഘടന Curl_https_rrinfo *rr;
  size_t i;

  /* Do we have HTTPS-RR information? */
  rr = Curl_conn_dns_get_https(data, cf->sockindex, ctx->destination);

  CURL_TRC_CF(data, cf, "HTTPS-RR %savailable", rr ? "" : "not ");
  /* We do not support `rr->no_def_alpn`. */
  എങ്കിൽ(Curl_httpsrr_applicable(data, rr) && !rr->no_def_alpn) {
    വേണ്ടി(i = 0; i < CURL_ARRAYSIZE(rr->alpns); ++i) {
      എണ്ണൽ alpnid alpn_rr = (എണ്ണൽ alpnid)rr->alpns[i];
      എങ്കിൽ(alpn_rr == not_this_one) /* do not want this one */
        തുടരുക;
      തിരഞ്ഞെടുപ്പ്(alpn_rr) {
      അവസ്ഥ ALPN_h3:
        എങ്കിൽ((data->state.http_neg.allowed & CURL_HTTP_V3x) &&
           cf_hc_may_h3(cf, data)) {
          തിരികെ alpn_rr;
        }
        നിർത്തുക;
      അവസ്ഥ ALPN_h2:
        എങ്കിൽ(data->state.http_neg.allowed & CURL_HTTP_V2x) {
          തിരികെ alpn_rr;
        }
        നിർത്തുക;
      അവസ്ഥ ALPN_h1:
        എങ്കിൽ(data->state.http_neg.allowed & CURL_HTTP_V1x) {
          തിരികെ alpn_rr;
        }
        നിർത്തുക;
      സ്ഥിരസ്ഥിതി: /* ignore */
        നിർത്തുക;
      }
    }
  }
#അല്ലെങ്കിൽ
  (ശൂന്യം)cf;
  (ശൂന്യം)data;
  (ശൂന്യം)not_this_one;
#endif
  തിരികെ ALPN_none;
}

സ്ഥിതം എണ്ണൽ alpnid cf_hc_get_pref_alpn(ഘടന Curl_cfilter *cf,
                                       ഘടന Curl_easy *data,
                                       എണ്ണൽ alpnid not_this_one)
{
  എങ്കിൽ((data->state.http_neg.preferred & data->state.http_neg.allowed)) {
    തിരഞ്ഞെടുപ്പ്(data->state.http_neg.preferred) {
    അവസ്ഥ CURL_HTTP_V3x:
      എങ്കിൽ(cf_hc_may_h3(cf, data) && (ALPN_h3 != not_this_one))
        തിരികെ ALPN_h3;
      നിർത്തുക;
    അവസ്ഥ CURL_HTTP_V2x:
      എങ്കിൽ(ALPN_h2 != not_this_one)
        തിരികെ ALPN_h2;
      നിർത്തുക;
    അവസ്ഥ CURL_HTTP_V1x:
      /* If we are trying h2 already, h1 is already used as fallback */
      എങ്കിൽ((ALPN_h1 != not_this_one) && (ALPN_h2 != not_this_one))
        തിരികെ ALPN_h1;
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      നിർത്തുക;
    }
  }
  തിരികെ ALPN_none;
}

സ്ഥിതം എണ്ണൽ alpnid cf_hc_get_first_alpn(ഘടന Curl_cfilter *cf,
                                        ഘടന Curl_easy *data,
                                        http_majors choices,
                                        എണ്ണൽ alpnid not_this_one)
{
  /* When told to not try h2, we also do not try h1 and vice versa */
  bool allow_h1_or_h2 = (not_this_one != ALPN_h1) &&
                        (not_this_one != ALPN_h2);
  എങ്കിൽ((ALPN_h3 != not_this_one) && (choices & CURL_HTTP_V3x) &&
     cf_hc_may_h3(cf, data)) {
    തിരികെ ALPN_h3;
  }
  എങ്കിൽ(allow_h1_or_h2 && (choices & CURL_HTTP_V2x)) {
    തിരികെ ALPN_h2;
  }
  എങ്കിൽ(allow_h1_or_h2 && (choices & CURL_HTTP_V1x)) {
    തിരികെ ALPN_h1;
  }
  തിരികെ ALPN_none;
}

സ്ഥിതം CURLcode cf_hc_set_baller1(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data)
{
  ഘടന cf_hc_ctx *ctx = cf->ctx;
  എണ്ണൽ alpnid alpn1 = ALPN_none;
  VERBOSE(സ്ഥിരം അക്ഷരം *source = "HTTPS-RR");

  DEBUGASSERT(cf->conn->bits.tls_enable_alpn);

  alpn1 = cf_hc_get_httpsrr_alpn(cf, data, ALPN_none);
  എങ്കിൽ(alpn1 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(source = "preferred version");
    alpn1 = cf_hc_get_pref_alpn(cf, data, ALPN_none);
  }
  എങ്കിൽ(alpn1 == ALPN_none) {
    VERBOSE(source = "wanted versions");
    alpn1 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.wanted,
                                 ALPN_none);
  }
  എങ്കിൽ(alpn1 == ALPN_none) {
    VERBOSE(source = "allowed versions");
    alpn1 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.allowed,
                                 ALPN_none);
  }

  എങ്കിൽ(alpn1 == ALPN_none) {
    /* None of the wanted/allowed HTTP versions could be chosen */
    എങ്കിൽ(ctx->check_h3_result) {
      CURL_TRC_CF(data, cf, "unable to use HTTP/3");
      തിരികെ ctx->check_h3_result;
    }
    CURL_TRC_CF(data, cf, "unable to select HTTP version");
    തിരികെ CURLE_FAILED_INIT;
  }

  cf_hc_baller_assign(&ctx->ballers[0], alpn1, ctx->def_transport);
  ctx->baller_എണ്ണം = 1;
  CURL_TRC_CF(data, cf, "1st attempt uses %s from %s",
              ctx->ballers[0].name, source);

  തിരഞ്ഞെടുപ്പ്(alpn1) {
  അവസ്ഥ ALPN_h1:
    /* We really want h1, switch off h2 to make it disappear in ALPN */
    data->state.http_neg.wanted &= (uint8_t)~CURL_HTTP_V2x;
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }

  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം cf_hc_set_baller2(ഘടന Curl_cfilter *cf,
                              ഘടന Curl_easy *data)
{
  ഘടന cf_hc_ctx *ctx = cf->ctx;
  എണ്ണൽ alpnid alpn2 = ALPN_none, alpn1 = ctx->ballers[0].alpn_id;
  VERBOSE(സ്ഥിരം അക്ഷരം *source = "HTTPS-RR");

  എങ്കിൽ(ctx->ballers_complete)
    തിരികെ; /* already done */
  എങ്കിൽ(!ctx->httpsrr_resolved)
    തിരികെ; /* HTTPS-RR pending */

  alpn2 = cf_hc_get_httpsrr_alpn(cf, data, alpn1);
  എങ്കിൽ(alpn2 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(source = "preferred version");
    alpn2 = cf_hc_get_pref_alpn(cf, data, alpn1);
  }
  എങ്കിൽ(alpn2 == ALPN_none) {
    VERBOSE(source = "wanted versions");
    alpn2 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.wanted,
                                 alpn1);
  }

  എങ്കിൽ(alpn2 != ALPN_none) {
    cf_hc_baller_assign(&ctx->ballers[1], alpn2, ctx->def_transport);
    ctx->baller_എണ്ണം = 2;
    CURL_TRC_CF(data, cf, "2nd attempt uses %s from %s",
                ctx->ballers[1].name, source);
  }
  ctx->ballers_complete = TRUE;
}

സ്ഥിതം CURLcode cf_hc_connect(ഘടന Curl_cfilter *cf,
                              ഘടന Curl_easy *data,
                              bool *done)
{
  ഘടന cf_hc_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  സ്ഥിരം ഘടന curltime *pnow = ശൂന്യ;

  എങ്കിൽ(cf->connected) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  *done = FALSE;

  എങ്കിൽ(!ctx->httpsrr_resolved) {
    ctx->httpsrr_resolved = Curl_conn_dns_resolved_https(
      data, cf->sockindex, ctx->destination);
#ifdef DEBUGBUILD
    എങ്കിൽ(!ctx->httpsrr_resolved && getenv("CURL_DBG_AWAIT_HTTPSRR")) {
      CURL_TRC_CF(data, cf, "awaiting HTTPS-RR");
      തിരികെ CURLE_OK;
    }
#endif
  }

  തിരഞ്ഞെടുപ്പ്(ctx->state) {
  അവസ്ഥ CF_HC_RESOLV:
    ctx->state = CF_HC_INIT;
    FALLTHROUGH();

  അവസ്ഥ CF_HC_INIT:
    DEBUGASSERT(!cf->next);
    CURL_TRC_CF(data, cf, "connect, init");
    result = cf_hc_set_baller1(cf, data);
    എങ്കിൽ(result) {
      ctx->result = result;
      ctx->state = CF_HC_FAILURE;
      പോകുക out;
    }
    cf_hc_set_baller2(cf, data);
    pnow = Curl_pgrs_now(data);
    ctx->started = *pnow;
    cf_hc_baller_init(&ctx->ballers[0], cf, data);
    എങ്കിൽ((ctx->baller_എണ്ണം > 1) || !ctx->ballers_complete) {
      Curl_expire_set(data, EXPIRE_ALPN_EYEBALLS,
                      ctx->soft_eyeballs_timeout_ms, pnow);
    }
    ctx->state = CF_HC_CONNECT;
    FALLTHROUGH();

  അവസ്ഥ CF_HC_CONNECT:
    എങ്കിൽ(!ctx->ballers_complete)
      cf_hc_set_baller2(cf, data);

    എങ്കിൽ(cf_hc_baller_is_connecting(&ctx->ballers[0])) {
      result = cf_hc_baller_connect(&ctx->ballers[0], cf, data, done);
      എങ്കിൽ(!result && *done) {
        result = baller_connected(cf, data, &ctx->ballers[0]);
        പോകുക out;
      }
    }

    pnow = Curl_pgrs_now(data);
    എങ്കിൽ(time_to_start_baller2(cf, data, pnow)) {
      cf_hc_baller_init(&ctx->ballers[1], cf, data);
    }

    എങ്കിൽ(cf_hc_baller_is_connecting(&ctx->ballers[1])) {
      result = cf_hc_baller_connect(&ctx->ballers[1], cf, data, done);
      എങ്കിൽ(!result && *done) {
        result = baller_connected(cf, data, &ctx->ballers[1]);
        പോകുക out;
      }
    }

    എങ്കിൽ(ctx->ballers[0].result &&
       (ctx->ballers[1].result ||
        (ctx->ballers_complete && (ctx->baller_എണ്ണം < 2)))) {
      /* all have failed. we give up */
      CURL_TRC_CF(data, cf, "connect, all attempts failed");
      ctx->result = result = ctx->ballers[0].result;
      ctx->state = CF_HC_FAILURE;
      പോകുക out;
    }
    result = CURLE_OK;
    *done = FALSE;
    നിർത്തുക;

  അവസ്ഥ CF_HC_FAILURE:
    result = ctx->result;
    cf->connected = FALSE;
    *done = FALSE;
    നിർത്തുക;

  അവസ്ഥ CF_HC_SUCCESS:
    result = CURLE_OK;
    cf->connected = TRUE;
    *done = TRUE;
    നിർത്തുക;
  }

out:
  CURL_TRC_CF(data, cf, "connect -> %d, done=%d", (പൂർണ്ണസംഖ്യ)result, *done);
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_hc_shutdown(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data, bool *done)
{
  ഘടന cf_hc_ctx *ctx = cf->ctx;
  size_t i;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  എങ്കിൽ(cf->connected) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  വേണ്ടി(i = 0; i < ctx->baller_എണ്ണം; i++) {
    ഘടന cf_hc_baller *b = &ctx->ballers[i];
    bool bdone = FALSE;
    എങ്കിൽ(!cf_hc_baller_is_connecting(b) || b->shutdown)
      തുടരുക;
    b->result = b->cf->cft->do_shutdown(b->cf, data, &bdone);
    എങ്കിൽ(b->result || bdone)
      b->shutdown = TRUE; /* treat a failed shutdown as done */
  }

  *done = TRUE;
  വേണ്ടി(i = 0; i < ctx->baller_എണ്ണം; i++) {
    എങ്കിൽ(!ctx->ballers[i].shutdown)
      *done = FALSE;
  }
  എങ്കിൽ(*done) {
    വേണ്ടി(i = 0; i < ctx->baller_എണ്ണം; i++) {
      എങ്കിൽ(ctx->ballers[i].result)
        result = ctx->ballers[i].result;
    }
  }
  CURL_TRC_CF(data, cf, "shutdown -> %d, done=%d", (പൂർണ്ണസംഖ്യ)result, *done);
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_hc_adjust_pollset(ഘടന Curl_cfilter *cf,
                                     ഘടന Curl_easy *data,
                                     ഘടന easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  എങ്കിൽ(!cf->connected) {
    ഘടന cf_hc_ctx *ctx = cf->ctx;
    size_t i;

    വേണ്ടി(i = 0; (i < ctx->baller_എണ്ണം) && !result; i++) {
      ഘടന cf_hc_baller *b = &ctx->ballers[i];
      എങ്കിൽ(!cf_hc_baller_is_connecting(b))
        തുടരുക;
      result = Curl_conn_cf_adjust_pollset(b->cf, data, ps);
    }
    CURL_TRC_CF(data, cf, "adjust_pollset -> %d, %u socks", (പൂർണ്ണസംഖ്യ)result,
                ps->n);
  }
  തിരികെ result;
}

സ്ഥിതം bool cf_hc_data_pending(ഘടന Curl_cfilter *cf,
                               സ്ഥിരം ഘടന Curl_easy *data)
{
  ഘടന cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  എങ്കിൽ(cf->connected)
    തിരികെ cf->next->cft->has_data_pending(cf->next, data);

  വേണ്ടി(i = 0; i < ctx->baller_എണ്ണം; i++)
    എങ്കിൽ(cf_hc_baller_data_pending(&ctx->ballers[i], data))
      തിരികെ TRUE;
  തിരികെ FALSE;
}

സ്ഥിതം CURLcode cf_hc_query(ഘടന Curl_cfilter *cf,
                            ഘടന Curl_easy *data,
                            പൂർണ്ണസംഖ്യ query, പൂർണ്ണസംഖ്യ *pres1, ശൂന്യം *pres2)
{
  ഘടന cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  എങ്കിൽ(!cf->connected) {
    തിരഞ്ഞെടുപ്പ്(query) {
    അവസ്ഥ CF_QUERY_NEED_FLUSH: {
      വേണ്ടി(i = 0; i < ctx->baller_എണ്ണം; i++)
        എങ്കിൽ(cf_hc_baller_needs_flush(&ctx->ballers[i], data)) {
          *pres1 = TRUE;
          തിരികെ CURLE_OK;
        }
      നിർത്തുക;
    }
    സ്ഥിരസ്ഥിതി:
      നിർത്തുക;
    }
  }
  തിരികെ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

സ്ഥിതം CURLcode cf_hc_cntrl(ഘടന Curl_cfilter *cf,
                            ഘടന Curl_easy *data,
                            പൂർണ്ണസംഖ്യ event, പൂർണ്ണസംഖ്യ arg1, ശൂന്യം *arg2)
{
  ഘടന cf_hc_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t i;

  എങ്കിൽ(!cf->connected) {
    തിരഞ്ഞെടുപ്പ്(event) {
    അവസ്ഥ CF_CTRL_REPORT_STATS:
    അവസ്ഥ CF_CTRL_CONN_INFO_UPDATE:
      വേണ്ടി(i = 0; i < ctx->baller_എണ്ണം; i++) {
        /* Make the first baller that connected at network level report */
        എങ്കിൽ(Curl_conn_cf_is_ip_connected(ctx->ballers[i].cf, data)) {
          Curl_conn_cf_cntrl(ctx->ballers[i].cf, data, TRUE,
                             event, arg1, arg2);
          നിർത്തുക;
        }
      }
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      വേണ്ടി(i = 0; i < ctx->baller_എണ്ണം; i++) {
        result = cf_hc_baller_cntrl(&ctx->ballers[i], data, event, arg1, arg2);
        എങ്കിൽ(result && (result != CURLE_AGAIN))
          പോകുക out;
      }
      result = CURLE_OK;
      നിർത്തുക;
    }
  }
out:
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം cf_hc_destroy(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data)
{
  ഘടന cf_hc_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  cf_hc_ctx_destroy(data, ctx);
}

ഘടന Curl_cftype Curl_cft_http_connect = {
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

സ്ഥിതം CURLcode cf_hc_create(ഘടന Curl_cfilter **pcf,
                             ഘടന Curl_easy *data,
                             ഘടന Curl_peer *destination,
                             uint8_t def_transport)
{
  ഘടന Curl_cfilter *cf = ശൂന്യ;
  ഘടന cf_hc_ctx *ctx;
  CURLcode result = CURLE_OK;

  ctx = curlx_calloc(1, വലുപ്പം(*ctx));
  എങ്കിൽ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }
  Curl_peer_link(&ctx->destination, destination);
  ctx->def_transport = def_transport;
  ctx->hard_eyeballs_timeout_ms = data->set.happy_eyeballs_timeout;
  ctx->soft_eyeballs_timeout_ms = data->set.happy_eyeballs_timeout / 2;

  result = Curl_cf_create(&cf, &Curl_cft_http_connect, ctx);
  എങ്കിൽ(result)
    പോകുക out;
  ctx = ശൂന്യ;

out:
  *pcf = result ? ശൂന്യ : cf;
  cf_hc_ctx_destroy(data, ctx);
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_hc_add(ഘടന Curl_easy *data,
                          ഘടന Curl_peer *destination,
                          ഘടന connectdata *conn,
                          int8_t sockindex,
                          uint8_t def_transport)
{
  ഘടന Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_hc_create(&cf, data, destination, def_transport);
  എങ്കിൽ(result)
    പോകുക out;
  Curl_conn_cf_add(data, conn, sockindex, cf);

#ifdef USE_HTTPSRR
  result = Curl_conn_dns_add_https_resolve(data, cf->conn, cf->sockindex,
                                           destination);
#endif
out:
  തിരികെ result;
}

CURLcode Curl_cf_https_setup(ഘടന Curl_easy *data,
                             ഘടന Curl_peer *destination,
                             ഘടന connectdata *conn,
                             int8_t sockindex)
{
  CURLcode result = CURLE_OK;

  DEBUGASSERT(conn->scheme->protocol == CURLPROTO_HTTPS);

  /* This filter is intended for HTTPS using ALPN and does
   * not support HTTPS Eyeballing to a proxy. */
  എങ്കിൽ((conn->scheme->protocol != CURLPROTO_HTTPS) ||
#ifndef CURL_DISABLE_PROXY
     conn->bits.origin_is_proxy ||
#endif
     !conn->bits.tls_enable_alpn)
    പോകുക out;

  result = cf_hc_add(data, destination, conn, sockindex,
                     conn->transport_wanted);

out:
  തിരികെ result;
}

#endif /* !CURL_DISABLE_HTTP */
