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

#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#ضمن <nghttp2/nghttp2.h>

#ضمن "urldata.h"
#ضمن "bufq.h"
#ضمن "u32_ptrset.h"
#ضمن "http1.h"
#ضمن "http2.h"
#ضمن "http.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "select.h"
#ضمن "curlx/base64.h"
#ضمن "multiif.h"
#ضمن "ngh2_shim.h"
#ضمن "progress.h"
#ضمن "url.h"
#ضمن "urlapi-int.h"
#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "transfer.h"
#ضمن "bufref.h"
#ضمن "curlx/dynbuf.h"
#ضمن "headers.h"
#ضمن "curl_share.h"

#إذا NGHTTP2_نسخو_انگ < 0x010f00
#غلطي "nghttp2 1.15.0 or greater required"
#جيڪڏهن_پورو

#وصف_ڪريو NGHTTP2_HAS_سيٽ_LOCAL_دري_سائيز 1

/* buffer dimensioning:
 * use 16K as chunk size, as that fits H2 DATA frames well */
#وصف_ڪريو H2_CHUNK_سائيز           (16 * 1024)
/* connection window size */
#وصف_ڪريو H2_CONN_دري_سائيز     (10 * 1024 * 1024)
/* on receiving from TLS, we prep for holding a full stream window */
#وصف_ڪريو H2_NW_RECV_CHUNKS       (H2_CONN_دري_سائيز / H2_CHUNK_سائيز)
/* on send into TLS, we want to accumulate small frames */
#وصف_ڪريو H2_NW_SEND_CHUNKS       1
/* this is how much we want "in flight" for a stream, unthrottled */
#وصف_ڪريو H2_وهڪرو_دري_سائيز_MAX   (10 * 1024 * 1024)
/* this is how much we want "in flight" for a stream, initially, IFF
 * nghttp2 allows us to tweak the local window size. */
#إذا NGHTTP2_HAS_سيٽ_LOCAL_دري_سائيز
#وصف_ڪريو H2_وهڪرو_دري_سائيز_INITIAL  (64 * 1024)
#وإلا
#وصف_ڪريو H2_وهڪرو_دري_سائيز_INITIAL H2_وهڪرو_دري_سائيز_MAX
#جيڪڏهن_پورو
/* keep smaller stream upload buffer (default h2 window size) to have
 * our progress bars and "upload done" reporting closer to reality */
#وصف_ڪريو H2_وهڪرو_SEND_CHUNKS   ((64 * 1024) / H2_CHUNK_سائيز)
/* spare chunks we keep for a full window */
#وصف_ڪريو H2_وهڪرو_POOL_SPARES   (H2_CONN_دري_سائيز / H2_CHUNK_سائيز)

/* We need to accommodate the max number of streams with their window sizes on
 * the overall connection. Streams might become PAUSED which will block their
 * received QUOTA in the connection window. If we run out of space, the server
 * is blocked from sending us any data. See #10988 for an issue with this. */
#وصف_ڪريو HTTP2_HUGE_دري_سائيز (100 * H2_وهڪرو_دري_سائيز_MAX)

#وصف_ڪريو H2_SETTINGS_IV_LEN 3
#وصف_ڪريو H2_BINSETTINGS_LEN 80

بنية cf_h2_ctx {
  بنية curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  بنية cf_call_معلومات call_معلومات;

  بنية bufq inbufq;           /* network input */
  بنية bufq outbufq;          /* network output */
  بنية bufc_pool وهڪرو_bufcp; /* spares for stream buffers */
  بنية dynbuf scratch;        /* scratch buffer for temp use */

  بنية u32_ptrset streams; /* hash of `data->mid` to `h2_stream_ctx` */
  size_t drain_ڪل; /* sum of all stream's UrlState drain */
  uint32_t initial_win_سائيز; /* current initial window size (settings) */
  uint32_t max_concurrent_streams;
  uint32_t goaway_error;        /* goaway error code from server */
  int32_t remote_max_sid;       /* max id processed by server */
  int32_t local_max_sid;        /* max id processed by us */
  ٻٽو_انگ(initialized);
  ٻٽو_انگ(ذريعي_h1_upgrade);
  ٻٽو_انگ(conn_closed);
  ٻٽو_انگ(rcvd_goaway);
  ٻٽو_انگ(موڪليل_goaway);
  ٻٽو_انگ(enable_ڌڪو);
  ٻٽو_انگ(nw_ٻاهريون_blocked);
};

/* How to access `call_data` from a cf_h2 filter */
#وصف_رد CF_CTX_CALL_معلومات
#وصف_ڪريو CF_CTX_CALL_معلومات(cf) ((بنية cf_h2_ctx *)(cf)->ctx)->call_معلومات

/**
 * All about the H2 internals of a stream
 */
بنية h2_وهڪرو_ctx {
  بنية bufq sendbuf; /* request buffer */
  بنية h1_req_parser h1; /* parsing the request */
  بنية dynhds resp_trailers; /* response trailer fields */
  size_t resp_hds_len; /* amount of response header bytes in recvbuf */
  curl_off_t xfer_id;  /* transfer `id` owning this stream */
  curl_off_t nrcvd_معلومات;  /* number of DATA bytes received */

  حرف **ڌڪو_سرا;       /* allocated array */
  size_t ڌڪو_سرا_استعمال_ٿيل;  /* number of entries filled in */
  size_t ڌڪو_سرا_alloc; /* number of entries allocated */

  int32_t id; /* HTTP/2 protocol identifier for stream */
  uint32_t mid; /* transfer `mid` owning this stream */
  uint32_t error; /* stream error code */
  CURLcode xfer_نتيجو; /* Result of writing out response */
  int32_t local_دري_سائيز; /* the local recv window size */
  int32_t حالت_ڪوڊ; /* HTTP response status code */
  ٻٽو_انگ(resp_hds_complete); /* we have a complete, final response */
  ٻٽو_انگ(closed); /* TRUE on stream close */
  ٻٽو_انگ(reset);  /* TRUE on stream reset */
  ٻٽو_انگ(reset_by_server);  /* TRUE on stream reset by server */
  ٻٽو_انگ(close_handled); /* TRUE if stream closure is handled by libcurl */
  ٻٽو_انگ(bodystarted);
  ٻٽو_انگ(body_eos);    /* the complete body has been added to `sendbuf` and
                     * is being/has been processed from there. */
  ٻٽو_انگ(write_paused);  /* stream write is paused */
};

ساكن فراغ free_ڌڪو_سرا(بنية h2_وهڪرو_ctx *وهڪرو)
{
  size_t i;
  لكل(i = 0; i < وهڪرو->ڌڪو_سرا_استعمال_ٿيل; i++)
    curlx_free(وهڪرو->ڌڪو_سرا[i]);
  curlx_safefree(وهڪرو->ڌڪو_سرا);
  وهڪرو->ڌڪو_سرا_استعمال_ٿيل = 0;
}

ساكن فراغ h2_وهڪرو_ctx_free(بنية h2_وهڪرو_ctx *وهڪرو)
{
  Curl_bufq_free(&وهڪرو->sendbuf);
  Curl_h1_req_parse_free(&وهڪرو->h1);
  Curl_dynhds_free(&وهڪرو->resp_trailers);
  free_ڌڪو_سرا(وهڪرو);
  curlx_free(وهڪرو);
}

ساكن فراغ h2_وهڪرو_hash_free(uint32_t id, فراغ *وهڪرو)
{
  (فراغ)id;
  DEBUGASSERT(وهڪرو);
  h2_وهڪرو_ctx_free((بنية h2_وهڪرو_ctx *)وهڪرو);
}

ساكن فراغ cf_h2_ctx_init(بنية cf_h2_ctx *ctx, bool ذريعي_h1_upgrade)
{
  Curl_bufcp_init(&ctx->وهڪرو_bufcp, H2_CHUNK_سائيز, H2_وهڪرو_POOL_SPARES);
  Curl_bufq_initp(&ctx->inbufq, &ctx->وهڪرو_bufcp, H2_NW_RECV_CHUNKS, 0);
  Curl_bufq_initp(&ctx->outbufq, &ctx->وهڪرو_bufcp, H2_NW_SEND_CHUNKS, 0);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_سرو);
  Curl_u32_ptrset_init(&ctx->streams, h2_وهڪرو_hash_free);
  ctx->remote_max_sid = INT32_MAX;
  ctx->ذريعي_h1_upgrade = ذريعي_h1_upgrade;
  ctx->initialized = TRUE;
}

ساكن فراغ cf_h2_ctx_free(بنية cf_h2_ctx *ctx)
{
  إذا(ctx && ctx->initialized) {
    Curl_ngh2_del(&ctx->ngh2);
    Curl_bufq_free(&ctx->inbufq);
    Curl_bufq_free(&ctx->outbufq);
    Curl_bufcp_free(&ctx->وهڪرو_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    memset(ctx, 0, حجم(*ctx));
  }
  curlx_free(ctx);
}

ساكن uint32_t cf_h2_initial_win_سائيز(بنية Curl_easy *معلومات)
{
#إذا NGHTTP2_HAS_سيٽ_LOCAL_دري_سائيز
  /* If the transfer has a rate-limit lower than the default initial
   * stream window size, use that. It needs to be at least 8k or servers
   * may be unhappy. */
  curl_off_t rps = Curl_rlimit_في_قدم(&معلومات->ترقي.dl.rlimit);
  إذا((rps > 0) && (rps < H2_وهڪرو_دري_سائيز_INITIAL))
    أرجع CURLMAX((uint32_t)rps, 8192);
#جيڪڏهن_پورو
  أرجع H2_وهڪرو_دري_سائيز_INITIAL;
}

ساكن size_t populate_settings(nghttp2_settings_داخلا *iv,
                                بنية Curl_easy *معلومات,
                                بنية cf_h2_ctx *ctx)
{
  iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
  iv[0].قدر = Curl_multi_max_concurrent_streams(معلومات->multi);

  iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_دري_سائيز;
  iv[1].قدر = cf_h2_initial_win_سائيز(معلومات);
  إذا(ctx)
    ctx->initial_win_سائيز = iv[1].قدر;
  iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_ڌڪو;
  iv[2].قدر = !!معلومات->multi->ڌڪو_cb;

  أرجع 3;
}

ساكن ssize_t populate_binsettings(uint8_t *binsettings,
                                    بنية Curl_easy *معلومات)
{
  nghttp2_settings_داخلا iv[H2_SETTINGS_IV_LEN];
  size_t ivlen;

  ivlen = populate_settings(iv, معلومات, عدم);
  /* this returns number of bytes it wrote or a negative number on error. */
  أرجع nghttp2_pack_settings_payload(binsettings, H2_BINSETTINGS_LEN,
                                       iv, ivlen);
}

ساكن CURLcode cf_h2_update_settings(بنية cf_h2_ctx *ctx,
                                      uint32_t initial_win_سائيز)
{
  nghttp2_settings_داخلا داخلا_2;
  داخلا_2.settings_id = NGHTTP2_SETTINGS_INITIAL_دري_سائيز;
  داخلا_2.قدر = initial_win_سائيز;
  إذا(Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, &داخلا_2, 1))
    أرجع CURLE_SEND_ERROR;
  ctx->initial_win_سائيز = initial_win_سائيز;
  أرجع CURLE_OK;
}

#وصف_ڪريو H2_وهڪرو_CTX(ctx, معلومات)                                         \
  ((بنية h2_وهڪرو_ctx *)(                                             \
    (معلومات) ? Curl_u32_ptrset_get(&(ctx)->streams, (معلومات)->mid) : عدم))

ساكن بنية h2_وهڪرو_ctx *h2_وهڪرو_ctx_create(بنية cf_h2_ctx *ctx,
                                                  بنية Curl_easy *معلومات)
{
  بنية h2_وهڪرو_ctx *وهڪرو;

  وهڪرو = curlx_calloc(1, حجم(*وهڪرو));
  إذا(!وهڪرو)
    أرجع عدم;

  وهڪرو->id = -1;
  وهڪرو->xfer_id = معلومات->id;
  وهڪرو->mid = معلومات->mid;
  Curl_bufq_initp(&وهڪرو->sendbuf, &ctx->وهڪرو_bufcp,
                  H2_وهڪرو_SEND_CHUNKS, BUFQ_OPT_NONE);
  Curl_h1_req_parse_init(&وهڪرو->h1, H1_PARSE_DEFAULT_MAX_سٽ_LEN);
  Curl_dynhds_init(&وهڪرو->resp_trailers,
                   MAX_HTTP_RESP_سرو_ڳڻپ, DYN_HTTP_REQUEST);
  وهڪرو->bodystarted = FALSE;
  وهڪرو->حالت_ڪوڊ = -1;
  وهڪرو->closed = FALSE;
  وهڪرو->close_handled = FALSE;
  وهڪرو->error = NGHTTP2_NO_ERROR;
  وهڪرو->local_دري_سائيز = H2_وهڪرو_دري_سائيز_INITIAL;
  وهڪرو->nrcvd_معلومات = 0;
  أرجع وهڪرو;
}

ساكن بنية Curl_easy *cf_h2_get_وهڪرو_xfer(بنية Curl_cfilter *cf,
                                               int32_t وهڪرو_id,
                                               بنية h2_وهڪرو_ctx **pstream)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_وهڪرو_ctx *وهڪرو =
    Curl_ngh2_session_get_وهڪرو_user_معلومات(&ctx->ngh2, وهڪرو_id);
  إذا(pstream)
    *pstream = وهڪرو;
  إذا(وهڪرو && (وهڪرو->mid != UINT32_MAX)) {
    بنية Curl_easy *معلومات, *call_معلومات = CF_معلومات_CURRENT(cf);
    إذا(!call_معلومات || !call_معلومات->multi) {
      DEBUGASSERT(0);
      أرجع عدم;
    }
    معلومات = Curl_multi_get_easy(call_معلومات->multi, وهڪرو->mid);
    إذا(معلومات && (معلومات->id == وهڪرو->xfer_id))
      أرجع معلومات;
  }
  أرجع عدم;
}

#جيڪڏهن_وصف_ٿيل NGHTTP2_HAS_سيٽ_LOCAL_دري_سائيز
ساكن int32_t cf_h2_get_desired_local_win(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *معلومات)
{
  curl_off_t avail = Curl_rlimit_avail(&معلومات->ترقي.dl.rlimit, عدم);

  (فراغ)cf;
  إذا(avail < CURL_OFF_T_MAX) { /* limit in place */
    إذا(avail <= 0)
      أرجع 0;
    وإلا إذا(avail < H2_وهڪرو_دري_سائيز_MAX)
      أرجع (int32_t)avail;
  }
  أرجع H2_وهڪرو_دري_سائيز_MAX;
}

