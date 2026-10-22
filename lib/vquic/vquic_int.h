#که_نه_تعریف_شوی سرلیک_CURL_VQUIC_QUIC_INT_H
#تعریف_کړه سرلیک_CURL_VQUIC_QUIC_INT_H
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

#که_تعریف_شوی USE_HTTP3

#ضمن "bufq.h"
#ضمن "u32_ptrset.h"

#تعریف_کړه MAX_UDP_PAYLOAD_اندازه  1452

/* definitions from RFC 9114, ch 8.1 */
تعريفنوع تعداد {
  CURL_H3_ERR_NO_ERROR = 0x0100,
  CURL_H3_ERR_GENERAL_PROTOCOL_ERROR = 0x0101,
  CURL_H3_ERR_دننی_ERROR = 0x0102,
  CURL_H3_ERR_بهیر_CREATION_ERROR = 0x0103,
  CURL_H3_ERR_CLOSED_CRITICAL_بهیر = 0x0104,
  CURL_H3_ERR_چوکاټ_UNEXPECTED = 0x0105,
  CURL_H3_ERR_چوکاټ_ERROR = 0x0106,
  CURL_H3_ERR_EXCESSIVE_بارول = 0x0107,
  CURL_H3_ERR_ID_ERROR = 0x0108,
  CURL_H3_ERR_SETTINGS_ERROR = 0x0109,
  CURL_H3_ERR_MISSING_SETTINGS = 0x010a,
  CURL_H3_ERR_REQUEST_REJECTED = 0x010b,
  CURL_H3_ERR_REQUEST_CANCELLED = 0x010c,
  CURL_H3_ERR_REQUEST_INCOMPLETE = 0x010d,
  CURL_H3_ERR_پیغام_ERROR = 0x010e,
  CURL_H3_ERR_CONNECT_ERROR = 0x010f,
  CURL_H3_ERR_نسخه_FALLBACK = 0x0110,
} vquic_h3_error;

#که_تعریف_شوی CURLVERBOSE
ثابت حرف *Curl_vquic_h3_err_str(uint64_t error_کوډ);
#وإلا
#تعریف_کړه Curl_vquic_h3_err_str(x)   ""
#د_که_پای /* CURLVERBOSE */

بنية cf_quic_ctx {
  curl_socket_t sockfd;               /* connected UDP socket */
  بنية sockaddr_storage local_addr; /* address socket is bound to */
  socklen_t local_addrlen;            /* length of local address */

  بنية bufq sendbuf;           /* buffer for sending one or more packets */
  بنية curltime لومړی_byte_at; /* when first byte was recvd */
  بنية curltime وروستی_op;       /* last (attempted) send/recv operation */
  بنية curltime وروستی_io;       /* last successful socket IO */
  size_t gsolen;                 /* length of individual packets in send buf */
  size_t وېشل_len;    /* if != 0, buffer length after which GSO differs */
  size_t وېشل_gsolen; /* length of individual packets after split_len */
#که_تعریف_شوی DEBUGBUILD
  صحيح wblock_percent;  /* percent of writes doing EAGAIN */
#د_که_پای
  بټ(got_لومړی_byte); /* if first byte was received */
  بټ(no_gso);         /* do not use gso on sending */
};

#تعریف_کړه H3_بهیر_CTX(ctx, معلومات)                                        \
  ((معلومات) ? Curl_u32_ptrset_get(&(ctx)->streams, (معلومات)->mid) : عدم)

CURLcode Curl_vquic_ctx_init(بنية Curl_easy *معلومات,
                             بنية cf_quic_ctx *qctx);
فراغ Curl_vquic_ctx_free(بنية cf_quic_ctx *qctx);

فراغ Curl_vquic_ctx_سېټ_وخت(بنية cf_quic_ctx *qctx,
                             ثابت بنية curltime *pnow);

فراغ Curl_vquic_ctx_update_وخت(بنية cf_quic_ctx *qctx,
                                ثابت بنية curltime *pnow);

CURLcode Curl_vquic_send(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                         بنية cf_quic_ctx *qctx, size_t gsolen);

CURLcode Curl_vquic_send_لکۍ_وېشل(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    بنية cf_quic_ctx *qctx, size_t gsolen,
                                    size_t لکۍ_len, size_t لکۍ_gsolen);

CURLcode Curl_vquic_flush(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                          بنية cf_quic_ctx *qctx);

تعريفنوع CURLcode Curl_vquic_recv_pkts_cb(ثابت بینښه حرف *buf,
                                         size_t buflen,
                                         size_t gso_اندازه,
                                         بنية sockaddr_storage *remote_addr,
                                         socklen_t remote_addrlen, uint8_t ecn,
                                         فراغ *userp);

CURLcode Curl_vquic_recv_packets(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 بنية cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb,
                                 فراغ *userp);

#که_تعریف_شوی USE_NGTCP2
بنية ngtcp2_mem;
بنية ngtcp2_mem *Curl_ngtcp2_mem(فراغ);
#د_که_پای
#که_تعریف_شوی USE_NGHTTP3
بنية nghttp3_mem;
بنية nghttp3_mem *Curl_nghttp3_mem(فراغ);
#د_که_پای

#د_که_پای /* !USE_HTTP3 */

#د_که_پای /* HEADER_CURL_VQUIC_QUIC_INT_H */
