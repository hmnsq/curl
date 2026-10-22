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
#அந்தர்பாவ "curl_setup.h"

#எனில் !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#அந்தர்பாவ <nghttp2/nghttp2.h>

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "bufq.h"
#அந்தர்பாவ "u32_ptrset.h"
#அந்தர்பாவ "http1.h"
#அந்தர்பாவ "http2.h"
#அந்தர்பாவ "http.h"
#அந்தர்பாவ "sendf.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "curlx/base64.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "ngh2_shim.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "urlapi-int.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "bufref.h"
#அந்தர்பாவ "curlx/dynbuf.h"
#அந்தர்பாவ "headers.h"
#அந்தர்பாவ "curl_share.h"

#எனில் NGHTTP2_VERSION_இலக்கம் < 0x010f00
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
#எனில் NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
#define H2_STREAM_WINDOW_SIZE_INITIAL  (64 * 1024)
#இல்லையெனில்
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

கட்டமைப்பு cf_h2_ctx {
  கட்டமைப்பு curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  கட்டமைப்பு cf_call_data call_data;

  கட்டமைப்பு bufq inbufq;           /* network input */
  கட்டமைப்பு bufq outbufq;          /* network output */
  கட்டமைப்பு bufc_pool stream_bufcp; /* spares for stream buffers */
  கட்டமைப்பு dynbuf scratch;        /* scratch buffer for temp use */

  கட்டமைப்பு u32_ptrset streams; /* hash of `data->mid` to `h2_stream_ctx` */
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
  BIT(enable_தள்ளு);
  BIT(nw_out_blocked);
};

/* How to access `call_data` from a cf_h2 filter */
#undef CF_CTX_CALL_DATA
#define CF_CTX_CALL_DATA(cf) ((கட்டமைப்பு cf_h2_ctx *)(cf)->ctx)->call_data

/**
 * All about the H2 internals of a stream
 */
கட்டமைப்பு h2_stream_ctx {
  கட்டமைப்பு bufq sendbuf; /* request buffer */
  கட்டமைப்பு h1_req_parser h1; /* parsing the request */
  கட்டமைப்பு dynhds resp_trailers; /* response trailer fields */
  size_t resp_hds_len; /* amount of response header bytes in recvbuf */
  curl_off_t xfer_id;  /* transfer `id` owning this stream */
  curl_off_t nrcvd_data;  /* number of DATA bytes received */

  எழுத்து **தள்ளு_headers;       /* allocated array */
  size_t தள்ளு_headers_used;  /* number of entries filled in */
  size_t தள்ளு_headers_alloc; /* number of entries allocated */

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

நிலையான வெறுமை free_தள்ளு_headers(கட்டமைப்பு h2_stream_ctx *stream)
{
  size_t i;
  க்கு(i = 0; i < stream->தள்ளு_headers_used; i++)
    curlx_free(stream->தள்ளு_headers[i]);
  curlx_safefree(stream->தள்ளு_headers);
  stream->தள்ளு_headers_used = 0;
}

நிலையான வெறுமை h2_stream_ctx_free(கட்டமைப்பு h2_stream_ctx *stream)
{
  Curl_bufq_free(&stream->sendbuf);
  Curl_h1_req_parse_free(&stream->h1);
  Curl_dynhds_free(&stream->resp_trailers);
  free_தள்ளு_headers(stream);
  curlx_free(stream);
}

நிலையான வெறுமை h2_stream_hash_free(uint32_t id, வெறுமை *stream)
{
  (வெறுமை)id;
  DEBUGASSERT(stream);
  h2_stream_ctx_free((கட்டமைப்பு h2_stream_ctx *)stream);
}

நிலையான வெறுமை cf_h2_ctx_init(கட்டமைப்பு cf_h2_ctx *ctx, bool via_h1_upgrade)
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

நிலையான வெறுமை cf_h2_ctx_free(கட்டமைப்பு cf_h2_ctx *ctx)
{
  எனில்(ctx && ctx->initialized) {
    Curl_ngh2_del(&ctx->ngh2);
    Curl_bufq_free(&ctx->inbufq);
    Curl_bufq_free(&ctx->outbufq);
    Curl_bufcp_free(&ctx->stream_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    memset(ctx, 0, அளவு(*ctx));
  }
  curlx_free(ctx);
}

நிலையான uint32_t cf_h2_initial_win_size(கட்டமைப்பு Curl_easy *data)
{
#எனில் NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
  /* If the transfer has a rate-limit lower than the default initial
   * stream window size, use that. It needs to be at least 8k or servers
   * may be unhappy. */
  curl_off_t rps = Curl_rlimit_per_step(&data->progress.dl.rlimit);
  எனில்((rps > 0) && (rps < H2_STREAM_WINDOW_SIZE_INITIAL))
    திரும்பு CURLMAX((uint32_t)rps, 8192);
#endif
  திரும்பு H2_STREAM_WINDOW_SIZE_INITIAL;
}

நிலையான size_t populate_settings(nghttp2_settings_entry *iv,
                                கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு cf_h2_ctx *ctx)
{
  iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
  iv[0].மதிப்பு = Curl_multi_max_concurrent_streams(data->multi);

  iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
  iv[1].மதிப்பு = cf_h2_initial_win_size(data);
  எனில்(ctx)
    ctx->initial_win_size = iv[1].மதிப்பு;
  iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_தள்ளு;
  iv[2].மதிப்பு = !!data->multi->தள்ளு_cb;

  திரும்பு 3;
}

நிலையான ssize_t populate_binsettings(uint8_t *binsettings,
                                    கட்டமைப்பு Curl_easy *data)
{
  nghttp2_settings_entry iv[H2_SETTINGS_IV_LEN];
  size_t ivlen;

  ivlen = populate_settings(iv, data, வெற்று);
  /* this returns number of bytes it wrote or a negative number on error. */
  திரும்பு nghttp2_pack_settings_payload(binsettings, H2_BINSETTINGS_LEN,
                                       iv, ivlen);
}

நிலையான CURLcode cf_h2_update_settings(கட்டமைப்பு cf_h2_ctx *ctx,
                                      uint32_t initial_win_size)
{
  nghttp2_settings_entry entry;
  entry.settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
  entry.மதிப்பு = initial_win_size;
  எனில்(Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, &entry, 1))
    திரும்பு CURLE_SEND_ERROR;
  ctx->initial_win_size = initial_win_size;
  திரும்பு CURLE_OK;
}

#define H2_STREAM_CTX(ctx, data)                                         \
  ((கட்டமைப்பு h2_stream_ctx *)(                                             \
    (data) ? Curl_u32_ptrset_get(&(ctx)->streams, (data)->mid) : வெற்று))

நிலையான கட்டமைப்பு h2_stream_ctx *h2_stream_ctx_create(கட்டமைப்பு cf_h2_ctx *ctx,
                                                  கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு h2_stream_ctx *stream;

  stream = curlx_calloc(1, அளவு(*stream));
  எனில்(!stream)
    திரும்பு வெற்று;

  stream->id = -1;
  stream->xfer_id = data->id;
  stream->mid = data->mid;
  Curl_bufq_initp(&stream->sendbuf, &ctx->stream_bufcp,
                  H2_STREAM_SEND_CHUNKS, BUFQ_OPT_NONE);
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_வரி_LEN);
  Curl_dynhds_init(&stream->resp_trailers,
                   MAX_HTTP_RESP_HEADER_மொத்தம், DYN_HTTP_REQUEST);
  stream->bodystarted = FALSE;
  stream->status_code = -1;
  stream->closed = FALSE;
  stream->close_handled = FALSE;
  stream->error = NGHTTP2_NO_ERROR;
  stream->local_window_size = H2_STREAM_WINDOW_SIZE_INITIAL;
  stream->nrcvd_data = 0;
  திரும்பு stream;
}

நிலையான கட்டமைப்பு Curl_easy *cf_h2_get_stream_xfer(கட்டமைப்பு Curl_cfilter *cf,
                                               int32_t stream_id,
                                               கட்டமைப்பு h2_stream_ctx **pstream)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  கட்டமைப்பு h2_stream_ctx *stream =
    Curl_ngh2_session_get_stream_user_data(&ctx->ngh2, stream_id);
  எனில்(pstream)
    *pstream = stream;
  எனில்(stream && (stream->mid != UINT32_MAX)) {
    கட்டமைப்பு Curl_easy *data, *call_data = CF_DATA_CURRENT(cf);
    எனில்(!call_data || !call_data->multi) {
      DEBUGASSERT(0);
      திரும்பு வெற்று;
    }
    data = Curl_multi_get_easy(call_data->multi, stream->mid);
    எனில்(data && (data->id == stream->xfer_id))
      திரும்பு data;
  }
  திரும்பு வெற்று;
}

#ifdef NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
நிலையான int32_t cf_h2_get_desired_local_win(கட்டமைப்பு Curl_cfilter *cf,
                                           கட்டமைப்பு Curl_easy *data)
{
  curl_off_t avail = Curl_rlimit_avail(&data->progress.dl.rlimit, வெற்று);

  (வெறுமை)cf;
  எனில்(avail < CURL_OFF_T_MAX) { /* limit in place */
    எனில்(avail <= 0)
      திரும்பு 0;
    இல்லையெனில் எனில்(avail < H2_STREAM_WINDOW_SIZE_MAX)
      திரும்பு (int32_t)avail;
  }
  திரும்பு H2_STREAM_WINDOW_SIZE_MAX;
}

நிலையான CURLcode cf_h2_update_local_win(கட்டமைப்பு Curl_cfilter *cf,
                                       கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு h2_stream_ctx *stream)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  int32_t dwsize;
  முழு rv;

  dwsize = (stream->write_paused || stream->xfer_result) ?
           0 : cf_h2_get_desired_local_win(cf, data);
  எனில்(dwsize != stream->local_window_size) {
    int32_t wsize = Curl_ngh2_session_get_stream_effective_local_window_size(
      &ctx->ngh2, stream->id);
    எனில்(dwsize > wsize) {
      rv = Curl_ngh2_session_set_local_window_size(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize);
      எனில்(rv) {
        failf(data, "[%d] nghttp2 set_local_window_size(%d) failed: "
              "%s(%d)", stream->id, dwsize, nghttp2_strerror(rv), rv);
        திரும்பு CURLE_HTTP2;
      }
      rv = Curl_ngh2_submit_window_update(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize - wsize);
      எனில்(rv) {
        failf(data, "[%d] nghttp2_submit_window_update() failed: "
              "%s(%d)", stream->id, nghttp2_strerror(rv), rv);
        திரும்பு CURLE_HTTP2;
      }
      stream->local_window_size = dwsize;
      CURL_TRC_CF(data, cf, "[%d] local window update by %d",
                  stream->id, dwsize - wsize);
    }
    இல்லையெனில் {
      rv = Curl_ngh2_session_set_local_window_size(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize);
      எனில்(rv) {
        failf(data, "[%d] nghttp2_session_set_local_window_size() failed: "
              "%s(%d)", stream->id, nghttp2_strerror(rv), rv);
        திரும்பு CURLE_HTTP2;
      }
      stream->local_window_size = dwsize;
      CURL_TRC_CF(data, cf, "[%d] local window size now %d",
                  stream->id, dwsize);
    }
  }
  திரும்பு CURLE_OK;
}

#இல்லையெனில் /* NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

நிலையான CURLcode cf_h2_update_local_win(கட்டமைப்பு Curl_cfilter *cf,
                                       கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு h2_stream_ctx *stream)
{
  (வெறுமை)cf;
  (வெறுமை)data;
  (வெறுமை)stream;
  திரும்பு CURLE_OK;
}
#endif /* !NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

நிலையான CURLcode http2_data_setup(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு h2_stream_ctx **pstream)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  கட்டமைப்பு h2_stream_ctx *stream;

  (வெறுமை)cf;
  DEBUGASSERT(data);

  எனில்(!data)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  stream = H2_STREAM_CTX(ctx, data);
  எனில்(stream) {
    *pstream = stream;
    திரும்பு CURLE_OK;
  }

  stream = h2_stream_ctx_create(ctx, data);
  எனில்(!stream)
    திரும்பு CURLE_OUT_OF_MEMORY;

  எனில்(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    h2_stream_ctx_free(stream);
    திரும்பு CURLE_OUT_OF_MEMORY;
  }

  *pstream = stream;
  திரும்பு CURLE_OK;
}

