/*
 * Copyright (C) 1996-2022  Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
 * INTERNET SOFTWARE CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * SPDX-License-Identifier: ISC
 */
#समावेश "curl_setup.h"

#अगर_घोषित HAVE_SYS_PARAM_H
#समावेश <sys/param.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर

#समावेश "curlx/inet_ntop.h"
#समावेश "curlx/snprintf.h"
#समावेश "curlx/strcopy.h"

#घोषणा IN6ADDRSZ       16
/* #define INADDRSZ         4 */
#घोषणा INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#अगर !defined(USE_IPV6) && !defined(AF_INET6)
#घोषणा AF_INET6 (AF_INET + 1)
#पूर्ण_अगर

/*
 * Format an IPv4 address, more or less like inet_ntop().
 *
 * Returns CURLcode.
 * Note:
 *  - uses no static variables
 *  - takes an unsigned char* not an in_addr as input
 */
अटल CURLcode inet_ntop4(स्थिर अचिन्हित अक्षर *src, अक्षर *dst, माप_प्रकार आकार)
{
  अक्षर tmp[माप("255.255.255.255")];
  माप_प्रकार len;

  DEBUGASSERT(आकार >= 16);

  /* this snprintf() does not overflow the buffer. */
  SNPRINTF(tmp, माप(tmp), "%d.%d.%d.%d",
           ((पूर्णांक)((अचिन्हित अक्षर)src[0])) & 0xff,
           ((पूर्णांक)((अचिन्हित अक्षर)src[1])) & 0xff,
           ((पूर्णांक)((अचिन्हित अक्षर)src[2])) & 0xff,
           ((पूर्णांक)((अचिन्हित अक्षर)src[3])) & 0xff);

  len = म_माप(tmp);
  अगर(len == 0 || len >= आकार)
    वापस CURLE_TOO_LARGE;
  curlx_strcopy(dst, आकार, tmp, len);
  वापस CURLE_OK;
}

/*
 * Convert IPv6 binary address into presentation (printable) format.
 */
अटल CURLcode inet_ntop6(स्थिर अचिन्हित अक्षर *src, अक्षर *dst, माप_प्रकार आकार)
{
  /*
   * Note that int32_t and int16_t need only be "at least" large enough
   * to contain a value of the specified size. On some systems, like
   * Crays, there is no such thing as an integer variable with 16 bits.
   * Keep this in mind if you think this function should have been coded
   * to use pointer overlays. All the world's not a VAX.
   */
  अक्षर tmp[माप("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")];
  अक्षर *tp;
  काष्ठा {
    पूर्णांक base;
    पूर्णांक len;
  } best, cur;
  अचिन्हित पूर्णांक शब्दगण[IN6ADDRSZ / INT16SZ];
  पूर्णांक i;

  /* Preprocess:
   *  Copy the input (bytewise) array into a wordwise array.
   *  Find the longest run of 0x00's in src[] for :: shorthanding.
   */
  स_रखो(शब्दगण, '\0', माप(शब्दगण));
  क्रम(i = 0; i < IN6ADDRSZ; i++)
    शब्दगण[i / 2] |= ((अचिन्हित पूर्णांक)src[i] << ((1 - (i % 2)) << 3));

  best.base = -1;
  cur.base = -1;
  best.len = 0;
  cur.len = 0;

  क्रम(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    अगर(शब्दगण[i] == 0) {
      अगर(cur.base == -1) {
        cur.base = i;
        cur.len = 1;
      }
      अन्यथा
        cur.len++;
    }
    अन्यथा अगर(cur.base != -1) {
      अगर(best.base == -1 || cur.len > best.len)
        best = cur;
      cur.base = -1;
    }
  }
  अगर((cur.base != -1) && (best.base == -1 || cur.len > best.len))
    best = cur;
  अगर(best.base != -1 && best.len < 2)
    best.base = -1;
  /* Format the result. */
  tp = tmp;
  क्रम(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    /* Are we inside the best run of 0x00's? */
    अगर(best.base != -1 && i >= best.base && i < (best.base + best.len)) {
      अगर(i == best.base)
        *tp++ = ':';
      जारी;
    }

    /* Are we following an initial run of 0x00s or any real hex?
     */
    अगर(i)
      *tp++ = ':';

    /* Is this address an encapsulated IPv4?
     */
    अगर(i == 6 && best.base == 0 &&
       (best.len == 6 || (best.len == 5 && शब्दगण[5] == 0xffff))) {
      CURLcode परिणाम = inet_ntop4(src + 12, tp, माप(tmp) - (tp - tmp));
      अगर(परिणाम)
        वापस परिणाम;
      tp += म_माप(tp);
      अवरोध;
    }
    अन्यथा {
      /* Lower-case digits. Cannot use the set from mprintf.c since this
         needs to work as a curlx function */
      अटल स्थिर अचिन्हित अक्षर ldigits[] = "0123456789abcdef";

      अचिन्हित पूर्णांक w = शब्दगण[i];
      /* output lowercase 16-bit hex number but ignore leading zeroes */
      अगर(w & 0xf000)
        *tp++ = ldigits[(w & 0xf000) >> 12];
      अगर(w & 0xff00)
        *tp++ = ldigits[(w & 0x0f00) >> 8];
      अगर(w & 0xfff0)
        *tp++ = ldigits[(w & 0x00f0) >> 4];
      *tp++ = ldigits[(w & 0x000f)];
    }
  }

  /* Was it a trailing run of 0x00's?
   */
  अगर(best.base != -1 && (best.base + best.len) == (IN6ADDRSZ / INT16SZ))
    *tp++ = ':';

  /* Check for overflow, copy, and we are done. */
  अगर((माप_प्रकार)(tp - tmp) >= आकार)
    वापस CURLE_TOO_LARGE;
  curlx_strcopy(dst, आकार, tmp, tp - tmp);
  वापस CURLE_OK;
}

/*
 * Convert a network format address to presentation format.
 *
 * Copies result to 'buf' and returns CURLcode.
 */
CURLcode curlx_inet_ntop(पूर्णांक af, स्थिर व्योम *src, अक्षर *buf, माप_प्रकार आकार)
{
  चयन(af) {
  हाल AF_INET:
    वापस inet_ntop4((स्थिर अचिन्हित अक्षर *)src, buf, आकार);
  हाल AF_INET6:
    वापस inet_ntop6((स्थिर अचिन्हित अक्षर *)src, buf, आकार);
  शेष:
    वापस CURLE_UNSUPPORTED_PROTOCOL;
  }
}
