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
#अन्तर्भाव "curl_setup.h"

#ifdef HAVE_SYS_PARAM_H
#अन्तर्भाव <sys/param.h>
#endif
#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif

#अन्तर्भाव "curlx/inet_pton.h"
#अन्तर्भाव "curlx/strparse.h"

#define IN6ADDRSZ       16
#define INADDRSZ         4
#define INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#यदि !defined(USE_IPV6) && !defined(AF_INET6)
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
स्थिर पूर्णाङ्क inet_pton4(ध्रुव वर्ण *src, unsigned वर्ण *dst)
{
  पूर्णाङ्क saw_digit, octets, ch;
  unsigned वर्ण tmp[INADDRSZ], *tp;

  saw_digit = 0;
  octets = 0;
  tp = tmp;
  *tp = 0;
  यावत्((ch = (unsigned वर्ण)*src++) != '\0') {
    यदि(ISDIGIT(ch)) {
      unsigned पूर्णाङ्क val = (*tp * 10) + (ch - '0');

      यदि(saw_digit && *tp == 0)
        प्रत्यागम 0;
      यदि(val > 255)
        प्रत्यागम 0;
      *tp = (unsigned वर्ण)val;
      यदि(!saw_digit) {
        यदि(++octets > 4)
          प्रत्यागम 0;
        saw_digit = 1;
      }
    }
    अन्यथा यदि(ch == '.' && saw_digit) {
      यदि(octets == 4)
        प्रत्यागम 0;
      *++tp = 0;
      saw_digit = 0;
    }
    अन्यथा
      प्रत्यागम 0;
  }
  यदि(octets < 4)
    प्रत्यागम 0;
  memcpy(dst, tmp, INADDRSZ);
  प्रत्यागम 1;
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
स्थिर पूर्णाङ्क inet_pton6(ध्रुव वर्ण *src, unsigned वर्ण *dst)
{
  unsigned वर्ण tmp[IN6ADDRSZ], *tp, *endp, *colonp;
  ध्रुव वर्ण *curtok;
  पूर्णाङ्क ch, saw_xdigit;
  size_t val;

  memset((tp = tmp), 0, IN6ADDRSZ);
  endp = tp + IN6ADDRSZ;
  colonp = रिक्त;
  /* Leading :: requires some special handling. */
  यदि(*src == ':' && *++src != ':')
    प्रत्यागम 0;
  curtok = src;
  saw_xdigit = 0;
  val = 0;
  यावत्((ch = (unsigned वर्ण)*src++) != '\0') {
    यदि(ISXDIGIT(ch)) {
      val <<= 4;
      val |= curlx_hexval(ch);
      यदि(++saw_xdigit > 4)
        प्रत्यागम 0;
      अनुवर्त;
    }
    यदि(ch == ':') {
      curtok = src;
      यदि(!saw_xdigit) {
        यदि(colonp)
          प्रत्यागम 0;
        colonp = tp;
        अनुवर्त;
      }
      यदि(!*src)
        प्रत्यागम 0;
      यदि(tp + INT16SZ > endp)
        प्रत्यागम 0;
      *tp++ = (unsigned वर्ण)((val >> 8) & 0xff);
      *tp++ = (unsigned वर्ण)(val & 0xff);
      saw_xdigit = 0;
      val = 0;
      अनुवर्त;
    }
    यदि(ch == '.' && ((tp + INADDRSZ) <= endp) &&
       inet_pton4(curtok, tp) > 0) {
      tp += INADDRSZ;
      saw_xdigit = 0;
      भङ्ग;    /* '\0' was seen by inet_pton4(). */
    }
    प्रत्यागम 0;
  }
  यदि(saw_xdigit) {
    यदि(tp + INT16SZ > endp)
      प्रत्यागम 0;
    *tp++ = (unsigned वर्ण)((val >> 8) & 0xff);
    *tp++ = (unsigned वर्ण)(val & 0xff);
  }
  यदि(colonp) {
    /*
     * Since some memmove()'s erroneously fail to handle
     * overlapping regions, we do the shift by hand.
     */
    ध्रुव ssize_t n = tp - colonp;
    ssize_t i;

    यदि(tp == endp)
      प्रत्यागम 0;
    प्रति(i = 1; i <= n; i++) {
      *(endp - i) = *(colonp + n - i);
      *(colonp + n - i) = 0;
    }
    tp = endp;
  }
  यदि(tp != endp)
    प्रत्यागम 0;
  memcpy(dst, tmp, IN6ADDRSZ);
  प्रत्यागम 1;
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
पूर्णाङ्क curlx_inet_pton(पूर्णाङ्क af, ध्रुव वर्ण *src, शून्य *dst)
{
  विकल्प(af) {
  पक्ष AF_INET:
    प्रत्यागम inet_pton4(src, (unsigned वर्ण *)dst);
  पक्ष AF_INET6:
    प्रत्यागम inet_pton6(src, (unsigned वर्ण *)dst);
  स्वभाव:
    प्रत्यागम -1;
  }
  /* NOTREACHED */
}