ساكن CURLcode cf_h2_update_local_win(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *معلومات,
                                       بنية h2_وهڪرو_ctx *وهڪرو)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  int32_t dwsize;
  صحيح rv;

  dwsize = (وهڪرو->write_paused || وهڪرو->xfer_نتيجو) ?
           0 : cf_h2_get_desired_local_win(cf, معلومات);
  إذا(dwsize != وهڪرو->local_دري_سائيز) {
    int32_t wsize = Curl_ngh2_session_get_وهڪرو_effective_local_دري_سائيز(
      &ctx->ngh2, وهڪرو->id);
    إذا(dwsize > wsize) {
      rv = Curl_ngh2_session_سيٽ_local_دري_سائيز(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, وهڪرو->id, dwsize);
      إذا(rv) {
        failf(معلومات, "[%d] nghttp2 set_local_window_size(%d) failed: "
              "%s(%d)", وهڪرو->id, dwsize, nghttp2_strerror(rv), rv);
        أرجع CURLE_HTTP2;
      }
      rv = Curl_ngh2_submit_دري_update(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, وهڪرو->id, dwsize - wsize);
      إذا(rv) {
        failf(معلومات, "[%d] nghttp2_submit_window_update() failed: "
              "%s(%d)", وهڪرو->id, nghttp2_strerror(rv), rv);
        أرجع CURLE_HTTP2;
      }
      وهڪرو->local_دري_سائيز = dwsize;
      CURL_TRC_CF(معلومات, cf, "[%d] local window update by %d",
                  وهڪرو->id, dwsize - wsize);
    }
    وإلا {
      rv = Curl_ngh2_session_سيٽ_local_دري_سائيز(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, وهڪرو->id, dwsize);
      إذا(rv) {
        failf(معلومات, "[%d] nghttp2_session_set_local_window_size() failed: "
              "%s(%d)", وهڪرو->id, nghttp2_strerror(rv), rv);
        أرجع CURLE_HTTP2;
      }
      وهڪرو->local_دري_سائيز = dwsize;
      CURL_TRC_CF(معلومات, cf, "[%d] local window size now %d",
                  وهڪرو->id, dwsize);
    }
  }
  أرجع CURLE_OK;
}

#وإلا /* NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

ساكن CURLcode cf_h2_update_local_win(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *معلومات,
                                       بنية h2_وهڪرو_ctx *وهڪرو)
{
  (فراغ)cf;
  (فراغ)معلومات;
  (فراغ)وهڪرو;
  أرجع CURLE_OK;
}
#جيڪڏهن_پورو /* !NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

ساكن CURLcode http2_معلومات_تياري(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 بنية h2_وهڪرو_ctx **pstream)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_وهڪرو_ctx *وهڪرو;

  (فراغ)cf;
  DEBUGASSERT(معلومات);

  إذا(!معلومات)
    أرجع CURLE_BAD_FUNCTION_دليل;

  وهڪرو = H2_وهڪرو_CTX(ctx, معلومات);
  إذا(وهڪرو) {
    *pstream = وهڪرو;
    أرجع CURLE_OK;
  }

  وهڪرو = h2_وهڪرو_ctx_create(ctx, معلومات);
  إذا(!وهڪرو)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  إذا(Curl_u32_ptrset_سيٽ(&ctx->streams, معلومات->mid, وهڪرو)) {
    h2_وهڪرو_ctx_free(وهڪرو);
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  }

  *pstream = وهڪرو;
  أرجع CURLE_OK;
}

