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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
#ਅਨ੍ਤਰ੍ਭਾਵ <nghttp2/nghttp2.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bufq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "u32_ptrset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http1.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http2.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/base64.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ngh2_shim.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urlapi-int.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bufref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/dynbuf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "headers.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_share.h"

#ਜੇ NGHTTP2_ਸੰਸਕਰਣ_ਅੰਕ < 0x010f00
#ਗਲਤੀ "nghttp2 1.15.0 or greater required"
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NGHTTP2_HAS_ਸੈੱਟ_LOCAL_ਖਿੜਕੀ_ਮਾਪ 1

/* buffer dimensioning:
 * use 16K as chunk size, as that fits H2 DATA frames well */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H2_CHUNK_ਮਾਪ           (16 * 1024)
/* connection window size */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H2_CONN_ਖਿੜਕੀ_ਮਾਪ     (10 * 1024 * 1024)
/* on receiving from TLS, we prep for holding a full stream window */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H2_NW_RECV_CHUNKS       (H2_CONN_ਖਿੜਕੀ_ਮਾਪ / H2_CHUNK_ਮਾਪ)
/* on send into TLS, we want to accumulate small frames */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H2_NW_SEND_CHUNKS       1
/* this is how much we want "in flight" for a stream, unthrottled */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H2_ਧਾਰਾ_ਖਿੜਕੀ_ਮਾਪ_MAX   (10 * 1024 * 1024)
/* this is how much we want "in flight" for a stream, initially, IFF
 * nghttp2 allows us to tweak the local window size. */
#ਜੇ NGHTTP2_HAS_ਸੈੱਟ_LOCAL_ਖਿੜਕੀ_ਮਾਪ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H2_ਧਾਰਾ_ਖਿੜਕੀ_ਮਾਪ_INITIAL  (64 * 1024)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H2_ਧਾਰਾ_ਖਿੜਕੀ_ਮਾਪ_INITIAL H2_ਧਾਰਾ_ਖਿੜਕੀ_ਮਾਪ_MAX
#ਜੇ_ਅੰਤ
/* keep smaller stream upload buffer (default h2 window size) to have
 * our progress bars and "upload done" reporting closer to reality */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H2_ਧਾਰਾ_SEND_CHUNKS   ((64 * 1024) / H2_CHUNK_ਮਾਪ)
/* spare chunks we keep for a full window */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H2_ਧਾਰਾ_POOL_SPARES   (H2_CONN_ਖਿੜਕੀ_ਮਾਪ / H2_CHUNK_ਮਾਪ)

/* We need to accommodate the max number of streams with their window sizes on
 * the overall connection. Streams might become PAUSED which will block their
 * received QUOTA in the connection window. If we run out of space, the server
 * is blocked from sending us any data. See #10988 for an issue with this. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTP2_HUGE_ਖਿੜਕੀ_ਮਾਪ (100 * H2_ਧਾਰਾ_ਖਿੜਕੀ_ਮਾਪ_MAX)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H2_SETTINGS_IV_LEN 3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H2_BINSETTINGS_LEN 80

ਬਣਤਰ cf_h2_ctx {
  ਬਣਤਰ curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  ਬਣਤਰ cf_call_ਅੰਕੜੇ call_ਅੰਕੜੇ;

  ਬਣਤਰ bufq inbufq;           /* network input */
  ਬਣਤਰ bufq outbufq;          /* network output */
  ਬਣਤਰ bufc_pool ਧਾਰਾ_bufcp; /* spares for stream buffers */
  ਬਣਤਰ dynbuf scratch;        /* scratch buffer for temp use */

  ਬਣਤਰ u32_ptrset streams; /* hash of `data->mid` to `h2_stream_ctx` */
  size_t drain_ਕੁੱਲ; /* sum of all stream's UrlState drain */
  uint32_t initial_win_ਮਾਪ; /* current initial window size (settings) */
  uint32_t max_concurrent_streams;
  uint32_t goaway_error;        /* goaway error code from server */
  int32_t remote_max_sid;       /* max id processed by server */
  int32_t local_max_sid;        /* max id processed by us */
  ਦੋਆਧਾਰੀ_ਅੰਕ(initialized);
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਰਾਹੀਂ_h1_upgrade);
  ਦੋਆਧਾਰੀ_ਅੰਕ(conn_closed);
  ਦੋਆਧਾਰੀ_ਅੰਕ(rcvd_goaway);
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਭੇਜਿਆ_goaway);
  ਦੋਆਧਾਰੀ_ਅੰਕ(enable_ਧੱਕੋ);
  ਦੋਆਧਾਰੀ_ਅੰਕ(nw_ਬਾਹਰਲਾ_blocked);
};

/* How to access `call_data` from a cf_h2 filter */
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ CF_CTX_CALL_ਅੰਕੜੇ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CF_CTX_CALL_ਅੰਕੜੇ(cf) ((ਬਣਤਰ cf_h2_ctx *)(cf)->ctx)->call_ਅੰਕੜੇ

/**
 * All about the H2 internals of a stream
 */
ਬਣਤਰ h2_ਧਾਰਾ_ctx {
  ਬਣਤਰ bufq sendbuf; /* request buffer */
  ਬਣਤਰ h1_req_parser h1; /* parsing the request */
  ਬਣਤਰ dynhds resp_trailers; /* response trailer fields */
  size_t resp_hds_len; /* amount of response header bytes in recvbuf */
  curl_off_t xfer_id;  /* transfer `id` owning this stream */
  curl_off_t nrcvd_ਅੰਕੜੇ;  /* number of DATA bytes received */

  ਅੱਖਰ **ਧੱਕੋ_ਸਿਰਲੇਖਾਂ;       /* allocated array */
  size_t ਧੱਕੋ_ਸਿਰਲੇਖਾਂ_ਵਰਤਿਆ;  /* number of entries filled in */
  size_t ਧੱਕੋ_ਸਿਰਲੇਖਾਂ_alloc; /* number of entries allocated */

  int32_t id; /* HTTP/2 protocol identifier for stream */
  uint32_t mid; /* transfer `mid` owning this stream */
  uint32_t error; /* stream error code */
  CURLcode xfer_ਨਤੀਜਾ; /* Result of writing out response */
  int32_t local_ਖਿੜਕੀ_ਮਾਪ; /* the local recv window size */
  int32_t ਸਥਿਤੀ_ਕੋਡ; /* HTTP response status code */
  ਦੋਆਧਾਰੀ_ਅੰਕ(resp_hds_complete); /* we have a complete, final response */
  ਦੋਆਧਾਰੀ_ਅੰਕ(closed); /* TRUE on stream close */
  ਦੋਆਧਾਰੀ_ਅੰਕ(reset);  /* TRUE on stream reset */
  ਦੋਆਧਾਰੀ_ਅੰਕ(reset_by_server);  /* TRUE on stream reset by server */
  ਦੋਆਧਾਰੀ_ਅੰਕ(close_handled); /* TRUE if stream closure is handled by libcurl */
  ਦੋਆਧਾਰੀ_ਅੰਕ(bodystarted);
  ਦੋਆਧਾਰੀ_ਅੰਕ(body_eos);    /* the complete body has been added to `sendbuf` and
                     * is being/has been processed from there. */
  ਦੋਆਧਾਰੀ_ਅੰਕ(write_paused);  /* stream write is paused */
};

ਸਥਿਰ ਖਾਲੀ free_ਧੱਕੋ_ਸਿਰਲੇਖਾਂ(ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ)
{
  size_t i;
  ਲਈ(i = 0; i < ਧਾਰਾ->ਧੱਕੋ_ਸਿਰਲੇਖਾਂ_ਵਰਤਿਆ; i++)
    curlx_free(ਧਾਰਾ->ਧੱਕੋ_ਸਿਰਲੇਖਾਂ[i]);
  curlx_safefree(ਧਾਰਾ->ਧੱਕੋ_ਸਿਰਲੇਖਾਂ);
  ਧਾਰਾ->ਧੱਕੋ_ਸਿਰਲੇਖਾਂ_ਵਰਤਿਆ = 0;
}

ਸਥਿਰ ਖਾਲੀ h2_ਧਾਰਾ_ctx_free(ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ)
{
  Curl_bufq_free(&ਧਾਰਾ->sendbuf);
  Curl_h1_req_parse_free(&ਧਾਰਾ->h1);
  Curl_dynhds_free(&ਧਾਰਾ->resp_trailers);
  free_ਧੱਕੋ_ਸਿਰਲੇਖਾਂ(ਧਾਰਾ);
  curlx_free(ਧਾਰਾ);
}

ਸਥਿਰ ਖਾਲੀ h2_ਧਾਰਾ_hash_free(uint32_t id, ਖਾਲੀ *ਧਾਰਾ)
{
  (ਖਾਲੀ)id;
  DEBUGASSERT(ਧਾਰਾ);
  h2_ਧਾਰਾ_ctx_free((ਬਣਤਰ h2_ਧਾਰਾ_ctx *)ਧਾਰਾ);
}

ਸਥਿਰ ਖਾਲੀ cf_h2_ctx_init(ਬਣਤਰ cf_h2_ctx *ctx, bool ਰਾਹੀਂ_h1_upgrade)
{
  Curl_bufcp_init(&ctx->ਧਾਰਾ_bufcp, H2_CHUNK_ਮਾਪ, H2_ਧਾਰਾ_POOL_SPARES);
  Curl_bufq_initp(&ctx->inbufq, &ctx->ਧਾਰਾ_bufcp, H2_NW_RECV_CHUNKS, 0);
  Curl_bufq_initp(&ctx->outbufq, &ctx->ਧਾਰਾ_bufcp, H2_NW_SEND_CHUNKS, 0);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_ਸਿਰਲੇਖ);
  Curl_u32_ptrset_init(&ctx->streams, h2_ਧਾਰਾ_hash_free);
  ctx->remote_max_sid = INT32_MAX;
  ctx->ਰਾਹੀਂ_h1_upgrade = ਰਾਹੀਂ_h1_upgrade;
  ctx->initialized = TRUE;
}

ਸਥਿਰ ਖਾਲੀ cf_h2_ctx_free(ਬਣਤਰ cf_h2_ctx *ctx)
{
  ਜੇ(ctx && ctx->initialized) {
    Curl_ngh2_del(&ctx->ngh2);
    Curl_bufq_free(&ctx->inbufq);
    Curl_bufq_free(&ctx->outbufq);
    Curl_bufcp_free(&ctx->ਧਾਰਾ_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    memset(ctx, 0, ਆਕਾਰ(*ctx));
  }
  curlx_free(ctx);
}

ਸਥਿਰ uint32_t cf_h2_initial_win_ਮਾਪ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
#ਜੇ NGHTTP2_HAS_ਸੈੱਟ_LOCAL_ਖਿੜਕੀ_ਮਾਪ
  /* If the transfer has a rate-limit lower than the default initial
   * stream window size, use that. It needs to be at least 8k or servers
   * may be unhappy. */
  curl_off_t rps = Curl_rlimit_ਪ੍ਰਤੀ_ਕਦਮ(&ਅੰਕੜੇ->ਤਰੱਕੀ.dl.rlimit);
  ਜੇ((rps > 0) && (rps < H2_ਧਾਰਾ_ਖਿੜਕੀ_ਮਾਪ_INITIAL))
    ਵਾਪਸ CURLMAX((uint32_t)rps, 8192);
#ਜੇ_ਅੰਤ
  ਵਾਪਸ H2_ਧਾਰਾ_ਖਿੜਕੀ_ਮਾਪ_INITIAL;
}

ਸਥਿਰ size_t populate_settings(nghttp2_settings_ਇੰਦਰਾਜ *iv,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ cf_h2_ctx *ctx)
{
  iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
  iv[0].ਮੁੱਲ = Curl_multi_max_concurrent_streams(ਅੰਕੜੇ->multi);

  iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_ਖਿੜਕੀ_ਮਾਪ;
  iv[1].ਮੁੱਲ = cf_h2_initial_win_ਮਾਪ(ਅੰਕੜੇ);
  ਜੇ(ctx)
    ctx->initial_win_ਮਾਪ = iv[1].ਮੁੱਲ;
  iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_ਧੱਕੋ;
  iv[2].ਮੁੱਲ = !!ਅੰਕੜੇ->multi->ਧੱਕੋ_cb;

  ਵਾਪਸ 3;
}

ਸਥਿਰ ssize_t populate_binsettings(uint8_t *binsettings,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  nghttp2_settings_ਇੰਦਰਾਜ iv[H2_SETTINGS_IV_LEN];
  size_t ivlen;

  ivlen = populate_settings(iv, ਅੰਕੜੇ, ਨਿਰਾ);
  /* this returns number of bytes it wrote or a negative number on error. */
  ਵਾਪਸ nghttp2_pack_settings_payload(binsettings, H2_BINSETTINGS_LEN,
                                       iv, ivlen);
}

ਸਥਿਰ CURLcode cf_h2_update_settings(ਬਣਤਰ cf_h2_ctx *ctx,
                                      uint32_t initial_win_ਮਾਪ)
{
  nghttp2_settings_ਇੰਦਰਾਜ ਇੰਦਰਾਜ;
  ਇੰਦਰਾਜ.settings_id = NGHTTP2_SETTINGS_INITIAL_ਖਿੜਕੀ_ਮਾਪ;
  ਇੰਦਰਾਜ.ਮੁੱਲ = initial_win_ਮਾਪ;
  ਜੇ(Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, &ਇੰਦਰਾਜ, 1))
    ਵਾਪਸ CURLE_SEND_ERROR;
  ctx->initial_win_ਮਾਪ = initial_win_ਮਾਪ;
  ਵਾਪਸ CURLE_OK;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H2_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ)                                         \
  ((ਬਣਤਰ h2_ਧਾਰਾ_ctx *)(                                             \
    (ਅੰਕੜੇ) ? Curl_u32_ptrset_get(&(ctx)->streams, (ਅੰਕੜੇ)->mid) : ਨਿਰਾ))

ਸਥਿਰ ਬਣਤਰ h2_ਧਾਰਾ_ctx *h2_ਧਾਰਾ_ctx_create(ਬਣਤਰ cf_h2_ctx *ctx,
                                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ;

  ਧਾਰਾ = curlx_calloc(1, ਆਕਾਰ(*ਧਾਰਾ));
  ਜੇ(!ਧਾਰਾ)
    ਵਾਪਸ ਨਿਰਾ;

  ਧਾਰਾ->id = -1;
  ਧਾਰਾ->xfer_id = ਅੰਕੜੇ->id;
  ਧਾਰਾ->mid = ਅੰਕੜੇ->mid;
  Curl_bufq_initp(&ਧਾਰਾ->sendbuf, &ctx->ਧਾਰਾ_bufcp,
                  H2_ਧਾਰਾ_SEND_CHUNKS, BUFQ_OPT_NONE);
  Curl_h1_req_parse_init(&ਧਾਰਾ->h1, H1_PARSE_DEFAULT_MAX_ਸਤਰ_LEN);
  Curl_dynhds_init(&ਧਾਰਾ->resp_trailers,
                   MAX_HTTP_RESP_ਸਿਰਲੇਖ_ਗਿਣਤੀ, DYN_HTTP_REQUEST);
  ਧਾਰਾ->bodystarted = FALSE;
  ਧਾਰਾ->ਸਥਿਤੀ_ਕੋਡ = -1;
  ਧਾਰਾ->closed = FALSE;
  ਧਾਰਾ->close_handled = FALSE;
  ਧਾਰਾ->error = NGHTTP2_NO_ERROR;
  ਧਾਰਾ->local_ਖਿੜਕੀ_ਮਾਪ = H2_ਧਾਰਾ_ਖਿੜਕੀ_ਮਾਪ_INITIAL;
  ਧਾਰਾ->nrcvd_ਅੰਕੜੇ = 0;
  ਵਾਪਸ ਧਾਰਾ;
}

ਸਥਿਰ ਬਣਤਰ Curl_easy *cf_h2_get_ਧਾਰਾ_xfer(ਬਣਤਰ Curl_cfilter *cf,
                                               int32_t ਧਾਰਾ_id,
                                               ਬਣਤਰ h2_ਧਾਰਾ_ctx **pstream)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ =
    Curl_ngh2_session_get_ਧਾਰਾ_user_ਅੰਕੜੇ(&ctx->ngh2, ਧਾਰਾ_id);
  ਜੇ(pstream)
    *pstream = ਧਾਰਾ;
  ਜੇ(ਧਾਰਾ && (ਧਾਰਾ->mid != UINT32_MAX)) {
    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, *call_ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
    ਜੇ(!call_ਅੰਕੜੇ || !call_ਅੰਕੜੇ->multi) {
      DEBUGASSERT(0);
      ਵਾਪਸ ਨਿਰਾ;
    }
    ਅੰਕੜੇ = Curl_multi_get_easy(call_ਅੰਕੜੇ->multi, ਧਾਰਾ->mid);
    ਜੇ(ਅੰਕੜੇ && (ਅੰਕੜੇ->id == ਧਾਰਾ->xfer_id))
      ਵਾਪਸ ਅੰਕੜੇ;
  }
  ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NGHTTP2_HAS_ਸੈੱਟ_LOCAL_ਖਿੜਕੀ_ਮਾਪ
ਸਥਿਰ int32_t cf_h2_get_desired_local_win(ਬਣਤਰ Curl_cfilter *cf,
                                           ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  curl_off_t avail = Curl_rlimit_avail(&ਅੰਕੜੇ->ਤਰੱਕੀ.dl.rlimit, ਨਿਰਾ);

  (ਖਾਲੀ)cf;
  ਜੇ(avail < CURL_OFF_T_MAX) { /* limit in place */
    ਜੇ(avail <= 0)
      ਵਾਪਸ 0;
    ਨਹੀਂਤਾਂ ਜੇ(avail < H2_ਧਾਰਾ_ਖਿੜਕੀ_ਮਾਪ_MAX)
      ਵਾਪਸ (int32_t)avail;
  }
  ਵਾਪਸ H2_ਧਾਰਾ_ਖਿੜਕੀ_ਮਾਪ_MAX;
}

