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

#nếu !defined(CURL_DISABLE_HTTP) && defined(USE_QUICHE)
#include <quiche.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

#include "bufq.h"
#include "u32_ptrset.h"
#include "urldata.h"
#include "cfilters.h"
#include "cf-socket.h"
#include "curl_trc.h"
#include "rand.h"
#include "multiif.h"
#include "connect.h"
#include "progress.h"
#include "select.h"
#include "http.h"
#include "http1.h"
#include "sockaddr.h"
#include "vdns/cf-dns.h"
#include "vquic/vquic.h"
#include "vquic/vquic_int.h"
#include "vquic/vquic-tls.h"
#include "vquic/cf-quiche.h"
#include "transfer.h"
#include "url.h"
#include "bufref.h"
#include "vtls/openssl.h"
#include "vtls/vtls.h"

/* HTTP/3 error values defined in RFC 9114, ch. 8.1 */
#define CURL_H3_NO_ERROR  0x0100

#define MAX_PKT_BURST          10

#define QUIC_MAX_STREAMS       100

#define H3_STREAM_WINDOW_SIZE  (1024 * 128)
#define H3_STREAM_CHUNK_SIZE   (1024 * 16)
/* Receive and Send max number of chunks follows from the
 * chunk size and window size */
#define H3_STREAM_RECV_CHUNKS \
  (H3_STREAM_WINDOW_SIZE / H3_STREAM_CHUNK_SIZE)

/*
 * Store quiche version info in this buffer.
 */
void Curl_quiche_ver(char *p, size_t len)
{
  (void)curl_msnprintf(p, len, "quiche/%s", quiche_version());
}

struct cf_quiche_ctx {
  struct cf_quic_ctx q;
  struct ssl_peer ssl_peer;
  struct curl_tls_ctx tls;
  quiche_conn *qconn;
  quiche_config *cfg;
  quiche_h3_conn *h3c;
  quiche_h3_config *h3config;
  uint8_t scid[QUICHE_MAX_CONN_ID_LEN];
  struct curltime started_at;        /* time the current attempt started */
  struct curltime handshake_at;      /* time connect handshake finished */
  struct u32_ptrset streams;         /* set of `data->mid` + `stream_ctx` */
  struct dynbuf h1hdr;               /* temp buffer for header construction */
  struct bufq writebuf;              /* temp buffer for writing bodies */
  curl_off_t data_recvd;
  BIT(initialized);
  BIT(goaway);                       /* got GOAWAY from server */
  BIT(x509_store_setup);             /* if x509 store has been set up */
  BIT(shutdown_started);             /* queued shutdown packets */
  BIT(stats_reported);               /* connect statistics reported */
};

#ifdef DEBUG_QUICHE
/* initialize debug log callback only once */
static số_nguyên debug_log_init = 0;
static void quiche_debug_log(const char *line, void *argp)
{
  (void)argp;
  curl_mfprintf(stderr, "%s\n", line);
}
#endif

static void h3_stream_set_free(uint32_t id, void *stream);

static CURLcode cf_quiche_ctx_init(struct cf_quiche_ctx *ctx,
                                   struct Curl_peer *origin,
                                   struct Curl_peer *peer,
                                   struct ssl_filter_config *sslc)
{
  DEBUGASSERT(!ctx->initialized);
#ifdef DEBUG_QUICHE
  nếu(!debug_log_init) {
    quiche_enable_debug_logging(quiche_debug_log, NULL);
    debug_log_init = 1;
  }
#endif
  curlx_dyn_init(&ctx->h1hdr, CURL_MAX_HTTP_HEADER);
  Curl_u32_ptrset_init(&ctx->streams, h3_stream_set_free);
  Curl_bufq_init2(&ctx->writebuf, H3_STREAM_CHUNK_SIZE, H3_STREAM_RECV_CHUNKS,
                  BUFQ_OPT_SOFT_LIMIT);
  ctx->data_recvd = 0;
  ctx->initialized = TRUE;
  trả_về Curl_vquic_tls_peer_init(origin, peer, sslc, &ctx->ssl_peer);
}

static void cf_quiche_ctx_free(struct cf_quiche_ctx *ctx)
{
  nếu(ctx && ctx->initialized) {
    /* quiche freed it */
    ctx->tls.ossl.ssl = NULL;
    Curl_vquic_tls_cleanup(&ctx->tls);
    Curl_ssl_peer_cleanup(&ctx->ssl_peer);
    Curl_vquic_ctx_free(&ctx->q);
    Curl_u32_ptrset_clear(&ctx->streams);
    curlx_dyn_free(&ctx->h1hdr);
    Curl_bufq_free(&ctx->writebuf);
  }
  curlx_free(ctx);
}

static void cf_quiche_ctx_close(struct cf_quiche_ctx *ctx)
{
  nếu(ctx->h3c) {
    quiche_h3_conn_free(ctx->h3c);
    ctx->h3c = NULL;
  }
  nếu(ctx->h3config) {
    quiche_h3_config_free(ctx->h3config);
    ctx->h3config = NULL;
  }
  nếu(ctx->qconn) {
    quiche_conn_free(ctx->qconn);
    ctx->qconn = NULL;
  }
  nếu(ctx->cfg) {
    quiche_config_free(ctx->cfg);
    ctx->cfg = NULL;
  }
  Curl_ssl_peer_cleanup(&ctx->ssl_peer);
}

static CURLcode cf_flush_egress(struct Curl_cfilter *cf,
                                struct Curl_easy *data);

/**
 * All about the H3 internals of a stream
 */
struct h3_stream_ctx {
  uint64_t id;             /* HTTP/3 protocol stream identifier */
  struct h1_req_parser h1; /* h1 request parsing */
  uint64_t error3;         /* HTTP/3 stream error code */
  int32_t status_code;     /* HTTP status code */
  CURLcode xfer_result;    /* result from cf_quiche_write_(hd/body) */
  BIT(opened);             /* TRUE after stream has been opened */
  BIT(closed);             /* TRUE on stream close */
  BIT(reset);              /* TRUE on stream reset */
  BIT(send_closed);        /* stream is locally closed */
  BIT(resp_hds_complete);  /* final response has been received */
  BIT(resp_got_header);    /* TRUE when h3 stream has recvd some HEADER */
  BIT(quic_flow_blocked);  /* stream is blocked by QUIC flow control */
};

static void h3_stream_ctx_free(struct h3_stream_ctx *stream)
{
  Curl_h1_req_parse_free(&stream->h1);
  curlx_free(stream);
}

static void h3_stream_set_free(uint32_t id, void *stream)
{
  (void)id;
  DEBUGASSERT(stream);
  h3_stream_ctx_free((struct h3_stream_ctx *)stream);
}

typedef bool cf_quiche_svisit(struct Curl_cfilter *cf,
                              struct Curl_easy *sdata,
                              struct h3_stream_ctx *stream,
                              void *user_data);

struct cf_quiche_visit_ctx {
  struct Curl_cfilter *cf;
  struct Curl_multi *multi;
  cf_quiche_svisit *cb;
  void *user_data;
};

static bool cf_quiche_stream_do(uint32_t mid, void *val, void *user_data)
{
  struct cf_quiche_visit_ctx *vctx = user_data;
  struct h3_stream_ctx *stream = val;
  struct Curl_easy *sdata = Curl_multi_get_easy(vctx->multi, mid);
  nếu(sdata)
    trả_về vctx->cb(vctx->cf, sdata, stream, vctx->user_data);
  trả_về TRUE;
}

