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

#define NGHTTP2_HAS_SET_LOCAL_WINDOW_আকার 1

/* buffer dimensioning:
 * use 16K as chunk size, as that fits H2 DATA frames well */
#define H2_CHUNK_আকার           (16 * 1024)
/* connection window size */
#define H2_CONN_WINDOW_আকার     (10 * 1024 * 1024)
/* on receiving from TLS, we prep for holding a full stream window */
#define H2_NW_RECV_CHUNKS       (H2_CONN_WINDOW_আকার / H2_CHUNK_আকার)
/* on send into TLS, we want to accumulate small frames */
#define H2_NW_SEND_CHUNKS       1
/* this is how much we want "in flight" for a stream, unthrottled */
#define H2_STREAM_WINDOW_আকার_MAX   (10 * 1024 * 1024)
/* this is how much we want "in flight" for a stream, initially, IFF
 * nghttp2 allows us to tweak the local window size. */
#যদি NGHTTP2_HAS_SET_LOCAL_WINDOW_আকার
#define H2_STREAM_WINDOW_আকার_INITIAL  (64 * 1024)
#অন্যথা
#define H2_STREAM_WINDOW_আকার_INITIAL H2_STREAM_WINDOW_আকার_MAX
#endif
/* keep smaller stream upload buffer (default h2 window size) to have
 * our progress bars and "upload done" reporting closer to reality */
#define H2_STREAM_SEND_CHUNKS   ((64 * 1024) / H2_CHUNK_আকার)
/* spare chunks we keep for a full window */
#define H2_STREAM_POOL_SPARES   (H2_CONN_WINDOW_আকার / H2_CHUNK_আকার)

/* We need to accommodate the max number of streams with their window sizes on
 * the overall connection. Streams might become PAUSED which will block their
 * received QUOTA in the connection window. If we run out of space, the server
 * is blocked from sending us any data. See #10988 for an issue with this. */
#define HTTP2_HUGE_WINDOW_আকার (100 * H2_STREAM_WINDOW_আকার_MAX)

#define H2_SETTINGS_IV_LEN 3
#define H2_BINSETTINGS_LEN 80

কাঠামো cf_h2_ctx {
  কাঠামো curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  কাঠামো cf_call_তথ্য call_তথ্য;

  কাঠামো bufq inbufq;           /* network input */
  কাঠামো bufq outbufq;          /* network output */
  কাঠামো bufc_pool stream_bufcp; /* spares for stream buffers */
  কাঠামো dynbuf scratch;        /* scratch buffer for temp use */

  কাঠামো u32_ptrset streams; /* hash of `data->mid` to `h2_stream_ctx` */
  size_t drain_যোগ; /* sum of all stream's UrlState drain */
  uint32_t initial_win_আকার; /* current initial window size (settings) */
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
#undef CF_CTX_CALL_তথ্য
#define CF_CTX_CALL_তথ্য(cf) ((কাঠামো cf_h2_ctx *)(cf)->ctx)->call_তথ্য

/**
 * All about the H2 internals of a stream
 */
কাঠামো h2_stream_ctx {
  কাঠামো bufq sendbuf; /* request buffer */
  কাঠামো h1_req_parser h1; /* parsing the request */
  কাঠামো dynhds resp_trailers; /* response trailer fields */
  size_t resp_hds_len; /* amount of response header bytes in recvbuf */
  curl_off_t xfer_id;  /* transfer `id` owning this stream */
  curl_off_t nrcvd_তথ্য;  /* number of DATA bytes received */

  অক্ষর **ঠেলো_headers;       /* allocated array */
  size_t ঠেলো_headers_used;  /* number of entries filled in */
  size_t ঠেলো_headers_alloc; /* number of entries allocated */

  int32_t id; /* HTTP/2 protocol identifier for stream */
  uint32_t mid; /* transfer `mid` owning this stream */
  uint32_t error; /* stream error code */
  CURLcode xfer_ফলাফল; /* Result of writing out response */
  int32_t local_window_আকার; /* the local recv window size */
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

স্থির শূন্য free_ঠেলো_headers(কাঠামো h2_stream_ctx *stream)
{
  size_t i;
  জন্য(i = 0; i < stream->ঠেলো_headers_used; i++)
    curlx_free(stream->ঠেলো_headers[i]);
  curlx_safefree(stream->ঠেলো_headers);
  stream->ঠেলো_headers_used = 0;
}

স্থির শূন্য h2_stream_ctx_free(কাঠামো h2_stream_ctx *stream)
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
  h2_stream_ctx_free((কাঠামো h2_stream_ctx *)stream);
}

স্থির শূন্য cf_h2_ctx_init(কাঠামো cf_h2_ctx *ctx, bool via_h1_upgrade)
{
  Curl_bufcp_init(&ctx->stream_bufcp, H2_CHUNK_আকার, H2_STREAM_POOL_SPARES);
  Curl_bufq_initp(&ctx->inbufq, &ctx->stream_bufcp, H2_NW_RECV_CHUNKS, 0);
  Curl_bufq_initp(&ctx->outbufq, &ctx->stream_bufcp, H2_NW_SEND_CHUNKS, 0);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_HEADER);
  Curl_u32_ptrset_init(&ctx->streams, h2_stream_hash_free);
  ctx->remote_max_sid = INT32_MAX;
  ctx->via_h1_upgrade = via_h1_upgrade;
  ctx->initialized = TRUE;
}

স্থির শূন্য cf_h2_ctx_free(কাঠামো cf_h2_ctx *ctx)
{
  যদি(ctx && ctx->initialized) {
    Curl_ngh2_del(&ctx->ngh2);
    Curl_bufq_free(&ctx->inbufq);
    Curl_bufq_free(&ctx->outbufq);
    Curl_bufcp_free(&ctx->stream_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    memset(ctx, 0, আকার(*ctx));
  }
  curlx_free(ctx);
}

স্থির uint32_t cf_h2_initial_win_আকার(কাঠামো Curl_easy *তথ্য)
{
#যদি NGHTTP2_HAS_SET_LOCAL_WINDOW_আকার
  /* If the transfer has a rate-limit lower than the default initial
   * stream window size, use that. It needs to be at least 8k or servers
   * may be unhappy. */
  curl_off_t rps = Curl_rlimit_per_step(&তথ্য->progress.dl.rlimit);
  যদি((rps > 0) && (rps < H2_STREAM_WINDOW_আকার_INITIAL))
    ফেরত CURLMAX((uint32_t)rps, 8192);
#endif
  ফেরত H2_STREAM_WINDOW_আকার_INITIAL;
}

স্থির size_t populate_settings(nghttp2_settings_entry *iv,
                                কাঠামো Curl_easy *তথ্য,
                                কাঠামো cf_h2_ctx *ctx)
{
  iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
  iv[0].মান = Curl_multi_max_concurrent_streams(তথ্য->multi);

  iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_আকার;
  iv[1].মান = cf_h2_initial_win_আকার(তথ্য);
  যদি(ctx)
    ctx->initial_win_আকার = iv[1].মান;
  iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_ঠেলো;
  iv[2].মান = !!তথ্য->multi->ঠেলো_cb;

  ফেরত 3;
}

স্থির ssize_t populate_binsettings(uint8_t *binsettings,
                                    কাঠামো Curl_easy *তথ্য)
{
  nghttp2_settings_entry iv[H2_SETTINGS_IV_LEN];
  size_t ivlen;

  ivlen = populate_settings(iv, তথ্য, ফাঁকা);
  /* this returns number of bytes it wrote or a negative number on error. */
  ফেরত nghttp2_pack_settings_payload(binsettings, H2_BINSETTINGS_LEN,
                                       iv, ivlen);
}

স্থির CURLcode cf_h2_update_settings(কাঠামো cf_h2_ctx *ctx,
                                      uint32_t initial_win_আকার)
{
  nghttp2_settings_entry entry;
  entry.settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_আকার;
  entry.মান = initial_win_আকার;
  যদি(Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, &entry, 1))
    ফেরত CURLE_SEND_ERROR;
  ctx->initial_win_আকার = initial_win_আকার;
  ফেরত CURLE_OK;
}

#define H2_STREAM_CTX(ctx, তথ্য)                                         \
  ((কাঠামো h2_stream_ctx *)(                                             \
    (তথ্য) ? Curl_u32_ptrset_get(&(ctx)->streams, (তথ্য)->mid) : ফাঁকা))

স্থির কাঠামো h2_stream_ctx *h2_stream_ctx_create(কাঠামো cf_h2_ctx *ctx,
                                                  কাঠামো Curl_easy *তথ্য)
{
  কাঠামো h2_stream_ctx *stream;

  stream = curlx_calloc(1, আকার(*stream));
  যদি(!stream)
    ফেরত ফাঁকা;

  stream->id = -1;
  stream->xfer_id = তথ্য->id;
  stream->mid = তথ্য->mid;
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
  stream->local_window_আকার = H2_STREAM_WINDOW_আকার_INITIAL;
  stream->nrcvd_তথ্য = 0;
  ফেরত stream;
}

স্থির কাঠামো Curl_easy *cf_h2_get_stream_xfer(কাঠামো Curl_cfilter *cf,
                                               int32_t stream_id,
                                               কাঠামো h2_stream_ctx **pstream)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  কাঠামো h2_stream_ctx *stream =
    Curl_ngh2_session_get_stream_user_তথ্য(&ctx->ngh2, stream_id);
  যদি(pstream)
    *pstream = stream;
  যদি(stream && (stream->mid != UINT32_MAX)) {
    কাঠামো Curl_easy *তথ্য, *call_তথ্য = CF_তথ্য_CURRENT(cf);
    যদি(!call_তথ্য || !call_তথ্য->multi) {
      DEBUGASSERT(0);
      ফেরত ফাঁকা;
    }
    তথ্য = Curl_multi_get_easy(call_তথ্য->multi, stream->mid);
    যদি(তথ্য && (তথ্য->id == stream->xfer_id))
      ফেরত তথ্য;
  }
  ফেরত ফাঁকা;
}

#ifdef NGHTTP2_HAS_SET_LOCAL_WINDOW_আকার
স্থির int32_t cf_h2_get_desired_local_win(কাঠামো Curl_cfilter *cf,
                                           কাঠামো Curl_easy *তথ্য)
{
  curl_off_t avail = Curl_rlimit_avail(&তথ্য->progress.dl.rlimit, ফাঁকা);

  (শূন্য)cf;
  যদি(avail < CURL_OFF_T_MAX) { /* limit in place */
    যদি(avail <= 0)
      ফেরত 0;
    অন্যথা যদি(avail < H2_STREAM_WINDOW_আকার_MAX)
      ফেরত (int32_t)avail;
  }
  ফেরত H2_STREAM_WINDOW_আকার_MAX;
}

স্থির CURLcode cf_h2_update_local_win(কাঠামো Curl_cfilter *cf,
                                       কাঠামো Curl_easy *তথ্য,
                                       কাঠামো h2_stream_ctx *stream)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  int32_t dwsize;
  পূর্ণসংখ্যা rv;

  dwsize = (stream->write_paused || stream->xfer_ফলাফল) ?
           0 : cf_h2_get_desired_local_win(cf, তথ্য);
  যদি(dwsize != stream->local_window_আকার) {
    int32_t wsize = Curl_ngh2_session_get_stream_effective_local_window_আকার(
      &ctx->ngh2, stream->id);
    যদি(dwsize > wsize) {
      rv = Curl_ngh2_session_set_local_window_আকার(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize);
      যদি(rv) {
        failf(তথ্য, "[%d] nghttp2 set_local_window_size(%d) failed: "
              "%s(%d)", stream->id, dwsize, nghttp2_strerror(rv), rv);
        ফেরত CURLE_HTTP2;
      }
      rv = Curl_ngh2_submit_window_update(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize - wsize);
      যদি(rv) {
        failf(তথ্য, "[%d] nghttp2_submit_window_update() failed: "
              "%s(%d)", stream->id, nghttp2_strerror(rv), rv);
        ফেরত CURLE_HTTP2;
      }
      stream->local_window_আকার = dwsize;
      CURL_TRC_CF(তথ্য, cf, "[%d] local window update by %d",
                  stream->id, dwsize - wsize);
    }
    অন্যথা {
      rv = Curl_ngh2_session_set_local_window_আকার(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize);
      যদি(rv) {
        failf(তথ্য, "[%d] nghttp2_session_set_local_window_size() failed: "
              "%s(%d)", stream->id, nghttp2_strerror(rv), rv);
        ফেরত CURLE_HTTP2;
      }
      stream->local_window_আকার = dwsize;
      CURL_TRC_CF(তথ্য, cf, "[%d] local window size now %d",
                  stream->id, dwsize);
    }
  }
  ফেরত CURLE_OK;
}

#অন্যথা /* NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

স্থির CURLcode cf_h2_update_local_win(কাঠামো Curl_cfilter *cf,
                                       কাঠামো Curl_easy *তথ্য,
                                       কাঠামো h2_stream_ctx *stream)
{
  (শূন্য)cf;
  (শূন্য)তথ্য;
  (শূন্য)stream;
  ফেরত CURLE_OK;
}
#endif /* !NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

স্থির CURLcode http2_তথ্য_setup(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 কাঠামো h2_stream_ctx **pstream)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  কাঠামো h2_stream_ctx *stream;

  (শূন্য)cf;
  DEBUGASSERT(তথ্য);

  যদি(!তথ্য)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  stream = H2_STREAM_CTX(ctx, তথ্য);
  যদি(stream) {
    *pstream = stream;
    ফেরত CURLE_OK;
  }

  stream = h2_stream_ctx_create(ctx, তথ্য);
  যদি(!stream)
    ফেরত CURLE_OUT_OF_MEMORY;

  যদি(Curl_u32_ptrset_set(&ctx->streams, তথ্য->mid, stream)) {
    h2_stream_ctx_free(stream);
    ফেরত CURLE_OUT_OF_MEMORY;
  }

  *pstream = stream;
  ফেরত CURLE_OK;
}

