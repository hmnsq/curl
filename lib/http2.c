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

#إذا NGHTTP2_نسخہ_عدد < 0x010f00
#غلطی "nghttp2 1.15.0 or greater required"
#جے_انت

#تعریف_کرو NGHTTP2_HAS_سیٹ_LOCAL_کھڑکی_ناپ 1

/* buffer dimensioning:
 * use 16K as chunk size, as that fits H2 DATA frames well */
#تعریف_کرو H2_CHUNK_ناپ           (16 * 1024)
/* connection window size */
#تعریف_کرو H2_CONN_کھڑکی_ناپ     (10 * 1024 * 1024)
/* on receiving from TLS, we prep for holding a full stream window */
#تعریف_کرو H2_NW_RECV_CHUNKS       (H2_CONN_کھڑکی_ناپ / H2_CHUNK_ناپ)
/* on send into TLS, we want to accumulate small frames */
#تعریف_کرو H2_NW_SEND_CHUNKS       1
/* this is how much we want "in flight" for a stream, unthrottled */
#تعریف_کرو H2_دھارا_کھڑکی_ناپ_MAX   (10 * 1024 * 1024)
/* this is how much we want "in flight" for a stream, initially, IFF
 * nghttp2 allows us to tweak the local window size. */
#إذا NGHTTP2_HAS_سیٹ_LOCAL_کھڑکی_ناپ
#تعریف_کرو H2_دھارا_کھڑکی_ناپ_INITIAL  (64 * 1024)
#وإلا
#تعریف_کرو H2_دھارا_کھڑکی_ناپ_INITIAL H2_دھارا_کھڑکی_ناپ_MAX
#جے_انت
/* keep smaller stream upload buffer (default h2 window size) to have
 * our progress bars and "upload done" reporting closer to reality */
#تعریف_کرو H2_دھارا_SEND_CHUNKS   ((64 * 1024) / H2_CHUNK_ناپ)
/* spare chunks we keep for a full window */
#تعریف_کرو H2_دھارا_POOL_SPARES   (H2_CONN_کھڑکی_ناپ / H2_CHUNK_ناپ)

/* We need to accommodate the max number of streams with their window sizes on
 * the overall connection. Streams might become PAUSED which will block their
 * received QUOTA in the connection window. If we run out of space, the server
 * is blocked from sending us any data. See #10988 for an issue with this. */
#تعریف_کرو HTTP2_HUGE_کھڑکی_ناپ (100 * H2_دھارا_کھڑکی_ناپ_MAX)

#تعریف_کرو H2_SETTINGS_IV_LEN 3
#تعریف_کرو H2_BINSETTINGS_LEN 80

بنية cf_h2_ctx {
  بنية curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  بنية cf_call_انکڑے call_انکڑے;

  بنية bufq inbufq;           /* network input */
  بنية bufq outbufq;          /* network output */
  بنية bufc_pool دھارا_bufcp; /* spares for stream buffers */
  بنية dynbuf scratch;        /* scratch buffer for temp use */

  بنية u32_ptrset streams; /* hash of `data->mid` to `h2_stream_ctx` */
  size_t drain_کل; /* sum of all stream's UrlState drain */
  uint32_t initial_win_ناپ; /* current initial window size (settings) */
  uint32_t max_concurrent_streams;
  uint32_t goaway_error;        /* goaway error code from server */
  int32_t remote_max_sid;       /* max id processed by server */
  int32_t local_max_sid;        /* max id processed by us */
  ثنائی_ہندسہ(initialized);
  ثنائی_ہندسہ(راہیں_h1_upgrade);
  ثنائی_ہندسہ(conn_closed);
  ثنائی_ہندسہ(rcvd_goaway);
  ثنائی_ہندسہ(گھلیا_goaway);
  ثنائی_ہندسہ(enable_ڈالو);
  ثنائی_ہندسہ(nw_باہرلا_blocked);
};

/* How to access `call_data` from a cf_h2 filter */
#تعریف_رد CF_CTX_CALL_انکڑے
#تعریف_کرو CF_CTX_CALL_انکڑے(cf) ((بنية cf_h2_ctx *)(cf)->ctx)->call_انکڑے

/**
 * All about the H2 internals of a stream
 */
بنية h2_دھارا_ctx {
  بنية bufq sendbuf; /* request buffer */
  بنية h1_req_parser h1; /* parsing the request */
  بنية dynhds resp_trailers; /* response trailer fields */
  size_t resp_hds_len; /* amount of response header bytes in recvbuf */
  curl_off_t xfer_id;  /* transfer `id` owning this stream */
  curl_off_t nrcvd_انکڑے;  /* number of DATA bytes received */

  حرف **ڈالو_سرخیاں;       /* allocated array */
  size_t ڈالو_سرخیاں_ورتیا;  /* number of entries filled in */
  size_t ڈالو_سرخیاں_alloc; /* number of entries allocated */

  int32_t id; /* HTTP/2 protocol identifier for stream */
  uint32_t mid; /* transfer `mid` owning this stream */
  uint32_t error; /* stream error code */
  CURLcode xfer_نتیجہ; /* Result of writing out response */
  int32_t local_کھڑکی_ناپ; /* the local recv window size */
  int32_t حالت_کوڈ; /* HTTP response status code */
  ثنائی_ہندسہ(resp_hds_complete); /* we have a complete, final response */
  ثنائی_ہندسہ(closed); /* TRUE on stream close */
  ثنائی_ہندسہ(reset);  /* TRUE on stream reset */
  ثنائی_ہندسہ(reset_by_server);  /* TRUE on stream reset by server */
  ثنائی_ہندسہ(close_handled); /* TRUE if stream closure is handled by libcurl */
  ثنائی_ہندسہ(bodystarted);
  ثنائی_ہندسہ(body_eos);    /* the complete body has been added to `sendbuf` and
                     * is being/has been processed from there. */
  ثنائی_ہندسہ(write_paused);  /* stream write is paused */
};

ساكن فراغ free_ڈالو_سرخیاں(بنية h2_دھارا_ctx *دھارا)
{
  size_t i;
  لكل(i = 0; i < دھارا->ڈالو_سرخیاں_ورتیا; i++)
    curlx_free(دھارا->ڈالو_سرخیاں[i]);
  curlx_safefree(دھارا->ڈالو_سرخیاں);
  دھارا->ڈالو_سرخیاں_ورتیا = 0;
}

ساكن فراغ h2_دھارا_ctx_free(بنية h2_دھارا_ctx *دھارا)
{
  Curl_bufq_free(&دھارا->sendbuf);
  Curl_h1_req_parse_free(&دھارا->h1);
  Curl_dynhds_free(&دھارا->resp_trailers);
  free_ڈالو_سرخیاں(دھارا);
  curlx_free(دھارا);
}

ساكن فراغ h2_دھارا_hash_free(uint32_t id, فراغ *دھارا)
{
  (فراغ)id;
  DEBUGASSERT(دھارا);
  h2_دھارا_ctx_free((بنية h2_دھارا_ctx *)دھارا);
}

ساكن فراغ cf_h2_ctx_init(بنية cf_h2_ctx *ctx, bool راہیں_h1_upgrade)
{
  Curl_bufcp_init(&ctx->دھارا_bufcp, H2_CHUNK_ناپ, H2_دھارا_POOL_SPARES);
  Curl_bufq_initp(&ctx->inbufq, &ctx->دھارا_bufcp, H2_NW_RECV_CHUNKS, 0);
  Curl_bufq_initp(&ctx->outbufq, &ctx->دھارا_bufcp, H2_NW_SEND_CHUNKS, 0);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_سرخی);
  Curl_u32_ptrset_init(&ctx->streams, h2_دھارا_hash_free);
  ctx->remote_max_sid = INT32_MAX;
  ctx->راہیں_h1_upgrade = راہیں_h1_upgrade;
  ctx->initialized = TRUE;
}

ساكن فراغ cf_h2_ctx_free(بنية cf_h2_ctx *ctx)
{
  إذا(ctx && ctx->initialized) {
    Curl_ngh2_del(&ctx->ngh2);
    Curl_bufq_free(&ctx->inbufq);
    Curl_bufq_free(&ctx->outbufq);
    Curl_bufcp_free(&ctx->دھارا_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    memset(ctx, 0, حجم(*ctx));
  }
  curlx_free(ctx);
}

ساكن uint32_t cf_h2_initial_win_ناپ(بنية Curl_easy *انکڑے)
{
#إذا NGHTTP2_HAS_سیٹ_LOCAL_کھڑکی_ناپ
  /* If the transfer has a rate-limit lower than the default initial
   * stream window size, use that. It needs to be at least 8k or servers
   * may be unhappy. */
  curl_off_t rps = Curl_rlimit_فی_قدم(&انکڑے->ترقی.dl.rlimit);
  إذا((rps > 0) && (rps < H2_دھارا_کھڑکی_ناپ_INITIAL))
    أرجع CURLMAX((uint32_t)rps, 8192);
#جے_انت
  أرجع H2_دھارا_کھڑکی_ناپ_INITIAL;
}

ساكن size_t populate_settings(nghttp2_settings_اندراج *iv,
                                بنية Curl_easy *انکڑے,
                                بنية cf_h2_ctx *ctx)
{
  iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
  iv[0].قدر = Curl_multi_max_concurrent_streams(انکڑے->multi);

  iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_کھڑکی_ناپ;
  iv[1].قدر = cf_h2_initial_win_ناپ(انکڑے);
  إذا(ctx)
    ctx->initial_win_ناپ = iv[1].قدر;
  iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_ڈالو;
  iv[2].قدر = !!انکڑے->multi->ڈالو_cb;

  أرجع 3;
}

ساكن ssize_t populate_binsettings(uint8_t *binsettings,
                                    بنية Curl_easy *انکڑے)
{
  nghttp2_settings_اندراج iv[H2_SETTINGS_IV_LEN];
  size_t ivlen;

  ivlen = populate_settings(iv, انکڑے, عدم);
  /* this returns number of bytes it wrote or a negative number on error. */
  أرجع nghttp2_pack_settings_payload(binsettings, H2_BINSETTINGS_LEN,
                                       iv, ivlen);
}

ساكن CURLcode cf_h2_update_settings(بنية cf_h2_ctx *ctx,
                                      uint32_t initial_win_ناپ)
{
  nghttp2_settings_اندراج اندراج;
  اندراج.settings_id = NGHTTP2_SETTINGS_INITIAL_کھڑکی_ناپ;
  اندراج.قدر = initial_win_ناپ;
  إذا(Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, &اندراج, 1))
    أرجع CURLE_SEND_ERROR;
  ctx->initial_win_ناپ = initial_win_ناپ;
  أرجع CURLE_OK;
}

#تعریف_کرو H2_دھارا_CTX(ctx, انکڑے)                                         \
  ((بنية h2_دھارا_ctx *)(                                             \
    (انکڑے) ? Curl_u32_ptrset_get(&(ctx)->streams, (انکڑے)->mid) : عدم))

ساكن بنية h2_دھارا_ctx *h2_دھارا_ctx_create(بنية cf_h2_ctx *ctx,
                                                  بنية Curl_easy *انکڑے)
{
  بنية h2_دھارا_ctx *دھارا;

  دھارا = curlx_calloc(1, حجم(*دھارا));
  إذا(!دھارا)
    أرجع عدم;

  دھارا->id = -1;
  دھارا->xfer_id = انکڑے->id;
  دھارا->mid = انکڑے->mid;
  Curl_bufq_initp(&دھارا->sendbuf, &ctx->دھارا_bufcp,
                  H2_دھارا_SEND_CHUNKS, BUFQ_OPT_NONE);
  Curl_h1_req_parse_init(&دھارا->h1, H1_PARSE_DEFAULT_MAX_سطر_LEN);
  Curl_dynhds_init(&دھارا->resp_trailers,
                   MAX_HTTP_RESP_سرخی_گنتی, DYN_HTTP_REQUEST);
  دھارا->bodystarted = FALSE;
  دھارا->حالت_کوڈ = -1;
  دھارا->closed = FALSE;
  دھارا->close_handled = FALSE;
  دھارا->error = NGHTTP2_NO_ERROR;
  دھارا->local_کھڑکی_ناپ = H2_دھارا_کھڑکی_ناپ_INITIAL;
  دھارا->nrcvd_انکڑے = 0;
  أرجع دھارا;
}

ساكن بنية Curl_easy *cf_h2_get_دھارا_xfer(بنية Curl_cfilter *cf,
                                               int32_t دھارا_id,
                                               بنية h2_دھارا_ctx **pstream)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_دھارا_ctx *دھارا =
    Curl_ngh2_session_get_دھارا_user_انکڑے(&ctx->ngh2, دھارا_id);
  إذا(pstream)
    *pstream = دھارا;
  إذا(دھارا && (دھارا->mid != UINT32_MAX)) {
    بنية Curl_easy *انکڑے, *call_انکڑے = CF_انکڑے_CURRENT(cf);
    إذا(!call_انکڑے || !call_انکڑے->multi) {
      DEBUGASSERT(0);
      أرجع عدم;
    }
    انکڑے = Curl_multi_get_easy(call_انکڑے->multi, دھارا->mid);
    إذا(انکڑے && (انکڑے->id == دھارا->xfer_id))
      أرجع انکڑے;
  }
  أرجع عدم;
}

#جے_تعریف_شدہ NGHTTP2_HAS_سیٹ_LOCAL_کھڑکی_ناپ
ساكن int32_t cf_h2_get_desired_local_win(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *انکڑے)
{
  curl_off_t avail = Curl_rlimit_avail(&انکڑے->ترقی.dl.rlimit, عدم);

  (فراغ)cf;
  إذا(avail < CURL_OFF_T_MAX) { /* limit in place */
    إذا(avail <= 0)
      أرجع 0;
    وإلا إذا(avail < H2_دھارا_کھڑکی_ناپ_MAX)
      أرجع (int32_t)avail;
  }
  أرجع H2_دھارا_کھڑکی_ناپ_MAX;
}

