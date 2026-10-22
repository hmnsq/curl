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
#অন্তর্ভাব "curl_setup.h"

#ifndef CURL_DISABLE_HTTP

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "cf-setup.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "cf-https-connect.h"
#অন্তর্ভাব "http2.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "vdns/cf-dns.h"
#অন্তর্ভাব "vdns/hostip.h"
#অন্তর্ভাব "vdns/httpsrr.h"
#অন্তর্ভাব "vquic/vquic.h"

প্রকারলেখ গণনা {
  CF_HC_RESOLV,
  CF_HC_INIT,
  CF_HC_CONNECT,
  CF_HC_SUCCESS,
  CF_HC_FAILURE
} cf_hc_state;

কাঠামো cf_hc_baller {
  ধ্রুবক অক্ষর *name;
  কাঠামো Curl_cfilter *cf;
  CURLcode ফলাফল;
  পূর্ণসংখ্যা reply_ms;
  uint8_t transport;
  গণনা alpnid alpn_id;
  BIT(shutdown);
};

স্থির শূন্য cf_hc_baller_discard(কাঠামো cf_hc_baller *b,
                                 কাঠামো Curl_easy *তথ্য)
{
  যদি(b->cf) {
    Curl_conn_cf_discard_chain(&b->cf, তথ্য);
    b->cf = ফাঁকা;
  }
}

স্থির bool cf_hc_baller_is_connecting(কাঠামো cf_hc_baller *b)
{
  ফেরত b->cf && !b->ফলাফল;
}

স্থির bool cf_hc_baller_has_started(কাঠামো cf_hc_baller *b)
{
  ফেরত !!b->cf;
}

স্থির পূর্ণসংখ্যা cf_hc_baller_reply_ms(কাঠামো cf_hc_baller *b,
                                 কাঠামো Curl_easy *তথ্য)
{
  যদি(b->cf && (b->reply_ms < 0))
    b->cf->cft->query(b->cf, তথ্য, CF_QUERY_CONNECT_REPLY_MS,
                      &b->reply_ms, ফাঁকা);
  ফেরত b->reply_ms;
}

স্থির bool cf_hc_baller_তথ্য_pending(কাঠামো cf_hc_baller *b,
                                      ধ্রুবক কাঠামো Curl_easy *তথ্য)
{
  ফেরত b->cf && !b->ফলাফল && b->cf->cft->has_তথ্য_pending(b->cf, তথ্য);
}

স্থির bool cf_hc_baller_needs_flush(কাঠামো cf_hc_baller *b,
                                     কাঠামো Curl_easy *তথ্য)
{
  ফেরত b->cf && !b->ফলাফল && Curl_conn_cf_needs_flush(b->cf, তথ্য);
}

স্থির CURLcode cf_hc_baller_cntrl(কাঠামো cf_hc_baller *b,
                                   কাঠামো Curl_easy *তথ্য,
                                   পূর্ণসংখ্যা event, পূর্ণসংখ্যা arg1, শূন্য *arg2)
{
  যদি(b->cf && !b->ফলাফল)
    ফেরত Curl_conn_cf_cntrl(b->cf, তথ্য, FALSE, event, arg1, arg2);
  ফেরত CURLE_OK;
}

কাঠামো cf_hc_ctx {
  cf_hc_state state;
  কাঠামো Curl_peer *destination; /* who we ultimately want to talk to */
  কাঠামো curltime started;  /* when connect started */
  CURLcode ফলাফল;          /* overall result */
  CURLcode check_h3_ফলাফল;
  কাঠামো cf_hc_baller ballers[2];
  size_t baller_গুনতি;
  timediff_t soft_eyeballs_timeout_ms;
  timediff_t hard_eyeballs_timeout_ms;
  uint8_t def_transport;
  BIT(httpsrr_resolved);
  BIT(checked_h3);
  BIT(ballers_complete);
};

