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
#شامل "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#شامل "urldata.h"
#شامل "curl_trc.h"
#شامل "cfilters.h"
#شامل "cf-setup.h"
#شامل "connect.h"
#شامل "multiif.h"
#شامل "cf-https-connect.h"
#شامل "http2.h"
#شامل "progress.h"
#شامل "select.h"
#شامل "vdns/cf-dns.h"
#شامل "vdns/hostip.h"
#شامل "vdns/httpsrr.h"
#شامل "vquic/vquic.h"

تعریفنوع شمارش {
  CF_HC_RESOLV,
  CF_HC_INIT,
  CF_HC_CONNECT,
  CF_HC_SUCCESS,
  CF_HC_FAILURE
} cf_hc_state;

ساختار cf_hc_baller {
  ثابت نویسه *name;
  ساختار Curl_cfilter *cf;
  CURLcode result;
  صحیح reply_ms;
  uint8_t transport;
  شمارش alpnid alpn_id;
  BIT(shutdown);
};

ایستا تهی cf_hc_baller_discard(ساختار cf_hc_baller *b,
                                 ساختار Curl_easy *data)
{
  اگر(b->cf) {
    Curl_conn_cf_discard_chain(&b->cf, data);
    b->cf = پوچ;
  }
}

ایستا bool cf_hc_baller_is_connecting(ساختار cf_hc_baller *b)
{
  بازگشت b->cf && !b->result;
}

ایستا bool cf_hc_baller_has_started(ساختار cf_hc_baller *b)
{
  بازگشت !!b->cf;
}

ایستا صحیح cf_hc_baller_reply_ms(ساختار cf_hc_baller *b,
                                 ساختار Curl_easy *data)
{
  اگر(b->cf && (b->reply_ms < 0))
    b->cf->cft->query(b->cf, data, CF_QUERY_CONNECT_REPLY_MS,
                      &b->reply_ms, پوچ);
  بازگشت b->reply_ms;
}

ایستا bool cf_hc_baller_data_pending(ساختار cf_hc_baller *b,
                                      ثابت ساختار Curl_easy *data)
{
  بازگشت b->cf && !b->result && b->cf->cft->has_data_pending(b->cf, data);
}

ایستا bool cf_hc_baller_needs_flush(ساختار cf_hc_baller *b,
                                     ساختار Curl_easy *data)
{
  بازگشت b->cf && !b->result && Curl_conn_cf_needs_flush(b->cf, data);
}

ایستا CURLcode cf_hc_baller_cntrl(ساختار cf_hc_baller *b,
                                   ساختار Curl_easy *data,
                                   صحیح event, صحیح arg1, تهی *arg2)
{
  اگر(b->cf && !b->result)
    بازگشت Curl_conn_cf_cntrl(b->cf, data, FALSE, event, arg1, arg2);
  بازگشت CURLE_OK;
}

ساختار cf_hc_ctx {
  cf_hc_state state;
  ساختار Curl_peer *destination; /* who we ultimately want to talk to */
  ساختار curltime started;  /* when connect started */
  CURLcode result;          /* overall result */
  CURLcode check_h3_result;
  ساختار cf_hc_baller ballers[2];
  size_t baller_شمار;
  timediff_t soft_eyeballs_timeout_ms;
  timediff_t hard_eyeballs_timeout_ms;
  uint8_t def_transport;
  BIT(httpsrr_resolved);
  BIT(checked_h3);
  BIT(ballers_complete);
};

ایستا تهی cf_hc_ctx_destroy(ساختار Curl_easy *data,
                              ساختار cf_hc_ctx *ctx)
{
  اگر(ctx) {
    size_t i;
    برای(i = 0; i < ctx->baller_شمار; ++i)
      cf_hc_baller_discard(&ctx->ballers[i], data);
    Curl_peer_unlink(&ctx->destination);
    curlx_free(ctx);
  }
}

