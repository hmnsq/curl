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
#अन्तर्भाव "curl_setup.h"

#जइ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#अन्तर्भाव <nghttp2/nghttp2.h>

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "bufq.h"
#अन्तर्भाव "u32_ptrset.h"
#अन्तर्भाव "http1.h"
#अन्तर्भाव "http2.h"
#अन्तर्भाव "http.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "curlx/base64.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "ngh2_shim.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "urlapi-int.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "bufref.h"
#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "headers.h"
#अन्तर्भाव "curl_share.h"

#जइ NGHTTP2_VERSION_अङ्कः < 0x010f00
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
#जइ NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
#define H2_STREAM_WINDOW_SIZE_INITIAL  (64 * 1024)
#अण्णहा
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

संरचना cf_h2_ctx {
  संरचना curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  संरचना cf_call_data call_data;

  संरचना bufq inbufq;           /* network input */
  संरचना bufq outbufq;          /* network output */
  संरचना bufc_pool stream_bufcp; /* spares for stream buffers */
  संरचना dynbuf scratch;        /* scratch buffer for temp use */

  संरचना u32_ptrset streams; /* hash of `data->mid` to `h2_stream_ctx` */
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
  BIT(enable_प्रक्षिप);
  BIT(nw_out_blocked);
};

/* How to access `call_data` from a cf_h2 filter */
#undef CF_CTX_CALL_DATA
#define CF_CTX_CALL_DATA(cf) ((संरचना cf_h2_ctx *)(cf)->ctx)->call_data

/**
 * All about the H2 internals of a stream
 */
संरचना h2_stream_ctx {
  संरचना bufq sendbuf; /* request buffer */
  संरचना h1_req_parser h1; /* parsing the request */
  संरचना dynhds resp_trailers; /* response trailer fields */
  size_t resp_hds_len; /* amount of response header bytes in recvbuf */
  curl_off_t xfer_id;  /* transfer `id` owning this stream */
  curl_off_t nrcvd_data;  /* number of DATA bytes received */

  वर्ण **प्रक्षिप_headers;       /* allocated array */
  size_t प्रक्षिप_headers_used;  /* number of entries filled in */
  size_t प्रक्षिप_headers_alloc; /* number of entries allocated */

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

स्थिर शून्य free_प्रक्षिप_headers(संरचना h2_stream_ctx *stream)
{
  size_t i;
  प्रति(i = 0; i < stream->प्रक्षिप_headers_used; i++)
    curlx_free(stream->प्रक्षिप_headers[i]);
  curlx_safefree(stream->प्रक्षिप_headers);
  stream->प्रक्षिप_headers_used = 0;
}

स्थिर शून्य h2_stream_ctx_free(संरचना h2_stream_ctx *stream)
{
  Curl_bufq_free(&stream->sendbuf);
  Curl_h1_req_parse_free(&stream->h1);
  Curl_dynhds_free(&stream->resp_trailers);
  free_प्रक्षिप_headers(stream);
  curlx_free(stream);
}

स्थिर शून्य h2_stream_hash_free(uint32_t id, शून्य *stream)
{
  (शून्य)id;
  DEBUGASSERT(stream);
  h2_stream_ctx_free((संरचना h2_stream_ctx *)stream);
}

स्थिर शून्य cf_h2_ctx_init(संरचना cf_h2_ctx *ctx, bool via_h1_upgrade)
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

स्थिर शून्य cf_h2_ctx_free(संरचना cf_h2_ctx *ctx)
{
  जइ(ctx && ctx->initialized) {
    Curl_ngh2_del(&ctx->ngh2);
    Curl_bufq_free(&ctx->inbufq);
    Curl_bufq_free(&ctx->outbufq);
    Curl_bufcp_free(&ctx->stream_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    memset(ctx, 0, परिमाण(*ctx));
  }
  curlx_free(ctx);
}

स्थिर uint32_t cf_h2_initial_win_size(संरचना Curl_easy *data)
{
#जइ NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
  /* If the transfer has a rate-limit lower than the default initial
   * stream window size, use that. It needs to be at least 8k or servers
   * may be unhappy. */
  curl_off_t rps = Curl_rlimit_per_step(&data->progress.dl.rlimit);
  जइ((rps > 0) && (rps < H2_STREAM_WINDOW_SIZE_INITIAL))
    प्रत्यागम CURLMAX((uint32_t)rps, 8192);
#endif
  प्रत्यागम H2_STREAM_WINDOW_SIZE_INITIAL;
}

स्थिर size_t populate_settings(nghttp2_settings_entry *iv,
                                संरचना Curl_easy *data,
                                संरचना cf_h2_ctx *ctx)
{
  iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
  iv[0].मानम् = Curl_multi_max_concurrent_streams(data->multi);

  iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
  iv[1].मानम् = cf_h2_initial_win_size(data);
  जइ(ctx)
    ctx->initial_win_size = iv[1].मानम्;
  iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_प्रक्षिप;
  iv[2].मानम् = !!data->multi->प्रक्षिप_cb;

  प्रत्यागम 3;
}

स्थिर ssize_t populate_binsettings(uint8_t *binsettings,
                                    संरचना Curl_easy *data)
{
  nghttp2_settings_entry iv[H2_SETTINGS_IV_LEN];
  size_t ivlen;

  ivlen = populate_settings(iv, data, रिक्त);
  /* this returns number of bytes it wrote or a negative number on error. */
  प्रत्यागम nghttp2_pack_settings_payload(binsettings, H2_BINSETTINGS_LEN,
                                       iv, ivlen);
}

स्थिर CURLcode cf_h2_update_settings(संरचना cf_h2_ctx *ctx,
                                      uint32_t initial_win_size)
{
  nghttp2_settings_entry entry;
  entry.settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
  entry.मानम् = initial_win_size;
  जइ(Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, &entry, 1))
    प्रत्यागम CURLE_SEND_ERROR;
  ctx->initial_win_size = initial_win_size;
  प्रत्यागम CURLE_OK;
}

#define H2_STREAM_CTX(ctx, data)                                         \
  ((संरचना h2_stream_ctx *)(                                             \
    (data) ? Curl_u32_ptrset_get(&(ctx)->streams, (data)->mid) : रिक्त))

स्थिर संरचना h2_stream_ctx *h2_stream_ctx_create(संरचना cf_h2_ctx *ctx,
                                                  संरचना Curl_easy *data)
{
  संरचना h2_stream_ctx *stream;

  stream = curlx_calloc(1, परिमाण(*stream));
  जइ(!stream)
    प्रत्यागम रिक्त;

  stream->id = -1;
  stream->xfer_id = data->id;
  stream->mid = data->mid;
  Curl_bufq_initp(&stream->sendbuf, &ctx->stream_bufcp,
                  H2_STREAM_SEND_CHUNKS, BUFQ_OPT_NONE);
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_पङ्क्तिः_LEN);
  Curl_dynhds_init(&stream->resp_trailers,
                   MAX_HTTP_RESP_HEADER_गणनम्, DYN_HTTP_REQUEST);
  stream->bodystarted = FALSE;
  stream->status_code = -1;
  stream->closed = FALSE;
  stream->close_handled = FALSE;
  stream->error = NGHTTP2_NO_ERROR;
  stream->local_window_size = H2_STREAM_WINDOW_SIZE_INITIAL;
  stream->nrcvd_data = 0;
  प्रत्यागम stream;
}

स्थिर संरचना Curl_easy *cf_h2_get_stream_xfer(संरचना Curl_cfilter *cf,
                                               int32_t stream_id,
                                               संरचना h2_stream_ctx **pstream)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना h2_stream_ctx *stream =
    Curl_ngh2_session_get_stream_user_data(&ctx->ngh2, stream_id);
  जइ(pstream)
    *pstream = stream;
  जइ(stream && (stream->mid != UINT32_MAX)) {
    संरचना Curl_easy *data, *call_data = CF_DATA_CURRENT(cf);
    जइ(!call_data || !call_data->multi) {
      DEBUGASSERT(0);
      प्रत्यागम रिक्त;
    }
    data = Curl_multi_get_easy(call_data->multi, stream->mid);
    जइ(data && (data->id == stream->xfer_id))
      प्रत्यागम data;
  }
  प्रत्यागम रिक्त;
}

#ifdef NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE
स्थिर int32_t cf_h2_get_desired_local_win(संरचना Curl_cfilter *cf,
                                           संरचना Curl_easy *data)
{
  curl_off_t avail = Curl_rlimit_avail(&data->progress.dl.rlimit, रिक्त);

  (शून्य)cf;
  जइ(avail < CURL_OFF_T_MAX) { /* limit in place */
    जइ(avail <= 0)
      प्रत्यागम 0;
    अण्णहा जइ(avail < H2_STREAM_WINDOW_SIZE_MAX)
      प्रत्यागम (int32_t)avail;
  }
  प्रत्यागम H2_STREAM_WINDOW_SIZE_MAX;
}

स्थिर CURLcode cf_h2_update_local_win(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *data,
                                       संरचना h2_stream_ctx *stream)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  int32_t dwsize;
  पूर्णाङ्क rv;

  dwsize = (stream->write_paused || stream->xfer_result) ?
           0 : cf_h2_get_desired_local_win(cf, data);
  जइ(dwsize != stream->local_window_size) {
    int32_t wsize = Curl_ngh2_session_get_stream_effective_local_window_size(
      &ctx->ngh2, stream->id);
    जइ(dwsize > wsize) {
      rv = Curl_ngh2_session_set_local_window_size(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize);
      जइ(rv) {
        failf(data, "[%d] nghttp2 set_local_window_size(%d) failed: "
              "%s(%d)", stream->id, dwsize, nghttp2_strerror(rv), rv);
        प्रत्यागम CURLE_HTTP2;
      }
      rv = Curl_ngh2_submit_window_update(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize - wsize);
      जइ(rv) {
        failf(data, "[%d] nghttp2_submit_window_update() failed: "
              "%s(%d)", stream->id, nghttp2_strerror(rv), rv);
        प्रत्यागम CURLE_HTTP2;
      }
      stream->local_window_size = dwsize;
      CURL_TRC_CF(data, cf, "[%d] local window update by %d",
                  stream->id, dwsize - wsize);
    }
    अण्णहा {
      rv = Curl_ngh2_session_set_local_window_size(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize);
      जइ(rv) {
        failf(data, "[%d] nghttp2_session_set_local_window_size() failed: "
              "%s(%d)", stream->id, nghttp2_strerror(rv), rv);
        प्रत्यागम CURLE_HTTP2;
      }
      stream->local_window_size = dwsize;
      CURL_TRC_CF(data, cf, "[%d] local window size now %d",
                  stream->id, dwsize);
    }
  }
  प्रत्यागम CURLE_OK;
}

#अण्णहा /* NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

स्थिर CURLcode cf_h2_update_local_win(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *data,
                                       संरचना h2_stream_ctx *stream)
{
  (शून्य)cf;
  (शून्य)data;
  (शून्य)stream;
  प्रत्यागम CURLE_OK;
}
#endif /* !NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

