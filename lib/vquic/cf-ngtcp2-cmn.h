#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_VQUIC_CF_NGTCP2_CMN_H
#وصف_ڪريو سرو_CURL_VQUIC_CF_NGTCP2_CMN_H
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
#ضمن "curl_setup.h"

#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#ضمن <ngtcp2/ngtcp2.h>
#ضمن <ngtcp2/ngtcp2_crypto.h>

#جيڪڏهن_وصف_ٿيل USE_OPENSSL
#ضمن <openssl/err.h>
#إذا defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#ضمن <ngtcp2/ngtcp2_crypto_boringssl.h>
#وإلاإذا defined(OPENSSL_QUIC_API2)
#ضمن <ngtcp2/ngtcp2_crypto_ossl.h>
#وإلا
#ضمن <ngtcp2/ngtcp2_crypto_quictls.h>
#جيڪڏهن_پورو
#ضمن "vtls/openssl.h"
#وإلاإذا defined(USE_GNUTLS)
#ضمن <ngtcp2/ngtcp2_crypto_gnutls.h>
#ضمن "vtls/gtls.h"
#وإلاإذا defined(USE_WOLFSSL)
#ضمن <wolfssl/options.h>
#ضمن <wolfssl/ssl.h>
#ضمن <wolfssl/quic.h>
#ضمن <ngtcp2/ngtcp2_crypto_wolfssl.h>
#ضمن "vtls/wolfssl.h"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_NETINET_UDP_H
#ضمن <netinet/udp.h>
#جيڪڏهن_پورو

#ضمن <nghttp3/nghttp3.h>

#ضمن "http1.h"
#ضمن "u32_ptrset.h"
#ضمن "vtls/vtls.h"
#ضمن "vquic/vquic_int.h"
#ضمن "vquic/vquic-tls.h"

بنية Curl_cfilter;
بنية Curl_easy;
بنية cf_ngtcp2_ctx;
بنية cf_quic_ctx;

#وصف_ڪريو QUIC_MAX_STREAMS       (256 * 1024)
#وصف_ڪريو QUIC_HANDSHAKE_TIMEOUT (10 * NGTCP2_SECONDS)
#وصف_ڪريو QUIC_TUNNEL_INBUF_سائيز (64 * 1024)

/* We announce a small window size in transport param to the server,
 * and grow that immediately to max when no rate limit is in place.
 * We need to start small as we are not able to decrease it. */
#وصف_ڪريو H3_وهڪرو_دري_سائيز_INITIAL (32 * 1024)
#وصف_ڪريو H3_وهڪرو_دري_سائيز_MAX     (10 * 1024 * 1024)
#وصف_ڪريو H3_CONN_دري_سائيز_MAX       (100 * H3_وهڪرو_دري_سائيز_MAX)

#وصف_ڪريو H3_وهڪرو_CHUNK_سائيز  (64 * 1024)
#إذا H3_وهڪرو_CHUNK_سائيز < NGTCP2_MAX_UDP_PAYLOAD_سائيز
#غلطي H3_وهڪرو_CHUNK_سائيز smaller than NGTCP2_MAX_UDP_PAYLOAD_سائيز
#جيڪڏهن_پورو
/* The pool keeps spares around for all streams.
 * The benefit of the pool is that stream buffers do not keep
 * spares. Memory consumption goes down when streams run empty,
 * have a large upload done, etc. */
#وصف_ڪريو H3_وهڪرو_POOL_SPARES      5
/* The max amount of un-acked upload data we keep around per stream */
#وصف_ڪريو H3_وهڪرو_SEND_بفر_MAX      (10 * H3_وهڪرو_CHUNK_سائيز)
#وصف_ڪريو H3_وهڪرو_SEND_CHUNKS \
  (H3_وهڪرو_SEND_بفر_MAX / H3_وهڪرو_CHUNK_سائيز)
/* How much data we initially want to buffer un-acked */
#وصف_ڪريو H3_وهڪرو_SEND_BUF_INITIAL     (32 * 1024)

