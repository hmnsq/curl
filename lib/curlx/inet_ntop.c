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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifdef HAVE_SYS_PARAM_H
#ಅನ್ತರ್ಭಾವ <sys/param.h>
#endif
#ifdef HAVE_NETINET_IN_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#endif

#ಅನ್ತರ್ಭಾವ "curlx/inet_ntop.h"
#ಅನ್ತರ್ಭಾವ "curlx/snprintf.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"

#define IN6ADDRSZ       16
/* #define INADDRSZ         4 */
#define INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#ಯದಿ !defined(USE_IPV6) && !defined(AF_INET6)
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
ಸ್ಥಿರ CURLcode inet_ntop4(ಧ್ರುವ unsigned ವರ್ಣ *src, ವರ್ಣ *dst, size_t size)
{
  ವರ್ಣ tmp[ಪರಿಮಾಣ("255.255.255.255")];
  size_t len;

  DEBUGASSERT(size >= 16);

  /* this snprintf() does not overflow the buffer. */
  SNPRINTF(tmp, ಪರಿಮಾಣ(tmp), "%d.%d.%d.%d",
           ((ಪೂರ್ಣಾಙ್ಕ)((unsigned ವರ್ಣ)src[0])) & 0xff,
           ((ಪೂರ್ಣಾಙ್ಕ)((unsigned ವರ್ಣ)src[1])) & 0xff,
           ((ಪೂರ್ಣಾಙ್ಕ)((unsigned ವರ್ಣ)src[2])) & 0xff,
           ((ಪೂರ್ಣಾಙ್ಕ)((unsigned ವರ್ಣ)src[3])) & 0xff);

  len = strlen(tmp);
  ಯದಿ(len == 0 || len >= size)
    ಪ್ರತ್ಯಾಗಮ CURLE_TOO_LARGE;
  curlx_strcopy(dst, size, tmp, len);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Convert IPv6 binary address into presentation (printable) format.
 */
ಸ್ಥಿರ CURLcode inet_ntop6(ಧ್ರುವ unsigned ವರ್ಣ *src, ವರ್ಣ *dst, size_t size)
{
  /*
   * Note that int32_t and int16_t need only be "at least" large enough
   * to contain a value of the specified size. On some systems, like
   * Crays, there is no such thing as an integer variable with 16 bits.
   * Keep this in mind if you think this function should have been coded
   * to use pointer overlays. All the world's not a VAX.
   */
  ವರ್ಣ tmp[ಪರಿಮಾಣ("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")];
  ವರ್ಣ *tp;
  ಸಂರಚನಾ {
    ಪೂರ್ಣಾಙ್ಕ base;
    ಪೂರ್ಣಾಙ್ಕ len;
  } best, cur;
  unsigned ಪೂರ್ಣಾಙ್ಕ ಪದಗಳು[IN6ADDRSZ / INT16SZ];
  ಪೂರ್ಣಾಙ್ಕ i;

  /* Preprocess:
   *  Copy the input (bytewise) array into a wordwise array.
   *  Find the longest run of 0x00's in src[] for :: shorthanding.
   */
  memset(ಪದಗಳು, '\0', ಪರಿಮಾಣ(ಪದಗಳು));
  ಪ್ರತಿ(i = 0; i < IN6ADDRSZ; i++)
    ಪದಗಳು[i / 2] |= ((unsigned ಪೂರ್ಣಾಙ್ಕ)src[i] << ((1 - (i % 2)) << 3));

  best.base = -1;
  cur.base = -1;
  best.len = 0;
  cur.len = 0;

  ಪ್ರತಿ(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    ಯದಿ(ಪದಗಳು[i] == 0) {
      ಯದಿ(cur.base == -1) {
        cur.base = i;
        cur.len = 1;
      }
      ಅನ್ಯಥಾ
        cur.len++;
    }
    ಅನ್ಯಥಾ ಯದಿ(cur.base != -1) {
      ಯದಿ(best.base == -1 || cur.len > best.len)
        best = cur;
      cur.base = -1;
    }
  }
  ಯದಿ((cur.base != -1) && (best.base == -1 || cur.len > best.len))
    best = cur;
  ಯದಿ(best.base != -1 && best.len < 2)
    best.base = -1;
  /* Format the result. */
  tp = tmp;
  ಪ್ರತಿ(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    /* Are we inside the best run of 0x00's? */
    ಯದಿ(best.base != -1 && i >= best.base && i < (best.base + best.len)) {
      ಯದಿ(i == best.base)
        *tp++ = ':';
      ಅನುವರ್ತ;
    }

    /* Are we following an initial run of 0x00s or any real hex?
     */
    ಯದಿ(i)
      *tp++ = ':';

    /* Is this address an encapsulated IPv4?
     */
    ಯದಿ(i == 6 && best.base == 0 &&
       (best.len == 6 || (best.len == 5 && ಪದಗಳು[5] == 0xffff))) {
      CURLcode result = inet_ntop4(src + 12, tp, ಪರಿಮಾಣ(tmp) - (tp - tmp));
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
      tp += strlen(tp);
      ಭಙ್ಗ;
    }
    ಅನ್ಯಥಾ {
      /* Lower-case digits. Cannot use the set from mprintf.c since this
         needs to work as a curlx function */
      ಸ್ಥಿರ ಧ್ರುವ unsigned ವರ್ಣ ldigits[] = "0123456789abcdef";

      unsigned ಪೂರ್ಣಾಙ್ಕ w = ಪದಗಳು[i];
      /* output lowercase 16-bit hex number but ignore leading zeroes */
      ಯದಿ(w & 0xf000)
        *tp++ = ldigits[(w & 0xf000) >> 12];
      ಯದಿ(w & 0xff00)
        *tp++ = ldigits[(w & 0x0f00) >> 8];
      ಯದಿ(w & 0xfff0)
        *tp++ = ldigits[(w & 0x00f0) >> 4];
      *tp++ = ldigits[(w & 0x000f)];
    }
  }

  /* Was it a trailing run of 0x00's?
   */
  ಯದಿ(best.base != -1 && (best.base + best.len) == (IN6ADDRSZ / INT16SZ))
    *tp++ = ':';

  /* Check for overflow, copy, and we are done. */
  ಯದಿ((size_t)(tp - tmp) >= size)
    ಪ್ರತ್ಯಾಗಮ CURLE_TOO_LARGE;
  curlx_strcopy(dst, size, tmp, tp - tmp);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Convert a network format address to presentation format.
 *
 * Copies result to 'buf' and returns CURLcode.
 */
CURLcode curlx_inet_ntop(ಪೂರ್ಣಾಙ್ಕ af, ಧ್ರುವ ಶೂನ್ಯ *src, ವರ್ಣ *buf, size_t size)
{
  ವಿಕಲ್ಪ(af) {
  ಪಕ್ಷ AF_INET:
    ಪ್ರತ್ಯಾಗಮ inet_ntop4((ಧ್ರುವ unsigned ವರ್ಣ *)src, buf, size);
  ಪಕ್ಷ AF_INET6:
    ಪ್ರತ್ಯಾಗಮ inet_ntop6((ಧ್ರುವ unsigned ವರ್ಣ *)src, buf, size);
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_UNSUPPORTED_PROTOCOL;
  }
}
