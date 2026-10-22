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

#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#অন্তর্ভাব <nghttp2/nghttp2.h>

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "bufq.h"
#অন্তর্ভাব "u32_ptrset.h"
#অন্তর্ভাব "http1.h"
#অন্তর্ভাব "http2.h"
#অন্তর্ভাব "http.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "curlx/base64.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "ngh2_shim.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "urlapi-int.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "bufref.h"
#অন্তর্ভাব "curlx/dynbuf.h"
#অন্তর্ভাব "headers.h"
#অন্তর্ভাব "curl_share.h"

#যদি NGHTTP2_VERSION_অঙ্ক < 0x010f00
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
#যদি NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
#define H2_STREAM_WINDOW_SIZE_INITIAL  (64 * 1024)
#অন্যথা
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

সংরচনা cf_h2_ctx {
  সংরচনা curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  সংরচনা cf_call_data call_data;

  সংরচনা bufq inbufq;           /* network input */
  সংরচনা bufq outbufq;          /* network output */
  সংরচনা bufc_pool stream_bufcp; /* spares for stream buffers */
  সংরচনা dynbuf scratch;        /* scratch buffer for temp use */

  সংরচনা u32_ptrset streams; /* hash of `data->mid` to `h2_stream_ctx` */
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
  BIT(enable_ঠেলো);
  BIT(nw_out_blocked);
};

/* How to access `call_data` from a cf_h2 filter */
#undef CF_CTX_CALL_DATA
#define CF_CTX_CALL_DATA(cf) ((সংরচনা cf_h2_ctx *)(cf)->ctx)->call_data

/**
 * All about the H2 internals of a stream
 */
সংরচনা h2_stream_ctx {
  সংরচনা bufq sendbuf; /* request buffer */
  সংরচনা h1_req_parser h1; /* parsing the request */
  সংরচনা dynhds resp_trailers; /* response trailer fields */
  size_t resp_hds_len; /* amount of response header bytes in recvbuf */
  curl_off_t xfer_id;  /* transfer `id` owning this stream */
  curl_off_t nrcvd_data;  /* number of DATA bytes received */

  বর্ণ **ঠেলো_headers;       /* allocated array */
  size_t ঠেলো_headers_used;  /* number of entries filled in */
  size_t ঠেলো_headers_alloc; /* number of entries allocated */

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

স্থির শূন্য free_ঠেলো_headers(সংরচনা h2_stream_ctx *stream)
{
  size_t i;
  প্রতি(i = 0; i < stream->ঠেলো_headers_used; i++)
    curlx_free(stream->ঠেলো_headers[i]);
  curlx_safefree(stream->ঠেলো_headers);
  stream->ঠেলো_headers_used = 0;
}

স্থির শূন্য h2_stream_ctx_free(সংরচনা h2_stream_ctx *stream)
{
  Curl_bufq_free(&stream->sendbuf);
  Curl_h1_req_parse_free(&stream->h1);
  Curl_dynhds_free(&stream->resp_trailers);
  free_ঠেলো_headers(stream);
  curlx_free(stream);
}

স্থির শূন্য h2_stream_hash_free(uint32_t id, শূন্য *stream)
{
  (শূন্য)id;
  DEBUGASSERT(stream);
  h2_stream_ctx_free((সংরচনা h2_stream_ctx *)stream);
}

স্থির শূন্য cf_h2_ctx_init(সংরচনা cf_h2_ctx *ctx, bool via_h1_upgrade)
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

স্থির শূন্য cf_h2_ctx_free(সংরচনা cf_h2_ctx *ctx)
{
  যদি(ctx && ctx->initialized) {
    Curl_ngh2_del(&ctx->ngh2);
    Curl_bufq_free(&ctx->inbufq);
    Curl_bufq_free(&ctx->outbufq);
    Curl_bufcp_free(&ctx->stream_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    memset(ctx, 0, পরিমাণ(*ctx));
  }
  curlx_free(ctx);
}

স্থির uint32_t cf_h2_initial_win_size(সংরচনা Curl_easy *data)
{
#যদি NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
  /* If the transfer has a rate-limit lower than the default initial
   * stream window size, use that. It needs to be at least 8k or servers
   * may be unhappy. */
  curl_off_t rps = Curl_rlimit_per_step(&data->progress.dl.rlimit);
  যদি((rps > 0) && (rps < H2_STREAM_WINDOW_SIZE_INITIAL))
    প্রত্যাগম CURLMAX((uint32_t)rps, 8192);
#endif
  প্রত্যাগম H2_STREAM_WINDOW_SIZE_INITIAL;
}

স্থির size_t populate_settings(nghttp2_settings_entry *iv,
                                সংরচনা Curl_easy *data,
                                সংরচনা cf_h2_ctx *ctx)
{
  iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
  iv[0].মান = Curl_multi_max_concurrent_streams(data->multi);

  iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
  iv[1].মান = cf_h2_initial_win_size(data);
  যদি(ctx)
    ctx->initial_win_size = iv[1].মান;
  iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_ঠেলো;
  iv[2].মান = !!data->multi->ঠেলো_cb;

  প্রত্যাগম 3;
}

স্থির ssize_t populate_binsettings(uint8_t *binsettings,
                                    সংরচনা Curl_easy *data)
{
  nghttp2_settings_entry iv[H2_SETTINGS_IV_LEN];
  size_t ivlen;

  ivlen = populate_settings(iv, data, রিক্ত);
  /* this returns number of bytes it wrote or a negative number on error. */
  প্রত্যাগম nghttp2_pack_settings_payload(binsettings, H2_BINSETTINGS_LEN,
                                       iv, ivlen);
}

স্থির CURLcode cf_h2_update_settings(সংরচনা cf_h2_ctx *ctx,
                                      uint32_t initial_win_size)
{
  nghttp2_settings_entry entry;
  entry.settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
  entry.মান = initial_win_size;
  যদি(Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, &entry, 1))
    প্রত্যাগম CURLE_SEND_ERROR;
  ctx->initial_win_size = initial_win_size;
  প্রত্যাগম CURLE_OK;
}

#define H2_STREAM_CTX(ctx, data)                                         \
  ((সংরচনা h2_stream_ctx *)(                                             \
    (data) ? Curl_u32_ptrset_get(&(ctx)->streams, (data)->mid) : রিক্ত))

স্থির সংরচনা h2_stream_ctx *h2_stream_ctx_create(সংরচনা cf_h2_ctx *ctx,
                                                  সংরচনা Curl_easy *data)
{
  সংরচনা h2_stream_ctx *stream;

  stream = curlx_calloc(1, পরিমাণ(*stream));
  যদি(!stream)
    প্রত্যাগম রিক্ত;

  stream->id = -1;
  stream->xfer_id = data->id;
  stream->mid = data->mid;
  Curl_bufq_initp(&stream->sendbuf, &ctx->stream_bufcp,
                  H2_STREAM_SEND_CHUNKS, BUFQ_OPT_NONE);
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_সারি_LEN);
  Curl_dynhds_init(&stream->resp_trailers,
                   MAX_HTTP_RESP_HEADER_গুনতি, DYN_HTTP_REQUEST);
  stream->bodystarted = FALSE;
  stream->status_code = -1;
  stream->closed = FALSE;
  stream->close_handled = FALSE;
  stream->error = NGHTTP2_NO_ERROR;
  stream->local_window_size = H2_STREAM_WINDOW_SIZE_INITIAL;
  stream->nrcvd_data = 0;
  প্রত্যাগম stream;
}

স্থির সংরচনা Curl_easy *cf_h2_get_stream_xfer(সংরচনা Curl_cfilter *cf,
                                               int32_t stream_id,
                                               সংরচনা h2_stream_ctx **pstream)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  সংরচনা h2_stream_ctx *stream =
    Curl_ngh2_session_get_stream_user_data(&ctx->ngh2, stream_id);
  যদি(pstream)
    *pstream = stream;
  যদি(stream && (stream->mid != UINT32_MAX)) {
    সংরচনা Curl_easy *data, *call_data = CF_DATA_CURRENT(cf);
    যদি(!call_data || !call_data->multi) {
      DEBUGASSERT(0);
      প্রত্যাগম রিক্ত;
    }
    data = Curl_multi_get_easy(call_data->multi, stream->mid);
    যদি(data && (data->id == stream->xfer_id))
      প্রত্যাগম data;
  }
  প্রত্যাগম রিক্ত;
}

#ifdef NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
স্থির int32_t cf_h2_get_desired_local_win(সংরচনা Curl_cfilter *cf,
                                           সংরচনা Curl_easy *data)
{
  curl_off_t avail = Curl_rlimit_avail(&data->progress.dl.rlimit, রিক্ত);

  (শূন্য)cf;
  যদি(avail < CURL_OFF_T_MAX) { /* limit in place */
    যদি(avail <= 0)
      প্রত্যাগম 0;
    অন্যথা যদি(avail < H2_STREAM_WINDOW_SIZE_MAX)
      প্রত্যাগম (int32_t)avail;
  }
  প্রত্যাগম H2_STREAM_WINDOW_SIZE_MAX;
}

স্থির CURLcode cf_h2_update_local_win(সংরচনা Curl_cfilter *cf,
                                       সংরচনা Curl_easy *data,
                                       সংরচনা h2_stream_ctx *stream)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  int32_t dwsize;
  পূর্ণাঙ্ক rv;

  dwsize = (stream->write_paused || stream->xfer_result) ?
           0 : cf_h2_get_desired_local_win(cf, data);
  যদি(dwsize != stream->local_window_size) {
    int32_t wsize = Curl_ngh2_session_get_stream_effective_local_window_size(
      &ctx->ngh2, stream->id);
    যদি(dwsize > wsize) {
      rv = Curl_ngh2_session_set_local_window_size(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize);
      যদি(rv) {
        failf(data, "[%d] nghttp2 set_local_window_size(%d) failed: "
              "%s(%d)", stream->id, dwsize, nghttp2_strerror(rv), rv);
        প্রত্যাগম CURLE_HTTP2;
      }
      rv = Curl_ngh2_submit_window_update(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize - wsize);
      যদি(rv) {
        failf(data, "[%d] nghttp2_submit_window_update() failed: "
              "%s(%d)", stream->id, nghttp2_strerror(rv), rv);
        প্রত্যাগম CURLE_HTTP2;
      }
      stream->local_window_size = dwsize;
      CURL_TRC_CF(data, cf, "[%d] local window update by %d",
                  stream->id, dwsize - wsize);
    }
    অন্যথা {
      rv = Curl_ngh2_session_set_local_window_size(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize);
      যদি(rv) {
        failf(data, "[%d] nghttp2_session_set_local_window_size() failed: "
              "%s(%d)", stream->id, nghttp2_strerror(rv), rv);
        প্রত্যাগম CURLE_HTTP2;
      }
      stream->local_window_size = dwsize;
      CURL_TRC_CF(data, cf, "[%d] local window size now %d",
                  stream->id, dwsize);
    }
  }
  প্রত্যাগম CURLE_OK;
}

#অন্যথা /* NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

স্থির CURLcode cf_h2_update_local_win(সংরচনা Curl_cfilter *cf,
                                       সংরচনা Curl_easy *data,
                                       সংরচনা h2_stream_ctx *stream)
{
  (শূন্য)cf;
  (শূন্য)data;
  (শূন্য)stream;
  প্রত্যাগম CURLE_OK;
}
#endif /* !NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

