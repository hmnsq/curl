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

#إذا !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#ifdef HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>  /* for htons() */
#endif

#ضمن <curl/curl.h>
#ضمن "urldata.h"
#ضمن "curlx/dynbuf.h"
#ضمن "cfilters.h"
#ضمن "curl_trc.h"
#ضمن "bufq.h"
#ضمن "vquic/capsule.h"


/**
 * Convert 64-bit value from network byte order to host byte order
 */
ساكن uint64_t capsule_ntohll(uint64_t قيمة)
{
#إذا defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
  أرجع قيمة;
#وإلاإذا (defined(__GNUC__) || defined(__clang__)) && \
  defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  أرجع __builtin_bswap64(قيمة);
#وإلا
  اتحاد {
    uint64_t u64;
    uint32_t u32[2];
  } src, dst;

  src.u64 = قيمة;
  dst.u32[0] = ntohl(src.u32[1]);
  dst.u32[1] = ntohl(src.u32[0]);
  أرجع dst.u64;
#endif
}

/**
 * Encode a variable-length integer into a plain buffer.
 * @param buf   Output buffer (must have at least 8 bytes)
 * @param value Value to encode (must be <= 0x3FFFFFFFFFFFFFFF)
 * @return Number of bytes written
 */
ساكن size_t capsule_encode_varint_buf(uint8_t *buf, uint64_t قيمة)
{
  DEBUGASSERT(قيمة <= 0x3FFFFFFFFFFFFFFF);

  إذا(قيمة <= 0x3F) {
    buf[0] = (uint8_t)قيمة;
    أرجع 1;
  }
  وإلا إذا(قيمة <= 0x3FFF) {
    uint16_t encoded = (uint16_t)قيمة & 0x3FFF;
    encoded = ntohs(encoded | 0x4000);
    memcpy(buf, &encoded, 2);
    أرجع 2;
  }
  وإلا إذا(قيمة <= 0x3FFFFFFF) {
    uint32_t encoded = (uint32_t)قيمة & 0x3FFFFFFF;
    encoded = ntohl(encoded | 0x80000000);
    memcpy(buf, &encoded, 4);
    أرجع 4;
  }
  وإلا {
    uint64_t encoded = (uint64_t)قيمة & 0x3FFFFFFFFFFFFFFF;
    encoded = capsule_ntohll(encoded | 0xC000000000000000);
    memcpy(buf, &encoded, 8);
    أرجع 8;
  }
}

ساكن CURLcode capsule_peek_u8(بنية bufq *recvbufq,
                                size_t offset,
                                uint8_t *pbyte)
{
  ثابت unsigned حرف *peek = عدم;
  size_t peeklen = 0;

  إذا(!Curl_bufq_peek_at(recvbufq, offset, &peek, &peeklen) || !peeklen)
    أرجع CURLE_AGAIN;
  *pbyte = peek[0];
  أرجع CURLE_OK;
}

ساكن CURLcode capsule_decode_varint_at(بنية bufq *recvbufq,
                                         size_t offset,
                                         uint64_t *pvalue,
                                         size_t *pconsumed)
{
  uint8_t first_byte, byte;
  uint64_t قيمة;
  size_t nbytes;
  size_t i;
  CURLcode result;

  result = capsule_peek_u8(recvbufq, offset, &first_byte);
  إذا(result)
    أرجع result;

  nbytes = (size_t)1 << (first_byte >> 6); /* 1, 2, 4 or 8 bytes */
  قيمة = first_byte & 0x3F;

  لكل(i = 1; i < nbytes; ++i) {
    result = capsule_peek_u8(recvbufq, offset + i, &byte);
    إذا(result)
      أرجع result;
    قيمة = (قيمة << 8) | byte;
  }

  *pvalue = قيمة;
  *pconsumed = nbytes;
  أرجع CURLE_OK;
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
  إذا(hdrlen < HTTP_CAPSULE_HEADER_MAX_SIZE)
    أرجع 0;
  hdr[off++] = 0; /* capsule type: HTTP Datagram */
  off += capsule_encode_varint_buf(hdr + off, (uint64_t)payload_len + 1);
  hdr[off++] = 0; /* context ID */
  أرجع off;
}

