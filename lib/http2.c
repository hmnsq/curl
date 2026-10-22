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

#ܐܢ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
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

#ܐܢ NGHTTP2_VERSION_NUM < 0x010f00
#error "nghttp2 1.15.0 or greater required"
#endif

#define NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE 1

/* buffer dimensioning:
 * use 16K as chunk size, as that fits H2 DATA frames well */
#define H2_CHUNK_SIZE           (16 * 1024)
/* connection window size */
#define H2_CONN_WINDOW_SIZE     (10 * 1024 * 1024)
/* on receiving from TLS, we prep for holding a full stream window */
#define H2_NW_RECV_CHUNKS       (H2_CONN_WINDOW_SIZE / H2_CHUNK_SIZE)
/* on send into TLS, we want to accumulate small frames */
#define H2_NW_SEND_CHUNKS       1
/* this is how much we want "in flight" for a stream, unthrottled */
#define H2_STREAM_WINDOW_SIZE_MAX   (10 * 1024 * 1024)
/* this is how much we want "in flight" for a stream, initially, IFF
 * nghttp2 allows us to tweak the local window size. */
#ܐܢ NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
#define H2_STREAM_WINDOW_SIZE_INITIAL  (64 * 1024)
#ܐܚܪܢܐ
#define H2_STREAM_WINDOW_SIZE_INITIAL H2_STREAM_WINDOW_SIZE_MAX
#endif
/* keep smaller stream upload buffer (default h2 window size) to have
 * our progress bars and "upload done" reporting closer to reality */
#define H2_STREAM_SEND_CHUNKS   ((64 * 1024) / H2_CHUNK_SIZE)
/* spare chunks we keep for a full window */
#define H2_STREAM_POOL_SPARES   (H2_CONN_WINDOW_SIZE / H2_CHUNK_SIZE)

/* We need to accommodate the max number of streams with their window sizes on
 * the overall connection. Streams might become PAUSED which will block their
 * received QUOTA in the connection window. If we run out of space, the server
 * is blocked from sending us any data. See #10988 for an issue with this. */
#define HTTP2_HUGE_WINDOW_SIZE (100 * H2_STREAM_WINDOW_SIZE_MAX)

#define H2_SETTINGS_IV_LEN 3
#define H2_BINSETTINGS_LEN 80

بنية cf_h2_ctx {
  بنية curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  بنية cf_call_data call_data;

  بنية bufq inbufq;           /* network input */
  بنية bufq outbufq;          /* network output */
  بنية bufc_pool stream_bufcp; /* spares for stream buffers */
  بنية dynbuf scratch;        /* scratch buffer for temp use */

  بنية u32_ptrset streams; /* hash of `data->mid` to `h2_stream_ctx` */
  size_t drain_total; /* sum of all stream's UrlState drain */
  uint32_t initial_win_size; /* current initial window size (settings) */
  uint32_t max_concurrent_streams;
  uint32_t goaway_error;        /* goaway error code from server */
  int32_t remote_max_sid;       /* max id processed by server */
  int32_t local_max_sid;        /* max id processed by us */
  BIT(initialized);
  BIT(via_h1_upgrade);
  BIT(conn_closed);
  BIT(rcvd_goaway);
  BIT(sent_goaway);
  BIT(enable_push);
  BIT(nw_out_blocked);
};

/* How to access `call_data` from a cf_h2 filter */
#undef CF_CTX_CALL_DATA
#define CF_CTX_CALL_DATA(cf) ((بنية cf_h2_ctx *)(cf)->ctx)->call_data

/**
 * All about the H2 internals of a stream
 */
بنية h2_stream_ctx {
  بنية bufq sendbuf; /* request buffer */
  بنية h1_req_parser h1; /* parsing the request */
  بنية dynhds resp_trailers; /* response trailer fields */
  size_t resp_hds_len; /* amount of response header bytes in recvbuf */
  curl_off_t xfer_id;  /* transfer `id` owning this stream */
  curl_off_t nrcvd_data;  /* number of DATA bytes received */

  حرف **push_headers;       /* allocated array */
  size_t push_headers_used;  /* number of entries filled in */
  size_t push_headers_alloc; /* number of entries allocated */

  int32_t id; /* HTTP/2 protocol identifier for stream */
  uint32_t mid; /* transfer `mid` owning this stream */
  uint32_t error; /* stream error code */
  CURLcode xfer_result; /* Result of writing out response */
  int32_t local_window_size; /* the local recv window size */
  int32_t status_code; /* HTTP response status code */
  BIT(resp_hds_complete); /* we have a complete, final response */
  BIT(closed); /* TRUE on stream close */
  BIT(reset);  /* TRUE on stream reset */
  BIT(reset_by_server);  /* TRUE on stream reset by server */
  BIT(close_handled); /* TRUE if stream closure is handled by libcurl */
  BIT(bodystarted);
  BIT(body_eos);    /* the complete body has been added to `sendbuf` and
                     * is being/has been processed from there. */
  BIT(write_paused);  /* stream write is paused */
};

ساكن فراغ free_push_headers(بنية h2_stream_ctx *stream)
{
  size_t i;
  لكل(i = 0; i < stream->push_headers_used; i++)
    curlx_free(stream->push_headers[i]);
  curlx_safefree(stream->push_headers);
  stream->push_headers_used = 0;
}

ساكن فراغ h2_stream_ctx_free(بنية h2_stream_ctx *stream)
{
  Curl_bufq_free(&stream->sendbuf);
  Curl_h1_req_parse_free(&stream->h1);
  Curl_dynhds_free(&stream->resp_trailers);
  free_push_headers(stream);
  curlx_free(stream);
}

ساكن فراغ h2_stream_hash_free(uint32_t id, فراغ *stream)
{
  (فراغ)id;
  DEBUGASSERT(stream);
  h2_stream_ctx_free((بنية h2_stream_ctx *)stream);
}

ساكن فراغ cf_h2_ctx_init(بنية cf_h2_ctx *ctx, bool via_h1_upgrade)
{
  Curl_bufcp_init(&ctx->stream_bufcp, H2_CHUNK_SIZE, H2_STREAM_POOL_SPARES);
  Curl_bufq_initp(&ctx->inbufq, &ctx->stream_bufcp, H2_NW_RECV_CHUNKS, 0);
  Curl_bufq_initp(&ctx->outbufq, &ctx->stream_bufcp, H2_NW_SEND_CHUNKS, 0);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_HEADER);
  Curl_u32_ptrset_init(&ctx->streams, h2_stream_hash_free);
  ctx->remote_max_sid = INT32_MAX;
  ctx->via_h1_upgrade = via_h1_upgrade;
  ctx->initialized = TRUE;
}

ساكن فراغ cf_h2_ctx_free(بنية cf_h2_ctx *ctx)
{
  ܐܢ(ctx && ctx->initialized) {
    Curl_ngh2_del(&ctx->ngh2);
    Curl_bufq_free(&ctx->inbufq);
    Curl_bufq_free(&ctx->outbufq);
    Curl_bufcp_free(&ctx->stream_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    memset(ctx, 0, حجم(*ctx));
  }
  curlx_free(ctx);
}

ساكن uint32_t cf_h2_initial_win_size(بنية Curl_easy *data)
{
#ܐܢ NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
  /* If the transfer has a rate-limit lower than the default initial
   * stream window size, use that. It needs to be at least 8k or servers
   * may be unhappy. */
  curl_off_t rps = Curl_rlimit_per_step(&data->progress.dl.rlimit);
  ܐܢ((rps > 0) && (rps < H2_STREAM_WINDOW_SIZE_INITIAL))
    ܦܢܐ CURLMAX((uint32_t)rps, 8192);
#endif
  ܦܢܐ H2_STREAM_WINDOW_SIZE_INITIAL;
}

ساكن size_t populate_settings(nghttp2_settings_entry *iv,
                                بنية Curl_easy *data,
                                بنية cf_h2_ctx *ctx)
{
  iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
  iv[0].value = Curl_multi_max_concurrent_streams(data->multi);

  iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
  iv[1].value = cf_h2_initial_win_size(data);
  ܐܢ(ctx)
    ctx->initial_win_size = iv[1].value;
  iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_PUSH;
  iv[2].value = !!data->multi->push_cb;

  ܦܢܐ 3;
}

ساكن ssize_t populate_binsettings(uint8_t *binsettings,
                                    بنية Curl_easy *data)
{
  nghttp2_settings_entry iv[H2_SETTINGS_IV_LEN];
  size_t ivlen;

  ivlen = populate_settings(iv, data, عدم);
  /* this returns number of bytes it wrote or a negative number on error. */
  ܦܢܐ nghttp2_pack_settings_payload(binsettings, H2_BINSETTINGS_LEN,
                                       iv, ivlen);
}

ساكن CURLcode cf_h2_update_settings(بنية cf_h2_ctx *ctx,
                                      uint32_t initial_win_size)
{
  nghttp2_settings_entry entry;
  entry.settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
  entry.value = initial_win_size;
  ܐܢ(Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, &entry, 1))
    ܦܢܐ CURLE_SEND_ERROR;
  ctx->initial_win_size = initial_win_size;
  ܦܢܐ CURLE_OK;
}

#define H2_STREAM_CTX(ctx, data)                                         \
  ((بنية h2_stream_ctx *)(                                             \
    (data) ? Curl_u32_ptrset_get(&(ctx)->streams, (data)->mid) : عدم))

ساكن بنية h2_stream_ctx *h2_stream_ctx_create(بنية cf_h2_ctx *ctx,
                                                  بنية Curl_easy *data)
{
  بنية h2_stream_ctx *stream;

  stream = curlx_calloc(1, حجم(*stream));
  ܐܢ(!stream)
    ܦܢܐ عدم;

  stream->id = -1;
  stream->xfer_id = data->id;
  stream->mid = data->mid;
  Curl_bufq_initp(&stream->sendbuf, &ctx->stream_bufcp,
                  H2_STREAM_SEND_CHUNKS, BUFQ_OPT_NONE);
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_LINE_LEN);
  Curl_dynhds_init(&stream->resp_trailers,
                   MAX_HTTP_RESP_HEADER_COUNT, DYN_HTTP_REQUEST);
  stream->bodystarted = FALSE;
  stream->status_code = -1;
  stream->closed = FALSE;
  stream->close_handled = FALSE;
  stream->error = NGHTTP2_NO_ERROR;
  stream->local_window_size = H2_STREAM_WINDOW_SIZE_INITIAL;
  stream->nrcvd_data = 0;
  ܦܢܐ stream;
}

ساكن بنية Curl_easy *cf_h2_get_stream_xfer(بنية Curl_cfilter *cf,
                                               int32_t stream_id,
                                               بنية h2_stream_ctx **pstream)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_stream_ctx *stream =
    Curl_ngh2_session_get_stream_user_data(&ctx->ngh2, stream_id);
  ܐܢ(pstream)
    *pstream = stream;
  ܐܢ(stream && (stream->mid != UINT32_MAX)) {
    بنية Curl_easy *data, *call_data = CF_DATA_CURRENT(cf);
    ܐܢ(!call_data || !call_data->multi) {
      DEBUGASSERT(0);
      ܦܢܐ عدم;
    }
    data = Curl_multi_get_easy(call_data->multi, stream->mid);
    ܐܢ(data && (data->id == stream->xfer_id))
      ܦܢܐ data;
  }
  ܦܢܐ عدم;
}

#ifdef NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
ساكن int32_t cf_h2_get_desired_local_win(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *data)
{
  curl_off_t avail = Curl_rlimit_avail(&data->progress.dl.rlimit, عدم);

  (فراغ)cf;
  ܐܢ(avail < CURL_OFF_T_MAX) { /* limit in place */
    ܐܢ(avail <= 0)
      ܦܢܐ 0;
    ܐܚܪܢܐ ܐܢ(avail < H2_STREAM_WINDOW_SIZE_MAX)
      ܦܢܐ (int32_t)avail;
  }
  ܦܢܐ H2_STREAM_WINDOW_SIZE_MAX;
}

