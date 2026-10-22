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

#إذا NGHTTP2_نسخه_عدد < 0x010f00
#تېروتنه "nghttp2 1.15.0 or greater required"
#د_که_پای

#تعریف_کړه NGHTTP2_HAS_سېټ_LOCAL_کړکۍ_اندازه 1

/* buffer dimensioning:
 * use 16K as chunk size, as that fits H2 DATA frames well */
#تعریف_کړه H2_CHUNK_اندازه           (16 * 1024)
/* connection window size */
#تعریف_کړه H2_CONN_کړکۍ_اندازه     (10 * 1024 * 1024)
/* on receiving from TLS, we prep for holding a full stream window */
#تعریف_کړه H2_NW_RECV_CHUNKS       (H2_CONN_کړکۍ_اندازه / H2_CHUNK_اندازه)
/* on send into TLS, we want to accumulate small frames */
#تعریف_کړه H2_NW_SEND_CHUNKS       1
/* this is how much we want "in flight" for a stream, unthrottled */
#تعریف_کړه H2_بهیر_کړکۍ_اندازه_MAX   (10 * 1024 * 1024)
/* this is how much we want "in flight" for a stream, initially, IFF
 * nghttp2 allows us to tweak the local window size. */
#إذا NGHTTP2_HAS_سېټ_LOCAL_کړکۍ_اندازه
#تعریف_کړه H2_بهیر_کړکۍ_اندازه_INITIAL  (64 * 1024)
#وإلا
#تعریف_کړه H2_بهیر_کړکۍ_اندازه_INITIAL H2_بهیر_کړکۍ_اندازه_MAX
#د_که_پای
/* keep smaller stream upload buffer (default h2 window size) to have
 * our progress bars and "upload done" reporting closer to reality */
#تعریف_کړه H2_بهیر_SEND_CHUNKS   ((64 * 1024) / H2_CHUNK_اندازه)
/* spare chunks we keep for a full window */
#تعریف_کړه H2_بهیر_POOL_SPARES   (H2_CONN_کړکۍ_اندازه / H2_CHUNK_اندازه)

/* We need to accommodate the max number of streams with their window sizes on
 * the overall connection. Streams might become PAUSED which will block their
 * received QUOTA in the connection window. If we run out of space, the server
 * is blocked from sending us any data. See #10988 for an issue with this. */
#تعریف_کړه HTTP2_HUGE_کړکۍ_اندازه (100 * H2_بهیر_کړکۍ_اندازه_MAX)

#تعریف_کړه H2_SETTINGS_IV_LEN 3
#تعریف_کړه H2_BINSETTINGS_LEN 80

بنية cf_h2_ctx {
  بنية curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  بنية cf_call_معلومات call_معلومات;

  بنية bufq inbufq;           /* network input */
  بنية bufq outbufq;          /* network output */
  بنية bufc_pool بهیر_bufcp; /* spares for stream buffers */
  بنية dynbuf scratch;        /* scratch buffer for temp use */

  بنية u32_ptrset streams; /* hash of `data->mid` to `h2_stream_ctx` */
  size_t drain_ټول; /* sum of all stream's UrlState drain */
  uint32_t initial_win_اندازه; /* current initial window size (settings) */
  uint32_t max_concurrent_streams;
  uint32_t goaway_error;        /* goaway error code from server */
  int32_t remote_max_sid;       /* max id processed by server */
  int32_t local_max_sid;        /* max id processed by us */
  بټ(initialized);
  بټ(له_لارې_h1_upgrade);
  بټ(conn_closed);
  بټ(rcvd_goaway);
  بټ(لېږل_شوی_goaway);
  بټ(enable_ټېل);
  بټ(nw_بهر_blocked);
};

/* How to access `call_data` from a cf_h2 filter */
#تعریف_لغوه CF_CTX_CALL_معلومات
#تعریف_کړه CF_CTX_CALL_معلومات(cf) ((بنية cf_h2_ctx *)(cf)->ctx)->call_معلومات

/**
 * All about the H2 internals of a stream
 */
بنية h2_بهیر_ctx {
  بنية bufq sendbuf; /* request buffer */
  بنية h1_req_parser h1; /* parsing the request */
  بنية dynhds resp_trailers; /* response trailer fields */
  size_t resp_hds_len; /* amount of response header bytes in recvbuf */
  curl_off_t xfer_id;  /* transfer `id` owning this stream */
  curl_off_t nrcvd_معلومات;  /* number of DATA bytes received */

  حرف **ټېل_سرلیکونه;       /* allocated array */
  size_t ټېل_سرلیکونه_کارول_شوی;  /* number of entries filled in */
  size_t ټېل_سرلیکونه_alloc; /* number of entries allocated */

  int32_t id; /* HTTP/2 protocol identifier for stream */
  uint32_t mid; /* transfer `mid` owning this stream */
  uint32_t error; /* stream error code */
  CURLcode xfer_پایله; /* Result of writing out response */
  int32_t local_کړکۍ_اندازه; /* the local recv window size */
  int32_t حالت_کوډ; /* HTTP response status code */
  بټ(resp_hds_complete); /* we have a complete, final response */
  بټ(closed); /* TRUE on stream close */
  بټ(reset);  /* TRUE on stream reset */
  بټ(reset_by_server);  /* TRUE on stream reset by server */
  بټ(close_handled); /* TRUE if stream closure is handled by libcurl */
  بټ(bodystarted);
  بټ(body_eos);    /* the complete body has been added to `sendbuf` and
                     * is being/has been processed from there. */
  بټ(write_paused);  /* stream write is paused */
};

ساكن فراغ free_ټېل_سرلیکونه(بنية h2_بهیر_ctx *بهیر)
{
  size_t i;
  لكل(i = 0; i < بهیر->ټېل_سرلیکونه_کارول_شوی; i++)
    curlx_free(بهیر->ټېل_سرلیکونه[i]);
  curlx_safefree(بهیر->ټېل_سرلیکونه);
  بهیر->ټېل_سرلیکونه_کارول_شوی = 0;
}

ساكن فراغ h2_بهیر_ctx_free(بنية h2_بهیر_ctx *بهیر)
{
  Curl_bufq_free(&بهیر->sendbuf);
  Curl_h1_req_parse_free(&بهیر->h1);
  Curl_dynhds_free(&بهیر->resp_trailers);
  free_ټېل_سرلیکونه(بهیر);
  curlx_free(بهیر);
}

ساكن فراغ h2_بهیر_hash_free(uint32_t id, فراغ *بهیر)
{
  (فراغ)id;
  DEBUGASSERT(بهیر);
  h2_بهیر_ctx_free((بنية h2_بهیر_ctx *)بهیر);
}

ساكن فراغ cf_h2_ctx_init(بنية cf_h2_ctx *ctx, bool له_لارې_h1_upgrade)
{
  Curl_bufcp_init(&ctx->بهیر_bufcp, H2_CHUNK_اندازه, H2_بهیر_POOL_SPARES);
  Curl_bufq_initp(&ctx->inbufq, &ctx->بهیر_bufcp, H2_NW_RECV_CHUNKS, 0);
  Curl_bufq_initp(&ctx->outbufq, &ctx->بهیر_bufcp, H2_NW_SEND_CHUNKS, 0);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_سرلیک);
  Curl_u32_ptrset_init(&ctx->streams, h2_بهیر_hash_free);
  ctx->remote_max_sid = INT32_MAX;
  ctx->له_لارې_h1_upgrade = له_لارې_h1_upgrade;
  ctx->initialized = TRUE;
}

ساكن فراغ cf_h2_ctx_free(بنية cf_h2_ctx *ctx)
{
  إذا(ctx && ctx->initialized) {
    Curl_ngh2_del(&ctx->ngh2);
    Curl_bufq_free(&ctx->inbufq);
    Curl_bufq_free(&ctx->outbufq);
    Curl_bufcp_free(&ctx->بهیر_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    memset(ctx, 0, حجم(*ctx));
  }
  curlx_free(ctx);
}

ساكن uint32_t cf_h2_initial_win_اندازه(بنية Curl_easy *معلومات)
{
#إذا NGHTTP2_HAS_سېټ_LOCAL_کړکۍ_اندازه
  /* If the transfer has a rate-limit lower than the default initial
   * stream window size, use that. It needs to be at least 8k or servers
   * may be unhappy. */
  curl_off_t rps = Curl_rlimit_فی_ګام(&معلومات->پرمختګ.dl.rlimit);
  إذا((rps > 0) && (rps < H2_بهیر_کړکۍ_اندازه_INITIAL))
    أرجع CURLMAX((uint32_t)rps, 8192);
#د_که_پای
  أرجع H2_بهیر_کړکۍ_اندازه_INITIAL;
}

ساكن size_t populate_settings(nghttp2_settings_ثبت *iv,
                                بنية Curl_easy *معلومات,
                                بنية cf_h2_ctx *ctx)
{
  iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
  iv[0].ارزښت = Curl_multi_max_concurrent_streams(معلومات->multi);

  iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_کړکۍ_اندازه;
  iv[1].ارزښت = cf_h2_initial_win_اندازه(معلومات);
  إذا(ctx)
    ctx->initial_win_اندازه = iv[1].ارزښت;
  iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_ټېل;
  iv[2].ارزښت = !!معلومات->multi->ټېل_cb;

  أرجع 3;
}

ساكن ssize_t populate_binsettings(uint8_t *binsettings,
                                    بنية Curl_easy *معلومات)
{
  nghttp2_settings_ثبت iv[H2_SETTINGS_IV_LEN];
  size_t ivlen;

  ivlen = populate_settings(iv, معلومات, عدم);
  /* this returns number of bytes it wrote or a negative number on error. */
  أرجع nghttp2_pack_settings_payload(binsettings, H2_BINSETTINGS_LEN,
                                       iv, ivlen);
}

ساكن CURLcode cf_h2_update_settings(بنية cf_h2_ctx *ctx,
                                      uint32_t initial_win_اندازه)
{
  nghttp2_settings_ثبت ثبت;
  ثبت.settings_id = NGHTTP2_SETTINGS_INITIAL_کړکۍ_اندازه;
  ثبت.ارزښت = initial_win_اندازه;
  إذا(Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, &ثبت, 1))
    أرجع CURLE_SEND_ERROR;
  ctx->initial_win_اندازه = initial_win_اندازه;
  أرجع CURLE_OK;
}

#تعریف_کړه H2_بهیر_CTX(ctx, معلومات)                                         \
  ((بنية h2_بهیر_ctx *)(                                             \
    (معلومات) ? Curl_u32_ptrset_get(&(ctx)->streams, (معلومات)->mid) : عدم))

ساكن بنية h2_بهیر_ctx *h2_بهیر_ctx_create(بنية cf_h2_ctx *ctx,
                                                  بنية Curl_easy *معلومات)
{
  بنية h2_بهیر_ctx *بهیر;

  بهیر = curlx_calloc(1, حجم(*بهیر));
  إذا(!بهیر)
    أرجع عدم;

  بهیر->id = -1;
  بهیر->xfer_id = معلومات->id;
  بهیر->mid = معلومات->mid;
  Curl_bufq_initp(&بهیر->sendbuf, &ctx->بهیر_bufcp,
                  H2_بهیر_SEND_CHUNKS, BUFQ_OPT_NONE);
  Curl_h1_req_parse_init(&بهیر->h1, H1_PARSE_DEFAULT_MAX_کرښه_LEN);
  Curl_dynhds_init(&بهیر->resp_trailers,
                   MAX_HTTP_RESP_سرلیک_شمېر, DYN_HTTP_REQUEST);
  بهیر->bodystarted = FALSE;
  بهیر->حالت_کوډ = -1;
  بهیر->closed = FALSE;
  بهیر->close_handled = FALSE;
  بهیر->error = NGHTTP2_NO_ERROR;
  بهیر->local_کړکۍ_اندازه = H2_بهیر_کړکۍ_اندازه_INITIAL;
  بهیر->nrcvd_معلومات = 0;
  أرجع بهیر;
}

ساكن بنية Curl_easy *cf_h2_get_بهیر_xfer(بنية Curl_cfilter *cf,
                                               int32_t بهیر_id,
                                               بنية h2_بهیر_ctx **pstream)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_بهیر_ctx *بهیر =
    Curl_ngh2_session_get_بهیر_user_معلومات(&ctx->ngh2, بهیر_id);
  إذا(pstream)
    *pstream = بهیر;
  إذا(بهیر && (بهیر->mid != UINT32_MAX)) {
    بنية Curl_easy *معلومات, *call_معلومات = CF_معلومات_CURRENT(cf);
    إذا(!call_معلومات || !call_معلومات->multi) {
      DEBUGASSERT(0);
      أرجع عدم;
    }
    معلومات = Curl_multi_get_easy(call_معلومات->multi, بهیر->mid);
    إذا(معلومات && (معلومات->id == بهیر->xfer_id))
      أرجع معلومات;
  }
  أرجع عدم;
}

#که_تعریف_شوی NGHTTP2_HAS_سېټ_LOCAL_کړکۍ_اندازه
ساكن int32_t cf_h2_get_desired_local_win(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *معلومات)
{
  curl_off_t avail = Curl_rlimit_avail(&معلومات->پرمختګ.dl.rlimit, عدم);

  (فراغ)cf;
  إذا(avail < CURL_OFF_T_MAX) { /* limit in place */
    إذا(avail <= 0)
      أرجع 0;
    وإلا إذا(avail < H2_بهیر_کړکۍ_اندازه_MAX)
      أرجع (int32_t)avail;
  }
  أرجع H2_بهیر_کړکۍ_اندازه_MAX;
}