ਸਥਿਰ CURLcode cf_h2_update_local_win(ਬਣਤਰ Curl_cfilter *cf,
                                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  int32_t dwsize;
  ਪੂਰਨ rv;

  dwsize = (ਧਾਰਾ->write_paused || ਧਾਰਾ->xfer_ਨਤੀਜਾ) ?
           0 : cf_h2_get_desired_local_win(cf, ਅੰਕੜੇ);
  ਜੇ(dwsize != ਧਾਰਾ->local_ਖਿੜਕੀ_ਮਾਪ) {
    int32_t wsize = Curl_ngh2_session_get_ਧਾਰਾ_effective_local_ਖਿੜਕੀ_ਮਾਪ(
      &ctx->ngh2, ਧਾਰਾ->id);
    ਜੇ(dwsize > wsize) {
      rv = Curl_ngh2_session_ਸੈੱਟ_local_ਖਿੜਕੀ_ਮਾਪ(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, ਧਾਰਾ->id, dwsize);
      ਜੇ(rv) {
        failf(ਅੰਕੜੇ, "[%d] nghttp2 set_local_window_size(%d) failed: "
              "%s(%d)", ਧਾਰਾ->id, dwsize, nghttp2_strerror(rv), rv);
        ਵਾਪਸ CURLE_HTTP2;
      }
      rv = Curl_ngh2_submit_ਖਿੜਕੀ_update(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, ਧਾਰਾ->id, dwsize - wsize);
      ਜੇ(rv) {
        failf(ਅੰਕੜੇ, "[%d] nghttp2_submit_window_update() failed: "
              "%s(%d)", ਧਾਰਾ->id, nghttp2_strerror(rv), rv);
        ਵਾਪਸ CURLE_HTTP2;
      }
      ਧਾਰਾ->local_ਖਿੜਕੀ_ਮਾਪ = dwsize;
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] local window update by %d",
                  ਧਾਰਾ->id, dwsize - wsize);
    }
    ਨਹੀਂਤਾਂ {
      rv = Curl_ngh2_session_ਸੈੱਟ_local_ਖਿੜਕੀ_ਮਾਪ(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, ਧਾਰਾ->id, dwsize);
      ਜੇ(rv) {
        failf(ਅੰਕੜੇ, "[%d] nghttp2_session_set_local_window_size() failed: "
              "%s(%d)", ਧਾਰਾ->id, nghttp2_strerror(rv), rv);
        ਵਾਪਸ CURLE_HTTP2;
      }
      ਧਾਰਾ->local_ਖਿੜਕੀ_ਮਾਪ = dwsize;
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] local window size now %d",
                  ਧਾਰਾ->id, dwsize);
    }
  }
  ਵਾਪਸ CURLE_OK;
}

#ਨਹੀਂਤਾਂ /* NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

ਸਥਿਰ CURLcode cf_h2_update_local_win(ਬਣਤਰ Curl_cfilter *cf,
                                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ)
{
  (ਖਾਲੀ)cf;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)ਧਾਰਾ;
  ਵਾਪਸ CURLE_OK;
}
#ਜੇ_ਅੰਤ /* !NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

ਸਥਿਰ CURLcode http2_ਅੰਕੜੇ_ਤਿਆਰੀ(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ h2_ਧਾਰਾ_ctx **pstream)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ;

  (ਖਾਲੀ)cf;
  DEBUGASSERT(ਅੰਕੜੇ);

  ਜੇ(!ਅੰਕੜੇ)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  ਧਾਰਾ = H2_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);
  ਜੇ(ਧਾਰਾ) {
    *pstream = ਧਾਰਾ;
    ਵਾਪਸ CURLE_OK;
  }

  ਧਾਰਾ = h2_ਧਾਰਾ_ctx_create(ctx, ਅੰਕੜੇ);
  ਜੇ(!ਧਾਰਾ)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਜੇ(Curl_u32_ptrset_ਸੈੱਟ(&ctx->streams, ਅੰਕੜੇ->mid, ਧਾਰਾ)) {
    h2_ਧਾਰਾ_ctx_free(ਧਾਰਾ);
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  *pstream = ਧਾਰਾ;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode nw_ਬਾਹਰਲਾ_flush(ਬਣਤਰ Curl_cfilter *cf,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->outbufq))
    ਵਾਪਸ CURLE_OK;

  ਨਤੀਜਾ = Curl_cf_send_bufq(cf->ਅਗਲਾ, ਅੰਕੜੇ, &ctx->outbufq, ਨਿਰਾ, 0,
                             &nwritten);
  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_ਬਾਹਰਲਾ_blocked = 1;
    }
    ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਵਾਪਸ Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

ਸਥਿਰ ਖਾਲੀ http2_ਅੰਕੜੇ_ਪੂਰਾ(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ = H2_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);

  DEBUGASSERT(ctx);
  ਜੇ(!ਧਾਰਾ || !ctx->initialized)
    ਵਾਪਸ;

  ਜੇ(Curl_ngh2_valid(&ctx->ngh2)) {
    bool flush_egress = FALSE;
    /* returns error if stream not known, which is fine here */
    (ਖਾਲੀ)Curl_ngh2_session_ਸੈੱਟ_ਧਾਰਾ_user_ਅੰਕੜੇ(&ctx->ngh2, ਧਾਰਾ->id, ਨਿਰਾ);

    ਜੇ(!ਧਾਰਾ->closed && ਧਾਰਾ->id > 0) {
      /* RST_STREAM */
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] premature DATA_DONE, RST stream",
                  ਧਾਰਾ->id);
      ਧਾਰਾ->closed = TRUE;
      ਧਾਰਾ->reset = TRUE;
      Curl_ngh2_submit_rst_ਧਾਰਾ(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                  ਧਾਰਾ->id, NGHTTP2_ਧਾਰਾ_CLOSED);
      flush_egress = TRUE;
    }

    ਜੇ(flush_egress) {
      (ਖਾਲੀ)Curl_ngh2_session_send(&ctx->ngh2);
      (ਖਾਲੀ)nw_ਬਾਹਰਲਾ_flush(cf, ਅੰਕੜੇ);
    }
  }

  Curl_u32_ptrset_unset(&ctx->streams, ਅੰਕੜੇ->mid);
}

ਸਥਿਰ ਪੂਰਨ h2_client_new(ਬਣਤਰ Curl_cfilter *cf,
                         nghttp2_session_callbacks *cbs)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { ਨਿਰਾ, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  ਪੂਰਨ rc = nghttp2_option_new(&o);
  ਜੇ(rc)
    ਵਾਪਸ rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_ਸੈੱਟ_no_auto_ਖਿੜਕੀ_update(o, 1);
#ਜੇ NGHTTP2_ਸੰਸਕਰਣ_ਅੰਕ >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_ਸੈੱਟ_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#ਜੇ_ਅੰਤ
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  ਵਾਪਸ rc;
}

/*
 * Returns nonzero if current HTTP/2 session should be closed.
 */
ਸਥਿਰ ਪੂਰਨ should_close_session(ਬਣਤਰ cf_h2_ctx *ctx)
{
  ਵਾਪਸ ctx->drain_ਕੁੱਲ == 0 && !Curl_ngh2_session_want_read(&ctx->ngh2) &&
    !Curl_ngh2_session_want_write(&ctx->ngh2);
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
ਸਥਿਰ CURLcode h2_process_pending_ਨਿਵੇਸ਼(ਬਣਤਰ Curl_cfilter *cf,
                                         ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf;
  size_t blen, nread;
  ssize_t rv;

  ਜਦੋਂ(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {
    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ਅਚਲ uint8_t *)buf, blen);
    ਜੇ(!curlx_sztouz(rv, &nread)) {
      failf(ਅੰਕੜੇ, "nghttp2 recv error %zd: %s", rv, nghttp2_strerror((ਪੂਰਨ)rv));
      ਵਾਪਸ CURLE_RECV_ERROR;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    ਜੇ(Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->inbufq)) {
      ਰੋਕੋ;
    }
    ਨਹੀਂਤਾਂ {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }

  ਜੇ(Curl_ngh2_session_check_request_allowed(&ctx->ngh2) == 0) {
    /* No more requests are allowed in the current session, so
       the connection may not be reused. This is set when a
       GOAWAY frame has been received or when the limit of stream
       identifiers has been reached. */
    CURL_TRC_M(ਅੰਕੜੇ, "http/2: No new requests allowed");
    connclose(cf->conn);
  }

  ਵਾਪਸ CURLE_OK;
}

/*
 * The server may send us data at any point (e.g. PING frames). Therefore, we
 * cannot assume that an HTTP/2 socket is dead because it is readable.
 *
 * Check the lower filters first and, if successful, peek at the socket
 * and distinguish between closed and data.
 */
ਸਥਿਰ bool http2_connisalive(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              bool *ਨਿਵੇਸ਼_pending)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *ਨਿਵੇਸ਼_pending = FALSE;
  ਜੇ(!cf->ਅਗਲਾ || !cf->ਅਗਲਾ->cft->is_alive(cf->ਅਗਲਾ, ਅੰਕੜੇ, ਨਿਵੇਸ਼_pending))
    ਵਾਪਸ FALSE;

  ਜੇ(*ਨਿਵੇਸ਼_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode ਨਤੀਜਾ;
    size_t nread;

    *ਨਿਵੇਸ਼_pending = FALSE;
    ਨਤੀਜਾ = Curl_cf_recv_bufq(cf->ਅਗਲਾ, ਅੰਕੜੇ, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "connisalive, recv pending input -> %d, %zu",
                (ਪੂਰਨ)ਨਤੀਜਾ, nread);
    ਜੇ(!ਨਤੀਜਾ) {
      ਨਤੀਜਾ = h2_process_pending_ਨਿਵੇਸ਼(cf, ਅੰਕੜੇ);
      ਜੇ(ਨਤੀਜਾ)
        /* immediate error, considered dead */
        alive = FALSE;
      ਨਹੀਂਤਾਂ {
        alive = !should_close_session(ctx);
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  ਵਾਪਸ alive;
}

ਸਥਿਰ CURLcode http2_send_ping(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  ਪੂਰਨ rc;

  rc = Curl_ngh2_submit_ping(&ctx->ngh2, 0, ZERO_NULL);
  ਜੇ(rc) {
    failf(ਅੰਕੜੇ, "nghttp2_submit_ping() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    ਵਾਪਸ CURLE_HTTP2;
  }

  rc = Curl_ngh2_session_send(&ctx->ngh2);
  ਜੇ(rc) {
    failf(ਅੰਕੜੇ, "nghttp2_session_send() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    ਵਾਪਸ CURLE_SEND_ERROR;
  }
  ਵਾਪਸ CURLE_OK;
}

/*
 * Store nghttp2 version info in this buffer.
 */
ਖਾਲੀ Curl_http2_ver(ਅੱਖਰ *p, size_t len)
{
  nghttp2_info *h2 = nghttp2_ਸੰਸਕਰਣ(0);
  (ਖਾਲੀ)curl_msnprintf(p, len, "nghttp2/%s", h2->ਸੰਸਕਰਣ_str);
}

/*
 * The implementation of nghttp2_send_callback type. Here we write |data| with
 * size |length| to the network and return the number of bytes actually
 * written. See the documentation of nghttp2_send_callback for the details.
 */
ਸਥਿਰ ssize_t send_callback(nghttp2_session *h2,
                             ਅਚਲ uint8_t *buf, size_t blen, ਪੂਰਨ flags,
                             ਖਾਲੀ *userp)
{
  ਬਣਤਰ Curl_cfilter *cf = userp;
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
  size_t nwritten;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  (ਖਾਲੀ)h2;
  (ਖਾਲੀ)flags;
  DEBUGASSERT(ਅੰਕੜੇ);

  ਜੇ(!cf->connected)
    ਨਤੀਜਾ = Curl_bufq_write(&ctx->outbufq, buf, blen, &nwritten);
  ਨਹੀਂਤਾਂ
    ਨਤੀਜਾ = Curl_cf_send_bufq(cf->ਅਗਲਾ, ਅੰਕੜੇ, &ctx->outbufq, buf, blen,
                               &nwritten);

  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
      ctx->nw_ਬਾਹਰਲਾ_blocked = 1;
      ਵਾਪਸ NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(ਅੰਕੜੇ, "Failed sending HTTP2 data");
    ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  ਜੇ(!nwritten) {
    ctx->nw_ਬਾਹਰਲਾ_blocked = 1;
    ਵਾਪਸ NGHTTP2_ERR_WOULDBLOCK;
  }
  ਵਾਪਸ (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

/* We pass a pointer to this struct in the push callback, but the contents of
   the struct are hidden from the user. */
ਬਣਤਰ curl_pushheaders {
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ;
  ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ;
  ਅਚਲ nghttp2_ਧੱਕੋ_promise *ਚੌਖਟਾ;
};

/*
 * push header access function. Only to be used from within the push callback
 */
ਅੱਖਰ *curl_pushheader_bynum(ਬਣਤਰ curl_pushheaders *h, size_t ਅੰਕ)
{
  /* Verify that we got a good easy handle in the push header struct, mostly to
     detect rubbish input fast(er). */
  ਜੇ(!h || !GOOD_EASY_HANDLE(h->ਅੰਕੜੇ))
    ਵਾਪਸ ਨਿਰਾ;
  ਨਹੀਂਤਾਂ {
    ਜੇ(h->ਧਾਰਾ && ਅੰਕ < h->ਧਾਰਾ->ਧੱਕੋ_ਸਿਰਲੇਖਾਂ_ਵਰਤਿਆ)
      ਵਾਪਸ h->ਧਾਰਾ->ਧੱਕੋ_ਸਿਰਲੇਖਾਂ[ਅੰਕ];
  }
  ਵਾਪਸ ਨਿਰਾ;
}

/*
 * push header access function. Only to be used from within the push callback
 */
ਅੱਖਰ *curl_pushheader_byname(ਬਣਤਰ curl_pushheaders *h, ਅਚਲ ਅੱਖਰ *name)
{
  ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ;
  size_t len;
  size_t i;
  /* Verify that we got a good easy handle in the push header struct,
     mostly to detect rubbish input fast(er). Also empty header name
     is rubbish too. We have to allow ":" at the beginning of
     the header, but header == ":" must be rejected. If we have ':' in
     the middle of header, it could be matched in middle of the value,
     this is because we do prefix match.*/
  ਜੇ(!h || !GOOD_EASY_HANDLE(h->ਅੰਕੜੇ) || !name || !name[0] ||
     !strcmp(name, ":") || strchr(name + 1, ':'))
    ਵਾਪਸ ਨਿਰਾ;

  ਧਾਰਾ = h->ਧਾਰਾ;
  ਜੇ(!ਧਾਰਾ)
    ਵਾਪਸ ਨਿਰਾ;

  len = ਲੜੀ_ਲੰਬਾਈ(name);
  ਲਈ(i = 0; i < ਧਾਰਾ->ਧੱਕੋ_ਸਿਰਲੇਖਾਂ_ਵਰਤਿਆ; i++) {
    ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(name, ਧਾਰਾ->ਧੱਕੋ_ਸਿਰਲੇਖਾਂ[i], len)) {
      /* sub-match, make sure that it is followed by a colon */
      ਜੇ(ਧਾਰਾ->ਧੱਕੋ_ਸਿਰਲੇਖਾਂ[i][len] != ':')
        ਜਾਰੀ;
      ਵਾਪਸ &ਧਾਰਾ->ਧੱਕੋ_ਸਿਰਲੇਖਾਂ[i][len + 1];
    }
  }
  ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ ਬਣਤਰ Curl_easy *h2_duphandle(ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_easy *ਦੂਜਾ = curl_easy_init();
  ਜੇ(ਦੂਜਾ) {
    ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਦੂਜਾ_ਧਾਰਾ;
    http2_ਅੰਕੜੇ_ਤਿਆਰੀ(cf, ਦੂਜਾ, &ਦੂਜਾ_ਧਾਰਾ);
    ਦੂਜਾ->state.ਭਾਰ = ਅੰਕੜੇ->state.ਭਾਰ;
    ਜੇ(ਅੰਕੜੇ->share)
      (ਖਾਲੀ)Curl_share_easy_link(ਦੂਜਾ, ਅੰਕੜੇ->share);
    /* copy a range of options from the parent handle */
    ਦੂਜਾ->ਸੈੱਟ.fwrite_func = ਅੰਕੜੇ->ਸੈੱਟ.fwrite_func;
    ਦੂਜਾ->ਸੈੱਟ.ਬਾਹਰਲਾ = ਅੰਕੜੇ->ਸੈੱਟ.ਬਾਹਰਲਾ;
    ਦੂਜਾ->ਸੈੱਟ.fwrite_ਸਿਰਲੇਖ = ਅੰਕੜੇ->ਸੈੱਟ.fwrite_ਸਿਰਲੇਖ;
    ਦੂਜਾ->ਸੈੱਟ.writeheader = ਅੰਕੜੇ->ਸੈੱਟ.writeheader;
    ਦੂਜਾ->ਸੈੱਟ.fdebug = ਅੰਕੜੇ->ਸੈੱਟ.fdebug;
    ਦੂਜਾ->ਸੈੱਟ.debugdata = ਅੰਕੜੇ->ਸੈੱਟ.debugdata;
    ਦੂਜਾ->ਸੈੱਟ.fxferinfo = ਅੰਕੜੇ->ਸੈੱਟ.fxferinfo;
    ਦੂਜਾ->ਸੈੱਟ.fprogress = ਅੰਕੜੇ->ਸੈੱਟ.fprogress;
    ਦੂਜਾ->ਸੈੱਟ.ਤਰੱਕੀ_client = ਅੰਕੜੇ->ਸੈੱਟ.ਤਰੱਕੀ_client;
    ਦੂਜਾ->ਤਰੱਕੀ.hide = ਅੰਕੜੇ->ਤਰੱਕੀ.hide;
    ਦੂਜਾ->ਤਰੱਕੀ.callback = ਅੰਕੜੇ->ਤਰੱਕੀ.callback;
  }
  ਵਾਪਸ ਦੂਜਾ;
}

ਸਥਿਰ ਪੂਰਨ ਸੈੱਟ_transfer_url(ਬਣਤਰ Curl_easy *newhandle,
                            ਬਣਤਰ curl_pushheaders *hp,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਅਚਲ ਅੱਖਰ *v;
  CURLUcode uc;
  ਅੱਖਰ *url = ਨਿਰਾ;
  ਪੂਰਨ rc = 0;
  CURLU *u = curl_url();

  ਜੇ(!u)
    ਵਾਪਸ 5;

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_SCHEME);
  ਜੇ(v) {
    uc = curl_url_ਸੈੱਟ(u, CURLUPART_SCHEME, v, 0);
    ਜੇ(uc) {
      rc = 1;
      ਜਾਓ fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_AUTHORITY);
  ਜੇ(v) {
    uc = Curl_url_ਸੈੱਟ_authority(u, v);
    ਜੇ(uc) {
      rc = 2;
      ਜਾਓ fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_ਰਸਤਾ);
  ਜੇ(v) {
    uc = curl_url_ਸੈੱਟ(u, CURLUPART_ਰਸਤਾ, v, 0);
    ਜੇ(uc) {
      rc = 3;
      ਜਾਓ fail;
    }
  }

  /* We can only allow PUSH of resource from the same origin, e.g.
   * scheme + hostname + port */
  ਜੇ(!Curl_url_same_origin(ਅੰਕੜੇ->state.uh, u)) {
    rc = 1;
    ਜਾਓ fail;
  }

  uc = curl_url_get(u, CURLUPART_URL, &url, 0);
  ਜੇ(uc)
    rc = 4;
fail:
  ਜੇ(rc) {
    curl_url_cleanup(u);
    ਵਾਪਸ rc;
  }

  DEBUGASSERT(!newhandle->state.uh);
  newhandle->state.uh = u;
  Curl_bufref_ਸੈੱਟ(&newhandle->state.url, url, 0, curl_free);
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ discard_newhandle(ਬਣਤਰ Curl_cfilter *cf,
                              ਬਣਤਰ Curl_easy *newhandle)
{
  http2_ਅੰਕੜੇ_ਪੂਰਾ(cf, newhandle);
  (ਖਾਲੀ)Curl_close(&newhandle);
}

ਸਥਿਰ ਪੂਰਨ ਧੱਕੋ_promise(ਬਣਤਰ Curl_cfilter *cf,
                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਅਚਲ nghttp2_ਧੱਕੋ_promise *ਚੌਖਟਾ)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  ਪੂਰਨ rv; /* one of the CURL_PUSH_* defines */

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] PUSH_PROMISE received",
              ਚੌਖਟਾ->promised_ਧਾਰਾ_id);
  ਜੇ(ਅੰਕੜੇ->multi->ਧੱਕੋ_cb) {
    ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ;
    ਬਣਤਰ h2_ਧਾਰਾ_ctx *newstream;
    ਬਣਤਰ curl_pushheaders heads;
    CURLMcode mresult;
    CURLcode ਨਤੀਜਾ;
    /* clone the parent */
    ਬਣਤਰ Curl_easy *newhandle = h2_duphandle(cf, ਅੰਕੜੇ);
    ਜੇ(!newhandle) {
      infof(ਅੰਕੜੇ, "failed to duplicate handle");
      rv = CURL_ਧੱਕੋ_DENY; /* FAIL HARD */
      ਜਾਓ fail;
    }

    ਧਾਰਾ = H2_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);
    ਜੇ(!ਧਾਰਾ) {
      failf(ਅੰਕੜੇ, "Internal NULL stream");
      discard_newhandle(cf, newhandle);
      rv = CURL_ਧੱਕੋ_DENY;
      ਜਾਓ fail;
    }

    heads.ਅੰਕੜੇ = ਅੰਕੜੇ;
    heads.ਧਾਰਾ = ਧਾਰਾ;
    heads.ਚੌਖਟਾ = ਚੌਖਟਾ;

    rv = ਸੈੱਟ_transfer_url(newhandle, &heads, ਅੰਕੜੇ);
    ਜੇ(rv) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] PUSH_PROMISE, failed to set URL -> %d",
                  ਚੌਖਟਾ->promised_ਧਾਰਾ_id, rv);
      discard_newhandle(cf, newhandle);
      rv = CURL_ਧੱਕੋ_DENY;
      ਜਾਓ fail;
    }

    {
      ਬਣਤਰ Curl_mapi_guard guard;
      CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, multi_ਧੱਕੋ_cb);
      rv = ਅੰਕੜੇ->multi->ਧੱਕੋ_cb(ਅੰਕੜੇ, newhandle,
                                ਧਾਰਾ->ਧੱਕੋ_ਸਿਰਲੇਖਾਂ_ਵਰਤਿਆ, &heads,
                                ਅੰਕੜੇ->multi->ਧੱਕੋ_userp);
      CURL_CBAPI_ਸਿਰਾ(&guard);
    }

    /* free the headers again */
    free_ਧੱਕੋ_ਸਿਰਲੇਖਾਂ(ਧਾਰਾ);

    ਜੇ(rv) {
      DEBUGASSERT((rv > CURL_ਧੱਕੋ_OK) && (rv <= CURL_ਧੱਕੋ_ERROROUT));
      /* denied, kill off the new handle again */
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] PUSH_PROMISE, denied by application -> %d",
                  ਚੌਖਟਾ->promised_ਧਾਰਾ_id, rv);
      discard_newhandle(cf, newhandle);
      ਜਾਓ fail;
    }

    /* approved, add to the multi handle for processing. This
     * assigns newhandle->mid. For the new `mid` we assign the
     * h2_stream instance and remember the stream_id already known. */
    mresult = Curl_multi_ਜੋੜੋ_perform(ਅੰਕੜੇ->multi, newhandle, cf->conn);
    ਜੇ(mresult) {
      infof(ਅੰਕੜੇ, "failed to add handle to multi");
      discard_newhandle(cf, newhandle);
      rv = CURL_ਧੱਕੋ_DENY;
      ਜਾਓ fail;
    }

    ਨਤੀਜਾ = http2_ਅੰਕੜੇ_ਤਿਆਰੀ(cf, newhandle, &newstream);
    ਜੇ(ਨਤੀਜਾ) {
      failf(ਅੰਕੜੇ, "error setting up stream: %d", (ਪੂਰਨ)ਨਤੀਜਾ);
      discard_newhandle(cf, newhandle);
      rv = CURL_ਧੱਕੋ_DENY;
      ਜਾਓ fail;
    }

    DEBUGASSERT(newstream);
    newstream->id = ਚੌਖਟਾ->promised_ਧਾਰਾ_id;
    newhandle->req.maxdownload = -1;
    newhandle->req.ਮਾਪ = -1;

    CURL_TRC_CF(ਅੰਕੜੇ, cf, "promise easy handle added to multi, mid=%u",
                newhandle->mid);
    rv = Curl_ngh2_session_ਸੈੱਟ_ਧਾਰਾ_user_ਅੰਕੜੇ(
      &ctx->ngh2, newstream->id, newstream);
    ਜੇ(rv) {
      infof(ਅੰਕੜੇ, "failed to set user_data for stream %d",
            newstream->id);
      DEBUGASSERT(0);
      discard_newhandle(cf, newhandle);
      rv = CURL_ਧੱਕੋ_DENY;
      ਜਾਓ fail;
    }

    /* success, remember max stream id processed */
    ਜੇ(newstream->id > ctx->local_max_sid)
      ctx->local_max_sid = newstream->id;
  }
  ਨਹੀਂਤਾਂ {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "Got PUSH_PROMISE, ignore it");
    rv = CURL_ਧੱਕੋ_DENY;
  }