ساكن CURLcode cf_h2_update_local_win(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *data,
                                       بنية h2_stream_ctx *stream)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  int32_t dwsize;
  صحيح rv;

  dwsize = (stream->write_paused || stream->xfer_result) ?
           0 : cf_h2_get_desired_local_win(cf, data);
  ܐܢ(dwsize != stream->local_window_size) {
    int32_t wsize = Curl_ngh2_session_get_stream_effective_local_window_size(
      &ctx->ngh2, stream->id);
    ܐܢ(dwsize > wsize) {
      rv = Curl_ngh2_session_set_local_window_size(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize);
      ܐܢ(rv) {
        failf(data, "[%d] nghttp2 set_local_window_size(%d) failed: "
              "%s(%d)", stream->id, dwsize, nghttp2_strerror(rv), rv);
        ܦܢܐ CURLE_HTTP2;
      }
      rv = Curl_ngh2_submit_window_update(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize - wsize);
      ܐܢ(rv) {
        failf(data, "[%d] nghttp2_submit_window_update() failed: "
              "%s(%d)", stream->id, nghttp2_strerror(rv), rv);
        ܦܢܐ CURLE_HTTP2;
      }
      stream->local_window_size = dwsize;
      CURL_TRC_CF(data, cf, "[%d] local window update by %d",
                  stream->id, dwsize - wsize);
    }
    ܐܚܪܢܐ {
      rv = Curl_ngh2_session_set_local_window_size(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize);
      ܐܢ(rv) {
        failf(data, "[%d] nghttp2_session_set_local_window_size() failed: "
              "%s(%d)", stream->id, nghttp2_strerror(rv), rv);
        ܦܢܐ CURLE_HTTP2;
      }
      stream->local_window_size = dwsize;
      CURL_TRC_CF(data, cf, "[%d] local window size now %d",
                  stream->id, dwsize);
    }
  }
  ܦܢܐ CURLE_OK;
}

#ܐܚܪܢܐ /* NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

ساكن CURLcode cf_h2_update_local_win(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *data,
                                       بنية h2_stream_ctx *stream)
{
  (فراغ)cf;
  (فراغ)data;
  (فراغ)stream;
  ܦܢܐ CURLE_OK;
}
#endif /* !NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

ساكن CURLcode http2_data_setup(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data,
                                 بنية h2_stream_ctx **pstream)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_stream_ctx *stream;

  (فراغ)cf;
  DEBUGASSERT(data);

  ܐܢ(!data)
    ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;

  stream = H2_STREAM_CTX(ctx, data);
  ܐܢ(stream) {
    *pstream = stream;
    ܦܢܐ CURLE_OK;
  }

  stream = h2_stream_ctx_create(ctx, data);
  ܐܢ(!stream)
    ܦܢܐ CURLE_OUT_OF_MEMORY;

  ܐܢ(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    h2_stream_ctx_free(stream);
    ܦܢܐ CURLE_OUT_OF_MEMORY;
  }

  *pstream = stream;
  ܦܢܐ CURLE_OK;
}

ساكن CURLcode nw_out_flush(بنية Curl_cfilter *cf,
                             بنية Curl_easy *data)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  ܐܢ(Curl_bufq_is_empty(&ctx->outbufq))
    ܦܢܐ CURLE_OK;

  result = Curl_cf_send_bufq(cf->next, data, &ctx->outbufq, عدم, 0,
                             &nwritten);
  ܐܢ(result) {
    ܐܢ(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_out_blocked = 1;
    }
    ܦܢܐ result;
  }
  ܦܢܐ Curl_bufq_is_empty(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

ساكن فراغ http2_data_done(بنية Curl_cfilter *cf, بنية Curl_easy *data)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);

  DEBUGASSERT(ctx);
  ܐܢ(!stream || !ctx->initialized)
    ܦܢܐ;

  ܐܢ(Curl_ngh2_valid(&ctx->ngh2)) {
    bool flush_egress = FALSE;
    /* returns error if stream not known, which is fine here */
    (فراغ)Curl_ngh2_session_set_stream_user_data(&ctx->ngh2, stream->id, عدم);

    ܐܢ(!stream->closed && stream->id > 0) {
      /* RST_STREAM */
      CURL_TRC_CF(data, cf, "[%d] premature DATA_DONE, RST stream",
                  stream->id);
      stream->closed = TRUE;
      stream->reset = TRUE;
      Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                  stream->id, NGHTTP2_STREAM_CLOSED);
      flush_egress = TRUE;
    }

    ܐܢ(flush_egress) {
      (فراغ)Curl_ngh2_session_send(&ctx->ngh2);
      (فراغ)nw_out_flush(cf, data);
    }
  }

  Curl_u32_ptrset_unset(&ctx->streams, data->mid);
}

ساكن صحيح h2_client_new(بنية Curl_cfilter *cf,
                         nghttp2_session_callbacks *cbs)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { عدم, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  صحيح rc = nghttp2_option_new(&o);
  ܐܢ(rc)
    ܦܢܐ rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_set_no_auto_window_update(o, 1);
#ܐܢ NGHTTP2_VERSION_NUM >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_set_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#endif
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  ܦܢܐ rc;
}

/*
 * Returns nonzero if current HTTP/2 session should be closed.
 */
