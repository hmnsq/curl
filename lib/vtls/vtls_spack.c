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

#إذا defined(USE_SSL) && defined(USE_SSLS_EXPORT)

#ضمن "urldata.h"
#ضمن "curl_trc.h"
#ضمن "vtls/vtls_scache.h"
#ضمن "vtls/vtls_spack.h"
#ضمن "curlx/strdup.h"

#تعریف_کن CURL_SPACK_نسخه       0x01
#تعریف_کن CURL_SPACK_IETF_ID       0x02
#تعریف_کن CURL_SPACK_VALID_UNTIL   0x03
#تعریف_کن CURL_SPACK_TICKET        0x04
#تعریف_کن CURL_SPACK_ALPN          0x05
#تعریف_کن CURL_SPACK_EARLYDATA     0x06
#تعریف_کن CURL_SPACK_QUICTP        0x07
#تعریف_کن CURL_SPACK_SECTRUST      0x08

ساكن CURLcode spack_enc8(بنية dynbuf *buf, uint8_t b)
{
  أرجع curlx_dyn_addn(buf, &b, 1);
}

ساكن CURLcode spack_dec8(uint8_t *val, ثابت uint8_t **src,
                           ثابت uint8_t *انتها)
{
  إذا(انتها - *src < 1)
    أرجع CURLE_READ_ERROR;
  *val = **src;
  *src += 1;
  أرجع CURLE_OK;
}

ساكن CURLcode spack_enc16(بنية dynbuf *buf, uint16_t val)
{
  uint8_t nval[2];
  nval[0] = (uint8_t)(val >> 8);
  nval[1] = (uint8_t)val;
  أرجع curlx_dyn_addn(buf, nval, حجم(nval));
}

ساكن CURLcode spack_dec16(uint16_t *val, ثابت uint8_t **src,
                            ثابت uint8_t *انتها)
{
  إذا(انتها - *src < 2)
    أرجع CURLE_READ_ERROR;
  *val = (uint16_t)((*src)[0] << 8 | (*src)[1]);
  *src += 2;
  أرجع CURLE_OK;
}

ساكن CURLcode spack_enc32(بنية dynbuf *buf, uint32_t val)
{
  uint8_t nval[4];
  nval[0] = (uint8_t)(val >> 24);
  nval[1] = (uint8_t)(val >> 16);
  nval[2] = (uint8_t)(val >> 8);
  nval[3] = (uint8_t)val;
  أرجع curlx_dyn_addn(buf, nval, حجم(nval));
}

ساكن CURLcode spack_dec32(uint32_t *val, ثابت uint8_t **src,
                            ثابت uint8_t *انتها)
{
  إذا(انتها - *src < 4)
    أرجع CURLE_READ_ERROR;
  *val = (uint32_t)(*src)[0] << 24 | (uint32_t)(*src)[1] << 16 |
         (uint32_t)(*src)[2] << 8 | (*src)[3];
  *src += 4;
  أرجع CURLE_OK;
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
  أرجع curlx_dyn_addn(buf, nval, حجم(nval));
}

ساكن CURLcode spack_dec64(uint64_t *val, ثابت uint8_t **src,
                            ثابت uint8_t *انتها)
{
  إذا(انتها - *src < 8)
    أرجع CURLE_READ_ERROR;
  *val = (uint64_t)(*src)[0] << 56 | (uint64_t)(*src)[1] << 48 |
         (uint64_t)(*src)[2] << 40 | (uint64_t)(*src)[3] << 32 |
         (uint64_t)(*src)[4] << 24 | (uint64_t)(*src)[5] << 16 |
         (uint64_t)(*src)[6] << 8 | (*src)[7];
  *src += 8;
  أرجع CURLE_OK;
}

ساكن CURLcode spack_encstr16(بنية dynbuf *buf, ثابت حرف *s)
{
  size_t slen = طول_رشته(s);
  CURLcode نتیجه;
  إذا(slen > UINT16_MAX)
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  نتیجه = spack_enc16(buf, (uint16_t)slen);
  إذا(!نتیجه) {
    نتیجه = curlx_dyn_addn(buf, s, slen);
  }
  أرجع نتیجه;
}