நிலையான CURLcode nw_out_flush(கட்டமைப்பு Curl_cfilter *cf,
                             கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  எனில்(Curl_bufq_is_empty(&ctx->outbufq))
    திரும்பு CURLE_OK;

  result = Curl_cf_send_bufq(cf->next, data, &ctx->outbufq, வெற்று, 0,
                             &nwritten);
  எனில்(result) {
    எனில்(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_out_blocked = 1;
    }
    திரும்பு result;
  }
  திரும்பு Curl_bufq_is_empty(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

நிலையான வெறுமை http2_data_done(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  கட்டமைப்பு h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);

  DEBUGASSERT(ctx);
  எனில்(!stream || !ctx->initialized)
    திரும்பு;

  எனில்(Curl_ngh2_valid(&ctx->ngh2)) {
    bool flush_egress = FALSE;
    /* returns error if stream not known, which is fine here */
    (வெறுமை)Curl_ngh2_session_set_stream_user_data(&ctx->ngh2, stream->id, வெற்று);

    எனில்(!stream->closed && stream->id > 0) {
      /* RST_STREAM */
      CURL_TRC_CF(data, cf, "[%d] premature DATA_DONE, RST stream",
                  stream->id);
      stream->closed = TRUE;
      stream->reset = TRUE;
      Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                  stream->id, NGHTTP2_STREAM_CLOSED);
      flush_egress = TRUE;
    }

    எனில்(flush_egress) {
      (வெறுமை)Curl_ngh2_session_send(&ctx->ngh2);
      (வெறுமை)nw_out_flush(cf, data);
    }
  }

  Curl_u32_ptrset_unset(&ctx->streams, data->mid);
}

நிலையான முழு h2_client_new(கட்டமைப்பு Curl_cfilter *cf,
                         nghttp2_session_callbacks *cbs)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { வெற்று, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  முழு rc = nghttp2_option_new(&o);
  எனில்(rc)
    திரும்பு rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_set_no_auto_window_update(o, 1);
#எனில் NGHTTP2_VERSION_இலக்கம் >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_set_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#endif
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  திரும்பு rc;
}

/*
 * Returns nonzero if current HTTP/2 session should be closed.
 */
