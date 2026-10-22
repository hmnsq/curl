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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_IN6_H
#ಅನ್ತರ್ಭಾವ <netinet/in6.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_UN_H
#ಅನ್ತರ್ಭಾವ <sys/un.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#ಆದರೆ_ಅಂತ್ಯ

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

#ಆದರೆ defined(__INTEL_COMPILER) && (__INTEL_COMPILER == 910) &&     \
  defined(__OPTIMIZE__) && defined(__unix__) && defined(__i386__)
  /* workaround icc 9.1 optimizer issue */
#  define vqualifier ಅಸ್ಥಿರ
#ಇಲ್ಲದಿದ್ದರೆ
#  define vqualifier
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ Curl_freeaddrinfo(ರಚನೆ Curl_addrinfo *cahead)
{
  ರಚನೆ Curl_addrinfo *vqualifier canext;
  ರಚನೆ Curl_addrinfo *ca;

  ಗಾಗಿ(ca = cahead; ca; ca = canext) {
    canext = ca->ai_ಮುಂದಿನ;
    curlx_free(ca);
  }
}

ರಚನೆ Curl_addrinfo *Curl_addrinfo_get(ರಚನೆ Curl_addrinfo *ai,
                                        ಪೂರ್ಣಾಂಕ ai_family,
                                        ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ n)
{
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;
  ಗಾಗಿ(i = 0; ai; ai = ai->ai_ಮುಂದಿನ) {
    ಆದರೆ(ai->ai_family == ai_family) {
      ಆದರೆ(i == n)
        ಹಿಂತಿರುಗಿ ai;
      ++i;
    }
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_GETADDRINFO
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
ಪೂರ್ಣಾಂಕ Curl_getaddrinfo_ex(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *nodename,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *servname,
                        ಸ್ಥಿರಾಂಕ ರಚನೆ addrinfo *hints,
                        ರಚನೆ Curl_addrinfo **ಫಲಿತಾಂಶ)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ addrinfo *ai;
  ರಚನೆ addrinfo *aihead;
  ರಚನೆ Curl_addrinfo *cafirst = ಖಾಲಿ;
  ರಚನೆ Curl_addrinfo *calast = ಖಾಲಿ;
  ರಚನೆ Curl_addrinfo *ca;
  size_t ss_ಗಾತ್ರ;
  ಪೂರ್ಣಾಂಕ error;

  *ಫಲಿತಾಂಶ = ಖಾಲಿ; /* assume failure */

  error = CURL_GETADDRINFO(nodename, servname, hints, &aihead);
  ಆದರೆ(error)
    ಹಿಂತಿರುಗಿ error;

  /* traverse the addrinfo list */

  ಗಾಗಿ(ai = aihead; ai; ai = ai->ai_ಮುಂದಿನ) {
    size_t namelen = ai->ai_canonname ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ai->ai_canonname) + 1 : 0;
    /* ignore elements with unsupported address family,
       settle family-specific sockaddr structure size. */
    ಆದರೆ(ai->ai_family == AF_INET)
      ss_ಗಾತ್ರ = ಗಾತ್ರ(ರಚನೆ sockaddr_in);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ai->ai_family == AF_INET6)
      ss_ಗಾತ್ರ = ಗಾತ್ರ(ರಚನೆ sockaddr_in6);
#ಆದರೆ_ಅಂತ್ಯ
    ಇಲ್ಲದಿದ್ದರೆ
      ಮುಂದುವರಿ;

    /* ignore elements without required address info */
    ಆದರೆ(!ai->ai_addr || !(ai->ai_addrlen > 0))
      ಮುಂದುವರಿ;

    /* ignore elements with bogus address size */
    ಆದರೆ((size_t)ai->ai_addrlen < ss_ಗಾತ್ರ)
      ಮುಂದುವರಿ;

    ca = curlx_malloc(ಗಾತ್ರ(ರಚನೆ Curl_addrinfo) + ss_ಗಾತ್ರ + namelen);
    ಆದರೆ(!ca) {
      error = EAI_MEMORY;
      ನಿಲ್ಲಿಸು;
    }

    /* copy each structure member individually, member ordering,
       size, or padding might be different for each platform. */

    ca->ai_flags     = ai->ai_flags;
    ca->ai_family    = ai->ai_family;
    ca->ai_socktype  = ai->ai_socktype;
    ca->ai_protocol  = ai->ai_protocol;
    ca->ai_addrlen   = (curl_socklen_t)ss_ಗಾತ್ರ;
    ca->ai_addr      = ಖಾಲಿ;
    ca->ai_canonname = ಖಾಲಿ;
    ca->ai_ಮುಂದಿನ      = ಖಾಲಿ;

    ca->ai_addr = (ಶೂನ್ಯ *)((ಅಕ್ಷರ *)ca + ಗಾತ್ರ(ರಚನೆ Curl_addrinfo));
    ಸ್ಮರಣೆ_ನಕಲು(ca->ai_addr, ai->ai_addr, ss_ಗಾತ್ರ);

    ಆದರೆ(namelen) {
      ca->ai_canonname = (ಶೂನ್ಯ *)((ಅಕ್ಷರ *)ca->ai_addr + ss_ಗಾತ್ರ);
      ಸ್ಮರಣೆ_ನಕಲು(ca->ai_canonname, ai->ai_canonname, namelen);
    }

    /* if the return list is empty, this becomes the first element */
    ಆದರೆ(!cafirst)
      cafirst = ca;

    /* add this element last in the return list */
    ಆದರೆ(calast)
      calast->ai_ಮುಂದಿನ = ca;
    calast = ca;
  }

  /* destroy the addrinfo list */
  ಆದರೆ(aihead)
    CURL_FREEADDRINFO(aihead);

  /* if we failed, also destroy the Curl_addrinfo list */
  ಆದರೆ(error) {
    Curl_freeaddrinfo(cafirst);
    cafirst = ಖಾಲಿ;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!cafirst) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ EAI_NONAME
    /* rfc3493 conformant */
    error = EAI_NONAME;
