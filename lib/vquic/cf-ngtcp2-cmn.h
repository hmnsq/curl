#ifndef HEADER_CURL_VQUIC_CF_NGTCP2_CMN_H
#define HEADER_CURL_VQUIC_CF_NGTCP2_CMN_H
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
#અન્તર્ભાવ "curl_setup.h"

#જો !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#અન્તર્ભાવ <ngtcp2/ngtcp2.h>
#અન્તર્ભાવ <ngtcp2/ngtcp2_crypto.h>

#ifdef USE_OPENSSL
#અન્તર્ભાવ <openssl/err.h>
#જો defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#અન્તર્ભાવ <ngtcp2/ngtcp2_crypto_boringssl.h>
#અથવાજો defined(OPENSSL_QUIC_API2)
#અન્તર્ભાવ <ngtcp2/ngtcp2_crypto_ossl.h>
#નહીંતર
#અન્તર્ભાવ <ngtcp2/ngtcp2_crypto_quictls.h>
#endif
#અન્તર્ભાવ "vtls/openssl.h"
#અથવાજો defined(USE_GNUTLS)
#અન્તર્ભાવ <ngtcp2/ngtcp2_crypto_gnutls.h>
#અન્તર્ભાવ "vtls/gtls.h"
#અથવાજો defined(USE_WOLFSSL)
#અન્તર્ભાવ <wolfssl/options.h>
#અન્તર્ભાવ <wolfssl/ssl.h>
#અન્તર્ભાવ <wolfssl/quic.h>
#અન્તર્ભાવ <ngtcp2/ngtcp2_crypto_wolfssl.h>
#અન્તર્ભાવ "vtls/wolfssl.h"
#endif

#ifdef HAVE_NETINET_UDP_H
#અન્તર્ભાવ <netinet/udp.h>
#endif

#અન્તર્ભાવ <nghttp3/nghttp3.h>

#અન્તર્ભાવ "http1.h"
#અન્તર્ભાવ "u32_ptrset.h"
#અન્તર્ભાવ "vtls/vtls.h"
#અન્તર્ભાવ "vquic/vquic_int.h"
#અન્તર્ભાવ "vquic/vquic-tls.h"

સંરચના Curl_cfilter;
સંરચના Curl_easy;
સંરચના cf_ngtcp2_ctx;
સંરચના cf_quic_ctx;

#define QUIC_MAX_STREAMS       (256 * 1024)
#define QUIC_HANDSHAKE_TIMEOUT (10 * NGTCP2_SECONDS)
#define QUIC_TUNNEL_INBUF_SIZE (64 * 1024)

/* We announce a small window size in transport param to the server,
 * and grow that immediately to max when no rate limit is in place.
 * We need to start small as we are not able to decrease it. */
#define H3_STREAM_WINDOW_SIZE_INITIAL (32 * 1024)
#define H3_STREAM_WINDOW_SIZE_MAX     (10 * 1024 * 1024)
#define H3_CONN_WINDOW_SIZE_MAX       (100 * H3_STREAM_WINDOW_SIZE_MAX)

#define H3_STREAM_CHUNK_SIZE  (64 * 1024)
#જો H3_STREAM_CHUNK_SIZE < NGTCP2_MAX_UDP_PAYLOAD_SIZE
#error H3_STREAM_CHUNK_SIZE smaller than NGTCP2_MAX_UDP_PAYLOAD_SIZE
#endif
/* The pool keeps spares around for all streams.
 * The benefit of the pool is that stream buffers do not keep
 * spares. Memory consumption goes down when streams run empty,
 * have a large upload done, etc. */
#define H3_STREAM_POOL_SPARES      5
/* The max amount of un-acked upload data we keep around per stream */
#define H3_STREAM_SEND_BUFFER_MAX      (10 * H3_STREAM_CHUNK_SIZE)
#define H3_STREAM_SEND_CHUNKS \
  (H3_STREAM_SEND_BUFFER_MAX / H3_STREAM_CHUNK_SIZE)
/* How much data we initially want to buffer un-acked */
#define H3_STREAM_SEND_BUF_INITIAL     (32 * 1024)

