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

#شامل "curlx/inet_pton.h"
#شامل "curlx/strparse.h"

#تعریف_کرو IN6ADDRSZ       16
#تعریف_کرو INADDRSZ         4
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
ساکن صحیح inet_pton4(ثابت حرف *src, غیرعلامتی حرف *dst)
{
  صحیح saw_digit, octets, ch;
  غیرعلامتی حرف tmp[INADDRSZ], *tp;

  saw_digit = 0;
  octets = 0;
  tp = tmp;
  *tp = 0;
  جب((ch = (غیرعلامتی حرف)*src++) != '\0') {
    اگر(ISDIGIT(ch)) {
      غیرعلامتی صحیح val = (*tp * 10) + (ch - '0');

      اگر(saw_digit && *tp == 0)
        واپس 0;
      اگر(val > 255)
        واپس 0;
      *tp = (غیرعلامتی حرف)val;
      اگر(!saw_digit) {
        اگر(++octets > 4)
          واپس 0;
        saw_digit = 1;
      }
    }
    ورنہ اگر(ch == '.' && saw_digit) {
      اگر(octets == 4)
        واپس 0;
      *++tp = 0;
      saw_digit = 0;
    }
    ورنہ
      واپس 0;
  }
  اگر(octets < 4)
    واپس 0;
  حافظہ_نقل(dst, tmp, INADDRSZ);
  واپس 1;
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
ساکن صحیح inet_pton6(ثابت حرف *src, غیرعلامتی حرف *dst)
{
  غیرعلامتی حرف tmp[IN6ADDRSZ], *tp, *endp, *colonp;
  ثابت حرف *curtok;
  صحیح ch, saw_xdigit;
  size_t val;

  memset((tp = tmp), 0, IN6ADDRSZ);
  endp = tp + IN6ADDRSZ;
  colonp = تہی;
  /* Leading :: requires some special handling. */
  اگر(*src == ':' && *++src != ':')
    واپس 0;
  curtok = src;
  saw_xdigit = 0;
  val = 0;
  جب((ch = (غیرعلامتی حرف)*src++) != '\0') {
    اگر(ISXDIGIT(ch)) {
      val <<= 4;
      val |= curlx_hexval(ch);
      اگر(++saw_xdigit > 4)
        واپس 0;
      جاری;
    }
    اگر(ch == ':') {
      curtok = src;
      اگر(!saw_xdigit) {
        اگر(colonp)
          واپس 0;
        colonp = tp;
        جاری;
      }
      اگر(!*src)
        واپس 0;
      اگر(tp + INT16SZ > endp)
        واپس 0;
      *tp++ = (غیرعلامتی حرف)((val >> 8) & 0xff);
      *tp++ = (غیرعلامتی حرف)(val & 0xff);
      saw_xdigit = 0;
      val = 0;
      جاری;
    }
    اگر(ch == '.' && ((tp + INADDRSZ) <= endp) &&
       inet_pton4(curtok, tp) > 0) {
      tp += INADDRSZ;
      saw_xdigit = 0;
      توڑو;    /* '\0' was seen by inet_pton4(). */
    }
    واپس 0;
  }
  اگر(saw_xdigit) {
    اگر(tp + INT16SZ > endp)
      واپس 0;
    *tp++ = (غیرعلامتی حرف)((val >> 8) & 0xff);
    *tp++ = (غیرعلامتی حرف)(val & 0xff);
  }
  اگر(colonp) {
    /*
     * Since some memmove()'s erroneously fail to handle
     * overlapping regions, we do the shift by hand.
     */
    ثابت ssize_t n = tp - colonp;
    ssize_t i;

    اگر(tp == endp)
      واپس 0;
    برائے(i = 1; i <= n; i++) {
      *(endp - i) = *(colonp + n - i);
      *(colonp + n - i) = 0;
    }
    tp = endp;
  }
  اگر(tp != endp)
    واپس 0;
  حافظہ_نقل(dst, tmp, IN6ADDRSZ);
  واپس 1;
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
صحیح curlx_inet_pton(صحیح af, ثابت حرف *src, خالی *dst)
{
  انتخاب(af) {
  حالت AF_INET:
    واپس inet_pton4(src, (غیرعلامتی حرف *)dst);
  حالت AF_INET6:
    واپس inet_pton6(src, (غیرعلامتی حرف *)dst);
  طےشدہ:
    واپس -1;
  }
  /* NOTREACHED */
}
