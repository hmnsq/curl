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

#ಆದರೆ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>  /* for htons() */
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <curl/curl.h>
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "bufq.h"
#ಅನ್ತರ್ಭಾವ "vquic/capsule.h"


/**
 * Convert 64-bit value from network byte order to host byte order
 */
ಸ್ಥಿರ uint64_t capsule_ntohll(uint64_t ಮೌಲ್ಯ)
{
#ಆದರೆ defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
  ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ;
#ಅಥವಾಆದರೆ (defined(__GNUC__) || defined(__clang__)) && \
  defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  ಹಿಂತಿರುಗಿ __builtin_bswap64(ಮೌಲ್ಯ);
#ಇಲ್ಲದಿದ್ದರೆ
  ಒಕ್ಕೂಟ {
    uint64_t u64;
    uint32_t u32[2];
  } src, dst;

  src.u64 = ಮೌಲ್ಯ;
  dst.u32[0] = ntohl(src.u32[1]);
  dst.u32[1] = ntohl(src.u32[0]);
  ಹಿಂತಿರುಗಿ dst.u64;
#ಆದರೆ_ಅಂತ್ಯ
}

/**
 * Encode a variable-length integer into a plain buffer.
 * @param buf   Output buffer (must have at least 8 bytes)
 * @param value Value to encode (must be <= 0x3FFFFFFFFFFFFFFF)
 * @return Number of bytes written
 */
ಸ್ಥಿರ size_t capsule_encode_varint_buf(uint8_t *buf, uint64_t ಮೌಲ್ಯ)
{
  DEBUGASSERT(ಮೌಲ್ಯ <= 0x3FFFFFFFFFFFFFFF);

  ಆದರೆ(ಮೌಲ್ಯ <= 0x3F) {
    buf[0] = (uint8_t)ಮೌಲ್ಯ;
    ಹಿಂತಿರುಗಿ 1;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಮೌಲ್ಯ <= 0x3FFF) {
    uint16_t encoded = (uint16_t)ಮೌಲ್ಯ & 0x3FFF;
    encoded = ntohs(encoded | 0x4000);
    ಸ್ಮರಣೆ_ನಕಲು(buf, &encoded, 2);
    ಹಿಂತಿರುಗಿ 2;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಮೌಲ್ಯ <= 0x3FFFFFFF) {
    uint32_t encoded = (uint32_t)ಮೌಲ್ಯ & 0x3FFFFFFF;
    encoded = ntohl(encoded | 0x80000000);
    ಸ್ಮರಣೆ_ನಕಲು(buf, &encoded, 4);
    ಹಿಂತಿರುಗಿ 4;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    uint64_t encoded = (uint64_t)ಮೌಲ್ಯ & 0x3FFFFFFFFFFFFFFF;
    encoded = capsule_ntohll(encoded | 0xC000000000000000);
    ಸ್ಮರಣೆ_ನಕಲು(buf, &encoded, 8);
    ಹಿಂತಿರುಗಿ 8;
  }
}

