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

#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#شامل <nghttp2/nghttp2.h>

#شامل "urldata.h"
#شامل "bufq.h"
#شامل "u32_ptrset.h"
#شامل "http1.h"
#شامل "http2.h"
#شامل "http.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "select.h"
#شامل "curlx/base64.h"
#شامل "multiif.h"
#شامل "ngh2_shim.h"
#شامل "progress.h"
#شامل "url.h"
#شامل "urlapi-int.h"
#شامل "cfilters.h"
#شامل "connect.h"
#شامل "transfer.h"
#شامل "bufref.h"
#شامل "curlx/dynbuf.h"
#شامل "headers.h"
#شامل "curl_share.h"

#اگر NGHTTP2_نسخہ_عدد < 0x010f00
#غلطی "nghttp2 1.15.0 or greater required"
#اگر_ختم

#تعریف_کرو NGHTTP2_HAS_سیٹ_LOCAL_کھڑکی_سائز 1

/* buffer dimensioning:
 * use 16K as chunk size, as that fits H2 DATA frames well */
#تعریف_کرو H2_CHUNK_سائز           (16 * 1024)
/* connection window size */
#تعریف_کرو H2_CONN_کھڑکی_سائز     (10 * 1024 * 1024)
/* on receiving from TLS, we prep for holding a full stream window */
#تعریف_کرو H2_NW_RECV_CHUNKS       (H2_CONN_کھڑکی_سائز / H2_CHUNK_سائز)
/* on send into TLS, we want to accumulate small frames */
#تعریف_کرو H2_NW_SEND_CHUNKS       1
/* this is how much we want "in flight" for a stream, unthrottled */
#تعریف_کرو H2_دھارا_کھڑکی_سائز_MAX   (10 * 1024 * 1024)
/* this is how much we want "in flight" for a stream, initially, IFF
 * nghttp2 allows us to tweak the local window size. */
#اگر NGHTTP2_HAS_سیٹ_LOCAL_کھڑکی_سائز
#تعریف_کرو H2_دھارا_کھڑکی_سائز_INITIAL  (64 * 1024)
#ورنہ
#تعریف_کرو H2_دھارا_کھڑکی_سائز_INITIAL H2_دھارا_کھڑکی_سائز_MAX
#اگر_ختم
/* keep smaller stream upload buffer (default h2 window size) to have
 * our progress bars and "upload done" reporting closer to reality */
#تعریف_کرو H2_دھارا_SEND_CHUNKS   ((64 * 1024) / H2_CHUNK_سائز)
/* spare chunks we keep for a full window */
#تعریف_کرو H2_دھارا_POOL_SPARES   (H2_CONN_کھڑکی_سائز / H2_CHUNK_سائز)

/* We need to accommodate the max number of streams with their window sizes on
 * the overall connection. Streams might become PAUSED which will block their
 * received QUOTA in the connection window. If we run out of space, the server
 * is blocked from sending us any data. See #10988 for an issue with this. */
#تعریف_کرو HTTP2_HUGE_کھڑکی_سائز (100 * H2_دھارا_کھڑکی_سائز_MAX)

#تعریف_کرو H2_SETTINGS_IV_LEN 3
#تعریف_کرو H2_BINSETTINGS_LEN 80

ساخت cf_h2_ctx {
  ساخت curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  ساخت cf_call_ڈیٹا call_ڈیٹا;

  ساخت bufq inbufq;           /* network input */
  ساخت bufq outbufq;          /* network output */
  ساخت bufc_pool دھارا_bufcp; /* spares for stream buffers */
  ساخت dynbuf scratch;        /* scratch buffer for temp use */

  ساخت u32_ptrset streams; /* hash of `data->mid` to `h2_stream_ctx` */
  size_t drain_کل; /* sum of all stream's UrlState drain */
  uint32_t initial_win_سائز; /* current initial window size (settings) */
  uint32_t max_concurrent_streams;
  uint32_t goaway_error;        /* goaway error code from server */
  int32_t remote_max_sid;       /* max id processed by server */
  int32_t local_max_sid;        /* max id processed by us */
  ثنائی_ہندسہ(initialized);
  ثنائی_ہندسہ(بذریعہ_h1_upgrade);
  ثنائی_ہندسہ(conn_closed);
  ثنائی_ہندسہ(rcvd_goaway);
  ثنائی_ہندسہ(بھیجا_goaway);
  ثنائی_ہندسہ(enable_ڈالو);
  ثنائی_ہندسہ(nw_برآمد_blocked);
};

/* How to access `call_data` from a cf_h2 filter */
#تعریف_منسوخ CF_CTX_CALL_ڈیٹا
#تعریف_کرو CF_CTX_CALL_ڈیٹا(cf) ((ساخت cf_h2_ctx *)(cf)->ctx)->call_ڈیٹا

/**
 * All about the H2 internals of a stream
 */
ساخت h2_دھارا_ctx {
  ساخت bufq sendbuf; /* request buffer */
  ساخت h1_req_parser h1; /* parsing the request */
  ساخت dynhds resp_trailers; /* response trailer fields */
  size_t resp_hds_len; /* amount of response header bytes in recvbuf */
  curl_off_t xfer_id;  /* transfer `id` owning this stream */
  curl_off_t nrcvd_ڈیٹا;  /* number of DATA bytes received */

  حرف **ڈالو_سرخیاں;       /* allocated array */
  size_t ڈالو_سرخیاں_مستعمل;  /* number of entries filled in */
  size_t ڈالو_سرخیاں_alloc; /* number of entries allocated */

  int32_t id; /* HTTP/2 protocol identifier for stream */
  uint32_t mid; /* transfer `mid` owning this stream */
  uint32_t error; /* stream error code */
  CURLcode xfer_نتیجہ; /* Result of writing out response */
  int32_t local_کھڑکی_سائز; /* the local recv window size */
  int32_t حالت_رمز; /* HTTP response status code */
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

ساکن خالی free_ڈالو_سرخیاں(ساخت h2_دھارا_ctx *دھارا)
{
  size_t i;
  برائے(i = 0; i < دھارا->ڈالو_سرخیاں_مستعمل; i++)
    curlx_free(دھارا->ڈالو_سرخیاں[i]);
  curlx_safefree(دھارا->ڈالو_سرخیاں);
  دھارا->ڈالو_سرخیاں_مستعمل = 0;
}

ساکن خالی h2_دھارا_ctx_free(ساخت h2_دھارا_ctx *دھارا)
{
  Curl_bufq_free(&دھارا->sendbuf);
  Curl_h1_req_parse_free(&دھارا->h1);
  Curl_dynhds_free(&دھارا->resp_trailers);
  free_ڈالو_سرخیاں(دھارا);
  curlx_free(دھارا);
}

ساکن خالی h2_دھارا_hash_free(uint32_t id, خالی *دھارا)
{
  (خالی)id;
  DEBUGASSERT(دھارا);
  h2_دھارا_ctx_free((ساخت h2_دھارا_ctx *)دھارا);
}

ساکن خالی cf_h2_ctx_init(ساخت cf_h2_ctx *ctx, bool بذریعہ_h1_upgrade)
{
  Curl_bufcp_init(&ctx->دھارا_bufcp, H2_CHUNK_سائز, H2_دھارا_POOL_SPARES);
  Curl_bufq_initp(&ctx->inbufq, &ctx->دھارا_bufcp, H2_NW_RECV_CHUNKS, 0);
  Curl_bufq_initp(&ctx->outbufq, &ctx->دھارا_bufcp, H2_NW_SEND_CHUNKS, 0);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_سرخی);
  Curl_u32_ptrset_init(&ctx->streams, h2_دھارا_hash_free);
  ctx->remote_max_sid = INT32_MAX;
  ctx->بذریعہ_h1_upgrade = بذریعہ_h1_upgrade;
  ctx->initialized = TRUE;
}

ساکن خالی cf_h2_ctx_free(ساخت cf_h2_ctx *ctx)
{
  اگر(ctx && ctx->initialized) {
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

ساکن uint32_t cf_h2_initial_win_سائز(ساخت Curl_easy *ڈیٹا)
{
#اگر NGHTTP2_HAS_سیٹ_LOCAL_کھڑکی_سائز
  /* If the transfer has a rate-limit lower than the default initial
   * stream window size, use that. It needs to be at least 8k or servers
   * may be unhappy. */
  curl_off_t rps = Curl_rlimit_فی_قدم(&ڈیٹا->پیش_رفت.dl.rlimit);
  اگر((rps > 0) && (rps < H2_دھارا_کھڑکی_سائز_INITIAL))
    واپس CURLMAX((uint32_t)rps, 8192);
#اگر_ختم
  واپس H2_دھارا_کھڑکی_سائز_INITIAL;
}

ساکن size_t populate_settings(nghttp2_settings_اندراج *iv,
                                ساخت Curl_easy *ڈیٹا,
                                ساخت cf_h2_ctx *ctx)
{
  iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
  iv[0].قدر = Curl_multi_max_concurrent_streams(ڈیٹا->multi);

  iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_کھڑکی_سائز;
  iv[1].قدر = cf_h2_initial_win_سائز(ڈیٹا);
  اگر(ctx)
    ctx->initial_win_سائز = iv[1].قدر;
  iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_ڈالو;
  iv[2].قدر = !!ڈیٹا->multi->ڈالو_cb;

  واپس 3;
}

ساکن ssize_t populate_binsettings(uint8_t *binsettings,
                                    ساخت Curl_easy *ڈیٹا)
{
  nghttp2_settings_اندراج iv[H2_SETTINGS_IV_LEN];
  size_t ivlen;

  ivlen = populate_settings(iv, ڈیٹا, تہی);
  /* this returns number of bytes it wrote or a negative number on error. */
  واپس nghttp2_pack_settings_payload(binsettings, H2_BINSETTINGS_LEN,
                                       iv, ivlen);
}

ساکن CURLcode cf_h2_update_settings(ساخت cf_h2_ctx *ctx,
                                      uint32_t initial_win_سائز)
{
  nghttp2_settings_اندراج اندراج_2;
  اندراج_2.settings_id = NGHTTP2_SETTINGS_INITIAL_کھڑکی_سائز;
  اندراج_2.قدر = initial_win_سائز;
  اگر(Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, &اندراج_2, 1))
    واپس CURLE_SEND_ERROR;
  ctx->initial_win_سائز = initial_win_سائز;
  واپس CURLE_OK;
}

#تعریف_کرو H2_دھارا_CTX(ctx, ڈیٹا)                                         \
  ((ساخت h2_دھارا_ctx *)(                                             \
    (ڈیٹا) ? Curl_u32_ptrset_get(&(ctx)->streams, (ڈیٹا)->mid) : تہی))

ساکن ساخت h2_دھارا_ctx *h2_دھارا_ctx_create(ساخت cf_h2_ctx *ctx,
                                                  ساخت Curl_easy *ڈیٹا)
{
  ساخت h2_دھارا_ctx *دھارا;

  دھارا = curlx_calloc(1, حجم(*دھارا));
  اگر(!دھارا)
    واپس تہی;

  دھارا->id = -1;
  دھارا->xfer_id = ڈیٹا->id;
  دھارا->mid = ڈیٹا->mid;
  Curl_bufq_initp(&دھارا->sendbuf, &ctx->دھارا_bufcp,
                  H2_دھارا_SEND_CHUNKS, BUFQ_OPT_NONE);
  Curl_h1_req_parse_init(&دھارا->h1, H1_PARSE_DEFAULT_MAX_سطر_LEN);
  Curl_dynhds_init(&دھارا->resp_trailers,
                   MAX_HTTP_RESP_سرخی_گنتی, DYN_HTTP_REQUEST);
  دھارا->bodystarted = FALSE;
  دھارا->حالت_رمز = -1;
  دھارا->closed = FALSE;
  دھارا->close_handled = FALSE;
  دھارا->error = NGHTTP2_NO_ERROR;
  دھارا->local_کھڑکی_سائز = H2_دھارا_کھڑکی_سائز_INITIAL;
  دھارا->nrcvd_ڈیٹا = 0;
  واپس دھارا;
}

ساکن ساخت Curl_easy *cf_h2_get_دھارا_xfer(ساخت Curl_cfilter *cf,
                                               int32_t دھارا_id,
                                               ساخت h2_دھارا_ctx **pstream)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;
  ساخت h2_دھارا_ctx *دھارا =
    Curl_ngh2_session_get_دھارا_user_ڈیٹا(&ctx->ngh2, دھارا_id);
  اگر(pstream)
    *pstream = دھارا;
  اگر(دھارا && (دھارا->mid != UINT32_MAX)) {
    ساخت Curl_easy *ڈیٹا, *call_ڈیٹا = CF_ڈیٹا_CURRENT(cf);
    اگر(!call_ڈیٹا || !call_ڈیٹا->multi) {
      DEBUGASSERT(0);
      واپس تہی;
    }
    ڈیٹا = Curl_multi_get_easy(call_ڈیٹا->multi, دھارا->mid);
    اگر(ڈیٹا && (ڈیٹا->id == دھارا->xfer_id))
      واپس ڈیٹا;
  }
  واپس تہی;
}

#اگر_تعریف_شدہ NGHTTP2_HAS_سیٹ_LOCAL_کھڑکی_سائز
ساکن int32_t cf_h2_get_desired_local_win(ساخت Curl_cfilter *cf,
                                           ساخت Curl_easy *ڈیٹا)
{
  curl_off_t avail = Curl_rlimit_avail(&ڈیٹا->پیش_رفت.dl.rlimit, تہی);

  (خالی)cf;
  اگر(avail < CURL_OFF_T_MAX) { /* limit in place */
    اگر(avail <= 0)
      واپس 0;
    ورنہ اگر(avail < H2_دھارا_کھڑکی_سائز_MAX)
      واپس (int32_t)avail;
  }
  واپس H2_دھارا_کھڑکی_سائز_MAX;
}

ساکن CURLcode cf_h2_update_local_win(ساخت Curl_cfilter *cf,
                                       ساخت Curl_easy *ڈیٹا,
                                       ساخت h2_دھارا_ctx *دھارا)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;
  int32_t dwsize;
  صحیح rv;

  dwsize = (دھارا->write_paused || دھارا->xfer_نتیجہ) ?
           0 : cf_h2_get_desired_local_win(cf, ڈیٹا);
  اگر(dwsize != دھارا->local_کھڑکی_سائز) {
    int32_t wsize = Curl_ngh2_session_get_دھارا_effective_local_کھڑکی_سائز(
      &ctx->ngh2, دھارا->id);
    اگر(dwsize > wsize) {
      rv = Curl_ngh2_session_سیٹ_local_کھڑکی_سائز(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, دھارا->id, dwsize);
      اگر(rv) {
        failf(ڈیٹا, "[%d] nghttp2 set_local_window_size(%d) failed: "
              "%s(%d)", دھارا->id, dwsize, nghttp2_strerror(rv), rv);
        واپس CURLE_HTTP2;
      }
      rv = Curl_ngh2_submit_کھڑکی_update(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, دھارا->id, dwsize - wsize);
      اگر(rv) {
        failf(ڈیٹا, "[%d] nghttp2_submit_window_update() failed: "
              "%s(%d)", دھارا->id, nghttp2_strerror(rv), rv);
        واپس CURLE_HTTP2;
      }
      دھارا->local_کھڑکی_سائز = dwsize;
      CURL_TRC_CF(ڈیٹا, cf, "[%d] local window update by %d",
                  دھارا->id, dwsize - wsize);
    }
    ورنہ {
      rv = Curl_ngh2_session_سیٹ_local_کھڑکی_سائز(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, دھارا->id, dwsize);
      اگر(rv) {
        failf(ڈیٹا, "[%d] nghttp2_session_set_local_window_size() failed: "
              "%s(%d)", دھارا->id, nghttp2_strerror(rv), rv);
        واپس CURLE_HTTP2;
      }
      دھارا->local_کھڑکی_سائز = dwsize;
      CURL_TRC_CF(ڈیٹا, cf, "[%d] local window size now %d",
                  دھارا->id, dwsize);
    }
  }
  واپس CURLE_OK;
}

