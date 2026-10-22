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
#यदि !defined(USE_IPV6) && !defined(AF_INET6)
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
स्थिर CURLcode inet_ntop4(अचर अचिन्हित वर्ण *src, वर्ण *dst, माप_प्रकार size)
{
  वर्ण tmp[परिमाण("255.255.255.255")];
  माप_प्रकार len;

  DEBUGASSERT(size >= 16);

  /* this snprintf() does not overflow the buffer. */
  SNPRINTF(tmp, परिमाण(tmp), "%d.%d.%d.%d",
           ((पूर्णाङ्क)((अचिन्हित वर्ण)src[0])) & 0xff,
           ((पूर्णाङ्क)((अचिन्हित वर्ण)src[1])) & 0xff,
           ((पूर्णाङ्क)((अचिन्हित वर्ण)src[2])) & 0xff,
           ((पूर्णाङ्क)((अचिन्हित वर्ण)src[3])) & 0xff);

  len = म_माप(tmp);
  यदि(len == 0 || len >= size)
    फर्काऊ CURLE_TOO_LARGE;
  curlx_strcopy(dst, size, tmp, len);
  फर्काऊ CURLE_OK;
}

/*
 * Convert IPv6 binary address into presentation (printable) format.
 */
स्थिर CURLcode inet_ntop6(अचर अचिन्हित वर्ण *src, वर्ण *dst, माप_प्रकार size)
{
  /*
   * Note that int32_t and int16_t need only be "at least" large enough
   * to contain a value of the specified size. On some systems, like
   * Crays, there is no such thing as an integer variable with 16 bits.
   * Keep this in mind if you think this function should have been coded
   * to use pointer overlays. All the world's not a VAX.
   */
  वर्ण tmp[परिमाण("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")];
  वर्ण *tp;
  संरचना {
    पूर्णाङ्क base;
    पूर्णाङ्क len;
  } best, cur;
  अचिन्हित पूर्णाङ्क शब्दहरू[IN6ADDRSZ / INT16SZ];
  पूर्णाङ्क i;

  /* Preprocess:
   *  Copy the input (bytewise) array into a wordwise array.
   *  Find the longest run of 0x00's in src[] for :: shorthanding.
   */
  स_रखो(शब्दहरू, '\0', परिमाण(शब्दहरू));
  लागि(i = 0; i < IN6ADDRSZ; i++)
    शब्दहरू[i / 2] |= ((अचिन्हित पूर्णाङ्क)src[i] << ((1 - (i % 2)) << 3));

  best.base = -1;
  cur.base = -1;
  best.len = 0;
  cur.len = 0;

  लागि(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    यदि(शब्दहरू[i] == 0) {
      यदि(cur.base == -1) {
        cur.base = i;
        cur.len = 1;
      }
      अन्यथा
        cur.len++;
    }
    अन्यथा यदि(cur.base != -1) {
      यदि(best.base == -1 || cur.len > best.len)
        best = cur;
      cur.base = -1;
    }
  }
  यदि((cur.base != -1) && (best.base == -1 || cur.len > best.len))
    best = cur;
  यदि(best.base != -1 && best.len < 2)
    best.base = -1;
  /* Format the result. */
  tp = tmp;
  लागि(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    /* Are we inside the best run of 0x00's? */
    यदि(best.base != -1 && i >= best.base && i < (best.base + best.len)) {
      यदि(i == best.base)
        *tp++ = ':';
      जारी;
    }

    /* Are we following an initial run of 0x00s or any real hex?
     */
    यदि(i)
      *tp++ = ':';

    /* Is this address an encapsulated IPv4?
     */
    यदि(i == 6 && best.base == 0 &&
       (best.len == 6 || (best.len == 5 && शब्दहरू[5] == 0xffff))) {
      CURLcode result = inet_ntop4(src + 12, tp, परिमाण(tmp) - (tp - tmp));
      यदि(result)
        फर्काऊ result;
      tp += म_माप(tp);
      रोक;
    }
    अन्यथा {
      /* Lower-case digits. Cannot use the set from mprintf.c since this
         needs to work as a curlx function */
      स्थिर अचर अचिन्हित वर्ण ldigits[] = "0123456789abcdef";

      अचिन्हित पूर्णाङ्क w = शब्दहरू[i];
      /* output lowercase 16-bit hex number but ignore leading zeroes */
      यदि(w & 0xf000)
        *tp++ = ldigits[(w & 0xf000) >> 12];
      यदि(w & 0xff00)
        *tp++ = ldigits[(w & 0x0f00) >> 8];
      यदि(w & 0xfff0)
        *tp++ = ldigits[(w & 0x00f0) >> 4];
      *tp++ = ldigits[(w & 0x000f)];
    }
  }

  /* Was it a trailing run of 0x00's?
   */
  यदि(best.base != -1 && (best.base + best.len) == (IN6ADDRSZ / INT16SZ))
    *tp++ = ':';

  /* Check for overflow, copy, and we are done. */
  यदि((माप_प्रकार)(tp - tmp) >= size)
    फर्काऊ CURLE_TOO_LARGE;
  curlx_strcopy(dst, size, tmp, tp - tmp);
  फर्काऊ CURLE_OK;
}

/*
 * Convert a network format address to presentation format.
 *
 * Copies result to 'buf' and returns CURLcode.
 */
CURLcode curlx_inet_ntop(पूर्णाङ्क af, अचर शून्य *src, वर्ण *buf, माप_प्रकार size)
{
  विकल्प(af) {
  पक्ष AF_INET:
    फर्काऊ inet_ntop4((अचर अचिन्हित वर्ण *)src, buf, size);
  पक्ष AF_INET6:
    फर्काऊ inet_ntop6((अचर अचिन्हित वर्ण *)src, buf, size);
  स्वभाव:
    फर्काऊ CURLE_UNSUPPORTED_PROTOCOL;
  }
}
