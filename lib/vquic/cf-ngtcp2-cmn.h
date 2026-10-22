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
#অন্তর্ভাব "curl_setup.h"

#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#অন্তর্ভাব <ngtcp2/ngtcp2.h>
#অন্তর্ভাব <ngtcp2/ngtcp2_crypto.h>

#ifdef USE_OPENSSL
#অন্তর্ভাব <openssl/err.h>
#যদি defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#অন্তর্ভাব <ngtcp2/ngtcp2_crypto_boringssl.h>
#নতুবা_যদি defined(OPENSSL_QUIC_API2)
#অন্তর্ভাব <ngtcp2/ngtcp2_crypto_ossl.h>
#অন্যথা
#অন্তর্ভাব <ngtcp2/ngtcp2_crypto_quictls.h>
#endif
#অন্তর্ভাব "vtls/openssl.h"
#নতুবা_যদি defined(USE_GNUTLS)
#অন্তর্ভাব <ngtcp2/ngtcp2_crypto_gnutls.h>
#অন্তর্ভাব "vtls/gtls.h"
#নতুবা_যদি defined(USE_WOLFSSL)
#অন্তর্ভাব <wolfssl/options.h>
#অন্তর্ভাব <wolfssl/ssl.h>
#অন্তর্ভাব <wolfssl/quic.h>
#অন্তর্ভাব <ngtcp2/ngtcp2_crypto_wolfssl.h>
#অন্তর্ভাব "vtls/wolfssl.h"
#endif

#ifdef HAVE_NETINET_UDP_H
#অন্তর্ভাব <netinet/udp.h>
#endif

#অন্তর্ভাব <nghttp3/nghttp3.h>

#অন্তর্ভাব "http1.h"
#অন্তর্ভাব "u32_ptrset.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "vquic/vquic_int.h"
#অন্তর্ভাব "vquic/vquic-tls.h"

কাঠামো Curl_cfilter;
কাঠামো Curl_easy;
কাঠামো cf_ngtcp2_ctx;
কাঠামো cf_quic_ctx;

#define QUIC_MAX_STREAMS       (256 * 1024)
#define QUIC_HANDSHAKE_TIMEOUT (10 * NGTCP2_SECONDS)
#define QUIC_TUNNEL_INBUF_আকার (64 * 1024)

/* We announce a small window size in transport param to the server,
 * and grow that immediately to max when no rate limit is in place.
 * We need to start small as we are not able to decrease it. */
#define H3_STREAM_WINDOW_আকার_INITIAL (32 * 1024)
#define H3_STREAM_WINDOW_আকার_MAX     (10 * 1024 * 1024)
#define H3_CONN_WINDOW_আকার_MAX       (100 * H3_STREAM_WINDOW_আকার_MAX)

#define H3_STREAM_CHUNK_আকার  (64 * 1024)
#যদি H3_STREAM_CHUNK_আকার < NGTCP2_MAX_UDP_PAYLOAD_আকার
#error H3_STREAM_CHUNK_আকার smaller than NGTCP2_MAX_UDP_PAYLOAD_আকার
#endif
/* The pool keeps spares around for all streams.
 * The benefit of the pool is that stream buffers do not keep
 * spares. Memory consumption goes down when streams run empty,
 * have a large upload done, etc. */
#define H3_STREAM_POOL_SPARES      5
/* The max amount of un-acked upload data we keep around per stream */
#define H3_STREAM_SEND_বাফার_MAX      (10 * H3_STREAM_CHUNK_আকার)
#define H3_STREAM_SEND_CHUNKS \
  (H3_STREAM_SEND_বাফার_MAX / H3_STREAM_CHUNK_আকার)
/* How much data we initially want to buffer un-acked */
#define H3_STREAM_SEND_BUF_INITIAL     (32 * 1024)

#define QUIC_TUNNEL_INGRESS_PKT_LIMIT 1000