স্থির CURLcode http2_data_setup(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 সংরচনা h2_stream_ctx **pstream)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  সংরচনা h2_stream_ctx *stream;

  (শূন্য)cf;
  DEBUGASSERT(data);

  যদি(!data)
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;

  stream = H2_STREAM_CTX(ctx, data);
  যদি(stream) {
    *pstream = stream;
    প্রত্যাগম CURLE_OK;
  }

  stream = h2_stream_ctx_create(ctx, data);
  যদি(!stream)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  যদি(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    h2_stream_ctx_free(stream);
    প্রত্যাগম CURLE_OUT_OF_MEMORY;
  }

  *pstream = stream;
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode nw_out_flush(সংরচনা Curl_cfilter *cf,
                             সংরচনা Curl_easy *data)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  যদি(Curl_bufq_is_empty(&ctx->outbufq))
    প্রত্যাগম CURLE_OK;

  result = Curl_cf_send_bufq(cf->next, data, &ctx->outbufq, রিক্ত, 0,
                             &nwritten);
  যদি(result) {
    যদি(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_out_blocked = 1;
    }
    প্রত্যাগম result;
  }
  প্রত্যাগম Curl_bufq_is_empty(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

স্থির শূন্য http2_data_done(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  সংরচনা h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);

  DEBUGASSERT(ctx);
  যদি(!stream || !ctx->initialized)
    প্রত্যাগম;

  যদি(Curl_ngh2_valid(&ctx->ngh2)) {
    bool flush_egress = FALSE;
    /* returns error if stream not known, which is fine here */
    (শূন্য)Curl_ngh2_session_set_stream_user_data(&ctx->ngh2, stream->id, রিক্ত);

    যদি(!stream->closed && stream->id > 0) {
      /* RST_STREAM */
      CURL_TRC_CF(data, cf, "[%d] premature DATA_DONE, RST stream",
                  stream->id);
      stream->closed = TRUE;
      stream->reset = TRUE;
      Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                  stream->id, NGHTTP2_STREAM_CLOSED);
      flush_egress = TRUE;
    }

    যদি(flush_egress) {
      (শূন্য)Curl_ngh2_session_send(&ctx->ngh2);
      (শূন্য)nw_out_flush(cf, data);
    }
  }

  Curl_u32_ptrset_unset(&ctx->streams, data->mid);
}

স্থির পূর্ণাঙ্ক h2_client_new(সংরচনা Curl_cfilter *cf,
                         nghttp2_session_callbacks *cbs)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { রিক্ত, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  পূর্ণাঙ্ক rc = nghttp2_option_new(&o);
  যদি(rc)
    প্রত্যাগম rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_set_no_auto_window_update(o, 1);
#যদি NGHTTP2_VERSION_অঙ্ক >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_set_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#endif
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  প্রত্যাগম rc;
}

/*
 * Returns nonzero if current HTTP/2 session should be closed.
 */
