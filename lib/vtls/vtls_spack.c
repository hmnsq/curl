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

#وصف_ڪريو CURL_SPACK_نسخو       0x01
#وصف_ڪريو CURL_SPACK_IETF_ID       0x02
#وصف_ڪريو CURL_SPACK_VALID_UNTIL   0x03
#وصف_ڪريو CURL_SPACK_TICKET        0x04
#وصف_ڪريو CURL_SPACK_ALPN          0x05
#وصف_ڪريو CURL_SPACK_EARLYDATA     0x06
#وصف_ڪريو CURL_SPACK_QUICTP        0x07
#وصف_ڪريو CURL_SPACK_SECTRUST      0x08

ساكن CURLcode spack_enc8(بنية dynbuf *buf, uint8_t b)
{
  أرجع curlx_dyn_addn(buf, &b, 1);
}

ساكن CURLcode spack_dec8(uint8_t *val, ثابت uint8_t **src,
                           ثابت uint8_t *پڇاڙي)
{
  إذا(پڇاڙي - *src < 1)
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
                            ثابت uint8_t *پڇاڙي)
{
  إذا(پڇاڙي - *src < 2)
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
                            ثابت uint8_t *پڇاڙي)
{
  إذا(پڇاڙي - *src < 4)
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
                            ثابت uint8_t *پڇاڙي)
{
  إذا(پڇاڙي - *src < 8)
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
  size_t slen = لڙي_ڊيگهه(s);
  CURLcode نتيجو;
  إذا(slen > UINT16_MAX)
    أرجع CURLE_BAD_FUNCTION_دليل;
  نتيجو = spack_enc16(buf, (uint16_t)slen);
  إذا(!نتيجو) {
    نتيجو = curlx_dyn_addn(buf, s, slen);
  }
  أرجع نتيجو;
}

ساكن CURLcode spack_decstr16(حرف **val, ثابت uint8_t **src,
                               ثابت uint8_t *پڇاڙي)
{
  uint16_t slen;
  CURLcode نتيجو;

  *val = عدم;
  نتيجو = spack_dec16(&slen, src, پڇاڙي);
  إذا(نتيجو)
    أرجع نتيجو;
  إذا(پڇاڙي - *src < slen)
    أرجع CURLE_READ_ERROR;
  *val = curlx_memdup0((ثابت حرف *)(*src), slen);
  *src += slen;
  أرجع *val ? CURLE_OK : CURLE_ٻاهريون_جو_يادگيري;
}

ساكن CURLcode spack_encdata16(بنية dynbuf *buf, ثابت uint8_t *معلومات,
                                size_t معلومات_len)
{
  CURLcode نتيجو;
  إذا(معلومات_len > UINT16_MAX)
    أرجع CURLE_BAD_FUNCTION_دليل;
  نتيجو = spack_enc16(buf, (uint16_t)معلومات_len);
  إذا(!نتيجو) {
    نتيجو = curlx_dyn_addn(buf, معلومات, معلومات_len);
  }
  أرجع نتيجو;
}

ساكن CURLcode spack_decdata16(uint8_t **val, size_t *val_len,
                                ثابت uint8_t **src, ثابت uint8_t *پڇاڙي)
{
  uint16_t معلومات_len;
  CURLcode نتيجو;

  *val = عدم;
  نتيجو = spack_dec16(&معلومات_len, src, پڇاڙي);
  إذا(نتيجو)
    أرجع نتيجو;
  إذا(پڇاڙي - *src < معلومات_len)
    أرجع CURLE_READ_ERROR;
  *val = curlx_memdup0((ثابت حرف *)(*src), معلومات_len);
  *val_len = معلومات_len;
  *src += معلومات_len;
  أرجع *val ? CURLE_OK : CURLE_ٻاهريون_جو_يادگيري;
}