நிலையான முழு should_close_session(கட்டமைப்பு cf_h2_ctx *ctx)
{
  திரும்பு ctx->drain_total == 0 && !Curl_ngh2_session_want_read(&ctx->ngh2) &&
    !Curl_ngh2_session_want_write(&ctx->ngh2);
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
நிலையான CURLcode h2_process_pending_உள்ளீடு(கட்டமைப்பு Curl_cfilter *cf,
                                         கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  மாறிலி unsigned எழுத்து *buf;
  size_t blen, nread;
  ssize_t rv;

  போது(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {
    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (மாறிலி uint8_t *)buf, blen);
    எனில்(!curlx_sztouz(rv, &nread)) {
      failf(data, "nghttp2 recv error %zd: %s", rv, nghttp2_strerror((முழு)rv));
      திரும்பு CURLE_RECV_ERROR;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    எனில்(Curl_bufq_is_empty(&ctx->inbufq)) {
      நிறுத்து;
    }
    இல்லையெனில் {
      CURL_TRC_CF(data, cf, "process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }

  எனில்(Curl_ngh2_session_check_request_allowed(&ctx->ngh2) == 0) {
    /* No more requests are allowed in the current session, so
       the connection may not be reused. This is set when a
       GOAWAY frame has been received or when the limit of stream
       identifiers has been reached. */
    CURL_TRC_M(data, "http/2: No new requests allowed");
    connclose(cf->conn);
  }

  திரும்பு CURLE_OK;
}

/*
 * The server may send us data at any point (e.g. PING frames). Therefore, we
 * cannot assume that an HTTP/2 socket is dead because it is readable.
 *
 * Check the lower filters first and, if successful, peek at the socket
 * and distinguish between closed and data.
 */
நிலையான bool http2_connisalive(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                              bool *உள்ளீடு_pending)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *உள்ளீடு_pending = FALSE;
  எனில்(!cf->next || !cf->next->cft->is_alive(cf->next, data, உள்ளீடு_pending))
    திரும்பு FALSE;

  எனில்(*உள்ளீடு_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode result;
    size_t nread;

    *உள்ளீடு_pending = FALSE;
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(data, cf, "connisalive, recv pending input -> %d, %zu",
                (முழு)result, nread);
    எனில்(!result) {
      result = h2_process_pending_உள்ளீடு(cf, data);
      எனில்(result)
        /* immediate error, considered dead */
        alive = FALSE;
      இல்லையெனில் {
        alive = !should_close_session(ctx);
      }
    }
    இல்லையெனில் எனில்(result != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  திரும்பு alive;
}

நிலையான CURLcode http2_send_ping(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  முழு rc;

  rc = Curl_ngh2_submit_ping(&ctx->ngh2, 0, ZERO_NULL);
  எனில்(rc) {
    failf(data, "nghttp2_submit_ping() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    திரும்பு CURLE_HTTP2;
  }

  rc = Curl_ngh2_session_send(&ctx->ngh2);
  எனில்(rc) {
    failf(data, "nghttp2_session_send() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    திரும்பு CURLE_SEND_ERROR;
  }
  திரும்பு CURLE_OK;
}

/*
 * Store nghttp2 version info in this buffer.
 */
வெறுமை Curl_http2_ver(எழுத்து *p, size_t len)
{
  nghttp2_info *h2 = nghttp2_version(0);
  (வெறுமை)curl_msnprintf(p, len, "nghttp2/%s", h2->version_str);
}

/*
 * The implementation of nghttp2_send_callback type. Here we write |data| with
 * size |length| to the network and return the number of bytes actually
 * written. See the documentation of nghttp2_send_callback for the details.
 */
நிலையான ssize_t send_callback(nghttp2_session *h2,
                             மாறிலி uint8_t *buf, size_t blen, முழு flags,
                             வெறுமை *userp)
{
  கட்டமைப்பு Curl_cfilter *cf = userp;
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result = CURLE_OK;

  (வெறுமை)h2;
  (வெறுமை)flags;
  DEBUGASSERT(data);

  எனில்(!cf->connected)
    result = Curl_bufq_write(&ctx->outbufq, buf, blen, &nwritten);
  இல்லையெனில்
    result = Curl_cf_send_bufq(cf->next, data, &ctx->outbufq, buf, blen,
                               &nwritten);

  எனில்(result) {
    எனில்(result == CURLE_AGAIN) {
      ctx->nw_out_blocked = 1;
      திரும்பு NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(data, "Failed sending HTTP2 data");
    திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  எனில்(!nwritten) {
    ctx->nw_out_blocked = 1;
    திரும்பு NGHTTP2_ERR_WOULDBLOCK;
  }
  திரும்பு (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

/* We pass a pointer to this struct in the push callback, but the contents of
   the struct are hidden from the user. */
கட்டமைப்பு curl_pushheaders {
  கட்டமைப்பு Curl_easy *data;
  கட்டமைப்பு h2_stream_ctx *stream;
  மாறிலி nghttp2_தள்ளு_promise *frame;
};

/*
 * push header access function. Only to be used from within the push callback
 */
எழுத்து *curl_pushheader_bynum(கட்டமைப்பு curl_pushheaders *h, size_t இலக்கம்)
{
  /* Verify that we got a good easy handle in the push header struct, mostly to
     detect rubbish input fast(er). */
  எனில்(!h || !GOOD_EASY_HANDLE(h->data))
    திரும்பு வெற்று;
  இல்லையெனில் {
    எனில்(h->stream && இலக்கம் < h->stream->தள்ளு_headers_used)
      திரும்பு h->stream->தள்ளு_headers[இலக்கம்];
  }
  திரும்பு வெற்று;
}

/*
 * push header access function. Only to be used from within the push callback
 */
எழுத்து *curl_pushheader_byname(கட்டமைப்பு curl_pushheaders *h, மாறிலி எழுத்து *name)
{
  கட்டமைப்பு h2_stream_ctx *stream;
  size_t len;
  size_t i;
  /* Verify that we got a good easy handle in the push header struct,
     mostly to detect rubbish input fast(er). Also empty header name
     is rubbish too. We have to allow ":" at the beginning of
     the header, but header == ":" must be rejected. If we have ':' in
     the middle of header, it could be matched in middle of the value,
     this is because we do prefix match.*/
  எனில்(!h || !GOOD_EASY_HANDLE(h->data) || !name || !name[0] ||
     !strcmp(name, ":") || strchr(name + 1, ':'))
    திரும்பு வெற்று;

  stream = h->stream;
  எனில்(!stream)
    திரும்பு வெற்று;

  len = strlen(name);
  க்கு(i = 0; i < stream->தள்ளு_headers_used; i++) {
    எனில்(!strncmp(name, stream->தள்ளு_headers[i], len)) {
      /* sub-match, make sure that it is followed by a colon */
      எனில்(stream->தள்ளு_headers[i][len] != ':')
        தொடர்;
      திரும்பு &stream->தள்ளு_headers[i][len + 1];
    }
  }
  திரும்பு வெற்று;
}

நிலையான கட்டமைப்பு Curl_easy *h2_duphandle(கட்டமைப்பு Curl_cfilter *cf,
                                      கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_easy *second = curl_easy_init();
  எனில்(second) {
    கட்டமைப்பு h2_stream_ctx *second_stream;
    http2_data_setup(cf, second, &second_stream);
    second->state.weight = data->state.weight;
    எனில்(data->share)
      (வெறுமை)Curl_share_easy_link(second, data->share);
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
  திரும்பு second;
}

நிலையான முழு set_transfer_url(கட்டமைப்பு Curl_easy *newhandle,
                            கட்டமைப்பு curl_pushheaders *hp,
                            கட்டமைப்பு Curl_easy *data)
{
  மாறிலி எழுத்து *v;
  CURLUcode uc;
  எழுத்து *url = வெற்று;
  முழு rc = 0;
  CURLU *u = curl_url();

  எனில்(!u)
    திரும்பு 5;

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_SCHEME);
  எனில்(v) {
    uc = curl_url_set(u, CURLUPART_SCHEME, v, 0);
    எனில்(uc) {
      rc = 1;
      செல் fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_AUTHORITY);
  எனில்(v) {
    uc = Curl_url_set_authority(u, v);
    எனில்(uc) {
      rc = 2;
      செல் fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_PATH);
  எனில்(v) {
    uc = curl_url_set(u, CURLUPART_PATH, v, 0);
    எனில்(uc) {
      rc = 3;
      செல் fail;
    }
  }

  /* We can only allow PUSH of resource from the same origin, e.g.
   * scheme + hostname + port */
  எனில்(!Curl_url_same_origin(data->state.uh, u)) {
    rc = 1;
    செல் fail;
  }

  uc = curl_url_get(u, CURLUPART_URL, &url, 0);
  எனில்(uc)
    rc = 4;
fail:
  எனில்(rc) {
    curl_url_cleanup(u);
    திரும்பு rc;
  }

  DEBUGASSERT(!newhandle->state.uh);
  newhandle->state.uh = u;
  Curl_bufref_set(&newhandle->state.url, url, 0, curl_free);
  திரும்பு 0;
}

நிலையான வெறுமை discard_newhandle(கட்டமைப்பு Curl_cfilter *cf,
                              கட்டமைப்பு Curl_easy *newhandle)
{
  http2_data_done(cf, newhandle);
  (வெறுமை)Curl_close(&newhandle);
}

நிலையான முழு தள்ளு_promise(கட்டமைப்பு Curl_cfilter *cf,
                        கட்டமைப்பு Curl_easy *data,
                        மாறிலி nghttp2_தள்ளு_promise *frame)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  முழு rv; /* one of the CURL_PUSH_* defines */

  CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE received",
              frame->promised_stream_id);
  எனில்(data->multi->தள்ளு_cb) {
    கட்டமைப்பு h2_stream_ctx *stream;
    கட்டமைப்பு h2_stream_ctx *newstream;
    கட்டமைப்பு curl_pushheaders heads;
    CURLMcode mresult;
    CURLcode result;
    /* clone the parent */
    கட்டமைப்பு Curl_easy *newhandle = h2_duphandle(cf, data);
    எனில்(!newhandle) {
      infof(data, "failed to duplicate handle");
      rv = CURL_தள்ளு_DENY; /* FAIL HARD */
      செல் fail;
    }

    stream = H2_STREAM_CTX(ctx, data);
    எனில்(!stream) {
      failf(data, "Internal NULL stream");
      discard_newhandle(cf, newhandle);
      rv = CURL_தள்ளு_DENY;
      செல் fail;
    }

    heads.data = data;
    heads.stream = stream;
    heads.frame = frame;

    rv = set_transfer_url(newhandle, &heads, data);
    எனில்(rv) {
      CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE, failed to set URL -> %d",
                  frame->promised_stream_id, rv);
      discard_newhandle(cf, newhandle);
      rv = CURL_தள்ளு_DENY;
      செல் fail;
    }

    {
      கட்டமைப்பு Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, multi_தள்ளு_cb);
      rv = data->multi->தள்ளு_cb(data, newhandle,
                                stream->தள்ளு_headers_used, &heads,
                                data->multi->தள்ளு_userp);
      CURL_CBAPI_END(&guard);
    }

    /* free the headers again */
    free_தள்ளு_headers(stream);

    எனில்(rv) {
      DEBUGASSERT((rv > CURL_தள்ளு_OK) && (rv <= CURL_தள்ளு_ERROROUT));
      /* denied, kill off the new handle again */
      CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE, denied by application -> %d",
                  frame->promised_stream_id, rv);
      discard_newhandle(cf, newhandle);
      செல் fail;
    }

    /* approved, add to the multi handle for processing. This
     * assigns newhandle->mid. For the new `mid` we assign the
     * h2_stream instance and remember the stream_id already known. */
    mresult = Curl_multi_add_perform(data->multi, newhandle, cf->conn);
    எனில்(mresult) {
      infof(data, "failed to add handle to multi");
      discard_newhandle(cf, newhandle);
      rv = CURL_தள்ளு_DENY;
      செல் fail;
    }

    result = http2_data_setup(cf, newhandle, &newstream);
    எனில்(result) {
      failf(data, "error setting up stream: %d", (முழு)result);
      discard_newhandle(cf, newhandle);
      rv = CURL_தள்ளு_DENY;
      செல் fail;
    }

    DEBUGASSERT(newstream);
    newstream->id = frame->promised_stream_id;
    newhandle->req.maxdownload = -1;
    newhandle->req.size = -1;

    CURL_TRC_CF(data, cf, "promise easy handle added to multi, mid=%u",
                newhandle->mid);
    rv = Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, newstream->id, newstream);
    எனில்(rv) {
      infof(data, "failed to set user_data for stream %d",
            newstream->id);
      DEBUGASSERT(0);
      discard_newhandle(cf, newhandle);
      rv = CURL_தள்ளு_DENY;
      செல் fail;
    }

    /* success, remember max stream id processed */
    எனில்(newstream->id > ctx->local_max_sid)
      ctx->local_max_sid = newstream->id;
  }
  இல்லையெனில் {
    CURL_TRC_CF(data, cf, "Got PUSH_PROMISE, ignore it");
    rv = CURL_தள்ளு_DENY;
  }
fail:
  திரும்பு rv;
}

நிலையான வெறுமை h2_xfer_write_resp_hd(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு h2_stream_ctx *stream,
                                  மாறிலி எழுத்து *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  எனில்(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, blen, eos);
    எனில்(!stream->xfer_result && !eos)
      stream->xfer_result = cf_h2_update_local_win(cf, data, stream);
    எனில்(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%d] error %d writing %zu bytes of headers",
                  stream->id, (முழு)stream->xfer_result, blen);
  }
}

நிலையான வெறுமை h2_xfer_write_resp(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு h2_stream_ctx *stream,
                               மாறிலி எழுத்து *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  எனில்(!stream->xfer_result)
    stream->xfer_result = Curl_xfer_write_resp(data, buf, blen, eos);
  /* If the transfer write is errored, we do not want any more data */
  எனில்(stream->xfer_result) {
    கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
    CURL_TRC_CF(data, cf, "[%d] error %d writing %zu bytes of data, "
                "RST-ing stream",
                stream->id, (முழு)stream->xfer_result, blen);
    Curl_ngh2_submit_rst_stream(
      &ctx->ngh2, 0, stream->id, (uint32_t)NGHTTP2_ERR_CALLBACK_FAILURE);
  }
  இல்லையெனில் எனில்(!stream->write_paused && Curl_xfer_write_is_paused(data)) {
    CURL_TRC_CF(data, cf, "[%d] stream output paused", stream->id);
    stream->write_paused = TRUE;
  }
  இல்லையெனில் எனில்(stream->write_paused && !Curl_xfer_write_is_paused(data)) {
    CURL_TRC_CF(data, cf, "[%d] stream output unpaused", stream->id);
    stream->write_paused = FALSE;
  }

  எனில்(!stream->xfer_result && !eos)
    stream->xfer_result = cf_h2_update_local_win(cf, data, stream);
}

நிலையான CURLcode on_stream_frame(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு h2_stream_ctx *stream,
                                மாறிலி nghttp2_frame *frame)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  int32_t stream_id = frame->hd.stream_id;
  முழு rv;

  எனில்(!stream) {
    CURL_TRC_CF(data, cf, "[%d] No stream_ctx set", stream_id);
    திரும்பு CURLE_FAILED_INIT;
  }

  தேர்வு(frame->hd.type) {
  நிலை NGHTTP2_DATA:
    CURL_TRC_CF(data, cf, "[%d] DATA, window=%d/%d",
                stream_id,
                Curl_ngh2_session_get_stream_effective_recv_data_length(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_stream_effective_local_window_size(
                  &ctx->ngh2, stream->id));
    /* If !body started on this stream, then receiving DATA is illegal. */
    எனில்(!stream->bodystarted) {
      எனில்(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                       stream_id, NGHTTP2_PROTOCOL_ERROR))) {
        திரும்பு CURLE_RECV_ERROR;
      }
    }
    நிறுத்து;
  நிலை NGHTTP2_HEADERS:
    எனில்(stream->bodystarted) {
      /* Only valid HEADERS after body started is trailer HEADERS. We
         buffer them in on_header callback. */
      நிறுத்து;
    }

    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    எனில்(stream->status_code == -1)
      திரும்பு CURLE_RECV_ERROR;

    /* Only final status code signals the end of header */
    எனில்(stream->status_code / 100 != 1) {
      stream->resp_hds_complete = TRUE;
      stream->bodystarted = TRUE;
    }
    இல்லையெனில்
      stream->status_code = -1;

    h2_xfer_write_resp_hd(cf, data, stream, STRCONST("\r\n"),
                          (bool)stream->closed);

    Curl_multi_mark_dirty(data);
    நிறுத்து;
  நிலை NGHTTP2_தள்ளு_PROMISE:
    rv = தள்ளு_promise(cf, data, &frame->தள்ளு_promise);
    எனில்(rv) { /* deny! */
      DEBUGASSERT((rv > CURL_தள்ளு_OK) && (rv <= CURL_தள்ளு_ERROROUT));
      எனில்(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
             frame->தள்ளு_promise.promised_stream_id, NGHTTP2_CANCEL)))
        திரும்பு CURLE_SEND_ERROR;
      இல்லையெனில் எனில்(rv == CURL_தள்ளு_ERROROUT) {
        /* fail the parent transfer, not the whole session */
        CURL_TRC_CF(data, cf, "[%d] fail in PUSH_PROMISE received",
                    stream_id);
        எனில்(!stream->xfer_result)
          stream->xfer_result = CURLE_RECV_ERROR;
        Curl_multi_mark_dirty(data);
      }
    }
    நிறுத்து;
  நிலை NGHTTP2_RST_STREAM:
    எனில்(frame->rst_stream.error_code)
      stream->reset_by_server = TRUE;
    Curl_multi_mark_dirty(data);
    நிறுத்து;
  நிலை NGHTTP2_WINDOW_UPDATE:
    எனில்(CURL_REQ_WANT_SEND(data) && Curl_bufq_is_empty(&stream->sendbuf)) {
      /* need more data, force processing of transfer */
      Curl_multi_mark_dirty(data);
    }
    இல்லையெனில் எனில்(!Curl_bufq_is_empty(&stream->sendbuf)) {
      /* resume the potentially suspended stream */
      எனில்(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id)))
        திரும்பு CURLE_SEND_ERROR;
    }
    நிறுத்து;
  இயல்பு:
    நிறுத்து;
  }

  எனில்(frame->hd.flags & NGHTTP2_FLAG_END_STREAM) {
    எனில்(!stream->closed && !stream->body_eos &&
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
  திரும்பு CURLE_OK;
}

#ifdef CURLVERBOSE
முழு Curl_nghttp2_fr_print(மாறிலி nghttp2_frame *frame, எழுத்து *buffer,
                          size_t blen)
{
  தேர்வு(frame->hd.type) {
  நிலை NGHTTP2_DATA: {
    திரும்பு curl_msnprintf(buffer, blen,
                          "FRAME[DATA, len=%d, eos=%d, padlen=%d]",
                          (முழு)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_STREAM),
                          (முழு)frame->data.padlen);
  }
  நிலை NGHTTP2_HEADERS: {
    திரும்பு curl_msnprintf(buffer, blen,
                          "FRAME[HEADERS, len=%d, hend=%d, eos=%d]",
                          (முழு)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_HEADERS),
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_STREAM));
  }
  நிலை NGHTTP2_PRIORITY: {
    திரும்பு curl_msnprintf(buffer, blen,
                          "FRAME[PRIORITY, len=%d, flags=%d]",
                          (முழு)frame->hd.length, frame->hd.flags);
  }
  நிலை NGHTTP2_RST_STREAM: {
    திரும்பு curl_msnprintf(buffer, blen,
                          "FRAME[RST_STREAM, len=%d, flags=%d, error=%u]",
                          (முழு)frame->hd.length, frame->hd.flags,
                          frame->rst_stream.error_code);
  }
  நிலை NGHTTP2_SETTINGS: {
    எனில்(frame->hd.flags & NGHTTP2_FLAG_ACK) {
      திரும்பு curl_msnprintf(buffer, blen, "FRAME[SETTINGS, ack=1]");
    }
    திரும்பு curl_msnprintf(buffer, blen,
                          "FRAME[SETTINGS, len=%d]", (முழு)frame->hd.length);
  }
  நிலை NGHTTP2_தள்ளு_PROMISE:
    திரும்பு curl_msnprintf(buffer, blen,
                          "FRAME[PUSH_PROMISE, len=%d, hend=%d]",
                          (முழு)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_HEADERS));
  நிலை NGHTTP2_PING:
    திரும்பு curl_msnprintf(buffer, blen,
                          "FRAME[PING, len=%d, ack=%d]",
                          (முழு)frame->hd.length,
                          frame->hd.flags & NGHTTP2_FLAG_ACK);
  நிலை NGHTTP2_GOAWAY: {
    எழுத்து scratch[128];
    size_t s_len = CURL_ARRAYSIZE(scratch);
    size_t len = (frame->goaway.opaque_data_len < s_len) ?
      frame->goaway.opaque_data_len : s_len - 1;
    எனில்(len)
      memcpy(scratch, frame->goaway.opaque_data, len);
    scratch[len] = '\0';
    திரும்பு curl_msnprintf(buffer, blen,
                          "FRAME[GOAWAY, error=%u, reason='%s', "
                          "last_stream=%d]", frame->goaway.error_code,
                          scratch, frame->goaway.last_stream_id);
  }
  நிலை NGHTTP2_WINDOW_UPDATE: {
    திரும்பு curl_msnprintf(buffer, blen,
                          "FRAME[WINDOW_UPDATE, incr=%d]",
                          frame->window_update.window_size_increment);
  }
  இயல்பு:
    திரும்பு curl_msnprintf(buffer, blen, "FRAME[%d, len=%d, flags=%d]",
                          frame->hd.type, (முழு)frame->hd.length,
                          frame->hd.flags);
  }
}