ساكن CURLcode cf_h2_update_local_win(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *انکڑے,
                                       بنية h2_دھارا_ctx *دھارا)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  int32_t dwsize;
  صحيح rv;

  dwsize = (دھارا->write_paused || دھارا->xfer_نتیجہ) ?
           0 : cf_h2_get_desired_local_win(cf, انکڑے);
  إذا(dwsize != دھارا->local_کھڑکی_ناپ) {
    int32_t wsize = Curl_ngh2_session_get_دھارا_effective_local_کھڑکی_ناپ(
      &ctx->ngh2, دھارا->id);
    إذا(dwsize > wsize) {
      rv = Curl_ngh2_session_سیٹ_local_کھڑکی_ناپ(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, دھارا->id, dwsize);
      إذا(rv) {
        failf(انکڑے, "[%d] nghttp2 set_local_window_size(%d) failed: "
              "%s(%d)", دھارا->id, dwsize, nghttp2_strerror(rv), rv);
        أرجع CURLE_HTTP2;
      }
      rv = Curl_ngh2_submit_کھڑکی_update(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, دھارا->id, dwsize - wsize);
      إذا(rv) {
        failf(انکڑے, "[%d] nghttp2_submit_window_update() failed: "
              "%s(%d)", دھارا->id, nghttp2_strerror(rv), rv);
        أرجع CURLE_HTTP2;
      }
      دھارا->local_کھڑکی_ناپ = dwsize;
      CURL_TRC_CF(انکڑے, cf, "[%d] local window update by %d",
                  دھارا->id, dwsize - wsize);
    }
    وإلا {
      rv = Curl_ngh2_session_سیٹ_local_کھڑکی_ناپ(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, دھارا->id, dwsize);
      إذا(rv) {
        failf(انکڑے, "[%d] nghttp2_session_set_local_window_size() failed: "
              "%s(%d)", دھارا->id, nghttp2_strerror(rv), rv);
        أرجع CURLE_HTTP2;
      }
      دھارا->local_کھڑکی_ناپ = dwsize;
      CURL_TRC_CF(انکڑے, cf, "[%d] local window size now %d",
                  دھارا->id, dwsize);
    }
  }
  أرجع CURLE_OK;
}

#وإلا /* NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

ساكن CURLcode cf_h2_update_local_win(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *انکڑے,
                                       بنية h2_دھارا_ctx *دھارا)
{
  (فراغ)cf;
  (فراغ)انکڑے;
  (فراغ)دھارا;
  أرجع CURLE_OK;
}
#جے_انت /* !NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

ساكن CURLcode http2_انکڑے_تیاری(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *انکڑے,
                                 بنية h2_دھارا_ctx **pstream)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_دھارا_ctx *دھارا;

  (فراغ)cf;
  DEBUGASSERT(انکڑے);

  إذا(!انکڑے)
    أرجع CURLE_BAD_FUNCTION_دلیل;

  دھارا = H2_دھارا_CTX(ctx, انکڑے);
  إذا(دھارا) {
    *pstream = دھارا;
    أرجع CURLE_OK;
  }

  دھارا = h2_دھارا_ctx_create(ctx, انکڑے);
  إذا(!دھارا)
    أرجع CURLE_باہرلا_دا_حافظہ;

  إذا(Curl_u32_ptrset_سیٹ(&ctx->streams, انکڑے->mid, دھارا)) {
    h2_دھارا_ctx_free(دھارا);
    أرجع CURLE_باہرلا_دا_حافظہ;
  }

  *pstream = دھارا;
  أرجع CURLE_OK;
}