ساكن CURLcode nw_ٻاهريون_flush(بنية Curl_cfilter *cf,
                             بنية Curl_easy *معلومات)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode نتيجو;

  إذا(Curl_bufq_is_خالي_جايون(&ctx->outbufq))
    أرجع CURLE_OK;

  نتيجو = Curl_cf_send_bufq(cf->ايندڙ, معلومات, &ctx->outbufq, عدم, 0,
                             &nwritten);
  إذا(نتيجو) {
    إذا(نتيجو == CURLE_AGAIN) {
      CURL_TRC_CF(معلومات, cf, "flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_ٻاهريون_blocked = 1;
    }
    أرجع نتيجو;
  }
  أرجع Curl_bufq_is_خالي_جايون(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

ساكن فراغ http2_معلومات_پورو(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_وهڪرو_ctx *وهڪرو = H2_وهڪرو_CTX(ctx, معلومات);

  DEBUGASSERT(ctx);
  إذا(!وهڪرو || !ctx->initialized)
    أرجع;

  إذا(Curl_ngh2_valid(&ctx->ngh2)) {
    bool flush_egress = FALSE;
    /* returns error if stream not known, which is fine here */
    (فراغ)Curl_ngh2_session_سيٽ_وهڪرو_user_معلومات(&ctx->ngh2, وهڪرو->id, عدم);

    إذا(!وهڪرو->closed && وهڪرو->id > 0) {
      /* RST_STREAM */
      CURL_TRC_CF(معلومات, cf, "[%d] premature DATA_DONE, RST stream",
                  وهڪرو->id);
      وهڪرو->closed = TRUE;
      وهڪرو->reset = TRUE;
      Curl_ngh2_submit_rst_وهڪرو(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                  وهڪرو->id, NGHTTP2_وهڪرو_CLOSED);
      flush_egress = TRUE;
    }

    إذا(flush_egress) {
      (فراغ)Curl_ngh2_session_send(&ctx->ngh2);
      (فراغ)nw_ٻاهريون_flush(cf, معلومات);
    }
  }

  Curl_u32_ptrset_unset(&ctx->streams, معلومات->mid);
}

ساكن صحيح h2_client_new(بنية Curl_cfilter *cf,
                         nghttp2_session_callbacks *cbs)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { عدم, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  صحيح rc = nghttp2_option_new(&o);
  إذا(rc)
    أرجع rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_سيٽ_no_auto_دري_update(o, 1);
#إذا NGHTTP2_نسخو_انگ >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_سيٽ_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#جيڪڏهن_پورو
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  أرجع rc;
}

/*
 * Returns nonzero if current HTTP/2 session should be closed.
 */
ساكن صحيح should_close_session(بنية cf_h2_ctx *ctx)
{
  أرجع ctx->drain_ڪل == 0 && !Curl_ngh2_session_want_read(&ctx->ngh2) &&
    !Curl_ngh2_session_want_write(&ctx->ngh2);
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
ساكن CURLcode h2_process_pending_داخلا(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *معلومات)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  ثابت بنانشان حرف *buf;
  size_t blen, nread;
  ssize_t rv;

  طالما(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {
    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ثابت uint8_t *)buf, blen);
    إذا(!curlx_sztouz(rv, &nread)) {
      failf(معلومات, "nghttp2 recv error %zd: %s", rv, nghttp2_strerror((صحيح)rv));
      أرجع CURLE_RECV_ERROR;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    إذا(Curl_bufq_is_خالي_جايون(&ctx->inbufq)) {
      اكسر;
    }
    وإلا {
      CURL_TRC_CF(معلومات, cf, "process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }

  إذا(Curl_ngh2_session_check_request_allowed(&ctx->ngh2) == 0) {
    /* No more requests are allowed in the current session, so
       the connection may not be reused. This is set when a
       GOAWAY frame has been received or when the limit of stream
       identifiers has been reached. */
    CURL_TRC_M(معلومات, "http/2: No new requests allowed");
    connclose(cf->conn);
  }

  أرجع CURLE_OK;
}

/*
 * The server may send us data at any point (e.g. PING frames). Therefore, we
 * cannot assume that an HTTP/2 socket is dead because it is readable.
 *
 * Check the lower filters first and, if successful, peek at the socket
 * and distinguish between closed and data.
 */
ساكن bool http2_connisalive(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                              bool *داخلا_pending)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *داخلا_pending = FALSE;
  إذا(!cf->ايندڙ || !cf->ايندڙ->cft->is_alive(cf->ايندڙ, معلومات, داخلا_pending))
    أرجع FALSE;

  إذا(*داخلا_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode نتيجو;
    size_t nread;

    *داخلا_pending = FALSE;
    نتيجو = Curl_cf_recv_bufq(cf->ايندڙ, معلومات, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(معلومات, cf, "connisalive, recv pending input -> %d, %zu",
                (صحيح)نتيجو, nread);
    إذا(!نتيجو) {
      نتيجو = h2_process_pending_داخلا(cf, معلومات);
      إذا(نتيجو)
        /* immediate error, considered dead */
        alive = FALSE;
      وإلا {
        alive = !should_close_session(ctx);
      }
    }
    وإلا إذا(نتيجو != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  أرجع alive;
}

ساكن CURLcode http2_send_ping(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  صحيح rc;

  rc = Curl_ngh2_submit_ping(&ctx->ngh2, 0, ZERO_NULL);
  إذا(rc) {
    failf(معلومات, "nghttp2_submit_ping() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    أرجع CURLE_HTTP2;
  }

  rc = Curl_ngh2_session_send(&ctx->ngh2);
  إذا(rc) {
    failf(معلومات, "nghttp2_session_send() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    أرجع CURLE_SEND_ERROR;
  }
  أرجع CURLE_OK;
}

/*
 * Store nghttp2 version info in this buffer.
 */
فراغ Curl_http2_ver(حرف *p, size_t len)
{
  nghttp2_info *h2 = nghttp2_نسخو(0);
  (فراغ)curl_msnprintf(p, len, "nghttp2/%s", h2->نسخو_str);
}

/*
 * The implementation of nghttp2_send_callback type. Here we write |data| with
 * size |length| to the network and return the number of bytes actually
 * written. See the documentation of nghttp2_send_callback for the details.
 */
ساكن ssize_t send_callback(nghttp2_session *h2,
                             ثابت uint8_t *buf, size_t blen, صحيح flags,
                             فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
  size_t nwritten;
  CURLcode نتيجو = CURLE_OK;

  (فراغ)h2;
  (فراغ)flags;
  DEBUGASSERT(معلومات);

  إذا(!cf->connected)
    نتيجو = Curl_bufq_write(&ctx->outbufq, buf, blen, &nwritten);
  وإلا
    نتيجو = Curl_cf_send_bufq(cf->ايندڙ, معلومات, &ctx->outbufq, buf, blen,
                               &nwritten);

  إذا(نتيجو) {
    إذا(نتيجو == CURLE_AGAIN) {
      ctx->nw_ٻاهريون_blocked = 1;
      أرجع NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(معلومات, "Failed sending HTTP2 data");
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  إذا(!nwritten) {
    ctx->nw_ٻاهريون_blocked = 1;
    أرجع NGHTTP2_ERR_WOULDBLOCK;
  }
  أرجع (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

/* We pass a pointer to this struct in the push callback, but the contents of
   the struct are hidden from the user. */
بنية curl_pushheaders {
  بنية Curl_easy *معلومات;
  بنية h2_وهڪرو_ctx *وهڪرو;
  ثابت nghttp2_ڌڪو_promise *چوکٺو;
};

/*
 * push header access function. Only to be used from within the push callback
 */
حرف *curl_pushheader_bynum(بنية curl_pushheaders *h, size_t انگ)
{
  /* Verify that we got a good easy handle in the push header struct, mostly to
     detect rubbish input fast(er). */
  إذا(!h || !GOOD_EASY_HANDLE(h->معلومات))
    أرجع عدم;
  وإلا {
    إذا(h->وهڪرو && انگ < h->وهڪرو->ڌڪو_سرا_استعمال_ٿيل)
      أرجع h->وهڪرو->ڌڪو_سرا[انگ];
  }
  أرجع عدم;
}

/*
 * push header access function. Only to be used from within the push callback
 */
حرف *curl_pushheader_byname(بنية curl_pushheaders *h, ثابت حرف *name)
{
  بنية h2_وهڪرو_ctx *وهڪرو;
  size_t len;
  size_t i;
  /* Verify that we got a good easy handle in the push header struct,
     mostly to detect rubbish input fast(er). Also empty header name
     is rubbish too. We have to allow ":" at the beginning of
     the header, but header == ":" must be rejected. If we have ':' in
     the middle of header, it could be matched in middle of the value,
     this is because we do prefix match.*/
  إذا(!h || !GOOD_EASY_HANDLE(h->معلومات) || !name || !name[0] ||
     !strcmp(name, ":") || strchr(name + 1, ':'))
    أرجع عدم;

  وهڪرو = h->وهڪرو;
  إذا(!وهڪرو)
    أرجع عدم;

  len = لڙي_ڊيگهه(name);
  لكل(i = 0; i < وهڪرو->ڌڪو_سرا_استعمال_ٿيل; i++) {
    إذا(!لڙي_ڀيٽ(name, وهڪرو->ڌڪو_سرا[i], len)) {
      /* sub-match, make sure that it is followed by a colon */
      إذا(وهڪرو->ڌڪو_سرا[i][len] != ':')
        استمر;
      أرجع &وهڪرو->ڌڪو_سرا[i][len + 1];
    }
  }
  أرجع عدم;
}

ساكن بنية Curl_easy *h2_duphandle(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات)
{
  بنية Curl_easy *ٻيو = curl_easy_init();
  إذا(ٻيو) {
    بنية h2_وهڪرو_ctx *ٻيو_وهڪرو;
    http2_معلومات_تياري(cf, ٻيو, &ٻيو_وهڪرو);
    ٻيو->state.وزن = معلومات->state.وزن;
    إذا(معلومات->share)
      (فراغ)Curl_share_easy_link(ٻيو, معلومات->share);
    /* copy a range of options from the parent handle */
    ٻيو->سيٽ.fwrite_func = معلومات->سيٽ.fwrite_func;
    ٻيو->سيٽ.ٻاهريون = معلومات->سيٽ.ٻاهريون;
    ٻيو->سيٽ.fwrite_سرو = معلومات->سيٽ.fwrite_سرو;
    ٻيو->سيٽ.writeheader = معلومات->سيٽ.writeheader;
    ٻيو->سيٽ.fdebug = معلومات->سيٽ.fdebug;
    ٻيو->سيٽ.debugdata = معلومات->سيٽ.debugdata;
    ٻيو->سيٽ.fxferinfo = معلومات->سيٽ.fxferinfo;
    ٻيو->سيٽ.fprogress = معلومات->سيٽ.fprogress;
    ٻيو->سيٽ.ترقي_client = معلومات->سيٽ.ترقي_client;
    ٻيو->ترقي.hide = معلومات->ترقي.hide;
    ٻيو->ترقي.callback = معلومات->ترقي.callback;
  }
  أرجع ٻيو;
}

ساكن صحيح سيٽ_transfer_url(بنية Curl_easy *newhandle,
                            بنية curl_pushheaders *hp,
                            بنية Curl_easy *معلومات)
{
  ثابت حرف *v;
  CURLUcode uc;
  حرف *url = عدم;
  صحيح rc = 0;
  CURLU *u = curl_url();

  إذا(!u)
    أرجع 5;

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_SCHEME);
  إذا(v) {
    uc = curl_url_سيٽ(u, CURLUPART_SCHEME, v, 0);
    إذا(uc) {
      rc = 1;
      اذهب fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_AUTHORITY);
  إذا(v) {
    uc = Curl_url_سيٽ_authority(u, v);
    إذا(uc) {
      rc = 2;
      اذهب fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_رستو);
  إذا(v) {
    uc = curl_url_سيٽ(u, CURLUPART_رستو, v, 0);
    إذا(uc) {
      rc = 3;
      اذهب fail;
    }
  }

  /* We can only allow PUSH of resource from the same origin, e.g.
   * scheme + hostname + port */
  إذا(!Curl_url_same_origin(معلومات->state.uh, u)) {
    rc = 1;
    اذهب fail;
  }

  uc = curl_url_get(u, CURLUPART_URL, &url, 0);
  إذا(uc)
    rc = 4;
fail:
  إذا(rc) {
    curl_url_cleanup(u);
    أرجع rc;
  }

  DEBUGASSERT(!newhandle->state.uh);
  newhandle->state.uh = u;
  Curl_bufref_سيٽ(&newhandle->state.url, url, 0, curl_free);
  أرجع 0;
}

ساكن فراغ discard_newhandle(بنية Curl_cfilter *cf,
                              بنية Curl_easy *newhandle)
{
  http2_معلومات_پورو(cf, newhandle);
  (فراغ)Curl_close(&newhandle);
}

ساكن صحيح ڌڪو_promise(بنية Curl_cfilter *cf,
                        بنية Curl_easy *معلومات,
                        ثابت nghttp2_ڌڪو_promise *چوکٺو)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  صحيح rv; /* one of the CURL_PUSH_* defines */

  CURL_TRC_CF(معلومات, cf, "[%d] PUSH_PROMISE received",
              چوکٺو->promised_وهڪرو_id);
  إذا(معلومات->multi->ڌڪو_cb) {
    بنية h2_وهڪرو_ctx *وهڪرو;
    بنية h2_وهڪرو_ctx *newstream;
    بنية curl_pushheaders heads;
    CURLMcode mresult;
    CURLcode نتيجو;
    /* clone the parent */
    بنية Curl_easy *newhandle = h2_duphandle(cf, معلومات);
    إذا(!newhandle) {
      infof(معلومات, "failed to duplicate handle");
      rv = CURL_ڌڪو_DENY; /* FAIL HARD */
      اذهب fail;
    }

    وهڪرو = H2_وهڪرو_CTX(ctx, معلومات);
    إذا(!وهڪرو) {
      failf(معلومات, "Internal NULL stream");
      discard_newhandle(cf, newhandle);
      rv = CURL_ڌڪو_DENY;
      اذهب fail;
    }

    heads.معلومات = معلومات;
    heads.وهڪرو = وهڪرو;
    heads.چوکٺو = چوکٺو;

    rv = سيٽ_transfer_url(newhandle, &heads, معلومات);
    إذا(rv) {
      CURL_TRC_CF(معلومات, cf, "[%d] PUSH_PROMISE, failed to set URL -> %d",
                  چوکٺو->promised_وهڪرو_id, rv);
      discard_newhandle(cf, newhandle);
      rv = CURL_ڌڪو_DENY;
      اذهب fail;
    }

    {
      بنية Curl_mapi_guard guard;
      CURL_CBAPI_شروعات(&guard, معلومات, multi_ڌڪو_cb);
      rv = معلومات->multi->ڌڪو_cb(معلومات, newhandle,
                                وهڪرو->ڌڪو_سرا_استعمال_ٿيل, &heads,
                                معلومات->multi->ڌڪو_userp);
      CURL_CBAPI_پڇاڙي(&guard);
    }

    /* free the headers again */
    free_ڌڪو_سرا(وهڪرو);

    إذا(rv) {
      DEBUGASSERT((rv > CURL_ڌڪو_OK) && (rv <= CURL_ڌڪو_ERROROUT));
      /* denied, kill off the new handle again */
      CURL_TRC_CF(معلومات, cf, "[%d] PUSH_PROMISE, denied by application -> %d",
                  چوکٺو->promised_وهڪرو_id, rv);
      discard_newhandle(cf, newhandle);
      اذهب fail;
    }

    /* approved, add to the multi handle for processing. This
     * assigns newhandle->mid. For the new `mid` we assign the
     * h2_stream instance and remember the stream_id already known. */
    mresult = Curl_multi_جوڙيو_perform(معلومات->multi, newhandle, cf->conn);
    إذا(mresult) {
      infof(معلومات, "failed to add handle to multi");
      discard_newhandle(cf, newhandle);
      rv = CURL_ڌڪو_DENY;
      اذهب fail;
    }

    نتيجو = http2_معلومات_تياري(cf, newhandle, &newstream);
    إذا(نتيجو) {
      failf(معلومات, "error setting up stream: %d", (صحيح)نتيجو);
      discard_newhandle(cf, newhandle);
      rv = CURL_ڌڪو_DENY;
      اذهب fail;
    }

    DEBUGASSERT(newstream);
    newstream->id = چوکٺو->promised_وهڪرو_id;
    newhandle->req.maxdownload = -1;
    newhandle->req.سائيز = -1;

    CURL_TRC_CF(معلومات, cf, "promise easy handle added to multi, mid=%u",
                newhandle->mid);
    rv = Curl_ngh2_session_سيٽ_وهڪرو_user_معلومات(
      &ctx->ngh2, newstream->id, newstream);
    إذا(rv) {
      infof(معلومات, "failed to set user_data for stream %d",
            newstream->id);
      DEBUGASSERT(0);
      discard_newhandle(cf, newhandle);
      rv = CURL_ڌڪو_DENY;
      اذهب fail;
    }

    /* success, remember max stream id processed */
    إذا(newstream->id > ctx->local_max_sid)
      ctx->local_max_sid = newstream->id;
  }
  وإلا {
    CURL_TRC_CF(معلومات, cf, "Got PUSH_PROMISE, ignore it");
    rv = CURL_ڌڪو_DENY;
  }
fail:
  أرجع rv;
}

ساكن فراغ h2_xfer_write_resp_hd(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  بنية h2_وهڪرو_ctx *وهڪرو,
                                  ثابت حرف *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  إذا(!وهڪرو->xfer_نتيجو) {
    وهڪرو->xfer_نتيجو = Curl_xfer_write_resp_hd(معلومات, buf, blen, eos);
    إذا(!وهڪرو->xfer_نتيجو && !eos)
      وهڪرو->xfer_نتيجو = cf_h2_update_local_win(cf, معلومات, وهڪرو);
    إذا(وهڪرو->xfer_نتيجو)
      CURL_TRC_CF(معلومات, cf, "[%d] error %d writing %zu bytes of headers",
                  وهڪرو->id, (صحيح)وهڪرو->xfer_نتيجو, blen);
  }
}

ساكن فراغ h2_xfer_write_resp(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات,
                               بنية h2_وهڪرو_ctx *وهڪرو,
                               ثابت حرف *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  إذا(!وهڪرو->xfer_نتيجو)
    وهڪرو->xfer_نتيجو = Curl_xfer_write_resp(معلومات, buf, blen, eos);
  /* If the transfer write is errored, we do not want any more data */
  إذا(وهڪرو->xfer_نتيجو) {
    بنية cf_h2_ctx *ctx = cf->ctx;
    CURL_TRC_CF(معلومات, cf, "[%d] error %d writing %zu bytes of data, "
                "RST-ing stream",
                وهڪرو->id, (صحيح)وهڪرو->xfer_نتيجو, blen);
    Curl_ngh2_submit_rst_وهڪرو(
      &ctx->ngh2, 0, وهڪرو->id, (uint32_t)NGHTTP2_ERR_CALLBACK_FAILURE);
  }
  وإلا إذا(!وهڪرو->write_paused && Curl_xfer_write_is_paused(معلومات)) {
    CURL_TRC_CF(معلومات, cf, "[%d] stream output paused", وهڪرو->id);
    وهڪرو->write_paused = TRUE;
  }
  وإلا إذا(وهڪرو->write_paused && !Curl_xfer_write_is_paused(معلومات)) {
    CURL_TRC_CF(معلومات, cf, "[%d] stream output unpaused", وهڪرو->id);
    وهڪرو->write_paused = FALSE;
  }

  إذا(!وهڪرو->xfer_نتيجو && !eos)
    وهڪرو->xfer_نتيجو = cf_h2_update_local_win(cf, معلومات, وهڪرو);
}

ساكن CURLcode on_وهڪرو_چوکٺو(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                بنية h2_وهڪرو_ctx *وهڪرو,
                                ثابت nghttp2_چوکٺو *چوکٺو)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  int32_t وهڪرو_id = چوکٺو->hd.وهڪرو_id;
  صحيح rv;

  إذا(!وهڪرو) {
    CURL_TRC_CF(معلومات, cf, "[%d] No stream_ctx set", وهڪرو_id);
    أرجع CURLE_FAILED_INIT;
  }

  بدل(چوکٺو->hd.type) {
  حالة NGHTTP2_معلومات:
    CURL_TRC_CF(معلومات, cf, "[%d] DATA, window=%d/%d",
                وهڪرو_id,
                Curl_ngh2_session_get_وهڪرو_effective_recv_معلومات_ڊيگهائي(
                  &ctx->ngh2, وهڪرو->id),
                Curl_ngh2_session_get_وهڪرو_effective_local_دري_سائيز(
                  &ctx->ngh2, وهڪرو->id));
    /* If !body started on this stream, then receiving DATA is illegal. */
    إذا(!وهڪرو->bodystarted) {
      إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_وهڪرو(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                       وهڪرو_id, NGHTTP2_PROTOCOL_ERROR))) {
        أرجع CURLE_RECV_ERROR;
      }
    }
    اكسر;
  حالة NGHTTP2_سرا:
    إذا(وهڪرو->bodystarted) {
      /* Only valid HEADERS after body started is trailer HEADERS. We
         buffer them in on_header callback. */
      اكسر;
    }

    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    إذا(وهڪرو->حالت_ڪوڊ == -1)
      أرجع CURLE_RECV_ERROR;

    /* Only final status code signals the end of header */
    إذا(وهڪرو->حالت_ڪوڊ / 100 != 1) {
      وهڪرو->resp_hds_complete = TRUE;
      وهڪرو->bodystarted = TRUE;
    }
    وإلا
      وهڪرو->حالت_ڪوڊ = -1;

    h2_xfer_write_resp_hd(cf, معلومات, وهڪرو, STRCONST("\r\n"),
                          (bool)وهڪرو->closed);

    Curl_multi_mark_dirty(معلومات);
    اكسر;
  حالة NGHTTP2_ڌڪو_PROMISE:
    rv = ڌڪو_promise(cf, معلومات, &چوکٺو->ڌڪو_promise);
    إذا(rv) { /* deny! */
      DEBUGASSERT((rv > CURL_ڌڪو_OK) && (rv <= CURL_ڌڪو_ERROROUT));
      إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_وهڪرو(&ctx->ngh2, NGHTTP2_FLAG_NONE,
             چوکٺو->ڌڪو_promise.promised_وهڪرو_id, NGHTTP2_CANCEL)))
        أرجع CURLE_SEND_ERROR;
      وإلا إذا(rv == CURL_ڌڪو_ERROROUT) {
        /* fail the parent transfer, not the whole session */
        CURL_TRC_CF(معلومات, cf, "[%d] fail in PUSH_PROMISE received",
                    وهڪرو_id);
        إذا(!وهڪرو->xfer_نتيجو)
          وهڪرو->xfer_نتيجو = CURLE_RECV_ERROR;
        Curl_multi_mark_dirty(معلومات);
      }
    }
    اكسر;
  حالة NGHTTP2_RST_وهڪرو:
    إذا(چوکٺو->rst_وهڪرو.error_ڪوڊ)
      وهڪرو->reset_by_server = TRUE;
    Curl_multi_mark_dirty(معلومات);
    اكسر;
  حالة NGHTTP2_دري_UPDATE:
    إذا(CURL_REQ_WANT_SEND(معلومات) && Curl_bufq_is_خالي_جايون(&وهڪرو->sendbuf)) {
      /* need more data, force processing of transfer */
      Curl_multi_mark_dirty(معلومات);
    }
    وإلا إذا(!Curl_bufq_is_خالي_جايون(&وهڪرو->sendbuf)) {
      /* resume the potentially suspended stream */
      إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_session_resume_معلومات(&ctx->ngh2, وهڪرو->id)))
        أرجع CURLE_SEND_ERROR;
    }
    اكسر;
  افتراضي:
    اكسر;
  }

  إذا(چوکٺو->hd.flags & NGHTTP2_FLAG_پڇاڙي_وهڪرو) {
    إذا(!وهڪرو->closed && !وهڪرو->body_eos &&
       ((وهڪرو->حالت_ڪوڊ >= 400) || (وهڪرو->حالت_ڪوڊ < 200))) {
      /* The server did not give us a positive response and we are not
       * done uploading the request body. We need to stop doing that and
       * also inform the server that we aborted our side. */
      CURL_TRC_CF(معلومات, cf, "[%d] EOS frame with unfinished upload and "
                  "HTTP status %d, abort upload by RST",
                  وهڪرو_id, وهڪرو->حالت_ڪوڊ);
      Curl_ngh2_submit_rst_وهڪرو(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, وهڪرو->id, NGHTTP2_وهڪرو_CLOSED);
      وهڪرو->closed = TRUE;
    }
    Curl_multi_mark_dirty(معلومات);
  }
  أرجع CURLE_OK;
}

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
صحيح Curl_nghttp2_fr_print(ثابت nghttp2_چوکٺو *چوکٺو, حرف *بفر,
                          size_t blen)
{
  بدل(چوکٺو->hd.type) {
  حالة NGHTTP2_معلومات: {
    أرجع curl_msnprintf(بفر, blen,
                          "FRAME[DATA, len=%d, eos=%d, padlen=%d]",
                          (صحيح)چوکٺو->hd.ڊيگهائي,
                          !!(چوکٺو->hd.flags & NGHTTP2_FLAG_پڇاڙي_وهڪرو),
                          (صحيح)چوکٺو->معلومات.padlen);
  }
  حالة NGHTTP2_سرا: {
    أرجع curl_msnprintf(بفر, blen,
                          "FRAME[HEADERS, len=%d, hend=%d, eos=%d]",
                          (صحيح)چوکٺو->hd.ڊيگهائي,
                          !!(چوکٺو->hd.flags & NGHTTP2_FLAG_پڇاڙي_سرا),
                          !!(چوکٺو->hd.flags & NGHTTP2_FLAG_پڇاڙي_وهڪرو));
  }
  حالة NGHTTP2_ترجيح: {
    أرجع curl_msnprintf(بفر, blen,
                          "FRAME[PRIORITY, len=%d, flags=%d]",
                          (صحيح)چوکٺو->hd.ڊيگهائي, چوکٺو->hd.flags);
  }
  حالة NGHTTP2_RST_وهڪرو: {
    أرجع curl_msnprintf(بفر, blen,
                          "FRAME[RST_STREAM, len=%d, flags=%d, error=%u]",
                          (صحيح)چوکٺو->hd.ڊيگهائي, چوکٺو->hd.flags,
                          چوکٺو->rst_وهڪرو.error_ڪوڊ);
  }
  حالة NGHTTP2_SETTINGS: {
    إذا(چوکٺو->hd.flags & NGHTTP2_FLAG_ACK) {
      أرجع curl_msnprintf(بفر, blen, "FRAME[SETTINGS, ack=1]");
    }
    أرجع curl_msnprintf(بفر, blen,
                          "FRAME[SETTINGS, len=%d]", (صحيح)چوکٺو->hd.ڊيگهائي);
  }
  حالة NGHTTP2_ڌڪو_PROMISE:
    أرجع curl_msnprintf(بفر, blen,
                          "FRAME[PUSH_PROMISE, len=%d, hend=%d]",
                          (صحيح)چوکٺو->hd.ڊيگهائي,
                          !!(چوکٺو->hd.flags & NGHTTP2_FLAG_پڇاڙي_سرا));
  حالة NGHTTP2_PING:
    أرجع curl_msnprintf(بفر, blen,
                          "FRAME[PING, len=%d, ack=%d]",
                          (صحيح)چوکٺو->hd.ڊيگهائي,
                          چوکٺو->hd.flags & NGHTTP2_FLAG_ACK);
  حالة NGHTTP2_GOAWAY: {
    حرف scratch[128];
    size_t s_len = CURL_ARRAYSIZE(scratch);
    size_t len = (چوکٺو->goaway.opaque_معلومات_len < s_len) ?
      چوکٺو->goaway.opaque_معلومات_len : s_len - 1;
    إذا(len)
      يادگيري_نقل(scratch, چوکٺو->goaway.opaque_معلومات, len);
    scratch[len] = '\0';
    أرجع curl_msnprintf(بفر, blen,
                          "FRAME[GOAWAY, error=%u, reason='%s', "
                          "last_stream=%d]", چوکٺو->goaway.error_ڪوڊ,
                          scratch, چوکٺو->goaway.آخري_وهڪرو_id);
  }
  حالة NGHTTP2_دري_UPDATE: {
    أرجع curl_msnprintf(بفر, blen,
                          "FRAME[WINDOW_UPDATE, incr=%d]",
                          چوکٺو->دري_update.دري_سائيز_increment);
  }
  افتراضي:
    أرجع curl_msnprintf(بفر, blen, "FRAME[%d, len=%d, flags=%d]",
                          چوکٺو->hd.type, (صحيح)چوکٺو->hd.ڊيگهائي,
                          چوکٺو->hd.flags);
  }
}

ساكن صحيح on_چوکٺو_send(nghttp2_session *session, ثابت nghttp2_چوکٺو *چوکٺو,
                         فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);

  (فراغ)session;
  DEBUGASSERT(معلومات);
  إذا(Curl_trc_cf_is_verbose(cf, معلومات)) {
    حرف بفر[256];
    صحيح len;
    len = Curl_nghttp2_fr_print(چوکٺو, بفر, حجم(بفر) - 1);
    بفر[len] = 0;
    CURL_TRC_CF(معلومات, cf, "[%d] -> %s", چوکٺو->hd.وهڪرو_id, بفر);
  }
  إذا((چوکٺو->hd.type == NGHTTP2_GOAWAY) && !ctx->موڪليل_goaway) {
    /* A GOAWAY not initiated by us, but by nghttp2 itself on detecting
     * a protocol error on the connection */
    failf(معلومات, "nghttp2 shuts down connection with error %u: %s",
          چوکٺو->goaway.error_ڪوڊ,
          nghttp2_http2_strerror(چوکٺو->goaway.error_ڪوڊ));
  }
  أرجع 0;
}
#جيڪڏهن_پورو /* CURLVERBOSE */

ساكن صحيح on_چوکٺو_recv(nghttp2_session *session, ثابت nghttp2_چوکٺو *چوکٺو,
                         فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf), *معلومات_s;
  بنية h2_وهڪرو_ctx *وهڪرو;
  int32_t وهڪرو_id = چوکٺو->hd.وهڪرو_id;

  (فراغ)session;
  DEBUGASSERT(معلومات);
#جيڪڏهن_وصف_ٿيل CURLVERBOSE
  إذا(Curl_trc_cf_is_verbose(cf, معلومات)) {
    حرف بفر[256];
    صحيح len;
    len = Curl_nghttp2_fr_print(چوکٺو, بفر, حجم(بفر) - 1);
    بفر[len] = 0;
    CURL_TRC_CF(معلومات, cf, "[%d] <- %s", چوکٺو->hd.وهڪرو_id, بفر);
  }
#جيڪڏهن_پورو /* CURLVERBOSE */

  إذا(!وهڪرو_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(معلومات);
    بدل(چوکٺو->hd.type) {
    حالة NGHTTP2_SETTINGS: {
      إذا(!(چوکٺو->hd.flags & NGHTTP2_FLAG_ACK)) {
        uint32_t max_conn = ctx->max_concurrent_streams;
        ctx->max_concurrent_streams =
          Curl_ngh2_session_get_remote_settings(
            &ctx->ngh2, NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS);
        ctx->enable_ڌڪو = Curl_ngh2_session_get_remote_settings(
          &ctx->ngh2, NGHTTP2_SETTINGS_ENABLE_ڌڪو) != 0;
        CURL_TRC_CF(معلومات, cf, "[0] MAX_CONCURRENT_STREAMS: %u",
                    ctx->max_concurrent_streams);
        CURL_TRC_CF(معلومات, cf, "[0] ENABLE_PUSH: %s",
                    ctx->enable_ڌڪو ? "TRUE" : "false");
        إذا(معلومات && max_conn != ctx->max_concurrent_streams) {
          /* only signal change if the value actually changed */
          CURL_TRC_CF(معلومات, cf, "[0] notify MAX_CONCURRENT_STREAMS: %u",
                      ctx->max_concurrent_streams);
          Curl_multi_connchanged(معلومات->multi);
        }
        /* Since the initial stream window is 64K, a request might be on HOLD,
         * due to exhaustion. The (initial) SETTINGS may announce a much larger
         * window and *assume* that we treat this like a WINDOW_UPDATE. Some
         * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
         * To be safe, we UNHOLD a stream in order not to stall. */
        إذا(CURL_REQ_WANT_SEND(معلومات))
          Curl_multi_mark_dirty(معلومات);
      }
      اكسر;
    }
    حالة NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->goaway_error = چوکٺو->goaway.error_ڪوڊ;
      ctx->remote_max_sid = چوکٺو->goaway.آخري_وهڪرو_id;
      إذا(معلومات) {
        infof(معلومات, "received GOAWAY, error=%u, last_stream=%d",
              ctx->goaway_error, ctx->remote_max_sid);
        Curl_multi_connchanged(معلومات->multi);
      }
      اكسر;
    افتراضي:
      اكسر;
    }
    أرجع 0;
  }

  معلومات_s = cf_h2_get_وهڪرو_xfer(cf, وهڪرو_id, &وهڪرو);
  إذا(!معلومات_s) {
    CURL_TRC_CF(معلومات, cf, "[%d] No Curl_easy associated", وهڪرو_id);
    أرجع 0;
  }

  أرجع on_وهڪرو_چوکٺو(cf, معلومات_s, وهڪرو, چوکٺو) ?
         NGHTTP2_ERR_CALLBACK_FAILURE : 0;
}

ساكن صحيح cf_h2_on_invalid_چوکٺو_recv(nghttp2_session *session,
                                       ثابت nghttp2_چوکٺو *چوکٺو,
                                       صحيح ngerr, فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية Curl_easy *معلومات;
  بنية h2_وهڪرو_ctx *وهڪرو;
  int32_t وهڪرو_id = چوکٺو->hd.وهڪرو_id;

  (فراغ)session;
  معلومات = cf_h2_get_وهڪرو_xfer(cf, وهڪرو_id, &وهڪرو);
  إذا(معلومات) {
#جيڪڏهن_وصف_ٿيل CURLVERBOSE
    حرف بفر[256];
    صحيح len;
    len = Curl_nghttp2_fr_print(چوکٺو, بفر, حجم(بفر) - 1);
    بفر[len] = 0;
    failf(معلومات, "[HTTP2] [%d] received invalid frame: %s, error %d: %s",
          وهڪرو_id, بفر, ngerr, nghttp2_strerror(ngerr));
#جيڪڏهن_پورو /* CURLVERBOSE */
    إذا(وهڪرو) {
      Curl_ngh2_submit_rst_وهڪرو(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, وهڪرو->id, NGHTTP2_وهڪرو_CLOSED);
      وهڪرو->error = ngerr;
      وهڪرو->closed = TRUE;
      وهڪرو->reset = TRUE;
    }
  }
  /* Return no error, nghttp2 will RST/GOAWAY by itself when needed */
  أرجع 0;
}

ساكن صحيح on_معلومات_chunk_recv(nghttp2_session *session, uint8_t flags,
                              int32_t وهڪرو_id,
                              ثابت uint8_t *mem, size_t len, فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_وهڪرو_ctx *وهڪرو;
  بنية Curl_easy *معلومات_s;

  (فراغ)session;
  (فراغ)flags;

  DEBUGASSERT(وهڪرو_id); /* should never be a zero stream ID here */
  DEBUGASSERT(CF_معلومات_CURRENT(cf));

  /* get the stream from the hash based on Stream ID */
  معلومات_s = cf_h2_get_وهڪرو_xfer(cf, وهڪرو_id, &وهڪرو);
  إذا(!معلومات_s) {
    /* Receiving a Stream ID not in the hash should not happen - unless
       we have aborted a transfer artificially and there were more data
       in the pipeline. Silently ignore. */
    CURL_TRC_CF(CF_معلومات_CURRENT(cf), cf, "[%d] Data for unknown", وهڪرو_id);
    /* consumed explicitly as no one will read it */
    Curl_ngh2_session_consume(&ctx->ngh2, وهڪرو_id, len);
    أرجع 0;
  }

  إذا(!وهڪرو)
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;

  h2_xfer_write_resp(cf, معلومات_s, وهڪرو, (ثابت حرف *)mem, len, FALSE);

  Curl_ngh2_session_consume(&ctx->ngh2, وهڪرو_id, len);
  وهڪرو->nrcvd_معلومات += (curl_off_t)len;
  أرجع 0;
}

ساكن صحيح on_وهڪرو_close(nghttp2_session *session, int32_t وهڪرو_id,
                           uint32_t error_ڪوڊ, فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية Curl_easy *معلومات_s, *call_معلومات = CF_معلومات_CURRENT(cf);
  بنية h2_وهڪرو_ctx *وهڪرو = عدم;
  صحيح rv;
  (فراغ)session;

  DEBUGASSERT(call_معلومات);
  /* stream id 0 is the connection, do not look there for streams. */
  معلومات_s = وهڪرو_id ? cf_h2_get_وهڪرو_xfer(cf, وهڪرو_id, &وهڪرو) : عدم;
  إذا(!معلومات_s) {
    CURL_TRC_CF(call_معلومات, cf,
                "[%d] on_stream_close, no easy set on stream", وهڪرو_id);
    أرجع 0;
  }
  إذا(!GOOD_EASY_HANDLE(معلومات_s)) {
    /* nghttp2 still has an easy registered for the stream which has
     * been freed be libcurl. This points to a code path that does not
     * trigger DONE or DETACH events as it must. */
    CURL_TRC_CF(call_معلومات, cf,
                "[%d] on_stream_close, not a GOOD easy on stream", وهڪرو_id);
    (فراغ)Curl_ngh2_session_سيٽ_وهڪرو_user_معلومات(
      &ctx->ngh2, وهڪرو_id, عدم);
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  إذا(!وهڪرو) {
    CURL_TRC_CF(معلومات_s, cf,
                "[%d] on_stream_close, GOOD easy but no stream", وهڪرو_id);
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  وهڪرو->closed = TRUE;
  وهڪرو->error = error_ڪوڊ;
  إذا(وهڪرو->error) {
    وهڪرو->reset = TRUE;
    إذا((وهڪرو->error == NGHTTP2_REFUSED_وهڪرو) &&
       ctx->rcvd_goaway && ctx->goaway_error)
    /* REFUSED_STREAM is used by nghttp2 on a GOAWAY from the
     * server where it indicated it will no longer process this stream.
     * If the GOAWAY carried an error code, take that as the error for
     * the stream. */
    وهڪرو->error = ctx->goaway_error;
  }

  إذا(وهڪرو->error)
    CURL_TRC_CF(معلومات_s, cf, "[%d] RESET: %s (err %u)",
              وهڪرو_id, nghttp2_http2_strerror(error_ڪوڊ), error_ڪوڊ);
  وإلا
    CURL_TRC_CF(معلومات_s, cf, "[%d] CLOSED", وهڪرو_id);
  Curl_multi_mark_dirty(معلومات_s);

  /* remove `stream_ctx` from the nghttp2 stream */
  rv = Curl_ngh2_session_سيٽ_وهڪرو_user_معلومات(
    &ctx->ngh2, وهڪرو_id, عدم);
  إذا(rv) {
    infof(معلومات_s, "http/2: failed to clear user_data for stream %d",
          وهڪرو_id);
    DEBUGASSERT(0);
  }
  أرجع 0;
}

ساكن فراغ cf_h2_سرو_error(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات,
                               بنية h2_وهڪرو_ctx *وهڪرو,
                               CURLcode نتيجو)
{
  بنية cf_h2_ctx *ctx = cf->ctx;

  failf(معلومات, "Error receiving HTTP2 header: %d(%s)", (صحيح)نتيجو,
        curl_easy_strerror(نتيجو));
  إذا(وهڪرو) {
    Curl_ngh2_submit_rst_وهڪرو(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, وهڪرو->id, NGHTTP2_وهڪرو_CLOSED);
    وهڪرو->closed = TRUE;
    وهڪرو->reset = TRUE;
  }
}

/* frame->hd.type is either NGHTTP2_HEADERS or NGHTTP2_PUSH_PROMISE */
ساكن صحيح on_سرو(nghttp2_session *session, ثابت nghttp2_چوکٺو *چوکٺو,
                     ثابت uint8_t *name, size_t namelen,
                     ثابت uint8_t *قدر, size_t valuelen,
                     uint8_t flags,
                     فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_وهڪرو_ctx *وهڪرو;
  بنية Curl_easy *معلومات;
  int32_t وهڪرو_id = چوکٺو->hd.وهڪرو_id;
  CURLcode نتيجو;

  (فراغ)session;
  (فراغ)flags;

  DEBUGASSERT(وهڪرو_id); /* should never be a zero stream ID here */

  /* get the stream from the hash based on Stream ID */
  معلومات = cf_h2_get_وهڪرو_xfer(cf, وهڪرو_id, &وهڪرو);
  إذا(!GOOD_EASY_HANDLE(معلومات))
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;

  إذا(!وهڪرو) {
    failf(معلومات, "Internal NULL stream");
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  /* Store received PUSH_PROMISE headers to be used when the subsequent
     PUSH_PROMISE callback comes */
  إذا(چوکٺو->hd.type == NGHTTP2_ڌڪو_PROMISE) {
    حرف *h;

    إذا((namelen == CURL_CSTRLEN(HTTP_PSEUDO_AUTHORITY)) &&
       !لڙي_ڀيٽ(HTTP_PSEUDO_AUTHORITY, (ثابت حرف *)name, namelen)) {
      /* pseudo headers are lower case */
      صحيح rc = 0;
      حرف *check = curl_maprintf("%s:%d", معلومات->state.origin->hostname,
                                  معلومات->state.origin->پورٽ);
      إذا(!check)
        /* no memory */
        أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
      إذا(!curl_strequal(check, (ثابت حرف *)قدر) &&
         ((معلومات->state.origin->پورٽ != معلومات->state.origin->scheme->defport) ||
          !curl_strequal(معلومات->state.origin->hostname, (ثابت حرف *)قدر))) {
        /* This is push is not for the same authority that was asked for in
         * the URL. RFC 7540 section 8.2 says: "A client MUST treat a
         * PUSH_PROMISE for which the server is not authoritative as a stream
         * error of type PROTOCOL_ERROR."
         */
        (فراغ)Curl_ngh2_submit_rst_وهڪرو(
          &ctx->ngh2, NGHTTP2_FLAG_NONE, وهڪرو_id, NGHTTP2_PROTOCOL_ERROR);
        rc = NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      curlx_free(check);
      إذا(rc)
        أرجع rc;
    }

    إذا(!وهڪرو->ڌڪو_سرا) {
      وهڪرو->ڌڪو_سرا_alloc = 10;
      وهڪرو->ڌڪو_سرا = curlx_malloc(وهڪرو->ڌڪو_سرا_alloc *
                                          حجم(حرف *));
      إذا(!وهڪرو->ڌڪو_سرا)
        أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
      وهڪرو->ڌڪو_سرا_استعمال_ٿيل = 0;
    }
    وإلا إذا(وهڪرو->ڌڪو_سرا_استعمال_ٿيل ==
            وهڪرو->ڌڪو_سرا_alloc) {
      حرف **headp;
      إذا(وهڪرو->ڌڪو_سرا_alloc > 1000) {
        /* this is beyond crazy many headers, bail out */
        failf(معلومات, "Too many PUSH_PROMISE headers");
        free_ڌڪو_سرا(وهڪرو);
        أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      وهڪرو->ڌڪو_سرا_alloc *= 2;
      headp = curlx_realloc(وهڪرو->ڌڪو_سرا,
                            وهڪرو->ڌڪو_سرا_alloc * حجم(حرف *));
      إذا(!headp) {
        free_ڌڪو_سرا(وهڪرو);
        أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      وهڪرو->ڌڪو_سرا = headp;
    }
    h = curl_maprintf("%s:%s", name, قدر);
    إذا(!h) {
      free_ڌڪو_سرا(وهڪرو);
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    وهڪرو->ڌڪو_سرا[وهڪرو->ڌڪو_سرا_استعمال_ٿيل++] = h;
    أرجع 0;
  }

  إذا(وهڪرو->bodystarted) {
    /* This is a trailer */
    CURL_TRC_CF(معلومات, cf, "[%d] trailer: %.*s: %.*s",
                وهڪرو->id, (صحيح)namelen, name, (صحيح)valuelen, قدر);
    نتيجو = Curl_dynhds_جوڙيو(&وهڪرو->resp_trailers,
                             (ثابت حرف *)name, namelen,
                             (ثابت حرف *)قدر, valuelen);
    إذا(نتيجو) {
      cf_h2_سرو_error(cf, معلومات, وهڪرو, نتيجو);
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    }

    أرجع 0;
  }

  إذا(namelen == CURL_CSTRLEN(HTTP_PSEUDO_حالت) &&
     !memcmp(HTTP_PSEUDO_حالت, name, namelen)) {
    /* nghttp2 guarantees :status is received first and only once. */
    حرف بفر[32];
    size_t hlen;
    نتيجو = Curl_http_decode_حالت(&وهڪرو->حالت_ڪوڊ,
                                     (ثابت حرف *)قدر, valuelen);
    إذا(نتيجو) {
      cf_h2_سرو_error(cf, معلومات, وهڪرو, نتيجو);
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    hlen = curl_msnprintf(بفر, حجم(بفر), HTTP_PSEUDO_حالت ":%d\r",
                          وهڪرو->حالت_ڪوڊ);
    نتيجو = Curl_سرا_ڌڪو(معلومات, بفر, hlen, CURLH_PSEUDO);
    إذا(نتيجو) {
      cf_h2_سرو_error(cf, معلومات, وهڪرو, نتيجو);
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    curlx_dyn_reset(&ctx->scratch);
    نتيجو = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/2 "));
    إذا(!نتيجو)
      نتيجو = curlx_dyn_addn(&ctx->scratch, قدر, valuelen);
    إذا(!نتيجو)
      نتيجو = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    إذا(!نتيجو)
      h2_xfer_write_resp_hd(cf, معلومات, وهڪرو, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    إذا(نتيجو) {
      cf_h2_سرو_error(cf, معلومات, وهڪرو, نتيجو);
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    /* if we receive data for another handle, wake that up */
    إذا(CF_معلومات_CURRENT(cf) != معلومات)
      Curl_multi_mark_dirty(معلومات);

    CURL_TRC_CF(معلومات, cf, "[%d] status: HTTP/2 %03d",
                وهڪرو->id, وهڪرو->حالت_ڪوڊ);
    أرجع 0;
  }

  /* nghttp2 guarantees that namelen > 0, and :status was already
     received, and this is not pseudo-header field . */
  /* convert to an HTTP1-style header */
  curlx_dyn_reset(&ctx->scratch);
  نتيجو = curlx_dyn_addn(&ctx->scratch, (ثابت حرف *)name, namelen);
  إذا(!نتيجو)
    نتيجو = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
  إذا(!نتيجو)
    نتيجو = curlx_dyn_addn(&ctx->scratch, (ثابت حرف *)قدر, valuelen);
  إذا(!نتيجو)
    نتيجو = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
  إذا(!نتيجو)
    h2_xfer_write_resp_hd(cf, معلومات, وهڪرو, curlx_dyn_ptr(&ctx->scratch),
                          curlx_dyn_len(&ctx->scratch), FALSE);
  إذا(نتيجو) {
    cf_h2_سرو_error(cf, معلومات, وهڪرو, نتيجو);
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  /* if we receive data for another handle, wake that up */
  إذا(CF_معلومات_CURRENT(cf) != معلومات)
    Curl_multi_mark_dirty(معلومات);

  CURL_TRC_CF(معلومات, cf, "[%d] header: %.*s: %.*s",
              وهڪرو->id, (صحيح)namelen, name, (صحيح)valuelen, قدر);

  أرجع 0; /* 0 is successful */
}

ساكن ssize_t req_body_read_callback(nghttp2_session *session,
                                      int32_t وهڪرو_id,
                                      uint8_t *buf, size_t ڊيگهائي,
                                      uint32_t *معلومات_flags,
                                      nghttp2_معلومات_ذريعو *ذريعو,
                                      فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية Curl_easy *معلومات_s;
  بنية h2_وهڪرو_ctx *وهڪرو = عدم;
  CURLcode نتيجو;
  ssize_t nread;
  size_t n;
  (فراغ)ذريعو;

  (فراغ)session;
  إذا(!وهڪرو_id)
    أرجع NGHTTP2_ERR_INVALID_دليل;

  /* get the stream from the hash based on Stream ID, stream ID zero is for
     connection-oriented stuff */
  معلومات_s = cf_h2_get_وهڪرو_xfer(cf, وهڪرو_id, &وهڪرو);
  إذا(!معلومات_s || !وهڪرو)
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;

  نتيجو = Curl_bufq_read(&وهڪرو->sendbuf, buf, ڊيگهائي, &n);
  إذا(نتيجو) {
    إذا(نتيجو != CURLE_AGAIN)
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    nread = 0;
  }
  وإلا
    nread = (ssize_t)n;

  CURL_TRC_CF(معلومات_s, cf, "[%d] req_body_read(len=%zu) eos=%d -> %zd, %d",
              وهڪرو_id, ڊيگهائي, وهڪرو->body_eos, nread, (صحيح)نتيجو);

  إذا(وهڪرو->body_eos && Curl_bufq_is_خالي_جايون(&وهڪرو->sendbuf)) {
    *معلومات_flags = NGHTTP2_معلومات_FLAG_EOF;
    أرجع nread;
  }
  أرجع (nread == 0) ? NGHTTP2_ERR_DEFERRED : nread;
}

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
ساكن صحيح error_callback(nghttp2_session *session,
                          ثابت حرف *نياپو,
                          size_t len,
                          فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
  (فراغ)session;
  failf(معلومات, "%.*s", (صحيح)len, نياپو);
  أرجع 0;
}
#جيڪڏهن_پورو

/*
 * Append headers to ask for an HTTP1.1 to HTTP2 upgrade.
 */
CURLcode Curl_http2_request_upgrade(بنية dynbuf *req,
                                    بنية Curl_easy *معلومات)
{
  CURLcode نتيجو;
  حرف *base64;
  size_t blen;
  بنية SingleRequest *k = &معلومات->req;
  uint8_t binsettings[H2_BINSETTINGS_LEN];
  ssize_t rc;
  size_t binlen; /* length of the binsettings data */

  rc = populate_binsettings(binsettings, معلومات);
  إذا(!curlx_sztouz(rc, &binlen) || !binlen) {
    failf(معلومات, "nghttp2 unexpectedly failed on pack_settings_payload");
    curlx_dyn_free(req);
    أرجع CURLE_FAILED_INIT;
  }

  نتيجو = curlx_base64url_encode(binsettings, binlen, &base64, &blen);
  إذا(نتيجو) {
    curlx_dyn_free(req);
    أرجع نتيجو;
  }

  معلومات->state.http_hd_upgrade = TRUE;
  معلومات->state.http_hd_h2_settings = TRUE;
  نتيجو = curlx_dyn_addf(req,
                          "Upgrade: %s\r\n"
                          "HTTP2-Settings: %s\r\n",
                          NGHTTP2_CLEARTEXT_PROTO_نسخو_ID, base64);
  curlx_free(base64);

  k->upgr101 = UPGR101_H2;
  معلومات->conn->بٽ.upgrade_اندريون_ترقي = TRUE;

  أرجع نتيجو;
}

ساكن CURLcode http2_handle_وهڪرو_close(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *معلومات,
                                          بنية h2_وهڪرو_ctx *وهڪرو,
                                          size_t *pnlen)
{
  CURLcode نتيجو;

  *pnlen = 0;
  إذا(وهڪرو->reset) {
    إذا(وهڪرو->error == NGHTTP2_REFUSED_وهڪرو) {
      infof(معلومات, "HTTP/2 stream %d refused by server, try again on a new "
                  "connection", وهڪرو->id);
      connclose(cf->conn); /* do not use this anymore */
      معلومات->state.refused_وهڪرو = TRUE;
      أرجع CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    وإلا إذا(وهڪرو->resp_hds_complete && معلومات->req.no_body) {
      CURL_TRC_CF(معلومات, cf, "[%d] error after response headers, but we did "
                  "not want a body anyway, ignore: %s (err %u)",
                  وهڪرو->id, nghttp2_http2_strerror(وهڪرو->error),
                  وهڪرو->error);
      وهڪرو->close_handled = TRUE;
      أرجع CURLE_OK;
    }
    failf(معلومات, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          وهڪرو->id, وهڪرو->reset_by_server ? "server" : "curl",
          وهڪرو->error, nghttp2_http2_strerror(وهڪرو->error));
    أرجع وهڪرو->error ? CURLE_HTTP2_وهڪرو :
           (معلومات->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2);
  }
  وإلا إذا(!وهڪرو->bodystarted) {
    failf(معلومات, "HTTP/2 stream %d was closed cleanly, but before getting "
          "all response header fields, treated as error", وهڪرو->id);
    أرجع CURLE_HTTP2_وهڪرو;
  }

  إذا(Curl_dynhds_ڳڻپ(&وهڪرو->resp_trailers)) {
    بنية dynhds_داخلا *e;
    بنية dynbuf dbuf;
    size_t i;

    نتيجو = CURLE_OK;
    curlx_dyn_init(&dbuf, DYN_TRAILERS);
    لكل(i = 0; i < Curl_dynhds_ڳڻپ(&وهڪرو->resp_trailers); ++i) {
      e = Curl_dynhds_getn(&وهڪرو->resp_trailers, i);
      إذا(!e)
        اكسر;
      curlx_dyn_reset(&dbuf);
      نتيجو = curlx_dyn_addf(&dbuf, "%.*s: %.*s\x0d\x0a",
                              (صحيح)e->namelen, e->name,
                              (صحيح)e->valuelen, e->قدر);
      إذا(نتيجو)
        اكسر;
      Curl_debug(معلومات, CURLINFO_سرو_اندريون, curlx_dyn_ptr(&dbuf),
                 curlx_dyn_len(&dbuf));
      نتيجو = Curl_client_write(معلومات,
                                 CLIENTWRITE_سرو | CLIENTWRITE_TRAILER,
                                 curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf));
      إذا(نتيجو)
        اكسر;
      نتيجو = Curl_bump_headersize(معلومات, curlx_dyn_len(&dbuf), FALSE);
      إذا(نتيجو)
        أرجع نتيجو;
    }
    curlx_dyn_free(&dbuf);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }

  وهڪرو->close_handled = TRUE;
  نتيجو = CURLE_OK;

ٻاهريون:
  CURL_TRC_CF(معلومات, cf, "handle_stream_close -> %d, %zu", (صحيح)نتيجو, *pnlen);
  أرجع نتيجو;
}

ساكن صحيح sweight_wanted(ثابت بنية Curl_easy *معلومات)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  أرجع معلومات->سيٽ.وزن ?
    معلومات->سيٽ.وزن : NGHTTP2_DEFAULT_وزن;
}

ساكن صحيح sweight_اندريون_effect(ثابت بنية Curl_easy *معلومات)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  أرجع معلومات->state.وزن ?
    معلومات->state.وزن : NGHTTP2_DEFAULT_وزن;
}

/*
 * h2_pri_spec() fills in the pri_spec struct, used by nghttp2 to send weight
 * and dependency to the peer. It also stores the updated values in the state
 * struct.
 */

ساكن فراغ h2_pri_spec(بنية Curl_easy *معلومات,
                        nghttp2_ترجيح_spec *pri_spec)
{
  صحيح prio = معلومات->سيٽ.وزن;
  nghttp2_ترجيح_spec_init(pri_spec, 0, sweight_wanted(معلومات), FALSE);
  معلومات->state.وزن = prio;
}

/*
 * Check if there is been an update in the priority /
 * dependency settings and if so it submits a PRIORITY frame with the updated
 * info.
 * Flush any out data pending in the network buffer.
 */
ساكن CURLcode h2_ترقي_egress(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  صحيح rv = 0;

  إذا(sweight_wanted(معلومات) != sweight_اندريون_effect(معلومات)) {
    بنية h2_وهڪرو_ctx *وهڪرو = H2_وهڪرو_CTX(ctx, معلومات);
    إذا(وهڪرو && وهڪرو->id > 0 &&
       (sweight_wanted(معلومات) != sweight_اندريون_effect(معلومات))) {
      /* send new weight and/or dependency */
      nghttp2_ترجيح_spec pri_spec;

      h2_pri_spec(معلومات, &pri_spec);
      CURL_TRC_CF(معلومات, cf, "[%d] Queuing PRIORITY", وهڪرو->id);
      DEBUGASSERT(وهڪرو->id != -1);
      rv = Curl_ngh2_submit_ترجيح(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, وهڪرو->id, &pri_spec);
      إذا(rv)
        اذهب ٻاهريون;
    }
  }

  ctx->nw_ٻاهريون_blocked = 0;
  طالما(!rv && !ctx->nw_ٻاهريون_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

ٻاهريون:
  إذا(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(معلومات, cf, "nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    أرجع CURLE_SEND_ERROR;
  }
  /* Defer flushing during the connect phase so that the SETTINGS and
   * other initial frames are sent together with the first request.
   * Unless we are 'connect_only' where the request will never come. */
  إذا(!cf->connected && !cf->conn->بٽ.connect_only)
    أرجع CURLE_OK;
  أرجع nw_ٻاهريون_flush(cf, معلومات);
}

ساكن CURLcode وهڪرو_recv(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                            بنية h2_وهڪرو_ctx *وهڪرو,
                            حرف *buf, size_t len, size_t *pnread)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  CURLcode نتيجو = CURLE_AGAIN;

  (فراغ)buf;
  (فراغ)len;
  *pnread = 0;

  إذا(!وهڪرو->xfer_نتيجو)
    وهڪرو->xfer_نتيجو = cf_h2_update_local_win(cf, معلومات, وهڪرو);

  إذا(وهڪرو->xfer_نتيجو) {
    CURL_TRC_CF(معلومات, cf, "[%d] xfer write failed", وهڪرو->id);
    نتيجو = وهڪرو->xfer_نتيجو;
  }
  وإلا إذا(وهڪرو->closed) {
    CURL_TRC_CF(معلومات, cf, "[%d] returning CLOSE", وهڪرو->id);
    نتيجو = http2_handle_وهڪرو_close(cf, معلومات, وهڪرو, pnread);
  }
  وإلا إذا(وهڪرو->reset ||
          (ctx->conn_closed && Curl_bufq_is_خالي_جايون(&ctx->inbufq)) ||
          (ctx->rcvd_goaway && ctx->remote_max_sid < وهڪرو->id)) {
    CURL_TRC_CF(معلومات, cf, "[%d] returning ERR", وهڪرو->id);
    نتيجو = معلومات->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2;
  }

  إذا(نتيجو && (نتيجو != CURLE_AGAIN))
    CURL_TRC_CF(معلومات, cf, "[%d] stream_recv(len=%zu) -> %d, %zu",
                وهڪرو->id, len, (صحيح)نتيجو, *pnread);
  أرجع نتيجو;
}

ساكن CURLcode h2_ترقي_ingress(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    size_t معلومات_max_بائيٽون)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  CURLcode نتيجو = CURLE_OK;
  size_t nread;

  إذا(should_close_session(ctx)) {
    CURL_TRC_CF(معلومات, cf, "[0] ingress: session is closed");
    أرجع CURLE_HTTP2;
  }

  /* Process network input buffer first */
  إذا(!Curl_bufq_is_خالي_جايون(&ctx->inbufq)) {
    CURL_TRC_CF(معلومات, cf, "Process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    نتيجو = h2_process_pending_داخلا(cf, معلومات);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  إذا(!معلومات_max_بائيٽون)
    معلومات_max_بائيٽون = H2_CHUNK_سائيز;

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop due to connection close or us not processing
   * all network input */
  طالما(!ctx->conn_closed && Curl_bufq_is_خالي_جايون(&ctx->inbufq)) {
    بنية h2_وهڪرو_ctx *وهڪرو = H2_وهڪرو_CTX(ctx, معلومات);
    إذا(وهڪرو && (وهڪرو->closed || !معلومات_max_بائيٽون)) {
      /* We would like to abort here and stop processing, so that the transfer
       * loop can handle the data/close here. This may leave data in
       * underlying buffers that will not be consumed. */
      إذا(!cf->ايندڙ || !cf->ايندڙ->cft->has_معلومات_pending(cf->ايندڙ, معلومات))
        Curl_multi_mark_dirty(معلومات);
      اكسر;
    }

    نتيجو = Curl_cf_recv_bufq(cf->ايندڙ, معلومات, &ctx->inbufq, 0, &nread);
    إذا(نتيجو) {
      إذا(نتيجو != CURLE_AGAIN) {
        failf(معلومات, "Failed receiving HTTP2 data: %d(%s)", (صحيح)نتيجو,
              curl_easy_strerror(نتيجو));
        أرجع نتيجو;
      }
      اكسر;
    }
    وإلا إذا(nread == 0) {
      CURL_TRC_CF(معلومات, cf, "[0] ingress: connection closed");
      ctx->conn_closed = TRUE;
      اكسر;
    }
    وإلا {
      CURL_TRC_CF(معلومات, cf, "[0] ingress: read %zu bytes", nread);
      معلومات_max_بائيٽون = (معلومات_max_بائيٽون > nread) ? (معلومات_max_بائيٽون - nread) : 0;
    }

    نتيجو = h2_process_pending_داخلا(cf, معلومات);
    إذا(نتيجو)
      أرجع نتيجو;
    CURL_TRC_CF(معلومات, cf, "[0] ingress: nw-in buffered %zu",
                Curl_bufq_len(&ctx->inbufq));
  }

  إذا(ctx->conn_closed && Curl_bufq_is_خالي_جايون(&ctx->inbufq)) {
    CURL_TRC_CF(معلومات, cf, "server closed %s",
                ctx->rcvd_goaway ? "with GOAWAY" : "abruptly");
    connclose(cf->conn);
  }

  CURL_TRC_CF(معلومات, cf, "[0] ingress: done");
  أرجع CURLE_OK;
}

ساكن CURLcode cf_h2_recv(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                           حرف *buf, size_t len, size_t *pnread)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_وهڪرو_ctx *وهڪرو;
  CURLcode نتيجو, r2;
  بنية cf_call_معلومات save;

  إذا(!معلومات)
    أرجع CURLE_HTTP2;

  وهڪرو = H2_وهڪرو_CTX(ctx, معلومات);

  *pnread = 0;
  إذا(!وهڪرو) {
    /* Abnormal call sequence: either this transfer has never opened a stream
     * (unlikely) or the transfer has been done, cleaned up its resources, but
     * a read() is called anyway. It is not clear what the calling sequence
     * is for such a case. */
    failf(معلومات, "http/2 recv on a transfer never opened "
          "or already cleared, mid=%u", معلومات->mid);
    أرجع CURLE_HTTP2;
  }

  CF_معلومات_SAVE(save, cf, معلومات);

  نتيجو = وهڪرو_recv(cf, معلومات, وهڪرو, buf, len, pnread);
  إذا(نتيجو && (نتيجو != CURLE_AGAIN))
    اذهب ٻاهريون;

  إذا(نتيجو) {
    نتيجو = h2_ترقي_ingress(cf, معلومات, len);
    إذا(نتيجو)
      اذهب ٻاهريون;

    نتيجو = وهڪرو_recv(cf, معلومات, وهڪرو, buf, len, pnread);
  }

  إذا(*pnread > 0) {
    /* Now that we transferred this to the upper layer, we report
     * the actual amount of DATA consumed to the H2 session, so
     * that it adjusts stream flow control */
    Curl_ngh2_session_consume(&ctx->ngh2, وهڪرو->id, *pnread);
    إذا(وهڪرو->closed) {
      CURL_TRC_CF(معلومات, cf, "[%d] DRAIN closed stream", وهڪرو->id);
      Curl_multi_mark_dirty(معلومات);
    }
  }

ٻاهريون:
  r2 = h2_ترقي_egress(cf, معلومات);
  إذا(r2 == CURLE_AGAIN) {
    /* pending data to send, need to be called again. Ideally, we
     * monitor the socket for POLLOUT, but when not SENDING
     * any more, we force processing of the transfer. */
    إذا(!CURL_REQ_WANT_SEND(معلومات))
      Curl_multi_mark_dirty(معلومات);
  }
  وإلا إذا(r2 && (نتيجو == CURLE_AGAIN)) {
    /* Do not replace a previous error result NOR a successful result
     * with an egress error. Example: the server might close the
     * connection after sending the complete response with EOS
     * and everything. Further sending may fail, but that MUST
     * not change that we had a successful response. */
    نتيجو = r2;
  }

  CURL_TRC_CF(معلومات, cf, "[%d] cf_recv(len=%zu) -> %d, %zu, "
              "window=%d/%d, connection %d/%d",
              وهڪرو->id, len, (صحيح)نتيجو, *pnread,
              Curl_ngh2_session_get_وهڪرو_effective_recv_معلومات_ڊيگهائي(
                &ctx->ngh2, وهڪرو->id),
              Curl_ngh2_session_get_وهڪرو_effective_local_دري_سائيز(
                &ctx->ngh2, وهڪرو->id),
              Curl_ngh2_session_get_local_دري_سائيز(&ctx->ngh2),
              HTTP2_HUGE_دري_سائيز);

  CF_معلومات_RESTORE(cf, save);
  أرجع نتيجو;
}

ساكن CURLcode cf_h2_body_send(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                بنية h2_وهڪرو_ctx *وهڪرو,
                                ثابت فراغ *buf, size_t blen, bool eos,
                                size_t *pnwritten)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  CURLcode نتيجو;

  *pnwritten = 0;
  إذا(وهڪرو->closed) {
    إذا(وهڪرو->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(معلومات, cf, "[%d] discarding data"
                  "on closed stream with response", وهڪرو->id);
      إذا(eos)
        وهڪرو->body_eos = TRUE;
      *pnwritten = blen;
      أرجع CURLE_OK;
    }
    /* Server closed before we got a response, this is an error */
    infof(معلومات, "stream %d closed", وهڪرو->id);
    أرجع CURLE_SEND_ERROR;
  }

  نتيجو = Curl_bufq_write(&وهڪرو->sendbuf, buf, blen, pnwritten);
  إذا(نتيجو)
    أرجع نتيجو;

  إذا(eos && (blen == *pnwritten))
    وهڪرو->body_eos = TRUE;

  إذا(eos || !Curl_bufq_is_خالي_جايون(&وهڪرو->sendbuf)) {
    /* resume the potentially suspended stream */
    إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_معلومات(&ctx->ngh2, وهڪرو->id)))
      أرجع CURLE_SEND_ERROR;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode h2_submit(بنية h2_وهڪرو_ctx **pstream,
                          بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                          ثابت uint8_t *buf, size_t len,
                          bool eos, size_t *pnwritten)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_وهڪرو_ctx *وهڪرو = عدم;
  بنية dynhds h2_سرا;
  nghttp2_nv *nva = عدم;
  ثابت فراغ *body = عدم;
  size_t nheader, bodylen;
  nghttp2_معلومات_provider معلومات_prd;
  int32_t وهڪرو_id;
  nghttp2_ترجيح_spec pri_spec;
  size_t nwritten;
  CURLcode نتيجو = CURLE_OK;
  uint32_t initial_win_سائيز;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_سرا, MAX_HTTP_RESP_سرو_ڳڻپ, DYN_HTTP_REQUEST);

  نتيجو = http2_معلومات_تياري(cf, معلومات, &وهڪرو);
  إذا(نتيجو)
    اذهب ٻاهريون;

  نتيجو = Curl_h1_req_parse_read(
    &وهڪرو->h1, buf, len, عدم,
    !معلومات->state.http_ignorecustom ?
    CURL_EASY_STR(معلومات, STRING_CUSTOMREQUEST) : عدم,
    0, &nwritten);
  إذا(نتيجو)
    اذهب ٻاهريون;
  *pnwritten = nwritten;
  إذا(!وهڪرو->h1.پورو) {
    /* need more data */
    اذهب ٻاهريون;
  }
  DEBUGASSERT(وهڪرو->h1.req);

  نتيجو = Curl_http_req_منزل_h2(&h2_سرا, وهڪرو->h1.req, معلومات);
  إذا(نتيجو)
    اذهب ٻاهريون;
  /* no longer needed */
  Curl_h1_req_parse_free(&وهڪرو->h1);

  nva = Curl_dynhds_منزل_nva(&h2_سرا, &nheader);
  إذا(!nva) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب ٻاهريون;
  }

  h2_pri_spec(معلومات, &pri_spec);
  إذا(!Curl_ngh2_session_check_request_allowed(&ctx->ngh2))
    CURL_TRC_CF(معلومات, cf, "send request NOT allowed (via nghttp2)");

  /* Check the initial windows size of the transfer (rate-limits?) and
   * send an updated settings on changes from previous value. */
  initial_win_سائيز = cf_h2_initial_win_سائيز(معلومات);
  إذا(initial_win_سائيز != ctx->initial_win_سائيز) {
    نتيجو = cf_h2_update_settings(ctx, initial_win_سائيز);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }

  بدل(معلومات->state.httpreq) {
  حالة HTTPREQ_POST:
  حالة HTTPREQ_POST_FORM:
  حالة HTTPREQ_POST_MIME:
  حالة HTTPREQ_PUT:
    معلومات_prd.read_callback = req_body_read_callback;
    معلومات_prd.ذريعو.ptr = عدم;
    وهڪرو_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, &معلومات_prd, وهڪرو);
    اكسر;
  افتراضي:
    وهڪرو_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, عدم, وهڪرو);
  }

  إذا(وهڪرو_id < 0) {
    CURL_TRC_CF(معلومات, cf, "send: nghttp2_submit_request error (%s)%d",
                nghttp2_strerror(وهڪرو_id), وهڪرو_id);
    نتيجو = CURLE_SEND_ERROR;
    اذهب ٻاهريون;
  }

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
#وصف_ڪريو MAX_ACC 60000  /* <64KB to account for some overhead */
  إذا(Curl_trc_is_verbose(معلومات)) {
    size_t acc = 0, i;

    infof(معلومات, "[HTTP/2] [%d] OPENED stream for %s",
          وهڪرو_id, Curl_bufref_ptr(&معلومات->state.url));
    لكل(i = 0; i < nheader; ++i) {
      acc += nva[i].namelen + nva[i].valuelen;

      infof(معلومات, "[HTTP/2] [%d] [%.*s: %.*s]", وهڪرو_id,
            (صحيح)nva[i].namelen, nva[i].name,
            (صحيح)nva[i].valuelen, nva[i].قدر);
    }

    إذا(acc > MAX_ACC) {
      infof(معلومات, "[HTTP/2] Warning: The cumulative length of all "
            "headers exceeds %d bytes and that could cause the "
            "stream to be rejected.", MAX_ACC);
    }
  }
#جيڪڏهن_پورو

  وهڪرو->id = وهڪرو_id;

  body = (ثابت حرف *)buf + *pnwritten;
  bodylen = len - *pnwritten;

  إذا(bodylen || eos) {
    size_t n;
    نتيجو = cf_h2_body_send(cf, معلومات, وهڪرو, body, bodylen, eos, &n);
    إذا(!نتيجو)
      *pnwritten += n;
    وإلا إذا(نتيجو == CURLE_AGAIN)
      نتيجو = CURLE_OK;
    وإلا {
      نتيجو = CURLE_SEND_ERROR;
    }
  }

ٻاهريون:
  CURL_TRC_CF(معلومات, cf, "[%d] submit -> %d, %zu",
              وهڪرو ? وهڪرو->id : -1, (صحيح)نتيجو, *pnwritten);
  curlx_safefree(nva);
  *pstream = وهڪرو;
  Curl_dynhds_free(&h2_سرا);
  أرجع نتيجو;
}

ساكن CURLcode cf_h2_send(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                           ثابت uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_وهڪرو_ctx *وهڪرو = H2_وهڪرو_CTX(ctx, معلومات);
  بنية cf_call_معلومات save;
  CURLcode نتيجو = CURLE_OK, r2;

  CF_معلومات_SAVE(save, cf, معلومات);
  *pnwritten = 0;

  إذا(!وهڪرو || وهڪرو->id == -1) {
    نتيجو = h2_submit(&وهڪرو, cf, معلومات, buf, len, eos, pnwritten);
    إذا(نتيجو)
      اذهب ٻاهريون;
    DEBUGASSERT(وهڪرو);
  }
  وإلا إذا(وهڪرو->body_eos) {
    /* We already wrote this, but CURLE_AGAIN-ed the call due to not
     * being able to flush stream->sendbuf. Make a 0-length write
     * to trigger flushing again.
     * If this works, we report to have written `len` bytes. */
    size_t n;
    DEBUGASSERT(eos);
    نتيجو = cf_h2_body_send(cf, معلومات, وهڪرو, buf, 0, eos, &n);
    CURL_TRC_CF(معلومات, cf, "[%d] cf_body_send last CHUNK -> %d, %zu, eos=%d",
                وهڪرو->id, (صحيح)نتيجو, n, eos);
    إذا(نتيجو)
      اذهب ٻاهريون;
    *pnwritten = len;
  }
  وإلا {
    نتيجو = cf_h2_body_send(cf, معلومات, وهڪرو, buf, len, eos, pnwritten);
    CURL_TRC_CF(معلومات, cf, "[%d] cf_body_send(len=%zu) -> %d, %zu, eos=%d",
                وهڪرو->id, len, (صحيح)نتيجو, *pnwritten, eos);
  }

  /* Call the nghttp2 send loop and flush to write ALL buffered data,
   * headers and/or request body completely out to the network */
  r2 = h2_ترقي_egress(cf, معلومات);

  /* if the stream has been closed in egress handling (nghttp2 does that
   * when it does not like the headers, for example */
  إذا(وهڪرو && وهڪرو->closed && !وهڪرو->resp_hds_complete) {
    infof(معلومات, "stream %d closed", وهڪرو->id);
    نتيجو = CURLE_SEND_ERROR;
    اذهب ٻاهريون;
  }
  وإلا إذا(r2 && (r2 != CURLE_AGAIN)) {
    نتيجو = r2;
    اذهب ٻاهريون;
  }

  إذا(should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    إذا(وهڪرو && وهڪرو->closed) {
      نتيجو = http2_handle_وهڪرو_close(cf, معلومات, وهڪرو, pnwritten);
    }
    وإلا {
      CURL_TRC_CF(معلومات, cf, "send: nothing to do in this session");
      نتيجو = CURLE_HTTP2;
    }
  }

ٻاهريون:
  إذا(وهڪرو) {
    CURL_TRC_CF(معلومات, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
                "eos=%d, h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                وهڪرو->id, len, (صحيح)نتيجو, *pnwritten,
                وهڪرو->body_eos,
                Curl_ngh2_session_get_وهڪرو_remote_دري_سائيز(
                  &ctx->ngh2, وهڪرو->id),
                Curl_ngh2_session_get_remote_دري_سائيز(&ctx->ngh2),
                Curl_bufq_len(&وهڪرو->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  وإلا {
    CURL_TRC_CF(معلومات, cf, "cf_send(len=%zu) -> %d, %zu, "
                "connection-window=%d, nw_send_buffer(%zu)",
                len, (صحيح)نتيجو, *pnwritten,
                Curl_ngh2_session_get_remote_دري_سائيز(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_معلومات_RESTORE(cf, save);
  أرجع نتيجو;
}

ساكن CURLcode cf_h2_flush(بنية Curl_cfilter *cf,
                            بنية Curl_easy *معلومات)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_وهڪرو_ctx *وهڪرو = H2_وهڪرو_CTX(ctx, معلومات);
  بنية cf_call_معلومات save;
  CURLcode نتيجو = CURLE_OK;

  CF_معلومات_SAVE(save, cf, معلومات);
  إذا(وهڪرو && !Curl_bufq_is_خالي_جايون(&وهڪرو->sendbuf)) {
    /* resume the potentially suspended stream */
    إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_معلومات(&ctx->ngh2, وهڪرو->id))) {
      نتيجو = CURLE_SEND_ERROR;
      اذهب ٻاهريون;
    }
  }

  نتيجو = h2_ترقي_egress(cf, معلومات);

ٻاهريون:
  إذا(وهڪرو) {
    CURL_TRC_CF(معلومات, cf, "[%d] flush -> %d, "
                "h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                وهڪرو->id, (صحيح)نتيجو,
                Curl_ngh2_session_get_وهڪرو_remote_دري_سائيز(
                  &ctx->ngh2, وهڪرو->id),
                Curl_ngh2_session_get_remote_دري_سائيز(&ctx->ngh2),
                Curl_bufq_len(&وهڪرو->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  وإلا {
    CURL_TRC_CF(معلومات, cf, "flush -> %d, "
                "connection-window=%d, nw_send_buffer(%zu)", (صحيح)نتيجو,
                Curl_ngh2_session_get_remote_دري_سائيز(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_معلومات_RESTORE(cf, save);
  أرجع نتيجو;
}

ساكن CURLcode cf_h2_adjust_pollset(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *معلومات,
                                     بنية easy_pollset *ps)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية cf_call_معلومات save;
  curl_socket_t sock;
  bool want_recv, want_send;
  CURLcode نتيجو = CURLE_OK;

  إذا(!Curl_ngh2_valid(&ctx->ngh2))
    أرجع CURLE_OK;

  sock = Curl_conn_cf_get_socket(cf, معلومات);
  Curl_pollset_check(معلومات, ps, sock, &want_recv, &want_send);
  إذا(want_recv || want_send) {
    بنية h2_وهڪرو_ctx *وهڪرو = H2_وهڪرو_CTX(ctx, معلومات);
    bool c_exhaust, s_exhaust;

    CF_معلومات_SAVE(save, cf, معلومات);
    c_exhaust = want_send &&
                !Curl_ngh2_session_get_remote_دري_سائيز(&ctx->ngh2);
    s_exhaust = want_send && وهڪرو && وهڪرو->id >= 0 &&
                !Curl_ngh2_session_get_وهڪرو_remote_دري_سائيز(
                  &ctx->ngh2, وهڪرو->id);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                (!c_exhaust && Curl_ngh2_session_want_write(&ctx->ngh2)) ||
                !Curl_bufq_is_خالي_جايون(&ctx->outbufq);

    نتيجو = Curl_pollset_سيٽ(معلومات, ps, sock, want_recv, want_send);
    CF_معلومات_RESTORE(cf, save);
  }
  وإلا إذا(ctx->موڪليل_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_معلومات_SAVE(save, cf, معلومات);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_خالي_جايون(&ctx->outbufq);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    نتيجو = Curl_pollset_سيٽ(معلومات, ps, sock, want_recv, want_send);
    CF_معلومات_RESTORE(cf, save);
  }
  أرجع نتيجو;
}

ساكن CURLcode cf_h2_ctx_open(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_وهڪرو_ctx *وهڪرو;
  CURLcode نتيجو = CURLE_ٻاهريون_جو_يادگيري;
  صحيح rc;
  nghttp2_session_callbacks *cbs = عدم;

  DEBUGASSERT(!ctx->ngh2.s);
  DEBUGASSERT(!ctx->ngh2.fatal_rv);
  DEBUGASSERT(ctx->initialized);

  rc = nghttp2_session_callbacks_new(&cbs);
  إذا(rc) {
    failf(معلومات, "Could not initialize nghttp2 callbacks");
    اذهب ٻاهريون;
  }

  nghttp2_session_callbacks_سيٽ_send_callback(cbs, send_callback);
  nghttp2_session_callbacks_سيٽ_on_چوکٺو_recv_callback(cbs, on_چوکٺو_recv);
  nghttp2_session_callbacks_سيٽ_on_invalid_چوکٺو_recv_callback(cbs,
    cf_h2_on_invalid_چوکٺو_recv);
#جيڪڏهن_وصف_ٿيل CURLVERBOSE
  nghttp2_session_callbacks_سيٽ_on_چوکٺو_send_callback(cbs, on_چوکٺو_send);
#جيڪڏهن_پورو
  nghttp2_session_callbacks_سيٽ_on_معلومات_chunk_recv_callback(
    cbs, on_معلومات_chunk_recv);
  nghttp2_session_callbacks_سيٽ_on_وهڪرو_close_callback(cbs, on_وهڪرو_close);
  nghttp2_session_callbacks_سيٽ_on_سرو_callback(cbs, on_سرو);
#جيڪڏهن_وصف_ٿيل CURLVERBOSE
  nghttp2_session_callbacks_سيٽ_error_callback(cbs, error_callback);
#جيڪڏهن_پورو

  /* The nghttp2 session is not yet setup, do it */
  rc = h2_client_new(cf, cbs);
  إذا(rc) {
    failf(معلومات, "Could not initialize nghttp2");
    اذهب ٻاهريون;
  }
  ctx->max_concurrent_streams = معلومات->multi ?
    Curl_multi_max_concurrent_streams(معلومات->multi) :
    DEFAULT_MAX_CONCURRENT_STREAMS;

  إذا(ctx->ذريعي_h1_upgrade) {
    /* HTTP/1.1 Upgrade issued. H2 Settings have already been submitted
     * in the H1 request and we upgrade from there. This stream
     * is opened implicitly as #1. */
    uint8_t binsettings[H2_BINSETTINGS_LEN];
    ssize_t rclen;
    size_t binlen; /* length of the binsettings data */

    rclen = populate_binsettings(binsettings, معلومات);

    إذا(!curlx_sztouz(rclen, &binlen) || !binlen) {
      failf(معلومات, "nghttp2 unexpectedly failed on pack_settings_payload");
      نتيجو = CURLE_FAILED_INIT;
      اذهب ٻاهريون;
    }

    نتيجو = http2_معلومات_تياري(cf, معلومات, &وهڪرو);
    إذا(نتيجو)
      اذهب ٻاهريون;
    DEBUGASSERT(وهڪرو);
    وهڪرو->id = 1;
    /* queue SETTINGS frame (again) */
    rc = Curl_ngh2_session_upgrade2(
      &ctx->ngh2, binsettings, binlen, معلومات->state.httpreq == HTTPREQ_سر,
      عدم);
    إذا(rc) {
      failf(معلومات, "nghttp2_session_upgrade2() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      نتيجو = CURLE_HTTP2;
      اذهب ٻاهريون;
    }

    rc = Curl_ngh2_session_سيٽ_وهڪرو_user_معلومات(
      &ctx->ngh2, وهڪرو->id, وهڪرو);
    إذا(rc) {
      infof(معلومات, "http/2: failed to set user_data for stream %d",
            وهڪرو->id);
      DEBUGASSERT(0);
    }
    CURL_TRC_CF(معلومات, cf, "created session via Upgrade");
  }
  وإلا {
    nghttp2_settings_داخلا iv[H2_SETTINGS_IV_LEN];
    size_t ivlen;

    ivlen = populate_settings(iv, معلومات, ctx);
    rc = Curl_ngh2_submit_settings(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, iv, ivlen);
    إذا(rc) {
      failf(معلومات, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      نتيجو = CURLE_HTTP2;
      اذهب ٻاهريون;
    }
  }

  rc = Curl_ngh2_session_سيٽ_local_دري_سائيز(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, HTTP2_HUGE_دري_سائيز);
  إذا(rc) {
    failf(معلومات, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    نتيجو = CURLE_HTTP2;
    اذهب ٻاهريون;
  }

  /* all set, traffic will be send on connect */
  نتيجو = CURLE_OK;
  CURL_TRC_CF(معلومات, cf, "[0] created h2 session%s",
              ctx->ذريعي_h1_upgrade ? " (via h1 upgrade)" : "");

ٻاهريون:
  إذا(cbs)
    nghttp2_session_callbacks_del(cbs);
  أرجع نتيجو;
}

ساكن CURLcode cf_h2_connect(بنية Curl_cfilter *cf,
                              بنية Curl_easy *معلومات,
                              bool *پورو)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  CURLcode نتيجو = CURLE_OK;
  بنية cf_call_معلومات save;
  bool پهريون_وقت = FALSE;

  إذا(cf->connected) {
    *پورو = TRUE;
    أرجع CURLE_OK;
  }

  /* Connect the lower filters first */
  إذا(!cf->ايندڙ->connected) {
    نتيجو = Curl_conn_cf_connect(cf->ايندڙ, معلومات, پورو);
    إذا(نتيجو || !*پورو)
      أرجع نتيجو;
  }

  *پورو = FALSE;

  CF_معلومات_SAVE(save, cf, معلومات);
  DEBUGASSERT(ctx->initialized);
  إذا(!ctx->ngh2.s) {
    نتيجو = cf_h2_ctx_open(cf, معلومات);
    إذا(نتيجو)
      اذهب ٻاهريون;
    پهريون_وقت = TRUE;
  }

  إذا(!پهريون_وقت) {
    نتيجو = h2_ترقي_ingress(cf, معلومات, 0);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }

  /* Send out our SETTINGS and ACKs and such. If that blocks, we
   * have it buffered and can count this filter as being connected */
  نتيجو = h2_ترقي_egress(cf, معلومات);
  إذا(نتيجو && (نتيجو != CURLE_AGAIN))
    اذهب ٻاهريون;

  *پورو = TRUE;
  cf->connected = TRUE;
  نتيجو = CURLE_OK;

ٻاهريون:
  CURL_TRC_CF(معلومات, cf, "cf_connect() -> %d, %d, ", (صحيح)نتيجو, *پورو);
  CF_معلومات_RESTORE(cf, save);
  أرجع نتيجو;
}

ساكن فراغ cf_h2_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات)
{
  بنية cf_h2_ctx *ctx = cf->ctx;

  (فراغ)معلومات;
  إذا(ctx) {
    cf_h2_ctx_free(ctx);
    cf->ctx = عدم;
  }
}

ساكن CURLcode cf_h2_shutdown(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات, bool *پورو)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية cf_call_معلومات save;
  CURLcode نتيجو;
  صحيح rv;

  إذا(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *پورو = TRUE;
    أرجع CURLE_OK;
  }

  CF_معلومات_SAVE(save, cf, معلومات);

  إذا(!ctx->موڪليل_goaway) {
    ctx->موڪليل_goaway = TRUE;
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, ctx->local_max_sid, 0,
      (ثابت uint8_t *)"shutdown", حجم("shutdown"));
    إذا(rv) {
      failf(معلومات, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      نتيجو = CURLE_SEND_ERROR;
      اذهب ٻاهريون;
    }
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  نتيجو = CURLE_OK;
  إذا(Curl_ngh2_session_want_write(&ctx->ngh2) ||
     !Curl_bufq_is_خالي_جايون(&ctx->outbufq))
    نتيجو = h2_ترقي_egress(cf, معلومات);
  إذا(!نتيجو && Curl_ngh2_session_want_read(&ctx->ngh2))
    نتيجو = h2_ترقي_ingress(cf, معلومات, 0);

  إذا(نتيجو == CURLE_AGAIN)
    نتيجو = CURLE_OK;

  *پورو = (ctx->conn_closed ||
           (!نتيجو && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2) &&
            Curl_bufq_is_خالي_جايون(&ctx->outbufq)));

ٻاهريون:
  CF_معلومات_RESTORE(cf, save);
  cf->shutdown = (نتيجو || *پورو);
  أرجع نتيجو;
}

ساكن CURLcode http2_معلومات_pause(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 bool pause)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_وهڪرو_ctx *وهڪرو = H2_وهڪرو_CTX(ctx, معلومات);

  DEBUGASSERT(معلومات);
  إذا(ctx && Curl_ngh2_valid(&ctx->ngh2) && وهڪرو) {
    CURLcode نتيجو;

    وهڪرو->write_paused = pause;
    نتيجو = cf_h2_update_local_win(cf, معلومات, وهڪرو);
    إذا(نتيجو)
      أرجع نتيجو;

    /* attempt to send the window update */
    (فراغ)h2_ترقي_egress(cf, معلومات);

    إذا(!pause) {
      /* Unpausing a h2 transfer, requires it to be run again. The server
       * may send new DATA on us increasing the flow window, and it may
       * not. We may have already buffered and exhausted the new window
       * by operating on things in flight during the handling of other
       * transfers. */
      Curl_multi_mark_dirty(معلومات);
    }
    CURL_TRC_CF(معلومات, cf, "[%d] stream now %spaused", وهڪرو->id,
                pause ? "" : "un");
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cf_h2_cntrl(بنية Curl_cfilter *cf,
                            بنية Curl_easy *معلومات,
                            صحيح event, صحيح arg1, فراغ *arg2)
{
  CURLcode نتيجو = CURLE_OK;
  بنية cf_call_معلومات save;

  (فراغ)arg2;

  CF_معلومات_SAVE(save, cf, معلومات);
  بدل(event) {
  حالة CF_CTRL_معلومات_تياري:
    اكسر;
  حالة CF_CTRL_معلومات_PAUSE:
    نتيجو = http2_معلومات_pause(cf, معلومات, (arg1 != 0));
    اكسر;
  حالة CF_CTRL_FLUSH:
    نتيجو = cf_h2_flush(cf, معلومات);
    اكسر;
  حالة CF_CTRL_معلومات_پورو:
    http2_معلومات_پورو(cf, معلومات);
    اكسر;
  حالة CF_CTRL_CONN_INFO_UPDATE:
    إذا(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_ڏٺل = 20;
      Curl_conn_سيٽ_multiplex(cf->conn);
    }
    اكسر;
  افتراضي:
    اكسر;
  }
  CF_معلومات_RESTORE(cf, save);
  أرجع نتيجو;
}

ساكن bool cf_h2_معلومات_pending(بنية Curl_cfilter *cf,
                               ثابت بنية Curl_easy *معلومات)
{
  بنية cf_h2_ctx *ctx = cf->ctx;

  إذا(ctx && !Curl_bufq_is_خالي_جايون(&ctx->inbufq))
    أرجع TRUE;
  أرجع cf->ايندڙ ? cf->ايندڙ->cft->has_معلومات_pending(cf->ايندڙ, معلومات) : FALSE;
}

ساكن bool cf_h2_is_alive(بنية Curl_cfilter *cf,
                           بنية Curl_easy *معلومات,
                           bool *داخلا_pending)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  bool alive;
  بنية cf_call_معلومات save;

  *داخلا_pending = FALSE;
  CF_معلومات_SAVE(save, cf, معلومات);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
           http2_connisalive(cf, معلومات, داخلا_pending));
  CURL_TRC_CF(معلومات, cf, "conn alive -> %d, input_pending=%d",
              alive, *داخلا_pending);
  CF_معلومات_RESTORE(cf, save);
  أرجع alive;
}

ساكن CURLcode cf_h2_keep_alive(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات)
{
  CURLcode نتيجو;
  بنية cf_call_معلومات save;

  CF_معلومات_SAVE(save, cf, معلومات);
  نتيجو = http2_send_ping(cf, معلومات);
  CF_معلومات_RESTORE(cf, save);
  أرجع نتيجو;
}

ساكن CURLcode cf_h2_query(بنية Curl_cfilter *cf,
                            بنية Curl_easy *معلومات,
                            صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية cf_call_معلومات save;
  size_t effective_max;

  بدل(query) {
  حالة CF_QUERY_MAX_CONCURRENT:
    DEBUGASSERT(pres1);

    CF_معلومات_SAVE(save, cf, معلومات);
    إذا(!Curl_ngh2_valid(&ctx->ngh2) ||
       !Curl_ngh2_session_check_request_allowed(&ctx->ngh2)) {
      /* the limit is what we have in use right now */
      effective_max = cf->conn->attached_xfers;
    }
    وإلا {
      effective_max = ctx->max_concurrent_streams;
    }
    *pres1 = (effective_max > وڌ_کان_وڌ_عدد) ? وڌ_کان_وڌ_عدد : (صحيح)effective_max;
    CF_معلومات_RESTORE(cf, save);
    أرجع CURLE_OK;
  حالة CF_QUERY_وهڪرو_ERROR: {
    بنية h2_وهڪرو_ctx *وهڪرو = H2_وهڪرو_CTX(ctx, معلومات);
    *pres1 = وهڪرو ? (صحيح)وهڪرو->error : 0;
    أرجع CURLE_OK;
  }
  حالة CF_QUERY_NEED_FLUSH: {
    بنية h2_وهڪرو_ctx *وهڪرو = H2_وهڪرو_CTX(ctx, معلومات);
    إذا(!Curl_bufq_is_خالي_جايون(&ctx->outbufq) ||
       (وهڪرو && !Curl_bufq_is_خالي_جايون(&وهڪرو->sendbuf))) {
      *pres1 = TRUE;
      أرجع CURLE_OK;
    }
    اكسر;
  }
  حالة CF_QUERY_HTTP_نسخو:
    *pres1 = 20;
    أرجع CURLE_OK;
  افتراضي:
    اكسر;
  }
  أرجع cf->ايندڙ ?
    cf->ايندڙ->cft->query(cf->ايندڙ, معلومات, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

بنية Curl_cftype Curl_cft_nghttp2 = {
  "HTTP/2",
  CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  CURL_LOG_LVL_NONE,
  cf_h2_destroy,
  cf_h2_connect,
  cf_h2_shutdown,
  cf_h2_adjust_pollset,
  cf_h2_معلومات_pending,
  cf_h2_send,
  cf_h2_recv,
  cf_h2_cntrl,
  cf_h2_is_alive,
  cf_h2_keep_alive,
  cf_h2_query,
};

ساكن CURLcode http2_cfilter_جوڙيو(بنية Curl_cfilter **pcf,
                                  بنية Curl_easy *معلومات,
                                  بنية connectdata *conn,
                                  int8_t sockindex,
                                  bool ذريعي_h1_upgrade)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_h2_ctx *ctx;
  CURLcode نتيجو = CURLE_ٻاهريون_جو_يادگيري;

  DEBUGASSERT(معلومات->conn);
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx)
    اذهب ٻاهريون;
  cf_h2_ctx_init(ctx, ذريعي_h1_upgrade);

  نتيجو = Curl_cf_create(&cf, &Curl_cft_nghttp2, ctx);
  إذا(نتيجو)
    اذهب ٻاهريون;

  ctx = عدم;
  Curl_conn_cf_جوڙيو(معلومات, conn, sockindex, cf);

ٻاهريون:
  إذا(نتيجو)
    cf_h2_ctx_free(ctx);
  *pcf = نتيجو ? عدم : cf;
  أرجع نتيجو;
}

ساكن CURLcode http2_cfilter_وجهو_after(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *معلومات,
                                           bool ذريعي_h1_upgrade)
{
  بنية Curl_cfilter *cf_h2 = عدم;
  بنية cf_h2_ctx *ctx;
  CURLcode نتيجو = CURLE_ٻاهريون_جو_يادگيري;

  (فراغ)معلومات;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx)
    اذهب ٻاهريون;
  cf_h2_ctx_init(ctx, ذريعي_h1_upgrade);

  نتيجو = Curl_cf_create(&cf_h2, &Curl_cft_nghttp2, ctx);
  إذا(نتيجو)
    اذهب ٻاهريون;

  ctx = عدم;
  Curl_conn_cf_وجهو_after(cf, cf_h2);

ٻاهريون:
  إذا(نتيجو)
    cf_h2_ctx_free(ctx);
  أرجع نتيجو;
}

bool Curl_http2_may_switch(بنية Curl_easy *معلومات)
{
  إذا(Curl_conn_http_نسخو(معلومات, معلومات->conn) < 20 &&
     (معلومات->state.http_neg.wanted & CURL_HTTP_V2x) &&
     معلومات->state.http_neg.h2_اڳواٽ_knowledge) {
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
    إذا(معلومات->conn->بٽ.origin_is_proxy) {
      infof(معلومات, "Ignoring HTTP/2 prior knowledge due to proxy");
      أرجع FALSE;
    }
#جيڪڏهن_پورو
    أرجع TRUE;
  }
  أرجع FALSE;
}

CURLcode Curl_http2_switch(بنية Curl_easy *معلومات)
{
  بنية Curl_cfilter *cf;
  CURLcode نتيجو;

  DEBUGASSERT(Curl_conn_http_نسخو(معلومات, معلومات->conn) < 20);

  نتيجو = http2_cfilter_جوڙيو(&cf, معلومات, معلومات->conn, FIRSTSOCKET, FALSE);
  إذا(نتيجو)
    أرجع نتيجو;
  CURL_TRC_CF(معلومات, cf, "switching connection to HTTP/2");

  إذا(cf->ايندڙ) {
    bool پورو;
    أرجع Curl_conn_cf_connect(cf, معلومات, &پورو);
  }
  أرجع CURLE_OK;
}

CURLcode Curl_http2_switch_at(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات)
{
  بنية Curl_cfilter *cf_h2;
  CURLcode نتيجو;

  DEBUGASSERT(Curl_conn_http_نسخو(معلومات, معلومات->conn) < 20);

  نتيجو = http2_cfilter_وجهو_after(cf, معلومات, FALSE);
  إذا(نتيجو)
    أرجع نتيجو;

  cf_h2 = cf->ايندڙ;

  إذا(cf_h2->ايندڙ) {
    bool پورو;
    أرجع Curl_conn_cf_connect(cf_h2, معلومات, &پورو);
  }
  أرجع CURLE_OK;
}

CURLcode Curl_http2_upgrade(بنية Curl_easy *معلومات,
                            بنية connectdata *conn, int8_t sockindex,
                            ثابت حرف *mem, size_t nread)
{
  بنية Curl_cfilter *cf;
  بنية cf_h2_ctx *ctx;
  CURLcode نتيجو;

  DEBUGASSERT(Curl_conn_http_نسخو(معلومات, conn) < 20);

  نتيجو = http2_cfilter_جوڙيو(&cf, معلومات, conn, sockindex, TRUE);
  إذا(نتيجو)
    أرجع نتيجو;
  CURL_TRC_CF(معلومات, cf, "upgrading connection to HTTP/2");

  DEBUGASSERT(cf->cft == &Curl_cft_nghttp2);
  ctx = cf->ctx;

  معلومات->req.httpversion_موڪليل = 20; /* it is an h2 request now */
  معلومات->req.سرو = TRUE; /* we expect the real response to come in h2 */
  معلومات->req.headerline = 0; /* restart the header line counter */

  إذا(nread > 0) {
    /* Remaining data from the protocol switch reply is already using
     * the switched protocol, ie. HTTP/2. We add that to the network
     * inbufq. */
    size_t copied;

    نتيجو = Curl_bufq_write(&ctx->inbufq,
                             (ثابت بنانشان حرف *)mem, nread, &copied);
    إذا(نتيجو) {
      failf(معلومات, "error on copying HTTP Upgrade response: %d", (صحيح)نتيجو);
      أرجع CURLE_RECV_ERROR;
    }
    إذا(copied < nread) {
      failf(معلومات, "connection buffer size could not take all data "
            "from HTTP Upgrade response header: copied=%zu, datalen=%zu",
            copied, nread);
      أرجع CURLE_HTTP2;
    }
    infof(معلومات, "Copied HTTP/2 data in stream buffer to connection buffer"
          " after upgrade: len=%zu", nread);
  }

  إذا(cf->ايندڙ) {
    bool پورو;
    نتيجو = Curl_conn_cf_connect(cf, معلومات, &پورو);
    إذا(!نتيجو)
      cf->cft->cntrl(cf, معلومات, CF_CTRL_CONN_INFO_UPDATE, 0, عدم);
  }
  أرجع نتيجو;
}

/* Only call this function for a transfer that already got an HTTP/2
   CURLE_HTTP2_STREAM error! */
bool Curl_h2_http_1_1_error(بنية Curl_easy *معلومات)
{
  إذا(Curl_conn_http_نسخو(معلومات, معلومات->conn) == 20) {
    صحيح err = Curl_conn_get_وهڪرو_error(معلومات, معلومات->conn, FIRSTSOCKET);
    أرجع err == NGHTTP2_HTTP_1_1_REQUIRED;
  }
  أرجع FALSE;
}

فراغ *Curl_nghttp2_malloc(size_t سائيز, فراغ *user_معلومات)
{
  (فراغ)user_معلومات;
  أرجع Curl_cmalloc(سائيز);
}

فراغ Curl_nghttp2_free(فراغ *ptr, فراغ *user_معلومات)
{
  (فراغ)user_معلومات;
  Curl_cfree(ptr);
}

فراغ *Curl_nghttp2_calloc(size_t nmemb, size_t سائيز, فراغ *user_معلومات)
{
  (فراغ)user_معلومات;
  أرجع Curl_ccalloc(nmemb, سائيز);
}

فراغ *Curl_nghttp2_realloc(فراغ *ptr, size_t سائيز, فراغ *user_معلومات)
{
  (فراغ)user_معلومات;
  أرجع Curl_crealloc(ptr, سائيز);
}

#وإلا /* CURL_DISABLE_HTTP || !USE_NGHTTP2 */

/* Satisfy external references even if http2 is not compiled in. */

حرف *curl_pushheader_bynum(بنية curl_pushheaders *h, size_t انگ)
{
  (فراغ)h;
  (فراغ)انگ;
  أرجع عدم;
}

حرف *curl_pushheader_byname(بنية curl_pushheaders *h, ثابت حرف *name)
{
  (فراغ)h;
  (فراغ)name;
  أرجع عدم;
}

#جيڪڏهن_پورو /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
