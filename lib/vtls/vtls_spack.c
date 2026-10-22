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
#அந்தர்பாவ "curl_setup.h"

#எனில் defined(USE_SSL) && defined(USE_SSLS_EXPORT)

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "vtls/vtls_scache.h"
#அந்தர்பாவ "vtls/vtls_spack.h"
#அந்தர்பாவ "curlx/strdup.h"

#define CURL_SPACK_VERSION       0x01
#define CURL_SPACK_IETF_ID       0x02
#define CURL_SPACK_VALID_UNTIL   0x03
#define CURL_SPACK_TICKET        0x04
#define CURL_SPACK_ALPN          0x05
#define CURL_SPACK_EARLYDATA     0x06
#define CURL_SPACK_QUICTP        0x07
#define CURL_SPACK_SECTRUST      0x08

நிலையான CURLcode spack_enc8(கட்டமைப்பு dynbuf *buf, uint8_t b)
{
  திரும்பு curlx_dyn_addn(buf, &b, 1);
}

நிலையான CURLcode spack_dec8(uint8_t *val, மாறிலி uint8_t **src,
                           மாறிலி uint8_t *end)
{
  எனில்(end - *src < 1)
    திரும்பு CURLE_READ_ERROR;
  *val = **src;
  *src += 1;
  திரும்பு CURLE_OK;
}

நிலையான CURLcode spack_enc16(கட்டமைப்பு dynbuf *buf, uint16_t val)
{
  uint8_t nval[2];
  nval[0] = (uint8_t)(val >> 8);
  nval[1] = (uint8_t)val;
  திரும்பு curlx_dyn_addn(buf, nval, அளவு(nval));
}

நிலையான CURLcode spack_dec16(uint16_t *val, மாறிலி uint8_t **src,
                            மாறிலி uint8_t *end)
{
  எனில்(end - *src < 2)
    திரும்பு CURLE_READ_ERROR;
  *val = (uint16_t)((*src)[0] << 8 | (*src)[1]);
  *src += 2;
  திரும்பு CURLE_OK;
}

நிலையான CURLcode spack_enc32(கட்டமைப்பு dynbuf *buf, uint32_t val)
{
  uint8_t nval[4];
  nval[0] = (uint8_t)(val >> 24);
  nval[1] = (uint8_t)(val >> 16);
  nval[2] = (uint8_t)(val >> 8);
  nval[3] = (uint8_t)val;
  திரும்பு curlx_dyn_addn(buf, nval, அளவு(nval));
}

நிலையான CURLcode spack_dec32(uint32_t *val, மாறிலி uint8_t **src,
                            மாறிலி uint8_t *end)
{
  எனில்(end - *src < 4)
    திரும்பு CURLE_READ_ERROR;
  *val = (uint32_t)(*src)[0] << 24 | (uint32_t)(*src)[1] << 16 |
         (uint32_t)(*src)[2] << 8 | (*src)[3];
  *src += 4;
  திரும்பு CURLE_OK;
}

நிலையான CURLcode spack_enc64(கட்டமைப்பு dynbuf *buf, uint64_t val)
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
  திரும்பு curlx_dyn_addn(buf, nval, அளவு(nval));
}

நிலையான CURLcode spack_dec64(uint64_t *val, மாறிலி uint8_t **src,
                            மாறிலி uint8_t *end)
{
  எனில்(end - *src < 8)
    திரும்பு CURLE_READ_ERROR;
  *val = (uint64_t)(*src)[0] << 56 | (uint64_t)(*src)[1] << 48 |
         (uint64_t)(*src)[2] << 40 | (uint64_t)(*src)[3] << 32 |
         (uint64_t)(*src)[4] << 24 | (uint64_t)(*src)[5] << 16 |
         (uint64_t)(*src)[6] << 8 | (*src)[7];
  *src += 8;
  திரும்பு CURLE_OK;
}

நிலையான CURLcode spack_encstr16(கட்டமைப்பு dynbuf *buf, மாறிலி எழுத்து *s)
{
  size_t slen = strlen(s);
  CURLcode result;
  எனில்(slen > UINT16_MAX)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  result = spack_enc16(buf, (uint16_t)slen);
  எனில்(!result) {
    result = curlx_dyn_addn(buf, s, slen);
  }
  திரும்பு result;
}

