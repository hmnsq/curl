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
#અન્તર્ભાવ "curl_setup.h"

#જો !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#અન્તર્ભાવ <nghttp2/nghttp2.h>

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "bufq.h"
#અન્તર્ભાવ "u32_ptrset.h"
#અન્તર્ભાવ "http1.h"
#અન્તર્ભાવ "http2.h"
#અન્તર્ભાવ "http.h"
#અન્તર્ભાવ "sendf.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "curlx/base64.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "ngh2_shim.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "urlapi-int.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "transfer.h"
#અન્તર્ભાવ "bufref.h"
#અન્તર્ભાવ "curlx/dynbuf.h"
#અન્તર્ભાવ "headers.h"
#અન્તર્ભાવ "curl_share.h"

#જો NGHTTP2_VERSION_અંક < 0x010f00
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
#જો NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
#define H2_STREAM_WINDOW_SIZE_INITIAL  (64 * 1024)
#નહીંતર
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

સંરચના cf_h2_ctx {
  સંરચના curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  સંરચના cf_call_data call_data;

  સંરચના bufq inbufq;           /* network input */
  સંરચના bufq outbufq;          /* network output */
  સંરચના bufc_pool stream_bufcp; /* spares for stream buffers */
  સંરચના dynbuf scratch;        /* scratch buffer for temp use */

  સંરચના u32_ptrset streams; /* hash of `data->mid` to `h2_stream_ctx` */
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
  BIT(enable_ધકેલો);
  BIT(nw_out_blocked);
};

/* How to access `call_data` from a cf_h2 filter */
#undef CF_CTX_CALL_DATA
#define CF_CTX_CALL_DATA(cf) ((સંરચના cf_h2_ctx *)(cf)->ctx)->call_data

/**
 * All about the H2 internals of a stream
 */
સંરચના h2_stream_ctx {
  સંરચના bufq sendbuf; /* request buffer */
  સંરચના h1_req_parser h1; /* parsing the request */
  સંરચના dynhds resp_trailers; /* response trailer fields */
  size_t resp_hds_len; /* amount of response header bytes in recvbuf */
  curl_off_t xfer_id;  /* transfer `id` owning this stream */
  curl_off_t nrcvd_data;  /* number of DATA bytes received */

  અક્ષર **ધકેલો_headers;       /* allocated array */
  size_t ધકેલો_headers_used;  /* number of entries filled in */
  size_t ધકેલો_headers_alloc; /* number of entries allocated */

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

સ્થિર શૂન્ય free_ધકેલો_headers(સંરચના h2_stream_ctx *stream)
{
  size_t i;
  માટે(i = 0; i < stream->ધકેલો_headers_used; i++)
    curlx_free(stream->ધકેલો_headers[i]);
  curlx_safefree(stream->ધકેલો_headers);
  stream->ધકેલો_headers_used = 0;
}

સ્થિર શૂન્ય h2_stream_ctx_free(સંરચના h2_stream_ctx *stream)
{
  Curl_bufq_free(&stream->sendbuf);
  Curl_h1_req_parse_free(&stream->h1);
  Curl_dynhds_free(&stream->resp_trailers);
  free_ધકેલો_headers(stream);
  curlx_free(stream);
}

સ્થિર શૂન્ય h2_stream_hash_free(uint32_t id, શૂન્ય *stream)
{
  (શૂન્ય)id;
  DEBUGASSERT(stream);
  h2_stream_ctx_free((સંરચના h2_stream_ctx *)stream);
}

સ્થિર શૂન્ય cf_h2_ctx_init(સંરચના cf_h2_ctx *ctx, bool via_h1_upgrade)
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

સ્થિર શૂન્ય cf_h2_ctx_free(સંરચના cf_h2_ctx *ctx)
{
  જો(ctx && ctx->initialized) {
    Curl_ngh2_del(&ctx->ngh2);
    Curl_bufq_free(&ctx->inbufq);
    Curl_bufq_free(&ctx->outbufq);
    Curl_bufcp_free(&ctx->stream_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    memset(ctx, 0, કદ(*ctx));
  }
  curlx_free(ctx);
}

સ્થિર uint32_t cf_h2_initial_win_size(સંરચના Curl_easy *data)
{
#જો NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
  /* If the transfer has a rate-limit lower than the default initial
   * stream window size, use that. It needs to be at least 8k or servers
   * may be unhappy. */
  curl_off_t rps = Curl_rlimit_per_step(&data->progress.dl.rlimit);
  જો((rps > 0) && (rps < H2_STREAM_WINDOW_SIZE_INITIAL))
    પાછા CURLMAX((uint32_t)rps, 8192);
#endif
  પાછા H2_STREAM_WINDOW_SIZE_INITIAL;
}

સ્થિર size_t populate_settings(nghttp2_settings_entry *iv,
                                સંરચના Curl_easy *data,
                                સંરચના cf_h2_ctx *ctx)
{
  iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
  iv[0].કિંમત = Curl_multi_max_concurrent_streams(data->multi);

  iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
  iv[1].કિંમત = cf_h2_initial_win_size(data);
  જો(ctx)
    ctx->initial_win_size = iv[1].કિંમત;
  iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_ધકેલો;
  iv[2].કિંમત = !!data->multi->ધકેલો_cb;

  પાછા 3;
}

સ્થિર ssize_t populate_binsettings(uint8_t *binsettings,
                                    સંરચના Curl_easy *data)
{
  nghttp2_settings_entry iv[H2_SETTINGS_IV_LEN];
  size_t ivlen;

  ivlen = populate_settings(iv, data, ખાલી);
  /* this returns number of bytes it wrote or a negative number on error. */
  પાછા nghttp2_pack_settings_payload(binsettings, H2_BINSETTINGS_LEN,
                                       iv, ivlen);
}

સ્થિર CURLcode cf_h2_update_settings(સંરચના cf_h2_ctx *ctx,
                                      uint32_t initial_win_size)
{
  nghttp2_settings_entry entry;
  entry.settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
  entry.કિંમત = initial_win_size;
  જો(Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, &entry, 1))
    પાછા CURLE_SEND_ERROR;
  ctx->initial_win_size = initial_win_size;
  પાછા CURLE_OK;
}

#define H2_STREAM_CTX(ctx, data)                                         \
  ((સંરચના h2_stream_ctx *)(                                             \
    (data) ? Curl_u32_ptrset_get(&(ctx)->streams, (data)->mid) : ખાલી))

સ્થિર સંરચના h2_stream_ctx *h2_stream_ctx_create(સંરચના cf_h2_ctx *ctx,
                                                  સંરચના Curl_easy *data)
{
  સંરચના h2_stream_ctx *stream;

  stream = curlx_calloc(1, કદ(*stream));
  જો(!stream)
    પાછા ખાલી;

  stream->id = -1;
  stream->xfer_id = data->id;
  stream->mid = data->mid;
  Curl_bufq_initp(&stream->sendbuf, &ctx->stream_bufcp,
                  H2_STREAM_SEND_CHUNKS, BUFQ_OPT_NONE);
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_લીટી_LEN);
  Curl_dynhds_init(&stream->resp_trailers,
                   MAX_HTTP_RESP_HEADER_ગણતરી, DYN_HTTP_REQUEST);
  stream->bodystarted = FALSE;
  stream->status_code = -1;
  stream->closed = FALSE;
  stream->close_handled = FALSE;
  stream->error = NGHTTP2_NO_ERROR;
  stream->local_window_size = H2_STREAM_WINDOW_SIZE_INITIAL;
  stream->nrcvd_data = 0;
  પાછા stream;
}

સ્થિર સંરચના Curl_easy *cf_h2_get_stream_xfer(સંરચના Curl_cfilter *cf,
                                               int32_t stream_id,
                                               સંરચના h2_stream_ctx **pstream)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  સંરચના h2_stream_ctx *stream =
    Curl_ngh2_session_get_stream_user_data(&ctx->ngh2, stream_id);
  જો(pstream)
    *pstream = stream;
  જો(stream && (stream->mid != UINT32_MAX)) {
    સંરચના Curl_easy *data, *call_data = CF_DATA_CURRENT(cf);
    જો(!call_data || !call_data->multi) {
      DEBUGASSERT(0);
      પાછા ખાલી;
    }
    data = Curl_multi_get_easy(call_data->multi, stream->mid);
    જો(data && (data->id == stream->xfer_id))
      પાછા data;
  }
  પાછા ખાલી;
}

#ifdef NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
સ્થિર int32_t cf_h2_get_desired_local_win(સંરચના Curl_cfilter *cf,
                                           સંરચના Curl_easy *data)
{
  curl_off_t avail = Curl_rlimit_avail(&data->progress.dl.rlimit, ખાલી);

  (શૂન્ય)cf;
  જો(avail < CURL_OFF_T_MAX) { /* limit in place */
    જો(avail <= 0)
      પાછા 0;
    નહીંતર જો(avail < H2_STREAM_WINDOW_SIZE_MAX)
      પાછા (int32_t)avail;
  }
  પાછા H2_STREAM_WINDOW_SIZE_MAX;
}

સ્થિર CURLcode cf_h2_update_local_win(સંરચના Curl_cfilter *cf,
                                       સંરચના Curl_easy *data,
                                       સંરચના h2_stream_ctx *stream)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  int32_t dwsize;
  પૂર્ણાંક rv;

  dwsize = (stream->write_paused || stream->xfer_result) ?
           0 : cf_h2_get_desired_local_win(cf, data);
  જો(dwsize != stream->local_window_size) {
    int32_t wsize = Curl_ngh2_session_get_stream_effective_local_window_size(
      &ctx->ngh2, stream->id);
    જો(dwsize > wsize) {
      rv = Curl_ngh2_session_set_local_window_size(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize);
      જો(rv) {
        failf(data, "[%d] nghttp2 set_local_window_size(%d) failed: "
              "%s(%d)", stream->id, dwsize, nghttp2_strerror(rv), rv);
        પાછા CURLE_HTTP2;
      }
      rv = Curl_ngh2_submit_window_update(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize - wsize);
      જો(rv) {
        failf(data, "[%d] nghttp2_submit_window_update() failed: "
              "%s(%d)", stream->id, nghttp2_strerror(rv), rv);
        પાછા CURLE_HTTP2;
      }
      stream->local_window_size = dwsize;
      CURL_TRC_CF(data, cf, "[%d] local window update by %d",
                  stream->id, dwsize - wsize);
    }
    નહીંતર {
      rv = Curl_ngh2_session_set_local_window_size(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize);
      જો(rv) {
        failf(data, "[%d] nghttp2_session_set_local_window_size() failed: "
              "%s(%d)", stream->id, nghttp2_strerror(rv), rv);
        પાછા CURLE_HTTP2;
      }
      stream->local_window_size = dwsize;
      CURL_TRC_CF(data, cf, "[%d] local window size now %d",
                  stream->id, dwsize);
    }
  }
  પાછા CURLE_OK;
}

#નહીંતર /* NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

સ્થિર CURLcode cf_h2_update_local_win(સંરચના Curl_cfilter *cf,
                                       સંરચના Curl_easy *data,
                                       સંરચના h2_stream_ctx *stream)
{
  (શૂન્ય)cf;
  (શૂન્ય)data;
  (શૂન્ય)stream;
  પાછા CURLE_OK;
}
#endif /* !NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

સ્થિર CURLcode http2_data_setup(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 સંરચના h2_stream_ctx **pstream)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  સંરચના h2_stream_ctx *stream;

  (શૂન્ય)cf;
  DEBUGASSERT(data);

  જો(!data)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  stream = H2_STREAM_CTX(ctx, data);
  જો(stream) {
    *pstream = stream;
    પાછા CURLE_OK;
  }

  stream = h2_stream_ctx_create(ctx, data);
  જો(!stream)
    પાછા CURLE_OUT_OF_MEMORY;

  જો(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    h2_stream_ctx_free(stream);
    પાછા CURLE_OUT_OF_MEMORY;
  }

  *pstream = stream;
  પાછા CURLE_OK;
}