ساكن CURLcode cf_h2_update_local_win(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *معلومات,
                                       بنية h2_بهیر_ctx *بهیر)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  int32_t dwsize;
  صحيح rv;

  dwsize = (بهیر->write_paused || بهیر->xfer_پایله) ?
           0 : cf_h2_get_desired_local_win(cf, معلومات);
  إذا(dwsize != بهیر->local_کړکۍ_اندازه) {
    int32_t wsize = Curl_ngh2_session_get_بهیر_effective_local_کړکۍ_اندازه(
      &ctx->ngh2, بهیر->id);
    إذا(dwsize > wsize) {
      rv = Curl_ngh2_session_سېټ_local_کړکۍ_اندازه(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, بهیر->id, dwsize);
      إذا(rv) {
        failf(معلومات, "[%d] nghttp2 set_local_window_size(%d) failed: "
              "%s(%d)", بهیر->id, dwsize, nghttp2_strerror(rv), rv);
        أرجع CURLE_HTTP2;
      }
      rv = Curl_ngh2_submit_کړکۍ_update(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, بهیر->id, dwsize - wsize);
      إذا(rv) {
        failf(معلومات, "[%d] nghttp2_submit_window_update() failed: "
              "%s(%d)", بهیر->id, nghttp2_strerror(rv), rv);
        أرجع CURLE_HTTP2;
      }
      بهیر->local_کړکۍ_اندازه = dwsize;
      CURL_TRC_CF(معلومات, cf, "[%d] local window update by %d",
                  بهیر->id, dwsize - wsize);
    }
    وإلا {
      rv = Curl_ngh2_session_سېټ_local_کړکۍ_اندازه(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, بهیر->id, dwsize);
      إذا(rv) {
        failf(معلومات, "[%d] nghttp2_session_set_local_window_size() failed: "
              "%s(%d)", بهیر->id, nghttp2_strerror(rv), rv);
        أرجع CURLE_HTTP2;
      }
      بهیر->local_کړکۍ_اندازه = dwsize;
      CURL_TRC_CF(معلومات, cf, "[%d] local window size now %d",
                  بهیر->id, dwsize);
    }
  }
  أرجع CURLE_OK;
}

#وإلا /* NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

ساكن CURLcode cf_h2_update_local_win(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *معلومات,
                                       بنية h2_بهیر_ctx *بهیر)
{
  (فراغ)cf;
  (فراغ)معلومات;
  (فراغ)بهیر;
  أرجع CURLE_OK;
}
#د_که_پای /* !NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

ساكن CURLcode http2_معلومات_تیاری(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 بنية h2_بهیر_ctx **pstream)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_بهیر_ctx *بهیر;

  (فراغ)cf;
  DEBUGASSERT(معلومات);

  إذا(!معلومات)
    أرجع CURLE_BAD_FUNCTION_دلیل;

  بهیر = H2_بهیر_CTX(ctx, معلومات);
  إذا(بهیر) {
    *pstream = بهیر;
    أرجع CURLE_OK;
  }

  بهیر = h2_بهیر_ctx_create(ctx, معلومات);
  إذا(!بهیر)
    أرجع CURLE_بهر_د_حافظه;

  إذا(Curl_u32_ptrset_سېټ(&ctx->streams, معلومات->mid, بهیر)) {
    h2_بهیر_ctx_free(بهیر);
    أرجع CURLE_بهر_د_حافظه;
  }

  *pstream = بهیر;
  أرجع CURLE_OK;
}

