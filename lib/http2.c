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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#ଅନ୍ତର୍ଭାଵ <nghttp2/nghttp2.h>

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "bufq.h"
#ଅନ୍ତର୍ଭାଵ "u32_ptrset.h"
#ଅନ୍ତର୍ଭାଵ "http1.h"
#ଅନ୍ତର୍ଭାଵ "http2.h"
#ଅନ୍ତର୍ଭାଵ "http.h"
#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "curlx/base64.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "ngh2_shim.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "urlapi-int.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "bufref.h"
#ଅନ୍ତର୍ଭାଵ "curlx/dynbuf.h"
#ଅନ୍ତର୍ଭାଵ "headers.h"
#ଅନ୍ତର୍ଭାଵ "curl_share.h"

#ଯଦି NGHTTP2_VERSION_ଅଙ୍କ < 0x010f00
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
#ଯଦି NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
#define H2_STREAM_WINDOW_SIZE_INITIAL  (64 * 1024)
#ଅନ୍ୟଥା
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

ସଂରଚନା cf_h2_ctx {
  ସଂରଚନା curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  ସଂରଚନା cf_call_data call_data;

  ସଂରଚନା bufq inbufq;           /* network input */
  ସଂରଚନା bufq outbufq;          /* network output */
  ସଂରଚନା bufc_pool stream_bufcp; /* spares for stream buffers */
  ସଂରଚନା dynbuf scratch;        /* scratch buffer for temp use */

  ସଂରଚନା u32_ptrset streams; /* hash of `data->mid` to `h2_stream_ctx` */
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
  BIT(enable_ଠେଲ);
  BIT(nw_out_blocked);
};

/* How to access `call_data` from a cf_h2 filter */
#undef CF_CTX_CALL_DATA
#define CF_CTX_CALL_DATA(cf) ((ସଂରଚନା cf_h2_ctx *)(cf)->ctx)->call_data

/**
 * All about the H2 internals of a stream
 */
ସଂରଚନା h2_stream_ctx {
  ସଂରଚନା bufq sendbuf; /* request buffer */
  ସଂରଚନା h1_req_parser h1; /* parsing the request */
  ସଂରଚନା dynhds resp_trailers; /* response trailer fields */
  size_t resp_hds_len; /* amount of response header bytes in recvbuf */
  curl_off_t xfer_id;  /* transfer `id` owning this stream */
  curl_off_t nrcvd_data;  /* number of DATA bytes received */

  ଅକ୍ଷର **ଠେଲ_headers;       /* allocated array */
  size_t ଠେଲ_headers_used;  /* number of entries filled in */
  size_t ଠେଲ_headers_alloc; /* number of entries allocated */

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

ସ୍ଥିର ଶୂନ୍ୟ free_ଠେଲ_headers(ସଂରଚନା h2_stream_ctx *stream)
{
  size_t i;
  ପାଇଁ(i = 0; i < stream->ଠେଲ_headers_used; i++)
    curlx_free(stream->ଠେଲ_headers[i]);
  curlx_safefree(stream->ଠେଲ_headers);
  stream->ଠେଲ_headers_used = 0;
}

ସ୍ଥିର ଶୂନ୍ୟ h2_stream_ctx_free(ସଂରଚନା h2_stream_ctx *stream)
{
  Curl_bufq_free(&stream->sendbuf);
  Curl_h1_req_parse_free(&stream->h1);
  Curl_dynhds_free(&stream->resp_trailers);
  free_ଠେଲ_headers(stream);
  curlx_free(stream);
}

ସ୍ଥିର ଶୂନ୍ୟ h2_stream_hash_free(uint32_t id, ଶୂନ୍ୟ *stream)
{
  (ଶୂନ୍ୟ)id;
  DEBUGASSERT(stream);
  h2_stream_ctx_free((ସଂରଚନା h2_stream_ctx *)stream);
}

ସ୍ଥିର ଶୂନ୍ୟ cf_h2_ctx_init(ସଂରଚନା cf_h2_ctx *ctx, bool via_h1_upgrade)
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

ସ୍ଥିର ଶୂନ୍ୟ cf_h2_ctx_free(ସଂରଚନା cf_h2_ctx *ctx)
{
  ଯଦି(ctx && ctx->initialized) {
    Curl_ngh2_del(&ctx->ngh2);
    Curl_bufq_free(&ctx->inbufq);
    Curl_bufq_free(&ctx->outbufq);
    Curl_bufcp_free(&ctx->stream_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    memset(ctx, 0, ଆକାର(*ctx));
  }
  curlx_free(ctx);
}

ସ୍ଥିର uint32_t cf_h2_initial_win_size(ସଂରଚନା Curl_easy *data)
{
#ଯଦି NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
  /* If the transfer has a rate-limit lower than the default initial
   * stream window size, use that. It needs to be at least 8k or servers
   * may be unhappy. */
  curl_off_t rps = Curl_rlimit_per_step(&data->progress.dl.rlimit);
  ଯଦି((rps > 0) && (rps < H2_STREAM_WINDOW_SIZE_INITIAL))
    ଫେରନ୍ତୁ CURLMAX((uint32_t)rps, 8192);
#endif
  ଫେରନ୍ତୁ H2_STREAM_WINDOW_SIZE_INITIAL;
}

ସ୍ଥିର size_t populate_settings(nghttp2_settings_entry *iv,
                                ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା cf_h2_ctx *ctx)
{
  iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
  iv[0].ମୂଲ୍ୟ = Curl_multi_max_concurrent_streams(data->multi);

  iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
  iv[1].ମୂଲ୍ୟ = cf_h2_initial_win_size(data);
  ଯଦି(ctx)
    ctx->initial_win_size = iv[1].ମୂଲ୍ୟ;
  iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_ଠେଲ;
  iv[2].ମୂଲ୍ୟ = !!data->multi->ଠେଲ_cb;

  ଫେରନ୍ତୁ 3;
}

ସ୍ଥିର ssize_t populate_binsettings(uint8_t *binsettings,
                                    ସଂରଚନା Curl_easy *data)
{
  nghttp2_settings_entry iv[H2_SETTINGS_IV_LEN];
  size_t ivlen;

  ivlen = populate_settings(iv, data, ଖାଲି);
  /* this returns number of bytes it wrote or a negative number on error. */
  ଫେରନ୍ତୁ nghttp2_pack_settings_payload(binsettings, H2_BINSETTINGS_LEN,
                                       iv, ivlen);
}

ସ୍ଥିର CURLcode cf_h2_update_settings(ସଂରଚନା cf_h2_ctx *ctx,
                                      uint32_t initial_win_size)
{
  nghttp2_settings_entry entry;
  entry.settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
  entry.ମୂଲ୍ୟ = initial_win_size;
  ଯଦି(Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, &entry, 1))
    ଫେରନ୍ତୁ CURLE_SEND_ERROR;
  ctx->initial_win_size = initial_win_size;
  ଫେରନ୍ତୁ CURLE_OK;
}

#define H2_STREAM_CTX(ctx, data)                                         \
  ((ସଂରଚନା h2_stream_ctx *)(                                             \
    (data) ? Curl_u32_ptrset_get(&(ctx)->streams, (data)->mid) : ଖାଲି))

ସ୍ଥିର ସଂରଚନା h2_stream_ctx *h2_stream_ctx_create(ସଂରଚନା cf_h2_ctx *ctx,
                                                  ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା h2_stream_ctx *stream;

  stream = curlx_calloc(1, ଆକାର(*stream));
  ଯଦି(!stream)
    ଫେରନ୍ତୁ ଖାଲି;

  stream->id = -1;
  stream->xfer_id = data->id;
  stream->mid = data->mid;
  Curl_bufq_initp(&stream->sendbuf, &ctx->stream_bufcp,
                  H2_STREAM_SEND_CHUNKS, BUFQ_OPT_NONE);
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_ଧାଡ଼ି_LEN);
  Curl_dynhds_init(&stream->resp_trailers,
                   MAX_HTTP_RESP_HEADER_ଗଣନା, DYN_HTTP_REQUEST);
  stream->bodystarted = FALSE;
  stream->status_code = -1;
  stream->closed = FALSE;
  stream->close_handled = FALSE;
  stream->error = NGHTTP2_NO_ERROR;
  stream->local_window_size = H2_STREAM_WINDOW_SIZE_INITIAL;
  stream->nrcvd_data = 0;
  ଫେରନ୍ତୁ stream;
}

