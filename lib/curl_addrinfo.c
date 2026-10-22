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
#ضمن "curl_setup.h"

#اگر_تعریف_شده HAVE_NETINET_درون_H
#ضمن <netinet/in.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETINET_IN6_H
#ضمن <netinet/in6.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETDB_H
#ضمن <netdb.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_SYS_UN_H
#ضمن <sys/un.h>
#پایان_اگر

#اگر_تعریف_شده __VMS
#ضمن <in.h>
#ضمن <inet.h>
#پایان_اگر

#ضمن <stddef.h>  /* for offsetof() */

#ضمن "curl_addrinfo.h"
#ضمن "fake_addrinfo.h"
#ضمن "curlx/inet_pton.h"
#ضمن "curlx/strparse.h"

/*
 * Curl_freeaddrinfo()
 *
 * This is used to free a linked list of Curl_addrinfo structs along
 * with all its associated allocated storage. This function should be
 * called once for each successful call to Curl_getaddrinfo_ex() or to
 * any function call which actually allocates a Curl_addrinfo struct.
 */

#إذا defined(__INTEL_COMPILER) && (__INTEL_COMPILER == 910) &&     \
  defined(__OPTIMIZE__) && defined(__unix__) && defined(__i386__)
  /* workaround icc 9.1 optimizer issue */
#  define vqualifier متطاير
#وإلا
#  define vqualifier
#پایان_اگر

فراغ Curl_freeaddrinfo(بنية Curl_addrinfo *cahead)
{
  بنية Curl_addrinfo *vqualifier canext;
  بنية Curl_addrinfo *ca;

  لكل(ca = cahead; ca; ca = canext) {
    canext = ca->ai_بعدی;
    curlx_free(ca);
  }
}

بنية Curl_addrinfo *Curl_addrinfo_get(بنية Curl_addrinfo *ai,
                                        صحيح ai_family,
                                        بیعلامت صحيح n)
{
  بیعلامت صحيح i;
  لكل(i = 0; ai; ai = ai->ai_بعدی) {
    إذا(ai->ai_family == ai_family) {
      إذا(i == n)
        أرجع ai;
      ++i;
    }
  }
  أرجع عدم;
}

