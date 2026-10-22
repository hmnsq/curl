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

#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>  /* for htons() */
#endif

#അന്തര്ഭാവ <curl/curl.h>
#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "curlx/dynbuf.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "bufq.h"
#അന്തര്ഭാവ "vquic/capsule.h"


/**
 * Convert 64-bit value from network byte order to host byte order
 */
സ്ഥിതം uint64_t capsule_ntohll(uint64_t മൂല്യം)
{
#എങ്കിൽ defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
  തിരികെ മൂല്യം;
#അല്ലെങ്കിലെങ്കിൽ (defined(__GNUC__) || defined(__clang__)) && \
  defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  തിരികെ __builtin_bswap64(മൂല്യം);
#അല്ലെങ്കിൽ
  സംഘം {
    uint64_t u64;
    uint32_t u32[2];
  } src, dst;

  src.u64 = മൂല്യം;
  dst.u32[0] = ntohl(src.u32[1]);
  dst.u32[1] = ntohl(src.u32[0]);
  തിരികെ dst.u64;
#endif
}

/**
 * Encode a variable-length integer into a plain buffer.
 * @param buf   Output buffer (must have at least 8 bytes)
 * @param value Value to encode (must be <= 0x3FFFFFFFFFFFFFFF)
 * @return Number of bytes written
 */