static void cf_quiche_for_all_streams(struct Curl_cfilter *cf,
                                      struct Curl_multi *multi,
                                      cf_quiche_svisit *do_cb,
                                      void *user_data)
{
  struct cf_quiche_ctx *ctx = cf->ctx;
  struct cf_quiche_visit_ctx vctx;
  vctx.cf = cf;
  vctx.multi = multi;
  vctx.cb = do_cb;
  vctx.user_data = user_data;
  Curl_u32_ptrset_visit(&ctx->streams, cf_quiche_stream_do, &vctx);
}

static bool cf_quiche_do_resume(struct Curl_cfilter *cf,
                                struct Curl_easy *sdata,
                                struct h3_stream_ctx *stream,
                                void *user_data)
{
  (void)user_data;
  nếu(stream->quic_flow_blocked) {
    stream->quic_flow_blocked = FALSE;
    Curl_multi_mark_dirty(sdata);
    CURL_TRC_CF(sdata, cf, "[%" PRIu64 "] unblock", stream->id);
  }
  trả_về TRUE;
}

static bool cf_quiche_do_expire(struct Curl_cfilter *cf,
                                struct Curl_easy *sdata,
                                struct h3_stream_ctx *stream,
                                void *user_data)
{
  (void)user_data;
  CURL_TRC_CF(sdata, cf, "conn closed, mark as dirty");
  stream->xfer_result = CURLE_SEND_ERROR;
  Curl_multi_mark_dirty(sdata);
  trả_về TRUE;
}

static CURLcode h3_data_setup(struct Curl_cfilter *cf,
                              struct Curl_easy *data)
{
  struct cf_quiche_ctx *ctx = cf->ctx;
  struct h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  nếu(stream)
    trả_về CURLE_OK;

  stream = curlx_calloc(1, sizeof(*stream));
  nếu(!stream)
    trả_về CURLE_OUT_OF_MEMORY;

  stream->id = -1;
  Curl_h1_req_parse_init(&stream->h1, H1_PARSE_DEFAULT_MAX_LINE_LEN);

  nếu(Curl_u32_ptrset_set(&ctx->streams, data->mid, stream)) {
    h3_stream_ctx_free(stream);
    trả_về CURLE_OUT_OF_MEMORY;
  }

  trả_về CURLE_OK;
}

static void cf_quiche_stream_close(struct Curl_cfilter *cf,
                                   struct Curl_easy *data,
                                   struct h3_stream_ctx *stream)
{
  struct cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result;

  nếu(ctx->qconn && !stream->closed) {
    quiche_conn_stream_shutdown(ctx->qconn, stream->id,
                                QUICHE_SHUTDOWN_READ, CURL_H3_NO_ERROR);
    nếu(!stream->send_closed) {
      quiche_conn_stream_shutdown(ctx->qconn, stream->id,
                                  QUICHE_SHUTDOWN_WRITE, CURL_H3_NO_ERROR);
      stream->send_closed = TRUE;
    }
    stream->closed = TRUE;
    result = cf_flush_egress(cf, data);
    nếu(result)
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] stream close, flush egress -> %d",
                  stream->id, (số_nguyên)result);
  }
}

static void h3_data_done(struct Curl_cfilter *cf, struct Curl_easy *data)
{
  struct cf_quiche_ctx *ctx = cf->ctx;
  struct h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  (void)cf;
  nếu(stream) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] easy handle is done", stream->id);
    cf_quiche_stream_close(cf, data, stream);
    Curl_u32_ptrset_unset(&ctx->streams, data->mid);
  }
}

static void cf_quiche_expire_conn_closed(struct Curl_cfilter *cf,
                                         struct Curl_easy *data)
{
  DEBUGASSERT(data->multi);
  CURL_TRC_CF(data, cf, "conn closed, expire all transfers");
  cf_quiche_for_all_streams(cf, data->multi, cf_quiche_do_expire, NULL);
}

static void cf_quiche_write_hd(struct Curl_cfilter *cf,
                               struct Curl_easy *data,
                               struct h3_stream_ctx *stream,
                               const char *buf, size_t blen, bool eos)
{
  /* This function returns no error intentionally, but records
   * the result at the stream, skipping further writes once the
   * `result` of the transfer is known.
   * The stream is subsequently cancelled "higher up" in the filter's
   * send/recv callbacks. Closing the stream here leads to SEND/RECV
   * errors in other places that then overwrite the transfer's result. */
  nếu(!stream->xfer_result) {
    stream->xfer_result = Curl_xfer_write_resp_hd(data, buf, blen, eos);
    nếu(stream->xfer_result)
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] error %d writing %zu "
                  "bytes of headers", stream->id, (số_nguyên)stream->xfer_result,
                  blen);
  }
}

struct cb_ctx {
  struct Curl_cfilter *cf;
  struct Curl_easy *data;
  struct h3_stream_ctx *stream;
};

static bool is_valid_h3_header(const uint8_t *hdr, size_t hlen)
{
  trong_khi(hlen--) {
    switch(*hdr++) {
    case '\n':
    case '\r':
    case '\0':
      trả_về FALSE;
    }
  }
  trả_về TRUE;
}

static số_nguyên cb_each_header(uint8_t *name, size_t name_len,
                          uint8_t *value, size_t value_len,
                          void *argp)
{
  struct cb_ctx *x = argp;
  struct Curl_cfilter *cf = x->cf;
  struct Curl_easy *data = x->data;
  struct h3_stream_ctx *stream = x->stream;
  struct cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  nếu(!stream || stream->xfer_result)
    trả_về 1; /* abort iteration */

  nếu((name_len == 7) && !strncmp(HTTP_PSEUDO_STATUS, (char *)name, 7) &&
     is_valid_h3_header(value, value_len)) {
    curlx_dyn_reset(&ctx->h1hdr);
    result = Curl_http_decode_status(&stream->status_code,
                                     (const char *)value, value_len);
    nếu(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, STRCONST("HTTP/3 "));
    nếu(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, (const char *)value, value_len);
    nếu(!result)
      result = curlx_dyn_addn(&ctx->h1hdr, STRCONST(" \r\n"));
    nếu(!result)
      cf_quiche_write_hd(cf, data, stream, curlx_dyn_ptr(&ctx->h1hdr),
                         curlx_dyn_len(&ctx->h1hdr), FALSE);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] status: %s",
                stream->id, curlx_dyn_ptr(&ctx->h1hdr));
  }
  không_thì {
    nếu(is_valid_h3_header(value, value_len) &&
       is_valid_h3_header(name, name_len)) {
      /* store as an HTTP1-style header */
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] header: %.*s: %.*s",
                  stream->id, (số_nguyên)name_len, name,
                  (số_nguyên)value_len, value);
      curlx_dyn_reset(&ctx->h1hdr);
      result = curlx_dyn_addn(&ctx->h1hdr, (const char *)name, name_len);
      nếu(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, STRCONST(": "));
      nếu(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, (const char *)value, value_len);
      nếu(!result)
        result = curlx_dyn_addn(&ctx->h1hdr, STRCONST("\r\n"));
      nếu(!result)
        cf_quiche_write_hd(cf, data, stream, curlx_dyn_ptr(&ctx->h1hdr),
                           curlx_dyn_len(&ctx->h1hdr), FALSE);
    }
    không_thì
      CURL_TRC_CF(x->data, x->cf, "[%" PRIu64 "] ignore %zu bytes bad header",
                  stream->id, value_len + name_len);
  }

  nếu(result) {
    CURL_TRC_CF(x->data, x->cf, "[%" PRIu64 "] on header error %d",
                stream->id, (số_nguyên)result);
    nếu(!stream->xfer_result)
      stream->xfer_result = result;
  }
  trả_về stream->xfer_result ? 1 : 0;
}

