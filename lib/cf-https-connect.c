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
#अन्तर्भाव "curl_setup.h"

#यदि_अपरिभाषितम् CURL_DISABLE_HTTP

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "cf-setup.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "cf-https-connect.h"
#अन्तर्भाव "http2.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "vdns/cf-dns.h"
#अन्तर्भाव "vdns/hostip.h"
#अन्तर्भाव "vdns/httpsrr.h"
#अन्तर्भाव "vquic/vquic.h"

प्रकारनिर्देश गणना {
  CF_HC_RESOLV,
  CF_HC_INIT,
  CF_HC_CONNECT,
  CF_HC_SUCCESS,
  CF_HC_FAILURE
} cf_hc_state;

संरचना cf_hc_baller {
  ध्रुव वर्ण *name;
  संरचना Curl_cfilter *cf;
  CURLcode परिणाम;
  पूर्णाङ्क प्रत्युत्तरम्_ms;
  uint8_t transport;
  गणना alpnid alpn_id;
  BIT(shutdown);
};

स्थिर शून्य cf_hc_baller_discard(संरचना cf_hc_baller *b,
                                 संरचना Curl_easy *दत्त)
{
  यदि(b->cf) {
    Curl_conn_cf_discard_chain(&b->cf, दत्त);
    b->cf = रिक्त;
  }
}

स्थिर bool cf_hc_baller_is_connecting(संरचना cf_hc_baller *b)
{
  प्रत्यागम b->cf && !b->परिणाम;
}

स्थिर bool cf_hc_baller_has_started(संरचना cf_hc_baller *b)
{
  प्रत्यागम !!b->cf;
}

स्थिर पूर्णाङ्क cf_hc_baller_प्रत्युत्तरम्_ms(संरचना cf_hc_baller *b,
                                 संरचना Curl_easy *दत्त)
{
  यदि(b->cf && (b->प्रत्युत्तरम्_ms < 0))
    b->cf->cft->query(b->cf, दत्त, CF_QUERY_CONNECT_प्रत्युत्तरम्_MS,
                      &b->प्रत्युत्तरम्_ms, रिक्त);
  प्रत्यागम b->प्रत्युत्तरम्_ms;
}

स्थिर bool cf_hc_baller_दत्त_pending(संरचना cf_hc_baller *b,
                                      ध्रुव संरचना Curl_easy *दत्त)
{
  प्रत्यागम b->cf && !b->परिणाम && b->cf->cft->has_दत्त_pending(b->cf, दत्त);
}

स्थिर bool cf_hc_baller_needs_flush(संरचना cf_hc_baller *b,
                                     संरचना Curl_easy *दत्त)
{
  प्रत्यागम b->cf && !b->परिणाम && Curl_conn_cf_needs_flush(b->cf, दत्त);
}

स्थिर CURLcode cf_hc_baller_cntrl(संरचना cf_hc_baller *b,
                                   संरचना Curl_easy *दत्त,
                                   पूर्णाङ्क event, पूर्णाङ्क arg1, शून्य *arg2)
{
  यदि(b->cf && !b->परिणाम)
    प्रत्यागम Curl_conn_cf_cntrl(b->cf, दत्त, FALSE, event, arg1, arg2);
  प्रत्यागम CURLE_OK;
}

संरचना cf_hc_ctx {
  cf_hc_state state;
  संरचना Curl_peer *destination; /* who we ultimately want to talk to */
  संरचना curltime started;  /* when connect started */
  CURLcode परिणाम;          /* overall result */
  CURLcode check_h3_परिणाम;
  संरचना cf_hc_baller ballers[2];
  size_t baller_गणनम्;
  timediff_t soft_eyeballs_timeout_ms;
  timediff_t hard_eyeballs_timeout_ms;
  uint8_t def_transport;
  BIT(httpsrr_resolved);
  BIT(checked_h3);
  BIT(ballers_complete);
};

