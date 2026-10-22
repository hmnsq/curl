#اگر_غیر_تعریف_شدہ سرخی_CURL_VQUIC_QUIC_INT_H
#تعریف_کرو سرخی_CURL_VQUIC_QUIC_INT_H
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

#اگر_تعریف_شدہ USE_HTTP3

#شامل "bufq.h"
#شامل "u32_ptrset.h"

#تعریف_کرو MAX_UDP_PAYLOAD_سائز  1452

/* definitions from RFC 9114, ch 8.1 */
قسمتعریف شمار {
  CURL_H3_ERR_NO_ERROR = 0x0100,
  CURL_H3_ERR_GENERAL_PROTOCOL_ERROR = 0x0101,
  CURL_H3_ERR_اندرونی_ERROR = 0x0102,
  CURL_H3_ERR_دھارا_CREATION_ERROR = 0x0103,
  CURL_H3_ERR_CLOSED_CRITICAL_دھارا = 0x0104,
  CURL_H3_ERR_چوکھٹا_UNEXPECTED = 0x0105,
  CURL_H3_ERR_چوکھٹا_ERROR = 0x0106,
  CURL_H3_ERR_EXCESSIVE_بھرو = 0x0107,
  CURL_H3_ERR_ID_ERROR = 0x0108,
  CURL_H3_ERR_SETTINGS_ERROR = 0x0109,
  CURL_H3_ERR_MISSING_SETTINGS = 0x010a,
  CURL_H3_ERR_REQUEST_REJECTED = 0x010b,
  CURL_H3_ERR_REQUEST_CANCELLED = 0x010c,
  CURL_H3_ERR_REQUEST_INCOMPLETE = 0x010d,
  CURL_H3_ERR_پیغام_ERROR = 0x010e,
  CURL_H3_ERR_CONNECT_ERROR = 0x010f,
  CURL_H3_ERR_نسخہ_FALLBACK = 0x0110,
} vquic_h3_error;

#اگر_تعریف_شدہ CURLVERBOSE
ثابت حرف *Curl_vquic_h3_err_str(uint64_t error_رمز);
#ورنہ
#تعریف_کرو Curl_vquic_h3_err_str(x)   ""
#اگر_ختم /* CURLVERBOSE */

ساخت cf_quic_ctx {
  curl_socket_t sockfd;               /* connected UDP socket */
  ساخت sockaddr_storage local_addr; /* address socket is bound to */
  socklen_t local_addrlen;            /* length of local address */

  ساخت bufq sendbuf;           /* buffer for sending one or more packets */
  ساخت curltime پہلا_byte_at; /* when first byte was recvd */
  ساخت curltime آخری_op;       /* last (attempted) send/recv operation */
  ساخت curltime آخری_io;       /* last successful socket IO */
  size_t gsolen;                 /* length of individual packets in send buf */
  size_t بانٹو_len;    /* if != 0, buffer length after which GSO differs */
  size_t بانٹو_gsolen; /* length of individual packets after split_len */
#اگر_تعریف_شدہ DEBUGBUILD
  صحیح wblock_percent;  /* percent of writes doing EAGAIN */
#اگر_ختم
  ثنائی_ہندسہ(got_پہلا_byte); /* if first byte was received */
  ثنائی_ہندسہ(no_gso);         /* do not use gso on sending */
};

#تعریف_کرو H3_دھارا_CTX(ctx, ڈیٹا)                                        \
  ((ڈیٹا) ? Curl_u32_ptrset_get(&(ctx)->streams, (ڈیٹا)->mid) : تہی)

CURLcode Curl_vquic_ctx_init(ساخت Curl_easy *ڈیٹا,
                             ساخت cf_quic_ctx *qctx);
خالی Curl_vquic_ctx_free(ساخت cf_quic_ctx *qctx);

خالی Curl_vquic_ctx_سیٹ_وقت(ساخت cf_quic_ctx *qctx,
                             ثابت ساخت curltime *pnow);

خالی Curl_vquic_ctx_update_وقت(ساخت cf_quic_ctx *qctx,
                                ثابت ساخت curltime *pnow);

CURLcode Curl_vquic_send(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                         ساخت cf_quic_ctx *qctx, size_t gsolen);

CURLcode Curl_vquic_send_دم_بانٹو(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا,
                                    ساخت cf_quic_ctx *qctx, size_t gsolen,
                                    size_t دم_len, size_t دم_gsolen);

CURLcode Curl_vquic_flush(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                          ساخت cf_quic_ctx *qctx);

قسمتعریف CURLcode Curl_vquic_recv_pkts_cb(ثابت غیرعلامتی حرف *buf,
                                         size_t buflen,
                                         size_t gso_سائز,
                                         ساخت sockaddr_storage *remote_addr,
                                         socklen_t remote_addrlen, uint8_t ecn,
                                         خالی *userp);

CURLcode Curl_vquic_recv_packets(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 ساخت cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb,
                                 خالی *userp);

#اگر_تعریف_شدہ USE_NGTCP2
ساخت ngtcp2_mem;
ساخت ngtcp2_mem *Curl_ngtcp2_mem(خالی);
#اگر_ختم
#اگر_تعریف_شدہ USE_NGHTTP3
ساخت nghttp3_mem;
ساخت nghttp3_mem *Curl_nghttp3_mem(خالی);
#اگر_ختم

#اگر_ختم /* !USE_HTTP3 */

#اگر_ختم /* HEADER_CURL_VQUIC_QUIC_INT_H */
