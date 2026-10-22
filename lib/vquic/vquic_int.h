#यदि_अपरिभाषितम् शीर्षकम्_CURL_VQUIC_QUIC_INT_H
#परिभाषय शीर्षकम्_CURL_VQUIC_QUIC_INT_H
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

#यदि_परिभाषितम् USE_HTTP3

#अन्तर्भाव "bufq.h"
#अन्तर्भाव "u32_ptrset.h"

#परिभाषय MAX_UDP_PAYLOAD_आकार  1452

/* definitions from RFC 9114, ch 8.1 */
प्रकारनिर्देश गणना {
  CURL_H3_ERR_NO_ERROR = 0x0100,
  CURL_H3_ERR_GENERAL_PROTOCOL_ERROR = 0x0101,
  CURL_H3_ERR_आन्तरिकम्_ERROR = 0x0102,
  CURL_H3_ERR_प्रवाहः_CREATION_ERROR = 0x0103,
  CURL_H3_ERR_CLOSED_CRITICAL_प्रवाहः = 0x0104,
  CURL_H3_ERR_FRAME_UNEXPECTED = 0x0105,
  CURL_H3_ERR_FRAME_ERROR = 0x0106,
  CURL_H3_ERR_EXCESSIVE_आरोपय = 0x0107,
  CURL_H3_ERR_ID_ERROR = 0x0108,
  CURL_H3_ERR_SETTINGS_ERROR = 0x0109,
  CURL_H3_ERR_MISSING_SETTINGS = 0x010a,
  CURL_H3_ERR_REQUEST_REJECTED = 0x010b,
  CURL_H3_ERR_REQUEST_CANCELLED = 0x010c,
  CURL_H3_ERR_REQUEST_INCOMPLETE = 0x010d,
  CURL_H3_ERR_सन्देशः_ERROR = 0x010e,
  CURL_H3_ERR_CONNECT_ERROR = 0x010f,
  CURL_H3_ERR_संस्करणम्_FALLBACK = 0x0110,
} vquic_h3_error;

#यदि_परिभाषितम् CURLVERBOSE
ध्रुव वर्ण *Curl_vquic_h3_err_str(uint64_t error_संहिता);
#अन्यथा
#परिभाषय Curl_vquic_h3_err_str(x)   ""
#यदि_अन्तः /* CURLVERBOSE */

संरचना cf_quic_ctx {
  curl_socket_t sockfd;               /* connected UDP socket */
  संरचना sockaddr_storage local_addr; /* address socket is bound to */
  socklen_t local_addrlen;            /* length of local address */

  संरचना bufq sendbuf;           /* buffer for sending one or more packets */
  संरचना curltime first_byte_at; /* when first byte was recvd */
  संरचना curltime चरमः_op;       /* last (attempted) send/recv operation */
  संरचना curltime चरमः_io;       /* last successful socket IO */
  size_t gsolen;                 /* length of individual packets in send buf */
  size_t विभज_len;    /* if != 0, buffer length after which GSO differs */
  size_t विभज_gsolen; /* length of individual packets after split_len */
#यदि_परिभाषितम् DEBUGBUILD
  पूर्णाङ्क wblock_percent;  /* percent of writes doing EAGAIN */
#यदि_अन्तः
  BIT(got_first_byte); /* if first byte was received */
  BIT(no_gso);         /* do not use gso on sending */
};

#परिभाषय H3_प्रवाहः_CTX(ctx, दत्त)                                        \
  ((दत्त) ? Curl_u32_ptrset_get(&(ctx)->streams, (दत्त)->mid) : रिक्त)

CURLcode Curl_vquic_ctx_init(संरचना Curl_easy *दत्त,
                             संरचना cf_quic_ctx *qctx);
शून्य Curl_vquic_ctx_free(संरचना cf_quic_ctx *qctx);

शून्य Curl_vquic_ctx_समुच्चयः_time(संरचना cf_quic_ctx *qctx,
                             ध्रुव संरचना curltime *pnow);

शून्य Curl_vquic_ctx_update_time(संरचना cf_quic_ctx *qctx,
                                ध्रुव संरचना curltime *pnow);

CURLcode Curl_vquic_send(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                         संरचना cf_quic_ctx *qctx, size_t gsolen);

CURLcode Curl_vquic_send_पुच्छ_विभज(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *दत्त,
                                    संरचना cf_quic_ctx *qctx, size_t gsolen,
                                    size_t पुच्छ_len, size_t पुच्छ_gsolen);

CURLcode Curl_vquic_flush(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                          संरचना cf_quic_ctx *qctx);

प्रकारनिर्देश CURLcode Curl_vquic_recv_pkts_cb(ध्रुव अचिह्नित वर्ण *buf,
                                         size_t buflen,
                                         size_t gso_आकार,
                                         संरचना sockaddr_storage *remote_addr,
                                         socklen_t remote_addrlen, uint8_t ecn,
                                         शून्य *userp);

CURLcode Curl_vquic_recv_packets(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 संरचना cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb,
                                 शून्य *userp);

#यदि_परिभाषितम् USE_NGTCP2
संरचना ngtcp2_mem;
संरचना ngtcp2_mem *Curl_ngtcp2_mem(शून्य);
#यदि_अन्तः
#यदि_परिभाषितम् USE_NGHTTP3
संरचना nghttp3_mem;
संरचना nghttp3_mem *Curl_nghttp3_mem(शून्य);
#यदि_अन्तः

#यदि_अन्तः /* !USE_HTTP3 */

#यदि_अन्तः /* HEADER_CURL_VQUIC_QUIC_INT_H */