স্থির শূন্য cf_hc_ctx_destroy(কাঠামো Curl_easy *তথ্য,
                              কাঠামো cf_hc_ctx *ctx)
{
  যদি(ctx) {
    size_t i;
    জন্য(i = 0; i < ctx->baller_গুনতি; ++i)
      cf_hc_baller_discard(&ctx->ballers[i], তথ্য);
    Curl_peer_unlink(&ctx->destination);
    curlx_free(ctx);
  }
}

স্থির শূন্য cf_hc_baller_assign(কাঠামো cf_hc_baller *b,
                                গণনা alpnid alpn_id,
                                uint8_t def_transport)
{
  b->alpn_id = alpn_id;
  b->transport = def_transport;
  b->cf = ফাঁকা;
  b->ফলাফল = CURLE_OK;
  b->reply_ms = -1;
  b->shutdown = FALSE;
  নির্বাচন(b->alpn_id) {
  ক্ষেত্র ALPN_h3:
    b->name = "h3";
    b->transport = TRNSPRT_QUIC;
    বিরতি;
  ক্ষেত্র ALPN_h2:
    b->name = "h2";
    বিরতি;
  ক্ষেত্র ALPN_h1:
    b->name = "h1";
    বিরতি;
  ক্ষেত্র ALPN_none:
    b->name = "no-alpn";
    বিরতি;
  পূর্বনির্ধারিত:
    b->ফলাফল = CURLE_FAILED_INIT;
    বিরতি;
  }
}

স্থির শূন্য cf_hc_baller_init(কাঠামো cf_hc_baller *b,
                              কাঠামো Curl_cfilter *cf,
                              কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_cfilter *save = cf->পরবর্তী;

  cf->পরবর্তী = ফাঁকা;
  b->ফলাফল = Curl_cf_setup_ঢোকাও_after(cf, তথ্য, b->transport,
                                         CURL_CF_SSL_ENABLE);
  b->cf = cf->পরবর্তী;
  cf->পরবর্তী = save;
}

স্থির CURLcode cf_hc_baller_connect(কাঠামো cf_hc_baller *b,
                                     কাঠামো Curl_cfilter *cf,
                                     কাঠামো Curl_easy *তথ্য,
                                     bool *done)
{
  কাঠামো Curl_cfilter *save = cf->পরবর্তী;

  cf->পরবর্তী = b->cf;
  b->ফলাফল = Curl_conn_cf_connect(cf->পরবর্তী, তথ্য, done);
  b->cf = cf->পরবর্তী; /* it might mutate */
  cf->পরবর্তী = save;
  ফেরত b->ফলাফল;
}

স্থির CURLcode baller_connected(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 কাঠামো cf_hc_baller *winner)
{
  কাঠামো cf_hc_ctx *ctx = cf->ctx;

  /* Make the winner's connection filter out own sub-filter, check, move,
   * close all remaining. */
  যদি(cf->পরবর্তী) {
    DEBUGASSERT(0);
    ফেরত CURLE_FAILED_INIT;
  }
  যদি(!winner->cf) {
    DEBUGASSERT(0);
    ফেরত CURLE_FAILED_INIT;
  }

  cf->পরবর্তী = winner->cf;
  winner->cf = ফাঁকা;
  ctx->state = CF_HC_SUCCESS;
  cf->connected = TRUE;

  /* ballers may have failf()'d, the winner resets it, so our
   * errorbuf is clean again. */
  Curl_reset_fail(তথ্য);

#ifdef USE_NGHTTP2
  {
    /* For a negotiated HTTP/2 connection insert the h2 filter. */
    ধ্রুবক অক্ষর *alpn = Curl_conn_cf_get_alpn_negotiated(cf->পরবর্তী, তথ্য);
    যদি(alpn && !strcmp("h2", alpn)) {
      CURLcode ফলাফল = Curl_http2_switch_at(cf, তথ্য);
      যদি(ফলাফল) {
        ctx->state = CF_HC_FAILURE;
        ctx->ফলাফল = ফলাফল;
        ফেরত ফলাফল;
      }
    }
  }
#endif
  ফেরত CURLE_OK;
}

