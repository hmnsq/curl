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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#അന്തര്ഭാവ <nghttp2/nghttp2.h>

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "bufq.h"
#അന്തര്ഭാവ "u32_ptrset.h"
#അന്തര്ഭാവ "http1.h"
#അന്തര്ഭാവ "http2.h"
#അന്തര്ഭാവ "http.h"
#അന്തര്ഭാവ "sendf.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "curlx/base64.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "ngh2_shim.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "urlapi-int.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "bufref.h"
#അന്തര്ഭാവ "curlx/dynbuf.h"
#അന്തര്ഭാവ "headers.h"
#അന്തര്ഭാവ "curl_share.h"

#എങ്കിൽ NGHTTP2_VERSION_അക്കം < 0x010f00
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
#എങ്കിൽ NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
#define H2_STREAM_WINDOW_SIZE_INITIAL  (64 * 1024)
#അല്ലെങ്കിൽ
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

ഘടന cf_h2_ctx {
  ഘടന curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  ഘടന cf_call_data call_data;

  ഘടന bufq inbufq;           /* network input */
  ഘടന bufq outbufq;          /* network output */
  ഘടന bufc_pool stream_bufcp; /* spares for stream buffers */
  ഘടന dynbuf scratch;        /* scratch buffer for temp use */

  ഘടന u32_ptrset streams; /* hash of `data->mid` to `h2_stream_ctx` */
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
  BIT(enable_തള്ളുക);
  BIT(nw_out_blocked);
};

/* How to access `call_data` from a cf_h2 filter */
#undef CF_CTX_CALL_DATA
#define CF_CTX_CALL_DATA(cf) ((ഘടന cf_h2_ctx *)(cf)->ctx)->call_data

/**
 * All about the H2 internals of a stream
 */
ഘടന h2_stream_ctx {
  ഘടന bufq sendbuf; /* request buffer */
  ഘടന h1_req_parser h1; /* parsing the request */
  ഘടന dynhds resp_trailers; /* response trailer fields */
  size_t resp_hds_len; /* amount of response header bytes in recvbuf */
  curl_off_t xfer_id;  /* transfer `id` owning this stream */
  curl_off_t nrcvd_data;  /* number of DATA bytes received */

  അക്ഷരം **തള്ളുക_headers;       /* allocated array */
  size_t തള്ളുക_headers_used;  /* number of entries filled in */
  size_t തള്ളുക_headers_alloc; /* number of entries allocated */

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

സ്ഥിതം ശൂന്യം free_തള്ളുക_headers(ഘടന h2_stream_ctx *stream)
{
  size_t i;
  വേണ്ടി(i = 0; i < stream->തള്ളുക_headers_used; i++)
    curlx_free(stream->തള്ളുക_headers[i]);
  curlx_safefree(stream->തള്ളുക_headers);
  stream->തള്ളുക_headers_used = 0;
}

സ്ഥിതം ശൂന്യം h2_stream_ctx_free(ഘടന h2_stream_ctx *stream)
{
  Curl_bufq_free(&stream->sendbuf);
  Curl_h1_req_parse_free(&stream->h1);
  Curl_dynhds_free(&stream->resp_trailers);
  free_തള്ളുക_headers(stream);
  curlx_free(stream);
}

സ്ഥിതം ശൂന്യം h2_stream_hash_free(uint32_t id, ശൂന്യം *stream)
{
  (ശൂന്യം)id;
  DEBUGASSERT(stream);
  h2_stream_ctx_free((ഘടന h2_stream_ctx *)stream);
}

സ്ഥിതം ശൂന്യം cf_h2_ctx_init(ഘടന cf_h2_ctx *ctx, bool via_h1_upgrade)
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

സ്ഥിതം ശൂന്യം cf_h2_ctx_free(ഘടന cf_h2_ctx *ctx)
{
  എങ്കിൽ(ctx && ctx->initialized) {
    Curl_ngh2_del(&ctx->ngh2);
    Curl_bufq_free(&ctx->inbufq);
    Curl_bufq_free(&ctx->outbufq);
    Curl_bufcp_free(&ctx->stream_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    memset(ctx, 0, വലുപ്പം(*ctx));
  }
  curlx_free(ctx);
}

സ്ഥിതം uint32_t cf_h2_initial_win_size(ഘടന Curl_easy *data)
{
#എങ്കിൽ NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
  /* If the transfer has a rate-limit lower than the default initial
   * stream window size, use that. It needs to be at least 8k or servers
   * may be unhappy. */
  curl_off_t rps = Curl_rlimit_per_step(&data->progress.dl.rlimit);
  എങ്കിൽ((rps > 0) && (rps < H2_STREAM_WINDOW_SIZE_INITIAL))
    തിരികെ CURLMAX((uint32_t)rps, 8192);
#endif
  തിരികെ H2_STREAM_WINDOW_SIZE_INITIAL;
}

സ്ഥിതം size_t populate_settings(nghttp2_settings_entry *iv,
                                ഘടന Curl_easy *data,
                                ഘടന cf_h2_ctx *ctx)
{
  iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
  iv[0].മൂല്യം = Curl_multi_max_concurrent_streams(data->multi);

  iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
  iv[1].മൂല്യം = cf_h2_initial_win_size(data);
  എങ്കിൽ(ctx)
    ctx->initial_win_size = iv[1].മൂല്യം;
  iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_തള്ളുക;
  iv[2].മൂല്യം = !!data->multi->തള്ളുക_cb;

  തിരികെ 3;
}

സ്ഥിതം ssize_t populate_binsettings(uint8_t *binsettings,
                                    ഘടന Curl_easy *data)
{
  nghttp2_settings_entry iv[H2_SETTINGS_IV_LEN];
  size_t ivlen;

  ivlen = populate_settings(iv, data, ശൂന്യ);
  /* this returns number of bytes it wrote or a negative number on error. */
  തിരികെ nghttp2_pack_settings_payload(binsettings, H2_BINSETTINGS_LEN,
                                       iv, ivlen);
}

സ്ഥിതം CURLcode cf_h2_update_settings(ഘടന cf_h2_ctx *ctx,
                                      uint32_t initial_win_size)
{
  nghttp2_settings_entry entry;
  entry.settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
  entry.മൂല്യം = initial_win_size;
  എങ്കിൽ(Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, &entry, 1))
    തിരികെ CURLE_SEND_ERROR;
  ctx->initial_win_size = initial_win_size;
  തിരികെ CURLE_OK;
}

#define H2_STREAM_CTX(ctx, data)                                         \
  ((ഘടന h2_stream_ctx *)(                                             \
    (data) ? Curl_u32_ptrset_get(&(ctx)->streams, (data)->mid) : ശൂന്യ))

സ്ഥിതം ഘടന h2_stream_ctx *h2_stream_ctx_create(ഘടന cf_h2_ctx *ctx,
                                                  ഘടന Curl_easy *data)
{
  ഘടന h2_stream_ctx *stream;

  stream = curlx_calloc(1, വലുപ്പം(*stream));
  എങ്കിൽ(!stream)
    തിരികെ ശൂന്യ;

  stream->id = -1;
  stream->xfer_id = data->id;
  stream->mid = data->mid;
  Curl_bufq_initp(&stream->sendbuf, &ctx->stream_bufcp,
                  H2_STREAM_SEND_CHUNKS, BUFQ_OPT_NONE);
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_വരി_LEN);
  Curl_dynhds_init(&stream->resp_trailers,
                   MAX_HTTP_RESP_HEADER_എണ്ണം, DYN_HTTP_REQUEST);
  stream->bodystarted = FALSE;
  stream->status_code = -1;
  stream->closed = FALSE;
  stream->close_handled = FALSE;
  stream->error = NGHTTP2_NO_ERROR;
  stream->local_window_size = H2_STREAM_WINDOW_SIZE_INITIAL;
  stream->nrcvd_data = 0;
  തിരികെ stream;
}

സ്ഥിതം ഘടന Curl_easy *cf_h2_get_stream_xfer(ഘടന Curl_cfilter *cf,
                                               int32_t stream_id,
                                               ഘടന h2_stream_ctx **pstream)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  ഘടന h2_stream_ctx *stream =
    Curl_ngh2_session_get_stream_user_data(&ctx->ngh2, stream_id);
  എങ്കിൽ(pstream)
    *pstream = stream;
  എങ്കിൽ(stream && (stream->mid != UINT32_MAX)) {
    ഘടന Curl_easy *data, *call_data = CF_DATA_CURRENT(cf);
    എങ്കിൽ(!call_data || !call_data->multi) {
      DEBUGASSERT(0);
      തിരികെ ശൂന്യ;
    }
    data = Curl_multi_get_easy(call_data->multi, stream->mid);
    എങ്കിൽ(data && (data->id == stream->xfer_id))
      തിരികെ data;
  }
  തിരികെ ശൂന്യ;
}

#ifdef NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
സ്ഥിതം int32_t cf_h2_get_desired_local_win(ഘടന Curl_cfilter *cf,
                                           ഘടന Curl_easy *data)
{
  curl_off_t avail = Curl_rlimit_avail(&data->progress.dl.rlimit, ശൂന്യ);

  (ശൂന്യം)cf;
  എങ്കിൽ(avail < CURL_OFF_T_MAX) { /* limit in place */
    എങ്കിൽ(avail <= 0)
      തിരികെ 0;
    അല്ലെങ്കിൽ എങ്കിൽ(avail < H2_STREAM_WINDOW_SIZE_MAX)
      തിരികെ (int32_t)avail;
  }
  തിരികെ H2_STREAM_WINDOW_SIZE_MAX;
}

സ്ഥിതം CURLcode cf_h2_update_local_win(ഘടന Curl_cfilter *cf,
                                       ഘടന Curl_easy *data,
                                       ഘടന h2_stream_ctx *stream)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  int32_t dwsize;
  പൂർണ്ണസംഖ്യ rv;

  dwsize = (stream->write_paused || stream->xfer_result) ?
           0 : cf_h2_get_desired_local_win(cf, data);
  എങ്കിൽ(dwsize != stream->local_window_size) {
    int32_t wsize = Curl_ngh2_session_get_stream_effective_local_window_size(
      &ctx->ngh2, stream->id);
    എങ്കിൽ(dwsize > wsize) {
      rv = Curl_ngh2_session_set_local_window_size(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize);
      എങ്കിൽ(rv) {
        failf(data, "[%d] nghttp2 set_local_window_size(%d) failed: "
              "%s(%d)", stream->id, dwsize, nghttp2_strerror(rv), rv);
        തിരികെ CURLE_HTTP2;
      }
      rv = Curl_ngh2_submit_window_update(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize - wsize);
      എങ്കിൽ(rv) {
        failf(data, "[%d] nghttp2_submit_window_update() failed: "
              "%s(%d)", stream->id, nghttp2_strerror(rv), rv);
        തിരികെ CURLE_HTTP2;
      }
      stream->local_window_size = dwsize;
      CURL_TRC_CF(data, cf, "[%d] local window update by %d",
                  stream->id, dwsize - wsize);
    }
    അല്ലെങ്കിൽ {
      rv = Curl_ngh2_session_set_local_window_size(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize);
      എങ്കിൽ(rv) {
        failf(data, "[%d] nghttp2_session_set_local_window_size() failed: "
              "%s(%d)", stream->id, nghttp2_strerror(rv), rv);
        തിരികെ CURLE_HTTP2;
      }
      stream->local_window_size = dwsize;
      CURL_TRC_CF(data, cf, "[%d] local window size now %d",
                  stream->id, dwsize);
    }
  }
  തിരികെ CURLE_OK;
}

#അല്ലെങ്കിൽ /* NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

സ്ഥിതം CURLcode cf_h2_update_local_win(ഘടന Curl_cfilter *cf,
                                       ഘടന Curl_easy *data,
                                       ഘടന h2_stream_ctx *stream)
{
  (ശൂന്യം)cf;
  (ശൂന്യം)data;
  (ശൂന്യം)stream;
  തിരികെ CURLE_OK;
}
#endif /* !NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

