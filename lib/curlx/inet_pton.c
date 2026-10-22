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
#include "curl_setup.h"

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#include "curlx/inet_pton.h"
#include "curlx/strparse.h"

#define IN6ADDRSZ       16
#define INADDRSZ         4
#define INT16SZ          2

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#如果 !defined(USE_IPV6) && !defined(AF_INET6)
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
static 整数 inet_pton4(const char *src, unsigned char *dst)
{
  整数 saw_digit, octets, ch;
  unsigned char tmp[INADDRSZ], *tp;

  saw_digit = 0;
  octets = 0;
  tp = tmp;
  *tp = 0;
  当((ch = (unsigned char)*src++) != '\0') {
    如果(ISDIGIT(ch)) {
      unsigned 整数 val = (*tp * 10) + (ch - '0');

      如果(saw_digit && *tp == 0)
        返回 0;
      如果(val > 255)
        返回 0;
      *tp = (unsigned char)val;
      如果(!saw_digit) {
        如果(++octets > 4)
          返回 0;
        saw_digit = 1;
      }
    }
    否则 如果(ch == '.' && saw_digit) {
      如果(octets == 4)
        返回 0;
      *++tp = 0;
      saw_digit = 0;
    }
    否则
      返回 0;
  }
  如果(octets < 4)
    返回 0;
  memcpy(dst, tmp, INADDRSZ);
  返回 1;
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
static 整数 inet_pton6(const char *src, unsigned char *dst)
{
  unsigned char tmp[IN6ADDRSZ], *tp, *endp, *colonp;
  const char *curtok;
  整数 ch, saw_xdigit;
  size_t val;

  memset((tp = tmp), 0, IN6ADDRSZ);
  endp = tp + IN6ADDRSZ;
  colonp = NULL;
  /* Leading :: requires some special handling. */
  如果(*src == ':' && *++src != ':')
    返回 0;
  curtok = src;
  saw_xdigit = 0;
  val = 0;
  当((ch = (unsigned char)*src++) != '\0') {
    如果(ISXDIGIT(ch)) {
      val <<= 4;
      val |= curlx_hexval(ch);
      如果(++saw_xdigit > 4)
        返回 0;
      continue;
    }
    如果(ch == ':') {
      curtok = src;
      如果(!saw_xdigit) {
        如果(colonp)
          返回 0;
        colonp = tp;
        continue;
      }
      如果(!*src)
        返回 0;
      如果(tp + INT16SZ > endp)
        返回 0;
      *tp++ = (unsigned char)((val >> 8) & 0xff);
      *tp++ = (unsigned char)(val & 0xff);
      saw_xdigit = 0;
      val = 0;
      continue;
    }
    如果(ch == '.' && ((tp + INADDRSZ) <= endp) &&
       inet_pton4(curtok, tp) > 0) {
      tp += INADDRSZ;
      saw_xdigit = 0;
      break;    /* '\0' was seen by inet_pton4(). */
    }
    返回 0;
  }
  如果(saw_xdigit) {
    如果(tp + INT16SZ > endp)
      返回 0;
    *tp++ = (unsigned char)((val >> 8) & 0xff);
    *tp++ = (unsigned char)(val & 0xff);
  }
  如果(colonp) {
    /*
     * Since some memmove()'s erroneously fail to handle
     * overlapping regions, we do the shift by hand.
     */
    const ssize_t n = tp - colonp;
    ssize_t i;

    如果(tp == endp)
      返回 0;
    为(i = 1; i <= n; i++) {
      *(endp - i) = *(colonp + n - i);
      *(colonp + n - i) = 0;
    }
    tp = endp;
  }
  如果(tp != endp)
    返回 0;
  memcpy(dst, tmp, IN6ADDRSZ);
  返回 1;
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
整数 curlx_inet_pton(整数 af, const char *src, void *dst)
{
  switch(af) {
  case AF_INET:
    返回 inet_pton4(src, (unsigned char *)dst);
  case AF_INET6:
    返回 inet_pton6(src, (unsigned char *)dst);
  default:
    返回 -1;
  }
  /* NOTREACHED */
}