static CURLcode stream_resp_read(void *reader_ctx,
                                 unsigned char *buf, size_t len,
                                 size_t *pnread)
{
  struct cb_ctx *x = reader_ctx;
  struct cf_quiche_ctx *ctx = x->cf->ctx;
  struct h3_stream_ctx *stream = H3_STREAM_CTX(ctx, x->data);
  ssize_t nread;

  *pnread = 0;
  nếu(!stream)
    trả_về CURLE_RECV_ERROR;

  nread = quiche_h3_recv_body(ctx->h3c, ctx->qconn, stream->id, buf, len);
  nếu(!curlx_sztouz(nread, pnread))
    trả_về CURLE_AGAIN;
  trả_về CURLE_OK;
}

static void cf_quiche_flush_body(struct Curl_cfilter *cf,
                                 struct Curl_easy *data,
                                 struct h3_stream_ctx *stream)
{
  struct cf_quiche_ctx *ctx = cf->ctx;
  const uint8_t *buf;
  size_t blen;

  trong_khi(stream && !stream->xfer_result) {
    nếu(Curl_bufq_peek(&ctx->writebuf, &buf, &blen)) {
      stream->xfer_result = Curl_xfer_write_resp(
        data, (const char *)buf, blen, FALSE);
      Curl_bufq_skip(&ctx->writebuf, blen);
      nếu(stream->xfer_result) {
        CURL_TRC_CF(data, cf, "[%" PRIu64 "] error %d writing %zu bytes"
                    " of data", stream->id, (số_nguyên)stream->xfer_result, blen);
      }
    }
    không_thì
      break;
  }
  Curl_bufq_reset(&ctx->writebuf);
}

static void cf_quiche_recv_body(struct Curl_cfilter *cf,
                                struct Curl_easy *data,
                                struct h3_stream_ctx *stream)
{
  struct cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  struct cb_ctx cb_ctx;
  CURLcode result = CURLE_OK;

  nếu(!stream)
    trả_về;

  /* Even when the transfer has already errored, we need to receive
   * the data from quiche, as quiche otherwise gets stuck and
   * raise events to receive over and over again. */
  cb_ctx.cf = cf;
  cb_ctx.data = data;
  cb_ctx.stream = stream;
  Curl_bufq_reset(&ctx->writebuf);
  trong_khi(!result) {
    result = Curl_bufq_slurp(&ctx->writebuf,
                             stream_resp_read, &cb_ctx, &nread);
    nếu(!result)
      cf_quiche_flush_body(cf, data, stream);
    không_thì nếu(result == CURLE_AGAIN)
      break;
    không_thì nếu(result) {
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] recv_body error %d",
                  stream->id, (số_nguyên)result);
      failf(data, "[%" PRIu64 "] Error %d in HTTP/3 response body for stream",
            stream->id, (số_nguyên)result);
      stream->closed = TRUE;
      stream->reset = TRUE;
      stream->send_closed = TRUE;
      nếu(!stream->xfer_result)
        stream->xfer_result = result;
    }
  }
  cf_quiche_flush_body(cf, data, stream);
}

static void cf_quiche_process_ev(struct Curl_cfilter *cf,
                                 struct Curl_easy *data,
                                 struct h3_stream_ctx *stream,
                                 quiche_h3_event *ev)
{
  enum quiche_h3_event_type type;

  nếu(!stream)
    trả_về;

  type = quiche_h3_event_type(ev);

  switch(type) {
  case QUICHE_H3_EVENT_HEADERS: {
    struct cb_ctx cb_ctx;
    stream->resp_got_header = TRUE;
    cb_ctx.cf = cf;
    cb_ctx.data = data;
    cb_ctx.stream = stream;
    quiche_h3_event_for_each_header(ev, cb_each_header, &cb_ctx);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [HEADERS]", stream->id);
    Curl_multi_mark_dirty(data);
    break;
  }
  case QUICHE_H3_EVENT_DATA:
    nếu(!stream->resp_hds_complete) {
      stream->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, data, stream, "\r\n", 2, FALSE);
    }
    cf_quiche_recv_body(cf, data, stream);
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [DATA]", stream->id);
    Curl_multi_mark_dirty(data);
    break;

  case QUICHE_H3_EVENT_RESET:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] RESET", stream->id);
    stream->closed = TRUE;
    stream->reset = TRUE;
    stream->send_closed = TRUE;
    Curl_multi_mark_dirty(data);
    break;

  case QUICHE_H3_EVENT_FINISHED:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] CLOSED", stream->id);
    nếu(!stream->resp_hds_complete) {
      stream->resp_hds_complete = TRUE;
      cf_quiche_write_hd(cf, data, stream, "\r\n", 2, TRUE);
    }
    stream->closed = TRUE;
    Curl_multi_mark_dirty(data);
    break;

  case QUICHE_H3_EVENT_GOAWAY:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] <- [GOAWAY]", stream->id);
    break;

  default:
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] recv, unhandled event %d",
                stream->id, (số_nguyên)type);
    break;
  }
}

struct cf_quiche_disp_ctx {
  uint64_t stream_id;
  struct Curl_cfilter *cf;
  struct Curl_multi *multi;
  quiche_h3_event *ev;
};

static bool cf_quiche_disp_event(uint32_t mid, void *val, void *user_data)
{
  struct cf_quiche_disp_ctx *dctx = user_data;
  struct h3_stream_ctx *stream = val;

  nếu(stream->id == dctx->stream_id) {
    struct Curl_easy *sdata = Curl_multi_get_easy(dctx->multi, mid);
    nếu(sdata)
      cf_quiche_process_ev(dctx->cf, sdata, stream, dctx->ev);
    trả_về FALSE; /* stop iterating */
  }
  trả_về TRUE;
}

static CURLcode cf_poll_events(struct Curl_cfilter *cf,
                               struct Curl_easy *data)
{
  struct cf_quiche_ctx *ctx = cf->ctx;
  struct h3_stream_ctx *stream = NULL;
  quiche_h3_event *ev;

  /* Take in the events and distribute them to the transfers. */
  trong_khi(ctx->h3c) {
    int64_t rv = quiche_h3_conn_poll(ctx->h3c, ctx->qconn, &ev);
    nếu(rv == QUICHE_H3_ERR_DONE) {
      break;
    }
    không_thì nếu(rv < 0) {
      CURL_TRC_CF(data, cf, "error poll: %" PRId64, rv);
      trả_về CURLE_HTTP3;
    }
    không_thì {
      stream = H3_STREAM_CTX(ctx, data);
      nếu(stream && stream->id == (uint64_t)rv) {
        /* event for calling transfer */
        cf_quiche_process_ev(cf, data, stream, ev);
        quiche_h3_event_free(ev);
        nếu(stream->xfer_result)
          trả_về stream->xfer_result;
      }
      không_thì {
        /* another transfer, do not return errors, as they are not for
         * the calling transfer */
        struct cf_quiche_disp_ctx dctx;
        dctx.stream_id = (uint64_t)rv;
        dctx.cf = cf;
        dctx.multi = data->multi;
        dctx.ev = ev;
        Curl_u32_ptrset_visit(&ctx->streams, cf_quiche_disp_event, &dctx);
        quiche_h3_event_free(ev);
      }
    }
  }
  trả_về CURLE_OK;
}

struct recv_ctx {
  struct Curl_cfilter *cf;
  struct Curl_easy *data;
  số_nguyên pkts;
};