স্থির bool time_to_start_baller2(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য,
                                  ধ্রুবক কাঠামো curltime *pnow)
{
  কাঠামো cf_hc_ctx *ctx = cf->ctx;
  timediff_t elapsed_ms;

  যদি(ctx->baller_গুনতি < 2)
    ফেরত FALSE;
  অন্যথা যদি(cf_hc_baller_has_started(&ctx->ballers[1]))
    ফেরত FALSE;
  অন্যথা যদি(ctx->ballers[0].ফলাফল) {
    CURL_TRC_CF(তথ্য, cf, "%s baller failed, starting %s",
                ctx->ballers[0].name, ctx->ballers[1].name);
    ফেরত TRUE;
  }

  elapsed_ms = curlx_ptimediff_ms(pnow, &ctx->started);
  যদি(elapsed_ms >= ctx->hard_eyeballs_timeout_ms) {
    CURL_TRC_CF(তথ্য, cf, "%s inconclusive after %" FMT_TIMEDIFF_T ", "
                "starting %s", ctx->ballers[0].name,
                ctx->hard_eyeballs_timeout_ms, ctx->ballers[1].name);
    ফেরত TRUE;
  }
  অন্যথা যদি(elapsed_ms >= ctx->soft_eyeballs_timeout_ms) {
    যদি(cf_hc_baller_reply_ms(&ctx->ballers[0], তথ্য) < 0) {
      CURL_TRC_CF(তথ্য, cf, "%s has not seen any data after %"
                  FMT_TIMEDIFF_T "ms, starting %s",
                  ctx->ballers[0].name, ctx->soft_eyeballs_timeout_ms,
                  ctx->ballers[1].name);
      ফেরত TRUE;
    }
    /* set the effective hard timeout again */
    Curl_expire_set(তথ্য, EXPIRE_ALPN_EYEBALLS,
                    ctx->hard_eyeballs_timeout_ms - elapsed_ms, pnow);
  }
  ফেরত FALSE;
}

স্থির bool cf_hc_may_h3(কাঠামো Curl_cfilter *cf,
                         কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_hc_ctx *ctx = cf->ctx;
  যদি(!ctx->checked_h3) {
    ctx->check_h3_ফলাফল =
      Curl_conn_may_http3(তথ্য, cf->conn);
    ctx->checked_h3 = TRUE;
  }
  ফেরত !ctx->check_h3_ফলাফল;
}

স্থির গণনা alpnid cf_hc_get_httpsrr_alpn(কাঠামো Curl_cfilter *cf,
                                          কাঠামো Curl_easy *তথ্য,
                                          গণনা alpnid not_this_one)
{
#ifdef USE_HTTPSRR
  কাঠামো cf_hc_ctx *ctx = cf->ctx;
  /* Is there an HTTPSRR use its ALPNs here.
   * We are here after having selected a connection to a host+port and
   * can no longer change that. Any HTTPSRR advice for other hosts and ports
   * we need to ignore. */
  ধ্রুবক কাঠামো Curl_https_rrinfo *rr;
  size_t i;

  /* Do we have HTTPS-RR information? */
  rr = Curl_conn_dns_get_https(তথ্য, cf->sockindex, ctx->destination);

  CURL_TRC_CF(তথ্য, cf, "HTTPS-RR %savailable", rr ? "" : "not ");
  /* We do not support `rr->no_def_alpn`. */
  যদি(Curl_httpsrr_applicable(তথ্য, rr) && !rr->no_def_alpn) {
    জন্য(i = 0; i < CURL_ARRAYSIZE(rr->alpns); ++i) {
      গণনা alpnid alpn_rr = (গণনা alpnid)rr->alpns[i];
      যদি(alpn_rr == not_this_one) /* do not want this one */
        চালু;
      নির্বাচন(alpn_rr) {
      ক্ষেত্র ALPN_h3:
        যদি((তথ্য->state.http_neg.allowed & CURL_HTTP_V3x) &&
           cf_hc_may_h3(cf, তথ্য)) {
          ফেরত alpn_rr;
        }
        বিরতি;
      ক্ষেত্র ALPN_h2:
        যদি(তথ্য->state.http_neg.allowed & CURL_HTTP_V2x) {
          ফেরত alpn_rr;
        }
        বিরতি;
      ক্ষেত্র ALPN_h1:
        যদি(তথ্য->state.http_neg.allowed & CURL_HTTP_V1x) {
          ফেরত alpn_rr;
        }
        বিরতি;
      পূর্বনির্ধারিত: /* ignore */
        বিরতি;
      }
    }
  }
#অন্যথা
  (শূন্য)cf;
  (শূন্য)তথ্য;
  (শূন্য)not_this_one;
#endif
  ফেরত ALPN_none;
}

