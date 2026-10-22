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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಯದಿ !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2.h>
#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2_crypto.h>

#ifdef USE_OPENSSL
#ಅನ್ತರ್ಭಾವ <openssl/err.h>
#ಯದಿ defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2_crypto_boringssl.h>
#ನೋಚೇದ್ಯದಿ defined(OPENSSL_QUIC_API2)
#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2_crypto_ossl.h>
#ಅನ್ಯಥಾ
#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2_crypto_quictls.h>
#endif
#ಅನ್ತರ್ಭಾವ "vtls/openssl.h"
#ನೋಚೇದ್ಯದಿ defined(USE_GNUTLS)
#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2_crypto_gnutls.h>
#ಅನ್ತರ್ಭಾವ "vtls/gtls.h"
#ನೋಚೇದ್ಯದಿ defined(USE_WOLFSSL)
#ಅನ್ತರ್ಭಾವ <wolfssl/options.h>
#ಅನ್ತರ್ಭಾವ <wolfssl/ssl.h>
#ಅನ್ತರ್ಭಾವ <wolfssl/quic.h>
#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2_crypto_wolfssl.h>
#ಅನ್ತರ್ಭಾವ "vtls/wolfssl.h"
#endif

#ifdef HAVE_NETINET_UDP_H
#ಅನ್ತರ್ಭಾವ <netinet/udp.h>
#endif

#ಅನ್ತರ್ಭಾವ <nghttp3/nghttp3.h>

#ಅನ್ತರ್ಭಾವ "http1.h"
#ಅನ್ತರ್ಭಾವ "u32_ptrset.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "vquic/vquic_int.h"
#ಅನ್ತರ್ಭಾವ "vquic/vquic-tls.h"

ಸಂರಚನಾ Curl_cfilter;
ಸಂರಚನಾ Curl_easy;
ಸಂರಚನಾ cf_ngtcp2_ctx;
ಸಂರಚನಾ cf_quic_ctx;

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
#ಯದಿ H3_STREAM_CHUNK_SIZE < NGTCP2_MAX_UDP_PAYLOAD_SIZE
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


ಶೂನ್ಯ Curl_ngtcp2_ver(ವರ್ಣ *p, size_t len);

ಪ್ರಕಾರನಿರ್ದೇಶ CURLcode cf_ngtcp2_init_h3_conn(ಸಂರಚನಾ Curl_cfilter *cf,
                                        ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ cf_ngtcp2_ctx *ctx);

