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

#ifndef CURL_DISABLE_HTTP

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
  CURLcode result;
  صحيح reply_ms;
  uint8_t transport;
  تعداد alpnid alpn_id;
  BIT(shutdown);
};

ساكن فراغ cf_hc_baller_discard(بنية cf_hc_baller *b,
                                 بنية Curl_easy *data)
{
  אם(b->cf) {
    Curl_conn_cf_discard_chain(&b->cf, data);
    b->cf = عدم;
  }
}

ساكن bool cf_hc_baller_is_connecting(بنية cf_hc_baller *b)
{
  החזר b->cf && !b->result;
}

ساكن bool cf_hc_baller_has_started(بنية cf_hc_baller *b)
{
  החזר !!b->cf;
}

ساكن صحيح cf_hc_baller_reply_ms(بنية cf_hc_baller *b,
                                 بنية Curl_easy *data)
{
  אם(b->cf && (b->reply_ms < 0))
    b->cf->cft->query(b->cf, data, CF_QUERY_CONNECT_REPLY_MS,
                      &b->reply_ms, عدم);
  החזר b->reply_ms;
}

ساكن bool cf_hc_baller_data_pending(بنية cf_hc_baller *b,
                                      ثابت بنية Curl_easy *data)
{
  החזר b->cf && !b->result && b->cf->cft->has_data_pending(b->cf, data);
}

ساكن bool cf_hc_baller_needs_flush(بنية cf_hc_baller *b,
                                     بنية Curl_easy *data)
{
  החזר b->cf && !b->result && Curl_conn_cf_needs_flush(b->cf, data);
}

ساكن CURLcode cf_hc_baller_cntrl(بنية cf_hc_baller *b,
                                   بنية Curl_easy *data,
                                   صحيح event, صحيح arg1, فراغ *arg2)
{
  אם(b->cf && !b->result)
    החזר Curl_conn_cf_cntrl(b->cf, data, FALSE, event, arg1, arg2);
  החזר CURLE_OK;
}

بنية cf_hc_ctx {
  cf_hc_state state;
  بنية Curl_peer *destination; /* who we ultimately want to talk to */
  بنية curltime started;  /* when connect started */
  CURLcode result;          /* overall result */
  CURLcode check_h3_result;
  بنية cf_hc_baller ballers[2];
  size_t baller_count;
  timediff_t soft_eyeballs_timeout_ms;
  timediff_t hard_eyeballs_timeout_ms;
  uint8_t def_transport;
  BIT(httpsrr_resolved);
  BIT(checked_h3);
  BIT(ballers_complete);
};

ساكن فراغ cf_hc_ctx_destroy(بنية Curl_easy *data,
                              بنية cf_hc_ctx *ctx)
{
  אם(ctx) {
    size_t i;
    עבור(i = 0; i < ctx->baller_count; ++i)
      cf_hc_baller_discard(&ctx->ballers[i], data);
    Curl_peer_unlink(&ctx->destination);
    curlx_free(ctx);
  }
}

ساكن فراغ cf_hc_baller_assign(بنية cf_hc_baller *b,
                                تعداد alpnid alpn_id,
                                uint8_t def_transport)
{
  b->alpn_id = alpn_id;
  b->transport = def_transport;
  b->cf = عدم;
  b->result = CURLE_OK;
  b->reply_ms = -1;
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
    b->result = CURLE_FAILED_INIT;
    اكسر;
  }
}

ساكن فراغ cf_hc_baller_init(بنية cf_hc_baller *b,
                              بنية Curl_cfilter *cf,
                              بنية Curl_easy *data)
{
  بنية Curl_cfilter *save = cf->next;

  cf->next = عدم;
  b->result = Curl_cf_setup_insert_after(cf, data, b->transport,
                                         CURL_CF_SSL_ENABLE);
  b->cf = cf->next;
  cf->next = save;
}

ساكن CURLcode cf_hc_baller_connect(بنية cf_hc_baller *b,
                                     بنية Curl_cfilter *cf,
                                     بنية Curl_easy *data,
                                     bool *done)
{
  بنية Curl_cfilter *save = cf->next;

  cf->next = b->cf;
  b->result = Curl_conn_cf_connect(cf->next, data, done);
  b->cf = cf->next; /* it might mutate */
  cf->next = save;
  החזר b->result;
}

