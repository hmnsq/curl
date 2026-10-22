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

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
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

#ಆದರೆ NGHTTP2_ಆವೃತ್ತಿ_ಅಂಕೆ < 0x010f00
#ದೋಷ "nghttp2 1.15.0 or greater required"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು NGHTTP2_HAS_ಗಣ_LOCAL_ಕಿಟಕಿ_ಗಾತ್ರ 1

/* buffer dimensioning:
 * use 16K as chunk size, as that fits H2 DATA frames well */
#ವ್ಯಾಖ್ಯಾನಿಸು H2_CHUNK_ಗಾತ್ರ           (16 * 1024)
/* connection window size */
#ವ್ಯಾಖ್ಯಾನಿಸು H2_CONN_ಕಿಟಕಿ_ಗಾತ್ರ     (10 * 1024 * 1024)
/* on receiving from TLS, we prep for holding a full stream window */
#ವ್ಯಾಖ್ಯಾನಿಸು H2_NW_RECV_CHUNKS       (H2_CONN_ಕಿಟಕಿ_ಗಾತ್ರ / H2_CHUNK_ಗಾತ್ರ)
/* on send into TLS, we want to accumulate small frames */
#ವ್ಯಾಖ್ಯಾನಿಸು H2_NW_SEND_CHUNKS       1
/* this is how much we want "in flight" for a stream, unthrottled */
#ವ್ಯಾಖ್ಯಾನಿಸು H2_ಹರಿವು_ಕಿಟಕಿ_ಗಾತ್ರ_MAX   (10 * 1024 * 1024)
/* this is how much we want "in flight" for a stream, initially, IFF
 * nghttp2 allows us to tweak the local window size. */
#ಆದರೆ NGHTTP2_HAS_ಗಣ_LOCAL_ಕಿಟಕಿ_ಗಾತ್ರ
#ವ್ಯಾಖ್ಯಾನಿಸು H2_ಹರಿವು_ಕಿಟಕಿ_ಗಾತ್ರ_INITIAL  (64 * 1024)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು H2_ಹರಿವು_ಕಿಟಕಿ_ಗಾತ್ರ_INITIAL H2_ಹರಿವು_ಕಿಟಕಿ_ಗಾತ್ರ_MAX
#ಆದರೆ_ಅಂತ್ಯ
/* keep smaller stream upload buffer (default h2 window size) to have
 * our progress bars and "upload done" reporting closer to reality */
#ವ್ಯಾಖ್ಯಾನಿಸು H2_ಹರಿವು_SEND_CHUNKS   ((64 * 1024) / H2_CHUNK_ಗಾತ್ರ)
/* spare chunks we keep for a full window */
#ವ್ಯಾಖ್ಯಾನಿಸು H2_ಹರಿವು_POOL_SPARES   (H2_CONN_ಕಿಟಕಿ_ಗಾತ್ರ / H2_CHUNK_ಗಾತ್ರ)

/* We need to accommodate the max number of streams with their window sizes on
 * the overall connection. Streams might become PAUSED which will block their
 * received QUOTA in the connection window. If we run out of space, the server
 * is blocked from sending us any data. See #10988 for an issue with this. */
#ವ್ಯಾಖ್ಯಾನಿಸು HTTP2_HUGE_ಕಿಟಕಿ_ಗಾತ್ರ (100 * H2_ಹರಿವು_ಕಿಟಕಿ_ಗಾತ್ರ_MAX)

#ವ್ಯಾಖ್ಯಾನಿಸು H2_SETTINGS_IV_LEN 3
#ವ್ಯಾಖ್ಯಾನಿಸು H2_BINSETTINGS_LEN 80

ರಚನೆ cf_h2_ctx {
  ರಚನೆ curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  ರಚನೆ cf_call_ದತ್ತಾಂಶ call_ದತ್ತಾಂಶ;

  ರಚನೆ bufq inbufq;           /* network input */
  ರಚನೆ bufq outbufq;          /* network output */
  ರಚನೆ bufc_pool ಹರಿವು_bufcp; /* spares for stream buffers */
  ರಚನೆ dynbuf scratch;        /* scratch buffer for temp use */

  ರಚನೆ u32_ptrset streams; /* hash of `data->mid` to `h2_stream_ctx` */
  size_t drain_ಒಟ್ಟು; /* sum of all stream's UrlState drain */
  uint32_t initial_win_ಗಾತ್ರ; /* current initial window size (settings) */
  uint32_t max_concurrent_streams;
  uint32_t goaway_error;        /* goaway error code from server */
  int32_t remote_max_sid;       /* max id processed by server */
  int32_t local_max_sid;        /* max id processed by us */
  ಬಿಟ್(initialized);
  ಬಿಟ್(ಮೂಲಕ_h1_upgrade);
  ಬಿಟ್(conn_closed);
  ಬಿಟ್(rcvd_goaway);
  ಬಿಟ್(ಕಳುಹಿಸಿದ_goaway);
  ಬಿಟ್(enable_ತಳ್ಳು);
  ಬಿಟ್(nw_ಹೊರಹೋಗು_blocked);
};

/* How to access `call_data` from a cf_h2 filter */
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು CF_CTX_CALL_ದತ್ತಾಂಶ
#ವ್ಯಾಖ್ಯಾನಿಸು CF_CTX_CALL_ದತ್ತಾಂಶ(cf) ((ರಚನೆ cf_h2_ctx *)(cf)->ctx)->call_ದತ್ತಾಂಶ

/**
 * All about the H2 internals of a stream
 */
ರಚನೆ h2_ಹರಿವು_ctx {
  ರಚನೆ bufq sendbuf; /* request buffer */
  ರಚನೆ h1_req_parser h1; /* parsing the request */
  ರಚನೆ dynhds resp_trailers; /* response trailer fields */
  size_t resp_hds_len; /* amount of response header bytes in recvbuf */
  curl_off_t xfer_id;  /* transfer `id` owning this stream */
  curl_off_t nrcvd_ದತ್ತಾಂಶ;  /* number of DATA bytes received */

  ಅಕ್ಷರ **ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು;       /* allocated array */
  size_t ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು_ಬಳಸಿದ;  /* number of entries filled in */
  size_t ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು_alloc; /* number of entries allocated */

  int32_t id; /* HTTP/2 protocol identifier for stream */
  uint32_t mid; /* transfer `mid` owning this stream */
  uint32_t error; /* stream error code */
  CURLcode xfer_ಫಲಿತಾಂಶ; /* Result of writing out response */
  int32_t local_ಕಿಟಕಿ_ಗಾತ್ರ; /* the local recv window size */
  int32_t ಸ್ಥಿತಿ_ಸಂಕೇತ; /* HTTP response status code */
  ಬಿಟ್(resp_hds_complete); /* we have a complete, final response */
  ಬಿಟ್(closed); /* TRUE on stream close */
  ಬಿಟ್(reset);  /* TRUE on stream reset */
  ಬಿಟ್(reset_by_server);  /* TRUE on stream reset by server */
  ಬಿಟ್(close_handled); /* TRUE if stream closure is handled by libcurl */
  ಬಿಟ್(bodystarted);
  ಬಿಟ್(body_eos);    /* the complete body has been added to `sendbuf` and
                     * is being/has been processed from there. */
  ಬಿಟ್(write_paused);  /* stream write is paused */
};

ಸ್ಥಿರ ಶೂನ್ಯ free_ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು(ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು)
{
  size_t i;
  ಗಾಗಿ(i = 0; i < ಹರಿವು->ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು_ಬಳಸಿದ; i++)
    curlx_free(ಹರಿವು->ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು[i]);
  curlx_safefree(ಹರಿವು->ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು);
  ಹರಿವು->ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು_ಬಳಸಿದ = 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ h2_ಹರಿವು_ctx_free(ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು)
{
  Curl_bufq_free(&ಹರಿವು->sendbuf);
  Curl_h1_req_parse_free(&ಹರಿವು->h1);
  Curl_dynhds_free(&ಹರಿವು->resp_trailers);
  free_ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು(ಹರಿವು);
  curlx_free(ಹರಿವು);
}

ಸ್ಥಿರ ಶೂನ್ಯ h2_ಹರಿವು_hash_free(uint32_t id, ಶೂನ್ಯ *ಹರಿವು)
{
  (ಶೂನ್ಯ)id;
  DEBUGASSERT(ಹರಿವು);
  h2_ಹರಿವು_ctx_free((ರಚನೆ h2_ಹರಿವು_ctx *)ಹರಿವು);
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_h2_ctx_init(ರಚನೆ cf_h2_ctx *ctx, bool ಮೂಲಕ_h1_upgrade)
{
  Curl_bufcp_init(&ctx->ಹರಿವು_bufcp, H2_CHUNK_ಗಾತ್ರ, H2_ಹರಿವು_POOL_SPARES);
  Curl_bufq_initp(&ctx->inbufq, &ctx->ಹರಿವು_bufcp, H2_NW_RECV_CHUNKS, 0);
  Curl_bufq_initp(&ctx->outbufq, &ctx->ಹರಿವು_bufcp, H2_NW_SEND_CHUNKS, 0);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_ಶೀರ್ಷಿಕೆ);
  Curl_u32_ptrset_init(&ctx->streams, h2_ಹರಿವು_hash_free);
  ctx->remote_max_sid = INT32_MAX;
  ctx->ಮೂಲಕ_h1_upgrade = ಮೂಲಕ_h1_upgrade;
  ctx->initialized = TRUE;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_h2_ctx_free(ರಚನೆ cf_h2_ctx *ctx)
{
  ಆದರೆ(ctx && ctx->initialized) {
    Curl_ngh2_del(&ctx->ngh2);
    Curl_bufq_free(&ctx->inbufq);
    Curl_bufq_free(&ctx->outbufq);
    Curl_bufcp_free(&ctx->ಹರಿವು_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    memset(ctx, 0, ಗಾತ್ರ(*ctx));
  }
  curlx_free(ctx);
}

ಸ್ಥಿರ uint32_t cf_h2_initial_win_ಗಾತ್ರ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
#ಆದರೆ NGHTTP2_HAS_ಗಣ_LOCAL_ಕಿಟಕಿ_ಗಾತ್ರ
  /* If the transfer has a rate-limit lower than the default initial
   * stream window size, use that. It needs to be at least 8k or servers
   * may be unhappy. */
  curl_off_t rps = Curl_rlimit_ಪ್ರತಿ_ಹೆಜ್ಜೆ(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.rlimit);
  ಆದರೆ((rps > 0) && (rps < H2_ಹರಿವು_ಕಿಟಕಿ_ಗಾತ್ರ_INITIAL))
    ಹಿಂತಿರುಗಿ CURLMAX((uint32_t)rps, 8192);
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ H2_ಹರಿವು_ಕಿಟಕಿ_ಗಾತ್ರ_INITIAL;
}

ಸ್ಥಿರ size_t populate_settings(nghttp2_settings_ನಮೂದು *iv,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ cf_h2_ctx *ctx)
{
  iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
  iv[0].ಮೌಲ್ಯ = Curl_multi_max_concurrent_streams(ದತ್ತಾಂಶ->multi);

  iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_ಕಿಟಕಿ_ಗಾತ್ರ;
  iv[1].ಮೌಲ್ಯ = cf_h2_initial_win_ಗಾತ್ರ(ದತ್ತಾಂಶ);
  ಆದರೆ(ctx)
    ctx->initial_win_ಗಾತ್ರ = iv[1].ಮೌಲ್ಯ;
  iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_ತಳ್ಳು;
  iv[2].ಮೌಲ್ಯ = !!ದತ್ತಾಂಶ->multi->ತಳ್ಳು_cb;

  ಹಿಂತಿರುಗಿ 3;
}

ಸ್ಥಿರ ssize_t populate_binsettings(uint8_t *binsettings,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  nghttp2_settings_ನಮೂದು iv[H2_SETTINGS_IV_LEN];
  size_t ivlen;

  ivlen = populate_settings(iv, ದತ್ತಾಂಶ, ಖಾಲಿ);
  /* this returns number of bytes it wrote or a negative number on error. */
  ಹಿಂತಿರುಗಿ nghttp2_pack_settings_payload(binsettings, H2_BINSETTINGS_LEN,
                                       iv, ivlen);
}

ಸ್ಥಿರ CURLcode cf_h2_update_settings(ರಚನೆ cf_h2_ctx *ctx,
                                      uint32_t initial_win_ಗಾತ್ರ)
{
  nghttp2_settings_ನಮೂದು ನಮೂದು;
  ನಮೂದು.settings_id = NGHTTP2_SETTINGS_INITIAL_ಕಿಟಕಿ_ಗಾತ್ರ;
  ನಮೂದು.ಮೌಲ್ಯ = initial_win_ಗಾತ್ರ;
  ಆದರೆ(Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, &ನಮೂದು, 1))
    ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
  ctx->initial_win_ಗಾತ್ರ = initial_win_ಗಾತ್ರ;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ವ್ಯಾಖ್ಯಾನಿಸು H2_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ)                                         \
  ((ರಚನೆ h2_ಹರಿವು_ctx *)(                                             \
    (ದತ್ತಾಂಶ) ? Curl_u32_ptrset_get(&(ctx)->streams, (ದತ್ತಾಂಶ)->mid) : ಖಾಲಿ))

ಸ್ಥಿರ ರಚನೆ h2_ಹರಿವು_ctx *h2_ಹರಿವು_ctx_create(ರಚನೆ cf_h2_ctx *ctx,
                                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು;

  ಹರಿವು = curlx_calloc(1, ಗಾತ್ರ(*ಹರಿವು));
  ಆದರೆ(!ಹರಿವು)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  ಹರಿವು->id = -1;
  ಹರಿವು->xfer_id = ದತ್ತಾಂಶ->id;
  ಹರಿವು->mid = ದತ್ತಾಂಶ->mid;
  Curl_bufq_initp(&ಹರಿವು->sendbuf, &ctx->ಹರಿವು_bufcp,
                  H2_ಹರಿವು_SEND_CHUNKS, BUFQ_OPT_NONE);
  Curl_h1_req_parse_init(&ಹರಿವು->h1, H1_PARSE_DEFAULT_MAX_ಸಾಲು_LEN);
  Curl_dynhds_init(&ಹರಿವು->resp_trailers,
                   MAX_HTTP_RESP_ಶೀರ್ಷಿಕೆ_ಎಣಿಕೆ, DYN_HTTP_REQUEST);
  ಹರಿವು->bodystarted = FALSE;
  ಹರಿವು->ಸ್ಥಿತಿ_ಸಂಕೇತ = -1;
  ಹರಿವು->closed = FALSE;
  ಹರಿವು->close_handled = FALSE;
  ಹರಿವು->error = NGHTTP2_NO_ERROR;
  ಹರಿವು->local_ಕಿಟಕಿ_ಗಾತ್ರ = H2_ಹರಿವು_ಕಿಟಕಿ_ಗಾತ್ರ_INITIAL;
  ಹರಿವು->nrcvd_ದತ್ತಾಂಶ = 0;
  ಹಿಂತಿರುಗಿ ಹರಿವು;
}

ಸ್ಥಿರ ರಚನೆ Curl_easy *cf_h2_get_ಹರಿವು_xfer(ರಚನೆ Curl_cfilter *cf,
                                               int32_t ಹರಿವು_id,
                                               ರಚನೆ h2_ಹರಿವು_ctx **pstream)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು =
    Curl_ngh2_session_get_ಹರಿವು_user_ದತ್ತಾಂಶ(&ctx->ngh2, ಹರಿವು_id);
  ಆದರೆ(pstream)
    *pstream = ಹರಿವು;
  ಆದರೆ(ಹರಿವು && (ಹರಿವು->mid != UINT32_MAX)) {
    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, *call_ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
    ಆದರೆ(!call_ದತ್ತಾಂಶ || !call_ದತ್ತಾಂಶ->multi) {
      DEBUGASSERT(0);
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ದತ್ತಾಂಶ = Curl_multi_get_easy(call_ದತ್ತಾಂಶ->multi, ಹರಿವು->mid);
    ಆದರೆ(ದತ್ತಾಂಶ && (ದತ್ತಾಂಶ->id == ಹರಿವು->xfer_id))
      ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ;
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NGHTTP2_HAS_ಗಣ_LOCAL_ಕಿಟಕಿ_ಗಾತ್ರ
ಸ್ಥಿರ int32_t cf_h2_get_desired_local_win(ರಚನೆ Curl_cfilter *cf,
                                           ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  curl_off_t avail = Curl_rlimit_avail(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ.dl.rlimit, ಖಾಲಿ);

  (ಶೂನ್ಯ)cf;
  ಆದರೆ(avail < CURL_OFF_T_MAX) { /* limit in place */
    ಆದರೆ(avail <= 0)
      ಹಿಂತಿರುಗಿ 0;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(avail < H2_ಹರಿವು_ಕಿಟಕಿ_ಗಾತ್ರ_MAX)
      ಹಿಂತಿರುಗಿ (int32_t)avail;
  }
  ಹಿಂತಿರುಗಿ H2_ಹರಿವು_ಕಿಟಕಿ_ಗಾತ್ರ_MAX;
}

ಸ್ಥಿರ CURLcode cf_h2_update_local_win(ರಚನೆ Curl_cfilter *cf,
                                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  int32_t dwsize;
  ಪೂರ್ಣಾಂಕ rv;

  dwsize = (ಹರಿವು->write_paused || ಹರಿವು->xfer_ಫಲಿತಾಂಶ) ?
           0 : cf_h2_get_desired_local_win(cf, ದತ್ತಾಂಶ);
  ಆದರೆ(dwsize != ಹರಿವು->local_ಕಿಟಕಿ_ಗಾತ್ರ) {
    int32_t wsize = Curl_ngh2_session_get_ಹರಿವು_effective_local_ಕಿಟಕಿ_ಗಾತ್ರ(
      &ctx->ngh2, ಹರಿವು->id);
    ಆದರೆ(dwsize > wsize) {
      rv = Curl_ngh2_session_ಗಣ_local_ಕಿಟಕಿ_ಗಾತ್ರ(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, ಹರಿವು->id, dwsize);
      ಆದರೆ(rv) {
        failf(ದತ್ತಾಂಶ, "[%d] nghttp2 set_local_window_size(%d) failed: "
              "%s(%d)", ಹರಿವು->id, dwsize, nghttp2_strerror(rv), rv);
        ಹಿಂತಿರುಗಿ CURLE_HTTP2;
      }
      rv = Curl_ngh2_submit_ಕಿಟಕಿ_update(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, ಹರಿವು->id, dwsize - wsize);
      ಆದರೆ(rv) {
        failf(ದತ್ತಾಂಶ, "[%d] nghttp2_submit_window_update() failed: "
              "%s(%d)", ಹರಿವು->id, nghttp2_strerror(rv), rv);
        ಹಿಂತಿರುಗಿ CURLE_HTTP2;
      }
      ಹರಿವು->local_ಕಿಟಕಿ_ಗಾತ್ರ = dwsize;
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] local window update by %d",
                  ಹರಿವು->id, dwsize - wsize);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      rv = Curl_ngh2_session_ಗಣ_local_ಕಿಟಕಿ_ಗಾತ್ರ(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, ಹರಿವು->id, dwsize);
      ಆದರೆ(rv) {
        failf(ದತ್ತಾಂಶ, "[%d] nghttp2_session_set_local_window_size() failed: "
              "%s(%d)", ಹರಿವು->id, nghttp2_strerror(rv), rv);
        ಹಿಂತಿರುಗಿ CURLE_HTTP2;
      }
      ಹರಿವು->local_ಕಿಟಕಿ_ಗಾತ್ರ = dwsize;
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] local window size now %d",
                  ಹರಿವು->id, dwsize);
    }
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ಇಲ್ಲದಿದ್ದರೆ /* NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

ಸ್ಥಿರ CURLcode cf_h2_update_local_win(ರಚನೆ Curl_cfilter *cf,
                                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು)
{
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)ಹರಿವು;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
#ಆದರೆ_ಅಂತ್ಯ /* !NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

ಸ್ಥಿರ CURLcode http2_ದತ್ತಾಂಶ_ಸಿದ್ಧತೆ(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ h2_ಹರಿವು_ctx **pstream)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು;

  (ಶೂನ್ಯ)cf;
  DEBUGASSERT(ದತ್ತಾಂಶ);

  ಆದರೆ(!ದತ್ತಾಂಶ)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  ಹರಿವು = H2_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);
  ಆದರೆ(ಹರಿವು) {
    *pstream = ಹರಿವು;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  ಹರಿವು = h2_ಹರಿವು_ctx_create(ctx, ದತ್ತಾಂಶ);
  ಆದರೆ(!ಹರಿವು)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಆದರೆ(Curl_u32_ptrset_ಗಣ(&ctx->streams, ದತ್ತಾಂಶ->mid, ಹರಿವು)) {
    h2_ಹರಿವು_ctx_free(ಹರಿವು);
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  *pstream = ಹರಿವು;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode nw_ಹೊರಹೋಗು_flush(ರಚನೆ Curl_cfilter *cf,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->outbufq))
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಫಲಿತಾಂಶ = Curl_cf_send_bufq(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, &ctx->outbufq, ಖಾಲಿ, 0,
                             &nwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_ಹೊರಹೋಗು_blocked = 1;
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಹಿಂತಿರುಗಿ Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

ಸ್ಥಿರ ಶೂನ್ಯ http2_ದತ್ತಾಂಶ_ಮುಗಿದ(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು = H2_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);

  DEBUGASSERT(ctx);
  ಆದರೆ(!ಹರಿವು || !ctx->initialized)
    ಹಿಂತಿರುಗಿ;

  ಆದರೆ(Curl_ngh2_valid(&ctx->ngh2)) {
    bool flush_egress = FALSE;
    /* returns error if stream not known, which is fine here */
    (ಶೂನ್ಯ)Curl_ngh2_session_ಗಣ_ಹರಿವು_user_ದತ್ತಾಂಶ(&ctx->ngh2, ಹರಿವು->id, ಖಾಲಿ);

    ಆದರೆ(!ಹರಿವು->closed && ಹರಿವು->id > 0) {
      /* RST_STREAM */
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] premature DATA_DONE, RST stream",
                  ಹರಿವು->id);
      ಹರಿವು->closed = TRUE;
      ಹರಿವು->reset = TRUE;
      Curl_ngh2_submit_rst_ಹರಿವು(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                  ಹರಿವು->id, NGHTTP2_ಹರಿವು_CLOSED);
      flush_egress = TRUE;
    }

    ಆದರೆ(flush_egress) {
      (ಶೂನ್ಯ)Curl_ngh2_session_send(&ctx->ngh2);
      (ಶೂನ್ಯ)nw_ಹೊರಹೋಗು_flush(cf, ದತ್ತಾಂಶ);
    }
  }

  Curl_u32_ptrset_unset(&ctx->streams, ದತ್ತಾಂಶ->mid);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ h2_client_new(ರಚನೆ Curl_cfilter *cf,
                         nghttp2_session_callbacks *cbs)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { ಖಾಲಿ, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  ಪೂರ್ಣಾಂಕ rc = nghttp2_option_new(&o);
  ಆದರೆ(rc)
    ಹಿಂತಿರುಗಿ rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_ಗಣ_no_auto_ಕಿಟಕಿ_update(o, 1);
#ಆದರೆ NGHTTP2_ಆವೃತ್ತಿ_ಅಂಕೆ >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_ಗಣ_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#ಆದರೆ_ಅಂತ್ಯ
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  ಹಿಂತಿರುಗಿ rc;
}

