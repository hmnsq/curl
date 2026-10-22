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

#जर defined(USE_SSL) && defined(USE_SSLS_EXPORT)

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
  परत curlx_dyn_addn(buf, &b, 1);
}

स्थिर CURLcode spack_dec8(uint8_t *val, स्थिरांक uint8_t **src,
                           स्थिरांक uint8_t *end)
{
  जर(end - *src < 1)
    परत CURLE_READ_ERROR;
  *val = **src;
  *src += 1;
  परत CURLE_OK;
}

स्थिर CURLcode spack_enc16(संरचना dynbuf *buf, uint16_t val)
{
  uint8_t nval[2];
  nval[0] = (uint8_t)(val >> 8);
  nval[1] = (uint8_t)val;
  परत curlx_dyn_addn(buf, nval, आकार(nval));
}

स्थिर CURLcode spack_dec16(uint16_t *val, स्थिरांक uint8_t **src,
                            स्थिरांक uint8_t *end)
{
  जर(end - *src < 2)
    परत CURLE_READ_ERROR;
  *val = (uint16_t)((*src)[0] << 8 | (*src)[1]);
  *src += 2;
  परत CURLE_OK;
}

स्थिर CURLcode spack_enc32(संरचना dynbuf *buf, uint32_t val)
{
  uint8_t nval[4];
  nval[0] = (uint8_t)(val >> 24);
  nval[1] = (uint8_t)(val >> 16);
  nval[2] = (uint8_t)(val >> 8);
  nval[3] = (uint8_t)val;
  परत curlx_dyn_addn(buf, nval, आकार(nval));
}

स्थिर CURLcode spack_dec32(uint32_t *val, स्थिरांक uint8_t **src,
                            स्थिरांक uint8_t *end)
{
  जर(end - *src < 4)
    परत CURLE_READ_ERROR;
  *val = (uint32_t)(*src)[0] << 24 | (uint32_t)(*src)[1] << 16 |
         (uint32_t)(*src)[2] << 8 | (*src)[3];
  *src += 4;
  परत CURLE_OK;
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
  परत curlx_dyn_addn(buf, nval, आकार(nval));
}

स्थिर CURLcode spack_dec64(uint64_t *val, स्थिरांक uint8_t **src,
                            स्थिरांक uint8_t *end)
{
  जर(end - *src < 8)
    परत CURLE_READ_ERROR;
  *val = (uint64_t)(*src)[0] << 56 | (uint64_t)(*src)[1] << 48 |
         (uint64_t)(*src)[2] << 40 | (uint64_t)(*src)[3] << 32 |
         (uint64_t)(*src)[4] << 24 | (uint64_t)(*src)[5] << 16 |
         (uint64_t)(*src)[6] << 8 | (*src)[7];
  *src += 8;
  परत CURLE_OK;
}

स्थिर CURLcode spack_encstr16(संरचना dynbuf *buf, स्थिरांक अक्षर *s)
{
  माप_प्रकार slen = म_माप(s);
  CURLcode result;
  जर(slen > UINT16_MAX)
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  result = spack_enc16(buf, (uint16_t)slen);
  जर(!result) {
    result = curlx_dyn_addn(buf, s, slen);
  }
  परत result;
}