#وصف_ڪريو QUIC_TUNNEL_INGRESS_PKT_حد_بندي 1000


فراغ Curl_ngtcp2_ver(حرف *p, size_t len);

تعريفنوع CURLcode cf_ngtcp2_init_h3_conn(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *معلومات,
                                        بنية cf_ngtcp2_ctx *ctx);

بنية cf_ngtcp2_ctx {
  بنية cf_quic_ctx q;
  بنية ssl_peer ssl_peer;
  بنية curl_tls_ctx tls;
#جيڪڏهن_وصف_ٿيل OPENSSL_QUIC_API2
  ngtcp2_crypto_ossl_ctx *ossl_ctx;
#جيڪڏهن_پورو
  ngtcp2_رستو connected_رستو;
  ngtcp2_conn *qconn;
  ngtcp2_cid dcid;
  ngtcp2_cid scid;
  uint32_t نسخو_2;
  ngtcp2_settings settings;
  ngtcp2_transport_params transport_params;
  ngtcp2_ccerr آخري_error;
  ngtcp2_crypto_conn_ref conn_ref;
  بنية cf_call_معلومات call_معلومات;
  cf_ngtcp2_init_h3_conn *init_h3_conn_cb;
  nghttp3_conn *h3conn;
  nghttp3_settings h3settings;
  بنية curltime started_at;       /* time the current attempt started */
  بنية curltime handshake_at;     /* time connect handshake finished */
  بنية bufc_pool وهڪرو_bufcp;    /* chunk pool for streams */
  بنية dynbuf scratch;            /* temp buffer for header construction */
  بنية u32_ptrset streams;        /* set of data->mid + h3_stream_ctx */
  uint64_t استعمال_ٿيل_bidi_streams;       /* bidi streams we have opened */
  uint64_t max_bidi_streams;        /* max bidi streams we can open */
  size_t earlydata_max;             /* max amount of early data supported by
                                       server on session reuse */
  size_t earlydata_skip;            /* sending bytes to skip when earlydata
                                       is accepted by peer */
  CURLcode tls_vrfy_نتيجو;         /* result of TLS peer verification */
  صحيح qlogfd;
  بنانشان حرف *tunnel_inbuf;      /* ingress buffer for tunneled packets */
  size_t tunnel_inbuf_len;
  ٻٽو_انگ(initialized);
  ٻٽو_انگ(tls_handshake_complete);      /* TLS handshake is done */
  ٻٽو_انگ(use_earlydata);               /* Using 0RTT data */
  ٻٽو_انگ(earlydata_accepted);          /* 0RTT was accepted by server */
  ٻٽو_انگ(shutdown_started);            /* queued shutdown packets */
  ٻٽو_انگ(stats_reported);              /* connect statistics reported */
};

/* How to access `call_data` from a cf_ngtcp2 filter */
#وصف_رد CF_CTX_CALL_معلومات
#وصف_ڪريو CF_CTX_CALL_معلومات(cf) ((بنية cf_ngtcp2_ctx *)(cf)->ctx)->call_معلومات

CURLcode Curl_cf_ngtcp2_ctx_init(بنية cf_ngtcp2_ctx *ctx,
                                 بنية Curl_peer *origin,
                                 بنية Curl_peer *peer,
                                 بنية ssl_filter_config *sslc,
                                 cf_ngtcp2_init_h3_conn *init_h3_conn_cb);
فراغ Curl_cf_ngtcp2_ctx_cleanup(بنية cf_ngtcp2_ctx *ctx);
فراغ Curl_cf_ngtcp2_cmn_err_سيٽ(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات, صحيح ڪوڊ);

/**
 * All about the H3 internals of a stream
 */
