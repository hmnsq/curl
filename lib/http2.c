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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಯದಿ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#ಅನ್ತರ್ಭಾವ <nghttp2/nghttp2.h>

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "bufq.h"
#ಅನ್ತರ್ಭಾವ "u32_ptrset.h"
#ಅನ್ತರ್ಭಾವ "http1.h"
#ಅನ್ತರ್ಭಾವ "http2.h"
#ಅನ್ತರ್ಭಾವ "http.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "curlx/base64.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "ngh2_shim.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "urlapi-int.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "bufref.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"
#ಅನ್ತರ್ಭಾವ "headers.h"
#ಅನ್ತರ್ಭಾವ "curl_share.h"

#ಯದಿ NGHTTP2_VERSION_ಅಂಕೆ < 0x010f00
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
#ಯದಿ NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
#define H2_STREAM_WINDOW_SIZE_INITIAL  (64 * 1024)
#ಅನ್ಯಥಾ
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

ಸಂರಚನಾ cf_h2_ctx {
  ಸಂರಚನಾ curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  ಸಂರಚನಾ cf_call_data call_data;

  ಸಂರಚನಾ bufq inbufq;           /* network input */
  ಸಂರಚನಾ bufq outbufq;          /* network output */
  ಸಂರಚನಾ bufc_pool stream_bufcp; /* spares for stream buffers */
  ಸಂರಚನಾ dynbuf scratch;        /* scratch buffer for temp use */

  ಸಂರಚನಾ u32_ptrset streams; /* hash of `data->mid` to `h2_stream_ctx` */
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
  BIT(enable_ತಳ್ಳು);
  BIT(nw_out_blocked);
};

/* How to access `call_data` from a cf_h2 filter */
#undef CF_CTX_CALL_DATA
#define CF_CTX_CALL_DATA(cf) ((ಸಂರಚನಾ cf_h2_ctx *)(cf)->ctx)->call_data

/**
 * All about the H2 internals of a stream
 */