ساكن CURLcode baller_connected(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data,
                                 بنية cf_hc_baller *winner)
{
  بنية cf_hc_ctx *ctx = cf->ctx;

  /* Make the winner's connection filter out own sub-filter, check, move,
   * close all remaining. */
  אם(cf->next) {
    DEBUGASSERT(0);
    החזר CURLE_FAILED_INIT;
  }
  אם(!winner->cf) {
    DEBUGASSERT(0);
    החזר CURLE_FAILED_INIT;
  }

  cf->next = winner->cf;
  winner->cf = عدم;
  ctx->state = CF_HC_SUCCESS;
  cf->connected = TRUE;

  /* ballers may have failf()'d, the winner resets it, so our
   * errorbuf is clean again. */
  Curl_reset_fail(data);

#ifdef USE_NGHTTP2
  {
    /* For a negotiated HTTP/2 connection insert the h2 filter. */
    ثابت حرف *alpn = Curl_conn_cf_get_alpn_negotiated(cf->next, data);
    אם(alpn && !strcmp("h2", alpn)) {
      CURLcode result = Curl_http2_switch_at(cf, data);
      אם(result) {
        ctx->state = CF_HC_FAILURE;
        ctx->result = result;
        החזר result;
      }
    }
  }
#endif
  החזר CURLE_OK;
}

ساكن bool time_to_start_baller2(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data,
                                  ثابت بنية curltime *pnow)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  timediff_t elapsed_ms;

  אם(ctx->baller_count < 2)
    החזר FALSE;
  אחרת אם(cf_hc_baller_has_started(&ctx->ballers[1]))
    החזר FALSE;
  אחרת אם(ctx->ballers[0].result) {
    CURL_TRC_CF(data, cf, "%s baller failed, starting %s",
                ctx->ballers[0].name, ctx->ballers[1].name);
    החזר TRUE;
  }

  elapsed_ms = curlx_ptimediff_ms(pnow, &ctx->started);
  אם(elapsed_ms >= ctx->hard_eyeballs_timeout_ms) {
    CURL_TRC_CF(data, cf, "%s inconclusive after %" FMT_TIMEDIFF_T ", "
                "starting %s", ctx->ballers[0].name,
                ctx->hard_eyeballs_timeout_ms, ctx->ballers[1].name);
    החזר TRUE;
  }
  אחרת אם(elapsed_ms >= ctx->soft_eyeballs_timeout_ms) {
    אם(cf_hc_baller_reply_ms(&ctx->ballers[0], data) < 0) {
      CURL_TRC_CF(data, cf, "%s has not seen any data after %"
                  FMT_TIMEDIFF_T "ms, starting %s",
                  ctx->ballers[0].name, ctx->soft_eyeballs_timeout_ms,
                  ctx->ballers[1].name);
      החזר TRUE;
    }
    /* set the effective hard timeout again */
    Curl_expire_set(data, EXPIRE_ALPN_EYEBALLS,
                    ctx->hard_eyeballs_timeout_ms - elapsed_ms, pnow);
  }
  החזר FALSE;
}

ساكن bool cf_hc_may_h3(بنية Curl_cfilter *cf,
                         بنية Curl_easy *data)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  אם(!ctx->checked_h3) {
    ctx->check_h3_result =
      Curl_conn_may_http3(data, cf->conn);
    ctx->checked_h3 = TRUE;
  }
  החזר !ctx->check_h3_result;
}

