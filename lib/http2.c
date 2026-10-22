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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#అన్తర్భావ <nghttp2/nghttp2.h>

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "bufq.h"
#అన్తర్భావ "u32_ptrset.h"
#అన్తర్భావ "http1.h"
#అన్తర్భావ "http2.h"
#అన్తర్భావ "http.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "curlx/base64.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "ngh2_shim.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "urlapi-int.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "bufref.h"
#అన్తర్భావ "curlx/dynbuf.h"
#అన్తర్భావ "headers.h"
#అన్తర్భావ "curl_share.h"

#ఒకవేళ NGHTTP2_సంస్కరణ_అంకె < 0x010f00
#దోషం "nghttp2 1.15.0 or greater required"
#అయితే_ముగింపు

#నిర్వచించు NGHTTP2_HAS_సమితి_LOCAL_కిటికీ_కొలత 1

/* buffer dimensioning:
 * use 16K as chunk size, as that fits H2 DATA frames well */
#నిర్వచించు H2_CHUNK_కొలత           (16 * 1024)
/* connection window size */
#నిర్వచించు H2_CONN_కిటికీ_కొలత     (10 * 1024 * 1024)
/* on receiving from TLS, we prep for holding a full stream window */
#నిర్వచించు H2_NW_RECV_CHUNKS       (H2_CONN_కిటికీ_కొలత / H2_CHUNK_కొలత)
/* on send into TLS, we want to accumulate small frames */
#నిర్వచించు H2_NW_SEND_CHUNKS       1
/* this is how much we want "in flight" for a stream, unthrottled */
#నిర్వచించు H2_ప్రవాహం_కిటికీ_కొలత_MAX   (10 * 1024 * 1024)
/* this is how much we want "in flight" for a stream, initially, IFF
 * nghttp2 allows us to tweak the local window size. */
#ఒకవేళ NGHTTP2_HAS_సమితి_LOCAL_కిటికీ_కొలత
#నిర్వచించు H2_ప్రవాహం_కిటికీ_కొలత_INITIAL  (64 * 1024)
#లేకపోతే
#నిర్వచించు H2_ప్రవాహం_కిటికీ_కొలత_INITIAL H2_ప్రవాహం_కిటికీ_కొలత_MAX
#అయితే_ముగింపు
/* keep smaller stream upload buffer (default h2 window size) to have
 * our progress bars and "upload done" reporting closer to reality */
#నిర్వచించు H2_ప్రవాహం_SEND_CHUNKS   ((64 * 1024) / H2_CHUNK_కొలత)
/* spare chunks we keep for a full window */
#నిర్వచించు H2_ప్రవాహం_POOL_SPARES   (H2_CONN_కిటికీ_కొలత / H2_CHUNK_కొలత)

/* We need to accommodate the max number of streams with their window sizes on
 * the overall connection. Streams might become PAUSED which will block their
 * received QUOTA in the connection window. If we run out of space, the server
 * is blocked from sending us any data. See #10988 for an issue with this. */
#నిర్వచించు HTTP2_HUGE_కిటికీ_కొలత (100 * H2_ప్రవాహం_కిటికీ_కొలత_MAX)

#నిర్వచించు H2_SETTINGS_IV_LEN 3
#నిర్వచించు H2_BINSETTINGS_LEN 80

నిర్మాణం cf_h2_ctx {
  నిర్మాణం curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  నిర్మాణం cf_call_దత్తాంశం call_దత్తాంశం;

  నిర్మాణం bufq inbufq;           /* network input */
  నిర్మాణం bufq outbufq;          /* network output */
  నిర్మాణం bufc_pool ప్రవాహం_bufcp; /* spares for stream buffers */
  నిర్మాణం dynbuf scratch;        /* scratch buffer for temp use */

  నిర్మాణం u32_ptrset streams; /* hash of `data->mid` to `h2_stream_ctx` */
  size_t drain_మొత్తం; /* sum of all stream's UrlState drain */
  uint32_t initial_win_కొలత; /* current initial window size (settings) */
  uint32_t max_concurrent_streams;
  uint32_t goaway_error;        /* goaway error code from server */
  int32_t remote_max_sid;       /* max id processed by server */
  int32_t local_max_sid;        /* max id processed by us */
  బిట్టు(initialized);
  బిట్టు(ద్వారా_h1_upgrade);
  బిట్టు(conn_closed);
  బిట్టు(rcvd_goaway);
  బిట్టు(పంపిన_goaway);
  బిట్టు(enable_నెట్టు);
  బిట్టు(nw_బహిర్గతం_blocked);
};

/* How to access `call_data` from a cf_h2 filter */
#నిర్వచనం_రద్దు CF_CTX_CALL_దత్తాంశం
#నిర్వచించు CF_CTX_CALL_దత్తాంశం(cf) ((నిర్మాణం cf_h2_ctx *)(cf)->ctx)->call_దత్తాంశం

/**
 * All about the H2 internals of a stream
 */
నిర్మాణం h2_ప్రవాహం_ctx {
  నిర్మాణం bufq sendbuf; /* request buffer */
  నిర్మాణం h1_req_parser h1; /* parsing the request */
  నిర్మాణం dynhds resp_trailers; /* response trailer fields */
  size_t resp_hds_len; /* amount of response header bytes in recvbuf */
  curl_off_t xfer_id;  /* transfer `id` owning this stream */
  curl_off_t nrcvd_దత్తాంశం;  /* number of DATA bytes received */

  అక్షరం **నెట్టు_శీర్షికలు;       /* allocated array */
  size_t నెట్టు_శీర్షికలు_వాడిన;  /* number of entries filled in */
  size_t నెట్టు_శీర్షికలు_alloc; /* number of entries allocated */

  int32_t id; /* HTTP/2 protocol identifier for stream */
  uint32_t mid; /* transfer `mid` owning this stream */
  uint32_t error; /* stream error code */
  CURLcode xfer_ఫలితం; /* Result of writing out response */
  int32_t local_కిటికీ_కొలత; /* the local recv window size */
  int32_t స్థితి_సంకేతం; /* HTTP response status code */
  బిట్టు(resp_hds_complete); /* we have a complete, final response */
  బిట్టు(closed); /* TRUE on stream close */
  బిట్టు(reset);  /* TRUE on stream reset */
  బిట్టు(reset_by_server);  /* TRUE on stream reset by server */
  బిట్టు(close_handled); /* TRUE if stream closure is handled by libcurl */
  బిట్టు(bodystarted);
  బిట్టు(body_eos);    /* the complete body has been added to `sendbuf` and
                     * is being/has been processed from there. */
  బిట్టు(write_paused);  /* stream write is paused */
};

స్థిర శూన్య free_నెట్టు_శీర్షికలు(నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం)
{
  size_t i;
  కొరకు(i = 0; i < ప్రవాహం->నెట్టు_శీర్షికలు_వాడిన; i++)
    curlx_free(ప్రవాహం->నెట్టు_శీర్షికలు[i]);
  curlx_safefree(ప్రవాహం->నెట్టు_శీర్షికలు);
  ప్రవాహం->నెట్టు_శీర్షికలు_వాడిన = 0;
}

స్థిర శూన్య h2_ప్రవాహం_ctx_free(నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం)
{
  Curl_bufq_free(&ప్రవాహం->sendbuf);
  Curl_h1_req_parse_free(&ప్రవాహం->h1);
  Curl_dynhds_free(&ప్రవాహం->resp_trailers);
  free_నెట్టు_శీర్షికలు(ప్రవాహం);
  curlx_free(ప్రవాహం);
}

స్థిర శూన్య h2_ప్రవాహం_hash_free(uint32_t id, శూన్య *ప్రవాహం)
{
  (శూన్య)id;
  DEBUGASSERT(ప్రవాహం);
  h2_ప్రవాహం_ctx_free((నిర్మాణం h2_ప్రవాహం_ctx *)ప్రవాహం);
}

స్థిర శూన్య cf_h2_ctx_init(నిర్మాణం cf_h2_ctx *ctx, bool ద్వారా_h1_upgrade)
{
  Curl_bufcp_init(&ctx->ప్రవాహం_bufcp, H2_CHUNK_కొలత, H2_ప్రవాహం_POOL_SPARES);
  Curl_bufq_initp(&ctx->inbufq, &ctx->ప్రవాహం_bufcp, H2_NW_RECV_CHUNKS, 0);
  Curl_bufq_initp(&ctx->outbufq, &ctx->ప్రవాహం_bufcp, H2_NW_SEND_CHUNKS, 0);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_శీర్షిక);
  Curl_u32_ptrset_init(&ctx->streams, h2_ప్రవాహం_hash_free);
  ctx->remote_max_sid = INT32_MAX;
  ctx->ద్వారా_h1_upgrade = ద్వారా_h1_upgrade;
  ctx->initialized = TRUE;
}

స్థిర శూన్య cf_h2_ctx_free(నిర్మాణం cf_h2_ctx *ctx)
{
  ఒకవేళ(ctx && ctx->initialized) {
    Curl_ngh2_del(&ctx->ngh2);
    Curl_bufq_free(&ctx->inbufq);
    Curl_bufq_free(&ctx->outbufq);
    Curl_bufcp_free(&ctx->ప్రవాహం_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    memset(ctx, 0, పరిమాణం(*ctx));
  }
  curlx_free(ctx);
}

స్థిర uint32_t cf_h2_initial_win_కొలత(నిర్మాణం Curl_easy *దత్తాంశం)
{
#ఒకవేళ NGHTTP2_HAS_సమితి_LOCAL_కిటికీ_కొలత
  /* If the transfer has a rate-limit lower than the default initial
   * stream window size, use that. It needs to be at least 8k or servers
   * may be unhappy. */
  curl_off_t rps = Curl_rlimit_చొప్పున_అడుగు(&దత్తాంశం->పురోగతి.dl.rlimit);
  ఒకవేళ((rps > 0) && (rps < H2_ప్రవాహం_కిటికీ_కొలత_INITIAL))
    తిరిగి CURLMAX((uint32_t)rps, 8192);
#అయితే_ముగింపు
  తిరిగి H2_ప్రవాహం_కిటికీ_కొలత_INITIAL;
}

స్థిర size_t populate_settings(nghttp2_settings_నమోదు *iv,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం cf_h2_ctx *ctx)
{
  iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
  iv[0].విలువ = Curl_multi_max_concurrent_streams(దత్తాంశం->multi);

  iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_కిటికీ_కొలత;
  iv[1].విలువ = cf_h2_initial_win_కొలత(దత్తాంశం);
  ఒకవేళ(ctx)
    ctx->initial_win_కొలత = iv[1].విలువ;
  iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_నెట్టు;
  iv[2].విలువ = !!దత్తాంశం->multi->నెట్టు_cb;

  తిరిగి 3;
}

స్థిర ssize_t populate_binsettings(uint8_t *binsettings,
                                    నిర్మాణం Curl_easy *దత్తాంశం)
{
  nghttp2_settings_నమోదు iv[H2_SETTINGS_IV_LEN];
  size_t ivlen;

  ivlen = populate_settings(iv, దత్తాంశం, ఖాళీ);
  /* this returns number of bytes it wrote or a negative number on error. */
  తిరిగి nghttp2_pack_settings_payload(binsettings, H2_BINSETTINGS_LEN,
                                       iv, ivlen);
}

స్థిర CURLcode cf_h2_update_settings(నిర్మాణం cf_h2_ctx *ctx,
                                      uint32_t initial_win_కొలత)
{
  nghttp2_settings_నమోదు నమోదు_2;
  నమోదు_2.settings_id = NGHTTP2_SETTINGS_INITIAL_కిటికీ_కొలత;
  నమోదు_2.విలువ = initial_win_కొలత;
  ఒకవేళ(Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, &నమోదు_2, 1))
    తిరిగి CURLE_SEND_ERROR;
  ctx->initial_win_కొలత = initial_win_కొలత;
  తిరిగి CURLE_OK;
}

#నిర్వచించు H2_ప్రవాహం_CTX(ctx, దత్తాంశం)                                         \
  ((నిర్మాణం h2_ప్రవాహం_ctx *)(                                             \
    (దత్తాంశం) ? Curl_u32_ptrset_get(&(ctx)->streams, (దత్తాంశం)->mid) : ఖాళీ))

స్థిర నిర్మాణం h2_ప్రవాహం_ctx *h2_ప్రవాహం_ctx_create(నిర్మాణం cf_h2_ctx *ctx,
                                                  నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం;

  ప్రవాహం = curlx_calloc(1, పరిమాణం(*ప్రవాహం));
  ఒకవేళ(!ప్రవాహం)
    తిరిగి ఖాళీ;

  ప్రవాహం->id = -1;
  ప్రవాహం->xfer_id = దత్తాంశం->id;
  ప్రవాహం->mid = దత్తాంశం->mid;
  Curl_bufq_initp(&ప్రవాహం->sendbuf, &ctx->ప్రవాహం_bufcp,
                  H2_ప్రవాహం_SEND_CHUNKS, BUFQ_OPT_NONE);
  Curl_h1_req_parse_init(&ప్రవాహం->h1, H1_PARSE_DEFAULT_MAX_పంక్తి_LEN);
  Curl_dynhds_init(&ప్రవాహం->resp_trailers,
                   MAX_HTTP_RESP_శీర్షిక_లెక్క, DYN_HTTP_REQUEST);
  ప్రవాహం->bodystarted = FALSE;
  ప్రవాహం->స్థితి_సంకేతం = -1;
  ప్రవాహం->closed = FALSE;
  ప్రవాహం->close_handled = FALSE;
  ప్రవాహం->error = NGHTTP2_NO_ERROR;
  ప్రవాహం->local_కిటికీ_కొలత = H2_ప్రవాహం_కిటికీ_కొలత_INITIAL;
  ప్రవాహం->nrcvd_దత్తాంశం = 0;
  తిరిగి ప్రవాహం;
}

స్థిర నిర్మాణం Curl_easy *cf_h2_get_ప్రవాహం_xfer(నిర్మాణం Curl_cfilter *cf,
                                               int32_t ప్రవాహం_id,
                                               నిర్మాణం h2_ప్రవాహం_ctx **pstream)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం =
    Curl_ngh2_session_get_ప్రవాహం_user_దత్తాంశం(&ctx->ngh2, ప్రవాహం_id);
  ఒకవేళ(pstream)
    *pstream = ప్రవాహం;
  ఒకవేళ(ప్రవాహం && (ప్రవాహం->mid != UINT32_MAX)) {
    నిర్మాణం Curl_easy *దత్తాంశం, *call_దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
    ఒకవేళ(!call_దత్తాంశం || !call_దత్తాంశం->multi) {
      DEBUGASSERT(0);
      తిరిగి ఖాళీ;
    }
    దత్తాంశం = Curl_multi_get_easy(call_దత్తాంశం->multi, ప్రవాహం->mid);
    ఒకవేళ(దత్తాంశం && (దత్తాంశం->id == ప్రవాహం->xfer_id))
      తిరిగి దత్తాంశం;
  }
  తిరిగి ఖాళీ;
}

#నిర్వచితమైతే NGHTTP2_HAS_సమితి_LOCAL_కిటికీ_కొలత
స్థిర int32_t cf_h2_get_desired_local_win(నిర్మాణం Curl_cfilter *cf,
                                           నిర్మాణం Curl_easy *దత్తాంశం)
{
  curl_off_t avail = Curl_rlimit_avail(&దత్తాంశం->పురోగతి.dl.rlimit, ఖాళీ);

  (శూన్య)cf;
  ఒకవేళ(avail < CURL_OFF_T_MAX) { /* limit in place */
    ఒకవేళ(avail <= 0)
      తిరిగి 0;
    లేకపోతే ఒకవేళ(avail < H2_ప్రవాహం_కిటికీ_కొలత_MAX)
      తిరిగి (int32_t)avail;
  }
  తిరిగి H2_ప్రవాహం_కిటికీ_కొలత_MAX;
}

