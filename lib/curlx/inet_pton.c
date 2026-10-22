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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef HAVE_SYS_PARAM_H
#അന്തര്ഭാവ <sys/param.h>
#endif
#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif

#അന്തര്ഭാവ "curlx/inet_pton.h"
#അന്തര്ഭാവ "curlx/strparse.h"

#define IN6ADDRSZ       16
#define INADDRSZ         4
#define INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#എങ്കിൽ !defined(USE_IPV6) && !defined(AF_INET6)
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
സ്ഥിതം പൂർണ്ണസംഖ്യ inet_pton4(സ്ഥിരം അക്ഷരം *src, unsigned അക്ഷരം *dst)
{
  പൂർണ്ണസംഖ്യ saw_digit, octets, ch;
  unsigned അക്ഷരം tmp[INADDRSZ], *tp;

  saw_digit = 0;
  octets = 0;
  tp = tmp;
  *tp = 0;
  ആകുമ്പോൾ((ch = (unsigned അക്ഷരം)*src++) != '\0') {
    എങ്കിൽ(ISDIGIT(ch)) {
      unsigned പൂർണ്ണസംഖ്യ val = (*tp * 10) + (ch - '0');

      എങ്കിൽ(saw_digit && *tp == 0)
        തിരികെ 0;
      എങ്കിൽ(val > 255)
        തിരികെ 0;
      *tp = (unsigned അക്ഷരം)val;
      എങ്കിൽ(!saw_digit) {
        എങ്കിൽ(++octets > 4)
          തിരികെ 0;
        saw_digit = 1;
      }
    }
    അല്ലെങ്കിൽ എങ്കിൽ(ch == '.' && saw_digit) {
      എങ്കിൽ(octets == 4)
        തിരികെ 0;
      *++tp = 0;
      saw_digit = 0;
    }
    അല്ലെങ്കിൽ
      തിരികെ 0;
  }
  എങ്കിൽ(octets < 4)
    തിരികെ 0;
  memcpy(dst, tmp, INADDRSZ);
  തിരികെ 1;
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
സ്ഥിതം പൂർണ്ണസംഖ്യ inet_pton6(സ്ഥിരം അക്ഷരം *src, unsigned അക്ഷരം *dst)
{
  unsigned അക്ഷരം tmp[IN6ADDRSZ], *tp, *endp, *colonp;
  സ്ഥിരം അക്ഷരം *curtok;
  പൂർണ്ണസംഖ്യ ch, saw_xdigit;
  size_t val;

  memset((tp = tmp), 0, IN6ADDRSZ);
  endp = tp + IN6ADDRSZ;
  colonp = ശൂന്യ;
  /* Leading :: requires some special handling. */
  എങ്കിൽ(*src == ':' && *++src != ':')
    തിരികെ 0;
  curtok = src;
  saw_xdigit = 0;
  val = 0;
  ആകുമ്പോൾ((ch = (unsigned അക്ഷരം)*src++) != '\0') {
    എങ്കിൽ(ISXDIGIT(ch)) {
      val <<= 4;
      val |= curlx_hexval(ch);
      എങ്കിൽ(++saw_xdigit > 4)
        തിരികെ 0;
      തുടരുക;
    }
    എങ്കിൽ(ch == ':') {
      curtok = src;
      എങ്കിൽ(!saw_xdigit) {
        എങ്കിൽ(colonp)
          തിരികെ 0;
        colonp = tp;
        തുടരുക;
      }
      എങ്കിൽ(!*src)
        തിരികെ 0;
      എങ്കിൽ(tp + INT16SZ > endp)
        തിരികെ 0;
      *tp++ = (unsigned അക്ഷരം)((val >> 8) & 0xff);
      *tp++ = (unsigned അക്ഷരം)(val & 0xff);
      saw_xdigit = 0;
      val = 0;
      തുടരുക;
    }
    എങ്കിൽ(ch == '.' && ((tp + INADDRSZ) <= endp) &&
       inet_pton4(curtok, tp) > 0) {
      tp += INADDRSZ;
      saw_xdigit = 0;
      നിർത്തുക;    /* '\0' was seen by inet_pton4(). */
    }
    തിരികെ 0;
  }
  എങ്കിൽ(saw_xdigit) {
    എങ്കിൽ(tp + INT16SZ > endp)
      തിരികെ 0;
    *tp++ = (unsigned അക്ഷരം)((val >> 8) & 0xff);
    *tp++ = (unsigned അക്ഷരം)(val & 0xff);
  }
  എങ്കിൽ(colonp) {
    /*
     * Since some memmove()'s erroneously fail to handle
     * overlapping regions, we do the shift by hand.
     */
    സ്ഥിരം ssize_t n = tp - colonp;
    ssize_t i;

    എങ്കിൽ(tp == endp)
      തിരികെ 0;
    വേണ്ടി(i = 1; i <= n; i++) {
      *(endp - i) = *(colonp + n - i);
      *(colonp + n - i) = 0;
    }
    tp = endp;
  }
  എങ്കിൽ(tp != endp)
    തിരികെ 0;
  memcpy(dst, tmp, IN6ADDRSZ);
  തിരികെ 1;
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
പൂർണ്ണസംഖ്യ curlx_inet_pton(പൂർണ്ണസംഖ്യ af, സ്ഥിരം അക്ഷരം *src, ശൂന്യം *dst)
{
  തിരഞ്ഞെടുപ്പ്(af) {
  അവസ്ഥ AF_INET:
    തിരികെ inet_pton4(src, (unsigned അക്ഷരം *)dst);
  അവസ്ഥ AF_INET6:
    തിരികെ inet_pton6(src, (unsigned അക്ഷരം *)dst);
  സ്ഥിരസ്ഥിതി:
    തിരികെ -1;
  }
  /* NOTREACHED */
}
