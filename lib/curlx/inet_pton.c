/* This is from the BIND 4.9.4 release, modified to compile by itself */

/* Copyright (c) Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL INTERNET SOFTWARE
 * CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 * SPDX-License-Identifier: ISC
 */
#ضمن "curl_setup.h"

#ifdef HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#endif
#ifdef HAVE_NETINET_IN_H
#ضمن <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#endif

#ضمن "curlx/inet_pton.h"
#ضمن "curlx/strparse.h"

#define IN6ADDRSZ       16
#define INADDRSZ         4
#define INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#إذا !defined(USE_IPV6) && !defined(AF_INET6)
#define AF_INET6 (AF_INET + 1)
#endif

/*
 * WARNING: Do not even consider trying to compile this on a system where
 * sizeof(int) < 4. sizeof(int) > 4 is fine; all the world's not a VAX.
 */

/* int inet_pton4(src, dst)
 *      like inet_aton() but without all the hexadecimal and shorthand.
 * return:
 *      1 if `src' is a valid dotted quad, else 0.
 * notice:
 *      does not touch `dst' unless it is returning 1.
 * author:
 *      Paul Vixie, 1996.
 */
ساكن صحيح inet_pton4(ثابت حرف *src, unsigned حرف *dst)
{
  صحيح saw_digit, octets, ch;
  unsigned حرف tmp[INADDRSZ], *tp;

  saw_digit = 0;
  octets = 0;
  tp = tmp;
  *tp = 0;
  بينما((ch = (unsigned حرف)*src++) != '\0') {
    إذا(ISDIGIT(ch)) {
      unsigned صحيح val = (*tp * 10) + (ch - '0');

      إذا(saw_digit && *tp == 0)
        إرجاع 0;
      إذا(val > 255)
        إرجاع 0;
      *tp = (unsigned حرف)val;
      إذا(!saw_digit) {
        إذا(++octets > 4)
          إرجاع 0;
        saw_digit = 1;
      }
    }
    وإلا إذا(ch == '.' && saw_digit) {
      إذا(octets == 4)
        إرجاع 0;
      *++tp = 0;
      saw_digit = 0;
    }
    وإلا
      إرجاع 0;
  }
  إذا(octets < 4)
    إرجاع 0;
  memcpy(dst, tmp, INADDRSZ);
  إرجاع 1;
}

/* int inet_pton6(src, dst)
 *      convert presentation level address to network order binary form.
 * return:
 *      1 if `src' is a valid [RFC1884 2.2] address, else 0.
 * notice:
 *      (1) does not touch `dst' unless it is returning 1.
 *      (2) :: in a full address is silently ignored.
 * credit:
 *      inspired by Mark Andrews.
 * author:
 *      Paul Vixie, 1996.
 */
ساكن صحيح inet_pton6(ثابت حرف *src, unsigned حرف *dst)
{
  unsigned حرف tmp[IN6ADDRSZ], *tp, *endp, *colonp;
  ثابت حرف *curtok;
  صحيح ch, saw_xdigit;
  size_t val;

  memset((tp = tmp), 0, IN6ADDRSZ);
  endp = tp + IN6ADDRSZ;
  colonp = عدم;
  /* Leading :: requires some special handling. */
  إذا(*src == ':' && *++src != ':')
    إرجاع 0;
  curtok = src;
  saw_xdigit = 0;
  val = 0;
  بينما((ch = (unsigned حرف)*src++) != '\0') {
    إذا(ISXDIGIT(ch)) {
      val <<= 4;
      val |= curlx_hexval(ch);
      إذا(++saw_xdigit > 4)
        إرجاع 0;
      استمر;
    }
    إذا(ch == ':') {
      curtok = src;
      إذا(!saw_xdigit) {
        إذا(colonp)
          إرجاع 0;
        colonp = tp;
        استمر;
      }
      إذا(!*src)
        إرجاع 0;
      إذا(tp + INT16SZ > endp)
        إرجاع 0;
      *tp++ = (unsigned حرف)((val >> 8) & 0xff);
      *tp++ = (unsigned حرف)(val & 0xff);
      saw_xdigit = 0;
      val = 0;
      استمر;
    }
    إذا(ch == '.' && ((tp + INADDRSZ) <= endp) &&
       inet_pton4(curtok, tp) > 0) {
      tp += INADDRSZ;
      saw_xdigit = 0;
      اكسر;    /* '\0' was seen by inet_pton4(). */
    }
    إرجاع 0;
  }
  إذا(saw_xdigit) {
    إذا(tp + INT16SZ > endp)
      إرجاع 0;
    *tp++ = (unsigned حرف)((val >> 8) & 0xff);
    *tp++ = (unsigned حرف)(val & 0xff);
  }
  إذا(colonp) {
    /*
     * Since some memmove()'s erroneously fail to handle
     * overlapping regions, we do the shift by hand.
     */
    ثابت ssize_t n = tp - colonp;
    ssize_t i;

    إذا(tp == endp)
      إرجاع 0;
    لكل(i = 1; i <= n; i++) {
      *(endp - i) = *(colonp + n - i);
      *(colonp + n - i) = 0;
    }
    tp = endp;
  }
  إذا(tp != endp)
    إرجاع 0;
  memcpy(dst, tmp, IN6ADDRSZ);
  إرجاع 1;
}

/*
 * Convert from presentation format (which usually means ASCII printable)
 * to network format (which is usually some kind of binary format).
 *
 * Return:
 *   1 if the address was valid for the specified address family
 *   0 if the address was not valid (`dst' is untouched in this case)
 *  -1 if some other error occurred (`dst' is untouched in this case, too)
 *
 * author:
 *      Paul Vixie, 1996.
 */
صحيح curlx_inet_pton(صحيح af, ثابت حرف *src, فراغ *dst)
{
  بدل(af) {
  حالة AF_INET:
    إرجاع inet_pton4(src, (unsigned حرف *)dst);
  حالة AF_INET6:
    إرجاع inet_pton6(src, (unsigned حرف *)dst);
  افتراضي:
    إرجاع -1;
  }
  /* NOTREACHED */
}