ساكن تعداد alpnid cf_hc_get_httpsrr_alpn(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *data,
                                          تعداد alpnid not_this_one)
{
#ifdef USE_HTTPSRR
  بنية cf_hc_ctx *ctx = cf->ctx;
  /* Is there an HTTPSRR use its ALPNs here.
   * We are here after having selected a connection to a host+port and
   * can no longer change that. Any HTTPSRR advice for other hosts and ports
   * we need to ignore. */
  ثابت بنية Curl_https_rrinfo *rr;
  size_t i;

  /* Do we have HTTPS-RR information? */
  rr = Curl_conn_dns_get_https(data, cf->sockindex, ctx->destination);

  CURL_TRC_CF(data, cf, "HTTPS-RR %savailable", rr ? "" : "not ");
  /* We do not support `rr->no_def_alpn`. */
  אם(Curl_httpsrr_applicable(data, rr) && !rr->no_def_alpn) {
    עבור(i = 0; i < CURL_ARRAYSIZE(rr->alpns); ++i) {
      تعداد alpnid alpn_rr = (تعداد alpnid)rr->alpns[i];
      אם(alpn_rr == not_this_one) /* do not want this one */
        استمر;
      بدل(alpn_rr) {
      حالة ALPN_h3:
        אם((data->state.http_neg.allowed & CURL_HTTP_V3x) &&
           cf_hc_may_h3(cf, data)) {
          החזר alpn_rr;
        }
        اكسر;
      حالة ALPN_h2:
        אם(data->state.http_neg.allowed & CURL_HTTP_V2x) {
          החזר alpn_rr;
        }
        اكسر;
      حالة ALPN_h1:
        אם(data->state.http_neg.allowed & CURL_HTTP_V1x) {
          החזר alpn_rr;
        }
        اكسر;
      افتراضي: /* ignore */
        اكسر;
      }
    }
  }
#אחרת
  (فراغ)cf;
  (فراغ)data;
  (فراغ)not_this_one;
#endif
  החזר ALPN_none;
}

ساكن تعداد alpnid cf_hc_get_pref_alpn(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *data,
                                       تعداد alpnid not_this_one)
{
  אם((data->state.http_neg.preferred & data->state.http_neg.allowed)) {
    بدل(data->state.http_neg.preferred) {
    حالة CURL_HTTP_V3x:
      אם(cf_hc_may_h3(cf, data) && (ALPN_h3 != not_this_one))
        החזר ALPN_h3;
      اكسر;
    حالة CURL_HTTP_V2x:
      אם(ALPN_h2 != not_this_one)
        החזר ALPN_h2;
      اكسر;
    حالة CURL_HTTP_V1x:
      /* If we are trying h2 already, h1 is already used as fallback */
      אם((ALPN_h1 != not_this_one) && (ALPN_h2 != not_this_one))
        החזר ALPN_h1;
      اكسر;
    افتراضي:
      اكسر;
    }
  }
  החזר ALPN_none;
}

ساكن تعداد alpnid cf_hc_get_first_alpn(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *data,
                                        http_majors choices,
                                        تعداد alpnid not_this_one)
{
  /* When told to not try h2, we also do not try h1 and vice versa */
  bool allow_h1_or_h2 = (not_this_one != ALPN_h1) &&
                        (not_this_one != ALPN_h2);
  אם((ALPN_h3 != not_this_one) && (choices & CURL_HTTP_V3x) &&
     cf_hc_may_h3(cf, data)) {
    החזר ALPN_h3;
  }
  אם(allow_h1_or_h2 && (choices & CURL_HTTP_V2x)) {
    החזר ALPN_h2;
  }
  אם(allow_h1_or_h2 && (choices & CURL_HTTP_V1x)) {
    החזר ALPN_h1;
  }
  החזר ALPN_none;
}

