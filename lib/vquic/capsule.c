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

#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>  /* for htons() */
#endif

#ଅନ୍ତର୍ଭାଵ <curl/curl.h>
#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "curlx/dynbuf.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "bufq.h"
#ଅନ୍ତର୍ଭାଵ "vquic/capsule.h"


/**
 * Convert 64-bit value from network byte order to host byte order
 */
ସ୍ଥିର uint64_t capsule_ntohll(uint64_t ମୂଲ୍ୟ)
{
#ଯଦି defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
  ଫେରନ୍ତୁ ମୂଲ୍ୟ;
#ନଚେତଯଦି (defined(__GNUC__) || defined(__clang__)) && \
  defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  ଫେରନ୍ତୁ __builtin_bswap64(ମୂଲ୍ୟ);
#ଅନ୍ୟଥା
  ସଂଘ {
    uint64_t u64;
    uint32_t u32[2];
  } src, dst;

  src.u64 = ମୂଲ୍ୟ;
  dst.u32[0] = ntohl(src.u32[1]);
  dst.u32[1] = ntohl(src.u32[0]);
  ଫେରନ୍ତୁ dst.u64;
#endif
}

/**
 * Encode a variable-length integer into a plain buffer.
 * @param buf   Output buffer (must have at least 8 bytes)
 * @param value Value to encode (must be <= 0x3FFFFFFFFFFFFFFF)
 * @return Number of bytes written
 */
ସ୍ଥିର size_t capsule_encode_varint_buf(uint8_t *buf, uint64_t ମୂଲ୍ୟ)
{
  DEBUGASSERT(ମୂଲ୍ୟ <= 0x3FFFFFFFFFFFFFFF);

  ଯଦି(ମୂଲ୍ୟ <= 0x3F) {
    buf[0] = (uint8_t)ମୂଲ୍ୟ;
    ଫେରନ୍ତୁ 1;
  }
  ଅନ୍ୟଥା ଯଦି(ମୂଲ୍ୟ <= 0x3FFF) {
    uint16_t encoded = (uint16_t)ମୂଲ୍ୟ & 0x3FFF;
    encoded = ntohs(encoded | 0x4000);
    memcpy(buf, &encoded, 2);
    ଫେରନ୍ତୁ 2;
  }
  ଅନ୍ୟଥା ଯଦି(ମୂଲ୍ୟ <= 0x3FFFFFFF) {
    uint32_t encoded = (uint32_t)ମୂଲ୍ୟ & 0x3FFFFFFF;
    encoded = ntohl(encoded | 0x80000000);
    memcpy(buf, &encoded, 4);
    ଫେରନ୍ତୁ 4;
  }
  ଅନ୍ୟଥା {
    uint64_t encoded = (uint64_t)ମୂଲ୍ୟ & 0x3FFFFFFFFFFFFFFF;
    encoded = capsule_ntohll(encoded | 0xC000000000000000);
    memcpy(buf, &encoded, 8);
    ଫେରନ୍ତୁ 8;
  }
}

