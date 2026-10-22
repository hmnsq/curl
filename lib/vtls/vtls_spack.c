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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ defined(USE_SSL) && defined(USE_SSLS_EXPORT)

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "vtls/vtls_scache.h"
#അന്തര്ഭാവ "vtls/vtls_spack.h"
#അന്തര്ഭാവ "curlx/strdup.h"

#define CURL_SPACK_VERSION       0x01
#define CURL_SPACK_IETF_ID       0x02
#define CURL_SPACK_VALID_UNTIL   0x03
#define CURL_SPACK_TICKET        0x04
#define CURL_SPACK_ALPN          0x05
#define CURL_SPACK_EARLYDATA     0x06
#define CURL_SPACK_QUICTP        0x07
#define CURL_SPACK_SECTRUST      0x08

സ്ഥിതം CURLcode spack_enc8(ഘടന dynbuf *buf, uint8_t b)
{
  തിരികെ curlx_dyn_addn(buf, &b, 1);
}

സ്ഥിതം CURLcode spack_dec8(uint8_t *val, സ്ഥിരം uint8_t **src,
                           സ്ഥിരം uint8_t *end)
{
  എങ്കിൽ(end - *src < 1)
    തിരികെ CURLE_READ_ERROR;
  *val = **src;
  *src += 1;
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode spack_enc16(ഘടന dynbuf *buf, uint16_t val)
{
  uint8_t nval[2];
  nval[0] = (uint8_t)(val >> 8);
  nval[1] = (uint8_t)val;
  തിരികെ curlx_dyn_addn(buf, nval, വലുപ്പം(nval));
}

സ്ഥിതം CURLcode spack_dec16(uint16_t *val, സ്ഥിരം uint8_t **src,
                            സ്ഥിരം uint8_t *end)
{
  എങ്കിൽ(end - *src < 2)
    തിരികെ CURLE_READ_ERROR;
  *val = (uint16_t)((*src)[0] << 8 | (*src)[1]);
  *src += 2;
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode spack_enc32(ഘടന dynbuf *buf, uint32_t val)
{
  uint8_t nval[4];
  nval[0] = (uint8_t)(val >> 24);
  nval[1] = (uint8_t)(val >> 16);
  nval[2] = (uint8_t)(val >> 8);
  nval[3] = (uint8_t)val;
  തിരികെ curlx_dyn_addn(buf, nval, വലുപ്പം(nval));
}

സ്ഥിതം CURLcode spack_dec32(uint32_t *val, സ്ഥിരം uint8_t **src,
                            സ്ഥിരം uint8_t *end)
{
  എങ്കിൽ(end - *src < 4)
    തിരികെ CURLE_READ_ERROR;
  *val = (uint32_t)(*src)[0] << 24 | (uint32_t)(*src)[1] << 16 |
         (uint32_t)(*src)[2] << 8 | (*src)[3];
  *src += 4;
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode spack_enc64(ഘടന dynbuf *buf, uint64_t val)
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
  തിരികെ curlx_dyn_addn(buf, nval, വലുപ്പം(nval));
}

സ്ഥിതം CURLcode spack_dec64(uint64_t *val, സ്ഥിരം uint8_t **src,
                            സ്ഥിരം uint8_t *end)
{
  എങ്കിൽ(end - *src < 8)
    തിരികെ CURLE_READ_ERROR;
  *val = (uint64_t)(*src)[0] << 56 | (uint64_t)(*src)[1] << 48 |
         (uint64_t)(*src)[2] << 40 | (uint64_t)(*src)[3] << 32 |
         (uint64_t)(*src)[4] << 24 | (uint64_t)(*src)[5] << 16 |
         (uint64_t)(*src)[6] << 8 | (*src)[7];
  *src += 8;
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode spack_encstr16(ഘടന dynbuf *buf, സ്ഥിരം അക്ഷരം *s)
{
  size_t slen = strlen(s);
  CURLcode result;
  എങ്കിൽ(slen > UINT16_MAX)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  result = spack_enc16(buf, (uint16_t)slen);
  എങ്കിൽ(!result) {
    result = curlx_dyn_addn(buf, s, slen);
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode spack_decstr16(അക്ഷരം **val, സ്ഥിരം uint8_t **src,
                               സ്ഥിരം uint8_t *end)
{
  uint16_t slen;
  CURLcode result;

  *val = ശൂന്യ;
  result = spack_dec16(&slen, src, end);
  എങ്കിൽ(result)
    തിരികെ result;
  എങ്കിൽ(end - *src < slen)
    തിരികെ CURLE_READ_ERROR;
  *val = curlx_memdup0((സ്ഥിരം അക്ഷരം *)(*src), slen);
  *src += slen;
  തിരികെ *val ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

സ്ഥിതം CURLcode spack_encdata16(ഘടന dynbuf *buf, സ്ഥിരം uint8_t *data,
                                size_t data_len)
{
  CURLcode result;
  എങ്കിൽ(data_len > UINT16_MAX)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  result = spack_enc16(buf, (uint16_t)data_len);
  എങ്കിൽ(!result) {
    result = curlx_dyn_addn(buf, data, data_len);
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode spack_decdata16(uint8_t **val, size_t *val_len,
                                സ്ഥിരം uint8_t **src, സ്ഥിരം uint8_t *end)
{
  uint16_t data_len;
  CURLcode result;

  *val = ശൂന്യ;
  result = spack_dec16(&data_len, src, end);
  എങ്കിൽ(result)
    തിരികെ result;
  എങ്കിൽ(end - *src < data_len)
    തിരികെ CURLE_READ_ERROR;
  *val = curlx_memdup0((സ്ഥിരം അക്ഷരം *)(*src), data_len);
  *val_len = data_len;
  *src += data_len;
  തിരികെ *val ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

CURLcode Curl_ssl_session_pack(ഘടന Curl_easy *data,
                               ഘടന Curl_ssl_session *s,
                               ഘടന dynbuf *buf)
{
  CURLcode result;
  DEBUGASSERT(s->sdata);
  DEBUGASSERT(s->sdata_len);

  എങ്കിൽ(s->valid_until < 0)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  result = spack_enc8(buf, CURL_SPACK_VERSION);
  എങ്കിൽ(!result)
    result = spack_enc8(buf, CURL_SPACK_TICKET);
  എങ്കിൽ(!result)
    result = spack_encdata16(buf, s->sdata, s->sdata_len);
  എങ്കിൽ(!result)
    result = spack_enc8(buf, CURL_SPACK_IETF_ID);
  എങ്കിൽ(!result)
    result = spack_enc16(buf, (uint16_t)s->ietf_tls_id);
  എങ്കിൽ(!result)
    result = spack_enc8(buf, CURL_SPACK_VALID_UNTIL);
  എങ്കിൽ(!result)
    result = spack_enc64(buf, (uint64_t)s->valid_until);
  എങ്കിൽ(!result && s->alpn) {
    result = spack_enc8(buf, CURL_SPACK_ALPN);
    എങ്കിൽ(!result)
      result = spack_encstr16(buf, s->alpn);
  }
  എങ്കിൽ(!result && s->earlydata_max) {
    എങ്കിൽ(s->earlydata_max > UINT32_MAX)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    എങ്കിൽ(!result)
      result = spack_enc8(buf, CURL_SPACK_EARLYDATA);
    എങ്കിൽ(!result)
      result = spack_enc32(buf, (uint32_t)s->earlydata_max);
  }
  എങ്കിൽ(!result && s->sectrust_verified) {
    result = spack_enc8(buf, CURL_SPACK_SECTRUST);
  }
  എങ്കിൽ(!result && s->quic_tp && s->quic_tp_len) {
    result = spack_enc8(buf, CURL_SPACK_QUICTP);
    എങ്കിൽ(!result)
      result = spack_encdata16(buf, s->quic_tp, s->quic_tp_len);
  }

  എങ്കിൽ(result)
    CURL_TRC_SSLS(data, "error packing data: %d", (പൂർണ്ണസംഖ്യ)result);
  തിരികെ result;
}

CURLcode Curl_ssl_session_unpack(ഘടന Curl_easy *data,
                                 സ്ഥിരം ശൂന്യം *bufv, size_t buflen,
                                 ഘടന Curl_ssl_session **ps)
{
  ഘടന Curl_ssl_session *s = ശൂന്യ;
  സ്ഥിരം unsigned അക്ഷരം *buf = (സ്ഥിരം unsigned അക്ഷരം *)bufv;
  സ്ഥിരം unsigned അക്ഷരം *end = buf + buflen;
  uint8_t val8, *pval8;
  uint16_t val16;
  uint32_t val32;
  uint64_t val64;
  size_t dlen;
  CURLcode result;

  DEBUGASSERT(ps);
  DEBUGASSERT(buf);
  DEBUGASSERT(buflen);
  *ps = ശൂന്യ;

  result = spack_dec8(&val8, &buf, end);
  എങ്കിൽ(result)
    പോകുക out;
  എങ്കിൽ(val8 != CURL_SPACK_VERSION) {
    result = CURLE_READ_ERROR;
    പോകുക out;
  }

  s = curlx_calloc(1, വലുപ്പം(*s));
  എങ്കിൽ(!s) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }

  ആകുമ്പോൾ(buf < end) {
    result = spack_dec8(&val8, &buf, end);
    എങ്കിൽ(result)
      പോകുക out;

    തിരഞ്ഞെടുപ്പ്(val8) {
    അവസ്ഥ CURL_SPACK_ALPN:
      curlx_free(s->alpn);
      result = spack_decstr16(&s->alpn, &buf, end);
      എങ്കിൽ(result)
        പോകുക out;
      നിർത്തുക;
    അവസ്ഥ CURL_SPACK_EARLYDATA:
      result = spack_dec32(&val32, &buf, end);
      എങ്കിൽ(result)
        പോകുക out;
      s->earlydata_max = val32;
      നിർത്തുക;
    അവസ്ഥ CURL_SPACK_IETF_ID:
      result = spack_dec16(&val16, &buf, end);
      എങ്കിൽ(result)
        പോകുക out;
      s->ietf_tls_id = val16;
      നിർത്തുക;
    അവസ്ഥ CURL_SPACK_QUICTP: {
      result = spack_decdata16(&pval8, &dlen, &buf, end);
      എങ്കിൽ(result)
        പോകുക out;
      curlx_free(s->quic_tp);
      s->quic_tp = pval8;
      s->quic_tp_len = dlen;
      നിർത്തുക;
    }
    അവസ്ഥ CURL_SPACK_TICKET: {
      result = spack_decdata16(&pval8, &dlen, &buf, end);
      എങ്കിൽ(result)
        പോകുക out;
      curlx_free(s->sdata);
      s->sdata = pval8;
      s->sdata_len = dlen;
      നിർത്തുക;
    }
    അവസ്ഥ CURL_SPACK_VALID_UNTIL:
      result = spack_dec64(&val64, &buf, end);
      എങ്കിൽ(result)
        പോകുക out;
      s->valid_until = (curl_off_t)val64;
      നിർത്തുക;
    അവസ്ഥ CURL_SPACK_SECTRUST:
      s->sectrust_verified = TRUE;
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:  /* unknown tag */
      result = CURLE_READ_ERROR;
      പോകുക out;
    }
  }

out:
  എങ്കിൽ(result) {
    CURL_TRC_SSLS(data, "error unpacking data: %d", (പൂർണ്ണസംഖ്യ)result);
    Curl_ssl_session_destroy(s);
  }
  അല്ലെങ്കിൽ
    *ps = s;
  തിരികെ result;
}

#endif /* USE_SSL && USE_SSLS_EXPORT */
