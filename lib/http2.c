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

#إذا NGHTTP2_نسخه_رقم < 0x010f00
#خطا "nghttp2 1.15.0 or greater required"
#پایان_اگر

#تعریف_کن NGHTTP2_HAS_مجموعه_LOCAL_پنجره_اندازه 1

/* buffer dimensioning:
 * use 16K as chunk size, as that fits H2 DATA frames well */
#تعریف_کن H2_CHUNK_اندازه           (16 * 1024)
/* connection window size */
#تعریف_کن H2_CONN_پنجره_اندازه     (10 * 1024 * 1024)
/* on receiving from TLS, we prep for holding a full stream window */
#تعریف_کن H2_NW_RECV_CHUNKS       (H2_CONN_پنجره_اندازه / H2_CHUNK_اندازه)
/* on send into TLS, we want to accumulate small frames */
#تعریف_کن H2_NW_SEND_CHUNKS       1
/* this is how much we want "in flight" for a stream, unthrottled */
#تعریف_کن H2_جریان_پنجره_اندازه_MAX   (10 * 1024 * 1024)
/* this is how much we want "in flight" for a stream, initially, IFF
 * nghttp2 allows us to tweak the local window size. */
#إذا NGHTTP2_HAS_مجموعه_LOCAL_پنجره_اندازه
#تعریف_کن H2_جریان_پنجره_اندازه_INITIAL  (64 * 1024)
#وإلا
#تعریف_کن H2_جریان_پنجره_اندازه_INITIAL H2_جریان_پنجره_اندازه_MAX
#پایان_اگر
/* keep smaller stream upload buffer (default h2 window size) to have
 * our progress bars and "upload done" reporting closer to reality */
#تعریف_کن H2_جریان_SEND_CHUNKS   ((64 * 1024) / H2_CHUNK_اندازه)
/* spare chunks we keep for a full window */
#تعریف_کن H2_جریان_POOL_SPARES   (H2_CONN_پنجره_اندازه / H2_CHUNK_اندازه)

/* We need to accommodate the max number of streams with their window sizes on
 * the overall connection. Streams might become PAUSED which will block their
 * received QUOTA in the connection window. If we run out of space, the server
 * is blocked from sending us any data. See #10988 for an issue with this. */
#تعریف_کن HTTP2_HUGE_پنجره_اندازه (100 * H2_جریان_پنجره_اندازه_MAX)

#تعریف_کن H2_SETTINGS_IV_LEN 3
#تعریف_کن H2_BINSETTINGS_LEN 80

بنية cf_h2_ctx {
  بنية curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  بنية cf_call_داده call_داده;

  بنية bufq inbufq;           /* network input */
  بنية bufq outbufq;          /* network output */
  بنية bufc_pool جریان_bufcp; /* spares for stream buffers */
  بنية dynbuf scratch;        /* scratch buffer for temp use */

  بنية u32_ptrset streams; /* hash of `data->mid` to `h2_stream_ctx` */
  size_t drain_مجموع; /* sum of all stream's UrlState drain */
  uint32_t initial_win_اندازه; /* current initial window size (settings) */
  uint32_t max_concurrent_streams;
  uint32_t goaway_error;        /* goaway error code from server */
  int32_t remote_max_sid;       /* max id processed by server */
  int32_t local_max_sid;        /* max id processed by us */
  بیت(initialized);
  بیت(از_طریق_h1_upgrade);
  بیت(conn_closed);
  بیت(rcvd_goaway);
  بیت(فرستاده_goaway);
  بیت(enable_افزودن);
  بیت(nw_بیرون_blocked);
};

/* How to access `call_data` from a cf_h2 filter */
#لغو_تعریف CF_CTX_CALL_داده
#تعریف_کن CF_CTX_CALL_داده(cf) ((بنية cf_h2_ctx *)(cf)->ctx)->call_داده

/**
 * All about the H2 internals of a stream
 */
بنية h2_جریان_ctx {
  بنية bufq sendbuf; /* request buffer */
  بنية h1_req_parser h1; /* parsing the request */
  بنية dynhds resp_trailers; /* response trailer fields */
  size_t resp_hds_len; /* amount of response header bytes in recvbuf */
  curl_off_t xfer_id;  /* transfer `id` owning this stream */
  curl_off_t nrcvd_داده;  /* number of DATA bytes received */

  حرف **افزودن_سرآیندها;       /* allocated array */
  size_t افزودن_سرآیندها_استفاده_شده;  /* number of entries filled in */
  size_t افزودن_سرآیندها_alloc; /* number of entries allocated */

  int32_t id; /* HTTP/2 protocol identifier for stream */
  uint32_t mid; /* transfer `mid` owning this stream */
  uint32_t error; /* stream error code */
  CURLcode xfer_نتیجه; /* Result of writing out response */
  int32_t local_پنجره_اندازه; /* the local recv window size */
  int32_t وضعیت_کد; /* HTTP response status code */
  بیت(resp_hds_complete); /* we have a complete, final response */
  بیت(closed); /* TRUE on stream close */
  بیت(reset);  /* TRUE on stream reset */
  بیت(reset_by_server);  /* TRUE on stream reset by server */
  بیت(close_handled); /* TRUE if stream closure is handled by libcurl */
  بیت(bodystarted);
  بیت(body_eos);    /* the complete body has been added to `sendbuf` and
                     * is being/has been processed from there. */
  بیت(write_paused);  /* stream write is paused */
};

ساكن فراغ free_افزودن_سرآیندها(بنية h2_جریان_ctx *جریان)
{
  size_t i;
  لكل(i = 0; i < جریان->افزودن_سرآیندها_استفاده_شده; i++)
    curlx_free(جریان->افزودن_سرآیندها[i]);
  curlx_safefree(جریان->افزودن_سرآیندها);
  جریان->افزودن_سرآیندها_استفاده_شده = 0;
}

ساكن فراغ h2_جریان_ctx_free(بنية h2_جریان_ctx *جریان)
{
  Curl_bufq_free(&جریان->sendbuf);
  Curl_h1_req_parse_free(&جریان->h1);
  Curl_dynhds_free(&جریان->resp_trailers);
  free_افزودن_سرآیندها(جریان);
  curlx_free(جریان);
}

ساكن فراغ h2_جریان_hash_free(uint32_t id, فراغ *جریان)
{
  (فراغ)id;
  DEBUGASSERT(جریان);
  h2_جریان_ctx_free((بنية h2_جریان_ctx *)جریان);
}

ساكن فراغ cf_h2_ctx_init(بنية cf_h2_ctx *ctx, bool از_طریق_h1_upgrade)
{
  Curl_bufcp_init(&ctx->جریان_bufcp, H2_CHUNK_اندازه, H2_جریان_POOL_SPARES);
  Curl_bufq_initp(&ctx->inbufq, &ctx->جریان_bufcp, H2_NW_RECV_CHUNKS, 0);
  Curl_bufq_initp(&ctx->outbufq, &ctx->جریان_bufcp, H2_NW_SEND_CHUNKS, 0);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_سرآیند);
  Curl_u32_ptrset_init(&ctx->streams, h2_جریان_hash_free);
  ctx->remote_max_sid = INT32_MAX;
  ctx->از_طریق_h1_upgrade = از_طریق_h1_upgrade;
  ctx->initialized = TRUE;
}

ساكن فراغ cf_h2_ctx_free(بنية cf_h2_ctx *ctx)
{
  إذا(ctx && ctx->initialized) {
    Curl_ngh2_del(&ctx->ngh2);
    Curl_bufq_free(&ctx->inbufq);
    Curl_bufq_free(&ctx->outbufq);
    Curl_bufcp_free(&ctx->جریان_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    memset(ctx, 0, حجم(*ctx));
  }
  curlx_free(ctx);
}

ساكن uint32_t cf_h2_initial_win_اندازه(بنية Curl_easy *داده)
{
#إذا NGHTTP2_HAS_مجموعه_LOCAL_پنجره_اندازه
  /* If the transfer has a rate-limit lower than the default initial
   * stream window size, use that. It needs to be at least 8k or servers
   * may be unhappy. */
  curl_off_t rps = Curl_rlimit_در_هر_گام(&داده->پیشرفت.dl.rlimit);
  إذا((rps > 0) && (rps < H2_جریان_پنجره_اندازه_INITIAL))
    أرجع CURLMAX((uint32_t)rps, 8192);
#پایان_اگر
  أرجع H2_جریان_پنجره_اندازه_INITIAL;
}

ساكن size_t populate_settings(nghttp2_settings_مدخل *iv,
                                بنية Curl_easy *داده,
                                بنية cf_h2_ctx *ctx)
{
  iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
  iv[0].مقدار = Curl_multi_max_concurrent_streams(داده->multi);

  iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_پنجره_اندازه;
  iv[1].مقدار = cf_h2_initial_win_اندازه(داده);
  إذا(ctx)
    ctx->initial_win_اندازه = iv[1].مقدار;
  iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_افزودن;
  iv[2].مقدار = !!داده->multi->افزودن_cb;

  أرجع 3;
}

ساكن ssize_t populate_binsettings(uint8_t *binsettings,
                                    بنية Curl_easy *داده)
{
  nghttp2_settings_مدخل iv[H2_SETTINGS_IV_LEN];
  size_t ivlen;

  ivlen = populate_settings(iv, داده, عدم);
  /* this returns number of bytes it wrote or a negative number on error. */
  أرجع nghttp2_pack_settings_payload(binsettings, H2_BINSETTINGS_LEN,
                                       iv, ivlen);
}

ساكن CURLcode cf_h2_update_settings(بنية cf_h2_ctx *ctx,
                                      uint32_t initial_win_اندازه)
{
  nghttp2_settings_مدخل مدخل;
  مدخل.settings_id = NGHTTP2_SETTINGS_INITIAL_پنجره_اندازه;
  مدخل.مقدار = initial_win_اندازه;
  إذا(Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, &مدخل, 1))
    أرجع CURLE_SEND_ERROR;
  ctx->initial_win_اندازه = initial_win_اندازه;
  أرجع CURLE_OK;
}

#تعریف_کن H2_جریان_CTX(ctx, داده)                                         \
  ((بنية h2_جریان_ctx *)(                                             \
    (داده) ? Curl_u32_ptrset_get(&(ctx)->streams, (داده)->mid) : عدم))

ساكن بنية h2_جریان_ctx *h2_جریان_ctx_create(بنية cf_h2_ctx *ctx,
                                                  بنية Curl_easy *داده)
{
  بنية h2_جریان_ctx *جریان;

  جریان = curlx_calloc(1, حجم(*جریان));
  إذا(!جریان)
    أرجع عدم;

  جریان->id = -1;
  جریان->xfer_id = داده->id;
  جریان->mid = داده->mid;
  Curl_bufq_initp(&جریان->sendbuf, &ctx->جریان_bufcp,
                  H2_جریان_SEND_CHUNKS, BUFQ_OPT_NONE);
  Curl_h1_req_parse_init(&جریان->h1, H1_PARSE_DEFAULT_MAX_سطر_LEN);
  Curl_dynhds_init(&جریان->resp_trailers,
                   MAX_HTTP_RESP_سرآیند_شمار, DYN_HTTP_REQUEST);
  جریان->bodystarted = FALSE;
  جریان->وضعیت_کد = -1;
  جریان->closed = FALSE;
  جریان->close_handled = FALSE;
  جریان->error = NGHTTP2_NO_ERROR;
  جریان->local_پنجره_اندازه = H2_جریان_پنجره_اندازه_INITIAL;
  جریان->nrcvd_داده = 0;
  أرجع جریان;
}

ساكن بنية Curl_easy *cf_h2_get_جریان_xfer(بنية Curl_cfilter *cf,
                                               int32_t جریان_id,
                                               بنية h2_جریان_ctx **pstream)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_جریان_ctx *جریان =
    Curl_ngh2_session_get_جریان_user_داده(&ctx->ngh2, جریان_id);
  إذا(pstream)
    *pstream = جریان;
  إذا(جریان && (جریان->mid != UINT32_MAX)) {
    بنية Curl_easy *داده, *call_داده = CF_داده_CURRENT(cf);
    إذا(!call_داده || !call_داده->multi) {
      DEBUGASSERT(0);
      أرجع عدم;
    }
    داده = Curl_multi_get_easy(call_داده->multi, جریان->mid);
    إذا(داده && (داده->id == جریان->xfer_id))
      أرجع داده;
  }
  أرجع عدم;
}

#اگر_تعریف_شده NGHTTP2_HAS_مجموعه_LOCAL_پنجره_اندازه
ساكن int32_t cf_h2_get_desired_local_win(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *داده)
{
  curl_off_t avail = Curl_rlimit_avail(&داده->پیشرفت.dl.rlimit, عدم);

  (فراغ)cf;
  إذا(avail < CURL_OFF_T_MAX) { /* limit in place */
    إذا(avail <= 0)
      أرجع 0;
    وإلا إذا(avail < H2_جریان_پنجره_اندازه_MAX)
      أرجع (int32_t)avail;
  }
  أرجع H2_جریان_پنجره_اندازه_MAX;
}