स्थिर शून्य cf_hc_ctx_destroy(संरचना Curl_easy *दत्त,
                              संरचना cf_hc_ctx *ctx)
{
  यदि(ctx) {
    size_t i;
    प्रति(i = 0; i < ctx->baller_गणनम्; ++i)
      cf_hc_baller_discard(&ctx->ballers[i], दत्त);
    Curl_peer_unlink(&ctx->destination);
    curlx_free(ctx);
  }
}

स्थिर शून्य cf_hc_baller_नियोजय(संरचना cf_hc_baller *b,
                                गणना alpnid alpn_id,
                                uint8_t def_transport)
{
  b->alpn_id = alpn_id;
  b->transport = def_transport;
  b->cf = रिक्त;
  b->परिणाम = CURLE_OK;
  b->प्रत्युत्तरम्_ms = -1;
  b->shutdown = FALSE;
  विकल्प(b->alpn_id) {
  पक्ष ALPN_h3:
    b->name = "h3";
    b->transport = TRNSPRT_QUIC;
    भङ्ग;
  पक्ष ALPN_h2:
    b->name = "h2";
    भङ्ग;
  पक्ष ALPN_h1:
    b->name = "h1";
    भङ्ग;
  पक्ष ALPN_none:
    b->name = "no-alpn";
    भङ्ग;
  स्वभाव:
    b->परिणाम = CURLE_FAILED_INIT;
    भङ्ग;
  }
}

स्थिर शून्य cf_hc_baller_init(संरचना cf_hc_baller *b,
                              संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *दत्त)
{
  संरचना Curl_cfilter *save = cf->अग्र;

  cf->अग्र = रिक्त;
  b->परिणाम = Curl_cf_सज्जता_निवेशय_after(cf, दत्त, b->transport,
                                         CURL_CF_SSL_ENABLE);
  b->cf = cf->अग्र;
  cf->अग्र = save;
}

स्थिर CURLcode cf_hc_baller_connect(संरचना cf_hc_baller *b,
                                     संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *दत्त,
                                     bool *समाप्तम्)
{
  संरचना Curl_cfilter *save = cf->अग्र;

  cf->अग्र = b->cf;
  b->परिणाम = Curl_conn_cf_connect(cf->अग्र, दत्त, समाप्तम्);
  b->cf = cf->अग्र; /* it might mutate */
  cf->अग्र = save;
  प्रत्यागम b->परिणाम;
}

स्थिर CURLcode baller_connected(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 संरचना cf_hc_baller *winner)
{
  संरचना cf_hc_ctx *ctx = cf->ctx;

  /* Make the winner's connection filter out own sub-filter, check, move,
   * close all remaining. */
  यदि(cf->अग्र) {
    DEBUGASSERT(0);
    प्रत्यागम CURLE_FAILED_INIT;
  }
  यदि(!winner->cf) {
    DEBUGASSERT(0);
    प्रत्यागम CURLE_FAILED_INIT;
  }

  cf->अग्र = winner->cf;
  winner->cf = रिक्त;
  ctx->state = CF_HC_SUCCESS;
  cf->connected = TRUE;

  /* ballers may have failf()'d, the winner resets it, so our
   * errorbuf is clean again. */
  Curl_reset_fail(दत्त);

#यदि_परिभाषितम् USE_NGHTTP2
  {
    /* For a negotiated HTTP/2 connection insert the h2 filter. */
    ध्रुव वर्ण *alpn = Curl_conn_cf_get_alpn_negotiated(cf->अग्र, दत्त);
    यदि(alpn && !वाक्यं_तुलय("h2", alpn)) {
      CURLcode परिणाम = Curl_http2_switch_at(cf, दत्त);
      यदि(परिणाम) {
        ctx->state = CF_HC_FAILURE;
        ctx->परिणाम = परिणाम;
        प्रत्यागम परिणाम;
      }
    }
  }
#यदि_अन्तः
  प्रत्यागम CURLE_OK;
}