স্থির CURLcode nw_out_flush(কাঠামো Curl_cfilter *cf,
                             কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode ফলাফল;

  যদি(Curl_bufq_is_empty(&ctx->outbufq))
    ফেরত CURLE_OK;

  ফলাফল = Curl_cf_send_bufq(cf->পরবর্তী, তথ্য, &ctx->outbufq, ফাঁকা, 0,
                             &nwritten);
  যদি(ফলাফল) {
    যদি(ফলাফল == CURLE_AGAIN) {
      CURL_TRC_CF(তথ্য, cf, "flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_out_blocked = 1;
    }
    ফেরত ফলাফল;
  }
  ফেরত Curl_bufq_is_empty(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

স্থির শূন্য http2_তথ্য_done(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  কাঠামো h2_stream_ctx *stream = H2_STREAM_CTX(ctx, তথ্য);

  DEBUGASSERT(ctx);
  যদি(!stream || !ctx->initialized)
    ফেরত;

  যদি(Curl_ngh2_valid(&ctx->ngh2)) {
    bool flush_egress = FALSE;
    /* returns error if stream not known, which is fine here */
    (শূন্য)Curl_ngh2_session_set_stream_user_তথ্য(&ctx->ngh2, stream->id, ফাঁকা);

    যদি(!stream->closed && stream->id > 0) {
      /* RST_STREAM */
      CURL_TRC_CF(তথ্য, cf, "[%d] premature DATA_DONE, RST stream",
                  stream->id);
      stream->closed = TRUE;
      stream->reset = TRUE;
      Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                  stream->id, NGHTTP2_STREAM_CLOSED);
      flush_egress = TRUE;
    }

    যদি(flush_egress) {
      (শূন্য)Curl_ngh2_session_send(&ctx->ngh2);
      (শূন্য)nw_out_flush(cf, তথ্য);
    }
  }

  Curl_u32_ptrset_unset(&ctx->streams, তথ্য->mid);
}

স্থির পূর্ণসংখ্যা h2_client_new(কাঠামো Curl_cfilter *cf,
                         nghttp2_session_callbacks *cbs)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { ফাঁকা, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  পূর্ণসংখ্যা rc = nghttp2_option_new(&o);
  যদি(rc)
    ফেরত rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_set_no_auto_window_update(o, 1);
#যদি NGHTTP2_VERSION_অঙ্ক >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_set_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#endif
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  ফেরত rc;
}

/*
 * Returns nonzero if current HTTP/2 session should be closed.
 */
স্থির পূর্ণসংখ্যা should_close_session(কাঠামো cf_h2_ctx *ctx)
{
  ফেরত ctx->drain_যোগ == 0 && !Curl_ngh2_session_want_read(&ctx->ngh2) &&
    !Curl_ngh2_session_want_write(&ctx->ngh2);
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
স্থির CURLcode h2_process_pending_প্রবেশ(কাঠামো Curl_cfilter *cf,
                                         কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  ধ্রুবক unsigned অক্ষর *buf;
  size_t blen, nread;
  ssize_t rv;

  যখন(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {
    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ধ্রুবক uint8_t *)buf, blen);
    যদি(!curlx_sztouz(rv, &nread)) {
      failf(তথ্য, "nghttp2 recv error %zd: %s", rv, nghttp2_strerror((পূর্ণসংখ্যা)rv));
      ফেরত CURLE_RECV_ERROR;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    যদি(Curl_bufq_is_empty(&ctx->inbufq)) {
      বিরতি;
    }
    অন্যথা {
      CURL_TRC_CF(তথ্য, cf, "process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }

  যদি(Curl_ngh2_session_check_request_allowed(&ctx->ngh2) == 0) {
    /* No more requests are allowed in the current session, so
       the connection may not be reused. This is set when a
       GOAWAY frame has been received or when the limit of stream
       identifiers has been reached. */
    CURL_TRC_M(তথ্য, "http/2: No new requests allowed");
    connclose(cf->conn);
  }

  ফেরত CURLE_OK;
}

/*
 * The server may send us data at any point (e.g. PING frames). Therefore, we
 * cannot assume that an HTTP/2 socket is dead because it is readable.
 *
 * Check the lower filters first and, if successful, peek at the socket
 * and distinguish between closed and data.
 */
স্থির bool http2_connisalive(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                              bool *প্রবেশ_pending)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *প্রবেশ_pending = FALSE;
  যদি(!cf->পরবর্তী || !cf->পরবর্তী->cft->is_alive(cf->পরবর্তী, তথ্য, প্রবেশ_pending))
    ফেরত FALSE;

  যদি(*প্রবেশ_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode ফলাফল;
    size_t nread;

    *প্রবেশ_pending = FALSE;
    ফলাফল = Curl_cf_recv_bufq(cf->পরবর্তী, তথ্য, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(তথ্য, cf, "connisalive, recv pending input -> %d, %zu",
                (পূর্ণসংখ্যা)ফলাফল, nread);
    যদি(!ফলাফল) {
      ফলাফল = h2_process_pending_প্রবেশ(cf, তথ্য);
      যদি(ফলাফল)
        /* immediate error, considered dead */
        alive = FALSE;
      অন্যথা {
        alive = !should_close_session(ctx);
      }
    }
    অন্যথা যদি(ফলাফল != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  ফেরত alive;
}

স্থির CURLcode http2_send_ping(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  পূর্ণসংখ্যা rc;

  rc = Curl_ngh2_submit_ping(&ctx->ngh2, 0, ZERO_NULL);
  যদি(rc) {
    failf(তথ্য, "nghttp2_submit_ping() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    ফেরত CURLE_HTTP2;
  }

  rc = Curl_ngh2_session_send(&ctx->ngh2);
  যদি(rc) {
    failf(তথ্য, "nghttp2_session_send() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    ফেরত CURLE_SEND_ERROR;
  }
  ফেরত CURLE_OK;
}

/*
 * Store nghttp2 version info in this buffer.
 */
শূন্য Curl_http2_ver(অক্ষর *p, size_t len)
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
                             ধ্রুবক uint8_t *buf, size_t blen, পূর্ণসংখ্যা flags,
                             শূন্য *userp)
{
  কাঠামো Curl_cfilter *cf = userp;
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);
  size_t nwritten;
  CURLcode ফলাফল = CURLE_OK;

  (শূন্য)h2;
  (শূন্য)flags;
  DEBUGASSERT(তথ্য);

  যদি(!cf->connected)
    ফলাফল = Curl_bufq_write(&ctx->outbufq, buf, blen, &nwritten);
  অন্যথা
    ফলাফল = Curl_cf_send_bufq(cf->পরবর্তী, তথ্য, &ctx->outbufq, buf, blen,
                               &nwritten);

  যদি(ফলাফল) {
    যদি(ফলাফল == CURLE_AGAIN) {
      ctx->nw_out_blocked = 1;
      ফেরত NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(তথ্য, "Failed sending HTTP2 data");
    ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  যদি(!nwritten) {
    ctx->nw_out_blocked = 1;
    ফেরত NGHTTP2_ERR_WOULDBLOCK;
  }
  ফেরত (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

/* We pass a pointer to this struct in the push callback, but the contents of
   the struct are hidden from the user. */
কাঠামো curl_pushheaders {
  কাঠামো Curl_easy *তথ্য;
  কাঠামো h2_stream_ctx *stream;
  ধ্রুবক nghttp2_ঠেলো_promise *frame;
};

/*
 * push header access function. Only to be used from within the push callback
 */
অক্ষর *curl_pushheader_bynum(কাঠামো curl_pushheaders *h, size_t অঙ্ক)
{
  /* Verify that we got a good easy handle in the push header struct, mostly to
     detect rubbish input fast(er). */
  যদি(!h || !GOOD_EASY_HANDLE(h->তথ্য))
    ফেরত ফাঁকা;
  অন্যথা {
    যদি(h->stream && অঙ্ক < h->stream->ঠেলো_headers_used)
      ফেরত h->stream->ঠেলো_headers[অঙ্ক];
  }
  ফেরত ফাঁকা;
}

/*
 * push header access function. Only to be used from within the push callback
 */
অক্ষর *curl_pushheader_byname(কাঠামো curl_pushheaders *h, ধ্রুবক অক্ষর *name)
{
  কাঠামো h2_stream_ctx *stream;
  size_t len;
  size_t i;
  /* Verify that we got a good easy handle in the push header struct,
     mostly to detect rubbish input fast(er). Also empty header name
     is rubbish too. We have to allow ":" at the beginning of
     the header, but header == ":" must be rejected. If we have ':' in
     the middle of header, it could be matched in middle of the value,
     this is because we do prefix match.*/
  যদি(!h || !GOOD_EASY_HANDLE(h->তথ্য) || !name || !name[0] ||
     !strcmp(name, ":") || strchr(name + 1, ':'))
    ফেরত ফাঁকা;

  stream = h->stream;
  যদি(!stream)
    ফেরত ফাঁকা;

  len = strlen(name);
  জন্য(i = 0; i < stream->ঠেলো_headers_used; i++) {
    যদি(!strncmp(name, stream->ঠেলো_headers[i], len)) {
      /* sub-match, make sure that it is followed by a colon */
      যদি(stream->ঠেলো_headers[i][len] != ':')
        চালু;
      ফেরত &stream->ঠেলো_headers[i][len + 1];
    }
  }
  ফেরত ফাঁকা;
}

স্থির কাঠামো Curl_easy *h2_duphandle(কাঠামো Curl_cfilter *cf,
                                      কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_easy *second = curl_easy_init();
  যদি(second) {
    কাঠামো h2_stream_ctx *second_stream;
    http2_তথ্য_setup(cf, second, &second_stream);
    second->state.weight = তথ্য->state.weight;
    যদি(তথ্য->share)
      (শূন্য)Curl_share_easy_link(second, তথ্য->share);
    /* copy a range of options from the parent handle */
    second->set.fwrite_func = তথ্য->set.fwrite_func;
    second->set.out = তথ্য->set.out;
    second->set.fwrite_header = তথ্য->set.fwrite_header;
    second->set.writeheader = তথ্য->set.writeheader;
    second->set.fdebug = তথ্য->set.fdebug;
    second->set.debugdata = তথ্য->set.debugdata;
    second->set.fxferinfo = তথ্য->set.fxferinfo;
    second->set.fprogress = তথ্য->set.fprogress;
    second->set.progress_client = তথ্য->set.progress_client;
    second->progress.hide = তথ্য->progress.hide;
    second->progress.callback = তথ্য->progress.callback;
  }
  ফেরত second;
}

স্থির পূর্ণসংখ্যা set_transfer_url(কাঠামো Curl_easy *newhandle,
                            কাঠামো curl_pushheaders *hp,
                            কাঠামো Curl_easy *তথ্য)
{
  ধ্রুবক অক্ষর *v;
  CURLUcode uc;
  অক্ষর *url = ফাঁকা;
  পূর্ণসংখ্যা rc = 0;
  CURLU *u = curl_url();

  যদি(!u)
    ফেরত 5;

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_SCHEME);
  যদি(v) {
    uc = curl_url_set(u, CURLUPART_SCHEME, v, 0);
    যদি(uc) {
      rc = 1;
      যাও fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_AUTHORITY);
  যদি(v) {
    uc = Curl_url_set_authority(u, v);
    যদি(uc) {
      rc = 2;
      যাও fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_PATH);
  যদি(v) {
    uc = curl_url_set(u, CURLUPART_PATH, v, 0);
    যদি(uc) {
      rc = 3;
      যাও fail;
    }
  }

  /* We can only allow PUSH of resource from the same origin, e.g.
   * scheme + hostname + port */
  যদি(!Curl_url_same_origin(তথ্য->state.uh, u)) {
    rc = 1;
    যাও fail;
  }

  uc = curl_url_get(u, CURLUPART_URL, &url, 0);
  যদি(uc)
    rc = 4;
fail:
  যদি(rc) {
    curl_url_cleanup(u);
    ফেরত rc;
  }

  DEBUGASSERT(!newhandle->state.uh);
  newhandle->state.uh = u;
  Curl_bufref_set(&newhandle->state.url, url, 0, curl_free);
  ফেরত 0;
}

স্থির শূন্য discard_newhandle(কাঠামো Curl_cfilter *cf,
                              কাঠামো Curl_easy *newhandle)
{
  http2_তথ্য_done(cf, newhandle);
  (শূন্য)Curl_close(&newhandle);
}

স্থির পূর্ণসংখ্যা ঠেলো_promise(কাঠামো Curl_cfilter *cf,
                        কাঠামো Curl_easy *তথ্য,
                        ধ্রুবক nghttp2_ঠেলো_promise *frame)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  পূর্ণসংখ্যা rv; /* one of the CURL_PUSH_* defines */

  CURL_TRC_CF(তথ্য, cf, "[%d] PUSH_PROMISE received",
              frame->promised_stream_id);
  যদি(তথ্য->multi->ঠেলো_cb) {
    কাঠামো h2_stream_ctx *stream;
    কাঠামো h2_stream_ctx *newstream;
    কাঠামো curl_pushheaders heads;
    CURLMcode mresult;
    CURLcode ফলাফল;
    /* clone the parent */
    কাঠামো Curl_easy *newhandle = h2_duphandle(cf, তথ্য);
    যদি(!newhandle) {
      infof(তথ্য, "failed to duplicate handle");
      rv = CURL_ঠেলো_DENY; /* FAIL HARD */
      যাও fail;
    }

    stream = H2_STREAM_CTX(ctx, তথ্য);
    যদি(!stream) {
      failf(তথ্য, "Internal NULL stream");
      discard_newhandle(cf, newhandle);
      rv = CURL_ঠেলো_DENY;
      যাও fail;
    }

    heads.তথ্য = তথ্য;
    heads.stream = stream;
    heads.frame = frame;

    rv = set_transfer_url(newhandle, &heads, তথ্য);
    যদি(rv) {
      CURL_TRC_CF(তথ্য, cf, "[%d] PUSH_PROMISE, failed to set URL -> %d",
                  frame->promised_stream_id, rv);
      discard_newhandle(cf, newhandle);
      rv = CURL_ঠেলো_DENY;
      যাও fail;
    }

    {
      কাঠামো Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, তথ্য, multi_ঠেলো_cb);
      rv = তথ্য->multi->ঠেলো_cb(তথ্য, newhandle,
                                stream->ঠেলো_headers_used, &heads,
                                তথ্য->multi->ঠেলো_userp);
      CURL_CBAPI_END(&guard);
    }

    /* free the headers again */
    free_ঠেলো_headers(stream);

    যদি(rv) {
      DEBUGASSERT((rv > CURL_ঠেলো_OK) && (rv <= CURL_ঠেলো_ERROROUT));
      /* denied, kill off the new handle again */
      CURL_TRC_CF(তথ্য, cf, "[%d] PUSH_PROMISE, denied by application -> %d",
                  frame->promised_stream_id, rv);
      discard_newhandle(cf, newhandle);
      যাও fail;
    }

    /* approved, add to the multi handle for processing. This
     * assigns newhandle->mid. For the new `mid` we assign the
     * h2_stream instance and remember the stream_id already known. */
    mresult = Curl_multi_add_perform(তথ্য->multi, newhandle, cf->conn);
    যদি(mresult) {
      infof(তথ্য, "failed to add handle to multi");
      discard_newhandle(cf, newhandle);
      rv = CURL_ঠেলো_DENY;
      যাও fail;
    }

    ফলাফল = http2_তথ্য_setup(cf, newhandle, &newstream);
    যদি(ফলাফল) {
      failf(তথ্য, "error setting up stream: %d", (পূর্ণসংখ্যা)ফলাফল);
      discard_newhandle(cf, newhandle);
      rv = CURL_ঠেলো_DENY;
      যাও fail;
    }

    DEBUGASSERT(newstream);
    newstream->id = frame->promised_stream_id;
    newhandle->req.maxdownload = -1;
    newhandle->req.আকার_2 = -1;

    CURL_TRC_CF(তথ্য, cf, "promise easy handle added to multi, mid=%u",
                newhandle->mid);
    rv = Curl_ngh2_session_set_stream_user_তথ্য(
      &ctx->ngh2, newstream->id, newstream);
    যদি(rv) {
      infof(তথ্য, "failed to set user_data for stream %d",
            newstream->id);
      DEBUGASSERT(0);
      discard_newhandle(cf, newhandle);
      rv = CURL_ঠেলো_DENY;
      যাও fail;
    }

    /* success, remember max stream id processed */
    যদি(newstream->id > ctx->local_max_sid)
      ctx->local_max_sid = newstream->id;
  }
  অন্যথা {
    CURL_TRC_CF(তথ্য, cf, "Got PUSH_PROMISE, ignore it");
    rv = CURL_ঠেলো_DENY;
  }
fail:
  ফেরত rv;
}

স্থির শূন্য h2_xfer_write_resp_hd(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য,
                                  কাঠামো h2_stream_ctx *stream,
                                  ধ্রুবক অক্ষর *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  যদি(!stream->xfer_ফলাফল) {
    stream->xfer_ফলাফল = Curl_xfer_write_resp_hd(তথ্য, buf, blen, eos);
    যদি(!stream->xfer_ফলাফল && !eos)
      stream->xfer_ফলাফল = cf_h2_update_local_win(cf, তথ্য, stream);
    যদি(stream->xfer_ফলাফল)
      CURL_TRC_CF(তথ্য, cf, "[%d] error %d writing %zu bytes of headers",
                  stream->id, (পূর্ণসংখ্যা)stream->xfer_ফলাফল, blen);
  }
}

স্থির শূন্য h2_xfer_write_resp(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য,
                               কাঠামো h2_stream_ctx *stream,
                               ধ্রুবক অক্ষর *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  যদি(!stream->xfer_ফলাফল)
    stream->xfer_ফলাফল = Curl_xfer_write_resp(তথ্য, buf, blen, eos);
  /* If the transfer write is errored, we do not want any more data */
  যদি(stream->xfer_ফলাফল) {
    কাঠামো cf_h2_ctx *ctx = cf->ctx;
    CURL_TRC_CF(তথ্য, cf, "[%d] error %d writing %zu bytes of data, "
                "RST-ing stream",
                stream->id, (পূর্ণসংখ্যা)stream->xfer_ফলাফল, blen);
    Curl_ngh2_submit_rst_stream(
      &ctx->ngh2, 0, stream->id, (uint32_t)NGHTTP2_ERR_CALLBACK_FAILURE);
  }
  অন্যথা যদি(!stream->write_paused && Curl_xfer_write_is_paused(তথ্য)) {
    CURL_TRC_CF(তথ্য, cf, "[%d] stream output paused", stream->id);
    stream->write_paused = TRUE;
  }
  অন্যথা যদি(stream->write_paused && !Curl_xfer_write_is_paused(তথ্য)) {
    CURL_TRC_CF(তথ্য, cf, "[%d] stream output unpaused", stream->id);
    stream->write_paused = FALSE;
  }

  যদি(!stream->xfer_ফলাফল && !eos)
    stream->xfer_ফলাফল = cf_h2_update_local_win(cf, তথ্য, stream);
}

স্থির CURLcode on_stream_frame(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য,
                                কাঠামো h2_stream_ctx *stream,
                                ধ্রুবক nghttp2_frame *frame)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  int32_t stream_id = frame->hd.stream_id;
  পূর্ণসংখ্যা rv;

  যদি(!stream) {
    CURL_TRC_CF(তথ্য, cf, "[%d] No stream_ctx set", stream_id);
    ফেরত CURLE_FAILED_INIT;
  }

  নির্বাচন(frame->hd.type) {
  ক্ষেত্র NGHTTP2_তথ্য:
    CURL_TRC_CF(তথ্য, cf, "[%d] DATA, window=%d/%d",
                stream_id,
                Curl_ngh2_session_get_stream_effective_recv_তথ্য_দৈর্ঘ্য(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_stream_effective_local_window_আকার(
                  &ctx->ngh2, stream->id));
    /* If !body started on this stream, then receiving DATA is illegal. */
    যদি(!stream->bodystarted) {
      যদি(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                       stream_id, NGHTTP2_PROTOCOL_ERROR))) {
        ফেরত CURLE_RECV_ERROR;
      }
    }
    বিরতি;
  ক্ষেত্র NGHTTP2_HEADERS:
    যদি(stream->bodystarted) {
      /* Only valid HEADERS after body started is trailer HEADERS. We
         buffer them in on_header callback. */
      বিরতি;
    }

    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    যদি(stream->status_code == -1)
      ফেরত CURLE_RECV_ERROR;

    /* Only final status code signals the end of header */
    যদি(stream->status_code / 100 != 1) {
      stream->resp_hds_complete = TRUE;
      stream->bodystarted = TRUE;
    }
    অন্যথা
      stream->status_code = -1;

    h2_xfer_write_resp_hd(cf, তথ্য, stream, STRCONST("\r\n"),
                          (bool)stream->closed);

    Curl_multi_mark_dirty(তথ্য);
    বিরতি;
  ক্ষেত্র NGHTTP2_ঠেলো_PROMISE:
    rv = ঠেলো_promise(cf, তথ্য, &frame->ঠেলো_promise);
    যদি(rv) { /* deny! */
      DEBUGASSERT((rv > CURL_ঠেলো_OK) && (rv <= CURL_ঠেলো_ERROROUT));
      যদি(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
             frame->ঠেলো_promise.promised_stream_id, NGHTTP2_CANCEL)))
        ফেরত CURLE_SEND_ERROR;
      অন্যথা যদি(rv == CURL_ঠেলো_ERROROUT) {
        /* fail the parent transfer, not the whole session */
        CURL_TRC_CF(তথ্য, cf, "[%d] fail in PUSH_PROMISE received",
                    stream_id);
        যদি(!stream->xfer_ফলাফল)
          stream->xfer_ফলাফল = CURLE_RECV_ERROR;
        Curl_multi_mark_dirty(তথ্য);
      }
    }
    বিরতি;
  ক্ষেত্র NGHTTP2_RST_STREAM:
    যদি(frame->rst_stream.error_code)
      stream->reset_by_server = TRUE;
    Curl_multi_mark_dirty(তথ্য);
    বিরতি;
  ক্ষেত্র NGHTTP2_WINDOW_UPDATE:
    যদি(CURL_REQ_WANT_SEND(তথ্য) && Curl_bufq_is_empty(&stream->sendbuf)) {
      /* need more data, force processing of transfer */
      Curl_multi_mark_dirty(তথ্য);
    }
    অন্যথা যদি(!Curl_bufq_is_empty(&stream->sendbuf)) {
      /* resume the potentially suspended stream */
      যদি(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_session_resume_তথ্য(&ctx->ngh2, stream->id)))
        ফেরত CURLE_SEND_ERROR;
    }
    বিরতি;
  পূর্বনির্ধারিত:
    বিরতি;
  }

  যদি(frame->hd.flags & NGHTTP2_FLAG_END_STREAM) {
    যদি(!stream->closed && !stream->body_eos &&
       ((stream->status_code >= 400) || (stream->status_code < 200))) {
      /* The server did not give us a positive response and we are not
       * done uploading the request body. We need to stop doing that and
       * also inform the server that we aborted our side. */
      CURL_TRC_CF(তথ্য, cf, "[%d] EOS frame with unfinished upload and "
                  "HTTP status %d, abort upload by RST",
                  stream_id, stream->status_code);
      Curl_ngh2_submit_rst_stream(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, NGHTTP2_STREAM_CLOSED);
      stream->closed = TRUE;
    }
    Curl_multi_mark_dirty(তথ্য);
  }
  ফেরত CURLE_OK;
}

#ifdef CURLVERBOSE
পূর্ণসংখ্যা Curl_nghttp2_fr_print(ধ্রুবক nghttp2_frame *frame, অক্ষর *বাফার,
                          size_t blen)
{
  নির্বাচন(frame->hd.type) {
  ক্ষেত্র NGHTTP2_তথ্য: {
    ফেরত curl_msnprintf(বাফার, blen,
                          "FRAME[DATA, len=%d, eos=%d, padlen=%d]",
                          (পূর্ণসংখ্যা)frame->hd.দৈর্ঘ্য,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_STREAM),
                          (পূর্ণসংখ্যা)frame->তথ্য.padlen);
  }
  ক্ষেত্র NGHTTP2_HEADERS: {
    ফেরত curl_msnprintf(বাফার, blen,
                          "FRAME[HEADERS, len=%d, hend=%d, eos=%d]",
                          (পূর্ণসংখ্যা)frame->hd.দৈর্ঘ্য,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_HEADERS),
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_STREAM));
  }
  ক্ষেত্র NGHTTP2_PRIORITY: {
    ফেরত curl_msnprintf(বাফার, blen,
                          "FRAME[PRIORITY, len=%d, flags=%d]",
                          (পূর্ণসংখ্যা)frame->hd.দৈর্ঘ্য, frame->hd.flags);
  }
  ক্ষেত্র NGHTTP2_RST_STREAM: {
    ফেরত curl_msnprintf(বাফার, blen,
                          "FRAME[RST_STREAM, len=%d, flags=%d, error=%u]",
                          (পূর্ণসংখ্যা)frame->hd.দৈর্ঘ্য, frame->hd.flags,
                          frame->rst_stream.error_code);
  }
  ক্ষেত্র NGHTTP2_SETTINGS: {
    যদি(frame->hd.flags & NGHTTP2_FLAG_ACK) {
      ফেরত curl_msnprintf(বাফার, blen, "FRAME[SETTINGS, ack=1]");
    }
    ফেরত curl_msnprintf(বাফার, blen,
                          "FRAME[SETTINGS, len=%d]", (পূর্ণসংখ্যা)frame->hd.দৈর্ঘ্য);
  }
  ক্ষেত্র NGHTTP2_ঠেলো_PROMISE:
    ফেরত curl_msnprintf(বাফার, blen,
                          "FRAME[PUSH_PROMISE, len=%d, hend=%d]",
                          (পূর্ণসংখ্যা)frame->hd.দৈর্ঘ্য,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_HEADERS));
  ক্ষেত্র NGHTTP2_PING:
    ফেরত curl_msnprintf(বাফার, blen,
                          "FRAME[PING, len=%d, ack=%d]",
                          (পূর্ণসংখ্যা)frame->hd.দৈর্ঘ্য,
                          frame->hd.flags & NGHTTP2_FLAG_ACK);
  ক্ষেত্র NGHTTP2_GOAWAY: {
    অক্ষর scratch[128];
    size_t s_len = CURL_ARRAYSIZE(scratch);
    size_t len = (frame->goaway.opaque_তথ্য_len < s_len) ?
      frame->goaway.opaque_তথ্য_len : s_len - 1;
    যদি(len)
      memcpy(scratch, frame->goaway.opaque_তথ্য, len);
    scratch[len] = '\0';
    ফেরত curl_msnprintf(বাফার, blen,
                          "FRAME[GOAWAY, error=%u, reason='%s', "
                          "last_stream=%d]", frame->goaway.error_code,
                          scratch, frame->goaway.last_stream_id);
  }
  ক্ষেত্র NGHTTP2_WINDOW_UPDATE: {
    ফেরত curl_msnprintf(বাফার, blen,
                          "FRAME[WINDOW_UPDATE, incr=%d]",
                          frame->window_update.window_আকার_increment);
  }
  পূর্বনির্ধারিত:
    ফেরত curl_msnprintf(বাফার, blen, "FRAME[%d, len=%d, flags=%d]",
                          frame->hd.type, (পূর্ণসংখ্যা)frame->hd.দৈর্ঘ্য,
                          frame->hd.flags);
  }
}

স্থির পূর্ণসংখ্যা on_frame_send(nghttp2_session *session, ধ্রুবক nghttp2_frame *frame,
                         শূন্য *userp)
{
  কাঠামো Curl_cfilter *cf = userp;
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);

  (শূন্য)session;
  DEBUGASSERT(তথ্য);
  যদি(Curl_trc_cf_is_verbose(cf, তথ্য)) {
    অক্ষর বাফার[256];
    পূর্ণসংখ্যা len;
    len = Curl_nghttp2_fr_print(frame, বাফার, আকার(বাফার) - 1);
    বাফার[len] = 0;
    CURL_TRC_CF(তথ্য, cf, "[%d] -> %s", frame->hd.stream_id, বাফার);
  }
  যদি((frame->hd.type == NGHTTP2_GOAWAY) && !ctx->sent_goaway) {
    /* A GOAWAY not initiated by us, but by nghttp2 itself on detecting
     * a protocol error on the connection */
    failf(তথ্য, "nghttp2 shuts down connection with error %u: %s",
          frame->goaway.error_code,
          nghttp2_http2_strerror(frame->goaway.error_code));
  }
  ফেরত 0;
}
#endif /* CURLVERBOSE */

স্থির পূর্ণসংখ্যা on_frame_recv(nghttp2_session *session, ধ্রুবক nghttp2_frame *frame,
                         শূন্য *userp)
{
  কাঠামো Curl_cfilter *cf = userp;
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf), *তথ্য_s;
  কাঠামো h2_stream_ctx *stream;
  int32_t stream_id = frame->hd.stream_id;

  (শূন্য)session;
  DEBUGASSERT(তথ্য);
#ifdef CURLVERBOSE
  যদি(Curl_trc_cf_is_verbose(cf, তথ্য)) {
    অক্ষর বাফার[256];
    পূর্ণসংখ্যা len;
    len = Curl_nghttp2_fr_print(frame, বাফার, আকার(বাফার) - 1);
    বাফার[len] = 0;
    CURL_TRC_CF(তথ্য, cf, "[%d] <- %s", frame->hd.stream_id, বাফার);
  }
#endif /* CURLVERBOSE */

  যদি(!stream_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(তথ্য);
    নির্বাচন(frame->hd.type) {
    ক্ষেত্র NGHTTP2_SETTINGS: {
      যদি(!(frame->hd.flags & NGHTTP2_FLAG_ACK)) {
        uint32_t max_conn = ctx->max_concurrent_streams;
        ctx->max_concurrent_streams =
          Curl_ngh2_session_get_remote_settings(
            &ctx->ngh2, NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS);
        ctx->enable_ঠেলো = Curl_ngh2_session_get_remote_settings(
          &ctx->ngh2, NGHTTP2_SETTINGS_ENABLE_ঠেলো) != 0;
        CURL_TRC_CF(তথ্য, cf, "[0] MAX_CONCURRENT_STREAMS: %u",
                    ctx->max_concurrent_streams);
        CURL_TRC_CF(তথ্য, cf, "[0] ENABLE_PUSH: %s",
                    ctx->enable_ঠেলো ? "TRUE" : "false");
        যদি(তথ্য && max_conn != ctx->max_concurrent_streams) {
          /* only signal change if the value actually changed */
          CURL_TRC_CF(তথ্য, cf, "[0] notify MAX_CONCURRENT_STREAMS: %u",
                      ctx->max_concurrent_streams);
          Curl_multi_connchanged(তথ্য->multi);
        }
        /* Since the initial stream window is 64K, a request might be on HOLD,
         * due to exhaustion. The (initial) SETTINGS may announce a much larger
         * window and *assume* that we treat this like a WINDOW_UPDATE. Some
         * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
         * To be safe, we UNHOLD a stream in order not to stall. */
        যদি(CURL_REQ_WANT_SEND(তথ্য))
          Curl_multi_mark_dirty(তথ্য);
      }
      বিরতি;
    }
    ক্ষেত্র NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->goaway_error = frame->goaway.error_code;
      ctx->remote_max_sid = frame->goaway.last_stream_id;
      যদি(তথ্য) {
        infof(তথ্য, "received GOAWAY, error=%u, last_stream=%d",
              ctx->goaway_error, ctx->remote_max_sid);
        Curl_multi_connchanged(তথ্য->multi);
      }
      বিরতি;
    পূর্বনির্ধারিত:
      বিরতি;
    }
    ফেরত 0;
  }

  তথ্য_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  যদি(!তথ্য_s) {
    CURL_TRC_CF(তথ্য, cf, "[%d] No Curl_easy associated", stream_id);
    ফেরত 0;
  }

  ফেরত on_stream_frame(cf, তথ্য_s, stream, frame) ?
         NGHTTP2_ERR_CALLBACK_FAILURE : 0;
}

