#ifndef HEADER_CURL_VQUIC_QUIC_INT_H
#define HEADER_CURL_VQUIC_QUIC_INT_H
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

#ifdef USE_HTTP3

#شامل "bufq.h"
#شامل "u32_ptrset.h"

#define MAX_UDP_PAYLOAD_SIZE  1452

/* definitions from RFC 9114, ch 8.1 */
تعریفنوع شمارش {
  CURL_H3_ERR_NO_ERROR = 0x0100,
  CURL_H3_ERR_GENERAL_PROTOCOL_ERROR = 0x0101,
  CURL_H3_ERR_INTERNAL_ERROR = 0x0102,
  CURL_H3_ERR_STREAM_CREATION_ERROR = 0x0103,
  CURL_H3_ERR_CLOSED_CRITICAL_STREAM = 0x0104,
  CURL_H3_ERR_FRAME_UNEXPECTED = 0x0105,
  CURL_H3_ERR_FRAME_ERROR = 0x0106,
  CURL_H3_ERR_EXCESSIVE_LOAD = 0x0107,
  CURL_H3_ERR_ID_ERROR = 0x0108,
  CURL_H3_ERR_SETTINGS_ERROR = 0x0109,
  CURL_H3_ERR_MISSING_SETTINGS = 0x010a,
  CURL_H3_ERR_REQUEST_REJECTED = 0x010b,
  CURL_H3_ERR_REQUEST_CANCELLED = 0x010c,
  CURL_H3_ERR_REQUEST_INCOMPLETE = 0x010d,
  CURL_H3_ERR_MESSAGE_ERROR = 0x010e,
  CURL_H3_ERR_CONNECT_ERROR = 0x010f,
  CURL_H3_ERR_VERSION_FALLBACK = 0x0110,
} vquic_h3_error;

#ifdef CURLVERBOSE
ثابت نویسه *Curl_vquic_h3_err_str(uint64_t error_code);
#وگرنه
#define Curl_vquic_h3_err_str(x)   ""
#endif /* CURLVERBOSE */

ساختار cf_quic_ctx {
  curl_socket_t sockfd;               /* connected UDP socket */
  ساختار sockaddr_storage local_addr; /* address socket is bound to */
  socklen_t local_addrlen;            /* length of local address */

  ساختار bufq sendbuf;           /* buffer for sending one or more packets */
  ساختار curltime first_byte_at; /* when first byte was recvd */
  ساختار curltime last_op;       /* last (attempted) send/recv operation */
  ساختار curltime last_io;       /* last successful socket IO */
  size_t gsolen;                 /* length of individual packets in send buf */
  size_t split_len;    /* if != 0, buffer length after which GSO differs */
  size_t split_gsolen; /* length of individual packets after split_len */
#ifdef DEBUGBUILD
  صحیح wblock_percent;  /* percent of writes doing EAGAIN */
#endif
  BIT(got_first_byte); /* if first byte was received */
  BIT(no_gso);         /* do not use gso on sending */
};

#define H3_STREAM_CTX(ctx, data)                                        \
  ((data) ? Curl_u32_ptrset_get(&(ctx)->streams, (data)->mid) : پوچ)

CURLcode Curl_vquic_ctx_init(ساختار Curl_easy *data,
                             ساختار cf_quic_ctx *qctx);
تهی Curl_vquic_ctx_free(ساختار cf_quic_ctx *qctx);

تهی Curl_vquic_ctx_set_time(ساختار cf_quic_ctx *qctx,
                             ثابت ساختار curltime *pnow);

تهی Curl_vquic_ctx_update_time(ساختار cf_quic_ctx *qctx,
                                ثابت ساختار curltime *pnow);

CURLcode Curl_vquic_send(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                         ساختار cf_quic_ctx *qctx, size_t gsolen);

CURLcode Curl_vquic_send_tail_split(ساختار Curl_cfilter *cf,
                                    ساختار Curl_easy *data,
                                    ساختار cf_quic_ctx *qctx, size_t gsolen,
                                    size_t tail_len, size_t tail_gsolen);

CURLcode Curl_vquic_flush(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                          ساختار cf_quic_ctx *qctx);

تعریفنوع CURLcode Curl_vquic_recv_pkts_cb(ثابت unsigned نویسه *buf,
                                         size_t buflen,
                                         size_t gso_size,
                                         ساختار sockaddr_storage *remote_addr,
                                         socklen_t remote_addrlen, uint8_t ecn,
                                         تهی *userp);

CURLcode Curl_vquic_recv_packets(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 ساختار cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb,
                                 تهی *userp);

#ifdef USE_NGTCP2
ساختار ngtcp2_mem;
ساختار ngtcp2_mem *Curl_ngtcp2_mem(تهی);
#endif
#ifdef USE_NGHTTP3
ساختار nghttp3_mem;
ساختار nghttp3_mem *Curl_nghttp3_mem(تهی);
#endif

#endif /* !USE_HTTP3 */

#endif /* HEADER_CURL_VQUIC_QUIC_INT_H */
