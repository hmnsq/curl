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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_PARAM_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/param.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/inet_ntop.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/snprintf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strcopy.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IN6ADDRSZ       16
/* #define INADDRSZ         4 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#ਜੇ !defined(USE_IPV6) && !defined(AF_INET6)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AF_INET6 (AF_INET + 1)
#ਜੇ_ਅੰਤ

/*
 * Format an IPv4 address, more or less like inet_ntop().
 *
 * Returns CURLcode.
 * Note:
 *  - uses no static variables
 *  - takes an unsigned char* not an in_addr as input
 */
ਸਥਿਰ CURLcode inet_ntop4(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *src, ਅੱਖਰ *dst, size_t ਮਾਪ)
{
  ਅੱਖਰ tmp[ਆਕਾਰ("255.255.255.255")];
  size_t len;

  DEBUGASSERT(ਮਾਪ >= 16);

  /* this snprintf() does not overflow the buffer. */
  SNPRINTF(tmp, ਆਕਾਰ(tmp), "%d.%d.%d.%d",
           ((ਪੂਰਨ)((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)src[0])) & 0xff,
           ((ਪੂਰਨ)((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)src[1])) & 0xff,
           ((ਪੂਰਨ)((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)src[2])) & 0xff,
           ((ਪੂਰਨ)((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)src[3])) & 0xff);

  len = ਲੜੀ_ਲੰਬਾਈ(tmp);
  ਜੇ(len == 0 || len >= ਮਾਪ)
    ਵਾਪਸ CURLE_TOO_LARGE;
  curlx_strcopy(dst, ਮਾਪ, tmp, len);
  ਵਾਪਸ CURLE_OK;
}

/*
 * Convert IPv6 binary address into presentation (printable) format.
 */