fail:
  ਵਾਪਸ rv;
}

ਸਥਿਰ ਖਾਲੀ h2_xfer_write_resp_hd(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ,
                                  ਅਚਲ ਅੱਖਰ *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  ਜੇ(!ਧਾਰਾ->xfer_ਨਤੀਜਾ) {
    ਧਾਰਾ->xfer_ਨਤੀਜਾ = Curl_xfer_write_resp_hd(ਅੰਕੜੇ, buf, blen, eos);
    ਜੇ(!ਧਾਰਾ->xfer_ਨਤੀਜਾ && !eos)
      ਧਾਰਾ->xfer_ਨਤੀਜਾ = cf_h2_update_local_win(cf, ਅੰਕੜੇ, ਧਾਰਾ);
    ਜੇ(ਧਾਰਾ->xfer_ਨਤੀਜਾ)
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] error %d writing %zu bytes of headers",
                  ਧਾਰਾ->id, (ਪੂਰਨ)ਧਾਰਾ->xfer_ਨਤੀਜਾ, blen);
  }
}

ਸਥਿਰ ਖਾਲੀ h2_xfer_write_resp(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ,
                               ਅਚਲ ਅੱਖਰ *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  ਜੇ(!ਧਾਰਾ->xfer_ਨਤੀਜਾ)
    ਧਾਰਾ->xfer_ਨਤੀਜਾ = Curl_xfer_write_resp(ਅੰਕੜੇ, buf, blen, eos);
  /* If the transfer write is errored, we do not want any more data */
  ਜੇ(ਧਾਰਾ->xfer_ਨਤੀਜਾ) {
    ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] error %d writing %zu bytes of data, "
                "RST-ing stream",
                ਧਾਰਾ->id, (ਪੂਰਨ)ਧਾਰਾ->xfer_ਨਤੀਜਾ, blen);
    Curl_ngh2_submit_rst_ਧਾਰਾ(
      &ctx->ngh2, 0, ਧਾਰਾ->id, (uint32_t)NGHTTP2_ERR_CALLBACK_FAILURE);
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਧਾਰਾ->write_paused && Curl_xfer_write_is_paused(ਅੰਕੜੇ)) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] stream output paused", ਧਾਰਾ->id);
    ਧਾਰਾ->write_paused = TRUE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਧਾਰਾ->write_paused && !Curl_xfer_write_is_paused(ਅੰਕੜੇ)) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] stream output unpaused", ਧਾਰਾ->id);
    ਧਾਰਾ->write_paused = FALSE;
  }

  ਜੇ(!ਧਾਰਾ->xfer_ਨਤੀਜਾ && !eos)
    ਧਾਰਾ->xfer_ਨਤੀਜਾ = cf_h2_update_local_win(cf, ਅੰਕੜੇ, ਧਾਰਾ);
}

ਸਥਿਰ CURLcode on_ਧਾਰਾ_ਚੌਖਟਾ(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ,
                                ਅਚਲ nghttp2_ਚੌਖਟਾ *ਚੌਖਟਾ)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  int32_t ਧਾਰਾ_id = ਚੌਖਟਾ->hd.ਧਾਰਾ_id;
  ਪੂਰਨ rv;

  ਜੇ(!ਧਾਰਾ) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] No stream_ctx set", ਧਾਰਾ_id);
    ਵਾਪਸ CURLE_FAILED_INIT;
  }

  ਚੋਣ(ਚੌਖਟਾ->hd.type) {
  ਹਾਲਤ NGHTTP2_ਅੰਕੜੇ:
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] DATA, window=%d/%d",
                ਧਾਰਾ_id,
                Curl_ngh2_session_get_ਧਾਰਾ_effective_recv_ਅੰਕੜੇ_ਲੰਮਾਈ(
                  &ctx->ngh2, ਧਾਰਾ->id),
                Curl_ngh2_session_get_ਧਾਰਾ_effective_local_ਖਿੜਕੀ_ਮਾਪ(
                  &ctx->ngh2, ਧਾਰਾ->id));
    /* If !body started on this stream, then receiving DATA is illegal. */
    ਜੇ(!ਧਾਰਾ->bodystarted) {
      ਜੇ(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_ਧਾਰਾ(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                       ਧਾਰਾ_id, NGHTTP2_PROTOCOL_ERROR))) {
        ਵਾਪਸ CURLE_RECV_ERROR;
      }
    }
    ਰੋਕੋ;
  ਹਾਲਤ NGHTTP2_ਸਿਰਲੇਖਾਂ:
    ਜੇ(ਧਾਰਾ->bodystarted) {
      /* Only valid HEADERS after body started is trailer HEADERS. We
         buffer them in on_header callback. */
      ਰੋਕੋ;
    }

    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    ਜੇ(ਧਾਰਾ->ਸਥਿਤੀ_ਕੋਡ == -1)
      ਵਾਪਸ CURLE_RECV_ERROR;

    /* Only final status code signals the end of header */
    ਜੇ(ਧਾਰਾ->ਸਥਿਤੀ_ਕੋਡ / 100 != 1) {
      ਧਾਰਾ->resp_hds_complete = TRUE;
      ਧਾਰਾ->bodystarted = TRUE;
    }
    ਨਹੀਂਤਾਂ
      ਧਾਰਾ->ਸਥਿਤੀ_ਕੋਡ = -1;

    h2_xfer_write_resp_hd(cf, ਅੰਕੜੇ, ਧਾਰਾ, STRCONST("\r\n"),
                          (bool)ਧਾਰਾ->closed);

    Curl_multi_mark_dirty(ਅੰਕੜੇ);
    ਰੋਕੋ;
  ਹਾਲਤ NGHTTP2_ਧੱਕੋ_PROMISE:
    rv = ਧੱਕੋ_promise(cf, ਅੰਕੜੇ, &ਚੌਖਟਾ->ਧੱਕੋ_promise);
    ਜੇ(rv) { /* deny! */
      DEBUGASSERT((rv > CURL_ਧੱਕੋ_OK) && (rv <= CURL_ਧੱਕੋ_ERROROUT));
      ਜੇ(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_ਧਾਰਾ(&ctx->ngh2, NGHTTP2_FLAG_NONE,
             ਚੌਖਟਾ->ਧੱਕੋ_promise.promised_ਧਾਰਾ_id, NGHTTP2_CANCEL)))
        ਵਾਪਸ CURLE_SEND_ERROR;
      ਨਹੀਂਤਾਂ ਜੇ(rv == CURL_ਧੱਕੋ_ERROROUT) {
        /* fail the parent transfer, not the whole session */
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] fail in PUSH_PROMISE received",
                    ਧਾਰਾ_id);
        ਜੇ(!ਧਾਰਾ->xfer_ਨਤੀਜਾ)
          ਧਾਰਾ->xfer_ਨਤੀਜਾ = CURLE_RECV_ERROR;
        Curl_multi_mark_dirty(ਅੰਕੜੇ);
      }
    }
    ਰੋਕੋ;
  ਹਾਲਤ NGHTTP2_RST_ਧਾਰਾ:
    ਜੇ(ਚੌਖਟਾ->rst_ਧਾਰਾ.error_ਕੋਡ)
      ਧਾਰਾ->reset_by_server = TRUE;
    Curl_multi_mark_dirty(ਅੰਕੜੇ);
    ਰੋਕੋ;
  ਹਾਲਤ NGHTTP2_ਖਿੜਕੀ_UPDATE:
    ਜੇ(CURL_REQ_WANT_SEND(ਅੰਕੜੇ) && Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ਧਾਰਾ->sendbuf)) {
      /* need more data, force processing of transfer */
      Curl_multi_mark_dirty(ਅੰਕੜੇ);
    }
    ਨਹੀਂਤਾਂ ਜੇ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ਧਾਰਾ->sendbuf)) {
      /* resume the potentially suspended stream */
      ਜੇ(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_session_resume_ਅੰਕੜੇ(&ctx->ngh2, ਧਾਰਾ->id)))
        ਵਾਪਸ CURLE_SEND_ERROR;
    }
    ਰੋਕੋ;
  ਮੂਲ:
    ਰੋਕੋ;
  }

  ਜੇ(ਚੌਖਟਾ->hd.flags & NGHTTP2_FLAG_ਸਿਰਾ_ਧਾਰਾ) {
    ਜੇ(!ਧਾਰਾ->closed && !ਧਾਰਾ->body_eos &&
       ((ਧਾਰਾ->ਸਥਿਤੀ_ਕੋਡ >= 400) || (ਧਾਰਾ->ਸਥਿਤੀ_ਕੋਡ < 200))) {
      /* The server did not give us a positive response and we are not
       * done uploading the request body. We need to stop doing that and
       * also inform the server that we aborted our side. */
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] EOS frame with unfinished upload and "
                  "HTTP status %d, abort upload by RST",
                  ਧਾਰਾ_id, ਧਾਰਾ->ਸਥਿਤੀ_ਕੋਡ);
      Curl_ngh2_submit_rst_ਧਾਰਾ(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, ਧਾਰਾ->id, NGHTTP2_ਧਾਰਾ_CLOSED);
      ਧਾਰਾ->closed = TRUE;
    }
    Curl_multi_mark_dirty(ਅੰਕੜੇ);
  }
  ਵਾਪਸ CURLE_OK;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਪੂਰਨ Curl_nghttp2_fr_print(ਅਚਲ nghttp2_ਚੌਖਟਾ *ਚੌਖਟਾ, ਅੱਖਰ *ਬਫਰ,
                          size_t blen)
{
  ਚੋਣ(ਚੌਖਟਾ->hd.type) {
  ਹਾਲਤ NGHTTP2_ਅੰਕੜੇ: {
    ਵਾਪਸ curl_msnprintf(ਬਫਰ, blen,
                          "FRAME[DATA, len=%d, eos=%d, padlen=%d]",
                          (ਪੂਰਨ)ਚੌਖਟਾ->hd.ਲੰਮਾਈ,
                          !!(ਚੌਖਟਾ->hd.flags & NGHTTP2_FLAG_ਸਿਰਾ_ਧਾਰਾ),
                          (ਪੂਰਨ)ਚੌਖਟਾ->ਅੰਕੜੇ.padlen);
  }
  ਹਾਲਤ NGHTTP2_ਸਿਰਲੇਖਾਂ: {
    ਵਾਪਸ curl_msnprintf(ਬਫਰ, blen,
                          "FRAME[HEADERS, len=%d, hend=%d, eos=%d]",
                          (ਪੂਰਨ)ਚੌਖਟਾ->hd.ਲੰਮਾਈ,
                          !!(ਚੌਖਟਾ->hd.flags & NGHTTP2_FLAG_ਸਿਰਾ_ਸਿਰਲੇਖਾਂ),
                          !!(ਚੌਖਟਾ->hd.flags & NGHTTP2_FLAG_ਸਿਰਾ_ਧਾਰਾ));
  }
  ਹਾਲਤ NGHTTP2_ਤਰਜੀਹ: {
    ਵਾਪਸ curl_msnprintf(ਬਫਰ, blen,
                          "FRAME[PRIORITY, len=%d, flags=%d]",
                          (ਪੂਰਨ)ਚੌਖਟਾ->hd.ਲੰਮਾਈ, ਚੌਖਟਾ->hd.flags);
  }
  ਹਾਲਤ NGHTTP2_RST_ਧਾਰਾ: {
    ਵਾਪਸ curl_msnprintf(ਬਫਰ, blen,
                          "FRAME[RST_STREAM, len=%d, flags=%d, error=%u]",
                          (ਪੂਰਨ)ਚੌਖਟਾ->hd.ਲੰਮਾਈ, ਚੌਖਟਾ->hd.flags,
                          ਚੌਖਟਾ->rst_ਧਾਰਾ.error_ਕੋਡ);
  }
  ਹਾਲਤ NGHTTP2_SETTINGS: {
    ਜੇ(ਚੌਖਟਾ->hd.flags & NGHTTP2_FLAG_ACK) {
      ਵਾਪਸ curl_msnprintf(ਬਫਰ, blen, "FRAME[SETTINGS, ack=1]");
    }
    ਵਾਪਸ curl_msnprintf(ਬਫਰ, blen,
                          "FRAME[SETTINGS, len=%d]", (ਪੂਰਨ)ਚੌਖਟਾ->hd.ਲੰਮਾਈ);
  }
  ਹਾਲਤ NGHTTP2_ਧੱਕੋ_PROMISE:
    ਵਾਪਸ curl_msnprintf(ਬਫਰ, blen,
                          "FRAME[PUSH_PROMISE, len=%d, hend=%d]",
                          (ਪੂਰਨ)ਚੌਖਟਾ->hd.ਲੰਮਾਈ,
                          !!(ਚੌਖਟਾ->hd.flags & NGHTTP2_FLAG_ਸਿਰਾ_ਸਿਰਲੇਖਾਂ));
  ਹਾਲਤ NGHTTP2_PING:
    ਵਾਪਸ curl_msnprintf(ਬਫਰ, blen,
                          "FRAME[PING, len=%d, ack=%d]",
                          (ਪੂਰਨ)ਚੌਖਟਾ->hd.ਲੰਮਾਈ,
                          ਚੌਖਟਾ->hd.flags & NGHTTP2_FLAG_ACK);
  ਹਾਲਤ NGHTTP2_GOAWAY: {
    ਅੱਖਰ scratch[128];
    size_t s_len = CURL_ARRAYSIZE(scratch);
    size_t len = (ਚੌਖਟਾ->goaway.opaque_ਅੰਕੜੇ_len < s_len) ?
      ਚੌਖਟਾ->goaway.opaque_ਅੰਕੜੇ_len : s_len - 1;
    ਜੇ(len)
      ਸਮ੍ਰਿਤੀ_ਨਕਲ(scratch, ਚੌਖਟਾ->goaway.opaque_ਅੰਕੜੇ, len);
    scratch[len] = '\0';
    ਵਾਪਸ curl_msnprintf(ਬਫਰ, blen,
                          "FRAME[GOAWAY, error=%u, reason='%s', "
                          "last_stream=%d]", ਚੌਖਟਾ->goaway.error_ਕੋਡ,
                          scratch, ਚੌਖਟਾ->goaway.ਆਖਰੀ_ਧਾਰਾ_id);
  }
  ਹਾਲਤ NGHTTP2_ਖਿੜਕੀ_UPDATE: {
    ਵਾਪਸ curl_msnprintf(ਬਫਰ, blen,
                          "FRAME[WINDOW_UPDATE, incr=%d]",
                          ਚੌਖਟਾ->ਖਿੜਕੀ_update.ਖਿੜਕੀ_ਮਾਪ_increment);
  }
  ਮੂਲ:
    ਵਾਪਸ curl_msnprintf(ਬਫਰ, blen, "FRAME[%d, len=%d, flags=%d]",
                          ਚੌਖਟਾ->hd.type, (ਪੂਰਨ)ਚੌਖਟਾ->hd.ਲੰਮਾਈ,
                          ਚੌਖਟਾ->hd.flags);
  }
}