நிலையான முழு on_frame_send(nghttp2_session *session, மாறிலி nghttp2_frame *frame,
                         வெறுமை *userp)
{
  கட்டமைப்பு Curl_cfilter *cf = userp;
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);

  (வெறுமை)session;
  DEBUGASSERT(data);
  எனில்(Curl_trc_cf_is_verbose(cf, data)) {
    எழுத்து buffer[256];
    முழு len;
    len = Curl_nghttp2_fr_print(frame, buffer, அளவு(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] -> %s", frame->hd.stream_id, buffer);
  }
  எனில்((frame->hd.type == NGHTTP2_GOAWAY) && !ctx->sent_goaway) {
    /* A GOAWAY not initiated by us, but by nghttp2 itself on detecting
     * a protocol error on the connection */
    failf(data, "nghttp2 shuts down connection with error %u: %s",
          frame->goaway.error_code,
          nghttp2_http2_strerror(frame->goaway.error_code));
  }
  திரும்பு 0;
}
#endif /* CURLVERBOSE */

நிலையான முழு on_frame_recv(nghttp2_session *session, மாறிலி nghttp2_frame *frame,
                         வெறுமை *userp)
{
  கட்டமைப்பு Curl_cfilter *cf = userp;
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf), *data_s;
  கட்டமைப்பு h2_stream_ctx *stream;
  int32_t stream_id = frame->hd.stream_id;

  (வெறுமை)session;
  DEBUGASSERT(data);
