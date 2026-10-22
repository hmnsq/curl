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

#اگر NGHTTP2_VERSION_رقم < 0x010f00
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
#اگر NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
#define H2_STREAM_WINDOW_SIZE_INITIAL  (64 * 1024)
#وگرنه
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

ساختار cf_h2_ctx {
  ساختار curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  ساختار cf_call_data call_data;

  ساختار bufq inbufq;           /* network input */
  ساختار bufq outbufq;          /* network output */
  ساختار bufc_pool stream_bufcp; /* spares for stream buffers */
  ساختار dynbuf scratch;        /* scratch buffer for temp use */

  ساختار u32_ptrset streams; /* hash of `data->mid` to `h2_stream_ctx` */
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
  BIT(enable_افزودن);
  BIT(nw_out_blocked);
};

/* How to access `call_data` from a cf_h2 filter */
#undef CF_CTX_CALL_DATA
#define CF_CTX_CALL_DATA(cf) ((ساختار cf_h2_ctx *)(cf)->ctx)->call_data

/**
 * All about the H2 internals of a stream
 */
ساختار h2_stream_ctx {
  ساختار bufq sendbuf; /* request buffer */
  ساختار h1_req_parser h1; /* parsing the request */
  ساختار dynhds resp_trailers; /* response trailer fields */
  size_t resp_hds_len; /* amount of response header bytes in recvbuf */
  curl_off_t xfer_id;  /* transfer `id` owning this stream */
  curl_off_t nrcvd_data;  /* number of DATA bytes received */

  نویسه **افزودن_headers;       /* allocated array */
  size_t افزودن_headers_used;  /* number of entries filled in */
  size_t افزودن_headers_alloc; /* number of entries allocated */

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

ایستا تهی free_افزودن_headers(ساختار h2_stream_ctx *stream)
{
  size_t i;
  برای(i = 0; i < stream->افزودن_headers_used; i++)
    curlx_free(stream->افزودن_headers[i]);
  curlx_safefree(stream->افزودن_headers);
  stream->افزودن_headers_used = 0;
}

ایستا تهی h2_stream_ctx_free(ساختار h2_stream_ctx *stream)
{
  Curl_bufq_free(&stream->sendbuf);
  Curl_h1_req_parse_free(&stream->h1);
  Curl_dynhds_free(&stream->resp_trailers);
  free_افزودن_headers(stream);
  curlx_free(stream);
}

ایستا تهی h2_stream_hash_free(uint32_t id, تهی *stream)
{
  (تهی)id;
  DEBUGASSERT(stream);
  h2_stream_ctx_free((ساختار h2_stream_ctx *)stream);
}

ایستا تهی cf_h2_ctx_init(ساختار cf_h2_ctx *ctx, bool via_h1_upgrade)
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

ایستا تهی cf_h2_ctx_free(ساختار cf_h2_ctx *ctx)
{
  اگر(ctx && ctx->initialized) {
    Curl_ngh2_del(&ctx->ngh2);
    Curl_bufq_free(&ctx->inbufq);
    Curl_bufq_free(&ctx->outbufq);
    Curl_bufcp_free(&ctx->stream_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    memset(ctx, 0, اندازه(*ctx));
  }
  curlx_free(ctx);
}

ایستا uint32_t cf_h2_initial_win_size(ساختار Curl_easy *data)
{
#اگر NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
  /* If the transfer has a rate-limit lower than the default initial
   * stream window size, use that. It needs to be at least 8k or servers
   * may be unhappy. */
  curl_off_t rps = Curl_rlimit_per_step(&data->progress.dl.rlimit);
  اگر((rps > 0) && (rps < H2_STREAM_WINDOW_SIZE_INITIAL))
    بازگشت CURLMAX((uint32_t)rps, 8192);
#endif
  بازگشت H2_STREAM_WINDOW_SIZE_INITIAL;
}

ایستا size_t populate_settings(nghttp2_settings_entry *iv,
                                ساختار Curl_easy *data,
                                ساختار cf_h2_ctx *ctx)
{
  iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
  iv[0].مقدار = Curl_multi_max_concurrent_streams(data->multi);

  iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
  iv[1].مقدار = cf_h2_initial_win_size(data);
  اگر(ctx)
    ctx->initial_win_size = iv[1].مقدار;
  iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_افزودن;
  iv[2].مقدار = !!data->multi->افزودن_cb;

  بازگشت 3;
}

ایستا ssize_t populate_binsettings(uint8_t *binsettings,
                                    ساختار Curl_easy *data)
{
  nghttp2_settings_entry iv[H2_SETTINGS_IV_LEN];
  size_t ivlen;

  ivlen = populate_settings(iv, data, پوچ);
  /* this returns number of bytes it wrote or a negative number on error. */
  بازگشت nghttp2_pack_settings_payload(binsettings, H2_BINSETTINGS_LEN,
                                       iv, ivlen);
}

ایستا CURLcode cf_h2_update_settings(ساختار cf_h2_ctx *ctx,
                                      uint32_t initial_win_size)
{
  nghttp2_settings_entry entry;
  entry.settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
  entry.مقدار = initial_win_size;
  اگر(Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, &entry, 1))
    بازگشت CURLE_SEND_ERROR;
  ctx->initial_win_size = initial_win_size;
  بازگشت CURLE_OK;
}

#define H2_STREAM_CTX(ctx, data)                                         \
  ((ساختار h2_stream_ctx *)(                                             \
    (data) ? Curl_u32_ptrset_get(&(ctx)->streams, (data)->mid) : پوچ))

ایستا ساختار h2_stream_ctx *h2_stream_ctx_create(ساختار cf_h2_ctx *ctx,
                                                  ساختار Curl_easy *data)
{
  ساختار h2_stream_ctx *stream;

  stream = curlx_calloc(1, اندازه(*stream));
  اگر(!stream)
    بازگشت پوچ;

  stream->id = -1;
  stream->xfer_id = data->id;
  stream->mid = data->mid;
  Curl_bufq_initp(&stream->sendbuf, &ctx->stream_bufcp,
                  H2_STREAM_SEND_CHUNKS, BUFQ_OPT_NONE);
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_سطر_LEN);
  Curl_dynhds_init(&stream->resp_trailers,
                   MAX_HTTP_RESP_HEADER_شمار, DYN_HTTP_REQUEST);
  stream->bodystarted = FALSE;
  stream->status_code = -1;
  stream->closed = FALSE;
  stream->close_handled = FALSE;
  stream->error = NGHTTP2_NO_ERROR;
  stream->local_window_size = H2_STREAM_WINDOW_SIZE_INITIAL;
  stream->nrcvd_data = 0;
  بازگشت stream;
}

ایستا ساختار Curl_easy *cf_h2_get_stream_xfer(ساختار Curl_cfilter *cf,
                                               int32_t stream_id,
                                               ساختار h2_stream_ctx **pstream)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;
  ساختار h2_stream_ctx *stream =
    Curl_ngh2_session_get_stream_user_data(&ctx->ngh2, stream_id);
  اگر(pstream)
    *pstream = stream;
  اگر(stream && (stream->mid != UINT32_MAX)) {
    ساختار Curl_easy *data, *call_data = CF_DATA_CURRENT(cf);
    اگر(!call_data || !call_data->multi) {
      DEBUGASSERT(0);
      بازگشت پوچ;
    }
    data = Curl_multi_get_easy(call_data->multi, stream->mid);
    اگر(data && (data->id == stream->xfer_id))
      بازگشت data;
  }
  بازگشت پوچ;
}

#ifdef NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
ایستا int32_t cf_h2_get_desired_local_win(ساختار Curl_cfilter *cf,
                                           ساختار Curl_easy *data)
{
  curl_off_t avail = Curl_rlimit_avail(&data->progress.dl.rlimit, پوچ);

  (تهی)cf;
  اگر(avail < CURL_OFF_T_MAX) { /* limit in place */
    اگر(avail <= 0)
      بازگشت 0;
    وگرنه اگر(avail < H2_STREAM_WINDOW_SIZE_MAX)
      بازگشت (int32_t)avail;
  }
  بازگشت H2_STREAM_WINDOW_SIZE_MAX;
}

ایستا CURLcode cf_h2_update_local_win(ساختار Curl_cfilter *cf,
                                       ساختار Curl_easy *data,
                                       ساختار h2_stream_ctx *stream)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;
  int32_t dwsize;
  صحیح rv;

  dwsize = (stream->write_paused || stream->xfer_result) ?
           0 : cf_h2_get_desired_local_win(cf, data);
  اگر(dwsize != stream->local_window_size) {
    int32_t wsize = Curl_ngh2_session_get_stream_effective_local_window_size(
      &ctx->ngh2, stream->id);
    اگر(dwsize > wsize) {
      rv = Curl_ngh2_session_set_local_window_size(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize);
      اگر(rv) {
        failf(data, "[%d] nghttp2 set_local_window_size(%d) failed: "
              "%s(%d)", stream->id, dwsize, nghttp2_strerror(rv), rv);
        بازگشت CURLE_HTTP2;
      }
      rv = Curl_ngh2_submit_window_update(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize - wsize);
      اگر(rv) {
        failf(data, "[%d] nghttp2_submit_window_update() failed: "
              "%s(%d)", stream->id, nghttp2_strerror(rv), rv);
        بازگشت CURLE_HTTP2;
      }
      stream->local_window_size = dwsize;
      CURL_TRC_CF(data, cf, "[%d] local window update by %d",
                  stream->id, dwsize - wsize);
    }
    وگرنه {
      rv = Curl_ngh2_session_set_local_window_size(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize);
      اگر(rv) {
        failf(data, "[%d] nghttp2_session_set_local_window_size() failed: "
              "%s(%d)", stream->id, nghttp2_strerror(rv), rv);
        بازگشت CURLE_HTTP2;
      }
      stream->local_window_size = dwsize;
      CURL_TRC_CF(data, cf, "[%d] local window size now %d",
                  stream->id, dwsize);
    }
  }
  بازگشت CURLE_OK;
}

#وگرنه /* NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

ایستا CURLcode cf_h2_update_local_win(ساختار Curl_cfilter *cf,
                                       ساختار Curl_easy *data,
                                       ساختار h2_stream_ctx *stream)
{
  (تهی)cf;
  (تهی)data;
  (تهی)stream;
  بازگشت CURLE_OK;
}
#endif /* !NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

ایستا CURLcode http2_data_setup(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 ساختار h2_stream_ctx **pstream)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;
  ساختار h2_stream_ctx *stream;

  (تهی)cf;
  DEBUGASSERT(data);

  اگر(!data)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  stream = H2_STREAM_CTX(ctx, data);
  اگر(stream) {
    *pstream = stream;
    بازگشت CURLE_OK;
  }

  stream = h2_stream_ctx_create(ctx, data);
  اگر(!stream)
    بازگشت CURLE_OUT_OF_MEMORY;

  اگر(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    h2_stream_ctx_free(stream);
    بازگشت CURLE_OUT_OF_MEMORY;
  }

  *pstream = stream;
  بازگشت CURLE_OK;
}