સ્થિર CURLcode nw_out_flush(સંરચના Curl_cfilter *cf,
                             સંરચના Curl_easy *data)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  જો(Curl_bufq_is_empty(&ctx->outbufq))
    પાછા CURLE_OK;

  result = Curl_cf_send_bufq(cf->next, data, &ctx->outbufq, ખાલી, 0,
                             &nwritten);
  જો(result) {
    જો(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_out_blocked = 1;
    }
    પાછા result;
  }
  પાછા Curl_bufq_is_empty(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

સ્થિર શૂન્ય http2_data_done(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  સંરચના h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);

  DEBUGASSERT(ctx);
  જો(!stream || !ctx->initialized)
    પાછા;

  જો(Curl_ngh2_valid(&ctx->ngh2)) {
    bool flush_egress = FALSE;
    /* returns error if stream not known, which is fine here */
    (શૂન્ય)Curl_ngh2_session_set_stream_user_data(&ctx->ngh2, stream->id, ખાલી);

    જો(!stream->closed && stream->id > 0) {
      /* RST_STREAM */
      CURL_TRC_CF(data, cf, "[%d] premature DATA_DONE, RST stream",
                  stream->id);
      stream->closed = TRUE;
      stream->reset = TRUE;
      Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                  stream->id, NGHTTP2_STREAM_CLOSED);
      flush_egress = TRUE;
    }

    જો(flush_egress) {
      (શૂન્ય)Curl_ngh2_session_send(&ctx->ngh2);
      (શૂન્ય)nw_out_flush(cf, data);
    }
  }

  Curl_u32_ptrset_unset(&ctx->streams, data->mid);
}

સ્થિર પૂર્ણાંક h2_client_new(સંરચના Curl_cfilter *cf,
                         nghttp2_session_callbacks *cbs)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { ખાલી, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  પૂર્ણાંક rc = nghttp2_option_new(&o);
  જો(rc)
    પાછા rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_set_no_auto_window_update(o, 1);
#જો NGHTTP2_VERSION_અંક >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_set_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#endif
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  પાછા rc;
}

/*
 * Returns nonzero if current HTTP/2 session should be closed.
 */