/*
 * Returns nonzero if current HTTP/2 session should be closed.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ should_close_session(ರಚನೆ cf_h2_ctx *ctx)
{
  ಹಿಂತಿರುಗಿ ctx->drain_ಒಟ್ಟು == 0 && !Curl_ngh2_session_want_read(&ctx->ngh2) &&
    !Curl_ngh2_session_want_write(&ctx->ngh2);
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
ಸ್ಥಿರ CURLcode h2_process_pending_ಒಳಹರಿವು(ರಚನೆ Curl_cfilter *cf,
                                         ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf;
  size_t blen, nread;
  ssize_t rv;

  ಆಗ(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {
    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ಸ್ಥಿರಾಂಕ uint8_t *)buf, blen);
    ಆದರೆ(!curlx_sztouz(rv, &nread)) {
      failf(ದತ್ತಾಂಶ, "nghttp2 recv error %zd: %s", rv, nghttp2_strerror((ಪೂರ್ಣಾಂಕ)rv));
      ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    ಆದರೆ(Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->inbufq)) {
      ನಿಲ್ಲಿಸು;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }

  ಆದರೆ(Curl_ngh2_session_check_request_allowed(&ctx->ngh2) == 0) {
    /* No more requests are allowed in the current session, so
       the connection may not be reused. This is set when a
       GOAWAY frame has been received or when the limit of stream
       identifiers has been reached. */
    CURL_TRC_M(ದತ್ತಾಂಶ, "http/2: No new requests allowed");
    connclose(cf->conn);
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * The server may send us data at any point (e.g. PING frames). Therefore, we
 * cannot assume that an HTTP/2 socket is dead because it is readable.
 *
 * Check the lower filters first and, if successful, peek at the socket
 * and distinguish between closed and data.
 */
