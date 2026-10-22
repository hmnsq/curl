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

#যদি defined(USE_SSL) && defined(USE_SSLS_EXPORT)

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "vtls/vtls_scache.h"
#অন্তর্ভাব "vtls/vtls_spack.h"
#অন্তর্ভাব "curlx/strdup.h"

#define CURL_SPACK_VERSION       0x01
#define CURL_SPACK_IETF_ID       0x02
#define CURL_SPACK_VALID_UNTIL   0x03
#define CURL_SPACK_TICKET        0x04
#define CURL_SPACK_ALPN          0x05
#define CURL_SPACK_EARLYDATA     0x06
#define CURL_SPACK_QUICTP        0x07
#define CURL_SPACK_SECTRUST      0x08

স্থির CURLcode spack_enc8(কাঠামো dynbuf *buf, uint8_t b)
{
  ফেরত curlx_dyn_addn(buf, &b, 1);
}

স্থির CURLcode spack_dec8(uint8_t *val, ধ্রুবক uint8_t **src,
                           ধ্রুবক uint8_t *end)
{
  যদি(end - *src < 1)
    ফেরত CURLE_READ_ERROR;
  *val = **src;
  *src += 1;
  ফেরত CURLE_OK;
}

স্থির CURLcode spack_enc16(কাঠামো dynbuf *buf, uint16_t val)
{
  uint8_t nval[2];
  nval[0] = (uint8_t)(val >> 8);
  nval[1] = (uint8_t)val;
  ফেরত curlx_dyn_addn(buf, nval, আকার(nval));
}

স্থির CURLcode spack_dec16(uint16_t *val, ধ্রুবক uint8_t **src,
                            ধ্রুবক uint8_t *end)
{
  যদি(end - *src < 2)
    ফেরত CURLE_READ_ERROR;
  *val = (uint16_t)((*src)[0] << 8 | (*src)[1]);
  *src += 2;
  ফেরত CURLE_OK;
}

স্থির CURLcode spack_enc32(কাঠামো dynbuf *buf, uint32_t val)
{
  uint8_t nval[4];
  nval[0] = (uint8_t)(val >> 24);
  nval[1] = (uint8_t)(val >> 16);
  nval[2] = (uint8_t)(val >> 8);
  nval[3] = (uint8_t)val;
  ফেরত curlx_dyn_addn(buf, nval, আকার(nval));
}

স্থির CURLcode spack_dec32(uint32_t *val, ধ্রুবক uint8_t **src,
                            ধ্রুবক uint8_t *end)
{
  যদি(end - *src < 4)
    ফেরত CURLE_READ_ERROR;
  *val = (uint32_t)(*src)[0] << 24 | (uint32_t)(*src)[1] << 16 |
         (uint32_t)(*src)[2] << 8 | (*src)[3];
  *src += 4;
  ফেরত CURLE_OK;
}

স্থির CURLcode spack_enc64(কাঠামো dynbuf *buf, uint64_t val)
{
  uint8_t nval[8];
  nval[0] = (uint8_t)(val >> 56);
  nval[1] = (uint8_t)(val >> 48);
  nval[2] = (uint8_t)(val >> 40);
  nval[3] = (uint8_t)(val >> 32);
  nval[4] = (uint8_t)(val >> 24);
  nval[5] = (uint8_t)(val >> 16);
  nval[6] = (uint8_t)(val >> 8);
  nval[7] = (uint8_t)val;
  ফেরত curlx_dyn_addn(buf, nval, আকার(nval));
}

স্থির CURLcode spack_dec64(uint64_t *val, ধ্রুবক uint8_t **src,
                            ধ্রুবক uint8_t *end)
{
  যদি(end - *src < 8)
    ফেরত CURLE_READ_ERROR;
  *val = (uint64_t)(*src)[0] << 56 | (uint64_t)(*src)[1] << 48 |
         (uint64_t)(*src)[2] << 40 | (uint64_t)(*src)[3] << 32 |
         (uint64_t)(*src)[4] << 24 | (uint64_t)(*src)[5] << 16 |
         (uint64_t)(*src)[6] << 8 | (*src)[7];
  *src += 8;
  ফেরত CURLE_OK;
}