ساكن CURLcode cf_h2_update_local_win(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *داده,
                                       بنية h2_جریان_ctx *جریان)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  int32_t dwsize;
  صحيح rv;

  dwsize = (جریان->write_paused || جریان->xfer_نتیجه) ?
           0 : cf_h2_get_desired_local_win(cf, داده);
  إذا(dwsize != جریان->local_پنجره_اندازه) {
    int32_t wsize = Curl_ngh2_session_get_جریان_effective_local_پنجره_اندازه(
      &ctx->ngh2, جریان->id);
    إذا(dwsize > wsize) {
      rv = Curl_ngh2_session_مجموعه_local_پنجره_اندازه(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, جریان->id, dwsize);
      إذا(rv) {
        failf(داده, "[%d] nghttp2 set_local_window_size(%d) failed: "
              "%s(%d)", جریان->id, dwsize, nghttp2_strerror(rv), rv);
        أرجع CURLE_HTTP2;
      }
      rv = Curl_ngh2_submit_پنجره_update(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, جریان->id, dwsize - wsize);
      إذا(rv) {
        failf(داده, "[%d] nghttp2_submit_window_update() failed: "
              "%s(%d)", جریان->id, nghttp2_strerror(rv), rv);
        أرجع CURLE_HTTP2;
      }
      جریان->local_پنجره_اندازه = dwsize;
      CURL_TRC_CF(داده, cf, "[%d] local window update by %d",
                  جریان->id, dwsize - wsize);
    }
    وإلا {
      rv = Curl_ngh2_session_مجموعه_local_پنجره_اندازه(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, جریان->id, dwsize);
      إذا(rv) {
        failf(داده, "[%d] nghttp2_session_set_local_window_size() failed: "
              "%s(%d)", جریان->id, nghttp2_strerror(rv), rv);
        أرجع CURLE_HTTP2;
      }
      جریان->local_پنجره_اندازه = dwsize;
      CURL_TRC_CF(داده, cf, "[%d] local window size now %d",
                  جریان->id, dwsize);
    }
  }
  أرجع CURLE_OK;
}

#وإلا /* NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

ساكن CURLcode cf_h2_update_local_win(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *داده,
                                       بنية h2_جریان_ctx *جریان)
{
  (فراغ)cf;
  (فراغ)داده;
  (فراغ)جریان;
  أرجع CURLE_OK;
}
#پایان_اگر /* !NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

ساكن CURLcode http2_داده_راهاندازی(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده,
                                 بنية h2_جریان_ctx **pstream)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_جریان_ctx *جریان;

  (فراغ)cf;
  DEBUGASSERT(داده);

  إذا(!داده)
    أرجع CURLE_BAD_FUNCTION_آرگومان;

  جریان = H2_جریان_CTX(ctx, داده);
  إذا(جریان) {
    *pstream = جریان;
    أرجع CURLE_OK;
  }

  جریان = h2_جریان_ctx_create(ctx, داده);
  إذا(!جریان)
    أرجع CURLE_بیرون_از_حافظه;

  إذا(Curl_u32_ptrset_مجموعه(&ctx->streams, داده->mid, جریان)) {
    h2_جریان_ctx_free(جریان);
    أرجع CURLE_بیرون_از_حافظه;
  }

  *pstream = جریان;
  أرجع CURLE_OK;
}