స్థిర CURLcode cf_h2_update_local_win(నిర్మాణం Curl_cfilter *cf,
                                       నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  int32_t dwsize;
  పూర్ణాంకం rv;

  dwsize = (ప్రవాహం->write_paused || ప్రవాహం->xfer_ఫలితం) ?
           0 : cf_h2_get_desired_local_win(cf, దత్తాంశం);
  ఒకవేళ(dwsize != ప్రవాహం->local_కిటికీ_కొలత) {
    int32_t wsize = Curl_ngh2_session_get_ప్రవాహం_effective_local_కిటికీ_కొలత(
      &ctx->ngh2, ప్రవాహం->id);
    ఒకవేళ(dwsize > wsize) {
      rv = Curl_ngh2_session_సమితి_local_కిటికీ_కొలత(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, ప్రవాహం->id, dwsize);
      ఒకవేళ(rv) {
        failf(దత్తాంశం, "[%d] nghttp2 set_local_window_size(%d) failed: "
              "%s(%d)", ప్రవాహం->id, dwsize, nghttp2_strerror(rv), rv);
        తిరిగి CURLE_HTTP2;
      }
      rv = Curl_ngh2_submit_కిటికీ_update(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, ప్రవాహం->id, dwsize - wsize);
      ఒకవేళ(rv) {
        failf(దత్తాంశం, "[%d] nghttp2_submit_window_update() failed: "
              "%s(%d)", ప్రవాహం->id, nghttp2_strerror(rv), rv);
        తిరిగి CURLE_HTTP2;
      }
      ప్రవాహం->local_కిటికీ_కొలత = dwsize;
      CURL_TRC_CF(దత్తాంశం, cf, "[%d] local window update by %d",
                  ప్రవాహం->id, dwsize - wsize);
    }
    లేకపోతే {
      rv = Curl_ngh2_session_సమితి_local_కిటికీ_కొలత(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, ప్రవాహం->id, dwsize);
      ఒకవేళ(rv) {
        failf(దత్తాంశం, "[%d] nghttp2_session_set_local_window_size() failed: "
              "%s(%d)", ప్రవాహం->id, nghttp2_strerror(rv), rv);
        తిరిగి CURLE_HTTP2;
      }
      ప్రవాహం->local_కిటికీ_కొలత = dwsize;
      CURL_TRC_CF(దత్తాంశం, cf, "[%d] local window size now %d",
                  ప్రవాహం->id, dwsize);
    }
  }
  తిరిగి CURLE_OK;
}

#లేకపోతే /* NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

స్థిర CURLcode cf_h2_update_local_win(నిర్మాణం Curl_cfilter *cf,
                                       నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం)
{
  (శూన్య)cf;
  (శూన్య)దత్తాంశం;
  (శూన్య)ప్రవాహం;
  తిరిగి CURLE_OK;
}
#అయితే_ముగింపు /* !NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

స్థిర CURLcode http2_దత్తాంశం_అమరిక(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం h2_ప్రవాహం_ctx **pstream)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం;

  (శూన్య)cf;
  DEBUGASSERT(దత్తాంశం);

  ఒకవేళ(!దత్తాంశం)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  ప్రవాహం = H2_ప్రవాహం_CTX(ctx, దత్తాంశం);
  ఒకవేళ(ప్రవాహం) {
    *pstream = ప్రవాహం;
    తిరిగి CURLE_OK;
  }

  ప్రవాహం = h2_ప్రవాహం_ctx_create(ctx, దత్తాంశం);
  ఒకవేళ(!ప్రవాహం)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ఒకవేళ(Curl_u32_ptrset_సమితి(&ctx->streams, దత్తాంశం->mid, ప్రవాహం)) {
    h2_ప్రవాహం_ctx_free(ప్రవాహం);
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  *pstream = ప్రవాహం;
  తిరిగి CURLE_OK;
}

