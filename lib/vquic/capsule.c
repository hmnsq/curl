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

#ಯದಿ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#ifdef HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>  /* for htons() */
#endif

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
#ಯದಿ defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
  ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ;
#ನೋಚೇದ್ಯದಿ (defined(__GNUC__) || defined(__clang__)) && \
  defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  ಪ್ರತ್ಯಾಗಮ __builtin_bswap64(ಮೌಲ್ಯ);
#ಅನ್ಯಥಾ
  ಸಙ್ಘ {
    uint64_t u64;
    uint32_t u32[2];
  } src, dst;

  src.u64 = ಮೌಲ್ಯ;
  dst.u32[0] = ntohl(src.u32[1]);
  dst.u32[1] = ntohl(src.u32[0]);
  ಪ್ರತ್ಯಾಗಮ dst.u64;
#endif
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

  ಯದಿ(ಮೌಲ್ಯ <= 0x3F) {
    buf[0] = (uint8_t)ಮೌಲ್ಯ;
    ಪ್ರತ್ಯಾಗಮ 1;
  }
  ಅನ್ಯಥಾ ಯದಿ(ಮೌಲ್ಯ <= 0x3FFF) {
    uint16_t encoded = (uint16_t)ಮೌಲ್ಯ & 0x3FFF;
    encoded = ntohs(encoded | 0x4000);
    memcpy(buf, &encoded, 2);
    ಪ್ರತ್ಯಾಗಮ 2;
  }
  ಅನ್ಯಥಾ ಯದಿ(ಮೌಲ್ಯ <= 0x3FFFFFFF) {
    uint32_t encoded = (uint32_t)ಮೌಲ್ಯ & 0x3FFFFFFF;
    encoded = ntohl(encoded | 0x80000000);
    memcpy(buf, &encoded, 4);
    ಪ್ರತ್ಯಾಗಮ 4;
  }
  ಅನ್ಯಥಾ {
    uint64_t encoded = (uint64_t)ಮೌಲ್ಯ & 0x3FFFFFFFFFFFFFFF;
    encoded = capsule_ntohll(encoded | 0xC000000000000000);
    memcpy(buf, &encoded, 8);
    ಪ್ರತ್ಯಾಗಮ 8;
  }
}