ساكن CURLcode cf_hc_set_baller1(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  تعداد alpnid alpn1 = ALPN_none;
  VERBOSE(ثابت حرف *source = "HTTPS-RR");

  DEBUGASSERT(cf->conn->bits.tls_enable_alpn);

  alpn1 = cf_hc_get_httpsrr_alpn(cf, data, ALPN_none);
  אם(alpn1 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(source = "preferred version");
    alpn1 = cf_hc_get_pref_alpn(cf, data, ALPN_none);
  }
  אם(alpn1 == ALPN_none) {
    VERBOSE(source = "wanted versions");
    alpn1 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.wanted,
                                 ALPN_none);
  }
  אם(alpn1 == ALPN_none) {
    VERBOSE(source = "allowed versions");
    alpn1 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.allowed,
                                 ALPN_none);
  }

  אם(alpn1 == ALPN_none) {
    /* None of the wanted/allowed HTTP versions could be chosen */
    אם(ctx->check_h3_result) {
      CURL_TRC_CF(data, cf, "unable to use HTTP/3");
      החזר ctx->check_h3_result;
    }
    CURL_TRC_CF(data, cf, "unable to select HTTP version");
    החזר CURLE_FAILED_INIT;
  }

  cf_hc_baller_assign(&ctx->ballers[0], alpn1, ctx->def_transport);
  ctx->baller_count = 1;
  CURL_TRC_CF(data, cf, "1st attempt uses %s from %s",
              ctx->ballers[0].name, source);

  بدل(alpn1) {
  حالة ALPN_h1:
    /* We really want h1, switch off h2 to make it disappear in ALPN */
    data->state.http_neg.wanted &= (uint8_t)~CURL_HTTP_V2x;
    اكسر;
  افتراضي:
    اكسر;
  }

  החזר CURLE_OK;
}

ساكن فراغ cf_hc_set_baller2(بنية Curl_cfilter *cf,
                              بنية Curl_easy *data)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  تعداد alpnid alpn2 = ALPN_none, alpn1 = ctx->ballers[0].alpn_id;
  VERBOSE(ثابت حرف *source = "HTTPS-RR");

  אם(ctx->ballers_complete)
    החזר; /* already done */
  אם(!ctx->httpsrr_resolved)
    החזר; /* HTTPS-RR pending */

  alpn2 = cf_hc_get_httpsrr_alpn(cf, data, alpn1);
  אם(alpn2 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(source = "preferred version");
    alpn2 = cf_hc_get_pref_alpn(cf, data, alpn1);
  }
  אם(alpn2 == ALPN_none) {
    VERBOSE(source = "wanted versions");
    alpn2 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.wanted,
                                 alpn1);
  }

  אם(alpn2 != ALPN_none) {
    cf_hc_baller_assign(&ctx->ballers[1], alpn2, ctx->def_transport);
    ctx->baller_count = 2;
    CURL_TRC_CF(data, cf, "2nd attempt uses %s from %s",
                ctx->ballers[1].name, source);
  }
  ctx->ballers_complete = TRUE;
}

ساكن CURLcode cf_hc_connect(بنية Curl_cfilter *cf,
                              بنية Curl_easy *data,
                              bool *done)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ثابت بنية curltime *pnow = عدم;

  אם(cf->connected) {
    *done = TRUE;
    החזר CURLE_OK;
  }

  *done = FALSE;

  אם(!ctx->httpsrr_resolved) {
    ctx->httpsrr_resolved = Curl_conn_dns_resolved_https(
      data, cf->sockindex, ctx->destination);
#ifdef DEBUGBUILD
    אם(!ctx->httpsrr_resolved && getenv("CURL_DBG_AWAIT_HTTPSRR")) {
      CURL_TRC_CF(data, cf, "awaiting HTTPS-RR");
      החזר CURLE_OK;
    }
#endif
  }

  بدل(ctx->state) {
  حالة CF_HC_RESOLV:
    ctx->state = CF_HC_INIT;
    FALLTHROUGH();

  حالة CF_HC_INIT:
    DEBUGASSERT(!cf->next);
    CURL_TRC_CF(data, cf, "connect, init");
    result = cf_hc_set_baller1(cf, data);
    אם(result) {
      ctx->result = result;
      ctx->state = CF_HC_FAILURE;
      اذهب out;
    }
    cf_hc_set_baller2(cf, data);
    pnow = Curl_pgrs_now(data);
    ctx->started = *pnow;
    cf_hc_baller_init(&ctx->ballers[0], cf, data);
    אם((ctx->baller_count > 1) || !ctx->ballers_complete) {
      Curl_expire_set(data, EXPIRE_ALPN_EYEBALLS,
                      ctx->soft_eyeballs_timeout_ms, pnow);
    }
    ctx->state = CF_HC_CONNECT;
    FALLTHROUGH();

  حالة CF_HC_CONNECT:
    אם(!ctx->ballers_complete)
      cf_hc_set_baller2(cf, data);

    אם(cf_hc_baller_is_connecting(&ctx->ballers[0])) {
      result = cf_hc_baller_connect(&ctx->ballers[0], cf, data, done);
      אם(!result && *done) {
        result = baller_connected(cf, data, &ctx->ballers[0]);
        اذهب out;
      }
    }

    pnow = Curl_pgrs_now(data);
    אם(time_to_start_baller2(cf, data, pnow)) {
      cf_hc_baller_init(&ctx->ballers[1], cf, data);
    }

    אם(cf_hc_baller_is_connecting(&ctx->ballers[1])) {
      result = cf_hc_baller_connect(&ctx->ballers[1], cf, data, done);
      אם(!result && *done) {
        result = baller_connected(cf, data, &ctx->ballers[1]);
        اذهب out;
      }
    }

    אם(ctx->ballers[0].result &&
       (ctx->ballers[1].result ||
        (ctx->ballers_complete && (ctx->baller_count < 2)))) {
      /* all have failed. we give up */
      CURL_TRC_CF(data, cf, "connect, all attempts failed");
      ctx->result = result = ctx->ballers[0].result;
      ctx->state = CF_HC_FAILURE;
      اذهب out;
    }
    result = CURLE_OK;
    *done = FALSE;
    اكسر;

  حالة CF_HC_FAILURE:
    result = ctx->result;
    cf->connected = FALSE;
    *done = FALSE;
    اكسر;

  حالة CF_HC_SUCCESS:
    result = CURLE_OK;
    cf->connected = TRUE;
    *done = TRUE;
    اكسر;
  }

