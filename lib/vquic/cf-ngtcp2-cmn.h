#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_VQUIC_CF_NGTCP2_CMN_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_VQUIC_CF_NGTCP2_CMN_H
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

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2.h>
#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2_crypto.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL
#ಅನ್ತರ್ಭಾವ <openssl/err.h>
#ಆದರೆ defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2_crypto_boringssl.h>
#ಅಥವಾಆದರೆ defined(OPENSSL_QUIC_API2)
#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2_crypto_ossl.h>
#ಇಲ್ಲದಿದ್ದರೆ
#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2_crypto_quictls.h>
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ "vtls/openssl.h"
#ಅಥವಾಆದರೆ defined(USE_GNUTLS)
#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2_crypto_gnutls.h>
#ಅನ್ತರ್ಭಾವ "vtls/gtls.h"
#ಅಥವಾಆದರೆ defined(USE_WOLFSSL)
#ಅನ್ತರ್ಭಾವ <wolfssl/options.h>
#ಅನ್ತರ್ಭಾವ <wolfssl/ssl.h>
#ಅನ್ತರ್ಭಾವ <wolfssl/quic.h>
#ಅನ್ತರ್ಭಾವ <ngtcp2/ngtcp2_crypto_wolfssl.h>
#ಅನ್ತರ್ಭಾವ "vtls/wolfssl.h"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_UDP_H
#ಅನ್ತರ್ಭಾವ <netinet/udp.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <nghttp3/nghttp3.h>

#ಅನ್ತರ್ಭಾವ "http1.h"
#ಅನ್ತರ್ಭಾವ "u32_ptrset.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "vquic/vquic_int.h"
#ಅನ್ತರ್ಭಾವ "vquic/vquic-tls.h"

ರಚನೆ Curl_cfilter;
ರಚನೆ Curl_easy;
ರಚನೆ cf_ngtcp2_ctx;
ರಚನೆ cf_quic_ctx;

#ವ್ಯಾಖ್ಯಾನಿಸು QUIC_MAX_STREAMS       (256 * 1024)
#ವ್ಯಾಖ್ಯಾನಿಸು QUIC_HANDSHAKE_TIMEOUT (10 * NGTCP2_SECONDS)
#ವ್ಯಾಖ್ಯಾನಿಸು QUIC_TUNNEL_INBUF_ಗಾತ್ರ (64 * 1024)

/* We announce a small window size in transport param to the server,
 * and grow that immediately to max when no rate limit is in place.
 * We need to start small as we are not able to decrease it. */
#ವ್ಯಾಖ್ಯಾನಿಸು H3_ಹರಿವು_ಕಿಟಕಿ_ಗಾತ್ರ_INITIAL (32 * 1024)
#ವ್ಯಾಖ್ಯಾನಿಸು H3_ಹರಿವು_ಕಿಟಕಿ_ಗಾತ್ರ_MAX     (10 * 1024 * 1024)
#ವ್ಯಾಖ್ಯಾನಿಸು H3_CONN_ಕಿಟಕಿ_ಗಾತ್ರ_MAX       (100 * H3_ಹರಿವು_ಕಿಟಕಿ_ಗಾತ್ರ_MAX)

#ವ್ಯಾಖ್ಯಾನಿಸು H3_ಹರಿವು_CHUNK_ಗಾತ್ರ  (64 * 1024)
#ಆದರೆ H3_ಹರಿವು_CHUNK_ಗಾತ್ರ < NGTCP2_MAX_UDP_PAYLOAD_ಗಾತ್ರ
#ದೋಷ H3_ಹರಿವು_CHUNK_ಗಾತ್ರ smaller than NGTCP2_MAX_UDP_PAYLOAD_ಗಾತ್ರ
#ಆದರೆ_ಅಂತ್ಯ
/* The pool keeps spares around for all streams.
 * The benefit of the pool is that stream buffers do not keep
 * spares. Memory consumption goes down when streams run empty,
 * have a large upload done, etc. */
#ವ್ಯಾಖ್ಯಾನಿಸು H3_ಹರಿವು_POOL_SPARES      5
/* The max amount of un-acked upload data we keep around per stream */
#ವ್ಯಾಖ್ಯಾನಿಸು H3_ಹರಿವು_SEND_ಬಫರ್_MAX      (10 * H3_ಹರಿವು_CHUNK_ಗಾತ್ರ)
#ವ್ಯಾಖ್ಯಾನಿಸು H3_ಹರಿವು_SEND_CHUNKS \
  (H3_ಹರಿವು_SEND_ಬಫರ್_MAX / H3_ಹರಿವು_CHUNK_ಗಾತ್ರ)
