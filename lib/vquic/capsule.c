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

#अन्तर्भाव "curl_setup.h"

#जर !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>  /* for htons() */
#endif

#अन्तर्भाव <curl/curl.h>
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "bufq.h"
#अन्तर्भाव "vquic/capsule.h"


/**
 * Convert 64-bit value from network byte order to host byte order
 */
स्थिर uint64_t capsule_ntohll(uint64_t मूल्य)
{
#जर defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
  परत मूल्य;
#किंवाजर (defined(__GNUC__) || defined(__clang__)) && \
  defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  परत __builtin_bswap64(मूल्य);
#अन्यथा
  संघ {
    uint64_t u64;
    uint32_t u32[2];
  } src, dst;

  src.u64 = मूल्य;
  dst.u32[0] = ntohl(src.u32[1]);
  dst.u32[1] = ntohl(src.u32[0]);
  परत dst.u64;
#endif
}

/**
 * Encode a variable-length integer into a plain buffer.
 * @param buf   Output buffer (must have at least 8 bytes)
 * @param value Value to encode (must be <= 0x3FFFFFFFFFFFFFFF)
 * @return Number of bytes written
 */
स्थिर माप_प्रकार capsule_encode_varint_buf(uint8_t *buf, uint64_t मूल्य)
{
  DEBUGASSERT(मूल्य <= 0x3FFFFFFFFFFFFFFF);

  जर(मूल्य <= 0x3F) {
    buf[0] = (uint8_t)मूल्य;
    परत 1;
  }
  अन्यथा जर(मूल्य <= 0x3FFF) {
    uint16_t encoded = (uint16_t)मूल्य & 0x3FFF;
    encoded = ntohs(encoded | 0x4000);
    स_नकल(buf, &encoded, 2);
    परत 2;
  }
  अन्यथा जर(मूल्य <= 0x3FFFFFFF) {
    uint32_t encoded = (uint32_t)मूल्य & 0x3FFFFFFF;
    encoded = ntohl(encoded | 0x80000000);
    स_नकल(buf, &encoded, 4);
    परत 4;
  }
  अन्यथा {
    uint64_t encoded = (uint64_t)मूल्य & 0x3FFFFFFFFFFFFFFF;
    encoded = capsule_ntohll(encoded | 0xC000000000000000);
    स_नकल(buf, &encoded, 8);
    परत 8;
  }
}

स्थिर CURLcode capsule_peek_u8(संरचना bufq *recvbufq,
                                माप_प्रकार offset,
                                uint8_t *pbyte)
{
  स्थिरांक अचिन्हित अक्षर *peek = रिक्त;
  माप_प्रकार peeklen = 0;

  जर(!Curl_bufq_peek_at(recvbufq, offset, &peek, &peeklen) || !peeklen)
    परत CURLE_AGAIN;
  *pbyte = peek[0];
  परत CURLE_OK;
}

