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

#include "curl_setup.h"

#အကယ်၍ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>  /* for htons() */
#endif

#include <curl/curl.h>
#include "urldata.h"
#include "curlx/dynbuf.h"
#include "cfilters.h"
#include "curl_trc.h"
#include "bufq.h"
#include "vquic/capsule.h"


/**
 * Convert 64-bit value from network byte order to host byte order
 */
static uint64_t capsule_ntohll(uint64_t value)
{
#အကယ်၍ defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
  ပြန်ပေး value;
#elif (defined(__GNUC__) || defined(__clang__)) && \
  defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  ပြန်ပေး __builtin_bswap64(value);
#မဟုတ်ပါက
  sa_ngha {
    uint64_t u64;
    uint32_t u32[2];
  } src, dst;

  src.u64 = value;
  dst.u32[0] = ntohl(src.u32[1]);
  dst.u32[1] = ntohl(src.u32[0]);
  ပြန်ပေး dst.u64;
#endif
}

/**
 * Encode a variable-length integer into a plain buffer.
 * @param buf   Output buffer (must have at least 8 bytes)
 * @param value Value to encode (must be <= 0x3FFFFFFFFFFFFFFF)
 * @return Number of bytes written
 */
static size_t capsule_encode_varint_buf(uint8_t *buf, uint64_t value)
{
  DEBUGASSERT(value <= 0x3FFFFFFFFFFFFFFF);

  အကယ်၍(value <= 0x3F) {
    buf[0] = (uint8_t)value;
    ပြန်ပေး 1;
  }
  မဟုတ်ပါက အကယ်၍(value <= 0x3FFF) {
    uint16_t encoded = (uint16_t)value & 0x3FFF;
    encoded = ntohs(encoded | 0x4000);
    memcpy(buf, &encoded, 2);
    ပြန်ပေး 2;
  }
  မဟုတ်ပါက အကယ်၍(value <= 0x3FFFFFFF) {
    uint32_t encoded = (uint32_t)value & 0x3FFFFFFF;
    encoded = ntohl(encoded | 0x80000000);
    memcpy(buf, &encoded, 4);
    ပြန်ပေး 4;
  }
  မဟုတ်ပါက {
    uint64_t encoded = (uint64_t)value & 0x3FFFFFFFFFFFFFFF;
    encoded = capsule_ntohll(encoded | 0xC000000000000000);
    memcpy(buf, &encoded, 8);
    ပြန်ပေး 8;
  }
}

static CURLcode capsule_peek_u8(sa_nracanaa bufq *recvbufq,
                                size_t offset,
                                uint8_t *pbyte)
{
  const unsigned var_na *peek = NULL;
  size_t peeklen = 0;

  အကယ်၍(!Curl_bufq_peek_at(recvbufq, offset, &peek, &peeklen) || !peeklen)
    ပြန်ပေး CURLE_AGAIN;
  *pbyte = peek[0];
  ပြန်ပေး CURLE_OK;
}

static CURLcode capsule_decode_varint_at(sa_nracanaa bufq *recvbufq,
                                         size_t offset,
                                         uint64_t *pvalue,
                                         size_t *pconsumed)
{
  uint8_t first_byte, byte;
  uint64_t value;
  size_t nbytes;
  size_t i;
  CURLcode result;

  result = capsule_peek_u8(recvbufq, offset, &first_byte);
  အကယ်၍(result)
    ပြန်ပေး result;

  nbytes = (size_t)1 << (first_byte >> 6); /* 1, 2, 4 or 8 bytes */
  value = first_byte & 0x3F;

  အတွက်(i = 1; i < nbytes; ++i) {
    result = capsule_peek_u8(recvbufq, offset + i, &byte);
    အကယ်၍(result)
      ပြန်ပေး result;
    value = (value << 8) | byte;
  }

  *pvalue = value;
  *pconsumed = nbytes;
  ပြန်ပေး CURLE_OK;
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
  အကယ်၍(hdrlen < HTTP_CAPSULE_HEADER_MAX_SIZE)
    ပြန်ပေး 0;
  hdr[off++] = 0; /* capsule type: HTTP Datagram */
  off += capsule_encode_varint_buf(hdr + off, (uint64_t)payload_len + 1);
  hdr[off++] = 0; /* context ID */
  ပြန်ပေး off;
}

CURLcode Curl_capsule_encap_udp_datagram(sa_nracanaa bufq *q,
                                         const void *buf, size_t blen)
{
  CURLcode result;
  uint8_t hdr[HTTP_CAPSULE_HEADER_MAX_SIZE];
  size_t hdr_len, nwritten;

  hdr_len = capsule_encap_udp_hdr(hdr, parimaa_na(hdr), blen);
  DEBUGASSERT(hdr_len);
  အကယ်၍(!hdr_len)
    ပြန်ပေး CURLE_FAILED_INIT;

  result = Curl_bufq_write(q, hdr, hdr_len, &nwritten);
  အကယ်၍(!result && (nwritten != hdr_len))
    ပြန်ပေး CURLE_WRITE_ERROR;
  အကယ်၍(!result) {
    result = Curl_bufq_write(q, buf, blen, &nwritten);
    အကယ်၍(!result && (nwritten != blen))
      ပြန်ပေး CURLE_WRITE_ERROR;
  }
  အကယ်၍(result == CURLE_AGAIN)
    ပြန်ပေး CURLE_WRITE_ERROR;
  ပြန်ပေး result;
}

