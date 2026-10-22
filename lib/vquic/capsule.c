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

#यदि !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#यदि_परिभाषितम् HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>  /* for htons() */
#यदि_अन्तः

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
स्थिर uint64_t capsule_ntohll(uint64_t मानम्)
{
#यदि defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
  प्रत्यागम मानम्;
#नोचेद्यदि (defined(__GNUC__) || defined(__clang__)) && \
  defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  प्रत्यागम __builtin_bswap64(मानम्);
#अन्यथा
  सङ्घ {
    uint64_t u64;
    uint32_t u32[2];
  } src, dst;

  src.u64 = मानम्;
  dst.u32[0] = ntohl(src.u32[1]);
  dst.u32[1] = ntohl(src.u32[0]);
  प्रत्यागम dst.u64;
#यदि_अन्तः
}

/**
 * Encode a variable-length integer into a plain buffer.
 * @param buf   Output buffer (must have at least 8 bytes)
 * @param value Value to encode (must be <= 0x3FFFFFFFFFFFFFFF)
 * @return Number of bytes written
 */
स्थिर size_t capsule_encode_varint_buf(uint8_t *buf, uint64_t मानम्)
{
  DEBUGASSERT(मानम् <= 0x3FFFFFFFFFFFFFFF);

  यदि(मानम् <= 0x3F) {
    buf[0] = (uint8_t)मानम्;
    प्रत्यागम 1;
  }
  अन्यथा यदि(मानम् <= 0x3FFF) {
    uint16_t encoded = (uint16_t)मानम् & 0x3FFF;
    encoded = ntohs(encoded | 0x4000);
    स्मृतिं_प्रतिलिख(buf, &encoded, 2);
    प्रत्यागम 2;
  }
  अन्यथा यदि(मानम् <= 0x3FFFFFFF) {
    uint32_t encoded = (uint32_t)मानम् & 0x3FFFFFFF;
    encoded = ntohl(encoded | 0x80000000);
    स्मृतिं_प्रतिलिख(buf, &encoded, 4);
    प्रत्यागम 4;
  }
  अन्यथा {
    uint64_t encoded = (uint64_t)मानम् & 0x3FFFFFFFFFFFFFFF;
    encoded = capsule_ntohll(encoded | 0xC000000000000000);
    स्मृतिं_प्रतिलिख(buf, &encoded, 8);
    प्रत्यागम 8;
  }
}

