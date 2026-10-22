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

#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
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

#यदि NGHTTP2_संस्करणम्_अङ्कः < 0x010f00
#दोषः "nghttp2 1.15.0 or greater required"
#यदि_अन्तः

#परिभाषय NGHTTP2_HAS_समुच्चयः_LOCAL_गवाक्षः_आकार 1

/* buffer dimensioning:
 * use 16K as chunk size, as that fits H2 DATA frames well */
#परिभाषय H2_CHUNK_आकार           (16 * 1024)
/* connection window size */
#परिभाषय H2_CONN_गवाक्षः_आकार     (10 * 1024 * 1024)
/* on receiving from TLS, we prep for holding a full stream window */
#परिभाषय H2_NW_RECV_CHUNKS       (H2_CONN_गवाक्षः_आकार / H2_CHUNK_आकार)
/* on send into TLS, we want to accumulate small frames */
#परिभाषय H2_NW_SEND_CHUNKS       1
/* this is how much we want "in flight" for a stream, unthrottled */
#परिभाषय H2_प्रवाहः_गवाक्षः_आकार_MAX   (10 * 1024 * 1024)
/* this is how much we want "in flight" for a stream, initially, IFF
 * nghttp2 allows us to tweak the local window size. */
#यदि NGHTTP2_HAS_समुच्चयः_LOCAL_गवाक्षः_आकार
#परिभाषय H2_प्रवाहः_गवाक्षः_आकार_INITIAL  (64 * 1024)
#अन्यथा
#परिभाषय H2_प्रवाहः_गवाक्षः_आकार_INITIAL H2_प्रवाहः_गवाक्षः_आकार_MAX
#यदि_अन्तः
/* keep smaller stream upload buffer (default h2 window size) to have
 * our progress bars and "upload done" reporting closer to reality */
#परिभाषय H2_प्रवाहः_SEND_CHUNKS   ((64 * 1024) / H2_CHUNK_आकार)
/* spare chunks we keep for a full window */
#परिभाषय H2_प्रवाहः_POOL_SPARES   (H2_CONN_गवाक्षः_आकार / H2_CHUNK_आकार)

/* We need to accommodate the max number of streams with their window sizes on
 * the overall connection. Streams might become PAUSED which will block their
 * received QUOTA in the connection window. If we run out of space, the server
 * is blocked from sending us any data. See #10988 for an issue with this. */
#परिभाषय HTTP2_HUGE_गवाक्षः_आकार (100 * H2_प्रवाहः_गवाक्षः_आकार_MAX)

#परिभाषय H2_SETTINGS_IV_LEN 3
#परिभाषय H2_BINSETTINGS_LEN 80

संरचना cf_h2_ctx {
  संरचना curl_ngh2 ngh2;
  /* The easy handle used in the current filter call, cleared at return */
  संरचना cf_call_दत्त call_दत्त;

  संरचना bufq inbufq;           /* network input */
  संरचना bufq outbufq;          /* network output */
  संरचना bufc_pool प्रवाहः_bufcp; /* spares for stream buffers */
  संरचना dynbuf scratch;        /* scratch buffer for temp use */

  संरचना u32_ptrset streams; /* hash of `data->mid` to `h2_stream_ctx` */
  size_t drain_योग; /* sum of all stream's UrlState drain */
  uint32_t initial_win_आकार; /* current initial window size (settings) */
  uint32_t max_concurrent_streams;
  uint32_t goaway_error;        /* goaway error code from server */
  int32_t remote_max_sid;       /* max id processed by server */
  int32_t local_max_sid;        /* max id processed by us */
  BIT(initialized);
  BIT(द्वारा_h1_upgrade);
  BIT(conn_closed);
  BIT(rcvd_goaway);
  BIT(प्रेषितम्_goaway);
  BIT(enable_प्रक्षिप);
  BIT(nw_बहिर्गमनम्_blocked);
};

/* How to access `call_data` from a cf_h2 filter */
#अपरिभाषय CF_CTX_CALL_दत्त
#परिभाषय CF_CTX_CALL_दत्त(cf) ((संरचना cf_h2_ctx *)(cf)->ctx)->call_दत्त

/**
 * All about the H2 internals of a stream
 */
संरचना h2_प्रवाहः_ctx {
  संरचना bufq sendbuf; /* request buffer */
  संरचना h1_req_parser h1; /* parsing the request */
  संरचना dynhds resp_trailers; /* response trailer fields */
  size_t resp_hds_len; /* amount of response header bytes in recvbuf */
  curl_off_t xfer_id;  /* transfer `id` owning this stream */
  curl_off_t nrcvd_दत्त;  /* number of DATA bytes received */

  वर्ण **प्रक्षिप_शीर्षकाणि;       /* allocated array */
  size_t प्रक्षिप_शीर्षकाणि_प्रयुक्तम्;  /* number of entries filled in */
  size_t प्रक्षिप_शीर्षकाणि_alloc; /* number of entries allocated */

  int32_t id; /* HTTP/2 protocol identifier for stream */
  uint32_t mid; /* transfer `mid` owning this stream */
  uint32_t error; /* stream error code */
  CURLcode xfer_परिणाम; /* Result of writing out response */
  int32_t local_गवाक्षः_आकार; /* the local recv window size */
  int32_t स्थितिः_संहिता; /* HTTP response status code */
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

स्थिर शून्य free_प्रक्षिप_शीर्षकाणि(संरचना h2_प्रवाहः_ctx *प्रवाहः)
{
  size_t i;
  प्रति(i = 0; i < प्रवाहः->प्रक्षिप_शीर्षकाणि_प्रयुक्तम्; i++)
    curlx_free(प्रवाहः->प्रक्षिप_शीर्षकाणि[i]);
  curlx_safefree(प्रवाहः->प्रक्षिप_शीर्षकाणि);
  प्रवाहः->प्रक्षिप_शीर्षकाणि_प्रयुक्तम् = 0;
}

स्थिर शून्य h2_प्रवाहः_ctx_free(संरचना h2_प्रवाहः_ctx *प्रवाहः)
{
  Curl_bufq_free(&प्रवाहः->sendbuf);
  Curl_h1_req_parse_free(&प्रवाहः->h1);
  Curl_dynhds_free(&प्रवाहः->resp_trailers);
  free_प्रक्षिप_शीर्षकाणि(प्रवाहः);
  curlx_free(प्रवाहः);
}

स्थिर शून्य h2_प्रवाहः_hash_free(uint32_t id, शून्य *प्रवाहः)
{
  (शून्य)id;
  DEBUGASSERT(प्रवाहः);
  h2_प्रवाहः_ctx_free((संरचना h2_प्रवाहः_ctx *)प्रवाहः);
}

स्थिर शून्य cf_h2_ctx_init(संरचना cf_h2_ctx *ctx, bool द्वारा_h1_upgrade)
{
  Curl_bufcp_init(&ctx->प्रवाहः_bufcp, H2_CHUNK_आकार, H2_प्रवाहः_POOL_SPARES);
  Curl_bufq_initp(&ctx->inbufq, &ctx->प्रवाहः_bufcp, H2_NW_RECV_CHUNKS, 0);
  Curl_bufq_initp(&ctx->outbufq, &ctx->प्रवाहः_bufcp, H2_NW_SEND_CHUNKS, 0);
  curlx_dyn_init(&ctx->scratch, CURL_MAX_HTTP_शीर्षकम्);
  Curl_u32_ptrset_init(&ctx->streams, h2_प्रवाहः_hash_free);
  ctx->remote_max_sid = INT32_MAX;
  ctx->द्वारा_h1_upgrade = द्वारा_h1_upgrade;
  ctx->initialized = TRUE;
}

स्थिर शून्य cf_h2_ctx_free(संरचना cf_h2_ctx *ctx)
{
  यदि(ctx && ctx->initialized) {
    Curl_ngh2_del(&ctx->ngh2);
    Curl_bufq_free(&ctx->inbufq);
    Curl_bufq_free(&ctx->outbufq);
    Curl_bufcp_free(&ctx->प्रवाहः_bufcp);
    curlx_dyn_free(&ctx->scratch);
    Curl_u32_ptrset_clear(&ctx->streams);
    स्मृतिं_पूरय(ctx, 0, परिमाण(*ctx));
  }
  curlx_free(ctx);
}

स्थिर uint32_t cf_h2_initial_win_आकार(संरचना Curl_easy *दत्त)
{
#यदि NGHTTP2_HAS_समुच्चयः_LOCAL_गवाक्षः_आकार
  /* If the transfer has a rate-limit lower than the default initial
   * stream window size, use that. It needs to be at least 8k or servers
   * may be unhappy. */
  curl_off_t rps = Curl_rlimit_प्रत्येकम्_चरणः(&दत्त->प्रगतिः.dl.rlimit);
  यदि((rps > 0) && (rps < H2_प्रवाहः_गवाक्षः_आकार_INITIAL))
    प्रत्यागम CURLMAX((uint32_t)rps, 8192);
#यदि_अन्तः
  प्रत्यागम H2_प्रवाहः_गवाक्षः_आकार_INITIAL;
}

स्थिर size_t populate_settings(nghttp2_settings_प्रविष्टिः *iv,
                                संरचना Curl_easy *दत्त,
                                संरचना cf_h2_ctx *ctx)
{
  iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
  iv[0].मानम् = Curl_multi_max_concurrent_streams(दत्त->multi);

  iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_गवाक्षः_आकार;
  iv[1].मानम् = cf_h2_initial_win_आकार(दत्त);
  यदि(ctx)
    ctx->initial_win_आकार = iv[1].मानम्;
  iv[2].settings_id = NGHTTP2_SETTINGS_ENABLE_प्रक्षिप;
  iv[2].मानम् = !!दत्त->multi->प्रक्षिप_cb;

  प्रत्यागम 3;
}

स्थिर ssize_t populate_binsettings(uint8_t *binsettings,
                                    संरचना Curl_easy *दत्त)
{
  nghttp2_settings_प्रविष्टिः iv[H2_SETTINGS_IV_LEN];
  size_t ivlen;

  ivlen = populate_settings(iv, दत्त, रिक्त);
  /* this returns number of bytes it wrote or a negative number on error. */
  प्रत्यागम nghttp2_pack_settings_payload(binsettings, H2_BINSETTINGS_LEN,
                                       iv, ivlen);
}

स्थिर CURLcode cf_h2_update_settings(संरचना cf_h2_ctx *ctx,
                                      uint32_t initial_win_आकार)
{
  nghttp2_settings_प्रविष्टिः प्रविष्टिः;
  प्रविष्टिः.settings_id = NGHTTP2_SETTINGS_INITIAL_गवाक्षः_आकार;
  प्रविष्टिः.मानम् = initial_win_आकार;
  यदि(Curl_ngh2_submit_settings(&ctx->ngh2, NGHTTP2_FLAG_NONE, &प्रविष्टिः, 1))
    प्रत्यागम CURLE_SEND_ERROR;
  ctx->initial_win_आकार = initial_win_आकार;
  प्रत्यागम CURLE_OK;
}

#परिभाषय H2_प्रवाहः_CTX(ctx, दत्त)                                         \
  ((संरचना h2_प्रवाहः_ctx *)(                                             \
    (दत्त) ? Curl_u32_ptrset_get(&(ctx)->streams, (दत्त)->mid) : रिक्त))

स्थिर संरचना h2_प्रवाहः_ctx *h2_प्रवाहः_ctx_create(संरचना cf_h2_ctx *ctx,
                                                  संरचना Curl_easy *दत्त)
{
  संरचना h2_प्रवाहः_ctx *प्रवाहः;

  प्रवाहः = curlx_calloc(1, परिमाण(*प्रवाहः));
  यदि(!प्रवाहः)
    प्रत्यागम रिक्त;

  प्रवाहः->id = -1;
  प्रवाहः->xfer_id = दत्त->id;
  प्रवाहः->mid = दत्त->mid;
  Curl_bufq_initp(&प्रवाहः->sendbuf, &ctx->प्रवाहः_bufcp,
                  H2_प्रवाहः_SEND_CHUNKS, BUFQ_OPT_NONE);
  Curl_h1_req_parse_init(&प्रवाहः->h1, H1_PARSE_DEFAULT_MAX_पङ्क्तिः_LEN);
  Curl_dynhds_init(&प्रवाहः->resp_trailers,
                   MAX_HTTP_RESP_शीर्षकम्_गणनम्, DYN_HTTP_REQUEST);
  प्रवाहः->bodystarted = FALSE;
  प्रवाहः->स्थितिः_संहिता = -1;
  प्रवाहः->closed = FALSE;
  प्रवाहः->close_handled = FALSE;
  प्रवाहः->error = NGHTTP2_NO_ERROR;
  प्रवाहः->local_गवाक्षः_आकार = H2_प्रवाहः_गवाक्षः_आकार_INITIAL;
  प्रवाहः->nrcvd_दत्त = 0;
  प्रत्यागम प्रवाहः;
}

स्थिर संरचना Curl_easy *cf_h2_get_प्रवाहः_xfer(संरचना Curl_cfilter *cf,
                                               int32_t प्रवाहः_id,
                                               संरचना h2_प्रवाहः_ctx **pstream)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना h2_प्रवाहः_ctx *प्रवाहः =
    Curl_ngh2_session_get_प्रवाहः_user_दत्त(&ctx->ngh2, प्रवाहः_id);
  यदि(pstream)
    *pstream = प्रवाहः;
  यदि(प्रवाहः && (प्रवाहः->mid != UINT32_MAX)) {
    संरचना Curl_easy *दत्त, *call_दत्त = CF_दत्त_CURRENT(cf);
    यदि(!call_दत्त || !call_दत्त->multi) {
      DEBUGASSERT(0);
      प्रत्यागम रिक्त;
    }
    दत्त = Curl_multi_get_easy(call_दत्त->multi, प्रवाहः->mid);
    यदि(दत्त && (दत्त->id == प्रवाहः->xfer_id))
      प्रत्यागम दत्त;
  }
  प्रत्यागम रिक्त;
}

#यदि_परिभाषितम् NGHTTP2_HAS_समुच्चयः_LOCAL_गवाक्षः_आकार
स्थिर int32_t cf_h2_get_desired_local_win(संरचना Curl_cfilter *cf,
                                           संरचना Curl_easy *दत्त)
{
  curl_off_t avail = Curl_rlimit_avail(&दत्त->प्रगतिः.dl.rlimit, रिक्त);

  (शून्य)cf;
  यदि(avail < CURL_OFF_T_MAX) { /* limit in place */
    यदि(avail <= 0)
      प्रत्यागम 0;
    अन्यथा यदि(avail < H2_प्रवाहः_गवाक्षः_आकार_MAX)
      प्रत्यागम (int32_t)avail;
  }
  प्रत्यागम H2_प्रवाहः_गवाक्षः_आकार_MAX;
}

स्थिर CURLcode cf_h2_update_local_win(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *दत्त,
                                       संरचना h2_प्रवाहः_ctx *प्रवाहः)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  int32_t dwsize;
  पूर्णाङ्क rv;

  dwsize = (प्रवाहः->write_paused || प्रवाहः->xfer_परिणाम) ?
           0 : cf_h2_get_desired_local_win(cf, दत्त);
  यदि(dwsize != प्रवाहः->local_गवाक्षः_आकार) {
    int32_t wsize = Curl_ngh2_session_get_प्रवाहः_effective_local_गवाक्षः_आकार(
      &ctx->ngh2, प्रवाहः->id);
    यदि(dwsize > wsize) {
      rv = Curl_ngh2_session_समुच्चयः_local_गवाक्षः_आकार(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, प्रवाहः->id, dwsize);
      यदि(rv) {
        failf(दत्त, "[%d] nghttp2 set_local_window_size(%d) failed: "
              "%s(%d)", प्रवाहः->id, dwsize, nghttp2_strerror(rv), rv);
        प्रत्यागम CURLE_HTTP2;
      }
      rv = Curl_ngh2_submit_गवाक्षः_update(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, प्रवाहः->id, dwsize - wsize);
      यदि(rv) {
        failf(दत्त, "[%d] nghttp2_submit_window_update() failed: "
              "%s(%d)", प्रवाहः->id, nghttp2_strerror(rv), rv);
        प्रत्यागम CURLE_HTTP2;
      }
      प्रवाहः->local_गवाक्षः_आकार = dwsize;
      CURL_TRC_CF(दत्त, cf, "[%d] local window update by %d",
                  प्रवाहः->id, dwsize - wsize);
    }
    अन्यथा {
      rv = Curl_ngh2_session_समुच्चयः_local_गवाक्षः_आकार(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, प्रवाहः->id, dwsize);
      यदि(rv) {
        failf(दत्त, "[%d] nghttp2_session_set_local_window_size() failed: "
              "%s(%d)", प्रवाहः->id, nghttp2_strerror(rv), rv);
        प्रत्यागम CURLE_HTTP2;
      }
      प्रवाहः->local_गवाक्षः_आकार = dwsize;
      CURL_TRC_CF(दत्त, cf, "[%d] local window size now %d",
                  प्रवाहः->id, dwsize);
    }
  }
  प्रत्यागम CURLE_OK;
}