ಸ್ಥಿರ CURLcode capsule_peek_u8(ರಚನೆ bufq *recvbufq,
                                size_t ಸ್ಥಾನಾಂತರ,
                                uint8_t *pbyte)
{
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *peek = ಖಾಲಿ;
  size_t peeklen = 0;

  ಆದರೆ(!Curl_bufq_peek_at(recvbufq, ಸ್ಥಾನಾಂತರ, &peek, &peeklen) || !peeklen)
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;
  *pbyte = peek[0];
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode capsule_decode_varint_at(ರಚನೆ bufq *recvbufq,
                                         size_t ಸ್ಥಾನಾಂತರ,
                                         uint64_t *pvalue,
                                         size_t *pconsumed)
{
  uint8_t ಮೊದಲ_byte, byte;
  uint64_t ಮೌಲ್ಯ;
  size_t nbytes;
  size_t i;
  CURLcode ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = capsule_peek_u8(recvbufq, ಸ್ಥಾನಾಂತರ, &ಮೊದಲ_byte);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  nbytes = (size_t)1 << (ಮೊದಲ_byte >> 6); /* 1, 2, 4 or 8 bytes */
  ಮೌಲ್ಯ = ಮೊದಲ_byte & 0x3F;

  ಗಾಗಿ(i = 1; i < nbytes; ++i) {
    ಫಲಿತಾಂಶ = capsule_peek_u8(recvbufq, ಸ್ಥಾನಾಂತರ + i, &byte);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    ಮೌಲ್ಯ = (ಮೌಲ್ಯ << 8) | byte;
  }

  *pvalue = ಮೌಲ್ಯ;
  *pconsumed = nbytes;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/**
 * Write the capsule header (type + varint length + context ID) into `hdr`.
 * @param hdr         Output buffer (must be >= HTTP_CAPSULE_HEADER_MAX_SIZE)
 * @param hdrlen      Size of `hdr` in bytes
 * @param payload_len Length of the UDP payload that follows
 * @return Number of header bytes written, or 0 on error
 *
 * @unittest 3400
 */
UNITTEST size_t capsule_encap_udp_hdr(uint8_t *hdr, size_t hdrlen,
                                      size_t payload_len);
UNITTEST size_t capsule_encap_udp_hdr(uint8_t *hdr, size_t hdrlen,
                                      size_t payload_len)
{
  size_t off = 0;
  DEBUGASSERT(hdrlen >= HTTP_CAPSULE_ಶೀರ್ಷಿಕೆ_MAX_ಗಾತ್ರ);
  ಆದರೆ(hdrlen < HTTP_CAPSULE_ಶೀರ್ಷಿಕೆ_MAX_ಗಾತ್ರ)
    ಹಿಂತಿರುಗಿ 0;
  hdr[off++] = 0; /* capsule type: HTTP Datagram */
  off += capsule_encode_varint_buf(hdr + off, (uint64_t)payload_len + 1);
  hdr[off++] = 0; /* context ID */
  ಹಿಂತಿರುಗಿ off;
}

CURLcode Curl_capsule_encap_udp_datagram(ರಚನೆ bufq *q,
                                         ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *buf, size_t blen)
{
  CURLcode ಫಲಿತಾಂಶ;
  uint8_t hdr[HTTP_CAPSULE_ಶೀರ್ಷಿಕೆ_MAX_ಗಾತ್ರ];
  size_t hdr_len, nwritten;

  hdr_len = capsule_encap_udp_hdr(hdr, ಗಾತ್ರ(hdr), blen);
  DEBUGASSERT(hdr_len);
  ಆದರೆ(!hdr_len)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಫಲಿತಾಂಶ = Curl_bufq_write(q, hdr, hdr_len, &nwritten);
  ಆದರೆ(!ಫಲಿತಾಂಶ && (nwritten != hdr_len))
    ಹಿಂತಿರುಗಿ CURLE_WRITE_ERROR;
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ಫಲಿತಾಂಶ = Curl_bufq_write(q, buf, blen, &nwritten);
    ಆದರೆ(!ಫಲಿತಾಂಶ && (nwritten != blen))
      ಹಿಂತಿರುಗಿ CURLE_WRITE_ERROR;
  }
  ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
    ಹಿಂತಿರುಗಿ CURLE_WRITE_ERROR;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

size_t Curl_capsule_process_udp_raw(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ bufq *recvbufq,
                                    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf, size_t len,
                                    CURLcode *err)
{
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *context_id, *capsule_type;
  size_t read_ಗಾತ್ರ, varint_len;
  uint64_t capsule_ಉದ್ದಳತೆ;
  size_t ಸ್ಥಾನಾಂತರ, payload_len;
  size_t ಬೈಟುಗಳು_read = 0;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(!len) {
    *err = CURLE_BAD_FUNCTION_ವಾದ;
    ಹಿಂತಿರುಗಿ 0;
  }

  ಆದರೆ(Curl_bufq_is_ಖಾಲಿಗಳು(recvbufq)) {
    *err = CURLE_AGAIN;
    ಹಿಂತಿರುಗಿ 0;
  }

  ಆದರೆ(!Curl_bufq_peek(recvbufq, &capsule_type, &read_ಗಾತ್ರ) || !read_ಗಾತ್ರ) {
    *err = CURLE_AGAIN;
    ಹಿಂತಿರುಗಿ 0;
  }

  ಆದರೆ(capsule_type[0]) {
    infof(ದತ್ತಾಂಶ, "Error! Invalid capsule type: %d", capsule_type[0]);
    Curl_bufq_skip(recvbufq, 1);
    *err = CURLE_RECV_ERROR;
    ಹಿಂತಿರುಗಿ 0;
  }

  ಸ್ಥಾನಾಂತರ = 1;
  ಫಲಿತಾಂಶ = capsule_decode_varint_at(recvbufq, ಸ್ಥಾನಾಂತರ, &capsule_ಉದ್ದಳತೆ,
                                    &varint_len);
  ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
    *err = CURLE_AGAIN;
    ಹಿಂತಿರುಗಿ 0;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ) {
    *err = CURLE_RECV_ERROR;
    ಹಿಂತಿರುಗಿ 0;
  }
  ಸ್ಥಾನಾಂತರ += varint_len;

  ಆದರೆ(!Curl_bufq_peek_at(recvbufq, ಸ್ಥಾನಾಂತರ, &context_id, &read_ಗಾತ್ರ) ||
     !read_ಗಾತ್ರ) {
    *err = CURLE_AGAIN;
    ಹಿಂತಿರುಗಿ 0;
  }

  ಆದರೆ(*context_id) {
    infof(ದತ್ತಾಂಶ, "Error! Invalid context ID: %02x", *context_id);
    Curl_bufq_skip(recvbufq, ಸ್ಥಾನಾಂತರ + 1);
    *err = CURLE_RECV_ERROR;
    ಹಿಂತಿರುಗಿ 0;
  }
  ಸ್ಥಾನಾಂತರ += 1;

  ಆದರೆ(!capsule_ಉದ್ದಳತೆ) {
    infof(ದತ್ತಾಂಶ, "Error! Invalid capsule length: 0");
    Curl_bufq_skip(recvbufq, ಸ್ಥಾನಾಂತರ);
    *err = CURLE_RECV_ERROR;
    ಹಿಂತಿರುಗಿ 0;
  }
  ಆದರೆ(capsule_ಉದ್ದಳತೆ - 1 > (uint64_t)(SIZE_MAX - ಸ್ಥಾನಾಂತರ)) {
    infof(ದತ್ತಾಂಶ, "Error! Capsule length too large: %" CURL_FORMAT_CURL_OFF_T,
          (curl_off_t)capsule_ಉದ್ದಳತೆ);
    *err = CURLE_RECV_ERROR;
    ಹಿಂತಿರುಗಿ 0;
  }
  payload_len = (size_t)(capsule_ಉದ್ದಳತೆ - 1);

  ಆದರೆ(payload_len > len) {
    infof(ದತ್ತಾಂಶ, "UDP payload does not fit destination buffer: %zu > %zu",
          payload_len, len);
    Curl_bufq_skip(recvbufq, ಸ್ಥಾನಾಂತರ + payload_len);
    *err = CURLE_RECV_ERROR;
    ಹಿಂತಿರುಗಿ 0;
  }

  ಆದರೆ(Curl_bufq_len(recvbufq) < ಸ್ಥಾನಾಂತರ + payload_len) {
    ಆದರೆ(Curl_bufq_is_ತುಂಬಿದ(recvbufq)) {
      infof(ದತ್ತಾಂಶ, "Error! Capsule does not fit the receive buffer: %zu",
            payload_len);
      *err = CURLE_RECV_ERROR;
      ಹಿಂತಿರುಗಿ 0;
    }
    *err = CURLE_AGAIN;
    ಹಿಂತಿರುಗಿ 0;
  }

  Curl_bufq_skip(recvbufq, ಸ್ಥಾನಾಂತರ);
  ಆದರೆ(!payload_len) {
    *err = CURLE_OK;
    ಹಿಂತಿರುಗಿ 0;
  }
  ಫಲಿತಾಂಶ = Curl_bufq_read(recvbufq, buf, payload_len, &ಬೈಟುಗಳು_read);
  ಆದರೆ(ಫಲಿತಾಂಶ || (ಬೈಟುಗಳು_read != payload_len)) {
    infof(ದತ್ತಾಂಶ, "Error! Read less than expected %zu %zu",
          payload_len, ಬೈಟುಗಳು_read);
    *err = CURLE_RECV_ERROR;
    ಹಿಂತಿರುಗಿ 0;
  }

  ಆದರೆ(cf && ದತ್ತಾಂಶ) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "Processed UDP capsule raw: size=%zu "
                "length_left %zu", payload_len, Curl_bufq_len(recvbufq));
  }
  *err = CURLE_OK;
  ಹಿಂತಿರುಗಿ ಬೈಟುಗಳು_read;
}

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