out:
  CURL_TRC_CF(data, cf, "connect -> %d, done=%d", (صحيح)result, *done);
  החזר result;
}

ساكن CURLcode cf_hc_shutdown(بنية Curl_cfilter *cf,
                               بنية Curl_easy *data, bool *done)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  size_t i;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  אם(cf->connected) {
    *done = TRUE;
    החזר CURLE_OK;
  }

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  עבור(i = 0; i < ctx->baller_count; i++) {
    بنية cf_hc_baller *b = &ctx->ballers[i];
    bool bdone = FALSE;
    אם(!cf_hc_baller_is_connecting(b) || b->shutdown)
      استمر;
    b->result = b->cf->cft->do_shutdown(b->cf, data, &bdone);
    אם(b->result || bdone)
      b->shutdown = TRUE; /* treat a failed shutdown as done */
  }

  *done = TRUE;
  עבור(i = 0; i < ctx->baller_count; i++) {
    אם(!ctx->ballers[i].shutdown)
      *done = FALSE;
  }
  אם(*done) {
    עבור(i = 0; i < ctx->baller_count; i++) {
      אם(ctx->ballers[i].result)
        result = ctx->ballers[i].result;
    }
  }
  CURL_TRC_CF(data, cf, "shutdown -> %d, done=%d", (صحيح)result, *done);
  החזר result;
}

ساكن CURLcode cf_hc_adjust_pollset(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *data,
                                     بنية easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  אם(!cf->connected) {
    بنية cf_hc_ctx *ctx = cf->ctx;
    size_t i;

    עבור(i = 0; (i < ctx->baller_count) && !result; i++) {
      بنية cf_hc_baller *b = &ctx->ballers[i];
      אם(!cf_hc_baller_is_connecting(b))
        استمر;
      result = Curl_conn_cf_adjust_pollset(b->cf, data, ps);
    }
    CURL_TRC_CF(data, cf, "adjust_pollset -> %d, %u socks", (صحيح)result,
                ps->n);
  }
  החזר result;
}

ساكن bool cf_hc_data_pending(بنية Curl_cfilter *cf,
                               ثابت بنية Curl_easy *data)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  אם(cf->connected)
    החזר cf->next->cft->has_data_pending(cf->next, data);

  עבור(i = 0; i < ctx->baller_count; i++)
    אם(cf_hc_baller_data_pending(&ctx->ballers[i], data))
      החזר TRUE;
  החזר FALSE;
}