static CURLcode cf_quiche_recv_pkts(const unsigned char *buf, size_t buflen,
                                    size_t gso_size,
                                    struct sockaddr_storage *remote_addr,
                                    socklen_t remote_addrlen, uint8_t ecn,
                                    void *userp)
{
  struct recv_ctx *r = userp;
  struct cf_quiche_ctx *ctx = r->cf->ctx;
  quiche_recv_info recv_info;
  size_t pktlen, offset, nread;
  ssize_t rv;

  (void)ecn;

  recv_info.to = (struct sockaddr *)&ctx->q.local_addr;
  recv_info.to_len = ctx->q.local_addrlen;
  recv_info.from = (struct sockaddr *)remote_addr;
  recv_info.from_len = remote_addrlen;

  cho(offset = 0; offset < buflen; offset += gso_size) {
    pktlen = ((offset + gso_size) <= buflen) ? gso_size : (buflen - offset);
    rv = quiche_conn_recv(ctx->qconn,
                          (unsigned char *)CURL_UNCONST(buf + offset),
                          pktlen, &recv_info);
    nếu(!curlx_sztouz(rv, &nread)) {
      nếu(QUICHE_ERR_DONE == rv) {
        nếu(quiche_conn_is_draining(ctx->qconn)) {
          CURL_TRC_CF(r->data, r->cf, "ingress, connection is draining");
          trả_về CURLE_RECV_ERROR;
        }
        nếu(quiche_conn_is_closed(ctx->qconn)) {
          CURL_TRC_CF(r->data, r->cf, "ingress, connection is closed");
          trả_về CURLE_RECV_ERROR;
        }
        CURL_TRC_CF(r->data, r->cf, "ingress, quiche is DONE");
        trả_về CURLE_OK;
      }
      không_thì nếu(QUICHE_ERR_TLS_FAIL == rv) {
        long verify_ok = SSL_get_verify_result(ctx->tls.ossl.ssl);
        nếu(verify_ok != X509_V_OK) {
          failf(r->data, "SSL certificate problem: %s",
                X509_verify_cert_error_string(verify_ok));
          trả_về CURLE_PEER_FAILED_VERIFICATION;
        }
        failf(r->data, "ingress, quiche reports TLS fail");
        trả_về CURLE_RECV_ERROR;
      }
      không_thì {
        failf(r->data, "quiche reports error %zd on receive", rv);
        trả_về CURLE_RECV_ERROR;
      }
    }
    không_thì nếu(nread < pktlen) {
      CURL_TRC_CF(r->data, r->cf, "ingress, quiche only read %zu/%zu bytes",
                  nread, pktlen);
    }
    ++r->pkts;
  }

  trả_về CURLE_OK;
}

static CURLcode cf_process_ingress(struct Curl_cfilter *cf,
                                   struct Curl_easy *data)
{
  struct cf_quiche_ctx *ctx = cf->ctx;
  struct recv_ctx rctx;
  CURLcode result;

  DEBUGASSERT(ctx->qconn);
  result = Curl_vquic_tls_before_recv(&ctx->tls, cf, data);
  nếu(result)
    trả_về result;

  rctx.cf = cf;
  rctx.data = data;
  rctx.pkts = 0;

  result = Curl_vquic_recv_packets(cf, data, &ctx->q, 1000,
                                   cf_quiche_recv_pkts, &rctx);
  nếu(result)
    trả_về result;

  nếu(rctx.pkts > 0) {
    /* quiche digested ingress packets. It might have opened flow control
     * windows again. */
    DEBUGASSERT(data->multi);
    cf_quiche_for_all_streams(cf, data->multi, cf_quiche_do_resume, NULL);
  }
  trả_về cf_poll_events(cf, data);
}

struct read_ctx {
  struct Curl_cfilter *cf;
  struct Curl_easy *data;
  quiche_send_info send_info;
};

static CURLcode read_pkt_to_send(void *userp,
                                 unsigned char *buf, size_t buflen,
                                 size_t *pnread)
{
  struct read_ctx *x = userp;
  struct cf_quiche_ctx *ctx = x->cf->ctx;
  ssize_t rv;

  *pnread = 0;
  rv = quiche_conn_send(ctx->qconn, buf, buflen, &x->send_info);
  nếu(rv == QUICHE_ERR_DONE)
    trả_về CURLE_AGAIN;

  nếu(!curlx_sztouz(rv, pnread)) {
    failf(x->data, "quiche_conn_send returned %zd", rv);
    trả_về CURLE_SEND_ERROR;
  }
  trả_về CURLE_OK;
}

/*
 * flush_egress drains the buffers and sends off data.
 * Calls failf() on errors.
 */
static CURLcode cf_flush_egress(struct Curl_cfilter *cf,
                                struct Curl_easy *data)
{
  struct cf_quiche_ctx *ctx = cf->ctx;
  size_t nread;
  CURLcode result;
  int64_t expiry_ns;
  int64_t timeout_ns;
  struct read_ctx readx;
  size_t pkt_count, gsolen;

  expiry_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  nếu(!expiry_ns) {
    quiche_conn_on_timeout(ctx->qconn);
    nếu(quiche_conn_is_closed(ctx->qconn)) {
      nếu(quiche_conn_is_timed_out(ctx->qconn))
        failf(data, "connection closed by idle timeout");
      không_thì
        failf(data, "connection closed by server");
      /* Connection timed out, expire all transfers belonging to it
       * as it does not get any more POLL events here. */
      cf_quiche_expire_conn_closed(cf, data);
      trả_về CURLE_SEND_ERROR;
    }
  }

  result = Curl_vquic_flush(cf, data, &ctx->q);
  nếu(result) {
    nếu(result == CURLE_AGAIN) {
      Curl_expire(data, 1, EXPIRE_QUIC);
      trả_về CURLE_OK;
    }
    trả_về result;
  }

  readx.cf = cf;
  readx.data = data;
  memset(&readx.send_info, 0, sizeof(readx.send_info));
  pkt_count = 0;
  gsolen = quiche_conn_max_send_udp_payload_size(ctx->qconn);
  cho(;;) {
    /* add the next packet to send, if any, to our buffer */
    result = Curl_bufq_sipn(&ctx->q.sendbuf, 0,
                            read_pkt_to_send, &readx, &nread);
    nếu(result) {
      nếu(result != CURLE_AGAIN)
        trả_về result;
      /* Nothing more to add, flush and leave */
      result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
      nếu(result) {
        nếu(result == CURLE_AGAIN) {
          Curl_expire(data, 1, EXPIRE_QUIC);
          trả_về CURLE_OK;
        }
        trả_về result;
      }
      goto out;
    }

    ++pkt_count;
    nếu(nread < gsolen || pkt_count >= MAX_PKT_BURST) {
      result = Curl_vquic_send(cf, data, &ctx->q, gsolen);
      nếu(result) {
        nếu(result == CURLE_AGAIN) {
          Curl_expire(data, 1, EXPIRE_QUIC);
          trả_về CURLE_OK;
        }
        goto out;
      }
      pkt_count = 0;
    }
  }

out:
  timeout_ns = quiche_conn_timeout_as_nanos(ctx->qconn);
  nếu(timeout_ns % 1000000)
    timeout_ns += 1000000;
  /* expire resolution is milliseconds */
  Curl_expire(data, (timeout_ns / 1000000), EXPIRE_QUIC);
  trả_về result;
}

