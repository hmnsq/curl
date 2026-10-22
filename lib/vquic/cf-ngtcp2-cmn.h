#اگر_غیر_تعریف_شدہ سرخی_CURL_VQUIC_CF_NGTCP2_CMN_H
#تعریف_کرو سرخی_CURL_VQUIC_CF_NGTCP2_CMN_H
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
#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_NGTCP2) && defined(USE_NGHTTP3)

#شامل <ngtcp2/ngtcp2.h>
#شامل <ngtcp2/ngtcp2_crypto.h>

#اگر_تعریف_شدہ USE_OPENSSL
#شامل <openssl/err.h>
#اگر defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#شامل <ngtcp2/ngtcp2_crypto_boringssl.h>
#وگرنہ_اگر defined(OPENSSL_QUIC_API2)
#شامل <ngtcp2/ngtcp2_crypto_ossl.h>
#ورنہ
#شامل <ngtcp2/ngtcp2_crypto_quictls.h>
#اگر_ختم
#شامل "vtls/openssl.h"
#وگرنہ_اگر defined(USE_GNUTLS)
#شامل <ngtcp2/ngtcp2_crypto_gnutls.h>
#شامل "vtls/gtls.h"
#وگرنہ_اگر defined(USE_WOLFSSL)
#شامل <wolfssl/options.h>
#شامل <wolfssl/ssl.h>
#شامل <wolfssl/quic.h>
#شامل <ngtcp2/ngtcp2_crypto_wolfssl.h>
#شامل "vtls/wolfssl.h"
#اگر_ختم

#اگر_تعریف_شدہ HAVE_NETINET_UDP_H
#شامل <netinet/udp.h>
#اگر_ختم

#شامل <nghttp3/nghttp3.h>

#شامل "http1.h"
#شامل "u32_ptrset.h"
#شامل "vtls/vtls.h"
#شامل "vquic/vquic_int.h"
#شامل "vquic/vquic-tls.h"

ساخت Curl_cfilter;
ساخت Curl_easy;
ساخت cf_ngtcp2_ctx;
ساخت cf_quic_ctx;

#تعریف_کرو QUIC_MAX_STREAMS       (256 * 1024)
#تعریف_کرو QUIC_HANDSHAKE_TIMEOUT (10 * NGTCP2_SECONDS)
#تعریف_کرو QUIC_TUNNEL_INBUF_سائز (64 * 1024)

/* We announce a small window size in transport param to the server,
 * and grow that immediately to max when no rate limit is in place.
 * We need to start small as we are not able to decrease it. */
#تعریف_کرو H3_دھارا_کھڑکی_سائز_INITIAL (32 * 1024)
#تعریف_کرو H3_دھارا_کھڑکی_سائز_MAX     (10 * 1024 * 1024)
#تعریف_کرو H3_CONN_کھڑکی_سائز_MAX       (100 * H3_دھارا_کھڑکی_سائز_MAX)

#تعریف_کرو H3_دھارا_CHUNK_سائز  (64 * 1024)
#اگر H3_دھارا_CHUNK_سائز < NGTCP2_MAX_UDP_PAYLOAD_سائز
#غلطی H3_دھارا_CHUNK_سائز smaller than NGTCP2_MAX_UDP_PAYLOAD_سائز
#اگر_ختم
/* The pool keeps spares around for all streams.
 * The benefit of the pool is that stream buffers do not keep
 * spares. Memory consumption goes down when streams run empty,
 * have a large upload done, etc. */
#تعریف_کرو H3_دھارا_POOL_SPARES      5
/* The max amount of un-acked upload data we keep around per stream */
#تعریف_کرو H3_دھارا_SEND_بفر_MAX      (10 * H3_دھارا_CHUNK_سائز)
#تعریف_کرو H3_دھارا_SEND_CHUNKS \
  (H3_دھارا_SEND_بفر_MAX / H3_دھارا_CHUNK_سائز)
/* How much data we initially want to buffer un-acked */
#تعریف_کرو H3_دھارا_SEND_BUF_INITIAL     (32 * 1024)

#تعریف_کرو QUIC_TUNNEL_INGRESS_PKT_حد_بندی 1000


خالی Curl_ngtcp2_ver(حرف *p, size_t len);