ಸ್ಥಿರ bool http2_connisalive(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              bool *ಒಳಹರಿವು_pending)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *ಒಳಹರಿವು_pending = FALSE;
  ಆದರೆ(!cf->ಮುಂದಿನ || !cf->ಮುಂದಿನ->cft->is_alive(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, ಒಳಹರಿವು_pending))
    ಹಿಂತಿರುಗಿ FALSE;

  ಆದರೆ(*ಒಳಹರಿವು_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode ಫಲಿತಾಂಶ;
    size_t nread;

    *ಒಳಹರಿವು_pending = FALSE;
    ಫಲಿತಾಂಶ = Curl_cf_recv_bufq(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "connisalive, recv pending input -> %d, %zu",
                (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, nread);
    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      ಫಲಿತಾಂಶ = h2_process_pending_ಒಳಹರಿವು(cf, ದತ್ತಾಂಶ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        /* immediate error, considered dead */
        alive = FALSE;
      ಇಲ್ಲದಿದ್ದರೆ {
        alive = !should_close_session(ctx);
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  ಹಿಂತಿರುಗಿ alive;
}

ಸ್ಥಿರ CURLcode http2_send_ping(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  ಪೂರ್ಣಾಂಕ rc;

  rc = Curl_ngh2_submit_ping(&ctx->ngh2, 0, ZERO_NULL);
  ಆದರೆ(rc) {
    failf(ದತ್ತಾಂಶ, "nghttp2_submit_ping() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    ಹಿಂತಿರುಗಿ CURLE_HTTP2;
  }

  rc = Curl_ngh2_session_send(&ctx->ngh2);
  ಆದರೆ(rc) {
    failf(ದತ್ತಾಂಶ, "nghttp2_session_send() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Store nghttp2 version info in this buffer.
 */
ಶೂನ್ಯ Curl_http2_ver(ಅಕ್ಷರ *p, size_t len)
{
  nghttp2_info *h2 = nghttp2_ಆವೃತ್ತಿ(0);
  (ಶೂನ್ಯ)curl_msnprintf(p, len, "nghttp2/%s", h2->ಆವೃತ್ತಿ_str);
}

/*
 * The implementation of nghttp2_send_callback type. Here we write |data| with
 * size |length| to the network and return the number of bytes actually
 * written. See the documentation of nghttp2_send_callback for the details.
 */
ಸ್ಥಿರ ssize_t send_callback(nghttp2_session *h2,
                             ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t blen, ಪೂರ್ಣಾಂಕ flags,
                             ಶೂನ್ಯ *userp)
{
  ರಚನೆ Curl_cfilter *cf = userp;
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
  size_t nwritten;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  (ಶೂನ್ಯ)h2;
  (ಶೂನ್ಯ)flags;
  DEBUGASSERT(ದತ್ತಾಂಶ);

  ಆದರೆ(!cf->connected)
    ಫಲಿತಾಂಶ = Curl_bufq_write(&ctx->outbufq, buf, blen, &nwritten);
  ಇಲ್ಲದಿದ್ದರೆ
    ಫಲಿತಾಂಶ = Curl_cf_send_bufq(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, &ctx->outbufq, buf, blen,
                               &nwritten);

  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
      ctx->nw_ಹೊರಹೋಗು_blocked = 1;
      ಹಿಂತಿರುಗಿ NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(ದತ್ತಾಂಶ, "Failed sending HTTP2 data");
    ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  ಆದರೆ(!nwritten) {
    ctx->nw_ಹೊರಹೋಗು_blocked = 1;
    ಹಿಂತಿರುಗಿ NGHTTP2_ERR_WOULDBLOCK;
  }
  ಹಿಂತಿರುಗಿ (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

/* We pass a pointer to this struct in the push callback, but the contents of
   the struct are hidden from the user. */
ರಚನೆ curl_pushheaders {
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ;
  ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು;
  ಸ್ಥಿರಾಂಕ nghttp2_ತಳ್ಳು_promise *ಚೌಕಟ್ಟು;
};

/*
 * push header access function. Only to be used from within the push callback
 */
ಅಕ್ಷರ *curl_pushheader_bynum(ರಚನೆ curl_pushheaders *h, size_t ಅಂಕೆ)
{
  /* Verify that we got a good easy handle in the push header struct, mostly to
     detect rubbish input fast(er). */
  ಆದರೆ(!h || !GOOD_EASY_HANDLE(h->ದತ್ತಾಂಶ))
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(h->ಹರಿವು && ಅಂಕೆ < h->ಹರಿವು->ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು_ಬಳಸಿದ)
      ಹಿಂತಿರುಗಿ h->ಹರಿವು->ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು[ಅಂಕೆ];
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * push header access function. Only to be used from within the push callback
 */
ಅಕ್ಷರ *curl_pushheader_byname(ರಚನೆ curl_pushheaders *h, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
  ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು;
  size_t len;
  size_t i;
  /* Verify that we got a good easy handle in the push header struct,
     mostly to detect rubbish input fast(er). Also empty header name
     is rubbish too. We have to allow ":" at the beginning of
     the header, but header == ":" must be rejected. If we have ':' in
     the middle of header, it could be matched in middle of the value,
     this is because we do prefix match.*/
  ಆದರೆ(!h || !GOOD_EASY_HANDLE(h->ದತ್ತಾಂಶ) || !name || !name[0] ||
     !strcmp(name, ":") || strchr(name + 1, ':'))
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  ಹರಿವು = h->ಹರಿವು;
  ಆದರೆ(!ಹರಿವು)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name);
  ಗಾಗಿ(i = 0; i < ಹರಿವು->ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು_ಬಳಸಿದ; i++) {
    ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(name, ಹರಿವು->ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು[i], len)) {
      /* sub-match, make sure that it is followed by a colon */
      ಆದರೆ(ಹರಿವು->ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು[i][len] != ':')
        ಮುಂದುವರಿ;
      ಹಿಂತಿರುಗಿ &ಹರಿವು->ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು[i][len + 1];
    }
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ ರಚನೆ Curl_easy *h2_duphandle(ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_easy *ಎರಡನೇ = curl_easy_init();
  ಆದರೆ(ಎರಡನೇ) {
    ರಚನೆ h2_ಹರಿವು_ctx *ಎರಡನೇ_ಹರಿವು;
    http2_ದತ್ತಾಂಶ_ಸಿದ್ಧತೆ(cf, ಎರಡನೇ, &ಎರಡನೇ_ಹರಿವು);
    ಎರಡನೇ->state.ತೂಕ = ದತ್ತಾಂಶ->state.ತೂಕ;
    ಆದರೆ(ದತ್ತಾಂಶ->share)
      (ಶೂನ್ಯ)Curl_share_easy_link(ಎರಡನೇ, ದತ್ತಾಂಶ->share);
    /* copy a range of options from the parent handle */
    ಎರಡನೇ->ಗಣ.fwrite_func = ದತ್ತಾಂಶ->ಗಣ.fwrite_func;
    ಎರಡನೇ->ಗಣ.ಹೊರಹೋಗು = ದತ್ತಾಂಶ->ಗಣ.ಹೊರಹೋಗು;
    ಎರಡನೇ->ಗಣ.fwrite_ಶೀರ್ಷಿಕೆ = ದತ್ತಾಂಶ->ಗಣ.fwrite_ಶೀರ್ಷಿಕೆ;
    ಎರಡನೇ->ಗಣ.writeheader = ದತ್ತಾಂಶ->ಗಣ.writeheader;
    ಎರಡನೇ->ಗಣ.fdebug = ದತ್ತಾಂಶ->ಗಣ.fdebug;
    ಎರಡನೇ->ಗಣ.debugdata = ದತ್ತಾಂಶ->ಗಣ.debugdata;
    ಎರಡನೇ->ಗಣ.fxferinfo = ದತ್ತಾಂಶ->ಗಣ.fxferinfo;
    ಎರಡನೇ->ಗಣ.fprogress = ದತ್ತಾಂಶ->ಗಣ.fprogress;
    ಎರಡನೇ->ಗಣ.ಪ್ರಗತಿ_client = ದತ್ತಾಂಶ->ಗಣ.ಪ್ರಗತಿ_client;
    ಎರಡನೇ->ಪ್ರಗತಿ.hide = ದತ್ತಾಂಶ->ಪ್ರಗತಿ.hide;
    ಎರಡನೇ->ಪ್ರಗತಿ.callback = ದತ್ತಾಂಶ->ಪ್ರಗತಿ.callback;
  }
  ಹಿಂತಿರುಗಿ ಎರಡನೇ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಗಣ_transfer_url(ರಚನೆ Curl_easy *newhandle,
                            ರಚನೆ curl_pushheaders *hp,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *v;
  CURLUcode uc;
  ಅಕ್ಷರ *url = ಖಾಲಿ;
  ಪೂರ್ಣಾಂಕ rc = 0;
  CURLU *u = curl_url();

  ಆದರೆ(!u)
    ಹಿಂತಿರುಗಿ 5;

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_SCHEME);
  ಆದರೆ(v) {
    uc = curl_url_ಗಣ(u, CURLUPART_SCHEME, v, 0);
    ಆದರೆ(uc) {
      rc = 1;
      ಹೋಗು fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_AUTHORITY);
  ಆದರೆ(v) {
    uc = Curl_url_ಗಣ_authority(u, v);
    ಆದರೆ(uc) {
      rc = 2;
      ಹೋಗು fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_ಮಾರ್ಗ);
  ಆದರೆ(v) {
    uc = curl_url_ಗಣ(u, CURLUPART_ಮಾರ್ಗ, v, 0);
    ಆದರೆ(uc) {
      rc = 3;
      ಹೋಗು fail;
    }
  }

  /* We can only allow PUSH of resource from the same origin, e.g.
   * scheme + hostname + port */
  ಆದರೆ(!Curl_url_same_origin(ದತ್ತಾಂಶ->state.uh, u)) {
    rc = 1;
    ಹೋಗು fail;
  }

  uc = curl_url_get(u, CURLUPART_URL, &url, 0);
  ಆದರೆ(uc)
    rc = 4;
fail:
  ಆದರೆ(rc) {
    curl_url_cleanup(u);
    ಹಿಂತಿರುಗಿ rc;
  }

  DEBUGASSERT(!newhandle->state.uh);
  newhandle->state.uh = u;
  Curl_bufref_ಗಣ(&newhandle->state.url, url, 0, curl_free);
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ discard_newhandle(ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *newhandle)
{
  http2_ದತ್ತಾಂಶ_ಮುಗಿದ(cf, newhandle);
  (ಶೂನ್ಯ)Curl_close(&newhandle);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ತಳ್ಳು_promise(ರಚನೆ Curl_cfilter *cf,
                        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ಸ್ಥಿರಾಂಕ nghttp2_ತಳ್ಳು_promise *ಚೌಕಟ್ಟು)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  ಪೂರ್ಣಾಂಕ rv; /* one of the CURL_PUSH_* defines */

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] PUSH_PROMISE received",
              ಚೌಕಟ್ಟು->promised_ಹರಿವು_id);
  ಆದರೆ(ದತ್ತಾಂಶ->multi->ತಳ್ಳು_cb) {
    ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು;
    ರಚನೆ h2_ಹರಿವು_ctx *newstream;
    ರಚನೆ curl_pushheaders heads;
    CURLMcode mresult;
    CURLcode ಫಲಿತಾಂಶ;
    /* clone the parent */
    ರಚನೆ Curl_easy *newhandle = h2_duphandle(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(!newhandle) {
      infof(ದತ್ತಾಂಶ, "failed to duplicate handle");
      rv = CURL_ತಳ್ಳು_DENY; /* FAIL HARD */
      ಹೋಗು fail;
    }

    ಹರಿವು = H2_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);
    ಆದರೆ(!ಹರಿವು) {
      failf(ದತ್ತಾಂಶ, "Internal NULL stream");
      discard_newhandle(cf, newhandle);
      rv = CURL_ತಳ್ಳು_DENY;
      ಹೋಗು fail;
    }

    heads.ದತ್ತಾಂಶ = ದತ್ತಾಂಶ;
    heads.ಹರಿವು = ಹರಿವು;
    heads.ಚೌಕಟ್ಟು = ಚೌಕಟ್ಟು;

    rv = ಗಣ_transfer_url(newhandle, &heads, ದತ್ತಾಂಶ);
    ಆದರೆ(rv) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] PUSH_PROMISE, failed to set URL -> %d",
                  ಚೌಕಟ್ಟು->promised_ಹರಿವು_id, rv);
      discard_newhandle(cf, newhandle);
      rv = CURL_ತಳ್ಳು_DENY;
      ಹೋಗು fail;
    }

    {
      ರಚನೆ Curl_mapi_guard guard;
      CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, multi_ತಳ್ಳು_cb);
      rv = ದತ್ತಾಂಶ->multi->ತಳ್ಳು_cb(ದತ್ತಾಂಶ, newhandle,
                                ಹರಿವು->ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು_ಬಳಸಿದ, &heads,
                                ದತ್ತಾಂಶ->multi->ತಳ್ಳು_userp);
      CURL_CBAPI_ಕೊನೆ(&guard);
    }

    /* free the headers again */
    free_ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು(ಹರಿವು);

    ಆದರೆ(rv) {
      DEBUGASSERT((rv > CURL_ತಳ್ಳು_OK) && (rv <= CURL_ತಳ್ಳು_ERROROUT));
      /* denied, kill off the new handle again */
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] PUSH_PROMISE, denied by application -> %d",
                  ಚೌಕಟ್ಟು->promised_ಹರಿವು_id, rv);
      discard_newhandle(cf, newhandle);
      ಹೋಗು fail;
    }

    /* approved, add to the multi handle for processing. This
     * assigns newhandle->mid. For the new `mid` we assign the
     * h2_stream instance and remember the stream_id already known. */
    mresult = Curl_multi_ಕೂಡಿಸು_perform(ದತ್ತಾಂಶ->multi, newhandle, cf->conn);
    ಆದರೆ(mresult) {
      infof(ದತ್ತಾಂಶ, "failed to add handle to multi");
      discard_newhandle(cf, newhandle);
      rv = CURL_ತಳ್ಳು_DENY;
      ಹೋಗು fail;
    }

    ಫಲಿತಾಂಶ = http2_ದತ್ತಾಂಶ_ಸಿದ್ಧತೆ(cf, newhandle, &newstream);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      failf(ದತ್ತಾಂಶ, "error setting up stream: %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
      discard_newhandle(cf, newhandle);
      rv = CURL_ತಳ್ಳು_DENY;
      ಹೋಗು fail;
    }

    DEBUGASSERT(newstream);
    newstream->id = ಚೌಕಟ್ಟು->promised_ಹರಿವು_id;
    newhandle->req.maxdownload = -1;
    newhandle->req.ಗಾತ್ರ_2 = -1;

    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "promise easy handle added to multi, mid=%u",
                newhandle->mid);
    rv = Curl_ngh2_session_ಗಣ_ಹರಿವು_user_ದತ್ತಾಂಶ(
      &ctx->ngh2, newstream->id, newstream);
    ಆದರೆ(rv) {
      infof(ದತ್ತಾಂಶ, "failed to set user_data for stream %d",
            newstream->id);
      DEBUGASSERT(0);
      discard_newhandle(cf, newhandle);
      rv = CURL_ತಳ್ಳು_DENY;
      ಹೋಗು fail;
    }

    /* success, remember max stream id processed */
    ಆದರೆ(newstream->id > ctx->local_max_sid)
      ctx->local_max_sid = newstream->id;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "Got PUSH_PROMISE, ignore it");
    rv = CURL_ತಳ್ಳು_DENY;
  }
fail:
  ಹಿಂತಿರುಗಿ rv;
}

ಸ್ಥಿರ ಶೂನ್ಯ h2_xfer_write_resp_hd(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  ಆದರೆ(!ಹರಿವು->xfer_ಫಲಿತಾಂಶ) {
    ಹರಿವು->xfer_ಫಲಿತಾಂಶ = Curl_xfer_write_resp_hd(ದತ್ತಾಂಶ, buf, blen, eos);
    ಆದರೆ(!ಹರಿವು->xfer_ಫಲಿತಾಂಶ && !eos)
      ಹರಿವು->xfer_ಫಲಿತಾಂಶ = cf_h2_update_local_win(cf, ದತ್ತಾಂಶ, ಹರಿವು);
    ಆದರೆ(ಹರಿವು->xfer_ಫಲಿತಾಂಶ)
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] error %d writing %zu bytes of headers",
                  ಹರಿವು->id, (ಪೂರ್ಣಾಂಕ)ಹರಿವು->xfer_ಫಲಿತಾಂಶ, blen);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ h2_xfer_write_resp(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  ಆದರೆ(!ಹರಿವು->xfer_ಫಲಿತಾಂಶ)
    ಹರಿವು->xfer_ಫಲಿತಾಂಶ = Curl_xfer_write_resp(ದತ್ತಾಂಶ, buf, blen, eos);
  /* If the transfer write is errored, we do not want any more data */
  ಆದರೆ(ಹರಿವು->xfer_ಫಲಿತಾಂಶ) {
    ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] error %d writing %zu bytes of data, "
                "RST-ing stream",
                ಹರಿವು->id, (ಪೂರ್ಣಾಂಕ)ಹರಿವು->xfer_ಫಲಿತಾಂಶ, blen);
    Curl_ngh2_submit_rst_ಹರಿವು(
      &ctx->ngh2, 0, ಹರಿವು->id, (uint32_t)NGHTTP2_ERR_CALLBACK_FAILURE);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಹರಿವು->write_paused && Curl_xfer_write_is_paused(ದತ್ತಾಂಶ)) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] stream output paused", ಹರಿವು->id);
    ಹರಿವು->write_paused = TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಹರಿವು->write_paused && !Curl_xfer_write_is_paused(ದತ್ತಾಂಶ)) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] stream output unpaused", ಹರಿವು->id);
    ಹರಿವು->write_paused = FALSE;
  }

  ಆದರೆ(!ಹರಿವು->xfer_ಫಲಿತಾಂಶ && !eos)
    ಹರಿವು->xfer_ಫಲಿತಾಂಶ = cf_h2_update_local_win(cf, ದತ್ತಾಂಶ, ಹರಿವು);
}