ସ୍ଥିର ସଂରଚନା Curl_easy *cf_h2_get_stream_xfer(ସଂରଚନା Curl_cfilter *cf,
                                               int32_t stream_id,
                                               ସଂରଚନା h2_stream_ctx **pstream)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  ସଂରଚନା h2_stream_ctx *stream =
    Curl_ngh2_session_get_stream_user_data(&ctx->ngh2, stream_id);
  ଯଦି(pstream)
    *pstream = stream;
  ଯଦି(stream && (stream->mid != UINT32_MAX)) {
    ସଂରଚନା Curl_easy *data, *call_data = CF_DATA_CURRENT(cf);
    ଯଦି(!call_data || !call_data->multi) {
      DEBUGASSERT(0);
      ଫେରନ୍ତୁ ଖାଲି;
    }
    data = Curl_multi_get_easy(call_data->multi, stream->mid);
    ଯଦି(data && (data->id == stream->xfer_id))
      ଫେରନ୍ତୁ data;
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

#ifdef NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
ସ୍ଥିର int32_t cf_h2_get_desired_local_win(ସଂରଚନା Curl_cfilter *cf,
                                           ସଂରଚନା Curl_easy *data)
{
  curl_off_t avail = Curl_rlimit_avail(&data->progress.dl.rlimit, ଖାଲି);

  (ଶୂନ୍ୟ)cf;
  ଯଦି(avail < CURL_OFF_T_MAX) { /* limit in place */
    ଯଦି(avail <= 0)
      ଫେରନ୍ତୁ 0;
    ଅନ୍ୟଥା ଯଦି(avail < H2_STREAM_WINDOW_SIZE_MAX)
      ଫେରନ୍ତୁ (int32_t)avail;
  }
  ଫେରନ୍ତୁ H2_STREAM_WINDOW_SIZE_MAX;
}

ସ୍ଥିର CURLcode cf_h2_update_local_win(ସଂରଚନା Curl_cfilter *cf,
                                       ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା h2_stream_ctx *stream)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  int32_t dwsize;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv;

  dwsize = (stream->write_paused || stream->xfer_result) ?
           0 : cf_h2_get_desired_local_win(cf, data);
  ଯଦି(dwsize != stream->local_window_size) {
    int32_t wsize = Curl_ngh2_session_get_stream_effective_local_window_size(
      &ctx->ngh2, stream->id);
    ଯଦି(dwsize > wsize) {
      rv = Curl_ngh2_session_set_local_window_size(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize);
      ଯଦି(rv) {
        failf(data, "[%d] nghttp2 set_local_window_size(%d) failed: "
              "%s(%d)", stream->id, dwsize, nghttp2_strerror(rv), rv);
        ଫେରନ୍ତୁ CURLE_HTTP2;
      }
      rv = Curl_ngh2_submit_window_update(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize - wsize);
      ଯଦି(rv) {
        failf(data, "[%d] nghttp2_submit_window_update() failed: "
              "%s(%d)", stream->id, nghttp2_strerror(rv), rv);
        ଫେରନ୍ତୁ CURLE_HTTP2;
      }
      stream->local_window_size = dwsize;
      CURL_TRC_CF(data, cf, "[%d] local window update by %d",
                  stream->id, dwsize - wsize);
    }
    ଅନ୍ୟଥା {
      rv = Curl_ngh2_session_set_local_window_size(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize);
      ଯଦି(rv) {
        failf(data, "[%d] nghttp2_session_set_local_window_size() failed: "
              "%s(%d)", stream->id, nghttp2_strerror(rv), rv);
        ଫେରନ୍ତୁ CURLE_HTTP2;
      }
      stream->local_window_size = dwsize;
      CURL_TRC_CF(data, cf, "[%d] local window size now %d",
                  stream->id, dwsize);
    }
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

#ଅନ୍ୟଥା /* NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

ସ୍ଥିର CURLcode cf_h2_update_local_win(ସଂରଚନା Curl_cfilter *cf,
                                       ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା h2_stream_ctx *stream)
{
  (ଶୂନ୍ୟ)cf;
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)stream;
  ଫେରନ୍ତୁ CURLE_OK;
}
#endif /* !NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

ସ୍ଥିର CURLcode http2_data_setup(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା h2_stream_ctx **pstream)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  ସଂରଚନା h2_stream_ctx *stream;

  (ଶୂନ୍ୟ)cf;
  DEBUGASSERT(data);

  ଯଦି(!data)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  stream = H2_STREAM_CTX(ctx, data);
  ଯଦି(stream) {
    *pstream = stream;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  stream = h2_stream_ctx_create(ctx, data);
  ଯଦି(!stream)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  ଯଦି(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    h2_stream_ctx_free(stream);
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  *pstream = stream;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode nw_out_flush(ସଂରଚନା Curl_cfilter *cf,
                             ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  ଯଦି(Curl_bufq_is_empty(&ctx->outbufq))
    ଫେରନ୍ତୁ CURLE_OK;

  result = Curl_cf_send_bufq(cf->next, data, &ctx->outbufq, ଖାଲି, 0,
                             &nwritten);
  ଯଦି(result) {
    ଯଦି(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_out_blocked = 1;
    }
    ଫେରନ୍ତୁ result;
  }
  ଫେରନ୍ତୁ Curl_bufq_is_empty(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

ସ୍ଥିର ଶୂନ୍ୟ http2_data_done(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  ସଂରଚନା h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);

  DEBUGASSERT(ctx);
  ଯଦି(!stream || !ctx->initialized)
    ଫେରନ୍ତୁ;

  ଯଦି(Curl_ngh2_valid(&ctx->ngh2)) {
    bool flush_egress = FALSE;
    /* returns error if stream not known, which is fine here */
    (ଶୂନ୍ୟ)Curl_ngh2_session_set_stream_user_data(&ctx->ngh2, stream->id, ଖାଲି);

    ଯଦି(!stream->closed && stream->id > 0) {
      /* RST_STREAM */
      CURL_TRC_CF(data, cf, "[%d] premature DATA_DONE, RST stream",
                  stream->id);
      stream->closed = TRUE;
      stream->reset = TRUE;
      Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                  stream->id, NGHTTP2_STREAM_CLOSED);
      flush_egress = TRUE;
    }

    ଯଦି(flush_egress) {
      (ଶୂନ୍ୟ)Curl_ngh2_session_send(&ctx->ngh2);
      (ଶୂନ୍ୟ)nw_out_flush(cf, data);
    }
  }

  Curl_u32_ptrset_unset(&ctx->streams, data->mid);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା h2_client_new(ସଂରଚନା Curl_cfilter *cf,
                         nghttp2_session_callbacks *cbs)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { ଖାଲି, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = nghttp2_option_new(&o);
  ଯଦି(rc)
    ଫେରନ୍ତୁ rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_set_no_auto_window_update(o, 1);
#ଯଦି NGHTTP2_VERSION_ଅଙ୍କ >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_set_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#endif
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  ଫେରନ୍ତୁ rc;
}

/*
 * Returns nonzero if current HTTP/2 session should be closed.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା should_close_session(ସଂରଚନା cf_h2_ctx *ctx)
{
  ଫେରନ୍ତୁ ctx->drain_total == 0 && !Curl_ngh2_session_want_read(&ctx->ngh2) &&
    !Curl_ngh2_session_want_write(&ctx->ngh2);
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
ସ୍ଥିର CURLcode h2_process_pending_ନିବେଶ(ସଂରଚନା Curl_cfilter *cf,
                                         ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  ଅଚଳ unsigned ଅକ୍ଷର *buf;
  size_t blen, nread;
  ssize_t rv;

  ଯେତେବେଳେ(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {
    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ଅଚଳ uint8_t *)buf, blen);
    ଯଦି(!curlx_sztouz(rv, &nread)) {
      failf(data, "nghttp2 recv error %zd: %s", rv, nghttp2_strerror((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)rv));
      ଫେରନ୍ତୁ CURLE_RECV_ERROR;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    ଯଦି(Curl_bufq_is_empty(&ctx->inbufq)) {
      ଭାଙ୍ଗ;
    }
    ଅନ୍ୟଥା {
      CURL_TRC_CF(data, cf, "process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }

  ଯଦି(Curl_ngh2_session_check_request_allowed(&ctx->ngh2) == 0) {
    /* No more requests are allowed in the current session, so
       the connection may not be reused. This is set when a
       GOAWAY frame has been received or when the limit of stream
       identifiers has been reached. */
    CURL_TRC_M(data, "http/2: No new requests allowed");
    connclose(cf->conn);
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * The server may send us data at any point (e.g. PING frames). Therefore, we
 * cannot assume that an HTTP/2 socket is dead because it is readable.
 *
 * Check the lower filters first and, if successful, peek at the socket
 * and distinguish between closed and data.
 */
ସ୍ଥିର bool http2_connisalive(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                              bool *ନିବେଶ_pending)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *ନିବେଶ_pending = FALSE;
  ଯଦି(!cf->next || !cf->next->cft->is_alive(cf->next, data, ନିବେଶ_pending))
    ଫେରନ୍ତୁ FALSE;

  ଯଦି(*ନିବେଶ_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode result;
    size_t nread;

    *ନିବେଶ_pending = FALSE;
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(data, cf, "connisalive, recv pending input -> %d, %zu",
                (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, nread);
    ଯଦି(!result) {
      result = h2_process_pending_ନିବେଶ(cf, data);
      ଯଦି(result)
        /* immediate error, considered dead */
        alive = FALSE;
      ଅନ୍ୟଥା {
        alive = !should_close_session(ctx);
      }
    }
    ଅନ୍ୟଥା ଯଦି(result != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  ଫେରନ୍ତୁ alive;
}

ସ୍ଥିର CURLcode http2_send_ping(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  rc = Curl_ngh2_submit_ping(&ctx->ngh2, 0, ZERO_NULL);
  ଯଦି(rc) {
    failf(data, "nghttp2_submit_ping() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    ଫେରନ୍ତୁ CURLE_HTTP2;
  }

  rc = Curl_ngh2_session_send(&ctx->ngh2);
  ଯଦି(rc) {
    failf(data, "nghttp2_session_send() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    ଫେରନ୍ତୁ CURLE_SEND_ERROR;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Store nghttp2 version info in this buffer.
 */
ଶୂନ୍ୟ Curl_http2_ver(ଅକ୍ଷର *p, size_t len)
{
  nghttp2_info *h2 = nghttp2_version(0);
  (ଶୂନ୍ୟ)curl_msnprintf(p, len, "nghttp2/%s", h2->version_str);
}

/*
 * The implementation of nghttp2_send_callback type. Here we write |data| with
 * size |length| to the network and return the number of bytes actually
 * written. See the documentation of nghttp2_send_callback for the details.
 */
ସ୍ଥିର ssize_t send_callback(nghttp2_session *h2,
                             ଅଚଳ uint8_t *buf, size_t blen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags,
                             ଶୂନ୍ୟ *userp)
{
  ସଂରଚନା Curl_cfilter *cf = userp;
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result = CURLE_OK;

  (ଶୂନ୍ୟ)h2;
  (ଶୂନ୍ୟ)flags;
  DEBUGASSERT(data);

  ଯଦି(!cf->connected)
    result = Curl_bufq_write(&ctx->outbufq, buf, blen, &nwritten);
  ଅନ୍ୟଥା
    result = Curl_cf_send_bufq(cf->next, data, &ctx->outbufq, buf, blen,
                               &nwritten);

  ଯଦି(result) {
    ଯଦି(result == CURLE_AGAIN) {
      ctx->nw_out_blocked = 1;
      ଫେରନ୍ତୁ NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(data, "Failed sending HTTP2 data");
    ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  ଯଦି(!nwritten) {
    ctx->nw_out_blocked = 1;
    ଫେରନ୍ତୁ NGHTTP2_ERR_WOULDBLOCK;
  }
  ଫେରନ୍ତୁ (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

/* We pass a pointer to this struct in the push callback, but the contents of
   the struct are hidden from the user. */
ସଂରଚନା curl_pushheaders {
  ସଂରଚନା Curl_easy *data;
  ସଂରଚନା h2_stream_ctx *stream;
  ଅଚଳ nghttp2_ଠେଲ_promise *frame;
};

/*
 * push header access function. Only to be used from within the push callback
 */
ଅକ୍ଷର *curl_pushheader_bynum(ସଂରଚନା curl_pushheaders *h, size_t ଅଙ୍କ)
{
  /* Verify that we got a good easy handle in the push header struct, mostly to
     detect rubbish input fast(er). */
  ଯଦି(!h || !GOOD_EASY_HANDLE(h->data))
    ଫେରନ୍ତୁ ଖାଲି;
  ଅନ୍ୟଥା {
    ଯଦି(h->stream && ଅଙ୍କ < h->stream->ଠେଲ_headers_used)
      ଫେରନ୍ତୁ h->stream->ଠେଲ_headers[ଅଙ୍କ];
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * push header access function. Only to be used from within the push callback
 */
ଅକ୍ଷର *curl_pushheader_byname(ସଂରଚନା curl_pushheaders *h, ଅଚଳ ଅକ୍ଷର *name)
{
  ସଂରଚନା h2_stream_ctx *stream;
  size_t len;
  size_t i;
  /* Verify that we got a good easy handle in the push header struct,
     mostly to detect rubbish input fast(er). Also empty header name
     is rubbish too. We have to allow ":" at the beginning of
     the header, but header == ":" must be rejected. If we have ':' in
     the middle of header, it could be matched in middle of the value,
     this is because we do prefix match.*/
  ଯଦି(!h || !GOOD_EASY_HANDLE(h->data) || !name || !name[0] ||
     !strcmp(name, ":") || strchr(name + 1, ':'))
    ଫେରନ୍ତୁ ଖାଲି;

  stream = h->stream;
  ଯଦି(!stream)
    ଫେରନ୍ତୁ ଖାଲି;

  len = strlen(name);
  ପାଇଁ(i = 0; i < stream->ଠେଲ_headers_used; i++) {
    ଯଦି(!strncmp(name, stream->ଠେଲ_headers[i], len)) {
      /* sub-match, make sure that it is followed by a colon */
      ଯଦି(stream->ଠେଲ_headers[i][len] != ':')
        ଜାରି;
      ଫେରନ୍ତୁ &stream->ଠେଲ_headers[i][len + 1];
    }
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର ସଂରଚନା Curl_easy *h2_duphandle(ସଂରଚନା Curl_cfilter *cf,
                                      ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_easy *second = curl_easy_init();
  ଯଦି(second) {
    ସଂରଚନା h2_stream_ctx *second_stream;
    http2_data_setup(cf, second, &second_stream);
    second->state.weight = data->state.weight;
    ଯଦି(data->share)
      (ଶୂନ୍ୟ)Curl_share_easy_link(second, data->share);
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
  ଫେରନ୍ତୁ second;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_transfer_url(ସଂରଚନା Curl_easy *newhandle,
                            ସଂରଚନା curl_pushheaders *hp,
                            ସଂରଚନା Curl_easy *data)
{
  ଅଚଳ ଅକ୍ଷର *v;
  CURLUcode uc;
  ଅକ୍ଷର *url = ଖାଲି;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;
  CURLU *u = curl_url();

  ଯଦି(!u)
    ଫେରନ୍ତୁ 5;

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_SCHEME);
  ଯଦି(v) {
    uc = curl_url_set(u, CURLUPART_SCHEME, v, 0);
    ଯଦି(uc) {
      rc = 1;
      ଯାଅ fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_AUTHORITY);
  ଯଦି(v) {
    uc = Curl_url_set_authority(u, v);
    ଯଦି(uc) {
      rc = 2;
      ଯାଅ fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_PATH);
  ଯଦି(v) {
    uc = curl_url_set(u, CURLUPART_PATH, v, 0);
    ଯଦି(uc) {
      rc = 3;
      ଯାଅ fail;
    }
  }

  /* We can only allow PUSH of resource from the same origin, e.g.
   * scheme + hostname + port */
  ଯଦି(!Curl_url_same_origin(data->state.uh, u)) {
    rc = 1;
    ଯାଅ fail;
  }

  uc = curl_url_get(u, CURLUPART_URL, &url, 0);
  ଯଦି(uc)
    rc = 4;
fail:
  ଯଦି(rc) {
    curl_url_cleanup(u);
    ଫେରନ୍ତୁ rc;
  }

  DEBUGASSERT(!newhandle->state.uh);
  newhandle->state.uh = u;
  Curl_bufref_set(&newhandle->state.url, url, 0, curl_free);
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ discard_newhandle(ସଂରଚନା Curl_cfilter *cf,
                              ସଂରଚନା Curl_easy *newhandle)
{
  http2_data_done(cf, newhandle);
  (ଶୂନ୍ୟ)Curl_close(&newhandle);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଠେଲ_promise(ସଂରଚନା Curl_cfilter *cf,
                        ସଂରଚନା Curl_easy *data,
                        ଅଚଳ nghttp2_ଠେଲ_promise *frame)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv; /* one of the CURL_PUSH_* defines */

  CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE received",
              frame->promised_stream_id);
  ଯଦି(data->multi->ଠେଲ_cb) {
    ସଂରଚନା h2_stream_ctx *stream;
    ସଂରଚନା h2_stream_ctx *newstream;
    ସଂରଚନା curl_pushheaders heads;
    CURLMcode mresult;
    CURLcode result;
    /* clone the parent */
    ସଂରଚନା Curl_easy *newhandle = h2_duphandle(cf, data);
    ଯଦି(!newhandle) {
      infof(data, "failed to duplicate handle");
      rv = CURL_ଠେଲ_DENY; /* FAIL HARD */
      ଯାଅ fail;
    }

    stream = H2_STREAM_CTX(ctx, data);
    ଯଦି(!stream) {
      failf(data, "Internal NULL stream");
      discard_newhandle(cf, newhandle);
      rv = CURL_ଠେଲ_DENY;
      ଯାଅ fail;
    }

    heads.data = data;
    heads.stream = stream;
    heads.frame = frame;

    rv = set_transfer_url(newhandle, &heads, data);
    ଯଦି(rv) {
      CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE, failed to set URL -> %d",
                  frame->promised_stream_id, rv);
      discard_newhandle(cf, newhandle);
      rv = CURL_ଠେଲ_DENY;
      ଯାଅ fail;
    }

    {
      ସଂରଚନା Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, multi_ଠେଲ_cb);
      rv = data->multi->ଠେଲ_cb(data, newhandle,
                                stream->ଠେଲ_headers_used, &heads,
                                data->multi->ଠେଲ_userp);
      CURL_CBAPI_END(&guard);
    }

    /* free the headers again */
    free_ଠେଲ_headers(stream);

    ଯଦି(rv) {
      DEBUGASSERT((rv > CURL_ଠେଲ_OK) && (rv <= CURL_ଠେଲ_ERROROUT));
      /* denied, kill off the new handle again */
      CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE, denied by application -> %d",
                  frame->promised_stream_id, rv);
      discard_newhandle(cf, newhandle);
      ଯାଅ fail;
    }

    /* approved, add to the multi handle for processing. This
     * assigns newhandle->mid. For the new `mid` we assign the
     * h2_stream instance and remember the stream_id already known. */
    mresult = Curl_multi_add_perform(data->multi, newhandle, cf->conn);
    ଯଦି(mresult) {
      infof(data, "failed to add handle to multi");
      discard_newhandle(cf, newhandle);
      rv = CURL_ଠେଲ_DENY;
      ଯାଅ fail;
    }

    result = http2_data_setup(cf, newhandle, &newstream);
    ଯଦି(result) {
      failf(data, "error setting up stream: %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
      discard_newhandle(cf, newhandle);
      rv = CURL_ଠେଲ_DENY;
      ଯାଅ fail;
    }

    DEBUGASSERT(newstream);
    newstream->id = frame->promised_stream_id;
    newhandle->req.maxdownload = -1;
    newhandle->req.size = -1;

    CURL_TRC_CF(data, cf, "promise easy handle added to multi, mid=%u",
                newhandle->mid);
    rv = Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, newstream->id, newstream);
    ଯଦି(rv) {
      infof(data, "failed to set user_data for stream %d",
            newstream->id);
      DEBUGASSERT(0);
      discard_newhandle(cf, newhandle);
      rv = CURL_ଠେଲ_DENY;
      ଯାଅ fail;
    }

    /* success, remember max stream id processed */
    ଯଦି(newstream->id > ctx->local_max_sid)
      ctx->local_max_sid = newstream->id;
  }
  ଅନ୍ୟଥା {
    CURL_TRC_CF(data, cf, "Got PUSH_PROMISE, ignore it");
    rv = CURL_ଠେଲ_DENY;
  }
fail:
  ଫେରନ୍ତୁ rv;
}

ସ୍ଥିର ଶୂନ୍ୟ h2_xfer_write_resp_hd(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା h2_stream_ctx *stream,
                                  ଅଚଳ ଅକ୍ଷର *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  ଯଦି(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, blen, eos);
    ଯଦି(!stream->xfer_result && !eos)
      stream->xfer_result = cf_h2_update_local_win(cf, data, stream);
    ଯଦି(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%d] error %d writing %zu bytes of headers",
                  stream->id, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)stream->xfer_result, blen);
  }
}

ସ୍ଥିର ଶୂନ୍ୟ h2_xfer_write_resp(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା h2_stream_ctx *stream,
                               ଅଚଳ ଅକ୍ଷର *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  ଯଦି(!stream->xfer_result)
    stream->xfer_result = Curl_xfer_write_resp(data, buf, blen, eos);
  /* If the transfer write is errored, we do not want any more data */
  ଯଦି(stream->xfer_result) {
    ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
    CURL_TRC_CF(data, cf, "[%d] error %d writing %zu bytes of data, "
                "RST-ing stream",
                stream->id, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)stream->xfer_result, blen);
    Curl_ngh2_submit_rst_stream(
      &ctx->ngh2, 0, stream->id, (uint32_t)NGHTTP2_ERR_CALLBACK_FAILURE);
  }
  ଅନ୍ୟଥା ଯଦି(!stream->write_paused && Curl_xfer_write_is_paused(data)) {
    CURL_TRC_CF(data, cf, "[%d] stream output paused", stream->id);
    stream->write_paused = TRUE;
  }
  ଅନ୍ୟଥା ଯଦି(stream->write_paused && !Curl_xfer_write_is_paused(data)) {
    CURL_TRC_CF(data, cf, "[%d] stream output unpaused", stream->id);
    stream->write_paused = FALSE;
  }

  ଯଦି(!stream->xfer_result && !eos)
    stream->xfer_result = cf_h2_update_local_win(cf, data, stream);
}

ସ୍ଥିର CURLcode on_stream_frame(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା h2_stream_ctx *stream,
                                ଅଚଳ nghttp2_frame *frame)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  int32_t stream_id = frame->hd.stream_id;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv;

  ଯଦି(!stream) {
    CURL_TRC_CF(data, cf, "[%d] No stream_ctx set", stream_id);
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }

  ବାଛ(frame->hd.type) {
  ସ୍ଥିତି NGHTTP2_DATA:
    CURL_TRC_CF(data, cf, "[%d] DATA, window=%d/%d",
                stream_id,
                Curl_ngh2_session_get_stream_effective_recv_data_length(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_stream_effective_local_window_size(
                  &ctx->ngh2, stream->id));
    /* If !body started on this stream, then receiving DATA is illegal. */
    ଯଦି(!stream->bodystarted) {
      ଯଦି(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                       stream_id, NGHTTP2_PROTOCOL_ERROR))) {
        ଫେରନ୍ତୁ CURLE_RECV_ERROR;
      }
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି NGHTTP2_HEADERS:
    ଯଦି(stream->bodystarted) {
      /* Only valid HEADERS after body started is trailer HEADERS. We
         buffer them in on_header callback. */
      ଭାଙ୍ଗ;
    }

    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    ଯଦି(stream->status_code == -1)
      ଫେରନ୍ତୁ CURLE_RECV_ERROR;

    /* Only final status code signals the end of header */
    ଯଦି(stream->status_code / 100 != 1) {
      stream->resp_hds_complete = TRUE;
      stream->bodystarted = TRUE;
    }
    ଅନ୍ୟଥା
      stream->status_code = -1;

    h2_xfer_write_resp_hd(cf, data, stream, STRCONST("\r\n"),
                          (bool)stream->closed);

    Curl_multi_mark_dirty(data);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି NGHTTP2_ଠେଲ_PROMISE:
    rv = ଠେଲ_promise(cf, data, &frame->ଠେଲ_promise);
    ଯଦି(rv) { /* deny! */
      DEBUGASSERT((rv > CURL_ଠେଲ_OK) && (rv <= CURL_ଠେଲ_ERROROUT));
      ଯଦି(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
             frame->ଠେଲ_promise.promised_stream_id, NGHTTP2_CANCEL)))
        ଫେରନ୍ତୁ CURLE_SEND_ERROR;
      ଅନ୍ୟଥା ଯଦି(rv == CURL_ଠେଲ_ERROROUT) {
        /* fail the parent transfer, not the whole session */
        CURL_TRC_CF(data, cf, "[%d] fail in PUSH_PROMISE received",
                    stream_id);
        ଯଦି(!stream->xfer_result)
          stream->xfer_result = CURLE_RECV_ERROR;
        Curl_multi_mark_dirty(data);
      }
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି NGHTTP2_RST_STREAM:
    ଯଦି(frame->rst_stream.error_code)
      stream->reset_by_server = TRUE;
    Curl_multi_mark_dirty(data);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି NGHTTP2_WINDOW_UPDATE:
    ଯଦି(CURL_REQ_WANT_SEND(data) && Curl_bufq_is_empty(&stream->sendbuf)) {
      /* need more data, force processing of transfer */
      Curl_multi_mark_dirty(data);
    }
    ଅନ୍ୟଥା ଯଦି(!Curl_bufq_is_empty(&stream->sendbuf)) {
      /* resume the potentially suspended stream */
      ଯଦି(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id)))
        ଫେରନ୍ତୁ CURLE_SEND_ERROR;
    }
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }

  ଯଦି(frame->hd.flags & NGHTTP2_FLAG_END_STREAM) {
    ଯଦି(!stream->closed && !stream->body_eos &&
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
  ଫେରନ୍ତୁ CURLE_OK;
}

#ifdef CURLVERBOSE
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_nghttp2_fr_print(ଅଚଳ nghttp2_frame *frame, ଅକ୍ଷର *buffer,
                          size_t blen)
{
  ବାଛ(frame->hd.type) {
  ସ୍ଥିତି NGHTTP2_DATA: {
    ଫେରନ୍ତୁ curl_msnprintf(buffer, blen,
                          "FRAME[DATA, len=%d, eos=%d, padlen=%d]",
                          (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_STREAM),
                          (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)frame->data.padlen);
  }
  ସ୍ଥିତି NGHTTP2_HEADERS: {
    ଫେରନ୍ତୁ curl_msnprintf(buffer, blen,
                          "FRAME[HEADERS, len=%d, hend=%d, eos=%d]",
                          (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_HEADERS),
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_STREAM));
  }
  ସ୍ଥିତି NGHTTP2_PRIORITY: {
    ଫେରନ୍ତୁ curl_msnprintf(buffer, blen,
                          "FRAME[PRIORITY, len=%d, flags=%d]",
                          (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)frame->hd.length, frame->hd.flags);
  }
  ସ୍ଥିତି NGHTTP2_RST_STREAM: {
    ଫେରନ୍ତୁ curl_msnprintf(buffer, blen,
                          "FRAME[RST_STREAM, len=%d, flags=%d, error=%u]",
                          (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)frame->hd.length, frame->hd.flags,
                          frame->rst_stream.error_code);
  }
  ସ୍ଥିତି NGHTTP2_SETTINGS: {
    ଯଦି(frame->hd.flags & NGHTTP2_FLAG_ACK) {
      ଫେରନ୍ତୁ curl_msnprintf(buffer, blen, "FRAME[SETTINGS, ack=1]");
    }
    ଫେରନ୍ତୁ curl_msnprintf(buffer, blen,
                          "FRAME[SETTINGS, len=%d]", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)frame->hd.length);
  }
  ସ୍ଥିତି NGHTTP2_ଠେଲ_PROMISE:
    ଫେରନ୍ତୁ curl_msnprintf(buffer, blen,
                          "FRAME[PUSH_PROMISE, len=%d, hend=%d]",
                          (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_HEADERS));
  ସ୍ଥିତି NGHTTP2_PING:
    ଫେରନ୍ତୁ curl_msnprintf(buffer, blen,
                          "FRAME[PING, len=%d, ack=%d]",
                          (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)frame->hd.length,
                          frame->hd.flags & NGHTTP2_FLAG_ACK);
  ସ୍ଥିତି NGHTTP2_GOAWAY: {
    ଅକ୍ଷର scratch[128];
    size_t s_len = CURL_ARRAYSIZE(scratch);
    size_t len = (frame->goaway.opaque_data_len < s_len) ?
      frame->goaway.opaque_data_len : s_len - 1;
    ଯଦି(len)
      memcpy(scratch, frame->goaway.opaque_data, len);
    scratch[len] = '\0';
    ଫେରନ୍ତୁ curl_msnprintf(buffer, blen,
                          "FRAME[GOAWAY, error=%u, reason='%s', "
                          "last_stream=%d]", frame->goaway.error_code,
                          scratch, frame->goaway.last_stream_id);
  }
  ସ୍ଥିତି NGHTTP2_WINDOW_UPDATE: {
    ଫେରନ୍ତୁ curl_msnprintf(buffer, blen,
                          "FRAME[WINDOW_UPDATE, incr=%d]",
                          frame->window_update.window_size_increment);
  }
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ curl_msnprintf(buffer, blen, "FRAME[%d, len=%d, flags=%d]",
                          frame->hd.type, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)frame->hd.length,
                          frame->hd.flags);
  }
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା on_frame_send(nghttp2_session *session, ଅଚଳ nghttp2_frame *frame,
                         ଶୂନ୍ୟ *userp)
{
  ସଂରଚନା Curl_cfilter *cf = userp;
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);

  (ଶୂନ୍ୟ)session;
  DEBUGASSERT(data);
  ଯଦି(Curl_trc_cf_is_verbose(cf, data)) {
    ଅକ୍ଷର buffer[256];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;
    len = Curl_nghttp2_fr_print(frame, buffer, ଆକାର(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] -> %s", frame->hd.stream_id, buffer);
  }
  ଯଦି((frame->hd.type == NGHTTP2_GOAWAY) && !ctx->sent_goaway) {
    /* A GOAWAY not initiated by us, but by nghttp2 itself on detecting
     * a protocol error on the connection */
    failf(data, "nghttp2 shuts down connection with error %u: %s",
          frame->goaway.error_code,
          nghttp2_http2_strerror(frame->goaway.error_code));
  }
  ଫେରନ୍ତୁ 0;
}
#endif /* CURLVERBOSE */

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା on_frame_recv(nghttp2_session *session, ଅଚଳ nghttp2_frame *frame,
                         ଶୂନ୍ୟ *userp)
{
  ସଂରଚନା Curl_cfilter *cf = userp;
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf), *data_s;
  ସଂରଚନା h2_stream_ctx *stream;
  int32_t stream_id = frame->hd.stream_id;

  (ଶୂନ୍ୟ)session;
  DEBUGASSERT(data);
#ifdef CURLVERBOSE
  ଯଦି(Curl_trc_cf_is_verbose(cf, data)) {
    ଅକ୍ଷର buffer[256];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;
    len = Curl_nghttp2_fr_print(frame, buffer, ଆକାର(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] <- %s", frame->hd.stream_id, buffer);
  }
#endif /* CURLVERBOSE */

  ଯଦି(!stream_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(data);
    ବାଛ(frame->hd.type) {
    ସ୍ଥିତି NGHTTP2_SETTINGS: {
      ଯଦି(!(frame->hd.flags & NGHTTP2_FLAG_ACK)) {
        uint32_t max_conn = ctx->max_concurrent_streams;
        ctx->max_concurrent_streams =
          Curl_ngh2_session_get_remote_settings(
            &ctx->ngh2, NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS);
        ctx->enable_ଠେଲ = Curl_ngh2_session_get_remote_settings(
          &ctx->ngh2, NGHTTP2_SETTINGS_ENABLE_ଠେଲ) != 0;
        CURL_TRC_CF(data, cf, "[0] MAX_CONCURRENT_STREAMS: %u",
                    ctx->max_concurrent_streams);
        CURL_TRC_CF(data, cf, "[0] ENABLE_PUSH: %s",
                    ctx->enable_ଠେଲ ? "TRUE" : "false");
        ଯଦି(data && max_conn != ctx->max_concurrent_streams) {
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
        ଯଦି(CURL_REQ_WANT_SEND(data))
          Curl_multi_mark_dirty(data);
      }
      ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->goaway_error = frame->goaway.error_code;
      ctx->remote_max_sid = frame->goaway.last_stream_id;
      ଯଦି(data) {
        infof(data, "received GOAWAY, error=%u, last_stream=%d",
              ctx->goaway_error, ctx->remote_max_sid);
        Curl_multi_connchanged(data->multi);
      }
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ 0;
  }

  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  ଯଦି(!data_s) {
    CURL_TRC_CF(data, cf, "[%d] No Curl_easy associated", stream_id);
    ଫେରନ୍ତୁ 0;
  }

  ଫେରନ୍ତୁ on_stream_frame(cf, data_s, stream, frame) ?
         NGHTTP2_ERR_CALLBACK_FAILURE : 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cf_h2_on_invalid_frame_recv(nghttp2_session *session,
                                       ଅଚଳ nghttp2_frame *frame,
                                       ପୂର୍ଣ୍ଣସଂଖ୍ୟା ngerr, ଶୂନ୍ୟ *userp)
{
  ସଂରଚନା Curl_cfilter *cf = userp;
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  ସଂରଚନା Curl_easy *data;
  ସଂରଚନା h2_stream_ctx *stream;
  int32_t stream_id = frame->hd.stream_id;

  (ଶୂନ୍ୟ)session;
  data = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  ଯଦି(data) {
#ifdef CURLVERBOSE
    ଅକ୍ଷର buffer[256];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;
    len = Curl_nghttp2_fr_print(frame, buffer, ଆକାର(buffer) - 1);
    buffer[len] = 0;
    failf(data, "[HTTP2] [%d] received invalid frame: %s, error %d: %s",
          stream_id, buffer, ngerr, nghttp2_strerror(ngerr));
#endif /* CURLVERBOSE */
    ଯଦି(stream) {
      Curl_ngh2_submit_rst_stream(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, NGHTTP2_STREAM_CLOSED);
      stream->error = ngerr;
      stream->closed = TRUE;
      stream->reset = TRUE;
    }
  }
  /* Return no error, nghttp2 will RST/GOAWAY by itself when needed */
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା on_data_chunk_recv(nghttp2_session *session, uint8_t flags,
                              int32_t stream_id,
                              ଅଚଳ uint8_t *mem, size_t len, ଶୂନ୍ୟ *userp)
{
  ସଂରଚନା Curl_cfilter *cf = userp;
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  ସଂରଚନା h2_stream_ctx *stream;
  ସଂରଚନା Curl_easy *data_s;

  (ଶୂନ୍ୟ)session;
  (ଶୂନ୍ୟ)flags;

  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */
  DEBUGASSERT(CF_DATA_CURRENT(cf));

  /* get the stream from the hash based on Stream ID */
  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  ଯଦି(!data_s) {
    /* Receiving a Stream ID not in the hash should not happen - unless
       we have aborted a transfer artificially and there were more data
       in the pipeline. Silently ignore. */
    CURL_TRC_CF(CF_DATA_CURRENT(cf), cf, "[%d] Data for unknown", stream_id);
    /* consumed explicitly as no one will read it */
    Curl_ngh2_session_consume(&ctx->ngh2, stream_id, len);
    ଫେରନ୍ତୁ 0;
  }

  ଯଦି(!stream)
    ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;

  h2_xfer_write_resp(cf, data_s, stream, (ଅଚଳ ଅକ୍ଷର *)mem, len, FALSE);

  Curl_ngh2_session_consume(&ctx->ngh2, stream_id, len);
  stream->nrcvd_data += (curl_off_t)len;
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା on_stream_close(nghttp2_session *session, int32_t stream_id,
                           uint32_t error_code, ଶୂନ୍ୟ *userp)
{
  ସଂରଚନା Curl_cfilter *cf = userp;
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  ସଂରଚନା Curl_easy *data_s, *call_data = CF_DATA_CURRENT(cf);
  ସଂରଚନା h2_stream_ctx *stream = ଖାଲି;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv;
  (ଶୂନ୍ୟ)session;

  DEBUGASSERT(call_data);
  /* stream id 0 is the connection, do not look there for streams. */
  data_s = stream_id ? cf_h2_get_stream_xfer(cf, stream_id, &stream) : ଖାଲି;
  ଯଦି(!data_s) {
    CURL_TRC_CF(call_data, cf,
                "[%d] on_stream_close, no easy set on stream", stream_id);
    ଫେରନ୍ତୁ 0;
  }
  ଯଦି(!GOOD_EASY_HANDLE(data_s)) {
    /* nghttp2 still has an easy registered for the stream which has
     * been freed be libcurl. This points to a code path that does not
     * trigger DONE or DETACH events as it must. */
    CURL_TRC_CF(call_data, cf,
                "[%d] on_stream_close, not a GOOD easy on stream", stream_id);
    (ଶୂନ୍ୟ)Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, stream_id, ଖାଲି);
    ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  ଯଦି(!stream) {
    CURL_TRC_CF(data_s, cf,
                "[%d] on_stream_close, GOOD easy but no stream", stream_id);
    ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  stream->closed = TRUE;
  stream->error = error_code;
  ଯଦି(stream->error) {
    stream->reset = TRUE;
    ଯଦି((stream->error == NGHTTP2_REFUSED_STREAM) &&
       ctx->rcvd_goaway && ctx->goaway_error)
    /* REFUSED_STREAM is used by nghttp2 on a GOAWAY from the
     * server where it indicated it will no longer process this stream.
     * If the GOAWAY carried an error code, take that as the error for
     * the stream. */
    stream->error = ctx->goaway_error;
  }

  ଯଦି(stream->error)
    CURL_TRC_CF(data_s, cf, "[%d] RESET: %s (err %u)",
              stream_id, nghttp2_http2_strerror(error_code), error_code);
  ଅନ୍ୟଥା
    CURL_TRC_CF(data_s, cf, "[%d] CLOSED", stream_id);
  Curl_multi_mark_dirty(data_s);

  /* remove `stream_ctx` from the nghttp2 stream */
  rv = Curl_ngh2_session_set_stream_user_data(
    &ctx->ngh2, stream_id, ଖାଲି);
  ଯଦି(rv) {
    infof(data_s, "http/2: failed to clear user_data for stream %d",
          stream_id);
    DEBUGASSERT(0);
  }
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_h2_header_error(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା h2_stream_ctx *stream,
                               CURLcode result)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;

  failf(data, "Error receiving HTTP2 header: %d(%s)", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result,
        curl_easy_strerror(result));
  ଯଦି(stream) {
    Curl_ngh2_submit_rst_stream(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, NGHTTP2_STREAM_CLOSED);
    stream->closed = TRUE;
    stream->reset = TRUE;
  }
}

/* frame->hd.type is either NGHTTP2_HEADERS or NGHTTP2_PUSH_PROMISE */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା on_header(nghttp2_session *session, ଅଚଳ nghttp2_frame *frame,
                     ଅଚଳ uint8_t *name, size_t namelen,
                     ଅଚଳ uint8_t *ମୂଲ୍ୟ, size_t valuelen,
                     uint8_t flags,
                     ଶୂନ୍ୟ *userp)
{
  ସଂରଚନା Curl_cfilter *cf = userp;
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  ସଂରଚନା h2_stream_ctx *stream;
  ସଂରଚନା Curl_easy *data;
  int32_t stream_id = frame->hd.stream_id;
  CURLcode result;

  (ଶୂନ୍ୟ)session;
  (ଶୂନ୍ୟ)flags;

  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */

  /* get the stream from the hash based on Stream ID */
  data = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  ଯଦି(!GOOD_EASY_HANDLE(data))
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;

  ଯଦି(!stream) {
    failf(data, "Internal NULL stream");
    ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  /* Store received PUSH_PROMISE headers to be used when the subsequent
     PUSH_PROMISE callback comes */
  ଯଦି(frame->hd.type == NGHTTP2_ଠେଲ_PROMISE) {
    ଅକ୍ଷର *h;

    ଯଦି((namelen == CURL_CSTRLEN(HTTP_PSEUDO_AUTHORITY)) &&
       !strncmp(HTTP_PSEUDO_AUTHORITY, (ଅଚଳ ଅକ୍ଷର *)name, namelen)) {
      /* pseudo headers are lower case */
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;
      ଅକ୍ଷର *check = curl_maprintf("%s:%d", data->state.origin->hostname,
                                  data->state.origin->port);
      ଯଦି(!check)
        /* no memory */
        ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;
      ଯଦି(!curl_strequal(check, (ଅଚଳ ଅକ୍ଷର *)ମୂଲ୍ୟ) &&
         ((data->state.origin->port != data->state.origin->scheme->defport) ||
          !curl_strequal(data->state.origin->hostname, (ଅଚଳ ଅକ୍ଷର *)ମୂଲ୍ୟ))) {
        /* This is push is not for the same authority that was asked for in
         * the URL. RFC 7540 section 8.2 says: "A client MUST treat a
         * PUSH_PROMISE for which the server is not authoritative as a stream
         * error of type PROTOCOL_ERROR."
         */
        (ଶୂନ୍ୟ)Curl_ngh2_submit_rst_stream(
          &ctx->ngh2, NGHTTP2_FLAG_NONE, stream_id, NGHTTP2_PROTOCOL_ERROR);
        rc = NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      curlx_free(check);
      ଯଦି(rc)
        ଫେରନ୍ତୁ rc;
    }

    ଯଦି(!stream->ଠେଲ_headers) {
      stream->ଠେଲ_headers_alloc = 10;
      stream->ଠେଲ_headers = curlx_malloc(stream->ଠେଲ_headers_alloc *
                                          ଆକାର(ଅକ୍ଷର *));
      ଯଦି(!stream->ଠେଲ_headers)
        ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;
      stream->ଠେଲ_headers_used = 0;
    }
    ଅନ୍ୟଥା ଯଦି(stream->ଠେଲ_headers_used ==
            stream->ଠେଲ_headers_alloc) {
      ଅକ୍ଷର **headp;
      ଯଦି(stream->ଠେଲ_headers_alloc > 1000) {
        /* this is beyond crazy many headers, bail out */
        failf(data, "Too many PUSH_PROMISE headers");
        free_ଠେଲ_headers(stream);
        ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      stream->ଠେଲ_headers_alloc *= 2;
      headp = curlx_realloc(stream->ଠେଲ_headers,
                            stream->ଠେଲ_headers_alloc * ଆକାର(ଅକ୍ଷର *));
      ଯଦି(!headp) {
        free_ଠେଲ_headers(stream);
        ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      stream->ଠେଲ_headers = headp;
    }
    h = curl_maprintf("%s:%s", name, ମୂଲ୍ୟ);
    ଯଦି(!h) {
      free_ଠେଲ_headers(stream);
      ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    stream->ଠେଲ_headers[stream->ଠେଲ_headers_used++] = h;
    ଫେରନ୍ତୁ 0;
  }

  ଯଦି(stream->bodystarted) {
    /* This is a trailer */
    CURL_TRC_CF(data, cf, "[%d] trailer: %.*s: %.*s",
                stream->id, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)namelen, name, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)valuelen, ମୂଲ୍ୟ);
    result = Curl_dynhds_add(&stream->resp_trailers,
                             (ଅଚଳ ଅକ୍ଷର *)name, namelen,
                             (ଅଚଳ ଅକ୍ଷର *)ମୂଲ୍ୟ, valuelen);
    ଯଦି(result) {
      cf_h2_header_error(cf, data, stream, result);
      ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;
    }

    ଫେରନ୍ତୁ 0;
  }

  ଯଦି(namelen == CURL_CSTRLEN(HTTP_PSEUDO_STATUS) &&
     !memcmp(HTTP_PSEUDO_STATUS, name, namelen)) {
    /* nghttp2 guarantees :status is received first and only once. */
    ଅକ୍ଷର buffer[32];
    size_t hlen;
    result = Curl_http_decode_status(&stream->status_code,
                                     (ଅଚଳ ଅକ୍ଷର *)ମୂଲ୍ୟ, valuelen);
    ଯଦି(result) {
      cf_h2_header_error(cf, data, stream, result);
      ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    hlen = curl_msnprintf(buffer, ଆକାର(buffer), HTTP_PSEUDO_STATUS ":%d\r",
                          stream->status_code);
    result = Curl_headers_ଠେଲ(data, buffer, hlen, CURLH_PSEUDO);
    ଯଦି(result) {
      cf_h2_header_error(cf, data, stream, result);
      ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    curlx_dyn_reset(&ctx->scratch);
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/2 "));
    ଯଦି(!result)
      result = curlx_dyn_addn(&ctx->scratch, ମୂଲ୍ୟ, valuelen);
    ଯଦି(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    ଯଦି(!result)
      h2_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    ଯଦି(result) {
      cf_h2_header_error(cf, data, stream, result);
      ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    /* if we receive data for another handle, wake that up */
    ଯଦି(CF_DATA_CURRENT(cf) != data)
      Curl_multi_mark_dirty(data);

    CURL_TRC_CF(data, cf, "[%d] status: HTTP/2 %03d",
                stream->id, stream->status_code);
    ଫେରନ୍ତୁ 0;
  }

  /* nghttp2 guarantees that namelen > 0, and :status was already
     received, and this is not pseudo-header field . */
  /* convert to an HTTP1-style header */
  curlx_dyn_reset(&ctx->scratch);
  result = curlx_dyn_addn(&ctx->scratch, (ଅଚଳ ଅକ୍ଷର *)name, namelen);
  ଯଦି(!result)
    result = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
  ଯଦି(!result)
    result = curlx_dyn_addn(&ctx->scratch, (ଅଚଳ ଅକ୍ଷର *)ମୂଲ୍ୟ, valuelen);
  ଯଦି(!result)
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
  ଯଦି(!result)
    h2_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                          curlx_dyn_len(&ctx->scratch), FALSE);
  ଯଦି(result) {
    cf_h2_header_error(cf, data, stream, result);
    ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  /* if we receive data for another handle, wake that up */
  ଯଦି(CF_DATA_CURRENT(cf) != data)
    Curl_multi_mark_dirty(data);

  CURL_TRC_CF(data, cf, "[%d] header: %.*s: %.*s",
              stream->id, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)namelen, name, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)valuelen, ମୂଲ୍ୟ);

  ଫେରନ୍ତୁ 0; /* 0 is successful */
}

ସ୍ଥିର ssize_t req_body_read_callback(nghttp2_session *session,
                                      int32_t stream_id,
                                      uint8_t *buf, size_t length,
                                      uint32_t *data_flags,
                                      nghttp2_data_source *source,
                                      ଶୂନ୍ୟ *userp)
{
  ସଂରଚନା Curl_cfilter *cf = userp;
  ସଂରଚନା Curl_easy *data_s;
  ସଂରଚନା h2_stream_ctx *stream = ଖାଲି;
  CURLcode result;
  ssize_t nread;
  size_t n;
  (ଶୂନ୍ୟ)source;

  (ଶୂନ୍ୟ)session;
  ଯଦି(!stream_id)
    ଫେରନ୍ତୁ NGHTTP2_ERR_INVALID_ARGUMENT;

  /* get the stream from the hash based on Stream ID, stream ID zero is for
     connection-oriented stuff */
  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  ଯଦି(!data_s || !stream)
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_bufq_read(&stream->sendbuf, buf, length, &n);
  ଯଦି(result) {
    ଯଦି(result != CURLE_AGAIN)
      ଫେରନ୍ତୁ NGHTTP2_ERR_CALLBACK_FAILURE;
    nread = 0;
  }
  ଅନ୍ୟଥା
    nread = (ssize_t)n;

  CURL_TRC_CF(data_s, cf, "[%d] req_body_read(len=%zu) eos=%d -> %zd, %d",
              stream_id, length, stream->body_eos, nread, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);

  ଯଦି(stream->body_eos && Curl_bufq_is_empty(&stream->sendbuf)) {
    *data_flags = NGHTTP2_DATA_FLAG_EOF;
    ଫେରନ୍ତୁ nread;
  }
  ଫେରନ୍ତୁ (nread == 0) ? NGHTTP2_ERR_DEFERRED : nread;
}

#ifdef CURLVERBOSE
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା error_callback(nghttp2_session *session,
                          ଅଚଳ ଅକ୍ଷର *ସନ୍ଦେଶ,
                          size_t len,
                          ଶୂନ୍ୟ *userp)
{
  ସଂରଚନା Curl_cfilter *cf = userp;
  ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);
  (ଶୂନ୍ୟ)session;
  failf(data, "%.*s", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len, ସନ୍ଦେଶ);
  ଫେରନ୍ତୁ 0;
}
#endif

/*
 * Append headers to ask for an HTTP1.1 to HTTP2 upgrade.
 */
CURLcode Curl_http2_request_upgrade(ସଂରଚନା dynbuf *req,
                                    ସଂରଚନା Curl_easy *data)
{
  CURLcode result;
  ଅକ୍ଷର *base64;
  size_t blen;
  ସଂରଚନା SingleRequest *k = &data->req;
  uint8_t binsettings[H2_BINSETTINGS_LEN];
  ssize_t rc;
  size_t binlen; /* length of the binsettings data */

  rc = populate_binsettings(binsettings, data);
  ଯଦି(!curlx_sztouz(rc, &binlen) || !binlen) {
    failf(data, "nghttp2 unexpectedly failed on pack_settings_payload");
    curlx_dyn_free(req);
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }

  result = curlx_base64url_encode(binsettings, binlen, &base64, &blen);
  ଯଦି(result) {
    curlx_dyn_free(req);
    ଫେରନ୍ତୁ result;
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

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode http2_handle_stream_close(ସଂରଚନା Curl_cfilter *cf,
                                          ସଂରଚନା Curl_easy *data,
                                          ସଂରଚନା h2_stream_ctx *stream,
                                          size_t *pnlen)
{
  CURLcode result;

  *pnlen = 0;
  ଯଦି(stream->reset) {
    ଯଦି(stream->error == NGHTTP2_REFUSED_STREAM) {
      infof(data, "HTTP/2 stream %d refused by server, try again on a new "
                  "connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      ଫେରନ୍ତୁ CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    ଅନ୍ୟଥା ଯଦି(stream->resp_hds_complete && data->req.no_body) {
      CURL_TRC_CF(data, cf, "[%d] error after response headers, but we did "
                  "not want a body anyway, ignore: %s (err %u)",
                  stream->id, nghttp2_http2_strerror(stream->error),
                  stream->error);
      stream->close_handled = TRUE;
      ଫେରନ୍ତୁ CURLE_OK;
    }
    failf(data, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          stream->id, stream->reset_by_server ? "server" : "curl",
          stream->error, nghttp2_http2_strerror(stream->error));
    ଫେରନ୍ତୁ stream->error ? CURLE_HTTP2_STREAM :
           (data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2);
  }
  ଅନ୍ୟଥା ଯଦି(!stream->bodystarted) {
    failf(data, "HTTP/2 stream %d was closed cleanly, but before getting "
          "all response header fields, treated as error", stream->id);
    ଫେରନ୍ତୁ CURLE_HTTP2_STREAM;
  }

  ଯଦି(Curl_dynhds_ଗଣନା(&stream->resp_trailers)) {
    ସଂରଚନା dynhds_entry *e;
    ସଂରଚନା dynbuf dbuf;
    size_t i;

    result = CURLE_OK;
    curlx_dyn_init(&dbuf, DYN_TRAILERS);
    ପାଇଁ(i = 0; i < Curl_dynhds_ଗଣନା(&stream->resp_trailers); ++i) {
      e = Curl_dynhds_getn(&stream->resp_trailers, i);
      ଯଦି(!e)
        ଭାଙ୍ଗ;
      curlx_dyn_reset(&dbuf);
      result = curlx_dyn_addf(&dbuf, "%.*s: %.*s\x0d\x0a",
                              (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)e->namelen, e->name,
                              (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)e->valuelen, e->ମୂଲ୍ୟ);
      ଯଦି(result)
        ଭାଙ୍ଗ;
      Curl_debug(data, CURLINFO_HEADER_IN, curlx_dyn_ptr(&dbuf),
                 curlx_dyn_len(&dbuf));
      result = Curl_client_write(data,
                                 CLIENTWRITE_HEADER | CLIENTWRITE_TRAILER,
                                 curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf));
      ଯଦି(result)
        ଭାଙ୍ଗ;
      result = Curl_bump_headersize(data, curlx_dyn_len(&dbuf), FALSE);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
    curlx_dyn_free(&dbuf);
    ଯଦି(result)
      ଯାଅ out;
  }

  stream->close_handled = TRUE;
  result = CURLE_OK;

out:
  CURL_TRC_CF(data, cf, "handle_stream_close -> %d, %zu", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnlen);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sweight_wanted(ଅଚଳ ସଂରଚନା Curl_easy *data)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  ଫେରନ୍ତୁ data->set.weight ?
    data->set.weight : NGHTTP2_DEFAULT_WEIGHT;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sweight_in_effect(ଅଚଳ ସଂରଚନା Curl_easy *data)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  ଫେରନ୍ତୁ data->state.weight ?
    data->state.weight : NGHTTP2_DEFAULT_WEIGHT;
}

/*
 * h2_pri_spec() fills in the pri_spec struct, used by nghttp2 to send weight
 * and dependency to the peer. It also stores the updated values in the state
 * struct.
 */

ସ୍ଥିର ଶୂନ୍ୟ h2_pri_spec(ସଂରଚନା Curl_easy *data,
                        nghttp2_priority_spec *pri_spec)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା prio = data->set.weight;
  nghttp2_priority_spec_init(pri_spec, 0, sweight_wanted(data), FALSE);
  data->state.weight = prio;
}

/*
 * Check if there is been an update in the priority /
 * dependency settings and if so it submits a PRIORITY frame with the updated
 * info.
 * Flush any out data pending in the network buffer.
 */
ସ୍ଥିର CURLcode h2_progress_egress(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv = 0;

  ଯଦି(sweight_wanted(data) != sweight_in_effect(data)) {
    ସଂରଚନା h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    ଯଦି(stream && stream->id > 0 &&
       (sweight_wanted(data) != sweight_in_effect(data))) {
      /* send new weight and/or dependency */
      nghttp2_priority_spec pri_spec;

      h2_pri_spec(data, &pri_spec);
      CURL_TRC_CF(data, cf, "[%d] Queuing PRIORITY", stream->id);
      DEBUGASSERT(stream->id != -1);
      rv = Curl_ngh2_submit_priority(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, &pri_spec);
      ଯଦି(rv)
        ଯାଅ out;
    }
  }

  ctx->nw_out_blocked = 0;
  ଯେତେବେଳେ(!rv && !ctx->nw_out_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

out:
  ଯଦି(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(data, cf, "nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    ଫେରନ୍ତୁ CURLE_SEND_ERROR;
  }
  /* Defer flushing during the connect phase so that the SETTINGS and
   * other initial frames are sent together with the first request.
   * Unless we are 'connect_only' where the request will never come. */
  ଯଦି(!cf->connected && !cf->conn->bits.connect_only)
    ଫେରନ୍ତୁ CURLE_OK;
  ଫେରନ୍ତୁ nw_out_flush(cf, data);
}

ସ୍ଥିର CURLcode stream_recv(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା h2_stream_ctx *stream,
                            ଅକ୍ଷର *buf, size_t len, size_t *pnread)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_AGAIN;

  (ଶୂନ୍ୟ)buf;
  (ଶୂନ୍ୟ)len;
  *pnread = 0;

  ଯଦି(!stream->xfer_result)
    stream->xfer_result = cf_h2_update_local_win(cf, data, stream);

  ଯଦି(stream->xfer_result) {
    CURL_TRC_CF(data, cf, "[%d] xfer write failed", stream->id);
    result = stream->xfer_result;
  }
  ଅନ୍ୟଥା ଯଦି(stream->closed) {
    CURL_TRC_CF(data, cf, "[%d] returning CLOSE", stream->id);
    result = http2_handle_stream_close(cf, data, stream, pnread);
  }
  ଅନ୍ୟଥା ଯଦି(stream->reset ||
          (ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) ||
          (ctx->rcvd_goaway && ctx->remote_max_sid < stream->id)) {
    CURL_TRC_CF(data, cf, "[%d] returning ERR", stream->id);
    result = data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2;
  }

  ଯଦି(result && (result != CURLE_AGAIN))
    CURL_TRC_CF(data, cf, "[%d] stream_recv(len=%zu) -> %d, %zu",
                stream->id, len, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnread);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode h2_progress_ingress(ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data,
                                    size_t data_max_bytes)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  ଯଦି(should_close_session(ctx)) {
    CURL_TRC_CF(data, cf, "[0] ingress: session is closed");
    ଫେରନ୍ତୁ CURLE_HTTP2;
  }

  /* Process network input buffer first */
  ଯଦି(!Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "Process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    result = h2_process_pending_ନିବେଶ(cf, data);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  ଯଦି(!data_max_bytes)
    data_max_bytes = H2_CHUNK_SIZE;

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop due to connection close or us not processing
   * all network input */
  ଯେତେବେଳେ(!ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) {
    ସଂରଚନା h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    ଯଦି(stream && (stream->closed || !data_max_bytes)) {
      /* We would like to abort here and stop processing, so that the transfer
       * loop can handle the data/close here. This may leave data in
       * underlying buffers that will not be consumed. */
      ଯଦି(!cf->next || !cf->next->cft->has_data_pending(cf->next, data))
        Curl_multi_mark_dirty(data);
      ଭାଙ୍ଗ;
    }

    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    ଯଦି(result) {
      ଯଦି(result != CURLE_AGAIN) {
        failf(data, "Failed receiving HTTP2 data: %d(%s)", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result,
              curl_easy_strerror(result));
        ଫେରନ୍ତୁ result;
      }
      ଭାଙ୍ଗ;
    }
    ଅନ୍ୟଥା ଯଦି(nread == 0) {
      CURL_TRC_CF(data, cf, "[0] ingress: connection closed");
      ctx->conn_closed = TRUE;
      ଭାଙ୍ଗ;
    }
    ଅନ୍ୟଥା {
      CURL_TRC_CF(data, cf, "[0] ingress: read %zu bytes", nread);
      data_max_bytes = (data_max_bytes > nread) ? (data_max_bytes - nread) : 0;
    }

    result = h2_process_pending_ନିବେଶ(cf, data);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    CURL_TRC_CF(data, cf, "[0] ingress: nw-in buffered %zu",
                Curl_bufq_len(&ctx->inbufq));
  }

  ଯଦି(ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "server closed %s",
                ctx->rcvd_goaway ? "with GOAWAY" : "abruptly");
    connclose(cf->conn);
  }

  CURL_TRC_CF(data, cf, "[0] ingress: done");
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode cf_h2_recv(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                           ଅକ୍ଷର *buf, size_t len, size_t *pnread)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  ସଂରଚନା h2_stream_ctx *stream;
  CURLcode result, r2;
  ସଂରଚନା cf_call_data save;

  ଯଦି(!data)
    ଫେରନ୍ତୁ CURLE_HTTP2;

  stream = H2_STREAM_CTX(ctx, data);

  *pnread = 0;
  ଯଦି(!stream) {
    /* Abnormal call sequence: either this transfer has never opened a stream
     * (unlikely) or the transfer has been done, cleaned up its resources, but
     * a read() is called anyway. It is not clear what the calling sequence
     * is for such a case. */
    failf(data, "http/2 recv on a transfer never opened "
          "or already cleared, mid=%u", data->mid);
    ଫେରନ୍ତୁ CURLE_HTTP2;
  }

  CF_DATA_SAVE(save, cf, data);

  result = stream_recv(cf, data, stream, buf, len, pnread);
  ଯଦି(result && (result != CURLE_AGAIN))
    ଯାଅ out;

  ଯଦି(result) {
    result = h2_progress_ingress(cf, data, len);
    ଯଦି(result)
      ଯାଅ out;

    result = stream_recv(cf, data, stream, buf, len, pnread);
  }

  ଯଦି(*pnread > 0) {
    /* Now that we transferred this to the upper layer, we report
     * the actual amount of DATA consumed to the H2 session, so
     * that it adjusts stream flow control */
    Curl_ngh2_session_consume(&ctx->ngh2, stream->id, *pnread);
    ଯଦି(stream->closed) {
      CURL_TRC_CF(data, cf, "[%d] DRAIN closed stream", stream->id);
      Curl_multi_mark_dirty(data);
    }
  }

out:
  r2 = h2_progress_egress(cf, data);
  ଯଦି(r2 == CURLE_AGAIN) {
    /* pending data to send, need to be called again. Ideally, we
     * monitor the socket for POLLOUT, but when not SENDING
     * any more, we force processing of the transfer. */
    ଯଦି(!CURL_REQ_WANT_SEND(data))
      Curl_multi_mark_dirty(data);
  }
  ଅନ୍ୟଥା ଯଦି(r2 && (result == CURLE_AGAIN)) {
    /* Do not replace a previous error result NOR a successful result
     * with an egress error. Example: the server might close the
     * connection after sending the complete response with EOS
     * and everything. Further sending may fail, but that MUST
     * not change that we had a successful response. */
    result = r2;
  }

  CURL_TRC_CF(data, cf, "[%d] cf_recv(len=%zu) -> %d, %zu, "
              "window=%d/%d, connection %d/%d",
              stream->id, len, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnread,
              Curl_ngh2_session_get_stream_effective_recv_data_length(
                &ctx->ngh2, stream->id),
              Curl_ngh2_session_get_stream_effective_local_window_size(
                &ctx->ngh2, stream->id),
              Curl_ngh2_session_get_local_window_size(&ctx->ngh2),
              HTTP2_HUGE_WINDOW_SIZE);

  CF_DATA_RESTORE(cf, save);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_h2_body_send(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା h2_stream_ctx *stream,
                                ଅଚଳ ଶୂନ୍ୟ *buf, size_t blen, bool eos,
                                size_t *pnwritten)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  CURLcode result;

  *pnwritten = 0;
  ଯଦି(stream->closed) {
    ଯଦି(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(data, cf, "[%d] discarding data"
                  "on closed stream with response", stream->id);
      ଯଦି(eos)
        stream->body_eos = TRUE;
      *pnwritten = blen;
      ଫେରନ୍ତୁ CURLE_OK;
    }
    /* Server closed before we got a response, this is an error */
    infof(data, "stream %d closed", stream->id);
    ଫେରନ୍ତୁ CURLE_SEND_ERROR;
  }

  result = Curl_bufq_write(&stream->sendbuf, buf, blen, pnwritten);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  ଯଦି(eos && (blen == *pnwritten))
    stream->body_eos = TRUE;

  ଯଦି(eos || !Curl_bufq_is_empty(&stream->sendbuf)) {
    /* resume the potentially suspended stream */
    ଯଦି(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id)))
      ଫେରନ୍ତୁ CURLE_SEND_ERROR;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode h2_submit(ସଂରଚନା h2_stream_ctx **pstream,
                          ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                          ଅଚଳ uint8_t *buf, size_t len,
                          bool eos, size_t *pnwritten)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  ସଂରଚନା h2_stream_ctx *stream = ଖାଲି;
  ସଂରଚନା dynhds h2_headers;
  nghttp2_nv *nva = ଖାଲି;
  ଅଚଳ ଶୂନ୍ୟ *body = ଖାଲି;
  size_t nheader, bodylen;
  nghttp2_data_provider data_prd;
  int32_t stream_id;
  nghttp2_priority_spec pri_spec;
  size_t nwritten;
  CURLcode result = CURLE_OK;
  uint32_t initial_win_size;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_headers, MAX_HTTP_RESP_HEADER_ଗଣନା, DYN_HTTP_REQUEST);

  result = http2_data_setup(cf, data, &stream);
  ଯଦି(result)
    ଯାଅ out;

  result = Curl_h1_req_parse_read(
    &stream->h1, buf, len, ଖାଲି,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : ଖାଲି,
    0, &nwritten);
  ଯଦି(result)
    ଯାଅ out;
  *pnwritten = nwritten;
  ଯଦି(!stream->h1.done) {
    /* need more data */
    ଯାଅ out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  ଯଦି(result)
    ଯାଅ out;
  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nva = Curl_dynhds_to_nva(&h2_headers, &nheader);
  ଯଦି(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }

  h2_pri_spec(data, &pri_spec);
  ଯଦି(!Curl_ngh2_session_check_request_allowed(&ctx->ngh2))
    CURL_TRC_CF(data, cf, "send request NOT allowed (via nghttp2)");

  /* Check the initial windows size of the transfer (rate-limits?) and
   * send an updated settings on changes from previous value. */
  initial_win_size = cf_h2_initial_win_size(data);
  ଯଦି(initial_win_size != ctx->initial_win_size) {
    result = cf_h2_update_settings(ctx, initial_win_size);
    ଯଦି(result)
      ଯାଅ out;
  }

  ବାଛ(data->state.httpreq) {
  ସ୍ଥିତି HTTPREQ_POST:
  ସ୍ଥିତି HTTPREQ_POST_FORM:
  ସ୍ଥିତି HTTPREQ_POST_MIME:
  ସ୍ଥିତି HTTPREQ_PUT:
    data_prd.read_callback = req_body_read_callback;
    data_prd.source.ptr = ଖାଲି;
    stream_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, &data_prd, stream);
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    stream_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, ଖାଲି, stream);
  }

  ଯଦି(stream_id < 0) {
    CURL_TRC_CF(data, cf, "send: nghttp2_submit_request error (%s)%d",
                nghttp2_strerror(stream_id), stream_id);
    result = CURLE_SEND_ERROR;
    ଯାଅ out;
  }

#ifdef CURLVERBOSE
#define MAX_ACC 60000  /* <64KB to account for some overhead */
  ଯଦି(Curl_trc_is_verbose(data)) {
    size_t acc = 0, i;

    infof(data, "[HTTP/2] [%d] OPENED stream for %s",
          stream_id, Curl_bufref_ptr(&data->state.url));
    ପାଇଁ(i = 0; i < nheader; ++i) {
      acc += nva[i].namelen + nva[i].valuelen;

      infof(data, "[HTTP/2] [%d] [%.*s: %.*s]", stream_id,
            (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)nva[i].namelen, nva[i].name,
            (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)nva[i].valuelen, nva[i].ମୂଲ୍ୟ);
    }

    ଯଦି(acc > MAX_ACC) {
      infof(data, "[HTTP/2] Warning: The cumulative length of all "
            "headers exceeds %d bytes and that could cause the "
            "stream to be rejected.", MAX_ACC);
    }
  }
#endif

  stream->id = stream_id;

  body = (ଅଚଳ ଅକ୍ଷର *)buf + *pnwritten;
  bodylen = len - *pnwritten;

  ଯଦି(bodylen || eos) {
    size_t n;
    result = cf_h2_body_send(cf, data, stream, body, bodylen, eos, &n);
    ଯଦି(!result)
      *pnwritten += n;
    ଅନ୍ୟଥା ଯଦି(result == CURLE_AGAIN)
      result = CURLE_OK;
    ଅନ୍ୟଥା {
      result = CURLE_SEND_ERROR;
    }
  }

out:
  CURL_TRC_CF(data, cf, "[%d] submit -> %d, %zu",
              stream ? stream->id : -1, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnwritten);
  curlx_safefree(nva);
  *pstream = stream;
  Curl_dynhds_free(&h2_headers);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_h2_send(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                           ଅଚଳ uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  ସଂରଚନା h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
  ସଂରଚନା cf_call_data save;
  CURLcode result = CURLE_OK, r2;

  CF_DATA_SAVE(save, cf, data);
  *pnwritten = 0;

  ଯଦି(!stream || stream->id == -1) {
    result = h2_submit(&stream, cf, data, buf, len, eos, pnwritten);
    ଯଦି(result)
      ଯାଅ out;
    DEBUGASSERT(stream);
  }
  ଅନ୍ୟଥା ଯଦି(stream->body_eos) {
    /* We already wrote this, but CURLE_AGAIN-ed the call due to not
     * being able to flush stream->sendbuf. Make a 0-length write
     * to trigger flushing again.
     * If this works, we report to have written `len` bytes. */
    size_t n;
    DEBUGASSERT(eos);
    result = cf_h2_body_send(cf, data, stream, buf, 0, eos, &n);
    CURL_TRC_CF(data, cf, "[%d] cf_body_send last CHUNK -> %d, %zu, eos=%d",
                stream->id, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, n, eos);
    ଯଦି(result)
      ଯାଅ out;
    *pnwritten = len;
  }
  ଅନ୍ୟଥା {
    result = cf_h2_body_send(cf, data, stream, buf, len, eos, pnwritten);
    CURL_TRC_CF(data, cf, "[%d] cf_body_send(len=%zu) -> %d, %zu, eos=%d",
                stream->id, len, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnwritten, eos);
  }

  /* Call the nghttp2 send loop and flush to write ALL buffered data,
   * headers and/or request body completely out to the network */
  r2 = h2_progress_egress(cf, data);

  /* if the stream has been closed in egress handling (nghttp2 does that
   * when it does not like the headers, for example */
  ଯଦି(stream && stream->closed && !stream->resp_hds_complete) {
    infof(data, "stream %d closed", stream->id);
    result = CURLE_SEND_ERROR;
    ଯାଅ out;
  }
  ଅନ୍ୟଥା ଯଦି(r2 && (r2 != CURLE_AGAIN)) {
    result = r2;
    ଯାଅ out;
  }

  ଯଦି(should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    ଯଦି(stream && stream->closed) {
      result = http2_handle_stream_close(cf, data, stream, pnwritten);
    }
    ଅନ୍ୟଥା {
      CURL_TRC_CF(data, cf, "send: nothing to do in this session");
      result = CURLE_HTTP2;
    }
  }

out:
  ଯଦି(stream) {
    CURL_TRC_CF(data, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
                "eos=%d, h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                stream->id, len, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnwritten,
                stream->body_eos,
                Curl_ngh2_session_get_stream_remote_window_size(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&stream->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  ଅନ୍ୟଥା {
    CURL_TRC_CF(data, cf, "cf_send(len=%zu) -> %d, %zu, "
                "connection-window=%d, nw_send_buffer(%zu)",
                len, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnwritten,
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_DATA_RESTORE(cf, save);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_h2_flush(ସଂରଚନା Curl_cfilter *cf,
                            ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  ସଂରଚନା h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
  ସଂରଚନା cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  ଯଦି(stream && !Curl_bufq_is_empty(&stream->sendbuf)) {
    /* resume the potentially suspended stream */
    ଯଦି(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id))) {
      result = CURLE_SEND_ERROR;
      ଯାଅ out;
    }
  }

  result = h2_progress_egress(cf, data);

out:
  ଯଦି(stream) {
    CURL_TRC_CF(data, cf, "[%d] flush -> %d, "
                "h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                stream->id, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result,
                Curl_ngh2_session_get_stream_remote_window_size(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&stream->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  ଅନ୍ୟଥା {
    CURL_TRC_CF(data, cf, "flush -> %d, "
                "connection-window=%d, nw_send_buffer(%zu)", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result,
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_DATA_RESTORE(cf, save);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_h2_adjust_pollset(ସଂରଚନା Curl_cfilter *cf,
                                     ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା easy_pollset *ps)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  ସଂରଚନା cf_call_data save;
  curl_socket_t sock;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  ଯଦି(!Curl_ngh2_valid(&ctx->ngh2))
    ଫେରନ୍ତୁ CURLE_OK;

  sock = Curl_conn_cf_get_socket(cf, data);
  Curl_pollset_check(data, ps, sock, &want_recv, &want_send);
  ଯଦି(want_recv || want_send) {
    ସଂରଚନା h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
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
  ଅନ୍ୟଥା ଯଦି(ctx->sent_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_DATA_SAVE(save, cf, data);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CF_DATA_RESTORE(cf, save);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_h2_ctx_open(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  ସଂରଚନା h2_stream_ctx *stream;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  nghttp2_session_callbacks *cbs = ଖାଲି;

  DEBUGASSERT(!ctx->ngh2.s);
  DEBUGASSERT(!ctx->ngh2.fatal_rv);
  DEBUGASSERT(ctx->initialized);

  rc = nghttp2_session_callbacks_new(&cbs);
  ଯଦି(rc) {
    failf(data, "Could not initialize nghttp2 callbacks");
    ଯାଅ out;
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
  ଯଦି(rc) {
    failf(data, "Could not initialize nghttp2");
    ଯାଅ out;
  }
  ctx->max_concurrent_streams = data->multi ?
    Curl_multi_max_concurrent_streams(data->multi) :
    DEFAULT_MAX_CONCURRENT_STREAMS;

  ଯଦି(ctx->via_h1_upgrade) {
    /* HTTP/1.1 Upgrade issued. H2 Settings have already been submitted
     * in the H1 request and we upgrade from there. This stream
     * is opened implicitly as #1. */
    uint8_t binsettings[H2_BINSETTINGS_LEN];
    ssize_t rclen;
    size_t binlen; /* length of the binsettings data */

    rclen = populate_binsettings(binsettings, data);

    ଯଦି(!curlx_sztouz(rclen, &binlen) || !binlen) {
      failf(data, "nghttp2 unexpectedly failed on pack_settings_payload");
      result = CURLE_FAILED_INIT;
      ଯାଅ out;
    }

    result = http2_data_setup(cf, data, &stream);
    ଯଦି(result)
      ଯାଅ out;
    DEBUGASSERT(stream);
    stream->id = 1;
    /* queue SETTINGS frame (again) */
    rc = Curl_ngh2_session_upgrade2(
      &ctx->ngh2, binsettings, binlen, data->state.httpreq == HTTPREQ_HEAD,
      ଖାଲି);
    ଯଦି(rc) {
      failf(data, "nghttp2_session_upgrade2() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      ଯାଅ out;
    }

    rc = Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, stream->id, stream);
    ଯଦି(rc) {
      infof(data, "http/2: failed to set user_data for stream %d",
            stream->id);
      DEBUGASSERT(0);
    }
    CURL_TRC_CF(data, cf, "created session via Upgrade");
  }
  ଅନ୍ୟଥା {
    nghttp2_settings_entry iv[H2_SETTINGS_IV_LEN];
    size_t ivlen;

    ivlen = populate_settings(iv, data, ctx);
    rc = Curl_ngh2_submit_settings(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, iv, ivlen);
    ଯଦି(rc) {
      failf(data, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      ଯାଅ out;
    }
  }

  rc = Curl_ngh2_session_set_local_window_size(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, HTTP2_HUGE_WINDOW_SIZE);
  ଯଦି(rc) {
    failf(data, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    result = CURLE_HTTP2;
    ଯାଅ out;
  }

  /* all set, traffic will be send on connect */
  result = CURLE_OK;
  CURL_TRC_CF(data, cf, "[0] created h2 session%s",
              ctx->via_h1_upgrade ? " (via h1 upgrade)" : "");

out:
  ଯଦି(cbs)
    nghttp2_session_callbacks_del(cbs);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_h2_connect(ସଂରଚନା Curl_cfilter *cf,
                              ସଂରଚନା Curl_easy *data,
                              bool *done)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  ସଂରଚନା cf_call_data save;
  bool first_time = FALSE;

  ଯଦି(cf->connected) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  /* Connect the lower filters first */
  ଯଦି(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    ଯଦି(result || !*done)
      ଫେରନ୍ତୁ result;
  }

  *done = FALSE;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(ctx->initialized);
  ଯଦି(!ctx->ngh2.s) {
    result = cf_h2_ctx_open(cf, data);
    ଯଦି(result)
      ଯାଅ out;
    first_time = TRUE;
  }

  ଯଦି(!first_time) {
    result = h2_progress_ingress(cf, data, 0);
    ଯଦି(result)
      ଯାଅ out;
  }

  /* Send out our SETTINGS and ACKs and such. If that blocks, we
   * have it buffered and can count this filter as being connected */
  result = h2_progress_egress(cf, data);
  ଯଦି(result && (result != CURLE_AGAIN))
    ଯାଅ out;

  *done = TRUE;
  cf->connected = TRUE;
  result = CURLE_OK;

out:
  CURL_TRC_CF(data, cf, "cf_connect() -> %d, %d, ", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *done);
  CF_DATA_RESTORE(cf, save);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_h2_destroy(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;

  (ଶୂନ୍ୟ)data;
  ଯଦି(ctx) {
    cf_h2_ctx_free(ctx);
    cf->ctx = ଖାଲି;
  }
}

ସ୍ଥିର CURLcode cf_h2_shutdown(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data, bool *done)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  ସଂରଚନା cf_call_data save;
  CURLcode result;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv;

  ଯଦି(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);

  ଯଦି(!ctx->sent_goaway) {
    ctx->sent_goaway = TRUE;
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, ctx->local_max_sid, 0,
      (ଅଚଳ uint8_t *)"shutdown", ଆକାର("shutdown"));
    ଯଦି(rv) {
      failf(data, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      result = CURLE_SEND_ERROR;
      ଯାଅ out;
    }
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  result = CURLE_OK;
  ଯଦି(Curl_ngh2_session_want_write(&ctx->ngh2) ||
     !Curl_bufq_is_empty(&ctx->outbufq))
    result = h2_progress_egress(cf, data);
  ଯଦି(!result && Curl_ngh2_session_want_read(&ctx->ngh2))
    result = h2_progress_ingress(cf, data, 0);

  ଯଦି(result == CURLE_AGAIN)
    result = CURLE_OK;

  *done = (ctx->conn_closed ||
           (!result && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2) &&
            Curl_bufq_is_empty(&ctx->outbufq)));

out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode http2_data_pause(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 bool pause)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  ସଂରଚନା h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);

  DEBUGASSERT(data);
  ଯଦି(ctx && Curl_ngh2_valid(&ctx->ngh2) && stream) {
    CURLcode result;

    stream->write_paused = pause;
    result = cf_h2_update_local_win(cf, data, stream);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;

    /* attempt to send the window update */
    (ଶୂନ୍ୟ)h2_progress_egress(cf, data);

    ଯଦି(!pause) {
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
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode cf_h2_cntrl(ସଂରଚନା Curl_cfilter *cf,
                            ସଂରଚନା Curl_easy *data,
                            ପୂର୍ଣ୍ଣସଂଖ୍ୟା event, ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg1, ଶୂନ୍ୟ *arg2)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା cf_call_data save;

  (ଶୂନ୍ୟ)arg2;

  CF_DATA_SAVE(save, cf, data);
  ବାଛ(event) {
  ସ୍ଥିତି CF_CTRL_DATA_SETUP:
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CF_CTRL_DATA_PAUSE:
    result = http2_data_pause(cf, data, (arg1 != 0));
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CF_CTRL_FLUSH:
    result = cf_h2_flush(cf, data);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CF_CTRL_DATA_DONE:
    http2_data_done(cf, data);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CF_CTRL_CONN_INFO_UPDATE:
    ଯଦି(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 20;
      Curl_conn_set_multiplex(cf->conn);
    }
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  CF_DATA_RESTORE(cf, save);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର bool cf_h2_data_pending(ସଂରଚନା Curl_cfilter *cf,
                               ଅଚଳ ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;

  ଯଦି(ctx && !Curl_bufq_is_empty(&ctx->inbufq))
    ଫେରନ୍ତୁ TRUE;
  ଫେରନ୍ତୁ cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

ସ୍ଥିର bool cf_h2_is_alive(ସଂରଚନା Curl_cfilter *cf,
                           ସଂରଚନା Curl_easy *data,
                           bool *ନିବେଶ_pending)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  bool alive;
  ସଂରଚନା cf_call_data save;

  *ନିବେଶ_pending = FALSE;
  CF_DATA_SAVE(save, cf, data);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
           http2_connisalive(cf, data, ନିବେଶ_pending));
  CURL_TRC_CF(data, cf, "conn alive -> %d, input_pending=%d",
              alive, *ନିବେଶ_pending);
  CF_DATA_RESTORE(cf, save);
  ଫେରନ୍ତୁ alive;
}

ସ୍ଥିର CURLcode cf_h2_keep_alive(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data)
{
  CURLcode result;
  ସଂରଚନା cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  result = http2_send_ping(cf, data);
  CF_DATA_RESTORE(cf, save);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_h2_query(ସଂରଚନା Curl_cfilter *cf,
                            ସଂରଚନା Curl_easy *data,
                            ପୂର୍ଣ୍ଣସଂଖ୍ୟା query, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pres1, ଶୂନ୍ୟ *pres2)
{
  ସଂରଚନା cf_h2_ctx *ctx = cf->ctx;
  ସଂରଚନା cf_call_data save;
  size_t effective_max;

  ବାଛ(query) {
  ସ୍ଥିତି CF_QUERY_MAX_CONCURRENT:
    DEBUGASSERT(pres1);

    CF_DATA_SAVE(save, cf, data);
    ଯଦି(!Curl_ngh2_valid(&ctx->ngh2) ||
       !Curl_ngh2_session_check_request_allowed(&ctx->ngh2)) {
      /* the limit is what we have in use right now */
      effective_max = cf->conn->attached_xfers;
    }
    ଅନ୍ୟଥା {
      effective_max = ctx->max_concurrent_streams;
    }
    *pres1 = (effective_max > INT_MAX) ? INT_MAX : (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)effective_max;
    CF_DATA_RESTORE(cf, save);
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି CF_QUERY_STREAM_ERROR: {
    ସଂରଚନା h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    *pres1 = stream ? (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)stream->error : 0;
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ସ୍ଥିତି CF_QUERY_NEED_FLUSH: {
    ସଂରଚନା h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    ଯଦି(!Curl_bufq_is_empty(&ctx->outbufq) ||
       (stream && !Curl_bufq_is_empty(&stream->sendbuf))) {
      *pres1 = TRUE;
      ଫେରନ୍ତୁ CURLE_OK;
    }
    ଭାଙ୍ଗ;
  }
  ସ୍ଥିତି CF_QUERY_HTTP_VERSION:
    *pres1 = 20;
    ଫେରନ୍ତୁ CURLE_OK;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ସଂରଚନା Curl_cftype Curl_cft_nghttp2 = {
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

ସ୍ଥିର CURLcode http2_cfilter_add(ସଂରଚନା Curl_cfilter **pcf,
                                  ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା connectdata *conn,
                                  int8_t sockindex,
                                  bool via_h1_upgrade)
{
  ସଂରଚନା Curl_cfilter *cf = ଖାଲି;
  ସଂରଚନା cf_h2_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(data->conn);
  ctx = curlx_calloc(1, ଆକାର(*ctx));
  ଯଦି(!ctx)
    ଯାଅ out;
  cf_h2_ctx_init(ctx, via_h1_upgrade);

  result = Curl_cf_create(&cf, &Curl_cft_nghttp2, ctx);
  ଯଦି(result)
    ଯାଅ out;

  ctx = ଖାଲି;
  Curl_conn_cf_add(data, conn, sockindex, cf);

out:
  ଯଦି(result)
    cf_h2_ctx_free(ctx);
  *pcf = result ? ଖାଲି : cf;
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode http2_cfilter_ଭର୍ତ୍ତି_after(ସଂରଚନା Curl_cfilter *cf,
                                           ସଂରଚନା Curl_easy *data,
                                           bool via_h1_upgrade)
{
  ସଂରଚନା Curl_cfilter *cf_h2 = ଖାଲି;
  ସଂରଚନା cf_h2_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (ଶୂନ୍ୟ)data;
  ctx = curlx_calloc(1, ଆକାର(*ctx));
  ଯଦି(!ctx)
    ଯାଅ out;
  cf_h2_ctx_init(ctx, via_h1_upgrade);

  result = Curl_cf_create(&cf_h2, &Curl_cft_nghttp2, ctx);
  ଯଦି(result)
    ଯାଅ out;

  ctx = ଖାଲି;
  Curl_conn_cf_ଭର୍ତ୍ତି_after(cf, cf_h2);

out:
  ଯଦି(result)
    cf_h2_ctx_free(ctx);
  ଫେରନ୍ତୁ result;
}

bool Curl_http2_may_switch(ସଂରଚନା Curl_easy *data)
{
  ଯଦି(Curl_conn_http_version(data, data->conn) < 20 &&
     (data->state.http_neg.wanted & CURL_HTTP_V2x) &&
     data->state.http_neg.h2_prior_knowledge) {
#ifndef CURL_DISABLE_PROXY
    ଯଦି(data->conn->bits.origin_is_proxy) {
      infof(data, "Ignoring HTTP/2 prior knowledge due to proxy");
      ଫେରନ୍ତୁ FALSE;
    }
#endif
    ଫେରନ୍ତୁ TRUE;
  }
  ଫେରନ୍ତୁ FALSE;
}

CURLcode Curl_http2_switch(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_cfilter *cf;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, data->conn) < 20);

  result = http2_cfilter_add(&cf, data, data->conn, FIRSTSOCKET, FALSE);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
  CURL_TRC_CF(data, cf, "switching connection to HTTP/2");

  ଯଦି(cf->next) {
    bool done;
    ଫେରନ୍ତୁ Curl_conn_cf_connect(cf, data, &done);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_http2_switch_at(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_cfilter *cf_h2;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, data->conn) < 20);

  result = http2_cfilter_ଭର୍ତ୍ତି_after(cf, data, FALSE);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  cf_h2 = cf->next;

  ଯଦି(cf_h2->next) {
    bool done;
    ଫେରନ୍ତୁ Curl_conn_cf_connect(cf_h2, data, &done);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_http2_upgrade(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା connectdata *conn, int8_t sockindex,
                            ଅଚଳ ଅକ୍ଷର *mem, size_t nread)
{
  ସଂରଚନା Curl_cfilter *cf;
  ସଂରଚନା cf_h2_ctx *ctx;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, conn) < 20);

  result = http2_cfilter_add(&cf, data, conn, sockindex, TRUE);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
  CURL_TRC_CF(data, cf, "upgrading connection to HTTP/2");

  DEBUGASSERT(cf->cft == &Curl_cft_nghttp2);
  ctx = cf->ctx;

  data->req.httpversion_sent = 20; /* it is an h2 request now */
  data->req.header = TRUE; /* we expect the real response to come in h2 */
  data->req.headerline = 0; /* restart the header line counter */

  ଯଦି(nread > 0) {
    /* Remaining data from the protocol switch reply is already using
     * the switched protocol, ie. HTTP/2. We add that to the network
     * inbufq. */
    size_t copied;

    result = Curl_bufq_write(&ctx->inbufq,
                             (ଅଚଳ unsigned ଅକ୍ଷର *)mem, nread, &copied);
    ଯଦି(result) {
      failf(data, "error on copying HTTP Upgrade response: %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
      ଫେରନ୍ତୁ CURLE_RECV_ERROR;
    }
    ଯଦି(copied < nread) {
      failf(data, "connection buffer size could not take all data "
            "from HTTP Upgrade response header: copied=%zu, datalen=%zu",
            copied, nread);
      ଫେରନ୍ତୁ CURLE_HTTP2;
    }
    infof(data, "Copied HTTP/2 data in stream buffer to connection buffer"
          " after upgrade: len=%zu", nread);
  }

  ଯଦି(cf->next) {
    bool done;
    result = Curl_conn_cf_connect(cf, data, &done);
    ଯଦି(!result)
      cf->cft->cntrl(cf, data, CF_CTRL_CONN_INFO_UPDATE, 0, ଖାଲି);
  }
  ଫେରନ୍ତୁ result;
}

/* Only call this function for a transfer that already got an HTTP/2
   CURLE_HTTP2_STREAM error! */
bool Curl_h2_http_1_1_error(ସଂରଚନା Curl_easy *data)
{
  ଯଦି(Curl_conn_http_version(data, data->conn) == 20) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା err = Curl_conn_get_stream_error(data, data->conn, FIRSTSOCKET);
    ଫେରନ୍ତୁ err == NGHTTP2_HTTP_1_1_REQUIRED;
  }
  ଫେରନ୍ତୁ FALSE;
}

ଶୂନ୍ୟ *Curl_nghttp2_malloc(size_t size, ଶୂନ୍ୟ *user_data)
{
  (ଶୂନ୍ୟ)user_data;
  ଫେରନ୍ତୁ Curl_cmalloc(size);
}

ଶୂନ୍ୟ Curl_nghttp2_free(ଶୂନ୍ୟ *ptr, ଶୂନ୍ୟ *user_data)
{
  (ଶୂନ୍ୟ)user_data;
  Curl_cfree(ptr);
}

ଶୂନ୍ୟ *Curl_nghttp2_calloc(size_t nmemb, size_t size, ଶୂନ୍ୟ *user_data)
{
  (ଶୂନ୍ୟ)user_data;
  ଫେରନ୍ତୁ Curl_ccalloc(nmemb, size);
}

ଶୂନ୍ୟ *Curl_nghttp2_realloc(ଶୂନ୍ୟ *ptr, size_t size, ଶୂନ୍ୟ *user_data)
{
  (ଶୂନ୍ୟ)user_data;
  ଫେରନ୍ତୁ Curl_crealloc(ptr, size);
}

#ଅନ୍ୟଥା /* CURL_DISABLE_HTTP || !USE_NGHTTP2 */

/* Satisfy external references even if http2 is not compiled in. */

ଅକ୍ଷର *curl_pushheader_bynum(ସଂରଚନା curl_pushheaders *h, size_t ଅଙ୍କ)
{
  (ଶୂନ୍ୟ)h;
  (ଶୂନ୍ୟ)ଅଙ୍କ;
  ଫେରନ୍ତୁ ଖାଲି;
}

ଅକ୍ଷର *curl_pushheader_byname(ସଂରଚନା curl_pushheaders *h, ଅଚଳ ଅକ୍ଷର *name)
{
  (ଶୂନ୍ୟ)h;
  (ଶୂନ୍ୟ)name;
  ଫେରନ୍ତୁ ଖାଲି;
}

#endif /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
