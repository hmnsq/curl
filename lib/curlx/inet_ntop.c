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
#அந்தர்பாவ "curl_setup.h"

#ifdef HAVE_SYS_PARAM_H
#அந்தர்பாவ <sys/param.h>
#endif
#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif

#அந்தர்பாவ "curlx/inet_ntop.h"
#அந்தர்பாவ "curlx/snprintf.h"
#அந்தர்பாவ "curlx/strcopy.h"

#define IN6ADDRSZ       16
/* #define INADDRSZ         4 */
#define INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#எனில் !defined(USE_IPV6) && !defined(AF_INET6)
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
நிலையான CURLcode inet_ntop4(மாறிலி unsigned எழுத்து *src, எழுத்து *dst, size_t size)
{
  எழுத்து tmp[அளவு("255.255.255.255")];
  size_t len;

  DEBUGASSERT(size >= 16);

  /* this snprintf() does not overflow the buffer. */
  SNPRINTF(tmp, அளவு(tmp), "%d.%d.%d.%d",
           ((முழு)((unsigned எழுத்து)src[0])) & 0xff,
           ((முழு)((unsigned எழுத்து)src[1])) & 0xff,
           ((முழு)((unsigned எழுத்து)src[2])) & 0xff,
           ((முழு)((unsigned எழுத்து)src[3])) & 0xff);

  len = strlen(tmp);
  எனில்(len == 0 || len >= size)
    திரும்பு CURLE_TOO_LARGE;
  curlx_strcopy(dst, size, tmp, len);
  திரும்பு CURLE_OK;
}

/*
 * Convert IPv6 binary address into presentation (printable) format.
 */
நிலையான CURLcode inet_ntop6(மாறிலி unsigned எழுத்து *src, எழுத்து *dst, size_t size)
{
  /*
   * Note that int32_t and int16_t need only be "at least" large enough
   * to contain a value of the specified size. On some systems, like
   * Crays, there is no such thing as an integer variable with 16 bits.
   * Keep this in mind if you think this function should have been coded
   * to use pointer overlays. All the world's not a VAX.
   */
  எழுத்து tmp[அளவு("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")];
  எழுத்து *tp;
  கட்டமைப்பு {
    முழு base;
    முழு len;
  } best, cur;
  unsigned முழு சொற்கள்[IN6ADDRSZ / INT16SZ];
  முழு i;

  /* Preprocess:
   *  Copy the input (bytewise) array into a wordwise array.
   *  Find the longest run of 0x00's in src[] for :: shorthanding.
   */
  memset(சொற்கள், '\0', அளவு(சொற்கள்));
  க்கு(i = 0; i < IN6ADDRSZ; i++)
    சொற்கள்[i / 2] |= ((unsigned முழு)src[i] << ((1 - (i % 2)) << 3));

  best.base = -1;
  cur.base = -1;
  best.len = 0;
  cur.len = 0;

  க்கு(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    எனில்(சொற்கள்[i] == 0) {
      எனில்(cur.base == -1) {
        cur.base = i;
        cur.len = 1;
      }
      இல்லையெனில்
        cur.len++;
    }
    இல்லையெனில் எனில்(cur.base != -1) {
      எனில்(best.base == -1 || cur.len > best.len)
        best = cur;
      cur.base = -1;
    }
  }
  எனில்((cur.base != -1) && (best.base == -1 || cur.len > best.len))
    best = cur;
  எனில்(best.base != -1 && best.len < 2)
    best.base = -1;
  /* Format the result. */
  tp = tmp;
  க்கு(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    /* Are we inside the best run of 0x00's? */
    எனில்(best.base != -1 && i >= best.base && i < (best.base + best.len)) {
      எனில்(i == best.base)
        *tp++ = ':';
      தொடர்;
    }

    /* Are we following an initial run of 0x00s or any real hex?
     */
    எனில்(i)
      *tp++ = ':';

    /* Is this address an encapsulated IPv4?
     */
    எனில்(i == 6 && best.base == 0 &&
       (best.len == 6 || (best.len == 5 && சொற்கள்[5] == 0xffff))) {
      CURLcode result = inet_ntop4(src + 12, tp, அளவு(tmp) - (tp - tmp));
      எனில்(result)
        திரும்பு result;
      tp += strlen(tp);
      நிறுத்து;
    }
    இல்லையெனில் {
      /* Lower-case digits. Cannot use the set from mprintf.c since this
         needs to work as a curlx function */
      நிலையான மாறிலி unsigned எழுத்து ldigits[] = "0123456789abcdef";

      unsigned முழு w = சொற்கள்[i];
      /* output lowercase 16-bit hex number but ignore leading zeroes */
      எனில்(w & 0xf000)
        *tp++ = ldigits[(w & 0xf000) >> 12];
      எனில்(w & 0xff00)
        *tp++ = ldigits[(w & 0x0f00) >> 8];
      எனில்(w & 0xfff0)
        *tp++ = ldigits[(w & 0x00f0) >> 4];
      *tp++ = ldigits[(w & 0x000f)];
    }
  }

  /* Was it a trailing run of 0x00's?
   */
  எனில்(best.base != -1 && (best.base + best.len) == (IN6ADDRSZ / INT16SZ))
    *tp++ = ':';

  /* Check for overflow, copy, and we are done. */
  எனில்((size_t)(tp - tmp) >= size)
    திரும்பு CURLE_TOO_LARGE;
  curlx_strcopy(dst, size, tmp, tp - tmp);
  திரும்பு CURLE_OK;
}

/*
 * Convert a network format address to presentation format.
 *
 * Copies result to 'buf' and returns CURLcode.
 */
CURLcode curlx_inet_ntop(முழு af, மாறிலி வெறுமை *src, எழுத்து *buf, size_t size)
{
  தேர்வு(af) {
  நிலை AF_INET:
    திரும்பு inet_ntop4((மாறிலி unsigned எழுத்து *)src, buf, size);
  நிலை AF_INET6:
    திரும்பு inet_ntop6((மாறிலி unsigned எழுத்து *)src, buf, size);
  இயல்பு:
    திரும்பு CURLE_UNSUPPORTED_PROTOCOL;
  }
}
