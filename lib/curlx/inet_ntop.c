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
#include "curl_setup.h"

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#include "curlx/inet_ntop.h"
#include "curlx/snprintf.h"
#include "curlx/strcopy.h"

#define IN6ADDRSZ       16
/* #define INADDRSZ         4 */
#define INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#གལ་ཏེ !defined(USE_IPV6) && !defined(AF_INET6)
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
static CURLcode inet_ntop4(const unsigned var_na *src, var_na *dst, size_t size)
{
  var_na tmp[parimaa_na("255.255.255.255")];
  size_t len;

  DEBUGASSERT(size >= 16);

  /* this snprintf() does not overflow the buffer. */
  SNPRINTF(tmp, parimaa_na(tmp), "%d.%d.%d.%d",
           ((ཧྲིལ)((unsigned var_na)src[0])) & 0xff,
           ((ཧྲིལ)((unsigned var_na)src[1])) & 0xff,
           ((ཧྲིལ)((unsigned var_na)src[2])) & 0xff,
           ((ཧྲིལ)((unsigned var_na)src[3])) & 0xff);

  len = strlen(tmp);
  གལ་ཏེ(len == 0 || len >= size)
    ཕྱིར CURLE_TOO_LARGE;
  curlx_strcopy(dst, size, tmp, len);
  ཕྱིར CURLE_OK;
}

/*
 * Convert IPv6 binary address into presentation (printable) format.
 */
static CURLcode inet_ntop6(const unsigned var_na *src, var_na *dst, size_t size)
{
  /*
   * Note that int32_t and int16_t need only be "at least" large enough
   * to contain a value of the specified size. On some systems, like
   * Crays, there is no such thing as an integer variable with 16 bits.
   * Keep this in mind if you think this function should have been coded
   * to use pointer overlays. All the world's not a VAX.
   */
  var_na tmp[parimaa_na("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")];
  var_na *tp;
  sa_nracanaa {
    ཧྲིལ base;
    ཧྲིལ len;
  } best, cur;
  unsigned ཧྲིལ words[IN6ADDRSZ / INT16SZ];
  ཧྲིལ i;

  /* Preprocess:
   *  Copy the input (bytewise) array into a wordwise array.
   *  Find the longest run of 0x00's in src[] for :: shorthanding.
   */
  memset(words, '\0', parimaa_na(words));
  ཆེད་དུ(i = 0; i < IN6ADDRSZ; i++)
    words[i / 2] |= ((unsigned ཧྲིལ)src[i] << ((1 - (i % 2)) << 3));

  best.base = -1;
  cur.base = -1;
  best.len = 0;
  cur.len = 0;

  ཆེད་དུ(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    གལ་ཏེ(words[i] == 0) {
      གལ་ཏེ(cur.base == -1) {
        cur.base = i;
        cur.len = 1;
      }
      དེ་མིན
        cur.len++;
    }
    དེ་མིན གལ་ཏེ(cur.base != -1) {
      གལ་ཏེ(best.base == -1 || cur.len > best.len)
        best = cur;
      cur.base = -1;
    }
  }
  གལ་ཏེ((cur.base != -1) && (best.base == -1 || cur.len > best.len))
    best = cur;
  གལ་ཏེ(best.base != -1 && best.len < 2)
    best.base = -1;
  /* Format the result. */
  tp = tmp;
  ཆེད་དུ(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    /* Are we inside the best run of 0x00's? */
    གལ་ཏེ(best.base != -1 && i >= best.base && i < (best.base + best.len)) {
      གལ་ཏེ(i == best.base)
        *tp++ = ':';
      continue;
    }

    /* Are we following an initial run of 0x00s or any real hex?
     */
    གལ་ཏེ(i)
      *tp++ = ':';

    /* Is this address an encapsulated IPv4?
     */
    གལ་ཏེ(i == 6 && best.base == 0 &&
       (best.len == 6 || (best.len == 5 && words[5] == 0xffff))) {
      CURLcode result = inet_ntop4(src + 12, tp, parimaa_na(tmp) - (tp - tmp));
      གལ་ཏེ(result)
        ཕྱིར result;
      tp += strlen(tp);
      bha_nga;
    }
    དེ་མིན {
      /* Lower-case digits. Cannot use the set from mprintf.c since this
         needs to work as a curlx function */
      static const unsigned var_na ldigits[] = "0123456789abcdef";

      unsigned ཧྲིལ w = words[i];
      /* output lowercase 16-bit hex number but ignore leading zeroes */
      གལ་ཏེ(w & 0xf000)
        *tp++ = ldigits[(w & 0xf000) >> 12];
      གལ་ཏེ(w & 0xff00)
        *tp++ = ldigits[(w & 0x0f00) >> 8];
      གལ་ཏེ(w & 0xfff0)
        *tp++ = ldigits[(w & 0x00f0) >> 4];
      *tp++ = ldigits[(w & 0x000f)];
    }
  }

  /* Was it a trailing run of 0x00's?
   */
  གལ་ཏེ(best.base != -1 && (best.base + best.len) == (IN6ADDRSZ / INT16SZ))
    *tp++ = ':';

  /* Check for overflow, copy, and we are done. */
  གལ་ཏེ((size_t)(tp - tmp) >= size)
    ཕྱིར CURLE_TOO_LARGE;
  curlx_strcopy(dst, size, tmp, tp - tmp);
  ཕྱིར CURLE_OK;
}

/*
 * Convert a network format address to presentation format.
 *
 * Copies result to 'buf' and returns CURLcode.
 */
CURLcode curlx_inet_ntop(ཧྲིལ af, const void *src, var_na *buf, size_t size)
{
  switch(af) {
  pak_sa AF_INET:
    ཕྱིར inet_ntop4((const unsigned var_na *)src, buf, size);
  pak_sa AF_INET6:
    ཕྱིར inet_ntop6((const unsigned var_na *)src, buf, size);
  default:
    ཕྱིར CURLE_UNSUPPORTED_PROTOCOL;
  }
}