#define QUIC_TUNNEL_INGRESS_PKT_LIMIT 1000


શૂન્ય Curl_ngtcp2_ver(અક્ષર *p, size_t len);

પ્રકારવ્યાખ્યા CURLcode cf_ngtcp2_init_h3_conn(સંરચના Curl_cfilter *cf,
                                        સંરચના Curl_easy *data,
                                        સંરચના cf_ngtcp2_ctx *ctx);

સંરચના cf_ngtcp2_ctx {
  સંરચના cf_quic_ctx q;
  સંરચના ssl_peer ssl_peer;
  સંરચના curl_tls_ctx tls;
#ifdef OPENSSL_QUIC_API2
  ngtcp2_crypto_ossl_ctx *ossl_ctx;
#endif
  ngtcp2_path connected_path;
  ngtcp2_conn *qconn;
  ngtcp2_cid dcid;
  ngtcp2_cid scid;
  uint32_t version;
  ngtcp2_settings settings;
  ngtcp2_transport_params transport_params;
  ngtcp2_ccerr last_error;
  ngtcp2_crypto_conn_ref conn_ref;
  સંરચના cf_call_data call_data;
  cf_ngtcp2_init_h3_conn *init_h3_conn_cb;
  nghttp3_conn *h3conn;
  nghttp3_settings h3settings;
  સંરચના curltime started_at;       /* time the current attempt started */
  સંરચના curltime handshake_at;     /* time connect handshake finished */
  સંરચના bufc_pool stream_bufcp;    /* chunk pool for streams */
  સંરચના dynbuf scratch;            /* temp buffer for header construction */
  સંરચના u32_ptrset streams;        /* set of data->mid + h3_stream_ctx */
  uint64_t used_bidi_streams;       /* bidi streams we have opened */
  uint64_t max_bidi_streams;        /* max bidi streams we can open */
  size_t earlydata_max;             /* max amount of early data supported by
                                       server on session reuse */
  size_t earlydata_skip;            /* sending bytes to skip when earlydata
                                       is accepted by peer */
  CURLcode tls_vrfy_result;         /* result of TLS peer verification */
  પૂર્ણાંક qlogfd;
  unsigned અક્ષર *tunnel_inbuf;      /* ingress buffer for tunneled packets */
  size_t tunnel_inbuf_len;
  BIT(initialized);
  BIT(tls_handshake_complete);      /* TLS handshake is done */
  BIT(use_earlydata);               /* Using 0RTT data */
  BIT(earlydata_accepted);          /* 0RTT was accepted by server */
  BIT(shutdown_started);            /* queued shutdown packets */
  BIT(stats_reported);              /* connect statistics reported */
};

/* How to access `call_data` from a cf_ngtcp2 filter */
#undef CF_CTX_CALL_DATA
#define CF_CTX_CALL_DATA(cf) ((સંરચના cf_ngtcp2_ctx *)(cf)->ctx)->call_data

CURLcode Curl_cf_ngtcp2_ctx_init(સંરચના cf_ngtcp2_ctx *ctx,
                                 સંરચના Curl_peer *origin,
                                 સંરચના Curl_peer *peer,
                                 સંરચના ssl_filter_config *sslc,
                                 cf_ngtcp2_init_h3_conn *init_h3_conn_cb);
શૂન્ય Curl_cf_ngtcp2_ctx_cleanup(સંરચના cf_ngtcp2_ctx *ctx);
શૂન્ય Curl_cf_ngtcp2_cmn_err_set(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data, પૂર્ણાંક code);

/**
 * All about the H3 internals of a stream
 */