قسمتعریف CURLcode cf_ngtcp2_init_h3_conn(ساخت Curl_cfilter *cf,
                                        ساخت Curl_easy *ڈیٹا,
                                        ساخت cf_ngtcp2_ctx *ctx);

ساخت cf_ngtcp2_ctx {
  ساخت cf_quic_ctx q;
  ساخت ssl_peer ssl_peer;
  ساخت curl_tls_ctx tls;
#اگر_تعریف_شدہ OPENSSL_QUIC_API2
  ngtcp2_crypto_ossl_ctx *ossl_ctx;
#اگر_ختم
  ngtcp2_راستہ connected_راستہ;
  ngtcp2_conn *qconn;
  ngtcp2_cid dcid;
  ngtcp2_cid scid;
  uint32_t نسخہ_2;
  ngtcp2_settings settings;
  ngtcp2_transport_params transport_params;
  ngtcp2_ccerr آخری_error;
  ngtcp2_crypto_conn_ref conn_ref;
  ساخت cf_call_ڈیٹا call_ڈیٹا;
  cf_ngtcp2_init_h3_conn *init_h3_conn_cb;
  nghttp3_conn *h3conn;
  nghttp3_settings h3settings;
  ساخت curltime started_at;       /* time the current attempt started */
  ساخت curltime handshake_at;     /* time connect handshake finished */
  ساخت bufc_pool دھارا_bufcp;    /* chunk pool for streams */
  ساخت dynbuf scratch;            /* temp buffer for header construction */
  ساخت u32_ptrset streams;        /* set of data->mid + h3_stream_ctx */
  uint64_t مستعمل_bidi_streams;       /* bidi streams we have opened */
  uint64_t max_bidi_streams;        /* max bidi streams we can open */
  size_t earlydata_max;             /* max amount of early data supported by
                                       server on session reuse */
  size_t earlydata_skip;            /* sending bytes to skip when earlydata
                                       is accepted by peer */
  CURLcode tls_vrfy_نتیجہ;         /* result of TLS peer verification */
  صحیح qlogfd;
  غیرعلامتی حرف *tunnel_inbuf;      /* ingress buffer for tunneled packets */
  size_t tunnel_inbuf_len;
  ثنائی_ہندسہ(initialized);
  ثنائی_ہندسہ(tls_handshake_complete);      /* TLS handshake is done */
  ثنائی_ہندسہ(use_earlydata);               /* Using 0RTT data */
  ثنائی_ہندسہ(earlydata_accepted);          /* 0RTT was accepted by server */
  ثنائی_ہندسہ(shutdown_started);            /* queued shutdown packets */
  ثنائی_ہندسہ(stats_reported);              /* connect statistics reported */
};

/* How to access `call_data` from a cf_ngtcp2 filter */
#تعریف_منسوخ CF_CTX_CALL_ڈیٹا
#تعریف_کرو CF_CTX_CALL_ڈیٹا(cf) ((ساخت cf_ngtcp2_ctx *)(cf)->ctx)->call_ڈیٹا

CURLcode Curl_cf_ngtcp2_ctx_init(ساخت cf_ngtcp2_ctx *ctx,
                                 ساخت Curl_peer *origin,
                                 ساخت Curl_peer *peer,
                                 ساخت ssl_filter_config *sslc,
                                 cf_ngtcp2_init_h3_conn *init_h3_conn_cb);
خالی Curl_cf_ngtcp2_ctx_cleanup(ساخت cf_ngtcp2_ctx *ctx);
خالی Curl_cf_ngtcp2_cmn_err_سیٹ(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا, صحیح رمز);

/**
 * All about the H3 internals of a stream
 */