CURLcode Curl_ssl_session_pack(بنية Curl_easy *معلومات,
                               بنية Curl_ssl_session *s,
                               بنية dynbuf *buf)
{
  CURLcode نتيجو;
  DEBUGASSERT(s->sdata);
  DEBUGASSERT(s->sdata_len);

  إذا(s->valid_until < 0)
    أرجع CURLE_BAD_FUNCTION_دليل;

  نتيجو = spack_enc8(buf, CURL_SPACK_نسخو);
  إذا(!نتيجو)
    نتيجو = spack_enc8(buf, CURL_SPACK_TICKET);
  إذا(!نتيجو)
    نتيجو = spack_encdata16(buf, s->sdata, s->sdata_len);
  إذا(!نتيجو)
    نتيجو = spack_enc8(buf, CURL_SPACK_IETF_ID);
  إذا(!نتيجو)
    نتيجو = spack_enc16(buf, (uint16_t)s->ietf_tls_id);
  إذا(!نتيجو)
    نتيجو = spack_enc8(buf, CURL_SPACK_VALID_UNTIL);
  إذا(!نتيجو)
    نتيجو = spack_enc64(buf, (uint64_t)s->valid_until);
  إذا(!نتيجو && s->alpn) {
    نتيجو = spack_enc8(buf, CURL_SPACK_ALPN);
    إذا(!نتيجو)
      نتيجو = spack_encstr16(buf, s->alpn);
  }
  إذا(!نتيجو && s->earlydata_max) {
    إذا(s->earlydata_max > UINT32_MAX)
      نتيجو = CURLE_BAD_FUNCTION_دليل;
    إذا(!نتيجو)
      نتيجو = spack_enc8(buf, CURL_SPACK_EARLYDATA);
    إذا(!نتيجو)
      نتيجو = spack_enc32(buf, (uint32_t)s->earlydata_max);
  }
  إذا(!نتيجو && s->sectrust_verified) {
    نتيجو = spack_enc8(buf, CURL_SPACK_SECTRUST);
  }
  إذا(!نتيجو && s->quic_tp && s->quic_tp_len) {
    نتيجو = spack_enc8(buf, CURL_SPACK_QUICTP);
    إذا(!نتيجو)
      نتيجو = spack_encdata16(buf, s->quic_tp, s->quic_tp_len);
  }

  إذا(نتيجو)
    CURL_TRC_SSLS(معلومات, "error packing data: %d", (صحيح)نتيجو);
  أرجع نتيجو;
}

CURLcode Curl_ssl_session_unpack(بنية Curl_easy *معلومات,
                                 ثابت فراغ *bufv, size_t buflen,
                                 بنية Curl_ssl_session **ps)
{
  بنية Curl_ssl_session *s = عدم;
  ثابت بنانشان حرف *buf = (ثابت بنانشان حرف *)bufv;
  ثابت بنانشان حرف *پڇاڙي = buf + buflen;
  uint8_t val8, *pval8;
  uint16_t val16;
  uint32_t val32;
  uint64_t val64;
  size_t dlen;
  CURLcode نتيجو;

  DEBUGASSERT(ps);
  DEBUGASSERT(buf);
  DEBUGASSERT(buflen);
  *ps = عدم;

  نتيجو = spack_dec8(&val8, &buf, پڇاڙي);
  إذا(نتيجو)
    اذهب ٻاهريون;
  إذا(val8 != CURL_SPACK_نسخو) {
    نتيجو = CURLE_READ_ERROR;
    اذهب ٻاهريون;
  }

  s = curlx_calloc(1, حجم(*s));
  إذا(!s) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب ٻاهريون;
  }

  طالما(buf < پڇاڙي) {
    نتيجو = spack_dec8(&val8, &buf, پڇاڙي);
    إذا(نتيجو)
      اذهب ٻاهريون;

    بدل(val8) {
    حالة CURL_SPACK_ALPN:
      curlx_free(s->alpn);
      نتيجو = spack_decstr16(&s->alpn, &buf, پڇاڙي);
      إذا(نتيجو)
        اذهب ٻاهريون;
      اكسر;
    حالة CURL_SPACK_EARLYDATA:
      نتيجو = spack_dec32(&val32, &buf, پڇاڙي);
      إذا(نتيجو)
        اذهب ٻاهريون;
      s->earlydata_max = val32;
      اكسر;
    حالة CURL_SPACK_IETF_ID:
      نتيجو = spack_dec16(&val16, &buf, پڇاڙي);
      إذا(نتيجو)
        اذهب ٻاهريون;
      s->ietf_tls_id = val16;
      اكسر;
    حالة CURL_SPACK_QUICTP: {
      نتيجو = spack_decdata16(&pval8, &dlen, &buf, پڇاڙي);
      إذا(نتيجو)
        اذهب ٻاهريون;
      curlx_free(s->quic_tp);
      s->quic_tp = pval8;
      s->quic_tp_len = dlen;
      اكسر;
    }
    حالة CURL_SPACK_TICKET: {
      نتيجو = spack_decdata16(&pval8, &dlen, &buf, پڇاڙي);
      إذا(نتيجو)
        اذهب ٻاهريون;
      curlx_free(s->sdata);
      s->sdata = pval8;
      s->sdata_len = dlen;
      اكسر;
    }
    حالة CURL_SPACK_VALID_UNTIL:
      نتيجو = spack_dec64(&val64, &buf, پڇاڙي);
      إذا(نتيجو)
        اذهب ٻاهريون;
      s->valid_until = (curl_off_t)val64;
      اكسر;
    حالة CURL_SPACK_SECTRUST:
      s->sectrust_verified = TRUE;
      اكسر;
    افتراضي:  /* unknown tag */
      نتيجو = CURLE_READ_ERROR;
      اذهب ٻاهريون;
    }
  }

ٻاهريون:
  إذا(نتيجو) {
    CURL_TRC_SSLS(معلومات, "error unpacking data: %d", (صحيح)نتيجو);
    Curl_ssl_session_destroy(s);
  }
  وإلا
    *ps = s;
  أرجع نتيجو;
}

#جيڪڏهن_پورو /* USE_SSL && USE_SSLS_EXPORT */