સંરચના h3_stream_ctx {
  int64_t id;                   /* HTTP/3 stream identifier */
  curl_off_t xfer_id;           /* `id` of transfer owning this stream */
  સંરચના bufq sendbuf;          /* h3 request body */
  સંરચના h1_req_parser h1;      /* h1 request parsing */
  uint64_t error3;              /* HTTP/3 stream error code */
  curl_off_t upload_ડાબું;       /* number of request bytes left to upload */
  curl_off_t rx_total;          /* total number of bytes received */
  uint64_t rx_offset;           /* current receive offset */
  uint64_t rx_offset_max;       /* allowed receive offset */
  size_t tx_in_flight_size;     /* sendbuf data "in flight" */
  size_t tx_in_flight_ideal;    /* ideal amount of un-acked send data */
  uint32_t mid;                 /* `mid` of transfer owning this stream */
  int32_t status_code;              /* HTTP status code */
  CURLcode xfer_result;         /* result from xfer_resp_write(_hd) */
  BIT(resp_hds_complete);       /* we have a complete, final response */
  BIT(closed);                  /* TRUE on stream close */
  BIT(reset);                   /* TRUE on stream reset */
  BIT(send_closed);             /* stream is local closed */
  BIT(quic_flow_blocked);       /* stream is blocked by QUIC flow control */
};

શૂન્ય Curl_cf_ngtcp2_h3_stream_ctx_free(સંરચના h3_stream_ctx *stream);
શૂન્ય Curl_cf_ngtcp2_h3_err_set(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data, પૂર્ણાંક code);

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(સંરચના cf_ngtcp2_ctx *ctx,
                                      સંરચના Curl_easy *data);

CURLcode Curl_cf_ngtcp2_cmn_connect(સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data,
                                    bool *done);

CURLcode Curl_cf_ngtcp2_cmn_shutdown(સંરચના Curl_cfilter *cf,
                                     સંરચના Curl_easy *data, bool *done);
શૂન્ય Curl_cf_ngtcp2_cmn_conn_close(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data);

સંરચના cf_ngtcp2_io_ctx {
  સંરચના Curl_cfilter *cf;
  સંરચના Curl_easy *data;
  સંરચના curltime now;
  ngtcp2_tstamp ts;
  ngtcp2_path_storage ps;
};

શૂન્ય Curl_cf_ngtcp2_io_ctx_init(સંરચના cf_ngtcp2_io_ctx *io_ctx,
                                સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data);
શૂન્ય Curl_cf_ngtcp2_io_ctx_update_time(સંરચના Curl_easy *data,
                                       સંરચના cf_ngtcp2_io_ctx *io_ctx,
                                       સંરચના Curl_cfilter *cf);

CURLcode Curl_cf_ngtcp2_progress_egress(સંરચના Curl_cfilter *cf,
                                        સંરચના Curl_easy *data,
                                        સંરચના cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_progress_ingress(સંરચના Curl_cfilter *cf,
                                         સંરચના Curl_easy *data,
                                         સંરચના cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_cmn_set_expiry(સંરચના Curl_cfilter *cf,
                                       સંરચના Curl_easy *data,
                                       સંરચના cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_h3_stream_setup(સંરચના Curl_cfilter *cf,
                                        સંરચના Curl_easy *data,
                                        સંરચના h3_stream_ctx **pstream);
શૂન્ય Curl_cf_ngtcp2_h3_stream_close(સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data,
                                    સંરચના h3_stream_ctx *stream);
શૂન્ય Curl_cf_ngtcp2_h3_stream_done(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data);

bool Curl_cf_ngtcp2_cmn_conn_is_alive(સંરચના Curl_cfilter *cf,
                                      સંરચના Curl_easy *data,
                                      bool *આગત_pending);

CURLcode Curl_cf_ngtcp2_cmn_query(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data,
                                  પૂર્ણાંક query, પૂર્ણાંક *pres1, શૂન્ય *pres2);

CURLcode Curl_cf_ngtcp2_cmn_cntrl(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data,
                                  પૂર્ણાંક event, પૂર્ણાંક arg1, શૂન્ય *arg2);

સંરચના Curl_easy *Curl_cf_ngtcp2_get_xfer(સંરચના Curl_cfilter *cf,
                                          શૂન્ય *stream_user_data,
                                          સંરચના h3_stream_ctx **pstream);

#endif /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */

#endif /* HEADER_CURL_VQUIC_CF_NGTCP2_CMN_H */
