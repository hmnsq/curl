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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_PARAM_H
#ಅನ್ತರ್ಭಾವ <sys/param.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "curlx/inet_pton.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

#ವ್ಯಾಖ್ಯಾನಿಸು IN6ADDRSZ       16
#ವ್ಯಾಖ್ಯಾನಿಸು INADDRSZ         4
#ವ್ಯಾಖ್ಯಾನಿಸು INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#ಆದರೆ !defined(USE_IPV6) && !defined(AF_INET6)
#ವ್ಯಾಖ್ಯಾನಿಸು AF_INET6 (AF_INET + 1)
#ಆದರೆ_ಅಂತ್ಯ

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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ inet_pton4(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *dst)
{
  ಪೂರ್ಣಾಂಕ saw_digit, octets, ch;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ tmp[INADDRSZ], *tp;

  saw_digit = 0;
  octets = 0;
  tp = tmp;
  *tp = 0;
  ಆಗ((ch = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)*src++) != '\0') {
    ಆದರೆ(ISDIGIT(ch)) {
      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ val = (*tp * 10) + (ch - '0');

      ಆದರೆ(saw_digit && *tp == 0)
        ಹಿಂತಿರುಗಿ 0;
      ಆದರೆ(val > 255)
        ಹಿಂತಿರುಗಿ 0;
      *tp = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)val;
      ಆದರೆ(!saw_digit) {
        ಆದರೆ(++octets > 4)
          ಹಿಂತಿರುಗಿ 0;
        saw_digit = 1;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ch == '.' && saw_digit) {
      ಆದರೆ(octets == 4)
        ಹಿಂತಿರುಗಿ 0;
      *++tp = 0;
      saw_digit = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಹಿಂತಿರುಗಿ 0;
  }
  ಆದರೆ(octets < 4)
    ಹಿಂತಿರುಗಿ 0;
  ಸ್ಮರಣೆ_ನಕಲು(dst, tmp, INADDRSZ);
  ಹಿಂತಿರುಗಿ 1;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ inet_pton6(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *dst)
{
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ tmp[IN6ADDRSZ], *tp, *endp, *colonp;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *curtok;
  ಪೂರ್ಣಾಂಕ ch, saw_xdigit;
  size_t val;

  memset((tp = tmp), 0, IN6ADDRSZ);
  endp = tp + IN6ADDRSZ;
  colonp = ಖಾಲಿ;
  /* Leading :: requires some special handling. */
  ಆದರೆ(*src == ':' && *++src != ':')
    ಹಿಂತಿರುಗಿ 0;
  curtok = src;
  saw_xdigit = 0;
  val = 0;
  ಆಗ((ch = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)*src++) != '\0') {
    ಆದರೆ(ISXDIGIT(ch)) {
      val <<= 4;
      val |= curlx_hexval(ch);
      ಆದರೆ(++saw_xdigit > 4)
        ಹಿಂತಿರುಗಿ 0;
      ಮುಂದುವರಿ;
    }
    ಆದರೆ(ch == ':') {
      curtok = src;
      ಆದರೆ(!saw_xdigit) {
        ಆದರೆ(colonp)
          ಹಿಂತಿರುಗಿ 0;
        colonp = tp;
        ಮುಂದುವರಿ;
      }
      ಆದರೆ(!*src)
        ಹಿಂತಿರುಗಿ 0;
      ಆದರೆ(tp + INT16SZ > endp)
        ಹಿಂತಿರುಗಿ 0;
      *tp++ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)((val >> 8) & 0xff);
      *tp++ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(val & 0xff);
      saw_xdigit = 0;
      val = 0;
      ಮುಂದುವರಿ;
    }
    ಆದರೆ(ch == '.' && ((tp + INADDRSZ) <= endp) &&
       inet_pton4(curtok, tp) > 0) {
      tp += INADDRSZ;
      saw_xdigit = 0;
      ನಿಲ್ಲಿಸು;    /* '\0' was seen by inet_pton4(). */
    }
    ಹಿಂತಿರುಗಿ 0;
  }
  ಆದರೆ(saw_xdigit) {
    ಆದರೆ(tp + INT16SZ > endp)
      ಹಿಂತಿರುಗಿ 0;
    *tp++ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)((val >> 8) & 0xff);
    *tp++ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(val & 0xff);
  }
  ಆದರೆ(colonp) {
    /*
     * Since some memmove()'s erroneously fail to handle
     * overlapping regions, we do the shift by hand.
     */
    ಸ್ಥಿರಾಂಕ ssize_t n = tp - colonp;
    ssize_t i;

    ಆದರೆ(tp == endp)
      ಹಿಂತಿರುಗಿ 0;
    ಗಾಗಿ(i = 1; i <= n; i++) {
      *(endp - i) = *(colonp + n - i);
      *(colonp + n - i) = 0;
    }
    tp = endp;
  }
  ಆದರೆ(tp != endp)
    ಹಿಂತಿರುಗಿ 0;
  ಸ್ಮರಣೆ_ನಕಲು(dst, tmp, IN6ADDRSZ);
  ಹಿಂತಿರುಗಿ 1;
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
ಪೂರ್ಣಾಂಕ curlx_inet_pton(ಪೂರ್ಣಾಂಕ af, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, ಶೂನ್ಯ *dst)
{
  ಆಯ್ಕೆ(af) {
  ಸ್ಥಿತಿ AF_INET:
    ಹಿಂತಿರುಗಿ inet_pton4(src, (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)dst);
  ಸ್ಥಿತಿ AF_INET6:
    ಹಿಂತಿರುಗಿ inet_pton6(src, (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)dst);
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ -1;
  }
  /* NOTREACHED */
}