#ಇಲ್ಲದಿದ್ದರೆ
    /* rfc3493 obsoleted */
    error = EAI_NODATA;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINSOCK
    ಗಣ_SOCKERRNO(error);
#ಆದರೆ_ಅಂತ್ಯ
  }

  *ಫಲಿತಾಂಶ = cafirst;

  /* This is not a CURLcode */
  ಹಿಂತಿರುಗಿ error;
}
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_GETADDRINFO */

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
#ಆದರೆ !(defined(HAVE_GETADDRINFO) && defined(HAVE_GETADDRINFO_THREADSAFE))
ರಚನೆ Curl_addrinfo *Curl_he2ai(ಸ್ಥಿರಾಂಕ ರಚನೆ hostent *he, ಪೂರ್ಣಾಂಕ ಪೋರ್ಟ್)
{
  ರಚನೆ Curl_addrinfo *ai;
  ರಚನೆ Curl_addrinfo *prevai = ಖಾಲಿ;
  ರಚನೆ Curl_addrinfo *firstai = ಖಾಲಿ;
  ರಚನೆ sockaddr_in *addr;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ರಚನೆ sockaddr_in6 *addr6;
#ಆದರೆ_ಅಂತ್ಯ
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಪೂರ್ಣಾಂಕ i;
  ಅಕ್ಷರ *curr;

  ಆದರೆ(!he)
    /* no input == no output! */
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  DEBUGASSERT(he->h_name && he->h_addr_list);

  ಗಾಗಿ(i = 0; (curr = he->h_addr_list[i]) != ಖಾಲಿ; i++) {
    size_t ss_ಗಾತ್ರ;
    size_t namelen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(he->h_name) + 1; /* include null-terminator */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
    ಆದರೆ(he->h_addrtype == AF_INET6)
      ss_ಗಾತ್ರ = ಗಾತ್ರ(ರಚನೆ sockaddr_in6);
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
      ss_ಗಾತ್ರ = ಗಾತ್ರ(ರಚನೆ sockaddr_in);

    /* allocate memory to hold the struct, the address and the name */
    ai = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ Curl_addrinfo) + ss_ಗಾತ್ರ + namelen);
    ಆದರೆ(!ai) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ನಿಲ್ಲಿಸು;
    }
    /* put the address after the struct */
    ai->ai_addr = (ಶೂನ್ಯ *)((ಅಕ್ಷರ *)ai + ಗಾತ್ರ(ರಚನೆ Curl_addrinfo));
    /* then put the name after the address */
    ai->ai_canonname = (ಅಕ್ಷರ *)ai->ai_addr + ss_ಗಾತ್ರ;
    ಸ್ಮರಣೆ_ನಕಲು(ai->ai_canonname, he->h_name, namelen);

    ಆದರೆ(!firstai)
      /* store the pointer we want to return from this function */
      firstai = ai;

    ಆದರೆ(prevai)
      /* make the previous entry point to this */
      prevai->ai_ಮುಂದಿನ = ai;

    ai->ai_family = he->h_addrtype;

    /* we return all names as STREAM, so when using this address for TFTP
       the type must be ignored and conn->socktype be used instead! */
    ai->ai_socktype = SOCK_STREAM;

    ai->ai_addrlen = (curl_socklen_t)ss_ಗಾತ್ರ;

    /* leave the rest of the struct filled with zero */

    ಆಯ್ಕೆ(ai->ai_family) {
    ಸ್ಥಿತಿ AF_INET:
      addr = (ಶೂನ್ಯ *)ai->ai_addr; /* storage area for this info */

      ಸ್ಮರಣೆ_ನಕಲು(&addr->sin_addr, curr, ಗಾತ್ರ(ರಚನೆ in_addr));
      addr->sin_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr->sin_port = htons((ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)ಪೋರ್ಟ್);
      ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
    ಸ್ಥಿತಿ AF_INET6:
      addr6 = (ಶೂನ್ಯ *)ai->ai_addr; /* storage area for this info */

      ಸ್ಮರಣೆ_ನಕಲು(&addr6->sin6_addr, curr, ಗಾತ್ರ(ರಚನೆ in6_addr));
      addr6->sin6_family = (CURL_SA_FAMILY_T)he->h_addrtype;
      addr6->sin6_port = htons((ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)ಪೋರ್ಟ್);
      ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
    }

    prevai = ai;
  }

  ಆದರೆ(ಫಲಿತಾಂಶ) {
    Curl_freeaddrinfo(firstai);
    firstai = ಖಾಲಿ;
  }

  ಹಿಂತಿರುಗಿ firstai;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * ip2addr()
 *
 * This function takes an Internet address, in binary form, as input parameter
 * along with its address family and the string version of the address, and it
 * returns a Curl_addrinfo chain filled in correctly with information for the
 * given address/host
 */