#अन्यथा /* NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

स्थिर CURLcode cf_h2_update_local_win(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *दत्त,
                                       संरचना h2_प्रवाहः_ctx *प्रवाहः)
{
  (शून्य)cf;
  (शून्य)दत्त;
  (शून्य)प्रवाहः;
  प्रत्यागम CURLE_OK;
}
#यदि_अन्तः /* !NGHTTP2_HAS_SET_LOCAL_WINDOW_SIZE */

स्थिर CURLcode http2_दत्त_सज्जता(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 संरचना h2_प्रवाहः_ctx **pstream)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना h2_प्रवाहः_ctx *प्रवाहः;

  (शून्य)cf;
  DEBUGASSERT(दत्त);

  यदि(!दत्त)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  प्रवाहः = H2_प्रवाहः_CTX(ctx, दत्त);
  यदि(प्रवाहः) {
    *pstream = प्रवाहः;
    प्रत्यागम CURLE_OK;
  }

  प्रवाहः = h2_प्रवाहः_ctx_create(ctx, दत्त);
  यदि(!प्रवाहः)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  यदि(Curl_u32_ptrset_समुच्चयः(&ctx->streams, दत्त->mid, प्रवाहः)) {
    h2_प्रवाहः_ctx_free(प्रवाहः);
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  *pstream = प्रवाहः;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode nw_बहिर्गमनम्_flush(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *दत्त)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  size_t nwritten;
  CURLcode परिणाम;

  यदि(Curl_bufq_is_रिक्तानि(&ctx->outbufq))
    प्रत्यागम CURLE_OK;

  परिणाम = Curl_cf_send_bufq(cf->अग्र, दत्त, &ctx->outbufq, रिक्त, 0,
                             &nwritten);
  यदि(परिणाम) {
    यदि(परिणाम == CURLE_AGAIN) {
      CURL_TRC_CF(दत्त, cf, "flush nw send buffer(%zu) -> EAGAIN",
                  Curl_bufq_len(&ctx->outbufq));
      ctx->nw_बहिर्गमनम्_blocked = 1;
    }
    प्रत्यागम परिणाम;
  }
  प्रत्यागम Curl_bufq_is_रिक्तानि(&ctx->outbufq) ? CURLE_OK : CURLE_AGAIN;
}

स्थिर शून्य http2_दत्त_समाप्तम्(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना h2_प्रवाहः_ctx *प्रवाहः = H2_प्रवाहः_CTX(ctx, दत्त);

  DEBUGASSERT(ctx);
  यदि(!प्रवाहः || !ctx->initialized)
    प्रत्यागम;

  यदि(Curl_ngh2_valid(&ctx->ngh2)) {
    bool flush_egress = FALSE;
    /* returns error if stream not known, which is fine here */
    (शून्य)Curl_ngh2_session_समुच्चयः_प्रवाहः_user_दत्त(&ctx->ngh2, प्रवाहः->id, रिक्त);

    यदि(!प्रवाहः->closed && प्रवाहः->id > 0) {
      /* RST_STREAM */
      CURL_TRC_CF(दत्त, cf, "[%d] premature DATA_DONE, RST stream",
                  प्रवाहः->id);
      प्रवाहः->closed = TRUE;
      प्रवाहः->reset = TRUE;
      Curl_ngh2_submit_rst_प्रवाहः(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                  प्रवाहः->id, NGHTTP2_प्रवाहः_CLOSED);
      flush_egress = TRUE;
    }

    यदि(flush_egress) {
      (शून्य)Curl_ngh2_session_send(&ctx->ngh2);
      (शून्य)nw_बहिर्गमनम्_flush(cf, दत्त);
    }
  }

  Curl_u32_ptrset_unset(&ctx->streams, दत्त->mid);
}

स्थिर पूर्णाङ्क h2_client_new(संरचना Curl_cfilter *cf,
                         nghttp2_session_callbacks *cbs)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  nghttp2_option *o;
  nghttp2_mem mem = { रिक्त, Curl_nghttp2_malloc, Curl_nghttp2_free,
                      Curl_nghttp2_calloc, Curl_nghttp2_realloc };

  पूर्णाङ्क rc = nghttp2_option_new(&o);
  यदि(rc)
    प्रत्यागम rc;
  /* We handle window updates ourself to enforce buffer limits */
  nghttp2_option_समुच्चयः_no_auto_गवाक्षः_update(o, 1);
#यदि NGHTTP2_संस्करणम्_अङ्कः >= 0x013200 /* with 1.50.0 */
  /* turn off RFC 9113 leading and trailing white spaces validation against
     HTTP field value. */
  nghttp2_option_समुच्चयः_no_rfc9113_leading_and_trailing_ws_validation(o, 1);
#यदि_अन्तः
  rc = Curl_ngh2_session_client_new3(&ctx->ngh2, cbs, cf, o, &mem);
  nghttp2_option_del(o);
  प्रत्यागम rc;
}

/*
 * Returns nonzero if current HTTP/2 session should be closed.
 */
स्थिर पूर्णाङ्क should_close_session(संरचना cf_h2_ctx *ctx)
{
  प्रत्यागम ctx->drain_योग == 0 && !Curl_ngh2_session_want_read(&ctx->ngh2) &&
    !Curl_ngh2_session_want_write(&ctx->ngh2);
}

/*
 * Processes pending input left in network input buffer.
 * This function returns 0 if it succeeds, or -1 and error code will
 * be assigned to *err.
 */
स्थिर CURLcode h2_process_pending_निवेशः(संरचना Curl_cfilter *cf,
                                         संरचना Curl_easy *दत्त)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  ध्रुव अचिह्नित वर्ण *buf;
  size_t blen, nread;
  ssize_t rv;

  यावत्(Curl_bufq_peek(&ctx->inbufq, &buf, &blen)) {
    rv = Curl_ngh2_session_mem_recv(&ctx->ngh2, (ध्रुव uint8_t *)buf, blen);
    यदि(!curlx_sztouz(rv, &nread)) {
      failf(दत्त, "nghttp2 recv error %zd: %s", rv, nghttp2_strerror((पूर्णाङ्क)rv));
      प्रत्यागम CURLE_RECV_ERROR;
    }
    Curl_bufq_skip(&ctx->inbufq, nread);
    यदि(Curl_bufq_is_रिक्तानि(&ctx->inbufq)) {
      भङ्ग;
    }
    अन्यथा {
      CURL_TRC_CF(दत्त, cf, "process_pending_input: %zu bytes left "
                  "in connection buffer", Curl_bufq_len(&ctx->inbufq));
    }
  }

  यदि(Curl_ngh2_session_check_request_allowed(&ctx->ngh2) == 0) {
    /* No more requests are allowed in the current session, so
       the connection may not be reused. This is set when a
       GOAWAY frame has been received or when the limit of stream
       identifiers has been reached. */
    CURL_TRC_M(दत्त, "http/2: No new requests allowed");
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
स्थिर bool http2_connisalive(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                              bool *निवेशः_pending)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *निवेशः_pending = FALSE;
  यदि(!cf->अग्र || !cf->अग्र->cft->is_alive(cf->अग्र, दत्त, निवेशः_pending))
    प्रत्यागम FALSE;

  यदि(*निवेशः_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    CURLcode परिणाम;
    size_t nread;

    *निवेशः_pending = FALSE;
    परिणाम = Curl_cf_recv_bufq(cf->अग्र, दत्त, &ctx->inbufq, 0, &nread);
    CURL_TRC_CF(दत्त, cf, "connisalive, recv pending input -> %d, %zu",
                (पूर्णाङ्क)परिणाम, nread);
    यदि(!परिणाम) {
      परिणाम = h2_process_pending_निवेशः(cf, दत्त);
      यदि(परिणाम)
        /* immediate error, considered dead */
        alive = FALSE;
      अन्यथा {
        alive = !should_close_session(ctx);
      }
    }
    अन्यथा यदि(परिणाम != CURLE_AGAIN) {
      /* the read failed so let's say this is dead anyway */
      alive = FALSE;
    }
  }

  प्रत्यागम alive;
}

