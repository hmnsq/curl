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

#ifdef HAVE_SYS_PARAM_H
#ಅನ್ತರ್ಭಾವ <sys/param.h>
#endif
#ifdef HAVE_NETINET_IN_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#endif

#ಅನ್ತರ್ಭಾವ "curlx/inet_pton.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

#define IN6ADDRSZ       16
#define INADDRSZ         4
#define INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#ಯದಿ !defined(USE_IPV6) && !defined(AF_INET6)
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ inet_pton4(ಧ್ರುವ ವರ್ಣ *src, unsigned ವರ್ಣ *dst)
{
  ಪೂರ್ಣಾಙ್ಕ saw_digit, octets, ch;
  unsigned ವರ್ಣ tmp[INADDRSZ], *tp;

  saw_digit = 0;
  octets = 0;
  tp = tmp;
  *tp = 0;
  ಯಾವತ್((ch = (unsigned ವರ್ಣ)*src++) != '\0') {
    ಯದಿ(ISDIGIT(ch)) {
      unsigned ಪೂರ್ಣಾಙ್ಕ val = (*tp * 10) + (ch - '0');

      ಯದಿ(saw_digit && *tp == 0)
        ಪ್ರತ್ಯಾಗಮ 0;
      ಯದಿ(val > 255)
        ಪ್ರತ್ಯಾಗಮ 0;
      *tp = (unsigned ವರ್ಣ)val;
      ಯದಿ(!saw_digit) {
        ಯದಿ(++octets > 4)
          ಪ್ರತ್ಯಾಗಮ 0;
        saw_digit = 1;
      }
    }
    ಅನ್ಯಥಾ ಯದಿ(ch == '.' && saw_digit) {
      ಯದಿ(octets == 4)
        ಪ್ರತ್ಯಾಗಮ 0;
      *++tp = 0;
      saw_digit = 0;
    }
    ಅನ್ಯಥಾ
      ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಯದಿ(octets < 4)
    ಪ್ರತ್ಯಾಗಮ 0;
  memcpy(dst, tmp, INADDRSZ);
  ಪ್ರತ್ಯಾಗಮ 1;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ inet_pton6(ಧ್ರುವ ವರ್ಣ *src, unsigned ವರ್ಣ *dst)
{
  unsigned ವರ್ಣ tmp[IN6ADDRSZ], *tp, *endp, *colonp;
  ಧ್ರುವ ವರ್ಣ *curtok;
  ಪೂರ್ಣಾಙ್ಕ ch, saw_xdigit;
  size_t val;

  memset((tp = tmp), 0, IN6ADDRSZ);
  endp = tp + IN6ADDRSZ;
  colonp = ರಿಕ್ತ;
  /* Leading :: requires some special handling. */
  ಯದಿ(*src == ':' && *++src != ':')
    ಪ್ರತ್ಯಾಗಮ 0;
  curtok = src;
  saw_xdigit = 0;
  val = 0;
  ಯಾವತ್((ch = (unsigned ವರ್ಣ)*src++) != '\0') {
    ಯದಿ(ISXDIGIT(ch)) {
      val <<= 4;
      val |= curlx_hexval(ch);
      ಯದಿ(++saw_xdigit > 4)
        ಪ್ರತ್ಯಾಗಮ 0;
      ಅನುವರ್ತ;
    }
    ಯದಿ(ch == ':') {
      curtok = src;
      ಯದಿ(!saw_xdigit) {
        ಯದಿ(colonp)
          ಪ್ರತ್ಯಾಗಮ 0;
        colonp = tp;
        ಅನುವರ್ತ;
      }
      ಯದಿ(!*src)
        ಪ್ರತ್ಯಾಗಮ 0;
      ಯದಿ(tp + INT16SZ > endp)
        ಪ್ರತ್ಯಾಗಮ 0;
      *tp++ = (unsigned ವರ್ಣ)((val >> 8) & 0xff);
      *tp++ = (unsigned ವರ್ಣ)(val & 0xff);
      saw_xdigit = 0;
      val = 0;
      ಅನುವರ್ತ;
    }
    ಯದಿ(ch == '.' && ((tp + INADDRSZ) <= endp) &&
       inet_pton4(curtok, tp) > 0) {
      tp += INADDRSZ;
      saw_xdigit = 0;
      ಭಙ್ಗ;    /* '\0' was seen by inet_pton4(). */
    }
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಯದಿ(saw_xdigit) {
    ಯದಿ(tp + INT16SZ > endp)
      ಪ್ರತ್ಯಾಗಮ 0;
    *tp++ = (unsigned ವರ್ಣ)((val >> 8) & 0xff);
    *tp++ = (unsigned ವರ್ಣ)(val & 0xff);
  }
  ಯದಿ(colonp) {
    /*
     * Since some memmove()'s erroneously fail to handle
     * overlapping regions, we do the shift by hand.
     */
    ಧ್ರುವ ssize_t n = tp - colonp;
    ssize_t i;

    ಯದಿ(tp == endp)
      ಪ್ರತ್ಯಾಗಮ 0;
    ಪ್ರತಿ(i = 1; i <= n; i++) {
      *(endp - i) = *(colonp + n - i);
      *(colonp + n - i) = 0;
    }
    tp = endp;
  }
  ಯದಿ(tp != endp)
    ಪ್ರತ್ಯಾಗಮ 0;
  memcpy(dst, tmp, IN6ADDRSZ);
  ಪ್ರತ್ಯಾಗಮ 1;
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
ಪೂರ್ಣಾಙ್ಕ curlx_inet_pton(ಪೂರ್ಣಾಙ್ಕ af, ಧ್ರುವ ವರ್ಣ *src, ಶೂನ್ಯ *dst)
{
  ವಿಕಲ್ಪ(af) {
  ಪಕ್ಷ AF_INET:
    ಪ್ರತ್ಯಾಗಮ inet_pton4(src, (unsigned ವರ್ಣ *)dst);
  ಪಕ್ಷ AF_INET6:
    ಪ್ರತ್ಯಾಗಮ inet_pton6(src, (unsigned ವರ್ಣ *)dst);
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ -1;
  }
  /* NOTREACHED */
}
