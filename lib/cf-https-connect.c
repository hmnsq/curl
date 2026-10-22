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
#include "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#include "urldata.h"
#include "curl_trc.h"
#include "cfilters.h"
#include "cf-setup.h"
#include "connect.h"
#include "multiif.h"
#include "cf-https-connect.h"
#include "http2.h"
#include "progress.h"
#include "select.h"
#include "vdns/cf-dns.h"
#include "vdns/hostip.h"
#include "vdns/httpsrr.h"
#include "vquic/vquic.h"

typedef ga_nanaa {
  CF_HC_RESOLV,
  CF_HC_INIT,
  CF_HC_CONNECT,
  CF_HC_SUCCESS,
  CF_HC_FAILURE
} cf_hc_state;

sa_nracanaa cf_hc_baller {
  const var_na *name;
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result;
  ចំនួនគត់ reply_ms;
  uint8_t transport;
  ga_nanaa alpnid alpn_id;
  BIT(shutdown);
};

static void cf_hc_baller_discard(sa_nracanaa cf_hc_baller *b,
                                 sa_nracanaa Curl_easy *data)
{
  ប្រសិនបើ(b->cf) {
    Curl_conn_cf_discard_chain(&b->cf, data);
    b->cf = NULL;
  }
}

static bool cf_hc_baller_is_connecting(sa_nracanaa cf_hc_baller *b)
{
  ត្រឡប់ b->cf && !b->result;
}

static bool cf_hc_baller_has_started(sa_nracanaa cf_hc_baller *b)
{
  ត្រឡប់ !!b->cf;
}

static ចំនួនគត់ cf_hc_baller_reply_ms(sa_nracanaa cf_hc_baller *b,
                                 sa_nracanaa Curl_easy *data)
{
  ប្រសិនបើ(b->cf && (b->reply_ms < 0))
    b->cf->cft->query(b->cf, data, CF_QUERY_CONNECT_REPLY_MS,
                      &b->reply_ms, NULL);
  ត្រឡប់ b->reply_ms;
}

static bool cf_hc_baller_data_pending(sa_nracanaa cf_hc_baller *b,
                                      const sa_nracanaa Curl_easy *data)
{
  ត្រឡប់ b->cf && !b->result && b->cf->cft->has_data_pending(b->cf, data);
}

static bool cf_hc_baller_needs_flush(sa_nracanaa cf_hc_baller *b,
                                     sa_nracanaa Curl_easy *data)
{
  ត្រឡប់ b->cf && !b->result && Curl_conn_cf_needs_flush(b->cf, data);
}

static CURLcode cf_hc_baller_cntrl(sa_nracanaa cf_hc_baller *b,
                                   sa_nracanaa Curl_easy *data,
                                   ចំនួនគត់ event, ចំនួនគត់ arg1, void *arg2)
{
  ប្រសិនបើ(b->cf && !b->result)
    ត្រឡប់ Curl_conn_cf_cntrl(b->cf, data, FALSE, event, arg1, arg2);
  ត្រឡប់ CURLE_OK;
}

sa_nracanaa cf_hc_ctx {
  cf_hc_state state;
  sa_nracanaa Curl_peer *destination; /* who we ultimately want to talk to */
  sa_nracanaa curltime started;  /* when connect started */
  CURLcode result;          /* overall result */
  CURLcode check_h3_result;
  sa_nracanaa cf_hc_baller ballers[2];
  size_t baller_count;
  timediff_t soft_eyeballs_timeout_ms;
  timediff_t hard_eyeballs_timeout_ms;
  uint8_t def_transport;
  BIT(httpsrr_resolved);
  BIT(checked_h3);
  BIT(ballers_complete);
};

static void cf_hc_ctx_destroy(sa_nracanaa Curl_easy *data,
                              sa_nracanaa cf_hc_ctx *ctx)
{
  ប្រសិនបើ(ctx) {
    size_t i;
    សម្រាប់(i = 0; i < ctx->baller_count; ++i)
      cf_hc_baller_discard(&ctx->ballers[i], data);
    Curl_peer_unlink(&ctx->destination);
    curlx_free(ctx);
  }
}