static CURLcode recv_closed_stream(struct Curl_cfilter *cf,
                                   struct Curl_easy *data,
                                   size_t *pnread)
{
  struct cf_quiche_ctx *ctx = cf->ctx;
  struct h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result = CURLE_OK;

  DEBUGASSERT(stream);
  *pnread = 0;
  nếu(stream->reset) {
    nếu(stream->error3 == CURL_H3_ERR_REQUEST_REJECTED) {
      infof(data, "HTTP/3 stream %" PRIu64 " refused by server, try again "
            "on a new connection", stream->id);
      connclose(cf->conn); /* do not use this anymore */
      data->state.refused_stream = TRUE;
      trả_về CURLE_RECV_ERROR; /* trigger Curl_retry_request() later */
    }
    không_thì nếu(stream->resp_hds_complete && data->req.no_body) {
        CURL_TRC_CF(data, cf, "[%" PRIu64 "] error after response headers, "
                    "but we did not want a body anyway, ignore error 0x%"
                    PRIx64 " %s", stream->id, stream->error3,
                    Curl_vquic_h3_err_str(stream->error3));
        trả_về CURLE_OK;
    }
    failf(data, "HTTP/3 stream %" PRIu64 " reset by server (error 0x%" PRIx64
          " %s)", stream->id, stream->error3,
          Curl_vquic_h3_err_str(stream->error3));
    result = data->req.bytecount ? CURLE_PARTIAL_FILE : CURLE_HTTP3;
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_recv, was reset -> %d",
                stream->id, (số_nguyên)result);
  }
  không_thì nếu(!stream->resp_got_header) {
    failf(data, "HTTP/3 stream %" PRIu64 " was closed cleanly, but before "
          "getting all response header fields, treated as error",
          stream->id);
    result = CURLE_HTTP3;
  }
  trả_về result;
}

static CURLcode cf_quiche_recv(struct Curl_cfilter *cf, struct Curl_easy *data,
                               char *buf, size_t blen, size_t *pnread)
{
  struct cf_quiche_ctx *ctx = cf->ctx;
  struct h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result = CURLE_OK;

  *pnread = 0;
  (void)buf;
  (void)blen;
  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  nếu(!stream)
    trả_về CURLE_RECV_ERROR;

  result = cf_process_ingress(cf, data);
  nếu(result) {
    CURL_TRC_CF(data, cf, "cf_recv, error on ingress");
    goto out;
  }

  nếu(stream->xfer_result) {
    cf_quiche_stream_close(cf, data, stream);
    result = stream->xfer_result;
    goto out;
  }
  không_thì nếu(stream->closed)
    result = recv_closed_stream(cf, data, pnread);
  không_thì nếu(quiche_conn_is_draining(ctx->qconn)) {
    failf(data, "QUIC connection is draining");
    result = CURLE_HTTP3;
  }
  không_thì
    result = CURLE_AGAIN;

out:
  result = Curl_1st_fatal(result, cf_flush_egress(cf, data));
  nếu(*pnread > 0)
    ctx->data_recvd += *pnread;
  CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_recv(len=%zu) -> %d, %zu, total=%"
              FMT_OFF_T, stream->id, blen, (số_nguyên)result, *pnread,
              ctx->data_recvd);
  trả_về result;
}

static CURLcode cf_quiche_send_body(struct Curl_cfilter *cf,
                                    struct Curl_easy *data,
                                    struct h3_stream_ctx *stream,
                                    const uint8_t *buf, size_t len, bool eos,
                                    size_t *pnwritten)
{
  struct cf_quiche_ctx *ctx = cf->ctx;
  ssize_t rv;

  *pnwritten = 0;
  rv = quiche_h3_send_body(ctx->h3c, ctx->qconn, stream->id,
                           (uint8_t *)CURL_UNCONST(buf), len, eos);
  nếu(rv == QUICHE_H3_ERR_DONE || (rv == 0 && len > 0)) {
    /* Blocked on flow control and should HOLD sending.
       When do we open again? */
    nếu(!quiche_conn_stream_writable(ctx->qconn, stream->id, len)) {
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                  "-> window exhausted", stream->id, len);
      stream->quic_flow_blocked = TRUE;
    }
    trả_về CURLE_AGAIN;
  }
  không_thì nếu(rv == QUICHE_H3_TRANSPORT_ERR_INVALID_STREAM_STATE) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> invalid stream state", stream->id, len);
    trả_về CURLE_HTTP3;
  }
  không_thì nếu(rv == QUICHE_H3_TRANSPORT_ERR_FINAL_SIZE) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) -> exceeds size",
                stream->id, len);
    trả_về CURLE_SEND_ERROR;
  }
  không_thì nếu(!curlx_sztouz(rv, pnwritten)) {
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) -> quiche err %zd",
                stream->id, len, rv);
    trả_về CURLE_SEND_ERROR;
  }
  không_thì {
    nếu(eos && (len == *pnwritten))
      stream->send_closed = TRUE;
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send body(len=%zu, eos=%d) -> %zu",
                stream->id, len, stream->send_closed, *pnwritten);
    trả_về CURLE_OK;
  }
}

static CURLcode h3_open_stream(struct Curl_cfilter *cf,
                               struct Curl_easy *data,
                               const uint8_t *buf, size_t blen, bool eos,
                               size_t *pnwritten)
{
  struct cf_quiche_ctx *ctx = cf->ctx;
  struct h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  size_t nheader, i;
  int64_t rv;
  struct dynhds h2_headers;
  quiche_h3_header *nva = NULL;
  CURLcode result = CURLE_OK;

  *pnwritten = 0;
  nếu(!stream) {
    result = h3_data_setup(cf, data);
    nếu(result)
      trả_về result;
    stream = H3_STREAM_CTX(ctx, data);
    DEBUGASSERT(stream);
  }

  Curl_dynhds_init(&h2_headers, 0, DYN_HTTP_REQUEST);

  DEBUGASSERT(stream);

  result = Curl_h1_req_parse_read(&stream->h1, buf, blen, NULL,
    !data->state.http_ignorecustom ?
    CURL_EASY_STR(data, STRING_CUSTOMREQUEST) : NULL,
    0, pnwritten);
  nếu(result)
    goto out;
  nếu(!stream->h1.done) {
    /* need more data */
    goto out;
  }
  DEBUGASSERT(stream->h1.req);

  result = Curl_http_req_to_h2(&h2_headers, stream->h1.req, data);
  nếu(result)
    goto out;

  /* no longer needed */
  Curl_h1_req_parse_free(&stream->h1);

  nheader = Curl_dynhds_count(&h2_headers);
  nva = curlx_malloc(sizeof(quiche_h3_header) * nheader);
  nếu(!nva) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  cho(i = 0; i < nheader; ++i) {
    struct dynhds_entry *e = Curl_dynhds_getn(&h2_headers, i);
    nva[i].name = (unsigned char *)e->name;
    nva[i].name_len = e->namelen;
    nva[i].value = (unsigned char *)e->value;
    nva[i].value_len = e->valuelen;
  }

  buf += *pnwritten;
  blen -= *pnwritten;

  nếu(eos && !blen)
    stream->send_closed = TRUE;

  rv = quiche_h3_send_request(ctx->h3c, ctx->qconn, nva, nheader,
                              stream->send_closed);
  CURL_TRC_CF(data, cf, "quiche_send_request() -> %" PRId64, rv);
  nếu(rv < 0) {
    nếu(QUICHE_H3_ERR_STREAM_BLOCKED == rv) {
      /* quiche seems to report this error if the connection window is
       * exhausted. Which happens frequently and intermittent. */
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] blocked", stream->id);
      stream->quic_flow_blocked = TRUE;
      result = CURLE_AGAIN;
      goto out;
    }
    không_thì {
      CURL_TRC_CF(data, cf, "send_request(%s) -> %" PRId64,
                  Curl_bufref_ptr(&data->state.url), rv);
    }
    result = CURLE_SEND_ERROR;
    goto out;
  }

  DEBUGASSERT(!stream->opened);
  stream->id = (uint64_t)rv;
  stream->opened = TRUE;
  stream->closed = FALSE;
  stream->reset = FALSE;

  nếu(Curl_trc_is_verbose(data)) {
    infof(data, "[HTTP/3] [%" PRIu64 "] OPENED stream for %s",
          stream->id, Curl_bufref_ptr(&data->state.url));
    cho(i = 0; i < nheader; ++i) {
      infof(data, "[HTTP/3] [%" PRIu64 "] [%.*s: %.*s]", stream->id,
            (số_nguyên)nva[i].name_len, nva[i].name,
            (số_nguyên)nva[i].value_len, nva[i].value);
    }
  }

  nếu(blen) {  /* after the headers, there was request BODY data */
    size_t nwritten;
    CURLcode r2 = CURLE_OK;

    r2 = cf_quiche_send_body(cf, data, stream, buf, blen, eos, &nwritten);
    nếu(r2 && (r2 != CURLE_AGAIN)) {  /* real error, fail */
      result = r2;
    }
    không_thì nếu(nwritten > 0) {
      *pnwritten += nwritten;
    }
  }