স্থির গণনা alpnid cf_hc_get_pref_alpn(কাঠামো Curl_cfilter *cf,
                                       কাঠামো Curl_easy *তথ্য,
                                       গণনা alpnid not_this_one)
{
  যদি((তথ্য->state.http_neg.preferred & তথ্য->state.http_neg.allowed)) {
    নির্বাচন(তথ্য->state.http_neg.preferred) {
    ক্ষেত্র CURL_HTTP_V3x:
      যদি(cf_hc_may_h3(cf, তথ্য) && (ALPN_h3 != not_this_one))
        ফেরত ALPN_h3;
      বিরতি;
    ক্ষেত্র CURL_HTTP_V2x:
      যদি(ALPN_h2 != not_this_one)
        ফেরত ALPN_h2;
      বিরতি;
    ক্ষেত্র CURL_HTTP_V1x:
      /* If we are trying h2 already, h1 is already used as fallback */
      যদি((ALPN_h1 != not_this_one) && (ALPN_h2 != not_this_one))
        ফেরত ALPN_h1;
      বিরতি;
    পূর্বনির্ধারিত:
      বিরতি;
    }
  }
  ফেরত ALPN_none;
}

স্থির গণনা alpnid cf_hc_get_first_alpn(কাঠামো Curl_cfilter *cf,
                                        কাঠামো Curl_easy *তথ্য,
                                        http_majors choices,
                                        গণনা alpnid not_this_one)
{
  /* When told to not try h2, we also do not try h1 and vice versa */
  bool allow_h1_or_h2 = (not_this_one != ALPN_h1) &&
                        (not_this_one != ALPN_h2);
  যদি((ALPN_h3 != not_this_one) && (choices & CURL_HTTP_V3x) &&
     cf_hc_may_h3(cf, তথ্য)) {
    ফেরত ALPN_h3;
  }
  যদি(allow_h1_or_h2 && (choices & CURL_HTTP_V2x)) {
    ফেরত ALPN_h2;
  }
  যদি(allow_h1_or_h2 && (choices & CURL_HTTP_V1x)) {
    ফেরত ALPN_h1;
  }
  ফেরত ALPN_none;
}

