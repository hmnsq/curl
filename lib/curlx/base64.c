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

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/base64.h"

/* ---- Base64 Encoding/Decoding Table --- */
ਅਚਲ ਅੱਖਰ curlx_base64encdec[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* The Base 64 encoding with a URL and filename safe alphabet, RFC 4648
   section 5 */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ base64url[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

ਸਥਿਰ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ decodetable[256] = {
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
CURLcode curlx_base64_decode(ਅਚਲ ਅੱਖਰ *src,
                             uint8_t **outptr, size_t *outlen)
{
  size_t srclen = 0;
  size_t padding = 0;
  size_t i;
  size_t ਅੰਕ_Quantums;
  size_t ਭਰਿਆ_Quantums;
  size_t rawlen = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *pos;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *newstr;

  *outptr = ਨਿਰਾ;
  *outlen = 0;
  srclen = ਲੜੀ_ਲੰਬਾਈ(src);

  /* Check the length of the input string is valid */
  ਜੇ(!srclen || srclen % 4)
    ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;

  /* srclen is at least 4 here */
  ਜਦੋਂ(src[srclen - 1 - padding] == '=') {
    /* count padding characters */
    padding++;
    /* A maximum of two = padding characters is allowed */
    ਜੇ(padding > 2)
      ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
  }

  /* Calculate the number of quantums */
  ਅੰਕ_Quantums = srclen / 4;
  ਭਰਿਆ_Quantums = ਅੰਕ_Quantums - (padding ? 1 : 0);

  /* Calculate the size of the decoded string */
  rawlen = (ਅੰਕ_Quantums * 3) - padding;

  /* Allocate our buffer including room for a null-terminator */
  newstr = curlx_malloc(rawlen + 1);
  ਜੇ(!newstr)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  pos = newstr;

  /* Decode the complete quantums first */
  ਲਈ(i = 0; i < ਭਰਿਆ_Quantums; i++) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ v0 = decodetable[(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)src[0]];
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ v1 = decodetable[(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)src[1]];
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ v2 = decodetable[(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)src[2]];
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ v3 = decodetable[(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)src[3]];
    ਜੇ((v0 | v1 | v2 | v3) & 0x80)
      ਜਾਓ bad;
    pos[0] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)((v0 << 2) | (v1 >> 4));
    pos[1] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)((v1 << 4) | (v2 >> 2));
    pos[2] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)((v2 << 6) | v3);
    pos += 3;
    src += 4;
  }
  ਜੇ(padding) {
    /* this means either 8 or 16 bits output */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ val;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ x = 0;
    ਪੂਰਨ j;
    size_t padc = 0;
    ਲਈ(j = 0; j < 4; j++) {
      ਜੇ(*src == '=') {
        x <<= 6;
        src++;
        ਜੇ(++padc > padding)
          /* this is a badly placed '=' symbol! */
          ਜਾਓ bad;
      }
      ਨਹੀਂਤਾਂ {
        val = decodetable[(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)*src++];
        ਜੇ(val == 0xff) /* bad symbol */
          ਜਾਓ bad;
        x = (x << 6) | val;
      }
    }
    ਜੇ(padding == 1)
      pos[1] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)((x >> 8) & 0xff);
    pos[0] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)((x >> 16) & 0xff);
    pos += 3 - padding;
  }

  /* null-terminate */
  *pos = '\0';

  /* Return the decoded data */
  *outptr = newstr;
  *outlen = rawlen;

  ਵਾਪਸ CURLE_OK;
bad:
  curlx_free(newstr);
  ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;
}

ਸਥਿਰ CURLcode base64_encode(ਅਚਲ ਅੱਖਰ *table64,
                              uint8_t padbyte,
                              ਅਚਲ uint8_t *inputbuff, size_t insize,
                              ਅੱਖਰ **outptr, size_t *outlen)
{
  ਅੱਖਰ *output;
  ਅੱਖਰ *base64data;
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਵਿੱਚ = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)inputbuff;

  *outptr = ਨਿਰਾ;
  *outlen = 0;

  ਜੇ(!insize)
    ਵਾਪਸ CURLE_OK;

  /* safety precaution */
  DEBUGASSERT(insize <= CURL_MAX_BASE64_ਨਿਵੇਸ਼);
  ਜੇ(insize > CURL_MAX_BASE64_ਨਿਵੇਸ਼)
    ਵਾਪਸ CURLE_TOO_LARGE;

  base64data = output = curlx_malloc(((insize + 2) / 3 * 4) + 1);
  ਜੇ(!output)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਜਦੋਂ(insize >= 3) {
    /* Load all three input bytes before storing output, avoiding reloads when
       the compiler cannot rule out aliasing. Extract four 6-bit indices from
       the packed value; byte-wise loads avoid alignment and endian issues. */
    uint32_t ਬਿੱਟ = ((uint32_t)ਵਿੱਚ[0] << 16) |
      ((uint32_t)ਵਿੱਚ[1] << 8) | ਵਿੱਚ[2];
    *output++ = table64[ਬਿੱਟ >> 18];
    *output++ = table64[(ਬਿੱਟ >> 12) & 0x3F];
    *output++ = table64[(ਬਿੱਟ >> 6) & 0x3F];
    *output++ = table64[ਬਿੱਟ & 0x3F];
    insize -= 3;
    ਵਿੱਚ += 3;
  }
  ਜੇ(insize) {
    /* this is only one or two bytes now */
    *output++ = table64[ਵਿੱਚ[0] >> 2];
    ਜੇ(insize == 1) {
      *output++ = table64[((ਵਿੱਚ[0] & 0x03) << 4)];
      ਜੇ(padbyte) {
        *output++ = padbyte;
        *output++ = padbyte;
      }
    }
    ਨਹੀਂਤਾਂ {
      /* insize == 2 */
      *output++ = table64[((ਵਿੱਚ[0] & 0x03) << 4) | ((ਵਿੱਚ[1] & 0xF0) >> 4)];
      *output++ = table64[((ਵਿੱਚ[1] & 0x0F) << 2)];
      ਜੇ(padbyte)
        *output++ = padbyte;
    }
  }

  /* null-terminate */
  *output = '\0';

  /* Return the pointer to the new data (allocated memory) */
  *outptr = base64data;

  /* Return the length of the new data */
  *outlen = (size_t)(output - base64data);

  ਵਾਪਸ CURLE_OK;
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
CURLcode curlx_base64_encode(ਅਚਲ uint8_t *inputbuff, size_t insize,
                             ਅੱਖਰ **outptr, size_t *outlen)
{
  ਵਾਪਸ base64_encode(curlx_base64encdec, '=',
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
CURLcode curlx_base64url_encode(ਅਚਲ uint8_t *inputbuff, size_t insize,
                                ਅੱਖਰ **outptr, size_t *outlen)
{
  ਵਾਪਸ base64_encode(base64url, 0, inputbuff, insize, outptr, outlen);
}
