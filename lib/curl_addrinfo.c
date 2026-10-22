/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * SPDX-License-Identifier: curl
 *
 ***************************************************************************/
#شامل "curl_setup.h"

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETINET_IN6_H
#شامل <netinet/in6.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETDB_H
#شامل <netdb.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_SYS_UN_H
#شامل <sys/un.h>
#اگر_ختم

#اگر_تعریف_شدہ __VMS
#شامل <in.h>
#شامل <inet.h>
#اگر_ختم

#شامل <stddef.h>  /* for offsetof() */

#شامل "curl_addrinfo.h"
#شامل "fake_addrinfo.h"
#شامل "curlx/inet_pton.h"
#شامل "curlx/strparse.h"

/*
 * Curl_freeaddrinfo()
 *
 * This is used to free a linked list of Curl_addrinfo structs along
 * with all its associated allocated storage. This function should be
 * called once for each successful call to Curl_getaddrinfo_ex() or to
 * any function call which actually allocates a Curl_addrinfo struct.
 */

#اگر defined(__INTEL_COMPILER) && (__INTEL_COMPILER == 910) &&     \
  defined(__OPTIMIZE__) && defined(__unix__) && defined(__i386__)
  /* workaround icc 9.1 optimizer issue */
#  define vqualifier متلون
#ورنہ
#  define vqualifier
#اگر_ختم

خالی Curl_freeaddrinfo(ساخت Curl_addrinfo *cahead)
{
  ساخت Curl_addrinfo *vqualifier canext;
  ساخت Curl_addrinfo *ca;

  برائے(ca = cahead; ca; ca = canext) {
    canext = ca->ai_اگلا;
    curlx_free(ca);
  }
}

ساخت Curl_addrinfo *Curl_addrinfo_get(ساخت Curl_addrinfo *ai,
                                        صحیح ai_family,
                                        غیرعلامتی صحیح n)
{
  غیرعلامتی صحیح i;
  برائے(i = 0; ai; ai = ai->ai_اگلا) {
    اگر(ai->ai_family == ai_family) {
      اگر(i == n)
        واپس ai;
      ++i;
    }
  }
  واپس تہی;
}

#اگر_تعریف_شدہ HAVE_GETADDRINFO
/*
 * Curl_getaddrinfo_ex()
 *
 * This is a wrapper function around system's getaddrinfo(), with
 * the only difference that instead of returning a linked list of
 * addrinfo structs this one returns a linked list of Curl_addrinfo
 * ones. The memory allocated by this function *MUST* be free'd with
 * Curl_freeaddrinfo(). For each successful call to this function
 * there must be an associated call later to Curl_freeaddrinfo().
 *
 * There should be no single call to system's getaddrinfo() in the
 * whole library, any such call should be 'routed' through this one.
 */