#ifdef CURLVERBOSE
  எனில்(Curl_trc_cf_is_verbose(cf, data)) {
    எழுத்து buffer[256];
    முழு len;
    len = Curl_nghttp2_fr_print(frame, buffer, அளவு(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] <- %s", frame->hd.stream_id, buffer);
  }
#endif /* CURLVERBOSE */

  எனில்(!stream_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(data);
    தேர்வு(frame->hd.type) {
    நிலை NGHTTP2_SETTINGS: {
      எனில்(!(frame->hd.flags & NGHTTP2_FLAG_ACK)) {
        uint32_t max_conn = ctx->max_concurrent_streams;
        ctx->max_concurrent_streams =
          Curl_ngh2_session_get_remote_settings(
            &ctx->ngh2, NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS);
        ctx->enable_தள்ளு = Curl_ngh2_session_get_remote_settings(
          &ctx->ngh2, NGHTTP2_SETTINGS_ENABLE_தள்ளு) != 0;
        CURL_TRC_CF(data, cf, "[0] MAX_CONCURRENT_STREAMS: %u",
                    ctx->max_concurrent_streams);
        CURL_TRC_CF(data, cf, "[0] ENABLE_PUSH: %s",
                    ctx->enable_தள்ளு ? "TRUE" : "false");
        எனில்(data && max_conn != ctx->max_concurrent_streams) {
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
        எனில்(CURL_REQ_WANT_SEND(data))
          Curl_multi_mark_dirty(data);
      }
      நிறுத்து;
    }
    நிலை NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->goaway_error = frame->goaway.error_code;
      ctx->remote_max_sid = frame->goaway.last_stream_id;
      எனில்(data) {
        infof(data, "received GOAWAY, error=%u, last_stream=%d",
              ctx->goaway_error, ctx->remote_max_sid);
        Curl_multi_connchanged(data->multi);
      }
      நிறுத்து;
    இயல்பு:
      நிறுத்து;
    }
    திரும்பு 0;
  }

  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  எனில்(!data_s) {
    CURL_TRC_CF(data, cf, "[%d] No Curl_easy associated", stream_id);
    திரும்பு 0;
  }

  திரும்பு on_stream_frame(cf, data_s, stream, frame) ?
         NGHTTP2_ERR_CALLBACK_FAILURE : 0;
}

நிலையான முழு cf_h2_on_invalid_frame_recv(nghttp2_session *session,
                                       மாறிலி nghttp2_frame *frame,
                                       முழு ngerr, வெறுமை *userp)
{
  கட்டமைப்பு Curl_cfilter *cf = userp;
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  கட்டமைப்பு Curl_easy *data;
  கட்டமைப்பு h2_stream_ctx *stream;
  int32_t stream_id = frame->hd.stream_id;

  (வெறுமை)session;
  data = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  எனில்(data) {
#ifdef CURLVERBOSE
    எழுத்து buffer[256];
    முழு len;
    len = Curl_nghttp2_fr_print(frame, buffer, அளவு(buffer) - 1);
    buffer[len] = 0;
    failf(data, "[HTTP2] [%d] received invalid frame: %s, error %d: %s",
          stream_id, buffer, ngerr, nghttp2_strerror(ngerr));
#endif /* CURLVERBOSE */
    எனில்(stream) {
      Curl_ngh2_submit_rst_stream(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, NGHTTP2_STREAM_CLOSED);
      stream->error = ngerr;
      stream->closed = TRUE;
      stream->reset = TRUE;
    }
  }
  /* Return no error, nghttp2 will RST/GOAWAY by itself when needed */
  திரும்பு 0;
}

நிலையான முழு on_data_chunk_recv(nghttp2_session *session, uint8_t flags,
                              int32_t stream_id,
                              மாறிலி uint8_t *mem, size_t len, வெறுமை *userp)
{
  கட்டமைப்பு Curl_cfilter *cf = userp;
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  கட்டமைப்பு h2_stream_ctx *stream;
  கட்டமைப்பு Curl_easy *data_s;

  (வெறுமை)session;
  (வெறுமை)flags;

  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */
  DEBUGASSERT(CF_DATA_CURRENT(cf));

  /* get the stream from the hash based on Stream ID */
  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  எனில்(!data_s) {
    /* Receiving a Stream ID not in the hash should not happen - unless
       we have aborted a transfer artificially and there were more data
       in the pipeline. Silently ignore. */
    CURL_TRC_CF(CF_DATA_CURRENT(cf), cf, "[%d] Data for unknown", stream_id);
    /* consumed explicitly as no one will read it */
    Curl_ngh2_session_consume(&ctx->ngh2, stream_id, len);
    திரும்பு 0;
  }

  எனில்(!stream)
    திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;

  h2_xfer_write_resp(cf, data_s, stream, (மாறிலி எழுத்து *)mem, len, FALSE);

  Curl_ngh2_session_consume(&ctx->ngh2, stream_id, len);
  stream->nrcvd_data += (curl_off_t)len;
  திரும்பு 0;
}

நிலையான முழு on_stream_close(nghttp2_session *session, int32_t stream_id,
                           uint32_t error_code, வெறுமை *userp)
{
  கட்டமைப்பு Curl_cfilter *cf = userp;
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  கட்டமைப்பு Curl_easy *data_s, *call_data = CF_DATA_CURRENT(cf);
  கட்டமைப்பு h2_stream_ctx *stream = வெற்று;
  முழு rv;
  (வெறுமை)session;

  DEBUGASSERT(call_data);
  /* stream id 0 is the connection, do not look there for streams. */
  data_s = stream_id ? cf_h2_get_stream_xfer(cf, stream_id, &stream) : வெற்று;
  எனில்(!data_s) {
    CURL_TRC_CF(call_data, cf,
                "[%d] on_stream_close, no easy set on stream", stream_id);
    திரும்பு 0;
  }
  எனில்(!GOOD_EASY_HANDLE(data_s)) {
    /* nghttp2 still has an easy registered for the stream which has
     * been freed be libcurl. This points to a code path that does not
     * trigger DONE or DETACH events as it must. */
    CURL_TRC_CF(call_data, cf,
                "[%d] on_stream_close, not a GOOD easy on stream", stream_id);
    (வெறுமை)Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, stream_id, வெற்று);
    திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  எனில்(!stream) {
    CURL_TRC_CF(data_s, cf,
                "[%d] on_stream_close, GOOD easy but no stream", stream_id);
    திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  stream->closed = TRUE;
  stream->error = error_code;
  எனில்(stream->error) {
    stream->reset = TRUE;
    எனில்((stream->error == NGHTTP2_REFUSED_STREAM) &&
       ctx->rcvd_goaway && ctx->goaway_error)
    /* REFUSED_STREAM is used by nghttp2 on a GOAWAY from the
     * server where it indicated it will no longer process this stream.
     * If the GOAWAY carried an error code, take that as the error for
     * the stream. */
    stream->error = ctx->goaway_error;
  }

  எனில்(stream->error)
    CURL_TRC_CF(data_s, cf, "[%d] RESET: %s (err %u)",
              stream_id, nghttp2_http2_strerror(error_code), error_code);
  இல்லையெனில்
    CURL_TRC_CF(data_s, cf, "[%d] CLOSED", stream_id);
  Curl_multi_mark_dirty(data_s);

  /* remove `stream_ctx` from the nghttp2 stream */
  rv = Curl_ngh2_session_set_stream_user_data(
    &ctx->ngh2, stream_id, வெற்று);
  எனில்(rv) {
    infof(data_s, "http/2: failed to clear user_data for stream %d",
          stream_id);
    DEBUGASSERT(0);
  }
  திரும்பு 0;
}

நிலையான வெறுமை cf_h2_header_error(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு h2_stream_ctx *stream,
                               CURLcode result)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;

  failf(data, "Error receiving HTTP2 header: %d(%s)", (முழு)result,
        curl_easy_strerror(result));
  எனில்(stream) {
    Curl_ngh2_submit_rst_stream(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, NGHTTP2_STREAM_CLOSED);
    stream->closed = TRUE;
    stream->reset = TRUE;
  }
}

/* frame->hd.type is either NGHTTP2_HEADERS or NGHTTP2_PUSH_PROMISE */
நிலையான முழு on_header(nghttp2_session *session, மாறிலி nghttp2_frame *frame,
                     மாறிலி uint8_t *name, size_t namelen,
                     மாறிலி uint8_t *மதிப்பு, size_t valuelen,
                     uint8_t flags,
                     வெறுமை *userp)
{
  கட்டமைப்பு Curl_cfilter *cf = userp;
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  கட்டமைப்பு h2_stream_ctx *stream;
  கட்டமைப்பு Curl_easy *data;
  int32_t stream_id = frame->hd.stream_id;
  CURLcode result;

  (வெறுமை)session;
  (வெறுமை)flags;

  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */

  /* get the stream from the hash based on Stream ID */
  data = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  எனில்(!GOOD_EASY_HANDLE(data))
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;

  எனில்(!stream) {
    failf(data, "Internal NULL stream");
    திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  /* Store received PUSH_PROMISE headers to be used when the subsequent
     PUSH_PROMISE callback comes */
  எனில்(frame->hd.type == NGHTTP2_தள்ளு_PROMISE) {
    எழுத்து *h;

    எனில்((namelen == CURL_CSTRLEN(HTTP_PSEUDO_AUTHORITY)) &&
       !strncmp(HTTP_PSEUDO_AUTHORITY, (மாறிலி எழுத்து *)name, namelen)) {
      /* pseudo headers are lower case */
      முழு rc = 0;
      எழுத்து *check = curl_maprintf("%s:%d", data->state.origin->hostname,
                                  data->state.origin->port);
      எனில்(!check)
        /* no memory */
        திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;
      எனில்(!curl_strequal(check, (மாறிலி எழுத்து *)மதிப்பு) &&
         ((data->state.origin->port != data->state.origin->scheme->defport) ||
          !curl_strequal(data->state.origin->hostname, (மாறிலி எழுத்து *)மதிப்பு))) {
        /* This is push is not for the same authority that was asked for in
         * the URL. RFC 7540 section 8.2 says: "A client MUST treat a
         * PUSH_PROMISE for which the server is not authoritative as a stream
         * error of type PROTOCOL_ERROR."
         */
        (வெறுமை)Curl_ngh2_submit_rst_stream(
          &ctx->ngh2, NGHTTP2_FLAG_NONE, stream_id, NGHTTP2_PROTOCOL_ERROR);
        rc = NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      curlx_free(check);
      எனில்(rc)
        திரும்பு rc;
    }

    எனில்(!stream->தள்ளு_headers) {
      stream->தள்ளு_headers_alloc = 10;
      stream->தள்ளு_headers = curlx_malloc(stream->தள்ளு_headers_alloc *
                                          அளவு(எழுத்து *));
      எனில்(!stream->தள்ளு_headers)
        திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;
      stream->தள்ளு_headers_used = 0;
    }
    இல்லையெனில் எனில்(stream->தள்ளு_headers_used ==
            stream->தள்ளு_headers_alloc) {
      எழுத்து **headp;
      எனில்(stream->தள்ளு_headers_alloc > 1000) {
        /* this is beyond crazy many headers, bail out */
        failf(data, "Too many PUSH_PROMISE headers");
        free_தள்ளு_headers(stream);
        திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      stream->தள்ளு_headers_alloc *= 2;
      headp = curlx_realloc(stream->தள்ளு_headers,
                            stream->தள்ளு_headers_alloc * அளவு(எழுத்து *));
      எனில்(!headp) {
        free_தள்ளு_headers(stream);
        திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      stream->தள்ளு_headers = headp;
    }
    h = curl_maprintf("%s:%s", name, மதிப்பு);
    எனில்(!h) {
      free_தள்ளு_headers(stream);
      திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    stream->தள்ளு_headers[stream->தள்ளு_headers_used++] = h;
    திரும்பு 0;
  }

  எனில்(stream->bodystarted) {
    /* This is a trailer */
    CURL_TRC_CF(data, cf, "[%d] trailer: %.*s: %.*s",
                stream->id, (முழு)namelen, name, (முழு)valuelen, மதிப்பு);
    result = Curl_dynhds_add(&stream->resp_trailers,
                             (மாறிலி எழுத்து *)name, namelen,
                             (மாறிலி எழுத்து *)மதிப்பு, valuelen);
    எனில்(result) {
      cf_h2_header_error(cf, data, stream, result);
      திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;
    }

    திரும்பு 0;
  }

  எனில்(namelen == CURL_CSTRLEN(HTTP_PSEUDO_STATUS) &&
     !memcmp(HTTP_PSEUDO_STATUS, name, namelen)) {
    /* nghttp2 guarantees :status is received first and only once. */
    எழுத்து buffer[32];
    size_t hlen;
    result = Curl_http_decode_status(&stream->status_code,
                                     (மாறிலி எழுத்து *)மதிப்பு, valuelen);
    எனில்(result) {
      cf_h2_header_error(cf, data, stream, result);
      திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    hlen = curl_msnprintf(buffer, அளவு(buffer), HTTP_PSEUDO_STATUS ":%d\r",
                          stream->status_code);
    result = Curl_headers_தள்ளு(data, buffer, hlen, CURLH_PSEUDO);
    எனில்(result) {
      cf_h2_header_error(cf, data, stream, result);
      திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    curlx_dyn_reset(&ctx->scratch);
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/2 "));
    எனில்(!result)
      result = curlx_dyn_addn(&ctx->scratch, மதிப்பு, valuelen);
    எனில்(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    எனில்(!result)
      h2_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    எனில்(result) {
      cf_h2_header_error(cf, data, stream, result);
      திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    /* if we receive data for another handle, wake that up */
    எனில்(CF_DATA_CURRENT(cf) != data)
      Curl_multi_mark_dirty(data);

    CURL_TRC_CF(data, cf, "[%d] status: HTTP/2 %03d",
                stream->id, stream->status_code);
    திரும்பு 0;
  }

  /* nghttp2 guarantees that namelen > 0, and :status was already
     received, and this is not pseudo-header field . */
  /* convert to an HTTP1-style header */
  curlx_dyn_reset(&ctx->scratch);
  result = curlx_dyn_addn(&ctx->scratch, (மாறிலி எழுத்து *)name, namelen);
  எனில்(!result)
    result = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
  எனில்(!result)
    result = curlx_dyn_addn(&ctx->scratch, (மாறிலி எழுத்து *)மதிப்பு, valuelen);
  எனில்(!result)
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
  எனில்(!result)
    h2_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                          curlx_dyn_len(&ctx->scratch), FALSE);
  எனில்(result) {
    cf_h2_header_error(cf, data, stream, result);
    திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  /* if we receive data for another handle, wake that up */
  எனில்(CF_DATA_CURRENT(cf) != data)
    Curl_multi_mark_dirty(data);

  CURL_TRC_CF(data, cf, "[%d] header: %.*s: %.*s",
              stream->id, (முழு)namelen, name, (முழு)valuelen, மதிப்பு);

  திரும்பு 0; /* 0 is successful */
}

நிலையான ssize_t req_body_read_callback(nghttp2_session *session,
                                      int32_t stream_id,
                                      uint8_t *buf, size_t length,
                                      uint32_t *data_flags,
                                      nghttp2_data_source *source,
                                      வெறுமை *userp)
{
  கட்டமைப்பு Curl_cfilter *cf = userp;
  கட்டமைப்பு Curl_easy *data_s;
  கட்டமைப்பு h2_stream_ctx *stream = வெற்று;
  CURLcode result;
  ssize_t nread;
  size_t n;
  (வெறுமை)source;

  (வெறுமை)session;
  எனில்(!stream_id)
    திரும்பு NGHTTP2_ERR_INVALID_ARGUMENT;

  /* get the stream from the hash based on Stream ID, stream ID zero is for
     connection-oriented stuff */
  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  எனில்(!data_s || !stream)
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_bufq_read(&stream->sendbuf, buf, length, &n);
  எனில்(result) {
    எனில்(result != CURLE_AGAIN)
      திரும்பு NGHTTP2_ERR_CALLBACK_FAILURE;
    nread = 0;
  }
  இல்லையெனில்
    nread = (ssize_t)n;

  CURL_TRC_CF(data_s, cf, "[%d] req_body_read(len=%zu) eos=%d -> %zd, %d",
              stream_id, length, stream->body_eos, nread, (முழு)result);

  எனில்(stream->body_eos && Curl_bufq_is_empty(&stream->sendbuf)) {
    *data_flags = NGHTTP2_DATA_FLAG_EOF;
    திரும்பு nread;
  }
  திரும்பு (nread == 0) ? NGHTTP2_ERR_DEFERRED : nread;
}

#ifdef CURLVERBOSE
நிலையான முழு error_callback(nghttp2_session *session,
                          மாறிலி எழுத்து *செய்தி,
                          size_t len,
                          வெறுமை *userp)
{
  கட்டமைப்பு Curl_cfilter *cf = userp;
  கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);
  (வெறுமை)session;
  failf(data, "%.*s", (முழு)len, செய்தி);
  திரும்பு 0;
}
#endif

/*
 * Append headers to ask for an HTTP1.1 to HTTP2 upgrade.
 */
CURLcode Curl_http2_request_upgrade(கட்டமைப்பு dynbuf *req,
                                    கட்டமைப்பு Curl_easy *data)
{
  CURLcode result;
  எழுத்து *base64;
  size_t blen;
  கட்டமைப்பு SingleRequest *k = &data->req;
  uint8_t binsettings[H2_BINSETTINGS_LEN];
  ssize_t rc;
  size_t binlen; /* length of the binsettings data */

  rc = populate_binsettings(binsettings, data);
  எனில்(!curlx_sztouz(rc, &binlen) || !binlen) {
    failf(data, "nghttp2 unexpectedly failed on pack_settings_payload");
    curlx_dyn_free(req);
    திரும்பு CURLE_FAILED_INIT;
  }

  result = curlx_base64url_encode(binsettings, binlen, &base64, &blen);
  எனில்(result) {
    curlx_dyn_free(req);
    திரும்பு result;
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

  திரும்பு result;
}

நிலையான CURLcode http2_handle_stream_close(கட்டமைப்பு Curl_cfilter *cf,
                                          கட்டமைப்பு Curl_easy *data,
                                          கட்டமைப்பு h2_stream_ctx *stream,
                                          size_t *pnlen)
{
  CURLcode result;

  *pnlen = 0;
  எனில்(stream->reset) {
    எனில்(stream->error == NGHTTP2_REFUSED_STREAM) {
      infof(data, "HTTP/2 stream %d refused by server, try again on a new "
                  "connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      திரும்பு CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    இல்லையெனில் எனில்(stream->resp_hds_complete && data->req.no_body) {
      CURL_TRC_CF(data, cf, "[%d] error after response headers, but we did "
                  "not want a body anyway, ignore: %s (err %u)",
                  stream->id, nghttp2_http2_strerror(stream->error),
                  stream->error);
      stream->close_handled = TRUE;
      திரும்பு CURLE_OK;
    }
    failf(data, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          stream->id, stream->reset_by_server ? "server" : "curl",
          stream->error, nghttp2_http2_strerror(stream->error));
    திரும்பு stream->error ? CURLE_HTTP2_STREAM :
           (data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2);
  }
  இல்லையெனில் எனில்(!stream->bodystarted) {
    failf(data, "HTTP/2 stream %d was closed cleanly, but before getting "
          "all response header fields, treated as error", stream->id);
    திரும்பு CURLE_HTTP2_STREAM;
  }

  எனில்(Curl_dynhds_மொத்தம்(&stream->resp_trailers)) {
    கட்டமைப்பு dynhds_entry *e;
    கட்டமைப்பு dynbuf dbuf;
    size_t i;

    result = CURLE_OK;
    curlx_dyn_init(&dbuf, DYN_TRAILERS);
    க்கு(i = 0; i < Curl_dynhds_மொத்தம்(&stream->resp_trailers); ++i) {
      e = Curl_dynhds_getn(&stream->resp_trailers, i);
      எனில்(!e)
        நிறுத்து;
      curlx_dyn_reset(&dbuf);
      result = curlx_dyn_addf(&dbuf, "%.*s: %.*s\x0d\x0a",
                              (முழு)e->namelen, e->name,
                              (முழு)e->valuelen, e->மதிப்பு);
      எனில்(result)
        நிறுத்து;
      Curl_debug(data, CURLINFO_HEADER_IN, curlx_dyn_ptr(&dbuf),
                 curlx_dyn_len(&dbuf));
      result = Curl_client_write(data,
                                 CLIENTWRITE_HEADER | CLIENTWRITE_TRAILER,
                                 curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf));
      எனில்(result)
        நிறுத்து;
      result = Curl_bump_headersize(data, curlx_dyn_len(&dbuf), FALSE);
      எனில்(result)
        திரும்பு result;
    }
    curlx_dyn_free(&dbuf);
    எனில்(result)
      செல் out;
  }

  stream->close_handled = TRUE;
  result = CURLE_OK;

out:
  CURL_TRC_CF(data, cf, "handle_stream_close -> %d, %zu", (முழு)result, *pnlen);
  திரும்பு result;
}

நிலையான முழு sweight_wanted(மாறிலி கட்டமைப்பு Curl_easy *data)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  திரும்பு data->set.weight ?
    data->set.weight : NGHTTP2_DEFAULT_WEIGHT;
}

நிலையான முழு sweight_in_effect(மாறிலி கட்டமைப்பு Curl_easy *data)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  திரும்பு data->state.weight ?
    data->state.weight : NGHTTP2_DEFAULT_WEIGHT;
}

/*
 * h2_pri_spec() fills in the pri_spec struct, used by nghttp2 to send weight
 * and dependency to the peer. It also stores the updated values in the state
 * struct.
 */

நிலையான வெறுமை h2_pri_spec(கட்டமைப்பு Curl_easy *data,
                        nghttp2_priority_spec *pri_spec)
{
  முழு prio = data->set.weight;
  nghttp2_priority_spec_init(pri_spec, 0, sweight_wanted(data), FALSE);
  data->state.weight = prio;
}

/*
 * Check if there is been an update in the priority /
 * dependency settings and if so it submits a PRIORITY frame with the updated
 * info.
 * Flush any out data pending in the network buffer.
 */
நிலையான CURLcode h2_progress_egress(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  முழு rv = 0;

  எனில்(sweight_wanted(data) != sweight_in_effect(data)) {
    கட்டமைப்பு h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    எனில்(stream && stream->id > 0 &&
       (sweight_wanted(data) != sweight_in_effect(data))) {
      /* send new weight and/or dependency */
      nghttp2_priority_spec pri_spec;

      h2_pri_spec(data, &pri_spec);
      CURL_TRC_CF(data, cf, "[%d] Queuing PRIORITY", stream->id);
      DEBUGASSERT(stream->id != -1);
      rv = Curl_ngh2_submit_priority(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, &pri_spec);
      எனில்(rv)
        செல் out;
    }
  }

  ctx->nw_out_blocked = 0;
  போது(!rv && !ctx->nw_out_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

out:
  எனில்(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(data, cf, "nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    திரும்பு CURLE_SEND_ERROR;
  }
  /* Defer flushing during the connect phase so that the SETTINGS and
   * other initial frames are sent together with the first request.
   * Unless we are 'connect_only' where the request will never come. */
  எனில்(!cf->connected && !cf->conn->bits.connect_only)
    திரும்பு CURLE_OK;
  திரும்பு nw_out_flush(cf, data);
}

நிலையான CURLcode stream_recv(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு h2_stream_ctx *stream,
                            எழுத்து *buf, size_t len, size_t *pnread)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_AGAIN;

  (வெறுமை)buf;
  (வெறுமை)len;
  *pnread = 0;

  எனில்(!stream->xfer_result)
    stream->xfer_result = cf_h2_update_local_win(cf, data, stream);

  எனில்(stream->xfer_result) {
    CURL_TRC_CF(data, cf, "[%d] xfer write failed", stream->id);
    result = stream->xfer_result;
  }
  இல்லையெனில் எனில்(stream->closed) {
    CURL_TRC_CF(data, cf, "[%d] returning CLOSE", stream->id);
    result = http2_handle_stream_close(cf, data, stream, pnread);
  }
  இல்லையெனில் எனில்(stream->reset ||
          (ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) ||
          (ctx->rcvd_goaway && ctx->remote_max_sid < stream->id)) {
    CURL_TRC_CF(data, cf, "[%d] returning ERR", stream->id);
    result = data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2;
  }

  எனில்(result && (result != CURLE_AGAIN))
    CURL_TRC_CF(data, cf, "[%d] stream_recv(len=%zu) -> %d, %zu",
                stream->id, len, (முழு)result, *pnread);
  திரும்பு result;
}

நிலையான CURLcode h2_progress_ingress(கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data,
                                    size_t data_max_bytes)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  எனில்(should_close_session(ctx)) {
    CURL_TRC_CF(data, cf, "[0] ingress: session is closed");
    திரும்பு CURLE_HTTP2;
  }

  /* Process network input buffer first */
  எனில்(!Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "Process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    result = h2_process_pending_உள்ளீடு(cf, data);
    எனில்(result)
      திரும்பு result;
  }

  எனில்(!data_max_bytes)
    data_max_bytes = H2_CHUNK_SIZE;

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop due to connection close or us not processing
   * all network input */
  போது(!ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) {
    கட்டமைப்பு h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    எனில்(stream && (stream->closed || !data_max_bytes)) {
      /* We would like to abort here and stop processing, so that the transfer
       * loop can handle the data/close here. This may leave data in
       * underlying buffers that will not be consumed. */
      எனில்(!cf->next || !cf->next->cft->has_data_pending(cf->next, data))
        Curl_multi_mark_dirty(data);
      நிறுத்து;
    }

    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    எனில்(result) {
      எனில்(result != CURLE_AGAIN) {
        failf(data, "Failed receiving HTTP2 data: %d(%s)", (முழு)result,
              curl_easy_strerror(result));
        திரும்பு result;
      }
      நிறுத்து;
    }
    இல்லையெனில் எனில்(nread == 0) {
      CURL_TRC_CF(data, cf, "[0] ingress: connection closed");
      ctx->conn_closed = TRUE;
      நிறுத்து;
    }
    இல்லையெனில் {
      CURL_TRC_CF(data, cf, "[0] ingress: read %zu bytes", nread);
      data_max_bytes = (data_max_bytes > nread) ? (data_max_bytes - nread) : 0;
    }

    result = h2_process_pending_உள்ளீடு(cf, data);
    எனில்(result)
      திரும்பு result;
    CURL_TRC_CF(data, cf, "[0] ingress: nw-in buffered %zu",
                Curl_bufq_len(&ctx->inbufq));
  }

  எனில்(ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "server closed %s",
                ctx->rcvd_goaway ? "with GOAWAY" : "abruptly");
    connclose(cf->conn);
  }

  CURL_TRC_CF(data, cf, "[0] ingress: done");
  திரும்பு CURLE_OK;
}

நிலையான CURLcode cf_h2_recv(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                           எழுத்து *buf, size_t len, size_t *pnread)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  கட்டமைப்பு h2_stream_ctx *stream;
  CURLcode result, r2;
  கட்டமைப்பு cf_call_data save;

  எனில்(!data)
    திரும்பு CURLE_HTTP2;

  stream = H2_STREAM_CTX(ctx, data);

  *pnread = 0;
  எனில்(!stream) {
    /* Abnormal call sequence: either this transfer has never opened a stream
     * (unlikely) or the transfer has been done, cleaned up its resources, but
     * a read() is called anyway. It is not clear what the calling sequence
     * is for such a case. */
    failf(data, "http/2 recv on a transfer never opened "
          "or already cleared, mid=%u", data->mid);
    திரும்பு CURLE_HTTP2;
  }

  CF_DATA_SAVE(save, cf, data);

  result = stream_recv(cf, data, stream, buf, len, pnread);
  எனில்(result && (result != CURLE_AGAIN))
    செல் out;

  எனில்(result) {
    result = h2_progress_ingress(cf, data, len);
    எனில்(result)
      செல் out;

    result = stream_recv(cf, data, stream, buf, len, pnread);
  }

  எனில்(*pnread > 0) {
    /* Now that we transferred this to the upper layer, we report
     * the actual amount of DATA consumed to the H2 session, so
     * that it adjusts stream flow control */
    Curl_ngh2_session_consume(&ctx->ngh2, stream->id, *pnread);
    எனில்(stream->closed) {
      CURL_TRC_CF(data, cf, "[%d] DRAIN closed stream", stream->id);
      Curl_multi_mark_dirty(data);
    }
  }

out:
  r2 = h2_progress_egress(cf, data);
  எனில்(r2 == CURLE_AGAIN) {
    /* pending data to send, need to be called again. Ideally, we
     * monitor the socket for POLLOUT, but when not SENDING
     * any more, we force processing of the transfer. */
    எனில்(!CURL_REQ_WANT_SEND(data))
      Curl_multi_mark_dirty(data);
  }
  இல்லையெனில் எனில்(r2 && (result == CURLE_AGAIN)) {
    /* Do not replace a previous error result NOR a successful result
     * with an egress error. Example: the server might close the
     * connection after sending the complete response with EOS
     * and everything. Further sending may fail, but that MUST
     * not change that we had a successful response. */
    result = r2;
  }

  CURL_TRC_CF(data, cf, "[%d] cf_recv(len=%zu) -> %d, %zu, "
              "window=%d/%d, connection %d/%d",
              stream->id, len, (முழு)result, *pnread,
              Curl_ngh2_session_get_stream_effective_recv_data_length(
                &ctx->ngh2, stream->id),
              Curl_ngh2_session_get_stream_effective_local_window_size(
                &ctx->ngh2, stream->id),
              Curl_ngh2_session_get_local_window_size(&ctx->ngh2),
              HTTP2_HUGE_WINDOW_SIZE);

  CF_DATA_RESTORE(cf, save);
  திரும்பு result;
}

நிலையான CURLcode cf_h2_body_send(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு h2_stream_ctx *stream,
                                மாறிலி வெறுமை *buf, size_t blen, bool eos,
                                size_t *pnwritten)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  CURLcode result;

  *pnwritten = 0;
  எனில்(stream->closed) {
    எனில்(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(data, cf, "[%d] discarding data"
                  "on closed stream with response", stream->id);
      எனில்(eos)
        stream->body_eos = TRUE;
      *pnwritten = blen;
      திரும்பு CURLE_OK;
    }
    /* Server closed before we got a response, this is an error */
    infof(data, "stream %d closed", stream->id);
    திரும்பு CURLE_SEND_ERROR;
  }

  result = Curl_bufq_write(&stream->sendbuf, buf, blen, pnwritten);
  எனில்(result)
    திரும்பு result;

  எனில்(eos && (blen == *pnwritten))
    stream->body_eos = TRUE;

  எனில்(eos || !Curl_bufq_is_empty(&stream->sendbuf)) {
    /* resume the potentially suspended stream */
    எனில்(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id)))
      திரும்பு CURLE_SEND_ERROR;
  }

  திரும்பு CURLE_OK;
}

நிலையான CURLcode h2_submit(கட்டமைப்பு h2_stream_ctx **pstream,
                          கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                          மாறிலி uint8_t *buf, size_t len,
                          bool eos, size_t *pnwritten)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  கட்டமைப்பு h2_stream_ctx *stream = வெற்று;
  கட்டமைப்பு dynhds h2_headers;
  nghttp2_nv *nva = வெற்று;
  மாறிலி வெறுமை *body = வெற்று;
  size_t nheader, bodylen;
  nghttp2_data_provider data_prd;
  int32_t stream_id;
  nghttp2_priority_spec pri_spec;
  size_t nwritten;
  CURLcode result = CURLE_OK;
  uint32_t initial_win_size;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_headers, MAX_HTTP_RESP_HEADER_மொத்தம், DYN_HTTP_REQUEST);

  result = http2_data_setup(cf, data, &stream);
  எனில்(result)
    செல் out;

  result = Curl_h1_req_parse_read(
    &stream->h1, buf, len, வெற்று,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : வெற்று,
    0, &nwritten);
  எனில்(result)
    செல் out;
  *pnwritten = nwritten;
  எனில்(!stream->h1.done) {
    /* need more data */
    செல் out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  எனில்(result)
    செல் out;
  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nva = Curl_dynhds_to_nva(&h2_headers, &nheader);
  எனில்(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }

  h2_pri_spec(data, &pri_spec);
  எனில்(!Curl_ngh2_session_check_request_allowed(&ctx->ngh2))
    CURL_TRC_CF(data, cf, "send request NOT allowed (via nghttp2)");

  /* Check the initial windows size of the transfer (rate-limits?) and
   * send an updated settings on changes from previous value. */
  initial_win_size = cf_h2_initial_win_size(data);
  எனில்(initial_win_size != ctx->initial_win_size) {
    result = cf_h2_update_settings(ctx, initial_win_size);
    எனில்(result)
      செல் out;
  }

  தேர்வு(data->state.httpreq) {
  நிலை HTTPREQ_POST:
  நிலை HTTPREQ_POST_FORM:
  நிலை HTTPREQ_POST_MIME:
  நிலை HTTPREQ_PUT:
    data_prd.read_callback = req_body_read_callback;
    data_prd.source.ptr = வெற்று;
    stream_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, &data_prd, stream);
    நிறுத்து;
  இயல்பு:
    stream_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, வெற்று, stream);
  }

  எனில்(stream_id < 0) {
    CURL_TRC_CF(data, cf, "send: nghttp2_submit_request error (%s)%d",
                nghttp2_strerror(stream_id), stream_id);
    result = CURLE_SEND_ERROR;
    செல் out;
  }

#ifdef CURLVERBOSE
#define MAX_ACC 60000  /* <64KB to account for some overhead */
  எனில்(Curl_trc_is_verbose(data)) {
    size_t acc = 0, i;

    infof(data, "[HTTP/2] [%d] OPENED stream for %s",
          stream_id, Curl_bufref_ptr(&data->state.url));
    க்கு(i = 0; i < nheader; ++i) {
      acc += nva[i].namelen + nva[i].valuelen;

      infof(data, "[HTTP/2] [%d] [%.*s: %.*s]", stream_id,
            (முழு)nva[i].namelen, nva[i].name,
            (முழு)nva[i].valuelen, nva[i].மதிப்பு);
    }

    எனில்(acc > MAX_ACC) {
      infof(data, "[HTTP/2] Warning: The cumulative length of all "
            "headers exceeds %d bytes and that could cause the "
            "stream to be rejected.", MAX_ACC);
    }
  }
#endif

  stream->id = stream_id;

  body = (மாறிலி எழுத்து *)buf + *pnwritten;
  bodylen = len - *pnwritten;

  எனில்(bodylen || eos) {
    size_t n;
    result = cf_h2_body_send(cf, data, stream, body, bodylen, eos, &n);
    எனில்(!result)
      *pnwritten += n;
    இல்லையெனில் எனில்(result == CURLE_AGAIN)
      result = CURLE_OK;
    இல்லையெனில் {
      result = CURLE_SEND_ERROR;
    }
  }

out:
  CURL_TRC_CF(data, cf, "[%d] submit -> %d, %zu",
              stream ? stream->id : -1, (முழு)result, *pnwritten);
  curlx_safefree(nva);
  *pstream = stream;
  Curl_dynhds_free(&h2_headers);
  திரும்பு result;
}