#اگر_تعریف_شده HAVE_GETADDRINFO
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
صحيح Curl_getaddrinfo_ex(ثابت حرف *nodename,
                        ثابت حرف *servname,
                        ثابت بنية addrinfo *hints,
                        بنية Curl_addrinfo **نتیجه)
{
  ثابت بنية addrinfo *ai;
  بنية addrinfo *aihead;
  بنية Curl_addrinfo *cafirst = عدم;
  بنية Curl_addrinfo *calast = عدم;
  بنية Curl_addrinfo *ca;
  size_t ss_اندازه;
  صحيح error;

  *نتیجه = عدم; /* assume failure */

  error = CURL_GETADDRINFO(nodename, servname, hints, &aihead);
  إذا(error)
    أرجع error;

  /* traverse the addrinfo list */

  لكل(ai = aihead; ai; ai = ai->ai_بعدی) {
    size_t namelen = ai->ai_canonname ? طول_رشته(ai->ai_canonname) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    إذا(ai->ai_family == AF_INET)
      ss_اندازه = حجم(بنية sockaddr_in);
#اگر_تعریف_شده USE_IPV6
    وإلا إذا(ai->ai_family == AF_INET6)
      ss_اندازه = حجم(بنية sockaddr_in6);
#پایان_اگر
    وإلا
      استمر;

    /* ignore elements without required address info */
    إذا(!ai->ai_addr || !(ai->ai_addrlen > 0))
      استمر;

    /* ignore elements with bogus address size */
    إذا((size_t)ai->ai_addrlen < ss_اندازه)
      استمر;

    ca = curlx_malloc(حجم(بنية Curl_addrinfo) + ss_اندازه + namelen);
    إذا(!ca) {
      error = EAI_MEMORY;
      اكسر;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_اندازه;
    ca->ai_addr      = عدم;
    ca->ai_canonname = عدم;
    ca->ai_بعدی      = عدم;

    ca->ai_addr = (فراغ *)((حرف *)ca + حجم(بنية Curl_addrinfo));
    کپی_حافظه(ca->ai_addr, ai->ai_addr, ss_اندازه);

    إذا(namelen) {
      ca->ai_canonname = (فراغ *)((حرف *)ca->ai_addr + ss_اندازه);
      کپی_حافظه(ca->ai_canonname, ai->ai_canonname, namelen);
    }

    /* if the return list is empty, this becomes the first element */
    إذا(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    إذا(calast)
      calast->ai_بعدی = ca;
    calast = ca;
  }

  /* destroy the addrinfo list */
  إذا(aihead)
    CURL_FREEADDRINFO(aihead);

  /* if we failed, also destroy the Curl_addrinfo list */
  إذا(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = عدم;
  }
  وإلا إذا(!cafirst) {
#اگر_تعریف_شده EAI_NONAME
    /* rfc3493 conformant */
    error = EAI_NONAME;
#وإلا
    /* rfc3493 obsoleted */
    error = EAI_NODATA;
#پایان_اگر
#اگر_تعریف_شده USE_WINSOCK
    مجموعه_SOCKERRNO(error);
#پایان_اگر
  }

  *نتیجه = cafirst;

  /* This is not a CURLcode */
  أرجع error;
}
#پایان_اگر /* HAVE_GETADDRINFO */

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
#إذا !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
بنية Curl_addrinfo *Curl_he2ai(ثابت بنية hostent *he, صحيح پورت)
{
  بنية Curl_addrinfo *ai;
  بنية Curl_addrinfo *prevai = عدم;
  بنية Curl_addrinfo *firstai = عدم;
  بنية sockaddr_in *addr;
#اگر_تعریف_شده USE_IPV6
  بنية sockaddr_in6 *addr6;
#پایان_اگر
  CURLcode نتیجه = CURLE_OK;
  صحيح i;
  حرف *curr;

  إذا(!he)
    /* no input == no output! */
    أرجع عدم;

  DEBUGASSERT(he->h_name && he->h_addr_list);

  لكل(i = 0; (curr = he->h_addr_list[i]) != عدم; i++) {
    size_t ss_اندازه;
    size_t namelen = طول_رشته(he->h_name) + 1; /* include null-terminator */
#اگر_تعریف_شده USE_IPV6
    إذا(he->h_addrtype == AF_INET6)
      ss_اندازه = حجم(بنية sockaddr_in6);
    وإلا
#پایان_اگر
      ss_اندازه = حجم(بنية sockaddr_in);

    /* allocate memory to hold the struct, the address and the name */
    ai = curlx_calloc(1, حجم(بنية Curl_addrinfo) + ss_اندازه + namelen);
    إذا(!ai) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اكسر;
    }
    /* put the address after the struct */
    ai->ai_addr = (فراغ *)((حرف *)ai + حجم(بنية Curl_addrinfo));
    /* then put the name after the address */
    ai->ai_canonname = (حرف *)ai->ai_addr + ss_اندازه;
    کپی_حافظه(ai->ai_canonname, he->h_name, namelen);

    إذا(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    إذا(prevai)
      /* make the previous entry point to this */
      prevai->ai_بعدی = ai;

    ai->ai_family = he->h_addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_اندازه;

    /* leave the rest of the struct filled with zero */

    بدل(ai->ai_family) {
    حالة AF_INET:
      addr = (فراغ *)ai->ai_addr; /* storage area for this info */

      کپی_حافظه(&addr->sin_addr, curr, حجم(بنية in_addr));
      addr->sin_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr->sin_port = htons((بیعلامت کوتاه)پورت);
      اكسر;

#اگر_تعریف_شده USE_IPV6
    حالة AF_INET6:
      addr6 = (فراغ *)ai->ai_addr; /* storage area for this info */

      کپی_حافظه(&addr6->sin6_addr, curr, حجم(بنية in6_addr));
      addr6->sin6_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr6->sin6_port = htons((بیعلامت کوتاه)پورت);
      اكسر;
#پایان_اگر
    }

    prevai = ai;
  }

  إذا(نتیجه) {
    Curl_freeaddrinfo(firstai);
    firstai = عدم;
  }

  أرجع firstai;
}
#پایان_اگر

/*
 * ip2addr()
 *
 * This function takes an Internet address, in binary form, as input parameter
 * along with its address family and the string version of the address, and it
 * returns a Curl_addrinfo chain filled in correctly with information for the
 * given address/host
 */
ساكن CURLcode ip2addr(بنية Curl_addrinfo **addrp, صحيح af,
                        ثابت فراغ *inaddr, ثابت حرف *hostname, صحيح پورت)
{
  بنية Curl_addrinfo *ai;
  size_t addrsize;
  size_t namelen;
  بنية sockaddr_in *addr;
#اگر_تعریف_شده USE_IPV6
  بنية sockaddr_in6 *addr6;
#پایان_اگر

  DEBUGASSERT(inaddr && hostname);

  namelen = طول_رشته(hostname) + 1;
  *addrp = عدم;

  إذا(af == AF_INET)
    addrsize = حجم(بنية sockaddr_in);
#اگر_تعریف_شده USE_IPV6
  وإلا إذا(af == AF_INET6)
    addrsize = حجم(بنية sockaddr_in6);
#پایان_اگر
  وإلا
    أرجع CURLE_BAD_FUNCTION_آرگومان;

  /* allocate memory to hold the struct, the address and the name */
  ai = curlx_calloc(1, حجم(بنية Curl_addrinfo) + addrsize + namelen);
  إذا(!ai)
    أرجع CURLE_بیرون_از_حافظه;
  /* put the address after the struct */
  ai->ai_addr = (فراغ *)((حرف *)ai + حجم(بنية Curl_addrinfo));
  /* then put the name after the address */
  ai->ai_canonname = (حرف *)ai->ai_addr + addrsize;
  کپی_حافظه(ai->ai_canonname, hostname, namelen);
  ai->ai_family = af;
  ai->ai_socktype = SOCK_STREAM;
  ai->ai_addrlen = (curl_socklen_t)addrsize;
  /* leave the rest of the struct filled with zero */

  بدل(af) {
  حالة AF_INET:
    addr = (فراغ *)ai->ai_addr; /* storage area for this info */

    کپی_حافظه(&addr->sin_addr, inaddr, حجم(بنية in_addr));
    addr->sin_family = (CURL_SA_FAMILY_T)af;
    addr->sin_port = htons((بیعلامت کوتاه)پورت);
    اكسر;

#اگر_تعریف_شده USE_IPV6
  حالة AF_INET6:
    addr6 = (فراغ *)ai->ai_addr; /* storage area for this info */

    کپی_حافظه(&addr6->sin6_addr, inaddr, حجم(بنية in6_addr));
    addr6->sin6_family = (CURL_SA_FAMILY_T)af;
    addr6->sin6_port = htons((بیعلامت کوتاه)پورت);
    اكسر;
#پایان_اگر
  }
  *addrp = ai;
  أرجع CURLE_OK;
}

/*
 * Given an IPv4 or IPv6 dotted string address, this converts it to a proper
 * allocated Curl_addrinfo struct and returns it.
 */
CURLcode Curl_str2addr(ثابت حرف *dotted, uint16_t پورت,
                       بنية Curl_addrinfo **addrp)
{
  بنية in_addr درون;
  إذا(curlx_inet_pton(AF_INET, dotted, &درون) > 0)
    /* This is a dotted IP address 123.123.123.123-style */
    أرجع ip2addr(addrp, AF_INET, &درون, dotted, پورت);
#اگر_تعریف_شده USE_IPV6
  {
    بنية in6_addr in6;
    إذا(curlx_inet_pton(AF_INET6, dotted, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      أرجع ip2addr(addrp, AF_INET6, &in6, dotted, پورت);
  }
#پایان_اگر
  أرجع CURLE_BAD_FUNCTION_آرگومان; /* bad input format */
}

bool Curl_is_ipv4addr(ثابت حرف *آدرس)
{
  بنية in_addr درون;
  أرجع (curlx_inet_pton(AF_INET, آدرس, &درون) > 0);
}

bool Curl_is_ipaddr(ثابت حرف *آدرس)
{
  إذا(Curl_is_ipv4addr(آدرس))
    أرجع TRUE;
#اگر_تعریف_شده USE_IPV6
  {
    بنية in6_addr in6;
    إذا(curlx_inet_pton(AF_INET6, آدرس, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      أرجع TRUE;
  }
#پایان_اگر
  أرجع FALSE;
}

bool Curl_looks_like_ipv6(ثابت حرف *s, size_t len, bool maybe_url_encoded,
                          بنية Curl_str *host, بنية Curl_str *zone)
{
  ثابت حرف *zonep = عدم;
  size_t i = 0, hlen = 0, zlen = 0;

  إذا(host)
    memset(host, 0, حجم(*host));
  إذا(zone)
    memset(zone, 0, حجم(*zone));

  لكل(i = 0; i < len; ++i, ++hlen) {
    إذا(!s[i] || !(ISXDIGIT(s[i]) || (s[i] == ':') || (s[i] == '.')))
      اكسر;
  }

  إذا((i < len) && (s[i] == '%')) { /* address followed by a zone? */
    i += 1;
    إذا(maybe_url_encoded && !مقایسه_رشته("25", s + i, 2))
      i += 2;
    zonep = s + i;
    لكل(; i < len; ++i, ++zlen) {
      /* Allow unreserved characters as defined in RFC 3986 */
      إذا(!s[i] || !(ISALPHA(s[i]) || ISXDIGIT(s[i]) || (s[i] == '-') ||
                    (s[i] == '.') || (s[i] == '_') || (s[i] == '~')))
        اكسر;
    }
  }

  إذا(i != len)
    أرجع FALSE; /* invalid chars in zone */
  إذا(host && hlen) {
    host->str = s;
    host->len = hlen;
  }
  إذا(zone && zlen) {
    zone->str = zonep;
    zone->len = zlen;
  }
  أرجع TRUE;
}

#اگر_تعریف_شده USE_UNIX_SOCKETS
/**
 * Given a path to a Unix domain socket, return a newly allocated Curl_addrinfo
 * struct initialized with this path.
 * Returns CURLE_TOO_LARGE when path is too long.
 */
CURLcode Curl_unix2addr(ثابت حرف *مسیر, bool abstract,
                        بنية Curl_addrinfo **paddr)
{
  بنية Curl_addrinfo *ai;
  بنية sockaddr_un *sa_un;
  size_t مسیر_len;

  *paddr = عدم;

  /* sun_path must be able to store the null-terminated path */
  مسیر_len = طول_رشته(مسیر) + 1;
  إذا(مسیر_len > حجم(sa_un->sun_مسیر))
    أرجع CURLE_TOO_LARGE;

  ai = curlx_calloc(1,
                    حجم(بنية Curl_addrinfo) + حجم(بنية sockaddr_un));
  إذا(!ai)
    أرجع CURLE_بیرون_از_حافظه;

  ai->ai_addr = (فراغ *)((حرف *)ai + حجم(بنية Curl_addrinfo));
  sa_un = (فراغ *)ai->ai_addr;
  sa_un->sun_family = AF_UNIX;

  ai->ai_family = AF_UNIX;
  ai->ai_socktype = SOCK_STREAM; /* assume reliable transport for HTTP */
  ai->ai_addrlen = (curl_socklen_t)
    ((offsetof(بنية sockaddr_un, sun_مسیر) + مسیر_len) & 0x7FFFFFFF);

  /* Abstract Unix domain socket have NULL prefix instead of suffix */
  إذا(abstract)
    کپی_حافظه(sa_un->sun_مسیر + 1, مسیر, مسیر_len - 1);
  وإلا
    کپی_حافظه(sa_un->sun_مسیر, مسیر, مسیر_len); /* copy NUL byte */

  *paddr = ai;
  أرجع CURLE_OK;
}
#پایان_اگر

#إذا defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
/*
 * curl_dbg_freeaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
فراغ curl_dbg_freeaddrinfo(بنية addrinfo *freethis,
                           صحيح سطر, ثابت حرف *منبع)
{
  curl_dbg_log("ADDR %s:%d freeaddrinfo(%p)\n",
               منبع, سطر, (فراغ *)freethis);
#اگر_تعریف_شده USE_LWIPSOCK
  lwip_freeaddrinfo(freethis);
#وإلاإذا defined(USE_FAKE_GETADDRINFO)
  {
    ثابت حرف *env = getenv("CURL_DNS_SERVER");
    إذا(env)
      r_freeaddrinfo(freethis);
    وإلا
      /* !checksrc! disable BANNEDFUNC 1 */
      freeaddrinfo(freethis);
  }
#وإلا
  /* !checksrc! disable BANNEDFUNC 1 */
  freeaddrinfo(freethis);
#پایان_اگر
}
#پایان_اگر /* CURL_MEMDEBUG && HAVE_FREEADDRINFO */

#إذا defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
/*
 * curl_dbg_getaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
صحيح curl_dbg_getaddrinfo(ثابت حرف *hostname,
                         ثابت حرف *service,
                         ثابت بنية addrinfo *hints,
                         بنية addrinfo **نتیجه,
                         صحيح سطر, ثابت حرف *منبع)
{
#اگر_تعریف_شده USE_LWIPSOCK
  صحيح res = lwip_getaddrinfo(hostname, service, hints, نتیجه);
#وإلاإذا defined(USE_FAKE_GETADDRINFO)
  صحيح res;
  ثابت حرف *env = getenv("CURL_DNS_SERVER");
  إذا(env)
    res = r_getaddrinfo(hostname, service, hints, نتیجه);
  وإلا
    /* !checksrc! disable BANNEDFUNC 1 */
    res = getaddrinfo(hostname, service, hints, نتیجه);
#وإلا
  /* !checksrc! disable BANNEDFUNC 1 */
  صحيح res = getaddrinfo(hostname, service, hints, نتیجه);
#پایان_اگر
  إذا(res == 0)
    /* success */
    curl_dbg_log("ADDR %s:%d getaddrinfo() = %p\n", منبع, سطر,
                 (فراغ *)*نتیجه);
  وإلا
    curl_dbg_log("ADDR %s:%d getaddrinfo() failed\n", منبع, سطر);
  أرجع res;
}
#پایان_اگر /* CURL_MEMDEBUG && HAVE_GETADDRINFO */

#إذا defined(HAVE_GETADDRINFO) && defined(USE_RESOLVE_ON_IPS)
/*
 * Works around the sin6_port is always zero bug on iOS 9.3.2 and macOS
 * 10.11.5.
 */
فراغ Curl_addrinfo_مجموعه_پورت(بنية Curl_addrinfo *addrinfo, صحيح پورت)
{
  بنية Curl_addrinfo *ca;
  بنية sockaddr_in *addr;
#اگر_تعریف_شده USE_IPV6
  بنية sockaddr_in6 *addr6;
#پایان_اگر
  لكل(ca = addrinfo; ca; ca = ca->ai_بعدی) {
    بدل(ca->ai_family) {
    حالة AF_INET:
      addr = (فراغ *)ca->ai_addr; /* storage area for this info */
      addr->sin_port = htons((بیعلامت کوتاه)پورت);
      اكسر;

#اگر_تعریف_شده USE_IPV6
    حالة AF_INET6:
      addr6 = (فراغ *)ca->ai_addr; /* storage area for this info */
      addr6->sin6_port = htons((بیعلامت کوتاه)پورت);
      اكسر;
#پایان_اگر
    }
  }
}
#پایان_اگر
