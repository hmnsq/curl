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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#endif
#ifdef HAVE_NETINET_IN6_H
#ಅನ್ತರ್ಭಾವ <netinet/in6.h>
#endif
#ifdef HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#endif
#ifdef HAVE_SYS_UN_H
#ಅನ್ತರ್ಭಾವ <sys/un.h>
#endif

#ifdef __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#endif

#ಅನ್ತರ್ಭಾವ <stddef.h>  /* for offsetof() */

#ಅನ್ತರ್ಭಾವ "curl_addrinfo.h"
#ಅನ್ತರ್ಭಾವ "fake_addrinfo.h"
#ಅನ್ತರ್ಭಾವ "curlx/inet_pton.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

/*
 * Curl_freeaddrinfo()
 *
 * This is used to free a linked list of Curl_addrinfo structs along
 * with all its associated allocated storage. This function should be
 * called once for each successful call to Curl_getaddrinfo_ex() or to
 * any function call which actually allocates a Curl_addrinfo struct.
 */

#ಯದಿ defined(__INTEL_COMPILER) && (__INTEL_COMPILER == 910) &&     \
  defined(__OPTIMIZE__) && defined(__unix__) && defined(__i386__)
  /* workaround icc 9.1 optimizer issue */
#  define vqualifier ಚಞ್ಚಲ
#ಅನ್ಯಥಾ
#  define vqualifier
#endif

ಶೂನ್ಯ Curl_freeaddrinfo(ಸಂರಚನಾ Curl_addrinfo *cahead)
{
  ಸಂರಚನಾ Curl_addrinfo *vqualifier canext;
  ಸಂರಚನಾ Curl_addrinfo *ca;

  ಪ್ರತಿ(ca = cahead; ca; ca = canext) {
    canext = ca->ai_next;
    curlx_free(ca);
  }
}