ساكن صحيح should_close_session(بنية cf_h2_ctx *ctx)
{
  ܦܢܐ ctx->drain_total == 0 && !Curl_ngh2_session_want_read(&ctx->ngh2) &&
    !Curl_ngh2_session_want_write(&ctx->ngh2);
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
ساكن CURLcode h2_process_pending_input(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *data)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  ثابت unsigned حرف *buf;
  size_t blen, nread;
  ssize_t rv;

  ܟܡܐ(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {
    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ثابت uint8_t *)buf, blen);
    ܐܢ(!curlx_sztouz(rv, &nread)) {
      failf(data, "nghttp2 recv error %zd: %s", rv, nghttp2_strerror((صحيح)rv));
      ܦܢܐ CURLE_RECV_ERROR;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    ܐܢ(Curl_bufq_is_empty(&ctx->inbufq)) {
      اكسر;
    }
    ܐܚܪܢܐ {
      CURL_TRC_CF(data, cf, "process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }

  ܐܢ(Curl_ngh2_session_check_request_allowed(&ctx->ngh2) == 0) {
    /* No more requests are allowed in the current session, so
       the connection may not be reused. This is set when a
       GOAWAY frame has been received or when the limit of stream
       identifiers has been reached. */
    CURL_TRC_M(data, "http/2: No new requests allowed");
    connclose(cf->conn);
  }

  ܦܢܐ CURLE_OK;
}

/*
 * The server may send us data at any point (e.g. PING frames). Therefore, we
 * cannot assume that an HTTP/2 socket is dead because it is readable.
 *
 * Check the lower filters first and, if successful, peek at the socket
 * and distinguish between closed and data.
 */
ساكن bool http2_connisalive(بنية Curl_cfilter *cf, بنية Curl_easy *data,
                              bool *input_pending)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *input_pending = FALSE;
  ܐܢ(!cf->next || !cf->next->cft->is_alive(cf->next, data, input_pending))
    ܦܢܐ FALSE;

  ܐܢ(*input_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode result;
    size_t nread;

    *input_pending = FALSE;
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(data, cf, "connisalive, recv pending input -> %d, %zu",
                (صحيح)result, nread);
    ܐܢ(!result) {
      result = h2_process_pending_input(cf, data);
      ܐܢ(result)
        /* immediate error, considered dead */
        alive = FALSE;
      ܐܚܪܢܐ {
        alive = !should_close_session(ctx);
      }
    }
    ܐܚܪܢܐ ܐܢ(result != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  ܦܢܐ alive;
}

ساكن CURLcode http2_send_ping(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  صحيح rc;

  rc = Curl_ngh2_submit_ping(&ctx->ngh2, 0, ZERO_NULL);
  ܐܢ(rc) {
    failf(data, "nghttp2_submit_ping() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    ܦܢܐ CURLE_HTTP2;
  }

  rc = Curl_ngh2_session_send(&ctx->ngh2);
  ܐܢ(rc) {
    failf(data, "nghttp2_session_send() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    ܦܢܐ CURLE_SEND_ERROR;
  }
  ܦܢܐ CURLE_OK;
}

/*
 * Store nghttp2 version info in this buffer.
 */
فراغ Curl_http2_ver(حرف *p, size_t len)
{
  nghttp2_info *h2 = nghttp2_version(0);
  (فراغ)curl_msnprintf(p, len, "nghttp2/%s", h2->version_str);
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
  بنية Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result = CURLE_OK;

  (فراغ)h2;
  (فراغ)flags;
  DEBUGASSERT(data);

  ܐܢ(!cf->connected)
    result = Curl_bufq_write(&ctx->outbufq, buf, blen, &nwritten);
  ܐܚܪܢܐ
    result = Curl_cf_send_bufq(cf->next, data, &ctx->outbufq, buf, blen,
                               &nwritten);

  ܐܢ(result) {
    ܐܢ(result == CURLE_AGAIN) {
      ctx->nw_out_blocked = 1;
      ܦܢܐ NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(data, "Failed sending HTTP2 data");
    ܦܢܐ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  ܐܢ(!nwritten) {
    ctx->nw_out_blocked = 1;
    ܦܢܐ NGHTTP2_ERR_WOULDBLOCK;
  }
  ܦܢܐ (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

/* We pass a pointer to this struct in the push callback, but the contents of
   the struct are hidden from the user. */
بنية curl_pushheaders {
  بنية Curl_easy *data;
  بنية h2_stream_ctx *stream;
  ثابت nghttp2_push_promise *frame;
};

/*
 * push header access function. Only to be used from within the push callback
 */
حرف *curl_pushheader_bynum(بنية curl_pushheaders *h, size_t num)
{
  /* Verify that we got a good easy handle in the push header struct, mostly to
     detect rubbish input fast(er). */
  ܐܢ(!h || !GOOD_EASY_HANDLE(h->data))
    ܦܢܐ عدم;
  ܐܚܪܢܐ {
    ܐܢ(h->stream && num < h->stream->push_headers_used)
      ܦܢܐ h->stream->push_headers[num];
  }
  ܦܢܐ عدم;
}

/*
 * push header access function. Only to be used from within the push callback
 */
حرف *curl_pushheader_byname(بنية curl_pushheaders *h, ثابت حرف *name)
{
  بنية h2_stream_ctx *stream;
  size_t len;
  size_t i;
  /* Verify that we got a good easy handle in the push header struct,
     mostly to detect rubbish input fast(er). Also empty header name
     is rubbish too. We have to allow ":" at the beginning of
     the header, but header == ":" must be rejected. If we have ':' in
     the middle of header, it could be matched in middle of the value,
     this is because we do prefix match.*/
  ܐܢ(!h || !GOOD_EASY_HANDLE(h->data) || !name || !name[0] ||
     !strcmp(name, ":") || strchr(name + 1, ':'))
    ܦܢܐ عدم;

  stream = h->stream;
  ܐܢ(!stream)
    ܦܢܐ عدم;

  len = strlen(name);
  لكل(i = 0; i < stream->push_headers_used; i++) {
    ܐܢ(!strncmp(name, stream->push_headers[i], len)) {
      /* sub-match, make sure that it is followed by a colon */
      ܐܢ(stream->push_headers[i][len] != ':')
        استمر;
      ܦܢܐ &stream->push_headers[i][len + 1];
    }
  }
  ܦܢܐ عدم;
}

ساكن بنية Curl_easy *h2_duphandle(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *data)
{
  بنية Curl_easy *second = curl_easy_init();
  ܐܢ(second) {
    بنية h2_stream_ctx *second_stream;
    http2_data_setup(cf, second, &second_stream);
    second->state.weight = data->state.weight;
    ܐܢ(data->share)
      (فراغ)Curl_share_easy_link(second, data->share);
    /* copy a range of options from the parent handle */
    second->set.fwrite_func = data->set.fwrite_func;
    second->set.out = data->set.out;
    second->set.fwrite_header = data->set.fwrite_header;
    second->set.writeheader = data->set.writeheader;
    second->set.fdebug = data->set.fdebug;
    second->set.debugdata = data->set.debugdata;
    second->set.fxferinfo = data->set.fxferinfo;
    second->set.fprogress = data->set.fprogress;
    second->set.progress_client = data->set.progress_client;
    second->progress.hide = data->progress.hide;
    second->progress.callback = data->progress.callback;
  }
  ܦܢܐ second;
}

ساكن صحيح set_transfer_url(بنية Curl_easy *newhandle,
                            بنية curl_pushheaders *hp,
                            بنية Curl_easy *data)
{
  ثابت حرف *v;
  CURLUcode uc;
  حرف *url = عدم;
  صحيح rc = 0;
  CURLU *u = curl_url();

  ܐܢ(!u)
    ܦܢܐ 5;

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_SCHEME);
  ܐܢ(v) {
    uc = curl_url_set(u, CURLUPART_SCHEME, v, 0);
    ܐܢ(uc) {
      rc = 1;
      اذهب fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_AUTHORITY);
  ܐܢ(v) {
    uc = Curl_url_set_authority(u, v);
    ܐܢ(uc) {
      rc = 2;
      اذهب fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_PATH);
  ܐܢ(v) {
    uc = curl_url_set(u, CURLUPART_PATH, v, 0);
    ܐܢ(uc) {
      rc = 3;
      اذهب fail;
    }
  }

  /* We can only allow PUSH of resource from the same origin, e.g.
   * scheme + hostname + port */
  ܐܢ(!Curl_url_same_origin(data->state.uh, u)) {
    rc = 1;
    اذهب fail;
  }

  uc = curl_url_get(u, CURLUPART_URL, &url, 0);
  ܐܢ(uc)
    rc = 4;
fail:
  ܐܢ(rc) {
    curl_url_cleanup(u);
    ܦܢܐ rc;
  }

  DEBUGASSERT(!newhandle->state.uh);
  newhandle->state.uh = u;
  Curl_bufref_set(&newhandle->state.url, url, 0, curl_free);
  ܦܢܐ 0;
}

ساكن فراغ discard_newhandle(بنية Curl_cfilter *cf,
                              بنية Curl_easy *newhandle)
{
  http2_data_done(cf, newhandle);
  (فراغ)Curl_close(&newhandle);
}

ساكن صحيح push_promise(بنية Curl_cfilter *cf,
                        بنية Curl_easy *data,
                        ثابت nghttp2_push_promise *frame)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  صحيح rv; /* one of the CURL_PUSH_* defines */

  CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE received",
              frame->promised_stream_id);
  ܐܢ(data->multi->push_cb) {
    بنية h2_stream_ctx *stream;
    بنية h2_stream_ctx *newstream;
    بنية curl_pushheaders heads;
    CURLMcode mresult;
    CURLcode result;
    /* clone the parent */
    بنية Curl_easy *newhandle = h2_duphandle(cf, data);
    ܐܢ(!newhandle) {
      infof(data, "failed to duplicate handle");
      rv = CURL_PUSH_DENY; /* FAIL HARD */
      اذهب fail;
    }

    stream = H2_STREAM_CTX(ctx, data);
    ܐܢ(!stream) {
      failf(data, "Internal NULL stream");
      discard_newhandle(cf, newhandle);
      rv = CURL_PUSH_DENY;
      اذهب fail;
    }

    heads.data = data;
    heads.stream = stream;
    heads.frame = frame;

    rv = set_transfer_url(newhandle, &heads, data);
    ܐܢ(rv) {
      CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE, failed to set URL -> %d",
                  frame->promised_stream_id, rv);
      discard_newhandle(cf, newhandle);
      rv = CURL_PUSH_DENY;
      اذهب fail;
    }

    {
      بنية Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, multi_push_cb);
      rv = data->multi->push_cb(data, newhandle,
                                stream->push_headers_used, &heads,
                                data->multi->push_userp);
      CURL_CBAPI_END(&guard);
    }

    /* free the headers again */
    free_push_headers(stream);

    ܐܢ(rv) {
      DEBUGASSERT((rv > CURL_PUSH_OK) && (rv <= CURL_PUSH_ERROROUT));
      /* denied, kill off the new handle again */
      CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE, denied by application -> %d",
                  frame->promised_stream_id, rv);
      discard_newhandle(cf, newhandle);
      اذهب fail;
    }

    /* approved, add to the multi handle for processing. This
     * assigns newhandle->mid. For the new `mid` we assign the
     * h2_stream instance and remember the stream_id already known. */
    mresult = Curl_multi_add_perform(data->multi, newhandle, cf->conn);
    ܐܢ(mresult) {
      infof(data, "failed to add handle to multi");
      discard_newhandle(cf, newhandle);
      rv = CURL_PUSH_DENY;
      اذهب fail;
    }

    result = http2_data_setup(cf, newhandle, &newstream);
    ܐܢ(result) {
      failf(data, "error setting up stream: %d", (صحيح)result);
      discard_newhandle(cf, newhandle);
      rv = CURL_PUSH_DENY;
      اذهب fail;
    }

    DEBUGASSERT(newstream);
    newstream->id = frame->promised_stream_id;
    newhandle->req.maxdownload = -1;
    newhandle->req.size = -1;

    CURL_TRC_CF(data, cf, "promise easy handle added to multi, mid=%u",
                newhandle->mid);
    rv = Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, newstream->id, newstream);
    ܐܢ(rv) {
      infof(data, "failed to set user_data for stream %d",
            newstream->id);
      DEBUGASSERT(0);
      discard_newhandle(cf, newhandle);
      rv = CURL_PUSH_DENY;
      اذهب fail;
    }

    /* success, remember max stream id processed */
    ܐܢ(newstream->id > ctx->local_max_sid)
      ctx->local_max_sid = newstream->id;
  }
  ܐܚܪܢܐ {
    CURL_TRC_CF(data, cf, "Got PUSH_PROMISE, ignore it");
    rv = CURL_PUSH_DENY;
  }
fail:
  ܦܢܐ rv;
}

ساكن فراغ h2_xfer_write_resp_hd(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data,
                                  بنية h2_stream_ctx *stream,
                                  ثابت حرف *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  ܐܢ(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, blen, eos);
    ܐܢ(!stream->xfer_result && !eos)
      stream->xfer_result = cf_h2_update_local_win(cf, data, stream);
    ܐܢ(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%d] error %d writing %zu bytes of headers",
                  stream->id, (صحيح)stream->xfer_result, blen);
  }
}

ساكن فراغ h2_xfer_write_resp(بنية Curl_cfilter *cf,
                               بنية Curl_easy *data,
                               بنية h2_stream_ctx *stream,
                               ثابت حرف *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  ܐܢ(!stream->xfer_result)
    stream->xfer_result = Curl_xfer_write_resp(data, buf, blen, eos);
  /* If the transfer write is errored, we do not want any more data */
  ܐܢ(stream->xfer_result) {
    بنية cf_h2_ctx *ctx = cf->ctx;
    CURL_TRC_CF(data, cf, "[%d] error %d writing %zu bytes of data, "
                "RST-ing stream",
                stream->id, (صحيح)stream->xfer_result, blen);
    Curl_ngh2_submit_rst_stream(
      &ctx->ngh2, 0, stream->id, (uint32_t)NGHTTP2_ERR_CALLBACK_FAILURE);
  }
  ܐܚܪܢܐ ܐܢ(!stream->write_paused && Curl_xfer_write_is_paused(data)) {
    CURL_TRC_CF(data, cf, "[%d] stream output paused", stream->id);
    stream->write_paused = TRUE;
  }
  ܐܚܪܢܐ ܐܢ(stream->write_paused && !Curl_xfer_write_is_paused(data)) {
    CURL_TRC_CF(data, cf, "[%d] stream output unpaused", stream->id);
    stream->write_paused = FALSE;
  }

  ܐܢ(!stream->xfer_result && !eos)
    stream->xfer_result = cf_h2_update_local_win(cf, data, stream);
}

ساكن CURLcode on_stream_frame(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data,
                                بنية h2_stream_ctx *stream,
                                ثابت nghttp2_frame *frame)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  int32_t stream_id = frame->hd.stream_id;
  صحيح rv;

  ܐܢ(!stream) {
    CURL_TRC_CF(data, cf, "[%d] No stream_ctx set", stream_id);
    ܦܢܐ CURLE_FAILED_INIT;
  }

  بدل(frame->hd.type) {
  حالة NGHTTP2_DATA:
    CURL_TRC_CF(data, cf, "[%d] DATA, window=%d/%d",
                stream_id,
                Curl_ngh2_session_get_stream_effective_recv_data_length(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_stream_effective_local_window_size(
                  &ctx->ngh2, stream->id));
    /* If !body started on this stream, then receiving DATA is illegal. */
    ܐܢ(!stream->bodystarted) {
      ܐܢ(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                       stream_id, NGHTTP2_PROTOCOL_ERROR))) {
        ܦܢܐ CURLE_RECV_ERROR;
      }
    }
    اكسر;
  حالة NGHTTP2_HEADERS:
    ܐܢ(stream->bodystarted) {
      /* Only valid HEADERS after body started is trailer HEADERS. We
         buffer them in on_header callback. */
      اكسر;
    }

    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    ܐܢ(stream->status_code == -1)
      ܦܢܐ CURLE_RECV_ERROR;

    /* Only final status code signals the end of header */
    ܐܢ(stream->status_code / 100 != 1) {
      stream->resp_hds_complete = TRUE;
      stream->bodystarted = TRUE;
    }
    ܐܚܪܢܐ
      stream->status_code = -1;

    h2_xfer_write_resp_hd(cf, data, stream, STRCONST("\r\n"),
                          (bool)stream->closed);

    Curl_multi_mark_dirty(data);
    اكسر;
  حالة NGHTTP2_PUSH_PROMISE:
    rv = push_promise(cf, data, &frame->push_promise);
    ܐܢ(rv) { /* deny! */
      DEBUGASSERT((rv > CURL_PUSH_OK) && (rv <= CURL_PUSH_ERROROUT));
      ܐܢ(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
             frame->push_promise.promised_stream_id, NGHTTP2_CANCEL)))
        ܦܢܐ CURLE_SEND_ERROR;
      ܐܚܪܢܐ ܐܢ(rv == CURL_PUSH_ERROROUT) {
        /* fail the parent transfer, not the whole session */
        CURL_TRC_CF(data, cf, "[%d] fail in PUSH_PROMISE received",
                    stream_id);
        ܐܢ(!stream->xfer_result)
          stream->xfer_result = CURLE_RECV_ERROR;
        Curl_multi_mark_dirty(data);
      }
    }
    اكسر;
  حالة NGHTTP2_RST_STREAM:
    ܐܢ(frame->rst_stream.error_code)
      stream->reset_by_server = TRUE;
    Curl_multi_mark_dirty(data);
    اكسر;
  حالة NGHTTP2_WINDOW_UPDATE:
    ܐܢ(CURL_REQ_WANT_SEND(data) && Curl_bufq_is_empty(&stream->sendbuf)) {
      /* need more data, force processing of transfer */
      Curl_multi_mark_dirty(data);
    }
    ܐܚܪܢܐ ܐܢ(!Curl_bufq_is_empty(&stream->sendbuf)) {
      /* resume the potentially suspended stream */
      ܐܢ(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id)))
        ܦܢܐ CURLE_SEND_ERROR;
    }
    اكسر;
  افتراضي:
    اكسر;
  }

  ܐܢ(frame->hd.flags & NGHTTP2_FLAG_END_STREAM) {
    ܐܢ(!stream->closed && !stream->body_eos &&
       ((stream->status_code >= 400) || (stream->status_code < 200))) {
      /* The server did not give us a positive response and we are not
       * done uploading the request body. We need to stop doing that and
       * also inform the server that we aborted our side. */
      CURL_TRC_CF(data, cf, "[%d] EOS frame with unfinished upload and "
                  "HTTP status %d, abort upload by RST",
                  stream_id, stream->status_code);
      Curl_ngh2_submit_rst_stream(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, NGHTTP2_STREAM_CLOSED);
      stream->closed = TRUE;
    }
    Curl_multi_mark_dirty(data);
  }
  ܦܢܐ CURLE_OK;
}

#ifdef CURLVERBOSE
صحيح Curl_nghttp2_fr_print(ثابت nghttp2_frame *frame, حرف *buffer,
                          size_t blen)
{
  بدل(frame->hd.type) {
  حالة NGHTTP2_DATA: {
    ܦܢܐ curl_msnprintf(buffer, blen,
                          "FRAME[DATA, len=%d, eos=%d, padlen=%d]",
                          (صحيح)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_STREAM),
                          (صحيح)frame->data.padlen);
  }
  حالة NGHTTP2_HEADERS: {
    ܦܢܐ curl_msnprintf(buffer, blen,
                          "FRAME[HEADERS, len=%d, hend=%d, eos=%d]",
                          (صحيح)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_HEADERS),
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_STREAM));
  }
  حالة NGHTTP2_PRIORITY: {
    ܦܢܐ curl_msnprintf(buffer, blen,
                          "FRAME[PRIORITY, len=%d, flags=%d]",
                          (صحيح)frame->hd.length, frame->hd.flags);
  }
  حالة NGHTTP2_RST_STREAM: {
    ܦܢܐ curl_msnprintf(buffer, blen,
                          "FRAME[RST_STREAM, len=%d, flags=%d, error=%u]",
                          (صحيح)frame->hd.length, frame->hd.flags,
                          frame->rst_stream.error_code);
  }
  حالة NGHTTP2_SETTINGS: {
    ܐܢ(frame->hd.flags & NGHTTP2_FLAG_ACK) {
      ܦܢܐ curl_msnprintf(buffer, blen, "FRAME[SETTINGS, ack=1]");
    }
    ܦܢܐ curl_msnprintf(buffer, blen,
                          "FRAME[SETTINGS, len=%d]", (صحيح)frame->hd.length);
  }
  حالة NGHTTP2_PUSH_PROMISE:
    ܦܢܐ curl_msnprintf(buffer, blen,
                          "FRAME[PUSH_PROMISE, len=%d, hend=%d]",
                          (صحيح)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_HEADERS));
  حالة NGHTTP2_PING:
    ܦܢܐ curl_msnprintf(buffer, blen,
                          "FRAME[PING, len=%d, ack=%d]",
                          (صحيح)frame->hd.length,
                          frame->hd.flags & NGHTTP2_FLAG_ACK);
  حالة NGHTTP2_GOAWAY: {
    حرف scratch[128];
    size_t s_len = CURL_ARRAYSIZE(scratch);
    size_t len = (frame->goaway.opaque_data_len < s_len) ?
      frame->goaway.opaque_data_len : s_len - 1;
    ܐܢ(len)
      memcpy(scratch, frame->goaway.opaque_data, len);
    scratch[len] = '\0';
    ܦܢܐ curl_msnprintf(buffer, blen,
                          "FRAME[GOAWAY, error=%u, reason='%s', "
                          "last_stream=%d]", frame->goaway.error_code,
                          scratch, frame->goaway.last_stream_id);
  }
  حالة NGHTTP2_WINDOW_UPDATE: {
    ܦܢܐ curl_msnprintf(buffer, blen,
                          "FRAME[WINDOW_UPDATE, incr=%d]",
                          frame->window_update.window_size_increment);
  }
  افتراضي:
    ܦܢܐ curl_msnprintf(buffer, blen, "FRAME[%d, len=%d, flags=%d]",
                          frame->hd.type, (صحيح)frame->hd.length,
                          frame->hd.flags);
  }
}

