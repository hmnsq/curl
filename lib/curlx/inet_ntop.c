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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_PARAM_H
#ಅನ್ತರ್ಭಾವ <sys/param.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "curlx/inet_ntop.h"
#ಅನ್ತರ್ಭಾವ "curlx/snprintf.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"

#ವ್ಯಾಖ್ಯಾನಿಸು IN6ADDRSZ       16
/* #define INADDRSZ         4 */
#ವ್ಯಾಖ್ಯಾನಿಸು INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#ಆದರೆ !defined(USE_IPV6) && !defined(AF_INET6)
#ವ್ಯಾಖ್ಯಾನಿಸು AF_INET6 (AF_INET + 1)
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Format an IPv4 address, more or less like inet_ntop().
 *
 * Returns CURLcode.
 * Note:
 *  - uses no static variables
 *  - takes an unsigned char* not an in_addr as input
 */
ಸ್ಥಿರ CURLcode inet_ntop4(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *src, ಅಕ್ಷರ *dst, size_t ಗಾತ್ರ_2)
{
  ಅಕ್ಷರ tmp[ಗಾತ್ರ("255.255.255.255")];
  size_t len;

  DEBUGASSERT(ಗಾತ್ರ_2 >= 16);

  /* this snprintf() does not overflow the buffer. */
  SNPRINTF(tmp, ಗಾತ್ರ(tmp), "%d.%d.%d.%d",
           ((ಪೂರ್ಣಾಂಕ)((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)src[0])) & 0xff,
           ((ಪೂರ್ಣಾಂಕ)((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)src[1])) & 0xff,
           ((ಪೂರ್ಣಾಂಕ)((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)src[2])) & 0xff,
           ((ಪೂರ್ಣಾಂಕ)((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)src[3])) & 0xff);

  len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(tmp);
  ಆದರೆ(len == 0 || len >= ಗಾತ್ರ_2)
    ಹಿಂತಿರುಗಿ CURLE_TOO_LARGE;
  curlx_strcopy(dst, ಗಾತ್ರ_2, tmp, len);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Convert IPv6 binary address into presentation (printable) format.
 */
ಸ್ಥಿರ CURLcode inet_ntop6(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *src, ಅಕ್ಷರ *dst, size_t ಗಾತ್ರ_2)
{
  /*
   * Note that int32_t and int16_t need only be "at least" large enough
   * to contain a value of the specified size. On some systems, like
   * Crays, there is no such thing as an integer variable with 16 bits.
   * Keep this in mind if you think this function should have been coded
   * to use pointer overlays. All the world's not a VAX.
   */
  ಅಕ್ಷರ tmp[ಗಾತ್ರ("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")];
  ಅಕ್ಷರ *tp;
  ರಚನೆ {
    ಪೂರ್ಣಾಂಕ ತಳ;
    ಪೂರ್ಣಾಂಕ len;
  } ಉತ್ತಮ, cur;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಪದಗಳು[IN6ADDRSZ / INT16SZ];
  ಪೂರ್ಣಾಂಕ i;

  /* Preprocess:
   *  Copy the input (bytewise) array into a wordwise array.
   *  Find the longest run of 0x00's in src[] for :: shorthanding.
   */
  memset(ಪದಗಳು, '\0', ಗಾತ್ರ(ಪದಗಳು));
  ಗಾಗಿ(i = 0; i < IN6ADDRSZ; i++)
    ಪದಗಳು[i / 2] |= ((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)src[i] << ((1 - (i % 2)) << 3));

  ಉತ್ತಮ.ತಳ = -1;
  cur.ತಳ = -1;
  ಉತ್ತಮ.len = 0;
  cur.len = 0;

  ಗಾಗಿ(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    ಆದರೆ(ಪದಗಳು[i] == 0) {
      ಆದರೆ(cur.ತಳ == -1) {
        cur.ತಳ = i;
        cur.len = 1;
      }
      ಇಲ್ಲದಿದ್ದರೆ
        cur.len++;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cur.ತಳ != -1) {
      ಆದರೆ(ಉತ್ತಮ.ತಳ == -1 || cur.len > ಉತ್ತಮ.len)
        ಉತ್ತಮ = cur;
      cur.ತಳ = -1;
    }
  }
  ಆದರೆ((cur.ತಳ != -1) && (ಉತ್ತಮ.ತಳ == -1 || cur.len > ಉತ್ತಮ.len))
    ಉತ್ತಮ = cur;
  ಆದರೆ(ಉತ್ತಮ.ತಳ != -1 && ಉತ್ತಮ.len < 2)
    ಉತ್ತಮ.ತಳ = -1;
  /* Format the result. */
  tp = tmp;
  ಗಾಗಿ(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    /* Are we inside the best run of 0x00's? */
    ಆದರೆ(ಉತ್ತಮ.ತಳ != -1 && i >= ಉತ್ತಮ.ತಳ && i < (ಉತ್ತಮ.ತಳ + ಉತ್ತಮ.len)) {
      ಆದರೆ(i == ಉತ್ತಮ.ತಳ)
        *tp++ = ':';
      ಮುಂದುವರಿ;
    }

    /* Are we following an initial run of 0x00s or any real hex?
     */
    ಆದರೆ(i)
      *tp++ = ':';

    /* Is this address an encapsulated IPv4?
     */
    ಆದರೆ(i == 6 && ಉತ್ತಮ.ತಳ == 0 &&
       (ಉತ್ತಮ.len == 6 || (ಉತ್ತಮ.len == 5 && ಪದಗಳು[5] == 0xffff))) {
      CURLcode ಫಲಿತಾಂಶ = inet_ntop4(src + 12, tp, ಗಾತ್ರ(tmp) - (tp - tmp));
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      tp += ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(tp);
      ನಿಲ್ಲಿಸು;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* Lower-case digits. Cannot use the set from mprintf.c since this
         needs to work as a curlx function */
      ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ldigits[] = "0123456789abcdef";

      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ w = ಪದಗಳು[i];
      /* output lowercase 16-bit hex number but ignore leading zeroes */
      ಆದರೆ(w & 0xf000)
        *tp++ = ldigits[(w & 0xf000) >> 12];
      ಆದರೆ(w & 0xff00)
        *tp++ = ldigits[(w & 0x0f00) >> 8];
      ಆದರೆ(w & 0xfff0)
        *tp++ = ldigits[(w & 0x00f0) >> 4];
      *tp++ = ldigits[(w & 0x000f)];
    }
  }

  /* Was it a trailing run of 0x00's?
   */
  ಆದರೆ(ಉತ್ತಮ.ತಳ != -1 && (ಉತ್ತಮ.ತಳ + ಉತ್ತಮ.len) == (IN6ADDRSZ / INT16SZ))
    *tp++ = ':';

  /* Check for overflow, copy, and we are done. */
  ಆದರೆ((size_t)(tp - tmp) >= ಗಾತ್ರ_2)
    ಹಿಂತಿರುಗಿ CURLE_TOO_LARGE;
  curlx_strcopy(dst, ಗಾತ್ರ_2, tmp, tp - tmp);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Convert a network format address to presentation format.
 *
 * Copies result to 'buf' and returns CURLcode.
 */
CURLcode curlx_inet_ntop(ಪೂರ್ಣಾಂಕ af, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src, ಅಕ್ಷರ *buf, size_t ಗಾತ್ರ_2)
{
  ಆಯ್ಕೆ(af) {
  ಸ್ಥಿತಿ AF_INET:
    ಹಿಂತಿರುಗಿ inet_ntop4((ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)src, buf, ಗಾತ್ರ_2);
  ಸ್ಥಿತಿ AF_INET6:
    ಹಿಂತಿರುಗಿ inet_ntop6((ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)src, buf, ಗಾತ್ರ_2);
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_UNSUPPORTED_PROTOCOL;
  }
}