स्थिर CURLcode http2_send_ping(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  पूर्णाङ्क rc;

  rc = Curl_ngh2_submit_ping(&ctx->ngh2, 0, ZERO_NULL);
  यदि(rc) {
    failf(दत्त, "nghttp2_submit_ping() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    प्रत्यागम CURLE_HTTP2;
  }

  rc = Curl_ngh2_session_send(&ctx->ngh2);
  यदि(rc) {
    failf(दत्त, "nghttp2_session_send() failed: %s(%d)",
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
  nghttp2_info *h2 = nghttp2_संस्करणम्(0);
  (शून्य)curl_msnprintf(p, len, "nghttp2/%s", h2->संस्करणम्_str);
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
  संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);
  size_t nwritten;
  CURLcode परिणाम = CURLE_OK;

  (शून्य)h2;
  (शून्य)flags;
  DEBUGASSERT(दत्त);

  यदि(!cf->connected)
    परिणाम = Curl_bufq_write(&ctx->outbufq, buf, blen, &nwritten);
  अन्यथा
    परिणाम = Curl_cf_send_bufq(cf->अग्र, दत्त, &ctx->outbufq, buf, blen,
                               &nwritten);

  यदि(परिणाम) {
    यदि(परिणाम == CURLE_AGAIN) {
      ctx->nw_बहिर्गमनम्_blocked = 1;
      प्रत्यागम NGHTTP2_ERR_WOULDBLOCK;
    }
    failf(दत्त, "Failed sending HTTP2 data");
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  यदि(!nwritten) {
    ctx->nw_बहिर्गमनम्_blocked = 1;
    प्रत्यागम NGHTTP2_ERR_WOULDBLOCK;
  }
  प्रत्यागम (nwritten > SSIZE_MAX) ?
    NGHTTP2_ERR_CALLBACK_FAILURE : (ssize_t)nwritten;
}

/* We pass a pointer to this struct in the push callback, but the contents of
   the struct are hidden from the user. */
संरचना curl_pushheaders {
  संरचना Curl_easy *दत्त;
  संरचना h2_प्रवाहः_ctx *प्रवाहः;
  ध्रुव nghttp2_प्रक्षिप_promise *frame;
};

/*
 * push header access function. Only to be used from within the push callback
 */
वर्ण *curl_pushheader_bynum(संरचना curl_pushheaders *h, size_t अङ्कः)
{
  /* Verify that we got a good easy handle in the push header struct, mostly to
     detect rubbish input fast(er). */
  यदि(!h || !GOOD_EASY_HANDLE(h->दत्त))
    प्रत्यागम रिक्त;
  अन्यथा {
    यदि(h->प्रवाहः && अङ्कः < h->प्रवाहः->प्रक्षिप_शीर्षकाणि_प्रयुक्तम्)
      प्रत्यागम h->प्रवाहः->प्रक्षिप_शीर्षकाणि[अङ्कः];
  }
  प्रत्यागम रिक्त;
}

/*
 * push header access function. Only to be used from within the push callback
 */
वर्ण *curl_pushheader_byname(संरचना curl_pushheaders *h, ध्रुव वर्ण *name)
{
  संरचना h2_प्रवाहः_ctx *प्रवाहः;
  size_t len;
  size_t i;
  /* Verify that we got a good easy handle in the push header struct,
     mostly to detect rubbish input fast(er). Also empty header name
     is rubbish too. We have to allow ":" at the beginning of
     the header, but header == ":" must be rejected. If we have ':' in
     the middle of header, it could be matched in middle of the value,
     this is because we do prefix match.*/
  यदि(!h || !GOOD_EASY_HANDLE(h->दत्त) || !name || !name[0] ||
     !वाक्यं_तुलय(name, ":") || strchr(name + 1, ':'))
    प्रत्यागम रिक्त;

  प्रवाहः = h->प्रवाहः;
  यदि(!प्रवाहः)
    प्रत्यागम रिक्त;

  len = वाक्यदैर्घ्यम्(name);
  प्रति(i = 0; i < प्रवाहः->प्रक्षिप_शीर्षकाणि_प्रयुक्तम्; i++) {
    यदि(!वाक्ये_तुलय(name, प्रवाहः->प्रक्षिप_शीर्षकाणि[i], len)) {
      /* sub-match, make sure that it is followed by a colon */
      यदि(प्रवाहः->प्रक्षिप_शीर्षकाणि[i][len] != ':')
        अनुवर्त;
      प्रत्यागम &प्रवाहः->प्रक्षिप_शीर्षकाणि[i][len + 1];
    }
  }
  प्रत्यागम रिक्त;
}

स्थिर संरचना Curl_easy *h2_duphandle(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *दत्त)
{
  संरचना Curl_easy *second = curl_easy_init();
  यदि(second) {
    संरचना h2_प्रवाहः_ctx *second_प्रवाहः;
    http2_दत्त_सज्जता(cf, second, &second_प्रवाहः);
    second->state.weight = दत्त->state.weight;
    यदि(दत्त->share)
      (शून्य)Curl_share_easy_link(second, दत्त->share);
    /* copy a range of options from the parent handle */
    second->समुच्चयः.fwrite_func = दत्त->समुच्चयः.fwrite_func;
    second->समुच्चयः.बहिर्गमनम् = दत्त->समुच्चयः.बहिर्गमनम्;
    second->समुच्चयः.fwrite_शीर्षकम् = दत्त->समुच्चयः.fwrite_शीर्षकम्;
    second->समुच्चयः.writeheader = दत्त->समुच्चयः.writeheader;
    second->समुच्चयः.fdebug = दत्त->समुच्चयः.fdebug;
    second->समुच्चयः.debugdata = दत्त->समुच्चयः.debugdata;
    second->समुच्चयः.fxferinfo = दत्त->समुच्चयः.fxferinfo;
    second->समुच्चयः.fprogress = दत्त->समुच्चयः.fprogress;
    second->समुच्चयः.प्रगतिः_client = दत्त->समुच्चयः.प्रगतिः_client;
    second->प्रगतिः.hide = दत्त->प्रगतिः.hide;
    second->प्रगतिः.callback = दत्त->प्रगतिः.callback;
  }
  प्रत्यागम second;
}

स्थिर पूर्णाङ्क समुच्चयः_transfer_url(संरचना Curl_easy *newhandle,
                            संरचना curl_pushheaders *hp,
                            संरचना Curl_easy *दत्त)
{
  ध्रुव वर्ण *v;
  CURLUcode uc;
  वर्ण *url = रिक्त;
  पूर्णाङ्क rc = 0;
  CURLU *u = curl_url();

  यदि(!u)
    प्रत्यागम 5;

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_SCHEME);
  यदि(v) {
    uc = curl_url_समुच्चयः(u, CURLUPART_SCHEME, v, 0);
    यदि(uc) {
      rc = 1;
      गच्छ fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_AUTHORITY);
  यदि(v) {
    uc = Curl_url_समुच्चयः_authority(u, v);
    यदि(uc) {
      rc = 2;
      गच्छ fail;
    }
  }

  v = curl_pushheader_byname(hp, HTTP_PSEUDO_मार्गः);
  यदि(v) {
    uc = curl_url_समुच्चयः(u, CURLUPART_मार्गः, v, 0);
    यदि(uc) {
      rc = 3;
      गच्छ fail;
    }
  }

  /* We can only allow PUSH of resource from the same origin, e.g.
   * scheme + hostname + port */
  यदि(!Curl_url_same_origin(दत्त->state.uh, u)) {
    rc = 1;
    गच्छ fail;
  }

  uc = curl_url_get(u, CURLUPART_URL, &url, 0);
  यदि(uc)
    rc = 4;
fail:
  यदि(rc) {
    curl_url_cleanup(u);
    प्रत्यागम rc;
  }

  DEBUGASSERT(!newhandle->state.uh);
  newhandle->state.uh = u;
  Curl_bufref_समुच्चयः(&newhandle->state.url, url, 0, curl_free);
  प्रत्यागम 0;
}

स्थिर शून्य discard_newhandle(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *newhandle)
{
  http2_दत्त_समाप्तम्(cf, newhandle);
  (शून्य)Curl_close(&newhandle);
}

स्थिर पूर्णाङ्क प्रक्षिप_promise(संरचना Curl_cfilter *cf,
                        संरचना Curl_easy *दत्त,
                        ध्रुव nghttp2_प्रक्षिप_promise *frame)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  पूर्णाङ्क rv; /* one of the CURL_PUSH_* defines */

  CURL_TRC_CF(दत्त, cf, "[%d] PUSH_PROMISE received",
              frame->promised_प्रवाहः_id);
  यदि(दत्त->multi->प्रक्षिप_cb) {
    संरचना h2_प्रवाहः_ctx *प्रवाहः;
    संरचना h2_प्रवाहः_ctx *newstream;
    संरचना curl_pushheaders heads;
    CURLMcode mresult;
    CURLcode परिणाम;
    /* clone the parent */
    संरचना Curl_easy *newhandle = h2_duphandle(cf, दत्त);
    यदि(!newhandle) {
      infof(दत्त, "failed to duplicate handle");
      rv = CURL_प्रक्षिप_DENY; /* FAIL HARD */
      गच्छ fail;
    }

    प्रवाहः = H2_प्रवाहः_CTX(ctx, दत्त);
    यदि(!प्रवाहः) {
      failf(दत्त, "Internal NULL stream");
      discard_newhandle(cf, newhandle);
      rv = CURL_प्रक्षिप_DENY;
      गच्छ fail;
    }

    heads.दत्त = दत्त;
    heads.प्रवाहः = प्रवाहः;
    heads.frame = frame;

    rv = समुच्चयः_transfer_url(newhandle, &heads, दत्त);
    यदि(rv) {
      CURL_TRC_CF(दत्त, cf, "[%d] PUSH_PROMISE, failed to set URL -> %d",
                  frame->promised_प्रवाहः_id, rv);
      discard_newhandle(cf, newhandle);
      rv = CURL_प्रक्षिप_DENY;
      गच्छ fail;
    }

    {
      संरचना Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, दत्त, multi_प्रक्षिप_cb);
      rv = दत्त->multi->प्रक्षिप_cb(दत्त, newhandle,
                                प्रवाहः->प्रक्षिप_शीर्षकाणि_प्रयुक्तम्, &heads,
                                दत्त->multi->प्रक्षिप_userp);
      CURL_CBAPI_अन्तः(&guard);
    }

    /* free the headers again */
    free_प्रक्षिप_शीर्षकाणि(प्रवाहः);

    यदि(rv) {
      DEBUGASSERT((rv > CURL_प्रक्षिप_OK) && (rv <= CURL_प्रक्षिप_ERROROUT));
      /* denied, kill off the new handle again */
      CURL_TRC_CF(दत्त, cf, "[%d] PUSH_PROMISE, denied by application -> %d",
                  frame->promised_प्रवाहः_id, rv);
      discard_newhandle(cf, newhandle);
      गच्छ fail;
    }

    /* approved, add to the multi handle for processing. This
     * assigns newhandle->mid. For the new `mid` we assign the
     * h2_stream instance and remember the stream_id already known. */
    mresult = Curl_multi_सङ्कलय_perform(दत्त->multi, newhandle, cf->conn);
    यदि(mresult) {
      infof(दत्त, "failed to add handle to multi");
      discard_newhandle(cf, newhandle);
      rv = CURL_प्रक्षिप_DENY;
      गच्छ fail;
    }

    परिणाम = http2_दत्त_सज्जता(cf, newhandle, &newstream);
    यदि(परिणाम) {
      failf(दत्त, "error setting up stream: %d", (पूर्णाङ्क)परिणाम);
      discard_newhandle(cf, newhandle);
      rv = CURL_प्रक्षिप_DENY;
      गच्छ fail;
    }

    DEBUGASSERT(newstream);
    newstream->id = frame->promised_प्रवाहः_id;
    newhandle->req.maxdownload = -1;
    newhandle->req.आकार = -1;

    CURL_TRC_CF(दत्त, cf, "promise easy handle added to multi, mid=%u",
                newhandle->mid);
    rv = Curl_ngh2_session_समुच्चयः_प्रवाहः_user_दत्त(
      &ctx->ngh2, newstream->id, newstream);
    यदि(rv) {
      infof(दत्त, "failed to set user_data for stream %d",
            newstream->id);
      DEBUGASSERT(0);
      discard_newhandle(cf, newhandle);
      rv = CURL_प्रक्षिप_DENY;
      गच्छ fail;
    }

    /* success, remember max stream id processed */
    यदि(newstream->id > ctx->local_max_sid)
      ctx->local_max_sid = newstream->id;
  }
  अन्यथा {
    CURL_TRC_CF(दत्त, cf, "Got PUSH_PROMISE, ignore it");
    rv = CURL_प्रक्षिप_DENY;
  }
fail:
  प्रत्यागम rv;
}

स्थिर शून्य h2_xfer_write_resp_hd(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त,
                                  संरचना h2_प्रवाहः_ctx *प्रवाहः,
                                  ध्रुव वर्ण *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  यदि(!प्रवाहः->xfer_परिणाम) {
    प्रवाहः->xfer_परिणाम = Curl_xfer_write_resp_hd(दत्त, buf, blen, eos);
    यदि(!प्रवाहः->xfer_परिणाम && !eos)
      प्रवाहः->xfer_परिणाम = cf_h2_update_local_win(cf, दत्त, प्रवाहः);
    यदि(प्रवाहः->xfer_परिणाम)
      CURL_TRC_CF(दत्त, cf, "[%d] error %d writing %zu bytes of headers",
                  प्रवाहः->id, (पूर्णाङ्क)प्रवाहः->xfer_परिणाम, blen);
  }
}

स्थिर शून्य h2_xfer_write_resp(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त,
                               संरचना h2_प्रवाहः_ctx *प्रवाहः,
                               ध्रुव वर्ण *buf, size_t blen, bool eos)
{
  /* If we already encountered an error, skip further writes */
  यदि(!प्रवाहः->xfer_परिणाम)
    प्रवाहः->xfer_परिणाम = Curl_xfer_write_resp(दत्त, buf, blen, eos);
  /* If the transfer write is errored, we do not want any more data */
  यदि(प्रवाहः->xfer_परिणाम) {
    संरचना cf_h2_ctx *ctx = cf->ctx;
    CURL_TRC_CF(दत्त, cf, "[%d] error %d writing %zu bytes of data, "
                "RST-ing stream",
                प्रवाहः->id, (पूर्णाङ्क)प्रवाहः->xfer_परिणाम, blen);
    Curl_ngh2_submit_rst_प्रवाहः(
      &ctx->ngh2, 0, प्रवाहः->id, (uint32_t)NGHTTP2_ERR_CALLBACK_FAILURE);
  }
  अन्यथा यदि(!प्रवाहः->write_paused && Curl_xfer_write_is_paused(दत्त)) {
    CURL_TRC_CF(दत्त, cf, "[%d] stream output paused", प्रवाहः->id);
    प्रवाहः->write_paused = TRUE;
  }
  अन्यथा यदि(प्रवाहः->write_paused && !Curl_xfer_write_is_paused(दत्त)) {
    CURL_TRC_CF(दत्त, cf, "[%d] stream output unpaused", प्रवाहः->id);
    प्रवाहः->write_paused = FALSE;
  }

  यदि(!प्रवाहः->xfer_परिणाम && !eos)
    प्रवाहः->xfer_परिणाम = cf_h2_update_local_win(cf, दत्त, प्रवाहः);
}

स्थिर CURLcode on_प्रवाहः_frame(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त,
                                संरचना h2_प्रवाहः_ctx *प्रवाहः,
                                ध्रुव nghttp2_frame *frame)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  int32_t प्रवाहः_id = frame->hd.प्रवाहः_id;
  पूर्णाङ्क rv;

  यदि(!प्रवाहः) {
    CURL_TRC_CF(दत्त, cf, "[%d] No stream_ctx set", प्रवाहः_id);
    प्रत्यागम CURLE_FAILED_INIT;
  }

  विकल्प(frame->hd.type) {
  पक्ष NGHTTP2_दत्त:
    CURL_TRC_CF(दत्त, cf, "[%d] DATA, window=%d/%d",
                प्रवाहः_id,
                Curl_ngh2_session_get_प्रवाहः_effective_recv_दत्त_दीर्घता(
                  &ctx->ngh2, प्रवाहः->id),
                Curl_ngh2_session_get_प्रवाहः_effective_local_गवाक्षः_आकार(
                  &ctx->ngh2, प्रवाहः->id));
    /* If !body started on this stream, then receiving DATA is illegal. */
    यदि(!प्रवाहः->bodystarted) {
      यदि(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_प्रवाहः(&ctx->ngh2, NGHTTP2_FLAG_NONE,
                                       प्रवाहः_id, NGHTTP2_PROTOCOL_ERROR))) {
        प्रत्यागम CURLE_RECV_ERROR;
      }
    }
    भङ्ग;
  पक्ष NGHTTP2_शीर्षकाणि:
    यदि(प्रवाहः->bodystarted) {
      /* Only valid HEADERS after body started is trailer HEADERS. We
         buffer them in on_header callback. */
      भङ्ग;
    }

    /* nghttp2 guarantees that :status is received, and we store it to
       stream->status_code. Fuzzing has proven this can still be reached
       without status code having been set. */
    यदि(प्रवाहः->स्थितिः_संहिता == -1)
      प्रत्यागम CURLE_RECV_ERROR;

    /* Only final status code signals the end of header */
    यदि(प्रवाहः->स्थितिः_संहिता / 100 != 1) {
      प्रवाहः->resp_hds_complete = TRUE;
      प्रवाहः->bodystarted = TRUE;
    }
    अन्यथा
      प्रवाहः->स्थितिः_संहिता = -1;

    h2_xfer_write_resp_hd(cf, दत्त, प्रवाहः, STRCONST("\r\n"),
                          (bool)प्रवाहः->closed);

    Curl_multi_mark_dirty(दत्त);
    भङ्ग;
  पक्ष NGHTTP2_प्रक्षिप_PROMISE:
    rv = प्रक्षिप_promise(cf, दत्त, &frame->प्रक्षिप_promise);
    यदि(rv) { /* deny! */
      DEBUGASSERT((rv > CURL_प्रक्षिप_OK) && (rv <= CURL_प्रक्षिप_ERROROUT));
      यदि(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_submit_rst_प्रवाहः(&ctx->ngh2, NGHTTP2_FLAG_NONE,
             frame->प्रक्षिप_promise.promised_प्रवाहः_id, NGHTTP2_CANCEL)))
        प्रत्यागम CURLE_SEND_ERROR;
      अन्यथा यदि(rv == CURL_प्रक्षिप_ERROROUT) {
        /* fail the parent transfer, not the whole session */
        CURL_TRC_CF(दत्त, cf, "[%d] fail in PUSH_PROMISE received",
                    प्रवाहः_id);
        यदि(!प्रवाहः->xfer_परिणाम)
          प्रवाहः->xfer_परिणाम = CURLE_RECV_ERROR;
        Curl_multi_mark_dirty(दत्त);
      }
    }
    भङ्ग;
  पक्ष NGHTTP2_RST_प्रवाहः:
    यदि(frame->rst_प्रवाहः.error_संहिता)
      प्रवाहः->reset_by_server = TRUE;
    Curl_multi_mark_dirty(दत्त);
    भङ्ग;
  पक्ष NGHTTP2_गवाक्षः_UPDATE:
    यदि(CURL_REQ_WANT_SEND(दत्त) && Curl_bufq_is_रिक्तानि(&प्रवाहः->sendbuf)) {
      /* need more data, force processing of transfer */
      Curl_multi_mark_dirty(दत्त);
    }
    अन्यथा यदि(!Curl_bufq_is_रिक्तानि(&प्रवाहः->sendbuf)) {
      /* resume the potentially suspended stream */
      यदि(Curl_ngh2_is_fatal(&ctx->ngh2,
           Curl_ngh2_session_resume_दत्त(&ctx->ngh2, प्रवाहः->id)))
        प्रत्यागम CURLE_SEND_ERROR;
    }
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }

  यदि(frame->hd.flags & NGHTTP2_FLAG_अन्तः_प्रवाहः) {
    यदि(!प्रवाहः->closed && !प्रवाहः->body_eos &&
       ((प्रवाहः->स्थितिः_संहिता >= 400) || (प्रवाहः->स्थितिः_संहिता < 200))) {
      /* The server did not give us a positive response and we are not
       * done uploading the request body. We need to stop doing that and
       * also inform the server that we aborted our side. */
      CURL_TRC_CF(दत्त, cf, "[%d] EOS frame with unfinished upload and "
                  "HTTP status %d, abort upload by RST",
                  प्रवाहः_id, प्रवाहः->स्थितिः_संहिता);
      Curl_ngh2_submit_rst_प्रवाहः(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, प्रवाहः->id, NGHTTP2_प्रवाहः_CLOSED);
      प्रवाहः->closed = TRUE;
    }
    Curl_multi_mark_dirty(दत्त);
  }
  प्रत्यागम CURLE_OK;
}

#यदि_परिभाषितम् CURLVERBOSE
पूर्णाङ्क Curl_nghttp2_fr_print(ध्रुव nghttp2_frame *frame, वर्ण *बफर,
                          size_t blen)
{
  विकल्प(frame->hd.type) {
  पक्ष NGHTTP2_दत्त: {
    प्रत्यागम curl_msnprintf(बफर, blen,
                          "FRAME[DATA, len=%d, eos=%d, padlen=%d]",
                          (पूर्णाङ्क)frame->hd.दीर्घता,
                          !!(frame->hd.flags & NGHTTP2_FLAG_अन्तः_प्रवाहः),
                          (पूर्णाङ्क)frame->दत्त.padlen);
  }
  पक्ष NGHTTP2_शीर्षकाणि: {
    प्रत्यागम curl_msnprintf(बफर, blen,
                          "FRAME[HEADERS, len=%d, hend=%d, eos=%d]",
                          (पूर्णाङ्क)frame->hd.दीर्घता,
                          !!(frame->hd.flags & NGHTTP2_FLAG_अन्तः_शीर्षकाणि),
                          !!(frame->hd.flags & NGHTTP2_FLAG_अन्तः_प्रवाहः));
  }
  पक्ष NGHTTP2_प्राथमिकता: {
    प्रत्यागम curl_msnprintf(बफर, blen,
                          "FRAME[PRIORITY, len=%d, flags=%d]",
                          (पूर्णाङ्क)frame->hd.दीर्घता, frame->hd.flags);
  }
  पक्ष NGHTTP2_RST_प्रवाहः: {
    प्रत्यागम curl_msnprintf(बफर, blen,
                          "FRAME[RST_STREAM, len=%d, flags=%d, error=%u]",
                          (पूर्णाङ्क)frame->hd.दीर्घता, frame->hd.flags,
                          frame->rst_प्रवाहः.error_संहिता);
  }
  पक्ष NGHTTP2_SETTINGS: {
    यदि(frame->hd.flags & NGHTTP2_FLAG_ACK) {
      प्रत्यागम curl_msnprintf(बफर, blen, "FRAME[SETTINGS, ack=1]");
    }
    प्रत्यागम curl_msnprintf(बफर, blen,
                          "FRAME[SETTINGS, len=%d]", (पूर्णाङ्क)frame->hd.दीर्घता);
  }
  पक्ष NGHTTP2_प्रक्षिप_PROMISE:
    प्रत्यागम curl_msnprintf(बफर, blen,
                          "FRAME[PUSH_PROMISE, len=%d, hend=%d]",
                          (पूर्णाङ्क)frame->hd.दीर्घता,
                          !!(frame->hd.flags & NGHTTP2_FLAG_अन्तः_शीर्षकाणि));
  पक्ष NGHTTP2_PING:
    प्रत्यागम curl_msnprintf(बफर, blen,
                          "FRAME[PING, len=%d, ack=%d]",
                          (पूर्णाङ्क)frame->hd.दीर्घता,
                          frame->hd.flags & NGHTTP2_FLAG_ACK);
  पक्ष NGHTTP2_GOAWAY: {
    वर्ण scratch[128];
    size_t s_len = CURL_ARRAYSIZE(scratch);
    size_t len = (frame->goaway.opaque_दत्त_len < s_len) ?
      frame->goaway.opaque_दत्त_len : s_len - 1;
    यदि(len)
      स्मृतिं_प्रतिलिख(scratch, frame->goaway.opaque_दत्त, len);
    scratch[len] = '\0';
    प्रत्यागम curl_msnprintf(बफर, blen,
                          "FRAME[GOAWAY, error=%u, reason='%s', "
                          "last_stream=%d]", frame->goaway.error_संहिता,
                          scratch, frame->goaway.चरमः_प्रवाहः_id);
  }
  पक्ष NGHTTP2_गवाक्षः_UPDATE: {
    प्रत्यागम curl_msnprintf(बफर, blen,
                          "FRAME[WINDOW_UPDATE, incr=%d]",
                          frame->गवाक्षः_update.गवाक्षः_आकार_increment);
  }
  स्वभाव:
    प्रत्यागम curl_msnprintf(बफर, blen, "FRAME[%d, len=%d, flags=%d]",
                          frame->hd.type, (पूर्णाङ्क)frame->hd.दीर्घता,
                          frame->hd.flags);
  }
}

