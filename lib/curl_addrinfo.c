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
#अन्तर्भाव "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#अन्तर्भाव <netinet/in6.h>
#endif
#ifdef HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif
#ifdef HAVE_SYS_UN_H
#अन्तर्भाव <sys/un.h>
#endif

#ifdef __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#endif

#अन्तर्भाव <stddef.h>  /* for offsetof() */

#अन्तर्भाव "curl_addrinfo.h"
#अन्तर्भाव "fake_addrinfo.h"
#अन्तर्भाव "curlx/inet_pton.h"
#अन्तर्भाव "curlx/strparse.h"

/*
 * Curl_freeaddrinfo()
 *
 * This is used to free a linked list of Curl_addrinfo structs along
 * with all its associated allocated storage. This function should be
 * called once for each successful call to Curl_getaddrinfo_ex() or to
 * any function call which actually allocates a Curl_addrinfo struct.
 */

#जइ defined(__INTEL_COMPILER) && (__INTEL_COMPILER == 910) &&     \
  defined(__OPTIMIZE__) && defined(__unix__) && defined(__i386__)
  /* workaround icc 9.1 optimizer issue */
#  define vqualifier चञ्चल
#अण्णहा
#  define vqualifier
#endif

शून्य Curl_freeaddrinfo(संरचना Curl_addrinfo *cahead)
{
  संरचना Curl_addrinfo *vqualifier canext;
  संरचना Curl_addrinfo *ca;

  प्रति(ca = cahead; ca; ca = canext) {
    canext = ca->ai_next;
    curlx_free(ca);
  }
}