/* How much data we initially want to buffer un-acked */
#ವ್ಯಾಖ್ಯಾನಿಸು H3_ಹರಿವು_SEND_BUF_INITIAL     (32 * 1024)

#ವ್ಯಾಖ್ಯಾನಿಸು QUIC_TUNNEL_INGRESS_PKT_ಎಲ್ಲೆ 1000


ಶೂನ್ಯ Curl_ngtcp2_ver(ಅಕ್ಷರ *p, size_t len);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CURLcode cf_ngtcp2_init_h3_conn(ರಚನೆ Curl_cfilter *cf,
                                        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ cf_ngtcp2_ctx *ctx);

ರಚನೆ cf_ngtcp2_ctx {
  ರಚನೆ cf_quic_ctx q;
  ರಚನೆ ssl_peer ssl_peer;
  ರಚನೆ curl_tls_ctx tls;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ OPENSSL_QUIC_API2
  ngtcp2_crypto_ossl_ctx *ossl_ctx;
#ಆದರೆ_ಅಂತ್ಯ
  ngtcp2_ಮಾರ್ಗ connected_ಮಾರ್ಗ;
  ngtcp2_conn *qconn;
  ngtcp2_cid dcid;
  ngtcp2_cid scid;
  uint32_t ಆವೃತ್ತಿ_2;
  ngtcp2_settings settings;
  ngtcp2_transport_params transport_params;
  ngtcp2_ccerr ಕೊನೆಯ_error;
  ngtcp2_crypto_conn_ref conn_ref;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ call_ದತ್ತಾಂಶ;
  cf_ngtcp2_init_h3_conn *init_h3_conn_cb;
  nghttp3_conn *h3conn;
  nghttp3_settings h3settings;
  ರಚನೆ curltime started_at;       /* time the current attempt started */
  ರಚನೆ curltime handshake_at;     /* time connect handshake finished */
  ರಚನೆ bufc_pool ಹರಿವು_bufcp;    /* chunk pool for streams */
  ರಚನೆ dynbuf scratch;            /* temp buffer for header construction */
  ರಚನೆ u32_ptrset streams;        /* set of data->mid + h3_stream_ctx */
  uint64_t ಬಳಸಿದ_bidi_streams;       /* bidi streams we have opened */
  uint64_t max_bidi_streams;        /* max bidi streams we can open */
  size_t earlydata_max;             /* max amount of early data supported by
                                       server on session reuse */
  size_t earlydata_skip;            /* sending bytes to skip when earlydata
                                       is accepted by peer */
  CURLcode tls_vrfy_ಫಲಿತಾಂಶ;         /* result of TLS peer verification */
  ಪೂರ್ಣಾಂಕ qlogfd;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *tunnel_inbuf;      /* ingress buffer for tunneled packets */
  size_t tunnel_inbuf_len;
  ಬಿಟ್(initialized);
  ಬಿಟ್(tls_handshake_complete);      /* TLS handshake is done */
  ಬಿಟ್(use_earlydata);               /* Using 0RTT data */
  ಬಿಟ್(earlydata_accepted);          /* 0RTT was accepted by server */
  ಬಿಟ್(shutdown_started);            /* queued shutdown packets */
  ಬಿಟ್(stats_reported);              /* connect statistics reported */
};

/* How to access `call_data` from a cf_ngtcp2 filter */
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು CF_CTX_CALL_ದತ್ತಾಂಶ
#ವ್ಯಾಖ್ಯಾನಿಸು CF_CTX_CALL_ದತ್ತಾಂಶ(cf) ((ರಚನೆ cf_ngtcp2_ctx *)(cf)->ctx)->call_ದತ್ತಾಂಶ

CURLcode Curl_cf_ngtcp2_ctx_init(ರಚನೆ cf_ngtcp2_ctx *ctx,
                                 ರಚನೆ Curl_peer *origin,
                                 ರಚನೆ Curl_peer *peer,
                                 ರಚನೆ ssl_filter_config *sslc,
                                 cf_ngtcp2_init_h3_conn *init_h3_conn_cb);
ಶೂನ್ಯ Curl_cf_ngtcp2_ctx_cleanup(ರಚನೆ cf_ngtcp2_ctx *ctx);
ಶೂನ್ಯ Curl_cf_ngtcp2_cmn_err_ಗಣ(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2);

/**
 * All about the H3 internals of a stream
 */
