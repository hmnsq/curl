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

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>  /* for htons() */
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <curl/curl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/dynbuf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bufq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/capsule.h"


/**
 * Convert 64-bit value from network byte order to host byte order
 */
ਸਥਿਰ uint64_t capsule_ntohll(uint64_t ਮੁੱਲ)
{
#ਜੇ defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
  ਵਾਪਸ ਮੁੱਲ;
#ਜਾਂਜੇ (defined(__GNUC__) || defined(__clang__)) && \
  defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  ਵਾਪਸ __builtin_bswap64(ਮੁੱਲ);
#ਨਹੀਂਤਾਂ
  ਸੰਘ {
    uint64_t u64;
    uint32_t u32[2];
  } src, dst;

  src.u64 = ਮੁੱਲ;
  dst.u32[0] = ntohl(src.u32[1]);
  dst.u32[1] = ntohl(src.u32[0]);
  ਵਾਪਸ dst.u64;
#ਜੇ_ਅੰਤ
}

/**
 * Encode a variable-length integer into a plain buffer.
 * @param buf   Output buffer (must have at least 8 bytes)
 * @param value Value to encode (must be <= 0x3FFFFFFFFFFFFFFF)
 * @return Number of bytes written
 */
ਸਥਿਰ size_t capsule_encode_varint_buf(uint8_t *buf, uint64_t ਮੁੱਲ)
{
  DEBUGASSERT(ਮੁੱਲ <= 0x3FFFFFFFFFFFFFFF);

  ਜੇ(ਮੁੱਲ <= 0x3F) {
    buf[0] = (uint8_t)ਮੁੱਲ;
    ਵਾਪਸ 1;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਮੁੱਲ <= 0x3FFF) {
    uint16_t encoded = (uint16_t)ਮੁੱਲ & 0x3FFF;
    encoded = ntohs(encoded | 0x4000);
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf, &encoded, 2);
    ਵਾਪਸ 2;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਮੁੱਲ <= 0x3FFFFFFF) {
    uint32_t encoded = (uint32_t)ਮੁੱਲ & 0x3FFFFFFF;
    encoded = ntohl(encoded | 0x80000000);
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf, &encoded, 4);
    ਵਾਪਸ 4;
  }
  ਨਹੀਂਤਾਂ {
    uint64_t encoded = (uint64_t)ਮੁੱਲ & 0x3FFFFFFFFFFFFFFF;
    encoded = capsule_ntohll(encoded | 0xC000000000000000);
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf, &encoded, 8);
    ਵਾਪਸ 8;
  }
}

