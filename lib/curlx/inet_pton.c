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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_PARAM_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/param.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/inet_pton.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IN6ADDRSZ       16
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INADDRSZ         4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#ਜੇ !defined(USE_IPV6) && !defined(AF_INET6)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AF_INET6 (AF_INET + 1)
#ਜੇ_ਅੰਤ

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
ਸਥਿਰ ਪੂਰਨ inet_pton4(ਅਚਲ ਅੱਖਰ *src, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *dst)
{
  ਪੂਰਨ saw_digit, octets, ch;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ tmp[INADDRSZ], *tp;

  saw_digit = 0;
  octets = 0;
  tp = tmp;
  *tp = 0;
  ਜਦੋਂ((ch = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)*src++) != '\0') {
    ਜੇ(ISDIGIT(ch)) {
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ val = (*tp * 10) + (ch - '0');

      ਜੇ(saw_digit && *tp == 0)
        ਵਾਪਸ 0;
      ਜੇ(val > 255)
        ਵਾਪਸ 0;
      *tp = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)val;
      ਜੇ(!saw_digit) {
        ਜੇ(++octets > 4)
          ਵਾਪਸ 0;
        saw_digit = 1;
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ(ch == '.' && saw_digit) {
      ਜੇ(octets == 4)
        ਵਾਪਸ 0;
      *++tp = 0;
      saw_digit = 0;
    }
    ਨਹੀਂਤਾਂ
      ਵਾਪਸ 0;
  }
  ਜੇ(octets < 4)
    ਵਾਪਸ 0;
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(dst, tmp, INADDRSZ);
  ਵਾਪਸ 1;
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
ਸਥਿਰ ਪੂਰਨ inet_pton6(ਅਚਲ ਅੱਖਰ *src, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *dst)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ tmp[IN6ADDRSZ], *tp, *endp, *colonp;
  ਅਚਲ ਅੱਖਰ *curtok;
  ਪੂਰਨ ch, saw_xdigit;
  size_t val;

  memset((tp = tmp), 0, IN6ADDRSZ);
  endp = tp + IN6ADDRSZ;
  colonp = ਨਿਰਾ;
  /* Leading :: requires some special handling. */
  ਜੇ(*src == ':' && *++src != ':')
    ਵਾਪਸ 0;
  curtok = src;
  saw_xdigit = 0;
  val = 0;
  ਜਦੋਂ((ch = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)*src++) != '\0') {
    ਜੇ(ISXDIGIT(ch)) {
      val <<= 4;
      val |= curlx_hexval(ch);
      ਜੇ(++saw_xdigit > 4)
        ਵਾਪਸ 0;
      ਜਾਰੀ;
    }
    ਜੇ(ch == ':') {
      curtok = src;
      ਜੇ(!saw_xdigit) {
        ਜੇ(colonp)
          ਵਾਪਸ 0;
        colonp = tp;
        ਜਾਰੀ;
      }
      ਜੇ(!*src)
        ਵਾਪਸ 0;
      ਜੇ(tp + INT16SZ > endp)
        ਵਾਪਸ 0;
      *tp++ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)((val >> 8) & 0xff);
      *tp++ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(val & 0xff);
      saw_xdigit = 0;
      val = 0;
      ਜਾਰੀ;
    }
    ਜੇ(ch == '.' && ((tp + INADDRSZ) <= endp) &&
       inet_pton4(curtok, tp) > 0) {
      tp += INADDRSZ;
      saw_xdigit = 0;
      ਰੋਕੋ;    /* '\0' was seen by inet_pton4(). */
    }
    ਵਾਪਸ 0;
  }
  ਜੇ(saw_xdigit) {
    ਜੇ(tp + INT16SZ > endp)
      ਵਾਪਸ 0;
    *tp++ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)((val >> 8) & 0xff);
    *tp++ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(val & 0xff);
  }
  ਜੇ(colonp) {
    /*
     * Since some memmove()'s erroneously fail to handle
     * overlapping regions, we do the shift by hand.
     */
    ਅਚਲ ssize_t n = tp - colonp;
    ssize_t i;

    ਜੇ(tp == endp)
      ਵਾਪਸ 0;
    ਲਈ(i = 1; i <= n; i++) {
      *(endp - i) = *(colonp + n - i);
      *(colonp + n - i) = 0;
    }
    tp = endp;
  }
  ਜੇ(tp != endp)
    ਵਾਪਸ 0;
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(dst, tmp, IN6ADDRSZ);
  ਵਾਪਸ 1;
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
ਪੂਰਨ curlx_inet_pton(ਪੂਰਨ af, ਅਚਲ ਅੱਖਰ *src, ਖਾਲੀ *dst)
{
  ਚੋਣ(af) {
  ਹਾਲਤ AF_INET:
    ਵਾਪਸ inet_pton4(src, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)dst);
  ਹਾਲਤ AF_INET6:
    ਵਾਪਸ inet_pton6(src, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)dst);
  ਮੂਲ:
    ਵਾਪਸ -1;
  }
  /* NOTREACHED */
}