স্থির CURLcode cf_hc_set_baller1(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_hc_ctx *ctx = cf->ctx;
  গণনা alpnid alpn1 = ALPN_none;
  VERBOSE(ধ্রুবক অক্ষর *source = "HTTPS-RR");

  DEBUGASSERT(cf->conn->bits.tls_enable_alpn);

  alpn1 = cf_hc_get_httpsrr_alpn(cf, তথ্য, ALPN_none);
  যদি(alpn1 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(source = "preferred version");
    alpn1 = cf_hc_get_pref_alpn(cf, তথ্য, ALPN_none);
  }
  যদি(alpn1 == ALPN_none) {
    VERBOSE(source = "wanted versions");
    alpn1 = cf_hc_get_first_alpn(cf, তথ্য,
                                 তথ্য->state.http_neg.wanted,
                                 ALPN_none);
  }
  যদি(alpn1 == ALPN_none) {
    VERBOSE(source = "allowed versions");
    alpn1 = cf_hc_get_first_alpn(cf, তথ্য,
                                 তথ্য->state.http_neg.allowed,
                                 ALPN_none);
  }

  যদি(alpn1 == ALPN_none) {
    /* None of the wanted/allowed HTTP versions could be chosen */
    যদি(ctx->check_h3_ফলাফল) {
      CURL_TRC_CF(তথ্য, cf, "unable to use HTTP/3");
      ফেরত ctx->check_h3_ফলাফল;
    }
    CURL_TRC_CF(তথ্য, cf, "unable to select HTTP version");
    ফেরত CURLE_FAILED_INIT;
  }

  cf_hc_baller_assign(&ctx->ballers[0], alpn1, ctx->def_transport);
  ctx->baller_গুনতি = 1;
  CURL_TRC_CF(তথ্য, cf, "1st attempt uses %s from %s",
              ctx->ballers[0].name, source);

  নির্বাচন(alpn1) {
  ক্ষেত্র ALPN_h1:
    /* We really want h1, switch off h2 to make it disappear in ALPN */
    তথ্য->state.http_neg.wanted &= (uint8_t)~CURL_HTTP_V2x;
    বিরতি;
  পূর্বনির্ধারিত:
    বিরতি;
  }

  ফেরত CURLE_OK;
}

স্থির শূন্য cf_hc_set_baller2(কাঠামো Curl_cfilter *cf,
                              কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_hc_ctx *ctx = cf->ctx;
  গণনা alpnid alpn2 = ALPN_none, alpn1 = ctx->ballers[0].alpn_id;
  VERBOSE(ধ্রুবক অক্ষর *source = "HTTPS-RR");

  যদি(ctx->ballers_complete)
    ফেরত; /* already done */
  যদি(!ctx->httpsrr_resolved)
    ফেরত; /* HTTPS-RR pending */

  alpn2 = cf_hc_get_httpsrr_alpn(cf, তথ্য, alpn1);
  যদি(alpn2 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(source = "preferred version");
    alpn2 = cf_hc_get_pref_alpn(cf, তথ্য, alpn1);
  }
  যদি(alpn2 == ALPN_none) {
    VERBOSE(source = "wanted versions");
    alpn2 = cf_hc_get_first_alpn(cf, তথ্য,
                                 তথ্য->state.http_neg.wanted,
                                 alpn1);
  }

  যদি(alpn2 != ALPN_none) {
    cf_hc_baller_assign(&ctx->ballers[1], alpn2, ctx->def_transport);
    ctx->baller_গুনতি = 2;
    CURL_TRC_CF(তথ্য, cf, "2nd attempt uses %s from %s",
                ctx->ballers[1].name, source);
  }
  ctx->ballers_complete = TRUE;
}