સ્થિર પૂર્ણાંક should_close_session(સંરચના cf_h2_ctx *ctx)
{
  પાછા ctx->drain_total == 0 && !Curl_ngh2_session_want_read(&ctx->ngh2) &&
    !Curl_ngh2_session_want_write(&ctx->ngh2);
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
સ્થિર CURLcode h2_process_pending_આગત(સંરચના Curl_cfilter *cf,
                                         સંરચના Curl_easy *data)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  અચલ unsigned અક્ષર *buf;
  size_t blen, nread;
  ssize_t rv;

  જ્યારે(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {
    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (અચલ uint8_t *)buf, blen);
    જો(!curlx_sztouz(rv, &nread)) {
      failf(data, "nghttp2 recv error %zd: %s", rv, nghttp2_strerror((પૂર્ણાંક)rv));
      પાછા CURLE_RECV_ERROR;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    જો(Curl_bufq_is_empty(&ctx->inbufq)) {
      તોડો;
    }
    નહીંતર {
      CURL_TRC_CF(data, cf, "process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }

  જો(Curl_ngh2_session_check_request_allowed(&ctx->ngh2) == 0) {
    /* No more requests are allowed in the current session, so
       the connection may not be reused. This is set when a
       GOAWAY frame has been received or when the limit of stream
       identifiers has been reached. */
    CURL_TRC_M(data, "http/2: No new requests allowed");
    connclose(cf->conn);
  }

  પાછા CURLE_OK;
}

/*
 * The server may send us data at any point (e.g. PING frames). Therefore, we
 * cannot assume that an HTTP/2 socket is dead because it is readable.
 *
 * Check the lower filters first and, if successful, peek at the socket
 * and distinguish between closed and data.
 */
સ્થિર bool http2_connisalive(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                              bool *આગત_pending)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *આગત_pending = FALSE;
  જો(!cf->next || !cf->next->cft->is_alive(cf->next, data, આગત_pending))
    પાછા FALSE;

  જો(*આગત_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode result;
    size_t nread;

    *આગત_pending = FALSE;
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(data, cf, "connisalive, recv pending input -> %d, %zu",
                (પૂર્ણાંક)result, nread);
    જો(!result) {
      result = h2_process_pending_આગત(cf, data);
      જો(result)
        /* immediate error, considered dead */
        alive = FALSE;
      નહીંતર {
        alive = !should_close_session(ctx);
      }
    }
    નહીંતર જો(result != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  પાછા alive;
}

સ્થિર CURLcode http2_send_ping(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  પૂર્ણાંક rc;

  rc = Curl_ngh2_submit_ping(&ctx->ngh2, 0, ZERO_NULL);
  જો(rc) {
    failf(data, "nghttp2_submit_ping() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    પાછા CURLE_HTTP2;
  }

  rc = Curl_ngh2_session_send(&ctx->ngh2);
  જો(rc) {
    failf(data, "nghttp2_session_send() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    પાછા CURLE_SEND_ERROR;
  }
  પાછા CURLE_OK;
}

/*
 * Store nghttp2 version info in this buffer.
 */
શૂન્ય Curl_http2_ver(અક્ષર *p, size_t len)
{
  nghttp2_info *h2 = nghttp2_version(0);
  (શૂન્ય)curl_msnprintf(p, len, "nghttp2/%s", h2->version_str);
}

/*
 * The implementation of nghttp2_send_callback type. Here we write |data| with
 * size |length| to the network and return the number of bytes actually
 * written. See the documentation of nghttp2_send_callback for the details.
 */
સ્થિર ssize_t send_callback(nghttp2_session *h2,
                             અચલ uint8_t *buf, size_t blen, પૂર્ણાંક flags,
                             શૂન્ય *userp)
{
  સંરચના Curl_cfilter *cf = userp;
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result = CURLE_OK;

  (શૂન્ય)h2;
  (શૂન્ય)flags;
  DEBUGASSERT(data);

  જો(!cf->connected)
    result = Curl_bufq_write(&ctx->outbufq, buf, blen, &nwritten);
  નહીંતર
    result = Curl_cf_send_bufq(cf->next, data, &ctx->outbufq, buf, blen,
                               &nwritten);

  જો(result) {
    જો(result == CURLE_AGAIN) {
      ctx->nw_out_blocked = 1;
      પાછા NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(data, "Failed sending HTTP2 data");
    પાછા NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  જો(!nwritten) {
    ctx->nw_out_blocked = 1;
    પાછા NGHTTP2_ERR_WOULDBLOCK;
  }
  પાછા (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

/* We pass a pointer to this struct in the push callback, but the contents of
   the struct are hidden from the user. */
સંરચના curl_pushheaders {
  સંરચના Curl_easy *data;
  સંરચના h2_stream_ctx *stream;
  અચલ nghttp2_ધકેલો_promise *frame;
};

/*
 * push header access function. Only to be used from within the push callback
 */
અક્ષર *curl_pushheader_bynum(સંરચના curl_pushheaders *h, size_t અંક)
{
  /* Verify that we got a good easy handle in the push header struct, mostly to
     detect rubbish input fast(er). */
  જો(!h || !GOOD_EASY_HANDLE(h->data))
    પાછા ખાલી;
  નહીંતર {
    જો(h->stream && અંક < h->stream->ધકેલો_headers_used)
      પાછા h->stream->ધકેલો_headers[અંક];
  }
  પાછા ખાલી;
}

/*
 * push header access function. Only to be used from within the push callback
 */
અક્ષર *curl_pushheader_byname(સંરચના curl_pushheaders *h, અચલ અક્ષર *name)
{
  સંરચના h2_stream_ctx *stream;
  size_t len;
  size_t i;
  /* Verify that we got a good easy handle in the push header struct,
     mostly to detect rubbish input fast(er). Also empty header name
     is rubbish too. We have to allow ":" at the beginning of
     the header, but header == ":" must be rejected. If we have ':' in
     the middle of header, it could be matched in middle of the value,
     this is because we do prefix match.*/
  જો(!h || !GOOD_EASY_HANDLE(h->data) || !name || !name[0] ||
     !strcmp(name, ":") || strchr(name + 1, ':'))
    પાછા ખાલી;

  stream = h->stream;
  જો(!stream)
    પાછા ખાલી;

  len = strlen(name);
  માટે(i = 0; i < stream->ધકેલો_headers_used; i++) {
    જો(!strncmp(name, stream->ધકેલો_headers[i], len)) {
      /* sub-match, make sure that it is followed by a colon */
      જો(stream->ધકેલો_headers[i][len] != ':')
        ચાલુ;
      પાછા &stream->ધકેલો_headers[i][len + 1];
    }
  }
  પાછા ખાલી;
}

સ્થિર સંરચના Curl_easy *h2_duphandle(સંરચના Curl_cfilter *cf,
                                      સંરચના Curl_easy *data)
{
  સંરચના Curl_easy *second = curl_easy_init();
  જો(second) {
    સંરચના h2_stream_ctx *second_stream;
    http2_data_setup(cf, second, &second_stream);
    second->state.weight = data->state.weight;
    જો(data->share)
      (શૂન્ય)Curl_share_easy_link(second, data->share);
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
  પાછા second;
}

સ્થિર પૂર્ણાંક set_transfer_url(સંરચના Curl_easy *newhandle,
                            સંરચના curl_pushheaders *hp,
                            સંરચના Curl_easy *data)
{
  અચલ અક્ષર *v;
  CURLUcode uc;
  અક્ષર *url = ખાલી;
  પૂર્ણાંક rc = 0;
  CURLU *u = curl_url();

  જો(!u)
    પાછા 5;

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_SCHEME);
  જો(v) {
    uc = curl_url_set(u, CURLUPART_SCHEME, v, 0);
    જો(uc) {
      rc = 1;
      જાઓ fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_AUTHORITY);
  જો(v) {
    uc = Curl_url_set_authority(u, v);
    જો(uc) {
      rc = 2;
      જાઓ fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_PATH);
  જો(v) {
    uc = curl_url_set(u, CURLUPART_PATH, v, 0);
    જો(uc) {
      rc = 3;
      જાઓ fail;
    }
  }

  /* We can only allow PUSH of resource from the same origin, e.g.
   * scheme + hostname + port */
  જો(!Curl_url_same_origin(data->state.uh, u)) {
    rc = 1;
    જાઓ fail;
  }

  uc = curl_url_get(u, CURLUPART_URL, &url, 0);
  જો(uc)
    rc = 4;
fail:
  જો(rc) {
    curl_url_cleanup(u);
    પાછા rc;
  }

  DEBUGASSERT(!newhandle->state.uh);
  newhandle->state.uh = u;
  Curl_bufref_set(&newhandle->state.url, url, 0, curl_free);
  પાછા 0;
}

સ્થિર શૂન્ય discard_newhandle(સંરચના Curl_cfilter *cf,
                              સંરચના Curl_easy *newhandle)
{
  http2_data_done(cf, newhandle);
  (શૂન્ય)Curl_close(&newhandle);
}

સ્થિર પૂર્ણાંક ધકેલો_promise(સંરચના Curl_cfilter *cf,
                        સંરચના Curl_easy *data,
                        અચલ nghttp2_ધકેલો_promise *frame)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  પૂર્ણાંક rv; /* one of the CURL_PUSH_* defines */

  CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE received",
              frame->promised_stream_id);
  જો(data->multi->ધકેલો_cb) {
    સંરચના h2_stream_ctx *stream;
    સંરચના h2_stream_ctx *newstream;
    સંરચના curl_pushheaders heads;
    CURLMcode mresult;
    CURLcode result;
    /* clone the parent */
    સંરચના Curl_easy *newhandle = h2_duphandle(cf, data);
    જો(!newhandle) {
      infof(data, "failed to duplicate handle");
      rv = CURL_ધકેલો_DENY; /* FAIL HARD */
      જાઓ fail;
    }

    stream = H2_STREAM_CTX(ctx, data);
    જો(!stream) {
      failf(data, "Internal NULL stream");
      discard_newhandle(cf, newhandle);
      rv = CURL_ધકેલો_DENY;
      જાઓ fail;
    }

    heads.data = data;
    heads.stream = stream;
    heads.frame = frame;

    rv = set_transfer_url(newhandle, &heads, data);
    જો(rv) {
      CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE, failed to set URL -> %d",
                  frame->promised_stream_id, rv);
      discard_newhandle(cf, newhandle);
      rv = CURL_ધકેલો_DENY;
      જાઓ fail;
    }

    {
      સંરચના Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, multi_ધકેલો_cb);
      rv = data->multi->ધકેલો_cb(data, newhandle,
                                stream->ધકેલો_headers_used, &heads,
                                data->multi->ધકેલો_userp);
      CURL_CBAPI_END(&guard);
    }

    /* free the headers again */
    free_ધકેલો_headers(stream);

    જો(rv) {
      DEBUGASSERT((rv > CURL_ધકેલો_OK) && (rv <= CURL_ધકેલો_ERROROUT));
      /* denied, kill off the new handle again */
      CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE, denied by application -> %d",
                  frame->promised_stream_id, rv);
      discard_newhandle(cf, newhandle);
      જાઓ fail;
    }

    /* approved, add to the multi handle for processing. This
     * assigns newhandle->mid. For the new `mid` we assign the
     * h2_stream instance and remember the stream_id already known. */
    mresult = Curl_multi_add_perform(data->multi, newhandle, cf->conn);
    જો(mresult) {
      infof(data, "failed to add handle to multi");
      discard_newhandle(cf, newhandle);
      rv = CURL_ધકેલો_DENY;
      જાઓ fail;
    }

    result = http2_data_setup(cf, newhandle, &newstream);
    જો(result) {
      failf(data, "error setting up stream: %d", (પૂર્ણાંક)result);
      discard_newhandle(cf, newhandle);
      rv = CURL_ધકેલો_DENY;
      જાઓ fail;
    }

    DEBUGASSERT(newstream);
    newstream->id = frame->promised_stream_id;
    newhandle->req.maxdownload = -1;
    newhandle->req.size = -1;

    CURL_TRC_CF(data, cf, "promise easy handle added to multi, mid=%u",
                newhandle->mid);
    rv = Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, newstream->id, newstream);
    જો(rv) {
      infof(data, "failed to set user_data for stream %d",
            newstream->id);
      DEBUGASSERT(0);
      discard_newhandle(cf, newhandle);
      rv = CURL_ધકેલો_DENY;
      જાઓ fail;
    }

    /* success, remember max stream id processed */
    જો(newstream->id > ctx->local_max_sid)
      ctx->local_max_sid = newstream->id;
  }
  નહીંતર {
    CURL_TRC_CF(data, cf, "Got PUSH_PROMISE, ignore it");
    rv = CURL_ધકેલો_DENY;
  }
fail:
  પાછા rv;
}

સ્થિર શૂન્ય h2_xfer_write_resp_hd(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data,
                                  સંરચના h2_stream_ctx *stream,
                                  અચલ અક્ષર *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  જો(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, blen, eos);
    જો(!stream->xfer_result && !eos)
      stream->xfer_result = cf_h2_update_local_win(cf, data, stream);
    જો(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%d] error %d writing %zu bytes of headers",
                  stream->id, (પૂર્ણાંક)stream->xfer_result, blen);
  }
}

સ્થિર શૂન્ય h2_xfer_write_resp(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data,
                               સંરચના h2_stream_ctx *stream,
                               અચલ અક્ષર *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  જો(!stream->xfer_result)
    stream->xfer_result = Curl_xfer_write_resp(data, buf, blen, eos);
  /* If the transfer write is errored, we do not want any more data */
  જો(stream->xfer_result) {
    સંરચના cf_h2_ctx *ctx = cf->ctx;
    CURL_TRC_CF(data, cf, "[%d] error %d writing %zu bytes of data, "
                "RST-ing stream",
                stream->id, (પૂર્ણાંક)stream->xfer_result, blen);
    Curl_ngh2_submit_rst_stream(
      &ctx->ngh2, 0, stream->id, (uint32_t)NGHTTP2_ERR_CALLBACK_FAILURE);
  }
  નહીંતર જો(!stream->write_paused && Curl_xfer_write_is_paused(data)) {
    CURL_TRC_CF(data, cf, "[%d] stream output paused", stream->id);
    stream->write_paused = TRUE;
  }
  નહીંતર જો(stream->write_paused && !Curl_xfer_write_is_paused(data)) {
    CURL_TRC_CF(data, cf, "[%d] stream output unpaused", stream->id);
    stream->write_paused = FALSE;
  }

  જો(!stream->xfer_result && !eos)
    stream->xfer_result = cf_h2_update_local_win(cf, data, stream);
}

સ્થિર CURLcode on_stream_frame(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data,
                                સંરચના h2_stream_ctx *stream,
                                અચલ nghttp2_frame *frame)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  int32_t stream_id = frame->hd.stream_id;
  પૂર્ણાંક rv;

  જો(!stream) {
    CURL_TRC_CF(data, cf, "[%d] No stream_ctx set", stream_id);
    પાછા CURLE_FAILED_INIT;
  }

  પસંદગી(frame->hd.type) {
  સ્થિતિ NGHTTP2_DATA:
    CURL_TRC_CF(data, cf, "[%d] DATA, window=%d/%d",
                stream_id,
                Curl_ngh2_session_get_stream_effective_recv_data_length(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_stream_effective_local_window_size(
                  &ctx->ngh2, stream->id));
    /* If !body started on this stream, then receiving DATA is illegal. */
    જો(!stream->bodystarted) {
      જો(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                       stream_id, NGHTTP2_PROTOCOL_ERROR))) {
        પાછા CURLE_RECV_ERROR;
      }
    }
    તોડો;
  સ્થિતિ NGHTTP2_HEADERS:
    જો(stream->bodystarted) {
      /* Only valid HEADERS after body started is trailer HEADERS. We
         buffer them in on_header callback. */
      તોડો;
    }

    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    જો(stream->status_code == -1)
      પાછા CURLE_RECV_ERROR;

    /* Only final status code signals the end of header */
    જો(stream->status_code / 100 != 1) {
      stream->resp_hds_complete = TRUE;
      stream->bodystarted = TRUE;
    }
    નહીંતર
      stream->status_code = -1;

    h2_xfer_write_resp_hd(cf, data, stream, STRCONST("\r\n"),
                          (bool)stream->closed);

    Curl_multi_mark_dirty(data);
    તોડો;
  સ્થિતિ NGHTTP2_ધકેલો_PROMISE:
    rv = ધકેલો_promise(cf, data, &frame->ધકેલો_promise);
    જો(rv) { /* deny! */
      DEBUGASSERT((rv > CURL_ધકેલો_OK) && (rv <= CURL_ધકેલો_ERROROUT));
      જો(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
             frame->ધકેલો_promise.promised_stream_id, NGHTTP2_CANCEL)))
        પાછા CURLE_SEND_ERROR;
      નહીંતર જો(rv == CURL_ધકેલો_ERROROUT) {
        /* fail the parent transfer, not the whole session */
        CURL_TRC_CF(data, cf, "[%d] fail in PUSH_PROMISE received",
                    stream_id);
        જો(!stream->xfer_result)
          stream->xfer_result = CURLE_RECV_ERROR;
        Curl_multi_mark_dirty(data);
      }
    }
    તોડો;
  સ્થિતિ NGHTTP2_RST_STREAM:
    જો(frame->rst_stream.error_code)
      stream->reset_by_server = TRUE;
    Curl_multi_mark_dirty(data);
    તોડો;
  સ્થિતિ NGHTTP2_WINDOW_UPDATE:
    જો(CURL_REQ_WANT_SEND(data) && Curl_bufq_is_empty(&stream->sendbuf)) {
      /* need more data, force processing of transfer */
      Curl_multi_mark_dirty(data);
    }
    નહીંતર જો(!Curl_bufq_is_empty(&stream->sendbuf)) {
      /* resume the potentially suspended stream */
      જો(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id)))
        પાછા CURLE_SEND_ERROR;
    }
    તોડો;
  મૂળભૂત:
    તોડો;
  }

  જો(frame->hd.flags & NGHTTP2_FLAG_END_STREAM) {
    જો(!stream->closed && !stream->body_eos &&
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
  પાછા CURLE_OK;
}

#ifdef CURLVERBOSE
પૂર્ણાંક Curl_nghttp2_fr_print(અચલ nghttp2_frame *frame, અક્ષર *buffer,
                          size_t blen)
{
  પસંદગી(frame->hd.type) {
  સ્થિતિ NGHTTP2_DATA: {
    પાછા curl_msnprintf(buffer, blen,
                          "FRAME[DATA, len=%d, eos=%d, padlen=%d]",
                          (પૂર્ણાંક)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_STREAM),
                          (પૂર્ણાંક)frame->data.padlen);
  }
  સ્થિતિ NGHTTP2_HEADERS: {
    પાછા curl_msnprintf(buffer, blen,
                          "FRAME[HEADERS, len=%d, hend=%d, eos=%d]",
                          (પૂર્ણાંક)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_HEADERS),
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_STREAM));
  }
  સ્થિતિ NGHTTP2_PRIORITY: {
    પાછા curl_msnprintf(buffer, blen,
                          "FRAME[PRIORITY, len=%d, flags=%d]",
                          (પૂર્ણાંક)frame->hd.length, frame->hd.flags);
  }
  સ્થિતિ NGHTTP2_RST_STREAM: {
    પાછા curl_msnprintf(buffer, blen,
                          "FRAME[RST_STREAM, len=%d, flags=%d, error=%u]",
                          (પૂર્ણાંક)frame->hd.length, frame->hd.flags,
                          frame->rst_stream.error_code);
  }
  સ્થિતિ NGHTTP2_SETTINGS: {
    જો(frame->hd.flags & NGHTTP2_FLAG_ACK) {
      પાછા curl_msnprintf(buffer, blen, "FRAME[SETTINGS, ack=1]");
    }
    પાછા curl_msnprintf(buffer, blen,
                          "FRAME[SETTINGS, len=%d]", (પૂર્ણાંક)frame->hd.length);
  }
  સ્થિતિ NGHTTP2_ધકેલો_PROMISE:
    પાછા curl_msnprintf(buffer, blen,
                          "FRAME[PUSH_PROMISE, len=%d, hend=%d]",
                          (પૂર્ણાંક)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_HEADERS));
  સ્થિતિ NGHTTP2_PING:
    પાછા curl_msnprintf(buffer, blen,
                          "FRAME[PING, len=%d, ack=%d]",
                          (પૂર્ણાંક)frame->hd.length,
                          frame->hd.flags & NGHTTP2_FLAG_ACK);
  સ્થિતિ NGHTTP2_GOAWAY: {
    અક્ષર scratch[128];
    size_t s_len = CURL_ARRAYSIZE(scratch);
    size_t len = (frame->goaway.opaque_data_len < s_len) ?
      frame->goaway.opaque_data_len : s_len - 1;
    જો(len)
      memcpy(scratch, frame->goaway.opaque_data, len);
    scratch[len] = '\0';
    પાછા curl_msnprintf(buffer, blen,
                          "FRAME[GOAWAY, error=%u, reason='%s', "
                          "last_stream=%d]", frame->goaway.error_code,
                          scratch, frame->goaway.last_stream_id);
  }
  સ્થિતિ NGHTTP2_WINDOW_UPDATE: {
    પાછા curl_msnprintf(buffer, blen,
                          "FRAME[WINDOW_UPDATE, incr=%d]",
                          frame->window_update.window_size_increment);
  }
  મૂળભૂત:
    પાછા curl_msnprintf(buffer, blen, "FRAME[%d, len=%d, flags=%d]",
                          frame->hd.type, (પૂર્ણાંક)frame->hd.length,
                          frame->hd.flags);
  }
}

