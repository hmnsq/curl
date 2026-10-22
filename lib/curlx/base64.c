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

#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "curlx/base64.h"

/* ---- Base64 Encoding/Decoding Table --- */
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ curlx_base64encdec[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* The Base 64 encoding with a URL and filename safe alphabet, RFC 4648
   section 5 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ base64url[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ decodetable[256] = {
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
CURLcode curlx_base64_decode(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src,
                             uint8_t **outptr, size_t *outlen)
{
  size_t srclen = 0;
  size_t padding = 0;
  size_t i;
  size_t ಅಂಕೆ_Quantums;
  size_t ತುಂಬಿದ_Quantums;
  size_t rawlen = 0;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *pos;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *newstr;

  *outptr = ಖಾಲಿ;
  *outlen = 0;
  srclen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(src);

  /* Check the length of the input string is valid */
  ಆದರೆ(!srclen || srclen % 4)
    ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;

  /* srclen is at least 4 here */
  ಆಗ(src[srclen - 1 - padding] == '=') {
    /* count padding characters */
    padding++;
    /* A maximum of two = padding characters is allowed */
    ಆದರೆ(padding > 2)
      ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
  }

  /* Calculate the number of quantums */
  ಅಂಕೆ_Quantums = srclen / 4;
  ತುಂಬಿದ_Quantums = ಅಂಕೆ_Quantums - (padding ? 1 : 0);

  /* Calculate the size of the decoded string */
  rawlen = (ಅಂಕೆ_Quantums * 3) - padding;

  /* Allocate our buffer including room for a null-terminator */
  newstr = curlx_malloc(rawlen + 1);
  ಆದರೆ(!newstr)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  pos = newstr;

  /* Decode the complete quantums first */
  ಗಾಗಿ(i = 0; i < ತುಂಬಿದ_Quantums; i++) {
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ v0 = decodetable[(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)src[0]];
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ v1 = decodetable[(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)src[1]];
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ v2 = decodetable[(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)src[2]];
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ v3 = decodetable[(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)src[3]];
    ಆದರೆ((v0 | v1 | v2 | v3) & 0x80)
      ಹೋಗು bad;
    pos[0] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)((v0 << 2) | (v1 >> 4));
    pos[1] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)((v1 << 4) | (v2 >> 2));
    pos[2] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)((v2 << 6) | v3);
    pos += 3;
    src += 4;
  }
  ಆದರೆ(padding) {
    /* this means either 8 or 16 bits output */
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ val;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ x = 0;
    ಪೂರ್ಣಾಂಕ j;
    size_t padc = 0;
    ಗಾಗಿ(j = 0; j < 4; j++) {
      ಆದರೆ(*src == '=') {
        x <<= 6;
        src++;
        ಆದರೆ(++padc > padding)
          /* this is a badly placed '=' symbol! */
          ಹೋಗು bad;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        val = decodetable[(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)*src++];
        ಆದರೆ(val == 0xff) /* bad symbol */
          ಹೋಗು bad;
        x = (x << 6) | val;
      }
    }
    ಆದರೆ(padding == 1)
      pos[1] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)((x >> 8) & 0xff);
    pos[0] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)((x >> 16) & 0xff);
    pos += 3 - padding;
  }

  /* null-terminate */
  *pos = '\0';

  /* Return the decoded data */
  *outptr = newstr;
  *outlen = rawlen;

  ಹಿಂತಿರುಗಿ CURLE_OK;
bad:
  curlx_free(newstr);
  ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;
}

ಸ್ಥಿರ CURLcode base64_encode(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *table64,
                              uint8_t padbyte,
                              ಸ್ಥಿರಾಂಕ uint8_t *inputbuff, size_t insize,
                              ಅಕ್ಷರ **outptr, size_t *outlen)
{
  ಅಕ್ಷರ *output;
  ಅಕ್ಷರ *base64data;
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಒಳ = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)inputbuff;

  *outptr = ಖಾಲಿ;
  *outlen = 0;

  ಆದರೆ(!insize)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  /* safety precaution */
  DEBUGASSERT(insize <= CURL_MAX_BASE64_ಒಳಹರಿವು);
  ಆದರೆ(insize > CURL_MAX_BASE64_ಒಳಹರಿವು)
    ಹಿಂತಿರುಗಿ CURLE_TOO_LARGE;

  base64data = output = curlx_malloc(((insize + 2) / 3 * 4) + 1);
  ಆದರೆ(!output)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಆಗ(insize >= 3) {
    /* Load all three input bytes before storing output, avoiding reloads when
       the compiler cannot rule out aliasing. Extract four 6-bit indices from
       the packed value; byte-wise loads avoid alignment and endian issues. */
    uint32_t ಬಿಟ್ಟುಗಳು = ((uint32_t)ಒಳ[0] << 16) |
      ((uint32_t)ಒಳ[1] << 8) | ಒಳ[2];
    *output++ = table64[ಬಿಟ್ಟುಗಳು >> 18];
    *output++ = table64[(ಬಿಟ್ಟುಗಳು >> 12) & 0x3F];
    *output++ = table64[(ಬಿಟ್ಟುಗಳು >> 6) & 0x3F];
    *output++ = table64[ಬಿಟ್ಟುಗಳು & 0x3F];
    insize -= 3;
    ಒಳ += 3;
  }
  ಆದರೆ(insize) {
    /* this is only one or two bytes now */
    *output++ = table64[ಒಳ[0] >> 2];
    ಆದರೆ(insize == 1) {
      *output++ = table64[((ಒಳ[0] & 0x03) << 4)];
      ಆದರೆ(padbyte) {
        *output++ = padbyte;
        *output++ = padbyte;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* insize == 2 */
      *output++ = table64[((ಒಳ[0] & 0x03) << 4) | ((ಒಳ[1] & 0xF0) >> 4)];
      *output++ = table64[((ಒಳ[1] & 0x0F) << 2)];
      ಆದರೆ(padbyte)
        *output++ = padbyte;
    }
  }

  /* null-terminate */
  *output = '\0';

  /* Return the pointer to the new data (allocated memory) */
  *outptr = base64data;

  /* Return the length of the new data */
  *outlen = (size_t)(output - base64data);

  ಹಿಂತಿರುಗಿ CURLE_OK;
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
CURLcode curlx_base64_encode(ಸ್ಥಿರಾಂಕ uint8_t *inputbuff, size_t insize,
                             ಅಕ್ಷರ **outptr, size_t *outlen)
{
  ಹಿಂತಿರುಗಿ base64_encode(curlx_base64encdec, '=',
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
CURLcode curlx_base64url_encode(ಸ್ಥಿರಾಂಕ uint8_t *inputbuff, size_t insize,
                                ಅಕ್ಷರ **outptr, size_t *outlen)
{
  ಹಿಂತಿರುಗಿ base64_encode(base64url, 0, inputbuff, insize, outptr, outlen);
}
