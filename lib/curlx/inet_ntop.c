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
#अन्तर्भाव "curl_setup.h"

#ifdef HAVE_SYS_PARAM_H
#अन्तर्भाव <sys/param.h>
#endif
#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif

#अन्तर्भाव "curlx/inet_ntop.h"
#अन्तर्भाव "curlx/snprintf.h"
#अन्तर्भाव "curlx/strcopy.h"

#define IN6ADDRSZ       16
/* #define INADDRSZ         4 */
#define INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#जर !defined(USE_IPV6) && !defined(AF_INET6)
#define AF_INET6 (AF_INET + 1)
#endif

/*
 * Format an IPv4 address, more or less like inet_ntop().
 *
 * Returns CURLcode.
 * Note:
 *  - uses no static variables
 *  - takes an unsigned char* not an in_addr as input
 */
स्थिर CURLcode inet_ntop4(स्थिरांक अचिन्हित अक्षर *src, अक्षर *dst, माप_प्रकार size)
{
  अक्षर tmp[आकार("255.255.255.255")];
  माप_प्रकार len;

  DEBUGASSERT(size >= 16);

  /* this snprintf() does not overflow the buffer. */
  SNPRINTF(tmp, आकार(tmp), "%d.%d.%d.%d",
           ((पूर्णांक)((अचिन्हित अक्षर)src[0])) & 0xff,
           ((पूर्णांक)((अचिन्हित अक्षर)src[1])) & 0xff,
           ((पूर्णांक)((अचिन्हित अक्षर)src[2])) & 0xff,
           ((पूर्णांक)((अचिन्हित अक्षर)src[3])) & 0xff);

  len = म_माप(tmp);
  जर(len == 0 || len >= size)
    परत CURLE_TOO_LARGE;
  curlx_strcopy(dst, size, tmp, len);
  परत CURLE_OK;
}

/*
 * Convert IPv6 binary address into presentation (printable) format.
 */
स्थिर CURLcode inet_ntop6(स्थिरांक अचिन्हित अक्षर *src, अक्षर *dst, माप_प्रकार size)
{
  /*
   * Note that int32_t and int16_t need only be "at least" large enough
   * to contain a value of the specified size. On some systems, like
   * Crays, there is no such thing as an integer variable with 16 bits.
   * Keep this in mind if you think this function should have been coded
   * to use pointer overlays. All the world's not a VAX.
   */
  अक्षर tmp[आकार("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")];
  अक्षर *tp;
  संरचना {
    पूर्णांक base;
    पूर्णांक len;
  } best, cur;
  अचिन्हित पूर्णांक शब्द[IN6ADDRSZ / INT16SZ];
  पूर्णांक i;

  /* Preprocess:
   *  Copy the input (bytewise) array into a wordwise array.
   *  Find the longest run of 0x00's in src[] for :: shorthanding.
   */
  स_रखो(शब्द, '\0', आकार(शब्द));
  साठी(i = 0; i < IN6ADDRSZ; i++)
    शब्द[i / 2] |= ((अचिन्हित पूर्णांक)src[i] << ((1 - (i % 2)) << 3));

  best.base = -1;
  cur.base = -1;
  best.len = 0;
  cur.len = 0;

  साठी(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    जर(शब्द[i] == 0) {
      जर(cur.base == -1) {
        cur.base = i;
        cur.len = 1;
      }
      अन्यथा
        cur.len++;
    }
    अन्यथा जर(cur.base != -1) {
      जर(best.base == -1 || cur.len > best.len)
        best = cur;
      cur.base = -1;
    }
  }
  जर((cur.base != -1) && (best.base == -1 || cur.len > best.len))
    best = cur;
  जर(best.base != -1 && best.len < 2)
    best.base = -1;
  /* Format the result. */
  tp = tmp;
  साठी(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    /* Are we inside the best run of 0x00's? */
    जर(best.base != -1 && i >= best.base && i < (best.base + best.len)) {
      जर(i == best.base)
        *tp++ = ':';
      सुरूठेवा;
    }

    /* Are we following an initial run of 0x00s or any real hex?
     */
    जर(i)
      *tp++ = ':';

    /* Is this address an encapsulated IPv4?
     */
    जर(i == 6 && best.base == 0 &&
       (best.len == 6 || (best.len == 5 && शब्द[5] == 0xffff))) {
      CURLcode result = inet_ntop4(src + 12, tp, आकार(tmp) - (tp - tmp));
      जर(result)
        परत result;
      tp += म_माप(tp);
      थांबा;
    }
    अन्यथा {
      /* Lower-case digits. Cannot use the set from mprintf.c since this
         needs to work as a curlx function */
      स्थिर स्थिरांक अचिन्हित अक्षर ldigits[] = "0123456789abcdef";

      अचिन्हित पूर्णांक w = शब्द[i];
      /* output lowercase 16-bit hex number but ignore leading zeroes */
      जर(w & 0xf000)
        *tp++ = ldigits[(w & 0xf000) >> 12];
      जर(w & 0xff00)
        *tp++ = ldigits[(w & 0x0f00) >> 8];
      जर(w & 0xfff0)
        *tp++ = ldigits[(w & 0x00f0) >> 4];
      *tp++ = ldigits[(w & 0x000f)];
    }
  }

  /* Was it a trailing run of 0x00's?
   */
  जर(best.base != -1 && (best.base + best.len) == (IN6ADDRSZ / INT16SZ))
    *tp++ = ':';

  /* Check for overflow, copy, and we are done. */
  जर((माप_प्रकार)(tp - tmp) >= size)
    परत CURLE_TOO_LARGE;
  curlx_strcopy(dst, size, tmp, tp - tmp);
  परत CURLE_OK;
}

/*
 * Convert a network format address to presentation format.
 *
 * Copies result to 'buf' and returns CURLcode.
 */
CURLcode curlx_inet_ntop(पूर्णांक af, स्थिरांक शून्य *src, अक्षर *buf, माप_प्रकार size)
{
  निवड(af) {
  स्थिती AF_INET:
    परत inet_ntop4((स्थिरांक अचिन्हित अक्षर *)src, buf, size);
  स्थिती AF_INET6:
    परत inet_ntop6((स्थिरांक अचिन्हित अक्षर *)src, buf, size);
  पूर्वनिर्धारित:
    परत CURLE_UNSUPPORTED_PROTOCOL;
  }
}
