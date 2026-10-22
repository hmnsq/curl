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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#അന്തര്ഭാവ <netinet/in6.h>
#endif
#ifdef HAVE_NETDB_H
#അന്തര്ഭാവ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif
#ifdef HAVE_SYS_UN_H
#അന്തര്ഭാവ <sys/un.h>
#endif

#ifdef __VMS
#അന്തര്ഭാവ <in.h>
#അന്തര്ഭാവ <inet.h>
#endif

#അന്തര്ഭാവ <stddef.h>  /* for offsetof() */

#അന്തര്ഭാവ "curl_addrinfo.h"
#അന്തര്ഭാവ "fake_addrinfo.h"
#അന്തര്ഭാവ "curlx/inet_pton.h"
#അന്തര്ഭാവ "curlx/strparse.h"

/*
 * Curl_freeaddrinfo()
 *
 * This is used to free a linked list of Curl_addrinfo structs along
 * with all its associated allocated storage. This function should be
 * called once for each successful call to Curl_getaddrinfo_ex() or to
 * any function call which actually allocates a Curl_addrinfo struct.
 */

#എങ്കിൽ defined(__INTEL_COMPILER) && (__INTEL_COMPILER == 910) &&     \
  defined(__OPTIMIZE__) && defined(__unix__) && defined(__i386__)
  /* workaround icc 9.1 optimizer issue */
#  define vqualifier അസ്ഥിര
#അല്ലെങ്കിൽ
#  define vqualifier
#endif

ശൂന്യം Curl_freeaddrinfo(ഘടന Curl_addrinfo *cahead)
{
  ഘടന Curl_addrinfo *vqualifier canext;
  ഘടന Curl_addrinfo *ca;

  വേണ്ടി(ca = cahead; ca; ca = canext) {
    canext = ca->ai_next;
    curlx_free(ca);
  }
}