ಸ್ಥಿರ CURLcode capsule_peek_u8(ಸಂರಚನಾ bufq *recvbufq,
                                size_t offset,
                                uint8_t *pbyte)
{
  ಧ್ರುವ unsigned ವರ್ಣ *peek = ರಿಕ್ತ;
  size_t peeklen = 0;

  ಯದಿ(!Curl_bufq_peek_at(recvbufq, offset, &peek, &peeklen) || !peeklen)
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
  *pbyte = peek[0];
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode capsule_decode_varint_at(ಸಂರಚನಾ bufq *recvbufq,
                                         size_t offset,
                                         uint64_t *pvalue,
                                         size_t *pconsumed)
{
  uint8_t first_byte, byte;
  uint64_t ಮೌಲ್ಯ;
  size_t nbytes;
  size_t i;
  CURLcode result;

  result = capsule_peek_u8(recvbufq, offset, &first_byte);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  nbytes = (size_t)1 << (first_byte >> 6); /* 1, 2, 4 or 8 bytes */
  ಮೌಲ್ಯ = first_byte & 0x3F;

  ಪ್ರತಿ(i = 1; i < nbytes; ++i) {
    result = capsule_peek_u8(recvbufq, offset + i, &byte);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    ಮೌಲ್ಯ = (ಮೌಲ್ಯ << 8) | byte;
  }

  *pvalue = ಮೌಲ್ಯ;
  *pconsumed = nbytes;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
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
  DEBUGASSERT(hdrlen >= HTTP_CAPSULE_HEADER_MAX_SIZE);
  ಯದಿ(hdrlen < HTTP_CAPSULE_HEADER_MAX_SIZE)
    ಪ್ರತ್ಯಾಗಮ 0;
  hdr[off++] = 0; /* capsule type: HTTP Datagram */
  off += capsule_encode_varint_buf(hdr + off, (uint64_t)payload_len + 1);
  hdr[off++] = 0; /* context ID */
  ಪ್ರತ್ಯಾಗಮ off;
}

CURLcode Curl_capsule_encap_udp_datagram(ಸಂರಚನಾ bufq *q,
                                         ಧ್ರುವ ಶೂನ್ಯ *buf, size_t blen)
{
  CURLcode result;
  uint8_t hdr[HTTP_CAPSULE_HEADER_MAX_SIZE];
  size_t hdr_len, nwritten;

  hdr_len = capsule_encap_udp_hdr(hdr, ಪರಿಮಾಣ(hdr), blen);
  DEBUGASSERT(hdr_len);
  ಯದಿ(!hdr_len)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  result = Curl_bufq_write(q, hdr, hdr_len, &nwritten);
  ಯದಿ(!result && (nwritten != hdr_len))
    ಪ್ರತ್ಯಾಗಮ CURLE_WRITE_ERROR;
  ಯದಿ(!result) {
    result = Curl_bufq_write(q, buf, blen, &nwritten);
    ಯದಿ(!result && (nwritten != blen))
      ಪ್ರತ್ಯಾಗಮ CURLE_WRITE_ERROR;
  }
  ಯದಿ(result == CURLE_AGAIN)
    ಪ್ರತ್ಯಾಗಮ CURLE_WRITE_ERROR;
  ಪ್ರತ್ಯಾಗಮ result;
}

size_t Curl_capsule_process_udp_raw(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ bufq *recvbufq,
                                    unsigned ವರ್ಣ *buf, size_t len,
                                    CURLcode *err)
{
  ಧ್ರುವ unsigned ವರ್ಣ *context_id, *capsule_type;
  size_t read_size, varint_len;
  uint64_t capsule_length;
  size_t offset, payload_len;
  size_t bytes_read = 0;
  CURLcode result = CURLE_OK;

  ಯದಿ(!len) {
    *err = CURLE_BAD_FUNCTION_ARGUMENT;
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಯದಿ(Curl_bufq_is_empty(recvbufq)) {
    *err = CURLE_AGAIN;
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಯದಿ(!Curl_bufq_peek(recvbufq, &capsule_type, &read_size) || !read_size) {
    *err = CURLE_AGAIN;
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಯದಿ(capsule_type[0]) {
    infof(data, "Error! Invalid capsule type: %d", capsule_type[0]);
    Curl_bufq_skip(recvbufq, 1);
    *err = CURLE_RECV_ERROR;
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  offset = 1;
  result = capsule_decode_varint_at(recvbufq, offset, &capsule_length,
                                    &varint_len);
  ಯದಿ(result == CURLE_AGAIN) {
    *err = CURLE_AGAIN;
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಅನ್ಯಥಾ ಯದಿ(result) {
    *err = CURLE_RECV_ERROR;
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  offset += varint_len;

  ಯದಿ(!Curl_bufq_peek_at(recvbufq, offset, &context_id, &read_size) ||
     !read_size) {
    *err = CURLE_AGAIN;
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಯದಿ(*context_id) {
    infof(data, "Error! Invalid context ID: %02x", *context_id);
    Curl_bufq_skip(recvbufq, offset + 1);
    *err = CURLE_RECV_ERROR;
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  offset += 1;

  ಯದಿ(!capsule_length) {
    infof(data, "Error! Invalid capsule length: 0");
    Curl_bufq_skip(recvbufq, offset);
    *err = CURLE_RECV_ERROR;
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಯದಿ(capsule_length - 1 > (uint64_t)(SIZE_MAX - offset)) {
    infof(data, "Error! Capsule length too large: %" CURL_FORMAT_CURL_OFF_T,
          (curl_off_t)capsule_length);
    *err = CURLE_RECV_ERROR;
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  payload_len = (size_t)(capsule_length - 1);

  ಯದಿ(payload_len > len) {
    infof(data, "UDP payload does not fit destination buffer: %zu > %zu",
          payload_len, len);
    Curl_bufq_skip(recvbufq, offset + payload_len);
    *err = CURLE_RECV_ERROR;
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಯದಿ(Curl_bufq_len(recvbufq) < offset + payload_len) {
    ಯದಿ(Curl_bufq_is_full(recvbufq)) {
      infof(data, "Error! Capsule does not fit the receive buffer: %zu",
            payload_len);
      *err = CURLE_RECV_ERROR;
      ಪ್ರತ್ಯಾಗಮ 0;
    }
    *err = CURLE_AGAIN;
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  Curl_bufq_skip(recvbufq, offset);
  ಯದಿ(!payload_len) {
    *err = CURLE_OK;
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  result = Curl_bufq_read(recvbufq, buf, payload_len, &bytes_read);
  ಯದಿ(result || (bytes_read != payload_len)) {
    infof(data, "Error! Read less than expected %zu %zu",
          payload_len, bytes_read);
    *err = CURLE_RECV_ERROR;
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಯದಿ(cf && data) {
    CURL_TRC_CF(data, cf, "Processed UDP capsule raw: size=%zu "
                "length_left %zu", payload_len, Curl_bufq_len(recvbufq));
  }
  *err = CURLE_OK;
  ಪ್ರತ್ಯಾಗಮ bytes_read;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