स्थिर CURLcode http2_data_setup(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 संरचना h2_stream_ctx **pstream)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना h2_stream_ctx *stream;

  (शून्य)cf;
  DEBUGASSERT(data);

  जइ(!data)
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;

  stream = H2_STREAM_CTX(ctx, data);
  जइ(stream) {
    *pstream = stream;
    प्रत्यागम CURLE_OK;
  }

  stream = h2_stream_ctx_create(ctx, data);
  जइ(!stream)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  जइ(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    h2_stream_ctx_free(stream);
    प्रत्यागम CURLE_OUT_OF_MEMORY;
  }

  *pstream = stream;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode nw_out_flush(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *data)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode result;

  जइ(Curl_bufq_is_empty(&ctx->outbufq))
    प्रत्यागम CURLE_OK;

  result = Curl_cf_send_bufq(cf->next, data, &ctx->outbufq, रिक्त, 0,
                             &nwritten);
  जइ(result) {
    जइ(result == CURLE_AGAIN) {
      CURL_TRC_CF(data, cf, "flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_out_blocked = 1;
    }
    प्रत्यागम result;
  }
  प्रत्यागम Curl_bufq_is_empty(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

स्थिर शून्य http2_data_done(संरचना Curl_cfilter *cf, संरचना Curl_easy *data)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);

  DEBUGASSERT(ctx);
  जइ(!stream || !ctx->initialized)
    प्रत्यागम;

  जइ(Curl_ngh2_valid(&ctx->ngh2)) {
    bool flush_egress = FALSE;
    /* returns error if stream not known, which is fine here */
    (शून्य)Curl_ngh2_session_set_stream_user_data(&ctx->ngh2, stream->id, रिक्त);

    जइ(!stream->closed && stream->id > 0) {
      /* RST_STREAM */
      CURL_TRC_CF(data, cf, "[%d] premature DATA_DONE, RST stream",
                  stream->id);
      stream->closed = TRUE;
      stream->reset = TRUE;
      Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                  stream->id, NGHTTP2_STREAM_CLOSED);
      flush_egress = TRUE;
    }

    जइ(flush_egress) {
      (शून्य)Curl_ngh2_session_send(&ctx->ngh2);
      (शून्य)nw_out_flush(cf, data);
    }
  }

  Curl_u32_ptrset_unset(&ctx->streams, data->mid);
}

स्थिर पूर्णाङ्क h2_client_new(संरचना Curl_cfilter *cf,
                         nghttp2_session_callbacks *cbs)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { रिक्त, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  पूर्णाङ्क rc = nghttp2_option_new(&o);
  जइ(rc)
    प्रत्यागम rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_set_no_auto_window_update(o, 1);
#जइ NGHTTP2_VERSION_अङ्कः >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_set_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#endif
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  प्रत्यागम rc;
}

/*
 * Returns nonzero if current HTTP/2 session should be closed.
 */