സ്ഥിതം CURLcode http2_data_setup(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 ഘടന h2_stream_ctx **pstream)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  ഘടന h2_stream_ctx *stream;

  (ശൂന്യം)cf;
  DEBUGASSERT(data);

  എങ്കിൽ(!data)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  stream = H2_STREAM_CTX(ctx, data);
  എങ്കിൽ(stream) {
    *pstream = stream;
    തിരികെ CURLE_OK;
  }

  stream = h2_stream_ctx_create(ctx, data);
  എങ്കിൽ(!stream)
    തിരികെ CURLE_OUT_OF_MEMORY;

  എങ്കിൽ(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    h2_stream_ctx_free(stream);
    തിരികെ CURLE_OUT_OF_MEMORY;
  }

  *pstream = stream;
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode nw_out_flush(ഘടന Curl_cfilter *cf,
                             ഘടന Curl_easy *data)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  എങ്കിൽ(Curl_bufq_is_empty(&ctx->outbufq))
    തിരികെ CURLE_OK;

  result = Curl_cf_send_bufq(cf->next, data, &ctx->outbufq, ശൂന്യ, 0,
                             &nwritten);
  എങ്കിൽ(result) {
    എങ്കിൽ(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_out_blocked = 1;
    }
    തിരികെ result;
  }
  തിരികെ Curl_bufq_is_empty(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

സ്ഥിതം ശൂന്യം http2_data_done(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  ഘടന h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);

  DEBUGASSERT(ctx);
  എങ്കിൽ(!stream || !ctx->initialized)
    തിരികെ;

  എങ്കിൽ(Curl_ngh2_valid(&ctx->ngh2)) {
    bool flush_egress = FALSE;
    /* returns error if stream not known, which is fine here */
    (ശൂന്യം)Curl_ngh2_session_set_stream_user_data(&ctx->ngh2, stream->id, ശൂന്യ);

    എങ്കിൽ(!stream->closed && stream->id > 0) {
      /* RST_STREAM */
      CURL_TRC_CF(data, cf, "[%d] premature DATA_DONE, RST stream",
                  stream->id);
      stream->closed = TRUE;
      stream->reset = TRUE;
      Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                  stream->id, NGHTTP2_STREAM_CLOSED);
      flush_egress = TRUE;
    }

    എങ്കിൽ(flush_egress) {
      (ശൂന്യം)Curl_ngh2_session_send(&ctx->ngh2);
      (ശൂന്യം)nw_out_flush(cf, data);
    }
  }

  Curl_u32_ptrset_unset(&ctx->streams, data->mid);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ h2_client_new(ഘടന Curl_cfilter *cf,
                         nghttp2_session_callbacks *cbs)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { ശൂന്യ, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  പൂർണ്ണസംഖ്യ rc = nghttp2_option_new(&o);
  എങ്കിൽ(rc)
    തിരികെ rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_set_no_auto_window_update(o, 1);
#എങ്കിൽ NGHTTP2_VERSION_അക്കം >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_set_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#endif
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  തിരികെ rc;
}

/*
 * Returns nonzero if current HTTP/2 session should be closed.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ should_close_session(ഘടന cf_h2_ctx *ctx)
{
  തിരികെ ctx->drain_total == 0 && !Curl_ngh2_session_want_read(&ctx->ngh2) &&
    !Curl_ngh2_session_want_write(&ctx->ngh2);
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
സ്ഥിതം CURLcode h2_process_pending_നിവേശം(ഘടന Curl_cfilter *cf,
                                         ഘടന Curl_easy *data)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  സ്ഥിരം unsigned അക്ഷരം *buf;
  size_t blen, nread;
  ssize_t rv;

  ആകുമ്പോൾ(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {
    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (സ്ഥിരം uint8_t *)buf, blen);
    എങ്കിൽ(!curlx_sztouz(rv, &nread)) {
      failf(data, "nghttp2 recv error %zd: %s", rv, nghttp2_strerror((പൂർണ്ണസംഖ്യ)rv));
      തിരികെ CURLE_RECV_ERROR;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    എങ്കിൽ(Curl_bufq_is_empty(&ctx->inbufq)) {
      നിർത്തുക;
    }
    അല്ലെങ്കിൽ {
      CURL_TRC_CF(data, cf, "process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }

  എങ്കിൽ(Curl_ngh2_session_check_request_allowed(&ctx->ngh2) == 0) {
    /* No more requests are allowed in the current session, so
       the connection may not be reused. This is set when a
       GOAWAY frame has been received or when the limit of stream
       identifiers has been reached. */
    CURL_TRC_M(data, "http/2: No new requests allowed");
    connclose(cf->conn);
  }

  തിരികെ CURLE_OK;
}

/*
 * The server may send us data at any point (e.g. PING frames). Therefore, we
 * cannot assume that an HTTP/2 socket is dead because it is readable.
 *
 * Check the lower filters first and, if successful, peek at the socket
 * and distinguish between closed and data.
 */