ಸಂರಚನಾ h2_stream_ctx {
  ಸಂರಚನಾ bufq sendbuf; /* request buffer */
  ಸಂರಚನಾ h1_req_parser h1; /* parsing the request */
  ಸಂರಚನಾ dynhds resp_trailers; /* response trailer fields */
  size_t resp_hds_len; /* amount of response header bytes in recvbuf */
  curl_off_t xfer_id;  /* transfer `id` owning this stream */
  curl_off_t nrcvd_data;  /* number of DATA bytes received */

  ವರ್ಣ **ತಳ್ಳು_headers;       /* allocated array */
  size_t ತಳ್ಳು_headers_used;  /* number of entries filled in */
  size_t ತಳ್ಳು_headers_alloc; /* number of entries allocated */

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

ಸ್ಥಿರ ಶೂನ್ಯ free_ತಳ್ಳು_headers(ಸಂರಚನಾ h2_stream_ctx *stream)
{
  size_t i;
  ಪ್ರತಿ(i = 0; i < stream->ತಳ್ಳು_headers_used; i++)
    curlx_free(stream->ತಳ್ಳು_headers[i]);
  curlx_safefree(stream->ತಳ್ಳು_headers);
  stream->ತಳ್ಳು_headers_used = 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ h2_stream_ctx_free(ಸಂರಚನಾ h2_stream_ctx *stream)
{
  Curl_bufq_free(&stream->sendbuf);
  Curl_h1_req_parse_free(&stream->h1);
  Curl_dynhds_free(&stream->resp_trailers);
  free_ತಳ್ಳು_headers(stream);
  curlx_free(stream);
}

ಸ್ಥಿರ ಶೂನ್ಯ h2_stream_hash_free(uint32_t id, ಶೂನ್ಯ *stream)
{
  (ಶೂನ್ಯ)id;
  DEBUGASSERT(stream);
  h2_stream_ctx_free((ಸಂರಚನಾ h2_stream_ctx *)stream);
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_h2_ctx_init(ಸಂರಚನಾ cf_h2_ctx *ctx, bool via_h1_upgrade)
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

ಸ್ಥಿರ ಶೂನ್ಯ cf_h2_ctx_free(ಸಂರಚನಾ cf_h2_ctx *ctx)
{
  ಯದಿ(ctx && ctx->initialized) {
    Curl_ngh2_del(&ctx->ngh2);
    Curl_bufq_free(&ctx->inbufq);
    Curl_bufq_free(&ctx->outbufq);
    Curl_bufcp_free(&ctx->stream_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    memset(ctx, 0, ಪರಿಮಾಣ(*ctx));
  }
  curlx_free(ctx);
}

ಸ್ಥಿರ uint32_t cf_h2_initial_win_size(ಸಂರಚನಾ Curl_easy *data)
{
#ಯದಿ NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
  /* If the transfer has a rate-limit lower than the default initial
   * stream window size, use that. It needs to be at least 8k or servers
   * may be unhappy. */
  curl_off_t rps = Curl_rlimit_per_step(&data->progress.dl.rlimit);
  ಯದಿ((rps > 0) && (rps < H2_STREAM_WINDOW_SIZE_INITIAL))
    ಪ್ರತ್ಯಾಗಮ CURLMAX((uint32_t)rps, 8192);
#endif
  ಪ್ರತ್ಯಾಗಮ H2_STREAM_WINDOW_SIZE_INITIAL;
}

ಸ್ಥಿರ size_t populate_settings(nghttp2_settings_entry *iv,
                                ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ cf_h2_ctx *ctx)
{
  iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
  iv[0].ಮೌಲ್ಯ = Curl_multi_max_concurrent_streams(data->multi);

  iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
  iv[1].ಮೌಲ್ಯ = cf_h2_initial_win_size(data);
  ಯದಿ(ctx)
    ctx->initial_win_size = iv[1].ಮೌಲ್ಯ;
  iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_ತಳ್ಳು;
  iv[2].ಮೌಲ್ಯ = !!data->multi->ತಳ್ಳು_cb;

  ಪ್ರತ್ಯಾಗಮ 3;
}

ಸ್ಥಿರ ssize_t populate_binsettings(uint8_t *binsettings,
                                    ಸಂರಚನಾ Curl_easy *data)
{
  nghttp2_settings_entry iv[H2_SETTINGS_IV_LEN];
  size_t ivlen;

  ivlen = populate_settings(iv, data, ರಿಕ್ತ);
  /* this returns number of bytes it wrote or a negative number on error. */
  ಪ್ರತ್ಯಾಗಮ nghttp2_pack_settings_payload(binsettings, H2_BINSETTINGS_LEN,
                                       iv, ivlen);
}

ಸ್ಥಿರ CURLcode cf_h2_update_settings(ಸಂರಚನಾ cf_h2_ctx *ctx,
                                      uint32_t initial_win_size)
{
  nghttp2_settings_entry entry;
  entry.settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
  entry.ಮೌಲ್ಯ = initial_win_size;
  ಯದಿ(Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, &entry, 1))
    ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
  ctx->initial_win_size = initial_win_size;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#define H2_STREAM_CTX(ctx, data)                                         \
  ((ಸಂರಚನಾ h2_stream_ctx *)(                                             \
    (data) ? Curl_u32_ptrset_get(&(ctx)->streams, (data)->mid) : ರಿಕ್ತ))

ಸ್ಥಿರ ಸಂರಚನಾ h2_stream_ctx *h2_stream_ctx_create(ಸಂರಚನಾ cf_h2_ctx *ctx,
                                                  ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ h2_stream_ctx *stream;

  stream = curlx_calloc(1, ಪರಿಮಾಣ(*stream));
  ಯದಿ(!stream)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  stream->id = -1;
  stream->xfer_id = data->id;
  stream->mid = data->mid;
  Curl_bufq_initp(&stream->sendbuf, &ctx->stream_bufcp,
                  H2_STREAM_SEND_CHUNKS, BUFQ_OPT_NONE);
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_ಸಾಲು_LEN);
  Curl_dynhds_init(&stream->resp_trailers,
                   MAX_HTTP_RESP_HEADER_ಎಣಿಕೆ, DYN_HTTP_REQUEST);
  stream->bodystarted = FALSE;
  stream->status_code = -1;
  stream->closed = FALSE;
  stream->close_handled = FALSE;
  stream->error = NGHTTP2_NO_ERROR;
  stream->local_window_size = H2_STREAM_WINDOW_SIZE_INITIAL;
  stream->nrcvd_data = 0;
  ಪ್ರತ್ಯಾಗಮ stream;
}

ಸ್ಥಿರ ಸಂರಚನಾ Curl_easy *cf_h2_get_stream_xfer(ಸಂರಚನಾ Curl_cfilter *cf,
                                               int32_t stream_id,
                                               ಸಂರಚನಾ h2_stream_ctx **pstream)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ h2_stream_ctx *stream =
    Curl_ngh2_session_get_stream_user_data(&ctx->ngh2, stream_id);
  ಯದಿ(pstream)
    *pstream = stream;
  ಯದಿ(stream && (stream->mid != UINT32_MAX)) {
    ಸಂರಚನಾ Curl_easy *data, *call_data = CF_DATA_CURRENT(cf);
    ಯದಿ(!call_data || !call_data->multi) {
      DEBUGASSERT(0);
      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    data = Curl_multi_get_easy(call_data->multi, stream->mid);
    ಯದಿ(data && (data->id == stream->xfer_id))
      ಪ್ರತ್ಯಾಗಮ data;
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ifdef NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
ಸ್ಥಿರ int32_t cf_h2_get_desired_local_win(ಸಂರಚನಾ Curl_cfilter *cf,
                                           ಸಂರಚನಾ Curl_easy *data)
{
  curl_off_t avail = Curl_rlimit_avail(&data->progress.dl.rlimit, ರಿಕ್ತ);

  (ಶೂನ್ಯ)cf;
  ಯದಿ(avail < CURL_OFF_T_MAX) { /* limit in place */
    ಯದಿ(avail <= 0)
      ಪ್ರತ್ಯಾಗಮ 0;
    ಅನ್ಯಥಾ ಯದಿ(avail < H2_STREAM_WINDOW_SIZE_MAX)
      ಪ್ರತ್ಯಾಗಮ (int32_t)avail;
  }
  ಪ್ರತ್ಯಾಗಮ H2_STREAM_WINDOW_SIZE_MAX;
}

ಸ್ಥಿರ CURLcode cf_h2_update_local_win(ಸಂರಚನಾ Curl_cfilter *cf,
                                       ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ h2_stream_ctx *stream)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  int32_t dwsize;
  ಪೂರ್ಣಾಙ್ಕ rv;

  dwsize = (stream->write_paused || stream->xfer_result) ?
           0 : cf_h2_get_desired_local_win(cf, data);
  ಯದಿ(dwsize != stream->local_window_size) {
    int32_t wsize = Curl_ngh2_session_get_stream_effective_local_window_size(
      &ctx->ngh2, stream->id);
    ಯದಿ(dwsize > wsize) {
      rv = Curl_ngh2_session_set_local_window_size(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize);
      ಯದಿ(rv) {
        failf(data, "[%d] nghttp2 set_local_window_size(%d) failed: "
              "%s(%d)", stream->id, dwsize, nghttp2_strerror(rv), rv);
        ಪ್ರತ್ಯಾಗಮ CURLE_HTTP2;
      }
      rv = Curl_ngh2_submit_window_update(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize - wsize);
      ಯದಿ(rv) {
        failf(data, "[%d] nghttp2_submit_window_update() failed: "
              "%s(%d)", stream->id, nghttp2_strerror(rv), rv);
        ಪ್ರತ್ಯಾಗಮ CURLE_HTTP2;
      }
      stream->local_window_size = dwsize;
      CURL_TRC_CF(data, cf, "[%d] local window update by %d",
                  stream->id, dwsize - wsize);
    }
    ಅನ್ಯಥಾ {
      rv = Curl_ngh2_session_set_local_window_size(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize);
      ಯದಿ(rv) {
        failf(data, "[%d] nghttp2_session_set_local_window_size() failed: "
              "%s(%d)", stream->id, nghttp2_strerror(rv), rv);
        ಪ್ರತ್ಯಾಗಮ CURLE_HTTP2;
      }
      stream->local_window_size = dwsize;
      CURL_TRC_CF(data, cf, "[%d] local window size now %d",
                  stream->id, dwsize);
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#ಅನ್ಯಥಾ /* NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

ಸ್ಥಿರ CURLcode cf_h2_update_local_win(ಸಂರಚನಾ Curl_cfilter *cf,
                                       ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ h2_stream_ctx *stream)
{
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)stream;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
#endif /* !NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

ಸ್ಥಿರ CURLcode http2_data_setup(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ h2_stream_ctx **pstream)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ h2_stream_ctx *stream;

  (ಶೂನ್ಯ)cf;
  DEBUGASSERT(data);

  ಯದಿ(!data)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  stream = H2_STREAM_CTX(ctx, data);
  ಯದಿ(stream) {
    *pstream = stream;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  stream = h2_stream_ctx_create(ctx, data);
  ಯದಿ(!stream)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  ಯದಿ(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    h2_stream_ctx_free(stream);
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  *pstream = stream;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode nw_out_flush(ಸಂರಚನಾ Curl_cfilter *cf,
                             ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  ಯದಿ(Curl_bufq_is_empty(&ctx->outbufq))
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  result = Curl_cf_send_bufq(cf->next, data, &ctx->outbufq, ರಿಕ್ತ, 0,
                             &nwritten);
  ಯದಿ(result) {
    ಯದಿ(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_out_blocked = 1;
    }
    ಪ್ರತ್ಯಾಗಮ result;
  }
  ಪ್ರತ್ಯಾಗಮ Curl_bufq_is_empty(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

ಸ್ಥಿರ ಶೂನ್ಯ http2_data_done(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);

  DEBUGASSERT(ctx);
  ಯದಿ(!stream || !ctx->initialized)
    ಪ್ರತ್ಯಾಗಮ;

  ಯದಿ(Curl_ngh2_valid(&ctx->ngh2)) {
    bool flush_egress = FALSE;
    /* returns error if stream not known, which is fine here */
    (ಶೂನ್ಯ)Curl_ngh2_session_set_stream_user_data(&ctx->ngh2, stream->id, ರಿಕ್ತ);

    ಯದಿ(!stream->closed && stream->id > 0) {
      /* RST_STREAM */
      CURL_TRC_CF(data, cf, "[%d] premature DATA_DONE, RST stream",
                  stream->id);
      stream->closed = TRUE;
      stream->reset = TRUE;
      Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                  stream->id, NGHTTP2_STREAM_CLOSED);
      flush_egress = TRUE;
    }

    ಯದಿ(flush_egress) {
      (ಶೂನ್ಯ)Curl_ngh2_session_send(&ctx->ngh2);
      (ಶೂನ್ಯ)nw_out_flush(cf, data);
    }
  }

  Curl_u32_ptrset_unset(&ctx->streams, data->mid);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ h2_client_new(ಸಂರಚನಾ Curl_cfilter *cf,
                         nghttp2_session_callbacks *cbs)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { ರಿಕ್ತ, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  ಪೂರ್ಣಾಙ್ಕ rc = nghttp2_option_new(&o);
  ಯದಿ(rc)
    ಪ್ರತ್ಯಾಗಮ rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_set_no_auto_window_update(o, 1);
#ಯದಿ NGHTTP2_VERSION_ಅಂಕೆ >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_set_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#endif
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  ಪ್ರತ್ಯಾಗಮ rc;
}

/*
 * Returns nonzero if current HTTP/2 session should be closed.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ should_close_session(ಸಂರಚನಾ cf_h2_ctx *ctx)
{
  ಪ್ರತ್ಯಾಗಮ ctx->drain_total == 0 && !Curl_ngh2_session_want_read(&ctx->ngh2) &&
    !Curl_ngh2_session_want_write(&ctx->ngh2);
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
ಸ್ಥಿರ CURLcode h2_process_pending_ಒಳಹರಿವು(ಸಂರಚನಾ Curl_cfilter *cf,
                                         ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  ಧ್ರುವ unsigned ವರ್ಣ *buf;
  size_t blen, nread;
  ssize_t rv;

  ಯಾವತ್(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {
    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ಧ್ರುವ uint8_t *)buf, blen);
    ಯದಿ(!curlx_sztouz(rv, &nread)) {
      failf(data, "nghttp2 recv error %zd: %s", rv, nghttp2_strerror((ಪೂರ್ಣಾಙ್ಕ)rv));
      ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    ಯದಿ(Curl_bufq_is_empty(&ctx->inbufq)) {
      ಭಙ್ಗ;
    }
    ಅನ್ಯಥಾ {
      CURL_TRC_CF(data, cf, "process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }

  ಯದಿ(Curl_ngh2_session_check_request_allowed(&ctx->ngh2) == 0) {
    /* No more requests are allowed in the current session, so
       the connection may not be reused. This is set when a
       GOAWAY frame has been received or when the limit of stream
       identifiers has been reached. */
    CURL_TRC_M(data, "http/2: No new requests allowed");
    connclose(cf->conn);
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * The server may send us data at any point (e.g. PING frames). Therefore, we
 * cannot assume that an HTTP/2 socket is dead because it is readable.
 *
 * Check the lower filters first and, if successful, peek at the socket
 * and distinguish between closed and data.
 */
ಸ್ಥಿರ bool http2_connisalive(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                              bool *ಒಳಹರಿವು_pending)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *ಒಳಹರಿವು_pending = FALSE;
  ಯದಿ(!cf->next || !cf->next->cft->is_alive(cf->next, data, ಒಳಹರಿವು_pending))
    ಪ್ರತ್ಯಾಗಮ FALSE;

  ಯದಿ(*ಒಳಹರಿವು_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode result;
    size_t nread;

    *ಒಳಹರಿವು_pending = FALSE;
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(data, cf, "connisalive, recv pending input -> %d, %zu",
                (ಪೂರ್ಣಾಙ್ಕ)result, nread);
    ಯದಿ(!result) {
      result = h2_process_pending_ಒಳಹರಿವು(cf, data);
      ಯದಿ(result)
        /* immediate error, considered dead */
        alive = FALSE;
      ಅನ್ಯಥಾ {
        alive = !should_close_session(ctx);
      }
    }
    ಅನ್ಯಥಾ ಯದಿ(result != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  ಪ್ರತ್ಯಾಗಮ alive;
}

ಸ್ಥಿರ CURLcode http2_send_ping(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  ಪೂರ್ಣಾಙ್ಕ rc;

  rc = Curl_ngh2_submit_ping(&ctx->ngh2, 0, ZERO_NULL);
  ಯದಿ(rc) {
    failf(data, "nghttp2_submit_ping() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    ಪ್ರತ್ಯಾಗಮ CURLE_HTTP2;
  }

  rc = Curl_ngh2_session_send(&ctx->ngh2);
  ಯದಿ(rc) {
    failf(data, "nghttp2_session_send() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Store nghttp2 version info in this buffer.
 */
ಶೂನ್ಯ Curl_http2_ver(ವರ್ಣ *p, size_t len)
{
  nghttp2_info *h2 = nghttp2_version(0);
  (ಶೂನ್ಯ)curl_msnprintf(p, len, "nghttp2/%s", h2->version_str);
}

/*
 * The implementation of nghttp2_send_callback type. Here we write |data| with
 * size |length| to the network and return the number of bytes actually
 * written. See the documentation of nghttp2_send_callback for the details.
 */
ಸ್ಥಿರ ssize_t send_callback(nghttp2_session *h2,
                             ಧ್ರುವ uint8_t *buf, size_t blen, ಪೂರ್ಣಾಙ್ಕ flags,
                             ಶೂನ್ಯ *userp)
{
  ಸಂರಚನಾ Curl_cfilter *cf = userp;
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result = CURLE_OK;

  (ಶೂನ್ಯ)h2;
  (ಶೂನ್ಯ)flags;
  DEBUGASSERT(data);

  ಯದಿ(!cf->connected)
    result = Curl_bufq_write(&ctx->outbufq, buf, blen, &nwritten);
  ಅನ್ಯಥಾ
    result = Curl_cf_send_bufq(cf->next, data, &ctx->outbufq, buf, blen,
                               &nwritten);

  ಯದಿ(result) {
    ಯದಿ(result == CURLE_AGAIN) {
      ctx->nw_out_blocked = 1;
      ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(data, "Failed sending HTTP2 data");
    ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  ಯದಿ(!nwritten) {
    ctx->nw_out_blocked = 1;
    ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_WOULDBLOCK;
  }
  ಪ್ರತ್ಯಾಗಮ (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

/* We pass a pointer to this struct in the push callback, but the contents of
   the struct are hidden from the user. */
ಸಂರಚನಾ curl_pushheaders {
  ಸಂರಚನಾ Curl_easy *data;
  ಸಂರಚನಾ h2_stream_ctx *stream;
  ಧ್ರುವ nghttp2_ತಳ್ಳು_promise *frame;
};

/*
 * push header access function. Only to be used from within the push callback
 */
ವರ್ಣ *curl_pushheader_bynum(ಸಂರಚನಾ curl_pushheaders *h, size_t ಅಂಕೆ)
{
  /* Verify that we got a good easy handle in the push header struct, mostly to
     detect rubbish input fast(er). */
  ಯದಿ(!h || !GOOD_EASY_HANDLE(h->data))
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  ಅನ್ಯಥಾ {
    ಯದಿ(h->stream && ಅಂಕೆ < h->stream->ತಳ್ಳು_headers_used)
      ಪ್ರತ್ಯಾಗಮ h->stream->ತಳ್ಳು_headers[ಅಂಕೆ];
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * push header access function. Only to be used from within the push callback
 */
ವರ್ಣ *curl_pushheader_byname(ಸಂರಚನಾ curl_pushheaders *h, ಧ್ರುವ ವರ್ಣ *name)
{
  ಸಂರಚನಾ h2_stream_ctx *stream;
  size_t len;
  size_t i;
  /* Verify that we got a good easy handle in the push header struct,
     mostly to detect rubbish input fast(er). Also empty header name
     is rubbish too. We have to allow ":" at the beginning of
     the header, but header == ":" must be rejected. If we have ':' in
     the middle of header, it could be matched in middle of the value,
     this is because we do prefix match.*/
  ಯದಿ(!h || !GOOD_EASY_HANDLE(h->data) || !name || !name[0] ||
     !strcmp(name, ":") || strchr(name + 1, ':'))
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  stream = h->stream;
  ಯದಿ(!stream)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  len = strlen(name);
  ಪ್ರತಿ(i = 0; i < stream->ತಳ್ಳು_headers_used; i++) {
    ಯದಿ(!strncmp(name, stream->ತಳ್ಳು_headers[i], len)) {
      /* sub-match, make sure that it is followed by a colon */
      ಯದಿ(stream->ತಳ್ಳು_headers[i][len] != ':')
        ಅನುವರ್ತ;
      ಪ್ರತ್ಯಾಗಮ &stream->ತಳ್ಳು_headers[i][len + 1];
    }
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸ್ಥಿರ ಸಂರಚನಾ Curl_easy *h2_duphandle(ಸಂರಚನಾ Curl_cfilter *cf,
                                      ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_easy *second = curl_easy_init();
  ಯದಿ(second) {
    ಸಂರಚನಾ h2_stream_ctx *second_stream;
    http2_data_setup(cf, second, &second_stream);
    second->state.weight = data->state.weight;
    ಯದಿ(data->share)
      (ಶೂನ್ಯ)Curl_share_easy_link(second, data->share);
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
  ಪ್ರತ್ಯಾಗಮ second;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ set_transfer_url(ಸಂರಚನಾ Curl_easy *newhandle,
                            ಸಂರಚನಾ curl_pushheaders *hp,
                            ಸಂರಚನಾ Curl_easy *data)
{
  ಧ್ರುವ ವರ್ಣ *v;
  CURLUcode uc;
  ವರ್ಣ *url = ರಿಕ್ತ;
  ಪೂರ್ಣಾಙ್ಕ rc = 0;
  CURLU *u = curl_url();

  ಯದಿ(!u)
    ಪ್ರತ್ಯಾಗಮ 5;

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_SCHEME);
  ಯದಿ(v) {
    uc = curl_url_set(u, CURLUPART_SCHEME, v, 0);
    ಯದಿ(uc) {
      rc = 1;
      ಗಚ್ಛ fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_AUTHORITY);
  ಯದಿ(v) {
    uc = Curl_url_set_authority(u, v);
    ಯದಿ(uc) {
      rc = 2;
      ಗಚ್ಛ fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_PATH);
  ಯದಿ(v) {
    uc = curl_url_set(u, CURLUPART_PATH, v, 0);
    ಯದಿ(uc) {
      rc = 3;
      ಗಚ್ಛ fail;
    }
  }

  /* We can only allow PUSH of resource from the same origin, e.g.
   * scheme + hostname + port */
  ಯದಿ(!Curl_url_same_origin(data->state.uh, u)) {
    rc = 1;
    ಗಚ್ಛ fail;
  }

  uc = curl_url_get(u, CURLUPART_URL, &url, 0);
  ಯದಿ(uc)
    rc = 4;
fail:
  ಯದಿ(rc) {
    curl_url_cleanup(u);
    ಪ್ರತ್ಯಾಗಮ rc;
  }

  DEBUGASSERT(!newhandle->state.uh);
  newhandle->state.uh = u;
  Curl_bufref_set(&newhandle->state.url, url, 0, curl_free);
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ discard_newhandle(ಸಂರಚನಾ Curl_cfilter *cf,
                              ಸಂರಚನಾ Curl_easy *newhandle)
{
  http2_data_done(cf, newhandle);
  (ಶೂನ್ಯ)Curl_close(&newhandle);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ತಳ್ಳು_promise(ಸಂರಚನಾ Curl_cfilter *cf,
                        ಸಂರಚನಾ Curl_easy *data,
                        ಧ್ರುವ nghttp2_ತಳ್ಳು_promise *frame)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  ಪೂರ್ಣಾಙ್ಕ rv; /* one of the CURL_PUSH_* defines */

  CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE received",
              frame->promised_stream_id);
  ಯದಿ(data->multi->ತಳ್ಳು_cb) {
    ಸಂರಚನಾ h2_stream_ctx *stream;
    ಸಂರಚನಾ h2_stream_ctx *newstream;
    ಸಂರಚನಾ curl_pushheaders heads;
    CURLMcode mresult;
    CURLcode result;
    /* clone the parent */
    ಸಂರಚನಾ Curl_easy *newhandle = h2_duphandle(cf, data);
    ಯದಿ(!newhandle) {
      infof(data, "failed to duplicate handle");
      rv = CURL_ತಳ್ಳು_DENY; /* FAIL HARD */
      ಗಚ್ಛ fail;
    }

    stream = H2_STREAM_CTX(ctx, data);
    ಯದಿ(!stream) {
      failf(data, "Internal NULL stream");
      discard_newhandle(cf, newhandle);
      rv = CURL_ತಳ್ಳು_DENY;
      ಗಚ್ಛ fail;
    }

    heads.data = data;
    heads.stream = stream;
    heads.frame = frame;

    rv = set_transfer_url(newhandle, &heads, data);
    ಯದಿ(rv) {
      CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE, failed to set URL -> %d",
                  frame->promised_stream_id, rv);
      discard_newhandle(cf, newhandle);
      rv = CURL_ತಳ್ಳು_DENY;
      ಗಚ್ಛ fail;
    }

    {
      ಸಂರಚನಾ Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, multi_ತಳ್ಳು_cb);
      rv = data->multi->ತಳ್ಳು_cb(data, newhandle,
                                stream->ತಳ್ಳು_headers_used, &heads,
                                data->multi->ತಳ್ಳು_userp);
      CURL_CBAPI_END(&guard);
    }

    /* free the headers again */
    free_ತಳ್ಳು_headers(stream);

    ಯದಿ(rv) {
      DEBUGASSERT((rv > CURL_ತಳ್ಳು_OK) && (rv <= CURL_ತಳ್ಳು_ERROROUT));
      /* denied, kill off the new handle again */
      CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE, denied by application -> %d",
                  frame->promised_stream_id, rv);
      discard_newhandle(cf, newhandle);
      ಗಚ್ಛ fail;
    }

    /* approved, add to the multi handle for processing. This
     * assigns newhandle->mid. For the new `mid` we assign the
     * h2_stream instance and remember the stream_id already known. */
    mresult = Curl_multi_add_perform(data->multi, newhandle, cf->conn);
    ಯದಿ(mresult) {
      infof(data, "failed to add handle to multi");
      discard_newhandle(cf, newhandle);
      rv = CURL_ತಳ್ಳು_DENY;
      ಗಚ್ಛ fail;
    }

    result = http2_data_setup(cf, newhandle, &newstream);
    ಯದಿ(result) {
      failf(data, "error setting up stream: %d", (ಪೂರ್ಣಾಙ್ಕ)result);
      discard_newhandle(cf, newhandle);
      rv = CURL_ತಳ್ಳು_DENY;
      ಗಚ್ಛ fail;
    }

    DEBUGASSERT(newstream);
    newstream->id = frame->promised_stream_id;
    newhandle->req.maxdownload = -1;
    newhandle->req.size = -1;

    CURL_TRC_CF(data, cf, "promise easy handle added to multi, mid=%u",
                newhandle->mid);
    rv = Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, newstream->id, newstream);
    ಯದಿ(rv) {
      infof(data, "failed to set user_data for stream %d",
            newstream->id);
      DEBUGASSERT(0);
      discard_newhandle(cf, newhandle);
      rv = CURL_ತಳ್ಳು_DENY;
      ಗಚ್ಛ fail;
    }

    /* success, remember max stream id processed */
    ಯದಿ(newstream->id > ctx->local_max_sid)
      ctx->local_max_sid = newstream->id;
  }
  ಅನ್ಯಥಾ {
    CURL_TRC_CF(data, cf, "Got PUSH_PROMISE, ignore it");
    rv = CURL_ತಳ್ಳು_DENY;
  }
fail:
  ಪ್ರತ್ಯಾಗಮ rv;
}

ಸ್ಥಿರ ಶೂನ್ಯ h2_xfer_write_resp_hd(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ h2_stream_ctx *stream,
                                  ಧ್ರುವ ವರ್ಣ *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  ಯದಿ(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, blen, eos);
    ಯದಿ(!stream->xfer_result && !eos)
      stream->xfer_result = cf_h2_update_local_win(cf, data, stream);
    ಯದಿ(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%d] error %d writing %zu bytes of headers",
                  stream->id, (ಪೂರ್ಣಾಙ್ಕ)stream->xfer_result, blen);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ h2_xfer_write_resp(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ h2_stream_ctx *stream,
                               ಧ್ರುವ ವರ್ಣ *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  ಯದಿ(!stream->xfer_result)
    stream->xfer_result = Curl_xfer_write_resp(data, buf, blen, eos);
  /* If the transfer write is errored, we do not want any more data */
  ಯದಿ(stream->xfer_result) {
    ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
    CURL_TRC_CF(data, cf, "[%d] error %d writing %zu bytes of data, "
                "RST-ing stream",
                stream->id, (ಪೂರ್ಣಾಙ್ಕ)stream->xfer_result, blen);
    Curl_ngh2_submit_rst_stream(
      &ctx->ngh2, 0, stream->id, (uint32_t)NGHTTP2_ERR_CALLBACK_FAILURE);
  }
  ಅನ್ಯಥಾ ಯದಿ(!stream->write_paused && Curl_xfer_write_is_paused(data)) {
    CURL_TRC_CF(data, cf, "[%d] stream output paused", stream->id);
    stream->write_paused = TRUE;
  }
  ಅನ್ಯಥಾ ಯದಿ(stream->write_paused && !Curl_xfer_write_is_paused(data)) {
    CURL_TRC_CF(data, cf, "[%d] stream output unpaused", stream->id);
    stream->write_paused = FALSE;
  }

  ಯದಿ(!stream->xfer_result && !eos)
    stream->xfer_result = cf_h2_update_local_win(cf, data, stream);
}

ಸ್ಥಿರ CURLcode on_stream_frame(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ h2_stream_ctx *stream,
                                ಧ್ರುವ nghttp2_frame *frame)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  int32_t stream_id = frame->hd.stream_id;
  ಪೂರ್ಣಾಙ್ಕ rv;

  ಯದಿ(!stream) {
    CURL_TRC_CF(data, cf, "[%d] No stream_ctx set", stream_id);
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }

  ವಿಕಲ್ಪ(frame->hd.type) {
  ಪಕ್ಷ NGHTTP2_DATA:
    CURL_TRC_CF(data, cf, "[%d] DATA, window=%d/%d",
                stream_id,
                Curl_ngh2_session_get_stream_effective_recv_data_length(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_stream_effective_local_window_size(
                  &ctx->ngh2, stream->id));
    /* If !body started on this stream, then receiving DATA is illegal. */
    ಯದಿ(!stream->bodystarted) {
      ಯದಿ(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                       stream_id, NGHTTP2_PROTOCOL_ERROR))) {
        ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
      }
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ NGHTTP2_HEADERS:
    ಯದಿ(stream->bodystarted) {
      /* Only valid HEADERS after body started is trailer HEADERS. We
         buffer them in on_header callback. */
      ಭಙ್ಗ;
    }

    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    ಯದಿ(stream->status_code == -1)
      ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;

    /* Only final status code signals the end of header */
    ಯದಿ(stream->status_code / 100 != 1) {
      stream->resp_hds_complete = TRUE;
      stream->bodystarted = TRUE;
    }
    ಅನ್ಯಥಾ
      stream->status_code = -1;

    h2_xfer_write_resp_hd(cf, data, stream, STRCONST("\r\n"),
                          (bool)stream->closed);

    Curl_multi_mark_dirty(data);
    ಭಙ್ಗ;
  ಪಕ್ಷ NGHTTP2_ತಳ್ಳು_PROMISE:
    rv = ತಳ್ಳು_promise(cf, data, &frame->ತಳ್ಳು_promise);
    ಯದಿ(rv) { /* deny! */
      DEBUGASSERT((rv > CURL_ತಳ್ಳು_OK) && (rv <= CURL_ತಳ್ಳು_ERROROUT));
      ಯದಿ(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
             frame->ತಳ್ಳು_promise.promised_stream_id, NGHTTP2_CANCEL)))
        ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
      ಅನ್ಯಥಾ ಯದಿ(rv == CURL_ತಳ್ಳು_ERROROUT) {
        /* fail the parent transfer, not the whole session */
        CURL_TRC_CF(data, cf, "[%d] fail in PUSH_PROMISE received",
                    stream_id);
        ಯದಿ(!stream->xfer_result)
          stream->xfer_result = CURLE_RECV_ERROR;
        Curl_multi_mark_dirty(data);
      }
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ NGHTTP2_RST_STREAM:
    ಯದಿ(frame->rst_stream.error_code)
      stream->reset_by_server = TRUE;
    Curl_multi_mark_dirty(data);
    ಭಙ್ಗ;
  ಪಕ್ಷ NGHTTP2_WINDOW_UPDATE:
    ಯದಿ(CURL_REQ_WANT_SEND(data) && Curl_bufq_is_empty(&stream->sendbuf)) {
      /* need more data, force processing of transfer */
      Curl_multi_mark_dirty(data);
    }
    ಅನ್ಯಥಾ ಯದಿ(!Curl_bufq_is_empty(&stream->sendbuf)) {
      /* resume the potentially suspended stream */
      ಯದಿ(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id)))
        ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
    }
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }

  ಯದಿ(frame->hd.flags & NGHTTP2_FLAG_END_STREAM) {
    ಯದಿ(!stream->closed && !stream->body_eos &&
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
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#ifdef CURLVERBOSE
ಪೂರ್ಣಾಙ್ಕ Curl_nghttp2_fr_print(ಧ್ರುವ nghttp2_frame *frame, ವರ್ಣ *buffer,
                          size_t blen)
{
  ವಿಕಲ್ಪ(frame->hd.type) {
  ಪಕ್ಷ NGHTTP2_DATA: {
    ಪ್ರತ್ಯಾಗಮ curl_msnprintf(buffer, blen,
                          "FRAME[DATA, len=%d, eos=%d, padlen=%d]",
                          (ಪೂರ್ಣಾಙ್ಕ)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_STREAM),
                          (ಪೂರ್ಣಾಙ್ಕ)frame->data.padlen);
  }
  ಪಕ್ಷ NGHTTP2_HEADERS: {
    ಪ್ರತ್ಯಾಗಮ curl_msnprintf(buffer, blen,
                          "FRAME[HEADERS, len=%d, hend=%d, eos=%d]",
                          (ಪೂರ್ಣಾಙ್ಕ)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_HEADERS),
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_STREAM));
  }
  ಪಕ್ಷ NGHTTP2_PRIORITY: {
    ಪ್ರತ್ಯಾಗಮ curl_msnprintf(buffer, blen,
                          "FRAME[PRIORITY, len=%d, flags=%d]",
                          (ಪೂರ್ಣಾಙ್ಕ)frame->hd.length, frame->hd.flags);
  }
  ಪಕ್ಷ NGHTTP2_RST_STREAM: {
    ಪ್ರತ್ಯಾಗಮ curl_msnprintf(buffer, blen,
                          "FRAME[RST_STREAM, len=%d, flags=%d, error=%u]",
                          (ಪೂರ್ಣಾಙ್ಕ)frame->hd.length, frame->hd.flags,
                          frame->rst_stream.error_code);
  }
  ಪಕ್ಷ NGHTTP2_SETTINGS: {
    ಯದಿ(frame->hd.flags & NGHTTP2_FLAG_ACK) {
      ಪ್ರತ್ಯಾಗಮ curl_msnprintf(buffer, blen, "FRAME[SETTINGS, ack=1]");
    }
    ಪ್ರತ್ಯಾಗಮ curl_msnprintf(buffer, blen,
                          "FRAME[SETTINGS, len=%d]", (ಪೂರ್ಣಾಙ್ಕ)frame->hd.length);
  }
  ಪಕ್ಷ NGHTTP2_ತಳ್ಳು_PROMISE:
    ಪ್ರತ್ಯಾಗಮ curl_msnprintf(buffer, blen,
                          "FRAME[PUSH_PROMISE, len=%d, hend=%d]",
                          (ಪೂರ್ಣಾಙ್ಕ)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_HEADERS));
  ಪಕ್ಷ NGHTTP2_PING:
    ಪ್ರತ್ಯಾಗಮ curl_msnprintf(buffer, blen,
                          "FRAME[PING, len=%d, ack=%d]",
                          (ಪೂರ್ಣಾಙ್ಕ)frame->hd.length,
                          frame->hd.flags & NGHTTP2_FLAG_ACK);
  ಪಕ್ಷ NGHTTP2_GOAWAY: {
    ವರ್ಣ scratch[128];
    size_t s_len = CURL_ARRAYSIZE(scratch);
    size_t len = (frame->goaway.opaque_data_len < s_len) ?
      frame->goaway.opaque_data_len : s_len - 1;
    ಯದಿ(len)
      memcpy(scratch, frame->goaway.opaque_data, len);
    scratch[len] = '\0';
    ಪ್ರತ್ಯಾಗಮ curl_msnprintf(buffer, blen,
                          "FRAME[GOAWAY, error=%u, reason='%s', "
                          "last_stream=%d]", frame->goaway.error_code,
                          scratch, frame->goaway.last_stream_id);
  }
  ಪಕ್ಷ NGHTTP2_WINDOW_UPDATE: {
    ಪ್ರತ್ಯಾಗಮ curl_msnprintf(buffer, blen,
                          "FRAME[WINDOW_UPDATE, incr=%d]",
                          frame->window_update.window_size_increment);
  }
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ curl_msnprintf(buffer, blen, "FRAME[%d, len=%d, flags=%d]",
                          frame->hd.type, (ಪೂರ್ಣಾಙ್ಕ)frame->hd.length,
                          frame->hd.flags);
  }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ on_frame_send(nghttp2_session *session, ಧ್ರುವ nghttp2_frame *frame,
                         ಶೂನ್ಯ *userp)
{
  ಸಂರಚನಾ Curl_cfilter *cf = userp;
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);

  (ಶೂನ್ಯ)session;
  DEBUGASSERT(data);
  ಯದಿ(Curl_trc_cf_is_verbose(cf, data)) {
    ವರ್ಣ buffer[256];
    ಪೂರ್ಣಾಙ್ಕ len;
    len = Curl_nghttp2_fr_print(frame, buffer, ಪರಿಮಾಣ(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] -> %s", frame->hd.stream_id, buffer);
  }
  ಯದಿ((frame->hd.type == NGHTTP2_GOAWAY) && !ctx->sent_goaway) {
    /* A GOAWAY not initiated by us, but by nghttp2 itself on detecting
     * a protocol error on the connection */
    failf(data, "nghttp2 shuts down connection with error %u: %s",
          frame->goaway.error_code,
          nghttp2_http2_strerror(frame->goaway.error_code));
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}
#endif /* CURLVERBOSE */

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ on_frame_recv(nghttp2_session *session, ಧ್ರುವ nghttp2_frame *frame,
                         ಶೂನ್ಯ *userp)
{
  ಸಂರಚನಾ Curl_cfilter *cf = userp;
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf), *data_s;
  ಸಂರಚನಾ h2_stream_ctx *stream;
  int32_t stream_id = frame->hd.stream_id;

  (ಶೂನ್ಯ)session;
  DEBUGASSERT(data);
#ifdef CURLVERBOSE
  ಯದಿ(Curl_trc_cf_is_verbose(cf, data)) {
    ವರ್ಣ buffer[256];
    ಪೂರ್ಣಾಙ್ಕ len;
    len = Curl_nghttp2_fr_print(frame, buffer, ಪರಿಮಾಣ(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] <- %s", frame->hd.stream_id, buffer);
  }
#endif /* CURLVERBOSE */

  ಯದಿ(!stream_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(data);
    ವಿಕಲ್ಪ(frame->hd.type) {
    ಪಕ್ಷ NGHTTP2_SETTINGS: {
      ಯದಿ(!(frame->hd.flags & NGHTTP2_FLAG_ACK)) {
        uint32_t max_conn = ctx->max_concurrent_streams;
        ctx->max_concurrent_streams =
          Curl_ngh2_session_get_remote_settings(
            &ctx->ngh2, NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS);
        ctx->enable_ತಳ್ಳು = Curl_ngh2_session_get_remote_settings(
          &ctx->ngh2, NGHTTP2_SETTINGS_ENABLE_ತಳ್ಳು) != 0;
        CURL_TRC_CF(data, cf, "[0] MAX_CONCURRENT_STREAMS: %u",
                    ctx->max_concurrent_streams);
        CURL_TRC_CF(data, cf, "[0] ENABLE_PUSH: %s",
                    ctx->enable_ತಳ್ಳು ? "TRUE" : "false");
        ಯದಿ(data && max_conn != ctx->max_concurrent_streams) {
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
        ಯದಿ(CURL_REQ_WANT_SEND(data))
          Curl_multi_mark_dirty(data);
      }
      ಭಙ್ಗ;
    }
    ಪಕ್ಷ NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->goaway_error = frame->goaway.error_code;
      ctx->remote_max_sid = frame->goaway.last_stream_id;
      ಯದಿ(data) {
        infof(data, "received GOAWAY, error=%u, last_stream=%d",
              ctx->goaway_error, ctx->remote_max_sid);
        Curl_multi_connchanged(data->multi);
      }
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  ಯದಿ(!data_s) {
    CURL_TRC_CF(data, cf, "[%d] No Curl_easy associated", stream_id);
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಪ್ರತ್ಯಾಗಮ on_stream_frame(cf, data_s, stream, frame) ?
         NGHTTP2_ERR_CALLBACK_FAILURE : 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cf_h2_on_invalid_frame_recv(nghttp2_session *session,
                                       ಧ್ರುವ nghttp2_frame *frame,
                                       ಪೂರ್ಣಾಙ್ಕ ngerr, ಶೂನ್ಯ *userp)
{
  ಸಂರಚನಾ Curl_cfilter *cf = userp;
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ Curl_easy *data;
  ಸಂರಚನಾ h2_stream_ctx *stream;
  int32_t stream_id = frame->hd.stream_id;

  (ಶೂನ್ಯ)session;
  data = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  ಯದಿ(data) {
#ifdef CURLVERBOSE
    ವರ್ಣ buffer[256];
    ಪೂರ್ಣಾಙ್ಕ len;
    len = Curl_nghttp2_fr_print(frame, buffer, ಪರಿಮಾಣ(buffer) - 1);
    buffer[len] = 0;
    failf(data, "[HTTP2] [%d] received invalid frame: %s, error %d: %s",
          stream_id, buffer, ngerr, nghttp2_strerror(ngerr));
#endif /* CURLVERBOSE */
    ಯದಿ(stream) {
      Curl_ngh2_submit_rst_stream(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, NGHTTP2_STREAM_CLOSED);
      stream->error = ngerr;
      stream->closed = TRUE;
      stream->reset = TRUE;
    }
  }
  /* Return no error, nghttp2 will RST/GOAWAY by itself when needed */
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ on_data_chunk_recv(nghttp2_session *session, uint8_t flags,
                              int32_t stream_id,
                              ಧ್ರುವ uint8_t *mem, size_t len, ಶೂನ್ಯ *userp)
{
  ಸಂರಚನಾ Curl_cfilter *cf = userp;
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ h2_stream_ctx *stream;
  ಸಂರಚನಾ Curl_easy *data_s;

  (ಶೂನ್ಯ)session;
  (ಶೂನ್ಯ)flags;

  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */
  DEBUGASSERT(CF_DATA_CURRENT(cf));

  /* get the stream from the hash based on Stream ID */
  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  ಯದಿ(!data_s) {
    /* Receiving a Stream ID not in the hash should not happen - unless
       we have aborted a transfer artificially and there were more data
       in the pipeline. Silently ignore. */
    CURL_TRC_CF(CF_DATA_CURRENT(cf), cf, "[%d] Data for unknown", stream_id);
    /* consumed explicitly as no one will read it */
    Curl_ngh2_session_consume(&ctx->ngh2, stream_id, len);
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಯದಿ(!stream)
    ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;

  h2_xfer_write_resp(cf, data_s, stream, (ಧ್ರುವ ವರ್ಣ *)mem, len, FALSE);

  Curl_ngh2_session_consume(&ctx->ngh2, stream_id, len);
  stream->nrcvd_data += (curl_off_t)len;
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ on_stream_close(nghttp2_session *session, int32_t stream_id,
                           uint32_t error_code, ಶೂನ್ಯ *userp)
{
  ಸಂರಚನಾ Curl_cfilter *cf = userp;
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ Curl_easy *data_s, *call_data = CF_DATA_CURRENT(cf);
  ಸಂರಚನಾ h2_stream_ctx *stream = ರಿಕ್ತ;
  ಪೂರ್ಣಾಙ್ಕ rv;
  (ಶೂನ್ಯ)session;

  DEBUGASSERT(call_data);
  /* stream id 0 is the connection, do not look there for streams. */
  data_s = stream_id ? cf_h2_get_stream_xfer(cf, stream_id, &stream) : ರಿಕ್ತ;
  ಯದಿ(!data_s) {
    CURL_TRC_CF(call_data, cf,
                "[%d] on_stream_close, no easy set on stream", stream_id);
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಯದಿ(!GOOD_EASY_HANDLE(data_s)) {
    /* nghttp2 still has an easy registered for the stream which has
     * been freed be libcurl. This points to a code path that does not
     * trigger DONE or DETACH events as it must. */
    CURL_TRC_CF(call_data, cf,
                "[%d] on_stream_close, not a GOOD easy on stream", stream_id);
    (ಶೂನ್ಯ)Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, stream_id, ರಿಕ್ತ);
    ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  ಯದಿ(!stream) {
    CURL_TRC_CF(data_s, cf,
                "[%d] on_stream_close, GOOD easy but no stream", stream_id);
    ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  stream->closed = TRUE;
  stream->error = error_code;
  ಯದಿ(stream->error) {
    stream->reset = TRUE;
    ಯದಿ((stream->error == NGHTTP2_REFUSED_STREAM) &&
       ctx->rcvd_goaway && ctx->goaway_error)
    /* REFUSED_STREAM is used by nghttp2 on a GOAWAY from the
     * server where it indicated it will no longer process this stream.
     * If the GOAWAY carried an error code, take that as the error for
     * the stream. */
    stream->error = ctx->goaway_error;
  }

  ಯದಿ(stream->error)
    CURL_TRC_CF(data_s, cf, "[%d] RESET: %s (err %u)",
              stream_id, nghttp2_http2_strerror(error_code), error_code);
  ಅನ್ಯಥಾ
    CURL_TRC_CF(data_s, cf, "[%d] CLOSED", stream_id);
  Curl_multi_mark_dirty(data_s);

  /* remove `stream_ctx` from the nghttp2 stream */
  rv = Curl_ngh2_session_set_stream_user_data(
    &ctx->ngh2, stream_id, ರಿಕ್ತ);
  ಯದಿ(rv) {
    infof(data_s, "http/2: failed to clear user_data for stream %d",
          stream_id);
    DEBUGASSERT(0);
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_h2_header_error(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ h2_stream_ctx *stream,
                               CURLcode result)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;

  failf(data, "Error receiving HTTP2 header: %d(%s)", (ಪೂರ್ಣಾಙ್ಕ)result,
        curl_easy_strerror(result));
  ಯದಿ(stream) {
    Curl_ngh2_submit_rst_stream(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, NGHTTP2_STREAM_CLOSED);
    stream->closed = TRUE;
    stream->reset = TRUE;
  }
}

/* frame->hd.type is either NGHTTP2_HEADERS or NGHTTP2_PUSH_PROMISE */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ on_header(nghttp2_session *session, ಧ್ರುವ nghttp2_frame *frame,
                     ಧ್ರುವ uint8_t *name, size_t namelen,
                     ಧ್ರುವ uint8_t *ಮೌಲ್ಯ, size_t valuelen,
                     uint8_t flags,
                     ಶೂನ್ಯ *userp)
{
  ಸಂರಚನಾ Curl_cfilter *cf = userp;
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ h2_stream_ctx *stream;
  ಸಂರಚನಾ Curl_easy *data;
  int32_t stream_id = frame->hd.stream_id;
  CURLcode result;

  (ಶೂನ್ಯ)session;
  (ಶೂನ್ಯ)flags;

  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */

  /* get the stream from the hash based on Stream ID */
  data = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  ಯದಿ(!GOOD_EASY_HANDLE(data))
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;

  ಯದಿ(!stream) {
    failf(data, "Internal NULL stream");
    ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  /* Store received PUSH_PROMISE headers to be used when the subsequent
     PUSH_PROMISE callback comes */
  ಯದಿ(frame->hd.type == NGHTTP2_ತಳ್ಳು_PROMISE) {
    ವರ್ಣ *h;

    ಯದಿ((namelen == CURL_CSTRLEN(HTTP_PSEUDO_AUTHORITY)) &&
       !strncmp(HTTP_PSEUDO_AUTHORITY, (ಧ್ರುವ ವರ್ಣ *)name, namelen)) {
      /* pseudo headers are lower case */
      ಪೂರ್ಣಾಙ್ಕ rc = 0;
      ವರ್ಣ *check = curl_maprintf("%s:%d", data->state.origin->hostname,
                                  data->state.origin->port);
      ಯದಿ(!check)
        /* no memory */
        ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;
      ಯದಿ(!curl_strequal(check, (ಧ್ರುವ ವರ್ಣ *)ಮೌಲ್ಯ) &&
         ((data->state.origin->port != data->state.origin->scheme->defport) ||
          !curl_strequal(data->state.origin->hostname, (ಧ್ರುವ ವರ್ಣ *)ಮೌಲ್ಯ))) {
        /* This is push is not for the same authority that was asked for in
         * the URL. RFC 7540 section 8.2 says: "A client MUST treat a
         * PUSH_PROMISE for which the server is not authoritative as a stream
         * error of type PROTOCOL_ERROR."
         */
        (ಶೂನ್ಯ)Curl_ngh2_submit_rst_stream(
          &ctx->ngh2, NGHTTP2_FLAG_NONE, stream_id, NGHTTP2_PROTOCOL_ERROR);
        rc = NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      curlx_free(check);
      ಯದಿ(rc)
        ಪ್ರತ್ಯಾಗಮ rc;
    }

    ಯದಿ(!stream->ತಳ್ಳು_headers) {
      stream->ತಳ್ಳು_headers_alloc = 10;
      stream->ತಳ್ಳು_headers = curlx_malloc(stream->ತಳ್ಳು_headers_alloc *
                                          ಪರಿಮಾಣ(ವರ್ಣ *));
      ಯದಿ(!stream->ತಳ್ಳು_headers)
        ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;
      stream->ತಳ್ಳು_headers_used = 0;
    }
    ಅನ್ಯಥಾ ಯದಿ(stream->ತಳ್ಳು_headers_used ==
            stream->ತಳ್ಳು_headers_alloc) {
      ವರ್ಣ **headp;
      ಯದಿ(stream->ತಳ್ಳು_headers_alloc > 1000) {
        /* this is beyond crazy many headers, bail out */
        failf(data, "Too many PUSH_PROMISE headers");
        free_ತಳ್ಳು_headers(stream);
        ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      stream->ತಳ್ಳು_headers_alloc *= 2;
      headp = curlx_realloc(stream->ತಳ್ಳು_headers,
                            stream->ತಳ್ಳು_headers_alloc * ಪರಿಮಾಣ(ವರ್ಣ *));
      ಯದಿ(!headp) {
        free_ತಳ್ಳು_headers(stream);
        ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      stream->ತಳ್ಳು_headers = headp;
    }
    h = curl_maprintf("%s:%s", name, ಮೌಲ್ಯ);
    ಯದಿ(!h) {
      free_ತಳ್ಳು_headers(stream);
      ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    stream->ತಳ್ಳು_headers[stream->ತಳ್ಳು_headers_used++] = h;
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಯದಿ(stream->bodystarted) {
    /* This is a trailer */
    CURL_TRC_CF(data, cf, "[%d] trailer: %.*s: %.*s",
                stream->id, (ಪೂರ್ಣಾಙ್ಕ)namelen, name, (ಪೂರ್ಣಾಙ್ಕ)valuelen, ಮೌಲ್ಯ);
    result = Curl_dynhds_add(&stream->resp_trailers,
                             (ಧ್ರುವ ವರ್ಣ *)name, namelen,
                             (ಧ್ರುವ ವರ್ಣ *)ಮೌಲ್ಯ, valuelen);
    ಯದಿ(result) {
      cf_h2_header_error(cf, data, stream, result);
      ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;
    }

    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಯದಿ(namelen == CURL_CSTRLEN(HTTP_PSEUDO_STATUS) &&
     !memcmp(HTTP_PSEUDO_STATUS, name, namelen)) {
    /* nghttp2 guarantees :status is received first and only once. */
    ವರ್ಣ buffer[32];
    size_t hlen;
    result = Curl_http_decode_status(&stream->status_code,
                                     (ಧ್ರುವ ವರ್ಣ *)ಮೌಲ್ಯ, valuelen);
    ಯದಿ(result) {
      cf_h2_header_error(cf, data, stream, result);
      ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    hlen = curl_msnprintf(buffer, ಪರಿಮಾಣ(buffer), HTTP_PSEUDO_STATUS ":%d\r",
                          stream->status_code);
    result = Curl_headers_ತಳ್ಳು(data, buffer, hlen, CURLH_PSEUDO);
    ಯದಿ(result) {
      cf_h2_header_error(cf, data, stream, result);
      ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    curlx_dyn_reset(&ctx->scratch);
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/2 "));
    ಯದಿ(!result)
      result = curlx_dyn_addn(&ctx->scratch, ಮೌಲ್ಯ, valuelen);
    ಯದಿ(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    ಯದಿ(!result)
      h2_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    ಯದಿ(result) {
      cf_h2_header_error(cf, data, stream, result);
      ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    /* if we receive data for another handle, wake that up */
    ಯದಿ(CF_DATA_CURRENT(cf) != data)
      Curl_multi_mark_dirty(data);

    CURL_TRC_CF(data, cf, "[%d] status: HTTP/2 %03d",
                stream->id, stream->status_code);
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  /* nghttp2 guarantees that namelen > 0, and :status was already
     received, and this is not pseudo-header field . */
  /* convert to an HTTP1-style header */
  curlx_dyn_reset(&ctx->scratch);
  result = curlx_dyn_addn(&ctx->scratch, (ಧ್ರುವ ವರ್ಣ *)name, namelen);
  ಯದಿ(!result)
    result = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
  ಯದಿ(!result)
    result = curlx_dyn_addn(&ctx->scratch, (ಧ್ರುವ ವರ್ಣ *)ಮೌಲ್ಯ, valuelen);
  ಯದಿ(!result)
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
  ಯದಿ(!result)
    h2_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                          curlx_dyn_len(&ctx->scratch), FALSE);
  ಯದಿ(result) {
    cf_h2_header_error(cf, data, stream, result);
    ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  /* if we receive data for another handle, wake that up */
  ಯದಿ(CF_DATA_CURRENT(cf) != data)
    Curl_multi_mark_dirty(data);

  CURL_TRC_CF(data, cf, "[%d] header: %.*s: %.*s",
              stream->id, (ಪೂರ್ಣಾಙ್ಕ)namelen, name, (ಪೂರ್ಣಾಙ್ಕ)valuelen, ಮೌಲ್ಯ);

  ಪ್ರತ್ಯಾಗಮ 0; /* 0 is successful */
}

ಸ್ಥಿರ ssize_t req_body_read_callback(nghttp2_session *session,
                                      int32_t stream_id,
                                      uint8_t *buf, size_t length,
                                      uint32_t *data_flags,
                                      nghttp2_data_source *source,
                                      ಶೂನ್ಯ *userp)
{
  ಸಂರಚನಾ Curl_cfilter *cf = userp;
  ಸಂರಚನಾ Curl_easy *data_s;
  ಸಂರಚನಾ h2_stream_ctx *stream = ರಿಕ್ತ;
  CURLcode result;
  ssize_t nread;
  size_t n;
  (ಶೂನ್ಯ)source;

  (ಶೂನ್ಯ)session;
  ಯದಿ(!stream_id)
    ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_INVALID_ARGUMENT;

  /* get the stream from the hash based on Stream ID, stream ID zero is for
     connection-oriented stuff */
  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  ಯದಿ(!data_s || !stream)
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_bufq_read(&stream->sendbuf, buf, length, &n);
  ಯದಿ(result) {
    ಯದಿ(result != CURLE_AGAIN)
      ಪ್ರತ್ಯಾಗಮ NGHTTP2_ERR_CALLBACK_FAILURE;
    nread = 0;
  }
  ಅನ್ಯಥಾ
    nread = (ssize_t)n;

  CURL_TRC_CF(data_s, cf, "[%d] req_body_read(len=%zu) eos=%d -> %zd, %d",
              stream_id, length, stream->body_eos, nread, (ಪೂರ್ಣಾಙ್ಕ)result);

  ಯದಿ(stream->body_eos && Curl_bufq_is_empty(&stream->sendbuf)) {
    *data_flags = NGHTTP2_DATA_FLAG_EOF;
    ಪ್ರತ್ಯಾಗಮ nread;
  }
  ಪ್ರತ್ಯಾಗಮ (nread == 0) ? NGHTTP2_ERR_DEFERRED : nread;
}

#ifdef CURLVERBOSE
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ error_callback(nghttp2_session *session,
                          ಧ್ರುವ ವರ್ಣ *ಸಂದೇಶ,
                          size_t len,
                          ಶೂನ್ಯ *userp)
{
  ಸಂರಚನಾ Curl_cfilter *cf = userp;
  ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);
  (ಶೂನ್ಯ)session;
  failf(data, "%.*s", (ಪೂರ್ಣಾಙ್ಕ)len, ಸಂದೇಶ);
  ಪ್ರತ್ಯಾಗಮ 0;
}
#endif

/*
 * Append headers to ask for an HTTP1.1 to HTTP2 upgrade.
 */
CURLcode Curl_http2_request_upgrade(ಸಂರಚನಾ dynbuf *req,
                                    ಸಂರಚನಾ Curl_easy *data)
{
  CURLcode result;
  ವರ್ಣ *base64;
  size_t blen;
  ಸಂರಚನಾ SingleRequest *k = &data->req;
  uint8_t binsettings[H2_BINSETTINGS_LEN];
  ssize_t rc;
  size_t binlen; /* length of the binsettings data */

  rc = populate_binsettings(binsettings, data);
  ಯದಿ(!curlx_sztouz(rc, &binlen) || !binlen) {
    failf(data, "nghttp2 unexpectedly failed on pack_settings_payload");
    curlx_dyn_free(req);
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }

  result = curlx_base64url_encode(binsettings, binlen, &base64, &blen);
  ಯದಿ(result) {
    curlx_dyn_free(req);
    ಪ್ರತ್ಯಾಗಮ result;
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

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode http2_handle_stream_close(ಸಂರಚನಾ Curl_cfilter *cf,
                                          ಸಂರಚನಾ Curl_easy *data,
                                          ಸಂರಚನಾ h2_stream_ctx *stream,
                                          size_t *pnlen)
{
  CURLcode result;

  *pnlen = 0;
  ಯದಿ(stream->reset) {
    ಯದಿ(stream->error == NGHTTP2_REFUSED_STREAM) {
      infof(data, "HTTP/2 stream %d refused by server, try again on a new "
                  "connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    ಅನ್ಯಥಾ ಯದಿ(stream->resp_hds_complete && data->req.no_body) {
      CURL_TRC_CF(data, cf, "[%d] error after response headers, but we did "
                  "not want a body anyway, ignore: %s (err %u)",
                  stream->id, nghttp2_http2_strerror(stream->error),
                  stream->error);
      stream->close_handled = TRUE;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    failf(data, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          stream->id, stream->reset_by_server ? "server" : "curl",
          stream->error, nghttp2_http2_strerror(stream->error));
    ಪ್ರತ್ಯಾಗಮ stream->error ? CURLE_HTTP2_STREAM :
           (data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2);
  }
  ಅನ್ಯಥಾ ಯದಿ(!stream->bodystarted) {
    failf(data, "HTTP/2 stream %d was closed cleanly, but before getting "
          "all response header fields, treated as error", stream->id);
    ಪ್ರತ್ಯಾಗಮ CURLE_HTTP2_STREAM;
  }

  ಯದಿ(Curl_dynhds_ಎಣಿಕೆ(&stream->resp_trailers)) {
    ಸಂರಚನಾ dynhds_entry *e;
    ಸಂರಚನಾ dynbuf dbuf;
    size_t i;

    result = CURLE_OK;
    curlx_dyn_init(&dbuf, DYN_TRAILERS);
    ಪ್ರತಿ(i = 0; i < Curl_dynhds_ಎಣಿಕೆ(&stream->resp_trailers); ++i) {
      e = Curl_dynhds_getn(&stream->resp_trailers, i);
      ಯದಿ(!e)
        ಭಙ್ಗ;
      curlx_dyn_reset(&dbuf);
      result = curlx_dyn_addf(&dbuf, "%.*s: %.*s\x0d\x0a",
                              (ಪೂರ್ಣಾಙ್ಕ)e->namelen, e->name,
                              (ಪೂರ್ಣಾಙ್ಕ)e->valuelen, e->ಮೌಲ್ಯ);
      ಯದಿ(result)
        ಭಙ್ಗ;
      Curl_debug(data, CURLINFO_HEADER_IN, curlx_dyn_ptr(&dbuf),
                 curlx_dyn_len(&dbuf));
      result = Curl_client_write(data,
                                 CLIENTWRITE_HEADER | CLIENTWRITE_TRAILER,
                                 curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf));
      ಯದಿ(result)
        ಭಙ್ಗ;
      result = Curl_bump_headersize(data, curlx_dyn_len(&dbuf), FALSE);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
    curlx_dyn_free(&dbuf);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  stream->close_handled = TRUE;
  result = CURLE_OK;

out:
  CURL_TRC_CF(data, cf, "handle_stream_close -> %d, %zu", (ಪೂರ್ಣಾಙ್ಕ)result, *pnlen);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sweight_wanted(ಧ್ರುವ ಸಂರಚನಾ Curl_easy *data)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  ಪ್ರತ್ಯಾಗಮ data->set.weight ?
    data->set.weight : NGHTTP2_DEFAULT_WEIGHT;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sweight_in_effect(ಧ್ರುವ ಸಂರಚನಾ Curl_easy *data)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  ಪ್ರತ್ಯಾಗಮ data->state.weight ?
    data->state.weight : NGHTTP2_DEFAULT_WEIGHT;
}

/*
 * h2_pri_spec() fills in the pri_spec struct, used by nghttp2 to send weight
 * and dependency to the peer. It also stores the updated values in the state
 * struct.
 */

ಸ್ಥಿರ ಶೂನ್ಯ h2_pri_spec(ಸಂರಚನಾ Curl_easy *data,
                        nghttp2_priority_spec *pri_spec)
{
  ಪೂರ್ಣಾಙ್ಕ prio = data->set.weight;
  nghttp2_priority_spec_init(pri_spec, 0, sweight_wanted(data), FALSE);
  data->state.weight = prio;
}

/*
 * Check if there is been an update in the priority /
 * dependency settings and if so it submits a PRIORITY frame with the updated
 * info.
 * Flush any out data pending in the network buffer.
 */
ಸ್ಥಿರ CURLcode h2_progress_egress(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  ಪೂರ್ಣಾಙ್ಕ rv = 0;

  ಯದಿ(sweight_wanted(data) != sweight_in_effect(data)) {
    ಸಂರಚನಾ h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    ಯದಿ(stream && stream->id > 0 &&
       (sweight_wanted(data) != sweight_in_effect(data))) {
      /* send new weight and/or dependency */
      nghttp2_priority_spec pri_spec;

      h2_pri_spec(data, &pri_spec);
      CURL_TRC_CF(data, cf, "[%d] Queuing PRIORITY", stream->id);
      DEBUGASSERT(stream->id != -1);
      rv = Curl_ngh2_submit_priority(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, &pri_spec);
      ಯದಿ(rv)
        ಗಚ್ಛ out;
    }
  }

  ctx->nw_out_blocked = 0;
  ಯಾವತ್(!rv && !ctx->nw_out_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

out:
  ಯದಿ(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(data, cf, "nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
  }
  /* Defer flushing during the connect phase so that the SETTINGS and
   * other initial frames are sent together with the first request.
   * Unless we are 'connect_only' where the request will never come. */
  ಯದಿ(!cf->connected && !cf->conn->bits.connect_only)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪ್ರತ್ಯಾಗಮ nw_out_flush(cf, data);
}

ಸ್ಥಿರ CURLcode stream_recv(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ h2_stream_ctx *stream,
                            ವರ್ಣ *buf, size_t len, size_t *pnread)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_AGAIN;

  (ಶೂನ್ಯ)buf;
  (ಶೂನ್ಯ)len;
  *pnread = 0;

  ಯದಿ(!stream->xfer_result)
    stream->xfer_result = cf_h2_update_local_win(cf, data, stream);

  ಯದಿ(stream->xfer_result) {
    CURL_TRC_CF(data, cf, "[%d] xfer write failed", stream->id);
    result = stream->xfer_result;
  }
  ಅನ್ಯಥಾ ಯದಿ(stream->closed) {
    CURL_TRC_CF(data, cf, "[%d] returning CLOSE", stream->id);
    result = http2_handle_stream_close(cf, data, stream, pnread);
  }
  ಅನ್ಯಥಾ ಯದಿ(stream->reset ||
          (ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) ||
          (ctx->rcvd_goaway && ctx->remote_max_sid < stream->id)) {
    CURL_TRC_CF(data, cf, "[%d] returning ERR", stream->id);
    result = data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2;
  }

  ಯದಿ(result && (result != CURLE_AGAIN))
    CURL_TRC_CF(data, cf, "[%d] stream_recv(len=%zu) -> %d, %zu",
                stream->id, len, (ಪೂರ್ಣಾಙ್ಕ)result, *pnread);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode h2_progress_ingress(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    size_t data_max_bytes)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  ಯದಿ(should_close_session(ctx)) {
    CURL_TRC_CF(data, cf, "[0] ingress: session is closed");
    ಪ್ರತ್ಯಾಗಮ CURLE_HTTP2;
  }

  /* Process network input buffer first */
  ಯದಿ(!Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "Process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    result = h2_process_pending_ಒಳಹರಿವು(cf, data);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  ಯದಿ(!data_max_bytes)
    data_max_bytes = H2_CHUNK_SIZE;

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop due to connection close or us not processing
   * all network input */
  ಯಾವತ್(!ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) {
    ಸಂರಚನಾ h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    ಯದಿ(stream && (stream->closed || !data_max_bytes)) {
      /* We would like to abort here and stop processing, so that the transfer
       * loop can handle the data/close here. This may leave data in
       * underlying buffers that will not be consumed. */
      ಯದಿ(!cf->next || !cf->next->cft->has_data_pending(cf->next, data))
        Curl_multi_mark_dirty(data);
      ಭಙ್ಗ;
    }

    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    ಯದಿ(result) {
      ಯದಿ(result != CURLE_AGAIN) {
        failf(data, "Failed receiving HTTP2 data: %d(%s)", (ಪೂರ್ಣಾಙ್ಕ)result,
              curl_easy_strerror(result));
        ಪ್ರತ್ಯಾಗಮ result;
      }
      ಭಙ್ಗ;
    }
    ಅನ್ಯಥಾ ಯದಿ(nread == 0) {
      CURL_TRC_CF(data, cf, "[0] ingress: connection closed");
      ctx->conn_closed = TRUE;
      ಭಙ್ಗ;
    }
    ಅನ್ಯಥಾ {
      CURL_TRC_CF(data, cf, "[0] ingress: read %zu bytes", nread);
      data_max_bytes = (data_max_bytes > nread) ? (data_max_bytes - nread) : 0;
    }

    result = h2_process_pending_ಒಳಹರಿವು(cf, data);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    CURL_TRC_CF(data, cf, "[0] ingress: nw-in buffered %zu",
                Curl_bufq_len(&ctx->inbufq));
  }

  ಯದಿ(ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "server closed %s",
                ctx->rcvd_goaway ? "with GOAWAY" : "abruptly");
    connclose(cf->conn);
  }

  CURL_TRC_CF(data, cf, "[0] ingress: done");
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_h2_recv(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                           ವರ್ಣ *buf, size_t len, size_t *pnread)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ h2_stream_ctx *stream;
  CURLcode result, r2;
  ಸಂರಚನಾ cf_call_data save;

  ಯದಿ(!data)
    ಪ್ರತ್ಯಾಗಮ CURLE_HTTP2;

  stream = H2_STREAM_CTX(ctx, data);

  *pnread = 0;
  ಯದಿ(!stream) {
    /* Abnormal call sequence: either this transfer has never opened a stream
     * (unlikely) or the transfer has been done, cleaned up its resources, but
     * a read() is called anyway. It is not clear what the calling sequence
     * is for such a case. */
    failf(data, "http/2 recv on a transfer never opened "
          "or already cleared, mid=%u", data->mid);
    ಪ್ರತ್ಯಾಗಮ CURLE_HTTP2;
  }

  CF_DATA_SAVE(save, cf, data);

  result = stream_recv(cf, data, stream, buf, len, pnread);
  ಯದಿ(result && (result != CURLE_AGAIN))
    ಗಚ್ಛ out;

  ಯದಿ(result) {
    result = h2_progress_ingress(cf, data, len);
    ಯದಿ(result)
      ಗಚ್ಛ out;

    result = stream_recv(cf, data, stream, buf, len, pnread);
  }

  ಯದಿ(*pnread > 0) {
    /* Now that we transferred this to the upper layer, we report
     * the actual amount of DATA consumed to the H2 session, so
     * that it adjusts stream flow control */
    Curl_ngh2_session_consume(&ctx->ngh2, stream->id, *pnread);
    ಯದಿ(stream->closed) {
      CURL_TRC_CF(data, cf, "[%d] DRAIN closed stream", stream->id);
      Curl_multi_mark_dirty(data);
    }
  }

out:
  r2 = h2_progress_egress(cf, data);
  ಯದಿ(r2 == CURLE_AGAIN) {
    /* pending data to send, need to be called again. Ideally, we
     * monitor the socket for POLLOUT, but when not SENDING
     * any more, we force processing of the transfer. */
    ಯದಿ(!CURL_REQ_WANT_SEND(data))
      Curl_multi_mark_dirty(data);
  }
  ಅನ್ಯಥಾ ಯದಿ(r2 && (result == CURLE_AGAIN)) {
    /* Do not replace a previous error result NOR a successful result
     * with an egress error. Example: the server might close the
     * connection after sending the complete response with EOS
     * and everything. Further sending may fail, but that MUST
     * not change that we had a successful response. */
    result = r2;
  }

  CURL_TRC_CF(data, cf, "[%d] cf_recv(len=%zu) -> %d, %zu, "
              "window=%d/%d, connection %d/%d",
              stream->id, len, (ಪೂರ್ಣಾಙ್ಕ)result, *pnread,
              Curl_ngh2_session_get_stream_effective_recv_data_length(
                &ctx->ngh2, stream->id),
              Curl_ngh2_session_get_stream_effective_local_window_size(
                &ctx->ngh2, stream->id),
              Curl_ngh2_session_get_local_window_size(&ctx->ngh2),
              HTTP2_HUGE_WINDOW_SIZE);

  CF_DATA_RESTORE(cf, save);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_h2_body_send(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ h2_stream_ctx *stream,
                                ಧ್ರುವ ಶೂನ್ಯ *buf, size_t blen, bool eos,
                                size_t *pnwritten)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  CURLcode result;

  *pnwritten = 0;
  ಯದಿ(stream->closed) {
    ಯದಿ(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(data, cf, "[%d] discarding data"
                  "on closed stream with response", stream->id);
      ಯದಿ(eos)
        stream->body_eos = TRUE;
      *pnwritten = blen;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    /* Server closed before we got a response, this is an error */
    infof(data, "stream %d closed", stream->id);
    ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
  }

  result = Curl_bufq_write(&stream->sendbuf, buf, blen, pnwritten);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  ಯದಿ(eos && (blen == *pnwritten))
    stream->body_eos = TRUE;

  ಯದಿ(eos || !Curl_bufq_is_empty(&stream->sendbuf)) {
    /* resume the potentially suspended stream */
    ಯದಿ(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id)))
      ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode h2_submit(ಸಂರಚನಾ h2_stream_ctx **pstream,
                          ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                          ಧ್ರುವ uint8_t *buf, size_t len,
                          bool eos, size_t *pnwritten)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ h2_stream_ctx *stream = ರಿಕ್ತ;
  ಸಂರಚನಾ dynhds h2_headers;
  nghttp2_nv *nva = ರಿಕ್ತ;
  ಧ್ರುವ ಶೂನ್ಯ *body = ರಿಕ್ತ;
  size_t nheader, bodylen;
  nghttp2_data_provider data_prd;
  int32_t stream_id;
  nghttp2_priority_spec pri_spec;
  size_t nwritten;
  CURLcode result = CURLE_OK;
  uint32_t initial_win_size;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_headers, MAX_HTTP_RESP_HEADER_ಎಣಿಕೆ, DYN_HTTP_REQUEST);

  result = http2_data_setup(cf, data, &stream);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  result = Curl_h1_req_parse_read(
    &stream->h1, buf, len, ರಿಕ್ತ,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : ರಿಕ್ತ,
    0, &nwritten);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  *pnwritten = nwritten;
  ಯದಿ(!stream->h1.done) {
    /* need more data */
    ಗಚ್ಛ out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nva = Curl_dynhds_to_nva(&h2_headers, &nheader);
  ಯದಿ(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  h2_pri_spec(data, &pri_spec);
  ಯದಿ(!Curl_ngh2_session_check_request_allowed(&ctx->ngh2))
    CURL_TRC_CF(data, cf, "send request NOT allowed (via nghttp2)");

  /* Check the initial windows size of the transfer (rate-limits?) and
   * send an updated settings on changes from previous value. */
  initial_win_size = cf_h2_initial_win_size(data);
  ಯದಿ(initial_win_size != ctx->initial_win_size) {
    result = cf_h2_update_settings(ctx, initial_win_size);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  ವಿಕಲ್ಪ(data->state.httpreq) {
  ಪಕ್ಷ HTTPREQ_POST:
  ಪಕ್ಷ HTTPREQ_POST_FORM:
  ಪಕ್ಷ HTTPREQ_POST_MIME:
  ಪಕ್ಷ HTTPREQ_PUT:
    data_prd.read_callback = req_body_read_callback;
    data_prd.source.ptr = ರಿಕ್ತ;
    stream_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, &data_prd, stream);
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    stream_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, ರಿಕ್ತ, stream);
  }

  ಯದಿ(stream_id < 0) {
    CURL_TRC_CF(data, cf, "send: nghttp2_submit_request error (%s)%d",
                nghttp2_strerror(stream_id), stream_id);
    result = CURLE_SEND_ERROR;
    ಗಚ್ಛ out;
  }

#ifdef CURLVERBOSE
#define MAX_ACC 60000  /* <64KB to account for some overhead */
  ಯದಿ(Curl_trc_is_verbose(data)) {
    size_t acc = 0, i;

    infof(data, "[HTTP/2] [%d] OPENED stream for %s",
          stream_id, Curl_bufref_ptr(&data->state.url));
    ಪ್ರತಿ(i = 0; i < nheader; ++i) {
      acc += nva[i].namelen + nva[i].valuelen;

      infof(data, "[HTTP/2] [%d] [%.*s: %.*s]", stream_id,
            (ಪೂರ್ಣಾಙ್ಕ)nva[i].namelen, nva[i].name,
            (ಪೂರ್ಣಾಙ್ಕ)nva[i].valuelen, nva[i].ಮೌಲ್ಯ);
    }

    ಯದಿ(acc > MAX_ACC) {
      infof(data, "[HTTP/2] Warning: The cumulative length of all "
            "headers exceeds %d bytes and that could cause the "
            "stream to be rejected.", MAX_ACC);
    }
  }
#endif

  stream->id = stream_id;

  body = (ಧ್ರುವ ವರ್ಣ *)buf + *pnwritten;
  bodylen = len - *pnwritten;

  ಯದಿ(bodylen || eos) {
    size_t n;
    result = cf_h2_body_send(cf, data, stream, body, bodylen, eos, &n);
    ಯದಿ(!result)
      *pnwritten += n;
    ಅನ್ಯಥಾ ಯದಿ(result == CURLE_AGAIN)
      result = CURLE_OK;
    ಅನ್ಯಥಾ {
      result = CURLE_SEND_ERROR;
    }
  }

out:
  CURL_TRC_CF(data, cf, "[%d] submit -> %d, %zu",
              stream ? stream->id : -1, (ಪೂರ್ಣಾಙ್ಕ)result, *pnwritten);
  curlx_safefree(nva);
  *pstream = stream;
  Curl_dynhds_free(&h2_headers);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_h2_send(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                           ಧ್ರುವ uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
  ಸಂರಚನಾ cf_call_data save;
  CURLcode result = CURLE_OK, r2;

  CF_DATA_SAVE(save, cf, data);
  *pnwritten = 0;

  ಯದಿ(!stream || stream->id == -1) {
    result = h2_submit(&stream, cf, data, buf, len, eos, pnwritten);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    DEBUGASSERT(stream);
  }
  ಅನ್ಯಥಾ ಯದಿ(stream->body_eos) {
    /* We already wrote this, but CURLE_AGAIN-ed the call due to not
     * being able to flush stream->sendbuf. Make a 0-length write
     * to trigger flushing again.
     * If this works, we report to have written `len` bytes. */
    size_t n;
    DEBUGASSERT(eos);
    result = cf_h2_body_send(cf, data, stream, buf, 0, eos, &n);
    CURL_TRC_CF(data, cf, "[%d] cf_body_send last CHUNK -> %d, %zu, eos=%d",
                stream->id, (ಪೂರ್ಣಾಙ್ಕ)result, n, eos);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    *pnwritten = len;
  }
  ಅನ್ಯಥಾ {
    result = cf_h2_body_send(cf, data, stream, buf, len, eos, pnwritten);
    CURL_TRC_CF(data, cf, "[%d] cf_body_send(len=%zu) -> %d, %zu, eos=%d",
                stream->id, len, (ಪೂರ್ಣಾಙ್ಕ)result, *pnwritten, eos);
  }

  /* Call the nghttp2 send loop and flush to write ALL buffered data,
   * headers and/or request body completely out to the network */
  r2 = h2_progress_egress(cf, data);

  /* if the stream has been closed in egress handling (nghttp2 does that
   * when it does not like the headers, for example */
  ಯದಿ(stream && stream->closed && !stream->resp_hds_complete) {
    infof(data, "stream %d closed", stream->id);
    result = CURLE_SEND_ERROR;
    ಗಚ್ಛ out;
  }
  ಅನ್ಯಥಾ ಯದಿ(r2 && (r2 != CURLE_AGAIN)) {
    result = r2;
    ಗಚ್ಛ out;
  }

  ಯದಿ(should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    ಯದಿ(stream && stream->closed) {
      result = http2_handle_stream_close(cf, data, stream, pnwritten);
    }
    ಅನ್ಯಥಾ {
      CURL_TRC_CF(data, cf, "send: nothing to do in this session");
      result = CURLE_HTTP2;
    }
  }

out:
  ಯದಿ(stream) {
    CURL_TRC_CF(data, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
                "eos=%d, h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                stream->id, len, (ಪೂರ್ಣಾಙ್ಕ)result, *pnwritten,
                stream->body_eos,
                Curl_ngh2_session_get_stream_remote_window_size(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&stream->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  ಅನ್ಯಥಾ {
    CURL_TRC_CF(data, cf, "cf_send(len=%zu) -> %d, %zu, "
                "connection-window=%d, nw_send_buffer(%zu)",
                len, (ಪೂರ್ಣಾಙ್ಕ)result, *pnwritten,
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_DATA_RESTORE(cf, save);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_h2_flush(ಸಂರಚನಾ Curl_cfilter *cf,
                            ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
  ಸಂರಚನಾ cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  ಯದಿ(stream && !Curl_bufq_is_empty(&stream->sendbuf)) {
    /* resume the potentially suspended stream */
    ಯದಿ(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id))) {
      result = CURLE_SEND_ERROR;
      ಗಚ್ಛ out;
    }
  }

  result = h2_progress_egress(cf, data);

out:
  ಯದಿ(stream) {
    CURL_TRC_CF(data, cf, "[%d] flush -> %d, "
                "h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                stream->id, (ಪೂರ್ಣಾಙ್ಕ)result,
                Curl_ngh2_session_get_stream_remote_window_size(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&stream->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  ಅನ್ಯಥಾ {
    CURL_TRC_CF(data, cf, "flush -> %d, "
                "connection-window=%d, nw_send_buffer(%zu)", (ಪೂರ್ಣಾಙ್ಕ)result,
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_DATA_RESTORE(cf, save);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_h2_adjust_pollset(ಸಂರಚನಾ Curl_cfilter *cf,
                                     ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ cf_call_data save;
  curl_socket_t sock;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  ಯದಿ(!Curl_ngh2_valid(&ctx->ngh2))
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  sock = Curl_conn_cf_get_socket(cf, data);
  Curl_pollset_check(data, ps, sock, &want_recv, &want_send);
  ಯದಿ(want_recv || want_send) {
    ಸಂರಚನಾ h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
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
  ಅನ್ಯಥಾ ಯದಿ(ctx->sent_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_DATA_SAVE(save, cf, data);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CF_DATA_RESTORE(cf, save);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_h2_ctx_open(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ h2_stream_ctx *stream;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ಪೂರ್ಣಾಙ್ಕ rc;
  nghttp2_session_callbacks *cbs = ರಿಕ್ತ;

  DEBUGASSERT(!ctx->ngh2.s);
  DEBUGASSERT(!ctx->ngh2.fatal_rv);
  DEBUGASSERT(ctx->initialized);

  rc = nghttp2_session_callbacks_new(&cbs);
  ಯದಿ(rc) {
    failf(data, "Could not initialize nghttp2 callbacks");
    ಗಚ್ಛ out;
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
  ಯದಿ(rc) {
    failf(data, "Could not initialize nghttp2");
    ಗಚ್ಛ out;
  }
  ctx->max_concurrent_streams = data->multi ?
    Curl_multi_max_concurrent_streams(data->multi) :
    DEFAULT_MAX_CONCURRENT_STREAMS;

  ಯದಿ(ctx->via_h1_upgrade) {
    /* HTTP/1.1 Upgrade issued. H2 Settings have already been submitted
     * in the H1 request and we upgrade from there. This stream
     * is opened implicitly as #1. */
    uint8_t binsettings[H2_BINSETTINGS_LEN];
    ssize_t rclen;
    size_t binlen; /* length of the binsettings data */

    rclen = populate_binsettings(binsettings, data);

    ಯದಿ(!curlx_sztouz(rclen, &binlen) || !binlen) {
      failf(data, "nghttp2 unexpectedly failed on pack_settings_payload");
      result = CURLE_FAILED_INIT;
      ಗಚ್ಛ out;
    }

    result = http2_data_setup(cf, data, &stream);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    DEBUGASSERT(stream);
    stream->id = 1;
    /* queue SETTINGS frame (again) */
    rc = Curl_ngh2_session_upgrade2(
      &ctx->ngh2, binsettings, binlen, data->state.httpreq == HTTPREQ_HEAD,
      ರಿಕ್ತ);
    ಯದಿ(rc) {
      failf(data, "nghttp2_session_upgrade2() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      ಗಚ್ಛ out;
    }

    rc = Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, stream->id, stream);
    ಯದಿ(rc) {
      infof(data, "http/2: failed to set user_data for stream %d",
            stream->id);
      DEBUGASSERT(0);
    }
    CURL_TRC_CF(data, cf, "created session via Upgrade");
  }
  ಅನ್ಯಥಾ {
    nghttp2_settings_entry iv[H2_SETTINGS_IV_LEN];
    size_t ivlen;

    ivlen = populate_settings(iv, data, ctx);
    rc = Curl_ngh2_submit_settings(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, iv, ivlen);
    ಯದಿ(rc) {
      failf(data, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      ಗಚ್ಛ out;
    }
  }

  rc = Curl_ngh2_session_set_local_window_size(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, HTTP2_HUGE_WINDOW_SIZE);
  ಯದಿ(rc) {
    failf(data, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    result = CURLE_HTTP2;
    ಗಚ್ಛ out;
  }

  /* all set, traffic will be send on connect */
  result = CURLE_OK;
  CURL_TRC_CF(data, cf, "[0] created h2 session%s",
              ctx->via_h1_upgrade ? " (via h1 upgrade)" : "");

out:
  ಯದಿ(cbs)
    nghttp2_session_callbacks_del(cbs);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_h2_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                              ಸಂರಚನಾ Curl_easy *data,
                              bool *done)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ cf_call_data save;
  bool first_time = FALSE;

  ಯದಿ(cf->connected) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  /* Connect the lower filters first */
  ಯದಿ(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    ಯದಿ(result || !*done)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  *done = FALSE;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(ctx->initialized);
  ಯದಿ(!ctx->ngh2.s) {
    result = cf_h2_ctx_open(cf, data);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    first_time = TRUE;
  }

  ಯದಿ(!first_time) {
    result = h2_progress_ingress(cf, data, 0);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  /* Send out our SETTINGS and ACKs and such. If that blocks, we
   * have it buffered and can count this filter as being connected */
  result = h2_progress_egress(cf, data);
  ಯದಿ(result && (result != CURLE_AGAIN))
    ಗಚ್ಛ out;

  *done = TRUE;
  cf->connected = TRUE;
  result = CURLE_OK;

out:
  CURL_TRC_CF(data, cf, "cf_connect() -> %d, %d, ", (ಪೂರ್ಣಾಙ್ಕ)result, *done);
  CF_DATA_RESTORE(cf, save);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_h2_destroy(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;

  (ಶೂನ್ಯ)data;
  ಯದಿ(ctx) {
    cf_h2_ctx_free(ctx);
    cf->ctx = ರಿಕ್ತ;
  }
}

ಸ್ಥಿರ CURLcode cf_h2_shutdown(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ cf_call_data save;
  CURLcode result;
  ಪೂರ್ಣಾಙ್ಕ rv;

  ಯದಿ(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);

  ಯದಿ(!ctx->sent_goaway) {
    ctx->sent_goaway = TRUE;
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, ctx->local_max_sid, 0,
      (ಧ್ರುವ uint8_t *)"shutdown", ಪರಿಮಾಣ("shutdown"));
    ಯದಿ(rv) {
      failf(data, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      result = CURLE_SEND_ERROR;
      ಗಚ್ಛ out;
    }
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  result = CURLE_OK;
  ಯದಿ(Curl_ngh2_session_want_write(&ctx->ngh2) ||
     !Curl_bufq_is_empty(&ctx->outbufq))
    result = h2_progress_egress(cf, data);
  ಯದಿ(!result && Curl_ngh2_session_want_read(&ctx->ngh2))
    result = h2_progress_ingress(cf, data, 0);

  ಯದಿ(result == CURLE_AGAIN)
    result = CURLE_OK;

  *done = (ctx->conn_closed ||
           (!result && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2) &&
            Curl_bufq_is_empty(&ctx->outbufq)));

out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode http2_data_pause(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 bool pause)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);

  DEBUGASSERT(data);
  ಯದಿ(ctx && Curl_ngh2_valid(&ctx->ngh2) && stream) {
    CURLcode result;

    stream->write_paused = pause;
    result = cf_h2_update_local_win(cf, data, stream);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;

    /* attempt to send the window update */
    (ಶೂನ್ಯ)h2_progress_egress(cf, data);

    ಯದಿ(!pause) {
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
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_h2_cntrl(ಸಂರಚನಾ Curl_cfilter *cf,
                            ಸಂರಚನಾ Curl_easy *data,
                            ಪೂರ್ಣಾಙ್ಕ event, ಪೂರ್ಣಾಙ್ಕ arg1, ಶೂನ್ಯ *arg2)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ cf_call_data save;

  (ಶೂನ್ಯ)arg2;

  CF_DATA_SAVE(save, cf, data);
  ವಿಕಲ್ಪ(event) {
  ಪಕ್ಷ CF_CTRL_DATA_SETUP:
    ಭಙ್ಗ;
  ಪಕ್ಷ CF_CTRL_DATA_PAUSE:
    result = http2_data_pause(cf, data, (arg1 != 0));
    ಭಙ್ಗ;
  ಪಕ್ಷ CF_CTRL_FLUSH:
    result = cf_h2_flush(cf, data);
    ಭಙ್ಗ;
  ಪಕ್ಷ CF_CTRL_DATA_DONE:
    http2_data_done(cf, data);
    ಭಙ್ಗ;
  ಪಕ್ಷ CF_CTRL_CONN_INFO_UPDATE:
    ಯದಿ(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 20;
      Curl_conn_set_multiplex(cf->conn);
    }
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  CF_DATA_RESTORE(cf, save);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ bool cf_h2_data_pending(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಧ್ರುವ ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;

  ಯದಿ(ctx && !Curl_bufq_is_empty(&ctx->inbufq))
    ಪ್ರತ್ಯಾಗಮ TRUE;
  ಪ್ರತ್ಯಾಗಮ cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

ಸ್ಥಿರ bool cf_h2_is_alive(ಸಂರಚನಾ Curl_cfilter *cf,
                           ಸಂರಚನಾ Curl_easy *data,
                           bool *ಒಳಹರಿವು_pending)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  bool alive;
  ಸಂರಚನಾ cf_call_data save;

  *ಒಳಹರಿವು_pending = FALSE;
  CF_DATA_SAVE(save, cf, data);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
           http2_connisalive(cf, data, ಒಳಹರಿವು_pending));
  CURL_TRC_CF(data, cf, "conn alive -> %d, input_pending=%d",
              alive, *ಒಳಹರಿವು_pending);
  CF_DATA_RESTORE(cf, save);
  ಪ್ರತ್ಯಾಗಮ alive;
}

ಸ್ಥಿರ CURLcode cf_h2_keep_alive(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data)
{
  CURLcode result;
  ಸಂರಚನಾ cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  result = http2_send_ping(cf, data);
  CF_DATA_RESTORE(cf, save);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_h2_query(ಸಂರಚನಾ Curl_cfilter *cf,
                            ಸಂರಚನಾ Curl_easy *data,
                            ಪೂರ್ಣಾಙ್ಕ query, ಪೂರ್ಣಾಙ್ಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ಸಂರಚನಾ cf_h2_ctx *ctx = cf->ctx;
  ಸಂರಚನಾ cf_call_data save;
  size_t effective_max;

  ವಿಕಲ್ಪ(query) {
  ಪಕ್ಷ CF_QUERY_MAX_CONCURRENT:
    DEBUGASSERT(pres1);

    CF_DATA_SAVE(save, cf, data);
    ಯದಿ(!Curl_ngh2_valid(&ctx->ngh2) ||
       !Curl_ngh2_session_check_request_allowed(&ctx->ngh2)) {
      /* the limit is what we have in use right now */
      effective_max = cf->conn->attached_xfers;
    }
    ಅನ್ಯಥಾ {
      effective_max = ctx->max_concurrent_streams;
    }
    *pres1 = (effective_max > INT_MAX) ? INT_MAX : (ಪೂರ್ಣಾಙ್ಕ)effective_max;
    CF_DATA_RESTORE(cf, save);
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ CF_QUERY_STREAM_ERROR: {
    ಸಂರಚನಾ h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    *pres1 = stream ? (ಪೂರ್ಣಾಙ್ಕ)stream->error : 0;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಪಕ್ಷ CF_QUERY_NEED_FLUSH: {
    ಸಂರಚನಾ h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    ಯದಿ(!Curl_bufq_is_empty(&ctx->outbufq) ||
       (stream && !Curl_bufq_is_empty(&stream->sendbuf))) {
      *pres1 = TRUE;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    ಭಙ್ಗ;
  }
  ಪಕ್ಷ CF_QUERY_HTTP_VERSION:
    *pres1 = 20;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ಸಂರಚನಾ Curl_cftype Curl_cft_nghttp2 = {
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

ಸ್ಥಿರ CURLcode http2_cfilter_add(ಸಂರಚನಾ Curl_cfilter **pcf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ connectdata *conn,
                                  int8_t sockindex,
                                  bool via_h1_upgrade)
{
  ಸಂರಚನಾ Curl_cfilter *cf = ರಿಕ್ತ;
  ಸಂರಚನಾ cf_h2_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(data->conn);
  ctx = curlx_calloc(1, ಪರಿಮಾಣ(*ctx));
  ಯದಿ(!ctx)
    ಗಚ್ಛ out;
  cf_h2_ctx_init(ctx, via_h1_upgrade);

  result = Curl_cf_create(&cf, &Curl_cft_nghttp2, ctx);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  ctx = ರಿಕ್ತ;
  Curl_conn_cf_add(data, conn, sockindex, cf);

out:
  ಯದಿ(result)
    cf_h2_ctx_free(ctx);
  *pcf = result ? ರಿಕ್ತ : cf;
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode http2_cfilter_ಸೇರಿಸು_after(ಸಂರಚನಾ Curl_cfilter *cf,
                                           ಸಂರಚನಾ Curl_easy *data,
                                           bool via_h1_upgrade)
{
  ಸಂರಚನಾ Curl_cfilter *cf_h2 = ರಿಕ್ತ;
  ಸಂರಚನಾ cf_h2_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (ಶೂನ್ಯ)data;
  ctx = curlx_calloc(1, ಪರಿಮಾಣ(*ctx));
  ಯದಿ(!ctx)
    ಗಚ್ಛ out;
  cf_h2_ctx_init(ctx, via_h1_upgrade);

  result = Curl_cf_create(&cf_h2, &Curl_cft_nghttp2, ctx);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  ctx = ರಿಕ್ತ;
  Curl_conn_cf_ಸೇರಿಸು_after(cf, cf_h2);

out:
  ಯದಿ(result)
    cf_h2_ctx_free(ctx);
  ಪ್ರತ್ಯಾಗಮ result;
}

bool Curl_http2_may_switch(ಸಂರಚನಾ Curl_easy *data)
{
  ಯದಿ(Curl_conn_http_version(data, data->conn) < 20 &&
     (data->state.http_neg.wanted & CURL_HTTP_V2x) &&
     data->state.http_neg.h2_prior_knowledge) {
#ifndef CURL_DISABLE_PROXY
    ಯದಿ(data->conn->bits.origin_is_proxy) {
      infof(data, "Ignoring HTTP/2 prior knowledge due to proxy");
      ಪ್ರತ್ಯಾಗಮ FALSE;
    }
#endif
    ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

CURLcode Curl_http2_switch(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_cfilter *cf;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, data->conn) < 20);

  result = http2_cfilter_add(&cf, data, data->conn, FIRSTSOCKET, FALSE);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
  CURL_TRC_CF(data, cf, "switching connection to HTTP/2");

  ಯದಿ(cf->next) {
    bool done;
    ಪ್ರತ್ಯಾಗಮ Curl_conn_cf_connect(cf, data, &done);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_http2_switch_at(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_cfilter *cf_h2;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, data->conn) < 20);

  result = http2_cfilter_ಸೇರಿಸು_after(cf, data, FALSE);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  cf_h2 = cf->next;

  ಯದಿ(cf_h2->next) {
    bool done;
    ಪ್ರತ್ಯಾಗಮ Curl_conn_cf_connect(cf_h2, data, &done);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_http2_upgrade(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ connectdata *conn, int8_t sockindex,
                            ಧ್ರುವ ವರ್ಣ *mem, size_t nread)
{
  ಸಂರಚನಾ Curl_cfilter *cf;
  ಸಂರಚನಾ cf_h2_ctx *ctx;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, conn) < 20);

  result = http2_cfilter_add(&cf, data, conn, sockindex, TRUE);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
  CURL_TRC_CF(data, cf, "upgrading connection to HTTP/2");

  DEBUGASSERT(cf->cft == &Curl_cft_nghttp2);
  ctx = cf->ctx;

  data->req.httpversion_sent = 20; /* it is an h2 request now */
  data->req.header = TRUE; /* we expect the real response to come in h2 */
  data->req.headerline = 0; /* restart the header line counter */

  ಯದಿ(nread > 0) {
    /* Remaining data from the protocol switch reply is already using
     * the switched protocol, ie. HTTP/2. We add that to the network
     * inbufq. */
    size_t copied;

    result = Curl_bufq_write(&ctx->inbufq,
                             (ಧ್ರುವ unsigned ವರ್ಣ *)mem, nread, &copied);
    ಯದಿ(result) {
      failf(data, "error on copying HTTP Upgrade response: %d", (ಪೂರ್ಣಾಙ್ಕ)result);
      ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
    }
    ಯದಿ(copied < nread) {
      failf(data, "connection buffer size could not take all data "
            "from HTTP Upgrade response header: copied=%zu, datalen=%zu",
            copied, nread);
      ಪ್ರತ್ಯಾಗಮ CURLE_HTTP2;
    }
    infof(data, "Copied HTTP/2 data in stream buffer to connection buffer"
          " after upgrade: len=%zu", nread);
  }

  ಯದಿ(cf->next) {
    bool done;
    result = Curl_conn_cf_connect(cf, data, &done);
    ಯದಿ(!result)
      cf->cft->cntrl(cf, data, CF_CTRL_CONN_INFO_UPDATE, 0, ರಿಕ್ತ);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/* Only call this function for a transfer that already got an HTTP/2
   CURLE_HTTP2_STREAM error! */
bool Curl_h2_http_1_1_error(ಸಂರಚನಾ Curl_easy *data)
{
  ಯದಿ(Curl_conn_http_version(data, data->conn) == 20) {
    ಪೂರ್ಣಾಙ್ಕ err = Curl_conn_get_stream_error(data, data->conn, FIRSTSOCKET);
    ಪ್ರತ್ಯಾಗಮ err == NGHTTP2_HTTP_1_1_REQUIRED;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಶೂನ್ಯ *Curl_nghttp2_malloc(size_t size, ಶೂನ್ಯ *user_data)
{
  (ಶೂನ್ಯ)user_data;
  ಪ್ರತ್ಯಾಗಮ Curl_cmalloc(size);
}

ಶೂನ್ಯ Curl_nghttp2_free(ಶೂನ್ಯ *ptr, ಶೂನ್ಯ *user_data)
{
  (ಶೂನ್ಯ)user_data;
  Curl_cfree(ptr);
}

ಶೂನ್ಯ *Curl_nghttp2_calloc(size_t nmemb, size_t size, ಶೂನ್ಯ *user_data)
{
  (ಶೂನ್ಯ)user_data;
  ಪ್ರತ್ಯಾಗಮ Curl_ccalloc(nmemb, size);
}

ಶೂನ್ಯ *Curl_nghttp2_realloc(ಶೂನ್ಯ *ptr, size_t size, ಶೂನ್ಯ *user_data)
{
  (ಶೂನ್ಯ)user_data;
  ಪ್ರತ್ಯಾಗಮ Curl_crealloc(ptr, size);
}

#ಅನ್ಯಥಾ /* CURL_DISABLE_HTTP || !USE_NGHTTP2 */

/* Satisfy external references even if http2 is not compiled in. */

ವರ್ಣ *curl_pushheader_bynum(ಸಂರಚನಾ curl_pushheaders *h, size_t ಅಂಕೆ)
{
  (ಶೂನ್ಯ)h;
  (ಶೂನ್ಯ)ಅಂಕೆ;
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ವರ್ಣ *curl_pushheader_byname(ಸಂರಚನಾ curl_pushheaders *h, ಧ್ರುವ ವರ್ಣ *name)
{
  (ಶೂನ್ಯ)h;
  (ಶೂನ್ಯ)name;
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#endif /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