ಸಂರಚನಾ Curl_addrinfo *Curl_addrinfo_get(ಸಂರಚನಾ Curl_addrinfo *ai,
                                        ಪೂರ್ಣಾಙ್ಕ ai_family,
                                        unsigned ಪೂರ್ಣಾಙ್ಕ n)
{
  unsigned ಪೂರ್ಣಾಙ್ಕ i;
  ಪ್ರತಿ(i = 0; ai; ai = ai->ai_next) {
    ಯದಿ(ai->ai_family == ai_family) {
      ಯದಿ(i == n)
        ಪ್ರತ್ಯಾಗಮ ai;
      ++i;
    }
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
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
ಪೂರ್ಣಾಙ್ಕ Curl_getaddrinfo_ex(ಧ್ರುವ ವರ್ಣ *nodename,
                        ಧ್ರುವ ವರ್ಣ *servname,
                        ಧ್ರುವ ಸಂರಚನಾ addrinfo *hints,
                        ಸಂರಚನಾ Curl_addrinfo **result)
{
  ಧ್ರುವ ಸಂರಚನಾ addrinfo *ai;
  ಸಂರಚನಾ addrinfo *aihead;
  ಸಂರಚನಾ Curl_addrinfo *cafirst = ರಿಕ್ತ;
  ಸಂರಚನಾ Curl_addrinfo *calast = ರಿಕ್ತ;
  ಸಂರಚನಾ Curl_addrinfo *ca;
  size_t ss_size;
  ಪೂರ್ಣಾಙ್ಕ error;

  *result = ರಿಕ್ತ; /* assume failure */

  error = CURL_GETADDRINFO(nodename, servname, hints, &aihead);
  ಯದಿ(error)
    ಪ್ರತ್ಯಾಗಮ error;

  /* traverse the addrinfo list */

  ಪ್ರತಿ(ai = aihead; ai; ai = ai->ai_next) {
    size_t namelen = ai->ai_canonname ? strlen(ai->ai_canonname) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    ಯದಿ(ai->ai_family == AF_INET)
      ss_size = ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_in);
#ifdef USE_IPV6
    ಅನ್ಯಥಾ ಯದಿ(ai->ai_family == AF_INET6)
      ss_size = ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_in6);
#endif
    ಅನ್ಯಥಾ
      ಅನುವರ್ತ;

    /* ignore elements without required address info */
    ಯದಿ(!ai->ai_addr || !(ai->ai_addrlen > 0))
      ಅನುವರ್ತ;

    /* ignore elements with bogus address size */
    ಯದಿ((size_t)ai->ai_addrlen < ss_size)
      ಅನುವರ್ತ;

    ca = curlx_malloc(ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_addrinfo) + ss_size + namelen);
    ಯದಿ(!ca) {
      error = EAI_MEMORY;
      ಭಙ್ಗ;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_size;
    ca->ai_addr      = ರಿಕ್ತ;
    ca->ai_canonname = ರಿಕ್ತ;
    ca->ai_next      = ರಿಕ್ತ;

    ca->ai_addr = (ಶೂನ್ಯ *)((ವರ್ಣ *)ca + ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_addrinfo));
    memcpy(ca->ai_addr, ai->ai_addr, ss_size);

    ಯದಿ(namelen) {
      ca->ai_canonname = (ಶೂನ್ಯ *)((ವರ್ಣ *)ca->ai_addr + ss_size);
      memcpy(ca->ai_canonname, ai->ai_canonname, namelen);
    }

    /* if the return list is empty, this becomes the first element */
    ಯದಿ(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    ಯದಿ(calast)
      calast->ai_next = ca;
    calast = ca;
  }

  /* destroy the addrinfo list */
  ಯದಿ(aihead)
    CURL_FREEADDRINFO(aihead);

  /* if we failed, also destroy the Curl_addrinfo list */
  ಯದಿ(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = ರಿಕ್ತ;
  }
  ಅನ್ಯಥಾ ಯದಿ(!cafirst) {
#ifdef EAI_NONAME
    /* rfc3493 conformant */
    error = EAI_NONAME;
#ಅನ್ಯಥಾ
    /* rfc3493 obsoleted */
    error = EAI_NODATA;
#endif
#ifdef USE_WINSOCK
    SET_SOCKERRNO(error);
#endif
  }

  *result = cafirst;

  /* This is not a CURLcode */
  ಪ್ರತ್ಯಾಗಮ error;
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
#ಯದಿ !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
ಸಂರಚನಾ Curl_addrinfo *Curl_he2ai(ಧ್ರುವ ಸಂರಚನಾ hostent *he, ಪೂರ್ಣಾಙ್ಕ port)
{
  ಸಂರಚನಾ Curl_addrinfo *ai;
  ಸಂರಚನಾ Curl_addrinfo *prevai = ರಿಕ್ತ;
  ಸಂರಚನಾ Curl_addrinfo *firstai = ರಿಕ್ತ;
  ಸಂರಚನಾ sockaddr_in *addr;
#ifdef USE_IPV6
  ಸಂರಚನಾ sockaddr_in6 *addr6;
#endif
  CURLcode result = CURLE_OK;
  ಪೂರ್ಣಾಙ್ಕ i;
  ವರ್ಣ *curr;

  ಯದಿ(!he)
    /* no input == no output! */
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  DEBUGASSERT(he->h_name && he->h_addr_list);

  ಪ್ರತಿ(i = 0; (curr = he->h_addr_list[i]) != ರಿಕ್ತ; i++) {
    size_t ss_size;
    size_t namelen = strlen(he->h_name) + 1; /* include null-terminator */
#ifdef USE_IPV6
    ಯದಿ(he->h_addrtype == AF_INET6)
      ss_size = ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_in6);
    ಅನ್ಯಥಾ
#endif
      ss_size = ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_in);

    /* allocate memory to hold the struct, the address and the name */
    ai = curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_addrinfo) + ss_size + namelen);
    ಯದಿ(!ai) {
      result = CURLE_OUT_OF_MEMORY;
      ಭಙ್ಗ;
    }
    /* put the address after the struct */
    ai->ai_addr = (ಶೂನ್ಯ *)((ವರ್ಣ *)ai + ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_addrinfo));
    /* then put the name after the address */
    ai->ai_canonname = (ವರ್ಣ *)ai->ai_addr + ss_size;
    memcpy(ai->ai_canonname, he->h_name, namelen);

    ಯದಿ(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    ಯದಿ(prevai)
      /* make the previous entry point to this */
      prevai->ai_next = ai;

    ai->ai_family = he->h_addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_size;

    /* leave the rest of the struct filled with zero */

    ವಿಕಲ್ಪ(ai->ai_family) {
    ಪಕ್ಷ AF_INET:
      addr = (ಶೂನ್ಯ *)ai->ai_addr; /* storage area for this info */

      memcpy(&addr->sin_addr, curr, ಪರಿಮಾಣ(ಸಂರಚನಾ in_addr));
      addr->sin_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr->sin_port = htons((unsigned short)port);
      ಭಙ್ಗ;

#ifdef USE_IPV6
    ಪಕ್ಷ AF_INET6:
      addr6 = (ಶೂನ್ಯ *)ai->ai_addr; /* storage area for this info */

      memcpy(&addr6->sin6_addr, curr, ಪರಿಮಾಣ(ಸಂರಚನಾ in6_addr));
      addr6->sin6_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr6->sin6_port = htons((unsigned short)port);
      ಭಙ್ಗ;
#endif
    }

    prevai = ai;
  }

  ಯದಿ(result) {
    Curl_freeaddrinfo(firstai);
    firstai = ರಿಕ್ತ;
  }

  ಪ್ರತ್ಯಾಗಮ firstai;
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
ಸ್ಥಿರ CURLcode ip2addr(ಸಂರಚನಾ Curl_addrinfo **addrp, ಪೂರ್ಣಾಙ್ಕ af,
                        ಧ್ರುವ ಶೂನ್ಯ *inaddr, ಧ್ರುವ ವರ್ಣ *hostname, ಪೂರ್ಣಾಙ್ಕ port)
{
  ಸಂರಚನಾ Curl_addrinfo *ai;
  size_t addrsize;
  size_t namelen;
  ಸಂರಚನಾ sockaddr_in *addr;
#ifdef USE_IPV6
  ಸಂರಚನಾ sockaddr_in6 *addr6;
#endif

  DEBUGASSERT(inaddr && hostname);

  namelen = strlen(hostname) + 1;
  *addrp = ರಿಕ್ತ;

  ಯದಿ(af == AF_INET)
    addrsize = ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_in);