ایستا CURLcode nw_out_flush(ساختار Curl_cfilter *cf,
                             ساختار Curl_easy *data)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  اگر(Curl_bufq_is_empty(&ctx->outbufq))
    بازگشت CURLE_OK;

  result = Curl_cf_send_bufq(cf->next, data, &ctx->outbufq, پوچ, 0,
                             &nwritten);
  اگر(result) {
    اگر(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_out_blocked = 1;
    }
    بازگشت result;
  }
  بازگشت Curl_bufq_is_empty(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

ایستا تهی http2_data_done(ساختار Curl_cfilter *cf, ساختار Curl_easy *data)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;
  ساختار h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);

  DEBUGASSERT(ctx);
  اگر(!stream || !ctx->initialized)
    بازگشت;

  اگر(Curl_ngh2_valid(&ctx->ngh2)) {
    bool flush_egress = FALSE;
    /* returns error if stream not known, which is fine here */
    (تهی)Curl_ngh2_session_set_stream_user_data(&ctx->ngh2, stream->id, پوچ);

    اگر(!stream->closed && stream->id > 0) {
      /* RST_STREAM */
      CURL_TRC_CF(data, cf, "[%d] premature DATA_DONE, RST stream",
                  stream->id);
      stream->closed = TRUE;
      stream->reset = TRUE;
      Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                  stream->id, NGHTTP2_STREAM_CLOSED);
      flush_egress = TRUE;
    }

    اگر(flush_egress) {
      (تهی)Curl_ngh2_session_send(&ctx->ngh2);
      (تهی)nw_out_flush(cf, data);
    }
  }

  Curl_u32_ptrset_unset(&ctx->streams, data->mid);
}

ایستا صحیح h2_client_new(ساختار Curl_cfilter *cf,
                         nghttp2_session_callbacks *cbs)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { پوچ, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  صحیح rc = nghttp2_option_new(&o);
  اگر(rc)
    بازگشت rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_set_no_auto_window_update(o, 1);
#اگر NGHTTP2_VERSION_رقم >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_set_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#endif
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  بازگشت rc;
}

/*
 * Returns nonzero if current HTTP/2 session should be closed.
 */