स्थिर CURLcode capsule_decode_varint_at(संरचना bufq *recvbufq,
                                         माप_प्रकार offset,
                                         uint64_t *pvalue,
                                         माप_प्रकार *pconsumed)
{
  uint8_t first_byte, byte;
  uint64_t मूल्य;
  माप_प्रकार nbytes;
  माप_प्रकार i;
  CURLcode result;

  result = capsule_peek_u8(recvbufq, offset, &first_byte);
  जर(result)
    परत result;

  nbytes = (माप_प्रकार)1 << (first_byte >> 6); /* 1, 2, 4 or 8 bytes */
  मूल्य = first_byte & 0x3F;

  साठी(i = 1; i < nbytes; ++i) {
    result = capsule_peek_u8(recvbufq, offset + i, &byte);
    जर(result)
      परत result;
    मूल्य = (मूल्य << 8) | byte;
  }

  *pvalue = मूल्य;
  *pconsumed = nbytes;
  परत CURLE_OK;
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
UNITTEST माप_प्रकार capsule_encap_udp_hdr(uint8_t *hdr, माप_प्रकार hdrlen,
                                      माप_प्रकार payload_len);
UNITTEST माप_प्रकार capsule_encap_udp_hdr(uint8_t *hdr, माप_प्रकार hdrlen,
                                      माप_प्रकार payload_len)
{
  माप_प्रकार off = 0;
  DEBUGASSERT(hdrlen >= HTTP_CAPSULE_HEADER_MAX_SIZE);
  जर(hdrlen < HTTP_CAPSULE_HEADER_MAX_SIZE)
    परत 0;
  hdr[off++] = 0; /* capsule type: HTTP Datagram */
  off += capsule_encode_varint_buf(hdr + off, (uint64_t)payload_len + 1);
  hdr[off++] = 0; /* context ID */
  परत off;
}

CURLcode Curl_capsule_encap_udp_datagram(संरचना bufq *q,
                                         स्थिरांक शून्य *buf, माप_प्रकार blen)
{
  CURLcode result;
  uint8_t hdr[HTTP_CAPSULE_HEADER_MAX_SIZE];
  माप_प्रकार hdr_len, nwritten;

  hdr_len = capsule_encap_udp_hdr(hdr, आकार(hdr), blen);
  DEBUGASSERT(hdr_len);
  जर(!hdr_len)
    परत CURLE_FAILED_INIT;

  result = Curl_bufq_write(q, hdr, hdr_len, &nwritten);
  जर(!result && (nwritten != hdr_len))
    परत CURLE_WRITE_ERROR;
  जर(!result) {
    result = Curl_bufq_write(q, buf, blen, &nwritten);
    जर(!result && (nwritten != blen))
      परत CURLE_WRITE_ERROR;
  }
  जर(result == CURLE_AGAIN)
    परत CURLE_WRITE_ERROR;
  परत result;
}

माप_प्रकार Curl_capsule_process_udp_raw(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    संरचना bufq *recvbufq,
                                    अचिन्हित अक्षर *buf, माप_प्रकार len,
                                    CURLcode *err)
{
  स्थिरांक अचिन्हित अक्षर *context_id, *capsule_type;
  माप_प्रकार read_size, varint_len;
  uint64_t capsule_length;
  माप_प्रकार offset, payload_len;
  माप_प्रकार bytes_read = 0;
  CURLcode result = CURLE_OK;

  जर(!len) {
    *err = CURLE_BAD_FUNCTION_ARGUMENT;
    परत 0;
  }

  जर(Curl_bufq_is_empty(recvbufq)) {
    *err = CURLE_AGAIN;
    परत 0;
  }

  जर(!Curl_bufq_peek(recvbufq, &capsule_type, &read_size) || !read_size) {
    *err = CURLE_AGAIN;
    परत 0;
  }

  जर(capsule_type[0]) {
    infof(data, "Error! Invalid capsule type: %d", capsule_type[0]);
    Curl_bufq_skip(recvbufq, 1);
    *err = CURLE_RECV_ERROR;
    परत 0;
  }

  offset = 1;
  result = capsule_decode_varint_at(recvbufq, offset, &capsule_length,
                                    &varint_len);
  जर(result == CURLE_AGAIN) {
    *err = CURLE_AGAIN;
    परत 0;
  }
  अन्यथा जर(result) {
    *err = CURLE_RECV_ERROR;
    परत 0;
  }
  offset += varint_len;

  जर(!Curl_bufq_peek_at(recvbufq, offset, &context_id, &read_size) ||
     !read_size) {
    *err = CURLE_AGAIN;
    परत 0;
  }

  जर(*context_id) {
    infof(data, "Error! Invalid context ID: %02x", *context_id);
    Curl_bufq_skip(recvbufq, offset + 1);
    *err = CURLE_RECV_ERROR;
    परत 0;
  }
  offset += 1;

  जर(!capsule_length) {
    infof(data, "Error! Invalid capsule length: 0");
    Curl_bufq_skip(recvbufq, offset);
    *err = CURLE_RECV_ERROR;
    परत 0;
  }
  जर(capsule_length - 1 > (uint64_t)(SIZE_MAX - offset)) {
    infof(data, "Error! Capsule length too large: %" CURL_FORMAT_CURL_OFF_T,
          (curl_off_t)capsule_length);
    *err = CURLE_RECV_ERROR;
    परत 0;
  }
  payload_len = (माप_प्रकार)(capsule_length - 1);

  जर(payload_len > len) {
    infof(data, "UDP payload does not fit destination buffer: %zu > %zu",
          payload_len, len);
    Curl_bufq_skip(recvbufq, offset + payload_len);
    *err = CURLE_RECV_ERROR;
    परत 0;
  }

  जर(Curl_bufq_len(recvbufq) < offset + payload_len) {
    जर(Curl_bufq_is_full(recvbufq)) {
      infof(data, "Error! Capsule does not fit the receive buffer: %zu",
            payload_len);
      *err = CURLE_RECV_ERROR;
      परत 0;
    }
    *err = CURLE_AGAIN;
    परत 0;
  }

  Curl_bufq_skip(recvbufq, offset);
  जर(!payload_len) {
    *err = CURLE_OK;
    परत 0;
  }
  result = Curl_bufq_read(recvbufq, buf, payload_len, &bytes_read);
  जर(result || (bytes_read != payload_len)) {
    infof(data, "Error! Read less than expected %zu %zu",
          payload_len, bytes_read);
    *err = CURLE_RECV_ERROR;
    परत 0;
  }

  जर(cf && data) {
    CURL_TRC_CF(data, cf, "Processed UDP capsule raw: size=%zu "
                "length_left %zu", payload_len, Curl_bufq_len(recvbufq));
  }
  *err = CURLE_OK;
  परत bytes_read;
}

#endif /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
