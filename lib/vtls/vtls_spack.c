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

#اگر defined(USE_SSL) && defined(USE_SSLS_EXPORT)

#شامل "urldata.h"
#شامل "curl_trc.h"
#شامل "vtls/vtls_scache.h"
#شامل "vtls/vtls_spack.h"
#شامل "curlx/strdup.h"

#define CURL_SPACK_VERSION       0x01
#define CURL_SPACK_IETF_ID       0x02
#define CURL_SPACK_VALID_UNTIL   0x03
#define CURL_SPACK_TICKET        0x04
#define CURL_SPACK_ALPN          0x05
#define CURL_SPACK_EARLYDATA     0x06
#define CURL_SPACK_QUICTP        0x07
#define CURL_SPACK_SECTRUST      0x08

ایستا CURLcode spack_enc8(ساختار dynbuf *buf, uint8_t b)
{
  بازگشت curlx_dyn_addn(buf, &b, 1);
}

ایستا CURLcode spack_dec8(uint8_t *val, ثابت uint8_t **src,
                           ثابت uint8_t *end)
{
  اگر(end - *src < 1)
    بازگشت CURLE_READ_ERROR;
  *val = **src;
  *src += 1;
  بازگشت CURLE_OK;
}

ایستا CURLcode spack_enc16(ساختار dynbuf *buf, uint16_t val)
{
  uint8_t nval[2];
  nval[0] = (uint8_t)(val >> 8);
  nval[1] = (uint8_t)val;
  بازگشت curlx_dyn_addn(buf, nval, اندازه(nval));
}

ایستا CURLcode spack_dec16(uint16_t *val, ثابت uint8_t **src,
                            ثابت uint8_t *end)
{
  اگر(end - *src < 2)
    بازگشت CURLE_READ_ERROR;
  *val = (uint16_t)((*src)[0] << 8 | (*src)[1]);
  *src += 2;
  بازگشت CURLE_OK;
}

ایستا CURLcode spack_enc32(ساختار dynbuf *buf, uint32_t val)
{
  uint8_t nval[4];
  nval[0] = (uint8_t)(val >> 24);
  nval[1] = (uint8_t)(val >> 16);
  nval[2] = (uint8_t)(val >> 8);
  nval[3] = (uint8_t)val;
  بازگشت curlx_dyn_addn(buf, nval, اندازه(nval));
}

ایستا CURLcode spack_dec32(uint32_t *val, ثابت uint8_t **src,
                            ثابت uint8_t *end)
{
  اگر(end - *src < 4)
    بازگشت CURLE_READ_ERROR;
  *val = (uint32_t)(*src)[0] << 24 | (uint32_t)(*src)[1] << 16 |
         (uint32_t)(*src)[2] << 8 | (*src)[3];
  *src += 4;
  بازگشت CURLE_OK;
}

ایستا CURLcode spack_enc64(ساختار dynbuf *buf, uint64_t val)
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
  بازگشت curlx_dyn_addn(buf, nval, اندازه(nval));
}

ایستا CURLcode spack_dec64(uint64_t *val, ثابت uint8_t **src,
                            ثابت uint8_t *end)
{
  اگر(end - *src < 8)
    بازگشت CURLE_READ_ERROR;
  *val = (uint64_t)(*src)[0] << 56 | (uint64_t)(*src)[1] << 48 |
         (uint64_t)(*src)[2] << 40 | (uint64_t)(*src)[3] << 32 |
         (uint64_t)(*src)[4] << 24 | (uint64_t)(*src)[5] << 16 |
         (uint64_t)(*src)[6] << 8 | (*src)[7];
  *src += 8;
  بازگشت CURLE_OK;
}

ایستا CURLcode spack_encstr16(ساختار dynbuf *buf, ثابت نویسه *s)
{
  size_t slen = strlen(s);
  CURLcode result;
  اگر(slen > UINT16_MAX)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  result = spack_enc16(buf, (uint16_t)slen);
  اگر(!result) {
    result = curlx_dyn_addn(buf, s, slen);
  }
  بازگشت result;
}