ایستا صحیح should_close_session(ساختار cf_h2_ctx *ctx)
{
  بازگشت ctx->drain_total == 0 && !Curl_ngh2_session_want_read(&ctx->ngh2) &&
    !Curl_ngh2_session_want_write(&ctx->ngh2);
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
ایستا CURLcode h2_process_pending_درون_داد(ساختار Curl_cfilter *cf,
                                         ساختار Curl_easy *data)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;
  ثابت unsigned نویسه *buf;
  size_t blen, nread;
  ssize_t rv;

  وقتی(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {
    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ثابت uint8_t *)buf, blen);
    اگر(!curlx_sztouz(rv, &nread)) {
      failf(data, "nghttp2 recv error %zd: %s", rv, nghttp2_strerror((صحیح)rv));
      بازگشت CURLE_RECV_ERROR;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    اگر(Curl_bufq_is_empty(&ctx->inbufq)) {
      بشکن;
    }
    وگرنه {
      CURL_TRC_CF(data, cf, "process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }

  اگر(Curl_ngh2_session_check_request_allowed(&ctx->ngh2) == 0) {
    /* No more requests are allowed in the current session, so
       the connection may not be reused. This is set when a
       GOAWAY frame has been received or when the limit of stream
       identifiers has been reached. */
    CURL_TRC_M(data, "http/2: No new requests allowed");
    connclose(cf->conn);
  }

  بازگشت CURLE_OK;
}

/*
 * The server may send us data at any point (e.g. PING frames). Therefore, we
 * cannot assume that an HTTP/2 socket is dead because it is readable.
 *
 * Check the lower filters first and, if successful, peek at the socket
 * and distinguish between closed and data.
 */
ایستا bool http2_connisalive(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                              bool *درون_داد_pending)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *درون_داد_pending = FALSE;
  اگر(!cf->next || !cf->next->cft->is_alive(cf->next, data, درون_داد_pending))
    بازگشت FALSE;

  اگر(*درون_داد_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode result;
    size_t nread;

    *درون_داد_pending = FALSE;
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(data, cf, "connisalive, recv pending input -> %d, %zu",
                (صحیح)result, nread);
    اگر(!result) {
      result = h2_process_pending_درون_داد(cf, data);
      اگر(result)
        /* immediate error, considered dead */
        alive = FALSE;
      وگرنه {
        alive = !should_close_session(ctx);
      }
    }
    وگرنه اگر(result != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  بازگشت alive;
}

ایستا CURLcode http2_send_ping(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;
  صحیح rc;

  rc = Curl_ngh2_submit_ping(&ctx->ngh2, 0, ZERO_NULL);
  اگر(rc) {
    failf(data, "nghttp2_submit_ping() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    بازگشت CURLE_HTTP2;
  }

  rc = Curl_ngh2_session_send(&ctx->ngh2);
  اگر(rc) {
    failf(data, "nghttp2_session_send() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    بازگشت CURLE_SEND_ERROR;
  }
  بازگشت CURLE_OK;
}

/*
 * Store nghttp2 version info in this buffer.
 */
تهی Curl_http2_ver(نویسه *p, size_t len)
{
  nghttp2_info *h2 = nghttp2_version(0);
  (تهی)curl_msnprintf(p, len, "nghttp2/%s", h2->version_str);
}

/*
 * The implementation of nghttp2_send_callback type. Here we write |data| with
 * size |length| to the network and return the number of bytes actually
 * written. See the documentation of nghttp2_send_callback for the details.
 */
ایستا ssize_t send_callback(nghttp2_session *h2,
                             ثابت uint8_t *buf, size_t blen, صحیح flags,
                             تهی *userp)
{
  ساختار Curl_cfilter *cf = userp;
  ساختار cf_h2_ctx *ctx = cf->ctx;
  ساختار Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result = CURLE_OK;

  (تهی)h2;
  (تهی)flags;
  DEBUGASSERT(data);

  اگر(!cf->connected)
    result = Curl_bufq_write(&ctx->outbufq, buf, blen, &nwritten);
  وگرنه
    result = Curl_cf_send_bufq(cf->next, data, &ctx->outbufq, buf, blen,
                               &nwritten);

  اگر(result) {
    اگر(result == CURLE_AGAIN) {
      ctx->nw_out_blocked = 1;
      بازگشت NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(data, "Failed sending HTTP2 data");
    بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  اگر(!nwritten) {
    ctx->nw_out_blocked = 1;
    بازگشت NGHTTP2_ERR_WOULDBLOCK;
  }
  بازگشت (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

/* We pass a pointer to this struct in the push callback, but the contents of
   the struct are hidden from the user. */
ساختار curl_pushheaders {
  ساختار Curl_easy *data;
  ساختار h2_stream_ctx *stream;
  ثابت nghttp2_افزودن_promise *frame;
};

/*
 * push header access function. Only to be used from within the push callback
 */
نویسه *curl_pushheader_bynum(ساختار curl_pushheaders *h, size_t رقم)
{
  /* Verify that we got a good easy handle in the push header struct, mostly to
     detect rubbish input fast(er). */
  اگر(!h || !GOOD_EASY_HANDLE(h->data))
    بازگشت پوچ;
  وگرنه {
    اگر(h->stream && رقم < h->stream->افزودن_headers_used)
      بازگشت h->stream->افزودن_headers[رقم];
  }
  بازگشت پوچ;
}

/*
 * push header access function. Only to be used from within the push callback
 */
نویسه *curl_pushheader_byname(ساختار curl_pushheaders *h, ثابت نویسه *name)
{
  ساختار h2_stream_ctx *stream;
  size_t len;
  size_t i;
  /* Verify that we got a good easy handle in the push header struct,
     mostly to detect rubbish input fast(er). Also empty header name
     is rubbish too. We have to allow ":" at the beginning of
     the header, but header == ":" must be rejected. If we have ':' in
     the middle of header, it could be matched in middle of the value,
     this is because we do prefix match.*/
  اگر(!h || !GOOD_EASY_HANDLE(h->data) || !name || !name[0] ||
     !strcmp(name, ":") || strchr(name + 1, ':'))
    بازگشت پوچ;

  stream = h->stream;
  اگر(!stream)
    بازگشت پوچ;

  len = strlen(name);
  برای(i = 0; i < stream->افزودن_headers_used; i++) {
    اگر(!strncmp(name, stream->افزودن_headers[i], len)) {
      /* sub-match, make sure that it is followed by a colon */
      اگر(stream->افزودن_headers[i][len] != ':')
        ادامه;
      بازگشت &stream->افزودن_headers[i][len + 1];
    }
  }
  بازگشت پوچ;
}

ایستا ساختار Curl_easy *h2_duphandle(ساختار Curl_cfilter *cf,
                                      ساختار Curl_easy *data)
{
  ساختار Curl_easy *second = curl_easy_init();
  اگر(second) {
    ساختار h2_stream_ctx *second_stream;
    http2_data_setup(cf, second, &second_stream);
    second->state.weight = data->state.weight;
    اگر(data->share)
      (تهی)Curl_share_easy_link(second, data->share);
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
  بازگشت second;
}

ایستا صحیح set_transfer_url(ساختار Curl_easy *newhandle,
                            ساختار curl_pushheaders *hp,
                            ساختار Curl_easy *data)
{
  ثابت نویسه *v;
  CURLUcode uc;
  نویسه *url = پوچ;
  صحیح rc = 0;
  CURLU *u = curl_url();

  اگر(!u)
    بازگشت 5;

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_SCHEME);
  اگر(v) {
    uc = curl_url_set(u, CURLUPART_SCHEME, v, 0);
    اگر(uc) {
      rc = 1;
      برو fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_AUTHORITY);
  اگر(v) {
    uc = Curl_url_set_authority(u, v);
    اگر(uc) {
      rc = 2;
      برو fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_PATH);
  اگر(v) {
    uc = curl_url_set(u, CURLUPART_PATH, v, 0);
    اگر(uc) {
      rc = 3;
      برو fail;
    }
  }

  /* We can only allow PUSH of resource from the same origin, e.g.
   * scheme + hostname + port */
  اگر(!Curl_url_same_origin(data->state.uh, u)) {
    rc = 1;
    برو fail;
  }

  uc = curl_url_get(u, CURLUPART_URL, &url, 0);
  اگر(uc)
    rc = 4;
fail:
  اگر(rc) {
    curl_url_cleanup(u);
    بازگشت rc;
  }

  DEBUGASSERT(!newhandle->state.uh);
  newhandle->state.uh = u;
  Curl_bufref_set(&newhandle->state.url, url, 0, curl_free);
  بازگشت 0;
}

ایستا تهی discard_newhandle(ساختار Curl_cfilter *cf,
                              ساختار Curl_easy *newhandle)
{
  http2_data_done(cf, newhandle);
  (تهی)Curl_close(&newhandle);
}

ایستا صحیح افزودن_promise(ساختار Curl_cfilter *cf,
                        ساختار Curl_easy *data,
                        ثابت nghttp2_افزودن_promise *frame)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;
  صحیح rv; /* one of the CURL_PUSH_* defines */

  CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE received",
              frame->promised_stream_id);
  اگر(data->multi->افزودن_cb) {
    ساختار h2_stream_ctx *stream;
    ساختار h2_stream_ctx *newstream;
    ساختار curl_pushheaders heads;
    CURLMcode mresult;
    CURLcode result;
    /* clone the parent */
    ساختار Curl_easy *newhandle = h2_duphandle(cf, data);
    اگر(!newhandle) {
      infof(data, "failed to duplicate handle");
      rv = CURL_افزودن_DENY; /* FAIL HARD */
      برو fail;
    }

    stream = H2_STREAM_CTX(ctx, data);
    اگر(!stream) {
      failf(data, "Internal NULL stream");
      discard_newhandle(cf, newhandle);
      rv = CURL_افزودن_DENY;
      برو fail;
    }

    heads.data = data;
    heads.stream = stream;
    heads.frame = frame;

    rv = set_transfer_url(newhandle, &heads, data);
    اگر(rv) {
      CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE, failed to set URL -> %d",
                  frame->promised_stream_id, rv);
      discard_newhandle(cf, newhandle);
      rv = CURL_افزودن_DENY;
      برو fail;
    }

    {
      ساختار Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, multi_افزودن_cb);
      rv = data->multi->افزودن_cb(data, newhandle,
                                stream->افزودن_headers_used, &heads,
                                data->multi->افزودن_userp);
      CURL_CBAPI_END(&guard);
    }

    /* free the headers again */
    free_افزودن_headers(stream);

    اگر(rv) {
      DEBUGASSERT((rv > CURL_افزودن_OK) && (rv <= CURL_افزودن_ERROROUT));
      /* denied, kill off the new handle again */
      CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE, denied by application -> %d",
                  frame->promised_stream_id, rv);
      discard_newhandle(cf, newhandle);
      برو fail;
    }

    /* approved, add to the multi handle for processing. This
     * assigns newhandle->mid. For the new `mid` we assign the
     * h2_stream instance and remember the stream_id already known. */
    mresult = Curl_multi_add_perform(data->multi, newhandle, cf->conn);
    اگر(mresult) {
      infof(data, "failed to add handle to multi");
      discard_newhandle(cf, newhandle);
      rv = CURL_افزودن_DENY;
      برو fail;
    }

    result = http2_data_setup(cf, newhandle, &newstream);
    اگر(result) {
      failf(data, "error setting up stream: %d", (صحیح)result);
      discard_newhandle(cf, newhandle);
      rv = CURL_افزودن_DENY;
      برو fail;
    }

    DEBUGASSERT(newstream);
    newstream->id = frame->promised_stream_id;
    newhandle->req.maxdownload = -1;
    newhandle->req.size = -1;

    CURL_TRC_CF(data, cf, "promise easy handle added to multi, mid=%u",
                newhandle->mid);
    rv = Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, newstream->id, newstream);
    اگر(rv) {
      infof(data, "failed to set user_data for stream %d",
            newstream->id);
      DEBUGASSERT(0);
      discard_newhandle(cf, newhandle);
      rv = CURL_افزودن_DENY;
      برو fail;
    }

    /* success, remember max stream id processed */
    اگر(newstream->id > ctx->local_max_sid)
      ctx->local_max_sid = newstream->id;
  }
  وگرنه {
    CURL_TRC_CF(data, cf, "Got PUSH_PROMISE, ignore it");
    rv = CURL_افزودن_DENY;
  }
fail:
  بازگشت rv;
}

ایستا تهی h2_xfer_write_resp_hd(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data,
                                  ساختار h2_stream_ctx *stream,
                                  ثابت نویسه *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  اگر(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, blen, eos);
    اگر(!stream->xfer_result && !eos)
      stream->xfer_result = cf_h2_update_local_win(cf, data, stream);
    اگر(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%d] error %d writing %zu bytes of headers",
                  stream->id, (صحیح)stream->xfer_result, blen);
  }
}

ایستا تهی h2_xfer_write_resp(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data,
                               ساختار h2_stream_ctx *stream,
                               ثابت نویسه *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  اگر(!stream->xfer_result)
    stream->xfer_result = Curl_xfer_write_resp(data, buf, blen, eos);
  /* If the transfer write is errored, we do not want any more data */
  اگر(stream->xfer_result) {
    ساختار cf_h2_ctx *ctx = cf->ctx;
    CURL_TRC_CF(data, cf, "[%d] error %d writing %zu bytes of data, "
                "RST-ing stream",
                stream->id, (صحیح)stream->xfer_result, blen);
    Curl_ngh2_submit_rst_stream(
      &ctx->ngh2, 0, stream->id, (uint32_t)NGHTTP2_ERR_CALLBACK_FAILURE);
  }
  وگرنه اگر(!stream->write_paused && Curl_xfer_write_is_paused(data)) {
    CURL_TRC_CF(data, cf, "[%d] stream output paused", stream->id);
    stream->write_paused = TRUE;
  }
  وگرنه اگر(stream->write_paused && !Curl_xfer_write_is_paused(data)) {
    CURL_TRC_CF(data, cf, "[%d] stream output unpaused", stream->id);
    stream->write_paused = FALSE;
  }

  اگر(!stream->xfer_result && !eos)
    stream->xfer_result = cf_h2_update_local_win(cf, data, stream);
}

ایستا CURLcode on_stream_frame(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data,
                                ساختار h2_stream_ctx *stream,
                                ثابت nghttp2_frame *frame)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;
  int32_t stream_id = frame->hd.stream_id;
  صحیح rv;

  اگر(!stream) {
    CURL_TRC_CF(data, cf, "[%d] No stream_ctx set", stream_id);
    بازگشت CURLE_FAILED_INIT;
  }

  انتخاب(frame->hd.type) {
  حالت NGHTTP2_DATA:
    CURL_TRC_CF(data, cf, "[%d] DATA, window=%d/%d",
                stream_id,
                Curl_ngh2_session_get_stream_effective_recv_data_length(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_stream_effective_local_window_size(
                  &ctx->ngh2, stream->id));
    /* If !body started on this stream, then receiving DATA is illegal. */
    اگر(!stream->bodystarted) {
      اگر(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                       stream_id, NGHTTP2_PROTOCOL_ERROR))) {
        بازگشت CURLE_RECV_ERROR;
      }
    }
    بشکن;
  حالت NGHTTP2_HEADERS:
    اگر(stream->bodystarted) {
      /* Only valid HEADERS after body started is trailer HEADERS. We
         buffer them in on_header callback. */
      بشکن;
    }

    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    اگر(stream->status_code == -1)
      بازگشت CURLE_RECV_ERROR;

    /* Only final status code signals the end of header */
    اگر(stream->status_code / 100 != 1) {
      stream->resp_hds_complete = TRUE;
      stream->bodystarted = TRUE;
    }
    وگرنه
      stream->status_code = -1;

    h2_xfer_write_resp_hd(cf, data, stream, STRCONST("\r\n"),
                          (bool)stream->closed);

    Curl_multi_mark_dirty(data);
    بشکن;
  حالت NGHTTP2_افزودن_PROMISE:
    rv = افزودن_promise(cf, data, &frame->افزودن_promise);
    اگر(rv) { /* deny! */
      DEBUGASSERT((rv > CURL_افزودن_OK) && (rv <= CURL_افزودن_ERROROUT));
      اگر(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
             frame->افزودن_promise.promised_stream_id, NGHTTP2_CANCEL)))
        بازگشت CURLE_SEND_ERROR;
      وگرنه اگر(rv == CURL_افزودن_ERROROUT) {
        /* fail the parent transfer, not the whole session */
        CURL_TRC_CF(data, cf, "[%d] fail in PUSH_PROMISE received",
                    stream_id);
        اگر(!stream->xfer_result)
          stream->xfer_result = CURLE_RECV_ERROR;
        Curl_multi_mark_dirty(data);
      }
    }
    بشکن;
  حالت NGHTTP2_RST_STREAM:
    اگر(frame->rst_stream.error_code)
      stream->reset_by_server = TRUE;
    Curl_multi_mark_dirty(data);
    بشکن;
  حالت NGHTTP2_WINDOW_UPDATE:
    اگر(CURL_REQ_WANT_SEND(data) && Curl_bufq_is_empty(&stream->sendbuf)) {
      /* need more data, force processing of transfer */
      Curl_multi_mark_dirty(data);
    }
    وگرنه اگر(!Curl_bufq_is_empty(&stream->sendbuf)) {
      /* resume the potentially suspended stream */
      اگر(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id)))
        بازگشت CURLE_SEND_ERROR;
    }
    بشکن;
  پیشفرض:
    بشکن;
  }

  اگر(frame->hd.flags & NGHTTP2_FLAG_END_STREAM) {
    اگر(!stream->closed && !stream->body_eos &&
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
  بازگشت CURLE_OK;
}

#ifdef CURLVERBOSE
صحیح Curl_nghttp2_fr_print(ثابت nghttp2_frame *frame, نویسه *buffer,
                          size_t blen)
{
  انتخاب(frame->hd.type) {
  حالت NGHTTP2_DATA: {
    بازگشت curl_msnprintf(buffer, blen,
                          "FRAME[DATA, len=%d, eos=%d, padlen=%d]",
                          (صحیح)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_STREAM),
                          (صحیح)frame->data.padlen);
  }
  حالت NGHTTP2_HEADERS: {
    بازگشت curl_msnprintf(buffer, blen,
                          "FRAME[HEADERS, len=%d, hend=%d, eos=%d]",
                          (صحیح)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_HEADERS),
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_STREAM));
  }
  حالت NGHTTP2_PRIORITY: {
    بازگشت curl_msnprintf(buffer, blen,
                          "FRAME[PRIORITY, len=%d, flags=%d]",
                          (صحیح)frame->hd.length, frame->hd.flags);
  }
  حالت NGHTTP2_RST_STREAM: {
    بازگشت curl_msnprintf(buffer, blen,
                          "FRAME[RST_STREAM, len=%d, flags=%d, error=%u]",
                          (صحیح)frame->hd.length, frame->hd.flags,
                          frame->rst_stream.error_code);
  }
  حالت NGHTTP2_SETTINGS: {
    اگر(frame->hd.flags & NGHTTP2_FLAG_ACK) {
      بازگشت curl_msnprintf(buffer, blen, "FRAME[SETTINGS, ack=1]");
    }
    بازگشت curl_msnprintf(buffer, blen,
                          "FRAME[SETTINGS, len=%d]", (صحیح)frame->hd.length);
  }
  حالت NGHTTP2_افزودن_PROMISE:
    بازگشت curl_msnprintf(buffer, blen,
                          "FRAME[PUSH_PROMISE, len=%d, hend=%d]",
                          (صحیح)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_HEADERS));
  حالت NGHTTP2_PING:
    بازگشت curl_msnprintf(buffer, blen,
                          "FRAME[PING, len=%d, ack=%d]",
                          (صحیح)frame->hd.length,
                          frame->hd.flags & NGHTTP2_FLAG_ACK);
  حالت NGHTTP2_GOAWAY: {
    نویسه scratch[128];
    size_t s_len = CURL_ARRAYSIZE(scratch);
    size_t len = (frame->goaway.opaque_data_len < s_len) ?
      frame->goaway.opaque_data_len : s_len - 1;
    اگر(len)
      memcpy(scratch, frame->goaway.opaque_data, len);
    scratch[len] = '\0';
    بازگشت curl_msnprintf(buffer, blen,
                          "FRAME[GOAWAY, error=%u, reason='%s', "
                          "last_stream=%d]", frame->goaway.error_code,
                          scratch, frame->goaway.last_stream_id);
  }
  حالت NGHTTP2_WINDOW_UPDATE: {
    بازگشت curl_msnprintf(buffer, blen,
                          "FRAME[WINDOW_UPDATE, incr=%d]",
                          frame->window_update.window_size_increment);
  }
  پیشفرض:
    بازگشت curl_msnprintf(buffer, blen, "FRAME[%d, len=%d, flags=%d]",
                          frame->hd.type, (صحیح)frame->hd.length,
                          frame->hd.flags);
  }
}

