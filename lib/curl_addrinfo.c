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
#அந்தர்பாவ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#அந்தர்பாவ <netinet/in6.h>
#endif
#ifdef HAVE_NETDB_H
#அந்தர்பாவ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif
#ifdef HAVE_SYS_UN_H
#அந்தர்பாவ <sys/un.h>
#endif

#ifdef __VMS
#அந்தர்பாவ <in.h>
#அந்தர்பாவ <inet.h>
#endif

#அந்தர்பாவ <stddef.h>  /* for offsetof() */

#அந்தர்பாவ "curl_addrinfo.h"
#அந்தர்பாவ "fake_addrinfo.h"
#அந்தர்பாவ "curlx/inet_pton.h"
#அந்தர்பாவ "curlx/strparse.h"

/*
 * Curl_freeaddrinfo()
 *
 * This is used to free a linked list of Curl_addrinfo structs along
 * with all its associated allocated storage. This function should be
 * called once for each successful call to Curl_getaddrinfo_ex() or to
 * any function call which actually allocates a Curl_addrinfo struct.
 */

#எனில் defined(__INTEL_COMPILER) && (__INTEL_COMPILER == 910) &&     \
  defined(__OPTIMIZE__) && defined(__unix__) && defined(__i386__)
  /* workaround icc 9.1 optimizer issue */
#  define vqualifier நிலையற்ற
#இல்லையெனில்
#  define vqualifier
#endif

வெறுமை Curl_freeaddrinfo(கட்டமைப்பு Curl_addrinfo *cahead)
{
  கட்டமைப்பு Curl_addrinfo *vqualifier canext;
  கட்டமைப்பு Curl_addrinfo *ca;

  க்கு(ca = cahead; ca; ca = canext) {
    canext = ca->ai_next;
    curlx_free(ca);
  }
}

