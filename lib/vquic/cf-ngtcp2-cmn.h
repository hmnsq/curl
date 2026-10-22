#నిర్వచితం_కాకపోతే శీర్షిక_CURL_VQUIC_CF_NGTCP2_CMN_H
#నిర్వచించు శీర్షిక_CURL_VQUIC_CF_NGTCP2_CMN_H
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

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#అన్తర్భావ <ngtcp2/ngtcp2.h>
#అన్తర్భావ <ngtcp2/ngtcp2_crypto.h>

#నిర్వచితమైతే USE_OPENSSL
#అన్తర్భావ <openssl/err.h>
#ఒకవేళ defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#అన్తర్భావ <ngtcp2/ngtcp2_crypto_boringssl.h>
#లేదావేళ defined(OPENSSL_QUIC_API2)
#అన్తర్భావ <ngtcp2/ngtcp2_crypto_ossl.h>
#లేకపోతే
#అన్తర్భావ <ngtcp2/ngtcp2_crypto_quictls.h>
#అయితే_ముగింపు
#అన్తర్భావ "vtls/openssl.h"
#లేదావేళ defined(USE_GNUTLS)
#అన్తర్భావ <ngtcp2/ngtcp2_crypto_gnutls.h>
#అన్తర్భావ "vtls/gtls.h"
#లేదావేళ defined(USE_WOLFSSL)
#అన్తర్భావ <wolfssl/options.h>
#అన్తర్భావ <wolfssl/ssl.h>
#అన్తర్భావ <wolfssl/quic.h>
#అన్తర్భావ <ngtcp2/ngtcp2_crypto_wolfssl.h>
#అన్తర్భావ "vtls/wolfssl.h"
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_NETINET_UDP_H
#అన్తర్భావ <netinet/udp.h>
#అయితే_ముగింపు

#అన్తర్భావ <nghttp3/nghttp3.h>

#అన్తర్భావ "http1.h"
#అన్తర్భావ "u32_ptrset.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "vquic/vquic_int.h"
#అన్తర్భావ "vquic/vquic-tls.h"

నిర్మాణం Curl_cfilter;
నిర్మాణం Curl_easy;
నిర్మాణం cf_ngtcp2_ctx;
నిర్మాణం cf_quic_ctx;

#నిర్వచించు QUIC_MAX_STREAMS       (256 * 1024)
#నిర్వచించు QUIC_HANDSHAKE_TIMEOUT (10 * NGTCP2_SECONDS)
#నిర్వచించు QUIC_TUNNEL_INBUF_కొలత (64 * 1024)

/* We announce a small window size in transport param to the server,
 * and grow that immediately to max when no rate limit is in place.
 * We need to start small as we are not able to decrease it. */
#నిర్వచించు H3_ప్రవాహం_కిటికీ_కొలత_INITIAL (32 * 1024)
#నిర్వచించు H3_ప్రవాహం_కిటికీ_కొలత_MAX     (10 * 1024 * 1024)
#నిర్వచించు H3_CONN_కిటికీ_కొలత_MAX       (100 * H3_ప్రవాహం_కిటికీ_కొలత_MAX)

#నిర్వచించు H3_ప్రవాహం_CHUNK_కొలత  (64 * 1024)
#ఒకవేళ H3_ప్రవాహం_CHUNK_కొలత < NGTCP2_MAX_UDP_PAYLOAD_కొలత
#దోషం H3_ప్రవాహం_CHUNK_కొలత smaller than NGTCP2_MAX_UDP_PAYLOAD_కొలత
#అయితే_ముగింపు
/* The pool keeps spares around for all streams.
 * The benefit of the pool is that stream buffers do not keep
 * spares. Memory consumption goes down when streams run empty,
 * have a large upload done, etc. */
#నిర్వచించు H3_ప్రవాహం_POOL_SPARES      5
/* The max amount of un-acked upload data we keep around per stream */
#నిర్వచించు H3_ప్రవాహం_SEND_బఫరు_MAX      (10 * H3_ప్రవాహం_CHUNK_కొలత)
#నిర్వచించు H3_ప్రవాహం_SEND_CHUNKS \
  (H3_ప్రవాహం_SEND_బఫరు_MAX / H3_ప్రవాహం_CHUNK_కొలత)
/* How much data we initially want to buffer un-acked */
#నిర్వచించు H3_ప్రవాహం_SEND_BUF_INITIAL     (32 * 1024)