ਸਥਿਰ ਪੂਰਨ on_ਚੌਖਟਾ_send(nghttp2_session *session, ਅਚਲ nghttp2_ਚੌਖਟਾ *ਚੌਖਟਾ,
                         ਖਾਲੀ *userp)
{
  ਬਣਤਰ Curl_cfilter *cf = userp;
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);

  (ਖਾਲੀ)session;
  DEBUGASSERT(ਅੰਕੜੇ);
  ਜੇ(Curl_trc_cf_is_verbose(cf, ਅੰਕੜੇ)) {
    ਅੱਖਰ ਬਫਰ[256];
    ਪੂਰਨ len;
    len = Curl_nghttp2_fr_print(ਚੌਖਟਾ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ) - 1);
    ਬਫਰ[len] = 0;
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] -> %s", ਚੌਖਟਾ->hd.ਧਾਰਾ_id, ਬਫਰ);
  }
  ਜੇ((ਚੌਖਟਾ->hd.type == NGHTTP2_GOAWAY) && !ctx->ਭੇਜਿਆ_goaway) {
    /* A GOAWAY not initiated by us, but by nghttp2 itself on detecting
     * a protocol error on the connection */
    failf(ਅੰਕੜੇ, "nghttp2 shuts down connection with error %u: %s",
          ਚੌਖਟਾ->goaway.error_ਕੋਡ,
          nghttp2_http2_strerror(ਚੌਖਟਾ->goaway.error_ਕੋਡ));
  }
  ਵਾਪਸ 0;
}
#ਜੇ_ਅੰਤ /* CURLVERBOSE */

ਸਥਿਰ ਪੂਰਨ on_ਚੌਖਟਾ_recv(nghttp2_session *session, ਅਚਲ nghttp2_ਚੌਖਟਾ *ਚੌਖਟਾ,
                         ਖਾਲੀ *userp)
{
  ਬਣਤਰ Curl_cfilter *cf = userp;
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf), *ਅੰਕੜੇ_s;
  ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ;
  int32_t ਧਾਰਾ_id = ਚੌਖਟਾ->hd.ਧਾਰਾ_id;

  (ਖਾਲੀ)session;
  DEBUGASSERT(ਅੰਕੜੇ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  ਜੇ(Curl_trc_cf_is_verbose(cf, ਅੰਕੜੇ)) {
    ਅੱਖਰ ਬਫਰ[256];
    ਪੂਰਨ len;
    len = Curl_nghttp2_fr_print(ਚੌਖਟਾ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ) - 1);
    ਬਫਰ[len] = 0;
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] <- %s", ਚੌਖਟਾ->hd.ਧਾਰਾ_id, ਬਫਰ);
  }
#ਜੇ_ਅੰਤ /* CURLVERBOSE */

  ਜੇ(!ਧਾਰਾ_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(ਅੰਕੜੇ);
    ਚੋਣ(ਚੌਖਟਾ->hd.type) {
    ਹਾਲਤ NGHTTP2_SETTINGS: {
      ਜੇ(!(ਚੌਖਟਾ->hd.flags & NGHTTP2_FLAG_ACK)) {
        uint32_t max_conn = ctx->max_concurrent_streams;
        ctx->max_concurrent_streams =
          Curl_ngh2_session_get_remote_settings(
            &ctx->ngh2, NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS);
        ctx->enable_ਧੱਕੋ = Curl_ngh2_session_get_remote_settings(
          &ctx->ngh2, NGHTTP2_SETTINGS_ENABLE_ਧੱਕੋ) != 0;
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "[0] MAX_CONCURRENT_STREAMS: %u",
                    ctx->max_concurrent_streams);
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "[0] ENABLE_PUSH: %s",
                    ctx->enable_ਧੱਕੋ ? "TRUE" : "false");
        ਜੇ(ਅੰਕੜੇ && max_conn != ctx->max_concurrent_streams) {
          /* only signal change if the value actually changed */
          CURL_TRC_CF(ਅੰਕੜੇ, cf, "[0] notify MAX_CONCURRENT_STREAMS: %u",
                      ctx->max_concurrent_streams);
          Curl_multi_connchanged(ਅੰਕੜੇ->multi);
        }
        /* Since the initial stream window is 64K, a request might be on HOLD,
         * due to exhaustion. The (initial) SETTINGS may announce a much larger
         * window and *assume* that we treat this like a WINDOW_UPDATE. Some
         * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
         * To be safe, we UNHOLD a stream in order not to stall. */
        ਜੇ(CURL_REQ_WANT_SEND(ਅੰਕੜੇ))
          Curl_multi_mark_dirty(ਅੰਕੜੇ);
      }
      ਰੋਕੋ;
    }
    ਹਾਲਤ NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->goaway_error = ਚੌਖਟਾ->goaway.error_ਕੋਡ;
      ctx->remote_max_sid = ਚੌਖਟਾ->goaway.ਆਖਰੀ_ਧਾਰਾ_id;
      ਜੇ(ਅੰਕੜੇ) {
        infof(ਅੰਕੜੇ, "received GOAWAY, error=%u, last_stream=%d",
              ctx->goaway_error, ctx->remote_max_sid);
        Curl_multi_connchanged(ਅੰਕੜੇ->multi);
      }
      ਰੋਕੋ;
    ਮੂਲ:
      ਰੋਕੋ;
    }
    ਵਾਪਸ 0;
  }

  ਅੰਕੜੇ_s = cf_h2_get_ਧਾਰਾ_xfer(cf, ਧਾਰਾ_id, &ਧਾਰਾ);
  ਜੇ(!ਅੰਕੜੇ_s) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] No Curl_easy associated", ਧਾਰਾ_id);
    ਵਾਪਸ 0;
  }

  ਵਾਪਸ on_ਧਾਰਾ_ਚੌਖਟਾ(cf, ਅੰਕੜੇ_s, ਧਾਰਾ, ਚੌਖਟਾ) ?
         NGHTTP2_ERR_CALLBACK_FAILURE : 0;
}

ਸਥਿਰ ਪੂਰਨ cf_h2_on_invalid_ਚੌਖਟਾ_recv(nghttp2_session *session,
                                       ਅਚਲ nghttp2_ਚੌਖਟਾ *ਚੌਖਟਾ,
                                       ਪੂਰਨ ngerr, ਖਾਲੀ *userp)
{
  ਬਣਤਰ Curl_cfilter *cf = userp;
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ;
  ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ;
  int32_t ਧਾਰਾ_id = ਚੌਖਟਾ->hd.ਧਾਰਾ_id;

  (ਖਾਲੀ)session;
  ਅੰਕੜੇ = cf_h2_get_ਧਾਰਾ_xfer(cf, ਧਾਰਾ_id, &ਧਾਰਾ);
  ਜੇ(ਅੰਕੜੇ) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
    ਅੱਖਰ ਬਫਰ[256];
    ਪੂਰਨ len;
    len = Curl_nghttp2_fr_print(ਚੌਖਟਾ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ) - 1);
    ਬਫਰ[len] = 0;
    failf(ਅੰਕੜੇ, "[HTTP2] [%d] received invalid frame: %s, error %d: %s",
          ਧਾਰਾ_id, ਬਫਰ, ngerr, nghttp2_strerror(ngerr));