स्थिर पूर्णाङ्क should_close_session(संरचना cf_h2_ctx *ctx)
{
  प्रत्यागम ctx->drain_total == 0 && !Curl_ngh2_session_want_read(&ctx->ngh2) &&
    !Curl_ngh2_session_want_write(&ctx->ngh2);
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
स्थिर CURLcode h2_process_pending_निवेशः(संरचना Curl_cfilter *cf,
                                         संरचना Curl_easy *data)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  ध्रुव unsigned वर्ण *buf;
  size_t blen, nread;
  ssize_t rv;

  जाव(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {
    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ध्रुव uint8_t *)buf, blen);
    जइ(!curlx_sztouz(rv, &nread)) {
      failf(data, "nghttp2 recv error %zd: %s", rv, nghttp2_strerror((पूर्णाङ्क)rv));
      प्रत्यागम CURLE_RECV_ERROR;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    जइ(Curl_bufq_is_empty(&ctx->inbufq)) {
      भङ्ग;
    }
    अण्णहा {
      CURL_TRC_CF(data, cf, "process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }

  जइ(Curl_ngh2_session_check_request_allowed(&ctx->ngh2) == 0) {
    /* No more requests are allowed in the current session, so
       the connection may not be reused. This is set when a
       GOAWAY frame has been received or when the limit of stream
       identifiers has been reached. */
    CURL_TRC_M(data, "http/2: No new requests allowed");
    connclose(cf->conn);
  }

  प्रत्यागम CURLE_OK;
}

/*
 * The server may send us data at any point (e.g. PING frames). Therefore, we
 * cannot assume that an HTTP/2 socket is dead because it is readable.
 *
 * Check the lower filters first and, if successful, peek at the socket
 * and distinguish between closed and data.
 */
स्थिर bool http2_connisalive(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                              bool *निवेशः_pending)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *निवेशः_pending = FALSE;
  जइ(!cf->next || !cf->next->cft->is_alive(cf->next, data, निवेशः_pending))
    प्रत्यागम FALSE;

  जइ(*निवेशः_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode result;
    size_t nread;

    *निवेशः_pending = FALSE;
    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(data, cf, "connisalive, recv pending input -> %d, %zu",
                (पूर्णाङ्क)result, nread);
    जइ(!result) {
      result = h2_process_pending_निवेशः(cf, data);
      जइ(result)
        /* immediate error, considered dead */
        alive = FALSE;
      अण्णहा {
        alive = !should_close_session(ctx);
      }
    }
    अण्णहा जइ(result != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  प्रत्यागम alive;
}

स्थिर CURLcode http2_send_ping(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  पूर्णाङ्क rc;

  rc = Curl_ngh2_submit_ping(&ctx->ngh2, 0, ZERO_NULL);
  जइ(rc) {
    failf(data, "nghttp2_submit_ping() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    प्रत्यागम CURLE_HTTP2;
  }

  rc = Curl_ngh2_session_send(&ctx->ngh2);
  जइ(rc) {
    failf(data, "nghttp2_session_send() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    प्रत्यागम CURLE_SEND_ERROR;
  }
  प्रत्यागम CURLE_OK;
}

/*
 * Store nghttp2 version info in this buffer.
 */
शून्य Curl_http2_ver(वर्ण *p, size_t len)
{
  nghttp2_info *h2 = nghttp2_version(0);
  (शून्य)curl_msnprintf(p, len, "nghttp2/%s", h2->version_str);
}

/*
 * The implementation of nghttp2_send_callback type. Here we write |data| with
 * size |length| to the network and return the number of bytes actually
 * written. See the documentation of nghttp2_send_callback for the details.
 */
स्थिर ssize_t send_callback(nghttp2_session *h2,
                             ध्रुव uint8_t *buf, size_t blen, पूर्णाङ्क flags,
                             शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result = CURLE_OK;

  (शून्य)h2;
  (शून्य)flags;
  DEBUGASSERT(data);

  जइ(!cf->connected)
    result = Curl_bufq_write(&ctx->outbufq, buf, blen, &nwritten);
  अण्णहा
    result = Curl_cf_send_bufq(cf->next, data, &ctx->outbufq, buf, blen,
                               &nwritten);

  जइ(result) {
    जइ(result == CURLE_AGAIN) {
      ctx->nw_out_blocked = 1;
      प्रत्यागम NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(data, "Failed sending HTTP2 data");
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  जइ(!nwritten) {
    ctx->nw_out_blocked = 1;
    प्रत्यागम NGHTTP2_ERR_WOULDBLOCK;
  }
  प्रत्यागम (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

/* We pass a pointer to this struct in the push callback, but the contents of
   the struct are hidden from the user. */
संरचना curl_pushheaders {
  संरचना Curl_easy *data;
  संरचना h2_stream_ctx *stream;
  ध्रुव nghttp2_प्रक्षिप_promise *frame;
};

/*
 * push header access function. Only to be used from within the push callback
 */
वर्ण *curl_pushheader_bynum(संरचना curl_pushheaders *h, size_t अङ्कः)
{
  /* Verify that we got a good easy handle in the push header struct, mostly to
     detect rubbish input fast(er). */
  जइ(!h || !GOOD_EASY_HANDLE(h->data))
    प्रत्यागम रिक्त;
  अण्णहा {
    जइ(h->stream && अङ्कः < h->stream->प्रक्षिप_headers_used)
      प्रत्यागम h->stream->प्रक्षिप_headers[अङ्कः];
  }
  प्रत्यागम रिक्त;
}

/*
 * push header access function. Only to be used from within the push callback
 */
वर्ण *curl_pushheader_byname(संरचना curl_pushheaders *h, ध्रुव वर्ण *name)
{
  संरचना h2_stream_ctx *stream;
  size_t len;
  size_t i;
  /* Verify that we got a good easy handle in the push header struct,
     mostly to detect rubbish input fast(er). Also empty header name
     is rubbish too. We have to allow ":" at the beginning of
     the header, but header == ":" must be rejected. If we have ':' in
     the middle of header, it could be matched in middle of the value,
     this is because we do prefix match.*/
  जइ(!h || !GOOD_EASY_HANDLE(h->data) || !name || !name[0] ||
     !strcmp(name, ":") || strchr(name + 1, ':'))
    प्रत्यागम रिक्त;

  stream = h->stream;
  जइ(!stream)
    प्रत्यागम रिक्त;

  len = strlen(name);
  प्रति(i = 0; i < stream->प्रक्षिप_headers_used; i++) {
    जइ(!strncmp(name, stream->प्रक्षिप_headers[i], len)) {
      /* sub-match, make sure that it is followed by a colon */
      जइ(stream->प्रक्षिप_headers[i][len] != ':')
        अनुवर्त;
      प्रत्यागम &stream->प्रक्षिप_headers[i][len + 1];
    }
  }
  प्रत्यागम रिक्त;
}

स्थिर संरचना Curl_easy *h2_duphandle(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *data)
{
  संरचना Curl_easy *second = curl_easy_init();
  जइ(second) {
    संरचना h2_stream_ctx *second_stream;
    http2_data_setup(cf, second, &second_stream);
    second->state.weight = data->state.weight;
    जइ(data->share)
      (शून्य)Curl_share_easy_link(second, data->share);
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
  प्रत्यागम second;
}

स्थिर पूर्णाङ्क set_transfer_url(संरचना Curl_easy *newhandle,
                            संरचना curl_pushheaders *hp,
                            संरचना Curl_easy *data)
{
  ध्रुव वर्ण *v;
  CURLUcode uc;
  वर्ण *url = रिक्त;
  पूर्णाङ्क rc = 0;
  CURLU *u = curl_url();

  जइ(!u)
    प्रत्यागम 5;

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_SCHEME);
  जइ(v) {
    uc = curl_url_set(u, CURLUPART_SCHEME, v, 0);
    जइ(uc) {
      rc = 1;
      गच्छ fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_AUTHORITY);
  जइ(v) {
    uc = Curl_url_set_authority(u, v);
    जइ(uc) {
      rc = 2;
      गच्छ fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_PATH);
  जइ(v) {
    uc = curl_url_set(u, CURLUPART_PATH, v, 0);
    जइ(uc) {
      rc = 3;
      गच्छ fail;
    }
  }

  /* We can only allow PUSH of resource from the same origin, e.g.
   * scheme + hostname + port */
  जइ(!Curl_url_same_origin(data->state.uh, u)) {
    rc = 1;
    गच्छ fail;
  }

  uc = curl_url_get(u, CURLUPART_URL, &url, 0);
  जइ(uc)
    rc = 4;
fail:
  जइ(rc) {
    curl_url_cleanup(u);
    प्रत्यागम rc;
  }

  DEBUGASSERT(!newhandle->state.uh);
  newhandle->state.uh = u;
  Curl_bufref_set(&newhandle->state.url, url, 0, curl_free);
  प्रत्यागम 0;
}

स्थिर शून्य discard_newhandle(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *newhandle)
{
  http2_data_done(cf, newhandle);
  (शून्य)Curl_close(&newhandle);
}

स्थिर पूर्णाङ्क प्रक्षिप_promise(संरचना Curl_cfilter *cf,
                        संरचना Curl_easy *data,
                        ध्रुव nghttp2_प्रक्षिप_promise *frame)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  पूर्णाङ्क rv; /* one of the CURL_PUSH_* defines */

  CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE received",
              frame->promised_stream_id);
  जइ(data->multi->प्रक्षिप_cb) {
    संरचना h2_stream_ctx *stream;
    संरचना h2_stream_ctx *newstream;
    संरचना curl_pushheaders heads;
    CURLMcode mresult;
    CURLcode result;
    /* clone the parent */
    संरचना Curl_easy *newhandle = h2_duphandle(cf, data);
    जइ(!newhandle) {
      infof(data, "failed to duplicate handle");
      rv = CURL_प्रक्षिप_DENY; /* FAIL HARD */
      गच्छ fail;
    }

    stream = H2_STREAM_CTX(ctx, data);
    जइ(!stream) {
      failf(data, "Internal NULL stream");
      discard_newhandle(cf, newhandle);
      rv = CURL_प्रक्षिप_DENY;
      गच्छ fail;
    }

    heads.data = data;
    heads.stream = stream;
    heads.frame = frame;

    rv = set_transfer_url(newhandle, &heads, data);
    जइ(rv) {
      CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE, failed to set URL -> %d",
                  frame->promised_stream_id, rv);
      discard_newhandle(cf, newhandle);
      rv = CURL_प्रक्षिप_DENY;
      गच्छ fail;
    }

    {
      संरचना Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, multi_प्रक्षिप_cb);
      rv = data->multi->प्रक्षिप_cb(data, newhandle,
                                stream->प्रक्षिप_headers_used, &heads,
                                data->multi->प्रक्षिप_userp);
      CURL_CBAPI_END(&guard);
    }

    /* free the headers again */
    free_प्रक्षिप_headers(stream);

    जइ(rv) {
      DEBUGASSERT((rv > CURL_प्रक्षिप_OK) && (rv <= CURL_प्रक्षिप_ERROROUT));
      /* denied, kill off the new handle again */
      CURL_TRC_CF(data, cf, "[%d] PUSH_PROMISE, denied by application -> %d",
                  frame->promised_stream_id, rv);
      discard_newhandle(cf, newhandle);
      गच्छ fail;
    }

    /* approved, add to the multi handle for processing. This
     * assigns newhandle->mid. For the new `mid` we assign the
     * h2_stream instance and remember the stream_id already known. */
    mresult = Curl_multi_add_perform(data->multi, newhandle, cf->conn);
    जइ(mresult) {
      infof(data, "failed to add handle to multi");
      discard_newhandle(cf, newhandle);
      rv = CURL_प्रक्षिप_DENY;
      गच्छ fail;
    }

    result = http2_data_setup(cf, newhandle, &newstream);
    जइ(result) {
      failf(data, "error setting up stream: %d", (पूर्णाङ्क)result);
      discard_newhandle(cf, newhandle);
      rv = CURL_प्रक्षिप_DENY;
      गच्छ fail;
    }

    DEBUGASSERT(newstream);
    newstream->id = frame->promised_stream_id;
    newhandle->req.maxdownload = -1;
    newhandle->req.size = -1;

    CURL_TRC_CF(data, cf, "promise easy handle added to multi, mid=%u",
                newhandle->mid);
    rv = Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, newstream->id, newstream);
    जइ(rv) {
      infof(data, "failed to set user_data for stream %d",
            newstream->id);
      DEBUGASSERT(0);
      discard_newhandle(cf, newhandle);
      rv = CURL_प्रक्षिप_DENY;
      गच्छ fail;
    }

    /* success, remember max stream id processed */
    जइ(newstream->id > ctx->local_max_sid)
      ctx->local_max_sid = newstream->id;
  }
  अण्णहा {
    CURL_TRC_CF(data, cf, "Got PUSH_PROMISE, ignore it");
    rv = CURL_प्रक्षिप_DENY;
  }
fail:
  प्रत्यागम rv;
}

स्थिर शून्य h2_xfer_write_resp_hd(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  संरचना h2_stream_ctx *stream,
                                  ध्रुव वर्ण *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  जइ(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, blen, eos);
    जइ(!stream->xfer_result && !eos)
      stream->xfer_result = cf_h2_update_local_win(cf, data, stream);
    जइ(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%d] error %d writing %zu bytes of headers",
                  stream->id, (पूर्णाङ्क)stream->xfer_result, blen);
  }
}

स्थिर शून्य h2_xfer_write_resp(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data,
                               संरचना h2_stream_ctx *stream,
                               ध्रुव वर्ण *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  जइ(!stream->xfer_result)
    stream->xfer_result = Curl_xfer_write_resp(data, buf, blen, eos);
  /* If the transfer write is errored, we do not want any more data */
  जइ(stream->xfer_result) {
    संरचना cf_h2_ctx *ctx = cf->ctx;
    CURL_TRC_CF(data, cf, "[%d] error %d writing %zu bytes of data, "
                "RST-ing stream",
                stream->id, (पूर्णाङ्क)stream->xfer_result, blen);
    Curl_ngh2_submit_rst_stream(
      &ctx->ngh2, 0, stream->id, (uint32_t)NGHTTP2_ERR_CALLBACK_FAILURE);
  }
  अण्णहा जइ(!stream->write_paused && Curl_xfer_write_is_paused(data)) {
    CURL_TRC_CF(data, cf, "[%d] stream output paused", stream->id);
    stream->write_paused = TRUE;
  }
  अण्णहा जइ(stream->write_paused && !Curl_xfer_write_is_paused(data)) {
    CURL_TRC_CF(data, cf, "[%d] stream output unpaused", stream->id);
    stream->write_paused = FALSE;
  }

  जइ(!stream->xfer_result && !eos)
    stream->xfer_result = cf_h2_update_local_win(cf, data, stream);
}

स्थिर CURLcode on_stream_frame(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                संरचना h2_stream_ctx *stream,
                                ध्रुव nghttp2_frame *frame)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  int32_t stream_id = frame->hd.stream_id;
  पूर्णाङ्क rv;

  जइ(!stream) {
    CURL_TRC_CF(data, cf, "[%d] No stream_ctx set", stream_id);
    प्रत्यागम CURLE_FAILED_INIT;
  }

  विकल्प(frame->hd.type) {
  पक्ष NGHTTP2_DATA:
    CURL_TRC_CF(data, cf, "[%d] DATA, window=%d/%d",
                stream_id,
                Curl_ngh2_session_get_stream_effective_recv_data_length(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_stream_effective_local_window_size(
                  &ctx->ngh2, stream->id));
    /* If !body started on this stream, then receiving DATA is illegal. */
    जइ(!stream->bodystarted) {
      जइ(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                       stream_id, NGHTTP2_PROTOCOL_ERROR))) {
        प्रत्यागम CURLE_RECV_ERROR;
      }
    }
    भङ्ग;
  पक्ष NGHTTP2_HEADERS:
    जइ(stream->bodystarted) {
      /* Only valid HEADERS after body started is trailer HEADERS. We
         buffer them in on_header callback. */
      भङ्ग;
    }

    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    जइ(stream->status_code == -1)
      प्रत्यागम CURLE_RECV_ERROR;

    /* Only final status code signals the end of header */
    जइ(stream->status_code / 100 != 1) {
      stream->resp_hds_complete = TRUE;
      stream->bodystarted = TRUE;
    }
    अण्णहा
      stream->status_code = -1;

    h2_xfer_write_resp_hd(cf, data, stream, STRCONST("\r\n"),
                          (bool)stream->closed);

    Curl_multi_mark_dirty(data);
    भङ्ग;
  पक्ष NGHTTP2_प्रक्षिप_PROMISE:
    rv = प्रक्षिप_promise(cf, data, &frame->प्रक्षिप_promise);
    जइ(rv) { /* deny! */
      DEBUGASSERT((rv > CURL_प्रक्षिप_OK) && (rv <= CURL_प्रक्षिप_ERROROUT));
      जइ(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
             frame->प्रक्षिप_promise.promised_stream_id, NGHTTP2_CANCEL)))
        प्रत्यागम CURLE_SEND_ERROR;
      अण्णहा जइ(rv == CURL_प्रक्षिप_ERROROUT) {
        /* fail the parent transfer, not the whole session */
        CURL_TRC_CF(data, cf, "[%d] fail in PUSH_PROMISE received",
                    stream_id);
        जइ(!stream->xfer_result)
          stream->xfer_result = CURLE_RECV_ERROR;
        Curl_multi_mark_dirty(data);
      }
    }
    भङ्ग;
  पक्ष NGHTTP2_RST_STREAM:
    जइ(frame->rst_stream.error_code)
      stream->reset_by_server = TRUE;
    Curl_multi_mark_dirty(data);
    भङ्ग;
  पक्ष NGHTTP2_WINDOW_UPDATE:
    जइ(CURL_REQ_WANT_SEND(data) && Curl_bufq_is_empty(&stream->sendbuf)) {
      /* need more data, force processing of transfer */
      Curl_multi_mark_dirty(data);
    }
    अण्णहा जइ(!Curl_bufq_is_empty(&stream->sendbuf)) {
      /* resume the potentially suspended stream */
      जइ(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id)))
        प्रत्यागम CURLE_SEND_ERROR;
    }
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }

  जइ(frame->hd.flags & NGHTTP2_FLAG_END_STREAM) {
    जइ(!stream->closed && !stream->body_eos &&
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
  प्रत्यागम CURLE_OK;
}

#ifdef CURLVERBOSE
पूर्णाङ्क Curl_nghttp2_fr_print(ध्रुव nghttp2_frame *frame, वर्ण *buffer,
                          size_t blen)
{
  विकल्प(frame->hd.type) {
  पक्ष NGHTTP2_DATA: {
    प्रत्यागम curl_msnprintf(buffer, blen,
                          "FRAME[DATA, len=%d, eos=%d, padlen=%d]",
                          (पूर्णाङ्क)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_STREAM),
                          (पूर्णाङ्क)frame->data.padlen);
  }
  पक्ष NGHTTP2_HEADERS: {
    प्रत्यागम curl_msnprintf(buffer, blen,
                          "FRAME[HEADERS, len=%d, hend=%d, eos=%d]",
                          (पूर्णाङ्क)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_HEADERS),
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_STREAM));
  }
  पक्ष NGHTTP2_PRIORITY: {
    प्रत्यागम curl_msnprintf(buffer, blen,
                          "FRAME[PRIORITY, len=%d, flags=%d]",
                          (पूर्णाङ्क)frame->hd.length, frame->hd.flags);
  }
  पक्ष NGHTTP2_RST_STREAM: {
    प्रत्यागम curl_msnprintf(buffer, blen,
                          "FRAME[RST_STREAM, len=%d, flags=%d, error=%u]",
                          (पूर्णाङ्क)frame->hd.length, frame->hd.flags,
                          frame->rst_stream.error_code);
  }
  पक्ष NGHTTP2_SETTINGS: {
    जइ(frame->hd.flags & NGHTTP2_FLAG_ACK) {
      प्रत्यागम curl_msnprintf(buffer, blen, "FRAME[SETTINGS, ack=1]");
    }
    प्रत्यागम curl_msnprintf(buffer, blen,
                          "FRAME[SETTINGS, len=%d]", (पूर्णाङ्क)frame->hd.length);
  }
  पक्ष NGHTTP2_प्रक्षिप_PROMISE:
    प्रत्यागम curl_msnprintf(buffer, blen,
                          "FRAME[PUSH_PROMISE, len=%d, hend=%d]",
                          (पूर्णाङ्क)frame->hd.length,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_HEADERS));
  पक्ष NGHTTP2_PING:
    प्रत्यागम curl_msnprintf(buffer, blen,
                          "FRAME[PING, len=%d, ack=%d]",
                          (पूर्णाङ्क)frame->hd.length,
                          frame->hd.flags & NGHTTP2_FLAG_ACK);
  पक्ष NGHTTP2_GOAWAY: {
    वर्ण scratch[128];
    size_t s_len = CURL_ARRAYSIZE(scratch);
    size_t len = (frame->goaway.opaque_data_len < s_len) ?
      frame->goaway.opaque_data_len : s_len - 1;
    जइ(len)
      memcpy(scratch, frame->goaway.opaque_data, len);
    scratch[len] = '\0';
    प्रत्यागम curl_msnprintf(buffer, blen,
                          "FRAME[GOAWAY, error=%u, reason='%s', "
                          "last_stream=%d]", frame->goaway.error_code,
                          scratch, frame->goaway.last_stream_id);
  }
  पक्ष NGHTTP2_WINDOW_UPDATE: {
    प्रत्यागम curl_msnprintf(buffer, blen,
                          "FRAME[WINDOW_UPDATE, incr=%d]",
                          frame->window_update.window_size_increment);
  }
  स्वभाव:
    प्रत्यागम curl_msnprintf(buffer, blen, "FRAME[%d, len=%d, flags=%d]",
                          frame->hd.type, (पूर्णाङ्क)frame->hd.length,
                          frame->hd.flags);
  }
}