#నిర్వచించు QUIC_TUNNEL_INGRESS_PKT_హద్దు 1000


శూన్య Curl_ngtcp2_ver(అక్షరం *p, size_t len);

రకనిర్వచనం CURLcode cf_ngtcp2_init_h3_conn(నిర్మాణం Curl_cfilter *cf,
                                        నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం cf_ngtcp2_ctx *ctx);

నిర్మాణం cf_ngtcp2_ctx {
  నిర్మాణం cf_quic_ctx q;
  నిర్మాణం ssl_peer ssl_peer;
  నిర్మాణం curl_tls_ctx tls;
#నిర్వచితమైతే OPENSSL_QUIC_API2
  ngtcp2_crypto_ossl_ctx *ossl_ctx;
#అయితే_ముగింపు
  ngtcp2_మార్గం connected_మార్గం;
  ngtcp2_conn *qconn;
  ngtcp2_cid dcid;
  ngtcp2_cid scid;
  uint32_t సంస్కరణ_2;
  ngtcp2_settings settings;
  ngtcp2_transport_params transport_params;
  ngtcp2_ccerr చివరి_error;
  ngtcp2_crypto_conn_ref conn_ref;
  నిర్మాణం cf_call_దత్తాంశం call_దత్తాంశం;
  cf_ngtcp2_init_h3_conn *init_h3_conn_cb;
  nghttp3_conn *h3conn;
  nghttp3_settings h3settings;
  నిర్మాణం curltime started_at;       /* time the current attempt started */
  నిర్మాణం curltime handshake_at;     /* time connect handshake finished */
  నిర్మాణం bufc_pool ప్రవాహం_bufcp;    /* chunk pool for streams */
  నిర్మాణం dynbuf scratch;            /* temp buffer for header construction */
  నిర్మాణం u32_ptrset streams;        /* set of data->mid + h3_stream_ctx */
  uint64_t వాడిన_bidi_streams;       /* bidi streams we have opened */
  uint64_t max_bidi_streams;        /* max bidi streams we can open */
  size_t earlydata_max;             /* max amount of early data supported by
                                       server on session reuse */
  size_t earlydata_skip;            /* sending bytes to skip when earlydata
                                       is accepted by peer */
  CURLcode tls_vrfy_ఫలితం;         /* result of TLS peer verification */
  పూర్ణాంకం qlogfd;
  గుర్తులేని అక్షరం *tunnel_inbuf;      /* ingress buffer for tunneled packets */
  size_t tunnel_inbuf_len;
  బిట్టు(initialized);
  బిట్టు(tls_handshake_complete);      /* TLS handshake is done */
  బిట్టు(use_earlydata);               /* Using 0RTT data */
  బిట్టు(earlydata_accepted);          /* 0RTT was accepted by server */
  బిట్టు(shutdown_started);            /* queued shutdown packets */
  బిట్టు(stats_reported);              /* connect statistics reported */
};

/* How to access `call_data` from a cf_ngtcp2 filter */
#నిర్వచనం_రద్దు CF_CTX_CALL_దత్తాంశం
#నిర్వచించు CF_CTX_CALL_దత్తాంశం(cf) ((నిర్మాణం cf_ngtcp2_ctx *)(cf)->ctx)->call_దత్తాంశం

CURLcode Curl_cf_ngtcp2_ctx_init(నిర్మాణం cf_ngtcp2_ctx *ctx,
                                 నిర్మాణం Curl_peer *origin,
                                 నిర్మాణం Curl_peer *peer,
                                 నిర్మాణం ssl_filter_config *sslc,
                                 cf_ngtcp2_init_h3_conn *init_h3_conn_cb);
శూన్య Curl_cf_ngtcp2_ctx_cleanup(నిర్మాణం cf_ngtcp2_ctx *ctx);
శూన్య Curl_cf_ngtcp2_cmn_err_సమితి(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం, పూర్ణాంకం సంకేతం_2);

/**
 * All about the H3 internals of a stream
 */
