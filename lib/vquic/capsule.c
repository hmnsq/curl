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

#समावेश "curl_setup.h"

#अगर !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>  /* for htons() */
#पूर्ण_अगर

#समावेश <curl/curl.h>
#समावेश "urldata.h"
#समावेश "curlx/dynbuf.h"
#समावेश "cfilters.h"
#समावेश "curl_trc.h"
#समावेश "bufq.h"
#समावेश "vquic/capsule.h"


/**
 * Convert 64-bit value from network byte order to host byte order
 */
अटल uint64_t capsule_ntohll(uint64_t मूल्य)
{
#अगर defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
  वापस मूल्य;
#या_अगर (defined(__GNUC__) || defined(__clang__)) && \
  defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  वापस __builtin_bswap64(मूल्य);
#अन्यथा
  जोड़ {
    uint64_t u64;
    uint32_t u32[2];
  } src, dst;

  src.u64 = मूल्य;
  dst.u32[0] = ntohl(src.u32[1]);
  dst.u32[1] = ntohl(src.u32[0]);
  वापस dst.u64;
#पूर्ण_अगर
}

/**
 * Encode a variable-length integer into a plain buffer.
 * @param buf   Output buffer (must have at least 8 bytes)
 * @param value Value to encode (must be <= 0x3FFFFFFFFFFFFFFF)
 * @return Number of bytes written
 */
अटल माप_प्रकार capsule_encode_varint_buf(uint8_t *buf, uint64_t मूल्य)
{
  DEBUGASSERT(मूल्य <= 0x3FFFFFFFFFFFFFFF);

  अगर(मूल्य <= 0x3F) {
    buf[0] = (uint8_t)मूल्य;
    वापस 1;
  }
  अन्यथा अगर(मूल्य <= 0x3FFF) {
    uint16_t encoded = (uint16_t)मूल्य & 0x3FFF;
    encoded = ntohs(encoded | 0x4000);
    स_नकल(buf, &encoded, 2);
    वापस 2;
  }
  अन्यथा अगर(मूल्य <= 0x3FFFFFFF) {
    uint32_t encoded = (uint32_t)मूल्य & 0x3FFFFFFF;
    encoded = ntohl(encoded | 0x80000000);
    स_नकल(buf, &encoded, 4);
    वापस 4;
  }
  अन्यथा {
    uint64_t encoded = (uint64_t)मूल्य & 0x3FFFFFFFFFFFFFFF;
    encoded = capsule_ntohll(encoded | 0xC000000000000000);
    स_नकल(buf, &encoded, 8);
    वापस 8;
  }
}

अटल CURLcode capsule_peek_u8(काष्ठा bufq *recvbufq,
                                माप_प्रकार offset,
                                uint8_t *pbyte)
{
  स्थिर अचिन्हित अक्षर *peek = शून्य;
  माप_प्रकार peeklen = 0;

  अगर(!Curl_bufq_peek_at(recvbufq, offset, &peek, &peeklen) || !peeklen)
    वापस CURLE_AGAIN;
  *pbyte = peek[0];
  वापस CURLE_OK;
}

अटल CURLcode capsule_decode_varint_at(काष्ठा bufq *recvbufq,
                                         माप_प्रकार offset,
                                         uint64_t *pvalue,
                                         माप_प्रकार *pconsumed)
{
  uint8_t first_byte, byte;
  uint64_t मूल्य;
  माप_प्रकार nbytes;
  माप_प्रकार i;
  CURLcode परिणाम;

  परिणाम = capsule_peek_u8(recvbufq, offset, &first_byte);
  अगर(परिणाम)
    वापस परिणाम;

  nbytes = (माप_प्रकार)1 << (first_byte >> 6); /* 1, 2, 4 or 8 bytes */
  मूल्य = first_byte & 0x3F;

  क्रम(i = 1; i < nbytes; ++i) {
    परिणाम = capsule_peek_u8(recvbufq, offset + i, &byte);
    अगर(परिणाम)
      वापस परिणाम;
    मूल्य = (मूल्य << 8) | byte;
  }

  *pvalue = मूल्य;
  *pconsumed = nbytes;
  वापस CURLE_OK;
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
  DEBUGASSERT(hdrlen >= HTTP_CAPSULE_HEADER_MAX_आकार);
  अगर(hdrlen < HTTP_CAPSULE_HEADER_MAX_आकार)
    वापस 0;
  hdr[off++] = 0; /* capsule type: HTTP Datagram */
  off += capsule_encode_varint_buf(hdr + off, (uint64_t)payload_len + 1);
  hdr[off++] = 0; /* context ID */
  वापस off;
}