static void cf_hc_baller_assign(sa_nracanaa cf_hc_baller *b,
                                ga_nanaa alpnid alpn_id,
                                uint8_t def_transport)
{
  b->alpn_id = alpn_id;
  b->transport = def_transport;
  b->cf = NULL;
  b->result = CURLE_OK;
  b->reply_ms = -1;
  b->shutdown = FALSE;
  switch(b->alpn_id) {
  pak_sa ALPN_h3:
    b->name = "h3";
    b->transport = TRNSPRT_QUIC;
    bha_nga;
  pak_sa ALPN_h2:
    b->name = "h2";
    bha_nga;
  pak_sa ALPN_h1:
    b->name = "h1";
    bha_nga;
  pak_sa ALPN_none:
    b->name = "no-alpn";
    bha_nga;
  default:
    b->result = CURLE_FAILED_INIT;
    bha_nga;
  }
}

static void cf_hc_baller_init(sa_nracanaa cf_hc_baller *b,
                              sa_nracanaa Curl_cfilter *cf,
                              sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_cfilter *save = cf->next;

  cf->next = NULL;
  b->result = Curl_cf_setup_insert_after(cf, data, b->transport,
                                         CURL_CF_SSL_ENABLE);
  b->cf = cf->next;
  cf->next = save;
}

static CURLcode cf_hc_baller_connect(sa_nracanaa cf_hc_baller *b,
                                     sa_nracanaa Curl_cfilter *cf,
                                     sa_nracanaa Curl_easy *data,
                                     bool *done)
{
  sa_nracanaa Curl_cfilter *save = cf->next;

  cf->next = b->cf;
  b->result = Curl_conn_cf_connect(cf->next, data, done);
  b->cf = cf->next; /* it might mutate */
  cf->next = save;
  ត្រឡប់ b->result;
}

static CURLcode baller_connected(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 sa_nracanaa cf_hc_baller *winner)
{
  sa_nracanaa cf_hc_ctx *ctx = cf->ctx;

  /* Make the winner's connection filter out own sub-filter, check, move,
   * close all remaining. */
  ប្រសិនបើ(cf->next) {
    DEBUGASSERT(0);
    ត្រឡប់ CURLE_FAILED_INIT;
  }
  ប្រសិនបើ(!winner->cf) {
    DEBUGASSERT(0);
    ត្រឡប់ CURLE_FAILED_INIT;
  }

  cf->next = winner->cf;
  winner->cf = NULL;
  ctx->state = CF_HC_SUCCESS;
  cf->connected = TRUE;

  /* ballers may have failf()'d, the winner resets it, so our
   * errorbuf is clean again. */
  Curl_reset_fail(data);

#ifdef USE_NGHTTP2
  {
    /* For a negotiated HTTP/2 connection insert the h2 filter. */
    const var_na *alpn = Curl_conn_cf_get_alpn_negotiated(cf->next, data);
    ប្រសិនបើ(alpn && !strcmp("h2", alpn)) {
      CURLcode result = Curl_http2_switch_at(cf, data);
      ប្រសិនបើ(result) {
        ctx->state = CF_HC_FAILURE;
        ctx->result = result;
        ត្រឡប់ result;
      }
    }
  }
#endif
  ត្រឡប់ CURLE_OK;
}

static bool time_to_start_baller2(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data,
                                  const sa_nracanaa curltime *pnow)
{
  sa_nracanaa cf_hc_ctx *ctx = cf->ctx;
  timediff_t elapsed_ms;

  ប្រសិនបើ(ctx->baller_count < 2)
    ត្រឡប់ FALSE;
  បើមិនដូច្នេះ ប្រសិនបើ(cf_hc_baller_has_started(&ctx->ballers[1]))
    ត្រឡប់ FALSE;
  បើមិនដូច្នេះ ប្រសិនបើ(ctx->ballers[0].result) {
    CURL_TRC_CF(data, cf, "%s baller failed, starting %s",
                ctx->ballers[0].name, ctx->ballers[1].name);
    ត្រឡប់ TRUE;
  }

  elapsed_ms = curlx_ptimediff_ms(pnow, &ctx->started);
  ប្រសិនបើ(elapsed_ms >= ctx->hard_eyeballs_timeout_ms) {
    CURL_TRC_CF(data, cf, "%s inconclusive after %" FMT_TIMEDIFF_T ", "
                "starting %s", ctx->ballers[0].name,
                ctx->hard_eyeballs_timeout_ms, ctx->ballers[1].name);
    ត្រឡប់ TRUE;
  }
  បើមិនដូច្នេះ ប្រសិនបើ(elapsed_ms >= ctx->soft_eyeballs_timeout_ms) {
    ប្រសិនបើ(cf_hc_baller_reply_ms(&ctx->ballers[0], data) < 0) {
      CURL_TRC_CF(data, cf, "%s has not seen any data after %"
                  FMT_TIMEDIFF_T "ms, starting %s",
                  ctx->ballers[0].name, ctx->soft_eyeballs_timeout_ms,
                  ctx->ballers[1].name);
      ត្រឡប់ TRUE;
    }
    /* set the effective hard timeout again */
    Curl_expire_set(data, EXPIRE_ALPN_EYEBALLS,
                    ctx->hard_eyeballs_timeout_ms - elapsed_ms, pnow);
  }
  ត្រឡប់ FALSE;
}

