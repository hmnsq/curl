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

#जर defined(__INTEL_COMPILER) && (__INTEL_COMPILER == 910) &&     \
  defined(__OPTIMIZE__) && defined(__unix__) && defined(__i386__)
  /* workaround icc 9.1 optimizer issue */
#  define vqualifier अस्थिर
#अन्यथा
#  define vqualifier
#endif

शून्य Curl_freeaddrinfo(संरचना Curl_addrinfo *cahead)
{
  संरचना Curl_addrinfo *vqualifier canext;
  संरचना Curl_addrinfo *ca;

  साठी(ca = cahead; ca; ca = canext) {
    canext = ca->ai_next;
    curlx_free(ca);
  }
}

संरचना Curl_addrinfo *Curl_addrinfo_get(संरचना Curl_addrinfo *ai,
                                        पूर्णांक ai_family,
                                        अचिन्हित पूर्णांक n)
{
  अचिन्हित पूर्णांक i;
  साठी(i = 0; ai; ai = ai->ai_next) {
    जर(ai->ai_family == ai_family) {
      जर(i == n)
        परत ai;
      ++i;
    }
  }
  परत रिक्त;
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
पूर्णांक Curl_getaddrinfo_ex(स्थिरांक अक्षर *nodename,
                        स्थिरांक अक्षर *servname,
                        स्थिरांक संरचना addrinfo *hints,
                        संरचना Curl_addrinfo **result)
{
  स्थिरांक संरचना addrinfo *ai;
  संरचना addrinfo *aihead;
  संरचना Curl_addrinfo *cafirst = रिक्त;
  संरचना Curl_addrinfo *calast = रिक्त;
  संरचना Curl_addrinfo *ca;
  माप_प्रकार ss_size;
  पूर्णांक error;

  *result = रिक्त; /* assume failure */

  error = CURL_GETADDRINFO(nodename, servname, hints, &aihead);
  जर(error)
    परत error;

  /* traverse the addrinfo list */

  साठी(ai = aihead; ai; ai = ai->ai_next) {
    माप_प्रकार namelen = ai->ai_canonname ? म_माप(ai->ai_canonname) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    जर(ai->ai_family == AF_INET)
      ss_size = आकार(संरचना sockaddr_in);
#ifdef USE_IPV6
    अन्यथा जर(ai->ai_family == AF_INET6)
      ss_size = आकार(संरचना sockaddr_in6);
#endif
    अन्यथा
      सुरूठेवा;

    /* ignore elements without required address info */
    जर(!ai->ai_addr || !(ai->ai_addrlen > 0))
      सुरूठेवा;

    /* ignore elements with bogus address size */
    जर((माप_प्रकार)ai->ai_addrlen < ss_size)
      सुरूठेवा;

    ca = curlx_malloc(आकार(संरचना Curl_addrinfo) + ss_size + namelen);
    जर(!ca) {
      error = EAI_MEMORY;
      थांबा;
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

    ca->ai_addr = (शून्य *)((अक्षर *)ca + आकार(संरचना Curl_addrinfo));
    स_नकल(ca->ai_addr, ai->ai_addr, ss_size);

    जर(namelen) {
      ca->ai_canonname = (शून्य *)((अक्षर *)ca->ai_addr + ss_size);
      स_नकल(ca->ai_canonname, ai->ai_canonname, namelen);
    }

    /* if the return list is empty, this becomes the first element */
    जर(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    जर(calast)
      calast->ai_next = ca;
    calast = ca;
  }

  /* destroy the addrinfo list */
  जर(aihead)
    CURL_FREEADDRINFO(aihead);

  /* if we failed, also destroy the Curl_addrinfo list */
  जर(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = रिक्त;
  }
  अन्यथा जर(!cafirst) {
#ifdef EAI_NONAME
    /* rfc3493 conformant */
    error = EAI_NONAME;
#अन्यथा
    /* rfc3493 obsoleted */
    error = EAI_NODATA;
#endif
#ifdef USE_WINSOCK
    SET_SOCKERRNO(error);
#endif
  }

  *result = cafirst;

  /* This is not a CURLcode */
  परत error;
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
#जर !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
संरचना Curl_addrinfo *Curl_he2ai(स्थिरांक संरचना hostent *he, पूर्णांक port)
{
  संरचना Curl_addrinfo *ai;
  संरचना Curl_addrinfo *prevai = रिक्त;
  संरचना Curl_addrinfo *firstai = रिक्त;
  संरचना sockaddr_in *addr;
#ifdef USE_IPV6
  संरचना sockaddr_in6 *addr6;
#endif
  CURLcode result = CURLE_OK;
  पूर्णांक i;
  अक्षर *curr;

  जर(!he)
    /* no input == no output! */
    परत रिक्त;

  DEBUGASSERT(he->h_name && he->h_addr_list);

  साठी(i = 0; (curr = he->h_addr_list[i]) != रिक्त; i++) {
    माप_प्रकार ss_size;
    माप_प्रकार namelen = म_माप(he->h_name) + 1; /* include null-terminator */
#ifdef USE_IPV6
    जर(he->h_addrtype == AF_INET6)
      ss_size = आकार(संरचना sockaddr_in6);
    अन्यथा
#endif
      ss_size = आकार(संरचना sockaddr_in);

    /* allocate memory to hold the struct, the address and the name */
    ai = curlx_calloc(1, आकार(संरचना Curl_addrinfo) + ss_size + namelen);
    जर(!ai) {
      result = CURLE_OUT_OF_MEMORY;
      थांबा;
    }
    /* put the address after the struct */
    ai->ai_addr = (शून्य *)((अक्षर *)ai + आकार(संरचना Curl_addrinfo));
    /* then put the name after the address */
    ai->ai_canonname = (अक्षर *)ai->ai_addr + ss_size;
    स_नकल(ai->ai_canonname, he->h_name, namelen);

    जर(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    जर(prevai)
      /* make the previous entry point to this */
      prevai->ai_next = ai;

    ai->ai_family = he->h_addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_size;

    /* leave the rest of the struct filled with zero */

    निवड(ai->ai_family) {
    स्थिती AF_INET:
      addr = (शून्य *)ai->ai_addr; /* storage area for this info */

      स_नकल(&addr->sin_addr, curr, आकार(संरचना in_addr));
      addr->sin_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr->sin_port = htons((अचिन्हित लघु)port);
      थांबा;

#ifdef USE_IPV6
    स्थिती AF_INET6:
      addr6 = (शून्य *)ai->ai_addr; /* storage area for this info */

      स_नकल(&addr6->sin6_addr, curr, आकार(संरचना in6_addr));
      addr6->sin6_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr6->sin6_port = htons((अचिन्हित लघु)port);
      थांबा;
#endif
    }

    prevai = ai;
  }

  जर(result) {
    Curl_freeaddrinfo(firstai);
    firstai = रिक्त;
  }

  परत firstai;
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
स्थिर CURLcode ip2addr(संरचना Curl_addrinfo **addrp, पूर्णांक af,
                        स्थिरांक शून्य *inaddr, स्थिरांक अक्षर *hostname, पूर्णांक port)
{
  संरचना Curl_addrinfo *ai;
  माप_प्रकार addrsize;
  माप_प्रकार namelen;
  संरचना sockaddr_in *addr;
#ifdef USE_IPV6
  संरचना sockaddr_in6 *addr6;
#endif

  DEBUGASSERT(inaddr && hostname);

  namelen = म_माप(hostname) + 1;
  *addrp = रिक्त;

  जर(af == AF_INET)
    addrsize = आकार(संरचना sockaddr_in);
#ifdef USE_IPV6
  अन्यथा जर(af == AF_INET6)
    addrsize = आकार(संरचना sockaddr_in6);
#endif
  अन्यथा
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  /* allocate memory to hold the struct, the address and the name */
  ai = curlx_calloc(1, आकार(संरचना Curl_addrinfo) + addrsize + namelen);
  जर(!ai)
    परत CURLE_OUT_OF_MEMORY;
  /* put the address after the struct */
  ai->ai_addr = (शून्य *)((अक्षर *)ai + आकार(संरचना Curl_addrinfo));
  /* then put the name after the address */
  ai->ai_canonname = (अक्षर *)ai->ai_addr + addrsize;
  स_नकल(ai->ai_canonname, hostname, namelen);
  ai->ai_family = af;
  ai->ai_socktype = SOCK_STREAM;
  ai->ai_addrlen = (curl_socklen_t)addrsize;
  /* leave the rest of the struct filled with zero */

  निवड(af) {
  स्थिती AF_INET:
    addr = (शून्य *)ai->ai_addr; /* storage area for this info */

    स_नकल(&addr->sin_addr, inaddr, आकार(संरचना in_addr));
    addr->sin_family = (CURL_SA_FAMILY_T)af;
    addr->sin_port = htons((अचिन्हित लघु)port);
    थांबा;

#ifdef USE_IPV6
  स्थिती AF_INET6:
    addr6 = (शून्य *)ai->ai_addr; /* storage area for this info */

    स_नकल(&addr6->sin6_addr, inaddr, आकार(संरचना in6_addr));
    addr6->sin6_family = (CURL_SA_FAMILY_T)af;
    addr6->sin6_port = htons((अचिन्हित लघु)port);
    थांबा;
#endif
  }
  *addrp = ai;
  परत CURLE_OK;
}

/*
 * Given an IPv4 or IPv6 dotted string address, this converts it to a proper
 * allocated Curl_addrinfo struct and returns it.
 */
CURLcode Curl_str2addr(स्थिरांक अक्षर *dotted, uint16_t port,
                       संरचना Curl_addrinfo **addrp)
{
  संरचना in_addr in;
  जर(curlx_inet_pton(AF_INET, dotted, &in) > 0)
    /* This is a dotted IP address 123.123.123.123-style */
    परत ip2addr(addrp, AF_INET, &in, dotted, port);
#ifdef USE_IPV6
  {
    संरचना in6_addr in6;
    जर(curlx_inet_pton(AF_INET6, dotted, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      परत ip2addr(addrp, AF_INET6, &in6, dotted, port);
  }
#endif
  परत CURLE_BAD_FUNCTION_ARGUMENT; /* bad input format */
}

bool Curl_is_ipv4addr(स्थिरांक अक्षर *address)
{
  संरचना in_addr in;
  परत (curlx_inet_pton(AF_INET, address, &in) > 0);
}

bool Curl_is_ipaddr(स्थिरांक अक्षर *address)
{
  जर(Curl_is_ipv4addr(address))
    परत TRUE;
#ifdef USE_IPV6
  {
    संरचना in6_addr in6;
    जर(curlx_inet_pton(AF_INET6, address, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      परत TRUE;
  }
#endif
  परत FALSE;
}

bool Curl_looks_like_ipv6(स्थिरांक अक्षर *s, माप_प्रकार len, bool maybe_url_encoded,
                          संरचना Curl_str *host, संरचना Curl_str *zone)
{
  स्थिरांक अक्षर *zonep = रिक्त;
  माप_प्रकार i = 0, hlen = 0, zlen = 0;

  जर(host)
    स_रखो(host, 0, आकार(*host));
  जर(zone)
    स_रखो(zone, 0, आकार(*zone));

  साठी(i = 0; i < len; ++i, ++hlen) {
    जर(!s[i] || !(ISXDIGIT(s[i]) || (s[i] == ':') || (s[i] == '.')))
      थांबा;
  }

  जर((i < len) && (s[i] == '%')) { /* address followed by a zone? */
    i += 1;
    जर(maybe_url_encoded && !म_भेदन("25", s + i, 2))
      i += 2;
    zonep = s + i;
    साठी(; i < len; ++i, ++zlen) {
      /* Allow unreserved characters as defined in RFC 3986 */
      जर(!s[i] || !(ISALPHA(s[i]) || ISXDIGIT(s[i]) || (s[i] == '-') ||
                    (s[i] == '.') || (s[i] == '_') || (s[i] == '~')))
        थांबा;
    }
  }

  जर(i != len)
    परत FALSE; /* invalid chars in zone */
  जर(host && hlen) {
    host->str = s;
    host->len = hlen;
  }
  जर(zone && zlen) {
    zone->str = zonep;
    zone->len = zlen;
  }
  परत TRUE;
}

#ifdef USE_UNIX_SOCKETS
/**
 * Given a path to a Unix domain socket, return a newly allocated Curl_addrinfo
 * struct initialized with this path.
 * Returns CURLE_TOO_LARGE when path is too long.
 */
CURLcode Curl_unix2addr(स्थिरांक अक्षर *path, bool abstract,
                        संरचना Curl_addrinfo **paddr)
{
  संरचना Curl_addrinfo *ai;
  संरचना sockaddr_un *sa_un;
  माप_प्रकार path_len;

  *paddr = रिक्त;

  /* sun_path must be able to store the null-terminated path */
  path_len = म_माप(path) + 1;
  जर(path_len > आकार(sa_un->sun_path))
    परत CURLE_TOO_LARGE;

  ai = curlx_calloc(1,
                    आकार(संरचना Curl_addrinfo) + आकार(संरचना sockaddr_un));
  जर(!ai)
    परत CURLE_OUT_OF_MEMORY;

  ai->ai_addr = (शून्य *)((अक्षर *)ai + आकार(संरचना Curl_addrinfo));
  sa_un = (शून्य *)ai->ai_addr;
  sa_un->sun_family = AF_UNIX;

  ai->ai_family = AF_UNIX;
  ai->ai_socktype = SOCK_STREAM; /* assume reliable transport for HTTP */
  ai->ai_addrlen = (curl_socklen_t)
    ((दुरत्व(संरचना sockaddr_un, sun_path) + path_len) & 0x7FFFFFFF);

  /* Abstract Unix domain socket have NULL prefix instead of suffix */
  जर(abstract)
    स_नकल(sa_un->sun_path + 1, path, path_len - 1);
  अन्यथा
    स_नकल(sa_un->sun_path, path, path_len); /* copy NUL byte */

  *paddr = ai;
  परत CURLE_OK;
}
#endif

#जर defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
/*
 * curl_dbg_freeaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
शून्य curl_dbg_freeaddrinfo(संरचना addrinfo *freethis,
                           पूर्णांक ओळ, स्थिरांक अक्षर *source)
{
  curl_dbg_log("ADDR %s:%d freeaddrinfo(%p)\n",
               source, ओळ, (शून्य *)freethis);
#ifdef USE_LWIPSOCK
  lwip_freeaddrinfo(freethis);
#किंवाजर defined(USE_FAKE_GETADDRINFO)
  {
    स्थिरांक अक्षर *env = दो_पर्या("CURL_DNS_SERVER");
    जर(env)
      r_freeaddrinfo(freethis);
    अन्यथा
      /* !checksrc! disable BANNEDFUNC 1 */
      freeaddrinfo(freethis);
  }
#अन्यथा
  /* !checksrc! disable BANNEDFUNC 1 */
  freeaddrinfo(freethis);
#endif
}
#endif /* CURL_MEMDEBUG && HAVE_FREEADDRINFO */

#जर defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
/*
 * curl_dbg_getaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
पूर्णांक curl_dbg_getaddrinfo(स्थिरांक अक्षर *hostname,
                         स्थिरांक अक्षर *service,
                         स्थिरांक संरचना addrinfo *hints,
                         संरचना addrinfo **result,
                         पूर्णांक ओळ, स्थिरांक अक्षर *source)
{
#ifdef USE_LWIPSOCK
  पूर्णांक res = lwip_getaddrinfo(hostname, service, hints, result);
#किंवाजर defined(USE_FAKE_GETADDRINFO)
  पूर्णांक res;
  स्थिरांक अक्षर *env = दो_पर्या("CURL_DNS_SERVER");
  जर(env)
    res = r_getaddrinfo(hostname, service, hints, result);
  अन्यथा
    /* !checksrc! disable BANNEDFUNC 1 */
    res = getaddrinfo(hostname, service, hints, result);
#अन्यथा
  /* !checksrc! disable BANNEDFUNC 1 */
  पूर्णांक res = getaddrinfo(hostname, service, hints, result);
#endif
  जर(res == 0)
    /* success */
    curl_dbg_log("ADDR %s:%d getaddrinfo() = %p\n", source, ओळ,
                 (शून्य *)*result);
  अन्यथा
    curl_dbg_log("ADDR %s:%d getaddrinfo() failed\n", source, ओळ);
  परत res;
}
#endif /* CURL_MEMDEBUG && HAVE_GETADDRINFO */

#जर defined(HAVE_GETADDRINFO) && defined(USE_RESOLVE_ON_IPS)
/*
 * Works around the sin6_port is always zero bug on iOS 9.3.2 and macOS
 * 10.11.5.
 */
शून्य Curl_addrinfo_set_port(संरचना Curl_addrinfo *addrinfo, पूर्णांक port)
{
  संरचना Curl_addrinfo *ca;
  संरचना sockaddr_in *addr;
#ifdef USE_IPV6
  संरचना sockaddr_in6 *addr6;
#endif
  साठी(ca = addrinfo; ca; ca = ca->ai_next) {
    निवड(ca->ai_family) {
    स्थिती AF_INET:
      addr = (शून्य *)ca->ai_addr; /* storage area for this info */
      addr->sin_port = htons((अचिन्हित लघु)port);
      थांबा;

#ifdef USE_IPV6
    स्थिती AF_INET6:
      addr6 = (शून्य *)ca->ai_addr; /* storage area for this info */
      addr6->sin6_port = htons((अचिन्हित लघु)port);
      थांबा;
#endif
    }
  }
}
#endif
