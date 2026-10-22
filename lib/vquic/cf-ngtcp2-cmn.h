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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#ଅନ୍ତର୍ଭାଵ <ngtcp2/ngtcp2.h>
#ଅନ୍ତର୍ଭାଵ <ngtcp2/ngtcp2_crypto.h>

#ifdef USE_OPENSSL
#ଅନ୍ତର୍ଭାଵ <openssl/err.h>
#ଯଦି defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#ଅନ୍ତର୍ଭାଵ <ngtcp2/ngtcp2_crypto_boringssl.h>
#ନଚେତଯଦି defined(OPENSSL_QUIC_API2)
#ଅନ୍ତର୍ଭାଵ <ngtcp2/ngtcp2_crypto_ossl.h>
#ଅନ୍ୟଥା
#ଅନ୍ତର୍ଭାଵ <ngtcp2/ngtcp2_crypto_quictls.h>
#endif
#ଅନ୍ତର୍ଭାଵ "vtls/openssl.h"
#ନଚେତଯଦି defined(USE_GNUTLS)
#ଅନ୍ତର୍ଭାଵ <ngtcp2/ngtcp2_crypto_gnutls.h>
#ଅନ୍ତର୍ଭାଵ "vtls/gtls.h"
#ନଚେତଯଦି defined(USE_WOLFSSL)
#ଅନ୍ତର୍ଭାଵ <wolfssl/options.h>
#ଅନ୍ତର୍ଭାଵ <wolfssl/ssl.h>
#ଅନ୍ତର୍ଭାଵ <wolfssl/quic.h>
#ଅନ୍ତର୍ଭାଵ <ngtcp2/ngtcp2_crypto_wolfssl.h>
#ଅନ୍ତର୍ଭାଵ "vtls/wolfssl.h"
#endif

#ifdef HAVE_NETINET_UDP_H
#ଅନ୍ତର୍ଭାଵ <netinet/udp.h>
#endif

#ଅନ୍ତର୍ଭାଵ <nghttp3/nghttp3.h>

#ଅନ୍ତର୍ଭାଵ "http1.h"
#ଅନ୍ତର୍ଭାଵ "u32_ptrset.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"
#ଅନ୍ତର୍ଭାଵ "vquic/vquic_int.h"
#ଅନ୍ତର୍ଭାଵ "vquic/vquic-tls.h"

ସଂରଚନା Curl_cfilter;
ସଂରଚନା Curl_easy;
ସଂରଚନା cf_ngtcp2_ctx;
ସଂରଚନା cf_quic_ctx;

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
#ଯଦି H3_STREAM_CHUNK_SIZE < NGTCP2_MAX_UDP_PAYLOAD_SIZE
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


ଶୂନ୍ୟ Curl_ngtcp2_ver(ଅକ୍ଷର *p, size_t len);

ପ୍ରକାରବ୍ୟାଖ୍ୟା CURLcode cf_ngtcp2_init_h3_conn(ସଂରଚନା Curl_cfilter *cf,
                                        ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା cf_ngtcp2_ctx *ctx);

