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

#অন্তর্ভাব "curl_setup.h"

#যদি !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>  /* for htons() */
#endif

#অন্তর্ভাব <curl/curl.h>
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curlx/dynbuf.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "bufq.h"
#অন্তর্ভাব "vquic/capsule.h"


/**
 * Convert 64-bit value from network byte order to host byte order
 */
স্থির uint64_t capsule_ntohll(uint64_t মান)
{
#যদি defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
  ফেরত মান;
#নতুবা_যদি (defined(__GNUC__) || defined(__clang__)) && \
  defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  ফেরত __builtin_bswap64(মান);
#অন্যথা
  সংঘ {
    uint64_t u64;
    uint32_t u32[2];
  } src, dst;

  src.u64 = মান;
  dst.u32[0] = ntohl(src.u32[1]);
  dst.u32[1] = ntohl(src.u32[0]);
  ফেরত dst.u64;
#endif
}

/**
 * Encode a variable-length integer into a plain buffer.
 * @param buf   Output buffer (must have at least 8 bytes)
 * @param value Value to encode (must be <= 0x3FFFFFFFFFFFFFFF)
 * @return Number of bytes written
 */
স্থির size_t capsule_encode_varint_buf(uint8_t *buf, uint64_t মান)
{
  DEBUGASSERT(মান <= 0x3FFFFFFFFFFFFFFF);

  যদি(মান <= 0x3F) {
    buf[0] = (uint8_t)মান;
    ফেরত 1;
  }
  অন্যথা যদি(মান <= 0x3FFF) {
    uint16_t encoded = (uint16_t)মান & 0x3FFF;
    encoded = ntohs(encoded | 0x4000);
    memcpy(buf, &encoded, 2);
    ফেরত 2;
  }
  অন্যথা যদি(মান <= 0x3FFFFFFF) {
    uint32_t encoded = (uint32_t)মান & 0x3FFFFFFF;
    encoded = ntohl(encoded | 0x80000000);
    memcpy(buf, &encoded, 4);
    ফেরত 4;
  }
  অন্যথা {
    uint64_t encoded = (uint64_t)মান & 0x3FFFFFFFFFFFFFFF;
    encoded = capsule_ntohll(encoded | 0xC000000000000000);
    memcpy(buf, &encoded, 8);
    ফেরত 8;
  }
}

স্থির CURLcode capsule_peek_u8(কাঠামো bufq *recvbufq,
                                size_t offset,
                                uint8_t *pbyte)
{
  ধ্রুবক unsigned অক্ষর *peek = ফাঁকা;
  size_t peeklen = 0;

  যদি(!Curl_bufq_peek_at(recvbufq, offset, &peek, &peeklen) || !peeklen)
    ফেরত CURLE_AGAIN;
  *pbyte = peek[0];
  ফেরত CURLE_OK;
}

স্থির CURLcode capsule_decode_varint_at(কাঠামো bufq *recvbufq,
                                         size_t offset,
                                         uint64_t *pvalue,
                                         size_t *pconsumed)
{
  uint8_t first_byte, byte;
  uint64_t মান;
  size_t nbytes;
  size_t i;
  CURLcode ফলাফল;

  ফলাফল = capsule_peek_u8(recvbufq, offset, &first_byte);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  nbytes = (size_t)1 << (first_byte >> 6); /* 1, 2, 4 or 8 bytes */
  মান = first_byte & 0x3F;

  জন্য(i = 1; i < nbytes; ++i) {
    ফলাফল = capsule_peek_u8(recvbufq, offset + i, &byte);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    মান = (মান << 8) | byte;
  }

  *pvalue = মান;
  *pconsumed = nbytes;
  ফেরত CURLE_OK;
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
  DEBUGASSERT(hdrlen >= HTTP_CAPSULE_HEADER_MAX_আকার);
  যদি(hdrlen < HTTP_CAPSULE_HEADER_MAX_আকার)
    ফেরত 0;
  hdr[off++] = 0; /* capsule type: HTTP Datagram */
  off += capsule_encode_varint_buf(hdr + off, (uint64_t)payload_len + 1);
  hdr[off++] = 0; /* context ID */
  ফেরত off;
}

CURLcode Curl_capsule_encap_udp_datagram(কাঠামো bufq *q,
                                         ধ্রুবক শূন্য *buf, size_t blen)
{
  CURLcode ফলাফল;
  uint8_t hdr[HTTP_CAPSULE_HEADER_MAX_আকার];
  size_t hdr_len, nwritten;

  hdr_len = capsule_encap_udp_hdr(hdr, আকার(hdr), blen);
  DEBUGASSERT(hdr_len);
  যদি(!hdr_len)
    ফেরত CURLE_FAILED_INIT;

  ফলাফল = Curl_bufq_write(q, hdr, hdr_len, &nwritten);
  যদি(!ফলাফল && (nwritten != hdr_len))
    ফেরত CURLE_WRITE_ERROR;
  যদি(!ফলাফল) {
    ফলাফল = Curl_bufq_write(q, buf, blen, &nwritten);
    যদি(!ফলাফল && (nwritten != blen))
      ফেরত CURLE_WRITE_ERROR;
  }
  যদি(ফলাফল == CURLE_AGAIN)
    ফেরত CURLE_WRITE_ERROR;
  ফেরত ফলাফল;
}

