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
#अन्तर्भाव "curl_setup.h"

#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#अन्तर्भाव <ngtcp2/ngtcp2.h>
#अन्तर्भाव <ngtcp2/ngtcp2_crypto.h>

#ifdef USE_OPENSSL
#अन्तर्भाव <openssl/err.h>
#यदि defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#अन्तर्भाव <ngtcp2/ngtcp2_crypto_boringssl.h>
#नोचेद्यदि defined(OPENSSL_QUIC_API2)
#अन्तर्भाव <ngtcp2/ngtcp2_crypto_ossl.h>
#अन्यथा
#अन्तर्भाव <ngtcp2/ngtcp2_crypto_quictls.h>
#endif
#अन्तर्भाव "vtls/openssl.h"
#नोचेद्यदि defined(USE_GNUTLS)
#अन्तर्भाव <ngtcp2/ngtcp2_crypto_gnutls.h>
#अन्तर्भाव "vtls/gtls.h"
#नोचेद्यदि defined(USE_WOLFSSL)
#अन्तर्भाव <wolfssl/options.h>
#अन्तर्भाव <wolfssl/ssl.h>
#अन्तर्भाव <wolfssl/quic.h>
#अन्तर्भाव <ngtcp2/ngtcp2_crypto_wolfssl.h>
#अन्तर्भाव "vtls/wolfssl.h"
#endif

#ifdef HAVE_NETINET_UDP_H
#अन्तर्भाव <netinet/udp.h>
#endif

#अन्तर्भाव <nghttp3/nghttp3.h>

#अन्तर्भाव "http1.h"
#अन्तर्भाव "u32_ptrset.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "vquic/vquic_int.h"
#अन्तर्भाव "vquic/vquic-tls.h"

संरचना Curl_cfilter;
संरचना Curl_easy;
संरचना cf_ngtcp2_ctx;
संरचना cf_quic_ctx;

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
#यदि H3_STREAM_CHUNK_SIZE < NGTCP2_MAX_UDP_PAYLOAD_SIZE
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


शून्य Curl_ngtcp2_ver(वर्ण *p, माप_प्रकार len);

प्रकारनिर्देश CURLcode cf_ngtcp2_init_h3_conn(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *data,
                                        संरचना cf_ngtcp2_ctx *ctx);

संरचना cf_ngtcp2_ctx {
  संरचना cf_quic_ctx q;
  संरचना ssl_peer ssl_peer;
  संरचना curl_tls_ctx tls;
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
  संरचना cf_call_data call_data;
  cf_ngtcp2_init_h3_conn *init_h3_conn_cb;
  nghttp3_conn *h3conn;
  nghttp3_settings h3settings;
  संरचना curltime started_at;       /* time the current attempt started */
  संरचना curltime handshake_at;     /* time connect handshake finished */
  संरचना bufc_pool stream_bufcp;    /* chunk pool for streams */
  संरचना dynbuf scratch;            /* temp buffer for header construction */
  संरचना u32_ptrset streams;        /* set of data->mid + h3_stream_ctx */
  uint64_t used_bidi_streams;       /* bidi streams we have opened */
  uint64_t max_bidi_streams;        /* max bidi streams we can open */
  माप_प्रकार earlydata_max;             /* max amount of early data supported by
                                       server on session reuse */
  माप_प्रकार earlydata_skip;            /* sending bytes to skip when earlydata
                                       is accepted by peer */
  CURLcode tls_vrfy_result;         /* result of TLS peer verification */
  पूर्णाङ्क qlogfd;
  अचिन्हित वर्ण *tunnel_inbuf;      /* ingress buffer for tunneled packets */
  माप_प्रकार tunnel_inbuf_len;
  BIT(initialized);
  BIT(tls_handshake_complete);      /* TLS handshake is done */
  BIT(use_earlydata);               /* Using 0RTT data */
  BIT(earlydata_accepted);          /* 0RTT was accepted by server */
  BIT(shutdown_started);            /* queued shutdown packets */
  BIT(stats_reported);              /* connect statistics reported */
};

/* How to access `call_data` from a cf_ngtcp2 filter */
#undef CF_CTX_CALL_DATA
#define CF_CTX_CALL_DATA(cf) ((संरचना cf_ngtcp2_ctx *)(cf)->ctx)->call_data