بنية h3_وهڪرو_ctx {
  int64_t id;                   /* HTTP/3 stream identifier */
  curl_off_t xfer_id;           /* `id` of transfer owning this stream */
  بنية bufq sendbuf;          /* h3 request body */
  بنية h1_req_parser h1;      /* h1 request parsing */
  uint64_t error3;              /* HTTP/3 stream error code */
  curl_off_t upload_کاٻو;       /* number of request bytes left to upload */
  curl_off_t rx_ڪل;          /* total number of bytes received */
  uint64_t rx_وٿي;           /* current receive offset */
  uint64_t rx_وٿي_max;       /* allowed receive offset */
  size_t tx_اندريون_flight_سائيز;     /* sendbuf data "in flight" */
  size_t tx_اندريون_flight_ideal;    /* ideal amount of un-acked send data */
  uint32_t mid;                 /* `mid` of transfer owning this stream */
  int32_t حالت_ڪوڊ;              /* HTTP status code */
  CURLcode xfer_نتيجو;         /* result from xfer_resp_write(_hd) */
  ٻٽو_انگ(resp_hds_complete);       /* we have a complete, final response */
  ٻٽو_انگ(closed);                  /* TRUE on stream close */
  ٻٽو_انگ(reset);                   /* TRUE on stream reset */
  ٻٽو_انگ(send_closed);             /* stream is local closed */
  ٻٽو_انگ(quic_flow_blocked);       /* stream is blocked by QUIC flow control */
};

فراغ Curl_cf_ngtcp2_h3_وهڪرو_ctx_free(بنية h3_وهڪرو_ctx *وهڪرو);
فراغ Curl_cf_ngtcp2_h3_err_سيٽ(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات, صحيح ڪوڊ);

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(بنية cf_ngtcp2_ctx *ctx,
                                      بنية Curl_easy *معلومات);

CURLcode Curl_cf_ngtcp2_cmn_connect(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    bool *پورو);

CURLcode Curl_cf_ngtcp2_cmn_shutdown(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *معلومات, bool *پورو);
فراغ Curl_cf_ngtcp2_cmn_conn_close(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات);

بنية cf_ngtcp2_io_ctx {
  بنية Curl_cfilter *cf;
  بنية Curl_easy *معلومات;
  بنية curltime now;
  ngtcp2_tstamp ts;
  ngtcp2_رستو_storage ps;
};

فراغ Curl_cf_ngtcp2_io_ctx_init(بنية cf_ngtcp2_io_ctx *io_ctx,
                                بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات);
فراغ Curl_cf_ngtcp2_io_ctx_update_وقت(بنية Curl_easy *معلومات,
                                       بنية cf_ngtcp2_io_ctx *io_ctx,
                                       بنية Curl_cfilter *cf);

CURLcode Curl_cf_ngtcp2_ترقي_egress(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *معلومات,
                                        بنية cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_ترقي_ingress(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *معلومات,
                                         بنية cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_cmn_سيٽ_expiry(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *معلومات,
                                       بنية cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_h3_وهڪرو_تياري(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *معلومات,
                                        بنية h3_وهڪرو_ctx **pstream);
فراغ Curl_cf_ngtcp2_h3_وهڪرو_close(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    بنية h3_وهڪرو_ctx *وهڪرو);
فراغ Curl_cf_ngtcp2_h3_وهڪرو_پورو(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات);

bool Curl_cf_ngtcp2_cmn_conn_is_alive(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات,
                                      bool *داخلا_pending);

CURLcode Curl_cf_ngtcp2_cmn_query(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  صحيح query, صحيح *pres1, فراغ *pres2);

CURLcode Curl_cf_ngtcp2_cmn_cntrl(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  صحيح event, صحيح arg1, فراغ *arg2);

بنية Curl_easy *Curl_cf_ngtcp2_get_xfer(بنية Curl_cfilter *cf,
                                          فراغ *وهڪرو_user_معلومات,
                                          بنية h3_وهڪرو_ctx **pstream);

#جيڪڏهن_پورو /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */

#جيڪڏهن_پورو /* HEADER_CURL_VQUIC_CF_NGTCP2_CMN_H */