स्थिर पूर्णाङ्क on_frame_send(nghttp2_session *session, ध्रुव nghttp2_frame *frame,
                         शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);

  (शून्य)session;
  DEBUGASSERT(दत्त);
  यदि(Curl_trc_cf_is_verbose(cf, दत्त)) {
    वर्ण बफर[256];
    पूर्णाङ्क len;
    len = Curl_nghttp2_fr_print(frame, बफर, परिमाण(बफर) - 1);
    बफर[len] = 0;
    CURL_TRC_CF(दत्त, cf, "[%d] -> %s", frame->hd.प्रवाहः_id, बफर);
  }
  यदि((frame->hd.type == NGHTTP2_GOAWAY) && !ctx->प्रेषितम्_goaway) {
    /* A GOAWAY not initiated by us, but by nghttp2 itself on detecting
     * a protocol error on the connection */
    failf(दत्त, "nghttp2 shuts down connection with error %u: %s",
          frame->goaway.error_संहिता,
          nghttp2_http2_strerror(frame->goaway.error_संहिता));
  }
  प्रत्यागम 0;
}
#यदि_अन्तः /* CURLVERBOSE */

स्थिर पूर्णाङ्क on_frame_recv(nghttp2_session *session, ध्रुव nghttp2_frame *frame,
                         शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf), *दत्त_s;
  संरचना h2_प्रवाहः_ctx *प्रवाहः;
  int32_t प्रवाहः_id = frame->hd.प्रवाहः_id;

  (शून्य)session;
  DEBUGASSERT(दत्त);
#यदि_परिभाषितम् CURLVERBOSE
  यदि(Curl_trc_cf_is_verbose(cf, दत्त)) {
    वर्ण बफर[256];
    पूर्णाङ्क len;
    len = Curl_nghttp2_fr_print(frame, बफर, परिमाण(बफर) - 1);
    बफर[len] = 0;
    CURL_TRC_CF(दत्त, cf, "[%d] <- %s", frame->hd.प्रवाहः_id, बफर);
  }
#यदि_अन्तः /* CURLVERBOSE */

  यदि(!प्रवाहः_id) {
    /* stream ID zero is for connection-oriented stuff */
    DEBUGASSERT(दत्त);
    विकल्प(frame->hd.type) {
    पक्ष NGHTTP2_SETTINGS: {
      यदि(!(frame->hd.flags & NGHTTP2_FLAG_ACK)) {
        uint32_t max_conn = ctx->max_concurrent_streams;
        ctx->max_concurrent_streams =
          Curl_ngh2_session_get_remote_settings(
            &ctx->ngh2, NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS);
        ctx->enable_प्रक्षिप = Curl_ngh2_session_get_remote_settings(
          &ctx->ngh2, NGHTTP2_SETTINGS_ENABLE_प्रक्षिप) != 0;
        CURL_TRC_CF(दत्त, cf, "[0] MAX_CONCURRENT_STREAMS: %u",
                    ctx->max_concurrent_streams);
        CURL_TRC_CF(दत्त, cf, "[0] ENABLE_PUSH: %s",
                    ctx->enable_प्रक्षिप ? "TRUE" : "false");
        यदि(दत्त && max_conn != ctx->max_concurrent_streams) {
          /* only signal change if the value actually changed */
          CURL_TRC_CF(दत्त, cf, "[0] notify MAX_CONCURRENT_STREAMS: %u",
                      ctx->max_concurrent_streams);
          Curl_multi_connchanged(दत्त->multi);
        }
        /* Since the initial stream window is 64K, a request might be on HOLD,
         * due to exhaustion. The (initial) SETTINGS may announce a much larger
         * window and *assume* that we treat this like a WINDOW_UPDATE. Some
         * servers send an explicit WINDOW_UPDATE, but not all seem to do that.
         * To be safe, we UNHOLD a stream in order not to stall. */
        यदि(CURL_REQ_WANT_SEND(दत्त))
          Curl_multi_mark_dirty(दत्त);
      }
      भङ्ग;
    }
    पक्ष NGHTTP2_GOAWAY:
      ctx->rcvd_goaway = TRUE;
      ctx->goaway_error = frame->goaway.error_संहिता;
      ctx->remote_max_sid = frame->goaway.चरमः_प्रवाहः_id;
      यदि(दत्त) {
        infof(दत्त, "received GOAWAY, error=%u, last_stream=%d",
              ctx->goaway_error, ctx->remote_max_sid);
        Curl_multi_connchanged(दत्त->multi);
      }
      भङ्ग;
    स्वभाव:
      भङ्ग;
    }
    प्रत्यागम 0;
  }

  दत्त_s = cf_h2_get_प्रवाहः_xfer(cf, प्रवाहः_id, &प्रवाहः);
  यदि(!दत्त_s) {
    CURL_TRC_CF(दत्त, cf, "[%d] No Curl_easy associated", प्रवाहः_id);
    प्रत्यागम 0;
  }

  प्रत्यागम on_प्रवाहः_frame(cf, दत्त_s, प्रवाहः, frame) ?
         NGHTTP2_ERR_CALLBACK_FAILURE : 0;
}

