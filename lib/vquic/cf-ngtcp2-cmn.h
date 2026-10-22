#अगर_अघोषित HEADER_CURL_VQUIC_CF_NGTCP2_CMN_H
#घोषणा HEADER_CURL_VQUIC_CF_NGTCP2_CMN_H
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
#समावेश "curl_setup.h"

#अगर !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#समावेश <ngtcp2/ngtcp2.h>
#समावेश <ngtcp2/ngtcp2_crypto.h>

#अगर_घोषित USE_OPENSSL
#समावेश <openssl/err.h>
#अगर defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#समावेश <ngtcp2/ngtcp2_crypto_boringssl.h>
#या_अगर defined(OPENSSL_QUIC_API2)
#समावेश <ngtcp2/ngtcp2_crypto_ossl.h>
#अन्यथा
#समावेश <ngtcp2/ngtcp2_crypto_quictls.h>
#पूर्ण_अगर
#समावेश "vtls/openssl.h"
#या_अगर defined(USE_GNUTLS)
#समावेश <ngtcp2/ngtcp2_crypto_gnutls.h>
#समावेश "vtls/gtls.h"
#या_अगर defined(USE_WOLFSSL)
#समावेश <wolfssl/options.h>
#समावेश <wolfssl/ssl.h>
#समावेश <wolfssl/quic.h>
#समावेश <ngtcp2/ngtcp2_crypto_wolfssl.h>
#समावेश "vtls/wolfssl.h"
#पूर्ण_अगर

#अगर_घोषित HAVE_NETINET_UDP_H
#समावेश <netinet/udp.h>
#पूर्ण_अगर

#समावेश <nghttp3/nghttp3.h>

#समावेश "http1.h"
#समावेश "u32_ptrset.h"
#समावेश "vtls/vtls.h"
#समावेश "vquic/vquic_int.h"
#समावेश "vquic/vquic-tls.h"

काष्ठा Curl_cfilter;
काष्ठा Curl_easy;
काष्ठा cf_ngtcp2_ctx;
काष्ठा cf_quic_ctx;

#घोषणा QUIC_MAX_STREAMS       (256 * 1024)
#घोषणा QUIC_HANDSHAKE_TIMEOUT (10 * NGTCP2_SECONDS)
#घोषणा QUIC_TUNNEL_INBUF_आकार (64 * 1024)

/* We announce a small window size in transport param to the server,
 * and grow that immediately to max when no rate limit is in place.
 * We need to start small as we are not able to decrease it. */
#घोषणा H3_STREAM_WINDOW_आकार_INITIAL (32 * 1024)
#घोषणा H3_STREAM_WINDOW_आकार_MAX     (10 * 1024 * 1024)
#घोषणा H3_CONN_WINDOW_आकार_MAX       (100 * H3_STREAM_WINDOW_आकार_MAX)

#घोषणा H3_STREAM_CHUNK_आकार  (64 * 1024)
#अगर H3_STREAM_CHUNK_आकार < NGTCP2_MAX_UDP_PAYLOAD_आकार
#त्रुटि H3_STREAM_CHUNK_आकार smaller than NGTCP2_MAX_UDP_PAYLOAD_आकार
#पूर्ण_अगर
/* The pool keeps spares around for all streams.
 * The benefit of the pool is that stream buffers do not keep
 * spares. Memory consumption goes down when streams run empty,
 * have a large upload done, etc. */
#घोषणा H3_STREAM_POOL_SPARES      5
/* The max amount of un-acked upload data we keep around per stream */
#घोषणा H3_STREAM_SEND_बफर_MAX      (10 * H3_STREAM_CHUNK_आकार)
#घोषणा H3_STREAM_SEND_CHUNKS \
  (H3_STREAM_SEND_बफर_MAX / H3_STREAM_CHUNK_आकार)
/* How much data we initially want to buffer un-acked */
#घोषणा H3_STREAM_SEND_BUF_INITIAL     (32 * 1024)

#घोषणा QUIC_TUNNEL_INGRESS_PKT_LIMIT 1000


व्योम Curl_ngtcp2_ver(अक्षर *p, माप_प्रकार len);