#ifdef USE_IPV6
  ಅನ್ಯಥಾ ಯದಿ(af == AF_INET6)
    addrsize = ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_in6);
#endif
  ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  /* allocate memory to hold the struct, the address and the name */
  ai = curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_addrinfo) + addrsize + namelen);
  ಯದಿ(!ai)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  /* put the address after the struct */
  ai->ai_addr = (ಶೂನ್ಯ *)((ವರ್ಣ *)ai + ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_addrinfo));
  /* then put the name after the address */
  ai->ai_canonname = (ವರ್ಣ *)ai->ai_addr + addrsize;
  memcpy(ai->ai_canonname, hostname, namelen);
  ai->ai_family = af;
  ai->ai_socktype = SOCK_STREAM;
  ai->ai_addrlen = (curl_socklen_t)addrsize;
  /* leave the rest of the struct filled with zero */

  ವಿಕಲ್ಪ(af) {
  ಪಕ್ಷ AF_INET:
    addr = (ಶೂನ್ಯ *)ai->ai_addr; /* storage area for this info */

    memcpy(&addr->sin_addr, inaddr, ಪರಿಮಾಣ(ಸಂರಚನಾ in_addr));
    addr->sin_family = (CURL_SA_FAMILY_T)af;
    addr->sin_port = htons((unsigned short)port);
    ಭಙ್ಗ;

#ifdef USE_IPV6
  ಪಕ್ಷ AF_INET6:
    addr6 = (ಶೂನ್ಯ *)ai->ai_addr; /* storage area for this info */

    memcpy(&addr6->sin6_addr, inaddr, ಪರಿಮಾಣ(ಸಂರಚನಾ in6_addr));
    addr6->sin6_family = (CURL_SA_FAMILY_T)af;
    addr6->sin6_port = htons((unsigned short)port);
    ಭಙ್ಗ;
#endif
  }
  *addrp = ai;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Given an IPv4 or IPv6 dotted string address, this converts it to a proper
 * allocated Curl_addrinfo struct and returns it.
 */