ایستا تهی cf_hc_baller_assign(ساختار cf_hc_baller *b,
                                شمارش alpnid alpn_id,
                                uint8_t def_transport)
{
  b->alpn_id = alpn_id;
  b->transport = def_transport;
  b->cf = پوچ;
  b->result = CURLE_OK;
  b->reply_ms = -1;
  b->shutdown = FALSE;
  انتخاب(b->alpn_id) {
  حالت ALPN_h3:
    b->name = "h3";
    b->transport = TRNSPRT_QUIC;
    بشکن;
  حالت ALPN_h2:
    b->name = "h2";
    بشکن;
  حالت ALPN_h1:
    b->name = "h1";
    بشکن;
  حالت ALPN_none:
    b->name = "no-alpn";
    بشکن;
  پیشفرض:
    b->result = CURLE_FAILED_INIT;
    بشکن;
  }
}

ایستا تهی cf_hc_baller_init(ساختار cf_hc_baller *b,
                              ساختار Curl_cfilter *cf,
                              ساختار Curl_easy *data)
{
  ساختار Curl_cfilter *save = cf->next;

  cf->next = پوچ;
  b->result = Curl_cf_setup_درج_after(cf, data, b->transport,
                                         CURL_CF_SSL_ENABLE);
  b->cf = cf->next;
  cf->next = save;
}

ایستا CURLcode cf_hc_baller_connect(ساختار cf_hc_baller *b,
                                     ساختار Curl_cfilter *cf,
                                     ساختار Curl_easy *data,
                                     bool *done)
{
  ساختار Curl_cfilter *save = cf->next;

  cf->next = b->cf;
  b->result = Curl_conn_cf_connect(cf->next, data, done);
  b->cf = cf->next; /* it might mutate */
  cf->next = save;
  بازگشت b->result;
}

ایستا CURLcode baller_connected(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 ساختار cf_hc_baller *winner)
{
  ساختار cf_hc_ctx *ctx = cf->ctx;

  /* Make the winner's connection filter out own sub-filter, check, move,
   * close all remaining. */
  اگر(cf->next) {
    DEBUGASSERT(0);
    بازگشت CURLE_FAILED_INIT;
  }
  اگر(!winner->cf) {
    DEBUGASSERT(0);
    بازگشت CURLE_FAILED_INIT;
  }

  cf->next = winner->cf;
  winner->cf = پوچ;
  ctx->state = CF_HC_SUCCESS;
  cf->connected = TRUE;

  /* ballers may have failf()'d, the winner resets it, so our
   * errorbuf is clean again. */
  Curl_reset_fail(data);

#ifdef USE_NGHTTP2
  {
    /* For a negotiated HTTP/2 connection insert the h2 filter. */
    ثابت نویسه *alpn = Curl_conn_cf_get_alpn_negotiated(cf->next, data);
    اگر(alpn && !strcmp("h2", alpn)) {
      CURLcode result = Curl_http2_switch_at(cf, data);
      اگر(result) {
        ctx->state = CF_HC_FAILURE;
        ctx->result = result;
        بازگشت result;
      }
    }
  }
#endif
  بازگشت CURLE_OK;
}