out:
  curlx_free(nva);
  Curl_dynhds_free(&h2_headers);
  trả_về result;
}

static CURLcode cf_quiche_send(struct Curl_cfilter *cf, struct Curl_easy *data,
                               const uint8_t *buf, size_t len, bool eos,
                               size_t *pnwritten)
{
  struct cf_quiche_ctx *ctx = cf->ctx;
  struct h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
  CURLcode result;

  *pnwritten = 0;
  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  result = cf_process_ingress(cf, data);
  nếu(result)
    goto out;

  nếu(!stream || !stream->opened) {
    result = h3_open_stream(cf, data, buf, len, eos, pnwritten);
    nếu(result)
      goto out;
    stream = H3_STREAM_CTX(ctx, data);
  }
  không_thì nếu(stream->xfer_result) {
    cf_quiche_stream_close(cf, data, stream);
    result = stream->xfer_result;
  }
  không_thì nếu(stream->closed) {
    nếu(stream->resp_hds_complete) {
      /* sending request body on a stream that has been closed by the
       * server. If the server has send us a final response, we should
       * silently discard the send data.
       * This happens for example on redirects where the server, instead
       * of reading the full request body closed the stream after
       * sending the 30x response.
       * This is sort of a race: had the transfer loop called recv first,
       * it would see the response and stop/discard sending on its own- */
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] discarding data "
                  "on closed stream with response", stream->id);
      result = CURLE_OK;
      *pnwritten = len;
      goto out;
    }
    CURL_TRC_CF(data, cf, "[%" PRIu64 "] send_body(len=%zu) "
                "-> stream closed", stream->id, len);
    result = CURLE_HTTP3;
    goto out;
  }
  không_thì {
    result = cf_quiche_send_body(cf, data, stream, buf, len, eos, pnwritten);
  }

out:
  result = Curl_1st_fatal(result, cf_flush_egress(cf, data));

  CURL_TRC_CF(data, cf, "[%" PRIu64 "] cf_send(len=%zu) -> %d, %zu",
              stream ? stream->id : (uint64_t)~0, len,
              (số_nguyên)result, *pnwritten);
  trả_về result;
}

static bool stream_is_writable(struct Curl_cfilter *cf,
                               struct Curl_easy *data)
{
  struct cf_quiche_ctx *ctx = cf->ctx;
  struct h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);

  trả_về stream && (quiche_conn_stream_writable(
    ctx->qconn, stream->id, 1) > 0);
}

static CURLcode cf_quiche_adjust_pollset(struct Curl_cfilter *cf,
                                         struct Curl_easy *data,
                                         struct easy_pollset *ps)
{
  struct cf_quiche_ctx *ctx = cf->ctx;
  bool want_recv, want_send;
  CURLcode result = CURLE_OK;

  nếu(!ctx->qconn)
    trả_về CURLE_OK;

  Curl_pollset_check(data, ps, ctx->q.sockfd, &want_recv, &want_send);
  nếu(want_recv || want_send) {
    struct h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    bool c_exhaust, s_exhaust;

    c_exhaust = FALSE; /* Have not found any call in quiche that tells
                          us if the connection itself is blocked */
    s_exhaust = want_send && stream && stream->opened &&
                (stream->quic_flow_blocked || !stream_is_writable(cf, data));
    want_recv = (want_recv || c_exhaust || s_exhaust);
    want_send = (!s_exhaust && want_send) ||
                 !Curl_bufq_is_empty(&ctx->q.sendbuf);

    result = Curl_pollset_set(data, ps, ctx->q.sockfd, want_recv, want_send);
  }
  trả_về result;
}

static CURLcode h3_data_pause(struct Curl_cfilter *cf,
                              struct Curl_easy *data,
                              bool pause)
{
  /* There seems to exist no API in quiche to shrink/enlarge the streams
   * windows. As we do in HTTP/2. */
  (void)cf;
  nếu(!pause) {
    Curl_multi_mark_dirty(data);
  }
  trả_về CURLE_OK;
}

static CURLcode cf_quiche_cntrl(struct Curl_cfilter *cf,
                                struct Curl_easy *data,
                                số_nguyên event, số_nguyên arg1, void *arg2)
{
  struct cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  (void)arg1;
  (void)arg2;
  switch(event) {
  case CF_CTRL_DATA_SETUP:
    break;
  case CF_CTRL_DATA_PAUSE:
    result = h3_data_pause(cf, data, (arg1 != 0));
    break;
  case CF_CTRL_DATA_DONE:
    h3_data_done(cf, data);
    break;
  case CF_CTRL_DATA_DONE_SEND: {
    struct h3_stream_ctx *stream = H3_STREAM_CTX(ctx, data);
    nếu(stream && !stream->send_closed) {
      unsigned char body[1];
      size_t sent;

      stream->send_closed = TRUE;
      body[0] = 'X';
      result = cf_quiche_send(cf, data, body, 0, TRUE, &sent);
      CURL_TRC_CF(data, cf, "[%" PRIu64 "] DONE_SEND -> %d, %zu",
                  stream->id, (số_nguyên)result, sent);
    }
    break;
  }
  case CF_CTRL_CONN_INFO_UPDATE:
    nếu(!cf->sockindex && cf->connected) {
      cf->conn->httpversion_seen = 30;
      Curl_conn_set_multiplex(cf->conn);
    }
    break;
  case CF_CTRL_REPORT_STATS:
    nếu(cf->connected && !ctx->stats_reported &&
       (ctx->handshake_at.tv_sec || ctx->handshake_at.tv_usec)) {
      Curl_pgrsTimeWas(data, TIMER_CONNECT, ctx->q.first_byte_at);
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, ctx->handshake_at);
      ctx->stats_reported = TRUE;
    }
    break;
  default:
    break;
  }
  trả_về result;
}

static CURLcode cf_quiche_ctx_open(struct Curl_cfilter *cf,
                                   struct Curl_easy *data)
{
  struct cf_quiche_ctx *ctx = cf->ctx;
  số_nguyên rv;
  CURLcode result;
  const struct Curl_sockaddr_ex *sockaddr;
  static const struct alpn_spec ALPN_SPEC_H3 = { { "h3" }, 1 };

  DEBUGASSERT(ctx->q.sockfd != CURL_SOCKET_BAD);
  DEBUGASSERT(ctx->initialized);

  result = Curl_vquic_ctx_init(data, &ctx->q);
  nếu(result)
    trả_về result;

