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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే HAVE_SYS_PARAM_H
#అన్తర్భావ <sys/param.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు

#అన్తర్భావ "curlx/inet_pton.h"
#అన్తర్భావ "curlx/strparse.h"

#నిర్వచించు IN6ADDRSZ       16
#నిర్వచించు INADDRSZ         4
#నిర్వచించు INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#ఒకవేళ !defined(USE_IPV6) && !defined(AF_INET6)
#నిర్వచించు AF_INET6 (AF_INET + 1)
#అయితే_ముగింపు

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
స్థిర పూర్ణాంకం inet_pton4(స్థిరరాశి అక్షరం *src, గుర్తులేని అక్షరం *dst)
{
  పూర్ణాంకం saw_digit, octets, ch;
  గుర్తులేని అక్షరం tmp[INADDRSZ], *tp;

  saw_digit = 0;
  octets = 0;
  tp = tmp;
  *tp = 0;
  అయితే((ch = (గుర్తులేని అక్షరం)*src++) != '\0') {
    ఒకవేళ(ISDIGIT(ch)) {
      గుర్తులేని పూర్ణాంకం val = (*tp * 10) + (ch - '0');

      ఒకవేళ(saw_digit && *tp == 0)
        తిరిగి 0;
      ఒకవేళ(val > 255)
        తిరిగి 0;
      *tp = (గుర్తులేని అక్షరం)val;
      ఒకవేళ(!saw_digit) {
        ఒకవేళ(++octets > 4)
          తిరిగి 0;
        saw_digit = 1;
      }
    }
    లేకపోతే ఒకవేళ(ch == '.' && saw_digit) {
      ఒకవేళ(octets == 4)
        తిరిగి 0;
      *++tp = 0;
      saw_digit = 0;
    }
    లేకపోతే
      తిరిగి 0;
  }
  ఒకవేళ(octets < 4)
    తిరిగి 0;
  జ్ఞాపకం_నకలు(dst, tmp, INADDRSZ);
  తిరిగి 1;
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
స్థిర పూర్ణాంకం inet_pton6(స్థిరరాశి అక్షరం *src, గుర్తులేని అక్షరం *dst)
{
  గుర్తులేని అక్షరం tmp[IN6ADDRSZ], *tp, *endp, *colonp;
  స్థిరరాశి అక్షరం *curtok;
  పూర్ణాంకం ch, saw_xdigit;
  size_t val;

  memset((tp = tmp), 0, IN6ADDRSZ);
  endp = tp + IN6ADDRSZ;
  colonp = ఖాళీ;
  /* Leading :: requires some special handling. */
  ఒకవేళ(*src == ':' && *++src != ':')
    తిరిగి 0;
  curtok = src;
  saw_xdigit = 0;
  val = 0;
  అయితే((ch = (గుర్తులేని అక్షరం)*src++) != '\0') {
    ఒకవేళ(ISXDIGIT(ch)) {
      val <<= 4;
      val |= curlx_hexval(ch);
      ఒకవేళ(++saw_xdigit > 4)
        తిరిగి 0;
      కొనసాగు;
    }
    ఒకవేళ(ch == ':') {
      curtok = src;
      ఒకవేళ(!saw_xdigit) {
        ఒకవేళ(colonp)
          తిరిగి 0;
        colonp = tp;
        కొనసాగు;
      }
      ఒకవేళ(!*src)
        తిరిగి 0;
      ఒకవేళ(tp + INT16SZ > endp)
        తిరిగి 0;
      *tp++ = (గుర్తులేని అక్షరం)((val >> 8) & 0xff);
      *tp++ = (గుర్తులేని అక్షరం)(val & 0xff);
      saw_xdigit = 0;
      val = 0;
      కొనసాగు;
    }
    ఒకవేళ(ch == '.' && ((tp + INADDRSZ) <= endp) &&
       inet_pton4(curtok, tp) > 0) {
      tp += INADDRSZ;
      saw_xdigit = 0;
      ఆపు;    /* '\0' was seen by inet_pton4(). */
    }
    తిరిగి 0;
  }
  ఒకవేళ(saw_xdigit) {
    ఒకవేళ(tp + INT16SZ > endp)
      తిరిగి 0;
    *tp++ = (గుర్తులేని అక్షరం)((val >> 8) & 0xff);
    *tp++ = (గుర్తులేని అక్షరం)(val & 0xff);
  }
  ఒకవేళ(colonp) {
    /*
     * Since some memmove()'s erroneously fail to handle
     * overlapping regions, we do the shift by hand.
     */
    స్థిరరాశి ssize_t n = tp - colonp;
    ssize_t i;

    ఒకవేళ(tp == endp)
      తిరిగి 0;
    కొరకు(i = 1; i <= n; i++) {
      *(endp - i) = *(colonp + n - i);
      *(colonp + n - i) = 0;
    }
    tp = endp;
  }
  ఒకవేళ(tp != endp)
    తిరిగి 0;
  జ్ఞాపకం_నకలు(dst, tmp, IN6ADDRSZ);
  తిరిగి 1;
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
పూర్ణాంకం curlx_inet_pton(పూర్ణాంకం af, స్థిరరాశి అక్షరం *src, శూన్య *dst)
{
  ఎంపిక(af) {
  స్థితి AF_INET:
    తిరిగి inet_pton4(src, (గుర్తులేని అక్షరం *)dst);
  స్థితి AF_INET6:
    తిరిగి inet_pton6(src, (గుర్తులేని అక్షరం *)dst);
  అప్రమేయ:
    తిరిగి -1;
  }
  /* NOTREACHED */
}