ایستا bool time_to_start_baller2(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data,
                                  ثابت ساختار curltime *pnow)
{
  ساختار cf_hc_ctx *ctx = cf->ctx;
  timediff_t elapsed_ms;

  اگر(ctx->baller_شمار < 2)
    بازگشت FALSE;
  وگرنه اگر(cf_hc_baller_has_started(&ctx->ballers[1]))
    بازگشت FALSE;
  وگرنه اگر(ctx->ballers[0].result) {
    CURL_TRC_CF(data, cf, "%s baller failed, starting %s",
                ctx->ballers[0].name, ctx->ballers[1].name);
    بازگشت TRUE;
  }

  elapsed_ms = curlx_ptimediff_ms(pnow, &ctx->started);
  اگر(elapsed_ms >= ctx->hard_eyeballs_timeout_ms) {
    CURL_TRC_CF(data, cf, "%s inconclusive after %" FMT_TIMEDIFF_T ", "
                "starting %s", ctx->ballers[0].name,
                ctx->hard_eyeballs_timeout_ms, ctx->ballers[1].name);
    بازگشت TRUE;
  }
  وگرنه اگر(elapsed_ms >= ctx->soft_eyeballs_timeout_ms) {
    اگر(cf_hc_baller_reply_ms(&ctx->ballers[0], data) < 0) {
      CURL_TRC_CF(data, cf, "%s has not seen any data after %"
                  FMT_TIMEDIFF_T "ms, starting %s",
                  ctx->ballers[0].name, ctx->soft_eyeballs_timeout_ms,
                  ctx->ballers[1].name);
      بازگشت TRUE;
    }
    /* set the effective hard timeout again */
    Curl_expire_set(data, EXPIRE_ALPN_EYEBALLS,
                    ctx->hard_eyeballs_timeout_ms - elapsed_ms, pnow);
  }
  بازگشت FALSE;
}

ایستا bool cf_hc_may_h3(ساختار Curl_cfilter *cf,
                         ساختار Curl_easy *data)
{
  ساختار cf_hc_ctx *ctx = cf->ctx;
  اگر(!ctx->checked_h3) {
    ctx->check_h3_result =
      Curl_conn_may_http3(data, cf->conn);
    ctx->checked_h3 = TRUE;
  }
  بازگشت !ctx->check_h3_result;
}

ایستا شمارش alpnid cf_hc_get_httpsrr_alpn(ساختار Curl_cfilter *cf,
                                          ساختار Curl_easy *data,
                                          شمارش alpnid not_this_one)
{
#ifdef USE_HTTPSRR
  ساختار cf_hc_ctx *ctx = cf->ctx;
  /* Is there an HTTPSRR use its ALPNs here.
   * We are here after having selected a connection to a host+port and
   * can no longer change that. Any HTTPSRR advice for other hosts and ports
   * we need to ignore. */
  ثابت ساختار Curl_https_rrinfo *rr;
  size_t i;

  /* Do we have HTTPS-RR information? */
  rr = Curl_conn_dns_get_https(data, cf->sockindex, ctx->destination);

  CURL_TRC_CF(data, cf, "HTTPS-RR %savailable", rr ? "" : "not ");
  /* We do not support `rr->no_def_alpn`. */
  اگر(Curl_httpsrr_applicable(data, rr) && !rr->no_def_alpn) {
    برای(i = 0; i < CURL_ARRAYSIZE(rr->alpns); ++i) {
      شمارش alpnid alpn_rr = (شمارش alpnid)rr->alpns[i];
      اگر(alpn_rr == not_this_one) /* do not want this one */
        ادامه;
      انتخاب(alpn_rr) {
      حالت ALPN_h3:
        اگر((data->state.http_neg.allowed & CURL_HTTP_V3x) &&
           cf_hc_may_h3(cf, data)) {
          بازگشت alpn_rr;
        }
        بشکن;
      حالت ALPN_h2:
        اگر(data->state.http_neg.allowed & CURL_HTTP_V2x) {
          بازگشت alpn_rr;
        }
        بشکن;
      حالت ALPN_h1:
        اگر(data->state.http_neg.allowed & CURL_HTTP_V1x) {
          بازگشت alpn_rr;
        }
        بشکن;
      پیشفرض: /* ignore */
        بشکن;
      }
    }
  }
#وگرنه
  (تهی)cf;
  (تهی)data;
  (تهی)not_this_one;
#endif
  بازگشت ALPN_none;
}