கட்டமைப்பு Curl_addrinfo *Curl_addrinfo_get(கட்டமைப்பு Curl_addrinfo *ai,
                                        முழு ai_family,
                                        unsigned முழு n)
{
  unsigned முழு i;
  க்கு(i = 0; ai; ai = ai->ai_next) {
    எனில்(ai->ai_family == ai_family) {
      எனில்(i == n)
        திரும்பு ai;
      ++i;
    }
  }
  திரும்பு வெற்று;
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
முழு Curl_getaddrinfo_ex(மாறிலி எழுத்து *nodename,
                        மாறிலி எழுத்து *servname,
                        மாறிலி கட்டமைப்பு addrinfo *hints,
                        கட்டமைப்பு Curl_addrinfo **result)
{
  மாறிலி கட்டமைப்பு addrinfo *ai;
  கட்டமைப்பு addrinfo *aihead;
  கட்டமைப்பு Curl_addrinfo *cafirst = வெற்று;
  கட்டமைப்பு Curl_addrinfo *calast = வெற்று;
  கட்டமைப்பு Curl_addrinfo *ca;
  size_t ss_size;
  முழு error;

  *result = வெற்று; /* assume failure */

  error = CURL_GETADDRINFO(nodename, servname, hints, &aihead);
  எனில்(error)
    திரும்பு error;

  /* traverse the addrinfo list */

  க்கு(ai = aihead; ai; ai = ai->ai_next) {
    size_t namelen = ai->ai_canonname ? strlen(ai->ai_canonname) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    எனில்(ai->ai_family == AF_INET)
      ss_size = அளவு(கட்டமைப்பு sockaddr_in);
#ifdef USE_IPV6
    இல்லையெனில் எனில்(ai->ai_family == AF_INET6)
      ss_size = அளவு(கட்டமைப்பு sockaddr_in6);
#endif
    இல்லையெனில்
      தொடர்;

    /* ignore elements without required address info */
    எனில்(!ai->ai_addr || !(ai->ai_addrlen > 0))
      தொடர்;

    /* ignore elements with bogus address size */
    எனில்((size_t)ai->ai_addrlen < ss_size)
      தொடர்;

    ca = curlx_malloc(அளவு(கட்டமைப்பு Curl_addrinfo) + ss_size + namelen);
    எனில்(!ca) {
      error = EAI_MEMORY;
      நிறுத்து;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = வெற்று;
    ca->ai_canonname = வெற்று;
    ca->ai_next      = வெற்று;

    ca->ai_addr = (வெறுமை *)((எழுத்து *)ca + அளவு(கட்டமைப்பு Curl_addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    எனில்(namelen) {
      ca->ai_canonname = (வெறுமை *)((எழுத்து *)ca->ai_addr + ss_size);
      memcpy(ca->ai_canonname, ai->ai_canonname, namelen);
    }

    /* if the return list is empty, this becomes the first element */
    எனில்(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    எனில்(calast)
      calast->ai_next = ca;
    calast = ca;
  }

  /* destroy the addrinfo list */
  எனில்(aihead)
    CURL_FREEADDRINFO(aihead);

  /* if we failed, also destroy the Curl_addrinfo list */
  எனில்(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = வெற்று;
  }
  இல்லையெனில் எனில்(!cafirst) {
#ifdef EAI_NONAME
    /* rfc3493 conformant */
    error = EAI_NONAME;
#இல்லையெனில்
    /* rfc3493 obsoleted */
    error = EAI_NODATA;
#endif
#ifdef USE_WINSOCK
    SET_SOCKERRNO(error);
#endif
  }

  *result = cafirst;

  /* This is not a CURLcode */
  திரும்பு error;
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
#எனில் !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
கட்டமைப்பு Curl_addrinfo *Curl_he2ai(மாறிலி கட்டமைப்பு hostent *he, முழு port)
{
  கட்டமைப்பு Curl_addrinfo *ai;
  கட்டமைப்பு Curl_addrinfo *prevai = வெற்று;
  கட்டமைப்பு Curl_addrinfo *firstai = வெற்று;
  கட்டமைப்பு sockaddr_in *addr;
#ifdef USE_IPV6
  கட்டமைப்பு sockaddr_in6 *addr6;
#endif
  CURLcode result = CURLE_OK;
  முழு i;
  எழுத்து *curr;

  எனில்(!he)
    /* no input == no output! */
    திரும்பு வெற்று;

  DEBUGASSERT(he->h_name && he->h_addr_list);

  க்கு(i = 0; (curr = he->h_addr_list[i]) != வெற்று; i++) {
    size_t ss_size;
    size_t namelen = strlen(he->h_name) + 1; /* include null-terminator */
#ifdef USE_IPV6
    எனில்(he->h_addrtype == AF_INET6)
      ss_size = அளவு(கட்டமைப்பு sockaddr_in6);
    இல்லையெனில்
#endif
      ss_size = அளவு(கட்டமைப்பு sockaddr_in);

    /* allocate memory to hold the struct, the address and the name */
    ai = curlx_calloc(1, அளவு(கட்டமைப்பு Curl_addrinfo) + ss_size + namelen);
    எனில்(!ai) {
      result = CURLE_OUT_OF_MEMORY;
      நிறுத்து;
    }
    /* put the address after the struct */
    ai->ai_addr = (வெறுமை *)((எழுத்து *)ai + அளவு(கட்டமைப்பு Curl_addrinfo));
    /* then put the name after the address */
    ai->ai_canonname = (எழுத்து *)ai->ai_addr + ss_size;
    memcpy(ai->ai_canonname, he->h_name, namelen);

    எனில்(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    எனில்(prevai)
      /* make the previous entry point to this */
      prevai->ai_next = ai;

    ai->ai_family = he->h_addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_size;

    /* leave the rest of the struct filled with zero */

    தேர்வு(ai->ai_family) {
    நிலை AF_INET:
      addr = (வெறுமை *)ai->ai_addr; /* storage area for this info */

      memcpy(&addr->sin_addr, curr, அளவு(கட்டமைப்பு in_addr));
      addr->sin_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr->sin_port = htons((unsigned short)port);
      நிறுத்து;

#ifdef USE_IPV6
    நிலை AF_INET6:
      addr6 = (வெறுமை *)ai->ai_addr; /* storage area for this info */

      memcpy(&addr6->sin6_addr, curr, அளவு(கட்டமைப்பு in6_addr));
      addr6->sin6_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr6->sin6_port = htons((unsigned short)port);
      நிறுத்து;
#endif
    }

    prevai = ai;
  }

  எனில்(result) {
    Curl_freeaddrinfo(firstai);
    firstai = வெற்று;
  }

  திரும்பு firstai;
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
நிலையான CURLcode ip2addr(கட்டமைப்பு Curl_addrinfo **addrp, முழு af,
                        மாறிலி வெறுமை *inaddr, மாறிலி எழுத்து *hostname, முழு port)
{
  கட்டமைப்பு Curl_addrinfo *ai;
  size_t addrsize;
  size_t namelen;
  கட்டமைப்பு sockaddr_in *addr;
#ifdef USE_IPV6
  கட்டமைப்பு sockaddr_in6 *addr6;
#endif

  DEBUGASSERT(inaddr && hostname);

  namelen = strlen(hostname) + 1;
  *addrp = வெற்று;

  எனில்(af == AF_INET)
    addrsize = அளவு(கட்டமைப்பு sockaddr_in);
#ifdef USE_IPV6
  இல்லையெனில் எனில்(af == AF_INET6)
    addrsize = அளவு(கட்டமைப்பு sockaddr_in6);
#endif
  இல்லையெனில்
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  /* allocate memory to hold the struct, the address and the name */
  ai = curlx_calloc(1, அளவு(கட்டமைப்பு Curl_addrinfo) + addrsize + namelen);
  எனில்(!ai)
    திரும்பு CURLE_OUT_OF_MEMORY;
  /* put the address after the struct */
  ai->ai_addr = (வெறுமை *)((எழுத்து *)ai + அளவு(கட்டமைப்பு Curl_addrinfo));
  /* then put the name after the address */
  ai->ai_canonname = (எழுத்து *)ai->ai_addr + addrsize;
  memcpy(ai->ai_canonname, hostname, namelen);
  ai->ai_family = af;
  ai->ai_socktype = SOCK_STREAM;
  ai->ai_addrlen = (curl_socklen_t)addrsize;
  /* leave the rest of the struct filled with zero */

  தேர்வு(af) {
  நிலை AF_INET:
    addr = (வெறுமை *)ai->ai_addr; /* storage area for this info */

    memcpy(&addr->sin_addr, inaddr, அளவு(கட்டமைப்பு in_addr));
    addr->sin_family = (CURL_SA_FAMILY_T)af;
    addr->sin_port = htons((unsigned short)port);
    நிறுத்து;

#ifdef USE_IPV6
  நிலை AF_INET6:
    addr6 = (வெறுமை *)ai->ai_addr; /* storage area for this info */

    memcpy(&addr6->sin6_addr, inaddr, அளவு(கட்டமைப்பு in6_addr));
    addr6->sin6_family = (CURL_SA_FAMILY_T)af;
    addr6->sin6_port = htons((unsigned short)port);
    நிறுத்து;
#endif
  }
  *addrp = ai;
  திரும்பு CURLE_OK;
}

/*
 * Given an IPv4 or IPv6 dotted string address, this converts it to a proper
 * allocated Curl_addrinfo struct and returns it.
 */
CURLcode Curl_str2addr(மாறிலி எழுத்து *dotted, uint16_t port,
                       கட்டமைப்பு Curl_addrinfo **addrp)
{
  கட்டமைப்பு in_addr in;
  எனில்(curlx_inet_pton(AF_INET, dotted, &in) > 0)
    /* This is a dotted IP address 123.123.123.123-style */
    திரும்பு ip2addr(addrp, AF_INET, &in, dotted, port);
#ifdef USE_IPV6
  {
    கட்டமைப்பு in6_addr in6;
    எனில்(curlx_inet_pton(AF_INET6, dotted, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      திரும்பு ip2addr(addrp, AF_INET6, &in6, dotted, port);
  }
#endif
  திரும்பு CURLE_BAD_FUNCTION_ARGUMENT; /* bad input format */
}

bool Curl_is_ipv4addr(மாறிலி எழுத்து *address)
{
  கட்டமைப்பு in_addr in;
  திரும்பு (curlx_inet_pton(AF_INET, address, &in) > 0);
}

bool Curl_is_ipaddr(மாறிலி எழுத்து *address)
{
  எனில்(Curl_is_ipv4addr(address))
    திரும்பு TRUE;
#ifdef USE_IPV6
  {
    கட்டமைப்பு in6_addr in6;
    எனில்(curlx_inet_pton(AF_INET6, address, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      திரும்பு TRUE;
  }
#endif
  திரும்பு FALSE;
}

bool Curl_looks_like_ipv6(மாறிலி எழுத்து *s, size_t len, bool maybe_url_encoded,
                          கட்டமைப்பு Curl_str *host, கட்டமைப்பு Curl_str *zone)
{
  மாறிலி எழுத்து *zonep = வெற்று;
  size_t i = 0, hlen = 0, zlen = 0;

  எனில்(host)
    memset(host, 0, அளவு(*host));
  எனில்(zone)
    memset(zone, 0, அளவு(*zone));

  க்கு(i = 0; i < len; ++i, ++hlen) {
    எனில்(!s[i] || !(ISXDIGIT(s[i]) || (s[i] == ':') || (s[i] == '.')))
      நிறுத்து;
  }

  எனில்((i < len) && (s[i] == '%')) { /* address followed by a zone? */
    i += 1;
    எனில்(maybe_url_encoded && !strncmp("25", s + i, 2))
      i += 2;
    zonep = s + i;
    க்கு(; i < len; ++i, ++zlen) {
      /* Allow unreserved characters as defined in RFC 3986 */
      எனில்(!s[i] || !(ISALPHA(s[i]) || ISXDIGIT(s[i]) || (s[i] == '-') ||
                    (s[i] == '.') || (s[i] == '_') || (s[i] == '~')))
        நிறுத்து;
    }
  }

  எனில்(i != len)
    திரும்பு FALSE; /* invalid chars in zone */
  எனில்(host && hlen) {
    host->str = s;
    host->len = hlen;
  }
  எனில்(zone && zlen) {
    zone->str = zonep;
    zone->len = zlen;
  }
  திரும்பு TRUE;
}

#ifdef USE_UNIX_SOCKETS
/**
 * Given a path to a Unix domain socket, return a newly allocated Curl_addrinfo
 * struct initialized with this path.
 * Returns CURLE_TOO_LARGE when path is too long.
 */
CURLcode Curl_unix2addr(மாறிலி எழுத்து *path, bool abstract,
                        கட்டமைப்பு Curl_addrinfo **paddr)
{
  கட்டமைப்பு Curl_addrinfo *ai;
  கட்டமைப்பு sockaddr_un *sa_un;
  size_t path_len;

  *paddr = வெற்று;

  /* sun_path must be able to store the null-terminated path */
  path_len = strlen(path) + 1;
  எனில்(path_len > அளவு(sa_un->sun_path))
    திரும்பு CURLE_TOO_LARGE;

  ai = curlx_calloc(1,
                    அளவு(கட்டமைப்பு Curl_addrinfo) + அளவு(கட்டமைப்பு sockaddr_un));
  எனில்(!ai)
    திரும்பு CURLE_OUT_OF_MEMORY;

  ai->ai_addr = (வெறுமை *)((எழுத்து *)ai + அளவு(கட்டமைப்பு Curl_addrinfo));
  sa_un = (வெறுமை *)ai->ai_addr;
  sa_un->sun_family = AF_UNIX;

  ai->ai_family = AF_UNIX;
  ai->ai_socktype = SOCK_STREAM; /* assume reliable transport for HTTP */
  ai->ai_addrlen = (curl_socklen_t)
    ((offsetof(கட்டமைப்பு sockaddr_un, sun_path) + path_len) & 0x7FFFFFFF);

  /* Abstract Unix domain socket have NULL prefix instead of suffix */
  எனில்(abstract)
    memcpy(sa_un->sun_path + 1, path, path_len - 1);
  இல்லையெனில்
    memcpy(sa_un->sun_path, path, path_len); /* copy NUL byte */

  *paddr = ai;
  திரும்பு CURLE_OK;
}
#endif

#எனில் defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
/*
 * curl_dbg_freeaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
வெறுமை curl_dbg_freeaddrinfo(கட்டமைப்பு addrinfo *freethis,
                           முழு வரி, மாறிலி எழுத்து *source)
{
  curl_dbg_log("ADDR %s:%d freeaddrinfo(%p)\n",
               source, வரி, (வெறுமை *)freethis);
#ifdef USE_LWIPSOCK
  lwip_freeaddrinfo(freethis);
#அல்லதெனில் defined(USE_FAKE_GETADDRINFO)
  {
    மாறிலி எழுத்து *env = getenv("CURL_DNS_SERVER");
    எனில்(env)
      r_freeaddrinfo(freethis);
    இல்லையெனில்
      /* !checksrc! disable BANNEDFUNC 1 */
      freeaddrinfo(freethis);
  }
#இல்லையெனில்
  /* !checksrc! disable BANNEDFUNC 1 */
  freeaddrinfo(freethis);
#endif
}
#endif /* CURL_MEMDEBUG && HAVE_FREEADDRINFO */

#எனில் defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
/*
 * curl_dbg_getaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
முழு curl_dbg_getaddrinfo(மாறிலி எழுத்து *hostname,
                         மாறிலி எழுத்து *service,
                         மாறிலி கட்டமைப்பு addrinfo *hints,
                         கட்டமைப்பு addrinfo **result,
                         முழு வரி, மாறிலி எழுத்து *source)
{
#ifdef USE_LWIPSOCK
  முழு res = lwip_getaddrinfo(hostname, service, hints, result);
#அல்லதெனில் defined(USE_FAKE_GETADDRINFO)
  முழு res;
  மாறிலி எழுத்து *env = getenv("CURL_DNS_SERVER");
  எனில்(env)
    res = r_getaddrinfo(hostname, service, hints, result);
  இல்லையெனில்
    /* !checksrc! disable BANNEDFUNC 1 */
    res = getaddrinfo(hostname, service, hints, result);
#இல்லையெனில்
  /* !checksrc! disable BANNEDFUNC 1 */
  முழு res = getaddrinfo(hostname, service, hints, result);
#endif
  எனில்(res == 0)
    /* success */
    curl_dbg_log("ADDR %s:%d getaddrinfo() = %p\n", source, வரி,
                 (வெறுமை *)*result);
  இல்லையெனில்
    curl_dbg_log("ADDR %s:%d getaddrinfo() failed\n", source, வரி);
  திரும்பு res;
}
#endif /* CURL_MEMDEBUG && HAVE_GETADDRINFO */

#எனில் defined(HAVE_GETADDRINFO) && defined(USE_RESOLVE_ON_IPS)
/*
 * Works around the sin6_port is always zero bug on iOS 9.3.2 and macOS
 * 10.11.5.
 */
வெறுமை Curl_addrinfo_set_port(கட்டமைப்பு Curl_addrinfo *addrinfo, முழு port)
{
  கட்டமைப்பு Curl_addrinfo *ca;
  கட்டமைப்பு sockaddr_in *addr;
#ifdef USE_IPV6
  கட்டமைப்பு sockaddr_in6 *addr6;
#endif
  க்கு(ca = addrinfo; ca; ca = ca->ai_next) {
    தேர்வு(ca->ai_family) {
    நிலை AF_INET:
      addr = (வெறுமை *)ca->ai_addr; /* storage area for this info */
      addr->sin_port = htons((unsigned short)port);
      நிறுத்து;

#ifdef USE_IPV6
    நிலை AF_INET6:
      addr6 = (வெறுமை *)ca->ai_addr; /* storage area for this info */
      addr6->sin6_port = htons((unsigned short)port);
      நிறுத்து;
#endif
    }
  }
}
#endif