  ctx->cfg = quiche_config_new(QUICHE_PROTOCOL_VERSION);
  nếu(!ctx->cfg) {
    failf(data, "cannot create quiche config");
    trả_về CURLE_FAILED_INIT;
  }
  quiche_config_enable_pacing(ctx->cfg, FALSE);
  quiche_config_set_initial_max_data(ctx->cfg, (1 * 1024 * 1024)
    /* (QUIC_MAX_STREAMS/2) * H3_STREAM_WINDOW_SIZE */);
  quiche_config_set_initial_max_streams_bidi(ctx->cfg, QUIC_MAX_STREAMS);
  quiche_config_set_initial_max_streams_uni(ctx->cfg, QUIC_MAX_STREAMS);
  quiche_config_set_initial_max_stream_data_bidi_local(ctx->cfg,
    H3_STREAM_WINDOW_SIZE);
  quiche_config_set_initial_max_stream_data_bidi_remote(ctx->cfg,
    H3_STREAM_WINDOW_SIZE);
  quiche_config_set_initial_max_stream_data_uni(ctx->cfg,
    H3_STREAM_WINDOW_SIZE);
  quiche_config_set_disable_active_migration(ctx->cfg, TRUE);

  quiche_config_set_max_connection_window(ctx->cfg,
    10 * QUIC_MAX_STREAMS * H3_STREAM_WINDOW_SIZE);
  quiche_config_set_max_stream_window(ctx->cfg, 10 * H3_STREAM_WINDOW_SIZE);
  quiche_config_set_application_protos(ctx->cfg,
                      (uint8_t *)CURL_UNCONST(QUICHE_H3_APPLICATION_PROTOCOL),
                                 CURL_CSTRLEN(QUICHE_H3_APPLICATION_PROTOCOL));

  result = Curl_vquic_tls_init(&ctx->tls, cf, data, &ctx->ssl_peer,
                               &ALPN_SPEC_H3, NULL, NULL, cf, NULL);
  nếu(result)
    trả_về result;

  result = Curl_rand(data, ctx->scid, sizeof(ctx->scid));
  nếu(result)
    trả_về result;

  nếu(Curl_cf_socket_peek(cf->next, data, &ctx->q.sockfd, &sockaddr, NULL))
    trả_về CURLE_QUIC_CONNECT_ERROR;

  ctx->q.local_addrlen = sizeof(ctx->q.local_addr);
  rv = getsockname(ctx->q.sockfd, (struct sockaddr *)&ctx->q.local_addr,
                   &ctx->q.local_addrlen);
  nếu(rv == -1)
    trả_về CURLE_QUIC_CONNECT_ERROR;

  ctx->qconn = quiche_conn_new_with_tls((const uint8_t *)ctx->scid,
                                        sizeof(ctx->scid), NULL, 0,
                                        (struct sockaddr *)&ctx->q.local_addr,
                                        ctx->q.local_addrlen,
                                        &sockaddr->curl_sa_addr,
                                        sockaddr->addrlen,
                                        ctx->cfg, ctx->tls.ossl.ssl, FALSE);
  nếu(!ctx->qconn) {
    failf(data, "cannot create quiche connection");
    trả_về CURLE_OUT_OF_MEMORY;
  }

  /* Known to not work on Windows */
#nếu !defined(_WIN32) && defined(HAVE_QUICHE_CONN_SET_QLOG_FD)
  {
    số_nguyên qfd;
    (void)Curl_qlogdir(data, ctx->scid, sizeof(ctx->scid), &qfd);
    nếu(qfd != -1)
      quiche_conn_set_qlog_fd(ctx->qconn, qfd, "qlog title", "curl qlog");
  }
#endif

  result = cf_flush_egress(cf, data);
  nếu(result)
    trả_về result;

  {
    unsigned char alpn_protocols[] = QUICHE_H3_APPLICATION_PROTOCOL;
    unsigned alpn_len, offset = 0;

    /* Replace each ALPN length prefix by a comma. */
    trong_khi(offset < CURL_CSTRLEN(alpn_protocols)) {
      alpn_len = alpn_protocols[offset];
      alpn_protocols[offset] = ',';
      offset += 1 + alpn_len;
    }

    CURL_TRC_CF(data, cf, "Sent QUIC client Initial, ALPN: %s",
                alpn_protocols + 1);
  }

  trả_về CURLE_OK;
}

static CURLcode cf_quiche_verify_peer(struct Curl_cfilter *cf,
                                      struct Curl_easy *data)
{
  struct cf_quiche_ctx *ctx = cf->ctx;
  trả_về Curl_vquic_tls_verify_peer(&ctx->tls, cf, data, &ctx->ssl_peer);
}

static CURLcode cf_quiche_connect(struct Curl_cfilter *cf,
                                  struct Curl_easy *data,
                                  bool *done)
{
  struct cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  nếu(cf->connected) {
    *done = TRUE;
    trả_về CURLE_OK;
  }

  /* Connect the UDP filter first */
  nếu(!cf->next->connected) {
    result = Curl_conn_cf_connect(cf->next, data, done);
    nếu(result || !*done)
      trả_về result;
  }

  *done = FALSE;
  nếu(Curl_ssl_need_httpsrr(cf) &&
     !Curl_conn_dns_resolved_https(data, cf->sockindex, ctx->ssl_peer.peer)) {
    CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
    trả_về CURLE_OK;
  }

  Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));

  nếu(!ctx->qconn) {
    result = cf_quiche_ctx_open(cf, data);
    nếu(result)
      goto out;
    ctx->started_at = ctx->q.last_op;
    result = cf_flush_egress(cf, data);
    /* we do not expect to be able to recv anything yet */
    goto out;
  }

  result = cf_process_ingress(cf, data);
  nếu(result)
    goto out;

  result = cf_flush_egress(cf, data);
  nếu(result)
    goto out;

  nếu(quiche_conn_is_established(ctx->qconn)) {
    ctx->handshake_at = ctx->q.last_op;
    CURL_TRC_CF(data, cf, "handshake complete after %" FMT_TIMEDIFF_T "ms",
                curlx_ptimediff_ms(&ctx->handshake_at, &ctx->started_at));
    result = cf_quiche_verify_peer(cf, data);
    nếu(!result) {
      CURL_TRC_CF(data, cf, "peer verified");
      ctx->h3config = quiche_h3_config_new();
      nếu(!ctx->h3config) {
        result = CURLE_OUT_OF_MEMORY;
        goto out;
      }
      /* quiche 0.29.3+ rejects response headers larger than 32 KiB by
         default. Allow as much as curl itself accepts. */
      quiche_h3_config_set_max_field_section_size(ctx->h3config,
                                                  MAX_HTTP_RESP_HEADER_SIZE);

      /* Create a new HTTP/3 connection on the QUIC connection. */
      ctx->h3c = quiche_h3_conn_new_with_transport(ctx->qconn, ctx->h3config);
      nếu(!ctx->h3c) {
        result = CURLE_OUT_OF_MEMORY;
        goto out;
      }
      cf->connected = TRUE;
      *done = TRUE;
    }
  }
  không_thì nếu(quiche_conn_is_draining(ctx->qconn)) {
    /* When a QUIC server instance is shutting down, it may send us a
     * CONNECTION_CLOSE right away. Our connection then enters the DRAINING
     * state. The CONNECT may work in the near future again. Indicate
     * that as a "weird" reply. */
    result = CURLE_WEIRD_SERVER_REPLY;
  }

out:
#ifdef CURLVERBOSE
  nếu(result && result != CURLE_AGAIN) {
    struct ip_quadruple ip;

    nếu(!Curl_cf_socket_peek(cf->next, data, NULL, NULL, &ip))
      infof(data, "connect to %s port %u failed: %s",
            ip.remote_ip, ip.remote_port, curl_easy_strerror(result));
    không_thì
      infof(data, "connect failed: %s", curl_easy_strerror(result));
  }
#endif
  trả_về result;
}

static CURLcode cf_quiche_shutdown(struct Curl_cfilter *cf,
                                   struct Curl_easy *data, bool *done)
{
  struct cf_quiche_ctx *ctx = cf->ctx;
  CURLcode result = CURLE_OK;