स्थिर bool time_गन्तव्यम्_start_baller2(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त,
                                  ध्रुव संरचना curltime *pnow)
{
  संरचना cf_hc_ctx *ctx = cf->ctx;
  timediff_t elapsed_ms;

  यदि(ctx->baller_गणनम् < 2)
    प्रत्यागम FALSE;
  अन्यथा यदि(cf_hc_baller_has_started(&ctx->ballers[1]))
    प्रत्यागम FALSE;
  अन्यथा यदि(ctx->ballers[0].परिणाम) {
    CURL_TRC_CF(दत्त, cf, "%s baller failed, starting %s",
                ctx->ballers[0].name, ctx->ballers[1].name);
    प्रत्यागम TRUE;
  }

  elapsed_ms = curlx_ptimediff_ms(pnow, &ctx->started);
  यदि(elapsed_ms >= ctx->hard_eyeballs_timeout_ms) {
    CURL_TRC_CF(दत्त, cf, "%s inconclusive after %" FMT_TIMEDIFF_T ", "
                "starting %s", ctx->ballers[0].name,
                ctx->hard_eyeballs_timeout_ms, ctx->ballers[1].name);
    प्रत्यागम TRUE;
  }
  अन्यथा यदि(elapsed_ms >= ctx->soft_eyeballs_timeout_ms) {
    यदि(cf_hc_baller_प्रत्युत्तरम्_ms(&ctx->ballers[0], दत्त) < 0) {
      CURL_TRC_CF(दत्त, cf, "%s has not seen any data after %"
                  FMT_TIMEDIFF_T "ms, starting %s",
                  ctx->ballers[0].name, ctx->soft_eyeballs_timeout_ms,
                  ctx->ballers[1].name);
      प्रत्यागम TRUE;
    }
    /* set the effective hard timeout again */
    Curl_expire_समुच्चयः(दत्त, EXPIRE_ALPN_EYEBALLS,
                    ctx->hard_eyeballs_timeout_ms - elapsed_ms, pnow);
  }
  प्रत्यागम FALSE;
}

स्थिर bool cf_hc_may_h3(संरचना Curl_cfilter *cf,
                         संरचना Curl_easy *दत्त)
{
  संरचना cf_hc_ctx *ctx = cf->ctx;
  यदि(!ctx->checked_h3) {
    ctx->check_h3_परिणाम =
      Curl_conn_may_http3(दत्त, cf->conn);
    ctx->checked_h3 = TRUE;
  }
  प्रत्यागम !ctx->check_h3_परिणाम;
}

स्थिर गणना alpnid cf_hc_get_httpsrr_alpn(संरचना Curl_cfilter *cf,
                                          संरचना Curl_easy *दत्त,
                                          गणना alpnid not_this_one)
{
#यदि_परिभाषितम् USE_HTTPSRR
  संरचना cf_hc_ctx *ctx = cf->ctx;
  /* Is there an HTTPSRR use its ALPNs here.
   * We are here after having selected a connection to a host+port and
   * can no longer change that. Any HTTPSRR advice for other hosts and ports
   * we need to ignore. */
  ध्रुव संरचना Curl_https_rrinfo *rr;
  size_t i;

  /* Do we have HTTPS-RR information? */
  rr = Curl_conn_dns_get_https(दत्त, cf->sockindex, ctx->destination);

  CURL_TRC_CF(दत्त, cf, "HTTPS-RR %savailable", rr ? "" : "not ");
  /* We do not support `rr->no_def_alpn`. */
  यदि(Curl_httpsrr_applicable(दत्त, rr) && !rr->no_def_alpn) {
    प्रति(i = 0; i < CURL_ARRAYSIZE(rr->alpns); ++i) {
      गणना alpnid alpn_rr = (गणना alpnid)rr->alpns[i];
      यदि(alpn_rr == not_this_one) /* do not want this one */
        अनुवर्त;
      विकल्प(alpn_rr) {
      पक्ष ALPN_h3:
        यदि((दत्त->state.http_neg.allowed & CURL_HTTP_V3x) &&
           cf_hc_may_h3(cf, दत्त)) {
          प्रत्यागम alpn_rr;
        }
        भङ्ग;
      पक्ष ALPN_h2:
        यदि(दत्त->state.http_neg.allowed & CURL_HTTP_V2x) {
          प्रत्यागम alpn_rr;
        }
        भङ्ग;
      पक्ष ALPN_h1:
        यदि(दत्त->state.http_neg.allowed & CURL_HTTP_V1x) {
          प्रत्यागम alpn_rr;
        }
        भङ्ग;
      स्वभाव: /* ignore */
        भङ्ग;
      }
    }
  }
#अन्यथा
  (शून्य)cf;
  (शून्य)दत्त;
  (शून्य)not_this_one;
#यदि_अन्तः
  प्रत्यागम ALPN_none;
}

