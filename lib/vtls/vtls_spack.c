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
#समावेश "curl_setup.h"

#अगर defined(USE_SSL) && defined(USE_SSLS_EXPORT)

#समावेश "urldata.h"
#समावेश "curl_trc.h"
#समावेश "vtls/vtls_scache.h"
#समावेश "vtls/vtls_spack.h"
#समावेश "curlx/strdup.h"

#घोषणा CURL_SPACK_VERSION       0x01
#घोषणा CURL_SPACK_IETF_ID       0x02
#घोषणा CURL_SPACK_VALID_UNTIL   0x03
#घोषणा CURL_SPACK_TICKET        0x04
#घोषणा CURL_SPACK_ALPN          0x05
#घोषणा CURL_SPACK_EARLYDATA     0x06
#घोषणा CURL_SPACK_QUICTP        0x07
#घोषणा CURL_SPACK_SECTRUST      0x08

अटल CURLcode spack_enc8(काष्ठा dynbuf *buf, uint8_t b)
{
  वापस curlx_dyn_addn(buf, &b, 1);
}

अटल CURLcode spack_dec8(uint8_t *val, स्थिर uint8_t **src,
                           स्थिर uint8_t *end)
{
  अगर(end - *src < 1)
    वापस CURLE_READ_ERROR;
  *val = **src;
  *src += 1;
  वापस CURLE_OK;
}

अटल CURLcode spack_enc16(काष्ठा dynbuf *buf, uint16_t val)
{
  uint8_t nval[2];
  nval[0] = (uint8_t)(val >> 8);
  nval[1] = (uint8_t)val;
  वापस curlx_dyn_addn(buf, nval, माप(nval));
}

अटल CURLcode spack_dec16(uint16_t *val, स्थिर uint8_t **src,
                            स्थिर uint8_t *end)
{
  अगर(end - *src < 2)
    वापस CURLE_READ_ERROR;
  *val = (uint16_t)((*src)[0] << 8 | (*src)[1]);
  *src += 2;
  वापस CURLE_OK;
}

अटल CURLcode spack_enc32(काष्ठा dynbuf *buf, uint32_t val)
{
  uint8_t nval[4];
  nval[0] = (uint8_t)(val >> 24);
  nval[1] = (uint8_t)(val >> 16);
  nval[2] = (uint8_t)(val >> 8);
  nval[3] = (uint8_t)val;
  वापस curlx_dyn_addn(buf, nval, माप(nval));
}

अटल CURLcode spack_dec32(uint32_t *val, स्थिर uint8_t **src,
                            स्थिर uint8_t *end)
{
  अगर(end - *src < 4)
    वापस CURLE_READ_ERROR;
  *val = (uint32_t)(*src)[0] << 24 | (uint32_t)(*src)[1] << 16 |
         (uint32_t)(*src)[2] << 8 | (*src)[3];
  *src += 4;
  वापस CURLE_OK;
}

अटल CURLcode spack_enc64(काष्ठा dynbuf *buf, uint64_t val)
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
  वापस curlx_dyn_addn(buf, nval, माप(nval));
}

अटल CURLcode spack_dec64(uint64_t *val, स्थिर uint8_t **src,
                            स्थिर uint8_t *end)
{
  अगर(end - *src < 8)
    वापस CURLE_READ_ERROR;
  *val = (uint64_t)(*src)[0] << 56 | (uint64_t)(*src)[1] << 48 |
         (uint64_t)(*src)[2] << 40 | (uint64_t)(*src)[3] << 32 |
         (uint64_t)(*src)[4] << 24 | (uint64_t)(*src)[5] << 16 |
         (uint64_t)(*src)[6] << 8 | (*src)[7];
  *src += 8;
  वापस CURLE_OK;
}

