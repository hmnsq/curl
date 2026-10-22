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

#که_تعریف_شوی HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>  /* for htons() */
#د_که_پای

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
ساكن uint64_t capsule_ntohll(uint64_t ارزښت)
{
#إذا defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
  أرجع ارزښت;
#وإلاإذا (defined(__GNUC__) || defined(__clang__)) && \
  defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  أرجع __builtin_bswap64(ارزښت);
#وإلا
  اتحاد {
    uint64_t u64;
    uint32_t u32[2];
  } src, dst;

  src.u64 = ارزښت;
  dst.u32[0] = ntohl(src.u32[1]);
  dst.u32[1] = ntohl(src.u32[0]);
  أرجع dst.u64;
#د_که_پای
}

/**
 * Encode a variable-length integer into a plain buffer.
 * @param buf   Output buffer (must have at least 8 bytes)
 * @param value Value to encode (must be <= 0x3FFFFFFFFFFFFFFF)
 * @return Number of bytes written
 */
ساكن size_t capsule_encode_varint_buf(uint8_t *buf, uint64_t ارزښت)
{
  DEBUGASSERT(ارزښت <= 0x3FFFFFFFFFFFFFFF);

  إذا(ارزښت <= 0x3F) {
    buf[0] = (uint8_t)ارزښت;
    أرجع 1;
  }
  وإلا إذا(ارزښت <= 0x3FFF) {
    uint16_t encoded = (uint16_t)ارزښت & 0x3FFF;
    encoded = ntohs(encoded | 0x4000);
    حافظه_نقل(buf, &encoded, 2);
    أرجع 2;
  }
  وإلا إذا(ارزښت <= 0x3FFFFFFF) {
    uint32_t encoded = (uint32_t)ارزښت & 0x3FFFFFFF;
    encoded = ntohl(encoded | 0x80000000);
    حافظه_نقل(buf, &encoded, 4);
    أرجع 4;
  }
  وإلا {
    uint64_t encoded = (uint64_t)ارزښت & 0x3FFFFFFFFFFFFFFF;
    encoded = capsule_ntohll(encoded | 0xC000000000000000);
    حافظه_نقل(buf, &encoded, 8);
    أرجع 8;
  }
}

ساكن CURLcode capsule_peek_u8(بنية bufq *recvbufq,
                                size_t دننه_واټن,
                                uint8_t *pbyte)
{
  ثابت بینښه حرف *peek = عدم;
  size_t peeklen = 0;

  إذا(!Curl_bufq_peek_at(recvbufq, دننه_واټن, &peek, &peeklen) || !peeklen)
    أرجع CURLE_AGAIN;
  *pbyte = peek[0];
  أرجع CURLE_OK;
}

ساكن CURLcode capsule_decode_varint_at(بنية bufq *recvbufq,
                                         size_t دننه_واټن,
                                         uint64_t *pvalue,
                                         size_t *pconsumed)
{
  uint8_t لومړی_byte, byte;
  uint64_t ارزښت;
  size_t nbytes;
  size_t i;
  CURLcode پایله;

  پایله = capsule_peek_u8(recvbufq, دننه_واټن, &لومړی_byte);
  إذا(پایله)
    أرجع پایله;

  nbytes = (size_t)1 << (لومړی_byte >> 6); /* 1, 2, 4 or 8 bytes */
  ارزښت = لومړی_byte & 0x3F;

  لكل(i = 1; i < nbytes; ++i) {
    پایله = capsule_peek_u8(recvbufq, دننه_واټن + i, &byte);
    إذا(پایله)
      أرجع پایله;
    ارزښت = (ارزښت << 8) | byte;
  }

  *pvalue = ارزښت;
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
  DEBUGASSERT(hdrlen >= HTTP_CAPSULE_سرلیک_MAX_اندازه);
  إذا(hdrlen < HTTP_CAPSULE_سرلیک_MAX_اندازه)
    أرجع 0;
  hdr[off++] = 0; /* capsule type: HTTP Datagram */
  off += capsule_encode_varint_buf(hdr + off, (uint64_t)payload_len + 1);
  hdr[off++] = 0; /* context ID */
  أرجع off;
}

CURLcode Curl_capsule_encap_udp_datagram(بنية bufq *q,
                                         ثابت فراغ *buf, size_t blen)
{
  CURLcode پایله;
  uint8_t hdr[HTTP_CAPSULE_سرلیک_MAX_اندازه];
  size_t hdr_len, nwritten;

  hdr_len = capsule_encap_udp_hdr(hdr, حجم(hdr), blen);
  DEBUGASSERT(hdr_len);
  إذا(!hdr_len)
    أرجع CURLE_FAILED_INIT;

  پایله = Curl_bufq_write(q, hdr, hdr_len, &nwritten);
  إذا(!پایله && (nwritten != hdr_len))
    أرجع CURLE_WRITE_ERROR;
  إذا(!پایله) {
    پایله = Curl_bufq_write(q, buf, blen, &nwritten);
    إذا(!پایله && (nwritten != blen))
      أرجع CURLE_WRITE_ERROR;
  }
  إذا(پایله == CURLE_AGAIN)
    أرجع CURLE_WRITE_ERROR;
  أرجع پایله;
}