size_t Curl_capsule_process_udp_raw(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data,
                                    sa_nracanaa bufq *recvbufq,
                                    unsigned var_na *buf, size_t len,
                                    CURLcode *err)
{
  const unsigned var_na *context_id, *capsule_type;
  size_t read_size, varint_len;
  uint64_t capsule_length;
  size_t offset, payload_len;
  size_t bytes_read = 0;
  CURLcode result = CURLE_OK;

  အကယ်၍(!len) {
    *err = CURLE_BAD_FUNCTION_ARGUMENT;
    ပြန်ပေး 0;
  }

  အကယ်၍(Curl_bufq_is_empty(recvbufq)) {
    *err = CURLE_AGAIN;
    ပြန်ပေး 0;
  }

  အကယ်၍(!Curl_bufq_peek(recvbufq, &capsule_type, &read_size) || !read_size) {
    *err = CURLE_AGAIN;
    ပြန်ပေး 0;
  }

  အကယ်၍(capsule_type[0]) {
    infof(data, "Error! Invalid capsule type: %d", capsule_type[0]);
    Curl_bufq_skip(recvbufq, 1);
    *err = CURLE_RECV_ERROR;
    ပြန်ပေး 0;
  }

  offset = 1;
  result = capsule_decode_varint_at(recvbufq, offset, &capsule_length,
                                    &varint_len);
  အကယ်၍(result == CURLE_AGAIN) {
    *err = CURLE_AGAIN;
    ပြန်ပေး 0;
  }
  မဟုတ်ပါက အကယ်၍(result) {
    *err = CURLE_RECV_ERROR;
    ပြန်ပေး 0;
  }
  offset += varint_len;

  အကယ်၍(!Curl_bufq_peek_at(recvbufq, offset, &context_id, &read_size) ||
     !read_size) {
    *err = CURLE_AGAIN;
    ပြန်ပေး 0;
  }

  အကယ်၍(*context_id) {
    infof(data, "Error! Invalid context ID: %02x", *context_id);
    Curl_bufq_skip(recvbufq, offset + 1);
    *err = CURLE_RECV_ERROR;
    ပြန်ပေး 0;
  }
  offset += 1;

  အကယ်၍(!capsule_length) {
    infof(data, "Error! Invalid capsule length: 0");
    Curl_bufq_skip(recvbufq, offset);
    *err = CURLE_RECV_ERROR;
    ပြန်ပေး 0;
  }
  အကယ်၍(capsule_length - 1 > (uint64_t)(SIZE_MAX - offset)) {
    infof(data, "Error! Capsule length too large: %" CURL_FORMAT_CURL_OFF_T,
          (curl_off_t)capsule_length);
    *err = CURLE_RECV_ERROR;
    ပြန်ပေး 0;
  }
  payload_len = (size_t)(capsule_length - 1);

  အကယ်၍(payload_len > len) {
    infof(data, "UDP payload does not fit destination buffer: %zu > %zu",
          payload_len, len);
    Curl_bufq_skip(recvbufq, offset + payload_len);
    *err = CURLE_RECV_ERROR;
    ပြန်ပေး 0;
  }

  အကယ်၍(Curl_bufq_len(recvbufq) < offset + payload_len) {
    အကယ်၍(Curl_bufq_is_full(recvbufq)) {
      infof(data, "Error! Capsule does not fit the receive buffer: %zu",
            payload_len);
      *err = CURLE_RECV_ERROR;
      ပြန်ပေး 0;
    }
    *err = CURLE_AGAIN;
    ပြန်ပေး 0;
  }

  Curl_bufq_skip(recvbufq, offset);
  အကယ်၍(!payload_len) {
    *err = CURLE_OK;
    ပြန်ပေး 0;
  }
  result = Curl_bufq_read(recvbufq, buf, payload_len, &bytes_read);
  အကယ်၍(result || (bytes_read != payload_len)) {
    infof(data, "Error! Read less than expected %zu %zu",
          payload_len, bytes_read);
    *err = CURLE_RECV_ERROR;
    ပြန်ပေး 0;
  }

  အကယ်၍(cf && data) {
    CURL_TRC_CF(data, cf, "Processed UDP capsule raw: size=%zu "
                "length_left %zu", payload_len, Curl_bufq_len(recvbufq));
  }
  *err = CURLE_OK;
  ပြန်ပေး bytes_read;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