স্থির CURLcode cf_hc_connect(কাঠামো Curl_cfilter *cf,
                              কাঠামো Curl_easy *তথ্য,
                              bool *done)
{
  কাঠামো cf_hc_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক কাঠামো curltime *pnow = ফাঁকা;

  যদি(cf->connected) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  *done = FALSE;

  যদি(!ctx->httpsrr_resolved) {
    ctx->httpsrr_resolved = Curl_conn_dns_resolved_https(
      তথ্য, cf->sockindex, ctx->destination);
#ifdef DEBUGBUILD
    যদি(!ctx->httpsrr_resolved && getenv("CURL_DBG_AWAIT_HTTPSRR")) {
      CURL_TRC_CF(তথ্য, cf, "awaiting HTTPS-RR");
      ফেরত CURLE_OK;
    }
#endif
  }

  নির্বাচন(ctx->state) {
  ক্ষেত্র CF_HC_RESOLV:
    ctx->state = CF_HC_INIT;
    FALLTHROUGH();

  ক্ষেত্র CF_HC_INIT:
    DEBUGASSERT(!cf->পরবর্তী);
    CURL_TRC_CF(তথ্য, cf, "connect, init");
    ফলাফল = cf_hc_set_baller1(cf, তথ্য);
    যদি(ফলাফল) {
      ctx->ফলাফল = ফলাফল;
      ctx->state = CF_HC_FAILURE;
      যাও out;
    }
    cf_hc_set_baller2(cf, তথ্য);
    pnow = Curl_pgrs_now(তথ্য);
    ctx->started = *pnow;
    cf_hc_baller_init(&ctx->ballers[0], cf, তথ্য);
    যদি((ctx->baller_গুনতি > 1) || !ctx->ballers_complete) {
      Curl_expire_set(তথ্য, EXPIRE_ALPN_EYEBALLS,
                      ctx->soft_eyeballs_timeout_ms, pnow);
    }
    ctx->state = CF_HC_CONNECT;
    FALLTHROUGH();

  ক্ষেত্র CF_HC_CONNECT:
    যদি(!ctx->ballers_complete)
      cf_hc_set_baller2(cf, তথ্য);

    যদি(cf_hc_baller_is_connecting(&ctx->ballers[0])) {
      ফলাফল = cf_hc_baller_connect(&ctx->ballers[0], cf, তথ্য, done);
      যদি(!ফলাফল && *done) {
        ফলাফল = baller_connected(cf, তথ্য, &ctx->ballers[0]);
        যাও out;
      }
    }

    pnow = Curl_pgrs_now(তথ্য);
    যদি(time_to_start_baller2(cf, তথ্য, pnow)) {
      cf_hc_baller_init(&ctx->ballers[1], cf, তথ্য);
    }

    যদি(cf_hc_baller_is_connecting(&ctx->ballers[1])) {
      ফলাফল = cf_hc_baller_connect(&ctx->ballers[1], cf, তথ্য, done);
      যদি(!ফলাফল && *done) {
        ফলাফল = baller_connected(cf, তথ্য, &ctx->ballers[1]);
        যাও out;
      }
    }

    যদি(ctx->ballers[0].ফলাফল &&
       (ctx->ballers[1].ফলাফল ||
        (ctx->ballers_complete && (ctx->baller_গুনতি < 2)))) {
      /* all have failed. we give up */
      CURL_TRC_CF(তথ্য, cf, "connect, all attempts failed");
      ctx->ফলাফল = ফলাফল = ctx->ballers[0].ফলাফল;
      ctx->state = CF_HC_FAILURE;
      যাও out;
    }
    ফলাফল = CURLE_OK;
    *done = FALSE;
    বিরতি;

  ক্ষেত্র CF_HC_FAILURE:
    ফলাফল = ctx->ফলাফল;
    cf->connected = FALSE;
    *done = FALSE;
    বিরতি;

  ক্ষেত্র CF_HC_SUCCESS:
    ফলাফল = CURLE_OK;
    cf->connected = TRUE;
    *done = TRUE;
    বিরতি;
  }

out:
  CURL_TRC_CF(তথ্য, cf, "connect -> %d, done=%d", (পূর্ণসংখ্যা)ফলাফল, *done);
  ফেরত ফলাফল;
}

স্থির CURLcode cf_hc_shutdown(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য, bool *done)
{
  কাঠামো cf_hc_ctx *ctx = cf->ctx;
  size_t i;
  CURLcode ফলাফল = CURLE_OK;

  DEBUGASSERT(তথ্য);
  যদি(cf->connected) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  জন্য(i = 0; i < ctx->baller_গুনতি; i++) {
    কাঠামো cf_hc_baller *b = &ctx->ballers[i];
    bool bdone = FALSE;
    যদি(!cf_hc_baller_is_connecting(b) || b->shutdown)
      চালু;
    b->ফলাফল = b->cf->cft->do_shutdown(b->cf, তথ্য, &bdone);
    যদি(b->ফলাফল || bdone)
      b->shutdown = TRUE; /* treat a failed shutdown as done */
  }

  *done = TRUE;
  জন্য(i = 0; i < ctx->baller_গুনতি; i++) {
    যদি(!ctx->ballers[i].shutdown)
      *done = FALSE;
  }
  যদি(*done) {
    জন্য(i = 0; i < ctx->baller_গুনতি; i++) {
      যদি(ctx->ballers[i].ফলাফল)
        ফলাফল = ctx->ballers[i].ফলাফল;
    }
  }
  CURL_TRC_CF(তথ্য, cf, "shutdown -> %d, done=%d", (পূর্ণসংখ্যা)ফলাফল, *done);
  ফেরত ফলাফল;
}