ساكن CURLcode spack_decstr16(حرف **val, ثابت uint8_t **src,
                               ثابت uint8_t *انتها)
{
  uint16_t slen;
  CURLcode نتیجه;

  *val = عدم;
  نتیجه = spack_dec16(&slen, src, انتها);
  إذا(نتیجه)
    أرجع نتیجه;
  إذا(انتها - *src < slen)
    أرجع CURLE_READ_ERROR;
  *val = curlx_memdup0((ثابت حرف *)(*src), slen);
  *src += slen;
  أرجع *val ? CURLE_OK : CURLE_بیرون_از_حافظه;
}

ساكن CURLcode spack_encdata16(بنية dynbuf *buf, ثابت uint8_t *داده,
                                size_t داده_len)
{
  CURLcode نتیجه;
  إذا(داده_len > UINT16_MAX)
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  نتیجه = spack_enc16(buf, (uint16_t)داده_len);
  إذا(!نتیجه) {
    نتیجه = curlx_dyn_addn(buf, داده, داده_len);
  }
  أرجع نتیجه;
}

ساكن CURLcode spack_decdata16(uint8_t **val, size_t *val_len,
                                ثابت uint8_t **src, ثابت uint8_t *انتها)
{
  uint16_t داده_len;
  CURLcode نتیجه;

  *val = عدم;
  نتیجه = spack_dec16(&داده_len, src, انتها);
  إذا(نتیجه)
    أرجع نتیجه;
  إذا(انتها - *src < داده_len)
    أرجع CURLE_READ_ERROR;
  *val = curlx_memdup0((ثابت حرف *)(*src), داده_len);
  *val_len = داده_len;
  *src += داده_len;
  أرجع *val ? CURLE_OK : CURLE_بیرون_از_حافظه;
}

CURLcode Curl_ssl_session_pack(بنية Curl_easy *داده,
                               بنية Curl_ssl_session *s,
                               بنية dynbuf *buf)
{
  CURLcode نتیجه;
  DEBUGASSERT(s->sdata);
  DEBUGASSERT(s->sdata_len);

  إذا(s->valid_until < 0)
    أرجع CURLE_BAD_FUNCTION_آرگومان;

  نتیجه = spack_enc8(buf, CURL_SPACK_نسخه);
  إذا(!نتیجه)
    نتیجه = spack_enc8(buf, CURL_SPACK_TICKET);
  إذا(!نتیجه)
    نتیجه = spack_encdata16(buf, s->sdata, s->sdata_len);
  إذا(!نتیجه)
    نتیجه = spack_enc8(buf, CURL_SPACK_IETF_ID);
  إذا(!نتیجه)
    نتیجه = spack_enc16(buf, (uint16_t)s->ietf_tls_id);
  إذا(!نتیجه)
    نتیجه = spack_enc8(buf, CURL_SPACK_VALID_UNTIL);
  إذا(!نتیجه)
    نتیجه = spack_enc64(buf, (uint64_t)s->valid_until);
  إذا(!نتیجه && s->alpn) {
    نتیجه = spack_enc8(buf, CURL_SPACK_ALPN);
    إذا(!نتیجه)
      نتیجه = spack_encstr16(buf, s->alpn);
  }
  إذا(!نتیجه && s->earlydata_max) {
    إذا(s->earlydata_max > UINT32_MAX)
      نتیجه = CURLE_BAD_FUNCTION_آرگومان;
    إذا(!نتیجه)
      نتیجه = spack_enc8(buf, CURL_SPACK_EARLYDATA);
    إذا(!نتیجه)
      نتیجه = spack_enc32(buf, (uint32_t)s->earlydata_max);
  }
  إذا(!نتیجه && s->sectrust_verified) {
    نتیجه = spack_enc8(buf, CURL_SPACK_SECTRUST);
  }
  إذا(!نتیجه && s->quic_tp && s->quic_tp_len) {
    نتیجه = spack_enc8(buf, CURL_SPACK_QUICTP);
    إذا(!نتیجه)
      نتیجه = spack_encdata16(buf, s->quic_tp, s->quic_tp_len);
  }

  إذا(نتیجه)
    CURL_TRC_SSLS(داده, "error packing data: %d", (صحيح)نتیجه);
  أرجع نتیجه;
}