स्थिर पूर्णाङ्क cf_h2_on_invalid_frame_recv(nghttp2_session *session,
                                       ध्रुव nghttp2_frame *frame,
                                       पूर्णाङ्क ngerr, शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना Curl_easy *दत्त;
  संरचना h2_प्रवाहः_ctx *प्रवाहः;
  int32_t प्रवाहः_id = frame->hd.प्रवाहः_id;

  (शून्य)session;
  दत्त = cf_h2_get_प्रवाहः_xfer(cf, प्रवाहः_id, &प्रवाहः);
  यदि(दत्त) {
#यदि_परिभाषितम् CURLVERBOSE
    वर्ण बफर[256];
    पूर्णाङ्क len;
    len = Curl_nghttp2_fr_print(frame, बफर, परिमाण(बफर) - 1);
    बफर[len] = 0;
    failf(दत्त, "[HTTP2] [%d] received invalid frame: %s, error %d: %s",
          प्रवाहः_id, बफर, ngerr, nghttp2_strerror(ngerr));
#यदि_अन्तः /* CURLVERBOSE */
    यदि(प्रवाहः) {
      Curl_ngh2_submit_rst_प्रवाहः(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, प्रवाहः->id, NGHTTP2_प्रवाहः_CLOSED);
      प्रवाहः->error = ngerr;
      प्रवाहः->closed = TRUE;
      प्रवाहः->reset = TRUE;
    }
  }
  /* Return no error, nghttp2 will RST/GOAWAY by itself when needed */
  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क on_दत्त_chunk_recv(nghttp2_session *session, uint8_t flags,
                              int32_t प्रवाहः_id,
                              ध्रुव uint8_t *mem, size_t len, शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना h2_प्रवाहः_ctx *प्रवाहः;
  संरचना Curl_easy *दत्त_s;

  (शून्य)session;
  (शून्य)flags;

  DEBUGASSERT(प्रवाहः_id); /* should never be a zero stream ID here */
  DEBUGASSERT(CF_दत्त_CURRENT(cf));

  /* get the stream from the hash based on Stream ID */
  दत्त_s = cf_h2_get_प्रवाहः_xfer(cf, प्रवाहः_id, &प्रवाहः);
  यदि(!दत्त_s) {
    /* Receiving a Stream ID not in the hash should not happen - unless
       we have aborted a transfer artificially and there were more data
       in the pipeline. Silently ignore. */
    CURL_TRC_CF(CF_दत्त_CURRENT(cf), cf, "[%d] Data for unknown", प्रवाहः_id);
    /* consumed explicitly as no one will read it */
    Curl_ngh2_session_consume(&ctx->ngh2, प्रवाहः_id, len);
    प्रत्यागम 0;
  }

  यदि(!प्रवाहः)
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;

  h2_xfer_write_resp(cf, दत्त_s, प्रवाहः, (ध्रुव वर्ण *)mem, len, FALSE);

  Curl_ngh2_session_consume(&ctx->ngh2, प्रवाहः_id, len);
  प्रवाहः->nrcvd_दत्त += (curl_off_t)len;
  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क on_प्रवाहः_close(nghttp2_session *session, int32_t प्रवाहः_id,
                           uint32_t error_संहिता, शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना Curl_easy *दत्त_s, *call_दत्त = CF_दत्त_CURRENT(cf);
  संरचना h2_प्रवाहः_ctx *प्रवाहः = रिक्त;
  पूर्णाङ्क rv;
  (शून्य)session;

  DEBUGASSERT(call_दत्त);
  /* stream id 0 is the connection, do not look there for streams. */
  दत्त_s = प्रवाहः_id ? cf_h2_get_प्रवाहः_xfer(cf, प्रवाहः_id, &प्रवाहः) : रिक्त;
  यदि(!दत्त_s) {
    CURL_TRC_CF(call_दत्त, cf,
                "[%d] on_stream_close, no easy set on stream", प्रवाहः_id);
    प्रत्यागम 0;
  }
  यदि(!GOOD_EASY_HANDLE(दत्त_s)) {
    /* nghttp2 still has an easy registered for the stream which has
     * been freed be libcurl. This points to a code path that does not
     * trigger DONE or DETACH events as it must. */
    CURL_TRC_CF(call_दत्त, cf,
                "[%d] on_stream_close, not a GOOD easy on stream", प्रवाहः_id);
    (शून्य)Curl_ngh2_session_समुच्चयः_प्रवाहः_user_दत्त(
      &ctx->ngh2, प्रवाहः_id, रिक्त);
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  यदि(!प्रवाहः) {
    CURL_TRC_CF(दत्त_s, cf,
                "[%d] on_stream_close, GOOD easy but no stream", प्रवाहः_id);
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  प्रवाहः->closed = TRUE;
  प्रवाहः->error = error_संहिता;
  यदि(प्रवाहः->error) {
    प्रवाहः->reset = TRUE;
    यदि((प्रवाहः->error == NGHTTP2_REFUSED_प्रवाहः) &&
       ctx->rcvd_goaway && ctx->goaway_error)
    /* REFUSED_STREAM is used by nghttp2 on a GOAWAY from the
     * server where it indicated it will no longer process this stream.
     * If the GOAWAY carried an error code, take that as the error for
     * the stream. */
    प्रवाहः->error = ctx->goaway_error;
  }

  यदि(प्रवाहः->error)
    CURL_TRC_CF(दत्त_s, cf, "[%d] RESET: %s (err %u)",
              प्रवाहः_id, nghttp2_http2_strerror(error_संहिता), error_संहिता);
  अन्यथा
    CURL_TRC_CF(दत्त_s, cf, "[%d] CLOSED", प्रवाहः_id);
  Curl_multi_mark_dirty(दत्त_s);

  /* remove `stream_ctx` from the nghttp2 stream */
  rv = Curl_ngh2_session_समुच्चयः_प्रवाहः_user_दत्त(
    &ctx->ngh2, प्रवाहः_id, रिक्त);
  यदि(rv) {
    infof(दत्त_s, "http/2: failed to clear user_data for stream %d",
          प्रवाहः_id);
    DEBUGASSERT(0);
  }
  प्रत्यागम 0;
}

स्थिर शून्य cf_h2_शीर्षकम्_error(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त,
                               संरचना h2_प्रवाहः_ctx *प्रवाहः,
                               CURLcode परिणाम)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;

  failf(दत्त, "Error receiving HTTP2 header: %d(%s)", (पूर्णाङ्क)परिणाम,
        curl_easy_strerror(परिणाम));
  यदि(प्रवाहः) {
    Curl_ngh2_submit_rst_प्रवाहः(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, प्रवाहः->id, NGHTTP2_प्रवाहः_CLOSED);
    प्रवाहः->closed = TRUE;
    प्रवाहः->reset = TRUE;
  }
}

/* frame->hd.type is either NGHTTP2_HEADERS or NGHTTP2_PUSH_PROMISE */
स्थिर पूर्णाङ्क on_शीर्षकम्(nghttp2_session *session, ध्रुव nghttp2_frame *frame,
                     ध्रुव uint8_t *name, size_t namelen,
                     ध्रुव uint8_t *मानम्, size_t valuelen,
                     uint8_t flags,
                     शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना h2_प्रवाहः_ctx *प्रवाहः;
  संरचना Curl_easy *दत्त;
  int32_t प्रवाहः_id = frame->hd.प्रवाहः_id;
  CURLcode परिणाम;

  (शून्य)session;
  (शून्य)flags;

  DEBUGASSERT(प्रवाहः_id); /* should never be a zero stream ID here */

  /* get the stream from the hash based on Stream ID */
  दत्त = cf_h2_get_प्रवाहः_xfer(cf, प्रवाहः_id, &प्रवाहः);
  यदि(!GOOD_EASY_HANDLE(दत्त))
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;

  यदि(!प्रवाहः) {
    failf(दत्त, "Internal NULL stream");
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
  }

  /* Store received PUSH_PROMISE headers to be used when the subsequent
     PUSH_PROMISE callback comes */
  यदि(frame->hd.type == NGHTTP2_प्रक्षिप_PROMISE) {
    वर्ण *h;

    यदि((namelen == CURL_CSTRLEN(HTTP_PSEUDO_AUTHORITY)) &&
       !वाक्ये_तुलय(HTTP_PSEUDO_AUTHORITY, (ध्रुव वर्ण *)name, namelen)) {
      /* pseudo headers are lower case */
      पूर्णाङ्क rc = 0;
      वर्ण *check = curl_maprintf("%s:%d", दत्त->state.origin->hostname,
                                  दत्त->state.origin->पत्तनम्);
      यदि(!check)
        /* no memory */
        प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
      यदि(!curl_strequal(check, (ध्रुव वर्ण *)मानम्) &&
         ((दत्त->state.origin->पत्तनम् != दत्त->state.origin->scheme->defport) ||
          !curl_strequal(दत्त->state.origin->hostname, (ध्रुव वर्ण *)मानम्))) {
        /* This is push is not for the same authority that was asked for in
         * the URL. RFC 7540 section 8.2 says: "A client MUST treat a
         * PUSH_PROMISE for which the server is not authoritative as a stream
         * error of type PROTOCOL_ERROR."
         */
        (शून्य)Curl_ngh2_submit_rst_प्रवाहः(
          &ctx->ngh2, NGHTTP2_FLAG_NONE, प्रवाहः_id, NGHTTP2_PROTOCOL_ERROR);
        rc = NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      curlx_free(check);
      यदि(rc)
        प्रत्यागम rc;
    }

    यदि(!प्रवाहः->प्रक्षिप_शीर्षकाणि) {
      प्रवाहः->प्रक्षिप_शीर्षकाणि_alloc = 10;
      प्रवाहः->प्रक्षिप_शीर्षकाणि = curlx_malloc(प्रवाहः->प्रक्षिप_शीर्षकाणि_alloc *
                                          परिमाण(वर्ण *));
      यदि(!प्रवाहः->प्रक्षिप_शीर्षकाणि)
        प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
      प्रवाहः->प्रक्षिप_शीर्षकाणि_प्रयुक्तम् = 0;
    }
    अन्यथा यदि(प्रवाहः->प्रक्षिप_शीर्षकाणि_प्रयुक्तम् ==
            प्रवाहः->प्रक्षिप_शीर्षकाणि_alloc) {
      वर्ण **headp;
      यदि(प्रवाहः->प्रक्षिप_शीर्षकाणि_alloc > 1000) {
        /* this is beyond crazy many headers, bail out */
        failf(दत्त, "Too many PUSH_PROMISE headers");
        free_प्रक्षिप_शीर्षकाणि(प्रवाहः);
        प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      प्रवाहः->प्रक्षिप_शीर्षकाणि_alloc *= 2;
      headp = curlx_realloc(प्रवाहः->प्रक्षिप_शीर्षकाणि,
                            प्रवाहः->प्रक्षिप_शीर्षकाणि_alloc * परिमाण(वर्ण *));
      यदि(!headp) {
        free_प्रक्षिप_शीर्षकाणि(प्रवाहः);
        प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      प्रवाहः->प्रक्षिप_शीर्षकाणि = headp;
    }
    h = curl_maprintf("%s:%s", name, मानम्);
    यदि(!h) {
      free_प्रक्षिप_शीर्षकाणि(प्रवाहः);
      प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    प्रवाहः->प्रक्षिप_शीर्षकाणि[प्रवाहः->प्रक्षिप_शीर्षकाणि_प्रयुक्तम्++] = h;
    प्रत्यागम 0;
  }

  यदि(प्रवाहः->bodystarted) {
    /* This is a trailer */
    CURL_TRC_CF(दत्त, cf, "[%d] trailer: %.*s: %.*s",
                प्रवाहः->id, (पूर्णाङ्क)namelen, name, (पूर्णाङ्क)valuelen, मानम्);
    परिणाम = Curl_dynhds_सङ्कलय(&प्रवाहः->resp_trailers,
                             (ध्रुव वर्ण *)name, namelen,
                             (ध्रुव वर्ण *)मानम्, valuelen);
    यदि(परिणाम) {
      cf_h2_शीर्षकम्_error(cf, दत्त, प्रवाहः, परिणाम);
      प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
    }

    प्रत्यागम 0;
  }

  यदि(namelen == CURL_CSTRLEN(HTTP_PSEUDO_स्थितिः) &&
     !memcmp(HTTP_PSEUDO_स्थितिः, name, namelen)) {
    /* nghttp2 guarantees :status is received first and only once. */
    वर्ण बफर[32];
    size_t hlen;
    परिणाम = Curl_http_decode_स्थितिः(&प्रवाहः->स्थितिः_संहिता,
                                     (ध्रुव वर्ण *)मानम्, valuelen);
    यदि(परिणाम) {
      cf_h2_शीर्षकम्_error(cf, दत्त, प्रवाहः, परिणाम);
      प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    hlen = curl_msnprintf(बफर, परिमाण(बफर), HTTP_PSEUDO_स्थितिः ":%d\r",
                          प्रवाहः->स्थितिः_संहिता);
    परिणाम = Curl_शीर्षकाणि_प्रक्षिप(दत्त, बफर, hlen, CURLH_PSEUDO);
    यदि(परिणाम) {
      cf_h2_शीर्षकम्_error(cf, दत्त, प्रवाहः, परिणाम);
      प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    curlx_dyn_reset(&ctx->scratch);
    परिणाम = curlx_dyn_addn(&ctx->scratch, STRCONST("HTTP/2 "));
    यदि(!परिणाम)
      परिणाम = curlx_dyn_addn(&ctx->scratch, मानम्, valuelen);
    यदि(!परिणाम)
      परिणाम = curlx_dyn_addn(&ctx->scratch, STRCONST(" \r\n"));
    यदि(!परिणाम)
      h2_xfer_write_resp_hd(cf, दत्त, प्रवाहः, curlx_dyn_ptr(&ctx->scratch),
                            curlx_dyn_len(&ctx->scratch), FALSE);
    यदि(परिणाम) {
      cf_h2_शीर्षकम्_error(cf, दत्त, प्रवाहः, परिणाम);
      प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
    }
    /* if we receive data for another handle, wake that up */
    यदि(CF_दत्त_CURRENT(cf) != दत्त)
      Curl_multi_mark_dirty(दत्त);

    CURL_TRC_CF(दत्त, cf, "[%d] status: HTTP/2 %03d",
                प्रवाहः->id, प्रवाहः->स्थितिः_संहिता);
    प्रत्यागम 0;
  }

  /* nghttp2 guarantees that namelen > 0, and :status was already
     received, and this is not pseudo-header field . */
  /* convert to an HTTP1-style header */
  curlx_dyn_reset(&ctx->scratch);
  परिणाम = curlx_dyn_addn(&ctx->scratch, (ध्रुव वर्ण *)name, namelen);
  यदि(!परिणाम)
    परिणाम = curlx_dyn_addn(&ctx->scratch, STRCONST(": "));
  यदि(!परिणाम)
    परिणाम = curlx_dyn_addn(&ctx->scratch, (ध्रुव वर्ण *)मानम्, valuelen);
  यदि(!परिणाम)
    परिणाम = curlx_dyn_addn(&ctx->scratch, STRCONST("\r\n"));
  यदि(!परिणाम)
    h2_xfer_write_resp_hd(cf, दत्त, प्रवाहः, curlx_dyn_ptr(&ctx->scratch),
                          curlx_dyn_len(&ctx->scratch), FALSE);
  यदि(परिणाम) {
    cf_h2_शीर्षकम्_error(cf, दत्त, प्रवाहः, परिणाम);
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
  }
  /* if we receive data for another handle, wake that up */
  यदि(CF_दत्त_CURRENT(cf) != दत्त)
    Curl_multi_mark_dirty(दत्त);

  CURL_TRC_CF(दत्त, cf, "[%d] header: %.*s: %.*s",
              प्रवाहः->id, (पूर्णाङ्क)namelen, name, (पूर्णाङ्क)valuelen, मानम्);

  प्रत्यागम 0; /* 0 is successful */
}

स्थिर ssize_t req_body_read_callback(nghttp2_session *session,
                                      int32_t प्रवाहः_id,
                                      uint8_t *buf, size_t दीर्घता,
                                      uint32_t *दत्त_flags,
                                      nghttp2_दत्त_source *source,
                                      शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना Curl_easy *दत्त_s;
  संरचना h2_प्रवाहः_ctx *प्रवाहः = रिक्त;
  CURLcode परिणाम;
  ssize_t nread;
  size_t n;
  (शून्य)source;

  (शून्य)session;
  यदि(!प्रवाहः_id)
    प्रत्यागम NGHTTP2_ERR_INVALID_तर्कः;

  /* get the stream from the hash based on Stream ID, stream ID zero is for
     connection-oriented stuff */
  दत्त_s = cf_h2_get_प्रवाहः_xfer(cf, प्रवाहः_id, &प्रवाहः);
  यदि(!दत्त_s || !प्रवाहः)
    /* Receiving a Stream ID not in the hash should not happen, this is an
       internal error more than anything else! */
    प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;

  परिणाम = Curl_bufq_read(&प्रवाहः->sendbuf, buf, दीर्घता, &n);
  यदि(परिणाम) {
    यदि(परिणाम != CURLE_AGAIN)
      प्रत्यागम NGHTTP2_ERR_CALLBACK_FAILURE;
    nread = 0;
  }
  अन्यथा
    nread = (ssize_t)n;

  CURL_TRC_CF(दत्त_s, cf, "[%d] req_body_read(len=%zu) eos=%d -> %zd, %d",
              प्रवाहः_id, दीर्घता, प्रवाहः->body_eos, nread, (पूर्णाङ्क)परिणाम);

  यदि(प्रवाहः->body_eos && Curl_bufq_is_रिक्तानि(&प्रवाहः->sendbuf)) {
    *दत्त_flags = NGHTTP2_दत्त_FLAG_EOF;
    प्रत्यागम nread;
  }
  प्रत्यागम (nread == 0) ? NGHTTP2_ERR_DEFERRED : nread;
}

#यदि_परिभाषितम् CURLVERBOSE
स्थिर पूर्णाङ्क error_callback(nghttp2_session *session,
                          ध्रुव वर्ण *सन्देशः,
                          size_t len,
                          शून्य *userp)
{
  संरचना Curl_cfilter *cf = userp;
  संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);
  (शून्य)session;
  failf(दत्त, "%.*s", (पूर्णाङ्क)len, सन्देशः);
  प्रत्यागम 0;
}
#यदि_अन्तः

/*
 * Append headers to ask for an HTTP1.1 to HTTP2 upgrade.
 */
CURLcode Curl_http2_request_upgrade(संरचना dynbuf *req,
                                    संरचना Curl_easy *दत्त)
{
  CURLcode परिणाम;
  वर्ण *base64;
  size_t blen;
  संरचना SingleRequest *k = &दत्त->req;
  uint8_t binsettings[H2_BINSETTINGS_LEN];
  ssize_t rc;
  size_t binlen; /* length of the binsettings data */

  rc = populate_binsettings(binsettings, दत्त);
  यदि(!curlx_sztouz(rc, &binlen) || !binlen) {
    failf(दत्त, "nghttp2 unexpectedly failed on pack_settings_payload");
    curlx_dyn_free(req);
    प्रत्यागम CURLE_FAILED_INIT;
  }

  परिणाम = curlx_base64url_encode(binsettings, binlen, &base64, &blen);
  यदि(परिणाम) {
    curlx_dyn_free(req);
    प्रत्यागम परिणाम;
  }

  दत्त->state.http_hd_upgrade = TRUE;
  दत्त->state.http_hd_h2_settings = TRUE;
  परिणाम = curlx_dyn_addf(req,
                          "Upgrade: %s\r\n"
                          "HTTP2-Settings: %s\r\n",
                          NGHTTP2_CLEARTEXT_PROTO_संस्करणम्_ID, base64);
  curlx_free(base64);

  k->upgr101 = UPGR101_H2;
  दत्त->conn->द्वयङ्काः.upgrade_अन्तर्_प्रगतिः = TRUE;

  प्रत्यागम परिणाम;
}

स्थिर CURLcode http2_handle_प्रवाहः_close(संरचना Curl_cfilter *cf,
                                          संरचना Curl_easy *दत्त,
                                          संरचना h2_प्रवाहः_ctx *प्रवाहः,
                                          size_t *pnlen)
{
  CURLcode परिणाम;

  *pnlen = 0;
  यदि(प्रवाहः->reset) {
    यदि(प्रवाहः->error == NGHTTP2_REFUSED_प्रवाहः) {
      infof(दत्त, "HTTP/2 stream %d refused by server, try again on a new "
                  "connection", प्रवाहः->id);
      connclose(cf->conn); /* do not use this anymore */
      दत्त->state.refused_प्रवाहः = TRUE;
      प्रत्यागम CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    अन्यथा यदि(प्रवाहः->resp_hds_complete && दत्त->req.no_body) {
      CURL_TRC_CF(दत्त, cf, "[%d] error after response headers, but we did "
                  "not want a body anyway, ignore: %s (err %u)",
                  प्रवाहः->id, nghttp2_http2_strerror(प्रवाहः->error),
                  प्रवाहः->error);
      प्रवाहः->close_handled = TRUE;
      प्रत्यागम CURLE_OK;
    }
    failf(दत्त, "HTTP/2 stream %d reset by %s (error 0x%x %s)",
          प्रवाहः->id, प्रवाहः->reset_by_server ? "server" : "curl",
          प्रवाहः->error, nghttp2_http2_strerror(प्रवाहः->error));
    प्रत्यागम प्रवाहः->error ? CURLE_HTTP2_प्रवाहः :
           (दत्त->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2);
  }
  अन्यथा यदि(!प्रवाहः->bodystarted) {
    failf(दत्त, "HTTP/2 stream %d was closed cleanly, but before getting "
          "all response header fields, treated as error", प्रवाहः->id);
    प्रत्यागम CURLE_HTTP2_प्रवाहः;
  }

  यदि(Curl_dynhds_गणनम्(&प्रवाहः->resp_trailers)) {
    संरचना dynhds_प्रविष्टिः *e;
    संरचना dynbuf dbuf;
    size_t i;

    परिणाम = CURLE_OK;
    curlx_dyn_init(&dbuf, DYN_TRAILERS);
    प्रति(i = 0; i < Curl_dynhds_गणनम्(&प्रवाहः->resp_trailers); ++i) {
      e = Curl_dynhds_getn(&प्रवाहः->resp_trailers, i);
      यदि(!e)
        भङ्ग;
      curlx_dyn_reset(&dbuf);
      परिणाम = curlx_dyn_addf(&dbuf, "%.*s: %.*s\x0d\x0a",
                              (पूर्णाङ्क)e->namelen, e->name,
                              (पूर्णाङ्क)e->valuelen, e->मानम्);
      यदि(परिणाम)
        भङ्ग;
      Curl_debug(दत्त, CURLINFO_शीर्षकम्_अन्तर्, curlx_dyn_ptr(&dbuf),
                 curlx_dyn_len(&dbuf));
      परिणाम = Curl_client_write(दत्त,
                                 CLIENTWRITE_शीर्षकम् | CLIENTWRITE_TRAILER,
                                 curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf));
      यदि(परिणाम)
        भङ्ग;
      परिणाम = Curl_bump_headersize(दत्त, curlx_dyn_len(&dbuf), FALSE);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
    curlx_dyn_free(&dbuf);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  प्रवाहः->close_handled = TRUE;
  परिणाम = CURLE_OK;

बहिर्गमनम्:
  CURL_TRC_CF(दत्त, cf, "handle_stream_close -> %d, %zu", (पूर्णाङ्क)परिणाम, *pnlen);
  प्रत्यागम परिणाम;
}

स्थिर पूर्णाङ्क sweight_wanted(ध्रुव संरचना Curl_easy *दत्त)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  प्रत्यागम दत्त->समुच्चयः.weight ?
    दत्त->समुच्चयः.weight : NGHTTP2_DEFAULT_WEIGHT;
}

स्थिर पूर्णाङ्क sweight_अन्तर्_effect(ध्रुव संरचना Curl_easy *दत्त)
{
  /* 0 weight is not set by user and we take the nghttp2 default one */
  प्रत्यागम दत्त->state.weight ?
    दत्त->state.weight : NGHTTP2_DEFAULT_WEIGHT;
}

/*
 * h2_pri_spec() fills in the pri_spec struct, used by nghttp2 to send weight
 * and dependency to the peer. It also stores the updated values in the state
 * struct.
 */

स्थिर शून्य h2_pri_spec(संरचना Curl_easy *दत्त,
                        nghttp2_प्राथमिकता_spec *pri_spec)
{
  पूर्णाङ्क prio = दत्त->समुच्चयः.weight;
  nghttp2_प्राथमिकता_spec_init(pri_spec, 0, sweight_wanted(दत्त), FALSE);
  दत्त->state.weight = prio;
}

/*
 * Check if there is been an update in the priority /
 * dependency settings and if so it submits a PRIORITY frame with the updated
 * info.
 * Flush any out data pending in the network buffer.
 */
स्थिर CURLcode h2_प्रगतिः_egress(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  पूर्णाङ्क rv = 0;

  यदि(sweight_wanted(दत्त) != sweight_अन्तर्_effect(दत्त)) {
    संरचना h2_प्रवाहः_ctx *प्रवाहः = H2_प्रवाहः_CTX(ctx, दत्त);
    यदि(प्रवाहः && प्रवाहः->id > 0 &&
       (sweight_wanted(दत्त) != sweight_अन्तर्_effect(दत्त))) {
      /* send new weight and/or dependency */
      nghttp2_प्राथमिकता_spec pri_spec;

      h2_pri_spec(दत्त, &pri_spec);
      CURL_TRC_CF(दत्त, cf, "[%d] Queuing PRIORITY", प्रवाहः->id);
      DEBUGASSERT(प्रवाहः->id != -1);
      rv = Curl_ngh2_submit_प्राथमिकता(
        &ctx->ngh2, NGHTTP2_FLAG_NONE, प्रवाहः->id, &pri_spec);
      यदि(rv)
        गच्छ बहिर्गमनम्;
    }
  }

  ctx->nw_बहिर्गमनम्_blocked = 0;
  यावत्(!rv && !ctx->nw_बहिर्गमनम्_blocked &&
        Curl_ngh2_session_want_write(&ctx->ngh2))
    rv = Curl_ngh2_session_send(&ctx->ngh2);

बहिर्गमनम्:
  यदि(Curl_ngh2_is_fatal(&ctx->ngh2, rv)) {
    VERBOSE(rv = Curl_ngh2_fatal(&ctx->ngh2));
    CURL_TRC_CF(दत्त, cf, "nghttp2_session_send error (%s)%d",
                nghttp2_strerror(rv), rv);
    प्रत्यागम CURLE_SEND_ERROR;
  }
  /* Defer flushing during the connect phase so that the SETTINGS and
   * other initial frames are sent together with the first request.
   * Unless we are 'connect_only' where the request will never come. */
  यदि(!cf->connected && !cf->conn->द्वयङ्काः.connect_only)
    प्रत्यागम CURLE_OK;
  प्रत्यागम nw_बहिर्गमनम्_flush(cf, दत्त);
}

स्थिर CURLcode प्रवाहः_recv(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                            संरचना h2_प्रवाहः_ctx *प्रवाहः,
                            वर्ण *buf, size_t len, size_t *pnread)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_AGAIN;

  (शून्य)buf;
  (शून्य)len;
  *pnread = 0;

  यदि(!प्रवाहः->xfer_परिणाम)
    प्रवाहः->xfer_परिणाम = cf_h2_update_local_win(cf, दत्त, प्रवाहः);

  यदि(प्रवाहः->xfer_परिणाम) {
    CURL_TRC_CF(दत्त, cf, "[%d] xfer write failed", प्रवाहः->id);
    परिणाम = प्रवाहः->xfer_परिणाम;
  }
  अन्यथा यदि(प्रवाहः->closed) {
    CURL_TRC_CF(दत्त, cf, "[%d] returning CLOSE", प्रवाहः->id);
    परिणाम = http2_handle_प्रवाहः_close(cf, दत्त, प्रवाहः, pnread);
  }
  अन्यथा यदि(प्रवाहः->reset ||
          (ctx->conn_closed && Curl_bufq_is_रिक्तानि(&ctx->inbufq)) ||
          (ctx->rcvd_goaway && ctx->remote_max_sid < प्रवाहः->id)) {
    CURL_TRC_CF(दत्त, cf, "[%d] returning ERR", प्रवाहः->id);
    परिणाम = दत्त->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP2;
  }

  यदि(परिणाम && (परिणाम != CURLE_AGAIN))
    CURL_TRC_CF(दत्त, cf, "[%d] stream_recv(len=%zu) -> %d, %zu",
                प्रवाहः->id, len, (पूर्णाङ्क)परिणाम, *pnread);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode h2_प्रगतिः_ingress(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *दत्त,
                                    size_t दत्त_max_अष्टकानि)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;
  size_t nread;

  यदि(should_close_session(ctx)) {
    CURL_TRC_CF(दत्त, cf, "[0] ingress: session is closed");
    प्रत्यागम CURLE_HTTP2;
  }

  /* Process network input buffer first */
  यदि(!Curl_bufq_is_रिक्तानि(&ctx->inbufq)) {
    CURL_TRC_CF(दत्त, cf, "Process %zu bytes in connection buffer",
                Curl_bufq_len(&ctx->inbufq));
    परिणाम = h2_process_pending_निवेशः(cf, दत्त);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  यदि(!दत्त_max_अष्टकानि)
    दत्त_max_अष्टकानि = H2_CHUNK_आकार;

  /* Receive data from the "lower" filters, e.g. network until
   * it is time to stop due to connection close or us not processing
   * all network input */
  यावत्(!ctx->conn_closed && Curl_bufq_is_रिक्तानि(&ctx->inbufq)) {
    संरचना h2_प्रवाहः_ctx *प्रवाहः = H2_प्रवाहः_CTX(ctx, दत्त);
    यदि(प्रवाहः && (प्रवाहः->closed || !दत्त_max_अष्टकानि)) {
      /* We would like to abort here and stop processing, so that the transfer
       * loop can handle the data/close here. This may leave data in
       * underlying buffers that will not be consumed. */
      यदि(!cf->अग्र || !cf->अग्र->cft->has_दत्त_pending(cf->अग्र, दत्त))
        Curl_multi_mark_dirty(दत्त);
      भङ्ग;
    }

    परिणाम = Curl_cf_recv_bufq(cf->अग्र, दत्त, &ctx->inbufq, 0, &nread);
    यदि(परिणाम) {
      यदि(परिणाम != CURLE_AGAIN) {
        failf(दत्त, "Failed receiving HTTP2 data: %d(%s)", (पूर्णाङ्क)परिणाम,
              curl_easy_strerror(परिणाम));
        प्रत्यागम परिणाम;
      }
      भङ्ग;
    }
    अन्यथा यदि(nread == 0) {
      CURL_TRC_CF(दत्त, cf, "[0] ingress: connection closed");
      ctx->conn_closed = TRUE;
      भङ्ग;
    }
    अन्यथा {
      CURL_TRC_CF(दत्त, cf, "[0] ingress: read %zu bytes", nread);
      दत्त_max_अष्टकानि = (दत्त_max_अष्टकानि > nread) ? (दत्त_max_अष्टकानि - nread) : 0;
    }

    परिणाम = h2_process_pending_निवेशः(cf, दत्त);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    CURL_TRC_CF(दत्त, cf, "[0] ingress: nw-in buffered %zu",
                Curl_bufq_len(&ctx->inbufq));
  }

  यदि(ctx->conn_closed && Curl_bufq_is_रिक्तानि(&ctx->inbufq)) {
    CURL_TRC_CF(दत्त, cf, "server closed %s",
                ctx->rcvd_goaway ? "with GOAWAY" : "abruptly");
    connclose(cf->conn);
  }

  CURL_TRC_CF(दत्त, cf, "[0] ingress: done");
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cf_h2_recv(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                           वर्ण *buf, size_t len, size_t *pnread)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना h2_प्रवाहः_ctx *प्रवाहः;
  CURLcode परिणाम, r2;
  संरचना cf_call_दत्त save;

  यदि(!दत्त)
    प्रत्यागम CURLE_HTTP2;

  प्रवाहः = H2_प्रवाहः_CTX(ctx, दत्त);

  *pnread = 0;
  यदि(!प्रवाहः) {
    /* Abnormal call sequence: either this transfer has never opened a stream
     * (unlikely) or the transfer has been done, cleaned up its resources, but
     * a read() is called anyway. It is not clear what the calling sequence
     * is for such a case. */
    failf(दत्त, "http/2 recv on a transfer never opened "
          "or already cleared, mid=%u", दत्त->mid);
    प्रत्यागम CURLE_HTTP2;
  }

  CF_दत्त_SAVE(save, cf, दत्त);

  परिणाम = प्रवाहः_recv(cf, दत्त, प्रवाहः, buf, len, pnread);
  यदि(परिणाम && (परिणाम != CURLE_AGAIN))
    गच्छ बहिर्गमनम्;

  यदि(परिणाम) {
    परिणाम = h2_प्रगतिः_ingress(cf, दत्त, len);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;

    परिणाम = प्रवाहः_recv(cf, दत्त, प्रवाहः, buf, len, pnread);
  }

  यदि(*pnread > 0) {
    /* Now that we transferred this to the upper layer, we report
     * the actual amount of DATA consumed to the H2 session, so
     * that it adjusts stream flow control */
    Curl_ngh2_session_consume(&ctx->ngh2, प्रवाहः->id, *pnread);
    यदि(प्रवाहः->closed) {
      CURL_TRC_CF(दत्त, cf, "[%d] DRAIN closed stream", प्रवाहः->id);
      Curl_multi_mark_dirty(दत्त);
    }
  }

बहिर्गमनम्:
  r2 = h2_प्रगतिः_egress(cf, दत्त);
  यदि(r2 == CURLE_AGAIN) {
    /* pending data to send, need to be called again. Ideally, we
     * monitor the socket for POLLOUT, but when not SENDING
     * any more, we force processing of the transfer. */
    यदि(!CURL_REQ_WANT_SEND(दत्त))
      Curl_multi_mark_dirty(दत्त);
  }
  अन्यथा यदि(r2 && (परिणाम == CURLE_AGAIN)) {
    /* Do not replace a previous error result NOR a successful result
     * with an egress error. Example: the server might close the
     * connection after sending the complete response with EOS
     * and everything. Further sending may fail, but that MUST
     * not change that we had a successful response. */
    परिणाम = r2;
  }

  CURL_TRC_CF(दत्त, cf, "[%d] cf_recv(len=%zu) -> %d, %zu, "
              "window=%d/%d, connection %d/%d",
              प्रवाहः->id, len, (पूर्णाङ्क)परिणाम, *pnread,
              Curl_ngh2_session_get_प्रवाहः_effective_recv_दत्त_दीर्घता(
                &ctx->ngh2, प्रवाहः->id),
              Curl_ngh2_session_get_प्रवाहः_effective_local_गवाक्षः_आकार(
                &ctx->ngh2, प्रवाहः->id),
              Curl_ngh2_session_get_local_गवाक्षः_आकार(&ctx->ngh2),
              HTTP2_HUGE_गवाक्षः_आकार);

  CF_दत्त_RESTORE(cf, save);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_h2_body_send(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त,
                                संरचना h2_प्रवाहः_ctx *प्रवाहः,
                                ध्रुव शून्य *buf, size_t blen, bool eos,
                                size_t *pnwritten)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  CURLcode परिणाम;

  *pnwritten = 0;
  यदि(प्रवाहः->closed) {
    यदि(प्रवाहः->resp_hds_complete) {
      /* Server decided to close the stream after having sent us a final
       * response. This is valid if it is not interested in the request
       * body. This happens on 30x or 40x responses.
       * We silently discard the data sent, since this is not a transport
       * error situation. */
      CURL_TRC_CF(दत्त, cf, "[%d] discarding data"
                  "on closed stream with response", प्रवाहः->id);
      यदि(eos)
        प्रवाहः->body_eos = TRUE;
      *pnwritten = blen;
      प्रत्यागम CURLE_OK;
    }
    /* Server closed before we got a response, this is an error */
    infof(दत्त, "stream %d closed", प्रवाहः->id);
    प्रत्यागम CURLE_SEND_ERROR;
  }

  परिणाम = Curl_bufq_write(&प्रवाहः->sendbuf, buf, blen, pnwritten);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  यदि(eos && (blen == *pnwritten))
    प्रवाहः->body_eos = TRUE;

  यदि(eos || !Curl_bufq_is_रिक्तानि(&प्रवाहः->sendbuf)) {
    /* resume the potentially suspended stream */
    यदि(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_दत्त(&ctx->ngh2, प्रवाहः->id)))
      प्रत्यागम CURLE_SEND_ERROR;
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode h2_submit(संरचना h2_प्रवाहः_ctx **pstream,
                          संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                          ध्रुव uint8_t *buf, size_t len,
                          bool eos, size_t *pnwritten)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना h2_प्रवाहः_ctx *प्रवाहः = रिक्त;
  संरचना dynhds h2_शीर्षकाणि;
  nghttp2_nv *nva = रिक्त;
  ध्रुव शून्य *body = रिक्त;
  size_t nheader, bodylen;
  nghttp2_दत्त_provider दत्त_prd;
  int32_t प्रवाहः_id;
  nghttp2_प्राथमिकता_spec pri_spec;
  size_t nwritten;
  CURLcode परिणाम = CURLE_OK;
  uint32_t initial_win_आकार;

  *pnwritten = 0;
  Curl_dynhds_init(&h2_शीर्षकाणि, MAX_HTTP_RESP_शीर्षकम्_गणनम्, DYN_HTTP_REQUEST);

  परिणाम = http2_दत्त_सज्जता(cf, दत्त, &प्रवाहः);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  परिणाम = Curl_h1_req_parse_read(
    &प्रवाहः->h1, buf, len, रिक्त,
    !दत्त->state.http_ignorecustom ?
    CURL_EASY_STR(दत्त, STRING_CUSTOMREQUEST) : रिक्त,
    0, &nwritten);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  *pnwritten = nwritten;
  यदि(!प्रवाहः->h1.समाप्तम्) {
    /* need more data */
    गच्छ बहिर्गमनम्;
  }
  DEBUGASSERT(प्रवाहः->h1.req);

  परिणाम = Curl_http_req_गन्तव्यम्_h2(&h2_शीर्षकाणि, प्रवाहः->h1.req, दत्त);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  /* no longer needed */
  Curl_h1_req_parse_free(&प्रवाहः->h1);

  nva = Curl_dynhds_गन्तव्यम्_nva(&h2_शीर्षकाणि, &nheader);
  यदि(!nva) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  h2_pri_spec(दत्त, &pri_spec);
  यदि(!Curl_ngh2_session_check_request_allowed(&ctx->ngh2))
    CURL_TRC_CF(दत्त, cf, "send request NOT allowed (via nghttp2)");

  /* Check the initial windows size of the transfer (rate-limits?) and
   * send an updated settings on changes from previous value. */
  initial_win_आकार = cf_h2_initial_win_आकार(दत्त);
  यदि(initial_win_आकार != ctx->initial_win_आकार) {
    परिणाम = cf_h2_update_settings(ctx, initial_win_आकार);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  विकल्प(दत्त->state.httpreq) {
  पक्ष HTTPREQ_POST:
  पक्ष HTTPREQ_POST_FORM:
  पक्ष HTTPREQ_POST_MIME:
  पक्ष HTTPREQ_PUT:
    दत्त_prd.read_callback = req_body_read_callback;
    दत्त_prd.source.ptr = रिक्त;
    प्रवाहः_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, &दत्त_prd, प्रवाहः);
    भङ्ग;
  स्वभाव:
    प्रवाहः_id = Curl_ngh2_submit_request(
      &ctx->ngh2, &pri_spec, nva, nheader, रिक्त, प्रवाहः);
  }

  यदि(प्रवाहः_id < 0) {
    CURL_TRC_CF(दत्त, cf, "send: nghttp2_submit_request error (%s)%d",
                nghttp2_strerror(प्रवाहः_id), प्रवाहः_id);
    परिणाम = CURLE_SEND_ERROR;
    गच्छ बहिर्गमनम्;
  }

#यदि_परिभाषितम् CURLVERBOSE
#परिभाषय MAX_ACC 60000  /* <64KB to account for some overhead */
  यदि(Curl_trc_is_verbose(दत्त)) {
    size_t acc = 0, i;

    infof(दत्त, "[HTTP/2] [%d] OPENED stream for %s",
          प्रवाहः_id, Curl_bufref_ptr(&दत्त->state.url));
    प्रति(i = 0; i < nheader; ++i) {
      acc += nva[i].namelen + nva[i].valuelen;

      infof(दत्त, "[HTTP/2] [%d] [%.*s: %.*s]", प्रवाहः_id,
            (पूर्णाङ्क)nva[i].namelen, nva[i].name,
            (पूर्णाङ्क)nva[i].valuelen, nva[i].मानम्);
    }

    यदि(acc > MAX_ACC) {
      infof(दत्त, "[HTTP/2] Warning: The cumulative length of all "
            "headers exceeds %d bytes and that could cause the "
            "stream to be rejected.", MAX_ACC);
    }
  }
#यदि_अन्तः

  प्रवाहः->id = प्रवाहः_id;

  body = (ध्रुव वर्ण *)buf + *pnwritten;
  bodylen = len - *pnwritten;

  यदि(bodylen || eos) {
    size_t n;
    परिणाम = cf_h2_body_send(cf, दत्त, प्रवाहः, body, bodylen, eos, &n);
    यदि(!परिणाम)
      *pnwritten += n;
    अन्यथा यदि(परिणाम == CURLE_AGAIN)
      परिणाम = CURLE_OK;
    अन्यथा {
      परिणाम = CURLE_SEND_ERROR;
    }
  }

बहिर्गमनम्:
  CURL_TRC_CF(दत्त, cf, "[%d] submit -> %d, %zu",
              प्रवाहः ? प्रवाहः->id : -1, (पूर्णाङ्क)परिणाम, *pnwritten);
  curlx_safefree(nva);
  *pstream = प्रवाहः;
  Curl_dynhds_free(&h2_शीर्षकाणि);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_h2_send(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                           ध्रुव uint8_t *buf, size_t len, bool eos,
                           size_t *pnwritten)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना h2_प्रवाहः_ctx *प्रवाहः = H2_प्रवाहः_CTX(ctx, दत्त);
  संरचना cf_call_दत्त save;
  CURLcode परिणाम = CURLE_OK, r2;

  CF_दत्त_SAVE(save, cf, दत्त);
  *pnwritten = 0;

  यदि(!प्रवाहः || प्रवाहः->id == -1) {
    परिणाम = h2_submit(&प्रवाहः, cf, दत्त, buf, len, eos, pnwritten);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    DEBUGASSERT(प्रवाहः);
  }
  अन्यथा यदि(प्रवाहः->body_eos) {
    /* We already wrote this, but CURLE_AGAIN-ed the call due to not
     * being able to flush stream->sendbuf. Make a 0-length write
     * to trigger flushing again.
     * If this works, we report to have written `len` bytes. */
    size_t n;
    DEBUGASSERT(eos);
    परिणाम = cf_h2_body_send(cf, दत्त, प्रवाहः, buf, 0, eos, &n);
    CURL_TRC_CF(दत्त, cf, "[%d] cf_body_send last CHUNK -> %d, %zu, eos=%d",
                प्रवाहः->id, (पूर्णाङ्क)परिणाम, n, eos);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    *pnwritten = len;
  }
  अन्यथा {
    परिणाम = cf_h2_body_send(cf, दत्त, प्रवाहः, buf, len, eos, pnwritten);
    CURL_TRC_CF(दत्त, cf, "[%d] cf_body_send(len=%zu) -> %d, %zu, eos=%d",
                प्रवाहः->id, len, (पूर्णाङ्क)परिणाम, *pnwritten, eos);
  }

  /* Call the nghttp2 send loop and flush to write ALL buffered data,
   * headers and/or request body completely out to the network */
  r2 = h2_प्रगतिः_egress(cf, दत्त);

  /* if the stream has been closed in egress handling (nghttp2 does that
   * when it does not like the headers, for example */
  यदि(प्रवाहः && प्रवाहः->closed && !प्रवाहः->resp_hds_complete) {
    infof(दत्त, "stream %d closed", प्रवाहः->id);
    परिणाम = CURLE_SEND_ERROR;
    गच्छ बहिर्गमनम्;
  }
  अन्यथा यदि(r2 && (r2 != CURLE_AGAIN)) {
    परिणाम = r2;
    गच्छ बहिर्गमनम्;
  }

  यदि(should_close_session(ctx)) {
    /* nghttp2 thinks this session is done. If the stream has not been
     * closed, this is an error state for out transfer */
    यदि(प्रवाहः && प्रवाहः->closed) {
      परिणाम = http2_handle_प्रवाहः_close(cf, दत्त, प्रवाहः, pnwritten);
    }
    अन्यथा {
      CURL_TRC_CF(दत्त, cf, "send: nothing to do in this session");
      परिणाम = CURLE_HTTP2;
    }
  }

बहिर्गमनम्:
  यदि(प्रवाहः) {
    CURL_TRC_CF(दत्त, cf, "[%d] cf_send(len=%zu) -> %d, %zu, "
                "eos=%d, h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                प्रवाहः->id, len, (पूर्णाङ्क)परिणाम, *pnwritten,
                प्रवाहः->body_eos,
                Curl_ngh2_session_get_प्रवाहः_remote_गवाक्षः_आकार(
                  &ctx->ngh2, प्रवाहः->id),
                Curl_ngh2_session_get_remote_गवाक्षः_आकार(&ctx->ngh2),
                Curl_bufq_len(&प्रवाहः->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  अन्यथा {
    CURL_TRC_CF(दत्त, cf, "cf_send(len=%zu) -> %d, %zu, "
                "connection-window=%d, nw_send_buffer(%zu)",
                len, (पूर्णाङ्क)परिणाम, *pnwritten,
                Curl_ngh2_session_get_remote_गवाक्षः_आकार(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_दत्त_RESTORE(cf, save);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_h2_flush(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *दत्त)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना h2_प्रवाहः_ctx *प्रवाहः = H2_प्रवाहः_CTX(ctx, दत्त);
  संरचना cf_call_दत्त save;
  CURLcode परिणाम = CURLE_OK;

  CF_दत्त_SAVE(save, cf, दत्त);
  यदि(प्रवाहः && !Curl_bufq_is_रिक्तानि(&प्रवाहः->sendbuf)) {
    /* resume the potentially suspended stream */
    यदि(Curl_ngh2_is_fatal(&ctx->ngh2,
         Curl_ngh2_session_resume_दत्त(&ctx->ngh2, प्रवाहः->id))) {
      परिणाम = CURLE_SEND_ERROR;
      गच्छ बहिर्गमनम्;
    }
  }

  परिणाम = h2_प्रगतिः_egress(cf, दत्त);

बहिर्गमनम्:
  यदि(प्रवाहः) {
    CURL_TRC_CF(दत्त, cf, "[%d] flush -> %d, "
                "h2 windows %d-%d (stream-conn), "
                "buffers %zu-%zu (stream-conn)",
                प्रवाहः->id, (पूर्णाङ्क)परिणाम,
                Curl_ngh2_session_get_प्रवाहः_remote_गवाक्षः_आकार(
                  &ctx->ngh2, प्रवाहः->id),
                Curl_ngh2_session_get_remote_गवाक्षः_आकार(&ctx->ngh2),
                Curl_bufq_len(&प्रवाहः->sendbuf),
                Curl_bufq_len(&ctx->outbufq));
  }
  अन्यथा {
    CURL_TRC_CF(दत्त, cf, "flush -> %d, "
                "connection-window=%d, nw_send_buffer(%zu)", (पूर्णाङ्क)परिणाम,
                Curl_ngh2_session_get_remote_गवाक्षः_आकार(&ctx->ngh2),
                Curl_bufq_len(&ctx->outbufq));
  }
  CF_दत्त_RESTORE(cf, save);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_h2_adjust_pollset(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *दत्त,
                                     संरचना easy_pollset *ps)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना cf_call_दत्त save;
  curl_socket_t sock;
  bool want_recv, want_send;
  CURLcode परिणाम = CURLE_OK;

  यदि(!Curl_ngh2_valid(&ctx->ngh2))
    प्रत्यागम CURLE_OK;

  sock = Curl_conn_cf_get_socket(cf, दत्त);
  Curl_pollset_check(दत्त, ps, sock, &want_recv, &want_send);
  यदि(want_recv || want_send) {
    संरचना h2_प्रवाहः_ctx *प्रवाहः = H2_प्रवाहः_CTX(ctx, दत्त);
    bool c_exhaust, s_exhaust;

    CF_दत्त_SAVE(save, cf, दत्त);
    c_exhaust = want_send &&
                !Curl_ngh2_session_get_remote_गवाक्षः_आकार(&ctx->ngh2);
    s_exhaust = want_send && प्रवाहः && प्रवाहः->id >= 0 &&
                !Curl_ngh2_session_get_प्रवाहः_remote_गवाक्षः_आकार(
                  &ctx->ngh2, प्रवाहः->id);
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                (!c_exhaust && Curl_ngh2_session_want_write(&ctx->ngh2)) ||
                !Curl_bufq_is_रिक्तानि(&ctx->outbufq);

    परिणाम = Curl_pollset_समुच्चयः(दत्त, ps, sock, want_recv, want_send);
    CF_दत्त_RESTORE(cf, save);
  }
  अन्यथा यदि(ctx->प्रेषितम्_goaway && !cf->shutdown) {
    /* shutdown in progress */
    CF_दत्त_SAVE(save, cf, दत्त);
    want_send = Curl_ngh2_session_want_write(&ctx->ngh2) ||
                !Curl_bufq_is_रिक्तानि(&ctx->outbufq);
    want_recv = Curl_ngh2_session_want_read(&ctx->ngh2);
    परिणाम = Curl_pollset_समुच्चयः(दत्त, ps, sock, want_recv, want_send);
    CF_दत्त_RESTORE(cf, save);
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_h2_ctx_open(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना h2_प्रवाहः_ctx *प्रवाहः;
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  पूर्णाङ्क rc;
  nghttp2_session_callbacks *cbs = रिक्त;

  DEBUGASSERT(!ctx->ngh2.s);
  DEBUGASSERT(!ctx->ngh2.fatal_rv);
  DEBUGASSERT(ctx->initialized);

  rc = nghttp2_session_callbacks_new(&cbs);
  यदि(rc) {
    failf(दत्त, "Could not initialize nghttp2 callbacks");
    गच्छ बहिर्गमनम्;
  }

  nghttp2_session_callbacks_समुच्चयः_send_callback(cbs, send_callback);
  nghttp2_session_callbacks_समुच्चयः_on_frame_recv_callback(cbs, on_frame_recv);
  nghttp2_session_callbacks_समुच्चयः_on_invalid_frame_recv_callback(cbs,
    cf_h2_on_invalid_frame_recv);
#यदि_परिभाषितम् CURLVERBOSE
  nghttp2_session_callbacks_समुच्चयः_on_frame_send_callback(cbs, on_frame_send);
#यदि_अन्तः
  nghttp2_session_callbacks_समुच्चयः_on_दत्त_chunk_recv_callback(
    cbs, on_दत्त_chunk_recv);
  nghttp2_session_callbacks_समुच्चयः_on_प्रवाहः_close_callback(cbs, on_प्रवाहः_close);
  nghttp2_session_callbacks_समुच्चयः_on_शीर्षकम्_callback(cbs, on_शीर्षकम्);
#यदि_परिभाषितम् CURLVERBOSE
  nghttp2_session_callbacks_समुच्चयः_error_callback(cbs, error_callback);
#यदि_अन्तः

  /* The nghttp2 session is not yet setup, do it */
  rc = h2_client_new(cf, cbs);
  यदि(rc) {
    failf(दत्त, "Could not initialize nghttp2");
    गच्छ बहिर्गमनम्;
  }
  ctx->max_concurrent_streams = दत्त->multi ?
    Curl_multi_max_concurrent_streams(दत्त->multi) :
    DEFAULT_MAX_CONCURRENT_STREAMS;

  यदि(ctx->द्वारा_h1_upgrade) {
    /* HTTP/1.1 Upgrade issued. H2 Settings have already been submitted
     * in the H1 request and we upgrade from there. This stream
     * is opened implicitly as #1. */
    uint8_t binsettings[H2_BINSETTINGS_LEN];
    ssize_t rclen;
    size_t binlen; /* length of the binsettings data */

    rclen = populate_binsettings(binsettings, दत्त);

    यदि(!curlx_sztouz(rclen, &binlen) || !binlen) {
      failf(दत्त, "nghttp2 unexpectedly failed on pack_settings_payload");
      परिणाम = CURLE_FAILED_INIT;
      गच्छ बहिर्गमनम्;
    }

    परिणाम = http2_दत्त_सज्जता(cf, दत्त, &प्रवाहः);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    DEBUGASSERT(प्रवाहः);
    प्रवाहः->id = 1;
    /* queue SETTINGS frame (again) */
    rc = Curl_ngh2_session_upgrade2(
      &ctx->ngh2, binsettings, binlen, दत्त->state.httpreq == HTTPREQ_शीर्ष,
      रिक्त);
    यदि(rc) {
      failf(दत्त, "nghttp2_session_upgrade2() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      परिणाम = CURLE_HTTP2;
      गच्छ बहिर्गमनम्;
    }

    rc = Curl_ngh2_session_समुच्चयः_प्रवाहः_user_दत्त(
      &ctx->ngh2, प्रवाहः->id, प्रवाहः);
    यदि(rc) {
      infof(दत्त, "http/2: failed to set user_data for stream %d",
            प्रवाहः->id);
      DEBUGASSERT(0);
    }
    CURL_TRC_CF(दत्त, cf, "created session via Upgrade");
  }
  अन्यथा {
    nghttp2_settings_प्रविष्टिः iv[H2_SETTINGS_IV_LEN];
    size_t ivlen;

    ivlen = populate_settings(iv, दत्त, ctx);
    rc = Curl_ngh2_submit_settings(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, iv, ivlen);
    यदि(rc) {
      failf(दत्त, "nghttp2_submit_settings() failed: %s(%d)",
            nghttp2_strerror(rc), rc);
      परिणाम = CURLE_HTTP2;
      गच्छ बहिर्गमनम्;
    }
  }

  rc = Curl_ngh2_session_समुच्चयः_local_गवाक्षः_आकार(
    &ctx->ngh2, NGHTTP2_FLAG_NONE, 0, HTTP2_HUGE_गवाक्षः_आकार);
  यदि(rc) {
    failf(दत्त, "nghttp2_session_set_local_window_size() failed: %s(%d)",
          nghttp2_strerror(rc), rc);
    परिणाम = CURLE_HTTP2;
    गच्छ बहिर्गमनम्;
  }

  /* all set, traffic will be send on connect */
  परिणाम = CURLE_OK;
  CURL_TRC_CF(दत्त, cf, "[0] created h2 session%s",
              ctx->द्वारा_h1_upgrade ? " (via h1 upgrade)" : "");

बहिर्गमनम्:
  यदि(cbs)
    nghttp2_session_callbacks_del(cbs);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_h2_connect(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *दत्त,
                              bool *समाप्तम्)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  CURLcode परिणाम = CURLE_OK;
  संरचना cf_call_दत्त save;
  bool first_time = FALSE;

  यदि(cf->connected) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  /* Connect the lower filters first */
  यदि(!cf->अग्र->connected) {
    परिणाम = Curl_conn_cf_connect(cf->अग्र, दत्त, समाप्तम्);
    यदि(परिणाम || !*समाप्तम्)
      प्रत्यागम परिणाम;
  }

  *समाप्तम् = FALSE;

  CF_दत्त_SAVE(save, cf, दत्त);
  DEBUGASSERT(ctx->initialized);
  यदि(!ctx->ngh2.s) {
    परिणाम = cf_h2_ctx_open(cf, दत्त);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    first_time = TRUE;
  }

  यदि(!first_time) {
    परिणाम = h2_प्रगतिः_ingress(cf, दत्त, 0);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  /* Send out our SETTINGS and ACKs and such. If that blocks, we
   * have it buffered and can count this filter as being connected */
  परिणाम = h2_प्रगतिः_egress(cf, दत्त);
  यदि(परिणाम && (परिणाम != CURLE_AGAIN))
    गच्छ बहिर्गमनम्;

  *समाप्तम् = TRUE;
  cf->connected = TRUE;
  परिणाम = CURLE_OK;

बहिर्गमनम्:
  CURL_TRC_CF(दत्त, cf, "cf_connect() -> %d, %d, ", (पूर्णाङ्क)परिणाम, *समाप्तम्);
  CF_दत्त_RESTORE(cf, save);
  प्रत्यागम परिणाम;
}

स्थिर शून्य cf_h2_destroy(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;

  (शून्य)दत्त;
  यदि(ctx) {
    cf_h2_ctx_free(ctx);
    cf->ctx = रिक्त;
  }
}

स्थिर CURLcode cf_h2_shutdown(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना cf_call_दत्त save;
  CURLcode परिणाम;
  पूर्णाङ्क rv;

  यदि(!cf->connected || !Curl_ngh2_valid(&ctx->ngh2) ||
     cf->shutdown || ctx->conn_closed) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  CF_दत्त_SAVE(save, cf, दत्त);

  यदि(!ctx->प्रेषितम्_goaway) {
    ctx->प्रेषितम्_goaway = TRUE;
    rv = Curl_ngh2_submit_goaway(
      &ctx->ngh2, NGHTTP2_FLAG_NONE, ctx->local_max_sid, 0,
      (ध्रुव uint8_t *)"shutdown", परिमाण("shutdown"));
    यदि(rv) {
      failf(दत्त, "nghttp2_submit_goaway() failed: %s(%d)",
            nghttp2_strerror(rv), rv);
      परिणाम = CURLE_SEND_ERROR;
      गच्छ बहिर्गमनम्;
    }
  }
  /* GOAWAY submitted, process egress and ingress until nghttp2 is done. */
  परिणाम = CURLE_OK;
  यदि(Curl_ngh2_session_want_write(&ctx->ngh2) ||
     !Curl_bufq_is_रिक्तानि(&ctx->outbufq))
    परिणाम = h2_प्रगतिः_egress(cf, दत्त);
  यदि(!परिणाम && Curl_ngh2_session_want_read(&ctx->ngh2))
    परिणाम = h2_प्रगतिः_ingress(cf, दत्त, 0);

  यदि(परिणाम == CURLE_AGAIN)
    परिणाम = CURLE_OK;

  *समाप्तम् = (ctx->conn_closed ||
           (!परिणाम && !Curl_ngh2_session_want_write(&ctx->ngh2) &&
            !Curl_ngh2_session_want_read(&ctx->ngh2) &&
            Curl_bufq_is_रिक्तानि(&ctx->outbufq)));

बहिर्गमनम्:
  CF_दत्त_RESTORE(cf, save);
  cf->shutdown = (परिणाम || *समाप्तम्);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode http2_दत्त_pause(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 bool pause)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना h2_प्रवाहः_ctx *प्रवाहः = H2_प्रवाहः_CTX(ctx, दत्त);

  DEBUGASSERT(दत्त);
  यदि(ctx && Curl_ngh2_valid(&ctx->ngh2) && प्रवाहः) {
    CURLcode परिणाम;

    प्रवाहः->write_paused = pause;
    परिणाम = cf_h2_update_local_win(cf, दत्त, प्रवाहः);
    यदि(परिणाम)
      प्रत्यागम परिणाम;

    /* attempt to send the window update */
    (शून्य)h2_प्रगतिः_egress(cf, दत्त);

    यदि(!pause) {
      /* Unpausing a h2 transfer, requires it to be run again. The server
       * may send new DATA on us increasing the flow window, and it may
       * not. We may have already buffered and exhausted the new window
       * by operating on things in flight during the handling of other
       * transfers. */
      Curl_multi_mark_dirty(दत्त);
    }
    CURL_TRC_CF(दत्त, cf, "[%d] stream now %spaused", प्रवाहः->id,
                pause ? "" : "un");
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cf_h2_cntrl(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *दत्त,
                            पूर्णाङ्क event, पूर्णाङ्क arg1, शून्य *arg2)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना cf_call_दत्त save;

  (शून्य)arg2;

  CF_दत्त_SAVE(save, cf, दत्त);
  विकल्प(event) {
  पक्ष CF_CTRL_दत्त_सज्जता:
    भङ्ग;
  पक्ष CF_CTRL_दत्त_PAUSE:
    परिणाम = http2_दत्त_pause(cf, दत्त, (arg1 != 0));
    भङ्ग;
  पक्ष CF_CTRL_FLUSH:
    परिणाम = cf_h2_flush(cf, दत्त);
    भङ्ग;
  पक्ष CF_CTRL_दत्त_समाप्तम्:
    http2_दत्त_समाप्तम्(cf, दत्त);
    भङ्ग;
  पक्ष CF_CTRL_CONN_INFO_UPDATE:
    यदि(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_आलोकितम् = 20;
      Curl_conn_समुच्चयः_multiplex(cf->conn);
    }
    भङ्ग;
  स्वभाव:
    भङ्ग;
  }
  CF_दत्त_RESTORE(cf, save);
  प्रत्यागम परिणाम;
}

स्थिर bool cf_h2_दत्त_pending(संरचना Curl_cfilter *cf,
                               ध्रुव संरचना Curl_easy *दत्त)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;

  यदि(ctx && !Curl_bufq_is_रिक्तानि(&ctx->inbufq))
    प्रत्यागम TRUE;
  प्रत्यागम cf->अग्र ? cf->अग्र->cft->has_दत्त_pending(cf->अग्र, दत्त) : FALSE;
}

स्थिर bool cf_h2_is_alive(संरचना Curl_cfilter *cf,
                           संरचना Curl_easy *दत्त,
                           bool *निवेशः_pending)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  bool alive;
  संरचना cf_call_दत्त save;

  *निवेशः_pending = FALSE;
  CF_दत्त_SAVE(save, cf, दत्त);
  alive = (ctx && Curl_ngh2_valid(&ctx->ngh2) &&
           http2_connisalive(cf, दत्त, निवेशः_pending));
  CURL_TRC_CF(दत्त, cf, "conn alive -> %d, input_pending=%d",
              alive, *निवेशः_pending);
  CF_दत्त_RESTORE(cf, save);
  प्रत्यागम alive;
}

स्थिर CURLcode cf_h2_keep_alive(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त)
{
  CURLcode परिणाम;
  संरचना cf_call_दत्त save;

  CF_दत्त_SAVE(save, cf, दत्त);
  परिणाम = http2_send_ping(cf, दत्त);
  CF_दत्त_RESTORE(cf, save);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_h2_query(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *दत्त,
                            पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2)
{
  संरचना cf_h2_ctx *ctx = cf->ctx;
  संरचना cf_call_दत्त save;
  size_t effective_max;

  विकल्प(query) {
  पक्ष CF_QUERY_MAX_CONCURRENT:
    DEBUGASSERT(pres1);

    CF_दत्त_SAVE(save, cf, दत्त);
    यदि(!Curl_ngh2_valid(&ctx->ngh2) ||
       !Curl_ngh2_session_check_request_allowed(&ctx->ngh2)) {
      /* the limit is what we have in use right now */
      effective_max = cf->conn->attached_xfers;
    }
    अन्यथा {
      effective_max = ctx->max_concurrent_streams;
    }
    *pres1 = (effective_max > अधिकतम_पूर्णाङ्कः) ? अधिकतम_पूर्णाङ्कः : (पूर्णाङ्क)effective_max;
    CF_दत्त_RESTORE(cf, save);
    प्रत्यागम CURLE_OK;
  पक्ष CF_QUERY_प्रवाहः_ERROR: {
    संरचना h2_प्रवाहः_ctx *प्रवाहः = H2_प्रवाहः_CTX(ctx, दत्त);
    *pres1 = प्रवाहः ? (पूर्णाङ्क)प्रवाहः->error : 0;
    प्रत्यागम CURLE_OK;
  }
  पक्ष CF_QUERY_NEED_FLUSH: {
    संरचना h2_प्रवाहः_ctx *प्रवाहः = H2_प्रवाहः_CTX(ctx, दत्त);
    यदि(!Curl_bufq_is_रिक्तानि(&ctx->outbufq) ||
       (प्रवाहः && !Curl_bufq_is_रिक्तानि(&प्रवाहः->sendbuf))) {
      *pres1 = TRUE;
      प्रत्यागम CURLE_OK;
    }
    भङ्ग;
  }
  पक्ष CF_QUERY_HTTP_संस्करणम्:
    *pres1 = 20;
    प्रत्यागम CURLE_OK;
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम cf->अग्र ?
    cf->अग्र->cft->query(cf->अग्र, दत्त, query, pres1, pres2) :
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
  cf_h2_दत्त_pending,
  cf_h2_send,
  cf_h2_recv,
  cf_h2_cntrl,
  cf_h2_is_alive,
  cf_h2_keep_alive,
  cf_h2_query,
};

स्थिर CURLcode http2_cfilter_सङ्कलय(संरचना Curl_cfilter **pcf,
                                  संरचना Curl_easy *दत्त,
                                  संरचना connectdata *conn,
                                  int8_t sockindex,
                                  bool द्वारा_h1_upgrade)
{
  संरचना Curl_cfilter *cf = रिक्त;
  संरचना cf_h2_ctx *ctx;
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  DEBUGASSERT(दत्त->conn);
  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx)
    गच्छ बहिर्गमनम्;
  cf_h2_ctx_init(ctx, द्वारा_h1_upgrade);

  परिणाम = Curl_cf_create(&cf, &Curl_cft_nghttp2, ctx);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  ctx = रिक्त;
  Curl_conn_cf_सङ्कलय(दत्त, conn, sockindex, cf);

बहिर्गमनम्:
  यदि(परिणाम)
    cf_h2_ctx_free(ctx);
  *pcf = परिणाम ? रिक्त : cf;
  प्रत्यागम परिणाम;
}

स्थिर CURLcode http2_cfilter_निवेशय_after(संरचना Curl_cfilter *cf,
                                           संरचना Curl_easy *दत्त,
                                           bool द्वारा_h1_upgrade)
{
  संरचना Curl_cfilter *cf_h2 = रिक्त;
  संरचना cf_h2_ctx *ctx;
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  (शून्य)दत्त;
  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx)
    गच्छ बहिर्गमनम्;
  cf_h2_ctx_init(ctx, द्वारा_h1_upgrade);

  परिणाम = Curl_cf_create(&cf_h2, &Curl_cft_nghttp2, ctx);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  ctx = रिक्त;
  Curl_conn_cf_निवेशय_after(cf, cf_h2);

बहिर्गमनम्:
  यदि(परिणाम)
    cf_h2_ctx_free(ctx);
  प्रत्यागम परिणाम;
}

bool Curl_http2_may_switch(संरचना Curl_easy *दत्त)
{
  यदि(Curl_conn_http_संस्करणम्(दत्त, दत्त->conn) < 20 &&
     (दत्त->state.http_neg.wanted & CURL_HTTP_V2x) &&
     दत्त->state.http_neg.h2_प्राक्_knowledge) {
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
    यदि(दत्त->conn->द्वयङ्काः.origin_is_proxy) {
      infof(दत्त, "Ignoring HTTP/2 prior knowledge due to proxy");
      प्रत्यागम FALSE;
    }
#यदि_अन्तः
    प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}

CURLcode Curl_http2_switch(संरचना Curl_easy *दत्त)
{
  संरचना Curl_cfilter *cf;
  CURLcode परिणाम;

  DEBUGASSERT(Curl_conn_http_संस्करणम्(दत्त, दत्त->conn) < 20);

  परिणाम = http2_cfilter_सङ्कलय(&cf, दत्त, दत्त->conn, FIRSTSOCKET, FALSE);
  यदि(परिणाम)
    प्रत्यागम परिणाम;
  CURL_TRC_CF(दत्त, cf, "switching connection to HTTP/2");

  यदि(cf->अग्र) {
    bool समाप्तम्;
    प्रत्यागम Curl_conn_cf_connect(cf, दत्त, &समाप्तम्);
  }
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_http2_switch_at(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त)
{
  संरचना Curl_cfilter *cf_h2;
  CURLcode परिणाम;

  DEBUGASSERT(Curl_conn_http_संस्करणम्(दत्त, दत्त->conn) < 20);

  परिणाम = http2_cfilter_निवेशय_after(cf, दत्त, FALSE);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  cf_h2 = cf->अग्र;

  यदि(cf_h2->अग्र) {
    bool समाप्तम्;
    प्रत्यागम Curl_conn_cf_connect(cf_h2, दत्त, &समाप्तम्);
  }
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_http2_upgrade(संरचना Curl_easy *दत्त,
                            संरचना connectdata *conn, int8_t sockindex,
                            ध्रुव वर्ण *mem, size_t nread)
{
  संरचना Curl_cfilter *cf;
  संरचना cf_h2_ctx *ctx;
  CURLcode परिणाम;

  DEBUGASSERT(Curl_conn_http_संस्करणम्(दत्त, conn) < 20);

  परिणाम = http2_cfilter_सङ्कलय(&cf, दत्त, conn, sockindex, TRUE);
  यदि(परिणाम)
    प्रत्यागम परिणाम;
  CURL_TRC_CF(दत्त, cf, "upgrading connection to HTTP/2");

  DEBUGASSERT(cf->cft == &Curl_cft_nghttp2);
  ctx = cf->ctx;

  दत्त->req.httpversion_प्रेषितम् = 20; /* it is an h2 request now */
  दत्त->req.शीर्षकम् = TRUE; /* we expect the real response to come in h2 */
  दत्त->req.headerline = 0; /* restart the header line counter */

  यदि(nread > 0) {
    /* Remaining data from the protocol switch reply is already using
     * the switched protocol, ie. HTTP/2. We add that to the network
     * inbufq. */
    size_t copied;

    परिणाम = Curl_bufq_write(&ctx->inbufq,
                             (ध्रुव अचिह्नित वर्ण *)mem, nread, &copied);
    यदि(परिणाम) {
      failf(दत्त, "error on copying HTTP Upgrade response: %d", (पूर्णाङ्क)परिणाम);
      प्रत्यागम CURLE_RECV_ERROR;
    }
    यदि(copied < nread) {
      failf(दत्त, "connection buffer size could not take all data "
            "from HTTP Upgrade response header: copied=%zu, datalen=%zu",
            copied, nread);
      प्रत्यागम CURLE_HTTP2;
    }
    infof(दत्त, "Copied HTTP/2 data in stream buffer to connection buffer"
          " after upgrade: len=%zu", nread);
  }

  यदि(cf->अग्र) {
    bool समाप्तम्;
    परिणाम = Curl_conn_cf_connect(cf, दत्त, &समाप्तम्);
    यदि(!परिणाम)
      cf->cft->cntrl(cf, दत्त, CF_CTRL_CONN_INFO_UPDATE, 0, रिक्त);
  }
  प्रत्यागम परिणाम;
}

/* Only call this function for a transfer that already got an HTTP/2
   CURLE_HTTP2_STREAM error! */
bool Curl_h2_http_1_1_error(संरचना Curl_easy *दत्त)
{
  यदि(Curl_conn_http_संस्करणम्(दत्त, दत्त->conn) == 20) {
    पूर्णाङ्क err = Curl_conn_get_प्रवाहः_error(दत्त, दत्त->conn, FIRSTSOCKET);
    प्रत्यागम err == NGHTTP2_HTTP_1_1_REQUIRED;
  }
  प्रत्यागम FALSE;
}

शून्य *Curl_nghttp2_malloc(size_t आकार, शून्य *user_दत्त)
{
  (शून्य)user_दत्त;
  प्रत्यागम Curl_cmalloc(आकार);
}

शून्य Curl_nghttp2_free(शून्य *ptr, शून्य *user_दत्त)
{
  (शून्य)user_दत्त;
  Curl_cfree(ptr);
}

शून्य *Curl_nghttp2_calloc(size_t nmemb, size_t आकार, शून्य *user_दत्त)
{
  (शून्य)user_दत्त;
  प्रत्यागम Curl_ccalloc(nmemb, आकार);
}

शून्य *Curl_nghttp2_realloc(शून्य *ptr, size_t आकार, शून्य *user_दत्त)
{
  (शून्य)user_दत्त;
  प्रत्यागम Curl_crealloc(ptr, आकार);
}

#अन्यथा /* CURL_DISABLE_HTTP || !USE_NGHTTP2 */

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

#यदि_अन्तः /* !CURL_DISABLE_HTTP && USE_NGHTTP2 */