স্থির CURLcode cf_hc_adjust_pollset(কাঠামো Curl_cfilter *cf,
                                     কাঠামো Curl_easy *তথ্য,
                                     কাঠামো easy_pollset *ps)
{
  CURLcode ফলাফল = CURLE_OK;
  যদি(!cf->connected) {
    কাঠামো cf_hc_ctx *ctx = cf->ctx;
    size_t i;

    জন্য(i = 0; (i < ctx->baller_গুনতি) && !ফলাফল; i++) {
      কাঠামো cf_hc_baller *b = &ctx->ballers[i];
      যদি(!cf_hc_baller_is_connecting(b))
        চালু;
      ফলাফল = Curl_conn_cf_adjust_pollset(b->cf, তথ্য, ps);
    }
    CURL_TRC_CF(তথ্য, cf, "adjust_pollset -> %d, %u socks", (পূর্ণসংখ্যা)ফলাফল,
                ps->n);
  }
  ফেরত ফলাফল;
}

স্থির bool cf_hc_তথ্য_pending(কাঠামো Curl_cfilter *cf,
                               ধ্রুবক কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  যদি(cf->connected)
    ফেরত cf->পরবর্তী->cft->has_তথ্য_pending(cf->পরবর্তী, তথ্য);

  জন্য(i = 0; i < ctx->baller_গুনতি; i++)
    যদি(cf_hc_baller_তথ্য_pending(&ctx->ballers[i], তথ্য))
      ফেরত TRUE;
  ফেরত FALSE;
}

