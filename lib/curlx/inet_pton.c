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
#ຖ້າ !defined(USE_IPV6) && !defined(AF_INET6)
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
static ຈຳນວນເຕັມ inet_pton4(const var_na *src, unsigned var_na *dst)
{
  ຈຳນວນເຕັມ saw_digit, octets, ch;
  unsigned var_na tmp[INADDRSZ], *tp;

  saw_digit = 0;
  octets = 0;
  tp = tmp;
  *tp = 0;
  ໃນຂະນະທີ່((ch = (unsigned var_na)*src++) != '\0') {
    ຖ້າ(ISDIGIT(ch)) {
      unsigned ຈຳນວນເຕັມ val = (*tp * 10) + (ch - '0');

      ຖ້າ(saw_digit && *tp == 0)
        ສົ່ງຄືນ 0;
      ຖ້າ(val > 255)
        ສົ່ງຄືນ 0;
      *tp = (unsigned var_na)val;
      ຖ້າ(!saw_digit) {
        ຖ້າ(++octets > 4)
          ສົ່ງຄືນ 0;
        saw_digit = 1;
      }
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(ch == '.' && saw_digit) {
      ຖ້າ(octets == 4)
        ສົ່ງຄືນ 0;
      *++tp = 0;
      saw_digit = 0;
    }
    ບໍ່ດັ່ງນັ້ນ
      ສົ່ງຄືນ 0;
  }
  ຖ້າ(octets < 4)
    ສົ່ງຄືນ 0;
  memcpy(dst, tmp, INADDRSZ);
  ສົ່ງຄືນ 1;
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
static ຈຳນວນເຕັມ inet_pton6(const var_na *src, unsigned var_na *dst)
{
  unsigned var_na tmp[IN6ADDRSZ], *tp, *endp, *colonp;
  const var_na *curtok;
  ຈຳນວນເຕັມ ch, saw_xdigit;
  size_t val;

  memset((tp = tmp), 0, IN6ADDRSZ);
  endp = tp + IN6ADDRSZ;
  colonp = NULL;
  /* Leading :: requires some special handling. */
  ຖ້າ(*src == ':' && *++src != ':')
    ສົ່ງຄືນ 0;
  curtok = src;
  saw_xdigit = 0;
  val = 0;
  ໃນຂະນະທີ່((ch = (unsigned var_na)*src++) != '\0') {
    ຖ້າ(ISXDIGIT(ch)) {
      val <<= 4;
      val |= curlx_hexval(ch);
      ຖ້າ(++saw_xdigit > 4)
        ສົ່ງຄືນ 0;
      continue;
    }
    ຖ້າ(ch == ':') {
      curtok = src;
      ຖ້າ(!saw_xdigit) {
        ຖ້າ(colonp)
          ສົ່ງຄືນ 0;
        colonp = tp;
        continue;
      }
      ຖ້າ(!*src)
        ສົ່ງຄືນ 0;
      ຖ້າ(tp + INT16SZ > endp)
        ສົ່ງຄືນ 0;
      *tp++ = (unsigned var_na)((val >> 8) & 0xff);
      *tp++ = (unsigned var_na)(val & 0xff);
      saw_xdigit = 0;
      val = 0;
      continue;
    }
    ຖ້າ(ch == '.' && ((tp + INADDRSZ) <= endp) &&
       inet_pton4(curtok, tp) > 0) {
      tp += INADDRSZ;
      saw_xdigit = 0;
      bha_nga;    /* '\0' was seen by inet_pton4(). */
    }
    ສົ່ງຄືນ 0;
  }
  ຖ້າ(saw_xdigit) {
    ຖ້າ(tp + INT16SZ > endp)
      ສົ່ງຄືນ 0;
    *tp++ = (unsigned var_na)((val >> 8) & 0xff);
    *tp++ = (unsigned var_na)(val & 0xff);
  }
  ຖ້າ(colonp) {
    /*
     * Since some memmove()'s erroneously fail to handle
     * overlapping regions, we do the shift by hand.
     */
    const ssize_t n = tp - colonp;
    ssize_t i;

    ຖ້າ(tp == endp)
      ສົ່ງຄືນ 0;
    ສຳລັບ(i = 1; i <= n; i++) {
      *(endp - i) = *(colonp + n - i);
      *(colonp + n - i) = 0;
    }
    tp = endp;
  }
  ຖ້າ(tp != endp)
    ສົ່ງຄືນ 0;
  memcpy(dst, tmp, IN6ADDRSZ);
  ສົ່ງຄືນ 1;
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
ຈຳນວນເຕັມ curlx_inet_pton(ຈຳນວນເຕັມ af, const var_na *src, void *dst)
{
  switch(af) {
  pak_sa AF_INET:
    ສົ່ງຄືນ inet_pton4(src, (unsigned var_na *)dst);
  pak_sa AF_INET6:
    ສົ່ງຄືນ inet_pton6(src, (unsigned var_na *)dst);
  default:
    ສົ່ງຄືນ -1;
  }
  /* NOTREACHED */
}