শূন্য Curl_ngtcp2_ver(অক্ষর *p, size_t len);

প্রকারলেখ CURLcode cf_ngtcp2_init_h3_conn(কাঠামো Curl_cfilter *cf,
                                        কাঠামো Curl_easy *তথ্য,
                                        কাঠামো cf_ngtcp2_ctx *ctx);

কাঠামো cf_ngtcp2_ctx {
  কাঠামো cf_quic_ctx q;
  কাঠামো ssl_peer ssl_peer;
  কাঠামো curl_tls_ctx tls;
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
  কাঠামো cf_call_তথ্য call_তথ্য;
  cf_ngtcp2_init_h3_conn *init_h3_conn_cb;
  nghttp3_conn *h3conn;
  nghttp3_settings h3settings;
  কাঠামো curltime started_at;       /* time the current attempt started */
  কাঠামো curltime handshake_at;     /* time connect handshake finished */
  কাঠামো bufc_pool stream_bufcp;    /* chunk pool for streams */
  কাঠামো dynbuf scratch;            /* temp buffer for header construction */
  কাঠামো u32_ptrset streams;        /* set of data->mid + h3_stream_ctx */
  uint64_t used_bidi_streams;       /* bidi streams we have opened */
  uint64_t max_bidi_streams;        /* max bidi streams we can open */
  size_t earlydata_max;             /* max amount of early data supported by
                                       server on session reuse */
  size_t earlydata_skip;            /* sending bytes to skip when earlydata
                                       is accepted by peer */
  CURLcode tls_vrfy_ফলাফল;         /* result of TLS peer verification */
  পূর্ণসংখ্যা qlogfd;
  unsigned অক্ষর *tunnel_inbuf;      /* ingress buffer for tunneled packets */
  size_t tunnel_inbuf_len;
  BIT(initialized);
  BIT(tls_handshake_complete);      /* TLS handshake is done */
  BIT(use_earlydata);               /* Using 0RTT data */
  BIT(earlydata_accepted);          /* 0RTT was accepted by server */
  BIT(shutdown_started);            /* queued shutdown packets */
  BIT(stats_reported);              /* connect statistics reported */
};

/* How to access `call_data` from a cf_ngtcp2 filter */
#undef CF_CTX_CALL_তথ্য
#define CF_CTX_CALL_তথ্য(cf) ((কাঠামো cf_ngtcp2_ctx *)(cf)->ctx)->call_তথ্য

CURLcode Curl_cf_ngtcp2_ctx_init(কাঠামো cf_ngtcp2_ctx *ctx,
                                 কাঠামো Curl_peer *origin,
                                 কাঠামো Curl_peer *peer,
                                 কাঠামো ssl_filter_config *sslc,
                                 cf_ngtcp2_init_h3_conn *init_h3_conn_cb);
শূন্য Curl_cf_ngtcp2_ctx_cleanup(কাঠামো cf_ngtcp2_ctx *ctx);
শূন্য Curl_cf_ngtcp2_cmn_err_set(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য, পূর্ণসংখ্যা code);

/**
 * All about the H3 internals of a stream
 */
কাঠামো h3_stream_ctx {
  int64_t id;                   /* HTTP/3 stream identifier */
  curl_off_t xfer_id;           /* `id` of transfer owning this stream */
  কাঠামো bufq sendbuf;          /* h3 request body */
  কাঠামো h1_req_parser h1;      /* h1 request parsing */
  uint64_t error3;              /* HTTP/3 stream error code */
  curl_off_t upload_বাম;       /* number of request bytes left to upload */
  curl_off_t rx_যোগ;          /* total number of bytes received */
  uint64_t rx_offset;           /* current receive offset */
  uint64_t rx_offset_max;       /* allowed receive offset */
  size_t tx_in_flight_আকার;     /* sendbuf data "in flight" */
  size_t tx_in_flight_ideal;    /* ideal amount of un-acked send data */
  uint32_t mid;                 /* `mid` of transfer owning this stream */
  int32_t status_code;              /* HTTP status code */
  CURLcode xfer_ফলাফল;         /* result from xfer_resp_write(_hd) */
  BIT(resp_hds_complete);       /* we have a complete, final response */
  BIT(closed);                  /* TRUE on stream close */
  BIT(reset);                   /* TRUE on stream reset */
  BIT(send_closed);             /* stream is local closed */
  BIT(quic_flow_blocked);       /* stream is blocked by QUIC flow control */
};