স্থির CURLcode cf_hc_query(কাঠামো Curl_cfilter *cf,
                            কাঠামো Curl_easy *তথ্য,
                            পূর্ণসংখ্যা query, পূর্ণসংখ্যা *pres1, শূন্য *pres2)
{
  কাঠামো cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  যদি(!cf->connected) {
    নির্বাচন(query) {
    ক্ষেত্র CF_QUERY_NEED_FLUSH: {
      জন্য(i = 0; i < ctx->baller_গুনতি; i++)
        যদি(cf_hc_baller_needs_flush(&ctx->ballers[i], তথ্য)) {
          *pres1 = TRUE;
          ফেরত CURLE_OK;
        }
      বিরতি;
    }
    পূর্বনির্ধারিত:
      বিরতি;
    }
  }
  ফেরত cf->পরবর্তী ?
    cf->পরবর্তী->cft->query(cf->পরবর্তী, তথ্য, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

স্থির CURLcode cf_hc_cntrl(কাঠামো Curl_cfilter *cf,
                            কাঠামো Curl_easy *তথ্য,
                            পূর্ণসংখ্যা event, পূর্ণসংখ্যা arg1, শূন্য *arg2)
{
  কাঠামো cf_hc_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;
  size_t i;

  যদি(!cf->connected) {
    নির্বাচন(event) {
    ক্ষেত্র CF_CTRL_REPORT_STATS:
    ক্ষেত্র CF_CTRL_CONN_INFO_UPDATE:
      জন্য(i = 0; i < ctx->baller_গুনতি; i++) {
        /* Make the first baller that connected at network level report */
        যদি(Curl_conn_cf_is_ip_connected(ctx->ballers[i].cf, তথ্য)) {
          Curl_conn_cf_cntrl(ctx->ballers[i].cf, তথ্য, TRUE,
                             event, arg1, arg2);
          বিরতি;
        }
      }
      বিরতি;
    পূর্বনির্ধারিত:
      জন্য(i = 0; i < ctx->baller_গুনতি; i++) {
        ফলাফল = cf_hc_baller_cntrl(&ctx->ballers[i], তথ্য, event, arg1, arg2);
        যদি(ফলাফল && (ফলাফল != CURLE_AGAIN))
          যাও out;
      }
      ফলাফল = CURLE_OK;
      বিরতি;
    }
  }
out:
  ফেরত ফলাফল;
}

স্থির শূন্য cf_hc_destroy(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_hc_ctx *ctx = cf->ctx;

  CURL_TRC_CF(তথ্য, cf, "destroy");
  cf_hc_ctx_destroy(তথ্য, ctx);
}

কাঠামো Curl_cftype Curl_cft_http_connect = {
  "HTTPS-CONNECT",
  CF_TYPE_SETUP,
  CURL_LOG_LVL_NONE,
  cf_hc_destroy,
  cf_hc_connect,
  cf_hc_shutdown,
  cf_hc_adjust_pollset,
  cf_hc_তথ্য_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  cf_hc_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_hc_query,
};

স্থির CURLcode cf_hc_create(কাঠামো Curl_cfilter **pcf,
                             কাঠামো Curl_easy *তথ্য,
                             কাঠামো Curl_peer *destination,
                             uint8_t def_transport)
{
  কাঠামো Curl_cfilter *cf = ফাঁকা;
  কাঠামো cf_hc_ctx *ctx;
  CURLcode ফলাফল = CURLE_OK;

  ctx = curlx_calloc(1, আকার(*ctx));
  যদি(!ctx) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }
  Curl_peer_link(&ctx->destination, destination);
  ctx->def_transport = def_transport;
  ctx->hard_eyeballs_timeout_ms = তথ্য->set.happy_eyeballs_timeout;
  ctx->soft_eyeballs_timeout_ms = তথ্য->set.happy_eyeballs_timeout / 2;

  ফলাফল = Curl_cf_create(&cf, &Curl_cft_http_connect, ctx);
  যদি(ফলাফল)
    যাও out;
  ctx = ফাঁকা;

out:
  *pcf = ফলাফল ? ফাঁকা : cf;
  cf_hc_ctx_destroy(তথ্য, ctx);
  ফেরত ফলাফল;
}

স্থির CURLcode cf_hc_add(কাঠামো Curl_easy *তথ্য,
                          কাঠামো Curl_peer *destination,
                          কাঠামো connectdata *conn,
                          int8_t sockindex,
                          uint8_t def_transport)
{
  কাঠামো Curl_cfilter *cf;
  CURLcode ফলাফল = CURLE_OK;

  DEBUGASSERT(তথ্য);
  ফলাফল = cf_hc_create(&cf, তথ্য, destination, def_transport);
  যদি(ফলাফল)
    যাও out;
  Curl_conn_cf_add(তথ্য, conn, sockindex, cf);

#ifdef USE_HTTPSRR
  ফলাফল = Curl_conn_dns_add_https_resolve(তথ্য, cf->conn, cf->sockindex,
                                           destination);
#endif
out:
  ফেরত ফলাফল;
}

CURLcode Curl_cf_https_setup(কাঠামো Curl_easy *তথ্য,
                             কাঠামো Curl_peer *destination,
                             কাঠামো connectdata *conn,
                             int8_t sockindex)
{
  CURLcode ফলাফল = CURLE_OK;

  DEBUGASSERT(conn->scheme->protocol == CURLPROTO_HTTPS);

  /* This filter is intended for HTTPS using ALPN and does
   * not support HTTPS Eyeballing to a proxy. */
  যদি((conn->scheme->protocol != CURLPROTO_HTTPS) ||
#ifndef CURL_DISABLE_PROXY
     conn->bits.origin_is_proxy ||
#endif
     !conn->bits.tls_enable_alpn)
    যাও out;

  ফলাফল = cf_hc_add(তথ্য, destination, conn, sockindex,
                     conn->transport_wanted);

out:
  ফেরত ফলাফল;
}

#endif /* !CURL_DISABLE_HTTP */