स्थिर गणना alpnid cf_hc_get_pref_alpn(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *दत्त,
                                       गणना alpnid not_this_one)
{
  यदि((दत्त->state.http_neg.preferred & दत्त->state.http_neg.allowed)) {
    विकल्प(दत्त->state.http_neg.preferred) {
    पक्ष CURL_HTTP_V3x:
      यदि(cf_hc_may_h3(cf, दत्त) && (ALPN_h3 != not_this_one))
        प्रत्यागम ALPN_h3;
      भङ्ग;
    पक्ष CURL_HTTP_V2x:
      यदि(ALPN_h2 != not_this_one)
        प्रत्यागम ALPN_h2;
      भङ्ग;
    पक्ष CURL_HTTP_V1x:
      /* If we are trying h2 already, h1 is already used as fallback */
      यदि((ALPN_h1 != not_this_one) && (ALPN_h2 != not_this_one))
        प्रत्यागम ALPN_h1;
      भङ्ग;
    स्वभाव:
      भङ्ग;
    }
  }
  प्रत्यागम ALPN_none;
}

स्थिर गणना alpnid cf_hc_get_first_alpn(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *दत्त,
                                        http_majors choices,
                                        गणना alpnid not_this_one)
{
  /* When told to not try h2, we also do not try h1 and vice versa */
  bool allow_h1_or_h2 = (not_this_one != ALPN_h1) &&
                        (not_this_one != ALPN_h2);
  यदि((ALPN_h3 != not_this_one) && (choices & CURL_HTTP_V3x) &&
     cf_hc_may_h3(cf, दत्त)) {
    प्रत्यागम ALPN_h3;
  }
  यदि(allow_h1_or_h2 && (choices & CURL_HTTP_V2x)) {
    प्रत्यागम ALPN_h2;
  }
  यदि(allow_h1_or_h2 && (choices & CURL_HTTP_V1x)) {
    प्रत्यागम ALPN_h1;
  }
  प्रत्यागम ALPN_none;
}

