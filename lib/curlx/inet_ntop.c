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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే HAVE_SYS_PARAM_H
#అన్తర్భావ <sys/param.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు

#అన్తర్భావ "curlx/inet_ntop.h"
#అన్తర్భావ "curlx/snprintf.h"
#అన్తర్భావ "curlx/strcopy.h"

#నిర్వచించు IN6ADDRSZ       16
/* #define INADDRSZ         4 */
#నిర్వచించు INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#ఒకవేళ !defined(USE_IPV6) && !defined(AF_INET6)
#నిర్వచించు AF_INET6 (AF_INET + 1)
#అయితే_ముగింపు

/*
 * Format an IPv4 address, more or less like inet_ntop().
 *
 * Returns CURLcode.
 * Note:
 *  - uses no static variables
 *  - takes an unsigned char* not an in_addr as input
 */
స్థిర CURLcode inet_ntop4(స్థిరరాశి గుర్తులేని అక్షరం *src, అక్షరం *dst, size_t కొలత)
{
  అక్షరం tmp[పరిమాణం("255.255.255.255")];
  size_t len;

  DEBUGASSERT(కొలత >= 16);

  /* this snprintf() does not overflow the buffer. */
  SNPRINTF(tmp, పరిమాణం(tmp), "%d.%d.%d.%d",
           ((పూర్ణాంకం)((గుర్తులేని అక్షరం)src[0])) & 0xff,
           ((పూర్ణాంకం)((గుర్తులేని అక్షరం)src[1])) & 0xff,
           ((పూర్ణాంకం)((గుర్తులేని అక్షరం)src[2])) & 0xff,
           ((పూర్ణాంకం)((గుర్తులేని అక్షరం)src[3])) & 0xff);

  len = అక్షరశ్రేణి_పొడవు(tmp);
  ఒకవేళ(len == 0 || len >= కొలత)
    తిరిగి CURLE_TOO_LARGE;
  curlx_strcopy(dst, కొలత, tmp, len);
  తిరిగి CURLE_OK;
}

/*
 * Convert IPv6 binary address into presentation (printable) format.
 */
స్థిర CURLcode inet_ntop6(స్థిరరాశి గుర్తులేని అక్షరం *src, అక్షరం *dst, size_t కొలత)
{
  /*
   * Note that int32_t and int16_t need only be "at least" large enough
   * to contain a value of the specified size. On some systems, like
   * Crays, there is no such thing as an integer variable with 16 bits.
   * Keep this in mind if you think this function should have been coded
   * to use pointer overlays. All the world's not a VAX.
   */
  అక్షరం tmp[పరిమాణం("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")];
  అక్షరం *tp;
  నిర్మాణం {
    పూర్ణాంకం పునాది;
    పూర్ణాంకం len;
  } ఉత్తమ, cur;
  గుర్తులేని పూర్ణాంకం పదాలు[IN6ADDRSZ / INT16SZ];
  పూర్ణాంకం i;

  /* Preprocess:
   *  Copy the input (bytewise) array into a wordwise array.
   *  Find the longest run of 0x00's in src[] for :: shorthanding.
   */
  memset(పదాలు, '\0', పరిమాణం(పదాలు));
  కొరకు(i = 0; i < IN6ADDRSZ; i++)
    పదాలు[i / 2] |= ((గుర్తులేని పూర్ణాంకం)src[i] << ((1 - (i % 2)) << 3));

  ఉత్తమ.పునాది = -1;
  cur.పునాది = -1;
  ఉత్తమ.len = 0;
  cur.len = 0;

  కొరకు(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    ఒకవేళ(పదాలు[i] == 0) {
      ఒకవేళ(cur.పునాది == -1) {
        cur.పునాది = i;
        cur.len = 1;
      }
      లేకపోతే
        cur.len++;
    }
    లేకపోతే ఒకవేళ(cur.పునాది != -1) {
      ఒకవేళ(ఉత్తమ.పునాది == -1 || cur.len > ఉత్తమ.len)
        ఉత్తమ = cur;
      cur.పునాది = -1;
    }
  }
  ఒకవేళ((cur.పునాది != -1) && (ఉత్తమ.పునాది == -1 || cur.len > ఉత్తమ.len))
    ఉత్తమ = cur;
  ఒకవేళ(ఉత్తమ.పునాది != -1 && ఉత్తమ.len < 2)
    ఉత్తమ.పునాది = -1;
  /* Format the result. */
  tp = tmp;
  కొరకు(i = 0; i < (IN6ADDRSZ / INT16SZ); i++) {
    /* Are we inside the best run of 0x00's? */
    ఒకవేళ(ఉత్తమ.పునాది != -1 && i >= ఉత్తమ.పునాది && i < (ఉత్తమ.పునాది + ఉత్తమ.len)) {
      ఒకవేళ(i == ఉత్తమ.పునాది)
        *tp++ = ':';
      కొనసాగు;
    }

    /* Are we following an initial run of 0x00s or any real hex?
     */
    ఒకవేళ(i)
      *tp++ = ':';

    /* Is this address an encapsulated IPv4?
     */
    ఒకవేళ(i == 6 && ఉత్తమ.పునాది == 0 &&
       (ఉత్తమ.len == 6 || (ఉత్తమ.len == 5 && పదాలు[5] == 0xffff))) {
      CURLcode ఫలితం = inet_ntop4(src + 12, tp, పరిమాణం(tmp) - (tp - tmp));
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
      tp += అక్షరశ్రేణి_పొడవు(tp);
      ఆపు;
    }
    లేకపోతే {
      /* Lower-case digits. Cannot use the set from mprintf.c since this
         needs to work as a curlx function */
      స్థిర స్థిరరాశి గుర్తులేని అక్షరం ldigits[] = "0123456789abcdef";

      గుర్తులేని పూర్ణాంకం w = పదాలు[i];
      /* output lowercase 16-bit hex number but ignore leading zeroes */
      ఒకవేళ(w & 0xf000)
        *tp++ = ldigits[(w & 0xf000) >> 12];
      ఒకవేళ(w & 0xff00)
        *tp++ = ldigits[(w & 0x0f00) >> 8];
      ఒకవేళ(w & 0xfff0)
        *tp++ = ldigits[(w & 0x00f0) >> 4];
      *tp++ = ldigits[(w & 0x000f)];
    }
  }

  /* Was it a trailing run of 0x00's?
   */
  ఒకవేళ(ఉత్తమ.పునాది != -1 && (ఉత్తమ.పునాది + ఉత్తమ.len) == (IN6ADDRSZ / INT16SZ))
    *tp++ = ':';

  /* Check for overflow, copy, and we are done. */
  ఒకవేళ((size_t)(tp - tmp) >= కొలత)
    తిరిగి CURLE_TOO_LARGE;
  curlx_strcopy(dst, కొలత, tmp, tp - tmp);
  తిరిగి CURLE_OK;
}

/*
 * Convert a network format address to presentation format.
 *
 * Copies result to 'buf' and returns CURLcode.
 */
CURLcode curlx_inet_ntop(పూర్ణాంకం af, స్థిరరాశి శూన్య *src, అక్షరం *buf, size_t కొలత)
{
  ఎంపిక(af) {
  స్థితి AF_INET:
    తిరిగి inet_ntop4((స్థిరరాశి గుర్తులేని అక్షరం *)src, buf, కొలత);
  స్థితి AF_INET6:
    తిరిగి inet_ntop6((స్థిరరాశి గుర్తులేని అక్షరం *)src, buf, కొలత);
  అప్రమేయ:
    తిరిగి CURLE_UNSUPPORTED_PROTOCOL;
  }
}