ರಚನೆ h3_ಹರಿವು_ctx {
  int64_t id;                   /* HTTP/3 stream identifier */
  curl_off_t xfer_id;           /* `id` of transfer owning this stream */
  ರಚನೆ bufq sendbuf;          /* h3 request body */
  ರಚನೆ h1_req_parser h1;      /* h1 request parsing */
  uint64_t error3;              /* HTTP/3 stream error code */
  curl_off_t upload_ಎಡ;       /* number of request bytes left to upload */
  curl_off_t rx_ಒಟ್ಟು;          /* total number of bytes received */
  uint64_t rx_ಸ್ಥಾನಾಂತರ;           /* current receive offset */
  uint64_t rx_ಸ್ಥಾನಾಂತರ_max;       /* allowed receive offset */
  size_t tx_ಒಳ_flight_ಗಾತ್ರ;     /* sendbuf data "in flight" */
  size_t tx_ಒಳ_flight_ideal;    /* ideal amount of un-acked send data */
  uint32_t mid;                 /* `mid` of transfer owning this stream */
  int32_t ಸ್ಥಿತಿ_ಸಂಕೇತ;              /* HTTP status code */
  CURLcode xfer_ಫಲಿತಾಂಶ;         /* result from xfer_resp_write(_hd) */
  ಬಿಟ್(resp_hds_complete);       /* we have a complete, final response */
  ಬಿಟ್(closed);                  /* TRUE on stream close */
  ಬಿಟ್(reset);                   /* TRUE on stream reset */
  ಬಿಟ್(send_closed);             /* stream is local closed */
  ಬಿಟ್(quic_flow_blocked);       /* stream is blocked by QUIC flow control */
};

ಶೂನ್ಯ Curl_cf_ngtcp2_h3_ಹರಿವು_ctx_free(ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು);
ಶೂನ್ಯ Curl_cf_ngtcp2_h3_err_ಗಣ(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2);

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(ರಚನೆ cf_ngtcp2_ctx *ctx,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

CURLcode Curl_cf_ngtcp2_cmn_connect(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    bool *ಮುಗಿದ);

CURLcode Curl_cf_ngtcp2_cmn_shutdown(ರಚನೆ Curl_cfilter *cf,
                                     ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ);
ಶೂನ್ಯ Curl_cf_ngtcp2_cmn_conn_close(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

ರಚನೆ cf_ngtcp2_io_ctx {
  ರಚನೆ Curl_cfilter *cf;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ;
  ರಚನೆ curltime now;
  ngtcp2_tstamp ts;
  ngtcp2_ಮಾರ್ಗ_storage ps;
};

ಶೂನ್ಯ Curl_cf_ngtcp2_io_ctx_init(ರಚನೆ cf_ngtcp2_io_ctx *io_ctx,
                                ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
ಶೂನ್ಯ Curl_cf_ngtcp2_io_ctx_update_ಸಮಯ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ cf_ngtcp2_io_ctx *io_ctx,
                                       ರಚನೆ Curl_cfilter *cf);

CURLcode Curl_cf_ngtcp2_ಪ್ರಗತಿ_egress(ರಚನೆ Curl_cfilter *cf,
                                        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_ಪ್ರಗತಿ_ingress(ರಚನೆ Curl_cfilter *cf,
                                         ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_cmn_ಗಣ_expiry(ರಚನೆ Curl_cfilter *cf,
                                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_h3_ಹರಿವು_ಸಿದ್ಧತೆ(ರಚನೆ Curl_cfilter *cf,
                                        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ h3_ಹರಿವು_ctx **pstream);
ಶೂನ್ಯ Curl_cf_ngtcp2_h3_ಹರಿವು_close(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ h3_ಹರಿವು_ctx *ಹರಿವು);
ಶೂನ್ಯ Curl_cf_ngtcp2_h3_ಹರಿವು_ಮುಗಿದ(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

bool Curl_cf_ngtcp2_cmn_conn_is_alive(ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      bool *ಒಳಹರಿವು_pending);

CURLcode Curl_cf_ngtcp2_cmn_query(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಪೂರ್ಣಾಂಕ query, ಪೂರ್ಣಾಂಕ *pres1, ಶೂನ್ಯ *pres2);

CURLcode Curl_cf_ngtcp2_cmn_cntrl(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಪೂರ್ಣಾಂಕ event, ಪೂರ್ಣಾಂಕ arg1, ಶೂನ್ಯ *arg2);

ರಚನೆ Curl_easy *Curl_cf_ngtcp2_get_xfer(ರಚನೆ Curl_cfilter *cf,
                                          ಶೂನ್ಯ *ಹರಿವು_user_ದತ್ತಾಂಶ,
                                          ರಚನೆ h3_ಹರಿವು_ctx **pstream);

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_VQUIC_CF_NGTCP2_CMN_H */
