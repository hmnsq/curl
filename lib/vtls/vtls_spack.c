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

#सचे defined(USE_SSL) && defined(USE_SSLS_EXPORT)

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "vtls/vtls_scache.h"
#अन्तर्भाव "vtls/vtls_spack.h"
#अन्तर्भाव "curlx/strdup.h"

#define CURL_SPACK_VERSION       0x01
#define CURL_SPACK_IETF_ID       0x02
#define CURL_SPACK_VALID_UNTIL   0x03
#define CURL_SPACK_TICKET        0x04
#define CURL_SPACK_ALPN          0x05
#define CURL_SPACK_EARLYDATA     0x06
#define CURL_SPACK_QUICTP        0x07
#define CURL_SPACK_SECTRUST      0x08

स्थिर CURLcode spack_enc8(संरचना dynbuf *buf, uint8_t b)
{
  पटिनिवत्त curlx_dyn_addn(buf, &b, 1);
}

स्थिर CURLcode spack_dec8(uint8_t *val, ध्रुव uint8_t **src,
                           ध्रुव uint8_t *end)
{
  सचे(end - *src < 1)
    पटिनिवत्त CURLE_READ_ERROR;
  *val = **src;
  *src += 1;
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode spack_enc16(संरचना dynbuf *buf, uint16_t val)
{
  uint8_t nval[2];
  nval[0] = (uint8_t)(val >> 8);
  nval[1] = (uint8_t)val;
  पटिनिवत्त curlx_dyn_addn(buf, nval, परिमाण(nval));
}

स्थिर CURLcode spack_dec16(uint16_t *val, ध्रुव uint8_t **src,
                            ध्रुव uint8_t *end)
{
  सचे(end - *src < 2)
    पटिनिवत्त CURLE_READ_ERROR;
  *val = (uint16_t)((*src)[0] << 8 | (*src)[1]);
  *src += 2;
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode spack_enc32(संरचना dynbuf *buf, uint32_t val)
{
  uint8_t nval[4];
  nval[0] = (uint8_t)(val >> 24);
  nval[1] = (uint8_t)(val >> 16);
  nval[2] = (uint8_t)(val >> 8);
  nval[3] = (uint8_t)val;
  पटिनिवत्त curlx_dyn_addn(buf, nval, परिमाण(nval));
}

स्थिर CURLcode spack_dec32(uint32_t *val, ध्रुव uint8_t **src,
                            ध्रुव uint8_t *end)
{
  सचे(end - *src < 4)
    पटिनिवत्त CURLE_READ_ERROR;
  *val = (uint32_t)(*src)[0] << 24 | (uint32_t)(*src)[1] << 16 |
         (uint32_t)(*src)[2] << 8 | (*src)[3];
  *src += 4;
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode spack_enc64(संरचना dynbuf *buf, uint64_t val)
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
  पटिनिवत्त curlx_dyn_addn(buf, nval, परिमाण(nval));
}

स्थिर CURLcode spack_dec64(uint64_t *val, ध्रुव uint8_t **src,
                            ध्रुव uint8_t *end)
{
  सचे(end - *src < 8)
    पटिनिवत्त CURLE_READ_ERROR;
  *val = (uint64_t)(*src)[0] << 56 | (uint64_t)(*src)[1] << 48 |
         (uint64_t)(*src)[2] << 40 | (uint64_t)(*src)[3] << 32 |
         (uint64_t)(*src)[4] << 24 | (uint64_t)(*src)[5] << 16 |
         (uint64_t)(*src)[6] << 8 | (*src)[7];
  *src += 8;
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode spack_encstr16(संरचना dynbuf *buf, ध्रुव वर्ण *s)
{
  size_t slen = strlen(s);
  CURLcode result;
  सचे(slen > UINT16_MAX)
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;
  result = spack_enc16(buf, (uint16_t)slen);
  सचे(!result) {
    result = curlx_dyn_addn(buf, s, slen);
  }
  पटिनिवत्त result;
}

स्थिर CURLcode spack_decstr16(वर्ण **val, ध्रुव uint8_t **src,
                               ध्रुव uint8_t *end)
{
  uint16_t slen;
  CURLcode result;

  *val = रिक्त;
  result = spack_dec16(&slen, src, end);
  सचे(result)
    पटिनिवत्त result;
  सचे(end - *src < slen)
    पटिनिवत्त CURLE_READ_ERROR;
  *val = curlx_memdup0((ध्रुव वर्ण *)(*src), slen);
  *src += slen;
  पटिनिवत्त *val ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

स्थिर CURLcode spack_encdata16(संरचना dynbuf *buf, ध्रुव uint8_t *data,
                                size_t data_len)
{
  CURLcode result;
  सचे(data_len > UINT16_MAX)
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;
  result = spack_enc16(buf, (uint16_t)data_len);
  सचे(!result) {
    result = curlx_dyn_addn(buf, data, data_len);
  }
  पटिनिवत्त result;
}

स्थिर CURLcode spack_decdata16(uint8_t **val, size_t *val_len,
                                ध्रुव uint8_t **src, ध्रुव uint8_t *end)
{
  uint16_t data_len;
  CURLcode result;

  *val = रिक्त;
  result = spack_dec16(&data_len, src, end);
  सचे(result)
    पटिनिवत्त result;
  सचे(end - *src < data_len)
    पटिनिवत्त CURLE_READ_ERROR;
  *val = curlx_memdup0((ध्रुव वर्ण *)(*src), data_len);
  *val_len = data_len;
  *src += data_len;
  पटिनिवत्त *val ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

CURLcode Curl_ssl_session_pack(संरचना Curl_easy *data,
                               संरचना Curl_ssl_session *s,
                               संरचना dynbuf *buf)
{
  CURLcode result;
  DEBUGASSERT(s->sdata);
  DEBUGASSERT(s->sdata_len);

  सचे(s->valid_until < 0)
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;

  result = spack_enc8(buf, CURL_SPACK_VERSION);
  सचे(!result)
    result = spack_enc8(buf, CURL_SPACK_TICKET);
  सचे(!result)
    result = spack_encdata16(buf, s->sdata, s->sdata_len);
  सचे(!result)
    result = spack_enc8(buf, CURL_SPACK_IETF_ID);
  सचे(!result)
    result = spack_enc16(buf, (uint16_t)s->ietf_tls_id);
  सचे(!result)
    result = spack_enc8(buf, CURL_SPACK_VALID_UNTIL);
  सचे(!result)
    result = spack_enc64(buf, (uint64_t)s->valid_until);
  सचे(!result && s->alpn) {
    result = spack_enc8(buf, CURL_SPACK_ALPN);
    सचे(!result)
      result = spack_encstr16(buf, s->alpn);
  }
  सचे(!result && s->earlydata_max) {
    सचे(s->earlydata_max > UINT32_MAX)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    सचे(!result)
      result = spack_enc8(buf, CURL_SPACK_EARLYDATA);
    सचे(!result)
      result = spack_enc32(buf, (uint32_t)s->earlydata_max);
  }
  सचे(!result && s->sectrust_verified) {
    result = spack_enc8(buf, CURL_SPACK_SECTRUST);
  }
  सचे(!result && s->quic_tp && s->quic_tp_len) {
    result = spack_enc8(buf, CURL_SPACK_QUICTP);
    सचे(!result)
      result = spack_encdata16(buf, s->quic_tp, s->quic_tp_len);
  }

  सचे(result)
    CURL_TRC_SSLS(data, "error packing data: %d", (पूर्णाङ्क)result);
  पटिनिवत्त result;
}

CURLcode Curl_ssl_session_unpack(संरचना Curl_easy *data,
                                 ध्रुव शून्य *bufv, size_t buflen,
                                 संरचना Curl_ssl_session **ps)
{
  संरचना Curl_ssl_session *s = रिक्त;
  ध्रुव unsigned वर्ण *buf = (ध्रुव unsigned वर्ण *)bufv;
  ध्रुव unsigned वर्ण *end = buf + buflen;
  uint8_t val8, *pval8;
  uint16_t val16;
  uint32_t val32;
  uint64_t val64;
  size_t dlen;
  CURLcode result;

  DEBUGASSERT(ps);
  DEBUGASSERT(buf);
  DEBUGASSERT(buflen);
  *ps = रिक्त;

  result = spack_dec8(&val8, &buf, end);
  सचे(result)
    गच्छ out;
  सचे(val8 != CURL_SPACK_VERSION) {
    result = CURLE_READ_ERROR;
    गच्छ out;
  }

  s = curlx_calloc(1, परिमाण(*s));
  सचे(!s) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }

  याव(buf < end) {
    result = spack_dec8(&val8, &buf, end);
    सचे(result)
      गच्छ out;

    विकल्प(val8) {
    पक्ष CURL_SPACK_ALPN:
      curlx_free(s->alpn);
      result = spack_decstr16(&s->alpn, &buf, end);
      सचे(result)
        गच्छ out;
      भङ्ग;
    पक्ष CURL_SPACK_EARLYDATA:
      result = spack_dec32(&val32, &buf, end);
      सचे(result)
        गच्छ out;
      s->earlydata_max = val32;
      भङ्ग;
    पक्ष CURL_SPACK_IETF_ID:
      result = spack_dec16(&val16, &buf, end);
      सचे(result)
        गच्छ out;
      s->ietf_tls_id = val16;
      भङ्ग;
    पक्ष CURL_SPACK_QUICTP: {
      result = spack_decdata16(&pval8, &dlen, &buf, end);
      सचे(result)
        गच्छ out;
      curlx_free(s->quic_tp);
      s->quic_tp = pval8;
      s->quic_tp_len = dlen;
      भङ्ग;
    }
    पक्ष CURL_SPACK_TICKET: {
      result = spack_decdata16(&pval8, &dlen, &buf, end);
      सचे(result)
        गच्छ out;
      curlx_free(s->sdata);
      s->sdata = pval8;
      s->sdata_len = dlen;
      भङ्ग;
    }
    पक्ष CURL_SPACK_VALID_UNTIL:
      result = spack_dec64(&val64, &buf, end);
      सचे(result)
        गच्छ out;
      s->valid_until = (curl_off_t)val64;
      भङ्ग;
    पक्ष CURL_SPACK_SECTRUST:
      s->sectrust_verified = TRUE;
      भङ्ग;
    स्वभाव:  /* unknown tag */
      result = CURLE_READ_ERROR;
      गच्छ out;
    }
  }

out:
  सचे(result) {
    CURL_TRC_SSLS(data, "error unpacking data: %d", (पूर्णाङ्क)result);
    Curl_ssl_session_destroy(s);
  }
  अञ्ञथा
    *ps = s;
  पटिनिवत्त result;
}

#endif /* USE_SSL && USE_SSLS_EXPORT */
