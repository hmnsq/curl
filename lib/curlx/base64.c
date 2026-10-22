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

#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "curlx/base64.h"

/* ---- Base64 Encoding/Decoding Table --- */
സ്ഥിരം അക്ഷരം curlx_base64encdec[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* The Base 64 encoding with a URL and filename safe alphabet, RFC 4648
   section 5 */
സ്ഥിതം സ്ഥിരം അക്ഷരം base64url[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

സ്ഥിതം സ്ഥിരം unsigned അക്ഷരം decodetable[256] = {
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
CURLcode curlx_base64_decode(സ്ഥിരം അക്ഷരം *src,
                             uint8_t **outptr, size_t *outlen)
{
  size_t srclen = 0;
  size_t padding = 0;
  size_t i;
  size_t അക്കം_Quantums;
  size_t fullQuantums;
  size_t rawlen = 0;
  unsigned അക്ഷരം *pos;
  unsigned അക്ഷരം *newstr;

  *outptr = ശൂന്യ;
  *outlen = 0;
  srclen = strlen(src);

  /* Check the length of the input string is valid */
  എങ്കിൽ(!srclen || srclen % 4)
    തിരികെ CURLE_BAD_CONTENT_ENCODING;

  /* srclen is at least 4 here */
  ആകുമ്പോൾ(src[srclen - 1 - padding] == '=') {
    /* count padding characters */
    padding++;
    /* A maximum of two = padding characters is allowed */
    എങ്കിൽ(padding > 2)
      തിരികെ CURLE_BAD_CONTENT_ENCODING;
  }

  /* Calculate the number of quantums */
  അക്കം_Quantums = srclen / 4;
  fullQuantums = അക്കം_Quantums - (padding ? 1 : 0);

  /* Calculate the size of the decoded string */
  rawlen = (അക്കം_Quantums * 3) - padding;

  /* Allocate our buffer including room for a null-terminator */
  newstr = curlx_malloc(rawlen + 1);
  എങ്കിൽ(!newstr)
    തിരികെ CURLE_OUT_OF_MEMORY;

  pos = newstr;

  /* Decode the complete quantums first */
  വേണ്ടി(i = 0; i < fullQuantums; i++) {
    unsigned അക്ഷരം v0 = decodetable[(unsigned അക്ഷരം)src[0]];
    unsigned അക്ഷരം v1 = decodetable[(unsigned അക്ഷരം)src[1]];
    unsigned അക്ഷരം v2 = decodetable[(unsigned അക്ഷരം)src[2]];
    unsigned അക്ഷരം v3 = decodetable[(unsigned അക്ഷരം)src[3]];
    എങ്കിൽ((v0 | v1 | v2 | v3) & 0x80)
      പോകുക bad;
    pos[0] = (unsigned അക്ഷരം)((v0 << 2) | (v1 >> 4));
    pos[1] = (unsigned അക്ഷരം)((v1 << 4) | (v2 >> 2));
    pos[2] = (unsigned അക്ഷരം)((v2 << 6) | v3);
    pos += 3;
    src += 4;
  }
  എങ്കിൽ(padding) {
    /* this means either 8 or 16 bits output */
    unsigned അക്ഷരം val;
    unsigned പൂർണ്ണസംഖ്യ x = 0;
    പൂർണ്ണസംഖ്യ j;
    size_t padc = 0;
    വേണ്ടി(j = 0; j < 4; j++) {
      എങ്കിൽ(*src == '=') {
        x <<= 6;
        src++;
        എങ്കിൽ(++padc > padding)
          /* this is a badly placed '=' symbol! */
          പോകുക bad;
      }
      അല്ലെങ്കിൽ {
        val = decodetable[(unsigned അക്ഷരം)*src++];
        എങ്കിൽ(val == 0xff) /* bad symbol */
          പോകുക bad;
        x = (x << 6) | val;
      }
    }
    എങ്കിൽ(padding == 1)
      pos[1] = (unsigned അക്ഷരം)((x >> 8) & 0xff);
    pos[0] = (unsigned അക്ഷരം)((x >> 16) & 0xff);
    pos += 3 - padding;
  }

  /* null-terminate */
  *pos = '\0';

  /* Return the decoded data */
  *outptr = newstr;
  *outlen = rawlen;

  തിരികെ CURLE_OK;
bad:
  curlx_free(newstr);
  തിരികെ CURLE_BAD_CONTENT_ENCODING;
}

സ്ഥിതം CURLcode base64_encode(സ്ഥിരം അക്ഷരം *table64,
                              uint8_t padbyte,
                              സ്ഥിരം uint8_t *inputbuff, size_t insize,
                              അക്ഷരം **outptr, size_t *outlen)
{
  അക്ഷരം *output;
  അക്ഷരം *base64data;
  സ്ഥിരം unsigned അക്ഷരം *in = (സ്ഥിരം unsigned അക്ഷരം *)inputbuff;

  *outptr = ശൂന്യ;
  *outlen = 0;

  എങ്കിൽ(!insize)
    തിരികെ CURLE_OK;

  /* safety precaution */
  DEBUGASSERT(insize <= CURL_MAX_BASE64_നിവേശം);
  എങ്കിൽ(insize > CURL_MAX_BASE64_നിവേശം)
    തിരികെ CURLE_TOO_LARGE;

  base64data = output = curlx_malloc(((insize + 2) / 3 * 4) + 1);
  എങ്കിൽ(!output)
    തിരികെ CURLE_OUT_OF_MEMORY;

  ആകുമ്പോൾ(insize >= 3) {
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
  എങ്കിൽ(insize) {
    /* this is only one or two bytes now */
    *output++ = table64[in[0] >> 2];
    എങ്കിൽ(insize == 1) {
      *output++ = table64[((in[0] & 0x03) << 4)];
      എങ്കിൽ(padbyte) {
        *output++ = padbyte;
        *output++ = padbyte;
      }
    }
    അല്ലെങ്കിൽ {
      /* insize == 2 */
      *output++ = table64[((in[0] & 0x03) << 4) | ((in[1] & 0xF0) >> 4)];
      *output++ = table64[((in[1] & 0x0F) << 2)];
      എങ്കിൽ(padbyte)
        *output++ = padbyte;
    }
  }

  /* null-terminate */
  *output = '\0';

  /* Return the pointer to the new data (allocated memory) */
  *outptr = base64data;

  /* Return the length of the new data */
  *outlen = (size_t)(output - base64data);

  തിരികെ CURLE_OK;
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
CURLcode curlx_base64_encode(സ്ഥിരം uint8_t *inputbuff, size_t insize,
                             അക്ഷരം **outptr, size_t *outlen)
{
  തിരികെ base64_encode(curlx_base64encdec, '=',
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
CURLcode curlx_base64url_encode(സ്ഥിരം uint8_t *inputbuff, size_t insize,
                                അക്ഷരം **outptr, size_t *outlen)
{
  തിരികെ base64_encode(base64url, 0, inputbuff, insize, outptr, outlen);
}
