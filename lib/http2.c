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
#include "curl_setup.h"

#ᮚᮦᮔ᮪ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#include <nghttp2/nghttp2.h>

#include "urldata.h"
#include "bufq.h"
#include "u32_ptrset.h"
#include "http1.h"
#include "http2.h"
#include "http.h"
#include "sendf.h"
#include "curl_trc.h"
#include "select.h"
#include "curlx/base64.h"
#include "multiif.h"
#include "ngh2_shim.h"
#include "progress.h"
#include "url.h"
#include "urlapi-int.h"
#include "cfilters.h"
#include "connect.h"
#include "transfer.h"
#include "bufref.h"
#include "curlx/dynbuf.h"
#include "headers.h"
#include "curl_share.h"

#ᮚᮦᮔ᮪ NGHTTP2_VERSION_NUM < 0x010f00
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
#ᮚᮦᮔ᮪ NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
#define H2_STREAM_WINDOW_SIZE_INITIAL  (64 * 1024)
#ᮜᮤᮚ
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

sa_nracanaa cf_h2_ctx {
  sa_nracanaa curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  sa_nracanaa cf_call_data call_data;

  sa_nracanaa bufq inbufq;           /* network input */
  sa_nracanaa bufq outbufq;          /* network output */
  sa_nracanaa bufc_pool stream_bufcp; /* spares for stream buffers */
  sa_nracanaa dynbuf scratch;        /* scratch buffer for temp use */

  sa_nracanaa u32_ptrset streams; /* hash of `data->mid` to `h2_stream_ctx` */
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
#define CF_CTX_CALL_DATA(cf) ((sa_nracanaa cf_h2_ctx *)(cf)->ctx)->call_data

/**
 * All about the H2 internals of a stream
 */
sa_nracanaa h2_stream_ctx {
  sa_nracanaa bufq sendbuf; /* request buffer */
  sa_nracanaa h1_req_parser h1; /* parsing the request */
  sa_nracanaa dynhds resp_trailers; /* response trailer fields */
  size_t resp_hds_len; /* amount of response header bytes in recvbuf */
  curl_off_t xfer_id;  /* transfer `id` owning this stream */
  curl_off_t nrcvd_data;  /* number of DATA bytes received */

  var_na **push_headers;       /* allocated array */
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

static void free_push_headers(sa_nracanaa h2_stream_ctx *stream)
{
  size_t i;
  for(i = 0; i < stream->push_headers_used; i++)
    curlx_free(stream->push_headers[i]);
  curlx_safefree(stream->push_headers);
  stream->push_headers_used = 0;
}

static void h2_stream_ctx_free(sa_nracanaa h2_stream_ctx *stream)
{
  Curl_bufq_free(&stream->sendbuf);
  Curl_h1_req_parse_free(&stream->h1);
  Curl_dynhds_free(&stream->resp_trailers);
  free_push_headers(stream);
  curlx_free(stream);
}

static void h2_stream_hash_free(uint32_t id, void *stream)
{
  (void)id;
  DEBUGASSERT(stream);
  h2_stream_ctx_free((sa_nracanaa h2_stream_ctx *)stream);
}

static void cf_h2_ctx_init(sa_nracanaa cf_h2_ctx *ctx, bool via_h1_upgrade)
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

static void cf_h2_ctx_free(sa_nracanaa cf_h2_ctx *ctx)
{
  ᮚᮦᮔ᮪(ctx && ctx->initialized) {
    Curl_ngh2_del(&ctx->ngh2);
    Curl_bufq_free(&ctx->inbufq);
    Curl_bufq_free(&ctx->outbufq);
    Curl_bufcp_free(&ctx->stream_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    memset(ctx, 0, parimaa_na(*ctx));
  }
  curlx_free(ctx);
}

static uint32_t cf_h2_initial_win_size(sa_nracanaa Curl_easy *data)
{
#ᮚᮦᮔ᮪ NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
  /* If the transfer has a rate-limit lower than the default initial
   * stream window size, use that. It needs to be at least 8k or servers
   * may be unhappy. */
  curl_off_t rps = Curl_rlimit_per_step(&data->progress.dl.rlimit);
  ᮚᮦᮔ᮪((rps > 0) && (rps < H2_STREAM_WINDOW_SIZE_INITIAL))
    ᮝᮜᮤ CURLMAX((uint32_t)rps, 8192);
#endif
  ᮝᮜᮤ H2_STREAM_WINDOW_SIZE_INITIAL;
}

static size_t populate_settings(nghttp2_settings_entry *iv,
                                sa_nracanaa Curl_easy *data,
                                sa_nracanaa cf_h2_ctx *ctx)
{
  iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
  iv[0].value = Curl_multi_max_concurrent_streams(data->multi);

  iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
  iv[1].value = cf_h2_initial_win_size(data);
  ᮚᮦᮔ᮪(ctx)
    ctx->initial_win_size = iv[1].value;
  iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_PUSH;
  iv[2].value = !!data->multi->push_cb;

  ᮝᮜᮤ 3;
}

static ssize_t populate_binsettings(uint8_t *binsettings,
                                    sa_nracanaa Curl_easy *data)
{
  nghttp2_settings_entry iv[H2_SETTINGS_IV_LEN];
  size_t ivlen;

  ivlen = populate_settings(iv, data, NULL);
  /* this returns number of bytes it wrote or a negative number on error. */
  ᮝᮜᮤ nghttp2_pack_settings_payload(binsettings, H2_BINSETTINGS_LEN,
                                       iv, ivlen);
}

static CURLcode cf_h2_update_settings(sa_nracanaa cf_h2_ctx *ctx,
                                      uint32_t initial_win_size)
{
  nghttp2_settings_entry entry;
  entry.settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
  entry.value = initial_win_size;
  ᮚᮦᮔ᮪(Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, &entry, 1))
    ᮝᮜᮤ CURLE_SEND_ERROR;
  ctx->initial_win_size = initial_win_size;
  ᮝᮜᮤ CURLE_OK;
}

#define H2_STREAM_CTX(ctx, data)                                         \
  ((sa_nracanaa h2_stream_ctx *)(                                             \
    (data) ? Curl_u32_ptrset_get(&(ctx)->streams, (data)->mid) : NULL))

static sa_nracanaa h2_stream_ctx *h2_stream_ctx_create(sa_nracanaa cf_h2_ctx *ctx,
                                                  sa_nracanaa Curl_easy *data)
{
  sa_nracanaa h2_stream_ctx *stream;

  stream = curlx_calloc(1, parimaa_na(*stream));
  ᮚᮦᮔ᮪(!stream)
    ᮝᮜᮤ NULL;

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
  ᮝᮜᮤ stream;
}

static sa_nracanaa Curl_easy *cf_h2_get_stream_xfer(sa_nracanaa Curl_cfilter *cf,
                                               int32_t stream_id,
                                               sa_nracanaa h2_stream_ctx **pstream)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  sa_nracanaa h2_stream_ctx *stream =
    Curl_ngh2_session_get_stream_user_data(&ctx->ngh2, stream_id);
  ᮚᮦᮔ᮪(pstream)
    *pstream = stream;
  ᮚᮦᮔ᮪(stream && (stream->mid != UINT32_MAX)) {
    sa_nracanaa Curl_easy *data, *call_data = CF_DATA_CURRENT(cf);
    ᮚᮦᮔ᮪(!call_data || !call_data->multi) {
      DEBUGASSERT(0);
      ᮝᮜᮤ NULL;
    }
    data = Curl_multi_get_easy(call_data->multi, stream->mid);
    ᮚᮦᮔ᮪(data && (data->id == stream->xfer_id))
      ᮝᮜᮤ data;
  }
  ᮝᮜᮤ NULL;
}

#ifdef NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
static int32_t cf_h2_get_desired_local_win(sa_nracanaa Curl_cfilter *cf,
                                           sa_nracanaa Curl_easy *data)
{
  curl_off_t avail = Curl_rlimit_avail(&data->progress.dl.rlimit, NULL);

  (void)cf;
  ᮚᮦᮔ᮪(avail < CURL_OFF_T_MAX) { /* limit in place */
    ᮚᮦᮔ᮪(avail <= 0)
      ᮝᮜᮤ 0;
    ᮜᮤᮚ ᮚᮦᮔ᮪(avail < H2_STREAM_WINDOW_SIZE_MAX)
      ᮝᮜᮤ (int32_t)avail;
  }
  ᮝᮜᮤ H2_STREAM_WINDOW_SIZE_MAX;
}

static CURLcode cf_h2_update_local_win(sa_nracanaa Curl_cfilter *cf,
                                       sa_nracanaa Curl_easy *data,
                                       sa_nracanaa h2_stream_ctx *stream)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  int32_t dwsize;
  puur_naa_nka rv;

  dwsize = (stream->write_paused || stream->xfer_result) ?
           0 : cf_h2_get_desired_local_win(cf, data);
  ᮚᮦᮔ᮪(dwsize != stream->local_window_size) {
    int32_t wsize = Curl_ngh2_session_get_stream_effective_local_window_size(
      &ctx->ngh2, stream->id);
    ᮚᮦᮔ᮪(dwsize > wsize) {
      rv = Curl_ngh2_session_set_local_window_size(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize);
      ᮚᮦᮔ᮪(rv) {
        failf(data, "[%d] nghttp2 set_local_window_size(%d) failed: "
              "%s(%d)", stream->id, dwsize, nghttp2_strerror(rv), rv);
        ᮝᮜᮤ CURLE_HTTP2;
      }
      rv = Curl_ngh2_submit_window_update(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize - wsize);
      ᮚᮦᮔ᮪(rv) {
        failf(data, "[%d] nghttp2_submit_window_update() failed: "
              "%s(%d)", stream->id, nghttp2_strerror(rv), rv);
        ᮝᮜᮤ CURLE_HTTP2;
      }
      stream->local_window_size = dwsize;
      CURL_TRC_CF(data, cf, "[%d] local window update by %d",
                  stream->id, dwsize - wsize);
    }
    ᮜᮤᮚ {
      rv = Curl_ngh2_session_set_local_window_size(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize);
      ᮚᮦᮔ᮪(rv) {
        failf(data, "[%d] nghttp2_session_set_local_window_size() failed: "
              "%s(%d)", stream->id, nghttp2_strerror(rv), rv);
        ᮝᮜᮤ CURLE_HTTP2;
      }
      stream->local_window_size = dwsize;
      CURL_TRC_CF(data, cf, "[%d] local window size now %d",
                  stream->id, dwsize);
    }
  }
  ᮝᮜᮤ CURLE_OK;
}

#ᮜᮤᮚ /* NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

static CURLcode cf_h2_update_local_win(sa_nracanaa Curl_cfilter *cf,
                                       sa_nracanaa Curl_easy *data,
                                       sa_nracanaa h2_stream_ctx *stream)
{
  (void)cf;
  (void)data;
  (void)stream;
  ᮝᮜᮤ CURLE_OK;
}
#endif /* !NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

static CURLcode http2_data_setup(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 sa_nracanaa h2_stream_ctx **pstream)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  sa_nracanaa h2_stream_ctx *stream;

  (void)cf;
  DEBUGASSERT(data);

  ᮚᮦᮔ᮪(!data)
    ᮝᮜᮤ CURLE_BAD_FUNCTION_ARGUMENT;

  stream = H2_STREAM_CTX(ctx, data);
  ᮚᮦᮔ᮪(stream) {
    *pstream = stream;
    ᮝᮜᮤ CURLE_OK;
  }

  stream = h2_stream_ctx_create(ctx, data);
  ᮚᮦᮔ᮪(!stream)
    ᮝᮜᮤ CURLE_OUT_OF_MEMORY;

  ᮚᮦᮔ᮪(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    h2_stream_ctx_free(stream);
    ᮝᮜᮤ CURLE_OUT_OF_MEMORY;
  }

  *pstream = stream;
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode nw_out_flush(sa_nracanaa Curl_cfilter *cf,
                             sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  ᮚᮦᮔ᮪(Curl_bufq_is_empty(&ctx->outbufq))
    ᮝᮜᮤ CURLE_OK;

  result = Curl_cf_send_bufq(cf->next, data, &ctx->outbufq, NULL, 0,
                             &nwritten);
  ᮚᮦᮔ᮪(result) {
    ᮚᮦᮔ᮪(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_out_blocked = 1;
    }
    ᮝᮜᮤ result;
  }
  ᮝᮜᮤ Curl_bufq_is_empty(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

static void http2_data_done(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  sa_nracanaa h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);

  DEBUGASSERT(ctx);
  ᮚᮦᮔ᮪(!stream || !ctx->initialized)
    ᮝᮜᮤ;

  ᮚᮦᮔ᮪(Curl_ngh2_valid(&ctx->ngh2)) {
    bool flush_egress = FALSE;
    /* returns error if stream not known, which is fine here */
    (void)Curl_ngh2_session_set_stream_user_data(&ctx->ngh2, stream->id, NULL);

    ᮚᮦᮔ᮪(!stream->closed && stream->id > 0) {
      /* RST_STREAM */
      CURL_TRC_CF(data, cf, "[%d] premature DATA_DONE, RST stream",
                  stream->id);
      stream->closed = TRUE;
      stream->reset = TRUE;
      Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                  stream->id, NGHTTP2_STREAM_CLOSED);
      flush_egress = TRUE;
    }

    ᮚᮦᮔ᮪(flush_egress) {
      (void)Curl_ngh2_session_send(&ctx->ngh2);
      (void)nw_out_flush(cf, data);
    }
  }

  Curl_u32_ptrset_unset(&ctx->streams, data->mid);
}

