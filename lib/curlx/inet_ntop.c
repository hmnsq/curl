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

#جے_تعریف_شدہ HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NETINET_وچ_H
#ضمن <netinet/in.h>
#جے_انت
#جے_تعریف_شدہ HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جے_انت

#ضمن "curlx/inet_ntop.h"
#ضمن "curlx/snprintf.h"
#ضمن "curlx/strcopy.h"

#تعریف_کرو IN6ADDRSZ       16
/* #define INADDRSZ         4 */
#تعریف_کرو INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#إذا !defined(USE_IPV6) && !defined(AF_INET6)
#تعریف_کرو AF_INET6 (AF_INET + 1)
#جے_انت

/*
 * Format an IPv4 address, more or less like inet_ntop().
 *
 * Returns CURLcode.
 * Note:
 *  - uses no static variables
 *  - takes an unsigned char* not an in_addr as input
 */
ساكن CURLcode inet_ntop4(ثابت بنانشانی حرف *src, حرف *dst, size_t ناپ)
{
  حرف tmp[حجم("255.255.255.255")];
  size_t len;

  DEBUGASSERT(ناپ >= 16);

  /* this snprintf() does not overflow the buffer. */
  SNPRINTF(tmp, حجم(tmp), "%d.%d.%d.%d",
           ((صحيح)((بنانشانی حرف)src[0])) & 0xff,
           ((صحيح)((بنانشانی حرف)src[1])) & 0xff,
           ((صحيح)((بنانشانی حرف)src[2])) & 0xff,
           ((صحيح)((بنانشانی حرف)src[3])) & 0xff);

  len = لڑی_لمبائی(tmp);
  إذا(len == 0 || len >= ناپ)
    أرجع CURLE_TOO_LARGE;
  curlx_strcopy(dst, ناپ, tmp, len);
  أرجع CURLE_OK;
}

/*
 * Convert IPv6 binary address into presentation (printable) format.
 */
ساكن CURLcode inet_ntop6(ثابت بنانشانی حرف *src, حرف *dst, size_t ناپ)
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
    صحيح ادھار;
    صحيح len;
  } بہترین, cur;
  بنانشانی صحيح الفاظ[IN6ADDRSZ / INT16SZ];
  صحيح i;

  /* Preprocess:
   *  Copy the input (bytewise) array into a wordwise array.
   *  Find the longest run of 0x00's in src[] for :: shorthanding.
   */
  memset(الفاظ, '\0', حجم(الفاظ));
  لكل(i = 0; i < IN6ADDRSZ; i++)
    الفاظ[i / 2] |= ((بنانشانی صحيح)src[i] << ((1 - (i % 2)) << 3));

  بہترین.ادھار = -1;
  cur.ادھار = -1;
  بہترین.len = 0;
  cur.len = 0;

  لكل(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    إذا(الفاظ[i] == 0) {
      إذا(cur.ادھار == -1) {
        cur.ادھار = i;
        cur.len = 1;
      }
      وإلا
        cur.len++;
    }
    وإلا إذا(cur.ادھار != -1) {
      إذا(بہترین.ادھار == -1 || cur.len > بہترین.len)
        بہترین = cur;
      cur.ادھار = -1;
    }
  }
  إذا((cur.ادھار != -1) && (بہترین.ادھار == -1 || cur.len > بہترین.len))
    بہترین = cur;
  إذا(بہترین.ادھار != -1 && بہترین.len < 2)
    بہترین.ادھار = -1;
  /* Format the result. */
  tp = tmp;
  لكل(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    /* Are we inside the best run of 0x00's? */
    إذا(بہترین.ادھار != -1 && i >= بہترین.ادھار && i < (بہترین.ادھار + بہترین.len)) {
      إذا(i == بہترین.ادھار)
        *tp++ = ':';
      استمر;
    }

    /* Are we following an initial run of 0x00s or any real hex?
     */
    إذا(i)
      *tp++ = ':';

    /* Is this address an encapsulated IPv4?
     */
    إذا(i == 6 && بہترین.ادھار == 0 &&
       (بہترین.len == 6 || (بہترین.len == 5 && الفاظ[5] == 0xffff))) {
      CURLcode نتیجہ = inet_ntop4(src + 12, tp, حجم(tmp) - (tp - tmp));
      إذا(نتیجہ)
        أرجع نتیجہ;
      tp += لڑی_لمبائی(tp);
      اكسر;
    }
    وإلا {
      /* Lower-case digits. Cannot use the set from mprintf.c since this
         needs to work as a curlx function */
      ساكن ثابت بنانشانی حرف ldigits[] = "0123456789abcdef";

      بنانشانی صحيح w = الفاظ[i];
      /* output lowercase 16-bit hex number but ignore leading zeroes */
      إذا(w & 0xf000)
        *tp++ = ldigits[(w & 0xf000) >> 12];
      إذا(w & 0xff00)
        *tp++ = ldigits[(w & 0x0f00) >> 8];
      إذا(w & 0xfff0)
        *tp++ = ldigits[(w & 0x00f0) >> 4];
      *tp++ = ldigits[(w & 0x000f)];
    }
  }

  /* Was it a trailing run of 0x00's?
   */
  إذا(بہترین.ادھار != -1 && (بہترین.ادھار + بہترین.len) == (IN6ADDRSZ / INT16SZ))
    *tp++ = ':';

  /* Check for overflow, copy, and we are done. */
  إذا((size_t)(tp - tmp) >= ناپ)
    أرجع CURLE_TOO_LARGE;
  curlx_strcopy(dst, ناپ, tmp, tp - tmp);
  أرجع CURLE_OK;
}

/*
 * Convert a network format address to presentation format.
 *
 * Copies result to 'buf' and returns CURLcode.
 */
CURLcode curlx_inet_ntop(صحيح af, ثابت فراغ *src, حرف *buf, size_t ناپ)
{
  بدل(af) {
  حالة AF_INET:
    أرجع inet_ntop4((ثابت بنانشانی حرف *)src, buf, ناپ);
  حالة AF_INET6:
    أرجع inet_ntop6((ثابت بنانشانی حرف *)src, buf, ناپ);
  افتراضي:
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
  }
}
