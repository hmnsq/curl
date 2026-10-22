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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_IN6_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in6.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_UN_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/un.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __VMS
#ਅਨ੍ਤਰ੍ਭਾਵ <in.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <inet.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <stddef.h>  /* for offsetof() */

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_addrinfo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fake_addrinfo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/inet_pton.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"

/*
 * Curl_freeaddrinfo()
 *
 * This is used to free a linked list of Curl_addrinfo structs along
 * with all its associated allocated storage. This function should be
 * called once for each successful call to Curl_getaddrinfo_ex() or to
 * any function call which actually allocates a Curl_addrinfo struct.
 */

#ਜੇ defined(__INTEL_COMPILER) && (__INTEL_COMPILER == 910) &&     \
  defined(__OPTIMIZE__) && defined(__unix__) && defined(__i386__)
  /* workaround icc 9.1 optimizer issue */
#  define vqualifier ਅਸਥਿਰ
#ਨਹੀਂਤਾਂ
#  define vqualifier
#ਜੇ_ਅੰਤ

ਖਾਲੀ Curl_freeaddrinfo(ਬਣਤਰ Curl_addrinfo *cahead)
{
  ਬਣਤਰ Curl_addrinfo *vqualifier canext;
  ਬਣਤਰ Curl_addrinfo *ca;

  ਲਈ(ca = cahead; ca; ca = canext) {
    canext = ca->ai_ਅਗਲਾ;
    curlx_free(ca);
  }
}

