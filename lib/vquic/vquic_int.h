#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_VQUIC_QUIC_INT_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_VQUIC_QUIC_INT_H
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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTP3

#ਅਨ੍ਤਰ੍ਭਾਵ "bufq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "u32_ptrset.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_UDP_PAYLOAD_ਮਾਪ  1452

/* definitions from RFC 9114, ch 8.1 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  CURL_H3_ERR_NO_ERROR = 0x0100,
  CURL_H3_ERR_GENERAL_PROTOCOL_ERROR = 0x0101,
  CURL_H3_ERR_ਅੰਦਰੂਨੀ_ERROR = 0x0102,
  CURL_H3_ERR_ਧਾਰਾ_CREATION_ERROR = 0x0103,
  CURL_H3_ERR_CLOSED_CRITICAL_ਧਾਰਾ = 0x0104,
  CURL_H3_ERR_ਚੌਖਟਾ_UNEXPECTED = 0x0105,
  CURL_H3_ERR_ਚੌਖਟਾ_ERROR = 0x0106,
  CURL_H3_ERR_EXCESSIVE_ਲੱਦੋ = 0x0107,
  CURL_H3_ERR_ID_ERROR = 0x0108,
  CURL_H3_ERR_SETTINGS_ERROR = 0x0109,
  CURL_H3_ERR_MISSING_SETTINGS = 0x010a,
  CURL_H3_ERR_REQUEST_REJECTED = 0x010b,
  CURL_H3_ERR_REQUEST_CANCELLED = 0x010c,
  CURL_H3_ERR_REQUEST_INCOMPLETE = 0x010d,
  CURL_H3_ERR_ਸੁਨੇਹਾ_ERROR = 0x010e,
  CURL_H3_ERR_CONNECT_ERROR = 0x010f,
  CURL_H3_ERR_ਸੰਸਕਰਣ_FALLBACK = 0x0110,
} vquic_h3_error;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਅਚਲ ਅੱਖਰ *Curl_vquic_h3_err_str(uint64_t error_ਕੋਡ);
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_vquic_h3_err_str(x)   ""
#ਜੇ_ਅੰਤ /* CURLVERBOSE */

ਬਣਤਰ cf_quic_ctx {
  curl_socket_t sockfd;               /* connected UDP socket */
  ਬਣਤਰ sockaddr_storage local_addr; /* address socket is bound to */
  socklen_t local_addrlen;            /* length of local address */

  ਬਣਤਰ bufq sendbuf;           /* buffer for sending one or more packets */
  ਬਣਤਰ curltime ਪਹਿਲਾ_byte_at; /* when first byte was recvd */
  ਬਣਤਰ curltime ਆਖਰੀ_op;       /* last (attempted) send/recv operation */
  ਬਣਤਰ curltime ਆਖਰੀ_io;       /* last successful socket IO */
  size_t gsolen;                 /* length of individual packets in send buf */
  size_t ਵੰਡੋ_len;    /* if != 0, buffer length after which GSO differs */
  size_t ਵੰਡੋ_gsolen; /* length of individual packets after split_len */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਪੂਰਨ wblock_percent;  /* percent of writes doing EAGAIN */
#ਜੇ_ਅੰਤ
  ਦੋਆਧਾਰੀ_ਅੰਕ(got_ਪਹਿਲਾ_byte); /* if first byte was received */
  ਦੋਆਧਾਰੀ_ਅੰਕ(no_gso);         /* do not use gso on sending */
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H3_ਧਾਰਾ_CTX(ctx, ਅੰਕੜੇ)                                        \
  ((ਅੰਕੜੇ) ? Curl_u32_ptrset_get(&(ctx)->streams, (ਅੰਕੜੇ)->mid) : ਨਿਰਾ)

CURLcode Curl_vquic_ctx_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ cf_quic_ctx *qctx);
ਖਾਲੀ Curl_vquic_ctx_free(ਬਣਤਰ cf_quic_ctx *qctx);

ਖਾਲੀ Curl_vquic_ctx_ਸੈੱਟ_ਸਮਾਂ(ਬਣਤਰ cf_quic_ctx *qctx,
                             ਅਚਲ ਬਣਤਰ curltime *pnow);

ਖਾਲੀ Curl_vquic_ctx_update_ਸਮਾਂ(ਬਣਤਰ cf_quic_ctx *qctx,
                                ਅਚਲ ਬਣਤਰ curltime *pnow);

CURLcode Curl_vquic_send(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         ਬਣਤਰ cf_quic_ctx *qctx, size_t gsolen);

CURLcode Curl_vquic_send_ਪੂਛ_ਵੰਡੋ(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ cf_quic_ctx *qctx, size_t gsolen,
                                    size_t ਪੂਛ_len, size_t ਪੂਛ_gsolen);

CURLcode Curl_vquic_flush(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਬਣਤਰ cf_quic_ctx *qctx);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ CURLcode Curl_vquic_recv_pkts_cb(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf,
                                         size_t buflen,
                                         size_t gso_ਮਾਪ,
                                         ਬਣਤਰ sockaddr_storage *remote_addr,
                                         socklen_t remote_addrlen, uint8_t ecn,
                                         ਖਾਲੀ *userp);

CURLcode Curl_vquic_recv_packets(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb,
                                 ਖਾਲੀ *userp);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NGTCP2
ਬਣਤਰ ngtcp2_mem;
ਬਣਤਰ ngtcp2_mem *Curl_ngtcp2_mem(ਖਾਲੀ);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NGHTTP3
ਬਣਤਰ nghttp3_mem;
ਬਣਤਰ nghttp3_mem *Curl_nghttp3_mem(ਖਾਲੀ);
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* !USE_HTTP3 */

#ਜੇ_ਅੰਤ /* HEADER_CURL_VQUIC_QUIC_INT_H */