#ਜੇ_ਅੰਤ /* CURLVERBOSE */
    ਜੇ(ਧਾਰਾ) {
      Curl_ngh2_submit_rst_ਧਾਰਾ(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, ਧਾਰਾ->id, NGHTTP2_ਧਾਰਾ_CLOSED);
      ਧਾਰਾ->error = ngerr;
      ਧਾਰਾ->closed = TRUE;
      ਧਾਰਾ->reset = TRUE;
    }
  }
  /* Return no error, nghttp2 will RST/GOAWAY by itself when needed */
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ on_ਅੰਕੜੇ_chunk_recv(nghttp2_session *session, uint8_t flags,
                              int32_t ਧਾਰਾ_id,
                              ਅਚਲ uint8_t *mem, size_t len, ਖਾਲੀ *userp)
{
  ਬਣਤਰ Curl_cfilter *cf = userp;
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ_s;

  (ਖਾਲੀ)session;
  (ਖਾਲੀ)flags;

  DEBUGASSERT(ਧਾਰਾ_id); /* should never be a zero stream ID here */
  DEBUGASSERT(CF_ਅੰਕੜੇ_CURRENT(cf));

  /* get the stream from the hash based on Stream ID */
  ਅੰਕੜੇ_s = cf_h2_get_ਧਾਰਾ_xfer(cf, ਧਾਰਾ_id, &ਧਾਰਾ);
  ਜੇ(!ਅੰਕੜੇ_s) {
    /* Receiving a Stream ID not in the hash should not happen - unless
       we have aborted a transfer artificially and there were more data
       in the pipeline. Silently ignore. */
    CURL_TRC_CF(CF_ਅੰਕੜੇ_CURRENT(cf), cf, "[%d] Data for unknown", ਧਾਰਾ_id);
    /* consumed explicitly as no one will read it */
    Curl_ngh2_session_consume(&ctx->ngh2, ਧਾਰਾ_id, len);
    ਵਾਪਸ 0;
  }

  ਜੇ(!ਧਾਰਾ)
    ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;

  h2_xfer_write_resp(cf, ਅੰਕੜੇ_s, ਧਾਰਾ, (ਅਚਲ ਅੱਖਰ *)mem, len, FALSE);

  Curl_ngh2_session_consume(&ctx->ngh2, ਧਾਰਾ_id, len);
  ਧਾਰਾ->nrcvd_ਅੰਕੜੇ += (curl_off_t)len;
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ on_ਧਾਰਾ_close(nghttp2_session *session, int32_t ਧਾਰਾ_id,
                           uint32_t error_ਕੋਡ, ਖਾਲੀ *userp)
{
  ਬਣਤਰ Curl_cfilter *cf = userp;
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ_s, *call_ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
  ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ = ਨਿਰਾ;
  ਪੂਰਨ rv;
  (ਖਾਲੀ)session;

  DEBUGASSERT(call_ਅੰਕੜੇ);
  /* stream id 0 is the connection, do not look there for streams. */
  ਅੰਕੜੇ_s = ਧਾਰਾ_id ? cf_h2_get_ਧਾਰਾ_xfer(cf, ਧਾਰਾ_id, &ਧਾਰਾ) : ਨਿਰਾ;
  ਜੇ(!ਅੰਕੜੇ_s) {
    CURL_TRC_CF(call_ਅੰਕੜੇ, cf,
                "[%d] on_stream_close, no easy set on stream", ਧਾਰਾ_id);
    ਵਾਪਸ 0;
  }
  ਜੇ(!GOOD_EASY_HANDLE(ਅੰਕੜੇ_s)) {
    /* nghttp2 still has an easy registered for the stream which has
     * been freed be libcurl. This points to a code path that does not
     * trigger DONE or DETACH events as it must. */
    CURL_TRC_CF(call_ਅੰਕੜੇ, cf,
                "[%d] on_stream_close, not a GOOD easy on stream", ਧਾਰਾ_id);
    (ਖਾਲੀ)Curl_ngh2_session_ਸੈੱਟ_ਧਾਰਾ_user_ਅੰਕੜੇ(
      &ctx->ngh2, ਧਾਰਾ_id, ਨਿਰਾ);
    ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  ਜੇ(!ਧਾਰਾ) {
    CURL_TRC_CF(ਅੰਕੜੇ_s, cf,
                "[%d] on_stream_close, GOOD easy but no stream", ਧਾਰਾ_id);
    ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  ਧਾਰਾ->closed = TRUE;
  ਧਾਰਾ->error = error_ਕੋਡ;
  ਜੇ(ਧਾਰਾ->error) {
    ਧਾਰਾ->reset = TRUE;
    ਜੇ((ਧਾਰਾ->error == NGHTTP2_REFUSED_ਧਾਰਾ) &&
       ctx->rcvd_goaway && ctx->goaway_error)
    /* REFUSED_STREAM is used by nghttp2 on a GOAWAY from the
     * server where it indicated it will no longer process this stream.
     * If the GOAWAY carried an error code, take that as the error for
     * the stream. */
    ਧਾਰਾ->error = ctx->goaway_error;
  }

  ਜੇ(ਧਾਰਾ->error)
    CURL_TRC_CF(ਅੰਕੜੇ_s, cf, "[%d] RESET: %s (err %u)",
              ਧਾਰਾ_id, nghttp2_http2_strerror(error_ਕੋਡ), error_ਕੋਡ);
  ਨਹੀਂਤਾਂ
    CURL_TRC_CF(ਅੰਕੜੇ_s, cf, "[%d] CLOSED", ਧਾਰਾ_id);
  Curl_multi_mark_dirty(ਅੰਕੜੇ_s);

  /* remove `stream_ctx` from the nghttp2 stream */
  rv = Curl_ngh2_session_ਸੈੱਟ_ਧਾਰਾ_user_ਅੰਕੜੇ(
    &ctx->ngh2, ਧਾਰਾ_id, ਨਿਰਾ);
  ਜੇ(rv) {
    infof(ਅੰਕੜੇ_s, "http/2: failed to clear user_data for stream %d",
          ਧਾਰਾ_id);
    DEBUGASSERT(0);
  }
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ cf_h2_ਸਿਰਲੇਖ_error(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ,
                               CURLcode ਨਤੀਜਾ)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;

  failf(ਅੰਕੜੇ, "Error receiving HTTP2 header: %d(%s)", (ਪੂਰਨ)ਨਤੀਜਾ,
        curl_easy_strerror(ਨਤੀਜਾ));
  ਜੇ(ਧਾਰਾ) {
    Curl_ngh2_submit_rst_ਧਾਰਾ(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, ਧਾਰਾ->id, NGHTTP2_ਧਾਰਾ_CLOSED);
    ਧਾਰਾ->closed = TRUE;
    ਧਾਰਾ->reset = TRUE;
  }
}

/* frame->hd.type is either NGHTTP2_HEADERS or NGHTTP2_PUSH_PROMISE */
ਸਥਿਰ ਪੂਰਨ on_ਸਿਰਲੇਖ(nghttp2_session *session, ਅਚਲ nghttp2_ਚੌਖਟਾ *ਚੌਖਟਾ,
                     ਅਚਲ uint8_t *name, size_t namelen,
                     ਅਚਲ uint8_t *ਮੁੱਲ, size_t valuelen,
                     uint8_t flags,
                     ਖਾਲੀ *userp)
{
  ਬਣਤਰ Curl_cfilter *cf = userp;
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ;
  int32_t ਧਾਰਾ_id = ਚੌਖਟਾ->hd.ਧਾਰਾ_id;
  CURLcode ਨਤੀਜਾ;

  (ਖਾਲੀ)session;
  (ਖਾਲੀ)flags;

  DEBUGASSERT(ਧਾਰਾ_id); /* should never be a zero stream ID here */

  /* get the stream from the hash based on Stream ID */
  ਅੰਕੜੇ = cf_h2_get_ਧਾਰਾ_xfer(cf, ਧਾਰਾ_id, &ਧਾਰਾ);
  ਜੇ(!GOOD_EASY_HANDLE(ਅੰਕੜੇ))
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;

  ਜੇ(!ਧਾਰਾ) {
    failf(ਅੰਕੜੇ, "Internal NULL stream");
    ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  /* Store received PUSH_PROMISE headers to be used when the subsequent
     PUSH_PROMISE callback comes */
  ਜੇ(ਚੌਖਟਾ->hd.type == NGHTTP2_ਧੱਕੋ_PROMISE) {
    ਅੱਖਰ *h;

    ਜੇ((namelen == CURL_CSTRLEN(HTTP_PSEUDO_AUTHORITY)) &&
       !ਲੜੀ_ਤੁਲਨਾ(HTTP_PSEUDO_AUTHORITY, (ਅਚਲ ਅੱਖਰ *)name, namelen)) {
      /* pseudo headers are lower case */
      ਪੂਰਨ rc = 0;
      ਅੱਖਰ *check = curl_maprintf("%s:%d", ਅੰਕੜੇ->state.origin->hostname,
                                  ਅੰਕੜੇ->state.origin->ਪੋਰਟ);
      ਜੇ(!check)
        /* no memory */
        ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;
      ਜੇ(!curl_strequal(check, (ਅਚਲ ਅੱਖਰ *)ਮੁੱਲ) &&
         ((ਅੰਕੜੇ->state.origin->ਪੋਰਟ != ਅੰਕੜੇ->state.origin->scheme->defport) ||
          !curl_strequal(ਅੰਕੜੇ->state.origin->hostname, (ਅਚਲ ਅੱਖਰ *)ਮੁੱਲ))) {
        /* This is push is not for the same authority that was asked for in
         * the URL. RFC 7540 section 8.2 says: "A client MUST treat a
         * PUSH_PROMISE for which the server is not authoritative as a stream
         * error of type PROTOCOL_ERROR."
         */
        (ਖਾਲੀ)Curl_ngh2_submit_rst_ਧਾਰਾ(
          &ctx->ngh2, NGHTTP2_FLAG_NONE, ਧਾਰਾ_id, NGHTTP2_PROTOCOL_ERROR);
        rc = NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      curlx_free(check);
      ਜੇ(rc)
        ਵਾਪਸ rc;
    }

    ਜੇ(!ਧਾਰਾ->ਧੱਕੋ_ਸਿਰਲੇਖਾਂ) {
      ਧਾਰਾ->ਧੱਕੋ_ਸਿਰਲੇਖਾਂ_alloc = 10;
      ਧਾਰਾ->ਧੱਕੋ_ਸਿਰਲੇਖਾਂ = curlx_malloc(ਧਾਰਾ->ਧੱਕੋ_ਸਿਰਲੇਖਾਂ_alloc *
                                          ਆਕਾਰ(ਅੱਖਰ *));
      ਜੇ(!ਧਾਰਾ->ਧੱਕੋ_ਸਿਰਲੇਖਾਂ)
        ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;
      ਧਾਰਾ->ਧੱਕੋ_ਸਿਰਲੇਖਾਂ_ਵਰਤਿਆ = 0;
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਧਾਰਾ->ਧੱਕੋ_ਸਿਰਲੇਖਾਂ_ਵਰਤਿਆ ==
            ਧਾਰਾ->ਧੱਕੋ_ਸਿਰਲੇਖਾਂ_alloc) {
      ਅੱਖਰ **headp;
      ਜੇ(ਧਾਰਾ->ਧੱਕੋ_ਸਿਰਲੇਖਾਂ_alloc > 1000) {
        /* this is beyond crazy many headers, bail out */
        failf(ਅੰਕੜੇ, "Too many PUSH_PROMISE headers");
        free_ਧੱਕੋ_ਸਿਰਲੇਖਾਂ(ਧਾਰਾ);
        ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      ਧਾਰਾ->ਧੱਕੋ_ਸਿਰਲੇਖਾਂ_alloc *= 2;
      headp = curlx_realloc(ਧਾਰਾ->ਧੱਕੋ_ਸਿਰਲੇਖਾਂ,
                            ਧਾਰਾ->ਧੱਕੋ_ਸਿਰਲੇਖਾਂ_alloc * ਆਕਾਰ(ਅੱਖਰ *));
      ਜੇ(!headp) {
        free_ਧੱਕੋ_ਸਿਰਲੇਖਾਂ(ਧਾਰਾ);
        ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      ਧਾਰਾ->ਧੱਕੋ_ਸਿਰਲੇਖਾਂ = headp;
    }
    h = curl_maprintf("%s:%s", name, ਮੁੱਲ);
    ਜੇ(!h) {
      free_ਧੱਕੋ_ਸਿਰਲੇਖਾਂ(ਧਾਰਾ);
      ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    ਧਾਰਾ->ਧੱਕੋ_ਸਿਰਲੇਖਾਂ[ਧਾਰਾ->ਧੱਕੋ_ਸਿਰਲੇਖਾਂ_ਵਰਤਿਆ++] = h;
    ਵਾਪਸ 0;
  }

  ਜੇ(ਧਾਰਾ->bodystarted) {
    /* This is a trailer */
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] trailer: %.*s: %.*s",
                ਧਾਰਾ->id, (ਪੂਰਨ)namelen, name, (ਪੂਰਨ)valuelen, ਮੁੱਲ);
    ਨਤੀਜਾ = Curl_dynhds_ਜੋੜੋ(&ਧਾਰਾ->resp_trailers,
                             (ਅਚਲ ਅੱਖਰ *)name, namelen,
                             (ਅਚਲ ਅੱਖਰ *)ਮੁੱਲ, valuelen);
    ਜੇ(ਨਤੀਜਾ) {
      cf_h2_ਸਿਰਲੇਖ_error(cf, ਅੰਕੜੇ, ਧਾਰਾ, ਨਤੀਜਾ);
      ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;
    }

    ਵਾਪਸ 0;
  }

  ਜੇ(namelen == CURL_CSTRLEN(HTTP_PSEUDO_ਸਥਿਤੀ) &&
     !memcmp(HTTP_PSEUDO_ਸਥਿਤੀ, name, namelen)) {
    /* nghttp2 guarantees :status is received first and only once. */
    ਅੱਖਰ ਬਫਰ[32];
    size_t hlen;
    ਨਤੀਜਾ = Curl_http_decode_ਸਥਿਤੀ(&ਧਾਰਾ->ਸਥਿਤੀ_ਕੋਡ,
                                     (ਅਚਲ ਅੱਖਰ *)ਮੁੱਲ, valuelen);
    ਜੇ(ਨਤੀਜਾ) {
      cf_h2_ਸਿਰਲੇਖ_error(cf, ਅੰਕੜੇ, ਧਾਰਾ, ਨਤੀਜਾ);
      ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    hlen = curl_msnprintf(ਬਫਰ, ਆਕਾਰ(ਬਫਰ), HTTP_PSEUDO_ਸਥਿਤੀ ":%d\r",
                          ਧਾਰਾ->ਸਥਿਤੀ_ਕੋਡ);
    ਨਤੀਜਾ = Curl_ਸਿਰਲੇਖਾਂ_ਧੱਕੋ(ਅੰਕੜੇ, ਬਫਰ, hlen, CURLH_PSEUDO);
    ਜੇ(ਨਤੀਜਾ) {
      cf_h2_ਸਿਰਲੇਖ_error(cf, ਅੰਕੜੇ, ਧਾਰਾ, ਨਤੀਜਾ);
      ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    curlx_dyn_reset(&ctx->scratch);
    ਨਤੀਜਾ = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/2 "));
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = curlx_dyn_addn(&ctx->scratch, ਮੁੱਲ, valuelen);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    ਜੇ(!ਨਤੀਜਾ)
      h2_xfer_write_resp_hd(cf, ਅੰਕੜੇ, ਧਾਰਾ, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    ਜੇ(ਨਤੀਜਾ) {
      cf_h2_ਸਿਰਲੇਖ_error(cf, ਅੰਕੜੇ, ਧਾਰਾ, ਨਤੀਜਾ);
      ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    /* if we receive data for another handle, wake that up */
    ਜੇ(CF_ਅੰਕੜੇ_CURRENT(cf) != ਅੰਕੜੇ)
      Curl_multi_mark_dirty(ਅੰਕੜੇ);

    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] status: HTTP/2 %03d",
                ਧਾਰਾ->id, ਧਾਰਾ->ਸਥਿਤੀ_ਕੋਡ);
    ਵਾਪਸ 0;
  }

  /* nghttp2 guarantees that namelen > 0, and :status was already
     received, and this is not pseudo-header field . */
  /* convert to an HTTP1-style header */
  curlx_dyn_reset(&ctx->scratch);
  ਨਤੀਜਾ = curlx_dyn_addn(&ctx->scratch, (ਅਚਲ ਅੱਖਰ *)name, namelen);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = curlx_dyn_addn(&ctx->scratch, (ਅਚਲ ਅੱਖਰ *)ਮੁੱਲ, valuelen);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
  ਜੇ(!ਨਤੀਜਾ)
    h2_xfer_write_resp_hd(cf, ਅੰਕੜੇ, ਧਾਰਾ, curlx_dyn_ptr(&ctx->scratch),
                          curlx_dyn_len(&ctx->scratch), FALSE);
  ਜੇ(ਨਤੀਜਾ) {
    cf_h2_ਸਿਰਲੇਖ_error(cf, ਅੰਕੜੇ, ਧਾਰਾ, ਨਤੀਜਾ);
    ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  /* if we receive data for another handle, wake that up */
  ਜੇ(CF_ਅੰਕੜੇ_CURRENT(cf) != ਅੰਕੜੇ)
    Curl_multi_mark_dirty(ਅੰਕੜੇ);

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] header: %.*s: %.*s",
              ਧਾਰਾ->id, (ਪੂਰਨ)namelen, name, (ਪੂਰਨ)valuelen, ਮੁੱਲ);

  ਵਾਪਸ 0; /* 0 is successful */
}

ਸਥਿਰ ssize_t req_body_read_callback(nghttp2_session *session,
                                      int32_t ਧਾਰਾ_id,
                                      uint8_t *buf, size_t ਲੰਮਾਈ,
                                      uint32_t *ਅੰਕੜੇ_flags,
                                      nghttp2_ਅੰਕੜੇ_ਸਰੋਤ *ਸਰੋਤ,
                                      ਖਾਲੀ *userp)
{
  ਬਣਤਰ Curl_cfilter *cf = userp;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ_s;
  ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;
  ssize_t nread;
  size_t n;
  (ਖਾਲੀ)ਸਰੋਤ;

  (ਖਾਲੀ)session;
  ਜੇ(!ਧਾਰਾ_id)
    ਵਾਪਸ NGHTTP2_ERR_INVALID_ਦਲੀਲ;

  /* get the stream from the hash based on Stream ID, stream ID zero is for
     connection-oriented stuff */
  ਅੰਕੜੇ_s = cf_h2_get_ਧਾਰਾ_xfer(cf, ਧਾਰਾ_id, &ਧਾਰਾ);
  ਜੇ(!ਅੰਕੜੇ_s || !ਧਾਰਾ)
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;

  ਨਤੀਜਾ = Curl_bufq_read(&ਧਾਰਾ->sendbuf, buf, ਲੰਮਾਈ, &n);
  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(ਨਤੀਜਾ != CURLE_AGAIN)
      ਵਾਪਸ NGHTTP2_ERR_CALLBACK_FAILURE;
    nread = 0;
  }
  ਨਹੀਂਤਾਂ
    nread = (ssize_t)n;

  CURL_TRC_CF(ਅੰਕੜੇ_s, cf, "[%d] req_body_read(len=%zu) eos=%d -> %zd, %d",
              ਧਾਰਾ_id, ਲੰਮਾਈ, ਧਾਰਾ->body_eos, nread, (ਪੂਰਨ)ਨਤੀਜਾ);

  ਜੇ(ਧਾਰਾ->body_eos && Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ਧਾਰਾ->sendbuf)) {
    *ਅੰਕੜੇ_flags = NGHTTP2_ਅੰਕੜੇ_FLAG_EOF;
    ਵਾਪਸ nread;
  }
  ਵਾਪਸ (nread == 0) ? NGHTTP2_ERR_DEFERRED : nread;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਸਥਿਰ ਪੂਰਨ error_callback(nghttp2_session *session,
                          ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ,
                          size_t len,
                          ਖਾਲੀ *userp)
{
  ਬਣਤਰ Curl_cfilter *cf = userp;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
  (ਖਾਲੀ)session;
  failf(ਅੰਕੜੇ, "%.*s", (ਪੂਰਨ)len, ਸੁਨੇਹਾ);
  ਵਾਪਸ 0;
}
#ਜੇ_ਅੰਤ

/*
 * Append headers to ask for an HTTP1.1 to HTTP2 upgrade.
 */
CURLcode Curl_http2_request_upgrade(ਬਣਤਰ dynbuf *req,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURLcode ਨਤੀਜਾ;
  ਅੱਖਰ *base64;
  size_t blen;
  ਬਣਤਰ SingleRequest *k = &ਅੰਕੜੇ->req;
  uint8_t binsettings[H2_BINSETTINGS_LEN];
  ssize_t rc;
  size_t binlen; /* length of the binsettings data */

  rc = populate_binsettings(binsettings, ਅੰਕੜੇ);
  ਜੇ(!curlx_sztouz(rc, &binlen) || !binlen) {
    failf(ਅੰਕੜੇ, "nghttp2 unexpectedly failed on pack_settings_payload");
    curlx_dyn_free(req);
    ਵਾਪਸ CURLE_FAILED_INIT;
  }

  ਨਤੀਜਾ = curlx_base64url_encode(binsettings, binlen, &base64, &blen);
  ਜੇ(ਨਤੀਜਾ) {
    curlx_dyn_free(req);
    ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਅੰਕੜੇ->state.http_hd_upgrade = TRUE;
  ਅੰਕੜੇ->state.http_hd_h2_settings = TRUE;
  ਨਤੀਜਾ = curlx_dyn_addf(req,
                          "Upgrade: %s\r\n"
                          "HTTP2-Settings: %s\r\n",
                          NGHTTP2_CLEARTEXT_PROTO_ਸੰਸਕਰਣ_ID, base64);
  curlx_free(base64);

  k->upgr101 = UPGR101_H2;
  ਅੰਕੜੇ->conn->ਬਿੱਟ.upgrade_ਵਿੱਚ_ਤਰੱਕੀ = TRUE;

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode http2_handle_ਧਾਰਾ_close(ਬਣਤਰ Curl_cfilter *cf,
                                          ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ,
                                          size_t *pnlen)
{
  CURLcode ਨਤੀਜਾ;

  *pnlen = 0;
  ਜੇ(ਧਾਰਾ->reset) {
    ਜੇ(ਧਾਰਾ->error == NGHTTP2_REFUSED_ਧਾਰਾ) {
      infof(ਅੰਕੜੇ, "HTTP/2 stream %d refused by server, try again on a new "
                  "connection", ਧਾਰਾ->id);
      connclose(cf->conn); /* do not use this anymore */
      ਅੰਕੜੇ->state.refused_ਧਾਰਾ = TRUE;
      ਵਾਪਸ CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਧਾਰਾ->resp_hds_complete && ਅੰਕੜੇ->req.no_body) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] error after response headers, but we did "
                  "not want a body anyway, ignore: %s (err %u)",
                  ਧਾਰਾ->id, nghttp2_http2_strerror(ਧਾਰਾ->error),
                  ਧਾਰਾ->error);
      ਧਾਰਾ->close_handled = TRUE;
      ਵਾਪਸ CURLE_OK;
    }
    failf(ਅੰਕੜੇ, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          ਧਾਰਾ->id, ਧਾਰਾ->reset_by_server ? "server" : "curl",
          ਧਾਰਾ->error, nghttp2_http2_strerror(ਧਾਰਾ->error));
    ਵਾਪਸ ਧਾਰਾ->error ? CURLE_HTTP2_ਧਾਰਾ :
           (ਅੰਕੜੇ->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2);
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਧਾਰਾ->bodystarted) {
    failf(ਅੰਕੜੇ, "HTTP/2 stream %d was closed cleanly, but before getting "
          "all response header fields, treated as error", ਧਾਰਾ->id);
    ਵਾਪਸ CURLE_HTTP2_ਧਾਰਾ;
  }

  ਜੇ(Curl_dynhds_ਗਿਣਤੀ(&ਧਾਰਾ->resp_trailers)) {
    ਬਣਤਰ dynhds_ਇੰਦਰਾਜ *e;
    ਬਣਤਰ dynbuf dbuf;
    size_t i;

    ਨਤੀਜਾ = CURLE_OK;
    curlx_dyn_init(&dbuf, DYN_TRAILERS);
    ਲਈ(i = 0; i < Curl_dynhds_ਗਿਣਤੀ(&ਧਾਰਾ->resp_trailers); ++i) {
      e = Curl_dynhds_getn(&ਧਾਰਾ->resp_trailers, i);
      ਜੇ(!e)
        ਰੋਕੋ;
      curlx_dyn_reset(&dbuf);
      ਨਤੀਜਾ = curlx_dyn_addf(&dbuf, "%.*s: %.*s\x0d\x0a",
                              (ਪੂਰਨ)e->namelen, e->name,
                              (ਪੂਰਨ)e->valuelen, e->ਮੁੱਲ);
      ਜੇ(ਨਤੀਜਾ)
        ਰੋਕੋ;
      Curl_debug(ਅੰਕੜੇ, CURLINFO_ਸਿਰਲੇਖ_ਵਿੱਚ, curlx_dyn_ptr(&dbuf),
                 curlx_dyn_len(&dbuf));
      ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ,
                                 CLIENTWRITE_ਸਿਰਲੇਖ | CLIENTWRITE_TRAILER,
                                 curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf));
      ਜੇ(ਨਤੀਜਾ)
        ਰੋਕੋ;
      ਨਤੀਜਾ = Curl_bump_headersize(ਅੰਕੜੇ, curlx_dyn_len(&dbuf), FALSE);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    curlx_dyn_free(&dbuf);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਧਾਰਾ->close_handled = TRUE;
  ਨਤੀਜਾ = CURLE_OK;

ਬਾਹਰਲਾ:
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "handle_stream_close -> %d, %zu", (ਪੂਰਨ)ਨਤੀਜਾ, *pnlen);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਪੂਰਨ sweight_wanted(ਅਚਲ ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  ਵਾਪਸ ਅੰਕੜੇ->ਸੈੱਟ.ਭਾਰ ?
    ਅੰਕੜੇ->ਸੈੱਟ.ਭਾਰ : NGHTTP2_DEFAULT_ਭਾਰ;
}

ਸਥਿਰ ਪੂਰਨ sweight_ਵਿੱਚ_effect(ਅਚਲ ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  ਵਾਪਸ ਅੰਕੜੇ->state.ਭਾਰ ?
    ਅੰਕੜੇ->state.ਭਾਰ : NGHTTP2_DEFAULT_ਭਾਰ;
}

/*
 * h2_pri_spec() fills in the pri_spec struct, used by nghttp2 to send weight
 * and dependency to the peer. It also stores the updated values in the state
 * struct.
 */

ਸਥਿਰ ਖਾਲੀ h2_pri_spec(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        nghttp2_ਤਰਜੀਹ_spec *pri_spec)
{
  ਪੂਰਨ prio = ਅੰਕੜੇ->ਸੈੱਟ.ਭਾਰ;
  nghttp2_ਤਰਜੀਹ_spec_init(pri_spec, 0, sweight_wanted(ਅੰਕੜੇ), FALSE);
  ਅੰਕੜੇ->state.ਭਾਰ = prio;
}

/*
 * Check if there is been an update in the priority /
 * dependency settings and if so it submits a PRIORITY frame with the updated
 * info.
 * Flush any out data pending in the network buffer.
 */
ਸਥਿਰ CURLcode h2_ਤਰੱਕੀ_egress(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  ਪੂਰਨ rv = 0;

  ਜੇ(sweight_wanted(ਅੰਕੜੇ) != sweight_ਵਿੱਚ_effect(ਅੰਕੜੇ)) {
    ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ = H2_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);
    ਜੇ(ਧਾਰਾ && ਧਾਰਾ->id > 0 &&
       (sweight_wanted(ਅੰਕੜੇ) != sweight_ਵਿੱਚ_effect(ਅੰਕੜੇ))) {
      /* send new weight and/or dependency */
      nghttp2_ਤਰਜੀਹ_spec pri_spec;

      h2_pri_spec(ਅੰਕੜੇ, &pri_spec);
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] Queuing PRIORITY", ਧਾਰਾ->id);
      DEBUGASSERT(ਧਾਰਾ->id != -1);
      rv = Curl_ngh2_submit_ਤਰਜੀਹ(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, ਧਾਰਾ->id, &pri_spec);
      ਜੇ(rv)
        ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  ctx->nw_ਬਾਹਰਲਾ_blocked = 0;
  ਜਦੋਂ(!rv && !ctx->nw_ਬਾਹਰਲਾ_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

ਬਾਹਰਲਾ:
  ਜੇ(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    ਵਾਪਸ CURLE_SEND_ERROR;
  }
  /* Defer flushing during the connect phase so that the SETTINGS and
   * other initial frames are sent together with the first request.
   * Unless we are 'connect_only' where the request will never come. */
  ਜੇ(!cf->connected && !cf->conn->ਬਿੱਟ.connect_only)
    ਵਾਪਸ CURLE_OK;
  ਵਾਪਸ nw_ਬਾਹਰਲਾ_flush(cf, ਅੰਕੜੇ);
}

ਸਥਿਰ CURLcode ਧਾਰਾ_recv(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ,
                            ਅੱਖਰ *buf, size_t len, size_t *pnread)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_AGAIN;

  (ਖਾਲੀ)buf;
  (ਖਾਲੀ)len;
  *pnread = 0;

  ਜੇ(!ਧਾਰਾ->xfer_ਨਤੀਜਾ)
    ਧਾਰਾ->xfer_ਨਤੀਜਾ = cf_h2_update_local_win(cf, ਅੰਕੜੇ, ਧਾਰਾ);

  ਜੇ(ਧਾਰਾ->xfer_ਨਤੀਜਾ) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] xfer write failed", ਧਾਰਾ->id);
    ਨਤੀਜਾ = ਧਾਰਾ->xfer_ਨਤੀਜਾ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਧਾਰਾ->closed) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] returning CLOSE", ਧਾਰਾ->id);
    ਨਤੀਜਾ = http2_handle_ਧਾਰਾ_close(cf, ਅੰਕੜੇ, ਧਾਰਾ, pnread);
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਧਾਰਾ->reset ||
          (ctx->conn_closed && Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->inbufq)) ||
          (ctx->rcvd_goaway && ctx->remote_max_sid < ਧਾਰਾ->id)) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] returning ERR", ਧਾਰਾ->id);
    ਨਤੀਜਾ = ਅੰਕੜੇ->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2;
  }

  ਜੇ(ਨਤੀਜਾ && (ਨਤੀਜਾ != CURLE_AGAIN))
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] stream_recv(len=%zu) -> %d, %zu",
                ਧਾਰਾ->id, len, (ਪੂਰਨ)ਨਤੀਜਾ, *pnread);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode h2_ਤਰੱਕੀ_ingress(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    size_t ਅੰਕੜੇ_max_ਬਾਈਟਾਂ)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t nread;

  ਜੇ(should_close_session(ctx)) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[0] ingress: session is closed");
    ਵਾਪਸ CURLE_HTTP2;
  }

  /* Process network input buffer first */
  ਜੇ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->inbufq)) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "Process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    ਨਤੀਜਾ = h2_process_pending_ਨਿਵੇਸ਼(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਜੇ(!ਅੰਕੜੇ_max_ਬਾਈਟਾਂ)
    ਅੰਕੜੇ_max_ਬਾਈਟਾਂ = H2_CHUNK_ਮਾਪ;

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop due to connection close or us not processing
   * all network input */
  ਜਦੋਂ(!ctx->conn_closed && Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->inbufq)) {
    ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ = H2_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);
    ਜੇ(ਧਾਰਾ && (ਧਾਰਾ->closed || !ਅੰਕੜੇ_max_ਬਾਈਟਾਂ)) {
      /* We would like to abort here and stop processing, so that the transfer
       * loop can handle the data/close here. This may leave data in
       * underlying buffers that will not be consumed. */
      ਜੇ(!cf->ਅਗਲਾ || !cf->ਅਗਲਾ->cft->has_ਅੰਕੜੇ_pending(cf->ਅਗਲਾ, ਅੰਕੜੇ))
        Curl_multi_mark_dirty(ਅੰਕੜੇ);
      ਰੋਕੋ;
    }

    ਨਤੀਜਾ = Curl_cf_recv_bufq(cf->ਅਗਲਾ, ਅੰਕੜੇ, &ctx->inbufq, 0, &nread);
    ਜੇ(ਨਤੀਜਾ) {
      ਜੇ(ਨਤੀਜਾ != CURLE_AGAIN) {
        failf(ਅੰਕੜੇ, "Failed receiving HTTP2 data: %d(%s)", (ਪੂਰਨ)ਨਤੀਜਾ,
              curl_easy_strerror(ਨਤੀਜਾ));
        ਵਾਪਸ ਨਤੀਜਾ;
      }
      ਰੋਕੋ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(nread == 0) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[0] ingress: connection closed");
      ctx->conn_closed = TRUE;
      ਰੋਕੋ;
    }
    ਨਹੀਂਤਾਂ {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[0] ingress: read %zu bytes", nread);
      ਅੰਕੜੇ_max_ਬਾਈਟਾਂ = (ਅੰਕੜੇ_max_ਬਾਈਟਾਂ > nread) ? (ਅੰਕੜੇ_max_ਬਾਈਟਾਂ - nread) : 0;
    }

    ਨਤੀਜਾ = h2_process_pending_ਨਿਵੇਸ਼(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[0] ingress: nw-in buffered %zu",
                Curl_bufq_len(&ctx->inbufq));
  }

  ਜੇ(ctx->conn_closed && Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->inbufq)) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "server closed %s",
                ctx->rcvd_goaway ? "with GOAWAY" : "abruptly");
    connclose(cf->conn);
  }

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[0] ingress: done");
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode cf_h2_recv(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਅੱਖਰ *buf, size_t len, size_t *pnread)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ;
  CURLcode ਨਤੀਜਾ, r2;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;

  ਜੇ(!ਅੰਕੜੇ)
    ਵਾਪਸ CURLE_HTTP2;

  ਧਾਰਾ = H2_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);

  *pnread = 0;
  ਜੇ(!ਧਾਰਾ) {
    /* Abnormal call sequence: either this transfer has never opened a stream
     * (unlikely) or the transfer has been done, cleaned up its resources, but
     * a read() is called anyway. It is not clear what the calling sequence
     * is for such a case. */
    failf(ਅੰਕੜੇ, "http/2 recv on a transfer never opened "
          "or already cleared, mid=%u", ਅੰਕੜੇ->mid);
    ਵਾਪਸ CURLE_HTTP2;
  }

  CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);

  ਨਤੀਜਾ = ਧਾਰਾ_recv(cf, ਅੰਕੜੇ, ਧਾਰਾ, buf, len, pnread);
  ਜੇ(ਨਤੀਜਾ && (ਨਤੀਜਾ != CURLE_AGAIN))
    ਜਾਓ ਬਾਹਰਲਾ;

  ਜੇ(ਨਤੀਜਾ) {
    ਨਤੀਜਾ = h2_ਤਰੱਕੀ_ingress(cf, ਅੰਕੜੇ, len);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;

    ਨਤੀਜਾ = ਧਾਰਾ_recv(cf, ਅੰਕੜੇ, ਧਾਰਾ, buf, len, pnread);
  }

  ਜੇ(*pnread > 0) {
    /* Now that we transferred this to the upper layer, we report
     * the actual amount of DATA consumed to the H2 session, so
     * that it adjusts stream flow control */
    Curl_ngh2_session_consume(&ctx->ngh2, ਧਾਰਾ->id, *pnread);
    ਜੇ(ਧਾਰਾ->closed) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] DRAIN closed stream", ਧਾਰਾ->id);
      Curl_multi_mark_dirty(ਅੰਕੜੇ);
    }
  }

