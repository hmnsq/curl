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
#ако !defined(USE_IPV6) && !defined(AF_INET6)
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
static CURLcode inet_ntop4(const unsigned char *src, char *dst, size_t size)
{
  char tmp[sizeof("255.255.255.255")];
  size_t len;

  DEBUGASSERT(size >= 16);

  /* this snprintf() does not overflow the buffer. */
  SNPRINTF(tmp, sizeof(tmp), "%d.%d.%d.%d",
           ((цео)((unsigned char)src[0])) & 0xff,
           ((цео)((unsigned char)src[1])) & 0xff,
           ((цео)((unsigned char)src[2])) & 0xff,
           ((цео)((unsigned char)src[3])) & 0xff);

  len = strlen(tmp);
  ако(len == 0 || len >= size)
    врати CURLE_TOO_LARGE;
  curlx_strcopy(dst, size, tmp, len);
  врати CURLE_OK;
}

/*
 * Convert IPv6 binary address into presentation (printable) format.
 */
static CURLcode inet_ntop6(const unsigned char *src, char *dst, size_t size)
{
  /*
   * Note that int32_t and int16_t need only be "at least" large enough
   * to contain a value of the specified size. On some systems, like
   * Crays, there is no such thing as an integer variable with 16 bits.
   * Keep this in mind if you think this function should have been coded
   * to use pointer overlays. All the world's not a VAX.
   */
  char tmp[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")];
  char *tp;
  struct {
    цео base;
    цео len;
  } best, cur;
  unsigned цео words[IN6ADDRSZ / INT16SZ];
  цео i;

  /* Preprocess:
   *  Copy the input (bytewise) array into a wordwise array.
   *  Find the longest run of 0x00's in src[] for :: shorthanding.
   */
  memset(words, '\0', sizeof(words));
  за(i = 0; i < IN6ADDRSZ; i++)
    words[i / 2] |= ((unsigned цео)src[i] << ((1 - (i % 2)) << 3));

  best.base = -1;
  cur.base = -1;
  best.len = 0;
  cur.len = 0;

  за(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    ако(words[i] == 0) {
      ако(cur.base == -1) {
        cur.base = i;
        cur.len = 1;
      }
      инако
        cur.len++;
    }
    инако ако(cur.base != -1) {
      ако(best.base == -1 || cur.len > best.len)
        best = cur;
      cur.base = -1;
    }
  }
  ако((cur.base != -1) && (best.base == -1 || cur.len > best.len))
    best = cur;
  ако(best.base != -1 && best.len < 2)
    best.base = -1;
  /* Format the result. */
  tp = tmp;
  за(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    /* Are we inside the best run of 0x00's? */
    ако(best.base != -1 && i >= best.base && i < (best.base + best.len)) {
      ако(i == best.base)
        *tp++ = ':';
      continue;
    }

    /* Are we following an initial run of 0x00s or any real hex?
     */
    ако(i)
      *tp++ = ':';

    /* Is this address an encapsulated IPv4?
     */
    ако(i == 6 && best.base == 0 &&
       (best.len == 6 || (best.len == 5 && words[5] == 0xffff))) {
      CURLcode result = inet_ntop4(src + 12, tp, sizeof(tmp) - (tp - tmp));
      ако(result)
        врати result;
      tp += strlen(tp);
      break;
    }
    инако {
      /* Lower-case digits. Cannot use the set from mprintf.c since this
         needs to work as a curlx function */
      static const unsigned char ldigits[] = "0123456789abcdef";

      unsigned цео w = words[i];
      /* output lowercase 16-bit hex number but ignore leading zeroes */
      ако(w & 0xf000)
        *tp++ = ldigits[(w & 0xf000) >> 12];
      ако(w & 0xff00)
        *tp++ = ldigits[(w & 0x0f00) >> 8];
      ако(w & 0xfff0)
        *tp++ = ldigits[(w & 0x00f0) >> 4];
      *tp++ = ldigits[(w & 0x000f)];
    }
  }

  /* Was it a trailing run of 0x00's?
   */
  ако(best.base != -1 && (best.base + best.len) == (IN6ADDRSZ / INT16SZ))
    *tp++ = ':';

  /* Check for overflow, copy, and we are done. */
  ако((size_t)(tp - tmp) >= size)
    врати CURLE_TOO_LARGE;
  curlx_strcopy(dst, size, tmp, tp - tmp);
  врати CURLE_OK;
}

/*
 * Convert a network format address to presentation format.
 *
 * Copies result to 'buf' and returns CURLcode.
 */
CURLcode curlx_inet_ntop(цео af, const void *src, char *buf, size_t size)
{
  switch(af) {
  case AF_INET:
    врати inet_ntop4((const unsigned char *)src, buf, size);
  case AF_INET6:
    врати inet_ntop6((const unsigned char *)src, buf, size);
  default:
    врати CURLE_UNSUPPORTED_PROTOCOL;
  }
}
