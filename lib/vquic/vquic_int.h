#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_VQUIC_QUIC_INT_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_VQUIC_QUIC_INT_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTP3

#ಅನ್ತರ್ಭಾವ "bufq.h"
#ಅನ್ತರ್ಭಾವ "u32_ptrset.h"

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_UDP_PAYLOAD_ಗಾತ್ರ  1452

/* definitions from RFC 9114, ch 8.1 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  CURL_H3_ERR_NO_ERROR = 0x0100,
  CURL_H3_ERR_GENERAL_PROTOCOL_ERROR = 0x0101,
  CURL_H3_ERR_ಆಂತರಿಕ_ERROR = 0x0102,
  CURL_H3_ERR_ಹರಿವು_CREATION_ERROR = 0x0103,
  CURL_H3_ERR_CLOSED_CRITICAL_ಹರಿವು = 0x0104,
  CURL_H3_ERR_ಚೌಕಟ್ಟು_UNEXPECTED = 0x0105,
  CURL_H3_ERR_ಚೌಕಟ್ಟು_ERROR = 0x0106,
  CURL_H3_ERR_EXCESSIVE_ತುಂಬು = 0x0107,
  CURL_H3_ERR_ID_ERROR = 0x0108,
  CURL_H3_ERR_SETTINGS_ERROR = 0x0109,
  CURL_H3_ERR_MISSING_SETTINGS = 0x010a,
  CURL_H3_ERR_REQUEST_REJECTED = 0x010b,
  CURL_H3_ERR_REQUEST_CANCELLED = 0x010c,
  CURL_H3_ERR_REQUEST_INCOMPLETE = 0x010d,
  CURL_H3_ERR_ಸಂದೇಶ_ERROR = 0x010e,
  CURL_H3_ERR_CONNECT_ERROR = 0x010f,
  CURL_H3_ERR_ಆವೃತ್ತಿ_FALLBACK = 0x0110,
} vquic_h3_error;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Curl_vquic_h3_err_str(uint64_t error_ಸಂಕೇತ);
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_vquic_h3_err_str(x)   ""
#ಆದರೆ_ಅಂತ್ಯ /* CURLVERBOSE */

ರಚನೆ cf_quic_ctx {
  curl_socket_t sockfd;               /* connected UDP socket */
  ರಚನೆ sockaddr_storage local_addr; /* address socket is bound to */
  socklen_t local_addrlen;            /* length of local address */

  ರಚನೆ bufq sendbuf;           /* buffer for sending one or more packets */
  ರಚನೆ curltime ಮೊದಲ_byte_at; /* when first byte was recvd */
  ರಚನೆ curltime ಕೊನೆಯ_op;       /* last (attempted) send/recv operation */
  ರಚನೆ curltime ಕೊನೆಯ_io;       /* last successful socket IO */
  size_t gsolen;                 /* length of individual packets in send buf */
  size_t ವಿಭಜಿಸು_len;    /* if != 0, buffer length after which GSO differs */
  size_t ವಿಭಜಿಸು_gsolen; /* length of individual packets after split_len */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಪೂರ್ಣಾಂಕ wblock_percent;  /* percent of writes doing EAGAIN */
#ಆದರೆ_ಅಂತ್ಯ
  ಬಿಟ್(got_ಮೊದಲ_byte); /* if first byte was received */
  ಬಿಟ್(no_gso);         /* do not use gso on sending */
};

#ವ್ಯಾಖ್ಯಾನಿಸು H3_ಹರಿವು_CTX(ctx, ದತ್ತಾಂಶ)                                        \
  ((ದತ್ತಾಂಶ) ? Curl_u32_ptrset_get(&(ctx)->streams, (ದತ್ತಾಂಶ)->mid) : ಖಾಲಿ)

CURLcode Curl_vquic_ctx_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ cf_quic_ctx *qctx);
ಶೂನ್ಯ Curl_vquic_ctx_free(ರಚನೆ cf_quic_ctx *qctx);

ಶೂನ್ಯ Curl_vquic_ctx_ಗಣ_ಸಮಯ(ರಚನೆ cf_quic_ctx *qctx,
                             ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow);

ಶೂನ್ಯ Curl_vquic_ctx_update_ಸಮಯ(ರಚನೆ cf_quic_ctx *qctx,
                                ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *pnow);

CURLcode Curl_vquic_send(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         ರಚನೆ cf_quic_ctx *qctx, size_t gsolen);

CURLcode Curl_vquic_send_ಬಾಲ_ವಿಭಜಿಸು(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ cf_quic_ctx *qctx, size_t gsolen,
                                    size_t ಬಾಲ_len, size_t ಬಾಲ_gsolen);

CURLcode Curl_vquic_flush(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ರಚನೆ cf_quic_ctx *qctx);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CURLcode Curl_vquic_recv_pkts_cb(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf,
                                         size_t buflen,
                                         size_t gso_ಗಾತ್ರ,
                                         ರಚನೆ sockaddr_storage *remote_addr,
                                         socklen_t remote_addrlen, uint8_t ecn,
                                         ಶೂನ್ಯ *userp);

CURLcode Curl_vquic_recv_packets(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ cf_quic_ctx *qctx,
                                 size_t max_pkts,
                                 Curl_vquic_recv_pkts_cb *recv_cb,
                                 ಶೂನ್ಯ *userp);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NGTCP2
ರಚನೆ ngtcp2_mem;
ರಚನೆ ngtcp2_mem *Curl_ngtcp2_mem(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NGHTTP3
ರಚನೆ nghttp3_mem;
ರಚನೆ nghttp3_mem *Curl_nghttp3_mem(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* !USE_HTTP3 */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_VQUIC_QUIC_INT_H */
