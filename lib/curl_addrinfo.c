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

#ifdef HAVE_NETINET_IN_H
#شامل <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#شامل <netinet/in6.h>
#endif
#ifdef HAVE_NETDB_H
#شامل <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#endif
#ifdef HAVE_SYS_UN_H
#شامل <sys/un.h>
#endif

#ifdef __VMS
#شامل <in.h>
#شامل <inet.h>
#endif

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
#  define vqualifier فرار
#وگرنه
#  define vqualifier
#endif

تهی Curl_freeaddrinfo(ساختار Curl_addrinfo *cahead)
{
  ساختار Curl_addrinfo *vqualifier canext;
  ساختار Curl_addrinfo *ca;

  برای(ca = cahead; ca; ca = canext) {
    canext = ca->ai_next;
    curlx_free(ca);
  }
}

ساختار Curl_addrinfo *Curl_addrinfo_get(ساختار Curl_addrinfo *ai,
                                        صحیح ai_family,
                                        unsigned صحیح n)
{
  unsigned صحیح i;
  برای(i = 0; ai; ai = ai->ai_next) {
    اگر(ai->ai_family == ai_family) {
      اگر(i == n)
        بازگشت ai;
      ++i;
    }
  }
  بازگشت پوچ;
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
صحیح Curl_getaddrinfo_ex(ثابت نویسه *nodename,
                        ثابت نویسه *servname,
                        ثابت ساختار addrinfo *hints,
                        ساختار Curl_addrinfo **result)
{
  ثابت ساختار addrinfo *ai;
  ساختار addrinfo *aihead;
  ساختار Curl_addrinfo *cafirst = پوچ;
  ساختار Curl_addrinfo *calast = پوچ;
  ساختار Curl_addrinfo *ca;
  size_t ss_size;
  صحیح error;

  *result = پوچ; /* assume failure */

  error = CURL_GETADDRINFO(nodename, servname, hints, &aihead);
  اگر(error)
    بازگشت error;

  /* traverse the addrinfo list */

  برای(ai = aihead; ai; ai = ai->ai_next) {
    size_t namelen = ai->ai_canonname ? strlen(ai->ai_canonname) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    اگر(ai->ai_family == AF_INET)
      ss_size = اندازه(ساختار sockaddr_in);
#ifdef USE_IPV6
    وگرنه اگر(ai->ai_family == AF_INET6)
      ss_size = اندازه(ساختار sockaddr_in6);
#endif
    وگرنه
      ادامه;

    /* ignore elements without required address info */
    اگر(!ai->ai_addr || !(ai->ai_addrlen > 0))
      ادامه;

    /* ignore elements with bogus address size */
    اگر((size_t)ai->ai_addrlen < ss_size)
      ادامه;

    ca = curlx_malloc(اندازه(ساختار Curl_addrinfo) + ss_size + namelen);
    اگر(!ca) {
      error = EAI_MEMORY;
      بشکن;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = پوچ;
    ca->ai_canonname = پوچ;
    ca->ai_next      = پوچ;

    ca->ai_addr = (تهی *)((نویسه *)ca + اندازه(ساختار Curl_addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    اگر(namelen) {
      ca->ai_canonname = (تهی *)((نویسه *)ca->ai_addr + ss_size);
      memcpy(ca->ai_canonname, ai->ai_canonname, namelen);
    }

    /* if the return list is empty, this becomes the first element */
    اگر(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    اگر(calast)
      calast->ai_next = ca;
    calast = ca;
  }

  /* destroy the addrinfo list */
  اگر(aihead)
    CURL_FREEADDRINFO(aihead);

  /* if we failed, also destroy the Curl_addrinfo list */
  اگر(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = پوچ;
  }
  وگرنه اگر(!cafirst) {
#ifdef EAI_NONAME
    /* rfc3493 conformant */
    error = EAI_NONAME;
#وگرنه
    /* rfc3493 obsoleted */
    error = EAI_NODATA;
#endif
#ifdef USE_WINSOCK
    SET_SOCKERRNO(error);
#endif
  }

  *result = cafirst;

  /* This is not a CURLcode */
  بازگشت error;
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
#اگر !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
ساختار Curl_addrinfo *Curl_he2ai(ثابت ساختار hostent *he, صحیح port)
{
  ساختار Curl_addrinfo *ai;
  ساختار Curl_addrinfo *prevai = پوچ;
  ساختار Curl_addrinfo *firstai = پوچ;
  ساختار sockaddr_in *addr;
#ifdef USE_IPV6
  ساختار sockaddr_in6 *addr6;
#endif
  CURLcode result = CURLE_OK;
  صحیح i;
  نویسه *curr;

  اگر(!he)
    /* no input == no output! */
    بازگشت پوچ;

  DEBUGASSERT(he->h_name && he->h_addr_list);

  برای(i = 0; (curr = he->h_addr_list[i]) != پوچ; i++) {
    size_t ss_size;
    size_t namelen = strlen(he->h_name) + 1; /* include null-terminator */
#ifdef USE_IPV6
    اگر(he->h_addrtype == AF_INET6)
      ss_size = اندازه(ساختار sockaddr_in6);
    وگرنه
#endif
      ss_size = اندازه(ساختار sockaddr_in);

    /* allocate memory to hold the struct, the address and the name */
    ai = curlx_calloc(1, اندازه(ساختار Curl_addrinfo) + ss_size + namelen);
    اگر(!ai) {
      result = CURLE_OUT_OF_MEMORY;
      بشکن;
    }
    /* put the address after the struct */
    ai->ai_addr = (تهی *)((نویسه *)ai + اندازه(ساختار Curl_addrinfo));
    /* then put the name after the address */
    ai->ai_canonname = (نویسه *)ai->ai_addr + ss_size;
    memcpy(ai->ai_canonname, he->h_name, namelen);

    اگر(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    اگر(prevai)
      /* make the previous entry point to this */
      prevai->ai_next = ai;

    ai->ai_family = he->h_addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_size;

    /* leave the rest of the struct filled with zero */

    انتخاب(ai->ai_family) {
    حالت AF_INET:
      addr = (تهی *)ai->ai_addr; /* storage area for this info */

      memcpy(&addr->sin_addr, curr, اندازه(ساختار in_addr));
      addr->sin_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr->sin_port = htons((unsigned short)port);
      بشکن;

#ifdef USE_IPV6
    حالت AF_INET6:
      addr6 = (تهی *)ai->ai_addr; /* storage area for this info */

      memcpy(&addr6->sin6_addr, curr, اندازه(ساختار in6_addr));
      addr6->sin6_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr6->sin6_port = htons((unsigned short)port);
      بشکن;
#endif
    }

    prevai = ai;
  }

  اگر(result) {
    Curl_freeaddrinfo(firstai);
    firstai = پوچ;
  }

  بازگشت firstai;
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
ایستا CURLcode ip2addr(ساختار Curl_addrinfo **addrp, صحیح af,
                        ثابت تهی *inaddr, ثابت نویسه *hostname, صحیح port)
{
  ساختار Curl_addrinfo *ai;
  size_t addrsize;
  size_t namelen;
  ساختار sockaddr_in *addr;
#ifdef USE_IPV6
  ساختار sockaddr_in6 *addr6;
#endif

  DEBUGASSERT(inaddr && hostname);

  namelen = strlen(hostname) + 1;
  *addrp = پوچ;

  اگر(af == AF_INET)
    addrsize = اندازه(ساختار sockaddr_in);
#ifdef USE_IPV6
  وگرنه اگر(af == AF_INET6)
    addrsize = اندازه(ساختار sockaddr_in6);
#endif
  وگرنه
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  /* allocate memory to hold the struct, the address and the name */
  ai = curlx_calloc(1, اندازه(ساختار Curl_addrinfo) + addrsize + namelen);
  اگر(!ai)
    بازگشت CURLE_OUT_OF_MEMORY;
  /* put the address after the struct */
  ai->ai_addr = (تهی *)((نویسه *)ai + اندازه(ساختار Curl_addrinfo));
  /* then put the name after the address */
  ai->ai_canonname = (نویسه *)ai->ai_addr + addrsize;
  memcpy(ai->ai_canonname, hostname, namelen);
  ai->ai_family = af;
  ai->ai_socktype = SOCK_STREAM;
  ai->ai_addrlen = (curl_socklen_t)addrsize;
  /* leave the rest of the struct filled with zero */

  انتخاب(af) {
  حالت AF_INET:
    addr = (تهی *)ai->ai_addr; /* storage area for this info */

    memcpy(&addr->sin_addr, inaddr, اندازه(ساختار in_addr));
    addr->sin_family = (CURL_SA_FAMILY_T)af;
    addr->sin_port = htons((unsigned short)port);
    بشکن;

#ifdef USE_IPV6
  حالت AF_INET6:
    addr6 = (تهی *)ai->ai_addr; /* storage area for this info */

    memcpy(&addr6->sin6_addr, inaddr, اندازه(ساختار in6_addr));
    addr6->sin6_family = (CURL_SA_FAMILY_T)af;
    addr6->sin6_port = htons((unsigned short)port);
    بشکن;
#endif
  }
  *addrp = ai;
  بازگشت CURLE_OK;
}

/*
 * Given an IPv4 or IPv6 dotted string address, this converts it to a proper
 * allocated Curl_addrinfo struct and returns it.
 */
CURLcode Curl_str2addr(ثابت نویسه *dotted, uint16_t port,
                       ساختار Curl_addrinfo **addrp)
{
  ساختار in_addr in;
  اگر(curlx_inet_pton(AF_INET, dotted, &in) > 0)
    /* This is a dotted IP address 123.123.123.123-style */
    بازگشت ip2addr(addrp, AF_INET, &in, dotted, port);
#ifdef USE_IPV6
  {
    ساختار in6_addr in6;
    اگر(curlx_inet_pton(AF_INET6, dotted, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      بازگشت ip2addr(addrp, AF_INET6, &in6, dotted, port);
  }
#endif
  بازگشت CURLE_BAD_FUNCTION_ARGUMENT; /* bad input format */
}

bool Curl_is_ipv4addr(ثابت نویسه *address)
{
  ساختار in_addr in;
  بازگشت (curlx_inet_pton(AF_INET, address, &in) > 0);
}

bool Curl_is_ipaddr(ثابت نویسه *address)
{
  اگر(Curl_is_ipv4addr(address))
    بازگشت TRUE;
#ifdef USE_IPV6
  {
    ساختار in6_addr in6;
    اگر(curlx_inet_pton(AF_INET6, address, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      بازگشت TRUE;
  }
#endif
  بازگشت FALSE;
}

bool Curl_looks_like_ipv6(ثابت نویسه *s, size_t len, bool maybe_url_encoded,
                          ساختار Curl_str *host, ساختار Curl_str *zone)
{
  ثابت نویسه *zonep = پوچ;
  size_t i = 0, hlen = 0, zlen = 0;

  اگر(host)
    memset(host, 0, اندازه(*host));
  اگر(zone)
    memset(zone, 0, اندازه(*zone));

  برای(i = 0; i < len; ++i, ++hlen) {
    اگر(!s[i] || !(ISXDIGIT(s[i]) || (s[i] == ':') || (s[i] == '.')))
      بشکن;
  }

  اگر((i < len) && (s[i] == '%')) { /* address followed by a zone? */
    i += 1;
    اگر(maybe_url_encoded && !strncmp("25", s + i, 2))
      i += 2;
    zonep = s + i;
    برای(; i < len; ++i, ++zlen) {
      /* Allow unreserved characters as defined in RFC 3986 */
      اگر(!s[i] || !(ISALPHA(s[i]) || ISXDIGIT(s[i]) || (s[i] == '-') ||
                    (s[i] == '.') || (s[i] == '_') || (s[i] == '~')))
        بشکن;
    }
  }

  اگر(i != len)
    بازگشت FALSE; /* invalid chars in zone */
  اگر(host && hlen) {
    host->str = s;
    host->len = hlen;
  }
  اگر(zone && zlen) {
    zone->str = zonep;
    zone->len = zlen;
  }
  بازگشت TRUE;
}

#ifdef USE_UNIX_SOCKETS
/**
 * Given a path to a Unix domain socket, return a newly allocated Curl_addrinfo
 * struct initialized with this path.
 * Returns CURLE_TOO_LARGE when path is too long.
 */
CURLcode Curl_unix2addr(ثابت نویسه *path, bool abstract,
                        ساختار Curl_addrinfo **paddr)
{
  ساختار Curl_addrinfo *ai;
  ساختار sockaddr_un *sa_un;
  size_t path_len;

  *paddr = پوچ;

  /* sun_path must be able to store the null-terminated path */
  path_len = strlen(path) + 1;
  اگر(path_len > اندازه(sa_un->sun_path))
    بازگشت CURLE_TOO_LARGE;

  ai = curlx_calloc(1,
                    اندازه(ساختار Curl_addrinfo) + اندازه(ساختار sockaddr_un));
  اگر(!ai)
    بازگشت CURLE_OUT_OF_MEMORY;

  ai->ai_addr = (تهی *)((نویسه *)ai + اندازه(ساختار Curl_addrinfo));
  sa_un = (تهی *)ai->ai_addr;
  sa_un->sun_family = AF_UNIX;

  ai->ai_family = AF_UNIX;
  ai->ai_socktype = SOCK_STREAM; /* assume reliable transport for HTTP */
  ai->ai_addrlen = (curl_socklen_t)
    ((offsetof(ساختار sockaddr_un, sun_path) + path_len) & 0x7FFFFFFF);

  /* Abstract Unix domain socket have NULL prefix instead of suffix */
  اگر(abstract)
    memcpy(sa_un->sun_path + 1, path, path_len - 1);
  وگرنه
    memcpy(sa_un->sun_path, path, path_len); /* copy NUL byte */

  *paddr = ai;
  بازگشت CURLE_OK;
}
#endif

#اگر defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
/*
 * curl_dbg_freeaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
تهی curl_dbg_freeaddrinfo(ساختار addrinfo *freethis,
                           صحیح سطر, ثابت نویسه *source)
{
  curl_dbg_log("ADDR %s:%d freeaddrinfo(%p)\n",
               source, سطر, (تهی *)freethis);
#ifdef USE_LWIPSOCK
  lwip_freeaddrinfo(freethis);
#وگرنهاگر defined(USE_FAKE_GETADDRINFO)
  {
    ثابت نویسه *env = getenv("CURL_DNS_SERVER");
    اگر(env)
      r_freeaddrinfo(freethis);
    وگرنه
      /* !checksrc! disable BANNEDFUNC 1 */
      freeaddrinfo(freethis);
  }
#وگرنه
  /* !checksrc! disable BANNEDFUNC 1 */
  freeaddrinfo(freethis);
#endif
}
#endif /* CURL_MEMDEBUG && HAVE_FREEADDRINFO */

#اگر defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
/*
 * curl_dbg_getaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
صحیح curl_dbg_getaddrinfo(ثابت نویسه *hostname,
                         ثابت نویسه *service,
                         ثابت ساختار addrinfo *hints,
                         ساختار addrinfo **result,
                         صحیح سطر, ثابت نویسه *source)
{
#ifdef USE_LWIPSOCK
  صحیح res = lwip_getaddrinfo(hostname, service, hints, result);
#وگرنهاگر defined(USE_FAKE_GETADDRINFO)
  صحیح res;
  ثابت نویسه *env = getenv("CURL_DNS_SERVER");
  اگر(env)
    res = r_getaddrinfo(hostname, service, hints, result);
  وگرنه
    /* !checksrc! disable BANNEDFUNC 1 */
    res = getaddrinfo(hostname, service, hints, result);
#وگرنه
  /* !checksrc! disable BANNEDFUNC 1 */
  صحیح res = getaddrinfo(hostname, service, hints, result);
#endif
  اگر(res == 0)
    /* success */
    curl_dbg_log("ADDR %s:%d getaddrinfo() = %p\n", source, سطر,
                 (تهی *)*result);
  وگرنه
    curl_dbg_log("ADDR %s:%d getaddrinfo() failed\n", source, سطر);
  بازگشت res;
}
#endif /* CURL_MEMDEBUG && HAVE_GETADDRINFO */

#اگر defined(HAVE_GETADDRINFO) && defined(USE_RESOLVE_ON_IPS)
/*
 * Works around the sin6_port is always zero bug on iOS 9.3.2 and macOS
 * 10.11.5.
 */
تهی Curl_addrinfo_set_port(ساختار Curl_addrinfo *addrinfo, صحیح port)
{
  ساختار Curl_addrinfo *ca;
  ساختار sockaddr_in *addr;
#ifdef USE_IPV6
  ساختار sockaddr_in6 *addr6;
#endif
  برای(ca = addrinfo; ca; ca = ca->ai_next) {
    انتخاب(ca->ai_family) {
    حالت AF_INET:
      addr = (تهی *)ca->ai_addr; /* storage area for this info */
      addr->sin_port = htons((unsigned short)port);
      بشکن;

#ifdef USE_IPV6
    حالت AF_INET6:
      addr6 = (تهی *)ca->ai_addr; /* storage area for this info */
      addr6->sin6_port = htons((unsigned short)port);
      بشکن;
#endif
    }
  }
}
#endif