size_t Curl_capsule_process_udp_raw(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    بنية bufq *recvbufq,
                                    بینښه حرف *buf, size_t len,
                                    CURLcode *err)
{
  ثابت بینښه حرف *context_id, *capsule_type;
  size_t read_اندازه, varint_len;
  uint64_t capsule_اوږدتیا;
  size_t دننه_واټن, payload_len;
  size_t بایټونه_read = 0;
  CURLcode پایله = CURLE_OK;

  إذا(!len) {
    *err = CURLE_BAD_FUNCTION_دلیل;
    أرجع 0;
  }

  إذا(Curl_bufq_is_تشه(recvbufq)) {
    *err = CURLE_AGAIN;
    أرجع 0;
  }

  إذا(!Curl_bufq_peek(recvbufq, &capsule_type, &read_اندازه) || !read_اندازه) {
    *err = CURLE_AGAIN;
    أرجع 0;
  }

  إذا(capsule_type[0]) {
    infof(معلومات, "Error! Invalid capsule type: %d", capsule_type[0]);
    Curl_bufq_skip(recvbufq, 1);
    *err = CURLE_RECV_ERROR;
    أرجع 0;
  }

  دننه_واټن = 1;
  پایله = capsule_decode_varint_at(recvbufq, دننه_واټن, &capsule_اوږدتیا,
                                    &varint_len);
  إذا(پایله == CURLE_AGAIN) {
    *err = CURLE_AGAIN;
    أرجع 0;
  }
  وإلا إذا(پایله) {
    *err = CURLE_RECV_ERROR;
    أرجع 0;
  }
  دننه_واټن += varint_len;

  إذا(!Curl_bufq_peek_at(recvbufq, دننه_واټن, &context_id, &read_اندازه) ||
     !read_اندازه) {
    *err = CURLE_AGAIN;
    أرجع 0;
  }

  إذا(*context_id) {
    infof(معلومات, "Error! Invalid context ID: %02x", *context_id);
    Curl_bufq_skip(recvbufq, دننه_واټن + 1);
    *err = CURLE_RECV_ERROR;
    أرجع 0;
  }
  دننه_واټن += 1;

  إذا(!capsule_اوږدتیا) {
    infof(معلومات, "Error! Invalid capsule length: 0");
    Curl_bufq_skip(recvbufq, دننه_واټن);
    *err = CURLE_RECV_ERROR;
    أرجع 0;
  }
  إذا(capsule_اوږدتیا - 1 > (uint64_t)(SIZE_MAX - دننه_واټن)) {
    infof(معلومات, "Error! Capsule length too large: %" CURL_FORMAT_CURL_OFF_T,
          (curl_off_t)capsule_اوږدتیا);
    *err = CURLE_RECV_ERROR;
    أرجع 0;
  }
  payload_len = (size_t)(capsule_اوږدتیا - 1);

  إذا(payload_len > len) {
    infof(معلومات, "UDP payload does not fit destination buffer: %zu > %zu",
          payload_len, len);
    Curl_bufq_skip(recvbufq, دننه_واټن + payload_len);
    *err = CURLE_RECV_ERROR;
    أرجع 0;
  }

  إذا(Curl_bufq_len(recvbufq) < دننه_واټن + payload_len) {
    إذا(Curl_bufq_is_ډک(recvbufq)) {
      infof(معلومات, "Error! Capsule does not fit the receive buffer: %zu",
            payload_len);
      *err = CURLE_RECV_ERROR;
      أرجع 0;
    }
    *err = CURLE_AGAIN;
    أرجع 0;
  }

  Curl_bufq_skip(recvbufq, دننه_واټن);
  إذا(!payload_len) {
    *err = CURLE_OK;
    أرجع 0;
  }
  پایله = Curl_bufq_read(recvbufq, buf, payload_len, &بایټونه_read);
  إذا(پایله || (بایټونه_read != payload_len)) {
    infof(معلومات, "Error! Read less than expected %zu %zu",
          payload_len, بایټونه_read);
    *err = CURLE_RECV_ERROR;
    أرجع 0;
  }

  إذا(cf && معلومات) {
    CURL_TRC_CF(معلومات, cf, "Processed UDP capsule raw: size=%zu "
                "length_left %zu", payload_len, Curl_bufq_len(recvbufq));
  }
  *err = CURLE_OK;
  أرجع بایټونه_read;
}

#د_که_پای /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