static bool cf_hc_may_h3(sa_nracanaa Curl_cfilter *cf,
                         sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_hc_ctx *ctx = cf->ctx;
  ប្រសិនបើ(!ctx->checked_h3) {
    ctx->check_h3_result =
      Curl_conn_may_http3(data, cf->conn);
    ctx->checked_h3 = TRUE;
  }
  ត្រឡប់ !ctx->check_h3_result;
}

static ga_nanaa alpnid cf_hc_get_httpsrr_alpn(sa_nracanaa Curl_cfilter *cf,
                                          sa_nracanaa Curl_easy *data,
                                          ga_nanaa alpnid not_this_one)
{
#ifdef USE_HTTPSRR
  sa_nracanaa cf_hc_ctx *ctx = cf->ctx;
  /* Is there an HTTPSRR use its ALPNs here.
   * We are here after having selected a connection to a host+port and
   * can no longer change that. Any HTTPSRR advice for other hosts and ports
   * we need to ignore. */
  const sa_nracanaa Curl_https_rrinfo *rr;
  size_t i;

  /* Do we have HTTPS-RR information? */
  rr = Curl_conn_dns_get_https(data, cf->sockindex, ctx->destination);

  CURL_TRC_CF(data, cf, "HTTPS-RR %savailable", rr ? "" : "not ");
  /* We do not support `rr->no_def_alpn`. */
  ប្រសិនបើ(Curl_httpsrr_applicable(data, rr) && !rr->no_def_alpn) {
    សម្រាប់(i = 0; i < CURL_ARRAYSIZE(rr->alpns); ++i) {
      ga_nanaa alpnid alpn_rr = (ga_nanaa alpnid)rr->alpns[i];
      ប្រសិនបើ(alpn_rr == not_this_one) /* do not want this one */
        continue;
      switch(alpn_rr) {
      pak_sa ALPN_h3:
        ប្រសិនបើ((data->state.http_neg.allowed & CURL_HTTP_V3x) &&
           cf_hc_may_h3(cf, data)) {
          ត្រឡប់ alpn_rr;
        }
        bha_nga;
      pak_sa ALPN_h2:
        ប្រសិនបើ(data->state.http_neg.allowed & CURL_HTTP_V2x) {
          ត្រឡប់ alpn_rr;
        }
        bha_nga;
      pak_sa ALPN_h1:
        ប្រសិនបើ(data->state.http_neg.allowed & CURL_HTTP_V1x) {
          ត្រឡប់ alpn_rr;
        }
        bha_nga;
      default: /* ignore */
        bha_nga;
      }
    }
  }
#បើមិនដូច្នេះ
  (void)cf;
  (void)data;
  (void)not_this_one;
#endif
  ត្រឡប់ ALPN_none;
}