ಸ್ಥಿರ CURLcode ip2addr(ರಚನೆ Curl_addrinfo **addrp, ಪೂರ್ಣಾಂಕ af,
                        ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *inaddr, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname, ಪೂರ್ಣಾಂಕ ಪೋರ್ಟ್)
{
  ರಚನೆ Curl_addrinfo *ai;
  size_t addrsize;
  size_t namelen;
  ರಚನೆ sockaddr_in *addr;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ರಚನೆ sockaddr_in6 *addr6;
#ಆದರೆ_ಅಂತ್ಯ

  DEBUGASSERT(inaddr && hostname);

  namelen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(hostname) + 1;
  *addrp = ಖಾಲಿ;

  ಆದರೆ(af == AF_INET)
    addrsize = ಗಾತ್ರ(ರಚನೆ sockaddr_in);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(af == AF_INET6)
    addrsize = ಗಾತ್ರ(ರಚನೆ sockaddr_in6);
#ಆದರೆ_ಅಂತ್ಯ
  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  /* allocate memory to hold the struct, the address and the name */
  ai = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ Curl_addrinfo) + addrsize + namelen);
  ಆದರೆ(!ai)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  /* put the address after the struct */
  ai->ai_addr = (ಶೂನ್ಯ *)((ಅಕ್ಷರ *)ai + ಗಾತ್ರ(ರಚನೆ Curl_addrinfo));
  /* then put the name after the address */
  ai->ai_canonname = (ಅಕ್ಷರ *)ai->ai_addr + addrsize;
  ಸ್ಮರಣೆ_ನಕಲು(ai->ai_canonname, hostname, namelen);
  ai->ai_family = af;
  ai->ai_socktype = SOCK_STREAM;
  ai->ai_addrlen = (curl_socklen_t)addrsize;
  /* leave the rest of the struct filled with zero */

  ಆಯ್ಕೆ(af) {
  ಸ್ಥಿತಿ AF_INET:
    addr = (ಶೂನ್ಯ *)ai->ai_addr; /* storage area for this info */

    ಸ್ಮರಣೆ_ನಕಲು(&addr->sin_addr, inaddr, ಗಾತ್ರ(ರಚನೆ in_addr));
    addr->sin_family = (CURL_SA_FAMILY_T)af;
    addr->sin_port = htons((ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)ಪೋರ್ಟ್);
    ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ಸ್ಥಿತಿ AF_INET6:
    addr6 = (ಶೂನ್ಯ *)ai->ai_addr; /* storage area for this info */

    ಸ್ಮರಣೆ_ನಕಲು(&addr6->sin6_addr, inaddr, ಗಾತ್ರ(ರಚನೆ in6_addr));
    addr6->sin6_family = (CURL_SA_FAMILY_T)af;
    addr6->sin6_port = htons((ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)ಪೋರ್ಟ್);
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  }
  *addrp = ai;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Given an IPv4 or IPv6 dotted string address, this converts it to a proper
 * allocated Curl_addrinfo struct and returns it.
 */