ਸਥਿਰ CURLcode capsule_peek_u8(ਬਣਤਰ bufq *recvbufq,
                                size_t ਵਿੱਥ,
                                uint8_t *pbyte)
{
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *peek = ਨਿਰਾ;
  size_t peeklen = 0;

  ਜੇ(!Curl_bufq_peek_at(recvbufq, ਵਿੱਥ, &peek, &peeklen) || !peeklen)
    ਵਾਪਸ CURLE_AGAIN;
  *pbyte = peek[0];
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode capsule_decode_varint_at(ਬਣਤਰ bufq *recvbufq,
                                         size_t ਵਿੱਥ,
                                         uint64_t *pvalue,
                                         size_t *pconsumed)
{
  uint8_t ਪਹਿਲਾ_byte, byte;
  uint64_t ਮੁੱਲ;
  size_t nbytes;
  size_t i;
  CURLcode ਨਤੀਜਾ;

  ਨਤੀਜਾ = capsule_peek_u8(recvbufq, ਵਿੱਥ, &ਪਹਿਲਾ_byte);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  nbytes = (size_t)1 << (ਪਹਿਲਾ_byte >> 6); /* 1, 2, 4 or 8 bytes */
  ਮੁੱਲ = ਪਹਿਲਾ_byte & 0x3F;

  ਲਈ(i = 1; i < nbytes; ++i) {
    ਨਤੀਜਾ = capsule_peek_u8(recvbufq, ਵਿੱਥ + i, &byte);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    ਮੁੱਲ = (ਮੁੱਲ << 8) | byte;
  }

  *pvalue = ਮੁੱਲ;
  *pconsumed = nbytes;
  ਵਾਪਸ CURLE_OK;
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
  DEBUGASSERT(hdrlen >= HTTP_CAPSULE_ਸਿਰਲੇਖ_MAX_ਮਾਪ);
  ਜੇ(hdrlen < HTTP_CAPSULE_ਸਿਰਲੇਖ_MAX_ਮਾਪ)
    ਵਾਪਸ 0;
  hdr[off++] = 0; /* capsule type: HTTP Datagram */
  off += capsule_encode_varint_buf(hdr + off, (uint64_t)payload_len + 1);
  hdr[off++] = 0; /* context ID */
  ਵਾਪਸ off;
}

CURLcode Curl_capsule_encap_udp_datagram(ਬਣਤਰ bufq *q,
                                         ਅਚਲ ਖਾਲੀ *buf, size_t blen)
{
  CURLcode ਨਤੀਜਾ;
  uint8_t hdr[HTTP_CAPSULE_ਸਿਰਲੇਖ_MAX_ਮਾਪ];
  size_t hdr_len, nwritten;

  hdr_len = capsule_encap_udp_hdr(hdr, ਆਕਾਰ(hdr), blen);
  DEBUGASSERT(hdr_len);
  ਜੇ(!hdr_len)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਨਤੀਜਾ = Curl_bufq_write(q, hdr, hdr_len, &nwritten);
  ਜੇ(!ਨਤੀਜਾ && (nwritten != hdr_len))
    ਵਾਪਸ CURLE_WRITE_ERROR;
  ਜੇ(!ਨਤੀਜਾ) {
    ਨਤੀਜਾ = Curl_bufq_write(q, buf, blen, &nwritten);
    ਜੇ(!ਨਤੀਜਾ && (nwritten != blen))
      ਵਾਪਸ CURLE_WRITE_ERROR;
  }
  ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
    ਵਾਪਸ CURLE_WRITE_ERROR;
  ਵਾਪਸ ਨਤੀਜਾ;
}

size_t Curl_capsule_process_udp_raw(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ bufq *recvbufq,
                                    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf, size_t len,
                                    CURLcode *err)
{
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *context_id, *capsule_type;
  size_t read_ਮਾਪ, varint_len;
  uint64_t capsule_ਲੰਮਾਈ;
  size_t ਵਿੱਥ, payload_len;
  size_t ਬਾਈਟਾਂ_read = 0;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(!len) {
    *err = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਵਾਪਸ 0;
  }

  ਜੇ(Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(recvbufq)) {
    *err = CURLE_AGAIN;
    ਵਾਪਸ 0;
  }

  ਜੇ(!Curl_bufq_peek(recvbufq, &capsule_type, &read_ਮਾਪ) || !read_ਮਾਪ) {
    *err = CURLE_AGAIN;
    ਵਾਪਸ 0;
  }

  ਜੇ(capsule_type[0]) {
    infof(ਅੰਕੜੇ, "Error! Invalid capsule type: %d", capsule_type[0]);
    Curl_bufq_skip(recvbufq, 1);
    *err = CURLE_RECV_ERROR;
    ਵਾਪਸ 0;
  }

  ਵਿੱਥ = 1;
  ਨਤੀਜਾ = capsule_decode_varint_at(recvbufq, ਵਿੱਥ, &capsule_ਲੰਮਾਈ,
                                    &varint_len);
  ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
    *err = CURLE_AGAIN;
    ਵਾਪਸ 0;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ) {
    *err = CURLE_RECV_ERROR;
    ਵਾਪਸ 0;
  }
  ਵਿੱਥ += varint_len;

  ਜੇ(!Curl_bufq_peek_at(recvbufq, ਵਿੱਥ, &context_id, &read_ਮਾਪ) ||
     !read_ਮਾਪ) {
    *err = CURLE_AGAIN;
    ਵਾਪਸ 0;
  }

  ਜੇ(*context_id) {
    infof(ਅੰਕੜੇ, "Error! Invalid context ID: %02x", *context_id);
    Curl_bufq_skip(recvbufq, ਵਿੱਥ + 1);
    *err = CURLE_RECV_ERROR;
    ਵਾਪਸ 0;
  }
  ਵਿੱਥ += 1;

  ਜੇ(!capsule_ਲੰਮਾਈ) {
    infof(ਅੰਕੜੇ, "Error! Invalid capsule length: 0");
    Curl_bufq_skip(recvbufq, ਵਿੱਥ);
    *err = CURLE_RECV_ERROR;
    ਵਾਪਸ 0;
  }
  ਜੇ(capsule_ਲੰਮਾਈ - 1 > (uint64_t)(SIZE_MAX - ਵਿੱਥ)) {
    infof(ਅੰਕੜੇ, "Error! Capsule length too large: %" CURL_FORMAT_CURL_OFF_T,
          (curl_off_t)capsule_ਲੰਮਾਈ);
    *err = CURLE_RECV_ERROR;
    ਵਾਪਸ 0;
  }
  payload_len = (size_t)(capsule_ਲੰਮਾਈ - 1);

  ਜੇ(payload_len > len) {
    infof(ਅੰਕੜੇ, "UDP payload does not fit destination buffer: %zu > %zu",
          payload_len, len);
    Curl_bufq_skip(recvbufq, ਵਿੱਥ + payload_len);
    *err = CURLE_RECV_ERROR;
    ਵਾਪਸ 0;
  }

  ਜੇ(Curl_bufq_len(recvbufq) < ਵਿੱਥ + payload_len) {
    ਜੇ(Curl_bufq_is_ਭਰਿਆ(recvbufq)) {
      infof(ਅੰਕੜੇ, "Error! Capsule does not fit the receive buffer: %zu",
            payload_len);
      *err = CURLE_RECV_ERROR;
      ਵਾਪਸ 0;
    }
    *err = CURLE_AGAIN;
    ਵਾਪਸ 0;
  }

  Curl_bufq_skip(recvbufq, ਵਿੱਥ);
  ਜੇ(!payload_len) {
    *err = CURLE_OK;
    ਵਾਪਸ 0;
  }
  ਨਤੀਜਾ = Curl_bufq_read(recvbufq, buf, payload_len, &ਬਾਈਟਾਂ_read);
  ਜੇ(ਨਤੀਜਾ || (ਬਾਈਟਾਂ_read != payload_len)) {
    infof(ਅੰਕੜੇ, "Error! Read less than expected %zu %zu",
          payload_len, ਬਾਈਟਾਂ_read);
    *err = CURLE_RECV_ERROR;
    ਵਾਪਸ 0;
  }

  ਜੇ(cf && ਅੰਕੜੇ) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "Processed UDP capsule raw: size=%zu "
                "length_left %zu", payload_len, Curl_bufq_len(recvbufq));
  }
  *err = CURLE_OK;
  ਵਾਪਸ ਬਾਈਟਾਂ_read;
}

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
