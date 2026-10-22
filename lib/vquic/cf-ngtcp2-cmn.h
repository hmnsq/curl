#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_VQUIC_CF_NGTCP2_CMN_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_VQUIC_CF_NGTCP2_CMN_H
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

#ਜੇ !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#ਅਨ੍ਤਰ੍ਭਾਵ <ngtcp2/ngtcp2.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <ngtcp2/ngtcp2_crypto.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/err.h>
#ਜੇ defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#ਅਨ੍ਤਰ੍ਭਾਵ <ngtcp2/ngtcp2_crypto_boringssl.h>
#ਜਾਂਜੇ defined(OPENSSL_QUIC_API2)
#ਅਨ੍ਤਰ੍ਭਾਵ <ngtcp2/ngtcp2_crypto_ossl.h>
#ਨਹੀਂਤਾਂ
#ਅਨ੍ਤਰ੍ਭਾਵ <ngtcp2/ngtcp2_crypto_quictls.h>
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/openssl.h"
#ਜਾਂਜੇ defined(USE_GNUTLS)
#ਅਨ੍ਤਰ੍ਭਾਵ <ngtcp2/ngtcp2_crypto_gnutls.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/gtls.h"
#ਜਾਂਜੇ defined(USE_WOLFSSL)
#ਅਨ੍ਤਰ੍ਭਾਵ <wolfssl/options.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <wolfssl/ssl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <wolfssl/quic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <ngtcp2/ngtcp2_crypto_wolfssl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/wolfssl.h"
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_UDP_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/udp.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <nghttp3/nghttp3.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "http1.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "u32_ptrset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/vquic_int.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/vquic-tls.h"

ਬਣਤਰ Curl_cfilter;
ਬਣਤਰ Curl_easy;
ਬਣਤਰ cf_ngtcp2_ctx;
ਬਣਤਰ cf_quic_ctx;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ QUIC_MAX_STREAMS       (256 * 1024)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ QUIC_HANDSHAKE_TIMEOUT (10 * NGTCP2_SECONDS)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ QUIC_TUNNEL_INBUF_ਮਾਪ (64 * 1024)