ایستا صحیح on_frame_send(nghttp2_session *session, ثابت nghttp2_frame *frame,
                         تهی *userp)
{
  ساختار Curl_cfilter *cf = userp;
  ساختار cf_h2_ctx *ctx = cf->ctx;
  ساختار Curl_easy *data = CF_DATA_CURRENT(cf);

  (تهی)session;
  DEBUGASSERT(data);
  اگر(Curl_trc_cf_is_verbose(cf, data)) {
    نویسه buffer[256];
    صحیح len;
    len = Curl_nghttp2_fr_print(frame, buffer, اندازه(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] -> %s", frame->hd.stream_id, buffer);
  }
  اگر((frame->hd.type == NGHTTP2_GOAWAY) && !ctx->sent_goaway) {
    /* A GOAWAY not initiated by us, but by nghttp2 itself on detecting
     * a protocol error on the connection */
    failf(data, "nghttp2 shuts down connection with error %u: %s",
          frame->goaway.error_code,
          nghttp2_http2_strerror(frame->goaway.error_code));
  }
  بازگشت 0;
}
#endif /* CURLVERBOSE */

ایستا صحیح on_frame_recv(nghttp2_session *session, ثابت nghttp2_frame *frame,
                         تهی *userp)
{
  ساختار Curl_cfilter *cf = userp;
  ساختار cf_h2_ctx *ctx = cf->ctx;
  ساختار Curl_easy *data = CF_DATA_CURRENT(cf), *data_s;
  ساختار h2_stream_ctx *stream;
  int32_t stream_id = frame->hd.stream_id;

  (تهی)session;
  DEBUGASSERT(data);
#ifdef CURLVERBOSE
  اگر(Curl_trc_cf_is_verbose(cf, data)) {
    نویسه buffer[256];
    صحیح len;
    len = Curl_nghttp2_fr_print(frame, buffer, اندازه(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] <- %s", frame->hd.stream_id, buffer);
  }
#endif /* CURLVERBOSE */

  اگر(!stream_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(data);
    انتخاب(frame->hd.type) {
    حالت NGHTTP2_SETTINGS: {
      اگر(!(frame->hd.flags & NGHTTP2_FLAG_ACK)) {
        uint32_t max_conn = ctx->max_concurrent_streams;
        ctx->max_concurrent_streams =
          Curl_ngh2_session_get_remote_settings(
            &ctx->ngh2, NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS);
        ctx->enable_افزودن = Curl_ngh2_session_get_remote_settings(
          &ctx->ngh2, NGHTTP2_SETTINGS_ENABLE_افزودن) != 0;
        CURL_TRC_CF(data, cf, "[0] MAX_CONCURRENT_STREAMS: %u",
                    ctx->max_concurrent_streams);
        CURL_TRC_CF(data, cf, "[0] ENABLE_PUSH: %s",
                    ctx->enable_افزودن ? "TRUE" : "false");
        اگر(data && max_conn != ctx->max_concurrent_streams) {
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
        اگر(CURL_REQ_WANT_SEND(data))
          Curl_multi_mark_dirty(data);
      }
      بشکن;
    }
    حالت NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->goaway_error = frame->goaway.error_code;
      ctx->remote_max_sid = frame->goaway.last_stream_id;
      اگر(data) {
        infof(data, "received GOAWAY, error=%u, last_stream=%d",
              ctx->goaway_error, ctx->remote_max_sid);
        Curl_multi_connchanged(data->multi);
      }
      بشکن;
    پیشفرض:
      بشکن;
    }
    بازگشت 0;
  }

  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  اگر(!data_s) {
    CURL_TRC_CF(data, cf, "[%d] No Curl_easy associated", stream_id);
    بازگشت 0;
  }

  بازگشت on_stream_frame(cf, data_s, stream, frame) ?
         NGHTTP2_ERR_CALLBACK_FAILURE : 0;
}

ایستا صحیح cf_h2_on_invalid_frame_recv(nghttp2_session *session,
                                       ثابت nghttp2_frame *frame,
                                       صحیح ngerr, تهی *userp)
{
  ساختار Curl_cfilter *cf = userp;
  ساختار cf_h2_ctx *ctx = cf->ctx;
  ساختار Curl_easy *data;
  ساختار h2_stream_ctx *stream;
  int32_t stream_id = frame->hd.stream_id;

  (تهی)session;
  data = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  اگر(data) {
#ifdef CURLVERBOSE
    نویسه buffer[256];
    صحیح len;
    len = Curl_nghttp2_fr_print(frame, buffer, اندازه(buffer) - 1);
    buffer[len] = 0;
    failf(data, "[HTTP2] [%d] received invalid frame: %s, error %d: %s",
          stream_id, buffer, ngerr, nghttp2_strerror(ngerr));
#endif /* CURLVERBOSE */
    اگر(stream) {
      Curl_ngh2_submit_rst_stream(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, NGHTTP2_STREAM_CLOSED);
      stream->error = ngerr;
      stream->closed = TRUE;
      stream->reset = TRUE;
    }
  }
  /* Return no error, nghttp2 will RST/GOAWAY by itself when needed */
  بازگشت 0;
}

ایستا صحیح on_data_chunk_recv(nghttp2_session *session, uint8_t flags,
                              int32_t stream_id,
                              ثابت uint8_t *mem, size_t len, تهی *userp)
{
  ساختار Curl_cfilter *cf = userp;
  ساختار cf_h2_ctx *ctx = cf->ctx;
  ساختار h2_stream_ctx *stream;
  ساختار Curl_easy *data_s;

  (تهی)session;
  (تهی)flags;

  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */
  DEBUGASSERT(CF_DATA_CURRENT(cf));

  /* get the stream from the hash based on Stream ID */
  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  اگر(!data_s) {
    /* Receiving a Stream ID not in the hash should not happen - unless
       we have aborted a transfer artificially and there were more data
       in the pipeline. Silently ignore. */
    CURL_TRC_CF(CF_DATA_CURRENT(cf), cf, "[%d] Data for unknown", stream_id);
    /* consumed explicitly as no one will read it */
    Curl_ngh2_session_consume(&ctx->ngh2, stream_id, len);
    بازگشت 0;
  }

  اگر(!stream)
    بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;

  h2_xfer_write_resp(cf, data_s, stream, (ثابت نویسه *)mem, len, FALSE);

  Curl_ngh2_session_consume(&ctx->ngh2, stream_id, len);
  stream->nrcvd_data += (curl_off_t)len;
  بازگشت 0;
}

ایستا صحیح on_stream_close(nghttp2_session *session, int32_t stream_id,
                           uint32_t error_code, تهی *userp)
{
  ساختار Curl_cfilter *cf = userp;
  ساختار cf_h2_ctx *ctx = cf->ctx;
  ساختار Curl_easy *data_s, *call_data = CF_DATA_CURRENT(cf);
  ساختار h2_stream_ctx *stream = پوچ;
  صحیح rv;
  (تهی)session;

  DEBUGASSERT(call_data);
  /* stream id 0 is the connection, do not look there for streams. */
  data_s = stream_id ? cf_h2_get_stream_xfer(cf, stream_id, &stream) : پوچ;
  اگر(!data_s) {
    CURL_TRC_CF(call_data, cf,
                "[%d] on_stream_close, no easy set on stream", stream_id);
    بازگشت 0;
  }
  اگر(!GOOD_EASY_HANDLE(data_s)) {
    /* nghttp2 still has an easy registered for the stream which has
     * been freed be libcurl. This points to a code path that does not
     * trigger DONE or DETACH events as it must. */
    CURL_TRC_CF(call_data, cf,
                "[%d] on_stream_close, not a GOOD easy on stream", stream_id);
    (تهی)Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, stream_id, پوچ);
    بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  اگر(!stream) {
    CURL_TRC_CF(data_s, cf,
                "[%d] on_stream_close, GOOD easy but no stream", stream_id);
    بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  stream->closed = TRUE;
  stream->error = error_code;
  اگر(stream->error) {
    stream->reset = TRUE;
    اگر((stream->error == NGHTTP2_REFUSED_STREAM) &&
       ctx->rcvd_goaway && ctx->goaway_error)
    /* REFUSED_STREAM is used by nghttp2 on a GOAWAY from the
     * server where it indicated it will no longer process this stream.
     * If the GOAWAY carried an error code, take that as the error for
     * the stream. */
    stream->error = ctx->goaway_error;
  }

  اگر(stream->error)
    CURL_TRC_CF(data_s, cf, "[%d] RESET: %s (err %u)",
              stream_id, nghttp2_http2_strerror(error_code), error_code);
  وگرنه
    CURL_TRC_CF(data_s, cf, "[%d] CLOSED", stream_id);
  Curl_multi_mark_dirty(data_s);

  /* remove `stream_ctx` from the nghttp2 stream */
  rv = Curl_ngh2_session_set_stream_user_data(
    &ctx->ngh2, stream_id, پوچ);
  اگر(rv) {
    infof(data_s, "http/2: failed to clear user_data for stream %d",
          stream_id);
    DEBUGASSERT(0);
  }
  بازگشت 0;
}

ایستا تهی cf_h2_header_error(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data,
                               ساختار h2_stream_ctx *stream,
                               CURLcode result)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;

  failf(data, "Error receiving HTTP2 header: %d(%s)", (صحیح)result,
        curl_easy_strerror(result));
  اگر(stream) {
    Curl_ngh2_submit_rst_stream(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, NGHTTP2_STREAM_CLOSED);
    stream->closed = TRUE;
    stream->reset = TRUE;
  }
}

/* frame->hd.type is either NGHTTP2_HEADERS or NGHTTP2_PUSH_PROMISE */
ایستا صحیح on_header(nghttp2_session *session, ثابت nghttp2_frame *frame,
                     ثابت uint8_t *name, size_t namelen,
                     ثابت uint8_t *مقدار, size_t valuelen,
                     uint8_t flags,
                     تهی *userp)
{
  ساختار Curl_cfilter *cf = userp;
  ساختار cf_h2_ctx *ctx = cf->ctx;
  ساختار h2_stream_ctx *stream;
  ساختار Curl_easy *data;
  int32_t stream_id = frame->hd.stream_id;
  CURLcode result;

  (تهی)session;
  (تهی)flags;

  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */

  /* get the stream from the hash based on Stream ID */
  data = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  اگر(!GOOD_EASY_HANDLE(data))
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;

  اگر(!stream) {
    failf(data, "Internal NULL stream");
    بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  /* Store received PUSH_PROMISE headers to be used when the subsequent
     PUSH_PROMISE callback comes */
  اگر(frame->hd.type == NGHTTP2_افزودن_PROMISE) {
    نویسه *h;

    اگر((namelen == CURL_CSTRLEN(HTTP_PSEUDO_AUTHORITY)) &&
       !strncmp(HTTP_PSEUDO_AUTHORITY, (ثابت نویسه *)name, namelen)) {
      /* pseudo headers are lower case */
      صحیح rc = 0;
      نویسه *check = curl_maprintf("%s:%d", data->state.origin->hostname,
                                  data->state.origin->port);
      اگر(!check)
        /* no memory */
        بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;
      اگر(!curl_strequal(check, (ثابت نویسه *)مقدار) &&
         ((data->state.origin->port != data->state.origin->scheme->defport) ||
          !curl_strequal(data->state.origin->hostname, (ثابت نویسه *)مقدار))) {
        /* This is push is not for the same authority that was asked for in
         * the URL. RFC 7540 section 8.2 says: "A client MUST treat a
         * PUSH_PROMISE for which the server is not authoritative as a stream
         * error of type PROTOCOL_ERROR."
         */
        (تهی)Curl_ngh2_submit_rst_stream(
          &ctx->ngh2, NGHTTP2_FLAG_NONE, stream_id, NGHTTP2_PROTOCOL_ERROR);
        rc = NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      curlx_free(check);
      اگر(rc)
        بازگشت rc;
    }

    اگر(!stream->افزودن_headers) {
      stream->افزودن_headers_alloc = 10;
      stream->افزودن_headers = curlx_malloc(stream->افزودن_headers_alloc *
                                          اندازه(نویسه *));
      اگر(!stream->افزودن_headers)
        بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;
      stream->افزودن_headers_used = 0;
    }
    وگرنه اگر(stream->افزودن_headers_used ==
            stream->افزودن_headers_alloc) {
      نویسه **headp;
      اگر(stream->افزودن_headers_alloc > 1000) {
        /* this is beyond crazy many headers, bail out */
        failf(data, "Too many PUSH_PROMISE headers");
        free_افزودن_headers(stream);
        بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      stream->افزودن_headers_alloc *= 2;
      headp = curlx_realloc(stream->افزودن_headers,
                            stream->افزودن_headers_alloc * اندازه(نویسه *));
      اگر(!headp) {
        free_افزودن_headers(stream);
        بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      stream->افزودن_headers = headp;
    }
    h = curl_maprintf("%s:%s", name, مقدار);
    اگر(!h) {
      free_افزودن_headers(stream);
      بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    stream->افزودن_headers[stream->افزودن_headers_used++] = h;
    بازگشت 0;
  }

  اگر(stream->bodystarted) {
    /* This is a trailer */
    CURL_TRC_CF(data, cf, "[%d] trailer: %.*s: %.*s",
                stream->id, (صحیح)namelen, name, (صحیح)valuelen, مقدار);
    result = Curl_dynhds_add(&stream->resp_trailers,
                             (ثابت نویسه *)name, namelen,
                             (ثابت نویسه *)مقدار, valuelen);
    اگر(result) {
      cf_h2_header_error(cf, data, stream, result);
      بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;
    }

    بازگشت 0;
  }

  اگر(namelen == CURL_CSTRLEN(HTTP_PSEUDO_STATUS) &&
     !memcmp(HTTP_PSEUDO_STATUS, name, namelen)) {
    /* nghttp2 guarantees :status is received first and only once. */
    نویسه buffer[32];
    size_t hlen;
    result = Curl_http_decode_status(&stream->status_code,
                                     (ثابت نویسه *)مقدار, valuelen);
    اگر(result) {
      cf_h2_header_error(cf, data, stream, result);
      بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    hlen = curl_msnprintf(buffer, اندازه(buffer), HTTP_PSEUDO_STATUS ":%d\r",
                          stream->status_code);
    result = Curl_headers_افزودن(data, buffer, hlen, CURLH_PSEUDO);
    اگر(result) {
      cf_h2_header_error(cf, data, stream, result);
      بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    curlx_dyn_reset(&ctx->scratch);
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/2 "));
    اگر(!result)
      result = curlx_dyn_addn(&ctx->scratch, مقدار, valuelen);
    اگر(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    اگر(!result)
      h2_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    اگر(result) {
      cf_h2_header_error(cf, data, stream, result);
      بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    /* if we receive data for another handle, wake that up */
    اگر(CF_DATA_CURRENT(cf) != data)
      Curl_multi_mark_dirty(data);

    CURL_TRC_CF(data, cf, "[%d] status: HTTP/2 %03d",
                stream->id, stream->status_code);
    بازگشت 0;
  }

  /* nghttp2 guarantees that namelen > 0, and :status was already
     received, and this is not pseudo-header field . */
  /* convert to an HTTP1-style header */
  curlx_dyn_reset(&ctx->scratch);
  result = curlx_dyn_addn(&ctx->scratch, (ثابت نویسه *)name, namelen);
  اگر(!result)
    result = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
  اگر(!result)
    result = curlx_dyn_addn(&ctx->scratch, (ثابت نویسه *)مقدار, valuelen);
  اگر(!result)
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
  اگر(!result)
    h2_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                          curlx_dyn_len(&ctx->scratch), FALSE);
  اگر(result) {
    cf_h2_header_error(cf, data, stream, result);
    بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  /* if we receive data for another handle, wake that up */
  اگر(CF_DATA_CURRENT(cf) != data)
    Curl_multi_mark_dirty(data);

  CURL_TRC_CF(data, cf, "[%d] header: %.*s: %.*s",
              stream->id, (صحیح)namelen, name, (صحیح)valuelen, مقدار);

  بازگشت 0; /* 0 is successful */
}

ایستا ssize_t req_body_read_callback(nghttp2_session *session,
                                      int32_t stream_id,
                                      uint8_t *buf, size_t length,
                                      uint32_t *data_flags,
                                      nghttp2_data_source *source,
                                      تهی *userp)
{
  ساختار Curl_cfilter *cf = userp;
  ساختار Curl_easy *data_s;
  ساختار h2_stream_ctx *stream = پوچ;
  CURLcode result;
  ssize_t nread;
  size_t n;
  (تهی)source;

  (تهی)session;
  اگر(!stream_id)
    بازگشت NGHTTP2_ERR_INVALID_ARGUMENT;

  /* get the stream from the hash based on Stream ID, stream ID zero is for
     connection-oriented stuff */
  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  اگر(!data_s || !stream)
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_bufq_read(&stream->sendbuf, buf, length, &n);
  اگر(result) {
    اگر(result != CURLE_AGAIN)
      بازگشت NGHTTP2_ERR_CALLBACK_FAILURE;
    nread = 0;
  }
  وگرنه
    nread = (ssize_t)n;

  CURL_TRC_CF(data_s, cf, "[%d] req_body_read(len=%zu) eos=%d -> %zd, %d",
              stream_id, length, stream->body_eos, nread, (صحیح)result);

  اگر(stream->body_eos && Curl_bufq_is_empty(&stream->sendbuf)) {
    *data_flags = NGHTTP2_DATA_FLAG_EOF;
    بازگشت nread;
  }
  بازگشت (nread == 0) ? NGHTTP2_ERR_DEFERRED : nread;
}

#ifdef CURLVERBOSE
ایستا صحیح error_callback(nghttp2_session *session,
                          ثابت نویسه *پیام,
                          size_t len,
                          تهی *userp)
{
  ساختار Curl_cfilter *cf = userp;
  ساختار Curl_easy *data = CF_DATA_CURRENT(cf);
  (تهی)session;
  failf(data, "%.*s", (صحیح)len, پیام);
  بازگشت 0;
}
#endif

/*
 * Append headers to ask for an HTTP1.1 to HTTP2 upgrade.
 */
CURLcode Curl_http2_request_upgrade(ساختار dynbuf *req,
                                    ساختار Curl_easy *data)
{
  CURLcode result;
  نویسه *base64;
  size_t blen;
  ساختار SingleRequest *k = &data->req;
  uint8_t binsettings[H2_BINSETTINGS_LEN];
  ssize_t rc;
  size_t binlen; /* length of the binsettings data */

  rc = populate_binsettings(binsettings, data);
  اگر(!curlx_sztouz(rc, &binlen) || !binlen) {
    failf(data, "nghttp2 unexpectedly failed on pack_settings_payload");
    curlx_dyn_free(req);
    بازگشت CURLE_FAILED_INIT;
  }

  result = curlx_base64url_encode(binsettings, binlen, &base64, &blen);
  اگر(result) {
    curlx_dyn_free(req);
    بازگشت result;
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

  بازگشت result;
}

ایستا CURLcode http2_handle_stream_close(ساختار Curl_cfilter *cf,
                                          ساختار Curl_easy *data,
                                          ساختار h2_stream_ctx *stream,
                                          size_t *pnlen)
{
  CURLcode result;

  *pnlen = 0;
  اگر(stream->reset) {
    اگر(stream->error == NGHTTP2_REFUSED_STREAM) {
      infof(data, "HTTP/2 stream %d refused by server, try again on a new "
                  "connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      بازگشت CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    وگرنه اگر(stream->resp_hds_complete && data->req.no_body) {
      CURL_TRC_CF(data, cf, "[%d] error after response headers, but we did "
                  "not want a body anyway, ignore: %s (err %u)",
                  stream->id, nghttp2_http2_strerror(stream->error),
                  stream->error);
      stream->close_handled = TRUE;
      بازگشت CURLE_OK;
    }
    failf(data, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          stream->id, stream->reset_by_server ? "server" : "curl",
          stream->error, nghttp2_http2_strerror(stream->error));
    بازگشت stream->error ? CURLE_HTTP2_STREAM :
           (data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2);
  }
  وگرنه اگر(!stream->bodystarted) {
    failf(data, "HTTP/2 stream %d was closed cleanly, but before getting "
          "all response header fields, treated as error", stream->id);
    بازگشت CURLE_HTTP2_STREAM;
  }

  اگر(Curl_dynhds_شمار(&stream->resp_trailers)) {
    ساختار dynhds_entry *e;
    ساختار dynbuf dbuf;
    size_t i;

    result = CURLE_OK;
    curlx_dyn_init(&dbuf, DYN_TRAILERS);
    برای(i = 0; i < Curl_dynhds_شمار(&stream->resp_trailers); ++i) {
      e = Curl_dynhds_getn(&stream->resp_trailers, i);
      اگر(!e)
        بشکن;
      curlx_dyn_reset(&dbuf);
      result = curlx_dyn_addf(&dbuf, "%.*s: %.*s\x0d\x0a",
                              (صحیح)e->namelen, e->name,
                              (صحیح)e->valuelen, e->مقدار);
      اگر(result)
        بشکن;
      Curl_debug(data, CURLINFO_HEADER_IN, curlx_dyn_ptr(&dbuf),
                 curlx_dyn_len(&dbuf));
      result = Curl_client_write(data,
                                 CLIENTWRITE_HEADER | CLIENTWRITE_TRAILER,
                                 curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf));
      اگر(result)
        بشکن;
      result = Curl_bump_headersize(data, curlx_dyn_len(&dbuf), FALSE);
      اگر(result)
        بازگشت result;
    }
    curlx_dyn_free(&dbuf);
    اگر(result)
      برو out;
  }

  stream->close_handled = TRUE;
  result = CURLE_OK;

out:
  CURL_TRC_CF(data, cf, "handle_stream_close -> %d, %zu", (صحیح)result, *pnlen);
  بازگشت result;
}

ایستا صحیح sweight_wanted(ثابت ساختار Curl_easy *data)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  بازگشت data->set.weight ?
    data->set.weight : NGHTTP2_DEFAULT_WEIGHT;
}

ایستا صحیح sweight_in_effect(ثابت ساختار Curl_easy *data)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  بازگشت data->state.weight ?
    data->state.weight : NGHTTP2_DEFAULT_WEIGHT;
}

/*
 * h2_pri_spec() fills in the pri_spec struct, used by nghttp2 to send weight
 * and dependency to the peer. It also stores the updated values in the state
 * struct.
 */

ایستا تهی h2_pri_spec(ساختار Curl_easy *data,
                        nghttp2_priority_spec *pri_spec)
{
  صحیح prio = data->set.weight;
  nghttp2_priority_spec_init(pri_spec, 0, sweight_wanted(data), FALSE);
  data->state.weight = prio;
}

/*
 * Check if there is been an update in the priority /
 * dependency settings and if so it submits a PRIORITY frame with the updated
 * info.
 * Flush any out data pending in the network buffer.
 */
ایستا CURLcode h2_progress_egress(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;
  صحیح rv = 0;

  اگر(sweight_wanted(data) != sweight_in_effect(data)) {
    ساختار h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    اگر(stream && stream->id > 0 &&
       (sweight_wanted(data) != sweight_in_effect(data))) {
      /* send new weight and/or dependency */
      nghttp2_priority_spec pri_spec;

      h2_pri_spec(data, &pri_spec);
      CURL_TRC_CF(data, cf, "[%d] Queuing PRIORITY", stream->id);
      DEBUGASSERT(stream->id != -1);
      rv = Curl_ngh2_submit_priority(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, &pri_spec);
      اگر(rv)
        برو out;
    }
  }

  ctx->nw_out_blocked = 0;
  وقتی(!rv && !ctx->nw_out_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

out:
  اگر(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(data, cf, "nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    بازگشت CURLE_SEND_ERROR;
  }
  /* Defer flushing during the connect phase so that the SETTINGS and
   * other initial frames are sent together with the first request.
   * Unless we are 'connect_only' where the request will never come. */
  اگر(!cf->connected && !cf->conn->bits.connect_only)
    بازگشت CURLE_OK;
  بازگشت nw_out_flush(cf, data);
}

ایستا CURLcode stream_recv(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                            ساختار h2_stream_ctx *stream,
                            نویسه *buf, size_t len, size_t *pnread)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_AGAIN;

  (تهی)buf;
  (تهی)len;
  *pnread = 0;

  اگر(!stream->xfer_result)
    stream->xfer_result = cf_h2_update_local_win(cf, data, stream);

  اگر(stream->xfer_result) {
    CURL_TRC_CF(data, cf, "[%d] xfer write failed", stream->id);
    result = stream->xfer_result;
  }
  وگرنه اگر(stream->closed) {
    CURL_TRC_CF(data, cf, "[%d] returning CLOSE", stream->id);
    result = http2_handle_stream_close(cf, data, stream, pnread);
  }
  وگرنه اگر(stream->reset ||
          (ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) ||
          (ctx->rcvd_goaway && ctx->remote_max_sid < stream->id)) {
    CURL_TRC_CF(data, cf, "[%d] returning ERR", stream->id);
    result = data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2;
  }

  اگر(result && (result != CURLE_AGAIN))
    CURL_TRC_CF(data, cf, "[%d] stream_recv(len=%zu) -> %d, %zu",
                stream->id, len, (صحیح)result, *pnread);
  بازگشت result;
}

ایستا CURLcode h2_progress_ingress(ساختار Curl_cfilter *cf,
                                    ساختار Curl_easy *data,
                                    size_t data_max_bytes)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  اگر(should_close_session(ctx)) {
    CURL_TRC_CF(data, cf, "[0] ingress: session is closed");
    بازگشت CURLE_HTTP2;
  }

  /* Process network input buffer first */
  اگر(!Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "Process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    result = h2_process_pending_درون_داد(cf, data);
    اگر(result)
      بازگشت result;
  }

  اگر(!data_max_bytes)
    data_max_bytes = H2_CHUNK_SIZE;

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop due to connection close or us not processing
   * all network input */
  وقتی(!ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) {
    ساختار h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    اگر(stream && (stream->closed || !data_max_bytes)) {
      /* We would like to abort here and stop processing, so that the transfer
       * loop can handle the data/close here. This may leave data in
       * underlying buffers that will not be consumed. */
      اگر(!cf->next || !cf->next->cft->has_data_pending(cf->next, data))
        Curl_multi_mark_dirty(data);
      بشکن;
    }

    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    اگر(result) {
      اگر(result != CURLE_AGAIN) {
        failf(data, "Failed receiving HTTP2 data: %d(%s)", (صحیح)result,
              curl_easy_strerror(result));
        بازگشت result;
      }
      بشکن;
    }
    وگرنه اگر(nread == 0) {
      CURL_TRC_CF(data, cf, "[0] ingress: connection closed");
      ctx->conn_closed = TRUE;
      بشکن;
    }
    وگرنه {
      CURL_TRC_CF(data, cf, "[0] ingress: read %zu bytes", nread);
      data_max_bytes = (data_max_bytes > nread) ? (data_max_bytes - nread) : 0;
    }

    result = h2_process_pending_درون_داد(cf, data);
    اگر(result)
      بازگشت result;
    CURL_TRC_CF(data, cf, "[0] ingress: nw-in buffered %zu",
                Curl_bufq_len(&ctx->inbufq));
  }

  اگر(ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "server closed %s",
                ctx->rcvd_goaway ? "with GOAWAY" : "abruptly");
    connclose(cf->conn);
  }

  CURL_TRC_CF(data, cf, "[0] ingress: done");
  بازگشت CURLE_OK;
}

ایستا CURLcode cf_h2_recv(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                           نویسه *buf, size_t len, size_t *pnread)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;
  ساختار h2_stream_ctx *stream;
  CURLcode result, r2;
  ساختار cf_call_data save;

  اگر(!data)
    بازگشت CURLE_HTTP2;

  stream = H2_STREAM_CTX(ctx, data);

  *pnread = 0;
  اگر(!stream) {
    /* Abnormal call sequence: either this transfer has never opened a stream
     * (unlikely) or the transfer has been done, cleaned up its resources, but
     * a read() is called anyway. It is not clear what the calling sequence
     * is for such a case. */
    failf(data, "http/2 recv on a transfer never opened "
          "or already cleared, mid=%u", data->mid);
    بازگشت CURLE_HTTP2;
  }

  CF_DATA_SAVE(save, cf, data);

  result = stream_recv(cf, data, stream, buf, len, pnread);
  اگر(result && (result != CURLE_AGAIN))
    برو out;

  اگر(result) {
    result = h2_progress_ingress(cf, data, len);
    اگر(result)
      برو out;

    result = stream_recv(cf, data, stream, buf, len, pnread);
  }

  اگر(*pnread > 0) {
    /* Now that we transferred this to the upper layer, we report
     * the actual amount of DATA consumed to the H2 session, so
     * that it adjusts stream flow control */
    Curl_ngh2_session_consume(&ctx->ngh2, stream->id, *pnread);
    اگر(stream->closed) {
      CURL_TRC_CF(data, cf, "[%d] DRAIN closed stream", stream->id);
      Curl_multi_mark_dirty(data);
    }
  }

out:
  r2 = h2_progress_egress(cf, data);
  اگر(r2 == CURLE_AGAIN) {
    /* pending data to send, need to be called again. Ideally, we
     * monitor the socket for POLLOUT, but when not SENDING
     * any more, we force processing of the transfer. */
    اگر(!CURL_REQ_WANT_SEND(data))
      Curl_multi_mark_dirty(data);
  }
  وگرنه اگر(r2 && (result == CURLE_AGAIN)) {
    /* Do not replace a previous error result NOR a successful result
     * with an egress error. Example: the server might close the
     * connection after sending the complete response with EOS
     * and everything. Further sending may fail, but that MUST
     * not change that we had a successful response. */
    result = r2;
  }

  CURL_TRC_CF(data, cf, "[%d] cf_recv(len=%zu) -> %d, %zu, "
              "window=%d/%d, connection %d/%d",
              stream->id, len, (صحیح)result, *pnread,
              Curl_ngh2_session_get_stream_effective_recv_data_length(
                &ctx->ngh2, stream->id),
              Curl_ngh2_session_get_stream_effective_local_window_size(
                &ctx->ngh2, stream->id),
              Curl_ngh2_session_get_local_window_size(&ctx->ngh2),
              HTTP2_HUGE_WINDOW_SIZE);

  CF_DATA_RESTORE(cf, save);
  بازگشت result;
}

ایستا CURLcode cf_h2_body_send(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data,
                                ساختار h2_stream_ctx *stream,
                                ثابت تهی *buf, size_t blen, bool eos,
                                size_t *pnwritten)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;
  CURLcode result;

  *pnwritten = 0;
  اگر(stream->closed) {
    اگر(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(data, cf, "[%d] discarding data"
                  "on closed stream with response", stream->id);
      اگر(eos)
        stream->body_eos = TRUE;
      *pnwritten = blen;
      بازگشت CURLE_OK;
    }
    /* Server closed before we got a response, this is an error */
    infof(data, "stream %d closed", stream->id);
    بازگشت CURLE_SEND_ERROR;
  }

  result = Curl_bufq_write(&stream->sendbuf, buf, blen, pnwritten);
  اگر(result)
    بازگشت result;

  اگر(eos && (blen == *pnwritten))
    stream->body_eos = TRUE;

  اگر(eos || !Curl_bufq_is_empty(&stream->sendbuf)) {
    /* resume the potentially suspended stream */
    اگر(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id)))
      بازگشت CURLE_SEND_ERROR;
  }

  بازگشت CURLE_OK;
}

ایستا CURLcode h2_submit(ساختار h2_stream_ctx **pstream,
                          ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                          ثابت uint8_t *buf, size_t len,
                          bool eos, size_t *pnwritten)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;
  ساختار h2_stream_ctx *stream = پوچ;
  ساختار dynhds h2_headers;
  nghttp2_nv *nva = پوچ;
  ثابت تهی *body = پوچ;
  size_t nheader, bodylen;
  nghttp2_data_provider data_prd;
  int32_t stream_id;
  nghttp2_priority_spec pri_spec;
  size_t nwritten;
  CURLcode result = CURLE_OK;
  uint32_t initial_win_size;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_headers, MAX_HTTP_RESP_HEADER_شمار, DYN_HTTP_REQUEST);

  result = http2_data_setup(cf, data, &stream);
  اگر(result)
    برو out;

  result = Curl_h1_req_parse_read(
    &stream->h1, buf, len, پوچ,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : پوچ,
    0, &nwritten);
  اگر(result)
    برو out;
  *pnwritten = nwritten;
  اگر(!stream->h1.done) {
    /* need more data */
    برو out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  اگر(result)
    برو out;
  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nva = Curl_dynhds_to_nva(&h2_headers, &nheader);
  اگر(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }

  h2_pri_spec(data, &pri_spec);
  اگر(!Curl_ngh2_session_check_request_allowed(&ctx->ngh2))
    CURL_TRC_CF(data, cf, "send request NOT allowed (via nghttp2)");

  /* Check the initial windows size of the transfer (rate-limits?) and
   * send an updated settings on changes from previous value. */
  initial_win_size = cf_h2_initial_win_size(data);
  اگر(initial_win_size != ctx->initial_win_size) {
    result = cf_h2_update_settings(ctx, initial_win_size);
    اگر(result)
      برو out;
  }

  انتخاب(data->state.httpreq) {
  حالت HTTPREQ_POST:
  حالت HTTPREQ_POST_FORM:
  حالت HTTPREQ_POST_MIME:
  حالت HTTPREQ_PUT:
    data_prd.read_callback = req_body_read_callback;
    data_prd.source.ptr = پوچ;
    stream_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, &data_prd, stream);
    بشکن;
  پیشفرض:
    stream_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, پوچ, stream);
  }

  اگر(stream_id < 0) {
    CURL_TRC_CF(data, cf, "send: nghttp2_submit_request error (%s)%d",
                nghttp2_strerror(stream_id), stream_id);
    result = CURLE_SEND_ERROR;
    برو out;
  }

#ifdef CURLVERBOSE
#define MAX_ACC 60000  /* <64KB to account for some overhead */
  اگر(Curl_trc_is_verbose(data)) {
    size_t acc = 0, i;

    infof(data, "[HTTP/2] [%d] OPENED stream for %s",
          stream_id, Curl_bufref_ptr(&data->state.url));
    برای(i = 0; i < nheader; ++i) {
      acc += nva[i].namelen + nva[i].valuelen;

      infof(data, "[HTTP/2] [%d] [%.*s: %.*s]", stream_id,
            (صحیح)nva[i].namelen, nva[i].name,
            (صحیح)nva[i].valuelen, nva[i].مقدار);
    }

    اگر(acc > MAX_ACC) {
      infof(data, "[HTTP/2] Warning: The cumulative length of all "
            "headers exceeds %d bytes and that could cause the "
            "stream to be rejected.", MAX_ACC);
    }
  }
#endif

  stream->id = stream_id;

  body = (ثابت نویسه *)buf + *pnwritten;
  bodylen = len - *pnwritten;

  اگر(bodylen || eos) {
    size_t n;
    result = cf_h2_body_send(cf, data, stream, body, bodylen, eos, &n);
    اگر(!result)
      *pnwritten += n;
    وگرنه اگر(result == CURLE_AGAIN)
      result = CURLE_OK;
    وگرنه {
      result = CURLE_SEND_ERROR;
    }
  }

out:
  CURL_TRC_CF(data, cf, "[%d] submit -> %d, %zu",
              stream ? stream->id : -1, (صحیح)result, *pnwritten);
  curlx_safefree(nva);
  *pstream = stream;
  Curl_dynhds_free(&h2_headers);
  بازگشت result;
}

ایستا CURLcode cf_h2_send(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                           ثابت uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;
  ساختار h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
  ساختار cf_call_data save;
  CURLcode result = CURLE_OK, r2;

  CF_DATA_SAVE(save, cf, data);
  *pnwritten = 0;

  اگر(!stream || stream->id == -1) {
    result = h2_submit(&stream, cf, data, buf, len, eos, pnwritten);
    اگر(result)
      برو out;
    DEBUGASSERT(stream);
  }
  وگرنه اگر(stream->body_eos) {
    /* We already wrote this, but CURLE_AGAIN-ed the call due to not
     * being able to flush stream->sendbuf. Make a 0-length write
     * to trigger flushing again.
     * If this works, we report to have written `len` bytes. */
    size_t n;
    DEBUGASSERT(eos);
    result = cf_h2_body_send(cf, data, stream, buf, 0, eos, &n);
    CURL_TRC_CF(data, cf, "[%d] cf_body_send last CHUNK -> %d, %zu, eos=%d",
                stream->id, (صحیح)result, n, eos);
    اگر(result)
      برو out;
    *pnwritten = len;
  }
  وگرنه {
    result = cf_h2_body_send(cf, data, stream, buf, len, eos, pnwritten);
    CURL_TRC_CF(data, cf, "[%d] cf_body_send(len=%zu) -> %d, %zu, eos=%d",
                stream->id, len, (صحیح)result, *pnwritten, eos);
  }

  /* Call the nghttp2 send loop and flush to write ALL buffered data,
   * headers and/or request body completely out to the network */
  r2 = h2_progress_egress(cf, data);

  /* if the stream has been closed in egress handling (nghttp2 does that
   * when it does not like the headers, for example */
  اگر(stream && stream->closed && !stream->resp_hds_complete) {
    infof(data, "stream %d closed", stream->id);
    result = CURLE_SEND_ERROR;
    برو out;
  }
  وگرنه اگر(r2 && (r2 != CURLE_AGAIN)) {
    result = r2;
    برو out;
  }

  اگر(should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    اگر(stream && stream->closed) {
      result = http2_handle_stream_close(cf, data, stream, pnwritten);
    }
    وگرنه {
      CURL_TRC_CF(data, cf, "send: nothing to do in this session");
      result = CURLE_HTTP2;
    }
  }

out:
  اگر(stream) {
    CURL_TRC_CF(data, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
                "eos=%d, h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                stream->id, len, (صحیح)result, *pnwritten,
                stream->body_eos,
                Curl_ngh2_session_get_stream_remote_window_size(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&stream->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  وگرنه {
    CURL_TRC_CF(data, cf, "cf_send(len=%zu) -> %d, %zu, "
                "connection-window=%d, nw_send_buffer(%zu)",
                len, (صحیح)result, *pnwritten,
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_DATA_RESTORE(cf, save);
  بازگشت result;
}

ایستا CURLcode cf_h2_flush(ساختار Curl_cfilter *cf,
                            ساختار Curl_easy *data)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;
  ساختار h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
  ساختار cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  اگر(stream && !Curl_bufq_is_empty(&stream->sendbuf)) {
    /* resume the potentially suspended stream */
    اگر(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id))) {
      result = CURLE_SEND_ERROR;
      برو out;
    }
  }

  result = h2_progress_egress(cf, data);

out:
  اگر(stream) {
    CURL_TRC_CF(data, cf, "[%d] flush -> %d, "
                "h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                stream->id, (صحیح)result,
                Curl_ngh2_session_get_stream_remote_window_size(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&stream->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  وگرنه {
    CURL_TRC_CF(data, cf, "flush -> %d, "
                "connection-window=%d, nw_send_buffer(%zu)", (صحیح)result,
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_DATA_RESTORE(cf, save);
  بازگشت result;
}

ایستا CURLcode cf_h2_adjust_pollset(ساختار Curl_cfilter *cf,
                                     ساختار Curl_easy *data,
                                     ساختار easy_pollset *ps)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;
  ساختار cf_call_data save;
  curl_socket_t sock;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  اگر(!Curl_ngh2_valid(&ctx->ngh2))
    بازگشت CURLE_OK;

  sock = Curl_conn_cf_get_socket(cf, data);
  Curl_pollset_check(data, ps, sock, &want_recv, &want_send);
  اگر(want_recv || want_send) {
    ساختار h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
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
  وگرنه اگر(ctx->sent_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_DATA_SAVE(save, cf, data);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CF_DATA_RESTORE(cf, save);
  }
  بازگشت result;
}

ایستا CURLcode cf_h2_ctx_open(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;
  ساختار h2_stream_ctx *stream;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  صحیح rc;
  nghttp2_session_callbacks *cbs = پوچ;

  DEBUGASSERT(!ctx->ngh2.s);
  DEBUGASSERT(!ctx->ngh2.fatal_rv);
  DEBUGASSERT(ctx->initialized);

  rc = nghttp2_session_callbacks_new(&cbs);
  اگر(rc) {
    failf(data, "Could not initialize nghttp2 callbacks");
    برو out;
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
  اگر(rc) {
    failf(data, "Could not initialize nghttp2");
    برو out;
  }
  ctx->max_concurrent_streams = data->multi ?
    Curl_multi_max_concurrent_streams(data->multi) :
    DEFAULT_MAX_CONCURRENT_STREAMS;

  اگر(ctx->via_h1_upgrade) {
    /* HTTP/1.1 Upgrade issued. H2 Settings have already been submitted
     * in the H1 request and we upgrade from there. This stream
     * is opened implicitly as #1. */
    uint8_t binsettings[H2_BINSETTINGS_LEN];
    ssize_t rclen;
    size_t binlen; /* length of the binsettings data */

    rclen = populate_binsettings(binsettings, data);

    اگر(!curlx_sztouz(rclen, &binlen) || !binlen) {
      failf(data, "nghttp2 unexpectedly failed on pack_settings_payload");
      result = CURLE_FAILED_INIT;
      برو out;
    }

    result = http2_data_setup(cf, data, &stream);
    اگر(result)
      برو out;
    DEBUGASSERT(stream);
    stream->id = 1;
    /* queue SETTINGS frame (again) */
    rc = Curl_ngh2_session_upgrade2(
      &ctx->ngh2, binsettings, binlen, data->state.httpreq == HTTPREQ_HEAD,
      پوچ);
    اگر(rc) {
      failf(data, "nghttp2_session_upgrade2() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      برو out;
    }

    rc = Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, stream->id, stream);
    اگر(rc) {
      infof(data, "http/2: failed to set user_data for stream %d",
            stream->id);
      DEBUGASSERT(0);
    }
    CURL_TRC_CF(data, cf, "created session via Upgrade");
  }
  وگرنه {
    nghttp2_settings_entry iv[H2_SETTINGS_IV_LEN];
    size_t ivlen;

    ivlen = populate_settings(iv, data, ctx);
    rc = Curl_ngh2_submit_settings(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, iv, ivlen);
    اگر(rc) {
      failf(data, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      برو out;
    }
  }

  rc = Curl_ngh2_session_set_local_window_size(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, HTTP2_HUGE_WINDOW_SIZE);
  اگر(rc) {
    failf(data, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    result = CURLE_HTTP2;
    برو out;
  }

  /* all set, traffic will be send on connect */
  result = CURLE_OK;
  CURL_TRC_CF(data, cf, "[0] created h2 session%s",
              ctx->via_h1_upgrade ? " (via h1 upgrade)" : "");

out:
  اگر(cbs)
    nghttp2_session_callbacks_del(cbs);
  بازگشت result;
}

ایستا CURLcode cf_h2_connect(ساختار Curl_cfilter *cf,
                              ساختار Curl_easy *data,
                              bool *done)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ساختار cf_call_data save;
  bool first_time = FALSE;

  اگر(cf->connected) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  /* Connect the lower filters first */
  اگر(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    اگر(result || !*done)
      بازگشت result;
  }

  *done = FALSE;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(ctx->initialized);
  اگر(!ctx->ngh2.s) {
    result = cf_h2_ctx_open(cf, data);
    اگر(result)
      برو out;
    first_time = TRUE;
  }

  اگر(!first_time) {
    result = h2_progress_ingress(cf, data, 0);
    اگر(result)
      برو out;
  }

  /* Send out our SETTINGS and ACKs and such. If that blocks, we
   * have it buffered and can count this filter as being connected */
  result = h2_progress_egress(cf, data);
  اگر(result && (result != CURLE_AGAIN))
    برو out;

  *done = TRUE;
  cf->connected = TRUE;
  result = CURLE_OK;

out:
  CURL_TRC_CF(data, cf, "cf_connect() -> %d, %d, ", (صحیح)result, *done);
  CF_DATA_RESTORE(cf, save);
  بازگشت result;
}

ایستا تهی cf_h2_destroy(ساختار Curl_cfilter *cf, ساختار Curl_easy *data)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;

  (تهی)data;
  اگر(ctx) {
    cf_h2_ctx_free(ctx);
    cf->ctx = پوچ;
  }
}

ایستا CURLcode cf_h2_shutdown(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data, bool *done)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;
  ساختار cf_call_data save;
  CURLcode result;
  صحیح rv;

  اگر(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);

  اگر(!ctx->sent_goaway) {
    ctx->sent_goaway = TRUE;
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, ctx->local_max_sid, 0,
      (ثابت uint8_t *)"shutdown", اندازه("shutdown"));
    اگر(rv) {
      failf(data, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      result = CURLE_SEND_ERROR;
      برو out;
    }
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  result = CURLE_OK;
  اگر(Curl_ngh2_session_want_write(&ctx->ngh2) ||
     !Curl_bufq_is_empty(&ctx->outbufq))
    result = h2_progress_egress(cf, data);
  اگر(!result && Curl_ngh2_session_want_read(&ctx->ngh2))
    result = h2_progress_ingress(cf, data, 0);

  اگر(result == CURLE_AGAIN)
    result = CURLE_OK;

  *done = (ctx->conn_closed ||
           (!result && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2) &&
            Curl_bufq_is_empty(&ctx->outbufq)));

out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  بازگشت result;
}

ایستا CURLcode http2_data_pause(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 bool pause)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;
  ساختار h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);

  DEBUGASSERT(data);
  اگر(ctx && Curl_ngh2_valid(&ctx->ngh2) && stream) {
    CURLcode result;

    stream->write_paused = pause;
    result = cf_h2_update_local_win(cf, data, stream);
    اگر(result)
      بازگشت result;

    /* attempt to send the window update */
    (تهی)h2_progress_egress(cf, data);

    اگر(!pause) {
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
  بازگشت CURLE_OK;
}

ایستا CURLcode cf_h2_cntrl(ساختار Curl_cfilter *cf,
                            ساختار Curl_easy *data,
                            صحیح event, صحیح arg1, تهی *arg2)
{
  CURLcode result = CURLE_OK;
  ساختار cf_call_data save;

  (تهی)arg2;

  CF_DATA_SAVE(save, cf, data);
  انتخاب(event) {
  حالت CF_CTRL_DATA_SETUP:
    بشکن;
  حالت CF_CTRL_DATA_PAUSE:
    result = http2_data_pause(cf, data, (arg1 != 0));
    بشکن;
  حالت CF_CTRL_FLUSH:
    result = cf_h2_flush(cf, data);
    بشکن;
  حالت CF_CTRL_DATA_DONE:
    http2_data_done(cf, data);
    بشکن;
  حالت CF_CTRL_CONN_INFO_UPDATE:
    اگر(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 20;
      Curl_conn_set_multiplex(cf->conn);
    }
    بشکن;
  پیشفرض:
    بشکن;
  }
  CF_DATA_RESTORE(cf, save);
  بازگشت result;
}

ایستا bool cf_h2_data_pending(ساختار Curl_cfilter *cf,
                               ثابت ساختار Curl_easy *data)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;

  اگر(ctx && !Curl_bufq_is_empty(&ctx->inbufq))
    بازگشت TRUE;
  بازگشت cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

ایستا bool cf_h2_is_alive(ساختار Curl_cfilter *cf,
                           ساختار Curl_easy *data,
                           bool *درون_داد_pending)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;
  bool alive;
  ساختار cf_call_data save;

  *درون_داد_pending = FALSE;
  CF_DATA_SAVE(save, cf, data);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
           http2_connisalive(cf, data, درون_داد_pending));
  CURL_TRC_CF(data, cf, "conn alive -> %d, input_pending=%d",
              alive, *درون_داد_pending);
  CF_DATA_RESTORE(cf, save);
  بازگشت alive;
}

ایستا CURLcode cf_h2_keep_alive(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data)
{
  CURLcode result;
  ساختار cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  result = http2_send_ping(cf, data);
  CF_DATA_RESTORE(cf, save);
  بازگشت result;
}

ایستا CURLcode cf_h2_query(ساختار Curl_cfilter *cf,
                            ساختار Curl_easy *data,
                            صحیح query, صحیح *pres1, تهی *pres2)
{
  ساختار cf_h2_ctx *ctx = cf->ctx;
  ساختار cf_call_data save;
  size_t effective_max;

  انتخاب(query) {
  حالت CF_QUERY_MAX_CONCURRENT:
    DEBUGASSERT(pres1);

    CF_DATA_SAVE(save, cf, data);
    اگر(!Curl_ngh2_valid(&ctx->ngh2) ||
       !Curl_ngh2_session_check_request_allowed(&ctx->ngh2)) {
      /* the limit is what we have in use right now */
      effective_max = cf->conn->attached_xfers;
    }
    وگرنه {
      effective_max = ctx->max_concurrent_streams;
    }
    *pres1 = (effective_max > INT_MAX) ? INT_MAX : (صحیح)effective_max;
    CF_DATA_RESTORE(cf, save);
    بازگشت CURLE_OK;
  حالت CF_QUERY_STREAM_ERROR: {
    ساختار h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    *pres1 = stream ? (صحیح)stream->error : 0;
    بازگشت CURLE_OK;
  }
  حالت CF_QUERY_NEED_FLUSH: {
    ساختار h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    اگر(!Curl_bufq_is_empty(&ctx->outbufq) ||
       (stream && !Curl_bufq_is_empty(&stream->sendbuf))) {
      *pres1 = TRUE;
      بازگشت CURLE_OK;
    }
    بشکن;
  }
  حالت CF_QUERY_HTTP_VERSION:
    *pres1 = 20;
    بازگشت CURLE_OK;
  پیشفرض:
    بشکن;
  }
  بازگشت cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساختار Curl_cftype Curl_cft_nghttp2 = {
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

ایستا CURLcode http2_cfilter_add(ساختار Curl_cfilter **pcf,
                                  ساختار Curl_easy *data,
                                  ساختار connectdata *conn,
                                  int8_t sockindex,
                                  bool via_h1_upgrade)
{
  ساختار Curl_cfilter *cf = پوچ;
  ساختار cf_h2_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(data->conn);
  ctx = curlx_calloc(1, اندازه(*ctx));
  اگر(!ctx)
    برو out;
  cf_h2_ctx_init(ctx, via_h1_upgrade);

  result = Curl_cf_create(&cf, &Curl_cft_nghttp2, ctx);
  اگر(result)
    برو out;

  ctx = پوچ;
  Curl_conn_cf_add(data, conn, sockindex, cf);

out:
  اگر(result)
    cf_h2_ctx_free(ctx);
  *pcf = result ? پوچ : cf;
  بازگشت result;
}

ایستا CURLcode http2_cfilter_درج_after(ساختار Curl_cfilter *cf,
                                           ساختار Curl_easy *data,
                                           bool via_h1_upgrade)
{
  ساختار Curl_cfilter *cf_h2 = پوچ;
  ساختار cf_h2_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (تهی)data;
  ctx = curlx_calloc(1, اندازه(*ctx));
  اگر(!ctx)
    برو out;
  cf_h2_ctx_init(ctx, via_h1_upgrade);

  result = Curl_cf_create(&cf_h2, &Curl_cft_nghttp2, ctx);
  اگر(result)
    برو out;

  ctx = پوچ;
  Curl_conn_cf_درج_after(cf, cf_h2);

out:
  اگر(result)
    cf_h2_ctx_free(ctx);
  بازگشت result;
}

bool Curl_http2_may_switch(ساختار Curl_easy *data)
{
  اگر(Curl_conn_http_version(data, data->conn) < 20 &&
     (data->state.http_neg.wanted & CURL_HTTP_V2x) &&
     data->state.http_neg.h2_prior_knowledge) {
#ifndef CURL_DISABLE_PROXY
    اگر(data->conn->bits.origin_is_proxy) {
      infof(data, "Ignoring HTTP/2 prior knowledge due to proxy");
      بازگشت FALSE;
    }
#endif
    بازگشت TRUE;
  }
  بازگشت FALSE;
}

CURLcode Curl_http2_switch(ساختار Curl_easy *data)
{
  ساختار Curl_cfilter *cf;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, data->conn) < 20);

  result = http2_cfilter_add(&cf, data, data->conn, FIRSTSOCKET, FALSE);
  اگر(result)
    بازگشت result;
  CURL_TRC_CF(data, cf, "switching connection to HTTP/2");

  اگر(cf->next) {
    bool done;
    بازگشت Curl_conn_cf_connect(cf, data, &done);
  }
  بازگشت CURLE_OK;
}

CURLcode Curl_http2_switch_at(ساختار Curl_cfilter *cf, ساختار Curl_easy *data)
{
  ساختار Curl_cfilter *cf_h2;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, data->conn) < 20);

  result = http2_cfilter_درج_after(cf, data, FALSE);
  اگر(result)
    بازگشت result;

  cf_h2 = cf->next;

  اگر(cf_h2->next) {
    bool done;
    بازگشت Curl_conn_cf_connect(cf_h2, data, &done);
  }
  بازگشت CURLE_OK;
}

CURLcode Curl_http2_upgrade(ساختار Curl_easy *data,
                            ساختار connectdata *conn, int8_t sockindex,
                            ثابت نویسه *mem, size_t nread)
{
  ساختار Curl_cfilter *cf;
  ساختار cf_h2_ctx *ctx;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, conn) < 20);

  result = http2_cfilter_add(&cf, data, conn, sockindex, TRUE);
  اگر(result)
    بازگشت result;
  CURL_TRC_CF(data, cf, "upgrading connection to HTTP/2");

  DEBUGASSERT(cf->cft == &Curl_cft_nghttp2);
  ctx = cf->ctx;

  data->req.httpversion_sent = 20; /* it is an h2 request now */
  data->req.header = TRUE; /* we expect the real response to come in h2 */
  data->req.headerline = 0; /* restart the header line counter */

  اگر(nread > 0) {
    /* Remaining data from the protocol switch reply is already using
     * the switched protocol, ie. HTTP/2. We add that to the network
     * inbufq. */
    size_t copied;

    result = Curl_bufq_write(&ctx->inbufq,
                             (ثابت unsigned نویسه *)mem, nread, &copied);
    اگر(result) {
      failf(data, "error on copying HTTP Upgrade response: %d", (صحیح)result);
      بازگشت CURLE_RECV_ERROR;
    }
    اگر(copied < nread) {
      failf(data, "connection buffer size could not take all data "
            "from HTTP Upgrade response header: copied=%zu, datalen=%zu",
            copied, nread);
      بازگشت CURLE_HTTP2;
    }
    infof(data, "Copied HTTP/2 data in stream buffer to connection buffer"
          " after upgrade: len=%zu", nread);
  }

  اگر(cf->next) {
    bool done;
    result = Curl_conn_cf_connect(cf, data, &done);
    اگر(!result)
      cf->cft->cntrl(cf, data, CF_CTRL_CONN_INFO_UPDATE, 0, پوچ);
  }
  بازگشت result;
}

/* Only call this function for a transfer that already got an HTTP/2
   CURLE_HTTP2_STREAM error! */
bool Curl_h2_http_1_1_error(ساختار Curl_easy *data)
{
  اگر(Curl_conn_http_version(data, data->conn) == 20) {
    صحیح err = Curl_conn_get_stream_error(data, data->conn, FIRSTSOCKET);
    بازگشت err == NGHTTP2_HTTP_1_1_REQUIRED;
  }
  بازگشت FALSE;
}

تهی *Curl_nghttp2_malloc(size_t size, تهی *user_data)
{
  (تهی)user_data;
  بازگشت Curl_cmalloc(size);
}

تهی Curl_nghttp2_free(تهی *ptr, تهی *user_data)
{
  (تهی)user_data;
  Curl_cfree(ptr);
}

تهی *Curl_nghttp2_calloc(size_t nmemb, size_t size, تهی *user_data)
{
  (تهی)user_data;
  بازگشت Curl_ccalloc(nmemb, size);
}

تهی *Curl_nghttp2_realloc(تهی *ptr, size_t size, تهی *user_data)
{
  (تهی)user_data;
  بازگشت Curl_crealloc(ptr, size);
}

#وگرنه /* CURL_DISABLE_HTTP || !USE_NGHTTP2 */

/* Satisfy external references even if http2 is not compiled in. */

نویسه *curl_pushheader_bynum(ساختار curl_pushheaders *h, size_t رقم)
{
  (تهی)h;
  (تهی)رقم;
  بازگشت پوچ;
}

نویسه *curl_pushheader_byname(ساختار curl_pushheaders *h, ثابت نویسه *name)
{
  (تهی)h;
  (تهی)name;
  بازگشت پوچ;
}

#endif /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