#ورنہ /* NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

ساکن CURLcode cf_h2_update_local_win(ساخت Curl_cfilter *cf,
                                       ساخت Curl_easy *ڈیٹا,
                                       ساخت h2_دھارا_ctx *دھارا)
{
  (خالی)cf;
  (خالی)ڈیٹا;
  (خالی)دھارا;
  واپس CURLE_OK;
}
#اگر_ختم /* !NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

ساکن CURLcode http2_ڈیٹا_تیاری(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 ساخت h2_دھارا_ctx **pstream)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;
  ساخت h2_دھارا_ctx *دھارا;

  (خالی)cf;
  DEBUGASSERT(ڈیٹا);

  اگر(!ڈیٹا)
    واپس CURLE_BAD_FUNCTION_دلیل;

  دھارا = H2_دھارا_CTX(ctx, ڈیٹا);
  اگر(دھارا) {
    *pstream = دھارا;
    واپس CURLE_OK;
  }

  دھارا = h2_دھارا_ctx_create(ctx, ڈیٹا);
  اگر(!دھارا)
    واپس CURLE_برآمد_کا_حافظہ;

  اگر(Curl_u32_ptrset_سیٹ(&ctx->streams, ڈیٹا->mid, دھارا)) {
    h2_دھارا_ctx_free(دھارا);
    واپس CURLE_برآمد_کا_حافظہ;
  }

  *pstream = دھارا;
  واپس CURLE_OK;
}

