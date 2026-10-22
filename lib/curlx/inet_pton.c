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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef HAVE_SYS_PARAM_H
#ଅନ୍ତର୍ଭାଵ <sys/param.h>
#endif
#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif

#ଅନ୍ତର୍ଭାଵ "curlx/inet_pton.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"

#define IN6ADDRSZ       16
#define INADDRSZ         4
#define INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#ଯଦି !defined(USE_IPV6) && !defined(AF_INET6)
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା inet_pton4(ଅଚଳ ଅକ୍ଷର *src, unsigned ଅକ୍ଷର *dst)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା saw_digit, octets, ch;
  unsigned ଅକ୍ଷର tmp[INADDRSZ], *tp;

  saw_digit = 0;
  octets = 0;
  tp = tmp;
  *tp = 0;
  ଯେତେବେଳେ((ch = (unsigned ଅକ୍ଷର)*src++) != '\0') {
    ଯଦି(ISDIGIT(ch)) {
      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା val = (*tp * 10) + (ch - '0');

      ଯଦି(saw_digit && *tp == 0)
        ଫେରନ୍ତୁ 0;
      ଯଦି(val > 255)
        ଫେରନ୍ତୁ 0;
      *tp = (unsigned ଅକ୍ଷର)val;
      ଯଦି(!saw_digit) {
        ଯଦି(++octets > 4)
          ଫେରନ୍ତୁ 0;
        saw_digit = 1;
      }
    }
    ଅନ୍ୟଥା ଯଦି(ch == '.' && saw_digit) {
      ଯଦି(octets == 4)
        ଫେରନ୍ତୁ 0;
      *++tp = 0;
      saw_digit = 0;
    }
    ଅନ୍ୟଥା
      ଫେରନ୍ତୁ 0;
  }
  ଯଦି(octets < 4)
    ଫେରନ୍ତୁ 0;
  memcpy(dst, tmp, INADDRSZ);
  ଫେରନ୍ତୁ 1;
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା inet_pton6(ଅଚଳ ଅକ୍ଷର *src, unsigned ଅକ୍ଷର *dst)
{
  unsigned ଅକ୍ଷର tmp[IN6ADDRSZ], *tp, *endp, *colonp;
  ଅଚଳ ଅକ୍ଷର *curtok;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ch, saw_xdigit;
  size_t val;

  memset((tp = tmp), 0, IN6ADDRSZ);
  endp = tp + IN6ADDRSZ;
  colonp = ଖାଲି;
  /* Leading :: requires some special handling. */
  ଯଦି(*src == ':' && *++src != ':')
    ଫେରନ୍ତୁ 0;
  curtok = src;
  saw_xdigit = 0;
  val = 0;
  ଯେତେବେଳେ((ch = (unsigned ଅକ୍ଷର)*src++) != '\0') {
    ଯଦି(ISXDIGIT(ch)) {
      val <<= 4;
      val |= curlx_hexval(ch);
      ଯଦି(++saw_xdigit > 4)
        ଫେରନ୍ତୁ 0;
      ଜାରି;
    }
    ଯଦି(ch == ':') {
      curtok = src;
      ଯଦି(!saw_xdigit) {
        ଯଦି(colonp)
          ଫେରନ୍ତୁ 0;
        colonp = tp;
        ଜାରି;
      }
      ଯଦି(!*src)
        ଫେରନ୍ତୁ 0;
      ଯଦି(tp + INT16SZ > endp)
        ଫେରନ୍ତୁ 0;
      *tp++ = (unsigned ଅକ୍ଷର)((val >> 8) & 0xff);
      *tp++ = (unsigned ଅକ୍ଷର)(val & 0xff);
      saw_xdigit = 0;
      val = 0;
      ଜାରି;
    }
    ଯଦି(ch == '.' && ((tp + INADDRSZ) <= endp) &&
       inet_pton4(curtok, tp) > 0) {
      tp += INADDRSZ;
      saw_xdigit = 0;
      ଭାଙ୍ଗ;    /* '\0' was seen by inet_pton4(). */
    }
    ଫେରନ୍ତୁ 0;
  }
  ଯଦି(saw_xdigit) {
    ଯଦି(tp + INT16SZ > endp)
      ଫେରନ୍ତୁ 0;
    *tp++ = (unsigned ଅକ୍ଷର)((val >> 8) & 0xff);
    *tp++ = (unsigned ଅକ୍ଷର)(val & 0xff);
  }
  ଯଦି(colonp) {
    /*
     * Since some memmove()'s erroneously fail to handle
     * overlapping regions, we do the shift by hand.
     */
    ଅଚଳ ssize_t n = tp - colonp;
    ssize_t i;

    ଯଦି(tp == endp)
      ଫେରନ୍ତୁ 0;
    ପାଇଁ(i = 1; i <= n; i++) {
      *(endp - i) = *(colonp + n - i);
      *(colonp + n - i) = 0;
    }
    tp = endp;
  }
  ଯଦି(tp != endp)
    ଫେରନ୍ତୁ 0;
  memcpy(dst, tmp, IN6ADDRSZ);
  ଫେରନ୍ତୁ 1;
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
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_inet_pton(ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, ଅଚଳ ଅକ୍ଷର *src, ଶୂନ୍ୟ *dst)
{
  ବାଛ(af) {
  ସ୍ଥିତି AF_INET:
    ଫେରନ୍ତୁ inet_pton4(src, (unsigned ଅକ୍ଷର *)dst);
  ସ୍ଥିତି AF_INET6:
    ଫେରନ୍ତୁ inet_pton6(src, (unsigned ଅକ୍ଷର *)dst);
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ -1;
  }
  /* NOTREACHED */
}