నిర్మాణం h3_ప్రవాహం_ctx {
  int64_t id;                   /* HTTP/3 stream identifier */
  curl_off_t xfer_id;           /* `id` of transfer owning this stream */
  నిర్మాణం bufq sendbuf;          /* h3 request body */
  నిర్మాణం h1_req_parser h1;      /* h1 request parsing */
  uint64_t error3;              /* HTTP/3 stream error code */
  curl_off_t upload_ఎడమ;       /* number of request bytes left to upload */
  curl_off_t rx_మొత్తం;          /* total number of bytes received */
  uint64_t rx_స్థానభ్రంశం;           /* current receive offset */
  uint64_t rx_స్థానభ్రంశం_max;       /* allowed receive offset */
  size_t tx_లోపల_flight_కొలత;     /* sendbuf data "in flight" */
  size_t tx_లోపల_flight_ideal;    /* ideal amount of un-acked send data */
  uint32_t mid;                 /* `mid` of transfer owning this stream */
  int32_t స్థితి_సంకేతం;              /* HTTP status code */
  CURLcode xfer_ఫలితం;         /* result from xfer_resp_write(_hd) */
  బిట్టు(resp_hds_complete);       /* we have a complete, final response */
  బిట్టు(closed);                  /* TRUE on stream close */
  బిట్టు(reset);                   /* TRUE on stream reset */
  బిట్టు(send_closed);             /* stream is local closed */
  బిట్టు(quic_flow_blocked);       /* stream is blocked by QUIC flow control */
};

శూన్య Curl_cf_ngtcp2_h3_ప్రవాహం_ctx_free(నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం);
శూన్య Curl_cf_ngtcp2_h3_err_సమితి(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం, పూర్ణాంకం సంకేతం_2);

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(నిర్మాణం cf_ngtcp2_ctx *ctx,
                                      నిర్మాణం Curl_easy *దత్తాంశం);

CURLcode Curl_cf_ngtcp2_cmn_connect(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    bool *పూర్తి);

CURLcode Curl_cf_ngtcp2_cmn_shutdown(నిర్మాణం Curl_cfilter *cf,
                                     నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి);
శూన్య Curl_cf_ngtcp2_cmn_conn_close(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం);

నిర్మాణం cf_ngtcp2_io_ctx {
  నిర్మాణం Curl_cfilter *cf;
  నిర్మాణం Curl_easy *దత్తాంశం;
  నిర్మాణం curltime now;
  ngtcp2_tstamp ts;
  ngtcp2_మార్గం_storage ps;
};

శూన్య Curl_cf_ngtcp2_io_ctx_init(నిర్మాణం cf_ngtcp2_io_ctx *io_ctx,
                                నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం);
శూన్య Curl_cf_ngtcp2_io_ctx_update_సమయం(నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం cf_ngtcp2_io_ctx *io_ctx,
                                       నిర్మాణం Curl_cfilter *cf);

CURLcode Curl_cf_ngtcp2_పురోగతి_egress(నిర్మాణం Curl_cfilter *cf,
                                        నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_పురోగతి_ingress(నిర్మాణం Curl_cfilter *cf,
                                         నిర్మాణం Curl_easy *దత్తాంశం,
                                         నిర్మాణం cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_cmn_సమితి_expiry(నిర్మాణం Curl_cfilter *cf,
                                       నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_h3_ప్రవాహం_అమరిక(నిర్మాణం Curl_cfilter *cf,
                                        నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం h3_ప్రవాహం_ctx **pstream);
శూన్య Curl_cf_ngtcp2_h3_ప్రవాహం_close(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం h3_ప్రవాహం_ctx *ప్రవాహం);
శూన్య Curl_cf_ngtcp2_h3_ప్రవాహం_పూర్తి(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం);

bool Curl_cf_ngtcp2_cmn_conn_is_alive(నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం Curl_easy *దత్తాంశం,
                                      bool *ఆదానం_pending);

CURLcode Curl_cf_ngtcp2_cmn_query(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  పూర్ణాంకం query, పూర్ణాంకం *pres1, శూన్య *pres2);

CURLcode Curl_cf_ngtcp2_cmn_cntrl(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  పూర్ణాంకం event, పూర్ణాంకం arg1, శూన్య *arg2);

నిర్మాణం Curl_easy *Curl_cf_ngtcp2_get_xfer(నిర్మాణం Curl_cfilter *cf,
                                          శూన్య *ప్రవాహం_user_దత్తాంశం,
                                          నిర్మాణం h3_ప్రవాహం_ctx **pstream);

#అయితే_ముగింపు /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */

#అయితే_ముగింపు /* HEADER_CURL_VQUIC_CF_NGTCP2_CMN_H */