ਬਣਤਰ Curl_addrinfo *Curl_addrinfo_get(ਬਣਤਰ Curl_addrinfo *ai,
                                        ਪੂਰਨ ai_family,
                                        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ n)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;
  ਲਈ(i = 0; ai; ai = ai->ai_ਅਗਲਾ) {
    ਜੇ(ai->ai_family == ai_family) {
      ਜੇ(i == n)
        ਵਾਪਸ ai;
      ++i;
    }
  }
  ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_GETADDRINFO
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
ਪੂਰਨ Curl_getaddrinfo_ex(ਅਚਲ ਅੱਖਰ *nodename,
                        ਅਚਲ ਅੱਖਰ *servname,
                        ਅਚਲ ਬਣਤਰ addrinfo *hints,
                        ਬਣਤਰ Curl_addrinfo **ਨਤੀਜਾ)
{
  ਅਚਲ ਬਣਤਰ addrinfo *ai;
  ਬਣਤਰ addrinfo *aihead;
  ਬਣਤਰ Curl_addrinfo *cafirst = ਨਿਰਾ;
  ਬਣਤਰ Curl_addrinfo *calast = ਨਿਰਾ;
  ਬਣਤਰ Curl_addrinfo *ca;
  size_t ss_ਮਾਪ;
  ਪੂਰਨ error;

  *ਨਤੀਜਾ = ਨਿਰਾ; /* assume failure */

  error = CURL_GETADDRINFO(nodename, servname, hints, &aihead);
  ਜੇ(error)
    ਵਾਪਸ error;

  /* traverse the addrinfo list */

  ਲਈ(ai = aihead; ai; ai = ai->ai_ਅਗਲਾ) {
    size_t namelen = ai->ai_canonname ? ਲੜੀ_ਲੰਬਾਈ(ai->ai_canonname) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    ਜੇ(ai->ai_family == AF_INET)
      ss_ਮਾਪ = ਆਕਾਰ(ਬਣਤਰ sockaddr_in);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
    ਨਹੀਂਤਾਂ ਜੇ(ai->ai_family == AF_INET6)
      ss_ਮਾਪ = ਆਕਾਰ(ਬਣਤਰ sockaddr_in6);
#ਜੇ_ਅੰਤ
    ਨਹੀਂਤਾਂ
      ਜਾਰੀ;

    /* ignore elements without required address info */
    ਜੇ(!ai->ai_addr || !(ai->ai_addrlen > 0))
      ਜਾਰੀ;

    /* ignore elements with bogus address size */
    ਜੇ((size_t)ai->ai_addrlen < ss_ਮਾਪ)
      ਜਾਰੀ;

    ca = curlx_malloc(ਆਕਾਰ(ਬਣਤਰ Curl_addrinfo) + ss_ਮਾਪ + namelen);
    ਜੇ(!ca) {
      error = EAI_MEMORY;
      ਰੋਕੋ;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_ਮਾਪ;
    ca->ai_addr      = ਨਿਰਾ;
    ca->ai_canonname = ਨਿਰਾ;
    ca->ai_ਅਗਲਾ      = ਨਿਰਾ;

    ca->ai_addr = (ਖਾਲੀ *)((ਅੱਖਰ *)ca + ਆਕਾਰ(ਬਣਤਰ Curl_addrinfo));
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(ca->ai_addr, ai->ai_addr, ss_ਮਾਪ);

    ਜੇ(namelen) {
      ca->ai_canonname = (ਖਾਲੀ *)((ਅੱਖਰ *)ca->ai_addr + ss_ਮਾਪ);
      ਸਮ੍ਰਿਤੀ_ਨਕਲ(ca->ai_canonname, ai->ai_canonname, namelen);
    }

    /* if the return list is empty, this becomes the first element */
    ਜੇ(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    ਜੇ(calast)
      calast->ai_ਅਗਲਾ = ca;
    calast = ca;
  }

  /* destroy the addrinfo list */
  ਜੇ(aihead)
    CURL_FREEADDRINFO(aihead);

  /* if we failed, also destroy the Curl_addrinfo list */
  ਜੇ(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = ਨਿਰਾ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!cafirst) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ EAI_NONAME
    /* rfc3493 conformant */
    error = EAI_NONAME;
#ਨਹੀਂਤਾਂ
    /* rfc3493 obsoleted */
    error = EAI_NODATA;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
    ਸੈੱਟ_SOCKERRNO(error);
#ਜੇ_ਅੰਤ
  }

  *ਨਤੀਜਾ = cafirst;

  /* This is not a CURLcode */
  ਵਾਪਸ error;
}
#ਜੇ_ਅੰਤ /* HAVE_GETADDRINFO */

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
#ਜੇ !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
ਬਣਤਰ Curl_addrinfo *Curl_he2ai(ਅਚਲ ਬਣਤਰ hostent *he, ਪੂਰਨ ਪੋਰਟ)
{
  ਬਣਤਰ Curl_addrinfo *ai;
  ਬਣਤਰ Curl_addrinfo *prevai = ਨਿਰਾ;
  ਬਣਤਰ Curl_addrinfo *firstai = ਨਿਰਾ;
  ਬਣਤਰ sockaddr_in *addr;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਬਣਤਰ sockaddr_in6 *addr6;
#ਜੇ_ਅੰਤ
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਪੂਰਨ i;
  ਅੱਖਰ *curr;

  ਜੇ(!he)
    /* no input == no output! */
    ਵਾਪਸ ਨਿਰਾ;

  DEBUGASSERT(he->h_name && he->h_addr_list);

  ਲਈ(i = 0; (curr = he->h_addr_list[i]) != ਨਿਰਾ; i++) {
    size_t ss_ਮਾਪ;
    size_t namelen = ਲੜੀ_ਲੰਬਾਈ(he->h_name) + 1; /* include null-terminator */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
    ਜੇ(he->h_addrtype == AF_INET6)
      ss_ਮਾਪ = ਆਕਾਰ(ਬਣਤਰ sockaddr_in6);
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
      ss_ਮਾਪ = ਆਕਾਰ(ਬਣਤਰ sockaddr_in);

    /* allocate memory to hold the struct, the address and the name */
    ai = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ Curl_addrinfo) + ss_ਮਾਪ + namelen);
    ਜੇ(!ai) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਰੋਕੋ;
    }
    /* put the address after the struct */
    ai->ai_addr = (ਖਾਲੀ *)((ਅੱਖਰ *)ai + ਆਕਾਰ(ਬਣਤਰ Curl_addrinfo));
    /* then put the name after the address */
    ai->ai_canonname = (ਅੱਖਰ *)ai->ai_addr + ss_ਮਾਪ;
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(ai->ai_canonname, he->h_name, namelen);

    ਜੇ(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    ਜੇ(prevai)
      /* make the previous entry point to this */
      prevai->ai_ਅਗਲਾ = ai;

    ai->ai_family = he->h_addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_ਮਾਪ;

    /* leave the rest of the struct filled with zero */

    ਚੋਣ(ai->ai_family) {
    ਹਾਲਤ AF_INET:
      addr = (ਖਾਲੀ *)ai->ai_addr; /* storage area for this info */

      ਸਮ੍ਰਿਤੀ_ਨਕਲ(&addr->sin_addr, curr, ਆਕਾਰ(ਬਣਤਰ in_addr));
      addr->sin_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr->sin_port = htons((ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)ਪੋਰਟ);
      ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
    ਹਾਲਤ AF_INET6:
      addr6 = (ਖਾਲੀ *)ai->ai_addr; /* storage area for this info */

      ਸਮ੍ਰਿਤੀ_ਨਕਲ(&addr6->sin6_addr, curr, ਆਕਾਰ(ਬਣਤਰ in6_addr));
      addr6->sin6_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr6->sin6_port = htons((ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)ਪੋਰਟ);
      ਰੋਕੋ;
#ਜੇ_ਅੰਤ
    }

    prevai = ai;
  }

  ਜੇ(ਨਤੀਜਾ) {
    Curl_freeaddrinfo(firstai);
    firstai = ਨਿਰਾ;
  }

  ਵਾਪਸ firstai;
}
#ਜੇ_ਅੰਤ

