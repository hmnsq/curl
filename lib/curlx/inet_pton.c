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
#அந்தர்பாவ "curl_setup.h"

#ifdef HAVE_SYS_PARAM_H
#அந்தர்பாவ <sys/param.h>
#endif
#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif

#அந்தர்பாவ "curlx/inet_pton.h"
#அந்தர்பாவ "curlx/strparse.h"

#define IN6ADDRSZ       16
#define INADDRSZ         4
#define INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#எனில் !defined(USE_IPV6) && !defined(AF_INET6)
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
நிலையான முழு inet_pton4(மாறிலி எழுத்து *src, unsigned எழுத்து *dst)
{
  முழு saw_digit, octets, ch;
  unsigned எழுத்து tmp[INADDRSZ], *tp;

  saw_digit = 0;
  octets = 0;
  tp = tmp;
  *tp = 0;
  போது((ch = (unsigned எழுத்து)*src++) != '\0') {
    எனில்(ISDIGIT(ch)) {
      unsigned முழு val = (*tp * 10) + (ch - '0');

      எனில்(saw_digit && *tp == 0)
        திரும்பு 0;
      எனில்(val > 255)
        திரும்பு 0;
      *tp = (unsigned எழுத்து)val;
      எனில்(!saw_digit) {
        எனில்(++octets > 4)
          திரும்பு 0;
        saw_digit = 1;
      }
    }
    இல்லையெனில் எனில்(ch == '.' && saw_digit) {
      எனில்(octets == 4)
        திரும்பு 0;
      *++tp = 0;
      saw_digit = 0;
    }
    இல்லையெனில்
      திரும்பு 0;
  }
  எனில்(octets < 4)
    திரும்பு 0;
  memcpy(dst, tmp, INADDRSZ);
  திரும்பு 1;
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
நிலையான முழு inet_pton6(மாறிலி எழுத்து *src, unsigned எழுத்து *dst)
{
  unsigned எழுத்து tmp[IN6ADDRSZ], *tp, *endp, *colonp;
  மாறிலி எழுத்து *curtok;
  முழு ch, saw_xdigit;
  size_t val;

  memset((tp = tmp), 0, IN6ADDRSZ);
  endp = tp + IN6ADDRSZ;
  colonp = வெற்று;
  /* Leading :: requires some special handling. */
  எனில்(*src == ':' && *++src != ':')
    திரும்பு 0;
  curtok = src;
  saw_xdigit = 0;
  val = 0;
  போது((ch = (unsigned எழுத்து)*src++) != '\0') {
    எனில்(ISXDIGIT(ch)) {
      val <<= 4;
      val |= curlx_hexval(ch);
      எனில்(++saw_xdigit > 4)
        திரும்பு 0;
      தொடர்;
    }
    எனில்(ch == ':') {
      curtok = src;
      எனில்(!saw_xdigit) {
        எனில்(colonp)
          திரும்பு 0;
        colonp = tp;
        தொடர்;
      }
      எனில்(!*src)
        திரும்பு 0;
      எனில்(tp + INT16SZ > endp)
        திரும்பு 0;
      *tp++ = (unsigned எழுத்து)((val >> 8) & 0xff);
      *tp++ = (unsigned எழுத்து)(val & 0xff);
      saw_xdigit = 0;
      val = 0;
      தொடர்;
    }
    எனில்(ch == '.' && ((tp + INADDRSZ) <= endp) &&
       inet_pton4(curtok, tp) > 0) {
      tp += INADDRSZ;
      saw_xdigit = 0;
      நிறுத்து;    /* '\0' was seen by inet_pton4(). */
    }
    திரும்பு 0;
  }
  எனில்(saw_xdigit) {
    எனில்(tp + INT16SZ > endp)
      திரும்பு 0;
    *tp++ = (unsigned எழுத்து)((val >> 8) & 0xff);
    *tp++ = (unsigned எழுத்து)(val & 0xff);
  }
  எனில்(colonp) {
    /*
     * Since some memmove()'s erroneously fail to handle
     * overlapping regions, we do the shift by hand.
     */
    மாறிலி ssize_t n = tp - colonp;
    ssize_t i;

    எனில்(tp == endp)
      திரும்பு 0;
    க்கு(i = 1; i <= n; i++) {
      *(endp - i) = *(colonp + n - i);
      *(colonp + n - i) = 0;
    }
    tp = endp;
  }
  எனில்(tp != endp)
    திரும்பு 0;
  memcpy(dst, tmp, IN6ADDRSZ);
  திரும்பு 1;
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
முழு curlx_inet_pton(முழு af, மாறிலி எழுத்து *src, வெறுமை *dst)
{
  தேர்வு(af) {
  நிலை AF_INET:
    திரும்பு inet_pton4(src, (unsigned எழுத்து *)dst);
  நிலை AF_INET6:
    திரும்பு inet_pton6(src, (unsigned எழுத்து *)dst);
  இயல்பு:
    திரும்பு -1;
  }
  /* NOTREACHED */
}