ساکن CURLcode nw_برآمد_flush(ساخت Curl_cfilter *cf,
                             ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode نتیجہ;

  اگر(Curl_bufq_is_خالی_خانے(&ctx->outbufq))
    واپس CURLE_OK;

  نتیجہ = Curl_cf_send_bufq(cf->اگلا, ڈیٹا, &ctx->outbufq, تہی, 0,
                             &nwritten);
  اگر(نتیجہ) {
    اگر(نتیجہ == CURLE_AGAIN) {
      CURL_TRC_CF(ڈیٹا, cf, "flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_برآمد_blocked = 1;
    }
    واپس نتیجہ;
  }
  واپس Curl_bufq_is_خالی_خانے(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

ساکن خالی http2_ڈیٹا_مکمل(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;
  ساخت h2_دھارا_ctx *دھارا = H2_دھارا_CTX(ctx, ڈیٹا);

  DEBUGASSERT(ctx);
  اگر(!دھارا || !ctx->initialized)
    واپس;

  اگر(Curl_ngh2_valid(&ctx->ngh2)) {
    bool flush_egress = FALSE;
    /* returns error if stream not known, which is fine here */
    (خالی)Curl_ngh2_session_سیٹ_دھارا_user_ڈیٹا(&ctx->ngh2, دھارا->id, تہی);

    اگر(!دھارا->closed && دھارا->id > 0) {
      /* RST_STREAM */
      CURL_TRC_CF(ڈیٹا, cf, "[%d] premature DATA_DONE, RST stream",
                  دھارا->id);
      دھارا->closed = TRUE;
      دھارا->reset = TRUE;
      Curl_ngh2_submit_rst_دھارا(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                  دھارا->id, NGHTTP2_دھارا_CLOSED);
      flush_egress = TRUE;
    }

    اگر(flush_egress) {
      (خالی)Curl_ngh2_session_send(&ctx->ngh2);
      (خالی)nw_برآمد_flush(cf, ڈیٹا);
    }
  }

  Curl_u32_ptrset_unset(&ctx->streams, ڈیٹا->mid);
}

ساکن صحیح h2_client_new(ساخت Curl_cfilter *cf,
                         nghttp2_session_callbacks *cbs)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { تہی, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  صحیح rc = nghttp2_option_new(&o);
  اگر(rc)
    واپس rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_سیٹ_no_auto_کھڑکی_update(o, 1);
#اگر NGHTTP2_نسخہ_عدد >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_سیٹ_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#اگر_ختم
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  واپس rc;
}

/*
 * Returns nonzero if current HTTP/2 session should be closed.
 */
ساکن صحیح should_close_session(ساخت cf_h2_ctx *ctx)
{
  واپس ctx->drain_کل == 0 && !Curl_ngh2_session_want_read(&ctx->ngh2) &&
    !Curl_ngh2_session_want_write(&ctx->ngh2);
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
ساکن CURLcode h2_process_pending_داخلہ(ساخت Curl_cfilter *cf,
                                         ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;
  ثابت غیرعلامتی حرف *buf;
  size_t blen, nread;
  ssize_t rv;

  جب(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {
    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ثابت uint8_t *)buf, blen);
    اگر(!curlx_sztouz(rv, &nread)) {
      failf(ڈیٹا, "nghttp2 recv error %zd: %s", rv, nghttp2_strerror((صحیح)rv));
      واپس CURLE_RECV_ERROR;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    اگر(Curl_bufq_is_خالی_خانے(&ctx->inbufq)) {
      توڑو;
    }
    ورنہ {
      CURL_TRC_CF(ڈیٹا, cf, "process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }

  اگر(Curl_ngh2_session_check_request_allowed(&ctx->ngh2) == 0) {
    /* No more requests are allowed in the current session, so
       the connection may not be reused. This is set when a
       GOAWAY frame has been received or when the limit of stream
       identifiers has been reached. */
    CURL_TRC_M(ڈیٹا, "http/2: No new requests allowed");
    connclose(cf->conn);
  }

  واپس CURLE_OK;
}

/*
 * The server may send us data at any point (e.g. PING frames). Therefore, we
 * cannot assume that an HTTP/2 socket is dead because it is readable.
 *
 * Check the lower filters first and, if successful, peek at the socket
 * and distinguish between closed and data.
 */
ساکن bool http2_connisalive(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                              bool *داخلہ_pending)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *داخلہ_pending = FALSE;
  اگر(!cf->اگلا || !cf->اگلا->cft->is_alive(cf->اگلا, ڈیٹا, داخلہ_pending))
    واپس FALSE;

  اگر(*داخلہ_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode نتیجہ;
    size_t nread;

    *داخلہ_pending = FALSE;
    نتیجہ = Curl_cf_recv_bufq(cf->اگلا, ڈیٹا, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(ڈیٹا, cf, "connisalive, recv pending input -> %d, %zu",
                (صحیح)نتیجہ, nread);
    اگر(!نتیجہ) {
      نتیجہ = h2_process_pending_داخلہ(cf, ڈیٹا);
      اگر(نتیجہ)
        /* immediate error, considered dead */
        alive = FALSE;
      ورنہ {
        alive = !should_close_session(ctx);
      }
    }
    ورنہ اگر(نتیجہ != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  واپس alive;
}

ساکن CURLcode http2_send_ping(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;
  صحیح rc;

  rc = Curl_ngh2_submit_ping(&ctx->ngh2, 0, ZERO_NULL);
  اگر(rc) {
    failf(ڈیٹا, "nghttp2_submit_ping() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    واپس CURLE_HTTP2;
  }

  rc = Curl_ngh2_session_send(&ctx->ngh2);
  اگر(rc) {
    failf(ڈیٹا, "nghttp2_session_send() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    واپس CURLE_SEND_ERROR;
  }
  واپس CURLE_OK;
}

/*
 * Store nghttp2 version info in this buffer.
 */
خالی Curl_http2_ver(حرف *p, size_t len)
{
  nghttp2_info *h2 = nghttp2_نسخہ(0);
  (خالی)curl_msnprintf(p, len, "nghttp2/%s", h2->نسخہ_str);
}

/*
 * The implementation of nghttp2_send_callback type. Here we write |data| with
 * size |length| to the network and return the number of bytes actually
 * written. See the documentation of nghttp2_send_callback for the details.
 */
ساکن ssize_t send_callback(nghttp2_session *h2,
                             ثابت uint8_t *buf, size_t blen, صحیح flags,
                             خالی *userp)
{
  ساخت Curl_cfilter *cf = userp;
  ساخت cf_h2_ctx *ctx = cf->ctx;
  ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);
  size_t nwritten;
  CURLcode نتیجہ = CURLE_OK;

  (خالی)h2;
  (خالی)flags;
  DEBUGASSERT(ڈیٹا);

  اگر(!cf->connected)
    نتیجہ = Curl_bufq_write(&ctx->outbufq, buf, blen, &nwritten);
  ورنہ
    نتیجہ = Curl_cf_send_bufq(cf->اگلا, ڈیٹا, &ctx->outbufq, buf, blen,
                               &nwritten);

  اگر(نتیجہ) {
    اگر(نتیجہ == CURLE_AGAIN) {
      ctx->nw_برآمد_blocked = 1;
      واپس NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(ڈیٹا, "Failed sending HTTP2 data");
    واپس NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  اگر(!nwritten) {
    ctx->nw_برآمد_blocked = 1;
    واپس NGHTTP2_ERR_WOULDBLOCK;
  }
  واپس (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

/* We pass a pointer to this struct in the push callback, but the contents of
   the struct are hidden from the user. */
ساخت curl_pushheaders {
  ساخت Curl_easy *ڈیٹا;
  ساخت h2_دھارا_ctx *دھارا;
  ثابت nghttp2_ڈالو_promise *چوکھٹا;
};

/*
 * push header access function. Only to be used from within the push callback
 */
حرف *curl_pushheader_bynum(ساخت curl_pushheaders *h, size_t عدد)
{
  /* Verify that we got a good easy handle in the push header struct, mostly to
     detect rubbish input fast(er). */
  اگر(!h || !GOOD_EASY_HANDLE(h->ڈیٹا))
    واپس تہی;
  ورنہ {
    اگر(h->دھارا && عدد < h->دھارا->ڈالو_سرخیاں_مستعمل)
      واپس h->دھارا->ڈالو_سرخیاں[عدد];
  }
  واپس تہی;
}

/*
 * push header access function. Only to be used from within the push callback
 */
حرف *curl_pushheader_byname(ساخت curl_pushheaders *h, ثابت حرف *name)
{
  ساخت h2_دھارا_ctx *دھارا;
  size_t len;
  size_t i;
  /* Verify that we got a good easy handle in the push header struct,
     mostly to detect rubbish input fast(er). Also empty header name
     is rubbish too. We have to allow ":" at the beginning of
     the header, but header == ":" must be rejected. If we have ':' in
     the middle of header, it could be matched in middle of the value,
     this is because we do prefix match.*/
  اگر(!h || !GOOD_EASY_HANDLE(h->ڈیٹا) || !name || !name[0] ||
     !strcmp(name, ":") || strchr(name + 1, ':'))
    واپس تہی;

  دھارا = h->دھارا;
  اگر(!دھارا)
    واپس تہی;

  len = لڑی_لمبائی(name);
  برائے(i = 0; i < دھارا->ڈالو_سرخیاں_مستعمل; i++) {
    اگر(!لڑی_موازنہ(name, دھارا->ڈالو_سرخیاں[i], len)) {
      /* sub-match, make sure that it is followed by a colon */
      اگر(دھارا->ڈالو_سرخیاں[i][len] != ':')
        جاری;
      واپس &دھارا->ڈالو_سرخیاں[i][len + 1];
    }
  }
  واپس تہی;
}

ساکن ساخت Curl_easy *h2_duphandle(ساخت Curl_cfilter *cf,
                                      ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_easy *دوسرا = curl_easy_init();
  اگر(دوسرا) {
    ساخت h2_دھارا_ctx *دوسرا_دھارا;
    http2_ڈیٹا_تیاری(cf, دوسرا, &دوسرا_دھارا);
    دوسرا->state.وزن = ڈیٹا->state.وزن;
    اگر(ڈیٹا->share)
      (خالی)Curl_share_easy_link(دوسرا, ڈیٹا->share);
    /* copy a range of options from the parent handle */
    دوسرا->سیٹ.fwrite_func = ڈیٹا->سیٹ.fwrite_func;
    دوسرا->سیٹ.برآمد = ڈیٹا->سیٹ.برآمد;
    دوسرا->سیٹ.fwrite_سرخی = ڈیٹا->سیٹ.fwrite_سرخی;
    دوسرا->سیٹ.writeheader = ڈیٹا->سیٹ.writeheader;
    دوسرا->سیٹ.fdebug = ڈیٹا->سیٹ.fdebug;
    دوسرا->سیٹ.debugdata = ڈیٹا->سیٹ.debugdata;
    دوسرا->سیٹ.fxferinfo = ڈیٹا->سیٹ.fxferinfo;
    دوسرا->سیٹ.fprogress = ڈیٹا->سیٹ.fprogress;
    دوسرا->سیٹ.پیش_رفت_client = ڈیٹا->سیٹ.پیش_رفت_client;
    دوسرا->پیش_رفت.hide = ڈیٹا->پیش_رفت.hide;
    دوسرا->پیش_رفت.callback = ڈیٹا->پیش_رفت.callback;
  }
  واپس دوسرا;
}

ساکن صحیح سیٹ_transfer_url(ساخت Curl_easy *newhandle,
                            ساخت curl_pushheaders *hp,
                            ساخت Curl_easy *ڈیٹا)
{
  ثابت حرف *v;
  CURLUcode uc;
  حرف *url = تہی;
  صحیح rc = 0;
  CURLU *u = curl_url();

  اگر(!u)
    واپس 5;

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_SCHEME);
  اگر(v) {
    uc = curl_url_سیٹ(u, CURLUPART_SCHEME, v, 0);
    اگر(uc) {
      rc = 1;
      جاؤ fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_AUTHORITY);
  اگر(v) {
    uc = Curl_url_سیٹ_authority(u, v);
    اگر(uc) {
      rc = 2;
      جاؤ fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_راستہ);
  اگر(v) {
    uc = curl_url_سیٹ(u, CURLUPART_راستہ, v, 0);
    اگر(uc) {
      rc = 3;
      جاؤ fail;
    }
  }

  /* We can only allow PUSH of resource from the same origin, e.g.
   * scheme + hostname + port */
  اگر(!Curl_url_same_origin(ڈیٹا->state.uh, u)) {
    rc = 1;
    جاؤ fail;
  }

  uc = curl_url_get(u, CURLUPART_URL, &url, 0);
  اگر(uc)
    rc = 4;
fail:
  اگر(rc) {
    curl_url_cleanup(u);
    واپس rc;
  }

  DEBUGASSERT(!newhandle->state.uh);
  newhandle->state.uh = u;
  Curl_bufref_سیٹ(&newhandle->state.url, url, 0, curl_free);
  واپس 0;
}

ساکن خالی discard_newhandle(ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *newhandle)
{
  http2_ڈیٹا_مکمل(cf, newhandle);
  (خالی)Curl_close(&newhandle);
}

ساکن صحیح ڈالو_promise(ساخت Curl_cfilter *cf,
                        ساخت Curl_easy *ڈیٹا,
                        ثابت nghttp2_ڈالو_promise *چوکھٹا)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;
  صحیح rv; /* one of the CURL_PUSH_* defines */

  CURL_TRC_CF(ڈیٹا, cf, "[%d] PUSH_PROMISE received",
              چوکھٹا->promised_دھارا_id);
  اگر(ڈیٹا->multi->ڈالو_cb) {
    ساخت h2_دھارا_ctx *دھارا;
    ساخت h2_دھارا_ctx *newstream;
    ساخت curl_pushheaders heads;
    CURLMcode mresult;
    CURLcode نتیجہ;
    /* clone the parent */
    ساخت Curl_easy *newhandle = h2_duphandle(cf, ڈیٹا);
    اگر(!newhandle) {
      infof(ڈیٹا, "failed to duplicate handle");
      rv = CURL_ڈالو_DENY; /* FAIL HARD */
      جاؤ fail;
    }

    دھارا = H2_دھارا_CTX(ctx, ڈیٹا);
    اگر(!دھارا) {
      failf(ڈیٹا, "Internal NULL stream");
      discard_newhandle(cf, newhandle);
      rv = CURL_ڈالو_DENY;
      جاؤ fail;
    }

    heads.ڈیٹا = ڈیٹا;
    heads.دھارا = دھارا;
    heads.چوکھٹا = چوکھٹا;

    rv = سیٹ_transfer_url(newhandle, &heads, ڈیٹا);
    اگر(rv) {
      CURL_TRC_CF(ڈیٹا, cf, "[%d] PUSH_PROMISE, failed to set URL -> %d",
                  چوکھٹا->promised_دھارا_id, rv);
      discard_newhandle(cf, newhandle);
      rv = CURL_ڈالو_DENY;
      جاؤ fail;
    }

    {
      ساخت Curl_mapi_guard guard;
      CURL_CBAPI_آغاز(&guard, ڈیٹا, multi_ڈالو_cb);
      rv = ڈیٹا->multi->ڈالو_cb(ڈیٹا, newhandle,
                                دھارا->ڈالو_سرخیاں_مستعمل, &heads,
                                ڈیٹا->multi->ڈالو_userp);
      CURL_CBAPI_انتہا(&guard);
    }

    /* free the headers again */
    free_ڈالو_سرخیاں(دھارا);

    اگر(rv) {
      DEBUGASSERT((rv > CURL_ڈالو_OK) && (rv <= CURL_ڈالو_ERROROUT));
      /* denied, kill off the new handle again */
      CURL_TRC_CF(ڈیٹا, cf, "[%d] PUSH_PROMISE, denied by application -> %d",
                  چوکھٹا->promised_دھارا_id, rv);
      discard_newhandle(cf, newhandle);
      جاؤ fail;
    }

    /* approved, add to the multi handle for processing. This
     * assigns newhandle->mid. For the new `mid` we assign the
     * h2_stream instance and remember the stream_id already known. */
    mresult = Curl_multi_جوڑو_perform(ڈیٹا->multi, newhandle, cf->conn);
    اگر(mresult) {
      infof(ڈیٹا, "failed to add handle to multi");
      discard_newhandle(cf, newhandle);
      rv = CURL_ڈالو_DENY;
      جاؤ fail;
    }

    نتیجہ = http2_ڈیٹا_تیاری(cf, newhandle, &newstream);
    اگر(نتیجہ) {
      failf(ڈیٹا, "error setting up stream: %d", (صحیح)نتیجہ);
      discard_newhandle(cf, newhandle);
      rv = CURL_ڈالو_DENY;
      جاؤ fail;
    }

    DEBUGASSERT(newstream);
    newstream->id = چوکھٹا->promised_دھارا_id;
    newhandle->req.maxdownload = -1;
    newhandle->req.سائز = -1;

    CURL_TRC_CF(ڈیٹا, cf, "promise easy handle added to multi, mid=%u",
                newhandle->mid);
    rv = Curl_ngh2_session_سیٹ_دھارا_user_ڈیٹا(
      &ctx->ngh2, newstream->id, newstream);
    اگر(rv) {
      infof(ڈیٹا, "failed to set user_data for stream %d",
            newstream->id);
      DEBUGASSERT(0);
      discard_newhandle(cf, newhandle);
      rv = CURL_ڈالو_DENY;
      جاؤ fail;
    }

    /* success, remember max stream id processed */
    اگر(newstream->id > ctx->local_max_sid)
      ctx->local_max_sid = newstream->id;
  }
  ورنہ {
    CURL_TRC_CF(ڈیٹا, cf, "Got PUSH_PROMISE, ignore it");
    rv = CURL_ڈالو_DENY;
  }
fail:
  واپس rv;
}

ساکن خالی h2_xfer_write_resp_hd(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  ساخت h2_دھارا_ctx *دھارا,
                                  ثابت حرف *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  اگر(!دھارا->xfer_نتیجہ) {
    دھارا->xfer_نتیجہ = Curl_xfer_write_resp_hd(ڈیٹا, buf, blen, eos);
    اگر(!دھارا->xfer_نتیجہ && !eos)
      دھارا->xfer_نتیجہ = cf_h2_update_local_win(cf, ڈیٹا, دھارا);
    اگر(دھارا->xfer_نتیجہ)
      CURL_TRC_CF(ڈیٹا, cf, "[%d] error %d writing %zu bytes of headers",
                  دھارا->id, (صحیح)دھارا->xfer_نتیجہ, blen);
  }
}

ساکن خالی h2_xfer_write_resp(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا,
                               ساخت h2_دھارا_ctx *دھارا,
                               ثابت حرف *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  اگر(!دھارا->xfer_نتیجہ)
    دھارا->xfer_نتیجہ = Curl_xfer_write_resp(ڈیٹا, buf, blen, eos);
  /* If the transfer write is errored, we do not want any more data */
  اگر(دھارا->xfer_نتیجہ) {
    ساخت cf_h2_ctx *ctx = cf->ctx;
    CURL_TRC_CF(ڈیٹا, cf, "[%d] error %d writing %zu bytes of data, "
                "RST-ing stream",
                دھارا->id, (صحیح)دھارا->xfer_نتیجہ, blen);
    Curl_ngh2_submit_rst_دھارا(
      &ctx->ngh2, 0, دھارا->id, (uint32_t)NGHTTP2_ERR_CALLBACK_FAILURE);
  }
  ورنہ اگر(!دھارا->write_paused && Curl_xfer_write_is_paused(ڈیٹا)) {
    CURL_TRC_CF(ڈیٹا, cf, "[%d] stream output paused", دھارا->id);
    دھارا->write_paused = TRUE;
  }
  ورنہ اگر(دھارا->write_paused && !Curl_xfer_write_is_paused(ڈیٹا)) {
    CURL_TRC_CF(ڈیٹا, cf, "[%d] stream output unpaused", دھارا->id);
    دھارا->write_paused = FALSE;
  }

  اگر(!دھارا->xfer_نتیجہ && !eos)
    دھارا->xfer_نتیجہ = cf_h2_update_local_win(cf, ڈیٹا, دھارا);
}

ساکن CURLcode on_دھارا_چوکھٹا(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا,
                                ساخت h2_دھارا_ctx *دھارا,
                                ثابت nghttp2_چوکھٹا *چوکھٹا)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;
  int32_t دھارا_id = چوکھٹا->hd.دھارا_id;
  صحیح rv;

  اگر(!دھارا) {
    CURL_TRC_CF(ڈیٹا, cf, "[%d] No stream_ctx set", دھارا_id);
    واپس CURLE_FAILED_INIT;
  }

  انتخاب(چوکھٹا->hd.type) {
  حالت NGHTTP2_ڈیٹا:
    CURL_TRC_CF(ڈیٹا, cf, "[%d] DATA, window=%d/%d",
                دھارا_id,
                Curl_ngh2_session_get_دھارا_effective_recv_ڈیٹا_لمبائی(
                  &ctx->ngh2, دھارا->id),
                Curl_ngh2_session_get_دھارا_effective_local_کھڑکی_سائز(
                  &ctx->ngh2, دھارا->id));
    /* If !body started on this stream, then receiving DATA is illegal. */
    اگر(!دھارا->bodystarted) {
      اگر(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_دھارا(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                       دھارا_id, NGHTTP2_PROTOCOL_ERROR))) {
        واپس CURLE_RECV_ERROR;
      }
    }
    توڑو;
  حالت NGHTTP2_سرخیاں:
    اگر(دھارا->bodystarted) {
      /* Only valid HEADERS after body started is trailer HEADERS. We
         buffer them in on_header callback. */
      توڑو;
    }

    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    اگر(دھارا->حالت_رمز == -1)
      واپس CURLE_RECV_ERROR;

    /* Only final status code signals the end of header */
    اگر(دھارا->حالت_رمز / 100 != 1) {
      دھارا->resp_hds_complete = TRUE;
      دھارا->bodystarted = TRUE;
    }
    ورنہ
      دھارا->حالت_رمز = -1;

    h2_xfer_write_resp_hd(cf, ڈیٹا, دھارا, STRCONST("\r\n"),
                          (bool)دھارا->closed);

    Curl_multi_mark_dirty(ڈیٹا);
    توڑو;
  حالت NGHTTP2_ڈالو_PROMISE:
    rv = ڈالو_promise(cf, ڈیٹا, &چوکھٹا->ڈالو_promise);
    اگر(rv) { /* deny! */
      DEBUGASSERT((rv > CURL_ڈالو_OK) && (rv <= CURL_ڈالو_ERROROUT));
      اگر(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_دھارا(&ctx->ngh2, NGHTTP2_FLAG_NONE,
             چوکھٹا->ڈالو_promise.promised_دھارا_id, NGHTTP2_CANCEL)))
        واپس CURLE_SEND_ERROR;
      ورنہ اگر(rv == CURL_ڈالو_ERROROUT) {
        /* fail the parent transfer, not the whole session */
        CURL_TRC_CF(ڈیٹا, cf, "[%d] fail in PUSH_PROMISE received",
                    دھارا_id);
        اگر(!دھارا->xfer_نتیجہ)
          دھارا->xfer_نتیجہ = CURLE_RECV_ERROR;
        Curl_multi_mark_dirty(ڈیٹا);
      }
    }
    توڑو;
  حالت NGHTTP2_RST_دھارا:
    اگر(چوکھٹا->rst_دھارا.error_رمز)
      دھارا->reset_by_server = TRUE;
    Curl_multi_mark_dirty(ڈیٹا);
    توڑو;
  حالت NGHTTP2_کھڑکی_UPDATE:
    اگر(CURL_REQ_WANT_SEND(ڈیٹا) && Curl_bufq_is_خالی_خانے(&دھارا->sendbuf)) {
      /* need more data, force processing of transfer */
      Curl_multi_mark_dirty(ڈیٹا);
    }
    ورنہ اگر(!Curl_bufq_is_خالی_خانے(&دھارا->sendbuf)) {
      /* resume the potentially suspended stream */
      اگر(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_session_resume_ڈیٹا(&ctx->ngh2, دھارا->id)))
        واپس CURLE_SEND_ERROR;
    }
    توڑو;
  طےشدہ:
    توڑو;
  }

  اگر(چوکھٹا->hd.flags & NGHTTP2_FLAG_انتہا_دھارا) {
    اگر(!دھارا->closed && !دھارا->body_eos &&
       ((دھارا->حالت_رمز >= 400) || (دھارا->حالت_رمز < 200))) {
      /* The server did not give us a positive response and we are not
       * done uploading the request body. We need to stop doing that and
       * also inform the server that we aborted our side. */
      CURL_TRC_CF(ڈیٹا, cf, "[%d] EOS frame with unfinished upload and "
                  "HTTP status %d, abort upload by RST",
                  دھارا_id, دھارا->حالت_رمز);
      Curl_ngh2_submit_rst_دھارا(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, دھارا->id, NGHTTP2_دھارا_CLOSED);
      دھارا->closed = TRUE;
    }
    Curl_multi_mark_dirty(ڈیٹا);
  }
  واپس CURLE_OK;
}

#اگر_تعریف_شدہ CURLVERBOSE
صحیح Curl_nghttp2_fr_print(ثابت nghttp2_چوکھٹا *چوکھٹا, حرف *بفر,
                          size_t blen)
{
  انتخاب(چوکھٹا->hd.type) {
  حالت NGHTTP2_ڈیٹا: {
    واپس curl_msnprintf(بفر, blen,
                          "FRAME[DATA, len=%d, eos=%d, padlen=%d]",
                          (صحیح)چوکھٹا->hd.لمبائی,
                          !!(چوکھٹا->hd.flags & NGHTTP2_FLAG_انتہا_دھارا),
                          (صحیح)چوکھٹا->ڈیٹا.padlen);
  }
  حالت NGHTTP2_سرخیاں: {
    واپس curl_msnprintf(بفر, blen,
                          "FRAME[HEADERS, len=%d, hend=%d, eos=%d]",
                          (صحیح)چوکھٹا->hd.لمبائی,
                          !!(چوکھٹا->hd.flags & NGHTTP2_FLAG_انتہا_سرخیاں),
                          !!(چوکھٹا->hd.flags & NGHTTP2_FLAG_انتہا_دھارا));
  }
  حالت NGHTTP2_ترجیح: {
    واپس curl_msnprintf(بفر, blen,
                          "FRAME[PRIORITY, len=%d, flags=%d]",
                          (صحیح)چوکھٹا->hd.لمبائی, چوکھٹا->hd.flags);
  }
  حالت NGHTTP2_RST_دھارا: {
    واپس curl_msnprintf(بفر, blen,
                          "FRAME[RST_STREAM, len=%d, flags=%d, error=%u]",
                          (صحیح)چوکھٹا->hd.لمبائی, چوکھٹا->hd.flags,
                          چوکھٹا->rst_دھارا.error_رمز);
  }
  حالت NGHTTP2_SETTINGS: {
    اگر(چوکھٹا->hd.flags & NGHTTP2_FLAG_ACK) {
      واپس curl_msnprintf(بفر, blen, "FRAME[SETTINGS, ack=1]");
    }
    واپس curl_msnprintf(بفر, blen,
                          "FRAME[SETTINGS, len=%d]", (صحیح)چوکھٹا->hd.لمبائی);
  }
  حالت NGHTTP2_ڈالو_PROMISE:
    واپس curl_msnprintf(بفر, blen,
                          "FRAME[PUSH_PROMISE, len=%d, hend=%d]",
                          (صحیح)چوکھٹا->hd.لمبائی,
                          !!(چوکھٹا->hd.flags & NGHTTP2_FLAG_انتہا_سرخیاں));
  حالت NGHTTP2_PING:
    واپس curl_msnprintf(بفر, blen,
                          "FRAME[PING, len=%d, ack=%d]",
                          (صحیح)چوکھٹا->hd.لمبائی,
                          چوکھٹا->hd.flags & NGHTTP2_FLAG_ACK);
  حالت NGHTTP2_GOAWAY: {
    حرف scratch[128];
    size_t s_len = CURL_ARRAYSIZE(scratch);
    size_t len = (چوکھٹا->goaway.opaque_ڈیٹا_len < s_len) ?
      چوکھٹا->goaway.opaque_ڈیٹا_len : s_len - 1;
    اگر(len)
      حافظہ_نقل(scratch, چوکھٹا->goaway.opaque_ڈیٹا, len);
    scratch[len] = '\0';
    واپس curl_msnprintf(بفر, blen,
                          "FRAME[GOAWAY, error=%u, reason='%s', "
                          "last_stream=%d]", چوکھٹا->goaway.error_رمز,
                          scratch, چوکھٹا->goaway.آخری_دھارا_id);
  }
  حالت NGHTTP2_کھڑکی_UPDATE: {
    واپس curl_msnprintf(بفر, blen,
                          "FRAME[WINDOW_UPDATE, incr=%d]",
                          چوکھٹا->کھڑکی_update.کھڑکی_سائز_increment);
  }
  طےشدہ:
    واپس curl_msnprintf(بفر, blen, "FRAME[%d, len=%d, flags=%d]",
                          چوکھٹا->hd.type, (صحیح)چوکھٹا->hd.لمبائی,
                          چوکھٹا->hd.flags);
  }
}

ساکن صحیح on_چوکھٹا_send(nghttp2_session *session, ثابت nghttp2_چوکھٹا *چوکھٹا,
                         خالی *userp)
{
  ساخت Curl_cfilter *cf = userp;
  ساخت cf_h2_ctx *ctx = cf->ctx;
  ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);

  (خالی)session;
  DEBUGASSERT(ڈیٹا);
  اگر(Curl_trc_cf_is_verbose(cf, ڈیٹا)) {
    حرف بفر[256];
    صحیح len;
    len = Curl_nghttp2_fr_print(چوکھٹا, بفر, حجم(بفر) - 1);
    بفر[len] = 0;
    CURL_TRC_CF(ڈیٹا, cf, "[%d] -> %s", چوکھٹا->hd.دھارا_id, بفر);
  }
  اگر((چوکھٹا->hd.type == NGHTTP2_GOAWAY) && !ctx->بھیجا_goaway) {
    /* A GOAWAY not initiated by us, but by nghttp2 itself on detecting
     * a protocol error on the connection */
    failf(ڈیٹا, "nghttp2 shuts down connection with error %u: %s",
          چوکھٹا->goaway.error_رمز,
          nghttp2_http2_strerror(چوکھٹا->goaway.error_رمز));
  }
  واپس 0;
}
#اگر_ختم /* CURLVERBOSE */

ساکن صحیح on_چوکھٹا_recv(nghttp2_session *session, ثابت nghttp2_چوکھٹا *چوکھٹا,
                         خالی *userp)
{
  ساخت Curl_cfilter *cf = userp;
  ساخت cf_h2_ctx *ctx = cf->ctx;
  ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf), *ڈیٹا_s;
  ساخت h2_دھارا_ctx *دھارا;
  int32_t دھارا_id = چوکھٹا->hd.دھارا_id;

  (خالی)session;
  DEBUGASSERT(ڈیٹا);
#اگر_تعریف_شدہ CURLVERBOSE
  اگر(Curl_trc_cf_is_verbose(cf, ڈیٹا)) {
    حرف بفر[256];
    صحیح len;
    len = Curl_nghttp2_fr_print(چوکھٹا, بفر, حجم(بفر) - 1);
    بفر[len] = 0;
    CURL_TRC_CF(ڈیٹا, cf, "[%d] <- %s", چوکھٹا->hd.دھارا_id, بفر);
  }
#اگر_ختم /* CURLVERBOSE */

  اگر(!دھارا_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(ڈیٹا);
    انتخاب(چوکھٹا->hd.type) {
    حالت NGHTTP2_SETTINGS: {
      اگر(!(چوکھٹا->hd.flags & NGHTTP2_FLAG_ACK)) {
        uint32_t max_conn = ctx->max_concurrent_streams;
        ctx->max_concurrent_streams =
          Curl_ngh2_session_get_remote_settings(
            &ctx->ngh2, NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS);
        ctx->enable_ڈالو = Curl_ngh2_session_get_remote_settings(
          &ctx->ngh2, NGHTTP2_SETTINGS_ENABLE_ڈالو) != 0;
        CURL_TRC_CF(ڈیٹا, cf, "[0] MAX_CONCURRENT_STREAMS: %u",
                    ctx->max_concurrent_streams);
        CURL_TRC_CF(ڈیٹا, cf, "[0] ENABLE_PUSH: %s",
                    ctx->enable_ڈالو ? "TRUE" : "false");
        اگر(ڈیٹا && max_conn != ctx->max_concurrent_streams) {
          /* only signal change if the value actually changed */
          CURL_TRC_CF(ڈیٹا, cf, "[0] notify MAX_CONCURRENT_STREAMS: %u",
                      ctx->max_concurrent_streams);
          Curl_multi_connchanged(ڈیٹا->multi);
        }
        /* Since the initial stream window is 64K, a request might be on HOLD,
         * due to exhaustion. The (initial) SETTINGS may announce a much larger
         * window and *assume* that we treat this like a WINDOW_UPDATE. Some
         * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
         * To be safe, we UNHOLD a stream in order not to stall. */
        اگر(CURL_REQ_WANT_SEND(ڈیٹا))
          Curl_multi_mark_dirty(ڈیٹا);
      }
      توڑو;
    }
    حالت NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->goaway_error = چوکھٹا->goaway.error_رمز;
      ctx->remote_max_sid = چوکھٹا->goaway.آخری_دھارا_id;
      اگر(ڈیٹا) {
        infof(ڈیٹا, "received GOAWAY, error=%u, last_stream=%d",
              ctx->goaway_error, ctx->remote_max_sid);
        Curl_multi_connchanged(ڈیٹا->multi);
      }
      توڑو;
    طےشدہ:
      توڑو;
    }
    واپس 0;
  }

  ڈیٹا_s = cf_h2_get_دھارا_xfer(cf, دھارا_id, &دھارا);
  اگر(!ڈیٹا_s) {
    CURL_TRC_CF(ڈیٹا, cf, "[%d] No Curl_easy associated", دھارا_id);
    واپس 0;
  }

  واپس on_دھارا_چوکھٹا(cf, ڈیٹا_s, دھارا, چوکھٹا) ?
         NGHTTP2_ERR_CALLBACK_FAILURE : 0;
}

ساکن صحیح cf_h2_on_invalid_چوکھٹا_recv(nghttp2_session *session,
                                       ثابت nghttp2_چوکھٹا *چوکھٹا,
                                       صحیح ngerr, خالی *userp)
{
  ساخت Curl_cfilter *cf = userp;
  ساخت cf_h2_ctx *ctx = cf->ctx;
  ساخت Curl_easy *ڈیٹا;
  ساخت h2_دھارا_ctx *دھارا;
  int32_t دھارا_id = چوکھٹا->hd.دھارا_id;

  (خالی)session;
  ڈیٹا = cf_h2_get_دھارا_xfer(cf, دھارا_id, &دھارا);
  اگر(ڈیٹا) {
#اگر_تعریف_شدہ CURLVERBOSE
    حرف بفر[256];
    صحیح len;
    len = Curl_nghttp2_fr_print(چوکھٹا, بفر, حجم(بفر) - 1);
    بفر[len] = 0;
    failf(ڈیٹا, "[HTTP2] [%d] received invalid frame: %s, error %d: %s",
          دھارا_id, بفر, ngerr, nghttp2_strerror(ngerr));
#اگر_ختم /* CURLVERBOSE */
    اگر(دھارا) {
      Curl_ngh2_submit_rst_دھارا(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, دھارا->id, NGHTTP2_دھارا_CLOSED);
      دھارا->error = ngerr;
      دھارا->closed = TRUE;
      دھارا->reset = TRUE;
    }
  }
  /* Return no error, nghttp2 will RST/GOAWAY by itself when needed */
  واپس 0;
}

ساکن صحیح on_ڈیٹا_chunk_recv(nghttp2_session *session, uint8_t flags,
                              int32_t دھارا_id,
                              ثابت uint8_t *mem, size_t len, خالی *userp)
{
  ساخت Curl_cfilter *cf = userp;
  ساخت cf_h2_ctx *ctx = cf->ctx;
  ساخت h2_دھارا_ctx *دھارا;
  ساخت Curl_easy *ڈیٹا_s;

  (خالی)session;
  (خالی)flags;

  DEBUGASSERT(دھارا_id); /* should never be a zero stream ID here */
  DEBUGASSERT(CF_ڈیٹا_CURRENT(cf));

  /* get the stream from the hash based on Stream ID */
  ڈیٹا_s = cf_h2_get_دھارا_xfer(cf, دھارا_id, &دھارا);
  اگر(!ڈیٹا_s) {
    /* Receiving a Stream ID not in the hash should not happen - unless
       we have aborted a transfer artificially and there were more data
       in the pipeline. Silently ignore. */
    CURL_TRC_CF(CF_ڈیٹا_CURRENT(cf), cf, "[%d] Data for unknown", دھارا_id);
    /* consumed explicitly as no one will read it */
    Curl_ngh2_session_consume(&ctx->ngh2, دھارا_id, len);
    واپس 0;
  }

  اگر(!دھارا)
    واپس NGHTTP2_ERR_CALLBACK_FAILURE;

  h2_xfer_write_resp(cf, ڈیٹا_s, دھارا, (ثابت حرف *)mem, len, FALSE);

  Curl_ngh2_session_consume(&ctx->ngh2, دھارا_id, len);
  دھارا->nrcvd_ڈیٹا += (curl_off_t)len;
  واپس 0;
}

ساکن صحیح on_دھارا_close(nghttp2_session *session, int32_t دھارا_id,
                           uint32_t error_رمز, خالی *userp)
{
  ساخت Curl_cfilter *cf = userp;
  ساخت cf_h2_ctx *ctx = cf->ctx;
  ساخت Curl_easy *ڈیٹا_s, *call_ڈیٹا = CF_ڈیٹا_CURRENT(cf);
  ساخت h2_دھارا_ctx *دھارا = تہی;
  صحیح rv;
  (خالی)session;

  DEBUGASSERT(call_ڈیٹا);
  /* stream id 0 is the connection, do not look there for streams. */
  ڈیٹا_s = دھارا_id ? cf_h2_get_دھارا_xfer(cf, دھارا_id, &دھارا) : تہی;
  اگر(!ڈیٹا_s) {
    CURL_TRC_CF(call_ڈیٹا, cf,
                "[%d] on_stream_close, no easy set on stream", دھارا_id);
    واپس 0;
  }
  اگر(!GOOD_EASY_HANDLE(ڈیٹا_s)) {
    /* nghttp2 still has an easy registered for the stream which has
     * been freed be libcurl. This points to a code path that does not
     * trigger DONE or DETACH events as it must. */
    CURL_TRC_CF(call_ڈیٹا, cf,
                "[%d] on_stream_close, not a GOOD easy on stream", دھارا_id);
    (خالی)Curl_ngh2_session_سیٹ_دھارا_user_ڈیٹا(
      &ctx->ngh2, دھارا_id, تہی);
    واپس NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  اگر(!دھارا) {
    CURL_TRC_CF(ڈیٹا_s, cf,
                "[%d] on_stream_close, GOOD easy but no stream", دھارا_id);
    واپس NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  دھارا->closed = TRUE;
  دھارا->error = error_رمز;
  اگر(دھارا->error) {
    دھارا->reset = TRUE;
    اگر((دھارا->error == NGHTTP2_REFUSED_دھارا) &&
       ctx->rcvd_goaway && ctx->goaway_error)
    /* REFUSED_STREAM is used by nghttp2 on a GOAWAY from the
     * server where it indicated it will no longer process this stream.
     * If the GOAWAY carried an error code, take that as the error for
     * the stream. */
    دھارا->error = ctx->goaway_error;
  }

  اگر(دھارا->error)
    CURL_TRC_CF(ڈیٹا_s, cf, "[%d] RESET: %s (err %u)",
              دھارا_id, nghttp2_http2_strerror(error_رمز), error_رمز);
  ورنہ
    CURL_TRC_CF(ڈیٹا_s, cf, "[%d] CLOSED", دھارا_id);
  Curl_multi_mark_dirty(ڈیٹا_s);

  /* remove `stream_ctx` from the nghttp2 stream */
  rv = Curl_ngh2_session_سیٹ_دھارا_user_ڈیٹا(
    &ctx->ngh2, دھارا_id, تہی);
  اگر(rv) {
    infof(ڈیٹا_s, "http/2: failed to clear user_data for stream %d",
          دھارا_id);
    DEBUGASSERT(0);
  }
  واپس 0;
}

ساکن خالی cf_h2_سرخی_error(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا,
                               ساخت h2_دھارا_ctx *دھارا,
                               CURLcode نتیجہ)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;

  failf(ڈیٹا, "Error receiving HTTP2 header: %d(%s)", (صحیح)نتیجہ,
        curl_easy_strerror(نتیجہ));
  اگر(دھارا) {
    Curl_ngh2_submit_rst_دھارا(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, دھارا->id, NGHTTP2_دھارا_CLOSED);
    دھارا->closed = TRUE;
    دھارا->reset = TRUE;
  }
}

/* frame->hd.type is either NGHTTP2_HEADERS or NGHTTP2_PUSH_PROMISE */
ساکن صحیح on_سرخی(nghttp2_session *session, ثابت nghttp2_چوکھٹا *چوکھٹا,
                     ثابت uint8_t *name, size_t namelen,
                     ثابت uint8_t *قدر, size_t valuelen,
                     uint8_t flags,
                     خالی *userp)
{
  ساخت Curl_cfilter *cf = userp;
  ساخت cf_h2_ctx *ctx = cf->ctx;
  ساخت h2_دھارا_ctx *دھارا;
  ساخت Curl_easy *ڈیٹا;
  int32_t دھارا_id = چوکھٹا->hd.دھارا_id;
  CURLcode نتیجہ;

  (خالی)session;
  (خالی)flags;

  DEBUGASSERT(دھارا_id); /* should never be a zero stream ID here */

  /* get the stream from the hash based on Stream ID */
  ڈیٹا = cf_h2_get_دھارا_xfer(cf, دھارا_id, &دھارا);
  اگر(!GOOD_EASY_HANDLE(ڈیٹا))
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    واپس NGHTTP2_ERR_CALLBACK_FAILURE;

  اگر(!دھارا) {
    failf(ڈیٹا, "Internal NULL stream");
    واپس NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  /* Store received PUSH_PROMISE headers to be used when the subsequent
     PUSH_PROMISE callback comes */
  اگر(چوکھٹا->hd.type == NGHTTP2_ڈالو_PROMISE) {
    حرف *h;

    اگر((namelen == CURL_CSTRLEN(HTTP_PSEUDO_AUTHORITY)) &&
       !لڑی_موازنہ(HTTP_PSEUDO_AUTHORITY, (ثابت حرف *)name, namelen)) {
      /* pseudo headers are lower case */
      صحیح rc = 0;
      حرف *check = curl_maprintf("%s:%d", ڈیٹا->state.origin->hostname,
                                  ڈیٹا->state.origin->پورٹ);
      اگر(!check)
        /* no memory */
        واپس NGHTTP2_ERR_CALLBACK_FAILURE;
      اگر(!curl_strequal(check, (ثابت حرف *)قدر) &&
         ((ڈیٹا->state.origin->پورٹ != ڈیٹا->state.origin->scheme->defport) ||
          !curl_strequal(ڈیٹا->state.origin->hostname, (ثابت حرف *)قدر))) {
        /* This is push is not for the same authority that was asked for in
         * the URL. RFC 7540 section 8.2 says: "A client MUST treat a
         * PUSH_PROMISE for which the server is not authoritative as a stream
         * error of type PROTOCOL_ERROR."
         */
        (خالی)Curl_ngh2_submit_rst_دھارا(
          &ctx->ngh2, NGHTTP2_FLAG_NONE, دھارا_id, NGHTTP2_PROTOCOL_ERROR);
        rc = NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      curlx_free(check);
      اگر(rc)
        واپس rc;
    }

    اگر(!دھارا->ڈالو_سرخیاں) {
      دھارا->ڈالو_سرخیاں_alloc = 10;
      دھارا->ڈالو_سرخیاں = curlx_malloc(دھارا->ڈالو_سرخیاں_alloc *
                                          حجم(حرف *));
      اگر(!دھارا->ڈالو_سرخیاں)
        واپس NGHTTP2_ERR_CALLBACK_FAILURE;
      دھارا->ڈالو_سرخیاں_مستعمل = 0;
    }
    ورنہ اگر(دھارا->ڈالو_سرخیاں_مستعمل ==
            دھارا->ڈالو_سرخیاں_alloc) {
      حرف **headp;
      اگر(دھارا->ڈالو_سرخیاں_alloc > 1000) {
        /* this is beyond crazy many headers, bail out */
        failf(ڈیٹا, "Too many PUSH_PROMISE headers");
        free_ڈالو_سرخیاں(دھارا);
        واپس NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      دھارا->ڈالو_سرخیاں_alloc *= 2;
      headp = curlx_realloc(دھارا->ڈالو_سرخیاں,
                            دھارا->ڈالو_سرخیاں_alloc * حجم(حرف *));
      اگر(!headp) {
        free_ڈالو_سرخیاں(دھارا);
        واپس NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      دھارا->ڈالو_سرخیاں = headp;
    }
    h = curl_maprintf("%s:%s", name, قدر);
    اگر(!h) {
      free_ڈالو_سرخیاں(دھارا);
      واپس NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    دھارا->ڈالو_سرخیاں[دھارا->ڈالو_سرخیاں_مستعمل++] = h;
    واپس 0;
  }

  اگر(دھارا->bodystarted) {
    /* This is a trailer */
    CURL_TRC_CF(ڈیٹا, cf, "[%d] trailer: %.*s: %.*s",
                دھارا->id, (صحیح)namelen, name, (صحیح)valuelen, قدر);
    نتیجہ = Curl_dynhds_جوڑو(&دھارا->resp_trailers,
                             (ثابت حرف *)name, namelen,
                             (ثابت حرف *)قدر, valuelen);
    اگر(نتیجہ) {
      cf_h2_سرخی_error(cf, ڈیٹا, دھارا, نتیجہ);
      واپس NGHTTP2_ERR_CALLBACK_FAILURE;
    }

    واپس 0;
  }

  اگر(namelen == CURL_CSTRLEN(HTTP_PSEUDO_حالت) &&
     !memcmp(HTTP_PSEUDO_حالت, name, namelen)) {
    /* nghttp2 guarantees :status is received first and only once. */
    حرف بفر[32];
    size_t hlen;
    نتیجہ = Curl_http_decode_حالت(&دھارا->حالت_رمز,
                                     (ثابت حرف *)قدر, valuelen);
    اگر(نتیجہ) {
      cf_h2_سرخی_error(cf, ڈیٹا, دھارا, نتیجہ);
      واپس NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    hlen = curl_msnprintf(بفر, حجم(بفر), HTTP_PSEUDO_حالت ":%d\r",
                          دھارا->حالت_رمز);
    نتیجہ = Curl_سرخیاں_ڈالو(ڈیٹا, بفر, hlen, CURLH_PSEUDO);
    اگر(نتیجہ) {
      cf_h2_سرخی_error(cf, ڈیٹا, دھارا, نتیجہ);
      واپس NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    curlx_dyn_reset(&ctx->scratch);
    نتیجہ = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/2 "));
    اگر(!نتیجہ)
      نتیجہ = curlx_dyn_addn(&ctx->scratch, قدر, valuelen);
    اگر(!نتیجہ)
      نتیجہ = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    اگر(!نتیجہ)
      h2_xfer_write_resp_hd(cf, ڈیٹا, دھارا, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    اگر(نتیجہ) {
      cf_h2_سرخی_error(cf, ڈیٹا, دھارا, نتیجہ);
      واپس NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    /* if we receive data for another handle, wake that up */
    اگر(CF_ڈیٹا_CURRENT(cf) != ڈیٹا)
      Curl_multi_mark_dirty(ڈیٹا);

    CURL_TRC_CF(ڈیٹا, cf, "[%d] status: HTTP/2 %03d",
                دھارا->id, دھارا->حالت_رمز);
    واپس 0;
  }

  /* nghttp2 guarantees that namelen > 0, and :status was already
     received, and this is not pseudo-header field . */
  /* convert to an HTTP1-style header */
  curlx_dyn_reset(&ctx->scratch);
  نتیجہ = curlx_dyn_addn(&ctx->scratch, (ثابت حرف *)name, namelen);
  اگر(!نتیجہ)
    نتیجہ = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
  اگر(!نتیجہ)
    نتیجہ = curlx_dyn_addn(&ctx->scratch, (ثابت حرف *)قدر, valuelen);
  اگر(!نتیجہ)
    نتیجہ = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
  اگر(!نتیجہ)
    h2_xfer_write_resp_hd(cf, ڈیٹا, دھارا, curlx_dyn_ptr(&ctx->scratch),
                          curlx_dyn_len(&ctx->scratch), FALSE);
  اگر(نتیجہ) {
    cf_h2_سرخی_error(cf, ڈیٹا, دھارا, نتیجہ);
    واپس NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  /* if we receive data for another handle, wake that up */
  اگر(CF_ڈیٹا_CURRENT(cf) != ڈیٹا)
    Curl_multi_mark_dirty(ڈیٹا);

  CURL_TRC_CF(ڈیٹا, cf, "[%d] header: %.*s: %.*s",
              دھارا->id, (صحیح)namelen, name, (صحیح)valuelen, قدر);

  واپس 0; /* 0 is successful */
}

ساکن ssize_t req_body_read_callback(nghttp2_session *session,
                                      int32_t دھارا_id,
                                      uint8_t *buf, size_t لمبائی,
                                      uint32_t *ڈیٹا_flags,
                                      nghttp2_ڈیٹا_ماخذ *ماخذ,
                                      خالی *userp)
{
  ساخت Curl_cfilter *cf = userp;
  ساخت Curl_easy *ڈیٹا_s;
  ساخت h2_دھارا_ctx *دھارا = تہی;
  CURLcode نتیجہ;
  ssize_t nread;
  size_t n;
  (خالی)ماخذ;

  (خالی)session;
  اگر(!دھارا_id)
    واپس NGHTTP2_ERR_INVALID_دلیل;

  /* get the stream from the hash based on Stream ID, stream ID zero is for
     connection-oriented stuff */
  ڈیٹا_s = cf_h2_get_دھارا_xfer(cf, دھارا_id, &دھارا);
  اگر(!ڈیٹا_s || !دھارا)
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    واپس NGHTTP2_ERR_CALLBACK_FAILURE;

  نتیجہ = Curl_bufq_read(&دھارا->sendbuf, buf, لمبائی, &n);
  اگر(نتیجہ) {
    اگر(نتیجہ != CURLE_AGAIN)
      واپس NGHTTP2_ERR_CALLBACK_FAILURE;
    nread = 0;
  }
  ورنہ
    nread = (ssize_t)n;

  CURL_TRC_CF(ڈیٹا_s, cf, "[%d] req_body_read(len=%zu) eos=%d -> %zd, %d",
              دھارا_id, لمبائی, دھارا->body_eos, nread, (صحیح)نتیجہ);

  اگر(دھارا->body_eos && Curl_bufq_is_خالی_خانے(&دھارا->sendbuf)) {
    *ڈیٹا_flags = NGHTTP2_ڈیٹا_FLAG_EOF;
    واپس nread;
  }
  واپس (nread == 0) ? NGHTTP2_ERR_DEFERRED : nread;
}

#اگر_تعریف_شدہ CURLVERBOSE
ساکن صحیح error_callback(nghttp2_session *session,
                          ثابت حرف *پیغام,
                          size_t len,
                          خالی *userp)
{
  ساخت Curl_cfilter *cf = userp;
  ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);
  (خالی)session;
  failf(ڈیٹا, "%.*s", (صحیح)len, پیغام);
  واپس 0;
}
#اگر_ختم

/*
 * Append headers to ask for an HTTP1.1 to HTTP2 upgrade.
 */
CURLcode Curl_http2_request_upgrade(ساخت dynbuf *req,
                                    ساخت Curl_easy *ڈیٹا)
{
  CURLcode نتیجہ;
  حرف *base64;
  size_t blen;
  ساخت SingleRequest *k = &ڈیٹا->req;
  uint8_t binsettings[H2_BINSETTINGS_LEN];
  ssize_t rc;
  size_t binlen; /* length of the binsettings data */

  rc = populate_binsettings(binsettings, ڈیٹا);
  اگر(!curlx_sztouz(rc, &binlen) || !binlen) {
    failf(ڈیٹا, "nghttp2 unexpectedly failed on pack_settings_payload");
    curlx_dyn_free(req);
    واپس CURLE_FAILED_INIT;
  }

  نتیجہ = curlx_base64url_encode(binsettings, binlen, &base64, &blen);
  اگر(نتیجہ) {
    curlx_dyn_free(req);
    واپس نتیجہ;
  }

  ڈیٹا->state.http_hd_upgrade = TRUE;
  ڈیٹا->state.http_hd_h2_settings = TRUE;
  نتیجہ = curlx_dyn_addf(req,
                          "Upgrade: %s\r\n"
                          "HTTP2-Settings: %s\r\n",
                          NGHTTP2_CLEARTEXT_PROTO_نسخہ_ID, base64);
  curlx_free(base64);

  k->upgr101 = UPGR101_H2;
  ڈیٹا->conn->بٹ.upgrade_اندر_پیش_رفت = TRUE;

  واپس نتیجہ;
}

ساکن CURLcode http2_handle_دھارا_close(ساخت Curl_cfilter *cf,
                                          ساخت Curl_easy *ڈیٹا,
                                          ساخت h2_دھارا_ctx *دھارا,
                                          size_t *pnlen)
{
  CURLcode نتیجہ;

  *pnlen = 0;
  اگر(دھارا->reset) {
    اگر(دھارا->error == NGHTTP2_REFUSED_دھارا) {
      infof(ڈیٹا, "HTTP/2 stream %d refused by server, try again on a new "
                  "connection", دھارا->id);
      connclose(cf->conn); /* do not use this anymore */
      ڈیٹا->state.refused_دھارا = TRUE;
      واپس CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    ورنہ اگر(دھارا->resp_hds_complete && ڈیٹا->req.no_body) {
      CURL_TRC_CF(ڈیٹا, cf, "[%d] error after response headers, but we did "
                  "not want a body anyway, ignore: %s (err %u)",
                  دھارا->id, nghttp2_http2_strerror(دھارا->error),
                  دھارا->error);
      دھارا->close_handled = TRUE;
      واپس CURLE_OK;
    }
    failf(ڈیٹا, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          دھارا->id, دھارا->reset_by_server ? "server" : "curl",
          دھارا->error, nghttp2_http2_strerror(دھارا->error));
    واپس دھارا->error ? CURLE_HTTP2_دھارا :
           (ڈیٹا->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2);
  }
  ورنہ اگر(!دھارا->bodystarted) {
    failf(ڈیٹا, "HTTP/2 stream %d was closed cleanly, but before getting "
          "all response header fields, treated as error", دھارا->id);
    واپس CURLE_HTTP2_دھارا;
  }

  اگر(Curl_dynhds_گنتی(&دھارا->resp_trailers)) {
    ساخت dynhds_اندراج *e;
    ساخت dynbuf dbuf;
    size_t i;

    نتیجہ = CURLE_OK;
    curlx_dyn_init(&dbuf, DYN_TRAILERS);
    برائے(i = 0; i < Curl_dynhds_گنتی(&دھارا->resp_trailers); ++i) {
      e = Curl_dynhds_getn(&دھارا->resp_trailers, i);
      اگر(!e)
        توڑو;
      curlx_dyn_reset(&dbuf);
      نتیجہ = curlx_dyn_addf(&dbuf, "%.*s: %.*s\x0d\x0a",
                              (صحیح)e->namelen, e->name,
                              (صحیح)e->valuelen, e->قدر);
      اگر(نتیجہ)
        توڑو;
      Curl_debug(ڈیٹا, CURLINFO_سرخی_اندر, curlx_dyn_ptr(&dbuf),
                 curlx_dyn_len(&dbuf));
      نتیجہ = Curl_client_write(ڈیٹا,
                                 CLIENTWRITE_سرخی | CLIENTWRITE_TRAILER,
                                 curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf));
      اگر(نتیجہ)
        توڑو;
      نتیجہ = Curl_bump_headersize(ڈیٹا, curlx_dyn_len(&dbuf), FALSE);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
    curlx_dyn_free(&dbuf);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  دھارا->close_handled = TRUE;
  نتیجہ = CURLE_OK;

برآمد:
  CURL_TRC_CF(ڈیٹا, cf, "handle_stream_close -> %d, %zu", (صحیح)نتیجہ, *pnlen);
  واپس نتیجہ;
}

ساکن صحیح sweight_wanted(ثابت ساخت Curl_easy *ڈیٹا)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  واپس ڈیٹا->سیٹ.وزن ?
    ڈیٹا->سیٹ.وزن : NGHTTP2_DEFAULT_وزن;
}

ساکن صحیح sweight_اندر_effect(ثابت ساخت Curl_easy *ڈیٹا)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  واپس ڈیٹا->state.وزن ?
    ڈیٹا->state.وزن : NGHTTP2_DEFAULT_وزن;
}

/*
 * h2_pri_spec() fills in the pri_spec struct, used by nghttp2 to send weight
 * and dependency to the peer. It also stores the updated values in the state
 * struct.
 */

ساکن خالی h2_pri_spec(ساخت Curl_easy *ڈیٹا,
                        nghttp2_ترجیح_spec *pri_spec)
{
  صحیح prio = ڈیٹا->سیٹ.وزن;
  nghttp2_ترجیح_spec_init(pri_spec, 0, sweight_wanted(ڈیٹا), FALSE);
  ڈیٹا->state.وزن = prio;
}

/*
 * Check if there is been an update in the priority /
 * dependency settings and if so it submits a PRIORITY frame with the updated
 * info.
 * Flush any out data pending in the network buffer.
 */
ساکن CURLcode h2_پیش_رفت_egress(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;
  صحیح rv = 0;

  اگر(sweight_wanted(ڈیٹا) != sweight_اندر_effect(ڈیٹا)) {
    ساخت h2_دھارا_ctx *دھارا = H2_دھارا_CTX(ctx, ڈیٹا);
    اگر(دھارا && دھارا->id > 0 &&
       (sweight_wanted(ڈیٹا) != sweight_اندر_effect(ڈیٹا))) {
      /* send new weight and/or dependency */
      nghttp2_ترجیح_spec pri_spec;

      h2_pri_spec(ڈیٹا, &pri_spec);
      CURL_TRC_CF(ڈیٹا, cf, "[%d] Queuing PRIORITY", دھارا->id);
      DEBUGASSERT(دھارا->id != -1);
      rv = Curl_ngh2_submit_ترجیح(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, دھارا->id, &pri_spec);
      اگر(rv)
        جاؤ برآمد;
    }
  }

  ctx->nw_برآمد_blocked = 0;
  جب(!rv && !ctx->nw_برآمد_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

برآمد:
  اگر(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(ڈیٹا, cf, "nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    واپس CURLE_SEND_ERROR;
  }
  /* Defer flushing during the connect phase so that the SETTINGS and
   * other initial frames are sent together with the first request.
   * Unless we are 'connect_only' where the request will never come. */
  اگر(!cf->connected && !cf->conn->بٹ.connect_only)
    واپس CURLE_OK;
  واپس nw_برآمد_flush(cf, ڈیٹا);
}

ساکن CURLcode دھارا_recv(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                            ساخت h2_دھارا_ctx *دھارا,
                            حرف *buf, size_t len, size_t *pnread)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_AGAIN;

  (خالی)buf;
  (خالی)len;
  *pnread = 0;

  اگر(!دھارا->xfer_نتیجہ)
    دھارا->xfer_نتیجہ = cf_h2_update_local_win(cf, ڈیٹا, دھارا);

  اگر(دھارا->xfer_نتیجہ) {
    CURL_TRC_CF(ڈیٹا, cf, "[%d] xfer write failed", دھارا->id);
    نتیجہ = دھارا->xfer_نتیجہ;
  }
  ورنہ اگر(دھارا->closed) {
    CURL_TRC_CF(ڈیٹا, cf, "[%d] returning CLOSE", دھارا->id);
    نتیجہ = http2_handle_دھارا_close(cf, ڈیٹا, دھارا, pnread);
  }
  ورنہ اگر(دھارا->reset ||
          (ctx->conn_closed && Curl_bufq_is_خالی_خانے(&ctx->inbufq)) ||
          (ctx->rcvd_goaway && ctx->remote_max_sid < دھارا->id)) {
    CURL_TRC_CF(ڈیٹا, cf, "[%d] returning ERR", دھارا->id);
    نتیجہ = ڈیٹا->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2;
  }

  اگر(نتیجہ && (نتیجہ != CURLE_AGAIN))
    CURL_TRC_CF(ڈیٹا, cf, "[%d] stream_recv(len=%zu) -> %d, %zu",
                دھارا->id, len, (صحیح)نتیجہ, *pnread);
  واپس نتیجہ;
}

ساکن CURLcode h2_پیش_رفت_ingress(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا,
                                    size_t ڈیٹا_max_بائٹیں)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;
  size_t nread;

  اگر(should_close_session(ctx)) {
    CURL_TRC_CF(ڈیٹا, cf, "[0] ingress: session is closed");
    واپس CURLE_HTTP2;
  }

  /* Process network input buffer first */
  اگر(!Curl_bufq_is_خالی_خانے(&ctx->inbufq)) {
    CURL_TRC_CF(ڈیٹا, cf, "Process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    نتیجہ = h2_process_pending_داخلہ(cf, ڈیٹا);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  اگر(!ڈیٹا_max_بائٹیں)
    ڈیٹا_max_بائٹیں = H2_CHUNK_سائز;

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop due to connection close or us not processing
   * all network input */
  جب(!ctx->conn_closed && Curl_bufq_is_خالی_خانے(&ctx->inbufq)) {
    ساخت h2_دھارا_ctx *دھارا = H2_دھارا_CTX(ctx, ڈیٹا);
    اگر(دھارا && (دھارا->closed || !ڈیٹا_max_بائٹیں)) {
      /* We would like to abort here and stop processing, so that the transfer
       * loop can handle the data/close here. This may leave data in
       * underlying buffers that will not be consumed. */
      اگر(!cf->اگلا || !cf->اگلا->cft->has_ڈیٹا_pending(cf->اگلا, ڈیٹا))
        Curl_multi_mark_dirty(ڈیٹا);
      توڑو;
    }

    نتیجہ = Curl_cf_recv_bufq(cf->اگلا, ڈیٹا, &ctx->inbufq, 0, &nread);
    اگر(نتیجہ) {
      اگر(نتیجہ != CURLE_AGAIN) {
        failf(ڈیٹا, "Failed receiving HTTP2 data: %d(%s)", (صحیح)نتیجہ,
              curl_easy_strerror(نتیجہ));
        واپس نتیجہ;
      }
      توڑو;
    }
    ورنہ اگر(nread == 0) {
      CURL_TRC_CF(ڈیٹا, cf, "[0] ingress: connection closed");
      ctx->conn_closed = TRUE;
      توڑو;
    }
    ورنہ {
      CURL_TRC_CF(ڈیٹا, cf, "[0] ingress: read %zu bytes", nread);
      ڈیٹا_max_بائٹیں = (ڈیٹا_max_بائٹیں > nread) ? (ڈیٹا_max_بائٹیں - nread) : 0;
    }

    نتیجہ = h2_process_pending_داخلہ(cf, ڈیٹا);
    اگر(نتیجہ)
      واپس نتیجہ;
    CURL_TRC_CF(ڈیٹا, cf, "[0] ingress: nw-in buffered %zu",
                Curl_bufq_len(&ctx->inbufq));
  }

  اگر(ctx->conn_closed && Curl_bufq_is_خالی_خانے(&ctx->inbufq)) {
    CURL_TRC_CF(ڈیٹا, cf, "server closed %s",
                ctx->rcvd_goaway ? "with GOAWAY" : "abruptly");
    connclose(cf->conn);
  }

  CURL_TRC_CF(ڈیٹا, cf, "[0] ingress: done");
  واپس CURLE_OK;
}

ساکن CURLcode cf_h2_recv(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                           حرف *buf, size_t len, size_t *pnread)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;
  ساخت h2_دھارا_ctx *دھارا;
  CURLcode نتیجہ, r2;
  ساخت cf_call_ڈیٹا save;

  اگر(!ڈیٹا)
    واپس CURLE_HTTP2;

  دھارا = H2_دھارا_CTX(ctx, ڈیٹا);

  *pnread = 0;
  اگر(!دھارا) {
    /* Abnormal call sequence: either this transfer has never opened a stream
     * (unlikely) or the transfer has been done, cleaned up its resources, but
     * a read() is called anyway. It is not clear what the calling sequence
     * is for such a case. */
    failf(ڈیٹا, "http/2 recv on a transfer never opened "
          "or already cleared, mid=%u", ڈیٹا->mid);
    واپس CURLE_HTTP2;
  }

  CF_ڈیٹا_SAVE(save, cf, ڈیٹا);

  نتیجہ = دھارا_recv(cf, ڈیٹا, دھارا, buf, len, pnread);
  اگر(نتیجہ && (نتیجہ != CURLE_AGAIN))
    جاؤ برآمد;

  اگر(نتیجہ) {
    نتیجہ = h2_پیش_رفت_ingress(cf, ڈیٹا, len);
    اگر(نتیجہ)
      جاؤ برآمد;

    نتیجہ = دھارا_recv(cf, ڈیٹا, دھارا, buf, len, pnread);
  }

  اگر(*pnread > 0) {
    /* Now that we transferred this to the upper layer, we report
     * the actual amount of DATA consumed to the H2 session, so
     * that it adjusts stream flow control */
    Curl_ngh2_session_consume(&ctx->ngh2, دھارا->id, *pnread);
    اگر(دھارا->closed) {
      CURL_TRC_CF(ڈیٹا, cf, "[%d] DRAIN closed stream", دھارا->id);
      Curl_multi_mark_dirty(ڈیٹا);
    }
  }

برآمد:
  r2 = h2_پیش_رفت_egress(cf, ڈیٹا);
  اگر(r2 == CURLE_AGAIN) {
    /* pending data to send, need to be called again. Ideally, we
     * monitor the socket for POLLOUT, but when not SENDING
     * any more, we force processing of the transfer. */
    اگر(!CURL_REQ_WANT_SEND(ڈیٹا))
      Curl_multi_mark_dirty(ڈیٹا);
  }
  ورنہ اگر(r2 && (نتیجہ == CURLE_AGAIN)) {
    /* Do not replace a previous error result NOR a successful result
     * with an egress error. Example: the server might close the
     * connection after sending the complete response with EOS
     * and everything. Further sending may fail, but that MUST
     * not change that we had a successful response. */
    نتیجہ = r2;
  }

  CURL_TRC_CF(ڈیٹا, cf, "[%d] cf_recv(len=%zu) -> %d, %zu, "
              "window=%d/%d, connection %d/%d",
              دھارا->id, len, (صحیح)نتیجہ, *pnread,
              Curl_ngh2_session_get_دھارا_effective_recv_ڈیٹا_لمبائی(
                &ctx->ngh2, دھارا->id),
              Curl_ngh2_session_get_دھارا_effective_local_کھڑکی_سائز(
                &ctx->ngh2, دھارا->id),
              Curl_ngh2_session_get_local_کھڑکی_سائز(&ctx->ngh2),
              HTTP2_HUGE_کھڑکی_سائز);

  CF_ڈیٹا_RESTORE(cf, save);
  واپس نتیجہ;
}

ساکن CURLcode cf_h2_body_send(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا,
                                ساخت h2_دھارا_ctx *دھارا,
                                ثابت خالی *buf, size_t blen, bool eos,
                                size_t *pnwritten)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;
  CURLcode نتیجہ;

  *pnwritten = 0;
  اگر(دھارا->closed) {
    اگر(دھارا->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(ڈیٹا, cf, "[%d] discarding data"
                  "on closed stream with response", دھارا->id);
      اگر(eos)
        دھارا->body_eos = TRUE;
      *pnwritten = blen;
      واپس CURLE_OK;
    }
    /* Server closed before we got a response, this is an error */
    infof(ڈیٹا, "stream %d closed", دھارا->id);
    واپس CURLE_SEND_ERROR;
  }

  نتیجہ = Curl_bufq_write(&دھارا->sendbuf, buf, blen, pnwritten);
  اگر(نتیجہ)
    واپس نتیجہ;

  اگر(eos && (blen == *pnwritten))
    دھارا->body_eos = TRUE;

  اگر(eos || !Curl_bufq_is_خالی_خانے(&دھارا->sendbuf)) {
    /* resume the potentially suspended stream */
    اگر(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_ڈیٹا(&ctx->ngh2, دھارا->id)))
      واپس CURLE_SEND_ERROR;
  }

  واپس CURLE_OK;
}

ساکن CURLcode h2_submit(ساخت h2_دھارا_ctx **pstream,
                          ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                          ثابت uint8_t *buf, size_t len,
                          bool eos, size_t *pnwritten)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;
  ساخت h2_دھارا_ctx *دھارا = تہی;
  ساخت dynhds h2_سرخیاں;
  nghttp2_nv *nva = تہی;
  ثابت خالی *body = تہی;
  size_t nheader, bodylen;
  nghttp2_ڈیٹا_provider ڈیٹا_prd;
  int32_t دھارا_id;
  nghttp2_ترجیح_spec pri_spec;
  size_t nwritten;
  CURLcode نتیجہ = CURLE_OK;
  uint32_t initial_win_سائز;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_سرخیاں, MAX_HTTP_RESP_سرخی_گنتی, DYN_HTTP_REQUEST);

  نتیجہ = http2_ڈیٹا_تیاری(cf, ڈیٹا, &دھارا);
  اگر(نتیجہ)
    جاؤ برآمد;

  نتیجہ = Curl_h1_req_parse_read(
    &دھارا->h1, buf, len, تہی,
    !ڈیٹا->state.http_ignorecustom ?
    CURL_EASY_STR(ڈیٹا, STRING_CUSTOMREQUEST) : تہی,
    0, &nwritten);
  اگر(نتیجہ)
    جاؤ برآمد;
  *pnwritten = nwritten;
  اگر(!دھارا->h1.مکمل) {
    /* need more data */
    جاؤ برآمد;
  }
  DEBUGASSERT(دھارا->h1.req);

  نتیجہ = Curl_http_req_مقصد_h2(&h2_سرخیاں, دھارا->h1.req, ڈیٹا);
  اگر(نتیجہ)
    جاؤ برآمد;
  /* no longer needed */
  Curl_h1_req_parse_free(&دھارا->h1);

  nva = Curl_dynhds_مقصد_nva(&h2_سرخیاں, &nheader);
  اگر(!nva) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  h2_pri_spec(ڈیٹا, &pri_spec);
  اگر(!Curl_ngh2_session_check_request_allowed(&ctx->ngh2))
    CURL_TRC_CF(ڈیٹا, cf, "send request NOT allowed (via nghttp2)");

  /* Check the initial windows size of the transfer (rate-limits?) and
   * send an updated settings on changes from previous value. */
  initial_win_سائز = cf_h2_initial_win_سائز(ڈیٹا);
  اگر(initial_win_سائز != ctx->initial_win_سائز) {
    نتیجہ = cf_h2_update_settings(ctx, initial_win_سائز);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  انتخاب(ڈیٹا->state.httpreq) {
  حالت HTTPREQ_POST:
  حالت HTTPREQ_POST_FORM:
  حالت HTTPREQ_POST_MIME:
  حالت HTTPREQ_PUT:
    ڈیٹا_prd.read_callback = req_body_read_callback;
    ڈیٹا_prd.ماخذ.ptr = تہی;
    دھارا_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, &ڈیٹا_prd, دھارا);
    توڑو;
  طےشدہ:
    دھارا_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, تہی, دھارا);
  }

  اگر(دھارا_id < 0) {
    CURL_TRC_CF(ڈیٹا, cf, "send: nghttp2_submit_request error (%s)%d",
                nghttp2_strerror(دھارا_id), دھارا_id);
    نتیجہ = CURLE_SEND_ERROR;
    جاؤ برآمد;
  }

#اگر_تعریف_شدہ CURLVERBOSE
#تعریف_کرو MAX_ACC 60000  /* <64KB to account for some overhead */
  اگر(Curl_trc_is_verbose(ڈیٹا)) {
    size_t acc = 0, i;

    infof(ڈیٹا, "[HTTP/2] [%d] OPENED stream for %s",
          دھارا_id, Curl_bufref_ptr(&ڈیٹا->state.url));
    برائے(i = 0; i < nheader; ++i) {
      acc += nva[i].namelen + nva[i].valuelen;

      infof(ڈیٹا, "[HTTP/2] [%d] [%.*s: %.*s]", دھارا_id,
            (صحیح)nva[i].namelen, nva[i].name,
            (صحیح)nva[i].valuelen, nva[i].قدر);
    }

    اگر(acc > MAX_ACC) {
      infof(ڈیٹا, "[HTTP/2] Warning: The cumulative length of all "
            "headers exceeds %d bytes and that could cause the "
            "stream to be rejected.", MAX_ACC);
    }
  }
#اگر_ختم

  دھارا->id = دھارا_id;

  body = (ثابت حرف *)buf + *pnwritten;
  bodylen = len - *pnwritten;

  اگر(bodylen || eos) {
    size_t n;
    نتیجہ = cf_h2_body_send(cf, ڈیٹا, دھارا, body, bodylen, eos, &n);
    اگر(!نتیجہ)
      *pnwritten += n;
    ورنہ اگر(نتیجہ == CURLE_AGAIN)
      نتیجہ = CURLE_OK;
    ورنہ {
      نتیجہ = CURLE_SEND_ERROR;
    }
  }

برآمد:
  CURL_TRC_CF(ڈیٹا, cf, "[%d] submit -> %d, %zu",
              دھارا ? دھارا->id : -1, (صحیح)نتیجہ, *pnwritten);
  curlx_safefree(nva);
  *pstream = دھارا;
  Curl_dynhds_free(&h2_سرخیاں);
  واپس نتیجہ;
}

ساکن CURLcode cf_h2_send(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                           ثابت uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;
  ساخت h2_دھارا_ctx *دھارا = H2_دھارا_CTX(ctx, ڈیٹا);
  ساخت cf_call_ڈیٹا save;
  CURLcode نتیجہ = CURLE_OK, r2;

  CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
  *pnwritten = 0;

  اگر(!دھارا || دھارا->id == -1) {
    نتیجہ = h2_submit(&دھارا, cf, ڈیٹا, buf, len, eos, pnwritten);
    اگر(نتیجہ)
      جاؤ برآمد;
    DEBUGASSERT(دھارا);
  }
  ورنہ اگر(دھارا->body_eos) {
    /* We already wrote this, but CURLE_AGAIN-ed the call due to not
     * being able to flush stream->sendbuf. Make a 0-length write
     * to trigger flushing again.
     * If this works, we report to have written `len` bytes. */
    size_t n;
    DEBUGASSERT(eos);
    نتیجہ = cf_h2_body_send(cf, ڈیٹا, دھارا, buf, 0, eos, &n);
    CURL_TRC_CF(ڈیٹا, cf, "[%d] cf_body_send last CHUNK -> %d, %zu, eos=%d",
                دھارا->id, (صحیح)نتیجہ, n, eos);
    اگر(نتیجہ)
      جاؤ برآمد;
    *pnwritten = len;
  }
  ورنہ {
    نتیجہ = cf_h2_body_send(cf, ڈیٹا, دھارا, buf, len, eos, pnwritten);
    CURL_TRC_CF(ڈیٹا, cf, "[%d] cf_body_send(len=%zu) -> %d, %zu, eos=%d",
                دھارا->id, len, (صحیح)نتیجہ, *pnwritten, eos);
  }

  /* Call the nghttp2 send loop and flush to write ALL buffered data,
   * headers and/or request body completely out to the network */
  r2 = h2_پیش_رفت_egress(cf, ڈیٹا);

  /* if the stream has been closed in egress handling (nghttp2 does that
   * when it does not like the headers, for example */
  اگر(دھارا && دھارا->closed && !دھارا->resp_hds_complete) {
    infof(ڈیٹا, "stream %d closed", دھارا->id);
    نتیجہ = CURLE_SEND_ERROR;
    جاؤ برآمد;
  }
  ورنہ اگر(r2 && (r2 != CURLE_AGAIN)) {
    نتیجہ = r2;
    جاؤ برآمد;
  }

  اگر(should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    اگر(دھارا && دھارا->closed) {
      نتیجہ = http2_handle_دھارا_close(cf, ڈیٹا, دھارا, pnwritten);
    }
    ورنہ {
      CURL_TRC_CF(ڈیٹا, cf, "send: nothing to do in this session");
      نتیجہ = CURLE_HTTP2;
    }
  }

برآمد:
  اگر(دھارا) {
    CURL_TRC_CF(ڈیٹا, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
                "eos=%d, h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                دھارا->id, len, (صحیح)نتیجہ, *pnwritten,
                دھارا->body_eos,
                Curl_ngh2_session_get_دھارا_remote_کھڑکی_سائز(
                  &ctx->ngh2, دھارا->id),
                Curl_ngh2_session_get_remote_کھڑکی_سائز(&ctx->ngh2),
                Curl_bufq_len(&دھارا->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  ورنہ {
    CURL_TRC_CF(ڈیٹا, cf, "cf_send(len=%zu) -> %d, %zu, "
                "connection-window=%d, nw_send_buffer(%zu)",
                len, (صحیح)نتیجہ, *pnwritten,
                Curl_ngh2_session_get_remote_کھڑکی_سائز(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_ڈیٹا_RESTORE(cf, save);
  واپس نتیجہ;
}

ساکن CURLcode cf_h2_flush(ساخت Curl_cfilter *cf,
                            ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;
  ساخت h2_دھارا_ctx *دھارا = H2_دھارا_CTX(ctx, ڈیٹا);
  ساخت cf_call_ڈیٹا save;
  CURLcode نتیجہ = CURLE_OK;

  CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
  اگر(دھارا && !Curl_bufq_is_خالی_خانے(&دھارا->sendbuf)) {
    /* resume the potentially suspended stream */
    اگر(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_ڈیٹا(&ctx->ngh2, دھارا->id))) {
      نتیجہ = CURLE_SEND_ERROR;
      جاؤ برآمد;
    }
  }

  نتیجہ = h2_پیش_رفت_egress(cf, ڈیٹا);

برآمد:
  اگر(دھارا) {
    CURL_TRC_CF(ڈیٹا, cf, "[%d] flush -> %d, "
                "h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                دھارا->id, (صحیح)نتیجہ,
                Curl_ngh2_session_get_دھارا_remote_کھڑکی_سائز(
                  &ctx->ngh2, دھارا->id),
                Curl_ngh2_session_get_remote_کھڑکی_سائز(&ctx->ngh2),
                Curl_bufq_len(&دھارا->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  ورنہ {
    CURL_TRC_CF(ڈیٹا, cf, "flush -> %d, "
                "connection-window=%d, nw_send_buffer(%zu)", (صحیح)نتیجہ,
                Curl_ngh2_session_get_remote_کھڑکی_سائز(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_ڈیٹا_RESTORE(cf, save);
  واپس نتیجہ;
}

ساکن CURLcode cf_h2_adjust_pollset(ساخت Curl_cfilter *cf,
                                     ساخت Curl_easy *ڈیٹا,
                                     ساخت easy_pollset *ps)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;
  ساخت cf_call_ڈیٹا save;
  curl_socket_t sock;
  bool want_recv, want_send;
  CURLcode نتیجہ = CURLE_OK;

  اگر(!Curl_ngh2_valid(&ctx->ngh2))
    واپس CURLE_OK;

  sock = Curl_conn_cf_get_socket(cf, ڈیٹا);
  Curl_pollset_check(ڈیٹا, ps, sock, &want_recv, &want_send);
  اگر(want_recv || want_send) {
    ساخت h2_دھارا_ctx *دھارا = H2_دھارا_CTX(ctx, ڈیٹا);
    bool c_exhaust, s_exhaust;

    CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
    c_exhaust = want_send &&
                !Curl_ngh2_session_get_remote_کھڑکی_سائز(&ctx->ngh2);
    s_exhaust = want_send && دھارا && دھارا->id >= 0 &&
                !Curl_ngh2_session_get_دھارا_remote_کھڑکی_سائز(
                  &ctx->ngh2, دھارا->id);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                (!c_exhaust && Curl_ngh2_session_want_write(&ctx->ngh2)) ||
                !Curl_bufq_is_خالی_خانے(&ctx->outbufq);

    نتیجہ = Curl_pollset_سیٹ(ڈیٹا, ps, sock, want_recv, want_send);
    CF_ڈیٹا_RESTORE(cf, save);
  }
  ورنہ اگر(ctx->بھیجا_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_خالی_خانے(&ctx->outbufq);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    نتیجہ = Curl_pollset_سیٹ(ڈیٹا, ps, sock, want_recv, want_send);
    CF_ڈیٹا_RESTORE(cf, save);
  }
  واپس نتیجہ;
}

ساکن CURLcode cf_h2_ctx_open(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;
  ساخت h2_دھارا_ctx *دھارا;
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;
  صحیح rc;
  nghttp2_session_callbacks *cbs = تہی;

  DEBUGASSERT(!ctx->ngh2.s);
  DEBUGASSERT(!ctx->ngh2.fatal_rv);
  DEBUGASSERT(ctx->initialized);

  rc = nghttp2_session_callbacks_new(&cbs);
  اگر(rc) {
    failf(ڈیٹا, "Could not initialize nghttp2 callbacks");
    جاؤ برآمد;
  }

  nghttp2_session_callbacks_سیٹ_send_callback(cbs, send_callback);
  nghttp2_session_callbacks_سیٹ_on_چوکھٹا_recv_callback(cbs, on_چوکھٹا_recv);
  nghttp2_session_callbacks_سیٹ_on_invalid_چوکھٹا_recv_callback(cbs,
    cf_h2_on_invalid_چوکھٹا_recv);
#اگر_تعریف_شدہ CURLVERBOSE
  nghttp2_session_callbacks_سیٹ_on_چوکھٹا_send_callback(cbs, on_چوکھٹا_send);
#اگر_ختم
  nghttp2_session_callbacks_سیٹ_on_ڈیٹا_chunk_recv_callback(
    cbs, on_ڈیٹا_chunk_recv);
  nghttp2_session_callbacks_سیٹ_on_دھارا_close_callback(cbs, on_دھارا_close);
  nghttp2_session_callbacks_سیٹ_on_سرخی_callback(cbs, on_سرخی);
#اگر_تعریف_شدہ CURLVERBOSE
  nghttp2_session_callbacks_سیٹ_error_callback(cbs, error_callback);
#اگر_ختم

  /* The nghttp2 session is not yet setup, do it */
  rc = h2_client_new(cf, cbs);
  اگر(rc) {
    failf(ڈیٹا, "Could not initialize nghttp2");
    جاؤ برآمد;
  }
  ctx->max_concurrent_streams = ڈیٹا->multi ?
    Curl_multi_max_concurrent_streams(ڈیٹا->multi) :
    DEFAULT_MAX_CONCURRENT_STREAMS;

  اگر(ctx->بذریعہ_h1_upgrade) {
    /* HTTP/1.1 Upgrade issued. H2 Settings have already been submitted
     * in the H1 request and we upgrade from there. This stream
     * is opened implicitly as #1. */
    uint8_t binsettings[H2_BINSETTINGS_LEN];
    ssize_t rclen;
    size_t binlen; /* length of the binsettings data */

    rclen = populate_binsettings(binsettings, ڈیٹا);

    اگر(!curlx_sztouz(rclen, &binlen) || !binlen) {
      failf(ڈیٹا, "nghttp2 unexpectedly failed on pack_settings_payload");
      نتیجہ = CURLE_FAILED_INIT;
      جاؤ برآمد;
    }

    نتیجہ = http2_ڈیٹا_تیاری(cf, ڈیٹا, &دھارا);
    اگر(نتیجہ)
      جاؤ برآمد;
    DEBUGASSERT(دھارا);
    دھارا->id = 1;
    /* queue SETTINGS frame (again) */
    rc = Curl_ngh2_session_upgrade2(
      &ctx->ngh2, binsettings, binlen, ڈیٹا->state.httpreq == HTTPREQ_سر,
      تہی);
    اگر(rc) {
      failf(ڈیٹا, "nghttp2_session_upgrade2() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      نتیجہ = CURLE_HTTP2;
      جاؤ برآمد;
    }

    rc = Curl_ngh2_session_سیٹ_دھارا_user_ڈیٹا(
      &ctx->ngh2, دھارا->id, دھارا);
    اگر(rc) {
      infof(ڈیٹا, "http/2: failed to set user_data for stream %d",
            دھارا->id);
      DEBUGASSERT(0);
    }
    CURL_TRC_CF(ڈیٹا, cf, "created session via Upgrade");
  }
  ورنہ {
    nghttp2_settings_اندراج iv[H2_SETTINGS_IV_LEN];
    size_t ivlen;

    ivlen = populate_settings(iv, ڈیٹا, ctx);
    rc = Curl_ngh2_submit_settings(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, iv, ivlen);
    اگر(rc) {
      failf(ڈیٹا, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      نتیجہ = CURLE_HTTP2;
      جاؤ برآمد;
    }
  }

  rc = Curl_ngh2_session_سیٹ_local_کھڑکی_سائز(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, HTTP2_HUGE_کھڑکی_سائز);
  اگر(rc) {
    failf(ڈیٹا, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    نتیجہ = CURLE_HTTP2;
    جاؤ برآمد;
  }

  /* all set, traffic will be send on connect */
  نتیجہ = CURLE_OK;
  CURL_TRC_CF(ڈیٹا, cf, "[0] created h2 session%s",
              ctx->بذریعہ_h1_upgrade ? " (via h1 upgrade)" : "");

برآمد:
  اگر(cbs)
    nghttp2_session_callbacks_del(cbs);
  واپس نتیجہ;
}

ساکن CURLcode cf_h2_connect(ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *ڈیٹا,
                              bool *مکمل)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;
  ساخت cf_call_ڈیٹا save;
  bool پہلا_وقت = FALSE;

  اگر(cf->connected) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  /* Connect the lower filters first */
  اگر(!cf->اگلا->connected) {
    نتیجہ = Curl_conn_cf_connect(cf->اگلا, ڈیٹا, مکمل);
    اگر(نتیجہ || !*مکمل)
      واپس نتیجہ;
  }

  *مکمل = FALSE;

  CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
  DEBUGASSERT(ctx->initialized);
  اگر(!ctx->ngh2.s) {
    نتیجہ = cf_h2_ctx_open(cf, ڈیٹا);
    اگر(نتیجہ)
      جاؤ برآمد;
    پہلا_وقت = TRUE;
  }

  اگر(!پہلا_وقت) {
    نتیجہ = h2_پیش_رفت_ingress(cf, ڈیٹا, 0);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  /* Send out our SETTINGS and ACKs and such. If that blocks, we
   * have it buffered and can count this filter as being connected */
  نتیجہ = h2_پیش_رفت_egress(cf, ڈیٹا);
  اگر(نتیجہ && (نتیجہ != CURLE_AGAIN))
    جاؤ برآمد;

  *مکمل = TRUE;
  cf->connected = TRUE;
  نتیجہ = CURLE_OK;

برآمد:
  CURL_TRC_CF(ڈیٹا, cf, "cf_connect() -> %d, %d, ", (صحیح)نتیجہ, *مکمل);
  CF_ڈیٹا_RESTORE(cf, save);
  واپس نتیجہ;
}

ساکن خالی cf_h2_destroy(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;

  (خالی)ڈیٹا;
  اگر(ctx) {
    cf_h2_ctx_free(ctx);
    cf->ctx = تہی;
  }
}

ساکن CURLcode cf_h2_shutdown(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;
  ساخت cf_call_ڈیٹا save;
  CURLcode نتیجہ;
  صحیح rv;

  اگر(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  CF_ڈیٹا_SAVE(save, cf, ڈیٹا);

  اگر(!ctx->بھیجا_goaway) {
    ctx->بھیجا_goaway = TRUE;
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, ctx->local_max_sid, 0,
      (ثابت uint8_t *)"shutdown", حجم("shutdown"));
    اگر(rv) {
      failf(ڈیٹا, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      نتیجہ = CURLE_SEND_ERROR;
      جاؤ برآمد;
    }
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  نتیجہ = CURLE_OK;
  اگر(Curl_ngh2_session_want_write(&ctx->ngh2) ||
     !Curl_bufq_is_خالی_خانے(&ctx->outbufq))
    نتیجہ = h2_پیش_رفت_egress(cf, ڈیٹا);
  اگر(!نتیجہ && Curl_ngh2_session_want_read(&ctx->ngh2))
    نتیجہ = h2_پیش_رفت_ingress(cf, ڈیٹا, 0);

  اگر(نتیجہ == CURLE_AGAIN)
    نتیجہ = CURLE_OK;

  *مکمل = (ctx->conn_closed ||
           (!نتیجہ && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2) &&
            Curl_bufq_is_خالی_خانے(&ctx->outbufq)));

برآمد:
  CF_ڈیٹا_RESTORE(cf, save);
  cf->shutdown = (نتیجہ || *مکمل);
  واپس نتیجہ;
}

ساکن CURLcode http2_ڈیٹا_pause(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 bool pause)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;
  ساخت h2_دھارا_ctx *دھارا = H2_دھارا_CTX(ctx, ڈیٹا);

  DEBUGASSERT(ڈیٹا);
  اگر(ctx && Curl_ngh2_valid(&ctx->ngh2) && دھارا) {
    CURLcode نتیجہ;

    دھارا->write_paused = pause;
    نتیجہ = cf_h2_update_local_win(cf, ڈیٹا, دھارا);
    اگر(نتیجہ)
      واپس نتیجہ;

    /* attempt to send the window update */
    (خالی)h2_پیش_رفت_egress(cf, ڈیٹا);

    اگر(!pause) {
      /* Unpausing a h2 transfer, requires it to be run again. The server
       * may send new DATA on us increasing the flow window, and it may
       * not. We may have already buffered and exhausted the new window
       * by operating on things in flight during the handling of other
       * transfers. */
      Curl_multi_mark_dirty(ڈیٹا);
    }
    CURL_TRC_CF(ڈیٹا, cf, "[%d] stream now %spaused", دھارا->id,
                pause ? "" : "un");
  }
  واپس CURLE_OK;
}

ساکن CURLcode cf_h2_cntrl(ساخت Curl_cfilter *cf,
                            ساخت Curl_easy *ڈیٹا,
                            صحیح event, صحیح arg1, خالی *arg2)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت cf_call_ڈیٹا save;

  (خالی)arg2;

  CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
  انتخاب(event) {
  حالت CF_CTRL_ڈیٹا_تیاری:
    توڑو;
  حالت CF_CTRL_ڈیٹا_PAUSE:
    نتیجہ = http2_ڈیٹا_pause(cf, ڈیٹا, (arg1 != 0));
    توڑو;
  حالت CF_CTRL_FLUSH:
    نتیجہ = cf_h2_flush(cf, ڈیٹا);
    توڑو;
  حالت CF_CTRL_ڈیٹا_مکمل:
    http2_ڈیٹا_مکمل(cf, ڈیٹا);
    توڑو;
  حالت CF_CTRL_CONN_INFO_UPDATE:
    اگر(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_نظر_آیا = 20;
      Curl_conn_سیٹ_multiplex(cf->conn);
    }
    توڑو;
  طےشدہ:
    توڑو;
  }
  CF_ڈیٹا_RESTORE(cf, save);
  واپس نتیجہ;
}

ساکن bool cf_h2_ڈیٹا_pending(ساخت Curl_cfilter *cf,
                               ثابت ساخت Curl_easy *ڈیٹا)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;

  اگر(ctx && !Curl_bufq_is_خالی_خانے(&ctx->inbufq))
    واپس TRUE;
  واپس cf->اگلا ? cf->اگلا->cft->has_ڈیٹا_pending(cf->اگلا, ڈیٹا) : FALSE;
}

ساکن bool cf_h2_is_alive(ساخت Curl_cfilter *cf,
                           ساخت Curl_easy *ڈیٹا,
                           bool *داخلہ_pending)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;
  bool alive;
  ساخت cf_call_ڈیٹا save;

  *داخلہ_pending = FALSE;
  CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
           http2_connisalive(cf, ڈیٹا, داخلہ_pending));
  CURL_TRC_CF(ڈیٹا, cf, "conn alive -> %d, input_pending=%d",
              alive, *داخلہ_pending);
  CF_ڈیٹا_RESTORE(cf, save);
  واپس alive;
}

ساکن CURLcode cf_h2_keep_alive(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا)
{
  CURLcode نتیجہ;
  ساخت cf_call_ڈیٹا save;

  CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
  نتیجہ = http2_send_ping(cf, ڈیٹا);
  CF_ڈیٹا_RESTORE(cf, save);
  واپس نتیجہ;
}

ساکن CURLcode cf_h2_query(ساخت Curl_cfilter *cf,
                            ساخت Curl_easy *ڈیٹا,
                            صحیح query, صحیح *pres1, خالی *pres2)
{
  ساخت cf_h2_ctx *ctx = cf->ctx;
  ساخت cf_call_ڈیٹا save;
  size_t effective_max;

  انتخاب(query) {
  حالت CF_QUERY_MAX_CONCURRENT:
    DEBUGASSERT(pres1);

    CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
    اگر(!Curl_ngh2_valid(&ctx->ngh2) ||
       !Curl_ngh2_session_check_request_allowed(&ctx->ngh2)) {
      /* the limit is what we have in use right now */
      effective_max = cf->conn->attached_xfers;
    }
    ورنہ {
      effective_max = ctx->max_concurrent_streams;
    }
    *pres1 = (effective_max > زیادہ_سے_زیادہ_عدد) ? زیادہ_سے_زیادہ_عدد : (صحیح)effective_max;
    CF_ڈیٹا_RESTORE(cf, save);
    واپس CURLE_OK;
  حالت CF_QUERY_دھارا_ERROR: {
    ساخت h2_دھارا_ctx *دھارا = H2_دھارا_CTX(ctx, ڈیٹا);
    *pres1 = دھارا ? (صحیح)دھارا->error : 0;
    واپس CURLE_OK;
  }
  حالت CF_QUERY_NEED_FLUSH: {
    ساخت h2_دھارا_ctx *دھارا = H2_دھارا_CTX(ctx, ڈیٹا);
    اگر(!Curl_bufq_is_خالی_خانے(&ctx->outbufq) ||
       (دھارا && !Curl_bufq_is_خالی_خانے(&دھارا->sendbuf))) {
      *pres1 = TRUE;
      واپس CURLE_OK;
    }
    توڑو;
  }
  حالت CF_QUERY_HTTP_نسخہ:
    *pres1 = 20;
    واپس CURLE_OK;
  طےشدہ:
    توڑو;
  }
  واپس cf->اگلا ?
    cf->اگلا->cft->query(cf->اگلا, ڈیٹا, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساخت Curl_cftype Curl_cft_nghttp2 = {
  "HTTP/2",
  CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  CURL_LOG_LVL_NONE,
  cf_h2_destroy,
  cf_h2_connect,
  cf_h2_shutdown,
  cf_h2_adjust_pollset,
  cf_h2_ڈیٹا_pending,
  cf_h2_send,
  cf_h2_recv,
  cf_h2_cntrl,
  cf_h2_is_alive,
  cf_h2_keep_alive,
  cf_h2_query,
};

ساکن CURLcode http2_cfilter_جوڑو(ساخت Curl_cfilter **pcf,
                                  ساخت Curl_easy *ڈیٹا,
                                  ساخت connectdata *conn,
                                  int8_t sockindex,
                                  bool بذریعہ_h1_upgrade)
{
  ساخت Curl_cfilter *cf = تہی;
  ساخت cf_h2_ctx *ctx;
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;

  DEBUGASSERT(ڈیٹا->conn);
  ctx = curlx_calloc(1, حجم(*ctx));
  اگر(!ctx)
    جاؤ برآمد;
  cf_h2_ctx_init(ctx, بذریعہ_h1_upgrade);

  نتیجہ = Curl_cf_create(&cf, &Curl_cft_nghttp2, ctx);
  اگر(نتیجہ)
    جاؤ برآمد;

  ctx = تہی;
  Curl_conn_cf_جوڑو(ڈیٹا, conn, sockindex, cf);

برآمد:
  اگر(نتیجہ)
    cf_h2_ctx_free(ctx);
  *pcf = نتیجہ ? تہی : cf;
  واپس نتیجہ;
}

ساکن CURLcode http2_cfilter_داخل_کرو_after(ساخت Curl_cfilter *cf,
                                           ساخت Curl_easy *ڈیٹا,
                                           bool بذریعہ_h1_upgrade)
{
  ساخت Curl_cfilter *cf_h2 = تہی;
  ساخت cf_h2_ctx *ctx;
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;

  (خالی)ڈیٹا;
  ctx = curlx_calloc(1, حجم(*ctx));
  اگر(!ctx)
    جاؤ برآمد;
  cf_h2_ctx_init(ctx, بذریعہ_h1_upgrade);

  نتیجہ = Curl_cf_create(&cf_h2, &Curl_cft_nghttp2, ctx);
  اگر(نتیجہ)
    جاؤ برآمد;

  ctx = تہی;
  Curl_conn_cf_داخل_کرو_after(cf, cf_h2);

برآمد:
  اگر(نتیجہ)
    cf_h2_ctx_free(ctx);
  واپس نتیجہ;
}

bool Curl_http2_may_switch(ساخت Curl_easy *ڈیٹا)
{
  اگر(Curl_conn_http_نسخہ(ڈیٹا, ڈیٹا->conn) < 20 &&
     (ڈیٹا->state.http_neg.wanted & CURL_HTTP_V2x) &&
     ڈیٹا->state.http_neg.h2_پیشگی_knowledge) {
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
    اگر(ڈیٹا->conn->بٹ.origin_is_proxy) {
      infof(ڈیٹا, "Ignoring HTTP/2 prior knowledge due to proxy");
      واپس FALSE;
    }
#اگر_ختم
    واپس TRUE;
  }
  واپس FALSE;
}

CURLcode Curl_http2_switch(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_cfilter *cf;
  CURLcode نتیجہ;

  DEBUGASSERT(Curl_conn_http_نسخہ(ڈیٹا, ڈیٹا->conn) < 20);

  نتیجہ = http2_cfilter_جوڑو(&cf, ڈیٹا, ڈیٹا->conn, FIRSTSOCKET, FALSE);
  اگر(نتیجہ)
    واپس نتیجہ;
  CURL_TRC_CF(ڈیٹا, cf, "switching connection to HTTP/2");

  اگر(cf->اگلا) {
    bool مکمل;
    واپس Curl_conn_cf_connect(cf, ڈیٹا, &مکمل);
  }
  واپس CURLE_OK;
}

CURLcode Curl_http2_switch_at(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_cfilter *cf_h2;
  CURLcode نتیجہ;

  DEBUGASSERT(Curl_conn_http_نسخہ(ڈیٹا, ڈیٹا->conn) < 20);

  نتیجہ = http2_cfilter_داخل_کرو_after(cf, ڈیٹا, FALSE);
  اگر(نتیجہ)
    واپس نتیجہ;

  cf_h2 = cf->اگلا;

  اگر(cf_h2->اگلا) {
    bool مکمل;
    واپس Curl_conn_cf_connect(cf_h2, ڈیٹا, &مکمل);
  }
  واپس CURLE_OK;
}

CURLcode Curl_http2_upgrade(ساخت Curl_easy *ڈیٹا,
                            ساخت connectdata *conn, int8_t sockindex,
                            ثابت حرف *mem, size_t nread)
{
  ساخت Curl_cfilter *cf;
  ساخت cf_h2_ctx *ctx;
  CURLcode نتیجہ;

  DEBUGASSERT(Curl_conn_http_نسخہ(ڈیٹا, conn) < 20);

  نتیجہ = http2_cfilter_جوڑو(&cf, ڈیٹا, conn, sockindex, TRUE);
  اگر(نتیجہ)
    واپس نتیجہ;
  CURL_TRC_CF(ڈیٹا, cf, "upgrading connection to HTTP/2");

  DEBUGASSERT(cf->cft == &Curl_cft_nghttp2);
  ctx = cf->ctx;

  ڈیٹا->req.httpversion_بھیجا = 20; /* it is an h2 request now */
  ڈیٹا->req.سرخی = TRUE; /* we expect the real response to come in h2 */
  ڈیٹا->req.headerline = 0; /* restart the header line counter */

  اگر(nread > 0) {
    /* Remaining data from the protocol switch reply is already using
     * the switched protocol, ie. HTTP/2. We add that to the network
     * inbufq. */
    size_t copied;

    نتیجہ = Curl_bufq_write(&ctx->inbufq,
                             (ثابت غیرعلامتی حرف *)mem, nread, &copied);
    اگر(نتیجہ) {
      failf(ڈیٹا, "error on copying HTTP Upgrade response: %d", (صحیح)نتیجہ);
      واپس CURLE_RECV_ERROR;
    }
    اگر(copied < nread) {
      failf(ڈیٹا, "connection buffer size could not take all data "
            "from HTTP Upgrade response header: copied=%zu, datalen=%zu",
            copied, nread);
      واپس CURLE_HTTP2;
    }
    infof(ڈیٹا, "Copied HTTP/2 data in stream buffer to connection buffer"
          " after upgrade: len=%zu", nread);
  }

  اگر(cf->اگلا) {
    bool مکمل;
    نتیجہ = Curl_conn_cf_connect(cf, ڈیٹا, &مکمل);
    اگر(!نتیجہ)
      cf->cft->cntrl(cf, ڈیٹا, CF_CTRL_CONN_INFO_UPDATE, 0, تہی);
  }
  واپس نتیجہ;
}

/* Only call this function for a transfer that already got an HTTP/2
   CURLE_HTTP2_STREAM error! */
bool Curl_h2_http_1_1_error(ساخت Curl_easy *ڈیٹا)
{
  اگر(Curl_conn_http_نسخہ(ڈیٹا, ڈیٹا->conn) == 20) {
    صحیح err = Curl_conn_get_دھارا_error(ڈیٹا, ڈیٹا->conn, FIRSTSOCKET);
    واپس err == NGHTTP2_HTTP_1_1_REQUIRED;
  }
  واپس FALSE;
}

خالی *Curl_nghttp2_malloc(size_t سائز, خالی *user_ڈیٹا)
{
  (خالی)user_ڈیٹا;
  واپس Curl_cmalloc(سائز);
}

خالی Curl_nghttp2_free(خالی *ptr, خالی *user_ڈیٹا)
{
  (خالی)user_ڈیٹا;
  Curl_cfree(ptr);
}

خالی *Curl_nghttp2_calloc(size_t nmemb, size_t سائز, خالی *user_ڈیٹا)
{
  (خالی)user_ڈیٹا;
  واپس Curl_ccalloc(nmemb, سائز);
}

خالی *Curl_nghttp2_realloc(خالی *ptr, size_t سائز, خالی *user_ڈیٹا)
{
  (خالی)user_ڈیٹا;
  واپس Curl_crealloc(ptr, سائز);
}

#ورنہ /* CURL_DISABLE_HTTP || !USE_NGHTTP2 */

/* Satisfy external references even if http2 is not compiled in. */

حرف *curl_pushheader_bynum(ساخت curl_pushheaders *h, size_t عدد)
{
  (خالی)h;
  (خالی)عدد;
  واپس تہی;
}

حرف *curl_pushheader_byname(ساخت curl_pushheaders *h, ثابت حرف *name)
{
  (خالی)h;
  (خالی)name;
  واپس تہی;
}

#اگر_ختم /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