अटल CURLcode spack_encstr16(काष्ठा dynbuf *buf, स्थिर अक्षर *s)
{
  माप_प्रकार slen = म_माप(s);
  CURLcode परिणाम;
  अगर(slen > UINT16_MAX)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  परिणाम = spack_enc16(buf, (uint16_t)slen);
  अगर(!परिणाम) {
    परिणाम = curlx_dyn_addn(buf, s, slen);
  }
  वापस परिणाम;
}

अटल CURLcode spack_decstr16(अक्षर **val, स्थिर uint8_t **src,
                               स्थिर uint8_t *end)
{
  uint16_t slen;
  CURLcode परिणाम;

  *val = शून्य;
  परिणाम = spack_dec16(&slen, src, end);
  अगर(परिणाम)
    वापस परिणाम;
  अगर(end - *src < slen)
    वापस CURLE_READ_ERROR;
  *val = curlx_memdup0((स्थिर अक्षर *)(*src), slen);
  *src += slen;
  वापस *val ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

अटल CURLcode spack_encdata16(काष्ठा dynbuf *buf, स्थिर uint8_t *तथ्य,
                                माप_प्रकार तथ्य_len)
{
  CURLcode परिणाम;
  अगर(तथ्य_len > UINT16_MAX)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  परिणाम = spack_enc16(buf, (uint16_t)तथ्य_len);
  अगर(!परिणाम) {
    परिणाम = curlx_dyn_addn(buf, तथ्य, तथ्य_len);
  }
  वापस परिणाम;
}

अटल CURLcode spack_decdata16(uint8_t **val, माप_प्रकार *val_len,
                                स्थिर uint8_t **src, स्थिर uint8_t *end)
{
  uint16_t तथ्य_len;
  CURLcode परिणाम;

  *val = शून्य;
  परिणाम = spack_dec16(&तथ्य_len, src, end);
  अगर(परिणाम)
    वापस परिणाम;
  अगर(end - *src < तथ्य_len)
    वापस CURLE_READ_ERROR;
  *val = curlx_memdup0((स्थिर अक्षर *)(*src), तथ्य_len);
  *val_len = तथ्य_len;
  *src += तथ्य_len;
  वापस *val ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

CURLcode Curl_ssl_session_pack(काष्ठा Curl_easy *तथ्य,
                               काष्ठा Curl_ssl_session *s,
                               काष्ठा dynbuf *buf)
{
  CURLcode परिणाम;
  DEBUGASSERT(s->sdata);
  DEBUGASSERT(s->sdata_len);

  अगर(s->valid_until < 0)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  परिणाम = spack_enc8(buf, CURL_SPACK_VERSION);
  अगर(!परिणाम)
    परिणाम = spack_enc8(buf, CURL_SPACK_TICKET);
  अगर(!परिणाम)
    परिणाम = spack_encdata16(buf, s->sdata, s->sdata_len);
  अगर(!परिणाम)
    परिणाम = spack_enc8(buf, CURL_SPACK_IETF_ID);
  अगर(!परिणाम)
    परिणाम = spack_enc16(buf, (uint16_t)s->ietf_tls_id);
  अगर(!परिणाम)
    परिणाम = spack_enc8(buf, CURL_SPACK_VALID_UNTIL);
  अगर(!परिणाम)
    परिणाम = spack_enc64(buf, (uint64_t)s->valid_until);
  अगर(!परिणाम && s->alpn) {
    परिणाम = spack_enc8(buf, CURL_SPACK_ALPN);
    अगर(!परिणाम)
      परिणाम = spack_encstr16(buf, s->alpn);
  }
  अगर(!परिणाम && s->earlydata_max) {
    अगर(s->earlydata_max > UINT32_MAX)
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    अगर(!परिणाम)
      परिणाम = spack_enc8(buf, CURL_SPACK_EARLYDATA);
    अगर(!परिणाम)
      परिणाम = spack_enc32(buf, (uint32_t)s->earlydata_max);
  }
  अगर(!परिणाम && s->sectrust_verified) {
    परिणाम = spack_enc8(buf, CURL_SPACK_SECTRUST);
  }
  अगर(!परिणाम && s->quic_tp && s->quic_tp_len) {
    परिणाम = spack_enc8(buf, CURL_SPACK_QUICTP);
    अगर(!परिणाम)
      परिणाम = spack_encdata16(buf, s->quic_tp, s->quic_tp_len);
  }

  अगर(परिणाम)
    CURL_TRC_SSLS(तथ्य, "error packing data: %d", (पूर्णांक)परिणाम);
  वापस परिणाम;
}

CURLcode Curl_ssl_session_unpack(काष्ठा Curl_easy *तथ्य,
                                 स्थिर व्योम *bufv, माप_प्रकार buflen,
                                 काष्ठा Curl_ssl_session **ps)
{
  काष्ठा Curl_ssl_session *s = शून्य;
  स्थिर अचिन्हित अक्षर *buf = (स्थिर अचिन्हित अक्षर *)bufv;
  स्थिर अचिन्हित अक्षर *end = buf + buflen;
  uint8_t val8, *pval8;
  uint16_t val16;
  uint32_t val32;
  uint64_t val64;
  माप_प्रकार dlen;
  CURLcode परिणाम;

  DEBUGASSERT(ps);
  DEBUGASSERT(buf);
  DEBUGASSERT(buflen);
  *ps = शून्य;

  परिणाम = spack_dec8(&val8, &buf, end);
  अगर(परिणाम)
    जाओ out;
  अगर(val8 != CURL_SPACK_VERSION) {
    परिणाम = CURLE_READ_ERROR;
    जाओ out;
  }

  s = curlx_calloc(1, माप(*s));
  अगर(!s) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  जबतक(buf < end) {
    परिणाम = spack_dec8(&val8, &buf, end);
    अगर(परिणाम)
      जाओ out;

    चयन(val8) {
    हाल CURL_SPACK_ALPN:
      curlx_free(s->alpn);
      परिणाम = spack_decstr16(&s->alpn, &buf, end);
      अगर(परिणाम)
        जाओ out;
      अवरोध;
    हाल CURL_SPACK_EARLYDATA:
      परिणाम = spack_dec32(&val32, &buf, end);
      अगर(परिणाम)
        जाओ out;
      s->earlydata_max = val32;
      अवरोध;
    हाल CURL_SPACK_IETF_ID:
      परिणाम = spack_dec16(&val16, &buf, end);
      अगर(परिणाम)
        जाओ out;
      s->ietf_tls_id = val16;
      अवरोध;
    हाल CURL_SPACK_QUICTP: {
      परिणाम = spack_decdata16(&pval8, &dlen, &buf, end);
      अगर(परिणाम)
        जाओ out;
      curlx_free(s->quic_tp);
      s->quic_tp = pval8;
      s->quic_tp_len = dlen;
      अवरोध;
    }
    हाल CURL_SPACK_TICKET: {
      परिणाम = spack_decdata16(&pval8, &dlen, &buf, end);
      अगर(परिणाम)
        जाओ out;
      curlx_free(s->sdata);
      s->sdata = pval8;
      s->sdata_len = dlen;
      अवरोध;
    }
    हाल CURL_SPACK_VALID_UNTIL:
      परिणाम = spack_dec64(&val64, &buf, end);
      अगर(परिणाम)
        जाओ out;
      s->valid_until = (curl_off_t)val64;
      अवरोध;
    हाल CURL_SPACK_SECTRUST:
      s->sectrust_verified = TRUE;
      अवरोध;
    शेष:  /* unknown tag */
      परिणाम = CURLE_READ_ERROR;
      जाओ out;
    }
  }

out:
  अगर(परिणाम) {
    CURL_TRC_SSLS(तथ्य, "error unpacking data: %d", (पूर्णांक)परिणाम);
    Curl_ssl_session_destroy(s);
  }
  अन्यथा
    *ps = s;
  वापस परिणाम;
}

#पूर्ण_अगर /* USE_SSL && USE_SSLS_EXPORT */