ساكن CURLcode nw_باہرلا_flush(بنية Curl_cfilter *cf,
                             بنية Curl_easy *انکڑے)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode نتیجہ;

  إذا(Curl_bufq_is_خالی_تھاواں(&ctx->outbufq))
    أرجع CURLE_OK;

  نتیجہ = Curl_cf_send_bufq(cf->اگلا, انکڑے, &ctx->outbufq, عدم, 0,
                             &nwritten);
  إذا(نتیجہ) {
    إذا(نتیجہ == CURLE_AGAIN) {
      CURL_TRC_CF(انکڑے, cf, "flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_باہرلا_blocked = 1;
    }
    أرجع نتیجہ;
  }
  أرجع Curl_bufq_is_خالی_تھاواں(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

ساكن فراغ http2_انکڑے_پورا(بنية Curl_cfilter *cf, بنية Curl_easy *انکڑے)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_دھارا_ctx *دھارا = H2_دھارا_CTX(ctx, انکڑے);

  DEBUGASSERT(ctx);
  إذا(!دھارا || !ctx->initialized)
    أرجع;

  إذا(Curl_ngh2_valid(&ctx->ngh2)) {
    bool flush_egress = FALSE;
    /* returns error if stream not known, which is fine here */
    (فراغ)Curl_ngh2_session_سیٹ_دھارا_user_انکڑے(&ctx->ngh2, دھارا->id, عدم);

    إذا(!دھارا->closed && دھارا->id > 0) {
      /* RST_STREAM */
      CURL_TRC_CF(انکڑے, cf, "[%d] premature DATA_DONE, RST stream",
                  دھارا->id);
      دھارا->closed = TRUE;
      دھارا->reset = TRUE;
      Curl_ngh2_submit_rst_دھارا(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                  دھارا->id, NGHTTP2_دھارا_CLOSED);
      flush_egress = TRUE;
    }

    إذا(flush_egress) {
      (فراغ)Curl_ngh2_session_send(&ctx->ngh2);
      (فراغ)nw_باہرلا_flush(cf, انکڑے);
    }
  }

  Curl_u32_ptrset_unset(&ctx->streams, انکڑے->mid);
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
  nghttp2_option_سیٹ_no_auto_کھڑکی_update(o, 1);
#إذا NGHTTP2_نسخہ_عدد >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_سیٹ_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#جے_انت
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  أرجع rc;
}

/*
 * Returns nonzero if current HTTP/2 session should be closed.
 */
ساكن صحيح should_close_session(بنية cf_h2_ctx *ctx)
{
  أرجع ctx->drain_کل == 0 && !Curl_ngh2_session_want_read(&ctx->ngh2) &&
    !Curl_ngh2_session_want_write(&ctx->ngh2);
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
ساكن CURLcode h2_process_pending_داخلہ(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *انکڑے)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  ثابت بنانشانی حرف *buf;
  size_t blen, nread;
  ssize_t rv;

  طالما(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {
    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ثابت uint8_t *)buf, blen);
    إذا(!curlx_sztouz(rv, &nread)) {
      failf(انکڑے, "nghttp2 recv error %zd: %s", rv, nghttp2_strerror((صحيح)rv));
      أرجع CURLE_RECV_ERROR;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    إذا(Curl_bufq_is_خالی_تھاواں(&ctx->inbufq)) {
      اكسر;
    }
    وإلا {
      CURL_TRC_CF(انکڑے, cf, "process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }

  إذا(Curl_ngh2_session_check_request_allowed(&ctx->ngh2) == 0) {
    /* No more requests are allowed in the current session, so
       the connection may not be reused. This is set when a
       GOAWAY frame has been received or when the limit of stream
       identifiers has been reached. */
    CURL_TRC_M(انکڑے, "http/2: No new requests allowed");
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
ساكن bool http2_connisalive(بنية Curl_cfilter *cf, بنية Curl_easy *انکڑے,
                              bool *داخلہ_pending)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *داخلہ_pending = FALSE;
  إذا(!cf->اگلا || !cf->اگلا->cft->is_alive(cf->اگلا, انکڑے, داخلہ_pending))
    أرجع FALSE;

  إذا(*داخلہ_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode نتیجہ;
    size_t nread;

    *داخلہ_pending = FALSE;
    نتیجہ = Curl_cf_recv_bufq(cf->اگلا, انکڑے, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(انکڑے, cf, "connisalive, recv pending input -> %d, %zu",
                (صحيح)نتیجہ, nread);
    إذا(!نتیجہ) {
      نتیجہ = h2_process_pending_داخلہ(cf, انکڑے);
      إذا(نتیجہ)
        /* immediate error, considered dead */
        alive = FALSE;
      وإلا {
        alive = !should_close_session(ctx);
      }
    }
    وإلا إذا(نتیجہ != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  أرجع alive;
}

ساكن CURLcode http2_send_ping(بنية Curl_cfilter *cf,
                                بنية Curl_easy *انکڑے)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  صحيح rc;

  rc = Curl_ngh2_submit_ping(&ctx->ngh2, 0, ZERO_NULL);
  إذا(rc) {
    failf(انکڑے, "nghttp2_submit_ping() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    أرجع CURLE_HTTP2;
  }

  rc = Curl_ngh2_session_send(&ctx->ngh2);
  إذا(rc) {
    failf(انکڑے, "nghttp2_session_send() failed: %s(%d)",
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
  nghttp2_info *h2 = nghttp2_نسخہ(0);
  (فراغ)curl_msnprintf(p, len, "nghttp2/%s", h2->نسخہ_str);
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
  بنية Curl_easy *انکڑے = CF_انکڑے_CURRENT(cf);
  size_t nwritten;
  CURLcode نتیجہ = CURLE_OK;

  (فراغ)h2;
  (فراغ)flags;
  DEBUGASSERT(انکڑے);

  إذا(!cf->connected)
    نتیجہ = Curl_bufq_write(&ctx->outbufq, buf, blen, &nwritten);
  وإلا
    نتیجہ = Curl_cf_send_bufq(cf->اگلا, انکڑے, &ctx->outbufq, buf, blen,
                               &nwritten);

  إذا(نتیجہ) {
    إذا(نتیجہ == CURLE_AGAIN) {
      ctx->nw_باہرلا_blocked = 1;
      أرجع NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(انکڑے, "Failed sending HTTP2 data");
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  إذا(!nwritten) {
    ctx->nw_باہرلا_blocked = 1;
    أرجع NGHTTP2_ERR_WOULDBLOCK;
  }
  أرجع (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

/* We pass a pointer to this struct in the push callback, but the contents of
   the struct are hidden from the user. */
بنية curl_pushheaders {
  بنية Curl_easy *انکڑے;
  بنية h2_دھارا_ctx *دھارا;
  ثابت nghttp2_ڈالو_promise *چوکھٹا;
};

/*
 * push header access function. Only to be used from within the push callback
 */
حرف *curl_pushheader_bynum(بنية curl_pushheaders *h, size_t عدد)
{
  /* Verify that we got a good easy handle in the push header struct, mostly to
     detect rubbish input fast(er). */
  إذا(!h || !GOOD_EASY_HANDLE(h->انکڑے))
    أرجع عدم;
  وإلا {
    إذا(h->دھارا && عدد < h->دھارا->ڈالو_سرخیاں_ورتیا)
      أرجع h->دھارا->ڈالو_سرخیاں[عدد];
  }
  أرجع عدم;
}

/*
 * push header access function. Only to be used from within the push callback
 */
حرف *curl_pushheader_byname(بنية curl_pushheaders *h, ثابت حرف *name)
{
  بنية h2_دھارا_ctx *دھارا;
  size_t len;
  size_t i;
  /* Verify that we got a good easy handle in the push header struct,
     mostly to detect rubbish input fast(er). Also empty header name
     is rubbish too. We have to allow ":" at the beginning of
     the header, but header == ":" must be rejected. If we have ':' in
     the middle of header, it could be matched in middle of the value,
     this is because we do prefix match.*/
  إذا(!h || !GOOD_EASY_HANDLE(h->انکڑے) || !name || !name[0] ||
     !strcmp(name, ":") || strchr(name + 1, ':'))
    أرجع عدم;

  دھارا = h->دھارا;
  إذا(!دھارا)
    أرجع عدم;

  len = لڑی_لمبائی(name);
  لكل(i = 0; i < دھارا->ڈالو_سرخیاں_ورتیا; i++) {
    إذا(!لڑی_مقابلہ(name, دھارا->ڈالو_سرخیاں[i], len)) {
      /* sub-match, make sure that it is followed by a colon */
      إذا(دھارا->ڈالو_سرخیاں[i][len] != ':')
        استمر;
      أرجع &دھارا->ڈالو_سرخیاں[i][len + 1];
    }
  }
  أرجع عدم;
}

ساكن بنية Curl_easy *h2_duphandle(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *انکڑے)
{
  بنية Curl_easy *دوجا = curl_easy_init();
  إذا(دوجا) {
    بنية h2_دھارا_ctx *دوجا_دھارا;
    http2_انکڑے_تیاری(cf, دوجا, &دوجا_دھارا);
    دوجا->state.بھار = انکڑے->state.بھار;
    إذا(انکڑے->share)
      (فراغ)Curl_share_easy_link(دوجا, انکڑے->share);
    /* copy a range of options from the parent handle */
    دوجا->سیٹ.fwrite_func = انکڑے->سیٹ.fwrite_func;
    دوجا->سیٹ.باہرلا = انکڑے->سیٹ.باہرلا;
    دوجا->سیٹ.fwrite_سرخی = انکڑے->سیٹ.fwrite_سرخی;
    دوجا->سیٹ.writeheader = انکڑے->سیٹ.writeheader;
    دوجا->سیٹ.fdebug = انکڑے->سیٹ.fdebug;
    دوجا->سیٹ.debugdata = انکڑے->سیٹ.debugdata;
    دوجا->سیٹ.fxferinfo = انکڑے->سیٹ.fxferinfo;
    دوجا->سیٹ.fprogress = انکڑے->سیٹ.fprogress;
    دوجا->سیٹ.ترقی_client = انکڑے->سیٹ.ترقی_client;
    دوجا->ترقی.hide = انکڑے->ترقی.hide;
    دوجا->ترقی.callback = انکڑے->ترقی.callback;
  }
  أرجع دوجا;
}

ساكن صحيح سیٹ_transfer_url(بنية Curl_easy *newhandle,
                            بنية curl_pushheaders *hp,
                            بنية Curl_easy *انکڑے)
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
    uc = curl_url_سیٹ(u, CURLUPART_SCHEME, v, 0);
    إذا(uc) {
      rc = 1;
      اذهب fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_AUTHORITY);
  إذا(v) {
    uc = Curl_url_سیٹ_authority(u, v);
    إذا(uc) {
      rc = 2;
      اذهب fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_رستہ);
  إذا(v) {
    uc = curl_url_سیٹ(u, CURLUPART_رستہ, v, 0);
    إذا(uc) {
      rc = 3;
      اذهب fail;
    }
  }

  /* We can only allow PUSH of resource from the same origin, e.g.
   * scheme + hostname + port */
  إذا(!Curl_url_same_origin(انکڑے->state.uh, u)) {
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
  Curl_bufref_سیٹ(&newhandle->state.url, url, 0, curl_free);
  أرجع 0;
}

ساكن فراغ discard_newhandle(بنية Curl_cfilter *cf,
                              بنية Curl_easy *newhandle)
{
  http2_انکڑے_پورا(cf, newhandle);
  (فراغ)Curl_close(&newhandle);
}

ساكن صحيح ڈالو_promise(بنية Curl_cfilter *cf,
                        بنية Curl_easy *انکڑے,
                        ثابت nghttp2_ڈالو_promise *چوکھٹا)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  صحيح rv; /* one of the CURL_PUSH_* defines */

  CURL_TRC_CF(انکڑے, cf, "[%d] PUSH_PROMISE received",
              چوکھٹا->promised_دھارا_id);
  إذا(انکڑے->multi->ڈالو_cb) {
    بنية h2_دھارا_ctx *دھارا;
    بنية h2_دھارا_ctx *newstream;
    بنية curl_pushheaders heads;
    CURLMcode mresult;
    CURLcode نتیجہ;
    /* clone the parent */
    بنية Curl_easy *newhandle = h2_duphandle(cf, انکڑے);
    إذا(!newhandle) {
      infof(انکڑے, "failed to duplicate handle");
      rv = CURL_ڈالو_DENY; /* FAIL HARD */
      اذهب fail;
    }

    دھارا = H2_دھارا_CTX(ctx, انکڑے);
    إذا(!دھارا) {
      failf(انکڑے, "Internal NULL stream");
      discard_newhandle(cf, newhandle);
      rv = CURL_ڈالو_DENY;
      اذهب fail;
    }

    heads.انکڑے = انکڑے;
    heads.دھارا = دھارا;
    heads.چوکھٹا = چوکھٹا;

    rv = سیٹ_transfer_url(newhandle, &heads, انکڑے);
    إذا(rv) {
      CURL_TRC_CF(انکڑے, cf, "[%d] PUSH_PROMISE, failed to set URL -> %d",
                  چوکھٹا->promised_دھارا_id, rv);
      discard_newhandle(cf, newhandle);
      rv = CURL_ڈالو_DENY;
      اذهب fail;
    }

    {
      بنية Curl_mapi_guard guard;
      CURL_CBAPI_شروع(&guard, انکڑے, multi_ڈالو_cb);
      rv = انکڑے->multi->ڈالو_cb(انکڑے, newhandle,
                                دھارا->ڈالو_سرخیاں_ورتیا, &heads,
                                انکڑے->multi->ڈالو_userp);
      CURL_CBAPI_سرا(&guard);
    }

    /* free the headers again */
    free_ڈالو_سرخیاں(دھارا);

    إذا(rv) {
      DEBUGASSERT((rv > CURL_ڈالو_OK) && (rv <= CURL_ڈالو_ERROROUT));
      /* denied, kill off the new handle again */
      CURL_TRC_CF(انکڑے, cf, "[%d] PUSH_PROMISE, denied by application -> %d",
                  چوکھٹا->promised_دھارا_id, rv);
      discard_newhandle(cf, newhandle);
      اذهب fail;
    }

    /* approved, add to the multi handle for processing. This
     * assigns newhandle->mid. For the new `mid` we assign the
     * h2_stream instance and remember the stream_id already known. */
    mresult = Curl_multi_جوڑو_perform(انکڑے->multi, newhandle, cf->conn);
    إذا(mresult) {
      infof(انکڑے, "failed to add handle to multi");
      discard_newhandle(cf, newhandle);
      rv = CURL_ڈالو_DENY;
      اذهب fail;
    }

    نتیجہ = http2_انکڑے_تیاری(cf, newhandle, &newstream);
    إذا(نتیجہ) {
      failf(انکڑے, "error setting up stream: %d", (صحيح)نتیجہ);
      discard_newhandle(cf, newhandle);
      rv = CURL_ڈالو_DENY;
      اذهب fail;
    }

    DEBUGASSERT(newstream);
    newstream->id = چوکھٹا->promised_دھارا_id;
    newhandle->req.maxdownload = -1;
    newhandle->req.ناپ = -1;

    CURL_TRC_CF(انکڑے, cf, "promise easy handle added to multi, mid=%u",
                newhandle->mid);
    rv = Curl_ngh2_session_سیٹ_دھارا_user_انکڑے(
      &ctx->ngh2, newstream->id, newstream);
    إذا(rv) {
      infof(انکڑے, "failed to set user_data for stream %d",
            newstream->id);
      DEBUGASSERT(0);
      discard_newhandle(cf, newhandle);
      rv = CURL_ڈالو_DENY;
      اذهب fail;
    }

    /* success, remember max stream id processed */
    إذا(newstream->id > ctx->local_max_sid)
      ctx->local_max_sid = newstream->id;
  }
  وإلا {
    CURL_TRC_CF(انکڑے, cf, "Got PUSH_PROMISE, ignore it");
    rv = CURL_ڈالو_DENY;
  }
fail:
  أرجع rv;
}

ساكن فراغ h2_xfer_write_resp_hd(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *انکڑے,
                                  بنية h2_دھارا_ctx *دھارا,
                                  ثابت حرف *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  إذا(!دھارا->xfer_نتیجہ) {
    دھارا->xfer_نتیجہ = Curl_xfer_write_resp_hd(انکڑے, buf, blen, eos);
    إذا(!دھارا->xfer_نتیجہ && !eos)
      دھارا->xfer_نتیجہ = cf_h2_update_local_win(cf, انکڑے, دھارا);
    إذا(دھارا->xfer_نتیجہ)
      CURL_TRC_CF(انکڑے, cf, "[%d] error %d writing %zu bytes of headers",
                  دھارا->id, (صحيح)دھارا->xfer_نتیجہ, blen);
  }
}

ساكن فراغ h2_xfer_write_resp(بنية Curl_cfilter *cf,
                               بنية Curl_easy *انکڑے,
                               بنية h2_دھارا_ctx *دھارا,
                               ثابت حرف *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  إذا(!دھارا->xfer_نتیجہ)
    دھارا->xfer_نتیجہ = Curl_xfer_write_resp(انکڑے, buf, blen, eos);
  /* If the transfer write is errored, we do not want any more data */
  إذا(دھارا->xfer_نتیجہ) {
    بنية cf_h2_ctx *ctx = cf->ctx;
    CURL_TRC_CF(انکڑے, cf, "[%d] error %d writing %zu bytes of data, "
                "RST-ing stream",
                دھارا->id, (صحيح)دھارا->xfer_نتیجہ, blen);
    Curl_ngh2_submit_rst_دھارا(
      &ctx->ngh2, 0, دھارا->id, (uint32_t)NGHTTP2_ERR_CALLBACK_FAILURE);
  }
  وإلا إذا(!دھارا->write_paused && Curl_xfer_write_is_paused(انکڑے)) {
    CURL_TRC_CF(انکڑے, cf, "[%d] stream output paused", دھارا->id);
    دھارا->write_paused = TRUE;
  }
  وإلا إذا(دھارا->write_paused && !Curl_xfer_write_is_paused(انکڑے)) {
    CURL_TRC_CF(انکڑے, cf, "[%d] stream output unpaused", دھارا->id);
    دھارا->write_paused = FALSE;
  }

  إذا(!دھارا->xfer_نتیجہ && !eos)
    دھارا->xfer_نتیجہ = cf_h2_update_local_win(cf, انکڑے, دھارا);
}

ساكن CURLcode on_دھارا_چوکھٹا(بنية Curl_cfilter *cf,
                                بنية Curl_easy *انکڑے,
                                بنية h2_دھارا_ctx *دھارا,
                                ثابت nghttp2_چوکھٹا *چوکھٹا)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  int32_t دھارا_id = چوکھٹا->hd.دھارا_id;
  صحيح rv;

  إذا(!دھارا) {
    CURL_TRC_CF(انکڑے, cf, "[%d] No stream_ctx set", دھارا_id);
    أرجع CURLE_FAILED_INIT;
  }

  بدل(چوکھٹا->hd.type) {
  حالة NGHTTP2_انکڑے:
    CURL_TRC_CF(انکڑے, cf, "[%d] DATA, window=%d/%d",
                دھارا_id,
                Curl_ngh2_session_get_دھارا_effective_recv_انکڑے_لمائی(
                  &ctx->ngh2, دھارا->id),
                Curl_ngh2_session_get_دھارا_effective_local_کھڑکی_ناپ(
                  &ctx->ngh2, دھارا->id));
    /* If !body started on this stream, then receiving DATA is illegal. */
    إذا(!دھارا->bodystarted) {
      إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_دھارا(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                       دھارا_id, NGHTTP2_PROTOCOL_ERROR))) {
        أرجع CURLE_RECV_ERROR;
      }
    }
    اكسر;
  حالة NGHTTP2_سرخیاں:
    إذا(دھارا->bodystarted) {
      /* Only valid HEADERS after body started is trailer HEADERS. We
         buffer them in on_header callback. */
      اكسر;
    }

    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    إذا(دھارا->حالت_کوڈ == -1)
      أرجع CURLE_RECV_ERROR;

    /* Only final status code signals the end of header */
    إذا(دھارا->حالت_کوڈ / 100 != 1) {
      دھارا->resp_hds_complete = TRUE;
      دھارا->bodystarted = TRUE;
    }
    وإلا
      دھارا->حالت_کوڈ = -1;

    h2_xfer_write_resp_hd(cf, انکڑے, دھارا, STRCONST("\r\n"),
                          (bool)دھارا->closed);

    Curl_multi_mark_dirty(انکڑے);
    اكسر;
  حالة NGHTTP2_ڈالو_PROMISE:
    rv = ڈالو_promise(cf, انکڑے, &چوکھٹا->ڈالو_promise);
    إذا(rv) { /* deny! */
      DEBUGASSERT((rv > CURL_ڈالو_OK) && (rv <= CURL_ڈالو_ERROROUT));
      إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_دھارا(&ctx->ngh2, NGHTTP2_FLAG_NONE,
             چوکھٹا->ڈالو_promise.promised_دھارا_id, NGHTTP2_CANCEL)))
        أرجع CURLE_SEND_ERROR;
      وإلا إذا(rv == CURL_ڈالو_ERROROUT) {
        /* fail the parent transfer, not the whole session */
        CURL_TRC_CF(انکڑے, cf, "[%d] fail in PUSH_PROMISE received",
                    دھارا_id);
        إذا(!دھارا->xfer_نتیجہ)
          دھارا->xfer_نتیجہ = CURLE_RECV_ERROR;
        Curl_multi_mark_dirty(انکڑے);
      }
    }
    اكسر;
  حالة NGHTTP2_RST_دھارا:
    إذا(چوکھٹا->rst_دھارا.error_کوڈ)
      دھارا->reset_by_server = TRUE;
    Curl_multi_mark_dirty(انکڑے);
    اكسر;
  حالة NGHTTP2_کھڑکی_UPDATE:
    إذا(CURL_REQ_WANT_SEND(انکڑے) && Curl_bufq_is_خالی_تھاواں(&دھارا->sendbuf)) {
      /* need more data, force processing of transfer */
      Curl_multi_mark_dirty(انکڑے);
    }
    وإلا إذا(!Curl_bufq_is_خالی_تھاواں(&دھارا->sendbuf)) {
      /* resume the potentially suspended stream */
      إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_session_resume_انکڑے(&ctx->ngh2, دھارا->id)))
        أرجع CURLE_SEND_ERROR;
    }
    اكسر;
  افتراضي:
    اكسر;
  }

  إذا(چوکھٹا->hd.flags & NGHTTP2_FLAG_سرا_دھارا) {
    إذا(!دھارا->closed && !دھارا->body_eos &&
       ((دھارا->حالت_کوڈ >= 400) || (دھارا->حالت_کوڈ < 200))) {
      /* The server did not give us a positive response and we are not
       * done uploading the request body. We need to stop doing that and
       * also inform the server that we aborted our side. */
      CURL_TRC_CF(انکڑے, cf, "[%d] EOS frame with unfinished upload and "
                  "HTTP status %d, abort upload by RST",
                  دھارا_id, دھارا->حالت_کوڈ);
      Curl_ngh2_submit_rst_دھارا(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, دھارا->id, NGHTTP2_دھارا_CLOSED);
      دھارا->closed = TRUE;
    }
    Curl_multi_mark_dirty(انکڑے);
  }
  أرجع CURLE_OK;
}

#جے_تعریف_شدہ CURLVERBOSE
صحيح Curl_nghttp2_fr_print(ثابت nghttp2_چوکھٹا *چوکھٹا, حرف *بفر,
                          size_t blen)
{
  بدل(چوکھٹا->hd.type) {
  حالة NGHTTP2_انکڑے: {
    أرجع curl_msnprintf(بفر, blen,
                          "FRAME[DATA, len=%d, eos=%d, padlen=%d]",
                          (صحيح)چوکھٹا->hd.لمائی,
                          !!(چوکھٹا->hd.flags & NGHTTP2_FLAG_سرا_دھارا),
                          (صحيح)چوکھٹا->انکڑے.padlen);
  }
  حالة NGHTTP2_سرخیاں: {
    أرجع curl_msnprintf(بفر, blen,
                          "FRAME[HEADERS, len=%d, hend=%d, eos=%d]",
                          (صحيح)چوکھٹا->hd.لمائی,
                          !!(چوکھٹا->hd.flags & NGHTTP2_FLAG_سرا_سرخیاں),
                          !!(چوکھٹا->hd.flags & NGHTTP2_FLAG_سرا_دھارا));
  }
  حالة NGHTTP2_ترجیح: {
    أرجع curl_msnprintf(بفر, blen,
                          "FRAME[PRIORITY, len=%d, flags=%d]",
                          (صحيح)چوکھٹا->hd.لمائی, چوکھٹا->hd.flags);
  }
  حالة NGHTTP2_RST_دھارا: {
    أرجع curl_msnprintf(بفر, blen,
                          "FRAME[RST_STREAM, len=%d, flags=%d, error=%u]",
                          (صحيح)چوکھٹا->hd.لمائی, چوکھٹا->hd.flags,
                          چوکھٹا->rst_دھارا.error_کوڈ);
  }
  حالة NGHTTP2_SETTINGS: {
    إذا(چوکھٹا->hd.flags & NGHTTP2_FLAG_ACK) {
      أرجع curl_msnprintf(بفر, blen, "FRAME[SETTINGS, ack=1]");
    }
    أرجع curl_msnprintf(بفر, blen,
                          "FRAME[SETTINGS, len=%d]", (صحيح)چوکھٹا->hd.لمائی);
  }
  حالة NGHTTP2_ڈالو_PROMISE:
    أرجع curl_msnprintf(بفر, blen,
                          "FRAME[PUSH_PROMISE, len=%d, hend=%d]",
                          (صحيح)چوکھٹا->hd.لمائی,
                          !!(چوکھٹا->hd.flags & NGHTTP2_FLAG_سرا_سرخیاں));
  حالة NGHTTP2_PING:
    أرجع curl_msnprintf(بفر, blen,
                          "FRAME[PING, len=%d, ack=%d]",
                          (صحيح)چوکھٹا->hd.لمائی,
                          چوکھٹا->hd.flags & NGHTTP2_FLAG_ACK);
  حالة NGHTTP2_GOAWAY: {
    حرف scratch[128];
    size_t s_len = CURL_ARRAYSIZE(scratch);
    size_t len = (چوکھٹا->goaway.opaque_انکڑے_len < s_len) ?
      چوکھٹا->goaway.opaque_انکڑے_len : s_len - 1;
    إذا(len)
      حافظہ_نقل(scratch, چوکھٹا->goaway.opaque_انکڑے, len);
    scratch[len] = '\0';
    أرجع curl_msnprintf(بفر, blen,
                          "FRAME[GOAWAY, error=%u, reason='%s', "
                          "last_stream=%d]", چوکھٹا->goaway.error_کوڈ,
                          scratch, چوکھٹا->goaway.آخری_دھارا_id);
  }
  حالة NGHTTP2_کھڑکی_UPDATE: {
    أرجع curl_msnprintf(بفر, blen,
                          "FRAME[WINDOW_UPDATE, incr=%d]",
                          چوکھٹا->کھڑکی_update.کھڑکی_ناپ_increment);
  }
  افتراضي:
    أرجع curl_msnprintf(بفر, blen, "FRAME[%d, len=%d, flags=%d]",
                          چوکھٹا->hd.type, (صحيح)چوکھٹا->hd.لمائی,
                          چوکھٹا->hd.flags);
  }
}

ساكن صحيح on_چوکھٹا_send(nghttp2_session *session, ثابت nghttp2_چوکھٹا *چوکھٹا,
                         فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية Curl_easy *انکڑے = CF_انکڑے_CURRENT(cf);

  (فراغ)session;
  DEBUGASSERT(انکڑے);
  إذا(Curl_trc_cf_is_verbose(cf, انکڑے)) {
    حرف بفر[256];
    صحيح len;
    len = Curl_nghttp2_fr_print(چوکھٹا, بفر, حجم(بفر) - 1);
    بفر[len] = 0;
    CURL_TRC_CF(انکڑے, cf, "[%d] -> %s", چوکھٹا->hd.دھارا_id, بفر);
  }
  إذا((چوکھٹا->hd.type == NGHTTP2_GOAWAY) && !ctx->گھلیا_goaway) {
    /* A GOAWAY not initiated by us, but by nghttp2 itself on detecting
     * a protocol error on the connection */
    failf(انکڑے, "nghttp2 shuts down connection with error %u: %s",
          چوکھٹا->goaway.error_کوڈ,
          nghttp2_http2_strerror(چوکھٹا->goaway.error_کوڈ));
  }
  أرجع 0;
}
#جے_انت /* CURLVERBOSE */

ساكن صحيح on_چوکھٹا_recv(nghttp2_session *session, ثابت nghttp2_چوکھٹا *چوکھٹا,
                         فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية Curl_easy *انکڑے = CF_انکڑے_CURRENT(cf), *انکڑے_s;
  بنية h2_دھارا_ctx *دھارا;
  int32_t دھارا_id = چوکھٹا->hd.دھارا_id;

  (فراغ)session;
  DEBUGASSERT(انکڑے);
#جے_تعریف_شدہ CURLVERBOSE
  إذا(Curl_trc_cf_is_verbose(cf, انکڑے)) {
    حرف بفر[256];
    صحيح len;
    len = Curl_nghttp2_fr_print(چوکھٹا, بفر, حجم(بفر) - 1);
    بفر[len] = 0;
    CURL_TRC_CF(انکڑے, cf, "[%d] <- %s", چوکھٹا->hd.دھارا_id, بفر);
  }
#جے_انت /* CURLVERBOSE */

  إذا(!دھارا_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(انکڑے);
    بدل(چوکھٹا->hd.type) {
    حالة NGHTTP2_SETTINGS: {
      إذا(!(چوکھٹا->hd.flags & NGHTTP2_FLAG_ACK)) {
        uint32_t max_conn = ctx->max_concurrent_streams;
        ctx->max_concurrent_streams =
          Curl_ngh2_session_get_remote_settings(
            &ctx->ngh2, NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS);
        ctx->enable_ڈالو = Curl_ngh2_session_get_remote_settings(
          &ctx->ngh2, NGHTTP2_SETTINGS_ENABLE_ڈالو) != 0;
        CURL_TRC_CF(انکڑے, cf, "[0] MAX_CONCURRENT_STREAMS: %u",
                    ctx->max_concurrent_streams);
        CURL_TRC_CF(انکڑے, cf, "[0] ENABLE_PUSH: %s",
                    ctx->enable_ڈالو ? "TRUE" : "false");
        إذا(انکڑے && max_conn != ctx->max_concurrent_streams) {
          /* only signal change if the value actually changed */
          CURL_TRC_CF(انکڑے, cf, "[0] notify MAX_CONCURRENT_STREAMS: %u",
                      ctx->max_concurrent_streams);
          Curl_multi_connchanged(انکڑے->multi);
        }
        /* Since the initial stream window is 64K, a request might be on HOLD,
         * due to exhaustion. The (initial) SETTINGS may announce a much larger
         * window and *assume* that we treat this like a WINDOW_UPDATE. Some
         * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
         * To be safe, we UNHOLD a stream in order not to stall. */
        إذا(CURL_REQ_WANT_SEND(انکڑے))
          Curl_multi_mark_dirty(انکڑے);
      }
      اكسر;
    }
    حالة NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->goaway_error = چوکھٹا->goaway.error_کوڈ;
      ctx->remote_max_sid = چوکھٹا->goaway.آخری_دھارا_id;
      إذا(انکڑے) {
        infof(انکڑے, "received GOAWAY, error=%u, last_stream=%d",
              ctx->goaway_error, ctx->remote_max_sid);
        Curl_multi_connchanged(انکڑے->multi);
      }
      اكسر;
    افتراضي:
      اكسر;
    }
    أرجع 0;
  }

  انکڑے_s = cf_h2_get_دھارا_xfer(cf, دھارا_id, &دھارا);
  إذا(!انکڑے_s) {
    CURL_TRC_CF(انکڑے, cf, "[%d] No Curl_easy associated", دھارا_id);
    أرجع 0;
  }

  أرجع on_دھارا_چوکھٹا(cf, انکڑے_s, دھارا, چوکھٹا) ?
         NGHTTP2_ERR_CALLBACK_FAILURE : 0;
}

ساكن صحيح cf_h2_on_invalid_چوکھٹا_recv(nghttp2_session *session,
                                       ثابت nghttp2_چوکھٹا *چوکھٹا,
                                       صحيح ngerr, فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية Curl_easy *انکڑے;
  بنية h2_دھارا_ctx *دھارا;
  int32_t دھارا_id = چوکھٹا->hd.دھارا_id;

  (فراغ)session;
  انکڑے = cf_h2_get_دھارا_xfer(cf, دھارا_id, &دھارا);
  إذا(انکڑے) {
#جے_تعریف_شدہ CURLVERBOSE
    حرف بفر[256];
    صحيح len;
    len = Curl_nghttp2_fr_print(چوکھٹا, بفر, حجم(بفر) - 1);
    بفر[len] = 0;
    failf(انکڑے, "[HTTP2] [%d] received invalid frame: %s, error %d: %s",
          دھارا_id, بفر, ngerr, nghttp2_strerror(ngerr));
#جے_انت /* CURLVERBOSE */
    إذا(دھارا) {
      Curl_ngh2_submit_rst_دھارا(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, دھارا->id, NGHTTP2_دھارا_CLOSED);
      دھارا->error = ngerr;
      دھارا->closed = TRUE;
      دھارا->reset = TRUE;
    }
  }
  /* Return no error, nghttp2 will RST/GOAWAY by itself when needed */
  أرجع 0;
}

ساكن صحيح on_انکڑے_chunk_recv(nghttp2_session *session, uint8_t flags,
                              int32_t دھارا_id,
                              ثابت uint8_t *mem, size_t len, فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_دھارا_ctx *دھارا;
  بنية Curl_easy *انکڑے_s;

  (فراغ)session;
  (فراغ)flags;

  DEBUGASSERT(دھارا_id); /* should never be a zero stream ID here */
  DEBUGASSERT(CF_انکڑے_CURRENT(cf));

  /* get the stream from the hash based on Stream ID */
  انکڑے_s = cf_h2_get_دھارا_xfer(cf, دھارا_id, &دھارا);
  إذا(!انکڑے_s) {
    /* Receiving a Stream ID not in the hash should not happen - unless
       we have aborted a transfer artificially and there were more data
       in the pipeline. Silently ignore. */
    CURL_TRC_CF(CF_انکڑے_CURRENT(cf), cf, "[%d] Data for unknown", دھارا_id);
    /* consumed explicitly as no one will read it */
    Curl_ngh2_session_consume(&ctx->ngh2, دھارا_id, len);
    أرجع 0;
  }

  إذا(!دھارا)
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;

  h2_xfer_write_resp(cf, انکڑے_s, دھارا, (ثابت حرف *)mem, len, FALSE);

  Curl_ngh2_session_consume(&ctx->ngh2, دھارا_id, len);
  دھارا->nrcvd_انکڑے += (curl_off_t)len;
  أرجع 0;
}

ساكن صحيح on_دھارا_close(nghttp2_session *session, int32_t دھارا_id,
                           uint32_t error_کوڈ, فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية Curl_easy *انکڑے_s, *call_انکڑے = CF_انکڑے_CURRENT(cf);
  بنية h2_دھارا_ctx *دھارا = عدم;
  صحيح rv;
  (فراغ)session;

  DEBUGASSERT(call_انکڑے);
  /* stream id 0 is the connection, do not look there for streams. */
  انکڑے_s = دھارا_id ? cf_h2_get_دھارا_xfer(cf, دھارا_id, &دھارا) : عدم;
  إذا(!انکڑے_s) {
    CURL_TRC_CF(call_انکڑے, cf,
                "[%d] on_stream_close, no easy set on stream", دھارا_id);
    أرجع 0;
  }
  إذا(!GOOD_EASY_HANDLE(انکڑے_s)) {
    /* nghttp2 still has an easy registered for the stream which has
     * been freed be libcurl. This points to a code path that does not
     * trigger DONE or DETACH events as it must. */
    CURL_TRC_CF(call_انکڑے, cf,
                "[%d] on_stream_close, not a GOOD easy on stream", دھارا_id);
    (فراغ)Curl_ngh2_session_سیٹ_دھارا_user_انکڑے(
      &ctx->ngh2, دھارا_id, عدم);
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  إذا(!دھارا) {
    CURL_TRC_CF(انکڑے_s, cf,
                "[%d] on_stream_close, GOOD easy but no stream", دھارا_id);
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  دھارا->closed = TRUE;
  دھارا->error = error_کوڈ;
  إذا(دھارا->error) {
    دھارا->reset = TRUE;
    إذا((دھارا->error == NGHTTP2_REFUSED_دھارا) &&
       ctx->rcvd_goaway && ctx->goaway_error)
    /* REFUSED_STREAM is used by nghttp2 on a GOAWAY from the
     * server where it indicated it will no longer process this stream.
     * If the GOAWAY carried an error code, take that as the error for
     * the stream. */
    دھارا->error = ctx->goaway_error;
  }

  إذا(دھارا->error)
    CURL_TRC_CF(انکڑے_s, cf, "[%d] RESET: %s (err %u)",
              دھارا_id, nghttp2_http2_strerror(error_کوڈ), error_کوڈ);
  وإلا
    CURL_TRC_CF(انکڑے_s, cf, "[%d] CLOSED", دھارا_id);
  Curl_multi_mark_dirty(انکڑے_s);

  /* remove `stream_ctx` from the nghttp2 stream */
  rv = Curl_ngh2_session_سیٹ_دھارا_user_انکڑے(
    &ctx->ngh2, دھارا_id, عدم);
  إذا(rv) {
    infof(انکڑے_s, "http/2: failed to clear user_data for stream %d",
          دھارا_id);
    DEBUGASSERT(0);
  }
  أرجع 0;
}

ساكن فراغ cf_h2_سرخی_error(بنية Curl_cfilter *cf,
                               بنية Curl_easy *انکڑے,
                               بنية h2_دھارا_ctx *دھارا,
                               CURLcode نتیجہ)
{
  بنية cf_h2_ctx *ctx = cf->ctx;

  failf(انکڑے, "Error receiving HTTP2 header: %d(%s)", (صحيح)نتیجہ,
        curl_easy_strerror(نتیجہ));
  إذا(دھارا) {
    Curl_ngh2_submit_rst_دھارا(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, دھارا->id, NGHTTP2_دھارا_CLOSED);
    دھارا->closed = TRUE;
    دھارا->reset = TRUE;
  }
}

/* frame->hd.type is either NGHTTP2_HEADERS or NGHTTP2_PUSH_PROMISE */
ساكن صحيح on_سرخی(nghttp2_session *session, ثابت nghttp2_چوکھٹا *چوکھٹا,
                     ثابت uint8_t *name, size_t namelen,
                     ثابت uint8_t *قدر, size_t valuelen,
                     uint8_t flags,
                     فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_دھارا_ctx *دھارا;
  بنية Curl_easy *انکڑے;
  int32_t دھارا_id = چوکھٹا->hd.دھارا_id;
  CURLcode نتیجہ;

  (فراغ)session;
  (فراغ)flags;

  DEBUGASSERT(دھارا_id); /* should never be a zero stream ID here */

  /* get the stream from the hash based on Stream ID */
  انکڑے = cf_h2_get_دھارا_xfer(cf, دھارا_id, &دھارا);
  إذا(!GOOD_EASY_HANDLE(انکڑے))
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;

  إذا(!دھارا) {
    failf(انکڑے, "Internal NULL stream");
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  /* Store received PUSH_PROMISE headers to be used when the subsequent
     PUSH_PROMISE callback comes */
  إذا(چوکھٹا->hd.type == NGHTTP2_ڈالو_PROMISE) {
    حرف *h;

    إذا((namelen == CURL_CSTRLEN(HTTP_PSEUDO_AUTHORITY)) &&
       !لڑی_مقابلہ(HTTP_PSEUDO_AUTHORITY, (ثابت حرف *)name, namelen)) {
      /* pseudo headers are lower case */
      صحيح rc = 0;
      حرف *check = curl_maprintf("%s:%d", انکڑے->state.origin->hostname,
                                  انکڑے->state.origin->پورٹ);
      إذا(!check)
        /* no memory */
        أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
      إذا(!curl_strequal(check, (ثابت حرف *)قدر) &&
         ((انکڑے->state.origin->پورٹ != انکڑے->state.origin->scheme->defport) ||
          !curl_strequal(انکڑے->state.origin->hostname, (ثابت حرف *)قدر))) {
        /* This is push is not for the same authority that was asked for in
         * the URL. RFC 7540 section 8.2 says: "A client MUST treat a
         * PUSH_PROMISE for which the server is not authoritative as a stream
         * error of type PROTOCOL_ERROR."
         */
        (فراغ)Curl_ngh2_submit_rst_دھارا(
          &ctx->ngh2, NGHTTP2_FLAG_NONE, دھارا_id, NGHTTP2_PROTOCOL_ERROR);
        rc = NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      curlx_free(check);
      إذا(rc)
        أرجع rc;
    }

    إذا(!دھارا->ڈالو_سرخیاں) {
      دھارا->ڈالو_سرخیاں_alloc = 10;
      دھارا->ڈالو_سرخیاں = curlx_malloc(دھارا->ڈالو_سرخیاں_alloc *
                                          حجم(حرف *));
      إذا(!دھارا->ڈالو_سرخیاں)
        أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
      دھارا->ڈالو_سرخیاں_ورتیا = 0;
    }
    وإلا إذا(دھارا->ڈالو_سرخیاں_ورتیا ==
            دھارا->ڈالو_سرخیاں_alloc) {
      حرف **headp;
      إذا(دھارا->ڈالو_سرخیاں_alloc > 1000) {
        /* this is beyond crazy many headers, bail out */
        failf(انکڑے, "Too many PUSH_PROMISE headers");
        free_ڈالو_سرخیاں(دھارا);
        أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      دھارا->ڈالو_سرخیاں_alloc *= 2;
      headp = curlx_realloc(دھارا->ڈالو_سرخیاں,
                            دھارا->ڈالو_سرخیاں_alloc * حجم(حرف *));
      إذا(!headp) {
        free_ڈالو_سرخیاں(دھارا);
        أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      دھارا->ڈالو_سرخیاں = headp;
    }
    h = curl_maprintf("%s:%s", name, قدر);
    إذا(!h) {
      free_ڈالو_سرخیاں(دھارا);
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    دھارا->ڈالو_سرخیاں[دھارا->ڈالو_سرخیاں_ورتیا++] = h;
    أرجع 0;
  }

  إذا(دھارا->bodystarted) {
    /* This is a trailer */
    CURL_TRC_CF(انکڑے, cf, "[%d] trailer: %.*s: %.*s",
                دھارا->id, (صحيح)namelen, name, (صحيح)valuelen, قدر);
    نتیجہ = Curl_dynhds_جوڑو(&دھارا->resp_trailers,
                             (ثابت حرف *)name, namelen,
                             (ثابت حرف *)قدر, valuelen);
    إذا(نتیجہ) {
      cf_h2_سرخی_error(cf, انکڑے, دھارا, نتیجہ);
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    }

    أرجع 0;
  }

  إذا(namelen == CURL_CSTRLEN(HTTP_PSEUDO_حالت) &&
     !memcmp(HTTP_PSEUDO_حالت, name, namelen)) {
    /* nghttp2 guarantees :status is received first and only once. */
    حرف بفر[32];
    size_t hlen;
    نتیجہ = Curl_http_decode_حالت(&دھارا->حالت_کوڈ,
                                     (ثابت حرف *)قدر, valuelen);
    إذا(نتیجہ) {
      cf_h2_سرخی_error(cf, انکڑے, دھارا, نتیجہ);
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    hlen = curl_msnprintf(بفر, حجم(بفر), HTTP_PSEUDO_حالت ":%d\r",
                          دھارا->حالت_کوڈ);
    نتیجہ = Curl_سرخیاں_ڈالو(انکڑے, بفر, hlen, CURLH_PSEUDO);
    إذا(نتیجہ) {
      cf_h2_سرخی_error(cf, انکڑے, دھارا, نتیجہ);
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    curlx_dyn_reset(&ctx->scratch);
    نتیجہ = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/2 "));
    إذا(!نتیجہ)
      نتیجہ = curlx_dyn_addn(&ctx->scratch, قدر, valuelen);
    إذا(!نتیجہ)
      نتیجہ = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    إذا(!نتیجہ)
      h2_xfer_write_resp_hd(cf, انکڑے, دھارا, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    إذا(نتیجہ) {
      cf_h2_سرخی_error(cf, انکڑے, دھارا, نتیجہ);
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    /* if we receive data for another handle, wake that up */
    إذا(CF_انکڑے_CURRENT(cf) != انکڑے)
      Curl_multi_mark_dirty(انکڑے);

    CURL_TRC_CF(انکڑے, cf, "[%d] status: HTTP/2 %03d",
                دھارا->id, دھارا->حالت_کوڈ);
    أرجع 0;
  }

  /* nghttp2 guarantees that namelen > 0, and :status was already
     received, and this is not pseudo-header field . */
  /* convert to an HTTP1-style header */
  curlx_dyn_reset(&ctx->scratch);
  نتیجہ = curlx_dyn_addn(&ctx->scratch, (ثابت حرف *)name, namelen);
  إذا(!نتیجہ)
    نتیجہ = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
  إذا(!نتیجہ)
    نتیجہ = curlx_dyn_addn(&ctx->scratch, (ثابت حرف *)قدر, valuelen);
  إذا(!نتیجہ)
    نتیجہ = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
  إذا(!نتیجہ)
    h2_xfer_write_resp_hd(cf, انکڑے, دھارا, curlx_dyn_ptr(&ctx->scratch),
                          curlx_dyn_len(&ctx->scratch), FALSE);
  إذا(نتیجہ) {
    cf_h2_سرخی_error(cf, انکڑے, دھارا, نتیجہ);
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  /* if we receive data for another handle, wake that up */
  إذا(CF_انکڑے_CURRENT(cf) != انکڑے)
    Curl_multi_mark_dirty(انکڑے);

  CURL_TRC_CF(انکڑے, cf, "[%d] header: %.*s: %.*s",
              دھارا->id, (صحيح)namelen, name, (صحيح)valuelen, قدر);

  أرجع 0; /* 0 is successful */
}

ساكن ssize_t req_body_read_callback(nghttp2_session *session,
                                      int32_t دھارا_id,
                                      uint8_t *buf, size_t لمائی,
                                      uint32_t *انکڑے_flags,
                                      nghttp2_انکڑے_سروت *سروت,
                                      فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية Curl_easy *انکڑے_s;
  بنية h2_دھارا_ctx *دھارا = عدم;
  CURLcode نتیجہ;
  ssize_t nread;
  size_t n;
  (فراغ)سروت;

  (فراغ)session;
  إذا(!دھارا_id)
    أرجع NGHTTP2_ERR_INVALID_دلیل;

  /* get the stream from the hash based on Stream ID, stream ID zero is for
     connection-oriented stuff */
  انکڑے_s = cf_h2_get_دھارا_xfer(cf, دھارا_id, &دھارا);
  إذا(!انکڑے_s || !دھارا)
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;

  نتیجہ = Curl_bufq_read(&دھارا->sendbuf, buf, لمائی, &n);
  إذا(نتیجہ) {
    إذا(نتیجہ != CURLE_AGAIN)
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    nread = 0;
  }
  وإلا
    nread = (ssize_t)n;

  CURL_TRC_CF(انکڑے_s, cf, "[%d] req_body_read(len=%zu) eos=%d -> %zd, %d",
              دھارا_id, لمائی, دھارا->body_eos, nread, (صحيح)نتیجہ);

  إذا(دھارا->body_eos && Curl_bufq_is_خالی_تھاواں(&دھارا->sendbuf)) {
    *انکڑے_flags = NGHTTP2_انکڑے_FLAG_EOF;
    أرجع nread;
  }
  أرجع (nread == 0) ? NGHTTP2_ERR_DEFERRED : nread;
}

#جے_تعریف_شدہ CURLVERBOSE
ساكن صحيح error_callback(nghttp2_session *session,
                          ثابت حرف *پیغام,
                          size_t len,
                          فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية Curl_easy *انکڑے = CF_انکڑے_CURRENT(cf);
  (فراغ)session;
  failf(انکڑے, "%.*s", (صحيح)len, پیغام);
  أرجع 0;
}
#جے_انت

/*
 * Append headers to ask for an HTTP1.1 to HTTP2 upgrade.
 */
CURLcode Curl_http2_request_upgrade(بنية dynbuf *req,
                                    بنية Curl_easy *انکڑے)
{
  CURLcode نتیجہ;
  حرف *base64;
  size_t blen;
  بنية SingleRequest *k = &انکڑے->req;
  uint8_t binsettings[H2_BINSETTINGS_LEN];
  ssize_t rc;
  size_t binlen; /* length of the binsettings data */

  rc = populate_binsettings(binsettings, انکڑے);
  إذا(!curlx_sztouz(rc, &binlen) || !binlen) {
    failf(انکڑے, "nghttp2 unexpectedly failed on pack_settings_payload");
    curlx_dyn_free(req);
    أرجع CURLE_FAILED_INIT;
  }

  نتیجہ = curlx_base64url_encode(binsettings, binlen, &base64, &blen);
  إذا(نتیجہ) {
    curlx_dyn_free(req);
    أرجع نتیجہ;
  }

  انکڑے->state.http_hd_upgrade = TRUE;
  انکڑے->state.http_hd_h2_settings = TRUE;
  نتیجہ = curlx_dyn_addf(req,
                          "Upgrade: %s\r\n"
                          "HTTP2-Settings: %s\r\n",
                          NGHTTP2_CLEARTEXT_PROTO_نسخہ_ID, base64);
  curlx_free(base64);

  k->upgr101 = UPGR101_H2;
  انکڑے->conn->بٹ.upgrade_وچ_ترقی = TRUE;

  أرجع نتیجہ;
}

ساكن CURLcode http2_handle_دھارا_close(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *انکڑے,
                                          بنية h2_دھارا_ctx *دھارا,
                                          size_t *pnlen)
{
  CURLcode نتیجہ;

  *pnlen = 0;
  إذا(دھارا->reset) {
    إذا(دھارا->error == NGHTTP2_REFUSED_دھارا) {
      infof(انکڑے, "HTTP/2 stream %d refused by server, try again on a new "
                  "connection", دھارا->id);
      connclose(cf->conn); /* do not use this anymore */
      انکڑے->state.refused_دھارا = TRUE;
      أرجع CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    وإلا إذا(دھارا->resp_hds_complete && انکڑے->req.no_body) {
      CURL_TRC_CF(انکڑے, cf, "[%d] error after response headers, but we did "
                  "not want a body anyway, ignore: %s (err %u)",
                  دھارا->id, nghttp2_http2_strerror(دھارا->error),
                  دھارا->error);
      دھارا->close_handled = TRUE;
      أرجع CURLE_OK;
    }
    failf(انکڑے, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          دھارا->id, دھارا->reset_by_server ? "server" : "curl",
          دھارا->error, nghttp2_http2_strerror(دھارا->error));
    أرجع دھارا->error ? CURLE_HTTP2_دھارا :
           (انکڑے->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2);
  }
  وإلا إذا(!دھارا->bodystarted) {
    failf(انکڑے, "HTTP/2 stream %d was closed cleanly, but before getting "
          "all response header fields, treated as error", دھارا->id);
    أرجع CURLE_HTTP2_دھارا;
  }

  إذا(Curl_dynhds_گنتی(&دھارا->resp_trailers)) {
    بنية dynhds_اندراج *e;
    بنية dynbuf dbuf;
    size_t i;

    نتیجہ = CURLE_OK;
    curlx_dyn_init(&dbuf, DYN_TRAILERS);
    لكل(i = 0; i < Curl_dynhds_گنتی(&دھارا->resp_trailers); ++i) {
      e = Curl_dynhds_getn(&دھارا->resp_trailers, i);
      إذا(!e)
        اكسر;
      curlx_dyn_reset(&dbuf);
      نتیجہ = curlx_dyn_addf(&dbuf, "%.*s: %.*s\x0d\x0a",
                              (صحيح)e->namelen, e->name,
                              (صحيح)e->valuelen, e->قدر);
      إذا(نتیجہ)
        اكسر;
      Curl_debug(انکڑے, CURLINFO_سرخی_وچ, curlx_dyn_ptr(&dbuf),
                 curlx_dyn_len(&dbuf));
      نتیجہ = Curl_client_write(انکڑے,
                                 CLIENTWRITE_سرخی | CLIENTWRITE_TRAILER,
                                 curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf));
      إذا(نتیجہ)
        اكسر;
      نتیجہ = Curl_bump_headersize(انکڑے, curlx_dyn_len(&dbuf), FALSE);
      إذا(نتیجہ)
        أرجع نتیجہ;
    }
    curlx_dyn_free(&dbuf);
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  دھارا->close_handled = TRUE;
  نتیجہ = CURLE_OK;

باہرلا:
  CURL_TRC_CF(انکڑے, cf, "handle_stream_close -> %d, %zu", (صحيح)نتیجہ, *pnlen);
  أرجع نتیجہ;
}

ساكن صحيح sweight_wanted(ثابت بنية Curl_easy *انکڑے)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  أرجع انکڑے->سیٹ.بھار ?
    انکڑے->سیٹ.بھار : NGHTTP2_DEFAULT_بھار;
}

ساكن صحيح sweight_وچ_effect(ثابت بنية Curl_easy *انکڑے)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  أرجع انکڑے->state.بھار ?
    انکڑے->state.بھار : NGHTTP2_DEFAULT_بھار;
}

/*
 * h2_pri_spec() fills in the pri_spec struct, used by nghttp2 to send weight
 * and dependency to the peer. It also stores the updated values in the state
 * struct.
 */

ساكن فراغ h2_pri_spec(بنية Curl_easy *انکڑے,
                        nghttp2_ترجیح_spec *pri_spec)
{
  صحيح prio = انکڑے->سیٹ.بھار;
  nghttp2_ترجیح_spec_init(pri_spec, 0, sweight_wanted(انکڑے), FALSE);
  انکڑے->state.بھار = prio;
}

/*
 * Check if there is been an update in the priority /
 * dependency settings and if so it submits a PRIORITY frame with the updated
 * info.
 * Flush any out data pending in the network buffer.
 */
ساكن CURLcode h2_ترقی_egress(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *انکڑے)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  صحيح rv = 0;

  إذا(sweight_wanted(انکڑے) != sweight_وچ_effect(انکڑے)) {
    بنية h2_دھارا_ctx *دھارا = H2_دھارا_CTX(ctx, انکڑے);
    إذا(دھارا && دھارا->id > 0 &&
       (sweight_wanted(انکڑے) != sweight_وچ_effect(انکڑے))) {
      /* send new weight and/or dependency */
      nghttp2_ترجیح_spec pri_spec;

      h2_pri_spec(انکڑے, &pri_spec);
      CURL_TRC_CF(انکڑے, cf, "[%d] Queuing PRIORITY", دھارا->id);
      DEBUGASSERT(دھارا->id != -1);
      rv = Curl_ngh2_submit_ترجیح(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, دھارا->id, &pri_spec);
      إذا(rv)
        اذهب باہرلا;
    }
  }

  ctx->nw_باہرلا_blocked = 0;
  طالما(!rv && !ctx->nw_باہرلا_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

باہرلا:
  إذا(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(انکڑے, cf, "nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    أرجع CURLE_SEND_ERROR;
  }
  /* Defer flushing during the connect phase so that the SETTINGS and
   * other initial frames are sent together with the first request.
   * Unless we are 'connect_only' where the request will never come. */
  إذا(!cf->connected && !cf->conn->بٹ.connect_only)
    أرجع CURLE_OK;
  أرجع nw_باہرلا_flush(cf, انکڑے);
}

ساكن CURLcode دھارا_recv(بنية Curl_cfilter *cf, بنية Curl_easy *انکڑے,
                            بنية h2_دھارا_ctx *دھارا,
                            حرف *buf, size_t len, size_t *pnread)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_AGAIN;

  (فراغ)buf;
  (فراغ)len;
  *pnread = 0;

  إذا(!دھارا->xfer_نتیجہ)
    دھارا->xfer_نتیجہ = cf_h2_update_local_win(cf, انکڑے, دھارا);

  إذا(دھارا->xfer_نتیجہ) {
    CURL_TRC_CF(انکڑے, cf, "[%d] xfer write failed", دھارا->id);
    نتیجہ = دھارا->xfer_نتیجہ;
  }
  وإلا إذا(دھارا->closed) {
    CURL_TRC_CF(انکڑے, cf, "[%d] returning CLOSE", دھارا->id);
    نتیجہ = http2_handle_دھارا_close(cf, انکڑے, دھارا, pnread);
  }
  وإلا إذا(دھارا->reset ||
          (ctx->conn_closed && Curl_bufq_is_خالی_تھاواں(&ctx->inbufq)) ||
          (ctx->rcvd_goaway && ctx->remote_max_sid < دھارا->id)) {
    CURL_TRC_CF(انکڑے, cf, "[%d] returning ERR", دھارا->id);
    نتیجہ = انکڑے->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2;
  }

  إذا(نتیجہ && (نتیجہ != CURLE_AGAIN))
    CURL_TRC_CF(انکڑے, cf, "[%d] stream_recv(len=%zu) -> %d, %zu",
                دھارا->id, len, (صحيح)نتیجہ, *pnread);
  أرجع نتیجہ;
}

ساكن CURLcode h2_ترقی_ingress(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *انکڑے,
                                    size_t انکڑے_max_بائٹاں)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;
  size_t nread;

  إذا(should_close_session(ctx)) {
    CURL_TRC_CF(انکڑے, cf, "[0] ingress: session is closed");
    أرجع CURLE_HTTP2;
  }

  /* Process network input buffer first */
  إذا(!Curl_bufq_is_خالی_تھاواں(&ctx->inbufq)) {
    CURL_TRC_CF(انکڑے, cf, "Process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    نتیجہ = h2_process_pending_داخلہ(cf, انکڑے);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }

  إذا(!انکڑے_max_بائٹاں)
    انکڑے_max_بائٹاں = H2_CHUNK_ناپ;

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop due to connection close or us not processing
   * all network input */
  طالما(!ctx->conn_closed && Curl_bufq_is_خالی_تھاواں(&ctx->inbufq)) {
    بنية h2_دھارا_ctx *دھارا = H2_دھارا_CTX(ctx, انکڑے);
    إذا(دھارا && (دھارا->closed || !انکڑے_max_بائٹاں)) {
      /* We would like to abort here and stop processing, so that the transfer
       * loop can handle the data/close here. This may leave data in
       * underlying buffers that will not be consumed. */
      إذا(!cf->اگلا || !cf->اگلا->cft->has_انکڑے_pending(cf->اگلا, انکڑے))
        Curl_multi_mark_dirty(انکڑے);
      اكسر;
    }

    نتیجہ = Curl_cf_recv_bufq(cf->اگلا, انکڑے, &ctx->inbufq, 0, &nread);
    إذا(نتیجہ) {
      إذا(نتیجہ != CURLE_AGAIN) {
        failf(انکڑے, "Failed receiving HTTP2 data: %d(%s)", (صحيح)نتیجہ,
              curl_easy_strerror(نتیجہ));
        أرجع نتیجہ;
      }
      اكسر;
    }
    وإلا إذا(nread == 0) {
      CURL_TRC_CF(انکڑے, cf, "[0] ingress: connection closed");
      ctx->conn_closed = TRUE;
      اكسر;
    }
    وإلا {
      CURL_TRC_CF(انکڑے, cf, "[0] ingress: read %zu bytes", nread);
      انکڑے_max_بائٹاں = (انکڑے_max_بائٹاں > nread) ? (انکڑے_max_بائٹاں - nread) : 0;
    }

    نتیجہ = h2_process_pending_داخلہ(cf, انکڑے);
    إذا(نتیجہ)
      أرجع نتیجہ;
    CURL_TRC_CF(انکڑے, cf, "[0] ingress: nw-in buffered %zu",
                Curl_bufq_len(&ctx->inbufq));
  }

  إذا(ctx->conn_closed && Curl_bufq_is_خالی_تھاواں(&ctx->inbufq)) {
    CURL_TRC_CF(انکڑے, cf, "server closed %s",
                ctx->rcvd_goaway ? "with GOAWAY" : "abruptly");
    connclose(cf->conn);
  }

  CURL_TRC_CF(انکڑے, cf, "[0] ingress: done");
  أرجع CURLE_OK;
}

ساكن CURLcode cf_h2_recv(بنية Curl_cfilter *cf, بنية Curl_easy *انکڑے,
                           حرف *buf, size_t len, size_t *pnread)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_دھارا_ctx *دھارا;
  CURLcode نتیجہ, r2;
  بنية cf_call_انکڑے save;

  إذا(!انکڑے)
    أرجع CURLE_HTTP2;

  دھارا = H2_دھارا_CTX(ctx, انکڑے);

  *pnread = 0;
  إذا(!دھارا) {
    /* Abnormal call sequence: either this transfer has never opened a stream
     * (unlikely) or the transfer has been done, cleaned up its resources, but
     * a read() is called anyway. It is not clear what the calling sequence
     * is for such a case. */
    failf(انکڑے, "http/2 recv on a transfer never opened "
          "or already cleared, mid=%u", انکڑے->mid);
    أرجع CURLE_HTTP2;
  }

  CF_انکڑے_SAVE(save, cf, انکڑے);

  نتیجہ = دھارا_recv(cf, انکڑے, دھارا, buf, len, pnread);
  إذا(نتیجہ && (نتیجہ != CURLE_AGAIN))
    اذهب باہرلا;

  إذا(نتیجہ) {
    نتیجہ = h2_ترقی_ingress(cf, انکڑے, len);
    إذا(نتیجہ)
      اذهب باہرلا;

    نتیجہ = دھارا_recv(cf, انکڑے, دھارا, buf, len, pnread);
  }

  إذا(*pnread > 0) {
    /* Now that we transferred this to the upper layer, we report
     * the actual amount of DATA consumed to the H2 session, so
     * that it adjusts stream flow control */
    Curl_ngh2_session_consume(&ctx->ngh2, دھارا->id, *pnread);
    إذا(دھارا->closed) {
      CURL_TRC_CF(انکڑے, cf, "[%d] DRAIN closed stream", دھارا->id);
      Curl_multi_mark_dirty(انکڑے);
    }
  }

باہرلا:
  r2 = h2_ترقی_egress(cf, انکڑے);
  إذا(r2 == CURLE_AGAIN) {
    /* pending data to send, need to be called again. Ideally, we
     * monitor the socket for POLLOUT, but when not SENDING
     * any more, we force processing of the transfer. */
    إذا(!CURL_REQ_WANT_SEND(انکڑے))
      Curl_multi_mark_dirty(انکڑے);
  }
  وإلا إذا(r2 && (نتیجہ == CURLE_AGAIN)) {
    /* Do not replace a previous error result NOR a successful result
     * with an egress error. Example: the server might close the
     * connection after sending the complete response with EOS
     * and everything. Further sending may fail, but that MUST
     * not change that we had a successful response. */
    نتیجہ = r2;
  }

  CURL_TRC_CF(انکڑے, cf, "[%d] cf_recv(len=%zu) -> %d, %zu, "
              "window=%d/%d, connection %d/%d",
              دھارا->id, len, (صحيح)نتیجہ, *pnread,
              Curl_ngh2_session_get_دھارا_effective_recv_انکڑے_لمائی(
                &ctx->ngh2, دھارا->id),
              Curl_ngh2_session_get_دھارا_effective_local_کھڑکی_ناپ(
                &ctx->ngh2, دھارا->id),
              Curl_ngh2_session_get_local_کھڑکی_ناپ(&ctx->ngh2),
              HTTP2_HUGE_کھڑکی_ناپ);

  CF_انکڑے_RESTORE(cf, save);
  أرجع نتیجہ;
}

ساكن CURLcode cf_h2_body_send(بنية Curl_cfilter *cf,
                                بنية Curl_easy *انکڑے,
                                بنية h2_دھارا_ctx *دھارا,
                                ثابت فراغ *buf, size_t blen, bool eos,
                                size_t *pnwritten)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  CURLcode نتیجہ;

  *pnwritten = 0;
  إذا(دھارا->closed) {
    إذا(دھارا->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(انکڑے, cf, "[%d] discarding data"
                  "on closed stream with response", دھارا->id);
      إذا(eos)
        دھارا->body_eos = TRUE;
      *pnwritten = blen;
      أرجع CURLE_OK;
    }
    /* Server closed before we got a response, this is an error */
    infof(انکڑے, "stream %d closed", دھارا->id);
    أرجع CURLE_SEND_ERROR;
  }

  نتیجہ = Curl_bufq_write(&دھارا->sendbuf, buf, blen, pnwritten);
  إذا(نتیجہ)
    أرجع نتیجہ;

  إذا(eos && (blen == *pnwritten))
    دھارا->body_eos = TRUE;

  إذا(eos || !Curl_bufq_is_خالی_تھاواں(&دھارا->sendbuf)) {
    /* resume the potentially suspended stream */
    إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_انکڑے(&ctx->ngh2, دھارا->id)))
      أرجع CURLE_SEND_ERROR;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode h2_submit(بنية h2_دھارا_ctx **pstream,
                          بنية Curl_cfilter *cf, بنية Curl_easy *انکڑے,
                          ثابت uint8_t *buf, size_t len,
                          bool eos, size_t *pnwritten)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_دھارا_ctx *دھارا = عدم;
  بنية dynhds h2_سرخیاں;
  nghttp2_nv *nva = عدم;
  ثابت فراغ *body = عدم;
  size_t nheader, bodylen;
  nghttp2_انکڑے_provider انکڑے_prd;
  int32_t دھارا_id;
  nghttp2_ترجیح_spec pri_spec;
  size_t nwritten;
  CURLcode نتیجہ = CURLE_OK;
  uint32_t initial_win_ناپ;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_سرخیاں, MAX_HTTP_RESP_سرخی_گنتی, DYN_HTTP_REQUEST);

  نتیجہ = http2_انکڑے_تیاری(cf, انکڑے, &دھارا);
  إذا(نتیجہ)
    اذهب باہرلا;

  نتیجہ = Curl_h1_req_parse_read(
    &دھارا->h1, buf, len, عدم,
    !انکڑے->state.http_ignorecustom ?
    CURL_EASY_STR(انکڑے, STRING_CUSTOMREQUEST) : عدم,
    0, &nwritten);
  إذا(نتیجہ)
    اذهب باہرلا;
  *pnwritten = nwritten;
  إذا(!دھارا->h1.پورا) {
    /* need more data */
    اذهب باہرلا;
  }
  DEBUGASSERT(دھارا->h1.req);

  نتیجہ = Curl_http_req_منزل_h2(&h2_سرخیاں, دھارا->h1.req, انکڑے);
  إذا(نتیجہ)
    اذهب باہرلا;
  /* no longer needed */
  Curl_h1_req_parse_free(&دھارا->h1);

  nva = Curl_dynhds_منزل_nva(&h2_سرخیاں, &nheader);
  إذا(!nva) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }

  h2_pri_spec(انکڑے, &pri_spec);
  إذا(!Curl_ngh2_session_check_request_allowed(&ctx->ngh2))
    CURL_TRC_CF(انکڑے, cf, "send request NOT allowed (via nghttp2)");

  /* Check the initial windows size of the transfer (rate-limits?) and
   * send an updated settings on changes from previous value. */
  initial_win_ناپ = cf_h2_initial_win_ناپ(انکڑے);
  إذا(initial_win_ناپ != ctx->initial_win_ناپ) {
    نتیجہ = cf_h2_update_settings(ctx, initial_win_ناپ);
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  بدل(انکڑے->state.httpreq) {
  حالة HTTPREQ_POST:
  حالة HTTPREQ_POST_FORM:
  حالة HTTPREQ_POST_MIME:
  حالة HTTPREQ_PUT:
    انکڑے_prd.read_callback = req_body_read_callback;
    انکڑے_prd.سروت.ptr = عدم;
    دھارا_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, &انکڑے_prd, دھارا);
    اكسر;
  افتراضي:
    دھارا_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, عدم, دھارا);
  }

  إذا(دھارا_id < 0) {
    CURL_TRC_CF(انکڑے, cf, "send: nghttp2_submit_request error (%s)%d",
                nghttp2_strerror(دھارا_id), دھارا_id);
    نتیجہ = CURLE_SEND_ERROR;
    اذهب باہرلا;
  }

#جے_تعریف_شدہ CURLVERBOSE
#تعریف_کرو MAX_ACC 60000  /* <64KB to account for some overhead */
  إذا(Curl_trc_is_verbose(انکڑے)) {
    size_t acc = 0, i;

    infof(انکڑے, "[HTTP/2] [%d] OPENED stream for %s",
          دھارا_id, Curl_bufref_ptr(&انکڑے->state.url));
    لكل(i = 0; i < nheader; ++i) {
      acc += nva[i].namelen + nva[i].valuelen;

      infof(انکڑے, "[HTTP/2] [%d] [%.*s: %.*s]", دھارا_id,
            (صحيح)nva[i].namelen, nva[i].name,
            (صحيح)nva[i].valuelen, nva[i].قدر);
    }

    إذا(acc > MAX_ACC) {
      infof(انکڑے, "[HTTP/2] Warning: The cumulative length of all "
            "headers exceeds %d bytes and that could cause the "
            "stream to be rejected.", MAX_ACC);
    }
  }
#جے_انت

  دھارا->id = دھارا_id;

  body = (ثابت حرف *)buf + *pnwritten;
  bodylen = len - *pnwritten;

  إذا(bodylen || eos) {
    size_t n;
    نتیجہ = cf_h2_body_send(cf, انکڑے, دھارا, body, bodylen, eos, &n);
    إذا(!نتیجہ)
      *pnwritten += n;
    وإلا إذا(نتیجہ == CURLE_AGAIN)
      نتیجہ = CURLE_OK;
    وإلا {
      نتیجہ = CURLE_SEND_ERROR;
    }
  }

باہرلا:
  CURL_TRC_CF(انکڑے, cf, "[%d] submit -> %d, %zu",
              دھارا ? دھارا->id : -1, (صحيح)نتیجہ, *pnwritten);
  curlx_safefree(nva);
  *pstream = دھارا;
  Curl_dynhds_free(&h2_سرخیاں);
  أرجع نتیجہ;
}

ساكن CURLcode cf_h2_send(بنية Curl_cfilter *cf, بنية Curl_easy *انکڑے,
                           ثابت uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_دھارا_ctx *دھارا = H2_دھارا_CTX(ctx, انکڑے);
  بنية cf_call_انکڑے save;
  CURLcode نتیجہ = CURLE_OK, r2;

  CF_انکڑے_SAVE(save, cf, انکڑے);
  *pnwritten = 0;

  إذا(!دھارا || دھارا->id == -1) {
    نتیجہ = h2_submit(&دھارا, cf, انکڑے, buf, len, eos, pnwritten);
    إذا(نتیجہ)
      اذهب باہرلا;
    DEBUGASSERT(دھارا);
  }
  وإلا إذا(دھارا->body_eos) {
    /* We already wrote this, but CURLE_AGAIN-ed the call due to not
     * being able to flush stream->sendbuf. Make a 0-length write
     * to trigger flushing again.
     * If this works, we report to have written `len` bytes. */
    size_t n;
    DEBUGASSERT(eos);
    نتیجہ = cf_h2_body_send(cf, انکڑے, دھارا, buf, 0, eos, &n);
    CURL_TRC_CF(انکڑے, cf, "[%d] cf_body_send last CHUNK -> %d, %zu, eos=%d",
                دھارا->id, (صحيح)نتیجہ, n, eos);
    إذا(نتیجہ)
      اذهب باہرلا;
    *pnwritten = len;
  }
  وإلا {
    نتیجہ = cf_h2_body_send(cf, انکڑے, دھارا, buf, len, eos, pnwritten);
    CURL_TRC_CF(انکڑے, cf, "[%d] cf_body_send(len=%zu) -> %d, %zu, eos=%d",
                دھارا->id, len, (صحيح)نتیجہ, *pnwritten, eos);
  }

  /* Call the nghttp2 send loop and flush to write ALL buffered data,
   * headers and/or request body completely out to the network */
  r2 = h2_ترقی_egress(cf, انکڑے);

  /* if the stream has been closed in egress handling (nghttp2 does that
   * when it does not like the headers, for example */
  إذا(دھارا && دھارا->closed && !دھارا->resp_hds_complete) {
    infof(انکڑے, "stream %d closed", دھارا->id);
    نتیجہ = CURLE_SEND_ERROR;
    اذهب باہرلا;
  }
  وإلا إذا(r2 && (r2 != CURLE_AGAIN)) {
    نتیجہ = r2;
    اذهب باہرلا;
  }

  إذا(should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    إذا(دھارا && دھارا->closed) {
      نتیجہ = http2_handle_دھارا_close(cf, انکڑے, دھارا, pnwritten);
    }
    وإلا {
      CURL_TRC_CF(انکڑے, cf, "send: nothing to do in this session");
      نتیجہ = CURLE_HTTP2;
    }
  }

باہرلا:
  إذا(دھارا) {
    CURL_TRC_CF(انکڑے, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
                "eos=%d, h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                دھارا->id, len, (صحيح)نتیجہ, *pnwritten,
                دھارا->body_eos,
                Curl_ngh2_session_get_دھارا_remote_کھڑکی_ناپ(
                  &ctx->ngh2, دھارا->id),
                Curl_ngh2_session_get_remote_کھڑکی_ناپ(&ctx->ngh2),
                Curl_bufq_len(&دھارا->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  وإلا {
    CURL_TRC_CF(انکڑے, cf, "cf_send(len=%zu) -> %d, %zu, "
                "connection-window=%d, nw_send_buffer(%zu)",
                len, (صحيح)نتیجہ, *pnwritten,
                Curl_ngh2_session_get_remote_کھڑکی_ناپ(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_انکڑے_RESTORE(cf, save);
  أرجع نتیجہ;
}

ساكن CURLcode cf_h2_flush(بنية Curl_cfilter *cf,
                            بنية Curl_easy *انکڑے)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_دھارا_ctx *دھارا = H2_دھارا_CTX(ctx, انکڑے);
  بنية cf_call_انکڑے save;
  CURLcode نتیجہ = CURLE_OK;

  CF_انکڑے_SAVE(save, cf, انکڑے);
  إذا(دھارا && !Curl_bufq_is_خالی_تھاواں(&دھارا->sendbuf)) {
    /* resume the potentially suspended stream */
    إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_انکڑے(&ctx->ngh2, دھارا->id))) {
      نتیجہ = CURLE_SEND_ERROR;
      اذهب باہرلا;
    }
  }

  نتیجہ = h2_ترقی_egress(cf, انکڑے);

باہرلا:
  إذا(دھارا) {
    CURL_TRC_CF(انکڑے, cf, "[%d] flush -> %d, "
                "h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                دھارا->id, (صحيح)نتیجہ,
                Curl_ngh2_session_get_دھارا_remote_کھڑکی_ناپ(
                  &ctx->ngh2, دھارا->id),
                Curl_ngh2_session_get_remote_کھڑکی_ناپ(&ctx->ngh2),
                Curl_bufq_len(&دھارا->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  وإلا {
    CURL_TRC_CF(انکڑے, cf, "flush -> %d, "
                "connection-window=%d, nw_send_buffer(%zu)", (صحيح)نتیجہ,
                Curl_ngh2_session_get_remote_کھڑکی_ناپ(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_انکڑے_RESTORE(cf, save);
  أرجع نتیجہ;
}

ساكن CURLcode cf_h2_adjust_pollset(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *انکڑے,
                                     بنية easy_pollset *ps)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية cf_call_انکڑے save;
  curl_socket_t sock;
  bool want_recv, want_send;
  CURLcode نتیجہ = CURLE_OK;

  إذا(!Curl_ngh2_valid(&ctx->ngh2))
    أرجع CURLE_OK;

  sock = Curl_conn_cf_get_socket(cf, انکڑے);
  Curl_pollset_check(انکڑے, ps, sock, &want_recv, &want_send);
  إذا(want_recv || want_send) {
    بنية h2_دھارا_ctx *دھارا = H2_دھارا_CTX(ctx, انکڑے);
    bool c_exhaust, s_exhaust;

    CF_انکڑے_SAVE(save, cf, انکڑے);
    c_exhaust = want_send &&
                !Curl_ngh2_session_get_remote_کھڑکی_ناپ(&ctx->ngh2);
    s_exhaust = want_send && دھارا && دھارا->id >= 0 &&
                !Curl_ngh2_session_get_دھارا_remote_کھڑکی_ناپ(
                  &ctx->ngh2, دھارا->id);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                (!c_exhaust && Curl_ngh2_session_want_write(&ctx->ngh2)) ||
                !Curl_bufq_is_خالی_تھاواں(&ctx->outbufq);

    نتیجہ = Curl_pollset_سیٹ(انکڑے, ps, sock, want_recv, want_send);
    CF_انکڑے_RESTORE(cf, save);
  }
  وإلا إذا(ctx->گھلیا_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_انکڑے_SAVE(save, cf, انکڑے);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_خالی_تھاواں(&ctx->outbufq);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    نتیجہ = Curl_pollset_سیٹ(انکڑے, ps, sock, want_recv, want_send);
    CF_انکڑے_RESTORE(cf, save);
  }
  أرجع نتیجہ;
}

ساكن CURLcode cf_h2_ctx_open(بنية Curl_cfilter *cf,
                               بنية Curl_easy *انکڑے)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_دھارا_ctx *دھارا;
  CURLcode نتیجہ = CURLE_باہرلا_دا_حافظہ;
  صحيح rc;
  nghttp2_session_callbacks *cbs = عدم;

  DEBUGASSERT(!ctx->ngh2.s);
  DEBUGASSERT(!ctx->ngh2.fatal_rv);
  DEBUGASSERT(ctx->initialized);

  rc = nghttp2_session_callbacks_new(&cbs);
  إذا(rc) {
    failf(انکڑے, "Could not initialize nghttp2 callbacks");
    اذهب باہرلا;
  }

  nghttp2_session_callbacks_سیٹ_send_callback(cbs, send_callback);
  nghttp2_session_callbacks_سیٹ_on_چوکھٹا_recv_callback(cbs, on_چوکھٹا_recv);
  nghttp2_session_callbacks_سیٹ_on_invalid_چوکھٹا_recv_callback(cbs,
    cf_h2_on_invalid_چوکھٹا_recv);
#جے_تعریف_شدہ CURLVERBOSE
  nghttp2_session_callbacks_سیٹ_on_چوکھٹا_send_callback(cbs, on_چوکھٹا_send);
#جے_انت
  nghttp2_session_callbacks_سیٹ_on_انکڑے_chunk_recv_callback(
    cbs, on_انکڑے_chunk_recv);
  nghttp2_session_callbacks_سیٹ_on_دھارا_close_callback(cbs, on_دھارا_close);
  nghttp2_session_callbacks_سیٹ_on_سرخی_callback(cbs, on_سرخی);
#جے_تعریف_شدہ CURLVERBOSE
  nghttp2_session_callbacks_سیٹ_error_callback(cbs, error_callback);
#جے_انت

  /* The nghttp2 session is not yet setup, do it */
  rc = h2_client_new(cf, cbs);
  إذا(rc) {
    failf(انکڑے, "Could not initialize nghttp2");
    اذهب باہرلا;
  }
  ctx->max_concurrent_streams = انکڑے->multi ?
    Curl_multi_max_concurrent_streams(انکڑے->multi) :
    DEFAULT_MAX_CONCURRENT_STREAMS;

  إذا(ctx->راہیں_h1_upgrade) {
    /* HTTP/1.1 Upgrade issued. H2 Settings have already been submitted
     * in the H1 request and we upgrade from there. This stream
     * is opened implicitly as #1. */
    uint8_t binsettings[H2_BINSETTINGS_LEN];
    ssize_t rclen;
    size_t binlen; /* length of the binsettings data */

    rclen = populate_binsettings(binsettings, انکڑے);

    إذا(!curlx_sztouz(rclen, &binlen) || !binlen) {
      failf(انکڑے, "nghttp2 unexpectedly failed on pack_settings_payload");
      نتیجہ = CURLE_FAILED_INIT;
      اذهب باہرلا;
    }

    نتیجہ = http2_انکڑے_تیاری(cf, انکڑے, &دھارا);
    إذا(نتیجہ)
      اذهب باہرلا;
    DEBUGASSERT(دھارا);
    دھارا->id = 1;
    /* queue SETTINGS frame (again) */
    rc = Curl_ngh2_session_upgrade2(
      &ctx->ngh2, binsettings, binlen, انکڑے->state.httpreq == HTTPREQ_سر,
      عدم);
    إذا(rc) {
      failf(انکڑے, "nghttp2_session_upgrade2() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      نتیجہ = CURLE_HTTP2;
      اذهب باہرلا;
    }

    rc = Curl_ngh2_session_سیٹ_دھارا_user_انکڑے(
      &ctx->ngh2, دھارا->id, دھارا);
    إذا(rc) {
      infof(انکڑے, "http/2: failed to set user_data for stream %d",
            دھارا->id);
      DEBUGASSERT(0);
    }
    CURL_TRC_CF(انکڑے, cf, "created session via Upgrade");
  }
  وإلا {
    nghttp2_settings_اندراج iv[H2_SETTINGS_IV_LEN];
    size_t ivlen;

    ivlen = populate_settings(iv, انکڑے, ctx);
    rc = Curl_ngh2_submit_settings(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, iv, ivlen);
    إذا(rc) {
      failf(انکڑے, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      نتیجہ = CURLE_HTTP2;
      اذهب باہرلا;
    }
  }

  rc = Curl_ngh2_session_سیٹ_local_کھڑکی_ناپ(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, HTTP2_HUGE_کھڑکی_ناپ);
  إذا(rc) {
    failf(انکڑے, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    نتیجہ = CURLE_HTTP2;
    اذهب باہرلا;
  }

  /* all set, traffic will be send on connect */
  نتیجہ = CURLE_OK;
  CURL_TRC_CF(انکڑے, cf, "[0] created h2 session%s",
              ctx->راہیں_h1_upgrade ? " (via h1 upgrade)" : "");

باہرلا:
  إذا(cbs)
    nghttp2_session_callbacks_del(cbs);
  أرجع نتیجہ;
}

ساكن CURLcode cf_h2_connect(بنية Curl_cfilter *cf,
                              بنية Curl_easy *انکڑے,
                              bool *پورا)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;
  بنية cf_call_انکڑے save;
  bool پہلا_ویلا = FALSE;

  إذا(cf->connected) {
    *پورا = TRUE;
    أرجع CURLE_OK;
  }

  /* Connect the lower filters first */
  إذا(!cf->اگلا->connected) {
    نتیجہ = Curl_conn_cf_connect(cf->اگلا, انکڑے, پورا);
    إذا(نتیجہ || !*پورا)
      أرجع نتیجہ;
  }

  *پورا = FALSE;

  CF_انکڑے_SAVE(save, cf, انکڑے);
  DEBUGASSERT(ctx->initialized);
  إذا(!ctx->ngh2.s) {
    نتیجہ = cf_h2_ctx_open(cf, انکڑے);
    إذا(نتیجہ)
      اذهب باہرلا;
    پہلا_ویلا = TRUE;
  }

  إذا(!پہلا_ویلا) {
    نتیجہ = h2_ترقی_ingress(cf, انکڑے, 0);
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  /* Send out our SETTINGS and ACKs and such. If that blocks, we
   * have it buffered and can count this filter as being connected */
  نتیجہ = h2_ترقی_egress(cf, انکڑے);
  إذا(نتیجہ && (نتیجہ != CURLE_AGAIN))
    اذهب باہرلا;

  *پورا = TRUE;
  cf->connected = TRUE;
  نتیجہ = CURLE_OK;

باہرلا:
  CURL_TRC_CF(انکڑے, cf, "cf_connect() -> %d, %d, ", (صحيح)نتیجہ, *پورا);
  CF_انکڑے_RESTORE(cf, save);
  أرجع نتیجہ;
}

ساكن فراغ cf_h2_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *انکڑے)
{
  بنية cf_h2_ctx *ctx = cf->ctx;

  (فراغ)انکڑے;
  إذا(ctx) {
    cf_h2_ctx_free(ctx);
    cf->ctx = عدم;
  }
}

ساكن CURLcode cf_h2_shutdown(بنية Curl_cfilter *cf,
                               بنية Curl_easy *انکڑے, bool *پورا)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية cf_call_انکڑے save;
  CURLcode نتیجہ;
  صحيح rv;

  إذا(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *پورا = TRUE;
    أرجع CURLE_OK;
  }

  CF_انکڑے_SAVE(save, cf, انکڑے);

  إذا(!ctx->گھلیا_goaway) {
    ctx->گھلیا_goaway = TRUE;
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, ctx->local_max_sid, 0,
      (ثابت uint8_t *)"shutdown", حجم("shutdown"));
    إذا(rv) {
      failf(انکڑے, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      نتیجہ = CURLE_SEND_ERROR;
      اذهب باہرلا;
    }
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  نتیجہ = CURLE_OK;
  إذا(Curl_ngh2_session_want_write(&ctx->ngh2) ||
     !Curl_bufq_is_خالی_تھاواں(&ctx->outbufq))
    نتیجہ = h2_ترقی_egress(cf, انکڑے);
  إذا(!نتیجہ && Curl_ngh2_session_want_read(&ctx->ngh2))
    نتیجہ = h2_ترقی_ingress(cf, انکڑے, 0);

  إذا(نتیجہ == CURLE_AGAIN)
    نتیجہ = CURLE_OK;

  *پورا = (ctx->conn_closed ||
           (!نتیجہ && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2) &&
            Curl_bufq_is_خالی_تھاواں(&ctx->outbufq)));

باہرلا:
  CF_انکڑے_RESTORE(cf, save);
  cf->shutdown = (نتیجہ || *پورا);
  أرجع نتیجہ;
}

ساكن CURLcode http2_انکڑے_pause(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *انکڑے,
                                 bool pause)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_دھارا_ctx *دھارا = H2_دھارا_CTX(ctx, انکڑے);

  DEBUGASSERT(انکڑے);
  إذا(ctx && Curl_ngh2_valid(&ctx->ngh2) && دھارا) {
    CURLcode نتیجہ;

    دھارا->write_paused = pause;
    نتیجہ = cf_h2_update_local_win(cf, انکڑے, دھارا);
    إذا(نتیجہ)
      أرجع نتیجہ;

    /* attempt to send the window update */
    (فراغ)h2_ترقی_egress(cf, انکڑے);

    إذا(!pause) {
      /* Unpausing a h2 transfer, requires it to be run again. The server
       * may send new DATA on us increasing the flow window, and it may
       * not. We may have already buffered and exhausted the new window
       * by operating on things in flight during the handling of other
       * transfers. */
      Curl_multi_mark_dirty(انکڑے);
    }
    CURL_TRC_CF(انکڑے, cf, "[%d] stream now %spaused", دھارا->id,
                pause ? "" : "un");
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cf_h2_cntrl(بنية Curl_cfilter *cf,
                            بنية Curl_easy *انکڑے,
                            صحيح event, صحيح arg1, فراغ *arg2)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية cf_call_انکڑے save;

  (فراغ)arg2;

  CF_انکڑے_SAVE(save, cf, انکڑے);
  بدل(event) {
  حالة CF_CTRL_انکڑے_تیاری:
    اكسر;
  حالة CF_CTRL_انکڑے_PAUSE:
    نتیجہ = http2_انکڑے_pause(cf, انکڑے, (arg1 != 0));
    اكسر;
  حالة CF_CTRL_FLUSH:
    نتیجہ = cf_h2_flush(cf, انکڑے);
    اكسر;
  حالة CF_CTRL_انکڑے_پورا:
    http2_انکڑے_پورا(cf, انکڑے);
    اكسر;
  حالة CF_CTRL_CONN_INFO_UPDATE:
    إذا(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_تکیا = 20;
      Curl_conn_سیٹ_multiplex(cf->conn);
    }
    اكسر;
  افتراضي:
    اكسر;
  }
  CF_انکڑے_RESTORE(cf, save);
  أرجع نتیجہ;
}

ساكن bool cf_h2_انکڑے_pending(بنية Curl_cfilter *cf,
                               ثابت بنية Curl_easy *انکڑے)
{
  بنية cf_h2_ctx *ctx = cf->ctx;

  إذا(ctx && !Curl_bufq_is_خالی_تھاواں(&ctx->inbufq))
    أرجع TRUE;
  أرجع cf->اگلا ? cf->اگلا->cft->has_انکڑے_pending(cf->اگلا, انکڑے) : FALSE;
}

ساكن bool cf_h2_is_alive(بنية Curl_cfilter *cf,
                           بنية Curl_easy *انکڑے,
                           bool *داخلہ_pending)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  bool alive;
  بنية cf_call_انکڑے save;

  *داخلہ_pending = FALSE;
  CF_انکڑے_SAVE(save, cf, انکڑے);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
           http2_connisalive(cf, انکڑے, داخلہ_pending));
  CURL_TRC_CF(انکڑے, cf, "conn alive -> %d, input_pending=%d",
              alive, *داخلہ_pending);
  CF_انکڑے_RESTORE(cf, save);
  أرجع alive;
}

ساكن CURLcode cf_h2_keep_alive(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *انکڑے)
{
  CURLcode نتیجہ;
  بنية cf_call_انکڑے save;

  CF_انکڑے_SAVE(save, cf, انکڑے);
  نتیجہ = http2_send_ping(cf, انکڑے);
  CF_انکڑے_RESTORE(cf, save);
  أرجع نتیجہ;
}

ساكن CURLcode cf_h2_query(بنية Curl_cfilter *cf,
                            بنية Curl_easy *انکڑے,
                            صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية cf_call_انکڑے save;
  size_t effective_max;

  بدل(query) {
  حالة CF_QUERY_MAX_CONCURRENT:
    DEBUGASSERT(pres1);

    CF_انکڑے_SAVE(save, cf, انکڑے);
    إذا(!Curl_ngh2_valid(&ctx->ngh2) ||
       !Curl_ngh2_session_check_request_allowed(&ctx->ngh2)) {
      /* the limit is what we have in use right now */
      effective_max = cf->conn->attached_xfers;
    }
    وإلا {
      effective_max = ctx->max_concurrent_streams;
    }
    *pres1 = (effective_max > ودھ_توں_ودھ_عدد) ? ودھ_توں_ودھ_عدد : (صحيح)effective_max;
    CF_انکڑے_RESTORE(cf, save);
    أرجع CURLE_OK;
  حالة CF_QUERY_دھارا_ERROR: {
    بنية h2_دھارا_ctx *دھارا = H2_دھارا_CTX(ctx, انکڑے);
    *pres1 = دھارا ? (صحيح)دھارا->error : 0;
    أرجع CURLE_OK;
  }
  حالة CF_QUERY_NEED_FLUSH: {
    بنية h2_دھارا_ctx *دھارا = H2_دھارا_CTX(ctx, انکڑے);
    إذا(!Curl_bufq_is_خالی_تھاواں(&ctx->outbufq) ||
       (دھارا && !Curl_bufq_is_خالی_تھاواں(&دھارا->sendbuf))) {
      *pres1 = TRUE;
      أرجع CURLE_OK;
    }
    اكسر;
  }
  حالة CF_QUERY_HTTP_نسخہ:
    *pres1 = 20;
    أرجع CURLE_OK;
  افتراضي:
    اكسر;
  }
  أرجع cf->اگلا ?
    cf->اگلا->cft->query(cf->اگلا, انکڑے, query, pres1, pres2) :
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
  cf_h2_انکڑے_pending,
  cf_h2_send,
  cf_h2_recv,
  cf_h2_cntrl,
  cf_h2_is_alive,
  cf_h2_keep_alive,
  cf_h2_query,
};

ساكن CURLcode http2_cfilter_جوڑو(بنية Curl_cfilter **pcf,
                                  بنية Curl_easy *انکڑے,
                                  بنية connectdata *conn,
                                  int8_t sockindex,
                                  bool راہیں_h1_upgrade)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_h2_ctx *ctx;
  CURLcode نتیجہ = CURLE_باہرلا_دا_حافظہ;

  DEBUGASSERT(انکڑے->conn);
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx)
    اذهب باہرلا;
  cf_h2_ctx_init(ctx, راہیں_h1_upgrade);

  نتیجہ = Curl_cf_create(&cf, &Curl_cft_nghttp2, ctx);
  إذا(نتیجہ)
    اذهب باہرلا;

  ctx = عدم;
  Curl_conn_cf_جوڑو(انکڑے, conn, sockindex, cf);

باہرلا:
  إذا(نتیجہ)
    cf_h2_ctx_free(ctx);
  *pcf = نتیجہ ? عدم : cf;
  أرجع نتیجہ;
}

ساكن CURLcode http2_cfilter_داخل_کرو_after(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *انکڑے,
                                           bool راہیں_h1_upgrade)
{
  بنية Curl_cfilter *cf_h2 = عدم;
  بنية cf_h2_ctx *ctx;
  CURLcode نتیجہ = CURLE_باہرلا_دا_حافظہ;

  (فراغ)انکڑے;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx)
    اذهب باہرلا;
  cf_h2_ctx_init(ctx, راہیں_h1_upgrade);

  نتیجہ = Curl_cf_create(&cf_h2, &Curl_cft_nghttp2, ctx);
  إذا(نتیجہ)
    اذهب باہرلا;

  ctx = عدم;
  Curl_conn_cf_داخل_کرو_after(cf, cf_h2);

باہرلا:
  إذا(نتیجہ)
    cf_h2_ctx_free(ctx);
  أرجع نتیجہ;
}

bool Curl_http2_may_switch(بنية Curl_easy *انکڑے)
{
  إذا(Curl_conn_http_نسخہ(انکڑے, انکڑے->conn) < 20 &&
     (انکڑے->state.http_neg.wanted & CURL_HTTP_V2x) &&
     انکڑے->state.http_neg.h2_پہلوں_knowledge) {
#جے_بنا_تعریف CURL_DISABLE_PROXY
    إذا(انکڑے->conn->بٹ.origin_is_proxy) {
      infof(انکڑے, "Ignoring HTTP/2 prior knowledge due to proxy");
      أرجع FALSE;
    }
#جے_انت
    أرجع TRUE;
  }
  أرجع FALSE;
}

CURLcode Curl_http2_switch(بنية Curl_easy *انکڑے)
{
  بنية Curl_cfilter *cf;
  CURLcode نتیجہ;

  DEBUGASSERT(Curl_conn_http_نسخہ(انکڑے, انکڑے->conn) < 20);

  نتیجہ = http2_cfilter_جوڑو(&cf, انکڑے, انکڑے->conn, FIRSTSOCKET, FALSE);
  إذا(نتیجہ)
    أرجع نتیجہ;
  CURL_TRC_CF(انکڑے, cf, "switching connection to HTTP/2");

  إذا(cf->اگلا) {
    bool پورا;
    أرجع Curl_conn_cf_connect(cf, انکڑے, &پورا);
  }
  أرجع CURLE_OK;
}

CURLcode Curl_http2_switch_at(بنية Curl_cfilter *cf, بنية Curl_easy *انکڑے)
{
  بنية Curl_cfilter *cf_h2;
  CURLcode نتیجہ;

  DEBUGASSERT(Curl_conn_http_نسخہ(انکڑے, انکڑے->conn) < 20);

  نتیجہ = http2_cfilter_داخل_کرو_after(cf, انکڑے, FALSE);
  إذا(نتیجہ)
    أرجع نتیجہ;

  cf_h2 = cf->اگلا;

  إذا(cf_h2->اگلا) {
    bool پورا;
    أرجع Curl_conn_cf_connect(cf_h2, انکڑے, &پورا);
  }
  أرجع CURLE_OK;
}

CURLcode Curl_http2_upgrade(بنية Curl_easy *انکڑے,
                            بنية connectdata *conn, int8_t sockindex,
                            ثابت حرف *mem, size_t nread)
{
  بنية Curl_cfilter *cf;
  بنية cf_h2_ctx *ctx;
  CURLcode نتیجہ;

  DEBUGASSERT(Curl_conn_http_نسخہ(انکڑے, conn) < 20);

  نتیجہ = http2_cfilter_جوڑو(&cf, انکڑے, conn, sockindex, TRUE);
  إذا(نتیجہ)
    أرجع نتیجہ;
  CURL_TRC_CF(انکڑے, cf, "upgrading connection to HTTP/2");

  DEBUGASSERT(cf->cft == &Curl_cft_nghttp2);
  ctx = cf->ctx;

  انکڑے->req.httpversion_گھلیا = 20; /* it is an h2 request now */
  انکڑے->req.سرخی = TRUE; /* we expect the real response to come in h2 */
  انکڑے->req.headerline = 0; /* restart the header line counter */

  إذا(nread > 0) {
    /* Remaining data from the protocol switch reply is already using
     * the switched protocol, ie. HTTP/2. We add that to the network
     * inbufq. */
    size_t copied;

    نتیجہ = Curl_bufq_write(&ctx->inbufq,
                             (ثابت بنانشانی حرف *)mem, nread, &copied);
    إذا(نتیجہ) {
      failf(انکڑے, "error on copying HTTP Upgrade response: %d", (صحيح)نتیجہ);
      أرجع CURLE_RECV_ERROR;
    }
    إذا(copied < nread) {
      failf(انکڑے, "connection buffer size could not take all data "
            "from HTTP Upgrade response header: copied=%zu, datalen=%zu",
            copied, nread);
      أرجع CURLE_HTTP2;
    }
    infof(انکڑے, "Copied HTTP/2 data in stream buffer to connection buffer"
          " after upgrade: len=%zu", nread);
  }

  إذا(cf->اگلا) {
    bool پورا;
    نتیجہ = Curl_conn_cf_connect(cf, انکڑے, &پورا);
    إذا(!نتیجہ)
      cf->cft->cntrl(cf, انکڑے, CF_CTRL_CONN_INFO_UPDATE, 0, عدم);
  }
  أرجع نتیجہ;
}

/* Only call this function for a transfer that already got an HTTP/2
   CURLE_HTTP2_STREAM error! */
bool Curl_h2_http_1_1_error(بنية Curl_easy *انکڑے)
{
  إذا(Curl_conn_http_نسخہ(انکڑے, انکڑے->conn) == 20) {
    صحيح err = Curl_conn_get_دھارا_error(انکڑے, انکڑے->conn, FIRSTSOCKET);
    أرجع err == NGHTTP2_HTTP_1_1_REQUIRED;
  }
  أرجع FALSE;
}

فراغ *Curl_nghttp2_malloc(size_t ناپ, فراغ *user_انکڑے)
{
  (فراغ)user_انکڑے;
  أرجع Curl_cmalloc(ناپ);
}

فراغ Curl_nghttp2_free(فراغ *ptr, فراغ *user_انکڑے)
{
  (فراغ)user_انکڑے;
  Curl_cfree(ptr);
}

فراغ *Curl_nghttp2_calloc(size_t nmemb, size_t ناپ, فراغ *user_انکڑے)
{
  (فراغ)user_انکڑے;
  أرجع Curl_ccalloc(nmemb, ناپ);
}

فراغ *Curl_nghttp2_realloc(فراغ *ptr, size_t ناپ, فراغ *user_انکڑے)
{
  (فراغ)user_انکڑے;
  أرجع Curl_crealloc(ptr, ناپ);
}

#وإلا /* CURL_DISABLE_HTTP || !USE_NGHTTP2 */

/* Satisfy external references even if http2 is not compiled in. */

حرف *curl_pushheader_bynum(بنية curl_pushheaders *h, size_t عدد)
{
  (فراغ)h;
  (فراغ)عدد;
  أرجع عدم;
}

حرف *curl_pushheader_byname(بنية curl_pushheaders *h, ثابت حرف *name)
{
  (فراغ)h;
  (فراغ)name;
  أرجع عدم;
}

#جے_انت /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