CURLcode Curl_ssl_session_unpack(بنية Curl_easy *داده,
                                 ثابت فراغ *bufv, size_t buflen,
                                 بنية Curl_ssl_session **ps)
{
  بنية Curl_ssl_session *s = عدم;
  ثابت بیعلامت حرف *buf = (ثابت بیعلامت حرف *)bufv;
  ثابت بیعلامت حرف *انتها = buf + buflen;
  uint8_t val8, *pval8;
  uint16_t val16;
  uint32_t val32;
  uint64_t val64;
  size_t dlen;
  CURLcode نتیجه;

  DEBUGASSERT(ps);
  DEBUGASSERT(buf);
  DEBUGASSERT(buflen);
  *ps = عدم;

  نتیجه = spack_dec8(&val8, &buf, انتها);
  إذا(نتیجه)
    اذهب بیرون;
  إذا(val8 != CURL_SPACK_نسخه) {
    نتیجه = CURLE_READ_ERROR;
    اذهب بیرون;
  }

  s = curlx_calloc(1, حجم(*s));
  إذا(!s) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  طالما(buf < انتها) {
    نتیجه = spack_dec8(&val8, &buf, انتها);
    إذا(نتیجه)
      اذهب بیرون;

    بدل(val8) {
    حالة CURL_SPACK_ALPN:
      curlx_free(s->alpn);
      نتیجه = spack_decstr16(&s->alpn, &buf, انتها);
      إذا(نتیجه)
        اذهب بیرون;
      اكسر;
    حالة CURL_SPACK_EARLYDATA:
      نتیجه = spack_dec32(&val32, &buf, انتها);
      إذا(نتیجه)
        اذهب بیرون;
      s->earlydata_max = val32;
      اكسر;
    حالة CURL_SPACK_IETF_ID:
      نتیجه = spack_dec16(&val16, &buf, انتها);
      إذا(نتیجه)
        اذهب بیرون;
      s->ietf_tls_id = val16;
      اكسر;
    حالة CURL_SPACK_QUICTP: {
      نتیجه = spack_decdata16(&pval8, &dlen, &buf, انتها);
      إذا(نتیجه)
        اذهب بیرون;
      curlx_free(s->quic_tp);
      s->quic_tp = pval8;
      s->quic_tp_len = dlen;
      اكسر;
    }
    حالة CURL_SPACK_TICKET: {
      نتیجه = spack_decdata16(&pval8, &dlen, &buf, انتها);
      إذا(نتیجه)
        اذهب بیرون;
      curlx_free(s->sdata);
      s->sdata = pval8;
      s->sdata_len = dlen;
      اكسر;
    }
    حالة CURL_SPACK_VALID_UNTIL:
      نتیجه = spack_dec64(&val64, &buf, انتها);
      إذا(نتیجه)
        اذهب بیرون;
      s->valid_until = (curl_off_t)val64;
      اكسر;
    حالة CURL_SPACK_SECTRUST:
      s->sectrust_verified = TRUE;
      اكسر;
    افتراضي:  /* unknown tag */
      نتیجه = CURLE_READ_ERROR;
      اذهب بیرون;
    }
  }

بیرون:
  إذا(نتیجه) {
    CURL_TRC_SSLS(داده, "error unpacking data: %d", (صحيح)نتیجه);
    Curl_ssl_session_destroy(s);
  }
  وإلا
    *ps = s;
  أرجع نتیجه;
}

#پایان_اگر /* USE_SSL && USE_SSLS_EXPORT */
