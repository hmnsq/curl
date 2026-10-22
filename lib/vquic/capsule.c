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

#அந்தர்பாவ "curl_setup.h"

#எனில் !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>  /* for htons() */
#endif

#அந்தர்பாவ <curl/curl.h>
#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "curlx/dynbuf.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "bufq.h"
#அந்தர்பாவ "vquic/capsule.h"


/**
 * Convert 64-bit value from network byte order to host byte order
 */
நிலையான uint64_t capsule_ntohll(uint64_t மதிப்பு)
{
#எனில் defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
  திரும்பு மதிப்பு;
#அல்லதெனில் (defined(__GNUC__) || defined(__clang__)) && \
  defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  திரும்பு __builtin_bswap64(மதிப்பு);
#இல்லையெனில்
  ஒன்றிப்பு {
    uint64_t u64;
    uint32_t u32[2];
  } src, dst;

  src.u64 = மதிப்பு;
  dst.u32[0] = ntohl(src.u32[1]);
  dst.u32[1] = ntohl(src.u32[0]);
  திரும்பு dst.u64;
#endif
}

/**
 * Encode a variable-length integer into a plain buffer.
 * @param buf   Output buffer (must have at least 8 bytes)
 * @param value Value to encode (must be <= 0x3FFFFFFFFFFFFFFF)
 * @return Number of bytes written
 */
நிலையான size_t capsule_encode_varint_buf(uint8_t *buf, uint64_t மதிப்பு)
{
  DEBUGASSERT(மதிப்பு <= 0x3FFFFFFFFFFFFFFF);

  எனில்(மதிப்பு <= 0x3F) {
    buf[0] = (uint8_t)மதிப்பு;
    திரும்பு 1;
  }
  இல்லையெனில் எனில்(மதிப்பு <= 0x3FFF) {
    uint16_t encoded = (uint16_t)மதிப்பு & 0x3FFF;
    encoded = ntohs(encoded | 0x4000);
    memcpy(buf, &encoded, 2);
    திரும்பு 2;
  }
  இல்லையெனில் எனில்(மதிப்பு <= 0x3FFFFFFF) {
    uint32_t encoded = (uint32_t)மதிப்பு & 0x3FFFFFFF;
    encoded = ntohl(encoded | 0x80000000);
    memcpy(buf, &encoded, 4);
    திரும்பு 4;
  }
  இல்லையெனில் {
    uint64_t encoded = (uint64_t)மதிப்பு & 0x3FFFFFFFFFFFFFFF;
    encoded = capsule_ntohll(encoded | 0xC000000000000000);
    memcpy(buf, &encoded, 8);
    திரும்பு 8;
  }
}

நிலையான CURLcode capsule_peek_u8(கட்டமைப்பு bufq *recvbufq,
                                size_t offset,
                                uint8_t *pbyte)
{
  மாறிலி unsigned எழுத்து *peek = வெற்று;
  size_t peeklen = 0;

  எனில்(!Curl_bufq_peek_at(recvbufq, offset, &peek, &peeklen) || !peeklen)
    திரும்பு CURLE_AGAIN;
  *pbyte = peek[0];
  திரும்பு CURLE_OK;
}

நிலையான CURLcode capsule_decode_varint_at(கட்டமைப்பு bufq *recvbufq,
                                         size_t offset,
                                         uint64_t *pvalue,
                                         size_t *pconsumed)
{
  uint8_t first_byte, byte;
  uint64_t மதிப்பு;
  size_t nbytes;
  size_t i;
  CURLcode result;

  result = capsule_peek_u8(recvbufq, offset, &first_byte);
  எனில்(result)
    திரும்பு result;

  nbytes = (size_t)1 << (first_byte >> 6); /* 1, 2, 4 or 8 bytes */
  மதிப்பு = first_byte & 0x3F;

  க்கு(i = 1; i < nbytes; ++i) {
    result = capsule_peek_u8(recvbufq, offset + i, &byte);
    எனில்(result)
      திரும்பு result;
    மதிப்பு = (மதிப்பு << 8) | byte;
  }

  *pvalue = மதிப்பு;
  *pconsumed = nbytes;
  திரும்பு CURLE_OK;
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
  எனில்(hdrlen < HTTP_CAPSULE_HEADER_MAX_SIZE)
    திரும்பு 0;
  hdr[off++] = 0; /* capsule type: HTTP Datagram */
  off += capsule_encode_varint_buf(hdr + off, (uint64_t)payload_len + 1);
  hdr[off++] = 0; /* context ID */
  திரும்பு off;
}