static ga_nanaa alpnid cf_hc_get_pref_alpn(sa_nracanaa Curl_cfilter *cf,
                                       sa_nracanaa Curl_easy *data,
                                       ga_nanaa alpnid not_this_one)
{
  ប្រសិនបើ((data->state.http_neg.preferred & data->state.http_neg.allowed)) {
    switch(data->state.http_neg.preferred) {
    pak_sa CURL_HTTP_V3x:
      ប្រសិនបើ(cf_hc_may_h3(cf, data) && (ALPN_h3 != not_this_one))
        ត្រឡប់ ALPN_h3;
      bha_nga;
    pak_sa CURL_HTTP_V2x:
      ប្រសិនបើ(ALPN_h2 != not_this_one)
        ត្រឡប់ ALPN_h2;
      bha_nga;
    pak_sa CURL_HTTP_V1x:
      /* If we are trying h2 already, h1 is already used as fallback */
      ប្រសិនបើ((ALPN_h1 != not_this_one) && (ALPN_h2 != not_this_one))
        ត្រឡប់ ALPN_h1;
      bha_nga;
    default:
      bha_nga;
    }
  }
  ត្រឡប់ ALPN_none;
}

static ga_nanaa alpnid cf_hc_get_first_alpn(sa_nracanaa Curl_cfilter *cf,
                                        sa_nracanaa Curl_easy *data,
                                        http_majors choices,
                                        ga_nanaa alpnid not_this_one)
{
  /* When told to not try h2, we also do not try h1 and vice versa */
  bool allow_h1_or_h2 = (not_this_one != ALPN_h1) &&
                        (not_this_one != ALPN_h2);
  ប្រសិនបើ((ALPN_h3 != not_this_one) && (choices & CURL_HTTP_V3x) &&
     cf_hc_may_h3(cf, data)) {
    ត្រឡប់ ALPN_h3;
  }
  ប្រសិនបើ(allow_h1_or_h2 && (choices & CURL_HTTP_V2x)) {
    ត្រឡប់ ALPN_h2;
  }
  ប្រសិនបើ(allow_h1_or_h2 && (choices & CURL_HTTP_V1x)) {
    ត្រឡប់ ALPN_h1;
  }
  ត្រឡប់ ALPN_none;
}

static CURLcode cf_hc_set_baller1(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_hc_ctx *ctx = cf->ctx;
  ga_nanaa alpnid alpn1 = ALPN_none;
  VERBOSE(const var_na *source = "HTTPS-RR");

  DEBUGASSERT(cf->conn->bits.tls_enable_alpn);

  alpn1 = cf_hc_get_httpsrr_alpn(cf, data, ALPN_none);
  ប្រសិនបើ(alpn1 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(source = "preferred version");
    alpn1 = cf_hc_get_pref_alpn(cf, data, ALPN_none);
  }
  ប្រសិនបើ(alpn1 == ALPN_none) {
    VERBOSE(source = "wanted versions");
    alpn1 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.wanted,
                                 ALPN_none);
  }
  ប្រសិនបើ(alpn1 == ALPN_none) {
    VERBOSE(source = "allowed versions");
    alpn1 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.allowed,
                                 ALPN_none);
  }

  ប្រសិនបើ(alpn1 == ALPN_none) {
    /* None of the wanted/allowed HTTP versions could be chosen */
    ប្រសិនបើ(ctx->check_h3_result) {
      CURL_TRC_CF(data, cf, "unable to use HTTP/3");
      ត្រឡប់ ctx->check_h3_result;
    }
    CURL_TRC_CF(data, cf, "unable to select HTTP version");
    ត្រឡប់ CURLE_FAILED_INIT;
  }

  cf_hc_baller_assign(&ctx->ballers[0], alpn1, ctx->def_transport);
  ctx->baller_count = 1;
  CURL_TRC_CF(data, cf, "1st attempt uses %s from %s",
              ctx->ballers[0].name, source);

  switch(alpn1) {
  pak_sa ALPN_h1:
    /* We really want h1, switch off h2 to make it disappear in ALPN */
    data->state.http_neg.wanted &= (uint8_t)~CURL_HTTP_V2x;
    bha_nga;
  default:
    bha_nga;
  }

  ត្រឡប់ CURLE_OK;
}