ಸ್ಥಿರ CURLcode on_ಹರಿವು_ಚೌಕಟ್ಟು(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು,
                                ಸ್ಥಿರಾಂಕ nghttp2_ಚೌಕಟ್ಟು *ಚೌಕಟ್ಟು)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  int32_t ಹರಿವು_id = ಚೌಕಟ್ಟು->hd.ಹರಿವು_id;
  ಪೂರ್ಣಾಂಕ rv;

  ಆದರೆ(!ಹರಿವು) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] No stream_ctx set", ಹರಿವು_id);
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }

  ಆಯ್ಕೆ(ಚೌಕಟ್ಟು->hd.type) {
  ಸ್ಥಿತಿ NGHTTP2_ದತ್ತಾಂಶ:
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] DATA, window=%d/%d",
                ಹರಿವು_id,
                Curl_ngh2_session_get_ಹರಿವು_effective_recv_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ(
                  &ctx->ngh2, ಹರಿವು->id),
                Curl_ngh2_session_get_ಹರಿವು_effective_local_ಕಿಟಕಿ_ಗಾತ್ರ(
                  &ctx->ngh2, ಹರಿವು->id));
    /* If !body started on this stream, then receiving DATA is illegal. */
    ಆದರೆ(!ಹರಿವು->bodystarted) {
      ಆದರೆ(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_ಹರಿವು(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                       ಹರಿವು_id, NGHTTP2_PROTOCOL_ERROR))) {
        ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
      }
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ NGHTTP2_ಶೀರ್ಷಿಕೆಗಳು:
    ಆದರೆ(ಹರಿವು->bodystarted) {
      /* Only valid HEADERS after body started is trailer HEADERS. We
         buffer them in on_header callback. */
      ನಿಲ್ಲಿಸು;
    }

    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    ಆದರೆ(ಹರಿವು->ಸ್ಥಿತಿ_ಸಂಕೇತ == -1)
      ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;

    /* Only final status code signals the end of header */
    ಆದರೆ(ಹರಿವು->ಸ್ಥಿತಿ_ಸಂಕೇತ / 100 != 1) {
      ಹರಿವು->resp_hds_complete = TRUE;
      ಹರಿವು->bodystarted = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಹರಿವು->ಸ್ಥಿತಿ_ಸಂಕೇತ = -1;

    h2_xfer_write_resp_hd(cf, ದತ್ತಾಂಶ, ಹರಿವು, STRCONST("\r\n"),
                          (bool)ಹರಿವು->closed);

    Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ NGHTTP2_ತಳ್ಳು_PROMISE:
    rv = ತಳ್ಳು_promise(cf, ದತ್ತಾಂಶ, &ಚೌಕಟ್ಟು->ತಳ್ಳು_promise);
    ಆದರೆ(rv) { /* deny! */
      DEBUGASSERT((rv > CURL_ತಳ್ಳು_OK) && (rv <= CURL_ತಳ್ಳು_ERROROUT));
      ಆದರೆ(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_ಹರಿವು(&ctx->ngh2, NGHTTP2_FLAG_NONE,
             ಚೌಕಟ್ಟು->ತಳ್ಳು_promise.promised_ಹರಿವು_id, NGHTTP2_CANCEL)))
        ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rv == CURL_ತಳ್ಳು_ERROROUT) {
        /* fail the parent transfer, not the whole session */
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] fail in PUSH_PROMISE received",
                    ಹರಿವು_id);
        ಆದರೆ(!ಹರಿವು->xfer_ಫಲಿತಾಂಶ)
          ಹರಿವು->xfer_ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
        Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
      }
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ NGHTTP2_RST_ಹರಿವು:
    ಆದರೆ(ಚೌಕಟ್ಟು->rst_ಹರಿವು.error_ಸಂಕೇತ)
      ಹರಿವು->reset_by_server = TRUE;
    Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ NGHTTP2_ಕಿಟಕಿ_UPDATE:
    ಆದರೆ(CURL_REQ_WANT_SEND(ದತ್ತಾಂಶ) && Curl_bufq_is_ಖಾಲಿಗಳು(&ಹರಿವು->sendbuf)) {
      /* need more data, force processing of transfer */
      Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!Curl_bufq_is_ಖಾಲಿಗಳು(&ಹರಿವು->sendbuf)) {
      /* resume the potentially suspended stream */
      ಆದರೆ(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_session_resume_ದತ್ತಾಂಶ(&ctx->ngh2, ಹರಿವು->id)))
        ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
    }
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }

  ಆದರೆ(ಚೌಕಟ್ಟು->hd.flags & NGHTTP2_FLAG_ಕೊನೆ_ಹರಿವು) {
    ಆದರೆ(!ಹರಿವು->closed && !ಹರಿವು->body_eos &&
       ((ಹರಿವು->ಸ್ಥಿತಿ_ಸಂಕೇತ >= 400) || (ಹರಿವು->ಸ್ಥಿತಿ_ಸಂಕೇತ < 200))) {
      /* The server did not give us a positive response and we are not
       * done uploading the request body. We need to stop doing that and
       * also inform the server that we aborted our side. */
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] EOS frame with unfinished upload and "
                  "HTTP status %d, abort upload by RST",
                  ಹರಿವು_id, ಹರಿವು->ಸ್ಥಿತಿ_ಸಂಕೇತ);
      Curl_ngh2_submit_rst_ಹರಿವು(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, ಹರಿವು->id, NGHTTP2_ಹರಿವು_CLOSED);
      ಹರಿವು->closed = TRUE;
    }
    Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಪೂರ್ಣಾಂಕ Curl_nghttp2_fr_print(ಸ್ಥಿರಾಂಕ nghttp2_ಚೌಕಟ್ಟು *ಚೌಕಟ್ಟು, ಅಕ್ಷರ *ಬಫರ್,
                          size_t blen)
{
  ಆಯ್ಕೆ(ಚೌಕಟ್ಟು->hd.type) {
  ಸ್ಥಿತಿ NGHTTP2_ದತ್ತಾಂಶ: {
    ಹಿಂತಿರುಗಿ curl_msnprintf(ಬಫರ್, blen,
                          "FRAME[DATA, len=%d, eos=%d, padlen=%d]",
                          (ಪೂರ್ಣಾಂಕ)ಚೌಕಟ್ಟು->hd.ಉದ್ದಳತೆ,
                          !!(ಚೌಕಟ್ಟು->hd.flags & NGHTTP2_FLAG_ಕೊನೆ_ಹರಿವು),
                          (ಪೂರ್ಣಾಂಕ)ಚೌಕಟ್ಟು->ದತ್ತಾಂಶ.padlen);
  }
  ಸ್ಥಿತಿ NGHTTP2_ಶೀರ್ಷಿಕೆಗಳು: {
    ಹಿಂತಿರುಗಿ curl_msnprintf(ಬಫರ್, blen,
                          "FRAME[HEADERS, len=%d, hend=%d, eos=%d]",
                          (ಪೂರ್ಣಾಂಕ)ಚೌಕಟ್ಟು->hd.ಉದ್ದಳತೆ,
                          !!(ಚೌಕಟ್ಟು->hd.flags & NGHTTP2_FLAG_ಕೊನೆ_ಶೀರ್ಷಿಕೆಗಳು),
                          !!(ಚೌಕಟ್ಟು->hd.flags & NGHTTP2_FLAG_ಕೊನೆ_ಹರಿವು));
  }
  ಸ್ಥಿತಿ NGHTTP2_ಆದ್ಯತೆ: {
    ಹಿಂತಿರುಗಿ curl_msnprintf(ಬಫರ್, blen,
                          "FRAME[PRIORITY, len=%d, flags=%d]",
                          (ಪೂರ್ಣಾಂಕ)ಚೌಕಟ್ಟು->hd.ಉದ್ದಳತೆ, ಚೌಕಟ್ಟು->hd.flags);
  }
  ಸ್ಥಿತಿ NGHTTP2_RST_ಹರಿವು: {
    ಹಿಂತಿರುಗಿ curl_msnprintf(ಬಫರ್, blen,
                          "FRAME[RST_STREAM, len=%d, flags=%d, error=%u]",
                          (ಪೂರ್ಣಾಂಕ)ಚೌಕಟ್ಟು->hd.ಉದ್ದಳತೆ, ಚೌಕಟ್ಟು->hd.flags,
                          ಚೌಕಟ್ಟು->rst_ಹರಿವು.error_ಸಂಕೇತ);
  }
  ಸ್ಥಿತಿ NGHTTP2_SETTINGS: {
    ಆದರೆ(ಚೌಕಟ್ಟು->hd.flags & NGHTTP2_FLAG_ACK) {
      ಹಿಂತಿರುಗಿ curl_msnprintf(ಬಫರ್, blen, "FRAME[SETTINGS, ack=1]");
    }
    ಹಿಂತಿರುಗಿ curl_msnprintf(ಬಫರ್, blen,
                          "FRAME[SETTINGS, len=%d]", (ಪೂರ್ಣಾಂಕ)ಚೌಕಟ್ಟು->hd.ಉದ್ದಳತೆ);
  }
  ಸ್ಥಿತಿ NGHTTP2_ತಳ್ಳು_PROMISE:
    ಹಿಂತಿರುಗಿ curl_msnprintf(ಬಫರ್, blen,
                          "FRAME[PUSH_PROMISE, len=%d, hend=%d]",
                          (ಪೂರ್ಣಾಂಕ)ಚೌಕಟ್ಟು->hd.ಉದ್ದಳತೆ,
                          !!(ಚೌಕಟ್ಟು->hd.flags & NGHTTP2_FLAG_ಕೊನೆ_ಶೀರ್ಷಿಕೆಗಳು));
  ಸ್ಥಿತಿ NGHTTP2_PING:
    ಹಿಂತಿರುಗಿ curl_msnprintf(ಬಫರ್, blen,
                          "FRAME[PING, len=%d, ack=%d]",
                          (ಪೂರ್ಣಾಂಕ)ಚೌಕಟ್ಟು->hd.ಉದ್ದಳತೆ,
                          ಚೌಕಟ್ಟು->hd.flags & NGHTTP2_FLAG_ACK);
  ಸ್ಥಿತಿ NGHTTP2_GOAWAY: {
    ಅಕ್ಷರ scratch[128];
    size_t s_len = CURL_ARRAYSIZE(scratch);
    size_t len = (ಚೌಕಟ್ಟು->goaway.opaque_ದತ್ತಾಂಶ_len < s_len) ?
      ಚೌಕಟ್ಟು->goaway.opaque_ದತ್ತಾಂಶ_len : s_len - 1;
    ಆದರೆ(len)
      ಸ್ಮರಣೆ_ನಕಲು(scratch, ಚೌಕಟ್ಟು->goaway.opaque_ದತ್ತಾಂಶ, len);
    scratch[len] = '\0';
    ಹಿಂತಿರುಗಿ curl_msnprintf(ಬಫರ್, blen,
                          "FRAME[GOAWAY, error=%u, reason='%s', "
                          "last_stream=%d]", ಚೌಕಟ್ಟು->goaway.error_ಸಂಕೇತ,
                          scratch, ಚೌಕಟ್ಟು->goaway.ಕೊನೆಯ_ಹರಿವು_id);
  }
  ಸ್ಥಿತಿ NGHTTP2_ಕಿಟಕಿ_UPDATE: {
    ಹಿಂತಿರುಗಿ curl_msnprintf(ಬಫರ್, blen,
                          "FRAME[WINDOW_UPDATE, incr=%d]",
                          ಚೌಕಟ್ಟು->ಕಿಟಕಿ_update.ಕಿಟಕಿ_ಗಾತ್ರ_increment);
  }
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ curl_msnprintf(ಬಫರ್, blen, "FRAME[%d, len=%d, flags=%d]",
                          ಚೌಕಟ್ಟು->hd.type, (ಪೂರ್ಣಾಂಕ)ಚೌಕಟ್ಟು->hd.ಉದ್ದಳತೆ,
                          ಚೌಕಟ್ಟು->hd.flags);
  }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ on_ಚೌಕಟ್ಟು_send(nghttp2_session *session, ಸ್ಥಿರಾಂಕ nghttp2_ಚೌಕಟ್ಟು *ಚೌಕಟ್ಟು,
                         ಶೂನ್ಯ *userp)
{
  ರಚನೆ Curl_cfilter *cf = userp;
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);

  (ಶೂನ್ಯ)session;
  DEBUGASSERT(ದತ್ತಾಂಶ);
  ಆದರೆ(Curl_trc_cf_is_verbose(cf, ದತ್ತಾಂಶ)) {
    ಅಕ್ಷರ ಬಫರ್[256];
    ಪೂರ್ಣಾಂಕ len;
    len = Curl_nghttp2_fr_print(ಚೌಕಟ್ಟು, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್) - 1);
    ಬಫರ್[len] = 0;
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] -> %s", ಚೌಕಟ್ಟು->hd.ಹರಿವು_id, ಬಫರ್);
  }
  ಆದರೆ((ಚೌಕಟ್ಟು->hd.type == NGHTTP2_GOAWAY) && !ctx->ಕಳುಹಿಸಿದ_goaway) {
    /* A GOAWAY not initiated by us, but by nghttp2 itself on detecting
     * a protocol error on the connection */
    failf(ದತ್ತಾಂಶ, "nghttp2 shuts down connection with error %u: %s",
          ಚೌಕಟ್ಟು->goaway.error_ಸಂಕೇತ,
          nghttp2_http2_strerror(ಚೌಕಟ್ಟು->goaway.error_ಸಂಕೇತ));
  }
  ಹಿಂತಿರುಗಿ 0;
}
#ಆದರೆ_ಅಂತ್ಯ /* CURLVERBOSE */

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ on_ಚೌಕಟ್ಟು_recv(nghttp2_session *session, ಸ್ಥಿರಾಂಕ nghttp2_ಚೌಕಟ್ಟು *ಚೌಕಟ್ಟು,
                         ಶೂನ್ಯ *userp)
{
  ರಚನೆ Curl_cfilter *cf = userp;
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf), *ದತ್ತಾಂಶ_s;
  ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು;
  int32_t ಹರಿವು_id = ಚೌಕಟ್ಟು->hd.ಹರಿವು_id;

  (ಶೂನ್ಯ)session;
  DEBUGASSERT(ದತ್ತಾಂಶ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  ಆದರೆ(Curl_trc_cf_is_verbose(cf, ದತ್ತಾಂಶ)) {
    ಅಕ್ಷರ ಬಫರ್[256];
    ಪೂರ್ಣಾಂಕ len;
    len = Curl_nghttp2_fr_print(ಚೌಕಟ್ಟು, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್) - 1);
    ಬಫರ್[len] = 0;
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] <- %s", ಚೌಕಟ್ಟು->hd.ಹರಿವು_id, ಬಫರ್);
  }
#ಆದರೆ_ಅಂತ್ಯ /* CURLVERBOSE */

  ಆದರೆ(!ಹರಿವು_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(ದತ್ತಾಂಶ);
    ಆಯ್ಕೆ(ಚೌಕಟ್ಟು->hd.type) {
    ಸ್ಥಿತಿ NGHTTP2_SETTINGS: {
      ಆದರೆ(!(ಚೌಕಟ್ಟು->hd.flags & NGHTTP2_FLAG_ACK)) {
        uint32_t max_conn = ctx->max_concurrent_streams;
        ctx->max_concurrent_streams =
          Curl_ngh2_session_get_remote_settings(
            &ctx->ngh2, NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS);
        ctx->enable_ತಳ್ಳು = Curl_ngh2_session_get_remote_settings(
          &ctx->ngh2, NGHTTP2_SETTINGS_ENABLE_ತಳ್ಳು) != 0;
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[0] MAX_CONCURRENT_STREAMS: %u",
                    ctx->max_concurrent_streams);
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[0] ENABLE_PUSH: %s",
                    ctx->enable_ತಳ್ಳು ? "TRUE" : "false");
        ಆದರೆ(ದತ್ತಾಂಶ && max_conn != ctx->max_concurrent_streams) {
          /* only signal change if the value actually changed */
          CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[0] notify MAX_CONCURRENT_STREAMS: %u",
                      ctx->max_concurrent_streams);
          Curl_multi_connchanged(ದತ್ತಾಂಶ->multi);
        }
        /* Since the initial stream window is 64K, a request might be on HOLD,
         * due to exhaustion. The (initial) SETTINGS may announce a much larger
         * window and *assume* that we treat this like a WINDOW_UPDATE. Some
         * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
         * To be safe, we UNHOLD a stream in order not to stall. */
        ಆದರೆ(CURL_REQ_WANT_SEND(ದತ್ತಾಂಶ))
          Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
      }
      ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->goaway_error = ಚೌಕಟ್ಟು->goaway.error_ಸಂಕೇತ;
      ctx->remote_max_sid = ಚೌಕಟ್ಟು->goaway.ಕೊನೆಯ_ಹರಿವು_id;
      ಆದರೆ(ದತ್ತಾಂಶ) {
        infof(ದತ್ತಾಂಶ, "received GOAWAY, error=%u, last_stream=%d",
              ctx->goaway_error, ctx->remote_max_sid);
        Curl_multi_connchanged(ದತ್ತಾಂಶ->multi);
      }
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ 0;
  }

  ದತ್ತಾಂಶ_s = cf_h2_get_ಹರಿವು_xfer(cf, ಹರಿವು_id, &ಹರಿವು);
  ಆದರೆ(!ದತ್ತಾಂಶ_s) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] No Curl_easy associated", ಹರಿವು_id);
    ಹಿಂತಿರುಗಿ 0;
  }

  ಹಿಂತಿರುಗಿ on_ಹರಿವು_ಚೌಕಟ್ಟು(cf, ದತ್ತಾಂಶ_s, ಹರಿವು, ಚೌಕಟ್ಟು) ?
         NGHTTP2_ERR_CALLBACK_FAILURE : 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cf_h2_on_invalid_ಚೌಕಟ್ಟು_recv(nghttp2_session *session,
                                       ಸ್ಥಿರಾಂಕ nghttp2_ಚೌಕಟ್ಟು *ಚೌಕಟ್ಟು,
                                       ಪೂರ್ಣಾಂಕ ngerr, ಶೂನ್ಯ *userp)
{
  ರಚನೆ Curl_cfilter *cf = userp;
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ;
  ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು;
  int32_t ಹರಿವು_id = ಚೌಕಟ್ಟು->hd.ಹರಿವು_id;

  (ಶೂನ್ಯ)session;
  ದತ್ತಾಂಶ = cf_h2_get_ಹರಿವು_xfer(cf, ಹರಿವು_id, &ಹರಿವು);
  ಆದರೆ(ದತ್ತಾಂಶ) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
    ಅಕ್ಷರ ಬಫರ್[256];
    ಪೂರ್ಣಾಂಕ len;
    len = Curl_nghttp2_fr_print(ಚೌಕಟ್ಟು, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್) - 1);
    ಬಫರ್[len] = 0;
    failf(ದತ್ತಾಂಶ, "[HTTP2] [%d] received invalid frame: %s, error %d: %s",
          ಹರಿವು_id, ಬಫರ್, ngerr, nghttp2_strerror(ngerr));