प्रकार CURLcode cf_ngtcp2_init_h3_conn(काष्ठा Curl_cfilter *cf,
                                        काष्ठा Curl_easy *तथ्य,
                                        काष्ठा cf_ngtcp2_ctx *ctx);

काष्ठा cf_ngtcp2_ctx {
  काष्ठा cf_quic_ctx q;
  काष्ठा ssl_peer ssl_peer;
  काष्ठा curl_tls_ctx tls;
#अगर_घोषित OPENSSL_QUIC_API2
  ngtcp2_crypto_ossl_ctx *ossl_ctx;
#पूर्ण_अगर
  ngtcp2_path connected_path;
  ngtcp2_conn *qconn;
  ngtcp2_cid dcid;
  ngtcp2_cid scid;
  uint32_t version;
  ngtcp2_settings settings;
  ngtcp2_transport_params transport_params;
  ngtcp2_ccerr last_error;
  ngtcp2_crypto_conn_ref conn_ref;
  काष्ठा cf_call_तथ्य call_तथ्य;
  cf_ngtcp2_init_h3_conn *init_h3_conn_cb;
  nghttp3_conn *h3conn;
  nghttp3_settings h3settings;
  काष्ठा curltime started_at;       /* time the current attempt started */
  काष्ठा curltime handshake_at;     /* time connect handshake finished */
  काष्ठा bufc_pool stream_bufcp;    /* chunk pool for streams */
  काष्ठा dynbuf scratch;            /* temp buffer for header construction */
  काष्ठा u32_ptrset streams;        /* set of data->mid + h3_stream_ctx */
  uint64_t used_bidi_streams;       /* bidi streams we have opened */
  uint64_t max_bidi_streams;        /* max bidi streams we can open */
  माप_प्रकार earlydata_max;             /* max amount of early data supported by
                                       server on session reuse */
  माप_प्रकार earlydata_skip;            /* sending bytes to skip when earlydata
                                       is accepted by peer */
  CURLcode tls_vrfy_परिणाम;         /* result of TLS peer verification */
  पूर्णांक qlogfd;
  अचिन्हित अक्षर *tunnel_inbuf;      /* ingress buffer for tunneled packets */
  माप_प्रकार tunnel_inbuf_len;
  BIT(initialized);
  BIT(tls_handshake_complete);      /* TLS handshake is done */
  BIT(use_earlydata);               /* Using 0RTT data */
  BIT(earlydata_accepted);          /* 0RTT was accepted by server */
  BIT(shutdown_started);            /* queued shutdown packets */
  BIT(stats_reported);              /* connect statistics reported */
};

/* How to access `call_data` from a cf_ngtcp2 filter */
#अघोषित CF_CTX_CALL_तथ्य
#घोषणा CF_CTX_CALL_तथ्य(cf) ((काष्ठा cf_ngtcp2_ctx *)(cf)->ctx)->call_तथ्य

CURLcode Curl_cf_ngtcp2_ctx_init(काष्ठा cf_ngtcp2_ctx *ctx,
                                 काष्ठा Curl_peer *origin,
                                 काष्ठा Curl_peer *peer,
                                 काष्ठा ssl_filter_config *sslc,
                                 cf_ngtcp2_init_h3_conn *init_h3_conn_cb);
व्योम Curl_cf_ngtcp2_ctx_cleanup(काष्ठा cf_ngtcp2_ctx *ctx);
व्योम Curl_cf_ngtcp2_cmn_err_set(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य, पूर्णांक code);

/**
 * All about the H3 internals of a stream
 */