സ്ഥിതം size_t capsule_encode_varint_buf(uint8_t *buf, uint64_t മൂല്യം)
{
  DEBUGASSERT(മൂല്യം <= 0x3FFFFFFFFFFFFFFF);

  എങ്കിൽ(മൂല്യം <= 0x3F) {
    buf[0] = (uint8_t)മൂല്യം;
    തിരികെ 1;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(മൂല്യം <= 0x3FFF) {
    uint16_t encoded = (uint16_t)മൂല്യം & 0x3FFF;
    encoded = ntohs(encoded | 0x4000);
    memcpy(buf, &encoded, 2);
    തിരികെ 2;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(മൂല്യം <= 0x3FFFFFFF) {
    uint32_t encoded = (uint32_t)മൂല്യം & 0x3FFFFFFF;
    encoded = ntohl(encoded | 0x80000000);
    memcpy(buf, &encoded, 4);
    തിരികെ 4;
  }
  അല്ലെങ്കിൽ {
    uint64_t encoded = (uint64_t)മൂല്യം & 0x3FFFFFFFFFFFFFFF;
    encoded = capsule_ntohll(encoded | 0xC000000000000000);
    memcpy(buf, &encoded, 8);
    തിരികെ 8;
  }
}

സ്ഥിതം CURLcode capsule_peek_u8(ഘടന bufq *recvbufq,
                                size_t offset,
                                uint8_t *pbyte)
{
  സ്ഥിരം unsigned അക്ഷരം *peek = ശൂന്യ;
  size_t peeklen = 0;

  എങ്കിൽ(!Curl_bufq_peek_at(recvbufq, offset, &peek, &peeklen) || !peeklen)
    തിരികെ CURLE_AGAIN;
  *pbyte = peek[0];
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode capsule_decode_varint_at(ഘടന bufq *recvbufq,
                                         size_t offset,
                                         uint64_t *pvalue,
                                         size_t *pconsumed)
{
  uint8_t first_byte, byte;
  uint64_t മൂല്യം;
  size_t nbytes;
  size_t i;
  CURLcode result;

  result = capsule_peek_u8(recvbufq, offset, &first_byte);
  എങ്കിൽ(result)
    തിരികെ result;

  nbytes = (size_t)1 << (first_byte >> 6); /* 1, 2, 4 or 8 bytes */
  മൂല്യം = first_byte & 0x3F;

  വേണ്ടി(i = 1; i < nbytes; ++i) {
    result = capsule_peek_u8(recvbufq, offset + i, &byte);
    എങ്കിൽ(result)
      തിരികെ result;
    മൂല്യം = (മൂല്യം << 8) | byte;
  }

  *pvalue = മൂല്യം;
  *pconsumed = nbytes;
  തിരികെ CURLE_OK;
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
  എങ്കിൽ(hdrlen < HTTP_CAPSULE_HEADER_MAX_SIZE)
    തിരികെ 0;
  hdr[off++] = 0; /* capsule type: HTTP Datagram */
  off += capsule_encode_varint_buf(hdr + off, (uint64_t)payload_len + 1);
  hdr[off++] = 0; /* context ID */
  തിരികെ off;
}

CURLcode Curl_capsule_encap_udp_datagram(ഘടന bufq *q,
                                         സ്ഥിരം ശൂന്യം *buf, size_t blen)
{
  CURLcode result;
  uint8_t hdr[HTTP_CAPSULE_HEADER_MAX_SIZE];
  size_t hdr_len, nwritten;

  hdr_len = capsule_encap_udp_hdr(hdr, വലുപ്പം(hdr), blen);
  DEBUGASSERT(hdr_len);
  എങ്കിൽ(!hdr_len)
    തിരികെ CURLE_FAILED_INIT;

  result = Curl_bufq_write(q, hdr, hdr_len, &nwritten);
  എങ്കിൽ(!result && (nwritten != hdr_len))
    തിരികെ CURLE_WRITE_ERROR;
  എങ്കിൽ(!result) {
    result = Curl_bufq_write(q, buf, blen, &nwritten);
    എങ്കിൽ(!result && (nwritten != blen))
      തിരികെ CURLE_WRITE_ERROR;
  }
  എങ്കിൽ(result == CURLE_AGAIN)
    തിരികെ CURLE_WRITE_ERROR;
  തിരികെ result;
}

size_t Curl_capsule_process_udp_raw(ഘടന Curl_cfilter *cf,
                                    ഘടന Curl_easy *data,
                                    ഘടന bufq *recvbufq,
                                    unsigned അക്ഷരം *buf, size_t len,
                                    CURLcode *err)
{
  സ്ഥിരം unsigned അക്ഷരം *context_id, *capsule_type;
  size_t read_size, varint_len;
  uint64_t capsule_length;
  size_t offset, payload_len;
  size_t bytes_read = 0;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(!len) {
    *err = CURLE_BAD_FUNCTION_ARGUMENT;
    തിരികെ 0;
  }

  എങ്കിൽ(Curl_bufq_is_empty(recvbufq)) {
    *err = CURLE_AGAIN;
    തിരികെ 0;
  }

  എങ്കിൽ(!Curl_bufq_peek(recvbufq, &capsule_type, &read_size) || !read_size) {
    *err = CURLE_AGAIN;
    തിരികെ 0;
  }

  എങ്കിൽ(capsule_type[0]) {
    infof(data, "Error! Invalid capsule type: %d", capsule_type[0]);
    Curl_bufq_skip(recvbufq, 1);
    *err = CURLE_RECV_ERROR;
    തിരികെ 0;
  }

  offset = 1;
  result = capsule_decode_varint_at(recvbufq, offset, &capsule_length,
                                    &varint_len);
  എങ്കിൽ(result == CURLE_AGAIN) {
    *err = CURLE_AGAIN;
    തിരികെ 0;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(result) {
    *err = CURLE_RECV_ERROR;
    തിരികെ 0;
  }
  offset += varint_len;

  എങ്കിൽ(!Curl_bufq_peek_at(recvbufq, offset, &context_id, &read_size) ||
     !read_size) {
    *err = CURLE_AGAIN;
    തിരികെ 0;
  }

  എങ്കിൽ(*context_id) {
    infof(data, "Error! Invalid context ID: %02x", *context_id);
    Curl_bufq_skip(recvbufq, offset + 1);
    *err = CURLE_RECV_ERROR;
    തിരികെ 0;
  }
  offset += 1;

  എങ്കിൽ(!capsule_length) {
    infof(data, "Error! Invalid capsule length: 0");
    Curl_bufq_skip(recvbufq, offset);
    *err = CURLE_RECV_ERROR;
    തിരികെ 0;
  }
  എങ്കിൽ(capsule_length - 1 > (uint64_t)(SIZE_MAX - offset)) {
    infof(data, "Error! Capsule length too large: %" CURL_FORMAT_CURL_OFF_T,
          (curl_off_t)capsule_length);
    *err = CURLE_RECV_ERROR;
    തിരികെ 0;
  }
  payload_len = (size_t)(capsule_length - 1);

  എങ്കിൽ(payload_len > len) {
    infof(data, "UDP payload does not fit destination buffer: %zu > %zu",
          payload_len, len);
    Curl_bufq_skip(recvbufq, offset + payload_len);
    *err = CURLE_RECV_ERROR;
    തിരികെ 0;
  }

  എങ്കിൽ(Curl_bufq_len(recvbufq) < offset + payload_len) {
    എങ്കിൽ(Curl_bufq_is_full(recvbufq)) {
      infof(data, "Error! Capsule does not fit the receive buffer: %zu",
            payload_len);
      *err = CURLE_RECV_ERROR;
      തിരികെ 0;
    }
    *err = CURLE_AGAIN;
    തിരികെ 0;
  }

  Curl_bufq_skip(recvbufq, offset);
  എങ്കിൽ(!payload_len) {
    *err = CURLE_OK;
    തിരികെ 0;
  }
  result = Curl_bufq_read(recvbufq, buf, payload_len, &bytes_read);
  എങ്കിൽ(result || (bytes_read != payload_len)) {
    infof(data, "Error! Read less than expected %zu %zu",
          payload_len, bytes_read);
    *err = CURLE_RECV_ERROR;
    തിരികെ 0;
  }

  എങ്കിൽ(cf && data) {
    CURL_TRC_CF(data, cf, "Processed UDP capsule raw: size=%zu "
                "length_left %zu", payload_len, Curl_bufq_len(recvbufq));
  }
  *err = CURLE_OK;
  തിരികെ bytes_read;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