CURLcode Curl_capsule_encap_udp_datagram(بنية bufq *q,
                                         ثابت فراغ *buf, size_t blen)
{
  CURLcode result;
  uint8_t hdr[HTTP_CAPSULE_HEADER_MAX_SIZE];
  size_t hdr_len, nwritten;

  hdr_len = capsule_encap_udp_hdr(hdr, حجم(hdr), blen);
  DEBUGASSERT(hdr_len);
  إذا(!hdr_len)
    أرجع CURLE_FAILED_INIT;

  result = Curl_bufq_write(q, hdr, hdr_len, &nwritten);
  إذا(!result && (nwritten != hdr_len))
    أرجع CURLE_WRITE_ERROR;
  إذا(!result) {
    result = Curl_bufq_write(q, buf, blen, &nwritten);
    إذا(!result && (nwritten != blen))
      أرجع CURLE_WRITE_ERROR;
  }
  إذا(result == CURLE_AGAIN)
    أرجع CURLE_WRITE_ERROR;
  أرجع result;
}

size_t Curl_capsule_process_udp_raw(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *data,
                                    بنية bufq *recvbufq,
                                    unsigned حرف *buf, size_t len,
                                    CURLcode *err)
{
  ثابت unsigned حرف *context_id, *capsule_type;
  size_t read_size, varint_len;
  uint64_t capsule_length;
  size_t offset, payload_len;
  size_t bytes_read = 0;
  CURLcode result = CURLE_OK;

  إذا(!len) {
    *err = CURLE_BAD_FUNCTION_ARGUMENT;
    أرجع 0;
  }

  إذا(Curl_bufq_is_empty(recvbufq)) {
    *err = CURLE_AGAIN;
    أرجع 0;
  }

  إذا(!Curl_bufq_peek(recvbufq, &capsule_type, &read_size) || !read_size) {
    *err = CURLE_AGAIN;
    أرجع 0;
  }

  إذا(capsule_type[0]) {
    infof(data, "Error! Invalid capsule type: %d", capsule_type[0]);
    Curl_bufq_skip(recvbufq, 1);
    *err = CURLE_RECV_ERROR;
    أرجع 0;
  }

  offset = 1;
  result = capsule_decode_varint_at(recvbufq, offset, &capsule_length,
                                    &varint_len);
  إذا(result == CURLE_AGAIN) {
    *err = CURLE_AGAIN;
    أرجع 0;
  }
  وإلا إذا(result) {
    *err = CURLE_RECV_ERROR;
    أرجع 0;
  }
  offset += varint_len;

  إذا(!Curl_bufq_peek_at(recvbufq, offset, &context_id, &read_size) ||
     !read_size) {
    *err = CURLE_AGAIN;
    أرجع 0;
  }

  إذا(*context_id) {
    infof(data, "Error! Invalid context ID: %02x", *context_id);
    Curl_bufq_skip(recvbufq, offset + 1);
    *err = CURLE_RECV_ERROR;
    أرجع 0;
  }
  offset += 1;

  إذا(!capsule_length) {
    infof(data, "Error! Invalid capsule length: 0");
    Curl_bufq_skip(recvbufq, offset);
    *err = CURLE_RECV_ERROR;
    أرجع 0;
  }
  إذا(capsule_length - 1 > (uint64_t)(SIZE_MAX - offset)) {
    infof(data, "Error! Capsule length too large: %" CURL_FORMAT_CURL_OFF_T,
          (curl_off_t)capsule_length);
    *err = CURLE_RECV_ERROR;
    أرجع 0;
  }
  payload_len = (size_t)(capsule_length - 1);

  إذا(payload_len > len) {
    infof(data, "UDP payload does not fit destination buffer: %zu > %zu",
          payload_len, len);
    Curl_bufq_skip(recvbufq, offset + payload_len);
    *err = CURLE_RECV_ERROR;
    أرجع 0;
  }

  إذا(Curl_bufq_len(recvbufq) < offset + payload_len) {
    إذا(Curl_bufq_is_full(recvbufq)) {
      infof(data, "Error! Capsule does not fit the receive buffer: %zu",
            payload_len);
      *err = CURLE_RECV_ERROR;
      أرجع 0;
    }
    *err = CURLE_AGAIN;
    أرجع 0;
  }

  Curl_bufq_skip(recvbufq, offset);
  إذا(!payload_len) {
    *err = CURLE_OK;
    أرجع 0;
  }
  result = Curl_bufq_read(recvbufq, buf, payload_len, &bytes_read);
  إذا(result || (bytes_read != payload_len)) {
    infof(data, "Error! Read less than expected %zu %zu",
          payload_len, bytes_read);
    *err = CURLE_RECV_ERROR;
    أرجع 0;
  }

  إذا(cf && data) {
    CURL_TRC_CF(data, cf, "Processed UDP capsule raw: size=%zu "
                "length_left %zu", payload_len, Curl_bufq_len(recvbufq));
  }
  *err = CURLE_OK;
  أرجع bytes_read;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
