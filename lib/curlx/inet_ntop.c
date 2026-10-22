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
#ضمن "curl_setup.h"

#ifdef HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#endif
#ifdef HAVE_NETINET_IN_H
#ضمن <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#endif

#ضمن "curlx/inet_ntop.h"
#ضمن "curlx/snprintf.h"
#ضمن "curlx/strcopy.h"

#define IN6ADDRSZ       16
/* #define INADDRSZ         4 */
#define INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#אם !defined(USE_IPV6) && !defined(AF_INET6)
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
ساكن CURLcode inet_ntop4(ثابت unsigned حرف *src, حرف *dst, size_t size)
{
  حرف tmp[حجم("255.255.255.255")];
  size_t len;

  DEBUGASSERT(size >= 16);

  /* this snprintf() does not overflow the buffer. */
  SNPRINTF(tmp, حجم(tmp), "%d.%d.%d.%d",
           ((صحيح)((unsigned حرف)src[0])) & 0xff,
           ((صحيح)((unsigned حرف)src[1])) & 0xff,
           ((صحيح)((unsigned حرف)src[2])) & 0xff,
           ((صحيح)((unsigned حرف)src[3])) & 0xff);

  len = strlen(tmp);
  אם(len == 0 || len >= size)
    החזר CURLE_TOO_LARGE;
  curlx_strcopy(dst, size, tmp, len);
  החזר CURLE_OK;
}

/*
 * Convert IPv6 binary address into presentation (printable) format.
 */
ساكن CURLcode inet_ntop6(ثابت unsigned حرف *src, حرف *dst, size_t size)
{
  /*
   * Note that int32_t and int16_t need only be "at least" large enough
   * to contain a value of the specified size. On some systems, like
   * Crays, there is no such thing as an integer variable with 16 bits.
   * Keep this in mind if you think this function should have been coded
   * to use pointer overlays. All the world's not a VAX.
   */
  حرف tmp[حجم("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")];
  حرف *tp;
  بنية {
    صحيح base;
    صحيح len;
  } best, cur;
  unsigned صحيح words[IN6ADDRSZ / INT16SZ];
  صحيح i;

  /* Preprocess:
   *  Copy the input (bytewise) array into a wordwise array.
   *  Find the longest run of 0x00's in src[] for :: shorthanding.
   */
  memset(words, '\0', حجم(words));
  עבור(i = 0; i < IN6ADDRSZ; i++)
    words[i / 2] |= ((unsigned صحيح)src[i] << ((1 - (i % 2)) << 3));

  best.base = -1;
  cur.base = -1;
  best.len = 0;
  cur.len = 0;

  עבור(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    אם(words[i] == 0) {
      אם(cur.base == -1) {
        cur.base = i;
        cur.len = 1;
      }
      אחרת
        cur.len++;
    }
    אחרת אם(cur.base != -1) {
      אם(best.base == -1 || cur.len > best.len)
        best = cur;
      cur.base = -1;
    }
  }
  אם((cur.base != -1) && (best.base == -1 || cur.len > best.len))
    best = cur;
  אם(best.base != -1 && best.len < 2)
    best.base = -1;
  /* Format the result. */
  tp = tmp;
  עבור(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    /* Are we inside the best run of 0x00's? */
    אם(best.base != -1 && i >= best.base && i < (best.base + best.len)) {
      אם(i == best.base)
        *tp++ = ':';
      استمر;
    }

    /* Are we following an initial run of 0x00s or any real hex?
     */
    אם(i)
      *tp++ = ':';

    /* Is this address an encapsulated IPv4?
     */
    אם(i == 6 && best.base == 0 &&
       (best.len == 6 || (best.len == 5 && words[5] == 0xffff))) {
      CURLcode result = inet_ntop4(src + 12, tp, حجم(tmp) - (tp - tmp));
      אם(result)
        החזר result;
      tp += strlen(tp);
      اكسر;
    }
    אחרת {
      /* Lower-case digits. Cannot use the set from mprintf.c since this
         needs to work as a curlx function */
      ساكن ثابت unsigned حرف ldigits[] = "0123456789abcdef";

      unsigned صحيح w = words[i];
      /* output lowercase 16-bit hex number but ignore leading zeroes */
      אם(w & 0xf000)
        *tp++ = ldigits[(w & 0xf000) >> 12];
      אם(w & 0xff00)
        *tp++ = ldigits[(w & 0x0f00) >> 8];
      אם(w & 0xfff0)
        *tp++ = ldigits[(w & 0x00f0) >> 4];
      *tp++ = ldigits[(w & 0x000f)];
    }
  }

  /* Was it a trailing run of 0x00's?
   */
  אם(best.base != -1 && (best.base + best.len) == (IN6ADDRSZ / INT16SZ))
    *tp++ = ':';

  /* Check for overflow, copy, and we are done. */
  אם((size_t)(tp - tmp) >= size)
    החזר CURLE_TOO_LARGE;
  curlx_strcopy(dst, size, tmp, tp - tmp);
  החזר CURLE_OK;
}

/*
 * Convert a network format address to presentation format.
 *
 * Copies result to 'buf' and returns CURLcode.
 */
CURLcode curlx_inet_ntop(صحيح af, ثابت فراغ *src, حرف *buf, size_t size)
{
  بدل(af) {
  حالة AF_INET:
    החזר inet_ntop4((ثابت unsigned حرف *)src, buf, size);
  حالة AF_INET6:
    החזר inet_ntop6((ثابت unsigned حرف *)src, buf, size);
  افتراضي:
    החזר CURLE_UNSUPPORTED_PROTOCOL;
  }
}