/* We announce a small window size in transport param to the server,
 * and grow that immediately to max when no rate limit is in place.
 * We need to start small as we are not able to decrease it. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H3_ਧਾਰਾ_ਖਿੜਕੀ_ਮਾਪ_INITIAL (32 * 1024)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H3_ਧਾਰਾ_ਖਿੜਕੀ_ਮਾਪ_MAX     (10 * 1024 * 1024)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H3_CONN_ਖਿੜਕੀ_ਮਾਪ_MAX       (100 * H3_ਧਾਰਾ_ਖਿੜਕੀ_ਮਾਪ_MAX)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H3_ਧਾਰਾ_CHUNK_ਮਾਪ  (64 * 1024)
#ਜੇ H3_ਧਾਰਾ_CHUNK_ਮਾਪ < NGTCP2_MAX_UDP_PAYLOAD_ਮਾਪ
#ਗਲਤੀ H3_ਧਾਰਾ_CHUNK_ਮਾਪ smaller than NGTCP2_MAX_UDP_PAYLOAD_ਮਾਪ
#ਜੇ_ਅੰਤ
/* The pool keeps spares around for all streams.
 * The benefit of the pool is that stream buffers do not keep
 * spares. Memory consumption goes down when streams run empty,
 * have a large upload done, etc. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H3_ਧਾਰਾ_POOL_SPARES      5
/* The max amount of un-acked upload data we keep around per stream */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H3_ਧਾਰਾ_SEND_ਬਫਰ_MAX      (10 * H3_ਧਾਰਾ_CHUNK_ਮਾਪ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H3_ਧਾਰਾ_SEND_CHUNKS \
  (H3_ਧਾਰਾ_SEND_ਬਫਰ_MAX / H3_ਧਾਰਾ_CHUNK_ਮਾਪ)
/* How much data we initially want to buffer un-acked */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H3_ਧਾਰਾ_SEND_BUF_INITIAL     (32 * 1024)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ QUIC_TUNNEL_INGRESS_PKT_ਹੱਦਬੰਦੀ 1000


ਖਾਲੀ Curl_ngtcp2_ver(ਅੱਖਰ *p, size_t len);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ CURLcode cf_ngtcp2_init_h3_conn(ਬਣਤਰ Curl_cfilter *cf,
                                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ cf_ngtcp2_ctx *ctx);

ਬਣਤਰ cf_ngtcp2_ctx {
  ਬਣਤਰ cf_quic_ctx q;
  ਬਣਤਰ ssl_peer ssl_peer;
  ਬਣਤਰ curl_tls_ctx tls;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ OPENSSL_QUIC_API2
  ngtcp2_crypto_ossl_ctx *ossl_ctx;
#ਜੇ_ਅੰਤ
  ngtcp2_ਰਸਤਾ connected_ਰਸਤਾ;
  ngtcp2_conn *qconn;
  ngtcp2_cid dcid;
  ngtcp2_cid scid;
  uint32_t ਸੰਸਕਰਣ_2;
  ngtcp2_settings settings;
  ngtcp2_transport_params transport_params;
  ngtcp2_ccerr ਆਖਰੀ_error;
  ngtcp2_crypto_conn_ref conn_ref;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ call_ਅੰਕੜੇ;
  cf_ngtcp2_init_h3_conn *init_h3_conn_cb;
  nghttp3_conn *h3conn;
  nghttp3_settings h3settings;
  ਬਣਤਰ curltime started_at;       /* time the current attempt started */
  ਬਣਤਰ curltime handshake_at;     /* time connect handshake finished */
  ਬਣਤਰ bufc_pool ਧਾਰਾ_bufcp;    /* chunk pool for streams */
  ਬਣਤਰ dynbuf scratch;            /* temp buffer for header construction */
  ਬਣਤਰ u32_ptrset streams;        /* set of data->mid + h3_stream_ctx */
  uint64_t ਵਰਤਿਆ_bidi_streams;       /* bidi streams we have opened */
  uint64_t max_bidi_streams;        /* max bidi streams we can open */
  size_t earlydata_max;             /* max amount of early data supported by
                                       server on session reuse */
  size_t earlydata_skip;            /* sending bytes to skip when earlydata
                                       is accepted by peer */
  CURLcode tls_vrfy_ਨਤੀਜਾ;         /* result of TLS peer verification */
  ਪੂਰਨ qlogfd;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *tunnel_inbuf;      /* ingress buffer for tunneled packets */
  size_t tunnel_inbuf_len;
  ਦੋਆਧਾਰੀ_ਅੰਕ(initialized);
  ਦੋਆਧਾਰੀ_ਅੰਕ(tls_handshake_complete);      /* TLS handshake is done */
  ਦੋਆਧਾਰੀ_ਅੰਕ(use_earlydata);               /* Using 0RTT data */
  ਦੋਆਧਾਰੀ_ਅੰਕ(earlydata_accepted);          /* 0RTT was accepted by server */
  ਦੋਆਧਾਰੀ_ਅੰਕ(shutdown_started);            /* queued shutdown packets */
  ਦੋਆਧਾਰੀ_ਅੰਕ(stats_reported);              /* connect statistics reported */
};

/* How to access `call_data` from a cf_ngtcp2 filter */
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ CF_CTX_CALL_ਅੰਕੜੇ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CF_CTX_CALL_ਅੰਕੜੇ(cf) ((ਬਣਤਰ cf_ngtcp2_ctx *)(cf)->ctx)->call_ਅੰਕੜੇ

CURLcode Curl_cf_ngtcp2_ctx_init(ਬਣਤਰ cf_ngtcp2_ctx *ctx,
                                 ਬਣਤਰ Curl_peer *origin,
                                 ਬਣਤਰ Curl_peer *peer,
                                 ਬਣਤਰ ssl_filter_config *sslc,
                                 cf_ngtcp2_init_h3_conn *init_h3_conn_cb);
ਖਾਲੀ Curl_cf_ngtcp2_ctx_cleanup(ਬਣਤਰ cf_ngtcp2_ctx *ctx);
ਖਾਲੀ Curl_cf_ngtcp2_cmn_err_ਸੈੱਟ(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਪੂਰਨ ਕੋਡ);

/**
 * All about the H3 internals of a stream
 */
ਬਣਤਰ h3_ਧਾਰਾ_ctx {
  int64_t id;                   /* HTTP/3 stream identifier */
  curl_off_t xfer_id;           /* `id` of transfer owning this stream */
  ਬਣਤਰ bufq sendbuf;          /* h3 request body */
  ਬਣਤਰ h1_req_parser h1;      /* h1 request parsing */
  uint64_t error3;              /* HTTP/3 stream error code */
  curl_off_t upload_ਖੱਬਾ;       /* number of request bytes left to upload */
  curl_off_t rx_ਕੁੱਲ;          /* total number of bytes received */
  uint64_t rx_ਵਿੱਥ;           /* current receive offset */
  uint64_t rx_ਵਿੱਥ_max;       /* allowed receive offset */
  size_t tx_ਵਿੱਚ_flight_ਮਾਪ;     /* sendbuf data "in flight" */
  size_t tx_ਵਿੱਚ_flight_ideal;    /* ideal amount of un-acked send data */
  uint32_t mid;                 /* `mid` of transfer owning this stream */
  int32_t ਸਥਿਤੀ_ਕੋਡ;              /* HTTP status code */
  CURLcode xfer_ਨਤੀਜਾ;         /* result from xfer_resp_write(_hd) */
  ਦੋਆਧਾਰੀ_ਅੰਕ(resp_hds_complete);       /* we have a complete, final response */
  ਦੋਆਧਾਰੀ_ਅੰਕ(closed);                  /* TRUE on stream close */
  ਦੋਆਧਾਰੀ_ਅੰਕ(reset);                   /* TRUE on stream reset */
  ਦੋਆਧਾਰੀ_ਅੰਕ(send_closed);             /* stream is local closed */
  ਦੋਆਧਾਰੀ_ਅੰਕ(quic_flow_blocked);       /* stream is blocked by QUIC flow control */
};

ਖਾਲੀ Curl_cf_ngtcp2_h3_ਧਾਰਾ_ctx_free(ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ);
ਖਾਲੀ Curl_cf_ngtcp2_h3_err_ਸੈੱਟ(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਪੂਰਨ ਕੋਡ);

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(ਬਣਤਰ cf_ngtcp2_ctx *ctx,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

CURLcode Curl_cf_ngtcp2_cmn_connect(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    bool *ਪੂਰਾ);

CURLcode Curl_cf_ngtcp2_cmn_shutdown(ਬਣਤਰ Curl_cfilter *cf,
                                     ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ);
ਖਾਲੀ Curl_cf_ngtcp2_cmn_conn_close(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

ਬਣਤਰ cf_ngtcp2_io_ctx {
  ਬਣਤਰ Curl_cfilter *cf;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ;
  ਬਣਤਰ curltime now;
  ngtcp2_tstamp ts;
  ngtcp2_ਰਸਤਾ_storage ps;
};

ਖਾਲੀ Curl_cf_ngtcp2_io_ctx_init(ਬਣਤਰ cf_ngtcp2_io_ctx *io_ctx,
                                ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);
ਖਾਲੀ Curl_cf_ngtcp2_io_ctx_update_ਸਮਾਂ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ cf_ngtcp2_io_ctx *io_ctx,
                                       ਬਣਤਰ Curl_cfilter *cf);

CURLcode Curl_cf_ngtcp2_ਤਰੱਕੀ_egress(ਬਣਤਰ Curl_cfilter *cf,
                                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_ਤਰੱਕੀ_ingress(ਬਣਤਰ Curl_cfilter *cf,
                                         ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_cmn_ਸੈੱਟ_expiry(ਬਣਤਰ Curl_cfilter *cf,
                                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_h3_ਧਾਰਾ_ਤਿਆਰੀ(ਬਣਤਰ Curl_cfilter *cf,
                                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ h3_ਧਾਰਾ_ctx **pstream);
ਖਾਲੀ Curl_cf_ngtcp2_h3_ਧਾਰਾ_close(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ h3_ਧਾਰਾ_ctx *ਧਾਰਾ);
ਖਾਲੀ Curl_cf_ngtcp2_h3_ਧਾਰਾ_ਪੂਰਾ(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

bool Curl_cf_ngtcp2_cmn_conn_is_alive(ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      bool *ਨਿਵੇਸ਼_pending);

CURLcode Curl_cf_ngtcp2_cmn_query(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਪੂਰਨ query, ਪੂਰਨ *pres1, ਖਾਲੀ *pres2);

CURLcode Curl_cf_ngtcp2_cmn_cntrl(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਪੂਰਨ event, ਪੂਰਨ arg1, ਖਾਲੀ *arg2);

ਬਣਤਰ Curl_easy *Curl_cf_ngtcp2_get_xfer(ਬਣਤਰ Curl_cfilter *cf,
                                          ਖਾਲੀ *ਧਾਰਾ_user_ਅੰਕੜੇ,
                                          ਬਣਤਰ h3_ਧਾਰਾ_ctx **pstream);

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */

#ਜੇ_ਅੰਤ /* HEADER_CURL_VQUIC_CF_NGTCP2_CMN_H */