ਬਾਹਰਲਾ:
  r2 = h2_ਤਰੱਕੀ_egress(cf, ਅੰਕੜੇ);
  ਜੇ(r2 == CURLE_AGAIN) {
    /* pending data to send, need to be called again. Ideally, we
     * monitor the socket for POLLOUT, but when not SENDING
     * any more, we force processing of the transfer. */
    ਜੇ(!CURL_REQ_WANT_SEND(ਅੰਕੜੇ))
      Curl_multi_mark_dirty(ਅੰਕੜੇ);
  }
  ਨਹੀਂਤਾਂ ਜੇ(r2 && (ਨਤੀਜਾ == CURLE_AGAIN)) {
    /* Do not replace a previous error result NOR a successful result
     * with an egress error. Example: the server might close the
     * connection after sending the complete response with EOS
     * and everything. Further sending may fail, but that MUST
     * not change that we had a successful response. */
    ਨਤੀਜਾ = r2;
  }

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] cf_recv(len=%zu) -> %d, %zu, "
              "window=%d/%d, connection %d/%d",
              ਧਾਰਾ->id, len, (ਪੂਰਨ)ਨਤੀਜਾ, *pnread,
              Curl_ngh2_session_get_ਧਾਰਾ_effective_recv_ਅੰਕੜੇ_ਲੰਮਾਈ(
                &ctx->ngh2, ਧਾਰਾ->id),
              Curl_ngh2_session_get_ਧਾਰਾ_effective_local_ਖਿੜਕੀ_ਮਾਪ(
                &ctx->ngh2, ਧਾਰਾ->id),
              Curl_ngh2_session_get_local_ਖਿੜਕੀ_ਮਾਪ(&ctx->ngh2),
              HTTP2_HUGE_ਖਿੜਕੀ_ਮਾਪ);

  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_h2_body_send(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ,
                                ਅਚਲ ਖਾਲੀ *buf, size_t blen, bool eos,
                                size_t *pnwritten)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ;

  *pnwritten = 0;
  ਜੇ(ਧਾਰਾ->closed) {
    ਜੇ(ਧਾਰਾ->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] discarding data"
                  "on closed stream with response", ਧਾਰਾ->id);
      ਜੇ(eos)
        ਧਾਰਾ->body_eos = TRUE;
      *pnwritten = blen;
      ਵਾਪਸ CURLE_OK;
    }
    /* Server closed before we got a response, this is an error */
    infof(ਅੰਕੜੇ, "stream %d closed", ਧਾਰਾ->id);
    ਵਾਪਸ CURLE_SEND_ERROR;
  }

  ਨਤੀਜਾ = Curl_bufq_write(&ਧਾਰਾ->sendbuf, buf, blen, pnwritten);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਜੇ(eos && (blen == *pnwritten))
    ਧਾਰਾ->body_eos = TRUE;

  ਜੇ(eos || !Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ਧਾਰਾ->sendbuf)) {
    /* resume the potentially suspended stream */
    ਜੇ(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_ਅੰਕੜੇ(&ctx->ngh2, ਧਾਰਾ->id)))
      ਵਾਪਸ CURLE_SEND_ERROR;
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode h2_submit(ਬਣਤਰ h2_ਧਾਰਾ_ctx **pstream,
                          ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਅਚਲ uint8_t *buf, size_t len,
                          bool eos, size_t *pnwritten)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ = ਨਿਰਾ;
  ਬਣਤਰ dynhds h2_ਸਿਰਲੇਖਾਂ;
  nghttp2_nv *nva = ਨਿਰਾ;
  ਅਚਲ ਖਾਲੀ *body = ਨਿਰਾ;
  size_t nheader, bodylen;
  nghttp2_ਅੰਕੜੇ_provider ਅੰਕੜੇ_prd;
  int32_t ਧਾਰਾ_id;
  nghttp2_ਤਰਜੀਹ_spec pri_spec;
  size_t nwritten;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  uint32_t initial_win_ਮਾਪ;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_ਸਿਰਲੇਖਾਂ, MAX_HTTP_RESP_ਸਿਰਲੇਖ_ਗਿਣਤੀ, DYN_HTTP_REQUEST);

  ਨਤੀਜਾ = http2_ਅੰਕੜੇ_ਤਿਆਰੀ(cf, ਅੰਕੜੇ, &ਧਾਰਾ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਨਤੀਜਾ = Curl_h1_req_parse_read(
    &ਧਾਰਾ->h1, buf, len, ਨਿਰਾ,
    !ਅੰਕੜੇ->state.http_ignorecustom ?
    CURL_EASY_STR(ਅੰਕੜੇ, STRING_CUSTOMREQUEST) : ਨਿਰਾ,
    0, &nwritten);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  *pnwritten = nwritten;
  ਜੇ(!ਧਾਰਾ->h1.ਪੂਰਾ) {
    /* need more data */
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  DEBUGASSERT(ਧਾਰਾ->h1.req);

  ਨਤੀਜਾ = Curl_http_req_ਮੰਜ਼ਿਲ_h2(&h2_ਸਿਰਲੇਖਾਂ, ਧਾਰਾ->h1.req, ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  /* no longer needed */
  Curl_h1_req_parse_free(&ਧਾਰਾ->h1);

  nva = Curl_dynhds_ਮੰਜ਼ਿਲ_nva(&h2_ਸਿਰਲੇਖਾਂ, &nheader);
  ਜੇ(!nva) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  h2_pri_spec(ਅੰਕੜੇ, &pri_spec);
  ਜੇ(!Curl_ngh2_session_check_request_allowed(&ctx->ngh2))
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "send request NOT allowed (via nghttp2)");

  /* Check the initial windows size of the transfer (rate-limits?) and
   * send an updated settings on changes from previous value. */
  initial_win_ਮਾਪ = cf_h2_initial_win_ਮਾਪ(ਅੰਕੜੇ);
  ਜੇ(initial_win_ਮਾਪ != ctx->initial_win_ਮਾਪ) {
    ਨਤੀਜਾ = cf_h2_update_settings(ctx, initial_win_ਮਾਪ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਚੋਣ(ਅੰਕੜੇ->state.httpreq) {
  ਹਾਲਤ HTTPREQ_POST:
  ਹਾਲਤ HTTPREQ_POST_FORM:
  ਹਾਲਤ HTTPREQ_POST_MIME:
  ਹਾਲਤ HTTPREQ_PUT:
    ਅੰਕੜੇ_prd.read_callback = req_body_read_callback;
    ਅੰਕੜੇ_prd.ਸਰੋਤ.ptr = ਨਿਰਾ;
    ਧਾਰਾ_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, &ਅੰਕੜੇ_prd, ਧਾਰਾ);
    ਰੋਕੋ;
  ਮੂਲ:
    ਧਾਰਾ_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, ਨਿਰਾ, ਧਾਰਾ);
  }

  ਜੇ(ਧਾਰਾ_id < 0) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "send: nghttp2_submit_request error (%s)%d",
                nghttp2_strerror(ਧਾਰਾ_id), ਧਾਰਾ_id);
    ਨਤੀਜਾ = CURLE_SEND_ERROR;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ACC 60000  /* <64KB to account for some overhead */
  ਜੇ(Curl_trc_is_verbose(ਅੰਕੜੇ)) {
    size_t acc = 0, i;

    infof(ਅੰਕੜੇ, "[HTTP/2] [%d] OPENED stream for %s",
          ਧਾਰਾ_id, Curl_bufref_ptr(&ਅੰਕੜੇ->state.url));
    ਲਈ(i = 0; i < nheader; ++i) {
      acc += nva[i].namelen + nva[i].valuelen;

      infof(ਅੰਕੜੇ, "[HTTP/2] [%d] [%.*s: %.*s]", ਧਾਰਾ_id,
            (ਪੂਰਨ)nva[i].namelen, nva[i].name,
            (ਪੂਰਨ)nva[i].valuelen, nva[i].ਮੁੱਲ);
    }

    ਜੇ(acc > MAX_ACC) {
      infof(ਅੰਕੜੇ, "[HTTP/2] Warning: The cumulative length of all "
            "headers exceeds %d bytes and that could cause the "
            "stream to be rejected.", MAX_ACC);
    }
  }
#ਜੇ_ਅੰਤ

  ਧਾਰਾ->id = ਧਾਰਾ_id;

  body = (ਅਚਲ ਅੱਖਰ *)buf + *pnwritten;
  bodylen = len - *pnwritten;

  ਜੇ(bodylen || eos) {
    size_t n;
    ਨਤੀਜਾ = cf_h2_body_send(cf, ਅੰਕੜੇ, ਧਾਰਾ, body, bodylen, eos, &n);
    ਜੇ(!ਨਤੀਜਾ)
      *pnwritten += n;
    ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
      ਨਤੀਜਾ = CURLE_OK;
    ਨਹੀਂਤਾਂ {
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
    }
  }

ਬਾਹਰਲਾ:
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] submit -> %d, %zu",
              ਧਾਰਾ ? ਧਾਰਾ->id : -1, (ਪੂਰਨ)ਨਤੀਜਾ, *pnwritten);
  curlx_safefree(nva);
  *pstream = ਧਾਰਾ;
  Curl_dynhds_free(&h2_ਸਿਰਲੇਖਾਂ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_h2_send(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਅਚਲ uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ = H2_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;
  CURLcode ਨਤੀਜਾ = CURLE_OK, r2;

  CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
  *pnwritten = 0;

  ਜੇ(!ਧਾਰਾ || ਧਾਰਾ->id == -1) {
    ਨਤੀਜਾ = h2_submit(&ਧਾਰਾ, cf, ਅੰਕੜੇ, buf, len, eos, pnwritten);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    DEBUGASSERT(ਧਾਰਾ);
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਧਾਰਾ->body_eos) {
    /* We already wrote this, but CURLE_AGAIN-ed the call due to not
     * being able to flush stream->sendbuf. Make a 0-length write
     * to trigger flushing again.
     * If this works, we report to have written `len` bytes. */
    size_t n;
    DEBUGASSERT(eos);
    ਨਤੀਜਾ = cf_h2_body_send(cf, ਅੰਕੜੇ, ਧਾਰਾ, buf, 0, eos, &n);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] cf_body_send last CHUNK -> %d, %zu, eos=%d",
                ਧਾਰਾ->id, (ਪੂਰਨ)ਨਤੀਜਾ, n, eos);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    *pnwritten = len;
  }
  ਨਹੀਂਤਾਂ {
    ਨਤੀਜਾ = cf_h2_body_send(cf, ਅੰਕੜੇ, ਧਾਰਾ, buf, len, eos, pnwritten);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] cf_body_send(len=%zu) -> %d, %zu, eos=%d",
                ਧਾਰਾ->id, len, (ਪੂਰਨ)ਨਤੀਜਾ, *pnwritten, eos);
  }

  /* Call the nghttp2 send loop and flush to write ALL buffered data,
   * headers and/or request body completely out to the network */
  r2 = h2_ਤਰੱਕੀ_egress(cf, ਅੰਕੜੇ);

  /* if the stream has been closed in egress handling (nghttp2 does that
   * when it does not like the headers, for example */
  ਜੇ(ਧਾਰਾ && ਧਾਰਾ->closed && !ਧਾਰਾ->resp_hds_complete) {
    infof(ਅੰਕੜੇ, "stream %d closed", ਧਾਰਾ->id);
    ਨਤੀਜਾ = CURLE_SEND_ERROR;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(r2 && (r2 != CURLE_AGAIN)) {
    ਨਤੀਜਾ = r2;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    ਜੇ(ਧਾਰਾ && ਧਾਰਾ->closed) {
      ਨਤੀਜਾ = http2_handle_ਧਾਰਾ_close(cf, ਅੰਕੜੇ, ਧਾਰਾ, pnwritten);
    }
    ਨਹੀਂਤਾਂ {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "send: nothing to do in this session");
      ਨਤੀਜਾ = CURLE_HTTP2;
    }
  }