സ്ഥിതം bool http2_connisalive(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                              bool *നിവേശം_pending)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *നിവേശം_pending = FALSE;
  എങ്കിൽ(!cf->next || !cf->next->cft->is_alive(cf->next, data, നിവേശം_pending))
    തിരികെ FALSE;

  എങ്കിൽ(*നിവേശം_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode result;
    size_t nread;

    *നിവേശം_pending = FALSE;
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(data, cf, "connisalive, recv pending input -> %d, %zu",
                (പൂർണ്ണസംഖ്യ)result, nread);
    എങ്കിൽ(!result) {
      result = h2_process_pending_നിവേശം(cf, data);
      എങ്കിൽ(result)
        /* immediate error, considered dead */
        alive = FALSE;
      അല്ലെങ്കിൽ {
        alive = !should_close_session(ctx);
      }
    }
    അല്ലെങ്കിൽ എങ്കിൽ(result != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  തിരികെ alive;
}

സ്ഥിതം CURLcode http2_send_ping(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  പൂർണ്ണസംഖ്യ rc;

  rc = Curl_ngh2_submit_ping(&ctx->ngh2, 0, ZERO_NULL);
  എങ്കിൽ(rc) {
    failf(data, "nghttp2_submit_ping() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    തിരികെ CURLE_HTTP2;
  }

  rc = Curl_ngh2_session_send(&ctx->ngh2);
  എങ്കിൽ(rc) {
    failf(data, "nghttp2_session_send() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    തിരികെ CURLE_SEND_ERROR;
  }
  തിരികെ CURLE_OK;
}

/*
 * Store nghttp2 version info in this buffer.
 */
ശൂന്യം Curl_http2_ver(അക്ഷരം *p, size_t len)
{
  nghttp2_info *h2 = nghttp2_version(0);
  (ശൂന്യം)curl_msnprintf(p, len, "nghttp2/%s", h2->version_str);
}

/*
 * The implementation of nghttp2_send_callback type. Here we write |data| with
 * size |length| to the network and return the number of bytes actually
 * written. See the documentation of nghttp2_send_callback for the details.
 */
സ്ഥിതം ssize_t send_callback(nghttp2_session *h2,
                             സ്ഥിരം uint8_t *buf, size_t blen, പൂർണ്ണസംഖ്യ flags,
                             ശൂന്യം *userp)
{
  ഘടന Curl_cfilter *cf = userp;
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result = CURLE_OK;

  (ശൂന്യം)h2;
  (ശൂന്യം)flags;
  DEBUGASSERT(data);

  എങ്കിൽ(!cf->connected)
    result = Curl_bufq_write(&ctx->outbufq, buf, blen, &nwritten);
  അല്ലെങ്കിൽ
    result = Curl_cf_send_bufq(cf->next, data, &ctx->outbufq, buf, blen,
                               &nwritten);

  എങ്കിൽ(result) {
    എങ്കിൽ(result == CURLE_AGAIN) {
      ctx->nw_out_blocked = 1;
      തിരികെ NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(data, "Failed sending HTTP2 data");
    തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  എങ്കിൽ(!nwritten) {
    ctx->nw_out_blocked = 1;
    തിരികെ NGHTTP2_ERR_WOULDBLOCK;
  }
  തിരികെ (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

/* We pass a pointer to this struct in the push callback, but the contents of
   the struct are hidden from the user. */
ഘടന curl_pushheaders {
  ഘടന Curl_easy *data;
  ഘടന h2_stream_ctx *stream;
  സ്ഥിരം nghttp2_തള്ളുക_promise *frame;
};

/*
 * push header access function. Only to be used from within the push callback
 */
അക്ഷരം *curl_pushheader_bynum(ഘടന curl_pushheaders *h, size_t അക്കം)
{
  /* Verify that we got a good easy handle in the push header struct, mostly to
     detect rubbish input fast(er). */
  എങ്കിൽ(!h || !GOOD_EASY_HANDLE(h->data))
    തിരികെ ശൂന്യ;
  അല്ലെങ്കിൽ {
    എങ്കിൽ(h->stream && അക്കം < h->stream->തള്ളുക_headers_used)
      തിരികെ h->stream->തള്ളുക_headers[അക്കം];
  }
  തിരികെ ശൂന്യ;
}

/*
 * push header access function. Only to be used from within the push callback
 */
അക്ഷരം *curl_pushheader_byname(ഘടന curl_pushheaders *h, സ്ഥിരം അക്ഷരം *name)
{
  ഘടന h2_stream_ctx *stream;
  size_t len;
  size_t i;
  /* Verify that we got a good easy handle in the push header struct,
     mostly to detect rubbish input fast(er). Also empty header name
     is rubbish too. We have to allow ":" at the beginning of
     the header, but header == ":" must be rejected. If we have ':' in
     the middle of header, it could be matched in middle of the value,
     this is because we do prefix match.*/
  എങ്കിൽ(!h || !GOOD_EASY_HANDLE(h->data) || !name || !name[0] ||
     !strcmp(name, ":") || strchr(name + 1, ':'))
    തിരികെ ശൂന്യ;

  stream = h->stream;
  എങ്കിൽ(!stream)
    തിരികെ ശൂന്യ;

  len = strlen(name);
  വേണ്ടി(i = 0; i < stream->തള്ളുക_headers_used; i++) {
    എങ്കിൽ(!strncmp(name, stream->തള്ളുക_headers[i], len)) {
      /* sub-match, make sure that it is followed by a colon */
      എങ്കിൽ(stream->തള്ളുക_headers[i][len] != ':')
        തുടരുക;
      തിരികെ &stream->തള്ളുക_headers[i][len + 1];
    }
  }
  തിരികെ ശൂന്യ;
}

സ്ഥിതം ഘടന Curl_easy *h2_duphandle(ഘടന Curl_cfilter *cf,
                                      ഘടന Curl_easy *data)
{
  ഘടന Curl_easy *second = curl_easy_init();
  എങ്കിൽ(second) {
    ഘടന h2_stream_ctx *second_stream;
    http2_data_setup(cf, second, &second_stream);
    second->state.weight = data->state.weight;
    എങ്കിൽ(data->share)
      (ശൂന്യം)Curl_share_easy_link(second, data->share);
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
  തിരികെ second;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ set_transfer_url(ഘടന Curl_easy *newhandle,
                            ഘടന curl_pushheaders *hp,
                            ഘടന Curl_easy *data)
{
  സ്ഥിരം അക്ഷരം *v;
  CURLUcode uc;
  അക്ഷരം *url = ശൂന്യ;
  പൂർണ്ണസംഖ്യ rc = 0;
  CURLU *u = curl_url();

  എങ്കിൽ(!u)
    തിരികെ 5;

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_SCHEME);
  എങ്കിൽ(v) {
    uc = curl_url_set(u, CURLUPART_SCHEME, v, 0);
    എങ്കിൽ(uc) {
      rc = 1;
      പോകുക fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_AUTHORITY);
  എങ്കിൽ(v) {
    uc = Curl_url_set_authority(u, v);
    എങ്കിൽ(uc) {
      rc = 2;
      പോകുക fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_PATH);
  എങ്കിൽ(v) {
    uc = curl_url_set(u, CURLUPART_PATH, v, 0);
    എങ്കിൽ(uc) {
      rc = 3;
      പോകുക fail;
    }
  }

  /* We can only allow PUSH of resource from the same origin, e.g.
   * scheme + hostname + port */
  എങ്കിൽ(!Curl_url_same_origin(data->state.uh, u)) {
    rc = 1;
    പോകുക fail;
  }

  uc = curl_url_get(u, CURLUPART_URL, &url, 0);
  എങ്കിൽ(uc)
    rc = 4;
fail:
  എങ്കിൽ(rc) {
    curl_url_cleanup(u);
    തിരികെ rc;
  }

  DEBUGASSERT(!newhandle->state.uh);
  newhandle->state.uh = u;
  Curl_bufref_set(&newhandle->state.url, url, 0, curl_free);
  തിരികെ 0;
}

സ്ഥിതം ശൂന്യം discard_newhandle(ഘടന Curl_cfilter *cf,
                              ഘടന Curl_easy *newhandle)
{
  http2_data_done(cf, newhandle);
  (ശൂന്യം)Curl_close(&newhandle);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ തള്ളുക_promise(ഘടന Curl_cfilter *cf,
                        ഘടന Curl_easy *data,
                        സ്ഥിരം nghttp2_തള്ളുക_promise *frame)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  പൂർണ്ണസംഖ്യ rv; /* one of the CURL_PUSH_* defines */

  CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE received",
              frame->promised_stream_id);
  എങ്കിൽ(data->multi->തള്ളുക_cb) {
    ഘടന h2_stream_ctx *stream;
    ഘടന h2_stream_ctx *newstream;
    ഘടന curl_pushheaders heads;
    CURLMcode mresult;
    CURLcode result;
    /* clone the parent */
    ഘടന Curl_easy *newhandle = h2_duphandle(cf, data);
    എങ്കിൽ(!newhandle) {
      infof(data, "failed to duplicate handle");
      rv = CURL_തള്ളുക_DENY; /* FAIL HARD */
      പോകുക fail;
    }

    stream = H2_STREAM_CTX(ctx, data);
    എങ്കിൽ(!stream) {
      failf(data, "Internal NULL stream");
      discard_newhandle(cf, newhandle);
      rv = CURL_തള്ളുക_DENY;
      പോകുക fail;
    }

    heads.data = data;
    heads.stream = stream;
    heads.frame = frame;

    rv = set_transfer_url(newhandle, &heads, data);
    എങ്കിൽ(rv) {
      CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE, failed to set URL -> %d",
                  frame->promised_stream_id, rv);
      discard_newhandle(cf, newhandle);
      rv = CURL_തള്ളുക_DENY;
      പോകുക fail;
    }

    {
      ഘടന Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, multi_തള്ളുക_cb);
      rv = data->multi->തള്ളുക_cb(data, newhandle,
                                stream->തള്ളുക_headers_used, &heads,
                                data->multi->തള്ളുക_userp);
      CURL_CBAPI_END(&guard);
    }

    /* free the headers again */
    free_തള്ളുക_headers(stream);

    എങ്കിൽ(rv) {
      DEBUGASSERT((rv > CURL_തള്ളുക_OK) && (rv <= CURL_തള്ളുക_ERROROUT));
      /* denied, kill off the new handle again */
      CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE, denied by application -> %d",
                  frame->promised_stream_id, rv);
      discard_newhandle(cf, newhandle);
      പോകുക fail;
    }

    /* approved, add to the multi handle for processing. This
     * assigns newhandle->mid. For the new `mid` we assign the
     * h2_stream instance and remember the stream_id already known. */
    mresult = Curl_multi_add_perform(data->multi, newhandle, cf->conn);
    എങ്കിൽ(mresult) {
      infof(data, "failed to add handle to multi");
      discard_newhandle(cf, newhandle);
      rv = CURL_തള്ളുക_DENY;
      പോകുക fail;
    }

    result = http2_data_setup(cf, newhandle, &newstream);
    എങ്കിൽ(result) {
      failf(data, "error setting up stream: %d", (പൂർണ്ണസംഖ്യ)result);
      discard_newhandle(cf, newhandle);
      rv = CURL_തള്ളുക_DENY;
      പോകുക fail;
    }

    DEBUGASSERT(newstream);
    newstream->id = frame->promised_stream_id;
    newhandle->req.maxdownload = -1;
    newhandle->req.size = -1;

    CURL_TRC_CF(data, cf, "promise easy handle added to multi, mid=%u",
                newhandle->mid);
    rv = Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, newstream->id, newstream);
    എങ്കിൽ(rv) {
      infof(data, "failed to set user_data for stream %d",
            newstream->id);
      DEBUGASSERT(0);
      discard_newhandle(cf, newhandle);
      rv = CURL_തള്ളുക_DENY;
      പോകുക fail;
    }

    /* success, remember max stream id processed */
    എങ്കിൽ(newstream->id > ctx->local_max_sid)
      ctx->local_max_sid = newstream->id;
  }
  അല്ലെങ്കിൽ {
    CURL_TRC_CF(data, cf, "Got PUSH_PROMISE, ignore it");
    rv = CURL_തള്ളുക_DENY;
  }
fail:
  തിരികെ rv;
}

സ്ഥിതം ശൂന്യം h2_xfer_write_resp_hd(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data,
                                  ഘടന h2_stream_ctx *stream,
                                  സ്ഥിരം അക്ഷരം *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  എങ്കിൽ(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, blen, eos);
    എങ്കിൽ(!stream->xfer_result && !eos)
      stream->xfer_result = cf_h2_update_local_win(cf, data, stream);
    എങ്കിൽ(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%d] error %d writing %zu bytes of headers",
                  stream->id, (പൂർണ്ണസംഖ്യ)stream->xfer_result, blen);
  }
}

സ്ഥിതം ശൂന്യം h2_xfer_write_resp(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data,
                               ഘടന h2_stream_ctx *stream,
                               സ്ഥിരം അക്ഷരം *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  എങ്കിൽ(!stream->xfer_result)
    stream->xfer_result = Curl_xfer_write_resp(data, buf, blen, eos);
  /* If the transfer write is errored, we do not want any more data */
  എങ്കിൽ(stream->xfer_result) {
    ഘടന cf_h2_ctx *ctx = cf->ctx;
    CURL_TRC_CF(data, cf, "[%d] error %d writing %zu bytes of data, "
                "RST-ing stream",
                stream->id, (പൂർണ്ണസംഖ്യ)stream->xfer_result, blen);
    Curl_ngh2_submit_rst_stream(
      &ctx->ngh2, 0, stream->id, (uint32_t)NGHTTP2_ERR_CALLBACK_FAILURE);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!stream->write_paused && Curl_xfer_write_is_paused(data)) {
    CURL_TRC_CF(data, cf, "[%d] stream output paused", stream->id);
    stream->write_paused = TRUE;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(stream->write_paused && !Curl_xfer_write_is_paused(data)) {
    CURL_TRC_CF(data, cf, "[%d] stream output unpaused", stream->id);
    stream->write_paused = FALSE;
  }

  എങ്കിൽ(!stream->xfer_result && !eos)
    stream->xfer_result = cf_h2_update_local_win(cf, data, stream);
}

സ്ഥിതം CURLcode on_stream_frame(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data,
                                ഘടന h2_stream_ctx *stream,
                                സ്ഥിരം nghttp2_frame *frame)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  int32_t stream_id = frame->hd.stream_id;
  പൂർണ്ണസംഖ്യ rv;

  എങ്കിൽ(!stream) {
    CURL_TRC_CF(data, cf, "[%d] No stream_ctx set", stream_id);
    തിരികെ CURLE_FAILED_INIT;
  }

  തിരഞ്ഞെടുപ്പ്(frame->hd.type) {
  അവസ്ഥ NGHTTP2_DATA:
    CURL_TRC_CF(data, cf, "[%d] DATA, window=%d/%d",
                stream_id,
                Curl_ngh2_session_get_stream_effective_recv_data_length(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_stream_effective_local_window_size(
                  &ctx->ngh2, stream->id));
    /* If !body started on this stream, then receiving DATA is illegal. */
    എങ്കിൽ(!stream->bodystarted) {
      എങ്കിൽ(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                       stream_id, NGHTTP2_PROTOCOL_ERROR))) {
        തിരികെ CURLE_RECV_ERROR;
      }
    }
    നിർത്തുക;
  അവസ്ഥ NGHTTP2_HEADERS:
    എങ്കിൽ(stream->bodystarted) {
      /* Only valid HEADERS after body started is trailer HEADERS. We
         buffer them in on_header callback. */
      നിർത്തുക;
    }

    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    എങ്കിൽ(stream->status_code == -1)
      തിരികെ CURLE_RECV_ERROR;

    /* Only final status code signals the end of header */
    എങ്കിൽ(stream->status_code / 100 != 1) {
      stream->resp_hds_complete = TRUE;
      stream->bodystarted = TRUE;
    }
    അല്ലെങ്കിൽ
      stream->status_code = -1;

    h2_xfer_write_resp_hd(cf, data, stream, STRCONST("\r\n"),
                          (bool)stream->closed);

    Curl_multi_mark_dirty(data);
    നിർത്തുക;
  അവസ്ഥ NGHTTP2_തള്ളുക_PROMISE:
    rv = തള്ളുക_promise(cf, data, &frame->തള്ളുക_promise);
    എങ്കിൽ(rv) { /* deny! */
      DEBUGASSERT((rv > CURL_തള്ളുക_OK) && (rv <= CURL_തള്ളുക_ERROROUT));
      എങ്കിൽ(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
             frame->തള്ളുക_promise.promised_stream_id, NGHTTP2_CANCEL)))
        തിരികെ CURLE_SEND_ERROR;
      അല്ലെങ്കിൽ എങ്കിൽ(rv == CURL_തള്ളുക_ERROROUT) {
        /* fail the parent transfer, not the whole session */
        CURL_TRC_CF(data, cf, "[%d] fail in PUSH_PROMISE received",
                    stream_id);
        എങ്കിൽ(!stream->xfer_result)
          stream->xfer_result = CURLE_RECV_ERROR;
        Curl_multi_mark_dirty(data);
      }
    }
    നിർത്തുക;
  അവസ്ഥ NGHTTP2_RST_STREAM:
    എങ്കിൽ(frame->rst_stream.error_code)
      stream->reset_by_server = TRUE;
    Curl_multi_mark_dirty(data);
    നിർത്തുക;
  അവസ്ഥ NGHTTP2_WINDOW_UPDATE:
    എങ്കിൽ(CURL_REQ_WANT_SEND(data) && Curl_bufq_is_empty(&stream->sendbuf)) {
      /* need more data, force processing of transfer */
      Curl_multi_mark_dirty(data);
    }
    അല്ലെങ്കിൽ എങ്കിൽ(!Curl_bufq_is_empty(&stream->sendbuf)) {
      /* resume the potentially suspended stream */
      എങ്കിൽ(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id)))
        തിരികെ CURLE_SEND_ERROR;
    }
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }

  എങ്കിൽ(frame->hd.flags & NGHTTP2_FLAG_END_STREAM) {
    എങ്കിൽ(!stream->closed && !stream->body_eos &&
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
  തിരികെ CURLE_OK;
}

#ifdef CURLVERBOSE
പൂർണ്ണസംഖ്യ Curl_nghttp2_fr_print(സ്ഥിരം nghttp2_frame *frame, അക്ഷരം *buffer,
                          size_t blen)
{
  തിരഞ്ഞെടുപ്പ്(frame->hd.type) {
  അവസ്ഥ NGHTTP2_DATA: {
    തിരികെ curl_msnprintf(buffer, blen,
                          "FRAME[DATA, len=%d, eos=%d, padlen=%d]",
                          (പൂർണ്ണസംഖ്യ)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_STREAM),
                          (പൂർണ്ണസംഖ്യ)frame->data.padlen);
  }
  അവസ്ഥ NGHTTP2_HEADERS: {
    തിരികെ curl_msnprintf(buffer, blen,
                          "FRAME[HEADERS, len=%d, hend=%d, eos=%d]",
                          (പൂർണ്ണസംഖ്യ)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_HEADERS),
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_STREAM));
  }
  അവസ്ഥ NGHTTP2_PRIORITY: {
    തിരികെ curl_msnprintf(buffer, blen,
                          "FRAME[PRIORITY, len=%d, flags=%d]",
                          (പൂർണ്ണസംഖ്യ)frame->hd.length, frame->hd.flags);
  }
  അവസ്ഥ NGHTTP2_RST_STREAM: {
    തിരികെ curl_msnprintf(buffer, blen,
                          "FRAME[RST_STREAM, len=%d, flags=%d, error=%u]",
                          (പൂർണ്ണസംഖ്യ)frame->hd.length, frame->hd.flags,
                          frame->rst_stream.error_code);
  }
  അവസ്ഥ NGHTTP2_SETTINGS: {
    എങ്കിൽ(frame->hd.flags & NGHTTP2_FLAG_ACK) {
      തിരികെ curl_msnprintf(buffer, blen, "FRAME[SETTINGS, ack=1]");
    }
    തിരികെ curl_msnprintf(buffer, blen,
                          "FRAME[SETTINGS, len=%d]", (പൂർണ്ണസംഖ്യ)frame->hd.length);
  }
  അവസ്ഥ NGHTTP2_തള്ളുക_PROMISE:
    തിരികെ curl_msnprintf(buffer, blen,
                          "FRAME[PUSH_PROMISE, len=%d, hend=%d]",
                          (പൂർണ്ണസംഖ്യ)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_HEADERS));
  അവസ്ഥ NGHTTP2_PING:
    തിരികെ curl_msnprintf(buffer, blen,
                          "FRAME[PING, len=%d, ack=%d]",
                          (പൂർണ്ണസംഖ്യ)frame->hd.length,
                          frame->hd.flags & NGHTTP2_FLAG_ACK);
  അവസ്ഥ NGHTTP2_GOAWAY: {
    അക്ഷരം scratch[128];
    size_t s_len = CURL_ARRAYSIZE(scratch);
    size_t len = (frame->goaway.opaque_data_len < s_len) ?
      frame->goaway.opaque_data_len : s_len - 1;
    എങ്കിൽ(len)
      memcpy(scratch, frame->goaway.opaque_data, len);
    scratch[len] = '\0';
    തിരികെ curl_msnprintf(buffer, blen,
                          "FRAME[GOAWAY, error=%u, reason='%s', "
                          "last_stream=%d]", frame->goaway.error_code,
                          scratch, frame->goaway.last_stream_id);
  }
  അവസ്ഥ NGHTTP2_WINDOW_UPDATE: {
    തിരികെ curl_msnprintf(buffer, blen,
                          "FRAME[WINDOW_UPDATE, incr=%d]",
                          frame->window_update.window_size_increment);
  }
  സ്ഥിരസ്ഥിതി:
    തിരികെ curl_msnprintf(buffer, blen, "FRAME[%d, len=%d, flags=%d]",
                          frame->hd.type, (പൂർണ്ണസംഖ്യ)frame->hd.length,
                          frame->hd.flags);
  }
}