स्थिर CURLcode spack_decstr16(अक्षर **val, स्थिरांक uint8_t **src,
                               स्थिरांक uint8_t *end)
{
  uint16_t slen;
  CURLcode result;

  *val = रिक्त;
  result = spack_dec16(&slen, src, end);
  जर(result)
    परत result;
  जर(end - *src < slen)
    परत CURLE_READ_ERROR;
  *val = curlx_memdup0((स्थिरांक अक्षर *)(*src), slen);
  *src += slen;
  परत *val ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

स्थिर CURLcode spack_encdata16(संरचना dynbuf *buf, स्थिरांक uint8_t *data,
                                माप_प्रकार data_len)
{
  CURLcode result;
  जर(data_len > UINT16_MAX)
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  result = spack_enc16(buf, (uint16_t)data_len);
  जर(!result) {
    result = curlx_dyn_addn(buf, data, data_len);
  }
  परत result;
}

स्थिर CURLcode spack_decdata16(uint8_t **val, माप_प्रकार *val_len,
                                स्थिरांक uint8_t **src, स्थिरांक uint8_t *end)
{
  uint16_t data_len;
  CURLcode result;

  *val = रिक्त;
  result = spack_dec16(&data_len, src, end);
  जर(result)
    परत result;
  जर(end - *src < data_len)
    परत CURLE_READ_ERROR;
  *val = curlx_memdup0((स्थिरांक अक्षर *)(*src), data_len);
  *val_len = data_len;
  *src += data_len;
  परत *val ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

CURLcode Curl_ssl_session_pack(संरचना Curl_easy *data,
                               संरचना Curl_ssl_session *s,
                               संरचना dynbuf *buf)
{
  CURLcode result;
  DEBUGASSERT(s->sdata);
  DEBUGASSERT(s->sdata_len);

  जर(s->valid_until < 0)
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  result = spack_enc8(buf, CURL_SPACK_VERSION);
  जर(!result)
    result = spack_enc8(buf, CURL_SPACK_TICKET);
  जर(!result)
    result = spack_encdata16(buf, s->sdata, s->sdata_len);
  जर(!result)
    result = spack_enc8(buf, CURL_SPACK_IETF_ID);
  जर(!result)
    result = spack_enc16(buf, (uint16_t)s->ietf_tls_id);
  जर(!result)
    result = spack_enc8(buf, CURL_SPACK_VALID_UNTIL);
  जर(!result)
    result = spack_enc64(buf, (uint64_t)s->valid_until);
  जर(!result && s->alpn) {
    result = spack_enc8(buf, CURL_SPACK_ALPN);
    जर(!result)
      result = spack_encstr16(buf, s->alpn);
  }
  जर(!result && s->earlydata_max) {
    जर(s->earlydata_max > UINT32_MAX)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    जर(!result)
      result = spack_enc8(buf, CURL_SPACK_EARLYDATA);
    जर(!result)
      result = spack_enc32(buf, (uint32_t)s->earlydata_max);
  }
  जर(!result && s->sectrust_verified) {
    result = spack_enc8(buf, CURL_SPACK_SECTRUST);
  }
  जर(!result && s->quic_tp && s->quic_tp_len) {
    result = spack_enc8(buf, CURL_SPACK_QUICTP);
    जर(!result)
      result = spack_encdata16(buf, s->quic_tp, s->quic_tp_len);
  }

  जर(result)
    CURL_TRC_SSLS(data, "error packing data: %d", (पूर्णांक)result);
  परत result;
}

CURLcode Curl_ssl_session_unpack(संरचना Curl_easy *data,
                                 स्थिरांक शून्य *bufv, माप_प्रकार buflen,
                                 संरचना Curl_ssl_session **ps)
{
  संरचना Curl_ssl_session *s = रिक्त;
  स्थिरांक अचिन्हित अक्षर *buf = (स्थिरांक अचिन्हित अक्षर *)bufv;
  स्थिरांक अचिन्हित अक्षर *end = buf + buflen;
  uint8_t val8, *pval8;
  uint16_t val16;
  uint32_t val32;
  uint64_t val64;
  माप_प्रकार dlen;
  CURLcode result;

  DEBUGASSERT(ps);
  DEBUGASSERT(buf);
  DEBUGASSERT(buflen);
  *ps = रिक्त;

  result = spack_dec8(&val8, &buf, end);
  जर(result)
    जा out;
  जर(val8 != CURL_SPACK_VERSION) {
    result = CURLE_READ_ERROR;
    जा out;
  }

  s = curlx_calloc(1, आकार(*s));
  जर(!s) {
    result = CURLE_OUT_OF_MEMORY;
    जा out;
  }

  पर्यंत(buf < end) {
    result = spack_dec8(&val8, &buf, end);
    जर(result)
      जा out;

    निवड(val8) {
    स्थिती CURL_SPACK_ALPN:
      curlx_free(s->alpn);
      result = spack_decstr16(&s->alpn, &buf, end);
      जर(result)
        जा out;
      थांबा;
    स्थिती CURL_SPACK_EARLYDATA:
      result = spack_dec32(&val32, &buf, end);
      जर(result)
        जा out;
      s->earlydata_max = val32;
      थांबा;
    स्थिती CURL_SPACK_IETF_ID:
      result = spack_dec16(&val16, &buf, end);
      जर(result)
        जा out;
      s->ietf_tls_id = val16;
      थांबा;
    स्थिती CURL_SPACK_QUICTP: {
      result = spack_decdata16(&pval8, &dlen, &buf, end);
      जर(result)
        जा out;
      curlx_free(s->quic_tp);
      s->quic_tp = pval8;
      s->quic_tp_len = dlen;
      थांबा;
    }
    स्थिती CURL_SPACK_TICKET: {
      result = spack_decdata16(&pval8, &dlen, &buf, end);
      जर(result)
        जा out;
      curlx_free(s->sdata);
      s->sdata = pval8;
      s->sdata_len = dlen;
      थांबा;
    }
    स्थिती CURL_SPACK_VALID_UNTIL:
      result = spack_dec64(&val64, &buf, end);
      जर(result)
        जा out;
      s->valid_until = (curl_off_t)val64;
      थांबा;
    स्थिती CURL_SPACK_SECTRUST:
      s->sectrust_verified = TRUE;
      थांबा;
    पूर्वनिर्धारित:  /* unknown tag */
      result = CURLE_READ_ERROR;
      जा out;
    }
  }

out:
  जर(result) {
    CURL_TRC_SSLS(data, "error unpacking data: %d", (पूर्णांक)result);
    Curl_ssl_session_destroy(s);
  }
  अन्यथा
    *ps = s;
  परत result;
}

#endif /* USE_SSL && USE_SSLS_EXPORT */