স্থির পূর্ণসংখ্যা cf_h2_on_invalid_frame_recv(nghttp2_session *session,
                                       ধ্রুবক nghttp2_frame *frame,
                                       পূর্ণসংখ্যা ngerr, শূন্য *userp)
{
  কাঠামো Curl_cfilter *cf = userp;
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  কাঠামো Curl_easy *তথ্য;
  কাঠামো h2_stream_ctx *stream;
  int32_t stream_id = frame->hd.stream_id;

  (শূন্য)session;
  তথ্য = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  যদি(তথ্য) {
#ifdef CURLVERBOSE
    অক্ষর বাফার[256];
    পূর্ণসংখ্যা len;
    len = Curl_nghttp2_fr_print(frame, বাফার, আকার(বাফার) - 1);
    বাফার[len] = 0;
    failf(তথ্য, "[HTTP2] [%d] received invalid frame: %s, error %d: %s",
          stream_id, বাফার, ngerr, nghttp2_strerror(ngerr));
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
  ফেরত 0;
}

স্থির পূর্ণসংখ্যা on_তথ্য_chunk_recv(nghttp2_session *session, uint8_t flags,
                              int32_t stream_id,
                              ধ্রুবক uint8_t *mem, size_t len, শূন্য *userp)
{
  কাঠামো Curl_cfilter *cf = userp;
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  কাঠামো h2_stream_ctx *stream;
  কাঠামো Curl_easy *তথ্য_s;

  (শূন্য)session;
  (শূন্য)flags;

  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */
  DEBUGASSERT(CF_তথ্য_CURRENT(cf));

  /* get the stream from the hash based on Stream ID */
  তথ্য_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  যদি(!তথ্য_s) {
    /* Receiving a Stream ID not in the hash should not happen - unless
       we have aborted a transfer artificially and there were more data
       in the pipeline. Silently ignore. */
    CURL_TRC_CF(CF_তথ্য_CURRENT(cf), cf, "[%d] Data for unknown", stream_id);
    /* consumed explicitly as no one will read it */
    Curl_ngh2_session_consume(&ctx->ngh2, stream_id, len);
    ফেরত 0;
  }

  যদি(!stream)
    ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;

  h2_xfer_write_resp(cf, তথ্য_s, stream, (ধ্রুবক অক্ষর *)mem, len, FALSE);

  Curl_ngh2_session_consume(&ctx->ngh2, stream_id, len);
  stream->nrcvd_তথ্য += (curl_off_t)len;
  ফেরত 0;
}

স্থির পূর্ণসংখ্যা on_stream_close(nghttp2_session *session, int32_t stream_id,
                           uint32_t error_code, শূন্য *userp)
{
  কাঠামো Curl_cfilter *cf = userp;
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  কাঠামো Curl_easy *তথ্য_s, *call_তথ্য = CF_তথ্য_CURRENT(cf);
  কাঠামো h2_stream_ctx *stream = ফাঁকা;
  পূর্ণসংখ্যা rv;
  (শূন্য)session;

  DEBUGASSERT(call_তথ্য);
  /* stream id 0 is the connection, do not look there for streams. */
  তথ্য_s = stream_id ? cf_h2_get_stream_xfer(cf, stream_id, &stream) : ফাঁকা;
  যদি(!তথ্য_s) {
    CURL_TRC_CF(call_তথ্য, cf,
                "[%d] on_stream_close, no easy set on stream", stream_id);
    ফেরত 0;
  }
  যদি(!GOOD_EASY_HANDLE(তথ্য_s)) {
    /* nghttp2 still has an easy registered for the stream which has
     * been freed be libcurl. This points to a code path that does not
     * trigger DONE or DETACH events as it must. */
    CURL_TRC_CF(call_তথ্য, cf,
                "[%d] on_stream_close, not a GOOD easy on stream", stream_id);
    (শূন্য)Curl_ngh2_session_set_stream_user_তথ্য(
      &ctx->ngh2, stream_id, ফাঁকা);
    ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  যদি(!stream) {
    CURL_TRC_CF(তথ্য_s, cf,
                "[%d] on_stream_close, GOOD easy but no stream", stream_id);
    ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;
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
    CURL_TRC_CF(তথ্য_s, cf, "[%d] RESET: %s (err %u)",
              stream_id, nghttp2_http2_strerror(error_code), error_code);
  অন্যথা
    CURL_TRC_CF(তথ্য_s, cf, "[%d] CLOSED", stream_id);
  Curl_multi_mark_dirty(তথ্য_s);

  /* remove `stream_ctx` from the nghttp2 stream */
  rv = Curl_ngh2_session_set_stream_user_তথ্য(
    &ctx->ngh2, stream_id, ফাঁকা);
  যদি(rv) {
    infof(তথ্য_s, "http/2: failed to clear user_data for stream %d",
          stream_id);
    DEBUGASSERT(0);
  }
  ফেরত 0;
}

স্থির শূন্য cf_h2_header_error(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য,
                               কাঠামো h2_stream_ctx *stream,
                               CURLcode ফলাফল)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;

  failf(তথ্য, "Error receiving HTTP2 header: %d(%s)", (পূর্ণসংখ্যা)ফলাফল,
        curl_easy_strerror(ফলাফল));
  যদি(stream) {
    Curl_ngh2_submit_rst_stream(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, NGHTTP2_STREAM_CLOSED);
    stream->closed = TRUE;
    stream->reset = TRUE;
  }
}

/* frame->hd.type is either NGHTTP2_HEADERS or NGHTTP2_PUSH_PROMISE */
স্থির পূর্ণসংখ্যা on_header(nghttp2_session *session, ধ্রুবক nghttp2_frame *frame,
                     ধ্রুবক uint8_t *name, size_t namelen,
                     ধ্রুবক uint8_t *মান, size_t valuelen,
                     uint8_t flags,
                     শূন্য *userp)
{
  কাঠামো Curl_cfilter *cf = userp;
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  কাঠামো h2_stream_ctx *stream;
  কাঠামো Curl_easy *তথ্য;
  int32_t stream_id = frame->hd.stream_id;
  CURLcode ফলাফল;

  (শূন্য)session;
  (শূন্য)flags;

  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */

  /* get the stream from the hash based on Stream ID */
  তথ্য = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  যদি(!GOOD_EASY_HANDLE(তথ্য))
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;

  যদি(!stream) {
    failf(তথ্য, "Internal NULL stream");
    ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  /* Store received PUSH_PROMISE headers to be used when the subsequent
     PUSH_PROMISE callback comes */
  যদি(frame->hd.type == NGHTTP2_ঠেলো_PROMISE) {
    অক্ষর *h;

    যদি((namelen == CURL_CSTRLEN(HTTP_PSEUDO_AUTHORITY)) &&
       !strncmp(HTTP_PSEUDO_AUTHORITY, (ধ্রুবক অক্ষর *)name, namelen)) {
      /* pseudo headers are lower case */
      পূর্ণসংখ্যা rc = 0;
      অক্ষর *check = curl_maprintf("%s:%d", তথ্য->state.origin->hostname,
                                  তথ্য->state.origin->port);
      যদি(!check)
        /* no memory */
        ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;
      যদি(!curl_strequal(check, (ধ্রুবক অক্ষর *)মান) &&
         ((তথ্য->state.origin->port != তথ্য->state.origin->scheme->defport) ||
          !curl_strequal(তথ্য->state.origin->hostname, (ধ্রুবক অক্ষর *)মান))) {
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
        ফেরত rc;
    }

    যদি(!stream->ঠেলো_headers) {
      stream->ঠেলো_headers_alloc = 10;
      stream->ঠেলো_headers = curlx_malloc(stream->ঠেলো_headers_alloc *
                                          আকার(অক্ষর *));
      যদি(!stream->ঠেলো_headers)
        ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;
      stream->ঠেলো_headers_used = 0;
    }
    অন্যথা যদি(stream->ঠেলো_headers_used ==
            stream->ঠেলো_headers_alloc) {
      অক্ষর **headp;
      যদি(stream->ঠেলো_headers_alloc > 1000) {
        /* this is beyond crazy many headers, bail out */
        failf(তথ্য, "Too many PUSH_PROMISE headers");
        free_ঠেলো_headers(stream);
        ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      stream->ঠেলো_headers_alloc *= 2;
      headp = curlx_realloc(stream->ঠেলো_headers,
                            stream->ঠেলো_headers_alloc * আকার(অক্ষর *));
      যদি(!headp) {
        free_ঠেলো_headers(stream);
        ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      stream->ঠেলো_headers = headp;
    }
    h = curl_maprintf("%s:%s", name, মান);
    যদি(!h) {
      free_ঠেলো_headers(stream);
      ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    stream->ঠেলো_headers[stream->ঠেলো_headers_used++] = h;
    ফেরত 0;
  }

  যদি(stream->bodystarted) {
    /* This is a trailer */
    CURL_TRC_CF(তথ্য, cf, "[%d] trailer: %.*s: %.*s",
                stream->id, (পূর্ণসংখ্যা)namelen, name, (পূর্ণসংখ্যা)valuelen, মান);
    ফলাফল = Curl_dynhds_add(&stream->resp_trailers,
                             (ধ্রুবক অক্ষর *)name, namelen,
                             (ধ্রুবক অক্ষর *)মান, valuelen);
    যদি(ফলাফল) {
      cf_h2_header_error(cf, তথ্য, stream, ফলাফল);
      ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;
    }

    ফেরত 0;
  }

  যদি(namelen == CURL_CSTRLEN(HTTP_PSEUDO_STATUS) &&
     !memcmp(HTTP_PSEUDO_STATUS, name, namelen)) {
    /* nghttp2 guarantees :status is received first and only once. */
    অক্ষর বাফার[32];
    size_t hlen;
    ফলাফল = Curl_http_decode_status(&stream->status_code,
                                     (ধ্রুবক অক্ষর *)মান, valuelen);
    যদি(ফলাফল) {
      cf_h2_header_error(cf, তথ্য, stream, ফলাফল);
      ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    hlen = curl_msnprintf(বাফার, আকার(বাফার), HTTP_PSEUDO_STATUS ":%d\r",
                          stream->status_code);
    ফলাফল = Curl_headers_ঠেলো(তথ্য, বাফার, hlen, CURLH_PSEUDO);
    যদি(ফলাফল) {
      cf_h2_header_error(cf, তথ্য, stream, ফলাফল);
      ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    curlx_dyn_reset(&ctx->scratch);
    ফলাফল = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/2 "));
    যদি(!ফলাফল)
      ফলাফল = curlx_dyn_addn(&ctx->scratch, মান, valuelen);
    যদি(!ফলাফল)
      ফলাফল = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    যদি(!ফলাফল)
      h2_xfer_write_resp_hd(cf, তথ্য, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    যদি(ফলাফল) {
      cf_h2_header_error(cf, তথ্য, stream, ফলাফল);
      ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    /* if we receive data for another handle, wake that up */
    যদি(CF_তথ্য_CURRENT(cf) != তথ্য)
      Curl_multi_mark_dirty(তথ্য);

    CURL_TRC_CF(তথ্য, cf, "[%d] status: HTTP/2 %03d",
                stream->id, stream->status_code);
    ফেরত 0;
  }

  /* nghttp2 guarantees that namelen > 0, and :status was already
     received, and this is not pseudo-header field . */
  /* convert to an HTTP1-style header */
  curlx_dyn_reset(&ctx->scratch);
  ফলাফল = curlx_dyn_addn(&ctx->scratch, (ধ্রুবক অক্ষর *)name, namelen);
  যদি(!ফলাফল)
    ফলাফল = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
  যদি(!ফলাফল)
    ফলাফল = curlx_dyn_addn(&ctx->scratch, (ধ্রুবক অক্ষর *)মান, valuelen);
  যদি(!ফলাফল)
    ফলাফল = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
  যদি(!ফলাফল)
    h2_xfer_write_resp_hd(cf, তথ্য, stream, curlx_dyn_ptr(&ctx->scratch),
                          curlx_dyn_len(&ctx->scratch), FALSE);
  যদি(ফলাফল) {
    cf_h2_header_error(cf, তথ্য, stream, ফলাফল);
    ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  /* if we receive data for another handle, wake that up */
  যদি(CF_তথ্য_CURRENT(cf) != তথ্য)
    Curl_multi_mark_dirty(তথ্য);

  CURL_TRC_CF(তথ্য, cf, "[%d] header: %.*s: %.*s",
              stream->id, (পূর্ণসংখ্যা)namelen, name, (পূর্ণসংখ্যা)valuelen, মান);

  ফেরত 0; /* 0 is successful */
}

স্থির ssize_t req_body_read_callback(nghttp2_session *session,
                                      int32_t stream_id,
                                      uint8_t *buf, size_t দৈর্ঘ্য,
                                      uint32_t *তথ্য_flags,
                                      nghttp2_তথ্য_source *source,
                                      শূন্য *userp)
{
  কাঠামো Curl_cfilter *cf = userp;
  কাঠামো Curl_easy *তথ্য_s;
  কাঠামো h2_stream_ctx *stream = ফাঁকা;
  CURLcode ফলাফল;
  ssize_t nread;
  size_t n;
  (শূন্য)source;

  (শূন্য)session;
  যদি(!stream_id)
    ফেরত NGHTTP2_ERR_INVALID_ARGUMENT;

  /* get the stream from the hash based on Stream ID, stream ID zero is for
     connection-oriented stuff */
  তথ্য_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  যদি(!তথ্য_s || !stream)
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;

  ফলাফল = Curl_bufq_read(&stream->sendbuf, buf, দৈর্ঘ্য, &n);
  যদি(ফলাফল) {
    যদি(ফলাফল != CURLE_AGAIN)
      ফেরত NGHTTP2_ERR_CALLBACK_FAILURE;
    nread = 0;
  }
  অন্যথা
    nread = (ssize_t)n;

  CURL_TRC_CF(তথ্য_s, cf, "[%d] req_body_read(len=%zu) eos=%d -> %zd, %d",
              stream_id, দৈর্ঘ্য, stream->body_eos, nread, (পূর্ণসংখ্যা)ফলাফল);

  যদি(stream->body_eos && Curl_bufq_is_empty(&stream->sendbuf)) {
    *তথ্য_flags = NGHTTP2_তথ্য_FLAG_EOF;
    ফেরত nread;
  }
  ফেরত (nread == 0) ? NGHTTP2_ERR_DEFERRED : nread;
}

#ifdef CURLVERBOSE
স্থির পূর্ণসংখ্যা error_callback(nghttp2_session *session,
                          ধ্রুবক অক্ষর *বার্তা,
                          size_t len,
                          শূন্য *userp)
{
  কাঠামো Curl_cfilter *cf = userp;
  কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);
  (শূন্য)session;
  failf(তথ্য, "%.*s", (পূর্ণসংখ্যা)len, বার্তা);
  ফেরত 0;
}
#endif

/*
 * Append headers to ask for an HTTP1.1 to HTTP2 upgrade.
 */
CURLcode Curl_http2_request_upgrade(কাঠামো dynbuf *req,
                                    কাঠামো Curl_easy *তথ্য)
{
  CURLcode ফলাফল;
  অক্ষর *base64;
  size_t blen;
  কাঠামো SingleRequest *k = &তথ্য->req;
  uint8_t binsettings[H2_BINSETTINGS_LEN];
  ssize_t rc;
  size_t binlen; /* length of the binsettings data */

  rc = populate_binsettings(binsettings, তথ্য);
  যদি(!curlx_sztouz(rc, &binlen) || !binlen) {
    failf(তথ্য, "nghttp2 unexpectedly failed on pack_settings_payload");
    curlx_dyn_free(req);
    ফেরত CURLE_FAILED_INIT;
  }

  ফলাফল = curlx_base64url_encode(binsettings, binlen, &base64, &blen);
  যদি(ফলাফল) {
    curlx_dyn_free(req);
    ফেরত ফলাফল;
  }

  তথ্য->state.http_hd_upgrade = TRUE;
  তথ্য->state.http_hd_h2_settings = TRUE;
  ফলাফল = curlx_dyn_addf(req,
                          "Upgrade: %s\r\n"
                          "HTTP2-Settings: %s\r\n",
                          NGHTTP2_CLEARTEXT_PROTO_VERSION_ID, base64);
  curlx_free(base64);

  k->upgr101 = UPGR101_H2;
  তথ্য->conn->bits.upgrade_in_progress = TRUE;

  ফেরত ফলাফল;
}

স্থির CURLcode http2_handle_stream_close(কাঠামো Curl_cfilter *cf,
                                          কাঠামো Curl_easy *তথ্য,
                                          কাঠামো h2_stream_ctx *stream,
                                          size_t *pnlen)
{
  CURLcode ফলাফল;

  *pnlen = 0;
  যদি(stream->reset) {
    যদি(stream->error == NGHTTP2_REFUSED_STREAM) {
      infof(তথ্য, "HTTP/2 stream %d refused by server, try again on a new "
                  "connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      তথ্য->state.refused_stream = TRUE;
      ফেরত CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    অন্যথা যদি(stream->resp_hds_complete && তথ্য->req.no_body) {
      CURL_TRC_CF(তথ্য, cf, "[%d] error after response headers, but we did "
                  "not want a body anyway, ignore: %s (err %u)",
                  stream->id, nghttp2_http2_strerror(stream->error),
                  stream->error);
      stream->close_handled = TRUE;
      ফেরত CURLE_OK;
    }
    failf(তথ্য, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          stream->id, stream->reset_by_server ? "server" : "curl",
          stream->error, nghttp2_http2_strerror(stream->error));
    ফেরত stream->error ? CURLE_HTTP2_STREAM :
           (তথ্য->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2);
  }
  অন্যথা যদি(!stream->bodystarted) {
    failf(তথ্য, "HTTP/2 stream %d was closed cleanly, but before getting "
          "all response header fields, treated as error", stream->id);
    ফেরত CURLE_HTTP2_STREAM;
  }

  যদি(Curl_dynhds_গুনতি(&stream->resp_trailers)) {
    কাঠামো dynhds_entry *e;
    কাঠামো dynbuf dbuf;
    size_t i;

    ফলাফল = CURLE_OK;
    curlx_dyn_init(&dbuf, DYN_TRAILERS);
    জন্য(i = 0; i < Curl_dynhds_গুনতি(&stream->resp_trailers); ++i) {
      e = Curl_dynhds_getn(&stream->resp_trailers, i);
      যদি(!e)
        বিরতি;
      curlx_dyn_reset(&dbuf);
      ফলাফল = curlx_dyn_addf(&dbuf, "%.*s: %.*s\x0d\x0a",
                              (পূর্ণসংখ্যা)e->namelen, e->name,
                              (পূর্ণসংখ্যা)e->valuelen, e->মান);
      যদি(ফলাফল)
        বিরতি;
      Curl_debug(তথ্য, CURLINFO_HEADER_IN, curlx_dyn_ptr(&dbuf),
                 curlx_dyn_len(&dbuf));
      ফলাফল = Curl_client_write(তথ্য,
                                 CLIENTWRITE_HEADER | CLIENTWRITE_TRAILER,
                                 curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf));
      যদি(ফলাফল)
        বিরতি;
      ফলাফল = Curl_bump_headersize(তথ্য, curlx_dyn_len(&dbuf), FALSE);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
    curlx_dyn_free(&dbuf);
    যদি(ফলাফল)
      যাও out;
  }

  stream->close_handled = TRUE;
  ফলাফল = CURLE_OK;

out:
  CURL_TRC_CF(তথ্য, cf, "handle_stream_close -> %d, %zu", (পূর্ণসংখ্যা)ফলাফল, *pnlen);
  ফেরত ফলাফল;
}

স্থির পূর্ণসংখ্যা sweight_wanted(ধ্রুবক কাঠামো Curl_easy *তথ্য)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  ফেরত তথ্য->set.weight ?
    তথ্য->set.weight : NGHTTP2_DEFAULT_WEIGHT;
}

স্থির পূর্ণসংখ্যা sweight_in_effect(ধ্রুবক কাঠামো Curl_easy *তথ্য)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  ফেরত তথ্য->state.weight ?
    তথ্য->state.weight : NGHTTP2_DEFAULT_WEIGHT;
}

/*
 * h2_pri_spec() fills in the pri_spec struct, used by nghttp2 to send weight
 * and dependency to the peer. It also stores the updated values in the state
 * struct.
 */

স্থির শূন্য h2_pri_spec(কাঠামো Curl_easy *তথ্য,
                        nghttp2_priority_spec *pri_spec)
{
  পূর্ণসংখ্যা prio = তথ্য->set.weight;
  nghttp2_priority_spec_init(pri_spec, 0, sweight_wanted(তথ্য), FALSE);
  তথ্য->state.weight = prio;
}

/*
 * Check if there is been an update in the priority /
 * dependency settings and if so it submits a PRIORITY frame with the updated
 * info.
 * Flush any out data pending in the network buffer.
 */
স্থির CURLcode h2_progress_egress(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  পূর্ণসংখ্যা rv = 0;

  যদি(sweight_wanted(তথ্য) != sweight_in_effect(তথ্য)) {
    কাঠামো h2_stream_ctx *stream = H2_STREAM_CTX(ctx, তথ্য);
    যদি(stream && stream->id > 0 &&
       (sweight_wanted(তথ্য) != sweight_in_effect(তথ্য))) {
      /* send new weight and/or dependency */
      nghttp2_priority_spec pri_spec;

      h2_pri_spec(তথ্য, &pri_spec);
      CURL_TRC_CF(তথ্য, cf, "[%d] Queuing PRIORITY", stream->id);
      DEBUGASSERT(stream->id != -1);
      rv = Curl_ngh2_submit_priority(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, &pri_spec);
      যদি(rv)
        যাও out;
    }
  }

  ctx->nw_out_blocked = 0;
  যখন(!rv && !ctx->nw_out_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

out:
  যদি(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(তথ্য, cf, "nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    ফেরত CURLE_SEND_ERROR;
  }
  /* Defer flushing during the connect phase so that the SETTINGS and
   * other initial frames are sent together with the first request.
   * Unless we are 'connect_only' where the request will never come. */
  যদি(!cf->connected && !cf->conn->bits.connect_only)
    ফেরত CURLE_OK;
  ফেরত nw_out_flush(cf, তথ্য);
}

স্থির CURLcode stream_recv(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                            কাঠামো h2_stream_ctx *stream,
                            অক্ষর *buf, size_t len, size_t *pnread)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_AGAIN;

  (শূন্য)buf;
  (শূন্য)len;
  *pnread = 0;

  যদি(!stream->xfer_ফলাফল)
    stream->xfer_ফলাফল = cf_h2_update_local_win(cf, তথ্য, stream);

  যদি(stream->xfer_ফলাফল) {
    CURL_TRC_CF(তথ্য, cf, "[%d] xfer write failed", stream->id);
    ফলাফল = stream->xfer_ফলাফল;
  }
  অন্যথা যদি(stream->closed) {
    CURL_TRC_CF(তথ্য, cf, "[%d] returning CLOSE", stream->id);
    ফলাফল = http2_handle_stream_close(cf, তথ্য, stream, pnread);
  }
  অন্যথা যদি(stream->reset ||
          (ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) ||
          (ctx->rcvd_goaway && ctx->remote_max_sid < stream->id)) {
    CURL_TRC_CF(তথ্য, cf, "[%d] returning ERR", stream->id);
    ফলাফল = তথ্য->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2;
  }

  যদি(ফলাফল && (ফলাফল != CURLE_AGAIN))
    CURL_TRC_CF(তথ্য, cf, "[%d] stream_recv(len=%zu) -> %d, %zu",
                stream->id, len, (পূর্ণসংখ্যা)ফলাফল, *pnread);
  ফেরত ফলাফল;
}

স্থির CURLcode h2_progress_ingress(কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য,
                                    size_t তথ্য_max_bytes)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;
  size_t nread;

  যদি(should_close_session(ctx)) {
    CURL_TRC_CF(তথ্য, cf, "[0] ingress: session is closed");
    ফেরত CURLE_HTTP2;
  }

  /* Process network input buffer first */
  যদি(!Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(তথ্য, cf, "Process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    ফলাফল = h2_process_pending_প্রবেশ(cf, তথ্য);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  যদি(!তথ্য_max_bytes)
    তথ্য_max_bytes = H2_CHUNK_আকার;

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop due to connection close or us not processing
   * all network input */
  যখন(!ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) {
    কাঠামো h2_stream_ctx *stream = H2_STREAM_CTX(ctx, তথ্য);
    যদি(stream && (stream->closed || !তথ্য_max_bytes)) {
      /* We would like to abort here and stop processing, so that the transfer
       * loop can handle the data/close here. This may leave data in
       * underlying buffers that will not be consumed. */
      যদি(!cf->পরবর্তী || !cf->পরবর্তী->cft->has_তথ্য_pending(cf->পরবর্তী, তথ্য))
        Curl_multi_mark_dirty(তথ্য);
      বিরতি;
    }

    ফলাফল = Curl_cf_recv_bufq(cf->পরবর্তী, তথ্য, &ctx->inbufq, 0, &nread);
    যদি(ফলাফল) {
      যদি(ফলাফল != CURLE_AGAIN) {
        failf(তথ্য, "Failed receiving HTTP2 data: %d(%s)", (পূর্ণসংখ্যা)ফলাফল,
              curl_easy_strerror(ফলাফল));
        ফেরত ফলাফল;
      }
      বিরতি;
    }
    অন্যথা যদি(nread == 0) {
      CURL_TRC_CF(তথ্য, cf, "[0] ingress: connection closed");
      ctx->conn_closed = TRUE;
      বিরতি;
    }
    অন্যথা {
      CURL_TRC_CF(তথ্য, cf, "[0] ingress: read %zu bytes", nread);
      তথ্য_max_bytes = (তথ্য_max_bytes > nread) ? (তথ্য_max_bytes - nread) : 0;
    }

    ফলাফল = h2_process_pending_প্রবেশ(cf, তথ্য);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    CURL_TRC_CF(তথ্য, cf, "[0] ingress: nw-in buffered %zu",
                Curl_bufq_len(&ctx->inbufq));
  }

  যদি(ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(তথ্য, cf, "server closed %s",
                ctx->rcvd_goaway ? "with GOAWAY" : "abruptly");
    connclose(cf->conn);
  }

  CURL_TRC_CF(তথ্য, cf, "[0] ingress: done");
  ফেরত CURLE_OK;
}

স্থির CURLcode cf_h2_recv(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                           অক্ষর *buf, size_t len, size_t *pnread)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  কাঠামো h2_stream_ctx *stream;
  CURLcode ফলাফল, r2;
  কাঠামো cf_call_তথ্য save;

  যদি(!তথ্য)
    ফেরত CURLE_HTTP2;

  stream = H2_STREAM_CTX(ctx, তথ্য);

  *pnread = 0;
  যদি(!stream) {
    /* Abnormal call sequence: either this transfer has never opened a stream
     * (unlikely) or the transfer has been done, cleaned up its resources, but
     * a read() is called anyway. It is not clear what the calling sequence
     * is for such a case. */
    failf(তথ্য, "http/2 recv on a transfer never opened "
          "or already cleared, mid=%u", তথ্য->mid);
    ফেরত CURLE_HTTP2;
  }

  CF_তথ্য_SAVE(save, cf, তথ্য);

  ফলাফল = stream_recv(cf, তথ্য, stream, buf, len, pnread);
  যদি(ফলাফল && (ফলাফল != CURLE_AGAIN))
    যাও out;

  যদি(ফলাফল) {
    ফলাফল = h2_progress_ingress(cf, তথ্য, len);
    যদি(ফলাফল)
      যাও out;

    ফলাফল = stream_recv(cf, তথ্য, stream, buf, len, pnread);
  }

  যদি(*pnread > 0) {
    /* Now that we transferred this to the upper layer, we report
     * the actual amount of DATA consumed to the H2 session, so
     * that it adjusts stream flow control */
    Curl_ngh2_session_consume(&ctx->ngh2, stream->id, *pnread);
    যদি(stream->closed) {
      CURL_TRC_CF(তথ্য, cf, "[%d] DRAIN closed stream", stream->id);
      Curl_multi_mark_dirty(তথ্য);
    }
  }

out:
  r2 = h2_progress_egress(cf, তথ্য);
  যদি(r2 == CURLE_AGAIN) {
    /* pending data to send, need to be called again. Ideally, we
     * monitor the socket for POLLOUT, but when not SENDING
     * any more, we force processing of the transfer. */
    যদি(!CURL_REQ_WANT_SEND(তথ্য))
      Curl_multi_mark_dirty(তথ্য);
  }
  অন্যথা যদি(r2 && (ফলাফল == CURLE_AGAIN)) {
    /* Do not replace a previous error result NOR a successful result
     * with an egress error. Example: the server might close the
     * connection after sending the complete response with EOS
     * and everything. Further sending may fail, but that MUST
     * not change that we had a successful response. */
    ফলাফল = r2;
  }

  CURL_TRC_CF(তথ্য, cf, "[%d] cf_recv(len=%zu) -> %d, %zu, "
              "window=%d/%d, connection %d/%d",
              stream->id, len, (পূর্ণসংখ্যা)ফলাফল, *pnread,
              Curl_ngh2_session_get_stream_effective_recv_তথ্য_দৈর্ঘ্য(
                &ctx->ngh2, stream->id),
              Curl_ngh2_session_get_stream_effective_local_window_আকার(
                &ctx->ngh2, stream->id),
              Curl_ngh2_session_get_local_window_আকার(&ctx->ngh2),
              HTTP2_HUGE_WINDOW_আকার);

  CF_তথ্য_RESTORE(cf, save);
  ফেরত ফলাফল;
}

স্থির CURLcode cf_h2_body_send(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য,
                                কাঠামো h2_stream_ctx *stream,
                                ধ্রুবক শূন্য *buf, size_t blen, bool eos,
                                size_t *pnwritten)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  CURLcode ফলাফল;

  *pnwritten = 0;
  যদি(stream->closed) {
    যদি(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(তথ্য, cf, "[%d] discarding data"
                  "on closed stream with response", stream->id);
      যদি(eos)
        stream->body_eos = TRUE;
      *pnwritten = blen;
      ফেরত CURLE_OK;
    }
    /* Server closed before we got a response, this is an error */
    infof(তথ্য, "stream %d closed", stream->id);
    ফেরত CURLE_SEND_ERROR;
  }

  ফলাফল = Curl_bufq_write(&stream->sendbuf, buf, blen, pnwritten);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  যদি(eos && (blen == *pnwritten))
    stream->body_eos = TRUE;

  যদি(eos || !Curl_bufq_is_empty(&stream->sendbuf)) {
    /* resume the potentially suspended stream */
    যদি(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_তথ্য(&ctx->ngh2, stream->id)))
      ফেরত CURLE_SEND_ERROR;
  }

  ফেরত CURLE_OK;
}

স্থির CURLcode h2_submit(কাঠামো h2_stream_ctx **pstream,
                          কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                          ধ্রুবক uint8_t *buf, size_t len,
                          bool eos, size_t *pnwritten)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  কাঠামো h2_stream_ctx *stream = ফাঁকা;
  কাঠামো dynhds h2_headers;
  nghttp2_nv *nva = ফাঁকা;
  ধ্রুবক শূন্য *body = ফাঁকা;
  size_t nheader, bodylen;
  nghttp2_তথ্য_provider তথ্য_prd;
  int32_t stream_id;
  nghttp2_priority_spec pri_spec;
  size_t nwritten;
  CURLcode ফলাফল = CURLE_OK;
  uint32_t initial_win_আকার;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_headers, MAX_HTTP_RESP_HEADER_গুনতি, DYN_HTTP_REQUEST);

  ফলাফল = http2_তথ্য_setup(cf, তথ্য, &stream);
  যদি(ফলাফল)
    যাও out;

  ফলাফল = Curl_h1_req_parse_read(
    &stream->h1, buf, len, ফাঁকা,
    !তথ্য->state.http_ignorecustom ?
    CURL_EASY_STR(তথ্য, STRING_CUSTOMREQUEST) : ফাঁকা,
    0, &nwritten);
  যদি(ফলাফল)
    যাও out;
  *pnwritten = nwritten;
  যদি(!stream->h1.done) {
    /* need more data */
    যাও out;
  }
  DEBUGASSERT(stream->h1.req);

  ফলাফল = Curl_http_req_to_h2(&h2_headers, stream->h1.req, তথ্য);
  যদি(ফলাফল)
    যাও out;
  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nva = Curl_dynhds_to_nva(&h2_headers, &nheader);
  যদি(!nva) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  h2_pri_spec(তথ্য, &pri_spec);
  যদি(!Curl_ngh2_session_check_request_allowed(&ctx->ngh2))
    CURL_TRC_CF(তথ্য, cf, "send request NOT allowed (via nghttp2)");

  /* Check the initial windows size of the transfer (rate-limits?) and
   * send an updated settings on changes from previous value. */
  initial_win_আকার = cf_h2_initial_win_আকার(তথ্য);
  যদি(initial_win_আকার != ctx->initial_win_আকার) {
    ফলাফল = cf_h2_update_settings(ctx, initial_win_আকার);
    যদি(ফলাফল)
      যাও out;
  }

  নির্বাচন(তথ্য->state.httpreq) {
  ক্ষেত্র HTTPREQ_POST:
  ক্ষেত্র HTTPREQ_POST_FORM:
  ক্ষেত্র HTTPREQ_POST_MIME:
  ক্ষেত্র HTTPREQ_PUT:
    তথ্য_prd.read_callback = req_body_read_callback;
    তথ্য_prd.source.ptr = ফাঁকা;
    stream_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, &তথ্য_prd, stream);
    বিরতি;
  পূর্বনির্ধারিত:
    stream_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, ফাঁকা, stream);
  }

  যদি(stream_id < 0) {
    CURL_TRC_CF(তথ্য, cf, "send: nghttp2_submit_request error (%s)%d",
                nghttp2_strerror(stream_id), stream_id);
    ফলাফল = CURLE_SEND_ERROR;
    যাও out;
  }

#ifdef CURLVERBOSE
#define MAX_ACC 60000  /* <64KB to account for some overhead */
  যদি(Curl_trc_is_verbose(তথ্য)) {
    size_t acc = 0, i;

    infof(তথ্য, "[HTTP/2] [%d] OPENED stream for %s",
          stream_id, Curl_bufref_ptr(&তথ্য->state.url));
    জন্য(i = 0; i < nheader; ++i) {
      acc += nva[i].namelen + nva[i].valuelen;

      infof(তথ্য, "[HTTP/2] [%d] [%.*s: %.*s]", stream_id,
            (পূর্ণসংখ্যা)nva[i].namelen, nva[i].name,
            (পূর্ণসংখ্যা)nva[i].valuelen, nva[i].মান);
    }

    যদি(acc > MAX_ACC) {
      infof(তথ্য, "[HTTP/2] Warning: The cumulative length of all "
            "headers exceeds %d bytes and that could cause the "
            "stream to be rejected.", MAX_ACC);
    }
  }
#endif

  stream->id = stream_id;

  body = (ধ্রুবক অক্ষর *)buf + *pnwritten;
  bodylen = len - *pnwritten;

  যদি(bodylen || eos) {
    size_t n;
    ফলাফল = cf_h2_body_send(cf, তথ্য, stream, body, bodylen, eos, &n);
    যদি(!ফলাফল)
      *pnwritten += n;
    অন্যথা যদি(ফলাফল == CURLE_AGAIN)
      ফলাফল = CURLE_OK;
    অন্যথা {
      ফলাফল = CURLE_SEND_ERROR;
    }
  }

out:
  CURL_TRC_CF(তথ্য, cf, "[%d] submit -> %d, %zu",
              stream ? stream->id : -1, (পূর্ণসংখ্যা)ফলাফল, *pnwritten);
  curlx_safefree(nva);
  *pstream = stream;
  Curl_dynhds_free(&h2_headers);
  ফেরত ফলাফল;
}

স্থির CURLcode cf_h2_send(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                           ধ্রুবক uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  কাঠামো h2_stream_ctx *stream = H2_STREAM_CTX(ctx, তথ্য);
  কাঠামো cf_call_তথ্য save;
  CURLcode ফলাফল = CURLE_OK, r2;

  CF_তথ্য_SAVE(save, cf, তথ্য);
  *pnwritten = 0;

  যদি(!stream || stream->id == -1) {
    ফলাফল = h2_submit(&stream, cf, তথ্য, buf, len, eos, pnwritten);
    যদি(ফলাফল)
      যাও out;
    DEBUGASSERT(stream);
  }
  অন্যথা যদি(stream->body_eos) {
    /* We already wrote this, but CURLE_AGAIN-ed the call due to not
     * being able to flush stream->sendbuf. Make a 0-length write
     * to trigger flushing again.
     * If this works, we report to have written `len` bytes. */
    size_t n;
    DEBUGASSERT(eos);
    ফলাফল = cf_h2_body_send(cf, তথ্য, stream, buf, 0, eos, &n);
    CURL_TRC_CF(তথ্য, cf, "[%d] cf_body_send last CHUNK -> %d, %zu, eos=%d",
                stream->id, (পূর্ণসংখ্যা)ফলাফল, n, eos);
    যদি(ফলাফল)
      যাও out;
    *pnwritten = len;
  }
  অন্যথা {
    ফলাফল = cf_h2_body_send(cf, তথ্য, stream, buf, len, eos, pnwritten);
    CURL_TRC_CF(তথ্য, cf, "[%d] cf_body_send(len=%zu) -> %d, %zu, eos=%d",
                stream->id, len, (পূর্ণসংখ্যা)ফলাফল, *pnwritten, eos);
  }

  /* Call the nghttp2 send loop and flush to write ALL buffered data,
   * headers and/or request body completely out to the network */
  r2 = h2_progress_egress(cf, তথ্য);

  /* if the stream has been closed in egress handling (nghttp2 does that
   * when it does not like the headers, for example */
  যদি(stream && stream->closed && !stream->resp_hds_complete) {
    infof(তথ্য, "stream %d closed", stream->id);
    ফলাফল = CURLE_SEND_ERROR;
    যাও out;
  }
  অন্যথা যদি(r2 && (r2 != CURLE_AGAIN)) {
    ফলাফল = r2;
    যাও out;
  }

  যদি(should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    যদি(stream && stream->closed) {
      ফলাফল = http2_handle_stream_close(cf, তথ্য, stream, pnwritten);
    }
    অন্যথা {
      CURL_TRC_CF(তথ্য, cf, "send: nothing to do in this session");
      ফলাফল = CURLE_HTTP2;
    }
  }

out:
  যদি(stream) {
    CURL_TRC_CF(তথ্য, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
                "eos=%d, h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                stream->id, len, (পূর্ণসংখ্যা)ফলাফল, *pnwritten,
                stream->body_eos,
                Curl_ngh2_session_get_stream_remote_window_আকার(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_remote_window_আকার(&ctx->ngh2),
                Curl_bufq_len(&stream->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  অন্যথা {
    CURL_TRC_CF(তথ্য, cf, "cf_send(len=%zu) -> %d, %zu, "
                "connection-window=%d, nw_send_buffer(%zu)",
                len, (পূর্ণসংখ্যা)ফলাফল, *pnwritten,
                Curl_ngh2_session_get_remote_window_আকার(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_তথ্য_RESTORE(cf, save);
  ফেরত ফলাফল;
}

স্থির CURLcode cf_h2_flush(কাঠামো Curl_cfilter *cf,
                            কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  কাঠামো h2_stream_ctx *stream = H2_STREAM_CTX(ctx, তথ্য);
  কাঠামো cf_call_তথ্য save;
  CURLcode ফলাফল = CURLE_OK;

  CF_তথ্য_SAVE(save, cf, তথ্য);
  যদি(stream && !Curl_bufq_is_empty(&stream->sendbuf)) {
    /* resume the potentially suspended stream */
    যদি(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_তথ্য(&ctx->ngh2, stream->id))) {
      ফলাফল = CURLE_SEND_ERROR;
      যাও out;
    }
  }

  ফলাফল = h2_progress_egress(cf, তথ্য);

out:
  যদি(stream) {
    CURL_TRC_CF(তথ্য, cf, "[%d] flush -> %d, "
                "h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                stream->id, (পূর্ণসংখ্যা)ফলাফল,
                Curl_ngh2_session_get_stream_remote_window_আকার(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_remote_window_আকার(&ctx->ngh2),
                Curl_bufq_len(&stream->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  অন্যথা {
    CURL_TRC_CF(তথ্য, cf, "flush -> %d, "
                "connection-window=%d, nw_send_buffer(%zu)", (পূর্ণসংখ্যা)ফলাফল,
                Curl_ngh2_session_get_remote_window_আকার(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_তথ্য_RESTORE(cf, save);
  ফেরত ফলাফল;
}

স্থির CURLcode cf_h2_adjust_pollset(কাঠামো Curl_cfilter *cf,
                                     কাঠামো Curl_easy *তথ্য,
                                     কাঠামো easy_pollset *ps)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  কাঠামো cf_call_তথ্য save;
  curl_socket_t sock;
  bool want_recv, want_send;
  CURLcode ফলাফল = CURLE_OK;

  যদি(!Curl_ngh2_valid(&ctx->ngh2))
    ফেরত CURLE_OK;

  sock = Curl_conn_cf_get_socket(cf, তথ্য);
  Curl_pollset_check(তথ্য, ps, sock, &want_recv, &want_send);
  যদি(want_recv || want_send) {
    কাঠামো h2_stream_ctx *stream = H2_STREAM_CTX(ctx, তথ্য);
    bool c_exhaust, s_exhaust;

    CF_তথ্য_SAVE(save, cf, তথ্য);
    c_exhaust = want_send &&
                !Curl_ngh2_session_get_remote_window_আকার(&ctx->ngh2);
    s_exhaust = want_send && stream && stream->id >= 0 &&
                !Curl_ngh2_session_get_stream_remote_window_আকার(
                  &ctx->ngh2, stream->id);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                (!c_exhaust && Curl_ngh2_session_want_write(&ctx->ngh2)) ||
                !Curl_bufq_is_empty(&ctx->outbufq);

    ফলাফল = Curl_pollset_set(তথ্য, ps, sock, want_recv, want_send);
    CF_তথ্য_RESTORE(cf, save);
  }
  অন্যথা যদি(ctx->sent_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_তথ্য_SAVE(save, cf, তথ্য);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    ফলাফল = Curl_pollset_set(তথ্য, ps, sock, want_recv, want_send);
    CF_তথ্য_RESTORE(cf, save);
  }
  ফেরত ফলাফল;
}

স্থির CURLcode cf_h2_ctx_open(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  কাঠামো h2_stream_ctx *stream;
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;
  পূর্ণসংখ্যা rc;
  nghttp2_session_callbacks *cbs = ফাঁকা;

  DEBUGASSERT(!ctx->ngh2.s);
  DEBUGASSERT(!ctx->ngh2.fatal_rv);
  DEBUGASSERT(ctx->initialized);

  rc = nghttp2_session_callbacks_new(&cbs);
  যদি(rc) {
    failf(তথ্য, "Could not initialize nghttp2 callbacks");
    যাও out;
  }

  nghttp2_session_callbacks_set_send_callback(cbs, send_callback);
  nghttp2_session_callbacks_set_on_frame_recv_callback(cbs, on_frame_recv);
  nghttp2_session_callbacks_set_on_invalid_frame_recv_callback(cbs,
    cf_h2_on_invalid_frame_recv);
#ifdef CURLVERBOSE
  nghttp2_session_callbacks_set_on_frame_send_callback(cbs, on_frame_send);
#endif
  nghttp2_session_callbacks_set_on_তথ্য_chunk_recv_callback(
    cbs, on_তথ্য_chunk_recv);
  nghttp2_session_callbacks_set_on_stream_close_callback(cbs, on_stream_close);
  nghttp2_session_callbacks_set_on_header_callback(cbs, on_header);
#ifdef CURLVERBOSE
  nghttp2_session_callbacks_set_error_callback(cbs, error_callback);
#endif

  /* The nghttp2 session is not yet setup, do it */
  rc = h2_client_new(cf, cbs);
  যদি(rc) {
    failf(তথ্য, "Could not initialize nghttp2");
    যাও out;
  }
  ctx->max_concurrent_streams = তথ্য->multi ?
    Curl_multi_max_concurrent_streams(তথ্য->multi) :
    DEFAULT_MAX_CONCURRENT_STREAMS;

  যদি(ctx->via_h1_upgrade) {
    /* HTTP/1.1 Upgrade issued. H2 Settings have already been submitted
     * in the H1 request and we upgrade from there. This stream
     * is opened implicitly as #1. */
    uint8_t binsettings[H2_BINSETTINGS_LEN];
    ssize_t rclen;
    size_t binlen; /* length of the binsettings data */

    rclen = populate_binsettings(binsettings, তথ্য);

    যদি(!curlx_sztouz(rclen, &binlen) || !binlen) {
      failf(তথ্য, "nghttp2 unexpectedly failed on pack_settings_payload");
      ফলাফল = CURLE_FAILED_INIT;
      যাও out;
    }

    ফলাফল = http2_তথ্য_setup(cf, তথ্য, &stream);
    যদি(ফলাফল)
      যাও out;
    DEBUGASSERT(stream);
    stream->id = 1;
    /* queue SETTINGS frame (again) */
    rc = Curl_ngh2_session_upgrade2(
      &ctx->ngh2, binsettings, binlen, তথ্য->state.httpreq == HTTPREQ_শীর্ষ,
      ফাঁকা);
    যদি(rc) {
      failf(তথ্য, "nghttp2_session_upgrade2() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      ফলাফল = CURLE_HTTP2;
      যাও out;
    }

    rc = Curl_ngh2_session_set_stream_user_তথ্য(
      &ctx->ngh2, stream->id, stream);
    যদি(rc) {
      infof(তথ্য, "http/2: failed to set user_data for stream %d",
            stream->id);
      DEBUGASSERT(0);
    }
    CURL_TRC_CF(তথ্য, cf, "created session via Upgrade");
  }
  অন্যথা {
    nghttp2_settings_entry iv[H2_SETTINGS_IV_LEN];
    size_t ivlen;

    ivlen = populate_settings(iv, তথ্য, ctx);
    rc = Curl_ngh2_submit_settings(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, iv, ivlen);
    যদি(rc) {
      failf(তথ্য, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      ফলাফল = CURLE_HTTP2;
      যাও out;
    }
  }

  rc = Curl_ngh2_session_set_local_window_আকার(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, HTTP2_HUGE_WINDOW_আকার);
  যদি(rc) {
    failf(তথ্য, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    ফলাফল = CURLE_HTTP2;
    যাও out;
  }

  /* all set, traffic will be send on connect */
  ফলাফল = CURLE_OK;
  CURL_TRC_CF(তথ্য, cf, "[0] created h2 session%s",
              ctx->via_h1_upgrade ? " (via h1 upgrade)" : "");

out:
  যদি(cbs)
    nghttp2_session_callbacks_del(cbs);
  ফেরত ফলাফল;
}

স্থির CURLcode cf_h2_connect(কাঠামো Curl_cfilter *cf,
                              কাঠামো Curl_easy *তথ্য,
                              bool *done)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো cf_call_তথ্য save;
  bool first_time = FALSE;

  যদি(cf->connected) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  /* Connect the lower filters first */
  যদি(!cf->পরবর্তী->connected) {
    ফলাফল = Curl_conn_cf_connect(cf->পরবর্তী, তথ্য, done);
    যদি(ফলাফল || !*done)
      ফেরত ফলাফল;
  }

  *done = FALSE;

  CF_তথ্য_SAVE(save, cf, তথ্য);
  DEBUGASSERT(ctx->initialized);
  যদি(!ctx->ngh2.s) {
    ফলাফল = cf_h2_ctx_open(cf, তথ্য);
    যদি(ফলাফল)
      যাও out;
    first_time = TRUE;
  }

  যদি(!first_time) {
    ফলাফল = h2_progress_ingress(cf, তথ্য, 0);
    যদি(ফলাফল)
      যাও out;
  }

  /* Send out our SETTINGS and ACKs and such. If that blocks, we
   * have it buffered and can count this filter as being connected */
  ফলাফল = h2_progress_egress(cf, তথ্য);
  যদি(ফলাফল && (ফলাফল != CURLE_AGAIN))
    যাও out;

  *done = TRUE;
  cf->connected = TRUE;
  ফলাফল = CURLE_OK;

out:
  CURL_TRC_CF(তথ্য, cf, "cf_connect() -> %d, %d, ", (পূর্ণসংখ্যা)ফলাফল, *done);
  CF_তথ্য_RESTORE(cf, save);
  ফেরত ফলাফল;
}

স্থির শূন্য cf_h2_destroy(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;

  (শূন্য)তথ্য;
  যদি(ctx) {
    cf_h2_ctx_free(ctx);
    cf->ctx = ফাঁকা;
  }
}

স্থির CURLcode cf_h2_shutdown(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য, bool *done)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  কাঠামো cf_call_তথ্য save;
  CURLcode ফলাফল;
  পূর্ণসংখ্যা rv;

  যদি(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  CF_তথ্য_SAVE(save, cf, তথ্য);

  যদি(!ctx->sent_goaway) {
    ctx->sent_goaway = TRUE;
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, ctx->local_max_sid, 0,
      (ধ্রুবক uint8_t *)"shutdown", আকার("shutdown"));
    যদি(rv) {
      failf(তথ্য, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      ফলাফল = CURLE_SEND_ERROR;
      যাও out;
    }
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  ফলাফল = CURLE_OK;
  যদি(Curl_ngh2_session_want_write(&ctx->ngh2) ||
     !Curl_bufq_is_empty(&ctx->outbufq))
    ফলাফল = h2_progress_egress(cf, তথ্য);
  যদি(!ফলাফল && Curl_ngh2_session_want_read(&ctx->ngh2))
    ফলাফল = h2_progress_ingress(cf, তথ্য, 0);

  যদি(ফলাফল == CURLE_AGAIN)
    ফলাফল = CURLE_OK;

  *done = (ctx->conn_closed ||
           (!ফলাফল && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2) &&
            Curl_bufq_is_empty(&ctx->outbufq)));

out:
  CF_তথ্য_RESTORE(cf, save);
  cf->shutdown = (ফলাফল || *done);
  ফেরত ফলাফল;
}

স্থির CURLcode http2_তথ্য_pause(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 bool pause)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  কাঠামো h2_stream_ctx *stream = H2_STREAM_CTX(ctx, তথ্য);

  DEBUGASSERT(তথ্য);
  যদি(ctx && Curl_ngh2_valid(&ctx->ngh2) && stream) {
    CURLcode ফলাফল;

    stream->write_paused = pause;
    ফলাফল = cf_h2_update_local_win(cf, তথ্য, stream);
    যদি(ফলাফল)
      ফেরত ফলাফল;

    /* attempt to send the window update */
    (শূন্য)h2_progress_egress(cf, তথ্য);

    যদি(!pause) {
      /* Unpausing a h2 transfer, requires it to be run again. The server
       * may send new DATA on us increasing the flow window, and it may
       * not. We may have already buffered and exhausted the new window
       * by operating on things in flight during the handling of other
       * transfers. */
      Curl_multi_mark_dirty(তথ্য);
    }
    CURL_TRC_CF(তথ্য, cf, "[%d] stream now %spaused", stream->id,
                pause ? "" : "un");
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode cf_h2_cntrl(কাঠামো Curl_cfilter *cf,
                            কাঠামো Curl_easy *তথ্য,
                            পূর্ণসংখ্যা event, পূর্ণসংখ্যা arg1, শূন্য *arg2)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো cf_call_তথ্য save;

  (শূন্য)arg2;

  CF_তথ্য_SAVE(save, cf, তথ্য);
  নির্বাচন(event) {
  ক্ষেত্র CF_CTRL_তথ্য_SETUP:
    বিরতি;
  ক্ষেত্র CF_CTRL_তথ্য_PAUSE:
    ফলাফল = http2_তথ্য_pause(cf, তথ্য, (arg1 != 0));
    বিরতি;
  ক্ষেত্র CF_CTRL_FLUSH:
    ফলাফল = cf_h2_flush(cf, তথ্য);
    বিরতি;
  ক্ষেত্র CF_CTRL_তথ্য_DONE:
    http2_তথ্য_done(cf, তথ্য);
    বিরতি;
  ক্ষেত্র CF_CTRL_CONN_INFO_UPDATE:
    যদি(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 20;
      Curl_conn_set_multiplex(cf->conn);
    }
    বিরতি;
  পূর্বনির্ধারিত:
    বিরতি;
  }
  CF_তথ্য_RESTORE(cf, save);
  ফেরত ফলাফল;
}

স্থির bool cf_h2_তথ্য_pending(কাঠামো Curl_cfilter *cf,
                               ধ্রুবক কাঠামো Curl_easy *তথ্য)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;

  যদি(ctx && !Curl_bufq_is_empty(&ctx->inbufq))
    ফেরত TRUE;
  ফেরত cf->পরবর্তী ? cf->পরবর্তী->cft->has_তথ্য_pending(cf->পরবর্তী, তথ্য) : FALSE;
}

স্থির bool cf_h2_is_alive(কাঠামো Curl_cfilter *cf,
                           কাঠামো Curl_easy *তথ্য,
                           bool *প্রবেশ_pending)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  bool alive;
  কাঠামো cf_call_তথ্য save;

  *প্রবেশ_pending = FALSE;
  CF_তথ্য_SAVE(save, cf, তথ্য);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
           http2_connisalive(cf, তথ্য, প্রবেশ_pending));
  CURL_TRC_CF(তথ্য, cf, "conn alive -> %d, input_pending=%d",
              alive, *প্রবেশ_pending);
  CF_তথ্য_RESTORE(cf, save);
  ফেরত alive;
}

স্থির CURLcode cf_h2_keep_alive(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য)
{
  CURLcode ফলাফল;
  কাঠামো cf_call_তথ্য save;

  CF_তথ্য_SAVE(save, cf, তথ্য);
  ফলাফল = http2_send_ping(cf, তথ্য);
  CF_তথ্য_RESTORE(cf, save);
  ফেরত ফলাফল;
}

স্থির CURLcode cf_h2_query(কাঠামো Curl_cfilter *cf,
                            কাঠামো Curl_easy *তথ্য,
                            পূর্ণসংখ্যা query, পূর্ণসংখ্যা *pres1, শূন্য *pres2)
{
  কাঠামো cf_h2_ctx *ctx = cf->ctx;
  কাঠামো cf_call_তথ্য save;
  size_t effective_max;

  নির্বাচন(query) {
  ক্ষেত্র CF_QUERY_MAX_CONCURRENT:
    DEBUGASSERT(pres1);

    CF_তথ্য_SAVE(save, cf, তথ্য);
    যদি(!Curl_ngh2_valid(&ctx->ngh2) ||
       !Curl_ngh2_session_check_request_allowed(&ctx->ngh2)) {
      /* the limit is what we have in use right now */
      effective_max = cf->conn->attached_xfers;
    }
    অন্যথা {
      effective_max = ctx->max_concurrent_streams;
    }
    *pres1 = (effective_max > INT_MAX) ? INT_MAX : (পূর্ণসংখ্যা)effective_max;
    CF_তথ্য_RESTORE(cf, save);
    ফেরত CURLE_OK;
  ক্ষেত্র CF_QUERY_STREAM_ERROR: {
    কাঠামো h2_stream_ctx *stream = H2_STREAM_CTX(ctx, তথ্য);
    *pres1 = stream ? (পূর্ণসংখ্যা)stream->error : 0;
    ফেরত CURLE_OK;
  }
  ক্ষেত্র CF_QUERY_NEED_FLUSH: {
    কাঠামো h2_stream_ctx *stream = H2_STREAM_CTX(ctx, তথ্য);
    যদি(!Curl_bufq_is_empty(&ctx->outbufq) ||
       (stream && !Curl_bufq_is_empty(&stream->sendbuf))) {
      *pres1 = TRUE;
      ফেরত CURLE_OK;
    }
    বিরতি;
  }
  ক্ষেত্র CF_QUERY_HTTP_VERSION:
    *pres1 = 20;
    ফেরত CURLE_OK;
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত cf->পরবর্তী ?
    cf->পরবর্তী->cft->query(cf->পরবর্তী, তথ্য, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

কাঠামো Curl_cftype Curl_cft_nghttp2 = {
  "HTTP/2",
  CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  CURL_LOG_LVL_NONE,
  cf_h2_destroy,
  cf_h2_connect,
  cf_h2_shutdown,
  cf_h2_adjust_pollset,
  cf_h2_তথ্য_pending,
  cf_h2_send,
  cf_h2_recv,
  cf_h2_cntrl,
  cf_h2_is_alive,
  cf_h2_keep_alive,
  cf_h2_query,
};

স্থির CURLcode http2_cfilter_add(কাঠামো Curl_cfilter **pcf,
                                  কাঠামো Curl_easy *তথ্য,
                                  কাঠামো connectdata *conn,
                                  int8_t sockindex,
                                  bool via_h1_upgrade)
{
  কাঠামো Curl_cfilter *cf = ফাঁকা;
  কাঠামো cf_h2_ctx *ctx;
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(তথ্য->conn);
  ctx = curlx_calloc(1, আকার(*ctx));
  যদি(!ctx)
    যাও out;
  cf_h2_ctx_init(ctx, via_h1_upgrade);

  ফলাফল = Curl_cf_create(&cf, &Curl_cft_nghttp2, ctx);
  যদি(ফলাফল)
    যাও out;

  ctx = ফাঁকা;
  Curl_conn_cf_add(তথ্য, conn, sockindex, cf);

out:
  যদি(ফলাফল)
    cf_h2_ctx_free(ctx);
  *pcf = ফলাফল ? ফাঁকা : cf;
  ফেরত ফলাফল;
}

স্থির CURLcode http2_cfilter_ঢোকাও_after(কাঠামো Curl_cfilter *cf,
                                           কাঠামো Curl_easy *তথ্য,
                                           bool via_h1_upgrade)
{
  কাঠামো Curl_cfilter *cf_h2 = ফাঁকা;
  কাঠামো cf_h2_ctx *ctx;
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;

  (শূন্য)তথ্য;
  ctx = curlx_calloc(1, আকার(*ctx));
  যদি(!ctx)
    যাও out;
  cf_h2_ctx_init(ctx, via_h1_upgrade);

  ফলাফল = Curl_cf_create(&cf_h2, &Curl_cft_nghttp2, ctx);
  যদি(ফলাফল)
    যাও out;

  ctx = ফাঁকা;
  Curl_conn_cf_ঢোকাও_after(cf, cf_h2);

out:
  যদি(ফলাফল)
    cf_h2_ctx_free(ctx);
  ফেরত ফলাফল;
}

bool Curl_http2_may_switch(কাঠামো Curl_easy *তথ্য)
{
  যদি(Curl_conn_http_version(তথ্য, তথ্য->conn) < 20 &&
     (তথ্য->state.http_neg.wanted & CURL_HTTP_V2x) &&
     তথ্য->state.http_neg.h2_prior_knowledge) {
#ifndef CURL_DISABLE_PROXY
    যদি(তথ্য->conn->bits.origin_is_proxy) {
      infof(তথ্য, "Ignoring HTTP/2 prior knowledge due to proxy");
      ফেরত FALSE;
    }
#endif
    ফেরত TRUE;
  }
  ফেরত FALSE;
}

CURLcode Curl_http2_switch(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_cfilter *cf;
  CURLcode ফলাফল;

  DEBUGASSERT(Curl_conn_http_version(তথ্য, তথ্য->conn) < 20);

  ফলাফল = http2_cfilter_add(&cf, তথ্য, তথ্য->conn, FIRSTSOCKET, FALSE);
  যদি(ফলাফল)
    ফেরত ফলাফল;
  CURL_TRC_CF(তথ্য, cf, "switching connection to HTTP/2");

  যদি(cf->পরবর্তী) {
    bool done;
    ফেরত Curl_conn_cf_connect(cf, তথ্য, &done);
  }
  ফেরত CURLE_OK;
}

CURLcode Curl_http2_switch_at(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_cfilter *cf_h2;
  CURLcode ফলাফল;

  DEBUGASSERT(Curl_conn_http_version(তথ্য, তথ্য->conn) < 20);

  ফলাফল = http2_cfilter_ঢোকাও_after(cf, তথ্য, FALSE);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  cf_h2 = cf->পরবর্তী;

  যদি(cf_h2->পরবর্তী) {
    bool done;
    ফেরত Curl_conn_cf_connect(cf_h2, তথ্য, &done);
  }
  ফেরত CURLE_OK;
}

CURLcode Curl_http2_upgrade(কাঠামো Curl_easy *তথ্য,
                            কাঠামো connectdata *conn, int8_t sockindex,
                            ধ্রুবক অক্ষর *mem, size_t nread)
{
  কাঠামো Curl_cfilter *cf;
  কাঠামো cf_h2_ctx *ctx;
  CURLcode ফলাফল;

  DEBUGASSERT(Curl_conn_http_version(তথ্য, conn) < 20);

  ফলাফল = http2_cfilter_add(&cf, তথ্য, conn, sockindex, TRUE);
  যদি(ফলাফল)
    ফেরত ফলাফল;
  CURL_TRC_CF(তথ্য, cf, "upgrading connection to HTTP/2");

  DEBUGASSERT(cf->cft == &Curl_cft_nghttp2);
  ctx = cf->ctx;

  তথ্য->req.httpversion_sent = 20; /* it is an h2 request now */
  তথ্য->req.header = TRUE; /* we expect the real response to come in h2 */
  তথ্য->req.headerline = 0; /* restart the header line counter */

  যদি(nread > 0) {
    /* Remaining data from the protocol switch reply is already using
     * the switched protocol, ie. HTTP/2. We add that to the network
     * inbufq. */
    size_t copied;

    ফলাফল = Curl_bufq_write(&ctx->inbufq,
                             (ধ্রুবক unsigned অক্ষর *)mem, nread, &copied);
    যদি(ফলাফল) {
      failf(তথ্য, "error on copying HTTP Upgrade response: %d", (পূর্ণসংখ্যা)ফলাফল);
      ফেরত CURLE_RECV_ERROR;
    }
    যদি(copied < nread) {
      failf(তথ্য, "connection buffer size could not take all data "
            "from HTTP Upgrade response header: copied=%zu, datalen=%zu",
            copied, nread);
      ফেরত CURLE_HTTP2;
    }
    infof(তথ্য, "Copied HTTP/2 data in stream buffer to connection buffer"
          " after upgrade: len=%zu", nread);
  }

  যদি(cf->পরবর্তী) {
    bool done;
    ফলাফল = Curl_conn_cf_connect(cf, তথ্য, &done);
    যদি(!ফলাফল)
      cf->cft->cntrl(cf, তথ্য, CF_CTRL_CONN_INFO_UPDATE, 0, ফাঁকা);
  }
  ফেরত ফলাফল;
}

/* Only call this function for a transfer that already got an HTTP/2
   CURLE_HTTP2_STREAM error! */
bool Curl_h2_http_1_1_error(কাঠামো Curl_easy *তথ্য)
{
  যদি(Curl_conn_http_version(তথ্য, তথ্য->conn) == 20) {
    পূর্ণসংখ্যা err = Curl_conn_get_stream_error(তথ্য, তথ্য->conn, FIRSTSOCKET);
    ফেরত err == NGHTTP2_HTTP_1_1_REQUIRED;
  }
  ফেরত FALSE;
}

শূন্য *Curl_nghttp2_malloc(size_t আকার_2, শূন্য *user_তথ্য)
{
  (শূন্য)user_তথ্য;
  ফেরত Curl_cmalloc(আকার_2);
}

শূন্য Curl_nghttp2_free(শূন্য *ptr, শূন্য *user_তথ্য)
{
  (শূন্য)user_তথ্য;
  Curl_cfree(ptr);
}

শূন্য *Curl_nghttp2_calloc(size_t nmemb, size_t আকার_2, শূন্য *user_তথ্য)
{
  (শূন্য)user_তথ্য;
  ফেরত Curl_ccalloc(nmemb, আকার_2);
}

শূন্য *Curl_nghttp2_realloc(শূন্য *ptr, size_t আকার_2, শূন্য *user_তথ্য)
{
  (শূন্য)user_তথ্য;
  ফেরত Curl_crealloc(ptr, আকার_2);
}

#অন্যথা /* CURL_DISABLE_HTTP || !USE_NGHTTP2 */

/* Satisfy external references even if http2 is not compiled in. */

অক্ষর *curl_pushheader_bynum(কাঠামো curl_pushheaders *h, size_t অঙ্ক)
{
  (শূন্য)h;
  (শূন্য)অঙ্ক;
  ফেরত ফাঁকা;
}

অক্ষর *curl_pushheader_byname(কাঠামো curl_pushheaders *h, ধ্রুবক অক্ষর *name)
{
  (শূন্য)h;
  (শূন্য)name;
  ফেরত ফাঁকা;
}

#endif /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
