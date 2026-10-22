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

#ifdef HAVE_NETINET_IN_H
#ضمن <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#ضمن <netinet/in6.h>
#endif
#ifdef HAVE_NETDB_H
#ضمن <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#endif
#ifdef HAVE_SYS_UN_H
#ضمن <sys/un.h>
#endif

#ifdef __VMS
#ضمن <in.h>
#ضمن <inet.h>
#endif

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

#אם defined(__INTEL_COMPILER) && (__INTEL_COMPILER == 910) &&     \
  defined(__OPTIMIZE__) && defined(__unix__) && defined(__i386__)
  /* workaround icc 9.1 optimizer issue */
#  define vqualifier متطاير
#אחרת
#  define vqualifier
#endif

فراغ Curl_freeaddrinfo(بنية Curl_addrinfo *cahead)
{
  بنية Curl_addrinfo *vqualifier canext;
  بنية Curl_addrinfo *ca;

  עבור(ca = cahead; ca; ca = canext) {
    canext = ca->ai_next;
    curlx_free(ca);
  }
}

بنية Curl_addrinfo *Curl_addrinfo_get(بنية Curl_addrinfo *ai,
                                        صحيح ai_family,
                                        unsigned صحيح n)
{
  unsigned صحيح i;
  עבור(i = 0; ai; ai = ai->ai_next) {
    אם(ai->ai_family == ai_family) {
      אם(i == n)
        החזר ai;
      ++i;
    }
  }
  החזר عدم;
}