ساكن صحيح on_frame_send(nghttp2_session *session, ثابت nghttp2_frame *frame,
                         فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية Curl_easy *data = CF_DATA_CURRENT(cf);

  (فراغ)session;
  DEBUGASSERT(data);
  ܐܢ(Curl_trc_cf_is_verbose(cf, data)) {
    حرف buffer[256];
    صحيح len;
    len = Curl_nghttp2_fr_print(frame, buffer, حجم(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] -> %s", frame->hd.stream_id, buffer);
  }
  ܐܢ((frame->hd.type == NGHTTP2_GOAWAY) && !ctx->sent_goaway) {
    /* A GOAWAY not initiated by us, but by nghttp2 itself on detecting
     * a protocol error on the connection */
    failf(data, "nghttp2 shuts down connection with error %u: %s",
          frame->goaway.error_code,
          nghttp2_http2_strerror(frame->goaway.error_code));
  }
  ܦܢܐ 0;
}
#endif /* CURLVERBOSE */

ساكن صحيح on_frame_recv(nghttp2_session *session, ثابت nghttp2_frame *frame,
                         فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية Curl_easy *data = CF_DATA_CURRENT(cf), *data_s;
  بنية h2_stream_ctx *stream;
  int32_t stream_id = frame->hd.stream_id;

  (فراغ)session;
  DEBUGASSERT(data);
#ifdef CURLVERBOSE
  ܐܢ(Curl_trc_cf_is_verbose(cf, data)) {
    حرف buffer[256];
    صحيح len;
    len = Curl_nghttp2_fr_print(frame, buffer, حجم(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] <- %s", frame->hd.stream_id, buffer);
  }
#endif /* CURLVERBOSE */

  ܐܢ(!stream_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(data);
    بدل(frame->hd.type) {
    حالة NGHTTP2_SETTINGS: {
      ܐܢ(!(frame->hd.flags & NGHTTP2_FLAG_ACK)) {
        uint32_t max_conn = ctx->max_concurrent_streams;
        ctx->max_concurrent_streams =
          Curl_ngh2_session_get_remote_settings(
            &ctx->ngh2, NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS);
        ctx->enable_push = Curl_ngh2_session_get_remote_settings(
          &ctx->ngh2, NGHTTP2_SETTINGS_ENABLE_PUSH) != 0;
        CURL_TRC_CF(data, cf, "[0] MAX_CONCURRENT_STREAMS: %u",
                    ctx->max_concurrent_streams);
        CURL_TRC_CF(data, cf, "[0] ENABLE_PUSH: %s",
                    ctx->enable_push ? "TRUE" : "false");
        ܐܢ(data && max_conn != ctx->max_concurrent_streams) {
          /* only signal change if the value actually changed */
          CURL_TRC_CF(data, cf, "[0] notify MAX_CONCURRENT_STREAMS: %u",
                      ctx->max_concurrent_streams);
          Curl_multi_connchanged(data->multi);
        }
        /* Since the initial stream window is 64K, a request might be on HOLD,
         * due to exhaustion. The (initial) SETTINGS may announce a much larger
         * window and *assume* that we treat this like a WINDOW_UPDATE. Some
         * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
         * To be safe, we UNHOLD a stream in order not to stall. */
        ܐܢ(CURL_REQ_WANT_SEND(data))
          Curl_multi_mark_dirty(data);
      }
      اكسر;
    }
    حالة NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->goaway_error = frame->goaway.error_code;
      ctx->remote_max_sid = frame->goaway.last_stream_id;
      ܐܢ(data) {
        infof(data, "received GOAWAY, error=%u, last_stream=%d",
              ctx->goaway_error, ctx->remote_max_sid);
        Curl_multi_connchanged(data->multi);
      }
      اكسر;
    افتراضي:
      اكسر;
    }
    ܦܢܐ 0;
  }

  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  ܐܢ(!data_s) {
    CURL_TRC_CF(data, cf, "[%d] No Curl_easy associated", stream_id);
    ܦܢܐ 0;
  }

  ܦܢܐ on_stream_frame(cf, data_s, stream, frame) ?
         NGHTTP2_ERR_CALLBACK_FAILURE : 0;
}

ساكن صحيح cf_h2_on_invalid_frame_recv(nghttp2_session *session,
                                       ثابت nghttp2_frame *frame,
                                       صحيح ngerr, فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية Curl_easy *data;
  بنية h2_stream_ctx *stream;
  int32_t stream_id = frame->hd.stream_id;

  (فراغ)session;
  data = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  ܐܢ(data) {
#ifdef CURLVERBOSE
    حرف buffer[256];
    صحيح len;
    len = Curl_nghttp2_fr_print(frame, buffer, حجم(buffer) - 1);
    buffer[len] = 0;
    failf(data, "[HTTP2] [%d] received invalid frame: %s, error %d: %s",
          stream_id, buffer, ngerr, nghttp2_strerror(ngerr));
#endif /* CURLVERBOSE */
    ܐܢ(stream) {
      Curl_ngh2_submit_rst_stream(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, NGHTTP2_STREAM_CLOSED);
      stream->error = ngerr;
      stream->closed = TRUE;
      stream->reset = TRUE;
    }
  }
  /* Return no error, nghttp2 will RST/GOAWAY by itself when needed */
  ܦܢܐ 0;
}

ساكن صحيح on_data_chunk_recv(nghttp2_session *session, uint8_t flags,
                              int32_t stream_id,
                              ثابت uint8_t *mem, size_t len, فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_stream_ctx *stream;
  بنية Curl_easy *data_s;

  (فراغ)session;
  (فراغ)flags;

  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */
  DEBUGASSERT(CF_DATA_CURRENT(cf));

  /* get the stream from the hash based on Stream ID */
  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  ܐܢ(!data_s) {
    /* Receiving a Stream ID not in the hash should not happen - unless
       we have aborted a transfer artificially and there were more data
       in the pipeline. Silently ignore. */
    CURL_TRC_CF(CF_DATA_CURRENT(cf), cf, "[%d] Data for unknown", stream_id);
    /* consumed explicitly as no one will read it */
    Curl_ngh2_session_consume(&ctx->ngh2, stream_id, len);
    ܦܢܐ 0;
  }

  ܐܢ(!stream)
    ܦܢܐ NGHTTP2_ERR_CALLBACK_FAILURE;

  h2_xfer_write_resp(cf, data_s, stream, (ثابت حرف *)mem, len, FALSE);

  Curl_ngh2_session_consume(&ctx->ngh2, stream_id, len);
  stream->nrcvd_data += (curl_off_t)len;
  ܦܢܐ 0;
}

ساكن صحيح on_stream_close(nghttp2_session *session, int32_t stream_id,
                           uint32_t error_code, فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية Curl_easy *data_s, *call_data = CF_DATA_CURRENT(cf);
  بنية h2_stream_ctx *stream = عدم;
  صحيح rv;
  (فراغ)session;

  DEBUGASSERT(call_data);
  /* stream id 0 is the connection, do not look there for streams. */
  data_s = stream_id ? cf_h2_get_stream_xfer(cf, stream_id, &stream) : عدم;
  ܐܢ(!data_s) {
    CURL_TRC_CF(call_data, cf,
                "[%d] on_stream_close, no easy set on stream", stream_id);
    ܦܢܐ 0;
  }
  ܐܢ(!GOOD_EASY_HANDLE(data_s)) {
    /* nghttp2 still has an easy registered for the stream which has
     * been freed be libcurl. This points to a code path that does not
     * trigger DONE or DETACH events as it must. */
    CURL_TRC_CF(call_data, cf,
                "[%d] on_stream_close, not a GOOD easy on stream", stream_id);
    (فراغ)Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, stream_id, عدم);
    ܦܢܐ NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  ܐܢ(!stream) {
    CURL_TRC_CF(data_s, cf,
                "[%d] on_stream_close, GOOD easy but no stream", stream_id);
    ܦܢܐ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  stream->closed = TRUE;
  stream->error = error_code;
  ܐܢ(stream->error) {
    stream->reset = TRUE;
    ܐܢ((stream->error == NGHTTP2_REFUSED_STREAM) &&
       ctx->rcvd_goaway && ctx->goaway_error)
    /* REFUSED_STREAM is used by nghttp2 on a GOAWAY from the
     * server where it indicated it will no longer process this stream.
     * If the GOAWAY carried an error code, take that as the error for
     * the stream. */
    stream->error = ctx->goaway_error;
  }

  ܐܢ(stream->error)
    CURL_TRC_CF(data_s, cf, "[%d] RESET: %s (err %u)",
              stream_id, nghttp2_http2_strerror(error_code), error_code);
  ܐܚܪܢܐ
    CURL_TRC_CF(data_s, cf, "[%d] CLOSED", stream_id);
  Curl_multi_mark_dirty(data_s);

  /* remove `stream_ctx` from the nghttp2 stream */
  rv = Curl_ngh2_session_set_stream_user_data(
    &ctx->ngh2, stream_id, عدم);
  ܐܢ(rv) {
    infof(data_s, "http/2: failed to clear user_data for stream %d",
          stream_id);
    DEBUGASSERT(0);
  }
  ܦܢܐ 0;
}

ساكن فراغ cf_h2_header_error(بنية Curl_cfilter *cf,
                               بنية Curl_easy *data,
                               بنية h2_stream_ctx *stream,
                               CURLcode result)
{
  بنية cf_h2_ctx *ctx = cf->ctx;

  failf(data, "Error receiving HTTP2 header: %d(%s)", (صحيح)result,
        curl_easy_strerror(result));
  ܐܢ(stream) {
    Curl_ngh2_submit_rst_stream(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, NGHTTP2_STREAM_CLOSED);
    stream->closed = TRUE;
    stream->reset = TRUE;
  }
}

/* frame->hd.type is either NGHTTP2_HEADERS or NGHTTP2_PUSH_PROMISE */
ساكن صحيح on_header(nghttp2_session *session, ثابت nghttp2_frame *frame,
                     ثابت uint8_t *name, size_t namelen,
                     ثابت uint8_t *value, size_t valuelen,
                     uint8_t flags,
                     فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_stream_ctx *stream;
  بنية Curl_easy *data;
  int32_t stream_id = frame->hd.stream_id;
  CURLcode result;

  (فراغ)session;
  (فراغ)flags;

  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */

  /* get the stream from the hash based on Stream ID */
  data = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  ܐܢ(!GOOD_EASY_HANDLE(data))
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    ܦܢܐ NGHTTP2_ERR_CALLBACK_FAILURE;

  ܐܢ(!stream) {
    failf(data, "Internal NULL stream");
    ܦܢܐ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  /* Store received PUSH_PROMISE headers to be used when the subsequent
     PUSH_PROMISE callback comes */
  ܐܢ(frame->hd.type == NGHTTP2_PUSH_PROMISE) {
    حرف *h;

    ܐܢ((namelen == CURL_CSTRLEN(HTTP_PSEUDO_AUTHORITY)) &&
       !strncmp(HTTP_PSEUDO_AUTHORITY, (ثابت حرف *)name, namelen)) {
      /* pseudo headers are lower case */
      صحيح rc = 0;
      حرف *check = curl_maprintf("%s:%d", data->state.origin->hostname,
                                  data->state.origin->port);
      ܐܢ(!check)
        /* no memory */
        ܦܢܐ NGHTTP2_ERR_CALLBACK_FAILURE;
      ܐܢ(!curl_strequal(check, (ثابت حرف *)value) &&
         ((data->state.origin->port != data->state.origin->scheme->defport) ||
          !curl_strequal(data->state.origin->hostname, (ثابت حرف *)value))) {
        /* This is push is not for the same authority that was asked for in
         * the URL. RFC 7540 section 8.2 says: "A client MUST treat a
         * PUSH_PROMISE for which the server is not authoritative as a stream
         * error of type PROTOCOL_ERROR."
         */
        (فراغ)Curl_ngh2_submit_rst_stream(
          &ctx->ngh2, NGHTTP2_FLAG_NONE, stream_id, NGHTTP2_PROTOCOL_ERROR);
        rc = NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      curlx_free(check);
      ܐܢ(rc)
        ܦܢܐ rc;
    }

    ܐܢ(!stream->push_headers) {
      stream->push_headers_alloc = 10;
      stream->push_headers = curlx_malloc(stream->push_headers_alloc *
                                          حجم(حرف *));
      ܐܢ(!stream->push_headers)
        ܦܢܐ NGHTTP2_ERR_CALLBACK_FAILURE;
      stream->push_headers_used = 0;
    }
    ܐܚܪܢܐ ܐܢ(stream->push_headers_used ==
            stream->push_headers_alloc) {
      حرف **headp;
      ܐܢ(stream->push_headers_alloc > 1000) {
        /* this is beyond crazy many headers, bail out */
        failf(data, "Too many PUSH_PROMISE headers");
        free_push_headers(stream);
        ܦܢܐ NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      stream->push_headers_alloc *= 2;
      headp = curlx_realloc(stream->push_headers,
                            stream->push_headers_alloc * حجم(حرف *));
      ܐܢ(!headp) {
        free_push_headers(stream);
        ܦܢܐ NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      stream->push_headers = headp;
    }
    h = curl_maprintf("%s:%s", name, value);
    ܐܢ(!h) {
      free_push_headers(stream);
      ܦܢܐ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    stream->push_headers[stream->push_headers_used++] = h;
    ܦܢܐ 0;
  }

  ܐܢ(stream->bodystarted) {
    /* This is a trailer */
    CURL_TRC_CF(data, cf, "[%d] trailer: %.*s: %.*s",
                stream->id, (صحيح)namelen, name, (صحيح)valuelen, value);
    result = Curl_dynhds_add(&stream->resp_trailers,
                             (ثابت حرف *)name, namelen,
                             (ثابت حرف *)value, valuelen);
    ܐܢ(result) {
      cf_h2_header_error(cf, data, stream, result);
      ܦܢܐ NGHTTP2_ERR_CALLBACK_FAILURE;
    }

    ܦܢܐ 0;
  }

  ܐܢ(namelen == CURL_CSTRLEN(HTTP_PSEUDO_STATUS) &&
     !memcmp(HTTP_PSEUDO_STATUS, name, namelen)) {
    /* nghttp2 guarantees :status is received first and only once. */
    حرف buffer[32];
    size_t hlen;
    result = Curl_http_decode_status(&stream->status_code,
                                     (ثابت حرف *)value, valuelen);
    ܐܢ(result) {
      cf_h2_header_error(cf, data, stream, result);
      ܦܢܐ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    hlen = curl_msnprintf(buffer, حجم(buffer), HTTP_PSEUDO_STATUS ":%d\r",
                          stream->status_code);
    result = Curl_headers_push(data, buffer, hlen, CURLH_PSEUDO);
    ܐܢ(result) {
      cf_h2_header_error(cf, data, stream, result);
      ܦܢܐ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    curlx_dyn_reset(&ctx->scratch);
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/2 "));
    ܐܢ(!result)
      result = curlx_dyn_addn(&ctx->scratch, value, valuelen);
    ܐܢ(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    ܐܢ(!result)
      h2_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    ܐܢ(result) {
      cf_h2_header_error(cf, data, stream, result);
      ܦܢܐ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    /* if we receive data for another handle, wake that up */
    ܐܢ(CF_DATA_CURRENT(cf) != data)
      Curl_multi_mark_dirty(data);

    CURL_TRC_CF(data, cf, "[%d] status: HTTP/2 %03d",
                stream->id, stream->status_code);
    ܦܢܐ 0;
  }

  /* nghttp2 guarantees that namelen > 0, and :status was already
     received, and this is not pseudo-header field . */
  /* convert to an HTTP1-style header */
  curlx_dyn_reset(&ctx->scratch);
  result = curlx_dyn_addn(&ctx->scratch, (ثابت حرف *)name, namelen);
  ܐܢ(!result)
    result = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
  ܐܢ(!result)
    result = curlx_dyn_addn(&ctx->scratch, (ثابت حرف *)value, valuelen);
  ܐܢ(!result)
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
  ܐܢ(!result)
    h2_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                          curlx_dyn_len(&ctx->scratch), FALSE);
  ܐܢ(result) {
    cf_h2_header_error(cf, data, stream, result);
    ܦܢܐ NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  /* if we receive data for another handle, wake that up */
  ܐܢ(CF_DATA_CURRENT(cf) != data)
    Curl_multi_mark_dirty(data);

  CURL_TRC_CF(data, cf, "[%d] header: %.*s: %.*s",
              stream->id, (صحيح)namelen, name, (صحيح)valuelen, value);

  ܦܢܐ 0; /* 0 is successful */
}

ساكن ssize_t req_body_read_callback(nghttp2_session *session,
                                      int32_t stream_id,
                                      uint8_t *buf, size_t length,
                                      uint32_t *data_flags,
                                      nghttp2_data_source *source,
                                      فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية Curl_easy *data_s;
  بنية h2_stream_ctx *stream = عدم;
  CURLcode result;
  ssize_t nread;
  size_t n;
  (فراغ)source;

  (فراغ)session;
  ܐܢ(!stream_id)
    ܦܢܐ NGHTTP2_ERR_INVALID_ARGUMENT;

  /* get the stream from the hash based on Stream ID, stream ID zero is for
     connection-oriented stuff */
  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  ܐܢ(!data_s || !stream)
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    ܦܢܐ NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_bufq_read(&stream->sendbuf, buf, length, &n);
  ܐܢ(result) {
    ܐܢ(result != CURLE_AGAIN)
      ܦܢܐ NGHTTP2_ERR_CALLBACK_FAILURE;
    nread = 0;
  }
  ܐܚܪܢܐ
    nread = (ssize_t)n;

  CURL_TRC_CF(data_s, cf, "[%d] req_body_read(len=%zu) eos=%d -> %zd, %d",
              stream_id, length, stream->body_eos, nread, (صحيح)result);

  ܐܢ(stream->body_eos && Curl_bufq_is_empty(&stream->sendbuf)) {
    *data_flags = NGHTTP2_DATA_FLAG_EOF;
    ܦܢܐ nread;
  }
  ܦܢܐ (nread == 0) ? NGHTTP2_ERR_DEFERRED : nread;
}

#ifdef CURLVERBOSE
ساكن صحيح error_callback(nghttp2_session *session,
                          ثابت حرف *msg,
                          size_t len,
                          فراغ *userp)
{
  بنية Curl_cfilter *cf = userp;
  بنية Curl_easy *data = CF_DATA_CURRENT(cf);
  (فراغ)session;
  failf(data, "%.*s", (صحيح)len, msg);
  ܦܢܐ 0;
}
#endif

/*
 * Append headers to ask for an HTTP1.1 to HTTP2 upgrade.
 */
CURLcode Curl_http2_request_upgrade(بنية dynbuf *req,
                                    بنية Curl_easy *data)
{
  CURLcode result;
  حرف *base64;
  size_t blen;
  بنية SingleRequest *k = &data->req;
  uint8_t binsettings[H2_BINSETTINGS_LEN];
  ssize_t rc;
  size_t binlen; /* length of the binsettings data */

  rc = populate_binsettings(binsettings, data);
  ܐܢ(!curlx_sztouz(rc, &binlen) || !binlen) {
    failf(data, "nghttp2 unexpectedly failed on pack_settings_payload");
    curlx_dyn_free(req);
    ܦܢܐ CURLE_FAILED_INIT;
  }

  result = curlx_base64url_encode(binsettings, binlen, &base64, &blen);
  ܐܢ(result) {
    curlx_dyn_free(req);
    ܦܢܐ result;
  }

  data->state.http_hd_upgrade = TRUE;
  data->state.http_hd_h2_settings = TRUE;
  result = curlx_dyn_addf(req,
                          "Upgrade: %s\r\n"
                          "HTTP2-Settings: %s\r\n",
                          NGHTTP2_CLEARTEXT_PROTO_VERSION_ID, base64);
  curlx_free(base64);

  k->upgr101 = UPGR101_H2;
  data->conn->bits.upgrade_in_progress = TRUE;

  ܦܢܐ result;
}

ساكن CURLcode http2_handle_stream_close(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *data,
                                          بنية h2_stream_ctx *stream,
                                          size_t *pnlen)
{
  CURLcode result;

  *pnlen = 0;
  ܐܢ(stream->reset) {
    ܐܢ(stream->error == NGHTTP2_REFUSED_STREAM) {
      infof(data, "HTTP/2 stream %d refused by server, try again on a new "
                  "connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      ܦܢܐ CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    ܐܚܪܢܐ ܐܢ(stream->resp_hds_complete && data->req.no_body) {
      CURL_TRC_CF(data, cf, "[%d] error after response headers, but we did "
                  "not want a body anyway, ignore: %s (err %u)",
                  stream->id, nghttp2_http2_strerror(stream->error),
                  stream->error);
      stream->close_handled = TRUE;
      ܦܢܐ CURLE_OK;
    }
    failf(data, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          stream->id, stream->reset_by_server ? "server" : "curl",
          stream->error, nghttp2_http2_strerror(stream->error));
    ܦܢܐ stream->error ? CURLE_HTTP2_STREAM :
           (data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2);
  }
  ܐܚܪܢܐ ܐܢ(!stream->bodystarted) {
    failf(data, "HTTP/2 stream %d was closed cleanly, but before getting "
          "all response header fields, treated as error", stream->id);
    ܦܢܐ CURLE_HTTP2_STREAM;
  }

  ܐܢ(Curl_dynhds_count(&stream->resp_trailers)) {
    بنية dynhds_entry *e;
    بنية dynbuf dbuf;
    size_t i;

    result = CURLE_OK;
    curlx_dyn_init(&dbuf, DYN_TRAILERS);
    لكل(i = 0; i < Curl_dynhds_count(&stream->resp_trailers); ++i) {
      e = Curl_dynhds_getn(&stream->resp_trailers, i);
      ܐܢ(!e)
        اكسر;
      curlx_dyn_reset(&dbuf);
      result = curlx_dyn_addf(&dbuf, "%.*s: %.*s\x0d\x0a",
                              (صحيح)e->namelen, e->name,
                              (صحيح)e->valuelen, e->value);
      ܐܢ(result)
        اكسر;
      Curl_debug(data, CURLINFO_HEADER_IN, curlx_dyn_ptr(&dbuf),
                 curlx_dyn_len(&dbuf));
      result = Curl_client_write(data,
                                 CLIENTWRITE_HEADER | CLIENTWRITE_TRAILER,
                                 curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf));
      ܐܢ(result)
        اكسر;
      result = Curl_bump_headersize(data, curlx_dyn_len(&dbuf), FALSE);
      ܐܢ(result)
        ܦܢܐ result;
    }
    curlx_dyn_free(&dbuf);
    ܐܢ(result)
      اذهب out;
  }

  stream->close_handled = TRUE;
  result = CURLE_OK;

out:
  CURL_TRC_CF(data, cf, "handle_stream_close -> %d, %zu", (صحيح)result, *pnlen);
  ܦܢܐ result;
}

ساكن صحيح sweight_wanted(ثابت بنية Curl_easy *data)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  ܦܢܐ data->set.weight ?
    data->set.weight : NGHTTP2_DEFAULT_WEIGHT;
}

ساكن صحيح sweight_in_effect(ثابت بنية Curl_easy *data)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  ܦܢܐ data->state.weight ?
    data->state.weight : NGHTTP2_DEFAULT_WEIGHT;
}

/*
 * h2_pri_spec() fills in the pri_spec struct, used by nghttp2 to send weight
 * and dependency to the peer. It also stores the updated values in the state
 * struct.
 */

ساكن فراغ h2_pri_spec(بنية Curl_easy *data,
                        nghttp2_priority_spec *pri_spec)
{
  صحيح prio = data->set.weight;
  nghttp2_priority_spec_init(pri_spec, 0, sweight_wanted(data), FALSE);
  data->state.weight = prio;
}

/*
 * Check if there is been an update in the priority /
 * dependency settings and if so it submits a PRIORITY frame with the updated
 * info.
 * Flush any out data pending in the network buffer.
 */
ساكن CURLcode h2_progress_egress(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *data)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  صحيح rv = 0;

  ܐܢ(sweight_wanted(data) != sweight_in_effect(data)) {
    بنية h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    ܐܢ(stream && stream->id > 0 &&
       (sweight_wanted(data) != sweight_in_effect(data))) {
      /* send new weight and/or dependency */
      nghttp2_priority_spec pri_spec;

      h2_pri_spec(data, &pri_spec);
      CURL_TRC_CF(data, cf, "[%d] Queuing PRIORITY", stream->id);
      DEBUGASSERT(stream->id != -1);
      rv = Curl_ngh2_submit_priority(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, &pri_spec);
      ܐܢ(rv)
        اذهب out;
    }
  }

  ctx->nw_out_blocked = 0;
  ܟܡܐ(!rv && !ctx->nw_out_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

out:
  ܐܢ(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(data, cf, "nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    ܦܢܐ CURLE_SEND_ERROR;
  }
  /* Defer flushing during the connect phase so that the SETTINGS and
   * other initial frames are sent together with the first request.
   * Unless we are 'connect_only' where the request will never come. */
  ܐܢ(!cf->connected && !cf->conn->bits.connect_only)
    ܦܢܐ CURLE_OK;
  ܦܢܐ nw_out_flush(cf, data);
}

ساكن CURLcode stream_recv(بنية Curl_cfilter *cf, بنية Curl_easy *data,
                            بنية h2_stream_ctx *stream,
                            حرف *buf, size_t len, size_t *pnread)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_AGAIN;

  (فراغ)buf;
  (فراغ)len;
  *pnread = 0;

  ܐܢ(!stream->xfer_result)
    stream->xfer_result = cf_h2_update_local_win(cf, data, stream);

  ܐܢ(stream->xfer_result) {
    CURL_TRC_CF(data, cf, "[%d] xfer write failed", stream->id);
    result = stream->xfer_result;
  }
  ܐܚܪܢܐ ܐܢ(stream->closed) {
    CURL_TRC_CF(data, cf, "[%d] returning CLOSE", stream->id);
    result = http2_handle_stream_close(cf, data, stream, pnread);
  }
  ܐܚܪܢܐ ܐܢ(stream->reset ||
          (ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) ||
          (ctx->rcvd_goaway && ctx->remote_max_sid < stream->id)) {
    CURL_TRC_CF(data, cf, "[%d] returning ERR", stream->id);
    result = data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2;
  }

  ܐܢ(result && (result != CURLE_AGAIN))
    CURL_TRC_CF(data, cf, "[%d] stream_recv(len=%zu) -> %d, %zu",
                stream->id, len, (صحيح)result, *pnread);
  ܦܢܐ result;
}

ساكن CURLcode h2_progress_ingress(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *data,
                                    size_t data_max_bytes)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  ܐܢ(should_close_session(ctx)) {
    CURL_TRC_CF(data, cf, "[0] ingress: session is closed");
    ܦܢܐ CURLE_HTTP2;
  }

  /* Process network input buffer first */
  ܐܢ(!Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "Process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    result = h2_process_pending_input(cf, data);
    ܐܢ(result)
      ܦܢܐ result;
  }

  ܐܢ(!data_max_bytes)
    data_max_bytes = H2_CHUNK_SIZE;

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop due to connection close or us not processing
   * all network input */
  ܟܡܐ(!ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) {
    بنية h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    ܐܢ(stream && (stream->closed || !data_max_bytes)) {
      /* We would like to abort here and stop processing, so that the transfer
       * loop can handle the data/close here. This may leave data in
       * underlying buffers that will not be consumed. */
      ܐܢ(!cf->next || !cf->next->cft->has_data_pending(cf->next, data))
        Curl_multi_mark_dirty(data);
      اكسر;
    }

    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    ܐܢ(result) {
      ܐܢ(result != CURLE_AGAIN) {
        failf(data, "Failed receiving HTTP2 data: %d(%s)", (صحيح)result,
              curl_easy_strerror(result));
        ܦܢܐ result;
      }
      اكسر;
    }
    ܐܚܪܢܐ ܐܢ(nread == 0) {
      CURL_TRC_CF(data, cf, "[0] ingress: connection closed");
      ctx->conn_closed = TRUE;
      اكسر;
    }
    ܐܚܪܢܐ {
      CURL_TRC_CF(data, cf, "[0] ingress: read %zu bytes", nread);
      data_max_bytes = (data_max_bytes > nread) ? (data_max_bytes - nread) : 0;
    }

    result = h2_process_pending_input(cf, data);
    ܐܢ(result)
      ܦܢܐ result;
    CURL_TRC_CF(data, cf, "[0] ingress: nw-in buffered %zu",
                Curl_bufq_len(&ctx->inbufq));
  }

  ܐܢ(ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "server closed %s",
                ctx->rcvd_goaway ? "with GOAWAY" : "abruptly");
    connclose(cf->conn);
  }

  CURL_TRC_CF(data, cf, "[0] ingress: done");
  ܦܢܐ CURLE_OK;
}

ساكن CURLcode cf_h2_recv(بنية Curl_cfilter *cf, بنية Curl_easy *data,
                           حرف *buf, size_t len, size_t *pnread)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_stream_ctx *stream;
  CURLcode result, r2;
  بنية cf_call_data save;

  ܐܢ(!data)
    ܦܢܐ CURLE_HTTP2;

  stream = H2_STREAM_CTX(ctx, data);

  *pnread = 0;
  ܐܢ(!stream) {
    /* Abnormal call sequence: either this transfer has never opened a stream
     * (unlikely) or the transfer has been done, cleaned up its resources, but
     * a read() is called anyway. It is not clear what the calling sequence
     * is for such a case. */
    failf(data, "http/2 recv on a transfer never opened "
          "or already cleared, mid=%u", data->mid);
    ܦܢܐ CURLE_HTTP2;
  }

  CF_DATA_SAVE(save, cf, data);

  result = stream_recv(cf, data, stream, buf, len, pnread);
  ܐܢ(result && (result != CURLE_AGAIN))
    اذهب out;

  ܐܢ(result) {
    result = h2_progress_ingress(cf, data, len);
    ܐܢ(result)
      اذهب out;

    result = stream_recv(cf, data, stream, buf, len, pnread);
  }

  ܐܢ(*pnread > 0) {
    /* Now that we transferred this to the upper layer, we report
     * the actual amount of DATA consumed to the H2 session, so
     * that it adjusts stream flow control */
    Curl_ngh2_session_consume(&ctx->ngh2, stream->id, *pnread);
    ܐܢ(stream->closed) {
      CURL_TRC_CF(data, cf, "[%d] DRAIN closed stream", stream->id);
      Curl_multi_mark_dirty(data);
    }
  }

out:
  r2 = h2_progress_egress(cf, data);
  ܐܢ(r2 == CURLE_AGAIN) {
    /* pending data to send, need to be called again. Ideally, we
     * monitor the socket for POLLOUT, but when not SENDING
     * any more, we force processing of the transfer. */
    ܐܢ(!CURL_REQ_WANT_SEND(data))
      Curl_multi_mark_dirty(data);
  }
  ܐܚܪܢܐ ܐܢ(r2 && (result == CURLE_AGAIN)) {
    /* Do not replace a previous error result NOR a successful result
     * with an egress error. Example: the server might close the
     * connection after sending the complete response with EOS
     * and everything. Further sending may fail, but that MUST
     * not change that we had a successful response. */
    result = r2;
  }

  CURL_TRC_CF(data, cf, "[%d] cf_recv(len=%zu) -> %d, %zu, "
              "window=%d/%d, connection %d/%d",
              stream->id, len, (صحيح)result, *pnread,
              Curl_ngh2_session_get_stream_effective_recv_data_length(
                &ctx->ngh2, stream->id),
              Curl_ngh2_session_get_stream_effective_local_window_size(
                &ctx->ngh2, stream->id),
              Curl_ngh2_session_get_local_window_size(&ctx->ngh2),
              HTTP2_HUGE_WINDOW_SIZE);

  CF_DATA_RESTORE(cf, save);
  ܦܢܐ result;
}

ساكن CURLcode cf_h2_body_send(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data,
                                بنية h2_stream_ctx *stream,
                                ثابت فراغ *buf, size_t blen, bool eos,
                                size_t *pnwritten)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  CURLcode result;

  *pnwritten = 0;
  ܐܢ(stream->closed) {
    ܐܢ(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(data, cf, "[%d] discarding data"
                  "on closed stream with response", stream->id);
      ܐܢ(eos)
        stream->body_eos = TRUE;
      *pnwritten = blen;
      ܦܢܐ CURLE_OK;
    }
    /* Server closed before we got a response, this is an error */
    infof(data, "stream %d closed", stream->id);
    ܦܢܐ CURLE_SEND_ERROR;
  }

  result = Curl_bufq_write(&stream->sendbuf, buf, blen, pnwritten);
  ܐܢ(result)
    ܦܢܐ result;

  ܐܢ(eos && (blen == *pnwritten))
    stream->body_eos = TRUE;

  ܐܢ(eos || !Curl_bufq_is_empty(&stream->sendbuf)) {
    /* resume the potentially suspended stream */
    ܐܢ(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id)))
      ܦܢܐ CURLE_SEND_ERROR;
  }

  ܦܢܐ CURLE_OK;
}

ساكن CURLcode h2_submit(بنية h2_stream_ctx **pstream,
                          بنية Curl_cfilter *cf, بنية Curl_easy *data,
                          ثابت uint8_t *buf, size_t len,
                          bool eos, size_t *pnwritten)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_stream_ctx *stream = عدم;
  بنية dynhds h2_headers;
  nghttp2_nv *nva = عدم;
  ثابت فراغ *body = عدم;
  size_t nheader, bodylen;
  nghttp2_data_provider data_prd;
  int32_t stream_id;
  nghttp2_priority_spec pri_spec;
  size_t nwritten;
  CURLcode result = CURLE_OK;
  uint32_t initial_win_size;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_headers, MAX_HTTP_RESP_HEADER_COUNT, DYN_HTTP_REQUEST);

  result = http2_data_setup(cf, data, &stream);
  ܐܢ(result)
    اذهب out;

  result = Curl_h1_req_parse_read(
    &stream->h1, buf, len, عدم,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : عدم,
    0, &nwritten);
  ܐܢ(result)
    اذهب out;
  *pnwritten = nwritten;
  ܐܢ(!stream->h1.done) {
    /* need more data */
    اذهب out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  ܐܢ(result)
    اذهب out;
  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nva = Curl_dynhds_to_nva(&h2_headers, &nheader);
  ܐܢ(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }

  h2_pri_spec(data, &pri_spec);
  ܐܢ(!Curl_ngh2_session_check_request_allowed(&ctx->ngh2))
    CURL_TRC_CF(data, cf, "send request NOT allowed (via nghttp2)");

  /* Check the initial windows size of the transfer (rate-limits?) and
   * send an updated settings on changes from previous value. */
  initial_win_size = cf_h2_initial_win_size(data);
  ܐܢ(initial_win_size != ctx->initial_win_size) {
    result = cf_h2_update_settings(ctx, initial_win_size);
    ܐܢ(result)
      اذهب out;
  }

  بدل(data->state.httpreq) {
  حالة HTTPREQ_POST:
  حالة HTTPREQ_POST_FORM:
  حالة HTTPREQ_POST_MIME:
  حالة HTTPREQ_PUT:
    data_prd.read_callback = req_body_read_callback;
    data_prd.source.ptr = عدم;
    stream_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, &data_prd, stream);
    اكسر;
  افتراضي:
    stream_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, عدم, stream);
  }

  ܐܢ(stream_id < 0) {
    CURL_TRC_CF(data, cf, "send: nghttp2_submit_request error (%s)%d",
                nghttp2_strerror(stream_id), stream_id);
    result = CURLE_SEND_ERROR;
    اذهب out;
  }

#ifdef CURLVERBOSE
#define MAX_ACC 60000  /* <64KB to account for some overhead */
  ܐܢ(Curl_trc_is_verbose(data)) {
    size_t acc = 0, i;

    infof(data, "[HTTP/2] [%d] OPENED stream for %s",
          stream_id, Curl_bufref_ptr(&data->state.url));
    لكل(i = 0; i < nheader; ++i) {
      acc += nva[i].namelen + nva[i].valuelen;

      infof(data, "[HTTP/2] [%d] [%.*s: %.*s]", stream_id,
            (صحيح)nva[i].namelen, nva[i].name,
            (صحيح)nva[i].valuelen, nva[i].value);
    }

    ܐܢ(acc > MAX_ACC) {
      infof(data, "[HTTP/2] Warning: The cumulative length of all "
            "headers exceeds %d bytes and that could cause the "
            "stream to be rejected.", MAX_ACC);
    }
  }
#endif

  stream->id = stream_id;

  body = (ثابت حرف *)buf + *pnwritten;
  bodylen = len - *pnwritten;

  ܐܢ(bodylen || eos) {
    size_t n;
    result = cf_h2_body_send(cf, data, stream, body, bodylen, eos, &n);
    ܐܢ(!result)
      *pnwritten += n;
    ܐܚܪܢܐ ܐܢ(result == CURLE_AGAIN)
      result = CURLE_OK;
    ܐܚܪܢܐ {
      result = CURLE_SEND_ERROR;
    }
  }

out:
  CURL_TRC_CF(data, cf, "[%d] submit -> %d, %zu",
              stream ? stream->id : -1, (صحيح)result, *pnwritten);
  curlx_safefree(nva);
  *pstream = stream;
  Curl_dynhds_free(&h2_headers);
  ܦܢܐ result;
}

ساكن CURLcode cf_h2_send(بنية Curl_cfilter *cf, بنية Curl_easy *data,
                           ثابت uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
  بنية cf_call_data save;
  CURLcode result = CURLE_OK, r2;

  CF_DATA_SAVE(save, cf, data);
  *pnwritten = 0;

  ܐܢ(!stream || stream->id == -1) {
    result = h2_submit(&stream, cf, data, buf, len, eos, pnwritten);
    ܐܢ(result)
      اذهب out;
    DEBUGASSERT(stream);
  }
  ܐܚܪܢܐ ܐܢ(stream->body_eos) {
    /* We already wrote this, but CURLE_AGAIN-ed the call due to not
     * being able to flush stream->sendbuf. Make a 0-length write
     * to trigger flushing again.
     * If this works, we report to have written `len` bytes. */
    size_t n;
    DEBUGASSERT(eos);
    result = cf_h2_body_send(cf, data, stream, buf, 0, eos, &n);
    CURL_TRC_CF(data, cf, "[%d] cf_body_send last CHUNK -> %d, %zu, eos=%d",
                stream->id, (صحيح)result, n, eos);
    ܐܢ(result)
      اذهب out;
    *pnwritten = len;
  }
  ܐܚܪܢܐ {
    result = cf_h2_body_send(cf, data, stream, buf, len, eos, pnwritten);
    CURL_TRC_CF(data, cf, "[%d] cf_body_send(len=%zu) -> %d, %zu, eos=%d",
                stream->id, len, (صحيح)result, *pnwritten, eos);
  }

  /* Call the nghttp2 send loop and flush to write ALL buffered data,
   * headers and/or request body completely out to the network */
  r2 = h2_progress_egress(cf, data);

  /* if the stream has been closed in egress handling (nghttp2 does that
   * when it does not like the headers, for example */
  ܐܢ(stream && stream->closed && !stream->resp_hds_complete) {
    infof(data, "stream %d closed", stream->id);
    result = CURLE_SEND_ERROR;
    اذهب out;
  }
  ܐܚܪܢܐ ܐܢ(r2 && (r2 != CURLE_AGAIN)) {
    result = r2;
    اذهب out;
  }

  ܐܢ(should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    ܐܢ(stream && stream->closed) {
      result = http2_handle_stream_close(cf, data, stream, pnwritten);
    }
    ܐܚܪܢܐ {
      CURL_TRC_CF(data, cf, "send: nothing to do in this session");
      result = CURLE_HTTP2;
    }
  }

out:
  ܐܢ(stream) {
    CURL_TRC_CF(data, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
                "eos=%d, h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                stream->id, len, (صحيح)result, *pnwritten,
                stream->body_eos,
                Curl_ngh2_session_get_stream_remote_window_size(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&stream->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  ܐܚܪܢܐ {
    CURL_TRC_CF(data, cf, "cf_send(len=%zu) -> %d, %zu, "
                "connection-window=%d, nw_send_buffer(%zu)",
                len, (صحيح)result, *pnwritten,
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_DATA_RESTORE(cf, save);
  ܦܢܐ result;
}

ساكن CURLcode cf_h2_flush(بنية Curl_cfilter *cf,
                            بنية Curl_easy *data)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
  بنية cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  ܐܢ(stream && !Curl_bufq_is_empty(&stream->sendbuf)) {
    /* resume the potentially suspended stream */
    ܐܢ(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id))) {
      result = CURLE_SEND_ERROR;
      اذهب out;
    }
  }

  result = h2_progress_egress(cf, data);

out:
  ܐܢ(stream) {
    CURL_TRC_CF(data, cf, "[%d] flush -> %d, "
                "h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                stream->id, (صحيح)result,
                Curl_ngh2_session_get_stream_remote_window_size(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&stream->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  ܐܚܪܢܐ {
    CURL_TRC_CF(data, cf, "flush -> %d, "
                "connection-window=%d, nw_send_buffer(%zu)", (صحيح)result,
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_DATA_RESTORE(cf, save);
  ܦܢܐ result;
}

ساكن CURLcode cf_h2_adjust_pollset(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *data,
                                     بنية easy_pollset *ps)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية cf_call_data save;
  curl_socket_t sock;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  ܐܢ(!Curl_ngh2_valid(&ctx->ngh2))
    ܦܢܐ CURLE_OK;

  sock = Curl_conn_cf_get_socket(cf, data);
  Curl_pollset_check(data, ps, sock, &want_recv, &want_send);
  ܐܢ(want_recv || want_send) {
    بنية h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    bool c_exhaust, s_exhaust;

    CF_DATA_SAVE(save, cf, data);
    c_exhaust = want_send &&
                !Curl_ngh2_session_get_remote_window_size(&ctx->ngh2);
    s_exhaust = want_send && stream && stream->id >= 0 &&
                !Curl_ngh2_session_get_stream_remote_window_size(
                  &ctx->ngh2, stream->id);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                (!c_exhaust && Curl_ngh2_session_want_write(&ctx->ngh2)) ||
                !Curl_bufq_is_empty(&ctx->outbufq);

    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CF_DATA_RESTORE(cf, save);
  }
  ܐܚܪܢܐ ܐܢ(ctx->sent_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_DATA_SAVE(save, cf, data);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CF_DATA_RESTORE(cf, save);
  }
  ܦܢܐ result;
}

ساكن CURLcode cf_h2_ctx_open(بنية Curl_cfilter *cf,
                               بنية Curl_easy *data)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_stream_ctx *stream;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  صحيح rc;
  nghttp2_session_callbacks *cbs = عدم;

  DEBUGASSERT(!ctx->ngh2.s);
  DEBUGASSERT(!ctx->ngh2.fatal_rv);
  DEBUGASSERT(ctx->initialized);

  rc = nghttp2_session_callbacks_new(&cbs);
  ܐܢ(rc) {
    failf(data, "Could not initialize nghttp2 callbacks");
    اذهب out;
  }

  nghttp2_session_callbacks_set_send_callback(cbs, send_callback);
  nghttp2_session_callbacks_set_on_frame_recv_callback(cbs, on_frame_recv);
  nghttp2_session_callbacks_set_on_invalid_frame_recv_callback(cbs,
    cf_h2_on_invalid_frame_recv);
#ifdef CURLVERBOSE
  nghttp2_session_callbacks_set_on_frame_send_callback(cbs, on_frame_send);
#endif
  nghttp2_session_callbacks_set_on_data_chunk_recv_callback(
    cbs, on_data_chunk_recv);
  nghttp2_session_callbacks_set_on_stream_close_callback(cbs, on_stream_close);
  nghttp2_session_callbacks_set_on_header_callback(cbs, on_header);
#ifdef CURLVERBOSE
  nghttp2_session_callbacks_set_error_callback(cbs, error_callback);
#endif

  /* The nghttp2 session is not yet setup, do it */
  rc = h2_client_new(cf, cbs);
  ܐܢ(rc) {
    failf(data, "Could not initialize nghttp2");
    اذهب out;
  }
  ctx->max_concurrent_streams = data->multi ?
    Curl_multi_max_concurrent_streams(data->multi) :
    DEFAULT_MAX_CONCURRENT_STREAMS;

  ܐܢ(ctx->via_h1_upgrade) {
    /* HTTP/1.1 Upgrade issued. H2 Settings have already been submitted
     * in the H1 request and we upgrade from there. This stream
     * is opened implicitly as #1. */
    uint8_t binsettings[H2_BINSETTINGS_LEN];
    ssize_t rclen;
    size_t binlen; /* length of the binsettings data */

    rclen = populate_binsettings(binsettings, data);

    ܐܢ(!curlx_sztouz(rclen, &binlen) || !binlen) {
      failf(data, "nghttp2 unexpectedly failed on pack_settings_payload");
      result = CURLE_FAILED_INIT;
      اذهب out;
    }

    result = http2_data_setup(cf, data, &stream);
    ܐܢ(result)
      اذهب out;
    DEBUGASSERT(stream);
    stream->id = 1;
    /* queue SETTINGS frame (again) */
    rc = Curl_ngh2_session_upgrade2(
      &ctx->ngh2, binsettings, binlen, data->state.httpreq == HTTPREQ_HEAD,
      عدم);
    ܐܢ(rc) {
      failf(data, "nghttp2_session_upgrade2() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      اذهب out;
    }

    rc = Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, stream->id, stream);
    ܐܢ(rc) {
      infof(data, "http/2: failed to set user_data for stream %d",
            stream->id);
      DEBUGASSERT(0);
    }
    CURL_TRC_CF(data, cf, "created session via Upgrade");
  }
  ܐܚܪܢܐ {
    nghttp2_settings_entry iv[H2_SETTINGS_IV_LEN];
    size_t ivlen;

    ivlen = populate_settings(iv, data, ctx);
    rc = Curl_ngh2_submit_settings(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, iv, ivlen);
    ܐܢ(rc) {
      failf(data, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      اذهب out;
    }
  }

  rc = Curl_ngh2_session_set_local_window_size(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, HTTP2_HUGE_WINDOW_SIZE);
  ܐܢ(rc) {
    failf(data, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    result = CURLE_HTTP2;
    اذهب out;
  }

  /* all set, traffic will be send on connect */
  result = CURLE_OK;
  CURL_TRC_CF(data, cf, "[0] created h2 session%s",
              ctx->via_h1_upgrade ? " (via h1 upgrade)" : "");

out:
  ܐܢ(cbs)
    nghttp2_session_callbacks_del(cbs);
  ܦܢܐ result;
}

ساكن CURLcode cf_h2_connect(بنية Curl_cfilter *cf,
                              بنية Curl_easy *data,
                              bool *done)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  بنية cf_call_data save;
  bool first_time = FALSE;

  ܐܢ(cf->connected) {
    *done = TRUE;
    ܦܢܐ CURLE_OK;
  }

  /* Connect the lower filters first */
  ܐܢ(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    ܐܢ(result || !*done)
      ܦܢܐ result;
  }

  *done = FALSE;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(ctx->initialized);
  ܐܢ(!ctx->ngh2.s) {
    result = cf_h2_ctx_open(cf, data);
    ܐܢ(result)
      اذهب out;
    first_time = TRUE;
  }

  ܐܢ(!first_time) {
    result = h2_progress_ingress(cf, data, 0);
    ܐܢ(result)
      اذهب out;
  }

  /* Send out our SETTINGS and ACKs and such. If that blocks, we
   * have it buffered and can count this filter as being connected */
  result = h2_progress_egress(cf, data);
  ܐܢ(result && (result != CURLE_AGAIN))
    اذهب out;

  *done = TRUE;
  cf->connected = TRUE;
  result = CURLE_OK;

out:
  CURL_TRC_CF(data, cf, "cf_connect() -> %d, %d, ", (صحيح)result, *done);
  CF_DATA_RESTORE(cf, save);
  ܦܢܐ result;
}

ساكن فراغ cf_h2_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *data)
{
  بنية cf_h2_ctx *ctx = cf->ctx;

  (فراغ)data;
  ܐܢ(ctx) {
    cf_h2_ctx_free(ctx);
    cf->ctx = عدم;
  }
}

ساكن CURLcode cf_h2_shutdown(بنية Curl_cfilter *cf,
                               بنية Curl_easy *data, bool *done)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية cf_call_data save;
  CURLcode result;
  صحيح rv;

  ܐܢ(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *done = TRUE;
    ܦܢܐ CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);

  ܐܢ(!ctx->sent_goaway) {
    ctx->sent_goaway = TRUE;
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, ctx->local_max_sid, 0,
      (ثابت uint8_t *)"shutdown", حجم("shutdown"));
    ܐܢ(rv) {
      failf(data, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      result = CURLE_SEND_ERROR;
      اذهب out;
    }
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  result = CURLE_OK;
  ܐܢ(Curl_ngh2_session_want_write(&ctx->ngh2) ||
     !Curl_bufq_is_empty(&ctx->outbufq))
    result = h2_progress_egress(cf, data);
  ܐܢ(!result && Curl_ngh2_session_want_read(&ctx->ngh2))
    result = h2_progress_ingress(cf, data, 0);

  ܐܢ(result == CURLE_AGAIN)
    result = CURLE_OK;

  *done = (ctx->conn_closed ||
           (!result && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2) &&
            Curl_bufq_is_empty(&ctx->outbufq)));

out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  ܦܢܐ result;
}

ساكن CURLcode http2_data_pause(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data,
                                 bool pause)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);

  DEBUGASSERT(data);
  ܐܢ(ctx && Curl_ngh2_valid(&ctx->ngh2) && stream) {
    CURLcode result;

    stream->write_paused = pause;
    result = cf_h2_update_local_win(cf, data, stream);
    ܐܢ(result)
      ܦܢܐ result;

    /* attempt to send the window update */
    (فراغ)h2_progress_egress(cf, data);

    ܐܢ(!pause) {
      /* Unpausing a h2 transfer, requires it to be run again. The server
       * may send new DATA on us increasing the flow window, and it may
       * not. We may have already buffered and exhausted the new window
       * by operating on things in flight during the handling of other
       * transfers. */
      Curl_multi_mark_dirty(data);
    }
    CURL_TRC_CF(data, cf, "[%d] stream now %spaused", stream->id,
                pause ? "" : "un");
  }
  ܦܢܐ CURLE_OK;
}

ساكن CURLcode cf_h2_cntrl(بنية Curl_cfilter *cf,
                            بنية Curl_easy *data,
                            صحيح event, صحيح arg1, فراغ *arg2)
{
  CURLcode result = CURLE_OK;
  بنية cf_call_data save;

  (فراغ)arg2;

  CF_DATA_SAVE(save, cf, data);
  بدل(event) {
  حالة CF_CTRL_DATA_SETUP:
    اكسر;
  حالة CF_CTRL_DATA_PAUSE:
    result = http2_data_pause(cf, data, (arg1 != 0));
    اكسر;
  حالة CF_CTRL_FLUSH:
    result = cf_h2_flush(cf, data);
    اكسر;
  حالة CF_CTRL_DATA_DONE:
    http2_data_done(cf, data);
    اكسر;
  حالة CF_CTRL_CONN_INFO_UPDATE:
    ܐܢ(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 20;
      Curl_conn_set_multiplex(cf->conn);
    }
    اكسر;
  افتراضي:
    اكسر;
  }
  CF_DATA_RESTORE(cf, save);
  ܦܢܐ result;
}

ساكن bool cf_h2_data_pending(بنية Curl_cfilter *cf,
                               ثابت بنية Curl_easy *data)
{
  بنية cf_h2_ctx *ctx = cf->ctx;

  ܐܢ(ctx && !Curl_bufq_is_empty(&ctx->inbufq))
    ܦܢܐ TRUE;
  ܦܢܐ cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

ساكن bool cf_h2_is_alive(بنية Curl_cfilter *cf,
                           بنية Curl_easy *data,
                           bool *input_pending)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  bool alive;
  بنية cf_call_data save;

  *input_pending = FALSE;
  CF_DATA_SAVE(save, cf, data);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
           http2_connisalive(cf, data, input_pending));
  CURL_TRC_CF(data, cf, "conn alive -> %d, input_pending=%d",
              alive, *input_pending);
  CF_DATA_RESTORE(cf, save);
  ܦܢܐ alive;
}

ساكن CURLcode cf_h2_keep_alive(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data)
{
  CURLcode result;
  بنية cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  result = http2_send_ping(cf, data);
  CF_DATA_RESTORE(cf, save);
  ܦܢܐ result;
}

ساكن CURLcode cf_h2_query(بنية Curl_cfilter *cf,
                            بنية Curl_easy *data,
                            صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية cf_h2_ctx *ctx = cf->ctx;
  بنية cf_call_data save;
  size_t effective_max;

  بدل(query) {
  حالة CF_QUERY_MAX_CONCURRENT:
    DEBUGASSERT(pres1);

    CF_DATA_SAVE(save, cf, data);
    ܐܢ(!Curl_ngh2_valid(&ctx->ngh2) ||
       !Curl_ngh2_session_check_request_allowed(&ctx->ngh2)) {
      /* the limit is what we have in use right now */
      effective_max = cf->conn->attached_xfers;
    }
    ܐܚܪܢܐ {
      effective_max = ctx->max_concurrent_streams;
    }
    *pres1 = (effective_max > INT_MAX) ? INT_MAX : (صحيح)effective_max;
    CF_DATA_RESTORE(cf, save);
    ܦܢܐ CURLE_OK;
  حالة CF_QUERY_STREAM_ERROR: {
    بنية h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    *pres1 = stream ? (صحيح)stream->error : 0;
    ܦܢܐ CURLE_OK;
  }
  حالة CF_QUERY_NEED_FLUSH: {
    بنية h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    ܐܢ(!Curl_bufq_is_empty(&ctx->outbufq) ||
       (stream && !Curl_bufq_is_empty(&stream->sendbuf))) {
      *pres1 = TRUE;
      ܦܢܐ CURLE_OK;
    }
    اكسر;
  }
  حالة CF_QUERY_HTTP_VERSION:
    *pres1 = 20;
    ܦܢܐ CURLE_OK;
  افتراضي:
    اكسر;
  }
  ܦܢܐ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
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
  cf_h2_data_pending,
  cf_h2_send,
  cf_h2_recv,
  cf_h2_cntrl,
  cf_h2_is_alive,
  cf_h2_keep_alive,
  cf_h2_query,
};

ساكن CURLcode http2_cfilter_add(بنية Curl_cfilter **pcf,
                                  بنية Curl_easy *data,
                                  بنية connectdata *conn,
                                  int8_t sockindex,
                                  bool via_h1_upgrade)
{
  بنية Curl_cfilter *cf = عدم;
  بنية cf_h2_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(data->conn);
  ctx = curlx_calloc(1, حجم(*ctx));
  ܐܢ(!ctx)
    اذهب out;
  cf_h2_ctx_init(ctx, via_h1_upgrade);

  result = Curl_cf_create(&cf, &Curl_cft_nghttp2, ctx);
  ܐܢ(result)
    اذهب out;

  ctx = عدم;
  Curl_conn_cf_add(data, conn, sockindex, cf);

out:
  ܐܢ(result)
    cf_h2_ctx_free(ctx);
  *pcf = result ? عدم : cf;
  ܦܢܐ result;
}

ساكن CURLcode http2_cfilter_insert_after(بنية Curl_cfilter *cf,
                                           بنية Curl_easy *data,
                                           bool via_h1_upgrade)
{
  بنية Curl_cfilter *cf_h2 = عدم;
  بنية cf_h2_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (فراغ)data;
  ctx = curlx_calloc(1, حجم(*ctx));
  ܐܢ(!ctx)
    اذهب out;
  cf_h2_ctx_init(ctx, via_h1_upgrade);

  result = Curl_cf_create(&cf_h2, &Curl_cft_nghttp2, ctx);
  ܐܢ(result)
    اذهب out;

  ctx = عدم;
  Curl_conn_cf_insert_after(cf, cf_h2);

out:
  ܐܢ(result)
    cf_h2_ctx_free(ctx);
  ܦܢܐ result;
}

bool Curl_http2_may_switch(بنية Curl_easy *data)
{
  ܐܢ(Curl_conn_http_version(data, data->conn) < 20 &&
     (data->state.http_neg.wanted & CURL_HTTP_V2x) &&
     data->state.http_neg.h2_prior_knowledge) {
#ifndef CURL_DISABLE_PROXY
    ܐܢ(data->conn->bits.origin_is_proxy) {
      infof(data, "Ignoring HTTP/2 prior knowledge due to proxy");
      ܦܢܐ FALSE;
    }
#endif
    ܦܢܐ TRUE;
  }
  ܦܢܐ FALSE;
}

CURLcode Curl_http2_switch(بنية Curl_easy *data)
{
  بنية Curl_cfilter *cf;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, data->conn) < 20);

  result = http2_cfilter_add(&cf, data, data->conn, FIRSTSOCKET, FALSE);
  ܐܢ(result)
    ܦܢܐ result;
  CURL_TRC_CF(data, cf, "switching connection to HTTP/2");

  ܐܢ(cf->next) {
    bool done;
    ܦܢܐ Curl_conn_cf_connect(cf, data, &done);
  }
  ܦܢܐ CURLE_OK;
}