size_t Curl_capsule_process_udp_raw(কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য,
                                    কাঠামো bufq *recvbufq,
                                    unsigned অক্ষর *buf, size_t len,
                                    CURLcode *err)
{
  ধ্রুবক unsigned অক্ষর *context_id, *capsule_type;
  size_t read_আকার, varint_len;
  uint64_t capsule_দৈর্ঘ্য;
  size_t offset, payload_len;
  size_t bytes_read = 0;
  CURLcode ফলাফল = CURLE_OK;

  যদি(!len) {
    *err = CURLE_BAD_FUNCTION_ARGUMENT;
    ফেরত 0;
  }

  যদি(Curl_bufq_is_empty(recvbufq)) {
    *err = CURLE_AGAIN;
    ফেরত 0;
  }

  যদি(!Curl_bufq_peek(recvbufq, &capsule_type, &read_আকার) || !read_আকার) {
    *err = CURLE_AGAIN;
    ফেরত 0;
  }

  যদি(capsule_type[0]) {
    infof(তথ্য, "Error! Invalid capsule type: %d", capsule_type[0]);
    Curl_bufq_skip(recvbufq, 1);
    *err = CURLE_RECV_ERROR;
    ফেরত 0;
  }

  offset = 1;
  ফলাফল = capsule_decode_varint_at(recvbufq, offset, &capsule_দৈর্ঘ্য,
                                    &varint_len);
  যদি(ফলাফল == CURLE_AGAIN) {
    *err = CURLE_AGAIN;
    ফেরত 0;
  }
  অন্যথা যদি(ফলাফল) {
    *err = CURLE_RECV_ERROR;
    ফেরত 0;
  }
  offset += varint_len;

  যদি(!Curl_bufq_peek_at(recvbufq, offset, &context_id, &read_আকার) ||
     !read_আকার) {
    *err = CURLE_AGAIN;
    ফেরত 0;
  }

  যদি(*context_id) {
    infof(তথ্য, "Error! Invalid context ID: %02x", *context_id);
    Curl_bufq_skip(recvbufq, offset + 1);
    *err = CURLE_RECV_ERROR;
    ফেরত 0;
  }
  offset += 1;

  যদি(!capsule_দৈর্ঘ্য) {
    infof(তথ্য, "Error! Invalid capsule length: 0");
    Curl_bufq_skip(recvbufq, offset);
    *err = CURLE_RECV_ERROR;
    ফেরত 0;
  }
  যদি(capsule_দৈর্ঘ্য - 1 > (uint64_t)(SIZE_MAX - offset)) {
    infof(তথ্য, "Error! Capsule length too large: %" CURL_FORMAT_CURL_OFF_T,
          (curl_off_t)capsule_দৈর্ঘ্য);
    *err = CURLE_RECV_ERROR;
    ফেরত 0;
  }
  payload_len = (size_t)(capsule_দৈর্ঘ্য - 1);

  যদি(payload_len > len) {
    infof(তথ্য, "UDP payload does not fit destination buffer: %zu > %zu",
          payload_len, len);
    Curl_bufq_skip(recvbufq, offset + payload_len);
    *err = CURLE_RECV_ERROR;
    ফেরত 0;
  }

  যদি(Curl_bufq_len(recvbufq) < offset + payload_len) {
    যদি(Curl_bufq_is_full(recvbufq)) {
      infof(তথ্য, "Error! Capsule does not fit the receive buffer: %zu",
            payload_len);
      *err = CURLE_RECV_ERROR;
      ফেরত 0;
    }
    *err = CURLE_AGAIN;
    ফেরত 0;
  }

  Curl_bufq_skip(recvbufq, offset);
  যদি(!payload_len) {
    *err = CURLE_OK;
    ফেরত 0;
  }
  ফলাফল = Curl_bufq_read(recvbufq, buf, payload_len, &bytes_read);
  যদি(ফলাফল || (bytes_read != payload_len)) {
    infof(তথ্য, "Error! Read less than expected %zu %zu",
          payload_len, bytes_read);
    *err = CURLE_RECV_ERROR;
    ফেরত 0;
  }

  যদি(cf && তথ্য) {
    CURL_TRC_CF(তথ্য, cf, "Processed UDP capsule raw: size=%zu "
                "length_left %zu", payload_len, Curl_bufq_len(recvbufq));
  }
  *err = CURLE_OK;
  ফেরত bytes_read;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