ساكن CURLcode cf_hc_query(بنية Curl_cfilter *cf,
                            بنية Curl_easy *data,
                            صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  אם(!cf->connected) {
    بدل(query) {
    حالة CF_QUERY_NEED_FLUSH: {
      עבור(i = 0; i < ctx->baller_count; i++)
        אם(cf_hc_baller_needs_flush(&ctx->ballers[i], data)) {
          *pres1 = TRUE;
          החזר CURLE_OK;
        }
      اكسر;
    }
    افتراضي:
      اكسر;
    }
  }
  החזר cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساكن CURLcode cf_hc_cntrl(بنية Curl_cfilter *cf,
                            بنية Curl_easy *data,
                            صحيح event, صحيح arg1, فراغ *arg2)
{
  بنية cf_hc_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t i;

  אם(!cf->connected) {
    بدل(event) {
    حالة CF_CTRL_REPORT_STATS:
    حالة CF_CTRL_CONN_INFO_UPDATE:
      עבור(i = 0; i < ctx->baller_count; i++) {
        /* Make the first baller that connected at network level report */
        אם(Curl_conn_cf_is_ip_connected(ctx->ballers[i].cf, data)) {
          Curl_conn_cf_cntrl(ctx->ballers[i].cf, data, TRUE,
                             event, arg1, arg2);
          اكسر;
        }
      }
      اكسر;
    افتراضي:
      עבור(i = 0; i < ctx->baller_count; i++) {
        result = cf_hc_baller_cntrl(&ctx->ballers[i], data, event, arg1, arg2);
        אם(result && (result != CURLE_AGAIN))
          اذهب out;
      }
      result = CURLE_OK;
      اكسر;
    }
  }
out:
  החזר result;
}

ساكن فراغ cf_hc_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *data)
{
  بنية cf_hc_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  cf_hc_ctx_destroy(data, ctx);
}

بنية Curl_cftype Curl_cft_http_connect = {
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

ساكن CURLcode cf_hc_create(بنية Curl_cfilter **pcf,
                             بنية Curl_easy *data,
                             بنية Curl_peer *destination,
                             uint8_t def_transport)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_hc_ctx *ctx;
  CURLcode result = CURLE_OK;

  ctx = curlx_calloc(1, حجم(*ctx));
  אם(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }
  Curl_peer_link(&ctx->destination, destination);
  ctx->def_transport = def_transport;
  ctx->hard_eyeballs_timeout_ms = data->set.happy_eyeballs_timeout;
  ctx->soft_eyeballs_timeout_ms = data->set.happy_eyeballs_timeout / 2;

  result = Curl_cf_create(&cf, &Curl_cft_http_connect, ctx);
  אם(result)
    اذهب out;
  ctx = عدم;

out:
  *pcf = result ? عدم : cf;
  cf_hc_ctx_destroy(data, ctx);
  החזר result;
}

ساكن CURLcode cf_hc_add(بنية Curl_easy *data,
                          بنية Curl_peer *destination,
                          بنية connectdata *conn,
                          int8_t sockindex,
                          uint8_t def_transport)
{
  بنية Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_hc_create(&cf, data, destination, def_transport);
  אם(result)
    اذهب out;
  Curl_conn_cf_add(data, conn, sockindex, cf);

#ifdef USE_HTTPSRR
  result = Curl_conn_dns_add_https_resolve(data, cf->conn, cf->sockindex,
                                           destination);
#endif
out:
  החזר result;
}

CURLcode Curl_cf_https_setup(بنية Curl_easy *data,
                             بنية Curl_peer *destination,
                             بنية connectdata *conn,
                             int8_t sockindex)
{
  CURLcode result = CURLE_OK;

  DEBUGASSERT(conn->scheme->protocol == CURLPROTO_HTTPS);

  /* This filter is intended for HTTPS using ALPN and does
   * not support HTTPS Eyeballing to a proxy. */
  אם((conn->scheme->protocol != CURLPROTO_HTTPS) ||
#ifndef CURL_DISABLE_PROXY
     conn->bits.origin_is_proxy ||
#endif
     !conn->bits.tls_enable_alpn)
    اذهب out;

  result = cf_hc_add(data, destination, conn, sockindex,
                     conn->transport_wanted);

out:
  החזר result;
}

#endif /* !CURL_DISABLE_HTTP */