CURLcode Curl_cf_ngtcp2_ctx_init(संरचना cf_ngtcp2_ctx *ctx,
                                 संरचना Curl_peer *origin,
                                 संरचना Curl_peer *peer,
                                 संरचना ssl_filter_config *sslc,
                                 cf_ngtcp2_init_h3_conn *init_h3_conn_cb);
शून्य Curl_cf_ngtcp2_ctx_cleanup(संरचना cf_ngtcp2_ctx *ctx);
शून्य Curl_cf_ngtcp2_cmn_err_set(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data, पूर्णाङ्क code);

/**
 * All about the H3 internals of a stream
 */
संरचना h3_stream_ctx {
  int64_t id;                   /* HTTP/3 stream identifier */
  curl_off_t xfer_id;           /* `id` of transfer owning this stream */
  संरचना bufq sendbuf;          /* h3 request body */
  संरचना h1_req_parser h1;      /* h1 request parsing */
  uint64_t error3;              /* HTTP/3 stream error code */
  curl_off_t upload_बायाँ;       /* number of request bytes left to upload */
  curl_off_t rx_total;          /* total number of bytes received */
  uint64_t rx_offset;           /* current receive offset */
  uint64_t rx_offset_max;       /* allowed receive offset */
  माप_प्रकार tx_in_flight_size;     /* sendbuf data "in flight" */
  माप_प्रकार tx_in_flight_ideal;    /* ideal amount of un-acked send data */
  uint32_t mid;                 /* `mid` of transfer owning this stream */
  int32_t status_code;              /* HTTP status code */
  CURLcode xfer_result;         /* result from xfer_resp_write(_hd) */
  BIT(resp_hds_complete);       /* we have a complete, final response */
  BIT(closed);                  /* TRUE on stream close */
  BIT(reset);                   /* TRUE on stream reset */
  BIT(send_closed);             /* stream is local closed */
  BIT(quic_flow_blocked);       /* stream is blocked by QUIC flow control */
};

शून्य Curl_cf_ngtcp2_h3_stream_ctx_free(संरचना h3_stream_ctx *stream);
शून्य Curl_cf_ngtcp2_h3_err_set(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data, पूर्णाङ्क code);

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(संरचना cf_ngtcp2_ctx *ctx,
                                      संरचना Curl_easy *data);

CURLcode Curl_cf_ngtcp2_cmn_connect(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    bool *done);

CURLcode Curl_cf_ngtcp2_cmn_shutdown(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *data, bool *done);
शून्य Curl_cf_ngtcp2_cmn_conn_close(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data);

संरचना cf_ngtcp2_io_ctx {
  संरचना Curl_cfilter *cf;
  संरचना Curl_easy *data;
  संरचना curltime now;
  ngtcp2_tstamp ts;
  ngtcp2_path_storage ps;
};

शून्य Curl_cf_ngtcp2_io_ctx_init(संरचना cf_ngtcp2_io_ctx *io_ctx,
                                संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data);
शून्य Curl_cf_ngtcp2_io_ctx_update_time(संरचना Curl_easy *data,
                                       संरचना cf_ngtcp2_io_ctx *io_ctx,
                                       संरचना Curl_cfilter *cf);

CURLcode Curl_cf_ngtcp2_progress_egress(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *data,
                                        संरचना cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_progress_ingress(संरचना Curl_cfilter *cf,
                                         संरचना Curl_easy *data,
                                         संरचना cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_cmn_set_expiry(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *data,
                                       संरचना cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_h3_stream_setup(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *data,
                                        संरचना h3_stream_ctx **pstream);
शून्य Curl_cf_ngtcp2_h3_stream_close(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    संरचना h3_stream_ctx *stream);
शून्य Curl_cf_ngtcp2_h3_stream_done(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data);

bool Curl_cf_ngtcp2_cmn_conn_is_alive(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *data,
                                      bool *निवेश_pending);

CURLcode Curl_cf_ngtcp2_cmn_query(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2);

CURLcode Curl_cf_ngtcp2_cmn_cntrl(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  पूर्णाङ्क event, पूर्णाङ्क arg1, शून्य *arg2);

संरचना Curl_easy *Curl_cf_ngtcp2_get_xfer(संरचना Curl_cfilter *cf,
                                          शून्य *stream_user_data,
                                          संरचना h3_stream_ctx **pstream);

#endif /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */

#endif /* HEADER_CURL_VQUIC_CF_NGTCP2_CMN_H */