static void cf_hc_set_baller2(sa_nracanaa Curl_cfilter *cf,
                              sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_hc_ctx *ctx = cf->ctx;
  ga_nanaa alpnid alpn2 = ALPN_none, alpn1 = ctx->ballers[0].alpn_id;
  VERBOSE(const var_na *source = "HTTPS-RR");

  ប្រសិនបើ(ctx->ballers_complete)
    ត្រឡប់; /* already done */
  ប្រសិនបើ(!ctx->httpsrr_resolved)
    ត្រឡប់; /* HTTPS-RR pending */

  alpn2 = cf_hc_get_httpsrr_alpn(cf, data, alpn1);
  ប្រសិនបើ(alpn2 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(source = "preferred version");
    alpn2 = cf_hc_get_pref_alpn(cf, data, alpn1);
  }
  ប្រសិនបើ(alpn2 == ALPN_none) {
    VERBOSE(source = "wanted versions");
    alpn2 = cf_hc_get_first_alpn(cf, data,
                                 data->state.http_neg.wanted,
                                 alpn1);
  }

  ប្រសិនបើ(alpn2 != ALPN_none) {
    cf_hc_baller_assign(&ctx->ballers[1], alpn2, ctx->def_transport);
    ctx->baller_count = 2;
    CURL_TRC_CF(data, cf, "2nd attempt uses %s from %s",
                ctx->ballers[1].name, source);
  }
  ctx->ballers_complete = TRUE;
}

static CURLcode cf_hc_connect(sa_nracanaa Curl_cfilter *cf,
                              sa_nracanaa Curl_easy *data,
                              bool *done)
{
  sa_nracanaa cf_hc_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  const sa_nracanaa curltime *pnow = NULL;

  ប្រសិនបើ(cf->connected) {
    *done = TRUE;
    ត្រឡប់ CURLE_OK;
  }

  *done = FALSE;

  ប្រសិនបើ(!ctx->httpsrr_resolved) {
    ctx->httpsrr_resolved = Curl_conn_dns_resolved_https(
      data, cf->sockindex, ctx->destination);
#ifdef DEBUGBUILD
    ប្រសិនបើ(!ctx->httpsrr_resolved && getenv("CURL_DBG_AWAIT_HTTPSRR")) {
      CURL_TRC_CF(data, cf, "awaiting HTTPS-RR");
      ត្រឡប់ CURLE_OK;
    }
#endif
  }

  switch(ctx->state) {
  pak_sa CF_HC_RESOLV:
    ctx->state = CF_HC_INIT;
    FALLTHROUGH();

  pak_sa CF_HC_INIT:
    DEBUGASSERT(!cf->next);
    CURL_TRC_CF(data, cf, "connect, init");
    result = cf_hc_set_baller1(cf, data);
    ប្រសិនបើ(result) {
      ctx->result = result;
      ctx->state = CF_HC_FAILURE;
      goto out;
    }
    cf_hc_set_baller2(cf, data);
    pnow = Curl_pgrs_now(data);
    ctx->started = *pnow;
    cf_hc_baller_init(&ctx->ballers[0], cf, data);
    ប្រសិនបើ((ctx->baller_count > 1) || !ctx->ballers_complete) {
      Curl_expire_set(data, EXPIRE_ALPN_EYEBALLS,
                      ctx->soft_eyeballs_timeout_ms, pnow);
    }
    ctx->state = CF_HC_CONNECT;
    FALLTHROUGH();

  pak_sa CF_HC_CONNECT:
    ប្រសិនបើ(!ctx->ballers_complete)
      cf_hc_set_baller2(cf, data);

    ប្រសិនបើ(cf_hc_baller_is_connecting(&ctx->ballers[0])) {
      result = cf_hc_baller_connect(&ctx->ballers[0], cf, data, done);
      ប្រសិនបើ(!result && *done) {
        result = baller_connected(cf, data, &ctx->ballers[0]);
        goto out;
      }
    }

    pnow = Curl_pgrs_now(data);
    ប្រសិនបើ(time_to_start_baller2(cf, data, pnow)) {
      cf_hc_baller_init(&ctx->ballers[1], cf, data);
    }

    ប្រសិនបើ(cf_hc_baller_is_connecting(&ctx->ballers[1])) {
      result = cf_hc_baller_connect(&ctx->ballers[1], cf, data, done);
      ប្រសិនបើ(!result && *done) {
        result = baller_connected(cf, data, &ctx->ballers[1]);
        goto out;
      }
    }

    ប្រសិនបើ(ctx->ballers[0].result &&
       (ctx->ballers[1].result ||
        (ctx->ballers_complete && (ctx->baller_count < 2)))) {
      /* all have failed. we give up */
      CURL_TRC_CF(data, cf, "connect, all attempts failed");
      ctx->result = result = ctx->ballers[0].result;
      ctx->state = CF_HC_FAILURE;
      goto out;
    }
    result = CURLE_OK;
    *done = FALSE;
    bha_nga;

  pak_sa CF_HC_FAILURE:
    result = ctx->result;
    cf->connected = FALSE;
    *done = FALSE;
    bha_nga;

  pak_sa CF_HC_SUCCESS:
    result = CURLE_OK;
    cf->connected = TRUE;
    *done = TRUE;
    bha_nga;
  }

out:
  CURL_TRC_CF(data, cf, "connect -> %d, done=%d", (ចំនួនគត់)result, *done);
  ត្រឡប់ result;
}