ਸਥਿਰ CURLcode inet_ntop6(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *src, ਅੱਖਰ *dst, size_t ਮਾਪ)
{
  /*
   * Note that int32_t and int16_t need only be "at least" large enough
   * to contain a value of the specified size. On some systems, like
   * Crays, there is no such thing as an integer variable with 16 bits.
   * Keep this in mind if you think this function should have been coded
   * to use pointer overlays. All the world's not a VAX.
   */
  ਅੱਖਰ tmp[ਆਕਾਰ("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")];
  ਅੱਖਰ *tp;
  ਬਣਤਰ {
    ਪੂਰਨ ਅਧਾਰ;
    ਪੂਰਨ len;
  } ਉੱਤਮ, cur;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਸ਼ਬਦ_2[IN6ADDRSZ / INT16SZ];
  ਪੂਰਨ i;

  /* Preprocess:
   *  Copy the input (bytewise) array into a wordwise array.
   *  Find the longest run of 0x00's in src[] for :: shorthanding.
   */
  memset(ਸ਼ਬਦ_2, '\0', ਆਕਾਰ(ਸ਼ਬਦ_2));
  ਲਈ(i = 0; i < IN6ADDRSZ; i++)
    ਸ਼ਬਦ_2[i / 2] |= ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)src[i] << ((1 - (i % 2)) << 3));

  ਉੱਤਮ.ਅਧਾਰ = -1;
  cur.ਅਧਾਰ = -1;
  ਉੱਤਮ.len = 0;
  cur.len = 0;

  ਲਈ(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    ਜੇ(ਸ਼ਬਦ_2[i] == 0) {
      ਜੇ(cur.ਅਧਾਰ == -1) {
        cur.ਅਧਾਰ = i;
        cur.len = 1;
      }
      ਨਹੀਂਤਾਂ
        cur.len++;
    }
    ਨਹੀਂਤਾਂ ਜੇ(cur.ਅਧਾਰ != -1) {
      ਜੇ(ਉੱਤਮ.ਅਧਾਰ == -1 || cur.len > ਉੱਤਮ.len)
        ਉੱਤਮ = cur;
      cur.ਅਧਾਰ = -1;
    }
  }
  ਜੇ((cur.ਅਧਾਰ != -1) && (ਉੱਤਮ.ਅਧਾਰ == -1 || cur.len > ਉੱਤਮ.len))
    ਉੱਤਮ = cur;
  ਜੇ(ਉੱਤਮ.ਅਧਾਰ != -1 && ਉੱਤਮ.len < 2)
    ਉੱਤਮ.ਅਧਾਰ = -1;
  /* Format the result. */
  tp = tmp;
  ਲਈ(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    /* Are we inside the best run of 0x00's? */
    ਜੇ(ਉੱਤਮ.ਅਧਾਰ != -1 && i >= ਉੱਤਮ.ਅਧਾਰ && i < (ਉੱਤਮ.ਅਧਾਰ + ਉੱਤਮ.len)) {
      ਜੇ(i == ਉੱਤਮ.ਅਧਾਰ)
        *tp++ = ':';
      ਜਾਰੀ;
    }

    /* Are we following an initial run of 0x00s or any real hex?
     */
    ਜੇ(i)
      *tp++ = ':';

    /* Is this address an encapsulated IPv4?
     */
    ਜੇ(i == 6 && ਉੱਤਮ.ਅਧਾਰ == 0 &&
       (ਉੱਤਮ.len == 6 || (ਉੱਤਮ.len == 5 && ਸ਼ਬਦ_2[5] == 0xffff))) {
      CURLcode ਨਤੀਜਾ = inet_ntop4(src + 12, tp, ਆਕਾਰ(tmp) - (tp - tmp));
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
      tp += ਲੜੀ_ਲੰਬਾਈ(tp);
      ਰੋਕੋ;
    }
    ਨਹੀਂਤਾਂ {
      /* Lower-case digits. Cannot use the set from mprintf.c since this
         needs to work as a curlx function */
      ਸਥਿਰ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ldigits[] = "0123456789abcdef";

      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ w = ਸ਼ਬਦ_2[i];
      /* output lowercase 16-bit hex number but ignore leading zeroes */
      ਜੇ(w & 0xf000)
        *tp++ = ldigits[(w & 0xf000) >> 12];
      ਜੇ(w & 0xff00)
        *tp++ = ldigits[(w & 0x0f00) >> 8];
      ਜੇ(w & 0xfff0)
        *tp++ = ldigits[(w & 0x00f0) >> 4];
      *tp++ = ldigits[(w & 0x000f)];
    }
  }

  /* Was it a trailing run of 0x00's?
   */
  ਜੇ(ਉੱਤਮ.ਅਧਾਰ != -1 && (ਉੱਤਮ.ਅਧਾਰ + ਉੱਤਮ.len) == (IN6ADDRSZ / INT16SZ))
    *tp++ = ':';

  /* Check for overflow, copy, and we are done. */
  ਜੇ((size_t)(tp - tmp) >= ਮਾਪ)
    ਵਾਪਸ CURLE_TOO_LARGE;
  curlx_strcopy(dst, ਮਾਪ, tmp, tp - tmp);
  ਵਾਪਸ CURLE_OK;
}

/*
 * Convert a network format address to presentation format.
 *
 * Copies result to 'buf' and returns CURLcode.
 */
CURLcode curlx_inet_ntop(ਪੂਰਨ af, ਅਚਲ ਖਾਲੀ *src, ਅੱਖਰ *buf, size_t ਮਾਪ)
{
  ਚੋਣ(af) {
  ਹਾਲਤ AF_INET:
    ਵਾਪਸ inet_ntop4((ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)src, buf, ਮਾਪ);
  ਹਾਲਤ AF_INET6:
    ਵਾਪਸ inet_ntop6((ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)src, buf, ਮਾਪ);
  ਮੂਲ:
    ਵਾਪਸ CURLE_UNSUPPORTED_PROTOCOL;
  }
}