स्थिर CURLcode cf_hc_समुच्चयः_baller1(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त)
{
  संरचना cf_hc_ctx *ctx = cf->ctx;
  गणना alpnid alpn1 = ALPN_none;
  VERBOSE(ध्रुव वर्ण *source = "HTTPS-RR");

  DEBUGASSERT(cf->conn->द्वयङ्काः.tls_enable_alpn);

  alpn1 = cf_hc_get_httpsrr_alpn(cf, दत्त, ALPN_none);
  यदि(alpn1 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(source = "preferred version");
    alpn1 = cf_hc_get_pref_alpn(cf, दत्त, ALPN_none);
  }
  यदि(alpn1 == ALPN_none) {
    VERBOSE(source = "wanted versions");
    alpn1 = cf_hc_get_first_alpn(cf, दत्त,
                                 दत्त->state.http_neg.wanted,
                                 ALPN_none);
  }
  यदि(alpn1 == ALPN_none) {
    VERBOSE(source = "allowed versions");
    alpn1 = cf_hc_get_first_alpn(cf, दत्त,
                                 दत्त->state.http_neg.allowed,
                                 ALPN_none);
  }

  यदि(alpn1 == ALPN_none) {
    /* None of the wanted/allowed HTTP versions could be chosen */
    यदि(ctx->check_h3_परिणाम) {
      CURL_TRC_CF(दत्त, cf, "unable to use HTTP/3");
      प्रत्यागम ctx->check_h3_परिणाम;
    }
    CURL_TRC_CF(दत्त, cf, "unable to select HTTP version");
    प्रत्यागम CURLE_FAILED_INIT;
  }

  cf_hc_baller_नियोजय(&ctx->ballers[0], alpn1, ctx->def_transport);
  ctx->baller_गणनम् = 1;
  CURL_TRC_CF(दत्त, cf, "1st attempt uses %s from %s",
              ctx->ballers[0].name, source);

  विकल्प(alpn1) {
  पक्ष ALPN_h1:
    /* We really want h1, switch off h2 to make it disappear in ALPN */
    दत्त->state.http_neg.wanted &= (uint8_t)~CURL_HTTP_V2x;
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }

  प्रत्यागम CURLE_OK;
}

स्थिर शून्य cf_hc_समुच्चयः_baller2(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *दत्त)
{
  संरचना cf_hc_ctx *ctx = cf->ctx;
  गणना alpnid alpn2 = ALPN_none, alpn1 = ctx->ballers[0].alpn_id;
  VERBOSE(ध्रुव वर्ण *source = "HTTPS-RR");

  यदि(ctx->ballers_complete)
    प्रत्यागम; /* already done */
  यदि(!ctx->httpsrr_resolved)
    प्रत्यागम; /* HTTPS-RR pending */

  alpn2 = cf_hc_get_httpsrr_alpn(cf, दत्त, alpn1);
  यदि(alpn2 == ALPN_none) {
    /* preference is configured and allowed, can we use it? */
    VERBOSE(source = "preferred version");
    alpn2 = cf_hc_get_pref_alpn(cf, दत्त, alpn1);
  }
  यदि(alpn2 == ALPN_none) {
    VERBOSE(source = "wanted versions");
    alpn2 = cf_hc_get_first_alpn(cf, दत्त,
                                 दत्त->state.http_neg.wanted,
                                 alpn1);
  }

  यदि(alpn2 != ALPN_none) {
    cf_hc_baller_नियोजय(&ctx->ballers[1], alpn2, ctx->def_transport);
    ctx->baller_गणनम् = 2;
    CURL_TRC_CF(दत्त, cf, "2nd attempt uses %s from %s",
                ctx->ballers[1].name, source);
  }
  ctx->ballers_complete = TRUE;
}