#ಆದರೆ_ಅಂತ್ಯ /* CURLVERBOSE */
    ಆದರೆ(ಹರಿವು) {
      Curl_ngh2_submit_rst_ಹರಿವು(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, ಹರಿವು->id, NGHTTP2_ಹರಿವು_CLOSED);
      ಹರಿವು->error = ngerr;
      ಹರಿವು->closed = TRUE;
      ಹರಿವು->reset = TRUE;
    }
  }
  /* Return no error, nghttp2 will RST/GOAWAY by itself when needed */
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ on_ದತ್ತಾಂಶ_chunk_recv(nghttp2_session *session, uint8_t flags,
                              int32_t ಹರಿವು_id,
                              ಸ್ಥಿರಾಂಕ uint8_t *mem, size_t len, ಶೂನ್ಯ *userp)
{
  ರಚನೆ Curl_cfilter *cf = userp;
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ_s;

  (ಶೂನ್ಯ)session;
  (ಶೂನ್ಯ)flags;

  DEBUGASSERT(ಹರಿವು_id); /* should never be a zero stream ID here */
  DEBUGASSERT(CF_ದತ್ತಾಂಶ_CURRENT(cf));

  /* get the stream from the hash based on Stream ID */
  ದತ್ತಾಂಶ_s = cf_h2_get_ಹರಿವು_xfer(cf, ಹರಿವು_id, &ಹರಿವು);
  ಆದರೆ(!ದತ್ತಾಂಶ_s) {
    /* Receiving a Stream ID not in the hash should not happen - unless
       we have aborted a transfer artificially and there were more data
       in the pipeline. Silently ignore. */
    CURL_TRC_CF(CF_ದತ್ತಾಂಶ_CURRENT(cf), cf, "[%d] Data for unknown", ಹರಿವು_id);
    /* consumed explicitly as no one will read it */
    Curl_ngh2_session_consume(&ctx->ngh2, ಹರಿವು_id, len);
    ಹಿಂತಿರುಗಿ 0;
  }

  ಆದರೆ(!ಹರಿವು)
    ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;

  h2_xfer_write_resp(cf, ದತ್ತಾಂಶ_s, ಹರಿವು, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)mem, len, FALSE);

  Curl_ngh2_session_consume(&ctx->ngh2, ಹರಿವು_id, len);
  ಹರಿವು->nrcvd_ದತ್ತಾಂಶ += (curl_off_t)len;
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ on_ಹರಿವು_close(nghttp2_session *session, int32_t ಹರಿವು_id,
                           uint32_t error_ಸಂಕೇತ, ಶೂನ್ಯ *userp)
{
  ರಚನೆ Curl_cfilter *cf = userp;
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ_s, *call_ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
  ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು = ಖಾಲಿ;
  ಪೂರ್ಣಾಂಕ rv;
  (ಶೂನ್ಯ)session;

  DEBUGASSERT(call_ದತ್ತಾಂಶ);
  /* stream id 0 is the connection, do not look there for streams. */
  ದತ್ತಾಂಶ_s = ಹರಿವು_id ? cf_h2_get_ಹರಿವು_xfer(cf, ಹರಿವು_id, &ಹರಿವು) : ಖಾಲಿ;
  ಆದರೆ(!ದತ್ತಾಂಶ_s) {
    CURL_TRC_CF(call_ದತ್ತಾಂಶ, cf,
                "[%d] on_stream_close, no easy set on stream", ಹರಿವು_id);
    ಹಿಂತಿರುಗಿ 0;
  }
  ಆದರೆ(!GOOD_EASY_HANDLE(ದತ್ತಾಂಶ_s)) {
    /* nghttp2 still has an easy registered for the stream which has
     * been freed be libcurl. This points to a code path that does not
     * trigger DONE or DETACH events as it must. */
    CURL_TRC_CF(call_ದತ್ತಾಂಶ, cf,
                "[%d] on_stream_close, not a GOOD easy on stream", ಹರಿವು_id);
    (ಶೂನ್ಯ)Curl_ngh2_session_ಗಣ_ಹರಿವು_user_ದತ್ತಾಂಶ(
      &ctx->ngh2, ಹರಿವು_id, ಖಾಲಿ);
    ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  ಆದರೆ(!ಹರಿವು) {
    CURL_TRC_CF(ದತ್ತಾಂಶ_s, cf,
                "[%d] on_stream_close, GOOD easy but no stream", ಹರಿವು_id);
    ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  ಹರಿವು->closed = TRUE;
  ಹರಿವು->error = error_ಸಂಕೇತ;
  ಆದರೆ(ಹರಿವು->error) {
    ಹರಿವು->reset = TRUE;
    ಆದರೆ((ಹರಿವು->error == NGHTTP2_REFUSED_ಹರಿವು) &&
       ctx->rcvd_goaway && ctx->goaway_error)
    /* REFUSED_STREAM is used by nghttp2 on a GOAWAY from the
     * server where it indicated it will no longer process this stream.
     * If the GOAWAY carried an error code, take that as the error for
     * the stream. */
    ಹರಿವು->error = ctx->goaway_error;
  }

  ಆದರೆ(ಹರಿವು->error)
    CURL_TRC_CF(ದತ್ತಾಂಶ_s, cf, "[%d] RESET: %s (err %u)",
              ಹರಿವು_id, nghttp2_http2_strerror(error_ಸಂಕೇತ), error_ಸಂಕೇತ);
  ಇಲ್ಲದಿದ್ದರೆ
    CURL_TRC_CF(ದತ್ತಾಂಶ_s, cf, "[%d] CLOSED", ಹರಿವು_id);
  Curl_multi_mark_dirty(ದತ್ತಾಂಶ_s);

  /* remove `stream_ctx` from the nghttp2 stream */
  rv = Curl_ngh2_session_ಗಣ_ಹರಿವು_user_ದತ್ತಾಂಶ(
    &ctx->ngh2, ಹರಿವು_id, ಖಾಲಿ);
  ಆದರೆ(rv) {
    infof(ದತ್ತಾಂಶ_s, "http/2: failed to clear user_data for stream %d",
          ಹರಿವು_id);
    DEBUGASSERT(0);
  }
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_h2_ಶೀರ್ಷಿಕೆ_error(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು,
                               CURLcode ಫಲಿತಾಂಶ)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;

  failf(ದತ್ತಾಂಶ, "Error receiving HTTP2 header: %d(%s)", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ,
        curl_easy_strerror(ಫಲಿತಾಂಶ));
  ಆದರೆ(ಹರಿವು) {
    Curl_ngh2_submit_rst_ಹರಿವು(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, ಹರಿವು->id, NGHTTP2_ಹರಿವು_CLOSED);
    ಹರಿವು->closed = TRUE;
    ಹರಿವು->reset = TRUE;
  }
}

/* frame->hd.type is either NGHTTP2_HEADERS or NGHTTP2_PUSH_PROMISE */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ on_ಶೀರ್ಷಿಕೆ(nghttp2_session *session, ಸ್ಥಿರಾಂಕ nghttp2_ಚೌಕಟ್ಟು *ಚೌಕಟ್ಟು,
                     ಸ್ಥಿರಾಂಕ uint8_t *name, size_t namelen,
                     ಸ್ಥಿರಾಂಕ uint8_t *ಮೌಲ್ಯ, size_t valuelen,
                     uint8_t flags,
                     ಶೂನ್ಯ *userp)
{
  ರಚನೆ Curl_cfilter *cf = userp;
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ;
  int32_t ಹರಿವು_id = ಚೌಕಟ್ಟು->hd.ಹರಿವು_id;
  CURLcode ಫಲಿತಾಂಶ;

  (ಶೂನ್ಯ)session;
  (ಶೂನ್ಯ)flags;

  DEBUGASSERT(ಹರಿವು_id); /* should never be a zero stream ID here */

  /* get the stream from the hash based on Stream ID */
  ದತ್ತಾಂಶ = cf_h2_get_ಹರಿವು_xfer(cf, ಹರಿವು_id, &ಹರಿವು);
  ಆದರೆ(!GOOD_EASY_HANDLE(ದತ್ತಾಂಶ))
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;

  ಆದರೆ(!ಹರಿವು) {
    failf(ದತ್ತಾಂಶ, "Internal NULL stream");
    ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  /* Store received PUSH_PROMISE headers to be used when the subsequent
     PUSH_PROMISE callback comes */
  ಆದರೆ(ಚೌಕಟ್ಟು->hd.type == NGHTTP2_ತಳ್ಳು_PROMISE) {
    ಅಕ್ಷರ *h;

    ಆದರೆ((namelen == CURL_CSTRLEN(HTTP_PSEUDO_AUTHORITY)) &&
       !ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(HTTP_PSEUDO_AUTHORITY, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)name, namelen)) {
      /* pseudo headers are lower case */
      ಪೂರ್ಣಾಂಕ rc = 0;
      ಅಕ್ಷರ *check = curl_maprintf("%s:%d", ದತ್ತಾಂಶ->state.origin->hostname,
                                  ದತ್ತಾಂಶ->state.origin->ಪೋರ್ಟ್);
      ಆದರೆ(!check)
        /* no memory */
        ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;
      ಆದರೆ(!curl_strequal(check, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೌಲ್ಯ) &&
         ((ದತ್ತಾಂಶ->state.origin->ಪೋರ್ಟ್ != ದತ್ತಾಂಶ->state.origin->scheme->defport) ||
          !curl_strequal(ದತ್ತಾಂಶ->state.origin->hostname, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೌಲ್ಯ))) {
        /* This is push is not for the same authority that was asked for in
         * the URL. RFC 7540 section 8.2 says: "A client MUST treat a
         * PUSH_PROMISE for which the server is not authoritative as a stream
         * error of type PROTOCOL_ERROR."
         */
        (ಶೂನ್ಯ)Curl_ngh2_submit_rst_ಹರಿವು(
          &ctx->ngh2, NGHTTP2_FLAG_NONE, ಹರಿವು_id, NGHTTP2_PROTOCOL_ERROR);
        rc = NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      curlx_free(check);
      ಆದರೆ(rc)
        ಹಿಂತಿರುಗಿ rc;
    }

    ಆದರೆ(!ಹರಿವು->ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು) {
      ಹರಿವು->ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು_alloc = 10;
      ಹರಿವು->ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು = curlx_malloc(ಹರಿವು->ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು_alloc *
                                          ಗಾತ್ರ(ಅಕ್ಷರ *));
      ಆದರೆ(!ಹರಿವು->ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು)
        ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;
      ಹರಿವು->ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು_ಬಳಸಿದ = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಹರಿವು->ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು_ಬಳಸಿದ ==
            ಹರಿವು->ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು_alloc) {
      ಅಕ್ಷರ **headp;
      ಆದರೆ(ಹರಿವು->ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು_alloc > 1000) {
        /* this is beyond crazy many headers, bail out */
        failf(ದತ್ತಾಂಶ, "Too many PUSH_PROMISE headers");
        free_ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು(ಹರಿವು);
        ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      ಹರಿವು->ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು_alloc *= 2;
      headp = curlx_realloc(ಹರಿವು->ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು,
                            ಹರಿವು->ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು_alloc * ಗಾತ್ರ(ಅಕ್ಷರ *));
      ಆದರೆ(!headp) {
        free_ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು(ಹರಿವು);
        ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      ಹರಿವು->ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು = headp;
    }
    h = curl_maprintf("%s:%s", name, ಮೌಲ್ಯ);
    ಆದರೆ(!h) {
      free_ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು(ಹರಿವು);
      ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    ಹರಿವು->ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು[ಹರಿವು->ತಳ್ಳು_ಶೀರ್ಷಿಕೆಗಳು_ಬಳಸಿದ++] = h;
    ಹಿಂತಿರುಗಿ 0;
  }

  ಆದರೆ(ಹರಿವು->bodystarted) {
    /* This is a trailer */
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] trailer: %.*s: %.*s",
                ಹರಿವು->id, (ಪೂರ್ಣಾಂಕ)namelen, name, (ಪೂರ್ಣಾಂಕ)valuelen, ಮೌಲ್ಯ);
    ಫಲಿತಾಂಶ = Curl_dynhds_ಕೂಡಿಸು(&ಹರಿವು->resp_trailers,
                             (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)name, namelen,
                             (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೌಲ್ಯ, valuelen);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      cf_h2_ಶೀರ್ಷಿಕೆ_error(cf, ದತ್ತಾಂಶ, ಹರಿವು, ಫಲಿತಾಂಶ);
      ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;
    }

    ಹಿಂತಿರುಗಿ 0;
  }

  ಆದರೆ(namelen == CURL_CSTRLEN(HTTP_PSEUDO_ಸ್ಥಿತಿ) &&
     !memcmp(HTTP_PSEUDO_ಸ್ಥಿತಿ, name, namelen)) {
    /* nghttp2 guarantees :status is received first and only once. */
    ಅಕ್ಷರ ಬಫರ್[32];
    size_t hlen;
    ಫಲಿತಾಂಶ = Curl_http_decode_ಸ್ಥಿತಿ(&ಹರಿವು->ಸ್ಥಿತಿ_ಸಂಕೇತ,
                                     (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೌಲ್ಯ, valuelen);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      cf_h2_ಶೀರ್ಷಿಕೆ_error(cf, ದತ್ತಾಂಶ, ಹರಿವು, ಫಲಿತಾಂಶ);
      ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    hlen = curl_msnprintf(ಬಫರ್, ಗಾತ್ರ(ಬಫರ್), HTTP_PSEUDO_ಸ್ಥಿತಿ ":%d\r",
                          ಹರಿವು->ಸ್ಥಿತಿ_ಸಂಕೇತ);
    ಫಲಿತಾಂಶ = Curl_ಶೀರ್ಷಿಕೆಗಳು_ತಳ್ಳು(ದತ್ತಾಂಶ, ಬಫರ್, hlen, CURLH_PSEUDO);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      cf_h2_ಶೀರ್ಷಿಕೆ_error(cf, ದತ್ತಾಂಶ, ಹರಿವು, ಫಲಿತಾಂಶ);
      ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    curlx_dyn_reset(&ctx->scratch);
    ಫಲಿತಾಂಶ = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/2 "));
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&ctx->scratch, ಮೌಲ್ಯ, valuelen);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      h2_xfer_write_resp_hd(cf, ದತ್ತಾಂಶ, ಹರಿವು, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      cf_h2_ಶೀರ್ಷಿಕೆ_error(cf, ದತ್ತಾಂಶ, ಹರಿವು, ಫಲಿತಾಂಶ);
      ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    /* if we receive data for another handle, wake that up */
    ಆದರೆ(CF_ದತ್ತಾಂಶ_CURRENT(cf) != ದತ್ತಾಂಶ)
      Curl_multi_mark_dirty(ದತ್ತಾಂಶ);

    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] status: HTTP/2 %03d",
                ಹರಿವು->id, ಹರಿವು->ಸ್ಥಿತಿ_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ 0;
  }

  /* nghttp2 guarantees that namelen > 0, and :status was already
     received, and this is not pseudo-header field . */
  /* convert to an HTTP1-style header */
  curlx_dyn_reset(&ctx->scratch);
  ಫಲಿತಾಂಶ = curlx_dyn_addn(&ctx->scratch, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)name, namelen);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = curlx_dyn_addn(&ctx->scratch, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೌಲ್ಯ, valuelen);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    h2_xfer_write_resp_hd(cf, ದತ್ತಾಂಶ, ಹರಿವು, curlx_dyn_ptr(&ctx->scratch),
                          curlx_dyn_len(&ctx->scratch), FALSE);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    cf_h2_ಶೀರ್ಷಿಕೆ_error(cf, ದತ್ತಾಂಶ, ಹರಿವು, ಫಲಿತಾಂಶ);
    ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  /* if we receive data for another handle, wake that up */
  ಆದರೆ(CF_ದತ್ತಾಂಶ_CURRENT(cf) != ದತ್ತಾಂಶ)
    Curl_multi_mark_dirty(ದತ್ತಾಂಶ);

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] header: %.*s: %.*s",
              ಹರಿವು->id, (ಪೂರ್ಣಾಂಕ)namelen, name, (ಪೂರ್ಣಾಂಕ)valuelen, ಮೌಲ್ಯ);

  ಹಿಂತಿರುಗಿ 0; /* 0 is successful */
}

ಸ್ಥಿರ ssize_t req_body_read_callback(nghttp2_session *session,
                                      int32_t ಹರಿವು_id,
                                      uint8_t *buf, size_t ಉದ್ದಳತೆ,
                                      uint32_t *ದತ್ತಾಂಶ_flags,
                                      nghttp2_ದತ್ತಾಂಶ_ಮೂಲ *ಮೂಲ,
                                      ಶೂನ್ಯ *userp)
{
  ರಚನೆ Curl_cfilter *cf = userp;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ_s;
  ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;
  ssize_t nread;
  size_t n;
  (ಶೂನ್ಯ)ಮೂಲ;

  (ಶೂನ್ಯ)session;
  ಆದರೆ(!ಹರಿವು_id)
    ಹಿಂತಿರುಗಿ NGHTTP2_ERR_INVALID_ವಾದ;

  /* get the stream from the hash based on Stream ID, stream ID zero is for
     connection-oriented stuff */
  ದತ್ತಾಂಶ_s = cf_h2_get_ಹರಿವು_xfer(cf, ಹರಿವು_id, &ಹರಿವು);
  ಆದರೆ(!ದತ್ತಾಂಶ_s || !ಹರಿವು)
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;

  ಫಲಿತಾಂಶ = Curl_bufq_read(&ಹರಿವು->sendbuf, buf, ಉದ್ದಳತೆ, &n);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_AGAIN)
      ಹಿಂತಿರುಗಿ NGHTTP2_ERR_CALLBACK_FAILURE;
    nread = 0;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    nread = (ssize_t)n;

  CURL_TRC_CF(ದತ್ತಾಂಶ_s, cf, "[%d] req_body_read(len=%zu) eos=%d -> %zd, %d",
              ಹರಿವು_id, ಉದ್ದಳತೆ, ಹರಿವು->body_eos, nread, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);

  ಆದರೆ(ಹರಿವು->body_eos && Curl_bufq_is_ಖಾಲಿಗಳು(&ಹರಿವು->sendbuf)) {
    *ದತ್ತಾಂಶ_flags = NGHTTP2_ದತ್ತಾಂಶ_FLAG_EOF;
    ಹಿಂತಿರುಗಿ nread;
  }
  ಹಿಂತಿರುಗಿ (nread == 0) ? NGHTTP2_ERR_DEFERRED : nread;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ error_callback(nghttp2_session *session,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ,
                          size_t len,
                          ಶೂನ್ಯ *userp)
{
  ರಚನೆ Curl_cfilter *cf = userp;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
  (ಶೂನ್ಯ)session;
  failf(ದತ್ತಾಂಶ, "%.*s", (ಪೂರ್ಣಾಂಕ)len, ಸಂದೇಶ);
  ಹಿಂತಿರುಗಿ 0;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Append headers to ask for an HTTP1.1 to HTTP2 upgrade.
 */
CURLcode Curl_http2_request_upgrade(ರಚನೆ dynbuf *req,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURLcode ಫಲಿತಾಂಶ;
  ಅಕ್ಷರ *base64;
  size_t blen;
  ರಚನೆ SingleRequest *k = &ದತ್ತಾಂಶ->req;
  uint8_t binsettings[H2_BINSETTINGS_LEN];
  ssize_t rc;
  size_t binlen; /* length of the binsettings data */

  rc = populate_binsettings(binsettings, ದತ್ತಾಂಶ);
  ಆದರೆ(!curlx_sztouz(rc, &binlen) || !binlen) {
    failf(ದತ್ತಾಂಶ, "nghttp2 unexpectedly failed on pack_settings_payload");
    curlx_dyn_free(req);
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }

  ಫಲಿತಾಂಶ = curlx_base64url_encode(binsettings, binlen, &base64, &blen);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    curlx_dyn_free(req);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ದತ್ತಾಂಶ->state.http_hd_upgrade = TRUE;
  ದತ್ತಾಂಶ->state.http_hd_h2_settings = TRUE;
  ಫಲಿತಾಂಶ = curlx_dyn_addf(req,
                          "Upgrade: %s\r\n"
                          "HTTP2-Settings: %s\r\n",
                          NGHTTP2_CLEARTEXT_PROTO_ಆವೃತ್ತಿ_ID, base64);
  curlx_free(base64);

  k->upgr101 = UPGR101_H2;
  ದತ್ತಾಂಶ->conn->ಬಿಟ್ಟುಗಳು.upgrade_ಒಳ_ಪ್ರಗತಿ = TRUE;

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode http2_handle_ಹರಿವು_close(ರಚನೆ Curl_cfilter *cf,
                                          ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು,
                                          size_t *pnlen)
{
  CURLcode ಫಲಿತಾಂಶ;

  *pnlen = 0;
  ಆದರೆ(ಹರಿವು->reset) {
    ಆದರೆ(ಹರಿವು->error == NGHTTP2_REFUSED_ಹರಿವು) {
      infof(ದತ್ತಾಂಶ, "HTTP/2 stream %d refused by server, try again on a new "
                  "connection", ಹರಿವು->id);
      connclose(cf->conn); /* do not use this anymore */
      ದತ್ತಾಂಶ->state.refused_ಹರಿವು = TRUE;
      ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಹರಿವು->resp_hds_complete && ದತ್ತಾಂಶ->req.no_body) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] error after response headers, but we did "
                  "not want a body anyway, ignore: %s (err %u)",
                  ಹರಿವು->id, nghttp2_http2_strerror(ಹರಿವು->error),
                  ಹರಿವು->error);
      ಹರಿವು->close_handled = TRUE;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    failf(ದತ್ತಾಂಶ, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          ಹರಿವು->id, ಹರಿವು->reset_by_server ? "server" : "curl",
          ಹರಿವು->error, nghttp2_http2_strerror(ಹರಿವು->error));
    ಹಿಂತಿರುಗಿ ಹರಿವು->error ? CURLE_HTTP2_ಹರಿವು :
           (ದತ್ತಾಂಶ->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಹರಿವು->bodystarted) {
    failf(ದತ್ತಾಂಶ, "HTTP/2 stream %d was closed cleanly, but before getting "
          "all response header fields, treated as error", ಹರಿವು->id);
    ಹಿಂತಿರುಗಿ CURLE_HTTP2_ಹರಿವು;
  }

  ಆದರೆ(Curl_dynhds_ಎಣಿಕೆ(&ಹರಿವು->resp_trailers)) {
    ರಚನೆ dynhds_ನಮೂದು *e;
    ರಚನೆ dynbuf dbuf;
    size_t i;

    ಫಲಿತಾಂಶ = CURLE_OK;
    curlx_dyn_init(&dbuf, DYN_TRAILERS);
    ಗಾಗಿ(i = 0; i < Curl_dynhds_ಎಣಿಕೆ(&ಹರಿವು->resp_trailers); ++i) {
      e = Curl_dynhds_getn(&ಹರಿವು->resp_trailers, i);
      ಆದರೆ(!e)
        ನಿಲ್ಲಿಸು;
      curlx_dyn_reset(&dbuf);
      ಫಲಿತಾಂಶ = curlx_dyn_addf(&dbuf, "%.*s: %.*s\x0d\x0a",
                              (ಪೂರ್ಣಾಂಕ)e->namelen, e->name,
                              (ಪೂರ್ಣಾಂಕ)e->valuelen, e->ಮೌಲ್ಯ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ನಿಲ್ಲಿಸು;
      Curl_debug(ದತ್ತಾಂಶ, CURLINFO_ಶೀರ್ಷಿಕೆ_ಒಳ, curlx_dyn_ptr(&dbuf),
                 curlx_dyn_len(&dbuf));
      ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ,
                                 CLIENTWRITE_ಶೀರ್ಷಿಕೆ | CLIENTWRITE_TRAILER,
                                 curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf));
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ನಿಲ್ಲಿಸು;
      ಫಲಿತಾಂಶ = Curl_bump_headersize(ದತ್ತಾಂಶ, curlx_dyn_len(&dbuf), FALSE);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    curlx_dyn_free(&dbuf);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  ಹರಿವು->close_handled = TRUE;
  ಫಲಿತಾಂಶ = CURLE_OK;

ಹೊರಹೋಗು:
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "handle_stream_close -> %d, %zu", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnlen);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sweight_wanted(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ->ಗಣ.ತೂಕ ?
    ದತ್ತಾಂಶ->ಗಣ.ತೂಕ : NGHTTP2_DEFAULT_ತೂಕ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sweight_ಒಳ_effect(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ->state.ತೂಕ ?
    ದತ್ತಾಂಶ->state.ತೂಕ : NGHTTP2_DEFAULT_ತೂಕ;
}

/*
 * h2_pri_spec() fills in the pri_spec struct, used by nghttp2 to send weight
 * and dependency to the peer. It also stores the updated values in the state
 * struct.
 */

ಸ್ಥಿರ ಶೂನ್ಯ h2_pri_spec(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        nghttp2_ಆದ್ಯತೆ_spec *pri_spec)
{
  ಪೂರ್ಣಾಂಕ prio = ದತ್ತಾಂಶ->ಗಣ.ತೂಕ;
  nghttp2_ಆದ್ಯತೆ_spec_init(pri_spec, 0, sweight_wanted(ದತ್ತಾಂಶ), FALSE);
  ದತ್ತಾಂಶ->state.ತೂಕ = prio;
}

/*
 * Check if there is been an update in the priority /
 * dependency settings and if so it submits a PRIORITY frame with the updated
 * info.
 * Flush any out data pending in the network buffer.
 */
ಸ್ಥಿರ CURLcode h2_ಪ್ರಗತಿ_egress(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  ಪೂರ್ಣಾಂಕ rv = 0;

  ಆದರೆ(sweight_wanted(ದತ್ತಾಂಶ) != sweight_ಒಳ_effect(ದತ್ತಾಂಶ)) {
    ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು = H2_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);
    ಆದರೆ(ಹರಿವು && ಹರಿವು->id > 0 &&
       (sweight_wanted(ದತ್ತಾಂಶ) != sweight_ಒಳ_effect(ದತ್ತಾಂಶ))) {
      /* send new weight and/or dependency */
      nghttp2_ಆದ್ಯತೆ_spec pri_spec;

      h2_pri_spec(ದತ್ತಾಂಶ, &pri_spec);
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] Queuing PRIORITY", ಹರಿವು->id);
      DEBUGASSERT(ಹರಿವು->id != -1);
      rv = Curl_ngh2_submit_ಆದ್ಯತೆ(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, ಹರಿವು->id, &pri_spec);
      ಆದರೆ(rv)
        ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  ctx->nw_ಹೊರಹೋಗು_blocked = 0;
  ಆಗ(!rv && !ctx->nw_ಹೊರಹೋಗು_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

ಹೊರಹೋಗು:
  ಆದರೆ(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
  }
  /* Defer flushing during the connect phase so that the SETTINGS and
   * other initial frames are sent together with the first request.
   * Unless we are 'connect_only' where the request will never come. */
  ಆದರೆ(!cf->connected && !cf->conn->ಬಿಟ್ಟುಗಳು.connect_only)
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಹಿಂತಿರುಗಿ nw_ಹೊರಹೋಗು_flush(cf, ದತ್ತಾಂಶ);
}

ಸ್ಥಿರ CURLcode ಹರಿವು_recv(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು,
                            ಅಕ್ಷರ *buf, size_t len, size_t *pnread)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_AGAIN;

  (ಶೂನ್ಯ)buf;
  (ಶೂನ್ಯ)len;
  *pnread = 0;

  ಆದರೆ(!ಹರಿವು->xfer_ಫಲಿತಾಂಶ)
    ಹರಿವು->xfer_ಫಲಿತಾಂಶ = cf_h2_update_local_win(cf, ದತ್ತಾಂಶ, ಹರಿವು);

  ಆದರೆ(ಹರಿವು->xfer_ಫಲಿತಾಂಶ) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] xfer write failed", ಹರಿವು->id);
    ಫಲಿತಾಂಶ = ಹರಿವು->xfer_ಫಲಿತಾಂಶ;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಹರಿವು->closed) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] returning CLOSE", ಹರಿವು->id);
    ಫಲಿತಾಂಶ = http2_handle_ಹರಿವು_close(cf, ದತ್ತಾಂಶ, ಹರಿವು, pnread);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಹರಿವು->reset ||
          (ctx->conn_closed && Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->inbufq)) ||
          (ctx->rcvd_goaway && ctx->remote_max_sid < ಹರಿವು->id)) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] returning ERR", ಹರಿವು->id);
    ಫಲಿತಾಂಶ = ದತ್ತಾಂಶ->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2;
  }

  ಆದರೆ(ಫಲಿತಾಂಶ && (ಫಲಿತಾಂಶ != CURLE_AGAIN))
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] stream_recv(len=%zu) -> %d, %zu",
                ಹರಿವು->id, len, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnread);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode h2_ಪ್ರಗತಿ_ingress(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    size_t ದತ್ತಾಂಶ_max_ಬೈಟುಗಳು)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t nread;

  ಆದರೆ(should_close_session(ctx)) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[0] ingress: session is closed");
    ಹಿಂತಿರುಗಿ CURLE_HTTP2;
  }

  /* Process network input buffer first */
  ಆದರೆ(!Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->inbufq)) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "Process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    ಫಲಿತಾಂಶ = h2_process_pending_ಒಳಹರಿವು(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಆದರೆ(!ದತ್ತಾಂಶ_max_ಬೈಟುಗಳು)
    ದತ್ತಾಂಶ_max_ಬೈಟುಗಳು = H2_CHUNK_ಗಾತ್ರ;

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop due to connection close or us not processing
   * all network input */
  ಆಗ(!ctx->conn_closed && Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->inbufq)) {
    ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು = H2_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);
    ಆದರೆ(ಹರಿವು && (ಹರಿವು->closed || !ದತ್ತಾಂಶ_max_ಬೈಟುಗಳು)) {
      /* We would like to abort here and stop processing, so that the transfer
       * loop can handle the data/close here. This may leave data in
       * underlying buffers that will not be consumed. */
      ಆದರೆ(!cf->ಮುಂದಿನ || !cf->ಮುಂದಿನ->cft->has_ದತ್ತಾಂಶ_pending(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ))
        Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
      ನಿಲ್ಲಿಸು;
    }

    ಫಲಿತಾಂಶ = Curl_cf_recv_bufq(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, &ctx->inbufq, 0, &nread);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_AGAIN) {
        failf(ದತ್ತಾಂಶ, "Failed receiving HTTP2 data: %d(%s)", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ,
              curl_easy_strerror(ಫಲಿತಾಂಶ));
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
      ನಿಲ್ಲಿಸು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(nread == 0) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[0] ingress: connection closed");
      ctx->conn_closed = TRUE;
      ನಿಲ್ಲಿಸು;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[0] ingress: read %zu bytes", nread);
      ದತ್ತಾಂಶ_max_ಬೈಟುಗಳು = (ದತ್ತಾಂಶ_max_ಬೈಟುಗಳು > nread) ? (ದತ್ತಾಂಶ_max_ಬೈಟುಗಳು - nread) : 0;
    }

    ಫಲಿತಾಂಶ = h2_process_pending_ಒಳಹರಿವು(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[0] ingress: nw-in buffered %zu",
                Curl_bufq_len(&ctx->inbufq));
  }

  ಆದರೆ(ctx->conn_closed && Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->inbufq)) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "server closed %s",
                ctx->rcvd_goaway ? "with GOAWAY" : "abruptly");
    connclose(cf->conn);
  }

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[0] ingress: done");
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_h2_recv(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ಅಕ್ಷರ *buf, size_t len, size_t *pnread)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು;
  CURLcode ಫಲಿತಾಂಶ, r2;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;

  ಆದರೆ(!ದತ್ತಾಂಶ)
    ಹಿಂತಿರುಗಿ CURLE_HTTP2;

  ಹರಿವು = H2_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);

  *pnread = 0;
  ಆದರೆ(!ಹರಿವು) {
    /* Abnormal call sequence: either this transfer has never opened a stream
     * (unlikely) or the transfer has been done, cleaned up its resources, but
     * a read() is called anyway. It is not clear what the calling sequence
     * is for such a case. */
    failf(ದತ್ತಾಂಶ, "http/2 recv on a transfer never opened "
          "or already cleared, mid=%u", ದತ್ತಾಂಶ->mid);
    ಹಿಂತಿರುಗಿ CURLE_HTTP2;
  }

  CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);

  ಫಲಿತಾಂಶ = ಹರಿವು_recv(cf, ದತ್ತಾಂಶ, ಹರಿವು, buf, len, pnread);
  ಆದರೆ(ಫಲಿತಾಂಶ && (ಫಲಿತಾಂಶ != CURLE_AGAIN))
    ಹೋಗು ಹೊರಹೋಗು;

  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಫಲಿತಾಂಶ = h2_ಪ್ರಗತಿ_ingress(cf, ದತ್ತಾಂಶ, len);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;

    ಫಲಿತಾಂಶ = ಹರಿವು_recv(cf, ದತ್ತಾಂಶ, ಹರಿವು, buf, len, pnread);
  }

  ಆದರೆ(*pnread > 0) {
    /* Now that we transferred this to the upper layer, we report
     * the actual amount of DATA consumed to the H2 session, so
     * that it adjusts stream flow control */
    Curl_ngh2_session_consume(&ctx->ngh2, ಹರಿವು->id, *pnread);
    ಆದರೆ(ಹರಿವು->closed) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] DRAIN closed stream", ಹರಿವು->id);
      Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
    }
  }

