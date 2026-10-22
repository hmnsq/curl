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
#समावेश "curl_setup.h"

#अगर !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#समावेश <nghttp2/nghttp2.h>

#समावेश "urldata.h"
#समावेश "bufq.h"
#समावेश "u32_ptrset.h"
#समावेश "http1.h"
#समावेश "http2.h"
#समावेश "http.h"
#समावेश "sendf.h"
#समावेश "curl_trc.h"
#समावेश "select.h"
#समावेश "curlx/base64.h"
#समावेश "multiif.h"
#समावेश "ngh2_shim.h"
#समावेश "progress.h"
#समावेश "url.h"
#समावेश "urlapi-int.h"
#समावेश "cfilters.h"
#समावेश "connect.h"
#समावेश "transfer.h"
#समावेश "bufref.h"
#समावेश "curlx/dynbuf.h"
#समावेश "headers.h"
#समावेश "curl_share.h"

#अगर NGHTTP2_VERSION_अंक < 0x010f00
#त्रुटि "nghttp2 1.15.0 or greater required"
#पूर्ण_अगर

#घोषणा NGHTTP2_HAS_SET_LOCAL_WINDOW_आकार 1

/* buffer dimensioning:
 * use 16K as chunk size, as that fits H2 DATA frames well */
#घोषणा H2_CHUNK_आकार           (16 * 1024)
/* connection window size */
#घोषणा H2_CONN_WINDOW_आकार     (10 * 1024 * 1024)
/* on receiving from TLS, we prep for holding a full stream window */
#घोषणा H2_NW_RECV_CHUNKS       (H2_CONN_WINDOW_आकार / H2_CHUNK_आकार)
/* on send into TLS, we want to accumulate small frames */
#घोषणा H2_NW_SEND_CHUNKS       1
/* this is how much we want "in flight" for a stream, unthrottled */
#घोषणा H2_STREAM_WINDOW_आकार_MAX   (10 * 1024 * 1024)
/* this is how much we want "in flight" for a stream, initially, IFF
 * nghttp2 allows us to tweak the local window size. */
#अगर NGHTTP2_HAS_SET_LOCAL_WINDOW_आकार
#घोषणा H2_STREAM_WINDOW_आकार_INITIAL  (64 * 1024)
#अन्यथा
#घोषणा H2_STREAM_WINDOW_आकार_INITIAL H2_STREAM_WINDOW_आकार_MAX
#पूर्ण_अगर
/* keep smaller stream upload buffer (default h2 window size) to have
 * our progress bars and "upload done" reporting closer to reality */
#घोषणा H2_STREAM_SEND_CHUNKS   ((64 * 1024) / H2_CHUNK_आकार)
/* spare chunks we keep for a full window */
#घोषणा H2_STREAM_POOL_SPARES   (H2_CONN_WINDOW_आकार / H2_CHUNK_आकार)

/* We need to accommodate the max number of streams with their window sizes on
 * the overall connection. Streams might become PAUSED which will block their
 * received QUOTA in the connection window. If we run out of space, the server
 * is blocked from sending us any data. See #10988 for an issue with this. */
#घोषणा HTTP2_HUGE_WINDOW_आकार (100 * H2_STREAM_WINDOW_आकार_MAX)

#घोषणा H2_SETTINGS_IV_LEN 3
#घोषणा H2_BINSETTINGS_LEN 80

काष्ठा cf_h2_ctx {
  काष्ठा curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  काष्ठा cf_call_तथ्य call_तथ्य;

  काष्ठा bufq inbufq;           /* network input */
  काष्ठा bufq outbufq;          /* network output */
  काष्ठा bufc_pool stream_bufcp; /* spares for stream buffers */
  काष्ठा dynbuf scratch;        /* scratch buffer for temp use */

  काष्ठा u32_ptrset streams; /* hash of `data->mid` to `h2_stream_ctx` */
  माप_प्रकार drain_योग; /* sum of all stream's UrlState drain */
  uint32_t initial_win_आकार; /* current initial window size (settings) */
  uint32_t max_concurrent_streams;
  uint32_t goaway_error;        /* goaway error code from server */
  int32_t remote_max_sid;       /* max id processed by server */
  int32_t local_max_sid;        /* max id processed by us */
  BIT(initialized);
  BIT(via_h1_upgrade);
  BIT(conn_closed);
  BIT(rcvd_goaway);
  BIT(sent_goaway);
  BIT(enable_धकेलो);
  BIT(nw_out_blocked);
};

/* How to access `call_data` from a cf_h2 filter */
#अघोषित CF_CTX_CALL_तथ्य
#घोषणा CF_CTX_CALL_तथ्य(cf) ((काष्ठा cf_h2_ctx *)(cf)->ctx)->call_तथ्य

/**
 * All about the H2 internals of a stream
 */
काष्ठा h2_stream_ctx {
  काष्ठा bufq sendbuf; /* request buffer */
  काष्ठा h1_req_parser h1; /* parsing the request */
  काष्ठा dynhds resp_trailers; /* response trailer fields */
  माप_प्रकार resp_hds_len; /* amount of response header bytes in recvbuf */
  curl_off_t xfer_id;  /* transfer `id` owning this stream */
  curl_off_t nrcvd_तथ्य;  /* number of DATA bytes received */

  अक्षर **धकेलो_headers;       /* allocated array */
  माप_प्रकार धकेलो_headers_used;  /* number of entries filled in */
  माप_प्रकार धकेलो_headers_alloc; /* number of entries allocated */

  int32_t id; /* HTTP/2 protocol identifier for stream */
  uint32_t mid; /* transfer `mid` owning this stream */
  uint32_t error; /* stream error code */
  CURLcode xfer_परिणाम; /* Result of writing out response */
  int32_t local_window_आकार; /* the local recv window size */
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

अटल व्योम free_धकेलो_headers(काष्ठा h2_stream_ctx *stream)
{
  माप_प्रकार i;
  क्रम(i = 0; i < stream->धकेलो_headers_used; i++)
    curlx_free(stream->धकेलो_headers[i]);
  curlx_safefree(stream->धकेलो_headers);
  stream->धकेलो_headers_used = 0;
}

अटल व्योम h2_stream_ctx_free(काष्ठा h2_stream_ctx *stream)
{
  Curl_bufq_free(&stream->sendbuf);
  Curl_h1_req_parse_free(&stream->h1);
  Curl_dynhds_free(&stream->resp_trailers);
  free_धकेलो_headers(stream);
  curlx_free(stream);
}

अटल व्योम h2_stream_hash_free(uint32_t id, व्योम *stream)
{
  (व्योम)id;
  DEBUGASSERT(stream);
  h2_stream_ctx_free((काष्ठा h2_stream_ctx *)stream);
}

अटल व्योम cf_h2_ctx_init(काष्ठा cf_h2_ctx *ctx, bool via_h1_upgrade)
{
  Curl_bufcp_init(&ctx->stream_bufcp, H2_CHUNK_आकार, H2_STREAM_POOL_SPARES);
  Curl_bufq_initp(&ctx->inbufq, &ctx->stream_bufcp, H2_NW_RECV_CHUNKS, 0);
  Curl_bufq_initp(&ctx->outbufq, &ctx->stream_bufcp, H2_NW_SEND_CHUNKS, 0);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_HEADER);
  Curl_u32_ptrset_init(&ctx->streams, h2_stream_hash_free);
  ctx->remote_max_sid = INT32_MAX;
  ctx->via_h1_upgrade = via_h1_upgrade;
  ctx->initialized = TRUE;
}

अटल व्योम cf_h2_ctx_free(काष्ठा cf_h2_ctx *ctx)
{
  अगर(ctx && ctx->initialized) {
    Curl_ngh2_del(&ctx->ngh2);
    Curl_bufq_free(&ctx->inbufq);
    Curl_bufq_free(&ctx->outbufq);
    Curl_bufcp_free(&ctx->stream_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    स_रखो(ctx, 0, माप(*ctx));
  }
  curlx_free(ctx);
}

अटल uint32_t cf_h2_initial_win_आकार(काष्ठा Curl_easy *तथ्य)
{
#अगर NGHTTP2_HAS_SET_LOCAL_WINDOW_आकार
  /* If the transfer has a rate-limit lower than the default initial
   * stream window size, use that. It needs to be at least 8k or servers
   * may be unhappy. */
  curl_off_t rps = Curl_rlimit_per_step(&तथ्य->progress.dl.rlimit);
  अगर((rps > 0) && (rps < H2_STREAM_WINDOW_आकार_INITIAL))
    वापस CURLMAX((uint32_t)rps, 8192);
#पूर्ण_अगर
  वापस H2_STREAM_WINDOW_आकार_INITIAL;
}

अटल माप_प्रकार populate_settings(nghttp2_settings_entry *iv,
                                काष्ठा Curl_easy *तथ्य,
                                काष्ठा cf_h2_ctx *ctx)
{
  iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
  iv[0].मूल्य = Curl_multi_max_concurrent_streams(तथ्य->multi);

  iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_आकार;
  iv[1].मूल्य = cf_h2_initial_win_आकार(तथ्य);
  अगर(ctx)
    ctx->initial_win_आकार = iv[1].मूल्य;
  iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_धकेलो;
  iv[2].मूल्य = !!तथ्य->multi->धकेलो_cb;

  वापस 3;
}

अटल ssize_t populate_binsettings(uint8_t *binsettings,
                                    काष्ठा Curl_easy *तथ्य)
{
  nghttp2_settings_entry iv[H2_SETTINGS_IV_LEN];
  माप_प्रकार ivlen;

  ivlen = populate_settings(iv, तथ्य, शून्य);
  /* this returns number of bytes it wrote or a negative number on error. */
  वापस nghttp2_pack_settings_payload(binsettings, H2_BINSETTINGS_LEN,
                                       iv, ivlen);
}

अटल CURLcode cf_h2_update_settings(काष्ठा cf_h2_ctx *ctx,
                                      uint32_t initial_win_आकार)
{
  nghttp2_settings_entry entry;
  entry.settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_आकार;
  entry.मूल्य = initial_win_आकार;
  अगर(Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, &entry, 1))
    वापस CURLE_SEND_ERROR;
  ctx->initial_win_आकार = initial_win_आकार;
  वापस CURLE_OK;
}

#घोषणा H2_STREAM_CTX(ctx, तथ्य)                                         \
  ((काष्ठा h2_stream_ctx *)(                                             \
    (तथ्य) ? Curl_u32_ptrset_get(&(ctx)->streams, (तथ्य)->mid) : शून्य))

अटल काष्ठा h2_stream_ctx *h2_stream_ctx_create(काष्ठा cf_h2_ctx *ctx,
                                                  काष्ठा Curl_easy *तथ्य)
{
  काष्ठा h2_stream_ctx *stream;

  stream = curlx_calloc(1, माप(*stream));
  अगर(!stream)
    वापस शून्य;

  stream->id = -1;
  stream->xfer_id = तथ्य->id;
  stream->mid = तथ्य->mid;
  Curl_bufq_initp(&stream->sendbuf, &ctx->stream_bufcp,
                  H2_STREAM_SEND_CHUNKS, BUFQ_OPT_NONE);
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_पंक्ति_LEN);
  Curl_dynhds_init(&stream->resp_trailers,
                   MAX_HTTP_RESP_HEADER_गिनती, DYN_HTTP_REQUEST);
  stream->bodystarted = FALSE;
  stream->status_code = -1;
  stream->closed = FALSE;
  stream->close_handled = FALSE;
  stream->error = NGHTTP2_NO_ERROR;
  stream->local_window_आकार = H2_STREAM_WINDOW_आकार_INITIAL;
  stream->nrcvd_तथ्य = 0;
  वापस stream;
}

अटल काष्ठा Curl_easy *cf_h2_get_stream_xfer(काष्ठा Curl_cfilter *cf,
                                               int32_t stream_id,
                                               काष्ठा h2_stream_ctx **pstream)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  काष्ठा h2_stream_ctx *stream =
    Curl_ngh2_session_get_stream_user_तथ्य(&ctx->ngh2, stream_id);
  अगर(pstream)
    *pstream = stream;
  अगर(stream && (stream->mid != UINT32_MAX)) {
    काष्ठा Curl_easy *तथ्य, *call_तथ्य = CF_तथ्य_CURRENT(cf);
    अगर(!call_तथ्य || !call_तथ्य->multi) {
      DEBUGASSERT(0);
      वापस शून्य;
    }
    तथ्य = Curl_multi_get_easy(call_तथ्य->multi, stream->mid);
    अगर(तथ्य && (तथ्य->id == stream->xfer_id))
      वापस तथ्य;
  }
  वापस शून्य;
}

#अगर_घोषित NGHTTP2_HAS_SET_LOCAL_WINDOW_आकार
अटल int32_t cf_h2_get_desired_local_win(काष्ठा Curl_cfilter *cf,
                                           काष्ठा Curl_easy *तथ्य)
{
  curl_off_t avail = Curl_rlimit_avail(&तथ्य->progress.dl.rlimit, शून्य);

  (व्योम)cf;
  अगर(avail < CURL_OFF_T_MAX) { /* limit in place */
    अगर(avail <= 0)
      वापस 0;
    अन्यथा अगर(avail < H2_STREAM_WINDOW_आकार_MAX)
      वापस (int32_t)avail;
  }
  वापस H2_STREAM_WINDOW_आकार_MAX;
}

अटल CURLcode cf_h2_update_local_win(काष्ठा Curl_cfilter *cf,
                                       काष्ठा Curl_easy *तथ्य,
                                       काष्ठा h2_stream_ctx *stream)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  int32_t dwsize;
  पूर्णांक rv;

  dwsize = (stream->write_paused || stream->xfer_परिणाम) ?
           0 : cf_h2_get_desired_local_win(cf, तथ्य);
  अगर(dwsize != stream->local_window_आकार) {
    int32_t wsize = Curl_ngh2_session_get_stream_effective_local_window_आकार(
      &ctx->ngh2, stream->id);
    अगर(dwsize > wsize) {
      rv = Curl_ngh2_session_set_local_window_आकार(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize);
      अगर(rv) {
        failf(तथ्य, "[%d] nghttp2 set_local_window_size(%d) failed: "
              "%s(%d)", stream->id, dwsize, nghttp2_strerror(rv), rv);
        वापस CURLE_HTTP2;
      }
      rv = Curl_ngh2_submit_window_update(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize - wsize);
      अगर(rv) {
        failf(तथ्य, "[%d] nghttp2_submit_window_update() failed: "
              "%s(%d)", stream->id, nghttp2_strerror(rv), rv);
        वापस CURLE_HTTP2;
      }
      stream->local_window_आकार = dwsize;
      CURL_TRC_CF(तथ्य, cf, "[%d] local window update by %d",
                  stream->id, dwsize - wsize);
    }
    अन्यथा {
      rv = Curl_ngh2_session_set_local_window_आकार(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, dwsize);
      अगर(rv) {
        failf(तथ्य, "[%d] nghttp2_session_set_local_window_size() failed: "
              "%s(%d)", stream->id, nghttp2_strerror(rv), rv);
        वापस CURLE_HTTP2;
      }
      stream->local_window_आकार = dwsize;
      CURL_TRC_CF(तथ्य, cf, "[%d] local window size now %d",
                  stream->id, dwsize);
    }
  }
  वापस CURLE_OK;
}