ساكن CURLcode nw_بهر_flush(بنية Curl_cfilter *cf,
                             بنية Curl_easy *معلومات)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode پایله;

  إذا(Curl_bufq_is_تشه(&ctx->outbufq))
    أرجع CURLE_OK;

  پایله = Curl_cf_send_bufq(cf->بل, معلومات, &ctx->outbufq, عدم, 0,
                             &nwritten);
  إذا(پایله) {
    إذا(پایله == CURLE_AGAIN) {
      CURL_TRC_CF(معلومات, cf, "flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_بهر_blocked = 1;
    }
    أرجع پایله;
  }
  أرجع Curl_bufq_is_تشه(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

ساكن فراغ http2_معلومات_بشپړ(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_بهیر_ctx *بهیر = H2_بهیر_CTX(ctx, معلومات);

  DEBUGASSERT(ctx);
  إذا(!بهیر || !ctx->initialized)
    أرجع;

  إذا(Curl_ngh2_valid(&ctx->ngh2)) {
    bool flush_egress = FALSE;
    /* returns error if stream not known, which is fine here */
    (فراغ)Curl_ngh2_session_سېټ_بهیر_user_معلومات(&ctx->ngh2, بهیر->id, عدم);

    إذا(!بهیر->closed && بهیر->id > 0) {
      /* RST_STREAM */
      CURL_TRC_CF(معلومات, cf, "[%d] premature DATA_DONE, RST stream",
                  بهیر->id);
      بهیر->closed = TRUE;
      بهیر->reset = TRUE;
      Curl_ngh2_submit_rst_بهیر(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                  بهیر->id, NGHTTP2_بهیر_CLOSED);
      flush_egress = TRUE;
    }

    إذا(flush_egress) {
      (فراغ)Curl_ngh2_session_send(&ctx->ngh2);
      (فراغ)nw_بهر_flush(cf, معلومات);
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
  nghttp2_option_سېټ_no_auto_کړکۍ_update(o, 1);
#إذا NGHTTP2_نسخه_عدد >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_سېټ_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#د_که_پای
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  أرجع rc;
}

/*
 * Returns nonzero if current HTTP/2 session should be closed.
 */
ساكن صحيح should_close_session(بنية cf_h2_ctx *ctx)
{
  أرجع ctx->drain_ټول == 0 && !Curl_ngh2_session_want_read(&ctx->ngh2) &&
    !Curl_ngh2_session_want_write(&ctx->ngh2);
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
ساكن CURLcode h2_process_pending_ننوت(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *معلومات)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  ثابت بینښه حرف *buf;
  size_t blen, nread;
  ssize_t rv;

  طالما(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {
    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ثابت uint8_t *)buf, blen);
    إذا(!curlx_sztouz(rv, &nread)) {
      failf(معلومات, "nghttp2 recv error %zd: %s", rv, nghttp2_strerror((صحيح)rv));
      أرجع CURLE_RECV_ERROR;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    إذا(Curl_bufq_is_تشه(&ctx->inbufq)) {
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
                              bool *ننوت_pending)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *ننوت_pending = FALSE;
  إذا(!cf->بل || !cf->بل->cft->is_alive(cf->بل, معلومات, ننوت_pending))
    أرجع FALSE;

  إذا(*ننوت_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode پایله;
    size_t nread;

    *ننوت_pending = FALSE;
    پایله = Curl_cf_recv_bufq(cf->بل, معلومات, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(معلومات, cf, "connisalive, recv pending input -> %d, %zu",
                (صحيح)پایله, nread);
    إذا(!پایله) {
      پایله = h2_process_pending_ننوت(cf, معلومات);
      إذا(پایله)
        /* immediate error, considered dead */
        alive = FALSE;
      وإلا {
        alive = !should_close_session(ctx);
      }
    }
    وإلا إذا(پایله != CURLE_AGAIN) {
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
  nghttp2_info *h2 = nghttp2_نسخه(0);
  (فراغ)curl_msnprintf(p, len, "nghttp2/%s", h2->نسخه_str);
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
  CURLcode پایله = CURLE_OK;

  (فراغ)h2;
  (فراغ)flags;
  DEBUGASSERT(معلومات);

  إذا(!cf->connected)
    پایله = Curl_bufq_write(&ctx->outbufq, buf, blen, &nwritten);
  وإلا
    پایله = Curl_cf_send_bufq(cf->بل, معلومات, &ctx->outbufq, buf, blen,
                               &nwritten);

  إذا(پایله) {
    إذا(پایله == CURLE_AGAIN) {
      ctx->nw_بهر_blocked = 1;
      أرجع NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(معلومات, "Failed sending HTTP2 data");
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  إذا(!nwritten) {
    ctx->nw_بهر_blocked = 1;
    أرجع NGHTTP2_ERR_WOULDBLOCK;
  }
  أرجع (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

/* We pass a pointer to this struct in the push callback, but the contents of
   the struct are hidden from the user. */
بنية curl_pushheaders {
  بنية Curl_easy *معلومات;
  بنية h2_بهیر_ctx *بهیر;
  ثابت nghttp2_ټېل_promise *چوکاټ;
};

/*
 * push header access function. Only to be used from within the push callback
 */
حرف *curl_pushheader_bynum(بنية curl_pushheaders *h, size_t عدد)
{
  /* Verify that we got a good easy handle in the push header struct, mostly to
     detect rubbish input fast(er). */
  إذا(!h || !GOOD_EASY_HANDLE(h->معلومات))
    أرجع عدم;
  وإلا {
    إذا(h->بهیر && عدد < h->بهیر->ټېل_سرلیکونه_کارول_شوی)
      أرجع h->بهیر->ټېل_سرلیکونه[عدد];
  }
  أرجع عدم;
}

/*
 * push header access function. Only to be used from within the push callback
 */
حرف *curl_pushheader_byname(بنية curl_pushheaders *h, ثابت حرف *name)
{
  بنية h2_بهیر_ctx *بهیر;
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

  بهیر = h->بهیر;
  إذا(!بهیر)
    أرجع عدم;

  len = مزي_اوږدوالی(name);
  لكل(i = 0; i < بهیر->ټېل_سرلیکونه_کارول_شوی; i++) {
    إذا(!مزي_پرتله(name, بهیر->ټېل_سرلیکونه[i], len)) {
      /* sub-match, make sure that it is followed by a colon */
      إذا(بهیر->ټېل_سرلیکونه[i][len] != ':')
        استمر;
      أرجع &بهیر->ټېل_سرلیکونه[i][len + 1];
    }
  }
  أرجع عدم;
}

ساكن بنية Curl_easy *h2_duphandle(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات)
{
  بنية Curl_easy *دوهم = curl_easy_init();
  إذا(دوهم) {
    بنية h2_بهیر_ctx *دوهم_بهیر;
    http2_معلومات_تیاری(cf, دوهم, &دوهم_بهیر);
    دوهم->state.وزن = معلومات->state.وزن;
    إذا(معلومات->share)
      (فراغ)Curl_share_easy_link(دوهم, معلومات->share);
    /* copy a range of options from the parent handle */
    دوهم->سېټ.fwrite_func = معلومات->سېټ.fwrite_func;
    دوهم->سېټ.بهر = معلومات->سېټ.بهر;
    دوهم->سېټ.fwrite_سرلیک = معلومات->سېټ.fwrite_سرلیک;
    دوهم->سېټ.writeheader = معلومات->سېټ.writeheader;
    دوهم->سېټ.fdebug = معلومات->سېټ.fdebug;
    دوهم->سېټ.debugdata = معلومات->سېټ.debugdata;
    دوهم->سېټ.fxferinfo = معلومات->سېټ.fxferinfo;
    دوهم->سېټ.fprogress = معلومات->سېټ.fprogress;
    دوهم->سېټ.پرمختګ_client = معلومات->سېټ.پرمختګ_client;
    دوهم->پرمختګ.hide = معلومات->پرمختګ.hide;
    دوهم->پرمختګ.callback = معلومات->پرمختګ.callback;
  }
  أرجع دوهم;
}

ساكن صحيح سېټ_transfer_url(بنية Curl_easy *newhandle,
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
    uc = curl_url_سېټ(u, CURLUPART_SCHEME, v, 0);
    إذا(uc) {
      rc = 1;
      اذهب fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_AUTHORITY);
  إذا(v) {
    uc = Curl_url_سېټ_authority(u, v);
    إذا(uc) {
      rc = 2;
      اذهب fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_لار);
  إذا(v) {
    uc = curl_url_سېټ(u, CURLUPART_لار, v, 0);
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
  Curl_bufref_سېټ(&newhandle->state.url, url, 0, curl_free);
  أرجع 0;
}

ساكن فراغ discard_newhandle(بنية Curl_cfilter *cf,
                              بنية Curl_easy *newhandle)
{
  http2_معلومات_بشپړ(cf, newhandle);
  (فراغ)Curl_close(&newhandle);
}

ساكن صحيح ټېل_promise(بنية Curl_cfilter *cf,
                        بنية Curl_easy *معلومات,
                        ثابت nghttp2_ټېل_promise *چوکاټ)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  صحيح rv; /* one of the CURL_PUSH_* defines */

  CURL_TRC_CF(معلومات, cf, "[%d] PUSH_PROMISE received",
              چوکاټ->promised_بهیر_id);
  إذا(معلومات->multi->ټېل_cb) {
    بنية h2_بهیر_ctx *بهیر;
    بنية h2_بهیر_ctx *newstream;
    بنية curl_pushheaders heads;
    CURLMcode mresult;
    CURLcode پایله;
    /* clone the parent */
    بنية Curl_easy *newhandle = h2_duphandle(cf, معلومات);
    إذا(!newhandle) {
      infof(معلومات, "failed to duplicate handle");
      rv = CURL_ټېل_DENY; /* FAIL HARD */
      اذهب fail;
    }

    بهیر = H2_بهیر_CTX(ctx, معلومات);
    إذا(!بهیر) {
      failf(معلومات, "Internal NULL stream");
      discard_newhandle(cf, newhandle);
      rv = CURL_ټېل_DENY;
      اذهب fail;
    }

    heads.معلومات = معلومات;
    heads.بهیر = بهیر;
    heads.چوکاټ = چوکاټ;

    rv = سېټ_transfer_url(newhandle, &heads, معلومات);
    إذا(rv) {
      CURL_TRC_CF(معلومات, cf, "[%d] PUSH_PROMISE, failed to set URL -> %d",
                  چوکاټ->promised_بهیر_id, rv);
      discard_newhandle(cf, newhandle);
      rv = CURL_ټېل_DENY;
      اذهب fail;
    }

    {
      بنية Curl_mapi_guard guard;
      CURL_CBAPI_پیل(&guard, معلومات, multi_ټېل_cb);
      rv = معلومات->multi->ټېل_cb(معلومات, newhandle,
                                بهیر->ټېل_سرلیکونه_کارول_شوی, &heads,
                                معلومات->multi->ټېل_userp);
      CURL_CBAPI_پای_ټکی(&guard);
    }

    /* free the headers again */
    free_ټېل_سرلیکونه(بهیر);

    إذا(rv) {
      DEBUGASSERT((rv > CURL_ټېل_OK) && (rv <= CURL_ټېل_ERROROUT));
      /* denied, kill off the new handle again */
      CURL_TRC_CF(معلومات, cf, "[%d] PUSH_PROMISE, denied by application -> %d",
                  چوکاټ->promised_بهیر_id, rv);
      discard_newhandle(cf, newhandle);
      اذهب fail;
    }

    /* approved, add to the multi handle for processing. This
     * assigns newhandle->mid. For the new `mid` we assign the
     * h2_stream instance and remember the stream_id already known. */
    mresult = Curl_multi_جمع_کړه_perform(معلومات->multi, newhandle, cf->conn);
    إذا(mresult) {
      infof(معلومات, "failed to add handle to multi");
      discard_newhandle(cf, newhandle);
      rv = CURL_ټېل_DENY;
      اذهب fail;
    }

    پایله = http2_معلومات_تیاری(cf, newhandle, &newstream);
    إذا(پایله) {
      failf(معلومات, "error setting up stream: %d", (صحيح)پایله);
      discard_newhandle(cf, newhandle);
      rv = CURL_ټېل_DENY;
      اذهب fail;
    }

    DEBUGASSERT(newstream);
    newstream->id = چوکاټ->promised_بهیر_id;
    newhandle->req.maxdownload = -1;
    newhandle->req.اندازه = -1;

    CURL_TRC_CF(معلومات, cf, "promise easy handle added to multi, mid=%u",
                newhandle->mid);
    rv = Curl_ngh2_session_سېټ_بهیر_user_معلومات(
      &ctx->ngh2, newstream->id, newstream);
    إذا(rv) {
      infof(معلومات, "failed to set user_data for stream %d",
            newstream->id);
      DEBUGASSERT(0);
      discard_newhandle(cf, newhandle);
      rv = CURL_ټېل_DENY;
      اذهب fail;
    }

    /* success, remember max stream id processed */
    إذا(newstream->id > ctx->local_max_sid)
      ctx->local_max_sid = newstream->id;
  }
  وإلا {
    CURL_TRC_CF(معلومات, cf, "Got PUSH_PROMISE, ignore it");
    rv = CURL_ټېل_DENY;
  }
fail:
  أرجع rv;
}

ساكن فراغ h2_xfer_write_resp_hd(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  بنية h2_بهیر_ctx *بهیر,
                                  ثابت حرف *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  إذا(!بهیر->xfer_پایله) {
    بهیر->xfer_پایله = Curl_xfer_write_resp_hd(معلومات, buf, blen, eos);
    إذا(!بهیر->xfer_پایله && !eos)
      بهیر->xfer_پایله = cf_h2_update_local_win(cf, معلومات, بهیر);
    إذا(بهیر->xfer_پایله)
      CURL_TRC_CF(معلومات, cf, "[%d] error %d writing %zu bytes of headers",
                  بهیر->id, (صحيح)بهیر->xfer_پایله, blen);
  }
}

ساكن فراغ h2_xfer_write_resp(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات,
                               بنية h2_بهیر_ctx *بهیر,
                               ثابت حرف *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  إذا(!بهیر->xfer_پایله)
    بهیر->xfer_پایله = Curl_xfer_write_resp(معلومات, buf, blen, eos);
  /* If the transfer write is errored, we do not want any more data */
  إذا(بهیر->xfer_پایله) {
    بنية cf_h2_ctx *ctx = cf->ctx;
    CURL_TRC_CF(معلومات, cf, "[%d] error %d writing %zu bytes of data, "
                "RST-ing stream",
                بهیر->id, (صحيح)بهیر->xfer_پایله, blen);
    Curl_ngh2_submit_rst_بهیر(
      &ctx->ngh2, 0, بهیر->id, (uint32_t)NGHTTP2_ERR_CALLBACK_FAILURE);
  }
  وإلا إذا(!بهیر->write_paused && Curl_xfer_write_is_paused(معلومات)) {
    CURL_TRC_CF(معلومات, cf, "[%d] stream output paused", بهیر->id);
    بهیر->write_paused = TRUE;
  }
  وإلا إذا(بهیر->write_paused && !Curl_xfer_write_is_paused(معلومات)) {
    CURL_TRC_CF(معلومات, cf, "[%d] stream output unpaused", بهیر->id);
    بهیر->write_paused = FALSE;
  }

  إذا(!بهیر->xfer_پایله && !eos)
    بهیر->xfer_پایله = cf_h2_update_local_win(cf, معلومات, بهیر);
}

ساكن CURLcode on_بهیر_چوکاټ(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                بنية h2_بهیر_ctx *بهیر,
                                ثابت nghttp2_چوکاټ *چوکاټ)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  int32_t بهیر_id = چوکاټ->hd.بهیر_id;
  صحيح rv;

  إذا(!بهیر) {
    CURL_TRC_CF(معلومات, cf, "[%d] No stream_ctx set", بهیر_id);
    أرجع CURLE_FAILED_INIT;
  }

  بدل(چوکاټ->hd.type) {
  حالة NGHTTP2_معلومات:
    CURL_TRC_CF(معلومات, cf, "[%d] DATA, window=%d/%d",
                بهیر_id,
                Curl_ngh2_session_get_بهیر_effective_recv_معلومات_اوږدتیا(
                  &ctx->ngh2, بهیر->id),
                Curl_ngh2_session_get_بهیر_effective_local_کړکۍ_اندازه(
                  &ctx->ngh2, بهیر->id));
    /* If !body started on this stream, then receiving DATA is illegal. */
    إذا(!بهیر->bodystarted) {
      إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_بهیر(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                       بهیر_id, NGHTTP2_PROTOCOL_ERROR))) {
        أرجع CURLE_RECV_ERROR;
      }
    }
    اكسر;
  حالة NGHTTP2_سرلیکونه:
    إذا(بهیر->bodystarted) {
      /* Only valid HEADERS after body started is trailer HEADERS. We
         buffer them in on_header callback. */
      اكسر;
    }

    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    إذا(بهیر->حالت_کوډ == -1)
      أرجع CURLE_RECV_ERROR;

    /* Only final status code signals the end of header */
    إذا(بهیر->حالت_کوډ / 100 != 1) {
      بهیر->resp_hds_complete = TRUE;
      بهیر->bodystarted = TRUE;
    }
    وإلا
      بهیر->حالت_کوډ = -1;

    h2_xfer_write_resp_hd(cf, معلومات, بهیر, STRCONST("\r\n"),
                          (bool)بهیر->closed);

    Curl_multi_mark_dirty(معلومات);
    اكسر;
  حالة NGHTTP2_ټېل_PROMISE:
    rv = ټېل_promise(cf, معلومات, &چوکاټ->ټېل_promise);
    إذا(rv) { /* deny! */
      DEBUGASSERT((rv > CURL_ټېل_OK) && (rv <= CURL_ټېل_ERROROUT));
      إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_بهیر(&ctx->ngh2, NGHTTP2_FLAG_NONE,
             چوکاټ->ټېل_promise.promised_بهیر_id, NGHTTP2_CANCEL)))
        أرجع CURLE_SEND_ERROR;
      وإلا إذا(rv == CURL_ټېل_ERROROUT) {
        /* fail the parent transfer, not the whole session */
        CURL_TRC_CF(معلومات, cf, "[%d] fail in PUSH_PROMISE received",
                    بهیر_id);
        إذا(!بهیر->xfer_پایله)
          بهیر->xfer_پایله = CURLE_RECV_ERROR;
        Curl_multi_mark_dirty(معلومات);
      }
    }
    اكسر;
  حالة NGHTTP2_RST_بهیر:
    إذا(چوکاټ->rst_بهیر.error_کوډ)
      بهیر->reset_by_server = TRUE;
    Curl_multi_mark_dirty(معلومات);
    اكسر;
  حالة NGHTTP2_کړکۍ_UPDATE:
    إذا(CURL_REQ_WANT_SEND(معلومات) && Curl_bufq_is_تشه(&بهیر->sendbuf)) {
      /* need more data, force processing of transfer */
      Curl_multi_mark_dirty(معلومات);
    }
    وإلا إذا(!Curl_bufq_is_تشه(&بهیر->sendbuf)) {
      /* resume the potentially suspended stream */
      إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_session_resume_معلومات(&ctx->ngh2, بهیر->id)))
        أرجع CURLE_SEND_ERROR;
    }
    اكسر;
  افتراضي:
    اكسر;
  }

  إذا(چوکاټ->hd.flags & NGHTTP2_FLAG_پای_ټکی_بهیر) {
    إذا(!بهیر->closed && !بهیر->body_eos &&
       ((بهیر->حالت_کوډ >= 400) || (بهیر->حالت_کوډ < 200))) {
      /* The server did not give us a positive response and we are not
       * done uploading the request body. We need to stop doing that and
       * also inform the server that we aborted our side. */
      CURL_TRC_CF(معلومات, cf, "[%d] EOS frame with unfinished upload and "
                  "HTTP status %d, abort upload by RST",
                  بهیر_id, بهیر->حالت_کوډ);
      Curl_ngh2_submit_rst_بهیر(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, بهیر->id, NGHTTP2_بهیر_CLOSED);
      بهیر->closed = TRUE;
    }
    Curl_multi_mark_dirty(معلومات);
  }
  أرجع CURLE_OK;
}

#که_تعریف_شوی CURLVERBOSE
صحيح Curl_nghttp2_fr_print(ثابت nghttp2_چوکاټ *چوکاټ, حرف *بفر,
                          size_t blen)
{
  بدل(چوکاټ->hd.type) {
  حالة NGHTTP2_معلومات: {
    أرجع curl_msnprintf(بفر, blen,
                          "FRAME[DATA, len=%d, eos=%d, padlen=%d]",
                          (صحيح)چوکاټ->hd.اوږدتیا,
                          !!(چوکاټ->hd.flags & NGHTTP2_FLAG_پای_ټکی_بهیر),
                          (صحيح)چوکاټ->معلومات.padlen);
  }
  حالة NGHTTP2_سرلیکونه: {
    أرجع curl_msnprintf(بفر, blen,
                          "FRAME[HEADERS, len=%d, hend=%d, eos=%d]",
                          (صحيح)چوکاټ->hd.اوږدتیا,
                          !!(چوکاټ->hd.flags & NGHTTP2_FLAG_پای_ټکی_سرلیکونه),
                          !!(چوکاټ->hd.flags & NGHTTP2_FLAG_پای_ټکی_بهیر));
  }
  حالة NGHTTP2_لومړیتوب: {
    أرجع curl_msnprintf(بفر, blen,
                          "FRAME[PRIORITY, len=%d, flags=%d]",
                          (صحيح)چوکاټ->hd.اوږدتیا, چوکاټ->hd.flags);
  }
  حالة NGHTTP2_RST_بهیر: {
    أرجع curl_msnprintf(بفر, blen,
                          "FRAME[RST_STREAM, len=%d, flags=%d, error=%u]",
                          (صحيح)چوکاټ->hd.اوږدتیا, چوکاټ->hd.flags,
                          چوکاټ->rst_بهیر.error_کوډ);
  }
  حالة NGHTTP2_SETTINGS: {
    إذا(چوکاټ->hd.flags & NGHTTP2_FLAG_ACK) {
      أرجع curl_msnprintf(بفر, blen, "FRAME[SETTINGS, ack=1]");
    }
    أرجع curl_msnprintf(بفر, blen,
                          "FRAME[SETTINGS, len=%d]", (صحيح)چوکاټ->hd.اوږدتیا);
  }
  حالة NGHTTP2_ټېل_PROMISE:
    أرجع curl_msnprintf(بفر, blen,
                          "FRAME[PUSH_PROMISE, len=%d, hend=%d]",
                          (صحيح)چوکاټ->hd.اوږدتیا,
                          !!(چوکاټ->hd.flags & NGHTTP2_FLAG_پای_ټکی_سرلیکونه));
  حالة NGHTTP2_PING:
    أرجع curl_msnprintf(بفر, blen,
                          "FRAME[PING, len=%d, ack=%d]",
                          (صحيح)چوکاټ->hd.اوږدتیا,
                          چوکاټ->hd.flags & NGHTTP2_FLAG_ACK);
  حالة NGHTTP2_GOAWAY: {
    حرف scratch[128];
    size_t s_len = CURL_ARRAYSIZE(scratch);
    size_t len = (چوکاټ->goaway.opaque_معلومات_len < s_len) ?
      چوکاټ->goaway.opaque_معلومات_len : s_len - 1;
    إذا(len)
      حافظه_نقل(scratch, چوکاټ->goaway.opaque_معلومات, len);
    scratch[len] = '\0';
    أرجع curl_msnprintf(بفر, blen,
                          "FRAME[GOAWAY, error=%u, reason='%s', "
                          "last_stream=%d]", چوکاټ->goaway.error_کوډ,
                          scratch, چوکاټ->goaway.وروستی_بهیر_id);
  }
  حالة NGHTTP2_کړکۍ_UPDATE: {
    أرجع curl_msnprintf(بفر, blen,
                          "FRAME[WINDOW_UPDATE, incr=%d]",
                          چوکاټ->کړکۍ_update.کړکۍ_اندازه_increment);
  }
  افتراضي:
    أرجع curl_msnprintf(بفر, blen, "FRAME[%d, len=%d, flags=%d]",
                          چوکاټ->hd.type, (صحيح)چوکاټ->hd.اوږدتیا,
                          چوکاټ->hd.flags);
  }
}

ساكن صحيح on_چوکاټ_send(nghttp2_session *session, ثابت nghttp2_چوکاټ *چوکاټ,
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
    len = Curl_nghttp2_fr_print(چوکاټ, بفر, حجم(بفر) - 1);
    بفر[len] = 0;
    CURL_TRC_CF(معلومات, cf, "[%d] -> %s", چوکاټ->hd.بهیر_id, بفر);
  }
  إذا((چوکاټ->hd.type == NGHTTP2_GOAWAY) && !ctx->لېږل_شوی_goaway) {
    /* A GOAWAY not initiated by us, but by nghttp2 itself on detecting
     * a protocol error on the connection */
    failf(معلومات, "nghttp2 shuts down connection with error %u: %s",
          چوکاټ->goaway.error_کوډ,
          nghttp2_http2_strerror(چوکاټ->goaway.error_کوډ));
  }
  أرجع 0;
}
#د_که_پای /* CURLVERBOSE */

ساكن صحيح on_چوکاټ_recv(nghttp2_session *session, ثابت nghttp2_چوکاټ *چوکاټ,
                         فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf), *معلومات_s;
  بنية h2_بهیر_ctx *بهیر;
  int32_t بهیر_id = چوکاټ->hd.بهیر_id;

  (فراغ)session;
  DEBUGASSERT(معلومات);
#که_تعریف_شوی CURLVERBOSE
  إذا(Curl_trc_cf_is_verbose(cf, معلومات)) {
    حرف بفر[256];
    صحيح len;
    len = Curl_nghttp2_fr_print(چوکاټ, بفر, حجم(بفر) - 1);
    بفر[len] = 0;
    CURL_TRC_CF(معلومات, cf, "[%d] <- %s", چوکاټ->hd.بهیر_id, بفر);
  }
#د_که_پای /* CURLVERBOSE */

  إذا(!بهیر_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(معلومات);
    بدل(چوکاټ->hd.type) {
    حالة NGHTTP2_SETTINGS: {
      إذا(!(چوکاټ->hd.flags & NGHTTP2_FLAG_ACK)) {
        uint32_t max_conn = ctx->max_concurrent_streams;
        ctx->max_concurrent_streams =
          Curl_ngh2_session_get_remote_settings(
            &ctx->ngh2, NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS);
        ctx->enable_ټېل = Curl_ngh2_session_get_remote_settings(
          &ctx->ngh2, NGHTTP2_SETTINGS_ENABLE_ټېل) != 0;
        CURL_TRC_CF(معلومات, cf, "[0] MAX_CONCURRENT_STREAMS: %u",
                    ctx->max_concurrent_streams);
        CURL_TRC_CF(معلومات, cf, "[0] ENABLE_PUSH: %s",
                    ctx->enable_ټېل ? "TRUE" : "false");
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
      ctx->goaway_error = چوکاټ->goaway.error_کوډ;
      ctx->remote_max_sid = چوکاټ->goaway.وروستی_بهیر_id;
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

  معلومات_s = cf_h2_get_بهیر_xfer(cf, بهیر_id, &بهیر);
  إذا(!معلومات_s) {
    CURL_TRC_CF(معلومات, cf, "[%d] No Curl_easy associated", بهیر_id);
    أرجع 0;
  }

  أرجع on_بهیر_چوکاټ(cf, معلومات_s, بهیر, چوکاټ) ?
         NGHTTP2_ERR_CALLBACK_FAILURE : 0;
}

ساكن صحيح cf_h2_on_invalid_چوکاټ_recv(nghttp2_session *session,
                                       ثابت nghttp2_چوکاټ *چوکاټ,
                                       صحيح ngerr, فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية Curl_easy *معلومات;
  بنية h2_بهیر_ctx *بهیر;
  int32_t بهیر_id = چوکاټ->hd.بهیر_id;

  (فراغ)session;
  معلومات = cf_h2_get_بهیر_xfer(cf, بهیر_id, &بهیر);
  إذا(معلومات) {
#که_تعریف_شوی CURLVERBOSE
    حرف بفر[256];
    صحيح len;
    len = Curl_nghttp2_fr_print(چوکاټ, بفر, حجم(بفر) - 1);
    بفر[len] = 0;
    failf(معلومات, "[HTTP2] [%d] received invalid frame: %s, error %d: %s",
          بهیر_id, بفر, ngerr, nghttp2_strerror(ngerr));
#د_که_پای /* CURLVERBOSE */
    إذا(بهیر) {
      Curl_ngh2_submit_rst_بهیر(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, بهیر->id, NGHTTP2_بهیر_CLOSED);
      بهیر->error = ngerr;
      بهیر->closed = TRUE;
      بهیر->reset = TRUE;
    }
  }
  /* Return no error, nghttp2 will RST/GOAWAY by itself when needed */
  أرجع 0;
}

ساكن صحيح on_معلومات_chunk_recv(nghttp2_session *session, uint8_t flags,
                              int32_t بهیر_id,
                              ثابت uint8_t *mem, size_t len, فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_بهیر_ctx *بهیر;
  بنية Curl_easy *معلومات_s;

  (فراغ)session;
  (فراغ)flags;

  DEBUGASSERT(بهیر_id); /* should never be a zero stream ID here */
  DEBUGASSERT(CF_معلومات_CURRENT(cf));

  /* get the stream from the hash based on Stream ID */
  معلومات_s = cf_h2_get_بهیر_xfer(cf, بهیر_id, &بهیر);
  إذا(!معلومات_s) {
    /* Receiving a Stream ID not in the hash should not happen - unless
       we have aborted a transfer artificially and there were more data
       in the pipeline. Silently ignore. */
    CURL_TRC_CF(CF_معلومات_CURRENT(cf), cf, "[%d] Data for unknown", بهیر_id);
    /* consumed explicitly as no one will read it */
    Curl_ngh2_session_consume(&ctx->ngh2, بهیر_id, len);
    أرجع 0;
  }

  إذا(!بهیر)
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;

  h2_xfer_write_resp(cf, معلومات_s, بهیر, (ثابت حرف *)mem, len, FALSE);

  Curl_ngh2_session_consume(&ctx->ngh2, بهیر_id, len);
  بهیر->nrcvd_معلومات += (curl_off_t)len;
  أرجع 0;
}

ساكن صحيح on_بهیر_close(nghttp2_session *session, int32_t بهیر_id,
                           uint32_t error_کوډ, فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية Curl_easy *معلومات_s, *call_معلومات = CF_معلومات_CURRENT(cf);
  بنية h2_بهیر_ctx *بهیر = عدم;
  صحيح rv;
  (فراغ)session;

  DEBUGASSERT(call_معلومات);
  /* stream id 0 is the connection, do not look there for streams. */
  معلومات_s = بهیر_id ? cf_h2_get_بهیر_xfer(cf, بهیر_id, &بهیر) : عدم;
  إذا(!معلومات_s) {
    CURL_TRC_CF(call_معلومات, cf,
                "[%d] on_stream_close, no easy set on stream", بهیر_id);
    أرجع 0;
  }
  إذا(!GOOD_EASY_HANDLE(معلومات_s)) {
    /* nghttp2 still has an easy registered for the stream which has
     * been freed be libcurl. This points to a code path that does not
     * trigger DONE or DETACH events as it must. */
    CURL_TRC_CF(call_معلومات, cf,
                "[%d] on_stream_close, not a GOOD easy on stream", بهیر_id);
    (فراغ)Curl_ngh2_session_سېټ_بهیر_user_معلومات(
      &ctx->ngh2, بهیر_id, عدم);
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  إذا(!بهیر) {
    CURL_TRC_CF(معلومات_s, cf,
                "[%d] on_stream_close, GOOD easy but no stream", بهیر_id);
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  بهیر->closed = TRUE;
  بهیر->error = error_کوډ;
  إذا(بهیر->error) {
    بهیر->reset = TRUE;
    إذا((بهیر->error == NGHTTP2_REFUSED_بهیر) &&
       ctx->rcvd_goaway && ctx->goaway_error)
    /* REFUSED_STREAM is used by nghttp2 on a GOAWAY from the
     * server where it indicated it will no longer process this stream.
     * If the GOAWAY carried an error code, take that as the error for
     * the stream. */
    بهیر->error = ctx->goaway_error;
  }

  إذا(بهیر->error)
    CURL_TRC_CF(معلومات_s, cf, "[%d] RESET: %s (err %u)",
              بهیر_id, nghttp2_http2_strerror(error_کوډ), error_کوډ);
  وإلا
    CURL_TRC_CF(معلومات_s, cf, "[%d] CLOSED", بهیر_id);
  Curl_multi_mark_dirty(معلومات_s);

  /* remove `stream_ctx` from the nghttp2 stream */
  rv = Curl_ngh2_session_سېټ_بهیر_user_معلومات(
    &ctx->ngh2, بهیر_id, عدم);
  إذا(rv) {
    infof(معلومات_s, "http/2: failed to clear user_data for stream %d",
          بهیر_id);
    DEBUGASSERT(0);
  }
  أرجع 0;
}

ساكن فراغ cf_h2_سرلیک_error(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات,
                               بنية h2_بهیر_ctx *بهیر,
                               CURLcode پایله)
{
  بنية cf_h2_ctx *ctx = cf->ctx;

  failf(معلومات, "Error receiving HTTP2 header: %d(%s)", (صحيح)پایله,
        curl_easy_strerror(پایله));
  إذا(بهیر) {
    Curl_ngh2_submit_rst_بهیر(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, بهیر->id, NGHTTP2_بهیر_CLOSED);
    بهیر->closed = TRUE;
    بهیر->reset = TRUE;
  }
}

/* frame->hd.type is either NGHTTP2_HEADERS or NGHTTP2_PUSH_PROMISE */
ساكن صحيح on_سرلیک(nghttp2_session *session, ثابت nghttp2_چوکاټ *چوکاټ,
                     ثابت uint8_t *name, size_t namelen,
                     ثابت uint8_t *ارزښت, size_t valuelen,
                     uint8_t flags,
                     فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_بهیر_ctx *بهیر;
  بنية Curl_easy *معلومات;
  int32_t بهیر_id = چوکاټ->hd.بهیر_id;
  CURLcode پایله;

  (فراغ)session;
  (فراغ)flags;

  DEBUGASSERT(بهیر_id); /* should never be a zero stream ID here */

  /* get the stream from the hash based on Stream ID */
  معلومات = cf_h2_get_بهیر_xfer(cf, بهیر_id, &بهیر);
  إذا(!GOOD_EASY_HANDLE(معلومات))
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;

  إذا(!بهیر) {
    failf(معلومات, "Internal NULL stream");
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  /* Store received PUSH_PROMISE headers to be used when the subsequent
     PUSH_PROMISE callback comes */
  إذا(چوکاټ->hd.type == NGHTTP2_ټېل_PROMISE) {
    حرف *h;

    إذا((namelen == CURL_CSTRLEN(HTTP_PSEUDO_AUTHORITY)) &&
       !مزي_پرتله(HTTP_PSEUDO_AUTHORITY, (ثابت حرف *)name, namelen)) {
      /* pseudo headers are lower case */
      صحيح rc = 0;
      حرف *check = curl_maprintf("%s:%d", معلومات->state.origin->hostname,
                                  معلومات->state.origin->پورټ);
      إذا(!check)
        /* no memory */
        أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
      إذا(!curl_strequal(check, (ثابت حرف *)ارزښت) &&
         ((معلومات->state.origin->پورټ != معلومات->state.origin->scheme->defport) ||
          !curl_strequal(معلومات->state.origin->hostname, (ثابت حرف *)ارزښت))) {
        /* This is push is not for the same authority that was asked for in
         * the URL. RFC 7540 section 8.2 says: "A client MUST treat a
         * PUSH_PROMISE for which the server is not authoritative as a stream
         * error of type PROTOCOL_ERROR."
         */
        (فراغ)Curl_ngh2_submit_rst_بهیر(
          &ctx->ngh2, NGHTTP2_FLAG_NONE, بهیر_id, NGHTTP2_PROTOCOL_ERROR);
        rc = NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      curlx_free(check);
      إذا(rc)
        أرجع rc;
    }

    إذا(!بهیر->ټېل_سرلیکونه) {
      بهیر->ټېل_سرلیکونه_alloc = 10;
      بهیر->ټېل_سرلیکونه = curlx_malloc(بهیر->ټېل_سرلیکونه_alloc *
                                          حجم(حرف *));
      إذا(!بهیر->ټېل_سرلیکونه)
        أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
      بهیر->ټېل_سرلیکونه_کارول_شوی = 0;
    }
    وإلا إذا(بهیر->ټېل_سرلیکونه_کارول_شوی ==
            بهیر->ټېل_سرلیکونه_alloc) {
      حرف **headp;
      إذا(بهیر->ټېل_سرلیکونه_alloc > 1000) {
        /* this is beyond crazy many headers, bail out */
        failf(معلومات, "Too many PUSH_PROMISE headers");
        free_ټېل_سرلیکونه(بهیر);
        أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      بهیر->ټېل_سرلیکونه_alloc *= 2;
      headp = curlx_realloc(بهیر->ټېل_سرلیکونه,
                            بهیر->ټېل_سرلیکونه_alloc * حجم(حرف *));
      إذا(!headp) {
        free_ټېل_سرلیکونه(بهیر);
        أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      بهیر->ټېل_سرلیکونه = headp;
    }
    h = curl_maprintf("%s:%s", name, ارزښت);
    إذا(!h) {
      free_ټېل_سرلیکونه(بهیر);
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    بهیر->ټېل_سرلیکونه[بهیر->ټېل_سرلیکونه_کارول_شوی++] = h;
    أرجع 0;
  }

  إذا(بهیر->bodystarted) {
    /* This is a trailer */
    CURL_TRC_CF(معلومات, cf, "[%d] trailer: %.*s: %.*s",
                بهیر->id, (صحيح)namelen, name, (صحيح)valuelen, ارزښت);
    پایله = Curl_dynhds_جمع_کړه(&بهیر->resp_trailers,
                             (ثابت حرف *)name, namelen,
                             (ثابت حرف *)ارزښت, valuelen);
    إذا(پایله) {
      cf_h2_سرلیک_error(cf, معلومات, بهیر, پایله);
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    }

    أرجع 0;
  }

  إذا(namelen == CURL_CSTRLEN(HTTP_PSEUDO_حالت) &&
     !memcmp(HTTP_PSEUDO_حالت, name, namelen)) {
    /* nghttp2 guarantees :status is received first and only once. */
    حرف بفر[32];
    size_t hlen;
    پایله = Curl_http_decode_حالت(&بهیر->حالت_کوډ,
                                     (ثابت حرف *)ارزښت, valuelen);
    إذا(پایله) {
      cf_h2_سرلیک_error(cf, معلومات, بهیر, پایله);
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    hlen = curl_msnprintf(بفر, حجم(بفر), HTTP_PSEUDO_حالت ":%d\r",
                          بهیر->حالت_کوډ);
    پایله = Curl_سرلیکونه_ټېل(معلومات, بفر, hlen, CURLH_PSEUDO);
    إذا(پایله) {
      cf_h2_سرلیک_error(cf, معلومات, بهیر, پایله);
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    curlx_dyn_reset(&ctx->scratch);
    پایله = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/2 "));
    إذا(!پایله)
      پایله = curlx_dyn_addn(&ctx->scratch, ارزښت, valuelen);
    إذا(!پایله)
      پایله = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    إذا(!پایله)
      h2_xfer_write_resp_hd(cf, معلومات, بهیر, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    إذا(پایله) {
      cf_h2_سرلیک_error(cf, معلومات, بهیر, پایله);
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    /* if we receive data for another handle, wake that up */
    إذا(CF_معلومات_CURRENT(cf) != معلومات)
      Curl_multi_mark_dirty(معلومات);

    CURL_TRC_CF(معلومات, cf, "[%d] status: HTTP/2 %03d",
                بهیر->id, بهیر->حالت_کوډ);
    أرجع 0;
  }

  /* nghttp2 guarantees that namelen > 0, and :status was already
     received, and this is not pseudo-header field . */
  /* convert to an HTTP1-style header */
  curlx_dyn_reset(&ctx->scratch);
  پایله = curlx_dyn_addn(&ctx->scratch, (ثابت حرف *)name, namelen);
  إذا(!پایله)
    پایله = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
  إذا(!پایله)
    پایله = curlx_dyn_addn(&ctx->scratch, (ثابت حرف *)ارزښت, valuelen);
  إذا(!پایله)
    پایله = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
  إذا(!پایله)
    h2_xfer_write_resp_hd(cf, معلومات, بهیر, curlx_dyn_ptr(&ctx->scratch),
                          curlx_dyn_len(&ctx->scratch), FALSE);
  إذا(پایله) {
    cf_h2_سرلیک_error(cf, معلومات, بهیر, پایله);
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  /* if we receive data for another handle, wake that up */
  إذا(CF_معلومات_CURRENT(cf) != معلومات)
    Curl_multi_mark_dirty(معلومات);

  CURL_TRC_CF(معلومات, cf, "[%d] header: %.*s: %.*s",
              بهیر->id, (صحيح)namelen, name, (صحيح)valuelen, ارزښت);

  أرجع 0; /* 0 is successful */
}

ساكن ssize_t req_body_read_callback(nghttp2_session *session,
                                      int32_t بهیر_id,
                                      uint8_t *buf, size_t اوږدتیا,
                                      uint32_t *معلومات_flags,
                                      nghttp2_معلومات_سرچینه *سرچینه,
                                      فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية Curl_easy *معلومات_s;
  بنية h2_بهیر_ctx *بهیر = عدم;
  CURLcode پایله;
  ssize_t nread;
  size_t n;
  (فراغ)سرچینه;

  (فراغ)session;
  إذا(!بهیر_id)
    أرجع NGHTTP2_ERR_INVALID_دلیل;

  /* get the stream from the hash based on Stream ID, stream ID zero is for
     connection-oriented stuff */
  معلومات_s = cf_h2_get_بهیر_xfer(cf, بهیر_id, &بهیر);
  إذا(!معلومات_s || !بهیر)
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;

  پایله = Curl_bufq_read(&بهیر->sendbuf, buf, اوږدتیا, &n);
  إذا(پایله) {
    إذا(پایله != CURLE_AGAIN)
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    nread = 0;
  }
  وإلا
    nread = (ssize_t)n;

  CURL_TRC_CF(معلومات_s, cf, "[%d] req_body_read(len=%zu) eos=%d -> %zd, %d",
              بهیر_id, اوږدتیا, بهیر->body_eos, nread, (صحيح)پایله);

  إذا(بهیر->body_eos && Curl_bufq_is_تشه(&بهیر->sendbuf)) {
    *معلومات_flags = NGHTTP2_معلومات_FLAG_EOF;
    أرجع nread;
  }
  أرجع (nread == 0) ? NGHTTP2_ERR_DEFERRED : nread;
}

#که_تعریف_شوی CURLVERBOSE
ساكن صحيح error_callback(nghttp2_session *session,
                          ثابت حرف *پیغام,
                          size_t len,
                          فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
  (فراغ)session;
  failf(معلومات, "%.*s", (صحيح)len, پیغام);
  أرجع 0;
}
#د_که_پای

/*
 * Append headers to ask for an HTTP1.1 to HTTP2 upgrade.
 */
CURLcode Curl_http2_request_upgrade(بنية dynbuf *req,
                                    بنية Curl_easy *معلومات)
{
  CURLcode پایله;
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

  پایله = curlx_base64url_encode(binsettings, binlen, &base64, &blen);
  إذا(پایله) {
    curlx_dyn_free(req);
    أرجع پایله;
  }

  معلومات->state.http_hd_upgrade = TRUE;
  معلومات->state.http_hd_h2_settings = TRUE;
  پایله = curlx_dyn_addf(req,
                          "Upgrade: %s\r\n"
                          "HTTP2-Settings: %s\r\n",
                          NGHTTP2_CLEARTEXT_PROTO_نسخه_ID, base64);
  curlx_free(base64);

  k->upgr101 = UPGR101_H2;
  معلومات->conn->بټونه.upgrade_دننه_پرمختګ = TRUE;

  أرجع پایله;
}

ساكن CURLcode http2_handle_بهیر_close(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *معلومات,
                                          بنية h2_بهیر_ctx *بهیر,
                                          size_t *pnlen)
{
  CURLcode پایله;

  *pnlen = 0;
  إذا(بهیر->reset) {
    إذا(بهیر->error == NGHTTP2_REFUSED_بهیر) {
      infof(معلومات, "HTTP/2 stream %d refused by server, try again on a new "
                  "connection", بهیر->id);
      connclose(cf->conn); /* do not use this anymore */
      معلومات->state.refused_بهیر = TRUE;
      أرجع CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    وإلا إذا(بهیر->resp_hds_complete && معلومات->req.no_body) {
      CURL_TRC_CF(معلومات, cf, "[%d] error after response headers, but we did "
                  "not want a body anyway, ignore: %s (err %u)",
                  بهیر->id, nghttp2_http2_strerror(بهیر->error),
                  بهیر->error);
      بهیر->close_handled = TRUE;
      أرجع CURLE_OK;
    }
    failf(معلومات, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          بهیر->id, بهیر->reset_by_server ? "server" : "curl",
          بهیر->error, nghttp2_http2_strerror(بهیر->error));
    أرجع بهیر->error ? CURLE_HTTP2_بهیر :
           (معلومات->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2);
  }
  وإلا إذا(!بهیر->bodystarted) {
    failf(معلومات, "HTTP/2 stream %d was closed cleanly, but before getting "
          "all response header fields, treated as error", بهیر->id);
    أرجع CURLE_HTTP2_بهیر;
  }

  إذا(Curl_dynhds_شمېر(&بهیر->resp_trailers)) {
    بنية dynhds_ثبت *e;
    بنية dynbuf dbuf;
    size_t i;

    پایله = CURLE_OK;
    curlx_dyn_init(&dbuf, DYN_TRAILERS);
    لكل(i = 0; i < Curl_dynhds_شمېر(&بهیر->resp_trailers); ++i) {
      e = Curl_dynhds_getn(&بهیر->resp_trailers, i);
      إذا(!e)
        اكسر;
      curlx_dyn_reset(&dbuf);
      پایله = curlx_dyn_addf(&dbuf, "%.*s: %.*s\x0d\x0a",
                              (صحيح)e->namelen, e->name,
                              (صحيح)e->valuelen, e->ارزښت);
      إذا(پایله)
        اكسر;
      Curl_debug(معلومات, CURLINFO_سرلیک_دننه, curlx_dyn_ptr(&dbuf),
                 curlx_dyn_len(&dbuf));
      پایله = Curl_client_write(معلومات,
                                 CLIENTWRITE_سرلیک | CLIENTWRITE_TRAILER,
                                 curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf));
      إذا(پایله)
        اكسر;
      پایله = Curl_bump_headersize(معلومات, curlx_dyn_len(&dbuf), FALSE);
      إذا(پایله)
        أرجع پایله;
    }
    curlx_dyn_free(&dbuf);
    إذا(پایله)
      اذهب بهر;
  }

  بهیر->close_handled = TRUE;
  پایله = CURLE_OK;

بهر:
  CURL_TRC_CF(معلومات, cf, "handle_stream_close -> %d, %zu", (صحيح)پایله, *pnlen);
  أرجع پایله;
}

ساكن صحيح sweight_wanted(ثابت بنية Curl_easy *معلومات)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  أرجع معلومات->سېټ.وزن ?
    معلومات->سېټ.وزن : NGHTTP2_DEFAULT_وزن;
}

ساكن صحيح sweight_دننه_effect(ثابت بنية Curl_easy *معلومات)
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
                        nghttp2_لومړیتوب_spec *pri_spec)
{
  صحيح prio = معلومات->سېټ.وزن;
  nghttp2_لومړیتوب_spec_init(pri_spec, 0, sweight_wanted(معلومات), FALSE);
  معلومات->state.وزن = prio;
}

/*
 * Check if there is been an update in the priority /
 * dependency settings and if so it submits a PRIORITY frame with the updated
 * info.
 * Flush any out data pending in the network buffer.
 */
ساكن CURLcode h2_پرمختګ_egress(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  صحيح rv = 0;

  إذا(sweight_wanted(معلومات) != sweight_دننه_effect(معلومات)) {
    بنية h2_بهیر_ctx *بهیر = H2_بهیر_CTX(ctx, معلومات);
    إذا(بهیر && بهیر->id > 0 &&
       (sweight_wanted(معلومات) != sweight_دننه_effect(معلومات))) {
      /* send new weight and/or dependency */
      nghttp2_لومړیتوب_spec pri_spec;

      h2_pri_spec(معلومات, &pri_spec);
      CURL_TRC_CF(معلومات, cf, "[%d] Queuing PRIORITY", بهیر->id);
      DEBUGASSERT(بهیر->id != -1);
      rv = Curl_ngh2_submit_لومړیتوب(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, بهیر->id, &pri_spec);
      إذا(rv)
        اذهب بهر;
    }
  }

  ctx->nw_بهر_blocked = 0;
  طالما(!rv && !ctx->nw_بهر_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

بهر:
  إذا(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(معلومات, cf, "nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    أرجع CURLE_SEND_ERROR;
  }
  /* Defer flushing during the connect phase so that the SETTINGS and
   * other initial frames are sent together with the first request.
   * Unless we are 'connect_only' where the request will never come. */
  إذا(!cf->connected && !cf->conn->بټونه.connect_only)
    أرجع CURLE_OK;
  أرجع nw_بهر_flush(cf, معلومات);
}

ساكن CURLcode بهیر_recv(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                            بنية h2_بهیر_ctx *بهیر,
                            حرف *buf, size_t len, size_t *pnread)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  CURLcode پایله = CURLE_AGAIN;

  (فراغ)buf;
  (فراغ)len;
  *pnread = 0;

  إذا(!بهیر->xfer_پایله)
    بهیر->xfer_پایله = cf_h2_update_local_win(cf, معلومات, بهیر);

  إذا(بهیر->xfer_پایله) {
    CURL_TRC_CF(معلومات, cf, "[%d] xfer write failed", بهیر->id);
    پایله = بهیر->xfer_پایله;
  }
  وإلا إذا(بهیر->closed) {
    CURL_TRC_CF(معلومات, cf, "[%d] returning CLOSE", بهیر->id);
    پایله = http2_handle_بهیر_close(cf, معلومات, بهیر, pnread);
  }
  وإلا إذا(بهیر->reset ||
          (ctx->conn_closed && Curl_bufq_is_تشه(&ctx->inbufq)) ||
          (ctx->rcvd_goaway && ctx->remote_max_sid < بهیر->id)) {
    CURL_TRC_CF(معلومات, cf, "[%d] returning ERR", بهیر->id);
    پایله = معلومات->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2;
  }

  إذا(پایله && (پایله != CURLE_AGAIN))
    CURL_TRC_CF(معلومات, cf, "[%d] stream_recv(len=%zu) -> %d, %zu",
                بهیر->id, len, (صحيح)پایله, *pnread);
  أرجع پایله;
}

ساكن CURLcode h2_پرمختګ_ingress(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    size_t معلومات_max_بایټونه)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  CURLcode پایله = CURLE_OK;
  size_t nread;

  إذا(should_close_session(ctx)) {
    CURL_TRC_CF(معلومات, cf, "[0] ingress: session is closed");
    أرجع CURLE_HTTP2;
  }

  /* Process network input buffer first */
  إذا(!Curl_bufq_is_تشه(&ctx->inbufq)) {
    CURL_TRC_CF(معلومات, cf, "Process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    پایله = h2_process_pending_ننوت(cf, معلومات);
    إذا(پایله)
      أرجع پایله;
  }

  إذا(!معلومات_max_بایټونه)
    معلومات_max_بایټونه = H2_CHUNK_اندازه;

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop due to connection close or us not processing
   * all network input */
  طالما(!ctx->conn_closed && Curl_bufq_is_تشه(&ctx->inbufq)) {
    بنية h2_بهیر_ctx *بهیر = H2_بهیر_CTX(ctx, معلومات);
    إذا(بهیر && (بهیر->closed || !معلومات_max_بایټونه)) {
      /* We would like to abort here and stop processing, so that the transfer
       * loop can handle the data/close here. This may leave data in
       * underlying buffers that will not be consumed. */
      إذا(!cf->بل || !cf->بل->cft->has_معلومات_pending(cf->بل, معلومات))
        Curl_multi_mark_dirty(معلومات);
      اكسر;
    }

    پایله = Curl_cf_recv_bufq(cf->بل, معلومات, &ctx->inbufq, 0, &nread);
    إذا(پایله) {
      إذا(پایله != CURLE_AGAIN) {
        failf(معلومات, "Failed receiving HTTP2 data: %d(%s)", (صحيح)پایله,
              curl_easy_strerror(پایله));
        أرجع پایله;
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
      معلومات_max_بایټونه = (معلومات_max_بایټونه > nread) ? (معلومات_max_بایټونه - nread) : 0;
    }

    پایله = h2_process_pending_ننوت(cf, معلومات);
    إذا(پایله)
      أرجع پایله;
    CURL_TRC_CF(معلومات, cf, "[0] ingress: nw-in buffered %zu",
                Curl_bufq_len(&ctx->inbufq));
  }

  إذا(ctx->conn_closed && Curl_bufq_is_تشه(&ctx->inbufq)) {
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
  بنية h2_بهیر_ctx *بهیر;
  CURLcode پایله, r2;
  بنية cf_call_معلومات save;

  إذا(!معلومات)
    أرجع CURLE_HTTP2;

  بهیر = H2_بهیر_CTX(ctx, معلومات);

  *pnread = 0;
  إذا(!بهیر) {
    /* Abnormal call sequence: either this transfer has never opened a stream
     * (unlikely) or the transfer has been done, cleaned up its resources, but
     * a read() is called anyway. It is not clear what the calling sequence
     * is for such a case. */
    failf(معلومات, "http/2 recv on a transfer never opened "
          "or already cleared, mid=%u", معلومات->mid);
    أرجع CURLE_HTTP2;
  }

  CF_معلومات_SAVE(save, cf, معلومات);

  پایله = بهیر_recv(cf, معلومات, بهیر, buf, len, pnread);
  إذا(پایله && (پایله != CURLE_AGAIN))
    اذهب بهر;

  إذا(پایله) {
    پایله = h2_پرمختګ_ingress(cf, معلومات, len);
    إذا(پایله)
      اذهب بهر;

    پایله = بهیر_recv(cf, معلومات, بهیر, buf, len, pnread);
  }

  إذا(*pnread > 0) {
    /* Now that we transferred this to the upper layer, we report
     * the actual amount of DATA consumed to the H2 session, so
     * that it adjusts stream flow control */
    Curl_ngh2_session_consume(&ctx->ngh2, بهیر->id, *pnread);
    إذا(بهیر->closed) {
      CURL_TRC_CF(معلومات, cf, "[%d] DRAIN closed stream", بهیر->id);
      Curl_multi_mark_dirty(معلومات);
    }
  }

بهر:
  r2 = h2_پرمختګ_egress(cf, معلومات);
  إذا(r2 == CURLE_AGAIN) {
    /* pending data to send, need to be called again. Ideally, we
     * monitor the socket for POLLOUT, but when not SENDING
     * any more, we force processing of the transfer. */
    إذا(!CURL_REQ_WANT_SEND(معلومات))
      Curl_multi_mark_dirty(معلومات);
  }
  وإلا إذا(r2 && (پایله == CURLE_AGAIN)) {
    /* Do not replace a previous error result NOR a successful result
     * with an egress error. Example: the server might close the
     * connection after sending the complete response with EOS
     * and everything. Further sending may fail, but that MUST
     * not change that we had a successful response. */
    پایله = r2;
  }

  CURL_TRC_CF(معلومات, cf, "[%d] cf_recv(len=%zu) -> %d, %zu, "
              "window=%d/%d, connection %d/%d",
              بهیر->id, len, (صحيح)پایله, *pnread,
              Curl_ngh2_session_get_بهیر_effective_recv_معلومات_اوږدتیا(
                &ctx->ngh2, بهیر->id),
              Curl_ngh2_session_get_بهیر_effective_local_کړکۍ_اندازه(
                &ctx->ngh2, بهیر->id),
              Curl_ngh2_session_get_local_کړکۍ_اندازه(&ctx->ngh2),
              HTTP2_HUGE_کړکۍ_اندازه);

  CF_معلومات_RESTORE(cf, save);
  أرجع پایله;
}

ساكن CURLcode cf_h2_body_send(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                بنية h2_بهیر_ctx *بهیر,
                                ثابت فراغ *buf, size_t blen, bool eos,
                                size_t *pnwritten)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  CURLcode پایله;

  *pnwritten = 0;
  إذا(بهیر->closed) {
    إذا(بهیر->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(معلومات, cf, "[%d] discarding data"
                  "on closed stream with response", بهیر->id);
      إذا(eos)
        بهیر->body_eos = TRUE;
      *pnwritten = blen;
      أرجع CURLE_OK;
    }
    /* Server closed before we got a response, this is an error */
    infof(معلومات, "stream %d closed", بهیر->id);
    أرجع CURLE_SEND_ERROR;
  }

  پایله = Curl_bufq_write(&بهیر->sendbuf, buf, blen, pnwritten);
  إذا(پایله)
    أرجع پایله;

  إذا(eos && (blen == *pnwritten))
    بهیر->body_eos = TRUE;

  إذا(eos || !Curl_bufq_is_تشه(&بهیر->sendbuf)) {
    /* resume the potentially suspended stream */
    إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_معلومات(&ctx->ngh2, بهیر->id)))
      أرجع CURLE_SEND_ERROR;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode h2_submit(بنية h2_بهیر_ctx **pstream,
                          بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                          ثابت uint8_t *buf, size_t len,
                          bool eos, size_t *pnwritten)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_بهیر_ctx *بهیر = عدم;
  بنية dynhds h2_سرلیکونه;
  nghttp2_nv *nva = عدم;
  ثابت فراغ *body = عدم;
  size_t nheader, bodylen;
  nghttp2_معلومات_provider معلومات_prd;
  int32_t بهیر_id;
  nghttp2_لومړیتوب_spec pri_spec;
  size_t nwritten;
  CURLcode پایله = CURLE_OK;
  uint32_t initial_win_اندازه;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_سرلیکونه, MAX_HTTP_RESP_سرلیک_شمېر, DYN_HTTP_REQUEST);

  پایله = http2_معلومات_تیاری(cf, معلومات, &بهیر);
  إذا(پایله)
    اذهب بهر;

  پایله = Curl_h1_req_parse_read(
    &بهیر->h1, buf, len, عدم,
    !معلومات->state.http_ignorecustom ?
    CURL_EASY_STR(معلومات, STRING_CUSTOMREQUEST) : عدم,
    0, &nwritten);
  إذا(پایله)
    اذهب بهر;
  *pnwritten = nwritten;
  إذا(!بهیر->h1.بشپړ) {
    /* need more data */
    اذهب بهر;
  }
  DEBUGASSERT(بهیر->h1.req);

  پایله = Curl_http_req_منزل_h2(&h2_سرلیکونه, بهیر->h1.req, معلومات);
  إذا(پایله)
    اذهب بهر;
  /* no longer needed */
  Curl_h1_req_parse_free(&بهیر->h1);

  nva = Curl_dynhds_منزل_nva(&h2_سرلیکونه, &nheader);
  إذا(!nva) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }

  h2_pri_spec(معلومات, &pri_spec);
  إذا(!Curl_ngh2_session_check_request_allowed(&ctx->ngh2))
    CURL_TRC_CF(معلومات, cf, "send request NOT allowed (via nghttp2)");

  /* Check the initial windows size of the transfer (rate-limits?) and
   * send an updated settings on changes from previous value. */
  initial_win_اندازه = cf_h2_initial_win_اندازه(معلومات);
  إذا(initial_win_اندازه != ctx->initial_win_اندازه) {
    پایله = cf_h2_update_settings(ctx, initial_win_اندازه);
    إذا(پایله)
      اذهب بهر;
  }

  بدل(معلومات->state.httpreq) {
  حالة HTTPREQ_POST:
  حالة HTTPREQ_POST_FORM:
  حالة HTTPREQ_POST_MIME:
  حالة HTTPREQ_PUT:
    معلومات_prd.read_callback = req_body_read_callback;
    معلومات_prd.سرچینه.ptr = عدم;
    بهیر_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, &معلومات_prd, بهیر);
    اكسر;
  افتراضي:
    بهیر_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, عدم, بهیر);
  }

  إذا(بهیر_id < 0) {
    CURL_TRC_CF(معلومات, cf, "send: nghttp2_submit_request error (%s)%d",
                nghttp2_strerror(بهیر_id), بهیر_id);
    پایله = CURLE_SEND_ERROR;
    اذهب بهر;
  }

#که_تعریف_شوی CURLVERBOSE
#تعریف_کړه MAX_ACC 60000  /* <64KB to account for some overhead */
  إذا(Curl_trc_is_verbose(معلومات)) {
    size_t acc = 0, i;

    infof(معلومات, "[HTTP/2] [%d] OPENED stream for %s",
          بهیر_id, Curl_bufref_ptr(&معلومات->state.url));
    لكل(i = 0; i < nheader; ++i) {
      acc += nva[i].namelen + nva[i].valuelen;

      infof(معلومات, "[HTTP/2] [%d] [%.*s: %.*s]", بهیر_id,
            (صحيح)nva[i].namelen, nva[i].name,
            (صحيح)nva[i].valuelen, nva[i].ارزښت);
    }

    إذا(acc > MAX_ACC) {
      infof(معلومات, "[HTTP/2] Warning: The cumulative length of all "
            "headers exceeds %d bytes and that could cause the "
            "stream to be rejected.", MAX_ACC);
    }
  }
#د_که_پای

  بهیر->id = بهیر_id;

  body = (ثابت حرف *)buf + *pnwritten;
  bodylen = len - *pnwritten;

  إذا(bodylen || eos) {
    size_t n;
    پایله = cf_h2_body_send(cf, معلومات, بهیر, body, bodylen, eos, &n);
    إذا(!پایله)
      *pnwritten += n;
    وإلا إذا(پایله == CURLE_AGAIN)
      پایله = CURLE_OK;
    وإلا {
      پایله = CURLE_SEND_ERROR;
    }
  }

بهر:
  CURL_TRC_CF(معلومات, cf, "[%d] submit -> %d, %zu",
              بهیر ? بهیر->id : -1, (صحيح)پایله, *pnwritten);
  curlx_safefree(nva);
  *pstream = بهیر;
  Curl_dynhds_free(&h2_سرلیکونه);
  أرجع پایله;
}

ساكن CURLcode cf_h2_send(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                           ثابت uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_بهیر_ctx *بهیر = H2_بهیر_CTX(ctx, معلومات);
  بنية cf_call_معلومات save;
  CURLcode پایله = CURLE_OK, r2;

  CF_معلومات_SAVE(save, cf, معلومات);
  *pnwritten = 0;

  إذا(!بهیر || بهیر->id == -1) {
    پایله = h2_submit(&بهیر, cf, معلومات, buf, len, eos, pnwritten);
    إذا(پایله)
      اذهب بهر;
    DEBUGASSERT(بهیر);
  }
  وإلا إذا(بهیر->body_eos) {
    /* We already wrote this, but CURLE_AGAIN-ed the call due to not
     * being able to flush stream->sendbuf. Make a 0-length write
     * to trigger flushing again.
     * If this works, we report to have written `len` bytes. */
    size_t n;
    DEBUGASSERT(eos);
    پایله = cf_h2_body_send(cf, معلومات, بهیر, buf, 0, eos, &n);
    CURL_TRC_CF(معلومات, cf, "[%d] cf_body_send last CHUNK -> %d, %zu, eos=%d",
                بهیر->id, (صحيح)پایله, n, eos);
    إذا(پایله)
      اذهب بهر;
    *pnwritten = len;
  }
  وإلا {
    پایله = cf_h2_body_send(cf, معلومات, بهیر, buf, len, eos, pnwritten);
    CURL_TRC_CF(معلومات, cf, "[%d] cf_body_send(len=%zu) -> %d, %zu, eos=%d",
                بهیر->id, len, (صحيح)پایله, *pnwritten, eos);
  }

  /* Call the nghttp2 send loop and flush to write ALL buffered data,
   * headers and/or request body completely out to the network */
  r2 = h2_پرمختګ_egress(cf, معلومات);

  /* if the stream has been closed in egress handling (nghttp2 does that
   * when it does not like the headers, for example */
  إذا(بهیر && بهیر->closed && !بهیر->resp_hds_complete) {
    infof(معلومات, "stream %d closed", بهیر->id);
    پایله = CURLE_SEND_ERROR;
    اذهب بهر;
  }
  وإلا إذا(r2 && (r2 != CURLE_AGAIN)) {
    پایله = r2;
    اذهب بهر;
  }

  إذا(should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    إذا(بهیر && بهیر->closed) {
      پایله = http2_handle_بهیر_close(cf, معلومات, بهیر, pnwritten);
    }
    وإلا {
      CURL_TRC_CF(معلومات, cf, "send: nothing to do in this session");
      پایله = CURLE_HTTP2;
    }
  }

بهر:
  إذا(بهیر) {
    CURL_TRC_CF(معلومات, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
                "eos=%d, h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                بهیر->id, len, (صحيح)پایله, *pnwritten,
                بهیر->body_eos,
                Curl_ngh2_session_get_بهیر_remote_کړکۍ_اندازه(
                  &ctx->ngh2, بهیر->id),
                Curl_ngh2_session_get_remote_کړکۍ_اندازه(&ctx->ngh2),
                Curl_bufq_len(&بهیر->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  وإلا {
    CURL_TRC_CF(معلومات, cf, "cf_send(len=%zu) -> %d, %zu, "
                "connection-window=%d, nw_send_buffer(%zu)",
                len, (صحيح)پایله, *pnwritten,
                Curl_ngh2_session_get_remote_کړکۍ_اندازه(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_معلومات_RESTORE(cf, save);
  أرجع پایله;
}

ساكن CURLcode cf_h2_flush(بنية Curl_cfilter *cf,
                            بنية Curl_easy *معلومات)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_بهیر_ctx *بهیر = H2_بهیر_CTX(ctx, معلومات);
  بنية cf_call_معلومات save;
  CURLcode پایله = CURLE_OK;

  CF_معلومات_SAVE(save, cf, معلومات);
  إذا(بهیر && !Curl_bufq_is_تشه(&بهیر->sendbuf)) {
    /* resume the potentially suspended stream */
    إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_معلومات(&ctx->ngh2, بهیر->id))) {
      پایله = CURLE_SEND_ERROR;
      اذهب بهر;
    }
  }

  پایله = h2_پرمختګ_egress(cf, معلومات);

بهر:
  إذا(بهیر) {
    CURL_TRC_CF(معلومات, cf, "[%d] flush -> %d, "
                "h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                بهیر->id, (صحيح)پایله,
                Curl_ngh2_session_get_بهیر_remote_کړکۍ_اندازه(
                  &ctx->ngh2, بهیر->id),
                Curl_ngh2_session_get_remote_کړکۍ_اندازه(&ctx->ngh2),
                Curl_bufq_len(&بهیر->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  وإلا {
    CURL_TRC_CF(معلومات, cf, "flush -> %d, "
                "connection-window=%d, nw_send_buffer(%zu)", (صحيح)پایله,
                Curl_ngh2_session_get_remote_کړکۍ_اندازه(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_معلومات_RESTORE(cf, save);
  أرجع پایله;
}

ساكن CURLcode cf_h2_adjust_pollset(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *معلومات,
                                     بنية easy_pollset *ps)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية cf_call_معلومات save;
  curl_socket_t sock;
  bool want_recv, want_send;
  CURLcode پایله = CURLE_OK;

  إذا(!Curl_ngh2_valid(&ctx->ngh2))
    أرجع CURLE_OK;

  sock = Curl_conn_cf_get_socket(cf, معلومات);
  Curl_pollset_check(معلومات, ps, sock, &want_recv, &want_send);
  إذا(want_recv || want_send) {
    بنية h2_بهیر_ctx *بهیر = H2_بهیر_CTX(ctx, معلومات);
    bool c_exhaust, s_exhaust;

    CF_معلومات_SAVE(save, cf, معلومات);
    c_exhaust = want_send &&
                !Curl_ngh2_session_get_remote_کړکۍ_اندازه(&ctx->ngh2);
    s_exhaust = want_send && بهیر && بهیر->id >= 0 &&
                !Curl_ngh2_session_get_بهیر_remote_کړکۍ_اندازه(
                  &ctx->ngh2, بهیر->id);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                (!c_exhaust && Curl_ngh2_session_want_write(&ctx->ngh2)) ||
                !Curl_bufq_is_تشه(&ctx->outbufq);

    پایله = Curl_pollset_سېټ(معلومات, ps, sock, want_recv, want_send);
    CF_معلومات_RESTORE(cf, save);
  }
  وإلا إذا(ctx->لېږل_شوی_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_معلومات_SAVE(save, cf, معلومات);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_تشه(&ctx->outbufq);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    پایله = Curl_pollset_سېټ(معلومات, ps, sock, want_recv, want_send);
    CF_معلومات_RESTORE(cf, save);
  }
  أرجع پایله;
}

ساكن CURLcode cf_h2_ctx_open(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_بهیر_ctx *بهیر;
  CURLcode پایله = CURLE_بهر_د_حافظه;
  صحيح rc;
  nghttp2_session_callbacks *cbs = عدم;

  DEBUGASSERT(!ctx->ngh2.s);
  DEBUGASSERT(!ctx->ngh2.fatal_rv);
  DEBUGASSERT(ctx->initialized);

  rc = nghttp2_session_callbacks_new(&cbs);
  إذا(rc) {
    failf(معلومات, "Could not initialize nghttp2 callbacks");
    اذهب بهر;
  }

  nghttp2_session_callbacks_سېټ_send_callback(cbs, send_callback);
  nghttp2_session_callbacks_سېټ_on_چوکاټ_recv_callback(cbs, on_چوکاټ_recv);
  nghttp2_session_callbacks_سېټ_on_invalid_چوکاټ_recv_callback(cbs,
    cf_h2_on_invalid_چوکاټ_recv);
#که_تعریف_شوی CURLVERBOSE
  nghttp2_session_callbacks_سېټ_on_چوکاټ_send_callback(cbs, on_چوکاټ_send);
#د_که_پای
  nghttp2_session_callbacks_سېټ_on_معلومات_chunk_recv_callback(
    cbs, on_معلومات_chunk_recv);
  nghttp2_session_callbacks_سېټ_on_بهیر_close_callback(cbs, on_بهیر_close);
  nghttp2_session_callbacks_سېټ_on_سرلیک_callback(cbs, on_سرلیک);
#که_تعریف_شوی CURLVERBOSE
  nghttp2_session_callbacks_سېټ_error_callback(cbs, error_callback);
#د_که_پای

  /* The nghttp2 session is not yet setup, do it */
  rc = h2_client_new(cf, cbs);
  إذا(rc) {
    failf(معلومات, "Could not initialize nghttp2");
    اذهب بهر;
  }
  ctx->max_concurrent_streams = معلومات->multi ?
    Curl_multi_max_concurrent_streams(معلومات->multi) :
    DEFAULT_MAX_CONCURRENT_STREAMS;

  إذا(ctx->له_لارې_h1_upgrade) {
    /* HTTP/1.1 Upgrade issued. H2 Settings have already been submitted
     * in the H1 request and we upgrade from there. This stream
     * is opened implicitly as #1. */
    uint8_t binsettings[H2_BINSETTINGS_LEN];
    ssize_t rclen;
    size_t binlen; /* length of the binsettings data */

    rclen = populate_binsettings(binsettings, معلومات);

    إذا(!curlx_sztouz(rclen, &binlen) || !binlen) {
      failf(معلومات, "nghttp2 unexpectedly failed on pack_settings_payload");
      پایله = CURLE_FAILED_INIT;
      اذهب بهر;
    }

    پایله = http2_معلومات_تیاری(cf, معلومات, &بهیر);
    إذا(پایله)
      اذهب بهر;
    DEBUGASSERT(بهیر);
    بهیر->id = 1;
    /* queue SETTINGS frame (again) */
    rc = Curl_ngh2_session_upgrade2(
      &ctx->ngh2, binsettings, binlen, معلومات->state.httpreq == HTTPREQ_سر,
      عدم);
    إذا(rc) {
      failf(معلومات, "nghttp2_session_upgrade2() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      پایله = CURLE_HTTP2;
      اذهب بهر;
    }

    rc = Curl_ngh2_session_سېټ_بهیر_user_معلومات(
      &ctx->ngh2, بهیر->id, بهیر);
    إذا(rc) {
      infof(معلومات, "http/2: failed to set user_data for stream %d",
            بهیر->id);
      DEBUGASSERT(0);
    }
    CURL_TRC_CF(معلومات, cf, "created session via Upgrade");
  }
  وإلا {
    nghttp2_settings_ثبت iv[H2_SETTINGS_IV_LEN];
    size_t ivlen;

    ivlen = populate_settings(iv, معلومات, ctx);
    rc = Curl_ngh2_submit_settings(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, iv, ivlen);
    إذا(rc) {
      failf(معلومات, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      پایله = CURLE_HTTP2;
      اذهب بهر;
    }
  }

  rc = Curl_ngh2_session_سېټ_local_کړکۍ_اندازه(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, HTTP2_HUGE_کړکۍ_اندازه);
  إذا(rc) {
    failf(معلومات, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    پایله = CURLE_HTTP2;
    اذهب بهر;
  }

  /* all set, traffic will be send on connect */
  پایله = CURLE_OK;
  CURL_TRC_CF(معلومات, cf, "[0] created h2 session%s",
              ctx->له_لارې_h1_upgrade ? " (via h1 upgrade)" : "");

بهر:
  إذا(cbs)
    nghttp2_session_callbacks_del(cbs);
  أرجع پایله;
}

ساكن CURLcode cf_h2_connect(بنية Curl_cfilter *cf,
                              بنية Curl_easy *معلومات,
                              bool *بشپړ)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  CURLcode پایله = CURLE_OK;
  بنية cf_call_معلومات save;
  bool لومړی_وخت = FALSE;

  إذا(cf->connected) {
    *بشپړ = TRUE;
    أرجع CURLE_OK;
  }

  /* Connect the lower filters first */
  إذا(!cf->بل->connected) {
    پایله = Curl_conn_cf_connect(cf->بل, معلومات, بشپړ);
    إذا(پایله || !*بشپړ)
      أرجع پایله;
  }

  *بشپړ = FALSE;

  CF_معلومات_SAVE(save, cf, معلومات);
  DEBUGASSERT(ctx->initialized);
  إذا(!ctx->ngh2.s) {
    پایله = cf_h2_ctx_open(cf, معلومات);
    إذا(پایله)
      اذهب بهر;
    لومړی_وخت = TRUE;
  }

  إذا(!لومړی_وخت) {
    پایله = h2_پرمختګ_ingress(cf, معلومات, 0);
    إذا(پایله)
      اذهب بهر;
  }

  /* Send out our SETTINGS and ACKs and such. If that blocks, we
   * have it buffered and can count this filter as being connected */
  پایله = h2_پرمختګ_egress(cf, معلومات);
  إذا(پایله && (پایله != CURLE_AGAIN))
    اذهب بهر;

  *بشپړ = TRUE;
  cf->connected = TRUE;
  پایله = CURLE_OK;

بهر:
  CURL_TRC_CF(معلومات, cf, "cf_connect() -> %d, %d, ", (صحيح)پایله, *بشپړ);
  CF_معلومات_RESTORE(cf, save);
  أرجع پایله;
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
                               بنية Curl_easy *معلومات, bool *بشپړ)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية cf_call_معلومات save;
  CURLcode پایله;
  صحيح rv;

  إذا(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *بشپړ = TRUE;
    أرجع CURLE_OK;
  }

  CF_معلومات_SAVE(save, cf, معلومات);

  إذا(!ctx->لېږل_شوی_goaway) {
    ctx->لېږل_شوی_goaway = TRUE;
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, ctx->local_max_sid, 0,
      (ثابت uint8_t *)"shutdown", حجم("shutdown"));
    إذا(rv) {
      failf(معلومات, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      پایله = CURLE_SEND_ERROR;
      اذهب بهر;
    }
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  پایله = CURLE_OK;
  إذا(Curl_ngh2_session_want_write(&ctx->ngh2) ||
     !Curl_bufq_is_تشه(&ctx->outbufq))
    پایله = h2_پرمختګ_egress(cf, معلومات);
  إذا(!پایله && Curl_ngh2_session_want_read(&ctx->ngh2))
    پایله = h2_پرمختګ_ingress(cf, معلومات, 0);

  إذا(پایله == CURLE_AGAIN)
    پایله = CURLE_OK;

  *بشپړ = (ctx->conn_closed ||
           (!پایله && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2) &&
            Curl_bufq_is_تشه(&ctx->outbufq)));

بهر:
  CF_معلومات_RESTORE(cf, save);
  cf->shutdown = (پایله || *بشپړ);
  أرجع پایله;
}

ساكن CURLcode http2_معلومات_pause(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 bool pause)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_بهیر_ctx *بهیر = H2_بهیر_CTX(ctx, معلومات);

  DEBUGASSERT(معلومات);
  إذا(ctx && Curl_ngh2_valid(&ctx->ngh2) && بهیر) {
    CURLcode پایله;

    بهیر->write_paused = pause;
    پایله = cf_h2_update_local_win(cf, معلومات, بهیر);
    إذا(پایله)
      أرجع پایله;

    /* attempt to send the window update */
    (فراغ)h2_پرمختګ_egress(cf, معلومات);

    إذا(!pause) {
      /* Unpausing a h2 transfer, requires it to be run again. The server
       * may send new DATA on us increasing the flow window, and it may
       * not. We may have already buffered and exhausted the new window
       * by operating on things in flight during the handling of other
       * transfers. */
      Curl_multi_mark_dirty(معلومات);
    }
    CURL_TRC_CF(معلومات, cf, "[%d] stream now %spaused", بهیر->id,
                pause ? "" : "un");
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cf_h2_cntrl(بنية Curl_cfilter *cf,
                            بنية Curl_easy *معلومات,
                            صحيح event, صحيح arg1, فراغ *arg2)
{
  CURLcode پایله = CURLE_OK;
  بنية cf_call_معلومات save;

  (فراغ)arg2;

  CF_معلومات_SAVE(save, cf, معلومات);
  بدل(event) {
  حالة CF_CTRL_معلومات_تیاری:
    اكسر;
  حالة CF_CTRL_معلومات_PAUSE:
    پایله = http2_معلومات_pause(cf, معلومات, (arg1 != 0));
    اكسر;
  حالة CF_CTRL_FLUSH:
    پایله = cf_h2_flush(cf, معلومات);
    اكسر;
  حالة CF_CTRL_معلومات_بشپړ:
    http2_معلومات_بشپړ(cf, معلومات);
    اكسر;
  حالة CF_CTRL_CONN_INFO_UPDATE:
    إذا(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_کتل_شوی = 20;
      Curl_conn_سېټ_multiplex(cf->conn);
    }
    اكسر;
  افتراضي:
    اكسر;
  }
  CF_معلومات_RESTORE(cf, save);
  أرجع پایله;
}

ساكن bool cf_h2_معلومات_pending(بنية Curl_cfilter *cf,
                               ثابت بنية Curl_easy *معلومات)
{
  بنية cf_h2_ctx *ctx = cf->ctx;

  إذا(ctx && !Curl_bufq_is_تشه(&ctx->inbufq))
    أرجع TRUE;
  أرجع cf->بل ? cf->بل->cft->has_معلومات_pending(cf->بل, معلومات) : FALSE;
}

ساكن bool cf_h2_is_alive(بنية Curl_cfilter *cf,
                           بنية Curl_easy *معلومات,
                           bool *ننوت_pending)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  bool alive;
  بنية cf_call_معلومات save;

  *ننوت_pending = FALSE;
  CF_معلومات_SAVE(save, cf, معلومات);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
           http2_connisalive(cf, معلومات, ننوت_pending));
  CURL_TRC_CF(معلومات, cf, "conn alive -> %d, input_pending=%d",
              alive, *ننوت_pending);
  CF_معلومات_RESTORE(cf, save);
  أرجع alive;
}

ساكن CURLcode cf_h2_keep_alive(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات)
{
  CURLcode پایله;
  بنية cf_call_معلومات save;

  CF_معلومات_SAVE(save, cf, معلومات);
  پایله = http2_send_ping(cf, معلومات);
  CF_معلومات_RESTORE(cf, save);
  أرجع پایله;
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
    *pres1 = (effective_max > لوی_عدد) ? لوی_عدد : (صحيح)effective_max;
    CF_معلومات_RESTORE(cf, save);
    أرجع CURLE_OK;
  حالة CF_QUERY_بهیر_ERROR: {
    بنية h2_بهیر_ctx *بهیر = H2_بهیر_CTX(ctx, معلومات);
    *pres1 = بهیر ? (صحيح)بهیر->error : 0;
    أرجع CURLE_OK;
  }
  حالة CF_QUERY_NEED_FLUSH: {
    بنية h2_بهیر_ctx *بهیر = H2_بهیر_CTX(ctx, معلومات);
    إذا(!Curl_bufq_is_تشه(&ctx->outbufq) ||
       (بهیر && !Curl_bufq_is_تشه(&بهیر->sendbuf))) {
      *pres1 = TRUE;
      أرجع CURLE_OK;
    }
    اكسر;
  }
  حالة CF_QUERY_HTTP_نسخه:
    *pres1 = 20;
    أرجع CURLE_OK;
  افتراضي:
    اكسر;
  }
  أرجع cf->بل ?
    cf->بل->cft->query(cf->بل, معلومات, query, pres1, pres2) :
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

ساكن CURLcode http2_cfilter_جمع_کړه(بنية Curl_cfilter **pcf,
                                  بنية Curl_easy *معلومات,
                                  بنية connectdata *conn,
                                  int8_t sockindex,
                                  bool له_لارې_h1_upgrade)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_h2_ctx *ctx;
  CURLcode پایله = CURLE_بهر_د_حافظه;

  DEBUGASSERT(معلومات->conn);
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx)
    اذهب بهر;
  cf_h2_ctx_init(ctx, له_لارې_h1_upgrade);

  پایله = Curl_cf_create(&cf, &Curl_cft_nghttp2, ctx);
  إذا(پایله)
    اذهب بهر;

  ctx = عدم;
  Curl_conn_cf_جمع_کړه(معلومات, conn, sockindex, cf);

بهر:
  إذا(پایله)
    cf_h2_ctx_free(ctx);
  *pcf = پایله ? عدم : cf;
  أرجع پایله;
}

ساكن CURLcode http2_cfilter_ورننباسه_after(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *معلومات,
                                           bool له_لارې_h1_upgrade)
{
  بنية Curl_cfilter *cf_h2 = عدم;
  بنية cf_h2_ctx *ctx;
  CURLcode پایله = CURLE_بهر_د_حافظه;

  (فراغ)معلومات;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx)
    اذهب بهر;
  cf_h2_ctx_init(ctx, له_لارې_h1_upgrade);

  پایله = Curl_cf_create(&cf_h2, &Curl_cft_nghttp2, ctx);
  إذا(پایله)
    اذهب بهر;

  ctx = عدم;
  Curl_conn_cf_ورننباسه_after(cf, cf_h2);

بهر:
  إذا(پایله)
    cf_h2_ctx_free(ctx);
  أرجع پایله;
}

bool Curl_http2_may_switch(بنية Curl_easy *معلومات)
{
  إذا(Curl_conn_http_نسخه(معلومات, معلومات->conn) < 20 &&
     (معلومات->state.http_neg.wanted & CURL_HTTP_V2x) &&
     معلومات->state.http_neg.h2_مخکینی_knowledge) {
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
    إذا(معلومات->conn->بټونه.origin_is_proxy) {
      infof(معلومات, "Ignoring HTTP/2 prior knowledge due to proxy");
      أرجع FALSE;
    }
#د_که_پای
    أرجع TRUE;
  }
  أرجع FALSE;
}

CURLcode Curl_http2_switch(بنية Curl_easy *معلومات)
{
  بنية Curl_cfilter *cf;
  CURLcode پایله;

  DEBUGASSERT(Curl_conn_http_نسخه(معلومات, معلومات->conn) < 20);

  پایله = http2_cfilter_جمع_کړه(&cf, معلومات, معلومات->conn, FIRSTSOCKET, FALSE);
  إذا(پایله)
    أرجع پایله;
  CURL_TRC_CF(معلومات, cf, "switching connection to HTTP/2");

  إذا(cf->بل) {
    bool بشپړ;
    أرجع Curl_conn_cf_connect(cf, معلومات, &بشپړ);
  }
  أرجع CURLE_OK;
}

CURLcode Curl_http2_switch_at(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات)
{
  بنية Curl_cfilter *cf_h2;
  CURLcode پایله;

  DEBUGASSERT(Curl_conn_http_نسخه(معلومات, معلومات->conn) < 20);

  پایله = http2_cfilter_ورننباسه_after(cf, معلومات, FALSE);
  إذا(پایله)
    أرجع پایله;

  cf_h2 = cf->بل;

  إذا(cf_h2->بل) {
    bool بشپړ;
    أرجع Curl_conn_cf_connect(cf_h2, معلومات, &بشپړ);
  }
  أرجع CURLE_OK;
}

CURLcode Curl_http2_upgrade(بنية Curl_easy *معلومات,
                            بنية connectdata *conn, int8_t sockindex,
                            ثابت حرف *mem, size_t nread)
{
  بنية Curl_cfilter *cf;
  بنية cf_h2_ctx *ctx;
  CURLcode پایله;

  DEBUGASSERT(Curl_conn_http_نسخه(معلومات, conn) < 20);

  پایله = http2_cfilter_جمع_کړه(&cf, معلومات, conn, sockindex, TRUE);
  إذا(پایله)
    أرجع پایله;
  CURL_TRC_CF(معلومات, cf, "upgrading connection to HTTP/2");

  DEBUGASSERT(cf->cft == &Curl_cft_nghttp2);
  ctx = cf->ctx;

  معلومات->req.httpversion_لېږل_شوی = 20; /* it is an h2 request now */
  معلومات->req.سرلیک = TRUE; /* we expect the real response to come in h2 */
  معلومات->req.headerline = 0; /* restart the header line counter */

  إذا(nread > 0) {
    /* Remaining data from the protocol switch reply is already using
     * the switched protocol, ie. HTTP/2. We add that to the network
     * inbufq. */
    size_t copied;

    پایله = Curl_bufq_write(&ctx->inbufq,
                             (ثابت بینښه حرف *)mem, nread, &copied);
    إذا(پایله) {
      failf(معلومات, "error on copying HTTP Upgrade response: %d", (صحيح)پایله);
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

  إذا(cf->بل) {
    bool بشپړ;
    پایله = Curl_conn_cf_connect(cf, معلومات, &بشپړ);
    إذا(!پایله)
      cf->cft->cntrl(cf, معلومات, CF_CTRL_CONN_INFO_UPDATE, 0, عدم);
  }
  أرجع پایله;
}

/* Only call this function for a transfer that already got an HTTP/2
   CURLE_HTTP2_STREAM error! */
bool Curl_h2_http_1_1_error(بنية Curl_easy *معلومات)
{
  إذا(Curl_conn_http_نسخه(معلومات, معلومات->conn) == 20) {
    صحيح err = Curl_conn_get_بهیر_error(معلومات, معلومات->conn, FIRSTSOCKET);
    أرجع err == NGHTTP2_HTTP_1_1_REQUIRED;
  }
  أرجع FALSE;
}

فراغ *Curl_nghttp2_malloc(size_t اندازه, فراغ *user_معلومات)
{
  (فراغ)user_معلومات;
  أرجع Curl_cmalloc(اندازه);
}

فراغ Curl_nghttp2_free(فراغ *ptr, فراغ *user_معلومات)
{
  (فراغ)user_معلومات;
  Curl_cfree(ptr);
}

فراغ *Curl_nghttp2_calloc(size_t nmemb, size_t اندازه, فراغ *user_معلومات)
{
  (فراغ)user_معلومات;
  أرجع Curl_ccalloc(nmemb, اندازه);
}

فراغ *Curl_nghttp2_realloc(فراغ *ptr, size_t اندازه, فراغ *user_معلومات)
{
  (فراغ)user_معلومات;
  أرجع Curl_crealloc(ptr, اندازه);
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

#د_که_پای /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