நிலையான CURLcode spack_decstr16(எழுத்து **val, மாறிலி uint8_t **src,
                               மாறிலி uint8_t *end)
{
  uint16_t slen;
  CURLcode result;

  *val = வெற்று;
  result = spack_dec16(&slen, src, end);
  எனில்(result)
    திரும்பு result;
  எனில்(end - *src < slen)
    திரும்பு CURLE_READ_ERROR;
  *val = curlx_memdup0((மாறிலி எழுத்து *)(*src), slen);
  *src += slen;
  திரும்பு *val ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

நிலையான CURLcode spack_encdata16(கட்டமைப்பு dynbuf *buf, மாறிலி uint8_t *data,
                                size_t data_len)
{
  CURLcode result;
  எனில்(data_len > UINT16_MAX)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  result = spack_enc16(buf, (uint16_t)data_len);
  எனில்(!result) {
    result = curlx_dyn_addn(buf, data, data_len);
  }
  திரும்பு result;
}

நிலையான CURLcode spack_decdata16(uint8_t **val, size_t *val_len,
                                மாறிலி uint8_t **src, மாறிலி uint8_t *end)
{
  uint16_t data_len;
  CURLcode result;

  *val = வெற்று;
  result = spack_dec16(&data_len, src, end);
  எனில்(result)
    திரும்பு result;
  எனில்(end - *src < data_len)
    திரும்பு CURLE_READ_ERROR;
  *val = curlx_memdup0((மாறிலி எழுத்து *)(*src), data_len);
  *val_len = data_len;
  *src += data_len;
  திரும்பு *val ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

CURLcode Curl_ssl_session_pack(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு Curl_ssl_session *s,
                               கட்டமைப்பு dynbuf *buf)
{
  CURLcode result;
  DEBUGASSERT(s->sdata);
  DEBUGASSERT(s->sdata_len);

  எனில்(s->valid_until < 0)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  result = spack_enc8(buf, CURL_SPACK_VERSION);
  எனில்(!result)
    result = spack_enc8(buf, CURL_SPACK_TICKET);
  எனில்(!result)
    result = spack_encdata16(buf, s->sdata, s->sdata_len);
  எனில்(!result)
    result = spack_enc8(buf, CURL_SPACK_IETF_ID);
  எனில்(!result)
    result = spack_enc16(buf, (uint16_t)s->ietf_tls_id);
  எனில்(!result)
    result = spack_enc8(buf, CURL_SPACK_VALID_UNTIL);
  எனில்(!result)
    result = spack_enc64(buf, (uint64_t)s->valid_until);
  எனில்(!result && s->alpn) {
    result = spack_enc8(buf, CURL_SPACK_ALPN);
    எனில்(!result)
      result = spack_encstr16(buf, s->alpn);
  }
  எனில்(!result && s->earlydata_max) {
    எனில்(s->earlydata_max > UINT32_MAX)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    எனில்(!result)
      result = spack_enc8(buf, CURL_SPACK_EARLYDATA);
    எனில்(!result)
      result = spack_enc32(buf, (uint32_t)s->earlydata_max);
  }
  எனில்(!result && s->sectrust_verified) {
    result = spack_enc8(buf, CURL_SPACK_SECTRUST);
  }
  எனில்(!result && s->quic_tp && s->quic_tp_len) {
    result = spack_enc8(buf, CURL_SPACK_QUICTP);
    எனில்(!result)
      result = spack_encdata16(buf, s->quic_tp, s->quic_tp_len);
  }

  எனில்(result)
    CURL_TRC_SSLS(data, "error packing data: %d", (முழு)result);
  திரும்பு result;
}

CURLcode Curl_ssl_session_unpack(கட்டமைப்பு Curl_easy *data,
                                 மாறிலி வெறுமை *bufv, size_t buflen,
                                 கட்டமைப்பு Curl_ssl_session **ps)
{
  கட்டமைப்பு Curl_ssl_session *s = வெற்று;
  மாறிலி unsigned எழுத்து *buf = (மாறிலி unsigned எழுத்து *)bufv;
  மாறிலி unsigned எழுத்து *end = buf + buflen;
  uint8_t val8, *pval8;
  uint16_t val16;
  uint32_t val32;
  uint64_t val64;
  size_t dlen;
  CURLcode result;

  DEBUGASSERT(ps);
  DEBUGASSERT(buf);
  DEBUGASSERT(buflen);
  *ps = வெற்று;

  result = spack_dec8(&val8, &buf, end);
  எனில்(result)
    செல் out;
  எனில்(val8 != CURL_SPACK_VERSION) {
    result = CURLE_READ_ERROR;
    செல் out;
  }

  s = curlx_calloc(1, அளவு(*s));
  எனில்(!s) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }

  போது(buf < end) {
    result = spack_dec8(&val8, &buf, end);
    எனில்(result)
      செல் out;

    தேர்வு(val8) {
    நிலை CURL_SPACK_ALPN:
      curlx_free(s->alpn);
      result = spack_decstr16(&s->alpn, &buf, end);
      எனில்(result)
        செல் out;
      நிறுத்து;
    நிலை CURL_SPACK_EARLYDATA:
      result = spack_dec32(&val32, &buf, end);
      எனில்(result)
        செல் out;
      s->earlydata_max = val32;
      நிறுத்து;
    நிலை CURL_SPACK_IETF_ID:
      result = spack_dec16(&val16, &buf, end);
      எனில்(result)
        செல் out;
      s->ietf_tls_id = val16;
      நிறுத்து;
    நிலை CURL_SPACK_QUICTP: {
      result = spack_decdata16(&pval8, &dlen, &buf, end);
      எனில்(result)
        செல் out;
      curlx_free(s->quic_tp);
      s->quic_tp = pval8;
      s->quic_tp_len = dlen;
      நிறுத்து;
    }
    நிலை CURL_SPACK_TICKET: {
      result = spack_decdata16(&pval8, &dlen, &buf, end);
      எனில்(result)
        செல் out;
      curlx_free(s->sdata);
      s->sdata = pval8;
      s->sdata_len = dlen;
      நிறுத்து;
    }
    நிலை CURL_SPACK_VALID_UNTIL:
      result = spack_dec64(&val64, &buf, end);
      எனில்(result)
        செல் out;
      s->valid_until = (curl_off_t)val64;
      நிறுத்து;
    நிலை CURL_SPACK_SECTRUST:
      s->sectrust_verified = TRUE;
      நிறுத்து;
    இயல்பு:  /* unknown tag */
      result = CURLE_READ_ERROR;
      செல் out;
    }
  }

out:
  எனில்(result) {
    CURL_TRC_SSLS(data, "error unpacking data: %d", (முழு)result);
    Curl_ssl_session_destroy(s);
  }
  இல்லையெனில்
    *ps = s;
  திரும்பு result;
}

#endif /* USE_SSL && USE_SSLS_EXPORT */
