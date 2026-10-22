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

#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "curlx/base64.h"

/* ---- Base64 Encoding/Decoding Table --- */
స్థిరరాశి అక్షరం curlx_base64encdec[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* The Base 64 encoding with a URL and filename safe alphabet, RFC 4648
   section 5 */
స్థిర స్థిరరాశి అక్షరం base64url[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

స్థిర స్థిరరాశి గుర్తులేని అక్షరం decodetable[256] = {
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
CURLcode curlx_base64_decode(స్థిరరాశి అక్షరం *src,
                             uint8_t **outptr, size_t *outlen)
{
  size_t srclen = 0;
  size_t padding = 0;
  size_t i;
  size_t అంకె_Quantums;
  size_t నిండిన_Quantums;
  size_t rawlen = 0;
  గుర్తులేని అక్షరం *pos;
  గుర్తులేని అక్షరం *newstr;

  *outptr = ఖాళీ;
  *outlen = 0;
  srclen = అక్షరశ్రేణి_పొడవు(src);

  /* Check the length of the input string is valid */
  ఒకవేళ(!srclen || srclen % 4)
    తిరిగి CURLE_BAD_CONTENT_ENCODING;

  /* srclen is at least 4 here */
  అయితే(src[srclen - 1 - padding] == '=') {
    /* count padding characters */
    padding++;
    /* A maximum of two = padding characters is allowed */
    ఒకవేళ(padding > 2)
      తిరిగి CURLE_BAD_CONTENT_ENCODING;
  }

  /* Calculate the number of quantums */
  అంకె_Quantums = srclen / 4;
  నిండిన_Quantums = అంకె_Quantums - (padding ? 1 : 0);

  /* Calculate the size of the decoded string */
  rawlen = (అంకె_Quantums * 3) - padding;

  /* Allocate our buffer including room for a null-terminator */
  newstr = curlx_malloc(rawlen + 1);
  ఒకవేళ(!newstr)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  pos = newstr;

  /* Decode the complete quantums first */
  కొరకు(i = 0; i < నిండిన_Quantums; i++) {
    గుర్తులేని అక్షరం v0 = decodetable[(గుర్తులేని అక్షరం)src[0]];
    గుర్తులేని అక్షరం v1 = decodetable[(గుర్తులేని అక్షరం)src[1]];
    గుర్తులేని అక్షరం v2 = decodetable[(గుర్తులేని అక్షరం)src[2]];
    గుర్తులేని అక్షరం v3 = decodetable[(గుర్తులేని అక్షరం)src[3]];
    ఒకవేళ((v0 | v1 | v2 | v3) & 0x80)
      వెళ్ళు bad;
    pos[0] = (గుర్తులేని అక్షరం)((v0 << 2) | (v1 >> 4));
    pos[1] = (గుర్తులేని అక్షరం)((v1 << 4) | (v2 >> 2));
    pos[2] = (గుర్తులేని అక్షరం)((v2 << 6) | v3);
    pos += 3;
    src += 4;
  }
  ఒకవేళ(padding) {
    /* this means either 8 or 16 bits output */
    గుర్తులేని అక్షరం val;
    గుర్తులేని పూర్ణాంకం x = 0;
    పూర్ణాంకం j;
    size_t padc = 0;
    కొరకు(j = 0; j < 4; j++) {
      ఒకవేళ(*src == '=') {
        x <<= 6;
        src++;
        ఒకవేళ(++padc > padding)
          /* this is a badly placed '=' symbol! */
          వెళ్ళు bad;
      }
      లేకపోతే {
        val = decodetable[(గుర్తులేని అక్షరం)*src++];
        ఒకవేళ(val == 0xff) /* bad symbol */
          వెళ్ళు bad;
        x = (x << 6) | val;
      }
    }
    ఒకవేళ(padding == 1)
      pos[1] = (గుర్తులేని అక్షరం)((x >> 8) & 0xff);
    pos[0] = (గుర్తులేని అక్షరం)((x >> 16) & 0xff);
    pos += 3 - padding;
  }

  /* null-terminate */
  *pos = '\0';

  /* Return the decoded data */
  *outptr = newstr;
  *outlen = rawlen;

  తిరిగి CURLE_OK;
bad:
  curlx_free(newstr);
  తిరిగి CURLE_BAD_CONTENT_ENCODING;
}

స్థిర CURLcode base64_encode(స్థిరరాశి అక్షరం *table64,
                              uint8_t padbyte,
                              స్థిరరాశి uint8_t *inputbuff, size_t insize,
                              అక్షరం **outptr, size_t *outlen)
{
  అక్షరం *output;
  అక్షరం *base64data;
  స్థిరరాశి గుర్తులేని అక్షరం *లోపల = (స్థిరరాశి గుర్తులేని అక్షరం *)inputbuff;

  *outptr = ఖాళీ;
  *outlen = 0;

  ఒకవేళ(!insize)
    తిరిగి CURLE_OK;

  /* safety precaution */
  DEBUGASSERT(insize <= CURL_MAX_BASE64_ఆదానం);
  ఒకవేళ(insize > CURL_MAX_BASE64_ఆదానం)
    తిరిగి CURLE_TOO_LARGE;

  base64data = output = curlx_malloc(((insize + 2) / 3 * 4) + 1);
  ఒకవేళ(!output)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  అయితే(insize >= 3) {
    /* Load all three input bytes before storing output, avoiding reloads when
       the compiler cannot rule out aliasing. Extract four 6-bit indices from
       the packed value; byte-wise loads avoid alignment and endian issues. */
    uint32_t బిట్లు = ((uint32_t)లోపల[0] << 16) |
      ((uint32_t)లోపల[1] << 8) | లోపల[2];
    *output++ = table64[బిట్లు >> 18];
    *output++ = table64[(బిట్లు >> 12) & 0x3F];
    *output++ = table64[(బిట్లు >> 6) & 0x3F];
    *output++ = table64[బిట్లు & 0x3F];
    insize -= 3;
    లోపల += 3;
  }
  ఒకవేళ(insize) {
    /* this is only one or two bytes now */
    *output++ = table64[లోపల[0] >> 2];
    ఒకవేళ(insize == 1) {
      *output++ = table64[((లోపల[0] & 0x03) << 4)];
      ఒకవేళ(padbyte) {
        *output++ = padbyte;
        *output++ = padbyte;
      }
    }
    లేకపోతే {
      /* insize == 2 */
      *output++ = table64[((లోపల[0] & 0x03) << 4) | ((లోపల[1] & 0xF0) >> 4)];
      *output++ = table64[((లోపల[1] & 0x0F) << 2)];
      ఒకవేళ(padbyte)
        *output++ = padbyte;
    }
  }

  /* null-terminate */
  *output = '\0';

  /* Return the pointer to the new data (allocated memory) */
  *outptr = base64data;

  /* Return the length of the new data */
  *outlen = (size_t)(output - base64data);

  తిరిగి CURLE_OK;
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
CURLcode curlx_base64_encode(స్థిరరాశి uint8_t *inputbuff, size_t insize,
                             అక్షరం **outptr, size_t *outlen)
{
  తిరిగి base64_encode(curlx_base64encdec, '=',
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
CURLcode curlx_base64url_encode(స్థిరరాశి uint8_t *inputbuff, size_t insize,
                                అక్షరం **outptr, size_t *outlen)
{
  తిరిగి base64_encode(base64url, 0, inputbuff, insize, outptr, outlen);
}