স্থির পূর্ণাঙ্ক should_close_session(সংরচনা cf_h2_ctx *ctx)
{
  প্রত্যাগম ctx->drain_total == 0 && !Curl_ngh2_session_want_read(&ctx->ngh2) &&
    !Curl_ngh2_session_want_write(&ctx->ngh2);
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
স্থির CURLcode h2_process_pending_প্রবেশ(সংরচনা Curl_cfilter *cf,
                                         সংরচনা Curl_easy *data)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  ধ্রুব unsigned বর্ণ *buf;
  size_t blen, nread;
  ssize_t rv;

  যাবত্(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {
    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ধ্রুব uint8_t *)buf, blen);
    যদি(!curlx_sztouz(rv, &nread)) {
      failf(data, "nghttp2 recv error %zd: %s", rv, nghttp2_strerror((পূর্ণাঙ্ক)rv));
      প্রত্যাগম CURLE_RECV_ERROR;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    যদি(Curl_bufq_is_empty(&ctx->inbufq)) {
      ভঙ্গ;
    }
    অন্যথা {
      CURL_TRC_CF(data, cf, "process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }

  যদি(Curl_ngh2_session_check_request_allowed(&ctx->ngh2) == 0) {
    /* No more requests are allowed in the current session, so
       the connection may not be reused. This is set when a
       GOAWAY frame has been received or when the limit of stream
       identifiers has been reached. */
    CURL_TRC_M(data, "http/2: No new requests allowed");
    connclose(cf->conn);
  }

  প্রত্যাগম CURLE_OK;
}

/*
 * The server may send us data at any point (e.g. PING frames). Therefore, we
 * cannot assume that an HTTP/2 socket is dead because it is readable.
 *
 * Check the lower filters first and, if successful, peek at the socket
 * and distinguish between closed and data.
 */
স্থির bool http2_connisalive(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                              bool *প্রবেশ_pending)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *প্রবেশ_pending = FALSE;
  যদি(!cf->next || !cf->next->cft->is_alive(cf->next, data, প্রবেশ_pending))
    প্রত্যাগম FALSE;

  যদি(*প্রবেশ_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode result;
    size_t nread;

    *প্রবেশ_pending = FALSE;
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(data, cf, "connisalive, recv pending input -> %d, %zu",
                (পূর্ণাঙ্ক)result, nread);
    যদি(!result) {
      result = h2_process_pending_প্রবেশ(cf, data);
      যদি(result)
        /* immediate error, considered dead */
        alive = FALSE;
      অন্যথা {
        alive = !should_close_session(ctx);
      }
    }
    অন্যথা যদি(result != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  প্রত্যাগম alive;
}

স্থির CURLcode http2_send_ping(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  পূর্ণাঙ্ক rc;

  rc = Curl_ngh2_submit_ping(&ctx->ngh2, 0, ZERO_NULL);
  যদি(rc) {
    failf(data, "nghttp2_submit_ping() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    প্রত্যাগম CURLE_HTTP2;
  }

  rc = Curl_ngh2_session_send(&ctx->ngh2);
  যদি(rc) {
    failf(data, "nghttp2_session_send() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    প্রত্যাগম CURLE_SEND_ERROR;
  }
  প্রত্যাগম CURLE_OK;
}

/*
 * Store nghttp2 version info in this buffer.
 */
শূন্য Curl_http2_ver(বর্ণ *p, size_t len)
{
  nghttp2_info *h2 = nghttp2_version(0);
  (শূন্য)curl_msnprintf(p, len, "nghttp2/%s", h2->version_str);
}

/*
 * The implementation of nghttp2_send_callback type. Here we write |data| with
 * size |length| to the network and return the number of bytes actually
 * written. See the documentation of nghttp2_send_callback for the details.
 */
স্থির ssize_t send_callback(nghttp2_session *h2,
                             ধ্রুব uint8_t *buf, size_t blen, পূর্ণাঙ্ক flags,
                             শূন্য *userp)
{
  সংরচনা Curl_cfilter *cf = userp;
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  সংরচনা Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result = CURLE_OK;

  (শূন্য)h2;
  (শূন্য)flags;
  DEBUGASSERT(data);

  যদি(!cf->connected)
    result = Curl_bufq_write(&ctx->outbufq, buf, blen, &nwritten);
  অন্যথা
    result = Curl_cf_send_bufq(cf->next, data, &ctx->outbufq, buf, blen,
                               &nwritten);

  যদি(result) {
    যদি(result == CURLE_AGAIN) {
      ctx->nw_out_blocked = 1;
      প্রত্যাগম NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(data, "Failed sending HTTP2 data");
    প্রত্যাগম NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  যদি(!nwritten) {
    ctx->nw_out_blocked = 1;
    প্রত্যাগম NGHTTP2_ERR_WOULDBLOCK;
  }
  প্রত্যাগম (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

/* We pass a pointer to this struct in the push callback, but the contents of
   the struct are hidden from the user. */
সংরচনা curl_pushheaders {
  সংরচনা Curl_easy *data;
  সংরচনা h2_stream_ctx *stream;
  ধ্রুব nghttp2_ঠেলো_promise *frame;
};

/*
 * push header access function. Only to be used from within the push callback
 */
বর্ণ *curl_pushheader_bynum(সংরচনা curl_pushheaders *h, size_t অঙ্ক)
{
  /* Verify that we got a good easy handle in the push header struct, mostly to
     detect rubbish input fast(er). */
  যদি(!h || !GOOD_EASY_HANDLE(h->data))
    প্রত্যাগম রিক্ত;
  অন্যথা {
    যদি(h->stream && অঙ্ক < h->stream->ঠেলো_headers_used)
      প্রত্যাগম h->stream->ঠেলো_headers[অঙ্ক];
  }
  প্রত্যাগম রিক্ত;
}

/*
 * push header access function. Only to be used from within the push callback
 */
বর্ণ *curl_pushheader_byname(সংরচনা curl_pushheaders *h, ধ্রুব বর্ণ *name)
{
  সংরচনা h2_stream_ctx *stream;
  size_t len;
  size_t i;
  /* Verify that we got a good easy handle in the push header struct,
     mostly to detect rubbish input fast(er). Also empty header name
     is rubbish too. We have to allow ":" at the beginning of
     the header, but header == ":" must be rejected. If we have ':' in
     the middle of header, it could be matched in middle of the value,
     this is because we do prefix match.*/
  যদি(!h || !GOOD_EASY_HANDLE(h->data) || !name || !name[0] ||
     !strcmp(name, ":") || strchr(name + 1, ':'))
    প্রত্যাগম রিক্ত;

  stream = h->stream;
  যদি(!stream)
    প্রত্যাগম রিক্ত;

  len = strlen(name);
  প্রতি(i = 0; i < stream->ঠেলো_headers_used; i++) {
    যদি(!strncmp(name, stream->ঠেলো_headers[i], len)) {
      /* sub-match, make sure that it is followed by a colon */
      যদি(stream->ঠেলো_headers[i][len] != ':')
        অনুবর্ত;
      প্রত্যাগম &stream->ঠেলো_headers[i][len + 1];
    }
  }
  প্রত্যাগম রিক্ত;
}

স্থির সংরচনা Curl_easy *h2_duphandle(সংরচনা Curl_cfilter *cf,
                                      সংরচনা Curl_easy *data)
{
  সংরচনা Curl_easy *second = curl_easy_init();
  যদি(second) {
    সংরচনা h2_stream_ctx *second_stream;
    http2_data_setup(cf, second, &second_stream);
    second->state.weight = data->state.weight;
    যদি(data->share)
      (শূন্য)Curl_share_easy_link(second, data->share);
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
  প্রত্যাগম second;
}

স্থির পূর্ণাঙ্ক set_transfer_url(সংরচনা Curl_easy *newhandle,
                            সংরচনা curl_pushheaders *hp,
                            সংরচনা Curl_easy *data)
{
  ধ্রুব বর্ণ *v;
  CURLUcode uc;
  বর্ণ *url = রিক্ত;
  পূর্ণাঙ্ক rc = 0;
  CURLU *u = curl_url();

  যদি(!u)
    প্রত্যাগম 5;

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_SCHEME);
  যদি(v) {
    uc = curl_url_set(u, CURLUPART_SCHEME, v, 0);
    যদি(uc) {
      rc = 1;
      গচ্ছ fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_AUTHORITY);
  যদি(v) {
    uc = Curl_url_set_authority(u, v);
    যদি(uc) {
      rc = 2;
      গচ্ছ fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_PATH);
  যদি(v) {
    uc = curl_url_set(u, CURLUPART_PATH, v, 0);
    যদি(uc) {
      rc = 3;
      গচ্ছ fail;
    }
  }

  /* We can only allow PUSH of resource from the same origin, e.g.
   * scheme + hostname + port */
  যদি(!Curl_url_same_origin(data->state.uh, u)) {
    rc = 1;
    গচ্ছ fail;
  }

  uc = curl_url_get(u, CURLUPART_URL, &url, 0);
  যদি(uc)
    rc = 4;
fail:
  যদি(rc) {
    curl_url_cleanup(u);
    প্রত্যাগম rc;
  }

  DEBUGASSERT(!newhandle->state.uh);
  newhandle->state.uh = u;
  Curl_bufref_set(&newhandle->state.url, url, 0, curl_free);
  প্রত্যাগম 0;
}

স্থির শূন্য discard_newhandle(সংরচনা Curl_cfilter *cf,
                              সংরচনা Curl_easy *newhandle)
{
  http2_data_done(cf, newhandle);
  (শূন্য)Curl_close(&newhandle);
}

স্থির পূর্ণাঙ্ক ঠেলো_promise(সংরচনা Curl_cfilter *cf,
                        সংরচনা Curl_easy *data,
                        ধ্রুব nghttp2_ঠেলো_promise *frame)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  পূর্ণাঙ্ক rv; /* one of the CURL_PUSH_* defines */

  CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE received",
              frame->promised_stream_id);
  যদি(data->multi->ঠেলো_cb) {
    সংরচনা h2_stream_ctx *stream;
    সংরচনা h2_stream_ctx *newstream;
    সংরচনা curl_pushheaders heads;
    CURLMcode mresult;
    CURLcode result;
    /* clone the parent */
    সংরচনা Curl_easy *newhandle = h2_duphandle(cf, data);
    যদি(!newhandle) {
      infof(data, "failed to duplicate handle");
      rv = CURL_ঠেলো_DENY; /* FAIL HARD */
      গচ্ছ fail;
    }

    stream = H2_STREAM_CTX(ctx, data);
    যদি(!stream) {
      failf(data, "Internal NULL stream");
      discard_newhandle(cf, newhandle);
      rv = CURL_ঠেলো_DENY;
      গচ্ছ fail;
    }

    heads.data = data;
    heads.stream = stream;
    heads.frame = frame;

    rv = set_transfer_url(newhandle, &heads, data);
    যদি(rv) {
      CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE, failed to set URL -> %d",
                  frame->promised_stream_id, rv);
      discard_newhandle(cf, newhandle);
      rv = CURL_ঠেলো_DENY;
      গচ্ছ fail;
    }

    {
      সংরচনা Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, multi_ঠেলো_cb);
      rv = data->multi->ঠেলো_cb(data, newhandle,
                                stream->ঠেলো_headers_used, &heads,
                                data->multi->ঠেলো_userp);
      CURL_CBAPI_END(&guard);
    }

    /* free the headers again */
    free_ঠেলো_headers(stream);

    যদি(rv) {
      DEBUGASSERT((rv > CURL_ঠেলো_OK) && (rv <= CURL_ঠেলো_ERROROUT));
      /* denied, kill off the new handle again */
      CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE, denied by application -> %d",
                  frame->promised_stream_id, rv);
      discard_newhandle(cf, newhandle);
      গচ্ছ fail;
    }

    /* approved, add to the multi handle for processing. This
     * assigns newhandle->mid. For the new `mid` we assign the
     * h2_stream instance and remember the stream_id already known. */
    mresult = Curl_multi_add_perform(data->multi, newhandle, cf->conn);
    যদি(mresult) {
      infof(data, "failed to add handle to multi");
      discard_newhandle(cf, newhandle);
      rv = CURL_ঠেলো_DENY;
      গচ্ছ fail;
    }

    result = http2_data_setup(cf, newhandle, &newstream);
    যদি(result) {
      failf(data, "error setting up stream: %d", (পূর্ণাঙ্ক)result);
      discard_newhandle(cf, newhandle);
      rv = CURL_ঠেলো_DENY;
      গচ্ছ fail;
    }

    DEBUGASSERT(newstream);
    newstream->id = frame->promised_stream_id;
    newhandle->req.maxdownload = -1;
    newhandle->req.size = -1;

    CURL_TRC_CF(data, cf, "promise easy handle added to multi, mid=%u",
                newhandle->mid);
    rv = Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, newstream->id, newstream);
    যদি(rv) {
      infof(data, "failed to set user_data for stream %d",
            newstream->id);
      DEBUGASSERT(0);
      discard_newhandle(cf, newhandle);
      rv = CURL_ঠেলো_DENY;
      গচ্ছ fail;
    }

    /* success, remember max stream id processed */
    যদি(newstream->id > ctx->local_max_sid)
      ctx->local_max_sid = newstream->id;
  }
  অন্যথা {
    CURL_TRC_CF(data, cf, "Got PUSH_PROMISE, ignore it");
    rv = CURL_ঠেলো_DENY;
  }
fail:
  প্রত্যাগম rv;
}

স্থির শূন্য h2_xfer_write_resp_hd(সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data,
                                  সংরচনা h2_stream_ctx *stream,
                                  ধ্রুব বর্ণ *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  যদি(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, blen, eos);
    যদি(!stream->xfer_result && !eos)
      stream->xfer_result = cf_h2_update_local_win(cf, data, stream);
    যদি(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%d] error %d writing %zu bytes of headers",
                  stream->id, (পূর্ণাঙ্ক)stream->xfer_result, blen);
  }
}

স্থির শূন্য h2_xfer_write_resp(সংরচনা Curl_cfilter *cf,
                               সংরচনা Curl_easy *data,
                               সংরচনা h2_stream_ctx *stream,
                               ধ্রুব বর্ণ *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  যদি(!stream->xfer_result)
    stream->xfer_result = Curl_xfer_write_resp(data, buf, blen, eos);
  /* If the transfer write is errored, we do not want any more data */
  যদি(stream->xfer_result) {
    সংরচনা cf_h2_ctx *ctx = cf->ctx;
    CURL_TRC_CF(data, cf, "[%d] error %d writing %zu bytes of data, "
                "RST-ing stream",
                stream->id, (পূর্ণাঙ্ক)stream->xfer_result, blen);
    Curl_ngh2_submit_rst_stream(
      &ctx->ngh2, 0, stream->id, (uint32_t)NGHTTP2_ERR_CALLBACK_FAILURE);
  }
  অন্যথা যদি(!stream->write_paused && Curl_xfer_write_is_paused(data)) {
    CURL_TRC_CF(data, cf, "[%d] stream output paused", stream->id);
    stream->write_paused = TRUE;
  }
  অন্যথা যদি(stream->write_paused && !Curl_xfer_write_is_paused(data)) {
    CURL_TRC_CF(data, cf, "[%d] stream output unpaused", stream->id);
    stream->write_paused = FALSE;
  }

  যদি(!stream->xfer_result && !eos)
    stream->xfer_result = cf_h2_update_local_win(cf, data, stream);
}

স্থির CURLcode on_stream_frame(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data,
                                সংরচনা h2_stream_ctx *stream,
                                ধ্রুব nghttp2_frame *frame)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  int32_t stream_id = frame->hd.stream_id;
  পূর্ণাঙ্ক rv;

  যদি(!stream) {
    CURL_TRC_CF(data, cf, "[%d] No stream_ctx set", stream_id);
    প্রত্যাগম CURLE_FAILED_INIT;
  }

  বিকল্প(frame->hd.type) {
  পক্ষ NGHTTP2_DATA:
    CURL_TRC_CF(data, cf, "[%d] DATA, window=%d/%d",
                stream_id,
                Curl_ngh2_session_get_stream_effective_recv_data_length(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_stream_effective_local_window_size(
                  &ctx->ngh2, stream->id));
    /* If !body started on this stream, then receiving DATA is illegal. */
    যদি(!stream->bodystarted) {
      যদি(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                       stream_id, NGHTTP2_PROTOCOL_ERROR))) {
        প্রত্যাগম CURLE_RECV_ERROR;
      }
    }
    ভঙ্গ;
  পক্ষ NGHTTP2_HEADERS:
    যদি(stream->bodystarted) {
      /* Only valid HEADERS after body started is trailer HEADERS. We
         buffer them in on_header callback. */
      ভঙ্গ;
    }

    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    যদি(stream->status_code == -1)
      প্রত্যাগম CURLE_RECV_ERROR;

    /* Only final status code signals the end of header */
    যদি(stream->status_code / 100 != 1) {
      stream->resp_hds_complete = TRUE;
      stream->bodystarted = TRUE;
    }
    অন্যথা
      stream->status_code = -1;

    h2_xfer_write_resp_hd(cf, data, stream, STRCONST("\r\n"),
                          (bool)stream->closed);

    Curl_multi_mark_dirty(data);
    ভঙ্গ;
  পক্ষ NGHTTP2_ঠেলো_PROMISE:
    rv = ঠেলো_promise(cf, data, &frame->ঠেলো_promise);
    যদি(rv) { /* deny! */
      DEBUGASSERT((rv > CURL_ঠেলো_OK) && (rv <= CURL_ঠেলো_ERROROUT));
      যদি(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
             frame->ঠেলো_promise.promised_stream_id, NGHTTP2_CANCEL)))
        প্রত্যাগম CURLE_SEND_ERROR;
      অন্যথা যদি(rv == CURL_ঠেলো_ERROROUT) {
        /* fail the parent transfer, not the whole session */
        CURL_TRC_CF(data, cf, "[%d] fail in PUSH_PROMISE received",
                    stream_id);
        যদি(!stream->xfer_result)
          stream->xfer_result = CURLE_RECV_ERROR;
        Curl_multi_mark_dirty(data);
      }
    }
    ভঙ্গ;
  পক্ষ NGHTTP2_RST_STREAM:
    যদি(frame->rst_stream.error_code)
      stream->reset_by_server = TRUE;
    Curl_multi_mark_dirty(data);
    ভঙ্গ;
  পক্ষ NGHTTP2_WINDOW_UPDATE:
    যদি(CURL_REQ_WANT_SEND(data) && Curl_bufq_is_empty(&stream->sendbuf)) {
      /* need more data, force processing of transfer */
      Curl_multi_mark_dirty(data);
    }
    অন্যথা যদি(!Curl_bufq_is_empty(&stream->sendbuf)) {
      /* resume the potentially suspended stream */
      যদি(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id)))
        প্রত্যাগম CURLE_SEND_ERROR;
    }
    ভঙ্গ;
  স্বভাব:
    ভঙ্গ;
  }

  যদি(frame->hd.flags & NGHTTP2_FLAG_END_STREAM) {
    যদি(!stream->closed && !stream->body_eos &&
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
  প্রত্যাগম CURLE_OK;
}

#ifdef CURLVERBOSE
পূর্ণাঙ্ক Curl_nghttp2_fr_print(ধ্রুব nghttp2_frame *frame, বর্ণ *buffer,
                          size_t blen)
{
  বিকল্প(frame->hd.type) {
  পক্ষ NGHTTP2_DATA: {
    প্রত্যাগম curl_msnprintf(buffer, blen,
                          "FRAME[DATA, len=%d, eos=%d, padlen=%d]",
                          (পূর্ণাঙ্ক)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_STREAM),
                          (পূর্ণাঙ্ক)frame->data.padlen);
  }
  পক্ষ NGHTTP2_HEADERS: {
    প্রত্যাগম curl_msnprintf(buffer, blen,
                          "FRAME[HEADERS, len=%d, hend=%d, eos=%d]",
                          (পূর্ণাঙ্ক)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_HEADERS),
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_STREAM));
  }
  পক্ষ NGHTTP2_PRIORITY: {
    প্রত্যাগম curl_msnprintf(buffer, blen,
                          "FRAME[PRIORITY, len=%d, flags=%d]",
                          (পূর্ণাঙ্ক)frame->hd.length, frame->hd.flags);
  }
  পক্ষ NGHTTP2_RST_STREAM: {
    প্রত্যাগম curl_msnprintf(buffer, blen,
                          "FRAME[RST_STREAM, len=%d, flags=%d, error=%u]",
                          (পূর্ণাঙ্ক)frame->hd.length, frame->hd.flags,
                          frame->rst_stream.error_code);
  }
  পক্ষ NGHTTP2_SETTINGS: {
    যদি(frame->hd.flags & NGHTTP2_FLAG_ACK) {
      প্রত্যাগম curl_msnprintf(buffer, blen, "FRAME[SETTINGS, ack=1]");
    }
    প্রত্যাগম curl_msnprintf(buffer, blen,
                          "FRAME[SETTINGS, len=%d]", (পূর্ণাঙ্ক)frame->hd.length);
  }
  পক্ষ NGHTTP2_ঠেলো_PROMISE:
    প্রত্যাগম curl_msnprintf(buffer, blen,
                          "FRAME[PUSH_PROMISE, len=%d, hend=%d]",
                          (পূর্ণাঙ্ক)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_HEADERS));
  পক্ষ NGHTTP2_PING:
    প্রত্যাগম curl_msnprintf(buffer, blen,
                          "FRAME[PING, len=%d, ack=%d]",
                          (পূর্ণাঙ্ক)frame->hd.length,
                          frame->hd.flags & NGHTTP2_FLAG_ACK);
  পক্ষ NGHTTP2_GOAWAY: {
    বর্ণ scratch[128];
    size_t s_len = CURL_ARRAYSIZE(scratch);
    size_t len = (frame->goaway.opaque_data_len < s_len) ?
      frame->goaway.opaque_data_len : s_len - 1;
    যদি(len)
      memcpy(scratch, frame->goaway.opaque_data, len);
    scratch[len] = '\0';
    প্রত্যাগম curl_msnprintf(buffer, blen,
                          "FRAME[GOAWAY, error=%u, reason='%s', "
                          "last_stream=%d]", frame->goaway.error_code,
                          scratch, frame->goaway.last_stream_id);
  }
  পক্ষ NGHTTP2_WINDOW_UPDATE: {
    প্রত্যাগম curl_msnprintf(buffer, blen,
                          "FRAME[WINDOW_UPDATE, incr=%d]",
                          frame->window_update.window_size_increment);
  }
  স্বভাব:
    প্রত্যাগম curl_msnprintf(buffer, blen, "FRAME[%d, len=%d, flags=%d]",
                          frame->hd.type, (পূর্ণাঙ্ক)frame->hd.length,
                          frame->hd.flags);
  }
}

