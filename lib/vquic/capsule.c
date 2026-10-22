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

#అన్తర్భావ "curl_setup.h"

#ఒకవేళ !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>  /* for htons() */
#అయితే_ముగింపు

#అన్తర్భావ <curl/curl.h>
#అన్తర్భావ "urldata.h"
#అన్తర్భావ "curlx/dynbuf.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "bufq.h"
#అన్తర్భావ "vquic/capsule.h"


/**
 * Convert 64-bit value from network byte order to host byte order
 */
స్థిర uint64_t capsule_ntohll(uint64_t విలువ)
{
#ఒకవేళ defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
  తిరిగి విలువ;
#లేదావేళ (defined(__GNUC__) || defined(__clang__)) && \
  defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  తిరిగి __builtin_bswap64(విలువ);
#లేకపోతే
  సంఘం {
    uint64_t u64;
    uint32_t u32[2];
  } src, dst;

  src.u64 = విలువ;
  dst.u32[0] = ntohl(src.u32[1]);
  dst.u32[1] = ntohl(src.u32[0]);
  తిరిగి dst.u64;
#అయితే_ముగింపు
}

/**
 * Encode a variable-length integer into a plain buffer.
 * @param buf   Output buffer (must have at least 8 bytes)
 * @param value Value to encode (must be <= 0x3FFFFFFFFFFFFFFF)
 * @return Number of bytes written
 */
స్థిర size_t capsule_encode_varint_buf(uint8_t *buf, uint64_t విలువ)
{
  DEBUGASSERT(విలువ <= 0x3FFFFFFFFFFFFFFF);

  ఒకవేళ(విలువ <= 0x3F) {
    buf[0] = (uint8_t)విలువ;
    తిరిగి 1;
  }
  లేకపోతే ఒకవేళ(విలువ <= 0x3FFF) {
    uint16_t encoded = (uint16_t)విలువ & 0x3FFF;
    encoded = ntohs(encoded | 0x4000);
    జ్ఞాపకం_నకలు(buf, &encoded, 2);
    తిరిగి 2;
  }
  లేకపోతే ఒకవేళ(విలువ <= 0x3FFFFFFF) {
    uint32_t encoded = (uint32_t)విలువ & 0x3FFFFFFF;
    encoded = ntohl(encoded | 0x80000000);
    జ్ఞాపకం_నకలు(buf, &encoded, 4);
    తిరిగి 4;
  }
  లేకపోతే {
    uint64_t encoded = (uint64_t)విలువ & 0x3FFFFFFFFFFFFFFF;
    encoded = capsule_ntohll(encoded | 0xC000000000000000);
    జ్ఞాపకం_నకలు(buf, &encoded, 8);
    తిరిగి 8;
  }
}

స్థిర CURLcode capsule_peek_u8(నిర్మాణం bufq *recvbufq,
                                size_t స్థానభ్రంశం,
                                uint8_t *pbyte)
{
  స్థిరరాశి గుర్తులేని అక్షరం *peek = ఖాళీ;
  size_t peeklen = 0;

  ఒకవేళ(!Curl_bufq_peek_at(recvbufq, స్థానభ్రంశం, &peek, &peeklen) || !peeklen)
    తిరిగి CURLE_AGAIN;
  *pbyte = peek[0];
  తిరిగి CURLE_OK;
}

స్థిర CURLcode capsule_decode_varint_at(నిర్మాణం bufq *recvbufq,
                                         size_t స్థానభ్రంశం,
                                         uint64_t *pvalue,
                                         size_t *pconsumed)
{
  uint8_t మొదటి_byte, byte;
  uint64_t విలువ;
  size_t nbytes;
  size_t i;
  CURLcode ఫలితం;

  ఫలితం = capsule_peek_u8(recvbufq, స్థానభ్రంశం, &మొదటి_byte);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  nbytes = (size_t)1 << (మొదటి_byte >> 6); /* 1, 2, 4 or 8 bytes */
  విలువ = మొదటి_byte & 0x3F;

  కొరకు(i = 1; i < nbytes; ++i) {
    ఫలితం = capsule_peek_u8(recvbufq, స్థానభ్రంశం + i, &byte);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    విలువ = (విలువ << 8) | byte;
  }

  *pvalue = విలువ;
  *pconsumed = nbytes;
  తిరిగి CURLE_OK;
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
  DEBUGASSERT(hdrlen >= HTTP_CAPSULE_శీర్షిక_MAX_కొలత);
  ఒకవేళ(hdrlen < HTTP_CAPSULE_శీర్షిక_MAX_కొలత)
    తిరిగి 0;
  hdr[off++] = 0; /* capsule type: HTTP Datagram */
  off += capsule_encode_varint_buf(hdr + off, (uint64_t)payload_len + 1);
  hdr[off++] = 0; /* context ID */
  తిరిగి off;
}

CURLcode Curl_capsule_encap_udp_datagram(నిర్మాణం bufq *q,
                                         స్థిరరాశి శూన్య *buf, size_t blen)
{
  CURLcode ఫలితం;
  uint8_t hdr[HTTP_CAPSULE_శీర్షిక_MAX_కొలత];
  size_t hdr_len, nwritten;

  hdr_len = capsule_encap_udp_hdr(hdr, పరిమాణం(hdr), blen);
  DEBUGASSERT(hdr_len);
  ఒకవేళ(!hdr_len)
    తిరిగి CURLE_FAILED_INIT;

  ఫలితం = Curl_bufq_write(q, hdr, hdr_len, &nwritten);
  ఒకవేళ(!ఫలితం && (nwritten != hdr_len))
    తిరిగి CURLE_WRITE_ERROR;
  ఒకవేళ(!ఫలితం) {
    ఫలితం = Curl_bufq_write(q, buf, blen, &nwritten);
    ఒకవేళ(!ఫలితం && (nwritten != blen))
      తిరిగి CURLE_WRITE_ERROR;
  }
  ఒకవేళ(ఫలితం == CURLE_AGAIN)
    తిరిగి CURLE_WRITE_ERROR;
  తిరిగి ఫలితం;
}

