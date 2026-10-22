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
#અન્તર્ભાવ "curl_setup.h"

#ifdef HAVE_SYS_PARAM_H
#અન્તર્ભાવ <sys/param.h>
#endif
#ifdef HAVE_NETINET_IN_H
#અન્તર્ભાવ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#અન્તર્ભાવ <arpa/inet.h>
#endif

#અન્તર્ભાવ "curlx/inet_pton.h"
#અન્તર્ભાવ "curlx/strparse.h"

#define IN6ADDRSZ       16
#define INADDRSZ         4
#define INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#જો !defined(USE_IPV6) && !defined(AF_INET6)
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
સ્થિર પૂર્ણાંક inet_pton4(અચલ અક્ષર *src, unsigned અક્ષર *dst)
{
  પૂર્ણાંક saw_digit, octets, ch;
  unsigned અક્ષર tmp[INADDRSZ], *tp;

  saw_digit = 0;
  octets = 0;
  tp = tmp;
  *tp = 0;
  જ્યારે((ch = (unsigned અક્ષર)*src++) != '\0') {
    જો(ISDIGIT(ch)) {
      unsigned પૂર્ણાંક val = (*tp * 10) + (ch - '0');

      જો(saw_digit && *tp == 0)
        પાછા 0;
      જો(val > 255)
        પાછા 0;
      *tp = (unsigned અક્ષર)val;
      જો(!saw_digit) {
        જો(++octets > 4)
          પાછા 0;
        saw_digit = 1;
      }
    }
    નહીંતર જો(ch == '.' && saw_digit) {
      જો(octets == 4)
        પાછા 0;
      *++tp = 0;
      saw_digit = 0;
    }
    નહીંતર
      પાછા 0;
  }
  જો(octets < 4)
    પાછા 0;
  memcpy(dst, tmp, INADDRSZ);
  પાછા 1;
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
સ્થિર પૂર્ણાંક inet_pton6(અચલ અક્ષર *src, unsigned અક્ષર *dst)
{
  unsigned અક્ષર tmp[IN6ADDRSZ], *tp, *endp, *colonp;
  અચલ અક્ષર *curtok;
  પૂર્ણાંક ch, saw_xdigit;
  size_t val;

  memset((tp = tmp), 0, IN6ADDRSZ);
  endp = tp + IN6ADDRSZ;
  colonp = ખાલી;
  /* Leading :: requires some special handling. */
  જો(*src == ':' && *++src != ':')
    પાછા 0;
  curtok = src;
  saw_xdigit = 0;
  val = 0;
  જ્યારે((ch = (unsigned અક્ષર)*src++) != '\0') {
    જો(ISXDIGIT(ch)) {
      val <<= 4;
      val |= curlx_hexval(ch);
      જો(++saw_xdigit > 4)
        પાછા 0;
      ચાલુ;
    }
    જો(ch == ':') {
      curtok = src;
      જો(!saw_xdigit) {
        જો(colonp)
          પાછા 0;
        colonp = tp;
        ચાલુ;
      }
      જો(!*src)
        પાછા 0;
      જો(tp + INT16SZ > endp)
        પાછા 0;
      *tp++ = (unsigned અક્ષર)((val >> 8) & 0xff);
      *tp++ = (unsigned અક્ષર)(val & 0xff);
      saw_xdigit = 0;
      val = 0;
      ચાલુ;
    }
    જો(ch == '.' && ((tp + INADDRSZ) <= endp) &&
       inet_pton4(curtok, tp) > 0) {
      tp += INADDRSZ;
      saw_xdigit = 0;
      તોડો;    /* '\0' was seen by inet_pton4(). */
    }
    પાછા 0;
  }
  જો(saw_xdigit) {
    જો(tp + INT16SZ > endp)
      પાછા 0;
    *tp++ = (unsigned અક્ષર)((val >> 8) & 0xff);
    *tp++ = (unsigned અક્ષર)(val & 0xff);
  }
  જો(colonp) {
    /*
     * Since some memmove()'s erroneously fail to handle
     * overlapping regions, we do the shift by hand.
     */
    અચલ ssize_t n = tp - colonp;
    ssize_t i;

    જો(tp == endp)
      પાછા 0;
    માટે(i = 1; i <= n; i++) {
      *(endp - i) = *(colonp + n - i);
      *(colonp + n - i) = 0;
    }
    tp = endp;
  }
  જો(tp != endp)
    પાછા 0;
  memcpy(dst, tmp, IN6ADDRSZ);
  પાછા 1;
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
પૂર્ણાંક curlx_inet_pton(પૂર્ણાંક af, અચલ અક્ષર *src, શૂન્ય *dst)
{
  પસંદગી(af) {
  સ્થિતિ AF_INET:
    પાછા inet_pton4(src, (unsigned અક્ષર *)dst);
  સ્થિતિ AF_INET6:
    પાછા inet_pton6(src, (unsigned અક્ષર *)dst);
  મૂળભૂત:
    પાછા -1;
  }
  /* NOTREACHED */
}