CURLcode Curl_str2addr(ಧ್ರುವ ವರ್ಣ *dotted, uint16_t port,
                       ಸಂರಚನಾ Curl_addrinfo **addrp)
{
  ಸಂರಚನಾ in_addr in;
  ಯದಿ(curlx_inet_pton(AF_INET, dotted, &in) > 0)
    /* This is a dotted IP address 123.123.123.123-style */
    ಪ್ರತ್ಯಾಗಮ ip2addr(addrp, AF_INET, &in, dotted, port);
#ifdef USE_IPV6
  {
    ಸಂರಚನಾ in6_addr in6;
    ಯದಿ(curlx_inet_pton(AF_INET6, dotted, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      ಪ್ರತ್ಯಾಗಮ ip2addr(addrp, AF_INET6, &in6, dotted, port);
  }
#endif
  ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT; /* bad input format */
}

bool Curl_is_ipv4addr(ಧ್ರುವ ವರ್ಣ *address)
{
  ಸಂರಚನಾ in_addr in;
  ಪ್ರತ್ಯಾಗಮ (curlx_inet_pton(AF_INET, address, &in) > 0);
}

bool Curl_is_ipaddr(ಧ್ರುವ ವರ್ಣ *address)
{
  ಯದಿ(Curl_is_ipv4addr(address))
    ಪ್ರತ್ಯಾಗಮ TRUE;
#ifdef USE_IPV6
  {
    ಸಂರಚನಾ in6_addr in6;
    ಯದಿ(curlx_inet_pton(AF_INET6, address, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      ಪ್ರತ್ಯಾಗಮ TRUE;
  }
#endif
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

bool Curl_looks_like_ipv6(ಧ್ರುವ ವರ್ಣ *s, size_t len, bool maybe_url_encoded,
                          ಸಂರಚನಾ Curl_str *host, ಸಂರಚನಾ Curl_str *zone)
{
  ಧ್ರುವ ವರ್ಣ *zonep = ರಿಕ್ತ;
  size_t i = 0, hlen = 0, zlen = 0;

  ಯದಿ(host)
    memset(host, 0, ಪರಿಮಾಣ(*host));
  ಯದಿ(zone)
    memset(zone, 0, ಪರಿಮಾಣ(*zone));

  ಪ್ರತಿ(i = 0; i < len; ++i, ++hlen) {
    ಯದಿ(!s[i] || !(ISXDIGIT(s[i]) || (s[i] == ':') || (s[i] == '.')))
      ಭಙ್ಗ;
  }

  ಯದಿ((i < len) && (s[i] == '%')) { /* address followed by a zone? */
    i += 1;
    ಯದಿ(maybe_url_encoded && !strncmp("25", s + i, 2))
      i += 2;
    zonep = s + i;
    ಪ್ರತಿ(; i < len; ++i, ++zlen) {
      /* Allow unreserved characters as defined in RFC 3986 */
      ಯದಿ(!s[i] || !(ISALPHA(s[i]) || ISXDIGIT(s[i]) || (s[i] == '-') ||
                    (s[i] == '.') || (s[i] == '_') || (s[i] == '~')))
        ಭಙ್ಗ;
    }
  }

  ಯದಿ(i != len)
    ಪ್ರತ್ಯಾಗಮ FALSE; /* invalid chars in zone */
  ಯದಿ(host && hlen) {
    host->str = s;
    host->len = hlen;
  }
  ಯದಿ(zone && zlen) {
    zone->str = zonep;
    zone->len = zlen;
  }
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

#ifdef USE_UNIX_SOCKETS
/**
 * Given a path to a Unix domain socket, return a newly allocated Curl_addrinfo
 * struct initialized with this path.
 * Returns CURLE_TOO_LARGE when path is too long.
 */
CURLcode Curl_unix2addr(ಧ್ರುವ ವರ್ಣ *path, bool abstract,
                        ಸಂರಚನಾ Curl_addrinfo **paddr)
{
  ಸಂರಚನಾ Curl_addrinfo *ai;
  ಸಂರಚನಾ sockaddr_un *sa_un;
  size_t path_len;

  *paddr = ರಿಕ್ತ;

  /* sun_path must be able to store the null-terminated path */
  path_len = strlen(path) + 1;
  ಯದಿ(path_len > ಪರಿಮಾಣ(sa_un->sun_path))
    ಪ್ರತ್ಯಾಗಮ CURLE_TOO_LARGE;

  ai = curlx_calloc(1,
                    ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_addrinfo) + ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_un));
  ಯದಿ(!ai)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  ai->ai_addr = (ಶೂನ್ಯ *)((ವರ್ಣ *)ai + ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_addrinfo));
  sa_un = (ಶೂನ್ಯ *)ai->ai_addr;
  sa_un->sun_family = AF_UNIX;

  ai->ai_family = AF_UNIX;
  ai->ai_socktype = SOCK_STREAM; /* assume reliable transport for HTTP */
  ai->ai_addrlen = (curl_socklen_t)
    ((offsetof(ಸಂರಚನಾ sockaddr_un, sun_path) + path_len) & 0x7FFFFFFF);

  /* Abstract Unix domain socket have NULL prefix instead of suffix */
  ಯದಿ(abstract)
    memcpy(sa_un->sun_path + 1, path, path_len - 1);
  ಅನ್ಯಥಾ
    memcpy(sa_un->sun_path, path, path_len); /* copy NUL byte */

  *paddr = ai;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
#endif

#ಯದಿ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
/*
 * curl_dbg_freeaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
ಶೂನ್ಯ curl_dbg_freeaddrinfo(ಸಂರಚನಾ addrinfo *freethis,
                           ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಧ್ರುವ ವರ್ಣ *source)
{
  curl_dbg_log("ADDR %s:%d freeaddrinfo(%p)\n",
               source, ಸಾಲು, (ಶೂನ್ಯ *)freethis);
#ifdef USE_LWIPSOCK
  lwip_freeaddrinfo(freethis);
#ನೋಚೇದ್ಯದಿ defined(USE_FAKE_GETADDRINFO)
  {
    ಧ್ರುವ ವರ್ಣ *env = getenv("CURL_DNS_SERVER");
    ಯದಿ(env)
      r_freeaddrinfo(freethis);
    ಅನ್ಯಥಾ
      /* !checksrc! disable BANNEDFUNC 1 */
      freeaddrinfo(freethis);
  }
#ಅನ್ಯಥಾ
  /* !checksrc! disable BANNEDFUNC 1 */
  freeaddrinfo(freethis);
#endif
}
#endif /* CURL_MEMDEBUG && HAVE_FREEADDRINFO */

#ಯದಿ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
/*
 * curl_dbg_getaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
ಪೂರ್ಣಾಙ್ಕ curl_dbg_getaddrinfo(ಧ್ರುವ ವರ್ಣ *hostname,
                         ಧ್ರುವ ವರ್ಣ *service,
                         ಧ್ರುವ ಸಂರಚನಾ addrinfo *hints,
                         ಸಂರಚನಾ addrinfo **result,
                         ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಧ್ರುವ ವರ್ಣ *source)
{
#ifdef USE_LWIPSOCK
  ಪೂರ್ಣಾಙ್ಕ res = lwip_getaddrinfo(hostname, service, hints, result);
#ನೋಚೇದ್ಯದಿ defined(USE_FAKE_GETADDRINFO)
  ಪೂರ್ಣಾಙ್ಕ res;
  ಧ್ರುವ ವರ್ಣ *env = getenv("CURL_DNS_SERVER");
  ಯದಿ(env)
    res = r_getaddrinfo(hostname, service, hints, result);
  ಅನ್ಯಥಾ
    /* !checksrc! disable BANNEDFUNC 1 */
    res = getaddrinfo(hostname, service, hints, result);
#ಅನ್ಯಥಾ
  /* !checksrc! disable BANNEDFUNC 1 */
  ಪೂರ್ಣಾಙ್ಕ res = getaddrinfo(hostname, service, hints, result);
#endif
  ಯದಿ(res == 0)
    /* success */
    curl_dbg_log("ADDR %s:%d getaddrinfo() = %p\n", source, ಸಾಲು,
                 (ಶೂನ್ಯ *)*result);
  ಅನ್ಯಥಾ
    curl_dbg_log("ADDR %s:%d getaddrinfo() failed\n", source, ಸಾಲು);
  ಪ್ರತ್ಯಾಗಮ res;
}
#endif /* CURL_MEMDEBUG && HAVE_GETADDRINFO */

#ಯದಿ defined(HAVE_GETADDRINFO) && defined(USE_RESOLVE_ON_IPS)
/*
 * Works around the sin6_port is always zero bug on iOS 9.3.2 and macOS
 * 10.11.5.
 */
ಶೂನ್ಯ Curl_addrinfo_set_port(ಸಂರಚನಾ Curl_addrinfo *addrinfo, ಪೂರ್ಣಾಙ್ಕ port)
{
  ಸಂರಚನಾ Curl_addrinfo *ca;
  ಸಂರಚನಾ sockaddr_in *addr;
#ifdef USE_IPV6
  ಸಂರಚನಾ sockaddr_in6 *addr6;
#endif
  ಪ್ರತಿ(ca = addrinfo; ca; ca = ca->ai_next) {
    ವಿಕಲ್ಪ(ca->ai_family) {
    ಪಕ್ಷ AF_INET:
      addr = (ಶೂನ್ಯ *)ca->ai_addr; /* storage area for this info */
      addr->sin_port = htons((unsigned short)port);
      ಭಙ್ಗ;

#ifdef USE_IPV6
    ಪಕ್ಷ AF_INET6:
      addr6 = (ಶೂನ್ಯ *)ca->ai_addr; /* storage area for this info */
      addr6->sin6_port = htons((unsigned short)port);
      ಭಙ್ಗ;
#endif
    }
  }
}
#endif