స్థిర CURLcode nw_బహిర్గతం_flush(నిర్మాణం Curl_cfilter *cf,
                             నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode ఫలితం;

  ఒకవేళ(Curl_bufq_is_ఖాళీలు(&ctx->outbufq))
    తిరిగి CURLE_OK;

  ఫలితం = Curl_cf_send_bufq(cf->తదుపరి, దత్తాంశం, &ctx->outbufq, ఖాళీ, 0,
                             &nwritten);
  ఒకవేళ(ఫలితం) {
    ఒకవేళ(ఫలితం == CURLE_AGAIN) {
      CURL_TRC_CF(దత్తాంశం, cf, "flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_బహిర్గతం_blocked = 1;
    }
    తిరిగి ఫలితం;
  }
  తిరిగి Curl_bufq_is_ఖాళీలు(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

స్థిర శూన్య http2_దత్తాంశం_పూర్తి(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం = H2_ప్రవాహం_CTX(ctx, దత్తాంశం);

  DEBUGASSERT(ctx);
  ఒకవేళ(!ప్రవాహం || !ctx->initialized)
    తిరిగి;

  ఒకవేళ(Curl_ngh2_valid(&ctx->ngh2)) {
    bool flush_egress = FALSE;
    /* returns error if stream not known, which is fine here */
    (శూన్య)Curl_ngh2_session_సమితి_ప్రవాహం_user_దత్తాంశం(&ctx->ngh2, ప్రవాహం->id, ఖాళీ);

    ఒకవేళ(!ప్రవాహం->closed && ప్రవాహం->id > 0) {
      /* RST_STREAM */
      CURL_TRC_CF(దత్తాంశం, cf, "[%d] premature DATA_DONE, RST stream",
                  ప్రవాహం->id);
      ప్రవాహం->closed = TRUE;
      ప్రవాహం->reset = TRUE;
      Curl_ngh2_submit_rst_ప్రవాహం(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                  ప్రవాహం->id, NGHTTP2_ప్రవాహం_CLOSED);
      flush_egress = TRUE;
    }

    ఒకవేళ(flush_egress) {
      (శూన్య)Curl_ngh2_session_send(&ctx->ngh2);
      (శూన్య)nw_బహిర్గతం_flush(cf, దత్తాంశం);
    }
  }

  Curl_u32_ptrset_unset(&ctx->streams, దత్తాంశం->mid);
}

స్థిర పూర్ణాంకం h2_client_new(నిర్మాణం Curl_cfilter *cf,
                         nghttp2_session_callbacks *cbs)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { ఖాళీ, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  పూర్ణాంకం rc = nghttp2_option_new(&o);
  ఒకవేళ(rc)
    తిరిగి rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_సమితి_no_auto_కిటికీ_update(o, 1);
#ఒకవేళ NGHTTP2_సంస్కరణ_అంకె >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_సమితి_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#అయితే_ముగింపు
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  తిరిగి rc;
}

/*
 * Returns nonzero if current HTTP/2 session should be closed.
 */
స్థిర పూర్ణాంకం should_close_session(నిర్మాణం cf_h2_ctx *ctx)
{
  తిరిగి ctx->drain_మొత్తం == 0 && !Curl_ngh2_session_want_read(&ctx->ngh2) &&
    !Curl_ngh2_session_want_write(&ctx->ngh2);
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
స్థిర CURLcode h2_process_pending_ఆదానం(నిర్మాణం Curl_cfilter *cf,
                                         నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  స్థిరరాశి గుర్తులేని అక్షరం *buf;
  size_t blen, nread;
  ssize_t rv;

  అయితే(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {
    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (స్థిరరాశి uint8_t *)buf, blen);
    ఒకవేళ(!curlx_sztouz(rv, &nread)) {
      failf(దత్తాంశం, "nghttp2 recv error %zd: %s", rv, nghttp2_strerror((పూర్ణాంకం)rv));
      తిరిగి CURLE_RECV_ERROR;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    ఒకవేళ(Curl_bufq_is_ఖాళీలు(&ctx->inbufq)) {
      ఆపు;
    }
    లేకపోతే {
      CURL_TRC_CF(దత్తాంశం, cf, "process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }

  ఒకవేళ(Curl_ngh2_session_check_request_allowed(&ctx->ngh2) == 0) {
    /* No more requests are allowed in the current session, so
       the connection may not be reused. This is set when a
       GOAWAY frame has been received or when the limit of stream
       identifiers has been reached. */
    CURL_TRC_M(దత్తాంశం, "http/2: No new requests allowed");
    connclose(cf->conn);
  }

  తిరిగి CURLE_OK;
}

/*
 * The server may send us data at any point (e.g. PING frames). Therefore, we
 * cannot assume that an HTTP/2 socket is dead because it is readable.
 *
 * Check the lower filters first and, if successful, peek at the socket
 * and distinguish between closed and data.
 */
స్థిర bool http2_connisalive(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                              bool *ఆదానం_pending)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *ఆదానం_pending = FALSE;
  ఒకవేళ(!cf->తదుపరి || !cf->తదుపరి->cft->is_alive(cf->తదుపరి, దత్తాంశం, ఆదానం_pending))
    తిరిగి FALSE;

  ఒకవేళ(*ఆదానం_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode ఫలితం;
    size_t nread;

    *ఆదానం_pending = FALSE;
    ఫలితం = Curl_cf_recv_bufq(cf->తదుపరి, దత్తాంశం, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(దత్తాంశం, cf, "connisalive, recv pending input -> %d, %zu",
                (పూర్ణాంకం)ఫలితం, nread);
    ఒకవేళ(!ఫలితం) {
      ఫలితం = h2_process_pending_ఆదానం(cf, దత్తాంశం);
      ఒకవేళ(ఫలితం)
        /* immediate error, considered dead */
        alive = FALSE;
      లేకపోతే {
        alive = !should_close_session(ctx);
      }
    }
    లేకపోతే ఒకవేళ(ఫలితం != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  తిరిగి alive;
}

స్థిర CURLcode http2_send_ping(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  పూర్ణాంకం rc;

  rc = Curl_ngh2_submit_ping(&ctx->ngh2, 0, ZERO_NULL);
  ఒకవేళ(rc) {
    failf(దత్తాంశం, "nghttp2_submit_ping() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    తిరిగి CURLE_HTTP2;
  }

  rc = Curl_ngh2_session_send(&ctx->ngh2);
  ఒకవేళ(rc) {
    failf(దత్తాంశం, "nghttp2_session_send() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    తిరిగి CURLE_SEND_ERROR;
  }
  తిరిగి CURLE_OK;
}

/*
 * Store nghttp2 version info in this buffer.
 */
శూన్య Curl_http2_ver(అక్షరం *p, size_t len)
{
  nghttp2_info *h2 = nghttp2_సంస్కరణ(0);
  (శూన్య)curl_msnprintf(p, len, "nghttp2/%s", h2->సంస్కరణ_str);
}

/*
 * The implementation of nghttp2_send_callback type. Here we write |data| with
 * size |length| to the network and return the number of bytes actually
 * written. See the documentation of nghttp2_send_callback for the details.
 */
స్థిర ssize_t send_callback(nghttp2_session *h2,
                             స్థిరరాశి uint8_t *buf, size_t blen, పూర్ణాంకం flags,
                             శూన్య *userp)
{
  నిర్మాణం Curl_cfilter *cf = userp;
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
  size_t nwritten;
  CURLcode ఫలితం = CURLE_OK;

  (శూన్య)h2;
  (శూన్య)flags;
  DEBUGASSERT(దత్తాంశం);

  ఒకవేళ(!cf->connected)
    ఫలితం = Curl_bufq_write(&ctx->outbufq, buf, blen, &nwritten);
  లేకపోతే
    ఫలితం = Curl_cf_send_bufq(cf->తదుపరి, దత్తాంశం, &ctx->outbufq, buf, blen,
                               &nwritten);

  ఒకవేళ(ఫలితం) {
    ఒకవేళ(ఫలితం == CURLE_AGAIN) {
      ctx->nw_బహిర్గతం_blocked = 1;
      తిరిగి NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(దత్తాంశం, "Failed sending HTTP2 data");
    తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  ఒకవేళ(!nwritten) {
    ctx->nw_బహిర్గతం_blocked = 1;
    తిరిగి NGHTTP2_ERR_WOULDBLOCK;
  }
  తిరిగి (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

/* We pass a pointer to this struct in the push callback, but the contents of
   the struct are hidden from the user. */
నిర్మాణం curl_pushheaders {
  నిర్మాణం Curl_easy *దత్తాంశం;
  నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం;
  స్థిరరాశి nghttp2_నెట్టు_promise *చట్రం;
};

/*
 * push header access function. Only to be used from within the push callback
 */
అక్షరం *curl_pushheader_bynum(నిర్మాణం curl_pushheaders *h, size_t అంకె)
{
  /* Verify that we got a good easy handle in the push header struct, mostly to
     detect rubbish input fast(er). */
  ఒకవేళ(!h || !GOOD_EASY_HANDLE(h->దత్తాంశం))
    తిరిగి ఖాళీ;
  లేకపోతే {
    ఒకవేళ(h->ప్రవాహం && అంకె < h->ప్రవాహం->నెట్టు_శీర్షికలు_వాడిన)
      తిరిగి h->ప్రవాహం->నెట్టు_శీర్షికలు[అంకె];
  }
  తిరిగి ఖాళీ;
}

/*
 * push header access function. Only to be used from within the push callback
 */
అక్షరం *curl_pushheader_byname(నిర్మాణం curl_pushheaders *h, స్థిరరాశి అక్షరం *name)
{
  నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం;
  size_t len;
  size_t i;
  /* Verify that we got a good easy handle in the push header struct,
     mostly to detect rubbish input fast(er). Also empty header name
     is rubbish too. We have to allow ":" at the beginning of
     the header, but header == ":" must be rejected. If we have ':' in
     the middle of header, it could be matched in middle of the value,
     this is because we do prefix match.*/
  ఒకవేళ(!h || !GOOD_EASY_HANDLE(h->దత్తాంశం) || !name || !name[0] ||
     !strcmp(name, ":") || strchr(name + 1, ':'))
    తిరిగి ఖాళీ;

  ప్రవాహం = h->ప్రవాహం;
  ఒకవేళ(!ప్రవాహం)
    తిరిగి ఖాళీ;

  len = అక్షరశ్రేణి_పొడవు(name);
  కొరకు(i = 0; i < ప్రవాహం->నెట్టు_శీర్షికలు_వాడిన; i++) {
    ఒకవేళ(!అక్షరశ్రేణి_పోలిక(name, ప్రవాహం->నెట్టు_శీర్షికలు[i], len)) {
      /* sub-match, make sure that it is followed by a colon */
      ఒకవేళ(ప్రవాహం->నెట్టు_శీర్షికలు[i][len] != ':')
        కొనసాగు;
      తిరిగి &ప్రవాహం->నెట్టు_శీర్షికలు[i][len + 1];
    }
  }
  తిరిగి ఖాళీ;
}

స్థిర నిర్మాణం Curl_easy *h2_duphandle(నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_easy *రెండవ = curl_easy_init();
  ఒకవేళ(రెండవ) {
    నిర్మాణం h2_ప్రవాహం_ctx *రెండవ_ప్రవాహం;
    http2_దత్తాంశం_అమరిక(cf, రెండవ, &రెండవ_ప్రవాహం);
    రెండవ->state.బరువు = దత్తాంశం->state.బరువు;
    ఒకవేళ(దత్తాంశం->share)
      (శూన్య)Curl_share_easy_link(రెండవ, దత్తాంశం->share);
    /* copy a range of options from the parent handle */
    రెండవ->సమితి.fwrite_func = దత్తాంశం->సమితి.fwrite_func;
    రెండవ->సమితి.బహిర్గతం = దత్తాంశం->సమితి.బహిర్గతం;
    రెండవ->సమితి.fwrite_శీర్షిక = దత్తాంశం->సమితి.fwrite_శీర్షిక;
    రెండవ->సమితి.writeheader = దత్తాంశం->సమితి.writeheader;
    రెండవ->సమితి.fdebug = దత్తాంశం->సమితి.fdebug;
    రెండవ->సమితి.debugdata = దత్తాంశం->సమితి.debugdata;
    రెండవ->సమితి.fxferinfo = దత్తాంశం->సమితి.fxferinfo;
    రెండవ->సమితి.fprogress = దత్తాంశం->సమితి.fprogress;
    రెండవ->సమితి.పురోగతి_client = దత్తాంశం->సమితి.పురోగతి_client;
    రెండవ->పురోగతి.hide = దత్తాంశం->పురోగతి.hide;
    రెండవ->పురోగతి.callback = దత్తాంశం->పురోగతి.callback;
  }
  తిరిగి రెండవ;
}

స్థిర పూర్ణాంకం సమితి_transfer_url(నిర్మాణం Curl_easy *newhandle,
                            నిర్మాణం curl_pushheaders *hp,
                            నిర్మాణం Curl_easy *దత్తాంశం)
{
  స్థిరరాశి అక్షరం *v;
  CURLUcode uc;
  అక్షరం *url = ఖాళీ;
  పూర్ణాంకం rc = 0;
  CURLU *u = curl_url();

  ఒకవేళ(!u)
    తిరిగి 5;

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_SCHEME);
  ఒకవేళ(v) {
    uc = curl_url_సమితి(u, CURLUPART_SCHEME, v, 0);
    ఒకవేళ(uc) {
      rc = 1;
      వెళ్ళు fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_AUTHORITY);
  ఒకవేళ(v) {
    uc = Curl_url_సమితి_authority(u, v);
    ఒకవేళ(uc) {
      rc = 2;
      వెళ్ళు fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_మార్గం);
  ఒకవేళ(v) {
    uc = curl_url_సమితి(u, CURLUPART_మార్గం, v, 0);
    ఒకవేళ(uc) {
      rc = 3;
      వెళ్ళు fail;
    }
  }

  /* We can only allow PUSH of resource from the same origin, e.g.
   * scheme + hostname + port */
  ఒకవేళ(!Curl_url_same_origin(దత్తాంశం->state.uh, u)) {
    rc = 1;
    వెళ్ళు fail;
  }

  uc = curl_url_get(u, CURLUPART_URL, &url, 0);
  ఒకవేళ(uc)
    rc = 4;
fail:
  ఒకవేళ(rc) {
    curl_url_cleanup(u);
    తిరిగి rc;
  }

  DEBUGASSERT(!newhandle->state.uh);
  newhandle->state.uh = u;
  Curl_bufref_సమితి(&newhandle->state.url, url, 0, curl_free);
  తిరిగి 0;
}

స్థిర శూన్య discard_newhandle(నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *newhandle)
{
  http2_దత్తాంశం_పూర్తి(cf, newhandle);
  (శూన్య)Curl_close(&newhandle);
}

స్థిర పూర్ణాంకం నెట్టు_promise(నిర్మాణం Curl_cfilter *cf,
                        నిర్మాణం Curl_easy *దత్తాంశం,
                        స్థిరరాశి nghttp2_నెట్టు_promise *చట్రం)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  పూర్ణాంకం rv; /* one of the CURL_PUSH_* defines */

  CURL_TRC_CF(దత్తాంశం, cf, "[%d] PUSH_PROMISE received",
              చట్రం->promised_ప్రవాహం_id);
  ఒకవేళ(దత్తాంశం->multi->నెట్టు_cb) {
    నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం;
    నిర్మాణం h2_ప్రవాహం_ctx *newstream;
    నిర్మాణం curl_pushheaders heads;
    CURLMcode mresult;
    CURLcode ఫలితం;
    /* clone the parent */
    నిర్మాణం Curl_easy *newhandle = h2_duphandle(cf, దత్తాంశం);
    ఒకవేళ(!newhandle) {
      infof(దత్తాంశం, "failed to duplicate handle");
      rv = CURL_నెట్టు_DENY; /* FAIL HARD */
      వెళ్ళు fail;
    }

    ప్రవాహం = H2_ప్రవాహం_CTX(ctx, దత్తాంశం);
    ఒకవేళ(!ప్రవాహం) {
      failf(దత్తాంశం, "Internal NULL stream");
      discard_newhandle(cf, newhandle);
      rv = CURL_నెట్టు_DENY;
      వెళ్ళు fail;
    }

    heads.దత్తాంశం = దత్తాంశం;
    heads.ప్రవాహం = ప్రవాహం;
    heads.చట్రం = చట్రం;

    rv = సమితి_transfer_url(newhandle, &heads, దత్తాంశం);
    ఒకవేళ(rv) {
      CURL_TRC_CF(దత్తాంశం, cf, "[%d] PUSH_PROMISE, failed to set URL -> %d",
                  చట్రం->promised_ప్రవాహం_id, rv);
      discard_newhandle(cf, newhandle);
      rv = CURL_నెట్టు_DENY;
      వెళ్ళు fail;
    }

    {
      నిర్మాణం Curl_mapi_guard guard;
      CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, multi_నెట్టు_cb);
      rv = దత్తాంశం->multi->నెట్టు_cb(దత్తాంశం, newhandle,
                                ప్రవాహం->నెట్టు_శీర్షికలు_వాడిన, &heads,
                                దత్తాంశం->multi->నెట్టు_userp);
      CURL_CBAPI_చివర(&guard);
    }

    /* free the headers again */
    free_నెట్టు_శీర్షికలు(ప్రవాహం);

    ఒకవేళ(rv) {
      DEBUGASSERT((rv > CURL_నెట్టు_OK) && (rv <= CURL_నెట్టు_ERROROUT));
      /* denied, kill off the new handle again */
      CURL_TRC_CF(దత్తాంశం, cf, "[%d] PUSH_PROMISE, denied by application -> %d",
                  చట్రం->promised_ప్రవాహం_id, rv);
      discard_newhandle(cf, newhandle);
      వెళ్ళు fail;
    }

    /* approved, add to the multi handle for processing. This
     * assigns newhandle->mid. For the new `mid` we assign the
     * h2_stream instance and remember the stream_id already known. */
    mresult = Curl_multi_కూడు_perform(దత్తాంశం->multi, newhandle, cf->conn);
    ఒకవేళ(mresult) {
      infof(దత్తాంశం, "failed to add handle to multi");
      discard_newhandle(cf, newhandle);
      rv = CURL_నెట్టు_DENY;
      వెళ్ళు fail;
    }

    ఫలితం = http2_దత్తాంశం_అమరిక(cf, newhandle, &newstream);
    ఒకవేళ(ఫలితం) {
      failf(దత్తాంశం, "error setting up stream: %d", (పూర్ణాంకం)ఫలితం);
      discard_newhandle(cf, newhandle);
      rv = CURL_నెట్టు_DENY;
      వెళ్ళు fail;
    }

    DEBUGASSERT(newstream);
    newstream->id = చట్రం->promised_ప్రవాహం_id;
    newhandle->req.maxdownload = -1;
    newhandle->req.కొలత = -1;

    CURL_TRC_CF(దత్తాంశం, cf, "promise easy handle added to multi, mid=%u",
                newhandle->mid);
    rv = Curl_ngh2_session_సమితి_ప్రవాహం_user_దత్తాంశం(
      &ctx->ngh2, newstream->id, newstream);
    ఒకవేళ(rv) {
      infof(దత్తాంశం, "failed to set user_data for stream %d",
            newstream->id);
      DEBUGASSERT(0);
      discard_newhandle(cf, newhandle);
      rv = CURL_నెట్టు_DENY;
      వెళ్ళు fail;
    }

    /* success, remember max stream id processed */
    ఒకవేళ(newstream->id > ctx->local_max_sid)
      ctx->local_max_sid = newstream->id;
  }
  లేకపోతే {
    CURL_TRC_CF(దత్తాంశం, cf, "Got PUSH_PROMISE, ignore it");
    rv = CURL_నెట్టు_DENY;
  }
fail:
  తిరిగి rv;
}

స్థిర శూన్య h2_xfer_write_resp_hd(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం,
                                  స్థిరరాశి అక్షరం *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  ఒకవేళ(!ప్రవాహం->xfer_ఫలితం) {
    ప్రవాహం->xfer_ఫలితం = Curl_xfer_write_resp_hd(దత్తాంశం, buf, blen, eos);
    ఒకవేళ(!ప్రవాహం->xfer_ఫలితం && !eos)
      ప్రవాహం->xfer_ఫలితం = cf_h2_update_local_win(cf, దత్తాంశం, ప్రవాహం);
    ఒకవేళ(ప్రవాహం->xfer_ఫలితం)
      CURL_TRC_CF(దత్తాంశం, cf, "[%d] error %d writing %zu bytes of headers",
                  ప్రవాహం->id, (పూర్ణాంకం)ప్రవాహం->xfer_ఫలితం, blen);
  }
}

స్థిర శూన్య h2_xfer_write_resp(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం,
                               స్థిరరాశి అక్షరం *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  ఒకవేళ(!ప్రవాహం->xfer_ఫలితం)
    ప్రవాహం->xfer_ఫలితం = Curl_xfer_write_resp(దత్తాంశం, buf, blen, eos);
  /* If the transfer write is errored, we do not want any more data */
  ఒకవేళ(ప్రవాహం->xfer_ఫలితం) {
    నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] error %d writing %zu bytes of data, "
                "RST-ing stream",
                ప్రవాహం->id, (పూర్ణాంకం)ప్రవాహం->xfer_ఫలితం, blen);
    Curl_ngh2_submit_rst_ప్రవాహం(
      &ctx->ngh2, 0, ప్రవాహం->id, (uint32_t)NGHTTP2_ERR_CALLBACK_FAILURE);
  }
  లేకపోతే ఒకవేళ(!ప్రవాహం->write_paused && Curl_xfer_write_is_paused(దత్తాంశం)) {
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] stream output paused", ప్రవాహం->id);
    ప్రవాహం->write_paused = TRUE;
  }
  లేకపోతే ఒకవేళ(ప్రవాహం->write_paused && !Curl_xfer_write_is_paused(దత్తాంశం)) {
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] stream output unpaused", ప్రవాహం->id);
    ప్రవాహం->write_paused = FALSE;
  }

  ఒకవేళ(!ప్రవాహం->xfer_ఫలితం && !eos)
    ప్రవాహం->xfer_ఫలితం = cf_h2_update_local_win(cf, దత్తాంశం, ప్రవాహం);
}

స్థిర CURLcode on_ప్రవాహం_చట్రం(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం,
                                స్థిరరాశి nghttp2_చట్రం *చట్రం)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  int32_t ప్రవాహం_id = చట్రం->hd.ప్రవాహం_id;
  పూర్ణాంకం rv;

  ఒకవేళ(!ప్రవాహం) {
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] No stream_ctx set", ప్రవాహం_id);
    తిరిగి CURLE_FAILED_INIT;
  }

  ఎంపిక(చట్రం->hd.type) {
  స్థితి NGHTTP2_దత్తాంశం:
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] DATA, window=%d/%d",
                ప్రవాహం_id,
                Curl_ngh2_session_get_ప్రవాహం_effective_recv_దత్తాంశం_నిడివి(
                  &ctx->ngh2, ప్రవాహం->id),
                Curl_ngh2_session_get_ప్రవాహం_effective_local_కిటికీ_కొలత(
                  &ctx->ngh2, ప్రవాహం->id));
    /* If !body started on this stream, then receiving DATA is illegal. */
    ఒకవేళ(!ప్రవాహం->bodystarted) {
      ఒకవేళ(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_ప్రవాహం(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                       ప్రవాహం_id, NGHTTP2_PROTOCOL_ERROR))) {
        తిరిగి CURLE_RECV_ERROR;
      }
    }
    ఆపు;
  స్థితి NGHTTP2_శీర్షికలు:
    ఒకవేళ(ప్రవాహం->bodystarted) {
      /* Only valid HEADERS after body started is trailer HEADERS. We
         buffer them in on_header callback. */
      ఆపు;
    }

    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    ఒకవేళ(ప్రవాహం->స్థితి_సంకేతం == -1)
      తిరిగి CURLE_RECV_ERROR;

    /* Only final status code signals the end of header */
    ఒకవేళ(ప్రవాహం->స్థితి_సంకేతం / 100 != 1) {
      ప్రవాహం->resp_hds_complete = TRUE;
      ప్రవాహం->bodystarted = TRUE;
    }
    లేకపోతే
      ప్రవాహం->స్థితి_సంకేతం = -1;

    h2_xfer_write_resp_hd(cf, దత్తాంశం, ప్రవాహం, STRCONST("\r\n"),
                          (bool)ప్రవాహం->closed);

    Curl_multi_mark_dirty(దత్తాంశం);
    ఆపు;
  స్థితి NGHTTP2_నెట్టు_PROMISE:
    rv = నెట్టు_promise(cf, దత్తాంశం, &చట్రం->నెట్టు_promise);
    ఒకవేళ(rv) { /* deny! */
      DEBUGASSERT((rv > CURL_నెట్టు_OK) && (rv <= CURL_నెట్టు_ERROROUT));
      ఒకవేళ(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_ప్రవాహం(&ctx->ngh2, NGHTTP2_FLAG_NONE,
             చట్రం->నెట్టు_promise.promised_ప్రవాహం_id, NGHTTP2_CANCEL)))
        తిరిగి CURLE_SEND_ERROR;
      లేకపోతే ఒకవేళ(rv == CURL_నెట్టు_ERROROUT) {
        /* fail the parent transfer, not the whole session */
        CURL_TRC_CF(దత్తాంశం, cf, "[%d] fail in PUSH_PROMISE received",
                    ప్రవాహం_id);
        ఒకవేళ(!ప్రవాహం->xfer_ఫలితం)
          ప్రవాహం->xfer_ఫలితం = CURLE_RECV_ERROR;
        Curl_multi_mark_dirty(దత్తాంశం);
      }
    }
    ఆపు;
  స్థితి NGHTTP2_RST_ప్రవాహం:
    ఒకవేళ(చట్రం->rst_ప్రవాహం.error_సంకేతం)
      ప్రవాహం->reset_by_server = TRUE;
    Curl_multi_mark_dirty(దత్తాంశం);
    ఆపు;
  స్థితి NGHTTP2_కిటికీ_UPDATE:
    ఒకవేళ(CURL_REQ_WANT_SEND(దత్తాంశం) && Curl_bufq_is_ఖాళీలు(&ప్రవాహం->sendbuf)) {
      /* need more data, force processing of transfer */
      Curl_multi_mark_dirty(దత్తాంశం);
    }
    లేకపోతే ఒకవేళ(!Curl_bufq_is_ఖాళీలు(&ప్రవాహం->sendbuf)) {
      /* resume the potentially suspended stream */
      ఒకవేళ(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_session_resume_దత్తాంశం(&ctx->ngh2, ప్రవాహం->id)))
        తిరిగి CURLE_SEND_ERROR;
    }
    ఆపు;
  అప్రమేయ:
    ఆపు;
  }

  ఒకవేళ(చట్రం->hd.flags & NGHTTP2_FLAG_చివర_ప్రవాహం) {
    ఒకవేళ(!ప్రవాహం->closed && !ప్రవాహం->body_eos &&
       ((ప్రవాహం->స్థితి_సంకేతం >= 400) || (ప్రవాహం->స్థితి_సంకేతం < 200))) {
      /* The server did not give us a positive response and we are not
       * done uploading the request body. We need to stop doing that and
       * also inform the server that we aborted our side. */
      CURL_TRC_CF(దత్తాంశం, cf, "[%d] EOS frame with unfinished upload and "
                  "HTTP status %d, abort upload by RST",
                  ప్రవాహం_id, ప్రవాహం->స్థితి_సంకేతం);
      Curl_ngh2_submit_rst_ప్రవాహం(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, ప్రవాహం->id, NGHTTP2_ప్రవాహం_CLOSED);
      ప్రవాహం->closed = TRUE;
    }
    Curl_multi_mark_dirty(దత్తాంశం);
  }
  తిరిగి CURLE_OK;
}

#నిర్వచితమైతే CURLVERBOSE
పూర్ణాంకం Curl_nghttp2_fr_print(స్థిరరాశి nghttp2_చట్రం *చట్రం, అక్షరం *బఫరు,
                          size_t blen)
{
  ఎంపిక(చట్రం->hd.type) {
  స్థితి NGHTTP2_దత్తాంశం: {
    తిరిగి curl_msnprintf(బఫరు, blen,
                          "FRAME[DATA, len=%d, eos=%d, padlen=%d]",
                          (పూర్ణాంకం)చట్రం->hd.నిడివి,
                          !!(చట్రం->hd.flags & NGHTTP2_FLAG_చివర_ప్రవాహం),
                          (పూర్ణాంకం)చట్రం->దత్తాంశం.padlen);
  }
  స్థితి NGHTTP2_శీర్షికలు: {
    తిరిగి curl_msnprintf(బఫరు, blen,
                          "FRAME[HEADERS, len=%d, hend=%d, eos=%d]",
                          (పూర్ణాంకం)చట్రం->hd.నిడివి,
                          !!(చట్రం->hd.flags & NGHTTP2_FLAG_చివర_శీర్షికలు),
                          !!(చట్రం->hd.flags & NGHTTP2_FLAG_చివర_ప్రవాహం));
  }
  స్థితి NGHTTP2_ప్రాధాన్యత: {
    తిరిగి curl_msnprintf(బఫరు, blen,
                          "FRAME[PRIORITY, len=%d, flags=%d]",
                          (పూర్ణాంకం)చట్రం->hd.నిడివి, చట్రం->hd.flags);
  }
  స్థితి NGHTTP2_RST_ప్రవాహం: {
    తిరిగి curl_msnprintf(బఫరు, blen,
                          "FRAME[RST_STREAM, len=%d, flags=%d, error=%u]",
                          (పూర్ణాంకం)చట్రం->hd.నిడివి, చట్రం->hd.flags,
                          చట్రం->rst_ప్రవాహం.error_సంకేతం);
  }
  స్థితి NGHTTP2_SETTINGS: {
    ఒకవేళ(చట్రం->hd.flags & NGHTTP2_FLAG_ACK) {
      తిరిగి curl_msnprintf(బఫరు, blen, "FRAME[SETTINGS, ack=1]");
    }
    తిరిగి curl_msnprintf(బఫరు, blen,
                          "FRAME[SETTINGS, len=%d]", (పూర్ణాంకం)చట్రం->hd.నిడివి);
  }
  స్థితి NGHTTP2_నెట్టు_PROMISE:
    తిరిగి curl_msnprintf(బఫరు, blen,
                          "FRAME[PUSH_PROMISE, len=%d, hend=%d]",
                          (పూర్ణాంకం)చట్రం->hd.నిడివి,
                          !!(చట్రం->hd.flags & NGHTTP2_FLAG_చివర_శీర్షికలు));
  స్థితి NGHTTP2_PING:
    తిరిగి curl_msnprintf(బఫరు, blen,
                          "FRAME[PING, len=%d, ack=%d]",
                          (పూర్ణాంకం)చట్రం->hd.నిడివి,
                          చట్రం->hd.flags & NGHTTP2_FLAG_ACK);
  స్థితి NGHTTP2_GOAWAY: {
    అక్షరం scratch[128];
    size_t s_len = CURL_ARRAYSIZE(scratch);
    size_t len = (చట్రం->goaway.opaque_దత్తాంశం_len < s_len) ?
      చట్రం->goaway.opaque_దత్తాంశం_len : s_len - 1;
    ఒకవేళ(len)
      జ్ఞాపకం_నకలు(scratch, చట్రం->goaway.opaque_దత్తాంశం, len);
    scratch[len] = '\0';
    తిరిగి curl_msnprintf(బఫరు, blen,
                          "FRAME[GOAWAY, error=%u, reason='%s', "
                          "last_stream=%d]", చట్రం->goaway.error_సంకేతం,
                          scratch, చట్రం->goaway.చివరి_ప్రవాహం_id);
  }
  స్థితి NGHTTP2_కిటికీ_UPDATE: {
    తిరిగి curl_msnprintf(బఫరు, blen,
                          "FRAME[WINDOW_UPDATE, incr=%d]",
                          చట్రం->కిటికీ_update.కిటికీ_కొలత_increment);
  }
  అప్రమేయ:
    తిరిగి curl_msnprintf(బఫరు, blen, "FRAME[%d, len=%d, flags=%d]",
                          చట్రం->hd.type, (పూర్ణాంకం)చట్రం->hd.నిడివి,
                          చట్రం->hd.flags);
  }
}

స్థిర పూర్ణాంకం on_చట్రం_send(nghttp2_session *session, స్థిరరాశి nghttp2_చట్రం *చట్రం,
                         శూన్య *userp)
{
  నిర్మాణం Curl_cfilter *cf = userp;
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);

  (శూన్య)session;
  DEBUGASSERT(దత్తాంశం);
  ఒకవేళ(Curl_trc_cf_is_verbose(cf, దత్తాంశం)) {
    అక్షరం బఫరు[256];
    పూర్ణాంకం len;
    len = Curl_nghttp2_fr_print(చట్రం, బఫరు, పరిమాణం(బఫరు) - 1);
    బఫరు[len] = 0;
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] -> %s", చట్రం->hd.ప్రవాహం_id, బఫరు);
  }
  ఒకవేళ((చట్రం->hd.type == NGHTTP2_GOAWAY) && !ctx->పంపిన_goaway) {
    /* A GOAWAY not initiated by us, but by nghttp2 itself on detecting
     * a protocol error on the connection */
    failf(దత్తాంశం, "nghttp2 shuts down connection with error %u: %s",
          చట్రం->goaway.error_సంకేతం,
          nghttp2_http2_strerror(చట్రం->goaway.error_సంకేతం));
  }
  తిరిగి 0;
}
#అయితే_ముగింపు /* CURLVERBOSE */

స్థిర పూర్ణాంకం on_చట్రం_recv(nghttp2_session *session, స్థిరరాశి nghttp2_చట్రం *చట్రం,
                         శూన్య *userp)
{
  నిర్మాణం Curl_cfilter *cf = userp;
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf), *దత్తాంశం_s;
  నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం;
  int32_t ప్రవాహం_id = చట్రం->hd.ప్రవాహం_id;

  (శూన్య)session;
  DEBUGASSERT(దత్తాంశం);
#నిర్వచితమైతే CURLVERBOSE
  ఒకవేళ(Curl_trc_cf_is_verbose(cf, దత్తాంశం)) {
    అక్షరం బఫరు[256];
    పూర్ణాంకం len;
    len = Curl_nghttp2_fr_print(చట్రం, బఫరు, పరిమాణం(బఫరు) - 1);
    బఫరు[len] = 0;
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] <- %s", చట్రం->hd.ప్రవాహం_id, బఫరు);
  }
#అయితే_ముగింపు /* CURLVERBOSE */

  ఒకవేళ(!ప్రవాహం_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(దత్తాంశం);
    ఎంపిక(చట్రం->hd.type) {
    స్థితి NGHTTP2_SETTINGS: {
      ఒకవేళ(!(చట్రం->hd.flags & NGHTTP2_FLAG_ACK)) {
        uint32_t max_conn = ctx->max_concurrent_streams;
        ctx->max_concurrent_streams =
          Curl_ngh2_session_get_remote_settings(
            &ctx->ngh2, NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS);
        ctx->enable_నెట్టు = Curl_ngh2_session_get_remote_settings(
          &ctx->ngh2, NGHTTP2_SETTINGS_ENABLE_నెట్టు) != 0;
        CURL_TRC_CF(దత్తాంశం, cf, "[0] MAX_CONCURRENT_STREAMS: %u",
                    ctx->max_concurrent_streams);
        CURL_TRC_CF(దత్తాంశం, cf, "[0] ENABLE_PUSH: %s",
                    ctx->enable_నెట్టు ? "TRUE" : "false");
        ఒకవేళ(దత్తాంశం && max_conn != ctx->max_concurrent_streams) {
          /* only signal change if the value actually changed */
          CURL_TRC_CF(దత్తాంశం, cf, "[0] notify MAX_CONCURRENT_STREAMS: %u",
                      ctx->max_concurrent_streams);
          Curl_multi_connchanged(దత్తాంశం->multi);
        }
        /* Since the initial stream window is 64K, a request might be on HOLD,
         * due to exhaustion. The (initial) SETTINGS may announce a much larger
         * window and *assume* that we treat this like a WINDOW_UPDATE. Some
         * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
         * To be safe, we UNHOLD a stream in order not to stall. */
        ఒకవేళ(CURL_REQ_WANT_SEND(దత్తాంశం))
          Curl_multi_mark_dirty(దత్తాంశం);
      }
      ఆపు;
    }
    స్థితి NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->goaway_error = చట్రం->goaway.error_సంకేతం;
      ctx->remote_max_sid = చట్రం->goaway.చివరి_ప్రవాహం_id;
      ఒకవేళ(దత్తాంశం) {
        infof(దత్తాంశం, "received GOAWAY, error=%u, last_stream=%d",
              ctx->goaway_error, ctx->remote_max_sid);
        Curl_multi_connchanged(దత్తాంశం->multi);
      }
      ఆపు;
    అప్రమేయ:
      ఆపు;
    }
    తిరిగి 0;
  }

  దత్తాంశం_s = cf_h2_get_ప్రవాహం_xfer(cf, ప్రవాహం_id, &ప్రవాహం);
  ఒకవేళ(!దత్తాంశం_s) {
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] No Curl_easy associated", ప్రవాహం_id);
    తిరిగి 0;
  }

  తిరిగి on_ప్రవాహం_చట్రం(cf, దత్తాంశం_s, ప్రవాహం, చట్రం) ?
         NGHTTP2_ERR_CALLBACK_FAILURE : 0;
}