#अन्यथा /* NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

अटल CURLcode cf_h2_update_local_win(काष्ठा Curl_cfilter *cf,
                                       काष्ठा Curl_easy *तथ्य,
                                       काष्ठा h2_stream_ctx *stream)
{
  (व्योम)cf;
  (व्योम)तथ्य;
  (व्योम)stream;
  वापस CURLE_OK;
}
#पूर्ण_अगर /* !NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

अटल CURLcode http2_तथ्य_setup(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 काष्ठा h2_stream_ctx **pstream)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  काष्ठा h2_stream_ctx *stream;

  (व्योम)cf;
  DEBUGASSERT(तथ्य);

  अगर(!तथ्य)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  stream = H2_STREAM_CTX(ctx, तथ्य);
  अगर(stream) {
    *pstream = stream;
    वापस CURLE_OK;
  }

  stream = h2_stream_ctx_create(ctx, तथ्य);
  अगर(!stream)
    वापस CURLE_OUT_OF_MEMORY;

  अगर(Curl_u32_ptrset_set(&ctx->streams, तथ्य->mid, stream)) {
    h2_stream_ctx_free(stream);
    वापस CURLE_OUT_OF_MEMORY;
  }

  *pstream = stream;
  वापस CURLE_OK;
}

अटल CURLcode nw_out_flush(काष्ठा Curl_cfilter *cf,
                             काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  माप_प्रकार nwritten;
  CURLcode परिणाम;

  अगर(Curl_bufq_is_empty(&ctx->outbufq))
    वापस CURLE_OK;

  परिणाम = Curl_cf_send_bufq(cf->अग्र, तथ्य, &ctx->outbufq, शून्य, 0,
                             &nwritten);
  अगर(परिणाम) {
    अगर(परिणाम == CURLE_AGAIN) {
      CURL_TRC_CF(तथ्य, cf, "flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_out_blocked = 1;
    }
    वापस परिणाम;
  }
  वापस Curl_bufq_is_empty(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

अटल व्योम http2_तथ्य_done(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  काष्ठा h2_stream_ctx *stream = H2_STREAM_CTX(ctx, तथ्य);

  DEBUGASSERT(ctx);
  अगर(!stream || !ctx->initialized)
    वापस;

  अगर(Curl_ngh2_valid(&ctx->ngh2)) {
    bool flush_egress = FALSE;
    /* returns error if stream not known, which is fine here */
    (व्योम)Curl_ngh2_session_set_stream_user_तथ्य(&ctx->ngh2, stream->id, शून्य);

    अगर(!stream->closed && stream->id > 0) {
      /* RST_STREAM */
      CURL_TRC_CF(तथ्य, cf, "[%d] premature DATA_DONE, RST stream",
                  stream->id);
      stream->closed = TRUE;
      stream->reset = TRUE;
      Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                  stream->id, NGHTTP2_STREAM_CLOSED);
      flush_egress = TRUE;
    }

    अगर(flush_egress) {
      (व्योम)Curl_ngh2_session_send(&ctx->ngh2);
      (व्योम)nw_out_flush(cf, तथ्य);
    }
  }

  Curl_u32_ptrset_unset(&ctx->streams, तथ्य->mid);
}

अटल पूर्णांक h2_client_new(काष्ठा Curl_cfilter *cf,
                         nghttp2_session_callbacks *cbs)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { शून्य, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  पूर्णांक rc = nghttp2_option_new(&o);
  अगर(rc)
    वापस rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_set_no_auto_window_update(o, 1);
#अगर NGHTTP2_VERSION_अंक >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_set_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#पूर्ण_अगर
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  वापस rc;
}

/*
 * Returns nonzero if current HTTP/2 session should be closed.
 */
अटल पूर्णांक should_close_session(काष्ठा cf_h2_ctx *ctx)
{
  वापस ctx->drain_योग == 0 && !Curl_ngh2_session_want_read(&ctx->ngh2) &&
    !Curl_ngh2_session_want_write(&ctx->ngh2);
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
अटल CURLcode h2_process_pending_निवेश(काष्ठा Curl_cfilter *cf,
                                         काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  स्थिर अचिन्हित अक्षर *buf;
  माप_प्रकार blen, nread;
  ssize_t rv;

  जबतक(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {
    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (स्थिर uint8_t *)buf, blen);
    अगर(!curlx_sztouz(rv, &nread)) {
      failf(तथ्य, "nghttp2 recv error %zd: %s", rv, nghttp2_strerror((पूर्णांक)rv));
      वापस CURLE_RECV_ERROR;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    अगर(Curl_bufq_is_empty(&ctx->inbufq)) {
      अवरोध;
    }
    अन्यथा {
      CURL_TRC_CF(तथ्य, cf, "process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }

  अगर(Curl_ngh2_session_check_request_allowed(&ctx->ngh2) == 0) {
    /* No more requests are allowed in the current session, so
       the connection may not be reused. This is set when a
       GOAWAY frame has been received or when the limit of stream
       identifiers has been reached. */
    CURL_TRC_M(तथ्य, "http/2: No new requests allowed");
    connclose(cf->conn);
  }

  वापस CURLE_OK;
}

/*
 * The server may send us data at any point (e.g. PING frames). Therefore, we
 * cannot assume that an HTTP/2 socket is dead because it is readable.
 *
 * Check the lower filters first and, if successful, peek at the socket
 * and distinguish between closed and data.
 */
अटल bool http2_connisalive(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                              bool *निवेश_pending)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *निवेश_pending = FALSE;
  अगर(!cf->अग्र || !cf->अग्र->cft->is_alive(cf->अग्र, तथ्य, निवेश_pending))
    वापस FALSE;

  अगर(*निवेश_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode परिणाम;
    माप_प्रकार nread;

    *निवेश_pending = FALSE;
    परिणाम = Curl_cf_recv_bufq(cf->अग्र, तथ्य, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(तथ्य, cf, "connisalive, recv pending input -> %d, %zu",
                (पूर्णांक)परिणाम, nread);
    अगर(!परिणाम) {
      परिणाम = h2_process_pending_निवेश(cf, तथ्य);
      अगर(परिणाम)
        /* immediate error, considered dead */
        alive = FALSE;
      अन्यथा {
        alive = !should_close_session(ctx);
      }
    }
    अन्यथा अगर(परिणाम != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  वापस alive;
}

अटल CURLcode http2_send_ping(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  पूर्णांक rc;

  rc = Curl_ngh2_submit_ping(&ctx->ngh2, 0, ZERO_NULL);
  अगर(rc) {
    failf(तथ्य, "nghttp2_submit_ping() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    वापस CURLE_HTTP2;
  }

  rc = Curl_ngh2_session_send(&ctx->ngh2);
  अगर(rc) {
    failf(तथ्य, "nghttp2_session_send() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    वापस CURLE_SEND_ERROR;
  }
  वापस CURLE_OK;
}

/*
 * Store nghttp2 version info in this buffer.
 */
व्योम Curl_http2_ver(अक्षर *p, माप_प्रकार len)
{
  nghttp2_info *h2 = nghttp2_version(0);
  (व्योम)curl_msnprintf(p, len, "nghttp2/%s", h2->version_str);
}

/*
 * The implementation of nghttp2_send_callback type. Here we write |data| with
 * size |length| to the network and return the number of bytes actually
 * written. See the documentation of nghttp2_send_callback for the details.
 */
अटल ssize_t send_callback(nghttp2_session *h2,
                             स्थिर uint8_t *buf, माप_प्रकार blen, पूर्णांक flags,
                             व्योम *userp)
{
  काष्ठा Curl_cfilter *cf = userp;
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);
  माप_प्रकार nwritten;
  CURLcode परिणाम = CURLE_OK;

  (व्योम)h2;
  (व्योम)flags;
  DEBUGASSERT(तथ्य);

  अगर(!cf->connected)
    परिणाम = Curl_bufq_write(&ctx->outbufq, buf, blen, &nwritten);
  अन्यथा
    परिणाम = Curl_cf_send_bufq(cf->अग्र, तथ्य, &ctx->outbufq, buf, blen,
                               &nwritten);

  अगर(परिणाम) {
    अगर(परिणाम == CURLE_AGAIN) {
      ctx->nw_out_blocked = 1;
      वापस NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(तथ्य, "Failed sending HTTP2 data");
    वापस NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  अगर(!nwritten) {
    ctx->nw_out_blocked = 1;
    वापस NGHTTP2_ERR_WOULDBLOCK;
  }
  वापस (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

/* We pass a pointer to this struct in the push callback, but the contents of
   the struct are hidden from the user. */
काष्ठा curl_pushheaders {
  काष्ठा Curl_easy *तथ्य;
  काष्ठा h2_stream_ctx *stream;
  स्थिर nghttp2_धकेलो_promise *frame;
};

/*
 * push header access function. Only to be used from within the push callback
 */
अक्षर *curl_pushheader_bynum(काष्ठा curl_pushheaders *h, माप_प्रकार अंक)
{
  /* Verify that we got a good easy handle in the push header struct, mostly to
     detect rubbish input fast(er). */
  अगर(!h || !GOOD_EASY_HANDLE(h->तथ्य))
    वापस शून्य;
  अन्यथा {
    अगर(h->stream && अंक < h->stream->धकेलो_headers_used)
      वापस h->stream->धकेलो_headers[अंक];
  }
  वापस शून्य;
}

/*
 * push header access function. Only to be used from within the push callback
 */
अक्षर *curl_pushheader_byname(काष्ठा curl_pushheaders *h, स्थिर अक्षर *name)
{
  काष्ठा h2_stream_ctx *stream;
  माप_प्रकार len;
  माप_प्रकार i;
  /* Verify that we got a good easy handle in the push header struct,
     mostly to detect rubbish input fast(er). Also empty header name
     is rubbish too. We have to allow ":" at the beginning of
     the header, but header == ":" must be rejected. If we have ':' in
     the middle of header, it could be matched in middle of the value,
     this is because we do prefix match.*/
  अगर(!h || !GOOD_EASY_HANDLE(h->तथ्य) || !name || !name[0] ||
     !म_भेद(name, ":") || म_अक्षर(name + 1, ':'))
    वापस शून्य;

  stream = h->stream;
  अगर(!stream)
    वापस शून्य;

  len = म_माप(name);
  क्रम(i = 0; i < stream->धकेलो_headers_used; i++) {
    अगर(!म_भेदन(name, stream->धकेलो_headers[i], len)) {
      /* sub-match, make sure that it is followed by a colon */
      अगर(stream->धकेलो_headers[i][len] != ':')
        जारी;
      वापस &stream->धकेलो_headers[i][len + 1];
    }
  }
  वापस शून्य;
}

अटल काष्ठा Curl_easy *h2_duphandle(काष्ठा Curl_cfilter *cf,
                                      काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_easy *second = curl_easy_init();
  अगर(second) {
    काष्ठा h2_stream_ctx *second_stream;
    http2_तथ्य_setup(cf, second, &second_stream);
    second->state.weight = तथ्य->state.weight;
    अगर(तथ्य->share)
      (व्योम)Curl_share_easy_link(second, तथ्य->share);
    /* copy a range of options from the parent handle */
    second->set.fwrite_func = तथ्य->set.fwrite_func;
    second->set.out = तथ्य->set.out;
    second->set.fwrite_header = तथ्य->set.fwrite_header;
    second->set.writeheader = तथ्य->set.writeheader;
    second->set.fdebug = तथ्य->set.fdebug;
    second->set.debugdata = तथ्य->set.debugdata;
    second->set.fxferinfo = तथ्य->set.fxferinfo;
    second->set.fprogress = तथ्य->set.fprogress;
    second->set.progress_client = तथ्य->set.progress_client;
    second->progress.hide = तथ्य->progress.hide;
    second->progress.callback = तथ्य->progress.callback;
  }
  वापस second;
}

अटल पूर्णांक set_transfer_url(काष्ठा Curl_easy *newhandle,
                            काष्ठा curl_pushheaders *hp,
                            काष्ठा Curl_easy *तथ्य)
{
  स्थिर अक्षर *v;
  CURLUcode uc;
  अक्षर *url = शून्य;
  पूर्णांक rc = 0;
  CURLU *u = curl_url();

  अगर(!u)
    वापस 5;

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_SCHEME);
  अगर(v) {
    uc = curl_url_set(u, CURLUPART_SCHEME, v, 0);
    अगर(uc) {
      rc = 1;
      जाओ fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_AUTHORITY);
  अगर(v) {
    uc = Curl_url_set_authority(u, v);
    अगर(uc) {
      rc = 2;
      जाओ fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_PATH);
  अगर(v) {
    uc = curl_url_set(u, CURLUPART_PATH, v, 0);
    अगर(uc) {
      rc = 3;
      जाओ fail;
    }
  }

  /* We can only allow PUSH of resource from the same origin, e.g.
   * scheme + hostname + port */
  अगर(!Curl_url_same_origin(तथ्य->state.uh, u)) {
    rc = 1;
    जाओ fail;
  }

  uc = curl_url_get(u, CURLUPART_URL, &url, 0);
  अगर(uc)
    rc = 4;
fail:
  अगर(rc) {
    curl_url_cleanup(u);
    वापस rc;
  }

  DEBUGASSERT(!newhandle->state.uh);
  newhandle->state.uh = u;
  Curl_bufref_set(&newhandle->state.url, url, 0, curl_free);
  वापस 0;
}

अटल व्योम discard_newhandle(काष्ठा Curl_cfilter *cf,
                              काष्ठा Curl_easy *newhandle)
{
  http2_तथ्य_done(cf, newhandle);
  (व्योम)Curl_close(&newhandle);
}

अटल पूर्णांक धकेलो_promise(काष्ठा Curl_cfilter *cf,
                        काष्ठा Curl_easy *तथ्य,
                        स्थिर nghttp2_धकेलो_promise *frame)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  पूर्णांक rv; /* one of the CURL_PUSH_* defines */

  CURL_TRC_CF(तथ्य, cf, "[%d] PUSH_PROMISE received",
              frame->promised_stream_id);
  अगर(तथ्य->multi->धकेलो_cb) {
    काष्ठा h2_stream_ctx *stream;
    काष्ठा h2_stream_ctx *newstream;
    काष्ठा curl_pushheaders heads;
    CURLMcode mresult;
    CURLcode परिणाम;
    /* clone the parent */
    काष्ठा Curl_easy *newhandle = h2_duphandle(cf, तथ्य);
    अगर(!newhandle) {
      infof(तथ्य, "failed to duplicate handle");
      rv = CURL_धकेलो_DENY; /* FAIL HARD */
      जाओ fail;
    }

    stream = H2_STREAM_CTX(ctx, तथ्य);
    अगर(!stream) {
      failf(तथ्य, "Internal NULL stream");
      discard_newhandle(cf, newhandle);
      rv = CURL_धकेलो_DENY;
      जाओ fail;
    }

    heads.तथ्य = तथ्य;
    heads.stream = stream;
    heads.frame = frame;

    rv = set_transfer_url(newhandle, &heads, तथ्य);
    अगर(rv) {
      CURL_TRC_CF(तथ्य, cf, "[%d] PUSH_PROMISE, failed to set URL -> %d",
                  frame->promised_stream_id, rv);
      discard_newhandle(cf, newhandle);
      rv = CURL_धकेलो_DENY;
      जाओ fail;
    }

    {
      काष्ठा Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, तथ्य, multi_धकेलो_cb);
      rv = तथ्य->multi->धकेलो_cb(तथ्य, newhandle,
                                stream->धकेलो_headers_used, &heads,
                                तथ्य->multi->धकेलो_userp);
      CURL_CBAPI_END(&guard);
    }

    /* free the headers again */
    free_धकेलो_headers(stream);

    अगर(rv) {
      DEBUGASSERT((rv > CURL_धकेलो_OK) && (rv <= CURL_धकेलो_ERROROUT));
      /* denied, kill off the new handle again */
      CURL_TRC_CF(तथ्य, cf, "[%d] PUSH_PROMISE, denied by application -> %d",
                  frame->promised_stream_id, rv);
      discard_newhandle(cf, newhandle);
      जाओ fail;
    }

    /* approved, add to the multi handle for processing. This
     * assigns newhandle->mid. For the new `mid` we assign the
     * h2_stream instance and remember the stream_id already known. */
    mresult = Curl_multi_add_perform(तथ्य->multi, newhandle, cf->conn);
    अगर(mresult) {
      infof(तथ्य, "failed to add handle to multi");
      discard_newhandle(cf, newhandle);
      rv = CURL_धकेलो_DENY;
      जाओ fail;
    }

    परिणाम = http2_तथ्य_setup(cf, newhandle, &newstream);
    अगर(परिणाम) {
      failf(तथ्य, "error setting up stream: %d", (पूर्णांक)परिणाम);
      discard_newhandle(cf, newhandle);
      rv = CURL_धकेलो_DENY;
      जाओ fail;
    }

    DEBUGASSERT(newstream);
    newstream->id = frame->promised_stream_id;
    newhandle->req.maxdownload = -1;
    newhandle->req.आकार = -1;

    CURL_TRC_CF(तथ्य, cf, "promise easy handle added to multi, mid=%u",
                newhandle->mid);
    rv = Curl_ngh2_session_set_stream_user_तथ्य(
      &ctx->ngh2, newstream->id, newstream);
    अगर(rv) {
      infof(तथ्य, "failed to set user_data for stream %d",
            newstream->id);
      DEBUGASSERT(0);
      discard_newhandle(cf, newhandle);
      rv = CURL_धकेलो_DENY;
      जाओ fail;
    }

    /* success, remember max stream id processed */
    अगर(newstream->id > ctx->local_max_sid)
      ctx->local_max_sid = newstream->id;
  }
  अन्यथा {
    CURL_TRC_CF(तथ्य, cf, "Got PUSH_PROMISE, ignore it");
    rv = CURL_धकेलो_DENY;
  }
fail:
  वापस rv;
}

अटल व्योम h2_xfer_write_resp_hd(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य,
                                  काष्ठा h2_stream_ctx *stream,
                                  स्थिर अक्षर *buf, माप_प्रकार blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  अगर(!stream->xfer_परिणाम) {
    stream->xfer_परिणाम = Curl_xfer_write_resp_hd(तथ्य, buf, blen, eos);
    अगर(!stream->xfer_परिणाम && !eos)
      stream->xfer_परिणाम = cf_h2_update_local_win(cf, तथ्य, stream);
    अगर(stream->xfer_परिणाम)
      CURL_TRC_CF(तथ्य, cf, "[%d] error %d writing %zu bytes of headers",
                  stream->id, (पूर्णांक)stream->xfer_परिणाम, blen);
  }
}

अटल व्योम h2_xfer_write_resp(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य,
                               काष्ठा h2_stream_ctx *stream,
                               स्थिर अक्षर *buf, माप_प्रकार blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  अगर(!stream->xfer_परिणाम)
    stream->xfer_परिणाम = Curl_xfer_write_resp(तथ्य, buf, blen, eos);
  /* If the transfer write is errored, we do not want any more data */
  अगर(stream->xfer_परिणाम) {
    काष्ठा cf_h2_ctx *ctx = cf->ctx;
    CURL_TRC_CF(तथ्य, cf, "[%d] error %d writing %zu bytes of data, "
                "RST-ing stream",
                stream->id, (पूर्णांक)stream->xfer_परिणाम, blen);
    Curl_ngh2_submit_rst_stream(
      &ctx->ngh2, 0, stream->id, (uint32_t)NGHTTP2_ERR_CALLBACK_FAILURE);
  }
  अन्यथा अगर(!stream->write_paused && Curl_xfer_write_is_paused(तथ्य)) {
    CURL_TRC_CF(तथ्य, cf, "[%d] stream output paused", stream->id);
    stream->write_paused = TRUE;
  }
  अन्यथा अगर(stream->write_paused && !Curl_xfer_write_is_paused(तथ्य)) {
    CURL_TRC_CF(तथ्य, cf, "[%d] stream output unpaused", stream->id);
    stream->write_paused = FALSE;
  }

  अगर(!stream->xfer_परिणाम && !eos)
    stream->xfer_परिणाम = cf_h2_update_local_win(cf, तथ्य, stream);
}

अटल CURLcode on_stream_frame(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य,
                                काष्ठा h2_stream_ctx *stream,
                                स्थिर nghttp2_frame *frame)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  int32_t stream_id = frame->hd.stream_id;
  पूर्णांक rv;

  अगर(!stream) {
    CURL_TRC_CF(तथ्य, cf, "[%d] No stream_ctx set", stream_id);
    वापस CURLE_FAILED_INIT;
  }

  चयन(frame->hd.type) {
  हाल NGHTTP2_तथ्य:
    CURL_TRC_CF(तथ्य, cf, "[%d] DATA, window=%d/%d",
                stream_id,
                Curl_ngh2_session_get_stream_effective_recv_तथ्य_लंबाई(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_stream_effective_local_window_आकार(
                  &ctx->ngh2, stream->id));
    /* If !body started on this stream, then receiving DATA is illegal. */
    अगर(!stream->bodystarted) {
      अगर(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                       stream_id, NGHTTP2_PROTOCOL_ERROR))) {
        वापस CURLE_RECV_ERROR;
      }
    }
    अवरोध;
  हाल NGHTTP2_HEADERS:
    अगर(stream->bodystarted) {
      /* Only valid HEADERS after body started is trailer HEADERS. We
         buffer them in on_header callback. */
      अवरोध;
    }

    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    अगर(stream->status_code == -1)
      वापस CURLE_RECV_ERROR;

    /* Only final status code signals the end of header */
    अगर(stream->status_code / 100 != 1) {
      stream->resp_hds_complete = TRUE;
      stream->bodystarted = TRUE;
    }
    अन्यथा
      stream->status_code = -1;

    h2_xfer_write_resp_hd(cf, तथ्य, stream, STRCONST("\r\n"),
                          (bool)stream->closed);

    Curl_multi_mark_dirty(तथ्य);
    अवरोध;
  हाल NGHTTP2_धकेलो_PROMISE:
    rv = धकेलो_promise(cf, तथ्य, &frame->धकेलो_promise);
    अगर(rv) { /* deny! */
      DEBUGASSERT((rv > CURL_धकेलो_OK) && (rv <= CURL_धकेलो_ERROROUT));
      अगर(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_stream(&ctx->ngh2, NGHTTP2_FLAG_NONE,
             frame->धकेलो_promise.promised_stream_id, NGHTTP2_CANCEL)))
        वापस CURLE_SEND_ERROR;
      अन्यथा अगर(rv == CURL_धकेलो_ERROROUT) {
        /* fail the parent transfer, not the whole session */
        CURL_TRC_CF(तथ्य, cf, "[%d] fail in PUSH_PROMISE received",
                    stream_id);
        अगर(!stream->xfer_परिणाम)
          stream->xfer_परिणाम = CURLE_RECV_ERROR;
        Curl_multi_mark_dirty(तथ्य);
      }
    }
    अवरोध;
  हाल NGHTTP2_RST_STREAM:
    अगर(frame->rst_stream.error_code)
      stream->reset_by_server = TRUE;
    Curl_multi_mark_dirty(तथ्य);
    अवरोध;
  हाल NGHTTP2_WINDOW_UPDATE:
    अगर(CURL_REQ_WANT_SEND(तथ्य) && Curl_bufq_is_empty(&stream->sendbuf)) {
      /* need more data, force processing of transfer */
      Curl_multi_mark_dirty(तथ्य);
    }
    अन्यथा अगर(!Curl_bufq_is_empty(&stream->sendbuf)) {
      /* resume the potentially suspended stream */
      अगर(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_session_resume_तथ्य(&ctx->ngh2, stream->id)))
        वापस CURLE_SEND_ERROR;
    }
    अवरोध;
  शेष:
    अवरोध;
  }

  अगर(frame->hd.flags & NGHTTP2_FLAG_END_STREAM) {
    अगर(!stream->closed && !stream->body_eos &&
       ((stream->status_code >= 400) || (stream->status_code < 200))) {
      /* The server did not give us a positive response and we are not
       * done uploading the request body. We need to stop doing that and
       * also inform the server that we aborted our side. */
      CURL_TRC_CF(तथ्य, cf, "[%d] EOS frame with unfinished upload and "
                  "HTTP status %d, abort upload by RST",
                  stream_id, stream->status_code);
      Curl_ngh2_submit_rst_stream(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, NGHTTP2_STREAM_CLOSED);
      stream->closed = TRUE;
    }
    Curl_multi_mark_dirty(तथ्य);
  }
  वापस CURLE_OK;
}

#अगर_घोषित CURLVERBOSE
पूर्णांक Curl_nghttp2_fr_print(स्थिर nghttp2_frame *frame, अक्षर *बफर,
                          माप_प्रकार blen)
{
  चयन(frame->hd.type) {
  हाल NGHTTP2_तथ्य: {
    वापस curl_msnprintf(बफर, blen,
                          "FRAME[DATA, len=%d, eos=%d, padlen=%d]",
                          (पूर्णांक)frame->hd.लंबाई,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_STREAM),
                          (पूर्णांक)frame->तथ्य.padlen);
  }
  हाल NGHTTP2_HEADERS: {
    वापस curl_msnprintf(बफर, blen,
                          "FRAME[HEADERS, len=%d, hend=%d, eos=%d]",
                          (पूर्णांक)frame->hd.लंबाई,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_HEADERS),
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_STREAM));
  }
  हाल NGHTTP2_PRIORITY: {
    वापस curl_msnprintf(बफर, blen,
                          "FRAME[PRIORITY, len=%d, flags=%d]",
                          (पूर्णांक)frame->hd.लंबाई, frame->hd.flags);
  }
  हाल NGHTTP2_RST_STREAM: {
    वापस curl_msnprintf(बफर, blen,
                          "FRAME[RST_STREAM, len=%d, flags=%d, error=%u]",
                          (पूर्णांक)frame->hd.लंबाई, frame->hd.flags,
                          frame->rst_stream.error_code);
  }
  हाल NGHTTP2_SETTINGS: {
    अगर(frame->hd.flags & NGHTTP2_FLAG_ACK) {
      वापस curl_msnprintf(बफर, blen, "FRAME[SETTINGS, ack=1]");
    }
    वापस curl_msnprintf(बफर, blen,
                          "FRAME[SETTINGS, len=%d]", (पूर्णांक)frame->hd.लंबाई);
  }
  हाल NGHTTP2_धकेलो_PROMISE:
    वापस curl_msnprintf(बफर, blen,
                          "FRAME[PUSH_PROMISE, len=%d, hend=%d]",
                          (पूर्णांक)frame->hd.लंबाई,
                          !!(frame->hd.flags & NGHTTP2_FLAG_END_HEADERS));
  हाल NGHTTP2_PING:
    वापस curl_msnprintf(बफर, blen,
                          "FRAME[PING, len=%d, ack=%d]",
                          (पूर्णांक)frame->hd.लंबाई,
                          frame->hd.flags & NGHTTP2_FLAG_ACK);
  हाल NGHTTP2_GOAWAY: {
    अक्षर scratch[128];
    माप_प्रकार s_len = CURL_ARRAYSIZE(scratch);
    माप_प्रकार len = (frame->goaway.opaque_तथ्य_len < s_len) ?
      frame->goaway.opaque_तथ्य_len : s_len - 1;
    अगर(len)
      स_नकल(scratch, frame->goaway.opaque_तथ्य, len);
    scratch[len] = '\0';
    वापस curl_msnprintf(बफर, blen,
                          "FRAME[GOAWAY, error=%u, reason='%s', "
                          "last_stream=%d]", frame->goaway.error_code,
                          scratch, frame->goaway.last_stream_id);
  }
  हाल NGHTTP2_WINDOW_UPDATE: {
    वापस curl_msnprintf(बफर, blen,
                          "FRAME[WINDOW_UPDATE, incr=%d]",
                          frame->window_update.window_आकार_increment);
  }
  शेष:
    वापस curl_msnprintf(बफर, blen, "FRAME[%d, len=%d, flags=%d]",
                          frame->hd.type, (पूर्णांक)frame->hd.लंबाई,
                          frame->hd.flags);
  }
}

अटल पूर्णांक on_frame_send(nghttp2_session *session, स्थिर nghttp2_frame *frame,
                         व्योम *userp)
{
  काष्ठा Curl_cfilter *cf = userp;
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);

  (व्योम)session;
  DEBUGASSERT(तथ्य);
  अगर(Curl_trc_cf_is_verbose(cf, तथ्य)) {
    अक्षर बफर[256];
    पूर्णांक len;
    len = Curl_nghttp2_fr_print(frame, बफर, माप(बफर) - 1);
    बफर[len] = 0;
    CURL_TRC_CF(तथ्य, cf, "[%d] -> %s", frame->hd.stream_id, बफर);
  }
  अगर((frame->hd.type == NGHTTP2_GOAWAY) && !ctx->sent_goaway) {
    /* A GOAWAY not initiated by us, but by nghttp2 itself on detecting
     * a protocol error on the connection */
    failf(तथ्य, "nghttp2 shuts down connection with error %u: %s",
          frame->goaway.error_code,
          nghttp2_http2_strerror(frame->goaway.error_code));
  }
  वापस 0;
}
#पूर्ण_अगर /* CURLVERBOSE */

अटल पूर्णांक on_frame_recv(nghttp2_session *session, स्थिर nghttp2_frame *frame,
                         व्योम *userp)
{
  काष्ठा Curl_cfilter *cf = userp;
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf), *तथ्य_s;
  काष्ठा h2_stream_ctx *stream;
  int32_t stream_id = frame->hd.stream_id;

  (व्योम)session;
  DEBUGASSERT(तथ्य);
#अगर_घोषित CURLVERBOSE
  अगर(Curl_trc_cf_is_verbose(cf, तथ्य)) {
    अक्षर बफर[256];
    पूर्णांक len;
    len = Curl_nghttp2_fr_print(frame, बफर, माप(बफर) - 1);
    बफर[len] = 0;
    CURL_TRC_CF(तथ्य, cf, "[%d] <- %s", frame->hd.stream_id, बफर);
  }
#पूर्ण_अगर /* CURLVERBOSE */

  अगर(!stream_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(तथ्य);
    चयन(frame->hd.type) {
    हाल NGHTTP2_SETTINGS: {
      अगर(!(frame->hd.flags & NGHTTP2_FLAG_ACK)) {
        uint32_t max_conn = ctx->max_concurrent_streams;
        ctx->max_concurrent_streams =
          Curl_ngh2_session_get_remote_settings(
            &ctx->ngh2, NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS);
        ctx->enable_धकेलो = Curl_ngh2_session_get_remote_settings(
          &ctx->ngh2, NGHTTP2_SETTINGS_ENABLE_धकेलो) != 0;
        CURL_TRC_CF(तथ्य, cf, "[0] MAX_CONCURRENT_STREAMS: %u",
                    ctx->max_concurrent_streams);
        CURL_TRC_CF(तथ्य, cf, "[0] ENABLE_PUSH: %s",
                    ctx->enable_धकेलो ? "TRUE" : "false");
        अगर(तथ्य && max_conn != ctx->max_concurrent_streams) {
          /* only signal change if the value actually changed */
          CURL_TRC_CF(तथ्य, cf, "[0] notify MAX_CONCURRENT_STREAMS: %u",
                      ctx->max_concurrent_streams);
          Curl_multi_connchanged(तथ्य->multi);
        }
        /* Since the initial stream window is 64K, a request might be on HOLD,
         * due to exhaustion. The (initial) SETTINGS may announce a much larger
         * window and *assume* that we treat this like a WINDOW_UPDATE. Some
         * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
         * To be safe, we UNHOLD a stream in order not to stall. */
        अगर(CURL_REQ_WANT_SEND(तथ्य))
          Curl_multi_mark_dirty(तथ्य);
      }
      अवरोध;
    }
    हाल NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->goaway_error = frame->goaway.error_code;
      ctx->remote_max_sid = frame->goaway.last_stream_id;
      अगर(तथ्य) {
        infof(तथ्य, "received GOAWAY, error=%u, last_stream=%d",
              ctx->goaway_error, ctx->remote_max_sid);
        Curl_multi_connchanged(तथ्य->multi);
      }
      अवरोध;
    शेष:
      अवरोध;
    }
    वापस 0;
  }

  तथ्य_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  अगर(!तथ्य_s) {
    CURL_TRC_CF(तथ्य, cf, "[%d] No Curl_easy associated", stream_id);
    वापस 0;
  }

  वापस on_stream_frame(cf, तथ्य_s, stream, frame) ?
         NGHTTP2_ERR_CALLBACK_FAILURE : 0;
}

अटल पूर्णांक cf_h2_on_invalid_frame_recv(nghttp2_session *session,
                                       स्थिर nghttp2_frame *frame,
                                       पूर्णांक ngerr, व्योम *userp)
{
  काष्ठा Curl_cfilter *cf = userp;
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  काष्ठा Curl_easy *तथ्य;
  काष्ठा h2_stream_ctx *stream;
  int32_t stream_id = frame->hd.stream_id;

  (व्योम)session;
  तथ्य = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  अगर(तथ्य) {
#अगर_घोषित CURLVERBOSE
    अक्षर बफर[256];
    पूर्णांक len;
    len = Curl_nghttp2_fr_print(frame, बफर, माप(बफर) - 1);
    बफर[len] = 0;
    failf(तथ्य, "[HTTP2] [%d] received invalid frame: %s, error %d: %s",
          stream_id, बफर, ngerr, nghttp2_strerror(ngerr));
#पूर्ण_अगर /* CURLVERBOSE */
    अगर(stream) {
      Curl_ngh2_submit_rst_stream(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, NGHTTP2_STREAM_CLOSED);
      stream->error = ngerr;
      stream->closed = TRUE;
      stream->reset = TRUE;
    }
  }
  /* Return no error, nghttp2 will RST/GOAWAY by itself when needed */
  वापस 0;
}

अटल पूर्णांक on_तथ्य_chunk_recv(nghttp2_session *session, uint8_t flags,
                              int32_t stream_id,
                              स्थिर uint8_t *mem, माप_प्रकार len, व्योम *userp)
{
  काष्ठा Curl_cfilter *cf = userp;
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  काष्ठा h2_stream_ctx *stream;
  काष्ठा Curl_easy *तथ्य_s;

  (व्योम)session;
  (व्योम)flags;

  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */
  DEBUGASSERT(CF_तथ्य_CURRENT(cf));

  /* get the stream from the hash based on Stream ID */
  तथ्य_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  अगर(!तथ्य_s) {
    /* Receiving a Stream ID not in the hash should not happen - unless
       we have aborted a transfer artificially and there were more data
       in the pipeline. Silently ignore. */
    CURL_TRC_CF(CF_तथ्य_CURRENT(cf), cf, "[%d] Data for unknown", stream_id);
    /* consumed explicitly as no one will read it */
    Curl_ngh2_session_consume(&ctx->ngh2, stream_id, len);
    वापस 0;
  }

  अगर(!stream)
    वापस NGHTTP2_ERR_CALLBACK_FAILURE;

  h2_xfer_write_resp(cf, तथ्य_s, stream, (स्थिर अक्षर *)mem, len, FALSE);

  Curl_ngh2_session_consume(&ctx->ngh2, stream_id, len);
  stream->nrcvd_तथ्य += (curl_off_t)len;
  वापस 0;
}

अटल पूर्णांक on_stream_close(nghttp2_session *session, int32_t stream_id,
                           uint32_t error_code, व्योम *userp)
{
  काष्ठा Curl_cfilter *cf = userp;
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  काष्ठा Curl_easy *तथ्य_s, *call_तथ्य = CF_तथ्य_CURRENT(cf);
  काष्ठा h2_stream_ctx *stream = शून्य;
  पूर्णांक rv;
  (व्योम)session;

  DEBUGASSERT(call_तथ्य);
  /* stream id 0 is the connection, do not look there for streams. */
  तथ्य_s = stream_id ? cf_h2_get_stream_xfer(cf, stream_id, &stream) : शून्य;
  अगर(!तथ्य_s) {
    CURL_TRC_CF(call_तथ्य, cf,
                "[%d] on_stream_close, no easy set on stream", stream_id);
    वापस 0;
  }
  अगर(!GOOD_EASY_HANDLE(तथ्य_s)) {
    /* nghttp2 still has an easy registered for the stream which has
     * been freed be libcurl. This points to a code path that does not
     * trigger DONE or DETACH events as it must. */
    CURL_TRC_CF(call_तथ्य, cf,
                "[%d] on_stream_close, not a GOOD easy on stream", stream_id);
    (व्योम)Curl_ngh2_session_set_stream_user_तथ्य(
      &ctx->ngh2, stream_id, शून्य);
    वापस NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  अगर(!stream) {
    CURL_TRC_CF(तथ्य_s, cf,
                "[%d] on_stream_close, GOOD easy but no stream", stream_id);
    वापस NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  stream->closed = TRUE;
  stream->error = error_code;
  अगर(stream->error) {
    stream->reset = TRUE;
    अगर((stream->error == NGHTTP2_REFUSED_STREAM) &&
       ctx->rcvd_goaway && ctx->goaway_error)
    /* REFUSED_STREAM is used by nghttp2 on a GOAWAY from the
     * server where it indicated it will no longer process this stream.
     * If the GOAWAY carried an error code, take that as the error for
     * the stream. */
    stream->error = ctx->goaway_error;
  }

  अगर(stream->error)
    CURL_TRC_CF(तथ्य_s, cf, "[%d] RESET: %s (err %u)",
              stream_id, nghttp2_http2_strerror(error_code), error_code);
  अन्यथा
    CURL_TRC_CF(तथ्य_s, cf, "[%d] CLOSED", stream_id);
  Curl_multi_mark_dirty(तथ्य_s);

  /* remove `stream_ctx` from the nghttp2 stream */
  rv = Curl_ngh2_session_set_stream_user_तथ्य(
    &ctx->ngh2, stream_id, शून्य);
  अगर(rv) {
    infof(तथ्य_s, "http/2: failed to clear user_data for stream %d",
          stream_id);
    DEBUGASSERT(0);
  }
  वापस 0;
}

अटल व्योम cf_h2_header_error(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य,
                               काष्ठा h2_stream_ctx *stream,
                               CURLcode परिणाम)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;

  failf(तथ्य, "Error receiving HTTP2 header: %d(%s)", (पूर्णांक)परिणाम,
        curl_easy_strerror(परिणाम));
  अगर(stream) {
    Curl_ngh2_submit_rst_stream(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, NGHTTP2_STREAM_CLOSED);
    stream->closed = TRUE;
    stream->reset = TRUE;
  }
}

/* frame->hd.type is either NGHTTP2_HEADERS or NGHTTP2_PUSH_PROMISE */
अटल पूर्णांक on_header(nghttp2_session *session, स्थिर nghttp2_frame *frame,
                     स्थिर uint8_t *name, माप_प्रकार namelen,
                     स्थिर uint8_t *मूल्य, माप_प्रकार valuelen,
                     uint8_t flags,
                     व्योम *userp)
{
  काष्ठा Curl_cfilter *cf = userp;
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  काष्ठा h2_stream_ctx *stream;
  काष्ठा Curl_easy *तथ्य;
  int32_t stream_id = frame->hd.stream_id;
  CURLcode परिणाम;

  (व्योम)session;
  (व्योम)flags;

  DEBUGASSERT(stream_id); /* should never be a zero stream ID here */

  /* get the stream from the hash based on Stream ID */
  तथ्य = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  अगर(!GOOD_EASY_HANDLE(तथ्य))
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    वापस NGHTTP2_ERR_CALLBACK_FAILURE;

  अगर(!stream) {
    failf(तथ्य, "Internal NULL stream");
    वापस NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  /* Store received PUSH_PROMISE headers to be used when the subsequent
     PUSH_PROMISE callback comes */
  अगर(frame->hd.type == NGHTTP2_धकेलो_PROMISE) {
    अक्षर *h;

    अगर((namelen == CURL_CSTRLEN(HTTP_PSEUDO_AUTHORITY)) &&
       !म_भेदन(HTTP_PSEUDO_AUTHORITY, (स्थिर अक्षर *)name, namelen)) {
      /* pseudo headers are lower case */
      पूर्णांक rc = 0;
      अक्षर *check = curl_maprintf("%s:%d", तथ्य->state.origin->hostname,
                                  तथ्य->state.origin->port);
      अगर(!check)
        /* no memory */
        वापस NGHTTP2_ERR_CALLBACK_FAILURE;
      अगर(!curl_strequal(check, (स्थिर अक्षर *)मूल्य) &&
         ((तथ्य->state.origin->port != तथ्य->state.origin->scheme->defport) ||
          !curl_strequal(तथ्य->state.origin->hostname, (स्थिर अक्षर *)मूल्य))) {
        /* This is push is not for the same authority that was asked for in
         * the URL. RFC 7540 section 8.2 says: "A client MUST treat a
         * PUSH_PROMISE for which the server is not authoritative as a stream
         * error of type PROTOCOL_ERROR."
         */
        (व्योम)Curl_ngh2_submit_rst_stream(
          &ctx->ngh2, NGHTTP2_FLAG_NONE, stream_id, NGHTTP2_PROTOCOL_ERROR);
        rc = NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      curlx_free(check);
      अगर(rc)
        वापस rc;
    }

    अगर(!stream->धकेलो_headers) {
      stream->धकेलो_headers_alloc = 10;
      stream->धकेलो_headers = curlx_malloc(stream->धकेलो_headers_alloc *
                                          माप(अक्षर *));
      अगर(!stream->धकेलो_headers)
        वापस NGHTTP2_ERR_CALLBACK_FAILURE;
      stream->धकेलो_headers_used = 0;
    }
    अन्यथा अगर(stream->धकेलो_headers_used ==
            stream->धकेलो_headers_alloc) {
      अक्षर **headp;
      अगर(stream->धकेलो_headers_alloc > 1000) {
        /* this is beyond crazy many headers, bail out */
        failf(तथ्य, "Too many PUSH_PROMISE headers");
        free_धकेलो_headers(stream);
        वापस NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      stream->धकेलो_headers_alloc *= 2;
      headp = curlx_realloc(stream->धकेलो_headers,
                            stream->धकेलो_headers_alloc * माप(अक्षर *));
      अगर(!headp) {
        free_धकेलो_headers(stream);
        वापस NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      stream->धकेलो_headers = headp;
    }
    h = curl_maprintf("%s:%s", name, मूल्य);
    अगर(!h) {
      free_धकेलो_headers(stream);
      वापस NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    stream->धकेलो_headers[stream->धकेलो_headers_used++] = h;
    वापस 0;
  }

  अगर(stream->bodystarted) {
    /* This is a trailer */
    CURL_TRC_CF(तथ्य, cf, "[%d] trailer: %.*s: %.*s",
                stream->id, (पूर्णांक)namelen, name, (पूर्णांक)valuelen, मूल्य);
    परिणाम = Curl_dynhds_add(&stream->resp_trailers,
                             (स्थिर अक्षर *)name, namelen,
                             (स्थिर अक्षर *)मूल्य, valuelen);
    अगर(परिणाम) {
      cf_h2_header_error(cf, तथ्य, stream, परिणाम);
      वापस NGHTTP2_ERR_CALLBACK_FAILURE;
    }

    वापस 0;
  }

  अगर(namelen == CURL_CSTRLEN(HTTP_PSEUDO_STATUS) &&
     !स_भेद(HTTP_PSEUDO_STATUS, name, namelen)) {
    /* nghttp2 guarantees :status is received first and only once. */
    अक्षर बफर[32];
    माप_प्रकार hlen;
    परिणाम = Curl_http_decode_status(&stream->status_code,
                                     (स्थिर अक्षर *)मूल्य, valuelen);
    अगर(परिणाम) {
      cf_h2_header_error(cf, तथ्य, stream, परिणाम);
      वापस NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    hlen = curl_msnprintf(बफर, माप(बफर), HTTP_PSEUDO_STATUS ":%d\r",
                          stream->status_code);
    परिणाम = Curl_headers_धकेलो(तथ्य, बफर, hlen, CURLH_PSEUDO);
    अगर(परिणाम) {
      cf_h2_header_error(cf, तथ्य, stream, परिणाम);
      वापस NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    curlx_dyn_reset(&ctx->scratch);
    परिणाम = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/2 "));
    अगर(!परिणाम)
      परिणाम = curlx_dyn_addn(&ctx->scratch, मूल्य, valuelen);
    अगर(!परिणाम)
      परिणाम = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    अगर(!परिणाम)
      h2_xfer_write_resp_hd(cf, तथ्य, stream, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    अगर(परिणाम) {
      cf_h2_header_error(cf, तथ्य, stream, परिणाम);
      वापस NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    /* if we receive data for another handle, wake that up */
    अगर(CF_तथ्य_CURRENT(cf) != तथ्य)
      Curl_multi_mark_dirty(तथ्य);

    CURL_TRC_CF(तथ्य, cf, "[%d] status: HTTP/2 %03d",
                stream->id, stream->status_code);
    वापस 0;
  }

  /* nghttp2 guarantees that namelen > 0, and :status was already
     received, and this is not pseudo-header field . */
  /* convert to an HTTP1-style header */
  curlx_dyn_reset(&ctx->scratch);
  परिणाम = curlx_dyn_addn(&ctx->scratch, (स्थिर अक्षर *)name, namelen);
  अगर(!परिणाम)
    परिणाम = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
  अगर(!परिणाम)
    परिणाम = curlx_dyn_addn(&ctx->scratch, (स्थिर अक्षर *)मूल्य, valuelen);
  अगर(!परिणाम)
    परिणाम = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
  अगर(!परिणाम)
    h2_xfer_write_resp_hd(cf, तथ्य, stream, curlx_dyn_ptr(&ctx->scratch),
                          curlx_dyn_len(&ctx->scratch), FALSE);
  अगर(परिणाम) {
    cf_h2_header_error(cf, तथ्य, stream, परिणाम);
    वापस NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  /* if we receive data for another handle, wake that up */
  अगर(CF_तथ्य_CURRENT(cf) != तथ्य)
    Curl_multi_mark_dirty(तथ्य);

  CURL_TRC_CF(तथ्य, cf, "[%d] header: %.*s: %.*s",
              stream->id, (पूर्णांक)namelen, name, (पूर्णांक)valuelen, मूल्य);

  वापस 0; /* 0 is successful */
}

अटल ssize_t req_body_read_callback(nghttp2_session *session,
                                      int32_t stream_id,
                                      uint8_t *buf, माप_प्रकार लंबाई,
                                      uint32_t *तथ्य_flags,
                                      nghttp2_तथ्य_source *source,
                                      व्योम *userp)
{
  काष्ठा Curl_cfilter *cf = userp;
  काष्ठा Curl_easy *तथ्य_s;
  काष्ठा h2_stream_ctx *stream = शून्य;
  CURLcode परिणाम;
  ssize_t nread;
  माप_प्रकार n;
  (व्योम)source;

  (व्योम)session;
  अगर(!stream_id)
    वापस NGHTTP2_ERR_INVALID_ARGUMENT;

  /* get the stream from the hash based on Stream ID, stream ID zero is for
     connection-oriented stuff */
  तथ्य_s = cf_h2_get_stream_xfer(cf, stream_id, &stream);
  अगर(!तथ्य_s || !stream)
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    वापस NGHTTP2_ERR_CALLBACK_FAILURE;

  परिणाम = Curl_bufq_read(&stream->sendbuf, buf, लंबाई, &n);
  अगर(परिणाम) {
    अगर(परिणाम != CURLE_AGAIN)
      वापस NGHTTP2_ERR_CALLBACK_FAILURE;
    nread = 0;
  }
  अन्यथा
    nread = (ssize_t)n;

  CURL_TRC_CF(तथ्य_s, cf, "[%d] req_body_read(len=%zu) eos=%d -> %zd, %d",
              stream_id, लंबाई, stream->body_eos, nread, (पूर्णांक)परिणाम);

  अगर(stream->body_eos && Curl_bufq_is_empty(&stream->sendbuf)) {
    *तथ्य_flags = NGHTTP2_तथ्य_FLAG_EOF;
    वापस nread;
  }
  वापस (nread == 0) ? NGHTTP2_ERR_DEFERRED : nread;
}

#अगर_घोषित CURLVERBOSE
अटल पूर्णांक error_callback(nghttp2_session *session,
                          स्थिर अक्षर *संदेश,
                          माप_प्रकार len,
                          व्योम *userp)
{
  काष्ठा Curl_cfilter *cf = userp;
  काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);
  (व्योम)session;
  failf(तथ्य, "%.*s", (पूर्णांक)len, संदेश);
  वापस 0;
}
#पूर्ण_अगर

/*
 * Append headers to ask for an HTTP1.1 to HTTP2 upgrade.
 */
CURLcode Curl_http2_request_upgrade(काष्ठा dynbuf *req,
                                    काष्ठा Curl_easy *तथ्य)
{
  CURLcode परिणाम;
  अक्षर *base64;
  माप_प्रकार blen;
  काष्ठा SingleRequest *k = &तथ्य->req;
  uint8_t binsettings[H2_BINSETTINGS_LEN];
  ssize_t rc;
  माप_प्रकार binlen; /* length of the binsettings data */

  rc = populate_binsettings(binsettings, तथ्य);
  अगर(!curlx_sztouz(rc, &binlen) || !binlen) {
    failf(तथ्य, "nghttp2 unexpectedly failed on pack_settings_payload");
    curlx_dyn_free(req);
    वापस CURLE_FAILED_INIT;
  }

  परिणाम = curlx_base64url_encode(binsettings, binlen, &base64, &blen);
  अगर(परिणाम) {
    curlx_dyn_free(req);
    वापस परिणाम;
  }

  तथ्य->state.http_hd_upgrade = TRUE;
  तथ्य->state.http_hd_h2_settings = TRUE;
  परिणाम = curlx_dyn_addf(req,
                          "Upgrade: %s\r\n"
                          "HTTP2-Settings: %s\r\n",
                          NGHTTP2_CLEARTEXT_PROTO_VERSION_ID, base64);
  curlx_free(base64);

  k->upgr101 = UPGR101_H2;
  तथ्य->conn->bits.upgrade_in_progress = TRUE;

  वापस परिणाम;
}

अटल CURLcode http2_handle_stream_close(काष्ठा Curl_cfilter *cf,
                                          काष्ठा Curl_easy *तथ्य,
                                          काष्ठा h2_stream_ctx *stream,
                                          माप_प्रकार *pnlen)
{
  CURLcode परिणाम;

  *pnlen = 0;
  अगर(stream->reset) {
    अगर(stream->error == NGHTTP2_REFUSED_STREAM) {
      infof(तथ्य, "HTTP/2 stream %d refused by server, try again on a new "
                  "connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      तथ्य->state.refused_stream = TRUE;
      वापस CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    अन्यथा अगर(stream->resp_hds_complete && तथ्य->req.no_body) {
      CURL_TRC_CF(तथ्य, cf, "[%d] error after response headers, but we did "
                  "not want a body anyway, ignore: %s (err %u)",
                  stream->id, nghttp2_http2_strerror(stream->error),
                  stream->error);
      stream->close_handled = TRUE;
      वापस CURLE_OK;
    }
    failf(तथ्य, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          stream->id, stream->reset_by_server ? "server" : "curl",
          stream->error, nghttp2_http2_strerror(stream->error));
    वापस stream->error ? CURLE_HTTP2_STREAM :
           (तथ्य->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2);
  }
  अन्यथा अगर(!stream->bodystarted) {
    failf(तथ्य, "HTTP/2 stream %d was closed cleanly, but before getting "
          "all response header fields, treated as error", stream->id);
    वापस CURLE_HTTP2_STREAM;
  }

  अगर(Curl_dynhds_गिनती(&stream->resp_trailers)) {
    काष्ठा dynhds_entry *e;
    काष्ठा dynbuf dbuf;
    माप_प्रकार i;

    परिणाम = CURLE_OK;
    curlx_dyn_init(&dbuf, DYN_TRAILERS);
    क्रम(i = 0; i < Curl_dynhds_गिनती(&stream->resp_trailers); ++i) {
      e = Curl_dynhds_getn(&stream->resp_trailers, i);
      अगर(!e)
        अवरोध;
      curlx_dyn_reset(&dbuf);
      परिणाम = curlx_dyn_addf(&dbuf, "%.*s: %.*s\x0d\x0a",
                              (पूर्णांक)e->namelen, e->name,
                              (पूर्णांक)e->valuelen, e->मूल्य);
      अगर(परिणाम)
        अवरोध;
      Curl_debug(तथ्य, CURLINFO_HEADER_IN, curlx_dyn_ptr(&dbuf),
                 curlx_dyn_len(&dbuf));
      परिणाम = Curl_client_write(तथ्य,
                                 CLIENTWRITE_HEADER | CLIENTWRITE_TRAILER,
                                 curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf));
      अगर(परिणाम)
        अवरोध;
      परिणाम = Curl_bump_headersize(तथ्य, curlx_dyn_len(&dbuf), FALSE);
      अगर(परिणाम)
        वापस परिणाम;
    }
    curlx_dyn_free(&dbuf);
    अगर(परिणाम)
      जाओ out;
  }

  stream->close_handled = TRUE;
  परिणाम = CURLE_OK;

out:
  CURL_TRC_CF(तथ्य, cf, "handle_stream_close -> %d, %zu", (पूर्णांक)परिणाम, *pnlen);
  वापस परिणाम;
}

अटल पूर्णांक sweight_wanted(स्थिर काष्ठा Curl_easy *तथ्य)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  वापस तथ्य->set.weight ?
    तथ्य->set.weight : NGHTTP2_DEFAULT_WEIGHT;
}

अटल पूर्णांक sweight_in_effect(स्थिर काष्ठा Curl_easy *तथ्य)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  वापस तथ्य->state.weight ?
    तथ्य->state.weight : NGHTTP2_DEFAULT_WEIGHT;
}

/*
 * h2_pri_spec() fills in the pri_spec struct, used by nghttp2 to send weight
 * and dependency to the peer. It also stores the updated values in the state
 * struct.
 */

अटल व्योम h2_pri_spec(काष्ठा Curl_easy *तथ्य,
                        nghttp2_priority_spec *pri_spec)
{
  पूर्णांक prio = तथ्य->set.weight;
  nghttp2_priority_spec_init(pri_spec, 0, sweight_wanted(तथ्य), FALSE);
  तथ्य->state.weight = prio;
}

/*
 * Check if there is been an update in the priority /
 * dependency settings and if so it submits a PRIORITY frame with the updated
 * info.
 * Flush any out data pending in the network buffer.
 */
अटल CURLcode h2_progress_egress(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  पूर्णांक rv = 0;

  अगर(sweight_wanted(तथ्य) != sweight_in_effect(तथ्य)) {
    काष्ठा h2_stream_ctx *stream = H2_STREAM_CTX(ctx, तथ्य);
    अगर(stream && stream->id > 0 &&
       (sweight_wanted(तथ्य) != sweight_in_effect(तथ्य))) {
      /* send new weight and/or dependency */
      nghttp2_priority_spec pri_spec;

      h2_pri_spec(तथ्य, &pri_spec);
      CURL_TRC_CF(तथ्य, cf, "[%d] Queuing PRIORITY", stream->id);
      DEBUGASSERT(stream->id != -1);
      rv = Curl_ngh2_submit_priority(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, stream->id, &pri_spec);
      अगर(rv)
        जाओ out;
    }
  }

  ctx->nw_out_blocked = 0;
  जबतक(!rv && !ctx->nw_out_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

out:
  अगर(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(तथ्य, cf, "nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    वापस CURLE_SEND_ERROR;
  }
  /* Defer flushing during the connect phase so that the SETTINGS and
   * other initial frames are sent together with the first request.
   * Unless we are 'connect_only' where the request will never come. */
  अगर(!cf->connected && !cf->conn->bits.connect_only)
    वापस CURLE_OK;
  वापस nw_out_flush(cf, तथ्य);
}

अटल CURLcode stream_recv(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                            काष्ठा h2_stream_ctx *stream,
                            अक्षर *buf, माप_प्रकार len, माप_प्रकार *pnread)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_AGAIN;

  (व्योम)buf;
  (व्योम)len;
  *pnread = 0;

  अगर(!stream->xfer_परिणाम)
    stream->xfer_परिणाम = cf_h2_update_local_win(cf, तथ्य, stream);

  अगर(stream->xfer_परिणाम) {
    CURL_TRC_CF(तथ्य, cf, "[%d] xfer write failed", stream->id);
    परिणाम = stream->xfer_परिणाम;
  }
  अन्यथा अगर(stream->closed) {
    CURL_TRC_CF(तथ्य, cf, "[%d] returning CLOSE", stream->id);
    परिणाम = http2_handle_stream_close(cf, तथ्य, stream, pnread);
  }
  अन्यथा अगर(stream->reset ||
          (ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) ||
          (ctx->rcvd_goaway && ctx->remote_max_sid < stream->id)) {
    CURL_TRC_CF(तथ्य, cf, "[%d] returning ERR", stream->id);
    परिणाम = तथ्य->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2;
  }

  अगर(परिणाम && (परिणाम != CURLE_AGAIN))
    CURL_TRC_CF(तथ्य, cf, "[%d] stream_recv(len=%zu) -> %d, %zu",
                stream->id, len, (पूर्णांक)परिणाम, *pnread);
  वापस परिणाम;
}

अटल CURLcode h2_progress_ingress(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य,
                                    माप_प्रकार तथ्य_max_bytes)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार nread;

  अगर(should_close_session(ctx)) {
    CURL_TRC_CF(तथ्य, cf, "[0] ingress: session is closed");
    वापस CURLE_HTTP2;
  }

  /* Process network input buffer first */
  अगर(!Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(तथ्य, cf, "Process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    परिणाम = h2_process_pending_निवेश(cf, तथ्य);
    अगर(परिणाम)
      वापस परिणाम;
  }

  अगर(!तथ्य_max_bytes)
    तथ्य_max_bytes = H2_CHUNK_आकार;

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop due to connection close or us not processing
   * all network input */
  जबतक(!ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) {
    काष्ठा h2_stream_ctx *stream = H2_STREAM_CTX(ctx, तथ्य);
    अगर(stream && (stream->closed || !तथ्य_max_bytes)) {
      /* We would like to abort here and stop processing, so that the transfer
       * loop can handle the data/close here. This may leave data in
       * underlying buffers that will not be consumed. */
      अगर(!cf->अग्र || !cf->अग्र->cft->has_तथ्य_pending(cf->अग्र, तथ्य))
        Curl_multi_mark_dirty(तथ्य);
      अवरोध;
    }

    परिणाम = Curl_cf_recv_bufq(cf->अग्र, तथ्य, &ctx->inbufq, 0, &nread);
    अगर(परिणाम) {
      अगर(परिणाम != CURLE_AGAIN) {
        failf(तथ्य, "Failed receiving HTTP2 data: %d(%s)", (पूर्णांक)परिणाम,
              curl_easy_strerror(परिणाम));
        वापस परिणाम;
      }
      अवरोध;
    }
    अन्यथा अगर(nread == 0) {
      CURL_TRC_CF(तथ्य, cf, "[0] ingress: connection closed");
      ctx->conn_closed = TRUE;
      अवरोध;
    }
    अन्यथा {
      CURL_TRC_CF(तथ्य, cf, "[0] ingress: read %zu bytes", nread);
      तथ्य_max_bytes = (तथ्य_max_bytes > nread) ? (तथ्य_max_bytes - nread) : 0;
    }

    परिणाम = h2_process_pending_निवेश(cf, तथ्य);
    अगर(परिणाम)
      वापस परिणाम;
    CURL_TRC_CF(तथ्य, cf, "[0] ingress: nw-in buffered %zu",
                Curl_bufq_len(&ctx->inbufq));
  }

  अगर(ctx->conn_closed && Curl_bufq_is_empty(&ctx->inbufq)) {
    CURL_TRC_CF(तथ्य, cf, "server closed %s",
                ctx->rcvd_goaway ? "with GOAWAY" : "abruptly");
    connclose(cf->conn);
  }

  CURL_TRC_CF(तथ्य, cf, "[0] ingress: done");
  वापस CURLE_OK;
}

अटल CURLcode cf_h2_recv(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                           अक्षर *buf, माप_प्रकार len, माप_प्रकार *pnread)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  काष्ठा h2_stream_ctx *stream;
  CURLcode परिणाम, r2;
  काष्ठा cf_call_तथ्य save;

  अगर(!तथ्य)
    वापस CURLE_HTTP2;

  stream = H2_STREAM_CTX(ctx, तथ्य);

  *pnread = 0;
  अगर(!stream) {
    /* Abnormal call sequence: either this transfer has never opened a stream
     * (unlikely) or the transfer has been done, cleaned up its resources, but
     * a read() is called anyway. It is not clear what the calling sequence
     * is for such a case. */
    failf(तथ्य, "http/2 recv on a transfer never opened "
          "or already cleared, mid=%u", तथ्य->mid);
    वापस CURLE_HTTP2;
  }

  CF_तथ्य_SAVE(save, cf, तथ्य);

  परिणाम = stream_recv(cf, तथ्य, stream, buf, len, pnread);
  अगर(परिणाम && (परिणाम != CURLE_AGAIN))
    जाओ out;

  अगर(परिणाम) {
    परिणाम = h2_progress_ingress(cf, तथ्य, len);
    अगर(परिणाम)
      जाओ out;

    परिणाम = stream_recv(cf, तथ्य, stream, buf, len, pnread);
  }

  अगर(*pnread > 0) {
    /* Now that we transferred this to the upper layer, we report
     * the actual amount of DATA consumed to the H2 session, so
     * that it adjusts stream flow control */
    Curl_ngh2_session_consume(&ctx->ngh2, stream->id, *pnread);
    अगर(stream->closed) {
      CURL_TRC_CF(तथ्य, cf, "[%d] DRAIN closed stream", stream->id);
      Curl_multi_mark_dirty(तथ्य);
    }
  }

out:
  r2 = h2_progress_egress(cf, तथ्य);
  अगर(r2 == CURLE_AGAIN) {
    /* pending data to send, need to be called again. Ideally, we
     * monitor the socket for POLLOUT, but when not SENDING
     * any more, we force processing of the transfer. */
    अगर(!CURL_REQ_WANT_SEND(तथ्य))
      Curl_multi_mark_dirty(तथ्य);
  }
  अन्यथा अगर(r2 && (परिणाम == CURLE_AGAIN)) {
    /* Do not replace a previous error result NOR a successful result
     * with an egress error. Example: the server might close the
     * connection after sending the complete response with EOS
     * and everything. Further sending may fail, but that MUST
     * not change that we had a successful response. */
    परिणाम = r2;
  }

  CURL_TRC_CF(तथ्य, cf, "[%d] cf_recv(len=%zu) -> %d, %zu, "
              "window=%d/%d, connection %d/%d",
              stream->id, len, (पूर्णांक)परिणाम, *pnread,
              Curl_ngh2_session_get_stream_effective_recv_तथ्य_लंबाई(
                &ctx->ngh2, stream->id),
              Curl_ngh2_session_get_stream_effective_local_window_आकार(
                &ctx->ngh2, stream->id),
              Curl_ngh2_session_get_local_window_आकार(&ctx->ngh2),
              HTTP2_HUGE_WINDOW_आकार);

  CF_तथ्य_RESTORE(cf, save);
  वापस परिणाम;
}

अटल CURLcode cf_h2_body_send(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य,
                                काष्ठा h2_stream_ctx *stream,
                                स्थिर व्योम *buf, माप_प्रकार blen, bool eos,
                                माप_प्रकार *pnwritten)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  CURLcode परिणाम;

  *pnwritten = 0;
  अगर(stream->closed) {
    अगर(stream->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(तथ्य, cf, "[%d] discarding data"
                  "on closed stream with response", stream->id);
      अगर(eos)
        stream->body_eos = TRUE;
      *pnwritten = blen;
      वापस CURLE_OK;
    }
    /* Server closed before we got a response, this is an error */
    infof(तथ्य, "stream %d closed", stream->id);
    वापस CURLE_SEND_ERROR;
  }

  परिणाम = Curl_bufq_write(&stream->sendbuf, buf, blen, pnwritten);
  अगर(परिणाम)
    वापस परिणाम;

  अगर(eos && (blen == *pnwritten))
    stream->body_eos = TRUE;

  अगर(eos || !Curl_bufq_is_empty(&stream->sendbuf)) {
    /* resume the potentially suspended stream */
    अगर(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_तथ्य(&ctx->ngh2, stream->id)))
      वापस CURLE_SEND_ERROR;
  }

  वापस CURLE_OK;
}

अटल CURLcode h2_submit(काष्ठा h2_stream_ctx **pstream,
                          काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                          स्थिर uint8_t *buf, माप_प्रकार len,
                          bool eos, माप_प्रकार *pnwritten)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  काष्ठा h2_stream_ctx *stream = शून्य;
  काष्ठा dynhds h2_headers;
  nghttp2_nv *nva = शून्य;
  स्थिर व्योम *body = शून्य;
  माप_प्रकार nheader, bodylen;
  nghttp2_तथ्य_provider तथ्य_prd;
  int32_t stream_id;
  nghttp2_priority_spec pri_spec;
  माप_प्रकार nwritten;
  CURLcode परिणाम = CURLE_OK;
  uint32_t initial_win_आकार;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_headers, MAX_HTTP_RESP_HEADER_गिनती, DYN_HTTP_REQUEST);

  परिणाम = http2_तथ्य_setup(cf, तथ्य, &stream);
  अगर(परिणाम)
    जाओ out;

  परिणाम = Curl_h1_req_parse_read(
    &stream->h1, buf, len, शून्य,
    !तथ्य->state.http_ignorecustom ?
    CURL_EASY_STR(तथ्य, STRING_CUSTOMREQUEST) : शून्य,
    0, &nwritten);
  अगर(परिणाम)
    जाओ out;
  *pnwritten = nwritten;
  अगर(!stream->h1.done) {
    /* need more data */
    जाओ out;
  }
  DEBUGASSERT(stream->h1.req);

  परिणाम = Curl_http_req_to_h2(&h2_headers, stream->h1.req, तथ्य);
  अगर(परिणाम)
    जाओ out;
  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nva = Curl_dynhds_to_nva(&h2_headers, &nheader);
  अगर(!nva) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  h2_pri_spec(तथ्य, &pri_spec);
  अगर(!Curl_ngh2_session_check_request_allowed(&ctx->ngh2))
    CURL_TRC_CF(तथ्य, cf, "send request NOT allowed (via nghttp2)");

  /* Check the initial windows size of the transfer (rate-limits?) and
   * send an updated settings on changes from previous value. */
  initial_win_आकार = cf_h2_initial_win_आकार(तथ्य);
  अगर(initial_win_आकार != ctx->initial_win_आकार) {
    परिणाम = cf_h2_update_settings(ctx, initial_win_आकार);
    अगर(परिणाम)
      जाओ out;
  }

  चयन(तथ्य->state.httpreq) {
  हाल HTTPREQ_POST:
  हाल HTTPREQ_POST_FORM:
  हाल HTTPREQ_POST_MIME:
  हाल HTTPREQ_PUT:
    तथ्य_prd.read_callback = req_body_read_callback;
    तथ्य_prd.source.ptr = शून्य;
    stream_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, &तथ्य_prd, stream);
    अवरोध;
  शेष:
    stream_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, शून्य, stream);
  }

  अगर(stream_id < 0) {
    CURL_TRC_CF(तथ्य, cf, "send: nghttp2_submit_request error (%s)%d",
                nghttp2_strerror(stream_id), stream_id);
    परिणाम = CURLE_SEND_ERROR;
    जाओ out;
  }

#अगर_घोषित CURLVERBOSE
#घोषणा MAX_ACC 60000  /* <64KB to account for some overhead */
  अगर(Curl_trc_is_verbose(तथ्य)) {
    माप_प्रकार acc = 0, i;

    infof(तथ्य, "[HTTP/2] [%d] OPENED stream for %s",
          stream_id, Curl_bufref_ptr(&तथ्य->state.url));
    क्रम(i = 0; i < nheader; ++i) {
      acc += nva[i].namelen + nva[i].valuelen;

      infof(तथ्य, "[HTTP/2] [%d] [%.*s: %.*s]", stream_id,
            (पूर्णांक)nva[i].namelen, nva[i].name,
            (पूर्णांक)nva[i].valuelen, nva[i].मूल्य);
    }

    अगर(acc > MAX_ACC) {
      infof(तथ्य, "[HTTP/2] Warning: The cumulative length of all "
            "headers exceeds %d bytes and that could cause the "
            "stream to be rejected.", MAX_ACC);
    }
  }
#पूर्ण_अगर

  stream->id = stream_id;

  body = (स्थिर अक्षर *)buf + *pnwritten;
  bodylen = len - *pnwritten;

  अगर(bodylen || eos) {
    माप_प्रकार n;
    परिणाम = cf_h2_body_send(cf, तथ्य, stream, body, bodylen, eos, &n);
    अगर(!परिणाम)
      *pnwritten += n;
    अन्यथा अगर(परिणाम == CURLE_AGAIN)
      परिणाम = CURLE_OK;
    अन्यथा {
      परिणाम = CURLE_SEND_ERROR;
    }
  }

out:
  CURL_TRC_CF(तथ्य, cf, "[%d] submit -> %d, %zu",
              stream ? stream->id : -1, (पूर्णांक)परिणाम, *pnwritten);
  curlx_safefree(nva);
  *pstream = stream;
  Curl_dynhds_free(&h2_headers);
  वापस परिणाम;
}

अटल CURLcode cf_h2_send(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                           स्थिर uint8_t *buf, माप_प्रकार len, bool eos,
                           माप_प्रकार *pnwritten)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  काष्ठा h2_stream_ctx *stream = H2_STREAM_CTX(ctx, तथ्य);
  काष्ठा cf_call_तथ्य save;
  CURLcode परिणाम = CURLE_OK, r2;

  CF_तथ्य_SAVE(save, cf, तथ्य);
  *pnwritten = 0;

  अगर(!stream || stream->id == -1) {
    परिणाम = h2_submit(&stream, cf, तथ्य, buf, len, eos, pnwritten);
    अगर(परिणाम)
      जाओ out;
    DEBUGASSERT(stream);
  }
  अन्यथा अगर(stream->body_eos) {
    /* We already wrote this, but CURLE_AGAIN-ed the call due to not
     * being able to flush stream->sendbuf. Make a 0-length write
     * to trigger flushing again.
     * If this works, we report to have written `len` bytes. */
    माप_प्रकार n;
    DEBUGASSERT(eos);
    परिणाम = cf_h2_body_send(cf, तथ्य, stream, buf, 0, eos, &n);
    CURL_TRC_CF(तथ्य, cf, "[%d] cf_body_send last CHUNK -> %d, %zu, eos=%d",
                stream->id, (पूर्णांक)परिणाम, n, eos);
    अगर(परिणाम)
      जाओ out;
    *pnwritten = len;
  }
  अन्यथा {
    परिणाम = cf_h2_body_send(cf, तथ्य, stream, buf, len, eos, pnwritten);
    CURL_TRC_CF(तथ्य, cf, "[%d] cf_body_send(len=%zu) -> %d, %zu, eos=%d",
                stream->id, len, (पूर्णांक)परिणाम, *pnwritten, eos);
  }

  /* Call the nghttp2 send loop and flush to write ALL buffered data,
   * headers and/or request body completely out to the network */
  r2 = h2_progress_egress(cf, तथ्य);

  /* if the stream has been closed in egress handling (nghttp2 does that
   * when it does not like the headers, for example */
  अगर(stream && stream->closed && !stream->resp_hds_complete) {
    infof(तथ्य, "stream %d closed", stream->id);
    परिणाम = CURLE_SEND_ERROR;
    जाओ out;
  }
  अन्यथा अगर(r2 && (r2 != CURLE_AGAIN)) {
    परिणाम = r2;
    जाओ out;
  }

  अगर(should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    अगर(stream && stream->closed) {
      परिणाम = http2_handle_stream_close(cf, तथ्य, stream, pnwritten);
    }
    अन्यथा {
      CURL_TRC_CF(तथ्य, cf, "send: nothing to do in this session");
      परिणाम = CURLE_HTTP2;
    }
  }

out:
  अगर(stream) {
    CURL_TRC_CF(तथ्य, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
                "eos=%d, h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                stream->id, len, (पूर्णांक)परिणाम, *pnwritten,
                stream->body_eos,
                Curl_ngh2_session_get_stream_remote_window_आकार(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_remote_window_आकार(&ctx->ngh2),
                Curl_bufq_len(&stream->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  अन्यथा {
    CURL_TRC_CF(तथ्य, cf, "cf_send(len=%zu) -> %d, %zu, "
                "connection-window=%d, nw_send_buffer(%zu)",
                len, (पूर्णांक)परिणाम, *pnwritten,
                Curl_ngh2_session_get_remote_window_आकार(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_तथ्य_RESTORE(cf, save);
  वापस परिणाम;
}

अटल CURLcode cf_h2_flush(काष्ठा Curl_cfilter *cf,
                            काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  काष्ठा h2_stream_ctx *stream = H2_STREAM_CTX(ctx, तथ्य);
  काष्ठा cf_call_तथ्य save;
  CURLcode परिणाम = CURLE_OK;

  CF_तथ्य_SAVE(save, cf, तथ्य);
  अगर(stream && !Curl_bufq_is_empty(&stream->sendbuf)) {
    /* resume the potentially suspended stream */
    अगर(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_तथ्य(&ctx->ngh2, stream->id))) {
      परिणाम = CURLE_SEND_ERROR;
      जाओ out;
    }
  }

  परिणाम = h2_progress_egress(cf, तथ्य);

out:
  अगर(stream) {
    CURL_TRC_CF(तथ्य, cf, "[%d] flush -> %d, "
                "h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                stream->id, (पूर्णांक)परिणाम,
                Curl_ngh2_session_get_stream_remote_window_आकार(
                  &ctx->ngh2, stream->id),
                Curl_ngh2_session_get_remote_window_आकार(&ctx->ngh2),
                Curl_bufq_len(&stream->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  अन्यथा {
    CURL_TRC_CF(तथ्य, cf, "flush -> %d, "
                "connection-window=%d, nw_send_buffer(%zu)", (पूर्णांक)परिणाम,
                Curl_ngh2_session_get_remote_window_आकार(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_तथ्य_RESTORE(cf, save);
  वापस परिणाम;
}

अटल CURLcode cf_h2_adjust_pollset(काष्ठा Curl_cfilter *cf,
                                     काष्ठा Curl_easy *तथ्य,
                                     काष्ठा easy_pollset *ps)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  काष्ठा cf_call_तथ्य save;
  curl_socket_t sock;
  bool want_recv, want_send;
  CURLcode परिणाम = CURLE_OK;

  अगर(!Curl_ngh2_valid(&ctx->ngh2))
    वापस CURLE_OK;

  sock = Curl_conn_cf_get_socket(cf, तथ्य);
  Curl_pollset_check(तथ्य, ps, sock, &want_recv, &want_send);
  अगर(want_recv || want_send) {
    काष्ठा h2_stream_ctx *stream = H2_STREAM_CTX(ctx, तथ्य);
    bool c_exhaust, s_exhaust;

    CF_तथ्य_SAVE(save, cf, तथ्य);
    c_exhaust = want_send &&
                !Curl_ngh2_session_get_remote_window_आकार(&ctx->ngh2);
    s_exhaust = want_send && stream && stream->id >= 0 &&
                !Curl_ngh2_session_get_stream_remote_window_आकार(
                  &ctx->ngh2, stream->id);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                (!c_exhaust && Curl_ngh2_session_want_write(&ctx->ngh2)) ||
                !Curl_bufq_is_empty(&ctx->outbufq);

    परिणाम = Curl_pollset_set(तथ्य, ps, sock, want_recv, want_send);
    CF_तथ्य_RESTORE(cf, save);
  }
  अन्यथा अगर(ctx->sent_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_तथ्य_SAVE(save, cf, तथ्य);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_empty(&ctx->outbufq);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    परिणाम = Curl_pollset_set(तथ्य, ps, sock, want_recv, want_send);
    CF_तथ्य_RESTORE(cf, save);
  }
  वापस परिणाम;
}

अटल CURLcode cf_h2_ctx_open(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  काष्ठा h2_stream_ctx *stream;
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;
  पूर्णांक rc;
  nghttp2_session_callbacks *cbs = शून्य;

  DEBUGASSERT(!ctx->ngh2.s);
  DEBUGASSERT(!ctx->ngh2.fatal_rv);
  DEBUGASSERT(ctx->initialized);

  rc = nghttp2_session_callbacks_new(&cbs);
  अगर(rc) {
    failf(तथ्य, "Could not initialize nghttp2 callbacks");
    जाओ out;
  }

  nghttp2_session_callbacks_set_send_callback(cbs, send_callback);
  nghttp2_session_callbacks_set_on_frame_recv_callback(cbs, on_frame_recv);
  nghttp2_session_callbacks_set_on_invalid_frame_recv_callback(cbs,
    cf_h2_on_invalid_frame_recv);
#अगर_घोषित CURLVERBOSE
  nghttp2_session_callbacks_set_on_frame_send_callback(cbs, on_frame_send);
#पूर्ण_अगर
  nghttp2_session_callbacks_set_on_तथ्य_chunk_recv_callback(
    cbs, on_तथ्य_chunk_recv);
  nghttp2_session_callbacks_set_on_stream_close_callback(cbs, on_stream_close);
  nghttp2_session_callbacks_set_on_header_callback(cbs, on_header);
#अगर_घोषित CURLVERBOSE
  nghttp2_session_callbacks_set_error_callback(cbs, error_callback);
#पूर्ण_अगर

  /* The nghttp2 session is not yet setup, do it */
  rc = h2_client_new(cf, cbs);
  अगर(rc) {
    failf(तथ्य, "Could not initialize nghttp2");
    जाओ out;
  }
  ctx->max_concurrent_streams = तथ्य->multi ?
    Curl_multi_max_concurrent_streams(तथ्य->multi) :
    DEFAULT_MAX_CONCURRENT_STREAMS;

  अगर(ctx->via_h1_upgrade) {
    /* HTTP/1.1 Upgrade issued. H2 Settings have already been submitted
     * in the H1 request and we upgrade from there. This stream
     * is opened implicitly as #1. */
    uint8_t binsettings[H2_BINSETTINGS_LEN];
    ssize_t rclen;
    माप_प्रकार binlen; /* length of the binsettings data */

    rclen = populate_binsettings(binsettings, तथ्य);

    अगर(!curlx_sztouz(rclen, &binlen) || !binlen) {
      failf(तथ्य, "nghttp2 unexpectedly failed on pack_settings_payload");
      परिणाम = CURLE_FAILED_INIT;
      जाओ out;
    }

    परिणाम = http2_तथ्य_setup(cf, तथ्य, &stream);
    अगर(परिणाम)
      जाओ out;
    DEBUGASSERT(stream);
    stream->id = 1;
    /* queue SETTINGS frame (again) */
    rc = Curl_ngh2_session_upgrade2(
      &ctx->ngh2, binsettings, binlen, तथ्य->state.httpreq == HTTPREQ_शीर्ष,
      शून्य);
    अगर(rc) {
      failf(तथ्य, "nghttp2_session_upgrade2() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      परिणाम = CURLE_HTTP2;
      जाओ out;
    }

    rc = Curl_ngh2_session_set_stream_user_तथ्य(
      &ctx->ngh2, stream->id, stream);
    अगर(rc) {
      infof(तथ्य, "http/2: failed to set user_data for stream %d",
            stream->id);
      DEBUGASSERT(0);
    }
    CURL_TRC_CF(तथ्य, cf, "created session via Upgrade");
  }
  अन्यथा {
    nghttp2_settings_entry iv[H2_SETTINGS_IV_LEN];
    माप_प्रकार ivlen;

    ivlen = populate_settings(iv, तथ्य, ctx);
    rc = Curl_ngh2_submit_settings(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, iv, ivlen);
    अगर(rc) {
      failf(तथ्य, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      परिणाम = CURLE_HTTP2;
      जाओ out;
    }
  }

  rc = Curl_ngh2_session_set_local_window_आकार(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, HTTP2_HUGE_WINDOW_आकार);
  अगर(rc) {
    failf(तथ्य, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    परिणाम = CURLE_HTTP2;
    जाओ out;
  }

  /* all set, traffic will be send on connect */
  परिणाम = CURLE_OK;
  CURL_TRC_CF(तथ्य, cf, "[0] created h2 session%s",
              ctx->via_h1_upgrade ? " (via h1 upgrade)" : "");

out:
  अगर(cbs)
    nghttp2_session_callbacks_del(cbs);
  वापस परिणाम;
}

अटल CURLcode cf_h2_connect(काष्ठा Curl_cfilter *cf,
                              काष्ठा Curl_easy *तथ्य,
                              bool *done)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;
  काष्ठा cf_call_तथ्य save;
  bool first_time = FALSE;

  अगर(cf->connected) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  /* Connect the lower filters first */
  अगर(!cf->अग्र->connected) {
    परिणाम = Curl_conn_cf_connect(cf->अग्र, तथ्य, done);
    अगर(परिणाम || !*done)
      वापस परिणाम;
  }

  *done = FALSE;

  CF_तथ्य_SAVE(save, cf, तथ्य);
  DEBUGASSERT(ctx->initialized);
  अगर(!ctx->ngh2.s) {
    परिणाम = cf_h2_ctx_open(cf, तथ्य);
    अगर(परिणाम)
      जाओ out;
    first_time = TRUE;
  }

  अगर(!first_time) {
    परिणाम = h2_progress_ingress(cf, तथ्य, 0);
    अगर(परिणाम)
      जाओ out;
  }

  /* Send out our SETTINGS and ACKs and such. If that blocks, we
   * have it buffered and can count this filter as being connected */
  परिणाम = h2_progress_egress(cf, तथ्य);
  अगर(परिणाम && (परिणाम != CURLE_AGAIN))
    जाओ out;

  *done = TRUE;
  cf->connected = TRUE;
  परिणाम = CURLE_OK;

out:
  CURL_TRC_CF(तथ्य, cf, "cf_connect() -> %d, %d, ", (पूर्णांक)परिणाम, *done);
  CF_तथ्य_RESTORE(cf, save);
  वापस परिणाम;
}

अटल व्योम cf_h2_destroy(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;

  (व्योम)तथ्य;
  अगर(ctx) {
    cf_h2_ctx_free(ctx);
    cf->ctx = शून्य;
  }
}

अटल CURLcode cf_h2_shutdown(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य, bool *done)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  काष्ठा cf_call_तथ्य save;
  CURLcode परिणाम;
  पूर्णांक rv;

  अगर(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  CF_तथ्य_SAVE(save, cf, तथ्य);

  अगर(!ctx->sent_goaway) {
    ctx->sent_goaway = TRUE;
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, ctx->local_max_sid, 0,
      (स्थिर uint8_t *)"shutdown", माप("shutdown"));
    अगर(rv) {
      failf(तथ्य, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      परिणाम = CURLE_SEND_ERROR;
      जाओ out;
    }
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  परिणाम = CURLE_OK;
  अगर(Curl_ngh2_session_want_write(&ctx->ngh2) ||
     !Curl_bufq_is_empty(&ctx->outbufq))
    परिणाम = h2_progress_egress(cf, तथ्य);
  अगर(!परिणाम && Curl_ngh2_session_want_read(&ctx->ngh2))
    परिणाम = h2_progress_ingress(cf, तथ्य, 0);

  अगर(परिणाम == CURLE_AGAIN)
    परिणाम = CURLE_OK;

  *done = (ctx->conn_closed ||
           (!परिणाम && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2) &&
            Curl_bufq_is_empty(&ctx->outbufq)));

out:
  CF_तथ्य_RESTORE(cf, save);
  cf->shutdown = (परिणाम || *done);
  वापस परिणाम;
}

अटल CURLcode http2_तथ्य_pause(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 bool छोड़ो)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  काष्ठा h2_stream_ctx *stream = H2_STREAM_CTX(ctx, तथ्य);

  DEBUGASSERT(तथ्य);
  अगर(ctx && Curl_ngh2_valid(&ctx->ngh2) && stream) {
    CURLcode परिणाम;

    stream->write_paused = छोड़ो;
    परिणाम = cf_h2_update_local_win(cf, तथ्य, stream);
    अगर(परिणाम)
      वापस परिणाम;

    /* attempt to send the window update */
    (व्योम)h2_progress_egress(cf, तथ्य);

    अगर(!छोड़ो) {
      /* Unpausing a h2 transfer, requires it to be run again. The server
       * may send new DATA on us increasing the flow window, and it may
       * not. We may have already buffered and exhausted the new window
       * by operating on things in flight during the handling of other
       * transfers. */
      Curl_multi_mark_dirty(तथ्य);
    }
    CURL_TRC_CF(तथ्य, cf, "[%d] stream now %spaused", stream->id,
                छोड़ो ? "" : "un");
  }
  वापस CURLE_OK;
}

अटल CURLcode cf_h2_cntrl(काष्ठा Curl_cfilter *cf,
                            काष्ठा Curl_easy *तथ्य,
                            पूर्णांक event, पूर्णांक arg1, व्योम *arg2)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा cf_call_तथ्य save;

  (व्योम)arg2;

  CF_तथ्य_SAVE(save, cf, तथ्य);
  चयन(event) {
  हाल CF_CTRL_तथ्य_SETUP:
    अवरोध;
  हाल CF_CTRL_तथ्य_PAUSE:
    परिणाम = http2_तथ्य_pause(cf, तथ्य, (arg1 != 0));
    अवरोध;
  हाल CF_CTRL_FLUSH:
    परिणाम = cf_h2_flush(cf, तथ्य);
    अवरोध;
  हाल CF_CTRL_तथ्य_DONE:
    http2_तथ्य_done(cf, तथ्य);
    अवरोध;
  हाल CF_CTRL_CONN_INFO_UPDATE:
    अगर(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 20;
      Curl_conn_set_multiplex(cf->conn);
    }
    अवरोध;
  शेष:
    अवरोध;
  }
  CF_तथ्य_RESTORE(cf, save);
  वापस परिणाम;
}

अटल bool cf_h2_तथ्य_pending(काष्ठा Curl_cfilter *cf,
                               स्थिर काष्ठा Curl_easy *तथ्य)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;

  अगर(ctx && !Curl_bufq_is_empty(&ctx->inbufq))
    वापस TRUE;
  वापस cf->अग्र ? cf->अग्र->cft->has_तथ्य_pending(cf->अग्र, तथ्य) : FALSE;
}

अटल bool cf_h2_is_alive(काष्ठा Curl_cfilter *cf,
                           काष्ठा Curl_easy *तथ्य,
                           bool *निवेश_pending)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  bool alive;
  काष्ठा cf_call_तथ्य save;

  *निवेश_pending = FALSE;
  CF_तथ्य_SAVE(save, cf, तथ्य);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
           http2_connisalive(cf, तथ्य, निवेश_pending));
  CURL_TRC_CF(तथ्य, cf, "conn alive -> %d, input_pending=%d",
              alive, *निवेश_pending);
  CF_तथ्य_RESTORE(cf, save);
  वापस alive;
}

अटल CURLcode cf_h2_keep_alive(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य)
{
  CURLcode परिणाम;
  काष्ठा cf_call_तथ्य save;

  CF_तथ्य_SAVE(save, cf, तथ्य);
  परिणाम = http2_send_ping(cf, तथ्य);
  CF_तथ्य_RESTORE(cf, save);
  वापस परिणाम;
}

अटल CURLcode cf_h2_query(काष्ठा Curl_cfilter *cf,
                            काष्ठा Curl_easy *तथ्य,
                            पूर्णांक query, पूर्णांक *pres1, व्योम *pres2)
{
  काष्ठा cf_h2_ctx *ctx = cf->ctx;
  काष्ठा cf_call_तथ्य save;
  माप_प्रकार effective_max;

  चयन(query) {
  हाल CF_QUERY_MAX_CONCURRENT:
    DEBUGASSERT(pres1);

    CF_तथ्य_SAVE(save, cf, तथ्य);
    अगर(!Curl_ngh2_valid(&ctx->ngh2) ||
       !Curl_ngh2_session_check_request_allowed(&ctx->ngh2)) {
      /* the limit is what we have in use right now */
      effective_max = cf->conn->attached_xfers;
    }
    अन्यथा {
      effective_max = ctx->max_concurrent_streams;
    }
    *pres1 = (effective_max > पूर्णांक_उच्च) ? पूर्णांक_उच्च : (पूर्णांक)effective_max;
    CF_तथ्य_RESTORE(cf, save);
    वापस CURLE_OK;
  हाल CF_QUERY_STREAM_ERROR: {
    काष्ठा h2_stream_ctx *stream = H2_STREAM_CTX(ctx, तथ्य);
    *pres1 = stream ? (पूर्णांक)stream->error : 0;
    वापस CURLE_OK;
  }
  हाल CF_QUERY_NEED_FLUSH: {
    काष्ठा h2_stream_ctx *stream = H2_STREAM_CTX(ctx, तथ्य);
    अगर(!Curl_bufq_is_empty(&ctx->outbufq) ||
       (stream && !Curl_bufq_is_empty(&stream->sendbuf))) {
      *pres1 = TRUE;
      वापस CURLE_OK;
    }
    अवरोध;
  }
  हाल CF_QUERY_HTTP_VERSION:
    *pres1 = 20;
    वापस CURLE_OK;
  शेष:
    अवरोध;
  }
  वापस cf->अग्र ?
    cf->अग्र->cft->query(cf->अग्र, तथ्य, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

काष्ठा Curl_cftype Curl_cft_nghttp2 = {
  "HTTP/2",
  CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  CURL_LOG_LVL_NONE,
  cf_h2_destroy,
  cf_h2_connect,
  cf_h2_shutdown,
  cf_h2_adjust_pollset,
  cf_h2_तथ्य_pending,
  cf_h2_send,
  cf_h2_recv,
  cf_h2_cntrl,
  cf_h2_is_alive,
  cf_h2_keep_alive,
  cf_h2_query,
};

अटल CURLcode http2_cfilter_add(काष्ठा Curl_cfilter **pcf,
                                  काष्ठा Curl_easy *तथ्य,
                                  काष्ठा connectdata *conn,
                                  int8_t sockindex,
                                  bool via_h1_upgrade)
{
  काष्ठा Curl_cfilter *cf = शून्य;
  काष्ठा cf_h2_ctx *ctx;
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(तथ्य->conn);
  ctx = curlx_calloc(1, माप(*ctx));
  अगर(!ctx)
    जाओ out;
  cf_h2_ctx_init(ctx, via_h1_upgrade);

  परिणाम = Curl_cf_create(&cf, &Curl_cft_nghttp2, ctx);
  अगर(परिणाम)
    जाओ out;

  ctx = शून्य;
  Curl_conn_cf_add(तथ्य, conn, sockindex, cf);

out:
  अगर(परिणाम)
    cf_h2_ctx_free(ctx);
  *pcf = परिणाम ? शून्य : cf;
  वापस परिणाम;
}

अटल CURLcode http2_cfilter_जोड़ो_after(काष्ठा Curl_cfilter *cf,
                                           काष्ठा Curl_easy *तथ्य,
                                           bool via_h1_upgrade)
{
  काष्ठा Curl_cfilter *cf_h2 = शून्य;
  काष्ठा cf_h2_ctx *ctx;
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;

  (व्योम)तथ्य;
  ctx = curlx_calloc(1, माप(*ctx));
  अगर(!ctx)
    जाओ out;
  cf_h2_ctx_init(ctx, via_h1_upgrade);

  परिणाम = Curl_cf_create(&cf_h2, &Curl_cft_nghttp2, ctx);
  अगर(परिणाम)
    जाओ out;

  ctx = शून्य;
  Curl_conn_cf_जोड़ो_after(cf, cf_h2);

out:
  अगर(परिणाम)
    cf_h2_ctx_free(ctx);
  वापस परिणाम;
}

bool Curl_http2_may_switch(काष्ठा Curl_easy *तथ्य)
{
  अगर(Curl_conn_http_version(तथ्य, तथ्य->conn) < 20 &&
     (तथ्य->state.http_neg.wanted & CURL_HTTP_V2x) &&
     तथ्य->state.http_neg.h2_prior_knowledge) {
#अगर_अघोषित CURL_DISABLE_PROXY
    अगर(तथ्य->conn->bits.origin_is_proxy) {
      infof(तथ्य, "Ignoring HTTP/2 prior knowledge due to proxy");
      वापस FALSE;
    }
#पूर्ण_अगर
    वापस TRUE;
  }
  वापस FALSE;
}

CURLcode Curl_http2_switch(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_cfilter *cf;
  CURLcode परिणाम;

  DEBUGASSERT(Curl_conn_http_version(तथ्य, तथ्य->conn) < 20);

  परिणाम = http2_cfilter_add(&cf, तथ्य, तथ्य->conn, FIRSTSOCKET, FALSE);
  अगर(परिणाम)
    वापस परिणाम;
  CURL_TRC_CF(तथ्य, cf, "switching connection to HTTP/2");

  अगर(cf->अग्र) {
    bool done;
    वापस Curl_conn_cf_connect(cf, तथ्य, &done);
  }
  वापस CURLE_OK;
}

CURLcode Curl_http2_switch_at(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_cfilter *cf_h2;
  CURLcode परिणाम;

  DEBUGASSERT(Curl_conn_http_version(तथ्य, तथ्य->conn) < 20);

  परिणाम = http2_cfilter_जोड़ो_after(cf, तथ्य, FALSE);
  अगर(परिणाम)
    वापस परिणाम;

  cf_h2 = cf->अग्र;

  अगर(cf_h2->अग्र) {
    bool done;
    वापस Curl_conn_cf_connect(cf_h2, तथ्य, &done);
  }
  वापस CURLE_OK;
}

CURLcode Curl_http2_upgrade(काष्ठा Curl_easy *तथ्य,
                            काष्ठा connectdata *conn, int8_t sockindex,
                            स्थिर अक्षर *mem, माप_प्रकार nread)
{
  काष्ठा Curl_cfilter *cf;
  काष्ठा cf_h2_ctx *ctx;
  CURLcode परिणाम;

  DEBUGASSERT(Curl_conn_http_version(तथ्य, conn) < 20);

  परिणाम = http2_cfilter_add(&cf, तथ्य, conn, sockindex, TRUE);
  अगर(परिणाम)
    वापस परिणाम;
  CURL_TRC_CF(तथ्य, cf, "upgrading connection to HTTP/2");

  DEBUGASSERT(cf->cft == &Curl_cft_nghttp2);
  ctx = cf->ctx;

  तथ्य->req.httpversion_sent = 20; /* it is an h2 request now */
  तथ्य->req.header = TRUE; /* we expect the real response to come in h2 */
  तथ्य->req.headerline = 0; /* restart the header line counter */

  अगर(nread > 0) {
    /* Remaining data from the protocol switch reply is already using
     * the switched protocol, ie. HTTP/2. We add that to the network
     * inbufq. */
    माप_प्रकार copied;

    परिणाम = Curl_bufq_write(&ctx->inbufq,
                             (स्थिर अचिन्हित अक्षर *)mem, nread, &copied);
    अगर(परिणाम) {
      failf(तथ्य, "error on copying HTTP Upgrade response: %d", (पूर्णांक)परिणाम);
      वापस CURLE_RECV_ERROR;
    }
    अगर(copied < nread) {
      failf(तथ्य, "connection buffer size could not take all data "
            "from HTTP Upgrade response header: copied=%zu, datalen=%zu",
            copied, nread);
      वापस CURLE_HTTP2;
    }
    infof(तथ्य, "Copied HTTP/2 data in stream buffer to connection buffer"
          " after upgrade: len=%zu", nread);
  }

  अगर(cf->अग्र) {
    bool done;
    परिणाम = Curl_conn_cf_connect(cf, तथ्य, &done);
    अगर(!परिणाम)
      cf->cft->cntrl(cf, तथ्य, CF_CTRL_CONN_INFO_UPDATE, 0, शून्य);
  }
  वापस परिणाम;
}

/* Only call this function for a transfer that already got an HTTP/2
   CURLE_HTTP2_STREAM error! */
bool Curl_h2_http_1_1_error(काष्ठा Curl_easy *तथ्य)
{
  अगर(Curl_conn_http_version(तथ्य, तथ्य->conn) == 20) {
    पूर्णांक err = Curl_conn_get_stream_error(तथ्य, तथ्य->conn, FIRSTSOCKET);
    वापस err == NGHTTP2_HTTP_1_1_REQUIRED;
  }
  वापस FALSE;
}

व्योम *Curl_nghttp2_malloc(माप_प्रकार आकार, व्योम *user_तथ्य)
{
  (व्योम)user_तथ्य;
  वापस Curl_cmalloc(आकार);
}

व्योम Curl_nghttp2_free(व्योम *ptr, व्योम *user_तथ्य)
{
  (व्योम)user_तथ्य;
  Curl_cfree(ptr);
}

व्योम *Curl_nghttp2_calloc(माप_प्रकार nmemb, माप_प्रकार आकार, व्योम *user_तथ्य)
{
  (व्योम)user_तथ्य;
  वापस Curl_ccalloc(nmemb, आकार);
}

व्योम *Curl_nghttp2_realloc(व्योम *ptr, माप_प्रकार आकार, व्योम *user_तथ्य)
{
  (व्योम)user_तथ्य;
  वापस Curl_crealloc(ptr, आकार);
}

#अन्यथा /* CURL_DISABLE_HTTP || !USE_NGHTTP2 */

/* Satisfy external references even if http2 is not compiled in. */

अक्षर *curl_pushheader_bynum(काष्ठा curl_pushheaders *h, माप_प्रकार अंक)
{
  (व्योम)h;
  (व्योम)अंक;
  वापस शून्य;
}

अक्षर *curl_pushheader_byname(काष्ठा curl_pushheaders *h, स्थिर अक्षर *name)
{
  (व्योम)h;
  (व्योम)name;
  वापस शून्य;
}

#पूर्ण_अगर /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
