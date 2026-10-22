#अगर_अघोषित HEADER_CURL_VQUIC_QUIC_INT_H
#घोषणा HEADER_CURL_VQUIC_QUIC_INT_H
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

#अगर_घोषित USE_HTTP3

#समावेश "bufq.h"
#समावेश "u32_ptrset.h"

#घोषणा MAX_UDP_PAYLOAD_आकार  1452

/* definitions from RFC 9114, ch 8.1 */
प्रकार क्रमागत {
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

#अगर_घोषित CURLVERBOSE
स्थिर अक्षर *Curl_vquic_h3_err_str(uint64_t error_code);
#अन्यथा
#घोषणा Curl_vquic_h3_err_str(x)   ""
#पूर्ण_अगर /* CURLVERBOSE */

काष्ठा cf_quic_ctx {
  curl_socket_t sockfd;               /* connected UDP socket */
  काष्ठा sockaddr_storage local_addr; /* address socket is bound to */
  socklen_t local_addrlen;            /* length of local address */

  काष्ठा bufq sendbuf;           /* buffer for sending one or more packets */
  काष्ठा curltime first_byte_at; /* when first byte was recvd */
  काष्ठा curltime last_op;       /* last (attempted) send/recv operation */
  काष्ठा curltime last_io;       /* last successful socket IO */
  माप_प्रकार gsolen;                 /* length of individual packets in send buf */
  माप_प्रकार split_len;    /* if != 0, buffer length after which GSO differs */
  माप_प्रकार split_gsolen; /* length of individual packets after split_len */
#अगर_घोषित DEBUGBUILD
  पूर्णांक wblock_percent;  /* percent of writes doing EAGAIN */
#पूर्ण_अगर
  BIT(got_first_byte); /* if first byte was received */
  BIT(no_gso);         /* do not use gso on sending */
};

#घोषणा H3_STREAM_CTX(ctx, तथ्य)                                        \
  ((तथ्य) ? Curl_u32_ptrset_get(&(ctx)->streams, (तथ्य)->mid) : शून्य)

CURLcode Curl_vquic_ctx_init(काष्ठा Curl_easy *तथ्य,
                             काष्ठा cf_quic_ctx *qctx);
व्योम Curl_vquic_ctx_free(काष्ठा cf_quic_ctx *qctx);

व्योम Curl_vquic_ctx_set_time(काष्ठा cf_quic_ctx *qctx,
                             स्थिर काष्ठा curltime *pnow);

व्योम Curl_vquic_ctx_update_time(काष्ठा cf_quic_ctx *qctx,
                                स्थिर काष्ठा curltime *pnow);

CURLcode Curl_vquic_send(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                         काष्ठा cf_quic_ctx *qctx, माप_प्रकार gsolen);

CURLcode Curl_vquic_send_पुच्छ_split(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य,
                                    काष्ठा cf_quic_ctx *qctx, माप_प्रकार gsolen,
                                    माप_प्रकार पुच्छ_len, माप_प्रकार पुच्छ_gsolen);

CURLcode Curl_vquic_flush(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                          काष्ठा cf_quic_ctx *qctx);

प्रकार CURLcode Curl_vquic_recv_pkts_cb(स्थिर अचिन्हित अक्षर *buf,
                                         माप_प्रकार buflen,
                                         माप_प्रकार gso_आकार,
                                         काष्ठा sockaddr_storage *remote_addr,
                                         socklen_t remote_addrlen, uint8_t ecn,
                                         व्योम *userp);

CURLcode Curl_vquic_recv_packets(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 काष्ठा cf_quic_ctx *qctx,
                                 माप_प्रकार max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb,
                                 व्योम *userp);

#अगर_घोषित USE_NGTCP2
काष्ठा ngtcp2_mem;
काष्ठा ngtcp2_mem *Curl_ngtcp2_mem(व्योम);
#पूर्ण_अगर
#अगर_घोषित USE_NGHTTP3
काष्ठा nghttp3_mem;
काष्ठा nghttp3_mem *Curl_nghttp3_mem(व्योम);
#पूर्ण_अगर

#पूर्ण_अगर /* !USE_HTTP3 */

#पूर्ण_अगर /* HEADER_CURL_VQUIC_QUIC_INT_H */
