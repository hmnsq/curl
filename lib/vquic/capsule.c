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

#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_PROXY) && !defined(CURL_DISABLE_HTTP)

#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>  /* for htons() */
#اگر_ختم

#شامل <curl/curl.h>
#شامل "urldata.h"
#شامل "curlx/dynbuf.h"
#شامل "cfilters.h"
#شامل "curl_trc.h"
#شامل "bufq.h"
#شامل "vquic/capsule.h"


/**
 * Convert 64-bit value from network byte order to host byte order
 */
ساکن uint64_t capsule_ntohll(uint64_t قدر)
{
#اگر defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
  واپس قدر;
#وگرنہ_اگر (defined(__GNUC__) || defined(__clang__)) && \
  defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  واپس __builtin_bswap64(قدر);
#ورنہ
  اتحاد {
    uint64_t u64;
    uint32_t u32[2];
  } src, dst;

  src.u64 = قدر;
  dst.u32[0] = ntohl(src.u32[1]);
  dst.u32[1] = ntohl(src.u32[0]);
  واپس dst.u64;
#اگر_ختم
}

/**
 * Encode a variable-length integer into a plain buffer.
 * @param buf   Output buffer (must have at least 8 bytes)
 * @param value Value to encode (must be <= 0x3FFFFFFFFFFFFFFF)
 * @return Number of bytes written
 */
ساکن size_t capsule_encode_varint_buf(uint8_t *buf, uint64_t قدر)
{
  DEBUGASSERT(قدر <= 0x3FFFFFFFFFFFFFFF);

  اگر(قدر <= 0x3F) {
    buf[0] = (uint8_t)قدر;
    واپس 1;
  }
  ورنہ اگر(قدر <= 0x3FFF) {
    uint16_t encoded = (uint16_t)قدر & 0x3FFF;
    encoded = ntohs(encoded | 0x4000);
    حافظہ_نقل(buf, &encoded, 2);
    واپس 2;
  }
  ورنہ اگر(قدر <= 0x3FFFFFFF) {
    uint32_t encoded = (uint32_t)قدر & 0x3FFFFFFF;
    encoded = ntohl(encoded | 0x80000000);
    حافظہ_نقل(buf, &encoded, 4);
    واپس 4;
  }
  ورنہ {
    uint64_t encoded = (uint64_t)قدر & 0x3FFFFFFFFFFFFFFF;
    encoded = capsule_ntohll(encoded | 0xC000000000000000);
    حافظہ_نقل(buf, &encoded, 8);
    واپس 8;
  }
}

ساکن CURLcode capsule_peek_u8(ساخت bufq *recvbufq,
                                size_t انحراف,
                                uint8_t *pbyte)
{
  ثابت غیرعلامتی حرف *peek = تہی;
  size_t peeklen = 0;

  اگر(!Curl_bufq_peek_at(recvbufq, انحراف, &peek, &peeklen) || !peeklen)
    واپس CURLE_AGAIN;
  *pbyte = peek[0];
  واپس CURLE_OK;
}

ساکن CURLcode capsule_decode_varint_at(ساخت bufq *recvbufq,
                                         size_t انحراف,
                                         uint64_t *pvalue,
                                         size_t *pconsumed)
{
  uint8_t پہلا_byte, byte;
  uint64_t قدر;
  size_t nbytes;
  size_t i;
  CURLcode نتیجہ;

  نتیجہ = capsule_peek_u8(recvbufq, انحراف, &پہلا_byte);
  اگر(نتیجہ)
    واپس نتیجہ;

  nbytes = (size_t)1 << (پہلا_byte >> 6); /* 1, 2, 4 or 8 bytes */
  قدر = پہلا_byte & 0x3F;

  برائے(i = 1; i < nbytes; ++i) {
    نتیجہ = capsule_peek_u8(recvbufq, انحراف + i, &byte);
    اگر(نتیجہ)
      واپس نتیجہ;
    قدر = (قدر << 8) | byte;
  }

  *pvalue = قدر;
  *pconsumed = nbytes;
  واپس CURLE_OK;
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
  DEBUGASSERT(hdrlen >= HTTP_CAPSULE_سرخی_MAX_سائز);
  اگر(hdrlen < HTTP_CAPSULE_سرخی_MAX_سائز)
    واپس 0;
  hdr[off++] = 0; /* capsule type: HTTP Datagram */
  off += capsule_encode_varint_buf(hdr + off, (uint64_t)payload_len + 1);
  hdr[off++] = 0; /* context ID */
  واپس off;
}