ସଂରଚନା cf_ngtcp2_ctx {
  ସଂରଚନା cf_quic_ctx q;
  ସଂରଚନା ssl_peer ssl_peer;
  ସଂରଚନା curl_tls_ctx tls;
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
  ସଂରଚନା cf_call_data call_data;
  cf_ngtcp2_init_h3_conn *init_h3_conn_cb;
  nghttp3_conn *h3conn;
  nghttp3_settings h3settings;
  ସଂରଚନା curltime started_at;       /* time the current attempt started */
  ସଂରଚନା curltime handshake_at;     /* time connect handshake finished */
  ସଂରଚନା bufc_pool stream_bufcp;    /* chunk pool for streams */
  ସଂରଚନା dynbuf scratch;            /* temp buffer for header construction */
  ସଂରଚନା u32_ptrset streams;        /* set of data->mid + h3_stream_ctx */
  uint64_t used_bidi_streams;       /* bidi streams we have opened */
  uint64_t max_bidi_streams;        /* max bidi streams we can open */
  size_t earlydata_max;             /* max amount of early data supported by
                                       server on session reuse */
  size_t earlydata_skip;            /* sending bytes to skip when earlydata
                                       is accepted by peer */
  CURLcode tls_vrfy_result;         /* result of TLS peer verification */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା qlogfd;
  unsigned ଅକ୍ଷର *tunnel_inbuf;      /* ingress buffer for tunneled packets */
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
#define CF_CTX_CALL_DATA(cf) ((ସଂରଚନା cf_ngtcp2_ctx *)(cf)->ctx)->call_data

CURLcode Curl_cf_ngtcp2_ctx_init(ସଂରଚନା cf_ngtcp2_ctx *ctx,
                                 ସଂରଚନା Curl_peer *origin,
                                 ସଂରଚନା Curl_peer *peer,
                                 ସଂରଚନା ssl_filter_config *sslc,
                                 cf_ngtcp2_init_h3_conn *init_h3_conn_cb);
ଶୂନ୍ୟ Curl_cf_ngtcp2_ctx_cleanup(ସଂରଚନା cf_ngtcp2_ctx *ctx);
ଶୂନ୍ୟ Curl_cf_ngtcp2_cmn_err_set(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା code);

/**
 * All about the H3 internals of a stream
 */
ସଂରଚନା h3_stream_ctx {
  int64_t id;                   /* HTTP/3 stream identifier */
  curl_off_t xfer_id;           /* `id` of transfer owning this stream */
  ସଂରଚନା bufq sendbuf;          /* h3 request body */
  ସଂରଚନା h1_req_parser h1;      /* h1 request parsing */
  uint64_t error3;              /* HTTP/3 stream error code */
  curl_off_t upload_ବାମ;       /* number of request bytes left to upload */
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

ଶୂନ୍ୟ Curl_cf_ngtcp2_h3_stream_ctx_free(ସଂରଚନା h3_stream_ctx *stream);
ଶୂନ୍ୟ Curl_cf_ngtcp2_h3_err_set(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା code);

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(ସଂରଚନା cf_ngtcp2_ctx *ctx,
                                      ସଂରଚନା Curl_easy *data);

CURLcode Curl_cf_ngtcp2_cmn_connect(ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data,
                                    bool *done);

CURLcode Curl_cf_ngtcp2_cmn_shutdown(ସଂରଚନା Curl_cfilter *cf,
                                     ସଂରଚନା Curl_easy *data, bool *done);
ଶୂନ୍ୟ Curl_cf_ngtcp2_cmn_conn_close(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data);

ସଂରଚନା cf_ngtcp2_io_ctx {
  ସଂରଚନା Curl_cfilter *cf;
  ସଂରଚନା Curl_easy *data;
  ସଂରଚନା curltime now;
  ngtcp2_tstamp ts;
  ngtcp2_path_storage ps;
};

ଶୂନ୍ୟ Curl_cf_ngtcp2_io_ctx_init(ସଂରଚନା cf_ngtcp2_io_ctx *io_ctx,
                                ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data);
ଶୂନ୍ୟ Curl_cf_ngtcp2_io_ctx_update_time(ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା cf_ngtcp2_io_ctx *io_ctx,
                                       ସଂରଚନା Curl_cfilter *cf);

CURLcode Curl_cf_ngtcp2_progress_egress(ସଂରଚନା Curl_cfilter *cf,
                                        ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_progress_ingress(ସଂରଚନା Curl_cfilter *cf,
                                         ସଂରଚନା Curl_easy *data,
                                         ସଂରଚନା cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_cmn_set_expiry(ସଂରଚନା Curl_cfilter *cf,
                                       ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_h3_stream_setup(ସଂରଚନା Curl_cfilter *cf,
                                        ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା h3_stream_ctx **pstream);
ଶୂନ୍ୟ Curl_cf_ngtcp2_h3_stream_close(ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା h3_stream_ctx *stream);
ଶୂନ୍ୟ Curl_cf_ngtcp2_h3_stream_done(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data);

bool Curl_cf_ngtcp2_cmn_conn_is_alive(ସଂରଚନା Curl_cfilter *cf,
                                      ସଂରଚନା Curl_easy *data,
                                      bool *ନିବେଶ_pending);

CURLcode Curl_cf_ngtcp2_cmn_query(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data,
                                  ପୂର୍ଣ୍ଣସଂଖ୍ୟା query, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pres1, ଶୂନ୍ୟ *pres2);

CURLcode Curl_cf_ngtcp2_cmn_cntrl(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data,
                                  ପୂର୍ଣ୍ଣସଂଖ୍ୟା event, ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg1, ଶୂନ୍ୟ *arg2);

ସଂରଚନା Curl_easy *Curl_cf_ngtcp2_get_xfer(ସଂରଚନା Curl_cfilter *cf,
                                          ଶୂନ୍ୟ *stream_user_data,
                                          ସଂରଚନା h3_stream_ctx **pstream);

#endif /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */

#endif /* HEADER_CURL_VQUIC_CF_NGTCP2_CMN_H */