ಸಂರಚನಾ cf_ngtcp2_ctx {
  ಸಂರಚನಾ cf_quic_ctx q;
  ಸಂರಚನಾ ssl_peer ssl_peer;
  ಸಂರಚನಾ curl_tls_ctx tls;
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
  ಸಂರಚನಾ cf_call_data call_data;
  cf_ngtcp2_init_h3_conn *init_h3_conn_cb;
  nghttp3_conn *h3conn;
  nghttp3_settings h3settings;
  ಸಂರಚನಾ curltime started_at;       /* time the current attempt started */
  ಸಂರಚನಾ curltime handshake_at;     /* time connect handshake finished */
  ಸಂರಚನಾ bufc_pool stream_bufcp;    /* chunk pool for streams */
  ಸಂರಚನಾ dynbuf scratch;            /* temp buffer for header construction */
  ಸಂರಚನಾ u32_ptrset streams;        /* set of data->mid + h3_stream_ctx */
  uint64_t used_bidi_streams;       /* bidi streams we have opened */
  uint64_t max_bidi_streams;        /* max bidi streams we can open */
  size_t earlydata_max;             /* max amount of early data supported by
                                       server on session reuse */
  size_t earlydata_skip;            /* sending bytes to skip when earlydata
                                       is accepted by peer */
  CURLcode tls_vrfy_result;         /* result of TLS peer verification */
  ಪೂರ್ಣಾಙ್ಕ qlogfd;
  unsigned ವರ್ಣ *tunnel_inbuf;      /* ingress buffer for tunneled packets */
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
#define CF_CTX_CALL_DATA(cf) ((ಸಂರಚನಾ cf_ngtcp2_ctx *)(cf)->ctx)->call_data

CURLcode Curl_cf_ngtcp2_ctx_init(ಸಂರಚನಾ cf_ngtcp2_ctx *ctx,
                                 ಸಂರಚನಾ Curl_peer *origin,
                                 ಸಂರಚನಾ Curl_peer *peer,
                                 ಸಂರಚನಾ ssl_filter_config *sslc,
                                 cf_ngtcp2_init_h3_conn *init_h3_conn_cb);
ಶೂನ್ಯ Curl_cf_ngtcp2_ctx_cleanup(ಸಂರಚನಾ cf_ngtcp2_ctx *ctx);
ಶೂನ್ಯ Curl_cf_ngtcp2_cmn_err_set(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data, ಪೂರ್ಣಾಙ್ಕ code);

/**
 * All about the H3 internals of a stream
 */
ಸಂರಚನಾ h3_stream_ctx {
  int64_t id;                   /* HTTP/3 stream identifier */
  curl_off_t xfer_id;           /* `id` of transfer owning this stream */
  ಸಂರಚನಾ bufq sendbuf;          /* h3 request body */
  ಸಂರಚನಾ h1_req_parser h1;      /* h1 request parsing */
  uint64_t error3;              /* HTTP/3 stream error code */
  curl_off_t upload_ಎಡ;       /* number of request bytes left to upload */
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

ಶೂನ್ಯ Curl_cf_ngtcp2_h3_stream_ctx_free(ಸಂರಚನಾ h3_stream_ctx *stream);
ಶೂನ್ಯ Curl_cf_ngtcp2_h3_err_set(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data, ಪೂರ್ಣಾಙ್ಕ code);

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(ಸಂರಚನಾ cf_ngtcp2_ctx *ctx,
                                      ಸಂರಚನಾ Curl_easy *data);

CURLcode Curl_cf_ngtcp2_cmn_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    bool *done);

CURLcode Curl_cf_ngtcp2_cmn_shutdown(ಸಂರಚನಾ Curl_cfilter *cf,
                                     ಸಂರಚನಾ Curl_easy *data, bool *done);
ಶೂನ್ಯ Curl_cf_ngtcp2_cmn_conn_close(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data);

ಸಂರಚನಾ cf_ngtcp2_io_ctx {
  ಸಂರಚನಾ Curl_cfilter *cf;
  ಸಂರಚನಾ Curl_easy *data;
  ಸಂರಚನಾ curltime now;
  ngtcp2_tstamp ts;
  ngtcp2_path_storage ps;
};

ಶೂನ್ಯ Curl_cf_ngtcp2_io_ctx_init(ಸಂರಚನಾ cf_ngtcp2_io_ctx *io_ctx,
                                ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data);
ಶೂನ್ಯ Curl_cf_ngtcp2_io_ctx_update_time(ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ cf_ngtcp2_io_ctx *io_ctx,
                                       ಸಂರಚನಾ Curl_cfilter *cf);

CURLcode Curl_cf_ngtcp2_progress_egress(ಸಂರಚನಾ Curl_cfilter *cf,
                                        ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_progress_ingress(ಸಂರಚನಾ Curl_cfilter *cf,
                                         ಸಂರಚನಾ Curl_easy *data,
                                         ಸಂರಚನಾ cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_cmn_set_expiry(ಸಂರಚನಾ Curl_cfilter *cf,
                                       ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_h3_stream_setup(ಸಂರಚನಾ Curl_cfilter *cf,
                                        ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ h3_stream_ctx **pstream);
ಶೂನ್ಯ Curl_cf_ngtcp2_h3_stream_close(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ h3_stream_ctx *stream);
ಶೂನ್ಯ Curl_cf_ngtcp2_h3_stream_done(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data);

bool Curl_cf_ngtcp2_cmn_conn_is_alive(ಸಂರಚನಾ Curl_cfilter *cf,
                                      ಸಂರಚನಾ Curl_easy *data,
                                      bool *ಒಳಹರಿವು_pending);

CURLcode Curl_cf_ngtcp2_cmn_query(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  ಪೂರ್ಣಾಙ್ಕ query, ಪೂರ್ಣಾಙ್ಕ *pres1, ಶೂನ್ಯ *pres2);

CURLcode Curl_cf_ngtcp2_cmn_cntrl(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  ಪೂರ್ಣಾಙ್ಕ event, ಪೂರ್ಣಾಙ್ಕ arg1, ಶೂನ್ಯ *arg2);

ಸಂರಚನಾ Curl_easy *Curl_cf_ngtcp2_get_xfer(ಸಂರಚನಾ Curl_cfilter *cf,
                                          ಶೂನ್ಯ *stream_user_data,
                                          ಸಂರಚನಾ h3_stream_ctx **pstream);

#endif /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */

#endif /* HEADER_CURL_VQUIC_CF_NGTCP2_CMN_H */