CURLcode Curl_http2_switch_at(بنية Curl_cfilter *cf, بنية Curl_easy *data)
{
  بنية Curl_cfilter *cf_h2;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, data->conn) < 20);

  result = http2_cfilter_insert_after(cf, data, FALSE);
  ܐܢ(result)
    ܦܢܐ result;

  cf_h2 = cf->next;

  ܐܢ(cf_h2->next) {
    bool done;
    ܦܢܐ Curl_conn_cf_connect(cf_h2, data, &done);
  }
  ܦܢܐ CURLE_OK;
}

CURLcode Curl_http2_upgrade(بنية Curl_easy *data,
                            بنية connectdata *conn, int8_t sockindex,
                            ثابت حرف *mem, size_t nread)
{
  بنية Curl_cfilter *cf;
  بنية cf_h2_ctx *ctx;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, conn) < 20);

  result = http2_cfilter_add(&cf, data, conn, sockindex, TRUE);
  ܐܢ(result)
    ܦܢܐ result;
  CURL_TRC_CF(data, cf, "upgrading connection to HTTP/2");

  DEBUGASSERT(cf->cft == &Curl_cft_nghttp2);
  ctx = cf->ctx;

  data->req.httpversion_sent = 20; /* it is an h2 request now */
  data->req.header = TRUE; /* we expect the real response to come in h2 */
  data->req.headerline = 0; /* restart the header line counter */

  ܐܢ(nread > 0) {
    /* Remaining data from the protocol switch reply is already using
     * the switched protocol, ie. HTTP/2. We add that to the network
     * inbufq. */
    size_t copied;

    result = Curl_bufq_write(&ctx->inbufq,
                             (ثابت unsigned حرف *)mem, nread, &copied);
    ܐܢ(result) {
      failf(data, "error on copying HTTP Upgrade response: %d", (صحيح)result);
      ܦܢܐ CURLE_RECV_ERROR;
    }
    ܐܢ(copied < nread) {
      failf(data, "connection buffer size could not take all data "
            "from HTTP Upgrade response header: copied=%zu, datalen=%zu",
            copied, nread);
      ܦܢܐ CURLE_HTTP2;
    }
    infof(data, "Copied HTTP/2 data in stream buffer to connection buffer"
          " after upgrade: len=%zu", nread);
  }

  ܐܢ(cf->next) {
    bool done;
    result = Curl_conn_cf_connect(cf, data, &done);
    ܐܢ(!result)
      cf->cft->cntrl(cf, data, CF_CTRL_CONN_INFO_UPDATE, 0, عدم);
  }
  ܦܢܐ result;
}