CURLcode Curl_capsule_encap_udp_datagram(काष्ठा bufq *q,
                                         स्थिर व्योम *buf, माप_प्रकार blen)
{
  CURLcode परिणाम;
  uint8_t hdr[HTTP_CAPSULE_HEADER_MAX_आकार];
  माप_प्रकार hdr_len, nwritten;

  hdr_len = capsule_encap_udp_hdr(hdr, माप(hdr), blen);
  DEBUGASSERT(hdr_len);
  अगर(!hdr_len)
    वापस CURLE_FAILED_INIT;

  परिणाम = Curl_bufq_write(q, hdr, hdr_len, &nwritten);
  अगर(!परिणाम && (nwritten != hdr_len))
    वापस CURLE_WRITE_ERROR;
  अगर(!परिणाम) {
    परिणाम = Curl_bufq_write(q, buf, blen, &nwritten);
    अगर(!परिणाम && (nwritten != blen))
      वापस CURLE_WRITE_ERROR;
  }
  अगर(परिणाम == CURLE_AGAIN)
    वापस CURLE_WRITE_ERROR;
  वापस परिणाम;
}

माप_प्रकार Curl_capsule_process_udp_raw(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य,
                                    काष्ठा bufq *recvbufq,
                                    अचिन्हित अक्षर *buf, माप_प्रकार len,
                                    CURLcode *err)
{
  स्थिर अचिन्हित अक्षर *context_id, *capsule_type;
  माप_प्रकार read_आकार, varint_len;
  uint64_t capsule_लंबाई;
  माप_प्रकार offset, payload_len;
  माप_प्रकार bytes_read = 0;
  CURLcode परिणाम = CURLE_OK;

  अगर(!len) {
    *err = CURLE_BAD_FUNCTION_ARGUMENT;
    वापस 0;
  }

  अगर(Curl_bufq_is_empty(recvbufq)) {
    *err = CURLE_AGAIN;
    वापस 0;
  }

  अगर(!Curl_bufq_peek(recvbufq, &capsule_type, &read_आकार) || !read_आकार) {
    *err = CURLE_AGAIN;
    वापस 0;
  }

  अगर(capsule_type[0]) {
    infof(तथ्य, "Error! Invalid capsule type: %d", capsule_type[0]);
    Curl_bufq_skip(recvbufq, 1);
    *err = CURLE_RECV_ERROR;
    वापस 0;
  }

  offset = 1;
  परिणाम = capsule_decode_varint_at(recvbufq, offset, &capsule_लंबाई,
                                    &varint_len);
  अगर(परिणाम == CURLE_AGAIN) {
    *err = CURLE_AGAIN;
    वापस 0;
  }
  अन्यथा अगर(परिणाम) {
    *err = CURLE_RECV_ERROR;
    वापस 0;
  }
  offset += varint_len;

  अगर(!Curl_bufq_peek_at(recvbufq, offset, &context_id, &read_आकार) ||
     !read_आकार) {
    *err = CURLE_AGAIN;
    वापस 0;
  }

  अगर(*context_id) {
    infof(तथ्य, "Error! Invalid context ID: %02x", *context_id);
    Curl_bufq_skip(recvbufq, offset + 1);
    *err = CURLE_RECV_ERROR;
    वापस 0;
  }
  offset += 1;

  अगर(!capsule_लंबाई) {
    infof(तथ्य, "Error! Invalid capsule length: 0");
    Curl_bufq_skip(recvbufq, offset);
    *err = CURLE_RECV_ERROR;
    वापस 0;
  }
  अगर(capsule_लंबाई - 1 > (uint64_t)(SIZE_MAX - offset)) {
    infof(तथ्य, "Error! Capsule length too large: %" CURL_FORMAT_CURL_OFF_T,
          (curl_off_t)capsule_लंबाई);
    *err = CURLE_RECV_ERROR;
    वापस 0;
  }
  payload_len = (माप_प्रकार)(capsule_लंबाई - 1);

  अगर(payload_len > len) {
    infof(तथ्य, "UDP payload does not fit destination buffer: %zu > %zu",
          payload_len, len);
    Curl_bufq_skip(recvbufq, offset + payload_len);
    *err = CURLE_RECV_ERROR;
    वापस 0;
  }

  अगर(Curl_bufq_len(recvbufq) < offset + payload_len) {
    अगर(Curl_bufq_is_full(recvbufq)) {
      infof(तथ्य, "Error! Capsule does not fit the receive buffer: %zu",
            payload_len);
      *err = CURLE_RECV_ERROR;
      वापस 0;
    }
    *err = CURLE_AGAIN;
    वापस 0;
  }

  Curl_bufq_skip(recvbufq, offset);
  अगर(!payload_len) {
    *err = CURLE_OK;
    वापस 0;
  }
  परिणाम = Curl_bufq_read(recvbufq, buf, payload_len, &bytes_read);
  अगर(परिणाम || (bytes_read != payload_len)) {
    infof(तथ्य, "Error! Read less than expected %zu %zu",
          payload_len, bytes_read);
    *err = CURLE_RECV_ERROR;
    वापस 0;
  }

  अगर(cf && तथ्य) {
    CURL_TRC_CF(तथ्य, cf, "Processed UDP capsule raw: size=%zu "
                "length_left %zu", payload_len, Curl_bufq_len(recvbufq));
  }
  *err = CURLE_OK;
  वापस bytes_read;
}

#पूर्ण_अगर /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