സ്ഥിതം പൂർണ്ണസംഖ്യ on_frame_send(nghttp2_session *session, സ്ഥിരം nghttp2_frame *frame,
                         ശൂന്യം *userp)
{
  ഘടന Curl_cfilter *cf = userp;
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);

  (ശൂന്യം)session;
  DEBUGASSERT(data);
  എങ്കിൽ(Curl_trc_cf_is_verbose(cf, data)) {
    അക്ഷരം buffer[256];
    പൂർണ്ണസംഖ്യ len;
    len = Curl_nghttp2_fr_print(frame, buffer, വലുപ്പം(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] -> %s", frame->hd.stream_id, buffer);
  }
  എങ്കിൽ((frame->hd.type == NGHTTP2_GOAWAY) && !ctx->sent_goaway) {
    /* A GOAWAY not initiated by us, but by nghttp2 itself on detecting
     * a protocol error on the connection */
    failf(data, "nghttp2 shuts down connection with error %u: %s",
          frame->goaway.error_code,
          nghttp2_http2_strerror(frame->goaway.error_code));
  }
  തിരികെ 0;
}
#endif /* CURLVERBOSE */

സ്ഥിതം പൂർണ്ണസംഖ്യ on_frame_recv(nghttp2_session *session, സ്ഥിരം nghttp2_frame *frame,
                         ശൂന്യം *userp)
{
  ഘടന Curl_cfilter *cf = userp;
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  ഘടന Curl_easy *data = CF_DATA_CURRENT(cf), *data_s;
  ഘടന h2_stream_ctx *stream;
  int32_t stream_id = frame->hd.stream_id;

  (ശൂന്യം)session;
  DEBUGASSERT(data);
#ifdef CURLVERBOSE
  എങ്കിൽ(Curl_trc_cf_is_verbose(cf, data)) {
    അക്ഷരം buffer[256];
    പൂർണ്ണസംഖ്യ len;
    len = Curl_nghttp2_fr_print(frame, buffer, വലുപ്പം(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] <- %s", frame->hd.stream_id, buffer);
  }
#endif /* CURLVERBOSE */

  എങ്കിൽ(!stream_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(data);
    തിരഞ്ഞെടുപ്പ്(frame->hd.type) {
    അവസ്ഥ NGHTTP2_SETTINGS: {
      എങ്കിൽ(!(frame->hd.flags & NGHTTP2_FLAG_ACK)) {
        uint32_t max_conn = ctx->max_concurrent_streams;
        ctx->max_concurrent_streams =
          Curl_ngh2_session_get_remote_settings(
            &ctx->ngh2, NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS);
        ctx->enable_തള്ളുക = Curl_ngh2_session_get_remote_settings(
          &ctx->ngh2, NGHTTP2_SETTINGS_ENABLE_തള്ളുക) != 0;
        CURL_TRC_CF(data, cf, "[0] MAX_CONCURRENT_STREAMS: %u",
                    ctx->max_concurrent_streams);
        CURL_TRC_CF(data, cf, "[0] ENABLE_PUSH: %s",
                    ctx->enable_തള്ളുക ? "TRUE" : "false");
        എങ്കിൽ(data && max_conn != ctx->max_concurrent_streams) {
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
        എങ്കിൽ(CURL_REQ_WANT_SEND(data))
          Curl_multi_mark_dirty(data);
      }
      നിർത്തുക;
    }
    അവസ്ഥ NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->goaway_error = frame->goaway.error_code;
      ctx->remote_max_sid = frame->goaway.last_stream_id;
      എങ്കിൽ(data) {
        infof(data, "received GOAWAY, error=%u, last_stream=%d",
              ctx->goaway_error, ctx->remote_max_sid);
        Curl_multi_connchanged(data->multi);
      }
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      നിർത്തുക;
    }
    തിരികെ 0;
  }

  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  എങ്കിൽ(!data_s) {
    CURL_TRC_CF(data, cf, "[%d] No Curl_easy associated", stream_id);
    തിരികെ 0;
  }

  തിരികെ on_stream_frame(cf, data_s, stream, frame) ?
         NGHTTP2_ERR_CALLBACK_FAILURE : 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cf_h2_on_invalid_frame_recv(nghttp2_session *session,
                                       സ്ഥിരം nghttp2_frame *frame,
                                       പൂർണ്ണസംഖ്യ ngerr, ശൂന്യം *userp)
{
  ഘടന Curl_cfilter *cf = userp;
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  ഘടന Curl_easy *data;
  ഘടന h2_stream_ctx *stream;
  int32_t stream_id = frame->hd.stream_id;

  (ശൂന്യം)session;
  data = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  എങ്കിൽ(data) {
#ifdef CURLVERBOSE
    അക്ഷരം buffer[256];
    പൂർണ്ണസംഖ്യ len;
    len = Curl_nghttp2_fr_print(frame, buffer, വലുപ്പം(buffer) - 1);
    buffer[len] = 0;
    failf(data, "[HTTP2] [%d] received invalid frame: %s, error %d: %s",
          stream_id, buffer, ngerr, nghttp2_strerror(ngerr));
#endif /* CURLVERBOSE */
    എങ്കിൽ(stream) {
      Curl_ngh2_submit_rst_stream(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, NGHTTP2_STREAM_CLOSED);
      stream->error = ngerr;
      stream->closed = TRUE;
      stream->reset = TRUE;
    }
  }
  /* Return no error, nghttp2 will RST/GOAWAY by itself when needed */
  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ on_data_chunk_recv(nghttp2_session *session, uint8_t flags,
                              int32_t stream_id,
                              സ്ഥിരം uint8_t *mem, size_t len, ശൂന്യം *userp)
{
  ഘടന Curl_cfilter *cf = userp;
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  ഘടന h2_stream_ctx *stream;
  ഘടന Curl_easy *data_s;

  (ശൂന്യം)session;
  (ശൂന്യം)flags;

  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */
  DEBUGASSERT(CF_DATA_CURRENT(cf));

  /* get the stream from the hash based on Stream ID */
  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  എങ്കിൽ(!data_s) {
    /* Receiving a Stream ID not in the hash should not happen - unless
       we have aborted a transfer artificially and there were more data
       in the pipeline. Silently ignore. */
    CURL_TRC_CF(CF_DATA_CURRENT(cf), cf, "[%d] Data for unknown", stream_id);
    /* consumed explicitly as no one will read it */
    Curl_ngh2_session_consume(&ctx->ngh2, stream_id, len);
    തിരികെ 0;
  }

  എങ്കിൽ(!stream)
    തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;

  h2_xfer_write_resp(cf, data_s, stream, (സ്ഥിരം അക്ഷരം *)mem, len, FALSE);

  Curl_ngh2_session_consume(&ctx->ngh2, stream_id, len);
  stream->nrcvd_data += (curl_off_t)len;
  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ on_stream_close(nghttp2_session *session, int32_t stream_id,
                           uint32_t error_code, ശൂന്യം *userp)
{
  ഘടന Curl_cfilter *cf = userp;
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  ഘടന Curl_easy *data_s, *call_data = CF_DATA_CURRENT(cf);
  ഘടന h2_stream_ctx *stream = ശൂന്യ;
  പൂർണ്ണസംഖ്യ rv;
  (ശൂന്യം)session;

  DEBUGASSERT(call_data);
  /* stream id 0 is the connection, do not look there for streams. */
  data_s = stream_id ? cf_h2_get_stream_xfer(cf, stream_id, &stream) : ശൂന്യ;
  എങ്കിൽ(!data_s) {
    CURL_TRC_CF(call_data, cf,
                "[%d] on_stream_close, no easy set on stream", stream_id);
    തിരികെ 0;
  }
  എങ്കിൽ(!GOOD_EASY_HANDLE(data_s)) {
    /* nghttp2 still has an easy registered for the stream which has
     * been freed be libcurl. This points to a code path that does not
     * trigger DONE or DETACH events as it must. */
    CURL_TRC_CF(call_data, cf,
                "[%d] on_stream_close, not a GOOD easy on stream", stream_id);
    (ശൂന്യം)Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, stream_id, ശൂന്യ);
    തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  എങ്കിൽ(!stream) {
    CURL_TRC_CF(data_s, cf,
                "[%d] on_stream_close, GOOD easy but no stream", stream_id);
    തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  stream->closed = TRUE;
  stream->error = error_code;
  എങ്കിൽ(stream->error) {
    stream->reset = TRUE;
    എങ്കിൽ((stream->error == NGHTTP2_REFUSED_STREAM) &&
       ctx->rcvd_goaway && ctx->goaway_error)
    /* REFUSED_STREAM is used by nghttp2 on a GOAWAY from the
     * server where it indicated it will no longer process this stream.
     * If the GOAWAY carried an error code, take that as the error for
     * the stream. */
    stream->error = ctx->goaway_error;
  }

  എങ്കിൽ(stream->error)
    CURL_TRC_CF(data_s, cf, "[%d] RESET: %s (err %u)",
              stream_id, nghttp2_http2_strerror(error_code), error_code);
  അല്ലെങ്കിൽ
    CURL_TRC_CF(data_s, cf, "[%d] CLOSED", stream_id);
  Curl_multi_mark_dirty(data_s);

  /* remove `stream_ctx` from the nghttp2 stream */
  rv = Curl_ngh2_session_set_stream_user_data(
    &ctx->ngh2, stream_id, ശൂന്യ);
  എങ്കിൽ(rv) {
    infof(data_s, "http/2: failed to clear user_data for stream %d",
          stream_id);
    DEBUGASSERT(0);
  }
  തിരികെ 0;
}

സ്ഥിതം ശൂന്യം cf_h2_header_error(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data,
                               ഘടന h2_stream_ctx *stream,
                               CURLcode result)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;

  failf(data, "Error receiving HTTP2 header: %d(%s)", (പൂർണ്ണസംഖ്യ)result,
        curl_easy_strerror(result));
  എങ്കിൽ(stream) {
    Curl_ngh2_submit_rst_stream(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, NGHTTP2_STREAM_CLOSED);
    stream->closed = TRUE;
    stream->reset = TRUE;
  }
}

/* frame->hd.type is either NGHTTP2_HEADERS or NGHTTP2_PUSH_PROMISE */
സ്ഥിതം പൂർണ്ണസംഖ്യ on_header(nghttp2_session *session, സ്ഥിരം nghttp2_frame *frame,
                     സ്ഥിരം uint8_t *name, size_t namelen,
                     സ്ഥിരം uint8_t *മൂല്യം, size_t valuelen,
                     uint8_t flags,
                     ശൂന്യം *userp)
{
  ഘടന Curl_cfilter *cf = userp;
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  ഘടന h2_stream_ctx *stream;
  ഘടന Curl_easy *data;
  int32_t stream_id = frame->hd.stream_id;
  CURLcode result;

  (ശൂന്യം)session;
  (ശൂന്യം)flags;

  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */

  /* get the stream from the hash based on Stream ID */
  data = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  എങ്കിൽ(!GOOD_EASY_HANDLE(data))
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;

  എങ്കിൽ(!stream) {
    failf(data, "Internal NULL stream");
    തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  /* Store received PUSH_PROMISE headers to be used when the subsequent
     PUSH_PROMISE callback comes */
  എങ്കിൽ(frame->hd.type == NGHTTP2_തള്ളുക_PROMISE) {
    അക്ഷരം *h;

    എങ്കിൽ((namelen == CURL_CSTRLEN(HTTP_PSEUDO_AUTHORITY)) &&
       !strncmp(HTTP_PSEUDO_AUTHORITY, (സ്ഥിരം അക്ഷരം *)name, namelen)) {
      /* pseudo headers are lower case */
      പൂർണ്ണസംഖ്യ rc = 0;
      അക്ഷരം *check = curl_maprintf("%s:%d", data->state.origin->hostname,
                                  data->state.origin->port);
      എങ്കിൽ(!check)
        /* no memory */
        തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;
      എങ്കിൽ(!curl_strequal(check, (സ്ഥിരം അക്ഷരം *)മൂല്യം) &&
         ((data->state.origin->port != data->state.origin->scheme->defport) ||
          !curl_strequal(data->state.origin->hostname, (സ്ഥിരം അക്ഷരം *)മൂല്യം))) {
        /* This is push is not for the same authority that was asked for in
         * the URL. RFC 7540 section 8.2 says: "A client MUST treat a
         * PUSH_PROMISE for which the server is not authoritative as a stream
         * error of type PROTOCOL_ERROR."
         */
        (ശൂന്യം)Curl_ngh2_submit_rst_stream(
          &ctx->ngh2, NGHTTP2_FLAG_NONE, stream_id, NGHTTP2_PROTOCOL_ERROR);
        rc = NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      curlx_free(check);
      എങ്കിൽ(rc)
        തിരികെ rc;
    }

    എങ്കിൽ(!stream->തള്ളുക_headers) {
      stream->തള്ളുക_headers_alloc = 10;
      stream->തള്ളുക_headers = curlx_malloc(stream->തള്ളുക_headers_alloc *
                                          വലുപ്പം(അക്ഷരം *));
      എങ്കിൽ(!stream->തള്ളുക_headers)
        തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;
      stream->തള്ളുക_headers_used = 0;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(stream->തള്ളുക_headers_used ==
            stream->തള്ളുക_headers_alloc) {
      അക്ഷരം **headp;
      എങ്കിൽ(stream->തള്ളുക_headers_alloc > 1000) {
        /* this is beyond crazy many headers, bail out */
        failf(data, "Too many PUSH_PROMISE headers");
        free_തള്ളുക_headers(stream);
        തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      stream->തള്ളുക_headers_alloc *= 2;
      headp = curlx_realloc(stream->തള്ളുക_headers,
                            stream->തള്ളുക_headers_alloc * വലുപ്പം(അക്ഷരം *));
      എങ്കിൽ(!headp) {
        free_തള്ളുക_headers(stream);
        തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      stream->തള്ളുക_headers = headp;
    }
    h = curl_maprintf("%s:%s", name, മൂല്യം);
    എങ്കിൽ(!h) {
      free_തള്ളുക_headers(stream);
      തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    stream->തള്ളുക_headers[stream->തള്ളുക_headers_used++] = h;
    തിരികെ 0;
  }

  എങ്കിൽ(stream->bodystarted) {
    /* This is a trailer */
    CURL_TRC_CF(data, cf, "[%d] trailer: %.*s: %.*s",
                stream->id, (പൂർണ്ണസംഖ്യ)namelen, name, (പൂർണ്ണസംഖ്യ)valuelen, മൂല്യം);
    result = Curl_dynhds_add(&stream->resp_trailers,
                             (സ്ഥിരം അക്ഷരം *)name, namelen,
                             (സ്ഥിരം അക്ഷരം *)മൂല്യം, valuelen);
    എങ്കിൽ(result) {
      cf_h2_header_error(cf, data, stream, result);
      തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;
    }

    തിരികെ 0;
  }

  എങ്കിൽ(namelen == CURL_CSTRLEN(HTTP_PSEUDO_STATUS) &&
     !memcmp(HTTP_PSEUDO_STATUS, name, namelen)) {
    /* nghttp2 guarantees :status is received first and only once. */
    അക്ഷരം buffer[32];
    size_t hlen;
    result = Curl_http_decode_status(&stream->status_code,
                                     (സ്ഥിരം അക്ഷരം *)മൂല്യം, valuelen);
    എങ്കിൽ(result) {
      cf_h2_header_error(cf, data, stream, result);
      തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    hlen = curl_msnprintf(buffer, വലുപ്പം(buffer), HTTP_PSEUDO_STATUS ":%d\r",
                          stream->status_code);
    result = Curl_headers_തള്ളുക(data, buffer, hlen, CURLH_PSEUDO);
    എങ്കിൽ(result) {
      cf_h2_header_error(cf, data, stream, result);
      തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    curlx_dyn_reset(&ctx->scratch);
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/2 "));
    എങ്കിൽ(!result)
      result = curlx_dyn_addn(&ctx->scratch, മൂല്യം, valuelen);
    എങ്കിൽ(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    എങ്കിൽ(!result)
      h2_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    എങ്കിൽ(result) {
      cf_h2_header_error(cf, data, stream, result);
      തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    /* if we receive data for another handle, wake that up */
    എങ്കിൽ(CF_DATA_CURRENT(cf) != data)
      Curl_multi_mark_dirty(data);

    CURL_TRC_CF(data, cf, "[%d] status: HTTP/2 %03d",
                stream->id, stream->status_code);
    തിരികെ 0;
  }

  /* nghttp2 guarantees that namelen > 0, and :status was already
     received, and this is not pseudo-header field . */
  /* convert to an HTTP1-style header */
  curlx_dyn_reset(&ctx->scratch);
  result = curlx_dyn_addn(&ctx->scratch, (സ്ഥിരം അക്ഷരം *)name, namelen);
  എങ്കിൽ(!result)
    result = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
  എങ്കിൽ(!result)
    result = curlx_dyn_addn(&ctx->scratch, (സ്ഥിരം അക്ഷരം *)മൂല്യം, valuelen);
  എങ്കിൽ(!result)
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
  എങ്കിൽ(!result)
    h2_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                          curlx_dyn_len(&ctx->scratch), FALSE);
  എങ്കിൽ(result) {
    cf_h2_header_error(cf, data, stream, result);
    തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  /* if we receive data for another handle, wake that up */
  എങ്കിൽ(CF_DATA_CURRENT(cf) != data)
    Curl_multi_mark_dirty(data);

  CURL_TRC_CF(data, cf, "[%d] header: %.*s: %.*s",
              stream->id, (പൂർണ്ണസംഖ്യ)namelen, name, (പൂർണ്ണസംഖ്യ)valuelen, മൂല്യം);

  തിരികെ 0; /* 0 is successful */
}

സ്ഥിതം ssize_t req_body_read_callback(nghttp2_session *session,
                                      int32_t stream_id,
                                      uint8_t *buf, size_t length,
                                      uint32_t *data_flags,
                                      nghttp2_data_source *source,
                                      ശൂന്യം *userp)
{
  ഘടന Curl_cfilter *cf = userp;
  ഘടന Curl_easy *data_s;
  ഘടന h2_stream_ctx *stream = ശൂന്യ;
  CURLcode result;
  ssize_t nread;
  size_t n;
  (ശൂന്യം)source;

  (ശൂന്യം)session;
  എങ്കിൽ(!stream_id)
    തിരികെ NGHTTP2_ERR_INVALID_ARGUMENT;

  /* get the stream from the hash based on Stream ID, stream ID zero is for
     connection-oriented stuff */
  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  എങ്കിൽ(!data_s || !stream)
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_bufq_read(&stream->sendbuf, buf, length, &n);
  എങ്കിൽ(result) {
    എങ്കിൽ(result != CURLE_AGAIN)
      തിരികെ NGHTTP2_ERR_CALLBACK_FAILURE;
    nread = 0;
  }
  അല്ലെങ്കിൽ
    nread = (ssize_t)n;

  CURL_TRC_CF(data_s, cf, "[%d] req_body_read(len=%zu) eos=%d -> %zd, %d",
              stream_id, length, stream->body_eos, nread, (പൂർണ്ണസംഖ്യ)result);

  എങ്കിൽ(stream->body_eos && Curl_bufq_is_empty(&stream->sendbuf)) {
    *data_flags = NGHTTP2_DATA_FLAG_EOF;
    തിരികെ nread;
  }
  തിരികെ (nread == 0) ? NGHTTP2_ERR_DEFERRED : nread;
}

#ifdef CURLVERBOSE
സ്ഥിതം പൂർണ്ണസംഖ്യ error_callback(nghttp2_session *session,
                          സ്ഥിരം അക്ഷരം *സന്ദേശം,
                          size_t len,
                          ശൂന്യം *userp)
{
  ഘടന Curl_cfilter *cf = userp;
  ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);
  (ശൂന്യം)session;
  failf(data, "%.*s", (പൂർണ്ണസംഖ്യ)len, സന്ദേശം);
  തിരികെ 0;
}
#endif

/*
 * Append headers to ask for an HTTP1.1 to HTTP2 upgrade.
 */
CURLcode Curl_http2_request_upgrade(ഘടന dynbuf *req,
                                    ഘടന Curl_easy *data)
{
  CURLcode result;
  അക്ഷരം *base64;
  size_t blen;
  ഘടന SingleRequest *k = &data->req;
  uint8_t binsettings[H2_BINSETTINGS_LEN];
  ssize_t rc;
  size_t binlen; /* length of the binsettings data */

  rc = populate_binsettings(binsettings, data);
  എങ്കിൽ(!curlx_sztouz(rc, &binlen) || !binlen) {
    failf(data, "nghttp2 unexpectedly failed on pack_settings_payload");
    curlx_dyn_free(req);
    തിരികെ CURLE_FAILED_INIT;
  }

  result = curlx_base64url_encode(binsettings, binlen, &base64, &blen);
  എങ്കിൽ(result) {
    curlx_dyn_free(req);
    തിരികെ result;
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

  തിരികെ result;
}

സ്ഥിതം CURLcode http2_handle_stream_close(ഘടന Curl_cfilter *cf,
                                          ഘടന Curl_easy *data,
                                          ഘടന h2_stream_ctx *stream,
                                          size_t *pnlen)
{
  CURLcode result;

  *pnlen = 0;
  എങ്കിൽ(stream->reset) {
    എങ്കിൽ(stream->error == NGHTTP2_REFUSED_STREAM) {
      infof(data, "HTTP/2 stream %d refused by server, try again on a new "
                  "connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      തിരികെ CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    അല്ലെങ്കിൽ എങ്കിൽ(stream->resp_hds_complete && data->req.no_body) {
      CURL_TRC_CF(data, cf, "[%d] error after response headers, but we did "
                  "not want a body anyway, ignore: %s (err %u)",
                  stream->id, nghttp2_http2_strerror(stream->error),
                  stream->error);
      stream->close_handled = TRUE;
      തിരികെ CURLE_OK;
    }
    failf(data, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          stream->id, stream->reset_by_server ? "server" : "curl",
          stream->error, nghttp2_http2_strerror(stream->error));
    തിരികെ stream->error ? CURLE_HTTP2_STREAM :
           (data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!stream->bodystarted) {
    failf(data, "HTTP/2 stream %d was closed cleanly, but before getting "
          "all response header fields, treated as error", stream->id);
    തിരികെ CURLE_HTTP2_STREAM;
  }

  എങ്കിൽ(Curl_dynhds_എണ്ണം(&stream->resp_trailers)) {
    ഘടന dynhds_entry *e;
    ഘടന dynbuf dbuf;
    size_t i;

    result = CURLE_OK;
    curlx_dyn_init(&dbuf, DYN_TRAILERS);
    വേണ്ടി(i = 0; i < Curl_dynhds_എണ്ണം(&stream->resp_trailers); ++i) {
      e = Curl_dynhds_getn(&stream->resp_trailers, i);
      എങ്കിൽ(!e)
        നിർത്തുക;
      curlx_dyn_reset(&dbuf);
      result = curlx_dyn_addf(&dbuf, "%.*s: %.*s\x0d\x0a",
                              (പൂർണ്ണസംഖ്യ)e->namelen, e->name,
                              (പൂർണ്ണസംഖ്യ)e->valuelen, e->മൂല്യം);
      എങ്കിൽ(result)
        നിർത്തുക;
      Curl_debug(data, CURLINFO_HEADER_IN, curlx_dyn_ptr(&dbuf),
                 curlx_dyn_len(&dbuf));
      result = Curl_client_write(data,
                                 CLIENTWRITE_HEADER | CLIENTWRITE_TRAILER,
                                 curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf));
      എങ്കിൽ(result)
        നിർത്തുക;
      result = Curl_bump_headersize(data, curlx_dyn_len(&dbuf), FALSE);
      എങ്കിൽ(result)
        തിരികെ result;
    }
    curlx_dyn_free(&dbuf);
    എങ്കിൽ(result)
      പോകുക out;
  }

  stream->close_handled = TRUE;
  result = CURLE_OK;

out:
  CURL_TRC_CF(data, cf, "handle_stream_close -> %d, %zu", (പൂർണ്ണസംഖ്യ)result, *pnlen);
  തിരികെ result;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sweight_wanted(സ്ഥിരം ഘടന Curl_easy *data)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  തിരികെ data->set.weight ?
    data->set.weight : NGHTTP2_DEFAULT_WEIGHT;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sweight_in_effect(സ്ഥിരം ഘടന Curl_easy *data)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  തിരികെ data->state.weight ?
    data->state.weight : NGHTTP2_DEFAULT_WEIGHT;
}

/*
 * h2_pri_spec() fills in the pri_spec struct, used by nghttp2 to send weight
 * and dependency to the peer. It also stores the updated values in the state
 * struct.
 */

സ്ഥിതം ശൂന്യം h2_pri_spec(ഘടന Curl_easy *data,
                        nghttp2_priority_spec *pri_spec)
{
  പൂർണ്ണസംഖ്യ prio = data->set.weight;
  nghttp2_priority_spec_init(pri_spec, 0, sweight_wanted(data), FALSE);
  data->state.weight = prio;
}

/*
 * Check if there is been an update in the priority /
 * dependency settings and if so it submits a PRIORITY frame with the updated
 * info.
 * Flush any out data pending in the network buffer.
 */
സ്ഥിതം CURLcode h2_progress_egress(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  പൂർണ്ണസംഖ്യ rv = 0;

  എങ്കിൽ(sweight_wanted(data) != sweight_in_effect(data)) {
    ഘടന h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    എങ്കിൽ(stream && stream->id > 0 &&
       (sweight_wanted(data) != sweight_in_effect(data))) {
      /* send new weight and/or dependency */
      nghttp2_priority_spec pri_spec;

      h2_pri_spec(data, &pri_spec);
      CURL_TRC_CF(data, cf, "[%d] Queuing PRIORITY", stream->id);
      DEBUGASSERT(stream->id != -1);
      rv = Curl_ngh2_submit_priority(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, &pri_spec);
      എങ്കിൽ(rv)
        പോകുക out;
    }
  }

  ctx->nw_out_blocked = 0;
  ആകുമ്പോൾ(!rv && !ctx->nw_out_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

out:
  എങ്കിൽ(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(data, cf, "nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    തിരികെ CURLE_SEND_ERROR;
  }
  /* Defer flushing during the connect phase so that the SETTINGS and
   * other initial frames are sent together with the first request.
   * Unless we are 'connect_only' where the request will never come. */
  എങ്കിൽ(!cf->connected && !cf->conn->bits.connect_only)
    തിരികെ CURLE_OK;
  തിരികെ nw_out_flush(cf, data);
}

സ്ഥിതം CURLcode stream_recv(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                            ഘടന h2_stream_ctx *stream,
                            അക്ഷരം *buf, size_t len, size_t *pnread)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_AGAIN;

  (ശൂന്യം)buf;
  (ശൂന്യം)len;
  *pnread = 0;

  എങ്കിൽ(!stream->xfer_result)
    stream->xfer_result = cf_h2_update_local_win(cf, data, stream);

  എങ്കിൽ(stream->xfer_result) {
    CURL_TRC_CF(data, cf, "[%d] xfer write failed", stream->id);
    result = stream->xfer_result;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(stream->closed) {
    CURL_TRC_CF(data, cf, "[%d] returning CLOSE", stream->id);
    result = http2_handle_stream_close(cf, data, stream, pnread);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(stream->reset ||
          (ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) ||
          (ctx->rcvd_goaway && ctx->remote_max_sid < stream->id)) {
    CURL_TRC_CF(data, cf, "[%d] returning ERR", stream->id);
    result = data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2;
  }

  എങ്കിൽ(result && (result != CURLE_AGAIN))
    CURL_TRC_CF(data, cf, "[%d] stream_recv(len=%zu) -> %d, %zu",
                stream->id, len, (പൂർണ്ണസംഖ്യ)result, *pnread);
  തിരികെ result;
}

സ്ഥിതം CURLcode h2_progress_ingress(ഘടന Curl_cfilter *cf,
                                    ഘടന Curl_easy *data,
                                    size_t data_max_bytes)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  എങ്കിൽ(should_close_session(ctx)) {
    CURL_TRC_CF(data, cf, "[0] ingress: session is closed");
    തിരികെ CURLE_HTTP2;
  }

  /* Process network input buffer first */
  എങ്കിൽ(!Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "Process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    result = h2_process_pending_നിവേശം(cf, data);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  എങ്കിൽ(!data_max_bytes)
    data_max_bytes = H2_CHUNK_SIZE;

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop due to connection close or us not processing
   * all network input */
  ആകുമ്പോൾ(!ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) {
    ഘടന h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    എങ്കിൽ(stream && (stream->closed || !data_max_bytes)) {
      /* We would like to abort here and stop processing, so that the transfer
       * loop can handle the data/close here. This may leave data in
       * underlying buffers that will not be consumed. */
      എങ്കിൽ(!cf->next || !cf->next->cft->has_data_pending(cf->next, data))
        Curl_multi_mark_dirty(data);
      നിർത്തുക;
    }

    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    എങ്കിൽ(result) {
      എങ്കിൽ(result != CURLE_AGAIN) {
        failf(data, "Failed receiving HTTP2 data: %d(%s)", (പൂർണ്ണസംഖ്യ)result,
              curl_easy_strerror(result));
        തിരികെ result;
      }
      നിർത്തുക;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(nread == 0) {
      CURL_TRC_CF(data, cf, "[0] ingress: connection closed");
      ctx->conn_closed = TRUE;
      നിർത്തുക;
    }
    അല്ലെങ്കിൽ {
      CURL_TRC_CF(data, cf, "[0] ingress: read %zu bytes", nread);
      data_max_bytes = (data_max_bytes > nread) ? (data_max_bytes - nread) : 0;
    }

    result = h2_process_pending_നിവേശം(cf, data);
    എങ്കിൽ(result)
      തിരികെ result;
    CURL_TRC_CF(data, cf, "[0] ingress: nw-in buffered %zu",
                Curl_bufq_len(&ctx->inbufq));
  }

  എങ്കിൽ(ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "server closed %s",
                ctx->rcvd_goaway ? "with GOAWAY" : "abruptly");
    connclose(cf->conn);
  }

  CURL_TRC_CF(data, cf, "[0] ingress: done");
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode cf_h2_recv(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                           അക്ഷരം *buf, size_t len, size_t *pnread)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  ഘടന h2_stream_ctx *stream;
  CURLcode result, r2;
  ഘടന cf_call_data save;

  എങ്കിൽ(!data)
    തിരികെ CURLE_HTTP2;

  stream = H2_STREAM_CTX(ctx, data);

  *pnread = 0;
  എങ്കിൽ(!stream) {
    /* Abnormal call sequence: either this transfer has never opened a stream
     * (unlikely) or the transfer has been done, cleaned up its resources, but
     * a read() is called anyway. It is not clear what the calling sequence
     * is for such a case. */
    failf(data, "http/2 recv on a transfer never opened "
          "or already cleared, mid=%u", data->mid);
    തിരികെ CURLE_HTTP2;
  }

  CF_DATA_SAVE(save, cf, data);

  result = stream_recv(cf, data, stream, buf, len, pnread);
  എങ്കിൽ(result && (result != CURLE_AGAIN))
    പോകുക out;

  എങ്കിൽ(result) {
    result = h2_progress_ingress(cf, data, len);
    എങ്കിൽ(result)
      പോകുക out;

    result = stream_recv(cf, data, stream, buf, len, pnread);
  }

  എങ്കിൽ(*pnread > 0) {
    /* Now that we transferred this to the upper layer, we report
     * the actual amount of DATA consumed to the H2 session, so
     * that it adjusts stream flow control */
    Curl_ngh2_session_consume(&ctx->ngh2, stream->id, *pnread);
    എങ്കിൽ(stream->closed) {
      CURL_TRC_CF(data, cf, "[%d] DRAIN closed stream", stream->id);
      Curl_multi_mark_dirty(data);
    }
  }

out:
  r2 = h2_progress_egress(cf, data);
  എങ്കിൽ(r2 == CURLE_AGAIN) {
    /* pending data to send, need to be called again. Ideally, we
     * monitor the socket for POLLOUT, but when not SENDING
     * any more, we force processing of the transfer. */
    എങ്കിൽ(!CURL_REQ_WANT_SEND(data))
      Curl_multi_mark_dirty(data);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(r2 && (result == CURLE_AGAIN)) {
    /* Do not replace a previous error result NOR a successful result
     * with an egress error. Example: the server might close the
     * connection after sending the complete response with EOS
     * and everything. Further sending may fail, but that MUST
     * not change that we had a successful response. */
    result = r2;
  }

  CURL_TRC_CF(data, cf, "[%d] cf_recv(len=%zu) -> %d, %zu, "
              "window=%d/%d, connection %d/%d",
              stream->id, len, (പൂർണ്ണസംഖ്യ)result, *pnread,
              Curl_ngh2_session_get_stream_effective_recv_data_length(
                &ctx->ngh2, stream->id),
              Curl_ngh2_session_get_stream_effective_local_window_size(
                &ctx->ngh2, stream->id),
              Curl_ngh2_session_get_local_window_size(&ctx->ngh2),
              HTTP2_HUGE_WINDOW_SIZE);

  CF_DATA_RESTORE(cf, save);
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_h2_body_send(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data,
                                ഘടന h2_stream_ctx *stream,
                                സ്ഥിരം ശൂന്യം *buf, size_t blen, bool eos,
                                size_t *pnwritten)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  CURLcode result;

  *pnwritten = 0;
  എങ്കിൽ(stream->closed) {
    എങ്കിൽ(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(data, cf, "[%d] discarding data"
                  "on closed stream with response", stream->id);
      എങ്കിൽ(eos)
        stream->body_eos = TRUE;
      *pnwritten = blen;
      തിരികെ CURLE_OK;
    }
    /* Server closed before we got a response, this is an error */
    infof(data, "stream %d closed", stream->id);
    തിരികെ CURLE_SEND_ERROR;
  }

  result = Curl_bufq_write(&stream->sendbuf, buf, blen, pnwritten);
  എങ്കിൽ(result)
    തിരികെ result;

  എങ്കിൽ(eos && (blen == *pnwritten))
    stream->body_eos = TRUE;

  എങ്കിൽ(eos || !Curl_bufq_is_empty(&stream->sendbuf)) {
    /* resume the potentially suspended stream */
    എങ്കിൽ(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id)))
      തിരികെ CURLE_SEND_ERROR;
  }

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode h2_submit(ഘടന h2_stream_ctx **pstream,
                          ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                          സ്ഥിരം uint8_t *buf, size_t len,
                          bool eos, size_t *pnwritten)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  ഘടന h2_stream_ctx *stream = ശൂന്യ;
  ഘടന dynhds h2_headers;
  nghttp2_nv *nva = ശൂന്യ;
  സ്ഥിരം ശൂന്യം *body = ശൂന്യ;
  size_t nheader, bodylen;
  nghttp2_data_provider data_prd;
  int32_t stream_id;
  nghttp2_priority_spec pri_spec;
  size_t nwritten;
  CURLcode result = CURLE_OK;
  uint32_t initial_win_size;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_headers, MAX_HTTP_RESP_HEADER_എണ്ണം, DYN_HTTP_REQUEST);

  result = http2_data_setup(cf, data, &stream);
  എങ്കിൽ(result)
    പോകുക out;

  result = Curl_h1_req_parse_read(
    &stream->h1, buf, len, ശൂന്യ,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : ശൂന്യ,
    0, &nwritten);
  എങ്കിൽ(result)
    പോകുക out;
  *pnwritten = nwritten;
  എങ്കിൽ(!stream->h1.done) {
    /* need more data */
    പോകുക out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  എങ്കിൽ(result)
    പോകുക out;
  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nva = Curl_dynhds_to_nva(&h2_headers, &nheader);
  എങ്കിൽ(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }

  h2_pri_spec(data, &pri_spec);
  എങ്കിൽ(!Curl_ngh2_session_check_request_allowed(&ctx->ngh2))
    CURL_TRC_CF(data, cf, "send request NOT allowed (via nghttp2)");

  /* Check the initial windows size of the transfer (rate-limits?) and
   * send an updated settings on changes from previous value. */
  initial_win_size = cf_h2_initial_win_size(data);
  എങ്കിൽ(initial_win_size != ctx->initial_win_size) {
    result = cf_h2_update_settings(ctx, initial_win_size);
    എങ്കിൽ(result)
      പോകുക out;
  }

  തിരഞ്ഞെടുപ്പ്(data->state.httpreq) {
  അവസ്ഥ HTTPREQ_POST:
  അവസ്ഥ HTTPREQ_POST_FORM:
  അവസ്ഥ HTTPREQ_POST_MIME:
  അവസ്ഥ HTTPREQ_PUT:
    data_prd.read_callback = req_body_read_callback;
    data_prd.source.ptr = ശൂന്യ;
    stream_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, &data_prd, stream);
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    stream_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, ശൂന്യ, stream);
  }

  എങ്കിൽ(stream_id < 0) {
    CURL_TRC_CF(data, cf, "send: nghttp2_submit_request error (%s)%d",
                nghttp2_strerror(stream_id), stream_id);
    result = CURLE_SEND_ERROR;
    പോകുക out;
  }

#ifdef CURLVERBOSE
#define MAX_ACC 60000  /* <64KB to account for some overhead */
  എങ്കിൽ(Curl_trc_is_verbose(data)) {
    size_t acc = 0, i;

    infof(data, "[HTTP/2] [%d] OPENED stream for %s",
          stream_id, Curl_bufref_ptr(&data->state.url));
    വേണ്ടി(i = 0; i < nheader; ++i) {
      acc += nva[i].namelen + nva[i].valuelen;

      infof(data, "[HTTP/2] [%d] [%.*s: %.*s]", stream_id,
            (പൂർണ്ണസംഖ്യ)nva[i].namelen, nva[i].name,
            (പൂർണ്ണസംഖ്യ)nva[i].valuelen, nva[i].മൂല്യം);
    }

    എങ്കിൽ(acc > MAX_ACC) {
      infof(data, "[HTTP/2] Warning: The cumulative length of all "
            "headers exceeds %d bytes and that could cause the "
            "stream to be rejected.", MAX_ACC);
    }
  }
#endif

  stream->id = stream_id;

  body = (സ്ഥിരം അക്ഷരം *)buf + *pnwritten;
  bodylen = len - *pnwritten;

  എങ്കിൽ(bodylen || eos) {
    size_t n;
    result = cf_h2_body_send(cf, data, stream, body, bodylen, eos, &n);
    എങ്കിൽ(!result)
      *pnwritten += n;
    അല്ലെങ്കിൽ എങ്കിൽ(result == CURLE_AGAIN)
      result = CURLE_OK;
    അല്ലെങ്കിൽ {
      result = CURLE_SEND_ERROR;
    }
  }

out:
  CURL_TRC_CF(data, cf, "[%d] submit -> %d, %zu",
              stream ? stream->id : -1, (പൂർണ്ണസംഖ്യ)result, *pnwritten);
  curlx_safefree(nva);
  *pstream = stream;
  Curl_dynhds_free(&h2_headers);
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_h2_send(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                           സ്ഥിരം uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  ഘടന h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
  ഘടന cf_call_data save;
  CURLcode result = CURLE_OK, r2;

  CF_DATA_SAVE(save, cf, data);
  *pnwritten = 0;

  എങ്കിൽ(!stream || stream->id == -1) {
    result = h2_submit(&stream, cf, data, buf, len, eos, pnwritten);
    എങ്കിൽ(result)
      പോകുക out;
    DEBUGASSERT(stream);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(stream->body_eos) {
    /* We already wrote this, but CURLE_AGAIN-ed the call due to not
     * being able to flush stream->sendbuf. Make a 0-length write
     * to trigger flushing again.
     * If this works, we report to have written `len` bytes. */
    size_t n;
    DEBUGASSERT(eos);
    result = cf_h2_body_send(cf, data, stream, buf, 0, eos, &n);
    CURL_TRC_CF(data, cf, "[%d] cf_body_send last CHUNK -> %d, %zu, eos=%d",
                stream->id, (പൂർണ്ണസംഖ്യ)result, n, eos);
    എങ്കിൽ(result)
      പോകുക out;
    *pnwritten = len;
  }
  അല്ലെങ്കിൽ {
    result = cf_h2_body_send(cf, data, stream, buf, len, eos, pnwritten);
    CURL_TRC_CF(data, cf, "[%d] cf_body_send(len=%zu) -> %d, %zu, eos=%d",
                stream->id, len, (പൂർണ്ണസംഖ്യ)result, *pnwritten, eos);
  }

  /* Call the nghttp2 send loop and flush to write ALL buffered data,
   * headers and/or request body completely out to the network */
  r2 = h2_progress_egress(cf, data);

  /* if the stream has been closed in egress handling (nghttp2 does that
   * when it does not like the headers, for example */
  എങ്കിൽ(stream && stream->closed && !stream->resp_hds_complete) {
    infof(data, "stream %d closed", stream->id);
    result = CURLE_SEND_ERROR;
    പോകുക out;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(r2 && (r2 != CURLE_AGAIN)) {
    result = r2;
    പോകുക out;
  }

  എങ്കിൽ(should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    എങ്കിൽ(stream && stream->closed) {
      result = http2_handle_stream_close(cf, data, stream, pnwritten);
    }
    അല്ലെങ്കിൽ {
      CURL_TRC_CF(data, cf, "send: nothing to do in this session");
      result = CURLE_HTTP2;
    }
  }

out:
  എങ്കിൽ(stream) {
    CURL_TRC_CF(data, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
                "eos=%d, h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                stream->id, len, (പൂർണ്ണസംഖ്യ)result, *pnwritten,
                stream->body_eos,
                Curl_ngh2_session_get_stream_remote_window_size(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&stream->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  അല്ലെങ്കിൽ {
    CURL_TRC_CF(data, cf, "cf_send(len=%zu) -> %d, %zu, "
                "connection-window=%d, nw_send_buffer(%zu)",
                len, (പൂർണ്ണസംഖ്യ)result, *pnwritten,
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_DATA_RESTORE(cf, save);
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_h2_flush(ഘടന Curl_cfilter *cf,
                            ഘടന Curl_easy *data)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  ഘടന h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
  ഘടന cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  എങ്കിൽ(stream && !Curl_bufq_is_empty(&stream->sendbuf)) {
    /* resume the potentially suspended stream */
    എങ്കിൽ(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id))) {
      result = CURLE_SEND_ERROR;
      പോകുക out;
    }
  }

  result = h2_progress_egress(cf, data);

out:
  എങ്കിൽ(stream) {
    CURL_TRC_CF(data, cf, "[%d] flush -> %d, "
                "h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                stream->id, (പൂർണ്ണസംഖ്യ)result,
                Curl_ngh2_session_get_stream_remote_window_size(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&stream->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  അല്ലെങ്കിൽ {
    CURL_TRC_CF(data, cf, "flush -> %d, "
                "connection-window=%d, nw_send_buffer(%zu)", (പൂർണ്ണസംഖ്യ)result,
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_DATA_RESTORE(cf, save);
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_h2_adjust_pollset(ഘടന Curl_cfilter *cf,
                                     ഘടന Curl_easy *data,
                                     ഘടന easy_pollset *ps)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  ഘടന cf_call_data save;
  curl_socket_t sock;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(!Curl_ngh2_valid(&ctx->ngh2))
    തിരികെ CURLE_OK;

  sock = Curl_conn_cf_get_socket(cf, data);
  Curl_pollset_check(data, ps, sock, &want_recv, &want_send);
  എങ്കിൽ(want_recv || want_send) {
    ഘടന h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
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
  അല്ലെങ്കിൽ എങ്കിൽ(ctx->sent_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_DATA_SAVE(save, cf, data);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CF_DATA_RESTORE(cf, save);
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_h2_ctx_open(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  ഘടന h2_stream_ctx *stream;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  പൂർണ്ണസംഖ്യ rc;
  nghttp2_session_callbacks *cbs = ശൂന്യ;

  DEBUGASSERT(!ctx->ngh2.s);
  DEBUGASSERT(!ctx->ngh2.fatal_rv);
  DEBUGASSERT(ctx->initialized);

  rc = nghttp2_session_callbacks_new(&cbs);
  എങ്കിൽ(rc) {
    failf(data, "Could not initialize nghttp2 callbacks");
    പോകുക out;
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
  എങ്കിൽ(rc) {
    failf(data, "Could not initialize nghttp2");
    പോകുക out;
  }
  ctx->max_concurrent_streams = data->multi ?
    Curl_multi_max_concurrent_streams(data->multi) :
    DEFAULT_MAX_CONCURRENT_STREAMS;

  എങ്കിൽ(ctx->via_h1_upgrade) {
    /* HTTP/1.1 Upgrade issued. H2 Settings have already been submitted
     * in the H1 request and we upgrade from there. This stream
     * is opened implicitly as #1. */
    uint8_t binsettings[H2_BINSETTINGS_LEN];
    ssize_t rclen;
    size_t binlen; /* length of the binsettings data */

    rclen = populate_binsettings(binsettings, data);

    എങ്കിൽ(!curlx_sztouz(rclen, &binlen) || !binlen) {
      failf(data, "nghttp2 unexpectedly failed on pack_settings_payload");
      result = CURLE_FAILED_INIT;
      പോകുക out;
    }

    result = http2_data_setup(cf, data, &stream);
    എങ്കിൽ(result)
      പോകുക out;
    DEBUGASSERT(stream);
    stream->id = 1;
    /* queue SETTINGS frame (again) */
    rc = Curl_ngh2_session_upgrade2(
      &ctx->ngh2, binsettings, binlen, data->state.httpreq == HTTPREQ_HEAD,
      ശൂന്യ);
    എങ്കിൽ(rc) {
      failf(data, "nghttp2_session_upgrade2() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      പോകുക out;
    }

    rc = Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, stream->id, stream);
    എങ്കിൽ(rc) {
      infof(data, "http/2: failed to set user_data for stream %d",
            stream->id);
      DEBUGASSERT(0);
    }
    CURL_TRC_CF(data, cf, "created session via Upgrade");
  }
  അല്ലെങ്കിൽ {
    nghttp2_settings_entry iv[H2_SETTINGS_IV_LEN];
    size_t ivlen;

    ivlen = populate_settings(iv, data, ctx);
    rc = Curl_ngh2_submit_settings(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, iv, ivlen);
    എങ്കിൽ(rc) {
      failf(data, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      പോകുക out;
    }
  }

  rc = Curl_ngh2_session_set_local_window_size(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, HTTP2_HUGE_WINDOW_SIZE);
  എങ്കിൽ(rc) {
    failf(data, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    result = CURLE_HTTP2;
    പോകുക out;
  }

  /* all set, traffic will be send on connect */
  result = CURLE_OK;
  CURL_TRC_CF(data, cf, "[0] created h2 session%s",
              ctx->via_h1_upgrade ? " (via h1 upgrade)" : "");

out:
  എങ്കിൽ(cbs)
    nghttp2_session_callbacks_del(cbs);
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_h2_connect(ഘടന Curl_cfilter *cf,
                              ഘടന Curl_easy *data,
                              bool *done)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ഘടന cf_call_data save;
  bool first_time = FALSE;

  എങ്കിൽ(cf->connected) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  /* Connect the lower filters first */
  എങ്കിൽ(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    എങ്കിൽ(result || !*done)
      തിരികെ result;
  }

  *done = FALSE;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(ctx->initialized);
  എങ്കിൽ(!ctx->ngh2.s) {
    result = cf_h2_ctx_open(cf, data);
    എങ്കിൽ(result)
      പോകുക out;
    first_time = TRUE;
  }

  എങ്കിൽ(!first_time) {
    result = h2_progress_ingress(cf, data, 0);
    എങ്കിൽ(result)
      പോകുക out;
  }

  /* Send out our SETTINGS and ACKs and such. If that blocks, we
   * have it buffered and can count this filter as being connected */
  result = h2_progress_egress(cf, data);
  എങ്കിൽ(result && (result != CURLE_AGAIN))
    പോകുക out;

  *done = TRUE;
  cf->connected = TRUE;
  result = CURLE_OK;

out:
  CURL_TRC_CF(data, cf, "cf_connect() -> %d, %d, ", (പൂർണ്ണസംഖ്യ)result, *done);
  CF_DATA_RESTORE(cf, save);
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം cf_h2_destroy(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;

  (ശൂന്യം)data;
  എങ്കിൽ(ctx) {
    cf_h2_ctx_free(ctx);
    cf->ctx = ശൂന്യ;
  }
}

സ്ഥിതം CURLcode cf_h2_shutdown(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data, bool *done)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  ഘടന cf_call_data save;
  CURLcode result;
  പൂർണ്ണസംഖ്യ rv;

  എങ്കിൽ(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);

  എങ്കിൽ(!ctx->sent_goaway) {
    ctx->sent_goaway = TRUE;
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, ctx->local_max_sid, 0,
      (സ്ഥിരം uint8_t *)"shutdown", വലുപ്പം("shutdown"));
    എങ്കിൽ(rv) {
      failf(data, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      result = CURLE_SEND_ERROR;
      പോകുക out;
    }
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  result = CURLE_OK;
  എങ്കിൽ(Curl_ngh2_session_want_write(&ctx->ngh2) ||
     !Curl_bufq_is_empty(&ctx->outbufq))
    result = h2_progress_egress(cf, data);
  എങ്കിൽ(!result && Curl_ngh2_session_want_read(&ctx->ngh2))
    result = h2_progress_ingress(cf, data, 0);

  എങ്കിൽ(result == CURLE_AGAIN)
    result = CURLE_OK;

  *done = (ctx->conn_closed ||
           (!result && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2) &&
            Curl_bufq_is_empty(&ctx->outbufq)));

out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  തിരികെ result;
}

സ്ഥിതം CURLcode http2_data_pause(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 bool pause)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  ഘടന h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);

  DEBUGASSERT(data);
  എങ്കിൽ(ctx && Curl_ngh2_valid(&ctx->ngh2) && stream) {
    CURLcode result;

    stream->write_paused = pause;
    result = cf_h2_update_local_win(cf, data, stream);
    എങ്കിൽ(result)
      തിരികെ result;

    /* attempt to send the window update */
    (ശൂന്യം)h2_progress_egress(cf, data);

    എങ്കിൽ(!pause) {
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
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode cf_h2_cntrl(ഘടന Curl_cfilter *cf,
                            ഘടന Curl_easy *data,
                            പൂർണ്ണസംഖ്യ event, പൂർണ്ണസംഖ്യ arg1, ശൂന്യം *arg2)
{
  CURLcode result = CURLE_OK;
  ഘടന cf_call_data save;

  (ശൂന്യം)arg2;

  CF_DATA_SAVE(save, cf, data);
  തിരഞ്ഞെടുപ്പ്(event) {
  അവസ്ഥ CF_CTRL_DATA_SETUP:
    നിർത്തുക;
  അവസ്ഥ CF_CTRL_DATA_PAUSE:
    result = http2_data_pause(cf, data, (arg1 != 0));
    നിർത്തുക;
  അവസ്ഥ CF_CTRL_FLUSH:
    result = cf_h2_flush(cf, data);
    നിർത്തുക;
  അവസ്ഥ CF_CTRL_DATA_DONE:
    http2_data_done(cf, data);
    നിർത്തുക;
  അവസ്ഥ CF_CTRL_CONN_INFO_UPDATE:
    എങ്കിൽ(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 20;
      Curl_conn_set_multiplex(cf->conn);
    }
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  CF_DATA_RESTORE(cf, save);
  തിരികെ result;
}

സ്ഥിതം bool cf_h2_data_pending(ഘടന Curl_cfilter *cf,
                               സ്ഥിരം ഘടന Curl_easy *data)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;

  എങ്കിൽ(ctx && !Curl_bufq_is_empty(&ctx->inbufq))
    തിരികെ TRUE;
  തിരികെ cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

സ്ഥിതം bool cf_h2_is_alive(ഘടന Curl_cfilter *cf,
                           ഘടന Curl_easy *data,
                           bool *നിവേശം_pending)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  bool alive;
  ഘടന cf_call_data save;

  *നിവേശം_pending = FALSE;
  CF_DATA_SAVE(save, cf, data);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
           http2_connisalive(cf, data, നിവേശം_pending));
  CURL_TRC_CF(data, cf, "conn alive -> %d, input_pending=%d",
              alive, *നിവേശം_pending);
  CF_DATA_RESTORE(cf, save);
  തിരികെ alive;
}

സ്ഥിതം CURLcode cf_h2_keep_alive(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data)
{
  CURLcode result;
  ഘടന cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  result = http2_send_ping(cf, data);
  CF_DATA_RESTORE(cf, save);
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_h2_query(ഘടന Curl_cfilter *cf,
                            ഘടന Curl_easy *data,
                            പൂർണ്ണസംഖ്യ query, പൂർണ്ണസംഖ്യ *pres1, ശൂന്യം *pres2)
{
  ഘടന cf_h2_ctx *ctx = cf->ctx;
  ഘടന cf_call_data save;
  size_t effective_max;

  തിരഞ്ഞെടുപ്പ്(query) {
  അവസ്ഥ CF_QUERY_MAX_CONCURRENT:
    DEBUGASSERT(pres1);

    CF_DATA_SAVE(save, cf, data);
    എങ്കിൽ(!Curl_ngh2_valid(&ctx->ngh2) ||
       !Curl_ngh2_session_check_request_allowed(&ctx->ngh2)) {
      /* the limit is what we have in use right now */
      effective_max = cf->conn->attached_xfers;
    }
    അല്ലെങ്കിൽ {
      effective_max = ctx->max_concurrent_streams;
    }
    *pres1 = (effective_max > INT_MAX) ? INT_MAX : (പൂർണ്ണസംഖ്യ)effective_max;
    CF_DATA_RESTORE(cf, save);
    തിരികെ CURLE_OK;
  അവസ്ഥ CF_QUERY_STREAM_ERROR: {
    ഘടന h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    *pres1 = stream ? (പൂർണ്ണസംഖ്യ)stream->error : 0;
    തിരികെ CURLE_OK;
  }
  അവസ്ഥ CF_QUERY_NEED_FLUSH: {
    ഘടന h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    എങ്കിൽ(!Curl_bufq_is_empty(&ctx->outbufq) ||
       (stream && !Curl_bufq_is_empty(&stream->sendbuf))) {
      *pres1 = TRUE;
      തിരികെ CURLE_OK;
    }
    നിർത്തുക;
  }
  അവസ്ഥ CF_QUERY_HTTP_VERSION:
    *pres1 = 20;
    തിരികെ CURLE_OK;
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ഘടന Curl_cftype Curl_cft_nghttp2 = {
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

സ്ഥിതം CURLcode http2_cfilter_add(ഘടന Curl_cfilter **pcf,
                                  ഘടന Curl_easy *data,
                                  ഘടന connectdata *conn,
                                  int8_t sockindex,
                                  bool via_h1_upgrade)
{
  ഘടന Curl_cfilter *cf = ശൂന്യ;
  ഘടന cf_h2_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(data->conn);
  ctx = curlx_calloc(1, വലുപ്പം(*ctx));
  എങ്കിൽ(!ctx)
    പോകുക out;
  cf_h2_ctx_init(ctx, via_h1_upgrade);

  result = Curl_cf_create(&cf, &Curl_cft_nghttp2, ctx);
  എങ്കിൽ(result)
    പോകുക out;

  ctx = ശൂന്യ;
  Curl_conn_cf_add(data, conn, sockindex, cf);

out:
  എങ്കിൽ(result)
    cf_h2_ctx_free(ctx);
  *pcf = result ? ശൂന്യ : cf;
  തിരികെ result;
}

സ്ഥിതം CURLcode http2_cfilter_തിരുകുക_after(ഘടന Curl_cfilter *cf,
                                           ഘടന Curl_easy *data,
                                           bool via_h1_upgrade)
{
  ഘടന Curl_cfilter *cf_h2 = ശൂന്യ;
  ഘടന cf_h2_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (ശൂന്യം)data;
  ctx = curlx_calloc(1, വലുപ്പം(*ctx));
  എങ്കിൽ(!ctx)
    പോകുക out;
  cf_h2_ctx_init(ctx, via_h1_upgrade);

  result = Curl_cf_create(&cf_h2, &Curl_cft_nghttp2, ctx);
  എങ്കിൽ(result)
    പോകുക out;

  ctx = ശൂന്യ;
  Curl_conn_cf_തിരുകുക_after(cf, cf_h2);

out:
  എങ്കിൽ(result)
    cf_h2_ctx_free(ctx);
  തിരികെ result;
}

bool Curl_http2_may_switch(ഘടന Curl_easy *data)
{
  എങ്കിൽ(Curl_conn_http_version(data, data->conn) < 20 &&
     (data->state.http_neg.wanted & CURL_HTTP_V2x) &&
     data->state.http_neg.h2_prior_knowledge) {
#ifndef CURL_DISABLE_PROXY
    എങ്കിൽ(data->conn->bits.origin_is_proxy) {
      infof(data, "Ignoring HTTP/2 prior knowledge due to proxy");
      തിരികെ FALSE;
    }
#endif
    തിരികെ TRUE;
  }
  തിരികെ FALSE;
}

CURLcode Curl_http2_switch(ഘടന Curl_easy *data)
{
  ഘടന Curl_cfilter *cf;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, data->conn) < 20);

  result = http2_cfilter_add(&cf, data, data->conn, FIRSTSOCKET, FALSE);
  എങ്കിൽ(result)
    തിരികെ result;
  CURL_TRC_CF(data, cf, "switching connection to HTTP/2");

  എങ്കിൽ(cf->next) {
    bool done;
    തിരികെ Curl_conn_cf_connect(cf, data, &done);
  }
  തിരികെ CURLE_OK;
}

CURLcode Curl_http2_switch_at(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data)
{
  ഘടന Curl_cfilter *cf_h2;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, data->conn) < 20);

  result = http2_cfilter_തിരുകുക_after(cf, data, FALSE);
  എങ്കിൽ(result)
    തിരികെ result;

  cf_h2 = cf->next;

  എങ്കിൽ(cf_h2->next) {
    bool done;
    തിരികെ Curl_conn_cf_connect(cf_h2, data, &done);
  }
  തിരികെ CURLE_OK;
}

CURLcode Curl_http2_upgrade(ഘടന Curl_easy *data,
                            ഘടന connectdata *conn, int8_t sockindex,
                            സ്ഥിരം അക്ഷരം *mem, size_t nread)
{
  ഘടന Curl_cfilter *cf;
  ഘടന cf_h2_ctx *ctx;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, conn) < 20);

  result = http2_cfilter_add(&cf, data, conn, sockindex, TRUE);
  എങ്കിൽ(result)
    തിരികെ result;
  CURL_TRC_CF(data, cf, "upgrading connection to HTTP/2");

  DEBUGASSERT(cf->cft == &Curl_cft_nghttp2);
  ctx = cf->ctx;

  data->req.httpversion_sent = 20; /* it is an h2 request now */
  data->req.header = TRUE; /* we expect the real response to come in h2 */
  data->req.headerline = 0; /* restart the header line counter */

  എങ്കിൽ(nread > 0) {
    /* Remaining data from the protocol switch reply is already using
     * the switched protocol, ie. HTTP/2. We add that to the network
     * inbufq. */
    size_t copied;

    result = Curl_bufq_write(&ctx->inbufq,
                             (സ്ഥിരം unsigned അക്ഷരം *)mem, nread, &copied);
    എങ്കിൽ(result) {
      failf(data, "error on copying HTTP Upgrade response: %d", (പൂർണ്ണസംഖ്യ)result);
      തിരികെ CURLE_RECV_ERROR;
    }
    എങ്കിൽ(copied < nread) {
      failf(data, "connection buffer size could not take all data "
            "from HTTP Upgrade response header: copied=%zu, datalen=%zu",
            copied, nread);
      തിരികെ CURLE_HTTP2;
    }
    infof(data, "Copied HTTP/2 data in stream buffer to connection buffer"
          " after upgrade: len=%zu", nread);
  }

  എങ്കിൽ(cf->next) {
    bool done;
    result = Curl_conn_cf_connect(cf, data, &done);
    എങ്കിൽ(!result)
      cf->cft->cntrl(cf, data, CF_CTRL_CONN_INFO_UPDATE, 0, ശൂന്യ);
  }
  തിരികെ result;
}

/* Only call this function for a transfer that already got an HTTP/2
   CURLE_HTTP2_STREAM error! */
bool Curl_h2_http_1_1_error(ഘടന Curl_easy *data)
{
  എങ്കിൽ(Curl_conn_http_version(data, data->conn) == 20) {
    പൂർണ്ണസംഖ്യ err = Curl_conn_get_stream_error(data, data->conn, FIRSTSOCKET);
    തിരികെ err == NGHTTP2_HTTP_1_1_REQUIRED;
  }
  തിരികെ FALSE;
}

ശൂന്യം *Curl_nghttp2_malloc(size_t size, ശൂന്യം *user_data)
{
  (ശൂന്യം)user_data;
  തിരികെ Curl_cmalloc(size);
}

ശൂന്യം Curl_nghttp2_free(ശൂന്യം *ptr, ശൂന്യം *user_data)
{
  (ശൂന്യം)user_data;
  Curl_cfree(ptr);
}

ശൂന്യം *Curl_nghttp2_calloc(size_t nmemb, size_t size, ശൂന്യം *user_data)
{
  (ശൂന്യം)user_data;
  തിരികെ Curl_ccalloc(nmemb, size);
}

ശൂന്യം *Curl_nghttp2_realloc(ശൂന്യം *ptr, size_t size, ശൂന്യം *user_data)
{
  (ശൂന്യം)user_data;
  തിരികെ Curl_crealloc(ptr, size);
}

#അല്ലെങ്കിൽ /* CURL_DISABLE_HTTP || !USE_NGHTTP2 */

/* Satisfy external references even if http2 is not compiled in. */

അക്ഷരം *curl_pushheader_bynum(ഘടന curl_pushheaders *h, size_t അക്കം)
{
  (ശൂന്യം)h;
  (ശൂന്യം)അക്കം;
  തിരികെ ശൂന്യ;
}

അക്ഷരം *curl_pushheader_byname(ഘടന curl_pushheaders *h, സ്ഥിരം അക്ഷരം *name)
{
  (ശൂന്യം)h;
  (ശൂന്യം)name;
  തിരികെ ശൂന്യ;
}

#endif /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