CURLcode Curl_str2addr(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dotted, uint16_t ಪೋರ್ಟ್,
                       ರಚನೆ Curl_addrinfo **addrp)
{
  ರಚನೆ in_addr ಒಳ;
  ಆದರೆ(curlx_inet_pton(AF_INET, dotted, &ಒಳ) > 0)
    /* This is a dotted IP address 123.123.123.123-style */
    ಹಿಂತಿರುಗಿ ip2addr(addrp, AF_INET, &ಒಳ, dotted, ಪೋರ್ಟ್);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  {
    ರಚನೆ in6_addr in6;
    ಆದರೆ(curlx_inet_pton(AF_INET6, dotted, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      ಹಿಂತಿರುಗಿ ip2addr(addrp, AF_INET6, &in6, dotted, ಪೋರ್ಟ್);
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ; /* bad input format */
}

bool Curl_is_ipv4addr(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ವಿಳಾಸ)
{
  ರಚನೆ in_addr ಒಳ;
  ಹಿಂತಿರುಗಿ (curlx_inet_pton(AF_INET, ವಿಳಾಸ, &ಒಳ) > 0);
}

bool Curl_is_ipaddr(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ವಿಳಾಸ)
{
  ಆದರೆ(Curl_is_ipv4addr(ವಿಳಾಸ))
    ಹಿಂತಿರುಗಿ TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  {
    ರಚನೆ in6_addr in6;
    ಆದರೆ(curlx_inet_pton(AF_INET6, ವಿಳಾಸ, &in6) > 0)
      /* This is a dotted IPv6 address ::1-style */
      ಹಿಂತಿರುಗಿ TRUE;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ FALSE;
}

bool Curl_looks_like_ipv6(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t len, bool maybe_url_encoded,
                          ರಚನೆ Curl_str *host, ರಚನೆ Curl_str *zone)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *zonep = ಖಾಲಿ;
  size_t i = 0, hlen = 0, zlen = 0;

  ಆದರೆ(host)
    memset(host, 0, ಗಾತ್ರ(*host));
  ಆದರೆ(zone)
    memset(zone, 0, ಗಾತ್ರ(*zone));

  ಗಾಗಿ(i = 0; i < len; ++i, ++hlen) {
    ಆದರೆ(!s[i] || !(ISXDIGIT(s[i]) || (s[i] == ':') || (s[i] == '.')))
      ನಿಲ್ಲಿಸು;
  }

  ಆದರೆ((i < len) && (s[i] == '%')) { /* address followed by a zone? */
    i += 1;
    ಆದರೆ(maybe_url_encoded && !ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ("25", s + i, 2))
      i += 2;
    zonep = s + i;
    ಗಾಗಿ(; i < len; ++i, ++zlen) {
      /* Allow unreserved characters as defined in RFC 3986 */
      ಆದರೆ(!s[i] || !(ISALPHA(s[i]) || ISXDIGIT(s[i]) || (s[i] == '-') ||
                    (s[i] == '.') || (s[i] == '_') || (s[i] == '~')))
        ನಿಲ್ಲಿಸು;
    }
  }

  ಆದರೆ(i != len)
    ಹಿಂತಿರುಗಿ FALSE; /* invalid chars in zone */
  ಆದರೆ(host && hlen) {
    host->str = s;
    host->len = hlen;
  }
  ಆದರೆ(zone && zlen) {
    zone->str = zonep;
    zone->len = zlen;
  }
  ಹಿಂತಿರುಗಿ TRUE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_UNIX_SOCKETS
/**
 * Given a path to a Unix domain socket, return a newly allocated Curl_addrinfo
 * struct initialized with this path.
 * Returns CURLE_TOO_LARGE when path is too long.
 */
CURLcode Curl_unix2addr(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, bool abstract,
                        ರಚನೆ Curl_addrinfo **paddr)
{
  ರಚನೆ Curl_addrinfo *ai;
  ರಚನೆ sockaddr_un *sa_un;
  size_t ಮಾರ್ಗ_len;

  *paddr = ಖಾಲಿ;

  /* sun_path must be able to store the null-terminated path */
  ಮಾರ್ಗ_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮಾರ್ಗ) + 1;
  ಆದರೆ(ಮಾರ್ಗ_len > ಗಾತ್ರ(sa_un->sun_ಮಾರ್ಗ))
    ಹಿಂತಿರುಗಿ CURLE_TOO_LARGE;

  ai = curlx_calloc(1,
                    ಗಾತ್ರ(ರಚನೆ Curl_addrinfo) + ಗಾತ್ರ(ರಚನೆ sockaddr_un));
  ಆದರೆ(!ai)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ai->ai_addr = (ಶೂನ್ಯ *)((ಅಕ್ಷರ *)ai + ಗಾತ್ರ(ರಚನೆ Curl_addrinfo));
  sa_un = (ಶೂನ್ಯ *)ai->ai_addr;
  sa_un->sun_family = AF_UNIX;

  ai->ai_family = AF_UNIX;
  ai->ai_socktype = SOCK_STREAM; /* assume reliable transport for HTTP */
  ai->ai_addrlen = (curl_socklen_t)
    ((offsetof(ರಚನೆ sockaddr_un, sun_ಮಾರ್ಗ) + ಮಾರ್ಗ_len) & 0x7FFFFFFF);

  /* Abstract Unix domain socket have NULL prefix instead of suffix */
  ಆದರೆ(abstract)
    ಸ್ಮರಣೆ_ನಕಲು(sa_un->sun_ಮಾರ್ಗ + 1, ಮಾರ್ಗ, ಮಾರ್ಗ_len - 1);
  ಇಲ್ಲದಿದ್ದರೆ
    ಸ್ಮರಣೆ_ನಕಲು(sa_un->sun_ಮಾರ್ಗ, ಮಾರ್ಗ, ಮಾರ್ಗ_len); /* copy NUL byte */

  *paddr = ai;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO) && \
  defined(HAVE_FREEADDRINFO)
/*
 * curl_dbg_freeaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
ಶೂನ್ಯ curl_dbg_freeaddrinfo(ರಚನೆ addrinfo *freethis,
                           ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ)
{
  curl_dbg_log("ADDR %s:%d freeaddrinfo(%p)\n",
               ಮೂಲ, ಸಾಲು, (ಶೂನ್ಯ *)freethis);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LWIPSOCK
  lwip_freeaddrinfo(freethis);
#ಅಥವಾಆದರೆ defined(USE_FAKE_GETADDRINFO)
  {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *env = getenv("CURL_DNS_SERVER");
    ಆದರೆ(env)
      r_freeaddrinfo(freethis);
    ಇಲ್ಲದಿದ್ದರೆ
      /* !checksrc! disable BANNEDFUNC 1 */
      freeaddrinfo(freethis);
  }
#ಇಲ್ಲದಿದ್ದರೆ
  /* !checksrc! disable BANNEDFUNC 1 */
  freeaddrinfo(freethis);
#ಆದರೆ_ಅಂತ್ಯ
}
#ಆದರೆ_ಅಂತ್ಯ /* CURL_MEMDEBUG && HAVE_FREEADDRINFO */

#ಆದರೆ defined(CURL_MEMDEBUG) && defined(HAVE_GETADDRINFO)
/*
 * curl_dbg_getaddrinfo()
 *
 * This is strictly for memory tracing and are using the same style as the
 * family otherwise present in memdebug.c. I put these ones here since they
 * require a bunch of structs I did not want to include in memdebug.c
 */
ಪೂರ್ಣಾಂಕ curl_dbg_getaddrinfo(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *service,
                         ಸ್ಥಿರಾಂಕ ರಚನೆ addrinfo *hints,
                         ರಚನೆ addrinfo **ಫಲಿತಾಂಶ,
                         ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LWIPSOCK
  ಪೂರ್ಣಾಂಕ res = lwip_getaddrinfo(hostname, service, hints, ಫಲಿತಾಂಶ);
#ಅಥವಾಆದರೆ defined(USE_FAKE_GETADDRINFO)
  ಪೂರ್ಣಾಂಕ res;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *env = getenv("CURL_DNS_SERVER");
  ಆದರೆ(env)
    res = r_getaddrinfo(hostname, service, hints, ಫಲಿತಾಂಶ);
  ಇಲ್ಲದಿದ್ದರೆ
    /* !checksrc! disable BANNEDFUNC 1 */
    res = getaddrinfo(hostname, service, hints, ಫಲಿತಾಂಶ);
#ಇಲ್ಲದಿದ್ದರೆ
  /* !checksrc! disable BANNEDFUNC 1 */
  ಪೂರ್ಣಾಂಕ res = getaddrinfo(hostname, service, hints, ಫಲಿತಾಂಶ);
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(res == 0)
    /* success */
    curl_dbg_log("ADDR %s:%d getaddrinfo() = %p\n", ಮೂಲ, ಸಾಲು,
                 (ಶೂನ್ಯ *)*ಫಲಿತಾಂಶ);
  ಇಲ್ಲದಿದ್ದರೆ
    curl_dbg_log("ADDR %s:%d getaddrinfo() failed\n", ಮೂಲ, ಸಾಲು);
  ಹಿಂತಿರುಗಿ res;
}
#ಆದರೆ_ಅಂತ್ಯ /* CURL_MEMDEBUG && HAVE_GETADDRINFO */

#ಆದರೆ defined(HAVE_GETADDRINFO) && defined(USE_RESOLVE_ON_IPS)
/*
 * Works around the sin6_port is always zero bug on iOS 9.3.2 and macOS
 * 10.11.5.
 */
ಶೂನ್ಯ Curl_addrinfo_ಗಣ_ಪೋರ್ಟ್(ರಚನೆ Curl_addrinfo *addrinfo, ಪೂರ್ಣಾಂಕ ಪೋರ್ಟ್)
{
  ರಚನೆ Curl_addrinfo *ca;
  ರಚನೆ sockaddr_in *addr;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ರಚನೆ sockaddr_in6 *addr6;
#ಆದರೆ_ಅಂತ್ಯ
  ಗಾಗಿ(ca = addrinfo; ca; ca = ca->ai_ಮುಂದಿನ) {
    ಆಯ್ಕೆ(ca->ai_family) {
    ಸ್ಥಿತಿ AF_INET:
      addr = (ಶೂನ್ಯ *)ca->ai_addr; /* storage area for this info */
      addr->sin_port = htons((ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)ಪೋರ್ಟ್);
      ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
    ಸ್ಥಿತಿ AF_INET6:
      addr6 = (ಶೂನ್ಯ *)ca->ai_addr; /* storage area for this info */
      addr6->sin6_port = htons((ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)ಪೋರ್ಟ್);
      ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
    }
  }
}
#ಆದರೆ_ಅಂತ್ಯ