ساكن CURLcode nw_بیرون_flush(بنية Curl_cfilter *cf,
                             بنية Curl_easy *داده)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode نتیجه;

  إذا(Curl_bufq_is_خالی(&ctx->outbufq))
    أرجع CURLE_OK;

  نتیجه = Curl_cf_send_bufq(cf->بعدی, داده, &ctx->outbufq, عدم, 0,
                             &nwritten);
  إذا(نتیجه) {
    إذا(نتیجه == CURLE_AGAIN) {
      CURL_TRC_CF(داده, cf, "flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_بیرون_blocked = 1;
    }
    أرجع نتیجه;
  }
  أرجع Curl_bufq_is_خالی(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

ساكن فراغ http2_داده_انجامشده(بنية Curl_cfilter *cf, بنية Curl_easy *داده)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_جریان_ctx *جریان = H2_جریان_CTX(ctx, داده);

  DEBUGASSERT(ctx);
  إذا(!جریان || !ctx->initialized)
    أرجع;

  إذا(Curl_ngh2_valid(&ctx->ngh2)) {
    bool flush_egress = FALSE;
    /* returns error if stream not known, which is fine here */
    (فراغ)Curl_ngh2_session_مجموعه_جریان_user_داده(&ctx->ngh2, جریان->id, عدم);

    إذا(!جریان->closed && جریان->id > 0) {
      /* RST_STREAM */
      CURL_TRC_CF(داده, cf, "[%d] premature DATA_DONE, RST stream",
                  جریان->id);
      جریان->closed = TRUE;
      جریان->reset = TRUE;
      Curl_ngh2_submit_rst_جریان(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                  جریان->id, NGHTTP2_جریان_CLOSED);
      flush_egress = TRUE;
    }

    إذا(flush_egress) {
      (فراغ)Curl_ngh2_session_send(&ctx->ngh2);
      (فراغ)nw_بیرون_flush(cf, داده);
    }
  }

  Curl_u32_ptrset_unset(&ctx->streams, داده->mid);
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
  nghttp2_option_مجموعه_no_auto_پنجره_update(o, 1);
#إذا NGHTTP2_نسخه_رقم >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_مجموعه_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#پایان_اگر
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  أرجع rc;
}

/*
 * Returns nonzero if current HTTP/2 session should be closed.
 */
ساكن صحيح should_close_session(بنية cf_h2_ctx *ctx)
{
  أرجع ctx->drain_مجموع == 0 && !Curl_ngh2_session_want_read(&ctx->ngh2) &&
    !Curl_ngh2_session_want_write(&ctx->ngh2);
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
ساكن CURLcode h2_process_pending_درون_داد(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *داده)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  ثابت بیعلامت حرف *buf;
  size_t blen, nread;
  ssize_t rv;

  طالما(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {
    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ثابت uint8_t *)buf, blen);
    إذا(!curlx_sztouz(rv, &nread)) {
      failf(داده, "nghttp2 recv error %zd: %s", rv, nghttp2_strerror((صحيح)rv));
      أرجع CURLE_RECV_ERROR;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    إذا(Curl_bufq_is_خالی(&ctx->inbufq)) {
      اكسر;
    }
    وإلا {
      CURL_TRC_CF(داده, cf, "process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }

  إذا(Curl_ngh2_session_check_request_allowed(&ctx->ngh2) == 0) {
    /* No more requests are allowed in the current session, so
       the connection may not be reused. This is set when a
       GOAWAY frame has been received or when the limit of stream
       identifiers has been reached. */
    CURL_TRC_M(داده, "http/2: No new requests allowed");
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
ساكن bool http2_connisalive(بنية Curl_cfilter *cf, بنية Curl_easy *داده,
                              bool *درون_داد_pending)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *درون_داد_pending = FALSE;
  إذا(!cf->بعدی || !cf->بعدی->cft->is_alive(cf->بعدی, داده, درون_داد_pending))
    أرجع FALSE;

  إذا(*درون_داد_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode نتیجه;
    size_t nread;

    *درون_داد_pending = FALSE;
    نتیجه = Curl_cf_recv_bufq(cf->بعدی, داده, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(داده, cf, "connisalive, recv pending input -> %d, %zu",
                (صحيح)نتیجه, nread);
    إذا(!نتیجه) {
      نتیجه = h2_process_pending_درون_داد(cf, داده);
      إذا(نتیجه)
        /* immediate error, considered dead */
        alive = FALSE;
      وإلا {
        alive = !should_close_session(ctx);
      }
    }
    وإلا إذا(نتیجه != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  أرجع alive;
}

ساكن CURLcode http2_send_ping(بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  صحيح rc;

  rc = Curl_ngh2_submit_ping(&ctx->ngh2, 0, ZERO_NULL);
  إذا(rc) {
    failf(داده, "nghttp2_submit_ping() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    أرجع CURLE_HTTP2;
  }

  rc = Curl_ngh2_session_send(&ctx->ngh2);
  إذا(rc) {
    failf(داده, "nghttp2_session_send() failed: %s(%d)",
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
  بنية Curl_easy *داده = CF_داده_CURRENT(cf);
  size_t nwritten;
  CURLcode نتیجه = CURLE_OK;

  (فراغ)h2;
  (فراغ)flags;
  DEBUGASSERT(داده);

  إذا(!cf->connected)
    نتیجه = Curl_bufq_write(&ctx->outbufq, buf, blen, &nwritten);
  وإلا
    نتیجه = Curl_cf_send_bufq(cf->بعدی, داده, &ctx->outbufq, buf, blen,
                               &nwritten);

  إذا(نتیجه) {
    إذا(نتیجه == CURLE_AGAIN) {
      ctx->nw_بیرون_blocked = 1;
      أرجع NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(داده, "Failed sending HTTP2 data");
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  إذا(!nwritten) {
    ctx->nw_بیرون_blocked = 1;
    أرجع NGHTTP2_ERR_WOULDBLOCK;
  }
  أرجع (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

/* We pass a pointer to this struct in the push callback, but the contents of
   the struct are hidden from the user. */
بنية curl_pushheaders {
  بنية Curl_easy *داده;
  بنية h2_جریان_ctx *جریان;
  ثابت nghttp2_افزودن_promise *قاب;
};

/*
 * push header access function. Only to be used from within the push callback
 */
حرف *curl_pushheader_bynum(بنية curl_pushheaders *h, size_t رقم)
{
  /* Verify that we got a good easy handle in the push header struct, mostly to
     detect rubbish input fast(er). */
  إذا(!h || !GOOD_EASY_HANDLE(h->داده))
    أرجع عدم;
  وإلا {
    إذا(h->جریان && رقم < h->جریان->افزودن_سرآیندها_استفاده_شده)
      أرجع h->جریان->افزودن_سرآیندها[رقم];
  }
  أرجع عدم;
}

/*
 * push header access function. Only to be used from within the push callback
 */
حرف *curl_pushheader_byname(بنية curl_pushheaders *h, ثابت حرف *name)
{
  بنية h2_جریان_ctx *جریان;
  size_t len;
  size_t i;
  /* Verify that we got a good easy handle in the push header struct,
     mostly to detect rubbish input fast(er). Also empty header name
     is rubbish too. We have to allow ":" at the beginning of
     the header, but header == ":" must be rejected. If we have ':' in
     the middle of header, it could be matched in middle of the value,
     this is because we do prefix match.*/
  إذا(!h || !GOOD_EASY_HANDLE(h->داده) || !name || !name[0] ||
     !strcmp(name, ":") || strchr(name + 1, ':'))
    أرجع عدم;

  جریان = h->جریان;
  إذا(!جریان)
    أرجع عدم;

  len = طول_رشته(name);
  لكل(i = 0; i < جریان->افزودن_سرآیندها_استفاده_شده; i++) {
    إذا(!مقایسه_رشته(name, جریان->افزودن_سرآیندها[i], len)) {
      /* sub-match, make sure that it is followed by a colon */
      إذا(جریان->افزودن_سرآیندها[i][len] != ':')
        استمر;
      أرجع &جریان->افزودن_سرآیندها[i][len + 1];
    }
  }
  أرجع عدم;
}

ساكن بنية Curl_easy *h2_duphandle(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *داده)
{
  بنية Curl_easy *دومی = curl_easy_init();
  إذا(دومی) {
    بنية h2_جریان_ctx *دومی_جریان;
    http2_داده_راهاندازی(cf, دومی, &دومی_جریان);
    دومی->state.وزن = داده->state.وزن;
    إذا(داده->share)
      (فراغ)Curl_share_easy_link(دومی, داده->share);
    /* copy a range of options from the parent handle */
    دومی->مجموعه.fwrite_func = داده->مجموعه.fwrite_func;
    دومی->مجموعه.بیرون = داده->مجموعه.بیرون;
    دومی->مجموعه.fwrite_سرآیند = داده->مجموعه.fwrite_سرآیند;
    دومی->مجموعه.writeheader = داده->مجموعه.writeheader;
    دومی->مجموعه.fdebug = داده->مجموعه.fdebug;
    دومی->مجموعه.debugdata = داده->مجموعه.debugdata;
    دومی->مجموعه.fxferinfo = داده->مجموعه.fxferinfo;
    دومی->مجموعه.fprogress = داده->مجموعه.fprogress;
    دومی->مجموعه.پیشرفت_client = داده->مجموعه.پیشرفت_client;
    دومی->پیشرفت.hide = داده->پیشرفت.hide;
    دومی->پیشرفت.callback = داده->پیشرفت.callback;
  }
  أرجع دومی;
}

ساكن صحيح مجموعه_transfer_url(بنية Curl_easy *newhandle,
                            بنية curl_pushheaders *hp,
                            بنية Curl_easy *داده)
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
    uc = curl_url_مجموعه(u, CURLUPART_SCHEME, v, 0);
    إذا(uc) {
      rc = 1;
      اذهب fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_AUTHORITY);
  إذا(v) {
    uc = Curl_url_مجموعه_authority(u, v);
    إذا(uc) {
      rc = 2;
      اذهب fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_مسیر);
  إذا(v) {
    uc = curl_url_مجموعه(u, CURLUPART_مسیر, v, 0);
    إذا(uc) {
      rc = 3;
      اذهب fail;
    }
  }

  /* We can only allow PUSH of resource from the same origin, e.g.
   * scheme + hostname + port */
  إذا(!Curl_url_same_origin(داده->state.uh, u)) {
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
  Curl_bufref_مجموعه(&newhandle->state.url, url, 0, curl_free);
  أرجع 0;
}

ساكن فراغ discard_newhandle(بنية Curl_cfilter *cf,
                              بنية Curl_easy *newhandle)
{
  http2_داده_انجامشده(cf, newhandle);
  (فراغ)Curl_close(&newhandle);
}

ساكن صحيح افزودن_promise(بنية Curl_cfilter *cf,
                        بنية Curl_easy *داده,
                        ثابت nghttp2_افزودن_promise *قاب)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  صحيح rv; /* one of the CURL_PUSH_* defines */

  CURL_TRC_CF(داده, cf, "[%d] PUSH_PROMISE received",
              قاب->promised_جریان_id);
  إذا(داده->multi->افزودن_cb) {
    بنية h2_جریان_ctx *جریان;
    بنية h2_جریان_ctx *newstream;
    بنية curl_pushheaders heads;
    CURLMcode mresult;
    CURLcode نتیجه;
    /* clone the parent */
    بنية Curl_easy *newhandle = h2_duphandle(cf, داده);
    إذا(!newhandle) {
      infof(داده, "failed to duplicate handle");
      rv = CURL_افزودن_DENY; /* FAIL HARD */
      اذهب fail;
    }

    جریان = H2_جریان_CTX(ctx, داده);
    إذا(!جریان) {
      failf(داده, "Internal NULL stream");
      discard_newhandle(cf, newhandle);
      rv = CURL_افزودن_DENY;
      اذهب fail;
    }

    heads.داده = داده;
    heads.جریان = جریان;
    heads.قاب = قاب;

    rv = مجموعه_transfer_url(newhandle, &heads, داده);
    إذا(rv) {
      CURL_TRC_CF(داده, cf, "[%d] PUSH_PROMISE, failed to set URL -> %d",
                  قاب->promised_جریان_id, rv);
      discard_newhandle(cf, newhandle);
      rv = CURL_افزودن_DENY;
      اذهب fail;
    }

    {
      بنية Curl_mapi_guard guard;
      CURL_CBAPI_شروع(&guard, داده, multi_افزودن_cb);
      rv = داده->multi->افزودن_cb(داده, newhandle,
                                جریان->افزودن_سرآیندها_استفاده_شده, &heads,
                                داده->multi->افزودن_userp);
      CURL_CBAPI_انتها(&guard);
    }

    /* free the headers again */
    free_افزودن_سرآیندها(جریان);

    إذا(rv) {
      DEBUGASSERT((rv > CURL_افزودن_OK) && (rv <= CURL_افزودن_ERROROUT));
      /* denied, kill off the new handle again */
      CURL_TRC_CF(داده, cf, "[%d] PUSH_PROMISE, denied by application -> %d",
                  قاب->promised_جریان_id, rv);
      discard_newhandle(cf, newhandle);
      اذهب fail;
    }

    /* approved, add to the multi handle for processing. This
     * assigns newhandle->mid. For the new `mid` we assign the
     * h2_stream instance and remember the stream_id already known. */
    mresult = Curl_multi_جمع_کن_perform(داده->multi, newhandle, cf->conn);
    إذا(mresult) {
      infof(داده, "failed to add handle to multi");
      discard_newhandle(cf, newhandle);
      rv = CURL_افزودن_DENY;
      اذهب fail;
    }

    نتیجه = http2_داده_راهاندازی(cf, newhandle, &newstream);
    إذا(نتیجه) {
      failf(داده, "error setting up stream: %d", (صحيح)نتیجه);
      discard_newhandle(cf, newhandle);
      rv = CURL_افزودن_DENY;
      اذهب fail;
    }

    DEBUGASSERT(newstream);
    newstream->id = قاب->promised_جریان_id;
    newhandle->req.maxdownload = -1;
    newhandle->req.اندازه = -1;

    CURL_TRC_CF(داده, cf, "promise easy handle added to multi, mid=%u",
                newhandle->mid);
    rv = Curl_ngh2_session_مجموعه_جریان_user_داده(
      &ctx->ngh2, newstream->id, newstream);
    إذا(rv) {
      infof(داده, "failed to set user_data for stream %d",
            newstream->id);
      DEBUGASSERT(0);
      discard_newhandle(cf, newhandle);
      rv = CURL_افزودن_DENY;
      اذهب fail;
    }

    /* success, remember max stream id processed */
    إذا(newstream->id > ctx->local_max_sid)
      ctx->local_max_sid = newstream->id;
  }
  وإلا {
    CURL_TRC_CF(داده, cf, "Got PUSH_PROMISE, ignore it");
    rv = CURL_افزودن_DENY;
  }
fail:
  أرجع rv;
}

ساكن فراغ h2_xfer_write_resp_hd(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده,
                                  بنية h2_جریان_ctx *جریان,
                                  ثابت حرف *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  إذا(!جریان->xfer_نتیجه) {
    جریان->xfer_نتیجه = Curl_xfer_write_resp_hd(داده, buf, blen, eos);
    إذا(!جریان->xfer_نتیجه && !eos)
      جریان->xfer_نتیجه = cf_h2_update_local_win(cf, داده, جریان);
    إذا(جریان->xfer_نتیجه)
      CURL_TRC_CF(داده, cf, "[%d] error %d writing %zu bytes of headers",
                  جریان->id, (صحيح)جریان->xfer_نتیجه, blen);
  }
}

ساكن فراغ h2_xfer_write_resp(بنية Curl_cfilter *cf,
                               بنية Curl_easy *داده,
                               بنية h2_جریان_ctx *جریان,
                               ثابت حرف *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  إذا(!جریان->xfer_نتیجه)
    جریان->xfer_نتیجه = Curl_xfer_write_resp(داده, buf, blen, eos);
  /* If the transfer write is errored, we do not want any more data */
  إذا(جریان->xfer_نتیجه) {
    بنية cf_h2_ctx *ctx = cf->ctx;
    CURL_TRC_CF(داده, cf, "[%d] error %d writing %zu bytes of data, "
                "RST-ing stream",
                جریان->id, (صحيح)جریان->xfer_نتیجه, blen);
    Curl_ngh2_submit_rst_جریان(
      &ctx->ngh2, 0, جریان->id, (uint32_t)NGHTTP2_ERR_CALLBACK_FAILURE);
  }
  وإلا إذا(!جریان->write_paused && Curl_xfer_write_is_paused(داده)) {
    CURL_TRC_CF(داده, cf, "[%d] stream output paused", جریان->id);
    جریان->write_paused = TRUE;
  }
  وإلا إذا(جریان->write_paused && !Curl_xfer_write_is_paused(داده)) {
    CURL_TRC_CF(داده, cf, "[%d] stream output unpaused", جریان->id);
    جریان->write_paused = FALSE;
  }

  إذا(!جریان->xfer_نتیجه && !eos)
    جریان->xfer_نتیجه = cf_h2_update_local_win(cf, داده, جریان);
}

ساكن CURLcode on_جریان_قاب(بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده,
                                بنية h2_جریان_ctx *جریان,
                                ثابت nghttp2_قاب *قاب)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  int32_t جریان_id = قاب->hd.جریان_id;
  صحيح rv;

  إذا(!جریان) {
    CURL_TRC_CF(داده, cf, "[%d] No stream_ctx set", جریان_id);
    أرجع CURLE_FAILED_INIT;
  }

  بدل(قاب->hd.type) {
  حالة NGHTTP2_داده:
    CURL_TRC_CF(داده, cf, "[%d] DATA, window=%d/%d",
                جریان_id,
                Curl_ngh2_session_get_جریان_effective_recv_داده_درازی(
                  &ctx->ngh2, جریان->id),
                Curl_ngh2_session_get_جریان_effective_local_پنجره_اندازه(
                  &ctx->ngh2, جریان->id));
    /* If !body started on this stream, then receiving DATA is illegal. */
    إذا(!جریان->bodystarted) {
      إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_جریان(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                       جریان_id, NGHTTP2_PROTOCOL_ERROR))) {
        أرجع CURLE_RECV_ERROR;
      }
    }
    اكسر;
  حالة NGHTTP2_سرآیندها:
    إذا(جریان->bodystarted) {
      /* Only valid HEADERS after body started is trailer HEADERS. We
         buffer them in on_header callback. */
      اكسر;
    }

    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    إذا(جریان->وضعیت_کد == -1)
      أرجع CURLE_RECV_ERROR;

    /* Only final status code signals the end of header */
    إذا(جریان->وضعیت_کد / 100 != 1) {
      جریان->resp_hds_complete = TRUE;
      جریان->bodystarted = TRUE;
    }
    وإلا
      جریان->وضعیت_کد = -1;

    h2_xfer_write_resp_hd(cf, داده, جریان, STRCONST("\r\n"),
                          (bool)جریان->closed);

    Curl_multi_mark_dirty(داده);
    اكسر;
  حالة NGHTTP2_افزودن_PROMISE:
    rv = افزودن_promise(cf, داده, &قاب->افزودن_promise);
    إذا(rv) { /* deny! */
      DEBUGASSERT((rv > CURL_افزودن_OK) && (rv <= CURL_افزودن_ERROROUT));
      إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_جریان(&ctx->ngh2, NGHTTP2_FLAG_NONE,
             قاب->افزودن_promise.promised_جریان_id, NGHTTP2_CANCEL)))
        أرجع CURLE_SEND_ERROR;
      وإلا إذا(rv == CURL_افزودن_ERROROUT) {
        /* fail the parent transfer, not the whole session */
        CURL_TRC_CF(داده, cf, "[%d] fail in PUSH_PROMISE received",
                    جریان_id);
        إذا(!جریان->xfer_نتیجه)
          جریان->xfer_نتیجه = CURLE_RECV_ERROR;
        Curl_multi_mark_dirty(داده);
      }
    }
    اكسر;
  حالة NGHTTP2_RST_جریان:
    إذا(قاب->rst_جریان.error_کد)
      جریان->reset_by_server = TRUE;
    Curl_multi_mark_dirty(داده);
    اكسر;
  حالة NGHTTP2_پنجره_UPDATE:
    إذا(CURL_REQ_WANT_SEND(داده) && Curl_bufq_is_خالی(&جریان->sendbuf)) {
      /* need more data, force processing of transfer */
      Curl_multi_mark_dirty(داده);
    }
    وإلا إذا(!Curl_bufq_is_خالی(&جریان->sendbuf)) {
      /* resume the potentially suspended stream */
      إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_session_resume_داده(&ctx->ngh2, جریان->id)))
        أرجع CURLE_SEND_ERROR;
    }
    اكسر;
  افتراضي:
    اكسر;
  }

  إذا(قاب->hd.flags & NGHTTP2_FLAG_انتها_جریان) {
    إذا(!جریان->closed && !جریان->body_eos &&
       ((جریان->وضعیت_کد >= 400) || (جریان->وضعیت_کد < 200))) {
      /* The server did not give us a positive response and we are not
       * done uploading the request body. We need to stop doing that and
       * also inform the server that we aborted our side. */
      CURL_TRC_CF(داده, cf, "[%d] EOS frame with unfinished upload and "
                  "HTTP status %d, abort upload by RST",
                  جریان_id, جریان->وضعیت_کد);
      Curl_ngh2_submit_rst_جریان(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, جریان->id, NGHTTP2_جریان_CLOSED);
      جریان->closed = TRUE;
    }
    Curl_multi_mark_dirty(داده);
  }
  أرجع CURLE_OK;
}

#اگر_تعریف_شده CURLVERBOSE
صحيح Curl_nghttp2_fr_print(ثابت nghttp2_قاب *قاب, حرف *میانگیر,
                          size_t blen)
{
  بدل(قاب->hd.type) {
  حالة NGHTTP2_داده: {
    أرجع curl_msnprintf(میانگیر, blen,
                          "FRAME[DATA, len=%d, eos=%d, padlen=%d]",
                          (صحيح)قاب->hd.درازی,
                          !!(قاب->hd.flags & NGHTTP2_FLAG_انتها_جریان),
                          (صحيح)قاب->داده.padlen);
  }
  حالة NGHTTP2_سرآیندها: {
    أرجع curl_msnprintf(میانگیر, blen,
                          "FRAME[HEADERS, len=%d, hend=%d, eos=%d]",
                          (صحيح)قاب->hd.درازی,
                          !!(قاب->hd.flags & NGHTTP2_FLAG_انتها_سرآیندها),
                          !!(قاب->hd.flags & NGHTTP2_FLAG_انتها_جریان));
  }
  حالة NGHTTP2_اولویت: {
    أرجع curl_msnprintf(میانگیر, blen,
                          "FRAME[PRIORITY, len=%d, flags=%d]",
                          (صحيح)قاب->hd.درازی, قاب->hd.flags);
  }
  حالة NGHTTP2_RST_جریان: {
    أرجع curl_msnprintf(میانگیر, blen,
                          "FRAME[RST_STREAM, len=%d, flags=%d, error=%u]",
                          (صحيح)قاب->hd.درازی, قاب->hd.flags,
                          قاب->rst_جریان.error_کد);
  }
  حالة NGHTTP2_SETTINGS: {
    إذا(قاب->hd.flags & NGHTTP2_FLAG_ACK) {
      أرجع curl_msnprintf(میانگیر, blen, "FRAME[SETTINGS, ack=1]");
    }
    أرجع curl_msnprintf(میانگیر, blen,
                          "FRAME[SETTINGS, len=%d]", (صحيح)قاب->hd.درازی);
  }
  حالة NGHTTP2_افزودن_PROMISE:
    أرجع curl_msnprintf(میانگیر, blen,
                          "FRAME[PUSH_PROMISE, len=%d, hend=%d]",
                          (صحيح)قاب->hd.درازی,
                          !!(قاب->hd.flags & NGHTTP2_FLAG_انتها_سرآیندها));
  حالة NGHTTP2_PING:
    أرجع curl_msnprintf(میانگیر, blen,
                          "FRAME[PING, len=%d, ack=%d]",
                          (صحيح)قاب->hd.درازی,
                          قاب->hd.flags & NGHTTP2_FLAG_ACK);
  حالة NGHTTP2_GOAWAY: {
    حرف scratch[128];
    size_t s_len = CURL_ARRAYSIZE(scratch);
    size_t len = (قاب->goaway.opaque_داده_len < s_len) ?
      قاب->goaway.opaque_داده_len : s_len - 1;
    إذا(len)
      کپی_حافظه(scratch, قاب->goaway.opaque_داده, len);
    scratch[len] = '\0';
    أرجع curl_msnprintf(میانگیر, blen,
                          "FRAME[GOAWAY, error=%u, reason='%s', "
                          "last_stream=%d]", قاب->goaway.error_کد,
                          scratch, قاب->goaway.آخرین_جریان_id);
  }
  حالة NGHTTP2_پنجره_UPDATE: {
    أرجع curl_msnprintf(میانگیر, blen,
                          "FRAME[WINDOW_UPDATE, incr=%d]",
                          قاب->پنجره_update.پنجره_اندازه_increment);
  }
  افتراضي:
    أرجع curl_msnprintf(میانگیر, blen, "FRAME[%d, len=%d, flags=%d]",
                          قاب->hd.type, (صحيح)قاب->hd.درازی,
                          قاب->hd.flags);
  }
}

ساكن صحيح on_قاب_send(nghttp2_session *session, ثابت nghttp2_قاب *قاب,
                         فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية Curl_easy *داده = CF_داده_CURRENT(cf);

  (فراغ)session;
  DEBUGASSERT(داده);
  إذا(Curl_trc_cf_is_verbose(cf, داده)) {
    حرف میانگیر[256];
    صحيح len;
    len = Curl_nghttp2_fr_print(قاب, میانگیر, حجم(میانگیر) - 1);
    میانگیر[len] = 0;
    CURL_TRC_CF(داده, cf, "[%d] -> %s", قاب->hd.جریان_id, میانگیر);
  }
  إذا((قاب->hd.type == NGHTTP2_GOAWAY) && !ctx->فرستاده_goaway) {
    /* A GOAWAY not initiated by us, but by nghttp2 itself on detecting
     * a protocol error on the connection */
    failf(داده, "nghttp2 shuts down connection with error %u: %s",
          قاب->goaway.error_کد,
          nghttp2_http2_strerror(قاب->goaway.error_کد));
  }
  أرجع 0;
}
#پایان_اگر /* CURLVERBOSE */

ساكن صحيح on_قاب_recv(nghttp2_session *session, ثابت nghttp2_قاب *قاب,
                         فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية Curl_easy *داده = CF_داده_CURRENT(cf), *داده_s;
  بنية h2_جریان_ctx *جریان;
  int32_t جریان_id = قاب->hd.جریان_id;

  (فراغ)session;
  DEBUGASSERT(داده);
#اگر_تعریف_شده CURLVERBOSE
  إذا(Curl_trc_cf_is_verbose(cf, داده)) {
    حرف میانگیر[256];
    صحيح len;
    len = Curl_nghttp2_fr_print(قاب, میانگیر, حجم(میانگیر) - 1);
    میانگیر[len] = 0;
    CURL_TRC_CF(داده, cf, "[%d] <- %s", قاب->hd.جریان_id, میانگیر);
  }
#پایان_اگر /* CURLVERBOSE */

  إذا(!جریان_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(داده);
    بدل(قاب->hd.type) {
    حالة NGHTTP2_SETTINGS: {
      إذا(!(قاب->hd.flags & NGHTTP2_FLAG_ACK)) {
        uint32_t max_conn = ctx->max_concurrent_streams;
        ctx->max_concurrent_streams =
          Curl_ngh2_session_get_remote_settings(
            &ctx->ngh2, NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS);
        ctx->enable_افزودن = Curl_ngh2_session_get_remote_settings(
          &ctx->ngh2, NGHTTP2_SETTINGS_ENABLE_افزودن) != 0;
        CURL_TRC_CF(داده, cf, "[0] MAX_CONCURRENT_STREAMS: %u",
                    ctx->max_concurrent_streams);
        CURL_TRC_CF(داده, cf, "[0] ENABLE_PUSH: %s",
                    ctx->enable_افزودن ? "TRUE" : "false");
        إذا(داده && max_conn != ctx->max_concurrent_streams) {
          /* only signal change if the value actually changed */
          CURL_TRC_CF(داده, cf, "[0] notify MAX_CONCURRENT_STREAMS: %u",
                      ctx->max_concurrent_streams);
          Curl_multi_connchanged(داده->multi);
        }
        /* Since the initial stream window is 64K, a request might be on HOLD,
         * due to exhaustion. The (initial) SETTINGS may announce a much larger
         * window and *assume* that we treat this like a WINDOW_UPDATE. Some
         * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
         * To be safe, we UNHOLD a stream in order not to stall. */
        إذا(CURL_REQ_WANT_SEND(داده))
          Curl_multi_mark_dirty(داده);
      }
      اكسر;
    }
    حالة NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->goaway_error = قاب->goaway.error_کد;
      ctx->remote_max_sid = قاب->goaway.آخرین_جریان_id;
      إذا(داده) {
        infof(داده, "received GOAWAY, error=%u, last_stream=%d",
              ctx->goaway_error, ctx->remote_max_sid);
        Curl_multi_connchanged(داده->multi);
      }
      اكسر;
    افتراضي:
      اكسر;
    }
    أرجع 0;
  }

  داده_s = cf_h2_get_جریان_xfer(cf, جریان_id, &جریان);
  إذا(!داده_s) {
    CURL_TRC_CF(داده, cf, "[%d] No Curl_easy associated", جریان_id);
    أرجع 0;
  }

  أرجع on_جریان_قاب(cf, داده_s, جریان, قاب) ?
         NGHTTP2_ERR_CALLBACK_FAILURE : 0;
}

ساكن صحيح cf_h2_on_invalid_قاب_recv(nghttp2_session *session,
                                       ثابت nghttp2_قاب *قاب,
                                       صحيح ngerr, فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية Curl_easy *داده;
  بنية h2_جریان_ctx *جریان;
  int32_t جریان_id = قاب->hd.جریان_id;

  (فراغ)session;
  داده = cf_h2_get_جریان_xfer(cf, جریان_id, &جریان);
  إذا(داده) {
#اگر_تعریف_شده CURLVERBOSE
    حرف میانگیر[256];
    صحيح len;
    len = Curl_nghttp2_fr_print(قاب, میانگیر, حجم(میانگیر) - 1);
    میانگیر[len] = 0;
    failf(داده, "[HTTP2] [%d] received invalid frame: %s, error %d: %s",
          جریان_id, میانگیر, ngerr, nghttp2_strerror(ngerr));
#پایان_اگر /* CURLVERBOSE */
    إذا(جریان) {
      Curl_ngh2_submit_rst_جریان(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, جریان->id, NGHTTP2_جریان_CLOSED);
      جریان->error = ngerr;
      جریان->closed = TRUE;
      جریان->reset = TRUE;
    }
  }
  /* Return no error, nghttp2 will RST/GOAWAY by itself when needed */
  أرجع 0;
}

ساكن صحيح on_داده_chunk_recv(nghttp2_session *session, uint8_t flags,
                              int32_t جریان_id,
                              ثابت uint8_t *mem, size_t len, فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_جریان_ctx *جریان;
  بنية Curl_easy *داده_s;

  (فراغ)session;
  (فراغ)flags;

  DEBUGASSERT(جریان_id); /* should never be a zero stream ID here */
  DEBUGASSERT(CF_داده_CURRENT(cf));

  /* get the stream from the hash based on Stream ID */
  داده_s = cf_h2_get_جریان_xfer(cf, جریان_id, &جریان);
  إذا(!داده_s) {
    /* Receiving a Stream ID not in the hash should not happen - unless
       we have aborted a transfer artificially and there were more data
       in the pipeline. Silently ignore. */
    CURL_TRC_CF(CF_داده_CURRENT(cf), cf, "[%d] Data for unknown", جریان_id);
    /* consumed explicitly as no one will read it */
    Curl_ngh2_session_consume(&ctx->ngh2, جریان_id, len);
    أرجع 0;
  }

  إذا(!جریان)
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;

  h2_xfer_write_resp(cf, داده_s, جریان, (ثابت حرف *)mem, len, FALSE);

  Curl_ngh2_session_consume(&ctx->ngh2, جریان_id, len);
  جریان->nrcvd_داده += (curl_off_t)len;
  أرجع 0;
}

ساكن صحيح on_جریان_close(nghttp2_session *session, int32_t جریان_id,
                           uint32_t error_کد, فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية Curl_easy *داده_s, *call_داده = CF_داده_CURRENT(cf);
  بنية h2_جریان_ctx *جریان = عدم;
  صحيح rv;
  (فراغ)session;

  DEBUGASSERT(call_داده);
  /* stream id 0 is the connection, do not look there for streams. */
  داده_s = جریان_id ? cf_h2_get_جریان_xfer(cf, جریان_id, &جریان) : عدم;
  إذا(!داده_s) {
    CURL_TRC_CF(call_داده, cf,
                "[%d] on_stream_close, no easy set on stream", جریان_id);
    أرجع 0;
  }
  إذا(!GOOD_EASY_HANDLE(داده_s)) {
    /* nghttp2 still has an easy registered for the stream which has
     * been freed be libcurl. This points to a code path that does not
     * trigger DONE or DETACH events as it must. */
    CURL_TRC_CF(call_داده, cf,
                "[%d] on_stream_close, not a GOOD easy on stream", جریان_id);
    (فراغ)Curl_ngh2_session_مجموعه_جریان_user_داده(
      &ctx->ngh2, جریان_id, عدم);
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  إذا(!جریان) {
    CURL_TRC_CF(داده_s, cf,
                "[%d] on_stream_close, GOOD easy but no stream", جریان_id);
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  جریان->closed = TRUE;
  جریان->error = error_کد;
  إذا(جریان->error) {
    جریان->reset = TRUE;
    إذا((جریان->error == NGHTTP2_REFUSED_جریان) &&
       ctx->rcvd_goaway && ctx->goaway_error)
    /* REFUSED_STREAM is used by nghttp2 on a GOAWAY from the
     * server where it indicated it will no longer process this stream.
     * If the GOAWAY carried an error code, take that as the error for
     * the stream. */
    جریان->error = ctx->goaway_error;
  }

  إذا(جریان->error)
    CURL_TRC_CF(داده_s, cf, "[%d] RESET: %s (err %u)",
              جریان_id, nghttp2_http2_strerror(error_کد), error_کد);
  وإلا
    CURL_TRC_CF(داده_s, cf, "[%d] CLOSED", جریان_id);
  Curl_multi_mark_dirty(داده_s);

  /* remove `stream_ctx` from the nghttp2 stream */
  rv = Curl_ngh2_session_مجموعه_جریان_user_داده(
    &ctx->ngh2, جریان_id, عدم);
  إذا(rv) {
    infof(داده_s, "http/2: failed to clear user_data for stream %d",
          جریان_id);
    DEBUGASSERT(0);
  }
  أرجع 0;
}

ساكن فراغ cf_h2_سرآیند_error(بنية Curl_cfilter *cf,
                               بنية Curl_easy *داده,
                               بنية h2_جریان_ctx *جریان,
                               CURLcode نتیجه)
{
  بنية cf_h2_ctx *ctx = cf->ctx;

  failf(داده, "Error receiving HTTP2 header: %d(%s)", (صحيح)نتیجه,
        curl_easy_strerror(نتیجه));
  إذا(جریان) {
    Curl_ngh2_submit_rst_جریان(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, جریان->id, NGHTTP2_جریان_CLOSED);
    جریان->closed = TRUE;
    جریان->reset = TRUE;
  }
}

/* frame->hd.type is either NGHTTP2_HEADERS or NGHTTP2_PUSH_PROMISE */
ساكن صحيح on_سرآیند(nghttp2_session *session, ثابت nghttp2_قاب *قاب,
                     ثابت uint8_t *name, size_t namelen,
                     ثابت uint8_t *مقدار, size_t valuelen,
                     uint8_t flags,
                     فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_جریان_ctx *جریان;
  بنية Curl_easy *داده;
  int32_t جریان_id = قاب->hd.جریان_id;
  CURLcode نتیجه;

  (فراغ)session;
  (فراغ)flags;

  DEBUGASSERT(جریان_id); /* should never be a zero stream ID here */

  /* get the stream from the hash based on Stream ID */
  داده = cf_h2_get_جریان_xfer(cf, جریان_id, &جریان);
  إذا(!GOOD_EASY_HANDLE(داده))
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;

  إذا(!جریان) {
    failf(داده, "Internal NULL stream");
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  /* Store received PUSH_PROMISE headers to be used when the subsequent
     PUSH_PROMISE callback comes */
  إذا(قاب->hd.type == NGHTTP2_افزودن_PROMISE) {
    حرف *h;

    إذا((namelen == CURL_CSTRLEN(HTTP_PSEUDO_AUTHORITY)) &&
       !مقایسه_رشته(HTTP_PSEUDO_AUTHORITY, (ثابت حرف *)name, namelen)) {
      /* pseudo headers are lower case */
      صحيح rc = 0;
      حرف *check = curl_maprintf("%s:%d", داده->state.origin->hostname,
                                  داده->state.origin->پورت);
      إذا(!check)
        /* no memory */
        أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
      إذا(!curl_strequal(check, (ثابت حرف *)مقدار) &&
         ((داده->state.origin->پورت != داده->state.origin->scheme->defport) ||
          !curl_strequal(داده->state.origin->hostname, (ثابت حرف *)مقدار))) {
        /* This is push is not for the same authority that was asked for in
         * the URL. RFC 7540 section 8.2 says: "A client MUST treat a
         * PUSH_PROMISE for which the server is not authoritative as a stream
         * error of type PROTOCOL_ERROR."
         */
        (فراغ)Curl_ngh2_submit_rst_جریان(
          &ctx->ngh2, NGHTTP2_FLAG_NONE, جریان_id, NGHTTP2_PROTOCOL_ERROR);
        rc = NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      curlx_free(check);
      إذا(rc)
        أرجع rc;
    }

    إذا(!جریان->افزودن_سرآیندها) {
      جریان->افزودن_سرآیندها_alloc = 10;
      جریان->افزودن_سرآیندها = curlx_malloc(جریان->افزودن_سرآیندها_alloc *
                                          حجم(حرف *));
      إذا(!جریان->افزودن_سرآیندها)
        أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
      جریان->افزودن_سرآیندها_استفاده_شده = 0;
    }
    وإلا إذا(جریان->افزودن_سرآیندها_استفاده_شده ==
            جریان->افزودن_سرآیندها_alloc) {
      حرف **headp;
      إذا(جریان->افزودن_سرآیندها_alloc > 1000) {
        /* this is beyond crazy many headers, bail out */
        failf(داده, "Too many PUSH_PROMISE headers");
        free_افزودن_سرآیندها(جریان);
        أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      جریان->افزودن_سرآیندها_alloc *= 2;
      headp = curlx_realloc(جریان->افزودن_سرآیندها,
                            جریان->افزودن_سرآیندها_alloc * حجم(حرف *));
      إذا(!headp) {
        free_افزودن_سرآیندها(جریان);
        أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      جریان->افزودن_سرآیندها = headp;
    }
    h = curl_maprintf("%s:%s", name, مقدار);
    إذا(!h) {
      free_افزودن_سرآیندها(جریان);
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    جریان->افزودن_سرآیندها[جریان->افزودن_سرآیندها_استفاده_شده++] = h;
    أرجع 0;
  }

  إذا(جریان->bodystarted) {
    /* This is a trailer */
    CURL_TRC_CF(داده, cf, "[%d] trailer: %.*s: %.*s",
                جریان->id, (صحيح)namelen, name, (صحيح)valuelen, مقدار);
    نتیجه = Curl_dynhds_جمع_کن(&جریان->resp_trailers,
                             (ثابت حرف *)name, namelen,
                             (ثابت حرف *)مقدار, valuelen);
    إذا(نتیجه) {
      cf_h2_سرآیند_error(cf, داده, جریان, نتیجه);
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    }

    أرجع 0;
  }

  إذا(namelen == CURL_CSTRLEN(HTTP_PSEUDO_وضعیت) &&
     !memcmp(HTTP_PSEUDO_وضعیت, name, namelen)) {
    /* nghttp2 guarantees :status is received first and only once. */
    حرف میانگیر[32];
    size_t hlen;
    نتیجه = Curl_http_decode_وضعیت(&جریان->وضعیت_کد,
                                     (ثابت حرف *)مقدار, valuelen);
    إذا(نتیجه) {
      cf_h2_سرآیند_error(cf, داده, جریان, نتیجه);
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    hlen = curl_msnprintf(میانگیر, حجم(میانگیر), HTTP_PSEUDO_وضعیت ":%d\r",
                          جریان->وضعیت_کد);
    نتیجه = Curl_سرآیندها_افزودن(داده, میانگیر, hlen, CURLH_PSEUDO);
    إذا(نتیجه) {
      cf_h2_سرآیند_error(cf, داده, جریان, نتیجه);
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    curlx_dyn_reset(&ctx->scratch);
    نتیجه = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/2 "));
    إذا(!نتیجه)
      نتیجه = curlx_dyn_addn(&ctx->scratch, مقدار, valuelen);
    إذا(!نتیجه)
      نتیجه = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    إذا(!نتیجه)
      h2_xfer_write_resp_hd(cf, داده, جریان, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    إذا(نتیجه) {
      cf_h2_سرآیند_error(cf, داده, جریان, نتیجه);
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    /* if we receive data for another handle, wake that up */
    إذا(CF_داده_CURRENT(cf) != داده)
      Curl_multi_mark_dirty(داده);

    CURL_TRC_CF(داده, cf, "[%d] status: HTTP/2 %03d",
                جریان->id, جریان->وضعیت_کد);
    أرجع 0;
  }

  /* nghttp2 guarantees that namelen > 0, and :status was already
     received, and this is not pseudo-header field . */
  /* convert to an HTTP1-style header */
  curlx_dyn_reset(&ctx->scratch);
  نتیجه = curlx_dyn_addn(&ctx->scratch, (ثابت حرف *)name, namelen);
  إذا(!نتیجه)
    نتیجه = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
  إذا(!نتیجه)
    نتیجه = curlx_dyn_addn(&ctx->scratch, (ثابت حرف *)مقدار, valuelen);
  إذا(!نتیجه)
    نتیجه = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
  إذا(!نتیجه)
    h2_xfer_write_resp_hd(cf, داده, جریان, curlx_dyn_ptr(&ctx->scratch),
                          curlx_dyn_len(&ctx->scratch), FALSE);
  إذا(نتیجه) {
    cf_h2_سرآیند_error(cf, داده, جریان, نتیجه);
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  /* if we receive data for another handle, wake that up */
  إذا(CF_داده_CURRENT(cf) != داده)
    Curl_multi_mark_dirty(داده);

  CURL_TRC_CF(داده, cf, "[%d] header: %.*s: %.*s",
              جریان->id, (صحيح)namelen, name, (صحيح)valuelen, مقدار);

  أرجع 0; /* 0 is successful */
}

ساكن ssize_t req_body_read_callback(nghttp2_session *session,
                                      int32_t جریان_id,
                                      uint8_t *buf, size_t درازی,
                                      uint32_t *داده_flags,
                                      nghttp2_داده_منبع *منبع,
                                      فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية Curl_easy *داده_s;
  بنية h2_جریان_ctx *جریان = عدم;
  CURLcode نتیجه;
  ssize_t nread;
  size_t n;
  (فراغ)منبع;

  (فراغ)session;
  إذا(!جریان_id)
    أرجع NGHTTP2_ERR_INVALID_آرگومان;

  /* get the stream from the hash based on Stream ID, stream ID zero is for
     connection-oriented stuff */
  داده_s = cf_h2_get_جریان_xfer(cf, جریان_id, &جریان);
  إذا(!داده_s || !جریان)
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    أرجع NGHTTP2_ERR_CALLBACK_FAILURE;

  نتیجه = Curl_bufq_read(&جریان->sendbuf, buf, درازی, &n);
  إذا(نتیجه) {
    إذا(نتیجه != CURLE_AGAIN)
      أرجع NGHTTP2_ERR_CALLBACK_FAILURE;
    nread = 0;
  }
  وإلا
    nread = (ssize_t)n;

  CURL_TRC_CF(داده_s, cf, "[%d] req_body_read(len=%zu) eos=%d -> %zd, %d",
              جریان_id, درازی, جریان->body_eos, nread, (صحيح)نتیجه);

  إذا(جریان->body_eos && Curl_bufq_is_خالی(&جریان->sendbuf)) {
    *داده_flags = NGHTTP2_داده_FLAG_EOF;
    أرجع nread;
  }
  أرجع (nread == 0) ? NGHTTP2_ERR_DEFERRED : nread;
}

#اگر_تعریف_شده CURLVERBOSE
ساكن صحيح error_callback(nghttp2_session *session,
                          ثابت حرف *پیام,
                          size_t len,
                          فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية Curl_easy *داده = CF_داده_CURRENT(cf);
  (فراغ)session;
  failf(داده, "%.*s", (صحيح)len, پیام);
  أرجع 0;
}
#پایان_اگر

/*
 * Append headers to ask for an HTTP1.1 to HTTP2 upgrade.
 */
CURLcode Curl_http2_request_upgrade(بنية dynbuf *req,
                                    بنية Curl_easy *داده)
{
  CURLcode نتیجه;
  حرف *base64;
  size_t blen;
  بنية SingleRequest *k = &داده->req;
  uint8_t binsettings[H2_BINSETTINGS_LEN];
  ssize_t rc;
  size_t binlen; /* length of the binsettings data */

  rc = populate_binsettings(binsettings, داده);
  إذا(!curlx_sztouz(rc, &binlen) || !binlen) {
    failf(داده, "nghttp2 unexpectedly failed on pack_settings_payload");
    curlx_dyn_free(req);
    أرجع CURLE_FAILED_INIT;
  }

  نتیجه = curlx_base64url_encode(binsettings, binlen, &base64, &blen);
  إذا(نتیجه) {
    curlx_dyn_free(req);
    أرجع نتیجه;
  }

  داده->state.http_hd_upgrade = TRUE;
  داده->state.http_hd_h2_settings = TRUE;
  نتیجه = curlx_dyn_addf(req,
                          "Upgrade: %s\r\n"
                          "HTTP2-Settings: %s\r\n",
                          NGHTTP2_CLEARTEXT_PROTO_نسخه_ID, base64);
  curlx_free(base64);

  k->upgr101 = UPGR101_H2;
  داده->conn->بیتها.upgrade_درون_پیشرفت = TRUE;

  أرجع نتیجه;
}

ساكن CURLcode http2_handle_جریان_close(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *داده,
                                          بنية h2_جریان_ctx *جریان,
                                          size_t *pnlen)
{
  CURLcode نتیجه;

  *pnlen = 0;
  إذا(جریان->reset) {
    إذا(جریان->error == NGHTTP2_REFUSED_جریان) {
      infof(داده, "HTTP/2 stream %d refused by server, try again on a new "
                  "connection", جریان->id);
      connclose(cf->conn); /* do not use this anymore */
      داده->state.refused_جریان = TRUE;
      أرجع CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    وإلا إذا(جریان->resp_hds_complete && داده->req.no_body) {
      CURL_TRC_CF(داده, cf, "[%d] error after response headers, but we did "
                  "not want a body anyway, ignore: %s (err %u)",
                  جریان->id, nghttp2_http2_strerror(جریان->error),
                  جریان->error);
      جریان->close_handled = TRUE;
      أرجع CURLE_OK;
    }
    failf(داده, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          جریان->id, جریان->reset_by_server ? "server" : "curl",
          جریان->error, nghttp2_http2_strerror(جریان->error));
    أرجع جریان->error ? CURLE_HTTP2_جریان :
           (داده->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2);
  }
  وإلا إذا(!جریان->bodystarted) {
    failf(داده, "HTTP/2 stream %d was closed cleanly, but before getting "
          "all response header fields, treated as error", جریان->id);
    أرجع CURLE_HTTP2_جریان;
  }

  إذا(Curl_dynhds_شمار(&جریان->resp_trailers)) {
    بنية dynhds_مدخل *e;
    بنية dynbuf dbuf;
    size_t i;

    نتیجه = CURLE_OK;
    curlx_dyn_init(&dbuf, DYN_TRAILERS);
    لكل(i = 0; i < Curl_dynhds_شمار(&جریان->resp_trailers); ++i) {
      e = Curl_dynhds_getn(&جریان->resp_trailers, i);
      إذا(!e)
        اكسر;
      curlx_dyn_reset(&dbuf);
      نتیجه = curlx_dyn_addf(&dbuf, "%.*s: %.*s\x0d\x0a",
                              (صحيح)e->namelen, e->name,
                              (صحيح)e->valuelen, e->مقدار);
      إذا(نتیجه)
        اكسر;
      Curl_debug(داده, CURLINFO_سرآیند_درون, curlx_dyn_ptr(&dbuf),
                 curlx_dyn_len(&dbuf));
      نتیجه = Curl_client_write(داده,
                                 CLIENTWRITE_سرآیند | CLIENTWRITE_TRAILER,
                                 curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf));
      إذا(نتیجه)
        اكسر;
      نتیجه = Curl_bump_headersize(داده, curlx_dyn_len(&dbuf), FALSE);
      إذا(نتیجه)
        أرجع نتیجه;
    }
    curlx_dyn_free(&dbuf);
    إذا(نتیجه)
      اذهب بیرون;
  }

  جریان->close_handled = TRUE;
  نتیجه = CURLE_OK;

بیرون:
  CURL_TRC_CF(داده, cf, "handle_stream_close -> %d, %zu", (صحيح)نتیجه, *pnlen);
  أرجع نتیجه;
}

ساكن صحيح sweight_wanted(ثابت بنية Curl_easy *داده)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  أرجع داده->مجموعه.وزن ?
    داده->مجموعه.وزن : NGHTTP2_DEFAULT_وزن;
}

ساكن صحيح sweight_درون_effect(ثابت بنية Curl_easy *داده)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  أرجع داده->state.وزن ?
    داده->state.وزن : NGHTTP2_DEFAULT_وزن;
}

/*
 * h2_pri_spec() fills in the pri_spec struct, used by nghttp2 to send weight
 * and dependency to the peer. It also stores the updated values in the state
 * struct.
 */

ساكن فراغ h2_pri_spec(بنية Curl_easy *داده,
                        nghttp2_اولویت_spec *pri_spec)
{
  صحيح prio = داده->مجموعه.وزن;
  nghttp2_اولویت_spec_init(pri_spec, 0, sweight_wanted(داده), FALSE);
  داده->state.وزن = prio;
}

/*
 * Check if there is been an update in the priority /
 * dependency settings and if so it submits a PRIORITY frame with the updated
 * info.
 * Flush any out data pending in the network buffer.
 */
ساكن CURLcode h2_پیشرفت_egress(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  صحيح rv = 0;

  إذا(sweight_wanted(داده) != sweight_درون_effect(داده)) {
    بنية h2_جریان_ctx *جریان = H2_جریان_CTX(ctx, داده);
    إذا(جریان && جریان->id > 0 &&
       (sweight_wanted(داده) != sweight_درون_effect(داده))) {
      /* send new weight and/or dependency */
      nghttp2_اولویت_spec pri_spec;

      h2_pri_spec(داده, &pri_spec);
      CURL_TRC_CF(داده, cf, "[%d] Queuing PRIORITY", جریان->id);
      DEBUGASSERT(جریان->id != -1);
      rv = Curl_ngh2_submit_اولویت(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, جریان->id, &pri_spec);
      إذا(rv)
        اذهب بیرون;
    }
  }

  ctx->nw_بیرون_blocked = 0;
  طالما(!rv && !ctx->nw_بیرون_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

بیرون:
  إذا(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(داده, cf, "nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    أرجع CURLE_SEND_ERROR;
  }
  /* Defer flushing during the connect phase so that the SETTINGS and
   * other initial frames are sent together with the first request.
   * Unless we are 'connect_only' where the request will never come. */
  إذا(!cf->connected && !cf->conn->بیتها.connect_only)
    أرجع CURLE_OK;
  أرجع nw_بیرون_flush(cf, داده);
}

ساكن CURLcode جریان_recv(بنية Curl_cfilter *cf, بنية Curl_easy *داده,
                            بنية h2_جریان_ctx *جریان,
                            حرف *buf, size_t len, size_t *pnread)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_AGAIN;

  (فراغ)buf;
  (فراغ)len;
  *pnread = 0;

  إذا(!جریان->xfer_نتیجه)
    جریان->xfer_نتیجه = cf_h2_update_local_win(cf, داده, جریان);

  إذا(جریان->xfer_نتیجه) {
    CURL_TRC_CF(داده, cf, "[%d] xfer write failed", جریان->id);
    نتیجه = جریان->xfer_نتیجه;
  }
  وإلا إذا(جریان->closed) {
    CURL_TRC_CF(داده, cf, "[%d] returning CLOSE", جریان->id);
    نتیجه = http2_handle_جریان_close(cf, داده, جریان, pnread);
  }
  وإلا إذا(جریان->reset ||
          (ctx->conn_closed && Curl_bufq_is_خالی(&ctx->inbufq)) ||
          (ctx->rcvd_goaway && ctx->remote_max_sid < جریان->id)) {
    CURL_TRC_CF(داده, cf, "[%d] returning ERR", جریان->id);
    نتیجه = داده->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2;
  }

  إذا(نتیجه && (نتیجه != CURLE_AGAIN))
    CURL_TRC_CF(داده, cf, "[%d] stream_recv(len=%zu) -> %d, %zu",
                جریان->id, len, (صحيح)نتیجه, *pnread);
  أرجع نتیجه;
}

ساكن CURLcode h2_پیشرفت_ingress(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *داده,
                                    size_t داده_max_بایتها)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;
  size_t nread;

  إذا(should_close_session(ctx)) {
    CURL_TRC_CF(داده, cf, "[0] ingress: session is closed");
    أرجع CURLE_HTTP2;
  }

  /* Process network input buffer first */
  إذا(!Curl_bufq_is_خالی(&ctx->inbufq)) {
    CURL_TRC_CF(داده, cf, "Process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    نتیجه = h2_process_pending_درون_داد(cf, داده);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  إذا(!داده_max_بایتها)
    داده_max_بایتها = H2_CHUNK_اندازه;

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop due to connection close or us not processing
   * all network input */
  طالما(!ctx->conn_closed && Curl_bufq_is_خالی(&ctx->inbufq)) {
    بنية h2_جریان_ctx *جریان = H2_جریان_CTX(ctx, داده);
    إذا(جریان && (جریان->closed || !داده_max_بایتها)) {
      /* We would like to abort here and stop processing, so that the transfer
       * loop can handle the data/close here. This may leave data in
       * underlying buffers that will not be consumed. */
      إذا(!cf->بعدی || !cf->بعدی->cft->has_داده_pending(cf->بعدی, داده))
        Curl_multi_mark_dirty(داده);
      اكسر;
    }

    نتیجه = Curl_cf_recv_bufq(cf->بعدی, داده, &ctx->inbufq, 0, &nread);
    إذا(نتیجه) {
      إذا(نتیجه != CURLE_AGAIN) {
        failf(داده, "Failed receiving HTTP2 data: %d(%s)", (صحيح)نتیجه,
              curl_easy_strerror(نتیجه));
        أرجع نتیجه;
      }
      اكسر;
    }
    وإلا إذا(nread == 0) {
      CURL_TRC_CF(داده, cf, "[0] ingress: connection closed");
      ctx->conn_closed = TRUE;
      اكسر;
    }
    وإلا {
      CURL_TRC_CF(داده, cf, "[0] ingress: read %zu bytes", nread);
      داده_max_بایتها = (داده_max_بایتها > nread) ? (داده_max_بایتها - nread) : 0;
    }

    نتیجه = h2_process_pending_درون_داد(cf, داده);
    إذا(نتیجه)
      أرجع نتیجه;
    CURL_TRC_CF(داده, cf, "[0] ingress: nw-in buffered %zu",
                Curl_bufq_len(&ctx->inbufq));
  }

  إذا(ctx->conn_closed && Curl_bufq_is_خالی(&ctx->inbufq)) {
    CURL_TRC_CF(داده, cf, "server closed %s",
                ctx->rcvd_goaway ? "with GOAWAY" : "abruptly");
    connclose(cf->conn);
  }

  CURL_TRC_CF(داده, cf, "[0] ingress: done");
  أرجع CURLE_OK;
}

ساكن CURLcode cf_h2_recv(بنية Curl_cfilter *cf, بنية Curl_easy *داده,
                           حرف *buf, size_t len, size_t *pnread)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_جریان_ctx *جریان;
  CURLcode نتیجه, r2;
  بنية cf_call_داده save;

  إذا(!داده)
    أرجع CURLE_HTTP2;

  جریان = H2_جریان_CTX(ctx, داده);

  *pnread = 0;
  إذا(!جریان) {
    /* Abnormal call sequence: either this transfer has never opened a stream
     * (unlikely) or the transfer has been done, cleaned up its resources, but
     * a read() is called anyway. It is not clear what the calling sequence
     * is for such a case. */
    failf(داده, "http/2 recv on a transfer never opened "
          "or already cleared, mid=%u", داده->mid);
    أرجع CURLE_HTTP2;
  }

  CF_داده_SAVE(save, cf, داده);

  نتیجه = جریان_recv(cf, داده, جریان, buf, len, pnread);
  إذا(نتیجه && (نتیجه != CURLE_AGAIN))
    اذهب بیرون;

  إذا(نتیجه) {
    نتیجه = h2_پیشرفت_ingress(cf, داده, len);
    إذا(نتیجه)
      اذهب بیرون;

    نتیجه = جریان_recv(cf, داده, جریان, buf, len, pnread);
  }

  إذا(*pnread > 0) {
    /* Now that we transferred this to the upper layer, we report
     * the actual amount of DATA consumed to the H2 session, so
     * that it adjusts stream flow control */
    Curl_ngh2_session_consume(&ctx->ngh2, جریان->id, *pnread);
    إذا(جریان->closed) {
      CURL_TRC_CF(داده, cf, "[%d] DRAIN closed stream", جریان->id);
      Curl_multi_mark_dirty(داده);
    }
  }

بیرون:
  r2 = h2_پیشرفت_egress(cf, داده);
  إذا(r2 == CURLE_AGAIN) {
    /* pending data to send, need to be called again. Ideally, we
     * monitor the socket for POLLOUT, but when not SENDING
     * any more, we force processing of the transfer. */
    إذا(!CURL_REQ_WANT_SEND(داده))
      Curl_multi_mark_dirty(داده);
  }
  وإلا إذا(r2 && (نتیجه == CURLE_AGAIN)) {
    /* Do not replace a previous error result NOR a successful result
     * with an egress error. Example: the server might close the
     * connection after sending the complete response with EOS
     * and everything. Further sending may fail, but that MUST
     * not change that we had a successful response. */
    نتیجه = r2;
  }

  CURL_TRC_CF(داده, cf, "[%d] cf_recv(len=%zu) -> %d, %zu, "
              "window=%d/%d, connection %d/%d",
              جریان->id, len, (صحيح)نتیجه, *pnread,
              Curl_ngh2_session_get_جریان_effective_recv_داده_درازی(
                &ctx->ngh2, جریان->id),
              Curl_ngh2_session_get_جریان_effective_local_پنجره_اندازه(
                &ctx->ngh2, جریان->id),
              Curl_ngh2_session_get_local_پنجره_اندازه(&ctx->ngh2),
              HTTP2_HUGE_پنجره_اندازه);

  CF_داده_RESTORE(cf, save);
  أرجع نتیجه;
}

ساكن CURLcode cf_h2_body_send(بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده,
                                بنية h2_جریان_ctx *جریان,
                                ثابت فراغ *buf, size_t blen, bool eos,
                                size_t *pnwritten)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  CURLcode نتیجه;

  *pnwritten = 0;
  إذا(جریان->closed) {
    إذا(جریان->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(داده, cf, "[%d] discarding data"
                  "on closed stream with response", جریان->id);
      إذا(eos)
        جریان->body_eos = TRUE;
      *pnwritten = blen;
      أرجع CURLE_OK;
    }
    /* Server closed before we got a response, this is an error */
    infof(داده, "stream %d closed", جریان->id);
    أرجع CURLE_SEND_ERROR;
  }

  نتیجه = Curl_bufq_write(&جریان->sendbuf, buf, blen, pnwritten);
  إذا(نتیجه)
    أرجع نتیجه;

  إذا(eos && (blen == *pnwritten))
    جریان->body_eos = TRUE;

  إذا(eos || !Curl_bufq_is_خالی(&جریان->sendbuf)) {
    /* resume the potentially suspended stream */
    إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_داده(&ctx->ngh2, جریان->id)))
      أرجع CURLE_SEND_ERROR;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode h2_submit(بنية h2_جریان_ctx **pstream,
                          بنية Curl_cfilter *cf, بنية Curl_easy *داده,
                          ثابت uint8_t *buf, size_t len,
                          bool eos, size_t *pnwritten)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_جریان_ctx *جریان = عدم;
  بنية dynhds h2_سرآیندها;
  nghttp2_nv *nva = عدم;
  ثابت فراغ *body = عدم;
  size_t nheader, bodylen;
  nghttp2_داده_provider داده_prd;
  int32_t جریان_id;
  nghttp2_اولویت_spec pri_spec;
  size_t nwritten;
  CURLcode نتیجه = CURLE_OK;
  uint32_t initial_win_اندازه;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_سرآیندها, MAX_HTTP_RESP_سرآیند_شمار, DYN_HTTP_REQUEST);

  نتیجه = http2_داده_راهاندازی(cf, داده, &جریان);
  إذا(نتیجه)
    اذهب بیرون;

  نتیجه = Curl_h1_req_parse_read(
    &جریان->h1, buf, len, عدم,
    !داده->state.http_ignorecustom ?
    CURL_EASY_STR(داده, STRING_CUSTOMREQUEST) : عدم,
    0, &nwritten);
  إذا(نتیجه)
    اذهب بیرون;
  *pnwritten = nwritten;
  إذا(!جریان->h1.انجامشده) {
    /* need more data */
    اذهب بیرون;
  }
  DEBUGASSERT(جریان->h1.req);

  نتیجه = Curl_http_req_مقصد_h2(&h2_سرآیندها, جریان->h1.req, داده);
  إذا(نتیجه)
    اذهب بیرون;
  /* no longer needed */
  Curl_h1_req_parse_free(&جریان->h1);

  nva = Curl_dynhds_مقصد_nva(&h2_سرآیندها, &nheader);
  إذا(!nva) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  h2_pri_spec(داده, &pri_spec);
  إذا(!Curl_ngh2_session_check_request_allowed(&ctx->ngh2))
    CURL_TRC_CF(داده, cf, "send request NOT allowed (via nghttp2)");

  /* Check the initial windows size of the transfer (rate-limits?) and
   * send an updated settings on changes from previous value. */
  initial_win_اندازه = cf_h2_initial_win_اندازه(داده);
  إذا(initial_win_اندازه != ctx->initial_win_اندازه) {
    نتیجه = cf_h2_update_settings(ctx, initial_win_اندازه);
    إذا(نتیجه)
      اذهب بیرون;
  }

  بدل(داده->state.httpreq) {
  حالة HTTPREQ_POST:
  حالة HTTPREQ_POST_FORM:
  حالة HTTPREQ_POST_MIME:
  حالة HTTPREQ_PUT:
    داده_prd.read_callback = req_body_read_callback;
    داده_prd.منبع.ptr = عدم;
    جریان_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, &داده_prd, جریان);
    اكسر;
  افتراضي:
    جریان_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, عدم, جریان);
  }

  إذا(جریان_id < 0) {
    CURL_TRC_CF(داده, cf, "send: nghttp2_submit_request error (%s)%d",
                nghttp2_strerror(جریان_id), جریان_id);
    نتیجه = CURLE_SEND_ERROR;
    اذهب بیرون;
  }

#اگر_تعریف_شده CURLVERBOSE
#تعریف_کن MAX_ACC 60000  /* <64KB to account for some overhead */
  إذا(Curl_trc_is_verbose(داده)) {
    size_t acc = 0, i;

    infof(داده, "[HTTP/2] [%d] OPENED stream for %s",
          جریان_id, Curl_bufref_ptr(&داده->state.url));
    لكل(i = 0; i < nheader; ++i) {
      acc += nva[i].namelen + nva[i].valuelen;

      infof(داده, "[HTTP/2] [%d] [%.*s: %.*s]", جریان_id,
            (صحيح)nva[i].namelen, nva[i].name,
            (صحيح)nva[i].valuelen, nva[i].مقدار);
    }

    إذا(acc > MAX_ACC) {
      infof(داده, "[HTTP/2] Warning: The cumulative length of all "
            "headers exceeds %d bytes and that could cause the "
            "stream to be rejected.", MAX_ACC);
    }
  }
#پایان_اگر

  جریان->id = جریان_id;

  body = (ثابت حرف *)buf + *pnwritten;
  bodylen = len - *pnwritten;

  إذا(bodylen || eos) {
    size_t n;
    نتیجه = cf_h2_body_send(cf, داده, جریان, body, bodylen, eos, &n);
    إذا(!نتیجه)
      *pnwritten += n;
    وإلا إذا(نتیجه == CURLE_AGAIN)
      نتیجه = CURLE_OK;
    وإلا {
      نتیجه = CURLE_SEND_ERROR;
    }
  }

بیرون:
  CURL_TRC_CF(داده, cf, "[%d] submit -> %d, %zu",
              جریان ? جریان->id : -1, (صحيح)نتیجه, *pnwritten);
  curlx_safefree(nva);
  *pstream = جریان;
  Curl_dynhds_free(&h2_سرآیندها);
  أرجع نتیجه;
}

ساكن CURLcode cf_h2_send(بنية Curl_cfilter *cf, بنية Curl_easy *داده,
                           ثابت uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_جریان_ctx *جریان = H2_جریان_CTX(ctx, داده);
  بنية cf_call_داده save;
  CURLcode نتیجه = CURLE_OK, r2;

  CF_داده_SAVE(save, cf, داده);
  *pnwritten = 0;

  إذا(!جریان || جریان->id == -1) {
    نتیجه = h2_submit(&جریان, cf, داده, buf, len, eos, pnwritten);
    إذا(نتیجه)
      اذهب بیرون;
    DEBUGASSERT(جریان);
  }
  وإلا إذا(جریان->body_eos) {
    /* We already wrote this, but CURLE_AGAIN-ed the call due to not
     * being able to flush stream->sendbuf. Make a 0-length write
     * to trigger flushing again.
     * If this works, we report to have written `len` bytes. */
    size_t n;
    DEBUGASSERT(eos);
    نتیجه = cf_h2_body_send(cf, داده, جریان, buf, 0, eos, &n);
    CURL_TRC_CF(داده, cf, "[%d] cf_body_send last CHUNK -> %d, %zu, eos=%d",
                جریان->id, (صحيح)نتیجه, n, eos);
    إذا(نتیجه)
      اذهب بیرون;
    *pnwritten = len;
  }
  وإلا {
    نتیجه = cf_h2_body_send(cf, داده, جریان, buf, len, eos, pnwritten);
    CURL_TRC_CF(داده, cf, "[%d] cf_body_send(len=%zu) -> %d, %zu, eos=%d",
                جریان->id, len, (صحيح)نتیجه, *pnwritten, eos);
  }

  /* Call the nghttp2 send loop and flush to write ALL buffered data,
   * headers and/or request body completely out to the network */
  r2 = h2_پیشرفت_egress(cf, داده);

  /* if the stream has been closed in egress handling (nghttp2 does that
   * when it does not like the headers, for example */
  إذا(جریان && جریان->closed && !جریان->resp_hds_complete) {
    infof(داده, "stream %d closed", جریان->id);
    نتیجه = CURLE_SEND_ERROR;
    اذهب بیرون;
  }
  وإلا إذا(r2 && (r2 != CURLE_AGAIN)) {
    نتیجه = r2;
    اذهب بیرون;
  }

  إذا(should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    إذا(جریان && جریان->closed) {
      نتیجه = http2_handle_جریان_close(cf, داده, جریان, pnwritten);
    }
    وإلا {
      CURL_TRC_CF(داده, cf, "send: nothing to do in this session");
      نتیجه = CURLE_HTTP2;
    }
  }

بیرون:
  إذا(جریان) {
    CURL_TRC_CF(داده, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
                "eos=%d, h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                جریان->id, len, (صحيح)نتیجه, *pnwritten,
                جریان->body_eos,
                Curl_ngh2_session_get_جریان_remote_پنجره_اندازه(
                  &ctx->ngh2, جریان->id),
                Curl_ngh2_session_get_remote_پنجره_اندازه(&ctx->ngh2),
                Curl_bufq_len(&جریان->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  وإلا {
    CURL_TRC_CF(داده, cf, "cf_send(len=%zu) -> %d, %zu, "
                "connection-window=%d, nw_send_buffer(%zu)",
                len, (صحيح)نتیجه, *pnwritten,
                Curl_ngh2_session_get_remote_پنجره_اندازه(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_داده_RESTORE(cf, save);
  أرجع نتیجه;
}

ساكن CURLcode cf_h2_flush(بنية Curl_cfilter *cf,
                            بنية Curl_easy *داده)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_جریان_ctx *جریان = H2_جریان_CTX(ctx, داده);
  بنية cf_call_داده save;
  CURLcode نتیجه = CURLE_OK;

  CF_داده_SAVE(save, cf, داده);
  إذا(جریان && !Curl_bufq_is_خالی(&جریان->sendbuf)) {
    /* resume the potentially suspended stream */
    إذا(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_داده(&ctx->ngh2, جریان->id))) {
      نتیجه = CURLE_SEND_ERROR;
      اذهب بیرون;
    }
  }

  نتیجه = h2_پیشرفت_egress(cf, داده);

بیرون:
  إذا(جریان) {
    CURL_TRC_CF(داده, cf, "[%d] flush -> %d, "
                "h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                جریان->id, (صحيح)نتیجه,
                Curl_ngh2_session_get_جریان_remote_پنجره_اندازه(
                  &ctx->ngh2, جریان->id),
                Curl_ngh2_session_get_remote_پنجره_اندازه(&ctx->ngh2),
                Curl_bufq_len(&جریان->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  وإلا {
    CURL_TRC_CF(داده, cf, "flush -> %d, "
                "connection-window=%d, nw_send_buffer(%zu)", (صحيح)نتیجه,
                Curl_ngh2_session_get_remote_پنجره_اندازه(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_داده_RESTORE(cf, save);
  أرجع نتیجه;
}

ساكن CURLcode cf_h2_adjust_pollset(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *داده,
                                     بنية easy_pollset *ps)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية cf_call_داده save;
  curl_socket_t sock;
  bool want_recv, want_send;
  CURLcode نتیجه = CURLE_OK;

  إذا(!Curl_ngh2_valid(&ctx->ngh2))
    أرجع CURLE_OK;

  sock = Curl_conn_cf_get_socket(cf, داده);
  Curl_pollset_check(داده, ps, sock, &want_recv, &want_send);
  إذا(want_recv || want_send) {
    بنية h2_جریان_ctx *جریان = H2_جریان_CTX(ctx, داده);
    bool c_exhaust, s_exhaust;

    CF_داده_SAVE(save, cf, داده);
    c_exhaust = want_send &&
                !Curl_ngh2_session_get_remote_پنجره_اندازه(&ctx->ngh2);
    s_exhaust = want_send && جریان && جریان->id >= 0 &&
                !Curl_ngh2_session_get_جریان_remote_پنجره_اندازه(
                  &ctx->ngh2, جریان->id);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                (!c_exhaust && Curl_ngh2_session_want_write(&ctx->ngh2)) ||
                !Curl_bufq_is_خالی(&ctx->outbufq);

    نتیجه = Curl_pollset_مجموعه(داده, ps, sock, want_recv, want_send);
    CF_داده_RESTORE(cf, save);
  }
  وإلا إذا(ctx->فرستاده_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_داده_SAVE(save, cf, داده);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_خالی(&ctx->outbufq);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    نتیجه = Curl_pollset_مجموعه(داده, ps, sock, want_recv, want_send);
    CF_داده_RESTORE(cf, save);
  }
  أرجع نتیجه;
}

ساكن CURLcode cf_h2_ctx_open(بنية Curl_cfilter *cf,
                               بنية Curl_easy *داده)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_جریان_ctx *جریان;
  CURLcode نتیجه = CURLE_بیرون_از_حافظه;
  صحيح rc;
  nghttp2_session_callbacks *cbs = عدم;

  DEBUGASSERT(!ctx->ngh2.s);
  DEBUGASSERT(!ctx->ngh2.fatal_rv);
  DEBUGASSERT(ctx->initialized);

  rc = nghttp2_session_callbacks_new(&cbs);
  إذا(rc) {
    failf(داده, "Could not initialize nghttp2 callbacks");
    اذهب بیرون;
  }

  nghttp2_session_callbacks_مجموعه_send_callback(cbs, send_callback);
  nghttp2_session_callbacks_مجموعه_on_قاب_recv_callback(cbs, on_قاب_recv);
  nghttp2_session_callbacks_مجموعه_on_invalid_قاب_recv_callback(cbs,
    cf_h2_on_invalid_قاب_recv);
#اگر_تعریف_شده CURLVERBOSE
  nghttp2_session_callbacks_مجموعه_on_قاب_send_callback(cbs, on_قاب_send);
#پایان_اگر
  nghttp2_session_callbacks_مجموعه_on_داده_chunk_recv_callback(
    cbs, on_داده_chunk_recv);
  nghttp2_session_callbacks_مجموعه_on_جریان_close_callback(cbs, on_جریان_close);
  nghttp2_session_callbacks_مجموعه_on_سرآیند_callback(cbs, on_سرآیند);
#اگر_تعریف_شده CURLVERBOSE
  nghttp2_session_callbacks_مجموعه_error_callback(cbs, error_callback);
#پایان_اگر

  /* The nghttp2 session is not yet setup, do it */
  rc = h2_client_new(cf, cbs);
  إذا(rc) {
    failf(داده, "Could not initialize nghttp2");
    اذهب بیرون;
  }
  ctx->max_concurrent_streams = داده->multi ?
    Curl_multi_max_concurrent_streams(داده->multi) :
    DEFAULT_MAX_CONCURRENT_STREAMS;

  إذا(ctx->از_طریق_h1_upgrade) {
    /* HTTP/1.1 Upgrade issued. H2 Settings have already been submitted
     * in the H1 request and we upgrade from there. This stream
     * is opened implicitly as #1. */
    uint8_t binsettings[H2_BINSETTINGS_LEN];
    ssize_t rclen;
    size_t binlen; /* length of the binsettings data */

    rclen = populate_binsettings(binsettings, داده);

    إذا(!curlx_sztouz(rclen, &binlen) || !binlen) {
      failf(داده, "nghttp2 unexpectedly failed on pack_settings_payload");
      نتیجه = CURLE_FAILED_INIT;
      اذهب بیرون;
    }

    نتیجه = http2_داده_راهاندازی(cf, داده, &جریان);
    إذا(نتیجه)
      اذهب بیرون;
    DEBUGASSERT(جریان);
    جریان->id = 1;
    /* queue SETTINGS frame (again) */
    rc = Curl_ngh2_session_upgrade2(
      &ctx->ngh2, binsettings, binlen, داده->state.httpreq == HTTPREQ_سر,
      عدم);
    إذا(rc) {
      failf(داده, "nghttp2_session_upgrade2() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      نتیجه = CURLE_HTTP2;
      اذهب بیرون;
    }

    rc = Curl_ngh2_session_مجموعه_جریان_user_داده(
      &ctx->ngh2, جریان->id, جریان);
    إذا(rc) {
      infof(داده, "http/2: failed to set user_data for stream %d",
            جریان->id);
      DEBUGASSERT(0);
    }
    CURL_TRC_CF(داده, cf, "created session via Upgrade");
  }
  وإلا {
    nghttp2_settings_مدخل iv[H2_SETTINGS_IV_LEN];
    size_t ivlen;

    ivlen = populate_settings(iv, داده, ctx);
    rc = Curl_ngh2_submit_settings(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, iv, ivlen);
    إذا(rc) {
      failf(داده, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      نتیجه = CURLE_HTTP2;
      اذهب بیرون;
    }
  }

  rc = Curl_ngh2_session_مجموعه_local_پنجره_اندازه(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, HTTP2_HUGE_پنجره_اندازه);
  إذا(rc) {
    failf(داده, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    نتیجه = CURLE_HTTP2;
    اذهب بیرون;
  }

  /* all set, traffic will be send on connect */
  نتیجه = CURLE_OK;
  CURL_TRC_CF(داده, cf, "[0] created h2 session%s",
              ctx->از_طریق_h1_upgrade ? " (via h1 upgrade)" : "");

بیرون:
  إذا(cbs)
    nghttp2_session_callbacks_del(cbs);
  أرجع نتیجه;
}

ساكن CURLcode cf_h2_connect(بنية Curl_cfilter *cf,
                              بنية Curl_easy *داده,
                              bool *انجامشده)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  CURLcode نتیجه = CURLE_OK;
  بنية cf_call_داده save;
  bool اولی_زمان = FALSE;

  إذا(cf->connected) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  /* Connect the lower filters first */
  إذا(!cf->بعدی->connected) {
    نتیجه = Curl_conn_cf_connect(cf->بعدی, داده, انجامشده);
    إذا(نتیجه || !*انجامشده)
      أرجع نتیجه;
  }

  *انجامشده = FALSE;

  CF_داده_SAVE(save, cf, داده);
  DEBUGASSERT(ctx->initialized);
  إذا(!ctx->ngh2.s) {
    نتیجه = cf_h2_ctx_open(cf, داده);
    إذا(نتیجه)
      اذهب بیرون;
    اولی_زمان = TRUE;
  }

  إذا(!اولی_زمان) {
    نتیجه = h2_پیشرفت_ingress(cf, داده, 0);
    إذا(نتیجه)
      اذهب بیرون;
  }

  /* Send out our SETTINGS and ACKs and such. If that blocks, we
   * have it buffered and can count this filter as being connected */
  نتیجه = h2_پیشرفت_egress(cf, داده);
  إذا(نتیجه && (نتیجه != CURLE_AGAIN))
    اذهب بیرون;

  *انجامشده = TRUE;
  cf->connected = TRUE;
  نتیجه = CURLE_OK;

بیرون:
  CURL_TRC_CF(داده, cf, "cf_connect() -> %d, %d, ", (صحيح)نتیجه, *انجامشده);
  CF_داده_RESTORE(cf, save);
  أرجع نتیجه;
}

ساكن فراغ cf_h2_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *داده)
{
  بنية cf_h2_ctx *ctx = cf->ctx;

  (فراغ)داده;
  إذا(ctx) {
    cf_h2_ctx_free(ctx);
    cf->ctx = عدم;
  }
}

ساكن CURLcode cf_h2_shutdown(بنية Curl_cfilter *cf,
                               بنية Curl_easy *داده, bool *انجامشده)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية cf_call_داده save;
  CURLcode نتیجه;
  صحيح rv;

  إذا(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  CF_داده_SAVE(save, cf, داده);

  إذا(!ctx->فرستاده_goaway) {
    ctx->فرستاده_goaway = TRUE;
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, ctx->local_max_sid, 0,
      (ثابت uint8_t *)"shutdown", حجم("shutdown"));
    إذا(rv) {
      failf(داده, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      نتیجه = CURLE_SEND_ERROR;
      اذهب بیرون;
    }
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  نتیجه = CURLE_OK;
  إذا(Curl_ngh2_session_want_write(&ctx->ngh2) ||
     !Curl_bufq_is_خالی(&ctx->outbufq))
    نتیجه = h2_پیشرفت_egress(cf, داده);
  إذا(!نتیجه && Curl_ngh2_session_want_read(&ctx->ngh2))
    نتیجه = h2_پیشرفت_ingress(cf, داده, 0);

  إذا(نتیجه == CURLE_AGAIN)
    نتیجه = CURLE_OK;

  *انجامشده = (ctx->conn_closed ||
           (!نتیجه && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2) &&
            Curl_bufq_is_خالی(&ctx->outbufq)));

بیرون:
  CF_داده_RESTORE(cf, save);
  cf->shutdown = (نتیجه || *انجامشده);
  أرجع نتیجه;
}

ساكن CURLcode http2_داده_pause(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده,
                                 bool pause)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_جریان_ctx *جریان = H2_جریان_CTX(ctx, داده);

  DEBUGASSERT(داده);
  إذا(ctx && Curl_ngh2_valid(&ctx->ngh2) && جریان) {
    CURLcode نتیجه;

    جریان->write_paused = pause;
    نتیجه = cf_h2_update_local_win(cf, داده, جریان);
    إذا(نتیجه)
      أرجع نتیجه;

    /* attempt to send the window update */
    (فراغ)h2_پیشرفت_egress(cf, داده);

    إذا(!pause) {
      /* Unpausing a h2 transfer, requires it to be run again. The server
       * may send new DATA on us increasing the flow window, and it may
       * not. We may have already buffered and exhausted the new window
       * by operating on things in flight during the handling of other
       * transfers. */
      Curl_multi_mark_dirty(داده);
    }
    CURL_TRC_CF(داده, cf, "[%d] stream now %spaused", جریان->id,
                pause ? "" : "un");
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cf_h2_cntrl(بنية Curl_cfilter *cf,
                            بنية Curl_easy *داده,
                            صحيح event, صحيح arg1, فراغ *arg2)
{
  CURLcode نتیجه = CURLE_OK;
  بنية cf_call_داده save;

  (فراغ)arg2;

  CF_داده_SAVE(save, cf, داده);
  بدل(event) {
  حالة CF_CTRL_داده_راهاندازی:
    اكسر;
  حالة CF_CTRL_داده_PAUSE:
    نتیجه = http2_داده_pause(cf, داده, (arg1 != 0));
    اكسر;
  حالة CF_CTRL_FLUSH:
    نتیجه = cf_h2_flush(cf, داده);
    اكسر;
  حالة CF_CTRL_داده_انجامشده:
    http2_داده_انجامشده(cf, داده);
    اكسر;
  حالة CF_CTRL_CONN_INFO_UPDATE:
    إذا(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_دیده = 20;
      Curl_conn_مجموعه_multiplex(cf->conn);
    }
    اكسر;
  افتراضي:
    اكسر;
  }
  CF_داده_RESTORE(cf, save);
  أرجع نتیجه;
}

ساكن bool cf_h2_داده_pending(بنية Curl_cfilter *cf,
                               ثابت بنية Curl_easy *داده)
{
  بنية cf_h2_ctx *ctx = cf->ctx;

  إذا(ctx && !Curl_bufq_is_خالی(&ctx->inbufq))
    أرجع TRUE;
  أرجع cf->بعدی ? cf->بعدی->cft->has_داده_pending(cf->بعدی, داده) : FALSE;
}

ساكن bool cf_h2_is_alive(بنية Curl_cfilter *cf,
                           بنية Curl_easy *داده,
                           bool *درون_داد_pending)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  bool alive;
  بنية cf_call_داده save;

  *درون_داد_pending = FALSE;
  CF_داده_SAVE(save, cf, داده);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
           http2_connisalive(cf, داده, درون_داد_pending));
  CURL_TRC_CF(داده, cf, "conn alive -> %d, input_pending=%d",
              alive, *درون_داد_pending);
  CF_داده_RESTORE(cf, save);
  أرجع alive;
}

ساكن CURLcode cf_h2_keep_alive(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده)
{
  CURLcode نتیجه;
  بنية cf_call_داده save;

  CF_داده_SAVE(save, cf, داده);
  نتیجه = http2_send_ping(cf, داده);
  CF_داده_RESTORE(cf, save);
  أرجع نتیجه;
}

ساكن CURLcode cf_h2_query(بنية Curl_cfilter *cf,
                            بنية Curl_easy *داده,
                            صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية cf_call_داده save;
  size_t effective_max;

  بدل(query) {
  حالة CF_QUERY_MAX_CONCURRENT:
    DEBUGASSERT(pres1);

    CF_داده_SAVE(save, cf, داده);
    إذا(!Curl_ngh2_valid(&ctx->ngh2) ||
       !Curl_ngh2_session_check_request_allowed(&ctx->ngh2)) {
      /* the limit is what we have in use right now */
      effective_max = cf->conn->attached_xfers;
    }
    وإلا {
      effective_max = ctx->max_concurrent_streams;
    }
    *pres1 = (effective_max > بیشترین_صحیح) ? بیشترین_صحیح : (صحيح)effective_max;
    CF_داده_RESTORE(cf, save);
    أرجع CURLE_OK;
  حالة CF_QUERY_جریان_ERROR: {
    بنية h2_جریان_ctx *جریان = H2_جریان_CTX(ctx, داده);
    *pres1 = جریان ? (صحيح)جریان->error : 0;
    أرجع CURLE_OK;
  }
  حالة CF_QUERY_NEED_FLUSH: {
    بنية h2_جریان_ctx *جریان = H2_جریان_CTX(ctx, داده);
    إذا(!Curl_bufq_is_خالی(&ctx->outbufq) ||
       (جریان && !Curl_bufq_is_خالی(&جریان->sendbuf))) {
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
  أرجع cf->بعدی ?
    cf->بعدی->cft->query(cf->بعدی, داده, query, pres1, pres2) :
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
  cf_h2_داده_pending,
  cf_h2_send,
  cf_h2_recv,
  cf_h2_cntrl,
  cf_h2_is_alive,
  cf_h2_keep_alive,
  cf_h2_query,
};

ساكن CURLcode http2_cfilter_جمع_کن(بنية Curl_cfilter **pcf,
                                  بنية Curl_easy *داده,
                                  بنية connectdata *conn,
                                  int8_t sockindex,
                                  bool از_طریق_h1_upgrade)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_h2_ctx *ctx;
  CURLcode نتیجه = CURLE_بیرون_از_حافظه;

  DEBUGASSERT(داده->conn);
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx)
    اذهب بیرون;
  cf_h2_ctx_init(ctx, از_طریق_h1_upgrade);

  نتیجه = Curl_cf_create(&cf, &Curl_cft_nghttp2, ctx);
  إذا(نتیجه)
    اذهب بیرون;

  ctx = عدم;
  Curl_conn_cf_جمع_کن(داده, conn, sockindex, cf);

بیرون:
  إذا(نتیجه)
    cf_h2_ctx_free(ctx);
  *pcf = نتیجه ? عدم : cf;
  أرجع نتیجه;
}

ساكن CURLcode http2_cfilter_درج_after(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *داده,
                                           bool از_طریق_h1_upgrade)
{
  بنية Curl_cfilter *cf_h2 = عدم;
  بنية cf_h2_ctx *ctx;
  CURLcode نتیجه = CURLE_بیرون_از_حافظه;

  (فراغ)داده;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx)
    اذهب بیرون;
  cf_h2_ctx_init(ctx, از_طریق_h1_upgrade);

  نتیجه = Curl_cf_create(&cf_h2, &Curl_cft_nghttp2, ctx);
  إذا(نتیجه)
    اذهب بیرون;

  ctx = عدم;
  Curl_conn_cf_درج_after(cf, cf_h2);

بیرون:
  إذا(نتیجه)
    cf_h2_ctx_free(ctx);
  أرجع نتیجه;
}

bool Curl_http2_may_switch(بنية Curl_easy *داده)
{
  إذا(Curl_conn_http_نسخه(داده, داده->conn) < 20 &&
     (داده->state.http_neg.wanted & CURL_HTTP_V2x) &&
     داده->state.http_neg.h2_پیشین_knowledge) {
#اگر_تعریف_نشده CURL_DISABLE_PROXY
    إذا(داده->conn->بیتها.origin_is_proxy) {
      infof(داده, "Ignoring HTTP/2 prior knowledge due to proxy");
      أرجع FALSE;
    }
#پایان_اگر
    أرجع TRUE;
  }
  أرجع FALSE;
}

CURLcode Curl_http2_switch(بنية Curl_easy *داده)
{
  بنية Curl_cfilter *cf;
  CURLcode نتیجه;

  DEBUGASSERT(Curl_conn_http_نسخه(داده, داده->conn) < 20);

  نتیجه = http2_cfilter_جمع_کن(&cf, داده, داده->conn, FIRSTSOCKET, FALSE);
  إذا(نتیجه)
    أرجع نتیجه;
  CURL_TRC_CF(داده, cf, "switching connection to HTTP/2");

  إذا(cf->بعدی) {
    bool انجامشده;
    أرجع Curl_conn_cf_connect(cf, داده, &انجامشده);
  }
  أرجع CURLE_OK;
}

CURLcode Curl_http2_switch_at(بنية Curl_cfilter *cf, بنية Curl_easy *داده)
{
  بنية Curl_cfilter *cf_h2;
  CURLcode نتیجه;

  DEBUGASSERT(Curl_conn_http_نسخه(داده, داده->conn) < 20);

  نتیجه = http2_cfilter_درج_after(cf, داده, FALSE);
  إذا(نتیجه)
    أرجع نتیجه;

  cf_h2 = cf->بعدی;

  إذا(cf_h2->بعدی) {
    bool انجامشده;
    أرجع Curl_conn_cf_connect(cf_h2, داده, &انجامشده);
  }
  أرجع CURLE_OK;
}

CURLcode Curl_http2_upgrade(بنية Curl_easy *داده,
                            بنية connectdata *conn, int8_t sockindex,
                            ثابت حرف *mem, size_t nread)
{
  بنية Curl_cfilter *cf;
  بنية cf_h2_ctx *ctx;
  CURLcode نتیجه;

  DEBUGASSERT(Curl_conn_http_نسخه(داده, conn) < 20);

  نتیجه = http2_cfilter_جمع_کن(&cf, داده, conn, sockindex, TRUE);
  إذا(نتیجه)
    أرجع نتیجه;
  CURL_TRC_CF(داده, cf, "upgrading connection to HTTP/2");

  DEBUGASSERT(cf->cft == &Curl_cft_nghttp2);
  ctx = cf->ctx;

  داده->req.httpversion_فرستاده = 20; /* it is an h2 request now */
  داده->req.سرآیند = TRUE; /* we expect the real response to come in h2 */
  داده->req.headerline = 0; /* restart the header line counter */

  إذا(nread > 0) {
    /* Remaining data from the protocol switch reply is already using
     * the switched protocol, ie. HTTP/2. We add that to the network
     * inbufq. */
    size_t copied;

    نتیجه = Curl_bufq_write(&ctx->inbufq,
                             (ثابت بیعلامت حرف *)mem, nread, &copied);
    إذا(نتیجه) {
      failf(داده, "error on copying HTTP Upgrade response: %d", (صحيح)نتیجه);
      أرجع CURLE_RECV_ERROR;
    }
    إذا(copied < nread) {
      failf(داده, "connection buffer size could not take all data "
            "from HTTP Upgrade response header: copied=%zu, datalen=%zu",
            copied, nread);
      أرجع CURLE_HTTP2;
    }
    infof(داده, "Copied HTTP/2 data in stream buffer to connection buffer"
          " after upgrade: len=%zu", nread);
  }

  إذا(cf->بعدی) {
    bool انجامشده;
    نتیجه = Curl_conn_cf_connect(cf, داده, &انجامشده);
    إذا(!نتیجه)
      cf->cft->cntrl(cf, داده, CF_CTRL_CONN_INFO_UPDATE, 0, عدم);
  }
  أرجع نتیجه;
}

/* Only call this function for a transfer that already got an HTTP/2
   CURLE_HTTP2_STREAM error! */
bool Curl_h2_http_1_1_error(بنية Curl_easy *داده)
{
  إذا(Curl_conn_http_نسخه(داده, داده->conn) == 20) {
    صحيح err = Curl_conn_get_جریان_error(داده, داده->conn, FIRSTSOCKET);
    أرجع err == NGHTTP2_HTTP_1_1_REQUIRED;
  }
  أرجع FALSE;
}

فراغ *Curl_nghttp2_malloc(size_t اندازه, فراغ *user_داده)
{
  (فراغ)user_داده;
  أرجع Curl_cmalloc(اندازه);
}

فراغ Curl_nghttp2_free(فراغ *ptr, فراغ *user_داده)
{
  (فراغ)user_داده;
  Curl_cfree(ptr);
}

فراغ *Curl_nghttp2_calloc(size_t nmemb, size_t اندازه, فراغ *user_داده)
{
  (فراغ)user_داده;
  أرجع Curl_ccalloc(nmemb, اندازه);
}

فراغ *Curl_nghttp2_realloc(فراغ *ptr, size_t اندازه, فراغ *user_داده)
{
  (فراغ)user_داده;
  أرجع Curl_crealloc(ptr, اندازه);
}

#وإلا /* CURL_DISABLE_HTTP || !USE_NGHTTP2 */

/* Satisfy external references even if http2 is not compiled in. */

حرف *curl_pushheader_bynum(بنية curl_pushheaders *h, size_t رقم)
{
  (فراغ)h;
  (فراغ)رقم;
  أرجع عدم;
}

حرف *curl_pushheader_byname(بنية curl_pushheaders *h, ثابت حرف *name)
{
  (فراغ)h;
  (فراغ)name;
  أرجع عدم;
}

#پایان_اگر /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
