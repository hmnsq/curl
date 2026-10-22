#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_VQUIC_QUIC_INT_H
#وصف_ڪريو سرو_CURL_VQUIC_QUIC_INT_H
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

#جيڪڏهن_وصف_ٿيل USE_HTTP3

#ضمن "bufq.h"
#ضمن "u32_ptrset.h"

#وصف_ڪريو MAX_UDP_PAYLOAD_سائيز  1452

/* definitions from RFC 9114, ch 8.1 */
تعريفنوع تعداد {
  CURL_H3_ERR_NO_ERROR = 0x0100,
  CURL_H3_ERR_GENERAL_PROTOCOL_ERROR = 0x0101,
  CURL_H3_ERR_اندروني_ERROR = 0x0102,
  CURL_H3_ERR_وهڪرو_CREATION_ERROR = 0x0103,
  CURL_H3_ERR_CLOSED_CRITICAL_وهڪرو = 0x0104,
  CURL_H3_ERR_چوکٺو_UNEXPECTED = 0x0105,
  CURL_H3_ERR_چوکٺو_ERROR = 0x0106,
  CURL_H3_ERR_EXCESSIVE_ڀريو = 0x0107,
  CURL_H3_ERR_ID_ERROR = 0x0108,
  CURL_H3_ERR_SETTINGS_ERROR = 0x0109,
  CURL_H3_ERR_MISSING_SETTINGS = 0x010a,
  CURL_H3_ERR_REQUEST_REJECTED = 0x010b,
  CURL_H3_ERR_REQUEST_CANCELLED = 0x010c,
  CURL_H3_ERR_REQUEST_INCOMPLETE = 0x010d,
  CURL_H3_ERR_پيغام_ERROR = 0x010e,
  CURL_H3_ERR_CONNECT_ERROR = 0x010f,
  CURL_H3_ERR_نسخو_FALLBACK = 0x0110,
} vquic_h3_error;

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
ثابت حرف *Curl_vquic_h3_err_str(uint64_t error_ڪوڊ);
#وإلا
#وصف_ڪريو Curl_vquic_h3_err_str(x)   ""
#جيڪڏهن_پورو /* CURLVERBOSE */

بنية cf_quic_ctx {
  curl_socket_t sockfd;               /* connected UDP socket */
  بنية sockaddr_storage local_addr; /* address socket is bound to */
  socklen_t local_addrlen;            /* length of local address */

  بنية bufq sendbuf;           /* buffer for sending one or more packets */
  بنية curltime پهريون_byte_at; /* when first byte was recvd */
  بنية curltime آخري_op;       /* last (attempted) send/recv operation */
  بنية curltime آخري_io;       /* last successful socket IO */
  size_t gsolen;                 /* length of individual packets in send buf */
  size_t ورهايو_len;    /* if != 0, buffer length after which GSO differs */
  size_t ورهايو_gsolen; /* length of individual packets after split_len */
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  صحيح wblock_percent;  /* percent of writes doing EAGAIN */
#جيڪڏهن_پورو
  ٻٽو_انگ(got_پهريون_byte); /* if first byte was received */
  ٻٽو_انگ(no_gso);         /* do not use gso on sending */
};

#وصف_ڪريو H3_وهڪرو_CTX(ctx, معلومات)                                        \
  ((معلومات) ? Curl_u32_ptrset_get(&(ctx)->streams, (معلومات)->mid) : عدم)

CURLcode Curl_vquic_ctx_init(بنية Curl_easy *معلومات,
                             بنية cf_quic_ctx *qctx);
فراغ Curl_vquic_ctx_free(بنية cf_quic_ctx *qctx);

فراغ Curl_vquic_ctx_سيٽ_وقت(بنية cf_quic_ctx *qctx,
                             ثابت بنية curltime *pnow);

فراغ Curl_vquic_ctx_update_وقت(بنية cf_quic_ctx *qctx,
                                ثابت بنية curltime *pnow);

CURLcode Curl_vquic_send(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                         بنية cf_quic_ctx *qctx, size_t gsolen);

CURLcode Curl_vquic_send_پڇ_ورهايو(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    بنية cf_quic_ctx *qctx, size_t gsolen,
                                    size_t پڇ_len, size_t پڇ_gsolen);

CURLcode Curl_vquic_flush(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                          بنية cf_quic_ctx *qctx);

تعريفنوع CURLcode Curl_vquic_recv_pkts_cb(ثابت بنانشان حرف *buf,
                                         size_t buflen,
                                         size_t gso_سائيز,
                                         بنية sockaddr_storage *remote_addr,
                                         socklen_t remote_addrlen, uint8_t ecn,
                                         فراغ *userp);

CURLcode Curl_vquic_recv_packets(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 بنية cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb,
                                 فراغ *userp);

#جيڪڏهن_وصف_ٿيل USE_NGTCP2
بنية ngtcp2_mem;
بنية ngtcp2_mem *Curl_ngtcp2_mem(فراغ);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_NGHTTP3
بنية nghttp3_mem;
بنية nghttp3_mem *Curl_nghttp3_mem(فراغ);
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* !USE_HTTP3 */

#جيڪڏهن_پورو /* HEADER_CURL_VQUIC_QUIC_INT_H */