CURLcode Curl_capsule_encap_udp_datagram(ساخت bufq *q,
                                         ثابت خالی *buf, size_t blen)
{
  CURLcode نتیجہ;
  uint8_t hdr[HTTP_CAPSULE_سرخی_MAX_سائز];
  size_t hdr_len, nwritten;

  hdr_len = capsule_encap_udp_hdr(hdr, حجم(hdr), blen);
  DEBUGASSERT(hdr_len);
  اگر(!hdr_len)
    واپس CURLE_FAILED_INIT;

  نتیجہ = Curl_bufq_write(q, hdr, hdr_len, &nwritten);
  اگر(!نتیجہ && (nwritten != hdr_len))
    واپس CURLE_WRITE_ERROR;
  اگر(!نتیجہ) {
    نتیجہ = Curl_bufq_write(q, buf, blen, &nwritten);
    اگر(!نتیجہ && (nwritten != blen))
      واپس CURLE_WRITE_ERROR;
  }
  اگر(نتیجہ == CURLE_AGAIN)
    واپس CURLE_WRITE_ERROR;
  واپس نتیجہ;
}

size_t Curl_capsule_process_udp_raw(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا,
                                    ساخت bufq *recvbufq,
                                    غیرعلامتی حرف *buf, size_t len,
                                    CURLcode *err)
{
  ثابت غیرعلامتی حرف *context_id, *capsule_type;
  size_t read_سائز, varint_len;
  uint64_t capsule_لمبائی;
  size_t انحراف, payload_len;
  size_t بائٹیں_read = 0;
  CURLcode نتیجہ = CURLE_OK;

  اگر(!len) {
    *err = CURLE_BAD_FUNCTION_دلیل;
    واپس 0;
  }

  اگر(Curl_bufq_is_خالی_خانے(recvbufq)) {
    *err = CURLE_AGAIN;
    واپس 0;
  }

  اگر(!Curl_bufq_peek(recvbufq, &capsule_type, &read_سائز) || !read_سائز) {
    *err = CURLE_AGAIN;
    واپس 0;
  }

  اگر(capsule_type[0]) {
    infof(ڈیٹا, "Error! Invalid capsule type: %d", capsule_type[0]);
    Curl_bufq_skip(recvbufq, 1);
    *err = CURLE_RECV_ERROR;
    واپس 0;
  }

  انحراف = 1;
  نتیجہ = capsule_decode_varint_at(recvbufq, انحراف, &capsule_لمبائی,
                                    &varint_len);
  اگر(نتیجہ == CURLE_AGAIN) {
    *err = CURLE_AGAIN;
    واپس 0;
  }
  ورنہ اگر(نتیجہ) {
    *err = CURLE_RECV_ERROR;
    واپس 0;
  }
  انحراف += varint_len;

  اگر(!Curl_bufq_peek_at(recvbufq, انحراف, &context_id, &read_سائز) ||
     !read_سائز) {
    *err = CURLE_AGAIN;
    واپس 0;
  }

  اگر(*context_id) {
    infof(ڈیٹا, "Error! Invalid context ID: %02x", *context_id);
    Curl_bufq_skip(recvbufq, انحراف + 1);
    *err = CURLE_RECV_ERROR;
    واپس 0;
  }
  انحراف += 1;

  اگر(!capsule_لمبائی) {
    infof(ڈیٹا, "Error! Invalid capsule length: 0");
    Curl_bufq_skip(recvbufq, انحراف);
    *err = CURLE_RECV_ERROR;
    واپس 0;
  }
  اگر(capsule_لمبائی - 1 > (uint64_t)(SIZE_MAX - انحراف)) {
    infof(ڈیٹا, "Error! Capsule length too large: %" CURL_FORMAT_CURL_OFF_T,
          (curl_off_t)capsule_لمبائی);
    *err = CURLE_RECV_ERROR;
    واپس 0;
  }
  payload_len = (size_t)(capsule_لمبائی - 1);

  اگر(payload_len > len) {
    infof(ڈیٹا, "UDP payload does not fit destination buffer: %zu > %zu",
          payload_len, len);
    Curl_bufq_skip(recvbufq, انحراف + payload_len);
    *err = CURLE_RECV_ERROR;
    واپس 0;
  }

  اگر(Curl_bufq_len(recvbufq) < انحراف + payload_len) {
    اگر(Curl_bufq_is_بھرا(recvbufq)) {
      infof(ڈیٹا, "Error! Capsule does not fit the receive buffer: %zu",
            payload_len);
      *err = CURLE_RECV_ERROR;
      واپس 0;
    }
    *err = CURLE_AGAIN;
    واپس 0;
  }

  Curl_bufq_skip(recvbufq, انحراف);
  اگر(!payload_len) {
    *err = CURLE_OK;
    واپس 0;
  }
  نتیجہ = Curl_bufq_read(recvbufq, buf, payload_len, &بائٹیں_read);
  اگر(نتیجہ || (بائٹیں_read != payload_len)) {
    infof(ڈیٹا, "Error! Read less than expected %zu %zu",
          payload_len, بائٹیں_read);
    *err = CURLE_RECV_ERROR;
    واپس 0;
  }

  اگر(cf && ڈیٹا) {
    CURL_TRC_CF(ڈیٹا, cf, "Processed UDP capsule raw: size=%zu "
                "length_left %zu", payload_len, Curl_bufq_len(recvbufq));
  }
  *err = CURLE_OK;
  واپس بائٹیں_read;
}

#اگر_ختم /* !CURL_DISABLE_PROXY && !CURL_DISABLE_HTTP */