ಹೊರಹೋಗು:
  r2 = h2_ಪ್ರಗತಿ_egress(cf, ದತ್ತಾಂಶ);
  ಆದರೆ(r2 == CURLE_AGAIN) {
    /* pending data to send, need to be called again. Ideally, we
     * monitor the socket for POLLOUT, but when not SENDING
     * any more, we force processing of the transfer. */
    ಆದರೆ(!CURL_REQ_WANT_SEND(ದತ್ತಾಂಶ))
      Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(r2 && (ಫಲಿತಾಂಶ == CURLE_AGAIN)) {
    /* Do not replace a previous error result NOR a successful result
     * with an egress error. Example: the server might close the
     * connection after sending the complete response with EOS
     * and everything. Further sending may fail, but that MUST
     * not change that we had a successful response. */
    ಫಲಿತಾಂಶ = r2;
  }

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] cf_recv(len=%zu) -> %d, %zu, "
              "window=%d/%d, connection %d/%d",
              ಹರಿವು->id, len, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnread,
              Curl_ngh2_session_get_ಹರಿವು_effective_recv_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ(
                &ctx->ngh2, ಹರಿವು->id),
              Curl_ngh2_session_get_ಹರಿವು_effective_local_ಕಿಟಕಿ_ಗಾತ್ರ(
                &ctx->ngh2, ಹರಿವು->id),
              Curl_ngh2_session_get_local_ಕಿಟಕಿ_ಗಾತ್ರ(&ctx->ngh2),
              HTTP2_HUGE_ಕಿಟಕಿ_ಗಾತ್ರ);

  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_h2_body_send(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು,
                                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *buf, size_t blen, bool eos,
                                size_t *pnwritten)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ;

  *pnwritten = 0;
  ಆದರೆ(ಹರಿವು->closed) {
    ಆದರೆ(ಹರಿವು->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] discarding data"
                  "on closed stream with response", ಹರಿವು->id);
      ಆದರೆ(eos)
        ಹರಿವು->body_eos = TRUE;
      *pnwritten = blen;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    /* Server closed before we got a response, this is an error */
    infof(ದತ್ತಾಂಶ, "stream %d closed", ಹರಿವು->id);
    ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
  }

  ಫಲಿತಾಂಶ = Curl_bufq_write(&ಹರಿವು->sendbuf, buf, blen, pnwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಆದರೆ(eos && (blen == *pnwritten))
    ಹರಿವು->body_eos = TRUE;

  ಆದರೆ(eos || !Curl_bufq_is_ಖಾಲಿಗಳು(&ಹರಿವು->sendbuf)) {
    /* resume the potentially suspended stream */
    ಆದರೆ(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_ದತ್ತಾಂಶ(&ctx->ngh2, ಹರಿವು->id)))
      ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode h2_submit(ರಚನೆ h2_ಹರಿವು_ctx **pstream,
                          ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t len,
                          bool eos, size_t *pnwritten)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು = ಖಾಲಿ;
  ರಚನೆ dynhds h2_ಶೀರ್ಷಿಕೆಗಳು;
  nghttp2_nv *nva = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *body = ಖಾಲಿ;
  size_t nheader, bodylen;
  nghttp2_ದತ್ತಾಂಶ_provider ದತ್ತಾಂಶ_prd;
  int32_t ಹರಿವು_id;
  nghttp2_ಆದ್ಯತೆ_spec pri_spec;
  size_t nwritten;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  uint32_t initial_win_ಗಾತ್ರ;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_ಶೀರ್ಷಿಕೆಗಳು, MAX_HTTP_RESP_ಶೀರ್ಷಿಕೆ_ಎಣಿಕೆ, DYN_HTTP_REQUEST);

  ಫಲಿತಾಂಶ = http2_ದತ್ತಾಂಶ_ಸಿದ್ಧತೆ(cf, ದತ್ತಾಂಶ, &ಹರಿವು);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಫಲಿತಾಂಶ = Curl_h1_req_parse_read(
    &ಹರಿವು->h1, buf, len, ಖಾಲಿ,
    !ದತ್ತಾಂಶ->state.http_ignorecustom ?
    CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_CUSTOMREQUEST) : ಖಾಲಿ,
    0, &nwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  *pnwritten = nwritten;
  ಆದರೆ(!ಹರಿವು->h1.ಮುಗಿದ) {
    /* need more data */
    ಹೋಗು ಹೊರಹೋಗು;
  }
  DEBUGASSERT(ಹರಿವು->h1.req);

  ಫಲಿತಾಂಶ = Curl_http_req_ಗಮ್ಯ_h2(&h2_ಶೀರ್ಷಿಕೆಗಳು, ಹರಿವು->h1.req, ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  /* no longer needed */
  Curl_h1_req_parse_free(&ಹರಿವು->h1);

  nva = Curl_dynhds_ಗಮ್ಯ_nva(&h2_ಶೀರ್ಷಿಕೆಗಳು, &nheader);
  ಆದರೆ(!nva) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  h2_pri_spec(ದತ್ತಾಂಶ, &pri_spec);
  ಆದರೆ(!Curl_ngh2_session_check_request_allowed(&ctx->ngh2))
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "send request NOT allowed (via nghttp2)");

  /* Check the initial windows size of the transfer (rate-limits?) and
   * send an updated settings on changes from previous value. */
  initial_win_ಗಾತ್ರ = cf_h2_initial_win_ಗಾತ್ರ(ದತ್ತಾಂಶ);
  ಆದರೆ(initial_win_ಗಾತ್ರ != ctx->initial_win_ಗಾತ್ರ) {
    ಫಲಿತಾಂಶ = cf_h2_update_settings(ctx, initial_win_ಗಾತ್ರ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆಯ್ಕೆ(ದತ್ತಾಂಶ->state.httpreq) {
  ಸ್ಥಿತಿ HTTPREQ_POST:
  ಸ್ಥಿತಿ HTTPREQ_POST_FORM:
  ಸ್ಥಿತಿ HTTPREQ_POST_MIME:
  ಸ್ಥಿತಿ HTTPREQ_PUT:
    ದತ್ತಾಂಶ_prd.read_callback = req_body_read_callback;
    ದತ್ತಾಂಶ_prd.ಮೂಲ.ptr = ಖಾಲಿ;
    ಹರಿವು_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, &ದತ್ತಾಂಶ_prd, ಹರಿವು);
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹರಿವು_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, ಖಾಲಿ, ಹರಿವು);
  }

  ಆದರೆ(ಹರಿವು_id < 0) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "send: nghttp2_submit_request error (%s)%d",
                nghttp2_strerror(ಹರಿವು_id), ಹರಿವು_id);
    ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
    ಹೋಗು ಹೊರಹೋಗು;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ACC 60000  /* <64KB to account for some overhead */
  ಆದರೆ(Curl_trc_is_verbose(ದತ್ತಾಂಶ)) {
    size_t acc = 0, i;

    infof(ದತ್ತಾಂಶ, "[HTTP/2] [%d] OPENED stream for %s",
          ಹರಿವು_id, Curl_bufref_ptr(&ದತ್ತಾಂಶ->state.url));
    ಗಾಗಿ(i = 0; i < nheader; ++i) {
      acc += nva[i].namelen + nva[i].valuelen;

      infof(ದತ್ತಾಂಶ, "[HTTP/2] [%d] [%.*s: %.*s]", ಹರಿವು_id,
            (ಪೂರ್ಣಾಂಕ)nva[i].namelen, nva[i].name,
            (ಪೂರ್ಣಾಂಕ)nva[i].valuelen, nva[i].ಮೌಲ್ಯ);
    }

    ಆದರೆ(acc > MAX_ACC) {
      infof(ದತ್ತಾಂಶ, "[HTTP/2] Warning: The cumulative length of all "
            "headers exceeds %d bytes and that could cause the "
            "stream to be rejected.", MAX_ACC);
    }
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಹರಿವು->id = ಹರಿವು_id;

  body = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)buf + *pnwritten;
  bodylen = len - *pnwritten;

  ಆದರೆ(bodylen || eos) {
    size_t n;
    ಫಲಿತಾಂಶ = cf_h2_body_send(cf, ದತ್ತಾಂಶ, ಹರಿವು, body, bodylen, eos, &n);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      *pnwritten += n;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
      ಫಲಿತಾಂಶ = CURLE_OK;
    ಇಲ್ಲದಿದ್ದರೆ {
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
    }
  }