स्थिर पूर्णाङ्क on_frame_send(nghttp2_session *session, ध्रुव nghttp2_frame *frame,
                         शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);

  (शून्य)session;
  DEBUGASSERT(data);
  जइ(Curl_trc_cf_is_verbose(cf, data)) {
    वर्ण buffer[256];
    पूर्णाङ्क len;
    len = Curl_nghttp2_fr_print(frame, buffer, परिमाण(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] -> %s", frame->hd.stream_id, buffer);
  }
  जइ((frame->hd.type == NGHTTP2_GOAWAY) && !ctx->sent_goaway) {
    /* A GOAWAY not initiated by us, but by nghttp2 itself on detecting
     * a protocol error on the connection */
    failf(data, "nghttp2 shuts down connection with error %u: %s",
          frame->goaway.error_code,
          nghttp2_http2_strerror(frame->goaway.error_code));
  }
  प्रत्यागम 0;
}
#endif /* CURLVERBOSE */

स्थिर पूर्णाङ्क on_frame_recv(nghttp2_session *session, ध्रुव nghttp2_frame *frame,
                         शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf), *data_s;
  संरचना h2_stream_ctx *stream;
  int32_t stream_id = frame->hd.stream_id;

  (शून्य)session;
  DEBUGASSERT(data);
#ifdef CURLVERBOSE
  जइ(Curl_trc_cf_is_verbose(cf, data)) {
    वर्ण buffer[256];
    पूर्णाङ्क len;
    len = Curl_nghttp2_fr_print(frame, buffer, परिमाण(buffer) - 1);
    buffer[len] = 0;
    CURL_TRC_CF(data, cf, "[%d] <- %s", frame->hd.stream_id, buffer);
  }
#endif /* CURLVERBOSE */

  जइ(!stream_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(data);
    विकल्प(frame->hd.type) {
    पक्ष NGHTTP2_SETTINGS: {
      जइ(!(frame->hd.flags & NGHTTP2_FLAG_ACK)) {
        uint32_t max_conn = ctx->max_concurrent_streams;
        ctx->max_concurrent_streams =
          Curl_ngh2_session_get_remote_settings(
            &ctx->ngh2, NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS);
        ctx->enable_प्रक्षिप = Curl_ngh2_session_get_remote_settings(
          &ctx->ngh2, NGHTTP2_SETTINGS_ENABLE_प्रक्षिप) != 0;
        CURL_TRC_CF(data, cf, "[0] MAX_CONCURRENT_STREAMS: %u",
                    ctx->max_concurrent_streams);
        CURL_TRC_CF(data, cf, "[0] ENABLE_PUSH: %s",
                    ctx->enable_प्रक्षिप ? "TRUE" : "false");
        जइ(data && max_conn != ctx->max_concurrent_streams) {
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
        जइ(CURL_REQ_WANT_SEND(data))
          Curl_multi_mark_dirty(data);
      }
      भङ्ग;
    }
    पक्ष NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->goaway_error = frame->goaway.error_code;
      ctx->remote_max_sid = frame->goaway.last_stream_id;
      जइ(data) {
        infof(data, "received GOAWAY, error=%u, last_stream=%d",
              ctx->goaway_error, ctx->remote_max_sid);
        Curl_multi_connchanged(data->multi);
      }
      भङ्ग;
    स्वभाव:
      भङ्ग;
    }
    प्रत्यागम 0;
  }

  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  जइ(!data_s) {
    CURL_TRC_CF(data, cf, "[%d] No Curl_easy associated", stream_id);
    प्रत्यागम 0;
  }

  प्रत्यागम on_stream_frame(cf, data_s, stream, frame) ?
         NGHTTP2_ERR_CALLBACK_FAILURE : 0;
}