ਬਾਹਰਲਾ:
  ਜੇ(ਧਾਰਾ) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
                "eos=%d, h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                ਧਾਰਾ->id, len, (ਪੂਰਨ)ਨਤੀਜਾ, *pnwritten,
                ਧਾਰਾ->body_eos,
                Curl_ngh2_session_get_ਧਾਰਾ_remote_ਖਿੜਕੀ_ਮਾਪ(
                  &ctx->ngh2, ਧਾਰਾ->id),
                Curl_ngh2_session_get_remote_ਖਿੜਕੀ_ਮਾਪ(&ctx->ngh2),
                Curl_bufq_len(&ਧਾਰਾ->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  ਨਹੀਂਤਾਂ {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "cf_send(len=%zu) -> %d, %zu, "
                "connection-window=%d, nw_send_buffer(%zu)",
                len, (ਪੂਰਨ)ਨਤੀਜਾ, *pnwritten,
                Curl_ngh2_session_get_remote_ਖਿੜਕੀ_ਮਾਪ(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_h2_flush(ਬਣਤਰ Curl_cfilter *cf,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ = H2_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
  ਜੇ(ਧਾਰਾ && !Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ਧਾਰਾ->sendbuf)) {
    /* resume the potentially suspended stream */
    ਜੇ(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_ਅੰਕੜੇ(&ctx->ngh2, ਧਾਰਾ->id))) {
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  ਨਤੀਜਾ = h2_ਤਰੱਕੀ_egress(cf, ਅੰਕੜੇ);

ਬਾਹਰਲਾ:
  ਜੇ(ਧਾਰਾ) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] flush -> %d, "
                "h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                ਧਾਰਾ->id, (ਪੂਰਨ)ਨਤੀਜਾ,
                Curl_ngh2_session_get_ਧਾਰਾ_remote_ਖਿੜਕੀ_ਮਾਪ(
                  &ctx->ngh2, ਧਾਰਾ->id),
                Curl_ngh2_session_get_remote_ਖਿੜਕੀ_ਮਾਪ(&ctx->ngh2),
                Curl_bufq_len(&ਧਾਰਾ->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  ਨਹੀਂਤਾਂ {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "flush -> %d, "
                "connection-window=%d, nw_send_buffer(%zu)", (ਪੂਰਨ)ਨਤੀਜਾ,
                Curl_ngh2_session_get_remote_ਖਿੜਕੀ_ਮਾਪ(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_h2_adjust_pollset(ਬਣਤਰ Curl_cfilter *cf,
                                     ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;
  curl_socket_t sock;
  bool want_recv, want_send;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(!Curl_ngh2_valid(&ctx->ngh2))
    ਵਾਪਸ CURLE_OK;

  sock = Curl_conn_cf_get_socket(cf, ਅੰਕੜੇ);
  Curl_pollset_check(ਅੰਕੜੇ, ps, sock, &want_recv, &want_send);
  ਜੇ(want_recv || want_send) {
    ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ = H2_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);
    bool c_exhaust, s_exhaust;

    CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
    c_exhaust = want_send &&
                !Curl_ngh2_session_get_remote_ਖਿੜਕੀ_ਮਾਪ(&ctx->ngh2);
    s_exhaust = want_send && ਧਾਰਾ && ਧਾਰਾ->id >= 0 &&
                !Curl_ngh2_session_get_ਧਾਰਾ_remote_ਖਿੜਕੀ_ਮਾਪ(
                  &ctx->ngh2, ਧਾਰਾ->id);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                (!c_exhaust && Curl_ngh2_session_want_write(&ctx->ngh2)) ||
                !Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->outbufq);

    ਨਤੀਜਾ = Curl_pollset_ਸੈੱਟ(ਅੰਕੜੇ, ps, sock, want_recv, want_send);
    CF_ਅੰਕੜੇ_RESTORE(cf, save);
  }
  ਨਹੀਂਤਾਂ ਜੇ(ctx->ਭੇਜਿਆ_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->outbufq);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    ਨਤੀਜਾ = Curl_pollset_ਸੈੱਟ(ਅੰਕੜੇ, ps, sock, want_recv, want_send);
    CF_ਅੰਕੜੇ_RESTORE(cf, save);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_h2_ctx_open(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  ਪੂਰਨ rc;
  nghttp2_session_callbacks *cbs = ਨਿਰਾ;

  DEBUGASSERT(!ctx->ngh2.s);
  DEBUGASSERT(!ctx->ngh2.fatal_rv);
  DEBUGASSERT(ctx->initialized);

  rc = nghttp2_session_callbacks_new(&cbs);
  ਜੇ(rc) {
    failf(ਅੰਕੜੇ, "Could not initialize nghttp2 callbacks");
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  nghttp2_session_callbacks_ਸੈੱਟ_send_callback(cbs, send_callback);
  nghttp2_session_callbacks_ਸੈੱਟ_on_ਚੌਖਟਾ_recv_callback(cbs, on_ਚੌਖਟਾ_recv);
  nghttp2_session_callbacks_ਸੈੱਟ_on_invalid_ਚੌਖਟਾ_recv_callback(cbs,
    cf_h2_on_invalid_ਚੌਖਟਾ_recv);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  nghttp2_session_callbacks_ਸੈੱਟ_on_ਚੌਖਟਾ_send_callback(cbs, on_ਚੌਖਟਾ_send);
#ਜੇ_ਅੰਤ
  nghttp2_session_callbacks_ਸੈੱਟ_on_ਅੰਕੜੇ_chunk_recv_callback(
    cbs, on_ਅੰਕੜੇ_chunk_recv);
  nghttp2_session_callbacks_ਸੈੱਟ_on_ਧਾਰਾ_close_callback(cbs, on_ਧਾਰਾ_close);
  nghttp2_session_callbacks_ਸੈੱਟ_on_ਸਿਰਲੇਖ_callback(cbs, on_ਸਿਰਲੇਖ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  nghttp2_session_callbacks_ਸੈੱਟ_error_callback(cbs, error_callback);
#ਜੇ_ਅੰਤ

  /* The nghttp2 session is not yet setup, do it */
  rc = h2_client_new(cf, cbs);
  ਜੇ(rc) {
    failf(ਅੰਕੜੇ, "Could not initialize nghttp2");
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ctx->max_concurrent_streams = ਅੰਕੜੇ->multi ?
    Curl_multi_max_concurrent_streams(ਅੰਕੜੇ->multi) :
    DEFAULT_MAX_CONCURRENT_STREAMS;

  ਜੇ(ctx->ਰਾਹੀਂ_h1_upgrade) {
    /* HTTP/1.1 Upgrade issued. H2 Settings have already been submitted
     * in the H1 request and we upgrade from there. This stream
     * is opened implicitly as #1. */
    uint8_t binsettings[H2_BINSETTINGS_LEN];
    ssize_t rclen;
    size_t binlen; /* length of the binsettings data */

    rclen = populate_binsettings(binsettings, ਅੰਕੜੇ);

    ਜੇ(!curlx_sztouz(rclen, &binlen) || !binlen) {
      failf(ਅੰਕੜੇ, "nghttp2 unexpectedly failed on pack_settings_payload");
      ਨਤੀਜਾ = CURLE_FAILED_INIT;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    ਨਤੀਜਾ = http2_ਅੰਕੜੇ_ਤਿਆਰੀ(cf, ਅੰਕੜੇ, &ਧਾਰਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    DEBUGASSERT(ਧਾਰਾ);
    ਧਾਰਾ->id = 1;
    /* queue SETTINGS frame (again) */
    rc = Curl_ngh2_session_upgrade2(
      &ctx->ngh2, binsettings, binlen, ਅੰਕੜੇ->state.httpreq == HTTPREQ_ਸਿਰ,
      ਨਿਰਾ);
    ਜੇ(rc) {
      failf(ਅੰਕੜੇ, "nghttp2_session_upgrade2() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      ਨਤੀਜਾ = CURLE_HTTP2;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    rc = Curl_ngh2_session_ਸੈੱਟ_ਧਾਰਾ_user_ਅੰਕੜੇ(
      &ctx->ngh2, ਧਾਰਾ->id, ਧਾਰਾ);
    ਜੇ(rc) {
      infof(ਅੰਕੜੇ, "http/2: failed to set user_data for stream %d",
            ਧਾਰਾ->id);
      DEBUGASSERT(0);
    }
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "created session via Upgrade");
  }
  ਨਹੀਂਤਾਂ {
    nghttp2_settings_ਇੰਦਰਾਜ iv[H2_SETTINGS_IV_LEN];
    size_t ivlen;

    ivlen = populate_settings(iv, ਅੰਕੜੇ, ctx);
    rc = Curl_ngh2_submit_settings(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, iv, ivlen);
    ਜੇ(rc) {
      failf(ਅੰਕੜੇ, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      ਨਤੀਜਾ = CURLE_HTTP2;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  rc = Curl_ngh2_session_ਸੈੱਟ_local_ਖਿੜਕੀ_ਮਾਪ(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, HTTP2_HUGE_ਖਿੜਕੀ_ਮਾਪ);
  ਜੇ(rc) {
    failf(ਅੰਕੜੇ, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    ਨਤੀਜਾ = CURLE_HTTP2;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  /* all set, traffic will be send on connect */
  ਨਤੀਜਾ = CURLE_OK;
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "[0] created h2 session%s",
              ctx->ਰਾਹੀਂ_h1_upgrade ? " (via h1 upgrade)" : "");

ਬਾਹਰਲਾ:
  ਜੇ(cbs)
    nghttp2_session_callbacks_del(cbs);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_h2_connect(ਬਣਤਰ Curl_cfilter *cf,
                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              bool *ਪੂਰਾ)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;
  bool ਪਹਿਲਾ_ਸਮਾਂ = FALSE;

  ਜੇ(cf->connected) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  /* Connect the lower filters first */
  ਜੇ(!cf->ਅਗਲਾ->connected) {
    ਨਤੀਜਾ = Curl_conn_cf_connect(cf->ਅਗਲਾ, ਅੰਕੜੇ, ਪੂਰਾ);
    ਜੇ(ਨਤੀਜਾ || !*ਪੂਰਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  *ਪੂਰਾ = FALSE;

  CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
  DEBUGASSERT(ctx->initialized);
  ਜੇ(!ctx->ngh2.s) {
    ਨਤੀਜਾ = cf_h2_ctx_open(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    ਪਹਿਲਾ_ਸਮਾਂ = TRUE;
  }

  ਜੇ(!ਪਹਿਲਾ_ਸਮਾਂ) {
    ਨਤੀਜਾ = h2_ਤਰੱਕੀ_ingress(cf, ਅੰਕੜੇ, 0);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  /* Send out our SETTINGS and ACKs and such. If that blocks, we
   * have it buffered and can count this filter as being connected */
  ਨਤੀਜਾ = h2_ਤਰੱਕੀ_egress(cf, ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ && (ਨਤੀਜਾ != CURLE_AGAIN))
    ਜਾਓ ਬਾਹਰਲਾ;

  *ਪੂਰਾ = TRUE;
  cf->connected = TRUE;
  ਨਤੀਜਾ = CURLE_OK;

ਬਾਹਰਲਾ:
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "cf_connect() -> %d, %d, ", (ਪੂਰਨ)ਨਤੀਜਾ, *ਪੂਰਾ);
  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ cf_h2_destroy(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਜੇ(ctx) {
    cf_h2_ctx_free(ctx);
    cf->ctx = ਨਿਰਾ;
  }
}

ਸਥਿਰ CURLcode cf_h2_shutdown(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;
  CURLcode ਨਤੀਜਾ;
  ਪੂਰਨ rv;

  ਜੇ(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);

  ਜੇ(!ctx->ਭੇਜਿਆ_goaway) {
    ctx->ਭੇਜਿਆ_goaway = TRUE;
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, ctx->local_max_sid, 0,
      (ਅਚਲ uint8_t *)"shutdown", ਆਕਾਰ("shutdown"));
    ਜੇ(rv) {
      failf(ਅੰਕੜੇ, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  ਨਤੀਜਾ = CURLE_OK;
  ਜੇ(Curl_ngh2_session_want_write(&ctx->ngh2) ||
     !Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->outbufq))
    ਨਤੀਜਾ = h2_ਤਰੱਕੀ_egress(cf, ਅੰਕੜੇ);
  ਜੇ(!ਨਤੀਜਾ && Curl_ngh2_session_want_read(&ctx->ngh2))
    ਨਤੀਜਾ = h2_ਤਰੱਕੀ_ingress(cf, ਅੰਕੜੇ, 0);

  ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
    ਨਤੀਜਾ = CURLE_OK;

  *ਪੂਰਾ = (ctx->conn_closed ||
           (!ਨਤੀਜਾ && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2) &&
            Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->outbufq)));

ਬਾਹਰਲਾ:
  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  cf->shutdown = (ਨਤੀਜਾ || *ਪੂਰਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode http2_ਅੰਕੜੇ_pause(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 bool pause)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ = H2_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);

  DEBUGASSERT(ਅੰਕੜੇ);
  ਜੇ(ctx && Curl_ngh2_valid(&ctx->ngh2) && ਧਾਰਾ) {
    CURLcode ਨਤੀਜਾ;

    ਧਾਰਾ->write_paused = pause;
    ਨਤੀਜਾ = cf_h2_update_local_win(cf, ਅੰਕੜੇ, ਧਾਰਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    /* attempt to send the window update */
    (ਖਾਲੀ)h2_ਤਰੱਕੀ_egress(cf, ਅੰਕੜੇ);

    ਜੇ(!pause) {
      /* Unpausing a h2 transfer, requires it to be run again. The server
       * may send new DATA on us increasing the flow window, and it may
       * not. We may have already buffered and exhausted the new window
       * by operating on things in flight during the handling of other
       * transfers. */
      Curl_multi_mark_dirty(ਅੰਕੜੇ);
    }
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "[%d] stream now %spaused", ਧਾਰਾ->id,
                pause ? "" : "un");
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode cf_h2_cntrl(ਬਣਤਰ Curl_cfilter *cf,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਪੂਰਨ event, ਪੂਰਨ arg1, ਖਾਲੀ *arg2)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;

  (ਖਾਲੀ)arg2;

  CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
  ਚੋਣ(event) {
  ਹਾਲਤ CF_CTRL_ਅੰਕੜੇ_ਤਿਆਰੀ:
    ਰੋਕੋ;
  ਹਾਲਤ CF_CTRL_ਅੰਕੜੇ_PAUSE:
    ਨਤੀਜਾ = http2_ਅੰਕੜੇ_pause(cf, ਅੰਕੜੇ, (arg1 != 0));
    ਰੋਕੋ;
  ਹਾਲਤ CF_CTRL_FLUSH:
    ਨਤੀਜਾ = cf_h2_flush(cf, ਅੰਕੜੇ);
    ਰੋਕੋ;
  ਹਾਲਤ CF_CTRL_ਅੰਕੜੇ_ਪੂਰਾ:
    http2_ਅੰਕੜੇ_ਪੂਰਾ(cf, ਅੰਕੜੇ);
    ਰੋਕੋ;
  ਹਾਲਤ CF_CTRL_CONN_INFO_UPDATE:
    ਜੇ(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_ਵੇਖਿਆ = 20;
      Curl_conn_ਸੈੱਟ_multiplex(cf->conn);
    }
    ਰੋਕੋ;
  ਮੂਲ:
    ਰੋਕੋ;
  }
  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ bool cf_h2_ਅੰਕੜੇ_pending(ਬਣਤਰ Curl_cfilter *cf,
                               ਅਚਲ ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;

  ਜੇ(ctx && !Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->inbufq))
    ਵਾਪਸ TRUE;
  ਵਾਪਸ cf->ਅਗਲਾ ? cf->ਅਗਲਾ->cft->has_ਅੰਕੜੇ_pending(cf->ਅਗਲਾ, ਅੰਕੜੇ) : FALSE;
}

ਸਥਿਰ bool cf_h2_is_alive(ਬਣਤਰ Curl_cfilter *cf,
                           ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           bool *ਨਿਵੇਸ਼_pending)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  bool alive;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;

  *ਨਿਵੇਸ਼_pending = FALSE;
  CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
           http2_connisalive(cf, ਅੰਕੜੇ, ਨਿਵੇਸ਼_pending));
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "conn alive -> %d, input_pending=%d",
              alive, *ਨਿਵੇਸ਼_pending);
  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  ਵਾਪਸ alive;
}

ਸਥਿਰ CURLcode cf_h2_keep_alive(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;

  CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
  ਨਤੀਜਾ = http2_send_ping(cf, ਅੰਕੜੇ);
  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_h2_query(ਬਣਤਰ Curl_cfilter *cf,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਪੂਰਨ query, ਪੂਰਨ *pres1, ਖਾਲੀ *pres2)
{
  ਬਣਤਰ cf_h2_ctx *ctx = cf->ctx;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;
  size_t effective_max;

  ਚੋਣ(query) {
  ਹਾਲਤ CF_QUERY_MAX_CONCURRENT:
    DEBUGASSERT(pres1);

    CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
    ਜੇ(!Curl_ngh2_valid(&ctx->ngh2) ||
       !Curl_ngh2_session_check_request_allowed(&ctx->ngh2)) {
      /* the limit is what we have in use right now */
      effective_max = cf->conn->attached_xfers;
    }
    ਨਹੀਂਤਾਂ {
      effective_max = ctx->max_concurrent_streams;
    }
    *pres1 = (effective_max > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) ? ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ : (ਪੂਰਨ)effective_max;
    CF_ਅੰਕੜੇ_RESTORE(cf, save);
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ CF_QUERY_ਧਾਰਾ_ERROR: {
    ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ = H2_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);
    *pres1 = ਧਾਰਾ ? (ਪੂਰਨ)ਧਾਰਾ->error : 0;
    ਵਾਪਸ CURLE_OK;
  }
  ਹਾਲਤ CF_QUERY_NEED_FLUSH: {
    ਬਣਤਰ h2_ਧਾਰਾ_ctx *ਧਾਰਾ = H2_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ);
    ਜੇ(!Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ctx->outbufq) ||
       (ਧਾਰਾ && !Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&ਧਾਰਾ->sendbuf))) {
      *pres1 = TRUE;
      ਵਾਪਸ CURLE_OK;
    }
    ਰੋਕੋ;
  }
  ਹਾਲਤ CF_QUERY_HTTP_ਸੰਸਕਰਣ:
    *pres1 = 20;
    ਵਾਪਸ CURLE_OK;
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ cf->ਅਗਲਾ ?
    cf->ਅਗਲਾ->cft->query(cf->ਅਗਲਾ, ਅੰਕੜੇ, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ਬਣਤਰ Curl_cftype Curl_cft_nghttp2 = {
  "HTTP/2",
  CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  CURL_LOG_LVL_NONE,
  cf_h2_destroy,
  cf_h2_connect,
  cf_h2_shutdown,
  cf_h2_adjust_pollset,
  cf_h2_ਅੰਕੜੇ_pending,
  cf_h2_send,
  cf_h2_recv,
  cf_h2_cntrl,
  cf_h2_is_alive,
  cf_h2_keep_alive,
  cf_h2_query,
};

ਸਥਿਰ CURLcode http2_cfilter_ਜੋੜੋ(ਬਣਤਰ Curl_cfilter **pcf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ connectdata *conn,
                                  int8_t sockindex,
                                  bool ਰਾਹੀਂ_h1_upgrade)
{
  ਬਣਤਰ Curl_cfilter *cf = ਨਿਰਾ;
  ਬਣਤਰ cf_h2_ctx *ctx;
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  DEBUGASSERT(ਅੰਕੜੇ->conn);
  ctx = curlx_calloc(1, ਆਕਾਰ(*ctx));
  ਜੇ(!ctx)
    ਜਾਓ ਬਾਹਰਲਾ;
  cf_h2_ctx_init(ctx, ਰਾਹੀਂ_h1_upgrade);

  ਨਤੀਜਾ = Curl_cf_create(&cf, &Curl_cft_nghttp2, ctx);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ctx = ਨਿਰਾ;
  Curl_conn_cf_ਜੋੜੋ(ਅੰਕੜੇ, conn, sockindex, cf);

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ)
    cf_h2_ctx_free(ctx);
  *pcf = ਨਤੀਜਾ ? ਨਿਰਾ : cf;
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode http2_cfilter_ਪਾਓ_after(ਬਣਤਰ Curl_cfilter *cf,
                                           ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                           bool ਰਾਹੀਂ_h1_upgrade)
{
  ਬਣਤਰ Curl_cfilter *cf_h2 = ਨਿਰਾ;
  ਬਣਤਰ cf_h2_ctx *ctx;
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  ctx = curlx_calloc(1, ਆਕਾਰ(*ctx));
  ਜੇ(!ctx)
    ਜਾਓ ਬਾਹਰਲਾ;
  cf_h2_ctx_init(ctx, ਰਾਹੀਂ_h1_upgrade);

  ਨਤੀਜਾ = Curl_cf_create(&cf_h2, &Curl_cft_nghttp2, ctx);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ctx = ਨਿਰਾ;
  Curl_conn_cf_ਪਾਓ_after(cf, cf_h2);

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ)
    cf_h2_ctx_free(ctx);
  ਵਾਪਸ ਨਤੀਜਾ;
}

bool Curl_http2_may_switch(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(Curl_conn_http_ਸੰਸਕਰਣ(ਅੰਕੜੇ, ਅੰਕੜੇ->conn) < 20 &&
     (ਅੰਕੜੇ->state.http_neg.wanted & CURL_HTTP_V2x) &&
     ਅੰਕੜੇ->state.http_neg.h2_ਪੂਰਵ_knowledge) {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
    ਜੇ(ਅੰਕੜੇ->conn->ਬਿੱਟ.origin_is_proxy) {
      infof(ਅੰਕੜੇ, "Ignoring HTTP/2 prior knowledge due to proxy");
      ਵਾਪਸ FALSE;
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ TRUE;
  }
  ਵਾਪਸ FALSE;
}

CURLcode Curl_http2_switch(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf;
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(Curl_conn_http_ਸੰਸਕਰਣ(ਅੰਕੜੇ, ਅੰਕੜੇ->conn) < 20);

  ਨਤੀਜਾ = http2_cfilter_ਜੋੜੋ(&cf, ਅੰਕੜੇ, ਅੰਕੜੇ->conn, FIRSTSOCKET, FALSE);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "switching connection to HTTP/2");

  ਜੇ(cf->ਅਗਲਾ) {
    bool ਪੂਰਾ;
    ਵਾਪਸ Curl_conn_cf_connect(cf, ਅੰਕੜੇ, &ਪੂਰਾ);
  }
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_http2_switch_at(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cfilter *cf_h2;
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(Curl_conn_http_ਸੰਸਕਰਣ(ਅੰਕੜੇ, ਅੰਕੜੇ->conn) < 20);

  ਨਤੀਜਾ = http2_cfilter_ਪਾਓ_after(cf, ਅੰਕੜੇ, FALSE);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  cf_h2 = cf->ਅਗਲਾ;

  ਜੇ(cf_h2->ਅਗਲਾ) {
    bool ਪੂਰਾ;
    ਵਾਪਸ Curl_conn_cf_connect(cf_h2, ਅੰਕੜੇ, &ਪੂਰਾ);
  }
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_http2_upgrade(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ connectdata *conn, int8_t sockindex,
                            ਅਚਲ ਅੱਖਰ *mem, size_t nread)
{
  ਬਣਤਰ Curl_cfilter *cf;
  ਬਣਤਰ cf_h2_ctx *ctx;
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(Curl_conn_http_ਸੰਸਕਰਣ(ਅੰਕੜੇ, conn) < 20);

  ਨਤੀਜਾ = http2_cfilter_ਜੋੜੋ(&cf, ਅੰਕੜੇ, conn, sockindex, TRUE);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "upgrading connection to HTTP/2");

  DEBUGASSERT(cf->cft == &Curl_cft_nghttp2);
  ctx = cf->ctx;

  ਅੰਕੜੇ->req.httpversion_ਭੇਜਿਆ = 20; /* it is an h2 request now */
  ਅੰਕੜੇ->req.ਸਿਰਲੇਖ = TRUE; /* we expect the real response to come in h2 */
  ਅੰਕੜੇ->req.headerline = 0; /* restart the header line counter */

  ਜੇ(nread > 0) {
    /* Remaining data from the protocol switch reply is already using
     * the switched protocol, ie. HTTP/2. We add that to the network
     * inbufq. */
    size_t copied;

    ਨਤੀਜਾ = Curl_bufq_write(&ctx->inbufq,
                             (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)mem, nread, &copied);
    ਜੇ(ਨਤੀਜਾ) {
      failf(ਅੰਕੜੇ, "error on copying HTTP Upgrade response: %d", (ਪੂਰਨ)ਨਤੀਜਾ);
      ਵਾਪਸ CURLE_RECV_ERROR;
    }
    ਜੇ(copied < nread) {
      failf(ਅੰਕੜੇ, "connection buffer size could not take all data "
            "from HTTP Upgrade response header: copied=%zu, datalen=%zu",
            copied, nread);
      ਵਾਪਸ CURLE_HTTP2;
    }
    infof(ਅੰਕੜੇ, "Copied HTTP/2 data in stream buffer to connection buffer"
          " after upgrade: len=%zu", nread);
  }

  ਜੇ(cf->ਅਗਲਾ) {
    bool ਪੂਰਾ;
    ਨਤੀਜਾ = Curl_conn_cf_connect(cf, ਅੰਕੜੇ, &ਪੂਰਾ);
    ਜੇ(!ਨਤੀਜਾ)
      cf->cft->cntrl(cf, ਅੰਕੜੇ, CF_CTRL_CONN_INFO_UPDATE, 0, ਨਿਰਾ);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Only call this function for a transfer that already got an HTTP/2
   CURLE_HTTP2_STREAM error! */
bool Curl_h2_http_1_1_error(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(Curl_conn_http_ਸੰਸਕਰਣ(ਅੰਕੜੇ, ਅੰਕੜੇ->conn) == 20) {
    ਪੂਰਨ err = Curl_conn_get_ਧਾਰਾ_error(ਅੰਕੜੇ, ਅੰਕੜੇ->conn, FIRSTSOCKET);
    ਵਾਪਸ err == NGHTTP2_HTTP_1_1_REQUIRED;
  }
  ਵਾਪਸ FALSE;
}

ਖਾਲੀ *Curl_nghttp2_malloc(size_t ਮਾਪ, ਖਾਲੀ *user_ਅੰਕੜੇ)
{
  (ਖਾਲੀ)user_ਅੰਕੜੇ;
  ਵਾਪਸ Curl_cmalloc(ਮਾਪ);
}

ਖਾਲੀ Curl_nghttp2_free(ਖਾਲੀ *ptr, ਖਾਲੀ *user_ਅੰਕੜੇ)
{
  (ਖਾਲੀ)user_ਅੰਕੜੇ;
  Curl_cfree(ptr);
}

ਖਾਲੀ *Curl_nghttp2_calloc(size_t nmemb, size_t ਮਾਪ, ਖਾਲੀ *user_ਅੰਕੜੇ)
{
  (ਖਾਲੀ)user_ਅੰਕੜੇ;
  ਵਾਪਸ Curl_ccalloc(nmemb, ਮਾਪ);
}

ਖਾਲੀ *Curl_nghttp2_realloc(ਖਾਲੀ *ptr, size_t ਮਾਪ, ਖਾਲੀ *user_ਅੰਕੜੇ)
{
  (ਖਾਲੀ)user_ਅੰਕੜੇ;
  ਵਾਪਸ Curl_crealloc(ptr, ਮਾਪ);
}

#ਨਹੀਂਤਾਂ /* CURL_DISABLE_HTTP || !USE_NGHTTP2 */

/* Satisfy external references even if http2 is not compiled in. */

ਅੱਖਰ *curl_pushheader_bynum(ਬਣਤਰ curl_pushheaders *h, size_t ਅੰਕ)
{
  (ਖਾਲੀ)h;
  (ਖਾਲੀ)ਅੰਕ;
  ਵਾਪਸ ਨਿਰਾ;
}

ਅੱਖਰ *curl_pushheader_byname(ਬਣਤਰ curl_pushheaders *h, ਅਚਲ ਅੱਖਰ *name)
{
  (ਖਾਲੀ)h;
  (ਖਾਲੀ)name;
  ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