ಹೊರಹೋಗು:
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] submit -> %d, %zu",
              ಹರಿವು ? ಹರಿವು->id : -1, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnwritten);
  curlx_safefree(nva);
  *pstream = ಹರಿವು;
  Curl_dynhds_free(&h2_ಶೀರ್ಷಿಕೆಗಳು);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_h2_send(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು = H2_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK, r2;

  CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
  *pnwritten = 0;

  ಆದರೆ(!ಹರಿವು || ಹರಿವು->id == -1) {
    ಫಲಿತಾಂಶ = h2_submit(&ಹರಿವು, cf, ದತ್ತಾಂಶ, buf, len, eos, pnwritten);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    DEBUGASSERT(ಹರಿವು);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಹರಿವು->body_eos) {
    /* We already wrote this, but CURLE_AGAIN-ed the call due to not
     * being able to flush stream->sendbuf. Make a 0-length write
     * to trigger flushing again.
     * If this works, we report to have written `len` bytes. */
    size_t n;
    DEBUGASSERT(eos);
    ಫಲಿತಾಂಶ = cf_h2_body_send(cf, ದತ್ತಾಂಶ, ಹರಿವು, buf, 0, eos, &n);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] cf_body_send last CHUNK -> %d, %zu, eos=%d",
                ಹರಿವು->id, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, n, eos);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    *pnwritten = len;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಫಲಿತಾಂಶ = cf_h2_body_send(cf, ದತ್ತಾಂಶ, ಹರಿವು, buf, len, eos, pnwritten);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] cf_body_send(len=%zu) -> %d, %zu, eos=%d",
                ಹರಿವು->id, len, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnwritten, eos);
  }

  /* Call the nghttp2 send loop and flush to write ALL buffered data,
   * headers and/or request body completely out to the network */
  r2 = h2_ಪ್ರಗತಿ_egress(cf, ದತ್ತಾಂಶ);

  /* if the stream has been closed in egress handling (nghttp2 does that
   * when it does not like the headers, for example */
  ಆದರೆ(ಹರಿವು && ಹರಿವು->closed && !ಹರಿವು->resp_hds_complete) {
    infof(ದತ್ತಾಂಶ, "stream %d closed", ಹರಿವು->id);
    ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(r2 && (r2 != CURLE_AGAIN)) {
    ಫಲಿತಾಂಶ = r2;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    ಆದರೆ(ಹರಿವು && ಹರಿವು->closed) {
      ಫಲಿತಾಂಶ = http2_handle_ಹರಿವು_close(cf, ದತ್ತಾಂಶ, ಹರಿವು, pnwritten);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "send: nothing to do in this session");
      ಫಲಿತಾಂಶ = CURLE_HTTP2;
    }
  }