static CURLcode cf_hc_shutdown(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa Curl_easy *data, bool *done)
{
  sa_nracanaa cf_hc_ctx *ctx = cf->ctx;
  size_t i;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  ប្រសិនបើ(cf->connected) {
    *done = TRUE;
    ត្រឡប់ CURLE_OK;
  }

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  សម្រាប់(i = 0; i < ctx->baller_count; i++) {
    sa_nracanaa cf_hc_baller *b = &ctx->ballers[i];
    bool bdone = FALSE;
    ប្រសិនបើ(!cf_hc_baller_is_connecting(b) || b->shutdown)
      continue;
    b->result = b->cf->cft->do_shutdown(b->cf, data, &bdone);
    ប្រសិនបើ(b->result || bdone)
      b->shutdown = TRUE; /* treat a failed shutdown as done */
  }

  *done = TRUE;
  សម្រាប់(i = 0; i < ctx->baller_count; i++) {
    ប្រសិនបើ(!ctx->ballers[i].shutdown)
      *done = FALSE;
  }
  ប្រសិនបើ(*done) {
    សម្រាប់(i = 0; i < ctx->baller_count; i++) {
      ប្រសិនបើ(ctx->ballers[i].result)
        result = ctx->ballers[i].result;
    }
  }
  CURL_TRC_CF(data, cf, "shutdown -> %d, done=%d", (ចំនួនគត់)result, *done);
  ត្រឡប់ result;
}

static CURLcode cf_hc_adjust_pollset(sa_nracanaa Curl_cfilter *cf,
                                     sa_nracanaa Curl_easy *data,
                                     sa_nracanaa easy_pollset *ps)
{
  CURLcode result = CURLE_OK;
  ប្រសិនបើ(!cf->connected) {
    sa_nracanaa cf_hc_ctx *ctx = cf->ctx;
    size_t i;

    សម្រាប់(i = 0; (i < ctx->baller_count) && !result; i++) {
      sa_nracanaa cf_hc_baller *b = &ctx->ballers[i];
      ប្រសិនបើ(!cf_hc_baller_is_connecting(b))
        continue;
      result = Curl_conn_cf_adjust_pollset(b->cf, data, ps);
    }
    CURL_TRC_CF(data, cf, "adjust_pollset -> %d, %u socks", (ចំនួនគត់)result,
                ps->n);
  }
  ត្រឡប់ result;
}

static bool cf_hc_data_pending(sa_nracanaa Curl_cfilter *cf,
                               const sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  ប្រសិនបើ(cf->connected)
    ត្រឡប់ cf->next->cft->has_data_pending(cf->next, data);

  សម្រាប់(i = 0; i < ctx->baller_count; i++)
    ប្រសិនបើ(cf_hc_baller_data_pending(&ctx->ballers[i], data))
      ត្រឡប់ TRUE;
  ត្រឡប់ FALSE;
}