ایستا شمارش alpnid cf_hc_get_pref_alpn(ساختار Curl_cfilter *cf,
                                       ساختار Curl_easy *data,
                                       شمارش alpnid not_this_one)
{
  اگر((data->state.http_neg.preferred & data->state.http_neg.allowed)) {
    انتخاب(data->state.http_neg.preferred) {
    حالت CURL_HTTP_V3x:
      اگر(cf_hc_may_h3(cf, data) && (ALPN_h3 != not_this_one))
        بازگشت ALPN_h3;
      بشکن;
    حالت CURL_HTTP_V2x:
      اگر(ALPN_h2 != not_this_one)
        بازگشت ALPN_h2;
      بشکن;
    حالت CURL_HTTP_V1x:
      /* If we are trying h2 already, h1 is already used as fallback */
      اگر((ALPN_h1 != not_this_one) && (ALPN_h2 != not_this_one))
        بازگشت ALPN_h1;
      بشکن;
    پیشفرض:
      بشکن;
    }
  }
  بازگشت ALPN_none;
}

ایستا شمارش alpnid cf_hc_get_first_alpn(ساختار Curl_cfilter *cf,
                                        ساختار Curl_easy *data,
                                        http_majors choices,
                                        شمارش alpnid not_this_one)
{
  /* When told to not try h2, we also do not try h1 and vice versa */
  bool allow_h1_or_h2 = (not_this_one != ALPN_h1) &&
                        (not_this_one != ALPN_h2);
  اگر((ALPN_h3 != not_this_one) && (choices & CURL_HTTP_V3x) &&
     cf_hc_may_h3(cf, data)) {
    بازگشت ALPN_h3;
  }
  اگر(allow_h1_or_h2 && (choices & CURL_HTTP_V2x)) {
    بازگشت ALPN_h2;
  }
  اگر(allow_h1_or_h2 && (choices & CURL_HTTP_V1x)) {
    بازگشت ALPN_h1;
  }
  بازگشت ALPN_none;
}

ایستا CURLcode cf_hc_set_baller1(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data)
{
  ساختار cf_hc_ctx *ctx = cf->ctx;
  شمارش alpnid alpn1 = ALPN_none;
  VERBOSE(ثابت نویسه *source = "HTTPS-RR");

  DEBUGASSERT(cf->conn->bits.tls_enable_alpn);

  alpn1 = cf_hc_get_httpsrr_alpn(cf, data, ALPN_none);
  اگر(alpn1 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(source = "preferred version");
    alpn1 = cf_hc_get_pref_alpn(cf, data, ALPN_none);
  }
  اگر(alpn1 == ALPN_none) {
    VERBOSE(source = "wanted versions");
    alpn1 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.wanted,
                                 ALPN_none);
  }
  اگر(alpn1 == ALPN_none) {
    VERBOSE(source = "allowed versions");
    alpn1 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.allowed,
                                 ALPN_none);
  }

  اگر(alpn1 == ALPN_none) {
    /* None of the wanted/allowed HTTP versions could be chosen */
    اگر(ctx->check_h3_result) {
      CURL_TRC_CF(data, cf, "unable to use HTTP/3");
      بازگشت ctx->check_h3_result;
    }
    CURL_TRC_CF(data, cf, "unable to select HTTP version");
    بازگشت CURLE_FAILED_INIT;
  }

  cf_hc_baller_assign(&ctx->ballers[0], alpn1, ctx->def_transport);
  ctx->baller_شمار = 1;
  CURL_TRC_CF(data, cf, "1st attempt uses %s from %s",
              ctx->ballers[0].name, source);

  انتخاب(alpn1) {
  حالت ALPN_h1:
    /* We really want h1, switch off h2 to make it disappear in ALPN */
    data->state.http_neg.wanted &= (uint8_t)~CURL_HTTP_V2x;
    بشکن;
  پیشفرض:
    بشکن;
  }

  بازگشت CURLE_OK;
}