size_t Curl_capsule_process_udp_raw(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం bufq *recvbufq,
                                    గుర్తులేని అక్షరం *buf, size_t len,
                                    CURLcode *err)
{
  స్థిరరాశి గుర్తులేని అక్షరం *context_id, *capsule_type;
  size_t read_కొలత, varint_len;
  uint64_t capsule_నిడివి;
  size_t స్థానభ్రంశం, payload_len;
  size_t బైట్లు_read = 0;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(!len) {
    *err = CURLE_BAD_FUNCTION_వాదం;
    తిరిగి 0;
  }

  ఒకవేళ(Curl_bufq_is_ఖాళీలు(recvbufq)) {
    *err = CURLE_AGAIN;
    తిరిగి 0;
  }

  ఒకవేళ(!Curl_bufq_peek(recvbufq, &capsule_type, &read_కొలత) || !read_కొలత) {
    *err = CURLE_AGAIN;
    తిరిగి 0;
  }

  ఒకవేళ(capsule_type[0]) {
    infof(దత్తాంశం, "Error! Invalid capsule type: %d", capsule_type[0]);
    Curl_bufq_skip(recvbufq, 1);
    *err = CURLE_RECV_ERROR;
    తిరిగి 0;
  }

  స్థానభ్రంశం = 1;
  ఫలితం = capsule_decode_varint_at(recvbufq, స్థానభ్రంశం, &capsule_నిడివి,
                                    &varint_len);
  ఒకవేళ(ఫలితం == CURLE_AGAIN) {
    *err = CURLE_AGAIN;
    తిరిగి 0;
  }
  లేకపోతే ఒకవేళ(ఫలితం) {
    *err = CURLE_RECV_ERROR;
    తిరిగి 0;
  }
  స్థానభ్రంశం += varint_len;

  ఒకవేళ(!Curl_bufq_peek_at(recvbufq, స్థానభ్రంశం, &context_id, &read_కొలత) ||
     !read_కొలత) {
    *err = CURLE_AGAIN;
    తిరిగి 0;
  }

  ఒకవేళ(*context_id) {
    infof(దత్తాంశం, "Error! Invalid context ID: %02x", *context_id);
    Curl_bufq_skip(recvbufq, స్థానభ్రంశం + 1);
    *err = CURLE_RECV_ERROR;
    తిరిగి 0;
  }
  స్థానభ్రంశం += 1;

  ఒకవేళ(!capsule_నిడివి) {
    infof(దత్తాంశం, "Error! Invalid capsule length: 0");
    Curl_bufq_skip(recvbufq, స్థానభ్రంశం);
    *err = CURLE_RECV_ERROR;
    తిరిగి 0;
  }
  ఒకవేళ(capsule_నిడివి - 1 > (uint64_t)(SIZE_MAX - స్థానభ్రంశం)) {
    infof(దత్తాంశం, "Error! Capsule length too large: %" CURL_FORMAT_CURL_OFF_T,
          (curl_off_t)capsule_నిడివి);
    *err = CURLE_RECV_ERROR;
    తిరిగి 0;
  }
  payload_len = (size_t)(capsule_నిడివి - 1);

  ఒకవేళ(payload_len > len) {
    infof(దత్తాంశం, "UDP payload does not fit destination buffer: %zu > %zu",
          payload_len, len);
    Curl_bufq_skip(recvbufq, స్థానభ్రంశం + payload_len);
    *err = CURLE_RECV_ERROR;
    తిరిగి 0;
  }

  ఒకవేళ(Curl_bufq_len(recvbufq) < స్థానభ్రంశం + payload_len) {
    ఒకవేళ(Curl_bufq_is_నిండిన(recvbufq)) {
      infof(దత్తాంశం, "Error! Capsule does not fit the receive buffer: %zu",
            payload_len);
      *err = CURLE_RECV_ERROR;
      తిరిగి 0;
    }
    *err = CURLE_AGAIN;
    తిరిగి 0;
  }

  Curl_bufq_skip(recvbufq, స్థానభ్రంశం);
  ఒకవేళ(!payload_len) {
    *err = CURLE_OK;
    తిరిగి 0;
  }
  ఫలితం = Curl_bufq_read(recvbufq, buf, payload_len, &బైట్లు_read);
  ఒకవేళ(ఫలితం || (బైట్లు_read != payload_len)) {
    infof(దత్తాంశం, "Error! Read less than expected %zu %zu",
          payload_len, బైట్లు_read);
    *err = CURLE_RECV_ERROR;
    తిరిగి 0;
  }

  ఒకవేళ(cf && దత్తాంశం) {
    CURL_TRC_CF(దత్తాంశం, cf, "Processed UDP capsule raw: size=%zu "
                "length_left %zu", payload_len, Curl_bufq_len(recvbufq));
  }
  *err = CURLE_OK;
  తిరిగి బైట్లు_read;
}

#అయితే_ముగింపు /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
