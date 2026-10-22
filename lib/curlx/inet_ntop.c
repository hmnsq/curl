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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef HAVE_SYS_PARAM_H
#അന്തര്ഭാവ <sys/param.h>
#endif
#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif

#അന്തര്ഭാവ "curlx/inet_ntop.h"
#അന്തര്ഭാവ "curlx/snprintf.h"
#അന്തര്ഭാവ "curlx/strcopy.h"

#define IN6ADDRSZ       16
/* #define INADDRSZ         4 */
#define INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#എങ്കിൽ !defined(USE_IPV6) && !defined(AF_INET6)
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
സ്ഥിതം CURLcode inet_ntop4(സ്ഥിരം unsigned അക്ഷരം *src, അക്ഷരം *dst, size_t size)
{
  അക്ഷരം tmp[വലുപ്പം("255.255.255.255")];
  size_t len;

  DEBUGASSERT(size >= 16);

  /* this snprintf() does not overflow the buffer. */
  SNPRINTF(tmp, വലുപ്പം(tmp), "%d.%d.%d.%d",
           ((പൂർണ്ണസംഖ്യ)((unsigned അക്ഷരം)src[0])) & 0xff,
           ((പൂർണ്ണസംഖ്യ)((unsigned അക്ഷരം)src[1])) & 0xff,
           ((പൂർണ്ണസംഖ്യ)((unsigned അക്ഷരം)src[2])) & 0xff,
           ((പൂർണ്ണസംഖ്യ)((unsigned അക്ഷരം)src[3])) & 0xff);

  len = strlen(tmp);
  എങ്കിൽ(len == 0 || len >= size)
    തിരികെ CURLE_TOO_LARGE;
  curlx_strcopy(dst, size, tmp, len);
  തിരികെ CURLE_OK;
}

/*
 * Convert IPv6 binary address into presentation (printable) format.
 */
സ്ഥിതം CURLcode inet_ntop6(സ്ഥിരം unsigned അക്ഷരം *src, അക്ഷരം *dst, size_t size)
{
  /*
   * Note that int32_t and int16_t need only be "at least" large enough
   * to contain a value of the specified size. On some systems, like
   * Crays, there is no such thing as an integer variable with 16 bits.
   * Keep this in mind if you think this function should have been coded
   * to use pointer overlays. All the world's not a VAX.
   */
  അക്ഷരം tmp[വലുപ്പം("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")];
  അക്ഷരം *tp;
  ഘടന {
    പൂർണ്ണസംഖ്യ base;
    പൂർണ്ണസംഖ്യ len;
  } best, cur;
  unsigned പൂർണ്ണസംഖ്യ വാക്കുകൾ[IN6ADDRSZ / INT16SZ];
  പൂർണ്ണസംഖ്യ i;

  /* Preprocess:
   *  Copy the input (bytewise) array into a wordwise array.
   *  Find the longest run of 0x00's in src[] for :: shorthanding.
   */
  memset(വാക്കുകൾ, '\0', വലുപ്പം(വാക്കുകൾ));
  വേണ്ടി(i = 0; i < IN6ADDRSZ; i++)
    വാക്കുകൾ[i / 2] |= ((unsigned പൂർണ്ണസംഖ്യ)src[i] << ((1 - (i % 2)) << 3));

  best.base = -1;
  cur.base = -1;
  best.len = 0;
  cur.len = 0;

  വേണ്ടി(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    എങ്കിൽ(വാക്കുകൾ[i] == 0) {
      എങ്കിൽ(cur.base == -1) {
        cur.base = i;
        cur.len = 1;
      }
      അല്ലെങ്കിൽ
        cur.len++;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(cur.base != -1) {
      എങ്കിൽ(best.base == -1 || cur.len > best.len)
        best = cur;
      cur.base = -1;
    }
  }
  എങ്കിൽ((cur.base != -1) && (best.base == -1 || cur.len > best.len))
    best = cur;
  എങ്കിൽ(best.base != -1 && best.len < 2)
    best.base = -1;
  /* Format the result. */
  tp = tmp;
  വേണ്ടി(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    /* Are we inside the best run of 0x00's? */
    എങ്കിൽ(best.base != -1 && i >= best.base && i < (best.base + best.len)) {
      എങ്കിൽ(i == best.base)
        *tp++ = ':';
      തുടരുക;
    }

    /* Are we following an initial run of 0x00s or any real hex?
     */
    എങ്കിൽ(i)
      *tp++ = ':';

    /* Is this address an encapsulated IPv4?
     */
    എങ്കിൽ(i == 6 && best.base == 0 &&
       (best.len == 6 || (best.len == 5 && വാക്കുകൾ[5] == 0xffff))) {
      CURLcode result = inet_ntop4(src + 12, tp, വലുപ്പം(tmp) - (tp - tmp));
      എങ്കിൽ(result)
        തിരികെ result;
      tp += strlen(tp);
      നിർത്തുക;
    }
    അല്ലെങ്കിൽ {
      /* Lower-case digits. Cannot use the set from mprintf.c since this
         needs to work as a curlx function */
      സ്ഥിതം സ്ഥിരം unsigned അക്ഷരം ldigits[] = "0123456789abcdef";

      unsigned പൂർണ്ണസംഖ്യ w = വാക്കുകൾ[i];
      /* output lowercase 16-bit hex number but ignore leading zeroes */
      എങ്കിൽ(w & 0xf000)
        *tp++ = ldigits[(w & 0xf000) >> 12];
      എങ്കിൽ(w & 0xff00)
        *tp++ = ldigits[(w & 0x0f00) >> 8];
      എങ്കിൽ(w & 0xfff0)
        *tp++ = ldigits[(w & 0x00f0) >> 4];
      *tp++ = ldigits[(w & 0x000f)];
    }
  }

  /* Was it a trailing run of 0x00's?
   */
  എങ്കിൽ(best.base != -1 && (best.base + best.len) == (IN6ADDRSZ / INT16SZ))
    *tp++ = ':';

  /* Check for overflow, copy, and we are done. */
  എങ്കിൽ((size_t)(tp - tmp) >= size)
    തിരികെ CURLE_TOO_LARGE;
  curlx_strcopy(dst, size, tmp, tp - tmp);
  തിരികെ CURLE_OK;
}

/*
 * Convert a network format address to presentation format.
 *
 * Copies result to 'buf' and returns CURLcode.
 */
CURLcode curlx_inet_ntop(പൂർണ്ണസംഖ്യ af, സ്ഥിരം ശൂന്യം *src, അക്ഷരം *buf, size_t size)
{
  തിരഞ്ഞെടുപ്പ്(af) {
  അവസ്ഥ AF_INET:
    തിരികെ inet_ntop4((സ്ഥിരം unsigned അക്ഷരം *)src, buf, size);
  അവസ്ഥ AF_INET6:
    തിരികെ inet_ntop6((സ്ഥിരം unsigned അക്ഷരം *)src, buf, size);
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_UNSUPPORTED_PROTOCOL;
  }
}
