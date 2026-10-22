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

/* Base64 encoding/decoding */

#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "curlx/base64.h"

/* ---- Base64 Encoding/Decoding Table --- */
ধ্রুবক অক্ষর curlx_base64encdec[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* The Base 64 encoding with a URL and filename safe alphabet, RFC 4648
   section 5 */
স্থির ধ্রুবক অক্ষর base64url[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

স্থির ধ্রুবক unsigned অক্ষর decodetable[256] = {
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 62,   0xff, 0xff, 0xff, 63,
  52,   53,   54,   55,   56,   57,   58,   59,
  60,   61,   0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0,    1,    2,    3,    4,    5,    6,
  7,    8,    9,    10,   11,   12,   13,   14,
  15,   16,   17,   18,   19,   20,   21,   22,
  23,   24,   25,   0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 26,   27,   28,   29,   30,   31,   32,
  33,   34,   35,   36,   37,   38,   39,   40,
  41,   42,   43,   44,   45,   46,   47,   48,
  49,   50,   51,   0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};
/*
 * curlx_base64_decode()
 *
 * Given a base64 null-terminated string at src, decode it and return a
 * pointer in *outptr to a newly allocated memory area holding decoded data.
 * Size of decoded data is returned in variable pointed by outlen.
 *
 * Returns CURLE_OK on success, otherwise specific error code. Function
 * output shall not be considered valid unless CURLE_OK is returned.
 *
 * When decoded data length is 0, returns NULL in *outptr.
 *
 * @unittest: 1302
 */
CURLcode curlx_base64_decode(ধ্রুবক অক্ষর *src,
                             uint8_t **outptr, size_t *outlen)
{
  size_t srclen = 0;
  size_t padding = 0;
  size_t i;
  size_t অঙ্ক_Quantums;
  size_t fullQuantums;
  size_t rawlen = 0;
  unsigned অক্ষর *pos;
  unsigned অক্ষর *newstr;

  *outptr = ফাঁকা;
  *outlen = 0;
  srclen = strlen(src);

  /* Check the length of the input string is valid */
  যদি(!srclen || srclen % 4)
    ফেরত CURLE_BAD_CONTENT_ENCODING;

  /* srclen is at least 4 here */
  যখন(src[srclen - 1 - padding] == '=') {
    /* count padding characters */
    padding++;
    /* A maximum of two = padding characters is allowed */
    যদি(padding > 2)
      ফেরত CURLE_BAD_CONTENT_ENCODING;
  }

  /* Calculate the number of quantums */
  অঙ্ক_Quantums = srclen / 4;
  fullQuantums = অঙ্ক_Quantums - (padding ? 1 : 0);

  /* Calculate the size of the decoded string */
  rawlen = (অঙ্ক_Quantums * 3) - padding;

  /* Allocate our buffer including room for a null-terminator */
  newstr = curlx_malloc(rawlen + 1);
  যদি(!newstr)
    ফেরত CURLE_OUT_OF_MEMORY;

  pos = newstr;

  /* Decode the complete quantums first */
  জন্য(i = 0; i < fullQuantums; i++) {
    unsigned অক্ষর v0 = decodetable[(unsigned অক্ষর)src[0]];
    unsigned অক্ষর v1 = decodetable[(unsigned অক্ষর)src[1]];
    unsigned অক্ষর v2 = decodetable[(unsigned অক্ষর)src[2]];
    unsigned অক্ষর v3 = decodetable[(unsigned অক্ষর)src[3]];
    যদি((v0 | v1 | v2 | v3) & 0x80)
      যাও bad;
    pos[0] = (unsigned অক্ষর)((v0 << 2) | (v1 >> 4));
    pos[1] = (unsigned অক্ষর)((v1 << 4) | (v2 >> 2));
    pos[2] = (unsigned অক্ষর)((v2 << 6) | v3);
    pos += 3;
    src += 4;
  }
  যদি(padding) {
    /* this means either 8 or 16 bits output */
    unsigned অক্ষর val;
    unsigned পূর্ণসংখ্যা x = 0;
    পূর্ণসংখ্যা j;
    size_t padc = 0;
    জন্য(j = 0; j < 4; j++) {
      যদি(*src == '=') {
        x <<= 6;
        src++;
        যদি(++padc > padding)
          /* this is a badly placed '=' symbol! */
          যাও bad;
      }
      অন্যথা {
        val = decodetable[(unsigned অক্ষর)*src++];
        যদি(val == 0xff) /* bad symbol */
          যাও bad;
        x = (x << 6) | val;
      }
    }
    যদি(padding == 1)
      pos[1] = (unsigned অক্ষর)((x >> 8) & 0xff);
    pos[0] = (unsigned অক্ষর)((x >> 16) & 0xff);
    pos += 3 - padding;
  }

  /* null-terminate */
  *pos = '\0';

  /* Return the decoded data */
  *outptr = newstr;
  *outlen = rawlen;

  ফেরত CURLE_OK;
bad:
  curlx_free(newstr);
  ফেরত CURLE_BAD_CONTENT_ENCODING;
}

স্থির CURLcode base64_encode(ধ্রুবক অক্ষর *table64,
                              uint8_t padbyte,
                              ধ্রুবক uint8_t *inputbuff, size_t insize,
                              অক্ষর **outptr, size_t *outlen)
{
  অক্ষর *output;
  অক্ষর *base64data;
  ধ্রুবক unsigned অক্ষর *in = (ধ্রুবক unsigned অক্ষর *)inputbuff;

  *outptr = ফাঁকা;
  *outlen = 0;

  যদি(!insize)
    ফেরত CURLE_OK;

  /* safety precaution */
  DEBUGASSERT(insize <= CURL_MAX_BASE64_প্রবেশ);
  যদি(insize > CURL_MAX_BASE64_প্রবেশ)
    ফেরত CURLE_TOO_LARGE;

  base64data = output = curlx_malloc(((insize + 2) / 3 * 4) + 1);
  যদি(!output)
    ফেরত CURLE_OUT_OF_MEMORY;

  যখন(insize >= 3) {
    /* Load all three input bytes before storing output, avoiding reloads when
       the compiler cannot rule out aliasing. Extract four 6-bit indices from
       the packed value; byte-wise loads avoid alignment and endian issues. */
    uint32_t bits = ((uint32_t)in[0] << 16) |
      ((uint32_t)in[1] << 8) | in[2];
    *output++ = table64[bits >> 18];
    *output++ = table64[(bits >> 12) & 0x3F];
    *output++ = table64[(bits >> 6) & 0x3F];
    *output++ = table64[bits & 0x3F];
    insize -= 3;
    in += 3;
  }
  যদি(insize) {
    /* this is only one or two bytes now */
    *output++ = table64[in[0] >> 2];
    যদি(insize == 1) {
      *output++ = table64[((in[0] & 0x03) << 4)];
      যদি(padbyte) {
        *output++ = padbyte;
        *output++ = padbyte;
      }
    }
    অন্যথা {
      /* insize == 2 */
      *output++ = table64[((in[0] & 0x03) << 4) | ((in[1] & 0xF0) >> 4)];
      *output++ = table64[((in[1] & 0x0F) << 2)];
      যদি(padbyte)
        *output++ = padbyte;
    }
  }

  /* null-terminate */
  *output = '\0';

  /* Return the pointer to the new data (allocated memory) */
  *outptr = base64data;

  /* Return the length of the new data */
  *outlen = (size_t)(output - base64data);

  ফেরত CURLE_OK;
}

/*
 * curlx_base64_encode()
 *
 * Given a pointer to an input buffer and an input size, encode it and
 * return a pointer in *outptr to a newly allocated memory area holding
 * encoded data. Size of encoded data is returned in variable pointed by
 * outlen.
 *
 * Returns CURLE_OK on success, otherwise specific error code. Function
 * output shall not be considered valid unless CURLE_OK is returned.
 *
 * @unittest: 1302
 */
CURLcode curlx_base64_encode(ধ্রুবক uint8_t *inputbuff, size_t insize,
                             অক্ষর **outptr, size_t *outlen)
{
  ফেরত base64_encode(curlx_base64encdec, '=',
                       inputbuff, insize, outptr, outlen);
}

/*
 * curlx_base64url_encode()
 *
 * Given a pointer to an input buffer and an input size, encode it and
 * return a pointer in *outptr to a newly allocated memory area holding
 * encoded data. Size of encoded data is returned in variable pointed by
 * outlen.
 *
 * Input length of 0 indicates input buffer holds a null-terminated string.
 *
 * Returns CURLE_OK on success, otherwise specific error code. Function
 * output shall not be considered valid unless CURLE_OK is returned.
 *
 * @unittest: 1302
 */
CURLcode curlx_base64url_encode(ধ্রুবক uint8_t *inputbuff, size_t insize,
                                অক্ষর **outptr, size_t *outlen)
{
  ফেরত base64_encode(base64url, 0, inputbuff, insize, outptr, outlen);
}
