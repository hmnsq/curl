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
#شامل "curl_setup.h"

#ifdef HAVE_SYS_PARAM_H
#شامل <sys/param.h>
#endif
#ifdef HAVE_NETINET_IN_H
#شامل <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#endif

#شامل "curlx/inet_ntop.h"
#شامل "curlx/snprintf.h"
#شامل "curlx/strcopy.h"

#define IN6ADDRSZ       16
/* #define INADDRSZ         4 */
#define INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#اگر !defined(USE_IPV6) && !defined(AF_INET6)
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
ایستا CURLcode inet_ntop4(ثابت unsigned نویسه *src, نویسه *dst, size_t size)
{
  نویسه tmp[اندازه("255.255.255.255")];
  size_t len;

  DEBUGASSERT(size >= 16);

  /* this snprintf() does not overflow the buffer. */
  SNPRINTF(tmp, اندازه(tmp), "%d.%d.%d.%d",
           ((صحیح)((unsigned نویسه)src[0])) & 0xff,
           ((صحیح)((unsigned نویسه)src[1])) & 0xff,
           ((صحیح)((unsigned نویسه)src[2])) & 0xff,
           ((صحیح)((unsigned نویسه)src[3])) & 0xff);

  len = strlen(tmp);
  اگر(len == 0 || len >= size)
    بازگشت CURLE_TOO_LARGE;
  curlx_strcopy(dst, size, tmp, len);
  بازگشت CURLE_OK;
}

/*
 * Convert IPv6 binary address into presentation (printable) format.
 */
ایستا CURLcode inet_ntop6(ثابت unsigned نویسه *src, نویسه *dst, size_t size)
{
  /*
   * Note that int32_t and int16_t need only be "at least" large enough
   * to contain a value of the specified size. On some systems, like
   * Crays, there is no such thing as an integer variable with 16 bits.
   * Keep this in mind if you think this function should have been coded
   * to use pointer overlays. All the world's not a VAX.
   */
  نویسه tmp[اندازه("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")];
  نویسه *tp;
  ساختار {
    صحیح base;
    صحیح len;
  } best, cur;
  unsigned صحیح واژگان[IN6ADDRSZ / INT16SZ];
  صحیح i;

  /* Preprocess:
   *  Copy the input (bytewise) array into a wordwise array.
   *  Find the longest run of 0x00's in src[] for :: shorthanding.
   */
  memset(واژگان, '\0', اندازه(واژگان));
  برای(i = 0; i < IN6ADDRSZ; i++)
    واژگان[i / 2] |= ((unsigned صحیح)src[i] << ((1 - (i % 2)) << 3));

  best.base = -1;
  cur.base = -1;
  best.len = 0;
  cur.len = 0;

  برای(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    اگر(واژگان[i] == 0) {
      اگر(cur.base == -1) {
        cur.base = i;
        cur.len = 1;
      }
      وگرنه
        cur.len++;
    }
    وگرنه اگر(cur.base != -1) {
      اگر(best.base == -1 || cur.len > best.len)
        best = cur;
      cur.base = -1;
    }
  }
  اگر((cur.base != -1) && (best.base == -1 || cur.len > best.len))
    best = cur;
  اگر(best.base != -1 && best.len < 2)
    best.base = -1;
  /* Format the result. */
  tp = tmp;
  برای(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    /* Are we inside the best run of 0x00's? */
    اگر(best.base != -1 && i >= best.base && i < (best.base + best.len)) {
      اگر(i == best.base)
        *tp++ = ':';
      ادامه;
    }

    /* Are we following an initial run of 0x00s or any real hex?
     */
    اگر(i)
      *tp++ = ':';

    /* Is this address an encapsulated IPv4?
     */
    اگر(i == 6 && best.base == 0 &&
       (best.len == 6 || (best.len == 5 && واژگان[5] == 0xffff))) {
      CURLcode result = inet_ntop4(src + 12, tp, اندازه(tmp) - (tp - tmp));
      اگر(result)
        بازگشت result;
      tp += strlen(tp);
      بشکن;
    }
    وگرنه {
      /* Lower-case digits. Cannot use the set from mprintf.c since this
         needs to work as a curlx function */
      ایستا ثابت unsigned نویسه ldigits[] = "0123456789abcdef";

      unsigned صحیح w = واژگان[i];
      /* output lowercase 16-bit hex number but ignore leading zeroes */
      اگر(w & 0xf000)
        *tp++ = ldigits[(w & 0xf000) >> 12];
      اگر(w & 0xff00)
        *tp++ = ldigits[(w & 0x0f00) >> 8];
      اگر(w & 0xfff0)
        *tp++ = ldigits[(w & 0x00f0) >> 4];
      *tp++ = ldigits[(w & 0x000f)];
    }
  }

  /* Was it a trailing run of 0x00's?
   */
  اگر(best.base != -1 && (best.base + best.len) == (IN6ADDRSZ / INT16SZ))
    *tp++ = ':';

  /* Check for overflow, copy, and we are done. */
  اگر((size_t)(tp - tmp) >= size)
    بازگشت CURLE_TOO_LARGE;
  curlx_strcopy(dst, size, tmp, tp - tmp);
  بازگشت CURLE_OK;
}

/*
 * Convert a network format address to presentation format.
 *
 * Copies result to 'buf' and returns CURLcode.
 */
CURLcode curlx_inet_ntop(صحیح af, ثابت تهی *src, نویسه *buf, size_t size)
{
  انتخاب(af) {
  حالت AF_INET:
    بازگشت inet_ntop4((ثابت unsigned نویسه *)src, buf, size);
  حالت AF_INET6:
    بازگشت inet_ntop6((ثابت unsigned نویسه *)src, buf, size);
  پیشفرض:
    بازگشت CURLE_UNSUPPORTED_PROTOCOL;
  }
}
