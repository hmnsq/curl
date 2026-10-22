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
#અન્તર્ભાવ "curl_setup.h"

#ifdef HAVE_SYS_PARAM_H
#અન્તર્ભાવ <sys/param.h>
#endif
#ifdef HAVE_NETINET_IN_H
#અન્તર્ભાવ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#અન્તર્ભાવ <arpa/inet.h>
#endif

#અન્તર્ભાવ "curlx/inet_ntop.h"
#અન્તર્ભાવ "curlx/snprintf.h"
#અન્તર્ભાવ "curlx/strcopy.h"

#define IN6ADDRSZ       16
/* #define INADDRSZ         4 */
#define INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#જો !defined(USE_IPV6) && !defined(AF_INET6)
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
સ્થિર CURLcode inet_ntop4(અચલ unsigned અક્ષર *src, અક્ષર *dst, size_t size)
{
  અક્ષર tmp[કદ("255.255.255.255")];
  size_t len;

  DEBUGASSERT(size >= 16);

  /* this snprintf() does not overflow the buffer. */
  SNPRINTF(tmp, કદ(tmp), "%d.%d.%d.%d",
           ((પૂર્ણાંક)((unsigned અક્ષર)src[0])) & 0xff,
           ((પૂર્ણાંક)((unsigned અક્ષર)src[1])) & 0xff,
           ((પૂર્ણાંક)((unsigned અક્ષર)src[2])) & 0xff,
           ((પૂર્ણાંક)((unsigned અક્ષર)src[3])) & 0xff);

  len = strlen(tmp);
  જો(len == 0 || len >= size)
    પાછા CURLE_TOO_LARGE;
  curlx_strcopy(dst, size, tmp, len);
  પાછા CURLE_OK;
}

/*
 * Convert IPv6 binary address into presentation (printable) format.
 */
સ્થિર CURLcode inet_ntop6(અચલ unsigned અક્ષર *src, અક્ષર *dst, size_t size)
{
  /*
   * Note that int32_t and int16_t need only be "at least" large enough
   * to contain a value of the specified size. On some systems, like
   * Crays, there is no such thing as an integer variable with 16 bits.
   * Keep this in mind if you think this function should have been coded
   * to use pointer overlays. All the world's not a VAX.
   */
  અક્ષર tmp[કદ("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")];
  અક્ષર *tp;
  સંરચના {
    પૂર્ણાંક base;
    પૂર્ણાંક len;
  } best, cur;
  unsigned પૂર્ણાંક શબ્દો[IN6ADDRSZ / INT16SZ];
  પૂર્ણાંક i;

  /* Preprocess:
   *  Copy the input (bytewise) array into a wordwise array.
   *  Find the longest run of 0x00's in src[] for :: shorthanding.
   */
  memset(શબ્દો, '\0', કદ(શબ્દો));
  માટે(i = 0; i < IN6ADDRSZ; i++)
    શબ્દો[i / 2] |= ((unsigned પૂર્ણાંક)src[i] << ((1 - (i % 2)) << 3));

  best.base = -1;
  cur.base = -1;
  best.len = 0;
  cur.len = 0;

  માટે(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    જો(શબ્દો[i] == 0) {
      જો(cur.base == -1) {
        cur.base = i;
        cur.len = 1;
      }
      નહીંતર
        cur.len++;
    }
    નહીંતર જો(cur.base != -1) {
      જો(best.base == -1 || cur.len > best.len)
        best = cur;
      cur.base = -1;
    }
  }
  જો((cur.base != -1) && (best.base == -1 || cur.len > best.len))
    best = cur;
  જો(best.base != -1 && best.len < 2)
    best.base = -1;
  /* Format the result. */
  tp = tmp;
  માટે(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    /* Are we inside the best run of 0x00's? */
    જો(best.base != -1 && i >= best.base && i < (best.base + best.len)) {
      જો(i == best.base)
        *tp++ = ':';
      ચાલુ;
    }

    /* Are we following an initial run of 0x00s or any real hex?
     */
    જો(i)
      *tp++ = ':';

    /* Is this address an encapsulated IPv4?
     */
    જો(i == 6 && best.base == 0 &&
       (best.len == 6 || (best.len == 5 && શબ્દો[5] == 0xffff))) {
      CURLcode result = inet_ntop4(src + 12, tp, કદ(tmp) - (tp - tmp));
      જો(result)
        પાછા result;
      tp += strlen(tp);
      તોડો;
    }
    નહીંતર {
      /* Lower-case digits. Cannot use the set from mprintf.c since this
         needs to work as a curlx function */
      સ્થિર અચલ unsigned અક્ષર ldigits[] = "0123456789abcdef";

      unsigned પૂર્ણાંક w = શબ્દો[i];
      /* output lowercase 16-bit hex number but ignore leading zeroes */
      જો(w & 0xf000)
        *tp++ = ldigits[(w & 0xf000) >> 12];
      જો(w & 0xff00)
        *tp++ = ldigits[(w & 0x0f00) >> 8];
      જો(w & 0xfff0)
        *tp++ = ldigits[(w & 0x00f0) >> 4];
      *tp++ = ldigits[(w & 0x000f)];
    }
  }

  /* Was it a trailing run of 0x00's?
   */
  જો(best.base != -1 && (best.base + best.len) == (IN6ADDRSZ / INT16SZ))
    *tp++ = ':';

  /* Check for overflow, copy, and we are done. */
  જો((size_t)(tp - tmp) >= size)
    પાછા CURLE_TOO_LARGE;
  curlx_strcopy(dst, size, tmp, tp - tmp);
  પાછા CURLE_OK;
}

/*
 * Convert a network format address to presentation format.
 *
 * Copies result to 'buf' and returns CURLcode.
 */
CURLcode curlx_inet_ntop(પૂર્ણાંક af, અચલ શૂન્ય *src, અક્ષર *buf, size_t size)
{
  પસંદગી(af) {
  સ્થિતિ AF_INET:
    પાછા inet_ntop4((અચલ unsigned અક્ષર *)src, buf, size);
  સ્થિતિ AF_INET6:
    પાછા inet_ntop6((અચલ unsigned અક્ષર *)src, buf, size);
  મૂળભૂત:
    પાછા CURLE_UNSUPPORTED_PROTOCOL;
  }
}