/*
 * ip2addr()
 *
 * This function takes an Internet address, in binary form, as input parameter
 * along with its address family and the string version of the address, and it
 * returns a Curl_addrinfo chain filled in correctly with information for the
 * given address/host
 */
ਸਥਿਰ CURLcode ip2addr(ਬਣਤਰ Curl_addrinfo **addrp, ਪੂਰਨ af,
                        ਅਚਲ ਖਾਲੀ *inaddr, ਅਚਲ ਅੱਖਰ *hostname, ਪੂਰਨ ਪੋਰਟ)
{
  ਬਣਤਰ Curl_addrinfo *ai;
  size_t addrsize;
  size_t namelen;
  ਬਣਤਰ sockaddr_in *addr;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਬਣਤਰ sockaddr_in6 *addr6;
#ਜੇ_ਅੰਤ

  DEBUGASSERT(inaddr && hostname);

  namelen = ਲੜੀ_ਲੰਬਾਈ(hostname) + 1;
  *addrp = ਨਿਰਾ;

  ਜੇ(af == AF_INET)
    addrsize = ਆਕਾਰ(ਬਣਤਰ sockaddr_in);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਨਹੀਂਤਾਂ ਜੇ(af == AF_INET6)
    addrsize = ਆਕਾਰ(ਬਣਤਰ sockaddr_in6);
#ਜੇ_ਅੰਤ
  ਨਹੀਂਤਾਂ
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  /* allocate memory to hold the struct, the address and the name */
  ai = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ Curl_addrinfo) + addrsize + namelen);
  ਜੇ(!ai)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  /* put the address after the struct */
  ai->ai_addr = (ਖਾਲੀ *)((ਅੱਖਰ *)ai + ਆਕਾਰ(ਬਣਤਰ Curl_addrinfo));
  /* then put the name after the address */
  ai->ai_canonname = (ਅੱਖਰ *)ai->ai_addr + addrsize;
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(ai->ai_canonname, hostname, namelen);
  ai->ai_family = af;
  ai->ai_socktype = SOCK_STREAM;
  ai->ai_addrlen = (curl_socklen_t)addrsize;
  /* leave the rest of the struct filled with zero */

  ਚੋਣ(af) {
  ਹਾਲਤ AF_INET:
    addr = (ਖਾਲੀ *)ai->ai_addr; /* storage area for this info */

    ਸਮ੍ਰਿਤੀ_ਨਕਲ(&addr->sin_addr, inaddr, ਆਕਾਰ(ਬਣਤਰ in_addr));
    addr->sin_family = (CURL_SA_FAMILY_T)af;
    addr->sin_port = htons((ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)ਪੋਰਟ);
    ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਹਾਲਤ AF_INET6:
    addr6 = (ਖਾਲੀ *)ai->ai_addr; /* storage area for this info */

    ਸਮ੍ਰਿਤੀ_ਨਕਲ(&addr6->sin6_addr, inaddr, ਆਕਾਰ(ਬਣਤਰ in6_addr));
    addr6->sin6_family = (CURL_SA_FAMILY_T)af;
    addr6->sin6_port = htons((ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)ਪੋਰਟ);
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  }
  *addrp = ai;
  ਵਾਪਸ CURLE_OK;
}

