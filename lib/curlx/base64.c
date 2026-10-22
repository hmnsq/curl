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

#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "curlx/base64.h"

/* ---- Base64 Encoding/Decoding Table --- */
மாறிலி எழுத்து curlx_base64encdec[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* The Base 64 encoding with a URL and filename safe alphabet, RFC 4648
   section 5 */
நிலையான மாறிலி எழுத்து base64url[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

நிலையான மாறிலி unsigned எழுத்து decodetable[256] = {
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
CURLcode curlx_base64_decode(மாறிலி எழுத்து *src,
                             uint8_t **outptr, size_t *outlen)
{
  size_t srclen = 0;
  size_t padding = 0;
  size_t i;
  size_t இலக்கம்_Quantums;
  size_t fullQuantums;
  size_t rawlen = 0;
  unsigned எழுத்து *pos;
  unsigned எழுத்து *newstr;

  *outptr = வெற்று;
  *outlen = 0;
  srclen = strlen(src);

  /* Check the length of the input string is valid */
  எனில்(!srclen || srclen % 4)
    திரும்பு CURLE_BAD_CONTENT_ENCODING;

  /* srclen is at least 4 here */
  போது(src[srclen - 1 - padding] == '=') {
    /* count padding characters */
    padding++;
    /* A maximum of two = padding characters is allowed */
    எனில்(padding > 2)
      திரும்பு CURLE_BAD_CONTENT_ENCODING;
  }

  /* Calculate the number of quantums */
  இலக்கம்_Quantums = srclen / 4;
  fullQuantums = இலக்கம்_Quantums - (padding ? 1 : 0);

  /* Calculate the size of the decoded string */
  rawlen = (இலக்கம்_Quantums * 3) - padding;

  /* Allocate our buffer including room for a null-terminator */
  newstr = curlx_malloc(rawlen + 1);
  எனில்(!newstr)
    திரும்பு CURLE_OUT_OF_MEMORY;

  pos = newstr;

  /* Decode the complete quantums first */
  க்கு(i = 0; i < fullQuantums; i++) {
    unsigned எழுத்து v0 = decodetable[(unsigned எழுத்து)src[0]];
    unsigned எழுத்து v1 = decodetable[(unsigned எழுத்து)src[1]];
    unsigned எழுத்து v2 = decodetable[(unsigned எழுத்து)src[2]];
    unsigned எழுத்து v3 = decodetable[(unsigned எழுத்து)src[3]];
    எனில்((v0 | v1 | v2 | v3) & 0x80)
      செல் bad;
    pos[0] = (unsigned எழுத்து)((v0 << 2) | (v1 >> 4));
    pos[1] = (unsigned எழுத்து)((v1 << 4) | (v2 >> 2));
    pos[2] = (unsigned எழுத்து)((v2 << 6) | v3);
    pos += 3;
    src += 4;
  }
  எனில்(padding) {
    /* this means either 8 or 16 bits output */
    unsigned எழுத்து val;
    unsigned முழு x = 0;
    முழு j;
    size_t padc = 0;
    க்கு(j = 0; j < 4; j++) {
      எனில்(*src == '=') {
        x <<= 6;
        src++;
        எனில்(++padc > padding)
          /* this is a badly placed '=' symbol! */
          செல் bad;
      }
      இல்லையெனில் {
        val = decodetable[(unsigned எழுத்து)*src++];
        எனில்(val == 0xff) /* bad symbol */
          செல் bad;
        x = (x << 6) | val;
      }
    }
    எனில்(padding == 1)
      pos[1] = (unsigned எழுத்து)((x >> 8) & 0xff);
    pos[0] = (unsigned எழுத்து)((x >> 16) & 0xff);
    pos += 3 - padding;
  }

  /* null-terminate */
  *pos = '\0';

  /* Return the decoded data */
  *outptr = newstr;
  *outlen = rawlen;

  திரும்பு CURLE_OK;
bad:
  curlx_free(newstr);
  திரும்பு CURLE_BAD_CONTENT_ENCODING;
}

நிலையான CURLcode base64_encode(மாறிலி எழுத்து *table64,
                              uint8_t padbyte,
                              மாறிலி uint8_t *inputbuff, size_t insize,
                              எழுத்து **outptr, size_t *outlen)
{
  எழுத்து *output;
  எழுத்து *base64data;
  மாறிலி unsigned எழுத்து *in = (மாறிலி unsigned எழுத்து *)inputbuff;

  *outptr = வெற்று;
  *outlen = 0;

  எனில்(!insize)
    திரும்பு CURLE_OK;

  /* safety precaution */
  DEBUGASSERT(insize <= CURL_MAX_BASE64_உள்ளீடு);
  எனில்(insize > CURL_MAX_BASE64_உள்ளீடு)
    திரும்பு CURLE_TOO_LARGE;

  base64data = output = curlx_malloc(((insize + 2) / 3 * 4) + 1);
  எனில்(!output)
    திரும்பு CURLE_OUT_OF_MEMORY;

  போது(insize >= 3) {
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
  எனில்(insize) {
    /* this is only one or two bytes now */
    *output++ = table64[in[0] >> 2];
    எனில்(insize == 1) {
      *output++ = table64[((in[0] & 0x03) << 4)];
      எனில்(padbyte) {
        *output++ = padbyte;
        *output++ = padbyte;
      }
    }
    இல்லையெனில் {
      /* insize == 2 */
      *output++ = table64[((in[0] & 0x03) << 4) | ((in[1] & 0xF0) >> 4)];
      *output++ = table64[((in[1] & 0x0F) << 2)];
      எனில்(padbyte)
        *output++ = padbyte;
    }
  }

  /* null-terminate */
  *output = '\0';

  /* Return the pointer to the new data (allocated memory) */
  *outptr = base64data;

  /* Return the length of the new data */
  *outlen = (size_t)(output - base64data);

  திரும்பு CURLE_OK;
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
CURLcode curlx_base64_encode(மாறிலி uint8_t *inputbuff, size_t insize,
                             எழுத்து **outptr, size_t *outlen)
{
  திரும்பு base64_encode(curlx_base64encdec, '=',
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
CURLcode curlx_base64url_encode(மாறிலி uint8_t *inputbuff, size_t insize,
                                எழுத்து **outptr, size_t *outlen)
{
  திரும்பு base64_encode(base64url, 0, inputbuff, insize, outptr, outlen);
}