صحیح Curl_getaddrinfo_ex(ثابت حرف *nodename,
                        ثابت حرف *servname,
                        ثابت ساخت addrinfo *hints,
                        ساخت Curl_addrinfo **نتیجہ)
{
  ثابت ساخت addrinfo *ai;
  ساخت addrinfo *aihead;
  ساخت Curl_addrinfo *cafirst = تہی;
  ساخت Curl_addrinfo *calast = تہی;
  ساخت Curl_addrinfo *ca;
  size_t ss_سائز;
  صحیح error;

  *نتیجہ = تہی; /* assume failure */

  error = CURL_GETADDRINFO(nodename, servname, hints, &aihead);
  اگر(error)
    واپس error;

  /* traverse the addrinfo list */

  برائے(ai = aihead; ai; ai = ai->ai_اگلا) {
    size_t namelen = ai->ai_canonname ? لڑی_لمبائی(ai->ai_canonname) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    اگر(ai->ai_family == AF_INET)
      ss_سائز = حجم(ساخت sockaddr_in);
#اگر_تعریف_شدہ USE_IPV6
    ورنہ اگر(ai->ai_family == AF_INET6)
      ss_سائز = حجم(ساخت sockaddr_in6);
#اگر_ختم
    ورنہ
      جاری;

    /* ignore elements without required address info */
    اگر(!ai->ai_addr || !(ai->ai_addrlen > 0))
      جاری;

    /* ignore elements with bogus address size */
    اگر((size_t)ai->ai_addrlen < ss_سائز)
      جاری;

    ca = curlx_malloc(حجم(ساخت Curl_addrinfo) + ss_سائز + namelen);
    اگر(!ca) {
      error = EAI_MEMORY;
      توڑو;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_سائز;
    ca->ai_addr      = تہی;
    ca->ai_canonname = تہی;
    ca->ai_اگلا      = تہی;

    ca->ai_addr = (خالی *)((حرف *)ca + حجم(ساخت Curl_addrinfo));
    حافظہ_نقل(ca->ai_addr, ai->ai_addr, ss_سائز);

    اگر(namelen) {
      ca->ai_canonname = (خالی *)((حرف *)ca->ai_addr + ss_سائز);
      حافظہ_نقل(ca->ai_canonname, ai->ai_canonname, namelen);
    }

    /* if the return list is empty, this becomes the first element */
    اگر(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    اگر(calast)
      calast->ai_اگلا = ca;
    calast = ca;
  }

  /* destroy the addrinfo list */
  اگر(aihead)
    CURL_FREEADDRINFO(aihead);

  /* if we failed, also destroy the Curl_addrinfo list */
  اگر(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = تہی;
  }
  ورنہ اگر(!cafirst) {
#اگر_تعریف_شدہ EAI_NONAME
    /* rfc3493 conformant */
    error = EAI_NONAME;
#ورنہ
    /* rfc3493 obsoleted */
    error = EAI_NODATA;
#اگر_ختم
#اگر_تعریف_شدہ USE_WINSOCK
    سیٹ_SOCKERRNO(error);
#اگر_ختم
  }

  *نتیجہ = cafirst;

  /* This is not a CURLcode */
  واپس error;
}
#اگر_ختم /* HAVE_GETADDRINFO */

/*
 * Curl_he2ai()
 *
 * This function returns a pointer to the first element of a newly allocated
 * Curl_addrinfo struct linked list filled with the data of a given hostent.
 * Curl_addrinfo is meant to work like the addrinfo struct does for an IPv6
 * stack, but usable also for IPv4, all hosts and environments.
 *
 * The memory allocated by this function *MUST* be free'd later on calling
 * Curl_freeaddrinfo(). For each successful call to this function there
 * must be an associated call later to Curl_freeaddrinfo().
 *
 *   Curl_addrinfo defined in "lib/curl_addrinfo.h"
 *
 *     struct Curl_addrinfo {
 *       int                   ai_flags;
 *       int                   ai_family;
 *       int                   ai_socktype;
 *       int                   ai_protocol;
 *       curl_socklen_t        ai_addrlen;   * Follow rfc3493 struct addrinfo *
 *       char                 *ai_canonname;
 *       struct sockaddr      *ai_addr;
 *       struct Curl_addrinfo *ai_next;
 *     };
 *
 *   hostent defined in <netdb.h>
 *
 *     struct hostent {
 *       char    *h_name;
 *       char    **h_aliases;
 *       int     h_addrtype;
 *       int     h_length;
 *       char    **h_addr_list;
 *     };
 *
 *   for backward compatibility:
 *
 *     #define h_addr  h_addr_list[0]
 */
#اگر !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
ساخت Curl_addrinfo *Curl_he2ai(ثابت ساخت hostent *he, صحیح پورٹ)
{
  ساخت Curl_addrinfo *ai;
  ساخت Curl_addrinfo *prevai = تہی;
  ساخت Curl_addrinfo *firstai = تہی;
  ساخت sockaddr_in *addr;
#اگر_تعریف_شدہ USE_IPV6
  ساخت sockaddr_in6 *addr6;
#اگر_ختم
  CURLcode نتیجہ = CURLE_OK;
  صحیح i;
  حرف *curr;

  اگر(!he)
    /* no input == no output! */
    واپس تہی;

  DEBUGASSERT(he->h_name && he->h_addr_list);

  برائے(i = 0; (curr = he->h_addr_list[i]) != تہی; i++) {
    size_t ss_سائز;
    size_t namelen = لڑی_لمبائی(he->h_name) + 1; /* include null-terminator */
#اگر_تعریف_شدہ USE_IPV6
    اگر(he->h_addrtype == AF_INET6)
      ss_سائز = حجم(ساخت sockaddr_in6);
    ورنہ
#اگر_ختم
      ss_سائز = حجم(ساخت sockaddr_in);

    /* allocate memory to hold the struct, the address and the name */
    ai = curlx_calloc(1, حجم(ساخت Curl_addrinfo) + ss_سائز + namelen);
    اگر(!ai) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      توڑو;
    }
    /* put the address after the struct */
    ai->ai_addr = (خالی *)((حرف *)ai + حجم(ساخت Curl_addrinfo));
    /* then put the name after the address */
    ai->ai_canonname = (حرف *)ai->ai_addr + ss_سائز;
    حافظہ_نقل(ai->ai_canonname, he->h_name, namelen);

    اگر(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    اگر(prevai)
      /* make the previous entry point to this */
      prevai->ai_اگلا = ai;

    ai->ai_family = he->h_addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_سائز;

    /* leave the rest of the struct filled with zero */

    انتخاب(ai->ai_family) {
    حالت AF_INET:
      addr = (خالی *)ai->ai_addr; /* storage area for this info */

      حافظہ_نقل(&addr->sin_addr, curr, حجم(ساخت in_addr));
      addr->sin_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr->sin_port = htons((غیرعلامتی مختصر)پورٹ);
      توڑو;

#اگر_تعریف_شدہ USE_IPV6
    حالت AF_INET6:
      addr6 = (خالی *)ai->ai_addr; /* storage area for this info */

      حافظہ_نقل(&addr6->sin6_addr, curr, حجم(ساخت in6_addr));
      addr6->sin6_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr6->sin6_port = htons((غیرعلامتی مختصر)پورٹ);
      توڑو;
#اگر_ختم
    }

    prevai = ai;
  }

  اگر(نتیجہ) {
    Curl_freeaddrinfo(firstai);
    firstai = تہی;
  }

  واپس firstai;
}
#اگر_ختم

/*
 * ip2addr()
 *
 * This function takes an Internet address, in binary form, as input parameter
 * along with its address family and the string version of the address, and it
 * returns a Curl_addrinfo chain filled in correctly with information for the
 * given address/host
 */
ساکن CURLcode ip2addr(ساخت Curl_addrinfo **addrp, صحیح af,
                        ثابت خالی *inaddr, ثابت حرف *hostname, صحیح پورٹ)
{
  ساخت Curl_addrinfo *ai;
  size_t addrsize;
  size_t namelen;
  ساخت sockaddr_in *addr;
#اگر_تعریف_شدہ USE_IPV6
  ساخت sockaddr_in6 *addr6;
#اگر_ختم

  DEBUGASSERT(inaddr && hostname);

  namelen = لڑی_لمبائی(hostname) + 1;
  *addrp = تہی;

  اگر(af == AF_INET)
    addrsize = حجم(ساخت sockaddr_in);
#اگر_تعریف_شدہ USE_IPV6
  ورنہ اگر(af == AF_INET6)
    addrsize = حجم(ساخت sockaddr_in6);
#اگر_ختم
  ورنہ
    واپس CURLE_BAD_FUNCTION_دلیل;

  /* allocate memory to hold the struct, the address and the name */
  ai = curlx_calloc(1, حجم(ساخت Curl_addrinfo) + addrsize + namelen);
  اگر(!ai)
    واپس CURLE_برآمد_کا_حافظہ;
  /* put the address after the struct */
  ai->ai_addr = (خالی *)((حرف *)ai + حجم(ساخت Curl_addrinfo));
  /* then put the name after the address */
  ai->ai_canonname = (حرف *)ai->ai_addr + addrsize;
  حافظہ_نقل(ai->ai_canonname, hostname, namelen);
  ai->ai_family = af;
  ai->ai_socktype = SOCK_STREAM;
  ai->ai_addrlen = (curl_socklen_t)addrsize;
  /* leave the rest of the struct filled with zero */

  انتخاب(af) {
  حالت AF_INET:
    addr = (خالی *)ai->ai_addr; /* storage area for this info */

    حافظہ_نقل(&addr->sin_addr, inaddr, حجم(ساخت in_addr));
    addr->sin_family = (CURL_SA_FAMILY_T)af;
    addr->sin_port = htons((غیرعلامتی مختصر)پورٹ);
    توڑو;

#اگر_تعریف_شدہ USE_IPV6
  حالت AF_INET6:
    addr6 = (خالی *)ai->ai_addr; /* storage area for this info */

    حافظہ_نقل(&addr6->sin6_addr, inaddr, حجم(ساخت in6_addr));
    addr6->sin6_family = (CURL_SA_FAMILY_T)af;
    addr6->sin6_port = htons((غیرعلامتی مختصر)پورٹ);
    توڑو;
#اگر_ختم
  }
  *addrp = ai;
  واپس CURLE_OK;
}

/*
 * Given an IPv4 or IPv6 dotted string address, this converts it to a proper
 * allocated Curl_addrinfo struct and returns it.
 */
CURLcode Curl_str2addr(ثابت حرف *dotted, uint16_t پورٹ,
                       ساخت Curl_addrinfo **addrp)
{
  ساخت in_addr اندر;
  اگر(curlx_inet_pton(AF_INET, dotted, &اندر) > 0)
    /* This is a dotted IP address 123.123.123.123-style */
    واپس ip2addr(addrp, AF_INET, &اندر, dotted, پورٹ);
#اگر_تعریف_شدہ USE_IPV6
  {
    ساخت in6_addr in6;
    اگر(curlx_inet_pton(AF_INET6, dotted, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      واپس ip2addr(addrp, AF_INET6, &in6, dotted, پورٹ);
  }
#اگر_ختم
  واپس CURLE_BAD_FUNCTION_دلیل; /* bad input format */
}

bool Curl_is_ipv4addr(ثابت حرف *پتہ)
{
  ساخت in_addr اندر;
  واپس (curlx_inet_pton(AF_INET, پتہ, &اندر) > 0);
}

bool Curl_is_ipaddr(ثابت حرف *پتہ)
{
  اگر(Curl_is_ipv4addr(پتہ))
    واپس TRUE;
#اگر_تعریف_شدہ USE_IPV6
  {
    ساخت in6_addr in6;
    اگر(curlx_inet_pton(AF_INET6, پتہ, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      واپس TRUE;
  }
#اگر_ختم
  واپس FALSE;
}

bool Curl_looks_like_ipv6(ثابت حرف *s, size_t len, bool maybe_url_encoded,
                          ساخت Curl_str *host, ساخت Curl_str *zone)
{
  ثابت حرف *zonep = تہی;
  size_t i = 0, hlen = 0, zlen = 0;

  اگر(host)
    memset(host, 0, حجم(*host));
  اگر(zone)
    memset(zone, 0, حجم(*zone));

  برائے(i = 0; i < len; ++i, ++hlen) {
    اگر(!s[i] || !(ISXDIGIT(s[i]) || (s[i] == ':') || (s[i] == '.')))
      توڑو;
  }

  اگر((i < len) && (s[i] == '%')) { /* address followed by a zone? */
    i += 1;
    اگر(maybe_url_encoded && !لڑی_موازنہ("25", s + i, 2))
      i += 2;
    zonep = s + i;
    برائے(; i < len; ++i, ++zlen) {
      /* Allow unreserved characters as defined in RFC 3986 */
      اگر(!s[i] || !(ISALPHA(s[i]) || ISXDIGIT(s[i]) || (s[i] == '-') ||
                    (s[i] == '.') || (s[i] == '_') || (s[i] == '~')))
        توڑو;
    }
  }

  اگر(i != len)
    واپس FALSE; /* invalid chars in zone */
  اگر(host && hlen) {
    host->str = s;
    host->len = hlen;
  }
  اگر(zone && zlen) {
    zone->str = zonep;
    zone->len = zlen;
  }
  واپس TRUE;
}

#اگر_تعریف_شدہ USE_UNIX_SOCKETS
/**
 * Given a path to a Unix domain socket, return a newly allocated Curl_addrinfo
 * struct initialized with this path.
 * Returns CURLE_TOO_LARGE when path is too long.
 */
CURLcode Curl_unix2addr(ثابت حرف *راستہ, bool abstract,
                        ساخت Curl_addrinfo **paddr)
{
  ساخت Curl_addrinfo *ai;
  ساخت sockaddr_un *sa_un;
  size_t راستہ_len;

  *paddr = تہی;

  /* sun_path must be able to store the null-terminated path */
  راستہ_len = لڑی_لمبائی(راستہ) + 1;
  اگر(راستہ_len > حجم(sa_un->sun_راستہ))
    واپس CURLE_TOO_LARGE;

  ai = curlx_calloc(1,
                    حجم(ساخت Curl_addrinfo) + حجم(ساخت sockaddr_un));
  اگر(!ai)
    واپس CURLE_برآمد_کا_حافظہ;

  ai->ai_addr = (خالی *)((حرف *)ai + حجم(ساخت Curl_addrinfo));
  sa_un = (خالی *)ai->ai_addr;
  sa_un->sun_family = AF_UNIX;

  ai->ai_family = AF_UNIX;
  ai->ai_socktype = SOCK_STREAM; /* assume reliable transport for HTTP */
  ai->ai_addrlen = (curl_socklen_t)
    ((offsetof(ساخت sockaddr_un, sun_راستہ) + راستہ_len) & 0x7FFFFFFF);

  /* Abstract Unix domain socket have NULL prefix instead of suffix */
  اگر(abstract)
    حافظہ_نقل(sa_un->sun_راستہ + 1, راستہ, راستہ_len - 1);
  ورنہ
    حافظہ_نقل(sa_un->sun_راستہ, راستہ, راستہ_len); /* copy NUL byte */

  *paddr = ai;
  واپس CURLE_OK;
}
#اگر_ختم

#اگر defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
/*
 * curl_dbg_freeaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
خالی curl_dbg_freeaddrinfo(ساخت addrinfo *freethis,
                           صحیح سطر, ثابت حرف *ماخذ)
{
  curl_dbg_log("ADDR %s:%d freeaddrinfo(%p)\n",
               ماخذ, سطر, (خالی *)freethis);
#اگر_تعریف_شدہ USE_LWIPSOCK
  lwip_freeaddrinfo(freethis);
#وگرنہ_اگر defined(USE_FAKE_GETADDRINFO)
  {
    ثابت حرف *env = getenv("CURL_DNS_SERVER");
    اگر(env)
      r_freeaddrinfo(freethis);
    ورنہ
      /* !checksrc! disable BANNEDFUNC 1 */
      freeaddrinfo(freethis);
  }
#ورنہ
  /* !checksrc! disable BANNEDFUNC 1 */
  freeaddrinfo(freethis);
#اگر_ختم
}
#اگر_ختم /* CURL_MEMDEBUG && HAVE_FREEADDRINFO */

#اگر defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
/*
 * curl_dbg_getaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
صحیح curl_dbg_getaddrinfo(ثابت حرف *hostname,
                         ثابت حرف *service,
                         ثابت ساخت addrinfo *hints,
                         ساخت addrinfo **نتیجہ,
                         صحیح سطر, ثابت حرف *ماخذ)
{
#اگر_تعریف_شدہ USE_LWIPSOCK
  صحیح res = lwip_getaddrinfo(hostname, service, hints, نتیجہ);
#وگرنہ_اگر defined(USE_FAKE_GETADDRINFO)
  صحیح res;
  ثابت حرف *env = getenv("CURL_DNS_SERVER");
  اگر(env)
    res = r_getaddrinfo(hostname, service, hints, نتیجہ);
  ورنہ
    /* !checksrc! disable BANNEDFUNC 1 */
    res = getaddrinfo(hostname, service, hints, نتیجہ);
#ورنہ
  /* !checksrc! disable BANNEDFUNC 1 */
  صحیح res = getaddrinfo(hostname, service, hints, نتیجہ);
#اگر_ختم
  اگر(res == 0)
    /* success */
    curl_dbg_log("ADDR %s:%d getaddrinfo() = %p\n", ماخذ, سطر,
                 (خالی *)*نتیجہ);
  ورنہ
    curl_dbg_log("ADDR %s:%d getaddrinfo() failed\n", ماخذ, سطر);
  واپس res;
}
#اگر_ختم /* CURL_MEMDEBUG && HAVE_GETADDRINFO */

#اگر defined(HAVE_GETADDRINFO) && defined(USE_RESOLVE_ON_IPS)
/*
 * Works around the sin6_port is always zero bug on iOS 9.3.2 and macOS
 * 10.11.5.
 */
خالی Curl_addrinfo_سیٹ_پورٹ(ساخت Curl_addrinfo *addrinfo, صحیح پورٹ)
{
  ساخت Curl_addrinfo *ca;
  ساخت sockaddr_in *addr;
#اگر_تعریف_شدہ USE_IPV6
  ساخت sockaddr_in6 *addr6;
#اگر_ختم
  برائے(ca = addrinfo; ca; ca = ca->ai_اگلا) {
    انتخاب(ca->ai_family) {
    حالت AF_INET:
      addr = (خالی *)ca->ai_addr; /* storage area for this info */
      addr->sin_port = htons((غیرعلامتی مختصر)پورٹ);
      توڑو;

#اگر_تعریف_شدہ USE_IPV6
    حالت AF_INET6:
      addr6 = (خالی *)ca->ai_addr; /* storage area for this info */
      addr6->sin6_port = htons((غیرعلامتی مختصر)پورٹ);
      توڑو;
#اگر_ختم
    }
  }
}
#اگر_ختم