CURLcode Curl_capsule_encap_udp_datagram(கட்டமைப்பு bufq *q,
                                         மாறிலி வெறுமை *buf, size_t blen)
{
  CURLcode result;
  uint8_t hdr[HTTP_CAPSULE_HEADER_MAX_SIZE];
  size_t hdr_len, nwritten;

  hdr_len = capsule_encap_udp_hdr(hdr, அளவு(hdr), blen);
  DEBUGASSERT(hdr_len);
  எனில்(!hdr_len)
    திரும்பு CURLE_FAILED_INIT;

  result = Curl_bufq_write(q, hdr, hdr_len, &nwritten);
  எனில்(!result && (nwritten != hdr_len))
    திரும்பு CURLE_WRITE_ERROR;
  எனில்(!result) {
    result = Curl_bufq_write(q, buf, blen, &nwritten);
    எனில்(!result && (nwritten != blen))
      திரும்பு CURLE_WRITE_ERROR;
  }
  எனில்(result == CURLE_AGAIN)
    திரும்பு CURLE_WRITE_ERROR;
  திரும்பு result;
}

size_t Curl_capsule_process_udp_raw(கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு bufq *recvbufq,
                                    unsigned எழுத்து *buf, size_t len,
                                    CURLcode *err)
{
  மாறிலி unsigned எழுத்து *context_id, *capsule_type;
  size_t read_size, varint_len;
  uint64_t capsule_length;
  size_t offset, payload_len;
  size_t bytes_read = 0;
  CURLcode result = CURLE_OK;

  எனில்(!len) {
    *err = CURLE_BAD_FUNCTION_ARGUMENT;
    திரும்பு 0;
  }

  எனில்(Curl_bufq_is_empty(recvbufq)) {
    *err = CURLE_AGAIN;
    திரும்பு 0;
  }

  எனில்(!Curl_bufq_peek(recvbufq, &capsule_type, &read_size) || !read_size) {
    *err = CURLE_AGAIN;
    திரும்பு 0;
  }

  எனில்(capsule_type[0]) {
    infof(data, "Error! Invalid capsule type: %d", capsule_type[0]);
    Curl_bufq_skip(recvbufq, 1);
    *err = CURLE_RECV_ERROR;
    திரும்பு 0;
  }

  offset = 1;
  result = capsule_decode_varint_at(recvbufq, offset, &capsule_length,
                                    &varint_len);
  எனில்(result == CURLE_AGAIN) {
    *err = CURLE_AGAIN;
    திரும்பு 0;
  }
  இல்லையெனில் எனில்(result) {
    *err = CURLE_RECV_ERROR;
    திரும்பு 0;
  }
  offset += varint_len;

  எனில்(!Curl_bufq_peek_at(recvbufq, offset, &context_id, &read_size) ||
     !read_size) {
    *err = CURLE_AGAIN;
    திரும்பு 0;
  }

  எனில்(*context_id) {
    infof(data, "Error! Invalid context ID: %02x", *context_id);
    Curl_bufq_skip(recvbufq, offset + 1);
    *err = CURLE_RECV_ERROR;
    திரும்பு 0;
  }
  offset += 1;

  எனில்(!capsule_length) {
    infof(data, "Error! Invalid capsule length: 0");
    Curl_bufq_skip(recvbufq, offset);
    *err = CURLE_RECV_ERROR;
    திரும்பு 0;
  }
  எனில்(capsule_length - 1 > (uint64_t)(SIZE_MAX - offset)) {
    infof(data, "Error! Capsule length too large: %" CURL_FORMAT_CURL_OFF_T,
          (curl_off_t)capsule_length);
    *err = CURLE_RECV_ERROR;
    திரும்பு 0;
  }
  payload_len = (size_t)(capsule_length - 1);

  எனில்(payload_len > len) {
    infof(data, "UDP payload does not fit destination buffer: %zu > %zu",
          payload_len, len);
    Curl_bufq_skip(recvbufq, offset + payload_len);
    *err = CURLE_RECV_ERROR;
    திரும்பு 0;
  }

  எனில்(Curl_bufq_len(recvbufq) < offset + payload_len) {
    எனில்(Curl_bufq_is_full(recvbufq)) {
      infof(data, "Error! Capsule does not fit the receive buffer: %zu",
            payload_len);
      *err = CURLE_RECV_ERROR;
      திரும்பு 0;
    }
    *err = CURLE_AGAIN;
    திரும்பு 0;
  }

  Curl_bufq_skip(recvbufq, offset);
  எனில்(!payload_len) {
    *err = CURLE_OK;
    திரும்பு 0;
  }
  result = Curl_bufq_read(recvbufq, buf, payload_len, &bytes_read);
  எனில்(result || (bytes_read != payload_len)) {
    infof(data, "Error! Read less than expected %zu %zu",
          payload_len, bytes_read);
    *err = CURLE_RECV_ERROR;
    திரும்பு 0;
  }

  எனில்(cf && data) {
    CURL_TRC_CF(data, cf, "Processed UDP capsule raw: size=%zu "
                "length_left %zu", payload_len, Curl_bufq_len(recvbufq));
  }
  *err = CURLE_OK;
  திரும்பு bytes_read;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