ಹೊರಹೋಗು:
  ಆದರೆ(ಹರಿವು) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
                "eos=%d, h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                ಹರಿವು->id, len, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnwritten,
                ಹರಿವು->body_eos,
                Curl_ngh2_session_get_ಹರಿವು_remote_ಕಿಟಕಿ_ಗಾತ್ರ(
                  &ctx->ngh2, ಹರಿವು->id),
                Curl_ngh2_session_get_remote_ಕಿಟಕಿ_ಗಾತ್ರ(&ctx->ngh2),
                Curl_bufq_len(&ಹರಿವು->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "cf_send(len=%zu) -> %d, %zu, "
                "connection-window=%d, nw_send_buffer(%zu)",
                len, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnwritten,
                Curl_ngh2_session_get_remote_ಕಿಟಕಿ_ಗಾತ್ರ(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_h2_flush(ರಚನೆ Curl_cfilter *cf,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು = H2_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
  ಆದರೆ(ಹರಿವು && !Curl_bufq_is_ಖಾಲಿಗಳು(&ಹರಿವು->sendbuf)) {
    /* resume the potentially suspended stream */
    ಆದರೆ(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_ದತ್ತಾಂಶ(&ctx->ngh2, ಹರಿವು->id))) {
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  ಫಲಿತಾಂಶ = h2_ಪ್ರಗತಿ_egress(cf, ದತ್ತಾಂಶ);

ಹೊರಹೋಗು:
  ಆದರೆ(ಹರಿವು) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] flush -> %d, "
                "h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                ಹರಿವು->id, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ,
                Curl_ngh2_session_get_ಹರಿವು_remote_ಕಿಟಕಿ_ಗಾತ್ರ(
                  &ctx->ngh2, ಹರಿವು->id),
                Curl_ngh2_session_get_remote_ಕಿಟಕಿ_ಗಾತ್ರ(&ctx->ngh2),
                Curl_bufq_len(&ಹರಿವು->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "flush -> %d, "
                "connection-window=%d, nw_send_buffer(%zu)", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ,
                Curl_ngh2_session_get_remote_ಕಿಟಕಿ_ಗಾತ್ರ(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_h2_adjust_pollset(ರಚನೆ Curl_cfilter *cf,
                                     ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;
  curl_socket_t sock;
  bool want_recv, want_send;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(!Curl_ngh2_valid(&ctx->ngh2))
    ಹಿಂತಿರುಗಿ CURLE_OK;

  sock = Curl_conn_cf_get_socket(cf, ದತ್ತಾಂಶ);
  Curl_pollset_check(ದತ್ತಾಂಶ, ps, sock, &want_recv, &want_send);
  ಆದರೆ(want_recv || want_send) {
    ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು = H2_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);
    bool c_exhaust, s_exhaust;

    CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
    c_exhaust = want_send &&
                !Curl_ngh2_session_get_remote_ಕಿಟಕಿ_ಗಾತ್ರ(&ctx->ngh2);
    s_exhaust = want_send && ಹರಿವು && ಹರಿವು->id >= 0 &&
                !Curl_ngh2_session_get_ಹರಿವು_remote_ಕಿಟಕಿ_ಗಾತ್ರ(
                  &ctx->ngh2, ಹರಿವು->id);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                (!c_exhaust && Curl_ngh2_session_want_write(&ctx->ngh2)) ||
                !Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->outbufq);

    ಫಲಿತಾಂಶ = Curl_pollset_ಗಣ(ದತ್ತಾಂಶ, ps, sock, want_recv, want_send);
    CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ctx->ಕಳುಹಿಸಿದ_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->outbufq);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    ಫಲಿತಾಂಶ = Curl_pollset_ಗಣ(ದತ್ತಾಂಶ, ps, sock, want_recv, want_send);
    CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_h2_ctx_open(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು;
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  ಪೂರ್ಣಾಂಕ rc;
  nghttp2_session_callbacks *cbs = ಖಾಲಿ;

  DEBUGASSERT(!ctx->ngh2.s);
  DEBUGASSERT(!ctx->ngh2.fatal_rv);
  DEBUGASSERT(ctx->initialized);

  rc = nghttp2_session_callbacks_new(&cbs);
  ಆದರೆ(rc) {
    failf(ದತ್ತಾಂಶ, "Could not initialize nghttp2 callbacks");
    ಹೋಗು ಹೊರಹೋಗು;
  }

  nghttp2_session_callbacks_ಗಣ_send_callback(cbs, send_callback);
  nghttp2_session_callbacks_ಗಣ_on_ಚೌಕಟ್ಟು_recv_callback(cbs, on_ಚೌಕಟ್ಟು_recv);
  nghttp2_session_callbacks_ಗಣ_on_invalid_ಚೌಕಟ್ಟು_recv_callback(cbs,
    cf_h2_on_invalid_ಚೌಕಟ್ಟು_recv);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  nghttp2_session_callbacks_ಗಣ_on_ಚೌಕಟ್ಟು_send_callback(cbs, on_ಚೌಕಟ್ಟು_send);
#ಆದರೆ_ಅಂತ್ಯ
  nghttp2_session_callbacks_ಗಣ_on_ದತ್ತಾಂಶ_chunk_recv_callback(
    cbs, on_ದತ್ತಾಂಶ_chunk_recv);
  nghttp2_session_callbacks_ಗಣ_on_ಹರಿವು_close_callback(cbs, on_ಹರಿವು_close);
  nghttp2_session_callbacks_ಗಣ_on_ಶೀರ್ಷಿಕೆ_callback(cbs, on_ಶೀರ್ಷಿಕೆ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  nghttp2_session_callbacks_ಗಣ_error_callback(cbs, error_callback);
#ಆದರೆ_ಅಂತ್ಯ

  /* The nghttp2 session is not yet setup, do it */
  rc = h2_client_new(cf, cbs);
  ಆದರೆ(rc) {
    failf(ದತ್ತಾಂಶ, "Could not initialize nghttp2");
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ctx->max_concurrent_streams = ದತ್ತಾಂಶ->multi ?
    Curl_multi_max_concurrent_streams(ದತ್ತಾಂಶ->multi) :
    DEFAULT_MAX_CONCURRENT_STREAMS;

  ಆದರೆ(ctx->ಮೂಲಕ_h1_upgrade) {
    /* HTTP/1.1 Upgrade issued. H2 Settings have already been submitted
     * in the H1 request and we upgrade from there. This stream
     * is opened implicitly as #1. */
    uint8_t binsettings[H2_BINSETTINGS_LEN];
    ssize_t rclen;
    size_t binlen; /* length of the binsettings data */

    rclen = populate_binsettings(binsettings, ದತ್ತಾಂಶ);

    ಆದರೆ(!curlx_sztouz(rclen, &binlen) || !binlen) {
      failf(ದತ್ತಾಂಶ, "nghttp2 unexpectedly failed on pack_settings_payload");
      ಫಲಿತಾಂಶ = CURLE_FAILED_INIT;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    ಫಲಿತಾಂಶ = http2_ದತ್ತಾಂಶ_ಸಿದ್ಧತೆ(cf, ದತ್ತಾಂಶ, &ಹರಿವು);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    DEBUGASSERT(ಹರಿವು);
    ಹರಿವು->id = 1;
    /* queue SETTINGS frame (again) */
    rc = Curl_ngh2_session_upgrade2(
      &ctx->ngh2, binsettings, binlen, ದತ್ತಾಂಶ->state.httpreq == HTTPREQ_ತಲೆ,
      ಖಾಲಿ);
    ಆದರೆ(rc) {
      failf(ದತ್ತಾಂಶ, "nghttp2_session_upgrade2() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      ಫಲಿತಾಂಶ = CURLE_HTTP2;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    rc = Curl_ngh2_session_ಗಣ_ಹರಿವು_user_ದತ್ತಾಂಶ(
      &ctx->ngh2, ಹರಿವು->id, ಹರಿವು);
    ಆದರೆ(rc) {
      infof(ದತ್ತಾಂಶ, "http/2: failed to set user_data for stream %d",
            ಹರಿವು->id);
      DEBUGASSERT(0);
    }
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "created session via Upgrade");
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    nghttp2_settings_ನಮೂದು iv[H2_SETTINGS_IV_LEN];
    size_t ivlen;

    ivlen = populate_settings(iv, ದತ್ತಾಂಶ, ctx);
    rc = Curl_ngh2_submit_settings(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, iv, ivlen);
    ಆದರೆ(rc) {
      failf(ದತ್ತಾಂಶ, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      ಫಲಿತಾಂಶ = CURLE_HTTP2;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  rc = Curl_ngh2_session_ಗಣ_local_ಕಿಟಕಿ_ಗಾತ್ರ(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, HTTP2_HUGE_ಕಿಟಕಿ_ಗಾತ್ರ);
  ಆದರೆ(rc) {
    failf(ದತ್ತಾಂಶ, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    ಫಲಿತಾಂಶ = CURLE_HTTP2;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  /* all set, traffic will be send on connect */
  ಫಲಿತಾಂಶ = CURLE_OK;
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[0] created h2 session%s",
              ctx->ಮೂಲಕ_h1_upgrade ? " (via h1 upgrade)" : "");

ಹೊರಹೋಗು:
  ಆದರೆ(cbs)
    nghttp2_session_callbacks_del(cbs);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_h2_connect(ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              bool *ಮುಗಿದ)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;
  bool ಮೊದಲ_ಸಮಯ = FALSE;

  ಆದರೆ(cf->connected) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  /* Connect the lower filters first */
  ಆದರೆ(!cf->ಮುಂದಿನ->connected) {
    ಫಲಿತಾಂಶ = Curl_conn_cf_connect(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, ಮುಗಿದ);
    ಆದರೆ(ಫಲಿತಾಂಶ || !*ಮುಗಿದ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  *ಮುಗಿದ = FALSE;

  CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
  DEBUGASSERT(ctx->initialized);
  ಆದರೆ(!ctx->ngh2.s) {
    ಫಲಿತಾಂಶ = cf_h2_ctx_open(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    ಮೊದಲ_ಸಮಯ = TRUE;
  }

  ಆದರೆ(!ಮೊದಲ_ಸಮಯ) {
    ಫಲಿತಾಂಶ = h2_ಪ್ರಗತಿ_ingress(cf, ದತ್ತಾಂಶ, 0);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  /* Send out our SETTINGS and ACKs and such. If that blocks, we
   * have it buffered and can count this filter as being connected */
  ಫಲಿತಾಂಶ = h2_ಪ್ರಗತಿ_egress(cf, ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ && (ಫಲಿತಾಂಶ != CURLE_AGAIN))
    ಹೋಗು ಹೊರಹೋಗು;

  *ಮುಗಿದ = TRUE;
  cf->connected = TRUE;
  ಫಲಿತಾಂಶ = CURLE_OK;

ಹೊರಹೋಗು:
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "cf_connect() -> %d, %d, ", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *ಮುಗಿದ);
  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_h2_destroy(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಆದರೆ(ctx) {
    cf_h2_ctx_free(ctx);
    cf->ctx = ಖಾಲಿ;
  }
}

ಸ್ಥಿರ CURLcode cf_h2_shutdown(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;
  CURLcode ಫಲಿತಾಂಶ;
  ಪೂರ್ಣಾಂಕ rv;

  ಆದರೆ(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);

  ಆದರೆ(!ctx->ಕಳುಹಿಸಿದ_goaway) {
    ctx->ಕಳುಹಿಸಿದ_goaway = TRUE;
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, ctx->local_max_sid, 0,
      (ಸ್ಥಿರಾಂಕ uint8_t *)"shutdown", ಗಾತ್ರ("shutdown"));
    ಆದರೆ(rv) {
      failf(ದತ್ತಾಂಶ, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  ಫಲಿತಾಂಶ = CURLE_OK;
  ಆದರೆ(Curl_ngh2_session_want_write(&ctx->ngh2) ||
     !Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->outbufq))
    ಫಲಿತಾಂಶ = h2_ಪ್ರಗತಿ_egress(cf, ದತ್ತಾಂಶ);
  ಆದರೆ(!ಫಲಿತಾಂಶ && Curl_ngh2_session_want_read(&ctx->ngh2))
    ಫಲಿತಾಂಶ = h2_ಪ್ರಗತಿ_ingress(cf, ದತ್ತಾಂಶ, 0);

  ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
    ಫಲಿತಾಂಶ = CURLE_OK;

  *ಮುಗಿದ = (ctx->conn_closed ||
           (!ಫಲಿತಾಂಶ && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2) &&
            Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->outbufq)));

ಹೊರಹೋಗು:
  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  cf->shutdown = (ಫಲಿತಾಂಶ || *ಮುಗಿದ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode http2_ದತ್ತಾಂಶ_pause(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 bool pause)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು = H2_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);

  DEBUGASSERT(ದತ್ತಾಂಶ);
  ಆದರೆ(ctx && Curl_ngh2_valid(&ctx->ngh2) && ಹರಿವು) {
    CURLcode ಫಲಿತಾಂಶ;

    ಹರಿವು->write_paused = pause;
    ಫಲಿತಾಂಶ = cf_h2_update_local_win(cf, ದತ್ತಾಂಶ, ಹರಿವು);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    /* attempt to send the window update */
    (ಶೂನ್ಯ)h2_ಪ್ರಗತಿ_egress(cf, ದತ್ತಾಂಶ);

    ಆದರೆ(!pause) {
      /* Unpausing a h2 transfer, requires it to be run again. The server
       * may send new DATA on us increasing the flow window, and it may
       * not. We may have already buffered and exhausted the new window
       * by operating on things in flight during the handling of other
       * transfers. */
      Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
    }
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "[%d] stream now %spaused", ಹರಿವು->id,
                pause ? "" : "un");
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_h2_cntrl(ರಚನೆ Curl_cfilter *cf,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ಪೂರ್ಣಾಂಕ event, ಪೂರ್ಣಾಂಕ arg1, ಶೂನ್ಯ *arg2)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;

  (ಶೂನ್ಯ)arg2;

  CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
  ಆಯ್ಕೆ(event) {
  ಸ್ಥಿತಿ CF_CTRL_ದತ್ತಾಂಶ_ಸಿದ್ಧತೆ:
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CF_CTRL_ದತ್ತಾಂಶ_PAUSE:
    ಫಲಿತಾಂಶ = http2_ದತ್ತಾಂಶ_pause(cf, ದತ್ತಾಂಶ, (arg1 != 0));
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CF_CTRL_FLUSH:
    ಫಲಿತಾಂಶ = cf_h2_flush(cf, ದತ್ತಾಂಶ);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CF_CTRL_ದತ್ತಾಂಶ_ಮುಗಿದ:
    http2_ದತ್ತಾಂಶ_ಮುಗಿದ(cf, ದತ್ತಾಂಶ);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CF_CTRL_CONN_INFO_UPDATE:
    ಆದರೆ(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_ನೋಡಿದ = 20;
      Curl_conn_ಗಣ_multiplex(cf->conn);
    }
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ bool cf_h2_ದತ್ತಾಂಶ_pending(ರಚನೆ Curl_cfilter *cf,
                               ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;

  ಆದರೆ(ctx && !Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->inbufq))
    ಹಿಂತಿರುಗಿ TRUE;
  ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ ? cf->ಮುಂದಿನ->cft->has_ದತ್ತಾಂಶ_pending(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ) : FALSE;
}

ಸ್ಥಿರ bool cf_h2_is_alive(ರಚನೆ Curl_cfilter *cf,
                           ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           bool *ಒಳಹರಿವು_pending)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  bool alive;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;

  *ಒಳಹರಿವು_pending = FALSE;
  CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
           http2_connisalive(cf, ದತ್ತಾಂಶ, ಒಳಹರಿವು_pending));
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "conn alive -> %d, input_pending=%d",
              alive, *ಒಳಹರಿವು_pending);
  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  ಹಿಂತಿರುಗಿ alive;
}

ಸ್ಥಿರ CURLcode cf_h2_keep_alive(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;

  CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
  ಫಲಿತಾಂಶ = http2_send_ping(cf, ದತ್ತಾಂಶ);
  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_h2_query(ರಚನೆ Curl_cfilter *cf,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ಪೂರ್ಣಾಂಕ query, ಪೂರ್ಣಾಂಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ರಚನೆ cf_h2_ctx *ctx = cf->ctx;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;
  size_t effective_max;

  ಆಯ್ಕೆ(query) {
  ಸ್ಥಿತಿ CF_QUERY_MAX_CONCURRENT:
    DEBUGASSERT(pres1);

    CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
    ಆದರೆ(!Curl_ngh2_valid(&ctx->ngh2) ||
       !Curl_ngh2_session_check_request_allowed(&ctx->ngh2)) {
      /* the limit is what we have in use right now */
      effective_max = cf->conn->attached_xfers;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      effective_max = ctx->max_concurrent_streams;
    }
    *pres1 = (effective_max > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) ? ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ : (ಪೂರ್ಣಾಂಕ)effective_max;
    CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ CF_QUERY_ಹರಿವು_ERROR: {
    ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು = H2_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);
    *pres1 = ಹರಿವು ? (ಪೂರ್ಣಾಂಕ)ಹರಿವು->error : 0;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಸ್ಥಿತಿ CF_QUERY_NEED_FLUSH: {
    ರಚನೆ h2_ಹರಿವು_ctx *ಹರಿವು = H2_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ);
    ಆದರೆ(!Curl_bufq_is_ಖಾಲಿಗಳು(&ctx->outbufq) ||
       (ಹರಿವು && !Curl_bufq_is_ಖಾಲಿಗಳು(&ಹರಿವು->sendbuf))) {
      *pres1 = TRUE;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    ನಿಲ್ಲಿಸು;
  }
  ಸ್ಥಿತಿ CF_QUERY_HTTP_ಆವೃತ್ತಿ:
    *pres1 = 20;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ ?
    cf->ಮುಂದಿನ->cft->query(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ರಚನೆ Curl_cftype Curl_cft_nghttp2 = {
  "HTTP/2",
  CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  CURL_LOG_LVL_NONE,
  cf_h2_destroy,
  cf_h2_connect,
  cf_h2_shutdown,
  cf_h2_adjust_pollset,
  cf_h2_ದತ್ತಾಂಶ_pending,
  cf_h2_send,
  cf_h2_recv,
  cf_h2_cntrl,
  cf_h2_is_alive,
  cf_h2_keep_alive,
  cf_h2_query,
};

ಸ್ಥಿರ CURLcode http2_cfilter_ಕೂಡಿಸು(ರಚನೆ Curl_cfilter **pcf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ connectdata *conn,
                                  int8_t sockindex,
                                  bool ಮೂಲಕ_h1_upgrade)
{
  ರಚನೆ Curl_cfilter *cf = ಖಾಲಿ;
  ರಚನೆ cf_h2_ctx *ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  DEBUGASSERT(ದತ್ತಾಂಶ->conn);
  ctx = curlx_calloc(1, ಗಾತ್ರ(*ctx));
  ಆದರೆ(!ctx)
    ಹೋಗು ಹೊರಹೋಗು;
  cf_h2_ctx_init(ctx, ಮೂಲಕ_h1_upgrade);

  ಫಲಿತಾಂಶ = Curl_cf_create(&cf, &Curl_cft_nghttp2, ctx);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ctx = ಖಾಲಿ;
  Curl_conn_cf_ಕೂಡಿಸು(ದತ್ತಾಂಶ, conn, sockindex, cf);

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ)
    cf_h2_ctx_free(ctx);
  *pcf = ಫಲಿತಾಂಶ ? ಖಾಲಿ : cf;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode http2_cfilter_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf,
                                           ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                           bool ಮೂಲಕ_h1_upgrade)
{
  ರಚನೆ Curl_cfilter *cf_h2 = ಖಾಲಿ;
  ರಚನೆ cf_h2_ctx *ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ctx = curlx_calloc(1, ಗಾತ್ರ(*ctx));
  ಆದರೆ(!ctx)
    ಹೋಗು ಹೊರಹೋಗು;
  cf_h2_ctx_init(ctx, ಮೂಲಕ_h1_upgrade);

  ಫಲಿತಾಂಶ = Curl_cf_create(&cf_h2, &Curl_cft_nghttp2, ctx);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ctx = ಖಾಲಿ;
  Curl_conn_cf_ಸೇರಿಸು_after(cf, cf_h2);

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ)
    cf_h2_ctx_free(ctx);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

bool Curl_http2_may_switch(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(Curl_conn_http_ಆವೃತ್ತಿ(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn) < 20 &&
     (ದತ್ತಾಂಶ->state.http_neg.wanted & CURL_HTTP_V2x) &&
     ದತ್ತಾಂಶ->state.http_neg.h2_ಪೂರ್ವ_knowledge) {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
    ಆದರೆ(ದತ್ತಾಂಶ->conn->ಬಿಟ್ಟುಗಳು.origin_is_proxy) {
      infof(ದತ್ತಾಂಶ, "Ignoring HTTP/2 prior knowledge due to proxy");
      ಹಿಂತಿರುಗಿ FALSE;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ TRUE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

CURLcode Curl_http2_switch(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf;
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(Curl_conn_http_ಆವೃತ್ತಿ(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn) < 20);

  ಫಲಿತಾಂಶ = http2_cfilter_ಕೂಡಿಸು(&cf, ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn, FIRSTSOCKET, FALSE);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "switching connection to HTTP/2");

  ಆದರೆ(cf->ಮುಂದಿನ) {
    bool ಮುಗಿದ;
    ಹಿಂತಿರುಗಿ Curl_conn_cf_connect(cf, ದತ್ತಾಂಶ, &ಮುಗಿದ);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_http2_switch_at(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cfilter *cf_h2;
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(Curl_conn_http_ಆವೃತ್ತಿ(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn) < 20);

  ಫಲಿತಾಂಶ = http2_cfilter_ಸೇರಿಸು_after(cf, ದತ್ತಾಂಶ, FALSE);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  cf_h2 = cf->ಮುಂದಿನ;

  ಆದರೆ(cf_h2->ಮುಂದಿನ) {
    bool ಮುಗಿದ;
    ಹಿಂತಿರುಗಿ Curl_conn_cf_connect(cf_h2, ದತ್ತಾಂಶ, &ಮುಗಿದ);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_http2_upgrade(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ connectdata *conn, int8_t sockindex,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mem, size_t nread)
{
  ರಚನೆ Curl_cfilter *cf;
  ರಚನೆ cf_h2_ctx *ctx;
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(Curl_conn_http_ಆವೃತ್ತಿ(ದತ್ತಾಂಶ, conn) < 20);

  ಫಲಿತಾಂಶ = http2_cfilter_ಕೂಡಿಸು(&cf, ದತ್ತಾಂಶ, conn, sockindex, TRUE);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "upgrading connection to HTTP/2");

  DEBUGASSERT(cf->cft == &Curl_cft_nghttp2);
  ctx = cf->ctx;

  ದತ್ತಾಂಶ->req.httpversion_ಕಳುಹಿಸಿದ = 20; /* it is an h2 request now */
  ದತ್ತಾಂಶ->req.ಶೀರ್ಷಿಕೆ = TRUE; /* we expect the real response to come in h2 */
  ದತ್ತಾಂಶ->req.headerline = 0; /* restart the header line counter */

  ಆದರೆ(nread > 0) {
    /* Remaining data from the protocol switch reply is already using
     * the switched protocol, ie. HTTP/2. We add that to the network
     * inbufq. */
    size_t copied;

    ಫಲಿತಾಂಶ = Curl_bufq_write(&ctx->inbufq,
                             (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)mem, nread, &copied);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      failf(ದತ್ತಾಂಶ, "error on copying HTTP Upgrade response: %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
      ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
    }
    ಆದರೆ(copied < nread) {
      failf(ದತ್ತಾಂಶ, "connection buffer size could not take all data "
            "from HTTP Upgrade response header: copied=%zu, datalen=%zu",
            copied, nread);
      ಹಿಂತಿರುಗಿ CURLE_HTTP2;
    }
    infof(ದತ್ತಾಂಶ, "Copied HTTP/2 data in stream buffer to connection buffer"
          " after upgrade: len=%zu", nread);
  }

  ಆದರೆ(cf->ಮುಂದಿನ) {
    bool ಮುಗಿದ;
    ಫಲಿತಾಂಶ = Curl_conn_cf_connect(cf, ದತ್ತಾಂಶ, &ಮುಗಿದ);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      cf->cft->cntrl(cf, ದತ್ತಾಂಶ, CF_CTRL_CONN_INFO_UPDATE, 0, ಖಾಲಿ);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Only call this function for a transfer that already got an HTTP/2
   CURLE_HTTP2_STREAM error! */
bool Curl_h2_http_1_1_error(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(Curl_conn_http_ಆವೃತ್ತಿ(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn) == 20) {
    ಪೂರ್ಣಾಂಕ err = Curl_conn_get_ಹರಿವು_error(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->conn, FIRSTSOCKET);
    ಹಿಂತಿರುಗಿ err == NGHTTP2_HTTP_1_1_REQUIRED;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

ಶೂನ್ಯ *Curl_nghttp2_malloc(size_t ಗಾತ್ರ_2, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ)
{
  (ಶೂನ್ಯ)user_ದತ್ತಾಂಶ;
  ಹಿಂತಿರುಗಿ Curl_cmalloc(ಗಾತ್ರ_2);
}

ಶೂನ್ಯ Curl_nghttp2_free(ಶೂನ್ಯ *ptr, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ)
{
  (ಶೂನ್ಯ)user_ದತ್ತಾಂಶ;
  Curl_cfree(ptr);
}

ಶೂನ್ಯ *Curl_nghttp2_calloc(size_t nmemb, size_t ಗಾತ್ರ_2, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ)
{
  (ಶೂನ್ಯ)user_ದತ್ತಾಂಶ;
  ಹಿಂತಿರುಗಿ Curl_ccalloc(nmemb, ಗಾತ್ರ_2);
}

ಶೂನ್ಯ *Curl_nghttp2_realloc(ಶೂನ್ಯ *ptr, size_t ಗಾತ್ರ_2, ಶೂನ್ಯ *user_ದತ್ತಾಂಶ)
{
  (ಶೂನ್ಯ)user_ದತ್ತಾಂಶ;
  ಹಿಂತಿರುಗಿ Curl_crealloc(ptr, ಗಾತ್ರ_2);
}

#ಇಲ್ಲದಿದ್ದರೆ /* CURL_DISABLE_HTTP || !USE_NGHTTP2 */

/* Satisfy external references even if http2 is not compiled in. */

ಅಕ್ಷರ *curl_pushheader_bynum(ರಚನೆ curl_pushheaders *h, size_t ಅಂಕೆ)
{
  (ಶೂನ್ಯ)h;
  (ಶೂನ್ಯ)ಅಂಕೆ;
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಅಕ್ಷರ *curl_pushheader_byname(ರಚನೆ curl_pushheaders *h, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
  (ಶೂನ್ಯ)h;
  (ಶೂನ್ಯ)name;
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