ഘടന Curl_addrinfo *Curl_addrinfo_get(ഘടന Curl_addrinfo *ai,
                                        പൂർണ്ണസംഖ്യ ai_family,
                                        unsigned പൂർണ്ണസംഖ്യ n)
{
  unsigned പൂർണ്ണസംഖ്യ i;
  വേണ്ടി(i = 0; ai; ai = ai->ai_next) {
    എങ്കിൽ(ai->ai_family == ai_family) {
      എങ്കിൽ(i == n)
        തിരികെ ai;
      ++i;
    }
  }
  തിരികെ ശൂന്യ;
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
പൂർണ്ണസംഖ്യ Curl_getaddrinfo_ex(സ്ഥിരം അക്ഷരം *nodename,
                        സ്ഥിരം അക്ഷരം *servname,
                        സ്ഥിരം ഘടന addrinfo *hints,
                        ഘടന Curl_addrinfo **result)
{
  സ്ഥിരം ഘടന addrinfo *ai;
  ഘടന addrinfo *aihead;
  ഘടന Curl_addrinfo *cafirst = ശൂന്യ;
  ഘടന Curl_addrinfo *calast = ശൂന്യ;
  ഘടന Curl_addrinfo *ca;
  size_t ss_size;
  പൂർണ്ണസംഖ്യ error;

  *result = ശൂന്യ; /* assume failure */

  error = CURL_GETADDRINFO(nodename, servname, hints, &aihead);
  എങ്കിൽ(error)
    തിരികെ error;

  /* traverse the addrinfo list */

  വേണ്ടി(ai = aihead; ai; ai = ai->ai_next) {
    size_t namelen = ai->ai_canonname ? strlen(ai->ai_canonname) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    എങ്കിൽ(ai->ai_family == AF_INET)
      ss_size = വലുപ്പം(ഘടന sockaddr_in);
#ifdef USE_IPV6
    അല്ലെങ്കിൽ എങ്കിൽ(ai->ai_family == AF_INET6)
      ss_size = വലുപ്പം(ഘടന sockaddr_in6);
#endif
    അല്ലെങ്കിൽ
      തുടരുക;

    /* ignore elements without required address info */
    എങ്കിൽ(!ai->ai_addr || !(ai->ai_addrlen > 0))
      തുടരുക;

    /* ignore elements with bogus address size */
    എങ്കിൽ((size_t)ai->ai_addrlen < ss_size)
      തുടരുക;

    ca = curlx_malloc(വലുപ്പം(ഘടന Curl_addrinfo) + ss_size + namelen);
    എങ്കിൽ(!ca) {
      error = EAI_MEMORY;
      നിർത്തുക;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = ശൂന്യ;
    ca->ai_canonname = ശൂന്യ;
    ca->ai_next      = ശൂന്യ;

    ca->ai_addr = (ശൂന്യം *)((അക്ഷരം *)ca + വലുപ്പം(ഘടന Curl_addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    എങ്കിൽ(namelen) {
      ca->ai_canonname = (ശൂന്യം *)((അക്ഷരം *)ca->ai_addr + ss_size);
      memcpy(ca->ai_canonname, ai->ai_canonname, namelen);
    }

    /* if the return list is empty, this becomes the first element */
    എങ്കിൽ(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    എങ്കിൽ(calast)
      calast->ai_next = ca;
    calast = ca;
  }

  /* destroy the addrinfo list */
  എങ്കിൽ(aihead)
    CURL_FREEADDRINFO(aihead);

  /* if we failed, also destroy the Curl_addrinfo list */
  എങ്കിൽ(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = ശൂന്യ;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!cafirst) {
#ifdef EAI_NONAME
    /* rfc3493 conformant */
    error = EAI_NONAME;
#അല്ലെങ്കിൽ
    /* rfc3493 obsoleted */
    error = EAI_NODATA;
#endif
#ifdef USE_WINSOCK
    SET_SOCKERRNO(error);
#endif
  }

  *result = cafirst;

  /* This is not a CURLcode */
  തിരികെ error;
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
#എങ്കിൽ !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
ഘടന Curl_addrinfo *Curl_he2ai(സ്ഥിരം ഘടന hostent *he, പൂർണ്ണസംഖ്യ port)
{
  ഘടന Curl_addrinfo *ai;
  ഘടന Curl_addrinfo *prevai = ശൂന്യ;
  ഘടന Curl_addrinfo *firstai = ശൂന്യ;
  ഘടന sockaddr_in *addr;
#ifdef USE_IPV6
  ഘടന sockaddr_in6 *addr6;
#endif
  CURLcode result = CURLE_OK;
  പൂർണ്ണസംഖ്യ i;
  അക്ഷരം *curr;

  എങ്കിൽ(!he)
    /* no input == no output! */
    തിരികെ ശൂന്യ;

  DEBUGASSERT(he->h_name && he->h_addr_list);

  വേണ്ടി(i = 0; (curr = he->h_addr_list[i]) != ശൂന്യ; i++) {
    size_t ss_size;
    size_t namelen = strlen(he->h_name) + 1; /* include null-terminator */
#ifdef USE_IPV6
    എങ്കിൽ(he->h_addrtype == AF_INET6)
      ss_size = വലുപ്പം(ഘടന sockaddr_in6);
    അല്ലെങ്കിൽ
#endif
      ss_size = വലുപ്പം(ഘടന sockaddr_in);

    /* allocate memory to hold the struct, the address and the name */
    ai = curlx_calloc(1, വലുപ്പം(ഘടന Curl_addrinfo) + ss_size + namelen);
    എങ്കിൽ(!ai) {
      result = CURLE_OUT_OF_MEMORY;
      നിർത്തുക;
    }
    /* put the address after the struct */
    ai->ai_addr = (ശൂന്യം *)((അക്ഷരം *)ai + വലുപ്പം(ഘടന Curl_addrinfo));
    /* then put the name after the address */
    ai->ai_canonname = (അക്ഷരം *)ai->ai_addr + ss_size;
    memcpy(ai->ai_canonname, he->h_name, namelen);

    എങ്കിൽ(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    എങ്കിൽ(prevai)
      /* make the previous entry point to this */
      prevai->ai_next = ai;

    ai->ai_family = he->h_addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_size;

    /* leave the rest of the struct filled with zero */

    തിരഞ്ഞെടുപ്പ്(ai->ai_family) {
    അവസ്ഥ AF_INET:
      addr = (ശൂന്യം *)ai->ai_addr; /* storage area for this info */

      memcpy(&addr->sin_addr, curr, വലുപ്പം(ഘടന in_addr));
      addr->sin_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr->sin_port = htons((unsigned short)port);
      നിർത്തുക;

#ifdef USE_IPV6
    അവസ്ഥ AF_INET6:
      addr6 = (ശൂന്യം *)ai->ai_addr; /* storage area for this info */

      memcpy(&addr6->sin6_addr, curr, വലുപ്പം(ഘടന in6_addr));
      addr6->sin6_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr6->sin6_port = htons((unsigned short)port);
      നിർത്തുക;
#endif
    }

    prevai = ai;
  }

  എങ്കിൽ(result) {
    Curl_freeaddrinfo(firstai);
    firstai = ശൂന്യ;
  }

  തിരികെ firstai;
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
സ്ഥിതം CURLcode ip2addr(ഘടന Curl_addrinfo **addrp, പൂർണ്ണസംഖ്യ af,
                        സ്ഥിരം ശൂന്യം *inaddr, സ്ഥിരം അക്ഷരം *hostname, പൂർണ്ണസംഖ്യ port)
{
  ഘടന Curl_addrinfo *ai;
  size_t addrsize;
  size_t namelen;
  ഘടന sockaddr_in *addr;
#ifdef USE_IPV6
  ഘടന sockaddr_in6 *addr6;
#endif

  DEBUGASSERT(inaddr && hostname);

  namelen = strlen(hostname) + 1;
  *addrp = ശൂന്യ;

  എങ്കിൽ(af == AF_INET)
    addrsize = വലുപ്പം(ഘടന sockaddr_in);
#ifdef USE_IPV6
  അല്ലെങ്കിൽ എങ്കിൽ(af == AF_INET6)
    addrsize = വലുപ്പം(ഘടന sockaddr_in6);
#endif
  അല്ലെങ്കിൽ
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  /* allocate memory to hold the struct, the address and the name */
  ai = curlx_calloc(1, വലുപ്പം(ഘടന Curl_addrinfo) + addrsize + namelen);
  എങ്കിൽ(!ai)
    തിരികെ CURLE_OUT_OF_MEMORY;
  /* put the address after the struct */
  ai->ai_addr = (ശൂന്യം *)((അക്ഷരം *)ai + വലുപ്പം(ഘടന Curl_addrinfo));
  /* then put the name after the address */
  ai->ai_canonname = (അക്ഷരം *)ai->ai_addr + addrsize;
  memcpy(ai->ai_canonname, hostname, namelen);
  ai->ai_family = af;
  ai->ai_socktype = SOCK_STREAM;
  ai->ai_addrlen = (curl_socklen_t)addrsize;
  /* leave the rest of the struct filled with zero */

  തിരഞ്ഞെടുപ്പ്(af) {
  അവസ്ഥ AF_INET:
    addr = (ശൂന്യം *)ai->ai_addr; /* storage area for this info */

    memcpy(&addr->sin_addr, inaddr, വലുപ്പം(ഘടന in_addr));
    addr->sin_family = (CURL_SA_FAMILY_T)af;
    addr->sin_port = htons((unsigned short)port);
    നിർത്തുക;

#ifdef USE_IPV6
  അവസ്ഥ AF_INET6:
    addr6 = (ശൂന്യം *)ai->ai_addr; /* storage area for this info */

    memcpy(&addr6->sin6_addr, inaddr, വലുപ്പം(ഘടന in6_addr));
    addr6->sin6_family = (CURL_SA_FAMILY_T)af;
    addr6->sin6_port = htons((unsigned short)port);
    നിർത്തുക;
#endif
  }
  *addrp = ai;
  തിരികെ CURLE_OK;
}

/*
 * Given an IPv4 or IPv6 dotted string address, this converts it to a proper
 * allocated Curl_addrinfo struct and returns it.
 */
CURLcode Curl_str2addr(സ്ഥിരം അക്ഷരം *dotted, uint16_t port,
                       ഘടന Curl_addrinfo **addrp)
{
  ഘടന in_addr in;
  എങ്കിൽ(curlx_inet_pton(AF_INET, dotted, &in) > 0)
    /* This is a dotted IP address 123.123.123.123-style */
    തിരികെ ip2addr(addrp, AF_INET, &in, dotted, port);
#ifdef USE_IPV6
  {
    ഘടന in6_addr in6;
    എങ്കിൽ(curlx_inet_pton(AF_INET6, dotted, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      തിരികെ ip2addr(addrp, AF_INET6, &in6, dotted, port);
  }
#endif
  തിരികെ CURLE_BAD_FUNCTION_ARGUMENT; /* bad input format */
}

bool Curl_is_ipv4addr(സ്ഥിരം അക്ഷരം *address)
{
  ഘടന in_addr in;
  തിരികെ (curlx_inet_pton(AF_INET, address, &in) > 0);
}

bool Curl_is_ipaddr(സ്ഥിരം അക്ഷരം *address)
{
  എങ്കിൽ(Curl_is_ipv4addr(address))
    തിരികെ TRUE;
#ifdef USE_IPV6
  {
    ഘടന in6_addr in6;
    എങ്കിൽ(curlx_inet_pton(AF_INET6, address, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      തിരികെ TRUE;
  }
#endif
  തിരികെ FALSE;
}

bool Curl_looks_like_ipv6(സ്ഥിരം അക്ഷരം *s, size_t len, bool maybe_url_encoded,
                          ഘടന Curl_str *host, ഘടന Curl_str *zone)
{
  സ്ഥിരം അക്ഷരം *zonep = ശൂന്യ;
  size_t i = 0, hlen = 0, zlen = 0;

  എങ്കിൽ(host)
    memset(host, 0, വലുപ്പം(*host));
  എങ്കിൽ(zone)
    memset(zone, 0, വലുപ്പം(*zone));

  വേണ്ടി(i = 0; i < len; ++i, ++hlen) {
    എങ്കിൽ(!s[i] || !(ISXDIGIT(s[i]) || (s[i] == ':') || (s[i] == '.')))
      നിർത്തുക;
  }

  എങ്കിൽ((i < len) && (s[i] == '%')) { /* address followed by a zone? */
    i += 1;
    എങ്കിൽ(maybe_url_encoded && !strncmp("25", s + i, 2))
      i += 2;
    zonep = s + i;
    വേണ്ടി(; i < len; ++i, ++zlen) {
      /* Allow unreserved characters as defined in RFC 3986 */
      എങ്കിൽ(!s[i] || !(ISALPHA(s[i]) || ISXDIGIT(s[i]) || (s[i] == '-') ||
                    (s[i] == '.') || (s[i] == '_') || (s[i] == '~')))
        നിർത്തുക;
    }
  }

  എങ്കിൽ(i != len)
    തിരികെ FALSE; /* invalid chars in zone */
  എങ്കിൽ(host && hlen) {
    host->str = s;
    host->len = hlen;
  }
  എങ്കിൽ(zone && zlen) {
    zone->str = zonep;
    zone->len = zlen;
  }
  തിരികെ TRUE;
}

#ifdef USE_UNIX_SOCKETS
/**
 * Given a path to a Unix domain socket, return a newly allocated Curl_addrinfo
 * struct initialized with this path.
 * Returns CURLE_TOO_LARGE when path is too long.
 */
CURLcode Curl_unix2addr(സ്ഥിരം അക്ഷരം *path, bool abstract,
                        ഘടന Curl_addrinfo **paddr)
{
  ഘടന Curl_addrinfo *ai;
  ഘടന sockaddr_un *sa_un;
  size_t path_len;

  *paddr = ശൂന്യ;

  /* sun_path must be able to store the null-terminated path */
  path_len = strlen(path) + 1;
  എങ്കിൽ(path_len > വലുപ്പം(sa_un->sun_path))
    തിരികെ CURLE_TOO_LARGE;

  ai = curlx_calloc(1,
                    വലുപ്പം(ഘടന Curl_addrinfo) + വലുപ്പം(ഘടന sockaddr_un));
  എങ്കിൽ(!ai)
    തിരികെ CURLE_OUT_OF_MEMORY;

  ai->ai_addr = (ശൂന്യം *)((അക്ഷരം *)ai + വലുപ്പം(ഘടന Curl_addrinfo));
  sa_un = (ശൂന്യം *)ai->ai_addr;
  sa_un->sun_family = AF_UNIX;

  ai->ai_family = AF_UNIX;
  ai->ai_socktype = SOCK_STREAM; /* assume reliable transport for HTTP */
  ai->ai_addrlen = (curl_socklen_t)
    ((offsetof(ഘടന sockaddr_un, sun_path) + path_len) & 0x7FFFFFFF);

  /* Abstract Unix domain socket have NULL prefix instead of suffix */
  എങ്കിൽ(abstract)
    memcpy(sa_un->sun_path + 1, path, path_len - 1);
  അല്ലെങ്കിൽ
    memcpy(sa_un->sun_path, path, path_len); /* copy NUL byte */

  *paddr = ai;
  തിരികെ CURLE_OK;
}
#endif

#എങ്കിൽ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
/*
 * curl_dbg_freeaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
ശൂന്യം curl_dbg_freeaddrinfo(ഘടന addrinfo *freethis,
                           പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source)
{
  curl_dbg_log("ADDR %s:%d freeaddrinfo(%p)\n",
               source, വരി, (ശൂന്യം *)freethis);
#ifdef USE_LWIPSOCK
  lwip_freeaddrinfo(freethis);
#അല്ലെങ്കിലെങ്കിൽ defined(USE_FAKE_GETADDRINFO)
  {
    സ്ഥിരം അക്ഷരം *env = getenv("CURL_DNS_SERVER");
    എങ്കിൽ(env)
      r_freeaddrinfo(freethis);
    അല്ലെങ്കിൽ
      /* !checksrc! disable BANNEDFUNC 1 */
      freeaddrinfo(freethis);
  }
#അല്ലെങ്കിൽ
  /* !checksrc! disable BANNEDFUNC 1 */
  freeaddrinfo(freethis);
#endif
}
#endif /* CURL_MEMDEBUG && HAVE_FREEADDRINFO */

#എങ്കിൽ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
/*
 * curl_dbg_getaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
പൂർണ്ണസംഖ്യ curl_dbg_getaddrinfo(സ്ഥിരം അക്ഷരം *hostname,
                         സ്ഥിരം അക്ഷരം *service,
                         സ്ഥിരം ഘടന addrinfo *hints,
                         ഘടന addrinfo **result,
                         പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source)
{
#ifdef USE_LWIPSOCK
  പൂർണ്ണസംഖ്യ res = lwip_getaddrinfo(hostname, service, hints, result);
#അല്ലെങ്കിലെങ്കിൽ defined(USE_FAKE_GETADDRINFO)
  പൂർണ്ണസംഖ്യ res;
  സ്ഥിരം അക്ഷരം *env = getenv("CURL_DNS_SERVER");
  എങ്കിൽ(env)
    res = r_getaddrinfo(hostname, service, hints, result);
  അല്ലെങ്കിൽ
    /* !checksrc! disable BANNEDFUNC 1 */
    res = getaddrinfo(hostname, service, hints, result);
#അല്ലെങ്കിൽ
  /* !checksrc! disable BANNEDFUNC 1 */
  പൂർണ്ണസംഖ്യ res = getaddrinfo(hostname, service, hints, result);
#endif
  എങ്കിൽ(res == 0)
    /* success */
    curl_dbg_log("ADDR %s:%d getaddrinfo() = %p\n", source, വരി,
                 (ശൂന്യം *)*result);
  അല്ലെങ്കിൽ
    curl_dbg_log("ADDR %s:%d getaddrinfo() failed\n", source, വരി);
  തിരികെ res;
}
#endif /* CURL_MEMDEBUG && HAVE_GETADDRINFO */

#എങ്കിൽ defined(HAVE_GETADDRINFO) && defined(USE_RESOLVE_ON_IPS)
/*
 * Works around the sin6_port is always zero bug on iOS 9.3.2 and macOS
 * 10.11.5.
 */
ശൂന്യം Curl_addrinfo_set_port(ഘടന Curl_addrinfo *addrinfo, പൂർണ്ണസംഖ്യ port)
{
  ഘടന Curl_addrinfo *ca;
  ഘടന sockaddr_in *addr;
#ifdef USE_IPV6
  ഘടന sockaddr_in6 *addr6;
#endif
  വേണ്ടി(ca = addrinfo; ca; ca = ca->ai_next) {
    തിരഞ്ഞെടുപ്പ്(ca->ai_family) {
    അവസ്ഥ AF_INET:
      addr = (ശൂന്യം *)ca->ai_addr; /* storage area for this info */
      addr->sin_port = htons((unsigned short)port);
      നിർത്തുക;

#ifdef USE_IPV6
    അവസ്ഥ AF_INET6:
      addr6 = (ശൂന്യം *)ca->ai_addr; /* storage area for this info */
      addr6->sin6_port = htons((unsigned short)port);
      നിർത്തുക;
#endif
    }
  }
}
#endif