স্থির পূর্ণাঙ্ক on_frame_send(nghttp2_session *session, ধ্রুব nghttp2_frame *frame,
                         শূন্য *userp)
{
  সংরচনা Curl_cfilter *cf = userp;
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  সংরচনা Curl_easy *data = CF_DATA_CURRENT(cf);

  (শূন্য)session;
  DEBUGASSERT(data);
  যদি(Curl_trc_cf_is_verbose(cf, data)) {
    বর্ণ buffer[256];
    পূর্ণাঙ্ক len;
    len = Curl_nghttp2_fr_print(frame, buffer, পরিমাণ(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] -> %s", frame->hd.stream_id, buffer);
  }
  যদি((frame->hd.type == NGHTTP2_GOAWAY) && !ctx->sent_goaway) {
    /* A GOAWAY not initiated by us, but by nghttp2 itself on detecting
     * a protocol error on the connection */
    failf(data, "nghttp2 shuts down connection with error %u: %s",
          frame->goaway.error_code,
          nghttp2_http2_strerror(frame->goaway.error_code));
  }
  প্রত্যাগম 0;
}
#endif /* CURLVERBOSE */

স্থির পূর্ণাঙ্ক on_frame_recv(nghttp2_session *session, ধ্রুব nghttp2_frame *frame,
                         শূন্য *userp)
{
  সংরচনা Curl_cfilter *cf = userp;
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  সংরচনা Curl_easy *data = CF_DATA_CURRENT(cf), *data_s;
  সংরচনা h2_stream_ctx *stream;
  int32_t stream_id = frame->hd.stream_id;

  (শূন্য)session;
  DEBUGASSERT(data);
#ifdef CURLVERBOSE
  যদি(Curl_trc_cf_is_verbose(cf, data)) {
    বর্ণ buffer[256];
    পূর্ণাঙ্ক len;
    len = Curl_nghttp2_fr_print(frame, buffer, পরিমাণ(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] <- %s", frame->hd.stream_id, buffer);
  }
#endif /* CURLVERBOSE */

  যদি(!stream_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(data);
    বিকল্প(frame->hd.type) {
    পক্ষ NGHTTP2_SETTINGS: {
      যদি(!(frame->hd.flags & NGHTTP2_FLAG_ACK)) {
        uint32_t max_conn = ctx->max_concurrent_streams;
        ctx->max_concurrent_streams =
          Curl_ngh2_session_get_remote_settings(
            &ctx->ngh2, NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS);
        ctx->enable_ঠেলো = Curl_ngh2_session_get_remote_settings(
          &ctx->ngh2, NGHTTP2_SETTINGS_ENABLE_ঠেলো) != 0;
        CURL_TRC_CF(data, cf, "[0] MAX_CONCURRENT_STREAMS: %u",
                    ctx->max_concurrent_streams);
        CURL_TRC_CF(data, cf, "[0] ENABLE_PUSH: %s",
                    ctx->enable_ঠেলো ? "TRUE" : "false");
        যদি(data && max_conn != ctx->max_concurrent_streams) {
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
        যদি(CURL_REQ_WANT_SEND(data))
          Curl_multi_mark_dirty(data);
      }
      ভঙ্গ;
    }
    পক্ষ NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->goaway_error = frame->goaway.error_code;
      ctx->remote_max_sid = frame->goaway.last_stream_id;
      যদি(data) {
        infof(data, "received GOAWAY, error=%u, last_stream=%d",
              ctx->goaway_error, ctx->remote_max_sid);
        Curl_multi_connchanged(data->multi);
      }
      ভঙ্গ;
    স্বভাব:
      ভঙ্গ;
    }
    প্রত্যাগম 0;
  }

  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  যদি(!data_s) {
    CURL_TRC_CF(data, cf, "[%d] No Curl_easy associated", stream_id);
    প্রত্যাগম 0;
  }

  প্রত্যাগম on_stream_frame(cf, data_s, stream, frame) ?
         NGHTTP2_ERR_CALLBACK_FAILURE : 0;
}

স্থির পূর্ণাঙ্ক cf_h2_on_invalid_frame_recv(nghttp2_session *session,
                                       ধ্রুব nghttp2_frame *frame,
                                       পূর্ণাঙ্ক ngerr, শূন্য *userp)
{
  সংরচনা Curl_cfilter *cf = userp;
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  সংরচনা Curl_easy *data;
  সংরচনা h2_stream_ctx *stream;
  int32_t stream_id = frame->hd.stream_id;

  (শূন্য)session;
  data = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  যদি(data) {
#ifdef CURLVERBOSE
    বর্ণ buffer[256];
    পূর্ণাঙ্ক len;
    len = Curl_nghttp2_fr_print(frame, buffer, পরিমাণ(buffer) - 1);
    buffer[len] = 0;
    failf(data, "[HTTP2] [%d] received invalid frame: %s, error %d: %s",
          stream_id, buffer, ngerr, nghttp2_strerror(ngerr));
#endif /* CURLVERBOSE */
    যদি(stream) {
      Curl_ngh2_submit_rst_stream(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, NGHTTP2_STREAM_CLOSED);
      stream->error = ngerr;
      stream->closed = TRUE;
      stream->reset = TRUE;
    }
  }
  /* Return no error, nghttp2 will RST/GOAWAY by itself when needed */
  প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক on_data_chunk_recv(nghttp2_session *session, uint8_t flags,
                              int32_t stream_id,
                              ধ্রুব uint8_t *mem, size_t len, শূন্য *userp)
{
  সংরচনা Curl_cfilter *cf = userp;
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  সংরচনা h2_stream_ctx *stream;
  সংরচনা Curl_easy *data_s;

  (শূন্য)session;
  (শূন্য)flags;

  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */
  DEBUGASSERT(CF_DATA_CURRENT(cf));

  /* get the stream from the hash based on Stream ID */
  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  যদি(!data_s) {
    /* Receiving a Stream ID not in the hash should not happen - unless
       we have aborted a transfer artificially and there were more data
       in the pipeline. Silently ignore. */
    CURL_TRC_CF(CF_DATA_CURRENT(cf), cf, "[%d] Data for unknown", stream_id);
    /* consumed explicitly as no one will read it */
    Curl_ngh2_session_consume(&ctx->ngh2, stream_id, len);
    প্রত্যাগম 0;
  }

  যদি(!stream)
    প্রত্যাগম NGHTTP2_ERR_CALLBACK_FAILURE;

  h2_xfer_write_resp(cf, data_s, stream, (ধ্রুব বর্ণ *)mem, len, FALSE);

  Curl_ngh2_session_consume(&ctx->ngh2, stream_id, len);
  stream->nrcvd_data += (curl_off_t)len;
  প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক on_stream_close(nghttp2_session *session, int32_t stream_id,
                           uint32_t error_code, শূন্য *userp)
{
  সংরচনা Curl_cfilter *cf = userp;
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  সংরচনা Curl_easy *data_s, *call_data = CF_DATA_CURRENT(cf);
  সংরচনা h2_stream_ctx *stream = রিক্ত;
  পূর্ণাঙ্ক rv;
  (শূন্য)session;

  DEBUGASSERT(call_data);
  /* stream id 0 is the connection, do not look there for streams. */
  data_s = stream_id ? cf_h2_get_stream_xfer(cf, stream_id, &stream) : রিক্ত;
  যদি(!data_s) {
    CURL_TRC_CF(call_data, cf,
                "[%d] on_stream_close, no easy set on stream", stream_id);
    প্রত্যাগম 0;
  }
  যদি(!GOOD_EASY_HANDLE(data_s)) {
    /* nghttp2 still has an easy registered for the stream which has
     * been freed be libcurl. This points to a code path that does not
     * trigger DONE or DETACH events as it must. */
    CURL_TRC_CF(call_data, cf,
                "[%d] on_stream_close, not a GOOD easy on stream", stream_id);
    (শূন্য)Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, stream_id, রিক্ত);
    প্রত্যাগম NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  যদি(!stream) {
    CURL_TRC_CF(data_s, cf,
                "[%d] on_stream_close, GOOD easy but no stream", stream_id);
    প্রত্যাগম NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  stream->closed = TRUE;
  stream->error = error_code;
  যদি(stream->error) {
    stream->reset = TRUE;
    যদি((stream->error == NGHTTP2_REFUSED_STREAM) &&
       ctx->rcvd_goaway && ctx->goaway_error)
    /* REFUSED_STREAM is used by nghttp2 on a GOAWAY from the
     * server where it indicated it will no longer process this stream.
     * If the GOAWAY carried an error code, take that as the error for
     * the stream. */
    stream->error = ctx->goaway_error;
  }

  যদি(stream->error)
    CURL_TRC_CF(data_s, cf, "[%d] RESET: %s (err %u)",
              stream_id, nghttp2_http2_strerror(error_code), error_code);
  অন্যথা
    CURL_TRC_CF(data_s, cf, "[%d] CLOSED", stream_id);
  Curl_multi_mark_dirty(data_s);

  /* remove `stream_ctx` from the nghttp2 stream */
  rv = Curl_ngh2_session_set_stream_user_data(
    &ctx->ngh2, stream_id, রিক্ত);
  যদি(rv) {
    infof(data_s, "http/2: failed to clear user_data for stream %d",
          stream_id);
    DEBUGASSERT(0);
  }
  প্রত্যাগম 0;
}

স্থির শূন্য cf_h2_header_error(সংরচনা Curl_cfilter *cf,
                               সংরচনা Curl_easy *data,
                               সংরচনা h2_stream_ctx *stream,
                               CURLcode result)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;

  failf(data, "Error receiving HTTP2 header: %d(%s)", (পূর্ণাঙ্ক)result,
        curl_easy_strerror(result));
  যদি(stream) {
    Curl_ngh2_submit_rst_stream(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, NGHTTP2_STREAM_CLOSED);
    stream->closed = TRUE;
    stream->reset = TRUE;
  }
}

/* frame->hd.type is either NGHTTP2_HEADERS or NGHTTP2_PUSH_PROMISE */
স্থির পূর্ণাঙ্ক on_header(nghttp2_session *session, ধ্রুব nghttp2_frame *frame,
                     ধ্রুব uint8_t *name, size_t namelen,
                     ধ্রুব uint8_t *মান, size_t valuelen,
                     uint8_t flags,
                     শূন্য *userp)
{
  সংরচনা Curl_cfilter *cf = userp;
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  সংরচনা h2_stream_ctx *stream;
  সংরচনা Curl_easy *data;
  int32_t stream_id = frame->hd.stream_id;
  CURLcode result;

  (শূন্য)session;
  (শূন্য)flags;

  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */

  /* get the stream from the hash based on Stream ID */
  data = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  যদি(!GOOD_EASY_HANDLE(data))
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    প্রত্যাগম NGHTTP2_ERR_CALLBACK_FAILURE;

  যদি(!stream) {
    failf(data, "Internal NULL stream");
    প্রত্যাগম NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  /* Store received PUSH_PROMISE headers to be used when the subsequent
     PUSH_PROMISE callback comes */
  যদি(frame->hd.type == NGHTTP2_ঠেলো_PROMISE) {
    বর্ণ *h;

    যদি((namelen == CURL_CSTRLEN(HTTP_PSEUDO_AUTHORITY)) &&
       !strncmp(HTTP_PSEUDO_AUTHORITY, (ধ্রুব বর্ণ *)name, namelen)) {
      /* pseudo headers are lower case */
      পূর্ণাঙ্ক rc = 0;
      বর্ণ *check = curl_maprintf("%s:%d", data->state.origin->hostname,
                                  data->state.origin->port);
      যদি(!check)
        /* no memory */
        প্রত্যাগম NGHTTP2_ERR_CALLBACK_FAILURE;
      যদি(!curl_strequal(check, (ধ্রুব বর্ণ *)মান) &&
         ((data->state.origin->port != data->state.origin->scheme->defport) ||
          !curl_strequal(data->state.origin->hostname, (ধ্রুব বর্ণ *)মান))) {
        /* This is push is not for the same authority that was asked for in
         * the URL. RFC 7540 section 8.2 says: "A client MUST treat a
         * PUSH_PROMISE for which the server is not authoritative as a stream
         * error of type PROTOCOL_ERROR."
         */
        (শূন্য)Curl_ngh2_submit_rst_stream(
          &ctx->ngh2, NGHTTP2_FLAG_NONE, stream_id, NGHTTP2_PROTOCOL_ERROR);
        rc = NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      curlx_free(check);
      যদি(rc)
        প্রত্যাগম rc;
    }

    যদি(!stream->ঠেলো_headers) {
      stream->ঠেলো_headers_alloc = 10;
      stream->ঠেলো_headers = curlx_malloc(stream->ঠেলো_headers_alloc *
                                          পরিমাণ(বর্ণ *));
      যদি(!stream->ঠেলো_headers)
        প্রত্যাগম NGHTTP2_ERR_CALLBACK_FAILURE;
      stream->ঠেলো_headers_used = 0;
    }
    অন্যথা যদি(stream->ঠেলো_headers_used ==
            stream->ঠেলো_headers_alloc) {
      বর্ণ **headp;
      যদি(stream->ঠেলো_headers_alloc > 1000) {
        /* this is beyond crazy many headers, bail out */
        failf(data, "Too many PUSH_PROMISE headers");
        free_ঠেলো_headers(stream);
        প্রত্যাগম NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      stream->ঠেলো_headers_alloc *= 2;
      headp = curlx_realloc(stream->ঠেলো_headers,
                            stream->ঠেলো_headers_alloc * পরিমাণ(বর্ণ *));
      যদি(!headp) {
        free_ঠেলো_headers(stream);
        প্রত্যাগম NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      stream->ঠেলো_headers = headp;
    }
    h = curl_maprintf("%s:%s", name, মান);
    যদি(!h) {
      free_ঠেলো_headers(stream);
      প্রত্যাগম NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    stream->ঠেলো_headers[stream->ঠেলো_headers_used++] = h;
    প্রত্যাগম 0;
  }

  যদি(stream->bodystarted) {
    /* This is a trailer */
    CURL_TRC_CF(data, cf, "[%d] trailer: %.*s: %.*s",
                stream->id, (পূর্ণাঙ্ক)namelen, name, (পূর্ণাঙ্ক)valuelen, মান);
    result = Curl_dynhds_add(&stream->resp_trailers,
                             (ধ্রুব বর্ণ *)name, namelen,
                             (ধ্রুব বর্ণ *)মান, valuelen);
    যদি(result) {
      cf_h2_header_error(cf, data, stream, result);
      প্রত্যাগম NGHTTP2_ERR_CALLBACK_FAILURE;
    }

    প্রত্যাগম 0;
  }

  যদি(namelen == CURL_CSTRLEN(HTTP_PSEUDO_STATUS) &&
     !memcmp(HTTP_PSEUDO_STATUS, name, namelen)) {
    /* nghttp2 guarantees :status is received first and only once. */
    বর্ণ buffer[32];
    size_t hlen;
    result = Curl_http_decode_status(&stream->status_code,
                                     (ধ্রুব বর্ণ *)মান, valuelen);
    যদি(result) {
      cf_h2_header_error(cf, data, stream, result);
      প্রত্যাগম NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    hlen = curl_msnprintf(buffer, পরিমাণ(buffer), HTTP_PSEUDO_STATUS ":%d\r",
                          stream->status_code);
    result = Curl_headers_ঠেলো(data, buffer, hlen, CURLH_PSEUDO);
    যদি(result) {
      cf_h2_header_error(cf, data, stream, result);
      প্রত্যাগম NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    curlx_dyn_reset(&ctx->scratch);
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/2 "));
    যদি(!result)
      result = curlx_dyn_addn(&ctx->scratch, মান, valuelen);
    যদি(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    যদি(!result)
      h2_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    যদি(result) {
      cf_h2_header_error(cf, data, stream, result);
      প্রত্যাগম NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    /* if we receive data for another handle, wake that up */
    যদি(CF_DATA_CURRENT(cf) != data)
      Curl_multi_mark_dirty(data);

    CURL_TRC_CF(data, cf, "[%d] status: HTTP/2 %03d",
                stream->id, stream->status_code);
    প্রত্যাগম 0;
  }

  /* nghttp2 guarantees that namelen > 0, and :status was already
     received, and this is not pseudo-header field . */
  /* convert to an HTTP1-style header */
  curlx_dyn_reset(&ctx->scratch);
  result = curlx_dyn_addn(&ctx->scratch, (ধ্রুব বর্ণ *)name, namelen);
  যদি(!result)
    result = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
  যদি(!result)
    result = curlx_dyn_addn(&ctx->scratch, (ধ্রুব বর্ণ *)মান, valuelen);
  যদি(!result)
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
  যদি(!result)
    h2_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                          curlx_dyn_len(&ctx->scratch), FALSE);
  যদি(result) {
    cf_h2_header_error(cf, data, stream, result);
    প্রত্যাগম NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  /* if we receive data for another handle, wake that up */
  যদি(CF_DATA_CURRENT(cf) != data)
    Curl_multi_mark_dirty(data);

  CURL_TRC_CF(data, cf, "[%d] header: %.*s: %.*s",
              stream->id, (পূর্ণাঙ্ক)namelen, name, (পূর্ণাঙ্ক)valuelen, মান);

  প্রত্যাগম 0; /* 0 is successful */
}

স্থির ssize_t req_body_read_callback(nghttp2_session *session,
                                      int32_t stream_id,
                                      uint8_t *buf, size_t length,
                                      uint32_t *data_flags,
                                      nghttp2_data_source *source,
                                      শূন্য *userp)
{
  সংরচনা Curl_cfilter *cf = userp;
  সংরচনা Curl_easy *data_s;
  সংরচনা h2_stream_ctx *stream = রিক্ত;
  CURLcode result;
  ssize_t nread;
  size_t n;
  (শূন্য)source;

  (শূন্য)session;
  যদি(!stream_id)
    প্রত্যাগম NGHTTP2_ERR_INVALID_ARGUMENT;

  /* get the stream from the hash based on Stream ID, stream ID zero is for
     connection-oriented stuff */
  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  যদি(!data_s || !stream)
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    প্রত্যাগম NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_bufq_read(&stream->sendbuf, buf, length, &n);
  যদি(result) {
    যদি(result != CURLE_AGAIN)
      প্রত্যাগম NGHTTP2_ERR_CALLBACK_FAILURE;
    nread = 0;
  }
  অন্যথা
    nread = (ssize_t)n;

  CURL_TRC_CF(data_s, cf, "[%d] req_body_read(len=%zu) eos=%d -> %zd, %d",
              stream_id, length, stream->body_eos, nread, (পূর্ণাঙ্ক)result);

  যদি(stream->body_eos && Curl_bufq_is_empty(&stream->sendbuf)) {
    *data_flags = NGHTTP2_DATA_FLAG_EOF;
    প্রত্যাগম nread;
  }
  প্রত্যাগম (nread == 0) ? NGHTTP2_ERR_DEFERRED : nread;
}

#ifdef CURLVERBOSE
স্থির পূর্ণাঙ্ক error_callback(nghttp2_session *session,
                          ধ্রুব বর্ণ *বার্তা,
                          size_t len,
                          শূন্য *userp)
{
  সংরচনা Curl_cfilter *cf = userp;
  সংরচনা Curl_easy *data = CF_DATA_CURRENT(cf);
  (শূন্য)session;
  failf(data, "%.*s", (পূর্ণাঙ্ক)len, বার্তা);
  প্রত্যাগম 0;
}
#endif

/*
 * Append headers to ask for an HTTP1.1 to HTTP2 upgrade.
 */
CURLcode Curl_http2_request_upgrade(সংরচনা dynbuf *req,
                                    সংরচনা Curl_easy *data)
{
  CURLcode result;
  বর্ণ *base64;
  size_t blen;
  সংরচনা SingleRequest *k = &data->req;
  uint8_t binsettings[H2_BINSETTINGS_LEN];
  ssize_t rc;
  size_t binlen; /* length of the binsettings data */

  rc = populate_binsettings(binsettings, data);
  যদি(!curlx_sztouz(rc, &binlen) || !binlen) {
    failf(data, "nghttp2 unexpectedly failed on pack_settings_payload");
    curlx_dyn_free(req);
    প্রত্যাগম CURLE_FAILED_INIT;
  }

  result = curlx_base64url_encode(binsettings, binlen, &base64, &blen);
  যদি(result) {
    curlx_dyn_free(req);
    প্রত্যাগম result;
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

  প্রত্যাগম result;
}

স্থির CURLcode http2_handle_stream_close(সংরচনা Curl_cfilter *cf,
                                          সংরচনা Curl_easy *data,
                                          সংরচনা h2_stream_ctx *stream,
                                          size_t *pnlen)
{
  CURLcode result;

  *pnlen = 0;
  যদি(stream->reset) {
    যদি(stream->error == NGHTTP2_REFUSED_STREAM) {
      infof(data, "HTTP/2 stream %d refused by server, try again on a new "
                  "connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      প্রত্যাগম CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    অন্যথা যদি(stream->resp_hds_complete && data->req.no_body) {
      CURL_TRC_CF(data, cf, "[%d] error after response headers, but we did "
                  "not want a body anyway, ignore: %s (err %u)",
                  stream->id, nghttp2_http2_strerror(stream->error),
                  stream->error);
      stream->close_handled = TRUE;
      প্রত্যাগম CURLE_OK;
    }
    failf(data, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          stream->id, stream->reset_by_server ? "server" : "curl",
          stream->error, nghttp2_http2_strerror(stream->error));
    প্রত্যাগম stream->error ? CURLE_HTTP2_STREAM :
           (data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2);
  }
  অন্যথা যদি(!stream->bodystarted) {
    failf(data, "HTTP/2 stream %d was closed cleanly, but before getting "
          "all response header fields, treated as error", stream->id);
    প্রত্যাগম CURLE_HTTP2_STREAM;
  }

  যদি(Curl_dynhds_গুনতি(&stream->resp_trailers)) {
    সংরচনা dynhds_entry *e;
    সংরচনা dynbuf dbuf;
    size_t i;

    result = CURLE_OK;
    curlx_dyn_init(&dbuf, DYN_TRAILERS);
    প্রতি(i = 0; i < Curl_dynhds_গুনতি(&stream->resp_trailers); ++i) {
      e = Curl_dynhds_getn(&stream->resp_trailers, i);
      যদি(!e)
        ভঙ্গ;
      curlx_dyn_reset(&dbuf);
      result = curlx_dyn_addf(&dbuf, "%.*s: %.*s\x0d\x0a",
                              (পূর্ণাঙ্ক)e->namelen, e->name,
                              (পূর্ণাঙ্ক)e->valuelen, e->মান);
      যদি(result)
        ভঙ্গ;
      Curl_debug(data, CURLINFO_HEADER_IN, curlx_dyn_ptr(&dbuf),
                 curlx_dyn_len(&dbuf));
      result = Curl_client_write(data,
                                 CLIENTWRITE_HEADER | CLIENTWRITE_TRAILER,
                                 curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf));
      যদি(result)
        ভঙ্গ;
      result = Curl_bump_headersize(data, curlx_dyn_len(&dbuf), FALSE);
      যদি(result)
        প্রত্যাগম result;
    }
    curlx_dyn_free(&dbuf);
    যদি(result)
      গচ্ছ out;
  }

  stream->close_handled = TRUE;
  result = CURLE_OK;

out:
  CURL_TRC_CF(data, cf, "handle_stream_close -> %d, %zu", (পূর্ণাঙ্ক)result, *pnlen);
  প্রত্যাগম result;
}

স্থির পূর্ণাঙ্ক sweight_wanted(ধ্রুব সংরচনা Curl_easy *data)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  প্রত্যাগম data->set.weight ?
    data->set.weight : NGHTTP2_DEFAULT_WEIGHT;
}

স্থির পূর্ণাঙ্ক sweight_in_effect(ধ্রুব সংরচনা Curl_easy *data)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  প্রত্যাগম data->state.weight ?
    data->state.weight : NGHTTP2_DEFAULT_WEIGHT;
}

/*
 * h2_pri_spec() fills in the pri_spec struct, used by nghttp2 to send weight
 * and dependency to the peer. It also stores the updated values in the state
 * struct.
 */

স্থির শূন্য h2_pri_spec(সংরচনা Curl_easy *data,
                        nghttp2_priority_spec *pri_spec)
{
  পূর্ণাঙ্ক prio = data->set.weight;
  nghttp2_priority_spec_init(pri_spec, 0, sweight_wanted(data), FALSE);
  data->state.weight = prio;
}

/*
 * Check if there is been an update in the priority /
 * dependency settings and if so it submits a PRIORITY frame with the updated
 * info.
 * Flush any out data pending in the network buffer.
 */
স্থির CURLcode h2_progress_egress(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  পূর্ণাঙ্ক rv = 0;

  যদি(sweight_wanted(data) != sweight_in_effect(data)) {
    সংরচনা h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    যদি(stream && stream->id > 0 &&
       (sweight_wanted(data) != sweight_in_effect(data))) {
      /* send new weight and/or dependency */
      nghttp2_priority_spec pri_spec;

      h2_pri_spec(data, &pri_spec);
      CURL_TRC_CF(data, cf, "[%d] Queuing PRIORITY", stream->id);
      DEBUGASSERT(stream->id != -1);
      rv = Curl_ngh2_submit_priority(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, &pri_spec);
      যদি(rv)
        গচ্ছ out;
    }
  }

  ctx->nw_out_blocked = 0;
  যাবত্(!rv && !ctx->nw_out_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

out:
  যদি(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(data, cf, "nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    প্রত্যাগম CURLE_SEND_ERROR;
  }
  /* Defer flushing during the connect phase so that the SETTINGS and
   * other initial frames are sent together with the first request.
   * Unless we are 'connect_only' where the request will never come. */
  যদি(!cf->connected && !cf->conn->bits.connect_only)
    প্রত্যাগম CURLE_OK;
  প্রত্যাগম nw_out_flush(cf, data);
}

স্থির CURLcode stream_recv(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                            সংরচনা h2_stream_ctx *stream,
                            বর্ণ *buf, size_t len, size_t *pnread)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_AGAIN;

  (শূন্য)buf;
  (শূন্য)len;
  *pnread = 0;

  যদি(!stream->xfer_result)
    stream->xfer_result = cf_h2_update_local_win(cf, data, stream);

  যদি(stream->xfer_result) {
    CURL_TRC_CF(data, cf, "[%d] xfer write failed", stream->id);
    result = stream->xfer_result;
  }
  অন্যথা যদি(stream->closed) {
    CURL_TRC_CF(data, cf, "[%d] returning CLOSE", stream->id);
    result = http2_handle_stream_close(cf, data, stream, pnread);
  }
  অন্যথা যদি(stream->reset ||
          (ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) ||
          (ctx->rcvd_goaway && ctx->remote_max_sid < stream->id)) {
    CURL_TRC_CF(data, cf, "[%d] returning ERR", stream->id);
    result = data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2;
  }

  যদি(result && (result != CURLE_AGAIN))
    CURL_TRC_CF(data, cf, "[%d] stream_recv(len=%zu) -> %d, %zu",
                stream->id, len, (পূর্ণাঙ্ক)result, *pnread);
  প্রত্যাগম result;
}

স্থির CURLcode h2_progress_ingress(সংরচনা Curl_cfilter *cf,
                                    সংরচনা Curl_easy *data,
                                    size_t data_max_bytes)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  যদি(should_close_session(ctx)) {
    CURL_TRC_CF(data, cf, "[0] ingress: session is closed");
    প্রত্যাগম CURLE_HTTP2;
  }

  /* Process network input buffer first */
  যদি(!Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "Process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    result = h2_process_pending_প্রবেশ(cf, data);
    যদি(result)
      প্রত্যাগম result;
  }

  যদি(!data_max_bytes)
    data_max_bytes = H2_CHUNK_SIZE;

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop due to connection close or us not processing
   * all network input */
  যাবত্(!ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) {
    সংরচনা h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    যদি(stream && (stream->closed || !data_max_bytes)) {
      /* We would like to abort here and stop processing, so that the transfer
       * loop can handle the data/close here. This may leave data in
       * underlying buffers that will not be consumed. */
      যদি(!cf->next || !cf->next->cft->has_data_pending(cf->next, data))
        Curl_multi_mark_dirty(data);
      ভঙ্গ;
    }

    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    যদি(result) {
      যদি(result != CURLE_AGAIN) {
        failf(data, "Failed receiving HTTP2 data: %d(%s)", (পূর্ণাঙ্ক)result,
              curl_easy_strerror(result));
        প্রত্যাগম result;
      }
      ভঙ্গ;
    }
    অন্যথা যদি(nread == 0) {
      CURL_TRC_CF(data, cf, "[0] ingress: connection closed");
      ctx->conn_closed = TRUE;
      ভঙ্গ;
    }
    অন্যথা {
      CURL_TRC_CF(data, cf, "[0] ingress: read %zu bytes", nread);
      data_max_bytes = (data_max_bytes > nread) ? (data_max_bytes - nread) : 0;
    }

    result = h2_process_pending_প্রবেশ(cf, data);
    যদি(result)
      প্রত্যাগম result;
    CURL_TRC_CF(data, cf, "[0] ingress: nw-in buffered %zu",
                Curl_bufq_len(&ctx->inbufq));
  }

  যদি(ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "server closed %s",
                ctx->rcvd_goaway ? "with GOAWAY" : "abruptly");
    connclose(cf->conn);
  }

  CURL_TRC_CF(data, cf, "[0] ingress: done");
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode cf_h2_recv(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                           বর্ণ *buf, size_t len, size_t *pnread)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  সংরচনা h2_stream_ctx *stream;
  CURLcode result, r2;
  সংরচনা cf_call_data save;

  যদি(!data)
    প্রত্যাগম CURLE_HTTP2;

  stream = H2_STREAM_CTX(ctx, data);

  *pnread = 0;
  যদি(!stream) {
    /* Abnormal call sequence: either this transfer has never opened a stream
     * (unlikely) or the transfer has been done, cleaned up its resources, but
     * a read() is called anyway. It is not clear what the calling sequence
     * is for such a case. */
    failf(data, "http/2 recv on a transfer never opened "
          "or already cleared, mid=%u", data->mid);
    প্রত্যাগম CURLE_HTTP2;
  }

  CF_DATA_SAVE(save, cf, data);

  result = stream_recv(cf, data, stream, buf, len, pnread);
  যদি(result && (result != CURLE_AGAIN))
    গচ্ছ out;

  যদি(result) {
    result = h2_progress_ingress(cf, data, len);
    যদি(result)
      গচ্ছ out;

    result = stream_recv(cf, data, stream, buf, len, pnread);
  }

  যদি(*pnread > 0) {
    /* Now that we transferred this to the upper layer, we report
     * the actual amount of DATA consumed to the H2 session, so
     * that it adjusts stream flow control */
    Curl_ngh2_session_consume(&ctx->ngh2, stream->id, *pnread);
    যদি(stream->closed) {
      CURL_TRC_CF(data, cf, "[%d] DRAIN closed stream", stream->id);
      Curl_multi_mark_dirty(data);
    }
  }

out:
  r2 = h2_progress_egress(cf, data);
  যদি(r2 == CURLE_AGAIN) {
    /* pending data to send, need to be called again. Ideally, we
     * monitor the socket for POLLOUT, but when not SENDING
     * any more, we force processing of the transfer. */
    যদি(!CURL_REQ_WANT_SEND(data))
      Curl_multi_mark_dirty(data);
  }
  অন্যথা যদি(r2 && (result == CURLE_AGAIN)) {
    /* Do not replace a previous error result NOR a successful result
     * with an egress error. Example: the server might close the
     * connection after sending the complete response with EOS
     * and everything. Further sending may fail, but that MUST
     * not change that we had a successful response. */
    result = r2;
  }

  CURL_TRC_CF(data, cf, "[%d] cf_recv(len=%zu) -> %d, %zu, "
              "window=%d/%d, connection %d/%d",
              stream->id, len, (পূর্ণাঙ্ক)result, *pnread,
              Curl_ngh2_session_get_stream_effective_recv_data_length(
                &ctx->ngh2, stream->id),
              Curl_ngh2_session_get_stream_effective_local_window_size(
                &ctx->ngh2, stream->id),
              Curl_ngh2_session_get_local_window_size(&ctx->ngh2),
              HTTP2_HUGE_WINDOW_SIZE);

  CF_DATA_RESTORE(cf, save);
  প্রত্যাগম result;
}

স্থির CURLcode cf_h2_body_send(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data,
                                সংরচনা h2_stream_ctx *stream,
                                ধ্রুব শূন্য *buf, size_t blen, bool eos,
                                size_t *pnwritten)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  CURLcode result;

  *pnwritten = 0;
  যদি(stream->closed) {
    যদি(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(data, cf, "[%d] discarding data"
                  "on closed stream with response", stream->id);
      যদি(eos)
        stream->body_eos = TRUE;
      *pnwritten = blen;
      প্রত্যাগম CURLE_OK;
    }
    /* Server closed before we got a response, this is an error */
    infof(data, "stream %d closed", stream->id);
    প্রত্যাগম CURLE_SEND_ERROR;
  }

  result = Curl_bufq_write(&stream->sendbuf, buf, blen, pnwritten);
  যদি(result)
    প্রত্যাগম result;

  যদি(eos && (blen == *pnwritten))
    stream->body_eos = TRUE;

  যদি(eos || !Curl_bufq_is_empty(&stream->sendbuf)) {
    /* resume the potentially suspended stream */
    যদি(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id)))
      প্রত্যাগম CURLE_SEND_ERROR;
  }

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode h2_submit(সংরচনা h2_stream_ctx **pstream,
                          সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                          ধ্রুব uint8_t *buf, size_t len,
                          bool eos, size_t *pnwritten)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  সংরচনা h2_stream_ctx *stream = রিক্ত;
  সংরচনা dynhds h2_headers;
  nghttp2_nv *nva = রিক্ত;
  ধ্রুব শূন্য *body = রিক্ত;
  size_t nheader, bodylen;
  nghttp2_data_provider data_prd;
  int32_t stream_id;
  nghttp2_priority_spec pri_spec;
  size_t nwritten;
  CURLcode result = CURLE_OK;
  uint32_t initial_win_size;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_headers, MAX_HTTP_RESP_HEADER_গুনতি, DYN_HTTP_REQUEST);

  result = http2_data_setup(cf, data, &stream);
  যদি(result)
    গচ্ছ out;

  result = Curl_h1_req_parse_read(
    &stream->h1, buf, len, রিক্ত,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : রিক্ত,
    0, &nwritten);
  যদি(result)
    গচ্ছ out;
  *pnwritten = nwritten;
  যদি(!stream->h1.done) {
    /* need more data */
    গচ্ছ out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  যদি(result)
    গচ্ছ out;
  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nva = Curl_dynhds_to_nva(&h2_headers, &nheader);
  যদি(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }

  h2_pri_spec(data, &pri_spec);
  যদি(!Curl_ngh2_session_check_request_allowed(&ctx->ngh2))
    CURL_TRC_CF(data, cf, "send request NOT allowed (via nghttp2)");

  /* Check the initial windows size of the transfer (rate-limits?) and
   * send an updated settings on changes from previous value. */
  initial_win_size = cf_h2_initial_win_size(data);
  যদি(initial_win_size != ctx->initial_win_size) {
    result = cf_h2_update_settings(ctx, initial_win_size);
    যদি(result)
      গচ্ছ out;
  }

  বিকল্প(data->state.httpreq) {
  পক্ষ HTTPREQ_POST:
  পক্ষ HTTPREQ_POST_FORM:
  পক্ষ HTTPREQ_POST_MIME:
  পক্ষ HTTPREQ_PUT:
    data_prd.read_callback = req_body_read_callback;
    data_prd.source.ptr = রিক্ত;
    stream_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, &data_prd, stream);
    ভঙ্গ;
  স্বভাব:
    stream_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, রিক্ত, stream);
  }

  যদি(stream_id < 0) {
    CURL_TRC_CF(data, cf, "send: nghttp2_submit_request error (%s)%d",
                nghttp2_strerror(stream_id), stream_id);
    result = CURLE_SEND_ERROR;
    গচ্ছ out;
  }

#ifdef CURLVERBOSE
#define MAX_ACC 60000  /* <64KB to account for some overhead */
  যদি(Curl_trc_is_verbose(data)) {
    size_t acc = 0, i;

    infof(data, "[HTTP/2] [%d] OPENED stream for %s",
          stream_id, Curl_bufref_ptr(&data->state.url));
    প্রতি(i = 0; i < nheader; ++i) {
      acc += nva[i].namelen + nva[i].valuelen;

      infof(data, "[HTTP/2] [%d] [%.*s: %.*s]", stream_id,
            (পূর্ণাঙ্ক)nva[i].namelen, nva[i].name,
            (পূর্ণাঙ্ক)nva[i].valuelen, nva[i].মান);
    }

    যদি(acc > MAX_ACC) {
      infof(data, "[HTTP/2] Warning: The cumulative length of all "
            "headers exceeds %d bytes and that could cause the "
            "stream to be rejected.", MAX_ACC);
    }
  }
#endif

  stream->id = stream_id;

  body = (ধ্রুব বর্ণ *)buf + *pnwritten;
  bodylen = len - *pnwritten;

  যদি(bodylen || eos) {
    size_t n;
    result = cf_h2_body_send(cf, data, stream, body, bodylen, eos, &n);
    যদি(!result)
      *pnwritten += n;
    অন্যথা যদি(result == CURLE_AGAIN)
      result = CURLE_OK;
    অন্যথা {
      result = CURLE_SEND_ERROR;
    }
  }

out:
  CURL_TRC_CF(data, cf, "[%d] submit -> %d, %zu",
              stream ? stream->id : -1, (পূর্ণাঙ্ক)result, *pnwritten);
  curlx_safefree(nva);
  *pstream = stream;
  Curl_dynhds_free(&h2_headers);
  প্রত্যাগম result;
}

স্থির CURLcode cf_h2_send(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                           ধ্রুব uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  সংরচনা h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
  সংরচনা cf_call_data save;
  CURLcode result = CURLE_OK, r2;

  CF_DATA_SAVE(save, cf, data);
  *pnwritten = 0;

  যদি(!stream || stream->id == -1) {
    result = h2_submit(&stream, cf, data, buf, len, eos, pnwritten);
    যদি(result)
      গচ্ছ out;
    DEBUGASSERT(stream);
  }
  অন্যথা যদি(stream->body_eos) {
    /* We already wrote this, but CURLE_AGAIN-ed the call due to not
     * being able to flush stream->sendbuf. Make a 0-length write
     * to trigger flushing again.
     * If this works, we report to have written `len` bytes. */
    size_t n;
    DEBUGASSERT(eos);
    result = cf_h2_body_send(cf, data, stream, buf, 0, eos, &n);
    CURL_TRC_CF(data, cf, "[%d] cf_body_send last CHUNK -> %d, %zu, eos=%d",
                stream->id, (পূর্ণাঙ্ক)result, n, eos);
    যদি(result)
      গচ্ছ out;
    *pnwritten = len;
  }
  অন্যথা {
    result = cf_h2_body_send(cf, data, stream, buf, len, eos, pnwritten);
    CURL_TRC_CF(data, cf, "[%d] cf_body_send(len=%zu) -> %d, %zu, eos=%d",
                stream->id, len, (পূর্ণাঙ্ক)result, *pnwritten, eos);
  }

  /* Call the nghttp2 send loop and flush to write ALL buffered data,
   * headers and/or request body completely out to the network */
  r2 = h2_progress_egress(cf, data);

  /* if the stream has been closed in egress handling (nghttp2 does that
   * when it does not like the headers, for example */
  যদি(stream && stream->closed && !stream->resp_hds_complete) {
    infof(data, "stream %d closed", stream->id);
    result = CURLE_SEND_ERROR;
    গচ্ছ out;
  }
  অন্যথা যদি(r2 && (r2 != CURLE_AGAIN)) {
    result = r2;
    গচ্ছ out;
  }

  যদি(should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    যদি(stream && stream->closed) {
      result = http2_handle_stream_close(cf, data, stream, pnwritten);
    }
    অন্যথা {
      CURL_TRC_CF(data, cf, "send: nothing to do in this session");
      result = CURLE_HTTP2;
    }
  }

out:
  যদি(stream) {
    CURL_TRC_CF(data, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
                "eos=%d, h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                stream->id, len, (পূর্ণাঙ্ক)result, *pnwritten,
                stream->body_eos,
                Curl_ngh2_session_get_stream_remote_window_size(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&stream->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  অন্যথা {
    CURL_TRC_CF(data, cf, "cf_send(len=%zu) -> %d, %zu, "
                "connection-window=%d, nw_send_buffer(%zu)",
                len, (পূর্ণাঙ্ক)result, *pnwritten,
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_DATA_RESTORE(cf, save);
  প্রত্যাগম result;
}

স্থির CURLcode cf_h2_flush(সংরচনা Curl_cfilter *cf,
                            সংরচনা Curl_easy *data)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  সংরচনা h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
  সংরচনা cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  যদি(stream && !Curl_bufq_is_empty(&stream->sendbuf)) {
    /* resume the potentially suspended stream */
    যদি(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id))) {
      result = CURLE_SEND_ERROR;
      গচ্ছ out;
    }
  }

  result = h2_progress_egress(cf, data);

out:
  যদি(stream) {
    CURL_TRC_CF(data, cf, "[%d] flush -> %d, "
                "h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                stream->id, (পূর্ণাঙ্ক)result,
                Curl_ngh2_session_get_stream_remote_window_size(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&stream->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  অন্যথা {
    CURL_TRC_CF(data, cf, "flush -> %d, "
                "connection-window=%d, nw_send_buffer(%zu)", (পূর্ণাঙ্ক)result,
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_DATA_RESTORE(cf, save);
  প্রত্যাগম result;
}

স্থির CURLcode cf_h2_adjust_pollset(সংরচনা Curl_cfilter *cf,
                                     সংরচনা Curl_easy *data,
                                     সংরচনা easy_pollset *ps)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  সংরচনা cf_call_data save;
  curl_socket_t sock;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  যদি(!Curl_ngh2_valid(&ctx->ngh2))
    প্রত্যাগম CURLE_OK;

  sock = Curl_conn_cf_get_socket(cf, data);
  Curl_pollset_check(data, ps, sock, &want_recv, &want_send);
  যদি(want_recv || want_send) {
    সংরচনা h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
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
  অন্যথা যদি(ctx->sent_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_DATA_SAVE(save, cf, data);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CF_DATA_RESTORE(cf, save);
  }
  প্রত্যাগম result;
}

স্থির CURLcode cf_h2_ctx_open(সংরচনা Curl_cfilter *cf,
                               সংরচনা Curl_easy *data)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  সংরচনা h2_stream_ctx *stream;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  পূর্ণাঙ্ক rc;
  nghttp2_session_callbacks *cbs = রিক্ত;

  DEBUGASSERT(!ctx->ngh2.s);
  DEBUGASSERT(!ctx->ngh2.fatal_rv);
  DEBUGASSERT(ctx->initialized);

  rc = nghttp2_session_callbacks_new(&cbs);
  যদি(rc) {
    failf(data, "Could not initialize nghttp2 callbacks");
    গচ্ছ out;
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
  যদি(rc) {
    failf(data, "Could not initialize nghttp2");
    গচ্ছ out;
  }
  ctx->max_concurrent_streams = data->multi ?
    Curl_multi_max_concurrent_streams(data->multi) :
    DEFAULT_MAX_CONCURRENT_STREAMS;

  যদি(ctx->via_h1_upgrade) {
    /* HTTP/1.1 Upgrade issued. H2 Settings have already been submitted
     * in the H1 request and we upgrade from there. This stream
     * is opened implicitly as #1. */
    uint8_t binsettings[H2_BINSETTINGS_LEN];
    ssize_t rclen;
    size_t binlen; /* length of the binsettings data */

    rclen = populate_binsettings(binsettings, data);

    যদি(!curlx_sztouz(rclen, &binlen) || !binlen) {
      failf(data, "nghttp2 unexpectedly failed on pack_settings_payload");
      result = CURLE_FAILED_INIT;
      গচ্ছ out;
    }

    result = http2_data_setup(cf, data, &stream);
    যদি(result)
      গচ্ছ out;
    DEBUGASSERT(stream);
    stream->id = 1;
    /* queue SETTINGS frame (again) */
    rc = Curl_ngh2_session_upgrade2(
      &ctx->ngh2, binsettings, binlen, data->state.httpreq == HTTPREQ_HEAD,
      রিক্ত);
    যদি(rc) {
      failf(data, "nghttp2_session_upgrade2() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      গচ্ছ out;
    }

    rc = Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, stream->id, stream);
    যদি(rc) {
      infof(data, "http/2: failed to set user_data for stream %d",
            stream->id);
      DEBUGASSERT(0);
    }
    CURL_TRC_CF(data, cf, "created session via Upgrade");
  }
  অন্যথা {
    nghttp2_settings_entry iv[H2_SETTINGS_IV_LEN];
    size_t ivlen;

    ivlen = populate_settings(iv, data, ctx);
    rc = Curl_ngh2_submit_settings(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, iv, ivlen);
    যদি(rc) {
      failf(data, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      গচ্ছ out;
    }
  }

  rc = Curl_ngh2_session_set_local_window_size(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, HTTP2_HUGE_WINDOW_SIZE);
  যদি(rc) {
    failf(data, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    result = CURLE_HTTP2;
    গচ্ছ out;
  }

  /* all set, traffic will be send on connect */
  result = CURLE_OK;
  CURL_TRC_CF(data, cf, "[0] created h2 session%s",
              ctx->via_h1_upgrade ? " (via h1 upgrade)" : "");

out:
  যদি(cbs)
    nghttp2_session_callbacks_del(cbs);
  প্রত্যাগম result;
}

স্থির CURLcode cf_h2_connect(সংরচনা Curl_cfilter *cf,
                              সংরচনা Curl_easy *data,
                              bool *done)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  সংরচনা cf_call_data save;
  bool first_time = FALSE;

  যদি(cf->connected) {
    *done = TRUE;
    প্রত্যাগম CURLE_OK;
  }

  /* Connect the lower filters first */
  যদি(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    যদি(result || !*done)
      প্রত্যাগম result;
  }

  *done = FALSE;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(ctx->initialized);
  যদি(!ctx->ngh2.s) {
    result = cf_h2_ctx_open(cf, data);
    যদি(result)
      গচ্ছ out;
    first_time = TRUE;
  }

  যদি(!first_time) {
    result = h2_progress_ingress(cf, data, 0);
    যদি(result)
      গচ্ছ out;
  }

  /* Send out our SETTINGS and ACKs and such. If that blocks, we
   * have it buffered and can count this filter as being connected */
  result = h2_progress_egress(cf, data);
  যদি(result && (result != CURLE_AGAIN))
    গচ্ছ out;

  *done = TRUE;
  cf->connected = TRUE;
  result = CURLE_OK;

out:
  CURL_TRC_CF(data, cf, "cf_connect() -> %d, %d, ", (পূর্ণাঙ্ক)result, *done);
  CF_DATA_RESTORE(cf, save);
  প্রত্যাগম result;
}

স্থির শূন্য cf_h2_destroy(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;

  (শূন্য)data;
  যদি(ctx) {
    cf_h2_ctx_free(ctx);
    cf->ctx = রিক্ত;
  }
}

স্থির CURLcode cf_h2_shutdown(সংরচনা Curl_cfilter *cf,
                               সংরচনা Curl_easy *data, bool *done)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  সংরচনা cf_call_data save;
  CURLcode result;
  পূর্ণাঙ্ক rv;

  যদি(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *done = TRUE;
    প্রত্যাগম CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);

  যদি(!ctx->sent_goaway) {
    ctx->sent_goaway = TRUE;
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, ctx->local_max_sid, 0,
      (ধ্রুব uint8_t *)"shutdown", পরিমাণ("shutdown"));
    যদি(rv) {
      failf(data, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      result = CURLE_SEND_ERROR;
      গচ্ছ out;
    }
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  result = CURLE_OK;
  যদি(Curl_ngh2_session_want_write(&ctx->ngh2) ||
     !Curl_bufq_is_empty(&ctx->outbufq))
    result = h2_progress_egress(cf, data);
  যদি(!result && Curl_ngh2_session_want_read(&ctx->ngh2))
    result = h2_progress_ingress(cf, data, 0);

  যদি(result == CURLE_AGAIN)
    result = CURLE_OK;

  *done = (ctx->conn_closed ||
           (!result && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2) &&
            Curl_bufq_is_empty(&ctx->outbufq)));

out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  প্রত্যাগম result;
}

স্থির CURLcode http2_data_pause(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 bool pause)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  সংরচনা h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);

  DEBUGASSERT(data);
  যদি(ctx && Curl_ngh2_valid(&ctx->ngh2) && stream) {
    CURLcode result;

    stream->write_paused = pause;
    result = cf_h2_update_local_win(cf, data, stream);
    যদি(result)
      প্রত্যাগম result;

    /* attempt to send the window update */
    (শূন্য)h2_progress_egress(cf, data);

    যদি(!pause) {
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
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode cf_h2_cntrl(সংরচনা Curl_cfilter *cf,
                            সংরচনা Curl_easy *data,
                            পূর্ণাঙ্ক event, পূর্ণাঙ্ক arg1, শূন্য *arg2)
{
  CURLcode result = CURLE_OK;
  সংরচনা cf_call_data save;

  (শূন্য)arg2;

  CF_DATA_SAVE(save, cf, data);
  বিকল্প(event) {
  পক্ষ CF_CTRL_DATA_SETUP:
    ভঙ্গ;
  পক্ষ CF_CTRL_DATA_PAUSE:
    result = http2_data_pause(cf, data, (arg1 != 0));
    ভঙ্গ;
  পক্ষ CF_CTRL_FLUSH:
    result = cf_h2_flush(cf, data);
    ভঙ্গ;
  পক্ষ CF_CTRL_DATA_DONE:
    http2_data_done(cf, data);
    ভঙ্গ;
  পক্ষ CF_CTRL_CONN_INFO_UPDATE:
    যদি(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 20;
      Curl_conn_set_multiplex(cf->conn);
    }
    ভঙ্গ;
  স্বভাব:
    ভঙ্গ;
  }
  CF_DATA_RESTORE(cf, save);
  প্রত্যাগম result;
}

স্থির bool cf_h2_data_pending(সংরচনা Curl_cfilter *cf,
                               ধ্রুব সংরচনা Curl_easy *data)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;

  যদি(ctx && !Curl_bufq_is_empty(&ctx->inbufq))
    প্রত্যাগম TRUE;
  প্রত্যাগম cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

স্থির bool cf_h2_is_alive(সংরচনা Curl_cfilter *cf,
                           সংরচনা Curl_easy *data,
                           bool *প্রবেশ_pending)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  bool alive;
  সংরচনা cf_call_data save;

  *প্রবেশ_pending = FALSE;
  CF_DATA_SAVE(save, cf, data);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
           http2_connisalive(cf, data, প্রবেশ_pending));
  CURL_TRC_CF(data, cf, "conn alive -> %d, input_pending=%d",
              alive, *প্রবেশ_pending);
  CF_DATA_RESTORE(cf, save);
  প্রত্যাগম alive;
}

স্থির CURLcode cf_h2_keep_alive(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data)
{
  CURLcode result;
  সংরচনা cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  result = http2_send_ping(cf, data);
  CF_DATA_RESTORE(cf, save);
  প্রত্যাগম result;
}

স্থির CURLcode cf_h2_query(সংরচনা Curl_cfilter *cf,
                            সংরচনা Curl_easy *data,
                            পূর্ণাঙ্ক query, পূর্ণাঙ্ক *pres1, শূন্য *pres2)
{
  সংরচনা cf_h2_ctx *ctx = cf->ctx;
  সংরচনা cf_call_data save;
  size_t effective_max;

  বিকল্প(query) {
  পক্ষ CF_QUERY_MAX_CONCURRENT:
    DEBUGASSERT(pres1);

    CF_DATA_SAVE(save, cf, data);
    যদি(!Curl_ngh2_valid(&ctx->ngh2) ||
       !Curl_ngh2_session_check_request_allowed(&ctx->ngh2)) {
      /* the limit is what we have in use right now */
      effective_max = cf->conn->attached_xfers;
    }
    অন্যথা {
      effective_max = ctx->max_concurrent_streams;
    }
    *pres1 = (effective_max > INT_MAX) ? INT_MAX : (পূর্ণাঙ্ক)effective_max;
    CF_DATA_RESTORE(cf, save);
    প্রত্যাগম CURLE_OK;
  পক্ষ CF_QUERY_STREAM_ERROR: {
    সংরচনা h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    *pres1 = stream ? (পূর্ণাঙ্ক)stream->error : 0;
    প্রত্যাগম CURLE_OK;
  }
  পক্ষ CF_QUERY_NEED_FLUSH: {
    সংরচনা h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    যদি(!Curl_bufq_is_empty(&ctx->outbufq) ||
       (stream && !Curl_bufq_is_empty(&stream->sendbuf))) {
      *pres1 = TRUE;
      প্রত্যাগম CURLE_OK;
    }
    ভঙ্গ;
  }
  পক্ষ CF_QUERY_HTTP_VERSION:
    *pres1 = 20;
    প্রত্যাগম CURLE_OK;
  স্বভাব:
    ভঙ্গ;
  }
  প্রত্যাগম cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

সংরচনা Curl_cftype Curl_cft_nghttp2 = {
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

স্থির CURLcode http2_cfilter_add(সংরচনা Curl_cfilter **pcf,
                                  সংরচনা Curl_easy *data,
                                  সংরচনা connectdata *conn,
                                  int8_t sockindex,
                                  bool via_h1_upgrade)
{
  সংরচনা Curl_cfilter *cf = রিক্ত;
  সংরচনা cf_h2_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(data->conn);
  ctx = curlx_calloc(1, পরিমাণ(*ctx));
  যদি(!ctx)
    গচ্ছ out;
  cf_h2_ctx_init(ctx, via_h1_upgrade);

  result = Curl_cf_create(&cf, &Curl_cft_nghttp2, ctx);
  যদি(result)
    গচ্ছ out;

  ctx = রিক্ত;
  Curl_conn_cf_add(data, conn, sockindex, cf);

out:
  যদি(result)
    cf_h2_ctx_free(ctx);
  *pcf = result ? রিক্ত : cf;
  প্রত্যাগম result;
}

স্থির CURLcode http2_cfilter_ঢোকাও_after(সংরচনা Curl_cfilter *cf,
                                           সংরচনা Curl_easy *data,
                                           bool via_h1_upgrade)
{
  সংরচনা Curl_cfilter *cf_h2 = রিক্ত;
  সংরচনা cf_h2_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (শূন্য)data;
  ctx = curlx_calloc(1, পরিমাণ(*ctx));
  যদি(!ctx)
    গচ্ছ out;
  cf_h2_ctx_init(ctx, via_h1_upgrade);

  result = Curl_cf_create(&cf_h2, &Curl_cft_nghttp2, ctx);
  যদি(result)
    গচ্ছ out;

  ctx = রিক্ত;
  Curl_conn_cf_ঢোকাও_after(cf, cf_h2);

out:
  যদি(result)
    cf_h2_ctx_free(ctx);
  প্রত্যাগম result;
}

bool Curl_http2_may_switch(সংরচনা Curl_easy *data)
{
  যদি(Curl_conn_http_version(data, data->conn) < 20 &&
     (data->state.http_neg.wanted & CURL_HTTP_V2x) &&
     data->state.http_neg.h2_prior_knowledge) {
#ifndef CURL_DISABLE_PROXY
    যদি(data->conn->bits.origin_is_proxy) {
      infof(data, "Ignoring HTTP/2 prior knowledge due to proxy");
      প্রত্যাগম FALSE;
    }
#endif
    প্রত্যাগম TRUE;
  }
  প্রত্যাগম FALSE;
}

CURLcode Curl_http2_switch(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_cfilter *cf;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, data->conn) < 20);

  result = http2_cfilter_add(&cf, data, data->conn, FIRSTSOCKET, FALSE);
  যদি(result)
    প্রত্যাগম result;
  CURL_TRC_CF(data, cf, "switching connection to HTTP/2");

  যদি(cf->next) {
    bool done;
    প্রত্যাগম Curl_conn_cf_connect(cf, data, &done);
  }
  প্রত্যাগম CURLE_OK;
}

CURLcode Curl_http2_switch_at(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data)
{
  সংরচনা Curl_cfilter *cf_h2;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, data->conn) < 20);

  result = http2_cfilter_ঢোকাও_after(cf, data, FALSE);
  যদি(result)
    প্রত্যাগম result;

  cf_h2 = cf->next;

  যদি(cf_h2->next) {
    bool done;
    প্রত্যাগম Curl_conn_cf_connect(cf_h2, data, &done);
  }
  প্রত্যাগম CURLE_OK;
}

CURLcode Curl_http2_upgrade(সংরচনা Curl_easy *data,
                            সংরচনা connectdata *conn, int8_t sockindex,
                            ধ্রুব বর্ণ *mem, size_t nread)
{
  সংরচনা Curl_cfilter *cf;
  সংরচনা cf_h2_ctx *ctx;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, conn) < 20);

  result = http2_cfilter_add(&cf, data, conn, sockindex, TRUE);
  যদি(result)
    প্রত্যাগম result;
  CURL_TRC_CF(data, cf, "upgrading connection to HTTP/2");

  DEBUGASSERT(cf->cft == &Curl_cft_nghttp2);
  ctx = cf->ctx;

  data->req.httpversion_sent = 20; /* it is an h2 request now */
  data->req.header = TRUE; /* we expect the real response to come in h2 */
  data->req.headerline = 0; /* restart the header line counter */

  যদি(nread > 0) {
    /* Remaining data from the protocol switch reply is already using
     * the switched protocol, ie. HTTP/2. We add that to the network
     * inbufq. */
    size_t copied;

    result = Curl_bufq_write(&ctx->inbufq,
                             (ধ্রুব unsigned বর্ণ *)mem, nread, &copied);
    যদি(result) {
      failf(data, "error on copying HTTP Upgrade response: %d", (পূর্ণাঙ্ক)result);
      প্রত্যাগম CURLE_RECV_ERROR;
    }
    যদি(copied < nread) {
      failf(data, "connection buffer size could not take all data "
            "from HTTP Upgrade response header: copied=%zu, datalen=%zu",
            copied, nread);
      প্রত্যাগম CURLE_HTTP2;
    }
    infof(data, "Copied HTTP/2 data in stream buffer to connection buffer"
          " after upgrade: len=%zu", nread);
  }

  যদি(cf->next) {
    bool done;
    result = Curl_conn_cf_connect(cf, data, &done);
    যদি(!result)
      cf->cft->cntrl(cf, data, CF_CTRL_CONN_INFO_UPDATE, 0, রিক্ত);
  }
  প্রত্যাগম result;
}

/* Only call this function for a transfer that already got an HTTP/2
   CURLE_HTTP2_STREAM error! */
bool Curl_h2_http_1_1_error(সংরচনা Curl_easy *data)
{
  যদি(Curl_conn_http_version(data, data->conn) == 20) {
    পূর্ণাঙ্ক err = Curl_conn_get_stream_error(data, data->conn, FIRSTSOCKET);
    প্রত্যাগম err == NGHTTP2_HTTP_1_1_REQUIRED;
  }
  প্রত্যাগম FALSE;
}

শূন্য *Curl_nghttp2_malloc(size_t size, শূন্য *user_data)
{
  (শূন্য)user_data;
  প্রত্যাগম Curl_cmalloc(size);
}

শূন্য Curl_nghttp2_free(শূন্য *ptr, শূন্য *user_data)
{
  (শূন্য)user_data;
  Curl_cfree(ptr);
}

শূন্য *Curl_nghttp2_calloc(size_t nmemb, size_t size, শূন্য *user_data)
{
  (শূন্য)user_data;
  প্রত্যাগম Curl_ccalloc(nmemb, size);
}

শূন্য *Curl_nghttp2_realloc(শূন্য *ptr, size_t size, শূন্য *user_data)
{
  (শূন্য)user_data;
  প্রত্যাগম Curl_crealloc(ptr, size);
}

#অন্যথা /* CURL_DISABLE_HTTP || !USE_NGHTTP2 */

/* Satisfy external references even if http2 is not compiled in. */

বর্ণ *curl_pushheader_bynum(সংরচনা curl_pushheaders *h, size_t অঙ্ক)
{
  (শূন্য)h;
  (শূন্য)অঙ্ক;
  প্রত্যাগম রিক্ত;
}

বর্ণ *curl_pushheader_byname(সংরচনা curl_pushheaders *h, ধ্রুব বর্ণ *name)
{
  (শূন্য)h;
  (শূন্য)name;
  প্রত্যাগম রিক্ত;
}

#endif /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