ایستا تهی cf_hc_set_baller2(ساختار Curl_cfilter *cf,
                              ساختار Curl_easy *data)
{
  ساختار cf_hc_ctx *ctx = cf->ctx;
  شمارش alpnid alpn2 = ALPN_none, alpn1 = ctx->ballers[0].alpn_id;
  VERBOSE(ثابت نویسه *source = "HTTPS-RR");

  اگر(ctx->ballers_complete)
    بازگشت; /* already done */
  اگر(!ctx->httpsrr_resolved)
    بازگشت; /* HTTPS-RR pending */

  alpn2 = cf_hc_get_httpsrr_alpn(cf, data, alpn1);
  اگر(alpn2 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(source = "preferred version");
    alpn2 = cf_hc_get_pref_alpn(cf, data, alpn1);
  }
  اگر(alpn2 == ALPN_none) {
    VERBOSE(source = "wanted versions");
    alpn2 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.wanted,
                                 alpn1);
  }

  اگر(alpn2 != ALPN_none) {
    cf_hc_baller_assign(&ctx->ballers[1], alpn2, ctx->def_transport);
    ctx->baller_شمار = 2;
    CURL_TRC_CF(data, cf, "2nd attempt uses %s from %s",
                ctx->ballers[1].name, source);
  }
  ctx->ballers_complete = TRUE;
}

ایستا CURLcode cf_hc_connect(ساختار Curl_cfilter *cf,
                              ساختار Curl_easy *data,
                              bool *done)
{
  ساختار cf_hc_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ثابت ساختار curltime *pnow = پوچ;

  اگر(cf->connected) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  *done = FALSE;

  اگر(!ctx->httpsrr_resolved) {
    ctx->httpsrr_resolved = Curl_conn_dns_resolved_https(
      data, cf->sockindex, ctx->destination);
#ifdef DEBUGBUILD
    اگر(!ctx->httpsrr_resolved && getenv("CURL_DBG_AWAIT_HTTPSRR")) {
      CURL_TRC_CF(data, cf, "awaiting HTTPS-RR");
      بازگشت CURLE_OK;
    }
#endif
  }

  انتخاب(ctx->state) {
  حالت CF_HC_RESOLV:
    ctx->state = CF_HC_INIT;
    FALLTHROUGH();

  حالت CF_HC_INIT:
    DEBUGASSERT(!cf->next);
    CURL_TRC_CF(data, cf, "connect, init");
    result = cf_hc_set_baller1(cf, data);
    اگر(result) {
      ctx->result = result;
      ctx->state = CF_HC_FAILURE;
      برو out;
    }
    cf_hc_set_baller2(cf, data);
    pnow = Curl_pgrs_now(data);
    ctx->started = *pnow;
    cf_hc_baller_init(&ctx->ballers[0], cf, data);
    اگر((ctx->baller_شمار > 1) || !ctx->ballers_complete) {
      Curl_expire_set(data, EXPIRE_ALPN_EYEBALLS,
                      ctx->soft_eyeballs_timeout_ms, pnow);
    }
    ctx->state = CF_HC_CONNECT;
    FALLTHROUGH();

  حالت CF_HC_CONNECT:
    اگر(!ctx->ballers_complete)
      cf_hc_set_baller2(cf, data);

    اگر(cf_hc_baller_is_connecting(&ctx->ballers[0])) {
      result = cf_hc_baller_connect(&ctx->ballers[0], cf, data, done);
      اگر(!result && *done) {
        result = baller_connected(cf, data, &ctx->ballers[0]);
        برو out;
      }
    }

    pnow = Curl_pgrs_now(data);
    اگر(time_to_start_baller2(cf, data, pnow)) {
      cf_hc_baller_init(&ctx->ballers[1], cf, data);
    }

    اگر(cf_hc_baller_is_connecting(&ctx->ballers[1])) {
      result = cf_hc_baller_connect(&ctx->ballers[1], cf, data, done);
      اگر(!result && *done) {
        result = baller_connected(cf, data, &ctx->ballers[1]);
        برو out;
      }
    }

    اگر(ctx->ballers[0].result &&
       (ctx->ballers[1].result ||
        (ctx->ballers_complete && (ctx->baller_شمار < 2)))) {
      /* all have failed. we give up */
      CURL_TRC_CF(data, cf, "connect, all attempts failed");
      ctx->result = result = ctx->ballers[0].result;
      ctx->state = CF_HC_FAILURE;
      برو out;
    }
    result = CURLE_OK;
    *done = FALSE;
    بشکن;

  حالت CF_HC_FAILURE:
    result = ctx->result;
    cf->connected = FALSE;
    *done = FALSE;
    بشکن;

  حالت CF_HC_SUCCESS:
    result = CURLE_OK;
    cf->connected = TRUE;
    *done = TRUE;
    بشکن;
  }

out:
  CURL_TRC_CF(data, cf, "connect -> %d, done=%d", (صحیح)result, *done);
  بازگشت result;
}