स्थिर CURLcode cf_hc_connect(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *दत्त,
                              bool *समाप्तम्)
{
  संरचना cf_hc_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;
  ध्रुव संरचना curltime *pnow = रिक्त;

  यदि(cf->connected) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  *समाप्तम् = FALSE;

  यदि(!ctx->httpsrr_resolved) {
    ctx->httpsrr_resolved = Curl_conn_dns_resolved_https(
      दत्त, cf->sockindex, ctx->destination);
#यदि_परिभाषितम् DEBUGBUILD
    यदि(!ctx->httpsrr_resolved && getenv("CURL_DBG_AWAIT_HTTPSRR")) {
      CURL_TRC_CF(दत्त, cf, "awaiting HTTPS-RR");
      प्रत्यागम CURLE_OK;
    }
#यदि_अन्तः
  }

  विकल्प(ctx->state) {
  पक्ष CF_HC_RESOLV:
    ctx->state = CF_HC_INIT;
    FALLTHROUGH();

  पक्ष CF_HC_INIT:
    DEBUGASSERT(!cf->अग्र);
    CURL_TRC_CF(दत्त, cf, "connect, init");
    परिणाम = cf_hc_समुच्चयः_baller1(cf, दत्त);
    यदि(परिणाम) {
      ctx->परिणाम = परिणाम;
      ctx->state = CF_HC_FAILURE;
      गच्छ बहिर्गमनम्;
    }
    cf_hc_समुच्चयः_baller2(cf, दत्त);
    pnow = Curl_pgrs_now(दत्त);
    ctx->started = *pnow;
    cf_hc_baller_init(&ctx->ballers[0], cf, दत्त);
    यदि((ctx->baller_गणनम् > 1) || !ctx->ballers_complete) {
      Curl_expire_समुच्चयः(दत्त, EXPIRE_ALPN_EYEBALLS,
                      ctx->soft_eyeballs_timeout_ms, pnow);
    }
    ctx->state = CF_HC_CONNECT;
    FALLTHROUGH();

  पक्ष CF_HC_CONNECT:
    यदि(!ctx->ballers_complete)
      cf_hc_समुच्चयः_baller2(cf, दत्त);

    यदि(cf_hc_baller_is_connecting(&ctx->ballers[0])) {
      परिणाम = cf_hc_baller_connect(&ctx->ballers[0], cf, दत्त, समाप्तम्);
      यदि(!परिणाम && *समाप्तम्) {
        परिणाम = baller_connected(cf, दत्त, &ctx->ballers[0]);
        गच्छ बहिर्गमनम्;
      }
    }

    pnow = Curl_pgrs_now(दत्त);
    यदि(time_गन्तव्यम्_start_baller2(cf, दत्त, pnow)) {
      cf_hc_baller_init(&ctx->ballers[1], cf, दत्त);
    }

    यदि(cf_hc_baller_is_connecting(&ctx->ballers[1])) {
      परिणाम = cf_hc_baller_connect(&ctx->ballers[1], cf, दत्त, समाप्तम्);
      यदि(!परिणाम && *समाप्तम्) {
        परिणाम = baller_connected(cf, दत्त, &ctx->ballers[1]);
        गच्छ बहिर्गमनम्;
      }
    }

    यदि(ctx->ballers[0].परिणाम &&
       (ctx->ballers[1].परिणाम ||
        (ctx->ballers_complete && (ctx->baller_गणनम् < 2)))) {
      /* all have failed. we give up */
      CURL_TRC_CF(दत्त, cf, "connect, all attempts failed");
      ctx->परिणाम = परिणाम = ctx->ballers[0].परिणाम;
      ctx->state = CF_HC_FAILURE;
      गच्छ बहिर्गमनम्;
    }
    परिणाम = CURLE_OK;
    *समाप्तम् = FALSE;
    भङ्ग;

  पक्ष CF_HC_FAILURE:
    परिणाम = ctx->परिणाम;
    cf->connected = FALSE;
    *समाप्तम् = FALSE;
    भङ्ग;

  पक्ष CF_HC_SUCCESS:
    परिणाम = CURLE_OK;
    cf->connected = TRUE;
    *समाप्तम् = TRUE;
    भङ्ग;
  }

बहिर्गमनम्:
  CURL_TRC_CF(दत्त, cf, "connect -> %d, done=%d", (पूर्णाङ्क)परिणाम, *समाप्तम्);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_hc_shutdown(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  संरचना cf_hc_ctx *ctx = cf->ctx;
  size_t i;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(दत्त);
  यदि(cf->connected) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  /* shutdown all ballers that have not done so already. If one fails,
   * continue shutting down others until all are shutdown. */
  प्रति(i = 0; i < ctx->baller_गणनम्; i++) {
    संरचना cf_hc_baller *b = &ctx->ballers[i];
    bool bdone = FALSE;
    यदि(!cf_hc_baller_is_connecting(b) || b->shutdown)
      अनुवर्त;
    b->परिणाम = b->cf->cft->do_shutdown(b->cf, दत्त, &bdone);
    यदि(b->परिणाम || bdone)
      b->shutdown = TRUE; /* treat a failed shutdown as done */
  }

  *समाप्तम् = TRUE;
  प्रति(i = 0; i < ctx->baller_गणनम्; i++) {
    यदि(!ctx->ballers[i].shutdown)
      *समाप्तम् = FALSE;
  }
  यदि(*समाप्तम्) {
    प्रति(i = 0; i < ctx->baller_गणनम्; i++) {
      यदि(ctx->ballers[i].परिणाम)
        परिणाम = ctx->ballers[i].परिणाम;
    }
  }
  CURL_TRC_CF(दत्त, cf, "shutdown -> %d, done=%d", (पूर्णाङ्क)परिणाम, *समाप्तम्);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_hc_adjust_pollset(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *दत्त,
                                     संरचना easy_pollset *ps)
{
  CURLcode परिणाम = CURLE_OK;
  यदि(!cf->connected) {
    संरचना cf_hc_ctx *ctx = cf->ctx;
    size_t i;

    प्रति(i = 0; (i < ctx->baller_गणनम्) && !परिणाम; i++) {
      संरचना cf_hc_baller *b = &ctx->ballers[i];
      यदि(!cf_hc_baller_is_connecting(b))
        अनुवर्त;
      परिणाम = Curl_conn_cf_adjust_pollset(b->cf, दत्त, ps);
    }
    CURL_TRC_CF(दत्त, cf, "adjust_pollset -> %d, %u socks", (पूर्णाङ्क)परिणाम,
                ps->n);
  }
  प्रत्यागम परिणाम;
}

स्थिर bool cf_hc_दत्त_pending(संरचना Curl_cfilter *cf,
                               ध्रुव संरचना Curl_easy *दत्त)
{
  संरचना cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  यदि(cf->connected)
    प्रत्यागम cf->अग्र->cft->has_दत्त_pending(cf->अग्र, दत्त);

  प्रति(i = 0; i < ctx->baller_गणनम्; i++)
    यदि(cf_hc_baller_दत्त_pending(&ctx->ballers[i], दत्त))
      प्रत्यागम TRUE;
  प्रत्यागम FALSE;
}

स्थिर CURLcode cf_hc_query(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *दत्त,
                            पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2)
{
  संरचना cf_hc_ctx *ctx = cf->ctx;
  size_t i;

  यदि(!cf->connected) {
    विकल्प(query) {
    पक्ष CF_QUERY_NEED_FLUSH: {
      प्रति(i = 0; i < ctx->baller_गणनम्; i++)
        यदि(cf_hc_baller_needs_flush(&ctx->ballers[i], दत्त)) {
          *pres1 = TRUE;
          प्रत्यागम CURLE_OK;
        }
      भङ्ग;
    }
    स्वभाव:
      भङ्ग;
    }
  }
  प्रत्यागम cf->अग्र ?
    cf->अग्र->cft->query(cf->अग्र, दत्त, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

स्थिर CURLcode cf_hc_cntrl(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *दत्त,
                            पूर्णाङ्क event, पूर्णाङ्क arg1, शून्य *arg2)
{
  संरचना cf_hc_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;
  size_t i;

  यदि(!cf->connected) {
    विकल्प(event) {
    पक्ष CF_CTRL_REPORT_STATS:
    पक्ष CF_CTRL_CONN_INFO_UPDATE:
      प्रति(i = 0; i < ctx->baller_गणनम्; i++) {
        /* Make the first baller that connected at network level report */
        यदि(Curl_conn_cf_is_ip_connected(ctx->ballers[i].cf, दत्त)) {
          Curl_conn_cf_cntrl(ctx->ballers[i].cf, दत्त, TRUE,
                             event, arg1, arg2);
          भङ्ग;
        }
      }
      भङ्ग;
    स्वभाव:
      प्रति(i = 0; i < ctx->baller_गणनम्; i++) {
        परिणाम = cf_hc_baller_cntrl(&ctx->ballers[i], दत्त, event, arg1, arg2);
        यदि(परिणाम && (परिणाम != CURLE_AGAIN))
          गच्छ बहिर्गमनम्;
      }
      परिणाम = CURLE_OK;
      भङ्ग;
    }
  }
बहिर्गमनम्:
  प्रत्यागम परिणाम;
}

स्थिर शून्य cf_hc_destroy(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त)
{
  संरचना cf_hc_ctx *ctx = cf->ctx;

  CURL_TRC_CF(दत्त, cf, "destroy");
  cf_hc_ctx_destroy(दत्त, ctx);
}

संरचना Curl_cftype Curl_cft_http_connect = {
  "HTTPS-CONNECT",
  CF_TYPE_सज्जता,
  CURL_LOG_LVL_NONE,
  cf_hc_destroy,
  cf_hc_connect,
  cf_hc_shutdown,
  cf_hc_adjust_pollset,
  cf_hc_दत्त_pending,
  Curl_cf_def_send,
  Curl_cf_def_recv,
  cf_hc_cntrl,
  Curl_cf_def_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_hc_query,
};

स्थिर CURLcode cf_hc_create(संरचना Curl_cfilter **pcf,
                             संरचना Curl_easy *दत्त,
                             संरचना Curl_peer *destination,
                             uint8_t def_transport)
{
  संरचना Curl_cfilter *cf = रिक्त;
  संरचना cf_hc_ctx *ctx;
  CURLcode परिणाम = CURLE_OK;

  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }
  Curl_peer_link(&ctx->destination, destination);
  ctx->def_transport = def_transport;
  ctx->hard_eyeballs_timeout_ms = दत्त->समुच्चयः.happy_eyeballs_timeout;
  ctx->soft_eyeballs_timeout_ms = दत्त->समुच्चयः.happy_eyeballs_timeout / 2;

  परिणाम = Curl_cf_create(&cf, &Curl_cft_http_connect, ctx);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  ctx = रिक्त;

बहिर्गमनम्:
  *pcf = परिणाम ? रिक्त : cf;
  cf_hc_ctx_destroy(दत्त, ctx);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_hc_सङ्कलय(संरचना Curl_easy *दत्त,
                          संरचना Curl_peer *destination,
                          संरचना connectdata *conn,
                          int8_t sockindex,
                          uint8_t def_transport)
{
  संरचना Curl_cfilter *cf;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(दत्त);
  परिणाम = cf_hc_create(&cf, दत्त, destination, def_transport);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  Curl_conn_cf_सङ्कलय(दत्त, conn, sockindex, cf);

#यदि_परिभाषितम् USE_HTTPSRR
  परिणाम = Curl_conn_dns_सङ्कलय_https_resolve(दत्त, cf->conn, cf->sockindex,
                                           destination);
#यदि_अन्तः
बहिर्गमनम्:
  प्रत्यागम परिणाम;
}

CURLcode Curl_cf_https_सज्जता(संरचना Curl_easy *दत्त,
                             संरचना Curl_peer *destination,
                             संरचना connectdata *conn,
                             int8_t sockindex)
{
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(conn->scheme->protocol == CURLPROTO_HTTPS);

  /* This filter is intended for HTTPS using ALPN and does
   * not support HTTPS Eyeballing to a proxy. */
  यदि((conn->scheme->protocol != CURLPROTO_HTTPS) ||
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
     conn->द्वयङ्काः.origin_is_proxy ||
#यदि_अन्तः
     !conn->द्वयङ्काः.tls_enable_alpn)
    गच्छ बहिर्गमनम्;

  परिणाम = cf_hc_सङ्कलय(दत्त, destination, conn, sockindex,
                     conn->transport_wanted);

बहिर्गमनम्:
  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* !CURL_DISABLE_HTTP */