static CURLcode cf_hc_query(sa_nracanaa Curl_cfilter *cf,
                            sa_nracanaa Curl_easy *data,
                            ចំនួនគត់ query, ចំនួនគត់ *pres1, void *pres2)
{
  sa_nracanaa cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  ប្រសិនបើ(!cf->connected) {
    switch(query) {
    pak_sa CF_QUERY_NEED_FLUSH: {
      សម្រាប់(i = 0; i < ctx->baller_count; i++)
        ប្រសិនបើ(cf_hc_baller_needs_flush(&ctx->ballers[i], data)) {
          *pres1 = TRUE;
          ត្រឡប់ CURLE_OK;
        }
      bha_nga;
    }
    default:
      bha_nga;
    }
  }
  ត្រឡប់ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

static CURLcode cf_hc_cntrl(sa_nracanaa Curl_cfilter *cf,
                            sa_nracanaa Curl_easy *data,
                            ចំនួនគត់ event, ចំនួនគត់ arg1, void *arg2)
{
  sa_nracanaa cf_hc_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t i;

  ប្រសិនបើ(!cf->connected) {
    switch(event) {
    pak_sa CF_CTRL_REPORT_STATS:
    pak_sa CF_CTRL_CONN_INFO_UPDATE:
      សម្រាប់(i = 0; i < ctx->baller_count; i++) {
        /* Make the first baller that connected at network level report */
        ប្រសិនបើ(Curl_conn_cf_is_ip_connected(ctx->ballers[i].cf, data)) {
          Curl_conn_cf_cntrl(ctx->ballers[i].cf, data, TRUE,
                             event, arg1, arg2);
          bha_nga;
        }
      }
      bha_nga;
    default:
      សម្រាប់(i = 0; i < ctx->baller_count; i++) {
        result = cf_hc_baller_cntrl(&ctx->ballers[i], data, event, arg1, arg2);
        ប្រសិនបើ(result && (result != CURLE_AGAIN))
          goto out;
      }
      result = CURLE_OK;
      bha_nga;
    }
  }
out:
  ត្រឡប់ result;
}

static void cf_hc_destroy(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_hc_ctx *ctx = cf->ctx;

  CURL_TRC_CF(data, cf, "destroy");
  cf_hc_ctx_destroy(data, ctx);
}

sa_nracanaa Curl_cftype Curl_cft_http_connect = {
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

static CURLcode cf_hc_create(sa_nracanaa Curl_cfilter **pcf,
                             sa_nracanaa Curl_easy *data,
                             sa_nracanaa Curl_peer *destination,
                             uint8_t def_transport)
{
  sa_nracanaa Curl_cfilter *cf = NULL;
  sa_nracanaa cf_hc_ctx *ctx;
  CURLcode result = CURLE_OK;

  ctx = curlx_calloc(1, parimaa_na(*ctx));
  ប្រសិនបើ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  Curl_peer_link(&ctx->destination, destination);
  ctx->def_transport = def_transport;
  ctx->hard_eyeballs_timeout_ms = data->set.happy_eyeballs_timeout;
  ctx->soft_eyeballs_timeout_ms = data->set.happy_eyeballs_timeout / 2;

  result = Curl_cf_create(&cf, &Curl_cft_http_connect, ctx);
  ប្រសិនបើ(result)
    goto out;
  ctx = NULL;

out:
  *pcf = result ? NULL : cf;
  cf_hc_ctx_destroy(data, ctx);
  ត្រឡប់ result;
}

static CURLcode cf_hc_add(sa_nracanaa Curl_easy *data,
                          sa_nracanaa Curl_peer *destination,
                          sa_nracanaa connectdata *conn,
                          int8_t sockindex,
                          uint8_t def_transport)
{
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(data);
  result = cf_hc_create(&cf, data, destination, def_transport);
  ប្រសិនបើ(result)
    goto out;
  Curl_conn_cf_add(data, conn, sockindex, cf);

#ifdef USE_HTTPSRR
  result = Curl_conn_dns_add_https_resolve(data, cf->conn, cf->sockindex,
                                           destination);
#endif
out:
  ត្រឡប់ result;
}

CURLcode Curl_cf_https_setup(sa_nracanaa Curl_easy *data,
                             sa_nracanaa Curl_peer *destination,
                             sa_nracanaa connectdata *conn,
                             int8_t sockindex)
{
  CURLcode result = CURLE_OK;

  DEBUGASSERT(conn->scheme->protocol == CURLPROTO_HTTPS);

  /* This filter is intended for HTTPS using ALPN and does
   * not support HTTPS Eyeballing to a proxy. */
  ប្រសិនបើ((conn->scheme->protocol != CURLPROTO_HTTPS) ||
#ifndef CURL_DISABLE_PROXY
     conn->bits.origin_is_proxy ||
#endif
     !conn->bits.tls_enable_alpn)
    goto out;

  result = cf_hc_add(data, destination, conn, sockindex,
                     conn->transport_wanted);

out:
  ត្រឡប់ result;
}

#endif /* !CURL_DISABLE_HTTP */