ایستا CURLcode cf_hc_shutdown(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data, bool *done)
{
  ساختار cf_hc_ctx *ctx = cf->ctx;
  size_t i;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  اگر(cf->connected) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  برای(i = 0; i < ctx->baller_شمار; i++) {
    ساختار cf_hc_baller *b = &ctx->ballers[i];
    bool bdone = FALSE;
    اگر(!cf_hc_baller_is_connecting(b) || b->shutdown)
      ادامه;
    b->result = b->cf->cft->do_shutdown(b->cf, data, &bdone);
    اگر(b->result || bdone)
      b->shutdown = TRUE; /* treat a failed shutdown as done */
  }

  *done = TRUE;
  برای(i = 0; i < ctx->baller_شمار; i++) {
    اگر(!ctx->ballers[i].shutdown)
      *done = FALSE;
  }
  اگر(*done) {
    برای(i = 0; i < ctx->baller_شمار; i++) {
      اگر(ctx->ballers[i].result)
        result = ctx->ballers[i].result;
    }
  }
  CURL_TRC_CF(data, cf, "shutdown -> %d, done=%d", (صحیح)result, *done);
  بازگشت result;
}

ایستا CURLcode cf_hc_adjust_pollset(ساختار Curl_cfilter *cf,
                                     ساختار Curl_easy *data,
                                     ساختار easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  اگر(!cf->connected) {
    ساختار cf_hc_ctx *ctx = cf->ctx;
    size_t i;

    برای(i = 0; (i < ctx->baller_شمار) && !result; i++) {
      ساختار cf_hc_baller *b = &ctx->ballers[i];
      اگر(!cf_hc_baller_is_connecting(b))
        ادامه;
      result = Curl_conn_cf_adjust_pollset(b->cf, data, ps);
    }
    CURL_TRC_CF(data, cf, "adjust_pollset -> %d, %u socks", (صحیح)result,
                ps->n);
  }
  بازگشت result;
}

ایستا bool cf_hc_data_pending(ساختار Curl_cfilter *cf,
                               ثابت ساختار Curl_easy *data)
{
  ساختار cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  اگر(cf->connected)
    بازگشت cf->next->cft->has_data_pending(cf->next, data);

  برای(i = 0; i < ctx->baller_شمار; i++)
    اگر(cf_hc_baller_data_pending(&ctx->ballers[i], data))
      بازگشت TRUE;
  بازگشت FALSE;
}