শূন্য Curl_cf_ngtcp2_h3_stream_ctx_free(কাঠামো h3_stream_ctx *stream);
শূন্য Curl_cf_ngtcp2_h3_err_set(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য, পূর্ণসংখ্যা code);

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(কাঠামো cf_ngtcp2_ctx *ctx,
                                      কাঠামো Curl_easy *তথ্য);

CURLcode Curl_cf_ngtcp2_cmn_connect(কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য,
                                    bool *done);

CURLcode Curl_cf_ngtcp2_cmn_shutdown(কাঠামো Curl_cfilter *cf,
                                     কাঠামো Curl_easy *তথ্য, bool *done);
শূন্য Curl_cf_ngtcp2_cmn_conn_close(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য);

কাঠামো cf_ngtcp2_io_ctx {
  কাঠামো Curl_cfilter *cf;
  কাঠামো Curl_easy *তথ্য;
  কাঠামো curltime now;
  ngtcp2_tstamp ts;
  ngtcp2_path_storage ps;
};

শূন্য Curl_cf_ngtcp2_io_ctx_init(কাঠামো cf_ngtcp2_io_ctx *io_ctx,
                                কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য);
শূন্য Curl_cf_ngtcp2_io_ctx_update_time(কাঠামো Curl_easy *তথ্য,
                                       কাঠামো cf_ngtcp2_io_ctx *io_ctx,
                                       কাঠামো Curl_cfilter *cf);

CURLcode Curl_cf_ngtcp2_progress_egress(কাঠামো Curl_cfilter *cf,
                                        কাঠামো Curl_easy *তথ্য,
                                        কাঠামো cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_progress_ingress(কাঠামো Curl_cfilter *cf,
                                         কাঠামো Curl_easy *তথ্য,
                                         কাঠামো cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_cmn_set_expiry(কাঠামো Curl_cfilter *cf,
                                       কাঠামো Curl_easy *তথ্য,
                                       কাঠামো cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_h3_stream_setup(কাঠামো Curl_cfilter *cf,
                                        কাঠামো Curl_easy *তথ্য,
                                        কাঠামো h3_stream_ctx **pstream);
শূন্য Curl_cf_ngtcp2_h3_stream_close(কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য,
                                    কাঠামো h3_stream_ctx *stream);
শূন্য Curl_cf_ngtcp2_h3_stream_done(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য);

bool Curl_cf_ngtcp2_cmn_conn_is_alive(কাঠামো Curl_cfilter *cf,
                                      কাঠামো Curl_easy *তথ্য,
                                      bool *প্রবেশ_pending);

CURLcode Curl_cf_ngtcp2_cmn_query(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য,
                                  পূর্ণসংখ্যা query, পূর্ণসংখ্যা *pres1, শূন্য *pres2);

CURLcode Curl_cf_ngtcp2_cmn_cntrl(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য,
                                  পূর্ণসংখ্যা event, পূর্ণসংখ্যা arg1, শূন্য *arg2);

কাঠামো Curl_easy *Curl_cf_ngtcp2_get_xfer(কাঠামো Curl_cfilter *cf,
                                          শূন্য *stream_user_তথ্য,
                                          কাঠামো h3_stream_ctx **pstream);

#endif /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */

#endif /* HEADER_CURL_VQUIC_CF_NGTCP2_CMN_H */
