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

#यदि defined(USE_SSL) && defined(USE_SSLS_EXPORT)

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "vtls/vtls_scache.h"
#अन्तर्भाव "vtls/vtls_spack.h"
#अन्तर्भाव "curlx/strdup.h"

#परिभाषय CURL_SPACK_संस्करणम्       0x01
#परिभाषय CURL_SPACK_IETF_ID       0x02
#परिभाषय CURL_SPACK_VALID_UNTIL   0x03
#परिभाषय CURL_SPACK_TICKET        0x04
#परिभाषय CURL_SPACK_ALPN          0x05
#परिभाषय CURL_SPACK_EARLYDATA     0x06
#परिभाषय CURL_SPACK_QUICTP        0x07
#परिभाषय CURL_SPACK_SECTRUST      0x08

स्थिर CURLcode spack_enc8(संरचना dynbuf *buf, uint8_t b)
{
  प्रत्यागम curlx_dyn_addn(buf, &b, 1);
}

स्थिर CURLcode spack_dec8(uint8_t *val, ध्रुव uint8_t **src,
                           ध्रुव uint8_t *अन्तः)
{
  यदि(अन्तः - *src < 1)
    प्रत्यागम CURLE_READ_ERROR;
  *val = **src;
  *src += 1;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode spack_enc16(संरचना dynbuf *buf, uint16_t val)
{
  uint8_t nval[2];
  nval[0] = (uint8_t)(val >> 8);
  nval[1] = (uint8_t)val;
  प्रत्यागम curlx_dyn_addn(buf, nval, परिमाण(nval));
}

स्थिर CURLcode spack_dec16(uint16_t *val, ध्रुव uint8_t **src,
                            ध्रुव uint8_t *अन्तः)
{
  यदि(अन्तः - *src < 2)
    प्रत्यागम CURLE_READ_ERROR;
  *val = (uint16_t)((*src)[0] << 8 | (*src)[1]);
  *src += 2;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode spack_enc32(संरचना dynbuf *buf, uint32_t val)
{
  uint8_t nval[4];
  nval[0] = (uint8_t)(val >> 24);
  nval[1] = (uint8_t)(val >> 16);
  nval[2] = (uint8_t)(val >> 8);
  nval[3] = (uint8_t)val;
  प्रत्यागम curlx_dyn_addn(buf, nval, परिमाण(nval));
}

स्थिर CURLcode spack_dec32(uint32_t *val, ध्रुव uint8_t **src,
                            ध्रुव uint8_t *अन्तः)
{
  यदि(अन्तः - *src < 4)
    प्रत्यागम CURLE_READ_ERROR;
  *val = (uint32_t)(*src)[0] << 24 | (uint32_t)(*src)[1] << 16 |
         (uint32_t)(*src)[2] << 8 | (*src)[3];
  *src += 4;
  प्रत्यागम CURLE_OK;
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
  प्रत्यागम curlx_dyn_addn(buf, nval, परिमाण(nval));
}

स्थिर CURLcode spack_dec64(uint64_t *val, ध्रुव uint8_t **src,
                            ध्रुव uint8_t *अन्तः)
{
  यदि(अन्तः - *src < 8)
    प्रत्यागम CURLE_READ_ERROR;
  *val = (uint64_t)(*src)[0] << 56 | (uint64_t)(*src)[1] << 48 |
         (uint64_t)(*src)[2] << 40 | (uint64_t)(*src)[3] << 32 |
         (uint64_t)(*src)[4] << 24 | (uint64_t)(*src)[5] << 16 |
         (uint64_t)(*src)[6] << 8 | (*src)[7];
  *src += 8;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode spack_encstr16(संरचना dynbuf *buf, ध्रुव वर्ण *s)
{
  size_t slen = वाक्यदैर्घ्यम्(s);
  CURLcode परिणाम;
  यदि(slen > UINT16_MAX)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  परिणाम = spack_enc16(buf, (uint16_t)slen);
  यदि(!परिणाम) {
    परिणाम = curlx_dyn_addn(buf, s, slen);
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode spack_decstr16(वर्ण **val, ध्रुव uint8_t **src,
                               ध्रुव uint8_t *अन्तः)
{
  uint16_t slen;
  CURLcode परिणाम;

  *val = रिक्त;
  परिणाम = spack_dec16(&slen, src, अन्तः);
  यदि(परिणाम)
    प्रत्यागम परिणाम;
  यदि(अन्तः - *src < slen)
    प्रत्यागम CURLE_READ_ERROR;
  *val = curlx_memdup0((ध्रुव वर्ण *)(*src), slen);
  *src += slen;
  प्रत्यागम *val ? CURLE_OK : CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
}

स्थिर CURLcode spack_encdata16(संरचना dynbuf *buf, ध्रुव uint8_t *दत्त,
                                size_t दत्त_len)
{
  CURLcode परिणाम;
  यदि(दत्त_len > UINT16_MAX)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  परिणाम = spack_enc16(buf, (uint16_t)दत्त_len);
  यदि(!परिणाम) {
    परिणाम = curlx_dyn_addn(buf, दत्त, दत्त_len);
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode spack_decdata16(uint8_t **val, size_t *val_len,
                                ध्रुव uint8_t **src, ध्रुव uint8_t *अन्तः)
{
  uint16_t दत्त_len;
  CURLcode परिणाम;

  *val = रिक्त;
  परिणाम = spack_dec16(&दत्त_len, src, अन्तः);
  यदि(परिणाम)
    प्रत्यागम परिणाम;
  यदि(अन्तः - *src < दत्त_len)
    प्रत्यागम CURLE_READ_ERROR;
  *val = curlx_memdup0((ध्रुव वर्ण *)(*src), दत्त_len);
  *val_len = दत्त_len;
  *src += दत्त_len;
  प्रत्यागम *val ? CURLE_OK : CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
}

CURLcode Curl_ssl_session_pack(संरचना Curl_easy *दत्त,
                               संरचना Curl_ssl_session *s,
                               संरचना dynbuf *buf)
{
  CURLcode परिणाम;
  DEBUGASSERT(s->sdata);
  DEBUGASSERT(s->sdata_len);

  यदि(s->valid_until < 0)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  परिणाम = spack_enc8(buf, CURL_SPACK_संस्करणम्);
  यदि(!परिणाम)
    परिणाम = spack_enc8(buf, CURL_SPACK_TICKET);
  यदि(!परिणाम)
    परिणाम = spack_encdata16(buf, s->sdata, s->sdata_len);
  यदि(!परिणाम)
    परिणाम = spack_enc8(buf, CURL_SPACK_IETF_ID);
  यदि(!परिणाम)
    परिणाम = spack_enc16(buf, (uint16_t)s->ietf_tls_id);
  यदि(!परिणाम)
    परिणाम = spack_enc8(buf, CURL_SPACK_VALID_UNTIL);
  यदि(!परिणाम)
    परिणाम = spack_enc64(buf, (uint64_t)s->valid_until);
  यदि(!परिणाम && s->alpn) {
    परिणाम = spack_enc8(buf, CURL_SPACK_ALPN);
    यदि(!परिणाम)
      परिणाम = spack_encstr16(buf, s->alpn);
  }
  यदि(!परिणाम && s->earlydata_max) {
    यदि(s->earlydata_max > UINT32_MAX)
      परिणाम = CURLE_BAD_FUNCTION_तर्कः;
    यदि(!परिणाम)
      परिणाम = spack_enc8(buf, CURL_SPACK_EARLYDATA);
    यदि(!परिणाम)
      परिणाम = spack_enc32(buf, (uint32_t)s->earlydata_max);
  }
  यदि(!परिणाम && s->sectrust_verified) {
    परिणाम = spack_enc8(buf, CURL_SPACK_SECTRUST);
  }
  यदि(!परिणाम && s->quic_tp && s->quic_tp_len) {
    परिणाम = spack_enc8(buf, CURL_SPACK_QUICTP);
    यदि(!परिणाम)
      परिणाम = spack_encdata16(buf, s->quic_tp, s->quic_tp_len);
  }

  यदि(परिणाम)
    CURL_TRC_SSLS(दत्त, "error packing data: %d", (पूर्णाङ्क)परिणाम);
  प्रत्यागम परिणाम;
}

CURLcode Curl_ssl_session_unpack(संरचना Curl_easy *दत्त,
                                 ध्रुव शून्य *bufv, size_t buflen,
                                 संरचना Curl_ssl_session **ps)
{
  संरचना Curl_ssl_session *s = रिक्त;
  ध्रुव अचिह्नित वर्ण *buf = (ध्रुव अचिह्नित वर्ण *)bufv;
  ध्रुव अचिह्नित वर्ण *अन्तः = buf + buflen;
  uint8_t val8, *pval8;
  uint16_t val16;
  uint32_t val32;
  uint64_t val64;
  size_t dlen;
  CURLcode परिणाम;

  DEBUGASSERT(ps);
  DEBUGASSERT(buf);
  DEBUGASSERT(buflen);
  *ps = रिक्त;

  परिणाम = spack_dec8(&val8, &buf, अन्तः);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  यदि(val8 != CURL_SPACK_संस्करणम्) {
    परिणाम = CURLE_READ_ERROR;
    गच्छ बहिर्गमनम्;
  }

  s = curlx_calloc(1, परिमाण(*s));
  यदि(!s) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  यावत्(buf < अन्तः) {
    परिणाम = spack_dec8(&val8, &buf, अन्तः);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;

    विकल्प(val8) {
    पक्ष CURL_SPACK_ALPN:
      curlx_free(s->alpn);
      परिणाम = spack_decstr16(&s->alpn, &buf, अन्तः);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      भङ्ग;
    पक्ष CURL_SPACK_EARLYDATA:
      परिणाम = spack_dec32(&val32, &buf, अन्तः);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      s->earlydata_max = val32;
      भङ्ग;
    पक्ष CURL_SPACK_IETF_ID:
      परिणाम = spack_dec16(&val16, &buf, अन्तः);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      s->ietf_tls_id = val16;
      भङ्ग;
    पक्ष CURL_SPACK_QUICTP: {
      परिणाम = spack_decdata16(&pval8, &dlen, &buf, अन्तः);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      curlx_free(s->quic_tp);
      s->quic_tp = pval8;
      s->quic_tp_len = dlen;
      भङ्ग;
    }
    पक्ष CURL_SPACK_TICKET: {
      परिणाम = spack_decdata16(&pval8, &dlen, &buf, अन्तः);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      curlx_free(s->sdata);
      s->sdata = pval8;
      s->sdata_len = dlen;
      भङ्ग;
    }
    पक्ष CURL_SPACK_VALID_UNTIL:
      परिणाम = spack_dec64(&val64, &buf, अन्तः);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      s->valid_until = (curl_off_t)val64;
      भङ्ग;
    पक्ष CURL_SPACK_SECTRUST:
      s->sectrust_verified = TRUE;
      भङ्ग;
    स्वभाव:  /* unknown tag */
      परिणाम = CURLE_READ_ERROR;
      गच्छ बहिर्गमनम्;
    }
  }

बहिर्गमनम्:
  यदि(परिणाम) {
    CURL_TRC_SSLS(दत्त, "error unpacking data: %d", (पूर्णाङ्क)परिणाम);
    Curl_ssl_session_destroy(s);
  }
  अन्यथा
    *ps = s;
  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* USE_SSL && USE_SSLS_EXPORT */