/* Only call this function for a transfer that already got an HTTP/2
   CURLE_HTTP2_STREAM error! */
bool Curl_h2_http_1_1_error(بنية Curl_easy *data)
{
  ܐܢ(Curl_conn_http_version(data, data->conn) == 20) {
    صحيح err = Curl_conn_get_stream_error(data, data->conn, FIRSTSOCKET);
    ܦܢܐ err == NGHTTP2_HTTP_1_1_REQUIRED;
  }
  ܦܢܐ FALSE;
}

فراغ *Curl_nghttp2_malloc(size_t size, فراغ *user_data)
{
  (فراغ)user_data;
  ܦܢܐ Curl_cmalloc(size);
}

فراغ Curl_nghttp2_free(فراغ *ptr, فراغ *user_data)
{
  (فراغ)user_data;
  Curl_cfree(ptr);
}

فراغ *Curl_nghttp2_calloc(size_t nmemb, size_t size, فراغ *user_data)
{
  (فراغ)user_data;
  ܦܢܐ Curl_ccalloc(nmemb, size);
}

فراغ *Curl_nghttp2_realloc(فراغ *ptr, size_t size, فراغ *user_data)
{
  (فراغ)user_data;
  ܦܢܐ Curl_crealloc(ptr, size);
}

#ܐܚܪܢܐ /* CURL_DISABLE_HTTP || !USE_NGHTTP2 */

/* Satisfy external references even if http2 is not compiled in. */

حرف *curl_pushheader_bynum(بنية curl_pushheaders *h, size_t num)
{
  (فراغ)h;
  (فراغ)num;
  ܦܢܐ عدم;
}

حرف *curl_pushheader_byname(بنية curl_pushheaders *h, ثابت حرف *name)
{
  (فراغ)h;
  (فراغ)name;
  ܦܢܐ عدم;
}

#endif /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