સ્થિર પૂર્ણાંક on_frame_send(nghttp2_session *session, અચલ nghttp2_frame *frame,
                         શૂન્ય *userp)
{
  સંરચના Curl_cfilter *cf = userp;
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);

  (શૂન્ય)session;
  DEBUGASSERT(data);
  જો(Curl_trc_cf_is_verbose(cf, data)) {
    અક્ષર buffer[256];
    પૂર્ણાંક len;
    len = Curl_nghttp2_fr_print(frame, buffer, કદ(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] -> %s", frame->hd.stream_id, buffer);
  }
  જો((frame->hd.type == NGHTTP2_GOAWAY) && !ctx->sent_goaway) {
    /* A GOAWAY not initiated by us, but by nghttp2 itself on detecting
     * a protocol error on the connection */
    failf(data, "nghttp2 shuts down connection with error %u: %s",
          frame->goaway.error_code,
          nghttp2_http2_strerror(frame->goaway.error_code));
  }
  પાછા 0;
}
#endif /* CURLVERBOSE */

સ્થિર પૂર્ણાંક on_frame_recv(nghttp2_session *session, અચલ nghttp2_frame *frame,
                         શૂન્ય *userp)
{
  સંરચના Curl_cfilter *cf = userp;
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  સંરચના Curl_easy *data = CF_DATA_CURRENT(cf), *data_s;
  સંરચના h2_stream_ctx *stream;
  int32_t stream_id = frame->hd.stream_id;

  (શૂન્ય)session;
  DEBUGASSERT(data);
#ifdef CURLVERBOSE
  જો(Curl_trc_cf_is_verbose(cf, data)) {
    અક્ષર buffer[256];
    પૂર્ણાંક len;
    len = Curl_nghttp2_fr_print(frame, buffer, કદ(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] <- %s", frame->hd.stream_id, buffer);
  }
#endif /* CURLVERBOSE */

  જો(!stream_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(data);
    પસંદગી(frame->hd.type) {
    સ્થિતિ NGHTTP2_SETTINGS: {
      જો(!(frame->hd.flags & NGHTTP2_FLAG_ACK)) {
        uint32_t max_conn = ctx->max_concurrent_streams;
        ctx->max_concurrent_streams =
          Curl_ngh2_session_get_remote_settings(
            &ctx->ngh2, NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS);
        ctx->enable_ધકેલો = Curl_ngh2_session_get_remote_settings(
          &ctx->ngh2, NGHTTP2_SETTINGS_ENABLE_ધકેલો) != 0;
        CURL_TRC_CF(data, cf, "[0] MAX_CONCURRENT_STREAMS: %u",
                    ctx->max_concurrent_streams);
        CURL_TRC_CF(data, cf, "[0] ENABLE_PUSH: %s",
                    ctx->enable_ધકેલો ? "TRUE" : "false");
        જો(data && max_conn != ctx->max_concurrent_streams) {
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
        જો(CURL_REQ_WANT_SEND(data))
          Curl_multi_mark_dirty(data);
      }
      તોડો;
    }
    સ્થિતિ NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->goaway_error = frame->goaway.error_code;
      ctx->remote_max_sid = frame->goaway.last_stream_id;
      જો(data) {
        infof(data, "received GOAWAY, error=%u, last_stream=%d",
              ctx->goaway_error, ctx->remote_max_sid);
        Curl_multi_connchanged(data->multi);
      }
      તોડો;
    મૂળભૂત:
      તોડો;
    }
    પાછા 0;
  }

  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  જો(!data_s) {
    CURL_TRC_CF(data, cf, "[%d] No Curl_easy associated", stream_id);
    પાછા 0;
  }

  પાછા on_stream_frame(cf, data_s, stream, frame) ?
         NGHTTP2_ERR_CALLBACK_FAILURE : 0;
}

સ્થિર પૂર્ણાંક cf_h2_on_invalid_frame_recv(nghttp2_session *session,
                                       અચલ nghttp2_frame *frame,
                                       પૂર્ણાંક ngerr, શૂન્ય *userp)
{
  સંરચના Curl_cfilter *cf = userp;
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  સંરચના Curl_easy *data;
  સંરચના h2_stream_ctx *stream;
  int32_t stream_id = frame->hd.stream_id;

  (શૂન્ય)session;
  data = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  જો(data) {
#ifdef CURLVERBOSE
    અક્ષર buffer[256];
    પૂર્ણાંક len;
    len = Curl_nghttp2_fr_print(frame, buffer, કદ(buffer) - 1);
    buffer[len] = 0;
    failf(data, "[HTTP2] [%d] received invalid frame: %s, error %d: %s",
          stream_id, buffer, ngerr, nghttp2_strerror(ngerr));
#endif /* CURLVERBOSE */
    જો(stream) {
      Curl_ngh2_submit_rst_stream(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, NGHTTP2_STREAM_CLOSED);
      stream->error = ngerr;
      stream->closed = TRUE;
      stream->reset = TRUE;
    }
  }
  /* Return no error, nghttp2 will RST/GOAWAY by itself when needed */
  પાછા 0;
}

સ્થિર પૂર્ણાંક on_data_chunk_recv(nghttp2_session *session, uint8_t flags,
                              int32_t stream_id,
                              અચલ uint8_t *mem, size_t len, શૂન્ય *userp)
{
  સંરચના Curl_cfilter *cf = userp;
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  સંરચના h2_stream_ctx *stream;
  સંરચના Curl_easy *data_s;

  (શૂન્ય)session;
  (શૂન્ય)flags;

  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */
  DEBUGASSERT(CF_DATA_CURRENT(cf));

  /* get the stream from the hash based on Stream ID */
  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  જો(!data_s) {
    /* Receiving a Stream ID not in the hash should not happen - unless
       we have aborted a transfer artificially and there were more data
       in the pipeline. Silently ignore. */
    CURL_TRC_CF(CF_DATA_CURRENT(cf), cf, "[%d] Data for unknown", stream_id);
    /* consumed explicitly as no one will read it */
    Curl_ngh2_session_consume(&ctx->ngh2, stream_id, len);
    પાછા 0;
  }

  જો(!stream)
    પાછા NGHTTP2_ERR_CALLBACK_FAILURE;

  h2_xfer_write_resp(cf, data_s, stream, (અચલ અક્ષર *)mem, len, FALSE);

  Curl_ngh2_session_consume(&ctx->ngh2, stream_id, len);
  stream->nrcvd_data += (curl_off_t)len;
  પાછા 0;
}

સ્થિર પૂર્ણાંક on_stream_close(nghttp2_session *session, int32_t stream_id,
                           uint32_t error_code, શૂન્ય *userp)
{
  સંરચના Curl_cfilter *cf = userp;
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  સંરચના Curl_easy *data_s, *call_data = CF_DATA_CURRENT(cf);
  સંરચના h2_stream_ctx *stream = ખાલી;
  પૂર્ણાંક rv;
  (શૂન્ય)session;

  DEBUGASSERT(call_data);
  /* stream id 0 is the connection, do not look there for streams. */
  data_s = stream_id ? cf_h2_get_stream_xfer(cf, stream_id, &stream) : ખાલી;
  જો(!data_s) {
    CURL_TRC_CF(call_data, cf,
                "[%d] on_stream_close, no easy set on stream", stream_id);
    પાછા 0;
  }
  જો(!GOOD_EASY_HANDLE(data_s)) {
    /* nghttp2 still has an easy registered for the stream which has
     * been freed be libcurl. This points to a code path that does not
     * trigger DONE or DETACH events as it must. */
    CURL_TRC_CF(call_data, cf,
                "[%d] on_stream_close, not a GOOD easy on stream", stream_id);
    (શૂન્ય)Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, stream_id, ખાલી);
    પાછા NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  જો(!stream) {
    CURL_TRC_CF(data_s, cf,
                "[%d] on_stream_close, GOOD easy but no stream", stream_id);
    પાછા NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  stream->closed = TRUE;
  stream->error = error_code;
  જો(stream->error) {
    stream->reset = TRUE;
    જો((stream->error == NGHTTP2_REFUSED_STREAM) &&
       ctx->rcvd_goaway && ctx->goaway_error)
    /* REFUSED_STREAM is used by nghttp2 on a GOAWAY from the
     * server where it indicated it will no longer process this stream.
     * If the GOAWAY carried an error code, take that as the error for
     * the stream. */
    stream->error = ctx->goaway_error;
  }

  જો(stream->error)
    CURL_TRC_CF(data_s, cf, "[%d] RESET: %s (err %u)",
              stream_id, nghttp2_http2_strerror(error_code), error_code);
  નહીંતર
    CURL_TRC_CF(data_s, cf, "[%d] CLOSED", stream_id);
  Curl_multi_mark_dirty(data_s);

  /* remove `stream_ctx` from the nghttp2 stream */
  rv = Curl_ngh2_session_set_stream_user_data(
    &ctx->ngh2, stream_id, ખાલી);
  જો(rv) {
    infof(data_s, "http/2: failed to clear user_data for stream %d",
          stream_id);
    DEBUGASSERT(0);
  }
  પાછા 0;
}

સ્થિર શૂન્ય cf_h2_header_error(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data,
                               સંરચના h2_stream_ctx *stream,
                               CURLcode result)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;

  failf(data, "Error receiving HTTP2 header: %d(%s)", (પૂર્ણાંક)result,
        curl_easy_strerror(result));
  જો(stream) {
    Curl_ngh2_submit_rst_stream(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, NGHTTP2_STREAM_CLOSED);
    stream->closed = TRUE;
    stream->reset = TRUE;
  }
}

/* frame->hd.type is either NGHTTP2_HEADERS or NGHTTP2_PUSH_PROMISE */
સ્થિર પૂર્ણાંક on_header(nghttp2_session *session, અચલ nghttp2_frame *frame,
                     અચલ uint8_t *name, size_t namelen,
                     અચલ uint8_t *કિંમત, size_t valuelen,
                     uint8_t flags,
                     શૂન્ય *userp)
{
  સંરચના Curl_cfilter *cf = userp;
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  સંરચના h2_stream_ctx *stream;
  સંરચના Curl_easy *data;
  int32_t stream_id = frame->hd.stream_id;
  CURLcode result;

  (શૂન્ય)session;
  (શૂન્ય)flags;

  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */

  /* get the stream from the hash based on Stream ID */
  data = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  જો(!GOOD_EASY_HANDLE(data))
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    પાછા NGHTTP2_ERR_CALLBACK_FAILURE;

  જો(!stream) {
    failf(data, "Internal NULL stream");
    પાછા NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  /* Store received PUSH_PROMISE headers to be used when the subsequent
     PUSH_PROMISE callback comes */
  જો(frame->hd.type == NGHTTP2_ધકેલો_PROMISE) {
    અક્ષર *h;

    જો((namelen == CURL_CSTRLEN(HTTP_PSEUDO_AUTHORITY)) &&
       !strncmp(HTTP_PSEUDO_AUTHORITY, (અચલ અક્ષર *)name, namelen)) {
      /* pseudo headers are lower case */
      પૂર્ણાંક rc = 0;
      અક્ષર *check = curl_maprintf("%s:%d", data->state.origin->hostname,
                                  data->state.origin->port);
      જો(!check)
        /* no memory */
        પાછા NGHTTP2_ERR_CALLBACK_FAILURE;
      જો(!curl_strequal(check, (અચલ અક્ષર *)કિંમત) &&
         ((data->state.origin->port != data->state.origin->scheme->defport) ||
          !curl_strequal(data->state.origin->hostname, (અચલ અક્ષર *)કિંમત))) {
        /* This is push is not for the same authority that was asked for in
         * the URL. RFC 7540 section 8.2 says: "A client MUST treat a
         * PUSH_PROMISE for which the server is not authoritative as a stream
         * error of type PROTOCOL_ERROR."
         */
        (શૂન્ય)Curl_ngh2_submit_rst_stream(
          &ctx->ngh2, NGHTTP2_FLAG_NONE, stream_id, NGHTTP2_PROTOCOL_ERROR);
        rc = NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      curlx_free(check);
      જો(rc)
        પાછા rc;
    }

    જો(!stream->ધકેલો_headers) {
      stream->ધકેલો_headers_alloc = 10;
      stream->ધકેલો_headers = curlx_malloc(stream->ધકેલો_headers_alloc *
                                          કદ(અક્ષર *));
      જો(!stream->ધકેલો_headers)
        પાછા NGHTTP2_ERR_CALLBACK_FAILURE;
      stream->ધકેલો_headers_used = 0;
    }
    નહીંતર જો(stream->ધકેલો_headers_used ==
            stream->ધકેલો_headers_alloc) {
      અક્ષર **headp;
      જો(stream->ધકેલો_headers_alloc > 1000) {
        /* this is beyond crazy many headers, bail out */
        failf(data, "Too many PUSH_PROMISE headers");
        free_ધકેલો_headers(stream);
        પાછા NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      stream->ધકેલો_headers_alloc *= 2;
      headp = curlx_realloc(stream->ધકેલો_headers,
                            stream->ધકેલો_headers_alloc * કદ(અક્ષર *));
      જો(!headp) {
        free_ધકેલો_headers(stream);
        પાછા NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      stream->ધકેલો_headers = headp;
    }
    h = curl_maprintf("%s:%s", name, કિંમત);
    જો(!h) {
      free_ધકેલો_headers(stream);
      પાછા NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    stream->ધકેલો_headers[stream->ધકેલો_headers_used++] = h;
    પાછા 0;
  }

  જો(stream->bodystarted) {
    /* This is a trailer */
    CURL_TRC_CF(data, cf, "[%d] trailer: %.*s: %.*s",
                stream->id, (પૂર્ણાંક)namelen, name, (પૂર્ણાંક)valuelen, કિંમત);
    result = Curl_dynhds_add(&stream->resp_trailers,
                             (અચલ અક્ષર *)name, namelen,
                             (અચલ અક્ષર *)કિંમત, valuelen);
    જો(result) {
      cf_h2_header_error(cf, data, stream, result);
      પાછા NGHTTP2_ERR_CALLBACK_FAILURE;
    }

    પાછા 0;
  }

  જો(namelen == CURL_CSTRLEN(HTTP_PSEUDO_STATUS) &&
     !memcmp(HTTP_PSEUDO_STATUS, name, namelen)) {
    /* nghttp2 guarantees :status is received first and only once. */
    અક્ષર buffer[32];
    size_t hlen;
    result = Curl_http_decode_status(&stream->status_code,
                                     (અચલ અક્ષર *)કિંમત, valuelen);
    જો(result) {
      cf_h2_header_error(cf, data, stream, result);
      પાછા NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    hlen = curl_msnprintf(buffer, કદ(buffer), HTTP_PSEUDO_STATUS ":%d\r",
                          stream->status_code);
    result = Curl_headers_ધકેલો(data, buffer, hlen, CURLH_PSEUDO);
    જો(result) {
      cf_h2_header_error(cf, data, stream, result);
      પાછા NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    curlx_dyn_reset(&ctx->scratch);
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/2 "));
    જો(!result)
      result = curlx_dyn_addn(&ctx->scratch, કિંમત, valuelen);
    જો(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    જો(!result)
      h2_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    જો(result) {
      cf_h2_header_error(cf, data, stream, result);
      પાછા NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    /* if we receive data for another handle, wake that up */
    જો(CF_DATA_CURRENT(cf) != data)
      Curl_multi_mark_dirty(data);

    CURL_TRC_CF(data, cf, "[%d] status: HTTP/2 %03d",
                stream->id, stream->status_code);
    પાછા 0;
  }

  /* nghttp2 guarantees that namelen > 0, and :status was already
     received, and this is not pseudo-header field . */
  /* convert to an HTTP1-style header */
  curlx_dyn_reset(&ctx->scratch);
  result = curlx_dyn_addn(&ctx->scratch, (અચલ અક્ષર *)name, namelen);
  જો(!result)
    result = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
  જો(!result)
    result = curlx_dyn_addn(&ctx->scratch, (અચલ અક્ષર *)કિંમત, valuelen);
  જો(!result)
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
  જો(!result)
    h2_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                          curlx_dyn_len(&ctx->scratch), FALSE);
  જો(result) {
    cf_h2_header_error(cf, data, stream, result);
    પાછા NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  /* if we receive data for another handle, wake that up */
  જો(CF_DATA_CURRENT(cf) != data)
    Curl_multi_mark_dirty(data);

  CURL_TRC_CF(data, cf, "[%d] header: %.*s: %.*s",
              stream->id, (પૂર્ણાંક)namelen, name, (પૂર્ણાંક)valuelen, કિંમત);

  પાછા 0; /* 0 is successful */
}

સ્થિર ssize_t req_body_read_callback(nghttp2_session *session,
                                      int32_t stream_id,
                                      uint8_t *buf, size_t length,
                                      uint32_t *data_flags,
                                      nghttp2_data_source *source,
                                      શૂન્ય *userp)
{
  સંરચના Curl_cfilter *cf = userp;
  સંરચના Curl_easy *data_s;
  સંરચના h2_stream_ctx *stream = ખાલી;
  CURLcode result;
  ssize_t nread;
  size_t n;
  (શૂન્ય)source;

  (શૂન્ય)session;
  જો(!stream_id)
    પાછા NGHTTP2_ERR_INVALID_ARGUMENT;

  /* get the stream from the hash based on Stream ID, stream ID zero is for
     connection-oriented stuff */
  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  જો(!data_s || !stream)
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    પાછા NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_bufq_read(&stream->sendbuf, buf, length, &n);
  જો(result) {
    જો(result != CURLE_AGAIN)
      પાછા NGHTTP2_ERR_CALLBACK_FAILURE;
    nread = 0;
  }
  નહીંતર
    nread = (ssize_t)n;

  CURL_TRC_CF(data_s, cf, "[%d] req_body_read(len=%zu) eos=%d -> %zd, %d",
              stream_id, length, stream->body_eos, nread, (પૂર્ણાંક)result);

  જો(stream->body_eos && Curl_bufq_is_empty(&stream->sendbuf)) {
    *data_flags = NGHTTP2_DATA_FLAG_EOF;
    પાછા nread;
  }
  પાછા (nread == 0) ? NGHTTP2_ERR_DEFERRED : nread;
}

#ifdef CURLVERBOSE
સ્થિર પૂર્ણાંક error_callback(nghttp2_session *session,
                          અચલ અક્ષર *સંદેશ,
                          size_t len,
                          શૂન્ય *userp)
{
  સંરચના Curl_cfilter *cf = userp;
  સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);
  (શૂન્ય)session;
  failf(data, "%.*s", (પૂર્ણાંક)len, સંદેશ);
  પાછા 0;
}
#endif

/*
 * Append headers to ask for an HTTP1.1 to HTTP2 upgrade.
 */
CURLcode Curl_http2_request_upgrade(સંરચના dynbuf *req,
                                    સંરચના Curl_easy *data)
{
  CURLcode result;
  અક્ષર *base64;
  size_t blen;
  સંરચના SingleRequest *k = &data->req;
  uint8_t binsettings[H2_BINSETTINGS_LEN];
  ssize_t rc;
  size_t binlen; /* length of the binsettings data */

  rc = populate_binsettings(binsettings, data);
  જો(!curlx_sztouz(rc, &binlen) || !binlen) {
    failf(data, "nghttp2 unexpectedly failed on pack_settings_payload");
    curlx_dyn_free(req);
    પાછા CURLE_FAILED_INIT;
  }

  result = curlx_base64url_encode(binsettings, binlen, &base64, &blen);
  જો(result) {
    curlx_dyn_free(req);
    પાછા result;
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

  પાછા result;
}

સ્થિર CURLcode http2_handle_stream_close(સંરચના Curl_cfilter *cf,
                                          સંરચના Curl_easy *data,
                                          સંરચના h2_stream_ctx *stream,
                                          size_t *pnlen)
{
  CURLcode result;

  *pnlen = 0;
  જો(stream->reset) {
    જો(stream->error == NGHTTP2_REFUSED_STREAM) {
      infof(data, "HTTP/2 stream %d refused by server, try again on a new "
                  "connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      પાછા CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    નહીંતર જો(stream->resp_hds_complete && data->req.no_body) {
      CURL_TRC_CF(data, cf, "[%d] error after response headers, but we did "
                  "not want a body anyway, ignore: %s (err %u)",
                  stream->id, nghttp2_http2_strerror(stream->error),
                  stream->error);
      stream->close_handled = TRUE;
      પાછા CURLE_OK;
    }
    failf(data, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          stream->id, stream->reset_by_server ? "server" : "curl",
          stream->error, nghttp2_http2_strerror(stream->error));
    પાછા stream->error ? CURLE_HTTP2_STREAM :
           (data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2);
  }
  નહીંતર જો(!stream->bodystarted) {
    failf(data, "HTTP/2 stream %d was closed cleanly, but before getting "
          "all response header fields, treated as error", stream->id);
    પાછા CURLE_HTTP2_STREAM;
  }

  જો(Curl_dynhds_ગણતરી(&stream->resp_trailers)) {
    સંરચના dynhds_entry *e;
    સંરચના dynbuf dbuf;
    size_t i;

    result = CURLE_OK;
    curlx_dyn_init(&dbuf, DYN_TRAILERS);
    માટે(i = 0; i < Curl_dynhds_ગણતરી(&stream->resp_trailers); ++i) {
      e = Curl_dynhds_getn(&stream->resp_trailers, i);
      જો(!e)
        તોડો;
      curlx_dyn_reset(&dbuf);
      result = curlx_dyn_addf(&dbuf, "%.*s: %.*s\x0d\x0a",
                              (પૂર્ણાંક)e->namelen, e->name,
                              (પૂર્ણાંક)e->valuelen, e->કિંમત);
      જો(result)
        તોડો;
      Curl_debug(data, CURLINFO_HEADER_IN, curlx_dyn_ptr(&dbuf),
                 curlx_dyn_len(&dbuf));
      result = Curl_client_write(data,
                                 CLIENTWRITE_HEADER | CLIENTWRITE_TRAILER,
                                 curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf));
      જો(result)
        તોડો;
      result = Curl_bump_headersize(data, curlx_dyn_len(&dbuf), FALSE);
      જો(result)
        પાછા result;
    }
    curlx_dyn_free(&dbuf);
    જો(result)
      જાઓ out;
  }

  stream->close_handled = TRUE;
  result = CURLE_OK;

out:
  CURL_TRC_CF(data, cf, "handle_stream_close -> %d, %zu", (પૂર્ણાંક)result, *pnlen);
  પાછા result;
}

સ્થિર પૂર્ણાંક sweight_wanted(અચલ સંરચના Curl_easy *data)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  પાછા data->set.weight ?
    data->set.weight : NGHTTP2_DEFAULT_WEIGHT;
}

સ્થિર પૂર્ણાંક sweight_in_effect(અચલ સંરચના Curl_easy *data)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  પાછા data->state.weight ?
    data->state.weight : NGHTTP2_DEFAULT_WEIGHT;
}

/*
 * h2_pri_spec() fills in the pri_spec struct, used by nghttp2 to send weight
 * and dependency to the peer. It also stores the updated values in the state
 * struct.
 */

સ્થિર શૂન્ય h2_pri_spec(સંરચના Curl_easy *data,
                        nghttp2_priority_spec *pri_spec)
{
  પૂર્ણાંક prio = data->set.weight;
  nghttp2_priority_spec_init(pri_spec, 0, sweight_wanted(data), FALSE);
  data->state.weight = prio;
}

/*
 * Check if there is been an update in the priority /
 * dependency settings and if so it submits a PRIORITY frame with the updated
 * info.
 * Flush any out data pending in the network buffer.
 */
સ્થિર CURLcode h2_progress_egress(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  પૂર્ણાંક rv = 0;

  જો(sweight_wanted(data) != sweight_in_effect(data)) {
    સંરચના h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    જો(stream && stream->id > 0 &&
       (sweight_wanted(data) != sweight_in_effect(data))) {
      /* send new weight and/or dependency */
      nghttp2_priority_spec pri_spec;

      h2_pri_spec(data, &pri_spec);
      CURL_TRC_CF(data, cf, "[%d] Queuing PRIORITY", stream->id);
      DEBUGASSERT(stream->id != -1);
      rv = Curl_ngh2_submit_priority(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, &pri_spec);
      જો(rv)
        જાઓ out;
    }
  }

  ctx->nw_out_blocked = 0;
  જ્યારે(!rv && !ctx->nw_out_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

out:
  જો(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(data, cf, "nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    પાછા CURLE_SEND_ERROR;
  }
  /* Defer flushing during the connect phase so that the SETTINGS and
   * other initial frames are sent together with the first request.
   * Unless we are 'connect_only' where the request will never come. */
  જો(!cf->connected && !cf->conn->bits.connect_only)
    પાછા CURLE_OK;
  પાછા nw_out_flush(cf, data);
}

સ્થિર CURLcode stream_recv(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                            સંરચના h2_stream_ctx *stream,
                            અક્ષર *buf, size_t len, size_t *pnread)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_AGAIN;

  (શૂન્ય)buf;
  (શૂન્ય)len;
  *pnread = 0;

  જો(!stream->xfer_result)
    stream->xfer_result = cf_h2_update_local_win(cf, data, stream);

  જો(stream->xfer_result) {
    CURL_TRC_CF(data, cf, "[%d] xfer write failed", stream->id);
    result = stream->xfer_result;
  }
  નહીંતર જો(stream->closed) {
    CURL_TRC_CF(data, cf, "[%d] returning CLOSE", stream->id);
    result = http2_handle_stream_close(cf, data, stream, pnread);
  }
  નહીંતર જો(stream->reset ||
          (ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) ||
          (ctx->rcvd_goaway && ctx->remote_max_sid < stream->id)) {
    CURL_TRC_CF(data, cf, "[%d] returning ERR", stream->id);
    result = data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2;
  }

  જો(result && (result != CURLE_AGAIN))
    CURL_TRC_CF(data, cf, "[%d] stream_recv(len=%zu) -> %d, %zu",
                stream->id, len, (પૂર્ણાંક)result, *pnread);
  પાછા result;
}

સ્થિર CURLcode h2_progress_ingress(સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data,
                                    size_t data_max_bytes)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  જો(should_close_session(ctx)) {
    CURL_TRC_CF(data, cf, "[0] ingress: session is closed");
    પાછા CURLE_HTTP2;
  }

  /* Process network input buffer first */
  જો(!Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "Process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    result = h2_process_pending_આગત(cf, data);
    જો(result)
      પાછા result;
  }

  જો(!data_max_bytes)
    data_max_bytes = H2_CHUNK_SIZE;

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop due to connection close or us not processing
   * all network input */
  જ્યારે(!ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) {
    સંરચના h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    જો(stream && (stream->closed || !data_max_bytes)) {
      /* We would like to abort here and stop processing, so that the transfer
       * loop can handle the data/close here. This may leave data in
       * underlying buffers that will not be consumed. */
      જો(!cf->next || !cf->next->cft->has_data_pending(cf->next, data))
        Curl_multi_mark_dirty(data);
      તોડો;
    }

    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    જો(result) {
      જો(result != CURLE_AGAIN) {
        failf(data, "Failed receiving HTTP2 data: %d(%s)", (પૂર્ણાંક)result,
              curl_easy_strerror(result));
        પાછા result;
      }
      તોડો;
    }
    નહીંતર જો(nread == 0) {
      CURL_TRC_CF(data, cf, "[0] ingress: connection closed");
      ctx->conn_closed = TRUE;
      તોડો;
    }
    નહીંતર {
      CURL_TRC_CF(data, cf, "[0] ingress: read %zu bytes", nread);
      data_max_bytes = (data_max_bytes > nread) ? (data_max_bytes - nread) : 0;
    }

    result = h2_process_pending_આગત(cf, data);
    જો(result)
      પાછા result;
    CURL_TRC_CF(data, cf, "[0] ingress: nw-in buffered %zu",
                Curl_bufq_len(&ctx->inbufq));
  }

  જો(ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "server closed %s",
                ctx->rcvd_goaway ? "with GOAWAY" : "abruptly");
    connclose(cf->conn);
  }

  CURL_TRC_CF(data, cf, "[0] ingress: done");
  પાછા CURLE_OK;
}

સ્થિર CURLcode cf_h2_recv(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                           અક્ષર *buf, size_t len, size_t *pnread)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  સંરચના h2_stream_ctx *stream;
  CURLcode result, r2;
  સંરચના cf_call_data save;

  જો(!data)
    પાછા CURLE_HTTP2;

  stream = H2_STREAM_CTX(ctx, data);

  *pnread = 0;
  જો(!stream) {
    /* Abnormal call sequence: either this transfer has never opened a stream
     * (unlikely) or the transfer has been done, cleaned up its resources, but
     * a read() is called anyway. It is not clear what the calling sequence
     * is for such a case. */
    failf(data, "http/2 recv on a transfer never opened "
          "or already cleared, mid=%u", data->mid);
    પાછા CURLE_HTTP2;
  }

  CF_DATA_SAVE(save, cf, data);

  result = stream_recv(cf, data, stream, buf, len, pnread);
  જો(result && (result != CURLE_AGAIN))
    જાઓ out;

  જો(result) {
    result = h2_progress_ingress(cf, data, len);
    જો(result)
      જાઓ out;

    result = stream_recv(cf, data, stream, buf, len, pnread);
  }

  જો(*pnread > 0) {
    /* Now that we transferred this to the upper layer, we report
     * the actual amount of DATA consumed to the H2 session, so
     * that it adjusts stream flow control */
    Curl_ngh2_session_consume(&ctx->ngh2, stream->id, *pnread);
    જો(stream->closed) {
      CURL_TRC_CF(data, cf, "[%d] DRAIN closed stream", stream->id);
      Curl_multi_mark_dirty(data);
    }
  }

out:
  r2 = h2_progress_egress(cf, data);
  જો(r2 == CURLE_AGAIN) {
    /* pending data to send, need to be called again. Ideally, we
     * monitor the socket for POLLOUT, but when not SENDING
     * any more, we force processing of the transfer. */
    જો(!CURL_REQ_WANT_SEND(data))
      Curl_multi_mark_dirty(data);
  }
  નહીંતર જો(r2 && (result == CURLE_AGAIN)) {
    /* Do not replace a previous error result NOR a successful result
     * with an egress error. Example: the server might close the
     * connection after sending the complete response with EOS
     * and everything. Further sending may fail, but that MUST
     * not change that we had a successful response. */
    result = r2;
  }

  CURL_TRC_CF(data, cf, "[%d] cf_recv(len=%zu) -> %d, %zu, "
              "window=%d/%d, connection %d/%d",
              stream->id, len, (પૂર્ણાંક)result, *pnread,
              Curl_ngh2_session_get_stream_effective_recv_data_length(
                &ctx->ngh2, stream->id),
              Curl_ngh2_session_get_stream_effective_local_window_size(
                &ctx->ngh2, stream->id),
              Curl_ngh2_session_get_local_window_size(&ctx->ngh2),
              HTTP2_HUGE_WINDOW_SIZE);

  CF_DATA_RESTORE(cf, save);
  પાછા result;
}

સ્થિર CURLcode cf_h2_body_send(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data,
                                સંરચના h2_stream_ctx *stream,
                                અચલ શૂન્ય *buf, size_t blen, bool eos,
                                size_t *pnwritten)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  CURLcode result;

  *pnwritten = 0;
  જો(stream->closed) {
    જો(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(data, cf, "[%d] discarding data"
                  "on closed stream with response", stream->id);
      જો(eos)
        stream->body_eos = TRUE;
      *pnwritten = blen;
      પાછા CURLE_OK;
    }
    /* Server closed before we got a response, this is an error */
    infof(data, "stream %d closed", stream->id);
    પાછા CURLE_SEND_ERROR;
  }

  result = Curl_bufq_write(&stream->sendbuf, buf, blen, pnwritten);
  જો(result)
    પાછા result;

  જો(eos && (blen == *pnwritten))
    stream->body_eos = TRUE;

  જો(eos || !Curl_bufq_is_empty(&stream->sendbuf)) {
    /* resume the potentially suspended stream */
    જો(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id)))
      પાછા CURLE_SEND_ERROR;
  }

  પાછા CURLE_OK;
}

સ્થિર CURLcode h2_submit(સંરચના h2_stream_ctx **pstream,
                          સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                          અચલ uint8_t *buf, size_t len,
                          bool eos, size_t *pnwritten)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  સંરચના h2_stream_ctx *stream = ખાલી;
  સંરચના dynhds h2_headers;
  nghttp2_nv *nva = ખાલી;
  અચલ શૂન્ય *body = ખાલી;
  size_t nheader, bodylen;
  nghttp2_data_provider data_prd;
  int32_t stream_id;
  nghttp2_priority_spec pri_spec;
  size_t nwritten;
  CURLcode result = CURLE_OK;
  uint32_t initial_win_size;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_headers, MAX_HTTP_RESP_HEADER_ગણતરી, DYN_HTTP_REQUEST);

  result = http2_data_setup(cf, data, &stream);
  જો(result)
    જાઓ out;

  result = Curl_h1_req_parse_read(
    &stream->h1, buf, len, ખાલી,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : ખાલી,
    0, &nwritten);
  જો(result)
    જાઓ out;
  *pnwritten = nwritten;
  જો(!stream->h1.done) {
    /* need more data */
    જાઓ out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  જો(result)
    જાઓ out;
  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nva = Curl_dynhds_to_nva(&h2_headers, &nheader);
  જો(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }

  h2_pri_spec(data, &pri_spec);
  જો(!Curl_ngh2_session_check_request_allowed(&ctx->ngh2))
    CURL_TRC_CF(data, cf, "send request NOT allowed (via nghttp2)");

  /* Check the initial windows size of the transfer (rate-limits?) and
   * send an updated settings on changes from previous value. */
  initial_win_size = cf_h2_initial_win_size(data);
  જો(initial_win_size != ctx->initial_win_size) {
    result = cf_h2_update_settings(ctx, initial_win_size);
    જો(result)
      જાઓ out;
  }

  પસંદગી(data->state.httpreq) {
  સ્થિતિ HTTPREQ_POST:
  સ્થિતિ HTTPREQ_POST_FORM:
  સ્થિતિ HTTPREQ_POST_MIME:
  સ્થિતિ HTTPREQ_PUT:
    data_prd.read_callback = req_body_read_callback;
    data_prd.source.ptr = ખાલી;
    stream_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, &data_prd, stream);
    તોડો;
  મૂળભૂત:
    stream_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, ખાલી, stream);
  }

  જો(stream_id < 0) {
    CURL_TRC_CF(data, cf, "send: nghttp2_submit_request error (%s)%d",
                nghttp2_strerror(stream_id), stream_id);
    result = CURLE_SEND_ERROR;
    જાઓ out;
  }

#ifdef CURLVERBOSE
#define MAX_ACC 60000  /* <64KB to account for some overhead */
  જો(Curl_trc_is_verbose(data)) {
    size_t acc = 0, i;

    infof(data, "[HTTP/2] [%d] OPENED stream for %s",
          stream_id, Curl_bufref_ptr(&data->state.url));
    માટે(i = 0; i < nheader; ++i) {
      acc += nva[i].namelen + nva[i].valuelen;

      infof(data, "[HTTP/2] [%d] [%.*s: %.*s]", stream_id,
            (પૂર્ણાંક)nva[i].namelen, nva[i].name,
            (પૂર્ણાંક)nva[i].valuelen, nva[i].કિંમત);
    }

    જો(acc > MAX_ACC) {
      infof(data, "[HTTP/2] Warning: The cumulative length of all "
            "headers exceeds %d bytes and that could cause the "
            "stream to be rejected.", MAX_ACC);
    }
  }
#endif

  stream->id = stream_id;

  body = (અચલ અક્ષર *)buf + *pnwritten;
  bodylen = len - *pnwritten;

  જો(bodylen || eos) {
    size_t n;
    result = cf_h2_body_send(cf, data, stream, body, bodylen, eos, &n);
    જો(!result)
      *pnwritten += n;
    નહીંતર જો(result == CURLE_AGAIN)
      result = CURLE_OK;
    નહીંતર {
      result = CURLE_SEND_ERROR;
    }
  }

out:
  CURL_TRC_CF(data, cf, "[%d] submit -> %d, %zu",
              stream ? stream->id : -1, (પૂર્ણાંક)result, *pnwritten);
  curlx_safefree(nva);
  *pstream = stream;
  Curl_dynhds_free(&h2_headers);
  પાછા result;
}

સ્થિર CURLcode cf_h2_send(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                           અચલ uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  સંરચના h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
  સંરચના cf_call_data save;
  CURLcode result = CURLE_OK, r2;

  CF_DATA_SAVE(save, cf, data);
  *pnwritten = 0;

  જો(!stream || stream->id == -1) {
    result = h2_submit(&stream, cf, data, buf, len, eos, pnwritten);
    જો(result)
      જાઓ out;
    DEBUGASSERT(stream);
  }
  નહીંતર જો(stream->body_eos) {
    /* We already wrote this, but CURLE_AGAIN-ed the call due to not
     * being able to flush stream->sendbuf. Make a 0-length write
     * to trigger flushing again.
     * If this works, we report to have written `len` bytes. */
    size_t n;
    DEBUGASSERT(eos);
    result = cf_h2_body_send(cf, data, stream, buf, 0, eos, &n);
    CURL_TRC_CF(data, cf, "[%d] cf_body_send last CHUNK -> %d, %zu, eos=%d",
                stream->id, (પૂર્ણાંક)result, n, eos);
    જો(result)
      જાઓ out;
    *pnwritten = len;
  }
  નહીંતર {
    result = cf_h2_body_send(cf, data, stream, buf, len, eos, pnwritten);
    CURL_TRC_CF(data, cf, "[%d] cf_body_send(len=%zu) -> %d, %zu, eos=%d",
                stream->id, len, (પૂર્ણાંક)result, *pnwritten, eos);
  }

  /* Call the nghttp2 send loop and flush to write ALL buffered data,
   * headers and/or request body completely out to the network */
  r2 = h2_progress_egress(cf, data);

  /* if the stream has been closed in egress handling (nghttp2 does that
   * when it does not like the headers, for example */
  જો(stream && stream->closed && !stream->resp_hds_complete) {
    infof(data, "stream %d closed", stream->id);
    result = CURLE_SEND_ERROR;
    જાઓ out;
  }
  નહીંતર જો(r2 && (r2 != CURLE_AGAIN)) {
    result = r2;
    જાઓ out;
  }

  જો(should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    જો(stream && stream->closed) {
      result = http2_handle_stream_close(cf, data, stream, pnwritten);
    }
    નહીંતર {
      CURL_TRC_CF(data, cf, "send: nothing to do in this session");
      result = CURLE_HTTP2;
    }
  }

out:
  જો(stream) {
    CURL_TRC_CF(data, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
                "eos=%d, h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                stream->id, len, (પૂર્ણાંક)result, *pnwritten,
                stream->body_eos,
                Curl_ngh2_session_get_stream_remote_window_size(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&stream->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  નહીંતર {
    CURL_TRC_CF(data, cf, "cf_send(len=%zu) -> %d, %zu, "
                "connection-window=%d, nw_send_buffer(%zu)",
                len, (પૂર્ણાંક)result, *pnwritten,
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_DATA_RESTORE(cf, save);
  પાછા result;
}

સ્થિર CURLcode cf_h2_flush(સંરચના Curl_cfilter *cf,
                            સંરચના Curl_easy *data)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  સંરચના h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
  સંરચના cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  જો(stream && !Curl_bufq_is_empty(&stream->sendbuf)) {
    /* resume the potentially suspended stream */
    જો(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id))) {
      result = CURLE_SEND_ERROR;
      જાઓ out;
    }
  }

  result = h2_progress_egress(cf, data);

out:
  જો(stream) {
    CURL_TRC_CF(data, cf, "[%d] flush -> %d, "
                "h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                stream->id, (પૂર્ણાંક)result,
                Curl_ngh2_session_get_stream_remote_window_size(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&stream->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  નહીંતર {
    CURL_TRC_CF(data, cf, "flush -> %d, "
                "connection-window=%d, nw_send_buffer(%zu)", (પૂર્ણાંક)result,
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_DATA_RESTORE(cf, save);
  પાછા result;
}

સ્થિર CURLcode cf_h2_adjust_pollset(સંરચના Curl_cfilter *cf,
                                     સંરચના Curl_easy *data,
                                     સંરચના easy_pollset *ps)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  સંરચના cf_call_data save;
  curl_socket_t sock;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  જો(!Curl_ngh2_valid(&ctx->ngh2))
    પાછા CURLE_OK;

  sock = Curl_conn_cf_get_socket(cf, data);
  Curl_pollset_check(data, ps, sock, &want_recv, &want_send);
  જો(want_recv || want_send) {
    સંરચના h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
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
  નહીંતર જો(ctx->sent_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_DATA_SAVE(save, cf, data);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CF_DATA_RESTORE(cf, save);
  }
  પાછા result;
}

સ્થિર CURLcode cf_h2_ctx_open(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  સંરચના h2_stream_ctx *stream;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  પૂર્ણાંક rc;
  nghttp2_session_callbacks *cbs = ખાલી;

  DEBUGASSERT(!ctx->ngh2.s);
  DEBUGASSERT(!ctx->ngh2.fatal_rv);
  DEBUGASSERT(ctx->initialized);

  rc = nghttp2_session_callbacks_new(&cbs);
  જો(rc) {
    failf(data, "Could not initialize nghttp2 callbacks");
    જાઓ out;
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
  જો(rc) {
    failf(data, "Could not initialize nghttp2");
    જાઓ out;
  }
  ctx->max_concurrent_streams = data->multi ?
    Curl_multi_max_concurrent_streams(data->multi) :
    DEFAULT_MAX_CONCURRENT_STREAMS;

  જો(ctx->via_h1_upgrade) {
    /* HTTP/1.1 Upgrade issued. H2 Settings have already been submitted
     * in the H1 request and we upgrade from there. This stream
     * is opened implicitly as #1. */
    uint8_t binsettings[H2_BINSETTINGS_LEN];
    ssize_t rclen;
    size_t binlen; /* length of the binsettings data */

    rclen = populate_binsettings(binsettings, data);

    જો(!curlx_sztouz(rclen, &binlen) || !binlen) {
      failf(data, "nghttp2 unexpectedly failed on pack_settings_payload");
      result = CURLE_FAILED_INIT;
      જાઓ out;
    }

    result = http2_data_setup(cf, data, &stream);
    જો(result)
      જાઓ out;
    DEBUGASSERT(stream);
    stream->id = 1;
    /* queue SETTINGS frame (again) */
    rc = Curl_ngh2_session_upgrade2(
      &ctx->ngh2, binsettings, binlen, data->state.httpreq == HTTPREQ_HEAD,
      ખાલી);
    જો(rc) {
      failf(data, "nghttp2_session_upgrade2() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      જાઓ out;
    }

    rc = Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, stream->id, stream);
    જો(rc) {
      infof(data, "http/2: failed to set user_data for stream %d",
            stream->id);
      DEBUGASSERT(0);
    }
    CURL_TRC_CF(data, cf, "created session via Upgrade");
  }
  નહીંતર {
    nghttp2_settings_entry iv[H2_SETTINGS_IV_LEN];
    size_t ivlen;

    ivlen = populate_settings(iv, data, ctx);
    rc = Curl_ngh2_submit_settings(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, iv, ivlen);
    જો(rc) {
      failf(data, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      જાઓ out;
    }
  }

  rc = Curl_ngh2_session_set_local_window_size(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, HTTP2_HUGE_WINDOW_SIZE);
  જો(rc) {
    failf(data, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    result = CURLE_HTTP2;
    જાઓ out;
  }

  /* all set, traffic will be send on connect */
  result = CURLE_OK;
  CURL_TRC_CF(data, cf, "[0] created h2 session%s",
              ctx->via_h1_upgrade ? " (via h1 upgrade)" : "");

out:
  જો(cbs)
    nghttp2_session_callbacks_del(cbs);
  પાછા result;
}

સ્થિર CURLcode cf_h2_connect(સંરચના Curl_cfilter *cf,
                              સંરચના Curl_easy *data,
                              bool *done)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  સંરચના cf_call_data save;
  bool first_time = FALSE;

  જો(cf->connected) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  /* Connect the lower filters first */
  જો(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    જો(result || !*done)
      પાછા result;
  }

  *done = FALSE;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(ctx->initialized);
  જો(!ctx->ngh2.s) {
    result = cf_h2_ctx_open(cf, data);
    જો(result)
      જાઓ out;
    first_time = TRUE;
  }

  જો(!first_time) {
    result = h2_progress_ingress(cf, data, 0);
    જો(result)
      જાઓ out;
  }

  /* Send out our SETTINGS and ACKs and such. If that blocks, we
   * have it buffered and can count this filter as being connected */
  result = h2_progress_egress(cf, data);
  જો(result && (result != CURLE_AGAIN))
    જાઓ out;

  *done = TRUE;
  cf->connected = TRUE;
  result = CURLE_OK;

out:
  CURL_TRC_CF(data, cf, "cf_connect() -> %d, %d, ", (પૂર્ણાંક)result, *done);
  CF_DATA_RESTORE(cf, save);
  પાછા result;
}

સ્થિર શૂન્ય cf_h2_destroy(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;

  (શૂન્ય)data;
  જો(ctx) {
    cf_h2_ctx_free(ctx);
    cf->ctx = ખાલી;
  }
}

સ્થિર CURLcode cf_h2_shutdown(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data, bool *done)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  સંરચના cf_call_data save;
  CURLcode result;
  પૂર્ણાંક rv;

  જો(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);

  જો(!ctx->sent_goaway) {
    ctx->sent_goaway = TRUE;
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, ctx->local_max_sid, 0,
      (અચલ uint8_t *)"shutdown", કદ("shutdown"));
    જો(rv) {
      failf(data, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      result = CURLE_SEND_ERROR;
      જાઓ out;
    }
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  result = CURLE_OK;
  જો(Curl_ngh2_session_want_write(&ctx->ngh2) ||
     !Curl_bufq_is_empty(&ctx->outbufq))
    result = h2_progress_egress(cf, data);
  જો(!result && Curl_ngh2_session_want_read(&ctx->ngh2))
    result = h2_progress_ingress(cf, data, 0);

  જો(result == CURLE_AGAIN)
    result = CURLE_OK;

  *done = (ctx->conn_closed ||
           (!result && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2) &&
            Curl_bufq_is_empty(&ctx->outbufq)));

out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  પાછા result;
}

સ્થિર CURLcode http2_data_pause(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 bool pause)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  સંરચના h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);

  DEBUGASSERT(data);
  જો(ctx && Curl_ngh2_valid(&ctx->ngh2) && stream) {
    CURLcode result;

    stream->write_paused = pause;
    result = cf_h2_update_local_win(cf, data, stream);
    જો(result)
      પાછા result;

    /* attempt to send the window update */
    (શૂન્ય)h2_progress_egress(cf, data);

    જો(!pause) {
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
  પાછા CURLE_OK;
}

સ્થિર CURLcode cf_h2_cntrl(સંરચના Curl_cfilter *cf,
                            સંરચના Curl_easy *data,
                            પૂર્ણાંક event, પૂર્ણાંક arg1, શૂન્ય *arg2)
{
  CURLcode result = CURLE_OK;
  સંરચના cf_call_data save;

  (શૂન્ય)arg2;

  CF_DATA_SAVE(save, cf, data);
  પસંદગી(event) {
  સ્થિતિ CF_CTRL_DATA_SETUP:
    તોડો;
  સ્થિતિ CF_CTRL_DATA_PAUSE:
    result = http2_data_pause(cf, data, (arg1 != 0));
    તોડો;
  સ્થિતિ CF_CTRL_FLUSH:
    result = cf_h2_flush(cf, data);
    તોડો;
  સ્થિતિ CF_CTRL_DATA_DONE:
    http2_data_done(cf, data);
    તોડો;
  સ્થિતિ CF_CTRL_CONN_INFO_UPDATE:
    જો(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 20;
      Curl_conn_set_multiplex(cf->conn);
    }
    તોડો;
  મૂળભૂત:
    તોડો;
  }
  CF_DATA_RESTORE(cf, save);
  પાછા result;
}

સ્થિર bool cf_h2_data_pending(સંરચના Curl_cfilter *cf,
                               અચલ સંરચના Curl_easy *data)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;

  જો(ctx && !Curl_bufq_is_empty(&ctx->inbufq))
    પાછા TRUE;
  પાછા cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

સ્થિર bool cf_h2_is_alive(સંરચના Curl_cfilter *cf,
                           સંરચના Curl_easy *data,
                           bool *આગત_pending)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  bool alive;
  સંરચના cf_call_data save;

  *આગત_pending = FALSE;
  CF_DATA_SAVE(save, cf, data);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
           http2_connisalive(cf, data, આગત_pending));
  CURL_TRC_CF(data, cf, "conn alive -> %d, input_pending=%d",
              alive, *આગત_pending);
  CF_DATA_RESTORE(cf, save);
  પાછા alive;
}

સ્થિર CURLcode cf_h2_keep_alive(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data)
{
  CURLcode result;
  સંરચના cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  result = http2_send_ping(cf, data);
  CF_DATA_RESTORE(cf, save);
  પાછા result;
}

સ્થિર CURLcode cf_h2_query(સંરચના Curl_cfilter *cf,
                            સંરચના Curl_easy *data,
                            પૂર્ણાંક query, પૂર્ણાંક *pres1, શૂન્ય *pres2)
{
  સંરચના cf_h2_ctx *ctx = cf->ctx;
  સંરચના cf_call_data save;
  size_t effective_max;

  પસંદગી(query) {
  સ્થિતિ CF_QUERY_MAX_CONCURRENT:
    DEBUGASSERT(pres1);

    CF_DATA_SAVE(save, cf, data);
    જો(!Curl_ngh2_valid(&ctx->ngh2) ||
       !Curl_ngh2_session_check_request_allowed(&ctx->ngh2)) {
      /* the limit is what we have in use right now */
      effective_max = cf->conn->attached_xfers;
    }
    નહીંતર {
      effective_max = ctx->max_concurrent_streams;
    }
    *pres1 = (effective_max > INT_MAX) ? INT_MAX : (પૂર્ણાંક)effective_max;
    CF_DATA_RESTORE(cf, save);
    પાછા CURLE_OK;
  સ્થિતિ CF_QUERY_STREAM_ERROR: {
    સંરચના h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    *pres1 = stream ? (પૂર્ણાંક)stream->error : 0;
    પાછા CURLE_OK;
  }
  સ્થિતિ CF_QUERY_NEED_FLUSH: {
    સંરચના h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    જો(!Curl_bufq_is_empty(&ctx->outbufq) ||
       (stream && !Curl_bufq_is_empty(&stream->sendbuf))) {
      *pres1 = TRUE;
      પાછા CURLE_OK;
    }
    તોડો;
  }
  સ્થિતિ CF_QUERY_HTTP_VERSION:
    *pres1 = 20;
    પાછા CURLE_OK;
  મૂળભૂત:
    તોડો;
  }
  પાછા cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

સંરચના Curl_cftype Curl_cft_nghttp2 = {
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

સ્થિર CURLcode http2_cfilter_add(સંરચના Curl_cfilter **pcf,
                                  સંરચના Curl_easy *data,
                                  સંરચના connectdata *conn,
                                  int8_t sockindex,
                                  bool via_h1_upgrade)
{
  સંરચના Curl_cfilter *cf = ખાલી;
  સંરચના cf_h2_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(data->conn);
  ctx = curlx_calloc(1, કદ(*ctx));
  જો(!ctx)
    જાઓ out;
  cf_h2_ctx_init(ctx, via_h1_upgrade);

  result = Curl_cf_create(&cf, &Curl_cft_nghttp2, ctx);
  જો(result)
    જાઓ out;

  ctx = ખાલી;
  Curl_conn_cf_add(data, conn, sockindex, cf);

out:
  જો(result)
    cf_h2_ctx_free(ctx);
  *pcf = result ? ખાલી : cf;
  પાછા result;
}

સ્થિર CURLcode http2_cfilter_ઉમેરો_after(સંરચના Curl_cfilter *cf,
                                           સંરચના Curl_easy *data,
                                           bool via_h1_upgrade)
{
  સંરચના Curl_cfilter *cf_h2 = ખાલી;
  સંરચના cf_h2_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (શૂન્ય)data;
  ctx = curlx_calloc(1, કદ(*ctx));
  જો(!ctx)
    જાઓ out;
  cf_h2_ctx_init(ctx, via_h1_upgrade);

  result = Curl_cf_create(&cf_h2, &Curl_cft_nghttp2, ctx);
  જો(result)
    જાઓ out;

  ctx = ખાલી;
  Curl_conn_cf_ઉમેરો_after(cf, cf_h2);

out:
  જો(result)
    cf_h2_ctx_free(ctx);
  પાછા result;
}

bool Curl_http2_may_switch(સંરચના Curl_easy *data)
{
  જો(Curl_conn_http_version(data, data->conn) < 20 &&
     (data->state.http_neg.wanted & CURL_HTTP_V2x) &&
     data->state.http_neg.h2_prior_knowledge) {
#ifndef CURL_DISABLE_PROXY
    જો(data->conn->bits.origin_is_proxy) {
      infof(data, "Ignoring HTTP/2 prior knowledge due to proxy");
      પાછા FALSE;
    }
#endif
    પાછા TRUE;
  }
  પાછા FALSE;
}

CURLcode Curl_http2_switch(સંરચના Curl_easy *data)
{
  સંરચના Curl_cfilter *cf;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, data->conn) < 20);

  result = http2_cfilter_add(&cf, data, data->conn, FIRSTSOCKET, FALSE);
  જો(result)
    પાછા result;
  CURL_TRC_CF(data, cf, "switching connection to HTTP/2");

  જો(cf->next) {
    bool done;
    પાછા Curl_conn_cf_connect(cf, data, &done);
  }
  પાછા CURLE_OK;
}

CURLcode Curl_http2_switch_at(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data)
{
  સંરચના Curl_cfilter *cf_h2;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, data->conn) < 20);

  result = http2_cfilter_ઉમેરો_after(cf, data, FALSE);
  જો(result)
    પાછા result;

  cf_h2 = cf->next;

  જો(cf_h2->next) {
    bool done;
    પાછા Curl_conn_cf_connect(cf_h2, data, &done);
  }
  પાછા CURLE_OK;
}

CURLcode Curl_http2_upgrade(સંરચના Curl_easy *data,
                            સંરચના connectdata *conn, int8_t sockindex,
                            અચલ અક્ષર *mem, size_t nread)
{
  સંરચના Curl_cfilter *cf;
  સંરચના cf_h2_ctx *ctx;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, conn) < 20);

  result = http2_cfilter_add(&cf, data, conn, sockindex, TRUE);
  જો(result)
    પાછા result;
  CURL_TRC_CF(data, cf, "upgrading connection to HTTP/2");

  DEBUGASSERT(cf->cft == &Curl_cft_nghttp2);
  ctx = cf->ctx;

  data->req.httpversion_sent = 20; /* it is an h2 request now */
  data->req.header = TRUE; /* we expect the real response to come in h2 */
  data->req.headerline = 0; /* restart the header line counter */

  જો(nread > 0) {
    /* Remaining data from the protocol switch reply is already using
     * the switched protocol, ie. HTTP/2. We add that to the network
     * inbufq. */
    size_t copied;

    result = Curl_bufq_write(&ctx->inbufq,
                             (અચલ unsigned અક્ષર *)mem, nread, &copied);
    જો(result) {
      failf(data, "error on copying HTTP Upgrade response: %d", (પૂર્ણાંક)result);
      પાછા CURLE_RECV_ERROR;
    }
    જો(copied < nread) {
      failf(data, "connection buffer size could not take all data "
            "from HTTP Upgrade response header: copied=%zu, datalen=%zu",
            copied, nread);
      પાછા CURLE_HTTP2;
    }
    infof(data, "Copied HTTP/2 data in stream buffer to connection buffer"
          " after upgrade: len=%zu", nread);
  }

  જો(cf->next) {
    bool done;
    result = Curl_conn_cf_connect(cf, data, &done);
    જો(!result)
      cf->cft->cntrl(cf, data, CF_CTRL_CONN_INFO_UPDATE, 0, ખાલી);
  }
  પાછા result;
}

/* Only call this function for a transfer that already got an HTTP/2
   CURLE_HTTP2_STREAM error! */
bool Curl_h2_http_1_1_error(સંરચના Curl_easy *data)
{
  જો(Curl_conn_http_version(data, data->conn) == 20) {
    પૂર્ણાંક err = Curl_conn_get_stream_error(data, data->conn, FIRSTSOCKET);
    પાછા err == NGHTTP2_HTTP_1_1_REQUIRED;
  }
  પાછા FALSE;
}

શૂન્ય *Curl_nghttp2_malloc(size_t size, શૂન્ય *user_data)
{
  (શૂન્ય)user_data;
  પાછા Curl_cmalloc(size);
}

શૂન્ય Curl_nghttp2_free(શૂન્ય *ptr, શૂન્ય *user_data)
{
  (શૂન્ય)user_data;
  Curl_cfree(ptr);
}

શૂન્ય *Curl_nghttp2_calloc(size_t nmemb, size_t size, શૂન્ય *user_data)
{
  (શૂન્ય)user_data;
  પાછા Curl_ccalloc(nmemb, size);
}

શૂન્ય *Curl_nghttp2_realloc(શૂન્ય *ptr, size_t size, શૂન્ય *user_data)
{
  (શૂન્ય)user_data;
  પાછા Curl_crealloc(ptr, size);
}

#નહીંતર /* CURL_DISABLE_HTTP || !USE_NGHTTP2 */

/* Satisfy external references even if http2 is not compiled in. */

અક્ષર *curl_pushheader_bynum(સંરચના curl_pushheaders *h, size_t અંક)
{
  (શૂન્ય)h;
  (શૂન્ય)અંક;
  પાછા ખાલી;
}

અક્ષર *curl_pushheader_byname(સંરચના curl_pushheaders *h, અચલ અક્ષર *name)
{
  (શૂન્ય)h;
  (શૂન્ય)name;
  પાછા ખાલી;
}

#endif /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