स्थिर CURLcode capsule_peek_u8(संरचना bufq *recvbufq,
                                size_t विस्थापनम्,
                                uint8_t *pbyte)
{
  ध्रुव अचिह्नित वर्ण *peek = रिक्त;
  size_t peeklen = 0;

  यदि(!Curl_bufq_peek_at(recvbufq, विस्थापनम्, &peek, &peeklen) || !peeklen)
    प्रत्यागम CURLE_AGAIN;
  *pbyte = peek[0];
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode capsule_decode_varint_at(संरचना bufq *recvbufq,
                                         size_t विस्थापनम्,
                                         uint64_t *pvalue,
                                         size_t *pconsumed)
{
  uint8_t first_byte, byte;
  uint64_t मानम्;
  size_t nbytes;
  size_t i;
  CURLcode परिणाम;

  परिणाम = capsule_peek_u8(recvbufq, विस्थापनम्, &first_byte);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  nbytes = (size_t)1 << (first_byte >> 6); /* 1, 2, 4 or 8 bytes */
  मानम् = first_byte & 0x3F;

  प्रति(i = 1; i < nbytes; ++i) {
    परिणाम = capsule_peek_u8(recvbufq, विस्थापनम् + i, &byte);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    मानम् = (मानम् << 8) | byte;
  }

  *pvalue = मानम्;
  *pconsumed = nbytes;
  प्रत्यागम CURLE_OK;
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
  DEBUGASSERT(hdrlen >= HTTP_CAPSULE_शीर्षकम्_MAX_आकार);
  यदि(hdrlen < HTTP_CAPSULE_शीर्षकम्_MAX_आकार)
    प्रत्यागम 0;
  hdr[off++] = 0; /* capsule type: HTTP Datagram */
  off += capsule_encode_varint_buf(hdr + off, (uint64_t)payload_len + 1);
  hdr[off++] = 0; /* context ID */
  प्रत्यागम off;
}

CURLcode Curl_capsule_encap_udp_datagram(संरचना bufq *q,
                                         ध्रुव शून्य *buf, size_t blen)
{
  CURLcode परिणाम;
  uint8_t hdr[HTTP_CAPSULE_शीर्षकम्_MAX_आकार];
  size_t hdr_len, nwritten;

  hdr_len = capsule_encap_udp_hdr(hdr, परिमाण(hdr), blen);
  DEBUGASSERT(hdr_len);
  यदि(!hdr_len)
    प्रत्यागम CURLE_FAILED_INIT;

  परिणाम = Curl_bufq_write(q, hdr, hdr_len, &nwritten);
  यदि(!परिणाम && (nwritten != hdr_len))
    प्रत्यागम CURLE_WRITE_ERROR;
  यदि(!परिणाम) {
    परिणाम = Curl_bufq_write(q, buf, blen, &nwritten);
    यदि(!परिणाम && (nwritten != blen))
      प्रत्यागम CURLE_WRITE_ERROR;
  }
  यदि(परिणाम == CURLE_AGAIN)
    प्रत्यागम CURLE_WRITE_ERROR;
  प्रत्यागम परिणाम;
}

size_t Curl_capsule_process_udp_raw(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *दत्त,
                                    संरचना bufq *recvbufq,
                                    अचिह्नित वर्ण *buf, size_t len,
                                    CURLcode *err)
{
  ध्रुव अचिह्नित वर्ण *context_id, *capsule_type;
  size_t read_आकार, varint_len;
  uint64_t capsule_दीर्घता;
  size_t विस्थापनम्, payload_len;
  size_t अष्टकानि_read = 0;
  CURLcode परिणाम = CURLE_OK;

  यदि(!len) {
    *err = CURLE_BAD_FUNCTION_तर्कः;
    प्रत्यागम 0;
  }

  यदि(Curl_bufq_is_रिक्तानि(recvbufq)) {
    *err = CURLE_AGAIN;
    प्रत्यागम 0;
  }

  यदि(!Curl_bufq_peek(recvbufq, &capsule_type, &read_आकार) || !read_आकार) {
    *err = CURLE_AGAIN;
    प्रत्यागम 0;
  }

  यदि(capsule_type[0]) {
    infof(दत्त, "Error! Invalid capsule type: %d", capsule_type[0]);
    Curl_bufq_skip(recvbufq, 1);
    *err = CURLE_RECV_ERROR;
    प्रत्यागम 0;
  }

  विस्थापनम् = 1;
  परिणाम = capsule_decode_varint_at(recvbufq, विस्थापनम्, &capsule_दीर्घता,
                                    &varint_len);
  यदि(परिणाम == CURLE_AGAIN) {
    *err = CURLE_AGAIN;
    प्रत्यागम 0;
  }
  अन्यथा यदि(परिणाम) {
    *err = CURLE_RECV_ERROR;
    प्रत्यागम 0;
  }
  विस्थापनम् += varint_len;

  यदि(!Curl_bufq_peek_at(recvbufq, विस्थापनम्, &context_id, &read_आकार) ||
     !read_आकार) {
    *err = CURLE_AGAIN;
    प्रत्यागम 0;
  }

  यदि(*context_id) {
    infof(दत्त, "Error! Invalid context ID: %02x", *context_id);
    Curl_bufq_skip(recvbufq, विस्थापनम् + 1);
    *err = CURLE_RECV_ERROR;
    प्रत्यागम 0;
  }
  विस्थापनम् += 1;

  यदि(!capsule_दीर्घता) {
    infof(दत्त, "Error! Invalid capsule length: 0");
    Curl_bufq_skip(recvbufq, विस्थापनम्);
    *err = CURLE_RECV_ERROR;
    प्रत्यागम 0;
  }
  यदि(capsule_दीर्घता - 1 > (uint64_t)(SIZE_MAX - विस्थापनम्)) {
    infof(दत्त, "Error! Capsule length too large: %" CURL_FORMAT_CURL_OFF_T,
          (curl_off_t)capsule_दीर्घता);
    *err = CURLE_RECV_ERROR;
    प्रत्यागम 0;
  }
  payload_len = (size_t)(capsule_दीर्घता - 1);

  यदि(payload_len > len) {
    infof(दत्त, "UDP payload does not fit destination buffer: %zu > %zu",
          payload_len, len);
    Curl_bufq_skip(recvbufq, विस्थापनम् + payload_len);
    *err = CURLE_RECV_ERROR;
    प्रत्यागम 0;
  }

  यदि(Curl_bufq_len(recvbufq) < विस्थापनम् + payload_len) {
    यदि(Curl_bufq_is_पूर्णम्(recvbufq)) {
      infof(दत्त, "Error! Capsule does not fit the receive buffer: %zu",
            payload_len);
      *err = CURLE_RECV_ERROR;
      प्रत्यागम 0;
    }
    *err = CURLE_AGAIN;
    प्रत्यागम 0;
  }

  Curl_bufq_skip(recvbufq, विस्थापनम्);
  यदि(!payload_len) {
    *err = CURLE_OK;
    प्रत्यागम 0;
  }
  परिणाम = Curl_bufq_read(recvbufq, buf, payload_len, &अष्टकानि_read);
  यदि(परिणाम || (अष्टकानि_read != payload_len)) {
    infof(दत्त, "Error! Read less than expected %zu %zu",
          payload_len, अष्टकानि_read);
    *err = CURLE_RECV_ERROR;
    प्रत्यागम 0;
  }

  यदि(cf && दत्त) {
    CURL_TRC_CF(दत्त, cf, "Processed UDP capsule raw: size=%zu "
                "length_left %zu", payload_len, Curl_bufq_len(recvbufq));
  }
  *err = CURLE_OK;
  प्रत्यागम अष्टकानि_read;
}

#यदि_अन्तः /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