संरचना Curl_addrinfo *Curl_addrinfo_get(संरचना Curl_addrinfo *ai,
                                        पूर्णाङ्क ai_family,
                                        unsigned पूर्णाङ्क n)
{
  unsigned पूर्णाङ्क i;
  प्रति(i = 0; ai; ai = ai->ai_next) {
    जइ(ai->ai_family == ai_family) {
      जइ(i == n)
        प्रत्यागम ai;
      ++i;
    }
  }
  प्रत्यागम रिक्त;
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
पूर्णाङ्क Curl_getaddrinfo_ex(ध्रुव वर्ण *nodename,
                        ध्रुव वर्ण *servname,
                        ध्रुव संरचना addrinfo *hints,
                        संरचना Curl_addrinfo **result)
{
  ध्रुव संरचना addrinfo *ai;
  संरचना addrinfo *aihead;
  संरचना Curl_addrinfo *cafirst = रिक्त;
  संरचना Curl_addrinfo *calast = रिक्त;
  संरचना Curl_addrinfo *ca;
  size_t ss_size;
  पूर्णाङ्क error;

  *result = रिक्त; /* assume failure */

  error = CURL_GETADDRINFO(nodename, servname, hints, &aihead);
  जइ(error)
    प्रत्यागम error;

  /* traverse the addrinfo list */

  प्रति(ai = aihead; ai; ai = ai->ai_next) {
    size_t namelen = ai->ai_canonname ? strlen(ai->ai_canonname) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    जइ(ai->ai_family == AF_INET)
      ss_size = परिमाण(संरचना sockaddr_in);
#ifdef USE_IPV6
    अण्णहा जइ(ai->ai_family == AF_INET6)
      ss_size = परिमाण(संरचना sockaddr_in6);
#endif
    अण्णहा
      अनुवर्त;

    /* ignore elements without required address info */
    जइ(!ai->ai_addr || !(ai->ai_addrlen > 0))
      अनुवर्त;

    /* ignore elements with bogus address size */
    जइ((size_t)ai->ai_addrlen < ss_size)
      अनुवर्त;

    ca = curlx_malloc(परिमाण(संरचना Curl_addrinfo) + ss_size + namelen);
    जइ(!ca) {
      error = EAI_MEMORY;
      भङ्ग;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = रिक्त;
    ca->ai_canonname = रिक्त;
    ca->ai_next      = रिक्त;

    ca->ai_addr = (शून्य *)((वर्ण *)ca + परिमाण(संरचना Curl_addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    जइ(namelen) {
      ca->ai_canonname = (शून्य *)((वर्ण *)ca->ai_addr + ss_size);
      memcpy(ca->ai_canonname, ai->ai_canonname, namelen);
    }

    /* if the return list is empty, this becomes the first element */
    जइ(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    जइ(calast)
      calast->ai_next = ca;
    calast = ca;
  }

  /* destroy the addrinfo list */
  जइ(aihead)
    CURL_FREEADDRINFO(aihead);

  /* if we failed, also destroy the Curl_addrinfo list */
  जइ(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = रिक्त;
  }
  अण्णहा जइ(!cafirst) {
#ifdef EAI_NONAME
    /* rfc3493 conformant */
    error = EAI_NONAME;
#अण्णहा
    /* rfc3493 obsoleted */
    error = EAI_NODATA;
#endif
#ifdef USE_WINSOCK
    SET_SOCKERRNO(error);
#endif
  }

  *result = cafirst;

  /* This is not a CURLcode */
  प्रत्यागम error;
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
#जइ !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
संरचना Curl_addrinfo *Curl_he2ai(ध्रुव संरचना hostent *he, पूर्णाङ्क port)
{
  संरचना Curl_addrinfo *ai;
  संरचना Curl_addrinfo *prevai = रिक्त;
  संरचना Curl_addrinfo *firstai = रिक्त;
  संरचना sockaddr_in *addr;
#ifdef USE_IPV6
  संरचना sockaddr_in6 *addr6;
#endif
  CURLcode result = CURLE_OK;
  पूर्णाङ्क i;
  वर्ण *curr;

  जइ(!he)
    /* no input == no output! */
    प्रत्यागम रिक्त;

  DEBUGASSERT(he->h_name && he->h_addr_list);

  प्रति(i = 0; (curr = he->h_addr_list[i]) != रिक्त; i++) {
    size_t ss_size;
    size_t namelen = strlen(he->h_name) + 1; /* include null-terminator */
#ifdef USE_IPV6
    जइ(he->h_addrtype == AF_INET6)
      ss_size = परिमाण(संरचना sockaddr_in6);
    अण्णहा
#endif
      ss_size = परिमाण(संरचना sockaddr_in);

    /* allocate memory to hold the struct, the address and the name */
    ai = curlx_calloc(1, परिमाण(संरचना Curl_addrinfo) + ss_size + namelen);
    जइ(!ai) {
      result = CURLE_OUT_OF_MEMORY;
      भङ्ग;
    }
    /* put the address after the struct */
    ai->ai_addr = (शून्य *)((वर्ण *)ai + परिमाण(संरचना Curl_addrinfo));
    /* then put the name after the address */
    ai->ai_canonname = (वर्ण *)ai->ai_addr + ss_size;
    memcpy(ai->ai_canonname, he->h_name, namelen);

    जइ(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    जइ(prevai)
      /* make the previous entry point to this */
      prevai->ai_next = ai;

    ai->ai_family = he->h_addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_size;

    /* leave the rest of the struct filled with zero */

    विकल्प(ai->ai_family) {
    पक्ष AF_INET:
      addr = (शून्य *)ai->ai_addr; /* storage area for this info */

      memcpy(&addr->sin_addr, curr, परिमाण(संरचना in_addr));
      addr->sin_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr->sin_port = htons((unsigned short)port);
      भङ्ग;

#ifdef USE_IPV6
    पक्ष AF_INET6:
      addr6 = (शून्य *)ai->ai_addr; /* storage area for this info */

      memcpy(&addr6->sin6_addr, curr, परिमाण(संरचना in6_addr));
      addr6->sin6_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr6->sin6_port = htons((unsigned short)port);
      भङ्ग;
#endif
    }

    prevai = ai;
  }

  जइ(result) {
    Curl_freeaddrinfo(firstai);
    firstai = रिक्त;
  }

  प्रत्यागम firstai;
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
स्थिर CURLcode ip2addr(संरचना Curl_addrinfo **addrp, पूर्णाङ्क af,
                        ध्रुव शून्य *inaddr, ध्रुव वर्ण *hostname, पूर्णाङ्क port)
{
  संरचना Curl_addrinfo *ai;
  size_t addrsize;
  size_t namelen;
  संरचना sockaddr_in *addr;
#ifdef USE_IPV6
  संरचना sockaddr_in6 *addr6;
#endif

  DEBUGASSERT(inaddr && hostname);

  namelen = strlen(hostname) + 1;
  *addrp = रिक्त;

  जइ(af == AF_INET)
    addrsize = परिमाण(संरचना sockaddr_in);
#ifdef USE_IPV6
  अण्णहा जइ(af == AF_INET6)
    addrsize = परिमाण(संरचना sockaddr_in6);
#endif
  अण्णहा
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;

  /* allocate memory to hold the struct, the address and the name */
  ai = curlx_calloc(1, परिमाण(संरचना Curl_addrinfo) + addrsize + namelen);
  जइ(!ai)
    प्रत्यागम CURLE_OUT_OF_MEMORY;
  /* put the address after the struct */
  ai->ai_addr = (शून्य *)((वर्ण *)ai + परिमाण(संरचना Curl_addrinfo));
  /* then put the name after the address */
  ai->ai_canonname = (वर्ण *)ai->ai_addr + addrsize;
  memcpy(ai->ai_canonname, hostname, namelen);
  ai->ai_family = af;
  ai->ai_socktype = SOCK_STREAM;
  ai->ai_addrlen = (curl_socklen_t)addrsize;
  /* leave the rest of the struct filled with zero */

  विकल्प(af) {
  पक्ष AF_INET:
    addr = (शून्य *)ai->ai_addr; /* storage area for this info */

    memcpy(&addr->sin_addr, inaddr, परिमाण(संरचना in_addr));
    addr->sin_family = (CURL_SA_FAMILY_T)af;
    addr->sin_port = htons((unsigned short)port);
    भङ्ग;

#ifdef USE_IPV6
  पक्ष AF_INET6:
    addr6 = (शून्य *)ai->ai_addr; /* storage area for this info */

    memcpy(&addr6->sin6_addr, inaddr, परिमाण(संरचना in6_addr));
    addr6->sin6_family = (CURL_SA_FAMILY_T)af;
    addr6->sin6_port = htons((unsigned short)port);
    भङ्ग;
#endif
  }
  *addrp = ai;
  प्रत्यागम CURLE_OK;
}

/*
 * Given an IPv4 or IPv6 dotted string address, this converts it to a proper
 * allocated Curl_addrinfo struct and returns it.
 */
CURLcode Curl_str2addr(ध्रुव वर्ण *dotted, uint16_t port,
                       संरचना Curl_addrinfo **addrp)
{
  संरचना in_addr in;
  जइ(curlx_inet_pton(AF_INET, dotted, &in) > 0)
    /* This is a dotted IP address 123.123.123.123-style */
    प्रत्यागम ip2addr(addrp, AF_INET, &in, dotted, port);
#ifdef USE_IPV6
  {
    संरचना in6_addr in6;
    जइ(curlx_inet_pton(AF_INET6, dotted, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      प्रत्यागम ip2addr(addrp, AF_INET6, &in6, dotted, port);
  }
#endif
  प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT; /* bad input format */
}

bool Curl_is_ipv4addr(ध्रुव वर्ण *address)
{
  संरचना in_addr in;
  प्रत्यागम (curlx_inet_pton(AF_INET, address, &in) > 0);
}

bool Curl_is_ipaddr(ध्रुव वर्ण *address)
{
  जइ(Curl_is_ipv4addr(address))
    प्रत्यागम TRUE;
#ifdef USE_IPV6
  {
    संरचना in6_addr in6;
    जइ(curlx_inet_pton(AF_INET6, address, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      प्रत्यागम TRUE;
  }
#endif
  प्रत्यागम FALSE;
}

bool Curl_looks_like_ipv6(ध्रुव वर्ण *s, size_t len, bool maybe_url_encoded,
                          संरचना Curl_str *host, संरचना Curl_str *zone)
{
  ध्रुव वर्ण *zonep = रिक्त;
  size_t i = 0, hlen = 0, zlen = 0;

  जइ(host)
    memset(host, 0, परिमाण(*host));
  जइ(zone)
    memset(zone, 0, परिमाण(*zone));

  प्रति(i = 0; i < len; ++i, ++hlen) {
    जइ(!s[i] || !(ISXDIGIT(s[i]) || (s[i] == ':') || (s[i] == '.')))
      भङ्ग;
  }

  जइ((i < len) && (s[i] == '%')) { /* address followed by a zone? */
    i += 1;
    जइ(maybe_url_encoded && !strncmp("25", s + i, 2))
      i += 2;
    zonep = s + i;
    प्रति(; i < len; ++i, ++zlen) {
      /* Allow unreserved characters as defined in RFC 3986 */
      जइ(!s[i] || !(ISALPHA(s[i]) || ISXDIGIT(s[i]) || (s[i] == '-') ||
                    (s[i] == '.') || (s[i] == '_') || (s[i] == '~')))
        भङ्ग;
    }
  }

  जइ(i != len)
    प्रत्यागम FALSE; /* invalid chars in zone */
  जइ(host && hlen) {
    host->str = s;
    host->len = hlen;
  }
  जइ(zone && zlen) {
    zone->str = zonep;
    zone->len = zlen;
  }
  प्रत्यागम TRUE;
}

#ifdef USE_UNIX_SOCKETS
/**
 * Given a path to a Unix domain socket, return a newly allocated Curl_addrinfo
 * struct initialized with this path.
 * Returns CURLE_TOO_LARGE when path is too long.
 */
CURLcode Curl_unix2addr(ध्रुव वर्ण *path, bool abstract,
                        संरचना Curl_addrinfo **paddr)
{
  संरचना Curl_addrinfo *ai;
  संरचना sockaddr_un *sa_un;
  size_t path_len;

  *paddr = रिक्त;

  /* sun_path must be able to store the null-terminated path */
  path_len = strlen(path) + 1;
  जइ(path_len > परिमाण(sa_un->sun_path))
    प्रत्यागम CURLE_TOO_LARGE;

  ai = curlx_calloc(1,
                    परिमाण(संरचना Curl_addrinfo) + परिमाण(संरचना sockaddr_un));
  जइ(!ai)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  ai->ai_addr = (शून्य *)((वर्ण *)ai + परिमाण(संरचना Curl_addrinfo));
  sa_un = (शून्य *)ai->ai_addr;
  sa_un->sun_family = AF_UNIX;

  ai->ai_family = AF_UNIX;
  ai->ai_socktype = SOCK_STREAM; /* assume reliable transport for HTTP */
  ai->ai_addrlen = (curl_socklen_t)
    ((offsetof(संरचना sockaddr_un, sun_path) + path_len) & 0x7FFFFFFF);

  /* Abstract Unix domain socket have NULL prefix instead of suffix */
  जइ(abstract)
    memcpy(sa_un->sun_path + 1, path, path_len - 1);
  अण्णहा
    memcpy(sa_un->sun_path, path, path_len); /* copy NUL byte */

  *paddr = ai;
  प्रत्यागम CURLE_OK;
}
#endif

#जइ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
/*
 * curl_dbg_freeaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
शून्य curl_dbg_freeaddrinfo(संरचना addrinfo *freethis,
                           पूर्णाङ्क पङ्क्तिः, ध्रुव वर्ण *source)
{
  curl_dbg_log("ADDR %s:%d freeaddrinfo(%p)\n",
               source, पङ्क्तिः, (शून्य *)freethis);
#ifdef USE_LWIPSOCK
  lwip_freeaddrinfo(freethis);
#नोचेद्यदि defined(USE_FAKE_GETADDRINFO)
  {
    ध्रुव वर्ण *env = getenv("CURL_DNS_SERVER");
    जइ(env)
      r_freeaddrinfo(freethis);
    अण्णहा
      /* !checksrc! disable BANNEDFUNC 1 */
      freeaddrinfo(freethis);
  }
#अण्णहा
  /* !checksrc! disable BANNEDFUNC 1 */
  freeaddrinfo(freethis);
#endif
}
#endif /* CURL_MEMDEBUG && HAVE_FREEADDRINFO */

#जइ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
/*
 * curl_dbg_getaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
पूर्णाङ्क curl_dbg_getaddrinfo(ध्रुव वर्ण *hostname,
                         ध्रुव वर्ण *service,
                         ध्रुव संरचना addrinfo *hints,
                         संरचना addrinfo **result,
                         पूर्णाङ्क पङ्क्तिः, ध्रुव वर्ण *source)
{
#ifdef USE_LWIPSOCK
  पूर्णाङ्क res = lwip_getaddrinfo(hostname, service, hints, result);
#नोचेद्यदि defined(USE_FAKE_GETADDRINFO)
  पूर्णाङ्क res;
  ध्रुव वर्ण *env = getenv("CURL_DNS_SERVER");
  जइ(env)
    res = r_getaddrinfo(hostname, service, hints, result);
  अण्णहा
    /* !checksrc! disable BANNEDFUNC 1 */
    res = getaddrinfo(hostname, service, hints, result);
#अण्णहा
  /* !checksrc! disable BANNEDFUNC 1 */
  पूर्णाङ्क res = getaddrinfo(hostname, service, hints, result);
#endif
  जइ(res == 0)
    /* success */
    curl_dbg_log("ADDR %s:%d getaddrinfo() = %p\n", source, पङ्क्तिः,
                 (शून्य *)*result);
  अण्णहा
    curl_dbg_log("ADDR %s:%d getaddrinfo() failed\n", source, पङ्क्तिः);
  प्रत्यागम res;
}
#endif /* CURL_MEMDEBUG && HAVE_GETADDRINFO */

#जइ defined(HAVE_GETADDRINFO) && defined(USE_RESOLVE_ON_IPS)
/*
 * Works around the sin6_port is always zero bug on iOS 9.3.2 and macOS
 * 10.11.5.
 */
शून्य Curl_addrinfo_set_port(संरचना Curl_addrinfo *addrinfo, पूर्णाङ्क port)
{
  संरचना Curl_addrinfo *ca;
  संरचना sockaddr_in *addr;
#ifdef USE_IPV6
  संरचना sockaddr_in6 *addr6;
#endif
  प्रति(ca = addrinfo; ca; ca = ca->ai_next) {
    विकल्प(ca->ai_family) {
    पक्ष AF_INET:
      addr = (शून्य *)ca->ai_addr; /* storage area for this info */
      addr->sin_port = htons((unsigned short)port);
      भङ्ग;

#ifdef USE_IPV6
    पक्ष AF_INET6:
      addr6 = (शून्य *)ca->ai_addr; /* storage area for this info */
      addr6->sin6_port = htons((unsigned short)port);
      भङ्ग;
#endif
    }
  }
}
#endif
