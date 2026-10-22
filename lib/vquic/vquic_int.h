#నిర్వచితం_కాకపోతే శీర్షిక_CURL_VQUIC_QUIC_INT_H
#నిర్వచించు శీర్షిక_CURL_VQUIC_QUIC_INT_H
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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_HTTP3

#అన్తర్భావ "bufq.h"
#అన్తర్భావ "u32_ptrset.h"

#నిర్వచించు MAX_UDP_PAYLOAD_కొలత  1452

/* definitions from RFC 9114, ch 8.1 */
రకనిర్వచనం గణన {
  CURL_H3_ERR_NO_ERROR = 0x0100,
  CURL_H3_ERR_GENERAL_PROTOCOL_ERROR = 0x0101,
  CURL_H3_ERR_అంతర్గత_ERROR = 0x0102,
  CURL_H3_ERR_ప్రవాహం_CREATION_ERROR = 0x0103,
  CURL_H3_ERR_CLOSED_CRITICAL_ప్రవాహం = 0x0104,
  CURL_H3_ERR_చట్రం_UNEXPECTED = 0x0105,
  CURL_H3_ERR_చట్రం_ERROR = 0x0106,
  CURL_H3_ERR_EXCESSIVE_ఎక్కించు = 0x0107,
  CURL_H3_ERR_ID_ERROR = 0x0108,
  CURL_H3_ERR_SETTINGS_ERROR = 0x0109,
  CURL_H3_ERR_MISSING_SETTINGS = 0x010a,
  CURL_H3_ERR_REQUEST_REJECTED = 0x010b,
  CURL_H3_ERR_REQUEST_CANCELLED = 0x010c,
  CURL_H3_ERR_REQUEST_INCOMPLETE = 0x010d,
  CURL_H3_ERR_సందేశం_ERROR = 0x010e,
  CURL_H3_ERR_CONNECT_ERROR = 0x010f,
  CURL_H3_ERR_సంస్కరణ_FALLBACK = 0x0110,
} vquic_h3_error;

#నిర్వచితమైతే CURLVERBOSE
స్థిరరాశి అక్షరం *Curl_vquic_h3_err_str(uint64_t error_సంకేతం);
#లేకపోతే
#నిర్వచించు Curl_vquic_h3_err_str(x)   ""
#అయితే_ముగింపు /* CURLVERBOSE */

నిర్మాణం cf_quic_ctx {
  curl_socket_t sockfd;               /* connected UDP socket */
  నిర్మాణం sockaddr_storage local_addr; /* address socket is bound to */
  socklen_t local_addrlen;            /* length of local address */

  నిర్మాణం bufq sendbuf;           /* buffer for sending one or more packets */
  నిర్మాణం curltime మొదటి_byte_at; /* when first byte was recvd */
  నిర్మాణం curltime చివరి_op;       /* last (attempted) send/recv operation */
  నిర్మాణం curltime చివరి_io;       /* last successful socket IO */
  size_t gsolen;                 /* length of individual packets in send buf */
  size_t విభజించు_len;    /* if != 0, buffer length after which GSO differs */
  size_t విభజించు_gsolen; /* length of individual packets after split_len */
#నిర్వచితమైతే DEBUGBUILD
  పూర్ణాంకం wblock_percent;  /* percent of writes doing EAGAIN */
#అయితే_ముగింపు
  బిట్టు(got_మొదటి_byte); /* if first byte was received */
  బిట్టు(no_gso);         /* do not use gso on sending */
};

#నిర్వచించు H3_ప్రవాహం_CTX(ctx, దత్తాంశం)                                        \
  ((దత్తాంశం) ? Curl_u32_ptrset_get(&(ctx)->streams, (దత్తాంశం)->mid) : ఖాళీ)

CURLcode Curl_vquic_ctx_init(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం cf_quic_ctx *qctx);
శూన్య Curl_vquic_ctx_free(నిర్మాణం cf_quic_ctx *qctx);

శూన్య Curl_vquic_ctx_సమితి_సమయం(నిర్మాణం cf_quic_ctx *qctx,
                             స్థిరరాశి నిర్మాణం curltime *pnow);

శూన్య Curl_vquic_ctx_update_సమయం(నిర్మాణం cf_quic_ctx *qctx,
                                స్థిరరాశి నిర్మాణం curltime *pnow);

CURLcode Curl_vquic_send(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                         నిర్మాణం cf_quic_ctx *qctx, size_t gsolen);

CURLcode Curl_vquic_send_తోక_విభజించు(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం cf_quic_ctx *qctx, size_t gsolen,
                                    size_t తోక_len, size_t తోక_gsolen);

CURLcode Curl_vquic_flush(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                          నిర్మాణం cf_quic_ctx *qctx);

రకనిర్వచనం CURLcode Curl_vquic_recv_pkts_cb(స్థిరరాశి గుర్తులేని అక్షరం *buf,
                                         size_t buflen,
                                         size_t gso_కొలత,
                                         నిర్మాణం sockaddr_storage *remote_addr,
                                         socklen_t remote_addrlen, uint8_t ecn,
                                         శూన్య *userp);

CURLcode Curl_vquic_recv_packets(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb,
                                 శూన్య *userp);

#నిర్వచితమైతే USE_NGTCP2
నిర్మాణం ngtcp2_mem;
నిర్మాణం ngtcp2_mem *Curl_ngtcp2_mem(శూన్య);
#అయితే_ముగింపు
#నిర్వచితమైతే USE_NGHTTP3
నిర్మాణం nghttp3_mem;
నిర్మాణం nghttp3_mem *Curl_nghttp3_mem(శూన్య);
#అయితే_ముగింపు

#అయితే_ముగింపు /* !USE_HTTP3 */

#అయితే_ముగింపు /* HEADER_CURL_VQUIC_QUIC_INT_H */