நிலையான CURLcode cf_h2_send(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                           மாறிலி uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  கட்டமைப்பு h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
  கட்டமைப்பு cf_call_data save;
  CURLcode result = CURLE_OK, r2;

  CF_DATA_SAVE(save, cf, data);
  *pnwritten = 0;

  எனில்(!stream || stream->id == -1) {
    result = h2_submit(&stream, cf, data, buf, len, eos, pnwritten);
    எனில்(result)
      செல் out;
    DEBUGASSERT(stream);
  }
  இல்லையெனில் எனில்(stream->body_eos) {
    /* We already wrote this, but CURLE_AGAIN-ed the call due to not
     * being able to flush stream->sendbuf. Make a 0-length write
     * to trigger flushing again.
     * If this works, we report to have written `len` bytes. */
    size_t n;
    DEBUGASSERT(eos);
    result = cf_h2_body_send(cf, data, stream, buf, 0, eos, &n);
    CURL_TRC_CF(data, cf, "[%d] cf_body_send last CHUNK -> %d, %zu, eos=%d",
                stream->id, (முழு)result, n, eos);
    எனில்(result)
      செல் out;
    *pnwritten = len;
  }
  இல்லையெனில் {
    result = cf_h2_body_send(cf, data, stream, buf, len, eos, pnwritten);
    CURL_TRC_CF(data, cf, "[%d] cf_body_send(len=%zu) -> %d, %zu, eos=%d",
                stream->id, len, (முழு)result, *pnwritten, eos);
  }

  /* Call the nghttp2 send loop and flush to write ALL buffered data,
   * headers and/or request body completely out to the network */
  r2 = h2_progress_egress(cf, data);

  /* if the stream has been closed in egress handling (nghttp2 does that
   * when it does not like the headers, for example */
  எனில்(stream && stream->closed && !stream->resp_hds_complete) {
    infof(data, "stream %d closed", stream->id);
    result = CURLE_SEND_ERROR;
    செல் out;
  }
  இல்லையெனில் எனில்(r2 && (r2 != CURLE_AGAIN)) {
    result = r2;
    செல் out;
  }

  எனில்(should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    எனில்(stream && stream->closed) {
      result = http2_handle_stream_close(cf, data, stream, pnwritten);
    }
    இல்லையெனில் {
      CURL_TRC_CF(data, cf, "send: nothing to do in this session");
      result = CURLE_HTTP2;
    }
  }

out:
  எனில்(stream) {
    CURL_TRC_CF(data, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
                "eos=%d, h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                stream->id, len, (முழு)result, *pnwritten,
                stream->body_eos,
                Curl_ngh2_session_get_stream_remote_window_size(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&stream->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  இல்லையெனில் {
    CURL_TRC_CF(data, cf, "cf_send(len=%zu) -> %d, %zu, "
                "connection-window=%d, nw_send_buffer(%zu)",
                len, (முழு)result, *pnwritten,
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_DATA_RESTORE(cf, save);
  திரும்பு result;
}

நிலையான CURLcode cf_h2_flush(கட்டமைப்பு Curl_cfilter *cf,
                            கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  கட்டமைப்பு h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
  கட்டமைப்பு cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  எனில்(stream && !Curl_bufq_is_empty(&stream->sendbuf)) {
    /* resume the potentially suspended stream */
    எனில்(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id))) {
      result = CURLE_SEND_ERROR;
      செல் out;
    }
  }

  result = h2_progress_egress(cf, data);

out:
  எனில்(stream) {
    CURL_TRC_CF(data, cf, "[%d] flush -> %d, "
                "h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                stream->id, (முழு)result,
                Curl_ngh2_session_get_stream_remote_window_size(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&stream->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  இல்லையெனில் {
    CURL_TRC_CF(data, cf, "flush -> %d, "
                "connection-window=%d, nw_send_buffer(%zu)", (முழு)result,
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_DATA_RESTORE(cf, save);
  திரும்பு result;
}

நிலையான CURLcode cf_h2_adjust_pollset(கட்டமைப்பு Curl_cfilter *cf,
                                     கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு easy_pollset *ps)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  கட்டமைப்பு cf_call_data save;
  curl_socket_t sock;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  எனில்(!Curl_ngh2_valid(&ctx->ngh2))
    திரும்பு CURLE_OK;

  sock = Curl_conn_cf_get_socket(cf, data);
  Curl_pollset_check(data, ps, sock, &want_recv, &want_send);
  எனில்(want_recv || want_send) {
    கட்டமைப்பு h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
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
  இல்லையெனில் எனில்(ctx->sent_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_DATA_SAVE(save, cf, data);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CF_DATA_RESTORE(cf, save);
  }
  திரும்பு result;
}

நிலையான CURLcode cf_h2_ctx_open(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  கட்டமைப்பு h2_stream_ctx *stream;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  முழு rc;
  nghttp2_session_callbacks *cbs = வெற்று;

  DEBUGASSERT(!ctx->ngh2.s);
  DEBUGASSERT(!ctx->ngh2.fatal_rv);
  DEBUGASSERT(ctx->initialized);

  rc = nghttp2_session_callbacks_new(&cbs);
  எனில்(rc) {
    failf(data, "Could not initialize nghttp2 callbacks");
    செல் out;
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
  எனில்(rc) {
    failf(data, "Could not initialize nghttp2");
    செல் out;
  }
  ctx->max_concurrent_streams = data->multi ?
    Curl_multi_max_concurrent_streams(data->multi) :
    DEFAULT_MAX_CONCURRENT_STREAMS;

  எனில்(ctx->via_h1_upgrade) {
    /* HTTP/1.1 Upgrade issued. H2 Settings have already been submitted
     * in the H1 request and we upgrade from there. This stream
     * is opened implicitly as #1. */
    uint8_t binsettings[H2_BINSETTINGS_LEN];
    ssize_t rclen;
    size_t binlen; /* length of the binsettings data */

    rclen = populate_binsettings(binsettings, data);

    எனில்(!curlx_sztouz(rclen, &binlen) || !binlen) {
      failf(data, "nghttp2 unexpectedly failed on pack_settings_payload");
      result = CURLE_FAILED_INIT;
      செல் out;
    }

    result = http2_data_setup(cf, data, &stream);
    எனில்(result)
      செல் out;
    DEBUGASSERT(stream);
    stream->id = 1;
    /* queue SETTINGS frame (again) */
    rc = Curl_ngh2_session_upgrade2(
      &ctx->ngh2, binsettings, binlen, data->state.httpreq == HTTPREQ_HEAD,
      வெற்று);
    எனில்(rc) {
      failf(data, "nghttp2_session_upgrade2() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      செல் out;
    }

    rc = Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, stream->id, stream);
    எனில்(rc) {
      infof(data, "http/2: failed to set user_data for stream %d",
            stream->id);
      DEBUGASSERT(0);
    }
    CURL_TRC_CF(data, cf, "created session via Upgrade");
  }
  இல்லையெனில் {
    nghttp2_settings_entry iv[H2_SETTINGS_IV_LEN];
    size_t ivlen;

    ivlen = populate_settings(iv, data, ctx);
    rc = Curl_ngh2_submit_settings(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, iv, ivlen);
    எனில்(rc) {
      failf(data, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      செல் out;
    }
  }

  rc = Curl_ngh2_session_set_local_window_size(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, HTTP2_HUGE_WINDOW_SIZE);
  எனில்(rc) {
    failf(data, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    result = CURLE_HTTP2;
    செல் out;
  }

  /* all set, traffic will be send on connect */
  result = CURLE_OK;
  CURL_TRC_CF(data, cf, "[0] created h2 session%s",
              ctx->via_h1_upgrade ? " (via h1 upgrade)" : "");

out:
  எனில்(cbs)
    nghttp2_session_callbacks_del(cbs);
  திரும்பு result;
}

நிலையான CURLcode cf_h2_connect(கட்டமைப்பு Curl_cfilter *cf,
                              கட்டமைப்பு Curl_easy *data,
                              bool *done)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  கட்டமைப்பு cf_call_data save;
  bool first_time = FALSE;

  எனில்(cf->connected) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  /* Connect the lower filters first */
  எனில்(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    எனில்(result || !*done)
      திரும்பு result;
  }

  *done = FALSE;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(ctx->initialized);
  எனில்(!ctx->ngh2.s) {
    result = cf_h2_ctx_open(cf, data);
    எனில்(result)
      செல் out;
    first_time = TRUE;
  }

  எனில்(!first_time) {
    result = h2_progress_ingress(cf, data, 0);
    எனில்(result)
      செல் out;
  }

  /* Send out our SETTINGS and ACKs and such. If that blocks, we
   * have it buffered and can count this filter as being connected */
  result = h2_progress_egress(cf, data);
  எனில்(result && (result != CURLE_AGAIN))
    செல் out;

  *done = TRUE;
  cf->connected = TRUE;
  result = CURLE_OK;

out:
  CURL_TRC_CF(data, cf, "cf_connect() -> %d, %d, ", (முழு)result, *done);
  CF_DATA_RESTORE(cf, save);
  திரும்பு result;
}

நிலையான வெறுமை cf_h2_destroy(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;

  (வெறுமை)data;
  எனில்(ctx) {
    cf_h2_ctx_free(ctx);
    cf->ctx = வெற்று;
  }
}

நிலையான CURLcode cf_h2_shutdown(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data, bool *done)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  கட்டமைப்பு cf_call_data save;
  CURLcode result;
  முழு rv;

  எனில்(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);

  எனில்(!ctx->sent_goaway) {
    ctx->sent_goaway = TRUE;
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, ctx->local_max_sid, 0,
      (மாறிலி uint8_t *)"shutdown", அளவு("shutdown"));
    எனில்(rv) {
      failf(data, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      result = CURLE_SEND_ERROR;
      செல் out;
    }
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  result = CURLE_OK;
  எனில்(Curl_ngh2_session_want_write(&ctx->ngh2) ||
     !Curl_bufq_is_empty(&ctx->outbufq))
    result = h2_progress_egress(cf, data);
  எனில்(!result && Curl_ngh2_session_want_read(&ctx->ngh2))
    result = h2_progress_ingress(cf, data, 0);

  எனில்(result == CURLE_AGAIN)
    result = CURLE_OK;

  *done = (ctx->conn_closed ||
           (!result && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2) &&
            Curl_bufq_is_empty(&ctx->outbufq)));

out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  திரும்பு result;
}

நிலையான CURLcode http2_data_pause(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 bool pause)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  கட்டமைப்பு h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);

  DEBUGASSERT(data);
  எனில்(ctx && Curl_ngh2_valid(&ctx->ngh2) && stream) {
    CURLcode result;

    stream->write_paused = pause;
    result = cf_h2_update_local_win(cf, data, stream);
    எனில்(result)
      திரும்பு result;

    /* attempt to send the window update */
    (வெறுமை)h2_progress_egress(cf, data);

    எனில்(!pause) {
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
  திரும்பு CURLE_OK;
}

நிலையான CURLcode cf_h2_cntrl(கட்டமைப்பு Curl_cfilter *cf,
                            கட்டமைப்பு Curl_easy *data,
                            முழு event, முழு arg1, வெறுமை *arg2)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு cf_call_data save;

  (வெறுமை)arg2;

  CF_DATA_SAVE(save, cf, data);
  தேர்வு(event) {
  நிலை CF_CTRL_DATA_SETUP:
    நிறுத்து;
  நிலை CF_CTRL_DATA_PAUSE:
    result = http2_data_pause(cf, data, (arg1 != 0));
    நிறுத்து;
  நிலை CF_CTRL_FLUSH:
    result = cf_h2_flush(cf, data);
    நிறுத்து;
  நிலை CF_CTRL_DATA_DONE:
    http2_data_done(cf, data);
    நிறுத்து;
  நிலை CF_CTRL_CONN_INFO_UPDATE:
    எனில்(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 20;
      Curl_conn_set_multiplex(cf->conn);
    }
    நிறுத்து;
  இயல்பு:
    நிறுத்து;
  }
  CF_DATA_RESTORE(cf, save);
  திரும்பு result;
}

நிலையான bool cf_h2_data_pending(கட்டமைப்பு Curl_cfilter *cf,
                               மாறிலி கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;

  எனில்(ctx && !Curl_bufq_is_empty(&ctx->inbufq))
    திரும்பு TRUE;
  திரும்பு cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

நிலையான bool cf_h2_is_alive(கட்டமைப்பு Curl_cfilter *cf,
                           கட்டமைப்பு Curl_easy *data,
                           bool *உள்ளீடு_pending)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  bool alive;
  கட்டமைப்பு cf_call_data save;

  *உள்ளீடு_pending = FALSE;
  CF_DATA_SAVE(save, cf, data);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
           http2_connisalive(cf, data, உள்ளீடு_pending));
  CURL_TRC_CF(data, cf, "conn alive -> %d, input_pending=%d",
              alive, *உள்ளீடு_pending);
  CF_DATA_RESTORE(cf, save);
  திரும்பு alive;
}

நிலையான CURLcode cf_h2_keep_alive(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data)
{
  CURLcode result;
  கட்டமைப்பு cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  result = http2_send_ping(cf, data);
  CF_DATA_RESTORE(cf, save);
  திரும்பு result;
}

நிலையான CURLcode cf_h2_query(கட்டமைப்பு Curl_cfilter *cf,
                            கட்டமைப்பு Curl_easy *data,
                            முழு query, முழு *pres1, வெறுமை *pres2)
{
  கட்டமைப்பு cf_h2_ctx *ctx = cf->ctx;
  கட்டமைப்பு cf_call_data save;
  size_t effective_max;

  தேர்வு(query) {
  நிலை CF_QUERY_MAX_CONCURRENT:
    DEBUGASSERT(pres1);

    CF_DATA_SAVE(save, cf, data);
    எனில்(!Curl_ngh2_valid(&ctx->ngh2) ||
       !Curl_ngh2_session_check_request_allowed(&ctx->ngh2)) {
      /* the limit is what we have in use right now */
      effective_max = cf->conn->attached_xfers;
    }
    இல்லையெனில் {
      effective_max = ctx->max_concurrent_streams;
    }
    *pres1 = (effective_max > INT_MAX) ? INT_MAX : (முழு)effective_max;
    CF_DATA_RESTORE(cf, save);
    திரும்பு CURLE_OK;
  நிலை CF_QUERY_STREAM_ERROR: {
    கட்டமைப்பு h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    *pres1 = stream ? (முழு)stream->error : 0;
    திரும்பு CURLE_OK;
  }
  நிலை CF_QUERY_NEED_FLUSH: {
    கட்டமைப்பு h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    எனில்(!Curl_bufq_is_empty(&ctx->outbufq) ||
       (stream && !Curl_bufq_is_empty(&stream->sendbuf))) {
      *pres1 = TRUE;
      திரும்பு CURLE_OK;
    }
    நிறுத்து;
  }
  நிலை CF_QUERY_HTTP_VERSION:
    *pres1 = 20;
    திரும்பு CURLE_OK;
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

கட்டமைப்பு Curl_cftype Curl_cft_nghttp2 = {
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

நிலையான CURLcode http2_cfilter_add(கட்டமைப்பு Curl_cfilter **pcf,
                                  கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு connectdata *conn,
                                  int8_t sockindex,
                                  bool via_h1_upgrade)
{
  கட்டமைப்பு Curl_cfilter *cf = வெற்று;
  கட்டமைப்பு cf_h2_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(data->conn);
  ctx = curlx_calloc(1, அளவு(*ctx));
  எனில்(!ctx)
    செல் out;
  cf_h2_ctx_init(ctx, via_h1_upgrade);

  result = Curl_cf_create(&cf, &Curl_cft_nghttp2, ctx);
  எனில்(result)
    செல் out;

  ctx = வெற்று;
  Curl_conn_cf_add(data, conn, sockindex, cf);

out:
  எனில்(result)
    cf_h2_ctx_free(ctx);
  *pcf = result ? வெற்று : cf;
  திரும்பு result;
}

நிலையான CURLcode http2_cfilter_செருகு_after(கட்டமைப்பு Curl_cfilter *cf,
                                           கட்டமைப்பு Curl_easy *data,
                                           bool via_h1_upgrade)
{
  கட்டமைப்பு Curl_cfilter *cf_h2 = வெற்று;
  கட்டமைப்பு cf_h2_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (வெறுமை)data;
  ctx = curlx_calloc(1, அளவு(*ctx));
  எனில்(!ctx)
    செல் out;
  cf_h2_ctx_init(ctx, via_h1_upgrade);

  result = Curl_cf_create(&cf_h2, &Curl_cft_nghttp2, ctx);
  எனில்(result)
    செல் out;

  ctx = வெற்று;
  Curl_conn_cf_செருகு_after(cf, cf_h2);

out:
  எனில்(result)
    cf_h2_ctx_free(ctx);
  திரும்பு result;
}

bool Curl_http2_may_switch(கட்டமைப்பு Curl_easy *data)
{
  எனில்(Curl_conn_http_version(data, data->conn) < 20 &&
     (data->state.http_neg.wanted & CURL_HTTP_V2x) &&
     data->state.http_neg.h2_prior_knowledge) {
#ifndef CURL_DISABLE_PROXY
    எனில்(data->conn->bits.origin_is_proxy) {
      infof(data, "Ignoring HTTP/2 prior knowledge due to proxy");
      திரும்பு FALSE;
    }
#endif
    திரும்பு TRUE;
  }
  திரும்பு FALSE;
}

CURLcode Curl_http2_switch(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_cfilter *cf;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, data->conn) < 20);

  result = http2_cfilter_add(&cf, data, data->conn, FIRSTSOCKET, FALSE);
  எனில்(result)
    திரும்பு result;
  CURL_TRC_CF(data, cf, "switching connection to HTTP/2");

  எனில்(cf->next) {
    bool done;
    திரும்பு Curl_conn_cf_connect(cf, data, &done);
  }
  திரும்பு CURLE_OK;
}

CURLcode Curl_http2_switch_at(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_cfilter *cf_h2;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, data->conn) < 20);

  result = http2_cfilter_செருகு_after(cf, data, FALSE);
  எனில்(result)
    திரும்பு result;

  cf_h2 = cf->next;

  எனில்(cf_h2->next) {
    bool done;
    திரும்பு Curl_conn_cf_connect(cf_h2, data, &done);
  }
  திரும்பு CURLE_OK;
}

CURLcode Curl_http2_upgrade(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு connectdata *conn, int8_t sockindex,
                            மாறிலி எழுத்து *mem, size_t nread)
{
  கட்டமைப்பு Curl_cfilter *cf;
  கட்டமைப்பு cf_h2_ctx *ctx;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, conn) < 20);

  result = http2_cfilter_add(&cf, data, conn, sockindex, TRUE);
  எனில்(result)
    திரும்பு result;
  CURL_TRC_CF(data, cf, "upgrading connection to HTTP/2");

  DEBUGASSERT(cf->cft == &Curl_cft_nghttp2);
  ctx = cf->ctx;

  data->req.httpversion_sent = 20; /* it is an h2 request now */
  data->req.header = TRUE; /* we expect the real response to come in h2 */
  data->req.headerline = 0; /* restart the header line counter */

  எனில்(nread > 0) {
    /* Remaining data from the protocol switch reply is already using
     * the switched protocol, ie. HTTP/2. We add that to the network
     * inbufq. */
    size_t copied;

    result = Curl_bufq_write(&ctx->inbufq,
                             (மாறிலி unsigned எழுத்து *)mem, nread, &copied);
    எனில்(result) {
      failf(data, "error on copying HTTP Upgrade response: %d", (முழு)result);
      திரும்பு CURLE_RECV_ERROR;
    }
    எனில்(copied < nread) {
      failf(data, "connection buffer size could not take all data "
            "from HTTP Upgrade response header: copied=%zu, datalen=%zu",
            copied, nread);
      திரும்பு CURLE_HTTP2;
    }
    infof(data, "Copied HTTP/2 data in stream buffer to connection buffer"
          " after upgrade: len=%zu", nread);
  }

  எனில்(cf->next) {
    bool done;
    result = Curl_conn_cf_connect(cf, data, &done);
    எனில்(!result)
      cf->cft->cntrl(cf, data, CF_CTRL_CONN_INFO_UPDATE, 0, வெற்று);
  }
  திரும்பு result;
}

/* Only call this function for a transfer that already got an HTTP/2
   CURLE_HTTP2_STREAM error! */
bool Curl_h2_http_1_1_error(கட்டமைப்பு Curl_easy *data)
{
  எனில்(Curl_conn_http_version(data, data->conn) == 20) {
    முழு err = Curl_conn_get_stream_error(data, data->conn, FIRSTSOCKET);
    திரும்பு err == NGHTTP2_HTTP_1_1_REQUIRED;
  }
  திரும்பு FALSE;
}

வெறுமை *Curl_nghttp2_malloc(size_t size, வெறுமை *user_data)
{
  (வெறுமை)user_data;
  திரும்பு Curl_cmalloc(size);
}

வெறுமை Curl_nghttp2_free(வெறுமை *ptr, வெறுமை *user_data)
{
  (வெறுமை)user_data;
  Curl_cfree(ptr);
}

வெறுமை *Curl_nghttp2_calloc(size_t nmemb, size_t size, வெறுமை *user_data)
{
  (வெறுமை)user_data;
  திரும்பு Curl_ccalloc(nmemb, size);
}

வெறுமை *Curl_nghttp2_realloc(வெறுமை *ptr, size_t size, வெறுமை *user_data)
{
  (வெறுமை)user_data;
  திரும்பு Curl_crealloc(ptr, size);
}

#இல்லையெனில் /* CURL_DISABLE_HTTP || !USE_NGHTTP2 */

/* Satisfy external references even if http2 is not compiled in. */

எழுத்து *curl_pushheader_bynum(கட்டமைப்பு curl_pushheaders *h, size_t இலக்கம்)
{
  (வெறுமை)h;
  (வெறுமை)இலக்கம்;
  திரும்பு வெற்று;
}

எழுத்து *curl_pushheader_byname(கட்டமைப்பு curl_pushheaders *h, மாறிலி எழுத்து *name)
{
  (வெறுமை)h;
  (வெறுமை)name;
  திரும்பு வெற்று;
}

#endif /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
