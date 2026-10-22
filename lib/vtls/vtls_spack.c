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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಯದಿ defined(USE_SSL) && defined(USE_SSLS_EXPORT)

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls_scache.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls_spack.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"

#define CURL_SPACK_VERSION       0x01
#define CURL_SPACK_IETF_ID       0x02
#define CURL_SPACK_VALID_UNTIL   0x03
#define CURL_SPACK_TICKET        0x04
#define CURL_SPACK_ALPN          0x05
#define CURL_SPACK_EARLYDATA     0x06
#define CURL_SPACK_QUICTP        0x07
#define CURL_SPACK_SECTRUST      0x08

ಸ್ಥಿರ CURLcode spack_enc8(ಸಂರಚನಾ dynbuf *buf, uint8_t b)
{
  ಪ್ರತ್ಯಾಗಮ curlx_dyn_addn(buf, &b, 1);
}

ಸ್ಥಿರ CURLcode spack_dec8(uint8_t *val, ಧ್ರುವ uint8_t **src,
                           ಧ್ರುವ uint8_t *end)
{
  ಯದಿ(end - *src < 1)
    ಪ್ರತ್ಯಾಗಮ CURLE_READ_ERROR;
  *val = **src;
  *src += 1;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode spack_enc16(ಸಂರಚನಾ dynbuf *buf, uint16_t val)
{
  uint8_t nval[2];
  nval[0] = (uint8_t)(val >> 8);
  nval[1] = (uint8_t)val;
  ಪ್ರತ್ಯಾಗಮ curlx_dyn_addn(buf, nval, ಪರಿಮಾಣ(nval));
}

ಸ್ಥಿರ CURLcode spack_dec16(uint16_t *val, ಧ್ರುವ uint8_t **src,
                            ಧ್ರುವ uint8_t *end)
{
  ಯದಿ(end - *src < 2)
    ಪ್ರತ್ಯಾಗಮ CURLE_READ_ERROR;
  *val = (uint16_t)((*src)[0] << 8 | (*src)[1]);
  *src += 2;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode spack_enc32(ಸಂರಚನಾ dynbuf *buf, uint32_t val)
{
  uint8_t nval[4];
  nval[0] = (uint8_t)(val >> 24);
  nval[1] = (uint8_t)(val >> 16);
  nval[2] = (uint8_t)(val >> 8);
  nval[3] = (uint8_t)val;
  ಪ್ರತ್ಯಾಗಮ curlx_dyn_addn(buf, nval, ಪರಿಮಾಣ(nval));
}

ಸ್ಥಿರ CURLcode spack_dec32(uint32_t *val, ಧ್ರುವ uint8_t **src,
                            ಧ್ರುವ uint8_t *end)
{
  ಯದಿ(end - *src < 4)
    ಪ್ರತ್ಯಾಗಮ CURLE_READ_ERROR;
  *val = (uint32_t)(*src)[0] << 24 | (uint32_t)(*src)[1] << 16 |
         (uint32_t)(*src)[2] << 8 | (*src)[3];
  *src += 4;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode spack_enc64(ಸಂರಚನಾ dynbuf *buf, uint64_t val)
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
  ಪ್ರತ್ಯಾಗಮ curlx_dyn_addn(buf, nval, ಪರಿಮಾಣ(nval));
}

ಸ್ಥಿರ CURLcode spack_dec64(uint64_t *val, ಧ್ರುವ uint8_t **src,
                            ಧ್ರುವ uint8_t *end)
{
  ಯದಿ(end - *src < 8)
    ಪ್ರತ್ಯಾಗಮ CURLE_READ_ERROR;
  *val = (uint64_t)(*src)[0] << 56 | (uint64_t)(*src)[1] << 48 |
         (uint64_t)(*src)[2] << 40 | (uint64_t)(*src)[3] << 32 |
         (uint64_t)(*src)[4] << 24 | (uint64_t)(*src)[5] << 16 |
         (uint64_t)(*src)[6] << 8 | (*src)[7];
  *src += 8;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode spack_encstr16(ಸಂರಚನಾ dynbuf *buf, ಧ್ರುವ ವರ್ಣ *s)
{
  size_t slen = strlen(s);
  CURLcode result;
  ಯದಿ(slen > UINT16_MAX)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  result = spack_enc16(buf, (uint16_t)slen);
  ಯದಿ(!result) {
    result = curlx_dyn_addn(buf, s, slen);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode spack_decstr16(ವರ್ಣ **val, ಧ್ರುವ uint8_t **src,
                               ಧ್ರುವ uint8_t *end)
{
  uint16_t slen;
  CURLcode result;

  *val = ರಿಕ್ತ;
  result = spack_dec16(&slen, src, end);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
  ಯದಿ(end - *src < slen)
    ಪ್ರತ್ಯಾಗಮ CURLE_READ_ERROR;
  *val = curlx_memdup0((ಧ್ರುವ ವರ್ಣ *)(*src), slen);
  *src += slen;
  ಪ್ರತ್ಯಾಗಮ *val ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

ಸ್ಥಿರ CURLcode spack_encdata16(ಸಂರಚನಾ dynbuf *buf, ಧ್ರುವ uint8_t *data,
                                size_t data_len)
{
  CURLcode result;
  ಯದಿ(data_len > UINT16_MAX)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  result = spack_enc16(buf, (uint16_t)data_len);
  ಯದಿ(!result) {
    result = curlx_dyn_addn(buf, data, data_len);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode spack_decdata16(uint8_t **val, size_t *val_len,
                                ಧ್ರುವ uint8_t **src, ಧ್ರುವ uint8_t *end)
{
  uint16_t data_len;
  CURLcode result;

  *val = ರಿಕ್ತ;
  result = spack_dec16(&data_len, src, end);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
  ಯದಿ(end - *src < data_len)
    ಪ್ರತ್ಯಾಗಮ CURLE_READ_ERROR;
  *val = curlx_memdup0((ಧ್ರುವ ವರ್ಣ *)(*src), data_len);
  *val_len = data_len;
  *src += data_len;
  ಪ್ರತ್ಯಾಗಮ *val ? CURLE_OK : CURLE_OUT_OF_MEMORY;
}

CURLcode Curl_ssl_session_pack(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ Curl_ssl_session *s,
                               ಸಂರಚನಾ dynbuf *buf)
{
  CURLcode result;
  DEBUGASSERT(s->sdata);
  DEBUGASSERT(s->sdata_len);

  ಯದಿ(s->valid_until < 0)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  result = spack_enc8(buf, CURL_SPACK_VERSION);
  ಯದಿ(!result)
    result = spack_enc8(buf, CURL_SPACK_TICKET);
  ಯದಿ(!result)
    result = spack_encdata16(buf, s->sdata, s->sdata_len);
  ಯದಿ(!result)
    result = spack_enc8(buf, CURL_SPACK_IETF_ID);
  ಯದಿ(!result)
    result = spack_enc16(buf, (uint16_t)s->ietf_tls_id);
  ಯದಿ(!result)
    result = spack_enc8(buf, CURL_SPACK_VALID_UNTIL);
  ಯದಿ(!result)
    result = spack_enc64(buf, (uint64_t)s->valid_until);
  ಯದಿ(!result && s->alpn) {
    result = spack_enc8(buf, CURL_SPACK_ALPN);
    ಯದಿ(!result)
      result = spack_encstr16(buf, s->alpn);
  }
  ಯದಿ(!result && s->earlydata_max) {
    ಯದಿ(s->earlydata_max > UINT32_MAX)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಯದಿ(!result)
      result = spack_enc8(buf, CURL_SPACK_EARLYDATA);
    ಯದಿ(!result)
      result = spack_enc32(buf, (uint32_t)s->earlydata_max);
  }
  ಯದಿ(!result && s->sectrust_verified) {
    result = spack_enc8(buf, CURL_SPACK_SECTRUST);
  }
  ಯದಿ(!result && s->quic_tp && s->quic_tp_len) {
    result = spack_enc8(buf, CURL_SPACK_QUICTP);
    ಯದಿ(!result)
      result = spack_encdata16(buf, s->quic_tp, s->quic_tp_len);
  }

  ಯದಿ(result)
    CURL_TRC_SSLS(data, "error packing data: %d", (ಪೂರ್ಣಾಙ್ಕ)result);
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_ssl_session_unpack(ಸಂರಚನಾ Curl_easy *data,
                                 ಧ್ರುವ ಶೂನ್ಯ *bufv, size_t buflen,
                                 ಸಂರಚನಾ Curl_ssl_session **ps)
{
  ಸಂರಚನಾ Curl_ssl_session *s = ರಿಕ್ತ;
  ಧ್ರುವ unsigned ವರ್ಣ *buf = (ಧ್ರುವ unsigned ವರ್ಣ *)bufv;
  ಧ್ರುವ unsigned ವರ್ಣ *end = buf + buflen;
  uint8_t val8, *pval8;
  uint16_t val16;
  uint32_t val32;
  uint64_t val64;
  size_t dlen;
  CURLcode result;

  DEBUGASSERT(ps);
  DEBUGASSERT(buf);
  DEBUGASSERT(buflen);
  *ps = ರಿಕ್ತ;

  result = spack_dec8(&val8, &buf, end);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  ಯದಿ(val8 != CURL_SPACK_VERSION) {
    result = CURLE_READ_ERROR;
    ಗಚ್ಛ out;
  }

  s = curlx_calloc(1, ಪರಿಮಾಣ(*s));
  ಯದಿ(!s) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  ಯಾವತ್(buf < end) {
    result = spack_dec8(&val8, &buf, end);
    ಯದಿ(result)
      ಗಚ್ಛ out;

    ವಿಕಲ್ಪ(val8) {
    ಪಕ್ಷ CURL_SPACK_ALPN:
      curlx_free(s->alpn);
      result = spack_decstr16(&s->alpn, &buf, end);
      ಯದಿ(result)
        ಗಚ್ಛ out;
      ಭಙ್ಗ;
    ಪಕ್ಷ CURL_SPACK_EARLYDATA:
      result = spack_dec32(&val32, &buf, end);
      ಯದಿ(result)
        ಗಚ್ಛ out;
      s->earlydata_max = val32;
      ಭಙ್ಗ;
    ಪಕ್ಷ CURL_SPACK_IETF_ID:
      result = spack_dec16(&val16, &buf, end);
      ಯದಿ(result)
        ಗಚ್ಛ out;
      s->ietf_tls_id = val16;
      ಭಙ್ಗ;
    ಪಕ್ಷ CURL_SPACK_QUICTP: {
      result = spack_decdata16(&pval8, &dlen, &buf, end);
      ಯದಿ(result)
        ಗಚ್ಛ out;
      curlx_free(s->quic_tp);
      s->quic_tp = pval8;
      s->quic_tp_len = dlen;
      ಭಙ್ಗ;
    }
    ಪಕ್ಷ CURL_SPACK_TICKET: {
      result = spack_decdata16(&pval8, &dlen, &buf, end);
      ಯದಿ(result)
        ಗಚ್ಛ out;
      curlx_free(s->sdata);
      s->sdata = pval8;
      s->sdata_len = dlen;
      ಭಙ್ಗ;
    }
    ಪಕ್ಷ CURL_SPACK_VALID_UNTIL:
      result = spack_dec64(&val64, &buf, end);
      ಯದಿ(result)
        ಗಚ್ಛ out;
      s->valid_until = (curl_off_t)val64;
      ಭಙ್ಗ;
    ಪಕ್ಷ CURL_SPACK_SECTRUST:
      s->sectrust_verified = TRUE;
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:  /* unknown tag */
      result = CURLE_READ_ERROR;
      ಗಚ್ಛ out;
    }
  }

out:
  ಯದಿ(result) {
    CURL_TRC_SSLS(data, "error unpacking data: %d", (ಪೂರ್ಣಾಙ್ಕ)result);
    Curl_ssl_session_destroy(s);
  }
  ಅನ್ಯಥಾ
    *ps = s;
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* USE_SSL && USE_SSLS_EXPORT */
