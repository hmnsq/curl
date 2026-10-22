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

#אם defined(USE_SSL) && defined(USE_SSLS_EXPORT)

#ضمن "urldata.h"
#ضمن "curl_trc.h"
#ضمن "vtls/vtls_scache.h"
#ضمن "vtls/vtls_spack.h"
#ضمن "curlx/strdup.h"

#define CURL_SPACK_VERSION       0x01
#define CURL_SPACK_IETF_ID       0x02
#define CURL_SPACK_VALID_UNTIL   0x03
#define CURL_SPACK_TICKET        0x04
#define CURL_SPACK_ALPN          0x05
#define CURL_SPACK_EARLYDATA     0x06
#define CURL_SPACK_QUICTP        0x07
#define CURL_SPACK_SECTRUST      0x08

ساكن CURLcode spack_enc8(بنية dynbuf *buf, uint8_t b)
{
  החזר curlx_dyn_addn(buf, &b, 1);
}

ساكن CURLcode spack_dec8(uint8_t *val, ثابت uint8_t **src,
                           ثابت uint8_t *end)
{
  אם(end - *src < 1)
    החזר CURLE_READ_ERROR;
  *val = **src;
  *src += 1;
  החזר CURLE_OK;
}

ساكن CURLcode spack_enc16(بنية dynbuf *buf, uint16_t val)
{
  uint8_t nval[2];
  nval[0] = (uint8_t)(val >> 8);
  nval[1] = (uint8_t)val;
  החזר curlx_dyn_addn(buf, nval, حجم(nval));
}

ساكن CURLcode spack_dec16(uint16_t *val, ثابت uint8_t **src,
                            ثابت uint8_t *end)
{
  אם(end - *src < 2)
    החזר CURLE_READ_ERROR;
  *val = (uint16_t)((*src)[0] << 8 | (*src)[1]);
  *src += 2;
  החזר CURLE_OK;
}

ساكن CURLcode spack_enc32(بنية dynbuf *buf, uint32_t val)
{
  uint8_t nval[4];
  nval[0] = (uint8_t)(val >> 24);
  nval[1] = (uint8_t)(val >> 16);
  nval[2] = (uint8_t)(val >> 8);
  nval[3] = (uint8_t)val;
  החזר curlx_dyn_addn(buf, nval, حجم(nval));
}

ساكن CURLcode spack_dec32(uint32_t *val, ثابت uint8_t **src,
                            ثابت uint8_t *end)
{
  אם(end - *src < 4)
    החזר CURLE_READ_ERROR;
  *val = (uint32_t)(*src)[0] << 24 | (uint32_t)(*src)[1] << 16 |
         (uint32_t)(*src)[2] << 8 | (*src)[3];
  *src += 4;
  החזר CURLE_OK;
}

ساكن CURLcode spack_enc64(بنية dynbuf *buf, uint64_t val)
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
  החזר curlx_dyn_addn(buf, nval, حجم(nval));
}

ساكن CURLcode spack_dec64(uint64_t *val, ثابت uint8_t **src,
                            ثابت uint8_t *end)
{
  אם(end - *src < 8)
    החזר CURLE_READ_ERROR;
  *val = (uint64_t)(*src)[0] << 56 | (uint64_t)(*src)[1] << 48 |
         (uint64_t)(*src)[2] << 40 | (uint64_t)(*src)[3] << 32 |
         (uint64_t)(*src)[4] << 24 | (uint64_t)(*src)[5] << 16 |
         (uint64_t)(*src)[6] << 8 | (*src)[7];
  *src += 8;
  החזר CURLE_OK;
}

ساكن CURLcode spack_encstr16(بنية dynbuf *buf, ثابت حرف *s)
{
  size_t slen = strlen(s);
  CURLcode result;
  אם(slen > UINT16_MAX)
    החזר CURLE_BAD_FUNCTION_ARGUMENT;
  result = spack_enc16(buf, (uint16_t)slen);
  אם(!result) {
    result = curlx_dyn_addn(buf, s, slen);
  }
  החזר result;
}