static puur_naa_nka h2_client_new(sa_nracanaa Curl_cfilter *cf,
                         nghttp2_session_callbacks *cbs)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { NULL, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  puur_naa_nka rc = nghttp2_option_new(&o);
  ᮚᮦᮔ᮪(rc)
    ᮝᮜᮤ rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_set_no_auto_window_update(o, 1);
#ᮚᮦᮔ᮪ NGHTTP2_VERSION_NUM >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_set_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#endif
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  ᮝᮜᮤ rc;
}

/*
 * Returns nonzero if current HTTP/2 session should be closed.
 */
static puur_naa_nka should_close_session(sa_nracanaa cf_h2_ctx *ctx)
{
  ᮝᮜᮤ ctx->drain_total == 0 && !Curl_ngh2_session_want_read(&ctx->ngh2) &&
    !Curl_ngh2_session_want_write(&ctx->ngh2);
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
static CURLcode h2_process_pending_input(sa_nracanaa Curl_cfilter *cf,
                                         sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  const unsigned var_na *buf;
  size_t blen, nread;
  ssize_t rv;

  ᮞᮨᮓᮨᮜ(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {
    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (const uint8_t *)buf, blen);
    ᮚᮦᮔ᮪(!curlx_sztouz(rv, &nread)) {
      failf(data, "nghttp2 recv error %zd: %s", rv, nghttp2_strerror((puur_naa_nka)rv));
      ᮝᮜᮤ CURLE_RECV_ERROR;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    ᮚᮦᮔ᮪(Curl_bufq_is_empty(&ctx->inbufq)) {
      bha_nga;
    }
    ᮜᮤᮚ {
      CURL_TRC_CF(data, cf, "process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }

  ᮚᮦᮔ᮪(Curl_ngh2_session_check_request_allowed(&ctx->ngh2) == 0) {
    /* No more requests are allowed in the current session, so
       the connection may not be reused. This is set when a
       GOAWAY frame has been received or when the limit of stream
       identifiers has been reached. */
    CURL_TRC_M(data, "http/2: No new requests allowed");
    connclose(cf->conn);
  }

  ᮝᮜᮤ CURLE_OK;
}

/*
 * The server may send us data at any point (e.g. PING frames). Therefore, we
 * cannot assume that an HTTP/2 socket is dead because it is readable.
 *
 * Check the lower filters first and, if successful, peek at the socket
 * and distinguish between closed and data.
 */
static bool http2_connisalive(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                              bool *input_pending)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *input_pending = FALSE;
  ᮚᮦᮔ᮪(!cf->next || !cf->next->cft->is_alive(cf->next, data, input_pending))
    ᮝᮜᮤ FALSE;

  ᮚᮦᮔ᮪(*input_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode result;
    size_t nread;

    *input_pending = FALSE;
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(data, cf, "connisalive, recv pending input -> %d, %zu",
                (puur_naa_nka)result, nread);
    ᮚᮦᮔ᮪(!result) {
      result = h2_process_pending_input(cf, data);
      ᮚᮦᮔ᮪(result)
        /* immediate error, considered dead */
        alive = FALSE;
      ᮜᮤᮚ {
        alive = !should_close_session(ctx);
      }
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪(result != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  ᮝᮜᮤ alive;
}

static CURLcode http2_send_ping(sa_nracanaa Curl_cfilter *cf,
                                sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  puur_naa_nka rc;

  rc = Curl_ngh2_submit_ping(&ctx->ngh2, 0, ZERO_NULL);
  ᮚᮦᮔ᮪(rc) {
    failf(data, "nghttp2_submit_ping() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    ᮝᮜᮤ CURLE_HTTP2;
  }

  rc = Curl_ngh2_session_send(&ctx->ngh2);
  ᮚᮦᮔ᮪(rc) {
    failf(data, "nghttp2_session_send() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    ᮝᮜᮤ CURLE_SEND_ERROR;
  }
  ᮝᮜᮤ CURLE_OK;
}

/*
 * Store nghttp2 version info in this buffer.
 */
void Curl_http2_ver(var_na *p, size_t len)
{
  nghttp2_info *h2 = nghttp2_version(0);
  (void)curl_msnprintf(p, len, "nghttp2/%s", h2->version_str);
}

/*
 * The implementation of nghttp2_send_callback type. Here we write |data| with
 * size |length| to the network and return the number of bytes actually
 * written. See the documentation of nghttp2_send_callback for the details.
 */
static ssize_t send_callback(nghttp2_session *h2,
                             const uint8_t *buf, size_t blen, puur_naa_nka flags,
                             void *userp)
{
  sa_nracanaa Curl_cfilter *cf = userp;
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  sa_nracanaa Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result = CURLE_OK;

  (void)h2;
  (void)flags;
  DEBUGASSERT(data);

  ᮚᮦᮔ᮪(!cf->connected)
    result = Curl_bufq_write(&ctx->outbufq, buf, blen, &nwritten);
  ᮜᮤᮚ
    result = Curl_cf_send_bufq(cf->next, data, &ctx->outbufq, buf, blen,
                               &nwritten);

  ᮚᮦᮔ᮪(result) {
    ᮚᮦᮔ᮪(result == CURLE_AGAIN) {
      ctx->nw_out_blocked = 1;
      ᮝᮜᮤ NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(data, "Failed sending HTTP2 data");
    ᮝᮜᮤ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  ᮚᮦᮔ᮪(!nwritten) {
    ctx->nw_out_blocked = 1;
    ᮝᮜᮤ NGHTTP2_ERR_WOULDBLOCK;
  }
  ᮝᮜᮤ (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

/* We pass a pointer to this struct in the push callback, but the contents of
   the struct are hidden from the user. */
sa_nracanaa curl_pushheaders {
  sa_nracanaa Curl_easy *data;
  sa_nracanaa h2_stream_ctx *stream;
  const nghttp2_push_promise *frame;
};

/*
 * push header access function. Only to be used from within the push callback
 */
var_na *curl_pushheader_bynum(sa_nracanaa curl_pushheaders *h, size_t num)
{
  /* Verify that we got a good easy handle in the push header struct, mostly to
     detect rubbish input fast(er). */
  ᮚᮦᮔ᮪(!h || !GOOD_EASY_HANDLE(h->data))
    ᮝᮜᮤ NULL;
  ᮜᮤᮚ {
    ᮚᮦᮔ᮪(h->stream && num < h->stream->push_headers_used)
      ᮝᮜᮤ h->stream->push_headers[num];
  }
  ᮝᮜᮤ NULL;
}

/*
 * push header access function. Only to be used from within the push callback
 */
var_na *curl_pushheader_byname(sa_nracanaa curl_pushheaders *h, const var_na *name)
{
  sa_nracanaa h2_stream_ctx *stream;
  size_t len;
  size_t i;
  /* Verify that we got a good easy handle in the push header struct,
     mostly to detect rubbish input fast(er). Also empty header name
     is rubbish too. We have to allow ":" at the beginning of
     the header, but header == ":" must be rejected. If we have ':' in
     the middle of header, it could be matched in middle of the value,
     this is because we do prefix match.*/
  ᮚᮦᮔ᮪(!h || !GOOD_EASY_HANDLE(h->data) || !name || !name[0] ||
     !strcmp(name, ":") || strchr(name + 1, ':'))
    ᮝᮜᮤ NULL;

  stream = h->stream;
  ᮚᮦᮔ᮪(!stream)
    ᮝᮜᮤ NULL;

  len = strlen(name);
  for(i = 0; i < stream->push_headers_used; i++) {
    ᮚᮦᮔ᮪(!strncmp(name, stream->push_headers[i], len)) {
      /* sub-match, make sure that it is followed by a colon */
      ᮚᮦᮔ᮪(stream->push_headers[i][len] != ':')
        continue;
      ᮝᮜᮤ &stream->push_headers[i][len + 1];
    }
  }
  ᮝᮜᮤ NULL;
}

static sa_nracanaa Curl_easy *h2_duphandle(sa_nracanaa Curl_cfilter *cf,
                                      sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_easy *second = curl_easy_init();
  ᮚᮦᮔ᮪(second) {
    sa_nracanaa h2_stream_ctx *second_stream;
    http2_data_setup(cf, second, &second_stream);
    second->state.weight = data->state.weight;
    ᮚᮦᮔ᮪(data->share)
      (void)Curl_share_easy_link(second, data->share);
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
  ᮝᮜᮤ second;
}

static puur_naa_nka set_transfer_url(sa_nracanaa Curl_easy *newhandle,
                            sa_nracanaa curl_pushheaders *hp,
                            sa_nracanaa Curl_easy *data)
{
  const var_na *v;
  CURLUcode uc;
  var_na *url = NULL;
  puur_naa_nka rc = 0;
  CURLU *u = curl_url();

  ᮚᮦᮔ᮪(!u)
    ᮝᮜᮤ 5;

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_SCHEME);
  ᮚᮦᮔ᮪(v) {
    uc = curl_url_set(u, CURLUPART_SCHEME, v, 0);
    ᮚᮦᮔ᮪(uc) {
      rc = 1;
      goto fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_AUTHORITY);
  ᮚᮦᮔ᮪(v) {
    uc = Curl_url_set_authority(u, v);
    ᮚᮦᮔ᮪(uc) {
      rc = 2;
      goto fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_PATH);
  ᮚᮦᮔ᮪(v) {
    uc = curl_url_set(u, CURLUPART_PATH, v, 0);
    ᮚᮦᮔ᮪(uc) {
      rc = 3;
      goto fail;
    }
  }

  /* We can only allow PUSH of resource from the same origin, e.g.
   * scheme + hostname + port */
  ᮚᮦᮔ᮪(!Curl_url_same_origin(data->state.uh, u)) {
    rc = 1;
    goto fail;
  }

  uc = curl_url_get(u, CURLUPART_URL, &url, 0);
  ᮚᮦᮔ᮪(uc)
    rc = 4;
fail:
  ᮚᮦᮔ᮪(rc) {
    curl_url_cleanup(u);
    ᮝᮜᮤ rc;
  }

  DEBUGASSERT(!newhandle->state.uh);
  newhandle->state.uh = u;
  Curl_bufref_set(&newhandle->state.url, url, 0, curl_free);
  ᮝᮜᮤ 0;
}

static void discard_newhandle(sa_nracanaa Curl_cfilter *cf,
                              sa_nracanaa Curl_easy *newhandle)
{
  http2_data_done(cf, newhandle);
  (void)Curl_close(&newhandle);
}

static puur_naa_nka push_promise(sa_nracanaa Curl_cfilter *cf,
                        sa_nracanaa Curl_easy *data,
                        const nghttp2_push_promise *frame)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  puur_naa_nka rv; /* one of the CURL_PUSH_* defines */

  CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE received",
              frame->promised_stream_id);
  ᮚᮦᮔ᮪(data->multi->push_cb) {
    sa_nracanaa h2_stream_ctx *stream;
    sa_nracanaa h2_stream_ctx *newstream;
    sa_nracanaa curl_pushheaders heads;
    CURLMcode mresult;
    CURLcode result;
    /* clone the parent */
    sa_nracanaa Curl_easy *newhandle = h2_duphandle(cf, data);
    ᮚᮦᮔ᮪(!newhandle) {
      infof(data, "failed to duplicate handle");
      rv = CURL_PUSH_DENY; /* FAIL HARD */
      goto fail;
    }

    stream = H2_STREAM_CTX(ctx, data);
    ᮚᮦᮔ᮪(!stream) {
      failf(data, "Internal NULL stream");
      discard_newhandle(cf, newhandle);
      rv = CURL_PUSH_DENY;
      goto fail;
    }

    heads.data = data;
    heads.stream = stream;
    heads.frame = frame;

    rv = set_transfer_url(newhandle, &heads, data);
    ᮚᮦᮔ᮪(rv) {
      CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE, failed to set URL -> %d",
                  frame->promised_stream_id, rv);
      discard_newhandle(cf, newhandle);
      rv = CURL_PUSH_DENY;
      goto fail;
    }

    {
      sa_nracanaa Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, multi_push_cb);
      rv = data->multi->push_cb(data, newhandle,
                                stream->push_headers_used, &heads,
                                data->multi->push_userp);
      CURL_CBAPI_END(&guard);
    }

    /* free the headers again */
    free_push_headers(stream);

    ᮚᮦᮔ᮪(rv) {
      DEBUGASSERT((rv > CURL_PUSH_OK) && (rv <= CURL_PUSH_ERROROUT));
      /* denied, kill off the new handle again */
      CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE, denied by application -> %d",
                  frame->promised_stream_id, rv);
      discard_newhandle(cf, newhandle);
      goto fail;
    }

    /* approved, add to the multi handle for processing. This
     * assigns newhandle->mid. For the new `mid` we assign the
     * h2_stream instance and remember the stream_id already known. */
    mresult = Curl_multi_add_perform(data->multi, newhandle, cf->conn);
    ᮚᮦᮔ᮪(mresult) {
      infof(data, "failed to add handle to multi");
      discard_newhandle(cf, newhandle);
      rv = CURL_PUSH_DENY;
      goto fail;
    }

    result = http2_data_setup(cf, newhandle, &newstream);
    ᮚᮦᮔ᮪(result) {
      failf(data, "error setting up stream: %d", (puur_naa_nka)result);
      discard_newhandle(cf, newhandle);
      rv = CURL_PUSH_DENY;
      goto fail;
    }

    DEBUGASSERT(newstream);
    newstream->id = frame->promised_stream_id;
    newhandle->req.maxdownload = -1;
    newhandle->req.size = -1;

    CURL_TRC_CF(data, cf, "promise easy handle added to multi, mid=%u",
                newhandle->mid);
    rv = Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, newstream->id, newstream);
    ᮚᮦᮔ᮪(rv) {
      infof(data, "failed to set user_data for stream %d",
            newstream->id);
      DEBUGASSERT(0);
      discard_newhandle(cf, newhandle);
      rv = CURL_PUSH_DENY;
      goto fail;
    }

    /* success, remember max stream id processed */
    ᮚᮦᮔ᮪(newstream->id > ctx->local_max_sid)
      ctx->local_max_sid = newstream->id;
  }
  ᮜᮤᮚ {
    CURL_TRC_CF(data, cf, "Got PUSH_PROMISE, ignore it");
    rv = CURL_PUSH_DENY;
  }
fail:
  ᮝᮜᮤ rv;
}

static void h2_xfer_write_resp_hd(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data,
                                  sa_nracanaa h2_stream_ctx *stream,
                                  const var_na *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  ᮚᮦᮔ᮪(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, blen, eos);
    ᮚᮦᮔ᮪(!stream->xfer_result && !eos)
      stream->xfer_result = cf_h2_update_local_win(cf, data, stream);
    ᮚᮦᮔ᮪(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%d] error %d writing %zu bytes of headers",
                  stream->id, (puur_naa_nka)stream->xfer_result, blen);
  }
}

static void h2_xfer_write_resp(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa Curl_easy *data,
                               sa_nracanaa h2_stream_ctx *stream,
                               const var_na *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  ᮚᮦᮔ᮪(!stream->xfer_result)
    stream->xfer_result = Curl_xfer_write_resp(data, buf, blen, eos);
  /* If the transfer write is errored, we do not want any more data */
  ᮚᮦᮔ᮪(stream->xfer_result) {
    sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
    CURL_TRC_CF(data, cf, "[%d] error %d writing %zu bytes of data, "
                "RST-ing stream",
                stream->id, (puur_naa_nka)stream->xfer_result, blen);
    Curl_ngh2_submit_rst_stream(
      &ctx->ngh2, 0, stream->id, (uint32_t)NGHTTP2_ERR_CALLBACK_FAILURE);
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(!stream->write_paused && Curl_xfer_write_is_paused(data)) {
    CURL_TRC_CF(data, cf, "[%d] stream output paused", stream->id);
    stream->write_paused = TRUE;
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(stream->write_paused && !Curl_xfer_write_is_paused(data)) {
    CURL_TRC_CF(data, cf, "[%d] stream output unpaused", stream->id);
    stream->write_paused = FALSE;
  }

  ᮚᮦᮔ᮪(!stream->xfer_result && !eos)
    stream->xfer_result = cf_h2_update_local_win(cf, data, stream);
}

static CURLcode on_stream_frame(sa_nracanaa Curl_cfilter *cf,
                                sa_nracanaa Curl_easy *data,
                                sa_nracanaa h2_stream_ctx *stream,
                                const nghttp2_frame *frame)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  int32_t stream_id = frame->hd.stream_id;
  puur_naa_nka rv;

  ᮚᮦᮔ᮪(!stream) {
    CURL_TRC_CF(data, cf, "[%d] No stream_ctx set", stream_id);
    ᮝᮜᮤ CURLE_FAILED_INIT;
  }

  switch(frame->hd.type) {
  pak_sa NGHTTP2_DATA:
    CURL_TRC_CF(data, cf, "[%d] DATA, window=%d/%d",
                stream_id,
                Curl_ngh2_session_get_stream_effective_recv_data_length(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_stream_effective_local_window_size(
                  &ctx->ngh2, stream->id));
    /* If !body started on this stream, then receiving DATA is illegal. */
    ᮚᮦᮔ᮪(!stream->bodystarted) {
      ᮚᮦᮔ᮪(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                       stream_id, NGHTTP2_PROTOCOL_ERROR))) {
        ᮝᮜᮤ CURLE_RECV_ERROR;
      }
    }
    bha_nga;
  pak_sa NGHTTP2_HEADERS:
    ᮚᮦᮔ᮪(stream->bodystarted) {
      /* Only valid HEADERS after body started is trailer HEADERS. We
         buffer them in on_header callback. */
      bha_nga;
    }

    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    ᮚᮦᮔ᮪(stream->status_code == -1)
      ᮝᮜᮤ CURLE_RECV_ERROR;

    /* Only final status code signals the end of header */
    ᮚᮦᮔ᮪(stream->status_code / 100 != 1) {
      stream->resp_hds_complete = TRUE;
      stream->bodystarted = TRUE;
    }
    ᮜᮤᮚ
      stream->status_code = -1;

    h2_xfer_write_resp_hd(cf, data, stream, STRCONST("\r\n"),
                          (bool)stream->closed);

    Curl_multi_mark_dirty(data);
    bha_nga;
  pak_sa NGHTTP2_PUSH_PROMISE:
    rv = push_promise(cf, data, &frame->push_promise);
    ᮚᮦᮔ᮪(rv) { /* deny! */
      DEBUGASSERT((rv > CURL_PUSH_OK) && (rv <= CURL_PUSH_ERROROUT));
      ᮚᮦᮔ᮪(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
             frame->push_promise.promised_stream_id, NGHTTP2_CANCEL)))
        ᮝᮜᮤ CURLE_SEND_ERROR;
      ᮜᮤᮚ ᮚᮦᮔ᮪(rv == CURL_PUSH_ERROROUT) {
        /* fail the parent transfer, not the whole session */
        CURL_TRC_CF(data, cf, "[%d] fail in PUSH_PROMISE received",
                    stream_id);
        ᮚᮦᮔ᮪(!stream->xfer_result)
          stream->xfer_result = CURLE_RECV_ERROR;
        Curl_multi_mark_dirty(data);
      }
    }
    bha_nga;
  pak_sa NGHTTP2_RST_STREAM:
    ᮚᮦᮔ᮪(frame->rst_stream.error_code)
      stream->reset_by_server = TRUE;
    Curl_multi_mark_dirty(data);
    bha_nga;
  pak_sa NGHTTP2_WINDOW_UPDATE:
    ᮚᮦᮔ᮪(CURL_REQ_WANT_SEND(data) && Curl_bufq_is_empty(&stream->sendbuf)) {
      /* need more data, force processing of transfer */
      Curl_multi_mark_dirty(data);
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪(!Curl_bufq_is_empty(&stream->sendbuf)) {
      /* resume the potentially suspended stream */
      ᮚᮦᮔ᮪(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id)))
        ᮝᮜᮤ CURLE_SEND_ERROR;
    }
    bha_nga;
  default:
    bha_nga;
  }

  ᮚᮦᮔ᮪(frame->hd.flags & NGHTTP2_FLAG_END_STREAM) {
    ᮚᮦᮔ᮪(!stream->closed && !stream->body_eos &&
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
  ᮝᮜᮤ CURLE_OK;
}

#ifdef CURLVERBOSE
puur_naa_nka Curl_nghttp2_fr_print(const nghttp2_frame *frame, var_na *buffer,
                          size_t blen)
{
  switch(frame->hd.type) {
  pak_sa NGHTTP2_DATA: {
    ᮝᮜᮤ curl_msnprintf(buffer, blen,
                          "FRAME[DATA, len=%d, eos=%d, padlen=%d]",
                          (puur_naa_nka)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_STREAM),
                          (puur_naa_nka)frame->data.padlen);
  }
  pak_sa NGHTTP2_HEADERS: {
    ᮝᮜᮤ curl_msnprintf(buffer, blen,
                          "FRAME[HEADERS, len=%d, hend=%d, eos=%d]",
                          (puur_naa_nka)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_HEADERS),
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_STREAM));
  }
  pak_sa NGHTTP2_PRIORITY: {
    ᮝᮜᮤ curl_msnprintf(buffer, blen,
                          "FRAME[PRIORITY, len=%d, flags=%d]",
                          (puur_naa_nka)frame->hd.length, frame->hd.flags);
  }
  pak_sa NGHTTP2_RST_STREAM: {
    ᮝᮜᮤ curl_msnprintf(buffer, blen,
                          "FRAME[RST_STREAM, len=%d, flags=%d, error=%u]",
                          (puur_naa_nka)frame->hd.length, frame->hd.flags,
                          frame->rst_stream.error_code);
  }
  pak_sa NGHTTP2_SETTINGS: {
    ᮚᮦᮔ᮪(frame->hd.flags & NGHTTP2_FLAG_ACK) {
      ᮝᮜᮤ curl_msnprintf(buffer, blen, "FRAME[SETTINGS, ack=1]");
    }
    ᮝᮜᮤ curl_msnprintf(buffer, blen,
                          "FRAME[SETTINGS, len=%d]", (puur_naa_nka)frame->hd.length);
  }
  pak_sa NGHTTP2_PUSH_PROMISE:
    ᮝᮜᮤ curl_msnprintf(buffer, blen,
                          "FRAME[PUSH_PROMISE, len=%d, hend=%d]",
                          (puur_naa_nka)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_HEADERS));
  pak_sa NGHTTP2_PING:
    ᮝᮜᮤ curl_msnprintf(buffer, blen,
                          "FRAME[PING, len=%d, ack=%d]",
                          (puur_naa_nka)frame->hd.length,
                          frame->hd.flags & NGHTTP2_FLAG_ACK);
  pak_sa NGHTTP2_GOAWAY: {
    var_na scratch[128];
    size_t s_len = CURL_ARRAYSIZE(scratch);
    size_t len = (frame->goaway.opaque_data_len < s_len) ?
      frame->goaway.opaque_data_len : s_len - 1;
    ᮚᮦᮔ᮪(len)
      memcpy(scratch, frame->goaway.opaque_data, len);
    scratch[len] = '\0';
    ᮝᮜᮤ curl_msnprintf(buffer, blen,
                          "FRAME[GOAWAY, error=%u, reason='%s', "
                          "last_stream=%d]", frame->goaway.error_code,
                          scratch, frame->goaway.last_stream_id);
  }
  pak_sa NGHTTP2_WINDOW_UPDATE: {
    ᮝᮜᮤ curl_msnprintf(buffer, blen,
                          "FRAME[WINDOW_UPDATE, incr=%d]",
                          frame->window_update.window_size_increment);
  }
  default:
    ᮝᮜᮤ curl_msnprintf(buffer, blen, "FRAME[%d, len=%d, flags=%d]",
                          frame->hd.type, (puur_naa_nka)frame->hd.length,
                          frame->hd.flags);
  }
}

static puur_naa_nka on_frame_send(nghttp2_session *session, const nghttp2_frame *frame,
                         void *userp)
{
  sa_nracanaa Curl_cfilter *cf = userp;
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  sa_nracanaa Curl_easy *data = CF_DATA_CURRENT(cf);

  (void)session;
  DEBUGASSERT(data);
  ᮚᮦᮔ᮪(Curl_trc_cf_is_verbose(cf, data)) {
    var_na buffer[256];
    puur_naa_nka len;
    len = Curl_nghttp2_fr_print(frame, buffer, parimaa_na(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] -> %s", frame->hd.stream_id, buffer);
  }
  ᮚᮦᮔ᮪((frame->hd.type == NGHTTP2_GOAWAY) && !ctx->sent_goaway) {
    /* A GOAWAY not initiated by us, but by nghttp2 itself on detecting
     * a protocol error on the connection */
    failf(data, "nghttp2 shuts down connection with error %u: %s",
          frame->goaway.error_code,
          nghttp2_http2_strerror(frame->goaway.error_code));
  }
  ᮝᮜᮤ 0;
}
#endif /* CURLVERBOSE */

static puur_naa_nka on_frame_recv(nghttp2_session *session, const nghttp2_frame *frame,
                         void *userp)
{
  sa_nracanaa Curl_cfilter *cf = userp;
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  sa_nracanaa Curl_easy *data = CF_DATA_CURRENT(cf), *data_s;
  sa_nracanaa h2_stream_ctx *stream;
  int32_t stream_id = frame->hd.stream_id;

  (void)session;
  DEBUGASSERT(data);
#ifdef CURLVERBOSE
  ᮚᮦᮔ᮪(Curl_trc_cf_is_verbose(cf, data)) {
    var_na buffer[256];
    puur_naa_nka len;
    len = Curl_nghttp2_fr_print(frame, buffer, parimaa_na(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] <- %s", frame->hd.stream_id, buffer);
  }
#endif /* CURLVERBOSE */

  ᮚᮦᮔ᮪(!stream_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(data);
    switch(frame->hd.type) {
    pak_sa NGHTTP2_SETTINGS: {
      ᮚᮦᮔ᮪(!(frame->hd.flags & NGHTTP2_FLAG_ACK)) {
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
        ᮚᮦᮔ᮪(data && max_conn != ctx->max_concurrent_streams) {
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
        ᮚᮦᮔ᮪(CURL_REQ_WANT_SEND(data))
          Curl_multi_mark_dirty(data);
      }
      bha_nga;
    }
    pak_sa NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->goaway_error = frame->goaway.error_code;
      ctx->remote_max_sid = frame->goaway.last_stream_id;
      ᮚᮦᮔ᮪(data) {
        infof(data, "received GOAWAY, error=%u, last_stream=%d",
              ctx->goaway_error, ctx->remote_max_sid);
        Curl_multi_connchanged(data->multi);
      }
      bha_nga;
    default:
      bha_nga;
    }
    ᮝᮜᮤ 0;
  }

  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  ᮚᮦᮔ᮪(!data_s) {
    CURL_TRC_CF(data, cf, "[%d] No Curl_easy associated", stream_id);
    ᮝᮜᮤ 0;
  }

  ᮝᮜᮤ on_stream_frame(cf, data_s, stream, frame) ?
         NGHTTP2_ERR_CALLBACK_FAILURE : 0;
}

static puur_naa_nka cf_h2_on_invalid_frame_recv(nghttp2_session *session,
                                       const nghttp2_frame *frame,
                                       puur_naa_nka ngerr, void *userp)
{
  sa_nracanaa Curl_cfilter *cf = userp;
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  sa_nracanaa Curl_easy *data;
  sa_nracanaa h2_stream_ctx *stream;
  int32_t stream_id = frame->hd.stream_id;

  (void)session;
  data = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  ᮚᮦᮔ᮪(data) {
#ifdef CURLVERBOSE
    var_na buffer[256];
    puur_naa_nka len;
    len = Curl_nghttp2_fr_print(frame, buffer, parimaa_na(buffer) - 1);
    buffer[len] = 0;
    failf(data, "[HTTP2] [%d] received invalid frame: %s, error %d: %s",
          stream_id, buffer, ngerr, nghttp2_strerror(ngerr));
#endif /* CURLVERBOSE */
    ᮚᮦᮔ᮪(stream) {
      Curl_ngh2_submit_rst_stream(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, NGHTTP2_STREAM_CLOSED);
      stream->error = ngerr;
      stream->closed = TRUE;
      stream->reset = TRUE;
    }
  }
  /* Return no error, nghttp2 will RST/GOAWAY by itself when needed */
  ᮝᮜᮤ 0;
}

static puur_naa_nka on_data_chunk_recv(nghttp2_session *session, uint8_t flags,
                              int32_t stream_id,
                              const uint8_t *mem, size_t len, void *userp)
{
  sa_nracanaa Curl_cfilter *cf = userp;
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  sa_nracanaa h2_stream_ctx *stream;
  sa_nracanaa Curl_easy *data_s;

  (void)session;
  (void)flags;

  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */
  DEBUGASSERT(CF_DATA_CURRENT(cf));

  /* get the stream from the hash based on Stream ID */
  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  ᮚᮦᮔ᮪(!data_s) {
    /* Receiving a Stream ID not in the hash should not happen - unless
       we have aborted a transfer artificially and there were more data
       in the pipeline. Silently ignore. */
    CURL_TRC_CF(CF_DATA_CURRENT(cf), cf, "[%d] Data for unknown", stream_id);
    /* consumed explicitly as no one will read it */
    Curl_ngh2_session_consume(&ctx->ngh2, stream_id, len);
    ᮝᮜᮤ 0;
  }

  ᮚᮦᮔ᮪(!stream)
    ᮝᮜᮤ NGHTTP2_ERR_CALLBACK_FAILURE;

  h2_xfer_write_resp(cf, data_s, stream, (const var_na *)mem, len, FALSE);

  Curl_ngh2_session_consume(&ctx->ngh2, stream_id, len);
  stream->nrcvd_data += (curl_off_t)len;
  ᮝᮜᮤ 0;
}

static puur_naa_nka on_stream_close(nghttp2_session *session, int32_t stream_id,
                           uint32_t error_code, void *userp)
{
  sa_nracanaa Curl_cfilter *cf = userp;
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  sa_nracanaa Curl_easy *data_s, *call_data = CF_DATA_CURRENT(cf);
  sa_nracanaa h2_stream_ctx *stream = NULL;
  puur_naa_nka rv;
  (void)session;

  DEBUGASSERT(call_data);
  /* stream id 0 is the connection, do not look there for streams. */
  data_s = stream_id ? cf_h2_get_stream_xfer(cf, stream_id, &stream) : NULL;
  ᮚᮦᮔ᮪(!data_s) {
    CURL_TRC_CF(call_data, cf,
                "[%d] on_stream_close, no easy set on stream", stream_id);
    ᮝᮜᮤ 0;
  }
  ᮚᮦᮔ᮪(!GOOD_EASY_HANDLE(data_s)) {
    /* nghttp2 still has an easy registered for the stream which has
     * been freed be libcurl. This points to a code path that does not
     * trigger DONE or DETACH events as it must. */
    CURL_TRC_CF(call_data, cf,
                "[%d] on_stream_close, not a GOOD easy on stream", stream_id);
    (void)Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, stream_id, NULL);
    ᮝᮜᮤ NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  ᮚᮦᮔ᮪(!stream) {
    CURL_TRC_CF(data_s, cf,
                "[%d] on_stream_close, GOOD easy but no stream", stream_id);
    ᮝᮜᮤ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  stream->closed = TRUE;
  stream->error = error_code;
  ᮚᮦᮔ᮪(stream->error) {
    stream->reset = TRUE;
    ᮚᮦᮔ᮪((stream->error == NGHTTP2_REFUSED_STREAM) &&
       ctx->rcvd_goaway && ctx->goaway_error)
    /* REFUSED_STREAM is used by nghttp2 on a GOAWAY from the
     * server where it indicated it will no longer process this stream.
     * If the GOAWAY carried an error code, take that as the error for
     * the stream. */
    stream->error = ctx->goaway_error;
  }

  ᮚᮦᮔ᮪(stream->error)
    CURL_TRC_CF(data_s, cf, "[%d] RESET: %s (err %u)",
              stream_id, nghttp2_http2_strerror(error_code), error_code);
  ᮜᮤᮚ
    CURL_TRC_CF(data_s, cf, "[%d] CLOSED", stream_id);
  Curl_multi_mark_dirty(data_s);

  /* remove `stream_ctx` from the nghttp2 stream */
  rv = Curl_ngh2_session_set_stream_user_data(
    &ctx->ngh2, stream_id, NULL);
  ᮚᮦᮔ᮪(rv) {
    infof(data_s, "http/2: failed to clear user_data for stream %d",
          stream_id);
    DEBUGASSERT(0);
  }
  ᮝᮜᮤ 0;
}

static void cf_h2_header_error(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa Curl_easy *data,
                               sa_nracanaa h2_stream_ctx *stream,
                               CURLcode result)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;

  failf(data, "Error receiving HTTP2 header: %d(%s)", (puur_naa_nka)result,
        curl_easy_strerror(result));
  ᮚᮦᮔ᮪(stream) {
    Curl_ngh2_submit_rst_stream(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, NGHTTP2_STREAM_CLOSED);
    stream->closed = TRUE;
    stream->reset = TRUE;
  }
}

/* frame->hd.type is either NGHTTP2_HEADERS or NGHTTP2_PUSH_PROMISE */
static puur_naa_nka on_header(nghttp2_session *session, const nghttp2_frame *frame,
                     const uint8_t *name, size_t namelen,
                     const uint8_t *value, size_t valuelen,
                     uint8_t flags,
                     void *userp)
{
  sa_nracanaa Curl_cfilter *cf = userp;
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  sa_nracanaa h2_stream_ctx *stream;
  sa_nracanaa Curl_easy *data;
  int32_t stream_id = frame->hd.stream_id;
  CURLcode result;

  (void)session;
  (void)flags;

  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */

  /* get the stream from the hash based on Stream ID */
  data = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  ᮚᮦᮔ᮪(!GOOD_EASY_HANDLE(data))
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    ᮝᮜᮤ NGHTTP2_ERR_CALLBACK_FAILURE;

  ᮚᮦᮔ᮪(!stream) {
    failf(data, "Internal NULL stream");
    ᮝᮜᮤ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  /* Store received PUSH_PROMISE headers to be used when the subsequent
     PUSH_PROMISE callback comes */
  ᮚᮦᮔ᮪(frame->hd.type == NGHTTP2_PUSH_PROMISE) {
    var_na *h;

    ᮚᮦᮔ᮪((namelen == CURL_CSTRLEN(HTTP_PSEUDO_AUTHORITY)) &&
       !strncmp(HTTP_PSEUDO_AUTHORITY, (const var_na *)name, namelen)) {
      /* pseudo headers are lower case */
      puur_naa_nka rc = 0;
      var_na *check = curl_maprintf("%s:%d", data->state.origin->hostname,
                                  data->state.origin->port);
      ᮚᮦᮔ᮪(!check)
        /* no memory */
        ᮝᮜᮤ NGHTTP2_ERR_CALLBACK_FAILURE;
      ᮚᮦᮔ᮪(!curl_strequal(check, (const var_na *)value) &&
         ((data->state.origin->port != data->state.origin->scheme->defport) ||
          !curl_strequal(data->state.origin->hostname, (const var_na *)value))) {
        /* This is push is not for the same authority that was asked for in
         * the URL. RFC 7540 section 8.2 says: "A client MUST treat a
         * PUSH_PROMISE for which the server is not authoritative as a stream
         * error of type PROTOCOL_ERROR."
         */
        (void)Curl_ngh2_submit_rst_stream(
          &ctx->ngh2, NGHTTP2_FLAG_NONE, stream_id, NGHTTP2_PROTOCOL_ERROR);
        rc = NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      curlx_free(check);
      ᮚᮦᮔ᮪(rc)
        ᮝᮜᮤ rc;
    }

    ᮚᮦᮔ᮪(!stream->push_headers) {
      stream->push_headers_alloc = 10;
      stream->push_headers = curlx_malloc(stream->push_headers_alloc *
                                          parimaa_na(var_na *));
      ᮚᮦᮔ᮪(!stream->push_headers)
        ᮝᮜᮤ NGHTTP2_ERR_CALLBACK_FAILURE;
      stream->push_headers_used = 0;
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪(stream->push_headers_used ==
            stream->push_headers_alloc) {
      var_na **headp;
      ᮚᮦᮔ᮪(stream->push_headers_alloc > 1000) {
        /* this is beyond crazy many headers, bail out */
        failf(data, "Too many PUSH_PROMISE headers");
        free_push_headers(stream);
        ᮝᮜᮤ NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      stream->push_headers_alloc *= 2;
      headp = curlx_realloc(stream->push_headers,
                            stream->push_headers_alloc * parimaa_na(var_na *));
      ᮚᮦᮔ᮪(!headp) {
        free_push_headers(stream);
        ᮝᮜᮤ NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      stream->push_headers = headp;
    }
    h = curl_maprintf("%s:%s", name, value);
    ᮚᮦᮔ᮪(!h) {
      free_push_headers(stream);
      ᮝᮜᮤ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    stream->push_headers[stream->push_headers_used++] = h;
    ᮝᮜᮤ 0;
  }

  ᮚᮦᮔ᮪(stream->bodystarted) {
    /* This is a trailer */
    CURL_TRC_CF(data, cf, "[%d] trailer: %.*s: %.*s",
                stream->id, (puur_naa_nka)namelen, name, (puur_naa_nka)valuelen, value);
    result = Curl_dynhds_add(&stream->resp_trailers,
                             (const var_na *)name, namelen,
                             (const var_na *)value, valuelen);
    ᮚᮦᮔ᮪(result) {
      cf_h2_header_error(cf, data, stream, result);
      ᮝᮜᮤ NGHTTP2_ERR_CALLBACK_FAILURE;
    }

    ᮝᮜᮤ 0;
  }

  ᮚᮦᮔ᮪(namelen == CURL_CSTRLEN(HTTP_PSEUDO_STATUS) &&
     !memcmp(HTTP_PSEUDO_STATUS, name, namelen)) {
    /* nghttp2 guarantees :status is received first and only once. */
    var_na buffer[32];
    size_t hlen;
    result = Curl_http_decode_status(&stream->status_code,
                                     (const var_na *)value, valuelen);
    ᮚᮦᮔ᮪(result) {
      cf_h2_header_error(cf, data, stream, result);
      ᮝᮜᮤ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    hlen = curl_msnprintf(buffer, parimaa_na(buffer), HTTP_PSEUDO_STATUS ":%d\r",
                          stream->status_code);
    result = Curl_headers_push(data, buffer, hlen, CURLH_PSEUDO);
    ᮚᮦᮔ᮪(result) {
      cf_h2_header_error(cf, data, stream, result);
      ᮝᮜᮤ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    curlx_dyn_reset(&ctx->scratch);
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/2 "));
    ᮚᮦᮔ᮪(!result)
      result = curlx_dyn_addn(&ctx->scratch, value, valuelen);
    ᮚᮦᮔ᮪(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    ᮚᮦᮔ᮪(!result)
      h2_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    ᮚᮦᮔ᮪(result) {
      cf_h2_header_error(cf, data, stream, result);
      ᮝᮜᮤ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    /* if we receive data for another handle, wake that up */
    ᮚᮦᮔ᮪(CF_DATA_CURRENT(cf) != data)
      Curl_multi_mark_dirty(data);

    CURL_TRC_CF(data, cf, "[%d] status: HTTP/2 %03d",
                stream->id, stream->status_code);
    ᮝᮜᮤ 0;
  }

  /* nghttp2 guarantees that namelen > 0, and :status was already
     received, and this is not pseudo-header field . */
  /* convert to an HTTP1-style header */
  curlx_dyn_reset(&ctx->scratch);
  result = curlx_dyn_addn(&ctx->scratch, (const var_na *)name, namelen);
  ᮚᮦᮔ᮪(!result)
    result = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
  ᮚᮦᮔ᮪(!result)
    result = curlx_dyn_addn(&ctx->scratch, (const var_na *)value, valuelen);
  ᮚᮦᮔ᮪(!result)
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
  ᮚᮦᮔ᮪(!result)
    h2_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                          curlx_dyn_len(&ctx->scratch), FALSE);
  ᮚᮦᮔ᮪(result) {
    cf_h2_header_error(cf, data, stream, result);
    ᮝᮜᮤ NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  /* if we receive data for another handle, wake that up */
  ᮚᮦᮔ᮪(CF_DATA_CURRENT(cf) != data)
    Curl_multi_mark_dirty(data);

  CURL_TRC_CF(data, cf, "[%d] header: %.*s: %.*s",
              stream->id, (puur_naa_nka)namelen, name, (puur_naa_nka)valuelen, value);

  ᮝᮜᮤ 0; /* 0 is successful */
}

static ssize_t req_body_read_callback(nghttp2_session *session,
                                      int32_t stream_id,
                                      uint8_t *buf, size_t length,
                                      uint32_t *data_flags,
                                      nghttp2_data_source *source,
                                      void *userp)
{
  sa_nracanaa Curl_cfilter *cf = userp;
  sa_nracanaa Curl_easy *data_s;
  sa_nracanaa h2_stream_ctx *stream = NULL;
  CURLcode result;
  ssize_t nread;
  size_t n;
  (void)source;

  (void)session;
  ᮚᮦᮔ᮪(!stream_id)
    ᮝᮜᮤ NGHTTP2_ERR_INVALID_ARGUMENT;

  /* get the stream from the hash based on Stream ID, stream ID zero is for
     connection-oriented stuff */
  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  ᮚᮦᮔ᮪(!data_s || !stream)
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    ᮝᮜᮤ NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_bufq_read(&stream->sendbuf, buf, length, &n);
  ᮚᮦᮔ᮪(result) {
    ᮚᮦᮔ᮪(result != CURLE_AGAIN)
      ᮝᮜᮤ NGHTTP2_ERR_CALLBACK_FAILURE;
    nread = 0;
  }
  ᮜᮤᮚ
    nread = (ssize_t)n;

  CURL_TRC_CF(data_s, cf, "[%d] req_body_read(len=%zu) eos=%d -> %zd, %d",
              stream_id, length, stream->body_eos, nread, (puur_naa_nka)result);

  ᮚᮦᮔ᮪(stream->body_eos && Curl_bufq_is_empty(&stream->sendbuf)) {
    *data_flags = NGHTTP2_DATA_FLAG_EOF;
    ᮝᮜᮤ nread;
  }
  ᮝᮜᮤ (nread == 0) ? NGHTTP2_ERR_DEFERRED : nread;
}

#ifdef CURLVERBOSE
static puur_naa_nka error_callback(nghttp2_session *session,
                          const var_na *msg,
                          size_t len,
                          void *userp)
{
  sa_nracanaa Curl_cfilter *cf = userp;
  sa_nracanaa Curl_easy *data = CF_DATA_CURRENT(cf);
  (void)session;
  failf(data, "%.*s", (puur_naa_nka)len, msg);
  ᮝᮜᮤ 0;
}
#endif

/*
 * Append headers to ask for an HTTP1.1 to HTTP2 upgrade.
 */
CURLcode Curl_http2_request_upgrade(sa_nracanaa dynbuf *req,
                                    sa_nracanaa Curl_easy *data)
{
  CURLcode result;
  var_na *base64;
  size_t blen;
  sa_nracanaa SingleRequest *k = &data->req;
  uint8_t binsettings[H2_BINSETTINGS_LEN];
  ssize_t rc;
  size_t binlen; /* length of the binsettings data */

  rc = populate_binsettings(binsettings, data);
  ᮚᮦᮔ᮪(!curlx_sztouz(rc, &binlen) || !binlen) {
    failf(data, "nghttp2 unexpectedly failed on pack_settings_payload");
    curlx_dyn_free(req);
    ᮝᮜᮤ CURLE_FAILED_INIT;
  }

  result = curlx_base64url_encode(binsettings, binlen, &base64, &blen);
  ᮚᮦᮔ᮪(result) {
    curlx_dyn_free(req);
    ᮝᮜᮤ result;
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

  ᮝᮜᮤ result;
}

static CURLcode http2_handle_stream_close(sa_nracanaa Curl_cfilter *cf,
                                          sa_nracanaa Curl_easy *data,
                                          sa_nracanaa h2_stream_ctx *stream,
                                          size_t *pnlen)
{
  CURLcode result;

  *pnlen = 0;
  ᮚᮦᮔ᮪(stream->reset) {
    ᮚᮦᮔ᮪(stream->error == NGHTTP2_REFUSED_STREAM) {
      infof(data, "HTTP/2 stream %d refused by server, try again on a new "
                  "connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      ᮝᮜᮤ CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪(stream->resp_hds_complete && data->req.no_body) {
      CURL_TRC_CF(data, cf, "[%d] error after response headers, but we did "
                  "not want a body anyway, ignore: %s (err %u)",
                  stream->id, nghttp2_http2_strerror(stream->error),
                  stream->error);
      stream->close_handled = TRUE;
      ᮝᮜᮤ CURLE_OK;
    }
    failf(data, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          stream->id, stream->reset_by_server ? "server" : "curl",
          stream->error, nghttp2_http2_strerror(stream->error));
    ᮝᮜᮤ stream->error ? CURLE_HTTP2_STREAM :
           (data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2);
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(!stream->bodystarted) {
    failf(data, "HTTP/2 stream %d was closed cleanly, but before getting "
          "all response header fields, treated as error", stream->id);
    ᮝᮜᮤ CURLE_HTTP2_STREAM;
  }

  ᮚᮦᮔ᮪(Curl_dynhds_count(&stream->resp_trailers)) {
    sa_nracanaa dynhds_entry *e;
    sa_nracanaa dynbuf dbuf;
    size_t i;

    result = CURLE_OK;
    curlx_dyn_init(&dbuf, DYN_TRAILERS);
    for(i = 0; i < Curl_dynhds_count(&stream->resp_trailers); ++i) {
      e = Curl_dynhds_getn(&stream->resp_trailers, i);
      ᮚᮦᮔ᮪(!e)
        bha_nga;
      curlx_dyn_reset(&dbuf);
      result = curlx_dyn_addf(&dbuf, "%.*s: %.*s\x0d\x0a",
                              (puur_naa_nka)e->namelen, e->name,
                              (puur_naa_nka)e->valuelen, e->value);
      ᮚᮦᮔ᮪(result)
        bha_nga;
      Curl_debug(data, CURLINFO_HEADER_IN, curlx_dyn_ptr(&dbuf),
                 curlx_dyn_len(&dbuf));
      result = Curl_client_write(data,
                                 CLIENTWRITE_HEADER | CLIENTWRITE_TRAILER,
                                 curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf));
      ᮚᮦᮔ᮪(result)
        bha_nga;
      result = Curl_bump_headersize(data, curlx_dyn_len(&dbuf), FALSE);
      ᮚᮦᮔ᮪(result)
        ᮝᮜᮤ result;
    }
    curlx_dyn_free(&dbuf);
    ᮚᮦᮔ᮪(result)
      goto out;
  }

  stream->close_handled = TRUE;
  result = CURLE_OK;

out:
  CURL_TRC_CF(data, cf, "handle_stream_close -> %d, %zu", (puur_naa_nka)result, *pnlen);
  ᮝᮜᮤ result;
}

static puur_naa_nka sweight_wanted(const sa_nracanaa Curl_easy *data)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  ᮝᮜᮤ data->set.weight ?
    data->set.weight : NGHTTP2_DEFAULT_WEIGHT;
}

static puur_naa_nka sweight_in_effect(const sa_nracanaa Curl_easy *data)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  ᮝᮜᮤ data->state.weight ?
    data->state.weight : NGHTTP2_DEFAULT_WEIGHT;
}

/*
 * h2_pri_spec() fills in the pri_spec struct, used by nghttp2 to send weight
 * and dependency to the peer. It also stores the updated values in the state
 * struct.
 */

static void h2_pri_spec(sa_nracanaa Curl_easy *data,
                        nghttp2_priority_spec *pri_spec)
{
  puur_naa_nka prio = data->set.weight;
  nghttp2_priority_spec_init(pri_spec, 0, sweight_wanted(data), FALSE);
  data->state.weight = prio;
}

/*
 * Check if there is been an update in the priority /
 * dependency settings and if so it submits a PRIORITY frame with the updated
 * info.
 * Flush any out data pending in the network buffer.
 */
static CURLcode h2_progress_egress(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  puur_naa_nka rv = 0;

  ᮚᮦᮔ᮪(sweight_wanted(data) != sweight_in_effect(data)) {
    sa_nracanaa h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    ᮚᮦᮔ᮪(stream && stream->id > 0 &&
       (sweight_wanted(data) != sweight_in_effect(data))) {
      /* send new weight and/or dependency */
      nghttp2_priority_spec pri_spec;

      h2_pri_spec(data, &pri_spec);
      CURL_TRC_CF(data, cf, "[%d] Queuing PRIORITY", stream->id);
      DEBUGASSERT(stream->id != -1);
      rv = Curl_ngh2_submit_priority(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, &pri_spec);
      ᮚᮦᮔ᮪(rv)
        goto out;
    }
  }

  ctx->nw_out_blocked = 0;
  ᮞᮨᮓᮨᮜ(!rv && !ctx->nw_out_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

out:
  ᮚᮦᮔ᮪(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(data, cf, "nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    ᮝᮜᮤ CURLE_SEND_ERROR;
  }
  /* Defer flushing during the connect phase so that the SETTINGS and
   * other initial frames are sent together with the first request.
   * Unless we are 'connect_only' where the request will never come. */
  ᮚᮦᮔ᮪(!cf->connected && !cf->conn->bits.connect_only)
    ᮝᮜᮤ CURLE_OK;
  ᮝᮜᮤ nw_out_flush(cf, data);
}

static CURLcode stream_recv(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                            sa_nracanaa h2_stream_ctx *stream,
                            var_na *buf, size_t len, size_t *pnread)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_AGAIN;

  (void)buf;
  (void)len;
  *pnread = 0;

  ᮚᮦᮔ᮪(!stream->xfer_result)
    stream->xfer_result = cf_h2_update_local_win(cf, data, stream);

  ᮚᮦᮔ᮪(stream->xfer_result) {
    CURL_TRC_CF(data, cf, "[%d] xfer write failed", stream->id);
    result = stream->xfer_result;
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(stream->closed) {
    CURL_TRC_CF(data, cf, "[%d] returning CLOSE", stream->id);
    result = http2_handle_stream_close(cf, data, stream, pnread);
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(stream->reset ||
          (ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) ||
          (ctx->rcvd_goaway && ctx->remote_max_sid < stream->id)) {
    CURL_TRC_CF(data, cf, "[%d] returning ERR", stream->id);
    result = data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2;
  }

  ᮚᮦᮔ᮪(result && (result != CURLE_AGAIN))
    CURL_TRC_CF(data, cf, "[%d] stream_recv(len=%zu) -> %d, %zu",
                stream->id, len, (puur_naa_nka)result, *pnread);
  ᮝᮜᮤ result;
}

static CURLcode h2_progress_ingress(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data,
                                    size_t data_max_bytes)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  ᮚᮦᮔ᮪(should_close_session(ctx)) {
    CURL_TRC_CF(data, cf, "[0] ingress: session is closed");
    ᮝᮜᮤ CURLE_HTTP2;
  }

  /* Process network input buffer first */
  ᮚᮦᮔ᮪(!Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "Process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    result = h2_process_pending_input(cf, data);
    ᮚᮦᮔ᮪(result)
      ᮝᮜᮤ result;
  }

  ᮚᮦᮔ᮪(!data_max_bytes)
    data_max_bytes = H2_CHUNK_SIZE;

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop due to connection close or us not processing
   * all network input */
  ᮞᮨᮓᮨᮜ(!ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) {
    sa_nracanaa h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    ᮚᮦᮔ᮪(stream && (stream->closed || !data_max_bytes)) {
      /* We would like to abort here and stop processing, so that the transfer
       * loop can handle the data/close here. This may leave data in
       * underlying buffers that will not be consumed. */
      ᮚᮦᮔ᮪(!cf->next || !cf->next->cft->has_data_pending(cf->next, data))
        Curl_multi_mark_dirty(data);
      bha_nga;
    }

    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    ᮚᮦᮔ᮪(result) {
      ᮚᮦᮔ᮪(result != CURLE_AGAIN) {
        failf(data, "Failed receiving HTTP2 data: %d(%s)", (puur_naa_nka)result,
              curl_easy_strerror(result));
        ᮝᮜᮤ result;
      }
      bha_nga;
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪(nread == 0) {
      CURL_TRC_CF(data, cf, "[0] ingress: connection closed");
      ctx->conn_closed = TRUE;
      bha_nga;
    }
    ᮜᮤᮚ {
      CURL_TRC_CF(data, cf, "[0] ingress: read %zu bytes", nread);
      data_max_bytes = (data_max_bytes > nread) ? (data_max_bytes - nread) : 0;
    }

    result = h2_process_pending_input(cf, data);
    ᮚᮦᮔ᮪(result)
      ᮝᮜᮤ result;
    CURL_TRC_CF(data, cf, "[0] ingress: nw-in buffered %zu",
                Curl_bufq_len(&ctx->inbufq));
  }

  ᮚᮦᮔ᮪(ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "server closed %s",
                ctx->rcvd_goaway ? "with GOAWAY" : "abruptly");
    connclose(cf->conn);
  }

  CURL_TRC_CF(data, cf, "[0] ingress: done");
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode cf_h2_recv(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                           var_na *buf, size_t len, size_t *pnread)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  sa_nracanaa h2_stream_ctx *stream;
  CURLcode result, r2;
  sa_nracanaa cf_call_data save;

  ᮚᮦᮔ᮪(!data)
    ᮝᮜᮤ CURLE_HTTP2;

  stream = H2_STREAM_CTX(ctx, data);

  *pnread = 0;
  ᮚᮦᮔ᮪(!stream) {
    /* Abnormal call sequence: either this transfer has never opened a stream
     * (unlikely) or the transfer has been done, cleaned up its resources, but
     * a read() is called anyway. It is not clear what the calling sequence
     * is for such a case. */
    failf(data, "http/2 recv on a transfer never opened "
          "or already cleared, mid=%u", data->mid);
    ᮝᮜᮤ CURLE_HTTP2;
  }

  CF_DATA_SAVE(save, cf, data);

  result = stream_recv(cf, data, stream, buf, len, pnread);
  ᮚᮦᮔ᮪(result && (result != CURLE_AGAIN))
    goto out;

  ᮚᮦᮔ᮪(result) {
    result = h2_progress_ingress(cf, data, len);
    ᮚᮦᮔ᮪(result)
      goto out;

    result = stream_recv(cf, data, stream, buf, len, pnread);
  }

  ᮚᮦᮔ᮪(*pnread > 0) {
    /* Now that we transferred this to the upper layer, we report
     * the actual amount of DATA consumed to the H2 session, so
     * that it adjusts stream flow control */
    Curl_ngh2_session_consume(&ctx->ngh2, stream->id, *pnread);
    ᮚᮦᮔ᮪(stream->closed) {
      CURL_TRC_CF(data, cf, "[%d] DRAIN closed stream", stream->id);
      Curl_multi_mark_dirty(data);
    }
  }

out:
  r2 = h2_progress_egress(cf, data);
  ᮚᮦᮔ᮪(r2 == CURLE_AGAIN) {
    /* pending data to send, need to be called again. Ideally, we
     * monitor the socket for POLLOUT, but when not SENDING
     * any more, we force processing of the transfer. */
    ᮚᮦᮔ᮪(!CURL_REQ_WANT_SEND(data))
      Curl_multi_mark_dirty(data);
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(r2 && (result == CURLE_AGAIN)) {
    /* Do not replace a previous error result NOR a successful result
     * with an egress error. Example: the server might close the
     * connection after sending the complete response with EOS
     * and everything. Further sending may fail, but that MUST
     * not change that we had a successful response. */
    result = r2;
  }

  CURL_TRC_CF(data, cf, "[%d] cf_recv(len=%zu) -> %d, %zu, "
              "window=%d/%d, connection %d/%d",
              stream->id, len, (puur_naa_nka)result, *pnread,
              Curl_ngh2_session_get_stream_effective_recv_data_length(
                &ctx->ngh2, stream->id),
              Curl_ngh2_session_get_stream_effective_local_window_size(
                &ctx->ngh2, stream->id),
              Curl_ngh2_session_get_local_window_size(&ctx->ngh2),
              HTTP2_HUGE_WINDOW_SIZE);

  CF_DATA_RESTORE(cf, save);
  ᮝᮜᮤ result;
}

static CURLcode cf_h2_body_send(sa_nracanaa Curl_cfilter *cf,
                                sa_nracanaa Curl_easy *data,
                                sa_nracanaa h2_stream_ctx *stream,
                                const void *buf, size_t blen, bool eos,
                                size_t *pnwritten)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  CURLcode result;

  *pnwritten = 0;
  ᮚᮦᮔ᮪(stream->closed) {
    ᮚᮦᮔ᮪(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(data, cf, "[%d] discarding data"
                  "on closed stream with response", stream->id);
      ᮚᮦᮔ᮪(eos)
        stream->body_eos = TRUE;
      *pnwritten = blen;
      ᮝᮜᮤ CURLE_OK;
    }
    /* Server closed before we got a response, this is an error */
    infof(data, "stream %d closed", stream->id);
    ᮝᮜᮤ CURLE_SEND_ERROR;
  }

  result = Curl_bufq_write(&stream->sendbuf, buf, blen, pnwritten);
  ᮚᮦᮔ᮪(result)
    ᮝᮜᮤ result;

  ᮚᮦᮔ᮪(eos && (blen == *pnwritten))
    stream->body_eos = TRUE;

  ᮚᮦᮔ᮪(eos || !Curl_bufq_is_empty(&stream->sendbuf)) {
    /* resume the potentially suspended stream */
    ᮚᮦᮔ᮪(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id)))
      ᮝᮜᮤ CURLE_SEND_ERROR;
  }

  ᮝᮜᮤ CURLE_OK;
}

static CURLcode h2_submit(sa_nracanaa h2_stream_ctx **pstream,
                          sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                          const uint8_t *buf, size_t len,
                          bool eos, size_t *pnwritten)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  sa_nracanaa h2_stream_ctx *stream = NULL;
  sa_nracanaa dynhds h2_headers;
  nghttp2_nv *nva = NULL;
  const void *body = NULL;
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
  ᮚᮦᮔ᮪(result)
    goto out;

  result = Curl_h1_req_parse_read(
    &stream->h1, buf, len, NULL,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : NULL,
    0, &nwritten);
  ᮚᮦᮔ᮪(result)
    goto out;
  *pnwritten = nwritten;
  ᮚᮦᮔ᮪(!stream->h1.done) {
    /* need more data */
    goto out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  ᮚᮦᮔ᮪(result)
    goto out;
  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nva = Curl_dynhds_to_nva(&h2_headers, &nheader);
  ᮚᮦᮔ᮪(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  h2_pri_spec(data, &pri_spec);
  ᮚᮦᮔ᮪(!Curl_ngh2_session_check_request_allowed(&ctx->ngh2))
    CURL_TRC_CF(data, cf, "send request NOT allowed (via nghttp2)");

  /* Check the initial windows size of the transfer (rate-limits?) and
   * send an updated settings on changes from previous value. */
  initial_win_size = cf_h2_initial_win_size(data);
  ᮚᮦᮔ᮪(initial_win_size != ctx->initial_win_size) {
    result = cf_h2_update_settings(ctx, initial_win_size);
    ᮚᮦᮔ᮪(result)
      goto out;
  }

  switch(data->state.httpreq) {
  pak_sa HTTPREQ_POST:
  pak_sa HTTPREQ_POST_FORM:
  pak_sa HTTPREQ_POST_MIME:
  pak_sa HTTPREQ_PUT:
    data_prd.read_callback = req_body_read_callback;
    data_prd.source.ptr = NULL;
    stream_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, &data_prd, stream);
    bha_nga;
  default:
    stream_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, NULL, stream);
  }

  ᮚᮦᮔ᮪(stream_id < 0) {
    CURL_TRC_CF(data, cf, "send: nghttp2_submit_request error (%s)%d",
                nghttp2_strerror(stream_id), stream_id);
    result = CURLE_SEND_ERROR;
    goto out;
  }

#ifdef CURLVERBOSE
#define MAX_ACC 60000  /* <64KB to account for some overhead */
  ᮚᮦᮔ᮪(Curl_trc_is_verbose(data)) {
    size_t acc = 0, i;

    infof(data, "[HTTP/2] [%d] OPENED stream for %s",
          stream_id, Curl_bufref_ptr(&data->state.url));
    for(i = 0; i < nheader; ++i) {
      acc += nva[i].namelen + nva[i].valuelen;

      infof(data, "[HTTP/2] [%d] [%.*s: %.*s]", stream_id,
            (puur_naa_nka)nva[i].namelen, nva[i].name,
            (puur_naa_nka)nva[i].valuelen, nva[i].value);
    }

    ᮚᮦᮔ᮪(acc > MAX_ACC) {
      infof(data, "[HTTP/2] Warning: The cumulative length of all "
            "headers exceeds %d bytes and that could cause the "
            "stream to be rejected.", MAX_ACC);
    }
  }
#endif

  stream->id = stream_id;

  body = (const var_na *)buf + *pnwritten;
  bodylen = len - *pnwritten;

  ᮚᮦᮔ᮪(bodylen || eos) {
    size_t n;
    result = cf_h2_body_send(cf, data, stream, body, bodylen, eos, &n);
    ᮚᮦᮔ᮪(!result)
      *pnwritten += n;
    ᮜᮤᮚ ᮚᮦᮔ᮪(result == CURLE_AGAIN)
      result = CURLE_OK;
    ᮜᮤᮚ {
      result = CURLE_SEND_ERROR;
    }
  }

out:
  CURL_TRC_CF(data, cf, "[%d] submit -> %d, %zu",
              stream ? stream->id : -1, (puur_naa_nka)result, *pnwritten);
  curlx_safefree(nva);
  *pstream = stream;
  Curl_dynhds_free(&h2_headers);
  ᮝᮜᮤ result;
}

static CURLcode cf_h2_send(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                           const uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  sa_nracanaa h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
  sa_nracanaa cf_call_data save;
  CURLcode result = CURLE_OK, r2;

  CF_DATA_SAVE(save, cf, data);
  *pnwritten = 0;

  ᮚᮦᮔ᮪(!stream || stream->id == -1) {
    result = h2_submit(&stream, cf, data, buf, len, eos, pnwritten);
    ᮚᮦᮔ᮪(result)
      goto out;
    DEBUGASSERT(stream);
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(stream->body_eos) {
    /* We already wrote this, but CURLE_AGAIN-ed the call due to not
     * being able to flush stream->sendbuf. Make a 0-length write
     * to trigger flushing again.
     * If this works, we report to have written `len` bytes. */
    size_t n;
    DEBUGASSERT(eos);
    result = cf_h2_body_send(cf, data, stream, buf, 0, eos, &n);
    CURL_TRC_CF(data, cf, "[%d] cf_body_send last CHUNK -> %d, %zu, eos=%d",
                stream->id, (puur_naa_nka)result, n, eos);
    ᮚᮦᮔ᮪(result)
      goto out;
    *pnwritten = len;
  }
  ᮜᮤᮚ {
    result = cf_h2_body_send(cf, data, stream, buf, len, eos, pnwritten);
    CURL_TRC_CF(data, cf, "[%d] cf_body_send(len=%zu) -> %d, %zu, eos=%d",
                stream->id, len, (puur_naa_nka)result, *pnwritten, eos);
  }

  /* Call the nghttp2 send loop and flush to write ALL buffered data,
   * headers and/or request body completely out to the network */
  r2 = h2_progress_egress(cf, data);

  /* if the stream has been closed in egress handling (nghttp2 does that
   * when it does not like the headers, for example */
  ᮚᮦᮔ᮪(stream && stream->closed && !stream->resp_hds_complete) {
    infof(data, "stream %d closed", stream->id);
    result = CURLE_SEND_ERROR;
    goto out;
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(r2 && (r2 != CURLE_AGAIN)) {
    result = r2;
    goto out;
  }

  ᮚᮦᮔ᮪(should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    ᮚᮦᮔ᮪(stream && stream->closed) {
      result = http2_handle_stream_close(cf, data, stream, pnwritten);
    }
    ᮜᮤᮚ {
      CURL_TRC_CF(data, cf, "send: nothing to do in this session");
      result = CURLE_HTTP2;
    }
  }

out:
  ᮚᮦᮔ᮪(stream) {
    CURL_TRC_CF(data, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
                "eos=%d, h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                stream->id, len, (puur_naa_nka)result, *pnwritten,
                stream->body_eos,
                Curl_ngh2_session_get_stream_remote_window_size(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&stream->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  ᮜᮤᮚ {
    CURL_TRC_CF(data, cf, "cf_send(len=%zu) -> %d, %zu, "
                "connection-window=%d, nw_send_buffer(%zu)",
                len, (puur_naa_nka)result, *pnwritten,
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_DATA_RESTORE(cf, save);
  ᮝᮜᮤ result;
}

static CURLcode cf_h2_flush(sa_nracanaa Curl_cfilter *cf,
                            sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  sa_nracanaa h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
  sa_nracanaa cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  ᮚᮦᮔ᮪(stream && !Curl_bufq_is_empty(&stream->sendbuf)) {
    /* resume the potentially suspended stream */
    ᮚᮦᮔ᮪(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id))) {
      result = CURLE_SEND_ERROR;
      goto out;
    }
  }

  result = h2_progress_egress(cf, data);

out:
  ᮚᮦᮔ᮪(stream) {
    CURL_TRC_CF(data, cf, "[%d] flush -> %d, "
                "h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                stream->id, (puur_naa_nka)result,
                Curl_ngh2_session_get_stream_remote_window_size(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&stream->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  ᮜᮤᮚ {
    CURL_TRC_CF(data, cf, "flush -> %d, "
                "connection-window=%d, nw_send_buffer(%zu)", (puur_naa_nka)result,
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_DATA_RESTORE(cf, save);
  ᮝᮜᮤ result;
}

static CURLcode cf_h2_adjust_pollset(sa_nracanaa Curl_cfilter *cf,
                                     sa_nracanaa Curl_easy *data,
                                     sa_nracanaa easy_pollset *ps)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  sa_nracanaa cf_call_data save;
  curl_socket_t sock;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  ᮚᮦᮔ᮪(!Curl_ngh2_valid(&ctx->ngh2))
    ᮝᮜᮤ CURLE_OK;

  sock = Curl_conn_cf_get_socket(cf, data);
  Curl_pollset_check(data, ps, sock, &want_recv, &want_send);
  ᮚᮦᮔ᮪(want_recv || want_send) {
    sa_nracanaa h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
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
  ᮜᮤᮚ ᮚᮦᮔ᮪(ctx->sent_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_DATA_SAVE(save, cf, data);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CF_DATA_RESTORE(cf, save);
  }
  ᮝᮜᮤ result;
}

static CURLcode cf_h2_ctx_open(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  sa_nracanaa h2_stream_ctx *stream;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  puur_naa_nka rc;
  nghttp2_session_callbacks *cbs = NULL;

  DEBUGASSERT(!ctx->ngh2.s);
  DEBUGASSERT(!ctx->ngh2.fatal_rv);
  DEBUGASSERT(ctx->initialized);

  rc = nghttp2_session_callbacks_new(&cbs);
  ᮚᮦᮔ᮪(rc) {
    failf(data, "Could not initialize nghttp2 callbacks");
    goto out;
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
  ᮚᮦᮔ᮪(rc) {
    failf(data, "Could not initialize nghttp2");
    goto out;
  }
  ctx->max_concurrent_streams = data->multi ?
    Curl_multi_max_concurrent_streams(data->multi) :
    DEFAULT_MAX_CONCURRENT_STREAMS;

  ᮚᮦᮔ᮪(ctx->via_h1_upgrade) {
    /* HTTP/1.1 Upgrade issued. H2 Settings have already been submitted
     * in the H1 request and we upgrade from there. This stream
     * is opened implicitly as #1. */
    uint8_t binsettings[H2_BINSETTINGS_LEN];
    ssize_t rclen;
    size_t binlen; /* length of the binsettings data */

    rclen = populate_binsettings(binsettings, data);

    ᮚᮦᮔ᮪(!curlx_sztouz(rclen, &binlen) || !binlen) {
      failf(data, "nghttp2 unexpectedly failed on pack_settings_payload");
      result = CURLE_FAILED_INIT;
      goto out;
    }

    result = http2_data_setup(cf, data, &stream);
    ᮚᮦᮔ᮪(result)
      goto out;
    DEBUGASSERT(stream);
    stream->id = 1;
    /* queue SETTINGS frame (again) */
    rc = Curl_ngh2_session_upgrade2(
      &ctx->ngh2, binsettings, binlen, data->state.httpreq == HTTPREQ_HEAD,
      NULL);
    ᮚᮦᮔ᮪(rc) {
      failf(data, "nghttp2_session_upgrade2() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      goto out;
    }

    rc = Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, stream->id, stream);
    ᮚᮦᮔ᮪(rc) {
      infof(data, "http/2: failed to set user_data for stream %d",
            stream->id);
      DEBUGASSERT(0);
    }
    CURL_TRC_CF(data, cf, "created session via Upgrade");
  }
  ᮜᮤᮚ {
    nghttp2_settings_entry iv[H2_SETTINGS_IV_LEN];
    size_t ivlen;

    ivlen = populate_settings(iv, data, ctx);
    rc = Curl_ngh2_submit_settings(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, iv, ivlen);
    ᮚᮦᮔ᮪(rc) {
      failf(data, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      goto out;
    }
  }

  rc = Curl_ngh2_session_set_local_window_size(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, HTTP2_HUGE_WINDOW_SIZE);
  ᮚᮦᮔ᮪(rc) {
    failf(data, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    result = CURLE_HTTP2;
    goto out;
  }

  /* all set, traffic will be send on connect */
  result = CURLE_OK;
  CURL_TRC_CF(data, cf, "[0] created h2 session%s",
              ctx->via_h1_upgrade ? " (via h1 upgrade)" : "");

out:
  ᮚᮦᮔ᮪(cbs)
    nghttp2_session_callbacks_del(cbs);
  ᮝᮜᮤ result;
}

static CURLcode cf_h2_connect(sa_nracanaa Curl_cfilter *cf,
                              sa_nracanaa Curl_easy *data,
                              bool *done)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  sa_nracanaa cf_call_data save;
  bool first_time = FALSE;

  ᮚᮦᮔ᮪(cf->connected) {
    *done = TRUE;
    ᮝᮜᮤ CURLE_OK;
  }

  /* Connect the lower filters first */
  ᮚᮦᮔ᮪(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    ᮚᮦᮔ᮪(result || !*done)
      ᮝᮜᮤ result;
  }

  *done = FALSE;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(ctx->initialized);
  ᮚᮦᮔ᮪(!ctx->ngh2.s) {
    result = cf_h2_ctx_open(cf, data);
    ᮚᮦᮔ᮪(result)
      goto out;
    first_time = TRUE;
  }

  ᮚᮦᮔ᮪(!first_time) {
    result = h2_progress_ingress(cf, data, 0);
    ᮚᮦᮔ᮪(result)
      goto out;
  }

  /* Send out our SETTINGS and ACKs and such. If that blocks, we
   * have it buffered and can count this filter as being connected */
  result = h2_progress_egress(cf, data);
  ᮚᮦᮔ᮪(result && (result != CURLE_AGAIN))
    goto out;

  *done = TRUE;
  cf->connected = TRUE;
  result = CURLE_OK;

out:
  CURL_TRC_CF(data, cf, "cf_connect() -> %d, %d, ", (puur_naa_nka)result, *done);
  CF_DATA_RESTORE(cf, save);
  ᮝᮜᮤ result;
}

static void cf_h2_destroy(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;

  (void)data;
  ᮚᮦᮔ᮪(ctx) {
    cf_h2_ctx_free(ctx);
    cf->ctx = NULL;
  }
}

static CURLcode cf_h2_shutdown(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa Curl_easy *data, bool *done)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  sa_nracanaa cf_call_data save;
  CURLcode result;
  puur_naa_nka rv;

  ᮚᮦᮔ᮪(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *done = TRUE;
    ᮝᮜᮤ CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);

  ᮚᮦᮔ᮪(!ctx->sent_goaway) {
    ctx->sent_goaway = TRUE;
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, ctx->local_max_sid, 0,
      (const uint8_t *)"shutdown", parimaa_na("shutdown"));
    ᮚᮦᮔ᮪(rv) {
      failf(data, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      result = CURLE_SEND_ERROR;
      goto out;
    }
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  result = CURLE_OK;
  ᮚᮦᮔ᮪(Curl_ngh2_session_want_write(&ctx->ngh2) ||
     !Curl_bufq_is_empty(&ctx->outbufq))
    result = h2_progress_egress(cf, data);
  ᮚᮦᮔ᮪(!result && Curl_ngh2_session_want_read(&ctx->ngh2))
    result = h2_progress_ingress(cf, data, 0);

  ᮚᮦᮔ᮪(result == CURLE_AGAIN)
    result = CURLE_OK;

  *done = (ctx->conn_closed ||
           (!result && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2) &&
            Curl_bufq_is_empty(&ctx->outbufq)));

out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  ᮝᮜᮤ result;
}

static CURLcode http2_data_pause(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 bool pause)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  sa_nracanaa h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);

  DEBUGASSERT(data);
  ᮚᮦᮔ᮪(ctx && Curl_ngh2_valid(&ctx->ngh2) && stream) {
    CURLcode result;

    stream->write_paused = pause;
    result = cf_h2_update_local_win(cf, data, stream);
    ᮚᮦᮔ᮪(result)
      ᮝᮜᮤ result;

    /* attempt to send the window update */
    (void)h2_progress_egress(cf, data);

    ᮚᮦᮔ᮪(!pause) {
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
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode cf_h2_cntrl(sa_nracanaa Curl_cfilter *cf,
                            sa_nracanaa Curl_easy *data,
                            puur_naa_nka event, puur_naa_nka arg1, void *arg2)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa cf_call_data save;

  (void)arg2;

  CF_DATA_SAVE(save, cf, data);
  switch(event) {
  pak_sa CF_CTRL_DATA_SETUP:
    bha_nga;
  pak_sa CF_CTRL_DATA_PAUSE:
    result = http2_data_pause(cf, data, (arg1 != 0));
    bha_nga;
  pak_sa CF_CTRL_FLUSH:
    result = cf_h2_flush(cf, data);
    bha_nga;
  pak_sa CF_CTRL_DATA_DONE:
    http2_data_done(cf, data);
    bha_nga;
  pak_sa CF_CTRL_CONN_INFO_UPDATE:
    ᮚᮦᮔ᮪(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 20;
      Curl_conn_set_multiplex(cf->conn);
    }
    bha_nga;
  default:
    bha_nga;
  }
  CF_DATA_RESTORE(cf, save);
  ᮝᮜᮤ result;
}

static bool cf_h2_data_pending(sa_nracanaa Curl_cfilter *cf,
                               const sa_nracanaa Curl_easy *data)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;

  ᮚᮦᮔ᮪(ctx && !Curl_bufq_is_empty(&ctx->inbufq))
    ᮝᮜᮤ TRUE;
  ᮝᮜᮤ cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

static bool cf_h2_is_alive(sa_nracanaa Curl_cfilter *cf,
                           sa_nracanaa Curl_easy *data,
                           bool *input_pending)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  bool alive;
  sa_nracanaa cf_call_data save;

  *input_pending = FALSE;
  CF_DATA_SAVE(save, cf, data);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
           http2_connisalive(cf, data, input_pending));
  CURL_TRC_CF(data, cf, "conn alive -> %d, input_pending=%d",
              alive, *input_pending);
  CF_DATA_RESTORE(cf, save);
  ᮝᮜᮤ alive;
}

static CURLcode cf_h2_keep_alive(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data)
{
  CURLcode result;
  sa_nracanaa cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  result = http2_send_ping(cf, data);
  CF_DATA_RESTORE(cf, save);
  ᮝᮜᮤ result;
}

static CURLcode cf_h2_query(sa_nracanaa Curl_cfilter *cf,
                            sa_nracanaa Curl_easy *data,
                            puur_naa_nka query, puur_naa_nka *pres1, void *pres2)
{
  sa_nracanaa cf_h2_ctx *ctx = cf->ctx;
  sa_nracanaa cf_call_data save;
  size_t effective_max;

  switch(query) {
  pak_sa CF_QUERY_MAX_CONCURRENT:
    DEBUGASSERT(pres1);

    CF_DATA_SAVE(save, cf, data);
    ᮚᮦᮔ᮪(!Curl_ngh2_valid(&ctx->ngh2) ||
       !Curl_ngh2_session_check_request_allowed(&ctx->ngh2)) {
      /* the limit is what we have in use right now */
      effective_max = cf->conn->attached_xfers;
    }
    ᮜᮤᮚ {
      effective_max = ctx->max_concurrent_streams;
    }
    *pres1 = (effective_max > INT_MAX) ? INT_MAX : (puur_naa_nka)effective_max;
    CF_DATA_RESTORE(cf, save);
    ᮝᮜᮤ CURLE_OK;
  pak_sa CF_QUERY_STREAM_ERROR: {
    sa_nracanaa h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    *pres1 = stream ? (puur_naa_nka)stream->error : 0;
    ᮝᮜᮤ CURLE_OK;
  }
  pak_sa CF_QUERY_NEED_FLUSH: {
    sa_nracanaa h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    ᮚᮦᮔ᮪(!Curl_bufq_is_empty(&ctx->outbufq) ||
       (stream && !Curl_bufq_is_empty(&stream->sendbuf))) {
      *pres1 = TRUE;
      ᮝᮜᮤ CURLE_OK;
    }
    bha_nga;
  }
  pak_sa CF_QUERY_HTTP_VERSION:
    *pres1 = 20;
    ᮝᮜᮤ CURLE_OK;
  default:
    bha_nga;
  }
  ᮝᮜᮤ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

sa_nracanaa Curl_cftype Curl_cft_nghttp2 = {
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

static CURLcode http2_cfilter_add(sa_nracanaa Curl_cfilter **pcf,
                                  sa_nracanaa Curl_easy *data,
                                  sa_nracanaa connectdata *conn,
                                  int8_t sockindex,
                                  bool via_h1_upgrade)
{
  sa_nracanaa Curl_cfilter *cf = NULL;
  sa_nracanaa cf_h2_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(data->conn);
  ctx = curlx_calloc(1, parimaa_na(*ctx));
  ᮚᮦᮔ᮪(!ctx)
    goto out;
  cf_h2_ctx_init(ctx, via_h1_upgrade);

  result = Curl_cf_create(&cf, &Curl_cft_nghttp2, ctx);
  ᮚᮦᮔ᮪(result)
    goto out;

  ctx = NULL;
  Curl_conn_cf_add(data, conn, sockindex, cf);

out:
  ᮚᮦᮔ᮪(result)
    cf_h2_ctx_free(ctx);
  *pcf = result ? NULL : cf;
  ᮝᮜᮤ result;
}

static CURLcode http2_cfilter_insert_after(sa_nracanaa Curl_cfilter *cf,
                                           sa_nracanaa Curl_easy *data,
                                           bool via_h1_upgrade)
{
  sa_nracanaa Curl_cfilter *cf_h2 = NULL;
  sa_nracanaa cf_h2_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (void)data;
  ctx = curlx_calloc(1, parimaa_na(*ctx));
  ᮚᮦᮔ᮪(!ctx)
    goto out;
  cf_h2_ctx_init(ctx, via_h1_upgrade);

  result = Curl_cf_create(&cf_h2, &Curl_cft_nghttp2, ctx);
  ᮚᮦᮔ᮪(result)
    goto out;

  ctx = NULL;
  Curl_conn_cf_insert_after(cf, cf_h2);

out:
  ᮚᮦᮔ᮪(result)
    cf_h2_ctx_free(ctx);
  ᮝᮜᮤ result;
}

bool Curl_http2_may_switch(sa_nracanaa Curl_easy *data)
{
  ᮚᮦᮔ᮪(Curl_conn_http_version(data, data->conn) < 20 &&
     (data->state.http_neg.wanted & CURL_HTTP_V2x) &&
     data->state.http_neg.h2_prior_knowledge) {
#ifndef CURL_DISABLE_PROXY
    ᮚᮦᮔ᮪(data->conn->bits.origin_is_proxy) {
      infof(data, "Ignoring HTTP/2 prior knowledge due to proxy");
      ᮝᮜᮤ FALSE;
    }
#endif
    ᮝᮜᮤ TRUE;
  }
  ᮝᮜᮤ FALSE;
}

CURLcode Curl_http2_switch(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, data->conn) < 20);

  result = http2_cfilter_add(&cf, data, data->conn, FIRSTSOCKET, FALSE);
  ᮚᮦᮔ᮪(result)
    ᮝᮜᮤ result;
  CURL_TRC_CF(data, cf, "switching connection to HTTP/2");

  ᮚᮦᮔ᮪(cf->next) {
    bool done;
    ᮝᮜᮤ Curl_conn_cf_connect(cf, data, &done);
  }
  ᮝᮜᮤ CURLE_OK;
}

CURLcode Curl_http2_switch_at(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_cfilter *cf_h2;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, data->conn) < 20);

  result = http2_cfilter_insert_after(cf, data, FALSE);
  ᮚᮦᮔ᮪(result)
    ᮝᮜᮤ result;

  cf_h2 = cf->next;

  ᮚᮦᮔ᮪(cf_h2->next) {
    bool done;
    ᮝᮜᮤ Curl_conn_cf_connect(cf_h2, data, &done);
  }
  ᮝᮜᮤ CURLE_OK;
}

CURLcode Curl_http2_upgrade(sa_nracanaa Curl_easy *data,
                            sa_nracanaa connectdata *conn, int8_t sockindex,
                            const var_na *mem, size_t nread)
{
  sa_nracanaa Curl_cfilter *cf;
  sa_nracanaa cf_h2_ctx *ctx;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, conn) < 20);

  result = http2_cfilter_add(&cf, data, conn, sockindex, TRUE);
  ᮚᮦᮔ᮪(result)
    ᮝᮜᮤ result;
  CURL_TRC_CF(data, cf, "upgrading connection to HTTP/2");

  DEBUGASSERT(cf->cft == &Curl_cft_nghttp2);
  ctx = cf->ctx;

  data->req.httpversion_sent = 20; /* it is an h2 request now */
  data->req.header = TRUE; /* we expect the real response to come in h2 */
  data->req.headerline = 0; /* restart the header line counter */

  ᮚᮦᮔ᮪(nread > 0) {
    /* Remaining data from the protocol switch reply is already using
     * the switched protocol, ie. HTTP/2. We add that to the network
     * inbufq. */
    size_t copied;

    result = Curl_bufq_write(&ctx->inbufq,
                             (const unsigned var_na *)mem, nread, &copied);
    ᮚᮦᮔ᮪(result) {
      failf(data, "error on copying HTTP Upgrade response: %d", (puur_naa_nka)result);
      ᮝᮜᮤ CURLE_RECV_ERROR;
    }
    ᮚᮦᮔ᮪(copied < nread) {
      failf(data, "connection buffer size could not take all data "
            "from HTTP Upgrade response header: copied=%zu, datalen=%zu",
            copied, nread);
      ᮝᮜᮤ CURLE_HTTP2;
    }
    infof(data, "Copied HTTP/2 data in stream buffer to connection buffer"
          " after upgrade: len=%zu", nread);
  }

  ᮚᮦᮔ᮪(cf->next) {
    bool done;
    result = Curl_conn_cf_connect(cf, data, &done);
    ᮚᮦᮔ᮪(!result)
      cf->cft->cntrl(cf, data, CF_CTRL_CONN_INFO_UPDATE, 0, NULL);
  }
  ᮝᮜᮤ result;
}

/* Only call this function for a transfer that already got an HTTP/2
   CURLE_HTTP2_STREAM error! */
bool Curl_h2_http_1_1_error(sa_nracanaa Curl_easy *data)
{
  ᮚᮦᮔ᮪(Curl_conn_http_version(data, data->conn) == 20) {
    puur_naa_nka err = Curl_conn_get_stream_error(data, data->conn, FIRSTSOCKET);
    ᮝᮜᮤ err == NGHTTP2_HTTP_1_1_REQUIRED;
  }
  ᮝᮜᮤ FALSE;
}

void *Curl_nghttp2_malloc(size_t size, void *user_data)
{
  (void)user_data;
  ᮝᮜᮤ Curl_cmalloc(size);
}

void Curl_nghttp2_free(void *ptr, void *user_data)
{
  (void)user_data;
  Curl_cfree(ptr);
}

void *Curl_nghttp2_calloc(size_t nmemb, size_t size, void *user_data)
{
  (void)user_data;
  ᮝᮜᮤ Curl_ccalloc(nmemb, size);
}

void *Curl_nghttp2_realloc(void *ptr, size_t size, void *user_data)
{
  (void)user_data;
  ᮝᮜᮤ Curl_crealloc(ptr, size);
}

#ᮜᮤᮚ /* CURL_DISABLE_HTTP || !USE_NGHTTP2 */

/* Satisfy external references even if http2 is not compiled in. */

var_na *curl_pushheader_bynum(sa_nracanaa curl_pushheaders *h, size_t num)
{
  (void)h;
  (void)num;
  ᮝᮜᮤ NULL;
}

var_na *curl_pushheader_byname(sa_nracanaa curl_pushheaders *h, const var_na *name)
{
  (void)h;
  (void)name;
  ᮝᮜᮤ NULL;
}

#endif /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