  nếu(cf->shutdown || !ctx || !ctx->qconn) {
    *done = TRUE;
    trả_về CURLE_OK;
  }

  *done = FALSE;
  nếu(!ctx->shutdown_started) {
    số_nguyên err;

    ctx->shutdown_started = TRUE;
    Curl_vquic_ctx_update_time(&ctx->q, Curl_pgrs_now(data));
    err = quiche_conn_close(ctx->qconn, TRUE, 0, NULL, 0);
    nếu(err) {
      CURL_TRC_CF(data, cf, "error %d adding shutdown packet, "
                  "aborting shutdown", err);
      result = CURLE_SEND_ERROR;
      goto out;
    }
  }

  nếu(!Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    CURL_TRC_CF(data, cf, "shutdown, flushing sendbuf");
    result = cf_flush_egress(cf, data);
    nếu(result)
      goto out;
  }

  nếu(Curl_bufq_is_empty(&ctx->q.sendbuf)) {
    /* sent everything, quiche does not seem to support a graceful shutdown
     * waiting for a reply, so we are done. */
    CURL_TRC_CF(data, cf, "shutdown completely sent off, done");
    *done = TRUE;
  }
  không_thì {
    CURL_TRC_CF(data, cf, "shutdown sending blocked");
  }

out:
  trả_về result;
}

static void cf_quiche_destroy(struct Curl_cfilter *cf, struct Curl_easy *data)
{
  (void)data;
  nếu(cf->ctx) {
    cf_quiche_ctx_close(cf->ctx);
    cf_quiche_ctx_free(cf->ctx);
    cf->ctx = NULL;
  }
}

static CURLcode cf_quiche_query(struct Curl_cfilter *cf,
                                struct Curl_easy *data,
                                số_nguyên query, số_nguyên *pres1, void *pres2)
{
  struct cf_quiche_ctx *ctx = cf->ctx;

  switch(query) {
  case CF_QUERY_MAX_CONCURRENT: {
    uint64_t max_streams = cf->conn->attached_xfers;
    nếu(!ctx->goaway && ctx->qconn) {
      max_streams += quiche_conn_peer_streams_left_bidi(ctx->qconn);
    }
    *pres1 = (max_streams > INT_MAX) ? INT_MAX : (số_nguyên)max_streams;
    CURL_TRC_CF(data, cf, "query conn[%" FMT_OFF_T "]: "
                "MAX_CONCURRENT -> %d (%u in use)",
                cf->conn->connection_id, *pres1, cf->conn->attached_xfers);
    trả_về CURLE_OK;
  }
  case CF_QUERY_CONNECT_REPLY_MS:
    nếu(ctx->q.got_first_byte) {
      timediff_t ms = curlx_ptimediff_ms(&ctx->q.first_byte_at,
                                         &ctx->started_at);
      *pres1 = (ms < INT_MAX) ? (số_nguyên)ms : INT_MAX;
    }
    không_thì
      *pres1 = -1;
    trả_về CURLE_OK;
  case CF_QUERY_HTTP_VERSION:
    *pres1 = 30;
    trả_về CURLE_OK;
  case CF_QUERY_SSL_INFO:
  case CF_QUERY_SSL_CTX_INFO: {
    struct curl_tlssessioninfo *info = pres2;
    nếu(Curl_vquic_tls_get_ssl_info(&ctx->tls,
                                   (query == CF_QUERY_SSL_CTX_INFO), info))
      trả_về CURLE_OK;
    break;
  }
  case CF_QUERY_ALPN_NEGOTIATED: {
    const char **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = cf->connected ? "h3" : NULL;
    trả_về CURLE_OK;
  }
  default:
    break;
  }
  trả_về cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

static bool cf_quiche_conn_is_alive(struct Curl_cfilter *cf,
                                    struct Curl_easy *data,
                                    bool *input_pending)
{
  struct cf_quiche_ctx *ctx = cf->ctx;
  bool alive = TRUE;

  *input_pending = FALSE;
  nếu(!ctx->qconn)
    trả_về FALSE;

  nếu(quiche_conn_is_closed(ctx->qconn)) {
    nếu(quiche_conn_is_timed_out(ctx->qconn))
      CURL_TRC_CF(data, cf, "connection was closed due to idle timeout");
    không_thì
      CURL_TRC_CF(data, cf, "connection is closed");
    trả_về FALSE;
  }

  nếu(!cf->next || !cf->next->cft->is_alive(cf->next, data, input_pending))
    trả_về FALSE;

  nếu(*input_pending) {
    /* This happens before we have sent off a request and the connection is
       not in use by any other transfer, there should not be any data here,
       only "protocol frames" */
    *input_pending = FALSE;
    nếu(cf_process_ingress(cf, data))
      alive = FALSE;
    không_thì {
      alive = TRUE;
    }
  }

  trả_về alive;
}

struct Curl_cftype Curl_cft_http3 = {
  "HTTP/3",
  CF_TYPE_IP_CONNECT | CF_TYPE_SSL | CF_TYPE_MULTIPLEX | CF_TYPE_HTTP,
  0,
  cf_quiche_destroy,
  cf_quiche_connect,
  cf_quiche_shutdown,
  cf_quiche_adjust_pollset,
  Curl_cf_def_data_pending,
  cf_quiche_send,
  cf_quiche_recv,
  cf_quiche_cntrl,
  cf_quiche_conn_is_alive,
  Curl_cf_def_conn_keep_alive,
  cf_quiche_query,
};

CURLcode Curl_cf_quiche_create(struct Curl_cfilter **pcf,
                               struct Curl_easy *data,
                               struct Curl_peer *origin,
                               struct Curl_peer *peer,
                               struct connectdata *conn,
                               struct Curl_sockaddr_ex *addr)
{
  struct cf_quiche_ctx *ctx = NULL;
  struct Curl_cfilter *cf = NULL;
  CURLcode result;

  ctx = curlx_calloc(1, sizeof(*ctx));
  nếu(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  result = cf_quiche_ctx_init(ctx, origin, peer, &conn->ssl_config);
  nếu(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  nếu(result)
    goto out;
  cf->conn = conn;

  result = Curl_cf_udp_create(&cf->next, data, origin, peer, TRNSPRT_QUIC,
                              conn, addr, NULL, TRNSPRT_QUIC);
  nếu(result)
    goto out;
  cf->next->conn = cf->conn;
  cf->next->sockindex = cf->sockindex;

out:
  *pcf = (!result) ? cf : NULL;
  nếu(result) {
    nếu(cf)
      Curl_conn_cf_discard_chain(&cf, data);
    không_thì nếu(ctx)
      cf_quiche_ctx_free(ctx);
  }

  trả_về result;
}

CURLcode Curl_cf_quiche_insert_after(struct Curl_cfilter *cf_at,
                                     struct Curl_peer *origin,
                                     struct Curl_peer *peer,
                                     struct Curl_cfilter **pcf)
{
  struct cf_quiche_ctx *ctx = NULL;
  struct Curl_cfilter *cf = NULL;
  CURLcode result;

  ctx = curlx_calloc(1, sizeof(*ctx));
  nếu(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  result = cf_quiche_ctx_init(ctx, origin, peer, &cf_at->conn->ssl_config);
  nếu(!result)
    result = Curl_cf_create(&cf, &Curl_cft_http3, ctx);
  nếu(result)
    goto out;
  Curl_conn_cf_insert_after(cf_at, cf);

out:
  nếu(result) {
    curlx_safefree(cf);
    nếu(ctx)
      cf_quiche_ctx_free(ctx);
  }
  *pcf = cf;
  trả_về result;
}

#endif