ساكن CURLcode spack_decstr16(حرف **val, ثابت uint8_t **src,
                               ثابت uint8_t *end)
{
  uint16_t slen;
  CURLcode result;

  *val = عدم;
  result = spack_dec16(&slen, src, end);
  אם(result)
    החזר result;
  אם(end - *src < slen)
    החזר CURLE_READ_ERROR;
  *val = curlx_memdup0((ثابت حرف *)(*src), slen);
  *src += slen;
  החזר *val ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

ساكن CURLcode spack_encdata16(بنية dynbuf *buf, ثابت uint8_t *data,
                                size_t data_len)
{
  CURLcode result;
  אם(data_len > UINT16_MAX)
    החזר CURLE_BAD_FUNCTION_ARGUMENT;
  result = spack_enc16(buf, (uint16_t)data_len);
  אם(!result) {
    result = curlx_dyn_addn(buf, data, data_len);
  }
  החזר result;
}

ساكن CURLcode spack_decdata16(uint8_t **val, size_t *val_len,
                                ثابت uint8_t **src, ثابت uint8_t *end)
{
  uint16_t data_len;
  CURLcode result;

  *val = عدم;
  result = spack_dec16(&data_len, src, end);
  אם(result)
    החזר result;
  אם(end - *src < data_len)
    החזר CURLE_READ_ERROR;
  *val = curlx_memdup0((ثابت حرف *)(*src), data_len);
  *val_len = data_len;
  *src += data_len;
  החזר *val ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

CURLcode Curl_ssl_session_pack(بنية Curl_easy *data,
                               بنية Curl_ssl_session *s,
                               بنية dynbuf *buf)
{
  CURLcode result;
  DEBUGASSERT(s->sdata);
  DEBUGASSERT(s->sdata_len);

  אם(s->valid_until < 0)
    החזר CURLE_BAD_FUNCTION_ARGUMENT;

  result = spack_enc8(buf, CURL_SPACK_VERSION);
  אם(!result)
    result = spack_enc8(buf, CURL_SPACK_TICKET);
  אם(!result)
    result = spack_encdata16(buf, s->sdata, s->sdata_len);
  אם(!result)
    result = spack_enc8(buf, CURL_SPACK_IETF_ID);
  אם(!result)
    result = spack_enc16(buf, (uint16_t)s->ietf_tls_id);
  אם(!result)
    result = spack_enc8(buf, CURL_SPACK_VALID_UNTIL);
  אם(!result)
    result = spack_enc64(buf, (uint64_t)s->valid_until);
  אם(!result && s->alpn) {
    result = spack_enc8(buf, CURL_SPACK_ALPN);
    אם(!result)
      result = spack_encstr16(buf, s->alpn);
  }
  אם(!result && s->earlydata_max) {
    אם(s->earlydata_max > UINT32_MAX)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    אם(!result)
      result = spack_enc8(buf, CURL_SPACK_EARLYDATA);
    אם(!result)
      result = spack_enc32(buf, (uint32_t)s->earlydata_max);
  }
  אם(!result && s->sectrust_verified) {
    result = spack_enc8(buf, CURL_SPACK_SECTRUST);
  }
  אם(!result && s->quic_tp && s->quic_tp_len) {
    result = spack_enc8(buf, CURL_SPACK_QUICTP);
    אם(!result)
      result = spack_encdata16(buf, s->quic_tp, s->quic_tp_len);
  }

  אם(result)
    CURL_TRC_SSLS(data, "error packing data: %d", (صحيح)result);
  החזר result;
}

CURLcode Curl_ssl_session_unpack(بنية Curl_easy *data,
                                 ثابت فراغ *bufv, size_t buflen,
                                 بنية Curl_ssl_session **ps)
{
  بنية Curl_ssl_session *s = عدم;
  ثابت unsigned حرف *buf = (ثابت unsigned حرف *)bufv;
  ثابت unsigned حرف *end = buf + buflen;
  uint8_t val8, *pval8;
  uint16_t val16;
  uint32_t val32;
  uint64_t val64;
  size_t dlen;
  CURLcode result;

  DEBUGASSERT(ps);
  DEBUGASSERT(buf);
  DEBUGASSERT(buflen);
  *ps = عدم;

  result = spack_dec8(&val8, &buf, end);
  אם(result)
    اذهب out;
  אם(val8 != CURL_SPACK_VERSION) {
    result = CURLE_READ_ERROR;
    اذهب out;
  }

  s = curlx_calloc(1, حجم(*s));
  אם(!s) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }

  כלעוד(buf < end) {
    result = spack_dec8(&val8, &buf, end);
    אם(result)
      اذهب out;

    بدل(val8) {
    حالة CURL_SPACK_ALPN:
      curlx_free(s->alpn);
      result = spack_decstr16(&s->alpn, &buf, end);
      אם(result)
        اذهب out;
      اكسر;
    حالة CURL_SPACK_EARLYDATA:
      result = spack_dec32(&val32, &buf, end);
      אם(result)
        اذهب out;
      s->earlydata_max = val32;
      اكسر;
    حالة CURL_SPACK_IETF_ID:
      result = spack_dec16(&val16, &buf, end);
      אם(result)
        اذهب out;
      s->ietf_tls_id = val16;
      اكسر;
    حالة CURL_SPACK_QUICTP: {
      result = spack_decdata16(&pval8, &dlen, &buf, end);
      אם(result)
        اذهب out;
      curlx_free(s->quic_tp);
      s->quic_tp = pval8;
      s->quic_tp_len = dlen;
      اكسر;
    }
    حالة CURL_SPACK_TICKET: {
      result = spack_decdata16(&pval8, &dlen, &buf, end);
      אם(result)
        اذهب out;
      curlx_free(s->sdata);
      s->sdata = pval8;
      s->sdata_len = dlen;
      اكسر;
    }
    حالة CURL_SPACK_VALID_UNTIL:
      result = spack_dec64(&val64, &buf, end);
      אם(result)
        اذهب out;
      s->valid_until = (curl_off_t)val64;
      اكسر;
    حالة CURL_SPACK_SECTRUST:
      s->sectrust_verified = TRUE;
      اكسر;
    افتراضي:  /* unknown tag */
      result = CURLE_READ_ERROR;
      اذهب out;
    }
  }

out:
  אם(result) {
    CURL_TRC_SSLS(data, "error unpacking data: %d", (صحيح)result);
    Curl_ssl_session_destroy(s);
  }
  אחרת
    *ps = s;
  החזר result;
}

#endif /* USE_SSL && USE_SSLS_EXPORT */