స్థిర పూర్ణాంకం cf_h2_on_invalid_చట్రం_recv(nghttp2_session *session,
                                       స్థిరరాశి nghttp2_చట్రం *చట్రం,
                                       పూర్ణాంకం ngerr, శూన్య *userp)
{
  నిర్మాణం Curl_cfilter *cf = userp;
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  నిర్మాణం Curl_easy *దత్తాంశం;
  నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం;
  int32_t ప్రవాహం_id = చట్రం->hd.ప్రవాహం_id;

  (శూన్య)session;
  దత్తాంశం = cf_h2_get_ప్రవాహం_xfer(cf, ప్రవాహం_id, &ప్రవాహం);
  ఒకవేళ(దత్తాంశం) {
#నిర్వచితమైతే CURLVERBOSE
    అక్షరం బఫరు[256];
    పూర్ణాంకం len;
    len = Curl_nghttp2_fr_print(చట్రం, బఫరు, పరిమాణం(బఫరు) - 1);
    బఫరు[len] = 0;
    failf(దత్తాంశం, "[HTTP2] [%d] received invalid frame: %s, error %d: %s",
          ప్రవాహం_id, బఫరు, ngerr, nghttp2_strerror(ngerr));
#అయితే_ముగింపు /* CURLVERBOSE */
    ఒకవేళ(ప్రవాహం) {
      Curl_ngh2_submit_rst_ప్రవాహం(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, ప్రవాహం->id, NGHTTP2_ప్రవాహం_CLOSED);
      ప్రవాహం->error = ngerr;
      ప్రవాహం->closed = TRUE;
      ప్రవాహం->reset = TRUE;
    }
  }
  /* Return no error, nghttp2 will RST/GOAWAY by itself when needed */
  తిరిగి 0;
}

స్థిర పూర్ణాంకం on_దత్తాంశం_chunk_recv(nghttp2_session *session, uint8_t flags,
                              int32_t ప్రవాహం_id,
                              స్థిరరాశి uint8_t *mem, size_t len, శూన్య *userp)
{
  నిర్మాణం Curl_cfilter *cf = userp;
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం;
  నిర్మాణం Curl_easy *దత్తాంశం_s;

  (శూన్య)session;
  (శూన్య)flags;

  DEBUGASSERT(ప్రవాహం_id); /* should never be a zero stream ID here */
  DEBUGASSERT(CF_దత్తాంశం_CURRENT(cf));

  /* get the stream from the hash based on Stream ID */
  దత్తాంశం_s = cf_h2_get_ప్రవాహం_xfer(cf, ప్రవాహం_id, &ప్రవాహం);
  ఒకవేళ(!దత్తాంశం_s) {
    /* Receiving a Stream ID not in the hash should not happen - unless
       we have aborted a transfer artificially and there were more data
       in the pipeline. Silently ignore. */
    CURL_TRC_CF(CF_దత్తాంశం_CURRENT(cf), cf, "[%d] Data for unknown", ప్రవాహం_id);
    /* consumed explicitly as no one will read it */
    Curl_ngh2_session_consume(&ctx->ngh2, ప్రవాహం_id, len);
    తిరిగి 0;
  }

  ఒకవేళ(!ప్రవాహం)
    తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;

  h2_xfer_write_resp(cf, దత్తాంశం_s, ప్రవాహం, (స్థిరరాశి అక్షరం *)mem, len, FALSE);

  Curl_ngh2_session_consume(&ctx->ngh2, ప్రవాహం_id, len);
  ప్రవాహం->nrcvd_దత్తాంశం += (curl_off_t)len;
  తిరిగి 0;
}