স্থির CURLcode spack_encstr16(কাঠামো dynbuf *buf, ধ্রুবক অক্ষর *s)
{
  size_t slen = strlen(s);
  CURLcode ফলাফল;
  যদি(slen > UINT16_MAX)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  ফলাফল = spack_enc16(buf, (uint16_t)slen);
  যদি(!ফলাফল) {
    ফলাফল = curlx_dyn_addn(buf, s, slen);
  }
  ফেরত ফলাফল;
}

স্থির CURLcode spack_decstr16(অক্ষর **val, ধ্রুবক uint8_t **src,
                               ধ্রুবক uint8_t *end)
{
  uint16_t slen;
  CURLcode ফলাফল;

  *val = ফাঁকা;
  ফলাফল = spack_dec16(&slen, src, end);
  যদি(ফলাফল)
    ফেরত ফলাফল;
  যদি(end - *src < slen)
    ফেরত CURLE_READ_ERROR;
  *val = curlx_memdup0((ধ্রুবক অক্ষর *)(*src), slen);
  *src += slen;
  ফেরত *val ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

স্থির CURLcode spack_encdata16(কাঠামো dynbuf *buf, ধ্রুবক uint8_t *তথ্য,
                                size_t তথ্য_len)
{
  CURLcode ফলাফল;
  যদি(তথ্য_len > UINT16_MAX)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  ফলাফল = spack_enc16(buf, (uint16_t)তথ্য_len);
  যদি(!ফলাফল) {
    ফলাফল = curlx_dyn_addn(buf, তথ্য, তথ্য_len);
  }
  ফেরত ফলাফল;
}

স্থির CURLcode spack_decdata16(uint8_t **val, size_t *val_len,
                                ধ্রুবক uint8_t **src, ধ্রুবক uint8_t *end)
{
  uint16_t তথ্য_len;
  CURLcode ফলাফল;

  *val = ফাঁকা;
  ফলাফল = spack_dec16(&তথ্য_len, src, end);
  যদি(ফলাফল)
    ফেরত ফলাফল;
  যদি(end - *src < তথ্য_len)
    ফেরত CURLE_READ_ERROR;
  *val = curlx_memdup0((ধ্রুবক অক্ষর *)(*src), তথ্য_len);
  *val_len = তথ্য_len;
  *src += তথ্য_len;
  ফেরত *val ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

CURLcode Curl_ssl_session_pack(কাঠামো Curl_easy *তথ্য,
                               কাঠামো Curl_ssl_session *s,
                               কাঠামো dynbuf *buf)
{
  CURLcode ফলাফল;
  DEBUGASSERT(s->sdata);
  DEBUGASSERT(s->sdata_len);

  যদি(s->valid_until < 0)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  ফলাফল = spack_enc8(buf, CURL_SPACK_VERSION);
  যদি(!ফলাফল)
    ফলাফল = spack_enc8(buf, CURL_SPACK_TICKET);
  যদি(!ফলাফল)
    ফলাফল = spack_encdata16(buf, s->sdata, s->sdata_len);
  যদি(!ফলাফল)
    ফলাফল = spack_enc8(buf, CURL_SPACK_IETF_ID);
  যদি(!ফলাফল)
    ফলাফল = spack_enc16(buf, (uint16_t)s->ietf_tls_id);
  যদি(!ফলাফল)
    ফলাফল = spack_enc8(buf, CURL_SPACK_VALID_UNTIL);
  যদি(!ফলাফল)
    ফলাফল = spack_enc64(buf, (uint64_t)s->valid_until);
  যদি(!ফলাফল && s->alpn) {
    ফলাফল = spack_enc8(buf, CURL_SPACK_ALPN);
    যদি(!ফলাফল)
      ফলাফল = spack_encstr16(buf, s->alpn);
  }
  যদি(!ফলাফল && s->earlydata_max) {
    যদি(s->earlydata_max > UINT32_MAX)
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    যদি(!ফলাফল)
      ফলাফল = spack_enc8(buf, CURL_SPACK_EARLYDATA);
    যদি(!ফলাফল)
      ফলাফল = spack_enc32(buf, (uint32_t)s->earlydata_max);
  }
  যদি(!ফলাফল && s->sectrust_verified) {
    ফলাফল = spack_enc8(buf, CURL_SPACK_SECTRUST);
  }
  যদি(!ফলাফল && s->quic_tp && s->quic_tp_len) {
    ফলাফল = spack_enc8(buf, CURL_SPACK_QUICTP);
    যদি(!ফলাফল)
      ফলাফল = spack_encdata16(buf, s->quic_tp, s->quic_tp_len);
  }

  যদি(ফলাফল)
    CURL_TRC_SSLS(তথ্য, "error packing data: %d", (পূর্ণসংখ্যা)ফলাফল);
  ফেরত ফলাফল;
}

CURLcode Curl_ssl_session_unpack(কাঠামো Curl_easy *তথ্য,
                                 ধ্রুবক শূন্য *bufv, size_t buflen,
                                 কাঠামো Curl_ssl_session **ps)
{
  কাঠামো Curl_ssl_session *s = ফাঁকা;
  ধ্রুবক unsigned অক্ষর *buf = (ধ্রুবক unsigned অক্ষর *)bufv;
  ধ্রুবক unsigned অক্ষর *end = buf + buflen;
  uint8_t val8, *pval8;
  uint16_t val16;
  uint32_t val32;
  uint64_t val64;
  size_t dlen;
  CURLcode ফলাফল;

  DEBUGASSERT(ps);
  DEBUGASSERT(buf);
  DEBUGASSERT(buflen);
  *ps = ফাঁকা;

  ফলাফল = spack_dec8(&val8, &buf, end);
  যদি(ফলাফল)
    যাও out;
  যদি(val8 != CURL_SPACK_VERSION) {
    ফলাফল = CURLE_READ_ERROR;
    যাও out;
  }

  s = curlx_calloc(1, আকার(*s));
  যদি(!s) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  যখন(buf < end) {
    ফলাফল = spack_dec8(&val8, &buf, end);
    যদি(ফলাফল)
      যাও out;

    নির্বাচন(val8) {
    ক্ষেত্র CURL_SPACK_ALPN:
      curlx_free(s->alpn);
      ফলাফল = spack_decstr16(&s->alpn, &buf, end);
      যদি(ফলাফল)
        যাও out;
      বিরতি;
    ক্ষেত্র CURL_SPACK_EARLYDATA:
      ফলাফল = spack_dec32(&val32, &buf, end);
      যদি(ফলাফল)
        যাও out;
      s->earlydata_max = val32;
      বিরতি;
    ক্ষেত্র CURL_SPACK_IETF_ID:
      ফলাফল = spack_dec16(&val16, &buf, end);
      যদি(ফলাফল)
        যাও out;
      s->ietf_tls_id = val16;
      বিরতি;
    ক্ষেত্র CURL_SPACK_QUICTP: {
      ফলাফল = spack_decdata16(&pval8, &dlen, &buf, end);
      যদি(ফলাফল)
        যাও out;
      curlx_free(s->quic_tp);
      s->quic_tp = pval8;
      s->quic_tp_len = dlen;
      বিরতি;
    }
    ক্ষেত্র CURL_SPACK_TICKET: {
      ফলাফল = spack_decdata16(&pval8, &dlen, &buf, end);
      যদি(ফলাফল)
        যাও out;
      curlx_free(s->sdata);
      s->sdata = pval8;
      s->sdata_len = dlen;
      বিরতি;
    }
    ক্ষেত্র CURL_SPACK_VALID_UNTIL:
      ফলাফল = spack_dec64(&val64, &buf, end);
      যদি(ফলাফল)
        যাও out;
      s->valid_until = (curl_off_t)val64;
      বিরতি;
    ক্ষেত্র CURL_SPACK_SECTRUST:
      s->sectrust_verified = TRUE;
      বিরতি;
    পূর্বনির্ধারিত:  /* unknown tag */
      ফলাফল = CURLE_READ_ERROR;
      যাও out;
    }
  }

out:
  যদি(ফলাফল) {
    CURL_TRC_SSLS(তথ্য, "error unpacking data: %d", (পূর্ণসংখ্যা)ফলাফল);
    Curl_ssl_session_destroy(s);
  }
  অন্যথা
    *ps = s;
  ফেরত ফলাফল;
}

#endif /* USE_SSL && USE_SSLS_EXPORT */
