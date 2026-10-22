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
#অন্তর্ভাব "curl_setup.h"

#ifdef HAVE_SYS_PARAM_H
#অন্তর্ভাব <sys/param.h>
#endif
#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif

#অন্তর্ভাব "curlx/inet_ntop.h"
#অন্তর্ভাব "curlx/snprintf.h"
#অন্তর্ভাব "curlx/strcopy.h"

#define IN6ADDRSZ       16
/* #define INADDRSZ         4 */
#define INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#যদি !defined(USE_IPV6) && !defined(AF_INET6)
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
স্থির CURLcode inet_ntop4(ধ্রুব unsigned বর্ণ *src, বর্ণ *dst, size_t size)
{
  বর্ণ tmp[পরিমাণ("255.255.255.255")];
  size_t len;

  DEBUGASSERT(size >= 16);

  /* this snprintf() does not overflow the buffer. */
  SNPRINTF(tmp, পরিমাণ(tmp), "%d.%d.%d.%d",
           ((পূর্ণাঙ্ক)((unsigned বর্ণ)src[0])) & 0xff,
           ((পূর্ণাঙ্ক)((unsigned বর্ণ)src[1])) & 0xff,
           ((পূর্ণাঙ্ক)((unsigned বর্ণ)src[2])) & 0xff,
           ((পূর্ণাঙ্ক)((unsigned বর্ণ)src[3])) & 0xff);

  len = strlen(tmp);
  যদি(len == 0 || len >= size)
    প্রত্যাগম CURLE_TOO_LARGE;
  curlx_strcopy(dst, size, tmp, len);
  প্রত্যাগম CURLE_OK;
}

/*
 * Convert IPv6 binary address into presentation (printable) format.
 */
স্থির CURLcode inet_ntop6(ধ্রুব unsigned বর্ণ *src, বর্ণ *dst, size_t size)
{
  /*
   * Note that int32_t and int16_t need only be "at least" large enough
   * to contain a value of the specified size. On some systems, like
   * Crays, there is no such thing as an integer variable with 16 bits.
   * Keep this in mind if you think this function should have been coded
   * to use pointer overlays. All the world's not a VAX.
   */
  বর্ণ tmp[পরিমাণ("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")];
  বর্ণ *tp;
  সংরচনা {
    পূর্ণাঙ্ক base;
    পূর্ণাঙ্ক len;
  } best, cur;
  unsigned পূর্ণাঙ্ক শব্দগুলি[IN6ADDRSZ / INT16SZ];
  পূর্ণাঙ্ক i;

  /* Preprocess:
   *  Copy the input (bytewise) array into a wordwise array.
   *  Find the longest run of 0x00's in src[] for :: shorthanding.
   */
  memset(শব্দগুলি, '\0', পরিমাণ(শব্দগুলি));
  প্রতি(i = 0; i < IN6ADDRSZ; i++)
    শব্দগুলি[i / 2] |= ((unsigned পূর্ণাঙ্ক)src[i] << ((1 - (i % 2)) << 3));

  best.base = -1;
  cur.base = -1;
  best.len = 0;
  cur.len = 0;

  প্রতি(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    যদি(শব্দগুলি[i] == 0) {
      যদি(cur.base == -1) {
        cur.base = i;
        cur.len = 1;
      }
      অন্যথা
        cur.len++;
    }
    অন্যথা যদি(cur.base != -1) {
      যদি(best.base == -1 || cur.len > best.len)
        best = cur;
      cur.base = -1;
    }
  }
  যদি((cur.base != -1) && (best.base == -1 || cur.len > best.len))
    best = cur;
  যদি(best.base != -1 && best.len < 2)
    best.base = -1;
  /* Format the result. */
  tp = tmp;
  প্রতি(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    /* Are we inside the best run of 0x00's? */
    যদি(best.base != -1 && i >= best.base && i < (best.base + best.len)) {
      যদি(i == best.base)
        *tp++ = ':';
      অনুবর্ত;
    }

    /* Are we following an initial run of 0x00s or any real hex?
     */
    যদি(i)
      *tp++ = ':';

    /* Is this address an encapsulated IPv4?
     */
    যদি(i == 6 && best.base == 0 &&
       (best.len == 6 || (best.len == 5 && শব্দগুলি[5] == 0xffff))) {
      CURLcode result = inet_ntop4(src + 12, tp, পরিমাণ(tmp) - (tp - tmp));
      যদি(result)
        প্রত্যাগম result;
      tp += strlen(tp);
      ভঙ্গ;
    }
    অন্যথা {
      /* Lower-case digits. Cannot use the set from mprintf.c since this
         needs to work as a curlx function */
      স্থির ধ্রুব unsigned বর্ণ ldigits[] = "0123456789abcdef";

      unsigned পূর্ণাঙ্ক w = শব্দগুলি[i];
      /* output lowercase 16-bit hex number but ignore leading zeroes */
      যদি(w & 0xf000)
        *tp++ = ldigits[(w & 0xf000) >> 12];
      যদি(w & 0xff00)
        *tp++ = ldigits[(w & 0x0f00) >> 8];
      যদি(w & 0xfff0)
        *tp++ = ldigits[(w & 0x00f0) >> 4];
      *tp++ = ldigits[(w & 0x000f)];
    }
  }

  /* Was it a trailing run of 0x00's?
   */
  যদি(best.base != -1 && (best.base + best.len) == (IN6ADDRSZ / INT16SZ))
    *tp++ = ':';

  /* Check for overflow, copy, and we are done. */
  যদি((size_t)(tp - tmp) >= size)
    প্রত্যাগম CURLE_TOO_LARGE;
  curlx_strcopy(dst, size, tmp, tp - tmp);
  প্রত্যাগম CURLE_OK;
}

/*
 * Convert a network format address to presentation format.
 *
 * Copies result to 'buf' and returns CURLcode.
 */
CURLcode curlx_inet_ntop(পূর্ণাঙ্ক af, ধ্রুব শূন্য *src, বর্ণ *buf, size_t size)
{
  বিকল্প(af) {
  পক্ষ AF_INET:
    প্রত্যাগম inet_ntop4((ধ্রুব unsigned বর্ণ *)src, buf, size);
  পক্ষ AF_INET6:
    প্রত্যাগম inet_ntop6((ধ্রুব unsigned বর্ণ *)src, buf, size);
  স্বভাব:
    প্রত্যাগম CURLE_UNSUPPORTED_PROTOCOL;
  }
}