/*
 * Given an IPv4 or IPv6 dotted string address, this converts it to a proper
 * allocated Curl_addrinfo struct and returns it.
 */
CURLcode Curl_str2addr(ਅਚਲ ਅੱਖਰ *dotted, uint16_t ਪੋਰਟ,
                       ਬਣਤਰ Curl_addrinfo **addrp)
{
  ਬਣਤਰ in_addr ਵਿੱਚ;
  ਜੇ(curlx_inet_pton(AF_INET, dotted, &ਵਿੱਚ) > 0)
    /* This is a dotted IP address 123.123.123.123-style */
    ਵਾਪਸ ip2addr(addrp, AF_INET, &ਵਿੱਚ, dotted, ਪੋਰਟ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  {
    ਬਣਤਰ in6_addr in6;
    ਜੇ(curlx_inet_pton(AF_INET6, dotted, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      ਵਾਪਸ ip2addr(addrp, AF_INET6, &in6, dotted, ਪੋਰਟ);
  }
#ਜੇ_ਅੰਤ
  ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ; /* bad input format */
}

bool Curl_is_ipv4addr(ਅਚਲ ਅੱਖਰ *ਪਤਾ)
{
  ਬਣਤਰ in_addr ਵਿੱਚ;
  ਵਾਪਸ (curlx_inet_pton(AF_INET, ਪਤਾ, &ਵਿੱਚ) > 0);
}

bool Curl_is_ipaddr(ਅਚਲ ਅੱਖਰ *ਪਤਾ)
{
  ਜੇ(Curl_is_ipv4addr(ਪਤਾ))
    ਵਾਪਸ TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  {
    ਬਣਤਰ in6_addr in6;
    ਜੇ(curlx_inet_pton(AF_INET6, ਪਤਾ, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      ਵਾਪਸ TRUE;
  }
#ਜੇ_ਅੰਤ
  ਵਾਪਸ FALSE;
}

bool Curl_looks_like_ipv6(ਅਚਲ ਅੱਖਰ *s, size_t len, bool maybe_url_encoded,
                          ਬਣਤਰ Curl_str *host, ਬਣਤਰ Curl_str *zone)
{
  ਅਚਲ ਅੱਖਰ *zonep = ਨਿਰਾ;
  size_t i = 0, hlen = 0, zlen = 0;

  ਜੇ(host)
    memset(host, 0, ਆਕਾਰ(*host));
  ਜੇ(zone)
    memset(zone, 0, ਆਕਾਰ(*zone));

  ਲਈ(i = 0; i < len; ++i, ++hlen) {
    ਜੇ(!s[i] || !(ISXDIGIT(s[i]) || (s[i] == ':') || (s[i] == '.')))
      ਰੋਕੋ;
  }

  ਜੇ((i < len) && (s[i] == '%')) { /* address followed by a zone? */
    i += 1;
    ਜੇ(maybe_url_encoded && !ਲੜੀ_ਤੁਲਨਾ("25", s + i, 2))
      i += 2;
    zonep = s + i;
    ਲਈ(; i < len; ++i, ++zlen) {
      /* Allow unreserved characters as defined in RFC 3986 */
      ਜੇ(!s[i] || !(ISALPHA(s[i]) || ISXDIGIT(s[i]) || (s[i] == '-') ||
                    (s[i] == '.') || (s[i] == '_') || (s[i] == '~')))
        ਰੋਕੋ;
    }
  }

  ਜੇ(i != len)
    ਵਾਪਸ FALSE; /* invalid chars in zone */
  ਜੇ(host && hlen) {
    host->str = s;
    host->len = hlen;
  }
  ਜੇ(zone && zlen) {
    zone->str = zonep;
    zone->len = zlen;
  }
  ਵਾਪਸ TRUE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_UNIX_SOCKETS
/**
 * Given a path to a Unix domain socket, return a newly allocated Curl_addrinfo
 * struct initialized with this path.
 * Returns CURLE_TOO_LARGE when path is too long.
 */
CURLcode Curl_unix2addr(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, bool abstract,
                        ਬਣਤਰ Curl_addrinfo **paddr)
{
  ਬਣਤਰ Curl_addrinfo *ai;
  ਬਣਤਰ sockaddr_un *sa_un;
  size_t ਰਸਤਾ_len;

  *paddr = ਨਿਰਾ;

  /* sun_path must be able to store the null-terminated path */
  ਰਸਤਾ_len = ਲੜੀ_ਲੰਬਾਈ(ਰਸਤਾ) + 1;
  ਜੇ(ਰਸਤਾ_len > ਆਕਾਰ(sa_un->sun_ਰਸਤਾ))
    ਵਾਪਸ CURLE_TOO_LARGE;

  ai = curlx_calloc(1,
                    ਆਕਾਰ(ਬਣਤਰ Curl_addrinfo) + ਆਕਾਰ(ਬਣਤਰ sockaddr_un));
  ਜੇ(!ai)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ai->ai_addr = (ਖਾਲੀ *)((ਅੱਖਰ *)ai + ਆਕਾਰ(ਬਣਤਰ Curl_addrinfo));
  sa_un = (ਖਾਲੀ *)ai->ai_addr;
  sa_un->sun_family = AF_UNIX;

  ai->ai_family = AF_UNIX;
  ai->ai_socktype = SOCK_STREAM; /* assume reliable transport for HTTP */
  ai->ai_addrlen = (curl_socklen_t)
    ((offsetof(ਬਣਤਰ sockaddr_un, sun_ਰਸਤਾ) + ਰਸਤਾ_len) & 0x7FFFFFFF);

  /* Abstract Unix domain socket have NULL prefix instead of suffix */
  ਜੇ(abstract)
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(sa_un->sun_ਰਸਤਾ + 1, ਰਸਤਾ, ਰਸਤਾ_len - 1);
  ਨਹੀਂਤਾਂ
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(sa_un->sun_ਰਸਤਾ, ਰਸਤਾ, ਰਸਤਾ_len); /* copy NUL byte */

  *paddr = ai;
  ਵਾਪਸ CURLE_OK;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
/*
 * curl_dbg_freeaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
ਖਾਲੀ curl_dbg_freeaddrinfo(ਬਣਤਰ addrinfo *freethis,
                           ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ)
{
  curl_dbg_log("ADDR %s:%d freeaddrinfo(%p)\n",
               ਸਰੋਤ, ਸਤਰ, (ਖਾਲੀ *)freethis);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LWIPSOCK
  lwip_freeaddrinfo(freethis);
#ਜਾਂਜੇ defined(USE_FAKE_GETADDRINFO)
  {
    ਅਚਲ ਅੱਖਰ *env = getenv("CURL_DNS_SERVER");
    ਜੇ(env)
      r_freeaddrinfo(freethis);
    ਨਹੀਂਤਾਂ
      /* !checksrc! disable BANNEDFUNC 1 */
      freeaddrinfo(freethis);
  }
#ਨਹੀਂਤਾਂ
  /* !checksrc! disable BANNEDFUNC 1 */
  freeaddrinfo(freethis);
#ਜੇ_ਅੰਤ
}
#ਜੇ_ਅੰਤ /* CURL_MEMDEBUG && HAVE_FREEADDRINFO */

#ਜੇ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
/*
 * curl_dbg_getaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
ਪੂਰਨ curl_dbg_getaddrinfo(ਅਚਲ ਅੱਖਰ *hostname,
                         ਅਚਲ ਅੱਖਰ *service,
                         ਅਚਲ ਬਣਤਰ addrinfo *hints,
                         ਬਣਤਰ addrinfo **ਨਤੀਜਾ,
                         ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LWIPSOCK
  ਪੂਰਨ res = lwip_getaddrinfo(hostname, service, hints, ਨਤੀਜਾ);
#ਜਾਂਜੇ defined(USE_FAKE_GETADDRINFO)
  ਪੂਰਨ res;
  ਅਚਲ ਅੱਖਰ *env = getenv("CURL_DNS_SERVER");
  ਜੇ(env)
    res = r_getaddrinfo(hostname, service, hints, ਨਤੀਜਾ);
  ਨਹੀਂਤਾਂ
    /* !checksrc! disable BANNEDFUNC 1 */
    res = getaddrinfo(hostname, service, hints, ਨਤੀਜਾ);
#ਨਹੀਂਤਾਂ
  /* !checksrc! disable BANNEDFUNC 1 */
  ਪੂਰਨ res = getaddrinfo(hostname, service, hints, ਨਤੀਜਾ);
#ਜੇ_ਅੰਤ
  ਜੇ(res == 0)
    /* success */
    curl_dbg_log("ADDR %s:%d getaddrinfo() = %p\n", ਸਰੋਤ, ਸਤਰ,
                 (ਖਾਲੀ *)*ਨਤੀਜਾ);
  ਨਹੀਂਤਾਂ
    curl_dbg_log("ADDR %s:%d getaddrinfo() failed\n", ਸਰੋਤ, ਸਤਰ);
  ਵਾਪਸ res;
}
#ਜੇ_ਅੰਤ /* CURL_MEMDEBUG && HAVE_GETADDRINFO */

#ਜੇ defined(HAVE_GETADDRINFO) && defined(USE_RESOLVE_ON_IPS)
/*
 * Works around the sin6_port is always zero bug on iOS 9.3.2 and macOS
 * 10.11.5.
 */
ਖਾਲੀ Curl_addrinfo_ਸੈੱਟ_ਪੋਰਟ(ਬਣਤਰ Curl_addrinfo *addrinfo, ਪੂਰਨ ਪੋਰਟ)
{
  ਬਣਤਰ Curl_addrinfo *ca;
  ਬਣਤਰ sockaddr_in *addr;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਬਣਤਰ sockaddr_in6 *addr6;
#ਜੇ_ਅੰਤ
  ਲਈ(ca = addrinfo; ca; ca = ca->ai_ਅਗਲਾ) {
    ਚੋਣ(ca->ai_family) {
    ਹਾਲਤ AF_INET:
      addr = (ਖਾਲੀ *)ca->ai_addr; /* storage area for this info */
      addr->sin_port = htons((ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)ਪੋਰਟ);
      ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
    ਹਾਲਤ AF_INET6:
      addr6 = (ਖਾਲੀ *)ca->ai_addr; /* storage area for this info */
      addr6->sin6_port = htons((ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)ਪੋਰਟ);
      ਰੋਕੋ;
#ਜੇ_ਅੰਤ
    }
  }
}
#ਜੇ_ਅੰਤ
