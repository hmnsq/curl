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

#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "curlx/base64.h"

/* ---- Base64 Encoding/Decoding Table --- */
અચલ અક્ષર curlx_base64encdec[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* The Base 64 encoding with a URL and filename safe alphabet, RFC 4648
   section 5 */
સ્થિર અચલ અક્ષર base64url[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

સ્થિર અચલ unsigned અક્ષર decodetable[256] = {
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
CURLcode curlx_base64_decode(અચલ અક્ષર *src,
                             uint8_t **outptr, size_t *outlen)
{
  size_t srclen = 0;
  size_t padding = 0;
  size_t i;
  size_t અંક_Quantums;
  size_t fullQuantums;
  size_t rawlen = 0;
  unsigned અક્ષર *pos;
  unsigned અક્ષર *newstr;

  *outptr = ખાલી;
  *outlen = 0;
  srclen = strlen(src);

  /* Check the length of the input string is valid */
  જો(!srclen || srclen % 4)
    પાછા CURLE_BAD_CONTENT_ENCODING;

  /* srclen is at least 4 here */
  જ્યારે(src[srclen - 1 - padding] == '=') {
    /* count padding characters */
    padding++;
    /* A maximum of two = padding characters is allowed */
    જો(padding > 2)
      પાછા CURLE_BAD_CONTENT_ENCODING;
  }

  /* Calculate the number of quantums */
  અંક_Quantums = srclen / 4;
  fullQuantums = અંક_Quantums - (padding ? 1 : 0);

  /* Calculate the size of the decoded string */
  rawlen = (અંક_Quantums * 3) - padding;

  /* Allocate our buffer including room for a null-terminator */
  newstr = curlx_malloc(rawlen + 1);
  જો(!newstr)
    પાછા CURLE_OUT_OF_MEMORY;

  pos = newstr;

  /* Decode the complete quantums first */
  માટે(i = 0; i < fullQuantums; i++) {
    unsigned અક્ષર v0 = decodetable[(unsigned અક્ષર)src[0]];
    unsigned અક્ષર v1 = decodetable[(unsigned અક્ષર)src[1]];
    unsigned અક્ષર v2 = decodetable[(unsigned અક્ષર)src[2]];
    unsigned અક્ષર v3 = decodetable[(unsigned અક્ષર)src[3]];
    જો((v0 | v1 | v2 | v3) & 0x80)
      જાઓ bad;
    pos[0] = (unsigned અક્ષર)((v0 << 2) | (v1 >> 4));
    pos[1] = (unsigned અક્ષર)((v1 << 4) | (v2 >> 2));
    pos[2] = (unsigned અક્ષર)((v2 << 6) | v3);
    pos += 3;
    src += 4;
  }
  જો(padding) {
    /* this means either 8 or 16 bits output */
    unsigned અક્ષર val;
    unsigned પૂર્ણાંક x = 0;
    પૂર્ણાંક j;
    size_t padc = 0;
    માટે(j = 0; j < 4; j++) {
      જો(*src == '=') {
        x <<= 6;
        src++;
        જો(++padc > padding)
          /* this is a badly placed '=' symbol! */
          જાઓ bad;
      }
      નહીંતર {
        val = decodetable[(unsigned અક્ષર)*src++];
        જો(val == 0xff) /* bad symbol */
          જાઓ bad;
        x = (x << 6) | val;
      }
    }
    જો(padding == 1)
      pos[1] = (unsigned અક્ષર)((x >> 8) & 0xff);
    pos[0] = (unsigned અક્ષર)((x >> 16) & 0xff);
    pos += 3 - padding;
  }

  /* null-terminate */
  *pos = '\0';

  /* Return the decoded data */
  *outptr = newstr;
  *outlen = rawlen;

  પાછા CURLE_OK;
bad:
  curlx_free(newstr);
  પાછા CURLE_BAD_CONTENT_ENCODING;
}

સ્થિર CURLcode base64_encode(અચલ અક્ષર *table64,
                              uint8_t padbyte,
                              અચલ uint8_t *inputbuff, size_t insize,
                              અક્ષર **outptr, size_t *outlen)
{
  અક્ષર *output;
  અક્ષર *base64data;
  અચલ unsigned અક્ષર *in = (અચલ unsigned અક્ષર *)inputbuff;

  *outptr = ખાલી;
  *outlen = 0;

  જો(!insize)
    પાછા CURLE_OK;

  /* safety precaution */
  DEBUGASSERT(insize <= CURL_MAX_BASE64_આગત);
  જો(insize > CURL_MAX_BASE64_આગત)
    પાછા CURLE_TOO_LARGE;

  base64data = output = curlx_malloc(((insize + 2) / 3 * 4) + 1);
  જો(!output)
    પાછા CURLE_OUT_OF_MEMORY;

  જ્યારે(insize >= 3) {
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
  જો(insize) {
    /* this is only one or two bytes now */
    *output++ = table64[in[0] >> 2];
    જો(insize == 1) {
      *output++ = table64[((in[0] & 0x03) << 4)];
      જો(padbyte) {
        *output++ = padbyte;
        *output++ = padbyte;
      }
    }
    નહીંતર {
      /* insize == 2 */
      *output++ = table64[((in[0] & 0x03) << 4) | ((in[1] & 0xF0) >> 4)];
      *output++ = table64[((in[1] & 0x0F) << 2)];
      જો(padbyte)
        *output++ = padbyte;
    }
  }

  /* null-terminate */
  *output = '\0';

  /* Return the pointer to the new data (allocated memory) */
  *outptr = base64data;

  /* Return the length of the new data */
  *outlen = (size_t)(output - base64data);

  પાછા CURLE_OK;
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
CURLcode curlx_base64_encode(અચલ uint8_t *inputbuff, size_t insize,
                             અક્ષર **outptr, size_t *outlen)
{
  પાછા base64_encode(curlx_base64encdec, '=',
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
CURLcode curlx_base64url_encode(અચલ uint8_t *inputbuff, size_t insize,
                                અક્ષર **outptr, size_t *outlen)
{
  પાછા base64_encode(base64url, 0, inputbuff, insize, outptr, outlen);
}