స్థిర పూర్ణాంకం on_ప్రవాహం_close(nghttp2_session *session, int32_t ప్రవాహం_id,
                           uint32_t error_సంకేతం, శూన్య *userp)
{
  నిర్మాణం Curl_cfilter *cf = userp;
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  నిర్మాణం Curl_easy *దత్తాంశం_s, *call_దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
  నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం = ఖాళీ;
  పూర్ణాంకం rv;
  (శూన్య)session;

  DEBUGASSERT(call_దత్తాంశం);
  /* stream id 0 is the connection, do not look there for streams. */
  దత్తాంశం_s = ప్రవాహం_id ? cf_h2_get_ప్రవాహం_xfer(cf, ప్రవాహం_id, &ప్రవాహం) : ఖాళీ;
  ఒకవేళ(!దత్తాంశం_s) {
    CURL_TRC_CF(call_దత్తాంశం, cf,
                "[%d] on_stream_close, no easy set on stream", ప్రవాహం_id);
    తిరిగి 0;
  }
  ఒకవేళ(!GOOD_EASY_HANDLE(దత్తాంశం_s)) {
    /* nghttp2 still has an easy registered for the stream which has
     * been freed be libcurl. This points to a code path that does not
     * trigger DONE or DETACH events as it must. */
    CURL_TRC_CF(call_దత్తాంశం, cf,
                "[%d] on_stream_close, not a GOOD easy on stream", ప్రవాహం_id);
    (శూన్య)Curl_ngh2_session_సమితి_ప్రవాహం_user_దత్తాంశం(
      &ctx->ngh2, ప్రవాహం_id, ఖాళీ);
    తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  ఒకవేళ(!ప్రవాహం) {
    CURL_TRC_CF(దత్తాంశం_s, cf,
                "[%d] on_stream_close, GOOD easy but no stream", ప్రవాహం_id);
    తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  ప్రవాహం->closed = TRUE;
  ప్రవాహం->error = error_సంకేతం;
  ఒకవేళ(ప్రవాహం->error) {
    ప్రవాహం->reset = TRUE;
    ఒకవేళ((ప్రవాహం->error == NGHTTP2_REFUSED_ప్రవాహం) &&
       ctx->rcvd_goaway && ctx->goaway_error)
    /* REFUSED_STREAM is used by nghttp2 on a GOAWAY from the
     * server where it indicated it will no longer process this stream.
     * If the GOAWAY carried an error code, take that as the error for
     * the stream. */
    ప్రవాహం->error = ctx->goaway_error;
  }

  ఒకవేళ(ప్రవాహం->error)
    CURL_TRC_CF(దత్తాంశం_s, cf, "[%d] RESET: %s (err %u)",
              ప్రవాహం_id, nghttp2_http2_strerror(error_సంకేతం), error_సంకేతం);
  లేకపోతే
    CURL_TRC_CF(దత్తాంశం_s, cf, "[%d] CLOSED", ప్రవాహం_id);
  Curl_multi_mark_dirty(దత్తాంశం_s);

  /* remove `stream_ctx` from the nghttp2 stream */
  rv = Curl_ngh2_session_సమితి_ప్రవాహం_user_దత్తాంశం(
    &ctx->ngh2, ప్రవాహం_id, ఖాళీ);
  ఒకవేళ(rv) {
    infof(దత్తాంశం_s, "http/2: failed to clear user_data for stream %d",
          ప్రవాహం_id);
    DEBUGASSERT(0);
  }
  తిరిగి 0;
}

స్థిర శూన్య cf_h2_శీర్షిక_error(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం,
                               CURLcode ఫలితం)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;

  failf(దత్తాంశం, "Error receiving HTTP2 header: %d(%s)", (పూర్ణాంకం)ఫలితం,
        curl_easy_strerror(ఫలితం));
  ఒకవేళ(ప్రవాహం) {
    Curl_ngh2_submit_rst_ప్రవాహం(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, ప్రవాహం->id, NGHTTP2_ప్రవాహం_CLOSED);
    ప్రవాహం->closed = TRUE;
    ప్రవాహం->reset = TRUE;
  }
}

/* frame->hd.type is either NGHTTP2_HEADERS or NGHTTP2_PUSH_PROMISE */
స్థిర పూర్ణాంకం on_శీర్షిక(nghttp2_session *session, స్థిరరాశి nghttp2_చట్రం *చట్రం,
                     స్థిరరాశి uint8_t *name, size_t namelen,
                     స్థిరరాశి uint8_t *విలువ, size_t valuelen,
                     uint8_t flags,
                     శూన్య *userp)
{
  నిర్మాణం Curl_cfilter *cf = userp;
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం;
  నిర్మాణం Curl_easy *దత్తాంశం;
  int32_t ప్రవాహం_id = చట్రం->hd.ప్రవాహం_id;
  CURLcode ఫలితం;

  (శూన్య)session;
  (శూన్య)flags;

  DEBUGASSERT(ప్రవాహం_id); /* should never be a zero stream ID here */

  /* get the stream from the hash based on Stream ID */
  దత్తాంశం = cf_h2_get_ప్రవాహం_xfer(cf, ప్రవాహం_id, &ప్రవాహం);
  ఒకవేళ(!GOOD_EASY_HANDLE(దత్తాంశం))
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;

  ఒకవేళ(!ప్రవాహం) {
    failf(దత్తాంశం, "Internal NULL stream");
    తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  /* Store received PUSH_PROMISE headers to be used when the subsequent
     PUSH_PROMISE callback comes */
  ఒకవేళ(చట్రం->hd.type == NGHTTP2_నెట్టు_PROMISE) {
    అక్షరం *h;

    ఒకవేళ((namelen == CURL_CSTRLEN(HTTP_PSEUDO_AUTHORITY)) &&
       !అక్షరశ్రేణి_పోలిక(HTTP_PSEUDO_AUTHORITY, (స్థిరరాశి అక్షరం *)name, namelen)) {
      /* pseudo headers are lower case */
      పూర్ణాంకం rc = 0;
      అక్షరం *check = curl_maprintf("%s:%d", దత్తాంశం->state.origin->hostname,
                                  దత్తాంశం->state.origin->పోర్టు);
      ఒకవేళ(!check)
        /* no memory */
        తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;
      ఒకవేళ(!curl_strequal(check, (స్థిరరాశి అక్షరం *)విలువ) &&
         ((దత్తాంశం->state.origin->పోర్టు != దత్తాంశం->state.origin->scheme->defport) ||
          !curl_strequal(దత్తాంశం->state.origin->hostname, (స్థిరరాశి అక్షరం *)విలువ))) {
        /* This is push is not for the same authority that was asked for in
         * the URL. RFC 7540 section 8.2 says: "A client MUST treat a
         * PUSH_PROMISE for which the server is not authoritative as a stream
         * error of type PROTOCOL_ERROR."
         */
        (శూన్య)Curl_ngh2_submit_rst_ప్రవాహం(
          &ctx->ngh2, NGHTTP2_FLAG_NONE, ప్రవాహం_id, NGHTTP2_PROTOCOL_ERROR);
        rc = NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      curlx_free(check);
      ఒకవేళ(rc)
        తిరిగి rc;
    }

    ఒకవేళ(!ప్రవాహం->నెట్టు_శీర్షికలు) {
      ప్రవాహం->నెట్టు_శీర్షికలు_alloc = 10;
      ప్రవాహం->నెట్టు_శీర్షికలు = curlx_malloc(ప్రవాహం->నెట్టు_శీర్షికలు_alloc *
                                          పరిమాణం(అక్షరం *));
      ఒకవేళ(!ప్రవాహం->నెట్టు_శీర్షికలు)
        తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;
      ప్రవాహం->నెట్టు_శీర్షికలు_వాడిన = 0;
    }
    లేకపోతే ఒకవేళ(ప్రవాహం->నెట్టు_శీర్షికలు_వాడిన ==
            ప్రవాహం->నెట్టు_శీర్షికలు_alloc) {
      అక్షరం **headp;
      ఒకవేళ(ప్రవాహం->నెట్టు_శీర్షికలు_alloc > 1000) {
        /* this is beyond crazy many headers, bail out */
        failf(దత్తాంశం, "Too many PUSH_PROMISE headers");
        free_నెట్టు_శీర్షికలు(ప్రవాహం);
        తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      ప్రవాహం->నెట్టు_శీర్షికలు_alloc *= 2;
      headp = curlx_realloc(ప్రవాహం->నెట్టు_శీర్షికలు,
                            ప్రవాహం->నెట్టు_శీర్షికలు_alloc * పరిమాణం(అక్షరం *));
      ఒకవేళ(!headp) {
        free_నెట్టు_శీర్షికలు(ప్రవాహం);
        తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      ప్రవాహం->నెట్టు_శీర్షికలు = headp;
    }
    h = curl_maprintf("%s:%s", name, విలువ);
    ఒకవేళ(!h) {
      free_నెట్టు_శీర్షికలు(ప్రవాహం);
      తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    ప్రవాహం->నెట్టు_శీర్షికలు[ప్రవాహం->నెట్టు_శీర్షికలు_వాడిన++] = h;
    తిరిగి 0;
  }

  ఒకవేళ(ప్రవాహం->bodystarted) {
    /* This is a trailer */
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] trailer: %.*s: %.*s",
                ప్రవాహం->id, (పూర్ణాంకం)namelen, name, (పూర్ణాంకం)valuelen, విలువ);
    ఫలితం = Curl_dynhds_కూడు(&ప్రవాహం->resp_trailers,
                             (స్థిరరాశి అక్షరం *)name, namelen,
                             (స్థిరరాశి అక్షరం *)విలువ, valuelen);
    ఒకవేళ(ఫలితం) {
      cf_h2_శీర్షిక_error(cf, దత్తాంశం, ప్రవాహం, ఫలితం);
      తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;
    }

    తిరిగి 0;
  }

  ఒకవేళ(namelen == CURL_CSTRLEN(HTTP_PSEUDO_స్థితి) &&
     !memcmp(HTTP_PSEUDO_స్థితి, name, namelen)) {
    /* nghttp2 guarantees :status is received first and only once. */
    అక్షరం బఫరు[32];
    size_t hlen;
    ఫలితం = Curl_http_decode_స్థితి(&ప్రవాహం->స్థితి_సంకేతం,
                                     (స్థిరరాశి అక్షరం *)విలువ, valuelen);
    ఒకవేళ(ఫలితం) {
      cf_h2_శీర్షిక_error(cf, దత్తాంశం, ప్రవాహం, ఫలితం);
      తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    hlen = curl_msnprintf(బఫరు, పరిమాణం(బఫరు), HTTP_PSEUDO_స్థితి ":%d\r",
                          ప్రవాహం->స్థితి_సంకేతం);
    ఫలితం = Curl_శీర్షికలు_నెట్టు(దత్తాంశం, బఫరు, hlen, CURLH_PSEUDO);
    ఒకవేళ(ఫలితం) {
      cf_h2_శీర్షిక_error(cf, దత్తాంశం, ప్రవాహం, ఫలితం);
      తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    curlx_dyn_reset(&ctx->scratch);
    ఫలితం = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/2 "));
    ఒకవేళ(!ఫలితం)
      ఫలితం = curlx_dyn_addn(&ctx->scratch, విలువ, valuelen);
    ఒకవేళ(!ఫలితం)
      ఫలితం = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    ఒకవేళ(!ఫలితం)
      h2_xfer_write_resp_hd(cf, దత్తాంశం, ప్రవాహం, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    ఒకవేళ(ఫలితం) {
      cf_h2_శీర్షిక_error(cf, దత్తాంశం, ప్రవాహం, ఫలితం);
      తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    /* if we receive data for another handle, wake that up */
    ఒకవేళ(CF_దత్తాంశం_CURRENT(cf) != దత్తాంశం)
      Curl_multi_mark_dirty(దత్తాంశం);

    CURL_TRC_CF(దత్తాంశం, cf, "[%d] status: HTTP/2 %03d",
                ప్రవాహం->id, ప్రవాహం->స్థితి_సంకేతం);
    తిరిగి 0;
  }

  /* nghttp2 guarantees that namelen > 0, and :status was already
     received, and this is not pseudo-header field . */
  /* convert to an HTTP1-style header */
  curlx_dyn_reset(&ctx->scratch);
  ఫలితం = curlx_dyn_addn(&ctx->scratch, (స్థిరరాశి అక్షరం *)name, namelen);
  ఒకవేళ(!ఫలితం)
    ఫలితం = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
  ఒకవేళ(!ఫలితం)
    ఫలితం = curlx_dyn_addn(&ctx->scratch, (స్థిరరాశి అక్షరం *)విలువ, valuelen);
  ఒకవేళ(!ఫలితం)
    ఫలితం = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
  ఒకవేళ(!ఫలితం)
    h2_xfer_write_resp_hd(cf, దత్తాంశం, ప్రవాహం, curlx_dyn_ptr(&ctx->scratch),
                          curlx_dyn_len(&ctx->scratch), FALSE);
  ఒకవేళ(ఫలితం) {
    cf_h2_శీర్షిక_error(cf, దత్తాంశం, ప్రవాహం, ఫలితం);
    తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  /* if we receive data for another handle, wake that up */
  ఒకవేళ(CF_దత్తాంశం_CURRENT(cf) != దత్తాంశం)
    Curl_multi_mark_dirty(దత్తాంశం);

  CURL_TRC_CF(దత్తాంశం, cf, "[%d] header: %.*s: %.*s",
              ప్రవాహం->id, (పూర్ణాంకం)namelen, name, (పూర్ణాంకం)valuelen, విలువ);

  తిరిగి 0; /* 0 is successful */
}

స్థిర ssize_t req_body_read_callback(nghttp2_session *session,
                                      int32_t ప్రవాహం_id,
                                      uint8_t *buf, size_t నిడివి,
                                      uint32_t *దత్తాంశం_flags,
                                      nghttp2_దత్తాంశం_ఆధారం *ఆధారం,
                                      శూన్య *userp)
{
  నిర్మాణం Curl_cfilter *cf = userp;
  నిర్మాణం Curl_easy *దత్తాంశం_s;
  నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం = ఖాళీ;
  CURLcode ఫలితం;
  ssize_t nread;
  size_t n;
  (శూన్య)ఆధారం;

  (శూన్య)session;
  ఒకవేళ(!ప్రవాహం_id)
    తిరిగి NGHTTP2_ERR_INVALID_వాదం;

  /* get the stream from the hash based on Stream ID, stream ID zero is for
     connection-oriented stuff */
  దత్తాంశం_s = cf_h2_get_ప్రవాహం_xfer(cf, ప్రవాహం_id, &ప్రవాహం);
  ఒకవేళ(!దత్తాంశం_s || !ప్రవాహం)
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;

  ఫలితం = Curl_bufq_read(&ప్రవాహం->sendbuf, buf, నిడివి, &n);
  ఒకవేళ(ఫలితం) {
    ఒకవేళ(ఫలితం != CURLE_AGAIN)
      తిరిగి NGHTTP2_ERR_CALLBACK_FAILURE;
    nread = 0;
  }
  లేకపోతే
    nread = (ssize_t)n;

  CURL_TRC_CF(దత్తాంశం_s, cf, "[%d] req_body_read(len=%zu) eos=%d -> %zd, %d",
              ప్రవాహం_id, నిడివి, ప్రవాహం->body_eos, nread, (పూర్ణాంకం)ఫలితం);

  ఒకవేళ(ప్రవాహం->body_eos && Curl_bufq_is_ఖాళీలు(&ప్రవాహం->sendbuf)) {
    *దత్తాంశం_flags = NGHTTP2_దత్తాంశం_FLAG_EOF;
    తిరిగి nread;
  }
  తిరిగి (nread == 0) ? NGHTTP2_ERR_DEFERRED : nread;
}

#నిర్వచితమైతే CURLVERBOSE
స్థిర పూర్ణాంకం error_callback(nghttp2_session *session,
                          స్థిరరాశి అక్షరం *సందేశం,
                          size_t len,
                          శూన్య *userp)
{
  నిర్మాణం Curl_cfilter *cf = userp;
  నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
  (శూన్య)session;
  failf(దత్తాంశం, "%.*s", (పూర్ణాంకం)len, సందేశం);
  తిరిగి 0;
}
#అయితే_ముగింపు

/*
 * Append headers to ask for an HTTP1.1 to HTTP2 upgrade.
 */
CURLcode Curl_http2_request_upgrade(నిర్మాణం dynbuf *req,
                                    నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURLcode ఫలితం;
  అక్షరం *base64;
  size_t blen;
  నిర్మాణం SingleRequest *k = &దత్తాంశం->req;
  uint8_t binsettings[H2_BINSETTINGS_LEN];
  ssize_t rc;
  size_t binlen; /* length of the binsettings data */

  rc = populate_binsettings(binsettings, దత్తాంశం);
  ఒకవేళ(!curlx_sztouz(rc, &binlen) || !binlen) {
    failf(దత్తాంశం, "nghttp2 unexpectedly failed on pack_settings_payload");
    curlx_dyn_free(req);
    తిరిగి CURLE_FAILED_INIT;
  }

  ఫలితం = curlx_base64url_encode(binsettings, binlen, &base64, &blen);
  ఒకవేళ(ఫలితం) {
    curlx_dyn_free(req);
    తిరిగి ఫలితం;
  }

  దత్తాంశం->state.http_hd_upgrade = TRUE;
  దత్తాంశం->state.http_hd_h2_settings = TRUE;
  ఫలితం = curlx_dyn_addf(req,
                          "Upgrade: %s\r\n"
                          "HTTP2-Settings: %s\r\n",
                          NGHTTP2_CLEARTEXT_PROTO_సంస్కరణ_ID, base64);
  curlx_free(base64);

  k->upgr101 = UPGR101_H2;
  దత్తాంశం->conn->బిట్లు.upgrade_లోపల_పురోగతి = TRUE;

  తిరిగి ఫలితం;
}

స్థిర CURLcode http2_handle_ప్రవాహం_close(నిర్మాణం Curl_cfilter *cf,
                                          నిర్మాణం Curl_easy *దత్తాంశం,
                                          నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం,
                                          size_t *pnlen)
{
  CURLcode ఫలితం;

  *pnlen = 0;
  ఒకవేళ(ప్రవాహం->reset) {
    ఒకవేళ(ప్రవాహం->error == NGHTTP2_REFUSED_ప్రవాహం) {
      infof(దత్తాంశం, "HTTP/2 stream %d refused by server, try again on a new "
                  "connection", ప్రవాహం->id);
      connclose(cf->conn); /* do not use this anymore */
      దత్తాంశం->state.refused_ప్రవాహం = TRUE;
      తిరిగి CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    లేకపోతే ఒకవేళ(ప్రవాహం->resp_hds_complete && దత్తాంశం->req.no_body) {
      CURL_TRC_CF(దత్తాంశం, cf, "[%d] error after response headers, but we did "
                  "not want a body anyway, ignore: %s (err %u)",
                  ప్రవాహం->id, nghttp2_http2_strerror(ప్రవాహం->error),
                  ప్రవాహం->error);
      ప్రవాహం->close_handled = TRUE;
      తిరిగి CURLE_OK;
    }
    failf(దత్తాంశం, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          ప్రవాహం->id, ప్రవాహం->reset_by_server ? "server" : "curl",
          ప్రవాహం->error, nghttp2_http2_strerror(ప్రవాహం->error));
    తిరిగి ప్రవాహం->error ? CURLE_HTTP2_ప్రవాహం :
           (దత్తాంశం->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2);
  }
  లేకపోతే ఒకవేళ(!ప్రవాహం->bodystarted) {
    failf(దత్తాంశం, "HTTP/2 stream %d was closed cleanly, but before getting "
          "all response header fields, treated as error", ప్రవాహం->id);
    తిరిగి CURLE_HTTP2_ప్రవాహం;
  }

  ఒకవేళ(Curl_dynhds_లెక్క(&ప్రవాహం->resp_trailers)) {
    నిర్మాణం dynhds_నమోదు *e;
    నిర్మాణం dynbuf dbuf;
    size_t i;

    ఫలితం = CURLE_OK;
    curlx_dyn_init(&dbuf, DYN_TRAILERS);
    కొరకు(i = 0; i < Curl_dynhds_లెక్క(&ప్రవాహం->resp_trailers); ++i) {
      e = Curl_dynhds_getn(&ప్రవాహం->resp_trailers, i);
      ఒకవేళ(!e)
        ఆపు;
      curlx_dyn_reset(&dbuf);
      ఫలితం = curlx_dyn_addf(&dbuf, "%.*s: %.*s\x0d\x0a",
                              (పూర్ణాంకం)e->namelen, e->name,
                              (పూర్ణాంకం)e->valuelen, e->విలువ);
      ఒకవేళ(ఫలితం)
        ఆపు;
      Curl_debug(దత్తాంశం, CURLINFO_శీర్షిక_లోపల, curlx_dyn_ptr(&dbuf),
                 curlx_dyn_len(&dbuf));
      ఫలితం = Curl_client_write(దత్తాంశం,
                                 CLIENTWRITE_శీర్షిక | CLIENTWRITE_TRAILER,
                                 curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf));
      ఒకవేళ(ఫలితం)
        ఆపు;
      ఫలితం = Curl_bump_headersize(దత్తాంశం, curlx_dyn_len(&dbuf), FALSE);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
    curlx_dyn_free(&dbuf);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  ప్రవాహం->close_handled = TRUE;
  ఫలితం = CURLE_OK;

బహిర్గతం:
  CURL_TRC_CF(దత్తాంశం, cf, "handle_stream_close -> %d, %zu", (పూర్ణాంకం)ఫలితం, *pnlen);
  తిరిగి ఫలితం;
}

స్థిర పూర్ణాంకం sweight_wanted(స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  తిరిగి దత్తాంశం->సమితి.బరువు ?
    దత్తాంశం->సమితి.బరువు : NGHTTP2_DEFAULT_బరువు;
}

స్థిర పూర్ణాంకం sweight_లోపల_effect(స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  తిరిగి దత్తాంశం->state.బరువు ?
    దత్తాంశం->state.బరువు : NGHTTP2_DEFAULT_బరువు;
}

/*
 * h2_pri_spec() fills in the pri_spec struct, used by nghttp2 to send weight
 * and dependency to the peer. It also stores the updated values in the state
 * struct.
 */

స్థిర శూన్య h2_pri_spec(నిర్మాణం Curl_easy *దత్తాంశం,
                        nghttp2_ప్రాధాన్యత_spec *pri_spec)
{
  పూర్ణాంకం prio = దత్తాంశం->సమితి.బరువు;
  nghttp2_ప్రాధాన్యత_spec_init(pri_spec, 0, sweight_wanted(దత్తాంశం), FALSE);
  దత్తాంశం->state.బరువు = prio;
}

/*
 * Check if there is been an update in the priority /
 * dependency settings and if so it submits a PRIORITY frame with the updated
 * info.
 * Flush any out data pending in the network buffer.
 */
స్థిర CURLcode h2_పురోగతి_egress(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  పూర్ణాంకం rv = 0;

  ఒకవేళ(sweight_wanted(దత్తాంశం) != sweight_లోపల_effect(దత్తాంశం)) {
    నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం = H2_ప్రవాహం_CTX(ctx, దత్తాంశం);
    ఒకవేళ(ప్రవాహం && ప్రవాహం->id > 0 &&
       (sweight_wanted(దత్తాంశం) != sweight_లోపల_effect(దత్తాంశం))) {
      /* send new weight and/or dependency */
      nghttp2_ప్రాధాన్యత_spec pri_spec;

      h2_pri_spec(దత్తాంశం, &pri_spec);
      CURL_TRC_CF(దత్తాంశం, cf, "[%d] Queuing PRIORITY", ప్రవాహం->id);
      DEBUGASSERT(ప్రవాహం->id != -1);
      rv = Curl_ngh2_submit_ప్రాధాన్యత(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, ప్రవాహం->id, &pri_spec);
      ఒకవేళ(rv)
        వెళ్ళు బహిర్గతం;
    }
  }

  ctx->nw_బహిర్గతం_blocked = 0;
  అయితే(!rv && !ctx->nw_బహిర్గతం_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

బహిర్గతం:
  ఒకవేళ(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(దత్తాంశం, cf, "nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    తిరిగి CURLE_SEND_ERROR;
  }
  /* Defer flushing during the connect phase so that the SETTINGS and
   * other initial frames are sent together with the first request.
   * Unless we are 'connect_only' where the request will never come. */
  ఒకవేళ(!cf->connected && !cf->conn->బిట్లు.connect_only)
    తిరిగి CURLE_OK;
  తిరిగి nw_బహిర్గతం_flush(cf, దత్తాంశం);
}

స్థిర CURLcode ప్రవాహం_recv(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం,
                            అక్షరం *buf, size_t len, size_t *pnread)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_AGAIN;

  (శూన్య)buf;
  (శూన్య)len;
  *pnread = 0;

  ఒకవేళ(!ప్రవాహం->xfer_ఫలితం)
    ప్రవాహం->xfer_ఫలితం = cf_h2_update_local_win(cf, దత్తాంశం, ప్రవాహం);

  ఒకవేళ(ప్రవాహం->xfer_ఫలితం) {
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] xfer write failed", ప్రవాహం->id);
    ఫలితం = ప్రవాహం->xfer_ఫలితం;
  }
  లేకపోతే ఒకవేళ(ప్రవాహం->closed) {
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] returning CLOSE", ప్రవాహం->id);
    ఫలితం = http2_handle_ప్రవాహం_close(cf, దత్తాంశం, ప్రవాహం, pnread);
  }
  లేకపోతే ఒకవేళ(ప్రవాహం->reset ||
          (ctx->conn_closed && Curl_bufq_is_ఖాళీలు(&ctx->inbufq)) ||
          (ctx->rcvd_goaway && ctx->remote_max_sid < ప్రవాహం->id)) {
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] returning ERR", ప్రవాహం->id);
    ఫలితం = దత్తాంశం->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2;
  }

  ఒకవేళ(ఫలితం && (ఫలితం != CURLE_AGAIN))
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] stream_recv(len=%zu) -> %d, %zu",
                ప్రవాహం->id, len, (పూర్ణాంకం)ఫలితం, *pnread);
  తిరిగి ఫలితం;
}

స్థిర CURLcode h2_పురోగతి_ingress(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    size_t దత్తాంశం_max_బైట్లు)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;
  size_t nread;

  ఒకవేళ(should_close_session(ctx)) {
    CURL_TRC_CF(దత్తాంశం, cf, "[0] ingress: session is closed");
    తిరిగి CURLE_HTTP2;
  }

  /* Process network input buffer first */
  ఒకవేళ(!Curl_bufq_is_ఖాళీలు(&ctx->inbufq)) {
    CURL_TRC_CF(దత్తాంశం, cf, "Process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    ఫలితం = h2_process_pending_ఆదానం(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  ఒకవేళ(!దత్తాంశం_max_బైట్లు)
    దత్తాంశం_max_బైట్లు = H2_CHUNK_కొలత;

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop due to connection close or us not processing
   * all network input */
  అయితే(!ctx->conn_closed && Curl_bufq_is_ఖాళీలు(&ctx->inbufq)) {
    నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం = H2_ప్రవాహం_CTX(ctx, దత్తాంశం);
    ఒకవేళ(ప్రవాహం && (ప్రవాహం->closed || !దత్తాంశం_max_బైట్లు)) {
      /* We would like to abort here and stop processing, so that the transfer
       * loop can handle the data/close here. This may leave data in
       * underlying buffers that will not be consumed. */
      ఒకవేళ(!cf->తదుపరి || !cf->తదుపరి->cft->has_దత్తాంశం_pending(cf->తదుపరి, దత్తాంశం))
        Curl_multi_mark_dirty(దత్తాంశం);
      ఆపు;
    }

    ఫలితం = Curl_cf_recv_bufq(cf->తదుపరి, దత్తాంశం, &ctx->inbufq, 0, &nread);
    ఒకవేళ(ఫలితం) {
      ఒకవేళ(ఫలితం != CURLE_AGAIN) {
        failf(దత్తాంశం, "Failed receiving HTTP2 data: %d(%s)", (పూర్ణాంకం)ఫలితం,
              curl_easy_strerror(ఫలితం));
        తిరిగి ఫలితం;
      }
      ఆపు;
    }
    లేకపోతే ఒకవేళ(nread == 0) {
      CURL_TRC_CF(దత్తాంశం, cf, "[0] ingress: connection closed");
      ctx->conn_closed = TRUE;
      ఆపు;
    }
    లేకపోతే {
      CURL_TRC_CF(దత్తాంశం, cf, "[0] ingress: read %zu bytes", nread);
      దత్తాంశం_max_బైట్లు = (దత్తాంశం_max_బైట్లు > nread) ? (దత్తాంశం_max_బైట్లు - nread) : 0;
    }

    ఫలితం = h2_process_pending_ఆదానం(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    CURL_TRC_CF(దత్తాంశం, cf, "[0] ingress: nw-in buffered %zu",
                Curl_bufq_len(&ctx->inbufq));
  }

  ఒకవేళ(ctx->conn_closed && Curl_bufq_is_ఖాళీలు(&ctx->inbufq)) {
    CURL_TRC_CF(దత్తాంశం, cf, "server closed %s",
                ctx->rcvd_goaway ? "with GOAWAY" : "abruptly");
    connclose(cf->conn);
  }

  CURL_TRC_CF(దత్తాంశం, cf, "[0] ingress: done");
  తిరిగి CURLE_OK;
}

స్థిర CURLcode cf_h2_recv(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                           అక్షరం *buf, size_t len, size_t *pnread)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం;
  CURLcode ఫలితం, r2;
  నిర్మాణం cf_call_దత్తాంశం save;

  ఒకవేళ(!దత్తాంశం)
    తిరిగి CURLE_HTTP2;

  ప్రవాహం = H2_ప్రవాహం_CTX(ctx, దత్తాంశం);

  *pnread = 0;
  ఒకవేళ(!ప్రవాహం) {
    /* Abnormal call sequence: either this transfer has never opened a stream
     * (unlikely) or the transfer has been done, cleaned up its resources, but
     * a read() is called anyway. It is not clear what the calling sequence
     * is for such a case. */
    failf(దత్తాంశం, "http/2 recv on a transfer never opened "
          "or already cleared, mid=%u", దత్తాంశం->mid);
    తిరిగి CURLE_HTTP2;
  }

  CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);

  ఫలితం = ప్రవాహం_recv(cf, దత్తాంశం, ప్రవాహం, buf, len, pnread);
  ఒకవేళ(ఫలితం && (ఫలితం != CURLE_AGAIN))
    వెళ్ళు బహిర్గతం;

  ఒకవేళ(ఫలితం) {
    ఫలితం = h2_పురోగతి_ingress(cf, దత్తాంశం, len);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;

    ఫలితం = ప్రవాహం_recv(cf, దత్తాంశం, ప్రవాహం, buf, len, pnread);
  }

  ఒకవేళ(*pnread > 0) {
    /* Now that we transferred this to the upper layer, we report
     * the actual amount of DATA consumed to the H2 session, so
     * that it adjusts stream flow control */
    Curl_ngh2_session_consume(&ctx->ngh2, ప్రవాహం->id, *pnread);
    ఒకవేళ(ప్రవాహం->closed) {
      CURL_TRC_CF(దత్తాంశం, cf, "[%d] DRAIN closed stream", ప్రవాహం->id);
      Curl_multi_mark_dirty(దత్తాంశం);
    }
  }

బహిర్గతం:
  r2 = h2_పురోగతి_egress(cf, దత్తాంశం);
  ఒకవేళ(r2 == CURLE_AGAIN) {
    /* pending data to send, need to be called again. Ideally, we
     * monitor the socket for POLLOUT, but when not SENDING
     * any more, we force processing of the transfer. */
    ఒకవేళ(!CURL_REQ_WANT_SEND(దత్తాంశం))
      Curl_multi_mark_dirty(దత్తాంశం);
  }
  లేకపోతే ఒకవేళ(r2 && (ఫలితం == CURLE_AGAIN)) {
    /* Do not replace a previous error result NOR a successful result
     * with an egress error. Example: the server might close the
     * connection after sending the complete response with EOS
     * and everything. Further sending may fail, but that MUST
     * not change that we had a successful response. */
    ఫలితం = r2;
  }

  CURL_TRC_CF(దత్తాంశం, cf, "[%d] cf_recv(len=%zu) -> %d, %zu, "
              "window=%d/%d, connection %d/%d",
              ప్రవాహం->id, len, (పూర్ణాంకం)ఫలితం, *pnread,
              Curl_ngh2_session_get_ప్రవాహం_effective_recv_దత్తాంశం_నిడివి(
                &ctx->ngh2, ప్రవాహం->id),
              Curl_ngh2_session_get_ప్రవాహం_effective_local_కిటికీ_కొలత(
                &ctx->ngh2, ప్రవాహం->id),
              Curl_ngh2_session_get_local_కిటికీ_కొలత(&ctx->ngh2),
              HTTP2_HUGE_కిటికీ_కొలత);

  CF_దత్తాంశం_RESTORE(cf, save);
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_h2_body_send(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం,
                                స్థిరరాశి శూన్య *buf, size_t blen, bool eos,
                                size_t *pnwritten)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  CURLcode ఫలితం;

  *pnwritten = 0;
  ఒకవేళ(ప్రవాహం->closed) {
    ఒకవేళ(ప్రవాహం->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(దత్తాంశం, cf, "[%d] discarding data"
                  "on closed stream with response", ప్రవాహం->id);
      ఒకవేళ(eos)
        ప్రవాహం->body_eos = TRUE;
      *pnwritten = blen;
      తిరిగి CURLE_OK;
    }
    /* Server closed before we got a response, this is an error */
    infof(దత్తాంశం, "stream %d closed", ప్రవాహం->id);
    తిరిగి CURLE_SEND_ERROR;
  }

  ఫలితం = Curl_bufq_write(&ప్రవాహం->sendbuf, buf, blen, pnwritten);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఒకవేళ(eos && (blen == *pnwritten))
    ప్రవాహం->body_eos = TRUE;

  ఒకవేళ(eos || !Curl_bufq_is_ఖాళీలు(&ప్రవాహం->sendbuf)) {
    /* resume the potentially suspended stream */
    ఒకవేళ(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_దత్తాంశం(&ctx->ngh2, ప్రవాహం->id)))
      తిరిగి CURLE_SEND_ERROR;
  }

  తిరిగి CURLE_OK;
}

స్థిర CURLcode h2_submit(నిర్మాణం h2_ప్రవాహం_ctx **pstream,
                          నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                          స్థిరరాశి uint8_t *buf, size_t len,
                          bool eos, size_t *pnwritten)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం = ఖాళీ;
  నిర్మాణం dynhds h2_శీర్షికలు;
  nghttp2_nv *nva = ఖాళీ;
  స్థిరరాశి శూన్య *body = ఖాళీ;
  size_t nheader, bodylen;
  nghttp2_దత్తాంశం_provider దత్తాంశం_prd;
  int32_t ప్రవాహం_id;
  nghttp2_ప్రాధాన్యత_spec pri_spec;
  size_t nwritten;
  CURLcode ఫలితం = CURLE_OK;
  uint32_t initial_win_కొలత;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_శీర్షికలు, MAX_HTTP_RESP_శీర్షిక_లెక్క, DYN_HTTP_REQUEST);

  ఫలితం = http2_దత్తాంశం_అమరిక(cf, దత్తాంశం, &ప్రవాహం);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఫలితం = Curl_h1_req_parse_read(
    &ప్రవాహం->h1, buf, len, ఖాళీ,
    !దత్తాంశం->state.http_ignorecustom ?
    CURL_EASY_STR(దత్తాంశం, STRING_CUSTOMREQUEST) : ఖాళీ,
    0, &nwritten);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  *pnwritten = nwritten;
  ఒకవేళ(!ప్రవాహం->h1.పూర్తి) {
    /* need more data */
    వెళ్ళు బహిర్గతం;
  }
  DEBUGASSERT(ప్రవాహం->h1.req);

  ఫలితం = Curl_http_req_గమ్యం_h2(&h2_శీర్షికలు, ప్రవాహం->h1.req, దత్తాంశం);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  /* no longer needed */
  Curl_h1_req_parse_free(&ప్రవాహం->h1);

  nva = Curl_dynhds_గమ్యం_nva(&h2_శీర్షికలు, &nheader);
  ఒకవేళ(!nva) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  h2_pri_spec(దత్తాంశం, &pri_spec);
  ఒకవేళ(!Curl_ngh2_session_check_request_allowed(&ctx->ngh2))
    CURL_TRC_CF(దత్తాంశం, cf, "send request NOT allowed (via nghttp2)");

  /* Check the initial windows size of the transfer (rate-limits?) and
   * send an updated settings on changes from previous value. */
  initial_win_కొలత = cf_h2_initial_win_కొలత(దత్తాంశం);
  ఒకవేళ(initial_win_కొలత != ctx->initial_win_కొలత) {
    ఫలితం = cf_h2_update_settings(ctx, initial_win_కొలత);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  ఎంపిక(దత్తాంశం->state.httpreq) {
  స్థితి HTTPREQ_POST:
  స్థితి HTTPREQ_POST_FORM:
  స్థితి HTTPREQ_POST_MIME:
  స్థితి HTTPREQ_PUT:
    దత్తాంశం_prd.read_callback = req_body_read_callback;
    దత్తాంశం_prd.ఆధారం.ptr = ఖాళీ;
    ప్రవాహం_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, &దత్తాంశం_prd, ప్రవాహం);
    ఆపు;
  అప్రమేయ:
    ప్రవాహం_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, ఖాళీ, ప్రవాహం);
  }

  ఒకవేళ(ప్రవాహం_id < 0) {
    CURL_TRC_CF(దత్తాంశం, cf, "send: nghttp2_submit_request error (%s)%d",
                nghttp2_strerror(ప్రవాహం_id), ప్రవాహం_id);
    ఫలితం = CURLE_SEND_ERROR;
    వెళ్ళు బహిర్గతం;
  }

#నిర్వచితమైతే CURLVERBOSE
#నిర్వచించు MAX_ACC 60000  /* <64KB to account for some overhead */
  ఒకవేళ(Curl_trc_is_verbose(దత్తాంశం)) {
    size_t acc = 0, i;

    infof(దత్తాంశం, "[HTTP/2] [%d] OPENED stream for %s",
          ప్రవాహం_id, Curl_bufref_ptr(&దత్తాంశం->state.url));
    కొరకు(i = 0; i < nheader; ++i) {
      acc += nva[i].namelen + nva[i].valuelen;

      infof(దత్తాంశం, "[HTTP/2] [%d] [%.*s: %.*s]", ప్రవాహం_id,
            (పూర్ణాంకం)nva[i].namelen, nva[i].name,
            (పూర్ణాంకం)nva[i].valuelen, nva[i].విలువ);
    }

    ఒకవేళ(acc > MAX_ACC) {
      infof(దత్తాంశం, "[HTTP/2] Warning: The cumulative length of all "
            "headers exceeds %d bytes and that could cause the "
            "stream to be rejected.", MAX_ACC);
    }
  }
#అయితే_ముగింపు

  ప్రవాహం->id = ప్రవాహం_id;

  body = (స్థిరరాశి అక్షరం *)buf + *pnwritten;
  bodylen = len - *pnwritten;

  ఒకవేళ(bodylen || eos) {
    size_t n;
    ఫలితం = cf_h2_body_send(cf, దత్తాంశం, ప్రవాహం, body, bodylen, eos, &n);
    ఒకవేళ(!ఫలితం)
      *pnwritten += n;
    లేకపోతే ఒకవేళ(ఫలితం == CURLE_AGAIN)
      ఫలితం = CURLE_OK;
    లేకపోతే {
      ఫలితం = CURLE_SEND_ERROR;
    }
  }

బహిర్గతం:
  CURL_TRC_CF(దత్తాంశం, cf, "[%d] submit -> %d, %zu",
              ప్రవాహం ? ప్రవాహం->id : -1, (పూర్ణాంకం)ఫలితం, *pnwritten);
  curlx_safefree(nva);
  *pstream = ప్రవాహం;
  Curl_dynhds_free(&h2_శీర్షికలు);
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_h2_send(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                           స్థిరరాశి uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం = H2_ప్రవాహం_CTX(ctx, దత్తాంశం);
  నిర్మాణం cf_call_దత్తాంశం save;
  CURLcode ఫలితం = CURLE_OK, r2;

  CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
  *pnwritten = 0;

  ఒకవేళ(!ప్రవాహం || ప్రవాహం->id == -1) {
    ఫలితం = h2_submit(&ప్రవాహం, cf, దత్తాంశం, buf, len, eos, pnwritten);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    DEBUGASSERT(ప్రవాహం);
  }
  లేకపోతే ఒకవేళ(ప్రవాహం->body_eos) {
    /* We already wrote this, but CURLE_AGAIN-ed the call due to not
     * being able to flush stream->sendbuf. Make a 0-length write
     * to trigger flushing again.
     * If this works, we report to have written `len` bytes. */
    size_t n;
    DEBUGASSERT(eos);
    ఫలితం = cf_h2_body_send(cf, దత్తాంశం, ప్రవాహం, buf, 0, eos, &n);
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] cf_body_send last CHUNK -> %d, %zu, eos=%d",
                ప్రవాహం->id, (పూర్ణాంకం)ఫలితం, n, eos);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    *pnwritten = len;
  }
  లేకపోతే {
    ఫలితం = cf_h2_body_send(cf, దత్తాంశం, ప్రవాహం, buf, len, eos, pnwritten);
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] cf_body_send(len=%zu) -> %d, %zu, eos=%d",
                ప్రవాహం->id, len, (పూర్ణాంకం)ఫలితం, *pnwritten, eos);
  }

  /* Call the nghttp2 send loop and flush to write ALL buffered data,
   * headers and/or request body completely out to the network */
  r2 = h2_పురోగతి_egress(cf, దత్తాంశం);

  /* if the stream has been closed in egress handling (nghttp2 does that
   * when it does not like the headers, for example */
  ఒకవేళ(ప్రవాహం && ప్రవాహం->closed && !ప్రవాహం->resp_hds_complete) {
    infof(దత్తాంశం, "stream %d closed", ప్రవాహం->id);
    ఫలితం = CURLE_SEND_ERROR;
    వెళ్ళు బహిర్గతం;
  }
  లేకపోతే ఒకవేళ(r2 && (r2 != CURLE_AGAIN)) {
    ఫలితం = r2;
    వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    ఒకవేళ(ప్రవాహం && ప్రవాహం->closed) {
      ఫలితం = http2_handle_ప్రవాహం_close(cf, దత్తాంశం, ప్రవాహం, pnwritten);
    }
    లేకపోతే {
      CURL_TRC_CF(దత్తాంశం, cf, "send: nothing to do in this session");
      ఫలితం = CURLE_HTTP2;
    }
  }

బహిర్గతం:
  ఒకవేళ(ప్రవాహం) {
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
                "eos=%d, h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                ప్రవాహం->id, len, (పూర్ణాంకం)ఫలితం, *pnwritten,
                ప్రవాహం->body_eos,
                Curl_ngh2_session_get_ప్రవాహం_remote_కిటికీ_కొలత(
                  &ctx->ngh2, ప్రవాహం->id),
                Curl_ngh2_session_get_remote_కిటికీ_కొలత(&ctx->ngh2),
                Curl_bufq_len(&ప్రవాహం->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  లేకపోతే {
    CURL_TRC_CF(దత్తాంశం, cf, "cf_send(len=%zu) -> %d, %zu, "
                "connection-window=%d, nw_send_buffer(%zu)",
                len, (పూర్ణాంకం)ఫలితం, *pnwritten,
                Curl_ngh2_session_get_remote_కిటికీ_కొలత(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_దత్తాంశం_RESTORE(cf, save);
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_h2_flush(నిర్మాణం Curl_cfilter *cf,
                            నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం = H2_ప్రవాహం_CTX(ctx, దత్తాంశం);
  నిర్మాణం cf_call_దత్తాంశం save;
  CURLcode ఫలితం = CURLE_OK;

  CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
  ఒకవేళ(ప్రవాహం && !Curl_bufq_is_ఖాళీలు(&ప్రవాహం->sendbuf)) {
    /* resume the potentially suspended stream */
    ఒకవేళ(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_దత్తాంశం(&ctx->ngh2, ప్రవాహం->id))) {
      ఫలితం = CURLE_SEND_ERROR;
      వెళ్ళు బహిర్గతం;
    }
  }

  ఫలితం = h2_పురోగతి_egress(cf, దత్తాంశం);

బహిర్గతం:
  ఒకవేళ(ప్రవాహం) {
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] flush -> %d, "
                "h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                ప్రవాహం->id, (పూర్ణాంకం)ఫలితం,
                Curl_ngh2_session_get_ప్రవాహం_remote_కిటికీ_కొలత(
                  &ctx->ngh2, ప్రవాహం->id),
                Curl_ngh2_session_get_remote_కిటికీ_కొలత(&ctx->ngh2),
                Curl_bufq_len(&ప్రవాహం->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  లేకపోతే {
    CURL_TRC_CF(దత్తాంశం, cf, "flush -> %d, "
                "connection-window=%d, nw_send_buffer(%zu)", (పూర్ణాంకం)ఫలితం,
                Curl_ngh2_session_get_remote_కిటికీ_కొలత(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_దత్తాంశం_RESTORE(cf, save);
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_h2_adjust_pollset(నిర్మాణం Curl_cfilter *cf,
                                     నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  నిర్మాణం cf_call_దత్తాంశం save;
  curl_socket_t sock;
  bool want_recv, want_send;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(!Curl_ngh2_valid(&ctx->ngh2))
    తిరిగి CURLE_OK;

  sock = Curl_conn_cf_get_socket(cf, దత్తాంశం);
  Curl_pollset_check(దత్తాంశం, ps, sock, &want_recv, &want_send);
  ఒకవేళ(want_recv || want_send) {
    నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం = H2_ప్రవాహం_CTX(ctx, దత్తాంశం);
    bool c_exhaust, s_exhaust;

    CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
    c_exhaust = want_send &&
                !Curl_ngh2_session_get_remote_కిటికీ_కొలత(&ctx->ngh2);
    s_exhaust = want_send && ప్రవాహం && ప్రవాహం->id >= 0 &&
                !Curl_ngh2_session_get_ప్రవాహం_remote_కిటికీ_కొలత(
                  &ctx->ngh2, ప్రవాహం->id);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                (!c_exhaust && Curl_ngh2_session_want_write(&ctx->ngh2)) ||
                !Curl_bufq_is_ఖాళీలు(&ctx->outbufq);

    ఫలితం = Curl_pollset_సమితి(దత్తాంశం, ps, sock, want_recv, want_send);
    CF_దత్తాంశం_RESTORE(cf, save);
  }
  లేకపోతే ఒకవేళ(ctx->పంపిన_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_ఖాళీలు(&ctx->outbufq);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    ఫలితం = Curl_pollset_సమితి(దత్తాంశం, ps, sock, want_recv, want_send);
    CF_దత్తాంశం_RESTORE(cf, save);
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_h2_ctx_open(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం;
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  పూర్ణాంకం rc;
  nghttp2_session_callbacks *cbs = ఖాళీ;

  DEBUGASSERT(!ctx->ngh2.s);
  DEBUGASSERT(!ctx->ngh2.fatal_rv);
  DEBUGASSERT(ctx->initialized);

  rc = nghttp2_session_callbacks_new(&cbs);
  ఒకవేళ(rc) {
    failf(దత్తాంశం, "Could not initialize nghttp2 callbacks");
    వెళ్ళు బహిర్గతం;
  }

  nghttp2_session_callbacks_సమితి_send_callback(cbs, send_callback);
  nghttp2_session_callbacks_సమితి_on_చట్రం_recv_callback(cbs, on_చట్రం_recv);
  nghttp2_session_callbacks_సమితి_on_invalid_చట్రం_recv_callback(cbs,
    cf_h2_on_invalid_చట్రం_recv);
#నిర్వచితమైతే CURLVERBOSE
  nghttp2_session_callbacks_సమితి_on_చట్రం_send_callback(cbs, on_చట్రం_send);
#అయితే_ముగింపు
  nghttp2_session_callbacks_సమితి_on_దత్తాంశం_chunk_recv_callback(
    cbs, on_దత్తాంశం_chunk_recv);
  nghttp2_session_callbacks_సమితి_on_ప్రవాహం_close_callback(cbs, on_ప్రవాహం_close);
  nghttp2_session_callbacks_సమితి_on_శీర్షిక_callback(cbs, on_శీర్షిక);
#నిర్వచితమైతే CURLVERBOSE
  nghttp2_session_callbacks_సమితి_error_callback(cbs, error_callback);
#అయితే_ముగింపు

  /* The nghttp2 session is not yet setup, do it */
  rc = h2_client_new(cf, cbs);
  ఒకవేళ(rc) {
    failf(దత్తాంశం, "Could not initialize nghttp2");
    వెళ్ళు బహిర్గతం;
  }
  ctx->max_concurrent_streams = దత్తాంశం->multi ?
    Curl_multi_max_concurrent_streams(దత్తాంశం->multi) :
    DEFAULT_MAX_CONCURRENT_STREAMS;

  ఒకవేళ(ctx->ద్వారా_h1_upgrade) {
    /* HTTP/1.1 Upgrade issued. H2 Settings have already been submitted
     * in the H1 request and we upgrade from there. This stream
     * is opened implicitly as #1. */
    uint8_t binsettings[H2_BINSETTINGS_LEN];
    ssize_t rclen;
    size_t binlen; /* length of the binsettings data */

    rclen = populate_binsettings(binsettings, దత్తాంశం);

    ఒకవేళ(!curlx_sztouz(rclen, &binlen) || !binlen) {
      failf(దత్తాంశం, "nghttp2 unexpectedly failed on pack_settings_payload");
      ఫలితం = CURLE_FAILED_INIT;
      వెళ్ళు బహిర్గతం;
    }

    ఫలితం = http2_దత్తాంశం_అమరిక(cf, దత్తాంశం, &ప్రవాహం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    DEBUGASSERT(ప్రవాహం);
    ప్రవాహం->id = 1;
    /* queue SETTINGS frame (again) */
    rc = Curl_ngh2_session_upgrade2(
      &ctx->ngh2, binsettings, binlen, దత్తాంశం->state.httpreq == HTTPREQ_తల,
      ఖాళీ);
    ఒకవేళ(rc) {
      failf(దత్తాంశం, "nghttp2_session_upgrade2() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      ఫలితం = CURLE_HTTP2;
      వెళ్ళు బహిర్గతం;
    }

    rc = Curl_ngh2_session_సమితి_ప్రవాహం_user_దత్తాంశం(
      &ctx->ngh2, ప్రవాహం->id, ప్రవాహం);
    ఒకవేళ(rc) {
      infof(దత్తాంశం, "http/2: failed to set user_data for stream %d",
            ప్రవాహం->id);
      DEBUGASSERT(0);
    }
    CURL_TRC_CF(దత్తాంశం, cf, "created session via Upgrade");
  }
  లేకపోతే {
    nghttp2_settings_నమోదు iv[H2_SETTINGS_IV_LEN];
    size_t ivlen;

    ivlen = populate_settings(iv, దత్తాంశం, ctx);
    rc = Curl_ngh2_submit_settings(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, iv, ivlen);
    ఒకవేళ(rc) {
      failf(దత్తాంశం, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      ఫలితం = CURLE_HTTP2;
      వెళ్ళు బహిర్గతం;
    }
  }

  rc = Curl_ngh2_session_సమితి_local_కిటికీ_కొలత(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, HTTP2_HUGE_కిటికీ_కొలత);
  ఒకవేళ(rc) {
    failf(దత్తాంశం, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    ఫలితం = CURLE_HTTP2;
    వెళ్ళు బహిర్గతం;
  }

  /* all set, traffic will be send on connect */
  ఫలితం = CURLE_OK;
  CURL_TRC_CF(దత్తాంశం, cf, "[0] created h2 session%s",
              ctx->ద్వారా_h1_upgrade ? " (via h1 upgrade)" : "");

బహిర్గతం:
  ఒకవేళ(cbs)
    nghttp2_session_callbacks_del(cbs);
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_h2_connect(నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *దత్తాంశం,
                              bool *పూర్తి)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం cf_call_దత్తాంశం save;
  bool మొదటి_సమయం = FALSE;

  ఒకవేళ(cf->connected) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  /* Connect the lower filters first */
  ఒకవేళ(!cf->తదుపరి->connected) {
    ఫలితం = Curl_conn_cf_connect(cf->తదుపరి, దత్తాంశం, పూర్తి);
    ఒకవేళ(ఫలితం || !*పూర్తి)
      తిరిగి ఫలితం;
  }

  *పూర్తి = FALSE;

  CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
  DEBUGASSERT(ctx->initialized);
  ఒకవేళ(!ctx->ngh2.s) {
    ఫలితం = cf_h2_ctx_open(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    మొదటి_సమయం = TRUE;
  }

  ఒకవేళ(!మొదటి_సమయం) {
    ఫలితం = h2_పురోగతి_ingress(cf, దత్తాంశం, 0);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  /* Send out our SETTINGS and ACKs and such. If that blocks, we
   * have it buffered and can count this filter as being connected */
  ఫలితం = h2_పురోగతి_egress(cf, దత్తాంశం);
  ఒకవేళ(ఫలితం && (ఫలితం != CURLE_AGAIN))
    వెళ్ళు బహిర్గతం;

  *పూర్తి = TRUE;
  cf->connected = TRUE;
  ఫలితం = CURLE_OK;

బహిర్గతం:
  CURL_TRC_CF(దత్తాంశం, cf, "cf_connect() -> %d, %d, ", (పూర్ణాంకం)ఫలితం, *పూర్తి);
  CF_దత్తాంశం_RESTORE(cf, save);
  తిరిగి ఫలితం;
}

స్థిర శూన్య cf_h2_destroy(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;

  (శూన్య)దత్తాంశం;
  ఒకవేళ(ctx) {
    cf_h2_ctx_free(ctx);
    cf->ctx = ఖాళీ;
  }
}

స్థిర CURLcode cf_h2_shutdown(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  నిర్మాణం cf_call_దత్తాంశం save;
  CURLcode ఫలితం;
  పూర్ణాంకం rv;

  ఒకవేళ(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);

  ఒకవేళ(!ctx->పంపిన_goaway) {
    ctx->పంపిన_goaway = TRUE;
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, ctx->local_max_sid, 0,
      (స్థిరరాశి uint8_t *)"shutdown", పరిమాణం("shutdown"));
    ఒకవేళ(rv) {
      failf(దత్తాంశం, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      ఫలితం = CURLE_SEND_ERROR;
      వెళ్ళు బహిర్గతం;
    }
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  ఫలితం = CURLE_OK;
  ఒకవేళ(Curl_ngh2_session_want_write(&ctx->ngh2) ||
     !Curl_bufq_is_ఖాళీలు(&ctx->outbufq))
    ఫలితం = h2_పురోగతి_egress(cf, దత్తాంశం);
  ఒకవేళ(!ఫలితం && Curl_ngh2_session_want_read(&ctx->ngh2))
    ఫలితం = h2_పురోగతి_ingress(cf, దత్తాంశం, 0);

  ఒకవేళ(ఫలితం == CURLE_AGAIN)
    ఫలితం = CURLE_OK;

  *పూర్తి = (ctx->conn_closed ||
           (!ఫలితం && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2) &&
            Curl_bufq_is_ఖాళీలు(&ctx->outbufq)));

బహిర్గతం:
  CF_దత్తాంశం_RESTORE(cf, save);
  cf->shutdown = (ఫలితం || *పూర్తి);
  తిరిగి ఫలితం;
}

స్థిర CURLcode http2_దత్తాంశం_pause(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 bool pause)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం = H2_ప్రవాహం_CTX(ctx, దత్తాంశం);

  DEBUGASSERT(దత్తాంశం);
  ఒకవేళ(ctx && Curl_ngh2_valid(&ctx->ngh2) && ప్రవాహం) {
    CURLcode ఫలితం;

    ప్రవాహం->write_paused = pause;
    ఫలితం = cf_h2_update_local_win(cf, దత్తాంశం, ప్రవాహం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;

    /* attempt to send the window update */
    (శూన్య)h2_పురోగతి_egress(cf, దత్తాంశం);

    ఒకవేళ(!pause) {
      /* Unpausing a h2 transfer, requires it to be run again. The server
       * may send new DATA on us increasing the flow window, and it may
       * not. We may have already buffered and exhausted the new window
       * by operating on things in flight during the handling of other
       * transfers. */
      Curl_multi_mark_dirty(దత్తాంశం);
    }
    CURL_TRC_CF(దత్తాంశం, cf, "[%d] stream now %spaused", ప్రవాహం->id,
                pause ? "" : "un");
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode cf_h2_cntrl(నిర్మాణం Curl_cfilter *cf,
                            నిర్మాణం Curl_easy *దత్తాంశం,
                            పూర్ణాంకం event, పూర్ణాంకం arg1, శూన్య *arg2)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం cf_call_దత్తాంశం save;

  (శూన్య)arg2;

  CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
  ఎంపిక(event) {
  స్థితి CF_CTRL_దత్తాంశం_అమరిక:
    ఆపు;
  స్థితి CF_CTRL_దత్తాంశం_PAUSE:
    ఫలితం = http2_దత్తాంశం_pause(cf, దత్తాంశం, (arg1 != 0));
    ఆపు;
  స్థితి CF_CTRL_FLUSH:
    ఫలితం = cf_h2_flush(cf, దత్తాంశం);
    ఆపు;
  స్థితి CF_CTRL_దత్తాంశం_పూర్తి:
    http2_దత్తాంశం_పూర్తి(cf, దత్తాంశం);
    ఆపు;
  స్థితి CF_CTRL_CONN_INFO_UPDATE:
    ఒకవేళ(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_చూసిన = 20;
      Curl_conn_సమితి_multiplex(cf->conn);
    }
    ఆపు;
  అప్రమేయ:
    ఆపు;
  }
  CF_దత్తాంశం_RESTORE(cf, save);
  తిరిగి ఫలితం;
}

స్థిర bool cf_h2_దత్తాంశం_pending(నిర్మాణం Curl_cfilter *cf,
                               స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;

  ఒకవేళ(ctx && !Curl_bufq_is_ఖాళీలు(&ctx->inbufq))
    తిరిగి TRUE;
  తిరిగి cf->తదుపరి ? cf->తదుపరి->cft->has_దత్తాంశం_pending(cf->తదుపరి, దత్తాంశం) : FALSE;
}

స్థిర bool cf_h2_is_alive(నిర్మాణం Curl_cfilter *cf,
                           నిర్మాణం Curl_easy *దత్తాంశం,
                           bool *ఆదానం_pending)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  bool alive;
  నిర్మాణం cf_call_దత్తాంశం save;

  *ఆదానం_pending = FALSE;
  CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
           http2_connisalive(cf, దత్తాంశం, ఆదానం_pending));
  CURL_TRC_CF(దత్తాంశం, cf, "conn alive -> %d, input_pending=%d",
              alive, *ఆదానం_pending);
  CF_దత్తాంశం_RESTORE(cf, save);
  తిరిగి alive;
}

స్థిర CURLcode cf_h2_keep_alive(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURLcode ఫలితం;
  నిర్మాణం cf_call_దత్తాంశం save;

  CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
  ఫలితం = http2_send_ping(cf, దత్తాంశం);
  CF_దత్తాంశం_RESTORE(cf, save);
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_h2_query(నిర్మాణం Curl_cfilter *cf,
                            నిర్మాణం Curl_easy *దత్తాంశం,
                            పూర్ణాంకం query, పూర్ణాంకం *pres1, శూన్య *pres2)
{
  నిర్మాణం cf_h2_ctx *ctx = cf->ctx;
  నిర్మాణం cf_call_దత్తాంశం save;
  size_t effective_max;

  ఎంపిక(query) {
  స్థితి CF_QUERY_MAX_CONCURRENT:
    DEBUGASSERT(pres1);

    CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
    ఒకవేళ(!Curl_ngh2_valid(&ctx->ngh2) ||
       !Curl_ngh2_session_check_request_allowed(&ctx->ngh2)) {
      /* the limit is what we have in use right now */
      effective_max = cf->conn->attached_xfers;
    }
    లేకపోతే {
      effective_max = ctx->max_concurrent_streams;
    }
    *pres1 = (effective_max > గరిష్ఠ_పూర్ణాంకం) ? గరిష్ఠ_పూర్ణాంకం : (పూర్ణాంకం)effective_max;
    CF_దత్తాంశం_RESTORE(cf, save);
    తిరిగి CURLE_OK;
  స్థితి CF_QUERY_ప్రవాహం_ERROR: {
    నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం = H2_ప్రవాహం_CTX(ctx, దత్తాంశం);
    *pres1 = ప్రవాహం ? (పూర్ణాంకం)ప్రవాహం->error : 0;
    తిరిగి CURLE_OK;
  }
  స్థితి CF_QUERY_NEED_FLUSH: {
    నిర్మాణం h2_ప్రవాహం_ctx *ప్రవాహం = H2_ప్రవాహం_CTX(ctx, దత్తాంశం);
    ఒకవేళ(!Curl_bufq_is_ఖాళీలు(&ctx->outbufq) ||
       (ప్రవాహం && !Curl_bufq_is_ఖాళీలు(&ప్రవాహం->sendbuf))) {
      *pres1 = TRUE;
      తిరిగి CURLE_OK;
    }
    ఆపు;
  }
  స్థితి CF_QUERY_HTTP_సంస్కరణ:
    *pres1 = 20;
    తిరిగి CURLE_OK;
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి cf->తదుపరి ?
    cf->తదుపరి->cft->query(cf->తదుపరి, దత్తాంశం, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

నిర్మాణం Curl_cftype Curl_cft_nghttp2 = {
  "HTTP/2",
  CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  CURL_LOG_LVL_NONE,
  cf_h2_destroy,
  cf_h2_connect,
  cf_h2_shutdown,
  cf_h2_adjust_pollset,
  cf_h2_దత్తాంశం_pending,
  cf_h2_send,
  cf_h2_recv,
  cf_h2_cntrl,
  cf_h2_is_alive,
  cf_h2_keep_alive,
  cf_h2_query,
};

స్థిర CURLcode http2_cfilter_కూడు(నిర్మాణం Curl_cfilter **pcf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం connectdata *conn,
                                  int8_t sockindex,
                                  bool ద్వారా_h1_upgrade)
{
  నిర్మాణం Curl_cfilter *cf = ఖాళీ;
  నిర్మాణం cf_h2_ctx *ctx;
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  DEBUGASSERT(దత్తాంశం->conn);
  ctx = curlx_calloc(1, పరిమాణం(*ctx));
  ఒకవేళ(!ctx)
    వెళ్ళు బహిర్గతం;
  cf_h2_ctx_init(ctx, ద్వారా_h1_upgrade);

  ఫలితం = Curl_cf_create(&cf, &Curl_cft_nghttp2, ctx);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ctx = ఖాళీ;
  Curl_conn_cf_కూడు(దత్తాంశం, conn, sockindex, cf);

బహిర్గతం:
  ఒకవేళ(ఫలితం)
    cf_h2_ctx_free(ctx);
  *pcf = ఫలితం ? ఖాళీ : cf;
  తిరిగి ఫలితం;
}

స్థిర CURLcode http2_cfilter_చొప్పించు_after(నిర్మాణం Curl_cfilter *cf,
                                           నిర్మాణం Curl_easy *దత్తాంశం,
                                           bool ద్వారా_h1_upgrade)
{
  నిర్మాణం Curl_cfilter *cf_h2 = ఖాళీ;
  నిర్మాణం cf_h2_ctx *ctx;
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  (శూన్య)దత్తాంశం;
  ctx = curlx_calloc(1, పరిమాణం(*ctx));
  ఒకవేళ(!ctx)
    వెళ్ళు బహిర్గతం;
  cf_h2_ctx_init(ctx, ద్వారా_h1_upgrade);

  ఫలితం = Curl_cf_create(&cf_h2, &Curl_cft_nghttp2, ctx);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ctx = ఖాళీ;
  Curl_conn_cf_చొప్పించు_after(cf, cf_h2);

బహిర్గతం:
  ఒకవేళ(ఫలితం)
    cf_h2_ctx_free(ctx);
  తిరిగి ఫలితం;
}

bool Curl_http2_may_switch(నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(Curl_conn_http_సంస్కరణ(దత్తాంశం, దత్తాంశం->conn) < 20 &&
     (దత్తాంశం->state.http_neg.wanted & CURL_HTTP_V2x) &&
     దత్తాంశం->state.http_neg.h2_పూర్వ_knowledge) {
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
    ఒకవేళ(దత్తాంశం->conn->బిట్లు.origin_is_proxy) {
      infof(దత్తాంశం, "Ignoring HTTP/2 prior knowledge due to proxy");
      తిరిగి FALSE;
    }
#అయితే_ముగింపు
    తిరిగి TRUE;
  }
  తిరిగి FALSE;
}

CURLcode Curl_http2_switch(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf;
  CURLcode ఫలితం;

  DEBUGASSERT(Curl_conn_http_సంస్కరణ(దత్తాంశం, దత్తాంశం->conn) < 20);

  ఫలితం = http2_cfilter_కూడు(&cf, దత్తాంశం, దత్తాంశం->conn, FIRSTSOCKET, FALSE);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
  CURL_TRC_CF(దత్తాంశం, cf, "switching connection to HTTP/2");

  ఒకవేళ(cf->తదుపరి) {
    bool పూర్తి;
    తిరిగి Curl_conn_cf_connect(cf, దత్తాంశం, &పూర్తి);
  }
  తిరిగి CURLE_OK;
}

CURLcode Curl_http2_switch_at(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_cfilter *cf_h2;
  CURLcode ఫలితం;

  DEBUGASSERT(Curl_conn_http_సంస్కరణ(దత్తాంశం, దత్తాంశం->conn) < 20);

  ఫలితం = http2_cfilter_చొప్పించు_after(cf, దత్తాంశం, FALSE);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  cf_h2 = cf->తదుపరి;

  ఒకవేళ(cf_h2->తదుపరి) {
    bool పూర్తి;
    తిరిగి Curl_conn_cf_connect(cf_h2, దత్తాంశం, &పూర్తి);
  }
  తిరిగి CURLE_OK;
}

CURLcode Curl_http2_upgrade(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం connectdata *conn, int8_t sockindex,
                            స్థిరరాశి అక్షరం *mem, size_t nread)
{
  నిర్మాణం Curl_cfilter *cf;
  నిర్మాణం cf_h2_ctx *ctx;
  CURLcode ఫలితం;

  DEBUGASSERT(Curl_conn_http_సంస్కరణ(దత్తాంశం, conn) < 20);

  ఫలితం = http2_cfilter_కూడు(&cf, దత్తాంశం, conn, sockindex, TRUE);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
  CURL_TRC_CF(దత్తాంశం, cf, "upgrading connection to HTTP/2");

  DEBUGASSERT(cf->cft == &Curl_cft_nghttp2);
  ctx = cf->ctx;

  దత్తాంశం->req.httpversion_పంపిన = 20; /* it is an h2 request now */
  దత్తాంశం->req.శీర్షిక = TRUE; /* we expect the real response to come in h2 */
  దత్తాంశం->req.headerline = 0; /* restart the header line counter */

  ఒకవేళ(nread > 0) {
    /* Remaining data from the protocol switch reply is already using
     * the switched protocol, ie. HTTP/2. We add that to the network
     * inbufq. */
    size_t copied;

    ఫలితం = Curl_bufq_write(&ctx->inbufq,
                             (స్థిరరాశి గుర్తులేని అక్షరం *)mem, nread, &copied);
    ఒకవేళ(ఫలితం) {
      failf(దత్తాంశం, "error on copying HTTP Upgrade response: %d", (పూర్ణాంకం)ఫలితం);
      తిరిగి CURLE_RECV_ERROR;
    }
    ఒకవేళ(copied < nread) {
      failf(దత్తాంశం, "connection buffer size could not take all data "
            "from HTTP Upgrade response header: copied=%zu, datalen=%zu",
            copied, nread);
      తిరిగి CURLE_HTTP2;
    }
    infof(దత్తాంశం, "Copied HTTP/2 data in stream buffer to connection buffer"
          " after upgrade: len=%zu", nread);
  }

  ఒకవేళ(cf->తదుపరి) {
    bool పూర్తి;
    ఫలితం = Curl_conn_cf_connect(cf, దత్తాంశం, &పూర్తి);
    ఒకవేళ(!ఫలితం)
      cf->cft->cntrl(cf, దత్తాంశం, CF_CTRL_CONN_INFO_UPDATE, 0, ఖాళీ);
  }
  తిరిగి ఫలితం;
}

/* Only call this function for a transfer that already got an HTTP/2
   CURLE_HTTP2_STREAM error! */
bool Curl_h2_http_1_1_error(నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(Curl_conn_http_సంస్కరణ(దత్తాంశం, దత్తాంశం->conn) == 20) {
    పూర్ణాంకం err = Curl_conn_get_ప్రవాహం_error(దత్తాంశం, దత్తాంశం->conn, FIRSTSOCKET);
    తిరిగి err == NGHTTP2_HTTP_1_1_REQUIRED;
  }
  తిరిగి FALSE;
}

శూన్య *Curl_nghttp2_malloc(size_t కొలత, శూన్య *user_దత్తాంశం)
{
  (శూన్య)user_దత్తాంశం;
  తిరిగి Curl_cmalloc(కొలత);
}

శూన్య Curl_nghttp2_free(శూన్య *ptr, శూన్య *user_దత్తాంశం)
{
  (శూన్య)user_దత్తాంశం;
  Curl_cfree(ptr);
}

శూన్య *Curl_nghttp2_calloc(size_t nmemb, size_t కొలత, శూన్య *user_దత్తాంశం)
{
  (శూన్య)user_దత్తాంశం;
  తిరిగి Curl_ccalloc(nmemb, కొలత);
}

శూన్య *Curl_nghttp2_realloc(శూన్య *ptr, size_t కొలత, శూన్య *user_దత్తాంశం)
{
  (శూన్య)user_దత్తాంశం;
  తిరిగి Curl_crealloc(ptr, కొలత);
}

#లేకపోతే /* CURL_DISABLE_HTTP || !USE_NGHTTP2 */

/* Satisfy external references even if http2 is not compiled in. */

అక్షరం *curl_pushheader_bynum(నిర్మాణం curl_pushheaders *h, size_t అంకె)
{
  (శూన్య)h;
  (శూన్య)అంకె;
  తిరిగి ఖాళీ;
}

అక్షరం *curl_pushheader_byname(నిర్మాణం curl_pushheaders *h, స్థిరరాశి అక్షరం *name)
{
  (శూన్య)h;
  (శూన్య)name;
  తిరిగి ఖాళీ;
}

#అయితే_ముగింపు /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
