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

#اگر_تعریف_شدہ HAVE_SYS_PARAM_H
#شامل <sys/param.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم

#شامل "curlx/inet_ntop.h"
#شامل "curlx/snprintf.h"
#شامل "curlx/strcopy.h"

#تعریف_کرو IN6ADDRSZ       16
/* #define INADDRSZ         4 */
#تعریف_کرو INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#اگر !defined(USE_IPV6) && !defined(AF_INET6)
#تعریف_کرو AF_INET6 (AF_INET + 1)
#اگر_ختم

/*
 * Format an IPv4 address, more or less like inet_ntop().
 *
 * Returns CURLcode.
 * Note:
 *  - uses no static variables
 *  - takes an unsigned char* not an in_addr as input
 */
ساکن CURLcode inet_ntop4(ثابت غیرعلامتی حرف *src, حرف *dst, size_t سائز)
{
  حرف tmp[حجم("255.255.255.255")];
  size_t len;

  DEBUGASSERT(سائز >= 16);

  /* this snprintf() does not overflow the buffer. */
  SNPRINTF(tmp, حجم(tmp), "%d.%d.%d.%d",
           ((صحیح)((غیرعلامتی حرف)src[0])) & 0xff,
           ((صحیح)((غیرعلامتی حرف)src[1])) & 0xff,
           ((صحیح)((غیرعلامتی حرف)src[2])) & 0xff,
           ((صحیح)((غیرعلامتی حرف)src[3])) & 0xff);

  len = لڑی_لمبائی(tmp);
  اگر(len == 0 || len >= سائز)
    واپس CURLE_TOO_LARGE;
  curlx_strcopy(dst, سائز, tmp, len);
  واپس CURLE_OK;
}

/*
 * Convert IPv6 binary address into presentation (printable) format.
 */
ساکن CURLcode inet_ntop6(ثابت غیرعلامتی حرف *src, حرف *dst, size_t سائز)
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
  ساخت {
    صحیح بنیاد;
    صحیح len;
  } بہترین, cur;
  غیرعلامتی صحیح الفاظ[IN6ADDRSZ / INT16SZ];
  صحیح i;

  /* Preprocess:
   *  Copy the input (bytewise) array into a wordwise array.
   *  Find the longest run of 0x00's in src[] for :: shorthanding.
   */
  memset(الفاظ, '\0', حجم(الفاظ));
  برائے(i = 0; i < IN6ADDRSZ; i++)
    الفاظ[i / 2] |= ((غیرعلامتی صحیح)src[i] << ((1 - (i % 2)) << 3));

  بہترین.بنیاد = -1;
  cur.بنیاد = -1;
  بہترین.len = 0;
  cur.len = 0;

  برائے(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    اگر(الفاظ[i] == 0) {
      اگر(cur.بنیاد == -1) {
        cur.بنیاد = i;
        cur.len = 1;
      }
      ورنہ
        cur.len++;
    }
    ورنہ اگر(cur.بنیاد != -1) {
      اگر(بہترین.بنیاد == -1 || cur.len > بہترین.len)
        بہترین = cur;
      cur.بنیاد = -1;
    }
  }
  اگر((cur.بنیاد != -1) && (بہترین.بنیاد == -1 || cur.len > بہترین.len))
    بہترین = cur;
  اگر(بہترین.بنیاد != -1 && بہترین.len < 2)
    بہترین.بنیاد = -1;
  /* Format the result. */
  tp = tmp;
  برائے(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    /* Are we inside the best run of 0x00's? */
    اگر(بہترین.بنیاد != -1 && i >= بہترین.بنیاد && i < (بہترین.بنیاد + بہترین.len)) {
      اگر(i == بہترین.بنیاد)
        *tp++ = ':';
      جاری;
    }

    /* Are we following an initial run of 0x00s or any real hex?
     */
    اگر(i)
      *tp++ = ':';

    /* Is this address an encapsulated IPv4?
     */
    اگر(i == 6 && بہترین.بنیاد == 0 &&
       (بہترین.len == 6 || (بہترین.len == 5 && الفاظ[5] == 0xffff))) {
      CURLcode نتیجہ = inet_ntop4(src + 12, tp, حجم(tmp) - (tp - tmp));
      اگر(نتیجہ)
        واپس نتیجہ;
      tp += لڑی_لمبائی(tp);
      توڑو;
    }
    ورنہ {
      /* Lower-case digits. Cannot use the set from mprintf.c since this
         needs to work as a curlx function */
      ساکن ثابت غیرعلامتی حرف ldigits[] = "0123456789abcdef";

      غیرعلامتی صحیح w = الفاظ[i];
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
  اگر(بہترین.بنیاد != -1 && (بہترین.بنیاد + بہترین.len) == (IN6ADDRSZ / INT16SZ))
    *tp++ = ':';

  /* Check for overflow, copy, and we are done. */
  اگر((size_t)(tp - tmp) >= سائز)
    واپس CURLE_TOO_LARGE;
  curlx_strcopy(dst, سائز, tmp, tp - tmp);
  واپس CURLE_OK;
}

/*
 * Convert a network format address to presentation format.
 *
 * Copies result to 'buf' and returns CURLcode.
 */
CURLcode curlx_inet_ntop(صحیح af, ثابت خالی *src, حرف *buf, size_t سائز)
{
  انتخاب(af) {
  حالت AF_INET:
    واپس inet_ntop4((ثابت غیرعلامتی حرف *)src, buf, سائز);
  حالت AF_INET6:
    واپس inet_ntop6((ثابت غیرعلامتی حرف *)src, buf, سائز);
  طےشدہ:
    واپس CURLE_UNSUPPORTED_PROTOCOL;
  }
}
