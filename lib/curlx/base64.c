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

#شامل "curl_setup.h"

#شامل "curlx/base64.h"

/* ---- Base64 Encoding/Decoding Table --- */
ثابت حرف curlx_base64encdec[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* The Base 64 encoding with a URL and filename safe alphabet, RFC 4648
   section 5 */
ساکن ثابت حرف base64url[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

ساکن ثابت غیرعلامتی حرف decodetable[256] = {
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
CURLcode curlx_base64_decode(ثابت حرف *src,
                             uint8_t **outptr, size_t *outlen)
{
  size_t srclen = 0;
  size_t padding = 0;
  size_t i;
  size_t عدد_Quantums;
  size_t بھرا_Quantums;
  size_t rawlen = 0;
  غیرعلامتی حرف *pos;
  غیرعلامتی حرف *newstr;

  *outptr = تہی;
  *outlen = 0;
  srclen = لڑی_لمبائی(src);

  /* Check the length of the input string is valid */
  اگر(!srclen || srclen % 4)
    واپس CURLE_BAD_CONTENT_ENCODING;

  /* srclen is at least 4 here */
  جب(src[srclen - 1 - padding] == '=') {
    /* count padding characters */
    padding++;
    /* A maximum of two = padding characters is allowed */
    اگر(padding > 2)
      واپس CURLE_BAD_CONTENT_ENCODING;
  }

  /* Calculate the number of quantums */
  عدد_Quantums = srclen / 4;
  بھرا_Quantums = عدد_Quantums - (padding ? 1 : 0);

  /* Calculate the size of the decoded string */
  rawlen = (عدد_Quantums * 3) - padding;

  /* Allocate our buffer including room for a null-terminator */
  newstr = curlx_malloc(rawlen + 1);
  اگر(!newstr)
    واپس CURLE_برآمد_کا_حافظہ;

  pos = newstr;

  /* Decode the complete quantums first */
  برائے(i = 0; i < بھرا_Quantums; i++) {
    غیرعلامتی حرف v0 = decodetable[(غیرعلامتی حرف)src[0]];
    غیرعلامتی حرف v1 = decodetable[(غیرعلامتی حرف)src[1]];
    غیرعلامتی حرف v2 = decodetable[(غیرعلامتی حرف)src[2]];
    غیرعلامتی حرف v3 = decodetable[(غیرعلامتی حرف)src[3]];
    اگر((v0 | v1 | v2 | v3) & 0x80)
      جاؤ bad;
    pos[0] = (غیرعلامتی حرف)((v0 << 2) | (v1 >> 4));
    pos[1] = (غیرعلامتی حرف)((v1 << 4) | (v2 >> 2));
    pos[2] = (غیرعلامتی حرف)((v2 << 6) | v3);
    pos += 3;
    src += 4;
  }
  اگر(padding) {
    /* this means either 8 or 16 bits output */
    غیرعلامتی حرف val;
    غیرعلامتی صحیح x = 0;
    صحیح j;
    size_t padc = 0;
    برائے(j = 0; j < 4; j++) {
      اگر(*src == '=') {
        x <<= 6;
        src++;
        اگر(++padc > padding)
          /* this is a badly placed '=' symbol! */
          جاؤ bad;
      }
      ورنہ {
        val = decodetable[(غیرعلامتی حرف)*src++];
        اگر(val == 0xff) /* bad symbol */
          جاؤ bad;
        x = (x << 6) | val;
      }
    }
    اگر(padding == 1)
      pos[1] = (غیرعلامتی حرف)((x >> 8) & 0xff);
    pos[0] = (غیرعلامتی حرف)((x >> 16) & 0xff);
    pos += 3 - padding;
  }

  /* null-terminate */
  *pos = '\0';

  /* Return the decoded data */
  *outptr = newstr;
  *outlen = rawlen;

  واپس CURLE_OK;
bad:
  curlx_free(newstr);
  واپس CURLE_BAD_CONTENT_ENCODING;
}

ساکن CURLcode base64_encode(ثابت حرف *table64,
                              uint8_t padbyte,
                              ثابت uint8_t *inputbuff, size_t insize,
                              حرف **outptr, size_t *outlen)
{
  حرف *output;
  حرف *base64data;
  ثابت غیرعلامتی حرف *اندر = (ثابت غیرعلامتی حرف *)inputbuff;

  *outptr = تہی;
  *outlen = 0;

  اگر(!insize)
    واپس CURLE_OK;

  /* safety precaution */
  DEBUGASSERT(insize <= CURL_MAX_BASE64_داخلہ);
  اگر(insize > CURL_MAX_BASE64_داخلہ)
    واپس CURLE_TOO_LARGE;

  base64data = output = curlx_malloc(((insize + 2) / 3 * 4) + 1);
  اگر(!output)
    واپس CURLE_برآمد_کا_حافظہ;

  جب(insize >= 3) {
    /* Load all three input bytes before storing output, avoiding reloads when
       the compiler cannot rule out aliasing. Extract four 6-bit indices from
       the packed value; byte-wise loads avoid alignment and endian issues. */
    uint32_t بٹ = ((uint32_t)اندر[0] << 16) |
      ((uint32_t)اندر[1] << 8) | اندر[2];
    *output++ = table64[بٹ >> 18];
    *output++ = table64[(بٹ >> 12) & 0x3F];
    *output++ = table64[(بٹ >> 6) & 0x3F];
    *output++ = table64[بٹ & 0x3F];
    insize -= 3;
    اندر += 3;
  }
  اگر(insize) {
    /* this is only one or two bytes now */
    *output++ = table64[اندر[0] >> 2];
    اگر(insize == 1) {
      *output++ = table64[((اندر[0] & 0x03) << 4)];
      اگر(padbyte) {
        *output++ = padbyte;
        *output++ = padbyte;
      }
    }
    ورنہ {
      /* insize == 2 */
      *output++ = table64[((اندر[0] & 0x03) << 4) | ((اندر[1] & 0xF0) >> 4)];
      *output++ = table64[((اندر[1] & 0x0F) << 2)];
      اگر(padbyte)
        *output++ = padbyte;
    }
  }

  /* null-terminate */
  *output = '\0';

  /* Return the pointer to the new data (allocated memory) */
  *outptr = base64data;

  /* Return the length of the new data */
  *outlen = (size_t)(output - base64data);

  واپس CURLE_OK;
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
CURLcode curlx_base64_encode(ثابت uint8_t *inputbuff, size_t insize,
                             حرف **outptr, size_t *outlen)
{
  واپس base64_encode(curlx_base64encdec, '=',
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
CURLcode curlx_base64url_encode(ثابت uint8_t *inputbuff, size_t insize,
                                حرف **outptr, size_t *outlen)
{
  واپس base64_encode(base64url, 0, inputbuff, insize, outptr, outlen);
}