ایستا CURLcode cf_hc_query(ساختار Curl_cfilter *cf,
                            ساختار Curl_easy *data,
                            صحیح query, صحیح *pres1, تهی *pres2)
{
  ساختار cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  اگر(!cf->connected) {
    انتخاب(query) {
    حالت CF_QUERY_NEED_FLUSH: {
      برای(i = 0; i < ctx->baller_شمار; i++)
        اگر(cf_hc_baller_needs_flush(&ctx->ballers[i], data)) {
          *pres1 = TRUE;
          بازگشت CURLE_OK;
        }
      بشکن;
    }
    پیشفرض:
      بشکن;
    }
  }
  بازگشت cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ایستا CURLcode cf_hc_cntrl(ساختار Curl_cfilter *cf,
                            ساختار Curl_easy *data,
                            صحیح event, صحیح arg1, تهی *arg2)
{
  ساختار cf_hc_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t i;

  اگر(!cf->connected) {
    انتخاب(event) {
    حالت CF_CTRL_REPORT_STATS:
    حالت CF_CTRL_CONN_INFO_UPDATE:
      برای(i = 0; i < ctx->baller_شمار; i++) {
        /* Make the first baller that connected at network level report */
        اگر(Curl_conn_cf_is_ip_connected(ctx->ballers[i].cf, data)) {
          Curl_conn_cf_cntrl(ctx->ballers[i].cf, data, TRUE,
                             event, arg1, arg2);
          بشکن;
        }
      }
      بشکن;
    پیشفرض:
      برای(i = 0; i < ctx->baller_شمار; i++) {
        result = cf_hc_baller_cntrl(&ctx->ballers[i], data, event, arg1, arg2);
        اگر(result && (result != CURLE_AGAIN))
          برو out;
      }
      result = CURLE_OK;
      بشکن;
    }
  }
out:
  بازگشت result;
}

ایستا تهی cf_hc_destroy(ساختار Curl_cfilter *cf, ساختار Curl_easy *data)
{
  ساختار cf_hc_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  cf_hc_ctx_destroy(data, ctx);
}

ساختار Curl_cftype Curl_cft_http_connect = {
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

ایستا CURLcode cf_hc_create(ساختار Curl_cfilter **pcf,
                             ساختار Curl_easy *data,
                             ساختار Curl_peer *destination,
                             uint8_t def_transport)
{
  ساختار Curl_cfilter *cf = پوچ;
  ساختار cf_hc_ctx *ctx;
  CURLcode result = CURLE_OK;

  ctx = curlx_calloc(1, اندازه(*ctx));
  اگر(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }
  Curl_peer_link(&ctx->destination, destination);
  ctx->def_transport = def_transport;
  ctx->hard_eyeballs_timeout_ms = data->set.happy_eyeballs_timeout;
  ctx->soft_eyeballs_timeout_ms = data->set.happy_eyeballs_timeout / 2;

  result = Curl_cf_create(&cf, &Curl_cft_http_connect, ctx);
  اگر(result)
    برو out;
  ctx = پوچ;

out:
  *pcf = result ? پوچ : cf;
  cf_hc_ctx_destroy(data, ctx);
  بازگشت result;
}

ایستا CURLcode cf_hc_add(ساختار Curl_easy *data,
                          ساختار Curl_peer *destination,
                          ساختار connectdata *conn,
                          int8_t sockindex,
                          uint8_t def_transport)
{
  ساختار Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_hc_create(&cf, data, destination, def_transport);
  اگر(result)
    برو out;
  Curl_conn_cf_add(data, conn, sockindex, cf);

#ifdef USE_HTTPSRR
  result = Curl_conn_dns_add_https_resolve(data, cf->conn, cf->sockindex,
                                           destination);
#endif
out:
  بازگشت result;
}

CURLcode Curl_cf_https_setup(ساختار Curl_easy *data,
                             ساختار Curl_peer *destination,
                             ساختار connectdata *conn,
                             int8_t sockindex)
{
  CURLcode result = CURLE_OK;

  DEBUGASSERT(conn->scheme->protocol == CURLPROTO_HTTPS);

  /* This filter is intended for HTTPS using ALPN and does
   * not support HTTPS Eyeballing to a proxy. */
  اگر((conn->scheme->protocol != CURLPROTO_HTTPS) ||
#ifndef CURL_DISABLE_PROXY
     conn->bits.origin_is_proxy ||
#endif
     !conn->bits.tls_enable_alpn)
    برو out;

  result = cf_hc_add(data, destination, conn, sockindex,
                     conn->transport_wanted);

out:
  بازگشت result;
}

#endif /* !CURL_DISABLE_HTTP */
