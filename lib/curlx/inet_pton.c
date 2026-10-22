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
#অন্তর্ভাব "curl_setup.h"

#ifdef HAVE_SYS_PARAM_H
#অন্তর্ভাব <sys/param.h>
#endif
#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif

#অন্তর্ভাব "curlx/inet_pton.h"
#অন্তর্ভাব "curlx/strparse.h"

#define IN6ADDRSZ       16
#define INADDRSZ         4
#define INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#যদি !defined(USE_IPV6) && !defined(AF_INET6)
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
স্থির পূর্ণসংখ্যা inet_pton4(ধ্রুবক অক্ষর *src, unsigned অক্ষর *dst)
{
  পূর্ণসংখ্যা saw_digit, octets, ch;
  unsigned অক্ষর tmp[INADDRSZ], *tp;

  saw_digit = 0;
  octets = 0;
  tp = tmp;
  *tp = 0;
  যখন((ch = (unsigned অক্ষর)*src++) != '\0') {
    যদি(ISDIGIT(ch)) {
      unsigned পূর্ণসংখ্যা val = (*tp * 10) + (ch - '0');

      যদি(saw_digit && *tp == 0)
        ফেরত 0;
      যদি(val > 255)
        ফেরত 0;
      *tp = (unsigned অক্ষর)val;
      যদি(!saw_digit) {
        যদি(++octets > 4)
          ফেরত 0;
        saw_digit = 1;
      }
    }
    অন্যথা যদি(ch == '.' && saw_digit) {
      যদি(octets == 4)
        ফেরত 0;
      *++tp = 0;
      saw_digit = 0;
    }
    অন্যথা
      ফেরত 0;
  }
  যদি(octets < 4)
    ফেরত 0;
  memcpy(dst, tmp, INADDRSZ);
  ফেরত 1;
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
স্থির পূর্ণসংখ্যা inet_pton6(ধ্রুবক অক্ষর *src, unsigned অক্ষর *dst)
{
  unsigned অক্ষর tmp[IN6ADDRSZ], *tp, *endp, *colonp;
  ধ্রুবক অক্ষর *curtok;
  পূর্ণসংখ্যা ch, saw_xdigit;
  size_t val;

  memset((tp = tmp), 0, IN6ADDRSZ);
  endp = tp + IN6ADDRSZ;
  colonp = ফাঁকা;
  /* Leading :: requires some special handling. */
  যদি(*src == ':' && *++src != ':')
    ফেরত 0;
  curtok = src;
  saw_xdigit = 0;
  val = 0;
  যখন((ch = (unsigned অক্ষর)*src++) != '\0') {
    যদি(ISXDIGIT(ch)) {
      val <<= 4;
      val |= curlx_hexval(ch);
      যদি(++saw_xdigit > 4)
        ফেরত 0;
      চালু;
    }
    যদি(ch == ':') {
      curtok = src;
      যদি(!saw_xdigit) {
        যদি(colonp)
          ফেরত 0;
        colonp = tp;
        চালু;
      }
      যদি(!*src)
        ফেরত 0;
      যদি(tp + INT16SZ > endp)
        ফেরত 0;
      *tp++ = (unsigned অক্ষর)((val >> 8) & 0xff);
      *tp++ = (unsigned অক্ষর)(val & 0xff);
      saw_xdigit = 0;
      val = 0;
      চালু;
    }
    যদি(ch == '.' && ((tp + INADDRSZ) <= endp) &&
       inet_pton4(curtok, tp) > 0) {
      tp += INADDRSZ;
      saw_xdigit = 0;
      বিরতি;    /* '\0' was seen by inet_pton4(). */
    }
    ফেরত 0;
  }
  যদি(saw_xdigit) {
    যদি(tp + INT16SZ > endp)
      ফেরত 0;
    *tp++ = (unsigned অক্ষর)((val >> 8) & 0xff);
    *tp++ = (unsigned অক্ষর)(val & 0xff);
  }
  যদি(colonp) {
    /*
     * Since some memmove()'s erroneously fail to handle
     * overlapping regions, we do the shift by hand.
     */
    ধ্রুবক ssize_t n = tp - colonp;
    ssize_t i;

    যদি(tp == endp)
      ফেরত 0;
    জন্য(i = 1; i <= n; i++) {
      *(endp - i) = *(colonp + n - i);
      *(colonp + n - i) = 0;
    }
    tp = endp;
  }
  যদি(tp != endp)
    ফেরত 0;
  memcpy(dst, tmp, IN6ADDRSZ);
  ফেরত 1;
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
পূর্ণসংখ্যা curlx_inet_pton(পূর্ণসংখ্যা af, ধ্রুবক অক্ষর *src, শূন্য *dst)
{
  নির্বাচন(af) {
  ক্ষেত্র AF_INET:
    ফেরত inet_pton4(src, (unsigned অক্ষর *)dst);
  ক্ষেত্র AF_INET6:
    ফেরত inet_pton6(src, (unsigned অক্ষর *)dst);
  পূর্বনির্ধারিত:
    ফেরত -1;
  }
  /* NOTREACHED */
}
