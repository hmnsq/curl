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
#অন্তর্ভাৱ "curl_setup.h"

#ifdef HAVE_SYS_PARAM_H
#অন্তর্ভাৱ <sys/param.h>
#endif
#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাৱ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাৱ <arpa/inet.h>
#endif

#অন্তর্ভাৱ "curlx/inet_pton.h"
#অন্তর্ভাৱ "curlx/strparse.h"

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
স্থির পূর্ণাঙ্ক inet_pton4(ধ্রুৱ ৱর্ণ *src, unsigned ৱর্ণ *dst)
{
  পূর্ণাঙ্ক saw_digit, octets, ch;
  unsigned ৱর্ণ tmp[INADDRSZ], *tp;

  saw_digit = 0;
  octets = 0;
  tp = tmp;
  *tp = 0;
  যেতিয়ালৈ((ch = (unsigned ৱর্ণ)*src++) != '\0') {
    যদি(ISDIGIT(ch)) {
      unsigned পূর্ণাঙ্ক val = (*tp * 10) + (ch - '0');

      যদি(saw_digit && *tp == 0)
        ঘূৰাই 0;
      যদি(val > 255)
        ঘূৰাই 0;
      *tp = (unsigned ৱর্ণ)val;
      যদি(!saw_digit) {
        যদি(++octets > 4)
          ঘূৰাই 0;
        saw_digit = 1;
      }
    }
    অন্যথা যদি(ch == '.' && saw_digit) {
      যদি(octets == 4)
        ঘূৰাই 0;
      *++tp = 0;
      saw_digit = 0;
    }
    অন্যথা
      ঘূৰাই 0;
  }
  যদি(octets < 4)
    ঘূৰাই 0;
  memcpy(dst, tmp, INADDRSZ);
  ঘূৰাই 1;
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
স্থির পূর্ণাঙ্ক inet_pton6(ধ্রুৱ ৱর্ণ *src, unsigned ৱর্ণ *dst)
{
  unsigned ৱর্ণ tmp[IN6ADDRSZ], *tp, *endp, *colonp;
  ধ্রুৱ ৱর্ণ *curtok;
  পূর্ণাঙ্ক ch, saw_xdigit;
  size_t val;

  memset((tp = tmp), 0, IN6ADDRSZ);
  endp = tp + IN6ADDRSZ;
  colonp = রিক্ত;
  /* Leading :: requires some special handling. */
  যদি(*src == ':' && *++src != ':')
    ঘূৰাই 0;
  curtok = src;
  saw_xdigit = 0;
  val = 0;
  যেতিয়ালৈ((ch = (unsigned ৱর্ণ)*src++) != '\0') {
    যদি(ISXDIGIT(ch)) {
      val <<= 4;
      val |= curlx_hexval(ch);
      যদি(++saw_xdigit > 4)
        ঘূৰাই 0;
      অনুৱর্ত;
    }
    যদি(ch == ':') {
      curtok = src;
      যদি(!saw_xdigit) {
        যদি(colonp)
          ঘূৰাই 0;
        colonp = tp;
        অনুৱর্ত;
      }
      যদি(!*src)
        ঘূৰাই 0;
      যদি(tp + INT16SZ > endp)
        ঘূৰাই 0;
      *tp++ = (unsigned ৱর্ণ)((val >> 8) & 0xff);
      *tp++ = (unsigned ৱর্ণ)(val & 0xff);
      saw_xdigit = 0;
      val = 0;
      অনুৱর্ত;
    }
    যদি(ch == '.' && ((tp + INADDRSZ) <= endp) &&
       inet_pton4(curtok, tp) > 0) {
      tp += INADDRSZ;
      saw_xdigit = 0;
      ভঙ্গ;    /* '\0' was seen by inet_pton4(). */
    }
    ঘূৰাই 0;
  }
  যদি(saw_xdigit) {
    যদি(tp + INT16SZ > endp)
      ঘূৰাই 0;
    *tp++ = (unsigned ৱর্ণ)((val >> 8) & 0xff);
    *tp++ = (unsigned ৱর্ণ)(val & 0xff);
  }
  যদি(colonp) {
    /*
     * Since some memmove()'s erroneously fail to handle
     * overlapping regions, we do the shift by hand.
     */
    ধ্রুৱ ssize_t n = tp - colonp;
    ssize_t i;

    যদি(tp == endp)
      ঘূৰাই 0;
    বাবে(i = 1; i <= n; i++) {
      *(endp - i) = *(colonp + n - i);
      *(colonp + n - i) = 0;
    }
    tp = endp;
  }
  যদি(tp != endp)
    ঘূৰাই 0;
  memcpy(dst, tmp, IN6ADDRSZ);
  ঘূৰাই 1;
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
পূর্ণাঙ্ক curlx_inet_pton(পূর্ণাঙ্ক af, ধ্রুৱ ৱর্ণ *src, শূন্য *dst)
{
  ৱিকল্প(af) {
  পক্ষ AF_INET:
    ঘূৰাই inet_pton4(src, (unsigned ৱর্ণ *)dst);
  পক্ষ AF_INET6:
    ঘূৰাই inet_pton6(src, (unsigned ৱর্ণ *)dst);
  স্ৱভাৱ:
    ঘূৰাই -1;
  }
  /* NOTREACHED */
}