ایستا CURLcode spack_decstr16(نویسه **val, ثابت uint8_t **src,
                               ثابت uint8_t *end)
{
  uint16_t slen;
  CURLcode result;

  *val = پوچ;
  result = spack_dec16(&slen, src, end);
  اگر(result)
    بازگشت result;
  اگر(end - *src < slen)
    بازگشت CURLE_READ_ERROR;
  *val = curlx_memdup0((ثابت نویسه *)(*src), slen);
  *src += slen;
  بازگشت *val ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

ایستا CURLcode spack_encdata16(ساختار dynbuf *buf, ثابت uint8_t *data,
                                size_t data_len)
{
  CURLcode result;
  اگر(data_len > UINT16_MAX)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  result = spack_enc16(buf, (uint16_t)data_len);
  اگر(!result) {
    result = curlx_dyn_addn(buf, data, data_len);
  }
  بازگشت result;
}

ایستا CURLcode spack_decdata16(uint8_t **val, size_t *val_len,
                                ثابت uint8_t **src, ثابت uint8_t *end)
{
  uint16_t data_len;
  CURLcode result;

  *val = پوچ;
  result = spack_dec16(&data_len, src, end);
  اگر(result)
    بازگشت result;
  اگر(end - *src < data_len)
    بازگشت CURLE_READ_ERROR;
  *val = curlx_memdup0((ثابت نویسه *)(*src), data_len);
  *val_len = data_len;
  *src += data_len;
  بازگشت *val ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

CURLcode Curl_ssl_session_pack(ساختار Curl_easy *data,
                               ساختار Curl_ssl_session *s,
                               ساختار dynbuf *buf)
{
  CURLcode result;
  DEBUGASSERT(s->sdata);
  DEBUGASSERT(s->sdata_len);

  اگر(s->valid_until < 0)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  result = spack_enc8(buf, CURL_SPACK_VERSION);
  اگر(!result)
    result = spack_enc8(buf, CURL_SPACK_TICKET);
  اگر(!result)
    result = spack_encdata16(buf, s->sdata, s->sdata_len);
  اگر(!result)
    result = spack_enc8(buf, CURL_SPACK_IETF_ID);
  اگر(!result)
    result = spack_enc16(buf, (uint16_t)s->ietf_tls_id);
  اگر(!result)
    result = spack_enc8(buf, CURL_SPACK_VALID_UNTIL);
  اگر(!result)
    result = spack_enc64(buf, (uint64_t)s->valid_until);
  اگر(!result && s->alpn) {
    result = spack_enc8(buf, CURL_SPACK_ALPN);
    اگر(!result)
      result = spack_encstr16(buf, s->alpn);
  }
  اگر(!result && s->earlydata_max) {
    اگر(s->earlydata_max > UINT32_MAX)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    اگر(!result)
      result = spack_enc8(buf, CURL_SPACK_EARLYDATA);
    اگر(!result)
      result = spack_enc32(buf, (uint32_t)s->earlydata_max);
  }
  اگر(!result && s->sectrust_verified) {
    result = spack_enc8(buf, CURL_SPACK_SECTRUST);
  }
  اگر(!result && s->quic_tp && s->quic_tp_len) {
    result = spack_enc8(buf, CURL_SPACK_QUICTP);
    اگر(!result)
      result = spack_encdata16(buf, s->quic_tp, s->quic_tp_len);
  }

  اگر(result)
    CURL_TRC_SSLS(data, "error packing data: %d", (صحیح)result);
  بازگشت result;
}

CURLcode Curl_ssl_session_unpack(ساختار Curl_easy *data,
                                 ثابت تهی *bufv, size_t buflen,
                                 ساختار Curl_ssl_session **ps)
{
  ساختار Curl_ssl_session *s = پوچ;
  ثابت unsigned نویسه *buf = (ثابت unsigned نویسه *)bufv;
  ثابت unsigned نویسه *end = buf + buflen;
  uint8_t val8, *pval8;
  uint16_t val16;
  uint32_t val32;
  uint64_t val64;
  size_t dlen;
  CURLcode result;

  DEBUGASSERT(ps);
  DEBUGASSERT(buf);
  DEBUGASSERT(buflen);
  *ps = پوچ;

  result = spack_dec8(&val8, &buf, end);
  اگر(result)
    برو out;
  اگر(val8 != CURL_SPACK_VERSION) {
    result = CURLE_READ_ERROR;
    برو out;
  }

  s = curlx_calloc(1, اندازه(*s));
  اگر(!s) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }

  وقتی(buf < end) {
    result = spack_dec8(&val8, &buf, end);
    اگر(result)
      برو out;

    انتخاب(val8) {
    حالت CURL_SPACK_ALPN:
      curlx_free(s->alpn);
      result = spack_decstr16(&s->alpn, &buf, end);
      اگر(result)
        برو out;
      بشکن;
    حالت CURL_SPACK_EARLYDATA:
      result = spack_dec32(&val32, &buf, end);
      اگر(result)
        برو out;
      s->earlydata_max = val32;
      بشکن;
    حالت CURL_SPACK_IETF_ID:
      result = spack_dec16(&val16, &buf, end);
      اگر(result)
        برو out;
      s->ietf_tls_id = val16;
      بشکن;
    حالت CURL_SPACK_QUICTP: {
      result = spack_decdata16(&pval8, &dlen, &buf, end);
      اگر(result)
        برو out;
      curlx_free(s->quic_tp);
      s->quic_tp = pval8;
      s->quic_tp_len = dlen;
      بشکن;
    }
    حالت CURL_SPACK_TICKET: {
      result = spack_decdata16(&pval8, &dlen, &buf, end);
      اگر(result)
        برو out;
      curlx_free(s->sdata);
      s->sdata = pval8;
      s->sdata_len = dlen;
      بشکن;
    }
    حالت CURL_SPACK_VALID_UNTIL:
      result = spack_dec64(&val64, &buf, end);
      اگر(result)
        برو out;
      s->valid_until = (curl_off_t)val64;
      بشکن;
    حالت CURL_SPACK_SECTRUST:
      s->sectrust_verified = TRUE;
      بشکن;
    پیشفرض:  /* unknown tag */
      result = CURLE_READ_ERROR;
      برو out;
    }
  }

out:
  اگر(result) {
    CURL_TRC_SSLS(data, "error unpacking data: %d", (صحیح)result);
    Curl_ssl_session_destroy(s);
  }
  وگرنه
    *ps = s;
  بازگشت result;
}

#endif /* USE_SSL && USE_SSLS_EXPORT */