स्थिर पूर्णाङ्क cf_h2_on_invalid_frame_recv(nghttp2_session *session,
                                       ध्रुव nghttp2_frame *frame,
                                       पूर्णाङ्क ngerr, शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना Curl_easy *data;
  संरचना h2_stream_ctx *stream;
  int32_t stream_id = frame->hd.stream_id;

  (शून्य)session;
  data = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  जइ(data) {
#ifdef CURLVERBOSE
    वर्ण buffer[256];
    पूर्णाङ्क len;
    len = Curl_nghttp2_fr_print(frame, buffer, परिमाण(buffer) - 1);
    buffer[len] = 0;
    failf(data, "[HTTP2] [%d] received invalid frame: %s, error %d: %s",
          stream_id, buffer, ngerr, nghttp2_strerror(ngerr));
#endif /* CURLVERBOSE */
    जइ(stream) {
      Curl_ngh2_submit_rst_stream(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, NGHTTP2_STREAM_CLOSED);
      stream->error = ngerr;
      stream->closed = TRUE;
      stream->reset = TRUE;
    }
  }
  /* Return no error, nghttp2 will RST/GOAWAY by itself when needed */
  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क on_data_chunk_recv(nghttp2_session *session, uint8_t flags,
                              int32_t stream_id,
                              ध्रुव uint8_t *mem, size_t len, शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना h2_stream_ctx *stream;
  संरचना Curl_easy *data_s;

  (शून्य)session;
  (शून्य)flags;

  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */
  DEBUGASSERT(CF_DATA_CURRENT(cf));

  /* get the stream from the hash based on Stream ID */
  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  जइ(!data_s) {
    /* Receiving a Stream ID not in the hash should not happen - unless
       we have aborted a transfer artificially and there were more data
       in the pipeline. Silently ignore. */
    CURL_TRC_CF(CF_DATA_CURRENT(cf), cf, "[%d] Data for unknown", stream_id);
    /* consumed explicitly as no one will read it */
    Curl_ngh2_session_consume(&ctx->ngh2, stream_id, len);
    प्रत्यागम 0;
  }

  जइ(!stream)
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;

  h2_xfer_write_resp(cf, data_s, stream, (ध्रुव वर्ण *)mem, len, FALSE);

  Curl_ngh2_session_consume(&ctx->ngh2, stream_id, len);
  stream->nrcvd_data += (curl_off_t)len;
  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क on_stream_close(nghttp2_session *session, int32_t stream_id,
                           uint32_t error_code, शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना Curl_easy *data_s, *call_data = CF_DATA_CURRENT(cf);
  संरचना h2_stream_ctx *stream = रिक्त;
  पूर्णाङ्क rv;
  (शून्य)session;

  DEBUGASSERT(call_data);
  /* stream id 0 is the connection, do not look there for streams. */
  data_s = stream_id ? cf_h2_get_stream_xfer(cf, stream_id, &stream) : रिक्त;
  जइ(!data_s) {
    CURL_TRC_CF(call_data, cf,
                "[%d] on_stream_close, no easy set on stream", stream_id);
    प्रत्यागम 0;
  }
  जइ(!GOOD_EASY_HANDLE(data_s)) {
    /* nghttp2 still has an easy registered for the stream which has
     * been freed be libcurl. This points to a code path that does not
     * trigger DONE or DETACH events as it must. */
    CURL_TRC_CF(call_data, cf,
                "[%d] on_stream_close, not a GOOD easy on stream", stream_id);
    (शून्य)Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, stream_id, रिक्त);
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  जइ(!stream) {
    CURL_TRC_CF(data_s, cf,
                "[%d] on_stream_close, GOOD easy but no stream", stream_id);
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  stream->closed = TRUE;
  stream->error = error_code;
  जइ(stream->error) {
    stream->reset = TRUE;
    जइ((stream->error == NGHTTP2_REFUSED_STREAM) &&
       ctx->rcvd_goaway && ctx->goaway_error)
    /* REFUSED_STREAM is used by nghttp2 on a GOAWAY from the
     * server where it indicated it will no longer process this stream.
     * If the GOAWAY carried an error code, take that as the error for
     * the stream. */
    stream->error = ctx->goaway_error;
  }

  जइ(stream->error)
    CURL_TRC_CF(data_s, cf, "[%d] RESET: %s (err %u)",
              stream_id, nghttp2_http2_strerror(error_code), error_code);
  अण्णहा
    CURL_TRC_CF(data_s, cf, "[%d] CLOSED", stream_id);
  Curl_multi_mark_dirty(data_s);

  /* remove `stream_ctx` from the nghttp2 stream */
  rv = Curl_ngh2_session_set_stream_user_data(
    &ctx->ngh2, stream_id, रिक्त);
  जइ(rv) {
    infof(data_s, "http/2: failed to clear user_data for stream %d",
          stream_id);
    DEBUGASSERT(0);
  }
  प्रत्यागम 0;
}

स्थिर शून्य cf_h2_header_error(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data,
                               संरचना h2_stream_ctx *stream,
                               CURLcode result)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;

  failf(data, "Error receiving HTTP2 header: %d(%s)", (पूर्णाङ्क)result,
        curl_easy_strerror(result));
  जइ(stream) {
    Curl_ngh2_submit_rst_stream(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, NGHTTP2_STREAM_CLOSED);
    stream->closed = TRUE;
    stream->reset = TRUE;
  }
}

/* frame->hd.type is either NGHTTP2_HEADERS or NGHTTP2_PUSH_PROMISE */
स्थिर पूर्णाङ्क on_header(nghttp2_session *session, ध्रुव nghttp2_frame *frame,
                     ध्रुव uint8_t *name, size_t namelen,
                     ध्रुव uint8_t *मानम्, size_t valuelen,
                     uint8_t flags,
                     शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना h2_stream_ctx *stream;
  संरचना Curl_easy *data;
  int32_t stream_id = frame->hd.stream_id;
  CURLcode result;

  (शून्य)session;
  (शून्य)flags;

  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */

  /* get the stream from the hash based on Stream ID */
  data = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  जइ(!GOOD_EASY_HANDLE(data))
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;

  जइ(!stream) {
    failf(data, "Internal NULL stream");
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  /* Store received PUSH_PROMISE headers to be used when the subsequent
     PUSH_PROMISE callback comes */
  जइ(frame->hd.type == NGHTTP2_प्रक्षिप_PROMISE) {
    वर्ण *h;

    जइ((namelen == CURL_CSTRLEN(HTTP_PSEUDO_AUTHORITY)) &&
       !strncmp(HTTP_PSEUDO_AUTHORITY, (ध्रुव वर्ण *)name, namelen)) {
      /* pseudo headers are lower case */
      पूर्णाङ्क rc = 0;
      वर्ण *check = curl_maprintf("%s:%d", data->state.origin->hostname,
                                  data->state.origin->port);
      जइ(!check)
        /* no memory */
        प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
      जइ(!curl_strequal(check, (ध्रुव वर्ण *)मानम्) &&
         ((data->state.origin->port != data->state.origin->scheme->defport) ||
          !curl_strequal(data->state.origin->hostname, (ध्रुव वर्ण *)मानम्))) {
        /* This is push is not for the same authority that was asked for in
         * the URL. RFC 7540 section 8.2 says: "A client MUST treat a
         * PUSH_PROMISE for which the server is not authoritative as a stream
         * error of type PROTOCOL_ERROR."
         */
        (शून्य)Curl_ngh2_submit_rst_stream(
          &ctx->ngh2, NGHTTP2_FLAG_NONE, stream_id, NGHTTP2_PROTOCOL_ERROR);
        rc = NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      curlx_free(check);
      जइ(rc)
        प्रत्यागम rc;
    }

    जइ(!stream->प्रक्षिप_headers) {
      stream->प्रक्षिप_headers_alloc = 10;
      stream->प्रक्षिप_headers = curlx_malloc(stream->प्रक्षिप_headers_alloc *
                                          परिमाण(वर्ण *));
      जइ(!stream->प्रक्षिप_headers)
        प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
      stream->प्रक्षिप_headers_used = 0;
    }
    अण्णहा जइ(stream->प्रक्षिप_headers_used ==
            stream->प्रक्षिप_headers_alloc) {
      वर्ण **headp;
      जइ(stream->प्रक्षिप_headers_alloc > 1000) {
        /* this is beyond crazy many headers, bail out */
        failf(data, "Too many PUSH_PROMISE headers");
        free_प्रक्षिप_headers(stream);
        प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      stream->प्रक्षिप_headers_alloc *= 2;
      headp = curlx_realloc(stream->प्रक्षिप_headers,
                            stream->प्रक्षिप_headers_alloc * परिमाण(वर्ण *));
      जइ(!headp) {
        free_प्रक्षिप_headers(stream);
        प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      stream->प्रक्षिप_headers = headp;
    }
    h = curl_maprintf("%s:%s", name, मानम्);
    जइ(!h) {
      free_प्रक्षिप_headers(stream);
      प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    stream->प्रक्षिप_headers[stream->प्रक्षिप_headers_used++] = h;
    प्रत्यागम 0;
  }

  जइ(stream->bodystarted) {
    /* This is a trailer */
    CURL_TRC_CF(data, cf, "[%d] trailer: %.*s: %.*s",
                stream->id, (पूर्णाङ्क)namelen, name, (पूर्णाङ्क)valuelen, मानम्);
    result = Curl_dynhds_add(&stream->resp_trailers,
                             (ध्रुव वर्ण *)name, namelen,
                             (ध्रुव वर्ण *)मानम्, valuelen);
    जइ(result) {
      cf_h2_header_error(cf, data, stream, result);
      प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
    }

    प्रत्यागम 0;
  }

  जइ(namelen == CURL_CSTRLEN(HTTP_PSEUDO_STATUS) &&
     !memcmp(HTTP_PSEUDO_STATUS, name, namelen)) {
    /* nghttp2 guarantees :status is received first and only once. */
    वर्ण buffer[32];
    size_t hlen;
    result = Curl_http_decode_status(&stream->status_code,
                                     (ध्रुव वर्ण *)मानम्, valuelen);
    जइ(result) {
      cf_h2_header_error(cf, data, stream, result);
      प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    hlen = curl_msnprintf(buffer, परिमाण(buffer), HTTP_PSEUDO_STATUS ":%d\r",
                          stream->status_code);
    result = Curl_headers_प्रक्षिप(data, buffer, hlen, CURLH_PSEUDO);
    जइ(result) {
      cf_h2_header_error(cf, data, stream, result);
      प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    curlx_dyn_reset(&ctx->scratch);
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/2 "));
    जइ(!result)
      result = curlx_dyn_addn(&ctx->scratch, मानम्, valuelen);
    जइ(!result)
      result = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    जइ(!result)
      h2_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    जइ(result) {
      cf_h2_header_error(cf, data, stream, result);
      प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    /* if we receive data for another handle, wake that up */
    जइ(CF_DATA_CURRENT(cf) != data)
      Curl_multi_mark_dirty(data);

    CURL_TRC_CF(data, cf, "[%d] status: HTTP/2 %03d",
                stream->id, stream->status_code);
    प्रत्यागम 0;
  }

  /* nghttp2 guarantees that namelen > 0, and :status was already
     received, and this is not pseudo-header field . */
  /* convert to an HTTP1-style header */
  curlx_dyn_reset(&ctx->scratch);
  result = curlx_dyn_addn(&ctx->scratch, (ध्रुव वर्ण *)name, namelen);
  जइ(!result)
    result = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
  जइ(!result)
    result = curlx_dyn_addn(&ctx->scratch, (ध्रुव वर्ण *)मानम्, valuelen);
  जइ(!result)
    result = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
  जइ(!result)
    h2_xfer_write_resp_hd(cf, data, stream, curlx_dyn_ptr(&ctx->scratch),
                          curlx_dyn_len(&ctx->scratch), FALSE);
  जइ(result) {
    cf_h2_header_error(cf, data, stream, result);
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  /* if we receive data for another handle, wake that up */
  जइ(CF_DATA_CURRENT(cf) != data)
    Curl_multi_mark_dirty(data);

  CURL_TRC_CF(data, cf, "[%d] header: %.*s: %.*s",
              stream->id, (पूर्णाङ्क)namelen, name, (पूर्णाङ्क)valuelen, मानम्);

  प्रत्यागम 0; /* 0 is successful */
}

स्थिर ssize_t req_body_read_callback(nghttp2_session *session,
                                      int32_t stream_id,
                                      uint8_t *buf, size_t length,
                                      uint32_t *data_flags,
                                      nghttp2_data_source *source,
                                      शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना Curl_easy *data_s;
  संरचना h2_stream_ctx *stream = रिक्त;
  CURLcode result;
  ssize_t nread;
  size_t n;
  (शून्य)source;

  (शून्य)session;
  जइ(!stream_id)
    प्रत्यागम NGHTTP2_ERR_INVALID_ARGUMENT;

  /* get the stream from the hash based on Stream ID, stream ID zero is for
     connection-oriented stuff */
  data_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  जइ(!data_s || !stream)
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;

  result = Curl_bufq_read(&stream->sendbuf, buf, length, &n);
  जइ(result) {
    जइ(result != CURLE_AGAIN)
      प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
    nread = 0;
  }
  अण्णहा
    nread = (ssize_t)n;

  CURL_TRC_CF(data_s, cf, "[%d] req_body_read(len=%zu) eos=%d -> %zd, %d",
              stream_id, length, stream->body_eos, nread, (पूर्णाङ्क)result);

  जइ(stream->body_eos && Curl_bufq_is_empty(&stream->sendbuf)) {
    *data_flags = NGHTTP2_DATA_FLAG_EOF;
    प्रत्यागम nread;
  }
  प्रत्यागम (nread == 0) ? NGHTTP2_ERR_DEFERRED : nread;
}

#ifdef CURLVERBOSE
स्थिर पूर्णाङ्क error_callback(nghttp2_session *session,
                          ध्रुव वर्ण *सन्देशः,
                          size_t len,
                          शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
  (शून्य)session;
  failf(data, "%.*s", (पूर्णाङ्क)len, सन्देशः);
  प्रत्यागम 0;
}
#endif

/*
 * Append headers to ask for an HTTP1.1 to HTTP2 upgrade.
 */
CURLcode Curl_http2_request_upgrade(संरचना dynbuf *req,
                                    संरचना Curl_easy *data)
{
  CURLcode result;
  वर्ण *base64;
  size_t blen;
  संरचना SingleRequest *k = &data->req;
  uint8_t binsettings[H2_BINSETTINGS_LEN];
  ssize_t rc;
  size_t binlen; /* length of the binsettings data */

  rc = populate_binsettings(binsettings, data);
  जइ(!curlx_sztouz(rc, &binlen) || !binlen) {
    failf(data, "nghttp2 unexpectedly failed on pack_settings_payload");
    curlx_dyn_free(req);
    प्रत्यागम CURLE_FAILED_INIT;
  }

  result = curlx_base64url_encode(binsettings, binlen, &base64, &blen);
  जइ(result) {
    curlx_dyn_free(req);
    प्रत्यागम result;
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

  प्रत्यागम result;
}

स्थिर CURLcode http2_handle_stream_close(संरचना Curl_cfilter *cf,
                                          संरचना Curl_easy *data,
                                          संरचना h2_stream_ctx *stream,
                                          size_t *pnlen)
{
  CURLcode result;

  *pnlen = 0;
  जइ(stream->reset) {
    जइ(stream->error == NGHTTP2_REFUSED_STREAM) {
      infof(data, "HTTP/2 stream %d refused by server, try again on a new "
                  "connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      प्रत्यागम CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    अण्णहा जइ(stream->resp_hds_complete && data->req.no_body) {
      CURL_TRC_CF(data, cf, "[%d] error after response headers, but we did "
                  "not want a body anyway, ignore: %s (err %u)",
                  stream->id, nghttp2_http2_strerror(stream->error),
                  stream->error);
      stream->close_handled = TRUE;
      प्रत्यागम CURLE_OK;
    }
    failf(data, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          stream->id, stream->reset_by_server ? "server" : "curl",
          stream->error, nghttp2_http2_strerror(stream->error));
    प्रत्यागम stream->error ? CURLE_HTTP2_STREAM :
           (data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2);
  }
  अण्णहा जइ(!stream->bodystarted) {
    failf(data, "HTTP/2 stream %d was closed cleanly, but before getting "
          "all response header fields, treated as error", stream->id);
    प्रत्यागम CURLE_HTTP2_STREAM;
  }

  जइ(Curl_dynhds_गणनम्(&stream->resp_trailers)) {
    संरचना dynhds_entry *e;
    संरचना dynbuf dbuf;
    size_t i;

    result = CURLE_OK;
    curlx_dyn_init(&dbuf, DYN_TRAILERS);
    प्रति(i = 0; i < Curl_dynhds_गणनम्(&stream->resp_trailers); ++i) {
      e = Curl_dynhds_getn(&stream->resp_trailers, i);
      जइ(!e)
        भङ्ग;
      curlx_dyn_reset(&dbuf);
      result = curlx_dyn_addf(&dbuf, "%.*s: %.*s\x0d\x0a",
                              (पूर्णाङ्क)e->namelen, e->name,
                              (पूर्णाङ्क)e->valuelen, e->मानम्);
      जइ(result)
        भङ्ग;
      Curl_debug(data, CURLINFO_HEADER_IN, curlx_dyn_ptr(&dbuf),
                 curlx_dyn_len(&dbuf));
      result = Curl_client_write(data,
                                 CLIENTWRITE_HEADER | CLIENTWRITE_TRAILER,
                                 curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf));
      जइ(result)
        भङ्ग;
      result = Curl_bump_headersize(data, curlx_dyn_len(&dbuf), FALSE);
      जइ(result)
        प्रत्यागम result;
    }
    curlx_dyn_free(&dbuf);
    जइ(result)
      गच्छ out;
  }

  stream->close_handled = TRUE;
  result = CURLE_OK;

out:
  CURL_TRC_CF(data, cf, "handle_stream_close -> %d, %zu", (पूर्णाङ्क)result, *pnlen);
  प्रत्यागम result;
}

स्थिर पूर्णाङ्क sweight_wanted(ध्रुव संरचना Curl_easy *data)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  प्रत्यागम data->set.weight ?
    data->set.weight : NGHTTP2_DEFAULT_WEIGHT;
}

स्थिर पूर्णाङ्क sweight_in_effect(ध्रुव संरचना Curl_easy *data)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  प्रत्यागम data->state.weight ?
    data->state.weight : NGHTTP2_DEFAULT_WEIGHT;
}

/*
 * h2_pri_spec() fills in the pri_spec struct, used by nghttp2 to send weight
 * and dependency to the peer. It also stores the updated values in the state
 * struct.
 */

स्थिर शून्य h2_pri_spec(संरचना Curl_easy *data,
                        nghttp2_priority_spec *pri_spec)
{
  पूर्णाङ्क prio = data->set.weight;
  nghttp2_priority_spec_init(pri_spec, 0, sweight_wanted(data), FALSE);
  data->state.weight = prio;
}

/*
 * Check if there is been an update in the priority /
 * dependency settings and if so it submits a PRIORITY frame with the updated
 * info.
 * Flush any out data pending in the network buffer.
 */
स्थिर CURLcode h2_progress_egress(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  पूर्णाङ्क rv = 0;

  जइ(sweight_wanted(data) != sweight_in_effect(data)) {
    संरचना h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    जइ(stream && stream->id > 0 &&
       (sweight_wanted(data) != sweight_in_effect(data))) {
      /* send new weight and/or dependency */
      nghttp2_priority_spec pri_spec;

      h2_pri_spec(data, &pri_spec);
      CURL_TRC_CF(data, cf, "[%d] Queuing PRIORITY", stream->id);
      DEBUGASSERT(stream->id != -1);
      rv = Curl_ngh2_submit_priority(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, &pri_spec);
      जइ(rv)
        गच्छ out;
    }
  }

  ctx->nw_out_blocked = 0;
  जाव(!rv && !ctx->nw_out_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

out:
  जइ(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(data, cf, "nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    प्रत्यागम CURLE_SEND_ERROR;
  }
  /* Defer flushing during the connect phase so that the SETTINGS and
   * other initial frames are sent together with the first request.
   * Unless we are 'connect_only' where the request will never come. */
  जइ(!cf->connected && !cf->conn->bits.connect_only)
    प्रत्यागम CURLE_OK;
  प्रत्यागम nw_out_flush(cf, data);
}

स्थिर CURLcode stream_recv(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                            संरचना h2_stream_ctx *stream,
                            वर्ण *buf, size_t len, size_t *pnread)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_AGAIN;

  (शून्य)buf;
  (शून्य)len;
  *pnread = 0;

  जइ(!stream->xfer_result)
    stream->xfer_result = cf_h2_update_local_win(cf, data, stream);

  जइ(stream->xfer_result) {
    CURL_TRC_CF(data, cf, "[%d] xfer write failed", stream->id);
    result = stream->xfer_result;
  }
  अण्णहा जइ(stream->closed) {
    CURL_TRC_CF(data, cf, "[%d] returning CLOSE", stream->id);
    result = http2_handle_stream_close(cf, data, stream, pnread);
  }
  अण्णहा जइ(stream->reset ||
          (ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) ||
          (ctx->rcvd_goaway && ctx->remote_max_sid < stream->id)) {
    CURL_TRC_CF(data, cf, "[%d] returning ERR", stream->id);
    result = data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2;
  }

  जइ(result && (result != CURLE_AGAIN))
    CURL_TRC_CF(data, cf, "[%d] stream_recv(len=%zu) -> %d, %zu",
                stream->id, len, (पूर्णाङ्क)result, *pnread);
  प्रत्यागम result;
}

स्थिर CURLcode h2_progress_ingress(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    size_t data_max_bytes)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  size_t nread;

  जइ(should_close_session(ctx)) {
    CURL_TRC_CF(data, cf, "[0] ingress: session is closed");
    प्रत्यागम CURLE_HTTP2;
  }

  /* Process network input buffer first */
  जइ(!Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "Process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    result = h2_process_pending_निवेशः(cf, data);
    जइ(result)
      प्रत्यागम result;
  }

  जइ(!data_max_bytes)
    data_max_bytes = H2_CHUNK_SIZE;

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop due to connection close or us not processing
   * all network input */
  जाव(!ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) {
    संरचना h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    जइ(stream && (stream->closed || !data_max_bytes)) {
      /* We would like to abort here and stop processing, so that the transfer
       * loop can handle the data/close here. This may leave data in
       * underlying buffers that will not be consumed. */
      जइ(!cf->next || !cf->next->cft->has_data_pending(cf->next, data))
        Curl_multi_mark_dirty(data);
      भङ्ग;
    }

    result = Curl_cf_recv_bufq(cf->next, data, &ctx->inbufq, 0, &nread);
    जइ(result) {
      जइ(result != CURLE_AGAIN) {
        failf(data, "Failed receiving HTTP2 data: %d(%s)", (पूर्णाङ्क)result,
              curl_easy_strerror(result));
        प्रत्यागम result;
      }
      भङ्ग;
    }
    अण्णहा जइ(nread == 0) {
      CURL_TRC_CF(data, cf, "[0] ingress: connection closed");
      ctx->conn_closed = TRUE;
      भङ्ग;
    }
    अण्णहा {
      CURL_TRC_CF(data, cf, "[0] ingress: read %zu bytes", nread);
      data_max_bytes = (data_max_bytes > nread) ? (data_max_bytes - nread) : 0;
    }

    result = h2_process_pending_निवेशः(cf, data);
    जइ(result)
      प्रत्यागम result;
    CURL_TRC_CF(data, cf, "[0] ingress: nw-in buffered %zu",
                Curl_bufq_len(&ctx->inbufq));
  }

  जइ(ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(data, cf, "server closed %s",
                ctx->rcvd_goaway ? "with GOAWAY" : "abruptly");
    connclose(cf->conn);
  }

  CURL_TRC_CF(data, cf, "[0] ingress: done");
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cf_h2_recv(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                           वर्ण *buf, size_t len, size_t *pnread)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना h2_stream_ctx *stream;
  CURLcode result, r2;
  संरचना cf_call_data save;

  जइ(!data)
    प्रत्यागम CURLE_HTTP2;

  stream = H2_STREAM_CTX(ctx, data);

  *pnread = 0;
  जइ(!stream) {
    /* Abnormal call sequence: either this transfer has never opened a stream
     * (unlikely) or the transfer has been done, cleaned up its resources, but
     * a read() is called anyway. It is not clear what the calling sequence
     * is for such a case. */
    failf(data, "http/2 recv on a transfer never opened "
          "or already cleared, mid=%u", data->mid);
    प्रत्यागम CURLE_HTTP2;
  }

  CF_DATA_SAVE(save, cf, data);

  result = stream_recv(cf, data, stream, buf, len, pnread);
  जइ(result && (result != CURLE_AGAIN))
    गच्छ out;

  जइ(result) {
    result = h2_progress_ingress(cf, data, len);
    जइ(result)
      गच्छ out;

    result = stream_recv(cf, data, stream, buf, len, pnread);
  }

  जइ(*pnread > 0) {
    /* Now that we transferred this to the upper layer, we report
     * the actual amount of DATA consumed to the H2 session, so
     * that it adjusts stream flow control */
    Curl_ngh2_session_consume(&ctx->ngh2, stream->id, *pnread);
    जइ(stream->closed) {
      CURL_TRC_CF(data, cf, "[%d] DRAIN closed stream", stream->id);
      Curl_multi_mark_dirty(data);
    }
  }

out:
  r2 = h2_progress_egress(cf, data);
  जइ(r2 == CURLE_AGAIN) {
    /* pending data to send, need to be called again. Ideally, we
     * monitor the socket for POLLOUT, but when not SENDING
     * any more, we force processing of the transfer. */
    जइ(!CURL_REQ_WANT_SEND(data))
      Curl_multi_mark_dirty(data);
  }
  अण्णहा जइ(r2 && (result == CURLE_AGAIN)) {
    /* Do not replace a previous error result NOR a successful result
     * with an egress error. Example: the server might close the
     * connection after sending the complete response with EOS
     * and everything. Further sending may fail, but that MUST
     * not change that we had a successful response. */
    result = r2;
  }

  CURL_TRC_CF(data, cf, "[%d] cf_recv(len=%zu) -> %d, %zu, "
              "window=%d/%d, connection %d/%d",
              stream->id, len, (पूर्णाङ्क)result, *pnread,
              Curl_ngh2_session_get_stream_effective_recv_data_length(
                &ctx->ngh2, stream->id),
              Curl_ngh2_session_get_stream_effective_local_window_size(
                &ctx->ngh2, stream->id),
              Curl_ngh2_session_get_local_window_size(&ctx->ngh2),
              HTTP2_HUGE_WINDOW_SIZE);

  CF_DATA_RESTORE(cf, save);
  प्रत्यागम result;
}

स्थिर CURLcode cf_h2_body_send(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                संरचना h2_stream_ctx *stream,
                                ध्रुव शून्य *buf, size_t blen, bool eos,
                                size_t *pnwritten)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  CURLcode result;

  *pnwritten = 0;
  जइ(stream->closed) {
    जइ(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(data, cf, "[%d] discarding data"
                  "on closed stream with response", stream->id);
      जइ(eos)
        stream->body_eos = TRUE;
      *pnwritten = blen;
      प्रत्यागम CURLE_OK;
    }
    /* Server closed before we got a response, this is an error */
    infof(data, "stream %d closed", stream->id);
    प्रत्यागम CURLE_SEND_ERROR;
  }

  result = Curl_bufq_write(&stream->sendbuf, buf, blen, pnwritten);
  जइ(result)
    प्रत्यागम result;

  जइ(eos && (blen == *pnwritten))
    stream->body_eos = TRUE;

  जइ(eos || !Curl_bufq_is_empty(&stream->sendbuf)) {
    /* resume the potentially suspended stream */
    जइ(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id)))
      प्रत्यागम CURLE_SEND_ERROR;
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode h2_submit(संरचना h2_stream_ctx **pstream,
                          संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                          ध्रुव uint8_t *buf, size_t len,
                          bool eos, size_t *pnwritten)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना h2_stream_ctx *stream = रिक्त;
  संरचना dynhds h2_headers;
  nghttp2_nv *nva = रिक्त;
  ध्रुव शून्य *body = रिक्त;
  size_t nheader, bodylen;
  nghttp2_data_provider data_prd;
  int32_t stream_id;
  nghttp2_priority_spec pri_spec;
  size_t nwritten;
  CURLcode result = CURLE_OK;
  uint32_t initial_win_size;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_headers, MAX_HTTP_RESP_HEADER_गणनम्, DYN_HTTP_REQUEST);

  result = http2_data_setup(cf, data, &stream);
  जइ(result)
    गच्छ out;

  result = Curl_h1_req_parse_read(
    &stream->h1, buf, len, रिक्त,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : रिक्त,
    0, &nwritten);
  जइ(result)
    गच्छ out;
  *pnwritten = nwritten;
  जइ(!stream->h1.done) {
    /* need more data */
    गच्छ out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  जइ(result)
    गच्छ out;
  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nva = Curl_dynhds_to_nva(&h2_headers, &nheader);
  जइ(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }

  h2_pri_spec(data, &pri_spec);
  जइ(!Curl_ngh2_session_check_request_allowed(&ctx->ngh2))
    CURL_TRC_CF(data, cf, "send request NOT allowed (via nghttp2)");

  /* Check the initial windows size of the transfer (rate-limits?) and
   * send an updated settings on changes from previous value. */
  initial_win_size = cf_h2_initial_win_size(data);
  जइ(initial_win_size != ctx->initial_win_size) {
    result = cf_h2_update_settings(ctx, initial_win_size);
    जइ(result)
      गच्छ out;
  }

  विकल्प(data->state.httpreq) {
  पक्ष HTTPREQ_POST:
  पक्ष HTTPREQ_POST_FORM:
  पक्ष HTTPREQ_POST_MIME:
  पक्ष HTTPREQ_PUT:
    data_prd.read_callback = req_body_read_callback;
    data_prd.source.ptr = रिक्त;
    stream_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, &data_prd, stream);
    भङ्ग;
  स्वभाव:
    stream_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, रिक्त, stream);
  }

  जइ(stream_id < 0) {
    CURL_TRC_CF(data, cf, "send: nghttp2_submit_request error (%s)%d",
                nghttp2_strerror(stream_id), stream_id);
    result = CURLE_SEND_ERROR;
    गच्छ out;
  }

#ifdef CURLVERBOSE
#define MAX_ACC 60000  /* <64KB to account for some overhead */
  जइ(Curl_trc_is_verbose(data)) {
    size_t acc = 0, i;

    infof(data, "[HTTP/2] [%d] OPENED stream for %s",
          stream_id, Curl_bufref_ptr(&data->state.url));
    प्रति(i = 0; i < nheader; ++i) {
      acc += nva[i].namelen + nva[i].valuelen;

      infof(data, "[HTTP/2] [%d] [%.*s: %.*s]", stream_id,
            (पूर्णाङ्क)nva[i].namelen, nva[i].name,
            (पूर्णाङ्क)nva[i].valuelen, nva[i].मानम्);
    }

    जइ(acc > MAX_ACC) {
      infof(data, "[HTTP/2] Warning: The cumulative length of all "
            "headers exceeds %d bytes and that could cause the "
            "stream to be rejected.", MAX_ACC);
    }
  }
#endif

  stream->id = stream_id;

  body = (ध्रुव वर्ण *)buf + *pnwritten;
  bodylen = len - *pnwritten;

  जइ(bodylen || eos) {
    size_t n;
    result = cf_h2_body_send(cf, data, stream, body, bodylen, eos, &n);
    जइ(!result)
      *pnwritten += n;
    अण्णहा जइ(result == CURLE_AGAIN)
      result = CURLE_OK;
    अण्णहा {
      result = CURLE_SEND_ERROR;
    }
  }

out:
  CURL_TRC_CF(data, cf, "[%d] submit -> %d, %zu",
              stream ? stream->id : -1, (पूर्णाङ्क)result, *pnwritten);
  curlx_safefree(nva);
  *pstream = stream;
  Curl_dynhds_free(&h2_headers);
  प्रत्यागम result;
}

स्थिर CURLcode cf_h2_send(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                           ध्रुव uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
  संरचना cf_call_data save;
  CURLcode result = CURLE_OK, r2;

  CF_DATA_SAVE(save, cf, data);
  *pnwritten = 0;

  जइ(!stream || stream->id == -1) {
    result = h2_submit(&stream, cf, data, buf, len, eos, pnwritten);
    जइ(result)
      गच्छ out;
    DEBUGASSERT(stream);
  }
  अण्णहा जइ(stream->body_eos) {
    /* We already wrote this, but CURLE_AGAIN-ed the call due to not
     * being able to flush stream->sendbuf. Make a 0-length write
     * to trigger flushing again.
     * If this works, we report to have written `len` bytes. */
    size_t n;
    DEBUGASSERT(eos);
    result = cf_h2_body_send(cf, data, stream, buf, 0, eos, &n);
    CURL_TRC_CF(data, cf, "[%d] cf_body_send last CHUNK -> %d, %zu, eos=%d",
                stream->id, (पूर्णाङ्क)result, n, eos);
    जइ(result)
      गच्छ out;
    *pnwritten = len;
  }
  अण्णहा {
    result = cf_h2_body_send(cf, data, stream, buf, len, eos, pnwritten);
    CURL_TRC_CF(data, cf, "[%d] cf_body_send(len=%zu) -> %d, %zu, eos=%d",
                stream->id, len, (पूर्णाङ्क)result, *pnwritten, eos);
  }

  /* Call the nghttp2 send loop and flush to write ALL buffered data,
   * headers and/or request body completely out to the network */
  r2 = h2_progress_egress(cf, data);

  /* if the stream has been closed in egress handling (nghttp2 does that
   * when it does not like the headers, for example */
  जइ(stream && stream->closed && !stream->resp_hds_complete) {
    infof(data, "stream %d closed", stream->id);
    result = CURLE_SEND_ERROR;
    गच्छ out;
  }
  अण्णहा जइ(r2 && (r2 != CURLE_AGAIN)) {
    result = r2;
    गच्छ out;
  }

  जइ(should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    जइ(stream && stream->closed) {
      result = http2_handle_stream_close(cf, data, stream, pnwritten);
    }
    अण्णहा {
      CURL_TRC_CF(data, cf, "send: nothing to do in this session");
      result = CURLE_HTTP2;
    }
  }

out:
  जइ(stream) {
    CURL_TRC_CF(data, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
                "eos=%d, h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                stream->id, len, (पूर्णाङ्क)result, *pnwritten,
                stream->body_eos,
                Curl_ngh2_session_get_stream_remote_window_size(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&stream->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  अण्णहा {
    CURL_TRC_CF(data, cf, "cf_send(len=%zu) -> %d, %zu, "
                "connection-window=%d, nw_send_buffer(%zu)",
                len, (पूर्णाङ्क)result, *pnwritten,
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_DATA_RESTORE(cf, save);
  प्रत्यागम result;
}

स्थिर CURLcode cf_h2_flush(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *data)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
  संरचना cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  जइ(stream && !Curl_bufq_is_empty(&stream->sendbuf)) {
    /* resume the potentially suspended stream */
    जइ(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_data(&ctx->ngh2, stream->id))) {
      result = CURLE_SEND_ERROR;
      गच्छ out;
    }
  }

  result = h2_progress_egress(cf, data);

out:
  जइ(stream) {
    CURL_TRC_CF(data, cf, "[%d] flush -> %d, "
                "h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                stream->id, (पूर्णाङ्क)result,
                Curl_ngh2_session_get_stream_remote_window_size(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&stream->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  अण्णहा {
    CURL_TRC_CF(data, cf, "flush -> %d, "
                "connection-window=%d, nw_send_buffer(%zu)", (पूर्णाङ्क)result,
                Curl_ngh2_session_get_remote_window_size(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_DATA_RESTORE(cf, save);
  प्रत्यागम result;
}

स्थिर CURLcode cf_h2_adjust_pollset(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *data,
                                     संरचना easy_pollset *ps)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना cf_call_data save;
  curl_socket_t sock;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  जइ(!Curl_ngh2_valid(&ctx->ngh2))
    प्रत्यागम CURLE_OK;

  sock = Curl_conn_cf_get_socket(cf, data);
  Curl_pollset_check(data, ps, sock, &want_recv, &want_send);
  जइ(want_recv || want_send) {
    संरचना h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
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
  अण्णहा जइ(ctx->sent_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_DATA_SAVE(save, cf, data);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    result = Curl_pollset_set(data, ps, sock, want_recv, want_send);
    CF_DATA_RESTORE(cf, save);
  }
  प्रत्यागम result;
}

स्थिर CURLcode cf_h2_ctx_open(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना h2_stream_ctx *stream;
  CURLcode result = CURLE_OUT_OF_MEMORY;
  पूर्णाङ्क rc;
  nghttp2_session_callbacks *cbs = रिक्त;

  DEBUGASSERT(!ctx->ngh2.s);
  DEBUGASSERT(!ctx->ngh2.fatal_rv);
  DEBUGASSERT(ctx->initialized);

  rc = nghttp2_session_callbacks_new(&cbs);
  जइ(rc) {
    failf(data, "Could not initialize nghttp2 callbacks");
    गच्छ out;
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
  जइ(rc) {
    failf(data, "Could not initialize nghttp2");
    गच्छ out;
  }
  ctx->max_concurrent_streams = data->multi ?
    Curl_multi_max_concurrent_streams(data->multi) :
    DEFAULT_MAX_CONCURRENT_STREAMS;

  जइ(ctx->via_h1_upgrade) {
    /* HTTP/1.1 Upgrade issued. H2 Settings have already been submitted
     * in the H1 request and we upgrade from there. This stream
     * is opened implicitly as #1. */
    uint8_t binsettings[H2_BINSETTINGS_LEN];
    ssize_t rclen;
    size_t binlen; /* length of the binsettings data */

    rclen = populate_binsettings(binsettings, data);

    जइ(!curlx_sztouz(rclen, &binlen) || !binlen) {
      failf(data, "nghttp2 unexpectedly failed on pack_settings_payload");
      result = CURLE_FAILED_INIT;
      गच्छ out;
    }

    result = http2_data_setup(cf, data, &stream);
    जइ(result)
      गच्छ out;
    DEBUGASSERT(stream);
    stream->id = 1;
    /* queue SETTINGS frame (again) */
    rc = Curl_ngh2_session_upgrade2(
      &ctx->ngh2, binsettings, binlen, data->state.httpreq == HTTPREQ_HEAD,
      रिक्त);
    जइ(rc) {
      failf(data, "nghttp2_session_upgrade2() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      गच्छ out;
    }

    rc = Curl_ngh2_session_set_stream_user_data(
      &ctx->ngh2, stream->id, stream);
    जइ(rc) {
      infof(data, "http/2: failed to set user_data for stream %d",
            stream->id);
      DEBUGASSERT(0);
    }
    CURL_TRC_CF(data, cf, "created session via Upgrade");
  }
  अण्णहा {
    nghttp2_settings_entry iv[H2_SETTINGS_IV_LEN];
    size_t ivlen;

    ivlen = populate_settings(iv, data, ctx);
    rc = Curl_ngh2_submit_settings(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, iv, ivlen);
    जइ(rc) {
      failf(data, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      result = CURLE_HTTP2;
      गच्छ out;
    }
  }

  rc = Curl_ngh2_session_set_local_window_size(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, HTTP2_HUGE_WINDOW_SIZE);
  जइ(rc) {
    failf(data, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    result = CURLE_HTTP2;
    गच्छ out;
  }

  /* all set, traffic will be send on connect */
  result = CURLE_OK;
  CURL_TRC_CF(data, cf, "[0] created h2 session%s",
              ctx->via_h1_upgrade ? " (via h1 upgrade)" : "");

out:
  जइ(cbs)
    nghttp2_session_callbacks_del(cbs);
  प्रत्यागम result;
}

स्थिर CURLcode cf_h2_connect(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *data,
                              bool *done)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;
  संरचना cf_call_data save;
  bool first_time = FALSE;

  जइ(cf->connected) {
    *done = TRUE;
    प्रत्यागम CURLE_OK;
  }

  /* Connect the lower filters first */
  जइ(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    जइ(result || !*done)
      प्रत्यागम result;
  }

  *done = FALSE;

  CF_DATA_SAVE(save, cf, data);
  DEBUGASSERT(ctx->initialized);
  जइ(!ctx->ngh2.s) {
    result = cf_h2_ctx_open(cf, data);
    जइ(result)
      गच्छ out;
    first_time = TRUE;
  }

  जइ(!first_time) {
    result = h2_progress_ingress(cf, data, 0);
    जइ(result)
      गच्छ out;
  }

  /* Send out our SETTINGS and ACKs and such. If that blocks, we
   * have it buffered and can count this filter as being connected */
  result = h2_progress_egress(cf, data);
  जइ(result && (result != CURLE_AGAIN))
    गच्छ out;

  *done = TRUE;
  cf->connected = TRUE;
  result = CURLE_OK;

out:
  CURL_TRC_CF(data, cf, "cf_connect() -> %d, %d, ", (पूर्णाङ्क)result, *done);
  CF_DATA_RESTORE(cf, save);
  प्रत्यागम result;
}

स्थिर शून्य cf_h2_destroy(संरचना Curl_cfilter *cf, संरचना Curl_easy *data)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;

  (शून्य)data;
  जइ(ctx) {
    cf_h2_ctx_free(ctx);
    cf->ctx = रिक्त;
  }
}

स्थिर CURLcode cf_h2_shutdown(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data, bool *done)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना cf_call_data save;
  CURLcode result;
  पूर्णाङ्क rv;

  जइ(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *done = TRUE;
    प्रत्यागम CURLE_OK;
  }

  CF_DATA_SAVE(save, cf, data);

  जइ(!ctx->sent_goaway) {
    ctx->sent_goaway = TRUE;
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, ctx->local_max_sid, 0,
      (ध्रुव uint8_t *)"shutdown", परिमाण("shutdown"));
    जइ(rv) {
      failf(data, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      result = CURLE_SEND_ERROR;
      गच्छ out;
    }
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  result = CURLE_OK;
  जइ(Curl_ngh2_session_want_write(&ctx->ngh2) ||
     !Curl_bufq_is_empty(&ctx->outbufq))
    result = h2_progress_egress(cf, data);
  जइ(!result && Curl_ngh2_session_want_read(&ctx->ngh2))
    result = h2_progress_ingress(cf, data, 0);

  जइ(result == CURLE_AGAIN)
    result = CURLE_OK;

  *done = (ctx->conn_closed ||
           (!result && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2) &&
            Curl_bufq_is_empty(&ctx->outbufq)));

out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  प्रत्यागम result;
}

स्थिर CURLcode http2_data_pause(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 bool pause)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);

  DEBUGASSERT(data);
  जइ(ctx && Curl_ngh2_valid(&ctx->ngh2) && stream) {
    CURLcode result;

    stream->write_paused = pause;
    result = cf_h2_update_local_win(cf, data, stream);
    जइ(result)
      प्रत्यागम result;

    /* attempt to send the window update */
    (शून्य)h2_progress_egress(cf, data);

    जइ(!pause) {
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
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cf_h2_cntrl(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *data,
                            पूर्णाङ्क event, पूर्णाङ्क arg1, शून्य *arg2)
{
  CURLcode result = CURLE_OK;
  संरचना cf_call_data save;

  (शून्य)arg2;

  CF_DATA_SAVE(save, cf, data);
  विकल्प(event) {
  पक्ष CF_CTRL_DATA_SETUP:
    भङ्ग;
  पक्ष CF_CTRL_DATA_PAUSE:
    result = http2_data_pause(cf, data, (arg1 != 0));
    भङ्ग;
  पक्ष CF_CTRL_FLUSH:
    result = cf_h2_flush(cf, data);
    भङ्ग;
  पक्ष CF_CTRL_DATA_DONE:
    http2_data_done(cf, data);
    भङ्ग;
  पक्ष CF_CTRL_CONN_INFO_UPDATE:
    जइ(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 20;
      Curl_conn_set_multiplex(cf->conn);
    }
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
  CF_DATA_RESTORE(cf, save);
  प्रत्यागम result;
}

स्थिर bool cf_h2_data_pending(संरचना Curl_cfilter *cf,
                               ध्रुव संरचना Curl_easy *data)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;

  जइ(ctx && !Curl_bufq_is_empty(&ctx->inbufq))
    प्रत्यागम TRUE;
  प्रत्यागम cf->next ? cf->next->cft->has_data_pending(cf->next, data) : FALSE;
}

स्थिर bool cf_h2_is_alive(संरचना Curl_cfilter *cf,
                           संरचना Curl_easy *data,
                           bool *निवेशः_pending)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  bool alive;
  संरचना cf_call_data save;

  *निवेशः_pending = FALSE;
  CF_DATA_SAVE(save, cf, data);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
           http2_connisalive(cf, data, निवेशः_pending));
  CURL_TRC_CF(data, cf, "conn alive -> %d, input_pending=%d",
              alive, *निवेशः_pending);
  CF_DATA_RESTORE(cf, save);
  प्रत्यागम alive;
}

स्थिर CURLcode cf_h2_keep_alive(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data)
{
  CURLcode result;
  संरचना cf_call_data save;

  CF_DATA_SAVE(save, cf, data);
  result = http2_send_ping(cf, data);
  CF_DATA_RESTORE(cf, save);
  प्रत्यागम result;
}

स्थिर CURLcode cf_h2_query(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *data,
                            पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना cf_call_data save;
  size_t effective_max;

  विकल्प(query) {
  पक्ष CF_QUERY_MAX_CONCURRENT:
    DEBUGASSERT(pres1);

    CF_DATA_SAVE(save, cf, data);
    जइ(!Curl_ngh2_valid(&ctx->ngh2) ||
       !Curl_ngh2_session_check_request_allowed(&ctx->ngh2)) {
      /* the limit is what we have in use right now */
      effective_max = cf->conn->attached_xfers;
    }
    अण्णहा {
      effective_max = ctx->max_concurrent_streams;
    }
    *pres1 = (effective_max > INT_MAX) ? INT_MAX : (पूर्णाङ्क)effective_max;
    CF_DATA_RESTORE(cf, save);
    प्रत्यागम CURLE_OK;
  पक्ष CF_QUERY_STREAM_ERROR: {
    संरचना h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    *pres1 = stream ? (पूर्णाङ्क)stream->error : 0;
    प्रत्यागम CURLE_OK;
  }
  पक्ष CF_QUERY_NEED_FLUSH: {
    संरचना h2_stream_ctx *stream = H2_STREAM_CTX(ctx, data);
    जइ(!Curl_bufq_is_empty(&ctx->outbufq) ||
       (stream && !Curl_bufq_is_empty(&stream->sendbuf))) {
      *pres1 = TRUE;
      प्रत्यागम CURLE_OK;
    }
    भङ्ग;
  }
  पक्ष CF_QUERY_HTTP_VERSION:
    *pres1 = 20;
    प्रत्यागम CURLE_OK;
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

संरचना Curl_cftype Curl_cft_nghttp2 = {
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

स्थिर CURLcode http2_cfilter_add(संरचना Curl_cfilter **pcf,
                                  संरचना Curl_easy *data,
                                  संरचना connectdata *conn,
                                  int8_t sockindex,
                                  bool via_h1_upgrade)
{
  संरचना Curl_cfilter *cf = रिक्त;
  संरचना cf_h2_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(data->conn);
  ctx = curlx_calloc(1, परिमाण(*ctx));
  जइ(!ctx)
    गच्छ out;
  cf_h2_ctx_init(ctx, via_h1_upgrade);

  result = Curl_cf_create(&cf, &Curl_cft_nghttp2, ctx);
  जइ(result)
    गच्छ out;

  ctx = रिक्त;
  Curl_conn_cf_add(data, conn, sockindex, cf);

out:
  जइ(result)
    cf_h2_ctx_free(ctx);
  *pcf = result ? रिक्त : cf;
  प्रत्यागम result;
}

स्थिर CURLcode http2_cfilter_निवेशय_after(संरचना Curl_cfilter *cf,
                                           संरचना Curl_easy *data,
                                           bool via_h1_upgrade)
{
  संरचना Curl_cfilter *cf_h2 = रिक्त;
  संरचना cf_h2_ctx *ctx;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  (शून्य)data;
  ctx = curlx_calloc(1, परिमाण(*ctx));
  जइ(!ctx)
    गच्छ out;
  cf_h2_ctx_init(ctx, via_h1_upgrade);

  result = Curl_cf_create(&cf_h2, &Curl_cft_nghttp2, ctx);
  जइ(result)
    गच्छ out;

  ctx = रिक्त;
  Curl_conn_cf_निवेशय_after(cf, cf_h2);

out:
  जइ(result)
    cf_h2_ctx_free(ctx);
  प्रत्यागम result;
}

bool Curl_http2_may_switch(संरचना Curl_easy *data)
{
  जइ(Curl_conn_http_version(data, data->conn) < 20 &&
     (data->state.http_neg.wanted & CURL_HTTP_V2x) &&
     data->state.http_neg.h2_prior_knowledge) {
#ifndef CURL_DISABLE_PROXY
    जइ(data->conn->bits.origin_is_proxy) {
      infof(data, "Ignoring HTTP/2 prior knowledge due to proxy");
      प्रत्यागम FALSE;
    }
#endif
    प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}

CURLcode Curl_http2_switch(संरचना Curl_easy *data)
{
  संरचना Curl_cfilter *cf;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, data->conn) < 20);

  result = http2_cfilter_add(&cf, data, data->conn, FIRSTSOCKET, FALSE);
  जइ(result)
    प्रत्यागम result;
  CURL_TRC_CF(data, cf, "switching connection to HTTP/2");

  जइ(cf->next) {
    bool done;
    प्रत्यागम Curl_conn_cf_connect(cf, data, &done);
  }
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_http2_switch_at(संरचना Curl_cfilter *cf, संरचना Curl_easy *data)
{
  संरचना Curl_cfilter *cf_h2;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, data->conn) < 20);

  result = http2_cfilter_निवेशय_after(cf, data, FALSE);
  जइ(result)
    प्रत्यागम result;

  cf_h2 = cf->next;

  जइ(cf_h2->next) {
    bool done;
    प्रत्यागम Curl_conn_cf_connect(cf_h2, data, &done);
  }
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_http2_upgrade(संरचना Curl_easy *data,
                            संरचना connectdata *conn, int8_t sockindex,
                            ध्रुव वर्ण *mem, size_t nread)
{
  संरचना Curl_cfilter *cf;
  संरचना cf_h2_ctx *ctx;
  CURLcode result;

  DEBUGASSERT(Curl_conn_http_version(data, conn) < 20);

  result = http2_cfilter_add(&cf, data, conn, sockindex, TRUE);
  जइ(result)
    प्रत्यागम result;
  CURL_TRC_CF(data, cf, "upgrading connection to HTTP/2");

  DEBUGASSERT(cf->cft == &Curl_cft_nghttp2);
  ctx = cf->ctx;

  data->req.httpversion_sent = 20; /* it is an h2 request now */
  data->req.header = TRUE; /* we expect the real response to come in h2 */
  data->req.headerline = 0; /* restart the header line counter */

  जइ(nread > 0) {
    /* Remaining data from the protocol switch reply is already using
     * the switched protocol, ie. HTTP/2. We add that to the network
     * inbufq. */
    size_t copied;

    result = Curl_bufq_write(&ctx->inbufq,
                             (ध्रुव unsigned वर्ण *)mem, nread, &copied);
    जइ(result) {
      failf(data, "error on copying HTTP Upgrade response: %d", (पूर्णाङ्क)result);
      प्रत्यागम CURLE_RECV_ERROR;
    }
    जइ(copied < nread) {
      failf(data, "connection buffer size could not take all data "
            "from HTTP Upgrade response header: copied=%zu, datalen=%zu",
            copied, nread);
      प्रत्यागम CURLE_HTTP2;
    }
    infof(data, "Copied HTTP/2 data in stream buffer to connection buffer"
          " after upgrade: len=%zu", nread);
  }

  जइ(cf->next) {
    bool done;
    result = Curl_conn_cf_connect(cf, data, &done);
    जइ(!result)
      cf->cft->cntrl(cf, data, CF_CTRL_CONN_INFO_UPDATE, 0, रिक्त);
  }
  प्रत्यागम result;
}

/* Only call this function for a transfer that already got an HTTP/2
   CURLE_HTTP2_STREAM error! */
bool Curl_h2_http_1_1_error(संरचना Curl_easy *data)
{
  जइ(Curl_conn_http_version(data, data->conn) == 20) {
    पूर्णाङ्क err = Curl_conn_get_stream_error(data, data->conn, FIRSTSOCKET);
    प्रत्यागम err == NGHTTP2_HTTP_1_1_REQUIRED;
  }
  प्रत्यागम FALSE;
}

शून्य *Curl_nghttp2_malloc(size_t size, शून्य *user_data)
{
  (शून्य)user_data;
  प्रत्यागम Curl_cmalloc(size);
}

शून्य Curl_nghttp2_free(शून्य *ptr, शून्य *user_data)
{
  (शून्य)user_data;
  Curl_cfree(ptr);
}

शून्य *Curl_nghttp2_calloc(size_t nmemb, size_t size, शून्य *user_data)
{
  (शून्य)user_data;
  प्रत्यागम Curl_ccalloc(nmemb, size);
}

शून्य *Curl_nghttp2_realloc(शून्य *ptr, size_t size, शून्य *user_data)
{
  (शून्य)user_data;
  प्रत्यागम Curl_crealloc(ptr, size);
}

#अण्णहा /* CURL_DISABLE_HTTP || !USE_NGHTTP2 */

/* Satisfy external references even if http2 is not compiled in. */

वर्ण *curl_pushheader_bynum(संरचना curl_pushheaders *h, size_t अङ्कः)
{
  (शून्य)h;
  (शून्य)अङ्कः;
  प्रत्यागम रिक्त;
}

वर्ण *curl_pushheader_byname(संरचना curl_pushheaders *h, ध्रुव वर्ण *name)
{
  (शून्य)h;
  (शून्य)name;
  प्रत्यागम रिक्त;
}

#endif /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