ସ୍ଥିର CURLcode capsule_peek_u8(ସଂରଚନା bufq *recvbufq,
                                size_t offset,
                                uint8_t *pbyte)
{
  ଅଚଳ unsigned ଅକ୍ଷର *peek = ଖାଲି;
  size_t peeklen = 0;

  ଯଦି(!Curl_bufq_peek_at(recvbufq, offset, &peek, &peeklen) || !peeklen)
    ଫେରନ୍ତୁ CURLE_AGAIN;
  *pbyte = peek[0];
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode capsule_decode_varint_at(ସଂରଚନା bufq *recvbufq,
                                         size_t offset,
                                         uint64_t *pvalue,
                                         size_t *pconsumed)
{
  uint8_t first_byte, byte;
  uint64_t ମୂଲ୍ୟ;
  size_t nbytes;
  size_t i;
  CURLcode result;

  result = capsule_peek_u8(recvbufq, offset, &first_byte);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  nbytes = (size_t)1 << (first_byte >> 6); /* 1, 2, 4 or 8 bytes */
  ମୂଲ୍ୟ = first_byte & 0x3F;

  ପାଇଁ(i = 1; i < nbytes; ++i) {
    result = capsule_peek_u8(recvbufq, offset + i, &byte);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    ମୂଲ୍ୟ = (ମୂଲ୍ୟ << 8) | byte;
  }

  *pvalue = ମୂଲ୍ୟ;
  *pconsumed = nbytes;
  ଫେରନ୍ତୁ CURLE_OK;
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
  ଯଦି(hdrlen < HTTP_CAPSULE_HEADER_MAX_SIZE)
    ଫେରନ୍ତୁ 0;
  hdr[off++] = 0; /* capsule type: HTTP Datagram */
  off += capsule_encode_varint_buf(hdr + off, (uint64_t)payload_len + 1);
  hdr[off++] = 0; /* context ID */
  ଫେରନ୍ତୁ off;
}

CURLcode Curl_capsule_encap_udp_datagram(ସଂରଚନା bufq *q,
                                         ଅଚଳ ଶୂନ୍ୟ *buf, size_t blen)
{
  CURLcode result;
  uint8_t hdr[HTTP_CAPSULE_HEADER_MAX_SIZE];
  size_t hdr_len, nwritten;

  hdr_len = capsule_encap_udp_hdr(hdr, ଆକାର(hdr), blen);
  DEBUGASSERT(hdr_len);
  ଯଦି(!hdr_len)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  result = Curl_bufq_write(q, hdr, hdr_len, &nwritten);
  ଯଦି(!result && (nwritten != hdr_len))
    ଫେରନ୍ତୁ CURLE_WRITE_ERROR;
  ଯଦି(!result) {
    result = Curl_bufq_write(q, buf, blen, &nwritten);
    ଯଦି(!result && (nwritten != blen))
      ଫେରନ୍ତୁ CURLE_WRITE_ERROR;
  }
  ଯଦି(result == CURLE_AGAIN)
    ଫେରନ୍ତୁ CURLE_WRITE_ERROR;
  ଫେରନ୍ତୁ result;
}

size_t Curl_capsule_process_udp_raw(ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା bufq *recvbufq,
                                    unsigned ଅକ୍ଷର *buf, size_t len,
                                    CURLcode *err)
{
  ଅଚଳ unsigned ଅକ୍ଷର *context_id, *capsule_type;
  size_t read_size, varint_len;
  uint64_t capsule_length;
  size_t offset, payload_len;
  size_t bytes_read = 0;
  CURLcode result = CURLE_OK;

  ଯଦି(!len) {
    *err = CURLE_BAD_FUNCTION_ARGUMENT;
    ଫେରନ୍ତୁ 0;
  }

  ଯଦି(Curl_bufq_is_empty(recvbufq)) {
    *err = CURLE_AGAIN;
    ଫେରନ୍ତୁ 0;
  }

  ଯଦି(!Curl_bufq_peek(recvbufq, &capsule_type, &read_size) || !read_size) {
    *err = CURLE_AGAIN;
    ଫେରନ୍ତୁ 0;
  }

  ଯଦି(capsule_type[0]) {
    infof(data, "Error! Invalid capsule type: %d", capsule_type[0]);
    Curl_bufq_skip(recvbufq, 1);
    *err = CURLE_RECV_ERROR;
    ଫେରନ୍ତୁ 0;
  }

  offset = 1;
  result = capsule_decode_varint_at(recvbufq, offset, &capsule_length,
                                    &varint_len);
  ଯଦି(result == CURLE_AGAIN) {
    *err = CURLE_AGAIN;
    ଫେରନ୍ତୁ 0;
  }
  ଅନ୍ୟଥା ଯଦି(result) {
    *err = CURLE_RECV_ERROR;
    ଫେରନ୍ତୁ 0;
  }
  offset += varint_len;

  ଯଦି(!Curl_bufq_peek_at(recvbufq, offset, &context_id, &read_size) ||
     !read_size) {
    *err = CURLE_AGAIN;
    ଫେରନ୍ତୁ 0;
  }

  ଯଦି(*context_id) {
    infof(data, "Error! Invalid context ID: %02x", *context_id);
    Curl_bufq_skip(recvbufq, offset + 1);
    *err = CURLE_RECV_ERROR;
    ଫେରନ୍ତୁ 0;
  }
  offset += 1;

  ଯଦି(!capsule_length) {
    infof(data, "Error! Invalid capsule length: 0");
    Curl_bufq_skip(recvbufq, offset);
    *err = CURLE_RECV_ERROR;
    ଫେରନ୍ତୁ 0;
  }
  ଯଦି(capsule_length - 1 > (uint64_t)(SIZE_MAX - offset)) {
    infof(data, "Error! Capsule length too large: %" CURL_FORMAT_CURL_OFF_T,
          (curl_off_t)capsule_length);
    *err = CURLE_RECV_ERROR;
    ଫେରନ୍ତୁ 0;
  }
  payload_len = (size_t)(capsule_length - 1);

  ଯଦି(payload_len > len) {
    infof(data, "UDP payload does not fit destination buffer: %zu > %zu",
          payload_len, len);
    Curl_bufq_skip(recvbufq, offset + payload_len);
    *err = CURLE_RECV_ERROR;
    ଫେରନ୍ତୁ 0;
  }

  ଯଦି(Curl_bufq_len(recvbufq) < offset + payload_len) {
    ଯଦି(Curl_bufq_is_full(recvbufq)) {
      infof(data, "Error! Capsule does not fit the receive buffer: %zu",
            payload_len);
      *err = CURLE_RECV_ERROR;
      ଫେରନ୍ତୁ 0;
    }
    *err = CURLE_AGAIN;
    ଫେରନ୍ତୁ 0;
  }

  Curl_bufq_skip(recvbufq, offset);
  ଯଦି(!payload_len) {
    *err = CURLE_OK;
    ଫେରନ୍ତୁ 0;
  }
  result = Curl_bufq_read(recvbufq, buf, payload_len, &bytes_read);
  ଯଦି(result || (bytes_read != payload_len)) {
    infof(data, "Error! Read less than expected %zu %zu",
          payload_len, bytes_read);
    *err = CURLE_RECV_ERROR;
    ଫେରନ୍ତୁ 0;
  }

  ଯଦି(cf && data) {
    CURL_TRC_CF(data, cf, "Processed UDP capsule raw: size=%zu "
                "length_left %zu", payload_len, Curl_bufq_len(recvbufq));
  }
  *err = CURLE_OK;
  ଫେରନ୍ତୁ bytes_read;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