#ifdef HAVE_GETADDRINFO
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
                        بنية Curl_addrinfo **result)
{
  ثابت بنية addrinfo *ai;
  بنية addrinfo *aihead;
  بنية Curl_addrinfo *cafirst = عدم;
  بنية Curl_addrinfo *calast = عدم;
  بنية Curl_addrinfo *ca;
  size_t ss_size;
  صحيح error;

  *result = عدم; /* assume failure */

  error = CURL_GETADDRINFO(nodename, servname, hints, &aihead);
  אם(error)
    החזר error;

  /* traverse the addrinfo list */

  עבור(ai = aihead; ai; ai = ai->ai_next) {
    size_t namelen = ai->ai_canonname ? strlen(ai->ai_canonname) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    אם(ai->ai_family == AF_INET)
      ss_size = حجم(بنية sockaddr_in);
#ifdef USE_IPV6
    אחרת אם(ai->ai_family == AF_INET6)
      ss_size = حجم(بنية sockaddr_in6);
#endif
    אחרת
      استمر;

    /* ignore elements without required address info */
    אם(!ai->ai_addr || !(ai->ai_addrlen > 0))
      استمر;

    /* ignore elements with bogus address size */
    אם((size_t)ai->ai_addrlen < ss_size)
      استمر;

    ca = curlx_malloc(حجم(بنية Curl_addrinfo) + ss_size + namelen);
    אם(!ca) {
      error = EAI_MEMORY;
      اكسر;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = عدم;
    ca->ai_canonname = عدم;
    ca->ai_next      = عدم;

    ca->ai_addr = (فراغ *)((حرف *)ca + حجم(بنية Curl_addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    אם(namelen) {
      ca->ai_canonname = (فراغ *)((حرف *)ca->ai_addr + ss_size);
      memcpy(ca->ai_canonname, ai->ai_canonname, namelen);
    }

    /* if the return list is empty, this becomes the first element */
    אם(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    אם(calast)
      calast->ai_next = ca;
    calast = ca;
  }

  /* destroy the addrinfo list */
  אם(aihead)
    CURL_FREEADDRINFO(aihead);

  /* if we failed, also destroy the Curl_addrinfo list */
  אם(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = عدم;
  }
  אחרת אם(!cafirst) {
#ifdef EAI_NONAME
    /* rfc3493 conformant */
    error = EAI_NONAME;
#אחרת
    /* rfc3493 obsoleted */
    error = EAI_NODATA;
#endif
#ifdef USE_WINSOCK
    SET_SOCKERRNO(error);
#endif
  }

  *result = cafirst;

  /* This is not a CURLcode */
  החזר error;
}
#endif /* HAVE_GETADDRINFO */

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
#אם !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
بنية Curl_addrinfo *Curl_he2ai(ثابت بنية hostent *he, صحيح port)
{
  بنية Curl_addrinfo *ai;
  بنية Curl_addrinfo *prevai = عدم;
  بنية Curl_addrinfo *firstai = عدم;
  بنية sockaddr_in *addr;
#ifdef USE_IPV6
  بنية sockaddr_in6 *addr6;
#endif
  CURLcode result = CURLE_OK;
  صحيح i;
  حرف *curr;

  אם(!he)
    /* no input == no output! */
    החזר عدم;

  DEBUGASSERT(he->h_name && he->h_addr_list);

  עבור(i = 0; (curr = he->h_addr_list[i]) != عدم; i++) {
    size_t ss_size;
    size_t namelen = strlen(he->h_name) + 1; /* include null-terminator */
#ifdef USE_IPV6
    אם(he->h_addrtype == AF_INET6)
      ss_size = حجم(بنية sockaddr_in6);
    אחרת
#endif
      ss_size = حجم(بنية sockaddr_in);

    /* allocate memory to hold the struct, the address and the name */
    ai = curlx_calloc(1, حجم(بنية Curl_addrinfo) + ss_size + namelen);
    אם(!ai) {
      result = CURLE_OUT_OF_MEMORY;
      اكسر;
    }
    /* put the address after the struct */
    ai->ai_addr = (فراغ *)((حرف *)ai + حجم(بنية Curl_addrinfo));
    /* then put the name after the address */
    ai->ai_canonname = (حرف *)ai->ai_addr + ss_size;
    memcpy(ai->ai_canonname, he->h_name, namelen);

    אם(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    אם(prevai)
      /* make the previous entry point to this */
      prevai->ai_next = ai;

    ai->ai_family = he->h_addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_size;

    /* leave the rest of the struct filled with zero */

    بدل(ai->ai_family) {
    حالة AF_INET:
      addr = (فراغ *)ai->ai_addr; /* storage area for this info */

      memcpy(&addr->sin_addr, curr, حجم(بنية in_addr));
      addr->sin_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr->sin_port = htons((unsigned short)port);
      اكسر;

#ifdef USE_IPV6
    حالة AF_INET6:
      addr6 = (فراغ *)ai->ai_addr; /* storage area for this info */

      memcpy(&addr6->sin6_addr, curr, حجم(بنية in6_addr));
      addr6->sin6_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr6->sin6_port = htons((unsigned short)port);
      اكسر;
#endif
    }

    prevai = ai;
  }

  אם(result) {
    Curl_freeaddrinfo(firstai);
    firstai = عدم;
  }

  החזר firstai;
}
#endif

/*
 * ip2addr()
 *
 * This function takes an Internet address, in binary form, as input parameter
 * along with its address family and the string version of the address, and it
 * returns a Curl_addrinfo chain filled in correctly with information for the
 * given address/host
 */
ساكن CURLcode ip2addr(بنية Curl_addrinfo **addrp, صحيح af,
                        ثابت فراغ *inaddr, ثابت حرف *hostname, صحيح port)
{
  بنية Curl_addrinfo *ai;
  size_t addrsize;
  size_t namelen;
  بنية sockaddr_in *addr;
#ifdef USE_IPV6
  بنية sockaddr_in6 *addr6;
#endif

  DEBUGASSERT(inaddr && hostname);

  namelen = strlen(hostname) + 1;
  *addrp = عدم;

  אם(af == AF_INET)
    addrsize = حجم(بنية sockaddr_in);
#ifdef USE_IPV6
  אחרת אם(af == AF_INET6)
    addrsize = حجم(بنية sockaddr_in6);
#endif
  אחרת
    החזר CURLE_BAD_FUNCTION_ARGUMENT;

  /* allocate memory to hold the struct, the address and the name */
  ai = curlx_calloc(1, حجم(بنية Curl_addrinfo) + addrsize + namelen);
  אם(!ai)
    החזר CURLE_OUT_OF_MEMORY;
  /* put the address after the struct */
  ai->ai_addr = (فراغ *)((حرف *)ai + حجم(بنية Curl_addrinfo));
  /* then put the name after the address */
  ai->ai_canonname = (حرف *)ai->ai_addr + addrsize;
  memcpy(ai->ai_canonname, hostname, namelen);
  ai->ai_family = af;
  ai->ai_socktype = SOCK_STREAM;
  ai->ai_addrlen = (curl_socklen_t)addrsize;
  /* leave the rest of the struct filled with zero */

  بدل(af) {
  حالة AF_INET:
    addr = (فراغ *)ai->ai_addr; /* storage area for this info */

    memcpy(&addr->sin_addr, inaddr, حجم(بنية in_addr));
    addr->sin_family = (CURL_SA_FAMILY_T)af;
    addr->sin_port = htons((unsigned short)port);
    اكسر;

#ifdef USE_IPV6
  حالة AF_INET6:
    addr6 = (فراغ *)ai->ai_addr; /* storage area for this info */

    memcpy(&addr6->sin6_addr, inaddr, حجم(بنية in6_addr));
    addr6->sin6_family = (CURL_SA_FAMILY_T)af;
    addr6->sin6_port = htons((unsigned short)port);
    اكسر;
#endif
  }
  *addrp = ai;
  החזר CURLE_OK;
}

/*
 * Given an IPv4 or IPv6 dotted string address, this converts it to a proper
 * allocated Curl_addrinfo struct and returns it.
 */
CURLcode Curl_str2addr(ثابت حرف *dotted, uint16_t port,
                       بنية Curl_addrinfo **addrp)
{
  بنية in_addr in;
  אם(curlx_inet_pton(AF_INET, dotted, &in) > 0)
    /* This is a dotted IP address 123.123.123.123-style */
    החזר ip2addr(addrp, AF_INET, &in, dotted, port);
#ifdef USE_IPV6
  {
    بنية in6_addr in6;
    אם(curlx_inet_pton(AF_INET6, dotted, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      החזר ip2addr(addrp, AF_INET6, &in6, dotted, port);
  }
#endif
  החזר CURLE_BAD_FUNCTION_ARGUMENT; /* bad input format */
}

bool Curl_is_ipv4addr(ثابت حرف *address)
{
  بنية in_addr in;
  החזר (curlx_inet_pton(AF_INET, address, &in) > 0);
}

bool Curl_is_ipaddr(ثابت حرف *address)
{
  אם(Curl_is_ipv4addr(address))
    החזר TRUE;
#ifdef USE_IPV6
  {
    بنية in6_addr in6;
    אם(curlx_inet_pton(AF_INET6, address, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      החזר TRUE;
  }
#endif
  החזר FALSE;
}

bool Curl_looks_like_ipv6(ثابت حرف *s, size_t len, bool maybe_url_encoded,
                          بنية Curl_str *host, بنية Curl_str *zone)
{
  ثابت حرف *zonep = عدم;
  size_t i = 0, hlen = 0, zlen = 0;

  אם(host)
    memset(host, 0, حجم(*host));
  אם(zone)
    memset(zone, 0, حجم(*zone));

  עבור(i = 0; i < len; ++i, ++hlen) {
    אם(!s[i] || !(ISXDIGIT(s[i]) || (s[i] == ':') || (s[i] == '.')))
      اكسر;
  }

  אם((i < len) && (s[i] == '%')) { /* address followed by a zone? */
    i += 1;
    אם(maybe_url_encoded && !strncmp("25", s + i, 2))
      i += 2;
    zonep = s + i;
    עבור(; i < len; ++i, ++zlen) {
      /* Allow unreserved characters as defined in RFC 3986 */
      אם(!s[i] || !(ISALPHA(s[i]) || ISXDIGIT(s[i]) || (s[i] == '-') ||
                    (s[i] == '.') || (s[i] == '_') || (s[i] == '~')))
        اكسر;
    }
  }

  אם(i != len)
    החזר FALSE; /* invalid chars in zone */
  אם(host && hlen) {
    host->str = s;
    host->len = hlen;
  }
  אם(zone && zlen) {
    zone->str = zonep;
    zone->len = zlen;
  }
  החזר TRUE;
}

#ifdef USE_UNIX_SOCKETS
/**
 * Given a path to a Unix domain socket, return a newly allocated Curl_addrinfo
 * struct initialized with this path.
 * Returns CURLE_TOO_LARGE when path is too long.
 */
CURLcode Curl_unix2addr(ثابت حرف *path, bool abstract,
                        بنية Curl_addrinfo **paddr)
{
  بنية Curl_addrinfo *ai;
  بنية sockaddr_un *sa_un;
  size_t path_len;

  *paddr = عدم;

  /* sun_path must be able to store the null-terminated path */
  path_len = strlen(path) + 1;
  אם(path_len > حجم(sa_un->sun_path))
    החזר CURLE_TOO_LARGE;

  ai = curlx_calloc(1,
                    حجم(بنية Curl_addrinfo) + حجم(بنية sockaddr_un));
  אם(!ai)
    החזר CURLE_OUT_OF_MEMORY;

  ai->ai_addr = (فراغ *)((حرف *)ai + حجم(بنية Curl_addrinfo));
  sa_un = (فراغ *)ai->ai_addr;
  sa_un->sun_family = AF_UNIX;

  ai->ai_family = AF_UNIX;
  ai->ai_socktype = SOCK_STREAM; /* assume reliable transport for HTTP */
  ai->ai_addrlen = (curl_socklen_t)
    ((offsetof(بنية sockaddr_un, sun_path) + path_len) & 0x7FFFFFFF);

  /* Abstract Unix domain socket have NULL prefix instead of suffix */
  אם(abstract)
    memcpy(sa_un->sun_path + 1, path, path_len - 1);
  אחרת
    memcpy(sa_un->sun_path, path, path_len); /* copy NUL byte */

  *paddr = ai;
  החזר CURLE_OK;
}
#endif

#אם defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
/*
 * curl_dbg_freeaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
فراغ curl_dbg_freeaddrinfo(بنية addrinfo *freethis,
                           صحيح line, ثابت حرف *source)
{
  curl_dbg_log("ADDR %s:%d freeaddrinfo(%p)\n",
               source, line, (فراغ *)freethis);
#ifdef USE_LWIPSOCK
  lwip_freeaddrinfo(freethis);
#وإلاإذا defined(USE_FAKE_GETADDRINFO)
  {
    ثابت حرف *env = getenv("CURL_DNS_SERVER");
    אם(env)
      r_freeaddrinfo(freethis);
    אחרת
      /* !checksrc! disable BANNEDFUNC 1 */
      freeaddrinfo(freethis);
  }
#אחרת
  /* !checksrc! disable BANNEDFUNC 1 */
  freeaddrinfo(freethis);
#endif
}
#endif /* CURL_MEMDEBUG && HAVE_FREEADDRINFO */

#אם defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
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
                         بنية addrinfo **result,
                         صحيح line, ثابت حرف *source)
{
#ifdef USE_LWIPSOCK
  صحيح res = lwip_getaddrinfo(hostname, service, hints, result);
#وإلاإذا defined(USE_FAKE_GETADDRINFO)
  صحيح res;
  ثابت حرف *env = getenv("CURL_DNS_SERVER");
  אם(env)
    res = r_getaddrinfo(hostname, service, hints, result);
  אחרת
    /* !checksrc! disable BANNEDFUNC 1 */
    res = getaddrinfo(hostname, service, hints, result);
#אחרת
  /* !checksrc! disable BANNEDFUNC 1 */
  صحيح res = getaddrinfo(hostname, service, hints, result);
#endif
  אם(res == 0)
    /* success */
    curl_dbg_log("ADDR %s:%d getaddrinfo() = %p\n", source, line,
                 (فراغ *)*result);
  אחרת
    curl_dbg_log("ADDR %s:%d getaddrinfo() failed\n", source, line);
  החזר res;
}
#endif /* CURL_MEMDEBUG && HAVE_GETADDRINFO */

#אם defined(HAVE_GETADDRINFO) && defined(USE_RESOLVE_ON_IPS)
/*
 * Works around the sin6_port is always zero bug on iOS 9.3.2 and macOS
 * 10.11.5.
 */
فراغ Curl_addrinfo_set_port(بنية Curl_addrinfo *addrinfo, صحيح port)
{
  بنية Curl_addrinfo *ca;
  بنية sockaddr_in *addr;
#ifdef USE_IPV6
  بنية sockaddr_in6 *addr6;
#endif
  עבור(ca = addrinfo; ca; ca = ca->ai_next) {
    بدل(ca->ai_family) {
    حالة AF_INET:
      addr = (فراغ *)ca->ai_addr; /* storage area for this info */
      addr->sin_port = htons((unsigned short)port);
      اكسر;

#ifdef USE_IPV6
    حالة AF_INET6:
      addr6 = (فراغ *)ca->ai_addr; /* storage area for this info */
      addr6->sin6_port = htons((unsigned short)port);
      اكسر;
#endif
    }
  }
}
#endif
