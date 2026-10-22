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

#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "curlx/base64.h"

/* ---- Base64 Encoding/Decoding Table --- */
ध्रुव वर्ण curlx_base64encdec[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* The Base 64 encoding with a URL and filename safe alphabet, RFC 4648
   section 5 */
स्थिर ध्रुव वर्ण base64url[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

स्थिर ध्रुव अचिन्हित वर्ण decodetable[256] = {
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
CURLcode curlx_base64_decode(ध्रुव वर्ण *src,
                             uint8_t **outptr, माप_प्रकार *outlen)
{
  माप_प्रकार srclen = 0;
  माप_प्रकार padding = 0;
  माप_प्रकार i;
  माप_प्रकार अंक_Quantums;
  माप_प्रकार fullQuantums;
  माप_प्रकार rawlen = 0;
  अचिन्हित वर्ण *pos;
  अचिन्हित वर्ण *newstr;

  *outptr = रिक्त;
  *outlen = 0;
  srclen = म_माप(src);

  /* Check the length of the input string is valid */
  यदि(!srclen || srclen % 4)
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;

  /* srclen is at least 4 here */
  यावत्(src[srclen - 1 - padding] == '=') {
    /* count padding characters */
    padding++;
    /* A maximum of two = padding characters is allowed */
    यदि(padding > 2)
      प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
  }

  /* Calculate the number of quantums */
  अंक_Quantums = srclen / 4;
  fullQuantums = अंक_Quantums - (padding ? 1 : 0);

  /* Calculate the size of the decoded string */
  rawlen = (अंक_Quantums * 3) - padding;

  /* Allocate our buffer including room for a null-terminator */
  newstr = curlx_malloc(rawlen + 1);
  यदि(!newstr)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  pos = newstr;

  /* Decode the complete quantums first */
  प्रति(i = 0; i < fullQuantums; i++) {
    अचिन्हित वर्ण v0 = decodetable[(अचिन्हित वर्ण)src[0]];
    अचिन्हित वर्ण v1 = decodetable[(अचिन्हित वर्ण)src[1]];
    अचिन्हित वर्ण v2 = decodetable[(अचिन्हित वर्ण)src[2]];
    अचिन्हित वर्ण v3 = decodetable[(अचिन्हित वर्ण)src[3]];
    यदि((v0 | v1 | v2 | v3) & 0x80)
      गच्छ bad;
    pos[0] = (अचिन्हित वर्ण)((v0 << 2) | (v1 >> 4));
    pos[1] = (अचिन्हित वर्ण)((v1 << 4) | (v2 >> 2));
    pos[2] = (अचिन्हित वर्ण)((v2 << 6) | v3);
    pos += 3;
    src += 4;
  }
  यदि(padding) {
    /* this means either 8 or 16 bits output */
    अचिन्हित वर्ण val;
    अचिन्हित पूर्णाङ्क x = 0;
    पूर्णाङ्क j;
    माप_प्रकार padc = 0;
    प्रति(j = 0; j < 4; j++) {
      यदि(*src == '=') {
        x <<= 6;
        src++;
        यदि(++padc > padding)
          /* this is a badly placed '=' symbol! */
          गच्छ bad;
      }
      अन्यथा {
        val = decodetable[(अचिन्हित वर्ण)*src++];
        यदि(val == 0xff) /* bad symbol */
          गच्छ bad;
        x = (x << 6) | val;
      }
    }
    यदि(padding == 1)
      pos[1] = (अचिन्हित वर्ण)((x >> 8) & 0xff);
    pos[0] = (अचिन्हित वर्ण)((x >> 16) & 0xff);
    pos += 3 - padding;
  }

  /* null-terminate */
  *pos = '\0';

  /* Return the decoded data */
  *outptr = newstr;
  *outlen = rawlen;

  प्रत्यागम CURLE_OK;
bad:
  curlx_free(newstr);
  प्रत्यागम CURLE_BAD_CONTENT_ENCODING;
}

स्थिर CURLcode base64_encode(ध्रुव वर्ण *table64,
                              uint8_t padbyte,
                              ध्रुव uint8_t *inputbuff, माप_प्रकार insize,
                              वर्ण **outptr, माप_प्रकार *outlen)
{
  वर्ण *output;
  वर्ण *base64data;
  ध्रुव अचिन्हित वर्ण *in = (ध्रुव अचिन्हित वर्ण *)inputbuff;

  *outptr = रिक्त;
  *outlen = 0;

  यदि(!insize)
    प्रत्यागम CURLE_OK;

  /* safety precaution */
  DEBUGASSERT(insize <= CURL_MAX_BASE64_निवेश);
  यदि(insize > CURL_MAX_BASE64_निवेश)
    प्रत्यागम CURLE_TOO_LARGE;

  base64data = output = curlx_malloc(((insize + 2) / 3 * 4) + 1);
  यदि(!output)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  यावत्(insize >= 3) {
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
  यदि(insize) {
    /* this is only one or two bytes now */
    *output++ = table64[in[0] >> 2];
    यदि(insize == 1) {
      *output++ = table64[((in[0] & 0x03) << 4)];
      यदि(padbyte) {
        *output++ = padbyte;
        *output++ = padbyte;
      }
    }
    अन्यथा {
      /* insize == 2 */
      *output++ = table64[((in[0] & 0x03) << 4) | ((in[1] & 0xF0) >> 4)];
      *output++ = table64[((in[1] & 0x0F) << 2)];
      यदि(padbyte)
        *output++ = padbyte;
    }
  }

  /* null-terminate */
  *output = '\0';

  /* Return the pointer to the new data (allocated memory) */
  *outptr = base64data;

  /* Return the length of the new data */
  *outlen = (माप_प्रकार)(output - base64data);

  प्रत्यागम CURLE_OK;
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
CURLcode curlx_base64_encode(ध्रुव uint8_t *inputbuff, माप_प्रकार insize,
                             वर्ण **outptr, माप_प्रकार *outlen)
{
  प्रत्यागम base64_encode(curlx_base64encdec, '=',
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
CURLcode curlx_base64url_encode(ध्रुव uint8_t *inputbuff, माप_प्रकार insize,
                                वर्ण **outptr, माप_प्रकार *outlen)
{
  प्रत्यागम base64_encode(base64url, 0, inputbuff, insize, outptr, outlen);
}