ساخت h3_دھارا_ctx {
  int64_t id;                   /* HTTP/3 stream identifier */
  curl_off_t xfer_id;           /* `id` of transfer owning this stream */
  ساخت bufq sendbuf;          /* h3 request body */
  ساخت h1_req_parser h1;      /* h1 request parsing */
  uint64_t error3;              /* HTTP/3 stream error code */
  curl_off_t upload_بایاں;       /* number of request bytes left to upload */
  curl_off_t rx_کل;          /* total number of bytes received */
  uint64_t rx_انحراف;           /* current receive offset */
  uint64_t rx_انحراف_max;       /* allowed receive offset */
  size_t tx_اندر_flight_سائز;     /* sendbuf data "in flight" */
  size_t tx_اندر_flight_ideal;    /* ideal amount of un-acked send data */
  uint32_t mid;                 /* `mid` of transfer owning this stream */
  int32_t حالت_رمز;              /* HTTP status code */
  CURLcode xfer_نتیجہ;         /* result from xfer_resp_write(_hd) */
  ثنائی_ہندسہ(resp_hds_complete);       /* we have a complete, final response */
  ثنائی_ہندسہ(closed);                  /* TRUE on stream close */
  ثنائی_ہندسہ(reset);                   /* TRUE on stream reset */
  ثنائی_ہندسہ(send_closed);             /* stream is local closed */
  ثنائی_ہندسہ(quic_flow_blocked);       /* stream is blocked by QUIC flow control */
};

خالی Curl_cf_ngtcp2_h3_دھارا_ctx_free(ساخت h3_دھارا_ctx *دھارا);
خالی Curl_cf_ngtcp2_h3_err_سیٹ(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا, صحیح رمز);

CURLcode Curl_cf_ngtcp2_h3_init_ctrls(ساخت cf_ngtcp2_ctx *ctx,
                                      ساخت Curl_easy *ڈیٹا);

CURLcode Curl_cf_ngtcp2_cmn_connect(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا,
                                    bool *مکمل);

CURLcode Curl_cf_ngtcp2_cmn_shutdown(ساخت Curl_cfilter *cf,
                                     ساخت Curl_easy *ڈیٹا, bool *مکمل);
خالی Curl_cf_ngtcp2_cmn_conn_close(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا);

ساخت cf_ngtcp2_io_ctx {
  ساخت Curl_cfilter *cf;
  ساخت Curl_easy *ڈیٹا;
  ساخت curltime now;
  ngtcp2_tstamp ts;
  ngtcp2_راستہ_storage ps;
};

خالی Curl_cf_ngtcp2_io_ctx_init(ساخت cf_ngtcp2_io_ctx *io_ctx,
                                ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا);
خالی Curl_cf_ngtcp2_io_ctx_update_وقت(ساخت Curl_easy *ڈیٹا,
                                       ساخت cf_ngtcp2_io_ctx *io_ctx,
                                       ساخت Curl_cfilter *cf);

CURLcode Curl_cf_ngtcp2_پیش_رفت_egress(ساخت Curl_cfilter *cf,
                                        ساخت Curl_easy *ڈیٹا,
                                        ساخت cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_پیش_رفت_ingress(ساخت Curl_cfilter *cf,
                                         ساخت Curl_easy *ڈیٹا,
                                         ساخت cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_cmn_سیٹ_expiry(ساخت Curl_cfilter *cf,
                                       ساخت Curl_easy *ڈیٹا,
                                       ساخت cf_ngtcp2_io_ctx *pktx);

CURLcode Curl_cf_ngtcp2_h3_دھارا_تیاری(ساخت Curl_cfilter *cf,
                                        ساخت Curl_easy *ڈیٹا,
                                        ساخت h3_دھارا_ctx **pstream);
خالی Curl_cf_ngtcp2_h3_دھارا_close(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا,
                                    ساخت h3_دھارا_ctx *دھارا);
خالی Curl_cf_ngtcp2_h3_دھارا_مکمل(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا);

bool Curl_cf_ngtcp2_cmn_conn_is_alive(ساخت Curl_cfilter *cf,
                                      ساخت Curl_easy *ڈیٹا,
                                      bool *داخلہ_pending);

CURLcode Curl_cf_ngtcp2_cmn_query(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  صحیح query, صحیح *pres1, خالی *pres2);

CURLcode Curl_cf_ngtcp2_cmn_cntrl(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  صحیح event, صحیح arg1, خالی *arg2);

ساخت Curl_easy *Curl_cf_ngtcp2_get_xfer(ساخت Curl_cfilter *cf,
                                          خالی *دھارا_user_ڈیٹا,
                                          ساخت h3_دھارا_ctx **pstream);

#اگر_ختم /* !CURL_DISABLE_HTTP && USE_NGTCP2 && USE_NGHTTP3 */

#اگر_ختم /* HEADER_CURL_VQUIC_CF_NGTCP2_CMN_H */
