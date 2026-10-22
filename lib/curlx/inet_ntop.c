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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef HAVE_SYS_PARAM_H
#ଅନ୍ତର୍ଭାଵ <sys/param.h>
#endif
#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif

#ଅନ୍ତର୍ଭାଵ "curlx/inet_ntop.h"
#ଅନ୍ତର୍ଭାଵ "curlx/snprintf.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strcopy.h"

#define IN6ADDRSZ       16
/* #define INADDRSZ         4 */
#define INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#ଯଦି !defined(USE_IPV6) && !defined(AF_INET6)
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
ସ୍ଥିର CURLcode inet_ntop4(ଅଚଳ unsigned ଅକ୍ଷର *src, ଅକ୍ଷର *dst, size_t size)
{
  ଅକ୍ଷର tmp[ଆକାର("255.255.255.255")];
  size_t len;

  DEBUGASSERT(size >= 16);

  /* this snprintf() does not overflow the buffer. */
  SNPRINTF(tmp, ଆକାର(tmp), "%d.%d.%d.%d",
           ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((unsigned ଅକ୍ଷର)src[0])) & 0xff,
           ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((unsigned ଅକ୍ଷର)src[1])) & 0xff,
           ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((unsigned ଅକ୍ଷର)src[2])) & 0xff,
           ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((unsigned ଅକ୍ଷର)src[3])) & 0xff);

  len = strlen(tmp);
  ଯଦି(len == 0 || len >= size)
    ଫେରନ୍ତୁ CURLE_TOO_LARGE;
  curlx_strcopy(dst, size, tmp, len);
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Convert IPv6 binary address into presentation (printable) format.
 */
ସ୍ଥିର CURLcode inet_ntop6(ଅଚଳ unsigned ଅକ୍ଷର *src, ଅକ୍ଷର *dst, size_t size)
{
  /*
   * Note that int32_t and int16_t need only be "at least" large enough
   * to contain a value of the specified size. On some systems, like
   * Crays, there is no such thing as an integer variable with 16 bits.
   * Keep this in mind if you think this function should have been coded
   * to use pointer overlays. All the world's not a VAX.
   */
  ଅକ୍ଷର tmp[ଆକାର("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")];
  ଅକ୍ଷର *tp;
  ସଂରଚନା {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା base;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;
  } best, cur;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଶବ୍ଦଗୁଡ଼ିକ[IN6ADDRSZ / INT16SZ];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

  /* Preprocess:
   *  Copy the input (bytewise) array into a wordwise array.
   *  Find the longest run of 0x00's in src[] for :: shorthanding.
   */
  memset(ଶବ୍ଦଗୁଡ଼ିକ, '\0', ଆକାର(ଶବ୍ଦଗୁଡ଼ିକ));
  ପାଇଁ(i = 0; i < IN6ADDRSZ; i++)
    ଶବ୍ଦଗୁଡ଼ିକ[i / 2] |= ((unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)src[i] << ((1 - (i % 2)) << 3));

  best.base = -1;
  cur.base = -1;
  best.len = 0;
  cur.len = 0;

  ପାଇଁ(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    ଯଦି(ଶବ୍ଦଗୁଡ଼ିକ[i] == 0) {
      ଯଦି(cur.base == -1) {
        cur.base = i;
        cur.len = 1;
      }
      ଅନ୍ୟଥା
        cur.len++;
    }
    ଅନ୍ୟଥା ଯଦି(cur.base != -1) {
      ଯଦି(best.base == -1 || cur.len > best.len)
        best = cur;
      cur.base = -1;
    }
  }
  ଯଦି((cur.base != -1) && (best.base == -1 || cur.len > best.len))
    best = cur;
  ଯଦି(best.base != -1 && best.len < 2)
    best.base = -1;
  /* Format the result. */
  tp = tmp;
  ପାଇଁ(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    /* Are we inside the best run of 0x00's? */
    ଯଦି(best.base != -1 && i >= best.base && i < (best.base + best.len)) {
      ଯଦି(i == best.base)
        *tp++ = ':';
      ଜାରି;
    }

    /* Are we following an initial run of 0x00s or any real hex?
     */
    ଯଦି(i)
      *tp++ = ':';

    /* Is this address an encapsulated IPv4?
     */
    ଯଦି(i == 6 && best.base == 0 &&
       (best.len == 6 || (best.len == 5 && ଶବ୍ଦଗୁଡ଼ିକ[5] == 0xffff))) {
      CURLcode result = inet_ntop4(src + 12, tp, ଆକାର(tmp) - (tp - tmp));
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
      tp += strlen(tp);
      ଭାଙ୍ଗ;
    }
    ଅନ୍ୟଥା {
      /* Lower-case digits. Cannot use the set from mprintf.c since this
         needs to work as a curlx function */
      ସ୍ଥିର ଅଚଳ unsigned ଅକ୍ଷର ldigits[] = "0123456789abcdef";

      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା w = ଶବ୍ଦଗୁଡ଼ିକ[i];
      /* output lowercase 16-bit hex number but ignore leading zeroes */
      ଯଦି(w & 0xf000)
        *tp++ = ldigits[(w & 0xf000) >> 12];
      ଯଦି(w & 0xff00)
        *tp++ = ldigits[(w & 0x0f00) >> 8];
      ଯଦି(w & 0xfff0)
        *tp++ = ldigits[(w & 0x00f0) >> 4];
      *tp++ = ldigits[(w & 0x000f)];
    }
  }

  /* Was it a trailing run of 0x00's?
   */
  ଯଦି(best.base != -1 && (best.base + best.len) == (IN6ADDRSZ / INT16SZ))
    *tp++ = ':';

  /* Check for overflow, copy, and we are done. */
  ଯଦି((size_t)(tp - tmp) >= size)
    ଫେରନ୍ତୁ CURLE_TOO_LARGE;
  curlx_strcopy(dst, size, tmp, tp - tmp);
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Convert a network format address to presentation format.
 *
 * Copies result to 'buf' and returns CURLcode.
 */
CURLcode curlx_inet_ntop(ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, ଅଚଳ ଶୂନ୍ୟ *src, ଅକ୍ଷର *buf, size_t size)
{
  ବାଛ(af) {
  ସ୍ଥିତି AF_INET:
    ଫେରନ୍ତୁ inet_ntop4((ଅଚଳ unsigned ଅକ୍ଷର *)src, buf, size);
  ସ୍ଥିତି AF_INET6:
    ଫେରନ୍ତୁ inet_ntop6((ଅଚଳ unsigned ଅକ୍ଷର *)src, buf, size);
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_UNSUPPORTED_PROTOCOL;
  }
}