काष्ठा h3_stream_ctx {
  int64_t id;                   /* HTTP/3 stream identifier */
  curl_off_t xfer_id;           /* `id` of transfer owning this stream */
  काष्ठा bufq sendbuf;          /* h3 request body */
  काष्ठा h1_req_parser h1;      /* h1 request parsing */
  uint64_t error3;              /* HTTP/3 stream error code */
  curl_off_t upload_बायाँ;       /* number of request bytes left to upload */
  curl_off_t rx_योग;          /* total number of bytes received */
  uint64_t rx_offset;           /* current receive offset */
  uint64_t rx_offset_max;       /* allowed receive offset */
  माप_प्रकार tx_in_flight_आकार;     /* sendbuf data "in flight" */
  माप_प्रकार tx_in_flight_ideal;    /* ideal amount of un-acked send data */
  uint32_t mid;                 /* `mid` of transfer owning this stream */
  int32_t status_code;              /* HTTP status code */
  CURLcode xfer_परिणाम;         /* result from xfer_resp_write(_hd) */
  BIT(resp_hds_complete);       /* we have a complete, final response */
  BIT(closed);                  /* TRUE on stream close */
  BIT(reset);                   /* TRUE on stream reset */
  BIT(send_closed);             /* stream is local closed */
  BIT(quic_flow_blocked);       /* stream is blocked by QUIC flow control */
};

व्योम Curl_cf_ngtcp2_h3_stream_ctx_free(काष्ठा h3_stream_ctx *stream);
व्योम Curl_cf_ngtcp2_h3_err_set(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य, पूर्णांक code);

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(काष्ठा cf_ngtcp2_ctx *ctx,
                                      काष्ठा Curl_easy *तथ्य);

CURLcode Curl_cf_ngtcp2_cmn_connect(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य,
                                    bool *done);

CURLcode Curl_cf_ngtcp2_cmn_shutdown(काष्ठा Curl_cfilter *cf,
                                     काष्ठा Curl_easy *तथ्य, bool *done);
व्योम Curl_cf_ngtcp2_cmn_conn_close(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य);

काष्ठा cf_ngtcp2_io_ctx {
  काष्ठा Curl_cfilter *cf;
  काष्ठा Curl_easy *तथ्य;
  काष्ठा curltime now;
  ngtcp2_tstamp ts;
  ngtcp2_path_storage ps;
};

व्योम Curl_cf_ngtcp2_io_ctx_init(काष्ठा cf_ngtcp2_io_ctx *io_ctx,
                                काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य);
व्योम Curl_cf_ngtcp2_io_ctx_update_time(काष्ठा Curl_easy *तथ्य,
                                       काष्ठा cf_ngtcp2_io_ctx *io_ctx,
                                       काष्ठा Curl_cfilter *cf);

CURLcode Curl_cf_ngtcp2_progress_egress(काष्ठा Curl_cfilter *cf,
                                        काष्ठा Curl_easy *तथ्य,
                                        काष्ठा cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_progress_ingress(काष्ठा Curl_cfilter *cf,
                                         काष्ठा Curl_easy *तथ्य,
                                         काष्ठा cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_cmn_set_expiry(काष्ठा Curl_cfilter *cf,
                                       काष्ठा Curl_easy *तथ्य,
                                       काष्ठा cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_h3_stream_setup(काष्ठा Curl_cfilter *cf,
                                        काष्ठा Curl_easy *तथ्य,
                                        काष्ठा h3_stream_ctx **pstream);
व्योम Curl_cf_ngtcp2_h3_stream_close(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य,
                                    काष्ठा h3_stream_ctx *stream);
व्योम Curl_cf_ngtcp2_h3_stream_done(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य);

bool Curl_cf_ngtcp2_cmn_conn_is_alive(काष्ठा Curl_cfilter *cf,
                                      काष्ठा Curl_easy *तथ्य,
                                      bool *निवेश_pending);

CURLcode Curl_cf_ngtcp2_cmn_query(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य,
                                  पूर्णांक query, पूर्णांक *pres1, व्योम *pres2);

CURLcode Curl_cf_ngtcp2_cmn_cntrl(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य,
                                  पूर्णांक event, पूर्णांक arg1, व्योम *arg2);

काष्ठा Curl_easy *Curl_cf_ngtcp2_get_xfer(काष्ठा Curl_cfilter *cf,
                                          व्योम *stream_user_तथ्य,
                                          काष्ठा h3_stream_ctx **pstream);

#पूर्ण_अगर /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */

#पूर्ण_अगर /* HEADER_CURL_VQUIC_CF_NGTCP2_CMN_H */
